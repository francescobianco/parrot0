# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen40 — critical decoding (decode loop, step 5, capstone)

The decode loop (gen36–gen38) generates fluent learned language; gen39 makes it
verbalize what it reasons. The capstone makes reasoning *constrain* generation:
the learned language model must not utter a claim the system knows to be false.

### Design question
How does the decoder reject a continuation that would produce a known-false
claim, without breaking the ordinary generative loop?

Candidate: when the running output is of the form "<x> is a", treat each
candidate next word `y` as the claim `y(x)`; skip any candidate for which
`kb_is_negated(y, x)` (or conflicted) holds, and take the next-best continuation
instead. Generation thus cannot assert something the KB rejects, while
unconstrained text is unaffected.

### Acceptance
- A continuation that would state a KB-known-false claim is skipped; the
  next-best (still frequency-ranked) continuation is emitted instead.
- When no claim is at stake, generation is identical to gen38.
- If every candidate is blocked, generation stops rather than lying.
- Held-out tokens prove it is belief-driven, not canned.

### Notes
- This is the point of the whole arc: generation that is *critically* filtered
  by inference — the LLM-like surface, disciplined by the KB's beliefs.
- Keep determinism; log the filter's scope/limits in JOURNAL "Decisions".
