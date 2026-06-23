# NEXTMOVE — handoff (2026-06-23)

Clean tree, all pushed. Head: `fbd64df` (gen197). `make test` 184/184 (only the
pre-existing `profiles.sh` 4 failures, identical on baseline — unrelated).

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
Pick ONE, smallest-first, KB-first where possible, grounded, EN+IT ratchet:

- **Option A (recommended): X1 run-grounding on C first** — add `code_run`
  (compile+link+EXECUTE, capture exit code) to close the `tests/code/run_execute.code`
  gap. It's a clean, fully-groundable C faculty (the `run_verify`→`run_execute`
  ladder), independent of the hard Python frontier, and is the reusable "did the
  test pass?" primitive every swe-bench solve needs. NOTE: I started this in a
  prior turn then reverted it when we pivoted to real data — re-add `code_run` in
  code.c/code.h and a `mod_codeast` branch ("build and run <path>"), flip
  `run_execute.code` to `#expect: pass`.
- **Option B: X6 toy localization** — map a few issue keywords to a function via
  the call-graph/name overlap on the committed `repo_excerpt`. High-risk (this is
  the F4 frontier CODE-MASTERY says may need statistical association) — keep it a
  measured probe, not a promise.
- **Option C: widen the map** — `tests/swebench/fetch_lite.sh 50` for a richer
  intercepted-failure map before choosing.

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

## Parked
basic-chat driver (`docs/plans/basic-chat.md`): `make basic-chat-bench` 26%
(260/974). Next 0% blocks when resumed: cat.52 lists, cat.18 animal biology,
cat.19 animal sounds (KB content).
