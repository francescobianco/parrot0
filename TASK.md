# Current task

> One goal at a time. When it's done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions (incl. D5.1: primitives-first + the pivot duty).

## Goal: gen11 — n-ary relations + multi-goal rule bodies

Everything so far is unary (`man(socrates)`) with single-goal rules
(`mortal(X) :- man(X)`). Real expertise — starting with grammar — needs
**relations** between things and rules whose body is a **conjunction of
goals** with shared variables. This is the representation jump D5 flagged.

### Idea
- Facts with arity ≥ 2 already store (kb supports KB_MAX_ARGS); add the query
  and resolution paths for them: e.g. `parent(tom, bob).` and
  `is tom the parent of bob?`.
- General unification with variable bindings across a rule body, e.g.
  `grandparent(X, Z) :- parent(X, Y), parent(Y, Z).`, resolved by backward
  chaining that threads bindings between goals.
- NL surface kept small and explicit (a binary relation form is enough to
  prove the engine); richer parsing is gen12.

### Acceptance
- A binary fact can be asserted and queried (ground and with a variable).
- A two-goal rule with a shared intermediate variable resolves correctly
  (the grandparent example, or similar).
- All existing tests still pass; new `tests/cases/*` covers relations +
  multi-goal resolution.
- Bump `brain_version()` to `gen11-...`.

### Notes (method watch — D5.1)
- **North-star check:** before building, name a concrete *grammar* need this
  serves (e.g. `follows(word1, word2)` / agreement relations). If no real
  grammar need pulls n-ary relations, that is a PIVOT signal toward domain-pull
  — call it, don't build speculatively.
- Generalising unification beyond the current single-binding `prove1` is the
  load-bearing change; keep it correct and well-tested.
