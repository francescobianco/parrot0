# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen41 — induce the generative model from real text (close the loop)

The generative-loop arc (gen36–gen40) is complete: a deterministic
autoregressive decoder, induced from `learn sequence`, frequency-ranked,
context-backed-off, able to verbalize reasoning and constrained by beliefs. The
remaining honest gap (Decision D-2026-06-15j/k and D-prop1's crux) is the
*source* of the continuation model: today it is taught explicitly. The next step
is to induce it from real text already flowing through the system.

### Design question
Can `read: <passage>` ALSO feed the generative model — learning `cont`/`cont2`
transitions from the passage's word stream — so generation grows from the same
prose the extractor reads, not only from `learn sequence`?

Candidate: in `mod_reader`, after (or alongside) fact extraction, run each
sentence's token stream through `learn_transition`/`learn_transition2`. Then
`say <w>` reflects what parrot0 has actually read. Keep it measurable and
deterministic; guard against the phrasebook trap by ensuring the model is
genuinely the read corpus, not authored.

### Acceptance
- Reading a passage populates the continuation model (verifiable via `say`).
- Generation after `read:` reflects the read text's transitions.
- Fact extraction (gen32) still works unchanged on the same passage.
- Held-out passages prove the model is the corpus, not canned.

### Notes
- This begins to unify the two halves built so far: the reasoning KB and the
  generative loop now share one input path (`read:`).
- Later (DESIGN D-prop1): interpolation over hard backoff; deterministic
  diversity; proving rule-derived contradictions in the gen40 filter.
- Do not hardcode outputs; the model must be the text.
