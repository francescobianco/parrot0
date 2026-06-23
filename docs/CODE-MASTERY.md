# Code mastery — the hidden process to recover

> **Theory layer.** PRINCIPLES.md sets the wager: re-express in deterministic C
> the algorithm the LLM already runs internally. This document asks that question
> for **one faculty — handling code on par with a modern LLM coding agent** — and
> answers *why* it can cohere with parrot0's existing language power, *which hidden
> process* we are actually chasing, and *how* to re-express it in C.
>
> This is the **why**. The phased *what to build* lives in
> `docs/plans/coding-agent.md` (infrastructure: filesystem, exec, parser, codegen,
> phases A–E) — a useful *part inventory* that is **subordinate to this theory**:
> read it as the parts, read this first for the direction that should *order*
> them. (An earlier `code-generation.md` proposed a template/phrasebook codegen
> path; it was removed as superseded — §4 explains why templates are the wrong
> engine.)

---

## 1. The question, in the experiment's terms

The naive view is that "coding" is a separate skill bolted onto a language model:
a code module, a parser, a generator. If that were the LLM's secret, parrot0 would
just need more modules.

It is not the secret. The thing to recover is **why an LLM is coherent across
natural language and code at all** — why the same machine that discusses the heart
can also read, reason about, and rewrite a function, *without a seam between the
two*. That coherence is the hidden structure PRINCIPLES.md tells us to make
explicit. If we recover it, codegen, debugging, and review fall out of it. If we
only build the modules, we get a phrasebook with a parser bolted on.

## 2. Central thesis — one substrate

An LLM handles code coherently because **NL and code occupy one representation
space**: the same token sequence, the same vocabulary, the same next-token
prediction. There is no "code subsystem" — there is one substrate, and code is a
dialect within it. Coherence *is* the unification.

The coherent analog for parrot0 is therefore not "add a code subsystem" but
**make code enter the same KB substrate the language already uses**. Today
parrot0's lexical/grammatical power turns text into facts — `concept/…`,
`part_of/2`, `wiki_concept/3` — and reasons over them with one rule engine. The
path to code is to make the *same* machinery:

1. **ingest** source like it ingests Wikipedia markdown (static files, no
   outsourced intelligence — see `dynamic-knowledge-direction`),
2. **assert** facts about that source (AST nodes, dataflow, calls, types),
3. **reason** over them with the *same* SLD engine and derivation rules
   (gen157's "derive a relation from structure" *is* a static analyser),
4. **reverbalize / generate** back out.

> **Code is just another corpus to learn; the AST is just more KB.**

This is the single load-bearing idea. Everything below is its consequence.

## 3. The hidden process, decomposed

What an LLM does in one opaque forward pass, decomposed into the faculties we must
make explicit. For each: what the LLM does, the C re-expression, and — most
importantly — **the part that is genuinely unknown to us today** (the thing the
experiment exists to surface).

### F1 — Lexing
- **LLM:** sub-word (BPE) tokens over a vocabulary shared by prose and code.
- **C:** a language-aware lexer feeding a common token-fact form, exactly as the
  NL tokenizer already feeds normalization.
- **Status:** near parity. Not the hard part.

### F2 — Grammar / structure
- **LLM:** an *implicit*, statistically-learned grammar.
- **C:** an **explicit** grammar (recursive descent / PEG) → AST → assert
  `node(Id, Kind, Parent, Pos)` facts.
- **Recovered insight:** here parrot0 has a **structural advantage**, because a
  programming grammar is *formal and decidable*. This is the one faculty where
  primitives-first dominates outright: you do not need to *learn* the grammar
  statistically — it is given. The LLM pays (in parameters and data) to
  approximate something parrot0 can simply *know*.

### F3 — Semantics / behaviour
- **LLM:** a learned mapping from code to its effect.
- **C:** operational **rules over AST facts** — derive `assigns/2`, `calls/2`,
  `reads_before_write/1`, `type_of/2`. Static analysis is the rule engine pointed
  at code facts.
- **Unknown to recover:** how far *deductive* semantics reaches before it needs
  the LLM's *associative* semantics (the "I have seen this idiom" shortcut). This
  boundary is a primary measurement of the experiment.

### F4 — Intent ↔ code (the bridge)
- **LLM:** one shared embedding space makes "sort a list" and the loop that does
  it neighbours.
- **C:** a **shared KB vocabulary** — an NL concept and a code construct asserting
  into the same predicate space and linking; a growing corpus of *idiom/pattern*
  facts learned from **static** code (coherent with reading static files, never an
  API).
- **Unknown to recover — this is the crux.** The fuzzy, robust mapping from a
  novel phrasing to the right construct is exactly the part an LLM gets "for free"
  from compression and we do **not** know how to do deterministically. This is
  where the experiment will either find a structural mechanism or prove a wall.

### F5 — The edit loop with grounding
- **LLM agent:** read → plan → edit → run → verify → iterate.
- **C:** the skeleton **already exists** — the gen160–169 reflexive arc (recognise
  a composition challenge, derive the parts from the self-model, emit a skeleton,
  **run** it on a fresh copy, **audit** the result). Extend it to code: locate a
  node by KB query → express the edit as a transformation rule → rewrite the AST →
  re-emit source → **verify by compiling/running** → iterate on the observed
  error.
- **Recovered insight:** the agent loop is not new infrastructure to invent; it is
  an *existing* parrot0 behaviour retargeted at code.

## 4. Where pure structure wins, where it must invent

**Wins (lean in):**
- **Formal grammar** (F2): decidable, no statistics needed.
- **Grounded verification:** a compiler / runtime is a *deterministic tool*, not
  outsourced intelligence — using it is as legitimate as reading a static file
  (`dynamic-knowledge-direction`). Where an LLM *guesses* behaviour, parrot0 can
  *derive and then check it for real*. Code is the rare domain where "understood"
  has an objective, mechanical oracle.

**Must invent (the open frontier):**
- **Fuzzy intent↔code** (F4) and **idiom generalization** — the associative
  shortcuts. The tempting wrong move is to *simulate* the statistics with a
  phrasebook. The disciplined move is to refuse the fake and lean on what code
  uniquely offers: verifiability. Let structure + the oracle carry the weight the
  LLM carries with learned association, and *measure where that is not enough*.

## 5. The honest reframe

> An LLM masters code by **compression of examples**.
> parrot0 must master code by **structure + grounded verification**.

Same destination — manipulate correct code — by a different engine: statistical
vs. structural-deductive. We do not fake the LLM's engine; we build the other one
and find out how far it reaches. That "how far, and why it stops" is not a
failure mode of the project — per PRINCIPLES.md it is the *deliverable*.

## 6. Why this is the sharpest test of the whole wager

Conversation lets the impostor hide: a fluent wrong answer can pass for
understanding. **Code does not.** It compiles or it does not; the test passes or
it does not. So the code faculty is the experiment's cleanest instrument: the
exact point where pure-symbolic re-expression stops keeping up with the LLM will
be *visible and mechanical*, not a matter of taste. Whatever the outcome —
structure suffices, or some faculty provably needs a statistical model — code is
where the answer will be unambiguous.

## 7. First step, and how it orders the roadmap

Per the discovery-harness method (`superglue-discovery-harness`,
`oracle-is-behavioural-signal`): **do not hardcode the roadmap; pull the next
feature from real pressure.**

- **`make code-bench`** — submit minimal code snippets and watch *behaviour*, not
  just correctness.
- **First generation:** ingest a snippet, parse it (deterministically), assert AST
  facts into the *same* KB, and answer **one structural question honestly from
  derived facts** ("what functions are defined?", "what does `foo` call?"). No
  generation yet — first *see code as KB*. Then grow **one faculty per
  generation** toward the F5 edit-and-verify loop.

**Relation to `coding-agent.md`.** That plan is a sound feature inventory, but it
front-loads infrastructure (filesystem, real exec) as Phase-ordered prerequisites.
This theory says the *order* should be pulled by benchmark pressure, not by a
dependency chart: build a piece only when a failing `code-bench` stimulus demands
it (the pivot duty — `primitives-first-pivot-duty`, DESIGN.md). Concretely, F2/F3
(AST-as-KB + structural reasoning, `coding-agent.md` B2–B4) come first because
they need no new "hands"; filesystem and exec (B1, C2) arrive only when a stimulus
cannot be answered without them. The theory chooses the order; the plan supplies
the parts.

---

## 7b. Language-as-delta — the KB-first growth law

A growth law, sharper than "add a Python parser". A programming language is itself
a **concept**, so it obeys the KB-first principle (PRINCIPLES.md, LOOP.md
"Acceleration" lever A): we do not *learn* each language from scratch — we model
the **shared computational substrate once** and declare every language as a
**delta (mirror concept) by reference**, letting inference cover the shared part.

- **The shared substrate is an abstract node vocabulary.** Analysis rules (F3:
  `calls/2`, `assigns/2`, `reads_before_write`, `type_of`) must speak `node(Id,
  function_def, …)`, `call`, `block`, `assign`, `cond`, `loop` — **not** C. Then
  localization, call-graph, blast-radius, dead-code transfer to ANY language for
  free. A new language costs only a concrete-syntax **front-end** that emits the
  same abstract facts, plus its language-specific semantic facts/rules.
- **A language is a mirror concept.** `like(python, <abstract>)` with overrides
  (indentation blocks vs braces, dynamic vs static typing, `def` vs type+name) and
  genuinely-new facts (generators, decorators, comprehensions). Inference is a
  **query-time delegation walk** (own facts shadow the reference; cycle-guarded),
  never copied rules — so the mirror stays *bound and live*: change the reference,
  the mirror tracks it except where overridden. The same mechanism scales down to
  a single TYPE ("in this C variant `casualint` is `like int` but 3 bytes").
- **The honest frontier (measure, don't promise).** "Inference does the rest" is
  true only for the SHARED abstractions. Concrete syntax is written, not derived
  (Python's significant-whitespace grammar is a different parser — but it only
  needs to emit the shared node vocabulary; the analyzers stay put). And some
  deltas are net-new semantics with no reference counterpart — additive, but their
  own rules. *How much transfers vs. how much is language-specific is the
  deliverable* (§6). Where the delta is uncertain, the real interpreter/compiler
  is the **oracle**: derive aggressively, then check.
- **Open question (do not over-commit).** We do not know whether, inside an LLM, a
  *protolanguage* forms or the system **collapses onto a known language and
  minimizes by differential**. Either way the engineering is the same here, and
  **C is a fine metalanguage** for now — the abstract vocabulary need not be a new
  notation, only a reference the deltas point at. Build the substrate + deltas; let
  the benches reveal where the analogy stops.

**Design imperative this implies:** before "add a language", factor the code engine
into (a) per-language concrete-syntax front-ends emitting a shared abstract
node/fact vocabulary, and (b) language-agnostic analysis rules over it. The first
swe-bench pull is therefore not "Python" but *ensuring the inference does not speak
C*. (Recorded as a task in TASKLIST.md.)

---

## 8. North-star benchmark — `make swe-bench`

The chapter's external measure is **SWE-bench** (https://www.swebench.com): real
GitHub issues where an agent must produce a patch that makes the repository's own
test suite pass. It is the honest scoreboard for "handles code like an LLM coding
agent" — and it scores exactly the thing the impostor cannot fake (the patch
either passes the hidden tests or it does not, §6).

**It is a north star, not a near step.** A real SWE-bench run needs faculties
parrot0 does not have yet: read arbitrary files (F-filesystem), localize across a
repo (F4 intent↔code), produce a multi-file patch, and run the test suite in a
sandbox (F5 grounding, real exec + Docker). Building `make swe-bench` before those
exist would only measure zero. So it arrives **after** the F2→F5 ladder, pulled in
when `code-bench` stimuli start requiring repo-scale localization and patching.

**Two things we build into it from the start:**
- **Prompt interception / trace.** Every instance's problem statement that we feed
  parrot0, and its response, is captured (a `PARROT0_TRACE`-style log, cf.
  `superglue-discovery-harness`). The harness reports *where* parrot0 behaves
  badly — which instances, which faculty failed, the exact input — so each failing
  cluster becomes the next `code-bench` stimulus and the next pull. The benchmark
  is thus both a scoreboard *and* a discovery instrument.
- **A degrade path.** Before full patch+test scoring is possible, `swe-bench` runs
  in a *behavioural* mode (like `code-bench`): present the issue, capture the
  response, score sub-goals we *can* check (did it name the right file/function?),
  and record the rest as gaps. Ranking/quality numbers become meaningful only once
  the F5 loop closes; until then the value is the intercepted-failure map.

---

## 9. One-line summary

Recover the LLM's coherence by **unifying code into the KB substrate** (AST-as-KB),
reason with the engine that already exists, **ground meaning in real
compilation/execution**, and let the discovery harness reveal the exact frontier
where structure + verification stops matching statistical association — that
frontier is the experiment's prize.
