# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen23 — tiny textual entailment surface
## (SuperGLUE-like driver: short-text inference)

gen22 added a minimal discourse pointer for pronoun coreference. The next
SuperGLUE-like pressure is short-text entailment: given a compact premise and a
hypothesis, parrot0 should decide whether the hypothesis follows from the KB
state it can build from the premise.

### Design question
What is the smallest entailment interface that reuses the existing KB instead of
creating a separate benchmark-only path?

Candidate: support a constrained single-turn form such as
`premise: <fact or rule>; hypothesis: <query>`. Parse the premise through the
same knowledge machinery, then answer whether the hypothesis is proven,
conflicted, false or unknown using existing query statuses.

### Acceptance
- A premise fact plus matching hypothesis returns entailed.
- A premise rule plus fact can entail a derived hypothesis.
- A negative/conflicted hypothesis reports the appropriate status, not just
  false.
- Held-out names/predicates prove the path is structural, not string-specific.

### Notes
- This is a SuperGLUE-like micro-driver, not an implementation of the external
  benchmark dataset.
- Keep the grammar tiny and explicit; do not build a general text parser yet.
