# The parrot0 self-improvement loop

parrot0 is an experiment: a conversational agent in pure C that grows, one
small generation at a time, driven by an AI agent running this loop. There is
no language model inside parrot0 at runtime — only algorithms that *emerged*
from iterating this loop.

> Read **[PRINCIPLES.md](PRINCIPLES.md)** first — it is the *why*. This file is
> only the *how*. Every iteration is an attempt to re-express, in deterministic
> C and verified against real conversation, the structure the model already
> runs internally.

## The contract

- `src/main.c` is the **stable I/O shell**. Avoid changing it.
- `src/brain.c` (+ `brain.h`) is the **part that evolves**. All intelligence
  lives here, as plain, deterministic C.
- The brain must keep `brain_respond()`'s signature so the harness always
  compiles and tests.

## One iteration

1. **Read the task.** `TASK.md` holds the single current goal. If it's done,
   write the next one.
2. **Observe.** Run `make chat` and talk to the current generation. Notice
   where it fails the task.
3. **Hypothesize.** Decide the smallest algorithmic change to `brain.c` that
   moves toward the task. Prefer one idea per iteration.
4. **Implement.** Edit `brain.c`. Bump `brain_version()` if behaviour changed.
5. **Encode the expectation.** Add/adjust a case in `tests/cases/*.chat` that
   captures the new behaviour you want to hold forever. **Then add the
   equivalent `tests/cases/*.it.chat` case** that exercises the *same* behaviour
   in Italian through the *same* code path (gen43). This is the bilingual
   ratchet: a competence proven in two languages lives in the algorithm, not in
   an English phrasebook (PRINCIPLES.md). If the Italian case passes only by
   *duplicating logic* (not by extending the `canonicalize_lang` lexicon), stop
   — that is the signal the logic was overfit to English; fix the core instead.
   Italian is not scored by the bench; it is the internal generalization probe.
6. **Verify.** `make test` must pass (old behaviour preserved, new behaviour
   present). `make chat` should feel better.
7. **Record.** Append a dated entry to `JOURNAL.md`: what changed, why, what
   you observed, what's next.
8. **Commit & push.** Run `convcommit` to commit the iteration and push it to the
   remote (e.g. `convcommit -t feat -s brain -m 'genN — one-line summary' -a -p`).
   Every iteration ends with a pushed commit, so each generation is an atomic,
   recoverable step on the remote — never batch several generations into one push.
9. **Iterate.** Go back to step 1.

## Principles

- **Small, reversible steps.** Each generation should be explainable in a
  sentence.
- **Tests are the ratchet.** Behaviour that earns a test never silently
  regresses.
- **Emergence over design.** Don't hand-build a grand architecture up front;
  let structure appear because the tasks demanded it.
- **Stay pure C.** No runtime model, no network. Complexity is allowed; magic
  is not.