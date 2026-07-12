# Making the test suite fast

> **Status 2026-07-12:** historical analysis, superseded operationally by
> **Fast Forge** in `parrot0-forge-master-plan.md` §§0.1, 10.6 and 10.12-10.18.
> The gen277/gen278 parallelism remains useful, but it no longer solves the
> current suite and a reset-based persistent engine is no longer the preferred
> endgame. The leading POSIX hypothesis is a forkserver plus fresh-exec
> differential verification, but it must beat batching and persistent reset in
> a measured prototype before becoming architecture.

## Current baseline and revised verdict (2026-07-12)

Measurements on the 22-core development host, warm binary, output suppressed:

| Path | Wall time | CPU/user signal |
|---|---:|---:|
| no-op build | 0.02 s | negligible |
| full release rebuild (`-O2`) | 12.37 s | brain unity TU dominates |
| `tests/run.sh` | 21.37 s | 351.64 user-s |
| `make test` | 171.11 s | 503.94 user-s |
| `make code-bench` | 24.29 s | 22.51 user-s |
| `make compose-bench` | 17.24 s | 7 per-dialog process launches |
| `make glue-bench` | 12.81 s | 11 per-dialog process launches |
| `make gate` (six offline targets) | 233.52 s | 561.83 user-s |
| `make gate` + capability report | about 467 s | same gates twice |

Inside `make test`, the current leaders are `llmscore_world.sh` 68.95 s,
`run.sh` 21.37 s, `knowledge.sh` 15.30 s, `profiles.sh` 8.66 s and
`grammar.sh` 8.15 s. The old approximately 51 s baseline below described a
smaller suite and must not be used for planning.

`gate.py` produces no progress during the first approximately 171 seconds
because target output is captured, and it keeps running later targets after a
red result. The current gate+capability-report workflow is therefore about
7m47s and feels opaque even when it eventually succeeds.

The revised conclusion is broader than “make `run.sh` parallel”:

1. **Do not put the full suite in the edit loop.** Run exact contract, collision
   negatives and impacted hot corpus first; full regression belongs to the one
   promotion champion.
2. **Benchmark a forkserver for conversational isolation.** Compare a 50-case
   prototype against fresh-exec batching and persistent reset. If it wins, load
   Brain/KB once, fork a child per session and let process exit discard mutable
   RAM; if it fails the speed/differential kill criterion, keep the simpler
   winner.
3. **Expose semantic verdicts.** Tests of routing/inference should assert
   status/winner/proof rather than paying for exact text and every adapter.
4. **Catalog individual contracts and select by impact.** Target ownership,
   dynamic trace, history and a global canary; audit the selector against the
   full nightly suite.
5. **Cache sealed evidence and never run it twice.** In particular,
   `capability-report` must consume a matching gate artifact rather than rerun
   the gate.
6. **Split dev and release build trees, then benchmark the dev profile.** The
   dominant brain TU compiles in about 1.92 s at `-O0`, 4.89 s at `-O1` and
   9.33 s at `-O2`; choose O0/Og/O1 on compile+focal wall time, keep objects
   separate and verify the champion with release flags.
7. **Stream progress and first failure.** Deterministic final ordering must not
   mean withholding all useful output until completion.

The historical material below is kept because it documents why parallelism was
introduced and the isolation bugs it exposed. Its “test-engine is the endgame”
recommendation is superseded by the measured runner tournament above.

---

## Historical gen277/gen278 analysis

### Measured timings of the then-current `make test`

Binary already compiled, real numbers from one pass:

| Test           | Seconds | % of total |
|----------------|--------:|-----------:|
| run            |   34.65 |      67.4% |
| knowledge      |    2.80 |       5.4% |
| llmscore_world |    2.40 |       4.7% |
| profiles       |    2.18 |       4.2% |
| check_sort     |    1.51 |       2.9% |
| archetype      |    1.27 |       2.5% |
| grammar        |    1.10 |       2.1% |
| posix          |    0.92 |       1.8% |
| experts        |    0.80 |       1.6% |
| synth          |    0.69 |       1.3% |
| persist        |    0.54 |       1.1% |
| skills         |    0.52 |       1.0% |
| explain        |    0.41 |       0.8% |
| howknow        |    0.35 |       0.7% |
| posix_oracle   |    0.34 |       0.7% |
| booklearn      |    0.25 |       0.5% |
| multigoal      |    0.25 |       0.5% |
| anon           |    0.16 |       0.3% |
| research_learn |    0.15 |       0.3% |
| wiki_learning  |    0.12 |       0.2% |
| **TOTAL**      | **~51.4 s** | **100%** |

### The bottleneck was `run.sh`

On its own it accounts for 34.6 s ≈ 67% of all of `make test`. The reason is
**structural, not a slow turn**:

- `run.sh` replays 209 `.chat` files (1320 turns in total);
- for each file it does a separate fork of the binary (`printf … | bin/parrot0`)
  → 209 process launches;
- at every launch parrot0 reloads the KB (`kb/core/*.p0` + base) from scratch.

So ~166 ms per file, almost all of it spent in repeated startup / KB loading,
**not** in inference. The 1320 real turns are very fast (see `archetype`: 16
turns in 1.27 s, most of which is still startup).

The other tests are all under 3 s; `wiki_learning`/`research_learn` are very fast
because they are offline by default (the fetch is gated).

### The levers considered, in order of effectiveness

1. **Run a single process across many conversations in `run.sh`** (would need a
   session separator in the `.chat` protocol) — removes most of the 34 s.
2. **Run the `.chat` files in parallel** (e.g. `xargs -P`), since they are
   independent — near-linear with the number of cores.
3. **Leave everything as is**: 51 s is the cost of a 1320-turn safety net.

---

### The `--test-engine` strategy — analysis (gen277, historical)

**The mission (F.).** Make the tests *extremely* fast. The idea: parrot0 is
itself a **test engine**. Launching `parrot0 --test-engine` starts a service that
accepts dialog files (or texts) framed as tests with expectations; **no new
processes are spawned** — every test is run by a single instance. All tests
should be easy to reduce to this technology.

#### Verdict at gen277

**The conclusion at gen277 was:** the idea is sound and NOT a waste of energy — parrot0 already has the exact
primitives it needs — but it is not the highest-ROI *first* move, and it carries
one real risk that must be managed.** The honest recommendation is a sequence:
parallelism first (a one-liner, same single-machine payoff, zero risk), then the
test-engine as the structural endgame that *composes* with parallelism and
unifies the harness — built on the **robust** whole-brain reset, never a sloppy
partial one, and guarded by a differential check against the per-process oracle.

#### Why the idea is genuinely good

1. **It targets the actual cost.** The 34.6 s of `run.sh` is 209 × (fork + exec +
   dynamic-link + KB load), i.e. startup, not inference. One persistent process
   pays the KB load **once** and eliminates 208 process launches — the dominant
   term.

2. **parrot0 already has the building blocks; this is a small delta, not a new
   engine.**
   - `brain_reload()` (gen276) rebuilds the brain **in place** — forget the
     unsaved session, reload the KB — as a whole-struct move that *cannot miss a
     field* (the only owned pointer is the KB). That is precisely the
     "reset between tests" primitive a single-process harness needs.
   - `--mcp-engine` (gen277) already proves the pattern: one persistent process
     serving many *stateful* requests over stdio, with `kb.restore` (=
     `brain_reload`) as the reset. A `--test-engine` is nearly the same code with
     a different request shape (a `.chat` in, a pass/fail out). It could even be
     *just another MCP tool* (`test.run {chat:"…"}`) rather than a new mode.
   - The `.chat` line protocol (`>` input, `<` expected) is trivially
     serializable as a test request, so reducing the existing cases to it is
     mechanical.

3. **It is philosophically coherent with the project.** parrot0 already *is* its
   own HTTP model (`--daemon`), its own MCP inference DB (`--mcp-engine`), and it
   already runs reflexive self-tests (gen167–169: a sub-brain runs a dialogue and
   scores itself). "parrot0 is its own test harness" is the same reflexive move,
   not a foreign bolt-on.

4. **It composes with parallelism for a multiplicative win.** N test-engines,
   each running a shard of the cases, gives (cores) × (no-respawn) speedup —
   strictly better than either lever alone.

#### The real risks (and how to keep it from becoming a waste)

1. **Isolation is a FEATURE, not just overhead — do not trade it away.** The whole
   reason each case runs in a fresh process today is hermetic isolation: a bug
   where state leaks from one turn/file to the next is *caught* because every test
   starts truly clean. A single-process engine that resets in-RAM state can
   **mask** a state-leak bug if the reset is imperfect. This is the one way the
   strategy turns into a net loss: buying speed by discarding the harness's most
   valuable property.
   - **Mitigation:** the reset MUST be the robust whole-brain rebuild
     (`brain_reload` / a fresh `brain_create`+`brain_boot` swapped in place),
     which by construction cannot forget a session field — *not* a hand-written
     "drop the session facts" partial reset, which is exactly where a leak would
     hide. And keep the old per-process path as the **gold oracle**: periodically
     (or in CI nightly) run a sample of cases both ways and require byte-identical
     verdicts, so the engine can never silently diverge from true isolation.

2. **Parallelism is the cheaper competitor for the same single-machine payoff.**
   The cases are independent; `run.sh` with `xargs -P` (this box has 22 cores)
   drops 34.6 s toward ~2–4 s with **one line of code, zero new protocol, and
   perfect process isolation preserved**. On a single dev machine that is as good
   as the test-engine, for a fraction of the effort. The test-engine's unique edge
   shows up on **low-core CI** and as an **architecture** (unified harness, richer
   per-turn diagnostics, incremental/interactive runs) — real, but secondary to
   the immediate speed goal.

3. **"All tests reduce to this" is optimistic.** The `.chat` cases (67%) reduce
   cleanly. But `check_sort`, `synth`, `posix`, and especially `persist` /
   `restore` / `mcp` exercise real compilation, subprocesses, file I/O, and —
   pointedly — **process-restart semantics**. A test whose whole point is "state
   survives a real restart" cannot be run inside one never-restarting instance
   without losing what it tests. The engine should own the `.chat` conversation
   suite; the process-level tests stay as they are. Selling it as "everything
   reduces to one engine" over-promises; scoped to the conversation suite it is
   exactly right.

#### A rough cost model (to keep the claims honest)

- Today: `run.sh` ≈ 34.6 s = 209 × ~166 ms, of which the great majority is
  startup, a small tail is the ~1320 turns.
- **Parallelism (-P N):** ≈ 34.6 / N + overhead → a handful of seconds on this
  box (22 cores). One line. No risk.
- **Test-engine, robust reset:** load the core KB once, then per case
  `brain_reload` (re-reads the small core `.p0` files, already in page cache) +
  the turns. Removes all fork/exec/dynlink; the reset is cheaper than a spawn but
  not free → run.sh in the ~8–12 s range single-threaded.
- **Test-engine + a truly cheap session reset** (drop `KB_SESSION|KB_INDUCED` +
  reset the Brain struct, no file reload) → run.sh ≈ the turn time alone, a few
  seconds — *but this is the reset that risks masking leaks*, so it is only
  acceptable behind the differential gold-oracle guard above.
- **Both (sharded engines):** the floor, and the right long-term shape.

#### Recommendation made at gen277

1. **Now (free win):** parallelize `run.sh` with `xargs -P` (or GNU `parallel`),
   keeping each case in its own process. Immediate ~10× on this machine, zero
   risk, nothing new to maintain.
2. **Then (called the endgame at the time):** build the test-engine — ideally as an MCP tool
   `test.run` on the existing `--mcp-engine`, or a thin `--test-engine` mode —
   using `brain_reload` as the reset. Scope it to the `.chat` conversation suite.
   Add a differential CI check: a sample of cases must produce identical verdicts
   in-engine and per-process, so the speed never costs isolation.
3. **Keep** the process-level tests (`persist`, `restore`, `mcp`, `check_sort`,
   `synth`, `posix`) on their own path — they test things a single instance
   structurally cannot.

### Implemented — recommendation #1 (gen278)

`tests/run.sh` now runs the cases in parallel via `xargs -P` (degree
`$PARROT0_TEST_JOBS`, default `nproc`), each case still in its own hermetic
parrot0 process; output is aggregated in sorted case order so PASS/FAIL lines and
the summary read identically, and `PARROT0_TEST_JOBS=1` restores the old serial
behaviour. **Measured on this box (22 cores): run.sh 102 s → ~14 s, ≈ 7.3×,
stable and byte-for-byte green across repeated runs.**

**A real finding that VALIDATES the "isolation is a feature" warning above.**
Parallelism immediately exposed a latent bug: the code oracle in `src/code.c`
(and the two edit temps in `src/brain/80-code.c`) used **fixed** temp filenames
(`.p0_run_tmp.out`, `.p0_gamecheck.c`, …) in the CWD. Serially that is harmless;
concurrently, two parrot0 processes doing code synthesis raced on the same file
and one produced a wrong result — the `reqgen`/`rulespec` cases flapped. The fix
is the right one regardless of tests: every oracle temp is now **per-process
unique** (`.p0_<stem>_<pid>.<ext>`), so the compile/run oracle is concurrency-safe
— which the MCP engine (an agent could trigger concurrent synthesis) wanted too.
This is exactly the "parallelism is not entirely free — shared mutable state
hides here" lesson; it was in the C temp files, not in the brain state, and it is
now closed. `.gitignore` gained `.p0_*`.

Historical note written at gen278: recommendation #2 was then considered the
endgame. **This sentence is superseded as of 2026-07-12:** build the forkserver
prototype and its alternatives first; implement the measured winner and verify
it against fresh-exec.

**Historical gen278 summary:** the test-engine is a legitimate, on-brand extension that parrot0
is already 90% equipped for; it becomes a waste only if it is built with a sloppy
reset that trades away hermetic isolation, or if it is done *instead of* the
one-line parallelism win rather than *after and alongside* it.
