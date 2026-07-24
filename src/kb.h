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
#define KB_TERM_LEN 512 /* max length of a predicate or argument atom. 512 is
                         * the next power-of-two above the longest curated quoted
                         * value (335 bytes including quotes); unlike the former
                         * 128-byte ceiling it leaves useful runtime-growth room
                         * without treating response templates specially. */
#define KB_MAX_BODY 8   /* goals per rule body (conjunctive rules) */

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

/* Assert a CONJUNCTIVE unary rule  head(X) :- body[0](X), …, body[n-1](X)
 * (gen133). All goals share the single variable X. Idempotent (an identical
 * rule is not duplicated). nbody must be 1..KB_MAX_BODY. Returns 1 on success
 * (or if already present), 0 on error. With nbody==1 it is equivalent to
 * kb_assert_rule. */
int    kb_assert_rule_n(KB *kb, const char *head,
                        const char *const *bodies, size_t nbody);

/* A goal/term: predicate + args. An arg is a constant atom or a variable
 * ($-prefixed, or legacy uppercase/'_'). `neg` marks a BODY goal as
 * negation-as-failure (naf(G)); 0 for the head and ordinary goals (U6). */
typedef struct {
    const char *pred; const char *const *args; size_t argc; int neg;
} KbGoal;

/* Assert a definite clause  head :- body[0], …, body[nbody-1]  with FULL n-ary
 * head and goals carrying distinct/shared variables — the general shape a .p0
 * rule can express, unlike kb_assert_rule_n (unary). Idempotent (an identical
 * clause is not duplicated). nbody must be 1..KB_MAX_BODY, each argc ≤
 * KB_MAX_ARGS. Returns 1 on success (or if already present), 0 on error. */
int    kb_assert_clause(KB *kb, const KbGoal *head,
                        const KbGoal *body, size_t nbody);

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

/* Dynamically enumerate every distinct binding that `kb_match` would return,
 * without a caller-selected cap. On success the caller owns `*out` and must
 * free it; an empty result is successful with `*nout == 0`. */
int kb_match_all(const KB *kb, const char *pred,
                 const char *const *args, size_t argc,
                 char (**out)[KB_TERM_LEN], size_t *nout);

/* Universal evidence matching (docs/plans/universal-input.md).
 *
 * A two-argument KB relation such as
 *
 *     intent_cue(code_fix, "fix this")
 *     register_evidence(c, keyword(int))
 *
 * describes hypotheses in its first argument and evidence in its second.  The
 * fixed engine below evaluates the evidence against a byte stream; the classes,
 * surface forms and structural choices remain facts.  `start`/`len` are byte
 * offsets in `text`, and `evidence` is preserved exactly as it lives in the KB
 * so it can be cited or retracted without losing quote/compound-term fidelity.
 */
#define KB_EVIDENCE_PROOF_LEN 512

typedef struct {
    char   hypothesis[KB_TERM_LEN];
    char   evidence[KB_TERM_LEN];
    size_t start;
    size_t len;
    int    weight;
} KbEvidenceMatch;

/* Enumerate every occurrence supported by `relation`/2.  If `hypothesis` is
 * non-NULL only that class is considered; NULL discovers every class from the
 * KB.  Results are ordered by byte offset, then by hypothesis/evidence. */
size_t kb_evidence_matches(const KB *kb, const char *relation,
                           const char *hypothesis, const char *text,
                           KbEvidenceMatch *out, size_t max);

/* Compare hypotheses using the same scorer for intent_cue, register_evidence,
 * segment_role, and future evidence relations. `candidates` may be NULL/empty
 * to discover all first arguments from the relation. Returns:
 *   1  one unique best hypothesis (winner/proof/score filled)
 *   0  no evidence matched (proof is a typed gap)
 *  -1  top hypotheses tied (proof names the ambiguity; no tiebreak is applied)
 * Evidence weights are data (`evidence_weight(Kind, Integer)`); absent weights
 * default to one. `default` evidence is considered only when no specific
 * evidence matched, so prose(default) cannot tie a genuinely supported register.
 */
int kb_hypothesis_best(const KB *kb, const char *relation, const char *text,
                       const char *const *candidates, size_t ncandidates,
                       char *winner, size_t winner_sz,
                       int *score, char *proof, size_t proof_sz);

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
 * KB (so multiple files JOIN). Format: logical clauses terminated by a top-level
 * `.`; clauses may span lines and several clauses may share one line. Blank
 * space and `%` comments outside quoted values are ignored. Malformed terminated
 * clauses are skipped with an explicit diagnostic; clauses beyond the structural
 * size/body-goal budgets are rejected whole, never loaded as a valid prefix. A
 * non-empty unterminated EOF fragment is rejected just as loudly. An empty/NULL
 * path or a missing file is a no-op. Returns the number of clauses loaded. */
int    kb_load(KB *kb, const char *path);

/* Write clauses whose provenance matches `origin_mask` (e.g. KB_SESSION |
 * KB_INDUCED) to `path`, in the same readable format. Returns the number
 * written, or -1 on error. */
int    kb_save(const KB *kb, const char *path, int origin_mask);

/* Persist SESSION|INDUCED knowledge, ROUTING each ground fact into the curated KB
 * file that already holds its nearest kin (a soft "save-map": coordinate =
 * (predicate, first-arg); tiers exact-pair -> same-predicate -> default). Positive
 * facts with a home are inserted in place under `root`'s tree; everything else
 * (unrouted facts, negatives, rules) goes to `default_path`. Rebuilds the on-disk
 * index `<root>/savemap.tsv`. Returns the clause count. Used by brain_save_session
 * when PARROT0_KB_ROOT is set. */
int    kb_save_routed(const KB *kb, const char *default_path, const char *root);

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

/* gen313: the DEFINITION view — like kb_describe_entity but only facts that
 * speak about `entity` as their SUBJECT and render naturally (unary class
 * facts or quoted descriptions). Never dumps a raw clause and never claims on
 * a mere object mention, so definition frames ("what is the X", "define X")
 * cannot leak machinery; the full dump stays with kb_describe_entity. */
int    kb_define_entity(const KB *kb, const char *entity,
                        char *out, size_t out_size);

/* gen155: recall the description-bearing concept whose key+description best
 * OVERLAP the query's content words `qwords` (cognate-tolerant, so it crosses
 * EN<->IT for loanwords). Writes the winning key/description and returns the
 * overlap score, or 0 if no concept clears the threshold and margin. The first
 * brick of a similarity space, derived from real KB structure, not enumeration. */
int    kb_nearest_concept(const KB *kb, const char *const *qwords, size_t nq,
                          char *key_out, size_t key_sz,
                          char *desc_out, size_t desc_sz);

/* gen157: true if `term` is the key of a description-bearing concept fact. */
int    kb_is_concept_key(const KB *kb, const char *term);

/* gen172: write the dequoted definition of the concept keyed by `key` into
 * `out`; returns 1 if found, 0 otherwise. Used to answer a re-ask from RAM. */
int    kb_concept_def(const KB *kb, const char *key, char *out, size_t out_size);

/* gen345 (test-engine): assert a single clause from text via the full .p0 parser.
 * Trailing '.' optional. Returns the number of clauses added (0 or 1). */
int    kb_load_clause(KB *kb, const char *text);

/* gen344: a language-specific full-sentence definition — concept_gloss(Key, Lang,
 * "sentence"). Returns 1 + the dequoted sentence when one exists for `lang`, so
 * the definitional path can mirror the asker's language. Knowledge, not C. */
int    kb_concept_gloss(const KB *kb, const char *key, const char *lang,
                        char *out, size_t out_size);

/* gen157: recover an emergent containment relation — the concept whose
 * description NAMES `term` (e.g. circulatory's description names the heart, so
 * "what is the heart part of?" -> circulatory). Derived from text, never
 * asserted. Writes key/description, returns 1 on a hit. */
int    kb_concept_mentioning(const KB *kb, const char *term,
                             char *key_out, size_t key_sz,
                             char *desc_out, size_t desc_sz);

/* gen158: materialize the emergent containment relation as first-class part_of/2
 * facts (KB_REFLECTIVE, never persisted), so the resolution engine can prove and
 * query it. Only CONTAINER predicates (those whose facts repeatedly name other
 * concept keys) are materialized, so a mere mention is not mistaken for
 * containment. Returns the number of relations added. */
int    kb_derive_part_of(KB *kb);

/* True if the predicate is known at all — mentioned by any fact or any rule
 * head, or explicit negative fact. Used (gen16+) to tell the *not-known*
 * ("I don't know about <pred>") from the closed-world *false* ("No.")
 * within a known domain. NOTE: a scaffold —
 * DESIGN.md D6 plans to replace this hardcoded epistemic check with emergent
 * meta-knowledge (reflection + negation-as-failure). */
int    kb_knows_pred(const KB *kb, const char *pred);

/* True if `pred` appears as a BODY goal in any rule (gen133). Lets the intake
 * accept "X is friendly" without an article precisely — and only — when
 * "friendly" is a class some rule already depends on, so the article-free frame
 * cannot hijack arbitrary "X is Y" prose. */
int    kb_rule_body_mentions(const KB *kb, const char *pred);

/* Collect the distinct UNARY predicate symbols known to the KB — those that
 * appear as a 1-arg fact or as a 1-arg rule head. Used by grounded
 * verbalization (gen39) to enumerate the classes an entity might belong to,
 * including ones reachable only through rules. Returns the count (capped). */
size_t kb_unary_predicates(const KB *kb, char out[][KB_TERM_LEN], size_t max);

/* Collect all distinct predicate symbols (any arity). gen77 for self-model
 * introspection: "what predicates do you know?". Returns count (capped). */
size_t kb_predicates(const KB *kb, char out[][KB_TERM_LEN], size_t max);

/* Collect the distinct direct binary relations that hold for (left, right).
 * This is an indexed-shape discovery primitive for analogy/routing consumers:
 * one linear fact pass instead of enumerating every predicate and re-querying
 * the whole KB for each one. Returns count (capped). */
size_t kb_binary_relations(const KB *kb, const char *left, const char *right,
                           char out[][KB_TERM_LEN], size_t max);

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

/* Branching abduction (gen134): how many rules have head predicate `head` with
 * arity `argc` — i.e. how many distinct ways the goal could be derived. */
size_t kb_rules_for_head(const KB *kb, const char *head, size_t argc);

/* Body predicates of the `idx`-th (0-based) rule whose head is `head`/`argc`,
 * written into out_body[] (each KB_TERM_LEN). Returns the body count, or 0 if
 * there is no such rule. Lets the caller enumerate each ALTERNATIVE explanation. */
size_t kb_nth_rule_body_preds(const KB *kb, const char *head, size_t argc,
                              size_t idx, char out_body[][KB_TERM_LEN], size_t max);

#endif /* PARROT0_KB_H */
