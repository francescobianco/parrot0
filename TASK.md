# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen38 — longer context via trigram backoff (decode loop, step 3)

gen36/gen37 condition only on the single previous word (Markov-1). An LLM
conditions on a long running context. The next honest step is to condition on
the previous *two* words, backing off to one when the longer context is unseen.

### Design question
How does parrot0 learn and use a 2-word context without abandoning the bigram
model it already has?

Candidate: alongside `cont(prev, word, count)`, learn `cont2(p1, p2, word,
count)` (4-ary; KB_MAX_ARGS is 4) from triples in `learn sequence`. In the
decode loop, track the last two emitted words; if a `cont2(last2, last1, ?)`
exists, choose the highest-count among those; otherwise BACK OFF to the gen37
bigram choice. Frequency choice (argmax, insertion-order tie-break) is reused.

### Acceptance
- A trigram context disambiguates where the bigram is ambiguous (e.g. the same
  middle word continues differently depending on the word before it).
- When no trigram context matches, generation backs off to the bigram model and
  still works.
- Counts drive the trigram choice exactly as for bigrams.
- Held-out tokens prove it is context-driven, not canned.

### Notes
- Reuse `learn_transition`/`next_word` shapes; keep determinism.
- Log the backoff policy (and any 4-ary representation choice) in JOURNAL
  "Decisions".
