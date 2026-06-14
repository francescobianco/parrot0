# Current task

> One goal at a time. When it's done, replace this with the next one.
> See LOOP.md for how to work a task, and PRINCIPLES.md for why.

## Goal: gen2 — make the brain articulable (a module registry)

Per PRINCIPLES.md's corollary, the brain must be able to become *a system of
cooperating parts*, fractally. gen1 is still a flat chain of `if`s inside
`brain_respond()`. Introduce the **infrastructure** for articulation — without
designing the eventual modules.

### Idea
- A "module" is a small unit that, given normalized input, may either *handle*
  it (produce a response) or *decline* (pass to the next).
- The brain holds an ordered **registry** of modules; `brain_respond()` walks
  it and uses the first handler that claims the turn; if none do, it parrots.
- Re-express gen1's greeting/farewell logic as the first two modules, so
  behaviour is unchanged but now lives in articulated parts.

### Acceptance
- All existing tests (`greet.chat`, `parrot.chat`) still pass unchanged.
- `brain.c` no longer hard-codes intents inline; they are modules in a registry.
- A module can be added/removed by touching only the registry, not
  `brain_respond()`'s control flow.
- Bump `brain_version()` to `gen2-...`.

### Notes
- Keep it pure, deterministic, dependency-free.
- Do NOT pre-invent a taxonomy of modules — only what gen1 already needs.
  The point is the *capacity* to articulate, so future tasks can grow parts
  (and parts-of-parts) as they become necessary.