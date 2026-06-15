# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen31 — coreference decision (WSC road)

Domain-pull continues. The first SuperGLUE WSC question asks whether two spans
("anyone" / "him") refer to the same entity — a yes/no *coreference decision*.
parrot0 has a discourse model (gen22: pronouns resolve to the most recent
concrete entity) but cannot yet be *asked* whether two mentions corefer.

### Design question
What is the smallest surface that turns the existing salience/coreference state
into an explicit yes/no judgement between two mentions, without a full anaphora
resolver?

Candidate: `does <a> refer to <b>?` answered from coreference state — yes when
`a` is a pronoun whose resolved antecedent is `b` (or when `a` and `b` are the
same entity), no otherwise; admit when there is nothing to resolve. Stay with
the last-entity discourse model already in place; full WSC-style syntactic
binding is out of scope.

### Acceptance
- After a mention sets the discourse entity, `does <pronoun> refer to <x>?`
  answers yes/no correctly.
- A pronoun with no antecedent is admitted, not guessed.
- A non-matching antecedent answers no.
- Held-out entities prove it is state-based, not canned.

### Notes
- Discovery tooling stays (`PARROT0_TRACE` + `--max-examples 1`).
- Do not hardcode benchmark answers; the prose→mentions bridge is deferred like
  the other extraction gaps. See JOURNAL "Decisions".
