# Current task

> One goal at a time. When it is done, replace this with the next one.

## Active — two live threads (docs/plans/coding-agent-evolution.md); pick ONE per generation

**Thread A — Track 5.4/5.5: the derived plan on the REAL codebase.**
Done through gen271: the whole arc runs on the foreign fixture (gen257–263) AND
on src/brain — gen271 migrated the FIRST real site (00-lex.c's teach-verb
guard → `kb_cue_match(b, "00_lex_chain332", low)`, facts in kb/core/intents.p0)
via knowledge, not C branches: `codebase_lookup/2` (which lookup a codebase
owns) + `lookup_call/2` (the call SHAPE, FN/ARG/KEY template), with both
compile judges fragment-honest (a file that never compiled standalone defers
to "its own build/test suite must judge"). Ratchet: tests/cuechains.sh in
`make test` — the chain counter (341 → 340) MUST ONLY DESCEND. gen272 closed
Track 5.5 as PURE DATA (two learnable/3 rows): "learn \"memorizza \" as a
teaching verb" asserts into the gate's own vocabulary and the deaf branch hooks
with no rebuild — recursion proven (taught verb teaches the next verb);
teachverb.chat/.it pin the arc. Next:
gen273 migrated a whole module (85-translate-synth-world.c, 2 counterfactual
chains, zero engine changes; MAX 340 → 338). Next:
1. The `b`-out-of-scope frontier (found in 65-induce-verify-shell.c,
   is_wellbeing_content(const char *buf)): per-chain applicability in
   patch_chains, or a context-aware call template — the pull the counter hits.
2. Keep migrating clean modules; lower MAX each time. Sites whose intent key
   deserves a SEMANTIC name get a learnable row (runtime-growable like the
   teach gate, gen272).

**Thread B — RULESCORE (`make rulescore`, F.'s gauge): text→rules→code.**
Done: gen264 harness + honest 0/25 baseline; gen265 category recognition (5/5
honest derived declines, 0 misclaims; `mod_rulespec`, cue classes in
kb/core/intents.p0); gen266 FIRST synthesized category `count_to_threshold`
(game_shape KB fact → parameter extraction from the rule segments →
`code_synth_game_counter` → `code_check_counter_game` two-scripted-plays
oracle; EN+IT; mis-extraction guard because fabrication is worse than decline).
Next pulls, in leverage order:
1. Widen count_to_threshold's expressible border (e.g. "guesses the word X",
   quoted tokens, "score of N" phrasings) — same schema, richer extraction.
2. Second game schema pulled from a real invented game that ALMOST fits
   (candidates from past runs: countdown resource — "start with N, each turn
   subtract input, lose below zero").
3. Multi-rule composition: input alphabet validation + threshold in one program
   (two schema fragments composing — the anti-phrasebook test).

Discipline unchanged: one goal per generation, pull from a red gauge, KB-first
(schemas and vocabulary are facts), honest decline where the category ends,
`make test` green always.

Discipline: the plan stays INFERRED from kb/experts/codebase/actions.p0 (never a
C pipeline); missing knowledge → honest decline naming the gap; EN+IT ratchet;
`make test` green always.

## (history) SOLVED - 3 REAL SWE-bench_Lite instances, by 3 general structural smells

All resolved by the official SWE-bench Docker oracle, no deception (parrot0 reads
only the committed buggy file, derives from structure, the real tests judge):
- **astropy-12907 (gen200)** — SYMMETRY BREAK (`code_symmetry_fix`): a sibling
  branch assigns a literal where the analogous variable belongs.
- **astropy-6938 (gen201)** — DISCARDED RESULT (`code_find_discarded_result`): a
  bare pure-method call whose value is thrown away; assign it back in place.
- **astropy-14995 (gen204)** — CONDITION ASYMMETRY (`code_find_cond_asymmetry`): a
  bare `NAME is None` guard where siblings test `X.ATTR is None` and `NAME.ATTR` is
  used elsewhere; the guard should test `NAME.ATTR is None`. (179 PASS_TO_PASS.)
Run: `make swe-solve [INSTANCE=astropy__astropy-6938|astropy__astropy-14995]`.
The brain's "find/fix the bug in <path>" tries each smell in turn — a growing
library of grounded localizers/transformations, NOT general APR. (gen204 also fixed
the oracle's ANSI-colour pass-detection + batched the test runs.)

Next swe-bench pulls (each: a grounded smell pulled from real pressure, judged by
the real oracle — never a hardcoded answer):
- remaining committed instances: astropy-14365 (case-insensitivity — likely needs
  issue-driven/associative localization, the X6 frontier §4), astropy-14182 (a
  FEATURE add — out of structural-repair scope), astropy-14995 (mask semantics).
- widen the map: `tests/swebench/fetch_lite.sh 50`, `make swe-bench`.
- the value-domain Python semantics delta (numpy arrays) for semantic, not just
  structural, bugs.

## (history) Active task - climb toward the first REAL SWE-bench_Lite instance (F., 2026-06-23)

F. directed: use the **real** SWE-bench_Lite, not a proxy. gen195 wired it
(`tests/swebench/fetch_lite.sh` fetches real rows once, cached under the gitignored
`.cache/`, committed static under `tests/swebench/lite/`; `make swe-bench` reads
them offline, behavioural degrade mode). The synthetic `swe-001` was removed.

**First real problem (what the bench proposes):**
- **astropy__astropy-12907** (astropy/astropy): *Modeling's `separability_matrix`
  does not compute separability correctly for nested CompoundModels.*
  FAIL_TO_PASS: `astropy/modeling/tests/test_separable.py::test_separable[...]`.

**Honest status:** parrot0 cannot solve this today and `make swe-bench` says so per
instance. All 5 fetched instances are **Python**; parrot0's code engine is C-only,
the repos are not checked out, and the hidden pytest suite needs a Python env. So
the real target is many generations away; the bench is the intercepted-failure map.

**Done — X1 run-grounding (gen198).** `code_run` (compile+link+EXECUTE, real exit
status read from the process) closes the C verify ladder compile->link->run — the
grounded "did it pass?" oracle now exists for C; `run_execute.code` ratcheted to
pass. Also fixed a latent compound double-dispatch in the codeast path-branches.

**Done — Python F3 semantics by delta (gen199, CODE-MASTERY §7b).** `code_eval` now
evaluates Python `def`s (locals + return, recursion) through the SAME engine as C —
only the concrete syntax is the delta. The inference no longer speaks C for
semantics. Remaining for astropy-12907: numpy ARRAY value-domain semantics (an
additive Python-specific fact set), X6 issue->`_cstack` localization (the
associative crux), X7 patch synthesis.

**Ordered pulls toward it (TASKLIST X-series) — smallest first, the next task:**
1. **X3 — abstract node vocabulary.** Audit whether the gen173-192 analyzers speak
   C-specific structures or an abstract `node/…` vocabulary; refactor so
   localization / `calls`/`assigns` read abstract facts. *This is the real
   "support Python", not a parser rewrite* (CODE-MASTERY §7b). It needs no new
   hands and unblocks every later pull.
2. **(curation)** check out a repo at `base_commit` as static files (network once,
   like the wiki corpus) so a real repo can be read offline.
3. **X1 run / X6 localization / X7 patch** — build+run the project's tests, locate
   issue->file, produce a patch that flips FAIL_TO_PASS. The F4 issue->edit
   *synthesis* is the measured frontier, not a promised feature.

Keep every claim grounded (real compiler/runtime), KB-first where a capability is
a lexical class or fact set, EN+IT ratchet, journal, commit.

## Parked - basic-chat driver (resume after the swe-bench thread or when F. asks)

`docs/plans/basic-chat.md` — ~974 elementary prompts, closed one structural
generation at a time within PRINCIPLES.md. `make basic-chat-bench`: gen189 24%,
gen190 **26% (260/974)**. Done: cat.0 (gen189), cat.4 arithmetic (gen190). Next
high-leverage 0% blocks when resumed: cat.52 Elencazione (structural lists),
cat.18 Biologia animale (KB class-membership), cat.19 Versi animali (KB content).

## Parked - dynamic knowledge gen171: learn.py consumes the research queue

(Parked when basic-chat became the driver. Resume after the chat backlog or when
F. asks.) Goal: close the next link of F.'s dynamic-knowledge loop. `mod_research`
(gen170) records gaps and, with `$PARROT0_RESEARCH_QUEUE` set, appends
`key<TAB>display`. Make `scripts/learn.py` read that queue: fetch the requested
topics from static Wikipedia FIRST (then the curated `sources.tsv` round-robin),
write `kb/learning`,
clear served entries, and commit — so a flagged gap is actually filled.

Acceptance:
- A queue entry causes learn.py to fetch that topic (offline fixture test), emit
  its `wiki_concept` + Markdown + event, and remove it from the queue.
- The curated round-robin still runs when the queue is empty.
- Offline regression test (extend `tests/wiki_learning.sh` or a sibling).

Then gen172: parrot0 answers from a freshly-learned `wiki_concept`, HONESTLY framed
("From what I learned on Wikipedia: ..."), not as prior knowledge — closing the
loop: gap -> flag -> fetch -> commit -> honest answer next time.

Anti-impostor: no API that reasons; static Wikipedia files only. Honesty: a
just-learned answer must be marked as just-learned.

## Done recently

- **gen170 - the research-need signal (dynamic knowledge).** `mod_research`
  (registered last) honestly flags a definitional gap, records the topic, and
  (with `$PARROT0_RESEARCH_QUEUE`) queues `key<TAB>display` for the external
  learner; "what do you need to research?" reads the list back. Pure C, no
  network; arith/known-concept guarded so nothing regresses. EN+IT
  `research.chat`/`.it`. 177 cases, `make test` 22/22. Founding "no network"
  reaffirmed as "no outsourced intelligence; static knowledge is fine".

- **gen169 - the self-audit matrix.** New `run_composition` helper + a
  `want_audit` branch run three triples of parrot0's parts on fresh copies of
  itself and report a real cooperation map ("knowledge+abduce+robust compose;
  knowledge+abduce+calibrate compose; knowledge+robust+calibrate seam. 2 of 3").
  Computed: retracting abduce collapses it to 0 of 3. Fixed the calibrate
  signature/turn so calibration can fire. `reflexive_audit.chat`/`.it` +
  `_retract.chat`. 175 cases, `make test` 22/22, compose-bench 7/7.

- **gen168 - self-test generates fresh held-out vocabulary.** A rotating
  `compose_vocab` pool + `build_turn` template + a per-session `selftest_runs`
  counter make each self-test use different names; the verification signatures are
  vocab-independent, so two runs differ yet both verify. The PASS report cites the
  real Observed line from the sub-run. `reflexive_selftest_fresh.chat` proves two
  runs use different vocab; `_seam.chat` keeps the computed FAIL. 172 cases.

- **gen167 - parrot0 runs its own composition (E1 capstone).** A `want_selftest`
  branch in `mod_loop` spins up a fresh sub-brain (`brain_create`), runs the
  dialogue derived from `module(X)` through `brain_respond`, and reports PASS/seam
  computed from real output. Default triple is 3/3 PASS; retracting `abduce`
  yields a real 1/3 FAIL (seam), proving the verdict is executed not canned.
  Footprint-free, no file edit/commit. `reflexive_selftest.chat`/`.it` +
  `reflexive_selftest_seam.chat`. 171 cases, `make test` 22/22, compose-bench 7/7.

- **gen166 - introspection closed to execution.** `mod_loop` now emits a runnable
  single-line `>`-turn dialogue skeleton over the parts derived from `module(X)`
  ("show me the dialogue you would run", EN+IT). The emitted spine composes for
  real — added as `tests/compose/skeleton_proposed_en.dlg`, taking compose-bench
  to 7/7, 100%. Derivation holds: retracting a module changes the emitted turns.
  `reflexive_skeleton.chat`/`.it`. 168 cases, `make test` 22/22.

- **gen165 - derived reflexive composition.** The `compose_challenge` branch in
  `mod_loop` now derives its three named parts via `kb_query("module", ...)` over
  a composable-core list, so the proposal tracks the live self-model. Proven, not
  asserted: "forget that abduce is a module" makes the next challenge name
  "knowledge, robustness, and calibration" instead — ratcheted hermetically in
  `reflexive_derived.chat`. Default triple (knowledge/abduce/robust) is exactly
  what `analytical_en.dlg` proves cooperates. EN/IT ratchets updated. 166 cases.

- **gen164 - reflexive composition (E1).** `mod_loop` now recognises a
  COMPOSITION self-challenge (composition word + parts reference, EN+IT) and
  answers with a loop-shaped method over real parts — the compose-bench
  discipline it was grown by — staying anti-self-management. The two gen160
  probes that walled now land; 10/10 held-out stress; gap challenges and innocent
  "compose" unaffected. `reflexive_compose.chat`/`.it`. `make test` 22/22.


- **gen163 - possession memory composes with discourse reference (E1, gap
  closed).** `remember_possession` now sets `last_entity` to the named pet, so an
  unbound "she/he/it" binds to it when no KB-fact antecedent exists (a real later
  subject still overrides by recency). `make compose-bench` reaches 6/6 composing
  unchanged, 0 gaps, 100% landing — E1 fully closed. Stress held over fresh vocab
  and he/it/she. `compose_coref.chat`/`.it`; `social_pet_en.dlg` `#expect: pass`.

- **gen162 - bare self-introduction feeds name memory.** A self-introduction
  block in `mod_memory` accepts "i'm <X>" / "i am <X>" / "im <X>" (behind an
  optional greeting) as a name only when the single trailing token clears a
  non-name filter (not an article/stopword/known KB class/common state), so it
  generalizes to unseen names while leaving affective turns ("i'm tired",
  "i am bored") to `mod_chitchat`. First miss of `social_gap_en` now lands;
  remaining: the she->pet coref half. `make test` 22/22.

- **gen161 - close the Italian half of the E1 composition gaps.** Two additive
  surface extensions in `brain.c`, no new module: an Italian subject-verb branch
  in the why-proof ("perché X è un Y?" -> the same proof "why is X a Y?" renders)
  and "come mi chiamo?" added to `mod_memory`'s name-recall list. `compose-bench`
  composing-unchanged 4 -> 5, gaps 2 -> 1, landing 87% -> 92%. Stress confirmed
  transfer to fresh vocab incl. feminine "una". `it_recall.dlg` (renamed) now
  `#expect: pass`; `compose.it.chat`/`compose_social.it.chat` grew the new reads.

- **gen160 - compositional emergence benchmark (E1).** New `make compose-bench`
  (`tests/composebench.sh`) runs held-out, fresh-vocab dialogues that force >=3
  independently-evolved subsystems to cooperate, scoring each as
  composes-unchanged / generic-parser / special-case. 4 of 6 compose UNCHANGED
  with no new module: an analytical chain of EIGHT subsystems (rule+fact intake,
  deduction, contrastive + optimal abduction, self-correction, proof, robustness)
  and a social chain of SIX (greeting, name + possession memory, KB fact,
  discourse reference, summary), both EN and IT through one path. Two gaps
  recorded as generic-parser growth edges. Bilingual ratchets `compose.chat`/`.it`
  and `compose_social.chat`/`.it`. No runtime behaviour changed; the gauge proves
  and guards composition. Self-challenge: parrot0 cannot yet reason about
  composing its own parts (candidate: extend `mod_loop`).

- **gen148 - user-model context for ordinary conversation (E4).** `mod_memory` now remembers a preference and a current constraint, answers specific preference/mood/topic/constraint questions, and summarizes `what do you remember about me?` with durable personal facts separated from session context. `mod_chitchat` records mood/boredom while preserving its existing replies; `mod_pragma` records the current topic when it already accepts a topic-change turn. EN/IT ratchets plus 10x stress. Long-chat moved felt landing 85% -> 87%, wall 9% -> 7%, continuity 71% -> 76%, topic changes 85% -> 100%.

- **gen147 - long casual conversation benchmark (E6).** Added `make long-chat-bench` and a deterministic long-chat suite under `tests/longchat/`: a 5-turn smoke run, a 16-turn Italian run, and a 50-turn mixed EN/IT stress session. The harness tracks landing rate, wall rate, immediate repetition, repair success, continuity references, contradiction handling, user-model precision, boredom handling and topic-change handling. No runtime behavior changed; this generation creates the pressure gauge for the next conversation tasks.

- **gen146 - open-domain humility protocol (E5).** `mod_knowledge` now gives scoped humility answers for open-domain fact questions that fall outside the KB/tool model: missing binary relations (`capital`, `mayor`), missing unary proof support (`blue(sky)`), and missing clock/calendar facts/tools (`current_year`). It names the exact gap and suggests teaching facts/rules or giving a passage, without pretending to know the answer. EN/IT ratchets plus a 10-turn stress block. `blankwall.chat`, `blankwall.it.chat`.

- **gen145 - self-challenge parity, not self-management.** `mod_loop` lets parrot0 answer explicit challenges about its own implementation by proposing a comparable loop-shaped solution: missing behavior, owning module/dispatch point, smallest deterministic change, EN/IT tests, version bump, and journal entry. It does not edit, test, commit, push, or choose tasks; the external loop still owns action and verification. `LOOP.md` now requires this parity probe before implementation, with a 10-turn held-out stress block. `self.chat`, `intent.it.chat`.
- **gen144 — pragmatic intent from turn shape (E3).** `mod_pragma` classifies a
  turn on a small SHAPE FEATURE vector (opener class, hedge, negation+stance
  predicate, contrastive, open-quantifier object, topic-intro frame, content
  gate) and routes each speech act to a different move; a pre-dispatch
  `pragma_peel` normalizes a leading discourse opener away so mixed social+content
  turns keep their content task. Generalizes to held-out phrasings, EN/IT one
  path. `pragma.chat`/`.it`, `pragma_stress.chat`.

- **gen143 — local-world working memory (E7).** `mod_world` gives parrot0 a
  scoped, session-only belief overlay: "in this story rex is a dragon" is usable
  across turns, the same name means different things in different worlds, "what is
  assumed?" reads the active scope back, and tearing a world down leaks nothing
  into the persisted KB. `world.chat`/`.it`, `world_stress.chat`.

- **gen142 — metacognitive calibration (E8).** `mod_calibrate` reports HOW it
  knows the last conclusion with confidence language COMPUTED from proof state —
  KNOWN / INFERRED / CONFLICTED / HYPOTHETICAL (detected by ablation through a
  standing assumption) / UNKNOWN — and answers "why?" and "what would change your
  mind?" from the same real state. `calibrate.chat`/`.it`, `calibrate_stress.chat`.

- **gen141 — conversational repair loop (E2).** `mod_repair` (registered first)
  turns a referential gap from a wall into a clarify→store→resume bridge across
  turns. A pronoun query with no antecedent ("is it a mammal?") or an arithmetic
  operand named by a pronoun ("what is it plus 10") opens a narrow clarification
  and stores the unfinished turn; the next turn fills the slot (a teaching answer
  resolves it by coreference, else a referent/number is substituted) and the
  ORIGINAL intent is re-dispatched for real; a fresh question instead expires the
  pending state. Composes with arithmetic, teaching, coref and rule derivation —
  no special-case glue. EN/IT ratchets + a ~10-episode stress probe.
  `repair.chat`/`.it`, `repair_stress.chat`.

- **gen140 — conversation companion register.** `mod_chitchat` now handles ordinary non-technical talk openings: no-topic turns ("I don't know what to say"), requests for conversational scaffolding ("say something"), casual talk invitations, low-energy mood, and upbeat acknowledgements. It stays anti-impostor: it guides toward tractable conversation rather than pretending world knowledge or empathy. Ratcheted with EN/IT minimal cases plus a 10-turn casual stress set. `chitchat.chat`/`.it`.


- **gen139 — hypothetical repair simulation (plan -> proof -> restore).** The optimal abductive plan can now be simulated: "try the easiest way to make X a Y" temporarily asserts the missing root facts, proves the goal with `kb_explain`, reports the proof, then retracts the temporary facts so the live KB remains unchanged. Ratcheted with EN/IT footprint checks and a 10-alternative stress simulation. `branching_abduce.chat`/`.it`.


- **gen138 — optimal abduction (lowest-cost repair plan).** `mod_abduce` now answers "what is the easiest way to make X a Y?" by scoring every rule alternative by missing root premises, skipping already-known conjuncts, and choosing the cheapest plan. This composes branching, conjunction, chain-rooting, and current KB state. Ratcheted with EN/IT minimal cases and a 10-alternative stress probe where the best path is last and partly satisfied. `branching_abduce.chat`/`.it`.


- **gen137 — branching contrastive abduction (all alternatives blocked).** Why-not over a goal with multiple rule alternatives now enumerates every blocked derivation instead of citing only the first rule (`cat -> pet needs rex is a cat; dog -> pet needs rex is a dog`). The branch also roots missing derived premises per alternative. Ratcheted with EN/IT minimal cases and a 10-alternative English stress probe per LOOP.md. `branching_abduce.chat`/`.it`.


- **gen136 — chained contrastive abduction (root-cause why-not).** Contrastive why-not now follows the same backward chain as ordinary abduction: if the missing immediate premise is itself derived, `mod_abduce` reports the root premise and spine ("missing rex is a human; by human -> man -> mortal") instead of stopping at the first failed link. Ratcheted with a minimal EN/IT chain and a 9-link English stress probe per LOOP.md. `abduce_chain.chat`/`.it`.


- **gen135 — contrastive abduction (why-not).** `mod_abduce` now answers failed-goal questions like "why isn't rex a goodboy?" / "perche rex non e un bravo?" by reading the same rule body used for abduction and naming the missing conjuncts that block the proof. The trigger is local to abduction, so Italian meta/strategy "perche" cues keep their old modules. `abduce.chat`/`.it`.


- **gen134 — branching abduction (enumerating the hypothesis space).** When >1
  rule concludes a goal, `mod_abduce` offers every alternative ("either rex is a
  cat, or rex is a dog — any one would make rex a pet") via new
  `kb_rules_for_head`/`kb_nth_rule_body_preds`; single-rule path unchanged.
  Completes the abduction triad (single/chained/branching) and the inference
  lattice over definite clauses. `branching_abduce.chat`/`.it`.
- **gen133 — conjunctive concepts (the keystone).** "every friendly dog is a
  goodboy" → `goodboy(X) :- friendly(X), dog(X)` via new `kb_assert_rule_n`; a
  guarded article-free intake ("rex is friendly" → `friendly(rex)`, only when the
  class is a rule-body predicate via `kb_rule_body_mentions`). One change closed
  three open edges at once: gen130 multi-fact robustness, gen131/132 conjunctive
  abduction, gen103 self-correction through a conjunctive rule — no module
  changed, the parts were already general. `conjunction.chat`/`.it`.
- **gen132 (L16/L19) — chained abduction.** `mod_abduce` now recurses backward
  (`abduce_roots`/`abduce_spine`): when the missing premise is itself derived,
  it chains to the ROOT fact and reports the inference spine ("by human -> man ->
  mortal"). Supplying the root propagates the whole chain forward (gen103). The
  backward dual of gen130's forward sweep. gen131 one-step format preserved.
  `abduce_chain.chat`/`.it`.
- **gen131 (L16/L19) — abduction (inference to the missing premise).** `mod_abduce`
  runs the rule engine BACKWARDS: from a goal that doesn't hold, find a rule whose
  head matches and name the body premise that would entail it ("If you told me
  socrates is a man, then socrates would be a mortal — that's the rule man ->
  mortal"). New spine query `kb_rule_body_preds`. Honest when no rule applies;
  defers to deduction (with proof) when the goal already holds. Composes with
  gen103: supply the abduced premise and the consequence is volunteered.
  `abduce.chat`/`.it`.
- **gen130 (L20-deep) — robustness by self-perturbation.** `mod_robust`
  stress-tests the last conclusion by ablating EACH ground unary fact in turn
  (retract → re-derive → restore) and reporting whether it is FRAGILE (one
  load-bearing fact, named) or ROBUST (no single point of failure). Composes
  gen129's ablation into a sweep; surfaces redundancy a proof trace can't.
  `robust.chat`/`.it`. Open: conjunctive support (needs multi-condition rules).
- **gen129 (L20-deep / L16) — epistemic counterfactual.** `mod_whatifnot`: "what
  would you conclude if you didn't know that <fact>?" hypothetically retracts a
  belief, re-derives the last conclusion, then RESTORES it (footprint-free) —
  reporting dependency ("rests on it"), redundancy ("another way"), or honest
  ignorance. The knowledge-sibling of gen128's control-flow counterfactual.
  `whatifnot.chat`/`.it`.
- **gen128 (L20-deep) — counterfactual meta-reasoning.** `mod_counterfactual`
  answers "what would you have said WITHOUT module X?" / "what else matched?" by
  RE-RUNNING its own first-match-wins dispatch over the previous turn with that
  module suppressed (`replay_dispatch`), reporting whatever the alternative self
  actually computes — a no-op suppression vs a decisive one vs an honest fallback
  are distinguished because the control flow is really re-executed. Footprint-free
  (snapshot/restore of `last_reply`/`last_module`/`fallbacks`); refuses unknown
  modules. The reflexive closure of gen105 `mod_strategy`.
  `counterfactual.chat`/`.it`.
- **gen127 (L12) — program synthesis, the inverse of `mod_shell`.** `mod_synth`
  maps a natural spec to a one-line command grounded in the SAME `cmd/flag`
  knowledge the interpreter reads: the action verb picks the command, the object
  noun picks the flag ("count the lines in a file" → `wc -l <file>`, "count the
  words" → `wc -w`). Held-out specs over known commands; honest decline. The verb
  is dropped before flag selection so the object noun disambiguates. `synth.sh`.
- **gen126 (L5) — grounded translation.** `mod_translate` composes a clause
  translation EN↔IT from per-word glosses (`tr/2`, `kb/gloss.p0`) plus a
  structural article rule (gender agreement, adjective agreement, vowel elision),
  never a stored sentence — held-out vocab transfers both directions. Reads the
  raw source (not the canonicalized surface, which would map "una"→"a").
  `translate.chat`/`.it`.
- **gen125 — the affective/phatic register; sim logs 16 → 5.** `mod_chitchat`
  (last in the registry) answers casual social TONE in register — emoji/laughter/
  emote, frustration/encouragement/agreement/language/filler (EN+IT) — honestly,
  never claiming content understanding, firing only on a real affective cue (so
  genuine questions still wall). `simclean` gained a garbage filter (LLM
  scratchpad leakage isn't a growth edge). 8 logs deleted by genuine engagement +
  staleness; the 5 kept hold real knowledge gaps (sky-blue, what-year, Tuesday-
  colour) that parrot0 must not fake. `chitchat.chat`/`.it`.
- **simclean + gen124 — autonomous chatsim-log janitor.** `tests/simclean.sh`
  (`make simclean`) replays each `tests/chat/sim/*.log` against the current
  parrot0 and deletes the ones that no longer wall ("I don't understand") or have
  no real turns; keeps the rest and prints their failing inputs. First run pruned
  3 empty model-error logs. gen124: the legitimate phatic walls it surfaced
  ("what's up", "you good", "you there?") fixed as DATA in `kb/social.p0`.
  `smalltalk.chat`.
- **gen121–123 (L6/L7) — read a passage and summarize it.** `mod_summary`:
  gen121 extractive summary (rank the really-extracted propositions by concept
  centrality, quote the top real sentences); gen122 the gist ("what is this
  about?" → central concept + most salient sentence); gen123 query-focused
  comprehension ("what did you learn about X?"). Reader canonicalizes each clause
  so Italian parses through the same path; honest skip count + honest "doesn't say
  anything about X". `summary.chat`/`.it`, `comprehension.chat`/`.it`.
- **gen120 (rung 19 + 16) — hypothesis testing / falsification.** `mod_verify`
  induces a law from examples, then judges a held-out transition against it —
  confirm or refute, naming the predicted value ("predicts 10 -> 21, not 20").
  Closes observe→hypothesize→plan→test. `agent_verify.chat`/`.it`.
- **gen119 (rung 19 + 13) — goal-directed search (planner).** `mod_search` runs
  BFS over a set of available actions to synthesize the SHORTEST sequence
  reaching a target ("from 3, using x3 and +1, reach 28"); honest on
  unreachable. `agent_search.chat`/`.it`.
- **gen118 (rung 19 + L10) — rule induction from data.** `mod_induce` fits a
  generative law (affine, else parity-split) from integer transitions — re-derives
  Collatz from examples and runs it; declines outside its hypothesis space.
  `agent_induce.chat`/`.it`.
- **gen117 (rung 19, deeper) — observation-driven branching agent.** The
  per-step action is CHOSEN by observing the state: a two-branch rule ("if even,
  halve; if odd, triple and add 1") parsed into per-branch op sequences, applied
  by the parity the loop observes — runs the Collatz process to a fixed point
  (27→1 in 111 real steps; held-out 6→8, 7→16; bilingual pari/dispari). No closed
  form, so the count comes only from running the loop. `agent_branch.chat`/`.it`.
- **gen116 (rung 19) — the autonomous act-loop.** `mod_agent`: a
  perceive→decide→act→observe cycle pursues a goal by repeated oracle calls
  ("start at 3, double until you reach 50"); trajectory + step count produced by
  the loop, not a formula; honest no-progress guard. `agent.chat`/`.it`.
- **gen115 (L15) — the first deliberate mid-turn tool call.** `mod_tool` compiles
  a word-count question to `echo … | wc -w` and INVOKES the pure POSIX oracle
  (`simulate_pipeline`), folding the computed result back and naming the command.
  The reason/act seam, with a real (not stubbed) oracle. `tool.chat`/`.it`.
- **gen114 (L17+) — multi-step word problems.** 3+ numbers fold an additive/
  subtractive chain clause by clause ("has 3, buys 5 more, then eats 2" → 6);
  2-number problems keep the single-op path. `wordproblem_multi.chat`.
- **gen113 (L17+) — two-step linear equations.** Coefficient on the unknown
  ("2x + 1 = 7" → 3) solved by inverting the additive part then dividing by the
  coefficient; one-step "5y = 20" → 4. `algebra2.chat` / `.it`.
- **gen112 (L17 support) — number words in prose.** Word problems and equations
  read number words ("three", "twenty-one", "two hundred"), EN+IT, via
  `collect_numbers`/`parse_value`. `numwords.chat` / `.it`.
- **gen111 (D-prop1 step 2) — generation policy as editable knowledge.**
  `next_word_ctx` reads its trigram/bigram interpolation weights from
  `weight(kind, N)` facts (default 3/1); "set trigram weight to 0" flips a
  decode choice (`red apple pie` → `red apple juice`). The decoder's ranking is
  now inspectable KB knowledge, not hardcoded C. `gen_weight.chat` / `.it`.
- **gen110 (L13+) — planner conjunction + quantities.** "cake requires batter
  and oven" learns two facts; "batter requires 3 eggs and 2 flour" records
  `amount()` and the plan shows "3 eggs, 2 flour, …". `plan_qty.chat` / `.it`.
- **gen109 (L17 prose) — one-sentence word problems.** `mod_wordproblem` maps
  prose to an operation by semantic cues (held-out numbers and verbs), incl. the
  "how many more … than" → subtraction case; declines without a clear cue.
  `wordproblem.chat` / `.it`.
- **gen108 (L13) — ordered procedure to a goal (a tiny planner).** `mod_plan`
  takes `requires(Goal, Step)` facts taught in any order and answers "how do I
  make X?" with a topologically-sorted plan (DFS), prerequisites before
  dependents — derived, never stored. Honest on unknown goals and circular
  prerequisites. Bilingual ("per X serve Y" / "come faccio X?"). `plan.chat` /
  `plan.it.chat`.
- **gen107 (L17) — one-step algebra.** `mod_algebra` solves an equation with one
  unknown and one operation by applying the inverse: `x + 3 = 7` ⇒ `x = 4`. All
  four ops, both inverse directions (incl. unknown in a denominator), any operand
  slot, either side of `=`; declines on no finite solution; plain arith (no `=`)
  falls through. Proof states the inversion. Bilingual (symbolic core + filler/op
  words). `algebra.chat` / `algebra.it.chat`.
- **gen106 (L1) — a learned end-of-sequence token.** The autoregressive decode
  loop (gen36–42, induced `cont`/`cont2` continuation relation) gains a learned
  STOP: `learn_word_stream` induces a transition to `end_of_seq` at every
  sentence boundary, so decoding halts because the model *learned* where
  utterances end, not at the 24-step ceiling. STOP competes in the same
  frequency model and can outweigh a real continuation (`the cat sat . the cat
  sat . the cat sat the mat .` ⇒ `say the` ⇒ "the cat sat"). Cyclic grammars
  with no learned boundary still stream to the bounded backstop. Bilingual.
  `gen_stop.chat` / `gen_stop.it.chat`.
- **gen105 (L20) — reasoning about its own strategy.** `mod_strategy` answers
  "why did you answer *that way*?" from the real dispatch trace: the modules that
  ran and declined, the one that claimed the turn, and the first-match-wins rule.
  Derived from runtime state, never confabulated; committed only on non-strategy
  turns so it doesn't overwrite the decision it reports. Surfaced a real quirk
  ("2 + 2" → palindrome detector, not arith). Bilingual. `strategy.chat` /
  `strategy.it.chat`.
- **gen104 (L10) — few-shot pattern induction in one turn.** `mod_fewshot`
  reads 2+ "in -> out" exemplars and a "probe -> ?" on one line, induces the
  rule they share, and applies it to the held-out probe — answer *derived*,
  never stored. Four families: numeric delta/ratio, suffix transform, prefix
  transform, and **relational** (infer which KB relation the examples instantiate
  and resolve the probe from world knowledge — e.g. capital(rome,italy) +
  capital(paris,france) taught separately ⇒ "rome -> italy, paris -> france,
  berlin -> ?" ⇒ germany, and backwards). Bilingual via structure, not words.
  `fewshot.chat` / `fewshot.it.chat`.
- **gen101 (C15) — role/character memory.** `make impersonate` 15% → **100%**.
  `mod_role` parses role uptake from the user's words, answers in character from
  role state + `kb/roles.p0`, and keeps a **layered self-model**: a
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