# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen19 — source-aware conflict sketch
## (TASKLIST T3, next slice)

gen17 stores explicit negatives by clearing the matching positive, and gen18 can
report direct beliefs. That is enough for correction, but it cannot yet represent
source disagreement: two sources that assert opposite claims collapse into the
last-loaded state.

### Design question
What is the smallest representation that can preserve a real conflict without
breaking the useful correction behavior from gen17?

Candidate: keep exact positive/negative overwrite for same-session user
correction, but introduce enough provenance/status metadata to detect when a
base claim and a session claim disagree. The first user-facing payoff can be a
belief report that says a claim is conflicted instead of silently choosing one.

### Acceptance
- If base says `man(socrates).` and session says `not(man(socrates)).`, a direct
  report for socrates exposes the disagreement instead of only showing one side.
- Same-session correction remains simple: `bob is a dog` then `bob is not a dog`
  should still settle to the explicit negative unless a distinct source boundary
  is present.
- Existing query behavior remains deterministic while conflict query semantics
  are designed explicitly.
- Tests cover held-out predicates and both base/session and same-session cases.

### Notes
- Do not rush full truth maintenance. This generation is about preserving and
  exposing source disagreement, not solving every derived conflict.
