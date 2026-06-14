# parrot0 evolution journal

Newest entries on top. One entry per iteration of the loop (see LOOP.md).

---

## 2026-06-14 — gen8: identity & self-reflection ("I know that I am")

**Changed:** `brain.c` → `gen8-self`; new `mod_self`.
- At birth `brain_create` writes the agent into its own KB: `i_am(parrot0).`
  and one `module(<name>)` per **registered** module (reified from the registry
  — self-description can't drift from real structure). These are `reflective`
  facts: regenerated every boot, not meant to persist (DESIGN.md D3).
- `mod_self` answers introspection by *querying that self-model*: "who/what are
  you?" → `i_am(X)`; "do you exist?" → resolves `i_am`; "what can you do?" →
  lists `module(X)`.
- New `tests/cases/self.chat`, incl. the reflexive payoff: "who is a module?"
  resolves over the self-model with the **same** engine as world knowledge.

**Why:** The identity principle (PRINCIPLES.md). The reflexive closure of the
method: the agent gets a model of itself, in the same substrate it uses for the
world, derived from real state (anti-impostor — no `printf "I am conscious"`).

**Observed:** `make test` green (8 cases). Live: "who is a module?" →
memory, self, knowledge, greet, farewell — the agent reading its own structure.
Honest scope: this is the structural *precondition* for self-knowledge; no
claim about felt experience.

**Next:** gen9 — persistence: human-readable knowledge files, base+session
joined at load, save only the session delta, provenance per clause (DESIGN.md
D1–D3).

---

## 2026-06-14 — gen7: induction — learning rules from facts ("training")

**Changed:** `brain.c` → `gen7-induce`; `kb_induce()` in `kb.{h,c}`.
- `kb_induce(min_support)`: ILP-lite. For unary predicates P, Q (P≠Q), if every
  constant with `P(c)` also has `Q(c)` and there are ≥ `min_support` such
  constants, it induces and asserts `Q(X) :- P(X)`. Deterministic order;
  already-known rules are skipped. Helpers `fact_present`, `rule_exists`.
- `mod_knowledge`: trigger words `"generalize"` / `"learn"` run induction and
  report the rules learned (or "Nothing new to generalize.").
- New `tests/cases/induce.chat`: induces `mortal(X):-man(X)` (≥2 examples, no
  counterexample), rejects the reverse (a mortal who isn't a man) and the
  single-example `dog`, then shows the learned rule generalizing to a new
  individual.

**Why:** This is the experiment's deterministic, legible analogue of "training"
(PRINCIPLES.md). The agent now *creates structure* (rules) from data, rather
than being handed it. Rules induced this way generalize to unseen individuals
via gen6 resolution — the anti-impostor property made concrete.

**Observed:** `make test` green (7 cases). A live note worth keeping: with
*symmetric* evidence (every man is mortal AND every mortal is a man among the
known individuals), induction learns BOTH directions — correct given the data,
but a vivid demonstration of **over-generalization from few examples**, the
core ILP/impostor risk. Counterexamples (gen-?) and confidence are the cure.

**Next:** gen8 — identity & self-reflection: reify the agent into its own KB
(`i_am(parrot0).`, `module(...)`) and answer introspective queries from real
state. See PRINCIPLES.md, "I know that I am".

---

## 2026-06-14 — gen6: rules + backward-chaining resolution

**Changed:** `brain.c` → `gen6-rules`; rules + resolution in `kb.{h,c}`.
- `kb_assert_rule(head, body)`: stores definite rules `head(X) :- body(X)`.
- `prove1()`: backward chaining over facts + rules with a depth guard
  (`KB_MAX_DEPTH`) against cyclic rules. `kb_query` now resolves unary goals.
- `kb_match` for unary variable queries now resolves over rules too (walks
  known constants, keeps the provable ones, distinct + in first-seen order).
- `mod_knowledge`: new surface `"every <y> is a/an <z>"` → rule `z(X):-y(X)`.
- New `tests/cases/rules.chat`: derivation, transitive chaining, and
  **generalization to a never-asserted subject** through a rule.

**Why:** This is where parrot0 stops *storing* answers and starts *deriving*
them — the anti-impostor move from PRINCIPLES.md. "Is plato a mortal?" is true
without ever being told, because `mortal(X):-man(X)` + `man(plato)`.

**Observed:** `make test` green (6 cases). Transitive chains and new-subject
generalization both work.

**Next:** gen7 — induction: learn the rules themselves from the facts ("the
training").

---

## 2026-06-14 — gen5: variables + unification

**Changed:** `brain.c` → `gen5-unify`; new `kb_match()` in `kb.{h,c}`.
- `kb_match()`: a pattern where any arg can be a variable (signalled by NULL);
  returns the binding of the first variable for every unifying fact, in
  insertion order.
- `mod_knowledge` rewritten/reordered: `"who/what is a <y>?"` → `y(X)` and
  lists the matches; ground query and assert kept, ordering fixed so a
  question word is never mistaken for a subject.
- New `tests/cases/unify.chat`.

**Why:** Unification is the load-bearing primitive for rules (gen6) and
induction (gen7) — built minimal but correct, with deterministic ordering.

**Observed:** `make test` green (5 cases). "who is a man?" → all men, in order.

**Next:** gen6 — rules + backward-chaining resolution.

---

## 2026-06-14 — gen4: the knowledge base spine begins (ground facts)

**Changed:** `brain.c` → `gen4-facts`; new sub-system `src/kb.{h,c}`.
- `kb.{h,c}`: a ground fact store — `kb_assert` / `kb_query` over atoms like
  `man(socrates)`, closed-world (absent => false), idempotent, growable.
- New `mod_knowledge` (a module that fronts the KB — the first **fractal
  split**: a part with its own sub-system): translates NL ⇄ facts:
  `"X is a/an Y"` → assert `y(x)`; `"is X a/an Y?"` → query `y(x)`.
- `Brain` now owns a `KB *`; create/destroy manage its lifecycle.
- New `tests/cases/facts.chat` locks assert/query, "an", no cross-leak between
  predicates/subjects, and no regression on non-knowledge turns.

**Why:** This is the substrate for the Prolog-like direction (PRINCIPLES.md +
the gen4–gen7 ladder). gen3's name memory was a 1-fact KB; gen4 generalises it
into a real store with closed-world querying. Representation (pred + ground
args) is chosen to admit variables (gen5), rules (gen6), induction (gen7)
without a rewrite.

**Observed:** `make test` green (4 cases). The fractal split happened
*because the domain demanded it* — knowledge earned its own file.

**Next:** See `TASK.md` — gen5: variables + unification, so queries like
"who is a man?" resolve against the facts.

---

## 2026-06-14 — gen3: the first stateful part (session memory)

**Changed:** `brain.c` → `gen3-memory`.
- `Brain` now carries `name` + `has_name` across turns.
- New `mod_memory` (registered first): learns "my name is <X>" (preserving
  original case), recalls on "what is my name?" and phrasing variants, handles
  the not-yet-known case, and supports updating the name.
- Added `copy_trim()` / `skip_ws()` helpers; included `<stdio.h>` for snprintf.
- New `tests/cases/memory.chat` locks it in, including recall *across* an
  intervening greeting (real cross-turn context).

**Why:** Up to gen2 the brain was purely reactive — each turn answered in
isolation. Reasoning needs context that persists. This is the smallest honest
step into statefulness, and it exercises a module reading/writing `Brain`.

**Observed:** `make test` green (3 cases). Greeting between teach and recall
does not disturb memory — modules compose cleanly through the registry.

**Next:** See `TASK.md` — gen4: let a module *decompose* into sub-parts
(fractal articulation), or add light reflection/transformation. One step.

---

## 2026-06-14 — gen2: the brain becomes a system of parts

**Changed:** `brain.c` → `gen2-modular`.
- Introduced a **module protocol**: `Handler` = a part that, given normalized
  input, either claims the turn (writes a response, returns 1) or declines (0).
- Added an ordered **registry** of modules; `brain_respond()` walks it and the
  first claimer wins; otherwise the gen0 parrot echo fires.
- Re-expressed gen1's greeting & farewell logic as two modules
  (`mod_greet`, `mod_farewell`). Behaviour identical, now articulated.

**Why:** PRINCIPLES.md's corollary — structure is functional to emergence. The
brain must be able to become a system of cooperating parts. This is the
*capacity* to articulate; no module taxonomy is pre-designed.

**Observed:** `make test` green with the existing cases unchanged — proof the
refactor preserved behaviour exactly. Adding/removing a behaviour now touches
only the registry table, not the dispatch flow.

**Next:** See `TASK.md` — gen3: the first *stateful* part (session memory),
which also exercises a module reading/writing `Brain` state.

---

## 2026-06-14 — gen1: first intent (greetings & farewells)

**Changed:** `brain.c` → `gen1-greet`.
- Added `normalize()` (lowercase + trim) and `matches_any()` so intent
  matching ignores case and stray spaces.
- Greetings (`hello`/`hi`/`hey`) → "Hi there!", farewells (`bye`/`goodbye`)
  → "Goodbye!". Everything else still falls back to the gen0 parrot echo.
- New `tests/cases/greet.chat` locks in the behaviour (incl. case-insensitive
  + spaces + no-regression echo). Updated `parrot.chat` to test the echo
  *fallback* with non-intent inputs, since "hello" is now an intent.

**Why:** Smallest honest step off pure echo, and a full end-to-end lap of the
loop (TASK → brain.c → tests → journal) to validate the mechanism before we
make the brain articulable.

**Observed:** `make test` green (2 cases); `make chat` greets and bids
farewell, parrots the rest.

**Next:** See `TASK.md` — give the brain the *capacity to articulate* into
cooperating sub-systems (a module registry), per PRINCIPLES.md's corollary.
This is infrastructure for emergence, not a design of the modules themselves.

---

## 2026-06-14 — gen0: bootstrap, the parrot

**Changed:** Created the experiment scaffold.
- `src/main.c`: stable I/O shell — line-based REPL, one stdout response per
  input line, decorative prompts on stderr, `/quit` to exit.
- `src/brain.c` + `brain.h`: the evolving "brain". gen0 (`gen0-parrot`) just
  echoes input verbatim — the most honest zero-agent.
- `Makefile`: `make`, `make chat`, `make test`, `make loop`, `make clean`.
- `tests/run.sh` + `tests/cases/parrot.chat`: conversation test harness using
  a `>` input / `<` expected-response line protocol.
- `LOOP.md`: the self-improvement protocol.

**Why:** Establish the smallest thing that runs, can be talked to (`make
chat`), and can be tested — so every later generation is a small, verified
step on top.

**Observed:** Echo works; tests green.

**Next:** See `TASK.md` — gen1: recognise greetings/farewells instead of
parroting them.