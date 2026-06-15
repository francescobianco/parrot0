# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen33 — equivalence / sameness relation (BoolQ #1 road)

Back to domain-pull on a fresh captured question. BoolQ #1 is "is house tax and
property tax are same" (gold: yes) — a question about whether two things are the
*same*. parrot0 has no notion of equivalence between entities.

### Design question
What is the smallest symmetric equivalence primitive that lets parrot0 assert
and answer "are <x> and <y> the same?" without conflating it with class
membership?

Candidate: a symmetric relation `same(a, b)`; surface `<x> is the same as <y>`
(assert, stored both ways or queried symmetrically), and `are <x> and <y> the
same?` → yes/no. Keep it to stated equivalences; deciding sameness from prose
(the real BoolQ difficulty) stays out of scope but is now reachable via `read:`.

### Acceptance
- An equivalence can be asserted and queried symmetrically (order-independent).
- `are <x> and <y> the same?` answers yes/no; identical names are trivially yes.
- Unknown / unrelated pairs answer no, never guess.
- Held-out entities prove it is relation-based, not canned.

### Notes
- Discovery tooling stays (`PARROT0_TRACE` + `--max-examples 1`).
- Decide and log whether `same` is transitive (a=b, b=c ⇒ a=c). See JOURNAL
  "Decisions". Do not hardcode benchmark answers.
