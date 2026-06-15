# TASKLIST - proving ground for parrot0

This file is not the current single-generation task. `TASK.md` keeps that role.
This is the longer proving ground: complex capabilities and tests that should
force parrot0 to expose more of the hidden architecture that implements general
intelligence.

Mission: make the architecture emerge under pressure. Each task below is a
behavioral demand, not an architectural decree. The hypotheses describe what
may need to evolve, but the loop should still choose the smallest tested change
that earns the next capability.

Acceptance discipline:
- Every task must end in executable tests, not only a demo conversation.
- A capability is accepted only when it transfers to held-out examples.
- Prefer tasks that require composition of previous capabilities.
- If an implementation passes by memorizing fixed strings, the test is too weak.
- If a task forces speculative plumbing with no domain payoff, pivot back to a
  domain-pulled version of the task.

---

## C-series — Conversational intelligence (TOP PRIORITY from 2026-06-15)

Why this jumps the queue: the SuperGLUE bench is *comforting but misleading*.
After gen45–gen49 it reads 46% overall with zero invalid — yet that score comes
from single-turn classification with the context handed in, plus shallow
lexical-overlap baselines and entity salience. In an actual chat the bot feels
**immediately unintelligent**. Bench accuracy and felt intelligence are
different axes; we optimized the measurable one and the experience did not move.

Observed chat (2026-06-15), one honest session:
- "what is your name?" -> "I don't understand that yet."  (but "who are you?"
  -> "I am parrot0." — only the exact template matches)
- "what can you do?" -> dumps internal module names (jargon, not capabilities)
- "I have a dog named Rex" / "what is my dog called?" -> not understood (only the
  rigid "X is a Y" assertion shape parses)
- "how are you?", "thanks", "tell me about yourself", "2+2",
  "what did we just talk about?" -> "I don't understand that yet."
Diagnosis: parrot0 answers only a narrow set of rigid templates; **everything
else hits the "I don't understand" wall**, which is the dominant first
impression. The reasoning core is real but unreachable through natural language.

Strategy: make *chat* the primary instrument, not the bench. Drive the loop from
real conversational failures (domain-pull from dialogue). Keep the no-phrasebook
discipline: robustness must come from role/keyword recognition and the existing
KB, not from enumerating fixed strings.

### C0 - A held-out conversation benchmark (DONE, gen50)
The anti-self-deception tool. Built: `tests/chatbench.sh` + `tests/chat/*.dlg`
(soft substring, normalized, multi-turn scorer) and `make chat-bench`, reporting
a felt-intelligence %. Baseline at gen50: **36% (4/11 turns)**. Without this we
would keep mistaking bench points for progress. Still TODO: more held-out
dialogues that vary names/order, and tighten the metric if it over-credits.

### C1 - Paraphrase-robust intent (DONE for identity+capability, gen51)
The #1 felt-intelligence bug. Done for identity and capability: `cue()`-based
intent over many phrasings (EN + IT), answered from the self-model; capability
now plain-language. Felt-intelligence 36% → 64%. Held-out phrasings pass
(`intent.chat`, `intent.it.chat`, `identity_paraphrase.dlg`). Still TODO: extend
the cue approach to the other intents as they arrive, and watch for cue
collisions (Decision D-2026-06-15u).

### C2 - Social register, in plain language (DONE, gen52)
Done as a dialogue-act layer `mod_social` (Decision D-2026-06-15v): phatic moves
recognized by communicative ACT — closed-class markers + discourse position
(same "ciao" opens early / closes late) + an elimination move (a contentless
first-turn word is by exclusion phatic contact, so novel openers work unlisted).
Greeting/thanks/wellbeing/farewell, EN + IT. Felt-intelligence 64% → 82%.
Unified gen1's greet/farewell into it. TODO: learned act classification (no
markers) and real closing detection instead of the turn<=2 rule.

### C3 - Natural assertion + personal memory ("listen to me")
First-person and natural-shape facts: "I have a dog named Rex", "my dog is
Rex", "I like jazz", "call me Sam" -> remembered and queryable ("what is my dog
called?" -> "Rex"). Makes the bot feel like it is listening. Builds on the
memory module and T6 working memory. Anti-impostor: held-out
possessions/attributes, varied phrasing.

### C4 - Discourse memory ("what did we talk about?")
Track the running conversation (topics, recent entities, the last few turns) so
parrot0 can summarize or refer back. Conversational continuity is a core signal
of a mind that is present. Overlaps gen22 salience and T6.

### C5 - Best-effort over the blank wall (retire "I don't understand that yet.")
When nothing parses, do something useful: extract whatever is parseable, make a
focused clarifying question, or admit the *specific* gap — never the flat wall as
the default. Ties T11 (uncertainty) and T16 (not-understood). The frequency of
the wall response is itself a metric to drive down.

These C-tasks are the path from "comforting bench numbers" to the intelligence
we actually expect. Promote one at a time into TASK.md, smallest tested step
first, each earning both a `.chat` case and a C0 dialogue.

---

## M-series — Knowledge-acquisition missions (noted 2026-06-15, to process next)

Two owner missions about parrot0 *acquiring* knowledge from a source and
carrying it forward. Shared requirement: **persist what is learned into a
committed `knowledge/*.pl` file** (the existing format: human-readable facts /
rules, loaded at boot; `brain_save_session` already serializes `KB_INDUCED`, so
both extracted facts and the induced `cont`/`cont2` model persist). This makes
learning **cumulative, auditable, and version-controlled** — you can `git diff`
what parrot0 learned. Both stay pure C, no runtime model.

### M1 - POSIX sh / bash knowledge from a deterministic oracle
Goal: parrot0 answers "what does this command/script do?", "what happens if I run
X?", "how would you do this in POSIX?".

Why this is the strong first mission: shell commands have DETERMINISTIC effects,
and the agent can RUN them — so the shell is a free **oracle of truth**. We can
generate verified command→effect data AND, crucially, *test* genuine learning by
predicting the effect of commands/combinations never explicitly taught.

The structural challenge (anti-phrasebook): do NOT settle for a "command X → does
Y" dictionary. Aim for COMPOSITIONALITY — a micro-grammar of the shell
(command / flags / args / pipes / redirection) + semantics of primitive commands
+ composition rules, so `cat f | grep x` is understood as "filter lines of f by
x" from parts. Start narrow (a handful of commands), prove on held-out
combinations.

Acceptance (to refine):
- [DONE gen53] Effect of a single command with flags, composed from learned
  semantics (`knowledge/bash.pl`, committed); held-out flag combinations work;
  case-sensitive; honest on unknowns; `tests/posix.sh`.
- [next] Effect of a PIPELINE never taught verbatim is composed correctly
  (held-out) — compose command effects across `|`.
- [next] Oracle-grounded output PREDICTION for pure commands (e.g. echo),
  verified by running the real shell.
- An intent → command translation ("how in POSIX?") for a small task set.
Anti-impostor: predict an unseen command combination; the shell oracle judges.
Open wiring: default `make chat` loads base.pl only — shell knowledge needs
`PARROT0_BASE=knowledge/bash.pl` until multi-file knowledge loading exists.

### M2 - Learning knowledge from books
Goal: demonstrate that after parrot0 reads a book, it has *learned* from it.

Honest split of "learning from a book":
- (a) LINGUISTIC/distributional — already real (gen41): reading prose induces the
  generative model, so reading book A vs B changes what `say` produces. Provable
  now (the model is the corpus). Quick, convincing demonstration.
- (b) PROPOSITIONAL (facts) — gated by the extraction grammar; real book prose is
  mostly skipped today (the multi-word-entity / open-prose wall, see gen49 / the
  current TASK arc). This is the long pull.

Acceptance (to refine):
- Read a passage/chapter; show a measurable, held-out change (generation
  reflects it; some facts extracted and queryable).
- The learned knowledge is persisted to a committed `knowledge/<book>.pl` and
  reloads on next boot (cumulative across commits).
Anti-impostor: held-out questions answerable only from the read text, not seeded.

Ordering: do M1 first — the shell oracle lets us build the learn → verify →
persist machinery where truth is free, then transfer it to books (M2), which have
no oracle. The persist-into-commits mechanism serves both from day one. These are
parallel to the C-series (conversational intelligence); sequence them per what
the owner wants to process next.

---

## T1 - Bidirectional relations and real anonymous variables

Goal: complete the relation-query primitive pulled by morphology, then remove
the accidental special cases around `_`.

Tests:
- With `plural(dog, dogs).`, "what is the plural of dog?" returns `dogs.`
- With `singular(dogs, dog).`, "what is the singular of dogs?" returns `dog.`
- `countable(X) :- plural(X, _).` proves "is dog a countable?".
- `has_plural(Y) :- plural(_, Y).` proves "is dogs a has_plural?".
- A rule with two anonymous variables, e.g. `related(X) :- edge(X, _), edge(_,
  X).`, must not accidentally force both `_` occurrences to be the same value.

Hypothesis of evolution:
- The NL layer only needs subject-position and object-position unknowns for now.
- The KB parser/renamer probably needs to treat every `_` as a fresh anonymous
  variable, not as one shared variable name.
- This should remain a small engine correction, not a general parser rewrite.

Anti-impostor check:
- Use held-out predicates unrelated to morphology to prove the feature is
  relational, not hard-coded to `plural`.

## T2 - Multi-answer relational explanations

Goal: when parrot0 answers from rules, it should be able to explain the proof
path that made the answer true.

Tests:
- Given `man(socrates).` and `mortal(X) :- man(X).`, "why is socrates a
  mortal?" returns a compact chain: `mortal(socrates) because man(socrates).`
- Given `grandparent(X, Z) :- parent(X, Y), parent(Y, Z).`, "why is tom the
  grandparent of ann?" names both supporting facts and the shared binding
  `Y = bob`.
- If the claim is false, explanation says no proof was found, without inventing
  a reason.
- Explanations work for facts, unary rules, binary relations and multi-goal
  rules.

Hypothesis of evolution:
- The solver needs an optional proof trace alongside boolean success and match
  collection.
- A proof object may become the first explicit internal representation of
  reasoning steps.
- `kb_query` may need a richer sibling API without breaking existing callers.

Anti-impostor check:
- Test with a new predicate name and a different rule shape. The explanation
  must derive from the proof tree, not from canned English.

## T3 - Contradiction, correction and belief status

Goal: parrot0 should distinguish unknown, known true, retracted, contradicted
and disputed knowledge.

Tests:
- If told `socrates is a man`, "is socrates a man?" returns `Yes.`
- If then told `socrates is not a man`, the positive fact is retracted or
  contradicted according to the chosen semantics.
- If a base fact is corrected in session, saving and reloading preserves the
  correction without editing the base file.
- If two sources disagree, "what do you know about socrates?" reports the
  conflict instead of silently choosing one.
- A derived fact becomes false or disputed when its support is retracted or
  contradicted.

Hypothesis of evolution:
- Provenance may need negative session deltas or tombstones for base facts.
- The KB may need a belief-status layer above raw fact storage: true, false,
  unknown, conflicted.
- Resolution may need support sets so derived beliefs can be invalidated or
  explained.

Anti-impostor check:
- Include contradictions through rules, not only direct facts.

## T4 - Domain-pulled grammar v1: morphology and agreement

Goal: expand the grammar expert from word categories into morphology and simple
agreement, using only primitives demanded by tests.

Tests:
- `plural(dog, dogs).` and `plural(child, children).` answer plural queries.
- `number(dog, singular).` and `number(dogs, plural).` can be derived from
  morphology facts/rules.
- A sentence like `dog runs` is grammatical, while `dog run` is rejected.
- A sentence like `dogs run` is grammatical, while `dogs runs` is rejected.
- Held-out nouns and verbs loaded from a separate file work without code
  changes.

Hypothesis of evolution:
- The current word-splitting NL layer is not enough for sentence analysis.
- A small token/sequence representation may emerge, for example
  `token(sentence, index, word)`.
- Grammar should remain a knowledge domain first; code changes should only add
  missing general primitives.

Anti-impostor check:
- Do not accept a hard-coded list of valid sentence strings. Tests must load
  new nouns/verbs at runtime.

## T5 - Parsing as knowledge: from surface text to facts

Goal: move from hand-coded NL templates toward a knowledge-driven parser that
can map user utterances into KB goals/assertions.

Tests:
- The grammar domain defines patterns equivalent to `"X is a Y" -> assert Y(X)`.
- A new pattern loaded from knowledge, e.g. `"X owns Y" -> owns(X, Y)`, works
  without modifying `brain.c`.
- Queries, assertions and retractions are all expressible as pattern rules.
- Ambiguous parses are reported or ranked; parrot0 must not silently choose an
  arbitrary parse when two meanings are plausible.

Hypothesis of evolution:
- A pattern language may need variables, literals, captures and action terms.
- `mod_knowledge` may split into `parse -> intent -> execute`.
- Language intake starts becoming a domain, not a pile of C string cases.

Anti-impostor check:
- Add a new pattern in a test fixture after the binary is built. If it works,
  parsing is genuinely data-driven.

## T6 - Working memory and multi-turn goal tracking

Goal: parrot0 should hold a task across turns, update it with new information,
and know when it has completed it.

Tests:
- User starts a plan, then supplies destination, dates and constraints across
  separate turns; parrot0 keeps the same active goal.
- If a critical field is missing, it asks for that field instead of guessing.
- Once all required fields are known, it summarizes the plan and marks the goal
  complete.
- Starting a second plan does not corrupt the first.

Hypothesis of evolution:
- The brain likely needs an explicit goal/task frame with slots, status and
  provenance.
- Memory should split into episodic facts, active goals and durable knowledge.
- Module dispatch may need coordination because goal tracking can claim turns
  that would otherwise go to knowledge or echo.

Anti-impostor check:
- Randomize field order and insert unrelated turns between updates.

## T7 - Planning with constraints

Goal: given a small world model, parrot0 should construct a valid sequence of
actions to reach a goal.

Tests:
- Blocks-world style facts describe objects, locations and legal moves.
- Query: "how can I put a on b?" returns a valid action sequence.
- If a precondition is missing, the plan includes the action that establishes it.
- If the goal is impossible under current facts, parrot0 says why.
- A changed world fact invalidates the old plan and forces replanning.

Hypothesis of evolution:
- The KB needs action schemas with preconditions and effects.
- Planning may emerge as search over symbolic state, reusing unification and
  resolution.
- parrot0 will need a distinction between current facts and hypothetical states.

Anti-impostor check:
- Use held-out object names and different initial layouts.

## T8 - Analogy and transfer across domains

Goal: parrot0 should reuse a structure learned in one domain to solve a related
problem in another.

Tests:
- Teach a family relation pattern: parent chains imply grandparent.
- Teach a graph relation pattern: edge chains imply path length two.
- Ask parrot0 to notice the shared structure and propose an analogous rule.
- The proposed rule must pass held-out examples in the target domain.

Hypothesis of evolution:
- The system may need meta-level representations of rules: predicates,
  argument roles and shared-variable topology as inspectable objects.
- Induction may evolve from unary subset rules into relational rule induction.
- This directly pressures abstract structure, one candidate piece of the hidden
  architecture.

Anti-impostor check:
- Rename all predicates in the target domain. Success must depend on structure,
  not vocabulary.

## T9 - Self-model with capability truth maintenance

Goal: parrot0's self-description should be derived from actual registered
capabilities, passing tests before being advertised.

Tests:
- "what can you do?" lists only modules/domains whose tests are marked passing.
- If grammar tests fail, grammar is not advertised as a reliable capability.
- "why can you answer plural questions?" points to loaded predicates/rules and
  passing morphology tests.
- Self facts are regenerated at boot and never persisted as session knowledge.

Hypothesis of evolution:
- The self-model needs capability records, test evidence and status, not only
  `module(X)`.
- Test results may become knowledge artifacts loaded into the reflective layer.
- Reflection becomes grounded in verification, not in code comments or slogans.

Anti-impostor check:
- Temporarily disable a domain file in a test and verify the claimed capability
  disappears.

## T10 - Learning from correction, not only assertion

Goal: corrections should update future generalizations and reduce repeated
errors.

Tests:
- Teach examples that induce an over-broad rule.
- Provide a counterexample.
- parrot0 retracts or weakens the rule and stops deriving the false conclusion.
- With enough positive and negative examples, it proposes a more specific rule.

Hypothesis of evolution:
- Induction needs negative evidence and support/counterexample accounting.
- Rules may need confidence or justification sets.
- The engine may need rule revision, not only rule addition.

Anti-impostor check:
- Use different predicates and constants in each test run.

## T11 - Conversation under uncertainty

Goal: parrot0 should recognize when it lacks enough information and ask a useful
question instead of guessing.

Tests:
- If asked about a missing fact, it reports the chosen epistemic state and what
  fact would decide it.
- If asked to plan with missing preconditions, it asks for the missing state.
- If two parses are possible, it asks a disambiguating question.
- If a rule has multiple possible bindings, it lists alternatives or asks which
  one the user means.

Hypothesis of evolution:
- The response layer needs epistemic states: true, false, unknown, ambiguous,
  conflicted.
- Modules may need to return clarification continuations.
- This is a step toward metacognition: reasoning about what the system does not
  know.

Anti-impostor check:
- Ensure the same question becomes answerable after the missing fact is supplied.

## T12 - Compositional language task: tiny story understanding

Goal: read a short story, build a world model, answer direct and derived
questions, and correct the model when the story changes.

Tests:
- Input: "alice picked up the key. alice opened the door." Query: "who opened
  the door?" returns `alice.`
- Query: "could alice open the door?" derives yes because she had the key.
- If later told "alice dropped the key before opening the door", the derived
  answer changes or becomes conflicted.
- A held-out story with different names, objects and order works.

Hypothesis of evolution:
- parrot0 needs event representation: actor, action, object, time/order.
- It needs temporal reasoning, at least before/after and state changes.
- This may force separation between timeless facts and event-indexed facts.

Anti-impostor check:
- Vary names and event order. Fixed transcript matching is not enough.

## T13 - Recursive abstraction and named concepts

Goal: parrot0 should form a named concept from a pattern, use it, explain it
and revise it.

Tests:
- Given examples of "safe move" in a toy domain, induce or accept a definition.
- Use the new concept in planning.
- Explain why a move is safe by expanding the concept.
- Revise the concept after a counterexample.

Hypothesis of evolution:
- Concepts become rules with names, provenance, examples and counterexamples.
- The system may need concept dependency graphs.
- This pressures layered abstractions, not just flat facts.

Anti-impostor check:
- The named concept must apply to unseen objects and fail on counterexamples.

## T14 - Resource-bounded reasoning

Goal: parrot0 should reason under depth/time limits and report partial results
honestly.

Tests:
- Cyclic rules do not hang.
- A deep but valid chain succeeds within a configured bound.
- A query that exceeds the bound returns "not proven within limit" rather than
  false.
- Increasing the bound changes the answer when appropriate.

Hypothesis of evolution:
- The solver needs to distinguish failure from resource exhaustion.
- Search strategy may become explicit: depth-first, breadth-first, iterative
  deepening or cost-bounded search.
- This pushes toward control architecture: not only what to reason, but how to
  allocate reasoning effort.

Anti-impostor check:
- Include both truly false and merely too-deep queries.

## T15 - Integrated benchmark: the small apprentice

Goal: combine grammar, parsing, memory, correction, explanation and planning in
one held-out scenario.

Tests:
- Load a small domain file unknown at compile time.
- Teach parrot0 new vocabulary through that file.
- Ask it to parse facts from natural language, derive a rule-based conclusion,
  explain the conclusion, correct one premise, and update the conclusion.
- Ask it to plan a short action sequence using the same facts.
- Ask "what did you learn?" and require a self-model answer grounded in stored
  facts, induced rules and passing tests.

Hypothesis of evolution:
- No single module can fake this cleanly. The pressure should reveal boundaries:
  parser, KB, solver, memory, planner, explainer, self-model and controller.
- The hidden architecture being sought may show up as the shape of the
  interfaces between these parts.
- This benchmark should be repeated after every major generation as the main
  anti-impostor test.

Anti-impostor check:
- Use a fresh domain fixture for each run. The binary must not know the domain
  names in advance.

## T16 - Handling the unknown and the not-understood (study + design)

Goal: parrot0 should deal gracefully and *distinctly* with two different gaps:
- the NOT-KNOWN (epistemic): a well-formed query about something absent from the
  KB — today answered "No." under closed-world, which wrongly conflates *false*
  with *unknown*;
- the NOT-UNDERSTOOD (linguistic): input it cannot parse at all — a different
  language, gibberish, a malformed or off-topic sentence — today silently
  *parroted* by the gen0 echo fallback.

Method: these patterns should EMERGE from testing in chat with inputs it does
not understand. Probe with other languages, gibberish, half-sentences, empty
intent, questions about unknown entities, contradictory framings — and let the
failures show what categories the agent actually needs.

Tests (to be shaped by probing):
- A clearly unparseable line is met with an honest non-understanding, not an
  echo and not a wrong "No.".
- A well-formed query about an unknown entity distinguishes "I don't know" from
  "No." where the difference matters.
- The response names, when possible, what would resolve the gap (which fact,
  which parse).
- Held-out: new gibberish / new unknown entities behave the same (not hard-coded
  to specific strings).

Hypotheses of evolution:
- Dispatch needs a real "not understood" outcome distinct from "no module
  claimed it -> echo". This RETIRES or reshapes the founding parrot-echo
  fallback — a deliberate identity decision (the parrot was always gen0, to be
  outgrown). `tests/cases/parrot.chat` would change.
- The response layer needs epistemic states (true / false / unknown / not-
  understood), overlapping with T3 and T11.
- A "parse coverage" / confidence signal may be needed to decide when to admit
  non-understanding vs attempt a best-effort interpretation.

Anti-impostor check:
- No special-casing fixed unknown strings. Non-understanding must be a general
  fallthrough property, provable with novel inputs.

Observed (probe, 2026-06-15) — current behaviour on inputs it can't handle:
- French / gibberish / "the the the" / "run" / "42"  -> ECHOED verbatim (the
  gen0 parrot), giving no sign it didn't understand.
- "is zorp a blarg?" (unknown entities, well-formed)  -> "No." (closed-world
  conflates *unknown* with *false*).
- "who is the president of france?" (relation, unknown) -> "Nobody that I know
  of." (same conflation, relational form).
Diagnosis: the agent NEVER admits ignorance — it echoes, denies, or empties.
Three outcomes to design: not-understood (retire/reshape echo), not-known-class
("No." -> "I don't know"?), not-known-relation ("Nobody" -> "I don't know"?).
The first two touch founding behaviour (parrot identity; closed-world denial).

---

## Suggested ordering

1. Finish T1 because it is already pulled by grammar/morphology.
2. Do T2 before larger planning work, because proof traces make later failures
   diagnosable.
3. Do T3 before serious learning, because correction is required for honest
   growth.
4. Alternate domain-pull tasks (T4, T12, T15) with the primitives they expose
   as necessary.
5. Promote any recurring primitive into `TASK.md` only when a concrete test
   demands it.
