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
1. **clauses** — a knowledge file (e.g. `knowledge/grammar.pl`): the domain's
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

**Decision.** No index until a profile shows linear scans are the bottleneck.
When that day comes, the first step is an **in-RAM index** (`predicate ->
facts/rules` hashmap) to make resolution sub-linear — *not* on-disk paging.
Out-of-core knowledge is the last resort, gated on KB > RAM.
