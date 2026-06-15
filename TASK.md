# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen37 — frequency-weighted continuation (decode loop, step 2)

gen36 proved the autoregressive loop but always takes the first continuation by
insertion order. An LLM follows the *most probable* next token. The
deterministic, legible analogue is the *most frequent* learned continuation.

### Design question
How does parrot0 keep a count per transition (the KB has no in-place update) and
choose the highest-count continuation deterministically?

Candidate: store `cont(prev, word, count)` with `count` a number atom; on
learning a transition, look up the current count, retract the old fact, assert
the incremented one. In `next_word`, gather the candidate words for `prev`,
read each one's count, and pick the maximum — tie-break by insertion order so
the choice stays deterministic.

### Acceptance
- Re-learning a transition increases its count (verifiable).
- `say <w>` follows the highest-count continuation, not merely the first.
- Ties fall back to insertion order (still deterministic).
- Held-out tokens prove it is frequency-driven, not canned.

### Notes
- This is the deterministic analogue of next-token probability, not sampling —
  no randomness (PRINCIPLES: determinism, no magic).
- Reuse `kb_match`/`kb_retract`/`kb_assert`; log any representation choice in
  JOURNAL "Decisions".
