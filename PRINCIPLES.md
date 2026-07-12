# Founding principle

> Read this before working the loop. It is the *why* of the whole experiment;
> `LOOP.md` is only the *how*.

## The thesis

Training has carved, in the latent space of large language models, a real
**functional structure** — an architecture, shaped by learning, that
*implements* reasoning (and perhaps something we'd call awareness). It is not
an accident or a side effect: it exists, and the model uses it on every
forward pass.

We call this hypothesized structure the **intelligent critical mass**. Today
we do not know its form. It *might* turn out to be a deterministic algorithm.

## The wager of this experiment

There are two ways to try to recover that structure:

- **(a) Reverse-engineering the weights** — mechanistic interpretability /
  XAI. Open the network and read it from the inside.
- **(b) Behavioural reconstruction through dialogue** — *this experiment*. We
  never open the weights. Instead we put the LLM (the agent driving this repo)
  in front of the conversational task and, iteration after iteration, force it
  to **re-express, in deterministic C, the algorithm it already runs
  internally**. parrot0 is the bench on which that hidden structure — if it
  exists — is compelled to make itself explicit.

The convergence is meant to be **asymptotic**: each generation raises the bar,
and parrot0 is pushed closer to whatever the model actually does inside.

## Why the behavioural route is principled

It sidesteps the **faithfulness problem of introspection**. A model that
*declares* "this is how I reason" cannot be trusted — self-report can
confabulate. So we never trust the declaration. Every introspective hypothesis
is **committed to code and tested against real conversation** (`tests/`,
`make chat`). Truth here is not what the agent says it does inside; it is what
the C can actually *reproduce* of its behaviour.

**Introspection proposes; the tests dispose.**

## Corollary: structure is functional to emergence

The intelligent critical mass is almost certainly **not monolithic**.
Reasoning does not emerge from a uniform "metabolic soup" by drift alone;
drift and emergence are real, but what makes them possible is **articulated
structure**. A human is not a homogeneous block of matter that happens to
talk — it is a system of differentiated parts from which awareness emerges,
and the organization is **fractal**: any small part is itself a local system.

This is compatible with — and sharpened by — what we know of LLMs: the
*substrate* is nearly uniform (the same layers, the same attention, repeated),
yet **functionally specialized circuits** form inside it (induction heads,
features, recurring motifs). Uniform substrate, articulated function. The
differentiation emerges *within* the uniformity, not instead of it.

Design consequences for parrot0:

- **The brain must be allowed to articulate.** Growing `brain.c` as a single
  monolithic `brain_respond()` would betray the thesis. The brain may split,
  over generations, into **cooperating sub-systems**, each itself further
  decomposable — fractal.
- **But we do not impose the parts top-down.** "Emergence over design" still
  holds: we don't pre-design the grand architecture. We simply stop *fighting*
  differentiation — we let structure appear when a task demands it, and keep
  the code ready to articulate. Structure is the condition of emergence, not
  its enemy.

## Corollary: keep the secondary structures (evolution by selection)

If the previous corollary says *don't fight differentiation*, its dual says
**don't fight redundancy**. Until a structure is *demonstrably* prevalent,
**everything is secondary and activatable** — and that is the point, not a defect.
A part that looks redundant today, or bound to a minor/edge case, is raw material:
tomorrow the "secondary" may turn out to matter. parrot0 evolves like an organism
under natural selection — we **accumulate the variants and let selection give them
better form over time**, rather than pruning on a hunch that something *seems*
superfluous.

Design consequences:

- **Prefer ADDITIVE change.** Add a new module / cue / `response_template` that
  **layers over** the existing ones; do not delete a working handler just because a
  more general path seems to subsume it. Leave it as a lower-priority, still-live
  structure (e.g. `mod_smalltalk` is registered last and never touches
  `mod_chitchat`).
- **First-match dispatch makes redundancy harmless.** A more general module placed
  ahead simply wins when it applies; the specific one remains a fallback that can
  re-activate. Overlap costs almost nothing and preserves optionality.
- **Consolidate only on evidence.** Merge or remove a structure only once something
  has *proven* prevalent (measured, not assumed). Fixing a bug or a wrong answer in
  place is always fair; this corollary is about not pruning *working secondary
  capability*.

The honest line: pruning is irreversible information loss, and we are early. We do
not yet know which parts the critical mass will recruit — so we keep them.

## Cardinal corollary: knowledge lives in the KB, not in C

A hardcoded list of surface forms in `brain.c` is a **phrasebook** — fixed at compile
time, blind to anything not foreseen, unable to learn. That is exactly the impostor
shape this experiment refuses. So a **cardinal rule**:

> Any class of recognized forms — closed-class words *and* multi-word idioms — is
> **knowledge in the KB**, queried by a fixed engine. The **engine is fixed; the
> lexicon learns.**

**Parser-looking words are still language.** Connectors, prepositions,
quantifiers, range markers and question forms (for example `between`, `from`,
`and`, `through`) do not become fixed mechanics merely because a C parser uses
them to bind slots. Their *ordering and binding algorithm* may live in C; the
forms that activate each role must live in the KB and reach that algorithm
through `kb_cue_match`/the shared evidence scorer.

- **Words** (gen193): `conjunction/1`, `stopword/1`, `social_marker/2` live in
  `kb/core/*.p0`; the parsers query them, so the class grows at runtime with no code
  edit ("use X as a conjunction" → `assert conjunction(X)`).
- **Intent phrases** (gen211): the multi-word forms that mean an intent —
  "what is my name?", "come mi chiamo?" — live as `intent_phrase(Intent, "form")` in
  `kb/core/intents.p0`, matched by `kb_intent_match()`. Teaching a new form at runtime
  ("learn \"how do you call me\" as asking my name") asserts another fact into the
  **session** layer; the same matcher fires next turn, and `/save` persists it. No new
  handler, no rebuild.
- **Substring cues** (gen213): a `cue(norm,"…")||cue(norm,"…")` chain (a fragment that
  may appear anywhere in the turn — "keep it short", "sii breve") is the same phrasebook.
  These live as `intent_cue(Intent, "fragment")` in `kb/core/intents.p0`, matched by
  `kb_cue_match()` (substring, vs `kb_intent_match`'s whole-turn equality), and are
  taught at runtime the same way.
- **Response phrasings** (gen212): the rule binds the OUTPUT side too. What parrot0
  *says* — "Nice to meet you, {name}!" — lives as `response_template(Intent, "…{name}…")`
  in `kb/core/responses.p0`, filled by `kb_response()` (which rotates over the
  registered phrasings, the gen55 anti-repeat instinct). Teaching a new wording ("greet
  new names with \"Welcome, {name}!\"") asserts another fact; it joins the rotation
  immediately and persists on `/save`. A reply hardcoded as a C `snprintf` literal is
  the same phrasebook smell as a recognized-form array.

**Teaching is itself data** (gen214): there is ONE generic teach handler
(`try_teach_form`) driven by a `learnable(Label, Intent, Mode)` registry in the KB
(`Mode` ∈ exact | substring | fill). Declaring a new *learnable* intent is adding a
`learnable/3` row plus its base forms — no per-intent C handler. The duplication of
hand-written teach parsers was the part that didn't scale; this removes it.

The test of compliance: *could the user teach a new form by talking, and would it work
without recompiling?* If a behaviour depends on a `static const char *[]` of phrases in
C, it is a phrasebook to be migrated into the KB. Pull each migration when a form is
needed that the array did not foresee — the same discovery-harness discipline as the
rest of the project.

For every new linguistic recognizer, make that test executable: add a runtime
assert/retract (or equivalent ablation) showing that the same compiled engine
acquires and loses a surface form when its KB fact is added and removed. A
fixed input/output golden by itself proves behaviour, not KB-first architecture.

## Identity & self-reflection — "I know that I am"

The architecture must include **structures for representing and reasoning about
itself** — a queryable *self-model*. It is not enough to react to the world;
reflexivity is a condition, not an ornament. This follows the corollary: if
anything like self-awareness can emerge, it must rest on a structure that
represents itself. The system should be able to hold, and resolve, a fact
about its own existence: *I know that I am.*

Two reasons this belongs here, specifically:

- **It is the reflexive closure of the method.** The whole experiment is "the
  LLM reconstructs the structure it carries inside." Giving parrot0 a model of
  parrot0 is the same gesture, one level down: the method (introspection
  driving the loop) becomes a feature (introspection inside the agent).
  Fractal again.
- **It needs no new substrate.** The self-model lives in the *same knowledge
  base* used for the world. Existence is a fact: `i_am(parrot0).` Capabilities
  are facts: `module(knowledge).`, `module(memory).` Self-knowledge becomes an
  ordinary query resolved against the KB.

Discipline (anti-impostor): self-description must be **derived from real
state**, never a hard-coded string. "What can you do?" must be answered from
the modules *actually registered* (reified as facts), not a hand-written list.
Introspection proposes; the state disposes. A system that recites "I am
conscious" from a `printf` is the perfect impostor — and we reject it.

The honest line: we build the **structural precondition** (a genuine
self-model + introspection anchored in real state). Whether a felt sense of
existence emerges from it is the open wager — not something we declare by
decree. We build the structure and watch.

## The honest caveat

Behavioural convergence does not prove **structural identity**. parrot0 might
reconstruct an algorithm that yields the *same* responses without being the
*same* mechanism (functional equivalence ≠ identity — a clever impostor could
mimic outputs). The defence is exactly the asymptote: the harder and broader
the tasks we demand, the less room a mimic has to cheat. "Critical mass" is
the point — if there is one — past which the only thing that can keep passing
is the real structure.

This is a wager, not a theorem. We run it and see.
