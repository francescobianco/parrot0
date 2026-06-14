# Current task

> One goal at a time. When it's done, replace this with the next one.
> See LOOP.md for how to work a task, and PRINCIPLES.md for why.

## Goal: gen4 — fractal articulation (a part made of parts)

So far the registry is flat: modules are siblings. PRINCIPLES.md says the
organization is *fractal* — any part is itself a local system. Take the first
step toward depth: let one module be, internally, its own little registry of
sub-parts.

### Idea
- Pick a domain that naturally splits (candidate: a "smalltalk" module that
  dispatches to sub-parts like "how are you", "what can you do", "thanks").
- Implement it so the parent module owns its own ordered list of sub-handlers
  and delegates to them — the same claim/decline protocol, one level down.
- This proves the protocol composes recursively, without yet building a deep
  tree.

### Acceptance
- All existing tests still pass unchanged.
- At least one module visibly delegates to ≥2 internal sub-parts.
- A new `tests/cases/*.chat` exercises the nested behaviour.
- Bump `brain_version()` to `gen4-...`.

### Notes
- Keep the recursion shallow and honest — depth should appear because the
  domain demanded it, not for its own sake.
- Watch for the impostor risk (PRINCIPLES.md): prefer behaviours that compose
  or generalise over canned one-liners, when a choice presents itself.