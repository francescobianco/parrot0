# SWE-bench_Lite instances — REAL data, committed as a static snapshot

`lite/<instance_id>/` holds **real** rows from
[SWE-bench/SWE-bench_Lite](https://huggingface.co/datasets/SWE-bench/SWE-bench_Lite)
(300 real GitHub issues; each row carries the problem statement, the repo +
base_commit, the gold patch and the FAIL_TO_PASS / PASS_TO_PASS test ids).

## How it stays faithful to "no network at runtime"
`fetch_lite.sh` touches the network **once, at curation time** (run by the
maintainer), exactly like the Wikipedia corpus under `kb/learning/`. It caches the
raw datasets-server response under the gitignored `.cache/huggingface/datasets/swebench/`
(re-runs reuse the cache — `--force` to refetch) and writes the committed static
fixtures here. **parrot0 itself never touches the network** (PRINCIPLES.md); the
offline harness `tests/swebench.sh` reads only these committed files.

```
tests/swebench/fetch_lite.sh [N]     # default 5; curation step (network once, cached)
make swe-bench                       # offline, reads lite/, behavioural degrade mode
```

## What `make swe-bench` measures today (honest)
A full SWE-bench run needs faculties parrot0 does not have yet: these repos are
**Python** (parrot0's code engine is C-only), they are not checked out at the base
commit, and the hidden pytest suite needs a Python/Docker env. So we cannot
reproduce or patch them now. The harness runs in **degrade / behavioural mode**:
it feeds parrot0 the real issue and reports, per instance, exactly which faculty
blocks it — the intercepted-failure map (CODE-MASTERY.md §8) that names the next
pulls (TASKLIST X-series: Python front-end, repo checkout, run, localization,
patch). **The numbers here are NOT comparable to published SWE-bench scores** —
this is a discovery instrument toward the north star, not a leaderboard run.

## Per-instance files
- `instance.json` — the full real row (problem_statement, repo, base_commit,
  patch, test_patch, FAIL_TO_PASS, PASS_TO_PASS, …).
- `problem.md`    — the problem statement, extracted for readability.
