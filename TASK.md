# Current task

> One goal at a time. When it's done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions.

## Goal: gen10 — retraction & correction

So far knowledge only grows. Real understanding also means being *wrong and
fixing it*. Let parrot0 retract knowledge, so the KB (and the saved session
delta) can be corrected, not only extended.

### Idea
- `kb_retract(pred, args, argc)`: remove a ground fact if present (returns
  whether something was removed). Optionally `kb_retract_rule(head, body)`.
- NL surface in `mod_knowledge`:
  - "forget that <x> is a <y>" → retract `y(x)`.
  - "<x> is not a <y>" → retract `y(x)` (and confirm).
- After retraction, queries reflect the change immediately, and `/save` writes
  the corrected delta.

### Acceptance
- Teaching then retracting a fact makes the query go back to "No.".
- Retraction is reported clearly; retracting an unknown fact says so gracefully.
- Retraction interacts sanely with rules (retracting `man(socrates)` makes
  `mortal(socrates)` underivable if it depended only on that fact).
- All existing tests still pass; new `tests/cases/*` (or persist check) covers
  retract + corrected save.
- Bump `brain_version()` to `gen10-...`.

### Notes
- Keep closed-world semantics: "X is not a Y" means *remove the positive
  fact*, not store an explicit negation (negation is a larger, later step).
- Watch provenance: retracting a `base` fact in a session — does it persist?
  For now, retraction removes from RAM; decide later whether to record
  "tombstones" for base corrections. Note the decision in DESIGN.md if taken.
