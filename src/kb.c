/*
 * kb.c - parrot0's logic engine (gen4-gen11).
 *
 * Grown one generation at a time into a small Prolog-like core:
 *   - ground facts + closed-world query            (gen4)
 *   - unification / variable queries (kb_match)    (gen5)
 *   - definite rules + backward chaining           (gen6)
 *   - induction of rules from facts (kb_induce)    (gen7)
 *   - human-readable persistence + provenance      (gen9)
 *   - retraction / correction (kb_retract)         (gen10)
 *   - explicit negative ground facts                (gen17)
 *   - n-ary relations + multi-goal rules + general SLD resolution with
 *     backtracking and standardize-apart           (gen11)
 * Everything is held in RAM (DESIGN.md D1); the file format is transparent
 * text so knowledge stays inspectable, diffable and hand-editable.
 */
#include "kb.h"

#include <errno.h>
#include <ctype.h>
#include <dirent.h>
#include <limits.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>

/* KB_MAX_BODY is declared in kb.h (part of kb_assert_rule_n's contract). */
#define KB_MAX_GOALS 64 /* resolvent size ceiling                      */
#define KB_MAX_DEPTH 64 /* resolution recursion guard (cyclic rules)   */
/* Work ceiling for ONE resolution (gen382). Depth alone does not bound cost: a
 * branching rule set explores exponentially many derivations inside depth 64.
 * Measured headroom: the whole .p0t suite peaks four orders of magnitude below
 * this, so hitting it means the knowledge is pathological, not merely large. */
#define KB_MAX_STEPS 500000UL
/* gen382o — bindings per substitution. Raised from 128 because a substitution is
 * a TRAIL: a deterministic fold like count_list/2 leaves every intermediate
 * binding behind, so folding a list of N costs ~4N slots and two folds in one
 * conjunction cost ~8N. At 128 that put the ceiling at a cohort of ~15, and the
 * fifteen game experts sat exactly on it — the sibling detector of
 * question-emergence.md silently lost every attribute with FULL support while
 * keeping the sparse ones, which reads as "nothing is expected here". The
 * overflow is now also reported (see `overflow` in Subst): running out of trail
 * is incompleteness, never a proof of absence.
 * The variable side of a binding is capped separately: names are short, and
 * paying a full term for each halved the trail for nothing. */
#define KB_MAX_BIND  384
#define KB_VAR_LEN   96 /* a renamed variable name: $Name_<frame>       */
#define KB_PROOF_LEN 480/* max length of a rendered proof string       */
#define KB_PROOF_PG  16 /* goals tracked while building an explanation */
/* One serialized rule can contain a head plus KB_MAX_BODY goals, each with a
 * predicate and KB_MAX_ARGS full-size arguments. Keep transport allocation
 * derived from those representation limits: a missing terminator must not turn
 * kb_load into an unbounded-memory sink. The small suffix budget covers commas,
 * parens, `:-`, `naf(...)`, and whitespace. */
#define KB_CLAUSE_MAX (((KB_MAX_BODY + 1) * (KB_MAX_ARGS + 1) * KB_TERM_LEN) + 256)

/* A term: predicate + args. An arg is a constant (lowercase atom) or a
 * variable (starts uppercase or '_'). Facts are ground terms; rule heads and
 * body goals may contain variables. */
typedef struct {
    char   pred[KB_TERM_LEN];
    size_t argc;
    char   args[KB_MAX_ARGS][KB_TERM_LEN];
    int    neg; /* U6: a BODY goal marked negation-as-failure (naf(G)). 0 for
                 * facts, heads, and ordinary positive goals. */
} Term;

/* gen401: copiare un TERMINE costava 2,6 KB per un termine da cinquanta byte.
 *
 * Stessa specie del difetto della sostituzione, un livello sotto. `Term` porta
 * `pred` piu' quattro argomenti da KB_TERM_LEN ciascuno: la copia per valore
 * muove sempre 2,6 KB, e il risolvente viene ricopiato a OGNI regola tentata,
 * per ogni goal rimasto. Con il profiler si vedeva che un passo di
 * `turn_response` costava venti volte un passo di `segment_role` a parita' di
 * numero: la differenza era quanta memoria quei passi muovevano.
 *
 * Si copia il PREFISSO VIVO — la stringa fino al terminatore, e solo gli
 * argomenti che `argc` dichiara. Il resto del buffer non viene mai letto da
 * nessuno: ogni lettore si ferma al NUL. Semantica identica, limiti identici. */
static void term_copy(Term *dst, const Term *src) {
    dst->argc = src->argc;
    dst->neg  = src->neg;
    memcpy(dst->pred, src->pred, strlen(src->pred) + 1);
    for (size_t i = 0; i < src->argc && i < KB_MAX_ARGS; i++)
        memcpy(dst->args[i], src->args[i], strlen(src->args[i]) + 1);
}


typedef struct {
    char   pred[KB_TERM_LEN];
    size_t argc;
    char   args[KB_MAX_ARGS][KB_TERM_LEN];
    int    origin;
    /* gen435 — QUESTO FATTO HA MAI FATTO QUALCOSA?
     *
     * Un byte per fatto, scritto solo quando l'audit e' acceso. Serve a una
     * domanda che parrot0 non sapeva porsi: «quali cose che dico di sapere non
     * hanno mai unificato con niente?». I sette difetti del gen427-432 erano
     * tutti di quella specie — conoscenza dichiarata che non poteva funzionare —
     * e nessuno di loro si e' mai lamentato. */
    unsigned char used;
} Fact;

/* A definite rule  head :- body[0], body[1], ...  (nbody >= 1). */
typedef struct {
    Term   head;
    Term   body[KB_MAX_BODY];
    size_t nbody;
    int    origin;
} Rule;

typedef struct {
    uint64_t hash;
    size_t index_plus_one;
} FactIndexEntry;

/* Per-PREDICATE census (gen382).
 *
 * The fact index answers "is this exact fact stored?" in O(1); nothing answered
 * "does this predicate exist at all?" without walking every fact. Every ground
 * lookup therefore paid a full scan of the KB — including the overwhelmingly
 * common MISS, where the predicate is not in the KB at all. That cost is
 * invisible while the KB is small and becomes quadratic as it grows: the boot
 * pass kb_derive_part_of asks is_model_pred() once per fact, and each ask
 * scanned every fact (13k x 15k on the gen380/381 KB, ~2s on one turn).
 *
 * The census is a hash from predicate name to its fact candidates and rule-head
 * candidates.  It also counts how many facts are NON-GROUND (contain a
 * variable), which is exactly what kb_query needs to decide whether a ground
 * lookup can be settled by the hash index alone. It is pure mechanism: it
 * stores no vocabulary and no domain knowledge, only the structural clauses
 * the KB already holds. */
typedef struct {
    char   pred[KB_TERM_LEN];
    size_t nfacts;      /* stored facts with this predicate                  */
    size_t nnonground;  /* ... of which carry a variable in some argument    */
    size_t *idx;        /* their positions in kb->facts, in insertion order  */
    size_t  idx_cap;
    size_t nrules;      /* rules whose head has this predicate               */
    size_t *ridx;       /* their positions in kb->rules, in insertion order  */
    size_t  ridx_cap;
} PredStat;

/* ── LA MAPPA DI SALVATAGGIO (save-map), IN RAM ───────────────────────────────
 *
 * Dove va a finire un fatto nuovo. La coordinata e' (predicato, primo
 * argomento) e la risposta e' (file, riga): il posto dove i suoi parenti stanno
 * gia', cosi' che la conoscenza nuova si attacchi a quella vecchia invece di
 * accumularsi in un deposito indistinto.
 *
 * SI COSTRUISCE CARICANDO, non rileggendo (F., gen411). Prima il salvataggio
 * riapriva e riparsava l'intero albero ogni volta — una seconda lettura della
 * KB, con un secondo parser piu' rozzo del vero, che e' il modo tipico di far
 * divergere due letture della stessa cosa. Ma il caricatore la posizione ce
 * l'ha gia' in mano: sa in che file e' e a che riga la clausola si chiude. La
 * mappa e' quel dato, tenuto invece che buttato.
 *
 * E' una MAPPA, non un elenco: una chiave, una posizione. Se la stessa chiave
 * torna, la posizione si aggiorna — vince l'ultima vista — invece di aggiungere
 * una riga. Dopo un inserimento la mappa NON si corregge: le righe sotto sono
 * slittate di uno, ma un fatto instradato «una riga piu' in la'» resta accanto
 * ai suoi parenti, ed e' l'unica cosa che conta. L'esattezza dell'ordine non e'
 * un obiettivo; la prossimita' lo e'.
 *
 * Il percorso del file si scrive UNA volta e le voci ne tengono l'indice: i file
 * sono cento, i fatti settemila. */
#define SM_PRED 64
#define SM_ARG  192

typedef struct { char *key; int file; int line; } SmSlot;

typedef struct {
    SmSlot *slots; size_t n, cap;      /* hash aperto, cap potenza di due */
    char  **files; size_t nfiles, fcap;
} SaveMap;

struct KB {
    Fact  *facts;
    size_t n;
    size_t cap;
    FactIndexEntry *fact_index;
    size_t fact_index_cap;
    Fact  *neg;
    size_t nn;
    size_t ncap;
    FactIndexEntry *neg_index;
    size_t neg_index_cap;
    Rule  *rules;
    size_t nr;
    size_t rcap;
    int    origin; /* provenance tag for newly asserted clauses */
    /* gen382 — what the last resolution cost, and whether a guard stopped it.
     * Kept on the KB so any host can ask after the fact; see kb_inference_report. */
    unsigned long infer_steps;
    int           infer_budget_hit;
    int           infer_loops_cut;
    char          infer_goal[KB_TERM_LEN];

    PredStat *pred_stats;      /* the census; NULL = unavailable, scan instead */
    size_t    pred_stats_cap;  /* power of two, open addressing               */
    size_t    pred_stats_n;    /* distinct predicates recorded                */
    int       pred_stats_dirty;/* a removal happened: rebuild before reading  */

    /* gen400: il profiler, dietro un flag. Spento non costa nulla. */
    int           prof_on;
    size_t        prof_calls;
    unsigned long prof_steps;
    double        prof_ms;
    size_t        prof_rebuilds;
    unsigned long prof_visits;
    size_t        prof_scans;
    struct timespec prof_t0;
    KbProfileRow  prof_top[64];
    size_t        prof_ntop;

    SaveMap smap;              /* dove abita ogni fatto: vedi SaveMap sopra */

    /* gen422 — LA FIRMA DEL FLUSSO DI INFERENZA (F.).
     *
     * Un CRC del RAGIONAMENTO, non della risposta: due turni che percorrono la
     * stessa strada portano la stessa firma anche se dicono parole diverse, e
     * due turni che dicono la stessa cosa per vie diverse no.
     *
     * E' l'XOR degli hash dei predicati risolti nel turno, ognuno preso UNA
     * VOLTA. L'XOR e' la scelta giusta perche' e' insensibile all'ordine — la
     * stessa strada percorsa in ordine diverso e' la stessa strada — ma per la
     * stessa ragione un predicato contato due volte si cancellerebbe: da qui
     * l'insieme dei gia' visti. */
    int           audit_on;   /* gen435: si segna quali fatti unificano */
    unsigned long fp_acc;
    unsigned long fp_seen[128];
    /* gen433 — I NOMI, non solo le impronte, e SOLO col profilo acceso.
     *
     * La firma basta a dire «due turni hanno fatto la stessa strada»; a un
     * supervisore che ispeziona serve sapere QUALE strada. I nomi costano
     * memoria e una copia per predicato, quindi si raccolgono soltanto quando
     * qualcuno sta guardando — la stessa disciplina del profiler: spento non
     * costa nulla. */
    char          fp_name[128][48];
    size_t        fp_n;
};

void kb_profile_set(KB *kb, int on) { if (kb) kb->prof_on = on ? 1 : 0; }
int  kb_profile_on(const KB *kb)    { return kb ? kb->prof_on : 0; }
unsigned long kb_profile_steps(const KB *kb) { return kb ? kb->prof_steps : 0; }
double kb_profile_ms(const KB *kb) { return kb ? kb->prof_ms : 0.0; }

/* gen400b: il primo giro del profiler ha misurato 960 passi per 482 ms — cioe'
 * mezzo millisecondo a passo, che per un passo di risoluzione e' assurdo. La
 * conclusione e' che il tempo NON sta dove sta il lavoro logico, e un profiler
 * che conta solo i passi non puo' dirlo. Da qui il cronometro per query. */
static void kb_prof_start(KB *kb) {
    if (kb && kb->prof_on) timespec_get(&kb->prof_t0, TIME_UTC);
}
static double kb_prof_elapsed(const KB *kb) {
    if (!kb || !kb->prof_on) return 0.0;
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    return (now.tv_sec - kb->prof_t0.tv_sec) * 1000.0
         + (now.tv_nsec - kb->prof_t0.tv_nsec) / 1000000.0;
}
size_t kb_profile_calls(const KB *kb)        { return kb ? kb->prof_calls : 0; }
size_t kb_profile_rebuilds(const KB *kb)     { return kb ? kb->prof_rebuilds : 0; }
unsigned long kb_profile_visits(const KB *kb) { return kb ? kb->prof_visits : 0; }
size_t kb_profile_scans(const KB *kb)         { return kb ? kb->prof_scans : 0; }

void kb_profile_reset(KB *kb) {
    if (!kb) return;
    kb->prof_calls = 0;
    kb->prof_steps = 0;
    kb->prof_ms = 0.0;
    kb->prof_rebuilds = 0;
    kb->prof_visits = 0;
    kb->prof_scans = 0;
    kb->prof_ntop = 0;
}

size_t kb_profile_top(const KB *kb, KbProfileRow *out, size_t max) {
    if (!kb || !out || max == 0) return 0;
    size_t n = kb->prof_ntop < max ? kb->prof_ntop : max;
    /* ordinato per TEMPO decrescente: e' la colonna che decide dove guardare,
     * e il primo giro ha mostrato che non coincide con quella dei passi. */
    for (size_t i = 0; i < kb->prof_ntop; i++) {
        KbProfileRow row = kb->prof_top[i];
        size_t place = 0;
        while (place < n && place < max && out[place].ms >= row.ms &&
               place < i) place++;
        if (place >= max) continue;
        for (size_t j = (i < max ? i : max - 1); j > place; j--) out[j] = out[j - 1];
        out[place] = row;
    }
    return n;
}

KB *kb_create(void) {
    KB *kb = calloc(1, sizeof *kb);
    if (kb) kb->origin = KB_SESSION; /* default provenance */
    return kb; /* may be NULL; caller handles it */
}

/* gen382i: l'origine corrente, per poterla ripristinare dopo un ragionamento
 * ipotetico condotto sulla KB vera. */
int kb_origin(const KB *kb) { return kb ? kb->origin : 0; }

void kb_set_origin(KB *kb, int origin) {
    if (kb) kb->origin = origin;
}

void kb_destroy(KB *kb) {
    if (!kb) return;
    free(kb->facts);
    free(kb->fact_index);
    free(kb->neg);
    free(kb->neg_index);
    free(kb->rules);
    if (kb->pred_stats)
        for (size_t i = 0; i < kb->pred_stats_cap; i++) {
            free(kb->pred_stats[i].idx);
            free(kb->pred_stats[i].ridx);
        }
    free(kb->pred_stats);
    for (size_t i = 0; i < kb->smap.cap; i++) free(kb->smap.slots[i].key);
    free(kb->smap.slots);
    for (size_t i = 0; i < kb->smap.nfiles; i++) free(kb->smap.files[i]);
    free(kb->smap.files);
    free(kb);
}

/* ----------------------------------------------------------------------------
 * terms, facts
 * ------------------------------------------------------------------------- */

static int term_ok(const char *s) {
    if (!s || s[0] == '\0') return 0;
    return strlen(s) < KB_TERM_LEN;
}

/* A variable is marked EXPLICITLY: a leading '$' (named, gen280/U1b) or a leading
 * '_' (anonymous). These are dedicated sigils that real data never uses, so an
 * uppercase-initial token — "Madrid", the char "M" — is an ordinary CONSTANT.
 * gen284 removed the legacy "uppercase = variable" rule (end of dual-accept, F.'s
 * request): case is now pure content, never a variable signal. */
static int is_var(const char *s) {
    return s && (s[0] == '$' || s[0] == '_');
}

static int fact_make(Fact *f, const char *pred, const char *const *args,
                     size_t argc) {
    if (!term_ok(pred)) return 0;
    memset(f, 0, sizeof *f);
    strcpy(f->pred, pred);
    f->argc = argc;
    for (size_t i = 0; i < argc; i++) {
        if (!term_ok(args[i])) return 0;
        strcpy(f->args[i], args[i]);
    }
    return 1;
}

static int fact_eq(const Fact *a, const Fact *b) {
    if (a->argc != b->argc) return 0;
    if (strcmp(a->pred, b->pred) != 0) return 0;
    for (size_t i = 0; i < a->argc; i++) {
        if (strcmp(a->args[i], b->args[i]) != 0) return 0;
    }
    return 1;
}

static uint64_t fact_hash(const Fact *f) {
    uint64_t h = UINT64_C(1469598103934665603);
    const unsigned char *p = (const unsigned char *)f->pred;
    while (*p) { h ^= *p++; h *= UINT64_C(1099511628211); }
    h ^= (uint64_t)f->argc;
    h *= UINT64_C(1099511628211);
    for (size_t i = 0; i < f->argc; i++) {
        p = (const unsigned char *)f->args[i];
        while (*p) { h ^= *p++; h *= UINT64_C(1099511628211); }
        h ^= UINT64_C(255);
        h *= UINT64_C(1099511628211);
    }
    return h;
}

static const Fact *fact_find(const Fact *facts, size_t n, const Fact *needle) {
    for (size_t i = 0; i < n; i++) {
        if (fact_eq(&facts[i], needle)) return &facts[i];
    }
    return NULL;
}

static const Fact *fact_index_find(const FactIndexEntry *index, size_t cap,
                                   const Fact *facts, size_t n,
                                   const Fact *needle) {
    if (!index || cap == 0) return fact_find(facts, n, needle);
    uint64_t hash = fact_hash(needle);
    size_t pos = (size_t)hash & (cap - 1);
    for (size_t probes = 0; probes < cap; probes++) {
        const FactIndexEntry *e = &index[pos];
        if (e->index_plus_one == 0) return NULL;
        size_t fi = e->index_plus_one - 1;
        if (e->hash == hash && fi < n && fact_eq(&facts[fi], needle))
            return &facts[fi];
        pos = (pos + 1) & (cap - 1);
    }
    return NULL;
}

static void fact_index_insert(FactIndexEntry *index, size_t cap,
                              const Fact *facts, size_t fi) {
    uint64_t hash = fact_hash(&facts[fi]);
    size_t pos = (size_t)hash & (cap - 1);
    while (index[pos].index_plus_one != 0)
        pos = (pos + 1) & (cap - 1);
    index[pos].hash = hash;
    index[pos].index_plus_one = fi + 1;
}

static int fact_index_rebuild(FactIndexEntry **index, size_t *cap,
                              const Fact *facts, size_t n,
                              size_t needed) {
    size_t next = 16;
    if (needed < n) needed = n;
    while (next / 2 < needed) {
        if (next > SIZE_MAX / 2) return 0;
        next *= 2;
    }
    FactIndexEntry *fresh = calloc(next, sizeof *fresh);
    if (!fresh) return 0;
    for (size_t i = 0; i < n; i++)
        fact_index_insert(fresh, next, facts, i);
    free(*index);
    *index = fresh;
    *cap = next;
    return 1;
}

static void fact_index_rebuild_after_remove(FactIndexEntry **index, size_t *cap,
                                            const Fact *facts, size_t n) {
    if (fact_index_rebuild(index, cap, facts, n, n)) return;
    free(*index);
    *index = NULL;
    *cap = 0;
}

/* ---- the per-predicate census (see PredStat) ---------------------------- */

static int term_contains_var(const char *s, int depth);   /* fwd */

static uint64_t pred_hash(const char *pred) {
    uint64_t h = UINT64_C(1469598103934665603);
    for (const unsigned char *p = (const unsigned char *)pred; *p; p++) {
        h ^= *p; h *= UINT64_C(1099511628211);
    }
    return h;
}

/* Slot for `pred`, or NULL. With `create`, an empty slot is claimed and zeroed;
 * the table is never full because pred_stats_note() grows it at 70% load. */
static PredStat *pred_stat_slot(KB *kb, const char *pred, int create) {
    if (!kb->pred_stats || !kb->pred_stats_cap) return NULL;
    size_t mask = kb->pred_stats_cap - 1;
    size_t pos = (size_t)pred_hash(pred) & mask;
    for (size_t probes = 0; probes <= mask; probes++) {
        PredStat *e = &kb->pred_stats[pos];
        if (!e->pred[0]) {
            if (!create) return NULL;
            snprintf(e->pred, sizeof e->pred, "%s", pred);
            e->nfacts = e->nnonground = 0;
            kb->pred_stats_n++;
            return e;
        }
        if (strcmp(e->pred, pred) == 0) return e;
        pos = (pos + 1) & mask;
    }
    return NULL;
}

static int fact_is_nonground(const Fact *f) {
    for (size_t a = 0; a < f->argc; a++)
        if (term_contains_var(f->args[a], 0)) return 1;
    return 0;
}

static void pred_stats_drop(KB *kb) {
    if (kb->pred_stats)
        for (size_t i = 0; i < kb->pred_stats_cap; i++) {
            free(kb->pred_stats[i].idx);
            free(kb->pred_stats[i].ridx);
        }
    free(kb->pred_stats);
    kb->pred_stats = NULL;
    kb->pred_stats_cap = kb->pred_stats_n = 0;
    kb->pred_stats_dirty = 1;
}

/* Claim a predicate census entry, growing/rehashing the table when needed.
 * Facts and rule heads share this one structural index: both are candidates for
 * exactly one goal predicate, and keeping separate hash tables would duplicate
 * the same key space. */
static PredStat *pred_stats_claim(KB *kb, const char *pred) {
    if (kb->pred_stats_dirty) return NULL;        /* a rebuild will recount */
    size_t needed = kb->pred_stats_n + 1;
    if (!kb->pred_stats || needed * 10 >= kb->pred_stats_cap * 7) {
        size_t next = kb->pred_stats_cap ? kb->pred_stats_cap * 2 : 64;
        while (next / 2 < needed) {
            if (next > SIZE_MAX / 2) { pred_stats_drop(kb); return NULL; }
            next *= 2;
        }
        PredStat *fresh = calloc(next, sizeof *fresh);
        if (!fresh) { pred_stats_drop(kb); return NULL; }
        PredStat *old = kb->pred_stats; size_t ocap = kb->pred_stats_cap;
        kb->pred_stats = fresh; kb->pred_stats_cap = next; kb->pred_stats_n = 0;
        for (size_t i = 0; i < ocap; i++) {       /* rehash, bucket and all */
            if (!old[i].pred[0]) continue;
            PredStat *e = pred_stat_slot(kb, old[i].pred, 1);
            if (e) *e = old[i];
            else {
                free(old[i].idx);
                free(old[i].ridx);
            }
        }
        free(old);
    }
    PredStat *e = pred_stat_slot(kb, pred, 1);
    if (!e) pred_stats_drop(kb);
    return e;
}

/* Record one stored fact (at position `fi`) in the census. On allocation
 * failure the census is dropped and every reader falls back to the historical
 * full scan — slower, never wrong. */
static void pred_stats_note(KB *kb, size_t fi) {
    if (kb->pred_stats_dirty) return;
    const Fact *f = &kb->facts[fi];
    PredStat *e = pred_stats_claim(kb, f->pred);
    if (!e) return;
    if (e->nfacts == e->idx_cap) {
        size_t next = e->idx_cap ? e->idx_cap * 2 : 4;
        size_t *grown = realloc(e->idx, next * sizeof *grown);
        if (!grown) { pred_stats_drop(kb); return; }
        e->idx = grown; e->idx_cap = next;
    }
    e->idx[e->nfacts++] = fi;
    if (fact_is_nonground(f)) e->nnonground++;
}

/* Rule-head twin of pred_stats_note(). The stored positions preserve clause
 * insertion order, so indexing changes candidate discovery cost but not SLD
 * order, backtracking, or the first solution selected. */
static void pred_stats_note_rule(KB *kb, size_t ri) {
    if (kb->pred_stats_dirty) return;
    const Rule *r = &kb->rules[ri];
    PredStat *e = pred_stats_claim(kb, r->head.pred);
    if (!e) return;
    if (e->nrules == e->ridx_cap) {
        size_t next = e->ridx_cap ? e->ridx_cap * 2 : 4;
        size_t *grown = realloc(e->ridx, next * sizeof *grown);
        if (!grown) { pred_stats_drop(kb); return; }
        e->ridx = grown;
        e->ridx_cap = next;
    }
    e->ridx[e->nrules++] = ri;
}

/* Removals compact the fact array, so every stored position can shift. Rather
 * than patch the buckets per path we mark the census stale and recount once,
 * lazily, at the next read. Retraction is rare next to lookup; this keeps the
 * hot path free of bookkeeping. */
static void pred_stats_invalidate(KB *kb) { if (kb) kb->pred_stats_dirty = 1; }

static void pred_stats_rebuild(KB *kb) {
    if (kb->prof_on) kb->prof_rebuilds++;
    if (kb->pred_stats)
        for (size_t i = 0; i < kb->pred_stats_cap; i++) {
            kb->pred_stats[i].pred[0] = '\0';
            kb->pred_stats[i].nfacts = kb->pred_stats[i].nnonground = 0;
            kb->pred_stats[i].nrules = 0;
            free(kb->pred_stats[i].idx);
            kb->pred_stats[i].idx = NULL;
            kb->pred_stats[i].idx_cap = 0;
            free(kb->pred_stats[i].ridx);
            kb->pred_stats[i].ridx = NULL;
            kb->pred_stats[i].ridx_cap = 0;
        }
    kb->pred_stats_n = 0;
    kb->pred_stats_dirty = 0;
    for (size_t i = 0; i < kb->n; i++) {
        pred_stats_note(kb, i);
        if (kb->pred_stats_dirty) return;         /* gave up: readers scan */
    }
    for (size_t i = 0; i < kb->nr; i++) {
        pred_stats_note_rule(kb, i);
        if (kb->pred_stats_dirty) return;
    }
}

/* The census entry for `pred`, or NULL when there is none — which, when the
 * census is live, means "no stored fact uses this predicate". `census_live`
 * tells the two cases apart, since NULL also means "census unavailable". */
static const PredStat *pred_stats_get(KB *kb, const char *pred, int *census_live) {
    if (kb->pred_stats_dirty) pred_stats_rebuild(kb);
    *census_live = (kb->pred_stats != NULL) && !kb->pred_stats_dirty;
    if (!*census_live) return NULL;
    return pred_stat_slot(kb, pred, 0);
}

/* The set of facts a reader must visit to see everything about `pred`.
 *
 *   live = 1 -> exactly idx[0..n), the census bucket
 *   live = 0 -> the census is unavailable; visit every fact as before
 *
 * The KB is logically const for readers, so the lazy recount casts it away: the
 * census is a cache OF the facts, never a change TO them. */
/* gen401: `nonground` dice se il bucket contiene ANCHE una sola clausola
 * unitaria con variabili. Il censimento lo conta gia' (`nnonground`), e saperlo
 * risparmia a ogni fatto una scansione dei propri argomenti: quando nessuna
 * clausola del predicato ha variabili — il caso normale, e nella KB di parrot0
 * la stragrande maggioranza — la domanda non va posta per ottantamila volte. */
typedef struct { const size_t *idx; size_t n; int live; int nonground; } PredBucket;

static PredBucket pred_bucket(const KB *kb, const char *pred) {
    PredBucket b = { NULL, 0, 0, 1 };
    int live = 0;
    const PredStat *ps = pred_stats_get((KB *)kb, pred, &live);
    if (!live) return b;
    b.live = 1;
    if (ps) { b.idx = ps->idx; b.n = ps->nfacts; b.nonground = ps->nnonground > 0; }
    return b;
}

/* The rule-head candidates for `pred`, with the same fallback contract as
 * pred_bucket(). */
static PredBucket rule_bucket(const KB *kb, const char *pred) {
    PredBucket b = { NULL, 0, 0, 0 };
    int live = 0;
    const PredStat *ps = pred_stats_get((KB *)kb, pred, &live);
    if (!live) return b;
    b.live = 1;
    if (ps) { b.idx = ps->ridx; b.n = ps->nrules; }
    return b;
}

/* Position of the `vi`-th fact to visit for this bucket. */
#define PRED_AT(bk, vi) ((bk).live ? (bk).idx[(vi)] : (vi))
#define PRED_VISITS(bk, kb) ((bk).live ? (bk).n : (kb)->n)

static const Fact *kb_find(const KB *kb, const Fact *needle) {
    return fact_index_find(kb->fact_index, kb->fact_index_cap,
                           kb->facts, kb->n, needle);
}

static const Fact *kb_find_neg(const KB *kb, const Fact *needle) {
    return fact_index_find(kb->neg_index, kb->neg_index_cap,
                           kb->neg, kb->nn, needle);
}

static int fact_remove(Fact *facts, size_t *n, const Fact *needle) {
    for (size_t i = 0; i < *n; i++) {
        if (fact_eq(&facts[i], needle)) {
            memmove(&facts[i], &facts[i + 1], (*n - i - 1) * sizeof *facts);
            (*n)--;
            return 1;
        }
    }
    return 0;
}

static int fact_remove_origin(Fact *facts, size_t *n, const Fact *needle,
                              int origin) {
    for (size_t i = 0; i < *n; i++) {
        if (fact_eq(&facts[i], needle) && facts[i].origin == origin) {
            memmove(&facts[i], &facts[i + 1], (*n - i - 1) * sizeof *facts);
            (*n)--;
            return 1;
        }
    }
    return 0;
}

static int fact_append(Fact **facts, size_t *n, size_t *cap, const Fact *f) {
    if (*n == *cap) {
        size_t next = *cap ? *cap * 2 : 16;
        Fact *grown = realloc(*facts, next * sizeof *grown);
        if (!grown) return 0;
        *facts = grown;
        *cap = next;
    }
    (*facts)[(*n)++] = *f;
    return 1;
}

static int fact_append_indexed(Fact **facts, size_t *n, size_t *cap,
                               FactIndexEntry **index, size_t *index_cap,
                               const Fact *f) {
    size_t needed = *n + 1;
    if (!*index || needed * 10 >= *index_cap * 7) {
        if (!fact_index_rebuild(index, index_cap, *facts, *n, needed)) {
            free(*index);
            *index = NULL;
            *index_cap = 0;
        }
    }
    if (!fact_append(facts, n, cap, f)) return 0;
    if (*index)
        fact_index_insert(*index, *index_cap, *facts, *n - 1);
    return 1;
}

int kb_assert(KB *kb, const char *pred, const char *const *args, size_t argc) {
    if (!kb || argc > KB_MAX_ARGS) return 0;

    Fact f;
    if (!fact_make(&f, pred, args, argc)) return 0;

    if (fact_remove_origin(kb->neg, &kb->nn, &f, kb->origin))
        fact_index_rebuild_after_remove(&kb->neg_index, &kb->neg_index_cap,
                                        kb->neg, kb->nn);
    if (kb_find(kb, &f)) return 1; /* already known — idempotent */
    f.origin = kb->origin;
    if (!fact_append_indexed(&kb->facts, &kb->n, &kb->cap,
                             &kb->fact_index, &kb->fact_index_cap, &f)) return 0;
    pred_stats_note(kb, kb->n - 1);
    return 1;
}

int kb_retract(KB *kb, const char *pred, const char *const *args, size_t argc) {
    if (!kb || argc > KB_MAX_ARGS) return 0;
    Fact f;
    if (!fact_make(&f, pred, args, argc)) return 0;
    int removed = fact_remove(kb->facts, &kb->n, &f);
    if (removed) {
        fact_index_rebuild_after_remove(&kb->fact_index, &kb->fact_index_cap,
                                        kb->facts, kb->n);
        pred_stats_invalidate(kb);
    }
    return removed;
}

size_t kb_retract_match(KB *kb, const char *pred,
                        const char *const *args, size_t argc) {
    if (!kb || !term_ok(pred) || argc > KB_MAX_ARGS || (argc && !args)) return 0;
    for (size_t a = 0; a < argc; a++)
        if (args[a] && !term_ok(args[a])) return 0;
    size_t removed = 0, w = 0;
    for (size_t i = 0; i < kb->n; i++) {
        Fact *f = &kb->facts[i];
        int match = f->argc == argc && strcmp(f->pred, pred) == 0;
        for (size_t a = 0; a < argc && match; a++)
            if (args[a] && strcmp(args[a], f->args[a]) != 0) match = 0;
        if (match) { removed++; continue; }
        if (w != i) kb->facts[w] = kb->facts[i];
        w++;
    }
    kb->n = w;
    if (removed) {
        fact_index_rebuild_after_remove(&kb->fact_index, &kb->fact_index_cap,
                                        kb->facts, kb->n);
        pred_stats_invalidate(kb);
    }
    return removed;
}

size_t kb_retract_pred(KB *kb, const char *pred) {
    if (!kb || !pred || !*pred) return 0;
    size_t removed = 0, w = 0;
    for (size_t i = 0; i < kb->n; i++) {
        if (strcmp(kb->facts[i].pred, pred) == 0) { removed++; continue; }
        if (w != i) kb->facts[w] = kb->facts[i];
        w++;
    }
    kb->n = w;
    if (removed) {
        fact_index_rebuild_after_remove(&kb->fact_index, &kb->fact_index_cap,
                                        kb->facts, kb->n);
        pred_stats_invalidate(kb);
    }
    return removed;
}

size_t kb_retract_origin(KB *kb, int origin_mask) {
    if (!kb || !origin_mask) return 0;
    size_t removed = 0, w = 0;
    for (size_t i = 0; i < kb->n; i++) {
        if (kb->facts[i].origin & origin_mask) { removed++; continue; }
        if (w != i) kb->facts[w] = kb->facts[i];
        w++;
    }
    kb->n = w;
    if (removed) {
        fact_index_rebuild_after_remove(&kb->fact_index, &kb->fact_index_cap,
                                        kb->facts, kb->n);
        pred_stats_invalidate(kb);
    }

    /* gen382k — anche le REGOLE, e non e' un'estensione di comodo.
     *
     * Toglieva solo i fatti, e finche' le premesse ipotetiche vivevano in un
     * sandbox usa-e-getta non si notava: moriva tutto col sandbox. Nel momento in
     * cui una supposizione vive sulla KB VERA, "tutti i gatti sono pesci" e' una
     * REGOLA — e una regola sopravvissuta a un'ipotesi non e' un residuo, e'
     * conoscenza falsa che continua a dedurre. Una provenienza che si puo'
     * scrivere ma non ritirare per intero non e' una provenienza. */
    size_t rw = 0, rremoved = 0;
    for (size_t i = 0; i < kb->nr; i++) {
        if (kb->rules[i].origin & origin_mask) { rremoved++; continue; }
        if (rw != i) kb->rules[rw] = kb->rules[i];
        rw++;
    }
    kb->nr = rw;
    if (rremoved) pred_stats_invalidate(kb);
    return removed + rremoved;
}

int kb_query_origin(const KB *kb, int origin_mask, const char *pred,
                    const char *const *args, size_t argc) {
    if (!kb || !origin_mask || !term_ok(pred) || argc > KB_MAX_ARGS) return 0;
    if (argc && !args) return 0;
    for (size_t i = 0; i < argc; i++) if (!term_ok(args[i])) return 0;
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *f = &kb->facts[i];
        if (!(f->origin & origin_mask)) continue;
        if (f->argc != argc || strcmp(f->pred, pred) != 0) continue;
        size_t a = 0;
        for (; a < argc; a++)
            if (strcmp(args[a], f->args[a]) != 0) break;
        if (a == argc) return 1;
    }
    return 0;
}

int kb_assert_neg(KB *kb, const char *pred, const char *const *args,
                  size_t argc) {
    if (!kb || argc > KB_MAX_ARGS) return 0;

    Fact f;
    if (!fact_make(&f, pred, args, argc)) return 0;

    if (fact_remove_origin(kb->facts, &kb->n, &f, kb->origin)) {
        fact_index_rebuild_after_remove(&kb->fact_index, &kb->fact_index_cap,
                                        kb->facts, kb->n);
        pred_stats_invalidate(kb);
    }
    if (kb_find_neg(kb, &f)) return 1; /* already known false */
    f.origin = kb->origin;
    return fact_append_indexed(&kb->neg, &kb->nn, &kb->ncap,
                               &kb->neg_index, &kb->neg_index_cap, &f);
}

int kb_is_negated(const KB *kb, const char *pred, const char *const *args,
                  size_t argc) {
    if (!kb || argc > KB_MAX_ARGS) return 0;
    Fact f;
    if (!fact_make(&f, pred, args, argc)) return 0;
    return kb_find_neg(kb, &f) != NULL;
}

int kb_is_conflicted(const KB *kb, const char *pred,
                     const char *const *args, size_t argc) {
    if (!kb || argc > KB_MAX_ARGS) return 0;
    Fact f;
    if (!fact_make(&f, pred, args, argc)) return 0;
    return kb_find(kb, &f) != NULL && kb_find_neg(kb, &f) != NULL;
}

/* ----------------------------------------------------------------------------
 * rules
 * ------------------------------------------------------------------------- */

static int kb_add_rule(KB *kb, const Rule *r) {
    if (kb->nr == kb->rcap) {
        size_t cap = kb->rcap ? kb->rcap * 2 : 8;
        Rule *grown = realloc(kb->rules, cap * sizeof *grown);
        if (!grown) return 0;
        kb->rules = grown;
        kb->rcap = cap;
    }
    kb->rules[kb->nr++] = *r;
    pred_stats_note_rule(kb, kb->nr - 1);
    return 1;
}

/* True if the simple unary rule  head(X) :- body(X)  already exists. */
static int rule_exists(KB *kb, const char *head, const char *body) {
    for (size_t i = 0; i < kb->nr; i++) {
        const Rule *r = &kb->rules[i];
        if (r->nbody == 1 && r->head.argc == 1 && r->body[0].argc == 1 &&
            strcmp(r->head.pred, head) == 0 &&
            strcmp(r->body[0].pred, body) == 0) return 1;
    }
    return 0;
}

int kb_assert_rule(KB *kb, const char *head, const char *body) {
    if (!kb || !term_ok(head) || !term_ok(body)) return 0;
    if (rule_exists(kb, head, body)) return 1; /* idempotent */

    Rule r;
    memset(&r, 0, sizeof r);
    strcpy(r.head.pred, head);  r.head.argc = 1;     strcpy(r.head.args[0], "$X");
    r.nbody = 1;
    strcpy(r.body[0].pred, body); r.body[0].argc = 1; strcpy(r.body[0].args[0], "$X");
    r.origin = kb->origin;
    return kb_add_rule(kb, &r);
}

int kb_assert_rule_n(KB *kb, const char *head,
                     const char *const *bodies, size_t nbody) {
    if (!kb || !term_ok(head) || nbody == 0 || nbody > KB_MAX_BODY) return 0;
    for (size_t i = 0; i < nbody; i++)
        if (!term_ok(bodies[i])) return 0;
    if (nbody == 1) return kb_assert_rule(kb, head, bodies[0]); /* idempotent path */

    /* idempotent: an identical conjunctive rule is not duplicated */
    for (size_t r = 0; r < kb->nr; r++) {
        const Rule *R = &kb->rules[r];
        if (R->nbody != nbody || strcmp(R->head.pred, head) != 0) continue;
        int same = 1;
        for (size_t b = 0; b < nbody; b++)
            if (strcmp(R->body[b].pred, bodies[b]) != 0) { same = 0; break; }
        if (same) return 1;
    }

    Rule r;
    memset(&r, 0, sizeof r);
    strcpy(r.head.pred, head); r.head.argc = 1; strcpy(r.head.args[0], "$X");
    r.nbody = nbody;
    for (size_t b = 0; b < nbody; b++) {
        strcpy(r.body[b].pred, bodies[b]);
        r.body[b].argc = 1;
        strcpy(r.body[b].args[0], "$X");
    }
    r.origin = kb->origin;
    return kb_add_rule(kb, &r);
}

/* ----------------------------------------------------------------------------
 * unification + SLD resolution with backtracking (gen11)
 * ------------------------------------------------------------------------- */

#define KB_MAX_DIF 32  /* max deferred dif/2 constraints per branch */

typedef struct { char var[KB_VAR_LEN]; char val[KB_TERM_LEN]; } Bind;
typedef struct { char a[KB_TERM_LEN]; char b[KB_TERM_LEN]; } DifConstraint;
/* `overflow` points at the owning solver's incompleteness flag. A substitution is
 * copied by value all the way down a derivation, so the pointer travels with it
 * and an exhausted trail eight frames deep still reaches the top — where the
 * answer must become "I could not finish", not "there is nothing". */
typedef struct { Bind b[KB_MAX_BIND]; size_t n;
                 DifConstraint dif[KB_MAX_DIF]; size_t ndif;
                 int *overflow; } Subst;

/* gen401: copiare una sostituzione costava 266 KB A PASSO.
 *
 * `Subst` dimensiona i propri array sul CASO PEGGIORE — 384 binding da 608 byte
 * piu' 32 vincoli `dif` — ma una derivazione tipica ne usa una manciata. La
 * copia per valore (`*s2 = *s`) copiava sempre tutto, e il profiler `/debug` ha
 * misurato il conto: mezzo millisecondo per passo di risoluzione, dove il lavoro
 * logico era di microsecondi.
 *
 * Qui non cambia ne' la semantica ne' il limite: si copia soltanto il PREFISSO
 * VIVO, che e' esattamente cio' che il resto del codice legge (`s->n`,
 * `s->ndif`). Il caso peggiore resta identico, il caso normale diventa
 * proporzionale a quanto si e' davvero legato. */
static void subst_copy(Subst *dst, const Subst *src) {
    dst->n = src->n;
    dst->ndif = src->ndif;
    dst->overflow = src->overflow;
    for (size_t i = 0; i < src->n; i++) dst->b[i] = src->b[i];
    for (size_t i = 0; i < src->ndif; i++) dst->dif[i] = src->dif[i];
}


/* fwd: structural unification (U3) splits compound-term strings with parse_term,
 * defined further down with the .p0 loader. */
static int parse_term(const char *s, char *pred,
                      char args[][KB_TERM_LEN], size_t *argc);

/* U3: split s into functor+args IFF it is a WELL-FORMED compound term — a bare
 * atom functor (ident/$ chars, no spaces) immediately followed by balanced
 * parens ending the string. This is the guard that keeps natural-language values
 * that merely contain "(…)" — e.g. "the source (see note)" — as opaque atoms,
 * not accidental compounds. Used by every structural operation (unify, rename,
 * deep_resolve) so prose never gets re-parsed. */
static int split_compound(const char *s, char *functor,
                          char args[][KB_TERM_LEN], size_t *argc) {
    if (!s || !*s) return 0;
    const char *lp = strchr(s, '(');
    if (!lp || lp == s) return 0;
    for (const char *p = s; p < lp; p++)
        if (!(isalnum((unsigned char)*p) || *p == '_' || *p == '$')) return 0;
    size_t n = strlen(s);
    if (s[n - 1] != ')') return 0;
    int d = 0;                          /* only the FINAL ')' closes depth 0 */
    for (const char *p = lp; *p; p++) {
        if (*p == '(') d++;
        else if (*p == ')') { d--; if (d == 0 && p[1] != '\0') return 0; }
    }
    if (d != 0) return 0;
    return parse_term(s, functor, args, argc);
}

/* True only when unification can bind something inside this term.  A ground
 * compound such as keyword(int) is still safe for the exact-fact fast path;
 * s($X) and a top-level $X are not. */
static int term_contains_var(const char *s, int depth) {
    if (is_var(s)) return 1;
    if (depth > KB_MAX_DEPTH) return 0;
    char functor[KB_TERM_LEN], args[KB_MAX_ARGS][KB_TERM_LEN];
    size_t argc = 0;
    if (!split_compound(s, functor, args, &argc)) return 0;
    for (size_t i = 0; i < argc; i++)
        if (term_contains_var(args[i], depth + 1)) return 1;
    return 0;
}

/* Follow the binding chain to a constant or an unbound variable. */
static const char *resolve(const Subst *s, const char *name) {
    while (is_var(name)) {
        const char *next = NULL;
        for (size_t i = 0; i < s->n; i++) {
            if (strcmp(s->b[i].var, name) == 0) { next = s->b[i].val; break; }
        }
        if (!next) break;
        name = next;
    }
    return name;
}

/* U3: resolve a term to its ground form, substituting variables RECURSIVELY
 * inside compound structure — so $R bound to s($Z), $Z to z, renders "s(z)".
 * Depth-capped against cyclic bindings (X↦s(X)); on overflow it stops expanding
 * (honest truncation, never a loop). */
static void deep_resolve(const Subst *s, const char *t,
                         char *dst, size_t dstsz, int depth) {
    const char *r = resolve(s, t);
    if (depth > KB_MAX_DEPTH) { snprintf(dst, dstsz, "%s", r); return; }
    /* U9 (gen335+): chain through variable-to-variable bindings — $Q↦$Z↦"belgium"
     * must resolve to "belgium", not "$Z". Guard against cycles. */
    for (int vg = 0; vg < KB_MAX_DEPTH && is_var(r); vg++) {
        const char *nxt = resolve(s, r);
        if (strcmp(nxt, r) == 0) break;
        r = nxt;
    }
    char f[KB_TERM_LEN], args[KB_MAX_ARGS][KB_TERM_LEN];
    size_t n = 0;
    if (split_compound(r, f, args, &n)) {
        int off = snprintf(dst, dstsz, "%s(", f);
        for (size_t i = 0; i < n && off > 0 && (size_t)off < dstsz; i++) {
            char sub[KB_TERM_LEN];
            deep_resolve(s, args[i], sub, sizeof sub, depth + 1);
            off += snprintf(dst + off, dstsz - (size_t)off,
                            "%s%s", i ? ", " : "", sub);
        }
        if (off > 0 && (size_t)off < dstsz)
            snprintf(dst + off, dstsz - (size_t)off, ")");
    } else {
        snprintf(dst, dstsz, "%s", r);
    }
}

static int bind_add(Subst *s, const char *var, const char *val) {
    /* gen427 — UN VALORE PIU' LUNGO DEL SUO POSTO ESAURISCE, NON SFONDA.
     *
     * La copia era una `strcpy` in un campo di KB_TERM_LEN, e il valore puo'
     * arrivare da `chars/2`, che lavora fino a KB_CHARLIST_MAX (4096): la lista
     * di caratteri di una frase di sessanta lettere e' un termine di
     * cinquecento e passa, e il processo ABORTIVA con «buffer overflow
     * detected». Non era un caso di laboratorio — bastava che una regola
     * guardasse i caratteri del turno, che e' esattamente quello che le forme
     * dei letterali fanno.
     *
     * Trattarlo come esaurimento e' l'unica risposta onesta: la sostituzione
     * non puo' rappresentare quel legame, e `overflow` esiste apposta per dire
     * «non l'ho esplorato», che e' diverso da «e' falso». */
    if (s->n >= KB_MAX_BIND || strlen(var) >= KB_VAR_LEN ||
        strlen(val) >= sizeof s->b[0].val) {
        if (s->overflow) *s->overflow = 1;   /* exhausted, not disproved */
        return 0;
    }
    snprintf(s->b[s->n].var, KB_VAR_LEN, "%s", var);
    snprintf(s->b[s->n].val, sizeof s->b[0].val, "%s", val);
    s->n++;
    return 1;
}

static int dif_check(Subst *s) {
    for (size_t i = 0; i < s->ndif; i++) {
        char ra[KB_TERM_LEN], rb[KB_TERM_LEN];
        deep_resolve(s, s->dif[i].a, ra, sizeof ra, 0);
        deep_resolve(s, s->dif[i].b, rb, sizeof rb, 0);
        if (!is_var(ra) && !is_var(rb) && strcmp(ra, rb) == 0)
            return 0;
    }
    return 1;
}

static int dif_add(Subst *s, const char *a, const char *b) {
    if (s->ndif >= KB_MAX_DIF) return 0;
    snprintf(s->dif[s->ndif].a, KB_TERM_LEN, "%s", a);
    snprintf(s->dif[s->ndif].b, KB_TERM_LEN, "%s", b);
    s->ndif++;
    return dif_check(s);
}

static int bind_add_dif(Subst *s, const char *var, const char *val) {
    if (!bind_add(s, var, val)) return 0;
    if (!dif_check(s)) { s->n--; return 0; }
    return 1;
}

static int unify(Subst *s, const char *a, const char *b) {
    const char *ra = resolve(s, a);
    const char *rb = resolve(s, b);
    int va = is_var(ra), vb = is_var(rb);
    if (va && vb) return strcmp(ra, rb) == 0 ? 1 : bind_add_dif(s, ra, rb);
    if (va) return bind_add_dif(s, ra, rb);
    if (vb) return bind_add_dif(s, rb, ra);

    /* U3: both non-variable — unify STRUCTURALLY. A term of the form f(a…) is a
     * compound; unify recurses into it (a variable can already have bound to a
     * whole sub-structure above). No occurs-check (as standard Prolog); term
     * size bounds the recursion. */
    char fa[KB_TERM_LEN], fb[KB_TERM_LEN];
    char aa[KB_MAX_ARGS][KB_TERM_LEN], bb[KB_MAX_ARGS][KB_TERM_LEN];
    size_t na = 0, nb = 0;
    int ca = split_compound(ra, fa, aa, &na);
    int cb = split_compound(rb, fb, bb, &nb);
    if (ca && cb) {
        if (na != nb || strcmp(fa, fb) != 0) return 0;
        for (size_t i = 0; i < na; i++)
            if (!unify(s, aa[i], bb[i])) return 0;
        return 1;
    }
    if (ca || cb) return 0;                 /* one compound, one atom */
    return strcmp(ra, rb) == 0;             /* both atoms */
}

static int unify_term_fact(Subst *s, const Term *g, const Fact *f) {
    if (g->argc != f->argc || strcmp(g->pred, f->pred) != 0) return 0;
    for (size_t i = 0; i < g->argc; i++) {
        if (!unify(s, g->args[i], f->args[i])) return 0;
    }
    return 1;
}

static int unify_term_term(Subst *s, const Term *g, const Term *h) {
    if (g->argc != h->argc || strcmp(g->pred, h->pred) != 0) return 0;
    for (size_t i = 0; i < g->argc; i++) {
        if (!unify(s, g->args[i], h->args[i])) return 0;
    }
    return 1;
}

/* Rename ONE argument to a unique frame, recursing into compound terms (U3) so
 * a NESTED variable — the $N in s($N) — is standardized-apart too. Each named
 * variable becomes "name_<frame>"; each anonymous "_" a FRESH variable. */
static void rename_arg(const char *a, int frame, int *anon,
                       char *dst, size_t dstsz) {
    if (strcmp(a, "_") == 0) {
        snprintf(dst, dstsz, "_A%d_%d", frame, (*anon)++);
        return;
    }
    if (is_var(a)) {
        snprintf(dst, dstsz, "%s_%d", a, frame);
        return;
    }
    char f[KB_TERM_LEN], args[KB_MAX_ARGS][KB_TERM_LEN];
    size_t n = 0;
    if (split_compound(a, f, args, &n)) {        /* compound: recurse */
        int off = snprintf(dst, dstsz, "%s(", f);
        for (size_t i = 0; i < n && off > 0 && (size_t)off < dstsz; i++) {
            char sub[KB_TERM_LEN];
            rename_arg(args[i], frame, anon, sub, sizeof sub);
            off += snprintf(dst + off, dstsz - (size_t)off,
                            "%s%s", i ? ", " : "", sub);
        }
        if (off > 0 && (size_t)off < dstsz)
            snprintf(dst + off, dstsz - (size_t)off, ")");
        return;
    }
    snprintf(dst, dstsz, "%s", a);              /* atom */
}

/* Rename a rule's variables to a unique frame (standardize-apart). */
static void rename_term(const Term *src, int frame, int *anon, Term *dst) {
    strcpy(dst->pred, src->pred);
    dst->argc = src->argc;
    for (size_t i = 0; i < src->argc; i++)
        rename_arg(src->args[i], frame, anon, dst->args[i], KB_TERM_LEN);
    dst->neg = src->neg;   /* U6: the naf flag travels with the renamed goal */
}

static void push_unique(char out[][KB_TERM_LEN], size_t *count, size_t max,
                        const char *c) {
    for (size_t i = 0; i < *count; i++) {
        if (strcmp(out[i], c) == 0) return;
    }
    if (*count < max) strcpy(out[(*count)++], c);
}

typedef struct {
    const KB *kb;
    KB *kb_mut;                /* mutable KB for assert/retract builtins */
    const char *qvar;          /* variable to collect, or NULL for boolean */
    char (*out)[KB_TERM_LEN];
    size_t max;
    size_t count;
    int    found;
    int    bag;                /* gen389: 1 = conserva i duplicati (findall_bag) */
    int    frame;

    /* gen382 — the two anti-HYSTERESIS guards.
     *
     * KB_MAX_DEPTH bounds how DEEP a derivation goes; nothing bounded how MUCH
     * work it does. A knowledge base that grows rules faster than facts can send
     * the resolver into a search that is finite on paper and unbounded in
     * practice: mutually recursive rules re-ask the same question at every level
     * until the depth guard fires, once per branch, and the turn stalls. Growth
     * in the KB then degrades the ENGINE, which is the failure mode a KB-first
     * system must not have.
     *
     * `steps`  counts resolution steps and stops the search at `budget`.
     * `anc`    holds the GROUND goals currently open on this derivation path
     *          (as 64-bit hashes — a Term is 2.6KB and would not fit the stack).
     *          Meeting the same ground goal again means this branch is asking a
     *          question already open above it: it cannot yield anything the
     *          shorter derivation does not, so it is cut. That is the loop check
     *          that turns `p :- q. q :- p.` from a depth-64 explosion into an
     *          immediate, honest failure.
     *
     * Both events are COUNTED, never silent: kb_inference_report() hands them to
     * the caller so parrot0 can say it stopped and why, instead of presenting a
     * truncated search as a confident "no". */
    unsigned long steps;
    unsigned long budget;
    int      budget_hit;
    int      loops_cut;
    uint64_t anc[KB_MAX_DEPTH + 2];
    size_t   nanc;
} Solver;

/* FNV-1a over a resolved goal. Collisions would cut a live branch, so the
 * width matters: with at most KB_MAX_DEPTH+2 open goals the chance of a
 * 64-bit collision on one path is ~1e-17, far below the odds of the
 * derivation being wrong for ordinary reasons. */
static uint64_t goal_hash(const Term *g) {
    uint64_t h = UINT64_C(1469598103934665603);
    for (const unsigned char *p = (const unsigned char *)g->pred; *p; p++) {
        h ^= *p; h *= UINT64_C(1099511628211);
    }
    h ^= (uint64_t)g->argc; h *= UINT64_C(1099511628211);
    for (size_t i = 0; i < g->argc; i++) {
        for (const unsigned char *p = (const unsigned char *)g->args[i]; *p; p++) {
            h ^= *p; h *= UINT64_C(1099511628211);
        }
        h ^= UINT64_C(255); h *= UINT64_C(1099511628211);
    }
    return h;
}

/* KB_TERM_LEN also sizes substitutions and resolvents. Keeping those two large
 * scratch objects in every recursive C frame would make the logical depth limit
 * depend on the host thread's stack (and raising the text ceiling exposed that
 * at factorial(15)). One bounded scratch object per logical frame lives on the
 * heap instead; ownership is exactly the wrapper call below. */
typedef struct {
    Subst subst;
    Term  goals[KB_MAX_GOALS];
} SolveFrame;

static int solve(Solver *S, const Term *goals, size_t ngoals, size_t idx,
                 const Subst *s, int depth);   /* fwd: naf helpers call solve */
static int solve_frame(Solver *S, const Term *goals, size_t ngoals, size_t idx,
                       const Subst *s, int depth, SolveFrame *scratch);
static int parse_to_term(const char *s, Term *t); /* fwd: call/1 builtin */

/* U6 (teach-comprehension-via-mcp.md §6.2) negation-as-failure helpers. A body
 * goal marked `neg` (from naf(G)) succeeds iff G is NOT derivable. */

/* Ground a goal by resolving each arg through the current substitution. */
static void resolve_goal(const Term *g, const Subst *s, Term *out) {
    memset(out, 0, sizeof *out);
    strcpy(out->pred, g->pred);
    out->argc = g->argc;
    for (size_t i = 0; i < g->argc; i++)
        deep_resolve(s, g->args[i], out->args[i], KB_TERM_LEN, 0);
}
static int goal_ground(const Term *g) {
    for (size_t i = 0; i < g->argc; i++)
        if (term_contains_var(g->args[i], 0)) return 0;
    return 1;
}
/* Is the (ground) goal provable now? A fresh boolean solve, depth-guarded.
 *
 * Returns 1 provable, 0 finite failure, -1 INCOMPLETE (the search was cut by the
 * step budget). gen382o: the third answer used to be folded into 0, and that is
 * the difference between "I searched and it is not there" and "I stopped
 * searching". Only the first licenses a negative conclusion. Everything that
 * reasons about ABSENCE — negation as failure, and above all the gap detector of
 * question-emergence.md, whose whole subject is what the KB does NOT contain —
 * has to be able to tell them apart, or it will report exhaustion as knowledge. */
#define GOAL_INCOMPLETE (-1)
static int goal_provable(const KB *kb, const Term *g, int depth) {
    Solver S;
    memset(&S, 0, sizeof S);
    S.kb = kb; S.kb_mut = NULL; S.qvar = NULL;
    S.budget = KB_MAX_STEPS;
    Subst *s = calloc(1, sizeof *s);
    if (!s) return 0;
    s->overflow = &S.budget_hit;
    solve(&S, g, 1, 0, s, depth);
    free(s);
    if (S.found) return 1;
    return S.budget_hit ? GOAL_INCOMPLETE : 0;
}

/* U4 (teach-comprehension-via-mcp.md §5.3): string ⟷ char-list (de)serialization
 * — the fixed, operation-BLIND substrate under "string actions as knowledge". A
 * word becomes cons(c1, …, nil) of single-char atoms; alnum/'_' chars are bare,
 * others quoted. This is the ONLY string primitive: capitalize_first, pluralize,
 * etc. are Horn rules over the list, not C. */
#define KB_CHARLIST_MAX 4096
static void char_atom(char c, char *buf) {
    if (isalnum((unsigned char)c) || c == '_') { buf[0] = c; buf[1] = '\0'; }
    else { buf[0] = '"'; buf[1] = c; buf[2] = '"'; buf[3] = '\0'; }
}
static int atom_to_charlist(const char *a, char *out, size_t outsz) {
    char tmp[KB_TERM_LEN];
    size_t n = a ? strlen(a) : 0;
    if (n >= 2 && a[0] == '"' && a[n - 1] == '"') {   /* strip a quoted literal */
        if (n - 2 >= sizeof tmp) return 0;
        memcpy(tmp, a + 1, n - 2); tmp[n - 2] = '\0'; a = tmp; n -= 2;
    }
    char acc[KB_CHARLIST_MAX];
    snprintf(acc, sizeof acc, "nil");
    for (size_t i = n; i > 0; i--) {
        char cb[8]; char_atom(a[i - 1], cb);
        char nxt[KB_CHARLIST_MAX];
        int w = snprintf(nxt, sizeof nxt, "cons(%s, %s)", cb, acc);
        if (w < 0 || (size_t)w >= sizeof nxt) return 0;   /* too long */
        memcpy(acc, nxt, (size_t)w + 1);
    }
    return snprintf(out, outsz, "%s", acc) >= 0 && strlen(acc) < outsz;
}
static int charlist_to_atom(const char *list, char *out, size_t outsz) {
    char cur[KB_CHARLIST_MAX];
    snprintf(cur, sizeof cur, "%s", list ? list : "");
    size_t pos = 0;
    for (int guard = 0; guard < KB_CHARLIST_MAX; guard++) {
        char *p = cur; while (*p == ' ') p++;
        if (strcmp(p, "nil") == 0) {
            if (pos >= outsz) return 0;
            out[pos] = '\0'; return 1;
        }
        char f[KB_TERM_LEN], args[KB_MAX_ARGS][KB_TERM_LEN]; size_t na = 0;
        if (!split_compound(p, f, args, &na) || na != 2 || strcmp(f, "cons") != 0)
            return 0;
        const char *h = args[0]; size_t hn = strlen(h);
        char ch;
        if (hn >= 3 && h[0] == '"' && h[hn - 1] == '"') ch = h[1];   /* quoted */
        else if (hn == 1) ch = h[0];
        else return 0;                                  /* not a single char */
        if (pos + 1 >= outsz) return 0;
        out[pos++] = ch;
        snprintf(cur, sizeof cur, "%s", args[1]);       /* recurse on the tail */
    }
    return 0;
}

/* Reflective argument lists (gen382n).
 *
 * `kb_fact/2` and `apply/2` below expose operation-blind views to the KB: the
 * direct clauses it contains and the ability to invoke a predicate whose name
 * is itself data. Arguments use the same cons(..., nil) representation as the
 * teachable list procedures. This codec is fixed mechanics; it interprets no
 * predicate name, domain or gap kind. */
static int args_to_list(const char args[][KB_TERM_LEN], size_t argc,
                        char *out, size_t outsz) {
    char acc[KB_TERM_LEN];
    snprintf(acc, sizeof acc, "nil");
    for (size_t i = argc; i > 0; i--) {
        char next[KB_TERM_LEN];
        int n = snprintf(next, sizeof next, "cons(%s, %s)", args[i - 1], acc);
        if (n < 0 || (size_t)n >= sizeof next) return 0;
        memcpy(acc, next, (size_t)n + 1);
    }
    return snprintf(out, outsz, "%s", acc) >= 0 && strlen(acc) < outsz;
}

static int list_to_args(const char *list,
                        char args[][KB_TERM_LEN], size_t *argc) {
    char cur[KB_TERM_LEN];
    snprintf(cur, sizeof cur, "%s", list ? list : "");
    *argc = 0;
    for (size_t guard = 0; guard <= KB_MAX_ARGS; guard++) {
        char *p = cur;
        while (*p == ' ') p++;
        if (strcmp(p, "nil") == 0) return 1;
        if (*argc >= KB_MAX_ARGS) return 0;
        char fun[KB_TERM_LEN], parts[KB_MAX_ARGS][KB_TERM_LEN];
        size_t np = 0;
        if (!split_compound(p, fun, parts, &np) ||
            strcmp(fun, "cons") != 0 || np != 2)
            return 0;
        snprintf(args[(*argc)++], KB_TERM_LEN, "%s", parts[0]);
        snprintf(cur, sizeof cur, "%s", parts[1]);
    }
    return 0;
}

/* Prove the goal list under substitution `s`. Returns 1 to stop all search
 * (boolean solution found, or the collector is full). */
/* gen335 (teachable-procedures): a small arithmetic evaluator over a RESOLVED term
 * string, so taught PROCEDURES can compute with real numbers as KNOWLEDGE (not a C
 * consumer). Grammar:  expr := number | fn '(' expr ',' expr ')'  with fn in
 * {add,sub,mul,div,mod,binom}. Numbers are doubles; integer results render without a
 * fraction. Returns 1 on success, 0 if the expression is ill-formed or has an
 * unbound variable (the builtin then flounders honestly). No <math.h> dependency. */
static int eval_num(const char *e, double *out) {
    while (*e && isspace((unsigned char)*e)) e++;
    { char *end; double v = strtod(e, &end);        /* a bare number? */
      const char *p = end; while (*p && isspace((unsigned char)*p)) p++;
      if (end != e && *p == '\0') { *out = v; return 1; } }
    const char *lp = strchr(e, '(');                 /* else fn(a, b) */
    if (!lp) return 0;
    size_t fl = (size_t)(lp - e);
    while (fl > 0 && isspace((unsigned char)e[fl - 1])) fl--;
    const char *rp = strrchr(lp, ')');
    if (!rp || rp < lp) return 0;
    int d = 0; const char *comma = NULL;             /* top-level comma */
    for (const char *p = lp + 1; p < rp; p++) {
        if (*p == '(') d++;
        else if (*p == ')') d--;
        else if (*p == ',' && d == 0) { comma = p; break; }
    }
    if (!comma) return 0;
    char ab[256], bb[256];
    size_t al = (size_t)(comma - (lp + 1)), bl = (size_t)(rp - (comma + 1));
    if (al >= sizeof ab || bl >= sizeof bb) return 0;
    memcpy(ab, lp + 1, al); ab[al] = '\0';
    memcpy(bb, comma + 1, bl); bb[bl] = '\0';
    double a, b;
    if (!eval_num(ab, &a) || !eval_num(bb, &b)) return 0;
    if (fl == 5 && !strncmp(e, "binom", 5)) {
        long long n = (long long)a, k = (long long)b;
        if ((double)n != a || (double)k != b || n < 0 || k < 0 || k > n) return 0;
        if (k > n - k) k = n - k;
        unsigned long long r = 1;
        for (long long i = 1; i <= k; i++) {
            unsigned long long num = (unsigned long long)(n - k + i);
            unsigned long long den = (unsigned long long)i;
            unsigned long long g = 0, x = num, y = den;
            while (y) { unsigned long long t = x % y; x = y; y = t; }
            g = x ? x : 1;
            num /= g; den /= g;
            x = r; y = den;
            while (y) { unsigned long long t = x % y; x = y; y = t; }
            g = x ? x : 1;
            r /= g; den /= g;
            if (den != 1 || (num && r > 18446744073709551615ULL / num)) return 0;
            r *= num;
        }
        *out = (double)r;
        return 1;
    }
    if (fl != 3) return 0;
    if (!strncmp(e, "add", 3)) { *out = a + b; return 1; }
    if (!strncmp(e, "sub", 3)) { *out = a - b; return 1; }
    if (!strncmp(e, "mul", 3)) { *out = a * b; return 1; }
    if (!strncmp(e, "div", 3)) { if (b == 0) return 0; *out = a / b; return 1; }
    if (!strncmp(e, "mod", 3)) { if ((long long)b == 0) return 0;
                                 *out = (double)((long long)a % (long long)b); return 1; }
    return 0;
}

/* gen401: lo scratch di un passo non si alloca, si riusa.
 *
 * `SolveFrame` porta una `Subst` piu' 64 goal: mezzo megabyte scarso, chiesto e
 * restituito al sistema A OGNI PASSO di risoluzione. Il profiler `/debug` ha
 * mostrato che il costo per passo era di mezzo millisecondo mentre il lavoro
 * logico era di microsecondi, e questo `malloc` ne era la seconda meta' dopo la
 * copia della sostituzione.
 *
 * L'uso e' rigorosamente a pila — si prende entrando, si rende uscendo — quindi
 * un pool indicizzato dalla profondita' di annidamento e' esatto e non ha nulla
 * da liberare: le prime profondita' restano allocate e vengono riusate, oltre il
 * pool si torna al comportamento di prima. Le celle sono allocate PIGRAMENTE,
 * quindi una conversazione che non annida a fondo non paga la memoria che non
 * usa. parrot0 risolve in un solo thread: il pool e' statico per questo, e se un
 * giorno il solver diventera' concorrente questa e' la riga da spostare nel
 * Solver. */
#define KB_FRAME_POOL 64
static SolveFrame *frame_pool[KB_FRAME_POOL];
static size_t frame_depth;

static SolveFrame *frame_take(void) {
    if (frame_depth < KB_FRAME_POOL) {
        if (!frame_pool[frame_depth] &&
            !(frame_pool[frame_depth] = malloc(sizeof(SolveFrame)))) return NULL;
        return frame_pool[frame_depth++];
    }
    frame_depth++;
    return malloc(sizeof(SolveFrame));
}

static void frame_give(SolveFrame *scratch) {
    if (!scratch) return;
    frame_depth--;
    if (frame_depth >= KB_FRAME_POOL) free(scratch);
}

static int solve(Solver *S, const Term *goals, size_t ngoals, size_t idx,
                 const Subst *s, int depth) {
    if (idx == ngoals) {                       /* a complete solution */
        if (S->qvar == NULL) { S->found = 1; return 1; }
        char v[KB_TERM_LEN];
        deep_resolve(s, S->qvar, v, sizeof v, 0);   /* U3: render nested structure */
        /* gen389: `findall/3` e' un SET — deduplica — e non un BAG come in
         * Prolog standard. Non e' un difetto in se': contare i MEMBRI distinti di
         * una coorte, che e' l'uso storico qui, vuole esattamente questo. Ma
         * rende sbagliata ogni SOMMA: 1+1+2+2+2+8 (i pezzi di un giocatore a
         * scacchi) diventava 1+2+8 = 11 invece di 16, in silenzio.
         * `findall_bag/3` conserva i duplicati. Due nomi, due semantiche, nessuna
         * delle due implicita. */
        if (!is_var(v)) {
            if (S->bag) { if (S->count < S->max) snprintf(S->out[S->count++],
                                                          KB_TERM_LEN, "%s", v); }
            else push_unique(S->out, &S->count, S->max, v);
        }
        return S->count >= S->max;
    }
    /* gen396: the DEPTH ceiling is a cut short, exactly like the work ceiling.
     *
     * This returned a bare 0 for as long as the guard has existed, and a bare 0
     * is read everywhere as FINITE FAILURE. So a derivation that merely ran out
     * of levels came back as "searched and not there", and `naf/1` — whose whole
     * job is to tell those two apart — then SUCCEEDED on it. The compositional
     * answer plan is where it finally became visible: a six-piece sentence lost a
     * piece per level of nesting and rendered «Yes, » instead of «Yes, total is
     * 7.», because the negative guard that checks for a next piece concluded
     * there was none. Same signature as the gen396 polarity defect and the same
     * worst form — a wrong answer wearing the costume of an honest absence.
     *
     * Marking it makes goal_provable() return GOAL_INCOMPLETE, so negation
     * declines instead of concluding, and kb_inference_report() tells the caller
     * the search was cut. */
    if (depth > KB_MAX_DEPTH) { S->budget_hit = 1; return 0; }
    /* gen382: the work ceiling. Once hit, every pending branch unwinds without
     * doing more work, and the caller is told the search was cut short. */
    if (S->budget && S->steps >= S->budget) { S->budget_hit = 1; return 0; }
    S->steps++;

    SolveFrame *scratch = frame_take();
    if (!scratch) return 0;
    int result = solve_frame(S, goals, ngoals, idx, s, depth, scratch);
    frame_give(scratch);
    return result;
}

static int parse_ll_strict(const char *s, long long *out) {
    if (!s || !out || is_var(s)) return 0;
    char buf[KB_TERM_LEN];
    snprintf(buf, sizeof buf, "%s", s);
    char *p = buf;
    size_t l = strlen(p);
    if (l >= 2 && p[0] == '"' && p[l - 1] == '"') {
        p[l - 1] = '\0';
        p++;
    }
    char *end = NULL;
    long long v = strtoll(p, &end, 10);
    if (!end || *end) return 0;
    *out = v;
    return 1;
}

static long long digit_count_value(long long n, int digit) {
    if (n < 0) n = -n;
    if (n == 0) return digit == 0 ? 1 : 0;
    long long count = 0;
    while (n > 0) {
        if ((int)(n % 10) == digit) count++;
        n /= 10;
    }
    return count;
}

static int solve_frame(Solver *S, const Term *goals, size_t ngoals, size_t idx,
                       const Subst *s, int depth, SolveFrame *scratch) {

    const Term *g = &goals[idx];

    /* A rule body and its caller continuation share one flattened resolvent so
     * substitutions can flow through them.  The ancestor set must not share
     * that flattening: once the body has succeeded, the expanded goal is no
     * longer open and a sibling continuation may legitimately ask the same
     * ground view again.  This internal marker closes that logical scope before
     * continuing, then restores it while backtracking unwinds to the owner. */
    if (strcmp(g->pred, "__end_inference_scope") == 0 && g->argc == 0) {
        if (S->nanc == 0) return 0;            /* malformed internal resolvent */
        uint64_t closed = S->anc[--S->nanc];
        int ok = solve(S, goals, ngoals, idx + 1, s, depth);
        S->anc[S->nanc++] = closed;
        return ok;
    }

    if (g->neg) {                              /* U6: negation-as-failure */
        Term gg;
        resolve_goal(g, s, &gg);
        if (!goal_ground(&gg)) return 0;       /* floundering: decline honestly */
        int pv = goal_provable(S->kb, &gg, depth + 1);
        if (pv == 1) return 0;                              /* provable -> naf fails */
        if (pv == GOAL_INCOMPLETE) {   /* exhaustion is not absence: decline, loudly */
            S->budget_hit = 1;
            return 0;
        }
        return solve(S, goals, ngoals, idx + 1, s, depth);   /* not provable -> naf ok */
    }

    /* gen382n: virtual reflection over DIRECT positive facts. Clauses can now
     * derive facts about the fact table without a C scanner for each diagnosis.
     * Which predicates count as obligations, evidence, machinery or gaps is
     * deliberately left to KB rules. */
    if (strcmp(g->pred, "kb_fact") == 0 && g->argc == 2) {
        /* gen382o: when the predicate slot is already BOUND — the common shape
         * once a detector has picked a facet and is now counting its support —
         * the census names that predicate's facts, so the reflective view costs
         * a bucket walk instead of a scan of the whole KB at every step. The
         * unbound case still enumerates everything: that is the question being
         * asked, not an oversight. Behaviour is identical either way. */
        char rp[KB_TERM_LEN];
        deep_resolve(s, g->args[0], rp, sizeof rp, 0);
        PredBucket fbk = { NULL, 0, 0, 0 };
        int bound = !is_var(rp) && term_ok(rp) && !term_contains_var(rp, 0);
        if (bound) {
            fbk = pred_bucket(S->kb, rp);
            if (fbk.live && fbk.n == 0) return 0;   /* predicate unknown here */
        }
        size_t visits = bound ? PRED_VISITS(fbk, S->kb) : S->kb->n;
        for (size_t vi = 0; vi < visits; vi++) {
            size_t i = bound ? PRED_AT(fbk, vi) : vi;
            const Fact *f = &S->kb->facts[i];
            if (bound && strcmp(f->pred, rp) != 0) continue;
            char list[KB_TERM_LEN];
            if (!args_to_list(f->args, f->argc, list, sizeof list)) continue;
            Subst *s2 = &scratch->subst;
            subst_copy(s2, s);
            if (!unify(s2, g->args[0], f->pred) ||
                !unify(s2, g->args[1], list))
                continue;
            if (solve(S, goals, ngoals, idx + 1, s2, depth)) return 1;
        }
        return 0;
    }

    if (strcmp(g->pred, "chars") == 0 && g->argc == 2) {   /* U4: chars/2 builtin */
        char a0[KB_CHARLIST_MAX], a1[KB_CHARLIST_MAX];
        deep_resolve(s, g->args[0], a0, sizeof a0, 0);
        deep_resolve(s, g->args[1], a1, sizeof a1, 0);
        /* gen427 — UN TESTO FRA VIRGOLETTE E' GROUND ANCHE SE CONTIENE UN «$».
         *
         * La prova di groundness era «nessun dollaro nel termine», e il dollaro
         * e' il marcatore di variabile: giusto per un termine nudo, sbagliato
         * per una STRINGA. L'effetto era che `chars("$1000", $L)` non produceva
         * niente — non un errore, zero soluzioni — e con esso ogni regola che
         * guardi i caratteri di un prezzo, di un identificatore shell, di una
         * variabile di ambiente. Fra virgolette il dollaro e' un carattere. */
        int g0 = !is_var(a0) && (a0[0] == '"' || strchr(a0, '$') == NULL);
        int g1 = !is_var(a1) && (a1[0] == '"' || strchr(a1, '$') == NULL);
        Subst *s2 = &scratch->subst;
        subst_copy(s2, s);
        if (g0) {                              /* atom -> char-list */
            char list[KB_CHARLIST_MAX];
            if (atom_to_charlist(a0, list, sizeof list) &&
                unify(s2, g->args[1], list))
                return solve(S, goals, ngoals, idx + 1, s2, depth);
            return 0;
        }
        if (g1) {                              /* char-list -> atom */
            char atom[KB_TERM_LEN];
            if (charlist_to_atom(a1, atom, sizeof atom) &&
                unify(s2, g->args[0], atom))
                return solve(S, goals, ngoals, idx + 1, s2, depth);
            return 0;
        }
        return 0;                              /* both unbound: flounder */
    }

    /* gen395: `concat_atoms/3` — la concatenazione come meccanica.
     *
     * Esisteva come procedura KB sopra `chars/2` e `append_list/3`, ed era la
     * scelta giusta finche' le risposte composte erano corte. Misurato: oltre
     * una sessantina di caratteri la LISTA di caratteri intermedia sfonda
     * KB_TERM_LEN (una stringa di 58 caratteri diventa un termine di ~470, e il
     * livello successivo del fold la supera), quindi il piano proposizionale a
     * cinque pezzi perdeva la risposta — non con un errore, ma con zero
     * soluzioni, e il turno cadeva al percorso storico. E' la stessa specie di
     * fallimento silenzioso del §2.1 vincolo 5.
     *
     * Concatenare due stringhe non e' conoscenza: e' aritmetica di byte, come
     * `is/2`. Il fold del §K6 resta interamente in KB — questo primitivo non
     * decide nulla su COSA dire, e la clausola in `procedures.p0` resta come
     * struttura secondaria e come documentazione della semantica. */
    if (strcmp(g->pred, "concat_atoms") == 0 && g->argc == 3) {
        char a0[KB_TERM_LEN], a1[KB_TERM_LEN];
        deep_resolve(s, g->args[0], a0, sizeof a0, 0);
        deep_resolve(s, g->args[1], a1, sizeof a1, 0);
        if (is_var(a0) || is_var(a1)) return 0;   /* both inputs must be ground */
        char *t0 = a0, *t1 = a1;
        size_t l0 = strlen(t0), l1 = strlen(t1);
        if (l0 >= 2 && t0[0] == '"' && t0[l0 - 1] == '"') { t0[l0 - 1] = '\0'; t0++; }
        if (l1 >= 2 && t1[0] == '"' && t1[l1 - 1] == '"') { t1[l1 - 1] = '\0'; t1++; }
        char joined[KB_TERM_LEN];
        if ((int)snprintf(joined, sizeof joined, "%s%s", t0, t1) >= (int)sizeof joined)
            return 0;
        Subst *s2 = &scratch->subst;
        subst_copy(s2, s);
        if (unify(s2, g->args[2], joined))
            return solve(S, goals, ngoals, idx + 1, s2, depth);
        return 0;
    }

    /* gen393: `upcase_first/2` — the case transform, and only the transform.
     *
     * A KB-composed answer could add a language's sentence terminator (that fact
     * has existed since gen396) but could not open the sentence, so every
     * realization built from proof came out lowercase while every C module
     * capitalized on its own. Capitalizing is not knowledge — WHETHER a language
     * or register opens a sentence in upper case is, and that decision stays in
     * the KB (`sentence_initial/2`). This primitive knows no language: it
     * uppercases the first letter it finds, skipping an opening quote so a
     * quoted surface keeps its delimiters. */
    if (strcmp(g->pred, "upcase_first") == 0 && g->argc == 2) {
        char a0[KB_TERM_LEN];
        deep_resolve(s, g->args[0], a0, sizeof a0, 0);
        if (is_var(a0)) return 0;              /* input unbound: flounder */
        char up[KB_TERM_LEN];
        snprintf(up, sizeof up, "%s", a0);
        for (size_t i = 0; up[i]; i++) {
            if (up[i] == '"') continue;
            if (isalpha((unsigned char)up[i]))
                up[i] = (char)toupper((unsigned char)up[i]);
            break;
        }
        Subst *s2 = &scratch->subst;
        subst_copy(s2, s);
        if (unify(s2, g->args[1], up))
            return solve(S, goals, ngoals, idx + 1, s2, depth);
        return 0;
    }

    /* gen335 (teachable-procedures): arithmetic EVALUATION as engine primitives, so a
     * taught clause computes with real numbers — "how to sum / compare / filter" is
     * KNOWLEDGE, not a C consumer. `is($R, expr)` evaluates expr and binds $R; the
     * comparisons lt/le/gt/ge/eq/ne evaluate BOTH sides as expressions and succeed or
     * fail. Reserved builtin names (like chars/naf): they never fall through to the KB. */
    if (strcmp(g->pred, "is") == 0 && g->argc == 2) {
        char ex[KB_TERM_LEN]; double r;
        deep_resolve(s, g->args[1], ex, sizeof ex, 0);
        if (!eval_num(ex, &r)) return 0;       /* unbound/ill-formed -> flounder */
        char rs[64];
        if (r == (double)(long long)r) snprintf(rs, sizeof rs, "%lld", (long long)r);
        else snprintf(rs, sizeof rs, "%g", r);
        Subst *s2 = &scratch->subst;
        subst_copy(s2, s);
        if (unify(s2, g->args[0], rs))
            return solve(S, goals, ngoals, idx + 1, s2, depth);
        return 0;
    }
    if (g->argc == 2 &&
        (strcmp(g->pred,"lt")==0 || strcmp(g->pred,"le")==0 ||
         strcmp(g->pred,"gt")==0 || strcmp(g->pred,"ge")==0 ||
         strcmp(g->pred,"eq")==0 || strcmp(g->pred,"ne")==0)) {
        char ea[KB_TERM_LEN], eb[KB_TERM_LEN]; double a, b;
        deep_resolve(s, g->args[0], ea, sizeof ea, 0);
        deep_resolve(s, g->args[1], eb, sizeof eb, 0);
        if (!eval_num(ea, &a) || !eval_num(eb, &b)) return 0;
        int ok = 0;
        if      (!strcmp(g->pred,"lt")) ok = a <  b;
        else if (!strcmp(g->pred,"le")) ok = a <= b;
        else if (!strcmp(g->pred,"gt")) ok = a >  b;
        else if (!strcmp(g->pred,"ge")) ok = a >= b;
        else if (!strcmp(g->pred,"eq")) ok = a == b;
        else if (!strcmp(g->pred,"ne")) ok = a != b;
        if (!ok) return 0;
        return solve(S, goals, ngoals, idx + 1, s, depth);
    }

    if (strcmp(g->pred, "digit_count_between_prim") == 0 && g->argc == 4) {
        char db[KB_TERM_LEN], lb[KB_TERM_LEN], hb[KB_TERM_LEN];
        long long digit_ll, low, high;
        deep_resolve(s, g->args[0], db, sizeof db, 0);
        deep_resolve(s, g->args[1], lb, sizeof lb, 0);
        deep_resolve(s, g->args[2], hb, sizeof hb, 0);
        if (!parse_ll_strict(db, &digit_ll) ||
            !parse_ll_strict(lb, &low) ||
            !parse_ll_strict(hb, &high))
            return 0;
        if (digit_ll < 0 || digit_ll > 9) return 0;
        if (low > high) { long long t = low; low = high; high = t; }
        if (high - low > 1000000) return 0;
        long long count = 0;
        for (long long n = low; n <= high; n++)
            count += digit_count_value(n, (int)digit_ll);
        char cs[64];
        snprintf(cs, sizeof cs, "%lld", count);
        Subst *s2 = &scratch->subst;
        subst_copy(s2, s);
        if (unify(s2, g->args[3], cs))
            return solve(S, goals, ngoals, idx + 1, s2, depth);
        return 0;
    }

    if (strcmp(g->pred, "call") == 0 && g->argc == 1) {   /* call/1 meta-call */
        char resolved[KB_TERM_LEN];
        deep_resolve(s, g->args[0], resolved, sizeof resolved, 0);
        Term called;
        if (!parse_to_term(resolved, &called) || called.argc == 0) return 0;
        Term *ng = scratch->goals;
        size_t m = 0;
        if (m < KB_MAX_GOALS) term_copy(&ng[m++], &called);
        for (size_t k = idx + 1; k < ngoals && m < KB_MAX_GOALS; k++)
            term_copy(&ng[m++], &goals[k]);
        /* gen396: the resolvent ceiling is a cut short, not an absence — see
         * the depth guard in solve(). */
        if (m >= KB_MAX_GOALS && idx + 1 < ngoals) { S->budget_hit = 1; return 0; }
        return solve(S, ng, m, 0, s, depth + 1);
    }

    /* gen382n: apply(Predicate, ArgsList), the constructive twin of kb_fact/2.
     * It invokes normal resolution, so coverage includes facts AND rules and a
     * detector cannot confuse "not stored directly" with "not known". */
    if (strcmp(g->pred, "apply") == 0 && g->argc == 2) {
        char pred[KB_TERM_LEN], list[KB_TERM_LEN];
        deep_resolve(s, g->args[0], pred, sizeof pred, 0);
        deep_resolve(s, g->args[1], list, sizeof list, 0);
        if (is_var(pred) || !term_ok(pred)) return 0;

        Term called;
        memset(&called, 0, sizeof called);
        snprintf(called.pred, sizeof called.pred, "%s", pred);
        if (!list_to_args(list, called.args, &called.argc)) return 0;

        Term *ng = scratch->goals;
        size_t m = 0;
        if (m < KB_MAX_GOALS) term_copy(&ng[m++], &called);
        for (size_t k = idx + 1; k < ngoals && m < KB_MAX_GOALS; k++)
            term_copy(&ng[m++], &goals[k]);
        /* gen396: the resolvent ceiling is a cut short, not an absence — see
         * the depth guard in solve(). */
        if (m >= KB_MAX_GOALS && idx + 1 < ngoals) { S->budget_hit = 1; return 0; }
        return solve(S, ng, m, 0, s, depth + 1);
    }

    if (g->argc >= 1 && g->argc <= KB_MAX_ARGS + 1 &&
        (strcmp(g->pred, "assert") == 0 || strcmp(g->pred, "retract") == 0)) {
        if (!S->kb_mut) return 0;
        int is_retract = (strcmp(g->pred, "retract") == 0);
        char pred[KB_TERM_LEN];
        deep_resolve(s, g->args[0], pred, sizeof pred, 0);
        const char *fact_args[KB_MAX_ARGS];
        char argbufs[KB_MAX_ARGS][KB_TERM_LEN];
        size_t arity = g->argc - 1;
        if (arity > KB_MAX_ARGS) return 0;
        for (size_t i = 0; i < arity; i++) {
            deep_resolve(s, g->args[i + 1], argbufs[i], KB_TERM_LEN, 0);
            if (is_var(argbufs[i])) return 0;
            fact_args[i] = argbufs[i];
        }
        int ok;
        if (is_retract)
            ok = kb_retract(S->kb_mut, pred, fact_args, arity);
        else
            ok = kb_assert(S->kb_mut, pred, fact_args, arity);
        if (!ok) return 0;
        return solve(S, goals, ngoals, idx + 1, s, depth);
    }

    if (strcmp(g->pred, "dif") == 0 && g->argc == 2) {   /* dif/2 deferred inequality */
        char ra[KB_TERM_LEN], rb[KB_TERM_LEN];
        deep_resolve(s, g->args[0], ra, sizeof ra, 0);
        deep_resolve(s, g->args[1], rb, sizeof rb, 0);
        if (!is_var(ra) && !is_var(rb))
            return strcmp(ra, rb) != 0 ? solve(S, goals, ngoals, idx + 1, s, depth) : 0;
        Subst *s2 = &scratch->subst;
        subst_copy(s2, s);
        if (!dif_add(s2, g->args[0], g->args[1])) return 0;
        return solve(S, goals, ngoals, idx + 1, s2, depth);
    }

    if ((strcmp(g->pred, "findall") == 0 ||
         strcmp(g->pred, "findall_bag") == 0) && g->argc == 3) {
        char gs[KB_TERM_LEN], tv[KB_TERM_LEN];
        deep_resolve(s, g->args[1], gs, sizeof gs, 0);
        deep_resolve(s, g->args[0], tv, sizeof tv, 0);
        Term goal;
        if (!parse_to_term(gs, &goal) || goal.argc == 0) return 0;
        size_t max_sol = 8192;
        char (*solutions)[KB_TERM_LEN] = calloc(max_sol, KB_TERM_LEN);
        if (!solutions) return 0;
        Solver F;
        memset(&F, 0, sizeof F);
        F.kb = S->kb; F.kb_mut = S->kb_mut;
        F.qvar = is_var(tv) ? tv : "$Q";
        F.out = solutions;
        F.max = max_sol;
        F.bag = strcmp(g->pred, "findall_bag") == 0;
        /* gen382o — the sub-solver CONTINUES the caller's frame counter.
         *
         * Clause variables are renamed apart as `$Name_<frame>`, and this
         * sub-solve inherits the caller's substitution (`*fs = *s`) because the
         * findall goal may be partly bound. Restarting the counter at 0 therefore
         * re-issues names that are already BOUND above, so a callee whose clause
         * happens to use the same variable NAME as an ancestor silently inherits
         * its binding — a capture that makes the findall enumerate a subset and
         * report it as complete. It cost this project a detector that looked like
         * an arithmetic bug (question-emergence.md §9.3). Names issued below must
         * never be reissued above either, so the counter travels back out.
         *
         * The budget travels too: an interrupted enumeration must not be able to
         * masquerade as a small finite answer. `incomplete` is a third epistemic
         * outcome, and anything reasoning about ABSENCE has to be able to see it. */
        F.frame = S->frame;
        F.budget = S->budget;
        Subst *fs = &scratch->subst;
        subst_copy(fs, s);
        solve(&F, &goal, 1, 0, fs, 0);
        S->frame = F.frame;
        if (F.budget_hit) S->budget_hit = 1;
        char list_buf[KB_CHARLIST_MAX];
        snprintf(list_buf, sizeof list_buf, "nil");
        for (size_t i = F.count; i > 0; i--) {
            char nxt[KB_CHARLIST_MAX];
            int w = snprintf(nxt, sizeof nxt, "cons(%s, %s)", solutions[i - 1], list_buf);
            if (w < 0 || (size_t)w >= (int)sizeof nxt) { free(solutions); return 0; }
            memcpy(list_buf, nxt, (size_t)w + 1);
        }
        free(solutions);
        Subst *s2 = &scratch->subst;
        subst_copy(s2, s);
        if (unify(s2, g->args[2], list_buf))
            return solve(S, goals, ngoals, idx + 1, s2, depth);
        return 0;
    }

    if (strcmp(g->pred, "prob") == 0 && g->argc == 2) {   /* prob/2 KB-backed */
        char gs[KB_TERM_LEN];
        deep_resolve(s, g->args[0], gs, sizeof gs, 0);
        if (is_var(gs)) return 0;
        char pbuf[KB_TERM_LEN];
        const char *cargs[2] = {gs, NULL};
        char matches[1][KB_TERM_LEN];
        size_t nm = kb_match(S->kb, "fact_confidence", cargs, 2, matches, 1);
        snprintf(pbuf, sizeof pbuf, "%s", nm > 0 ? matches[0] : "0.5");
        Subst *s2 = &scratch->subst;
        subst_copy(s2, s);
        if (unify(s2, g->args[1], pbuf))
            return solve(S, goals, ngoals, idx + 1, s2, depth);
        return 0;
    }

    if (strcmp(g->pred, "ranges_over") == 0 && g->argc == 3) { /* temporal range */
        char ea[KB_TERM_LEN], eb[KB_TERM_LEN];
        double a, b;
        deep_resolve(s, g->args[1], ea, sizeof ea, 0);
        deep_resolve(s, g->args[2], eb, sizeof eb, 0);
        if (!eval_num(ea, &a) || !eval_num(eb, &b)) return 0;
        if (a > b) return 0;
        return solve(S, goals, ngoals, idx + 1, s, depth);
    }

    /* A goal that became ground through the current substitution has exactly
     * one possible matching ground fact.  Reuse the full-tuple hash here rather
     * than walking (and copying a large substitution for) every fact in the
     * predicate bucket.  This matters especially for selective rule guards:
     *
     *   humanities_summary(X, S) :- humanities_topic(X), means(X, S).
     *
     * A negative answer_frame probe used to scan every humanities_topic once
     * per input token even though humanities_topic(flimbo) is an exact miss.
     * The hash is already maintained by assert/retract, so this remains a view
     * of the live KB rather than a semantic cache that could go stale.
     *
     * Non-ground unit clauses are still visited below: a stored p($X) can
     * satisfy any ground p(value), and therefore is not represented by the
     * exact ground key.  If the predicate census is unavailable, retain the
     * historical full scan as the correctness fallback. */
    int ground_fact_mode = 0; /* 0 = scan all, 1 = only non-ground, 2 = skip */
    Term grounded_goal;
    resolve_goal(g, s, &grounded_goal);
    if (goal_ground(&grounded_goal)) {
        int census_live = 0;
        const PredStat *ps = pred_stats_get((KB *)S->kb, grounded_goal.pred,
                                            &census_live);
        if (census_live) {
            int has_nonground = ps && ps->nnonground > 0;
            const char *exact_args[KB_MAX_ARGS];
            for (size_t a = 0; a < grounded_goal.argc; a++)
                exact_args[a] = grounded_goal.args[a];
            Fact needle;
            int exact = fact_make(&needle, grounded_goal.pred, exact_args,
                                  grounded_goal.argc) &&
                        kb_find(S->kb, &needle) != NULL;
            if (exact) {
                if (solve(S, goals, ngoals, idx + 1, s, depth)) return 1;
                /* A continuation may contain assert/retract and then fail.
                 * Side effects persist in this engine, so refresh the census
                 * before considering alternative unit clauses. */
                ps = pred_stats_get((KB *)S->kb, grounded_goal.pred,
                                    &census_live);
                has_nonground = ps && ps->nnonground > 0;
            }
            if (census_live) ground_fact_mode = has_nonground ? 1 : 2;
        }
    }

    /* gen382: unify_term_fact can only succeed on a fact with this goal's
     * predicate, so the census bucket visits exactly the candidates instead of
     * the whole KB at every resolution step. */
    PredBucket gbk = pred_bucket(S->kb, g->pred);
    if (S->kb->prof_on) {
        KB *pm = (KB *)S->kb;   /* il contatore e' diagnostica, non stato logico */
        pm->prof_visits += PRED_VISITS(gbk, S->kb);
        if (!gbk.live) pm->prof_scans++;
    }
    for (size_t vi = 0; ground_fact_mode != 2 &&
                        vi < PRED_VISITS(gbk, S->kb); vi++) {  /* match facts */
        const Fact *f = &S->kb->facts[PRED_AT(gbk, vi)];
        Subst *s2 = &scratch->subst;
        int nonground = 0;
        if (gbk.nonground)
            for (size_t a = 0; a < f->argc && !nonground; a++)
                nonground = term_contains_var(f->args[a], 0);
        if (ground_fact_mode == 1 && !nonground) continue;
        /* gen401: la sostituzione si copia UNA VOLTA per goal, non una per
         * fatto candidato.
         *
         * Prima si copiava prima di ogni tentativo, cioe' anche per gli
         * ottantamila fatti che poi non unificavano: il profiler ha misurato
         * 81.842 visite per 960 passi, e ogni visita portava con se' l'intera
         * sostituzione viva. Era il grosso del costo per passo.
         *
         * L'annullamento e' esatto e non ha bisogno di un trail: `bind_add` e
         * `dif_add` APPENDONO soltanto — nessuno riscrive una voce esistente —
         * quindi riportare i due contatori dove erano cancella esattamente cio'
         * che il tentativo fallito aveva aggiunto. Se un giorno una di quelle
         * due funzioni cominciasse a modificare in luogo, questa riga diventa
         * sbagliata: e' la condizione da tenere d'occhio. */
        if (vi == 0 || s2->overflow != s->overflow) subst_copy(s2, s);
        size_t undo_n = s2->n, undo_ndif = s2->ndif;
        int matched = 0;
        if (!nonground) {
            matched = unify_term_fact(s2, g, f);
        } else {
            /* A fact containing variables is a unit clause, not a mutable
             * global substitution.  Its variables are fresh on every use just
             * like those in an ordinary rule (standardize-apart). */
            Term unit, renamed;
            memset(&unit, 0, sizeof unit);
            snprintf(unit.pred, sizeof unit.pred, "%s", f->pred);
            unit.argc = f->argc;
            for (size_t a = 0; a < f->argc; a++)
                snprintf(unit.args[a], sizeof unit.args[a], "%s", f->args[a]);
            int anon = 0;
            rename_term(&unit, ++S->frame, &anon, &renamed);
            matched = unify_term_term(s2, g, &renamed);
        }
        if (matched) {
            if (S->kb->audit_on)
                ((Fact *)f)->used = 1;      /* gen435: ha unificato almeno una volta */
            if (solve(S, goals, ngoals, idx + 1, s2, depth)) return 1;
        }
        s2->n = undo_n;            /* annulla il tentativo, riusa la copia */
        s2->ndif = undo_ndif;
    }

    PredBucket rbk = rule_bucket(S->kb, g->pred);
    int rule_copy_done = 0;
    for (size_t vi = 0; vi < PRED_VISITS(rbk, S->kb); vi++) { /* expand rules */
        const Rule *R = &S->kb->rules[PRED_AT(rbk, vi)];
        if (R->head.argc != g->argc || strcmp(R->head.pred, g->pred) != 0)
            continue;

        int fr = ++S->frame;
        int anon = 0; /* fresh-anonymous counter, shared across this clause */
        Term rhead;
        rename_term(&R->head, fr, &anon, &rhead);
        Subst *s2 = &scratch->subst;
        /* gen401: stessa economia del ciclo dei fatti — una copia per goal e
         * l'annullamento dei due contatori dopo un tentativo fallito. Le teste
         * di regola che non unificano sono la maggioranza, e copiare la
         * sostituzione per ciascuna era lavoro buttato. */
        if (!rule_copy_done) { subst_copy(s2, s); rule_copy_done = 1; }
        size_t rundo_n = s2->n, rundo_ndif = s2->ndif;
        if (!unify_term_term(s2, g, &rhead)) {
            s2->n = rundo_n; s2->ndif = rundo_ndif;
            continue;
        }

        /* gen382 — the loop check. Before descending into this rule, note the
         * goal it is expanding. If that exact GROUND goal is already open above
         * us, the branch is re-asking a question this path has not answered yet:
         * it can only reproduce the derivation we are already inside. Cut it,
         * count it, and go on to the next rule — the search stays complete for
         * everything that is not the repetition. Non-ground goals are never cut:
         * `ancestor_of($X, $Y)` above and `ancestor_of(ann, $Y)` below are
         * different questions, and pruning on a variable would silence real
         * answers. */
        Term rg;
        resolve_goal(g, s2, &rg);
        int pushed = 0;
        if (goal_ground(&rg)) {
            uint64_t h = goal_hash(&rg);
            int seen = 0;
            for (size_t a = 0; a < S->nanc && !seen; a++) if (S->anc[a] == h) seen = 1;
            if (seen) { S->loops_cut++; continue; }
            if (S->nanc < sizeof S->anc / sizeof S->anc[0]) {
                S->anc[S->nanc++] = h;
                pushed = 1;
            }
        }

        Term *ng = scratch->goals;
        size_t m = 0;
        int overflow = 0;
        for (size_t b = 0; b < R->nbody; b++) {
            if (m >= KB_MAX_GOALS) { overflow = 1; break; }
            rename_term(&R->body[b], fr, &anon, &ng[m++]);
        }
        if (pushed && !overflow) {
            if (m >= KB_MAX_GOALS) overflow = 1;
            else {
                memset(&ng[m], 0, sizeof ng[m]);
                snprintf(ng[m].pred, sizeof ng[m].pred,
                         "%s", "__end_inference_scope");
                m++;
            }
        }
        for (size_t k = idx + 1; k < ngoals && !overflow; k++) {
            if (m >= KB_MAX_GOALS) { overflow = 1; break; }
            term_copy(&ng[m++], &goals[k]);
        }
        /* gen396: the resolvent could not hold this rule's body plus the
         * caller's continuation. Skipping the rule silently is the third face of
         * the same defect as the depth guard: an unexplored branch presented as
         * one that was explored and found empty. Mark the search incomplete so
         * negation declines instead of concluding. */
        if (overflow) {
            if (pushed) S->nanc--;
            S->budget_hit = 1;
            s2->n = rundo_n; s2->ndif = rundo_ndif;
            continue;
        }
        int ok = solve(S, ng, m, 0, s2, depth + 1);
        if (pushed) S->nanc--;
        if (ok) return 1;
        /* La regola non ha portato una soluzione: la sostituzione torna dov'era
         * PRIMA della sua testa. Senza questa riga la clausola successiva
         * partirebbe da un contesto sporcato da quella fallita — che e' il
         * prezzo esatto di aver smesso di ricopiare a ogni tentativo, e va
         * pagato qui invece che ottantamila volte piu' in la'. */
        s2->n = rundo_n; s2->ndif = rundo_ndif;
    }
    return 0;
}

static int term_make(Term *t, const char *pred, const char *const *args,
                     size_t argc) {
    if (!term_ok(pred)) return 0;
    memset(t, 0, sizeof *t);
    strcpy(t->pred, pred);
    t->argc = argc;
    for (size_t i = 0; i < argc; i++) {
        if (!term_ok(args[i])) return 0;
        strcpy(t->args[i], args[i]);
    }
    return 1;
}

int kb_assert_clause(KB *kb, const KbGoal *head,
                     const KbGoal *body, size_t nbody) {
    if (!kb || !head || (nbody > 0 && !body)) return 0;
    if (nbody == 0 || nbody > KB_MAX_BODY) return 0;
    if (head->argc > KB_MAX_ARGS) return 0;
    if (!term_ok(head->pred)) return 0;
    for (size_t i = 0; i < head->argc; i++)
        if (!term_ok(head->args[i])) return 0;
    for (size_t gi = 0; gi < nbody; gi++) {
        if (!term_ok(body[gi].pred)) return 0;
        if (body[gi].argc > KB_MAX_ARGS) return 0;
        for (size_t j = 0; j < body[gi].argc; j++)
            if (!term_ok(body[gi].args[j])) return 0;
    }

    Rule r;
    memset(&r, 0, sizeof r);
    if (!term_make(&r.head, head->pred, head->args, head->argc)) return 0;
    for (size_t gi = 0; gi < nbody; gi++) {
        if (!term_make(&r.body[gi], body[gi].pred,
                       body[gi].args, body[gi].argc)) return 0;
        r.body[gi].neg = body[gi].neg;   /* U6: carry naf onto the stored goal */
    }
    r.nbody = nbody;
    r.origin = kb->origin;

    for (size_t ri = 0; ri < kb->nr; ri++) {
        const Rule *R = &kb->rules[ri];
        if (R->nbody != r.nbody || R->head.argc != r.head.argc) continue;
        if (strcmp(R->head.pred, r.head.pred) != 0) continue;
        int same = 1;
        for (size_t a = 0; a < r.head.argc && same; a++)
            if (strcmp(R->head.args[a], r.head.args[a]) != 0) same = 0;
        for (size_t b = 0; b < r.nbody && same; b++) {
            if (R->body[b].argc != r.body[b].argc ||
                R->body[b].neg != r.body[b].neg ||
                strcmp(R->body[b].pred, r.body[b].pred) != 0) { same = 0; break; }
            for (size_t a = 0; a < r.body[b].argc && same; a++)
                if (strcmp(R->body[b].args[a], r.body[b].args[a]) != 0) same = 0;
        }
        if (same) return 1;
    }

    return kb_add_rule(kb, &r);
}

/* Record what a finished search cost. `kb` may be the const-cast query target;
 * this writes only the report, never the knowledge. */
/* djb2: basta e costa poco. Non deve resistere a un avversario — deve
 * distinguere due strade diverse. */
static unsigned long kb_fp_hash(const char *s) {
    unsigned long h = 5381;
    while (s && *s) h = h * 33u ^ (unsigned char)*s++;
    return h;
}

/* gen435 — L'AUDIT A FREDDO.
 *
 * Acceso, ogni fatto che unifica si segna. Spento (il default) non costa
 * nulla, stessa disciplina del profiler. Il giudizio su che cosa significhi
 * «mai usato» non sta qui: sta in KB (`dormant_by_design/1`), perche' una
 * conoscenza che tace per disegno e una che tace per un difetto si distinguono
 * sapendo a che cosa servono, e questo il motore non lo sa. */
void kb_audit_set(KB *kb, int on) {
    if (!kb) return;
    kb->audit_on = on ? 1 : 0;
    if (on) for (size_t i = 0; i < kb->n; i++) kb->facts[i].used = 0;
}

size_t kb_unused_by_pred(const KB *kb, char preds[][KB_TERM_LEN],
                         size_t unused[], size_t total[], size_t max) {
    if (!kb || !preds || !unused || !total) return 0;
    size_t np = 0;
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *f = &kb->facts[i];
        size_t k = 0;
        for (; k < np; k++) if (strcmp(preds[k], f->pred) == 0) break;
        if (k == np) {
            if (np >= max) continue;
            snprintf(preds[np], KB_TERM_LEN, "%s", f->pred);
            unused[np] = 0; total[np] = 0; np++;
        }
        total[k]++;
        if (!f->used) unused[k]++;
    }
    return np;
}

int kb_first_unused_row(const KB *kb, const char *pred, char *out, size_t sz) {
    if (!kb || !pred || !out || sz == 0) return 0;
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *f = &kb->facts[i];
        if (f->used || strcmp(f->pred, pred) != 0) continue;
        size_t o = (size_t)snprintf(out, sz, "%s(", f->pred);
        for (size_t a = 0; a < f->argc && o < sz; a++)
            o += (size_t)snprintf(out + o, sz - o, "%s%s", a ? ", " : "", f->args[a]);
        if (o + 2 < sz) snprintf(out + o, sz - o, ")");
        return 1;
    }
    return 0;
}

void kb_footprint_reset(KB *kb) { if (kb) { kb->fp_acc = 0; kb->fp_n = 0; } }
unsigned long kb_footprint(const KB *kb) { return kb ? kb->fp_acc : 0; }
size_t kb_footprint_width(const KB *kb) { return kb ? kb->fp_n : 0; }



static void kb_footprint_note(KB *kb, const char *pred) {
    if (!kb || !pred || !*pred) return;
    unsigned long h = kb_fp_hash(pred);
    for (size_t i = 0; i < kb->fp_n; i++) if (kb->fp_seen[i] == h) return;
    if (kb->fp_n < sizeof kb->fp_seen / sizeof kb->fp_seen[0]) {
        if (kb->prof_on)
            snprintf(kb->fp_name[kb->fp_n], sizeof kb->fp_name[0], "%s", pred);
        else
            kb->fp_name[kb->fp_n][0] = '\0';
        kb->fp_seen[kb->fp_n++] = h;
    }
    kb->fp_acc ^= h;
}

/* I nomi dei predicati toccati in questo turno, in ordine di prima visita.
 * Vuoto (NULL) se il profilo era spento: si dice, non si finge. */
const char *kb_footprint_pred(const KB *kb, size_t i) {
    if (!kb || i >= kb->fp_n || !kb->fp_name[i][0]) return NULL;
    return kb->fp_name[i];
}

/* gen422d — anche CHI ha risposto fa parte della strada.
 *
 * Misurato: «9-4» e «why» toccavano gli STESSI 57 predicati e portavano la
 * stessa firma. Non era una collisione di hash — erano davvero gli stessi
 * predicati, perche' l'aritmetica non interroga la KB: la calcola in C. La firma
 * stava catturando l'impalcatura del dispatch, uguale per tutti, e mancava
 * l'unica cosa che quei due turni avevano di diverso — il modulo che ha
 * risposto. Un turno risolto in C senza toccare la conoscenza e' comunque una
 * strada, e va firmata come tale. */
void kb_footprint_mark(KB *kb, const char *tag) { kb_footprint_note(kb, tag); }

static void kb_note_inference(KB *kb, const Solver *S, const char *goalpred) {
    if (!kb) return;
    kb_footprint_note(kb, goalpred);
    kb->infer_steps      = S->steps;
    kb->infer_budget_hit = S->budget_hit;
    kb->infer_loops_cut  = S->loops_cut;
    snprintf(kb->infer_goal, sizeof kb->infer_goal, "%s", goalpred ? goalpred : "");
    /* gen400: accumulo per TURNO. La domanda vera non e' quanto costa un goal —
     * quella la dice gia' `infer_steps` — ma dove siano finiti i passi di un
     * turno intero, che di goal ne apre decine. */
    if (!kb->prof_on) return;
    double ms = kb_prof_elapsed(kb);
    kb->prof_calls++;
    kb->prof_steps += S->steps;
    kb->prof_ms += ms;
    const char *name = goalpred ? goalpred : "";
    for (size_t i = 0; i < kb->prof_ntop; i++)
        if (strcmp(kb->prof_top[i].pred, name) == 0) {
            kb->prof_top[i].calls++;
            kb->prof_top[i].steps += S->steps;
            kb->prof_top[i].ms += ms;
            return;
        }
    if (kb->prof_ntop < sizeof kb->prof_top / sizeof kb->prof_top[0]) {
        KbProfileRow *row = &kb->prof_top[kb->prof_ntop++];
        snprintf(row->pred, sizeof row->pred, "%s", name);
        row->calls = 1;
        row->steps = S->steps;
        row->ms = ms;
    }
}

int kb_query(KB *kb, const char *pred, const char *const *args, size_t argc) {
    /* gen422b: la firma si raccoglie QUI e in kb_match, non solo alla fine di
     * una ricerca del solver. La prima stesura annotava solo `kb_note_inference`
     * — cioe' le sole risoluzioni con regole — e la firma veniva identica per
     * turni lontanissimi: «1+1», «dog», «?» e «is rex a dog» davano tutti
     * 19e0ffa3, perche' la maggior parte dei moduli interroga fatti ground per
     * la via rapida e non passava mai di li'. La strada di un turno e' fatta
     * soprattutto di QUELLE domande. */
    kb_footprint_note(kb, pred);
    if (!kb || !term_ok(pred) || argc > KB_MAX_ARGS || (argc && !args)) return 0;
    for (size_t i = 0; i < argc; i++) if (!term_ok(args[i])) return 0;
    if (kb_is_negated(kb, pred, args, argc)) return 0;

    /* The overwhelmingly common KB-first lookup is a ground fact predicate with
     * no rules. Avoid constructing an SLD search that scans every unrelated fact
     * at every evidence query; rule-bearing predicates keep the full solver. */
    int has_rule = (argc == 2 && (strcmp(pred, "chars") == 0 ||   /* solver builtins */
        strcmp(pred,"upcase_first")==0 || strcmp(pred,"concat_atoms")==0 ||
        strcmp(pred,"kb_fact")==0 || strcmp(pred,"apply")==0 ||
        strcmp(pred,"is")==0 || strcmp(pred,"lt")==0 || strcmp(pred,"le")==0 ||
        strcmp(pred,"gt")==0 || strcmp(pred,"ge")==0 || strcmp(pred,"eq")==0 ||
        strcmp(pred,"ne")==0 || strcmp(pred,"call")==0 ||
        strcmp(pred,"assert")==0 || strcmp(pred,"retract")==0 ||
        strcmp(pred,"dif")==0 ||         strcmp(pred,"findall")==0 ||
        strcmp(pred,"findall_bag")==0 || strcmp(pred,"prob")==0 ||
        strcmp(pred,"ranges_over")==0));
    if (!has_rule) {
        PredBucket rbk = rule_bucket(kb, pred);
        for (size_t vi = 0; vi < PRED_VISITS(rbk, kb); vi++) {
            const Rule *r = &kb->rules[PRED_AT(rbk, vi)];
            if (r->head.argc == argc && strcmp(r->head.pred, pred) == 0) {
                has_rule = 1;
                break;
            }
        }
    }

    /* gen382 — settle the common lookup from the census, without touching the
     * fact array. Two decisions become O(1) that used to cost a full scan each:
     *
     *   no rule, no fact for this predicate  -> nothing can prove it, answer NO
     *   no rule, no NON-GROUND fact, ground query -> the hash index is exact
     *
     * Both are conservative: whenever the census is unavailable or the predicate
     * carries a clause with variables, control falls through to the historical
     * scan below, so behaviour is unchanged and only the cost differs. */
    int census_live = 0;
    const PredStat *ps = pred_stats_get(kb, pred, &census_live);
    int query_ground = 1;
    for (size_t i = 0; i < argc && query_ground; i++)
        if (args[i] && term_contains_var(args[i], 0)) query_ground = 0;
    if (!has_rule && census_live) {
        if (!ps) return 0;                          /* predicate unknown here */
        if (ps->nnonground == 0 && query_ground) {
            Fact needle;
            if (!fact_make(&needle, pred, args, argc)) return 0;
            Fact *hit = (Fact *)kb_find(kb, &needle);
            /* gen435: anche la via RAPIDA e' un uso. Senza questo segno l'audit
             * dichiarava muta meta' della KB — «vowel_letter mai usato» mentre
             * il classificatore della parola sola lo interroga a ogni turno —
             * cioe' misurava il proprio percorso invece della conoscenza. */
            if (hit && kb->audit_on) hit->used = 1;
            return hit != NULL;
        }
    }

    /* A body-less clause is stored in the fact table and may still contain
     * variables/compound terms.  Such facts require real unification too. */
    if (!has_rule && census_live && ps->nnonground > 0) has_rule = 1;
    for (size_t i = 0; i < kb->n && !has_rule && !census_live; i++) {
        const Fact *f = &kb->facts[i];
        if (f->argc != argc || strcmp(f->pred, pred) != 0) continue;
        for (size_t a = 0; a < argc; a++) {
            if (term_contains_var(f->args[a], 0)) {
                has_rule = 1;
                break;
            }
        }
    }
    if (!has_rule && !query_ground)
        has_rule = 1; /* variable/structural unify */
    if (!has_rule) {
        Subst *work = malloc(sizeof *work);
        if (!work) return 0;
        PredBucket bk = pred_bucket(kb, pred);
        for (size_t vi = 0; vi < PRED_VISITS(bk, kb); vi++) {
            const Fact *f = &kb->facts[PRED_AT(bk, vi)];
            if (f->argc != argc || strcmp(f->pred, pred) != 0) continue;
            work->n = 0; work->ndif = 0;
            int same = 1;
            for (size_t a = 0; a < argc; a++)
                if (!unify(work, args[a], f->args[a])) { same = 0; break; }
            if (same) {
                if (kb->audit_on) ((Fact *)f)->used = 1;   /* gen435 */
                free(work); return 1;
            }
        }
        free(work);
        return 0;
    }

    Term g;
    if (!term_make(&g, pred, args, argc)) return 0;

    Solver S;
    memset(&S, 0, sizeof S);
    S.kb = kb; S.kb_mut = kb;
    S.budget = KB_MAX_STEPS;
    kb_prof_start(kb);
    Subst *s = malloc(sizeof *s);
    if (!s) return 0;
    s->n = 0; s->ndif = 0; s->overflow = &S.budget_hit;
    solve(&S, &g, 1, 0, s, 0);
    kb_note_inference(kb, &S, pred);
    free(s);
    return S.found;
}

size_t kb_match(const KB *kb, const char *pred, const char *const *args,
                size_t argc, char out[][KB_TERM_LEN], size_t max) {
    if (!kb || !term_ok(pred) || argc > KB_MAX_ARGS || (argc && !args) ||
        (max && !out)) return 0;
    /* vedi kb_query: la firma e' scritta anche da qui. `kb` e' const per
     * contratto sulla CONOSCENZA — la firma e' un contatore di percorso, non
     * conoscenza, come gia' fa kb_note_inference con il report. */
    kb_footprint_note((KB *)kb, pred);

    /* Fast exact-fact pattern match when no rule can derive this predicate and
     * the only variables are the public NULL slots.  Semantics are identical to
     * the solver path (distinct NULL variables; collect the first; deduplicate).
     * Compound patterns containing nested $/_ variables still use unification. */
    int first_var = -1, simple = max > 0 && strcmp(pred, "chars") != 0 &&
                    strcmp(pred, "kb_fact") != 0 && strcmp(pred, "apply") != 0;
    for (size_t i = 0; i < argc; i++) {
        if (!args[i]) { if (first_var < 0) first_var = (int)i; continue; }
        if (term_contains_var(args[i], 0))
            simple = 0;
    }
    if (first_var < 0) simple = 0; /* preserve the boolean-query behaviour */
    if (simple) {
        PredBucket rbk = rule_bucket(kb, pred);
        for (size_t vi = 0; vi < PRED_VISITS(rbk, kb); vi++) {
            const Rule *r = &kb->rules[PRED_AT(rbk, vi)];
            if (r->head.argc == argc && strcmp(r->head.pred, pred) == 0) {
                simple = 0;
                break;
            }
        }
    }
    /* gen382: only this predicate's own facts can disqualify the fast path, and
     * the census names them without walking the KB. */
    PredBucket bk = pred_bucket(kb, pred);
    if (simple && bk.live && bk.n == 0) return 0;   /* predicate unknown here */
    for (size_t vi = 0; vi < PRED_VISITS(bk, kb) && simple; vi++) {
        const Fact *f = &kb->facts[PRED_AT(bk, vi)];
        if (f->argc != argc || strcmp(f->pred, pred) != 0) continue;
        for (size_t a = 0; a < argc; a++) {
            if (term_contains_var(f->args[a], 0)) {
                simple = 0;
                break;
            }
        }
    }
    if (simple) {
        size_t count = 0;
        Subst *work = malloc(sizeof *work);
        if (!work) return 0;
        for (size_t vi = 0; vi < PRED_VISITS(bk, kb); vi++) {
            size_t i = PRED_AT(bk, vi);
            const Fact *f = &kb->facts[i];
            if (f->argc != argc || strcmp(f->pred, pred) != 0) continue;
            work->n = 0; work->ndif = 0;
            int match = 1;
            for (size_t a = 0; a < argc; a++)
                if (args[a] && !unify(work, args[a], f->args[a])) {
                    match = 0; break;
                }
            if (match) {
                if (kb->audit_on) ((Fact *)f)->used = 1;   /* gen435 */
                char value[KB_TERM_LEN];
                deep_resolve(work, f->args[first_var], value, sizeof value, 0);
                push_unique(out, &count, max, value);
            }
            if (count >= max) break;
        }
        free(work);
        return count;
    }

    Term g;
    memset(&g, 0, sizeof g);
    if (!term_ok(pred)) return 0;
    strcpy(g.pred, pred);
    g.argc = argc;
    /* Each NULL slot is a DISTINCT fresh variable; we collect the first one.
     * (Naming every NULL the same would force those slots to be equal — e.g.
     * cont(prev, ?, ?) would wrongly require word == count.) */
    int hasvar = 0;
    for (size_t i = 0; i < argc; i++) {
        if (args[i] == NULL) {
            /* $-prefixed since gen284 ($-only variables): "Q" is now a constant. */
            if (!hasvar) { strcpy(g.args[i], "$Q"); hasvar = 1; }
            else snprintf(g.args[i], KB_TERM_LEN, "$Q%zu", i);
        }
        else { if (!term_ok(args[i])) return 0; strcpy(g.args[i], args[i]); }
    }

    Solver S;
    memset(&S, 0, sizeof S);
    S.kb = kb; S.kb_mut = (KB *)kb;
    S.budget = KB_MAX_STEPS;
    kb_prof_start((KB *)kb);
    S.qvar = hasvar ? "$Q" : NULL;
    S.out = out;
    S.max = max;
    Subst *s = malloc(sizeof *s);
    if (!s) return 0;
    s->n = 0; s->ndif = 0; s->overflow = &S.budget_hit;
    solve(&S, &g, 1, 0, s, 0);
    kb_note_inference((KB *)kb, &S, pred);
    free(s);
    return S.count;
}

/* ----------------------------------------------------------------------------
 * explanation: prove a goal AND render its proof tree (gen14)
 * ------------------------------------------------------------------------- */

/* Render a goal grounded by the substitution into "pred(a, b)" form. */
static void render_goal(const Subst *s, const Term *g, char *buf, size_t sz) {
    int off = snprintf(buf, sz, "%s(", g->pred);
    for (size_t i = 0; i < g->argc && off > 0 && (size_t)off < sz; i++) {
        char v[KB_TERM_LEN];
        deep_resolve(s, g->args[i], v, sizeof v, 0);   /* U3: nested structure */
        off += snprintf(buf + off, sz - (size_t)off, "%s%s", i ? ", " : "", v);
    }
    if (off > 0 && (size_t)off < sz) snprintf(buf + off, sz - (size_t)off, ")");
}

typedef struct {
    Subst subst;
    Term  goals[KB_PROOF_PG];
    char  proofs[KB_PROOF_PG][KB_PROOF_LEN];
} ProofFrame;

static int prove_seq_frame(KB *kb, const Term *goals, size_t n, size_t idx,
                           const Subst *s, int depth, int *frame,
                           char out[][KB_PROOF_LEN], ProofFrame *scratch);

/* Prove goals[idx..n-1] and, on success, render each goal's proof into
 * out[idx..n-1]. A goal proven by a fact renders as the ground goal; a goal
 * proven by a rule renders as "<goal> because <sub0> and <sub1> ...". The
 * resolvent trick (body ++ continuation) keeps backtracking correct; the first
 * `nbody` resulting proofs belong to the rule's body, the rest to the
 * continuation. */
static int prove_seq_ex(KB *kb, const Term *goals, size_t n, size_t idx,
                        const Subst *s, int depth, int *frame,
                        char out[][KB_PROOF_LEN]) {
    if (idx == n) return 1;
    if (idx >= KB_PROOF_PG) return 0;
    ProofFrame *scratch = malloc(sizeof *scratch);
    if (!scratch) return 0;
    int result = prove_seq_frame(kb, goals, n, idx, s, depth, frame, out, scratch);
    free(scratch);
    return result;
}

static int prove_seq_frame(KB *kb, const Term *goals, size_t n, size_t idx,
                           const Subst *s, int depth, int *frame,
                           char out[][KB_PROOF_LEN], ProofFrame *scratch) {
    const Term *g = &goals[idx];

    if (g->neg) {                                   /* U6: NAF in the proof */
        Term gg;
        resolve_goal(g, s, &gg);
        if (!goal_ground(&gg)) return 0;
        if (goal_provable(kb, &gg, depth + 1) != 0) return 0;  /* provable OR incomplete */
        if (prove_seq_ex(kb, goals, n, idx + 1, s, depth, frame, out)) {
            char inner[KB_PROOF_LEN];
            render_goal(s, g, inner, sizeof inner);
            snprintf(out[idx], KB_PROOF_LEN, "not %s", inner);
            return 1;
        }
        return 0;
    }

    if (strcmp(g->pred, "call") == 0 && g->argc == 1) { /* call/1 proof */
        char resolved[KB_TERM_LEN];
        deep_resolve(s, g->args[0], resolved, sizeof resolved, 0);
        Term called;
        if (!parse_to_term(resolved, &called) || called.argc == 0) return 0;
        Term *comb = scratch->goals;
        size_t m = 0;
        if (m < KB_PROOF_PG) comb[m++] = called;
        for (size_t k = idx + 1; k < n && m < KB_PROOF_PG; k++)
            term_copy(&comb[m++], &goals[k]);
        char (*cout)[KB_PROOF_LEN] = scratch->proofs;
        if (prove_seq_ex(kb, comb, m, 0, s, depth, frame, cout)) {
            snprintf(out[idx], KB_PROOF_LEN, "%s", cout[0]);
            for (size_t k = idx + 1; k < n; k++)
                strcpy(out[k], cout[1 + (k - (idx + 1))]);
            return 1;
        }
        return 0;
    }

    if (strcmp(g->pred, "assert") == 0 || strcmp(g->pred, "retract") == 0) {
        if (prove_seq_ex(kb, goals, n, idx + 1, s, depth, frame, out)) {
            char body[KB_PROOF_LEN];
            render_goal(s, g, body, sizeof body);
            snprintf(out[idx], KB_PROOF_LEN, "%s", body);
            return 1;
        }
        return 0;
    }

    if (strcmp(g->pred, "dif") == 0 && g->argc == 2) {
        char ra[KB_TERM_LEN], rb[KB_TERM_LEN];
        deep_resolve(s, g->args[0], ra, sizeof ra, 0);
        deep_resolve(s, g->args[1], rb, sizeof rb, 0);
        if (!is_var(ra) && !is_var(rb) && strcmp(ra, rb) == 0) return 0;
        if (prove_seq_ex(kb, goals, n, idx + 1, s, depth, frame, out)) {
            char body[KB_PROOF_LEN];
            render_goal(s, g, body, sizeof body);
            snprintf(out[idx], KB_PROOF_LEN, "%s", body);
            return 1;
        }
        return 0;
    }

    if (strcmp(g->pred, "findall") == 0 && g->argc == 3) { /* findall/3 proof */
        if (prove_seq_ex(kb, goals, n, idx + 1, s, depth, frame, out)) {
            char body[KB_PROOF_LEN];
            render_goal(s, g, body, sizeof body);
            snprintf(out[idx], KB_PROOF_LEN, "%s", body);
            return 1;
        }
        return 0;
    }

    if (strcmp(g->pred, "prob") == 0 && g->argc == 2) { /* prob/2 proof */
        if (prove_seq_ex(kb, goals, n, idx + 1, s, depth, frame, out)) {
            char body[KB_PROOF_LEN];
            render_goal(s, g, body, sizeof body);
            snprintf(out[idx], KB_PROOF_LEN, "%s", body);
            return 1;
        }
        return 0;
    }

    if (strcmp(g->pred, "ranges_over") == 0 && g->argc == 3) { /* ranges_over proof */
        if (prove_seq_ex(kb, goals, n, idx + 1, s, depth, frame, out)) {
            char body[KB_PROOF_LEN];
            render_goal(s, g, body, sizeof body);
            snprintf(out[idx], KB_PROOF_LEN, "%s", body);
            return 1;
        }
        return 0;
    }

    PredBucket pbk = pred_bucket(kb, g->pred);
    for (size_t vi = 0; vi < PRED_VISITS(pbk, kb); vi++) {  /* close by a fact */
        Subst *s2 = &scratch->subst;
        subst_copy(s2, s);
        if (unify_term_fact(s2, g, &kb->facts[PRED_AT(pbk, vi)])) {
            if (prove_seq_ex(kb, goals, n, idx + 1, s2, depth, frame, out)) {
                render_goal(s2, g, out[idx], KB_PROOF_LEN);
                return 1;
            }
        }
    }

    if (depth > KB_MAX_DEPTH) return 0;
    PredBucket rbk = rule_bucket(kb, g->pred);
    for (size_t vi = 0; vi < PRED_VISITS(rbk, kb); vi++) { /* expand a rule */
        const Rule *R = &kb->rules[PRED_AT(rbk, vi)];
        if (R->head.argc != g->argc || strcmp(R->head.pred, g->pred) != 0)
            continue;
        int fr = ++(*frame), anon = 0;
        Term rhead;
        rename_term(&R->head, fr, &anon, &rhead);
        Subst *s2 = &scratch->subst;
        subst_copy(s2, s);
        if (!unify_term_term(s2, g, &rhead)) continue;

        Term *comb = scratch->goals;
        size_t m = 0;
        int overflow = 0;
        for (size_t b = 0; b < R->nbody; b++) {
            if (m >= KB_PROOF_PG) { overflow = 1; break; }
            rename_term(&R->body[b], fr, &anon, &comb[m++]);
        }
        for (size_t k = idx + 1; k < n && !overflow; k++) {
            if (m >= KB_PROOF_PG) { overflow = 1; break; }
            term_copy(&comb[m++], &goals[k]);
        }
        if (overflow) continue;

        char (*cout)[KB_PROOF_LEN] = scratch->proofs;
        if (prove_seq_ex(kb, comb, m, 0, s2, depth + 1, frame, cout)) {
            char head[KB_PROOF_LEN];
            render_goal(s2, g, head, sizeof head);
            int off = snprintf(out[idx], KB_PROOF_LEN, "%s because ", head);
            for (size_t b = 0; b < R->nbody && off > 0 &&
                               (size_t)off < KB_PROOF_LEN; b++) {
                off += snprintf(out[idx] + off, KB_PROOF_LEN - (size_t)off,
                                "%s%s", b ? " and " : "", cout[b]);
            }
            for (size_t k = idx + 1; k < n; k++)
                strcpy(out[k], cout[R->nbody + (k - (idx + 1))]);
            return 1;
        }
    }
    return 0;
}

int kb_explain(KB *kb, const char *pred, const char *const *args,
               size_t argc, char *out, size_t out_size) {
    if (!kb || argc > KB_MAX_ARGS || out_size == 0) return 0;
    Term g;
    if (!term_make(&g, pred, args, argc)) return 0;

    char proofs[KB_PROOF_PG][KB_PROOF_LEN];
    Subst *s = calloc(1, sizeof *s);
    if (!s) return 0;
    int frame = 0;
    if (!prove_seq_ex(kb, &g, 1, 0, s, 0, &frame, proofs)) {
        free(s);
        return 0;
    }

    snprintf(out, out_size, "%s", proofs[0]);
    free(s);
    return 1;
}

/* ----------------------------------------------------------------------------
 * induction ("training"): learn unary rules from facts
 * ------------------------------------------------------------------------- */

static int fact_present(KB *kb, const char *pred, const char *c) {
    const char *a[] = {c};
    Fact f;
    return fact_make(&f, pred, a, 1) && kb_find(kb, &f) != NULL;
}

size_t kb_induce(KB *kb, size_t min_support,
                 char out_head[][KB_TERM_LEN], char out_body[][KB_TERM_LEN],
                 size_t max) {
    if (!kb) return 0;

    int saved_origin = kb->origin;
    kb->origin = KB_INDUCED; /* tag everything we induce */

    char (*preds)[KB_TERM_LEN] = malloc(256 * sizeof *preds);
    if (!preds) { kb->origin = saved_origin; return 0; }
    size_t np = 0;
    for (size_t i = 0; i < kb->n; i++) {
        /* gen73: skip meta-knowledge predicates (lexicon, social, reflective).
         * Induction should operate on domain facts, not on curated word lists. */
        const char *mp = kb->facts[i].pred;
        if (strcmp(mp, "stopword") == 0 ||
            strcmp(mp, "question_word") == 0 ||
            strcmp(mp, "reaction_word") == 0 ||
            strcmp(mp, "social_marker") == 0 ||
            strcmp(mp, "social_pattern") == 0 ||
            strcmp(mp, "i_am") == 0 ||
            strcmp(mp, "module") == 0 ||
            strcmp(mp, "cmd") == 0 ||
            strcmp(mp, "flag") == 0 ||
            strcmp(mp, "cont") == 0 ||
            strcmp(mp, "cont2") == 0) continue;
        /* gen432 — LA MECCANICA NON SI GENERALIZZA, e adesso conta davvero.
         *
         * L'elenco cablato qui sopra nomina undici predicati; il resto della
         * meccanica si dichiara da se' con `machinery/1` dal gen344. Non
         * filtrarla qui la faceva entrare nell'induzione e RIEMPIRE il buffer
         * delle regole indotte — sedici posti — con cose come
         * `content_kind(X) :- countable_opener(X)`, che il chiamante poi
         * scartava una per una lasciando fuori la regola vera. Il sintomo era
         * «Nothing new to generalize» su una KB che aveva appena imparato che
         * ogni uomo e' mortale (misurato: induce.p0t). */
        {
            const char *mq[1] = { mp };
            if (kb_query((KB *)kb, "machinery", mq, 1)) continue;
        }
        if (kb->facts[i].argc == 1) push_unique(preds, &np, 256, kb->facts[i].pred);
    }

    size_t found = 0;
    for (size_t bi = 0; bi < np; bi++) {
        const char *P = preds[bi];
        for (size_t hi = 0; hi < np; hi++) {
            if (hi == bi) continue;
            const char *Q = preds[hi];
            if (rule_exists(kb, Q, P)) continue;

            size_t support = 0;
            int all_q = 1;
            for (size_t i = 0; i < kb->n && all_q; i++) {
                const Fact *f = &kb->facts[i];
                if (f->argc != 1 || strcmp(f->pred, P) != 0) continue;
                support++;
                if (!fact_present(kb, Q, f->args[0])) all_q = 0;
            }

            if (all_q && support >= min_support) {
                kb_assert_rule(kb, Q, P);
                /* gen432 — SI RESTITUISCE QUANTE SE NE SONO SCRITTE.
                 *
                 * La firma promette «capped at max» e il conteggio invece
                 * cresceva oltre il buffer: il chiamante ciclava fino a `found`
                 * e leggeva righe MAI SCRITTE, cioe' memoria a caso, e la
                 * stampava come regole indotte — «Induced: <byte a caso>(X) :-
                 * <byte a caso>(X)». Non si vedeva finche' le regole inducibili
                 * restavano meno di sedici; e' bastato far crescere la KB di
                 * qualche fatto per scoprirlo (misurato: abduce.p0t).
                 *
                 * Stessa specie del tetto sui registri delle cue: un numero che
                 * descrive un buffer non puo' superarlo. */
                if (found >= max) continue;
                strcpy(out_head[found], Q);
                strcpy(out_body[found], P);
                found++;
            }
        }
    }

    kb->origin = saved_origin;
    free(preds);
    return found;
}

/* ----------------------------------------------------------------------------
 * persistence: human-readable, Prolog-like text (DESIGN.md D1-D3)
 * ------------------------------------------------------------------------- */

static int parse_term(const char *s, char *pred,
                      char args[][KB_TERM_LEN], size_t *argc) {
    while (*s && isspace((unsigned char)*s)) s++;
    const char *lp = strchr(s, '(');
    if (!lp) {                                    /* zero-arity atom: ready. */
        size_t plen = strlen(s);
        while (plen > 0 && isspace((unsigned char)s[plen - 1])) plen--;
        if (plen == 0 || plen >= KB_TERM_LEN) return 0;
        for (size_t i = 0; i < plen; i++)
            if (!(isalnum((unsigned char)s[i]) || s[i] == '_' || s[i] == '$'))
                return 0;
        memcpy(pred, s, plen);
        pred[plen] = '\0';
        *argc = 0;
        return 1;
    }
    const char *rp = lp ? strrchr(lp, ')') : NULL;
    if (!lp || !rp || rp < lp) return 0;

    size_t plen = (size_t)(lp - s);
    while (plen > 0 && isspace((unsigned char)s[plen - 1])) plen--;
    if (plen == 0 || plen >= KB_TERM_LEN) return 0;
    memcpy(pred, s, plen);
    pred[plen] = '\0';

    *argc = 0;
    const char *p = lp + 1;
    {   const char *empty = p;
        while (empty < rp && isspace((unsigned char)*empty)) empty++;
        if (empty == rp) return 1;                /* explicit zero arity: ready() */
    }
    while (p < rp) {
        while (p < rp && isspace((unsigned char)*p)) p++;
        const char *start = p;
        if (p < rp && *p == '"') {
            /* gen152: a quoted string literal is ONE argument — commas (and any
             * other punctuation) inside the quotes are content, not separators.
             * The stored atom keeps its quotes, so the description renderer can
             * recognise it. Without this, prose descriptions with commas were
             * shredded into garbage multi-arg facts. */
            int escaped = 0, closed = 0;
            p++;                                   /* opening quote */
            while (p < rp) {
                if (escaped) { escaped = 0; p++; continue; }
                if (*p == '\\') { escaped = 1; p++; continue; }
                if (*p == '"') { p++; closed = 1; break; }
                p++;
            }
            if (!closed) return 0;
        } else {
            /* U3: split on TOP-LEVEL commas only, so a compound-term argument
             * f(a, b) stays one arg (nested commas are structure, not
             * separators). */
            int d = 0;
            while (p < rp && !(*p == ',' && d == 0)) {
                if (*p == '(') d++;
                else if (*p == ')' && d > 0) d--;
                p++;
            }
        }
        const char *end = p;
        while (end > start && isspace((unsigned char)end[-1])) end--;
        size_t alen = (size_t)(end - start);
        if (alen == 0 || alen >= KB_TERM_LEN || *argc >= KB_MAX_ARGS) return 0;
        memcpy(args[*argc], start, alen);
        args[*argc][alen] = '\0';
        (*argc)++;
        { int d = 0;                               /* advance to top-level ',' */
          while (p < rp && !(*p == ',' && d == 0)) {
              if (*p == '(') d++;
              else if (*p == ')' && d > 0) d--;
              p++;
          } }
        if (p < rp && *p == ',') p++;
    }
    return *argc > 0;
}

/* gen396: parse_to_term DEFINES the whole goal, `neg` included.
 *
 * `parse_term` writes pred/args/argc only, so the polarity of the produced goal
 * used to come from whatever the caller's storage happened to hold. Three
 * callers pass an automatic `Term` they never zero — `findall/3` and both
 * `call/1` paths — so a goal built at runtime inherited a stack byte as its
 * negation flag. When that byte was non-zero the solver read the goal as
 * `naf(G)`, found it non-ground, and declined by FLOUNDERING: the enumeration
 * returned nothing and the caller saw a clean, silent empty set.
 *
 * That is the whole shape of the symptom recorded in KB_TODO ("apply/2 does not
 * behave inside findall/3", "three consecutive kb_match returning 0"): a
 * derivation that is correct on its own collapses to zero solutions depending on
 * what ran BEFORE it in the same process — a wrong answer wearing the clothes of
 * an honest absence. A goal's polarity is part of the goal; it is set here, at
 * the single point that builds one, and never left to memory. */
static int parse_to_term(const char *s, Term *t) {
    t->neg = 0;
    return parse_term(s, t->pred, t->args, &t->argc);
}

/* Parse a rule BODY goal, honoring a naf(…) wrapper (U6). naf(G) parses G and
 * marks the goal negation-as-failure; anything else is an ordinary goal. */
static int parse_goal(const char *s, Term *t) {
    while (*s && isspace((unsigned char)*s)) s++;
    size_t n = strlen(s);
    while (n > 0 && isspace((unsigned char)s[n - 1])) n--;
    if (n >= 6 && strncmp(s, "naf(", 4) == 0 && s[n - 1] == ')') {
        char inner[KB_TERM_LEN * KB_MAX_ARGS];
        size_t len = n - 5;                 /* strip "naf(" (4) and ")" (1) */
        if (len == 0 || len >= sizeof inner) return 0;
        memcpy(inner, s + 4, len);
        inner[len] = '\0';
        if (!parse_to_term(inner, t)) return 0;
        t->neg = 1;
        return 1;
    }
    return parse_to_term(s, t);
}

static int parse_neg_term(const char *s, char *pred,
                          char args[][KB_TERM_LEN], size_t *argc) {
    size_t n = strlen(s);
    if (n < 7 || strncmp(s, "not(", 4) != 0 || s[n - 1] != ')') return 0;
    char inner[KB_TERM_LEN * (KB_MAX_ARGS + 1)];
    size_t len = n - 5; /* strip "not(" and the final ')' */
    if (len == 0 || len >= sizeof inner) return 0;
    memcpy(inner, s + 4, len);
    inner[len] = '\0';
    return parse_term(inner, pred, args, argc);
}

/* Split a rule body into goal strings on top-level (depth-0) commas. The
 * input buffer is modified in place. */
static size_t split_goals(char *body, char *goals[], size_t max) {
    size_t n = 0;
    int depth = 0, quoted = 0, escaped = 0;
    char *start = body;
    for (char *p = body;; p++) {
        if (quoted) {
            if (escaped) escaped = 0;
            else if (*p == '\\') escaped = 1;
            else if (*p == '"') quoted = 0;
        } else if (*p == '"') quoted = 1;
        else if (*p == '(') depth++;
        else if (*p == ')') depth--;
        if ((*p == ',' && depth == 0 && !quoted) || *p == '\0') {
            char saved = *p;
            *p = '\0';
            if (n < max) goals[n] = start;
            n++;                                    /* preserve overflow evidence */
            start = p + 1;
            if (saved == '\0') break;
        }
    }
    return n;
}

/* Process ONE clause string `s` (already trimmed, trailing '.' removed): a
 * `:- include(...)` directive, an explicit negative, a rule (head :- goals) or a
 * fact. `dir` is the directory of the enclosing file, for relative includes.
 * Returns the number of clauses added to `kb` (0 or 1; an include loads and counts
 * its own clauses separately). Factored out of kb_load (gen335) so the loader can
 * feed it MULTIPLE clauses per physical line — the .p0 parser is now mature enough
 * to read "a(1). b(2). c(3)." on one line instead of silently dropping all but the
 * first (F.: multi-clause lines were not read). */
/* L'implicazione `:-` di una clausola sta sempre FUORI da una stringa quotata.
 * Cercarla con strstr trovava anche quella dentro `":-)"`. */
static char *find_implication(char *s) {
    int q = 0;
    for (char *p = s; *p; p++) {
        if (*p == '\\' && p[1]) { p++; continue; }
        if (*p == '"') { q = !q; continue; }
        if (!q && p[0] == ':' && p[1] == '-') return p;
    }
    return NULL;
}

static int load_clause(KB *kb, const char *path, const char *dir, char *s) {
    size_t n = strlen(s);
    if (n == 0) return 0;

    if (strncmp(s, ":- include(", 11) == 0) {   /* gen150: relative include */
        char inc_path[512];
        const char *start = s + 11;
        const char *end = s + n - 1;             /* the ')' (trailing '.' gone) */
        if (*end == ')') {
            size_t ilen = (size_t)(end - start);
            if (ilen > 0 && ilen < sizeof inc_path) {
                memcpy(inc_path, start, ilen);
                inc_path[ilen] = '\0';
                char *ip = inc_path;
                while (*ip && isspace((unsigned char)*ip)) ip++;
                if (*ip == '"' || *ip == '\'') { ip++; inc_path[ilen - 1] = '\0'; }
                size_t iplen = strlen(ip);
                while (iplen > 0 && (isspace((unsigned char)ip[iplen - 1]) ||
                       ip[iplen - 1] == '"' || ip[iplen - 1] == '\''))
                    ip[--iplen] = '\0';
                char full[768];
                if (dir[0] && ip[0] != '/')
                    snprintf(full, sizeof full, "%s/%s", dir, ip);
                else
                    snprintf(full, sizeof full, "%s", ip);
                kb_load(kb, full);
            }
        }
        return 0;
    }

    char neg_pred[KB_TERM_LEN];
    char neg_args[KB_MAX_ARGS][KB_TERM_LEN];
    size_t neg_argc;
    if (parse_neg_term(s, neg_pred, neg_args, &neg_argc)) {
        const char *argp[KB_MAX_ARGS];
        for (size_t i = 0; i < neg_argc; i++) argp[i] = neg_args[i];
        return kb_assert_neg(kb, neg_pred, argp, neg_argc) ? 1 : 0;
    }

    /* gen405: l'implicazione va cercata FUORI dalle stringhe. `intent_cue(playful,
     * ":-)")` contiene `:-` dentro le virgolette, e il caricatore lo leggeva come
     * una regola malformata e lo scartava — silenziosamente per chi non guardava
     * stderr. Erano cinque emoticon del registro affettivo, entrate al gen403 e
     * mai arrivate nella KB.
     *
     * La regola generale e' quella: la conoscenza non puo' essere limitata dalla
     * punteggiatura che contiene. Una faccina e' un dato come un altro. */
    char *arrow = find_implication(s);
    if (arrow) {                                /* rule: head :- goals */
        char *rulebuf = malloc(n + 1);           /* keep `s` intact for error text */
        if (!rulebuf) {
            fprintf(stderr, "kb_load: PARSE ERROR in %s: out of memory while parsing clause\n",
                    path ? path : "?");
            return 0;
        }
        memcpy(rulebuf, s, n + 1);
        char *rarrow = find_implication(rulebuf);
        *rarrow = '\0';
        Rule r;
        memset(&r, 0, sizeof r);
        r.origin = kb->origin;
        int ok = parse_to_term(rulebuf, &r.head);
        char *goals[KB_MAX_BODY];
        size_t ng = ok ? split_goals(rarrow + 2, goals, KB_MAX_BODY) : 0;
        if (ng > KB_MAX_BODY) {
            fprintf(stderr,
                    "kb_load: PARSE ERROR in %s: too many body goals (limit %d), dropped without partial rule: '%s.'\n",
                    path ? path : "?", KB_MAX_BODY, s);
            free(rulebuf);
            return 0;
        }
        if (ok && ng == 0) ok = 0;
        for (size_t i = 0; i < ng && ok; i++)
            if (!parse_goal(goals[i], &r.body[i])) ok = 0;
        if (ok) {
            r.nbody = ng;
            ok = kb_add_rule(kb, &r);
            free(rulebuf);
            return ok ? 1 : 0;
        }
        fprintf(stderr, "kb_load: PARSE ERROR in %s: bad rule, dropped: '%s.'\n",
                path ? path : "?", s);
        free(rulebuf);
        return 0;
    }

    char pred[KB_TERM_LEN];                     /* fact */
    char a[KB_MAX_ARGS][KB_TERM_LEN];
    size_t ac;
    if (parse_term(s, pred, a, &ac)) {
        const char *argp[KB_MAX_ARGS];
        for (size_t i = 0; i < ac; i++) argp[i] = a[i];
        return kb_assert(kb, pred, argp, ac) ? 1 : 0;
    }
    /* F. (gen335): NEVER drop a non-empty clause in silence — that has bitten us at
     * least 5 times (multi-clause lines, stray syntax). A loud error is mandatory so
     * the next malformed .p0 is caught at load, not by a mysterious missing fact. */
    fprintf(stderr, "kb_load: PARSE ERROR in %s: cannot parse clause, dropped: '%s.'\n",
            path ? path : "?", s);
    return 0;
}

/* Grow the logical-clause accumulator without imposing a second, unrelated
 * line-size ceiling. The term/arity parser remains the authority on what can be
 * represented; the transport must not silently truncate before it gets there. */
/* 1 = appended, 0 = allocation failure, -1 = structural clause budget hit. */
static int loadbuf_put(char **buf, size_t *len, size_t *cap, char c) {
    if (*len >= KB_CLAUSE_MAX) return -1;
    if (*len + 1 >= *cap) {
        size_t next = *cap ? *cap * 2 : 256;
        if (next > KB_CLAUSE_MAX + 1) next = KB_CLAUSE_MAX + 1;
        if (next <= *cap || next > (size_t)-1 / sizeof **buf) return 0;
        char *grown = realloc(*buf, next);
        if (!grown) return 0;
        *buf = grown;
        *cap = next;
    }
    (*buf)[(*len)++] = c;
    (*buf)[*len] = '\0';
    return 1;
}

static void loadbuf_trim(char *buf, size_t *len) {
    size_t start = 0;
    while (start < *len && isspace((unsigned char)buf[start])) start++;
    while (*len > start && isspace((unsigned char)buf[*len - 1])) (*len)--;
    if (start > 0) memmove(buf, buf + start, *len - start);
    *len -= start;
    buf[*len] = '\0';
}

/* gen345 (test-engine): assert a SINGLE clause from a text string, reusing the
 * full .p0 parser (facts, rules, quoted strings, negatives). A trailing '.' is
 * optional — load_clause is fed the clause with the period already stripped, so
 * we drop one here if present. `dir` is "." so a bare include would resolve from
 * the CWD; test mocks assert facts, not includes. Returns clauses added (0/1). */
/* ── la mappa di salvataggio: primitive ──────────────────────────────────────
 *
 * Parse di una clausola gia' normalizzata (una riga, senza il punto finale) in
 * (predicato, primo argomento). Ritorna 1 solo per un fatto ground: commenti,
 * regole, direttive e negativi espliciti non sono case per nessuno — infilare
 * un fatto dentro il corpo di una regola e' esattamente il difetto che questa
 * funzione esiste per non commettere. */
static int sm_parse(const char *line, char *pred, char *arg1) {
    const char *s = line;
    while (*s && isspace((unsigned char)*s)) s++;
    if (*s == '\0' || *s == '%' || *s == ':') return 0;
    if (strstr(s, ":-")) return 0;              /* a rule */
    if (!strncmp(s, "not(", 4)) return 0;       /* explicit negative */
    const char *lp = strchr(s, '(');
    if (!lp || lp == s) return 0;
    size_t pl = (size_t)(lp - s);
    if (pl >= SM_PRED) return 0;
    for (size_t i = 0; i < pl; i++)
        if (!isalnum((unsigned char)s[i]) && s[i] != '_') return 0;
    memcpy(pred, s, pl); pred[pl] = '\0';
    const char *a = lp + 1;
    while (*a && isspace((unsigned char)*a)) a++;
    int depth = 0, q = 0;
    const char *start = a, *end = NULL;
    for (const char *p = a; *p; p++) {
        char c = *p;
        if (c == '"') q = !q;
        else if (q) continue;
        else if (c == '(') depth++;
        else if (c == ')') { if (depth == 0) { end = p; break; } depth--; }
        else if (c == ',' && depth == 0) { end = p; break; }
    }
    if (!end) return 0;
    size_t al = (size_t)(end - start);
    while (al > 0 && isspace((unsigned char)start[al - 1])) al--;
    if (al >= 2 && start[0] == '"' && start[al - 1] == '"') { start++; al -= 2; }
    if (al == 0 || al >= SM_ARG) return 0;
    memcpy(arg1, start, al); arg1[al] = '\0';
    return 1;
}

/* La chiave di un argomento: senza virgolette. In KB lo stesso atomo puo'
 * arrivare citato o nudo (`blue` e `"blue"` sono la stessa cosa) mentre la mappa
 * tiene sempre la forma nuda; confrontare le due forme com'erano faceva fallire
 * in silenzio ogni instradamento per entita' citata. */
static void sm_key(const char *arg, char *out) {
    size_t l = strlen(arg);
    if (l >= 2 && arg[0] == '"' && arg[l - 1] == '"') { arg++; l -= 2; }
    if (l >= SM_ARG) l = SM_ARG - 1;
    memcpy(out, arg, l); out[l] = '\0';
}

/* ── la tabella: UNA chiave, UNA posizione ───────────────────────────────────
 *
 * Non un elenco di righe, una MAPPA (F., gen411). Se la stessa chiave torna, la
 * posizione si AGGIORNA invece di aggiungere una riga: vince l'ultima vista.
 * Cosi' la tabella non cresce con i fatti ma con le coppie distinte, e la
 * ricerca e' un accesso, non una scansione.
 *
 * Due forme di chiave nella stessa tabella — e' questo il «multichiave»:
 *
 *     "sound_of\\x01dog"  -> (file, riga)     la coppia
 *     "sound_of"          -> (file, riga)     la sola relazione
 *
 * Non possono collidere fra loro perche' un predicato non contiene \\x01. */
static unsigned long sm_hash(const char *s) {
    unsigned long h = 5381;
    while (*s) h = h * 33u ^ (unsigned char)*s++;
    return h;
}

static int smap_grow(SaveMap *m) {
    size_t cap = m->cap ? m->cap * 2 : 2048;
    SmSlot *ns = calloc(cap, sizeof *ns);
    if (!ns) return 0;
    for (size_t i = 0; i < m->cap; i++) {
        if (!m->slots[i].key) continue;
        size_t j = sm_hash(m->slots[i].key) & (cap - 1);
        while (ns[j].key) j = (j + 1) & (cap - 1);
        ns[j] = m->slots[i];
    }
    free(m->slots);
    m->slots = ns; m->cap = cap;
    return 1;
}

static void smap_put(SaveMap *m, const char *key, int file, int line) {
    if (m->n * 10 >= m->cap * 7 && !smap_grow(m)) return;
    size_t j = sm_hash(key) & (m->cap - 1);
    while (m->slots[j].key) {
        if (!strcmp(m->slots[j].key, key)) {      /* gia' nota: si AGGIORNA */
            m->slots[j].file = file; m->slots[j].line = line;
            return;
        }
        j = (j + 1) & (m->cap - 1);
    }
    size_t l = strlen(key);
    char *cp = malloc(l + 1);
    if (!cp) return;
    memcpy(cp, key, l + 1);
    m->slots[j].key = cp; m->slots[j].file = file; m->slots[j].line = line;
    m->n++;
}

static int smap_get(const SaveMap *m, const char *key, int *file, int *line) {
    if (!m->cap) return 0;
    size_t j = sm_hash(key) & (m->cap - 1);
    while (m->slots[j].key) {
        if (!strcmp(m->slots[j].key, key)) {
            *file = m->slots[j].file; *line = m->slots[j].line;
            return 1;
        }
        j = (j + 1) & (m->cap - 1);
    }
    return 0;
}

/* L'indice di un percorso, interned: i file sono cento, i fatti settemila. */
static int smap_file(SaveMap *m, const char *path) {
    for (size_t i = 0; i < m->nfiles; i++)
        if (!strcmp(m->files[i], path)) return (int)i;
    if (m->nfiles >= m->fcap) {
        size_t cap = m->fcap ? m->fcap * 2 : 32;
        char **nv = realloc(m->files, cap * sizeof *nv);
        if (!nv) return -1;
        m->files = nv; m->fcap = cap;
    }
    size_t l = strlen(path);
    char *cp = malloc(l + 1);
    if (!cp) return -1;
    memcpy(cp, path, l + 1);
    m->files[m->nfiles] = cp;
    return (int)m->nfiles++;
}

/* La posizione di una clausola appena caricata. Chiamata dal caricatore, che e'
 * l'unico posto dove file e riga si sanno gia' senza rileggere niente.
 *
 * `line` e' l'ULTIMA riga della clausola, non la prima: un inserimento fatto
 * «subito dopo» dev'essere subito dopo la clausola intera. E le REGOLE non
 * entrano — sm_parse le scarta — perche' `animal($X)` nel corpo di una regola
 * non e' una casa per `animal(cow, mammal)`. */
static void smap_note(KB *kb, const char *path, int line, const char *clause) {
    if (!kb || !path || !*path) return;
    char pred[SM_PRED], arg1[SM_ARG];
    if (!sm_parse(clause, pred, arg1)) return;
    int fi = smap_file(&kb->smap, path);
    if (fi < 0) return;
    char key[SM_PRED + SM_ARG + 2];
    smap_put(&kb->smap, pred, fi, line);
    snprintf(key, sizeof key, "%s\x01%s", pred, arg1);
    smap_put(&kb->smap, key, fi, line);
}

/* La casa di un fatto: prima la coppia, poi la sola relazione. Se nessuna delle
 * due risponde, il chiamante usa la ricaduta — ed e' l'unico caso legittimo. */
static int smap_home(const KB *kb, const char *pred, const char *arg0,
                     const char **path, int *line) {
    char key[SM_PRED + SM_ARG + 2], arg1[SM_ARG];
    int fi = -1;
    sm_key(arg0, arg1);
    snprintf(key, sizeof key, "%s\x01%s", pred, arg1);
    if (!smap_get(&kb->smap, key, &fi, line) &&
        !smap_get(&kb->smap, pred, &fi, line)) return 0;
    if (fi < 0 || (size_t)fi >= kb->smap.nfiles) return 0;
    *path = kb->smap.files[fi];
    return 1;
}

int kb_load_clause(KB *kb, const char *text) {
    if (!kb || !text) return 0;
    char buf[KB_CLAUSE_MAX + 1];
    size_t n = strlen(text);
    if (n > KB_CLAUSE_MAX) return 0;
    memcpy(buf, text, n + 1);
    size_t len = n;
    loadbuf_trim(buf, &len);
    if (len > 0 && buf[len - 1] == '.') buf[--len] = '\0';
    loadbuf_trim(buf, &len);
    if (len == 0) return 0;
    return load_clause(kb, "<mock>", ".", buf);
}

int kb_load(KB *kb, const char *path) {
    if (!kb || !path || !*path) return 0;
    FILE *f = fopen(path, "r");
    if (!f) return 0; /* missing file: a no-op */

    /* Resolve the directory of the current file for relative includes. */
    char dir[512] = {0};
    {   const char *slash = strrchr(path, '/');
        if (slash) {
            size_t dlen = (size_t)(slash - path);
            if (dlen < sizeof dir) { memcpy(dir, path, dlen); dir[dlen] = '\0'; }
        }
    }

    /* gen382f — la MACCHINERIA si dichiara per FILE, non per predicato.
     *
     * La regola "il file che introduce il predicato dichiara il suo stato"
     * era giusta ma pagata una riga per predicato, e si dimentica: i commenti
     * del motore registrano quattro fughe (gen275, gen325, gen327, gen372) e
     * gen382e ne ha trovate altre otto in una volta, nascoste da un troncamento.
     * Un meccanismo che perde pezzi ogni volta che il progetto cresce non e'
     * manutenibile.
     *
     * Ora il file lo dice UNA volta, con la clausola `machinery_file.` in testa,
     * e ogni predicato che introduce viene marcato al caricamento. Il dato resta
     * DICHIARATIVO — sono normalissimi fatti machinery/1, interrogabili,
     * insegnabili e ritrattabili a runtime — quindi non e' la partizione
     * congelata al boot che gen374 ha provato e scartato: e' la stessa
     * conoscenza di prima, derivata dalla provenienza invece che ricopiata.
     * machinery/1 scritto a mano resta valido per le eccezioni: un file misto
     * (meta.p0 tiene sia la grammatica interrogativa sia incompatible/2, che e'
     * conoscenza del mondo) non si dichiara e marca solo cio' che serve. */
    size_t n0 = kb->n, r0 = kb->nr;
    char file_attr[KB_MAX_ARGS][KB_TERM_LEN];
    size_t n_file_attr = 0;

    char *clause = NULL;
    size_t len = 0, cap = 0;
    int count = 0;
    int quoted = 0, escaped = 0, comment = 0, depth = 0, oom = 0;
    int discarding = 0;
    int ch;
    /* La riga corrente, per la save-map: il caricatore e' l'unico posto dove la
     * posizione di una clausola si sa senza rileggere niente. */
    int line = 1;
    while ((ch = fgetc(f)) != EOF) {
        char c = (char)ch;
        if (c == '\n') line++;
        if (comment) {
            if (c != '\n') continue;
            comment = 0;
            c = ' ';                             /* do not merge tokens across comment */
        }

        if (quoted) {
            if (escaped) escaped = 0;
            else if (c == '\\') escaped = 1;
            else if (c == '"') quoted = 0;
            if (!discarding) {
                int put = loadbuf_put(&clause, &len, &cap, c);
                if (put < 0) {
                    fprintf(stderr,
                            "kb_load: PARSE ERROR in %s: clause too large (limit %d bytes), dropped without parsing\n",
                            path ? path : "?", KB_CLAUSE_MAX);
                    discarding = 1;
                    len = 0;
                    clause[0] = '\0';
                } else if (put == 0) { oom = 1; break; }
            }
            continue;
        }

        if (c == '%') { comment = 1; continue; }
        if (c == '"') quoted = 1;
        else if (c == '(') depth++;
        else if (c == ')' && depth > 0) depth--;

        if (c == '.' && depth == 0 && !quoted) {
            if (discarding) {
                discarding = 0;
                len = 0;
                if (clause) clause[0] = '\0';
            } else if (clause) {
                loadbuf_trim(clause, &len);
                if (len > 0) {
                    /* Un ATTRIBUTO DI FILE: `:- file_attribute(X).` in testa
                     * vale per ogni predicato che il file introduce. Il motore
                     * non conosce X — non sa cosa sia "machinery" — applica
                     * soltanto la propagazione; QUALE attributo si propaghi e'
                     * conoscenza, quindi domani `:- file_attribute(sperimentale)`
                     * funziona con zero C.
                     *
                     * gen383 (F.): la forma NUDA `file_attribute(X).` era
                     * indistinguibile da un fatto sul mondo — chi legge il file
                     * non poteva sapere che quella riga parla del FILE e non del
                     * dominio. Ora e' una DIRETTIVA, come `:- include(...)`: il
                     * `:-` dice "questa riga istruisce il caricatore". La forma
                     * nuda resta riconosciuta per compatibilita', ma nell'albero
                     * non ne sopravvive nessuna.
                     *
                     * ── L'ATTRIBUTO NON SI EREDITA (gen419, nota di F.) ──────
                     *
                     * Vale per il file che lo dichiara e SOLO per i predicati che
                     * quel file introduce. Un `:- include(altro.p0)` e' una
                     * chiamata RICORSIVA a kb_load, e `file_attr` e' una
                     * variabile locale di quella chiamata: il file incluso parte
                     * con la lavagna pulita, dichiara i propri attributi o non ne
                     * ha nessuno.
                     *
                     * Va detto qui perche' il malinteso opposto era gia' scritto
                     * nell'albero e ha fatto danno: un commento in procedures.p0
                     * sosteneva che non si potesse mettere l'attributo su quel
                     * file «perche' include meta.p0, che contiene anche
                     * conoscenza del mondo». Non e' un rischio che esista —
                     * meta.p0 se lo gestisce da solo — e nel frattempo quella
                     * frase ha convinto chi la leggeva a dichiarare la
                     * macchineria una riga per predicato, che e' il lavoro che
                     * questa direttiva esiste per evitare. */
                    const char *fa = clause;
                    if (strncmp(fa, ":-", 2) == 0) {
                        fa += 2;
                        while (*fa == ' ' || *fa == '\t') fa++;
                    }
                    if (strncmp(fa, "file_attribute(", 15) == 0 &&
                        n_file_attr < KB_MAX_ARGS) {
                        const char *a = fa + 15;
                        size_t al = strcspn(a, ")");
                        if (al > 0 && al < KB_TERM_LEN) {
                            memcpy(file_attr[n_file_attr], a, al);
                            file_attr[n_file_attr][al] = '\0';
                            n_file_attr++;
                        }
                    }
                    else {
                        count += load_clause(kb, path, dir, clause);
                        /* `line` e' la riga del punto che chiude la clausola:
                         * la sua ULTIMA riga, che e' il confine dove un fatto
                         * imparato puo' essere inserito senza spezzare nulla. */
                        smap_note(kb, path, line, clause);
                    }
                }
                len = 0;
                clause[0] = '\0';
            }
            continue;
        }
        if (discarding) continue;

        if (isspace((unsigned char)c) && !quoted) {
            if (len == 0 || isspace((unsigned char)clause[len - 1])) continue;
            c = ' ';
        }
        { int put = loadbuf_put(&clause, &len, &cap, c);
          if (put < 0) {
              fprintf(stderr,
                      "kb_load: PARSE ERROR in %s: clause too large (limit %d bytes), dropped without parsing\n",
                      path ? path : "?", KB_CLAUSE_MAX);
              discarding = 1;
              len = 0;
              clause[0] = '\0';
          } else if (put == 0) { oom = 1; break; } }
    }

    if (oom) {
        fprintf(stderr, "kb_load: PARSE ERROR in %s: out of memory while accumulating clause\n",
                path ? path : "?");
    } else if (!discarding && clause) {
        loadbuf_trim(clause, &len);
        if (len > 0)
            fprintf(stderr,
                    "kb_load: PARSE ERROR in %s: unterminated clause at EOF, dropped: '%s'\n",
                    path ? path : "?", clause);
    }
    free(clause);
    fclose(f);

    /* La PROPAGAZIONE dell'attributo di file: per ogni attributo dichiarato in
     * testa, ogni predicato che il file ha introdotto lo riceve come fatto
     * normale. Il risultato e' indistinguibile da una riga scritta a mano —
     * interrogabile, insegnabile, ritrattabile — quindi non e' la partizione
     * congelata al boot che gen374 ha provato e scartato: e' la stessa
     * conoscenza, derivata dalla provenienza invece che ricopiata. */
    for (size_t a = 0; a < n_file_attr; a++) {
        for (size_t i = n0; i < kb->n; i++) {
            const char *m[] = { kb->facts[i].pred };
            if (!kb_query(kb, file_attr[a], m, 1)) kb_assert(kb, file_attr[a], m, 1);
        }
        for (size_t i = r0; i < kb->nr; i++) {
            const char *m[] = { kb->rules[i].head.pred };
            if (!kb_query(kb, file_attr[a], m, 1)) kb_assert(kb, file_attr[a], m, 1);
        }
    }
    return count;
}

/* ----------------------------------------------------------------------------
 * universal evidence / hypothesis engine (universal-input U1/U3/U7/U8)
 * ------------------------------------------------------------------------- */

/* `kb_match` deliberately caps its result at the caller's buffer.  Evidence
 * discovery, unlike a presentation buffer, must not silently drop a 257th
 * hypothesis/support: retry with a growing buffer until the result no longer
 * fills it.  The KB is finite, so one final retry also disambiguates the exact
 * power-of-two case. */
int kb_match_all(const KB *kb, const char *pred,
                 const char *const *args, size_t argc,
                 char (**out)[KB_TERM_LEN], size_t *nout) {
    if (!out || !nout) return 0;
    *out = NULL;
    *nout = 0;
    size_t cap = 16;
    char (*rows)[KB_TERM_LEN] = NULL;
    for (;;) {
        if (cap > (size_t)-1 / sizeof *rows) {
            free(rows);
            return 0;
        }
        char (*grown)[KB_TERM_LEN] = realloc(rows, cap * sizeof *rows);
        if (!grown) {
            free(rows);
            return 0;
        }
        rows = grown;
        size_t n = kb_match(kb, pred, args, argc, rows, cap);
        if (n < cap) {
            *out = rows;
            *nout = n;
            return 1;
        }
        if (cap > (size_t)-1 / 2) {
            free(rows);
            return 0;
        }
        cap *= 2;
    }
}

static void evidence_atom_text(const char *atom, char *out, size_t outsz) {
    if (!out || outsz == 0) return;
    out[0] = '\0';
    if (!atom) return;
    size_t n = strlen(atom);
    const char *p = atom;
    if (n >= 2 && atom[0] == '"' && atom[n - 1] == '"') {
        p++;
        n -= 2;
    }
    if (n >= outsz) n = outsz - 1;
    memcpy(out, p, n);
    out[n] = '\0';
}

static int evidence_word_char(unsigned char c) {
    return isalnum(c) || c == '_';
}

static int evidence_ci_at(const char *s, const char *needle) {
    for (size_t i = 0; needle[i]; i++) {
        if (!s[i] || tolower((unsigned char)s[i]) !=
                     tolower((unsigned char)needle[i])) return 0;
    }
    return 1;
}

static size_t evidence_ci_find(const char *s, const char *needle, size_t from) {
    if (!s || !needle || !*needle) return (size_t)-1;
    size_t n = strlen(s), m = strlen(needle);
    if (m > n || from > n - m) return (size_t)-1;
    for (size_t i = from; i + m <= n; i++)
        if (evidence_ci_at(s + i, needle)) return i;
    return (size_t)-1;
}

static size_t evidence_keyword_find(const char *s, const char *word, size_t from) {
    size_t n = strlen(s), m = strlen(word), at = from;
    while (m && at <= n) {
        at = evidence_ci_find(s, word, at);
        if (at == (size_t)-1) return at;
        int left = (at == 0) || !evidence_word_char((unsigned char)s[at - 1]);
        int right = (at + m >= n) ||
                    !evidence_word_char((unsigned char)s[at + m]);
        if (left && right) return at;
        at++;
    }
    return (size_t)-1;
}

static size_t evidence_line_prefix_find(const char *s, const char *prefix,
                                        size_t from) {
    size_t n = strlen(s), m = strlen(prefix);
    for (size_t at = 0; at <= n; ) {
        if (at >= from && at + m <= n && evidence_ci_at(s + at, prefix)) return at;
        const char *nl = strchr(s + at, '\n');
        if (!nl) break;
        at = (size_t)(nl - s) + 1;
    }
    return (size_t)-1;
}

static size_t evidence_line_suffix_find(const char *s, const char *suffix,
                                        size_t from) {
    size_t n = strlen(s), m = strlen(suffix);
    for (size_t ls = 0; ls <= n; ) {
        const char *nl = strchr(s + ls, '\n');
        size_t le = nl ? (size_t)(nl - s) : n;
        while (le > ls && (s[le - 1] == '\r' || s[le - 1] == ' ' ||
                           s[le - 1] == '\t')) le--;
        if (le >= ls + m && le - m >= from &&
            strncasecmp(s + le - m, suffix, m) == 0)
            return le - m;
        if (!nl) break;
        ls = (size_t)(nl - s) + 1;
    }
    return (size_t)-1;
}

static int evidence_token_extension(const char *s, const char *ext, size_t from,
                                    size_t *at_out, size_t *len_out) {
    size_t n = strlen(s), el = strlen(ext);
    for (size_t i = from; i < n; ) {
        while (i < n && isspace((unsigned char)s[i])) i++;
        size_t a = i;
        while (i < n && !isspace((unsigned char)s[i])) i++;
        size_t b = i;
        while (b > a && strchr(".,;:!?)]}'\"`", s[b - 1])) b--;
        if (b >= a + el && strncasecmp(s + b - el, ext, el) == 0) {
            if (at_out) *at_out = b - el;
            if (len_out) *len_out = el;
            return 1;
        }
    }
    return 0;
}

static size_t evidence_fence_find(const char *s, const char *tag, size_t from,
                                  size_t *len_out) {
    char pat[KB_TERM_LEN + 4];
    snprintf(pat, sizeof pat, "```%s", tag);
    size_t plen = strlen(pat);
    for (;;) {
        size_t at = evidence_ci_find(s, pat, from);
        if (at == (size_t)-1) return at;
        unsigned char after = (unsigned char)s[at + plen];
        int line_start = at == 0 || s[at - 1] == '\n';
        int tag_end = after == '\0' || after == '\n' || after == '\r' ||
                      after == ' ' || after == '\t';
        if (line_start && tag_end) {
            if (len_out) *len_out = plen;
            return at;
        }
        from = at + 1;
    }
}

static int evidence_quote_at(const char *s, size_t i, size_t n) {
    if (s[i] == '"') return 1;
    if (s[i] != '\'') return 0;
    /* Apostrophes inside words are punctuation, not the start of a quoted
     * region: "don't" must not hide every delimiter that follows it. */
    if (i > 0 && i + 1 < n && isalnum((unsigned char)s[i - 1]) &&
        isalnum((unsigned char)s[i + 1])) return 0;
    for (size_t j = i + 1; j < n && s[j] != '\n'; j++) {
        if (s[j] == '\\' && j + 1 < n) { j++; continue; }
        if (s[j] == '\'') return 1;
    }
    return 0;
}

static size_t evidence_visible_find(const char *s, const char *needle,
                                    size_t from, size_t end) {
    size_t nl = strlen(needle), n = strlen(s);
    if (end > n) end = n;
    if (!nl) return from <= end ? from : (size_t)-1;
    for (size_t i = from; i + nl <= end; ) {
        if (evidence_quote_at(s, i, n)) {
            char q = s[i++];
            while (i < end && s[i] != q) {
                if (s[i] == '\\' && i + 1 < end) i += 2; else i++;
            }
            if (i < end) i++;
            continue;
        }
        if (strncasecmp(s + i, needle, nl) == 0) return i;
        i++;
    }
    return (size_t)-1;
}

/* Match a generic delimiter pair, ignoring quoted regions and backslash escapes.
 * Quote/comment policy can grow as facts later; this fixed primitive deliberately
 * names no language or concrete delimiter. */
static int evidence_balanced_find(const char *s, const char *open,
                                  const char *close, size_t from,
                                  size_t *at_out, size_t *len_out) {
    size_t ol = strlen(open), cl = strlen(close), n = strlen(s);
    if (!ol || !cl || from >= n) return 0;

    /* Symmetric delimiters are non-nesting: the next visible occurrence closes
     * the first. This keeps `delim_pair(pipe, "|", "|")` a valid runtime fact. */
    if (ol == cl && memcmp(open, close, ol) == 0) {
        size_t at = (size_t)-1;
        for (size_t i = from; i < n; ) {
            int mark_quote = ol == 1 && (open[0] == '\'' || open[0] == '"');
            if (i + ol <= n && memcmp(s + i, open, ol) == 0 &&
                (!mark_quote || evidence_quote_at(s, i, n))) {
                at = i;
                break;
            }
            if (!mark_quote && evidence_quote_at(s, i, n)) {
                char q = s[i++];
                while (i < n && s[i] != q) {
                    if (s[i] == '\\' && i + 1 < n) i += 2; else i++;
                }
                if (i < n) i++;
            } else i++;
        }
        if (at == (size_t)-1) return 0;
        for (size_t i = at + ol; i < n; ) {
            if (s[i] == '\\' && i + 1 < n) { i += 2; continue; }
            int mark_quote = ol == 1 && (open[0] == '\'' || open[0] == '"');
            if (!mark_quote && evidence_quote_at(s, i, n)) {
                char q = s[i++];
                while (i < n && s[i] != q) {
                    if (s[i] == '\\' && i + 1 < n) i += 2; else i++;
                }
                if (i < n) i++;
                continue;
            }
            if (i + cl <= n && memcmp(s + i, close, cl) == 0) {
                if (at_out) *at_out = at;
                if (len_out) *len_out = i + cl - at;
                return 1;
            }
            i++;
        }
        return 0;
    }

    size_t at = (size_t)-1;
    for (size_t i = from; i < n; ) {
        if (evidence_quote_at(s, i, n)) {
            char q = s[i++];
            while (i < n && s[i] != q) {
                if (s[i] == '\\' && i + 1 < n) i += 2; else i++;
            }
            if (i < n) i++;
            continue;
        }
        if (i + ol <= n && memcmp(s + i, open, ol) == 0) { at = i; break; }
        i++;
    }
    if (at == (size_t)-1) return 0;
    size_t depth = 0;
    for (size_t i = at; i < n; ) {
        if (evidence_quote_at(s, i, n)) {
            char q = s[i++];
            while (i < n && s[i] != q) {
                if (s[i] == '\\' && i + 1 < n) i += 2;
                else i++;
            }
            if (i < n) i++;
            continue;
        }
        if (i + ol <= n && memcmp(s + i, open, ol) == 0) {
            depth++;
            i += ol;
            continue;
        }
        if (i + cl <= n && memcmp(s + i, close, cl) == 0) {
            if (depth > 0) depth--;
            i += cl;
            if (depth == 0) {
                if (at_out) *at_out = at;
                if (len_out) *len_out = i - at;
                return 1;
            }
            continue;
        }
        i++;
    }
    return 0;
}

static int evidence_indent_find(const char *s, size_t from,
                                size_t *at_out, size_t *len_out) {
    size_t n = strlen(s);
    for (size_t ls = 0; ls < n; ) {
        const char *nl = strchr(s + ls, '\n');
        size_t le = nl ? (size_t)(nl - s) : n;
        size_t next = nl ? le + 1 : n;
        if (next >= n) break;
        size_t ind = 0, nind = 0;
        while (ls + ind < le && (s[ls + ind] == ' ' || s[ls + ind] == '\t')) ind++;
        size_t ne = next;
        while (ne < n && s[ne] != '\n') ne++;
        while (next + nind < ne && (s[next + nind] == ' ' ||
                                     s[next + nind] == '\t')) nind++;
        if (ls >= from && nind > ind && next + nind < ne) {
            if (at_out) *at_out = ls;
            if (len_out) *len_out = ne - ls;
            return 1;
        }
        if (!nl) break;
        ls = le + 1;
    }
    return 0;
}

static int evidence_kind(const char *evidence, char *kind, size_t kindsz,
                         char args[][KB_TERM_LEN], size_t *argc) {
    char pred[KB_TERM_LEN];
    size_t ac = 0;
    int bare_default = evidence && strcmp(evidence, "default") == 0;
    /* A quoted atom is surface text even when the text itself contains parens. */
    if (evidence && evidence[0] != '"' &&
        split_compound(evidence, pred, args, &ac)) {
        snprintf(kind, kindsz, "%s", pred);
        if (argc) *argc = ac;
        return 1;
    }
    evidence_atom_text(evidence, kind, kindsz);
    if (!bare_default) snprintf(kind, kindsz, "cue");
    if (argc) *argc = 0;
    return 0;
}

static int evidence_weight(const KB *kb, const char *kind) {
    char vals[1][KB_TERM_LEN];
    const char *q[2] = { kind, NULL };
    if (kb_match(kb, "evidence_weight", q, 2, vals, 1) == 1) {
        char v[KB_TERM_LEN];
        evidence_atom_text(vals[0], v, sizeof v);
        char *end = NULL;
        errno = 0;
        long w = strtol(v, &end, 10);
        while (end && isspace((unsigned char)*end)) end++;
        if (errno == 0 && end && *end == '\0' && w > 0 && w <= INT_MAX)
            return (int)w;
    }
    return 1;
}

/* Locate the next occurrence of one evidence term at or after `from`. */
static int evidence_next(const KB *kb, const char *evidence, const char *text,
                         size_t from, size_t *at_out, size_t *len_out,
                         int *is_default) {
    char kind[KB_TERM_LEN], a[KB_MAX_ARGS][KB_TERM_LEN];
    size_t ac = 0, at = (size_t)-1, len = 0;
    int compound = evidence_kind(evidence, kind, sizeof kind, a, &ac);
    if (is_default) *is_default = 0;

    if (!strcmp(kind, "default")) {
        if (from > 0) return 0;
        if (is_default) *is_default = 1;
        at = 0;
    } else if (!strcmp(kind, "cue") || !strcmp(kind, "contains")) {
        char needle[KB_TERM_LEN];
        evidence_atom_text(compound && ac ? a[0] : evidence, needle, sizeof needle);
        at = evidence_ci_find(text, needle, from);
        len = strlen(needle);
    } else if (!strcmp(kind, "keyword") && ac >= 1) {
        char word[KB_TERM_LEN]; evidence_atom_text(a[0], word, sizeof word);
        at = evidence_keyword_find(text, word, from); len = strlen(word);
    } else if (!strcmp(kind, "line_prefix") && ac >= 1) {
        char p[KB_TERM_LEN]; evidence_atom_text(a[0], p, sizeof p);
        at = evidence_line_prefix_find(text, p, from); len = strlen(p);
    } else if (!strcmp(kind, "line_suffix") && ac >= 1) {
        char p[KB_TERM_LEN]; evidence_atom_text(a[0], p, sizeof p);
        at = evidence_line_suffix_find(text, p, from); len = strlen(p);
    } else if (!strcmp(kind, "prefix") && ac >= 1) {
        char p[KB_TERM_LEN]; evidence_atom_text(a[0], p, sizeof p);
        if (from == 0 && evidence_ci_at(text, p)) { at = 0; len = strlen(p); }
    } else if (!strcmp(kind, "suffix") && ac >= 1) {
        char p[KB_TERM_LEN]; evidence_atom_text(a[0], p, sizeof p);
        size_t n = strlen(text), pl = strlen(p);
        while (n && isspace((unsigned char)text[n - 1])) n--;
        if (from == 0 && n >= pl && strncasecmp(text + n - pl, p, pl) == 0) {
            at = n - pl; len = pl;
        }
    } else if (!strcmp(kind, "extension") && ac >= 1) {
        char p[KB_TERM_LEN]; evidence_atom_text(a[0], p, sizeof p);
        if (!evidence_token_extension(text, p, from, &at, &len)) at = (size_t)-1;
    } else if (!strcmp(kind, "balanced") && ac >= 1) {
        char open[KB_TERM_LEN] = "", close[KB_TERM_LEN] = "";
        if (ac >= 2) {
            evidence_atom_text(a[0], open, sizeof open);
            evidence_atom_text(a[1], close, sizeof close);
        } else {
            char opens[16][KB_TERM_LEN];
            const char *q[3] = { a[0], NULL, NULL };
            size_t no = kb_match(kb, "delim_pair", q, 3, opens, 16);
            for (size_t i = 0; i < no && !open[0]; i++) {
                char closes[1][KB_TERM_LEN];
                const char *cq[3] = { a[0], opens[i], NULL };
                if (kb_match(kb, "delim_pair", cq, 3, closes, 1) == 1) {
                    evidence_atom_text(opens[i], open, sizeof open);
                    evidence_atom_text(closes[0], close, sizeof close);
                }
            }
        }
        if (!evidence_balanced_find(text, open, close, from, &at, &len))
            at = (size_t)-1;
    } else if (!strcmp(kind, "paired_contains") && ac >= 2) {
        char open[KB_TERM_LEN] = "", close[KB_TERM_LEN] = "";
        char opens[16][KB_TERM_LEN];
        const char *q[3] = { a[0], NULL, NULL };
        size_t no = kb_match(kb, "delim_pair", q, 3, opens, 16);
        for (size_t i = 0; i < no && !open[0]; i++) {
            char closes[1][KB_TERM_LEN]; const char *cq[3] = { a[0], opens[i], NULL };
            if (kb_match(kb, "delim_pair", cq, 3, closes, 1) == 1) {
                evidence_atom_text(opens[i], open, sizeof open);
                evidence_atom_text(closes[0], close, sizeof close);
            }
        }
        size_t bat = 0, blen = 0, scan = from;
        char needle[KB_TERM_LEN]; evidence_atom_text(a[1], needle, sizeof needle);
        at = (size_t)-1;
        while (evidence_balanced_find(text, open, close, scan, &bat, &blen)) {
            size_t hit = evidence_visible_find(text, needle, bat, bat + blen);
            if (hit != (size_t)-1) {
                at = hit; len = strlen(needle);
                break;
            }
            size_t next = bat + (blen ? blen : 1);
            if (next <= scan) break;
            scan = next;
        }
    } else if (!strcmp(kind, "block") && ac >= 1) {
        char mode[KB_TERM_LEN]; evidence_atom_text(a[0], mode, sizeof mode);
        if (strcmp(mode, "indent") != 0 ||
            !evidence_indent_find(text, from, &at, &len)) at = (size_t)-1;
    } else if (!strcmp(kind, "fence") && ac >= 1) {
        char tag[KB_TERM_LEN];
        evidence_atom_text(a[0], tag, sizeof tag);
        at = evidence_fence_find(text, tag, from, &len);
    }

    if (at == (size_t)-1) return 0;
    if (at_out) *at_out = at;
    if (len_out) *len_out = len;
    return 1;
}

static int evidence_match_cmp(const KbEvidenceMatch *a,
                              const KbEvidenceMatch *b) {
    if (a->start != b->start) return a->start < b->start ? -1 : 1;
    int c = strcmp(a->hypothesis, b->hypothesis);
    if (c) return c;
    return strcmp(a->evidence, b->evidence);
}

/* Keep the earliest `max` matches while still scanning every fact and every
 * occurrence.  This makes a small caller buffer independent of KB fact order. */
static void evidence_match_keep(KbEvidenceMatch *out, size_t max, size_t *kept,
                                const KbEvidenceMatch *candidate) {
    size_t n = *kept;
    if (n < max) {
        out[n++] = *candidate;
        *kept = n;
    } else {
        if (evidence_match_cmp(candidate, &out[n - 1]) >= 0) return;
        out[n - 1] = *candidate;
    }
    size_t i = n - 1;
    while (i > 0 && evidence_match_cmp(&out[i], &out[i - 1]) < 0) {
        KbEvidenceMatch tmp = out[i];
        out[i] = out[i - 1];
        out[i - 1] = tmp;
        i--;
    }
}

size_t kb_evidence_matches(const KB *kb, const char *relation,
                           const char *hypothesis, const char *text,
                           KbEvidenceMatch *out, size_t max) {
    if (!kb || !term_ok(relation) || !text || !out || max == 0 ||
        (hypothesis && (!term_ok(hypothesis) || term_contains_var(hypothesis, 0))))
        return 0;
    char (*classes)[KB_TERM_LEN] = NULL;
    size_t nc = 0;
    if (hypothesis) {
        classes = malloc(sizeof *classes);
        if (!classes) return 0;
        snprintf(classes[nc++], KB_TERM_LEN, "%s", hypothesis);
    } else {
        const char *q[2] = { NULL, NULL };
        if (!kb_match_all(kb, relation, q, 2, &classes, &nc)) return 0;
    }

    size_t found = 0;
    for (size_t ci = 0; ci < nc; ci++) {
        /* A rule/unit clause may expose a partially-ground first argument such
         * as f($X).  It is not a class name; feeding it back to kb_match would
         * reinterpret it as a pattern and merge unrelated hypotheses. */
        if (!term_ok(classes[ci]) || term_contains_var(classes[ci], 0)) continue;
        char (*evs)[KB_TERM_LEN] = NULL;
        const char *q[2] = { classes[ci], NULL };
        size_t ne = 0;
        if (!kb_match_all(kb, relation, q, 2, &evs, &ne)) {
            free(classes);
            return 0;
        }
        for (size_t ei = 0; ei < ne; ei++) {
            char kind[KB_TERM_LEN], args[KB_MAX_ARGS][KB_TERM_LEN]; size_t ac = 0;
            evidence_kind(evs[ei], kind, sizeof kind, args, &ac);
            int weight = evidence_weight(kb, kind);
            size_t from = 0;
            for (;;) {
                size_t at = 0, len = 0; int def = 0;
                if (!evidence_next(kb, evs[ei], text, from, &at, &len, &def)) break;
                KbEvidenceMatch candidate;
                memset(&candidate, 0, sizeof candidate);
                snprintf(candidate.hypothesis, KB_TERM_LEN, "%s", classes[ci]);
                snprintf(candidate.evidence, KB_TERM_LEN, "%s", evs[ei]);
                candidate.start = at;
                candidate.len = len;
                candidate.weight = weight;
                evidence_match_keep(out, max, &found, &candidate);
                if (def || at + (len ? len : 1) <= from) break;
                from = at + (len ? len : 1);
            }
        }
        free(evs);
    }
    free(classes);

    /* Stable insertion sort: evidence files are small, while deterministic byte
     * order makes span construction and proof tests independent of fact order. */
    for (size_t i = 1; i < found; i++) {
        KbEvidenceMatch x = out[i];
        size_t j = i;
        while (j > 0 && evidence_match_cmp(&x, &out[j - 1]) < 0) {
            out[j] = out[j - 1];
            j--;
        }
        out[j] = x;
    }
    return found;
}

typedef struct {
    char name[KB_TERM_LEN];
    long long score;
    long long fallback_score;
    int specific;
    char (*support)[KB_TERM_LEN];
    size_t nsupport, capsupport;
    char (*fallback_support)[KB_TERM_LEN];
    size_t nfallback_support, capfallback_support;
} EvidenceHyp;

static int evidence_support_push(char (**rows)[KB_TERM_LEN],
                                 size_t *n, size_t *cap, const char *value) {
    if (*n == *cap) {
        size_t next = *cap ? *cap * 2 : 8;
        if (next < *cap || next > (size_t)-1 / sizeof **rows) return 0;
        char (*grown)[KB_TERM_LEN] = realloc(*rows, next * sizeof **rows);
        if (!grown) return 0;
        *rows = grown;
        *cap = next;
    }
    snprintf((*rows)[(*n)++], KB_TERM_LEN, "%s", value);
    return 1;
}

static void evidence_hypotheses_free(EvidenceHyp *hs, size_t nh) {
    if (!hs) return;
    for (size_t i = 0; i < nh; i++) {
        free(hs[i].support);
        free(hs[i].fallback_support);
    }
    free(hs);
}

static void evidence_score_add(long long *score, int weight) {
    if (*score > LLONG_MAX - weight) *score = LLONG_MAX;
    else *score += weight;
}

typedef struct {
    char *out;
    size_t cap;
    size_t len;
    int truncated;
} EvidenceProofBuf;

static void evidence_proof_mark_truncated(EvidenceProofBuf *p) {
    static const char marker[] = " [truncated]";
    p->truncated = 1;
    if (!p->out || p->cap == 0) return;
    size_t avail = p->cap - 1;
    size_t ml = sizeof marker - 1;
    if (avail >= ml) {
        size_t start = avail - ml;
        memcpy(p->out + start, marker, ml);
        p->out[avail] = '\0';
        p->len = avail;
    } else {
        memcpy(p->out, marker, avail);
        p->out[avail] = '\0';
        p->len = avail;
    }
}

static void evidence_proof_init(EvidenceProofBuf *p, char *out, size_t cap) {
    p->out = out;
    p->cap = cap;
    p->len = 0;
    p->truncated = 0;
    if (out && cap) out[0] = '\0';
}

static void evidence_proof_append(EvidenceProofBuf *p, const char *fmt, ...) {
    if (p->truncated) return;
    if (!p->out || p->cap == 0 || p->len >= p->cap) {
        evidence_proof_mark_truncated(p);
        return;
    }
    va_list ap;
    va_start(ap, fmt);
    int w = vsnprintf(p->out + p->len, p->cap - p->len, fmt, ap);
    va_end(ap);
    if (w < 0 || (size_t)w >= p->cap - p->len) {
        evidence_proof_mark_truncated(p);
        return;
    }
    p->len += (size_t)w;
}

static int evidence_hyp_consume(const KB *kb, EvidenceHyp *h,
                                const char *evidence, const char *text) {
    size_t at, len;
    int def = 0;
    if (!evidence_next(kb, evidence, text, 0, &at, &len, &def)) return 1;
    char kind[KB_TERM_LEN], args[KB_MAX_ARGS][KB_TERM_LEN];
    size_t ac = 0;
    evidence_kind(evidence, kind, sizeof kind, args, &ac);
    int weight = evidence_weight(kb, kind);
    if (def) {
        evidence_score_add(&h->fallback_score, weight);
        return evidence_support_push(&h->fallback_support,
                                     &h->nfallback_support,
                                     &h->capfallback_support, evidence);
    }
    evidence_score_add(&h->score, weight);
    h->specific = 1;
    return evidence_support_push(&h->support, &h->nsupport,
                                 &h->capsupport, evidence);
}

int kb_hypothesis_best(const KB *kb, const char *relation, const char *text,
                       const char *const *candidates, size_t ncandidates,
                       char *winner, size_t winner_sz,
                       int *score, char *proof, size_t proof_sz) {
    if (winner && winner_sz) winner[0] = '\0';
    if (score) *score = 0;
    EvidenceProofBuf pb;
    evidence_proof_init(&pb, proof, proof_sz);
    if (!kb || !term_ok(relation) || !text) return 0;
    if (!candidates && ncandidates) {
        evidence_proof_append(&pb, "gap(%s): missing hypothesis candidates", relation);
        return 0;
    }
    if (candidates && ncandidates) {
        for (size_t i = 0; i < ncandidates; i++) {
            if (!term_ok(candidates[i]) || term_contains_var(candidates[i], 0)) {
                evidence_proof_append(&pb,
                                      "gap(%s): invalid hypothesis candidate",
                                      relation);
                return 0;
            }
        }
    }

    char (*discovered)[KB_TERM_LEN] = NULL;
    size_t nd = 0;
    EvidenceHyp *hs = NULL;
    size_t nh = 0;

    /* Direct two-column evidence tables are the dominant KB-first form
     * (intent_cue, semantic_topic_cue, analysis_*_cue, ...). The generic path
     * historically discovered candidates and then rescanned the whole KB once
     * per candidate. That is semantically sound but O(facts * hypotheses), and
     * under LLMSCORE's eight fresh processes it pushed correct answers over the
     * one-second deadline. When no rule can derive the relation, collect and
     * score all rows in ONE fact pass. Rule-backed/non-ground tables retain the
     * full solver path below; the result and proof contract is unchanged. */
    int direct_fast = (!candidates || ncandidates == 0);
    for (size_t i = 0; i < kb->nr && direct_fast; i++)
        if (kb->rules[i].head.argc == 2 &&
            strcmp(kb->rules[i].head.pred, relation) == 0)
            direct_fast = 0;
    size_t direct_rows = 0;
    for (size_t i = 0; i < kb->n && direct_fast; i++) {
        const Fact *f = &kb->facts[i];
        if (f->argc != 2 || strcmp(f->pred, relation) != 0) continue;
        if (term_contains_var(f->args[0], 0) ||
            term_contains_var(f->args[1], 0)) {
            direct_fast = 0;
            break;
        }
        direct_rows++;
    }

    if (direct_fast) {
        if (direct_rows > (size_t)-1 / sizeof *hs) goto oom;
        if (direct_rows) {
            hs = calloc(direct_rows, sizeof *hs);
            if (!hs) goto oom;
        }
        for (size_t i = 0; i < kb->n; i++) {
            const Fact *f = &kb->facts[i];
            if (f->argc != 2 || strcmp(f->pred, relation) != 0) continue;
            if (!term_ok(f->args[0]) || term_contains_var(f->args[0], 0))
                continue;
            size_t hi = 0;
            while (hi < nh && strcmp(hs[hi].name, f->args[0]) != 0) hi++;
            if (hi == nh) {
                snprintf(hs[nh].name, sizeof hs[nh].name, "%s", f->args[0]);
                nh++;
            }
            if (!evidence_hyp_consume(kb, &hs[hi], f->args[1], text)) goto oom;
        }
    } else {
        if (!candidates || ncandidates == 0) {
            const char *q[2] = { NULL, NULL };
            if (!kb_match_all(kb, relation, q, 2, &discovered, &nd)) {
                evidence_proof_append(&pb, "gap(%s): evidence discovery failed",
                                      relation);
                return 0;
            }
        }
        size_t total = candidates && ncandidates ? ncandidates : nd;
        if (total > (size_t)-1 / sizeof *hs) goto oom;
        if (total) {
            hs = calloc(total, sizeof *hs);
            if (!hs) goto oom;
        }
        for (size_t ci = 0; ci < total; ci++) {
            const char *name =
                candidates && ncandidates ? candidates[ci] : discovered[ci];
            if (!term_ok(name) || term_contains_var(name, 0)) continue;
            int seen = 0;
            for (size_t j = 0; j < nh; j++)
                if (!strcmp(hs[j].name, name)) seen = 1;
            if (seen) continue;
            EvidenceHyp *h = &hs[nh++];
            snprintf(h->name, sizeof h->name, "%s", name);

            char (*evs)[KB_TERM_LEN] = NULL;
            const char *q[2] = { name, NULL };
            size_t ne = 0;
            if (!kb_match_all(kb, relation, q, 2, &evs, &ne)) goto oom;
            for (size_t ei = 0; ei < ne; ei++) {
                if (!evidence_hyp_consume(kb, h, evs[ei], text)) {
                    free(evs);
                    goto oom;
                }
            }
            free(evs);
        }
    }

    int any_specific = 0;
    for (size_t i = 0; i < nh; i++) if (hs[i].specific) any_specific = 1;
    if (!any_specific) {
        for (size_t i = 0; i < nh; i++) {
            hs[i].score = hs[i].fallback_score;
            free(hs[i].support);
            hs[i].support = hs[i].fallback_support;
            hs[i].nsupport = hs[i].nfallback_support;
            hs[i].capsupport = hs[i].capfallback_support;
            hs[i].fallback_support = NULL;
            hs[i].nfallback_support = 0;
            hs[i].capfallback_support = 0;
        }
    }
    long long top = 0; size_t ntop = 0, topidx = 0;
    for (size_t i = 0; i < nh; i++) {
        if (any_specific && !hs[i].specific) continue;
        if (hs[i].score <= 0) continue;
        if (hs[i].score > top) { top = hs[i].score; ntop = 1; topidx = i; }
        else if (hs[i].score == top) ntop++;
    }

    if (top == 0) {
        evidence_proof_append(&pb, "gap(%s): no evidence matched", relation);
        evidence_hypotheses_free(hs, nh);
        free(discovered);
        return 0;
    }
    if (score) *score = top > INT_MAX ? INT_MAX : (int)top;

    if (ntop > 1) {
        if (proof && proof_sz) {
            evidence_proof_append(&pb, "ambiguous_hypothesis(%s): ", relation);
            int first = 1;
            for (size_t i = 0; i < nh; i++) {
                if ((any_specific && !hs[i].specific) || hs[i].score != top) continue;
                evidence_proof_append(&pb, "%s%s=%lld because ",
                                      first ? "" : "; ", hs[i].name, hs[i].score);
                for (size_t k = 0; k < hs[i].nsupport && !pb.truncated; k++)
                    evidence_proof_append(&pb, "%s%s(%s, %s)",
                                          k ? " + " : "", relation,
                                          hs[i].name, hs[i].support[k]);
                first = 0;
                if (pb.truncated) break;
            }
        }
        evidence_hypotheses_free(hs, nh);
        free(discovered);
        return -1;
    }

    EvidenceHyp *best = &hs[topidx];
    if (winner && winner_sz) snprintf(winner, winner_sz, "%s", best->name);
    if (proof && proof_sz) {
        evidence_proof_append(&pb, "because ");
        for (size_t i = 0; i < best->nsupport && !pb.truncated; i++)
            evidence_proof_append(&pb, "%s%s(%s, %s)", i ? " + " : "",
                                  relation, best->name, best->support[i]);
        if (!pb.truncated)
            evidence_proof_append(&pb, " [score=%lld]", best->score);
    }
    evidence_hypotheses_free(hs, nh);
    free(discovered);
    return 1;

oom:
    evidence_hypotheses_free(hs, nh);
    free(discovered);
    evidence_proof_init(&pb, proof, proof_sz);
    evidence_proof_append(&pb, "gap(%s): evidence collection out of memory", relation);
    return 0;
}

static void write_term(FILE *f, const Term *t) {
    fprintf(f, "%s(", t->pred);
    for (size_t i = 0; i < t->argc; i++) {
        fprintf(f, "%s%s", i ? ", " : "", t->args[i]);
    }
    fputc(')', f);
}

int kb_save(const KB *kb, const char *path, int origin_mask) {
    if (!kb || !path || !*path) return -1;
    FILE *f = fopen(path, "w");
    if (!f) return -1;

    int count = 0;
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *fa = &kb->facts[i];
        if (!(fa->origin & origin_mask)) continue;
        fprintf(f, "%s(", fa->pred);
        for (size_t j = 0; j < fa->argc; j++) {
            fprintf(f, "%s%s", j ? ", " : "", fa->args[j]);
        }
        fprintf(f, ").\n");
        count++;
    }
    for (size_t i = 0; i < kb->nn; i++) {
        const Fact *fa = &kb->neg[i];
        if (!(fa->origin & origin_mask)) continue;
        fprintf(f, "not(%s(", fa->pred);
        for (size_t j = 0; j < fa->argc; j++) {
            fprintf(f, "%s%s", j ? ", " : "", fa->args[j]);
        }
        fprintf(f, ")).\n");
        count++;
    }
    for (size_t i = 0; i < kb->nr; i++) {
        const Rule *r = &kb->rules[i];
        if (!(r->origin & origin_mask)) continue;
        write_term(f, &r->head);
        fprintf(f, " :- ");
        for (size_t j = 0; j < r->nbody; j++) {
            if (j) fprintf(f, ", ");
            if (r->body[j].neg) {           /* U6: round-trip the naf wrapper */
                fprintf(f, "naf(");
                write_term(f, &r->body[j]);
                fputc(')', f);
            } else {
                write_term(f, &r->body[j]);
            }
        }
        fprintf(f, ".\n");
        count++;
    }
    fclose(f);
    return count;
}

/* ----------------------------------------------------------------------------
 * save-map: mettere un fatto nuovo accanto ai suoi parenti
 * ----------------------------------------------------------------------------
 * Quando la conoscenza nuova si persiste (MCP kb.save, /save, il sogno), invece
 * di ammucchiarla in un file unico ogni fatto ground va nel file curato che
 * tiene gia' i suoi parenti. Non e' tracciamento di provenienza ed e' per
 * progetto approssimativo: l'obiettivo e' la PROSSIMITA' fra fatti simili, non
 * l'ordine esatto delle righe.
 *
 * La coordinata di un fatto e' (predicato, primo argomento), la risposta e'
 * (file, riga). La mappa vive in RAM ed e' costruita CARICANDO — vedi SaveMap
 * in testa al file. I gradi sono due e bastano:
 *
 *   1. la coppia (pred, arg1)  — la stessa cosa detta dello stesso soggetto
 *   2. il solo pred            — la casa della relazione
 *   3. altrimenti la ricaduta, ed e' l'unico caso in cui learned.p0 e' giusto.
 */
#define SM_PATH 512

/* Il primo confine di clausola da `after` in poi.
 *
 * `kb_load` non legge per righe: accumula caratteri e chiude la clausola al
 * punto a profondita' zero fuori dalle virgolette, trattando il fine-riga come
 * uno spazio qualunque. Una clausola su piu' righe percio' e' legale, e
 * nell'albero ce ne sono (le regole di context-scope.p0, le quartine di
 * responses.p0: 336 righe di continuazione). Inserire un fatto in mezzo a una
 * di quelle non da' errore di sintassi — cambia solo cio' che la regola dice, e
 * nessuno se ne accorge:
 *
 *     noisy($X) :-            noisy($X) :-
 *         animal($X),             animal($X),
 *         has_sound($X, $S).  animal(cow, mammal).      <- il fatto instradato
 *                                 has_sound($X, $S).    <- ora e' un fatto a se'
 *
 * La guardia sta qui, nello scrittore, e non nella mappa: il file lo stiamo
 * leggendo comunque per deduplicare, quindi sapere dove finiscono le clausole
 * non costa niente. Se la riga puntata cade dentro una clausola si va alla sua
 * fine. La mappa resta ferma e approssimativa. */
static int sm_safe_after(char *const *lines, size_t n, int after) {
    int in_clause = 0, in_quote = 0;
    for (size_t i = 0; i < n; i++) {
        const char *s = lines[i];
        while (*s && isspace((unsigned char)*s)) s++;
        if (!in_clause) {
            if (*s == '\0' || *s == '%') { if ((int)(i + 1) >= after) return (int)(i + 1); continue; }
            in_clause = 1;
        } else if (*s == '%') {
            continue;              /* un commento in mezzo non chiude nulla */
        }
        int dot = 0;
        for (const char *p = lines[i]; *p; p++) {
            if (*p == '"') { in_quote = !in_quote; dot = 0; continue; }
            if (in_quote) continue;
            if (*p == '.') dot = 1;
            else if (!isspace((unsigned char)*p)) dot = 0;
        }
        if (in_quote || !dot) continue;                 /* la clausola continua */
        in_clause = 0;
        if ((int)(i + 1) >= after) return (int)(i + 1);
    }
    return (int)n;
}

/* Insert `text` (a full "fact." line, no newline) into `file` right after 1-based
 * line `after`. If the exact line already exists anywhere in the file, do nothing
 * (dedup).
 *
 * Ritorna 0 se non ha potuto, 1 se ha INSERITO, 2 se il fatto era gia' a casa.
 *
 * LA RIGA PUNTATA PUO' ESSERE VECCHIA, e va bene: la mappa non si corregge dopo
 * un inserimento, quindi il secondo fatto instradato nello stesso file mira a
 * una riga slittata di uno. Non e' un problema — resta accanto ai suoi parenti,
 * ed e' quello che si vuole. L'UNICA cosa che non deve succedere e' cadere in
 * mezzo a una clausola, e per quella basta guardare il file mentre lo si legge
 * comunque: vedi sm_safe_after. */
/* Due percorsi possono nominare lo STESSO file: `kb/learning/learned.p0` e
 * `kb/profiles/../learning/learned.p0` sono la ricaduta, scritta una volta dal
 * chiamante e una volta dalla mappa che l'ha risolta a partire dal profilo. Il
 * confronto per stringa li diceva diversi, e kb_save_routed ne concludeva che il
 * fatto avesse trovato casa ALTROVE: lo inseriva, poi lo toglieva dalla ricaduta
 * e riscriveva il file senza di lui. Il fatto spariva dopo essere stato contato.
 * L'identita' di un file la dice il filesystem, non il testo del percorso. */
static int sm_same_file(const char *a, const char *b) {
    if (!a || !b) return 0;
    if (!strcmp(a, b)) return 1;
    struct stat sa, sb;
    if (stat(a, &sa) || stat(b, &sb)) return 0;
    return sa.st_dev == sb.st_dev && sa.st_ino == sb.st_ino;
}

static int sm_insert(const char *file, int after, const char *text) {
    FILE *f = fopen(file, "r");
    if (!f) return 0;
    char **lines = NULL; size_t n = 0, cap = 0; char buf[2048];
    while (fgets(buf, sizeof buf, f)) {
        char t[2048]; snprintf(t, sizeof t, "%s", buf);
        size_t tl = strlen(t);
        while (tl && (t[tl - 1] == '\n' || t[tl - 1] == '\r' || t[tl - 1] == ' ')) t[--tl] = '\0';
        if (!strcmp(t, text)) {
            for (size_t i = 0; i < n; i++) free(lines[i]);
            free(lines); fclose(f); return 2;                 /* already home */
        }
        if (n >= cap) { cap = cap ? cap * 2 : 128;
            char **nl = realloc(lines, cap * sizeof *lines);
            if (!nl) { for (size_t i = 0; i < n; i++) free(lines[i]); free(lines); fclose(f); return 0; }
            lines = nl;
        }
        size_t bl = strlen(buf);
        lines[n] = malloc(bl + 1);
        if (!lines[n]) { for (size_t i = 0; i < n; i++) free(lines[i]); free(lines); fclose(f); return 0; }
        memcpy(lines[n], buf, bl + 1); n++;
    }
    fclose(f);
    if (after < 0 || (size_t)after > n) after = (int)n;
    after = sm_safe_after(lines, n, after);
    FILE *o = fopen(file, "w");
    if (!o) { for (size_t i = 0; i < n; i++) free(lines[i]); free(lines); return 0; }
    int inserted = 0;
    for (size_t i = 0; i < n; i++) {
        size_t bl = strlen(lines[i]);
        if (bl && lines[i][bl - 1] != '\n') { fputs(lines[i], o); fputc('\n', o); }
        else fputs(lines[i], o);
        if ((int)(i + 1) == after) { fprintf(o, "%s\n", text); inserted = 1; }
        free(lines[i]);
    }
    if (!inserted) fprintf(o, "%s\n", text);   /* empty file / insert-at-top */
    free(lines);
    fclose(o);
    return 1;
}

/* Un elenco di righe in memoria: serve a trattare il file di ricaduta come un
 * testo da RISCRIVERE (leggi, decidi, riscrivi) invece che come un flusso a cui
 * accodare alla cieca. Vedi kb_save_routed per il perche'. */
typedef struct { char **v; size_t n, cap; } SmLines;

static int sml_push(SmLines *L, const char *s) {
    if (L->n >= L->cap) {
        size_t cap = L->cap ? L->cap * 2 : 128;
        char **nv = realloc(L->v, cap * sizeof *nv);
        if (!nv) return 0;
        L->v = nv; L->cap = cap;
    }
    size_t sl = strlen(s);
    L->v[L->n] = malloc(sl + 1);
    if (!L->v[L->n]) return 0;
    memcpy(L->v[L->n], s, sl + 1);
    L->n++;
    return 1;
}

static int sml_has(const SmLines *L, const char *s) {
    for (size_t i = 0; i < L->n; i++) if (!strcmp(L->v[i], s)) return 1;
    return 0;
}

static void sml_free(SmLines *L) {
    for (size_t i = 0; i < L->n; i++) free(L->v[i]);
    free(L->v); L->v = NULL; L->n = L->cap = 0;
}

/* Le righe del file, senza il fine-riga: il confronto fra «cio' che c'e' gia'» e
 * «cio' che si vuole scrivere» dev'essere fra testi, non fra buffer. */
static void sml_read(SmLines *L, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char buf[2048];
    while (fgets(buf, sizeof buf, f)) {
        size_t l = strlen(buf);
        while (l && (buf[l - 1] == '\n' || buf[l - 1] == '\r')) buf[--l] = '\0';
        if (!sml_push(L, buf)) break;
    }
    fclose(f);
}

/* write_term, ma su stringa: la ricaduta si compone in memoria prima di essere
 * confrontata con cio' che il file contiene gia'. */
static size_t sm_term_str(const Term *t, char *out, size_t sz) {
    size_t o = (size_t)snprintf(out, sz, "%s(", t->pred);
    for (size_t i = 0; i < t->argc && o < sz; i++)
        o += (size_t)snprintf(out + o, sz - o, "%s%s", i ? ", " : "", t->args[i]);
    if (o < sz) o += (size_t)snprintf(out + o, sz - o, ")");
    return o;
}

static void sm_fact_text(const Fact *fa, char *out, size_t sz) {
    size_t o = (size_t)snprintf(out, sz, "%s(", fa->pred);
    for (size_t j = 0; j < fa->argc && o < sz; j++)
        o += (size_t)snprintf(out + o, sz - o, "%s%s", j ? ", " : "", fa->args[j]);
    if (o < sz) snprintf(out + o, sz - o, ").");
}

/* Persist SESSION|INDUCED knowledge, ROUTING each ground fact to its kin file.
 * Positive facts with a home are inserted in place; everything else (unrouted
 * facts, negatives, rules) is written to `default_path` (rewritten). Returns the
 * clause count. The on-disk index at `<root>/savemap.tsv` is refreshed for
 * inspection. */
/* Lo scratch del turno non e' patrimonio: `turn_counter/1` e i token dell'ultima
 * frase descrivono il mezzo turno in corso, e salvarli significa farli rientrare
 * al boot come conoscenza — un secondo orologio dentro `turn_bookkeeping`. Quali
 * predicati siano effimeri lo dice la KB (`turn_scratch/1` in discourse.p0), non
 * un elenco qui: uno nuovo domani e' una riga di conoscenza. */
static int sm_is_turn_scratch(const KB *kb, const char *pred) {
    const char *a[1] = { pred };
    return kb_query((KB *)kb, "turn_scratch", a, 1);
}

int kb_save_routed(const KB *kb, const char *default_path, const char *root) {
    if (!kb || !default_path || !*default_path) return -1;
    /* `root` non serve piu' a trovare le case — la mappa e' in RAM — ma resta il
     * posto dove si deposita l'indice ispezionabile. */

    int count = 0;
    char *routed = calloc(kb->n ? kb->n : 1, 1);
    if (!routed) return kb_save(kb, default_path, KB_SESSION | KB_INDUCED);

    /* I fatti che hanno trovato una casa ALTROVE: servono dopo, per togliere
     * dalla ricaduta cio' che nel frattempo ha imparato dove stare. */
    SmLines homed = {0};
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *fa = &kb->facts[i];
        if (!(fa->origin & (KB_SESSION | KB_INDUCED)) || fa->argc == 0) continue;
        if (sm_is_turn_scratch(kb, fa->pred)) { routed[i] = 1; continue; }
        const char *file = NULL; int line = 0;
        if (!smap_home(kb, fa->pred, fa->args[0], &file, &line)) continue;
        char text[2048];
        sm_fact_text(fa, text, sizeof text);
        if (!sm_insert(file, line, text)) continue;
        routed[i] = 1; count++;
        if (!sm_same_file(file, default_path)) sml_push(&homed, text);
    }

    /* ── LA RICADUTA SI RISCRIVE ─────────────────────────────────────────────
     *
     * Qui finisce solo cio' che la mappa non sa collocare: fatti di predicati
     * che l'albero non ha mai visto, negativi e regole. E' il caso in cui
     * `learned.p0` e' la risposta giusta — l'unico.
     *
     * Due errori gia' commessi su questo file, per non rifarli:
     *
     *  - aprirlo in "w": un `/save` qualunque lo riscriveva da zero e cancellava
     *    tutto cio' che le sessioni precedenti avevano depositato li'. Misurato
     *    mentre accadeva: 1403 righe -> 14. L'intenzione era «riscrivere
     *    l'istantanea della sessione», che sarebbe giusta per un file di
     *    sessione — ma questo e' un accumulatore, e un accumulatore non si
     *    tronca;
     *  - aprirlo in "a" e basta: nessuna perdita, ma ogni giro riappendeva cio'
     *    che era gia' dentro e il file cresceva di doppioni a ogni salvataggio.
     *
     * La forma giusta e' la terza: si rilegge cio' che c'e', si TOGLIE cio' che
     * nel frattempo ha trovato una casa vera (l'albero cresce, e un fatto che
     * ieri non aveva parenti oggi puo' averne), si aggiunge solo cio' che manca,
     * si riscrive il file intero. Cosi' la ricaduta resta cio' che dice di
     * essere: il deposito di cio' che non ha ancora un posto. */
    SmLines keep = {0};
    sml_read(&keep, default_path);
    if (homed.n) {
        size_t w = 0;
        for (size_t i = 0; i < keep.n; i++) {
            if (sml_has(&homed, keep.v[i])) { free(keep.v[i]); continue; }
            keep.v[w++] = keep.v[i];
        }
        keep.n = w;
    }

    size_t kept = keep.n;
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *fa = &kb->facts[i];
        if (!(fa->origin & (KB_SESSION | KB_INDUCED)) || routed[i]) continue;
        char text[2048]; sm_fact_text(fa, text, sizeof text);
        if (!sml_has(&keep, text)) sml_push(&keep, text);
        count++;
    }
    for (size_t i = 0; i < kb->nn; i++) {
        const Fact *fa = &kb->neg[i];
        if (!(fa->origin & (KB_SESSION | KB_INDUCED))) continue;
        char text[2048];
        size_t o = (size_t)snprintf(text, sizeof text, "not(%s(", fa->pred);
        for (size_t j = 0; j < fa->argc && o < sizeof text; j++)
            o += (size_t)snprintf(text + o, sizeof text - o, "%s%s", j ? ", " : "", fa->args[j]);
        if (o < sizeof text) snprintf(text + o, sizeof text - o, ")).");
        if (!sml_has(&keep, text)) sml_push(&keep, text);
        count++;
    }
    for (size_t i = 0; i < kb->nr; i++) {
        const Rule *r = &kb->rules[i];
        if (!(r->origin & (KB_SESSION | KB_INDUCED))) continue;
        char text[2048];
        size_t o = sm_term_str(&r->head, text, sizeof text);
        if (o < sizeof text) o += (size_t)snprintf(text + o, sizeof text - o, " :- ");
        for (size_t j = 0; j < r->nbody && o < sizeof text; j++) {
            if (j) o += (size_t)snprintf(text + o, sizeof text - o, ", ");
            if (o >= sizeof text) break;
            if (r->body[j].neg) {
                o += (size_t)snprintf(text + o, sizeof text - o, "naf(");
                o += sm_term_str(&r->body[j], text + o, sizeof text - o);
                if (o < sizeof text) o += (size_t)snprintf(text + o, sizeof text - o, ")");
            } else {
                o += sm_term_str(&r->body[j], text + o, sizeof text - o);
            }
        }
        if (o < sizeof text) snprintf(text + o, sizeof text - o, ".");
        if (!sml_has(&keep, text)) sml_push(&keep, text);
        count++;
    }

    /* Si riscrive solo se qualcosa e' davvero cambiato: un salvataggio che non
     * ha niente da dire non deve toccare il file. */
    if (keep.n != kept || homed.n) {
        FILE *df = fopen(default_path, "w");
        if (df) {
            for (size_t i = 0; i < keep.n; i++) fprintf(df, "%s\n", keep.v[i]);
            fclose(df);
        }
    }

    /* L'indice ispezionabile: la mappa cosi' com'e' in RAM, su disco. Non viene
     * mai riletta — serve a un umano per rispondere alla sola domanda che conta
     * su un instradamento, «perche' e' finito li'». */
    char idx[SM_PATH];
    if (root && *root &&
        (size_t)snprintf(idx, sizeof idx, "%s/savemap.tsv", root) < sizeof idx) {
        FILE *ix = fopen(idx, "w");
        if (ix) {
            for (size_t i = 0; i < kb->smap.cap; i++) {
                const SmSlot *sl = &kb->smap.slots[i];
                if (!sl->key || sl->file < 0 || (size_t)sl->file >= kb->smap.nfiles) continue;
                const char *sep = strchr(sl->key, '\x01');
                if (sep) fprintf(ix, "%.*s\t%s\t%s\t%d\n", (int)(sep - sl->key), sl->key,
                                 sep + 1, kb->smap.files[sl->file], sl->line);
                else     fprintf(ix, "%s\t*\t%s\t%d\n", sl->key,
                                 kb->smap.files[sl->file], sl->line);
            }
            fclose(ix);
        }
    }

    sml_free(&keep);
    sml_free(&homed);
    free(routed);
    return count;
}


/* ----------------------------------------------------------------------------
 * direct belief reports (gen18)
 * ------------------------------------------------------------------------- */

static int fact_mentions(const Fact *f, const char *entity) {
    for (size_t i = 0; i < f->argc; i++) {
        if (strcmp(f->args[i], entity) == 0) return 1;
    }
    return 0;
}

static void render_fact_direct(const Fact *f, const char *entity, int neg,
                                char *buf, size_t sz) {
    /* gen74: i_am(X) means X asserts its own identity — render "X is X." */
    if (strcmp(f->pred, "i_am") == 0 && f->argc == 1 &&
        strcmp(f->args[0], entity) == 0) {
        snprintf(buf, sz, "%s is %s", entity, entity);
        return;
    }
    if (f->argc == 1 && strcmp(f->args[0], entity) == 0) {
        snprintf(buf, sz, "%s is %sa %s", entity, neg ? "not " : "", f->pred);
        return;
    }

    /* gen151/gen152: description-bearing knowledge facts whose LAST argument is a
     * quoted string — the gen150 expert/skill convention pred(key, "human text")
     * and the 3-ary pred(key, category, "human text"). Speak the description
     * instead of dumping the raw clause: math_op(addition, "combining ...") ->
     * "addition is combining ...". General over every domain's description facts,
     * so held-out concepts verbalize through the same path (no phrasebook). */
    if (f->argc >= 2 && f->args[f->argc - 1][0] == '"') {
        char d[KB_TERM_LEN];
        snprintf(d, sizeof d, "%s", f->args[f->argc - 1]);
        size_t dl = strlen(d);
        if (dl > 0 && d[dl - 1] == '"') d[--dl] = '\0';
        const char *desc = (d[0] == '"') ? d + 1 : d;
        snprintf(buf, sz, "%s%s is %s", neg ? "not " : "", f->args[0], desc);
        return;
    }

    int off = snprintf(buf, sz, "%s%s(", neg ? "not " : "", f->pred);
    for (size_t i = 0; i < f->argc && off > 0 && (size_t)off < sz; i++) {
        off += snprintf(buf + off, sz - (size_t)off, "%s%s",
                        i ? ", " : "", f->args[i]);
    }
    if (off > 0 && (size_t)off < sz) snprintf(buf + off, sz - (size_t)off, ")");
}

static void render_conflict_direct(const Fact *f, const char *entity,
                                   char *buf, size_t sz) {
    if (f->argc == 1 && strcmp(f->args[0], entity) == 0) {
        snprintf(buf, sz, "%s is conflicted about being a %s", entity, f->pred);
        return;
    }

    int off = snprintf(buf, sz, "conflict: %s(", f->pred);
    for (size_t i = 0; i < f->argc && off > 0 && (size_t)off < sz; i++) {
        off += snprintf(buf + off, sz - (size_t)off, "%s%s",
                        i ? ", " : "", f->args[i]);
    }
    if (off > 0 && (size_t)off < sz) snprintf(buf + off, sz - (size_t)off, ")");
}

static int append_piece(char *out, size_t out_size, size_t *off,
                        const char *piece) {
    if (*off >= out_size) return 0;
    int n = snprintf(out + *off, out_size - *off, "%s%s",
                     *off ? "; " : "", piece);
    if (n < 0) return 0;
    if ((size_t)n >= out_size - *off) {
        out[out_size - 1] = '\0';
        return 0;
    }
    *off += (size_t)n;
    return 1;
}

/* Continuation transitions (cont/cont2) are generative-model machinery, not
 * world beliefs about an entity (gen41): a passage fed to `read:` populates the
 * KB with both. Introspection ("what do you know about x?") should report
 * knowledge, not the language model's internals, so these are filtered out. */
static int is_model_pred(const KB *kb, const char *pred) {
    /* New machinery declares itself beside the facts that introduce it.  This
     * keeps input/intent evidence out of concept similarity and descriptions
     * without growing another distant C whitelist for every learned schema. */
    if (kb) {
        const char *m[] = { pred };
        if (kb_query((KB *)kb, "machinery", m, 1)) return 1;
    }
    return strcmp(pred, "cont") == 0 || strcmp(pred, "cont2") == 0 ||
           strcmp(pred, "module") == 0 ||
           strcmp(pred, "stopword") == 0 || strcmp(pred, "question_word") == 0 ||
           strcmp(pred, "reaction_word") == 0 || strcmp(pred, "social_marker") == 0 ||
           strcmp(pred, "social_pattern") == 0 ||
           strcmp(pred, "cmd") == 0 || strcmp(pred, "flag") == 0 ||
           /* gen275: dispatch vocabulary and reply phrasings are the language
            * model's internals too — as the cue-chain migrations move trigger
            * words from C into intent_cue facts, they must not masquerade as
            * concept descriptions in kb_nearest_concept (their cue strings
            * inflated the idf pass until a real recall-by-paraphrase abstained). */
           strcmp(pred, "intent_cue") == 0 || strcmp(pred, "goal_cue") == 0 ||
           strcmp(pred, "response_template") == 0 ||
           strcmp(pred, "plan_param") == 0 || strcmp(pred, "lookup_call") == 0 ||
           strcmp(pred, "codebase_lookup") == 0 || strcmp(pred, "learnable") == 0 ||
           /* gen325: the capability LEDGER (kb/core/capabilities.p0) is the
            * agent's model of ITSELF — how far each faculty reaches and what
            * blocks the next level — exactly like module/2. It is not knowledge
            * about the world, so a hermetic brain that has been taught nothing
            * must still answer "I know 0 facts": otherwise the self-model
            * masquerades as world knowledge, the pollution gen275 named. */
           strcmp(pred, "capability") == 0 || strcmp(pred, "capability_wall") == 0 ||
           /* gen335d: knowledge-gap bridge predicates are session-state machinery,
            * not world knowledge — filter from concept descriptions and fact counts
            * like last_result/1 and user_value/2. */
           strcmp(pred, "pending_gap") == 0 ||
           strcmp(pred, "pending_gap_question") == 0 ||
           strcmp(pred, "pending_gap_failed") == 0;
}

/* gen151: structural metadata predicates — registry/relation plumbing from the
 * gen150 expert/skill/profile architecture and the coding substrate. They carry
 * no conversational content about an entity (they would render as raw clauses
 * like expert_domain(arithmetic, mathematics)), so a "what is X?" description
 * skips them and speaks only the content facts. */
static int is_struct_pred(const char *pred) {
    static const char *const s[] = {
        "expert", "expert_domain", "skill", "skill_domain",
        "profile", "profile_domain", "profile_description",
        "expert_description", "skill_description",
        "code_action", "code_template", "code_target", "code_pattern",
        "language", "keyword", "ctype", "py_builtin", "c_stdlib", "c_header",
        "compiled_language", "interpreted_language", "paradigm", "typed",
        "data_structure", "complexity", "faster_than",
        "fix", "fix_suggestion", "review_check", "review_pattern",
        "tr", "gender", "trait", "family_relation", /* gen295: kinship class */
        "concept_gloss", /* gen344: localized definition — spoken by the language-aware
                          * definitional path, NOT a second English definition of the key */
        "learning_event", "learning_event_concept", "learning_event_time",
        /* gen344: autolearn bookkeeping (learned.p0) — internal provenance, must
         * not leak into a "what is X" concept dump as if it described X */
        "tr_es_phrase", "tr_fr_phrase", /* gen310: phrase translation units */
        "fact_source", "answer_frame", "aggregate_frame",
        /* gen286/gen287/gen288/gen289 (U5): grammar glue (grammar.p0), not concepts */
        "article", "fem", "agree_f", "swap_last", "article_fr", "article_es",
        "aux_progressive", "progressive", "ends_ing",
        /* gen307 (U5): FR object-clitic placement + string-concat machinery */
        "app", "glue", "clitic_obj_fr", "elide_fr", "vowel_fr",
        "vowel_initial_fr", "clitic_join",
        "part_of", /* gen158: a derived relation, not a describable concept */
        "category_member", /* gen230: mod_namestart substrate, not describable */
        "opposite", /* gen231: antonym relation, queried not described */
        "color_of", /* gen231: colour facts, queried not described */
        "because", "explanation", "qa_cue", "qa_reply", "verb_syn", "causal_process_verb", "time_unit", "count_of", "created_by", "creation_verb", /* gen232/349: causal reasons + verb-synonym/trigger/unit/count + universal created_by maps, queried not described */
        "grows_with", "increases", /* gen233: qualitative-change substrate */
        "capital_of_country", "kind_is", "borders", "no_land_border",
        "landmark_of", "planet_superlative", "planet_superlative_cue",
        "world_superlative", "world_superlative_cue",
        "element_type", "heritage_built_by", "grammar_error_correction",
        "emotion_signal", "emotion_inference", "creative_text", "creative_text_cue",
        "creative_response",
        "semantic_alias", "semantic_topic_cue", "semantic_summary",
        "answer_projection", "projection_source", "topic_noise",
        "unique_trait", "measure", "compare_cue", "entity_alias",
        "distance_between", /* gen240/gen251: queried world commons */
        "scene_cue", "continuation_template",
        "synesthetic_taste", "synesthetic_default",
        "opening_scene_cue", "opening_line", "past_participle",
        "tr_es", "gender_es", "tr_fr", "gender_fr", "very_cold_result",
        "historical_figure", "figure_domain", "figure_reason",
        "paint_mix", /* gen239: curated world commons, not describable */
        "haiku_open", "haiku_mid", "haiku_close", /* gen240: poetic image lines */
        "couplet", /* gen240: two-line poems, queried not described */
        "quantity", /* gen240: known-fact counts, queried not described */
        "default_color", /* gen240: parrot0's offered pick, not a world fact */
        "default_pick", "landmark_city", /* gen240 */
        "magnitude", "magnitude_cue", "difference_between", "sound_of", /* gen240 */
        "language_marker", "language_name", "current_language", /* gen240 */
        "utterance", /* gen240: session conversation log */
        "artifact",
        "process_pid", "os_language", /* gen240: process/locale session context */
        "compound_word", /* gen240: compound-word riddle facts */
        "appearance", /* gen240: sensory descriptions, queried not described */
        "taste_of",
        "analogy_relation",
        "synonym", /* gen231/236: synonym relation, queried not described */
        "idiom_meaning", "boils_at", "freezes_at", "historical_fact", /* gen241 */
        "river_of", "ocean_west_of", "ocean_borders", "moon_of", "anagram_of", /* gen241 */
        "process_step", "process_topic", "limerick_l1", "limerick_l2", "limerick_l3", /* gen241 */
        "limerick_l4", "limerick_l5", "poem4", "completion_exact", "fill_three",
        "scenario_step", "activity_topic", "activity_step", "activity_summary", "place_for",
        "sensory_topic", "sensory_phrase", "concise_topic", "concise_explain",
        /* gen311: morphology + rewrite + inference + compute substrate — these carry
         * common English words as subjects (conj_es(need,...), clue_verb(cry,...)),
         * so they must NEVER be dumped as an entity "description". */
        "conj_es", "conj_fr", "pro_drop", "negation_es", "aux_question",
        "wh_front_es", "subject_pron_fr", "rewrite_es", "elide_join", "describe_cue",
        "pair_magnitude", "clue_verb", "emits", "is_like", "inanimate", "cries",
        "flashes", "depicts", "contains", "has_part", "has_property", "can_do",
        "add", "len", "nat", "choose", "valid_hypergeom_take",
        "skip_hypergeom_take", "hypergeom_term", "fav_at_least",
        "probability_procedure", "approach_speed", "meet_time",
        "paired_dimensions_from_doubled_sum_ratio",
        "rectangle_dimensions_from_perimeter_ratio", "proportional_cost",
        "change_due", "digit_count_between", "digit_count_between_prim",
        "ratio_word", "quantity_role_cue", "schema_role_class",
        "event_frame", "event_attr",
        "compound_guard",
        "state_cue", "ordinal_position", "border_count", "sea_borders",
        "state_consequence", "positional_sentence",
        "physical_contrast", "default_pick_reason",
        "formatted_explanation", "formatted_explanation_cue",
        "response_format_variant",
        "exchange_initial_cue", "exchange_transfer_cue",
        "country_with_two_border_constraints",
        "riddle_sig", "response_template",
        /* gen313: code/KB-substrate predicates — never entity descriptions */
        "code_function", "code_calls", "day_order",
        /* gen335 (F.): presentation-layer machinery (kb/core/presentation.p0) —
         * how a datum is rendered in output, never a describable concept. */
        "present_rule", "proper_name",
        NULL,
    };
    for (size_t i = 0; s[i]; i++) if (strcmp(pred, s[i]) == 0) return 1;
    return 0;
}

int kb_describe_entity(const KB *kb, const char *entity,
                       char *out, size_t out_size) {
    if (!kb || !term_ok(entity) || !out || out_size == 0) return 0;
    out[0] = '\0';

    size_t off = 0;
    int count = 0;
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *f = &kb->facts[i];
        if (!fact_mentions(f, entity) || is_model_pred(kb, f->pred) ||
            is_struct_pred(f->pred)) continue;
        char piece[220];
        if (kb_find_neg(kb, f)) render_conflict_direct(f, entity, piece, sizeof piece);
        else render_fact_direct(f, entity, 0, piece, sizeof piece);
        if (!append_piece(out, out_size, &off, piece)) break;
        count++;
    }
    for (size_t i = 0; i < kb->nn; i++) {
        const Fact *f = &kb->neg[i];
        if (!fact_mentions(f, entity) || kb_find(kb, f)) continue;
        char piece[220];
        render_fact_direct(f, entity, 1, piece, sizeof piece);
        if (!append_piece(out, out_size, &off, piece)) break;
        count++;
    }
    if (count == 0) return 0;
    if (off + 1 < out_size) {
        out[off++] = '.';
        out[off] = '\0';
    } else {
        out[out_size - 1] = '\0';
    }
    return 1;
}

/* gen313: the DEFINITION view of an entity. kb_describe_entity above is the
 * honest full belief dump ("what do you know about X?") and may render raw
 * clauses and object mentions — that is ratcheted behavior. A DEFINITION frame
 * ("what is the X", "define X") is stricter: only facts that SPEAK about the
 * entity as their subject qualify — pred(entity) ("X is a pred") or a
 * description-bearing pred(entity, ..., "text"). A fact that would fall back
 * to a raw clause dump, or that merely mentions the entity as an object
 * (is_a(skin, organ) is not a definition of "organ"), never claims here, so
 * fuzzy recall and honest declines downstream keep their turn. */
int kb_define_entity(const KB *kb, const char *entity,
                     char *out, size_t out_size) {
    if (!kb || !term_ok(entity) || !out || out_size == 0) return 0;
    out[0] = '\0';

    size_t off = 0;
    int count = 0;
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *f = &kb->facts[i];
        if (is_model_pred(kb, f->pred) || is_struct_pred(f->pred)) continue;
        if (f->argc < 1 || strcmp(f->args[0], entity) != 0) continue;
        /* speakable only: unary class fact or quoted description — the raw
         * clause fallback of render_fact_direct is machinery, not a definition */
        if (f->argc != 1 && f->args[f->argc - 1][0] != '"') continue;
        char piece[220];
        if (kb_find_neg(kb, f)) render_conflict_direct(f, entity, piece, sizeof piece);
        else render_fact_direct(f, entity, 0, piece, sizeof piece);
        if (!append_piece(out, out_size, &off, piece)) break;
        count++;
    }
    if (count == 0) return 0;
    if (off + 1 < out_size) {
        out[off++] = '.';
        out[off] = '\0';
    } else {
        out[out_size - 1] = '\0';
    }
    return 1;
}

/* gen155: the first brick of a SIMILARITY SPACE. An LLM generalises by vector
 * proximity; in discrete C the honest analogue is structural overlap derived
 * from the KB's own descriptions — not an enumerated synonym table. Two words
 * count as similar if equal or sharing a >=4-char prefix, so the match is
 * morphology- and even COGNATE-tolerant (circonferenza ~ circumference), letting
 * recall cross EN<->IT for loanwords with no translation list. */
static int word_sim(const char *a, const char *b) {
    if (strcmp(a, b) == 0) return 1;
    size_t la = strlen(a), lb = strlen(b);
    if (la < 4 || lb < 4) return 0;          /* short words must match exactly */
    size_t m = la < lb ? la : lb, p = 0;
    while (p < m && a[p] == b[p]) p++;
    return p >= 4;
}

/* Tokenise a snake_case key OR a quoted description into lowercased content
 * tokens (>=3 chars, minus a tiny stoplist and punctuation). */
static size_t concept_tokens(const char *s, char toks[][KB_TERM_LEN], size_t max) {
    /* TODO(kb-first): una lista di stopword dentro il KERNEL, mentre
     * `stopword/1` in kb/core/lexicon.p0 ne ha duecentonovantasette. Qui il
     * vincolo e' reale — kb.c non deve dipendere dal contenuto di una KB
     * specifica — quindi la strada non e' interrogare la KB da qui, ma far
     * passare la lista dal chiamante, che la KB ce l'ha. */
    static const char *const stop[] = {"the","and","its","their","that","for",
        "with","two","one","into","from","each","than","not", NULL};
    size_t n = 0;
    char buf[KB_TERM_LEN]; snprintf(buf, sizeof buf, "%s", s);
    for (char *q = buf; *q; q++) *q = (char)tolower((unsigned char)*q);
    char *p = buf;
    while (*p && n < max) {
        while (*p && !isalpha((unsigned char)*p)) p++;
        char *start = p;
        while (*p && isalpha((unsigned char)*p)) p++;
        size_t len = (size_t)(p - start);
        if (len >= 3 && len < KB_TERM_LEN) {
            char t[KB_TERM_LEN]; memcpy(t, start, len); t[len] = '\0';
            int isstop = 0;
            for (size_t i = 0; stop[i]; i++) if (strcmp(t, stop[i]) == 0) { isstop = 1; break; }
            if (!isstop) snprintf(toks[n++], KB_TERM_LEN, "%s", t);
        }
    }
    return n;
}

int kb_nearest_concept(const KB *kb, const char *const *qwords, size_t nq,
                       char *key_out, size_t key_sz,
                       char *desc_out, size_t desc_sz) {
    if (!kb || nq == 0 || nq > 64 || !key_out || !desc_out) return 0;

    /* gen156: idf-like weighting. A learned metric is what an LLM has and parrot0
     * lacks; the cheapest honest analogue derived from the corpus (the KB
     * itself) is INVERSE DOCUMENT FREQUENCY — a query word that matches many
     * concepts ("number", "blood", "system") is uninformative and must count
     * less than a rare, discriminative one. Pass 1 measures each query word's
     * document frequency across the concept descriptions; pass 2 scores overlap
     * weighted by 1/df, so ties that plain counting could not break ("the bone
     * that protects the brain": skull vs skeletal) resolve toward the concept
     * the rarer words point at. */
    /* gen346 (scalability): tokenize each concept ONCE. Pass 1 records, per fact,
     * a bitmask of which query words it matched (nq<=64) and accumulates the
     * document frequency; pass 2 scores straight from the bitmasks — no fact is
     * tokenized or word_sim'd twice, so this stays fast as the KB grows. Behaviour
     * is identical to the old two-tokenization version. */
    unsigned long long *hits = calloc(kb->n ? kb->n : 1, sizeof *hits);
    if (!hits) return 0;
    size_t df[64] = {0};
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *f = &kb->facts[i];
        if (f->argc < 2 || f->args[f->argc - 1][0] != '"') continue;
        if (is_model_pred(kb, f->pred) || is_struct_pred(f->pred)) continue;
        char ctoks[96][KB_TERM_LEN];
        size_t nc = concept_tokens(f->args[0], ctoks, 96);
        nc += concept_tokens(f->args[f->argc - 1], ctoks + nc, 96 - nc);
        unsigned long long mask = 0;
        for (size_t q = 0; q < nq; q++)
            for (size_t c = 0; c < nc; c++)
                if (word_sim(qwords[q], ctoks[c])) { mask |= 1ULL << q; break; }
        hits[i] = mask;
        for (size_t q = 0; q < nq; q++) if (mask & (1ULL << q)) df[q]++;
    }
    double w[64];
    for (size_t q = 0; q < nq; q++) w[q] = 1.0 / (double)(df[q] ? df[q] : 1);

    double bestw = 0.0, secondw = 0.0;
    int bestcount = 0;
    const Fact *bestf = NULL;
    for (size_t i = 0; i < kb->n; i++) {
        unsigned long long mask = hits[i];
        if (!mask) continue;
        double sw = 0.0; int cnt = 0;
        for (size_t q = 0; q < nq; q++)
            if (mask & (1ULL << q)) { sw += w[q]; cnt++; }
        if (sw > bestw) { secondw = bestw; bestw = sw; bestcount = cnt; bestf = &kb->facts[i]; }
        else if (sw > secondw) secondw = sw;
    }
    free(hits);
    /* Need real evidence (>=2 overlapping words) AND a clear WEIGHTED winner over
     * the runner-up, so one coincidental token never triggers a confident guess
     * and a genuinely symmetric tie still abstains. */
    if (bestcount >= 2 && bestf && bestw - secondw >= 0.15 * bestw) {
        snprintf(key_out, key_sz, "%s", bestf->args[0]);
        char d[KB_TERM_LEN]; snprintf(d, sizeof d, "%s", bestf->args[bestf->argc - 1]);
        size_t dl = strlen(d); if (dl && d[dl - 1] == '"') d[--dl] = '\0';
        snprintf(desc_out, desc_sz, "%s", d[0] == '"' ? d + 1 : d);
        return bestcount;
    }
    return 0;
}

/* gen157: true if `term` is the key of some description-bearing concept fact. */
int kb_is_concept_key(const KB *kb, const char *term) {
    if (!kb || !term) return 0;
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *f = &kb->facts[i];
        if (f->argc < 2 || f->args[f->argc - 1][0] != '"') continue;
        if (is_model_pred(kb, f->pred) || is_struct_pred(f->pred)) continue;
        if (strcmp(f->args[0], term) == 0) return 1;
    }
    return 0;
}

/* gen172: fetch the (dequoted) definition of the concept whose key is `key` and
 * whose last argument is a quoted description — learned (wiki_concept) or loaded.
 * Mirrors kb_is_concept_key. Returns 1 + the text in `out`, 0 if none. Lets a
 * re-ask of a just-learned topic answer from RAM. */
int kb_concept_def(const KB *kb, const char *key, char *out, size_t out_size) {
    if (!kb || !key || !out || out_size == 0) return 0;
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *f = &kb->facts[i];
        if (f->argc < 2 || f->args[f->argc - 1][0] != '"') continue;
        if (is_model_pred(kb, f->pred) || is_struct_pred(f->pred)) continue;
        if (strcmp(f->args[0], key) != 0) continue;
        snprintf(out, out_size, "%s", f->args[f->argc - 1] + 1); /* skip open quote */
        size_t l = strlen(out);
        if (l > 0 && out[l - 1] == '"') out[l - 1] = '\0';       /* drop close quote */
        return 1;
    }
    return 0;
}

/* gen344 (language mirroring): a mature interlocutor answers in the ASKER's
 * language. The definition text is knowledge; its localizations are knowledge
 * too — concept_gloss(Key, Lang, "full localized sentence"). When the current
 * language is not English and such a gloss exists, the definitional path speaks
 * it verbatim (a complete sentence, article and copula included, so no English
 * "%s is %s" frame wraps it). Returns 1 + the dequoted sentence, 0 if none.
 * Teaching an Italian (or any-language) definition is ONE fact, zero C. */
int kb_concept_gloss(const KB *kb, const char *key, const char *lang,
                     char *out, size_t out_size) {
    if (!kb || !key || !lang || !out || out_size == 0) return 0;
    const char *q[3] = { key, lang, NULL };
    char hit[1][KB_TERM_LEN];
    if (kb_match((KB *)kb, "concept_gloss", q, 3, hit, 1) != 1) return 0;
    const char *s = hit[0];
    if (*s == '"') s++;
    snprintf(out, out_size, "%s", s);
    size_t l = strlen(out);
    if (l > 0 && out[l - 1] == '"') out[l - 1] = '\0';
    return 1;
}

/* gen157: relational reasoning DERIVED from unstructured descriptions. parrot0
 * was never told "heart is part of circulatory" — but the circulatory
 * description NAMES the heart, so the containment relation can be recovered from
 * the text. Find the concept whose description mentions `term` (a different
 * concept), recovering an emergent taxonomy that was never asserted as facts. */
static const char *concept_pred(const KB *kb, const char *key);
static int valid_member(const KB *kb, const char *mem, const char *contpred);

int kb_concept_mentioning(const KB *kb, const char *term,
                          char *key_out, size_t key_sz,
                          char *desc_out, size_t desc_sz) {
    if (!kb || !term || !key_out || !desc_out) return 0;
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *f = &kb->facts[i];
        if (f->argc < 2 || f->args[f->argc - 1][0] != '"') continue;
        if (is_model_pred(kb, f->pred) || is_struct_pred(f->pred)) continue;
        if (strcmp(f->args[0], term) == 0) continue;   /* a concept never contains itself */
        if (!valid_member(kb, term, f->pred)) continue; /* gen159: sibling, not a part */
        char ctoks[96][KB_TERM_LEN];
        size_t nc = concept_tokens(f->args[f->argc - 1], ctoks, 96);
        for (size_t c = 0; c < nc; c++) {
            /* EXACT mention (gen158): a containment claim must not rest on a
             * cognate near-match (respiratory ~ responses). */
            if (strcmp(term, ctoks[c]) == 0) {
                snprintf(key_out, key_sz, "%s", f->args[0]);
                char d[KB_TERM_LEN]; snprintf(d, sizeof d, "%s", f->args[f->argc - 1]);
                size_t dl = strlen(d); if (dl && d[dl - 1] == '"') d[--dl] = '\0';
                snprintf(desc_out, desc_sz, "%s", d[0] == '"' ? d + 1 : d);
                return 1;
            }
        }
    }
    return 0;
}

/* gen158: MATERIALIZE the emergent containment relation as first-class part_of/2
 * facts so the resolution engine can PROVE and query it ("is the heart part of
 * circulatory?" -> Yes; "what is part of circulatory?" -> heart). The danger is
 * that a mention is not a containment ("composite is a number that is not
 * prime"), so we only materialize from CONTAINER predicates — detected
 * structurally as predicates whose facts repeatedly name OTHER concept keys
 * (body_system names organs; number_property names almost none). No hardcoded
 * domain list; the facts are KB_REFLECTIVE so they regenerate each boot and are
 * never persisted. Returns the number of relations added. */
static int str_in(const char set[][KB_TERM_LEN], size_t n, const char *s) {
    for (size_t i = 0; i < n; i++) if (strcmp(set[i], s) == 0) return 1;
    return 0;
}

/* gen159: the predicate ("taxonomic level") of the concept keyed by `key`. */
static const char *concept_pred(const KB *kb, const char *key) {
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *f = &kb->facts[i];
        if (f->argc < 2 || f->args[f->argc - 1][0] != '"') continue;
        if (is_model_pred(kb, f->pred) || is_struct_pred(f->pred)) continue;
        if (strcmp(f->args[0], key) == 0) return f->pred;
    }
    return NULL;
}

/* gen159: a valid containment crosses TWO taxonomic levels (an organ is part of
 * a system); two concepts of the SAME predicate are siblings, never nested. This
 * type gate kills the text-ambiguity false positive where a system name appears
 * as an ADJECTIVE in a sibling's description ("skeletal ... muscles" in the
 * muscular system). `mem` is a concept key, `contpred` the container's predicate. */
static int valid_member(const KB *kb, const char *mem, const char *contpred) {
    const char *mp = concept_pred(kb, mem);
    return mp && strcmp(mp, contpred) != 0;
}

typedef struct {
    char keys[512][KB_TERM_LEN];
    char key_pred[512][KB_TERM_LEN];
    char preds[128][KB_TERM_LEN];
    int  pcnt[128];
    char tokens[96][KB_TERM_LEN];
} DerivePartFrame;

int kb_derive_part_of(KB *kb) {
    if (!kb) return 0;
    const size_t n0 = kb->n;
    DerivePartFrame *scratch = malloc(sizeof *scratch);
    if (!scratch) return 0;

    /* collect the concept keys AND their predicates once (exact membership) */
    char (*keys)[KB_TERM_LEN] = scratch->keys;
    char (*key_pred)[KB_TERM_LEN] = scratch->key_pred;
    size_t nkeys = 0;
    for (size_t i = 0; i < n0 && nkeys < 512; i++) {
        const Fact *f = &kb->facts[i];
        if (f->argc < 2 || f->args[f->argc - 1][0] != '"') continue;
        if (is_model_pred(kb, f->pred) || is_struct_pred(f->pred)) continue;
        int dup = 0;
        for (size_t k = 0; k < nkeys; k++) if (strcmp(keys[k], f->args[0]) == 0) { dup = 1; break; }
        if (!dup) {
            snprintf(keys[nkeys], KB_TERM_LEN, "%s", f->args[0]);
            snprintf(key_pred[nkeys], KB_TERM_LEN, "%s", f->pred);
            nkeys++;
        }
    }

    /* per-predicate count of facts that name some OTHER concept key */
    char (*preds)[KB_TERM_LEN] = scratch->preds;
    int *pcnt = scratch->pcnt;
    size_t npreds = 0;
    for (size_t i = 0; i < n0; i++) {
        const Fact *f = &kb->facts[i];
        if (f->argc < 2 || f->args[f->argc - 1][0] != '"') continue;
        if (is_model_pred(kb, f->pred) || is_struct_pred(f->pred)) continue;
        char (*ctoks)[KB_TERM_LEN] = scratch->tokens;
        size_t nc = concept_tokens(f->args[f->argc - 1], ctoks, 96);
        int names_other = 0;
        for (size_t c = 0; c < nc; c++) {
            if (strcmp(ctoks[c], f->args[0]) == 0) continue;
            if (!str_in((const char (*)[KB_TERM_LEN])keys, nkeys, ctoks[c])) continue;
            {   /* gen334: cached key→predicate lookup, O(nkeys) instead of
                 * old concept_pred()'s O(n0). */
                int ok = 0;
                for (size_t ki = 0; ki < nkeys; ki++)
                    if (strcmp(keys[ki], ctoks[c]) == 0 &&
                        strcmp(key_pred[ki], f->pred) != 0) { ok = 1; break; }
                if (!ok) continue;
            }
            names_other = 1; break;
        }
        if (!names_other) continue;
        size_t p = 0; for (; p < npreds; p++) if (strcmp(preds[p], f->pred) == 0) break;
        if (p == npreds && npreds < 128) { snprintf(preds[npreds], KB_TERM_LEN, "%s", f->pred); pcnt[npreds] = 0; npreds++; }
        if (p < 128) pcnt[p]++;
    }

    /* materialize part_of for facts of container predicates (>=2 such facts) */
    int saved = kb->origin;
    kb_set_origin(kb, KB_REFLECTIVE);
    int added = 0;
    for (size_t i = 0; i < n0; i++) {
        const Fact *f = &kb->facts[i];
        if (f->argc < 2 || f->args[f->argc - 1][0] != '"') continue;
        if (is_model_pred(kb, f->pred) || is_struct_pred(f->pred)) continue;
        int container = 0;
        for (size_t p = 0; p < npreds; p++)
            if (strcmp(preds[p], f->pred) == 0) { container = (pcnt[p] >= 2); break; }
        if (!container) continue;
        char key[KB_TERM_LEN], pred[KB_TERM_LEN];
        snprintf(key, sizeof key, "%s", f->args[0]);
        snprintf(pred, sizeof pred, "%s", f->pred);
        char (*ctoks)[KB_TERM_LEN] = scratch->tokens;
        size_t nc = concept_tokens(f->args[f->argc - 1], ctoks, 96);
        for (size_t c = 0; c < nc; c++) {
            if (strcmp(ctoks[c], key) == 0) continue;
            if (!str_in((const char (*)[KB_TERM_LEN])keys, nkeys, ctoks[c])) continue;
            {   /* gen334: cached key→predicate lookup */
                int ok = 0;
                for (size_t ki = 0; ki < nkeys; ki++)
                    if (strcmp(keys[ki], ctoks[c]) == 0 &&
                        strcmp(key_pred[ki], pred) != 0) { ok = 1; break; }
                if (!ok) continue;
            }
            const char *args[2] = { ctoks[c], key };
            if (kb_assert(kb, "part_of", args, 2)) added++;
        }
    }
    kb_set_origin(kb, saved);
    free(scratch);
    return added;
}

int kb_knows_pred(const KB *kb, const char *pred) {
    if (!kb || !pred) return 0;
    PredBucket bk = pred_bucket(kb, pred);
    if (bk.live && bk.n > 0) return 1;
    for (size_t i = 0; i < kb->n && !bk.live; i++)
        if (strcmp(kb->facts[i].pred, pred) == 0) return 1;
    for (size_t i = 0; i < kb->nn; i++)
        if (strcmp(kb->neg[i].pred, pred) == 0) return 1;
    PredBucket rbk = rule_bucket(kb, pred);
    if (rbk.live && rbk.n > 0) return 1;
    for (size_t i = 0; i < kb->nr && !rbk.live; i++)
        if (strcmp(kb->rules[i].head.pred, pred) == 0) return 1;
    return 0;
}

/* gen408 — LE REGOLE MORTE, cioe' §4d di question-emergence.md finalmente
 * calcolato invece che elencato.
 *
 * Una regola e' morta quando un predicato del suo CORPO non ha nessun
 * produttore: nessun fatto lo asserisce e nessuna regola lo conclude. Non
 * fallira' mai rumorosamente — semplicemente non si dimostrera' mai, e chi
 * guarda la KB la vede piena di capacita' che non esistono.
 *
 * E' la forma di spazio negativo piu' economica da trovare di tutte: si calcola
 * dalla sola KB, senza corpus, senza oracolo, senza conversazione. Il caso che
 * l'ha fatta nascere e' istruttivo — l'intero strato del ragionamento familiare
 * (`ancestor_of`, `grandfather_of`, `sibling_of`, `child_of`) legge
 * `parent_of/2`, che nessuno scrive mai: la conversazione produce `father/2` e
 * `mother/2`. Due vocabolari che non si toccano, e uno strato di ragionamento
 * che poteva funzionare solo sui cinque fatti di esempio scritti a mano accanto
 * alle regole.
 *
 * Restano fuori i builtin del solutore, che non hanno produttori per
 * definizione. Scrive coppie (testa, predicato-mancante): la stessa regola puo'
 * comparire piu' volte se le mancano piu' pezzi, ed e' giusto cosi' — sono
 * lacune distinte. */
/* Un predicato e' INERTE quando nessuna regola lo conclude e i suoi unici fatti
 * sono quelli curati a mano: non puo' crescere parlando, quindi le regole che lo
 * leggono varranno per sempre soltanto sui propri esempi.
 *
 * E' il caso vero trovato da una domanda di F. — «il padre del padre è inteso
 * il?». Lo strato del ragionamento familiare legge `parent_of/2`, che esiste
 * come cinque fatti di esempio scritti accanto alle regole e che nessun'altra
 * strada scrive mai: la conversazione produce `father/2`. Formalmente la regola
 * non e' morta e infatti il rilevatore degli orfani non trova niente; nei fatti
 * non potra' mai rispondere di nessuna famiglia che non sia quella di esempio.
 *
 * Due vocabolari per la stessa relazione, senza un ponte: e' §4d di
 * question-emergence.md, «dialetti privati», e questa e' la sua forma
 * calcolabile. */
static int kb_pred_is_inert(const KB *kb, const char *pred) {
    if (!kb || !pred || !*pred) return 0;
    PredBucket rb = rule_bucket(kb, pred);
    size_t rvisits = rb.live ? rb.n : kb->nr;
    for (size_t vi = 0; vi < rvisits; vi++) {
        size_t at = rb.live ? rb.idx[vi] : vi;
        if (at < kb->nr && strcmp(kb->rules[at].head.pred, pred) == 0) return 0;
    }
    int any = 0;
    PredBucket fb = pred_bucket(kb, pred);
    for (size_t vi = 0; vi < PRED_VISITS(fb, kb); vi++) {
        const Fact *f = &kb->facts[PRED_AT(fb, vi)];
        if (strcmp(f->pred, pred) != 0) continue;
        any = 1;
        if (f->origin != KB_BASE) return 0;   /* qualcosa lo scrive davvero */
    }
    return any;
}

static int kb_pred_has_producer(const KB *kb, const char *pred, size_t argc) {
    if (!kb || !pred || !*pred) return 1;
    static const char *const builtins[] = {
        "is","lt","le","gt","ge","eq","ne","dif","call","naf","not",
        "findall","findall_bag","prob","ranges_over","assert","retract",
        "chars","upcase_first","concat_atoms","kb_fact","apply", NULL };
    for (size_t i = 0; builtins[i]; i++)
        if (strcmp(pred, builtins[i]) == 0) return 1;
    /* L'indice per predicato esiste dal gen401 e va usato: una scansione
     * lineare qui sarebbe O(regole x fatti), cioe' centinaia di milioni di
     * confronti su una KB vera. */
    PredBucket fb = pred_bucket(kb, pred);
    for (size_t vi = 0; vi < PRED_VISITS(fb, kb); vi++)
        if (strcmp(kb->facts[PRED_AT(fb, vi)].pred, pred) == 0) return 1;
    PredBucket rb = rule_bucket(kb, pred);
    size_t rvisits = rb.live ? rb.n : kb->nr;   /* il fallback e' sulle REGOLE */
    for (size_t vi = 0; vi < rvisits; vi++) {
        size_t at = rb.live ? rb.idx[vi] : vi;
        if (at < kb->nr && strcmp(kb->rules[at].head.pred, pred) == 0) return 1;
    }
    (void)argc;
    return 0;
}

size_t kb_dead_rules(const KB *kb, char heads[][KB_TERM_LEN],
                     char missing[][KB_TERM_LEN], size_t max) {
    if (!kb || !heads || !missing || !max) return 0;
    size_t n = 0;
    for (size_t i = 0; i < kb->nr && n < max; i++) {
        const Rule *r = &kb->rules[i];
        for (size_t j = 0; j < r->nbody && n < max; j++) {
            const char *bp = r->body[j].pred;
            if (!bp || !*bp) continue;
            if (kb_pred_has_producer(kb, bp, r->body[j].argc) &&
                !kb_pred_is_inert(kb, bp)) continue;
            snprintf(heads[n], KB_TERM_LEN, "%s", r->head.pred);
            snprintf(missing[n], KB_TERM_LEN, "%s", bp);
            n++;
        }
    }
    return n;
}

int kb_rule_body_mentions(const KB *kb, const char *pred) {
    if (!kb || !pred) return 0;
    for (size_t r = 0; r < kb->nr; r++)
        for (size_t b = 0; b < kb->rules[r].nbody; b++)
            if (strcmp(kb->rules[r].body[b].pred, pred) == 0) return 1;
    return 0;
}

size_t kb_unary_predicates(const KB *kb, char out[][KB_TERM_LEN], size_t max) {
    if (!kb) return 0;
    size_t n = 0;
    for (size_t i = 0; i < kb->n; i++)
        if (kb->facts[i].argc == 1) push_unique(out, &n, max, kb->facts[i].pred);
    for (size_t i = 0; i < kb->nr; i++)
        if (kb->rules[i].head.argc == 1)
            push_unique(out, &n, max, kb->rules[i].head.pred);
    return n;
}

/* gen394: le classi che possono valere PER QUESTA entita', non tutte.
 *
 * `kb_unary_predicates` raccoglie ogni predicato unario della KB in ordine di
 * caricamento, e chi la usa deve dargli un tetto. Il tetto e' un difetto di
 * ordine travestito da limite di memoria: misurato, aggiungere sei regole
 * qualunque spingeva fuori dal centoventottesimo posto una testa di regola
 * imparata a runtime, e «describe tariq» perdeva in SILENZIO l'ultima credenza
 * derivata. Nessun errore, nessun residuo: una risposta piu' corta. Crescere in
 * conoscenza non puo' accorciare una risposta gia' data.
 *
 * La selezione giusta e' piu' stretta e insieme piu' completa: i predicati sotto
 * cui l'entita' compare gia' come fatto, piu' TUTTE le teste di regola unarie —
 * che sono le sole che possono derivare qualcosa su di lei. Il numero non
 * dipende piu' da quanti fatti irrilevanti contenga la KB, e l'ordine di
 * caricamento smette di decidere che cosa venga detto. */
size_t kb_unary_predicates_for(const KB *kb, const char *entity,
                               char out[][KB_TERM_LEN], size_t max) {
    if (!kb || !entity) return 0;
    size_t n = 0;
    for (size_t i = 0; i < kb->n; i++)
        if (kb->facts[i].argc == 1 && strcmp(kb->facts[i].args[0], entity) == 0)
            push_unique(out, &n, max, kb->facts[i].pred);
    for (size_t i = 0; i < kb->nr; i++)
        if (kb->rules[i].head.argc == 1)
            push_unique(out, &n, max, kb->rules[i].head.pred);
    return n;
}

/* gen382i — questo termine compare DA QUALCHE PARTE nella KB?
 *
 * Serve a garantire che un token scelto per chiudere un ragionamento ipotetico
 * non sia intercettato da conoscenza esistente: non basta che non sia un
 * predicato, non deve comparire nemmeno come argomento o dentro la testa o il
 * corpo di una regola. E' la verifica che rende onesta la rinominazione — e che
 * l'amputazione non poteva offrire, perche' li' non si verificava nulla: si
 * toglieva e basta. */
int kb_mentions_term(const KB *kb, const char *term) {
    if (!kb || !term || !*term) return 0;
    /* I fatti stanno in ordine di caricamento, quindi quelli di uno stesso
     * predicato sono contigui: una memoria di UN elemento basta a non
     * interrogare il solver una volta per riscontro. Senza, un termine che
     * compare in molti fatti di contabilita' pagava una risoluzione per
     * ciascuno. */
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *f = &kb->facts[i];
        if (strcmp(f->pred, term) == 0) return 1;
        for (size_t a = 0; a < f->argc; a++)
            if (strstr(f->args[a], term)) return 1;
    }
    for (size_t i = 0; i < kb->nr; i++) {
        const Rule *r = &kb->rules[i];
        if (strcmp(r->head.pred, term) == 0) return 1;
        for (size_t a = 0; a < r->head.argc; a++)
            if (strstr(r->head.args[a], term)) return 1;
        for (size_t bgi = 0; bgi < r->nbody; bgi++) {
            if (strcmp(r->body[bgi].pred, term) == 0) return 1;
            for (size_t a = 0; a < r->body[bgi].argc; a++)
                if (strstr(r->body[bgi].args[a], term)) return 1;
        }
    }
    return 0;
}

size_t kb_predicates(const KB *kb, char out[][KB_TERM_LEN], size_t max) {
    if (!kb) return 0;
    size_t n = 0;
    for (size_t i = 0; i < kb->n; i++)
        push_unique(out, &n, max, kb->facts[i].pred);
    for (size_t i = 0; i < kb->nr; i++)
        push_unique(out, &n, max, kb->rules[i].head.pred);
    return n;
}

size_t kb_binary_relations(const KB *kb, const char *left, const char *right,
                           char out[][KB_TERM_LEN], size_t max) {
    if (!kb || !left || !right || (max && !out)) return 0;
    size_t n = 0;
    for (size_t i = 0; i < kb->n; i++) {
        const Fact *f = &kb->facts[i];
        if (f->argc != 2 || strcmp(f->args[0], left) != 0 ||
            strcmp(f->args[1], right) != 0)
            continue;
        push_unique(out, &n, max, f->pred);
    }
    return n;
}

int kb_dump_all(const KB *kb, char *out, size_t out_size) {
    if (!kb || !out || out_size == 0) return 0;
    size_t off = 0;
    size_t written = 0;
    for (size_t i = 0; i < kb->n && off + 1 < out_size; i++) {
        Fact *f = &kb->facts[i];
        off += (size_t)snprintf(out + off, out_size - off, "%s(", f->pred);
        for (size_t j = 0; j < f->argc; j++)
            off += (size_t)snprintf(out + off, out_size - off, "%s%s",
                                     j ? ", " : "", f->args[j]);
        off += (size_t)snprintf(out + off, out_size - off, "). ");
        written++;
    }
    if (off > 0 && off + 1 < out_size) {
        if (out[off - 1] == ' ') out[--off] = '\0';
    }
    if (written == 0) out[0] = '\0';
    return written > 0;
}

size_t kb_size(const KB *kb) {
    return kb ? kb->n : 0;
}

size_t kb_rule_count(const KB *kb) {
    return kb ? kb->nr : 0;
}

void kb_inference_report(const KB *kb, KbInferenceReport *out) {
    if (!out) return;
    memset(out, 0, sizeof *out);
    if (!kb) return;
    out->steps      = kb->infer_steps;
    out->budget_hit = kb->infer_budget_hit;
    out->loops_cut  = kb->infer_loops_cut;
    snprintf(out->goal, sizeof out->goal, "%s", kb->infer_goal);
}

size_t kb_pred_fact_count(const KB *kb, const char *pred) {
    if (!kb || !pred) return 0;
    size_t count = 0;
    for (size_t i = 0; i < kb->n; i++)
        if (strcmp(kb->facts[i].pred, pred) == 0) count++;
    return count;
}

size_t kb_rule_body_preds(const KB *kb, const char *head, size_t argc,
                          char out_body[][KB_TERM_LEN], size_t max) {
    if (!kb || !head || max == 0) return 0;
    for (size_t r = 0; r < kb->nr; r++) {
        const Rule *R = &kb->rules[r];
        if (R->head.argc != argc || strcmp(R->head.pred, head) != 0) continue;
        size_t n = 0;
        for (size_t b = 0; b < R->nbody && n < max; b++)
            snprintf(out_body[n++], KB_TERM_LEN, "%s", R->body[b].pred);
        return n;
    }
    return 0;
}

size_t kb_rules_for_head(const KB *kb, const char *head, size_t argc) {
    if (!kb || !head) return 0;
    size_t count = 0;
    for (size_t r = 0; r < kb->nr; r++)
        if (kb->rules[r].head.argc == argc &&
            strcmp(kb->rules[r].head.pred, head) == 0) count++;
    return count;
}

size_t kb_nth_rule_body_preds(const KB *kb, const char *head, size_t argc,
                              size_t idx, char out_body[][KB_TERM_LEN], size_t max) {
    if (!kb || !head || max == 0) return 0;
    size_t seen = 0;
    for (size_t r = 0; r < kb->nr; r++) {
        const Rule *R = &kb->rules[r];
        if (R->head.argc != argc || strcmp(R->head.pred, head) != 0) continue;
        if (seen++ != idx) continue;
        size_t n = 0;
        for (size_t b = 0; b < R->nbody && n < max; b++)
            snprintf(out_body[n++], KB_TERM_LEN, "%s", R->body[b].pred);
        return n;
    }
    return 0;
}
