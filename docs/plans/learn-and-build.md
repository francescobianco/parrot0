# Learn-and-Build: from Wikipedia to a verified algorithm

> The challenge, the honest ground truth of where parrot0 stands today, and a
> detailed, reversible plan for the next step. Read alongside
> [PRINCIPLES.md](../../PRINCIPLES.md) (anti-impostor) and
> [generative.md](generative.md) (generation = structure under an oracle).

## The challenge (F., 2026-06-24)

Demonstrate, with **repeatable tests**, that parrot0 can:

1. **learn** a sorting algorithm from a Wikipedia page, from a single prompt;
2. **implement** the algorithm as real code;
3. **test** that implementation;
4. all driven through the **pi agent**, in **one multi-step interaction**;
5. inside a test that first **removes the sorting skill from the KB**, then has
   parrot0 **re-learn it via research**, then **build + verify** it.

The non-negotiable: no impostor. A hardcoded sort replayed from a `printf` is
exactly the "perfect impostor" PRINCIPLES rejects. The implementation must be
*derived* and *disposed by an oracle*, or it must be *honestly refused*.

## Ground truth discovered (verified, not assumed)

What follows was checked against the running binary and the source, not guessed.

### What already works — the LEARN half is real

- **In-C research learner** (`mod_research` + the learner): reads a Markdown page
  from `PARROT0_WIKI_DIR`, extracts a `## Learned Concept` + `Domain`, and persists
  `wiki_concept(name, domain, "<description>")` into `PARROT0_LEARN_KB`; a re-ask is
  answered from RAM. Regression: `tests/research_learn.sh`.
- **Verified probe** (empty KB → learn → recall):
  ```
  > tell me about bubble sort
  I didn't know about bubble sort, so I just read it up: a sorting algorithm that
  repeatedly steps through the list, compares adjacent elements and swaps them ...
  → wiki_concept(bubble_sort, algorithms, "...").   (persisted once)
  ```
- **Curated algorithm knowledge** already in the KB:
  `kb/experts/programming/algo.p0` —
  `algorithm(bubblesort, sorting, "O(n^2) -- repeatedly swaps adjacent elements until sorted").`
  plus quicksort/mergesort/insertionsort/heapsort, complexity facts, `faster_than/2`.
- **Multi-step orchestration** exists: the articulated planner (gen207, in
  `mod_compose`) splits a long prompt into ordered sub-goals and threads artifacts,
  each oracle-checked. `make piagent-bench` (14/14).
- **Verification primitives** exist in `src/code.c`:
  - `code_eval(src, name, argv, argc, *out)` — symbolic execution of a single
    arithmetic `return` (no loops/arrays).
  - `code_compile(path, …)` — real `cc -fsyntax-only` in a sandbox.
  - `code_build(path, …)` — compile **and link**.
  - `code_run(src_path, *exit_code, …)` — compile+link+**execute**, observe the exit
    status. **This is the array-aware oracle a sort needs** ("did the test pass?").

### What does NOT exist — the BUILD half is missing

> **Superseded (gen209, Track B shipped).** The three points below described the
> starting state. The BUILD half now exists for a named, schema-backed sort:
> `code_synth_from_shape` composes the body from a KB schema and `code_check_sort` runs
> it on real vectors. The refusal probe below still holds **only for the honest gap** —
> a request whose algorithm is *not* in the KB (e.g. an unnamed "sort an array", or a
> linked-list reverse). Kept verbatim as the record of where we started; see Track B.

- **No code synthesis beyond a one-line arithmetic body.** `mod_compose` composes
  `int f(int a,int b){return a OP b;}` from a KB operator and verifies with
  `code_eval`. There is **no synthesis of loops, arrays, indices, or swaps.**
- **Verified refusal probe** (the honest wall today):
  ```
  > write a C function bubblesort that sorts an array of integers
  I can only synthesize and VERIFY the sum, product, or difference of two integers
  so far — I will not emit code I cannot check.
  ```
- **The learned knowledge is prose, not executable structure.** `wiki_concept(...)`
  holds a sentence ("...swaps adjacent elements..."), not the structured steps a
  synthesizer could instantiate. Even the *input* to synthesis is in the wrong form.

### The boundary, stated plainly

The LEARN→RECALL→ORCHESTRATE path is genuine and repeatable **today**. The
LEARN→**BUILD**→TEST path is blocked at BUILD: synthesizing a correct sort from a
description is real program synthesis parrot0 cannot do, and faking it violates the
founding rule. So the full challenge is **not honestly demonstrable today** — and we
will not cheat it. The plan below makes the honest part repeatable now and lays the
reversible road to the real BUILD faculty.

> **Update (gen209):** that road has been walked. The BUILD faculty exists for a named,
> KB-schema-backed algorithm: the full LEARN→BUILD→TEST chain runs end to end in
> `make sortlearn-bench` (9/9), with the sort **synthesized from a schema and verified
> by execution**, never hardcoded. The boundary now sits exactly at the edge of curated
> schema knowledge: what has an `algo_shape` is built and checked; what does not is still
> refused honestly. See Track B below.

## The two-track plan

Two tracks, independent and both honest. Track A ships a repeatable demonstration of
the real pipeline immediately and *records* the BUILD gap as a ratcheting discovery
test. Track B builds the BUILD faculty in small, oracle-gated steps until that same
gap closes by itself.

---

## Track A — the honest, repeatable harness (ship now)

Goal: a deterministic test, driven through the pi wrapper, that proves learn +
re-learn + multi-step, and asserts the synthesis gap is *declared*, not hallucinated.
Modelled on `swe-bench` (a discovery instrument that never fakes a solve).

**New test:** `tests/piagent/sortlearn_bench.py`, target `make sortlearn-bench`.

Steps (each an assertion):

1. **forget** — start parrot0 with a KB that has *no* sorting knowledge (empty
   `PARROT0_LEARN_KB`, and a base without `algo.p0`, or a filtered copy). Probe
   "tell me about bubble sort" → must be an honest miss.
2. **relearn via research** — point `PARROT0_WIKI_DIR` at a fixture page
   `tests/piagent/wiki/bubble_sort.md`; probe again → learns + persists
   `wiki_concept(bubble_sort, …)`; assert the fact is in the learned KB and the
   recall answer quotes it.
3. **multi-step via the articulated planner** — one prompt:
   *"tell me what you learned about bubble sort, then write a function add that
   returns the sum of a and b, then compute add(2,3), and finally implement bubble
   sort for an array of integers"* → assert: step 1 recalls the learned concept;
   the `add` step is composed+verified; `add(2,3)=5`; the **sort step is refused
   honestly** (the declared gap).
4. **gap ledger** — print the unmet sub-goal as "next pull: synthesize+run a sort",
   the same way `swe-bench` prints the first unsolved instance. Never fails the build.

Wiring notes:
- The wrapper already sets `PARROT0_TOOLS=1`; the harness must also pass
  `PARROT0_WIKI_DIR` / `PARROT0_LEARN_KB` into the `pi_server.py` child env (extend
  the `env` dict, mirroring the existing `PARROT0_TOOLS` pass-through).
- The planner must route a "tell me what you learned about X" clause to
  `mod_research`/recall. Today `compose_plan` only handles compose/eval clauses; add a
  fallback that dispatches an unrecognised clause through the normal registry (bounded,
  no recursion into the planner) so a recall clause is answered for real.

Deliverable: parrot0 demonstrably *learns and orchestrates*; the wall is explicit and
the test is green and repeatable. This is the honest answer to "show me", today.

---

## Track B — the BUILD faculty (the real pull, small reversible steps)

Goal: turn "implement the algorithm" from a refusal into a *verified synthesis*,
without ever hardcoding the algorithm. Generation = bounded search over structure,
**disposed by `code_run`** on real test inputs. Each step is independently shippable,
keeps `make test` green, and is gated by `PARROT0_TOOLS`.

### B0 — run-grounded verification harness for whole programs ✅ SHIPPED

Before synthesizing anything new, make the oracle usable for array code. Add a helper
that wraps a candidate function with a generated `main` that runs it on fixed test
vectors and `assert`s the expected result, then calls `code_run` and reads the exit
code. This is the "the test passed" oracle; it reuses `code_run` (real cc + execute,
sandboxed). Test: feed it a known-good and a known-bad function, assert pass/fail.
*No synthesis yet — just the judge.*

> **Done (gen209).** `code_check_sort(func_src, fnname, …)` in `src/code.c` (proto in
> `src/code.h`): wraps a candidate `void fn(int a[], int n)` in a generated `main` that
> runs it on 8 fixed vectors (sorted, reverse, dups, negatives, single, empty) and, for
> each, recomputes **sortedness AND permutation** itself before `code_run`. Returns
> 1=all pass / 0=a case failed or crashed / −1=build/run error. The harness owns the
> vectors and the oracle, so a candidate cannot "pass" by printing. Test:
> `tests/check_sort.sh` (a C driver linked against the real `code.c`) feeds it a real
> sort + noop + zero-everything + descending + a build error and asserts every verdict.
> Wired into `make test`.

### B1 — structured algorithm knowledge (KB-first input) ✅ SHIPPED

The synthesizer's input must be **structure, not prose**. Represent an algorithm as
ordered KB steps, e.g. in a new `kb/experts/programming/algo_steps.p0`:

```
algo_shape(bubblesort, nested_loop_compare_swap).
algo_io(bubblesort, array_int, array_int_sorted_ascending).
```

`algo_shape/2` names a **general schema** (not a verbatim body). The schema is the
unit of reuse: many algorithms share `nested_loop_compare_swap`. Crucially the schema
is *knowledge*, queried by the synthesizer, never a C string in the brain.

Bridge from learning: a later step can *induce* `algo_shape` from a learned page that
lists structured steps; until then it is curated, like `algo.p0`.

> **Done (gen209).** `kb/experts/programming/algo_steps.p0` holds exactly the two facts
> above. Loaded LAZILY into `KB_REFLECTIVE` by `mod_compose` (alongside `compose.p0`),
> so it never pollutes ordinary conversation or the introspection counts.

### B2 — schema → C instantiation (the synthesizer) ✅ SHIPPED

A `code_synth_from_shape(shape, name, …)` in `src/code.c` that emits C for a *general*
schema by composition, parameterised by the comparator/operation — so
`nested_loop_compare_swap` with `>` yields ascending sort, with `<` descending, and
the SAME schema instantiates selection-sort-like variants. It must be general enough
that it is plainly not "the bubble sort printed back".

> **Done (gen209).** `code_synth_from_shape(shape, name, comparator, out, sz)` emits
> `void name(int a[], int n)` for `nested_loop_compare_swap`, with `comparator` ('>' /
> '<') the single parameter separating ascending from descending — the body is composed
> here, never a C literal in `brain.c`. Validates `name` as a C identifier; rejects an
> unknown schema with 0.

### B3 — synthesize + verify end to end ✅ SHIPPED

Wire B1+B2+B0: from `algo_shape(bubblesort, …)` emit a `sort(int a[], int n)`, then
**B0 runs it** on several unsorted arrays and checks the output is sorted (and a
permutation of the input). Report the code **only if `code_run` passes**. This is the
first honestly-synthesized, executed-and-checked algorithm.

> **Done (gen209).** `compose_one` (`src/brain.c`) now opens with a sort branch:
> `sort_shape_from_kb` queries `algo_shape(Name, Shape)` and matches the request's named
> algorithm space/underscore-insensitively (so the learned "bubble sort" and
> "bubble_sort" both hit `bubblesort`); on a hit it calls `code_synth_from_shape` then
> `code_check_sort`, and reports the code **only when the judge returns 1**. An UNNAMED
> "sort an array" finds no schema and still refuses honestly (so the gap stays honest
> exactly where no knowledge exists). Verified live and through both benches.

### B4 — repair loop (NEXT PULL — not yet, by discipline)

When B0 fails, read the failing case, form one new hypothesis (e.g. flip a loop bound
or comparator), retry; bounded step cap. This is generative.md's Repair Layer, and the
articulated planner (gen207) is exactly where it hangs: a failed step proposes a fix
instead of stopping.

> **Deliberately deferred.** The curated `nested_loop_compare_swap` schema is correct, so
> `code_check_sort` passes first try — there is **no failing trace to repair**. Per the
> project discipline ("every faculty is pulled from a failing test"), B4 should be pulled
> by a real failure: add a *deliberately wrong* schema variant (or a second algorithm
> whose first emission is buggy) so the judge fails, THEN build the repair step against
> that red. Implementing repair now would be speculative code with no oracle to gate it.

### B5 — close Track A's gap ✅ SHIPPED

Re-point `sortlearn-bench`'s step 3 to now **expect a verified sort**. The ratchet
fires: the same repeatable test that recorded the gap now proves the full
learn→build→test chain — earned, not faked.

> **Done (gen209).** `sortlearn_bench.py` step 4 flipped from
> `multistep-step4-sort-refused-honestly` to
> `multistep-step4-sort-synthesized-and-verified` (asserts the emitted
> `void bubblesort(int a[], int n)` body + "verified by execution" + "permutation"). The
> ledger now reports the closed chain instead of a gap. **9/9 PASS**; `piagent-bench`
> still 14/14 (its unnamed "sorts an array" step stays a correct refusal); `make test`
> green.

## Verification discipline (applies to every step above)

- Nothing is reported "done" until an oracle (`code_eval` / `code_compile` /
  `code_build` / `code_run`) has passed it. Generator proposes; oracle disposes.
- No algorithm body is a C literal in `brain.c`. Schemas live in the KB; the
  synthesizer composes from them.
- Every faculty is pulled from a failing test (Track A's gap ledger drives Track B),
  the same loop as `code-bench` and `swe-bench`.
- `make test` stays hermetic (agent faculties gated by `PARROT0_TOOLS`).

## Status / next action

- **The full challenge is now honestly demonstrable.** `make sortlearn-bench` (9/9)
  drives the whole chain through the pi wrapper: forget → re-learn via research →
  recall → compose+verify `add` → **synthesize a bubble sort from a KB schema and
  dispose it with the run-grounded judge**. Nothing hardcoded, nothing faked.
- Track A (gen208): shipped — planner recall fallback + `sortlearn-bench` harness.
- Track B (gen209): B0 (judge) · B1 (schema KB) · B2 (synthesizer) · B3 (synth+verify
  end to end) · B5 (ratchet flipped) — all shipped and green.
- **Recommended next action:** B4, the repair loop — but pull it from a *real* failing
  trace (a deliberately-wrong schema or a second algorithm whose first emission is
  buggy), not speculatively. Other natural pulls: a second schema (selection/insertion
  sort) to prove `algo_shape` reuse, and the learning→`algo_shape` induction bridge
  (induce structure from a learned page, not just prose).
- Regression status: `piagent-bench` 14/14, `make test` green, `tests/check_sort.sh`
  green. No straggler `pi_server` processes (benches clean up in try/finally).

### Track A — progress (in flight)

- [x] **Probed the forget boundary.** With the agi profile (which `:- include`s
  `algo.p0`) "tell me about bubble sort" is answered from `algorithm(bubblesort,…)`;
  with **no profile** it is a genuine miss: *"I don't actually know about bubble sort
  yet, and I have no source to read on it."* → the harness must boot **without** the
  agi profile so the forget is real.
- [x] **pi_server.py made config-overridable** (`scripts/pi_server.py`):
  `PARROT0_BASE` / `PARROT0_SESSION` / `PARROT0_PROFILE` now honour env (defaults
  unchanged), so the bench can launch with `PARROT0_PROFILE=""` to drop `algo.p0`.
  `PARROT0_WIKI_DIR` / `PARROT0_LEARN_KB` already propagate through `**os.environ`.
- [x] **compose_plan recall fallback (gen208).** A clause that is neither compose nor
  eval (e.g. "tell me about bubble sort") is now dispatched through the registry so
  `mod_research` answers it. `dispatch_one(b, clause, out, sz)` (`src/brain.c`) walks
  `registry[]` skipping `compose` (no re-entry) and `repair` (no clarification),
  forward-declared above `compose_plan` and defined after the registry table;
  `compose_plan`'s else-branch calls it before falling back to "(not yet: …)".
- [x] **`tests/piagent/sortlearn_bench.py` + `make sortlearn-bench`** — fixture
  `tests/piagent/wiki/bubble_sort.md` (shortened Learned Concept so it fits
  `KB_TERM_LEN` whole). Steps: forget (miss, server booted WITHOUT `PARROT0_WIKI_DIR`)
  → relearn (persist + recall, server WITH wiki dir) → multi-step prompt → assert
  recall + add verified + add(2,3)=5 + **sort refused honestly**; prints the gap
  ledger. Two server boots, each in try/finally with `terminate→wait→kill` cleanup
  (verified: no straggler `pi_server` processes, port freed). **9/9 PASS.**
  Regression check: `make piagent-bench` still 14/14, `make test` green.

Resume point: Track A is shipped and green. Next is **Track B / B0** — the run-grounded
verification harness for whole programs (the array-aware oracle a sort needs), the
smallest safe move toward closing the gap this bench records.

## Pointers

- Learner: `tests/research_learn.sh`, `kb/learning/`, `PARROT0_WIKI_DIR` /
  `PARROT0_LEARN_KB`.
- Algorithm knowledge: `kb/experts/programming/algo.p0`.
- Synthesis + oracle: `src/code.c` (`code_eval`, `code_compile`, `code_build`,
  `code_run`), `src/brain.c` (`mod_compose`, `compose_plan`, gen207).
- pi bridge: `scripts/pi_server.py`, `tests/piagent/`, `make piagent-bench`.
- Principles: [PRINCIPLES.md](../../PRINCIPLES.md), [generative.md](generative.md).
