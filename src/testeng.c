/* testeng.c — the parrot0 test-engine (gen345, docs/plans/test-engine.md).
 *
 * A local validation SERVICE. `parrot0 --test-engine` boots ONE brain and listens
 * on a Unix socket; `make test` then sends each `.p0t` file as its own connection
 * (`parrot0 --test-send FILE`) and finally `parrot0 --test-report`. The KB loads
 * once, in the daemon; the client loads NOTHING — it is a bare socket relay, so
 * every `make test` line is cheap.
 *
 * ── .p0t GRAMMAR (minimal base) ───────────────────────────────────────────────
 *
 *   # comment                 a comment line, ignored (also: blank lines)
 *   [test NAME]               open a named section (for the report)
 *
 *   > text                    send `text` to parrot0 as one user turn
 *   < text                    assert the reply equals `text`
 *
 * A section is MULTI-TURN: list several `> / <` pairs and they run in order
 * against the same live brain (so coreference across turns is testable). A reply
 * is MULTI-LINE: write one `<` per output line and consecutive `<` lines are
 * joined and matched against the whole multi-line reply.
 *
 * Mock / stub / flag / KB-state control are NOT part of this base — they will be
 * designed later. `!shutdown` is the one internal control line (sent by
 * --test-report) and is not a test primitive.
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

    int passed, failed, line_no, shutdown;
} TeState;

/* ── assertion ─────────────────────────────────────────────────────────────── */

static void te_flush(TeState *t) {
    if (!t->have_expect) return;
    const char *got = t->have_reply ? t->reply : "";
    if (strcmp(t->expect, got) == 0) {
        t->passed++;
        fprintf(t->out, "  PASS  [%s] line %d\n",
                t->section[0] ? t->section : "-", t->expect_startline);
    } else {
        t->failed++;
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
    brain_respond(t->b, text, t->reply, sizeof t->reply);
    size_t n = strlen(t->reply);
    while (n > 0 && (t->reply[n - 1] == '\n' || t->reply[n - 1] == '\r'))
        t->reply[--n] = '\0';
    t->have_reply = 1;
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
            fprintf(t->out, "[%s]\n", t->section);
            continue;
        }
        if (p[0] == '>') { te_turn(t, p[1] == ' ' ? p + 2 : p + 1); continue; }
        if (p[0] == '<') { te_expect(t, p[1] == ' ' ? p + 2 : p + 1); continue; }
        if (strncmp(p, "!shutdown", 9) == 0) { te_flush(t); t->shutdown = 1; continue; }

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

    for (;;) {
        int cfd = accept(lfd, NULL, NULL);
        if (cfd < 0) { if (errno == EINTR) continue; break; }

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
            int failed_before = t.failed;
            te_process_stream(&t, fin);
            if (t.shutdown) {
                /* --test-report: the whole-session verdict, then stop. */
                te_summary(&t);
                fprintf(fout, "EXIT %d\n", t.failed > 0 ? 1 : 0);
            } else {
                /* --test-send: FAIL-FAST — this file's own verdict, so a failing
                 * file makes the client exit 1 and `make` stops right there. */
                fprintf(fout, "EXIT %d\n", (t.failed - failed_before) > 0 ? 1 : 0);
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

int test_engine_send(const char *sockpath, FILE *in) {
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

    int code = 0;
    if (rep && len) {
        /* find the last non-empty line; if it is "EXIT n", strip it and adopt n */
        size_t e = len;
        while (e > 0 && (rep[e - 1] == '\n' || rep[e - 1] == '\r')) e--;
        size_t ls = e;
        while (ls > 0 && rep[ls - 1] != '\n') ls--;
        if (strncmp(rep + ls, "EXIT ", 5) == 0) {
            code = atoi(rep + ls + 5);
            len = ls;                 /* hide the control line from the printout */
        }
        fwrite(rep, 1, len, stdout);
    }
    free(rep);
    return code;
}

int test_engine_send_str(const char *sockpath, const char *payload) {
    FILE *m = fmemopen((void *)payload, strlen(payload), "r");
    if (!m) return 2;
    int rc = test_engine_send(sockpath, m);
    fclose(m);
    return rc;
}