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
    size_t nbridge, walls_closed, from_gap_w, from_gap_m, prose_unread;
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

/* La prosa della pagina: la sezione `## Extract` del corpus statico, cioe' la
 * stessa che legge l'estrattore. Ritorna il numero di byte scritti. */
static size_t page_prose(const char *key, char *out, size_t sz) {
    const char *dir = p0env("PARROT0_WIKI_DIR");
    if (!dir || !*dir) dir = "kb/learning/pages";
    char path[512];
    snprintf(path, sizeof path, "%s/%s.md", dir, key);
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    char line[2048];
    size_t o = 0;
    int in_extract = 0;
    out[0] = '\0';
    while (fgets(line, sizeof line, f)) {
        if (strncmp(line, "## Extract", 10) == 0) { in_extract = 1; continue; }
        if (in_extract && strncmp(line, "##", 2) == 0) break;
        if (!in_extract) continue;
        size_t l = strlen(line);
        if (o + l + 1 >= sz) break;
        memcpy(out + o, line, l);
        o += l;
        out[o] = '\0';
    }
    fclose(f);
    return o;
}

/* Un turno di conversazione, guidato: e' il punto in cui il sogno riusa la
 * comprensione invece di duplicarla. */
static void ask(Brain *b, const char *prompt, char *reply, size_t sz) {
    reply[0] = '\0';
    brain_respond(b, prompt, reply, sz);
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

/* LA VERIFICA, e non e' un giudizio: si ri-pone il turno che murava. Se ora
 * risponde, `turn_done` ritira la lacuna da solo (gen404) — quindi qui basta
 * guardare se il fatto e' sparito. E' il motivo per cui la classe M e' l'unica
 * decidibile: il rimedio si prova da se'.
 *
 * Il costo e' un turno per lacuna aperta, dopo ogni nodo che ha prodotto fatti.
 * Vale la pena solo se qualcosa e' entrato: sognare non cambia niente finche'
 * la KB non cresce. */
static size_t retry_open_walls(Brain *b, DreamState *st, FILE *o, int depth) {
    KB *kb = brain_kb(b);
    if (!kb) return 0;
    size_t closed = 0;
    for (size_t i = 0; i < st->nbridge; i++) {
        if (!st->bridge[i][0]) continue;
        char q[KB_TERM_LEN];
        snprintf(q, sizeof q, "\"%s\"", st->bridge[i]);
        const char *ga[1] = { q };
        if (!kb_query(kb, "machinery_gap", ga, 1)) { st->bridge[i][0] = 0; continue; }
        char reply[4096];
        ask(b, st->bridge[i], reply, sizeof reply);
        if (!kb_query(kb, "machinery_gap", ga, 1)) {
            closed++;
            st->bridge[i][0] = 0;
            indent(o, depth);
            fprintf(o, "  ✔ ponte trovato: «%s» -> %s\n", st->bridge[i], reply);
        }
    }
    st->walls_closed += closed;
    return closed;
}

int dream_run(Brain *b, const char *topic, const DreamOpts *opts) {
    if (!b || !topic || !*topic || !opts) return 0;
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
        key_normalize(topic, root, sizeof root);
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

    fprintf(o, "dream: %s   (profondita' max %d, nodi max %d, fetch %s)\n",
            root, max_depth, max_nodes, opts->fetch ? "on" : "off");
    if (guided) agenda_from_gaps(b, &st, o);
    fprintf(o, "%s\n", "----------------------------------------------------------------------");

    DreamNode node;
    while (pop(&st, &node) && (int)st.nodes < max_nodes) {
        st.nodes++;

        char prose[8192];
        size_t plen = page_prose(node.key, prose, sizeof prose);
        int fetched = 0;
        if (!plen && opts->fetch) {
            /* --fetch E' il consenso esplicito: senza questo la richiesta sarebbe
             * silenziosamente ignorata dal gate di learn.c e il sogno direbbe
             * "nessuna pagina" mentendo sul motivo. */
            if (!p0env("PARROT0_WIKI_FETCH")) p0env_set("PARROT0_WIKI_FETCH", "1");
            if (wiki_fetch_topic(node.key)) {
                plen = page_prose(node.key, prose, sizeof prose);
                fetched = plen > 0;
            }
        }

        indent(o, node.depth);
        if (!plen) {
            st.pages_missing++;
            lost_add(&st, node.key);
            fprintf(o, "· %s — nessuna pagina%s\n", node.key,
                    opts->fetch ? " (nemmeno da wikipedia)" : "");
            continue;
        }
        if (fetched) st.pages_fetched++; else st.pages_local++;

        /* gen405: quante forme di prosa hanno battuto l'estrattore su QUESTA
         * pagina. Va misurato PRIMA della lettura — la prima versione contava
         * dopo, e riportava zero mentre le frasi cadevano davvero. Il registro
         * delle lacune e' lo stesso della conversazione, quindi il conto di
         * questa pagina si legge solo come differenza. */
        size_t gaps_before;
        {
            const char *gq[1] = { NULL }; char rows[128][KB_TERM_LEN];
            gaps_before = kb_match(brain_kb(b), "machinery_gap", gq, 1, rows, 128);
        }

        /* La comprensione: lo stesso turno che userebbe una persona. */
        char prompt[256], reply[4096];
        snprintf(prompt, sizeof prompt, "read the page on %s", node.key);
        ask(b, prompt, reply, sizeof reply);

        const char *facts = strstr(reply, "extracted");
        if (facts) {
            st.facts_seen++;
            fprintf(o, "▸ %s%s\n", node.key, fetched ? "  [fetch]" : "");
            indent(o, node.depth);
            fprintf(o, "  %s\n", reply);
            {
                const char *gq[1] = { NULL }; char rows[128][KB_TERM_LEN];
                size_t after = kb_match(brain_kb(b), "machinery_gap", gq, 1, rows, 128);
                if (after > gaps_before) {
                    st.prose_unread += after - gaps_before;
                    indent(o, node.depth);
                    fprintf(o, "  ✗ %zu frasi non lette (forma sconosciuta)\n",
                            after - gaps_before);
                }
            }
            /* Qualcosa e' entrato: e' l'unico momento in cui vale la pena
             * ri-provare i turni che murarono. Sognare non cambia niente
             * finche' la KB non cresce. */
            if (st.nbridge) retry_open_walls(b, &st, o, node.depth);
        } else {
            st.silent_pages++;
            lost_add(&st, node.key);
            fprintf(o, "▸ %s — pagina letta ma NESSUN fatto: %s\n", node.key, reply);
        }

        if (node.depth >= max_depth) continue;

        /* Parola per parola, nell'ordine in cui la prosa le presenta. Le
         * stopword NON sono escluse: sognare "of" e non trovare nulla e' un
         * dato sul confine della comprensione, non rumore. */
        size_t queued = 0;
        char wbuf[8192];
        snprintf(wbuf, sizeof wbuf, "%s", prose);
        char words[512][DREAM_KEY_LEN];
        size_t nw = 0;
        for (char *tok = strtok(wbuf, " \t\r\n"); tok && nw < 512;
             tok = strtok(NULL, " \t\r\n")) {
            char key[DREAM_KEY_LEN];
            key_normalize(tok, key, sizeof key);
            if (!key[0] || seen_has(&st, key)) continue;
            seen_add(&st, key);
            snprintf(words[nw++], DREAM_KEY_LEN, "%s", key);
        }
        /* Impilate al contrario: cosi' la PRIMA parola della prosa e' la prima a
         * uscire, e la discesa segue l'ordine in cui il testo le presenta. */
        for (size_t i = nw; i-- > 0; ) {
            push(&st, words[i], node.depth + 1, node.key);
            queued++;
        }
        if (queued) {
            indent(o, node.depth);
            fprintf(o, "  → %zu parole in coda\n", queued);
        }
    }

    {
        const char *da[] = { "1" };
        kb_retract(brain_kb(b), "dreaming", da, 1);
    }

    fprintf(o, "%s\n", "----------------------------------------------------------------------");
    fprintf(o, "nodi sognati       %zu\n", st.nodes);
    fprintf(o, "  pagine locali    %zu\n", st.pages_local);
    fprintf(o, "  pagine scaricate %zu\n", st.pages_fetched);
    fprintf(o, "  pagine assenti   %zu\n", st.pages_missing);
    fprintf(o, "  lette ma mute    %zu\n", st.silent_pages);
    fprintf(o, "in coda non visti  %zu\n", st.tail - st.head);
    fprintf(o, "fatti nella KB     %zu\n", kb_size(brain_kb(b)));
    /* gen405 (F.): la meta' che mancava al trace. «lette ma mute» contava le
     * PAGINE da cui non e' uscito niente; questo conta le FRASI dentro le
     * pagine che invece hanno prodotto — la prosa che parrot0 ha avuto sotto
     * gli occhi e non ha saputo leggere. E' la misura di quanto il sogno
     * SEMBRI produttivo piu' di quanto sia, e l'agenda di cosa insegnargli. */
    fprintf(o, "frasi non lette    %zu  (forme di prosa che non riconosce)\n",
            st.prose_unread);
    /* E QUALI. Il numero dice quanto il sogno sembri produttivo piu' di quanto
     * sia; l'elenco dice cosa fare — ogni riga qui e' una forma di frase da
     * insegnare a leggere, e quando la si insegna il conto scende da solo. */
    if (st.prose_unread) {
        const char *gq[1] = { NULL }; char rows[128][KB_TERM_LEN];
        size_t n = kb_match(brain_kb(b), "machinery_gap", gq, 1, rows, 128);
        fprintf(o, "\nforme di prosa che lo battono:\n");
        for (size_t i = 0; i < n && i < 12; i++)
            fprintf(o, "  · %s\n", kb_dequote_dream(rows[i]));
        if (n > 12) fprintf(o, "  … e altre %zu\n", n - 12);
    }

    /* gen405: LA RESA, che e' la sola cosa che dice se il sogno e' servito.
     *
     * «fatti nella KB» misura quanto parrot0 SA; questo misura quanto ha
     * CAPITO in piu': quanti turni che murarono ora rispondono. Un sogno che
     * estrae mille fatti e non chiude nessun ponte non e' un successo piccolo,
     * e' un dato — dice che stava leggendo la cosa sbagliata, o che la lacuna
     * non era di conoscenza. Tenere separate le due misure e' cio' che
     * impedisce di scambiare l'accumulo per la comprensione. */
    if (st.nbridge || st.from_gap_w || st.from_gap_m) {
        fprintf(o, "\nresa sulle lacune\n");
        fprintf(o, "  semi da lacune   %zu (parola %zu, ponte %zu)\n",
                st.from_gap_w + st.from_gap_m, st.from_gap_w, st.from_gap_m);
        fprintf(o, "  ponti trovati    %zu\n", st.walls_closed);
        size_t still = 0;
        for (size_t i = 0; i < st.nbridge; i++) if (st.bridge[i][0]) still++;
        if (still) {
            fprintf(o, "  ancora aperti    %zu:\n", still);
            for (size_t i = 0; i < st.nbridge; i++)
                if (st.bridge[i][0]) fprintf(o, "    · %s\n", st.bridge[i]);
        }
    }

    /* Cio' che si e' PERSO, per nome. E' la meta' del trace che conta: un sogno
     * che mostra solo cio' che ha imparato non permette di accorgersi di nulla. */
    if (st.nlost) {
        fprintf(o, "\nperso per strada (%zu):\n  ", st.nlost);
        for (size_t i = 0; i < st.nlost; i++)
            fprintf(o, "%s%s", i ? ", " : "", st.lost[i]);
        fprintf(o, "\n");
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
         * ogni fatto accanto ai suoi simili (PARROT0_KB_ROOT). Il file di ricaduta
         * e' kb/learning/dreamed.p0 — versionato — per cio' che il routing non sa
         * dove mettere. Cosi' un sogno e' un contributo committabile, non un giro
         * a vuoto. */
        const char *dst = p0env("PARROT0_DREAM_KB");
        if (!dst || !*dst) dst = "kb/learning/dreamed.p0";
        if (!p0env("PARROT0_KB_ROOT")) p0env_set("PARROT0_KB_ROOT", "kb");
        int n = brain_save_session(b, dst);
        fprintf(o, "\npersistito: %d clausole instradate nell'albero curato "
                   "(save-map, ricaduta %s)\n", n, dst);
    }

    int nodes = (int)st.nodes;
    free(st.q); free(st.seen); free(st.lost);
    return nodes;
}
