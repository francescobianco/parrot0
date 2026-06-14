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

/* Assert a definite rule of the form  head(X) :- body(X)  (unary, one shared
 * variable — the shape gen6 needs). Idempotent. Returns 1 if stored/known. */
int    kb_assert_rule(KB *kb, const char *head, const char *body);

/* Query a fact. For unary queries this performs backward-chaining resolution
 * over facts AND rules (with a depth limit); for other arities it is a direct
 * ground lookup. Returns 1 if provable (closed-world), else 0. */
int    kb_query(KB *kb, const char *pred, const char *const *args, size_t argc);

/* Match a pattern where any arg may be a VARIABLE, signalled by passing NULL
 * in args[i]. For every stored fact that unifies (predicate, arity, and all
 * ground args equal), the binding of the FIRST variable slot is appended to
 * `out` (each entry up to KB_TERM_LEN). Results are in insertion order. Returns
 * the number of bindings written (capped at `max`). */
size_t kb_match(const KB *kb, const char *pred, const char *const *args,
                size_t argc, char out[][KB_TERM_LEN], size_t max);

/* Induce definite rules from the ground facts — parrot0's deterministic,
 * legible analogue of "training" (see PRINCIPLES.md). For unary predicates
 * P, Q with P != Q: if EVERY constant c with fact P(c) also has fact Q(c),
 * and there are at least `min_support` such constants, induce Q(X) :- P(X).
 * Newly induced rules are asserted into the KB and reported via out_head /
 * out_body (each up to KB_TERM_LEN). Returns the number induced (capped at
 * `max`). Rules already present are not re-induced. */
size_t kb_induce(KB *kb, size_t min_support,
                 char out_head[][KB_TERM_LEN], char out_body[][KB_TERM_LEN],
                 size_t max);

/* Number of distinct facts currently stored. */
size_t kb_size(const KB *kb);

#endif /* PARROT0_KB_H */