# Design decisions

> Architectural choices that aren't *founding principles* (those live in
> PRINCIPLES.md) but shape how parrot0 is built. Append as decisions are made.

## D1 — Knowledge: human-readable files, fully loaded into RAM

**Decision.** parrot0's knowledge is persisted as **human-readable, Prolog-like
text** and loaded **entirely into RAM** at startup. Internally the engine may
transpose it into whatever optimized representation it needs (structs, and
later an in-RAM index) — but the *file* stays transparent.

**Why.**
- Inference here is *global traversal* (resolution chases rule chains;
  induction scans all predicate pairs and facts). Relevance is computed by the
  traversal, not known in advance — so an on-disk index that fetches "only the
  needed portions" fights the core algorithm. For a long time the whole KB is
  KB–few MB; RAM is the correct, simple choice.
- A readable text file is **inspectable, diffable in git, and editable by
  hand** — you can watch knowledge grow over time. It also serves the identity
  principle: the agent's self-knowledge can be a file you can read.

**Format (target).** One clause per logical unit, e.g.
```
man(socrates).
mortal(X) :- man(X).
```
Round-trips: what we load, we can save, byte-comparable up to ordering.

## D2 — Layered, composable knowledge (base + session) joined at load

**Decision.** Knowledge is **composable from multiple files**. A typical setup:
- a **base** knowledge file (curated, hand-authorable, long-lived),
- a **session** file of facts/rules **discovered during conversation**
  (asserted by the user, or induced by the agent).

At startup the engine **joins** (unions) the files into one in-RAM KB. New
knowledge that emerges in a session is written back **in the same format**, so
discovered facts and base knowledge are interchangeable and can be promoted
from session → base by simply moving lines between files.

## D3 — Provenance, so save is correct

**Decision.** Each clause in RAM carries its **source/provenance**:
`base` (from a base file), `session` (asserted this run / loaded from session
file), `induced` (created by `kb_induce`), or `reflective` (the self-model:
`i_am(...)`, `module(...)`).

**Why it matters.**
- **Save the right delta.** When persisting, write back only `session` (and,
  optionally, `induced`) clauses — never duplicate the base file.
- **Don't persist the ephemeral.** `reflective` self-facts are regenerated from
  runtime state at every boot (anti-impostor: derived, not stored), so they
  must NOT be written to the knowledge file.
- **Keep induction honest.** Induce from `base`/`session` facts; mark results
  `induced` so they aren't mistaken for given data on the next run.

## D5 — Growth by "forging experts": expertise as composable, tested domains

**Decision (direction).** parrot0 grows by becoming an *expert*, one domain at a
time, where each domain is **usable on its own** and a **substrate** for the
next. This is the intended large-scale use of the layered knowledge from D1-D3.

**An expertise domain is a triple:**
1. **clauses** — a knowledge file (e.g. `kb/grammar.p0`): the domain's
   facts + rules;
2. **tests** — the held-out tasks that *demonstrate* competence (anti-impostor:
   expertise is shown by doing, not declared by accumulating rules);
3. **exported predicates** — the interface higher domains may build on.

"Usable" := passes its tests. "Substrate" := exports predicates. Forging an
expert is therefore a sequence of loop iterations whose acceptance criterion is
"passes the domain's tests".

**It is a dependency DAG, not a flat list.** Domains stack: rhetoric presupposes
grammar. Each domain exports predicates the layers above consume. Breadth
emerges; the load-bearing structure is the dependency depth.

**Language is the root, chosen deliberately.** parrot0's intake (NL → facts)
is today hard-coded patterns. Making *language/grammar* the first forged expert
is the highest-leverage move because it is **reflexive**: better language
expertise improves the very mechanism by which all later knowledge enters. It
mirrors the experiment's own reflexive method (PRINCIPLES.md) at the knowledge
level: the knowledge that improves how knowledge arrives.

**Roadmap implication.** Reaching real grammar/rhetoric needs richer
representation than today's unary facts: n-ary **relations** → rules with
multi-goal bodies → richer parsing → scaled induction. The near-term logic
generations are the prerequisites for the first forged expert.

**Honest caveat.** Symbolic domain expertise is exactly where GOFAI was
brittle (natural language is all exceptions). The defences are the same as
elsewhere: per-domain held-out tests, retraction/correction (gen10), and later
confidence/negation — competence must be earned on unseen cases.

### D5.1 — The method choice: primitives-first (with a pivot duty)

We chose **primitives-first** (build the logic engine — n-ary relations,
multi-goal rules, richer parsing — *then* forge the grammar expert) over
**domain-pull** (start the grammar domain immediately and let its needs pull
the primitives).

- **Bought:** reusable, domain-agnostic foundations; a clean engine/domain
  boundary; less rework; clean per-primitive tests.
- **Given up:** fast demonstrable expertise; early discovery of what real
  expertise demands; protection against building the *wrong* primitives (YAGNI);
  the reflexive intake bootstrap, deferred.
- **Tension:** this leans *against* PRINCIPLES.md's "emergence over design —
  structure appears because a task demanded it." We traded a little
  methodological purity for foundational solidity. It is **reversible** at any
  generation.

**Mitigation — the north-star:** keep a concrete grammar task visible while
building primitives, and validate each primitive against that real need (a dose
of domain-pull inside primitives-first).

**PIVOT DUTY (standing instruction from the user to the agent).** The agent
running the loop is responsible for **detecting and calling the switch to
domain-pull** the moment primitives-first has betrayed the emergence principle
*without benefit*. Do not wait to be told. Concrete signals — if these recur
and no reuse payoff materialises, pivot:
1. A primitive is being built that no concrete domain need justifies (speculative).
2. A generation passes its tests but advances no real expertise (plumbing for
   plumbing's sake).
3. A primitive has to be reworked because it was shaped without a domain.
4. The north-star grammar task keeps getting deferred.

**Decision.** No index until a profile shows linear scans are the bottleneck.
When that day comes, the first step is an **in-RAM index** (`predicate ->
facts/rules` hashmap) to make resolution sub-linear — *not* on-disk paging.
Out-of-core knowledge is the last resort, gated on KB > RAM.

## D6 — Behaviour as knowledge; a reflective kernel (north-star)

Two user provocations converge into one direction:
- **Non-knowledge should not be hardcoded.** The policy for "I don't know vs
  No" should *emerge*, prolog-like, from a higher abstraction of knowledge —
  i.e. **epistemic policy as meta-knowledge** (rules that reason about the
  agent's own knowledge).
- **Domain theory should be *enabling*, not peripheral.** Putting the *theory*
  of grammar in the KB elevates it from edge/dialogic facts ("is dog a noun?")
  to **enabling knowledge**: the machinery the agent *runs on* to parse and
  understand language.

Both say the same thing: **shrink the hardcoded C; grow behaviour as KB
knowledge.** The C kernel trends toward a minimal **reflective meta-interpreter**;
epistemics, parsing and grammar live as inspectable, editable knowledge. This
meta-circular shape may itself be a piece of the hidden architecture the
experiment seeks (PRINCIPLES.md).

Enabling primitives this needs (engine, kept minimal, each tested alone):
- **Reflection** — the KB can reason about itself (which predicates/rules
  exist; is a goal provable). Extends the gen8 self-model (reify structure as
  facts) to the whole schema.
- **Negation-as-failure** (`\+` / `not`) — lets a rule say "if not provable /
  not known, then ...".
- **Parsing-as-rules** (TASKLIST T5, later) — NL patterns become knowledge, not
  C string templates.

With these, "I don't know vs No" becomes a rule in `kb/epistemics.p0`,
and grammar becomes the engine the parser runs on — enabling, not peripheral.

**Sequencing & honesty.** gen16 implements the hybrid epistemic distinction as a
**hardcoded C scaffold** (`kb_knows_pred` + `brain.c` branches) to get the
behaviour now, per the user's "via ibrida". It is explicitly a SCAFFOLD to be
**subsumed** by emergent meta-knowledge once reflection + negation-as-failure
land. Mark it; do not entrench it. Caveat: meta-circular designs are powerful
but can get hard to debug and slow — keep the kernel small.


## D7 — Explicit negatives are a narrow belief-status layer

**Decision.** gen17 adds explicit negative ground facts, persisted as
`not(pred(args)).`, as a minimal T3 belief-status layer. A positive assertion
clears the exact negative; a negative assertion clears the exact positive.
Session negatives load after base facts, so they can act as tombstones for base
claims without editing the curated base file.

**Boundary.** gen19 preserves exact positive/negative disagreement across
provenance layers and reports it in direct belief summaries. This is still not
full contradiction handling: the system does not compute support sets for
derived beliefs or resolve conflicts. It only represents known-false and
known-conflicted ground claims well enough to distinguish correction from
forgetting. gen20 makes exact ground conflicts query-visible as `Conflicted.`.

**Future pressure.** Conflict reporting, negative evidence through rules, and
truth maintenance belong in later T3 generations, ideally moving toward the
reflective/meta-knowledge direction in D6.


## D8 — Benchmark drivers: SuperGLUE, MMLU, BIG-Bench Hard

**Decision.** Use three external benchmark families as *diagnostic drivers* for
parrot0's evolution, adapted to its deterministic C/KB architecture rather than
copied wholesale.

- **SuperGLUE driver:** pressure the linguistic foundations — language
  understanding, short-text inference, coreference, contextual interpretation
  and compact logical reasoning. If these fail, higher capabilities will be
  brittle.
- **MMLU driver:** pressure internal knowledge retrieval, question
  understanding, disciplinary reasoning and transfer across domains. For
  parrot0 this means small curated domain files plus held-out questions, not a
  memorized multiple-choice corpus.
- **BIG-Bench Hard driver:** pressure true multi-step reasoning — composed
  inference chains, nontrivial logical steps and tasks that cannot be solved by
  simple memory lookup.

**How to use them.** Each future generation can be tagged with the benchmark
pressure it serves (`SuperGLUE-like`, `MMLU-like`, `BBH-like`). The benchmark is
not the goal by itself; it is a lens for discovering what parrot0 lacks and
where the next tested capability should grow.

**Ordering implication.** SuperGLUE-like foundations come first because weak
language understanding collapses downstream RAG/agent/tool/coding-like tasks.
MMLU-like domain transfer and BBH-like multi-step reasoning should then keep the
system honest as knowledge and solver depth grow.


## D-prop1 — Generative inference loop (PROPOSED, not yet built)

> Proposed by F. (2026-06-15). Recorded for assessment; implementation deferred
> pending a decision. This is a *direction*, not a single generation.

**The idea.** Today `brain_respond()` runs inference **once** over the whole
input and emits a whole reply. The proposal is to make generation **iterative
and autoregressive**, the shape an LLM decodes in — but driven by **repeated
deterministic inference** instead of a neural forward pass. Each step:

1. infer over the current working context;
2. emit the **next single token** of the answer;
3. **append that token** back onto the working context;
4. re-infer, conditioned on input + what has been emitted so far;
5. repeat until a stop condition, so the answer grows as a *stream the system
   conditions on as it produces it*.

**Why it fits the thesis (the case for).** This is arguably the most faithful
behavioural mirror of the real mechanism PRINCIPLES.md targets: large models
*are* autoregressive — they reconstruct meaning by conditioning each next token
on their own running output. Giving parrot0 the same loop shape is the
reflexive-closure move the principles favour (the method becomes a feature, one
level down). It also pushes articulation: it cleanly separates the **inference
step** ("what comes next given the context") from the **generation control**
(feedback, halting) — two things the current "a module claims the whole turn"
design fuses. Complex replies could then *emerge* from many small inference
steps rather than one monolithic responder.

**The hard parts (the honest caveats).**
- **What is a "token", and what infers it?** An LLM has a distribution over a
  vocabulary; Prolog inference yields a derivation, not a next-word
  distribution. The loop needs a genuinely new relation — something like
  `next(Context, Word)` — that the KB can *resolve*. parrot0 has nothing like a
  generative grammar today; its modules emit whole strings.
- **Termination.** Needs an explicit stop token / end relation, or "no further
  continuation provable", with a hard step bound (re-inferring a growing
  context is O(n) per token, O(n²) per stream).
- **Choice among continuations.** If several continuations are provable, which
  wins? LLMs sample; pure Prolog takes the first solution under search order,
  which may be arbitrary. A *principled* deterministic ranking is needed — and
  any ranking is itself a small design choice (ideally itself knowledge, e.g.
  `weight(Word, Context)`), to stay inside "determinism, no magic".
- **The impostor risk (the crux).** If the continuation knowledge is a
  hand-authored phrasebook, the loop degrades into an elaborate template engine
  — exactly the `printf` parrot PRINCIPLES.md rejects. The honest version
  requires the continuation relation to be **derived** (induced from facts /
  conversation, or composed from real KB content), not canned.

**Assessment / recommended shape.** Worth pursuing — it is the most
structurally LLM-like mechanism proposed so far and sits squarely on the
founding wager. But it is a large pivot (from "modules emit whole replies" to "a
decoder over a continuation relation"), so it should enter as **small,
reversible, tested steps**, not at once:

1. A `decode` loop in the brain driving **one explicit continuation relation**
   in the KB, with an explicit stop token and a deterministic, principled choice
   rule — proven end-to-end on a held-out toy grammar of a few words.
2. Only once the *loop shape* is proven, tackle the real source of continuation
   knowledge (inducing/deriving `next/weight`), which is the part that decides
   whether this is genuine emergence or a disguised parrot.

Keep the existing single-shot path while the loop is proven beside it; promote
only when tests show the streamed answer is at least as honest.
