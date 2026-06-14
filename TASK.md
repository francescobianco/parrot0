# Current task

> One goal at a time. When it's done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen17 — explicit negative knowledge vs forgetting
## (TASKLIST T3, first narrow slice)

gen16 treats a predicate with no remaining facts/rules as unknown. That is fine
for "forget that X is a Y", but it makes an explicit correction ("X is not a Y")
look identical to simple absence. Start T3 by separating those two cases.

### Design question
What is the smallest belief-status layer that can represent **known false**
without committing to the full future contradiction/conflict architecture?

Candidate: store explicit negative ground facts in a separate KB layer or API,
query them before positive proof, and keep them visible enough that
`kb_knows_pred(pred)` still regards the predicate as known.

### Acceptance
- After `bob is not a dog`, "is bob a dog?" returns `No.` because the negative
  correction is known, not because `dog` is unknown.
- `forget that bob is a dog` still removes only the positive fact; if no
  positive or negative knowledge remains about `dog`, "is bob a dog?" returns
  "I don't know about dog.".
- A positive assertion should clear the matching negative assertion, and a
  negative assertion should clear the matching positive assertion.
- Persistence remains honest: saved session knowledge preserves explicit
  corrections enough that reload answers the same way.
- Tests cover held-out predicates; no string-specific shortcut.

### Notes
- Do not build full source disagreement or derived-belief truth maintenance
  yet. This generation only earns explicit known-false ground facts.
