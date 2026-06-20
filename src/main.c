/*
 * main.c - the I/O shell of parrot0. STABLE.
 *
 * This file is intentionally boring and should rarely change: it owns the
 * read-eval-print loop, line buffering and the chat protocol, so that the
 * self-improvement loop can focus entirely on src/brain.c.
 *
 * Protocol (kept deterministic & test-friendly):
 *   - Reads one line of user input per turn from stdin.
 *   - Writes exactly one line of response to stdout (and flushes).
 *   - Decorative prompts go to stderr, so piping stdin/stdout stays clean
 *     for the test harness.
 *   - Input "/quit", "/exit" or EOF (Ctrl-D) ends the session.
 */
#include "brain.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Long enough to hold a whole benchmark prompt (passage+question) on one line;
 * fgets would otherwise split a long passage across reads, hiding its tail
 * markers from the brain (gen49). */
#define LINE_MAX_LEN 65536
#define RESP_MAX_LEN 8192

static void chomp(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r')) {
        s[--n] = '\0';
    }
}

/* Opt-in input capture (off unless PARROT0_TRACE names a file). Appends every
 * received input line so the self-improvement loop can SEE exactly what a
 * benchmark feeds parrot0 — a discovery tool for which reasoning features to
 * build next, never a runtime behaviour. Stays in the I/O shell because what
 * arrives on stdin is the shell's concern, not the brain's. */
static void trace_input(const char *line) {
    const char *path = getenv("PARROT0_TRACE");
    if (!path || !*path) return;
    FILE *f = fopen(path, "a");
    if (!f) return;
    fprintf(f, "%s\n", line);
    fclose(f);
}

int main(void) {
    Brain *brain = brain_create();
    if (!brain) {
        fprintf(stderr, "parrot0: out of memory\n");
        return 1;
    }

    /* Knowledge layers: a curated base file + a session file of discovered
     * facts, joined into RAM at startup. Paths come from the environment
     * (empty disables loading — used by the hermetic test harness).
     * gen150: PARROT0_PROFILE loads a knowledge profile (e.g. profiles/agi.p0)
     * that chains experts and skills via :- include directives. */
    const char *base = getenv("PARROT0_BASE");
    const char *sess = getenv("PARROT0_SESSION");
    const char *profile = getenv("PARROT0_PROFILE");
    if (!base) base = "kb/core/base.p0";
    if (!sess) sess = "kb/core/session.p0";
    brain_load(brain, base, 1);
    brain_load(brain, sess, 0);
    brain_load(brain, "kb/experts/programming/coding.p0", 1); /* gen149: coding domain knowledge */
    if (profile && *profile)
        brain_load(brain, profile, 1);    /* gen150: expert/skill profile */

    fprintf(stderr, "parrot0 [%s] - say something ('/quit' to exit, "
                    "'/save' to persist)\n", brain_version());

    char line[LINE_MAX_LEN];
    char resp[RESP_MAX_LEN];

    for (;;) {
        fprintf(stderr, "you> ");
        fflush(stderr);

        if (!fgets(line, sizeof line, stdin)) {
            break; /* EOF / Ctrl-D */
        }
        chomp(line);
        trace_input(line);

        if (strcmp(line, "/quit") == 0 || strcmp(line, "/exit") == 0) {
            break;
        }
        if (strcmp(line, "/save") == 0) {
            int n = brain_save_session(brain, sess);
            if (n >= 0) fprintf(stderr, "parrot0: saved %d clause(s) to %s\n",
                                n, sess);
            else        fprintf(stderr, "parrot0: could not save to %s\n", sess);
            continue;
        }
        if (line[0] == '\0') {
            continue; /* ignore empty turns */
        }

        brain_respond(brain, line, resp, sizeof resp);
        printf("%s\n", resp);
        fflush(stdout);
    }

    fprintf(stderr, "\nparrot0: bye\n");
    brain_destroy(brain);
    return 0;
}