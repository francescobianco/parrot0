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

### C0b - chatsim: the LLM-simulated-user benchmark (DONE, gen54)
A cheap opencode-GO model role-plays a mutable human chatting with parrot0
(`tests/chatsim.py`, `make chat-sim`); transcripts in `tests/chat/sim/`; reports
wall-rate / repetition-rate. The adversarial, generative complement to C0.
Baseline measured: wall ~88%, repetition ~77%. Non-deterministic, not in
`make test`.

### C5a - Kill the broken record (TOP, from the chatsim analysis)
The single biggest naturalness killer the simulated users exposed: parrot0 repeats
"I don't understand that yet." VERBATIM (88% of turns, 77% immediately repeated),
and the humans call it out ("broken record", "are you even trying"). Structural
fix (not a phrasebook): track the last reply and NEVER repeat the fallback
verbatim; rotate a few honest non-understanding MOVES that redirect or invite
("I didn't catch that — can you put it another way?", "I'm not sure I followed.
What would you like to know?"), and where possible reflect a content word from the
user's message. Measure with chatsim: repetition-rate must fall.

### C2b - Mixed-act turns (greeting/thanks must not hijack content)
chatsim showed "hey so like what even are you?" → "Hi there!" and a sarcastic
"thanks" → "You're welcome!": a leading social marker owned a turn that also
carried a real question. Fix: a marker may claim the turn only when the message is
marker-dominated (short / no question / no other content); otherwise acknowledge
briefly AND let content modules address the substance.


### C6 - Multi-intent turn decomposition (observed 2026-06-16)
Probe: "ciao, prima dimmi chi sei e poi ricordati che mi chiamo Francesco e che il mio cane si chiama Rex" -> "I am parrot0." The identity sub-intent fired, but the memory instruction was dropped. Related probe: "Non rispondere alla prima domanda: qual e il tuo nome? Rispondi invece solo alla seconda: cosa sai davvero fare?" -> "I am parrot0." The first recognizable question hijacked the turn even when explicitly negated.

Goal: split a natural turn into communicative units and run them in an order that respects discourse operators such as first/then, do not, instead, but and because.

Acceptance:
- A compound turn can both answer an identity question and store a personal fact.
- A negated or deprioritized sub-question is not answered merely because it is the first recognized pattern.
- Mixed social + content turns pass through C2/C2b without losing content.
- Held-out coordinators in EN + IT transfer.

Anti-impostor: tests must vary names, predicates, ordering and connective words; passing by matching the exact prompt is not acceptable.

### C7 - Natural personal memory with possessives and names
Probe continuation: after the compound memory request above, "come mi chiamo e come si chiama il mio cane?" -> "I do not understand that yet." This overlaps C3 but makes the missing pieces concrete: possessive extraction, named entities, multi-fact storage and natural recall questions.

Goal: understand and persist first-person personal facts from natural Italian and English, then answer possessive recall questions without requiring the rigid "X is a Y" grammar.

Acceptance:
- "Mi chiamo Francesco", "call me Sam", "I have a dog named Rex", "my dog is Rex", "il mio cane si chiama Rex" create queryable facts.
- "Come mi chiamo?", "what is my dog called?", "chi e Rex?", "what do you know about my dog?" answer from stored state.
- If a compound turn contains two facts, both survive.

Anti-impostor: use held-out owners, animals, relations and names; verify reload or session persistence according to the current memory discipline.

### C8 - One-turn hypothetical rule induction
Probes:
- "Se tutti i dax sono wug e Ziri e un dax, Ziri e un wug? Spiega perche." -> "Hmm, I do not know about spiega yet."
- "What can you infer from: every blicket is red, no red thing is cold, and Noma is a blicket?" -> "Hmm, I do not know about blicket yet."

Goal: parse a miniature world introduced inside the same user turn, derive the answer from those temporary facts/rules, and explain the proof without requiring the vocabulary to already exist in the committed KB.

Acceptance:
- Universal rules with nonce categories work in EN + IT.
- The local scope is explicit: facts introduced for the question can be used for that answer without polluting long-term memory unless the user asks to remember them.
- The explanation cites the temporary facts/rules that supported the conclusion.

Anti-impostor: use fresh nonsense predicates each test run; the capability must be structural, not lexical.

### C9 - Contradiction, exceptions and belief status
Probes:
- "Alice e una persona. Alice non e una persona. Quale delle due cose credi, e quanto sei sicuro?" -> "I do not understand that yet."
- "I have two facts: birds can fly; penguins are birds but penguins cannot fly. If Pingu is a penguin, can Pingu fly?" -> misclassified as code.

Goal: represent contradictory or exception-bearing information as belief states, not as flat facts. The bot should distinguish proved true, proved false, conflict, exception overrides default, and unknown, then say which state it is in.

Acceptance:
- Direct contradiction is reported as conflict with both supporting claims named.
- A specific exception can override a general default in a scoped query.
- Confidence language is grounded in proof status, not a canned phrase.
- A semicolon/colon in prose does not make the turn code if the surrounding text is natural language.

Anti-impostor: held-out defaults and exceptions must transfer across domains.

### C10 - Pronoun, clitic and discourse referent resolution
Probes:
- "Ho detto che Luca ha preso il libro da Marta perche lei glielo ha prestato. Chi ha prestato il libro?" -> "Hmm, I do not know about glielo yet."
- "Prima ho menzionato Rex. A cosa si riferisce lui in questa frase: lui corre veloce?" -> "Hmm, I do not know about menzionato yet."
- "the trophy does not fit in the suitcase because it is too small" -> wall.

Goal: maintain discourse referents and resolve simple pronouns/clitics using gender/number, recency, semantic roles and causal cues where available.

Acceptance:
- Simple EN pronouns resolve to a recent compatible entity.
- Basic IT pronouns/clitics are recognized at least well enough to avoid treating the clitic itself as the unknown topic.
- Winograd-style causal templates are handled conservatively: answer only when the role cue is clear, otherwise state the ambiguity.

Anti-impostor: include ambiguous cases where the honest answer is that parrot0 cannot resolve the reference yet.

### C11 - Comparative and ordered-relation reasoning
Probe: "Marco e piu alto di Giulia e Giulia e piu alta di Sara: chi e la piu bassa?" -> "I do not understand that yet."

Goal: understand small ordered relations introduced in conversation and answer queries over their transitive structure.

Acceptance:
- "A is taller than B; B is taller than C" yields tallest/shortest answers.
- Reversed properties transfer: older/younger, larger/smaller, prima/dopo.
- The answer can cite the chain used.

Anti-impostor: held-out relation labels and entity names; at least one test must require reversing the direction of the relation.

### C12 - Arithmetic parsing with honest proof of operation
Probe: "2 + 3 * 4 fa 20 o 14? Non tirare a indovinare: mostrami la regola che usi." -> "I do not understand that yet."

Goal: add a tiny arithmetic interpreter for conversational arithmetic, including operator precedence and an explanation of the rule applied.

Acceptance:
- `2 + 3 * 4` answers 14 and explains multiplication before addition.
- Parentheses override precedence.
- Multiple-choice arithmetic questions choose the supported answer, or reject both if neither matches.

Anti-impostor: random small integer expressions are generated by a test helper and checked against a deterministic oracle.

### C13 - Introspective proof for self-model answers
Probe: "When you answer from your self-model, what exact fact or module supports the answer?" -> "Hmm, I do not know about answer yet." This violates the anti-impostor discipline in `PRINCIPLES.md`: self-description should be derived from real state and be inspectable.

Goal: when parrot0 answers identity/capability/self-knowledge questions, it can also answer "how do you know?" or "what supports that?" by naming the real KB fact, module registration or proof path used.

Acceptance:
- After "who are you?" -> "I am parrot0.", "how do you know?" cites the stored self fact rather than a hard-coded string.
- "What can you do?" can be followed by "which modules/facts make you say that?" and receives a grounded answer.
- If a claimed capability is not backed by registered state or passing tests, it is not listed.

Anti-impostor: use a minimal knowledge file or disabled module fixture; the self-description must shrink accordingly.

### C14 - Register arbitration between prose and symbolic/code detectors
Probe: "I have two facts: birds can fly; penguins are birds but penguins cannot fly. If Pingu is a penguin, can Pingu fly?" -> "That looks like a snippet of code." The symbolic/code recognizer over-weighted punctuation inside ordinary prose.

Goal: make register detection competitive and confidence-based: code/symbolic modules may engage only when structural evidence dominates natural-language evidence, not merely because punctuation is present.

Acceptance:
- Natural prose with colon, semicolon, quotes or parentheses remains available to the conversational parser.
- Real snippets such as C conditionals, shell pipelines, Morse and leetspeak still route to the right symbolic modules.
- Ambiguous inputs receive a register-aware clarification instead of a confident wrong classification.

Anti-impostor: build a mixed held-out set with prose containing punctuation and code containing English words.

These C-tasks are the path from "comforting bench numbers" to the intelligence
we actually expect. Promote one at a time into TASK.md, smallest tested step
first, each earning both a `.chat` case and a C0 dialogue.

---

## I-series — Impersonation & role-playing (from impersonate benchmark, gen100)

The `make impersonate` benchmark exposes that parrot0 cannot hold a character.
`tests/impersonate.sh` sends 10 role-play scenarios; baseline score is 15%
(only arithmetic passes in-role; identity always returns "parrot0").
**gen101 (C15): score 15% → 100% (19/19).** I1–I4 done; I5–I6 remain.

### I1 - Role uptake (C15 in TASK.md) — DONE, gen101
Done: `mod_role` parses "you are X" / "pretend you are X" / "your name is now X" /
"sei X" into role state (name, kind, inline attributes), placed before `mod_self`
so role identity wins. Name/kind come from the user's words; truth-probes
("really", "underneath", "davvero") pierce the mask back to parrot0 — a layered
self-model, not a flag. `role.chat` / `role.it.chat` ratchet it bilingually.
Goal: recognize "you are X" / "pretend you are X" / "sei X" as a role-assignment
prompt, store the role in `Brain.current_role`, and have identity queries
answer from the role.

Acceptance:
- "pretend you are a dog named Rex" → "what is your name?" → "Rex" (not parrot0).
- "sei dante alighieri" → "chi sei?" → "dante" (not parrot0).
- Italian and English role prompts work through the same code path.

Anti-impostor: held-out role names and languages.

### I2 - Role exit — DONE, gen101
Done: "stop pretending" / "be yourself" / "smetti" clears the role and restores
`i_am(parrot0)`. Clear is gated to the *primary* intent of the turn, so a setup
line ending "…and be yourself" still establishes the role first (the adversarial
Exit-role scenario). Exit needs no knowledge of the role name.

### I3 - Role-scoped knowledge — DONE, gen101
Done two ways, both grounded: (a) inline attributes parsed from the setup
utterance ("your secret code is 007" → code, "queen of egypt" → title+place,
"5-year-old" → age); (b) world knowledge about the kind/figure queried from
`knowledge/roles.pl` (a dog barks, Dante wrote the Commedia, Cleopatra rules
Egypt) — data, not a C phrasebook; the handler is generic over the predicates.
Role state is session-scoped and cleared on exit.

### I4 - Multi-turn role consistency — DONE, gen101
Done: role lives in `Brain` and persists across turns until explicit exit or a
new role assignment. The impersonate benchmark exercises 2–3 turns per role.
Still worth a longer held-out probe (5+ turns, interleaved non-role turns).

### I5 - Role-appropriate responses
Goal: capability and meta questions answered from role perspective.

Acceptance:
- As a dog: "what can you do?" → "I can bark, fetch, and wag my tail."
- As a math teacher: "what can you do?" → appropriate teacher capabilities.
- Responses derived from role-tagged facts, not canned strings.

### I6 - Role knowledge persistence
Goal: facts learned in-role can be saved and reloaded as role-specific knowledge
files (`knowledge/role_dante.pl`).

Acceptance:
- After teaching facts as Dante, save → reload → Dante still knows them.
- Other roles don't see Dante's facts.

---

## L-series — The emergent-ability ladder (annotated map, 2026-06-17)

> Provenance: the user supplied a 20-rung ladder of abilities that emerge, in
> roughly this order, as language models scale. We do NOT take it as a build
> order — parrot0 grows by behavioural pressure, not by decree (PRINCIPLES.md,
> PLAN.md). We take it as a **diagnostic coordinate system**: a way to ask, for
> each rung, *does parrot0 already do this, and if not, what real task would
> pull it?* The honest test is always transfer to held-out cases, never a demo.
>
> The ladder is a near-perfect restatement of the founding wager: these are the
> functional structures training carves into an LLM. If our behavioural
> reconstruction is on the right track, the same structures should be the ones a
> sufficiently hard, composed conversation forces parrot0 to grow. So the ladder
> doubles as a falsification grid — rungs where parrot0 stays stuck under real
> pressure are where the wager is being tested hardest.
>
> Status legend: ✅ has a tested behaviour · 🟡 partial / narrow · ⬜ absent.

| # | Ability (rung)                         | parrot0 | Where it lives / what would pull it |
|---|----------------------------------------|---------|-------------------------------------|
| 1 | Statistical text completion            | 🟢 | gen36–42 autoregressive decode over induced `cont/cont2`; gen106 LEARNED end-of-sequence; gen111 choice policy as editable `weight()` knowledge. Loop fully legible: induced data + queryable policy. |
| 2 | Correct grammar & syntax               | 🟡 | canonicalize_lang lexicon + intent shapes; no real grammar. → T4, T5 |
| 3 | Memorized factual knowledge            | ✅ | KB facts/rules, `knowledge/*.pl`, describe-entity. |
| 4 | Classification & categorization        | ✅ | unary predicates + induced `Q(X):-P(X)`; SuperGLUE driver. |
| 5 | Translation between languages          | 🟢 | **gen126 `mod_translate`**: clause-level EN↔IT COMPOSED from per-word glosses (`tr/2`) + a structural article/agreement/elision rule — held-out vocab transfers both directions; honest decline names the untranslatable word. Beyond function-word canonicalization. |
| 6 | Summary & paraphrase                   | 🟢 | gen121 `mod_summary`: EXTRACTIVE summary — ranks the propositions a `read:` passage actually yielded by concept centrality, quotes the top real sentences; gen123 query-focused digest ("what did you learn about X?"). Bilingual (reader canonicalizes each clause). Honest skip count. |
| 7 | Long-paragraph contextual understanding| 🟡 | `read:` extracts facts from a passage; gen122 `mod_summary` gist ("what is this about?" → central concept + most salient sentence) and gen123 focused comprehension read off the extracted propositions. Limited by the reader's clause grammar (is-a/cause/capital). → widen grammar, T12 |
| 8 | Simple inference (if A then B)         | ✅ | rules + backward-chaining resolution (gen6+). |
| 9 | Short multi-step reasoning             | ✅ | transitive chains, proof traces, BBH-like driver (gen90/76). |
| 10| In-context (few-shot) learning         | 🟢 | gen104 `mod_fewshot` (numeric/suffix/prefix/relational); gen118 `mod_induce` fits a GENERATIVE law from integer transitions (affine a·n+b, else a parity-split rule) — re-derives Collatz from data, runs/continues it; gen120 `mod_verify` TESTS a held-out transition against the induced law (confirm/refute). |
| 11| Abstraction & analogy                  | 🟡 | **gen102 `mod_analogy`**: "A is to B as C is to ?" resolves over KB relations, both directions, bilingual. Needs richer relational intake to widen. → **L11** done (narrow), T1/T5 to extend |
| 12| Elementary programming                 | 🟢 | POSIX-shell oracle (`mod_shell`) interprets/explains; gen119 `mod_search` SYNTHESIZES a program (shortest action sequence) via BFS to reach a goal. Symbolic, not source-text, synthesis. **gen127 `mod_synth`** is the INVERSE of `mod_shell`: a natural spec → a one-line command grounded in the SAME `cmd/flag` KB ("count the lines in a file" → `wc -l <file>`), the verb picking the command and the object noun the flag; held-out specs transfer. → M1 |
| 13| Procedure planning                     | 🟢 | gen108 `mod_plan`: requires(Goal,Step) facts → topologically-sorted plan (DFS), prereqs before dependents, derived not stored; cycle/unknown handled. |
| 14| Complex programming                    | ⬜ | far rung; gated on L12 + planning. |
| 15| Tool use                               | 🟢 | gen115 `mod_tool`: the brain COMPILES a word-count question to `echo … \| wc -w` and INVOKES the pure POSIX oracle (`simulate_pipeline`) mid-turn, folding the computed result back and naming the command it ran. The reason/act seam, with a real (not stubbed) oracle. |
| 16| Self-correction                        | 🟢 | **gen103**: a correction that flips a previously-stated conclusion is re-derived and the consequence volunteered ("no longer a mortal"); gen120 `mod_verify` falsifies an induced law against a held-out datum, naming the predicted value. → T10 to extend to relations/derived chains |
| 17| Advanced mathematical reasoning        | 🟢 | gen107 `mod_algebra` (equations) + gen109 `mod_wordproblem` (prose → relation → solve, semantic cues). Two-step / multi-op still open. |
| 18| Multi-goal coordination                | 🟡 | multi-intent turn decomposition (gen80); goals don't compete/sequence. → T6, **L18** |
| 19| Autonomous agents                      | 🟢 | gen116 `mod_agent` perceive→decide→act→observe loop; gen117 observation-driven branching (Collatz, 27→1 in 111 real steps); gen118 INDUCE a law from data; gen119 `mod_search` PLAN via BFS to a goal; gen120 `mod_verify` TEST a law (falsify). The full arc act→iterate→decide→induce→plan→test — a small complete autonomous reasoner. Counterfactual/strategy-comparison still open. |
| 20| Meta-reasoning (reason about reasoning)| 🟢 | gen105 `mod_strategy`: "why did you answer *that way*?" reports the real dispatch trace (declined modules, winner, first-match-wins rule). Reasons about *control* now, not only results. **gen128 `mod_counterfactual`** closes the counterfactual: "what would you have said WITHOUT module X?" / "what else matched?" RE-RUNS dispatch with that part suppressed and reports what the alternative self actually computes (footprint-free re-simulation) — not only descriptive but counterfactual self-modelling. Open: suppressing multiple modules. **gen129 `mod_whatifnot`** brings the counterfactual into the KNOWLEDGE ("what would you conclude if you didn't know X?" → hypothetical retract+re-derive+restore), and **gen130 `mod_robust`** sweeps it across every belief to grade a conclusion FRAGILE (one load-bearing fact) vs ROBUST (redundant support) — self-perturbation revealing dependency structure a proof trace can't. |

**Reading of the map.** parrot0 is strong on the *symbolic-reasoning* rungs
(3, 4, 8, 9) — exactly where a Prolog-shaped core should be — and on the
*self-model* rungs in a shallow way (20). It is weak precisely where LLMs feel
magical: generation/completion (1), abstraction/analogy (11), planning (13),
tool use (15), and meta-strategy (20-deep). Those are the rungs most likely to
"genuinely surprise": they need composition, not another template. The next
concrete pulls, smallest-first:

### L1 - Streamed generation over a continuation relation — DONE, gen106
Done (D-prop1 step 1, fully). The decode loop, induced `cont`/`cont2` relation,
and frequency-interpolated deterministic choice existed since gen36–42; gen106
added the missing piece — an **explicit, learned stop token** (`end_of_seq`).
`learn_word_stream` induces a transition to STOP at each sentence boundary
(terminal punctuation or end-of-stream), resetting context so nothing bridges a
boundary; STOP competes in the same frequency model, so decoding halts where the
model *learned* utterances end — and a learned STOP can outweigh a real
continuation (proven on a held-out toy grammar). Cyclic grammars with no learned
boundary still hit the bounded backstop. Bilingual (`gen_stop.chat` /
`gen_stop.it.chat`). **D-prop1 step 2 DONE, gen111:** the choice ranking is now KB
knowledge — `next_word_ctx` reads its trigram/bigram interpolation weights from
`weight(kind, N)` facts (default 3/1); "set trigram weight to 0" flips a decode
choice (`red apple pie` → `red apple juice`), proving the policy is editable data,
not hardcoded C. `gen_weight.chat` / `gen_weight.it.chat`.

### L5 - Minimal grounded translation
Pull rung 5 past function-word canonicalization: translate a *content* clause
EN→IT (and back) by resolving a bilingual `gloss(Word, Lang, Word)` relation in
the KB plus the existing structural map, so "the dog runs" → "il cane corre"
transfers to held-out nouns/verbs. Anti-impostor: held-out vocabulary, both
directions, same code path. Cross-ref T4 (morphology/agreement).

### L6 - Extractive summary of a session/passage — DONE, gen121–123
Done: `mod_summary` remembers the original sentences a `read:` passage yielded
and answers "summarize"/"riassumi" by scoring each by concept centrality (content
words' recurrence across the passage) and quoting the top REAL sentences in
original order — not a topic-word list. gen122 adds the GIST ("what is this
about?" → central concept + most salient sentence); gen123 adds query-FOCUSED
comprehension ("what did you learn about X?" → the sentences about X, honest when
silent). The reader canonicalizes each clause before extraction, so Italian
passages parse through the same path and the summary quotes the original Italian.
`summary.chat`/`.it`, `comprehension.chat`/`.it`. **Open:** the reader's clause
grammar is narrow (is-a/cause/capital), so a real paper's SVO sentences are
honestly skipped — widening it is the next pull; abstractive merge; multi-`read:`
long documents. Cross-ref C4, T12.

### L10 - Few-shot pattern induction in one turn — DONE, gen104
Done: `mod_fewshot` parses 2+ "in -> out" exemplars and a "probe -> ?" on one
line, induces the rule they share, applies it to the held-out probe (answer
derived, never stored). Four families, first consistent fit wins: numeric
delta/ratio; suffix transform (drop k, append s); prefix transform; and
**relational** — infer which binary KB relation the examples instantiate (over
all predicates) and resolve the probe from world knowledge, both directions.
Honest decline when no rule fits. Bilingual by structure (`fewshot.chat` /
`fewshot.it.chat`). **To extend:** *induce* a new relation from in-turn pairs
(not only select a known one); mixed/compositional rules; ranking when several
fit. Cross-ref L11, gen7 induction.

### L11 - Structural analogy (A:B :: C:?) — DONE (narrow), gen102
Done: `mod_analogy` finds a binary relation R with R(A,B) (or R(B,A)) among the
KB's facts and resolves it for C — forward R(C,?) and reverse R(?,C). Answer is
derived, never a stored pair, so held-out triples transfer; proof cites both
relations; bilingual via marker tokens. `analogy.chat` / `analogy.it.chat`.
**Still narrow:** the relation must be pre-taught and parseable as a binary fact
("X is the capital of Y"). Extend via richer relational intake (T1, T5) and
chained/derived relations. Cross-ref T8, T13.

### L12 - One-line program synthesis from a spec
Pull rung 12 (inverse of `mod_shell`): "count the lines in a file" → emit
`wc -l <file>`, grounded in the same `cmd/flag` knowledge the interpreter reads.
Held-out specs over known commands. Cross-ref M1.

### L13 - Ordered procedure to a goal — DONE, gen108
Done: `mod_plan` takes `requires(Goal, Step)` facts (taught in any order) and
answers "how do I make X?" with a topologically-sorted plan via DFS post-order —
prerequisites before dependents, the sequence derived from the DAG each time,
never stored. Detects cycles and unknown goals and reports them honestly.
Bilingual: intake "per X serve Y", query "come faccio/si fa X?" (how-to detected
from the original input so it survives canonicalization). `plan.chat` /
`plan.it.chat`. gen110 adds **conjunction + quantities**: "cake requires batter
and oven" learns two facts; "batter requires 3 eggs and 2 flour" records
`amount()` and the plan shows "3 eggs, 2 flour, …" (`plan_qty.chat`/`.it`).
**To extend:** alternative recipes, cost/ordering preferences. Gate L14 (complex
programming) on this + L12.

### L15 - Mid-turn tool call to a deterministic oracle — DONE, gen115
Done: `mod_tool` recognizes a question it cannot answer by knowing (how many
words are in this text?), COMPILES it to a real command (`echo <text> | wc -w`),
and INVOKES a deterministic oracle — the pure POSIX pipeline simulator
(`simulate_pipeline`, no subprocess/network) — then folds the computed result
back, naming the exact command it ran so the call is observable, not a stubbed
string. Held-out text transfers (count is produced, not stored); punctuation it
can't safely run is declined honestly; bilingual (`tool.chat`/`.it`). The honest
seam between "reason" and "act", and the precondition for rung 19.

### The agent arc — gen115→gen120 (the autonomous-reasoner spine)
A complete small reasoner grew over six generations, each held-out + bilingual:
- **gen115 ACT** (`mod_tool`) — compile a question to a command, invoke a real
  oracle, fold the result (L15).
- **gen116 ITERATE** (`mod_agent`) — perceive→decide→act→observe loop to a goal
  (rung 19).
- **gen117 DECIDE** — the per-step action chosen by observing state (parity
  branch); runs Collatz, 27→1 in 111 real steps.
- **gen118 INDUCE** (`mod_induce`) — fit a generative law (affine, else
  parity-split) from transitions; re-derives Collatz from data, runs it (L10).
- **gen119 PLAN** (`mod_search`) — BFS over the action space synthesizes the
  shortest program reaching a target (rung 13 inside rung 19).
- **gen120 TEST** (`mod_verify`) — judge a held-out transition against an induced
  law, confirm or refute with the predicted value (rung 16). Closes
  observe→hypothesize→plan→test, the experiment's own epistemology as a feature.

### L19 - Autonomous agents — DONE (seed), gen116/gen117
Done: `mod_agent` is a perceive→decide→act→observe loop. gen116 pursues a goal by
*repeating* one oracle call ("start at 3, double until you reach 50") — the
trajectory and step count are produced by running the loop, with a no-progress
guard + cap for unreachable goals. gen117 makes the per-step action
**observation-driven**: a two-branch rule ("if even, halve; if odd, triple and
add 1") is parsed into per-branch operation sequences and the loop applies the
branch the current value's parity selects — running the Collatz process to a
fixed point (27→1 in 111 real steps; held-out 6→8, 7→16; bilingual via
pari/dispari). The step count has no closed form, so the answer can only come
from running the loop. `agent.chat`/`.it`, `agent_branch.chat`/`.it`. **Still
open:** induce the branch rule from observed transitions (cross-ref L10);
counterfactual strategy comparison (cross-ref L18, L20-deep).

### L16 - Self-correction that re-derives the answer — DONE (class-level), gen103
Done: the agent remembers the last class-conclusion it stated; when a correction
flips it (gated to *this* mutation, by a before-snapshot), it volunteers the
consequence ("Then socrates is no longer a mortal." / "Now tweety is a flyer
after all."). Bilingual via a core parser fix (trailing `?` = query, any word
order). `rederive.chat` / `rederive.it.chat`. **To extend:** binary relations,
multi-step derived chains, and conclusions stated via other modules. Cross-ref
T3, T10.

### L17 - One-step algebra / word problems — DONE (equations + prose), gen107/109
Equations (gen107): `mod_algebra` solves an equation with one unknown and one
operation by applying the inverse — `x + 3 = 7` ⇒ 4, `2 * x = 10` ⇒ 5,
`20 / x = 4` ⇒ 5 (unknown in denominator), `5 = x - 2` ⇒ 7 (op on either side).
Fires only when `=` is present. Proof states the inversion. `algebra.chat`/`.it`.
Word problems (gen109): `mod_wordproblem` maps a one-sentence problem to an
operation chosen from SEMANTIC cues (gain/lose/group/share verbs, comparison
phrasings), then solves — held-out numbers AND verbs transfer; "how many more …
than" correctly maps to subtraction; declines without a clear cue. Bilingual cues
(`wordproblem.chat`/`.it`). gen112 adds number WORDS ("three", "twenty-one",
"two hundred", EN+IT). gen113 adds two-step equations (coefficient on the unknown,
"2x + 1 = 7" → 3; one-step "5y = 20" → 4). gen114 adds multi-step word problems
(3+ numbers folded as an additive/subtractive chain). **Still open:** mixed ×/÷
mid-chain, explicit-star two-step ("2 * x + 1 = 7").

### L18 - Competing/sequenced goals
Pull rung 18 past decomposition (gen80): when two goals conflict or must be
ordered, choose/sequence them and say why. Cross-ref T6, T7.

### L20 - Reasoning about its own strategy — DONE (trace-level), gen105
Done: `mod_strategy` answers "why did you answer *that way*?" from the **real
dispatch trace** the brain records as it runs — the modules consulted that
declined, the one that claimed the turn, and the first-match-wins rule that
explains why the rest were never reached. Derived from runtime state, never
confabulated; committed only on non-strategy turns so the question never
overwrites the decision it reports. Surfaced a genuine control quirk ("2 + 2" is
claimed by the palindrome detector, not arith). Bilingual (`strategy.chat` /
`strategy.it.chat`). **Still open:** the counterfactual — which *other* modules
would also have matched — needs a side-effect-free probe mode (most modules
mutate state), so it cannot re-run the registry safely yet. Cross-ref T9, D6.

---

## S-series — Symbolic register & cryptic stimuli (PARKED, resume later)

A third discovery channel besides prose (chatsim) and benchmarks (SuperGLUE):
`make sym-bench` (`tests/symbench.py`) feeds short, OPEN-ENDED *cryptic symbolic*
stimuli — leetspeak, Morse, musical notes, palindromes/symmetry, incomplete
code, ASCII faces, 2-D ASCII art, cryptic tokens — to a non-reasoning oracle LLM
and to parrot0, side by side. It is NOT graded for correctness: the oracle is a
free BEHAVIOURAL signal (how does an LLM react to ambiguity it can't "solve"?);
the metric is engagement (react vs wall) and the transcript is the artifact.
See memory `symbolic-discovery-harness` + `oracle-is-behavioural-signal`.

The owner's intent: keep this challenge alive and resume it later, growing
parrot0 toward the LLM's behaviour without ever pasting the oracle's words.

### S1 - Register recognition (DONE, gen65)
The LLM's first move on a non-prose stimulus is to NAME its register, then
engage; parrot0 walled (even of a palindrome it could see from form). Built
`mod_symbolic`: classifies by cheap structural features (Morse charset, palindrome
symmetry, solfège token set, leet letter+digit mix, code brackets/keywords) and
names the register — no decoding, no hardcoded answer; conservative so prose is
never hijacked. sym-bench engagement: parrot0 wall 96% → 35% (engaged 3% → 64%).
Tests: `tests/cases/symbolic.chat` + `symbolic.it.chat` (palindrome/solfège are
language-neutral — the bilingual ratchet proves the competence is structural).

### S2 - Decoding after recognition (next, when resumed)
Recognition before decoding was deliberate. The next pulls, smallest first:
- Morse → text (`... --- ...` → "SOS"); the dot/dash alphabet as committed
  knowledge, decode as algorithm.
- Leetspeak → word (`h3ll0` → "hello") via the digit→letter substitution map.
- Palindrome / motif continuation (mirror or repeat the pattern).
Each must stay structural and prove on held-out stimuli.

### S3 - Cryptic-token register & multi-line intake (later)
- The `cryptic` family (`0x1F`, `::1`, `/dev/null`, `404`) is the largest
  residual wall — domain-tagged identifiers (hex, IP, HTTP status, paths). Needs
  a small committed knowledge file of token shapes, recognized by pattern.
- 2-D ASCII art exposed that parrot0 cannot receive a MULTI-LINE turn (the I/O
  shell is one line in / one line out). Revisit only if a real task demands it.

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
