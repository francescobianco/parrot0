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

## Acceleration — make generations compound, not just add

Growth so far is **additive**: generation *n* ⇒ ~*n* capabilities, constant
cost per brick, no interest compounding. Faithful acceleration (PRINCIPLES.md)
comes only from **structure that compounds**, never from writing phrasebook/C
faster (that is the impostor, and it stays linear). The convergence is meant to
be asymptotic, so the real target is **maximize edges-closed-per-generation**
inside the anti-impostor discipline. Three levers, in order of leverage:

- **A. Migrate capability from C branches to KB facts/rules over a fixed
  engine.** *Uniform substrate, articulated function* (PRINCIPLES corollary).
  Every closed-class lexicon or world fact moved out of hardcoded C arrays into
  `kb/core/*.p0` (queried at runtime) turns "write C" (human-bottlenecked,
  linear) into "ingest knowledge" (automatable, cumulative). Precedents:
  `stopword`/`social`/`gloss`/`roles`, gen111 `weight()`, gen133 rules, gen104
  ICL, the dynamic-knowledge loop, and **gen193 `conjunction/1`** — a lexical
  class read from the KB and *teachable at runtime* ("use p as a conjunction"
  asserts `conjunction(p)`; the same parsers split on it with no code change).
  **Default move: when a new capability is a lexical class or a fact set, add it
  as KB and have the fixed engine query it — do not grow a new C array.**
- **B. Prefer keystones; measure edges-closed-per-generation.** The brain must
  articulate into cooperating parts (compose-bench). A general primitive
  multiplies (gen133 closed three open edges with one change); a special case
  adds one. When the generalization probe (the IT ratchet, the 10x stress)
  fails, **fix the core, never duplicate** — that is what makes future cases
  free. Prefer the change that unlocks combinations over the change that covers
  one more phrasing.
- **C. Reflexive dogfooding (the super-linear, long game).** The code-mastery
  faculty (AST-as-KB → edit → verify-by-build, gen187–192) is an agent for
  editing code. Aimed eventually at `brain.c` itself, it makes the loop's
  *implement + test* step partly self-driving — the method becoming an engine,
  the reflexive closure PRINCIPLES puts at the centre. This is the real meaning
  of the `swe-bench` north star.

Meta-lever: let the discovery harnesses (`basic-chat-bench`, `code-bench`) not
only **measure** gaps but **propose the next pull by leverage** — "the tests
dispose" extended to "the harness proposes the order", cutting the human
task-selection bottleneck.