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
 *   !forget PRED(a, b)        drop one specific ground fact
 *   !forget @LAYER            drop a whole provenance layer: @base, @session,
 *                             @induced, @reflective, @hypothetical
 *   !assert PRED(a, b, …)     add a ground fact from inside the test
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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include "testeng.h"
#include "env.h"
#include "kb.h"

#ifndef TE_LINE
#define TE_LINE 4096
#endif
#define TE_NAME 64

typedef struct {
    Brain *b;
    FILE  *out;              /* where the report is written (a socket or stdout) */

    char section[TE_NAME];   /* current [section] name, for the report */

    char reply[TE_LINE];     /* reply from the most recent `>` turn */
    int  have_reply;

    char expect[TE_LINE];    /* accumulated consecutive `<` expected lines */
    size_t expect_len;
    int  have_expect;
    int  expect_startline;
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
    brain_reload(t->b);
    t->have_reply = 0;
    te_mark_clean(t);
    if (getenv("PARROT0_TE_DEBUG"))
        fprintf(stderr, "test-engine: brain reloaded (config changed)\n");
}

/* ── assertion ─────────────────────────────────────────────────────────────── */

static void te_flush(TeState *t) {
    if (t->poisoned) {                /* the turn already failed (timeout) — absorb its < */
        t->poisoned = 0;
        t->expect_len = 0; t->expect[0] = '\0'; t->have_expect = 0;
        return;
    }
    if (!t->have_expect) return;
    const char *got = t->have_reply ? t->reply : "";
    int ok;
    switch (t->expect_mode) {
        case TE_EXPECT_HAS:   ok = strstr(got, t->expect) != NULL; break;
        case TE_EXPECT_LACKS: ok = strstr(got, t->expect) == NULL; break;
        default:              ok = strcmp(t->expect, got) == 0;    break;
    }
    if (ok) {
        t->passed++;                         /* silent — the one-line file report counts it */
    } else {
        t->failed++;                         /* only failures print, with useful detail */
        fprintf(t->out, "  FAIL  [%s] line %d\n",
                t->section[0] ? t->section : "-", t->expect_startline);
        fprintf(t->out, "        expected%s: %s\n",
                t->expect_mode == TE_EXPECT_HAS   ? " (contains)" :
                t->expect_mode == TE_EXPECT_LACKS ? " (absent)"   : "", t->expect);
        fprintf(t->out, "        got:      %s\n", got);
    }
    t->expect_len = 0;
    t->expect[0] = '\0';
    t->have_expect = 0;
    t->expect_mode = TE_EXPECT_EXACT;
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

        if (*p == '[') {
            te_flush(t);
            char *close = strchr(p, ']');
            if (!close) { syntax_err = 1; continue; }
            *close = '\0';
            char *hdr = p + 1;
            char *sp = strchr(hdr, ' ');          /* label = text after the type word */
            snprintf(t->section, sizeof t->section, "%s", sp ? sp + 1 : hdr);
            t->timeout_sec = TE_DEFAULT_TIMEOUT;  /* each test starts at the 1s default */
            continue;                             /* section name surfaces only in a FAIL */
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