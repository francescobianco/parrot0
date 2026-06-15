# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

> gen67 (M2 step 1, measurable book learning shift) is DONE — see JOURNAL.
> gen68 (meta-conversation wall: attention/reading/understanding/repetition) is DONE — see JOURNAL.
> gen69 (C6 step 2, polar meta-questions) is DONE — see JOURNAL.
> gen65 symbolic challenge remains parked as the S-series in TASKLIST.md.

## Goal: C6 step 3 — validate meta/polar fixes with a chat-sim round

The held-out English and Italian cases for polar meta-questions now pass, but
those are scripted probes. The real question is whether the recent meta and
polar work moves the adversarial conversation benchmark (`make chat-sim`).

### Design question
Does the latest `make chat-sim` run show a lower wall-rate and fewer
meta/casual failures ("are you there?", "can you hear me?", "are you a bot?",
Italian mirrors, and related variants) than the baseline recorded in
JOURNAL.md? If not, which failure class should be addressed next?

### Acceptance
- Run `make chat-sim` (needs `$OPENCODE_API_KEY` + network) and inspect the
  logged transcripts under `tests/chat/sim/`.
- Report wall-rate, immediate-repetition-rate, and the dominant residual
  failure class compared to the previous chat-sim baseline.
- If a clean, recurrent, fixable gap appears, promote it to a deterministic
  `.chat` ratchet + a small `brain.c` fix; if the signal is noisy, run another
  seed or tighten the prompt before coding.
- `make test` must stay green.

### Notes
- M2 propositional book learning remains parked until the multi-word-entity /
  open-prose extraction wall is addressed.
- S-series symbolic decoding (Morse/text, leet/word, cryptic tokens) remains
  parked in TASKLIST.md.
