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

/* Provenance of a clause (bit flags, so save can select layers). See DESIGN.md
 * D3: knowledge is composed from layers, and only some are persisted. */
#define KB_BASE       1 /* curated, hand-authorable, long-lived             */
#define KB_SESSION    2 /* asserted this run / loaded from a session file   */
#define KB_INDUCED    4 /* created by kb_induce                             */
#define KB_REFLECTIVE 8 /* the self-model (i_am/module) — never persisted   */

typedef struct KB KB;

/* Create / destroy a knowledge base. Returns NULL on allocation failure. */
KB    *kb_create(void);
void   kb_destroy(KB *kb);

/* Assert a ground fact: predicate `pred` with `argc` ground argument atoms.
 * Idempotent (asserting a known fact is a no-op). Returns:
 *   1  fact is now in the KB (added or already present),
 *   0  rejected (bad arity, over-long term, or KB full).
 * Clears the matching explicit negative fact from the same provenance layer, if
 * any; opposite claims from distinct layers are preserved as conflicts. */
int    kb_assert(KB *kb, const char *pred, const char *const *args, size_t argc);

/* Retract a positive ground fact if present, preserving the order of the rest.
 * Returns 1 if a fact was removed, 0 if it was not there. Does not remove
 * explicit negative knowledge. */
int    kb_retract(KB *kb, const char *pred, const char *const *args, size_t argc);

/* Assert an explicit negative ground fact: known-false `pred(args...)`.
 * Idempotent. Clears the matching positive fact from the same provenance layer,
 * if any; opposite claims from distinct layers are preserved as conflicts. */
int    kb_assert_neg(KB *kb, const char *pred, const char *const *args,
                     size_t argc);

/* True if the exact ground fact is explicitly known false. */
int    kb_is_negated(const KB *kb, const char *pred, const char *const *args,
                     size_t argc);

/* True if the exact ground fact has both positive and negative direct support. */
int    kb_is_conflicted(const KB *kb, const char *pred,
                        const char *const *args, size_t argc);

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

/* Set the provenance tag applied to clauses asserted from now on (default
 * KB_SESSION). Used to mark a file as base vs session at load time. */
void   kb_set_origin(KB *kb, int origin);

/* Load clauses from a human-readable Prolog-like file and union them into the
 * KB (so multiple files JOIN). Format: one clause per line, `pred(a, b).` or
 * `head(X) :- body(X).`, blank lines and `%` comments ignored, malformed lines
 * skipped. An empty/NULL path or a missing file is a no-op. Returns the number
 * of clauses loaded. */
int    kb_load(KB *kb, const char *path);

/* Write clauses whose provenance matches `origin_mask` (e.g. KB_SESSION |
 * KB_INDUCED) to `path`, in the same readable format. Returns the number
 * written, or -1 on error. */
int    kb_save(const KB *kb, const char *path, int origin_mask);

/* Prove a goal AND, if provable, write a one-line explanation of the proof into
 * `out` — e.g. "mortal(socrates) because man(socrates)" — derived from the
 * actual proof tree (facts, rule chains, multi-goal bodies with their
 * bindings). Returns 1 if proven (out filled), 0 if not (out untouched). */
int    kb_explain(KB *kb, const char *pred, const char *const *args,
                  size_t argc, char *out, size_t out_size);

/* Render direct ground facts about `entity` into `out`, e.g.
 * "socrates is a man; socrates is not a dog.". This is a report over stored
 * facts only: it does not include derived beliefs. Returns 1 if anything direct
 * was known, else 0. */
int    kb_describe_entity(const KB *kb, const char *entity,
                          char *out, size_t out_size);

/* True if the predicate is known at all — mentioned by any fact or any rule
 * head, or explicit negative fact. Used (gen16+) to tell the *not-known*
 * ("I don't know about <pred>") from the closed-world *false* ("No.")
 * within a known domain. NOTE: a scaffold —
 * DESIGN.md D6 plans to replace this hardcoded epistemic check with emergent
 * meta-knowledge (reflection + negation-as-failure). */
int    kb_knows_pred(const KB *kb, const char *pred);

/* Collect the distinct UNARY predicate symbols known to the KB — those that
 * appear as a 1-arg fact or as a 1-arg rule head. Used by grounded
 * verbalization (gen39) to enumerate the classes an entity might belong to,
 * including ones reachable only through rules. Returns the count (capped). */
size_t kb_unary_predicates(const KB *kb, char out[][KB_TERM_LEN], size_t max);

/* Collect all distinct predicate symbols (any arity). gen77 for self-model
 * introspection: "what predicates do you know?". Returns count (capped). */
size_t kb_predicates(const KB *kb, char out[][KB_TERM_LEN], size_t max);

/* Write all facts in the KB to out as human-readable lines, one per fact,
 * truncated to out_size. Returns 1, 0 if empty. gen77. */
int    kb_dump_all(const KB *kb, char *out, size_t out_size);

/* Number of distinct facts currently stored. */
size_t kb_size(const KB *kb);

/* Count direct stored facts for a predicate (no rule resolution). gen77. */
size_t kb_pred_fact_count(const KB *kb, const char *pred);

/* Abduction support (gen131): find the FIRST rule whose head predicate is
 * `head` with arity `argc`, and write its body goal predicates into out_body[]
 * (each up to KB_TERM_LEN), returning the body count. 0 if no such rule exists.
 * The current rules are variable-sharing (head(X) :- b0(X), b1(X), …), so the
 * caller can ground each body predicate with the head's argument to recover the
 * premises that WOULD entail the goal. Capped at `max`. */
size_t kb_rule_body_preds(const KB *kb, const char *head, size_t argc,
                          char out_body[][KB_TERM_LEN], size_t max);

#endif /* PARROT0_KB_H */