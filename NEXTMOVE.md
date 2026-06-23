# NEXTMOVE — handoff (2026-06-23)

Clean tree. Head: gen203. `make test` FULLY GREEN (zero failures) — gen203 fixed a
heap-use-after-free in `kb_derive_part_of` that was crashing the agi profile and was
the real cause of the 4 long-dismissed `profiles.sh` "agi" failures (now 13/13).
`make code-bench` 21/21 gates. gen202 fixed the stray `<` in `make chat`.

## Just landed — TWO real SWE-bench instances RESOLVED, two general smells
parrot0 derives patches from STRUCTURE and the OFFICIAL SWE-bench Docker image
judges them RESOLVED (it never sees gold patch / tests):
- **astropy-12907 (gen200)** SYMMETRY BREAK — `code_symmetry_fix`. `make swe-solve`.
- **astropy-6938 (gen201)** DISCARDED RESULT — `code_find_discarded_result`: a bare
  pure-method call (`output_field.replace(...)`) whose value is thrown away; assign
  it back in place (receiver is a parameter -> rebinding is a no-op). `make
  swe-solve INSTANCE=astropy__astropy-6938`.

The `mod_codeast` "find/fix the bug in <path>" branch tries each structural smell
in turn — a growing LIBRARY of grounded localizers, not general APR. New since
gen200: `code_find_discarded_result` + `is_pure_method` KB; EN+IT `discarded.chat`/
`.it`. Harness `tests/swebench/{oracle,parrot_solve}.sh`.

How to run: needs docker + jq; pulls the ~2.7GB official image once per instance (a
curation step; parrot0 never touches the net). To target a NEW instance: snapshot
its base-commit source into `tests/swebench/lite/<id>/repo_excerpt/...` (copy from
the official image at base_commit, like fitsrec.py/separable.py), then `make
swe-solve INSTANCE=<id>`.

NEXT: widen the map (`tests/swebench/fetch_lite.sh 50`, `make swe-bench`) and add
the next grounded localizer/transformation the next instance pulls — each judged
by the real oracle, never hardcoded. The numpy value-domain semantics delta is the
big one for instances needing semantic (not structural) reasoning.

## Just landed — gen199 (Python F3 semantics by delta, CODE-MASTERY §7b)
F. steered: Python semantics is derived by DIFFERENCE from C (§7b), not built
separately. `code.c`: `eval_py_fn` (Python front-end — `def name(params):`,
identifier-first params, newline statements: locals + return) reuses the SAME
`ev_rel` expression evaluator as C; `eval_any` dispatches C-then-Python and is used
by `code_eval` AND the in-expression call recursion, so Python→Python recursion
works. No brain logic changed — the eval branch already called `code_eval`
language-agnostically, so Python lit up for free (the §7b payoff). Proven: `add`,
precedence, annotated params, multi-line file locals (`sqpy`=36), recursion
(`usepy(4)`=20); honest refusal on if/string/unknown-call. EN+IT `eval_py.chat`/
`.it`, codebench gate `evaluate_py.code`. Distance report item 2 updated.

NEXT for astropy-12907: the value-domain delta (numpy ARRAY assign/broadcast — an
additive Python-specific fact set), X6 issue->`_cstack` localization (the
associative crux §4), X7 expression-level patch synthesis (rename/delete exist; an
expression edit `= 1`->`= right` is the missing transformation).

## Earlier — gen198 (X1 run-grounding, was NEXTMOVE Option A)
`code_run` in code.c/code.h: compile+link, then EXECUTE the built binary in a
sandboxed child and report its REAL exit status (`WEXITSTATUS`). The C verify
ladder compile->link->run is now COMPLETE — the grounded "did it pass?" oracle.
`mod_codeast` branch: "build and run <path> ... exit code" (verb cue from the
per-clause `norm`, path from raw). Flipped `run_execute.code` to `#expect: pass`
(proves exit 0 AND 7); EN+IT `run_execute.chat`/`.it`. Also fixed a latent
compound double-dispatch (codeast branches read path from the whole raw input,
which `decompose_and_dispatch` passes to every sub-clause).

## Where we are
Driver = the REAL SWE-bench north star (CODE-MASTERY.md). The active goal in
`TASK.md` is climbing toward the first real Lite instance **`astropy__astropy-12907`**.

Recent generations:
- **gen193** — `conjunction/1` is KB knowledge, teachable at runtime ("use p as a
  conjunction"). The template for acceleration lever A (LOOP.md "Acceleration").
- **gen194** — `make swe-bench` (degrade harness) + the language-as-delta growth
  law written into CODE-MASTERY §7b, LOOP.md, TASKLIST X-series.
- **gen195** — `make swe-bench` wired to the REAL `SWE-bench/SWE-bench_Lite`:
  `tests/swebench/fetch_lite.sh` fetches rows ONCE (network=curation), caches under
  the gitignored `.cache/huggingface/datasets/swebench/` (re-runs reuse it,
  `--force` to refetch), commits static fixtures to `tests/swebench/lite/`. parrot0
  never touches the net. 5 real instances committed (all astropy).
- **gen196** — Python "by delta": `code_ingest_py` (src/code.c) emits the SAME
  abstract facts as the C front-end (`code_function/1`, `code_calls/2`), scoped by
  indentation. `code_defines`/`code_locate` learned the `def` head + `.py`; the
  structural handler dispatches by `identify_code_lang` and accepts `.py` paths;
  `strip_edge_punct` keeps `_`. Verified on the real 317-line `separable.py`.
  Distance report: `docs/swebench/astropy-12907-distance.md`.
- **gen197** — multi-line interactive input in `src/main.c` (TTY only; piped path
  unchanged): Shift+Enter (CSI-u + modifyOtherKeys), bracketed paste, `\`-continuation.

## What works toward astropy-12907 (run `make swe-bench`)
On the REAL `separable.py`, parrot0 already: lists all functions, gives `_cstack`'s
call list, and locates `_cstack` by name. The read/structure distance is CLOSED.

## The remaining distance (see docs/swebench/astropy-12907-distance.md)
1. **X6 — issue → code localization** (the issue says "nested CompoundModels", not
   "_cstack"). The associative frontier; biggest gap.
2. **Python semantics** — parrot0 sees calls but cannot reason that `= 1` vs
   `= right` changes the result (C `code_eval` doesn't cover Python).
3. **X7 — patch synthesis** — no expression-level edit transformation yet
   (rename/delete exist; this needs a new AST transformation).
4. **Run-oracle** — verifying needs the repo at base_commit + a Python env to run
   pytest; today the only grounded oracle is the C compiler.

## Recommended NEXT MOVE
X1 (Option A) is DONE. The C verify ladder is complete; the binding constraint is
now the **Python frontier**. Pick ONE, smallest-first, KB-first where possible,
grounded, EN+IT ratchet:

- **Option A (recommended): X3 — abstract node vocabulary.** Audit whether the
  gen173-196 analyzers (localization, `calls`/`assigns`, find_callers) speak
  C-specific structure or an abstract `node/…` vocabulary; refactor so they read
  abstract facts. CODE-MASTERY §7b: *this* is "support Python", not a parser
  rewrite. gen196 already emits the same `code_function`/`code_calls` facts from
  Python — extend that delta to the analyzers. Unblocks every later pull.
- **Option B: static repo checkout (curation)** — check out a repo at
  `base_commit` as static files (network once, like the wiki corpus) so a real
  swe-bench repo is readable offline. Prereq for X6 localization / X7 patch.
- **Option C: extend run-grounding to a TEST oracle** — today `code_run` reports
  one program's exit code; a swe-bench solve needs "run THIS test and see it flip
  fail->pass". Smallest C version: run a file whose `main` asserts, report
  pass/fail. (Stays C-grounded; bridges toward the pytest oracle conceptually.)

## Gotchas to remember
- `split_words` null-terminates its buffer in place and `w[8]` truncates — read
  cues from the intact `norm`, re-split into a larger array (see gen190 mod_arith).
- KB leading `_`/uppercase = a Prolog variable; modules see the canonicalized
  surface (`canonicalize_lang`), but arithmetic operator words (per/più/diviso)
  are NOT canonicalized — match them directly.
- New fixtures under `tests/code/` are walked by the recursive locate gates —
  keep function names non-colliding (gen196 used `pyadd/pysquare/pymain`).
- Push may need `git pull --rebase` first (an automated "learn wikipedia page"
  cron pushes to main). Rebuild + `make test` after rebasing, then push.
- Commit with `convcommit -t <type> -s <scope> -m "genN - ..." -a -p`.
- `decompose_and_dispatch` passes each sub-clause as `norm` but the WHOLE raw
  input as `raw`. A module that reads from `raw` (e.g. the codeast path-branches)
  will re-fire on every sub-clause of "X and tell me Y" and DOUBLE its answer —
  gate the trigger cue on `norm`, take only positional data (paths) from `raw`.

## Parked
basic-chat driver (`docs/plans/basic-chat.md`): `make basic-chat-bench` 26%
(260/974). Next 0% blocks when resumed: cat.52 lists, cat.18 animal biology,
cat.19 animal sounds (KB content).
