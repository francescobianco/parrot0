# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen25 — MMLU-like multiple-choice class questions
## (MMLU-like driver: knowledge retrieval + domain transfer)

gen23/gen24 added SuperGLUE-like short-text inference. Start applying the MMLU
benchmark driver with a tiny multiple-choice interface over KB knowledge.

### Design question
What is the smallest multiple-choice task that pressures knowledge retrieval and
reasoning without importing an external dataset?

Candidate: support `which is a <class>: a, b, c?`. For each choice, query the KB
for `<class>(choice)`, including rule-derived membership. Return the matching
choice(s), or an honest unknown/no-match status.

### Acceptance
- With direct facts, the correct choice is selected.
- With a rule plus fact, a derived correct choice is selected.
- If multiple choices satisfy the class, list all matching choices.
- If the class predicate is unknown, return `I don't know about <class>.`.
- Tests use held-out names/classes so the interface is structural.

### Notes
- This is an MMLU-like micro-driver, not a real MMLU benchmark run.
- Keep choices single-token for now; the point is retrieval/reasoning over the
  KB, not parsing arbitrary answer text.
