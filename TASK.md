# Current task

> One goal at a time. When it is done, replace this with the next one.

> gen80-101 done — decomposition, priority, session stats, entities, hypothesis,
> explanation depth, module caps, negation, causal chains, confidence, correction,
> goals, bulk-forget, KB completion, impersonation benchmark, role memory.

## Done recently

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