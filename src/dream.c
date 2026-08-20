/* dream.c — l'esplorazione ricorsiva di un topic attraverso la sua prosa.
 *
 * Vedi dream.h per il senso. Qui vale la nota architetturale: questo file NON
 * contiene un secondo estrattore. Sognare significa guidare, in modo ricorsivo,
 * lo stesso percorso di comprensione che parrot0 usa in conversazione — il
 * driver non sa nulla di prosa, di classi o di grammatica, e non deve saperne.
 * Le uniche decisioni che prende (quanto in profondita', quanti nodi, quali
 * parole valga la pena sognare) sono lette dalla KB, perche' la POLITICA del
 * sogno e' conoscenza tanto quanto il suo contenuto.
 */
#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "dream.h"
#include "kb.h"
#include "learn.h"
#include "env.h"

#define DREAM_MAX_FRONTIER 4096
#define DREAM_KEY_LEN      96
#define DREAM_MAX_BRIDGE   32

typedef struct {
    char   key[DREAM_KEY_LEN];
    int    depth;
    char   parent[DREAM_KEY_LEN];
} DreamNode;

typedef struct {
    DreamNode *q;
    size_t     head, tail, cap;
    char     (*seen)[DREAM_KEY_LEN];
    size_t     nseen, seen_cap;

    /* il registro di cio' che si e' visto e di cio' che si e' PERSO */
    size_t nodes, pages_local, pages_fetched, pages_missing;
    size_t facts_seen, silent_pages;
    char (*lost)[DREAM_KEY_LEN];
    size_t nlost, lost_cap;

    /* gen405: il sogno guidato dalle lacune — i turni ancora senza ponte, e il
     * conto di cio' che il sogno ha effettivamente CHIUSO. Un sogno che estrae
     * mille fatti e non chiude niente e' un dato, non un successo. */
    char   bridge[DREAM_MAX_BRIDGE][KB_TERM_LEN];
    size_t nbridge, walls_closed, from_gap_w, from_gap_m, prose_unread, proposed;
} DreamState;

/* Un valore di politica letto dalla KB: `pred(N).` con N numerico. Se la KB non
 * lo dichiara si usa `fallback`, perche' un sogno deve poter partire anche su
 * un cervello nudo — ma quando la KB parla, decide lei. */
static int dream_policy_int(Brain *b, const char *pred, int fallback) {
    if (!b || !brain_kb(b)) return fallback;
    const char *q[] = { NULL };
    char vals[4][KB_TERM_LEN];
    if (kb_match(brain_kb(b), pred, q, 1, vals, 4) == 0) return fallback;
    int v = atoi(vals[0]);
    return v > 0 ? v : fallback;
}

static void key_normalize(const char *in, char *out, size_t sz) {
    size_t o = 0;
    for (size_t i = 0; in[i] && o + 1 < sz; i++) {
        unsigned char c = (unsigned char)in[i];
        if (isalnum(c)) out[o++] = (char)tolower(c);
        else if ((c == ' ' || c == '_' || c == '-') && o && out[o - 1] != '_')
            out[o++] = '_';
    }
    while (o && out[o - 1] == '_') o--;
    out[o] = '\0';
}

static const char *kb_dequote_dream(const char *s);
static size_t dream_fact_sources(KB *kb, char rows[][KB_TERM_LEN], size_t max);
static int dream_row_seen(char rows[][KB_TERM_LEN], size_t n, const char *row);
static size_t dream_reading_facts(KB *kb, char rows[][KB_TERM_LEN], size_t max);

/* Resolve a surface topic through the KB's translation/alias knowledge before
 * addressing Wikipedia.  `scacchi` must not be sent literally to EN Wikipedia:
 * that title denotes a surname, while the KB already says tr(chess, scacchi). */
static void dream_topic_key(Brain *b, const char *surface,
                            char *out, size_t out_size) {
    snprintf(out, out_size, "%s", surface ? surface : "");
    if (!b || !brain_kb(b) || !*out) return;
    const char *q[] = { NULL, out };
    char hit[4][KB_TERM_LEN];
    size_t n = kb_match(brain_kb(b), "tr", q, 2, hit, 4);
    if (n == 1) snprintf(out, out_size, "%s", kb_dequote_dream(hit[0]));
}

static int seen_has(DreamState *st, const char *key) {
    for (size_t i = 0; i < st->nseen; i++)
        if (strcmp(st->seen[i], key) == 0) return 1;
    return 0;
}

static void seen_add(DreamState *st, const char *key) {
    if (st->nseen >= st->seen_cap) return;
    snprintf(st->seen[st->nseen++], DREAM_KEY_LEN, "%s", key);
}

static void lost_add(DreamState *st, const char *key) {
    if (st->nlost >= st->lost_cap) return;
    for (size_t i = 0; i < st->nlost; i++)
        if (strcmp(st->lost[i], key) == 0) return;
    snprintf(st->lost[st->nlost++], DREAM_KEY_LEN, "%s", key);
}

/* La frontiera e' una PILA, non una coda, e la differenza non e' un dettaglio di
 * implementazione: e' che cosa significa sognare.
 *
 * Con una coda si legge la prosa del topic, si accodano tutte le sue parole, e si
 * studiano TUTTE quelle del primo livello prima di scendere: l'esplorazione e'
 * decisa in anticipo, sulla base di un solo testo. Con una pila si studia una
 * prosa e si SCENDE subito dentro la prima parola che ha prodotto — quindi ogni
 * passo successivo e' deciso da cio' che si e' appena capito, non da cio' che si
 * era previsto. E' l'unica delle due che meriti il nome di ricorsione, ed e' la
 * sola su cui abbia senso innestare, piu' avanti, una potatura in stile
 * alfa-beta: si pota un RAMO, e un ramo esiste solo se si scende. */
static void push(DreamState *st, const char *key, int depth, const char *parent) {
    if (st->tail >= st->cap) return;
    snprintf(st->q[st->tail].key, DREAM_KEY_LEN, "%s", key);
    st->q[st->tail].depth = depth;
    snprintf(st->q[st->tail].parent, DREAM_KEY_LEN, "%s", parent ? parent : "");
    st->tail++;
}

/* Il prossimo nodo: l'ultimo inserito (discesa in profondita'). */
static int pop(DreamState *st, DreamNode *out) {
    if (st->tail == st->head) return 0;
    *out = st->q[--st->tail];
    return 1;
}

static void indent(FILE *o, int depth);

/* Expose the fetched bytes to the real reader one sentence at a time. */
static int dream_read_prose(Brain *b, const char *prose, FILE *o, int depth,
                            int debug, size_t *learned, size_t *skipped) {
    if (!b || !prose || !o) return 0;
    const char *p = prose;
    size_t sentence_no = 0;
    while (*p) {
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) break;
        const char *q = p;
        while (*q) {
            char ch = *q;
            if (ch == '!' || ch == '?') { q++; break; }
            if (ch == '.') {
                char prev = q > p ? q[-1] : '\0';
                if (!(isdigit((unsigned char)prev) &&
                      isdigit((unsigned char)q[1]))) { q++; break; }
            }
            q++;
        }
        size_t len = (size_t)(q - p);
        while (len && isspace((unsigned char)p[len - 1])) len--;
        if (!len) { p = q; continue; }

        char sentence[4096], reply[4096];
        size_t cap = len < sizeof sentence - 1 ? len : sizeof sentence - 1;
        memcpy(sentence, p, cap);
        sentence[cap] = '\0';
        fputc('\n', o);
        indent(o, depth);
        fprintf(o, "prompt[%zu]: read: %s\n", sentence_no++, sentence);
        brain_read_prose(b, sentence, reply, sizeof reply);
        indent(o, depth);
        fprintf(o, "learned: %s\n", reply[0] ? reply : "no outcome");
        fflush(o);

        char before[128][KB_TERM_LEN], after[128][KB_TERM_LEN];
        size_t nb = dream_fact_sources(brain_kb(b), before, 128);
        if (debug) {
            indent(o, depth);
            fprintf(o, "debug: reader=brain_read_prose prompt=read: %s\n", sentence);
            fprintf(o, "debug: fact_source_before=%zu\n", nb);
        }

        size_t got = 0, miss = 0;
        if (sscanf(reply, "Learned %zu fact(s), skipped %zu.", &got, &miss) != 2) {
            if (skipped) (*skipped)++;
            indent(o, depth);
            fprintf(o, "STOP: esito di apprendimento non riconosciuto.\n");
            return 0;
        }
        char produced[32][KB_TERM_LEN];
        size_t np = dream_reading_facts(brain_kb(b), produced, 32);
        for (size_t i = 0; i < np; i++)
            fprintf(o, "  fact: %s\n", kb_dequote_dream(produced[i]));
        size_t na = dream_fact_sources(brain_kb(b), after, 128);
        size_t new_facts = 0;
        for (size_t i = 0; i < na; i++) {
            if (dream_row_seen(before, nb, after[i])) continue;
            new_facts++;
        }
        if (debug) {
            indent(o, depth);
            fprintf(o, "debug: fact_source_after=%zu new_facts=%zu reported_facts=%zu\n",
                    na, new_facts, got);
            fprintf(o, "debug: skipped=%zu\n", miss);
        }
        if (learned) *learned += got;
        if (skipped) *skipped += miss;
        if (miss > 0) {
            indent(o, depth);
            fprintf(o, "STOP: prima forma di prosa non compresa.\n");
            return 0;
        }
        p = q;
    }
    return 1;
}

static void indent(FILE *o, int depth) {
    for (int i = 0; i < depth; i++) fputs("  ", o);
}


/* I fatti arrivano dalla KB con le virgolette con cui sono stati scritti. */
static const char *kb_dequote_dream(const char *s) {
    static char buf[KB_TERM_LEN];
    size_t n = strlen(s);
    if (n >= 2 && s[0] == '"' && s[n - 1] == '"') {
        snprintf(buf, sizeof buf, "%.*s", (int)(n - 2), s + 1);
        return buf;
    }
    return s;
}

static size_t dream_fact_sources(KB *kb, char rows[][KB_TERM_LEN], size_t max) {
    if (!kb || !rows || max == 0) return 0;
    const char *q[] = { NULL, NULL, NULL };
    return kb_match(kb, "fact_source", q, 3, rows, max);
}

static size_t dream_reading_facts(KB *kb, char rows[][KB_TERM_LEN], size_t max) {
    if (!kb || !rows || max == 0) return 0;
    const char *q[] = { NULL, NULL };
    return kb_match(kb, "reading_fact", q, 2, rows, max);
}

static int dream_row_seen(char rows[][KB_TERM_LEN], size_t n, const char *row) {
    for (size_t i = 0; i < n; i++)
        if (strcmp(rows[i], row) == 0) return 1;
    return 0;
}

/* ── L'AGENDA DEL SOGNO SONO LE PROPRIE LACUNE (gen405) ────────────────────
 *
 * Fin qui il topic lo dava una persona, e il sogno era esplorazione pura: utile
 * da leggere, ma senza un criterio interno per dire se fosse servito a
 * qualcosa. Un sogno che non sa cosa gli manca puo' solo sapere di piu'; per
 * capire di piu' deve sapere DOVE si e' fermato.
 *
 * Le due sorgenti sono gia' scritte in KB dal ciclo conversazionale:
 *   pending_gap(parola)     — classe W: il turno nominava qualcosa di ignoto.
 *                             La parola E' gia' un topic: si sogna com'e'.
 *   machinery_gap("turno")  — classe M: le parole c'erano tutte e a mancare era
 *                             il ponte. Qui il topic non e' dato, va estratto:
 *                             le parole di contenuto del turno sono i semi.
 *
 * Notare l'asimmetria, ed e' quella di question-emergence.md §10.2: una lacuna
 * W si colma leggendo, una lacuna M no. Sognare su una lacuna M serve lo stesso
 * — spesso il ponte manca perche' manca la nozione sotto — ma la verifica e'
 * diversa, e infatti e' l'unica che sappiamo fare da soli. */
static void agenda_from_gaps(Brain *b, DreamState *st, FILE *o) {
    KB *kb = brain_kb(b);
    if (!kb) return;
    const char *q1[1] = { NULL };
    char rows[64][KB_TERM_LEN];

    size_t nw = kb_match(kb, "pending_gap", q1, 1, rows, 64);
    for (size_t i = 0; i < nw; i++) {
        char key[DREAM_KEY_LEN];
        key_normalize(kb_dequote_dream(rows[i]), key, sizeof key);
        if (*key && !seen_has(st, key)) { push(st, key, 0, "gap:W"); seen_add(st, key); st->from_gap_w++; }
    }

    char mrows[64][KB_TERM_LEN];
    size_t nm = kb_match(kb, "machinery_gap", q1, 1, mrows, 64);
    for (size_t i = 0; i < nm && st->nbridge < DREAM_MAX_BRIDGE; i++) {
        const char *turn = kb_dequote_dream(mrows[i]);
        snprintf(st->bridge[st->nbridge], sizeof st->bridge[0], "%s", turn);
        st->nbridge++;
        /* i semi: le parole di contenuto del turno, lunghe abbastanza da poter
         * essere il nome di qualcosa. La soglia e' grezza e va detta: e' la
         * stessa euristica del declino informato, e quando il lessico sapra'
         * distinguere una parola piena da una funzione, sparira'. */
        char buf[KB_TERM_LEN]; snprintf(buf, sizeof buf, "%s", turn);
        for (char *tok = strtok(buf, " \t,.;:?!"); tok; tok = strtok(NULL, " \t,.;:?!")) {
            if (strlen(tok) < 4) continue;
            char key[DREAM_KEY_LEN];
            key_normalize(tok, key, sizeof key);
            if (*key && !seen_has(st, key)) { push(st, key, 0, "gap:M"); seen_add(st, key); st->from_gap_m++; }
        }
    }
    fprintf(o, "agenda: %zu lacune di parola, %zu turni senza ponte -> %zu semi\n",
            nw, nm, st->from_gap_w + st->from_gap_m);
}

int dream_run(Brain *b, const char *topic, const DreamOpts *opts) {
    /* Un topic VUOTO non e' un errore: e' il sogno guidato dalle proprie lacune
     * (gen405). Il guardiano lo rifiutava prima ancora di guardarlo, quindi
     * `--dream` senza argomenti usciva in silenzio con zero nodi — e sembrava
     * che non ci fosse niente da sognare. */
    if (!b || !topic || !opts) return 0;
    FILE *o = opts->out ? opts->out : stdout;

    int max_depth = opts->max_depth > 0 ? opts->max_depth
                                        : dream_policy_int(b, "dream_max_depth", 2);
    int max_nodes = opts->max_nodes > 0 ? opts->max_nodes
                                        : dream_policy_int(b, "dream_max_nodes", 40);

    DreamState st;
    memset(&st, 0, sizeof st);
    st.cap = DREAM_MAX_FRONTIER;
    st.q = calloc(st.cap, sizeof *st.q);
    st.seen_cap = DREAM_MAX_FRONTIER;
    st.seen = calloc(st.seen_cap, DREAM_KEY_LEN);
    st.lost_cap = 512;
    st.lost = calloc(st.lost_cap, DREAM_KEY_LEN);
    if (!st.q || !st.seen || !st.lost) {
        free(st.q); free(st.seen); free(st.lost);
        return 0;
    }

    char root[DREAM_KEY_LEN];
    int guided = (*topic == '\0');
    if (guided) {
        snprintf(root, sizeof root, "%s", "(le proprie lacune)");
    } else {
        char surface[DREAM_KEY_LEN];
        key_normalize(topic, surface, sizeof surface);
        dream_topic_key(b, surface, root, sizeof root);
        push(&st, root, 0, NULL);
        seen_add(&st, root);
    }

    /* gen405: il sogno si dichiara. Una lacuna chiusa mentre e' in corso e' un
     * ponte trovato leggendo; una chiusa in conversazione l'ha portata qualcun
     * altro. Il registro deve poterli distinguere (vedi machinery_gap_close). */
    {
        const char *da[] = { "1" };
        kb_assert(brain_kb(b), "dreaming", da, 1);
    }

    if (!guided && strcmp(root, topic) != 0)
        fprintf(o, "dream: %s -> %s   (profondita' max %d, nodi max %d, sorgente Wikipedia in memoria)\n",
                topic, root, max_depth, max_nodes);
    else
        fprintf(o, "dream: %s   (profondita' max %d, nodi max %d, sorgente Wikipedia in memoria)\n",
                root, max_depth, max_nodes);
    if (guided) agenda_from_gaps(b, &st, o);

    fprintf(o, "%s\n", "----------------------------------------------------------------------");

    DreamNode node;
    while (pop(&st, &node) && (int)st.nodes < max_nodes) {
        st.nodes++;

        char prose[8192];
        size_t plen = 0;
        int fetched = 0;
        /* A topic dream always acquires the certified prose directly in RAM.
         * There is no local-page fallback and no opt-in fetch flag anymore. */
        if (!p0env("PARROT0_WIKI_FETCH")) p0env_set("PARROT0_WIKI_FETCH", "1");
        if (wiki_fetch_topic_lang_prose(node.key, "en", prose, sizeof prose)) {
            plen = strlen(prose);
            fetched = plen > 0;
        }

        indent(o, node.depth);
        if (!plen) {
            st.pages_missing++;
            lost_add(&st, node.key);
            fprintf(o, "· %s — nessuna pagina%s\n", node.key,
                    " (Wikipedia non ha restituito prosa)");
            continue;
        }
        if (fetched) st.pages_fetched++; else st.pages_local++;

        fprintf(o, "\n");
        indent(o, node.depth);
        fprintf(o, "pagina: %s%s\n", node.key, fetched ? " [fetch]" : "");
        size_t learned = 0, skipped = 0;
        int complete = dream_read_prose(b, prose, o, node.depth, opts->debug,
                                        &learned, &skipped);
        st.facts_seen += learned;
        st.prose_unread += skipped;
        if (!complete) {
            lost_add(&st, node.key);
            break;
        }
        if (node.depth >= max_depth) break;
    }

    /* ── gen411: IL RIMEDIO SI SCEGLIE ────────────────────────────────────
     *
     * Il sogno finora sapeva fare una cosa sola — leggere — e la faceva su ogni
     * lacuna. Ma l'ancora del gen406 dice quali lacune leggere puo' colmare e
     * quali no: se il turno non aveva nessuna parola opaca, tutte le parole
     * erano note e a mancare e' un PONTE. Su quella, leggere e' tempo perso per
     * definizione, e l'unica mossa e' proporre.
     *
     * Qui il sogno smette di essere un lettore e diventa un processo che decide
     * cosa fare delle proprie lacune. E' il senso di «il sogno e' il comando di
     * run», non un modo di imparare a parte.
     *
     * Va in fondo, dopo la lettura: cio' che si e' letto puo' aver chiuso da
     * solo qualche lacuna, e proporre un ponte per una lacuna gia' chiusa
     * sarebbe rumore. */
    {
        const char *da[] = { "1" };
        kb_retract(brain_kb(b), "dreaming", da, 1);
    }

    if (opts->persist) {
        /* Dove finisce cio' che il sogno ha imparato — e la prima versione lo
         * sbagliava in un modo istruttivo.
         *
         * Il salvataggio andava in PARROT0_SESSION, cioe' kb/core/session.p0, che
         * e' in .gitignore: il sogno imparava davvero e poi la conoscenza spariva
         * al primo commit. Una memoria che non sopravvive al commit non e'
         * memoria, e' una cache.
         *
         * La destinazione giusta e' l'albero CURATO, dove il save-map instrada
         * ogni fatto accanto ai suoi simili. Cosi' un sogno e' un contributo
         * committabile, non un giro a vuoto.
         *
         * gen411 (F.): la ricaduta e' quella di tutti, kb/learning/learned.p0, e
         * non un `dreamed.p0` a parte. Un secondo deposito indistinto e' un
         * secondo posto dove la conoscenza si ferma senza categoria, e la
         * domanda «di che cosa parla questo, e dove vive la sua specie?» va
         * fatta in un posto solo. Chi legge il sogno vede da dove viene un fatto
         * dal registro delle letture, non dal nome del file in cui e' caduto. */
        const char *dst = p0env("PARROT0_DREAM_KB");
        if (!dst || !*dst) dst = "kb/learning/learned.p0";
        if (!p0env("PARROT0_KB_ROOT")) p0env_set("PARROT0_KB_ROOT", "kb");
        brain_save_session(b, dst);
    }

    int nodes = (int)st.nodes;
    free(st.q); free(st.seen); free(st.lost);
    return nodes;
}
