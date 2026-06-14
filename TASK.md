# Current task

> One goal at a time. When it's done, replace this with the next one.
> See LOOP.md for how to work a task, and PRINCIPLES.md for why.

## Goal: gen5 — variables + unification

gen4 gave us a ground fact store with closed-world query. The next rung of the
Prolog-like spine: let queries carry **variables**, resolved by **unification**
against the stored facts.

### Idea
- A query term may contain a variable (e.g. `man(X)`); answering it means
  finding all facts that unify and binding the variable.
- NL surface: `"who is a <y>?"` / `"what is a <y>?"` → query `y(X)` and report
  the matching subject(s).
- Implement unification at the `kb.*` level (ground-vs-pattern for now), and a
  query that returns bindings, not just a yes/no.

### Acceptance
- All existing tests still pass unchanged.
- `"who is a man?"` after asserting `man(socrates)`, `man(plato)` lists both.
- A `"who is a <y>?"` with no matches answers gracefully (e.g. "Nobody that I
  know of.").
- New `tests/cases/*.chat` exercises variable queries.
- Bump `brain_version()` to `gen5-...`.

### Notes
- Keep unification minimal but *correct* — it is the load-bearing primitive for
  rules (gen6) and induction (gen7). Don't cut corners here.
- Deterministic ordering of results (insertion order) so tests are stable.
