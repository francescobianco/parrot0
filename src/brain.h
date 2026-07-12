/*
 * brain.h - the part of parrot0 that EVOLVES.
 *
 * Everything in this header/implementation pair is fair game for the
 * self-improvement loop. The I/O shell (main.c) should stay stable; the
 * intelligence of the agent emerges here, as pure C algorithms.
 *
 * Contract (keep this stable so the loop can always compile & test):
 *   - brain_respond() takes a single line of user input and writes a
 *     single line of response into `out` (NUL-terminated, never overflowing).
 *   - It must be deterministic given the same input + same brain state,
 *     so the test harness can check behaviour.
 */
#ifndef PARROT0_BRAIN_H
#define PARROT0_BRAIN_H

#include <stddef.h>

/* Opaque, persistent state the brain may accumulate during a session.
 * v0 keeps almost nothing here, but evolution will grow it. */
typedef struct Brain Brain;

/* Forward-declared so a host can reach the brain's KB (brain_kb) without
 * pulling in kb.h; identical to kb.h's own typedef (C11 allows the repeat). */
typedef struct KB KB;

/* gen277: the brain's knowledge base, so a host (the MCP engine) can call the
 * kb.h primitives directly on it — expose the engine, don't wrap every call. */
KB *brain_kb(Brain *b);

/* Create / destroy brain state. Returns NULL on allocation failure. */
Brain *brain_create(void);
void   brain_destroy(Brain *b);

/* Produce a response to `input`, writing at most `out_size-1` chars into
 * `out` plus a NUL terminator. Returns the number of chars written. */
size_t brain_respond(Brain *b, const char *input, char *out, size_t out_size);

/* A short, human-readable name/version of the current brain generation.
 * Bump this whenever the algorithm meaningfully changes. */
const char *brain_version(void);

/* Load a knowledge file into the brain's KB. `as_base` non-zero tags the
 * loaded clauses as base knowledge, otherwise as session knowledge. An
 * empty/NULL path or missing file is a no-op. Returns clauses loaded. */
int brain_load(Brain *b, const char *path, int as_base);

/* Persist the session delta (session + induced clauses, never the reflective
 * self-model) to `path`. Returns clauses written, or -1 on error. */
int brain_save_session(Brain *b, const char *path);

/* gen331 (TODO.md P1/09): the EFFECTIVE runtime policy, projected into the KB at
 * boot as policy(tools|network|mode, …) and read back through these. Every host
 * and every module must ask HERE rather than call getenv() and reach its own
 * conclusion — otherwise the banner can promise what a decline denies, which is
 * precisely what `make chat` did: it advertised the AGI profile, refused every
 * file request as "I don't understand", and silently enabled the network. */
int  brain_policy_on(Brain *b, const char *key);      /* "tools" / "network" */
void brain_mode(Brain *b, char *out, size_t cap);     /* conversational|agent|acquire */

/* gen276: load the outer KB layers on top of a freshly-created brain — the
 * curated base, the session delta, the coding expert, and (if named by
 * PARROT0_PROFILE) an expert/skill profile. brain_create() already loads the
 * kernel lexicon and the reflective self-model; brain_boot() adds what the CLI
 * shell used to load inline, so the same full boot is reachable from any host
 * (the chat REPL, the daemon, the MCP engine). The file paths come from the
 * environment (PARROT0_BASE / PARROT0_SESSION / PARROT0_PROFILE), each with the
 * historical default, an empty value disabling that layer. */
void brain_boot(Brain *b);

/* gen276: forget the UNSAVED current session and reload every knowledge file
 * from disk into a fresh KB, in place — so an agent that has written new
 * knowledge to a `.p0` file (e.g. via the MCP engine) makes parrot0 pick it up
 * WITHOUT restarting the process: just `/restore`. Anything asserted this
 * session but never persisted (via /save or session.save) is dropped; anything
 * on disk — including files changed since boot — is re-read. All conversational
 * state (name, topics, proof trace, open worlds, …) resets to a clean session.
 * The caller's `Brain *` stays valid (rebuilt in place). Returns the number of
 * clauses now loaded, or -1 on allocation failure (in which case `b` is
 * unchanged). */
int brain_reload(Brain *b);

#endif /* PARROT0_BRAIN_H */