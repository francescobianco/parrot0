# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen26 — BBH-like multi-step reasoning probe
## (BIG-Bench Hard-like driver: composed inference)

gen23/gen24 covered short-text entailment and gen25 started MMLU-like
multiple-choice retrieval. The next benchmark driver is BBH-like: tasks that
require composed inference rather than direct memory lookup.

### Design question
What is the smallest probe that distinguishes direct facts from multi-step
reasoning already available in the KB solver?

Candidate: add a compact surface such as `how do you know <query>?` or extend
entailment explanations to report proof depth. The test should require at least
a two-rule chain and should fail if parrot0 only checks direct facts.

### Acceptance
- A two-step rule chain is recognized and reported as multi-step reasoning.
- A direct fact is reported as direct, not multi-step.
- A false claim does not invent a chain.
- Held-out predicates prove the behavior is proof-structure based.

### Notes
- This is a BBH-like micro-driver, not a real BIG-Bench Hard task import.
- Prefer reusing `kb_explain`/proof traces before adding new solver machinery.
