# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen30 — cause/effect reasoning primitive (COPA road)

Domain-pull continues. The captured first COPA question ("The man turned on the
faucet. effect: 1 toilet filled / 2 water flowed from the spout") needs
*causal* inference — picking the plausible cause or effect — which parrot0 has
never had. Unlike gen29 (a remap of existing entailment), this is a genuinely
new relation.

### Design question
What is the smallest causal primitive that lets parrot0 answer "what is the
effect of <x>?" / "what is the cause of <x>?" and choose between two candidates,
without a commonsense model of the world?

Candidate: a directed fact `causes(a, b)` (a causes b); surfaces `<a> causes
<b>` (assert), `what is the effect of <a>?` → b, `what is the cause of <b>?` →
a, and a COPA-shaped chooser `effect of <a>: <c1> or <c2>?` → the candidate that
is a known effect. Keep it to stated causal facts; commonsense causality (the
real COPA difficulty) stays out of scope.

### Acceptance
- A causal link can be asserted and queried in both directions.
- The chooser picks the candidate that is the known effect (or cause), and
  says so when neither/both qualify.
- Unknown causes/effects are admitted, never guessed.
- Held-out predicates prove it is relation-based, not canned.

### Notes
- Discovery tooling stays (`PARROT0_TRACE` + `--max-examples 1`).
- Do not hardcode benchmark answers; the prose→`causes(a,b)` bridge is deferred
  exactly like the other extraction gaps. See JOURNAL "Decisions".
