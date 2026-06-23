# NEXTMOVE — handoff (2026-06-23)

Clean tree, all pushed. Head: gen198 (`gen198-run-grounding`). `make test` green
except the 4 pre-existing `profiles.sh` agi failures (verified identical on
baseline via stash — unrelated). `make code-bench` 20/20 gates, 0 gaps.

## Just landed — gen198 (X1 run-grounding, was NEXTMOVE Option A)
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
