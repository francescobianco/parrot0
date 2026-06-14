# Current task

> One goal at a time. When it's done, replace this with the next one.
> See LOOP.md for how to work a task, and PRINCIPLES.md for why.

## Goal: gen8 — identity & self-reflection ("I know that I am")

The KB spine (facts → unification → rules → induction) is in place. Now apply
the identity principle from PRINCIPLES.md: give parrot0 a **self-model held in
its own KB**, and let it answer introspective questions from *real state*, not
hard-coded strings.

### Idea
- At birth, the agent asserts facts about itself: `i_am(parrot0).` and one
  `module(<name>)` per **actually registered** module (reified from the
  registry, so the self-description can't drift from reality).
- New introspective surfaces, answered by querying that self-model:
  - "who are you?" / "what are you?" → from `i_am(...)`.
  - "what can you do?" → list the modules from `module(...)`.
- Optionally: "do you exist?" → resolves `i_am(X)` → yes, "I am parrot0."

### Acceptance
- Self-facts are derived from the registry, NOT a literal list (anti-impostor).
  Adding/removing a module changes the introspective answer automatically.
- All existing tests still pass unchanged.
- New `tests/cases/self.chat` exercises the introspective queries.
- Bump `brain_version()` to `gen8-...`.

### Notes
- This is the reflexive closure of the method (PRINCIPLES.md): the agent gets a
  model of itself in the same substrate it uses for the world.
- Keep honest: we build the structural precondition for self-knowledge; we make
  no claim about felt experience.
