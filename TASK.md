# Current task

> One goal at a time. When it is done, replace this with the next one.

> gen75-79 done — 5 iterations extracting the architecture: flexible arith (C9),
> proof trace storage (C13 part 1), self-model introspection (C13 part 2),
> module activation tracking, and emergent induction.

## Goal: C6 — multi-intent turn decomposition

The brain can now answer facts, track proofs, introspect, and induce rules. But
a single turn containing multiple communicative acts still only fires the first
matching module. "ciao, chi sei e ricordati che mi chiamo Francesco" → only
identity fires; the memory instruction is dropped.

The architecture needs a dispatcher that recognizes multiple intents in one turn
and runs them in an order that respects discourse operators (first/then/and/but).

### Acceptance
- A compound turn can both answer an identity question AND store a personal fact.
- "Non rispondere a X, rispondi a Y" suppresses the first recognizable intent.
- Mixed social + content turns pass through without losing content.
- `make test` stays green.

## Completed (gen75-79)