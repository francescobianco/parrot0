# Current task

> One goal at a time. When it's done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen16 — epistemic states: distinguish "I don't know" from "No."
## (TASKLIST T16 part 2, overlaps T3/T11)

gen15 made *not-understood* honest. The remaining half: *not-known*. Today a
well-formed query about something absent answers a confident "No." / "Nobody"
(closed-world conflating unknown with false). Add a genuine **unknown** state.

### The design question to resolve first
When is a failed query "No." vs "I don't know"? Candidate (test it, don't
assume): if the queried **predicate is entirely unknown** (no fact mentions it,
no rule head defines it) → "I don't know about <pred>."; if the predicate is
known but the goal isn't provable → "No." (closed-world *within* a known
domain). Watch the interaction with retraction (retracting the last fact of a
predicate makes it unknown again — is that the desired semantics?).

### Idea
- `kb_knows_pred(pred)`: any fact with that predicate, or any rule head.
- Query surfaces ("is X a Y?", "is X the rel of Y?", "who/what is the rel of
  Y?") consult it: unknown predicate → "I don't know about <pred>."; else the
  current Yes/No/list/Nobody.

### Acceptance
- "is zorp a blarg?" (blarg never mentioned) → "I don't know about blarg.";
  after teaching some `blarg`, an absent member → "No."
- Existing tests reconciled with the chosen semantics (some `No.` answers about
  never-mentioned predicates may legitimately become "I don't know"); update
  them deliberately, not reflexively.
- New `tests/*` for the unknown-predicate distinction with held-out predicates.
- Bump `brain_version()` to `gen16-...`.

### Notes (method watch — D5.1)
- This is genuinely subtle (open vs closed world). Keep the rule simple and
  documented; if it fights retraction or rules, reconsider the semantics before
  piling on special cases. Consider whether this wants a small `DESIGN.md` note
  on parrot0's epistemic stance.
