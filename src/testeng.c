/* testeng.c — the parrot0 test-engine (gen345, docs/plans/test-engine.md).
 *
 * A local validation SERVICE. `parrot0 --test-engine` boots ONE brain and listens
 * on a Unix socket; `make test` then sends each `.p0t` file as its own connection
 * (`parrot0 --test-send FILE`) and finally `parrot0 --test-report`. The KB loads
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
    if (strcmp(t->expect, got) == 0) {
        t->passed++;                         /* silent — the one-line file report counts it */
    } else {
        t->failed++;                         /* only failures print, with useful detail */
        fprintf(t->out, "  FAIL  [%s] line %d\n",
                t->section[0] ? t->section : "-", t->expect_startline);
        fprintf(t->out, "        expected: %s\n", t->expect);
        fprintf(t->out, "        got:      %s\n", got);
    }
    t->expect_len = 0;
    t->expect[0] = '\0';
    t->have_expect = 0;
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

static void te_expect(TeState *t, const char *raw) {
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