# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen24 — explainable textual entailment
## (SuperGLUE-like driver + T2 proof trace reuse)

gen23 can label a constrained premise/hypothesis pair, but an entailed answer is
opaque. Since gen14 already built proof traces, reuse that machinery inside the
entailment micro-driver.

### Design question
What is the smallest syntax that asks for an entailment proof without widening
the parser?

Candidate: support `explain premise: ...; hypothesis: ...`. It uses the same
temporary KB as gen23, but if the hypothesis is entailed it returns the proof
trace. Non-entailed statuses stay compact and honest.

### Acceptance
- A fact entailment explains as a known fact.
- A rule+fact entailment explains the proof chain.
- Contradicted/conflicted/unknown hypotheses keep their status and do not invent
  a proof.
- Existing gen23 label-only entailment remains unchanged.

### Notes
- This is still a micro-driver. Do not add broad text parsing; reuse the exact
  gen23 hypothesis grammar.
