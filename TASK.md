# Current task

> One goal at a time. When it's done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions (incl. D5.1: primitives-first + the pivot duty).

## Goal: gen12 — flexible relation queries (pulled by grammar/morphology)

**Why now (D5.1 — a real pull, not a guess):** grammar v0 works, but extending
it to morphology immediately hit a wall: "what is the plural of dog?" wants the
variable in the *object* position (`plural(dog, X)`), while today's NL only
offers "who is the <rel> of <y>?" (`rel(X, y)`). The grammar domain is pulling
for queries that can put the unknown in either argument position. This is the
first domain-justified primitive — build exactly what it asked for, no more.

### Idea
- NL surface: "what is the <rel> of <y>?" → `rel(y, X)` (unknown in arg 1),
  complementing the existing "who is the <rel> of <y>?" → `rel(X, y)`.
- Then morphology becomes expressible in `knowledge/grammar.pl`:
  `plural(dog, dogs).` and a rule `countable(X) :- plural(X, _).`
- Report the bound value(s) the same way as other variable queries.

### Acceptance
- After `plural(dog, dogs).`, "what is the plural of dog?" → "dogs."
- `countable(X) :- plural(X, _)` resolves ("is dog a countable?" → "Yes.").
- All existing tests still pass; extend `tests/grammar.sh` with the morphology
  competence; bump `brain_version()` to `gen12-...` (this one IS an engine/NL
  change).

### Notes (method watch — D5.1)
- Keep it to what grammar asked for. If a second domain later needs the same
  flexibility, good — that confirms reuse. Don't pre-build positions/arities no
  domain needs yet.
