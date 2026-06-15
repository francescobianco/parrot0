# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen34 — conjunctive membership ("are <x> and <y> both a <z>?")

Domain-pull continues toward multi-fact reasoning. MultiRC-style questions need
combining several facts in one judgement; the smallest such step parrot0 lacks
is a conjunctive yes/no over two subjects (or two classes). Today every query
asks about a single ground goal.

### Design question
What is the smallest conjunctive query that reuses the existing resolver
(`kb_query`, which already chains rules) for each conjunct, so AND-composition
emerges without new solver machinery?

Candidate: `are <x> and <y> both a/an <z>?` → yes iff `z(x)` AND `z(y)`; and
optionally `is <x> both a/an <y> and a/an <z>?` → yes iff `y(x)` AND `z(x)`.
Each conjunct goes through `kb_query` (so rule-derived membership counts);
answer no if either fails, and admit an unknown class rather than guess.

### Acceptance
- `are <x> and <y> both a <z>?` is yes only when both hold (incl. via rules).
- `is <x> both a <y> and a <z>?` is yes only when both classes hold.
- Unknown class → admitted (the gen16 "I don't know about <z>"), not guessed.
- Held-out predicates prove it is conjunction over the resolver, not canned.

### Notes
- Discovery tooling stays (`PARROT0_TRACE` + `--max-examples 1`).
- Reuse `kb_query`/`kb_knows_pred`; do not add a new solver. Conjuncts should
  benefit from rules exactly as single queries do.
