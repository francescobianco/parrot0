# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen49 — multi-word entities (the real-bench wall)

The bench is now a live instrument (gen45) and earned its first real nonzero:
ReCoRD 0% → 24.47% via a transparent salience baseline (gen48), lifting overall
SuperGLUE 0.00% → 3.06%. The label tasks (BoolQ, RTE, CB, COPA, WiC, WSC) are
still 0% — they need genuine comprehension, and the single biggest blocker is
that parrot0 treats every entity/class as ONE token. Real prose says "property
tax", "house tax", "United States" — multi-word noun phrases the extractor and
the query parsers cannot represent.

### Design question
Can a multi-word noun phrase be carried as a single canonical atom (e.g.
`property_tax`) so the existing reasoning works unchanged on it? Candidate: a
chunking pass that joins adjacent capitalized words (and known noun phrases)
into one token before dispatch — the smallest step that lets "X or 'Y' is a Z"
and "is X and Y the same?" reach the `same` module on a real BoolQ example
(validation #2, label yes, is genuinely within parrot0's `same` reasoning once
the entities are single atoms).

### Acceptance
- A real BoolQ/validation example whose answer is derivable by parrot0's
  reasoning (start with the property-tax sameness example) becomes VALID and
  CORRECT through genuine inference — measured against the live bench.
- All existing English + Italian `.chat` cases unchanged (single-word entities
  still work); the chunker must not corrupt them.
- The bilingual ratchet holds: the chunker is language-neutral (operates on
  surface tokens, not English-specific words).

### Notes
- This is genuine structure the domain demanded (PRINCIPLES.md), not a trick:
  multi-word reference is a real latent structure LLMs handle effortlessly.
- Keep it minimal and reversible; chunk conservatively (capitalized runs,
  quoted spans) before attempting general noun-phrase detection.
- Never inflate the label tasks by guessing; abstention stays the honest answer
  until reasoning genuinely covers the example.
