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

### B0 — run-grounded verification harness for whole programs

Before synthesizing anything new, make the oracle usable for array code. Add a helper
that wraps a candidate function with a generated `main` that runs it on fixed test
vectors and `assert`s the expected result, then calls `code_run` and reads the exit
code. This is the "the test passed" oracle; it reuses `code_run` (real cc + execute,
sandboxed). Test: feed it a known-good and a known-bad function, assert pass/fail.
*No synthesis yet — just the judge.*

### B1 — structured algorithm knowledge (KB-first input)

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

### B2 — schema → C instantiation (the synthesizer)

A `code_synth_from_shape(shape, name, …)` in `src/code.c` that emits C for a *general*
schema by composition, parameterised by the comparator/operation — so
`nested_loop_compare_swap` with `>` yields ascending sort, with `<` descending, and
the SAME schema instantiates selection-sort-like variants. It must be general enough
that it is plainly not "the bubble sort printed back".

### B3 — synthesize + verify end to end

Wire B1+B2+B0: from `algo_shape(bubblesort, …)` emit a `sort(int a[], int n)`, then
**B0 runs it** on several unsorted arrays and checks the output is sorted (and a
permutation of the input). Report the code **only if `code_run` passes**. This is the
first honestly-synthesized, executed-and-checked algorithm.

### B4 — repair loop

When B0 fails, read the failing case, form one new hypothesis (e.g. flip a loop bound
or comparator), retry; bounded step cap. This is generative.md's Repair Layer, and the
articulated planner (gen207) is exactly where it hangs: a failed step proposes a fix
instead of stopping.

### B5 — close Track A's gap

Re-point `sortlearn-bench`'s step 3 to now **expect a verified sort**. The ratchet
fires: the same repeatable test that recorded the gap now proves the full
learn→build→test chain — earned, not faked.

## Verification discipline (applies to every step above)

- Nothing is reported "done" until an oracle (`code_eval` / `code_compile` /
  `code_build` / `code_run`) has passed it. Generator proposes; oracle disposes.
- No algorithm body is a C literal in `brain.c`. Schemas live in the KB; the
  synthesizer composes from them.
- Every faculty is pulled from a failing test (Track A's gap ledger drives Track B),
  the same loop as `code-bench` and `swe-bench`.
- `make test` stays hermetic (agent faculties gated by `PARROT0_TOOLS`).

## Status / next action

- Ground truth: **confirmed** (learn works; build is the gap; refusal is honest).
- Recommended next action: **Track A first** (`make sortlearn-bench`) — it makes the
  honest demonstration repeatable now and produces the gap ledger that drives Track B.
- Track B is a real, multi-step pull; B0 (the run-grounded judge) is the smallest
  safe first move and is independently useful.

## Pointers

- Learner: `tests/research_learn.sh`, `kb/learning/`, `PARROT0_WIKI_DIR` /
  `PARROT0_LEARN_KB`.
- Algorithm knowledge: `kb/experts/programming/algo.p0`.
- Synthesis + oracle: `src/code.c` (`code_eval`, `code_compile`, `code_build`,
  `code_run`), `src/brain.c` (`mod_compose`, `compose_plan`, gen207).
- pi bridge: `scripts/pi_server.py`, `tests/piagent/`, `make piagent-bench`.
- Principles: [PRINCIPLES.md](../../PRINCIPLES.md), [generative.md](generative.md).
