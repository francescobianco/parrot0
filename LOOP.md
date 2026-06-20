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

1. **Select/read the task.** `TASK.md` holds the single current goal. If it is
   empty or done, promote exactly one next item from `TASKLIST.md` into
   `TASK.md` before observing. Do not substitute a local housekeeping idea unless
   the user explicitly asks for it.
2. **Observe.** Run `make chat` and talk to the current generation. Notice
   where it fails the task.
3. **Hypothesize.** Decide the smallest algorithmic change to `brain.c` that
   moves toward the task. Prefer one idea per iteration.
4. **Self-challenge parity.** Before editing, restate the current `TASK.md`
   problem to parrot0 as a conversational challenge about itself and ask what
   should change. Do **not** let parrot0 manage itself: it must not edit files,
   choose the task, run tests, commit, or push. Judge only the quality of its
   proposed solution against the external agent hypothesis: did it name the
   missing behaviour, the likely module/dispatch point, the test ratchet, the
   version / journal bookkeeping, and its limits? If its support is weaker than
   the external solution, record that gap; it may become the next task.
5. **Implement.** Edit `brain.c`. Bump `brain_version()` if behaviour changed.
6. **Encode the expectation.** Add/adjust a case in `tests/cases/*.chat` that
   captures the new behaviour you want to hold forever. **Then add the
   equivalent `tests/cases/*.it.chat` case** that exercises the *same* behaviour
   in Italian through the *same* code path (gen43). This is the bilingual
   ratchet: a competence proven in two languages lives in the algorithm, not in
   an English phrasebook (PRINCIPLES.md). If the Italian case passes only by
   *duplicating logic* (not by extending the `canonicalize_lang` lexicon), stop
   - that is the signal the logic was overfit to English; fix the core instead.
   Italian is not scored by the bench; it is the internal generalization probe.
7. **Verify.** `make test` must pass (old behaviour preserved, new behaviour
   present). `make chat` should feel better.
8. **Stress the feature (mandatory).** When experimenting with a new feature,
   first try it with basic, minimal experiments. Once it works, immediately add
   or run a trial set roughly **10x more complex** to see whether the structure
   generalizes or only survives the toy case.
9. **Record.** Append a dated entry to `JOURNAL.md`: what changed, why, what
   you observed, what comes next.
10. **Commit & push.** Run `convcommit` to commit the iteration and push it to the
   remote (e.g. `convcommit -t feat -s brain -m "genN - one-line summary" -a -p`).
   Every iteration ends with a pushed commit, so each generation is an atomic,
   recoverable step on the remote - never batch several generations into one push.
11. **Iterate.** Go back to step 1.

## Principles

- **Small, reversible steps.** Each generation should be explainable in a
  sentence.
- **Tests are the ratchet.** Behaviour that earns a test never silently
  regresses.
- **Emergence over design.** Don't hand-build a grand architecture up front;
  let structure appear because the tasks demanded it.
- **Stay pure C.** No runtime model, no network. Complexity is allowed; magic
  is not.