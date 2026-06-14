# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen22 — minimal discourse coreference
## (SuperGLUE-like driver: linguistic foundations)

The benchmark-driver note in DESIGN D8 says SuperGLUE-like pressure should test
language understanding, context and coreference. parrot0 currently treats
pronouns as literal constants: after `dana is a pilot`, `she is a teacher` would
store `teacher(she)` instead of resolving `she -> dana`.

### Design question
What is the smallest useful coreference mechanism that improves short factual
discourse without pretending to solve full natural-language reference?

Candidate: keep the most recent concrete entity mentioned in the knowledge
surface. Resolve `he/she/it/they/him/her/them` to that entity for unary factual
assertions, unary ground queries and direct belief reports. If no antecedent is
known, answer that the referent is unknown.

### Acceptance
- `dana is a pilot` then `she is a teacher` stores `teacher(dana)`.
- `is she a pilot?` resolves to `dana` and answers `Yes.`.
- `what do you know about her?` reports facts about `dana`.
- A pronoun before any antecedent is rejected honestly.
- Keep the scope explicit: no gender/number semantics, no multi-candidate
  resolution, no cross-module memory yet.

### Notes
- This is a SuperGLUE-like foundation slice, not a full benchmark implementation.
- Prefer one deterministic discourse pointer over a speculative reference model.
