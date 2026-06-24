/*
 * serve.h - parrot0's built-in OpenAI-compatible HTTP server (gen221).
 *
 * Replaces the external scripts/pi_server.py wrapper: parrot0 now serves the
 * /v1/chat/completions, /v1/models and /health endpoints directly from C, so a
 * coding agent (pi, or anything OpenAI-compatible) can talk to it over HTTP with
 * no Python bridge. Launched via `parrot0 --daemon --port <N> [--host <H>]`.
 *
 * The brain is single-instance and stateful, so requests are served iteratively
 * (one at a time) — the same serialization the Python wrapper enforced with a
 * lock. This keeps brain state deterministic across turns.
 */
#ifndef PARROT0_SERVE_H
#define PARROT0_SERVE_H

#include "brain.h"

/* Run the HTTP server on host:port, dispatching each chat request through
 * `brain`. Blocks until the process is signalled. Returns a process exit code
 * (non-zero on a fatal socket/bind error). */
int serve_http(Brain *brain, const char *host, int port);

#endif /* PARROT0_SERVE_H */
