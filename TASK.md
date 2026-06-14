# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen20 — conflict-aware ground queries
## (TASKLIST T3, query semantics slice)

gen19 preserves source disagreement and exposes it in direct belief reports, but
`is X a Y?` still answers `No.` when a matching positive and negative fact both
exist. That is deterministic, but it hides the disagreement.

### Design question
What is the smallest query-level status that exposes exact ground conflicts
without changing the solver into full truth maintenance?

Candidate: add `kb_is_conflicted(pred,args)` for exact positive+negative ground
facts. The NL ground-query surfaces check it before `kb_query`; if conflicted,
answer `Conflicted.`. Variable queries can remain positive-fact enumeration for
now, with the conflict visible through direct reports.

### Acceptance
- If base says `man(socrates).` and session says `not(man(socrates)).`, `is
  socrates a man?` answers `Conflicted.`.
- Same-session correction still answers `No.`, not `Conflicted.`.
- Unknown predicates still answer `I don't know about <pred>.`.
- Binary ground queries get the same exact-conflict treatment if a conflict is
  present in KB state.

### Notes
- This is exact-ground conflict only. Derived conflicts and variable-query
  conflict summaries are later work.
