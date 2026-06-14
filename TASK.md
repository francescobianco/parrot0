# Current task

> One goal at a time. When it's done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions (incl. D5.1: primitives-first + the pivot duty).

## Goal: grammar v2 — sentence structure (try-first, domain-pull)

The grammar expert can do parts of speech and morphology. The next competence
it pulls toward is **sentence well-formedness**: e.g. "a (toy) sentence is a
noun followed by a verb".

### Discipline (D5.1 — try before you build)
1. FIRST attempt to express this with the **current** primitives only — e.g. a
   relation `next(w1, w2)` for adjacency plus a multi-goal rule
   `sentence(A, B) :- noun(A), next(A, B), verb(B).` in `knowledge/grammar.pl`,
   queried with existing NL. Write `tests/grammar.sh` cases for it.
2. ONLY if it genuinely cannot be expressed, identify the missing primitive
   (sequences/list terms? a sentence-as-structure?) and make THAT the next gen,
   citing the concrete blocker. Do not pre-build sequence machinery on a hunch.

### Acceptance
- A well-formed toy sentence is recognised; an ill-formed one is rejected, via
  the existing engine if at all possible.
- All existing tests still pass.
- Only bump `brain_version()` if step 2 forced an engine/NL change.

### Notes
- This is the moment to watch the reflexive payoff (PRINCIPLES.md): once the
  agent has real sentence structure, it becomes the substrate for *parsing*
  input better — the language-improves-intake loop. But don't force it; let the
  need be demonstrated.
