# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen18 — direct belief report for an entity
## (TASKLIST T3, second narrow slice)

gen17 can store positive and explicit negative ground facts, but the user still
has to query one predicate at a time. Add a small report surface so parrot0 can
show what it directly believes about an entity.

### Design question
What is the smallest useful answer to "what do you know about X?" that exposes
belief status without building full conflict/source reporting yet?

Candidate: scan direct ground facts. Report positive unary facts as `X is a Y`,
positive binary facts involving X as `pred(X, Y)` / `pred(Y, X)`, and explicit
negative unary facts as `X is not a Y`. Keep it direct-only for now; derived
facts and proof-backed summaries come later.

### Acceptance
- After `socrates is a man`, "what do you know about socrates?" includes
  `socrates is a man.`.
- After `bob is not a dog`, "what do you know about bob?" includes
  `bob is not a dog.`.
- If nothing direct is known about the entity, answer honestly without inventing
  derived facts.
- Tests use held-out names and predicates; no string-specific shortcut.

### Notes
- This is a reporting surface over existing KB state, not a new inference layer.
- Keep output deterministic and compact so it can become a stable test ratchet.
