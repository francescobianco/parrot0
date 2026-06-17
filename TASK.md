# Current task

> One goal at a time. When it is done, replace this with the next one.

> gen80-101 done — decomposition, priority, session stats, entities, hypothesis,
> explanation depth, module caps, negation, causal chains, confidence, correction,
> goals, bulk-forget, KB completion, impersonation benchmark, role memory.

## Done recently

- **gen101 (C15) — role/character memory.** `make impersonate` 15% → **100%**.
  `mod_role` parses role uptake from the user's words, answers in character from
  role state + `knowledge/roles.pl`, and keeps a **layered self-model**: a
  truth-probe ("really" / "davvero") pierces any role back to parrot0.
  (I-series I1–I4 done.)
- **gen102 (L11) — structural analogy.** `mod_analogy` solves "A is to B as C is
  to ?" by finding a KB relation and transferring it — derived, not stored;
  both directions; bilingual; held-out triples pass. First answer parrot0 was
  never told.
- **gen103 (L16) — self-correction that re-derives.** A correction that flips a
  previously-stated conclusion is recomputed and the consequence volunteered
  ("no longer a mortal"). A core parser fix (trailing `?` = query, any word
  order) makes it bilingual through one path.

### Next candidate — pick the smallest high-surprise rung

The **L-series** (TASKLIST.md) maps a 20-rung ability ladder onto parrot0. The
honest next pulls, where behaviour cannot be templated, are:
- **L10** few-shot pattern induction in one turn,
- **L1** streamed generation over a continuation relation (D-prop1),
- **L20** meta-strategy introspection ("why did you answer *that way*?"),
- **L17** one-step algebra / word problems.

Choose one, smallest-first, drive it from a held-out test, keep `make test` green.

---

## Completed benchmarks

### impersonate (gen100 baseline)
`make impersonate` — 10 scenarios, 19 in-character question checks.
Baseline: 3/19 (15%). Passing checks are accidental (arithmetic always works;
"what are you really?" returns parrot0). Zero genuine impersonation.

Gaps found → C15, C16, C17, C18 in TASKLIST.md.

---

## Completed (gen75-100)
- gen75-79: flexible arith, proof traces, introspection, module tracking, emergent induction
- gen80-81: turn decomposition, priority operators, but/ma negation
- gen82-83: session stats, named entity detection
- gen84: hypothesis mode (suppose X, then Y)
- gen85-88: explanation depth, per-module caps, negation of intent
- gen89: session knowledge introspection
- gen90: causality chaining (transitive cause/effect)
- gen91: confidence check from proof trace
- gen92: correction acknowledgment
- gen93: goal tracking
- gen94-100: scaffolding, impersonation benchmark