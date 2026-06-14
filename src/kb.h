/*
 * kb.h - parrot0's knowledge base. A sub-system of the brain.
 *
 * This is the spine of the Prolog-like direction (see PRINCIPLES.md and the
 * gen4+ ladder in TASK.md). It begins as the smallest honest thing: a store
 * of GROUND facts — atoms like man(socrates) or owns(alice, cat) — that can be
 * asserted and queried under the closed-world assumption (a fact not present
 * is taken to be false).
 *
 * Deliberately minimal so it can grow: gen5 will add variables/unification to
 * queries, gen6 rules + resolution, gen7 induction ("training") of rules from
 * facts. The representation here (predicate + ground args) is chosen to admit
 * those without a rewrite.
 */
#ifndef PARROT0_KB_H
#define PARROT0_KB_H

#include <stddef.h>

#define KB_MAX_ARGS 4   /* arity ceiling for now */
#define KB_TERM_LEN 64  /* max length of a predicate or argument atom */

typedef struct KB KB;

/* Create / destroy a knowledge base. Returns NULL on allocation failure. */
KB    *kb_create(void);
void   kb_destroy(KB *kb);

/* Assert a ground fact: predicate `pred` with `argc` ground argument atoms.
 * Idempotent (asserting a known fact is a no-op). Returns:
 *   1  fact is now in the KB (added or already present),
 *   0  rejected (bad arity, over-long term, or KB full). */
int    kb_assert(KB *kb, const char *pred, const char *const *args, size_t argc);

/* Query a ground fact. Returns 1 if present (true under closed-world), else 0. */
int    kb_query(KB *kb, const char *pred, const char *const *args, size_t argc);

/* Number of distinct facts currently stored. */
size_t kb_size(const KB *kb);

#endif /* PARROT0_KB_H */