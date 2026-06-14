# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen21 — conflict-aware explanations
## (TASKLIST T3/T2 overlap)

gen20 makes exact conflicts visible to yes/no queries, but `why is X a Y?` can
still render a proof for the positive side of a conflicted claim. That makes a
disputed claim look settled.

### Design question
What is the smallest explanation-level behavior that respects exact conflicts
without building source-level proof trees?

Candidate: before `kb_explain`, the explanation surface checks
`kb_is_conflicted(pred,args)`. If conflicted, it returns a compact conflict
message instead of proving either side.

### Acceptance
- If base says `man(socrates).` and session says `not(man(socrates)).`, `why is
  socrates a man?` reports conflict rather than `man(socrates) is a known fact.`.
- Non-conflicted explanations keep the gen14 proof behavior.
- False non-conflicted claims still say no proof was found.
- Tests cover unary and binary explanation surfaces where possible.

### Notes
- Do not invent source explanations yet. This generation only prevents disputed
  claims from being explained as settled truths.
