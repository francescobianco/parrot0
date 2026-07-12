# The parrot0 self-improvement loop

parrot0 is an experiment: a conversational agent in pure C that grows, one
small generation at a time, driven by an AI agent running this loop. There is
no language model inside parrot0 at runtime — only algorithms that *emerged*
from iterating this loop.

> Read **[PRINCIPLES.md](PRINCIPLES.md)** first — it is the *why*. This file is
> only the *how*. Every iteration is an attempt to re-express, in deterministic
> C and verified against real conversation, the structure the model already
> runs internally.

## The contract

- `src/main.c` is the **stable I/O shell**. Avoid changing it.
- `src/brain.c` (+ `brain.h`) is the **part that evolves**. All intelligence
  lives here, as plain, deterministic C.
- The brain must keep `brain_respond()`'s signature so the harness always
  compiles and tests.

## Unit of work: experiment != generation

- A **Tick** is one cheap hypothesis and its first directional verdict. It may be
  discarded; it is not a generation and does not require a commit or full suite.
- A **Turn** selects whether a locally-green candidate deserves transfer tests.
- A **generation** is one promoted champion, after the expensive Release gate.
- Many Ticks may compete inside one generation. Only the winner pays complete
  regression, determinism replay and the full adversarial safety campaign.
- Containment, resource limits, test/oracle tamper protection and workspace
  isolation apply to **every** executed candidate; they are not deferred.

This distinction preserves atomic generations while removing the old mistake
of treating every edit as if it were already a release candidate.

## One generation

1. **Select/read one contract.** `TASK.md` holds the current goal. If it is empty
   or done, promote exactly one next item from `TASKLIST.md`. Seal a minimal red
   contract/counterexample and name its owner and mechanical oracle. Do not
   substitute unrelated housekeeping unless the user asked for it.
2. **Observe focally.** Prefer `make check TEST=<id>` once W0b provides it; until
   then drive the exact fixture/script directly instead of using the whole suite
   as a probe. Use `make chat` only when live conversation is the contract.
   Record the first broken semantic layer, not merely the final wrong sentence.
3. **Hypothesize.** Produce one small idea, or several explicit alternatives
   when comparison is informative. Rank the next probe by information gained
   per second.
4. **Self-challenge parity once per candidate family.** Restate the task to
   parrot0 and compare its proposal with the external hypothesis. Do **not** let
   parrot0 edit files, choose the task, run tests, commit or push. Repeating the
   same self-challenge after every mechanical edit adds latency but no evidence.
5. **Implement an experiment.** Work in an overlay/worktree where possible.
   Use the dev build once W0b provides separate build trees; until then use the
   current build and the exact focal test. Keep the change reversible. A failed
   experiment is deleted or archived as a counterexample; it does not consume a
   generation.
6. **Encode the expectation at the lowest truthful seam.** Prefer a structured
   status/winner/proof/action assertion once that seam exists; until then use the
   smallest honest golden or direct oracle. Exact text belongs to rendering or
   public CLI contracts. Add the equivalent Italian case when the behaviour is
   linguistic, through the same code path; never duplicate logic to pass it.
7. **Tick — Spark + Focal.** Run static/build checks, the exact red test and a
   few collision negatives. Stop at the first counterexample. Iterate steps
   3-7 rapidly; do not run `make test` after each edit.
8. **Turn — Impact + stress.** Only locally-green candidates run the hot impact
   slice and the roughly **10x more complex** trial. Mutation, EN/IT and visible
   variants choose one champion. Only after that choice is frozen may the
   champion see the promotion holdout. If it fails, that case becomes train and
   a new holdout is sealed before the next attempt. Reject or keep in shadow;
   never hide timeout, flaky harness or infra error as a functional result.
9. **Freeze one champion and use the promotion mode actually available.** The
   Impact selector never replaces full promotion regression.

   - **Legacy transition (today):** run exactly one `make gate` on the finalized
     working tree (`gate` already includes `make test`). If green, this permits a
     legacy generation, but does **not** prove Forge attestation, exact-artifact
     identity, atomic rollback or generalized oracle-integrity; do not claim
     those properties before their milestones land.
   - **Forge promotion (after W0b/W1/W2 provide it):** finalize source, tests,
     KB, manifest/policy and the pre-run decision record; create a candidate
     commit on a temporary ref; checkout/build that exact commit; run promotion
     holdout, the full offline ratchet, oracle-integrity, required safety,
     replay and rollback. The trusted controller writes evidence outside the
     candidate worktree, keyed by candidate commit and release-binary digest.
10. **Record without changing a Forge candidate.** In legacy mode append the
    dated journal entry after the green gate and state explicitly that evidence
    is legacy. In Forge mode the tracked pre-run record is already in the
    candidate commit; the final verdict lives in the external attestation or an
    annotated ref/tag that does not change that commit. A red candidate is not
    activated.
11. **Promote exactly what was tested.** Legacy mode uses `convcommit` once after
    the gate and accepts that the resulting commit was not exact-artifact Forge
    evidence. Forge mode fast-forwards/activates and pushes the **same candidate
    commit** whose binary passed Release; no post-gate edit or relink is allowed.
    Never batch multiple promoted contracts into one generation.
12. **Anneal.** Nightly/periodic full, fresh-exec differential and field tests
    may downgrade or rollback the package. Their result becomes the next
    minimized hot counterexample, not a surprise rediscovered at the end of a
    later generation.

## Principles

- **Small, reversible steps.** Each generation should be explainable in a
  sentence.
- **Tests are the ratchet.** Behaviour that earns a test never silently
  regresses.
- **Feedback is progressive.** Cheap tests direct exploration; the full ratchet
  certifies the survivor. Neither substitutes for the other.
- **Latency is a constraint.** If Focal or Impact exceeds its budget, improve
  the test seam before adding more synchronous black-box cases.
- **Emergence over design.** Don't hand-build a grand architecture up front;
  let structure appear because the tasks demanded it.
- **Stay pure C.** No runtime model, no network. Complexity is allowed; magic
  is not.

## Acceleration — make generations compound, not just add

Growth so far is **additive**: generation *n* ⇒ ~*n* capabilities, constant
cost per brick, no interest compounding. Faithful acceleration (PRINCIPLES.md)
comes only from **structure that compounds**, never from writing phrasebook/C
faster (that is the impostor, and it stays linear). The convergence is meant to
be asymptotic, so the real target is **maximize edges-closed-per-generation**
inside the anti-impostor discipline. Three levers, in order of leverage:

- **A. Migrate capability from C branches to KB facts/rules over a fixed
  engine.** *Uniform substrate, articulated function* (PRINCIPLES corollary).
  Every closed-class lexicon or world fact moved out of hardcoded C arrays into
  `kb/core/*.p0` (queried at runtime) turns "write C" (human-bottlenecked,
  linear) into "ingest knowledge" (automatable, cumulative). Precedents:
  `stopword`/`social`/`gloss`/`roles`, gen111 `weight()`, gen133 rules, gen104
  ICL, the dynamic-knowledge loop, and **gen193 `conjunction/1`** — a lexical
  class read from the KB and *teachable at runtime* ("use p as a conjunction"
  asserts `conjunction(p)`; the same parsers split on it with no code change).
  **Default move: when a new capability is a lexical class or a fact set, add it
  as KB and have the fixed engine query it — do not grow a new C array.**
- **B. Prefer keystones; measure edges-closed-per-generation.** The brain must
  articulate into cooperating parts (compose-bench). A general primitive
  multiplies (gen133 closed three open edges with one change); a special case
  adds one. When the generalization probe (the IT ratchet, the 10x stress)
  fails, **fix the core, never duplicate** — that is what makes future cases
  free. Prefer the change that unlocks combinations over the change that covers
  one more phrasing.
- **C. Reflexive dogfooding (the super-linear, long game).** The code-mastery
  faculty (AST-as-KB → edit → verify-by-build, gen187–192) is an agent for
  editing code. Aimed eventually at `brain.c` itself, it makes the loop's
  *implement + test* step partly self-driving — the method becoming an engine,
  the reflexive closure PRINCIPLES puts at the centre. This is the real meaning
  of the `swe-bench` north star. Lever A applies here at the largest grain
  (CODE-MASTERY.md §7b, "language-as-delta"): don't *learn* each language — model
  the shared computational substrate once and declare languages/types as **mirror
  concepts (deltas by reference)**, letting inference cover the shared part and the
  oracle check the rest. C is a fine metalanguage; the imperative is that the
  inference must NOT speak C.

Meta-lever: let the discovery harnesses (`basic-chat-bench`, `code-bench`) not
only **measure** gaps but **propose the next pull by leverage** — "the tests
dispose" extended to "the harness proposes the order", cutting the human
task-selection bottleneck.
