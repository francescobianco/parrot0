# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen28 — drive comparison from text (question→query + tiny extraction)

gen27 was discovered by domain-pull: capturing the first official BoolQ
question (`PARROT0_TRACE`) showed that answering it needs a *magnitude
comparison*, which now exists as `mod_compare` (`is <a> more/less than <b>?`).
But the benchmark still scores 0, correctly — parrot0 cannot yet turn the
passage's prose into the two numbers, nor map the question to a comparison.
That bridge is the next investment this loop surfaced.

### Design question
What is the smallest honest surface that lets the comparison primitive be
*driven from language* instead of from pre-extracted numbers — without faking
open-domain extraction?

Candidate: a quantity surface such as `<x> has <n> <unit>` asserting a numeric
fact, plus a comparative question `does <x> have more <unit> than <y>?` that
looks the two quantities up and reuses `mod_compare`'s magnitude test. Keep it
to explicitly stated quantities; full passage parsing stays out of scope.

### Acceptance
- A numeric quantity can be asserted and recalled as a fact.
- `does <x> have more <unit> than <y>?` answers yes/no by comparing the two
  asserted quantities (reusing the gen27 magnitude test, not a new one).
- Missing quantities are admitted, never guessed.
- Held-out units/entities prove the behaviour is value-based, not canned.

### Notes
- Discovery tooling stays: `PARROT0_TRACE=<file>` (opt-in input capture) +
  `make bench-superglue ... --max-examples 1`. Use it to keep choosing the next
  feature from real benchmark pressure, not a priori.
- The hard remaining gap (prose → quantity facts) is explicitly deferred; do
  not hardcode benchmark answers (PRINCIPLES: a printf impostor is rejected).
