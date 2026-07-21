#ifndef PARROT0_TESTENG_H
#define PARROT0_TESTENG_H

#include <stdio.h>
#include "brain.h"

/* gen345 — the test-engine (docs/plans/test-engine.md).
 *
 * A local validation SERVICE: `parrot0 --test-engine [--sock PATH]` boots ONE
 * brain and listens on a Unix socket. Each `.p0t` file is then sent as its own
 * connection by the client (`parrot0 --test-send FILE`), so the KB loads once and
 * brain state carries across files. `parrot0 --test-report` asks for the
 * aggregate summary and shuts the daemon down, exiting non-zero if anything
 * failed. Fail-fast: a --test-send exits 1 the moment its own file fails. See
 * testeng.c for the `.p0t` grammar (assert-only, multi-turn, multi-line).
 *
 * The default socket path when --sock is omitted. */
#define TEST_ENGINE_SOCK_DEFAULT "obj/test-engine.sock"

/* Daemon: listen on `sockpath`, validating each connection's `.p0t` payload
 * against `b`. Returns when a `!shutdown` payload arrives; the exit code is
 * 1 if any assertion failed across the whole session, else 0. */
int test_engine_serve(Brain *b, const char *sockpath);

/* Client: connect to `sockpath` (retrying briefly while the daemon starts), send
 * everything on `in`, and print a ONE-LINE report — `label` (the file name) plus
 * how many assertions passed. On failure it first prints the failing assertions'
 * detail and returns 1; otherwise 0. A NULL `label` prints a grand-total line
 * instead (used by --test-report). */
int test_engine_send(const char *sockpath, FILE *in, const char *label);

/* Client convenience: send a literal command string (e.g. "!shutdown\n"). */
int test_engine_send_str(const char *sockpath, const char *payload, const char *label);

/* Batch fallback: validate a whole `.p0t` stream from `in` against `b` in-process
 * (no socket), printing the report to stdout. Same exit codes as the CLI. */
int test_engine_run(Brain *b, FILE *in);

#endif /* PARROT0_TESTENG_H */