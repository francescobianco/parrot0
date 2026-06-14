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

/* Create / destroy brain state. Returns NULL on allocation failure. */
Brain *brain_create(void);
void   brain_destroy(Brain *b);

/* Produce a response to `input`, writing at most `out_size-1` chars into
 * `out` plus a NUL terminator. Returns the number of chars written. */
size_t brain_respond(Brain *b, const char *input, char *out, size_t out_size);

/* A short, human-readable name/version of the current brain generation.
 * Bump this whenever the algorithm meaningfully changes. */
const char *brain_version(void);

#endif /* PARROT0_BRAIN_H */