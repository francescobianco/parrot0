# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen35 — numeric arithmetic / divisibility (BoolQ #6 road)

Domain-pull continues. BoolQ #6 is "can an odd number be divided by an even
number" (gold: yes) — a question about *numbers and arithmetic*, which parrot0
still cannot do: gen27/gen28 only *compared* magnitudes, never computed with
them. This also directly revisits Decision D-2026-06-15a (numbers are inert
string atoms).

### Design question
What is the smallest arithmetic primitive that lets parrot0 actually compute on
numbers — not just order them — without turning into a calculator language?

Candidate: `what is <a> plus/minus/times <b>?` → the computed value; and
`is <a> divisible by <b>?` → yes/no via integer remainder. Pure literal numbers
first (like gen27's `mod_compare`), parsed with the existing `parse_num`. Keep
the operator set tiny and the surface fixed.

### Acceptance
- `what is <a> plus <b>?` (and minus/times) returns the correct number.
- `is <a> divisible by <b>?` answers yes/no by remainder; division by zero is
  refused, not crashed.
- Non-numbers are declined and fall through honestly (as gen27 does).
- Held-out values prove it computes, not memorizes.

### Notes
- Discovery tooling stays (`PARROT0_TRACE` + `--max-examples 1`).
- If integer-vs-float formatting forces a choice (e.g. `2 plus 2` → `4` vs
  `4.0`), make it and log it in JOURNAL "Decisions".
- Do not hardcode benchmark answers.
