/* testeng.c — the parrot0 test-engine (gen345, docs/plans/test-engine.md).
 *
 * A local validation SERVICE. `parrot0 --test-engine` boots ONE brain and listens
 * on a Unix socket; `make test` then sends each `.p0t` file as its own connection
 * (`parrot0 --test FILE`) and finally `parrot0 --test-report`. The KB loads
 * once, in the daemon; the client loads NOTHING — it is a bare socket relay, so
 * every `make test` line is cheap.
 *
 * ── .p0t GRAMMAR ──────────────────────────────────────────────────────────────
 *
 *   # comment                 a comment line, ignored (also: blank lines)
 *   [test NAME]               open a named section (for the report)
 *
 *   > text                    send `text` to parrot0 as one user turn
 *   < text                    assert the reply equals `text`
 *   <~ text                   assert the reply CONTAINS `text`
 *   <! text                   assert the reply does NOT contain `text`
 *   !expect SOURCE text       assert a primitive output contains `text`
 *   !expect! SOURCE text      assert a primitive output lacks `text`
 *   !expect= SOURCE text      assert a primitive output equals `text`
 *   !random NAME LENGTH       bind a fresh lowercase string; `${NAME}` is
 *                             expanded in later test lines
 *
 * Exact match is right for a short, fully determined reply. A generated analytical
 * answer is a paragraph, and a growth contract asserts that one phrase appears or
 * disappears when a fact is taught or retracted — pinning the whole paragraph would
 * be brittle and would miss the point.
 *
 *   !set NAME=VALUE           pilot a runtime config global (env.h): PARROT0_BASE,
 *                             PARROT0_WORLD_FACTS, PARROT0_LANG, PARROT0_ORACLE,
 *                             HOME, PARROT0_PID, … The brain reloads only when the
 *                             effective memory-config actually MOVES (a no-op set —
 *                             a repeat, or a value equal to the real current one —
 *                             costs nothing; a per-turn var like HOME needs no
 *                             reload at all).
 *   !reload                   apply the config reload now (no-op if the effective
 *                             memory-config did not change)
 *   !reset                    virgin brain with the current config, for per-case
 *                             isolation (opt-in) — but SMART: it is skipped when
 *                             the brain is ALREADY virgin for this config, i.e.
 *                             neither the loading footprint changed NOR anything
 *                             was taught since the last clean load. Mesh tests that
 *                             want state to persist across files just omit it.
 *   !timeout SECONDS          per-test turn budget (default 1s, reset at each
 *                             [test]). A turn slower than the budget is a FAILURE
 *                             (a perf-regression guard); 0 disables it.
 *   !forget PRED              switch a predicate OFF: retract every fact of it
 *   !exec CMD                 run CMD; `!exec` wants exit 0, `!exec!` wants it to
 *   !exec! CMD                fail. The output takes the reply's place, so the
 *                             `<~`/`<!` lines below check it like any other. It is
 *                             the wildcard — prefer a punctual primitive when one
 *                             fits: it says what it looks at, a command does not.
 *   !direxists PATH           assert a directory is there / is not.
 *   !dirmissing PATH
 *   !fileexists PATH          assert the file is there / is not. Agnostic: they
 *   !filemissing PATH         look at the DISK and nothing else.
 *   !fileclean PATH text      drop every line containing `text` — run it BEFORE
 *                             a test so an interrupted run cannot fail the next
 *                             one, and again AFTER so the repository is left as
 *                             it was found.
 *   !filehas PATH text        assert a file does / does not contain `text`.
 *   !filelacks PATH text      Some promises are a FILE, not a reply: the save-map
 *                             says a learned fact lands next to its kin, and only
 *                             looking at where it went can check that. These work
 *                             on the REAL KB — a test that builds itself a fake
 *                             tree measures a creature we do not ship.
 *   !symlink TARGET NAME      create a symlink fixture (removed at the next
 *                             `[test …]` and at end of file). The interesting
 *                             containment case is the symlink whose NAME is
 *                             innocent and inside the workspace: only resolving
 *                             it shows it points out. Testing that needs to
 *                             CREATE one, which had no .p0t form.
 *   !sandbox / !sandbox off   run inside a private 0700 temp dir (and clean it up).
 *                             Some tests need a WRITABLE cwd — the code judge
 *                             compiles and runs a candidate, writing a work tree
 *                             where it stands. In shell that was `mktemp -d` +
 *                             `cd`; it had no .p0t form, which is why that family
 *                             stayed out. It closes itself at the next `[test …]`
 *                             and at end of file: the daemon is shared, so a
 *                             sandbox left open would silently break every test
 *                             after it.
 *   !mcp TOOL {json}          invoke ONE MCP tool on the SAME brain and put its
 *                             JSON payload where the reply goes, so the `<`/`<~`/`<!`
 *                             lines below check it like any other answer. The MCP
 *                             layer used to be reachable only from outside, over
 *                             JSON-RPC to a separate process — which is why a dozen
 *                             suites still lived in shell. This walks the same road
 *                             as `tools/call`, so it proves the real MCP surface.
 *
 *                               !mcp kb.assert {"pred":"dog","args":["rex"]}
 *                               !mcp kb.query  {"pred":"dog","args":["rex"]}
 *                               <~ true
 *   !forget PRED(a, b)        drop one specific ground fact
 *   !forget @LAYER            drop a whole provenance layer: @base, @session,
 *                             @induced, @reflective, @hypothetical
 *   !assert PRED(a, b, …)     add a ground fact from inside the test
 *   !query  PRED(a, $X)       assert that a FACT is provable ($X = free slot)
 *   !query! PRED(a, $X)       assert that it is NOT
 *
 * On `!forget` (F.): what a test needs ABSENT is the test's job, not the load's.
 * The KB is part of parrot0, not a mounted volume, so knowledge is subtracted
 * from INSIDE the dialogue — a test can teach something, use it, forget it, and
 * assert that the answer changed. Prefer this over amputating the KB at load time
 * with `!set PARROT0_WORLD_FACTS=0` / empty BASE: profiles and env stay for
 * high-level BEHAVIOUR, never for making a fact disappear.
 *
 * A section is MULTI-TURN: list several `> / <` pairs and they run in order
 * against the same live brain. A reply is MULTI-LINE: write one `<` per output
 * line and consecutive `<` lines are matched against the whole multi-line reply.
 *
 * Two state axes make !reload and !reset exact and cheap: the config SIGNATURE
 * (what the brain loaded with) and the LEARNED delta (kb grew since that load).
 * `!shutdown` is the internal control line (sent by --test-report), not a test
 * primitive.
 */
#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include "testeng.h"
#include "env.h"
#include "kb.h"
#include "mcp.h"

#ifndef TE_LINE
#define TE_LINE 4096
/* La risposta di un turno sta in poche righe; il payload di uno strumento MCP
 * no. `input.segment` puo' restituire 128 span annotati con la loro
 * provenienza, e troncarlo farebbe fallire un'asserzione per la ragione
 * sbagliata — non «il contenuto non c'e'» ma «non ci stava». */
#define TE_REPLY 70000
#endif
#define TE_NAME 64
#define TE_RANDOM_MAX 128
#define TE_VARS 32

enum { TE_OUTPUT_NONE = 0, TE_OUTPUT_TURN, TE_OUTPUT_MCP, TE_OUTPUT_EXEC };

typedef struct {
    Brain *b;
    FILE  *out;              /* where the report is written (a socket or stdout) */

    char section[TE_NAME];   /* current [section] name, for the report */

    char reply[TE_REPLY];    /* reply from the most recent `>` turn, o payload !mcp */
    int  have_reply;
    int output_source;       /* TE_OUTPUT_*: where the latest result came from */

    char expect[TE_LINE];    /* accumulated consecutive `<` expected lines */
    size_t expect_len;
    int  have_expect;
    int  expect_startline;
    int  expect_source;
    struct { char name[TE_NAME]; char value[TE_RANDOM_MAX]; } vars[TE_VARS];
    size_t n_vars;
    /* !sandbox: la directory privata in cui il test lavora, e quella da cui
     * veniva. Il demone e' uno solo e condiviso, quindi una sandbox non chiusa
     * sarebbe un guasto silenzioso per ogni test successivo: si chiude da se'. */
    char sandbox_dir[512];
    char sandbox_prev[512];
    int  in_sandbox;
    /* !symlink: le fixture di filesystem create da un test. Si tolgono da sole,
     * come la sandbox: un link lasciato nel workspace e' sporcizia che il test
     * dopo si ritrova fra i piedi. */
    char fixture[8][512];
    size_t n_fixture;
    /* gen377: how `expect` is compared. Exact match is right for a short, fully
     * determined reply, but a generated ANALYTICAL answer is a paragraph, and what
     * a growth test asserts is that one phrase APPEARS or DISAPPEARS when a fact is
     * taught or retracted. Pinning the whole paragraph would be brittle and would
     * miss the point. TE_EXPECT_HAS / _LACKS say exactly what is meant. */
    enum { TE_EXPECT_EXACT = 0, TE_EXPECT_HAS = 1, TE_EXPECT_LACKS = 2 } expect_mode;

    /* The brain's state is signed on TWO axes so both !reload and !reset can tell
     * exactly what (if anything) needs redoing:
     *   loaded_sig    — the KB-LOADING footprint (which config the brain booted
     *                   with). Only a change here needs a full reload from disk.
     *   clean_kb_size — the KB size right after that clean load. A later turn that
     *                   TAUGHT something grows it; that is the "learned" axis a
     *                   !reset must undo. If neither axis moved, the brain is
     *                   already virgin for this config and nothing is done. */
    char loaded_sig[2048];
    size_t clean_kb_size;
    double timeout_sec;      /* per-test budget for a turn (default 1s; 0 = off) */
    int poisoned;            /* the current turn already failed (e.g. on timeout) */
    int passed, failed, line_no, shutdown;
    char bench_category[TE_NAME];
    void (*bench_record)(void *ctx, const char *category, int passed);
    void *bench_record_ctx;
} TeState;

#define TE_DEFAULT_TIMEOUT 1.0   /* each test's turn budget unless it says otherwise */

/* record the just-loaded clean state on both axes */
static void te_mark_clean(TeState *t) {
    p0env_mem_signature(t->loaded_sig, sizeof t->loaded_sig);
    t->clean_kb_size = kb_size(brain_kb(t->b));
}

/* did a turn teach anything since the last clean load? (the "learned" axis) */
static int te_learned(TeState *t) {
    return kb_size(brain_kb(t->b)) != t->clean_kb_size;
}

/* Reload the brain ONLY if the effective memory-config actually differs from what
 * it is currently loaded with. This is the fully-smart reload the design asks for:
 * a batch of `!set`s costs at most one reload, and `!set`s that don't change any
 * effective value (identical repeats, or a value equal to the real current one)
 * cost none — the signatures match, so nothing happens. */
static void te_apply_config(TeState *t) {
    char cur[2048];
    p0env_mem_signature(cur, sizeof cur);
    if (strcmp(cur, t->loaded_sig) == 0) return;    /* nothing effective changed */
    /* La KB si carica da percorsi RELATIVI alla radice del repository. Dentro
     * una sandbox la directory corrente e' un'altra, quindi un reload li' non
     * troverebbe ne' il profilo ne' la base: il brain ripartirebbe monco e il
     * test misurerebbe una creatura diversa da quella che voleva. La sandbox
     * serve a dove gli STRUMENTI scrivono, non a dove la conoscenza vive. */
    int hopped = 0;
    if (t->in_sandbox && t->sandbox_prev[0] && chdir(t->sandbox_prev) == 0) hopped = 1;
    brain_reload(t->b);
    if (hopped && t->sandbox_dir[0]) { if (chdir(t->sandbox_dir) != 0) { /* nulla */ } }
    t->have_reply = 0;
    t->output_source = TE_OUTPUT_NONE;
    te_mark_clean(t);
    if (getenv("PARROT0_TE_DEBUG"))
        fprintf(stderr, "test-engine: brain reloaded (config changed)\n");
}

/* ── assertion ─────────────────────────────────────────────────────────────── */

/* `<~` e `<!` confrontano SENZA distinguere maiuscole e minuscole.
 *
 * Il confronto esatto (`<`) resta letterale: li' si sta fissando una risposta
 * breve e determinata, e la forma conta. Ma `<~` e `<!` asseriscono che una
 * PAROLA compare o non compare in una risposta in lingua naturale, e li' la
 * maiuscola e' un accidente della resa: parrot0 scrive «Orwell.» a inizio frase
 * e «orwell» dentro un elenco, e un'asserzione che passa o fallisce per quello
 * non misura niente — produce falsi negativi, che in una batteria di misura
 * sono peggio di un buco (gen412, trovato scrivendo la batteria di rinforzo).
 *
 * Su `<!` la insensibilita' e' anche piu' stretta: «non deve comparire nera»
 * deve valere anche per «Nera». */
static const char *te_casestr(const char *hay, const char *needle) {
    if (!hay || !needle) return NULL;
    if (!*needle) return hay;
    for (const char *h = hay; *h; h++) {
        const char *a = h, *b = needle;
        while (*a && *b && tolower((unsigned char)*a) == tolower((unsigned char)*b)) { a++; b++; }
        if (!*b) return h;
    }
    return NULL;
}

static const char *te_var(const TeState *t, const char *name) {
    for (size_t i = 0; i < t->n_vars; i++)
        if (strcmp(t->vars[i].name, name) == 0) return t->vars[i].value;
    return NULL;
}

static int te_expand(const TeState *t, const char *in, char *out, size_t cap) {
    size_t o = 0;
    for (size_t i = 0; in[i]; i++) {
        if (in[i] == '$' && in[i + 1] == '{') {
            const char *end = strchr(in + i + 2, '}');
            if (end) {
                size_t nl = (size_t)(end - (in + i + 2));
                char name[TE_NAME];
                if (nl > 0 && nl < sizeof name) {
                    memcpy(name, in + i + 2, nl); name[nl] = '\0';
                    const char *value = te_var(t, name);
                    if (value) {
                        size_t vl = strlen(value);
                        if (o + vl + 1 > cap) return 0;
                        memcpy(out + o, value, vl); o += vl;
                        i = (size_t)(end - in);
                        continue;
                    }
                }
            }
        }
        if (o + 2 > cap) return 0;
        out[o++] = in[i];
    }
    out[o] = '\0';
    return 1;
}

static int te_random_string(char *out, size_t len) {
    static const char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
    unsigned char bytes[TE_RANDOM_MAX];
    FILE *f = fopen("/dev/urandom", "rb");
    size_t got = f ? fread(bytes, 1, len, f) : 0;
    if (f) fclose(f);
    if (got != len) {
        unsigned long seed = (unsigned long)time(NULL) ^ (unsigned long)getpid();
        for (size_t i = 0; i < len; i++) {
            seed = seed * 1103515245UL + 12345UL;
            bytes[i] = (unsigned char)(seed >> 16);
        }
    }
    for (size_t i = 0; i < len; i++) out[i] = alphabet[bytes[i] % 26];
    out[len] = '\0';
    return 1;
}

static int te_sandbox_enter(TeState *t);
static void te_sandbox_leave(TeState *t);
static void te_fixtures_clear(TeState *t);

static int te_sandbox_enter(TeState *t) {
    if (t->in_sandbox) te_sandbox_leave(t);
    if (!getcwd(t->sandbox_prev, sizeof t->sandbox_prev)) return 0;
    const char *tmp = getenv("TMPDIR");
    if (!tmp || !*tmp) tmp = "/tmp";
    snprintf(t->sandbox_dir, sizeof t->sandbox_dir,
             "%s/parrot0-p0t-sandbox.XXXXXX", tmp);
    if (!mkdtemp(t->sandbox_dir)) return 0;
    if (chdir(t->sandbox_dir) != 0) return 0;
    t->in_sandbox = 1;
    return 1;
}

static void te_fixtures_clear(TeState *t) {
    for (size_t i = 0; i < t->n_fixture; i++)
        if (t->fixture[i][0]) unlink(t->fixture[i]);
    t->n_fixture = 0;
}

static void te_sandbox_leave(TeState *t) {
    if (!t->in_sandbox) return;
    if (t->sandbox_prev[0]) { if (chdir(t->sandbox_prev) != 0) { /* niente da fare */ } }
    /* si porta via il proprio albero: una sandbox che resta e' spazzatura che
     * si accumula a ogni `make test`. */
    if (t->sandbox_dir[0]) {
        char cmd[600];
        snprintf(cmd, sizeof cmd, "rm -rf '%s'", t->sandbox_dir);
        if (system(cmd) != 0) { /* best effort */ }
    }
    t->in_sandbox = 0;
    t->sandbox_dir[0] = '\0';
    t->sandbox_prev[0] = '\0';
}

static void te_flush(TeState *t) {
    if (t->poisoned) {                /* the turn already failed (timeout) — absorb its < */
        t->poisoned = 0;
        t->expect_len = 0; t->expect[0] = '\0'; t->have_expect = 0;
        return;
    }
    if (!t->have_expect) return;
    const char *got = t->have_reply ? t->reply : "";
    int ok = t->output_source == t->expect_source;
    if (ok) {
        switch (t->expect_mode) {
            case TE_EXPECT_HAS:   ok = te_casestr(got, t->expect) != NULL; break;
            case TE_EXPECT_LACKS: ok = te_casestr(got, t->expect) == NULL; break;
            default:              ok = strcmp(t->expect, got) == 0;    break;
        }
    }
    if (ok) {
        t->passed++;                         /* silent — the one-line file report counts it */
    } else {
        t->failed++;                         /* only failures print, with useful detail */
        fprintf(t->out, "  FAIL  [%s] line %d\n",
                t->section[0] ? t->section : "-", t->expect_startline);
        if (t->output_source != t->expect_source) {
            const char *actual = t->output_source == TE_OUTPUT_MCP ? "mcp" :
                                 t->output_source == TE_OUTPUT_EXEC ? "exec" :
                                 t->output_source == TE_OUTPUT_TURN ? "turn" : "none";
            const char *wanted = t->expect_source == TE_OUTPUT_MCP ? "mcp" :
                                 t->expect_source == TE_OUTPUT_EXEC ? "exec" :
                                 t->expect_source == TE_OUTPUT_TURN ? "turn" : "none";
            fprintf(t->out, "        expected source: %s, got source: %s\n", wanted, actual);
        }
        fprintf(t->out, "        expected%s: %s\n",
                t->expect_mode == TE_EXPECT_HAS   ? " (contains)" :
                t->expect_mode == TE_EXPECT_LACKS ? " (absent)"   : "", t->expect);
        fprintf(t->out, "        got:      %s\n", got);
    }
    if (t->bench_record)
        t->bench_record(t->bench_record_ctx, t->bench_category, ok);
    t->expect_len = 0;
    t->expect[0] = '\0';
    t->have_expect = 0;
    t->expect_mode = TE_EXPECT_EXACT;
    t->expect_source = TE_OUTPUT_NONE;
}

static void te_turn(TeState *t, const char *text) {
    te_flush(t);
    te_apply_config(t);               /* reload lazily iff the config really moved */
    /* time ONLY the turn itself (a reload above is infrastructure, not the test). */
    struct timespec ta, tb;
    clock_gettime(CLOCK_MONOTONIC, &ta);
    brain_respond(t->b, text, t->reply, sizeof t->reply);
    clock_gettime(CLOCK_MONOTONIC, &tb);
    size_t n = strlen(t->reply);
    while (n > 0 && (t->reply[n - 1] == '\n' || t->reply[n - 1] == '\r'))
        t->reply[--n] = '\0';
    t->have_reply = 1;
    t->output_source = TE_OUTPUT_TURN;
    {
        /* gen382 — the SLOW-TURN LEDGER, independent of the per-test budget.
         *
         * `!timeout N` raises the bar for one test, and a raised bar hides what it
         * covers: a turn that quietly costs 8s reads as green forever. With
         * PARROT0_TE_SLOW=<seconds> every turn above the threshold is named on
         * stderr whatever its budget, so the overrides can be re-examined against
         * measurement instead of belief. Use it after any change to the KB size or
         * the resolution path: `PARROT0_TE_SLOW=0.2 make test`. */
        static double slow_at = -1.0;
        if (slow_at < 0) {
            const char *env = getenv("PARROT0_TE_SLOW");
            slow_at = env && *env ? atof(env) : 0.0;
        }
        if (slow_at > 0) {
            double el = (double)(tb.tv_sec - ta.tv_sec) +
                        (double)(tb.tv_nsec - ta.tv_nsec) / 1e9;
            if (el >= slow_at)
                fprintf(stderr, "  slow  %6.2fs (budget %.2fs) [%s] line %d — %s\n",
                        el, t->timeout_sec, t->section[0] ? t->section : "-",
                        t->line_no, text);
        }
    }
    if (t->timeout_sec > 0) {
        double el = (double)(tb.tv_sec - ta.tv_sec) +
                    (double)(tb.tv_nsec - ta.tv_nsec) / 1e9;
        if (el > t->timeout_sec) {    /* too slow IS a failure (perf regression guard) */
            t->failed++;
            fprintf(t->out, "  FAIL  [%s] line %d — turn took %.2fs (timeout %.2fs)\n",
                    t->section[0] ? t->section : "-", t->line_no, el, t->timeout_sec);
            fprintf(t->out, "        > %s\n", text);
            t->poisoned = 1;          /* absorb the following < so it isn't double-judged */
        }
    }
}

static void te_expect_mode(TeState *t, const char *raw, int mode) {
    if (t->have_expect && (mode != TE_EXPECT_EXACT || t->expect_mode != TE_EXPECT_EXACT))
        te_flush(t);                     /* a substring assertion stands alone */
    t->expect_mode = mode;
    t->expect_source = TE_OUTPUT_TURN;
    if (!t->have_expect) { t->have_expect = 1; t->expect_startline = t->line_no; }
    else if (t->expect_len + 1 < sizeof t->expect) {
        t->expect[t->expect_len++] = '\n';        /* consecutive `<` = one reply */
        t->expect[t->expect_len] = '\0';
    }
    size_t rl = strlen(raw);
    if (t->expect_len + rl < sizeof t->expect) {
        memcpy(t->expect + t->expect_len, raw, rl + 1);
        t->expect_len += rl;
    }
}

static void te_expect_primitive(TeState *t, const char *source, const char *raw, int mode) {
    int output = TE_OUTPUT_NONE;
    if (strcmp(source, "mcp") == 0) output = TE_OUTPUT_MCP;
    else if (strcmp(source, "exec") == 0) output = TE_OUTPUT_EXEC;
    te_expect_mode(t, raw, mode);
    t->expect_source = output;
}

static void te_expect(TeState *t, const char *raw) {
    te_expect_mode(t, raw, TE_EXPECT_EXACT);
}

/* gen396: split `a, b, "c, d"` respecting quotes.
 *
 * The separator scan used to ignore quoting, so a fact whose text carried a
 * comma — which is most curated prose in this KB — was silently chopped into
 * extra arguments and then truncated at the arity ceiling. `!assert` stored the
 * wrong fact and `!forget` retracted nothing, both without a word: a ratchet
 * that cannot express the fact it is about is worse than a missing one, because
 * it still reports green. Advances `*end` past the closing paren. */
static size_t te_split_args(char *q, char argbuf[KB_MAX_ARGS][KB_TERM_LEN],
                            const char *args[KB_MAX_ARGS], char **end) {
    size_t argc = 0;
    while (*q && *q != ')' && argc < KB_MAX_ARGS) {
        while (*q == ' ' || *q == '\t') q++;
        size_t a = 0;
        int quoted = 0, depth = 0;
        while (*q && a + 1 < KB_TERM_LEN) {
            /* gen395: un argomento puo' essere un TERMINE COMPOSTO.
             *
             * Spezzando su ogni virgola fuori dalle virgolette, `!forget
             * holds_in(w, fact(r, s, o))` diventava quattro argomenti e non
             * ritraeva nulla — in silenzio, perche' retract su un fatto che non
             * esiste non e' un errore. Il dialetto della KB annida i termini
             * (`cons/2`, `fact/3`, le proposizioni reificate del gen395), quindi
             * il dialetto dei test deve saperli NOMINARE: un ratchet che non puo'
             * dire cosa gli serve assente non e' un ratchet. La profondita' di
             * parentesi e' l'unica cosa che serve saper contare. */
            if (*q == '"') quoted = !quoted;
            else if (!quoted && *q == '(') depth++;
            else if (!quoted && *q == ')' && depth > 0) depth--;
            else if (!quoted && depth == 0 && (*q == ',' || *q == ')')) break;
            argbuf[argc][a++] = *q++;
        }
        while (a > 0 && (argbuf[argc][a-1] == ' ' || argbuf[argc][a-1] == '\t')) a--;
        argbuf[argc][a] = '\0';
        args[argc] = argbuf[argc];
        argc++;
        if (*q == ',') q++;
    }
    *end = q;
    return argc;
}

/* ── per-stream driver (shared by socket connections and the batch mode) ─────── */

/* Report goes to t->out. Sets t->shutdown on the control line. Returns 2 on a
 * syntax error, else 0. */
static int te_process_stream(TeState *t, FILE *in) {
    char line[TE_LINE];
    int syntax_err = 0;

    while (fgets(line, sizeof line, in)) {
        t->line_no++;
        size_t n = strlen(line);
        while (n > 0 && (line[n - 1] == '\n' || line[n - 1] == '\r')) line[--n] = '\0';

        char *p = line;
        while (*p == ' ' || *p == '\t') p++;

        if (*p == '\0' || *p == '#') continue;

        if (strncmp(p, "!bench-category", 15) == 0 &&
            (p[15] == ' ' || p[15] == '\t')) {
            te_flush(t);
            const char *category = p + 15;
            while (*category == ' ' || *category == '\t') category++;
            snprintf(t->bench_category, sizeof t->bench_category, "%s", category);
            continue;
        }

        if (*p == '[') {
            te_flush(t);
            te_sandbox_leave(t);   /* una sezione non eredita la sandbox di prima */
            te_fixtures_clear(t);  /* ne' le sue fixture di filesystem */
            char *close = strchr(p, ']');
            if (!close) { syntax_err = 1; continue; }
            *close = '\0';
            char *hdr = p + 1;
            char *sp = strchr(hdr, ' ');          /* label = text after the type word */
            snprintf(t->section, sizeof t->section, "%s", sp ? sp + 1 : hdr);
            t->timeout_sec = TE_DEFAULT_TIMEOUT;  /* each test starts at the 1s default */
            continue;                             /* section name surfaces only in a FAIL */
        }
        if (strncmp(p, "!random", 7) == 0 && (p[7] == ' ' || p[7] == '\t')) {
            te_flush(t);
            char *q = p + 7;
            while (*q == ' ' || *q == '\t') q++;
            char name[TE_NAME]; size_t nl = 0;
            while (*q && *q != ' ' && *q != '\t' && nl + 1 < sizeof name)
                name[nl++] = *q++;
            name[nl] = '\0';
            while (*q == ' ' || *q == '\t') q++;
            char *end = NULL; long length = strtol(q, &end, 10);
            while (end && (*end == ' ' || *end == '\t')) end++;
            if (!nl || !*q || (end && *end) || length < 1 || length >= TE_RANDOM_MAX ||
                t->n_vars >= TE_VARS) { syntax_err = 1; continue; }
            size_t slot = t->n_vars++;
            snprintf(t->vars[slot].name, sizeof t->vars[slot].name, "%s", name);
            te_random_string(t->vars[slot].value, (size_t)length);
            continue;
        }
        char expanded[TE_LINE];
        if (!te_expand(t, p, expanded, sizeof expanded)) { syntax_err = 1; continue; }
        p = expanded;
        if (strncmp(p, "!expect", 7) == 0 &&
            (p[7] == ' ' || p[7] == '\t' || p[7] == '!' || p[7] == '=')) {
            te_flush(t);
            int mode = TE_EXPECT_HAS;
            size_t prefix = 7;
            if (p[7] == '!') { mode = TE_EXPECT_LACKS; prefix++; }
            else if (p[7] == '=') { mode = TE_EXPECT_EXACT; prefix++; }
            char *q = p + prefix;
            while (*q == ' ' || *q == '\t') q++;
            char source[TE_NAME]; size_t k = 0;
            while (*q && *q != ' ' && *q != '\t' && k + 1 < sizeof source)
                source[k++] = *q++;
            source[k] = '\0';
            while (*q == ' ' || *q == '\t') q++;
            if (!k || !*q || (strcmp(source, "mcp") != 0 && strcmp(source, "exec") != 0)) {
                syntax_err = 1;
                continue;
            }
            te_expect_primitive(t, source, q, mode);
            continue;
        }
        if (p[0] == '>') { te_turn(t, p[1] == ' ' ? p + 2 : p + 1); continue; }
        if (p[0] == '<' && p[1] == '~') {          /* reply CONTAINS this */
            const char *v = p + 2; if (*v == ' ') v++;
            te_expect_mode(t, v, TE_EXPECT_HAS); continue;
        }
        if (p[0] == '<' && p[1] == '!') {          /* reply does NOT contain this */
            const char *v = p + 2; if (*v == ' ') v++;
            te_expect_mode(t, v, TE_EXPECT_LACKS); continue;
        }
        if (p[0] == '<') { te_expect(t, p[1] == ' ' ? p + 2 : p + 1); continue; }
        if (strncmp(p, "!shutdown", 9) == 0) { te_flush(t); t->shutdown = 1; continue; }
        if (strncmp(p, "!reload", 7) == 0) { te_flush(t); te_apply_config(t); continue; }
        if (strncmp(p, "!timeout", 8) == 0 && (p[8] == ' ' || p[8] == '\t')) {
            const char *q = p + 8; while (*q == ' ' || *q == '\t') q++;
            t->timeout_sec = atof(q);   /* seconds (may be fractional); 0 disables */
            continue;
        }
        if (strncmp(p, "!reset", 6) == 0) {
            /* Isolation, opt-in — but SMART: a virgin brain with this exact config
             * is already what a reset would produce, so skip when NEITHER axis
             * moved (config unchanged AND nothing was taught since the last clean
             * load). Consecutive query-only cases with the same footprint pay no
             * reset; a case that taught facts, or changed the config, does reset.
             * Mesh tests that WANT state to persist across files just omit !reset. */
            te_flush(t);
            char cur[2048];
            p0env_mem_signature(cur, sizeof cur);
            int config_changed = strcmp(cur, t->loaded_sig) != 0;
            if (!config_changed && !te_learned(t)) {
                if (getenv("PARROT0_TE_DEBUG"))
                    fprintf(stderr, "test-engine: reset skipped (already virgin, config unchanged)\n");
                continue;
            }
            brain_reload(t->b);
            t->have_reply = 0;
            t->output_source = TE_OUTPUT_NONE;
            te_mark_clean(t);
            if (getenv("PARROT0_TE_DEBUG"))
                fprintf(stderr, "test-engine: brain reset (virgin)\n");
            continue;
        }
        /* `!forget <pred>`  — switch a predicate OFF from inside the test.
         * `!forget <pred>(a, b)` — drop one specific ground fact.
         *
         * F.'s rule: what a test needs absent must be handled BY THE TEST, not by
         * piloting the KB's load from outside (`!set PARROT0_WORLD_FACTS=0` and
         * friends). The KB is part of parrot0, not a mounted volume: subtracting
         * knowledge is a move inside the dialogue, so the same test can teach
         * something, use it, then make parrot0 forget it and prove the answer
         * changes. Profiles stay for high-level BEHAVIOUR, never for this. */
        /* `!assert PRED(a, b, …)` — the WRITE twin of !forget. A test could already
         * subtract knowledge from inside the dialogue but not add an arbitrary n-ary
         * fact, so growth contracts (teach a cue, probe, retract, probe) had to live
         * in shell scripts driving the MCP engine. The asymmetry was the reason the
         * .p0t migration was not finished; this closes it. */
        /* `!clause <text>` — the same move one level up. `!assert` writes a ground
         * fact; a test that needs to prove something about RULES (that a taught
         * clause closes a gap, that a derived answer counts as knowledge and not
         * as a hole) could not state one, and had to fall back to a shell script
         * driving the MCP engine — exactly the gap the fact form closed at gen345.
         * The whole .p0 parser does the work, so a rule, a fact, a negative or a
         * quoted string all arrive by the same door the KB files use. */
        if (strncmp(p, "!clause", 7) == 0 && (p[7] == ' ' || p[7] == '\t')) {
            te_flush(t);
            char *q = p + 7;
            while (*q == ' ' || *q == '\t') q++;
            if (!*q || !kb_load_clause(brain_kb(t->b), q)) syntax_err = 1;
            continue;
        }
        if (strncmp(p, "!assert", 7) == 0 && (p[7] == ' ' || p[7] == '\t')) {
            te_flush(t);
            char *q = p + 7;
            while (*q == ' ' || *q == '\t') q++;
            char pred[TE_NAME]; size_t k = 0;
            while (*q && *q != '(' && *q != ' ' && *q != '\t' && k + 1 < sizeof pred)
                pred[k++] = *q++;
            pred[k] = '\0';
            while (*q == ' ' || *q == '\t') q++;
            if (k == 0 || *q != '(') { syntax_err = 1; continue; }
            q++;
            char argbuf[KB_MAX_ARGS][KB_TERM_LEN];
            const char *args[KB_MAX_ARGS];
            size_t argc = 0;
            argc = te_split_args(q, argbuf, args, &q);
            if (argc == 0) { syntax_err = 1; continue; }
            kb_assert(brain_kb(t->b), pred, args, argc);
            continue;
        }
        /* `!query PRED(a, …)` / `!query! PRED(a, …)` — un'ASSERZIONE su un FATTO
         * (gen404). Fino a qui un cricchetto poteva asserire solo su una FRASE:
         * poteva scrivere conoscenza (`!assert`, `!clause`) e toglierla
         * (`!forget`), ma per sapere se un fatto c'era doveva sperare che
         * qualche superficie lo raccontasse.
         *
         * E' una lacuna di MISURA, e si e' vista appena e' servita: il sensore
         * delle lacune di macchineria (`machinery_gap/1`) scrive un fatto che
         * nessuna frase legge, quindi era invisibile a tutta la suite. Un anello
         * che produce fatti e non risposte non e' verificabile con asserzioni
         * sulle risposte — e questo vale per l'autocorrezione in generale, non
         * solo per questo predicato.
         *
         * `!query` passa se il fatto e' dimostrabile, `!query!` se non lo e'. Un
         * argomento `$Var` e' una variabile: `!query machinery_gap($X)` chiede
         * «ne esiste almeno uno». */
        if (strncmp(p, "!query", 6) == 0 &&
            (p[6] == ' ' || p[6] == '\t' || (p[6] == '!' && (p[7] == ' ' || p[7] == '\t')))) {
            te_flush(t);
            int want = (p[6] != '!');
            char *q = p + (want ? 6 : 7);
            while (*q == ' ' || *q == '\t') q++;
            char pred[TE_NAME]; size_t k = 0;
            while (*q && *q != '(' && *q != ' ' && *q != '\t' && k + 1 < sizeof pred)
                pred[k++] = *q++;
            pred[k] = '\0';
            while (*q == ' ' || *q == '\t') q++;
            if (k == 0 || *q != '(') { syntax_err = 1; continue; }
            q++;
            char argbuf[KB_MAX_ARGS][KB_TERM_LEN];
            const char *args[KB_MAX_ARGS];
            size_t argc = te_split_args(q, argbuf, args, &q);
            if (argc == 0) { syntax_err = 1; continue; }
            /* Ogni `$Var` e' una posizione libera. La prima versione ne
             * liberava una sola — l'ultima incontrata — e `dead_rule($H, $M)`
             * finiva per cercare un fatto il cui secondo argomento fosse la
             * stringa «$M». Falliva sempre, e sembrava che il predicato non
             * esistesse: un errore dello strumento di misura travestito da
             * risultato, che e' il tipo peggiore. */
            int free_slot = -1;
            for (size_t i = 0; i < argc; i++)
                if (args[i] && args[i][0] == '$') {
                    if (free_slot < 0) free_slot = (int)i;
                    args[i] = NULL;
                }
            int found;
            if (free_slot >= 0) {
                char hit[1][KB_TERM_LEN];
                found = kb_match(brain_kb(t->b), pred, args, argc, hit, 1) > 0;
            } else {
                found = kb_query(brain_kb(t->b), pred, args, argc);
            }
            if (found == want) {
                t->passed++;
            } else {
                t->failed++;
                fprintf(t->out, "  FAIL  [%s] line %d\n",
                        t->section[0] ? t->section : "-", t->line_no);
                fprintf(t->out, "        %s: %s/%zu\n",
                        want ? "atteso dimostrabile" : "atteso NON dimostrabile",
                        pred, argc);
            }
            continue;
        }
        /* ── !mcp <strumento> <json-argomenti> ──────────────────────────────
         *
         * Il layer MCP si poteva provare solo da fuori, a colpi di JSON-RPC su
         * un processo separato: e' il motivo per cui una dozzina di suite vive
         * ancora in shell invece che qui. Questa direttiva percorre la STESSA
         * strada di `tools/call` sul brain del test-engine, quindi prova il MCP
         * vero e non una sua imitazione.
         *
         * Il risultato prende il posto della risposta del turno, cosi' le righe
         * `<` e `<~` che seguono lo verificano come verificherebbero una replica
         * qualunque — una primitiva sola, nessuna sintassi di confronto nuova.
         *
         *     !mcp kb.assert {"pred":"dog","args":["rex"]}
         *     !mcp kb.query  {"pred":"dog","args":["rex"]}
         *     <~ true
         */
        /* ── !sandbox [off] ─────────────────────────────────────────────────
         *
         * Alcuni test hanno bisogno di una directory SCRIVIBILE e propria: il
         * giudice del codice compila ed esegue un candidato, e scrive un albero
         * di lavoro dove si trova. Nello shell la ricetta era `mktemp -d` +
         * `cd`; qui non si poteva esprimere, ed e' il motivo per cui quella
         * famiglia di test e' rimasta fuori dal .p0t.
         *
         * `!sandbox` crea una directory privata 0700 e ci sposta il demone;
         * `!sandbox off` torna dove si era. Il ritorno e' automatico a fine
         * file e a ogni `[test …]`, cosi' un test che dimentica di chiudere non
         * lascia il demone in una directory temporanea — che sarebbe un guasto
         * silenzioso per tutti i test successivi. */
        /* ── !symlink BERSAGLIO NOME ────────────────────────────────────────
         *
         * Il caso interessante del contenimento e' il collegamento simbolico:
         * il suo NOME e' dentro il workspace e perfettamente innocente, e solo
         * risolverlo dice che punta fuori. Provarlo richiede di CREARLO, e in
         * shell era `ln -sf`; in .p0t non era esprimibile, ed e' il motivo per
         * cui le suite sugli strumenti erano rimaste fuori.
         *
         * Si rimuove da se' alla sezione dopo e a fine file: un link lasciato
         * nel workspace e' sporcizia che il test successivo si ritrova fra i
         * piedi. */
        /* ── !exec / !exec! COMANDO ─────────────────────────────────────────
         *
         * Il jolly: esegue un comando e asserisce il suo ESITO — `!exec` vuole
         * uscita zero, `!exec!` vuole che fallisca. L'output prende il posto
         * della risposta, quindi le righe `<~` / `<!` che seguono lo verificano
         * come qualunque replica.
         *
         * Serve per le promesse che non sono ne' una risposta ne' un file: «il
         * binario si costruisce», «questo comando rifiuta», «l'albero e' pulito
         * dopo». Le primitive puntuali qui sotto restano preferibili quando
         * bastano: dicono da sole che cosa stanno guardando, e un comando no. */
        if (strncmp(p, "!exec", 5) == 0 &&
            (p[5] == ' ' || p[5] == '\t' || (p[5] == '!' && (p[6] == ' ' || p[6] == '\t')))) {
            te_flush(t);
            int want_ok = (p[5] != '!');
            char *q = p + (want_ok ? 5 : 6);
            while (*q == ' ' || *q == '\t') q++;
            if (!*q) { syntax_err = 1; continue; }
            FILE *ph = popen(q, "r");
            if (!ph) {
                t->failed++;
                fprintf(t->out, "  FAIL  [%s] line %d\n",
                        t->section[0] ? t->section : "-", t->line_no);
                fprintf(t->out, "        exec: non sono riuscito ad avviare «%s»\n", q);
                continue;
            }
            size_t o = 0;
            while (o + 1 < sizeof t->reply) {
                size_t r = fread(t->reply + o, 1, sizeof t->reply - o - 1, ph);
                if (r == 0) break;
                o += r;
            }
            t->reply[o] = '\0';
            while (o > 0 && (t->reply[o - 1] == '\n' || t->reply[o - 1] == '\r'))
                t->reply[--o] = '\0';
            int rc = pclose(ph);
            int ok = (rc == 0);
            t->have_reply = 1;
            t->output_source = TE_OUTPUT_EXEC;
            if (ok == want_ok) t->passed++;
            else {
                t->failed++;
                fprintf(t->out, "  FAIL  [%s] line %d\n",
                        t->section[0] ? t->section : "-", t->line_no);
                fprintf(t->out, "        exec: «%s» %s (uscita %d)\n", q,
                        want_ok ? "doveva riuscire" : "doveva fallire", rc);
            }
            continue;
        }

        /* ── !direxists / !dirmissing PERCORSO ──────────────────────────────── */
        if ((strncmp(p, "!direxists", 10) == 0 && (p[10] == ' ' || p[10] == '\t')) ||
            (strncmp(p, "!dirmissing", 11) == 0 && (p[11] == ' ' || p[11] == '\t'))) {
            te_flush(t);
            int want = (p[4] == 'e');
            char *q = strchr(p, ' ');
            while (q && (*q == ' ' || *q == '\t')) q++;
            if (!q || !*q) { syntax_err = 1; continue; }
            struct stat st;
            int there = (stat(q, &st) == 0 && S_ISDIR(st.st_mode));
            if (there == want) t->passed++;
            else {
                t->failed++;
                fprintf(t->out, "  FAIL  [%s] line %d\n",
                        t->section[0] ? t->section : "-", t->line_no);
                fprintf(t->out, "        %s: %s\n",
                        want ? "attesa DIRECTORY esistente" : "attesa directory ASSENTE", q);
            }
            continue;
        }
        /* ── !fileexists / !filemissing PERCORSO ────────────────────────────
         *
         * Agnostiche: guardano il DISCO e basta, senza sapere che cosa il file
         * contenga o a quale sottosistema appartenga. Servono ovunque una
         * promessa sia «questo file c'e'» o «questo file non deve esserci» —
         * il save-map e' solo il primo che le chiede. */
        if ((strncmp(p, "!fileexists", 11) == 0 && (p[11] == ' ' || p[11] == '\t')) ||
            (strncmp(p, "!filemissing", 12) == 0 && (p[12] == ' ' || p[12] == '\t'))) {
            te_flush(t);
            int want = (p[5] == 'e');
            char *q = strchr(p, ' ');
            while (q && (*q == ' ' || *q == '\t')) q++;
            if (!q || !*q) { syntax_err = 1; continue; }
            struct stat st;
            int there = (stat(q, &st) == 0);
            if (there == want) t->passed++;
            else {
                t->failed++;
                fprintf(t->out, "  FAIL  [%s] line %d\n",
                        t->section[0] ? t->section : "-", t->line_no);
                fprintf(t->out, "        %s: %s\n",
                        want ? "atteso ESISTENTE" : "atteso ASSENTE", q);
            }
            continue;
        }
        /* ── !fileclean / !filehas / !filelacks PERCORSO testo ──────────────
         *
         * Alcune cose si possono verificare solo guardando DOVE sono finite: il
         * save-map promette che un fatto appreso vada accanto ai suoi simili, e
         * quella promessa e' un file, non una risposta.
         *
         * La regola che le governa: si lavora sulla KB VERA, mai su un albero
         * di comodo. Un test che si costruisce una KB finta misura una creatura
         * che non spediamo. Percio' `!fileclean` toglie le righe lasciate da un
         * giro precedente PRIMA di cominciare — cosi' il test e' ripetibile — e
         * si richiama alla fine per non lasciare il repository sporco. */
        if ((strncmp(p, "!fileclean", 10) == 0 && (p[10] == ' ' || p[10] == '\t')) ||
            (strncmp(p, "!filehas", 8) == 0   && (p[8]  == ' ' || p[8]  == '\t')) ||
            (strncmp(p, "!filelacks", 10) == 0 && (p[10] == ' ' || p[10] == '\t'))) {
            te_flush(t);
            int mode = p[5] == 'c' ? 0 : (p[5] == 'h' ? 1 : 2);   /* clean/has/lacks */
            char *q = strchr(p, ' ');
            while (q && (*q == ' ' || *q == '\t')) q++;
            char path[512]; size_t k = 0;
            while (q && *q && *q != ' ' && *q != '\t' && k + 1 < sizeof path) path[k++] = *q++;
            path[k] = '\0';
            while (q && (*q == ' ' || *q == '\t')) q++;
            if (!k || !q || !*q) { syntax_err = 1; continue; }
            const char *needle = q;
            FILE *f = fopen(path, "r");
            int found = 0;
            char keep[262144]; size_t ko = 0;
            if (f) {
                char lb[4096];
                while (fgets(lb, sizeof lb, f)) {
                    if (strstr(lb, needle)) {
                        found = 1;
                        if (mode == 0) continue;      /* clean: la riga sparisce */
                    }
                    if (mode == 0 && ko + strlen(lb) + 1 < sizeof keep) {
                        memcpy(keep + ko, lb, strlen(lb)); ko += strlen(lb);
                    }
                }
                fclose(f);
            }
            if (mode == 0) {
                if (f && found) {
                    FILE *o = fopen(path, "w");
                    if (o) { fwrite(keep, 1, ko, o); fclose(o); }
                }
                continue;                              /* clean non asserisce mai */
            }
            int want = (mode == 1);
            if (found == want) t->passed++;
            else {
                t->failed++;
                fprintf(t->out, "  FAIL  [%s] line %d\n",
                        t->section[0] ? t->section : "-", t->line_no);
                fprintf(t->out, "        %s: %s %s «%s»\n",
                        want ? "atteso nel file" : "atteso ASSENTE dal file",
                        path, want ? "non contiene" : "contiene", needle);
            }
            continue;
        }
        if (strncmp(p, "!symlink", 8) == 0 && (p[8] == ' ' || p[8] == '\t')) {
            te_flush(t);
            char *q = p + 8;
            while (*q == ' ' || *q == '\t') q++;
            char target[400]; size_t k = 0;
            while (*q && *q != ' ' && *q != '\t' && k + 1 < sizeof target) target[k++] = *q++;
            target[k] = '\0';
            while (*q == ' ' || *q == '\t') q++;
            if (!k || !*q || t->n_fixture >= 8) { syntax_err = 1; continue; }
            unlink(q);
            if (symlink(target, q) != 0) {
                t->failed++;
                fprintf(t->out, "  FAIL  [%s] line %d\n",
                        t->section[0] ? t->section : "-", t->line_no);
                fprintf(t->out, "        symlink: non sono riuscito a creare %s\n", q);
            } else {
                snprintf(t->fixture[t->n_fixture++], 512, "%s", q);
            }
            continue;
        }
        if (strncmp(p, "!sandbox", 8) == 0 &&
            (p[8] == '\0' || p[8] == ' ' || p[8] == '\t')) {
            te_flush(t);
            const char *arg = p + 8;
            while (*arg == ' ' || *arg == '\t') arg++;
            if (strncmp(arg, "off", 3) == 0) { te_sandbox_leave(t); continue; }
            if (!te_sandbox_enter(t)) {
                t->failed++;
                fprintf(t->out, "  FAIL  [%s] line %d\n",
                        t->section[0] ? t->section : "-", t->line_no);
                fprintf(t->out, "        sandbox: non sono riuscito a crearla\n");
            }
            continue;
        }
        if (strncmp(p, "!mcp", 4) == 0 && (p[4] == ' ' || p[4] == '\t')) {
            te_flush(t);
            te_apply_config(t);
            char *q = p + 4;
            while (*q == ' ' || *q == '\t') q++;
            char tool[TE_NAME]; size_t k = 0;
            while (*q && *q != ' ' && *q != '\t' && k + 1 < sizeof tool) tool[k++] = *q++;
            tool[k] = '\0';
            while (*q == ' ' || *q == '\t') q++;
            if (k == 0) { syntax_err = 1; continue; }
            mcp_tool_invoke(t->b, tool, *q ? q : NULL, t->reply, sizeof t->reply);
            size_t rn = strlen(t->reply);
            while (rn > 0 && (t->reply[rn - 1] == '\n' || t->reply[rn - 1] == '\r'))
                t->reply[--rn] = '\0';
            t->have_reply = 1;
            t->output_source = TE_OUTPUT_MCP;
            continue;
        }
        if (strncmp(p, "!forget", 7) == 0 && (p[7] == ' ' || p[7] == '\t')) {
            te_flush(t);
            char *q = p + 7;
            while (*q == ' ' || *q == '\t') q++;
            /* `!forget @LAYER` drops a whole provenance layer from the ONE KB —
             * the in-test way to narrow the view, instead of amputating the load
             * from outside with PARROT0_WORLD_FACTS=0 / an empty BASE. */
            if (*q == '@') {
                q++;
                int mask = 0;
                if      (!strncmp(q, "base", 4))         mask = KB_BASE;
                else if (!strncmp(q, "session", 7))      mask = KB_SESSION;
                else if (!strncmp(q, "induced", 7))      mask = KB_INDUCED;
                else if (!strncmp(q, "reflective", 10))  mask = KB_REFLECTIVE;
                else if (!strncmp(q, "hypothetical", 12)) mask = KB_HYPOTHETICAL;
                if (!mask) { syntax_err = 1; continue; }
                kb_retract_origin(brain_kb(t->b), mask);
                continue;
            }
            char pred[TE_NAME]; size_t k = 0;
            while (*q && *q != '(' && *q != ' ' && *q != '\t' && k + 1 < sizeof pred)
                pred[k++] = *q++;
            pred[k] = '\0';
            if (k == 0) { syntax_err = 1; continue; }
            while (*q == ' ' || *q == '\t') q++;
            if (*q != '(') {                       /* whole predicate */
                kb_retract_pred(brain_kb(t->b), pred);
                continue;   /* te_learned() sees the size change on its own */
            }
            q++;                                   /* one ground fact: pred(a, b) */
            char argbuf[KB_MAX_ARGS][KB_TERM_LEN];
            const char *args[KB_MAX_ARGS];
            size_t argc = 0;
            argc = te_split_args(q, argbuf, args, &q);
            if (argc == 0) { syntax_err = 1; continue; }
            kb_retract(brain_kb(t->b), pred, args, argc);
            continue;
        }
        if (strncmp(p, "!set", 4) == 0 && (p[4] == ' ' || p[4] == '\t')) {
            te_flush(t);
            char *q = p + 4;
            while (*q == ' ' || *q == '\t') q++;
            char name[TE_NAME]; size_t k = 0;      /* NAME up to '=' or space */
            while (*q && *q != '=' && *q != ' ' && *q != '\t' && k + 1 < sizeof name)
                name[k++] = *q++;
            name[k] = '\0';
            while (*q == ' ' || *q == '\t') q++;
            if (*q == '=') q++;                     /* the value is the rest, verbatim */
            if (k == 0) { syntax_err = 1; continue; }
            p0env_set(name, q);   /* the signature check in te_apply_config decides reload */
            continue;
        }

        syntax_err = 1;
    }
    te_flush(t);
    te_sandbox_leave(t);   /* mai lasciare il demone dentro una temporanea */
    te_fixtures_clear(t);
    return syntax_err ? 2 : 0;
}

static void te_summary(TeState *t) {
    fprintf(t->out, "\n=== test-engine: %d passed, %d failed (%d total) ===\n",
            t->passed, t->failed, t->passed + t->failed);
}

/* ── batch fallback (no socket) ────────────────────────────────────────────── */

int test_engine_run(Brain *b, FILE *in) {
    TeState t;
    memset(&t, 0, sizeof t);
    t.b = b;
    t.out = stdout;
    te_mark_clean(&t);   /* baseline: config + kb size the brain booted with */
    t.timeout_sec = TE_DEFAULT_TIMEOUT;
    int rc = te_process_stream(&t, in);
    te_summary(&t);
    if (rc == 2) return 2;
    return t.failed > 0 ? 1 : 0;
}

/* ── daemon ────────────────────────────────────────────────────────────────── */

static ssize_t write_all(int fd, const char *buf, size_t len) {
    size_t off = 0;
    while (off < len) {
        ssize_t k = write(fd, buf + off, len - off);
        if (k < 0) { if (errno == EINTR) continue; return -1; }
        off += (size_t)k;
    }
    return (ssize_t)off;
}

int test_engine_serve(Brain *b, const char *sockpath) {
    int lfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (lfd < 0) { perror("test-engine: socket"); return 1; }
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof addr);
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof addr.sun_path, "%s", sockpath);
    unlink(sockpath);
    if (bind(lfd, (struct sockaddr *)&addr, sizeof addr) < 0) {
        perror("test-engine: bind"); close(lfd); return 1;
    }
    if (listen(lfd, 8) < 0) {
        perror("test-engine: listen"); close(lfd); unlink(sockpath); return 1;
    }

    TeState t;
    memset(&t, 0, sizeof t);
    t.b = b;
    te_mark_clean(&t);   /* baseline: config + kb size the brain booted with */
    t.timeout_sec = TE_DEFAULT_TIMEOUT;

    for (;;) {
        int cfd = accept(lfd, NULL, NULL);
        if (cfd < 0) { if (errno == EINTR) continue; break; }
        /* each file starts from the default environment: a hermetic file's
         * overrides never bleed into the next. te_apply_config still reloads only
         * if the resulting signature actually differs from what's loaded, so two
         * files that need the SAME context in a row cost a single reload. */
        p0env_clear();

        /* slurp the whole payload (client half-closes its write side at EOF) */
        char *inbuf = NULL; size_t incap = 0, inlen = 0; char rd[4096]; ssize_t k;
        int oom = 0;
        while ((k = read(cfd, rd, sizeof rd)) > 0) {
            if (inlen + (size_t)k + 1 > incap) {
                size_t nc = incap ? incap * 2 : 8192;
                while (nc < inlen + (size_t)k + 1) nc *= 2;
                char *g = realloc(inbuf, nc);
                if (!g) { oom = 1; break; }
                inbuf = g; incap = nc;
            }
            memcpy(inbuf + inlen, rd, (size_t)k); inlen += (size_t)k;
        }
        if (oom) { free(inbuf); close(cfd); continue; }
        if (inbuf) inbuf[inlen] = '\0';

        FILE *fin = fmemopen(inbuf ? inbuf : (char *)"", inlen, "r");
        char *ob = NULL; size_t ol = 0;
        FILE *fout = open_memstream(&ob, &ol);
        if (fin && fout) {
            t.out = fout;
            t.line_no = 0;         /* line numbers refer to the file just sent */
            int passed_before = t.passed, failed_before = t.failed;
            te_process_stream(&t, fin);
            /* The client turns COUNT into the one-line file report and EXIT into
             * its exit code. On a normal send the counts are THIS file's (fail-fast:
             * a failing file exits 1 and stops make); on --test-report they are the
             * whole-session grand totals. Failure detail was already written above. */
            if (t.shutdown)
                fprintf(fout, "COUNT %d %d\nEXIT %d\n", t.passed, t.failed,
                        t.failed > 0 ? 1 : 0);
            else {
                int fp = t.passed - passed_before, ff = t.failed - failed_before;
                fprintf(fout, "COUNT %d %d\nEXIT %d\n", fp, ff, ff > 0 ? 1 : 0);
            }
        }
        if (fout) fclose(fout);
        if (fin) fclose(fin);
        if (ob) write_all(cfd, ob, ol);
        free(ob);
        free(inbuf);
        close(cfd);
        if (t.shutdown) break;
    }
    close(lfd);
    unlink(sockpath);
    return t.failed > 0 ? 1 : 0;
}

/* ── client (featherweight: no brain, no KB — just a socket relay) ─────────── */

/* basename of a path, for a tidy one-line report ("facts.p0t", not the full path) */
static const char *te_base(const char *path) {
    if (!path) return NULL;
    const char *b = strrchr(path, '/');
    return b ? b + 1 : path;
}

int test_engine_send(const char *sockpath, FILE *in, const char *label) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) { perror("test-send: socket"); return 2; }
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof addr);
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof addr.sun_path, "%s", sockpath);

    /* retry briefly so `make test` need not sleep after starting the daemon */
    int connected = 0;
    for (int i = 0; i < 300; i++) {
        if (connect(fd, (struct sockaddr *)&addr, sizeof addr) == 0) { connected = 1; break; }
        struct timespec ts = { 0, 10 * 1000 * 1000 };  /* 10 ms */
        nanosleep(&ts, NULL);
    }
    if (!connected) {
        fprintf(stderr, "test-send: cannot reach engine at %s\n", sockpath);
        close(fd); return 2;
    }

    char buf[4096]; size_t r;
    while ((r = fread(buf, 1, sizeof buf, in)) > 0)
        if (write_all(fd, buf, r) < 0) { close(fd); return 2; }
    shutdown(fd, SHUT_WR);

    /* read the reply into memory so a trailing "EXIT n" line can set our code */
    char *rep = NULL; size_t cap = 0, len = 0; ssize_t k;
    while ((k = read(fd, buf, sizeof buf)) > 0) {
        if (len + (size_t)k + 1 > cap) {
            size_t nc = cap ? cap * 2 : 8192;
            while (nc < len + (size_t)k + 1) nc *= 2;
            char *g = realloc(rep, nc);
            if (!g) break;
            rep = g; cap = nc;
        }
        memcpy(rep + len, buf, (size_t)k); len += (size_t)k;
    }
    close(fd);
    if (rep) rep[len] = '\0';

    int code = 0, passed = 0, failed = 0;
    size_t body = len;   /* bytes of the reply that are failure detail (printed) */
    if (rep && len) {
        /* strip the trailing control lines, youngest first: EXIT then COUNT. */
        size_t e = len;
        while (e > 0 && (rep[e - 1] == '\n' || rep[e - 1] == '\r')) e--;
        size_t ls = e; while (ls > 0 && rep[ls - 1] != '\n') ls--;
        if (strncmp(rep + ls, "EXIT ", 5) == 0) {
            code = atoi(rep + ls + 5);
            e = ls; while (e > 0 && rep[e - 1] == '\n') e--;
            ls = e; while (ls > 0 && rep[ls - 1] != '\n') ls--;
        }
        if (strncmp(rep + ls, "COUNT ", 6) == 0) {
            sscanf(rep + ls + 6, "%d %d", &passed, &failed);
            body = ls;              /* everything before COUNT is failure detail */
        }
    }

    if (failed > 0 && body > 0) fwrite(rep, 1, body, stdout);   /* useful detail */
    if (label) {
        const char *name = te_base(label);
        if (failed > 0) printf("FAIL  %s — %d passed, %d failed\n", name, passed, failed);
        else            printf("ok    %s — %d passed\n", name, passed);
    } else {
        printf("total: %d passed, %d failed\n", passed, failed);
    }
    free(rep);
    return code;
}

int test_engine_send_str(const char *sockpath, const char *payload, const char *label) {
    FILE *m = fmemopen((void *)payload, strlen(payload), "r");
    if (!m) return 2;
    int rc = test_engine_send(sockpath, m, label);
    fclose(m);
    return rc;
}

/* ── benchmark engine ────────────────────────────────────────────────────────
 *
 * The benchmark is deliberately a sibling of the test daemon, not a second
 * prompt runner. It uses the same TeState parser and assertions above. The
 * only extra protocol line is `!bench-slot PATH`, which lets the daemon attach
 * the existing per-turn counts to a resumable slot in its TSV ledger. */

#define BE_MAX_ROWS 4096
#define BE_FIELD 512

typedef struct {
    char slot[BE_FIELD];
    char category[BE_FIELD];
    char status[16];
    int passed, failed, total;
    long long started_epoch;
    long elapsed_ms;
} BenchRow;

typedef struct {
    char slot[BE_FIELD];
    char category[BE_FIELD];
    int passed, failed, total;
} BenchCategory;

typedef struct {
    const char *path;
    BenchRow rows[BE_MAX_ROWS];
    size_t count;
    BenchCategory categories[BE_MAX_ROWS];
    size_t category_count;
} BenchLedger;

typedef struct {
    BenchLedger *ledger;
    const char *slot;
} BenchRunContext;

static void be_category(const char *slot, char *out, size_t cap) {
    snprintf(out, cap, "%s", slot ? slot : "unknown");
    const char *base = strrchr(out, '/');
    base = base ? base + 1 : out;
    if (strncmp(base, "slot-", 5) == 0 && strlen(base) == 8) {
        snprintf(out, cap, "%s", base);
        return;
    }
    char *p = strrchr(out, '/');
    if (p) *p = '\0';
    char *corpus = strstr(out, "corpus/");
    if (corpus) memmove(out, corpus + 7, strlen(corpus + 7) + 1);
}

static BenchRow *be_find(BenchLedger *l, const char *slot, int create) {
    for (size_t i = 0; i < l->count; i++)
        if (strcmp(l->rows[i].slot, slot) == 0) return &l->rows[i];
    if (!create || l->count >= BE_MAX_ROWS) return NULL;
    BenchRow *r = &l->rows[l->count++];
    memset(r, 0, sizeof *r);
    snprintf(r->slot, sizeof r->slot, "%s", slot);
    be_category(slot, r->category, sizeof r->category);
    snprintf(r->status, sizeof r->status, "pending");
    return r;
}

static BenchCategory *be_find_category(BenchLedger *l, const char *slot,
                                        const char *category, int create) {
    for (size_t i = 0; i < l->category_count; i++)
        if (strcmp(l->categories[i].slot, slot) == 0 &&
            strcmp(l->categories[i].category, category) == 0)
            return &l->categories[i];
    if (!create || l->category_count >= BE_MAX_ROWS) return NULL;
    BenchCategory *c = &l->categories[l->category_count++];
    memset(c, 0, sizeof *c);
    snprintf(c->slot, sizeof c->slot, "%s", slot);
    snprintf(c->category, sizeof c->category, "%s", category);
    return c;
}

static void be_remove_slot_categories(BenchLedger *l, const char *slot) {
    size_t write = 0;
    for (size_t i = 0; i < l->category_count; i++) {
        if (strcmp(l->categories[i].slot, slot) == 0) continue;
        if (write != i) l->categories[write] = l->categories[i];
        write++;
    }
    l->category_count = write;
}

static void be_record_assertion(void *opaque, const char *category, int passed) {
    BenchRunContext *ctx = opaque;
    const char *name = category && *category ? category : "uncategorized";
    BenchCategory *c = be_find_category(ctx->ledger, ctx->slot, name, 1);
    if (!c) return;
    if (passed) c->passed++;
    else c->failed++;
    c->total++;
}

static void be_load(BenchLedger *l) {
    FILE *f = fopen(l->path, "r");
    if (!f) return;
    char line[2048];
    while (fgets(line, sizeof line, f)) {
        if (line[0] == '#' || strncmp(line, "slot\t", 5) == 0) continue;
        char *v[9]; size_t n = 0;
        char *p = strtok(line, "\t\r\n");
        while (p && n < 9) { v[n++] = p; p = strtok(NULL, "\t\r\n"); }
        if (n < 7) continue;
        BenchRow *r = be_find(l, v[0], 1);
        if (!r) continue;
        snprintf(r->category, sizeof r->category, "%s", v[1]);
        snprintf(r->status, sizeof r->status, "%s", v[2]);
        r->passed = atoi(v[3]); r->failed = atoi(v[4]); r->total = atoi(v[5]);
        if (n >= 9) {
            r->started_epoch = atoll(v[7]);
            r->elapsed_ms = atol(v[8]);
        }
    }
    fclose(f);
    char categories_path[BE_FIELD * 2];
    snprintf(categories_path, sizeof categories_path, "%s", l->path);
    char *slash = strrchr(categories_path, '/');
    if (slash) snprintf(slash + 1, sizeof categories_path - (size_t)(slash + 1 - categories_path), "categories.tsv");
    FILE *cf = fopen(categories_path, "r");
    if (!cf) return;
    while (fgets(line, sizeof line, cf)) {
        if (line[0] == '#' || strncmp(line, "slot\t", 5) == 0) continue;
        char *v[5]; size_t n = 0;
        char *p = strtok(line, "\t\r\n");
        while (p && n < 5) { v[n++] = p; p = strtok(NULL, "\t\r\n"); }
        if (n < 5) continue;
        BenchCategory *c = be_find_category(l, v[0], v[1], 1);
        if (!c) continue;
        c->passed = atoi(v[2]); c->failed = atoi(v[3]); c->total = atoi(v[4]);
    }
    fclose(cf);
}

static void be_write(BenchLedger *l) {
    char tmp[BE_FIELD * 2];
    snprintf(tmp, sizeof tmp, "%s.tmp", l->path);
    FILE *f = fopen(tmp, "w");
    if (!f) return;
    fprintf(f, "# Bench-engine progress registry; manual-only, never a TDD or regression gate.\n");
    fprintf(f, "slot\tcategory\tstatus\tpassed\tfailed\ttotal\tpercent\tstarted_epoch\telapsed_ms\n");
    for (size_t i = 0; i < l->count; i++) {
        BenchRow *r = &l->rows[i];
        double pct = r->total ? 100.0 * r->passed / r->total : 0.0;
        fprintf(f, "%s\t%s\t%s\t%d\t%d\t%d\t%.2f\t%lld\t%ld\n",
                r->slot, r->category, r->status, r->passed, r->failed, r->total, pct,
                r->started_epoch, r->elapsed_ms);
    }
    fclose(f);
    rename(tmp, l->path);

    char categories_path[BE_FIELD * 2];
    snprintf(categories_path, sizeof categories_path, "%s", l->path);
    char *categories_slash = strrchr(categories_path, '/');
    if (categories_slash)
        snprintf(categories_slash + 1,
                 sizeof categories_path - (size_t)(categories_slash + 1 - categories_path),
                 "categories.tsv");
    FILE *cf = fopen(categories_path, "w");
    if (cf) {
        fprintf(cf, "# Bench-engine category measurements grouped by slot.\n");
        fprintf(cf, "slot\tcategory\tpassed\tfailed\ttotal\n");
        for (size_t i = 0; i < l->category_count; i++) {
            BenchCategory *c = &l->categories[i];
            fprintf(cf, "%s\t%s\t%d\t%d\t%d\n", c->slot, c->category,
                    c->passed, c->failed, c->total);
        }
        fclose(cf);
    }

    char hist[BE_FIELD * 2];
    snprintf(hist, sizeof hist, "%s", l->path);
    char *slash = strrchr(hist, '/');
    if (slash) snprintf(slash + 1, sizeof hist - (size_t)(slash + 1 - hist), "histogram.tsv");
    FILE *h = fopen(hist, "w");
    if (!h) return;
    fprintf(h, "# Bench-engine skill histogram; partial while a run is in progress.\n");
    fprintf(h, "category\tpassed\tfailed\ttotal\tpercent\tavg_elapsed_ms\n");
    for (size_t i = 0; i < l->category_count; i++) {
        BenchCategory *c = &l->categories[i];
        int seen = 0, p = 0, ff = 0, total = 0, slot_count = 0;
        long category_elapsed = 0;
        for (size_t j = 0; j < i; j++)
            if (strcmp(l->categories[j].category, c->category) == 0) { seen = 1; break; }
        if (seen) continue;
        for (size_t j = i; j < l->category_count; j++)
            if (strcmp(l->categories[j].category, c->category) == 0) {
                BenchRow *slot = be_find(l, l->categories[j].slot, 0);
                if (!slot || strcmp(slot->status, "complete") != 0) continue;
                p += l->categories[j].passed; ff += l->categories[j].failed;
                total += l->categories[j].total;
                category_elapsed += slot->elapsed_ms; slot_count++;
            }
        fprintf(h, "%s\t%d\t%d\t%d\t%.2f\t%ld\n", c->category, p, ff, total,
                total ? 100.0 * p / total : 0.0,
                slot_count ? category_elapsed / slot_count : 0L);
    }
    fclose(h);
}

static void be_totals(BenchLedger *l, int *passed, int *failed, long *elapsed) {
    *passed = 0;
    *failed = 0;
    *elapsed = 0;
    for (size_t i = 0; i < l->count; i++) {
        if (strcmp(l->rows[i].status, "complete") != 0) continue;
        *passed += l->rows[i].passed;
        *failed += l->rows[i].failed;
        *elapsed += l->rows[i].elapsed_ms;
    }
}

static int be_read_payload(int fd, char **out, size_t *outlen) {
    char *buf = NULL; size_t cap = 0, len = 0; char rd[4096]; ssize_t k;
    while ((k = read(fd, rd, sizeof rd)) > 0) {
        if (len + (size_t)k + 1 > cap) {
            size_t nc = cap ? cap * 2 : 8192;
            while (nc < len + (size_t)k + 1) nc *= 2;
            char *g = realloc(buf, nc);
            if (!g) { free(buf); return 0; }
            buf = g; cap = nc;
        }
        memcpy(buf + len, rd, (size_t)k); len += (size_t)k;
    }
    if (!buf) buf = calloc(1, 1);
    if (!buf) return 0;
    buf[len] = '\0'; *out = buf; *outlen = len; return 1;
}

static int be_complete(BenchLedger *l, const char *slot) {
    BenchRow *r = be_find(l, slot, 0);
    return r && strcmp(r->status, "complete") == 0;
}

int bench_engine_serve(Brain *b, const char *sockpath, const char *stats_path) {
    int lfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (lfd < 0) { perror("bench-engine: socket"); return 1; }
    struct sockaddr_un addr; memset(&addr, 0, sizeof addr); addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof addr.sun_path, "%s", sockpath); unlink(sockpath);
    if (bind(lfd, (struct sockaddr *)&addr, sizeof addr) < 0 || listen(lfd, 8) < 0) {
        perror("bench-engine: bind/listen"); close(lfd); unlink(sockpath); return 1;
    }
    BenchLedger *ledger = calloc(1, sizeof *ledger);
    if (!ledger) { close(lfd); unlink(sockpath); return 1; }
    ledger->path = stats_path;
    be_load(ledger); be_write(ledger);
    TeState t; memset(&t, 0, sizeof t); t.b = b; te_mark_clean(&t); t.timeout_sec = TE_DEFAULT_TIMEOUT;
    for (;;) {
        int cfd = accept(lfd, NULL, NULL);
        if (cfd < 0) { if (errno == EINTR) continue; break; }
        char *payload = NULL; size_t plen = 0;
        if (!be_read_payload(cfd, &payload, &plen)) { close(cfd); continue; }
        FILE *fin = fmemopen(payload, plen, "r");
        char control[BE_FIELD] = "";
        if (fin && !fgets(control, sizeof control, fin)) control[0] = '\0';
        size_t cn = strlen(control); while (cn && (control[cn-1] == '\n' || control[cn-1] == '\r')) control[--cn] = '\0';
        char *ob = NULL; size_t ol = 0; FILE *fout = open_memstream(&ob, &ol);
        if (!fin || !fout) { free(payload); if (fin) fclose(fin); if (fout) fclose(fout); close(cfd); continue; }
        if (strcmp(control, "!bench-shutdown") == 0) {
            int p = 0, ff = 0; long elapsed = 0;
            for (size_t i = 0; i < ledger->count; i++) if (strcmp(ledger->rows[i].status, "complete") == 0) {
                p += ledger->rows[i].passed; ff += ledger->rows[i].failed; elapsed += ledger->rows[i].elapsed_ms;
                fprintf(fout, "REPORT %s status=%s passed=%d failed=%d elapsed_ms=%ld\n",
                        ledger->rows[i].slot, ledger->rows[i].status, ledger->rows[i].passed,
                        ledger->rows[i].failed, ledger->rows[i].elapsed_ms);
            }
            fprintf(fout, "BENCH %d %d %d percent=%.2f elapsed_ms=%ld\n", p, ff, p + ff,
                    p + ff ? 100.0 * p / (p + ff) : 0.0, elapsed);
            t.shutdown = 1;
        } else if (strcmp(control, "!bench-health") == 0) {
            p0env_clear(); t.out = fout; t.line_no = 0; t.shutdown = 0;
            t.bench_record = NULL; t.bench_record_ctx = NULL;
            int pb = t.passed, fb = t.failed;
            te_process_stream(&t, fin);
            int passed = t.passed - pb, failed = t.failed - fb;
            fprintf(fout, "HEALTH %d %d %d\nEXIT %d\n", passed, failed,
                    passed + failed, failed > 0 ? 1 : 0);
        } else if (strncmp(control, "!bench-slot ", 12) != 0) {
            fprintf(fout, "BENCH ERROR missing !bench-slot\nEXIT 2\n");
        } else {
            const char *slot = control + 12;
            BenchRow *row = be_find(ledger, slot, 1);
            if (be_complete(ledger, slot)) {
                BenchRow *done = be_find(ledger, slot, 0);
                int p, ff; long elapsed;
                be_totals(ledger, &p, &ff, &elapsed);
                (void)elapsed;
                fprintf(fout, "SLOT skipped %s elapsed_ms=%ld\nCOUNT 0 0\n"
                        "BENCH partial passed=%d failed=%d total=%d percent=%.2f delta_pct=0.00\nEXIT 0\n",
                        slot, done ? done->elapsed_ms : 0L, p, ff, p + ff,
                        p + ff ? 100.0 * p / (p + ff) : 0.0);
            } else if (!row) {
                fprintf(fout, "BENCH ERROR too many slots\nEXIT 2\n");
            } else {
                BenchRunContext runctx = { ledger, slot };
                snprintf(row->status, sizeof row->status, "running");
                row->passed = row->failed = row->total = 0;
                row->started_epoch = (long long)time(NULL);
                row->elapsed_ms = 0;
                be_remove_slot_categories(ledger, slot);
                be_write(ledger);
                p0env_clear(); t.out = fout; t.line_no = 0; t.shutdown = 0;
                t.bench_record = be_record_assertion;
                t.bench_record_ctx = &runctx;
                t.bench_category[0] = '\0';
                int pb = t.passed, fb = t.failed;
                struct timespec started, finished;
                clock_gettime(CLOCK_MONOTONIC, &started);
                te_process_stream(&t, fin);
                clock_gettime(CLOCK_MONOTONIC, &finished);
                t.bench_record = NULL; t.bench_record_ctx = NULL;
                row->passed = t.passed - pb; row->failed = t.failed - fb; row->total = row->passed + row->failed;
                row->elapsed_ms = (long)((finished.tv_sec - started.tv_sec) * 1000L +
                    (finished.tv_nsec - started.tv_nsec) / 1000000L);
                snprintf(row->status, sizeof row->status, "complete"); be_write(ledger);
                int p, ff; long elapsed;
                be_totals(ledger, &p, &ff, &elapsed);
                (void)elapsed;
                int before_total = p + ff - row->total;
                double partial = p + ff ? 100.0 * p / (p + ff) : 0.0;
                double before = before_total ?
                    100.0 * (p - row->passed) / before_total : 0.0;
                double slot_pct = row->total ? 100.0 * row->passed / row->total : 0.0;
                fprintf(fout, "SLOT complete %s elapsed_ms=%ld\nCOUNT %d %d\n"
                        "BENCH partial passed=%d failed=%d total=%d percent=%.2f "
                        "delta_pct=%.2f slot_pct=%.2f\nEXIT 0\n", slot, row->elapsed_ms,
                        row->passed, row->failed, p, ff, p + ff, partial, partial - before,
                        slot_pct);
            }
        }
        fclose(fout); fclose(fin); if (ob) write_all(cfd, ob, ol); free(ob); free(payload); close(cfd);
        if (t.shutdown) break;
    }
    close(lfd); unlink(sockpath); free(ledger); return 0;
}

static int be_client_send_payload(const char *sockpath, const char *payload, size_t plen,
                                  const char *label, const char *kind) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0); if (fd < 0) return 2;
    struct sockaddr_un addr; memset(&addr, 0, sizeof addr); addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof addr.sun_path, "%s", sockpath);
    int connected = 0;
    for (int i = 0; i < 300; i++) { if (connect(fd, (struct sockaddr *)&addr, sizeof addr) == 0) { connected = 1; break; } struct timespec ts = {0, 10 * 1000 * 1000}; nanosleep(&ts, NULL); }
    if (!connected) { close(fd); fprintf(stderr, "bench: cannot reach engine at %s\n", sockpath); return 2; }
    char head[BE_FIELD + 32];
    int is_shutdown = strcmp(kind, "shutdown") == 0;
    int health = strcmp(kind, "health") == 0;
    int hn = is_shutdown ? 0 : health ? snprintf(head, sizeof head, "!bench-health\n") :
                                  snprintf(head, sizeof head, "!bench-slot %s\n", label ? label : "stdin");
    if (hn) write_all(fd, head, (size_t)hn);
    write_all(fd, payload, plen); shutdown(fd, SHUT_WR);
    char buf[4096], *rep = NULL; size_t cap = 0, len = 0; ssize_t k;
    while ((k = read(fd, buf, sizeof buf)) > 0) { if (len + (size_t)k + 1 > cap) { size_t nc = cap ? cap * 2 : 8192; char *g = realloc(rep, nc); if (!g) break; rep = g; cap = nc; } memcpy(rep + len, buf, (size_t)k); len += (size_t)k; }
    close(fd);
    int exit_code = 0;
    if (rep) {
        rep[len] = '\0';
        char *line = rep;
        while (line && *line) {
            char *next = strchr(line, '\n');
            if (next) *next = '\0';
            if (strncmp(line, "SLOT ", 5) == 0 ||
                strncmp(line, "COUNT ", 6) == 0 ||
                strncmp(line, "BENCH ", 6) == 0 ||
                strncmp(line, "HEALTH ", 7) == 0 ||
                strncmp(line, "REPORT ", 7) == 0)
                puts(line);
            if (strncmp(line, "EXIT ", 5) == 0) exit_code = atoi(line + 5);
            if (!next) break;
            line = next + 1;
        }
    }
    free(rep);
    return health ? exit_code : 0;
}

int bench_engine_send(const char *sockpath, FILE *in, const char *label) {
    if (fseek(in, 0, SEEK_END) != 0) return 2;
    long n = ftell(in);
    if (n < 0 || fseek(in, 0, SEEK_SET) != 0) return 2;
    char *buf = malloc((size_t)n + 1); if (!buf) return 2; size_t got = fread(buf, 1, (size_t)n, in); int rc = be_client_send_payload(sockpath, buf, got, label, "slot"); free(buf); return rc;
}

int bench_engine_send_str(const char *sockpath, const char *payload, const char *label) {
    return be_client_send_payload(sockpath, payload, strlen(payload), label,
                                  "shutdown");
}

int bench_engine_health(const char *sockpath, FILE *in, const char *label) {
    if (fseek(in, 0, SEEK_END) != 0) return 2;
    long n = ftell(in);
    if (n < 0 || fseek(in, 0, SEEK_SET) != 0) return 2;
    char *buf = malloc((size_t)n + 1);
    if (!buf) return 2;
    size_t got = fread(buf, 1, (size_t)n, in);
    int rc = be_client_send_payload(sockpath, buf, got, label, "health");
    free(buf);
    return rc;
}

static int be_is_p0t(const char *path) {
    size_t n = strlen(path);
    return n >= 4 && strcmp(path + n - 4, ".p0t") == 0;
}

static int be_is_slot_dir(const char *path) {
    const char *base = strrchr(path, '/');
    base = base ? base + 1 : path;
    if (strncmp(base, "slot-", 5) != 0 || strlen(base) != 8) return 0;
    for (size_t i = 5; i < 8; i++) if (!isdigit((unsigned char)base[i])) return 0;
    return 1;
}

typedef struct {
    char **paths;
    size_t count, capacity;
} BenchFiles;

static int be_collect_files(const char *path, BenchFiles *files) {
    struct stat st;
    if (stat(path, &st) != 0) return 2;
    if (S_ISREG(st.st_mode)) {
        if (!be_is_p0t(path)) return 0;
        if (files->count == files->capacity) {
            size_t nc = files->capacity ? files->capacity * 2 : 32;
            char **grown = realloc(files->paths, nc * sizeof *grown);
            if (!grown) return 2;
            files->paths = grown; files->capacity = nc;
        }
        files->paths[files->count] = strdup(path);
        if (!files->paths[files->count]) return 2;
        files->count++;
        return 0;
    }
    if (!S_ISDIR(st.st_mode)) return 0;

    struct dirent **entries = NULL;
    int count = scandir(path, &entries, NULL, alphasort);
    if (count < 0) return 2;
    int rc = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(entries[i]->d_name, ".") == 0 || strcmp(entries[i]->d_name, "..") == 0) {
            free(entries[i]);
            continue;
        }
        size_t n = strlen(path) + strlen(entries[i]->d_name) + 2;
        char *child = malloc(n);
        if (!child) { rc = 2; free(entries[i]); break; }
        snprintf(child, n, "%s/%s", path, entries[i]->d_name);
        if (be_collect_files(child, files) != 0) rc = 2;
        free(child);
        free(entries[i]);
        if (rc != 0) break;
    }
    free(entries);
    return rc;
}

static int be_append_file(char **data, size_t *len, size_t *capacity, const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return 2;
    char buf[4096]; size_t n;
    while ((n = fread(buf, 1, sizeof buf, f)) > 0) {
        if (*len + n + 2 > *capacity) {
            size_t nc = *capacity ? *capacity * 2 : 8192;
            while (nc < *len + n + 2) nc *= 2;
            char *grown = realloc(*data, nc);
            if (!grown) { fclose(f); return 2; }
            *data = grown; *capacity = nc;
        }
        memcpy(*data + *len, buf, n); *len += n;
    }
    fclose(f);
    if (*len == 0 || (*data)[*len - 1] != '\n') (*data)[(*len)++] = '\n';
    (*data)[*len] = '\0';
    return 0;
}

static int be_append_text(char **data, size_t *len, size_t *capacity, const char *text) {
    size_t n = strlen(text);
    if (*len + n + 1 > *capacity) {
        size_t nc = *capacity ? *capacity * 2 : 8192;
        while (nc < *len + n + 1) nc *= 2;
        char *grown = realloc(*data, nc);
        if (!grown) return 2;
        *data = grown; *capacity = nc;
    }
    memcpy(*data + *len, text, n);
    *len += n;
    (*data)[*len] = '\0';
    return 0;
}

static void be_file_category(const char *slot, const char *file,
                             char *out, size_t cap) {
    const char *relative = file;
    size_t slot_len = strlen(slot);
    if (strncmp(file, slot, slot_len) == 0 && file[slot_len] == '/')
        relative = file + slot_len + 1;
    snprintf(out, cap, "%s", relative);
    char *last = strrchr(out, '/');
    if (last) *last = '\0';
    else snprintf(out, cap, "uncategorized");
}

static int be_send_slot(const char *sockpath, const char *path) {
    printf("BENCH start %s\n", path);
    fflush(stdout);
    BenchFiles files = {0};
    if (be_collect_files(path, &files) != 0 || files.count == 0) {
        for (size_t i = 0; i < files.count; i++) free(files.paths[i]);
        free(files.paths);
        return 2;
    }
    char *data = NULL; size_t len = 0, capacity = 0;
    int rc = 0;
    for (size_t i = 0; i < files.count; i++) {
        char category[BE_FIELD];
        char marker[BE_FIELD + 24];
        be_file_category(path, files.paths[i], category, sizeof category);
        snprintf(marker, sizeof marker, "!bench-category %s\n", category);
        if (be_append_text(&data, &len, &capacity, marker) != 0) rc = 2;
        if (be_append_file(&data, &len, &capacity, files.paths[i]) != 0) rc = 2;
        free(files.paths[i]);
    }
    free(files.paths);
    if (rc == 0) rc = be_client_send_payload(sockpath, data, len, path, "slot");
    free(data);
    return rc;
}

static int be_send_path_entry(const char *sockpath, const char *path, int *sent) {
    struct stat st;
    if (stat(path, &st) != 0) return 2;
    if (S_ISREG(st.st_mode)) {
        if (!be_is_p0t(path)) return 0;
        FILE *f = fopen(path, "rb");
        if (!f) return 2;
        int rc = bench_engine_send(sockpath, f, path);
        fclose(f);
        if (rc == 0) (*sent)++;
        return rc;
    }
    if (!S_ISDIR(st.st_mode)) return 0;
    if (!be_is_slot_dir(path)) return 2;
    int rc = be_send_slot(sockpath, path);
    if (rc == 0) (*sent)++;
    return rc;
}

static int be_send_root(const char *sockpath, const char *path, int *sent) {
    struct dirent **entries = NULL;
    int count = scandir(path, &entries, NULL, alphasort);
    if (count < 0) return 2;
    int found = 0, rc = 0, slot_total = 0;
    for (int i = 0; i < count; i++) {
        const char *name = entries[i]->d_name;
        if (name[0] == '.' || strcmp(name, "results") == 0) continue;
        size_t n = strlen(path) + strlen(name) + 2;
        char *child = malloc(n);
        if (!child) continue;
        snprintf(child, n, "%s/%s", path, name);
        struct stat st;
        if (stat(child, &st) == 0 && S_ISDIR(st.st_mode) && be_is_slot_dir(child)) slot_total++;
        free(child);
    }
    for (int i = 0; i < count; i++) {
        const char *name = entries[i]->d_name;
        if (name[0] == '.' || strcmp(name, "results") == 0) { free(entries[i]); continue; }
        size_t n = strlen(path) + strlen(name) + 2;
        char *child = malloc(n);
        if (!child) { rc = 2; free(entries[i]); break; }
        snprintf(child, n, "%s/%s", path, name);
        struct stat st;
        if (stat(child, &st) == 0 && S_ISDIR(st.st_mode)) {
            if (!be_is_slot_dir(child)) {
                fprintf(stderr, "bench: invalid root entry '%s' (expected slot-XXX)\n", child);
                rc = 2;
            } else {
                found = 1;
                if (be_send_slot(sockpath, child) != 0) rc = 2;
                else {
                    (*sent)++;
                    printf("BENCH progress %d/%d done %s\n", *sent, slot_total, child);
                    fflush(stdout);
                }
            }
        }
        free(child); free(entries[i]);
        if (rc != 0) break;
    }
    free(entries);
    if (!found && rc == 0) {
        fprintf(stderr, "bench: no test slots found under '%s' (expected slot-XXX directories)\n", path);
        return 2;
    }
    return rc;
}

int bench_engine_send_path(const char *sockpath, const char *path) {
    int sent = 0, rc = 0;
    int has_glob = strpbrk(path, "*?[") != NULL;
    if (has_glob) {
        glob_t matches;
        int grc = glob(path, 0, NULL, &matches);
        if (grc != 0) { globfree(&matches); return 2; }
        for (size_t i = 0; i < matches.gl_pathc; i++) {
            struct stat st;
            if (stat(matches.gl_pathv[i], &st) == 0 && S_ISDIR(st.st_mode) &&
                be_is_slot_dir(matches.gl_pathv[i])) {
                if (be_send_slot(sockpath, matches.gl_pathv[i]) != 0) rc = 2;
                else sent++;
            } else if (be_send_path_entry(sockpath, matches.gl_pathv[i], &sent) != 0) rc = 2;
        }
        globfree(&matches);
    } else {
        struct stat st;
        if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
            rc = be_is_slot_dir(path) ? be_send_slot(sockpath, path) : be_send_root(sockpath, path, &sent);
            if (be_is_slot_dir(path) && rc == 0) sent++;
        } else {
            rc = be_send_path_entry(sockpath, path, &sent);
        }
    }
    if (sent == 0 && rc == 0) {
        fprintf(stderr, "bench: no .p0t files matched '%s'\n", path);
        return 2;
    }
    return rc;
}
