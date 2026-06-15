# parrot0 evolution journal

Newest entries on top. One entry per iteration of the loop (see LOOP.md).

---

## 2026-06-15 — gen23: tiny textual entailment surface (SuperGLUE-like)

**Changed:** `brain.c` → `gen23-entailment`.
- Added a constrained single-turn entailment form:
  `premise: <clauses>; hypothesis: <query>`.
- Premises are evaluated in a temporary KB, so benchmark probes do not pollute
  session knowledge.
- Multiple premise clauses can be separated with `and`; `base says` / `session
  says` prefixes let the micro-driver create source conflicts structurally.
- Entailment returns `Entailed.`, `Not entailed.`, `Contradicted.`,
  `Conflicted.` or `Unknown.` using the same KB statuses as normal queries.
- New `tests/cases/entail.chat` covers fact entailment, rule+fact derivation,
  explicit negative contradiction, source conflict, false known-domain and
  unknown-domain cases.

**Why:** This is the first short-text inference slice from the SuperGLUE-like
benchmark driver in DESIGN D8, kept small enough to reuse the existing KB rather
than add a benchmark-only parser.

**Observed:** all suites green — 13 conversation, 10 persistence, 3 multigoal,
14 grammar, 2 anonymous-var, 5 explanation checks.

**Method watch (D5.1):** the grammar is intentionally tiny. The value is the
status pipeline and temporary-KB driver, not broad natural-language parsing.

**Next:** add an explainable entailment variant so entailed answers can expose
the proof path instead of returning only a label.

---

## 2026-06-15 — gen22: minimal discourse coreference (SuperGLUE-like)

**Changed:** `brain.c` → `gen22-coref`.
- The knowledge surface now remembers the most recent concrete entity.
- `he/she/it/they/him/her/them` resolve to that entity for unary assertions,
  unary ground queries, unary explanations and direct belief reports.
- Pronouns without an antecedent are rejected honestly instead of becoming
  literal KB constants.
- New `tests/cases/coref.chat` covers unknown antecedent, assertion,
  subsequent query, negative assertion, direct report and explanation.

**Why:** First application of the benchmark-driver framing from DESIGN D8.
SuperGLUE-like pressure starts with language foundations: context and
coreference over short factual discourse.

**Observed:** all suites green — 12 conversation, 10 persistence, 3 multigoal,
14 grammar, 2 anonymous-var, 5 explanation checks.

**Method watch (D5.1):** intentionally a single discourse pointer. No gender,
number, salience ranking or multi-candidate reference yet; the point is to stop
obviously wrong `teacher(she)`-style literal storage.

**Next:** continue SuperGLUE-like pressure with a tiny textual-entailment
surface over short premise/hypothesis pairs.

---

## 2026-06-15 — gen21: conflict-aware explanations (T3/T2 slice)

**Changed:** `brain.c` → `gen21-explain-conflict`.
- `explain_reply` now checks exact conflicts before rendering a proof.
- A disputed claim answers `I have conflicting evidence for that.` instead of
  citing the positive side as settled.
- Persistence tests cover unary and binary conflicted explanations.
- `DESIGN.md` now records the user's benchmark-driver framing: SuperGLUE,
  MMLU and BIG-Bench Hard adapted as diagnostic pressure for parrot0.

**Why:** gen20 made yes/no queries conflict-aware; explanations needed the same
honesty so proof traces do not launder disputed claims into facts.

**Observed:** all suites green — 11 conversation, 10 persistence, 3 multigoal,
14 grammar, 2 anonymous-var, 5 explanation checks.

**Method watch (D5.1):** still exact-ground. No source proof tree yet; this only
prevents an actively conflicted claim from being explained as settled.

**Next:** start applying the benchmark-driver frame with a SuperGLUE-like
foundation slice: minimal discourse coreference for short factual turns.

---

## 2026-06-15 — gen20: conflict-aware ground queries (T3 slice)

**Changed:** `brain.c` → `gen20-query-conflict`; `kb_is_conflicted` in `kb.{h,c}`.
- Exact positive+negative ground collisions now have a query-visible status.
- Unary and binary ground NL queries check `kb_is_conflicted` before `kb_query`
  and answer `Conflicted.` when direct support disagrees.
- Same-session correction still settles to a negative fact and answers `No.`.
- Persistence tests now cover both unary base/session conflict and binary exact
  conflict loaded from knowledge files.

**Why:** gen19 preserved disagreement but ground queries still flattened it into
`No.`. This makes the response layer expose the T3 status directly.

**Observed:** all suites green — 11 conversation, 10 persistence, 3 multigoal,
14 grammar, 2 anonymous-var, 5 explanation checks.

**Method watch (D5.1):** exact-ground only. Variable queries and derived
conflicts remain deliberately out of scope.

**Next:** make explanations conflict-aware so `why ...?` does not cite one side
of a disputed claim as if it were settled.

---

## 2026-06-15 — gen19: source-aware conflict sketch (T3 slice)

**Changed:** `brain.c` → `gen19-conflict`; conflict preservation in `kb.c`.
- Positive/negative exact opposites still overwrite within the same provenance
  layer, so same-session correction remains simple.
- Opposite claims from distinct layers are preserved. A session negative can now
  coexist with a base positive instead of deleting it from RAM.
- `kb_describe_entity` renders exact positive/negative collisions as a direct
  conflict, e.g. `socrates is conflicted about being a man.`.
- Persistence tests now cover base/session disagreement while retaining the
  same-session correction behavior from gen17.

**Why:** T3 needs disagreement to be representable before it can be reasoned
about. This keeps correction cheap while preserving a real source boundary.

**Observed:** all suites green — 11 conversation, 9 persistence, 3 multigoal,
14 grammar, 2 anonymous-var, 5 explanation checks.

**Method watch (D5.1):** still deliberately narrow. Conflict is exact-ground
only and reported in the direct belief surface; query/explanation semantics are
left for the next small steps rather than guessed wholesale.

**Next:** make ground yes/no queries conflict-aware so disagreement is not
silently flattened into `No.`.

---

## 2026-06-15 — gen18: direct belief reports for entities (T3 slice)

**Changed:** `brain.c` → `gen18-beliefs`; `kb_describe_entity` in `kb.{h,c}`.
- New surface: `what do you know about <x>?` reports direct ground facts about
  an entity.
- Positive unary facts render as `x is a pred`; explicit negative unary facts
  render as `x is not a pred`; relation facts render as their direct KB term.
- The report is intentionally direct-only: it does not list derived facts or
  hide that gap behind speculation.
- New `tests/cases/belief.chat` covers unknown entity, positive fact, relation
  fact, negative fact and reverse relation lookup.

**Why:** T3 needs belief status to be visible, not only queryable one predicate
at a time. This gives users a compact window into the KB state created by gen17.

**Observed:** all suites green — 11 conversation, 9 persistence, 3 multigoal,
14 grammar, 2 anonymous-var, 5 explanation checks.

**Method watch (D5.1):** small reporting API, no new inference semantics. It
keeps the next pressure visible: source-aware conflict reporting and derived
belief summaries need more structure than a direct fact scan.

**Next:** continue T3 toward source-aware belief status: stop treating every
positive/negative collision as a silent overwrite once distinct sources matter.

---

## 2026-06-15 — gen17: explicit negative knowledge (T3 slice)

**Changed:** `brain.c` → `gen17-negative`; `kb.{h,c}` gained explicit negative
facts.
- `kb_assert_neg` stores known-false ground facts separately from positive
  facts. Positive and negative assertions clear their exact opposite, so the KB
  does not keep both for the same ground claim.
- Ground queries treat an explicit negative as known false; `kb_knows_pred`
  counts negative facts so a predicate can be known even when it has no positive
  members.
- Persistence now round-trips `not(pred(args)).` session clauses. A session
  negative can override a base positive fact without editing the base file.
- `bob is not a dog` is no longer just forgetting: it records `not dog(bob)` and
  later answers `No.` for the known-false claim.

**Why:** First narrow slice of TASKLIST T3. gen16 separated unknown predicates
from false answers; gen17 separates simple absence/forgetting from explicit
negative correction.

**Observed:** all suites green — 10 conversation, 9 persistence, 3 multigoal,
14 grammar, 2 anonymous-var, 5 explanation checks.

**Method watch (D5.1):** still a scaffold, but a useful one: the representation
is minimal, ground-only, and does not pretend to solve conflicts or derived
truth maintenance yet.

**Next:** continue T3 with a direct belief-status report: let the user ask what
is known about an entity and see positive and negative ground facts.

---

## 2026-06-15 — gen16: epistemic unknowns for unseen predicates (T16, part 2)

**Changed:** `brain.c` → `gen16-idk`; `kb_knows_pred` in `kb.{h,c}`.
- Query surfaces now distinguish an unknown predicate from a known-but-false
  goal. A never-mentioned class/relation answers "I don't know about <pred>.";
  once the predicate is known, unprovable members still answer "No." or
  "Nobody that I know of." as before.
- `kb_knows_pred` checks facts and rule heads; `brain.c` uses it before unary
  and binary ground/list queries.
- Tests updated deliberately for the new semantics, including retraction of the
  last fact making a predicate unknown again.
- `DESIGN.md` records this as a hardcoded scaffold, not the destination:
  epistemic policy should eventually move into reflective meta-knowledge.

**Why:** Completes TASKLIST T16 after gen15 handled not-understood input. The
agent now has two separate failure modes: "I don't understand that yet." for
parse failure, and "I don't know about <pred>." for well-formed questions whose
predicate is outside its known domain.

**Observed:** all suites green — 10 conversation, 5 persistence, 3 multigoal,
14 grammar, 2 anonymous-var, 5 explanation checks.

**Method watch (D5.1):** useful but temporary. The C check is intentionally
small and documented because the north-star is reflection + negation-as-failure
inside the KB, not permanent epistemic branching in `brain.c`.

**Next:** TASKLIST T3, first narrow slice — distinguish explicit negative
correction from simple forgetting, without building the full belief-status
architecture up front.

---

## 2026-06-15 — gen15: the parrot grows up — honest non-understanding (T16, part 1)

**Changed:** `brain.c` → `gen15-unknown`.
- The not-understood fallback is no longer the **gen0 parrot echo**. Input no
  module can parse now gets an honest "I don't understand that yet." — the
  founding parrot identity is deliberately outgrown (it was always gen0).
- Tests updated where echo was the expectation: `parrot.chat` repurposed to the
  non-understanding contract (incl. other-language/gibberish fallthrough);
  `greet.chat` and `facts.chat` no-regression lines updated.

**Why:** Raised by the user — handle what the agent does NOT understand. A chat
probe confirmed three failure modes: not-understood was *echoed* (misleading),
not-known was a confident "No."/"Nobody" (closed-world conflating unknown with
false). gen15 fixes the first (clean, low-risk). The epistemic *not-known* half
(distinguish "I don't know" from "No.") is subtler — entangled with closed-world
and retraction — and is left, with the probe findings, as TASKLIST T16 part 2.

**Observed:** all 39 checks green. Live: French/gibberish → honest admission;
known templates and proofs unaffected.

**Method watch (D5.1):** an identity decision (retiring the parrot) made under
the user's explicit interest; reversible. The harder epistemic part is
deliberately deferred rather than rushed.

**Next:** TASKLIST T16 part 2 (not-known epistemic states) and/or T3
(contradiction/belief) — both want a true/false/unknown response layer.

---

## 2026-06-15 — gen14: proof traces / explanations (T2)

**Changed:** `brain.c` → `gen14-explain`; `kb_explain` + `prove_seq_ex` in
`kb.c`.
- `prove_seq_ex`: a proof-rendering resolver. On success it renders each goal's
  proof — a fact as the ground goal, a rule as "<goal> because <sub> and
  <sub>..." — using the resolvent trick (body ++ continuation) so backtracking
  stays correct while the proof tree is reconstructed.
- `kb_explain(pred, args, argc, out)`: prove a goal and render a one-line proof.
- `mod_knowledge`: "why is <x> a/an <y>?" and "why is <x> the <rel> of <y>?";
  base facts answer "<goal> is a known fact."; unprovable → "I can't show that."
  (invents nothing).
- New `tests/explain.sh`: fact, one-step rule, rule chain, multi-goal + binding,
  and a false claim — held-out predicates so the text comes from the proof.

**Why:** TASKLIST T2. The proof tree is the **first explicit internal
representation of reasoning steps** (PRINCIPLES.md) — the agent can now show
*how* it knows, not just *what*.

**Observed:** all suites green incl. 5 explanation checks (e.g. "being(socrates)
because mortal(socrates) because man(socrates)").

**Method watch (D5.1):** healthy — a proof-returning sibling added without
touching `kb_query`/`kb_match` callers; the trace is reusable later (planning,
truth maintenance).

**Next:** study/handle the UNKNOWN and the NOT-UNDERSTOOD (new TASKLIST T16,
raised by the user) — and TASKLIST T3 (contradiction/belief). The first forces
a decision about the founding parrot-echo fallback.

---

## 2026-06-15 — gen13: real anonymous variables + bidirectional relations (T1)

**Changed:** `brain.c` → `gen13-anon`; `rename_term` in `kb.c` fixed.
- **Bug fix (introduced in gen12):** distinct `_` in one clause were renamed to
  the *same* name, aliasing them. Now each `_` becomes a FRESH variable
  (`_A<frame>_<n>`) via a per-clause anonymous counter; named vars still share
  per frame.
- `knowledge/grammar.pl`: `singular/2` facts and `has_plural(Y) :- plural(_, Y)`
  (anonymous var in subject position), complementing `countable/1`.
- Tests: grammar.sh +singular/has_plural; new `tests/anon.sh` proves two `_`
  are independent using held-out `edge/related` (anti-impostor: not morphology).

**Why:** First TASKLIST item (T1), and it was *pulled* — gen12's morphology
needed anonymous vars, and the implementation hid a real aliasing defect. Fixed
the engine correctly rather than papering over it.

**Observed:** all green — 14 grammar + 2 anon + every prior suite. The
`related(X) :- edge(X,_), edge(_,X)` case is true only because the two `_` no
longer alias (verified: would be false if aliased).

**Method watch (D5.1):** healthy — a correctness fix demanded by a real rule
shape; no speculative work.

**Next (TASKLIST T2):** proof traces / explanations — "why is socrates a
mortal?" → a chain derived from the proof tree (the first explicit internal
representation of reasoning steps).

---

## 2026-06-15 — gen12: flexible relation queries (object position)

**Changed:** `brain.c` → `gen12-relquery`; grammar domain extended.
- `mod_knowledge`: "what is the <rel> of <y>?" → `rel(y, X)` (unknown in the
  object position), complementing "who is the <rel> of <y>?" → `rel(X, y)`.
- `knowledge/grammar.pl`: morphology — `plural(dog, dogs).` (+ irregular
  `child/children`) and `countable(X) :- plural(X, _).` (anonymous variable).
- `tests/grammar.sh` extended: object-position query + the countable rule.

**Why (D5.1 — domain-pulled, not speculative):** grammar v0 itself pulled this
— "what is the plural of dog?" needs the variable in the object slot, which the
prior NL couldn't express. Built exactly that, no more. Confirms the loop: the
domain asks, the primitive answers.

**Observed:** all green — conversation + persistence + multigoal + 11 grammar
checks (incl. irregular plural and the anonymous-variable rule).

**Method watch (D5.1):** healthy — primitive justified by a concrete demand and
immediately consumed by the domain.

**Next:** grammar pulls toward sentence structure (word order / well-formedness).
First *try* it with current primitives; build new ones only if it genuinely
can't be expressed (domain-pull discipline).

---

## 2026-06-15 — expertise track: the first forged expert — grammar v0

**Changed:** no engine change (`brain_version` stays `gen11-relations`).
- `knowledge/grammar.pl`: parts of speech (`noun`/`verb`/`adjective`) + category
  rules `word(X) :- noun(X). word(X) :- verb(X). word(X) :- adjective(X).`
  (multiple clauses per head = disjunction — exercised live).
- `tests/grammar.sh`: loads the domain and proves competence (POS membership,
  derived `word/1`, negatives, `who is a word?`).

**Why (pivot duty exercised — D5.1):** the planned "richer NL parsing" primitive
was speculative (parsing before any grammar knowledge is backwards). Switched to
**domain-pull**: forge a real, tested expert slice on the existing engine. This
also surfaced an axis insight — **expertise (knowledge + tests) is orthogonal to
brain/engine generations**; a forged expert needs no version bump.

**Observed:** 7 grammar checks green, all prior tests still green. The
forge-an-expert pipeline (D5: clauses + tests + exported predicates) works end
to end. Exports `noun/verb/adjective/word` for higher layers.

**Next:** let the grammar domain pull the next need (morphology? sentence
structure? then parsing) — the demand, not a guess, picks what comes next.

---

## 2026-06-15 — gen11: n-ary relations + multi-goal rules (general SLD)

**Changed:** `brain.c` → `gen11-relations`; `kb.c` engine rewritten.
- New general term representation (constants vs variables by case), a real
  **unification** with a substitution + binding chains, and **SLD resolution
  with backtracking** and **standardize-apart** (rule vars renamed per frame).
- Rules are now `head :- goal1, goal2, ...` (multi-goal bodies). `kb_assert_rule`
  still builds the unary `head(X):-body(X)` so gen6/induction are unchanged;
  the loader (`kb_load`) parses general rules (top-level comma split), `kb_save`
  serialises them back.
- `kb_query` (any arity, facts+rules) and `kb_match` (collect variable bindings)
  both run through the new solver — old unary behaviour preserved exactly.
- `mod_knowledge`: binary surface "<x> is the <rel> of <y>" (assert / ground
  query / "who is the <rel> of <y>?").
- New `tests/cases/relations.chat` and `tests/multigoal.sh` (grandparent via a
  two-goal rule with a shared intermediate variable, incl. variable query).

**Why:** The representation jump D5 flagged as a prerequisite for the grammar
expert. North-star check (D5.1): n-ary relations + conjunctive rules are
genuinely demanded by grammar — agreement (subject↔verb), dependency
(head↔modifier), precedence, and "sentence = NP then VP" style rules are all
≥2-place relations and multi-goal. Not speculative; the family-tree test is just
the cleanest demonstration of a general engine.

**Observed:** 10 conversation + 5 persistence + 3 multi-goal checks green. The
engine rewrite preserved every prior test (the solver reduces to the old
behaviour on unary goals).

**Method watch (D5.1):** no pivot signal — this primitive is pulled by a real
grammar need and reused engine-wide.

**Next:** gen12 — richer NL parsing, so knowledge can be stated more naturally
than the fixed templates (the bridge toward the grammar expert in gen13).

---

## 2026-06-14 — gen10: retraction & correction

**Changed:** `brain.c` → `gen10-retract`; `kb_retract()` in `kb.{h,c}`.
- `kb_retract(pred, args, argc)`: removes a ground fact if present, preserving
  the order of the rest (closed-world correction — remove the positive fact,
  not store an explicit negation).
- `mod_knowledge`: "forget that <x> is a/an <y>" and "<x> is not a/an <y>" →
  retract `y(x)`; reports "Forgotten: y(x)." or "I didn't know that anyway."
- New `tests/cases/retract.chat`, incl. **rule interaction**: retracting the
  supporting fact makes the derived conclusion underivable again.

**Why:** Understanding means being wrong and fixing it. Knowledge (and the
saved session delta) can now be corrected, not only extended — the first of the
defences D5.1 lists for the brittleness of forged expertise.

**Observed:** 9 conversation + 5 persistence checks green.

**Decision deferred (provenance):** retraction removes from RAM only. Retracting
a `base` fact thus reappears on next boot (base file unchanged). Whether to
record "tombstones" for base corrections is left to a later gen — noted here so
it isn't forgotten.

**Method watch (D5.1):** gen10 is a genuine capability (correction), justified
independently — not speculative plumbing. No pivot signal yet.

**Next:** gen11 — n-ary relations + multi-goal rule bodies (e.g.
`grandparent(X,Z) :- parent(X,Y), parent(Y,Z)`): the representation jump the
grammar expert will need. Keep a concrete grammar north-star in view (D5.1).

---

## 2026-06-14 — gen9: persistent, human-readable, composable knowledge

**Changed:** `brain.c` → `gen9-persist`; persistence + provenance in
`kb.{h,c}`; knowledge loading wired into `main.c`.
- Provenance per clause (`KB_BASE`/`KB_SESSION`/`KB_INDUCED`/`KB_REFLECTIVE`),
  set via `kb_set_origin`; induced rules tagged `KB_INDUCED`; the self-model
  tagged `KB_REFLECTIVE`.
- `kb_load(path)`: parses Prolog-like text (`pred(a, b).`, `head(X):-body(X).`,
  `%` comments) and **unions** clauses into the KB — so files JOIN.
- `kb_save(path, mask)`: writes back only the selected layers, same readable
  format. `brain_save_session` persists `SESSION|INDUCED`, never reflective.
- `main.c`: loads `knowledge/base.pl` + `knowledge/session.pl` (paths via
  `PARROT0_BASE`/`PARROT0_SESSION`; empty disables — used for hermetic tests);
  `/save` command writes the session delta.
- `knowledge/base.pl` seed added; `session.pl` gitignored. New
  `tests/persist.sh` (round-trip, no-reflective-leak, base+session join, no
  base duplication); `tests/run.sh` made hermetic.

**Why:** DESIGN.md D1-D3. Knowledge becomes a durable, transparent artifact:
readable, hand-editable, diffable in git, and composable (base + discovered).
New knowledge from conversation is saved in the *same* format and can be
promoted session→base by moving lines.

**Observed:** 8 conversation + 5 persistence checks green. A fresh parrot0 now
boots already knowing its base, and facts taught in one run survive into the
next.

**Next:** gen10 — retraction & correction: "forget that X is a Y" / "X is not a
Y", so knowledge (and the saved delta) can be corrected, not only grown.

---

## 2026-06-14 — gen8: identity & self-reflection ("I know that I am")

**Changed:** `brain.c` → `gen8-self`; new `mod_self`.
- At birth `brain_create` writes the agent into its own KB: `i_am(parrot0).`
  and one `module(<name>)` per **registered** module (reified from the registry
  — self-description can't drift from real structure). These are `reflective`
  facts: regenerated every boot, not meant to persist (DESIGN.md D3).
- `mod_self` answers introspection by *querying that self-model*: "who/what are
  you?" → `i_am(X)`; "do you exist?" → resolves `i_am`; "what can you do?" →
  lists `module(X)`.
- New `tests/cases/self.chat`, incl. the reflexive payoff: "who is a module?"
  resolves over the self-model with the **same** engine as world knowledge.

**Why:** The identity principle (PRINCIPLES.md). The reflexive closure of the
method: the agent gets a model of itself, in the same substrate it uses for the
world, derived from real state (anti-impostor — no `printf "I am conscious"`).

**Observed:** `make test` green (8 cases). Live: "who is a module?" →
memory, self, knowledge, greet, farewell — the agent reading its own structure.
Honest scope: this is the structural *precondition* for self-knowledge; no
claim about felt experience.

**Next:** gen9 — persistence: human-readable knowledge files, base+session
joined at load, save only the session delta, provenance per clause (DESIGN.md
D1–D3).

---

## 2026-06-14 — gen7: induction — learning rules from facts ("training")

**Changed:** `brain.c` → `gen7-induce`; `kb_induce()` in `kb.{h,c}`.
- `kb_induce(min_support)`: ILP-lite. For unary predicates P, Q (P≠Q), if every
  constant with `P(c)` also has `Q(c)` and there are ≥ `min_support` such
  constants, it induces and asserts `Q(X) :- P(X)`. Deterministic order;
  already-known rules are skipped. Helpers `fact_present`, `rule_exists`.
- `mod_knowledge`: trigger words `"generalize"` / `"learn"` run induction and
  report the rules learned (or "Nothing new to generalize.").
- New `tests/cases/induce.chat`: induces `mortal(X):-man(X)` (≥2 examples, no
  counterexample), rejects the reverse (a mortal who isn't a man) and the
  single-example `dog`, then shows the learned rule generalizing to a new
  individual.

**Why:** This is the experiment's deterministic, legible analogue of "training"
(PRINCIPLES.md). The agent now *creates structure* (rules) from data, rather
than being handed it. Rules induced this way generalize to unseen individuals
via gen6 resolution — the anti-impostor property made concrete.

**Observed:** `make test` green (7 cases). A live note worth keeping: with
*symmetric* evidence (every man is mortal AND every mortal is a man among the
known individuals), induction learns BOTH directions — correct given the data,
but a vivid demonstration of **over-generalization from few examples**, the
core ILP/impostor risk. Counterexamples (gen-?) and confidence are the cure.

**Next:** gen8 — identity & self-reflection: reify the agent into its own KB
(`i_am(parrot0).`, `module(...)`) and answer introspective queries from real
state. See PRINCIPLES.md, "I know that I am".

---

## 2026-06-14 — gen6: rules + backward-chaining resolution

**Changed:** `brain.c` → `gen6-rules`; rules + resolution in `kb.{h,c}`.
- `kb_assert_rule(head, body)`: stores definite rules `head(X) :- body(X)`.
- `prove1()`: backward chaining over facts + rules with a depth guard
  (`KB_MAX_DEPTH`) against cyclic rules. `kb_query` now resolves unary goals.
- `kb_match` for unary variable queries now resolves over rules too (walks
  known constants, keeps the provable ones, distinct + in first-seen order).
- `mod_knowledge`: new surface `"every <y> is a/an <z>"` → rule `z(X):-y(X)`.
- New `tests/cases/rules.chat`: derivation, transitive chaining, and
  **generalization to a never-asserted subject** through a rule.

**Why:** This is where parrot0 stops *storing* answers and starts *deriving*
them — the anti-impostor move from PRINCIPLES.md. "Is plato a mortal?" is true
without ever being told, because `mortal(X):-man(X)` + `man(plato)`.

**Observed:** `make test` green (6 cases). Transitive chains and new-subject
generalization both work.

**Next:** gen7 — induction: learn the rules themselves from the facts ("the
training").

---

## 2026-06-14 — gen5: variables + unification

**Changed:** `brain.c` → `gen5-unify`; new `kb_match()` in `kb.{h,c}`.
- `kb_match()`: a pattern where any arg can be a variable (signalled by NULL);
  returns the binding of the first variable for every unifying fact, in
  insertion order.
- `mod_knowledge` rewritten/reordered: `"who/what is a <y>?"` → `y(X)` and
  lists the matches; ground query and assert kept, ordering fixed so a
  question word is never mistaken for a subject.
- New `tests/cases/unify.chat`.

**Why:** Unification is the load-bearing primitive for rules (gen6) and
induction (gen7) — built minimal but correct, with deterministic ordering.

**Observed:** `make test` green (5 cases). "who is a man?" → all men, in order.

**Next:** gen6 — rules + backward-chaining resolution.

---

## 2026-06-14 — gen4: the knowledge base spine begins (ground facts)

**Changed:** `brain.c` → `gen4-facts`; new sub-system `src/kb.{h,c}`.
- `kb.{h,c}`: a ground fact store — `kb_assert` / `kb_query` over atoms like
  `man(socrates)`, closed-world (absent => false), idempotent, growable.
- New `mod_knowledge` (a module that fronts the KB — the first **fractal
  split**: a part with its own sub-system): translates NL ⇄ facts:
  `"X is a/an Y"` → assert `y(x)`; `"is X a/an Y?"` → query `y(x)`.
- `Brain` now owns a `KB *`; create/destroy manage its lifecycle.
- New `tests/cases/facts.chat` locks assert/query, "an", no cross-leak between
  predicates/subjects, and no regression on non-knowledge turns.

**Why:** This is the substrate for the Prolog-like direction (PRINCIPLES.md +
the gen4–gen7 ladder). gen3's name memory was a 1-fact KB; gen4 generalises it
into a real store with closed-world querying. Representation (pred + ground
args) is chosen to admit variables (gen5), rules (gen6), induction (gen7)
without a rewrite.

**Observed:** `make test` green (4 cases). The fractal split happened
*because the domain demanded it* — knowledge earned its own file.

**Next:** See `TASK.md` — gen5: variables + unification, so queries like
"who is a man?" resolve against the facts.

---

## 2026-06-14 — gen3: the first stateful part (session memory)

**Changed:** `brain.c` → `gen3-memory`.
- `Brain` now carries `name` + `has_name` across turns.
- New `mod_memory` (registered first): learns "my name is <X>" (preserving
  original case), recalls on "what is my name?" and phrasing variants, handles
  the not-yet-known case, and supports updating the name.
- Added `copy_trim()` / `skip_ws()` helpers; included `<stdio.h>` for snprintf.
- New `tests/cases/memory.chat` locks it in, including recall *across* an
  intervening greeting (real cross-turn context).

**Why:** Up to gen2 the brain was purely reactive — each turn answered in
isolation. Reasoning needs context that persists. This is the smallest honest
step into statefulness, and it exercises a module reading/writing `Brain`.

**Observed:** `make test` green (3 cases). Greeting between teach and recall
does not disturb memory — modules compose cleanly through the registry.

**Next:** See `TASK.md` — gen4: let a module *decompose* into sub-parts
(fractal articulation), or add light reflection/transformation. One step.

---

## 2026-06-14 — gen2: the brain becomes a system of parts

**Changed:** `brain.c` → `gen2-modular`.
- Introduced a **module protocol**: `Handler` = a part that, given normalized
  input, either claims the turn (writes a response, returns 1) or declines (0).
- Added an ordered **registry** of modules; `brain_respond()` walks it and the
  first claimer wins; otherwise the gen0 parrot echo fires.
- Re-expressed gen1's greeting & farewell logic as two modules
  (`mod_greet`, `mod_farewell`). Behaviour identical, now articulated.

**Why:** PRINCIPLES.md's corollary — structure is functional to emergence. The
brain must be able to become a system of cooperating parts. This is the
*capacity* to articulate; no module taxonomy is pre-designed.

**Observed:** `make test` green with the existing cases unchanged — proof the
refactor preserved behaviour exactly. Adding/removing a behaviour now touches
only the registry table, not the dispatch flow.

**Next:** See `TASK.md` — gen3: the first *stateful* part (session memory),
which also exercises a module reading/writing `Brain` state.

---

## 2026-06-14 — gen1: first intent (greetings & farewells)

**Changed:** `brain.c` → `gen1-greet`.
- Added `normalize()` (lowercase + trim) and `matches_any()` so intent
  matching ignores case and stray spaces.
- Greetings (`hello`/`hi`/`hey`) → "Hi there!", farewells (`bye`/`goodbye`)
  → "Goodbye!". Everything else still falls back to the gen0 parrot echo.
- New `tests/cases/greet.chat` locks in the behaviour (incl. case-insensitive
  + spaces + no-regression echo). Updated `parrot.chat` to test the echo
  *fallback* with non-intent inputs, since "hello" is now an intent.

**Why:** Smallest honest step off pure echo, and a full end-to-end lap of the
loop (TASK → brain.c → tests → journal) to validate the mechanism before we
make the brain articulable.

**Observed:** `make test` green (2 cases); `make chat` greets and bids
farewell, parrots the rest.

**Next:** See `TASK.md` — give the brain the *capacity to articulate* into
cooperating sub-systems (a module registry), per PRINCIPLES.md's corollary.
This is infrastructure for emergence, not a design of the modules themselves.

---

## 2026-06-14 — gen0: bootstrap, the parrot

**Changed:** Created the experiment scaffold.
- `src/main.c`: stable I/O shell — line-based REPL, one stdout response per
  input line, decorative prompts on stderr, `/quit` to exit.
- `src/brain.c` + `brain.h`: the evolving "brain". gen0 (`gen0-parrot`) just
  echoes input verbatim — the most honest zero-agent.
- `Makefile`: `make`, `make chat`, `make test`, `make loop`, `make clean`.
- `tests/run.sh` + `tests/cases/parrot.chat`: conversation test harness using
  a `>` input / `<` expected-response line protocol.
- `LOOP.md`: the self-improvement protocol.

**Why:** Establish the smallest thing that runs, can be talked to (`make
chat`), and can be tested — so every later generation is a small, verified
step on top.

**Observed:** Echo works; tests green.

**Next:** See `TASK.md` — gen1: recognise greetings/farewells instead of
parroting them.