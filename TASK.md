# Current task

> One goal at a time. When it's done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen14 — proof traces / explanations (TASKLIST T2)

parrot0 can answer from rules; now it should be able to **explain the proof**
that made an answer true. This is the first explicit internal representation of
reasoning steps — a candidate piece of the hidden architecture (PRINCIPLES.md).

### Idea
- Give the solver an optional **proof trace** alongside boolean success: when a
  goal succeeds, record which fact closed it, or which rule fired plus the
  proofs of its sub-goals (a small proof tree).
- NL surface: "why is <x> a <y>?" and "why is <x> the <rel> of <y>?" →
  - fact: `mortal(socrates) because man(socrates).` (or "... because it's a
    known fact." for a base fact with no further proof)
  - rule: name the rule and recurse into sub-goals; for multi-goal rules report
    the supporting goals and the shared binding (e.g. `Y = bob`).
  - false claim: "I can't show that." — never invent a reason.

### Acceptance
- "why is socrates a mortal?" (with `man(socrates).`, `mortal(X):-man(X).`)
  yields a chain naming `man(socrates)`.
- A grandparent "why" names both `parent` facts and the intermediate binding.
- A false "why" reports no proof, inventing nothing.
- All existing tests pass; new `tests/*` cover fact / unary-rule / multi-goal
  explanations with held-out predicates (anti-impostor: derived from the proof
  tree, not canned English).
- Bump `brain_version()` to `gen14-...`.

### Notes (method watch — D5.1)
- Add a proof-returning sibling to the solver without breaking `kb_query` /
  `kb_match` callers. Keep the trace structure small and inspectable — it may
  become reusable (planning, truth maintenance) later.
