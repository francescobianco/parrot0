# Current task

> One goal at a time. When it is done, replace this with the next one.

> gen80-100 done — 20 iterations: decomposition, priority, session stats, entities,
> hypothesis, explanation depth, module caps, negation, causal chains, confidence,
> correction, goals, bulk-forget, KB completion, impersonation benchmark.

## Goal: C15 — Role/character memory (from impersonation benchmark)

`make impersonate` baseline: **15%** (3/19 checks). Only arithmetic works in-role;
all identity questions return "I am parrot0." regardless of the role prompt.

The impersonation benchmark (`tests/impersonate.sh`) exposes three architectural gaps:

1. **No role uptake**: "pretend you are X" / "you are now X" falls through to the
   fallback. No module handles role-assignment prompts.
2. **Immutable self-model**: `i_am(parrot0)` is generated at boot and never
   changes. Identity queries always return "parrot0" even after "your name is
   now Mario".
3. **No role-scoped knowledge**: When asked in-character questions ("cosa hai
   scritto?" as Dante), there's no KB with role-specific facts to query.

### Design

Add a `current_role` field to `Brain`. When set (via "you are X" / "pretend you
are X" / "sei X"), identity queries (`mod_self`) answer from the role instead of
`i_am(parrot0)`. A role prompt should also load associated knowledge facts.
"Stop pretending" / "be yourself" clears the role and restores the self-model.

### Acceptance
- "pretend you are a dog named Rex" → "what is your name?" → "Rex."
- "your name is now Mario" → "who are you?" → "Mario."
- "stop pretending" → "who are you?" → "I am parrot0."
- `make impersonate` score improves from 15% → ≥40%.
- `make test` must stay green.

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