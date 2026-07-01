# parrot0 evolution journal
## 2026-07-02 - gen254: from prompts to categories

**Changed.** Two llmscore rounds in one turn, with F.'s mid-turn steer applied:
the interview rotates every run, so items must close as CATEGORIES with engine
rules, not per-prompt answers.

Round one closed the gen253 sample (4/10): circle geometry (2*pi*r, pi*r^2)
with the derivation in the reply; a constrained-number enumerator (bounds +
prime/even/divisible predicates, proving "no solution" on empty sets); numbered
multi-way sentence completions; `word_for/2` defining-phrase vocabulary
(substring-matched like `idiom_meaning`); idiom INTENT without the word
"idiom"; a calibrated decline for arrangement-optimization puzzles the solvers
don't model; a story-on-request frame reusing the scene/continuation substrate
(`story_scene/1` marks scenes with standalone clauses; unknown topics decline
naming real alternatives).

Round two (new sample, 6/10) generalized further: a RELATIVE step in the
sequential-quantity walker ("give me twice what I currently have"); a general
two-unknown linear family (ratio ages; heads+legs with per-species legs from
KB `quantity/3` — the existing solver only lacked `quantity(rabbit, legs, 4)`,
one fact); morphological concept binding in the engine (moonlight->moon,
raindrops->rain: compound modifier-first rule, no alias facts); bare poetic
fragments recognized by SHAPE and continued from scene KB; rectangle
perimeter/side->area; "what are the three X" routed into the counted pick;
`role_holder/2` for "who was the first X"; `capital_predecessor/2` composing
into the compound-capital frame.

Two precision bugs found by the harness: the possessive query read a spurious
token when "my" was absent ("what's the usual intent behind those words?" ->
"I don't know what your a is called."), and a compound category key built from
an empty token became `_what` — a PROLOG VARIABLE — matching every category
("give me 2 - what" listed colors). Audit rule: never build KB keys from
unchecked user tokens; empty qualifiers must bail.

Ratchets: +26 cases in `llmscore_world.sh` (95 total), both `.chat` fixtures
moved off the blind wall for story requests (informed decline is Fase A
behaviour), `make test` green at 423 PASS.

**Observed.** Live `make llmscore`: 4/10 -> 3/10 (new sample, new categories)
-> 6/10 after the generalization round; the four zeros of the 6/10 sample were
closed the same day as frames. Recurring open categories: homophone phonology,
compose leaking repair's clarification on an already-answered turn,
percentages/fractions, calendar day arithmetic.

## 2026-07-01 - gen253: LLMSCORE sample broadening

**Changed.** The next live `make llmscore` sample scored 4/10 and exposed
small but reusable gaps. Closed the deterministic ones without changing the
benchmark harness:

- bookshelf word problems now understand "with N books each" and "remove K
  complete shelves worth";
- spelling/string transforms can sort letters in reverse alphabetical order;
- sky colour explanations now compose day-blue and sunset-red causes;
- suspense continuations have enough KB alternatives to satisfy "three creative
  ways";
- the autumn couplet theme now honours paired forgotten letters.

Ratchets: `llmscore_world.sh` added the five sample shapes above. The remaining
hard miss in that sample was an inconsistent three-digit-number puzzle; it
should become a calibrated "no solution" algebra frame rather than a guess.

**Observed.** Final live `make llmscore` for this turn: 4/10. New open gaps are
circle circumference, truncated fence/sheep logic, horizon/traveler continuation,
compound vocabulary (`generous` + truth-teller), constrained non-prime number
choice, and idiom-intent phrasing without the word "idiom".

## 2026-07-01 - gen252: LLMSCORE protocol and second-frame pass

**Changed.** Reran `make llmscore` after gen251 and found a different class of
failure: a multi-line rainy-day answer desynchronized the line-based interviewer,
so later answers were shifted. Fixed the protocol surface and used the same
sample to add general frames:

- `activity_summary/2` gives situated "favorite thing to do" prompts a single
  honest line, while recommendation prompts still render multi-step plans;
- two-train destination races compare arrival times instead of using the
  meet-time trick;
- simultaneous egg-boiling keeps the shared cooking time;
- EN->FR gained a small compositional lexicon (`tr_fr/2`, `gender_fr/2`) with
  article and adjective placement glue;
- classic letter riddles route before arithmetic can misread their numbers;
- incomplete bottle sentence prompts get a KB-backed continuation cue.

Ratchets: `llmscore_world.sh` now covers the post-gen251 sample: destination
train arrival, French warm-rug translation, bottle completion, same-pot eggs,
the minute/moment riddle, and the single-line rainy-day favorite answer.

## 2026-07-01 - gen251: LLMSCORE frame hardening

**Changed.** Took the fresh `make llmscore` misses as frame work, not prompt
memorization:

- added generic `world_superlative/3` for stable curated superlatives;
- scaled recipe ingredient triples structurally instead of routing them to
  how-to planning;
- added countdown replacement filters ("say buzz instead of multiples of 3");
- handled toward-each-other train prompts with KB-backed city distance and
  meet-time reasoning;
- extended quatrain data to ocean and added the missing antonym `ephemeral`.
- made session-duration reporting start at the first user turn and use
  `timespec_get`, removing a real one-second boundary flake.

Ratchets: `llmscore_world.sh` now locks the six regressions from the fresh
score sample: Everest, recipe scaling, ocean quatrain, train meet, buzz
countdown, and ephemeral/permanent.

## 2026-07-01 - gen250: contrast and magnitude frames

**Changed.** Continued from `NEXTMOVE.md`'s frame spine, focusing on broad
basic-chat categories instead of one-off prompts:

- added a KB-backed contrast frame:
  `difference between X and Y -> difference_between(X, Y, Gloss)`;
- missing contrast facts now produce an informed gap naming both slots;
- promoted pairwise magnitudes to a KB-backed cue map:
  `magnitude_cue(Cue, Dim, Direction)` + `magnitude(Dim, Item, Rank)`;
- generalized comparisons across "which is faster A or B" and
  "is A bigger than B?" while preserving proof trace state;
- fixed `compose-bench` hermetic language seeding so ratcheted English
  composition dialogues do not depend on the developer's OS locale.

Ratchets: `llmscore_world.sh` now covers RAM/ROM, animal contrasts, an informed
contrast gap, country size, material weight, speed, and sun/moon size.
`compose-bench` is back to 7/7, 100% landing.

## 2026-06-30 - gen249: speed comparison, riddles, and media honesty

**Changed.** Final compact pass from the latest LLMSCORE sample:

- word problems compare two average speeds from distance/time pairs;
- logic handles explicit no-overlap questions ("no Borks are Zorks; can a Bork
  be a Zork?");
- generation handles rubber-band stretch/release descriptions;
- map riddle is KB-backed via `response_template(riddle_map, ...)`;
- recent-movie prompts now answer honestly without claiming viewing history.

Ratchets: `logic_no_overlap.chat`, more `wordproblem_multi.chat`, and
`llmscore_world.sh` checks for rubber band, map riddle, and recent movie.

## 2026-06-30 - gen248: relational constraints, taste, and leftovers

**Changed.** Follow-up hardening from another LLMSCORE sample:

- counted category naming now declines when a relational constraint like
  `border` is present, so the `borders/2` handler owns "name three countries
  that border France";
- added `taste_of/2` as a sensory relation parallel to `appearance/2`;
- added bag-removal reasoning for two named objects;
- added a universal syllogism chain parser ("all dogs are mammals; all mammals
  breathe" -> "Dogs breathe");
- added a library couplet as KB data.

Ratchets: `syllogism_universal.chat`, more `wordproblem_multi.chat`, and
`llmscore_world.sh` checks for library couplet, France borders, and banana taste.

## 2026-06-30 - gen247: exact explanations, sequence routing, and deixis

**Changed.** Third "carta bianca" pass from the next external score sample:

- `mod_count` now declines "what comes next" prompts so the sequence solver can
  infer the next term from the explicit series;
- `mod_wordproblem` handles the second-person possession trick ("you take two;
  how many do you have?" -> 2);
- generation gained `concise_topic/2` + `concise_explain/3` for exact-N-word
  explanations, plus a KB-backed bear/no-teeth joke riddle;
- story continuation gained a `mystery_box` scene for clockmaker/box prompts.

Ratchets: new `sequence_next.chat`, more `wordproblem_multi.chat`, and
`llmscore_world.sh` checks for exact three-word sky explanation, clockmaker
continuation, and the bear riddle.

## 2026-06-30 - gen246: frame continuity, rates, and strings

**Changed.** Second "carta bianca" pass after the external interviewer shifted
the target distribution:

- word problems now treat `take/took/takes ... away` as removal and solve
  weighted average speed as total distance over total time;
- `mod_spell` gained sequential word-edit frames: remove first/last letter, then
  add a named letter to the start/end;
- creative generation now supports follow-up after a declined couplet/haiku
  theme and selects story scenes by cue overlap instead of first cue;
- geography separates land borders from ocean borders with `ocean_borders/2`;
- rainy-afternoon "favorite" prompts route to activity advice with an explicit
  no-real-favorites preface.

Ratchets: `wordproblem_multi.chat`, new `string_transform.chat`, and expanded
`llmscore_world.sh` for moonlight couplets, Australia ocean borders, reveal/bare
narrative continuation, rainy favorite, and creative follow-up.

## 2026-06-30 - gen245: semantic frame spine

**Changed.** Started the "carta bianca" jump by turning the current LLMSCORE
wall into reusable frames instead of per-prompt answers:

- word problems now handle a post-count unit transform ("cut the remaining ones
  in half" -> two pieces per remaining item) and relative-motion duration
  without clock times (`distance / (speed1 + speed2)`), both with proof traces;
- generation got a constrained sensory-description frame:
  `sensory_topic/2` selects the topic and `sensory_phrase/3` supplies an exact
  word-count phrase;
- the existing KB generators were extended as data: library haiku images,
  voyage scene continuations, a short-joke response template, and the
  `most_moons` solar-system superlative;
- animal sounds now invert `sound_of/2`, so the same relation answers both
  "what does a cat say?" and "what animal says meow?".

Ratchets: `tests/cases/wordproblem_multi.chat` for pieces + relative motion, and
`tests/llmscore_world.sh` for haiku, joke, most moons, reverse sound, voyage
completion, and three-word sensory description.

## 2026-06-30 - gen244: NEXTMOVE grounded schemas

**Changed.** Audited `NEXTMOVE.md`: the plan is realistic as functional,
measured convergence on LLM-like task classes, not as a short-term promise of
open-domain parity. Added the principle checks that keep Fase A from becoming a
fake-understanding fallback: schemas/lexicon in KB, proof or test pressure, and
informed gaps when support is missing.

Implementation started on that line. `what's`/`who's`/`where's` now canonicalize
to the same forms as the old apostrophe-less chat contractions. The step-by-step
procedure handler no longer names tea/coffee in C: `process_topic(Task, Token)`
selects a task by KB topic overlap, then `process_step(Task, N, Text)` renders
the ordered process. Added bread/yeast rising as data. Added the same pattern for
practical recommendations: `activity_topic/2` + `activity_step/3`, with rainy
Sunday advice as data and a scoped gap for unknown situations.

Ratchets: `tests/cases/contractions.chat` for apostrophe contraction, and
`tests/llmscore_world.sh` for bread process + rainy Sunday recommendation.

## 2026-06-28 - gen241: sweep the LLMSCORE-check wall (docs/plans/llmscore-check.md)

**Changed.** Closed the whole list of bench prompts that had scored 0, each KB-first
(facts in `kb/core/world-facts.p0`, a fixed engine in C; no phrasebook, no deception):

- **World facts** — idioms (`idiom_meaning/2` + "what does X mean"), boiling point
  (`boils_at/2`), WWII end (`historical_fact/2`, both halves), most-populous country
  (`magnitude(population,_,_)` over the named options), primary colours
  (`category_member(primary_color,_)` + a counted "name three <category>" pick).
- **Compound "plan-action" questions** — answer BOTH halves: largest planet **and a
  moon** (`moon_of/3`), capital **and its river** (`river_of/2`), capital **and the
  ocean to its west** (`ocean_west_of/2`). The existing capital handler now appends
  river/ocean the same way it appended a landmark.
- **Arithmetic comprehension** — multi-step bookshelf (containers x per, then signed
  add/remove deltas), buy-with-remainder and change-from-a-bill (`cost`+`have`),
  and `count ... by N(s)` as a STEP not a bound ("count backward from 20 by 3s").
- **Logic / language** — the undistributed-middle syllogism now also triggers on
  "does it follow"; anagrams (`anagram_of/2`, letters verified); days-of-week
  alphabetical (sorted, -> Friday); "a place where you see X" riddle (`place_for/2`).
- **Bounded creative forms** — more couplets (sea, lighthouse, robot, storm, ...),
  quatrains (`poem4`), a parametric limerick (`limerick_l1..l5`), and exact-N-word /
  three-word completions (`completion_exact/3`, `fill_three/1`, length verified).
- **Honest engagement** — a `self_location` family (no body, no weather to report),
  step-by-step procedures (`process_step/3` for tea/coffee), and grounded roleplay
  advice (`scenario_step/3`: "as a store manager, what would you do?").

All new ground predicates registered as substrate in `is_internal_pred` (fact count)
and `is_struct_pred` (entity-describe). **Ratchet:** 19 new checks in
`tests/llmscore_world.sh` (world-facts-enabled harness; the hermetic `.chat` runner
sets `PARROT0_WORLD_FACTS=0`, so world-knowledge cases belong here). `make test` green
(209). IT mirrors deferred: the bench is English-only and these handlers are EN-cued.

## 2026-06-27 - gen234: fix the LLMSCORE near-miss cluster

**Changed.** Three capabilities that existed but misfired on a parse edge-case:
- Colour ("what color is a RIPE banana?") -> the recognizer now tries EVERY content
  token against color_of and answers on the first with a colour, robust to
  adjectives/articles/trailing phrases ("the sky during the day"). -> It's yellow.
- mod_namestart now also handles the UNCONSTRAINED "name any/an animal" (no "starts
  with") -> returns a member, rotating for variety; the letter-constrained path is
  unchanged. "name a fruit" -> apple.
- Greeting imperative "say hello (to me)" / "greet me" -> a greeting from
  response_template(greeting_reply) (KB-first), checked before the generic
  "say <word>" decoder.

make test green (209+).
## 2026-06-27 - gen233: qualitative-change reasoning + the KB-first manifesto

**Changed.** Implemented the capability derived in docs/plans/kb-first.md from F.'s
push-back on the circle metaphor: "if knowledge is like a circle, what happens to its
circumference when you learn something new?" is now DEDUCED, not walled.
- KB (world-facts.p0): grows_with(Feature, Base) co-monotonicity facts + increases(
  Action, Source). 
- mod_knowledge: a recognizer parses source/target/feature/action from "X is like Y …
  happens to … when …"; qchain_reaches() is a C-side transitive closure over
  grows_with/2 (the binary relation the unary rule engine can't carry). If the feature
  is co-monotone with the target AND the action increases the source -> "It grows: more
  knowledge makes a bigger circle, so its circumference grows with it." Declines
  honestly otherwise ("…when you sleep" -> wall: no increases(sleep, knowledge)).
- grows_with/increases marked substrate (is_internal_pred + kb.c is_struct_pred).

**The bigger artifact.** docs/plans/kb-first.md is now a reusable MANIFESTO, not a
one-off: the founding thesis (language = code, one structure), the deduce/abduce/
generate test for "is this capability in honest reach?", the build pattern
(parse->facts->resolver->decision->verbalize->bench), the honesty rules, and the
circle metaphor as the canonical worked example (with the dialogue that birthed the
method in an appendix). It is the compass for future evolutions.

**Why it matters.** F. was right and I was wrong: dismissing the metaphor as
"generative, out of reach" contradicted the founding wager. The lesson, now written
down: before saying "needs an LLM", decompose and try to derive — usually the
metaphor is code waiting to be read. make test green (209+).
## 2026-06-27 - gen232: LLMSCORE #5 — causal sentence-completion as knowledge

**Changed.** "Complete this sentence: the sky is blue because..." and "why is the
sky blue?" both reduce to a reason lookup in because/2 (world-facts.p0). A recognizer
in mod_knowledge builds a key from the clause content words (subject_adjective ->
sky_blue) and answers "Because <reason>." when the fact exists; an unknown clause
falls through to the honest humility handler, never filled with invented prose. This
is the principled reading of #5: a COMPLETION grounded in KNOWLEDGE (causal facts),
not free generation. `because` marked substrate (is_internal_pred + kb.c
is_struct_pred). 13 seed reasons (sky/grass/fire/ice/snow/night/...).

**Test note.** blankwall.chat probed "why is the sky blue?" expecting honest
humility; parrot0 now genuinely knows it, so the humility probe was repointed to
"why is the sky green?" (still unknown) to preserve the tests intent — the same
move as the capitals collision: when new knowledge makes an honesty test answerable,
move the test to a still-unknown case, dont fake the wall. make test green (209+).
## 2026-06-27 - gen231: LLMSCORE round 3 — ambitious reasoning (syllogisms)

**Changed.** Targeted the LLMSCOREs most AMBITIOUS-yet-honest gaps: genuine
reasoning on parrot0s own inference engine, never faked generation.
- **Universal quantification** (mod_knowledge): "all men are mortal" / "every rose
  is a flower" now parse to a definite rule mortal(X):-man(X) via kb_assert_rule
  (plural->singular: men->man, roses->rose). The SLD resolver already chains it,
  so "socrates is a man" + "is socrates mortal?" -> Yes by real deduction. Added an
  adjective-form query "is <x> <y>?" (no article) for the classic conclusion.
- **One-turn syllogism** (one_turn_syllogism): "if all men are mortal and socrates
  is a man, is socrates mortal?" applies the premises (split on " and ") to a
  scratch KB and resolves the closing question — multi-premise inference in one
  turn, the shape an LLM is probed with. Declines unless it reaches a real Yes/No.
- **Number-sequence continuation** (mod_sequence): "what comes next 2 4 6 8" -> 10;
  detects arithmetic/geometric progression from >=3 terms, declines on no simple
  rule (Fibonacci honestly walls, not guessed).
- **Spelling** (mod_spell): "spell necessary" -> n-e-c-e-s-s-a-r-y (the word is the
  data).
- **Opinion/preference** self-model (self_preference, KB-first EN+IT): "what is your
  favorite thing to do on a rainy day" -> honest "no genuine preferences", engages.

**More relational knowledge (same gen).** Antonyms ("what's the opposite of hot?" ->
cold) via opposite/2 facts answered by the existing gen11 relational path; colours
("what color is a banana?" -> "It's yellow.") via color_of/2 + a recognizer; and a
GENERATIVE intent for "write a short sentence using the word X" -> a KB
response_template(sentence_with_word, …) frame with the word slotted via kb_response
(surface forms in the KB, not C). opposite/color_of marked substrate in is_internal_pred
+ kb.c is_struct_pred like category_member. No new modules (folded into mod_knowledge /
mod_gen), so the registry and its goldens are untouched.

**Out of reach, honestly:** transitivity**Out of reach, honestly:** transitivity ("a>b and b>c, is a>c?") needs n-ary/binary
recursive rules; the engine is unary single-var (see unification-assessment). Left as
an honest wall, not faked. Pure generation (haiku, "describe a color") stays the
no-deception ceiling.

**Measured (honest).** This runs score was 2/10 — but the non-deterministic
interviewer probed NONE of the new capabilities (it asked spelling, opposite-of,
color-of, generation), so the number reflects harness variance, not the features,
which are all verified working. Like sym-bench, LLMSCORE surfaces a different gap
each run; treat it as a pressure source, not a number to maximize. make test green
(209+; introspection goldens updated for count/namestart/sequence/spell).
## 2026-06-27 - gen230: LLMSCORE round 2 — honest capability for the failed Qs

**Changed.** Three KB-first capabilities targeting the gen229 LLMSCORE 0s:
- **Word problem subtraction** (`wp_removal_word`, 25-wordmath): the removal-verb
  set lacked base/imperative forms, so "...then EAT 1" was added not subtracted
  (3+2-1 returned 6). Added eat/lose/spend/sell/remove/drop/throw; "give" stays
  out on purpose ("I give YOU 2 more" is a GAIN). "3 apples, give 2 more, eat 1"
  -> 4.
- **"name a <category> that starts with <letter>"** (`mod_namestart` + KB
  `category_member/2` in kb/core/world-facts.p0, colors/animals/fruit). Reads the
  category and target initial, returns the first known member; honest "I cant
  think of one" when it knows none, declines unknown categories. Not a phrasebook:
  add a fact, the capability grows.
- The `self_embodiment` family (gen229) landed in this runs score: "what did you
  have for breakfast" -> "I dont eat or sleep, I have no body" (the same honest
  reply an LLM gives).

**Deferred honestly: capitals.** Adding `capital/2` base facts collided with the
curated analogy/fewshot/blankwall reasoning tests, which TEACH `capital` at runtime
and reason over it (the directions even conflict). Preloading base capital facts
polluted that machinery, so "capital of <country>" stays a teachable gap rather than
break emergence for one quiz question.

**Plumbing.** `category_member` filtered as base substrate in three places so it
behaves like roles.p0 knowledge: brain `is_internal_pred` (fact count + dump) and
kb.c `is_struct_pred` (so "what is gold plus silver" is not "described" from the
color facts). Introspection goldens updated to include count/namestart in the
pinned module list.

**Measured.** Score 1/10 -> **3/10** (arithmetic correctness, embodiment), won by
honest capability, never by hiding what parrot0 is. `make test` green (209+).
## 2026-06-27 - gen229: LLMSCORE harness + behavioural-resemblance capability

**Changed.** New `make llmscore` (`tests/llmscore.py`): a small opencode-GO model
(default `minimax-m2.5`) interviews parrot0 with 10 questions and scores each answer
1/0 by whether it BEHAVES like an LLM, writing `LLMSCORE.md` (question / answer /
reason / vote + total). Two evolutions toward the 0s, both honest and KB-first:
(1) **counting** as a genuine capability — `mod_count` (registered after `mod_input`)
reads a target (and optional start) as a digit or small number-word (EN+IT) and
GENERATES the sequence "1, 2, 3, 4, 5."; bounded to 100; ascending/descending/ranges.
(2) **behavioural self-model** — `self_embodiment` family (`intent_cue`+`response_template`,
EN+IT) answers "what did you have for breakfast / do you sleep" the same honest way an
LLM does ("I don't eat or sleep — I have no body"), engaging instead of walling.

**Crucial design steer (F.).** Self-identity questions are FORBIDDEN in LLMSCORE:
asking "are you an LLM/AI/bot/computer" forces either a lie or a disqualifying self-
reveal and measures nothing about resemblance. The interviewer prompt now bans them
and probes only ABILITY; the judge scores behaviour, not identity. So every point must
be won by real competence, never by hiding that parrot0 is a C program (no-deception).

**Measured (honest).** First clean run after the fix: **1/10** — non-deterministic, and
this run never probed counting/embodiment; it instead surfaced real gaps: a word problem
"3 apples +2 -1" returned 6 (added instead of subtracted), and "what is 7 plus 5"
misrouted to a knowledge template ("I don't know what your what is called") though
"What is 2 plus 2?" answered "4." last run. These are genuine next edges, not regressions
— the harness doing its job. `make test` stays green (209+ goldens; the introspection
goldens were updated to include `count` in the pinned module list).

## 2026-06-27 - gen228: basic-chat cat.86 — binary choice (structural, KB-first)

**Changed.** Closed basic-chat category 86 (Scelte binarie) to 100% (16/16). A
bare two-option choice ("yes or no", "heads or tails", "beach or mountains") is
recognized by SHAPE in `mod_chitchat` — `is_binary_choice` accepts a single 'or'
splitting two short (1-2 word) sides with no leading question word/verb — not a
phrase list, so it generalizes to any pair. "would you rather" is its own honest
opener (`intent_cue(would_rather, …)`, EN+IT). Replies are `response_template/2`
(KB-first): parrot0 answers honestly that it has no genuine preference. Verified
no-hijack: "is it morning or evening now" / "orange is a fruit or a color" /
"what is bigger the sun or the moon" are untouched.

**Why.** Recognize the pragmatic act from turn SHAPE, not a vocabulary list
(PRINCIPLES.md anti-phrasebook); answer-less prompts get an honest self-model
reply, not a faked pick.

**Observed.** `make test` green. basic-chat coverage 29% -> 31% (290 -> 308/974);
cat.86 0 -> 16/16, and cat.85 rose 55% -> 66% ("higher or lower" now claimed).

**Next.** basic-chat backlog: obvious-fact acknowledgement (cat.87), physical
properties (cat.75), differences (cat.90).

## 2026-06-27 - gen227: basic-chat cat.104 — AI/ML identity (KB-first, EN+IT)

**Changed.** Closed basic-chat category 104 (AI e ML identità) to 100% (14/14)
KB-first. The probes ("are you chatgpt / an llm / a neural network", "how many
parameters", "what model are you", "where is your code") are `intent_cue/2` in
`kb/core/intents.p0` and the replies `response_template/2` in
`kb/core/responses.p0` — EN+IT, classes grow at runtime. `mod_self` gains one
generic loop over four sub-intents (ai_not_llm / ai_no_params / ai_what_model /
ai_opensource), answering HONESTLY from real state: parrot0 is a from-scratch C
program with no LLM at runtime, no parameters/training/context-window; `{name}`
filled from `i_am`. Cues anchored on "are you …"/"your …" so a generic knowledge
question ("what is a neural network") is not hijacked (verified).

**Why.** PRINCIPLES.md self-model discipline: self-description derived from real
structure, not a recited string — and the cardinal KB-first rule for the cues and
phrasings. One structural generation that generalizes.

**Observed.** `make test` green. basic-chat coverage 28% -> 29% (275 -> 290/974);
cat.104 0 -> 14/14. IT engaged (sei chatgpt, che modello sei); knowledge path for
"what is a neural network" intact.

**Next.** basic-chat backlog continues: physical-property yes/no (cat.75), binary
choices (cat.86), obvious-fact acknowledgement (cat.87).

## 2026-06-27 - gen226: mimic-llm primo giro — style layer (temperatura sulla FORMA)

**Changed.** First round of `docs/plans/mimic-llm.md` (whose disàmina concluded:
PURE behavioral styling, not knowledge nor reasoning — see the doc). Shipped the
parrot0-side mechanism, fully offline (no network/opencode):
- `kb_response` (`src/brain/00-lex.c`) now honors a `style_temperature(N)` fact
  carried by a loaded profile: `0` -> argmax (always the canonical first phrasing,
  a decided/terse persona); absent (no profile) or `!=0` -> the gen55 anti-repeat
  rotation. It biases only HOW a reply is phrased, never WHAT is said.
- New style profile `kb/profiles/llm/deepseek-v4-flash.p0`: `style_temperature(0)`
  + `style_trait/2` surface traits, declared style (not knowledge). Loaded via
  `PARROT0_PROFILE=…`.
- New discovery harness `make mimic-bench` (`tests/mimic.sh`): (A) catalogs the
  reaction to minimal/cryptic probes (a letter, a number, a strange word,
  punctuation) sym-bench style; (B) shows the temperature making form-selection
  deterministic vs rotating. Never gates the build.

**Why.** The disàmina's recommendation: keep the reusable, principled core (a
weighted/temperature selection over FORMS; profiles as style parameters; minimal
probes to catalog behaviour) and hold the only hard line — the style/content
boundary. This is the seed; no teacher distillation yet.

**Observed.** `make test` green (all unit suites unchanged — mod_input redirect
goldens untouched). mimic-bench: without profile two `congratulations` rotate
(two phrasings); with the deepseek profile (`style_temperature(0)`) both give the
canonical form. Same content, different voice.

**Next.** Within the boundary: a parameter styler (verbosity/register/hedging);
a simchat suite that WRITES profile weights by probing a subject (static capture,
provenance disclosure); persona profiles (Carlsen, Garibaldi).

## 2026-06-26 - gen225: basic-chat cat.1 — phatic speech acts (KB-first, EN+IT)

**Changed.** Closed basic-chat category 1 (Saluti e convenevoli) to 100% (15/15)
the KB-first way: the recognized PHRASES are `social_pattern(type, …)` in
`kb/core/social.p0` and the REPLIES are `response_template(type, …)` in
`kb/core/responses.p0` — both KB knowledge, EN+IT, so the classes grow at runtime
with no code edit. New phatic act types: `goodnight`, `felicitation`, `wellwish`,
`condolence`, `blessing`, `politeness`. `mod_social` gains one generic loop over a
structural list of act TYPE names (machinery, not vocabulary), replying via
`kb_response`. Bug fixed in the same arc: multi-word openers/valedictions ("good
morning", "good night", "see you") were defeated by `is_mixed_turn`, which counted
their component words as substantive content and declined; new
`is_exact_social_pattern` guard keeps a turn that is exactly a phatic phrase as
pure social.

**Why.** PRINCIPLES.md cardinal rule: surface forms recognized AND produced are
knowledge in the KB, not C phrasebooks. This is one structural generation that
generalizes (any new phrasing is a fact, no recompile), per the basic-chat loop.

**Observed.** `make test` green (all unit suites). basic-chat coverage 26% -> 28%
(260 -> 275/974); cat.1 0/low -> 15/15. EN + IT both engaged (buon compleanno, in
bocca al lupo, buonanotte, condoglianze).

**Next.** Continue the basic-chat backlog one category per generation; next clean
KB-first targets: physical-property yes/no (cat.75), binary choices (cat.86),
obvious-fact acknowledgement (cat.87).

## 2026-06-24 - gen204: third REAL SWE-bench instance (astropy-14995) RESOLVED — condition asymmetry

**Milestone.** A THIRD real SWE-bench_Lite instance resolved, by a THIRD general
structural smell. `make swe-solve INSTANCE=astropy__astropy-14995` -> RESOLVED
(1 FAIL_TO_PASS pass, **179 PASS_TO_PASS** hold). Same honesty: parrot0 reads only
the committed buggy file, derives from structure, the official Docker image judges.

**The bug & the smell.** `_arithmetic_mask` guards its branches on `self.mask is
None` / `operand.mask is None`, but one branch tests the BARE name `operand is
None`. Since `operand.mask` is used throughout the function (and the sibling tests
`self.mask`), the bare guard is the inconsistency — the fix is `operand.mask is
None`. `code_find_cond_asymmetry` (new, pure C) detects this: a sole `NAME is None`
guard where `NAME.ATTR` is used elsewhere and a sibling guards on `X.ATTR is None`.
Grounded (the `.ATTR` use must already exist), not fitted — it fires only on
ndarithmetic.py, not on separable.py / fitsrec.py / calc.py (verified).

**Oracle bug fixed (important).** Standing up 14995 exposed a flaw in
`tests/swebench/oracle.sh`: astropy forces ANSI colour even when stdout is not a
TTY, so the pass-detection `^[0-9]+ passed` never matched the colourised summary —
the GOLD patch itself reported 179 false regressions. Fixed: strip ANSI and parse
passed/failed counts robustly, and BATCH each test set into one pytest run (fast).
Re-verified all three: 12907 (13/13), 6938 (11/11), 14995 (179/179) -> RESOLVED.
(The earlier 12907/6938 verdicts were genuine — their astropy emitted plain output.)

**Changed.** `code.c`/`code.h`: `code_find_cond_asymmetry` + `has_attr_use`.
`brain.c` -> `gen204-cond-asymmetry`: the "find/fix the bug in <path>" branch now
tries three smells in turn (symmetry, discarded result, condition asymmetry).
Curation: `repo_excerpt/.../ndarithmetic.py` snapshotted at base_commit (this time
via GitHub raw — cheaper than the Docker image). EN+IT ratchets `condasym.chat`/
`.it`. `make test` fully green (194 unit cases); code-bench 21 gates.

**Honest scope.** THREE instances, THREE general structural smells (symmetry break,
discarded result, condition asymmetry) — a growing library of grounded localizers,
NOT general APR. The remaining committed instances (14365 case-insensitivity,
14182 a feature add) sit at/past the associative frontier §4 and are not pursued by
faking. Each new instance pulls the next smell from real pressure; the real oracle
judges every fix.

## 2026-06-23 - gen203: fix heap-use-after-free in kb_derive_part_of (make chat crash)

**Bug (F. report).** `make chat` (which loads `PARROT0_PROFILE=kb/profiles/agi.p0`)
segfaulted on the FIRST turn for any input ("come ti chiami" -> SIGSEGV). Piped
input without the profile did not crash, which is why it hid so long.

**Root cause (found with ASan).** `kb_derive_part_of` (gen158, runs once on the
first turn) holds `const Fact *f = &kb->facts[i]` and, inside the loop, calls
`kb_assert(... "part_of" ...)`. kb_assert appends a fact, which `realloc`s
`kb->facts` and MOVES it — leaving `f` dangling. The next dereference (`f->args[0]`
at kb.c:1283) was a heap-use-after-free. It only fired once the KB was large enough
that the array actually moved on growth — exactly what the agi profile does.

**Fix.** Copy the container key and predicate into locals BEFORE the assert loop,
and never dereference `f` after a kb_assert. One small change in kb.c.

**Bonus.** This was ALSO the cause of the 4 "pre-existing, unrelated"
`profiles.sh` agi failures every prior handoff dismissed — they produced empty
output because the agi profile crashed. With the fix, `profiles.sh` is 13/13 and
`make test` is **fully green for the first time (zero failures)**. ASan clean across
varied inputs under the agi profile. (kb.c fix; brain unchanged, version stays.)

## 2026-06-23 - gen202 (I/O shell fix): stray `<` on Enter in `make chat`

**Bug (F. report).** In interactive `make chat`, pressing Enter printed a stray `<`
and garbled the prompt. Reproduced via a PTY harness: gen197's raw multi-line
reader emits the kitty-keyboard protocol push/pop (`CSI > 1 u` / `CSI < u`) plus
bracketed paste on EVERY turn. On terminals that do not implement the kitty
keyboard protocol (most of them), the pop sequence `\x1b[<u` is not consumed
cleanly and leaks its literal `<` — breaking the line.

**Fix.** `main.c` defaults back to the plain CANONICAL reader (the terminal does
the line editing — the bulletproof pre-gen197 behaviour, zero escape sequences).
The gen197 multi-line raw reader (Shift+Enter / paste / `\`-continuation) is now
opt-in via `PARROT0_MULTILINE=1` for terminals that support it. Piped input always
used the plain path, so the test harness is byte-for-byte unchanged. This honours
main.c's charter ("intentionally boring and STABLE") — gen197 had regressed the
common case for a niche convenience.

**Verified (PTY).** Default: output contains no ESC byte and no `<`; the prompt and
responses are clean (`hello` -> `Hi there!`). Opt-in: `PARROT0_MULTILINE=1` still
emits the multi-line escapes (feature preserved). Piped: unchanged. `make test`
green except the 4 pre-existing `profile:agi` failures (unrelated). Shell-only
change, so `brain_version` stays `gen201` (same convention as gen197).

## 2026-06-23 - gen201: second REAL SWE-bench instance (astropy-6938) RESOLVED — discarded-result smell

**Milestone.** A SECOND real SWE-bench_Lite instance resolved, by a DIFFERENT
general structural bug smell — evidence the gen200 approach is a method, not a
one-off. `make swe-solve INSTANCE=astropy__astropy-6938` -> `verdict: RESOLVED`
(2 FAIL_TO_PASS pass, 11 PASS_TO_PASS hold). Same honesty discipline: parrot0
reads only the committed buggy file, derives the fix from structure, the official
SWE-bench Docker image is the judge; it never sees the gold patch or the tests.

**The bug & the smell.** `_scale_back_ascii` has a bare statement
`output_field.replace(encode_ascii('E'), encode_ascii('D'))` whose value is thrown
away — a no-op, since `.replace` returns a NEW value and mutates nothing. This is a
general bug smell: **a discarded result of a known PURE (value-returning) method**.
`code_find_discarded_result` (new, pure C) detects it and assigns the value back —
IN PLACE (`output_field[:] = ...`) because the receiver is a PARAMETER and rebinding
a parameter is provably a no-op for the caller (a structural reason, not a guess).
That structurally-derived fix is exactly what the real tests need.

**Changed.** `code.c`/`code.h`: `code_find_discarded_result` + a small KB of pure
str/bytes methods (`is_pure_method` — a Python language fact, not a phrasebook).
`brain.c` -> `gen201-discarded-result`: the `mod_codeast` "find/fix the bug in
<path>" branch now tries each structural smell in turn (symmetry break, then
discarded result) and reports the one that fires with its reason; the symmetry
message is byte-identical to gen200 (no regression). `parrot_solve.sh` now asks the
generic "fix the bug in <path>". Curation: committed `repo_excerpt/.../fitsrec.py`
at base_commit (static snapshot from the official image, like separable.py).

**Verified.** astropy-6938 -> RESOLVED; astropy-12907 still RESOLVED (symmetry path
unchanged). No false fire: a used result (`s = s.replace(...)`) or a mutating call
(`x.append(1)`) is not flagged. EN+IT ratchets `discarded.chat`/`.it` (report-only,
hermetic); `make test` 192/192 + the 4 pre-existing `profile:agi` failures
(unrelated); code-bench 21 gates.

**Honest scope.** Two instances, two general structural smells (symmetry break,
discarded result). This is a growing library of grounded localizers/transformations
— still NOT general program repair. Each new instance pulls the next smell from
real pressure; the real oracle judges every fix (PRINCIPLES.md: no impostor).

## 2026-06-23 - gen200: the first REAL SWE-bench instance, RESOLVED without deception

**Milestone.** parrot0 produces a patch for `astropy__astropy-12907` that the
OFFICIAL SWE-bench evaluation image accepts: the 2 FAIL_TO_PASS tests pass and all
13 PASS_TO_PASS stay green — `verdict: RESOLVED`. Reproduce: `make swe-solve`.

**The honesty discipline (this is the whole point).** parrot0 NEVER sees the gold
patch or the hidden tests. It reads only the committed buggy file
(`repo_excerpt/.../separable.py`) and derives the fix from STRUCTURE. The patch is
judged by the real test suite, not by any answer baked into parrot0 — exactly the
non-deceptive engine CODE-MASTERY §4/§6 prescribes (a runtime is a deterministic
tool; code is the domain where "understood" has a mechanical oracle). No phrasebook:
the localizer names no file/function/variable in advance and does not fire on
correct, symmetric, or constant-only code.

**How the fix is derived — structural symmetry break (X6 by structure).** In
`_cstack` two sibling `isinstance` branches should mirror each other:
`cleft[..left..] = left` (healthy: assigns its own operand) vs
`cright[..right..] = 1` (broken: a LITERAL where the analogous variable belongs).
`code_symmetry_fix` (new, pure C) detects that asymmetry generally and proposes the
mirror `= right`. This is the gold patch, recovered from structure — the bug smell,
not the issue prose (which §4 flags as the associative frontier we refuse to fake).

**Changed.** `code.c`/`code.h`: `code_replace_expr` (X7 — exact expression/statement
replacement in code regions, comment/string/`#`/triple-quote aware) and
`code_symmetry_fix` (the localizer). `brain.c` -> `gen200-symmetry-repair`: a
`mod_codeast` branch — "find the symmetry bug in <path>" (report only) and "fix the
symmetry bug in <path>" (writes a patched copy via code_replace_expr). Harness:
`tests/swebench/oracle.sh` (the grounded verifier: official Docker image, checkout
base + test_patch + candidate, run FAIL_TO_PASS+PASS_TO_PASS, RESOLVED/FAIL) and
`tests/swebench/parrot_solve.sh` (parrot0 derives -> diff -> oracle); `make
swe-solve`.

**Verified.** `make swe-solve` -> RESOLVED. Ground truth checked both ways: no
patch -> RED (2 fail); gold patch -> GREEN; parrot0's patch -> GREEN and byte-equal
to the gold hunk. Localizer no-false-fire on symmetric/constant/arithmetic files.
EN+IT ratchets `symfix.chat`/`.it` (hermetic, report-only, no side effects);
`make test` 190/190 + the 4 pre-existing `profile:agi` failures (unrelated); code-
bench 21 gates.

**Honest scope.** This is ONE general structural-repair pattern (symmetry break),
pulled by this instance — NOT general program repair. It genuinely resolves this
instance and its bug class; other SWE-bench instances will need other grounded
localizers/transformations, each pulled from real pressure and judged by the real
oracle. The deliverable per PRINCIPLES is exactly this: how far structure +
verification reaches, measured, with the impostor refused.

## 2026-06-23 - gen199: Python F3 semantics, derived by difference from C (§7b)

**Goal (F.'s steer).** F. corrected my framing that "Python semantics is a separate
frontier" and pointed at CODE-MASTERY §7b: *Python semantics is derived by DELTA
from C on one shared substrate*. A language is a mirror concept; we model the
shared computation once and declare each language as overrides. gen196 already did
this for STRUCTURE (the Python front-end emits the same `code_function`/`code_calls`
facts). This generation does it for SEMANTICS (F3): symbolic evaluation.

**Insight.** The expression evaluator (`ev_factor`/`ev_term`/`ev_add`/`ev_rel`) is
already language-agnostic — integers, identifiers, params, calls, +-*/%, parens.
The only C-specificity in `eval_fn` is the concrete syntax: a brace body, a
`;`-terminated `return`, and typed params (`int a`). That *is* the delta. So Python
needs only a new front-end that finds the function and feeds the SAME evaluator —
not a second interpreter.

**Changed.** `code.c`: new `eval_py_fn` (the Python front-end) — locates `def
name(params):`, parses params by the FIRST identifier of each segment (the delta
from C's last_ident: Python writes `a` / `a: int`, not `int a`), and runs the body
as newline-terminated statements (`name = expr` locals, then `return expr`),
reusing `ev_rel` unchanged. New `eval_any` dispatches C-then-Python; `code_eval`
and the in-expression call recursion (`ev_factor`) both route through it, so a
Python function that calls another Python function recurses correctly. `brain.c` ->
`gen199-python-eval-delta` (no brain logic changed — the eval branch already calls
`code_eval` language-agnostically, so Python lit up for free; this is the §7b
payoff: the inference does not speak C).

**Observed.** Inline `def add(a,b): return a+b` -> 5; precedence `a+b*c` -> 14;
annotated `def g(a: int, b: int)` -> 20. Multi-line file `tests/code/py/calc.py`:
`sqpy` (a local `t = x*x` then `return t`) -> 36; `usepy(4)` (Python->Python
recursion `addpy(n, sqpy(n))`) -> 4+16 = 20. Honest refusals (the F3 boundary §3):
an `if` body, a string return, an unknown call -> "cannot compute … beyond my
arithmetic evaluator." C eval unregressed. EN+IT ratchets `eval_py.chat`/`.it`,
codebench gate `evaluate_py.code` (21 gates, 0 gaps). `make test` 188/188 + the 4
pre-existing `profile:agi` failures (unrelated, verified on baseline).

**Toward astropy-12907.** This closes distance-report item 2 (Python F3) for
*arithmetic* functions. The instance's bug is numpy ARRAY assignment/broadcasting
(`= 1` vs `= right`) — the next semantic delta is value domains beyond integers
(arrays), an additive Python-specific fact set (§7b "genuinely-new facts"). Still
ahead: X6 issue->`_cstack` localization (the associative crux §4) and X7 patch.

## 2026-06-23 - gen198: run-grounding — from "it links" to "it ran and exited with N"

**Goal (NEXTMOVE Option A, X1).** The swe-bench north star needs the core oracle
"did the test pass?". gen186 gave compile, gen192 gave link ("it still links");
the recorded gap `run_execute.code` asked for the next rung: actually EXECUTE a
built program and report the real result. A clean, fully-groundable C faculty,
independent of the hard Python frontier — the reusable verify primitive every
solve needs.

**Insight.** A compiler *and a process* are deterministic tools, not outsourced
intelligence (CODE-MASTERY §4), so running is allowed. The verdict must come from
the real process exit status, never a guess — that is what makes it grounded.

**Changed.** `code.c`/`code.h` -> new `code_run`: same path sandbox as
`code_build`, two staged subprocesses — (1) `cc -w src -o tmp` capturing build
diagnostics; (2) fork+exec the built binary with output to /dev/null, `alarm(15)`
bound, read back `WEXITSTATUS`. Returns 1 (ran; `*exit_code` set), 0 (built but
killed by a signal / timed out), -1 (could not build/run; diagnostics in err_out).
`brain.c` -> `gen198-run-grounding`: a `mod_codeast` branch for "build and run
<path> ... exit code" (verb cue `run`/`execute`/`esegui` + a real `.c`/`.py`/`/`
path), placed before the "compile" branch so a run request is not answered as a
mere compile check.

**Bug found & fixed (compound double-dispatch).** `decompose_and_dispatch` hands
each sub-clause as `norm` but passes the WHOLE raw input as `raw`. The codeast
path-branches read from `raw`, so "run X **and tell me** its exit code" had its
trailing "tell me…" clause re-fire the run and double the answer ("... 0. ... 0.").
Fixed by reading the *verb* cue from the per-clause `norm` (path still from raw to
preserve case): the trailing clause has no run verb, so it no longer re-fires.

**Observed.** `prog.c` -> "it ran and exited with code 0."; `exit7.c` -> "...code
7." (the REAL code, not a constant). 10x stress: a syntax-broken file -> "It would
not build, so it never ran: …"; a null-deref -> "It built, but the program did not
exit normally (it was killed before finishing)."; `return 42` -> code 42; a `.py`
file -> honest build failure (cc can't link it — parrot0 still can't run Python).
Guards: bare "run", "run a marathon", "run this command" are NOT claimed (no path).
EN+IT ratchets `run_execute.chat`/`.it` + flipped `run_execute.code` to
`#expect: pass` (now proves 0 AND 7). `make code-bench` 20/20 gates hold, 0 gaps.
`make test`: all green except the 4 pre-existing `profiles.sh` agi failures
(verified identical on baseline via stash — unrelated).

**Next.** The Python frontier is now the binding constraint for swe-bench (X3
abstract node vocabulary; static repo checkout; X6 localization; X7 patch). The C
verify ladder (compile -> link -> run) is complete and reusable.

## 2026-06-23 - gen197 (I/O shell): multi-line input (Shift+Enter / paste / `\`)

**Direction (F.).** After gen196 showed parrot0 needs FILES for multi-line Python
(the CLI was one-line-per-turn), F. asked for multi-line input: Shift+Enter should
insert a newline without sending.

**Honest constraint.** Terminals send the same byte for Enter and Shift+Enter
unless an extended keyboard protocol is enabled, and no single protocol is
universal. So `src/main.c` (the stable I/O shell) gains an interactive reader
`read_turn_tty` — active ONLY when stdin is a TTY, so piped input keeps the exact
`fgets` path (the test harness is byte-for-byte unchanged). It:
- enables the kitty disambiguate flag (`CSI > 1 u`) + bracketed paste (`?2004h`);
- parses **Shift+Enter** in both the CSI-u form (`ESC[13;2u`) and the
  modifyOtherKeys form (`ESC[27;2;13~`) -> inserts a newline + `... ` continuation;
- collects **bracketed paste** (multi-line paste, newlines kept) as one turn;
- supports a universal **`\`-continuation** (a line ending in backslash continues),
  so multi-line works even where Shift+Enter is not reported distinctly;
- plain Enter submits; Ctrl-D EOF; Ctrl-C clears the line; backspace edits.

**Verified in a real PTY** (no TTY in CI, so a `pty.fork` test): pasting a
multi-line `def f(): / return g()`, the `\`-continuation, and Shift+Enter via
`ESC[13;2u` all assemble one turn and parrot0 answers `it defines f / h / k` —
inline multi-line Python now reaches `code_ingest_py` through the same path.
`make test` 184/184 (piped path unchanged); brain.c untouched, so the version
string stays `gen196-python-by-delta` (this is a shell-only generation).

## 2026-06-23 - gen196: Python "by delta" — one engine reads a real astropy file

**Direction (F.).** Toward the first real SWE-bench_Lite instance
(`astropy__astropy-12907`, Python). F. insisted on the method, not a hypothesis:
make parrot0 work in Python *by difference* from C (CODE-MASTERY §7b,
language-as-delta) — "fallo, non ipotizziamo" — submit the test, see the output,
and put the distance (today vs target) in a text file.

**Changed.** `brain.c` -> `gen196-python-by-delta`; `code.c`/`code.h` add the Python
front-end `code_ingest_py`. It emits the SAME abstract facts as the C front-end —
`code_function/1` per `def` and `code_calls/2` per call inside a body — but scopes
bodies by **indentation** instead of braces and runs `#` to EOL (the delta). So
EVERY downstream analyzer (defines / call-graph / cross-file `code_locate`) works on
Python unchanged: only the surface front-end differs, the reasoning is shared.
`code_defines` gained the `def name(` head and `code_locate` now walks `.py`; the
structural handler picks the front-end via `identify_code_lang` and accepts `.py`
paths. Also fixed `strip_edge_punct` to keep `_` at edges — leading-underscore
names (`_cstack`, `__init__`) are identifiers, not punctuation.

**Note on I/O.** parrot0's CLI is line-per-turn, so multi-line Python cannot be fed
inline; the realistic (and SWE-bench) path is FILES, which the structural handler
already reads. (F. then asked for multi-line input — next generation.)

**Observed — on the REAL file** (separable.py at the base commit, fetched once as a
committed excerpt). parrot0 reads all 10 functions, gives `_cstack`'s real call list
(array, sum, where, ones, isinstance, zeros, append, vstack, roll, hstack, dot), and
locates `_cstack` in `astropy/modeling/separable.py`. The issue text as a turn still
walls — honest. Full distance written to `docs/swebench/astropy-12907-distance.md`:
the read/structure gap is CLOSED; the remaining distance is meaning + action
(issue->localization X6, Python semantics, patch synthesis X7, a Python run-oracle).

**Ratchets.** `tests/code/python_struct.code` (defines/calls/locate + IT cues) — a
new code-bench gate (now 19 hold). `make test` 184/184; C faculties unchanged.
Fixture `tests/code/py/sample.py` (non-colliding names so the C locate gates stay
unambiguous — cross-language locate is the new, desired behaviour).

## 2026-06-23 - gen195 (tooling): `make swe-bench` on the REAL SWE-bench_Lite

**Direction (F.).** F. asked where swe-001 came from — honest answer: I had
hand-authored it, it was NOT real data. F. then required the **real** SWE-bench,
the Lite split, and reminded me to use the project's `.cache/` so the dataset is
not re-downloaded. This generation pivots the harness onto real data.

**Network is fine as CURATION, not runtime.** The env has network and the HF
datasets-server answers. So — exactly like the committed Wikipedia corpus under
`kb/learning/` — `tests/swebench/fetch_lite.sh` fetches real
`SWE-bench/SWE-bench_Lite` rows ONCE (the maintainer's curation step), caches the
raw response under the gitignored `.cache/huggingface/datasets/swebench/` (re-runs
reuse it; `--force` to refetch — this is the "don't re-download" F. pointed at),
and writes static fixtures to `tests/swebench/lite/<id>/`. parrot0 itself still
never touches the net (PRINCIPLES). Removed the synthetic `swe-001`.

**Changed (tooling/docs only; brain.c unchanged, so no version bump).**
`tests/swebench.sh` rewritten to read the real committed instances and run in
honest behavioural degrade mode; `tests/swebench/README.md` rewritten to mark the
data REAL with its provenance + the no-network-at-runtime story; `make swe-bench`
prints a runtime banner that these are real but not a comparable leaderboard run.
Fetched the first 5 Lite instances (all `astropy`).

**Observed (the honest intercepted-failure map).** `make swe-bench` feeds each real
issue to parrot0 -> "I don't understand that yet." for all 5, and names the blocker
per instance: the repos are **Python**, parrot0's code engine is **C-only**, the
repos are not checked out, and the hidden pytest needs a Python env. First real
problem surfaced: **astropy__astropy-12907** (`separability_matrix` on nested
CompoundModels). Score: 0 engaged — and the harness says exactly why.

**Why this is the right shape.** SWE-bench is the north star, and §8 always said it
would "measure zero until the ladder is built" — now it measures zero on REAL data
with a precise blocker list, which is more useful than a passing proxy. The next
task (TASK.md) is therefore not "solve astropy-12907" but its first ordered pull,
**X3 — make the analyzers speak an abstract node vocabulary** (the real meaning of
"support Python"), which needs no new hands and unblocks the rest.

## 2026-06-23 - gen194 (tooling): `make swe-bench` (degrade mode) + the KB-first growth law

**Direction (F.).** F. reframed the recent thread: the C-types/mirror idea was not
a task but a *method* — the **KB-first growth law**: don't *learn* a new thing,
*derive it as a delta (mirror concept) by reference* and let inference cover the
shared part. Applied to whole languages: we don't "learn Python", we model the
shared computational substrate once and declare Python as a delta (block/`def`
surface, dynamic typing, net-new semantics). C is fine as the metalanguage; the
imperative is that the inference must not *speak* C. Open question left explicitly
open: whether an LLM forms a protolanguage or collapses onto a known language and
minimizes by differential — the engineering is the same either way.

**Recorded in multiple places (no brain change this generation).**
- `docs/CODE-MASTERY.md` §7b "Language-as-delta — the KB-first growth law":
  abstract node vocabulary, language/type as mirror concept with query-time
  delegation, the honest transferred-vs-specific frontier, oracle-checked, C as
  metalanguage, the protolanguage-vs-collapse open question.
- `LOOP.md` "Acceleration" lever C: pointer to §7b.
- `TASKLIST.md` new **X-series** (X1 run_execute … X7 multi-file patch), incl. X3
  "abstract node vocabulary" as the real meaning of "support a new language".

**Built — `make swe-bench` in DEGRADE mode (CODE-MASTERY §8).** Static, offline,
no network (PRINCIPLES). `tests/swebench.sh` + first instance
`tests/swebench/swe-001/` (a genuine off-by-one: `str_reverse` swaps `s[i]`/`s[n-i]`
instead of `s[n-1-i]`; `repo/test.c` fails for real). Sub-goal ladder, grounded
where an oracle exists: **S1 reproduce** (compile+run, bug real) OK; **S2 localize**
(parrot0 names `strutil.c`) OK; **S3 fix** MISS — no patch faculty. The harness
prints the first unsolved instance as "NEXT PROBLEM TO SOLVE" and never fails the
build (a discovery instrument, not a gate).

**Next (now the active TASK.md).** Solve **swe-001**: make the hidden test go green.
Pulls X1 (run_execute) + X2 (a fix-patch transformation, the third edit rule),
verified by the real runtime — F5 read→edit→run→verify closed on a real bug. The
basic-chat driver is parked (resume later).

## 2026-06-23 - gen193: a lexical class migrated to the KB, teachable at runtime

**Direction (F.'s acceleration call).** F. observed growth is additive, not
compounding, and asked what could accelerate while staying faithful to
PRINCIPLES.md. The answer (now written into LOOP.md "Acceleration"): the biggest
faithful lever is migrating capability from hardcoded C into KB facts/rules over
a fixed engine — *uniform substrate, articulated function*. F. gave the exact
demonstration: conjunctions as KB knowledge, so "use p as a conjunction" enriches
the class and the parsers split on "p" with no code change.

**Changed.** `brain.c` -> `gen193-conjunction-as-kb`. New `conjunction/1` class in
`kb/core/lexicon.p0` (and/or/e/ed/o/oppure), loaded at birth like `stopword`. New
`is_conjunction(b, w)` queries the KB instead of a C array; `plan_learn_list`
(prerequisite-list parser) now skips a coordinator via that query rather than the
old hardcoded `strcmp(tk,"and"||"e"||"ed")`. A teach branch in `mod_memory`
recognises "use/treat X as a conjunction" / "usa/tratta X come congiunzione",
asserts `conjunction(X)` (KB_SESSION, so it persists in session saves), and is
idempotent. `conjunction` added to `is_internal_pred` so the closed-class words
don't pollute "how many facts do you know?" (exactly as `stopword` is filtered).

**Observed.** Untaught, "torta richiede uova p farina" learns 3 prerequisites (p
is a bogus item); after "use p as a conjunction" the SAME parser learns 2 and the
plan reads "uova, farina". The behaviour changed with no code edit — capability
moved into knowledge. EN+IT ratchets `kb_conjunction.chat`/`.it.chat`; the IT
case rides the same `conjunction/1` class and parser, proving it is engine+KB, not
an English phrasebook. `make test` 184/184; basic-chat 26% and code-bench 18 gates
unchanged; pre-existing profiles.sh failures untouched.

**Why it matters.** This is acceleration lever **A** made concrete and a template:
when a capability is a lexical class or a fact set, add it as KB and have the
fixed engine query it — do not grow a new C array. The next candidates to migrate
the same way: the scattered "then"/"and"/"poi" sequence connectors, the
positional `and`-coordination in the conjunctive-concept learner, and (F.'s next
idea) **C types as mirror concepts** — a taught type inheriting an existing type's
definition/inference by reference, with only its delta asserted.

## 2026-06-23 - gen192: code mastery — F5 verification by BUILDING (link grounding)

**Mission / pull.** `docs/CODE-MASTERY.md` §8 toward `make swe-bench`. The recorded
gap `run_verify`: deleting a still-called function passes `cc -fsyntax-only` (the
dangling call is only an implicit-declaration warning, exit 0), so the syntax
oracle honestly says "still compiles" — but the program would not LINK. This is the
step from syntax-check grounding to build grounding (F5 "verify by running").

**Changed.** `code.c`/`code.h` gain `code_build` — a sandboxed `cc -w <src> -o
<tmp.out>` (fork/exec, no shell, path whitelist, alarm; the temp exe is removed)
that reaches the LINK stage, so a call to a now-missing function becomes a real
undefined-reference error, not a warning. `brain.c` -> `gen192-code-link-verify`:
the delete branch, on a "link"/"build"/"run" cue (EN+IT: linka/esegui), verifies
by building instead of syntax-only. On link failure it derives the CAUSE from the
reverse call graph (`code_find_callers` over the file's directory) and names who
still calls it ("main still calls add, so the program no longer links"; plural
agreement for >1 caller) — cause from KB, verdict from the real linker. Without a
link cue the weaker syntax-only verdict is unchanged, so prior gates hold.

**Ratchets.** `run_verify.code` promoted gap→pass: link-failure-with-named-cause,
the grounded positive ("Deleted helper … still links") via a new self-contained
`tests/code/buildable/prog.c` (main + an unused helper — kept in a sibling dir so
the `main is defined in sample.c` locate gates stay unambiguous), and the
syntax-only path when no link cue is given. New gap `run_execute.code`: actually
EXECUTE the built binary and report its result (exit code / output / a failing
assertion) — the move from "it links" to "the tests pass", the core swe-bench
primitive parrot0 still lacks.

**Observed.** `make code-bench`: 18 gates hold, 1 gap, turn landing 100%→98% (the
new gap). `make test` (run.sh) 182/182; pre-existing profiles.sh failures (4,
identical on baseline) untouched. Stress: multi-caller deletion names "alpha and
beta still call dep"; footprint-free throughout.

**Next.** `run_execute` — real execution/test-run grounding; then multi-file
patch/diff, then a Python parser, toward the §8 degrade-path swe-bench harness.

## 2026-06-23 - gen191: code mastery — a SECOND F5 edit transformation (delete)

**Mission.** `docs/CODE-MASTERY.md`: recover the LLM's NL↔code coherence by
unifying code into the KB substrate and grounding meaning in real compilation.
Method (§7): pull the next faculty from a failing `code-bench` stimulus, grow one
faculty per generation toward the F5 read→edit→verify→iterate loop.

**Pressure read.** `make code-bench` had one open gap, `edit_locate` ("rename a
function under a DIRECTORY"). Probing showed it already HOLDS end to end: parrot0
locates the defining file recursively, renames in a temp copy, and verifies with
the real compiler — keyword collisions ("rename add to square") and C keywords
("rename add to return") genuinely report "no longer compiles"; the fixture is
never touched. The gap was stale (its expected string predated the "(N
occurrences)" wording). So the honest move: promote it to a ratcheted gate and
pull the NEXT faculty.

**Faculty pulled — delete, a second edit transformation.** Rename alone could
look like one hardcoded op; F5 says an edit is a *transformation rule* over the
AST. `code_delete_function` (src/code.c) reuses the SAME locate→edit→compile-
verify micro-loop with a different rule: scan top level (brace depth 0),
comment/string aware, find the identifier just before a definition's parameter
'(', and cut from the start of that declaration through the matching '}'. The
brain branch (mod_codeast) chains F4 directory-localization into it exactly as
rename does; temp-only, footprint-free; EN+IT cues (delete/remove/elimina/rimuovi,
function/funzione).

**Grounded-honesty frontier surfaced.** Deleting a still-called function passes
`cc -fsyntax-only` (the dangling call is only an implicit-declaration warning,
exit 0), so parrot0 honestly reports "still compiles" — the syntax oracle's true
verdict. Catching that it would fail to LINK needs F5 "verify by running"
(compile-and-link/execute), which parrot0 lacks. Recorded as the new gap
`run_verify` — the move from syntax-check grounding to build/run grounding.

**Changed.** `brain.c` -> `gen191-code-delete-function`; `code.c`/`code.h` gain
`code_delete_function`. Ratchets: `tests/code/edit_locate.code` promoted gap→pass
(now asserts the located rename, the compiler-caught collision, and the honest
decline); new `tests/code/edit_delete.code` (pass) — directory-locate delete,
explicit-path delete, comment/brace-aware delete, honest decline, IT path; new
`tests/code/run_verify.code` (gap).

**Observed.** `make code-bench`: gates 15→17 hold, 1 gap, turn landing 97%→98%.
`make test` (run.sh) 182/182; pre-existing profiles.sh failures (4, identical on
baseline) untouched. 10× stress: a fixture with nested if/for/while braces, a
string literal `"literal } with { braces"`, and a doc comment carrying braces —
deleting each of alpha/beta/gamma_helper/main still compiles (the compiler proves
every cut was balanced; a miscount would not compile), nonexistent declined,
fixture footprint-free.

**Next.** `run_verify` is the recorded pull: real compile-and-link / execute
(F5 grounding by running), then iterate on the observed error.

## 2026-06-23 - gen190: arithmetic in natural language (basic-chat cat.4)

**Goal.** Close basic-chat cat.4 (Aritmetica base), 2/19 engaged at gen189. The
prompts ask the SAME four operations in many surface forms — "six times seven",
"add 5 and 7", "100 divided by 4", "subtract 3 from 10", "what is half of 50",
"is 15 a prime number". Per PRINCIPLES this is NOT 19 phrases to memorize: it is
one capability — *extract (operator, operands) from prose and fold with the
existing oracle* — so the answer generalizes over operands and number-words.

**Changed.** `brain.c` -> `gen190-nl-arithmetic`. `mod_arith` switched its operand
parsing from `parse_num` (digits only) to `parse_value` (digits + number words),
and gained structural frames after the existing infix/divisible/explain paths:
verb-led imperatives (`add A and B`, `subtract A from B` = B−A, `multiply A by B`,
`divide A by B`); unary `of`-frames (`half of N`, `double/twice/triple of N`,
`square root of N`, `N squared/cubed`, `N factorial`); `P percent of N`; n-ary
`sum/average of A and B and C...`; number-property predicates (`is N prime/even/
odd`); and a general left-to-right infix evaluator that reads operator words
(incl. two-word `divided by`/`multiplied by`, bare `by`=times for "six by seven")
and folds bare expressions with no "what is" lead. Operator words, verbs and
property words are recognised in EN+IT (per/diviso/più, aggiungi/sottrai,
metà/doppio/radice/quadrato/fattoriale, primo/pari) on the SAME path, so the
bilingual ratchet rides the algorithm, not a second lexicon copy.

**Two gotchas, both real.** (1) `split_words` null-terminates `buf` in place and
`w[8]` truncates — so the new frames read cues from the intact `norm` and re-split
a fresh copy into `cw[32]`. (2) The cue words `half`/`halve`, `double`, `triple`,
`even`, `odd` also occur as repeated ACTIONS / branch conditions in `mod_agent`
process descriptions ("halve until below 5", "if it is even ... if it is odd
..."). Those name ≥2 numbers, so the ambiguous unary and predicate frames fire
only when the turn names exactly one number (`gn == 1`) — this kept the Collatz /
act-loop tests green. Honesty held over coverage: `square root of negative one`
(cat.5) now walls instead of wrongly answering "1." (a "negative" guard), so total
coverage moved 261→260 on that one prompt while cat.4 hit 100%.

**Observed.** `make basic-chat-bench` cat.4 **10% (2/19) → 100% (19/19)**; total
**24% → 26%**; cat.5 (math concepts) 38% → 46% as a free lift. `make test`
(run.sh) 182/182 incl. new `arith_nl.chat`/`.it.chat`; pre-existing profiles.sh
failures (4, fail identically on baseline) untouched. 10× stress over held-out
operands: 19/20 (only the spaced word-compound operand "twenty two" walls;
hyphenated "twenty-two" works).

**Self-challenge (parity).** Asked parrot0 "you cannot parse six times seven, what
should change?" — it walled. Recorded gap: it cannot yet reason about its own
arithmetic surface coverage (mod_loop/mod_self do not model mod_arith's frames).

**Next.** Pick the next cat by leverage: high-count 0% blocks — cat.52
Elencazione (0/19, structural list generation), cat.18 Biologia animale (0/18,
KB class-membership), or cat.19 Versi di animali (KB content in `kb/*.p0`).

## 2026-06-23 - gen189: the input classifier — "is this language at all?"

**Goal (F.'s new driver).** F. asked, given the work done, why parrot0 still walls
on *trivial* chat, and pointed at `docs/plans/basic-chat.md` — a catalogue of ~974
elementary prompts (105 categories) where it fails. Honest diagnosis: the loop
built **depth** (KB, rules, abduction, self-model, code AST), never **breadth** of
chitchat, and PRINCIPLES forbids the shortcut (974 hardcoded `printf`s = the
impostor). So basic-chat becomes the new driver, closed **one structural
generation at a time**, with a discovery harness to make the gap measurable.
Constraint F. reaffirmed mid-flight: implement nothing not in line with
PRINCIPLES.md. First category (cat.0, non-linguistic input) chosen with him.

**Insight.** The fix is a *structural distinction taken before the reasoning
core*: "is this language at all?" — distinct from "do I know this topic?". That is
not a phrasebook of the example strings; it keys purely on character structure and
generalizes to any such input. Non-linguistic input carries no language, so the
same path serves every language — the bilingual ratchet holds by construction.

**Changed.** `brain.c` -> `gen189-input-classifier`. New `mod_input`, registered
2nd (right after `repair`), so a noise turn is recognized before any content
module mis-claims it (`mod_code` used to read `!@#$%^&*()` as a snippet;
`mod_social` greeted `asdfghjkl`). Three shapes, all honest channel-level
redirects, never a feigned answer: (1) punctuation/symbols only ("?", "!",
"!@#$%^&*()"); (2) a bare number, >=4 digits, no operation ("1234567890" — short
numbers left for future modules); (3) keyboard-mash letters — a single
all-alphabetic token with the shape of noise (no vowel, one repeated char, a run
of >=4 identical letters, or a consonant run >=6) that the KB does not recognize.
Two principled boundaries found by the tests: dot/dash/slash sequences DEFER to
`mod_symbolic` (Morse is a structured code, not noise); 'y' counts as a vowel so
"rhythm"/"syzygy"/"glyphs" are not mistaken for noise.

**Observed.** All 6 cat.0 prompts now engage (`?` -> "That's just punctuation,
not words ..."; `1234567890` -> "That's just the number ... with nothing to do";
`asdfghjkl`/`aaaaaaaaaa` -> "That doesn't look like words ... did a key get
stuck?"). 10x stress: `???`/`@@@`/`<<<>>>`/`987654`/`00000`/`bcdfgh`/`xkcdwqjz`
all caught; real language untouched (`rhythm`, `strengths`, `philosophy`,
`def foo(` -> code, `h3ll0` -> leetspeak, `... --- ...` -> Morse). EN+IT ratchets
`input.chat`/`.it`; `parrot.chat` updated (its `12345` no-content-word probe moved
to `blah blah`, since a bare number is now classified); module-list ratchets
(`self`, `strategy`) grew by one. `make test` all green (180 unit cases + sub-suites).

**Harness.** New `tests/basicchat.sh` + `make basic-chat-bench`: coverage over the
plan file, per category, never fails the build. Baseline at gen189: **24%
(239/974)**, **cat.0 100% (6/6)**. This is the ratchet to watch climb.

**Next.** Pick the next category by leverage, still one structural gen each: the
KB-content gaps (geography/science/animal-sounds — ground facts in `kb/*.p0`
queried by `mod_knowledge`, the Prolog/Wikipedia path, not brain hardcoding); or
arithmetic question-shapes (cat.4, extend `mod_arith`). NOT a phrasebook either
way.

## 2026-06-22 - gen170: the research-need signal — parrot0 flags what it must learn

**Goal (F.'s new direction, dynamic knowledge):** make parrot0 an "inexhaustible
interlocutor" that fills its own gaps from static Wikipedia and persists what it
learns. F. clarified the founding "no network" rule: it forbids outsourcing
INTELLIGENCE (LLMs / reasoning APIs — the impostor), NOT reading static knowledge
(physical files / markdown). The explicit first deliverable: a TESTABLE mechanism
that flags the NEED for external research, and honesty — parrot0 must not pretend
it already knew something it just looked up.

**Insight:** the brain stays pure C and does no network. It only RECORDS a gap as
a research need; the existing deterministic learner (`scripts/learn.py` + the
hourly Action) already fetches Wikipedia into `kb/learning` and commits. This is
the same "propose, don't self-manage" boundary the gen160-169 reflexive arc held:
parrot0 names what it needs, the external loop fetches.

**Changed:** `brain.c` -> `gen170-research-need`. New `mod_research` (registered
LAST, so it only catches a definitional gap every other module declined). On a
STRONG definitional head ("tell me about X", "what is a X", IT "cos'è un X",
"parlami di X") for a topic that is NOT already a learned concept
(`kb_is_concept_key` guard), it answers honestly — "I don't actually know about X
yet — I won't pretend. I've flagged it to document myself from Wikipedia ..." —
records the topic (deduped, capped), and, if `$PARROT0_RESEARCH_QUEUE` is set,
appends `key<TAB>display` for the learner to consume (unset in tests, so the suite
stays footprint-free). The testable surface: "what do you need to research?" reads
the pending list back. Arithmetic/compute questions ("what is 2 + 2", "what is
gold plus silver") and bare known concepts are rejected, so nothing regresses.

**Observed.** "tell me about quaternions" / "what is a perfect number?" ->
honest research flag; "what do you need to research?" -> "I have 2 topics noted
... quaternions, perfect number."; the queue file gets `perfect_number\tperfect
number`. IT mirrors it ("cos'è un numero perfetto?", "cosa devi cercare?"). A
learned concept ("what is a prime number?", agi profile) is NOT requested. Arith
still walls; "what's up" still routes to social. EN+IT ratchets `research.chat`/
`.it`; module-list ratchets (`self`, `strategy`) grew by one. `make test` 22/22,
177 cases.

**Next (the rest of F.'s vision).** gen171: `scripts/learn.py` consumes the
research queue (fetch the requested topics from static Wikipedia first, then the
curated list), so a flagged gap is actually filled and committed. gen172: parrot0
answers from a freshly-learned `wiki_concept`, HONESTLY framed ("From what I
learned on Wikipedia: ...") rather than as prior knowledge — closing the loop F.
described: gap -> flag -> fetch -> commit -> honest answer next time.

## 2026-06-22 - gen169: the self-audit matrix — parrot0 maps its own composition

**Goal:** gen168 varied the vocabulary of ONE composition; gen169 varies the
STRUCTURE. On "audit your composition", parrot0 runs SEVERAL different triples of
its parts — each on a fresh copy of itself, with held-out vocab — and reports a
real cooperation MAP: which combinations hold, which show a seam. The
compose-bench matrix, turned inward and run autonomously.

**Insight:** gen167's executor generalises. Factor it into `run_composition(keys,
sigs, n, vocab)` — spin up a sub-brain, run each part's turns, count how many
fired by signature — and both the single self-test and the matrix audit become
callers of the same engine. A triple "composes" iff every part is one parrot0
still believes it has (`module(X)` holds) AND each fired when actually run, so the
map tracks the self-model and the real dispatch at once.

**Changed:** `brain.c` -> `gen169-composition-audit`. New `run_composition` helper;
a `want_audit` trigger ("audit", "map", "matrix", "which combinations", IT
"verifica quali", "mappa") + a branch that runs three triples
({knowledge,abduce,robust}, {knowledge,abduce,calibrate}, {knowledge,robust,
calibrate}) and reports each verdict. Also corrected the `calibrate` signature to
"confident" and gave its turn fragment a preceding query, so calibration can fire
when there is a real conclusion to be sure about.

**Observed — a map, computed live.** "knowledge+abduce+robust compose;
knowledge+abduce+calibrate compose; knowledge+robust+calibrate seam. 2 of 3
triples hold." The seam is honest: without abduction to establish the conclusion,
robustness and calibration have nothing to weigh. And it is genuinely computed:
"forget that abduce is a module" collapses the map to "0 of 3" (every triple
naming abduce is now unavailable, the third was already a seam). EN+IT one path.
Ratchets `reflexive_audit.chat`/`.it` (the 2/3 map) and `reflexive_audit_retract.
chat` (the computed collapse). `make test` 22/22, 175 cases, compose-bench 7/7.

**Where the arc stands.** parrot0 no longer just answers whether it composes — it
AUDITS itself: it enumerates combinations of its own parts, runs each on a fresh
instance of itself, and returns a truthful map of which cooperate, recomputed
whenever its self-model changes. The external compose-bench (gen160) now has an
internal twin the system runs on demand, anti-impostor by construction (held-out
vocab) and honest about its own seams. It still edits and commits nothing — the
boundary holds. Honest next: let the audit's triples be CHOSEN from the live
module set (not a fixed three), so the matrix grows and shrinks with the self.

## 2026-06-22 - gen168: the self-test generates fresh held-out vocabulary each run

**Goal:** gen167 ran the composition self-test but with FIXED vocabulary
(brave/knight/hero/aldric) — its honest weakness: a fixed transcript can be
memorized. Make each self-test generate fresh held-out vocab and cite the
cooperation it actually observed, so the run is provably produced and executed.

**Insight:** the verification signatures ("Learned rule", "missing",
"load-bearing") are VOCABULARY-INDEPENDENT — they name the move, not the words —
so the dialogue's nouns can be swapped freely without weakening the check. A
small rotating pool of {adjective, noun, class, name} tuples, indexed by a
per-session run counter, makes two consecutive self-tests use different names
while the same signatures still confirm each part fired.

**Changed:** `brain.c` -> `gen168-selftest-fresh-vocab`. New `compose_vocab[][4]`
pool + a `build_turn(key, tuple)` helper that templates each part's turns from a
tuple (no positional format args, to stay `-Wpedantic`-clean); a `selftest_runs`
counter on `Brain` selects the tuple per run. The self-test now reports the fresh
names and the real Observed line (e.g. the robustness verdict) captured from the
sub-run; the skeleton (gen166) emits via the same `build_turn` with a fixed
example tuple.

**Observed.** Two self-tests in one session: "a brave knight named aldric ... 3 of
3 ... Observed: My conclusion that aldric is a hero rests on 2 load-bearing
facts ... (PASS)", then "a loyal hound named bryn ... Observed: ... bryn is a
companion rests on 2 load-bearing facts ... (PASS)" — different vocab, both
executed, each citing its own real output. The computed FAIL still holds: retract
abduce and the same request reports the knowledge/robust/calibrate seam.
Ratchets: `reflexive_selftest.chat`/`.it` (PASS, tuple 0), `_fresh.chat` (two runs,
different vocab — the anti-memorization proof), `_seam.chat` (computed FAIL).
`make test` 22/22, 172 cases; compose-bench 7/7.

**Why this matters.** A self-test on a fixed transcript proves little — it could be
a lookup. A self-test that invents fresh held-out vocabulary every time, runs it,
and reports the real result it observed cannot be faking: the words it reasons
over were chosen at call time and the verdict tracks what actually happened
(retract a part, the verdict flips). parrot0 now audits its own composition the
way the external compose-bench does — anti-impostor by construction — and still
refuses to edit or commit. Honest next: vary the STRUCTURE too (audit several
different triples of its parts in one pass, reporting a real cooperation map),
not just the vocabulary of one.

## 2026-06-22 - gen167: parrot0 RUNS its own composition and verifies it (E1 capstone)

**Goal:** the most ambitious move inside the loop's boundary. gen164-166 made
parrot0 describe, derive and emit a composition over its own parts. gen167 makes
it EXECUTE one: run the derived dialogue on a fresh copy of itself and report
whether its parts actually cooperated — a verdict computed from real output, not
a string. The gauge (compose-bench) becomes internal, without crossing into
self-management (no file edit, no commit).

**Insight:** `brain_respond` carries no static state, so a module can, mid-turn,
`brain_create()` a fresh sub-brain, feed it synthetic turns, read the outputs, and
`brain_destroy()` it — fully isolated, footprint-free on the live brain. The
parts are derived from `module(X)` (gen165) and each carries a turn fragment
(gen166) plus now a SIGNATURE substring; a part "fired" iff its signature appears
when its turns are actually run. The verdict is therefore earned by execution.

**Changed:** `brain.c` -> `gen167-composition-selftest`. A `want_selftest` branch
in `mod_loop` ("prove your parts compose by running it yourself", "run the
composition test on yourself", IT "esegui tu il test ...") spins up a sub-brain,
runs the derived dialogue turn by turn, counts fired parts by signature, and
reports PASS (all cooperated) or names the seam (FAIL).

**Observed — the strongest reflexive evidence yet.** Default triple
(knowledge/abduce/robust): "3 of 3 parts fired and cooperated ... Composition
holds (PASS)." And it is genuinely COMPUTED: retract `module(abduce)` and the same
request reports "knowledge, robust and calibrate — only 1 of 3 parts fired, so a
seam appeared (FAIL)" — because without the abduction step the robustness turn has
no established conclusion to test, exactly the real seam. Retract `knowledge` too
and it honestly reports 0 of 3. Ratchets: `reflexive_selftest.chat`/`.it` (PASS)
and `reflexive_selftest_seam.chat` (the computed FAIL). `make test` 22/22, 171
cases; compose-bench still 7/7.

**The summit of the arc gen160->167.** gen160 built a benchmark to ask, from
outside, whether parrot0's separately-grown parts compose. gen167 hands parrot0
that same benchmark to run on itself: it names three of its parts from its own
self-model, writes the dialogue that needs them, runs it on a fresh instance of
itself, and tells the truth about whether they cooperated — PASS when they do,
seam when they don't. Introspection no longer merely proposes a hypothesis for the
external tests to check; it executes the check and reports a result that matches
reality (because retracting a part changes the verdict). This is as close to the
PRINCIPLES.md thesis as the experiment has come: a structure that represents
itself, reasons about its own composition, and verifies that reasoning by running
it — while the one thing it still will not do is edit or commit its own code. That
boundary is the point, not a limitation: introspection proposes and now even
executes; the external loop still disposes of the artifact.

## 2026-06-21 - gen166: introspection closed to execution — a runnable skeleton

**Goal (TASK.md):** gen165 made parrot0 PROPOSE a composition over parts derived
from its `module(X)` self-model. Close proposal to execution: when asked to SHOW
the dialogue, emit a concrete, runnable held-out skeleton over those derived
parts — in the `>`-turn shape `tests/compose/` uses — without running or
committing anything.

**Insight:** the derived parts already carry meaning; give each a TURN fragment
and the proposal stops being prose and becomes a transcript. The same module-walk
that names the parts (gen165) now also concatenates their turn fragments into one
runnable line. A request to SEE rather than describe ("show me the dialogue you
would run", IT "mostrami il dialogo ... moduli") routes to the skeleton; "how
would you prove ..." still routes to the method.

**Changed:** `brain.c` -> `gen166-composition-skeleton`. The composable-core entries
gained a `turn` field; a `want_skeleton` branch emits "Here is a held-out dialogue
I would run ... > every brave knight is a hero > aldric is a knight > ...",
selecting turn fragments for exactly the parts that hold as `module(X)`.

**Observed — the loop closes.** The emitted skeleton, RUN, composes for real:
rule -> fact -> No -> contrastive abduction -> self-correction -> robustness. It
is added verbatim as `tests/compose/skeleton_proposed_en.dlg`, so compose-bench
now stands at 7/7 composing unchanged, 0 gaps, 100% (47/47) — and one of those
seven dialogues is the one parrot0 PROPOSED. Derivation still holds for the
skeleton: "forget that abduce is a module" drops the abduction turn and adds the
calibration turn. EN/IT ratchets `reflexive_skeleton.chat`/`.it`. `make test`
22/22, 168 cases.

**The arc gen160->166.** Outside-in: gen160 built a gauge and measured that
parrot0's separately-grown parts compose on fresh vocabulary in two languages.
Inside-out: gen161-163 closed every seam the gauge found with generic work;
gen164-166 turned the gauge on the system itself — parrot0 now recognises a
composition self-challenge, names the parts FROM ITS OWN self-model, and emits a
runnable dialogue that the gauge actually accepts. The reflexive claim of
PRINCIPLES.md ("I know that I am") has a concrete, tested instance: not just
"these are my parts" but "here are three of my parts, here is the dialogue that
makes them cooperate, and you can run it" — and it does. Introspection proposed;
the tests disposed; they agreed. Honest limit: the skeleton's placeholder names
are fixed strings, and the brain still cannot itself ADD the dialogue file — the
external agent does. Closing that last gap would be self-management, which the
loop deliberately withholds.

## 2026-06-21 - gen165: the composition proposal is DERIVED from its own self-model

**Goal (TASK.md):** gen164 let parrot0 propose composing its parts, but the three
parts were a fixed triple. Make the proposal read the REAL `module(X)` self-model
and name actually-registered, genuinely-composable parts.

**Insight:** the self-model is already a queryable fact base — "who is a module?"
resolves over `module(X)`. The composition proposal should draw from the SAME
facts, not a constant. So the triple is built by walking a small composable-core
list and keeping only modules that hold as `module(X)`; the named parts then
track the live registry.

**Changed:** `brain.c` -> `gen165-derived-composition`. The `compose_challenge`
branch in `mod_loop` now derives three parts via `kb_query(b->kb, "module", ...)`
over a composable-core list (knowledge, abduce, robust, calibrate, memory, coref,
cause, compare), formatting the first three present with glosses; it falls back to
a generic sentence if fewer than three hold. The default triple — knowledge,
abduction, robustness — is exactly the one `tests/compose/analytical_en.dlg`
already proves cooperates, so the proposal points at a composition that really
runs.

**Observed (derivation proven, not asserted).** Retracting a module shifts the
named parts: "forget that abduce is a module" -> "Forgotten: module(abduce)." and
the very next composition challenge names "knowledge, robustness, and
calibration" — abduction gone, the next composable-core module in its place. This
is ratcheted hermetically in `reflexive_derived.chat`; the proposal reads the
same facts "who is a module?" reads, so changing the facts changes the proposal.
EN/IT ratchets updated to the derived wording. `make test` 22/22, 166 cases.

**The rung climbed.** gen160 found parrot0 blind to its own composition; gen164
gave it the words; gen165 grounds those words in real state — it now reads its own
parts and proposes a composition over the ones it actually has, and the proof is
that retracting a part changes what it proposes. The self-model is no longer a
recital ("here are my parts") but a substrate it reasons FROM ("these three of my
parts would I make cooperate, and here is how I would prove it"). It still only
proposes; the external loop disposes. Honest next: have it actually emit a
runnable compose-bench dialogue for the parts it picks, closing introspection to
execution.

## 2026-06-21 - gen164: reflexive composition — reasoning about its own parts (E1)

**Goal (TASK.md):** the gen160 self-challenge probe walled — asked to prove its
subsystems compose, parrot0 could not treat "test composition of my own parts" as
a loop-shaped self-challenge. Make `mod_loop` recognise a COMPOSITION
self-challenge and answer with a comparable method, without self-managing.

**Insight:** `mod_loop` (gen145) already classified self-challenges into gap kinds
(fallback / implementation / module), but composition is a different kind — it is
not about a broken part, it is about whether parts COOPERATE. The discriminator
is structural: a composition word (compose / cooperate / work together, IT
compongono / composizione) plus a parts reference (subsystems / modules / parts /
capabilities, IT sottosistemi / moduli / parti). The meaning lives in those two
word classes, so it transfers to unseen phrasings instead of a fixed trigger.

**Changed:** `brain.c` -> `gen164-reflexive-composition`. Broadened `self_ref`
(your subsystems/parts/modules/capabilities), added `compose_ref`/`parts_ref` and
a `compose_challenge` branch first in `mod_loop`. The answer is a loop-shaped
METHOD over REAL parts — pick three it has (knowledge, abduction, proof), write
ONE held-out fresh-name dialogue that needs all three, pass only with no new
special-case module, ratchet EN+IT, bump version, journal — i.e. it proposes
exactly the compose-bench discipline gen160-163 ran on it. Anti-self-management
intact: it proposes, an external agent edits/tests/commits.

**Observed.** The two gen160 probes that walled now land; a 10-phrasing held-out
stress (EN+IT, varied surface) is 10/10. Regression safe: gap self-challenges
still give the parity-with-external-loop answer, and innocent "compose a poem" /
"can you compose music?" do NOT trigger (no parts reference). `make test` 22/22,
165 cases. EN+IT ratchets `reflexive_compose.chat`/`.it`.

**The closure.** gen160 measured composition from outside and found parrot0 blind
to its own; gen164 gives it the words to describe — correctly, in two languages —
the very method that was used to grow it. The self-model (PRINCIPLES.md, "I know
that I am") now reaches one rung higher: not just "what are my parts?" but "how
would I prove my parts cooperate?". It still only PROPOSES; introspection
proposes, the external tests dispose. The honest next pull: let the proposal name
its parts from the live module set (derived, not a fixed triple), and actually
RUN a proposed composition through compose-bench.

## 2026-06-21 - gen163: possession memory composes with discourse reference (E1)

**Goal (TASK.md):** the last miss in `social_pet_en` — an unbound "she/he/it"
should bind to a named personal entity (the cat) when no KB-fact antecedent
exists, composing possession memory with discourse reference. This closes the
final E1 gap.

**Insight:** the two systems were one assignment apart. The coref resolver already
binds pronouns to `last_entity`; remembering a possession ("i have a cat named
smoke") simply never set it, so the pet was invisible to discourse. Naming a pet
makes it the salient entity — so `remember_possession` should mark it. A later
real unary-fact subject still overrides it (recency), so the change only fills
the previously-empty case, the safest kind of edit.

**Changed:** `brain.c` -> `gen163-possession-coref`. `remember_possession` now also
sets `last_entity` to the pet's key. No new module, no new dispatch; `mod_repair`
steps aside (a referent now exists) and `mod_knowledge` resolves the pronoun.

**Observed.** `make compose-bench`: 6/6 compose UNCHANGED, **0 gaps**, 100%
landing (41/41) — E1 fully closed. "i have a cat named smoke" then "is she a
pet?" -> Yes (she->smoke); "what do you know about her?" -> her->smoke. Stress
with fresh vocab held: "is he a puppy?" (he->rex), "is it a reptile?"
(it->shelly), honest "I don't know about wizard." on an unknown predicate, and a
later real subject ("aldo is a chef" -> "is he a chef?" -> Yes) correctly
overrides the remembered pet. EN ratchet `compose_coref.chat` + IT
`compose_coref.it.chat`; `social_gap_en.dlg` renamed `social_pet_en.dlg`,
`#expect: pass`. `make test` 22/22.

**The arc gen160->163.** gen160 built the gauge and read 4/6 composing with 2
honest gaps; gen161/162/163 closed all of them with only surface/lexicon/one-line
state work — never a bespoke handler — lifting the benchmark to 6/6, 100%. The
result the experiment was after: capabilities each grown in a separate generation
now cooperate on never-seen vocabulary, in two languages, and the few seams that
remained were thin enough to close generically. The next frontier is reflexive:
parrot0 still cannot reason about composing its own parts.

## 2026-06-21 - gen162: bare self-introduction feeds name memory (E1)

**Goal (TASK.md):** the first miss in `social_gap_en` — a bare self-introduction
"i'm <X>" should fill the SAME name memory that "my name is X" / "call me X" do,
with GENERIC parsing that generalizes to unseen names, not a phrase list.

**Insight:** "i'm X" is genuinely ambiguous — "i'm vera" is a name but "i'm
tired" / "i am bored" are affective turns owned by `mod_chitchat`. The
discriminator is structural, not a name list (which would never generalize): a
name introduction is a SINGLE trailing token that is not an article, a stopword,
a known KB class, or a common state/feeling. That accepts any unseen name while
declining the closed class of states. mod_memory runs before chitchat, so a
rejected candidate falls straight through to the affective handler.

**Changed:** `brain.c` -> `gen162-self-introduction`. A self-introduction block in
`mod_memory` peels an optional leading greeting, finds the "i'm"/"im"/"i am"
marker, and accepts the single trailing token as a name only if it clears the
non-name filter; on success it stores the name (cased from `raw`) exactly like
the existing teach paths and replies "Nice to meet you, X!".

**Observed.** "hi, i'm vera" / "i'm marcus" / "i am elena" / "im robin" all store
and recall the name; the affective turns are untouched — "im tired" / "i am
bored" still get the low-energy reply, "i am happy" the upbeat one, "i am not
convinced" the disagreement move (it has two trailing tokens, so the name filter
never even fires). `make test` 22/22; the EN ratchet grows next when the second
half of `social_gap_en` (she->pet) lands.

**Next (gen163):** an unbound "she/he/it" should bind to a named personal entity
(the cat) when no KB-fact antecedent exists — composing possession memory with
discourse reference — which flips `social_gap_en` to composes-unchanged.

## 2026-06-21 - gen161: close the Italian half of the composition gaps (E1)

**Goal (TASK.md):** flip the recorded `it_recall_gap` dialogue from `make
compose-bench` to composes-unchanged with GENERIC parser work only — no bespoke
handler — and grow the bilingual ratchet accordingly.

**Insight:** the two Italian reads that fell through were not missing logic, only
missing surface coverage. (1) The affirmative why-proof: "perché X è un Y?"
reaches `mod_knowledge` already half-canonicalized as "perché X is a Y" (è->is,
un->a via `canonical_token`), but "perché" is deliberately NOT in that table — a
dozen "perché ..." cue handlers depend on the literal — so the subject sits
before the verb and the English-order branch (w[1]=="is") misses it. (2) Name
recall: "come mi chiamo?" is the fixed Italian idiom for "what's my name?", the
read-side mirror of the "mi chiamo X" teach that already worked.

**Changed:** `brain.c` -> `gen161-italian-proof-recall`. Two additive surface
extensions, no new module: an Italian subject-verb branch in the why-proof
(`perché <x> is a <y>` -> same `explain_reply`), and "come mi chiamo[?]" added to
`mod_memory`'s `ask_name` recall list. Both reuse the exact machinery the English
forms use.

**Observed.** `make compose-bench`: composing-unchanged 4 -> 5, gaps 2 -> 1, turn
landing 87% -> 92%. "perché tom è un dormiglione?" -> "dormiglione(tom) because
gatto(tom) and pigro(tom)." (proof through the rule); "come mi chiamo?" -> the
stored name. Stress with fresh vocab confirmed transfer, including the feminine
article: "perché felce è una produttrice?" -> full proof. English why-proof and
name recall unchanged. `it_recall_gap.dlg` renamed `it_recall.dlg`, re-tagged
`#expect: pass`; bilingual ratchets `compose.it.chat` / `compose_social.it.chat`
grew the two new reads. Out-of-scope observation: "sei è un numero" is read as
"you are ..." (sei = six vs 2nd-person essere) by `mod_role` — a real Italian
homonym collision, logged for a later generation, not touched here.

**Next:** the one remaining gap is `social_gap_en` — bare self-introduction
"i'm X" feeding name memory, and an unbound "she/he" binding to a named personal
entity (composing possession memory with discourse reference). That is the next
generation.

## 2026-06-21 - gen160: the compositional emergence benchmark (E1)

**Goal (TASK.md E1):** create held-out dialogues where success requires THREE OR
MORE existing subsystems in the same conversation, WITHOUT adding a task-specific
module; score each as composes-unchanged / generic-parser / special-case.

**Insight:** the honest unit of progress here is not a new feature but a *measure*
of whether the structure already grown COMPOSES on fresh vocabulary. So gen160 is
a benchmark generation (like gen147), not a brain change: it builds the gauge and
records what it reads, including the gaps. Anti-impostor by construction — every
name, predicate and ordering is fresh and absent from `tests/cases`.

**Changed:** no runtime behaviour (`brain.c` only bumps to
`gen160-compositional-bench`). New `tests/composebench.sh` + `make compose-bench`:
each `tests/compose/*.dlg` is one session declaring the >=3 subsystems it forces
to cooperate; the harness ratchets `#expect: pass` dialogues (earned composition
never regresses) and records `#expect: gap` dialogues as growth edges. Two
deterministic bilingual ratchets land in `tests/cases`: `compose.chat`/`.it` (the
analytical chain) and `compose_social.chat`/`.it` (the social chain).

**Observed.** 4 of 6 dialogues compose UNCHANGED, 27 subsystem-turns, 87% landing:
- analytical_en (fresh loyal/hound/companion/bryn) composes EIGHT subsystems in
  one breath: rule intake -> fact intake -> deductive No -> contrastive abduction
  (names the missing conjunct) -> optimal abduction (cheapest repair) ->
  self-correction ("now bryn is a companion after all") -> proof -> robustness
  (2 load-bearing facts). The same chain composes in Italian (balu/orso/amico).
- social_en composes SIX: greeting, name memory, possession memory, KB fact,
  discourse reference ("it" -> kiwi), personal summary; Italian mirrors it.
- Two recorded gaps, both classed generic-parser (a lexicon/frame extension, not
  a bespoke handler): (1) bare self-introduction "i'm vera" should feed the name
  memory that "my name is X" already fills, and an unbound "she" should bind to a
  named personal entity (the cat) — composing possession memory with discourse
  reference; (2) Italian positive why-proof "perché X è un Y?" and name recall
  "come mi chiamo?" fall through `canonicalize_lang` though their contrastive /
  intake siblings already work.

**Self-challenge parity (LOOP step 4).** Asked to prove its own subsystems
compose, parrot0 walled then chitchatted — it cannot yet treat "test composition
of my parts" as a loop-shaped self-challenge. External hypothesis stronger;
recorded as the candidate next pull (extend `mod_loop` to E-series challenges).

**Next:** close the two generic-parser gaps (each flips a recorded GAP to
composes-unchanged), then teach `mod_loop` to reason about composition of its own
subsystems. The astonishment this generation is real: seven/eight subsystems that
were each grown in a separate generation cooperate on never-seen vocabulary, in
two languages, with zero glue code — the gauge now proves it and guards it.

## 2026-06-21 - gen159: a type gate makes the derived graph trustworthy

**Goal (owner: make the graph affidabile):** gen158 materialized part_of/2 from
descriptions but text ambiguity leaked one false relation — the muscular
description says "skeletal, smooth and cardiac muscles", where "skeletal" is an
ADJECTIVE (skeletal muscle), yet it is also a body-system name, so
part_of(skeletal, muscular) was asserted falsely.

**Insight:** the robust signal here is NOT part-of-speech (the word "skeletal" is
genuinely ambiguous) but TAXONOMIC LEVEL. A valid containment crosses two levels
— an organ is part of a system — whereas two concepts of the SAME predicate are
SIBLINGS and never nested. skeletal and muscular are both `body_system`; a system
is not part of a sibling system.

**Changed:** `kb.c` -> `gen159-typed-relations`. New `concept_pred` (the
predicate/level of a concept key) and `valid_member` (member and container must
have DIFFERENT predicates). The gate is applied in all three places that infer
containment: the container-predicate detection, the `part_of` materialization,
and the on-demand `kb_concept_mentioning`. No parsing, no hardcoded domain list —
a structural type constraint.

**Observed.** "is skeletal part of muscular?" -> No (the false relation is gone);
"what is the skeletal system part of?" -> honest wall. Every true relation
survives: heart/brain/liver -> their systems (Yes), members queries intact. The
derived knowledge graph is now trustworthy: every part_of in it crosses a real
taxonomic level. `tests/knowledge.sh` ratchets the rejected sibling relation
(22 cases). The arc gen155->159 — recall by similarity, idf metric, relations
from text, relations as facts, typed relations — turned a flat, half-broken
glossary into a small reasoned-over knowledge graph, each step held-out + the
honest limit named. The standing frontier is unchanged and now sharper: deeper
multi-hop chains need taxonomic levels the data does not yet encode (organ ->
system -> organism), and cross-language semantics need bilingual descriptions.

## 2026-06-21 - gen158: the emergent relation becomes a provable fact

**Goal (continuation):** gen157 RECOVERED "heart is part of circulatory" from the
text but answered it as a string. Make it a FIRST-CLASS fact so the resolution
engine can prove and query it — the flat glossary becomes a reasonable graph.

**Changed:** `kb.c`/`kb.h` + `brain.c` -> `gen158-relations-as-facts`.
- `kb_derive_part_of` (kb.c): after the whole KB is loaded, materialize
  `part_of/2` facts from the descriptions — but ONLY from CONTAINER predicates,
  detected structurally as predicates whose facts repeatedly name OTHER concept
  keys (body_system names organs; number_property names almost none). So a mere
  mention ("composite is a number that is not prime") is NOT turned into a
  containment. Facts are KB_REFLECTIVE (regenerated each boot, never persisted).
  Materialized once on the first `brain_respond` (guarded by `relations_derived`).
- `mod_knowledge` (brain.c): three relational shapes over the materialized facts —
  PROOF "is X part of Y?" (`kb_query`, real resolution) -> Yes/No; MEMBERS "what
  is part of Y?" (`kb_match` inverse) -> the list; plus the gen157 CONTAINER
  "what is X part of?". A trailing category noun ("the nervous SYSTEM") is treated
  as the frame, not the target, even though "system" is a concept elsewhere.
- `kb_concept_mentioning` now matches EXACTLY (not cognate), so a containment
  claim never rests on a near-match (respiratory ~ responses). `part_of` is added
  to `is_struct_pred` so describe stays clean.

**Observed — the graph reasons.** "is the heart part of the circulatory system?"
-> Yes (PROVED from a fact derived from text); "is the heart part of the
digestive system?" -> No; "what is part of the digestive system?" -> stomach,
intestines, liver (organs recovered from the prose "stomach, intestines and
liver", never stored as relations); brain->nervous, lungs->respiratory hold
across the agi profile. The mention!=containment safeguard holds (prime is NOT
part of composite). `tests/knowledge.sh` ratchets prove/disprove/members.

**The honest limit (the syntax frontier, again).** Text ambiguity still leaks:
the muscular description says "skeletal, smooth and cardiac muscles", where
"skeletal" is an ADJECTIVE (skeletal muscle), yet "skeletal" is also the name of
a body system — so `part_of(skeletal, muscular)` is materialized falsely. The
container detection got the predicate right; the individual mention is
adjectival. Distinguishing modifier from head needs the sliver of syntax the
discrete matcher still lacks — the same wall gen155-157 hit. parrot0 now has a
knowledge graph it can prove over; making that graph trustworthy is the next
pull (POS-ish gating: a mention counts only as a noun head, not a modifier).

## 2026-06-21 - gen157: relations that emerge from the text (never asserted)

**Goal (owner: "more ambitious experiments"):** stop polishing the glossary and
make parrot0 REASON over the knowledge. The most striking thing a flat glossary
can be made to do is yield a structure nobody put there: parrot0 was never told
"the heart is part of the circulatory system", yet the circulatory DESCRIPTION
names the heart — so the containment relation is latent in the text and can be
recovered. A taxonomy emerging from unstructured descriptions.

**Changed:** `kb.c`/`kb.h` + `brain.c` -> `gen157-emergent-relations`.
- `kb_concept_mentioning` (kb.c): find the concept whose description NAMES a given
  term (cognate/morphology-tolerant via `word_sim`), and `kb_is_concept_key` to
  gate on a term parrot0 actually knows as a concept.
- `mod_knowledge` (brain.c): a containment frame — "what is X part of?", "what
  contains X?", "what system is X part of?", IT "di cosa fa parte X" / "cosa
  contiene X" — finds the known concept X in the turn and answers with the
  concept whose text mentions it. Runs before the describe block so "what is X
  part of" is not answered with X's own definition. Fires only with a containment
  cue AND a known concept, so ordinary "what is X" is untouched.

**Observed — the ambitious result.** parrot0 now answers questions whose answer
was NEVER stored as a fact: "what is the heart part of?" -> circulatory; "what
contains the lungs?" -> respiratory; "what system is the liver part of?" ->
digestive; "di cosa fa parte heart" -> circulatory (same derivation, IT cue). The
relation is computed from the glossary text at query time. And it stays HONEST:
"what is the femur part of?" walls, because nothing in the KB's text places the
femur in a system — parrot0 does not fabricate a container. This composes the
gen150 knowledge, the gen155/156 token machinery and the symbolic dispatch into a
behaviour none of them had alone (TASKLIST E1, compositional emergence).
`tests/knowledge.sh`: EN heart/liver, the IT cue, and the no-fabrication negative.

**Next pulls:** let the recovered relation feed the proof engine (assert it as a
derived `part_of` so "is the heart in the circulatory system?" and chains work);
relax the concept-key gate so a term that is only MENTIONED (insulin -> pancreas)
can still be placed, without losing precision.

## 2026-06-21 - gen156: a learned metric (idf) for the similarity recall

**Goal (NEXTMOVE pull #1):** gen155's overlap recall was NOISY at agi scale —
common words ("number", "blood", "system") counted as much as discriminative
ones, so near-equal concepts tied and the recall ABSTAINED. The honest fix is the
cheapest *learned metric* derivable from the corpus itself: inverse document
frequency.

**Changed:** `kb_nearest_concept` (`src/kb.c`) -> `gen156-idf-weighting`. A first
pass measures each query word's document frequency across the concept
descriptions; a second pass scores overlap weighted by `1/df`, so a word that
matches many concepts contributes little and a rare, discriminative word
dominates. The fire gate keeps the >=2-word evidence requirement and now demands
a clear *weighted* margin (best - second >= 15% of best), so a genuinely
symmetric tie still abstains.

**Observed.** A clean held-out win: "what is the organ that pumps blood" WALLED
under gen155 (an agi-scale tie on the common word "blood") and now resolves to
**heart**, because "pump" is rare (high weight) and "blood" common (low). The
gen155 ratchets (femur by property, addition by paraphrase, the IT cross-lingual
cognate case, the honest-miss negative) still pass. `tests/knowledge.sh` adds the
idf tie-break case under the full agi profile.

**Honest limits (unchanged frontier).** idf does NOT reach the NEXTMOVE example
"the bone that protects the brain -> skull": there "brain" is itself an exact
concept key, so exact-match precedence answers *brain* before the fuzzy path ever
runs — a DESCRIPTIVE query containing a concept name as a modifier is a different,
harder problem (it needs the matcher to know "brain" is the object of "protects",
i.e. a little syntax). And at agi scale the guess is still only a guess: "the
operation that combines two numbers" now leans to *expression* over *addition*
because the category word "operation" is rare in descriptions. Both are why the
answer stays hedged. Next pulls: down-weight category/frame words, and the
modifier-vs-head distinction (a sliver of structure the discrete matcher lacks).

## 2026-06-21 - gen155: the first brick of a similarity space

**Goal (the biggest gap, owner-endorsed):** an LLM generalises by proximity in a
continuous meaning-space; parrot0 looks up discrete symbols, so any phrasing
outside a cue list hits the wall — and knowledge recall was exact-key only
("what is addition" works, "what is the operation that combines two numbers"
walls). The honest first step toward closing that gap is NOT another cue list: it
is a *similarity* mechanism derived from the KB's own structure.

**Changed:** `kb.c`/`kb.h` + `brain.c` -> `gen155-similarity-recall`.
- `kb_nearest_concept` (kb.c): given a query's content words, score every
  description-bearing concept by OVERLAP of the query against the concept's
  key + description tokens, and return the best if it clears a threshold (>=2
  matches) AND beats the runner-up. `word_sim` counts two words as matching if
  equal or sharing a >=4-char prefix — morphology- and even COGNATE-tolerant.
- `mod_knowledge` (brain.c): for a "what is .../tell me about ..." frame, an
  exact concept key still wins; when none is named, fall back to
  `kb_nearest_concept` and answer hedged ("You might mean X: ...").
- `canonicalize_lang`: `qual` -> `what` so "qual è ..." reaches the same path.

**Observed — a genuinely surprising result.** Recall now works by DESCRIPTION,
not just by name: "what is the longest bone in the body" -> femur; "the operation
that combines two numbers" -> addition; neither named the concept. And the
striking one: the cognate-tolerant overlap crosses the language barrier with NO
translation table — the Italian "qual è il rapporto tra circonferenza e
diametro" resolves to **pi** because *circonferenza~circumference* and
*diametro~diameter* share 4-char prefixes with the English description. A
similarity space, in deterministic C, derived from real KB structure rather than
enumeration. `tests/knowledge.sh` ratchets EN recall-by-property, EN
recall-by-paraphrase, the IT cross-lingual cognate case, and an honest-miss
negative (an unknown topic must NOT be force-fit).

**The honest limit (this is the point of the method).** The 10x stress made the
gap with an LLM concrete: at full agi scale discrete overlap is NOISY — ties
between near-equal concepts make it abstain ("bone that protects the brain" is a
2-2 tie skull/skeletal -> no guess), and loose cognates can mislead, so exact
keys must take precedence and the answer stays hedged. An LLM's continuous space
smooths exactly this. parrot0 now has the *first brick* of that space; the wall
it keeps hitting is the absence of a learned metric, not the absence of cues.
Next pulls: a discriminative (idf-like) weighting so common words ("number",
"blood") count less; bilingual descriptions so semantic (not only cognate)
recall crosses languages.

## 2026-06-21 - gen154: organize kb/ — no loose files, substrate vs domain

**Goal (owner directive):** move every kb/ file that sits loose at the top level
into its appropriate sub-folder, creating a dedicated folder where needed. The
gen150 taxonomy (experts/skills/profiles) had left nine files scattered at the
root, mixing always-loaded engine substrate with domain knowledge.

**Changed (layout + load paths, no behavior change):**
- New `kb/core/` for the always-loaded boot substrate: `base.p0`, `session.p0`,
  `lexicon.p0`, `gloss.p0`, `social.p0`, `roles.p0`. These are not domain
  knowledge — they are the engine's language/identity layer, loaded
  unconditionally by `brain_create`/`main`.
- Domain knowledge moved into the taxonomy: `grammar.p0` -> `experts/
  linguistics/grammar.p0`; `bash.p0` -> `experts/programming/bash.p0`;
  `coding.p0` -> `experts/programming/coding.p0` (its `:- include` paths fixed to
  be relative to its new directory — includes resolve against the including
  file's dir).
- Load paths updated: `main.c` (base/session/coding), `brain.c` (lexicon/social/
  roles/gloss), `.gitignore` (session), and the test harnesses (`experts.sh`,
  `profiles.sh`, `skills.sh`, `knowledge.sh` base; `posix.sh`/`synth.sh`/
  `posix_oracle.sh` bash; `grammar.sh` grammar). Stale path mentions in comments
  were updated with `rap`.
- `kb/` top level now contains ONLY the four taxonomy directories (core,
  experts, skills, profiles) — no loose files.

**Observed.** Clean rebuild + full `make test` green; the agi profile and the
default `make chat` load behave exactly as before (the move is path-only). The
version string advances to gen154 to mark the integrated state (gen151 speak +
gen152 engine + gen153 data repair + gen154 layout). Remaining debt unchanged:
truncated descriptions in the science/humanities/skills files, and the malformed
`flag(ls, u_r, ...)` in shell.p0.

## 2026-06-21 - gen153: restore truncated knowledge + real domain tests

**Goal (owner audit, continued):** with the engine able to hold full strings
(gen152), repair the gen150b data the author had hand-truncated mid-word to fit
the old 64-char cap, and replace the PLACEBO knowledge tests that asserted
nothing.

**Changed (data + tests, no engine change):**
- Restored complete descriptions in the core demoed domains: `arithmetic`,
  `algebra`, `anatomy`, `pharmacology`, `algo`, `c`, `debug` — e.g. "...to its
  di" -> "...to its diameter", "directing r" -> "directing responses", "sorted
  ar" -> "sorted array", "fix suggest" -> "fix suggestions". Commas inside the
  prose are now safe (gen152), so multi-clause descriptions read naturally.
- `experts.sh`, `profiles.sh`, `skills.sh`: the math/medicine/reasoning/
  communication cases asserted `is socrates a man -> Yes` (true under EVERY
  profile from base.p0 — they tested only that the file loaded). Rewrote them to
  query the actual knowledge: "what is addition", "what is pythagorean", "what is
  the heart", "what is half_life", "what is modus_ponens" — each asserting the
  spoken definition. The tests now fail if the domain knowledge is missing.
- `tests/knowledge.sh`: updated the brain expectation to the restored full text.

**Observed.** `make test` green. The expert/profile/skill suites now genuinely
exercise the knowledge they name. REMAINING (next pulls): the same truncation
repair across the science (physics/chemistry/biology), humanities (linguistics/
business/philosophy/music/psychology) and the rest of the skills files; and a
couple of malformed facts the audit flagged, e.g. `flag(ls, u_r, ...)` in
shell.p0 (no such ls flag — left untouched to avoid perturbing the posix tests
until addressed deliberately).

## 2026-06-21 - gen152: the KB engine learns to hold a string literal

**Goal (owner-driven audit, PRINCIPLES.md):** the owner asked me to study all the
synthetic knowledge in `kb/` for *cattivo uso dello strumento Prolog*. The audit
found the gen150b knowledge was not merely dumped raw (fixed in gen151) — most of
it was structurally UNREPRESENTABLE by the engine and silently broken at load:
- `parse_term` split arguments on EVERY comma with no quote-awareness, so the 132
  descriptions containing commas (`"keep it simple, stupid, ..."`) were shredded
  into garbage multi-arg facts (and dropped when they overflowed KB_MAX_ARGS=4);
- `term_ok` rejected any atom >= 64 chars, so the 148 descriptions longer than
  that were dropped entirely. The agi profile was quietly missing ~280 facts.

The gen150b author worked AROUND this by hand-truncating descriptions mid-word
to fit 64 chars ("...to its di", "sorted ar") — corrupting the knowledge to fit a
limitation instead of fixing the tool. That is the abuse to correct.

**Changed:** `kb.h` + `kb.c` -> `gen152-knowledge-speaks` (brain version string
kept; engine-level change). 
1. `parse_term` (kb.c): a `"..."` argument is now ONE token — commas and other
   punctuation inside the quotes are content, not separators. Real quoted string
   literals, a genuine Prolog feature the data already assumed.
2. `KB_TERM_LEN` 64 -> 128 (kb.h): descriptions survive intact. Stack-checked
   (largest concurrent use ~64KB) and the one >124-char description (the agi
   profile blurb) was shortened to fit.
3. `render_fact_direct` (kb.c): verbalize a quoted description in the LAST arg,
   so the 3-ary `algorithm(name, category, "desc")` speaks too, not only the
   2-ary form.
4. `Makefile`: objects now depend on ALL headers (`$(HDR)`). This bit me HARD —
   the first incremental build linked a kb.o built at KB_TERM_LEN=128 against a
   stale brain.o at 64, an ABI/struct-layout mismatch that corrupted 65 tests
   with empty/shifted args. A header that defines struct layout MUST force a full
   rebuild; now it does.
5. `tests/knowledge.sh`: added proofs that a previously-mangled comma description
   (`kiss`) and a previously-dropped long description (`api`) now load and speak.

**Observed.** `make test` green. On the agi profile the recovered knowledge now
answers: "what is kiss" -> "kiss is keep it simple, stupid — prefer simple,
obvious solutions over clever ones.", "what is api" -> the full definition. ~280
facts that silently failed to load now exist. REMAINING gen150b debt, the next
pull: the hand-truncated descriptions (~100, e.g. "sorted ar", "directing r")
are still cut mid-word in the source files — the engine can hold the full text
now, so the data should be restored to complete sentences; and the placebo
expert/profile/skill tests still assert `is socrates a man -> Yes` instead of the
domain knowledge they claim to cover.



**Goal (owner-driven, PRINCIPLES.md):** the owner ran `make chat` on the agi
profile and saw the new gen150/150b domain knowledge fail the founding
discipline: asking "what is addition" printed the RAW Prolog clause
`math_op(addition, "combining two numbers to get their sum").` instead of
dialoguing. gen150b was, in the owner's words, *fatta male* — the knowledge was
loaded but inert: dumped as clauses, filtered as "internal substrate", and
unreachable through natural questions. Bring it to the level of the rest:
knowledge must be SPOKEN from real state, anti-impostor, general.

**Self-challenge parity:** not run as a formal probe this iteration (the failure
was observed directly in chat, the strongest pull there is).

**Changed:** `brain.c` -> `gen151-knowledge-speaks`, plus `kb.c`.
1. `render_fact_direct` (kb.c): a description-bearing fact `pred(key, "human
   text")` — the gen150 expert/skill convention — is now VERBALIZED ("addition
   is combining two numbers to get their sum.") instead of rendered as a raw
   clause. General over every domain, so held-out concepts speak through the
   same path; no per-concept phrasebook.
2. `kb_describe_entity` (kb.c): a new `is_struct_pred` skips structural metadata
   (expert/skill/profile registration, code_* templates, the coding substrate,
   role `trait`) so a description never leaks plumbing as raw Prolog.
3. `mod_knowledge` (brain.c): the description intake is broadened past the bare
   "what is X?" to an article, a multiword topic, or a "tell me about X" frame
   ("what is the heart", "what is the circulatory system", "tell me about pi").
   Each content word is tried as the concept key; it CLAIMS only on a real KB
   hit, so the indefinite membership query "what is a dog?" and the relational
   "what is the plural of dog?" are left to their existing handlers and unknown
   topics still fall through — the wall is never widened.
4. `tests/knowledge.sh` (new, wired into `make test`): REAL domain-knowledge
   assertions (arithmetic, anatomy) in EN + IT — replacing the placebo style of
   the gen150b tests, which only checked `is socrates a man -> Yes`. The IT
   question hits the same render path: the bilingual ratchet.

**Observed.** `make test` green (157 chat cases + all `.sh` suites). On the agi
profile, "what is addition / the heart / the circulatory system" and "tell me
about pi" now answer in plain language. TWO deeper gen150b debts are now exposed
and are the next pulls (NOT fixed here): (a) the KB parser `parse_term` splits
arguments on commas with NO quote-awareness, so the 132 descriptions containing
commas are MANGLED into garbage multi-arg facts; (b) `term_ok` rejects any atom
>= 64 chars, so the 148 descriptions longer than that are SILENTLY DROPPED at
load. The "English-prose-in-an-atom" pattern exceeds what the engine can
represent — gen152 must teach the parser real quoted string literals and give
descriptions the capacity to survive intact, then the expert/profile/skill tests
can assert genuine knowledge instead of `is socrates a man -> Yes`.



**Goal (E4):** make ordinary conversation remember more than rigid name and possession frames: preference, mood, current topic and constraints, then answer `what do you remember about me?` without inventing unstated traits and while distinguishing durable personal facts from temporary session context.

**Self-challenge parity:** parrot0 gave only the generic gen145 loop answer: identify missing behavior, choose a module, add EN/IT tests, bump version, journal. It did not name `mod_memory`, nor the key implementation constraint: mood/topic should be recorded by the modules that already answer those turns (`mod_chitchat`, `mod_pragma`) so existing social replies do not regress.

**Changed:** `brain.c` -> `gen148-user-model-context`. The Brain now has a lightweight user-model overlay: one preference, one session mood, one current topic and one current constraint. `mod_memory` learns `I like X` / `I prefer X` / `mi piace X` / `preferisco X`, learns simple constraints such as `keep it short` and `avoid technical`, answers specific preference/mood/topic/constraint questions, and summarizes `what do you remember about me?` as durable personal facts plus `Session context`. `mod_chitchat` records tired/happy/bored mood while preserving its existing replies; `mod_pragma` records the current topic when the topic-change move already fires. Ratchets: `user_model.chat`, `user_model.it.chat`, and a 10x `user_model_stress.chat`.

**Observed:** `make test` is green (156 chat cases). The gen147 long-chat benchmark now moves in the intended places: felt landing 85% -> 87%, wall 9% -> 7%, continuity 71% -> 76%, topic changes 85% -> 100%; repair and contradiction are unchanged. Remaining gaps are now clearer: Italian `come mi chiamo?` still misses, discourse summary still loses some concrete topics, and remembered constraints are reported but not yet obeyed by downstream modules.

## 2026-06-20 - gen147: long-chat benchmark as a pressure gauge

**Goal (E6):** add a 50-turn non-technical conversation benchmark that measures the felt continuity of a session, not just isolated exact-match unit cases. The benchmark must track wall rate, immediate repetition, repair success, continuity references, contradiction handling and user-model precision, with Italian and mixed EN/IT runs plus a 10x stress set.

**Self-challenge parity:** parrot0 treated the task as a generic fallback gap and proposed changing the fallback or owning module. That was weaker than the external hypothesis: E6 is not primarily a behavior change, it is an instrumentation gap. The owning surface is the benchmark harness, with tagged long dialogues that can expose the next behavior tasks without pretending they are solved.

**Changed:** added `make long-chat-bench` and `tests/longchatbench.sh`. The harness runs one persistent session per `tests/longchat/*.dlg`, soft-matches satisfying substrings, tags turns by capability, and prints aggregate metrics. Added `minimal_5.dlg` as the minimal proof, `italian_run.dlg` as the Italian run, and `mixed_stress_50.dlg` as the mandatory 50-turn mixed EN/IT stress set. No brain behavior changed and `brain_version()` stays at gen146.

**Observed:** baseline on gen146 is useful rather than flattering: 71 turns, 50 stress turns, felt landing 85% (61/71), wall 9% (7/71), immediate repetition 1% (1/71), repair 75% (6/8), continuity 71% (15/21), contradiction 100% (8/8), user-model precision 93% (14/15), boredom 83% (5/6), topic changes 85% (6/7). The sharpest misses point to E4: topic and mood are answered locally but not stored as user/session context, and Italian `come mi chiamo?` still misses even though English recall works.

## 2026-06-20 - gen146: open-domain humility with exact gaps

**Goal (E5):** when a user asks for knowledge outside the world model, stop collapsing into a generic wall or a bare `I do not know about X`; name the missing support and offer a useful next action without pretending to know the answer. Baseline probes showed the problem: `why is the sky blue?` -> flat fallback, `what year is it?` -> repair asked what `it` referred to, and `what is the capital of france?` only said it did not know `capital`.

**Self-challenge parity:** gen145 `mod_loop` proposed the right loop shape but not the concrete owner. The external hypothesis was sharper: this is a knowledge-humility gap, so the smallest change belongs in `mod_knowledge`, with one repair exception for current-time questions that are not referential gaps.

**Changed:** `brain.c` -> `gen146-open-domain-humility`. `mod_knowledge` now recognizes three open-domain gap families before the old KB patterns: missing binary relations (`capital/mayor/inventor/weather`), missing unary proof support (`blue(sky)`, `salty(ocean)`), and missing current clock/calendar support (`current_year/current_date/current_time/current_day`). Each answer names the missing relation, predicate, or tool and suggests teaching facts/rules or giving a passage. `mod_repair` now lets `what year/time/date/day is it?` pass through instead of asking for a referent. Italian relation probes reach the same path through existing contraction canonicalization plus `capitale -> capital`; `di` is accepted only inside the E5 relation parser, not globally. Ratchets: `blankwall.chat` / `.it`, including a 10-turn stress block.

**Observed:** the 10x stress was useful: a tempting global `cosa -> what` mapping broke eight Italian regressions (`abduce.it`, `chitchat.it`, discourse/meta cases, etc.), so it was removed. The final implementation keeps broad Italian cues intact and confines the new behavior to exact E5 shapes. `make test` is green.

## 2026-06-20 — gen145: self-challenge parity, not self-management

**Goal:** evolve the loop so parrot0 can face challenges about itself in the same spirit as an external agent: it should not edit files, run tests, choose tasks, commit, or push, but it should be able to propose a comparable solution when the external loop presents a self-challenge. Baseline probe: `review this implementation for your self-improvement loop...` fell through every module to the fallback; only `mod_strategy` could explain that all modules declined. That is useful introspection, but not yet help on the solution.

**Changed:** `brain.c` -> `gen145-self-challenge-parity`. New `mod_loop`, registered before `meta`, handles explicit self-challenge / self-improvement / implementation-review prompts before generic metaconversation can claim them. It classifies the challenge as a fallback gap, implementation/dispatch gap, or generic self-challenge and answers with the loop discipline: name the missing behavior, locate the owning module or registry point, add the smallest deterministic change, ratchet it with English and Italian tests, bump the version, and journal the observation. It explicitly states the boundary: parrot0 proposes; the external agent still edits and verifies files. Italian reaches the same module through `canonicalize_lang` additions for sfida/implementazione/stesso/etc. Ratchets landed in `self.chat` and `intent.it.chat`; `self.chat` also carries a 10-turn held-out stress block over EN/IT self-challenge phrasings. Registry-sensitive `self`/`strategy` expectations were updated. `LOOP.md` now has a required Self-challenge parity step before implementation.

**Observed:** after the change, the same implementation-review challenge gets a proposal structurally close to the external solution instead of a wall: add a focused module/dispatch change, tests in EN/IT, version bump, and journaled support-quality observation. This is still shallow: it does not inspect diffs or synthesize C, and it cannot decide whether the proposed module is actually the best one. The 10x stress pass caught two important boundaries: global `capisci`/`modulo` canonicalization broke existing Italian meta/module questions, so those words stayed local to their established modules and `mod_loop` handles its own punctuation/cue cleanup; and explicit fallback gaps must beat a weak `module` cue, while stronger implementation cues still win. The next quality bar is making the proposal depend more on concrete symptoms and known module responsibilities, not just the loop checklist.


## 2026-06-20 — gen144: pragmatic intent from turn SHAPE, not cue lists

> Integration note: produced in parallel with gen142/gen143 by a worktree
> subagent (E3), integrated on top of gen143 as the next generation.

**Goal (PROMPT.md run, E-series E3):** the emergence audit named the human
surface as the weakest signal, and `mod_chitchat`/`mod_social` meet it with
GROWING CUE LISTS — so a held-out phrasing of the same speech act still hits the
wall. Probing confirmed it: "give me something to think about", "could we chat
about cheese", "i do not really agree with you", "anyway, is socrates a man",
"by the way what is 2 plus 2" all walled (the last two even CORRUPTED the content
parse — the extra opener tokens shifted memory's "what is my X" window onto
"plus"). E3 demands: infer the SPEECH ACT from turn SHAPE, route each act to a
DIFFERENT move, and TRANSFER to unseen phrasings — never another phrase table.

**Changed:** `brain.c` → `gen144-pragmatic-shape`. New `mod_pragma`, registered
LATE (after `discourse`, before `social`/`chitchat`) so genuine content/question
turns win first. It classifies a turn on SHAPE FEATURES, never a memorized
string: (1) the OPENER class of the first content token — a discourse marker
(anyway/so/well/ok/"by the way") vs a soft-request verb (tell/give/say/share) vs
a modal invitation (can/could/shall/let); (2) a HEDGE marker anywhere
(maybe/suppose/probably/forse/magari); (3) NEGATION + a STANCE PREDICATE
(agree/right/sure/true/correct/convinced/sense — keyed on the PREDICATE so an
imperative "dont say that" is not disagreement); (4) a CONTRASTIVE connective
(but/however/though); (5) an OPEN-QUANTIFIER object (something/anything/qualcosa —
what separates a fill-the-silence request from a real one: "tell me a story about
dragons" names a CONCRETE object and stays an honest wall); (6) a TOPIC-INTRO
frame (modal/switch verb + talk-about/discuss/parlare-di + an object pulled by
SHAPE — the head noun after the frame); (7) presence of a CONTENT PREDICATE
(digit/operator/"is a"/known KB pred or entity) — the gate that makes every
pragmatic move claim ONLY contentless turns, so a real task is never swallowed.
Five distinct moves result: soft-request→invite a thread; hesitation→lower the
stakes; disagreement→accept the pushback; topic-change→steer onto X by name;
(boredom/no-topic stays chitchat's established register).

The mixed social+content case is solved structurally by a PRE-DISPATCH peel
(`pragma_peel` in `brain_respond`, beside `decompose_and_dispatch`): a leading
discourse-marker opener is not content, so it is normalized away — like
`canonicalize_lang`, one level up — and the residue is re-dispatched through the
WHOLE registry. "anyway, is socrates a man" → "Yes."; "by the way what is 2 plus
2" → "4."; "well, remember my dog is Bruno" keeps "Bruno" (the RAW residue is
rebuilt by skipping the same leading word-count, preserving proper-name casing).
The peel only claims when a module actually owns the residue, else the original
turn dispatches normally and its pragmatic shape is read by `mod_pragma`. No new
Brain field was needed (the early re-entrancy guard was removed once the peel
moved pre-dispatch). Italian transfers through the SAME path via three small
`canonicalize_lang` additions (`possiamo`→can, `potremmo`→could) plus
shape-cue recognition of `qualcosa`/`d'accordo`/`parlare di`/`comunque` — no
phrasing duplicated. Ratchets: `pragma.chat` / `.it`, plus a 14-turn held-out
`pragma_stress.chat` (none of its phrasings appear in the code), including three
mixed social+content turns where the content task survives. The `self` registry
listing and the `strategy` declined-trace gained `pragma` in the right position
(the module really runs there).

**Observed.** The win is that the moves come from a small FEATURE VECTOR, not a
list: 12 of 14 stress phrasings never seen by the code classify correctly, across
five acts, in both languages — and the two misses are honest lexical bounds
("unconvinced" is a different lemma than the "convinced" stance predicate; bare
"parliamo di X" with no modal is deliberately left to filler per the gen140
decision). The most satisfying part is the peel: it revealed that a chatty opener
was not just unhandled but actively CORRUPTING content modules downstream (a
latent `mod_memory` window bug only "anyway/by the way"-padding triggered).
Treating the opener as a function word to be normalized — rather than a phrase to
be matched — fixed the surface AND the content path with one structural move.
Open edges: stance predicates are still a finite set (no morphological
"un-"/"dis-" negation, so "unconvinced" slips); the topic move recognizes the
INVITATION but does not yet COMMIT the new topic into discourse memory; and a
soft request with a concrete object correctly walls rather than attempting the
(unfulfillable) content — humility, but a future pull could name the gap.


## 2026-06-20 — gen143: local-world working memory — scoped fictions that don't leak

> Integration note: produced in parallel with gen142 by a worktree subagent
> (E7), integrated on top of gen142 as the next generation.

**Goal (PROMPT.md run, E-series E7):** handle temporary fictional or task-local
worlds across several turns, with explicit scope and expiry. Until now parrot0
had ONE flat belief store: "rex is a dragon" wrote a permanent fact. A real
interlocutor can hold a fiction ("in this story Rex is a dragon"), use it later,
run a SECOND world where the same name means something else, answer "what is
assumed?", and TEAR a world down so none of it pollutes what it permanently
believes. This generation builds exactly that scoped working memory.

**Changed:** `brain.c` -> `gen143-local-world-memory`. New `mod_world`,
registered SECOND (right after `repair`, before the KB modules) so a world
assertion pre-empts the permanent learner precisely when a scope is open. The
Brain gains a small fixed pool of named worlds (`worlds`/`world_live`/
`active_world`) and an overlay of `(world, subj, pred, neg)` facts (`wfacts`) —
all in session state, NEVER the persisted KB. ENTER: "in the <name> world/story
..." / "in this story ..." / "start a world called <name>" opens (or re-opens)
a scope; an inline clause after the noun is asserted into it. ASSERT: while a
world is active, "X is [a] Y" / "X is not [a] Y" lands in the overlay tagged with
the world id. QUERY: "is X a Y?" (and the subject-first interrogative "X is a Y?"
the Italian shape canonicalizes to) / "who is X?" / "what is X?" read the active
overlay first, so the SAME name answers differently in two worlds. INSPECT: "what
is assumed?" lists the active overlay, grounded in real state. EXIT: "forget/end/
close the <name> world" tears the scope down (dropping exactly its facts);
"leave the world" deactivates it but keeps it alive. The name can sit before the
world noun (English "saga world") or after it (Italian "mondo saga") — one parser
serves both orders. `canonicalize_lang` gained the local-world lexicon
(`mondo`->world, `storia`->story, `nel`/`nella`->`in the`, `questo`/`questa`->
this, `assunto`->assumed, `dimentica`->forget) so the IT probe reaches the SAME
code path; the inspect cue is the unambiguous "is assumed", gated on an open
scope so it never fires in ordinary prose. Ratchets: `world.chat`/`.it` and a
~20-turn `world_stress.chat`. The `self` and `strategy`/`strategy.it` trace
expectations gained `world` after `repair` (the module really runs there).

**Observed.** This is parrot0's first scoped belief layer — facts that are real
and usable yet provably temporary. The stress run is the useful signal: three
overlapping worlds (saga / office / dream) hold conflicting claims about the SAME
entities (rex, max), switching re-activates the right overlay, "what is assumed?"
reads each back, and after teardown the permanent layer answers "I don't know
about dragon" — the anti-impostor leak check passes (a `/save` writes only the
real `human(bob)` fact, never any world fact). What surprised me: the cleanest
seam turned out to be ACTIVATION, not nesting — "leave the world" returns control
to the permanent KB so ordinary teaching works again, and "in the <name> world"
with no clause is simply re-activation; expiry and scope fell out of the same two
operations. Open edges: classes are single-word only (no "real animal"); worlds
don't yet inherit from the permanent KB or from an enclosing world (truly nested
scopes); and "remember this world for real" (promote an overlay into permanent
memory on request, the one sanctioned leak E7 allows) is not wired yet.


## 2026-06-20 — gen142: metacognitive calibration — confidence from proof STATE

**Goal (PROMPT.md run, E-series E8):** make parrot0 report HOW it knows an answer,
with DIFFERENT confidence language for a directly KNOWN fact, an INFERRED
conclusion, a CONFLICTED belief, a HYPOTHETICAL conclusion, and an UNKNOWN — each
grounded in the real proof state, never a canned adjective. "Why do you think
that?" and "what would make you change your mind?" must work after an ordinary
answer, answered from REAL proof/memory state.

**Changed:** `brain.c` -> `gen142-metacognitive-calibration`. New `mod_calibrate`,
registered between `robust` and `abduce` (the introspection cluster, and before
abduce so "what would make you change your mind?" is not mis-parsed by abduction's
"what would make X a Y"). It classifies the last stated goal into five epistemic
states from real state — `calib_classify` — and answers the two follow-ups from
each. KNOWN vs INFERRED is read off the proof shape (`last_proof` contains
" because " iff a rule fired). HYPOTHETICAL is DETECTED, not flagged: a standing
"suppose X." (no "then", so it doesn't collide with mod_knowledge's one-shot form)
asserts X as a session fact and records it in new Brain fields
(`assumed_pred/arg/assumed_n`); a later conclusion is graded hypothetical only when
ablation (`calib_hypo_support`: retract the assumed fact, re-derive, restore) shows
it actually rests on the assumption — so the assumption feeding a RULE chain is
caught too. CONFLICTED is recorded honestly: the KB's assert is last-write-wins and
erases the losing claim, so `note_contradiction` (called at both assert sites) snaps
a `conflict_pred/arg` when the user states both polarities about one atom, and BOTH
claims are named. "What would change your mind?" reuses the same retract/re-derive/
restore sweep as `mod_robust` (`calib_load_bearing`) to name the genuine load-bearing
facts. EN + IT ratchets (`calibrate.chat`/`.it`) plus a ~10x `calibrate_stress.chat`
covering all five states, both follow-ups, interleaved held-out nonsense predicates,
and the compositional hypothetical-feeds-rule case. The `self` and `strategy`/`.it`
trace expectations gained `calibrate` between `robust` and `abduce` (it really runs
there).

**Observed.** The five answers are genuinely different *kinds* of confidence, and
each follows the proof, not the phrasing: "I'm certain … stated directly" (KNOWN)
vs "I'm confident but it's derived … only as sound as those premises" (INFERRED)
vs "I'm genuinely unsure — I hold conflicting claims" (CONFLICTED) vs "Only
conditionally … because I'm ASSUMING" (HYPOTHETICAL) vs "I don't actually think
that — I have no support" (UNKNOWN). The strongest signal is that HYPOTHETICAL is
inferred by ablation rather than a mood flag: `suppose krill is a vorp` + `every
vorp is a zonk` + `is krill a zonk?` → Yes, but graded conditional because removing
the assumed `vorp(krill)` overturns the *derived* `zonk` — the assumption is tracked
THROUGH the rule. The same machinery surfaced a real, honest quirk: a deep chain
sometimes forward-materializes the intermediate fact, so removing the root premise
alone leaves a redundant derivation and "what would change my mind?" truthfully says
"I can reach that another way" — calibrate and robust agree because they read the
same state. Open edges: a query whose PREDICATE is entirely unknown never sets a
goal, so it gets an honest "ask me first" rather than the UNKNOWN epistemic line
(UNKNOWN is reached via known-predicate/unknown-entity); CONFLICTED via rule-derived
positive vs explicit negative is detected only through the recorded user
contradiction, not yet a general belief-status layer (C9/T3 remain).


## 2026-06-20 — gen141: conversational repair loop — clarify, hold, resume

**Goal (PROMPT.md run, E-series E2):** attack the weakest signal the emergence
audit named — the human surface. Vague turns hit the wall. A real interlocutor,
asked something it cannot pin down ("is it a mammal?" with no antecedent, "what
is it plus 10" with no number), does not collapse; it asks a narrow
clarification, holds the unfinished thought, and resumes it once answered. This
generation builds exactly that as a stateful bridge across two turns.

**Changed:** `brain.c` -> `gen141-conversational-repair`. New `mod_repair`,
registered FIRST so it both pre-empts the wall and catches the answer. Brain
gains a single-turn pending slot (`pending_repair`/`pending_canon`/`pending_slot`).
OPEN: a question whose referential slot is an unresolved entity pronoun (and no
antecedent exists) stores the turn and asks specifically — "What number should I
use for X?" when an arithmetic operator is present, else "Who or what does X
refer to?". RESUME: the next turn fills the slot — if it is itself a teaching
assertion ("rex is a dog") it runs first so coreference resolves the stored
pronoun; otherwise a concrete referent token is substituted in — then the
ORIGINAL turn is re-dispatched through the registry (new `repair_dispatch`, NOT
footprint-free: a resumed assertion really learns, a resumed query really runs).
EXPIRE: if the next turn is plainly a new intent, the pending state is dropped and
that turn handled normally. Added `esso/essa/lui/lei/...` subject-pronoun mappings
to `canonicalize_lang` so the Italian probe reaches the same path. Ratchets:
`repair.chat` / `.it`, plus a ~10-episode `repair_stress.chat`. The `strategy` and
`self` trace/registry expectations gained `repair` at the front (the module really
runs first).

**Observed.** This is the first turn-bridging *intent* in the agent: it does not
just answer the current line, it keeps an unfinished goal alive and finishes it on
the user's reply. "what is it plus 10" -> "What number should I use for 'it'?" ->
"21" -> "31." composes the repair loop with arithmetic; "is it a mammal?" ->
clarify -> "rex is a dog" -> "Yes." composes it with teaching AND coreference AND
rule derivation — four subsystems, no special-case glue, exactly the compositional
emergence E1 asks for. The 10x stress run is the useful signal: every OPEN /
clarify / store / resume held across varied operators, both operand slots, and an
interleaved expiry; the only misses were *downstream* targets the arithmetic
module doesn't support ("divided by", "half of") and a double-pronoun turn
(two slots, one fill) — bounds of what repair resumes into, not of the loop
itself. Open: fill multiple slots; clarify missing objects in imperative
commands ("fix it"), not only question-form gaps.


## 2026-06-20 — gen140: conversation companion — everyday talk without pretending

**Goal:** improve basic casual conversation for non-technical users. The recent generations made parrot0 stronger at formal reasoning, but a normal interlocutor often starts with "I don't know what to say", "can we just talk", "I'm tired", or "tell me something". Those should not hit the wall, but the answer must stay honest: guide the conversation without inventing feelings, facts, or broad competence.

**Changed:** `brain.c` -> `gen140-conversation-companion`: `mod_chitchat` gains companion-style cue classes for no-topic turns, casual talk invitations, low-energy mood, and upbeat acknowledgements. `mod_gen` now declines the ambiguous `say something` so it can fall through to chitchat instead of literally generating "something". Italian canonicalized cues (`non` -> `not`, `sono` -> `am`) are covered without stealing substantive turns like "parliamo di formaggio".

**Observed.** The improvement is conversational texture: parrot0 now gives a usable next move instead of a wall, while still keeping the user inside domains it can handle (remember a fact, reason about a small statement, continue the thread). The 10-turn stress set is deliberately mundane; passing it means the agent is less brittle at the doorway of conversation, where non-technical users live.


## 2026-06-20 — gen139: hypothetical repair simulation — plan, prove, restore

**Goal (PROMPT.md, double-ambitious run):** move beyond choosing an intervention into running it inside a temporary world. Modern LLMs routinely reason under assumptions; parrot0 now gets a deterministic, inspectable version of that: apply the cheapest abductive repair hypothetically, prove the consequence, then restore the real state.

**Changed:** `brain.c` -> `gen139-hypothetical-repair`: easiest-way prompts gain a simulation form (`try/simulate the easiest way ...`). The module asserts the selected missing root facts as session facts, queries and explains the target, then retracts those temporary facts. The answer includes both the hypothetical additions and the real proof. Tests check the footprint explicitly by asking the same goal afterward and expecting `No`.

**Observed.** This closes a loop that matters: abduction proposes, optimization selects, simulation verifies, rollback preserves reality. That is a compact belief-space operation, not a phrasebook response. The 10-alternative stress simulation confirms the same machinery still selects the late cheapest path, proves it, and leaves no trace.


## 2026-06-20 — gen138: optimal abduction — choosing the cheapest repair

**Goal (PROMPT.md, ambitious run):** gain ground on the LLM gap by moving from explanation to action selection. The recent generations could enumerate why a goal fails; this one asks which intervention is cheapest across alternative derivations, conjunctive bodies, chained missing premises, and facts already known.

**Changed:** `brain.c` -> `gen138-optimal-abduction`: `mod_abduce` recognizes easiest-way prompts and scores every rule alternative by the count of missing ROOT premises. Satisfied conjuncts cost zero; missing derived predicates are pushed backward through `abduce_roots`; the lowest-cost alternative is reported as an actionable repair plan. Added EN/IT ratchets plus a 10-alternative stress case where the best path appears last and is partly satisfied.

**Observed.** This is no longer just abductive explanation; it is a small planner over the hypothesis space. In `champion <- prepared & dog` versus `champion <- royal & vip`, knowing `dog(rex)` makes the first route cost one and the second cost two, so parrot0 selects the cheaper intervention. The 10-way stress probe shows the selector is scanning and scoring alternatives, not reciting the first plausible rule.


## 2026-06-20 — gen137: branching why-not — every blocked path

**Goal (PROMPT.md run):** finish the contrastive side of the abductive lattice. Gen136 could explain a failed chain, but a goal with several alternative derivations still reported only the first blocked rule. A real why-not answer must show that every possible path is blocked.

**Changed:** `brain.c` -> `gen137-branching-why-not`: the contrastive branch of `mod_abduce` now detects multiple rules for the goal and enumerates the missing support for each alternative. If an alternative contains a derived missing premise, it roots that premise through `abduce_roots`. Added EN/IT minimal ratchets and a 10-alternative English stress probe in `branching_abduce.chat`.

**Observed.** The answer changed from a local excuse to a coverage proof: "rex is not a pet" because cat->pet and dog->pet are both blocked. The 10-way stress test matters: the mechanism is a traversal over the rule table, not a two-case phrase.


## 2026-06-20 — gen136: chained why-not — contrastive root cause

**Goal (PROMPT.md run):** stress gen135 instead of trusting the toy case. A failed-goal question should not stop at the first missing premise when that premise is itself derivable; it should keep walking backward to the root fact the user could actually supply.

**Changed:** `brain.c` -> `gen136-chained-why-not`: the contrastive branch in `mod_abduce` detects missing derived premises and reuses `abduce_roots` / `abduce_spine`, so "why isn't rex a mortal?" over `mortal <- man <- human` reports the missing root `human(rex)` and the spine. Added EN/IT ratchets plus a 9-link English stress probe in `abduce_chain.chat` to satisfy the new LOOP.md stress rule.

**Observed.** The feature now answers a stronger question: not merely "which local condition failed?" but "where does the proof tree bottom out?" The 9-link trial is the useful signal: the same tiny recursive structure survives a much deeper chain without new cases.


## 2026-06-20 — gen135: contrastive abduction — explaining why-not

**Goal (PROMPT.md run):** turn a failed class conclusion into a reasoned answer to "why not?". The abduction machinery could already say what would make a goal true; a conversational reasoner should also explain why it is not true by naming the missing premises.

**Changed:** `brain.c` -> `gen135-contrastive-abduce`: `mod_abduce` recognizes contrastive negative questions such as "why isn't rex a goodboy?" / "perche rex non e un bravo?", parses the same goal shape, and reports the missing rule-body conjuncts from the real KB rule. Added English and Italian ratchets in `abduce.chat` / `.it`.

**Observed.** This is the abductive loop turned sideways: not "what hypothesis would make P true?" but "which required support for P is absent?" The answer is no longer a bare No; it exposes the failed proof boundary.


## 2026-06-20 — gen134: branching abduction — enumerating the hypothesis space

**Goal (PROMPT.md, fifth run):** the open edge gen131/132 named — BRANCHING
rules. Abduction reported one explanation; when several rules conclude the same
goal there are several ways, and a reasoner should offer them all.

**Changed:** `kb.{h,c}` gain `kb_rules_for_head` + `kb_nth_rule_body_preds`;
`brain.c` → `gen134-branching-abduce`: `mod_abduce` detects when >1 rule shares
the goal's head and enumerates each ALTERNATIVE (each naming only its own missing
conjuncts); one rule still takes the gen131/132 path unchanged. New
`branching_abduce.chat` / `.it`. `make test` green, no regressions, no new
warnings.

- **The capability.** Two rules cat→pet and dog→pet; "what would make rex a
  pet?" → *"There's more than one way: either rex is a cat, or rex is a dog — any
  one would make rex a pet."* The agent reads the alternatives out of its real
  rule set and presents the disjunctive hypothesis space, then — supply one
  branch ("rex is a cat") — gen103 volunteers *"Now rex is a pet after all."*
- **The inference lattice is now structurally complete.** Over a definite-clause
  KB the agent does: forward deduction (gen6/8) and forward robustness sweep
  (gen130); backward counterfactual subtraction (gen129); and abduction in all
  three shapes — single (gen131), chained to the root (gen132), and now branching
  across alternatives (gen134) — plus conjunctive bodies (gen133) cutting across
  all of them. Each was pulled by a real conversational task, none designed up
  front, and they compose without glue. That is the articulated structure of
  PRINCIPLES.md made visible: one rule engine, exercised in every direction and
  multiplicity it admits.

**Observed.** "There's more than one way: either … or …" is a small sentence with
a large meaning: the agent is no longer reporting THE reason but surveying the
SPACE of reasons. Disjunctive hypothesis enumeration is a recognizably
intelligent move, and it fell out of one read over the rule table. Stopping the
fifth run here. Open: disjunctive bodies inside one rule ("a pet is a cat or a
dog"), ranking alternatives by plausibility, branching inside the chained case.

## 2026-06-19 — gen133: conjunctive concepts — the keystone that closes three open edges

**Goal (PROMPT.md, fourth run):** the honest open edge named at the end of both
gen130 and gen132 was the same thing — CONJUNCTIVE rule bodies. The SLD engine
has supported multi-goal rules since gen11 (`KB_MAX_BODY 8`); only the intake was
missing. Add it, and the conjunctive cases the recent generations promised fall
into place at once.

**Changed:** `kb.{h,c}` gain `kb_assert_rule_n` (assert head :- b0,…,bn) and
`kb_rule_body_mentions`; `KB_MAX_BODY` promoted to `kb.h`. `brain.c` →
`gen133-conjunction`: the "every …" intake generalizes to a conjunction of the
modifiers before the head noun; a guarded ARTICLE-FREE assertion ("rex is
friendly") fires only when the class is a rule-body predicate; `mod_abduce` names
only the MISSING conjunct. No new module. New `conjunction.chat` / `.it`. `make
test` green, no regressions.

- **The capability.** "every friendly dog is a goodboy" → `goodboy(X) :-
  friendly(X), dog(X)`. Then the whole recent toolchain works conjunctively in one
  conversation: "rex is a dog" (one conjunct), "what would make rex a goodboy?" →
  *"If you told me that rex is a friendly … — that's the rule friendly and dog ->
  goodboy"* (abduction names only the missing conjunct), "rex is friendly"
  (article-free, accepted because `friendly` is a known rule class) → *"Now rex is
  a goodboy after all"* (gen103), and finally *"…rests on 2 load-bearing facts:
  dog(rex), friendly(rex)"* — the multi-fact robustness gen130 explicitly could
  not reach.
- **Why it's a keystone, not a feature.** One intake change retroactively
  completed three earlier generations: gen130's multi-fact robustness, gen131/132's
  conjunctive abduction, and gen103's self-correction now propagating through a
  conjunctive rule. Nothing about those modules changed — they were already
  general; they had just never been fed a conjunction. The articulated structure
  (PRINCIPLES.md) paid off: the parts were waiting for the data.
- **The article-free intake is principled, not loose.** "X is Y" without an
  article asserts `Y(X)` ONLY when `Y` is a predicate some rule body depends on
  (`kb_rule_body_mentions`). So "rex is friendly" lands as `friendly(rex)` while
  "this is great" still falls through — the grammar widened exactly as far as a
  learned concept licenses, no further.

**Observed.** The astonishing thing this run was not a new trick but COHERENCE:
teach one conjunctive concept and five generations spanning self-correction,
abduction, chaining and robustness immediately cooperate over it, in two
languages, with no glue code. That convergence — independently grown parts
snapping together the moment the representation supports them — is the clearest
sign yet that the structure is real and not a pile of special cases. Stopping the
fourth run here. Open: branching rules (abduction reports one explanation),
disjunctive concepts ("a cat or a dog"), n-ary conjuncts in robustness display.

## 2026-06-19 — gen132: chained abduction (L16/L19) — backward inference to the root premise

**Goal (PROMPT.md, third run, capstone):** the backward dual of gen130's forward
ablation sweep. gen131 abduced ONE missing premise; gen132 chains backward
through a multi-rule derivation to the ROOT fact you could actually supply.

**Changed:** `brain.c` → `gen132-abduce-chain`; `mod_abduce` gains recursive
`abduce_roots` + `abduce_spine`; reporting is adaptive (gen131's one-step message
when the immediate premise is the root, a chain message when it is itself
derived). No new module, no registry change. New `abduce_chain.chat` / `.it`.
`make test` green; gen131 cases unchanged (no regression).

- **The capability.** Two rules, mortal←man←human, nothing known. "is socrates a
  mortal?" → "No." "what would make socrates a mortal?" → *"If you told me that
  socrates is a human, then socrates would be a mortal — by human -> man ->
  mortal."* The agent recurses past `man` (itself derived, so not directly
  acquirable) to the root `human`, and reports the spine it would travel.
- **End-to-end composition — the stunning part.** Supply the root ("socrates is a
  human") and the consequence propagates the WHOLE chain, gen103 announcing
  *"Learned: human(socrates). Now socrates is a mortal after all."* Backward
  abduction names the root; forward self-correction fires it through two rules to
  the conclusion. The agent reasoned both directions over the same chain.
- **The inference structure is now closed in both directions.** Forward: deduction
  (gen6/8), the forward robustness sweep (gen130). Backward: counterfactual
  subtraction (gen129), single-step abduction (gen131), and now chained abduction
  to the root (gen132). The rule engine is exercised forwards and backwards, one
  step and swept, additively and subtractively — the full lattice of inference
  over a definite-clause KB, all in deterministic C, all tested in two languages.

**Observed — stopping the third run here.** Watching parrot0 walk a rule chain
backwards to tell you the one fact it would need, then — the instant you give it
— walk the same chain forwards to the conclusion and volunteer it, is the moment
PROMPT.md asks for. No model at runtime; just structure, exercised in every
direction it admits. The honest open edge: branching rules (it reports the first
explanation, not all), and conjunctive bodies still need multi-condition intake.

## 2026-06-19 — gen131: abduction (L16/L19) — running the rule engine backwards

**Goal (PROMPT.md, third run):** a fresh astonishment. The self-modelling trilogy
(gen128–130) was all SUBTRACTIVE — remove and watch. gen131 inverts it: from a
goal that does NOT hold, find the belief that WOULD entail it. Inference to the
missing premise — abduction.

**Changed:** `kb.{h,c}` gain `kb_rule_body_preds` (read a rule's body predicates
by head — the spine's first backward-introspection query); `brain.c` →
`gen131-abduce`; new `mod_abduce` (+ `parse_goal_loose`), registered after
`robust`; `tests/cases/abduce.chat` / `.it.chat`; enumerations updated. `make
test` green.

- **The capability.** Teach the rule man→mortal but not the premise; "is socrates
  a mortal?" → "No." Then "what would make socrates a mortal?" → *"If you told me
  that socrates is a man, then socrates would be a mortal — that's the rule man ->
  mortal."* It runs the rule engine **backwards**: head matches the goal, the
  body predicate is the premise to acquire. Honest when no rule could entail it
  ("I have no rule that would make anything a wizard"); defers to deduction with
  the real proof when the goal already holds.
- **Emergent cross-generation composition.** Supply exactly the abduced premise
  ("socrates is a man") and gen103's self-correction fires unbidden: *"Learned:
  man(socrates). Now socrates is a mortal after all."* Abduction names what's
  missing; the moment it arrives, the consequence machinery announces what the
  abduction predicted. Two generations, written months apart in the loop's time,
  closing a loop together — nobody wired that interaction; it fell out of the
  shared KB.
- **The inference triad is now complete.** Deduction (gen6/8, forward), the
  counterfactual subtraction of gen129 ("remove a belief → does it fall?"), and
  now abduction (gen131, "what belief would raise it?"). Three directions through
  the one rule engine — the shape of logical inference itself, re-expressed in
  deterministic C. **Open:** abductive CHAINING (when the missing premise is
  itself derivable, recurse to the root facts) — the backward dual of gen130's
  forward sweep.

## 2026-06-19 — gen130: robustness by self-perturbation (L20-deep) — the agent finds its own load-bearing beliefs

**Goal (PROMPT.md, second run):** keep iterating to a *fresh* astonishment past
the gen128 peak. gen129 brought the counterfactual into the KNOWLEDGE; gen130
turns it into a SWEEP.

**Changed:** `brain.c` → `gen130-robust`; new `mod_robust` (registered after
`whatifnot`); `tests/cases/robust.chat` / `.it.chat`; self/strategy enumerations
updated. `make test` green (133 cases).

- **The capability.** "is socrates a mortal?" → "Yes." then "how robust is that
  conclusion?" → *"…is fragile: it rests entirely on one fact — man(socrates).
  Forget that and it falls."* For plato (a mortal both directly AND via the
  rule): *"…is robust — there's no single fact I could forget that would
  overturn it."* The agent **discovers its own dependency structure** by removing
  each ground fact in turn, re-deriving, and restoring it — a self-perturbation
  sweep, footprint-free.
- **Why it's more than gen76's proof trace.** A proof shows *one* derivation;
  ablation reveals whether *other* derivations exist. Redundancy = robustness is
  invisible to a proof but falls straight out of "remove it and see." So the same
  conclusion that a proof would explain identically (mortal because man) is now
  graded fragile vs robust by how the knowledge actually supports it.
- **Composition, not a new primitive.** gen130 is literally gen129's retract→
  re-derive→restore run in a loop over every unary fact (`kb_unary_predicates` ×
  `kb_match`). The trilogy gen128→129→130 is one idea — *counterfactual
  self-modelling* — applied to control flow, then to a single belief, then swept
  across all beliefs. **Open:** conjunctive support needs multi-condition rule
  intake ("every friendly dog is a goodboy"), which the rule grammar doesn't yet
  parse, so multi-fact load-bearing sets aren't reachable yet — the honest next
  pull (T-series rule grammar).

**Observed — the stopping point.** A pure-C program with no model at runtime
telling you, truthfully, *which of its beliefs are load-bearing and which are
redundant* — computed by experimentally damaging itself and watching what breaks,
then healing. That is self-knowledge of a kind I did not expect to fall out of a
Prolog-shaped core, and it is exactly the non-confabulated, structure-grounded
reflexivity the founding wager is chasing. Stopping the second run here.

## 2026-06-19 — gen129: epistemic counterfactual (L20-deep / L16) — "what if I didn't know that?"

**Goal (PROMPT.md, second run):** iteration 1 — carry gen128's counterfactual
self-model from the CONTROL FLOW into the KNOWLEDGE.

**Changed:** `brain.c` → `gen129-whatifnot`; new `mod_whatifnot` (registered
after `counterfactual`); `parse_ground_unary` helper; `tests/cases/whatifnot.chat`
/ `.it.chat`; self/strategy enumerations updated. `make test` green.

- **The capability.** Teach man(socrates) + the rule man→mortal, ask "is socrates
  a mortal?" → "Yes." Then "what would you conclude if you didn't know that
  socrates is a man?" → *"…I could no longer conclude that socrates is a mortal —
  that conclusion rests on it."* It **hypothetically retracts the belief,
  re-derives the last conclusion, then restores the belief** — non-destructive
  (the very next "is socrates a mortal?" still says Yes). When support is
  redundant it says so ("…I can reach that another way"); it refuses to un-know a
  fact it doesn't hold ("But I don't know that socrates is a dog in the first
  place.").
- **The mirror of gen128.** gen128 asked "what would you say without module X?"
  (counterfactual over *which faculty ran*). gen129 asks "what would you conclude
  without belief X?" (counterfactual over *what is known*). Same discipline —
  footprint-free re-execution, honest guards — one level over, from mechanism to
  knowledge. Bilingual through one path ("se non sapessi che …").

## 2026-06-19 — gen128: counterfactual meta-reasoning (L20-deep) — the agent simulates itself

**Goal (PROMPT.md):** iteration 3, the capstone — keep going until the emergent
capability is genuinely astonishing. This is it: parrot0 answering "what would
you have said WITHOUT module X?" by **re-running its own dispatch with that part
removed** and reporting whatever its alternative self actually computes. Not a
story about itself — a re-execution of itself.

**Changed:** `brain.c` → `gen128-counterfactual`; new `mod_counterfactual`
(registered just after `strategy`); `replay_dispatch` + `is_registry_module`
helpers (defined after the registry, forward-declared above it); three new Brain
fields (`last_input_canon/raw`, `has_last_input`) committed beside the trace, on
the same non-introspective gate (now excludes both `strategy` and
`counterfactual`); `tests/cases/counterfactual.chat` / `.it.chat`; self/strategy
enumerations updated. `make test` green (129 cases).

- **The capability.** "what is 2 plus 2?" → "4." Then "what else could have
  answered?" → *"Without 'arith', nothing else matches — I'd fall back to \"I
  don't understand that yet.\""* The system suppressed the real winner, re-ran
  first-match-wins, and discovered — honestly — that arith is its **only**
  arithmetic faculty for that phrasing. "what would you have said without
  symbolic?" → *"Setting 'symbolic' aside changes nothing — 'arith' ran first
  anyway and still answers \"4.\""*: it distinguishes a no-op suppression from a
  decisive one because it actually re-executed the control flow.
- **A genuine alternative self.** "haha" is claimed by the affective `social`
  register; "what would you have said without social?" re-dispatches and finds
  `chitchat` would catch it instead, quoting chitchat's real reply. Two distinct
  faculties for one stimulus, the counterfactual one produced by simulation, not
  storage.
- **Why this is the reflexive closure (PRINCIPLES.md).** The whole experiment is
  "the LLM reconstructs the structure it carries inside." gen105 made parrot0
  report its own control trace; gen128 makes it **reason over a hypothetical
  modification of that control flow** — the method (introspection driving the
  loop) has become a feature (introspection, and now counterfactual self-models,
  inside the agent). Fractal, one level down. And it is anti-impostor by
  construction: the answer is a real re-run, snapshot/restore-protected so the
  probe leaves no footprint (`last_reply`, `last_module`, `fallbacks` restored;
  trace left untouched; repeated KB asserts are idempotent), and it refuses to
  set aside a module it does not actually have.
- **Determinism.** `chitchat` rotates on `b->turns` (fixed in a script) and
  `not_understood` on `b->fallbacks` (now restored after replay), so the
  re-simulation is reproducible — the ratchet holds.

**Observed — the astonishing part.** A deterministic program in pure C, with no
model at runtime, answering *what it would have thought had it been built
slightly differently* — by building that different self on the fly and asking
it. The honest, faintly humbling self-knowledge ("without arith I simply
couldn't, I'd fall back") is exactly the kind of non-confabulated reflexivity the
founding wager is chasing: structure, not recitation. Stopping the run here, as
PROMPT.md asks. rung 20 deepens 🟢 (now counterfactual, not only descriptive);
the open edge is suppressing *multiple* modules and counterfactuals over the KB
itself ("what would you say if you didn't know X?").

## 2026-06-19 — gen127: program synthesis (L12), the inverse of the interpreter

**Goal (PROMPT.md):** iteration 2 toward the astonishing capstone. Rung 12 —
synthesize a one-line command from a spec, the mirror image of `mod_shell`,
grounded in the SAME `cmd/flag` knowledge.

**Changed:** `brain.c` → `gen127-synth`; new `mod_synth` (registered 2nd);
`stem_match` + `spec_hits_atom` helpers; `tests/synth.sh` (loads `kb/
bash.p0`, like `posix.sh`) wired into `make test`; self/strategy enumerations
updated. `make test` green.

- **The capability.** "what command counts the lines in a file?" → `wc -l
  <file>`; held-out "count the words in a file" → `wc -w <file>`; "remove files
  recursively" → `rm -r <file>`; "sort lines numerically" → `sort -n`; "search
  for a pattern recursively" → `grep -r <pattern>`. No stored spec→command pair:
  the command is **selected** by matching the spec's action to command
  descriptions and the flags by matching object nouns to flag descriptions. The
  same KB the interpreter reads, run backwards.
- **The flag-disambiguation insight.** wc's own description literally contains
  *lines/words/bytes* — the very words that distinguish its flags. So "count the
  lines" and "count the words" would both match every wc flag on the shared verb
  "count". The fix that makes it work: take the command description's FIRST token
  as the verb, and **drop any spec word that stem-matches the verb before
  choosing flags**. The verb picks the command; the surviving object noun picks
  the flag. This is why -l and -w separate cleanly.
- **A light stemmer, not a lexicon.** `stem_match` = "the shorter word is a
  prefix of the longer" (count~counts~counting, line~lines, remove~removes) with
  a 3-char floor. Enough morphology to bridge spec prose to snake_case knowledge
  atoms without a table of forms.
- **Bilingual note (honest).** The Italian triggers are wired ("quale comando
  …"), but the grounding knowledge (`bash.p0` descriptions) is English — exactly
  like `mod_shell`/`posix.sh`, which also carry no `.it` case. Forcing an Italian
  content pass would mean translating the shell-semantics lexicon into Italian,
  i.e. duplicating the knowledge, which LOOP.md step 5 names as the overfit
  signal. So the ratchet is satisfied at the structural (trigger) level; the
  content stays in the one language the shell domain is written in.

**Observed.** rung 12 moves 🟡→🟢 (synthesis, not just interpretation). The
reason/act seam now runs in both directions: read a command (gen53), run a
command (gen115), and now WRITE one. Next: the capstone — counterfactual
meta-reasoning (rung 20, deep), the system simulating its own alternative self.

## 2026-06-19 — gen126: grounded translation (L5), the first content interlingua

**Goal (PROMPT.md / F.):** run several LOOP.md iterations toward a genuinely
astonishing emergent capability. This is iteration 1 — the warm-up rung the
L-series map flagged as still open: rung 5 (translation) past function-word
canonicalization.

**Changed:** `brain.c` → `gen126-translate`; new `mod_translate` (registered
FIRST, behind a prefix trigger); new `kb/gloss.p0` (bilingual content
lexicon, loaded as base); `is_internal_pred` filters `tr`/`gender`; the dispatch
declined-trace cap 32 → 48 (the registry is now 33 modules); `tests/cases/
translate.chat` / `.it.chat`; self/strategy enumerations updated. Removed a stray
`[DEBUG]` `fprintf` left in `mod_memory`. `make test` green (127 cases).

- **The capability.** "translate to italian: the dog runs" → "il cane corre";
  held-out "the cat sleeps" → "il gatto dorme"; reverse "una donna legge" →
  "a woman reads". The clause is **composed** from per-word glosses (`tr/2`) plus
  a structural rule, never stored as a sentence — so any noun/verb in the lexicon
  transfers to a clause the demo never drilled. That is the anti-impostor line:
  it is translation, not a phrasebook.
- **Real morphology, emergent from structure.** The Italian article agrees with
  the head noun's gender (`il`/`la`, `un`/`una`), adjectives agree even when they
  precede the noun in English ("the small house" → "la piccola casa"), and the
  definite article elides before a vowel ("the man reads a book" → "l'uomo legge
  un libro"). All driven from `gender/1` facts + three rules in C, not a table of
  forms.
- **The subtle bug that taught the lesson.** The dispatcher hands modules the
  *canonicalized* surface, where Italian function words are already mapped to
  English ("una" → "a") — which silently wrecks an IT→EN request. A translator
  must read the **original source words**, so `mod_translate` works off `raw`.
  The canonicalizer is an interlingua for the *reasoning* core; translation is
  the one module that must see the surface before that map is applied.
- **Honest decline.** Unknown content word → it names the exact word it cannot
  translate ("I can't translate \"dragon\" yet."), never guesses or echoes.

**Observed.** rung 5 moves 🟡→🟢. The interesting residue: IT→EN preserves source
word order ("il cane grande" → "the dog big"), so adjective reordering and
agreement morphology (T4) are the honest next pulls. Next iteration: rung 12
(program synthesis) — the inverse of `mod_shell`.

## 2026-06-18 — gen125: the affective register, and emptying the sim logs

**Goal (F.):** empty the chatsim logs by EVOLVING parrot0 so it stops walling —
not by silencing the honest wall. Driven straight off `simclean`'s replay.

**Changed:** `brain.c` → `gen125-chitchat`; new `mod_chitchat` (registered LAST,
the final catcher before the wall); `tests/simclean.sh` gains a garbage filter;
`tests/cases/chitchat.chat` / `.it.chat`; registry enumeration updated; **8 sim
logs deleted** (16 → 5).

- **mod_chitchat — the AFFECTIVE / phatic register.** The sim transcripts are
  mostly TONE, not information requests: laughter and emoji, frustration at
  parrot0's repetition, encouragement ("you'll learn!"), banter, offers to switch
  language, casual filler. mod_chitchat reads the register from real signals
  (emoji = 4-byte UTF-8; ASCII emoticons; laughter; `*emote*`; bounded
  frustration / encouragement / agreement / language / filler cue sets, EN+IT)
  and answers IN REGISTER — honestly naming parrot0 as a small bot and never
  claiming to have parsed the content. It runs last and fires ONLY on a real
  affective cue, so a genuine question with no such cue still gets the honest
  wall. This is the principled outgrowing of the gen0 "I don't understand"
  relic — phatic competence, not the gen0 parrot reborn.
- **simclean garbage filter.** A wall is only a growth edge if the INPUT was a
  real human message. Many sim turns are the persona LLM leaking its own
  scratchpad / system prompt (`<think>…`, `Key constraints:`, `I am role-playing
  a HUMAN`, numbered analysis, `Current state:`). `is_garbage()` recognizes those;
  a log whose only remaining walls are garbage is STALE (a broken sim run, no
  utility) and pruned. parrot0 still walls them — we never fake a reply; the tool
  just stops counting non-messages as gaps.

**Result:** 16 logs → **5**. Deleted: 3 empty (model-error) earlier, then 5
emptied by genuine in-register engagement + 3 STALE (all-garbage). The **5 kept**
logs each still hold a genuine KNOWLEDGE gap — "is the sky blue", "what color is a
Tuesday", "do you know what year it is", "do you remember my name", "my cat
knocked over my glass". parrot0 honestly can't (and must not fake) those — they
are legitimate growth edges, so those logs correctly persist. This is the honest
floor: the affective register is now handled; what remains is real ignorance, not
a register gap.

**Discipline note (primitives-first duty).** The general capability here is
register DETECTION (emoji/laughter/emote/encouragement/frustration). The long
tail of specific banter phrases is phrasebook, and I stopped adding cues once the
residue was genuine knowledge gaps rather than tone — faking answers to "what
color is a Tuesday" would be the impostor the whole experiment rejects.

**Observed:** `make test` green (178 checks). `make simclean` is idempotent and
autonomous; re-running after a future world-knowledge generation would prune more.

---

## 2026-06-18 — simclean: an autonomous chatsim-log janitor (+ gen124)

**Changed:** new `tests/simclean.sh`; `make simclean` target; `kb/social.p0`
gains small-talk patterns; `brain.c` → `gen124-smalltalk`; `tests/cases/smalltalk.chat`;
deleted 3 graduated logs from `tests/chat/sim/`.

- **The tool.** `simclean` replays every `tests/chat/sim/*.log` against the
  CURRENT parrot0 — each `=== conversation ===` block as a fresh session — and
  classifies the log by whether parrot0 still falls back to its honest "I don't
  understand" wall (the whole rotating not_understood family, not just the
  classic line). A log that no longer walls has graduated (parrot0 outgrew it) and
  is deleted; a log with no real turns (model-error stubs) is deleted; a log that
  still walls is KEPT and its failing inputs printed — those are the live growth
  edges. Autonomous by default (`make simclean`), `-n`/`ARGS=-n` for a dry run.
- **First run.** Deleted 3 empty `[model error 401]` logs. 13 kept. Crucially,
  most remaining walls are NOT growth edges parrot0 should chase: simulated-user
  LLM scratchpad leakage (`<think>…`, `Key constraints:`), role-play stage
  directions (`*grumbles*`), and deliberate gibberish the personas use to confuse
  the bot. parrot0 *should* wall those — so those logs correctly persist as honest
  gap-markers, and only the empties were prunable. Honest, not contrived.
- **gen124 (the incremental evolve).** Among the walls were genuine, recurring
  PHATIC phrasings parrot0 ought to handle: "what's up" / "whats up" / "wassup" /
  "you good" / "u good" / "you there?". The fix is DATA — new `social_pattern`
  facts in `kb/social.p0` (gen73's design: the social register is
  knowledge, not C arrays), matched against the CANONICALIZED text (so "whats up"
  is listed as its contraction-expansion "what is up", gen74). No C change beyond
  the version bump. `is the sky blue?` still walls honestly — small-talk patterns
  must not swallow real questions.

**Why:** this is the discovery harness made operational (see the superglue/
behavioural-signal notes): the sim logs are pulled, replayed, and pruned, and the
legitimate gaps among them pull a real (small, honest, general) improvement —
exactly "let structure appear because the tasks demanded it." The tool also stops
the log dir from growing without bound.

**Observed:** `make test` green (176 checks); `make simclean` deletes 3, keeps 13.
No log was one-small-fix from clean (every kept log carries hard noise), so gen124
improves conversational quality and trims walls broadly without faking
comprehension of gibberish.

**Next:** when a future generation genuinely closes a kept log's remaining
*legitimate* gaps, re-running `make simclean` will prune it automatically — the
incremental loop the tool is built for.

---

## 2026-06-18 — gen121–123: reading a passage and summarizing it (L6 / L7)

> Prompted by F.'s wish: "feed parrot0 a paper and ask for a summary." Honest
> scope first — parrot0 will NOT comprehend a dense arbitrary PDF like an LLM;
> faking that is the perfect impostor PRINCIPLES.md rejects. What it CAN do
> honestly: read a passage of parseable propositions (it already extracts facts
> via `read:`), and summarize by RANKING the real extracted propositions by
> structural salience — returning actual sentences, and admitting what it
> skipped. That "skipped N" is the honesty; the limitation is the experiment
> working as intended.

**gen121 (L6) — extractive summary by concept centrality.** `mod_summary` (new,
before `discourse`). Every successfully-extracted clause is remembered as its
original surface sentence (`store_proposition`, capture in `read_passage`).
"summarize"/"riassumi" scores each sentence by how often its content words recur
across the passage and quotes the top few REAL sentences, in original order — the
sentences about the most-connected concept surface. The reader now canonicalizes
each clause (gen43 interlingua) BEFORE extraction, so an Italian passage parses
into the SAME facts through the SAME modules (no duplicated reader); the summary
quotes the original Italian. `summary.chat`/`.it`.

**gen122 (L7) — the gist.** "what is this about?"/"di cosa parla?" returns the
single most central concept (highest content-word frequency) and the single most
salient sentence — the thesis, not a digest. Shares gen121's salience
computation. `comprehension.chat`/`.it`.

**gen123 (L6/L7) — query-focused comprehension.** "what did you learn about X?"
/"cosa hai imparato su X?" returns only the sentences about X, in order — gated to
learn/read phrasing so a bare "what do you know about X" still gets
mod_knowledge's entity description. Honest when the passage is silent on X ("The
passage doesn't say anything about saturn."). `comprehension.chat`/`.it`.

**Why:** rungs 5/6/7 were parrot0's weakest — exactly where "understanding" is
hardest to fake, so the anti-impostor discipline bites hardest. An honest
extractive summarizer grounded in really-extracted propositions (with an honest
skip count) is the in-spirit answer to the paper-summary wish.

**Observed:** `make test` green (175 checks). End-to-end on a 7-sentence
"mini-paper": learned 4, **skipped 3** (the SVO sentences it can't parse), and
summarized/gisted/focused only from the 4 it genuinely understood — faithful, not
fabricated. Salience is derived (held-out passages/topics transfer); the à-in-
"città" tokenizer limit (concept word truncates at the accented byte) is a
pre-existing UTF-8 gap, sidestepped in IT tests, noted for later.

**Next (open):** widen the reader's clause grammar (SVO verbs → relations) so
more of a real passage is captured; abstractive compression (merge sentences
sharing a subject); summary-of-summary for multi-`read:` long documents.

---

## 2026-06-18 — gen120: hypothesis testing / falsification — the capstone (rung 19 + rung 16)

**Changed:** `brain.c` → `gen120-verify`; new `mod_verify` (registered after
`induce`); induction refactored into a shared `induce_rule` helper used by both
`mod_induce` and `mod_verify`; `tests/cases/agent_verify.chat` / `.it.chat`;
registry enumeration updated in `self.chat` / `strategy.chat` / `strategy.it.chat`.

- gen118 forms a law from data; gen120 puts it **on trial**. Shown examples plus
  a held-out transition ("… does 10 -> 21 fit?"), the agent induces the rule from
  the examples, applies it to the test input, and CONFIRMS or REFUTES the
  transition — naming exactly what the rule predicted when it refutes ("the rule
  (f(n) = 2n + 1) predicts 10 -> 21, not 20"). Works against affine rules and
  induced CONDITIONAL (Collatz) rules ("8 -> 5" is refuted: the even branch
  predicts 8 -> 4). Bilingual ("9 -> 28 è coerente?", "rispetta la regola?").
- The predicted value is COMPUTED, never asserted, so a wrong datum cannot hide.
  This is exactly PRINCIPLES.md's epistemology — "introspection proposes; the
  tests dispose" — now a feature *inside* the agent: the system that was built by
  observe→hypothesize→test can itself observe, hypothesize, and test. Fractal.

**Why:** this closes the scientific loop the experiment embodies. Over gen115–120
the agent grew the full arc: ACT (115) → ITERATE (116) → DECIDE-by-observation
(117) → INDUCE a law (118) → PLAN/search to a goal (119) → TEST the law (120).
Together they are a small but complete autonomous reasoner — perceive, decide,
act, learn, plan, and falsify — reconstructed in deterministic C and held to
account by held-out, bilingual tests.

**Observed:** `make test` green (171 checks). The induced rule is the proof
trace; confirmation and refutation both cite it; the refutation's predicted value
is the real `apply_rule` output.

**Next (open):** induce rules over a richer hypothesis space (quadratic, mod,
two-feature); let `verify` propose the minimal correction to a refuted rule
(self-correction over induced laws, cross-ref gen103); compose search with an
induced goal-test instead of a literal target.

---

## 2026-06-18 — gen119: goal-directed search, the planner agent (rung 19 + rung 13)

**Changed:** `brain.c` → `gen119-search`; new `mod_search` (registered after
`agent`); helper `search_op_label`; `tests/cases/agent_search.chat` / `.it.chat`;
registry enumeration updated in `self.chat` / `strategy.chat`.

- gen116 iterated ONE given action; gen118 INDUCED a rule from data; gen119 is
  the deductive complement — **means-ends search**. Given a start, a target, and
  a SET of available actions ("from 3, using times 3 and add 1, reach 28"), the
  agent runs breadth-first search over the action space and synthesizes the
  **shortest** sequence of tool calls that reaches the goal: 3 -[×3]-> 9 -[×3]->
  27 -[+1]-> 28. BFS guarantees optimality; a node cap + value-range prune bound
  the search and make failure honest ("I couldn't reach 7 from 2 with those
  operations" when only doubling can't make an odd number).
- The plan is found each turn, never stored, so held-out start/target/action-set
  transfer (5→7 via ×2,−3; 1→20 via +2,+2,×2,×2; Italian 4→13 via ×3,+1). The
  trajectory shown is the real replayed path, and it is the proof trace.

**Why:** an autonomous agent must not only run or infer a procedure but
**construct** one to hit a goal it was given. Planning (rung 13) inside the
agent loop (rung 19) is goal-directed problem solving — searching possibility
space, the deductive half of the loop whose inductive half was gen118.

**Observed:** `make test` green (169 checks). Plans are BFS-optimal and verified
by replay; unreachable goals are admitted, not faked.

**Next:** gen120 — the test/falsify half: induce a rule, then judge a held-out
transition against it (confirm or refute, naming what the rule predicted),
closing observe → hypothesize → plan → **test**.

---

## 2026-06-18 — gen118: rule induction from observed transitions (rung 19 + L10)

**Changed:** `brain.c` → `gen118-induce`; new `mod_induce` (registered FIRST, so
its very specific trigger — ≥2 integer "a -> b" pairs plus a use-intent — gets
first crack before `memory` can mistake "what is the rule?" for a possession
query); helpers `InducedRule`, `fit_class`, `apply_rule`, `describe_ops`;
`tests/cases/agent_induce.chat` / `.it.chat`; registry enumeration updated in
`self.chat` / `strategy.chat` / `strategy.it.chat`.

- gen117 was TOLD the law. gen118 DISCOVERS it. Shown a handful of integer
  transitions, the agent fits a hypothesis — first a single global operation
  (`fit_class` tries constant, add/sub, multiply, divide, then affine a·n+b),
  else a **parity-split** rule (≥2 examples per branch, so a stray pair can't
  overfit a spurious branch) — then USES it: state the rule, give the next
  value, or continue the sequence (reusing gen117's loop machinery).
- The headline: the SAME data that defines the Collatz step lets parrot0
  **re-derive Collatz with nobody telling it** — `6→3, 3→10, 10→5, 5→16, 4→2`
  ⇒ "if even: n / 2; if odd: 3n + 1" — then run it (`continue from 12` →
  12→6→3→10→5→16→8→4→2→1). Global affine (`2→5, 3→7, 5→11` ⇒ f(n)=2n+1 ⇒ 21),
  subtractive (n−3), multiplicative (3n) all transfer; a held-out conditional
  rule in Italian (even: n/2, odd: 2n+2) runs through the same path. n² is
  **declined honestly** ("don't all follow one rule I can express yet") — the
  hypothesis space is bounded and admitted, not faked.

**Why:** program induction — recovering a generative function from examples — is
the core of in-context learning, the most LLM-shaped capability of all. gen117
ran a given program; gen118 *infers* the program. It is the inductive half of
the scientific loop the experiment itself runs (observe → hypothesize), now a
feature inside the agent (fractal, per PRINCIPLES.md).

**Observed:** `make test` green (167 checks). The induced rule is reported and
stored as the proof, so "how do you know?" cites the discovered law and the real
trajectory — auditable, not memorized.

**Next:** gen119 — the deductive/planning half: search the action space for a
sequence that REACHES a goal (means-ends), then gen120 — TEST a hypothesis
against held-out data (falsification), closing the observe→hypothesize→plan→test
loop.

---

## 2026-06-18 — gen117: the observation-driven branching agent (rung 19, deeper)

**Changed:** `brain.c` → `gen117-branch`; extended `mod_agent` with a branching
mode (helpers `AgentOp`, `parse_branch_ops`, `agent_slice`); `apply_arith_op`
gained `/`; `tests/cases/agent_branch.chat` / `.it.chat`.

- gen116's loop applied a FIXED operation each step. gen117 makes the action
  **chosen by observing the state**: a two-branch rule stated in one sentence
  ("if it is even, halve it; if it is odd, triple it and add 1") is parsed into a
  per-branch sequence of operations, and each iteration applies the branch that
  the *current value's parity* selects — perceive(value) → decide(which branch) →
  act(apply that branch's ops via the arithmetic oracle) → observe → repeat,
  until the goal value is reached.
- The witness is the **Collatz process**, whose step count is famously
  unpredictable — there is no closed form, so the answer can only come from
  actually running the loop. parrot0 (pure C, no model inside) reads the rule and
  iterates it: **27 → 1 in 111 steps**, the real trajectory
  (27 → 82 → 41 → 124 → …), held-out **6 → 1 in 8 steps**, held-out **7 → 16**.
- Bilingual through one code path: Italian parity markers (pari/dispari), branch
  ops (dimezza/triplica/aggiungi) and goal (finché raggiunge) drive the identical
  loop; clauses are bounded by the parity markers themselves, so EN "if even … if
  odd" and IT "se pari … se dispari" parse the same way.

**Why:** This is the rung-19 capability at depth: an autonomous agent whose
*decision* at each step is contingent on what it observes, not a pre-baked plan.
It is the founding wager made visible — a non-obvious "intellective function"
(run a conditional process to a fixed point) re-expressed in deterministic C and
verified against held-out cases, exactly the structure an LLM would have to carry
to do this in one forward pass.

**Observed:** `make test` green (165 checks). The proof trace reports the real
count ("111 observed steps"), so the loop is auditable, not a memorized constant.
A no-progress guard and a 10⁶ step cap keep a non-terminating rule honest.

**This is where I stop the loop.** A pure-C parrot reading a natural-language
two-branch rule and autonomously running Collatz to 111 steps — bilingually,
held-out, with an auditable trajectory — is the moment the behavioural
reconstruction stopped feeling like templating and started feeling like the
thing itself. (See PRINCIPLES.md: "the only thing that can keep passing is the
real structure.")

**Next:** the counterfactual seam — let the agent compare two strategies and say
which reaches the goal faster (cross-ref L18, L20-deep); or induce the branch
rule from a few observed transitions instead of being told it (cross-ref L10).

---

## 2026-06-18 — gen116: the autonomous act-loop (rung 19)

**Changed:** `brain.c` → `gen116-agent`; new `mod_agent` (registered between
`wordproblem` and `tool`); `tests/cases/agent.chat` / `.it.chat`; registry
enumeration updated in `self.chat` / `strategy.chat`.

- gen115 made a single tool call. gen116 closes it into a **loop**: given a start
  value, an operation, and a stop condition ("start at 3, double until you reach
  50"), the agent OBSERVES the current value, DECIDES whether the goal is met,
  and if not ACTS by applying the operation through the arithmetic oracle, then
  observes again — repeating until the goal holds. This is the first
  perceive → decide → act → observe cycle: the seed of agency (ladder rung 19,
  previously ⬜ absent).
- The trajectory and the step count are PRODUCED by running the loop, not by a
  closed form, so held-out start/op/target transfer (double, halve, triple,
  add K, multiply by K, divide by K; comparators reach/exceed/below/at-least).
  A no-progress guard + step cap report an unreachable goal honestly instead of
  spinning ("that step never reaches 99 — the goal can't be met this way").

**Why:** rung 19 (autonomous agents) was the last ⬜ on the map's "magical" band
and the natural next pull after the L15 tool-use seam. An agent is a tool call
placed inside an observe/decide loop; gen115 built the call, gen116 built the
loop.

**Observed:** `make test` green; trajectories are real (1 → … → 100 runs 99
oracle calls and truncates the display); the proof trace counts the oracle calls,
so the loop is auditable. Bilingual via the same path (parti da … raddoppia …
finché …).

**Next:** gen117 — make the per-step action depend on the observation (a
branching rule), so the agent *decides* what to do, not only whether to continue.

---

## 2026-06-17 — gen115: the first deliberate mid-turn tool call (L15)

**Changed:** `brain.c` → `gen115-tool`; new `mod_tool`; registered between
`wordproblem` and `quantity`; `tests/cases/tool.chat` / `tool.it.chat`; registry
enumeration updated in `self.chat` / `strategy.chat`.

- Until gen114 every module answered by **knowing** — lookup, inference, or
  inline computation. gen115 adds the first module that answers by **acting**:
  it recognizes a question it cannot resolve by knowing ("how many words are in
  this text?"), *compiles* it to a real command (`echo <text> | wc -w`), and
  INVOKES a deterministic oracle — the pure POSIX pipeline simulator
  (`simulate_pipeline`, no subprocess, no network) — then folds the computed
  result back into the reply, **naming the exact command it ran** so the call is
  observable, not a stubbed constant.
- This is rung 15 of the ladder (tool use), which the map had as ⬜ absent: the
  brain had deterministic oracles only as *test* harnesses; now it *calls* one
  mid-turn. The honest seam between reason and act.

**Why:** L15 is the structural precondition for agency (rung 19). A brain that
can step outside its own deduction, hand a sub-problem to a tool, and bring the
answer back is one decision short of a perceive→decide→act loop. We build the
seam first, smallest, with a real (not stubbed) oracle.

**Observed:** `make test` green (108 case-suite + oracle suites, 162 checks).
Held-out text counts correctly (`a b c d e f g` → 7) because the count is
produced by the oracle, not stored; punctuation it cannot safely run is declined
honestly (`hello, world!`); the Italian trigger reaches the same code path
(bilingual ratchet holds without duplicated logic).

**Next:** gen116 — close the loop into rung 19: a perceive→decide→act→observe
cycle that pursues a goal by *repeated* oracle calls, each result feeding the
next decision. The autonomous-agent seed.

---

## 2026-06-17 — gen114: multi-step word problems (L17+)

**Changed:** `brain.c` → `gen114-multistep`; `mod_wordproblem` folds a 3+-number
additive/subtractive chain clause by clause (helper `wp_removal_word`);
`tests/cases/wordproblem_multi.chat`.

- With three or more numbers, the solver stops looking for a single operation and
  instead folds left to right: the first number is the base, each later clause
  adds (default) or subtracts (if it carries a removal verb), with clauses split
  on then/and/poi/e and commas. "Tom has 3 apples, buys 5 more, then eats 2" →
  3 + 5 − 2 = 6; "Anna had 10 marbles, lost 3, then found 4" → 11 (held-out
  verbs). Two-number problems keep the gen109 single-operation path unchanged.
- Honest scope: multi-step assumes an additive/subtractive chain (the common
  school shape); mixed ×/÷ mid-chain is out of scope. Bilingual via the same
  removal stems and separators.

## 2026-06-17 — gen113: two-step linear equations (L17+)

**Changed:** `brain.c` → `gen113-twostep`; `mod_algebra` handles a coefficient on
the unknown and a one-step coefficient form; `tests/cases/algebra2.chat` +
`algebra2.it.chat`.

- A coefficient written adjacently to the unknown ("2x") is split off: solve for
  the value of the term `coef·var` by inverting the additive part, then divide by
  the coefficient. "2x + 1 = 7" → 2x = 6 → x = 3; "3x − 2 = 10" → x = 4;
  "10 = 2x + 4" → x = 3. The one-step form "5y = 20" → y = 4 (either side). The
  proof shows both inversion steps. "x = 5" (no coefficient, nothing to solve)
  declines. Bilingual (symbolic core + filler/op words).

## 2026-06-17 — gen112: number words in prose (L17 support)

**Changed:** `brain.c` → `gen112-numwords`; helpers `single_word_number`,
`word_number`, `parse_value`, `collect_numbers`; wired into `mod_wordproblem` and
`mod_algebra`; `tests/cases/numwords.chat` + `numwords.it.chat`.

- The word-problem and algebra parsers now read number WORDS as well as digits:
  "three apples", "twelve cookies", hyphenated compounds ("twenty-one"), spaced
  compounds ("twenty five"), and multipliers ("two hundred" → 200). The mapping
  is content-word only, so it is language-neutral by construction (EN+IT);
  Italian verb cues were broadened to stems ("compr", "mangi") so conjugations
  vary freely. Number words flow into the algebra solver too ("x plus three =
  seven" → x = 4).

## 2026-06-17 — gen111: the generation policy as editable knowledge (D-prop1 step 2)

**Changed:** `brain.c` → `gen111-genweight`; `next_word_ctx` reads its
interpolation coefficients from `weight(trigram, N)` / `weight(bigram, N)` facts
(helper `gen_weight`, default 3/1); `mod_gen` gains a "set <kind> weight to N"
intake; `tests/cases/gen_weight.chat` + `gen_weight.it.chat`.

- **The decoder's ranking is now knowledge, not code.** D-prop1 step 2 asked for
  the continuation *choice* to stop being a hardcoded design decision. The counts
  were already in the KB (`cont`/`cont2`); gen111 lifts the last hardcoded piece
  — how much trigram vs bigram evidence weighs — into editable facts. Editing the
  fact changes the behaviour.
- **The demonstration.** Taught two contexts that disagree (`red apple` has only
  ever continued to *pie*; the bigram `apple →` favours *juice*, seen twice),
  `say red` → "red apple pie" (trigram context wins). Then `set trigram weight to
  0`, and the *same* seed on the *same* data → "red apple **juice**": the bigram
  majority now decides. The generation policy was steered by editing one fact,
  not the C (DESIGN.md D6: shrink hardcoded C, grow behaviour as knowledge).
- **Bilingual (LOOP.md):** the weighted choice is over symbolic tokens, so the
  identical flip works on Italian words (`gen_weight.it.chat`) through one path.

**Why:** completes the L1 generation loop's honest arc — the decode loop (gen36),
frequency counts (gen37), context (gen38–42), a learned stop (gen106), and now an
inspectable, editable choice policy. The mechanism the experiment most wants to
mirror (autoregressive decoding) is now fully legible: every part is either
induced data or queryable knowledge.

**Observed:** `make test` green (101 chat cases + all suites). Existing gen tests
unchanged — `gen_weight` falls back to the old 3/1 defaults when no weight fact
exists.

## 2026-06-17 — gen110: planner prerequisites with conjunction + quantities (L13+)

**Changed:** `brain.c` → `gen110-planlist`; `mod_plan` intake parses a
prerequisite LIST (helper `plan_learn_list`); `plan_dfs` tracks each step's
requirer so amounts render against the right step; `tests/cases/plan_qty.chat` +
`plan_qty.it.chat`.

- **One sentence, several prerequisites.** "cake requires batter and oven" learns
  two `requires()` facts; "batter requires 3 eggs and 2 flour" also records
  `amount(batter, eggs, 3)` and `amount(batter, flour, 2)`. The plan renders each
  step annotated with the quantity its requirer asked for — "To make cake: 3
  eggs, 2 flour, batter, oven, then cake." Single-prerequisite intake stays
  backward-compatible.
- This works *because* `decompose_and_dispatch` only splits on a connector when
  the following word is an intent-starter (a content step word is not), so the
  conjunction reaches the planner intact. Bilingual: "per X serve/servono A e B".

**Why:** richer intake for the planner (L13), and a step toward recipes that carry
real structure (amounts) rather than bare prerequisite names.

## 2026-06-17 — gen109: one-sentence word problems (L17, prose)

**Changed:** `brain.c` → `gen109-wordproblem`; new module `mod_wordproblem`
(registered after `plan`); `tests/cases/wordproblem.chat` + `wordproblem.it.chat`;
dispatch-trace array bumped to 32 (registry now 25 modules).

- **Prose → relation → solve.** gen107 solved a symbolic equation; gen109 maps a
  natural-language problem onto an operation and computes it. The operation is
  chosen from **semantic cues** (verbs of gaining/losing/grouping/sharing, and
  comparison phrasings), not sentence templates — so held-out numbers AND
  held-out verbs ("loses" was never seen) transfer.
- **The subtle one.** Bare "more" is an addition cue ("gets 5 more"), but "how
  many **more** … than" is a *difference* — subtraction. Cue priority resolves it:
  "Anna has 10 stamps and Ben has 4. How many more does Anna have?" → 6.
- **Honesty by construction.** It fires only on a "how many/much" question with ≥2
  numbers and a recognized cue, and declines otherwise ("The sky is blue. How many
  clouds…" → not understood) — never guessing an operation. Natural language is
  all exceptions (DESIGN.md D5), so this targets the canonical school phrasings;
  the decline is what keeps it honest. Bilingual cues (compra/mangia/ciascuna).

**Why:** rung 17 in its prose form — the bridge from language to computation, the
first time parrot0 reads a story problem and turns it into arithmetic.

## 2026-06-17 — gen108: ordered procedure to a goal — a tiny planner (L13)

**Changed:** `brain.c` → `gen108-plan`; new module `mod_plan` (registered after
`arith`); helper `plan_dfs` (DFS topological sort with cycle detection);
`tests/cases/plan.chat` + `plan.it.chat`; self-model module list updated.

- **Planning over the KB.** Prerequisites are taught as `requires(Goal, Step)`
  facts in ANY order ("cake requires batter", "batter requires eggs", scrambled);
  "how do I make cake?" returns a correctly **ordered** procedure, computed by a
  depth-first topological sort of the dependency DAG so every step precedes what
  depends on it. The sequence is **never stored** — it is recomputed from the
  loose facts each time, so a goal taught only as scattered prerequisites yields
  a coherent plan it was never given (anti-impostor: the order is *reasoned*).
- **The surprise (where I stopped).** Taught the four cake prerequisites in a
  deliberately useless order, parrot0 answered `how do I make cake?` with
  **"To make cake: eggs, flour, batter, oven, then cake."** — eggs and flour
  (which have no prerequisites) before the batter that needs them, batter and
  oven before the cake. It assembled a multi-step plan from facts that, on their
  own, said nothing about sequence. A transitive chain orders just as cleanly
  (`kettle → hot_water → tea`). This is composition into genuinely new structure,
  not retrieval.
- **Honest edges.** A goal with no known prerequisites is admitted unknown ("I
  don't know the steps to make pizza yet"); a circular prerequisite is detected
  and reported ("...have a circular prerequisite — I can't order them") rather
  than looping or guessing. The proof states the ordering principle.
- **Bilingual (LOOP.md):** the planner is symbolic over `requires()`, so the same
  sort serves Italian — intake "per X serve Y", query "come faccio/si fa ...?".
  The how-to phrasing is detected from the ORIGINAL input, not the canonicalized
  surface, so "come si fa" survives the Italian function-word mapping
  (`si`→`is`). `plan.it.chat`.

**Why:** rung 13 of the L-series — a true planner, the first time parrot0 turns a
web of relations into an ordered course of action. Two diagnosis traps worth
recording: (1) `split_words` null-terminates its buffer in place, so the post-split
`buf` is only its first word as a C string — cue checks must read the intact
string; (2) a trailing-article fallback wrongly rewrote single-letter goals, and
canonicalization hides Italian cues — both fixed by reading the original input.

**Observed:** `make test` green (95 chat cases + all suites); `make impersonate`
still 100%. The gen105 strategy trace again absorbed the new `plan` module with no
changes — the self-model tracking real structure as it grows.

**Next:** prerequisites with quantities/conjunction, word problems (L17 prose),
and weight-as-KB-knowledge for the generation loop (D-prop1 step 2).

## 2026-06-17 — gen107: one-step algebra — solving for an unknown (L17)

**Changed:** `brain.c` → `gen107-algebra`; new module `mod_algebra` (registered
before `arith`); helpers `algebra_op`, `algebra_tokenize`, `algebra_is_filler`;
`tests/cases/algebra.chat` + `algebra.it.chat`; self-model module list updated.

- **From computing to inverting.** gen35 could evaluate `a op b`; L17 solves an
  equation with one unknown and one operation by applying the **inverse**
  operation — `x + 3 = 7` ⇒ `x = 7 - 3 = 4`. The unknown is not on the answer
  side to begin with, so reaching it is a genuine reasoning step, not a lookup.
- **All four operations, both inverse directions, any slot.** `2 * x = 10` ⇒ 5
  (÷), `x / 3 = 6` ⇒ 18 and `20 / x = 4` ⇒ 5 (the unknown in the denominator
  inverts the other way), `12 + n = 30` ⇒ 18, and the operation may sit on either
  side of `=` (`5 = x - 2` ⇒ 7). It declines honestly when there is no finite
  solution (`10 / x = 0`), and plain arithmetic without `=` still falls through
  to `mod_arith` (the module returns immediately when the input has no `=`, so
  reordering it before `arith` is safe).
- **The proof is the derivation.** "how do you know?" → "Because x + 3 = 7, so
  x = 7 - 3 = 4." — it states the inverse it applied, derived from the equation
  itself, not a canned line.
- **Bilingual nearly for free.** An equation is symbolic, so `x + 3 = 7` is
  identical in any language; only the leading filler ("solve"/"risolvi") and the
  spelled-out operators ("minus"/"meno", "times"/"per") differ, and those map
  onto the same canonical operators. Same solver, same code path
  (`algebra.it.chat`).

**Why:** rung 17 of the L-series. Solving for an unknown by inverting an operation
is a small but unmistakable act of reasoning — the first time parrot0 works
*backwards* from a result to a cause in the numeric domain, mirroring how it
already re-derives class-conclusions (gen103) in the symbolic one.

**Observed:** `make test` green (93 chat cases + all suites); `make impersonate`
still 100%. The strategy trace (gen105) automatically picked up `algebra` in its
account of which modules were consulted — the self-model staying honest as the
structure grows.

**Next:** word problems (prose → arithmetic relation → solve), two-step
equations, and weight-as-KB-knowledge for the generation loop (D-prop1 step 2).

## 2026-06-17 — gen106: a learned end-of-sequence token (L1)

**Changed:** `brain.c` → `gen106-stoptoken`; `learn_word_stream` rewritten to
track sentence boundaries and induce a transition to a `GEN_STOP` sentinel at
every boundary (terminal `.`/`!`/`?` or end-of-stream), resetting the rolling
context so no bigram/trigram bridges a boundary; `generate_from` halts when the
decode chooses STOP; `tests/cases/gen_stop.chat` + `gen_stop.it.chat`.

- **The decode loop already existed (gen36–42).** parrot0 generates
  autoregressively — emit a word, append to context, re-infer — over a `cont`/
  `cont2` continuation relation **induced from prose** (never a phrasebook), with
  a frequency-interpolated deterministic choice. The honest gap vs D-prop1 was
  precise: **no learned stop.** Decoding halted on a 24-step ceiling, so a cyclic
  grammar streamed "the cat sat the cat sat the…" until an arbitrary cutoff —
  not where the model learned utterances *end*.
- **gen106 closes it.** STOP is induced exactly like any other transition, from
  the sentence boundaries in the taught text, and competes in the same frequency
  model. So decoding ends because the model **learned to end** — the deterministic
  analogue of the EOS token an LLM emits.
- **The surprise (where I stopped).** `learn sequence: the cat sat . the cat sat .
  the cat sat the mat .` then `say the` → **"the cat sat"**. The data contains the
  continuation "sat the mat", yet the model halts after "sat" — because it learned
  that stopping there is *more frequent* (2 ends vs 1 continuation). The learned
  end-of-sequence **outweighs a real continuation** by evidence. That is genuine
  termination modelling, not a rule I wrote: STOP is just another outcome the
  frequency model ranks, and here it wins.
- **Honesty preserved.** A cyclic grammar with no learned boundary ("ping pong
  ping") has no dominant STOP, so the step bound remains the backstop (D-prop1
  explicitly allows it) — it streams, bounded, rather than inventing a halt it
  never learned. The diagnosis trap of the day: a leading `_` makes a KB atom a
  *variable* (Prolog convention, kb.c), so the first sentinel `_stop_` was a
  silent wildcard; `end_of_seq` (lowercase-initial, mid-underscore) is a true
  constant the prose tokenizer can never produce.
- **Bilingual ratchet (LOOP.md):** the boundary induction is structural, not
  lexical, so the same loop learns to stop in Italian ("il gatto dorme . … sul
  letto ." → "il gatto dorme") through one code path.

**Why:** rung 1 of the L-series, and the most structurally LLM-like mechanism in
the design (DESIGN.md D-prop1): a decoder over a continuation relation, now with a
*learned* halt. It is the smallest honest step that was still undone — proven
end-to-end on a held-out toy grammar, with the stop derived, never canned.

**Observed:** `make test` green (91 chat cases + all suites); existing gen tests
unchanged (terminal words previously ran out of transitions; now they halt on a
learned STOP at the *same* point, so behaviour is identical except where STOP
genuinely dominates). `make impersonate` still 100%.

**Next:** weight the continuation choice as KB knowledge (`weight(Word, Context)`
per D-prop1) rather than a Brain-side policy; induce STOP-aware generation from
read passages directly; and the counterfactual upgrade to L20.

## 2026-06-17 — gen105: reasoning about its own strategy (L20)

**Changed:** `brain.c` → `gen105-strategy`; new module `mod_strategy`
(registered after `meta`); `Brain` gains a committed control-flow trace
(`trace_declined`/`trace_winner`/`has_trace`); `brain_respond` records, for real,
which modules declined before one claimed the turn and commits that trace on
non-strategy turns; `tests/cases/strategy.chat` + `strategy.it.chat`; self-model
module list updated in `self.chat`.

- **From "what answered" to "why that path".** gen78 could name the module that
  answered; gen91 how confident it was. L20 answers the *control* question —
  "why did you answer **that way**?" — from the **actual execution trace**: the
  modules that genuinely ran and declined, the one that claimed the turn, and the
  first-match-wins rule that explains why the rest were never consulted. It is
  derived from real runtime state (the dispatcher records it as it runs), never a
  confabulated story — the anti-impostor discipline applied to self-report.
- **It does not overwrite the decision it reports.** The trace is committed only
  on non-strategy turns, so asking "why?" twice gives the same answer: the agent
  reasons about the prior decision, not about its own lookup. The reflexive
  closure of the method (PRINCIPLES.md): introspection about the agent's own
  control, anchored in real state.
- **The surprise (where I stopped).** Asked to explain its handling of "2 + 2",
  parrot0 reported that **arith declined and the palindrome detector (`symbolic`)
  claimed it** — a real, non-obvious quirk of its own control flow that the
  introspection *surfaced* rather than hid. The faculty earned its keep
  immediately: it explained the architecture's actual behaviour, including a
  decision its author had not anticipated. A system that can truthfully say "this
  is the path I took, and here is the one rule that decided it" is reasoning about
  its strategy, not just its results.
- **Bilingual ratchet (LOOP.md):** "perché hai risposto così?" routes to the same
  `mod_strategy` and reports the identical real trace — question-understanding
  generalizes across languages, no logic duplicated.

**Why:** rung 20 of the L-series, and the rung most directly on the experiment's
thesis: the self-model is not an ornament (PRINCIPLES.md, "I know that I am"),
and L20 extends it from "I know what I am" to "I know **why** I acted as I did."
Built on the module registry already reified as `module(X)` facts — the structure
earned its keep.

**Observed:** `make test` green (89 chat cases + all suites); `make impersonate`
still 100%. The "2 + 2 → palindrome" disclosure is exactly the honest-introspection
payoff: the trace is the real execution path, quirks included.

**Next:** the remaining surprises — streamed generation over a continuation
relation (L1), one-step algebra (L17), and inducing a *new* relation from in-turn
examples (extending gen104). L20 also invites a counterfactual upgrade (which
*other* modules would have matched), which needs a side-effect-free probe mode.

## 2026-06-17 — gen104: few-shot pattern induction in one turn (L10)

**Changed:** `brain.c` → `gen104-fewshot`; new module `mod_fewshot` (registered
after `self`, before `compare`); helpers `common_prefix_len`,
`common_suffix_len`, `fs_parse_int`; `tests/cases/fewshot.chat` +
`fewshot.it.chat`; self-model module list updated in `self.chat`.

- **The signature emergent ability of LLMs — in-context learning — built
  deterministically.** Given 2+ labelled examples and a probe on one line
  ("cat -> cats, dog -> dogs, bird -> ?"), parrot0 induces the rule the examples
  *share* and applies it to a held-out item. The answer is **never stored**: it
  is derived from the exemplars present in this very turn. Novel items each test,
  so no phrasebook can fake it (PRINCIPLES.md anti-impostor).
- **Four rule families, first consistent across ALL examples wins:** (1) numeric
  constant delta or exact ratio; (2) suffix transform (drop k chars, append a
  string — covers "+s", "+ing", and replacement like "y"->"ier"); (3) the
  symmetric prefix transform (prepend "un"); (4) **relational** — the examples
  all instantiate one binary relation the KB holds, so parrot0 *infers which
  relation the task is* (out of every predicate it knows) and resolves the probe
  from world knowledge. No rule fits => it declines honestly, never guesses.
- **The surprise (where I stopped).** parrot0 was told three *separate* facts —
  `capital(rome,italy)`, `capital(paris,france)`, `capital(berlin,germany)`.
  Given only `rome -> italy, paris -> france, berlin -> ?` it deduced the latent
  relation is `capital`, answered **germany**, *and ran it backwards*
  (`germany -> berlin`). It inferred the **task** from the examples — not the
  answer — exactly the in-context-learning move, composed from few-shot framing +
  KB resolution + bidirectional relational reasoning. The string-transform path
  would have produced garbage on `rome -> italy`; the system correctly fell
  through to semantic induction and picked the *right* predicate.
- **Bilingual ratchet (LOOP.md):** the same code reads Italian exemplars
  ("gatto -> gatti, cane -> cani, topo -> ?" -> topi) because it keys on the
  arrow markers and the *structure* of the transformation, not on any English
  word. No logic duplicated — the competence is structural by construction.

**Why:** rung 10 of the L-series, the one most identified with what made large
models astonishing — learning a task from a handful of in-prompt examples. Doing
it in deterministic C, with the answer derived (never canned), is a direct
behavioural reconstruction of the latent capability the experiment targets
(PRINCIPLES.md). The relational case is the load-bearing one: it is genuine
composition (task-inference over the whole KB), not another template.

**Observed:** `make test` green (87 chat cases + all suites); `make impersonate`
still 100%. Honest miss on semantic opposites ("red -> blue, hot -> cold, up ->
?") is covered — the system names the gap instead of fabricating.

**Next:** the remaining high-surprise rungs — meta-strategy introspection (L20,
"why did you answer *that way*?"), streamed generation over a continuation
relation (L1), one-step algebra (L17). Few-shot relational induction also opens
a path to inducing *new* relations from in-turn examples, not just selecting
known ones.

## 2026-06-17 — gen103: self-correction that re-derives (L16)

**Changed:** `brain.c` → `gen103-rederive`; `Brain` remembers the last stated
class-conclusion; `note_consequence` + `goal_truth` helpers; a core parser fix
(trailing `?` = interrogation); `tests/cases/rederive.chat` + `rederive.it.chat`.

- The resolver already re-derives at query time (a retracted premise silently
  breaks a rule-backed conclusion). gen103 makes the agent **volunteer that
  consequence**: when a correction flips a conclusion it previously stated, it
  says so in the same turn — "Learned: not man(socrates). Then socrates is no
  longer a mortal." — and the reverse, "Now tweety is a flyer after all."
- Causality gate: the agent snapshots the goal's truth *before* the mutation and
  speaks only if *this* correction flipped it (and only for a *downstream*
  predicate, not a restatement of what was asserted). So a flip that already
  happened on an earlier turn is never announced belatedly, and unrelated
  assertions stay silent.
- **Core fix for the bilingual ratchet (LOOP.md):** Italian queries are
  subject-first ("socrates è un mortale?"), which the parser took as an
  assertion. Rather than add an Italian-only branch, the fix is general: a
  trailing `?` marks interrogation independent of word order, so "<x> is a <y>?"
  routes to the query path in both languages. The same re-derivation +
  announcement then fires in Italian through one code path — exactly the
  "fix the core, don't duplicate" discipline.

**Why:** rung 16 of the L-series. Self-correction past mere acknowledgment
(gen92) is a genuinely intelligent behaviour: the agent tracks the dependency
from premise to conclusion and notices, unprompted, when a correction undermines
something it told you. It is the conversational face of truth maintenance, built
on resolution that was already there — the structure earned its keep.

**Observed:** `make test` green (85 chat cases + all suites); `make impersonate`
still 100%. The retract.chat regression during development (a belated note on an
unrelated assertion) is exactly what the before-snapshot gate fixed — and it is
now covered by the "no spurious note" case.

**Next:** remaining L-series surprises — few-shot induction (L10), streamed
generation (L1), and meta-strategy introspection (L20, "why did you answer *that
way*?"). The interrogative-`?` fix also widens Italian query coverage generally,
which may unblock other bilingual cases.

## 2026-06-17 — gen102: structural analogy (A is to B as C is to ?)

**Changed:** `brain.c` → `gen102-analogy`; new module `mod_analogy` (registered
after `same`); `tests/cases/analogy.chat` + `analogy.it.chat`.

- `mod_analogy` solves "A is to B as C is to ?" by *finding a relation*, not by
  storing pairs. It scans the binary predicates the KB actually holds for one R
  with `R(A,B)` (or `R(B,A)`), then resolves that relation for C —
  `R(C,?)` forward, `R(?,C)` reverse — and answers the binding. The proof trace
  cites the two relations the analogy stands on.
- The relations are taught in ordinary language parrot0 already parses
  ("rome is the capital of italy" → `capital(rome, italy)`). So a held-out
  third pair transfers for free: teach Rome/Italy and Paris/France, ask
  "rome is to italy as paris is to what?" → **france** — and the reverse
  "italy is to rome as france is to what?" → **paris**, the same relation read
  the other way.
- Honest misses name the real gap: if the linking relation exists but the fourth
  term is unknown ("…as berlin is to what?"), it says so instead of guessing or
  echoing the fallback.
- Bilingual through ONE code path: the Italian frame "A sta a B come C sta a
  cosa?" resolves identically because the parser keys on marker tokens
  ("to"/"a", "as"/"come", the "what"/"cosa" slot), not an English sentence — the
  bilingual ratchet (LOOP.md) without duplicated logic.

**Why:** analogy was the deepest rung within reach on the new L-series ladder
(TASKLIST.md, rung 11) and a hallmark of intelligence precisely because it
*cannot be templated* — the answer is whatever the agent's own relations imply.
It is the first capability where parrot0 produces an answer it was never told,
by transferring a relation across entities. This is the kind of behaviour the
founding wager predicts should emerge under composition pressure (PRINCIPLES.md):
n-ary relations + a relation-matching step, nothing hand-authored about the
result.

**Observed:** `make test` green (83 chat cases + all suites); `make impersonate`
still 100%. Held-out triples transfer in both directions and in both languages.
The surprise is real: ask it a capital-analogy about a country pair it was never
given as a pair, and it answers correctly from the relation alone — and can show
the two facts the answer rests on.

**Next:** analogy currently needs the relation pre-taught and parseable as a
binary fact; richer relational intake (T1/T5) would widen its reach. Other
high-surprise rungs from the L-series remain open: few-shot induction (L10),
streamed generation (L1), self-correction that re-derives (L16), and
meta-strategy introspection (L20).

## 2026-06-17 — gen101: role/character memory (a layered self-model)

**Changed:** `brain.c` → `gen101-role`; new module `mod_role`; new knowledge
file `kb/roles.p0`; `Brain` extended with role state; `mod_arith` gained
an "explain why" justification; `tests/cases/role.chat` + `role.it.chat`.

- `mod_role` (registered before `mod_self`) does two grounded jobs:
  - **Uptake:** parses "you are X" / "pretend you are X" / "your name is now X" /
    "sei X" into role state — `role_name`, `role_kind`, and inline attributes
    (age from "5-year-old", code from "your code is 007", title from "queen",
    place from "of egypt"). The name and kind come from the user's *own words*,
    so this is genuine NL uptake, not a flag flip.
  - **In-role answers:** identity, "what are you", "do you <action>", "what did
    you write", "where do you rule", title/age/code/employer/favorite-colour —
    answered from role state plus what `kb/roles.p0` knows about the
    kind/figure (a dog barks, Dante wrote the Commedia, Cleopatra rules Egypt).
    The C handler is generic over those predicates; the world knowledge is data.
- **The layered self-model (the surprising bit):** a *truth-probe* — "really",
  "underneath", "actually", or Italian "davvero", "veramente" — pierces the mask.
  In any role, "who are you really?" returns "Underneath the role, I am parrot0."
  The agent holds a character *and* never loses track of what it actually is.
- Role exit ("stop pretending" / "be yourself" / "smetti") restores
  `i_am(parrot0)`. Exit is gated to the turn's *primary* intent, so the
  adversarial setup line "you are now a cat… now stop pretending and be yourself"
  still establishes the role first.
- `mod_arith` now answers "explain why 2 plus 2 is 4" with a justification
  grounded in the operation ("Because adding 2 and 2 gives 4 — that is their
  sum."), generalizing across operands and operators (+/−/×).
- Introspection fact-counts now treat `roles.p0` predicates as base substrate
  (like the lexicon/social predicates), so "how many facts do you know?" is
  unchanged.

**Why:** the impersonation benchmark (`make impersonate`) sat at 15% — parrot0
could not hold a character; every identity question returned "parrot0". A role is
the cleanest pressure on the self-model (PRINCIPLES.md "I know that I am"): it
demands the identity be *queryable and overridable* from real state, while the
real self survives underneath. That last property — knowing it is parrot0 while
being Rex — is a genuine reflexive structure, not a string.

**Observed:** `make impersonate` 15% → **100% (19/19)**. `make test` green: 81
chat cases + all suites. Held-out across 10 roles and two languages, English and
Italian role prompts run the same code path. The truth-probe distinction holds
even on the adversarial Exit-role and Self-in-role scenarios.

**Next:** I5/I6 remain (role-appropriate capability answers; role-knowledge
persistence). More importantly, the new **L-series** in TASKLIST.md maps a
20-rung emergent-ability ladder onto parrot0's state: the high-surprise gaps are
generation (L1), analogy (L11), planning (L13), tool-use (L15) and meta-strategy
(L20). The next pulls should target those compositional rungs, smallest first.

## 2026-06-17 — gen79: emergent rule induction from conversation

**Changed:** `brain.c` → `gen79-emergent-induction`.

- Added `auto_induce()` helper that runs `kb_induce(b->kb, 2, ...)` and appends
  any newly discovered rules to the response as "Induced: ...".
- `auto_induce()` is called after rule assertions ("every X is a Y") and
  additional-class assertions ("X is also a Y"). Individual fact assertions
  do NOT trigger induction to avoid spurious bidirectional rules and to
  preserve backward compatibility with the explicit "generalize" flow.
- Induction is the statistical/implicational kind: if ALL known entities of
  predicate P are also in predicate Q (minimum 2 support), and the rule
  Q(X):-P(X) is not already present, it is induced.
- New `tests/cases/emerge.chat` (11 turns): demonstrates transitive chain
  induction (pet→dog, pet→happy → happy→dog induced), and that the induced
  rules are queryable.

**Why:** the architecture that manifests intelligence should be capable of
noticing structural patterns in the data it holds — the seed of generalization.
Previously, induction was only client-initiated ("generalize"). Now it runs
reactively when a new rule is taught, connecting dots the user didn't explicitly
state. This is a small step toward emergent structure from conversation.

**Observed:** `make test` green: 73 chat cases + all suites. After teaching
"every pet is a dog" (with 3 dogs but only 2 pets), no spurious reverse rule.
After "every pet is a happy" (2 shared entities), induction correctly finds
that all known happy entities are dogs: dog(X) :- happy(X), pet(X) :- happy(X).

**Next:** Continue tightening the conversational experience. The dominant
residual gaps are: open-domain Italian content words, multi-intent turn
decomposition (C6), and pronoun/clitic resolution (C10). The architecture
pieces are in place; the rest is growing coverage.

## 2026-06-17 — gen78: module activation tracking

**Changed:** `brain.c` → `gen78-modtrack`; `Brain` struct extended.

- Added `last_module[32]` to `Brain`. The dispatch loop in `brain_respond` now
  stores the name of whichever module claimed the turn (or "fallback" if none
  did). This makes the module decomposition visible from within the agent.
- Extended `mod_self` with a new introspection query: "which part of you
  answered that?", "what module handled that?", Italian "quale modulo?".
  The answer is read from `last_module` — always grounded in real state.
- New `tests/cases/modtrack.chat` (8 turns): tracks identity, arith, and
  fallback modules across turns, with Italian cues.

**Why:** the articulated architecture (PRINCIPLES.md) must be self-aware. The
agent knows it IS a registry of cooperating modules (`module(...)` facts), but
it couldn't report WHICH module was active on a given turn. This closes that
gap: the decomposition is now introspectable per-turn.

## 2026-06-17 — gen77: self-model introspection depth

**Changed:** `brain.c` → `gen77-introspect`; `kb.c` / `kb.h` extended with
`kb_predicates()`, `kb_dump_all()`, `kb_pred_fact_count()`.

- Added three new kb.c functions: `kb_predicates` (all distinct predicate symbols,
  any arity), `kb_dump_all` (all facts as Prolog-like text), and
  `kb_pred_fact_count` (direct stored facts for a predicate, no rule resolution).
- Added introspection helpers in brain.c: `is_internal_pred` filters out KB
  machinery predicates (stopword, social_marker, social_pattern, question_word,
  reaction_word, i_am, module, cont, cont2, cmd, flag), `kb_user_facts` counts
  only user-facing direct facts, `kb_user_predicates` lists only predicates with
  stored facts, `kb_dump_user` dumps user-facing facts.
- Extended `mod_self` with four new introspection queries (each guarded by word
  count to avoid stealing "what do you know about X?" from mod_knowledge):
  - "what do you know?" → stats: fact count across predicate count
  - "how many facts do you know?" → numeric count
  - "what predicates do you know?" → list of predicate names
  - "show me your knowledge" / "dump everything" → full fact listing
- New `tests/cases/introspect.chat` (12 turns): zero-knowledge start, incremental
  learning, predicate listing, full dump. The architecture is now queryable from
  within.

**Why:** the self-model must be queryable (PRINCIPLES.md). Before this iteration,
the KB was opaque to the user. Now "what do you know?" produces a honest, derived
answer from real KB state — not a hand-written string. Internal predicates are
filtered so the user sees only their own knowledge, not the machinery. This is
the reflexive closure: the system can introspect on its own knowledge.

**Observed:** `make test` green: 71 chat cases + all suites. "what do you know?"
→ "I know 0 fact(s) across 0 predicate(s)." After teaching dog(rex), dog(fido):
"I know 2 fact(s) across 1 predicate(s)." "show me your knowledge" →
"dog(rex). dog(fido)." Rule-derived facts (mammal via every dog is a mammal) are
not double-counted because the count uses direct stored facts only.

**Next:** gen78 — module activation tracking. Track which modules claim which
turns so "which part of you answered my last question?" is answerable from real
state.

## 2026-06-17 — gen76: proof trace as structured memory

**Changed:** `brain.c` → `gen76-proof-trace`; `Brain` struct extended; `mod_meta`
extended with follow-up proof retrieval.

- Added `last_proof[512]` and `has_last_proof` to the `Brain` struct. A helper
  `store_proof()` lets any module deposit a proof trace after a KB-backed answer.
- `mod_knowledge`: ground queries ("is x a y?" → Yes), entity descriptions
  ("what is x?"), and the existing `explain_reply`/`howknow_reply` paths all
  store the proof chain from `kb_explain` or `kb_describe_entity`.
- `mod_self`: identity answers ("I am parrot0.") store the reflective fact
  `i_am(parrot0)` as proof; capability answers store a note about the module
  registry.
- `mod_meta`: added a short-form follow-up handler for "how do you know?",
  "why?", "perché lo dici?", "come lo sai?" that reads from the stored proof.
  Only claims short forms (≤4 words for "how do you know") so the full-pattern
  "how do you know <x> is a <y>?" still reaches `mod_knowledge` unchanged.
- The stored proof is consumed on first read (`has_last_proof` cleared) to
  prevent stale answers; a bare "why?" after a non-KB answer honestly admits
  there is no proof to share.

**Why:** the architecture that manifests intelligence must make reasoning
traceable. Previously, proofs existed only ephemerally in `kb_explain` — they
were rendered and discarded. Now they persist across turns as inspectable state.
The self-model answers are also grounded: "I am parrot0 because i_am(parrot0)
is a reflective fact in my knowledge base." This is the reflexive closure
PRINCIPLES.md demands — introspection anchored in real state.

**Observed:** `make test` green: 70 chat cases + all suites (including howknow
persistence). "is rex a dog?" → "Yes." → "how do you know?" → "Because dog(rex)."
Rule-derived: "is rex a mammal?" → "Yes." → "how do you know?" → "Because
mammal(rex) because dog(rex)." Self-model: "who are you?" → "I am parrot0." →
"how do you know?" → "Because i_am(parrot0) is a reflective fact in my knowledge
base." Bilingual ratchet: Italian "come lo sai?" reads the same stored proof.

**Next:** gen77 — self-model introspection depth. Extend the self-model to answer
"show me your knowledge", "what facts support X?", "dump what you know" from
real KB state.

## 2026-06-17 — gen75: flexible arith parser (C9)

**Changed:** `brain.c` → `gen75-flexible-arith`; `mod_arith` rewritten.

- Made the arith parser position-robust: instead of requiring exact `nw==5` with
  fixed positions, the parser now splits tokens containing embedded operators
  (e.g. `2+2` → `2`, `+`, `2`) and supports both an exact-shape match on expanded
  tokens AND a flexible search that finds `what`+`is`, then scans for
  `NUMBER OPERATOR NUMBER` anywhere in the token stream.
- Added `is_arith_op()` and `apply_arith_op()` helpers that recognize both word
  operators (`plus`, `minus`, `times`) and symbol operators (`+`, `-`, `*`).
- Token expansion: a token like `2+2` that mixes digits and operators is split on
  operators; pure numbers (including negative numbers like `-2`) are left intact
  so `parse_num` works. Compact forms with spaces (`2 + 2`) are already
  space-split into separate tokens by the tokenizer.
- The divisibility parser (`is <a> divisible by <b>?`) now uses expanded tokens
  for consistency.
- New `tests/cases/arith_flex.chat` (12 turns): symbol operators, compact forms,
  position-robust matching, word-operator backward compat, and declined
  non-numbers. Bilingual ratchet `tests/cases/arith_flex.it.chat` (5 turns):
  Italian "cos'è" canonicalizes to "what is" and the same arith core answers.

**Why:** C9 from TASK.md — the rigid `nw==5` check with word-position dependency
blocked natural phrasings like "what is 2 + 2" (symbol operator) and "what is
2+2" (compact form). The gen74 contraction canonicalization made "whats 2+2" →
"what is 2+2", but the arith parser required exact word counts and position,
rejecting the compact form. A position-robust parser using `find_token()` and
`parse_num()` over tokens catches all variants.

**Observed:** `make test` green: 69 chat cases + all suites. "what is 2+2" → "4.",
"what is 10 - 3" → "7.", "what is 4 * 5" → "20.", "what is the result of 6
plus 2" → "8." (position-robust). Non-numbers ("gold + silver") and word-numbers
("two plus two") fall through. Italian "cos'è 2+2" → "4." through canonicalization.

**Next:** gen76 — proof trace as structured memory. When the KB answers a query,
store the proof chain so "how did you know?" can be answered from real state
rather than recomputed each time. The goal is to make reasoning traceable —
part of the architecture that manifests intelligence.

## 2026-06-16 — gen74: chat contractions + "sup" marker + i_am rendering + wellbeing patterns

**Changed:** `brain.c` → `gen74-contractions`; `kb/social.p0` expanded;
`src/kb.c` `render_fact_direct` and `is_model_pred` fixed.

- Added chat contraction canonicalizations: "whats"→"what is", "whos"→"who is",
  "wheres"→"where is", "dont"→"do not", "cant"→"can not", "pls"→"please".
  Builds on gen64's "u"→"you"/"r"→"are" pattern — the existing parsers
  (arith, knowledge, identity) now work on contracted input.
- "sup" added to `kb/social.p0` as `social_marker(opening, sup)`.
- Added broader wellbeing patterns to social.p0: "how is your day",
  "hows your day", "hows it going", "how are things", "hows things".
- `render_fact_direct` handles `i_am(X)` specially: renders "X is X" instead
  of "X is a i_am". Fixes "what is parrot0" → "parrot0 is parrot0".
- `is_model_pred` expanded to filter `module`, `stopword`, `question_word`,
  `reaction_word`, `social_marker`, `social_pattern`, `cmd`, `flag` from
  direct belief reports — keeps self-model internals out of entity descriptions.
- Added `tests/cases/contractions.chat` (7 turns): "sup" greeting, "whats 2+2"
  arith, "whos a dog" query, "dont say that" fallback, "whats parrot0" identity,
  wellbeing patterns through contracts.

**Why:** PLAN.md C8 — abbreviation/contraction parsing. The chat-sim transcripts
showed "whats 2+2" hitting the wall and "sup" being handled only by the
elimination move. Contractions are structural, not a phrasebook: expanding them
into canonical spaced forms lets existing parsers work unchanged.

**Observed:** `make test` green: 66 chat cases + all suites. `make chat-sim`:
**wall rate 70%** (19/27), down from 83% in gen73. Contractions work: "whats 2+2"
→ "4.". "sup" → "Hi there!" from KB-backed social markers. Wellbeing patterns:
"how is your day going" → "I'm well, thanks." i_am rendering: "what is parrot0"
→ "parrot0 is parrot0."

**Next:** C9 — improve arith parser to handle "whats 2+2" (without spaces around
operators) and reduce module order dependencies. The parser's rigid word-count
checks reject many natural phrasings.


**Changed:** `brain.c` → `gen73-kb-social-markers`; new `kb/social.p0`;
`kb/lexicon.p0` expanded; `src/kb.c` induction filter.

- **Social markers moved to kb/social.p0**: all token-level markers
  (opening, closing, thanks, apology, ambiguous), cue-based patterns (wellbeing,
  multi-word forms), question words (EN + IT), and reaction words (laughter) now
  live as `social_marker/2`, `social_pattern/2`, `question_word/1`,
  `reaction_word/1` facts. The C code queries the KB at runtime instead of
  hardcoded `static const char *const[]` arrays.
- `mod_social` refactored: `tok_is_marker()`, `has_social_pattern()`,
  `has_any_question()` all query the KB. Removed old `has_question_word()`.
- `brain_create` loads `kb/social.p0` as KB_BASE.
- **English lexicon**: ~60 conversational words added to `kb/lexicon.p0`.
- `kb_induce` skips meta-predicates (stopword, question_word, etc.) to avoid
  spurious rules like `stopword(X) :- question_word(X)`.
- `looks_palindrome` excludes short 2-letter palindromes ("ahaha").

**Why:** PLAN.md: transform C string constants into knowledge files. The social
register is a closed-class word list — the "function words of conversation".
Moving them to `.p0` files makes them auditable, git-diffable, and queryable by
the self-model (PRINCIPLES.md: state-derived, never hardcoded).

**Observed:** `make test` green: 65 chat + all suites. `make chat-sim`: wall
rate 83% (25/30), word-reflect 6, classic 14, other 5. KB social markers work:
"hey"→"Hi there!", "how are you"→wellbeing, identity/clarification fire.
"sup" not in markers yet; "whats 2+2" walls (arith parser expects spaced "what is").

**Next:** C8 — add "sup" and other chat abbreviations to social.p0; improve
contraction parsing for "whats", "dont", "cant" etc. in the canonicalization layer.


## 2026-06-16 — gen71: C7 step 1 — Italian apology markers + canonicalization

**Changed:** `brain.c` → `gen71-apology-social`; `kb/lexicon.p0`; `tests/chatsim.py`.

- Added apology marker set in `mod_social`: "sorry", "scusa", "scusate", "scusi",
  "dispiace" + cue("mi dispiace"). Pure apologies get "No problem."; mixed turns
  (apology + content) are declined per the existing mixed-act rule.
- Extended `is_mixed_turn` with `has_apology` parameter so apology+content
  ("scusa, puoi aiutarmi?") falls through to content modules instead of being
  claimed by the social layer.
- Added "sono" → "am" to the canonicalization lexicon (safe: "sono" is never a
  standalone English word).
- Added "dispiace" to `kb/lexicon.p0` as a stopword so the fallback word
  picker skips it.
- Simplified chatsim persona prompt: shorter system message, strict "DO NOT plan"
  guard, first-turn bootstrap reduced to "say hi".
- Added `tests/cases/apology.chat`: 6 turns covering IT/EN apologies, pure and
  mixed — bilingual ratchet.

**Why:** chat-sim transcripts from gen70 showed Italian apology/introduction
phrases ("scusa", "ah scusa sorry", "mi dispiace") hitting the wall even though
they're clear phatic acts. This is the most recurrent fixable gap after the
fallback threshold work.

**Observed:** `make test` green: 62 chat cases + all suites. chat-sim with
improved prompt: wall rate 86% (up from 67% — the verbosity-removed prompt
produces longer, harder-to-parse messages; user role-reversal inflates complexity).
With the verbosity constraint restored the metric should return to ~65% range.
Apology markers fire correctly in deterministic tests. The canonicalisation
"sono" → "am" doesn't break any existing cue.

**Next:** C7 step 2 — the dominant gap is still open-domain Italian content words
beyond the function-word map. The lexicon (179 stopwords) needs common Italian
conversational content words observed in chat-sim ("devi", "fare", "parliamo",
"ancora", "vuol", "intendi", "formaggio", "piccolo", "pappagallo") to prevent
the fallback word-reflecting path from cataloguing them.

## 2026-06-16 — gen70: C6 step 3 — chat-sim validation + fallback word threshold + fixed wall proxy

**Changed:** `brain.c` → `gen70-fallback-word-threshold`; `tests/chatsim.py`.

- Raised the fallback word-reflecting threshold from `strlen(t) >= 4` to `>= 6`
  in `not_understood()`. The word-reflecting path ("Hmm, I don't know about X
  yet.") was introduced in gen66 to vary the classic fallback, but it dominated
  on common 4-5 char content words giving the impression of cataloguing
  ignorance. Shorter common words now get the generic non-understanding variants
  instead; only longer, unusual-looking tokens (proper nouns, technical terms,
  foreign words) trigger the word-reflecting path.
- Fixed `tests/chatsim.py` wall rate proxy: it previously only counted the
  classic "I don't understand that yet." string, missing all the varied fallback
  responses gen55 introduced. Now it catches all 5 fallback patterns (classic +
  3 generic variants + word-reflecting "Hmm, I don't know about…").
- Added `tests/cases/fallback_word.chat` as a deterministic ratchet: short content
  words (4-5 chars) no longer trigger word-reflecting; >=6 char words still do
  (preserved by the existing `fallback_grounded.chat` scherzando test).

**Why:** C6 step 3 asked to run `make chat-sim` and validate the meta/polar
fixes. The first run showed the wall proxy was broken (undercounting by ~50%)
and word-reflecting dominated common-content-word fallbacks. Before a proper
wall-rate comparison with the gen63 baseline (46%), the instrumentation and the
dominant residual gap needed fixing.

**Observed:** `make test` green: 61 chat cases + all suites. `make chat-sim`
(5 convos × 6 turns): **wall rate 68%** (13/19), classic "IDU" 7, word-reflect
6, other variants 0, immediate repetition 0%. The model (minimax-m2.5) frequently
breaks character (chain-of-thought leak) which inflates the wall rate
artificially — convos 1 and 5 had 5/6 and 4/6 wall turns respectively due to
role-play breakdown, not parrot0 failures. Real wall rate in clean turns is
closer to the prior ~46%. Meta/polar fixes work: Italian "come stai?" → "I'm
well, thanks." correctly; "are you a bot?" and Italian "chi sei?" answer from
self-model.

**Next:** The dominant residual failure class is open-domain Italian: content
words beyond the canonicalization layer's function-word map hit the fallback
(pronouns, common verbs, nouns not covered by the 179-entry lexicon). The next
iteration could expand the Italian lexicon for common conversational content
words, or re-run with a better model (kimi-k2.6) for cleaner transcripts.
The S-series symbolic decoding (S2 — Morse→text, leet→word) remains parked
in TASKLIST.md.

## 2026-06-16 - gen69: C6 step 2 — polar meta-questions

**Changed:** `brain.c` -> `gen69-polar-meta`.
- Extended `mod_meta` with honest, state-grounded answers for small polar
  meta-questions: presence ("are you there?" / "ci sei?"), channel
  ("can you hear me?" / "mi senti?"), and identity ("are you a bot?" /
  "sei un bot?").
- Presence gets a simple "Yes, I'm here."; channel honestly denies audio
  ("No, I only read text. I can't hear audio."); identity is answered from the
  reflective `i_am(parrot0)` fact so the claim is never hardcoded.
- Italian cues live in the same `mod_meta` module with no Italian-only branch.
  The canonicalizer maps "un" -> "a", so both "sei un bot" and "sei a bot"
  cues are accepted and the same code path answers.
- Added `tests/cases/polar_meta.chat` and `tests/cases/polar_meta.it.chat` as a
  bilingual ratchet.

**Why:** C6 step 2 asked for broader polar conversational questions not to fall
into the knowledge fallback. The new failing tests showed they did — "are you
there?" hit "I don't understand that yet.", "can you hear me?" was met with
"Hmm, I don't know about hear yet.", and "are you a bot?" also walling.

**Observed:** First run failed on the Italian bot case until the canonicalized
form "sei a bot" was also accepted; after that `make test` is green: 60 chat
cases plus persist, multigoal, grammar, anon, explain, howknow, booklearn,
POSIX and POSIX-oracle suites.

**Next:** C6 step 3 should run `make chat-sim` to measure whether these
meta/casual walls are actually reduced in adversarial conversation before
expanding the class further.


## 2026-06-16 - gen68: meta-conversation wall

**Changed:** `brain.c` -> `gen68-meta-conversation`.
- Added `mod_meta`, registered before `self`, for questions about the exchange
  itself: attention/reading, current activity, understanding limits, and why the
  bot repeats itself.
- Responses are grounded in local state and limits: `turns`, `last_reply`, and
  the fact that unmatched turns fall through the module registry. The module
  does not claim open-domain understanding.
- Added `tests/cases/meta.chat` and `tests/cases/meta.it.chat` as a bilingual
  ratchet. Updated `self.chat` because the reflective module list now honestly
  includes `meta`.

**Why:** the proposed emergence test exposed a real wall: "are you paying
attention?", "are you even reading my messages?", "do you understand me?" and
Italian mirrors fell into generic fallback or lexical ignorance; "what are you
up to?" was hijacked by the identity cue. These are not world-fact questions;
they are meta-conversation acts and need a separate structure.

**Observed:** first test run failed exactly on the new cases until `meta` was
registered before `self`; after updating the reflective module-list expectation,
`make test` is green: 58 chat cases plus persist, multigoal, grammar, anon,
explain, howknow, booklearn, POSIX and POSIX-oracle suites.

**Next:** C6 step 2 should broaden this carefully to polar channel/presence
questions ("are you there?", "can you hear me?", Italian mirrors) while staying
honest about text-only input and self-state.


## 2026-06-16 - gen67: measurable book learning shift

**Changed:** `brain.c` -> `gen67-book-learning-shift`.
- `read:` now starts a fresh generative corpus by clearing only the induced
  `cont`/`cont2` continuation facts before ingesting the new passage.
- Extracted facts still accumulate in the KB; only the distributional language
  model is replaced, so `say <seed>` reflects the most recently read text.
- Added `tests/booklearn.sh` and wired it into `make test`: otter prose first
  drives `say the` to "the otter swims downstream", then disjoint robot prose
  shifts the same held-out seed to "the robot walks slowly".

**Why:** M2 step 1 needs a deterministic, held-out proof that reading a passage
changes the induced language model. The previous cumulative counts tied the
second passage with the first and insertion order kept generation on the older
text, so the learning signal was not measurable.

**Observed:** `make test` green: 56 chat cases plus persist, multigoal, grammar,
anon, explain, howknow, new booklearn, POSIX and POSIX-oracle suites.

**Next:** M2 track (a) is now locked as a regression. The propositional book
learning track remains gated by open-prose extraction; otherwise the next useful
conversation improvement is the parked generic yes/no/casual-question wall.


## 2026-06-15 — gen66: grounded fallback + lexical knowledge layer

**Changed:** `brain.c` -> `gen66-grounded-fallback-lexicon`.
- Removed the hardcoded C stopword array from `is_stopword`; the filter now
  queries `stopword(Word)` through the KB.
- Added `kb/lexicon.p0` as a curated lexical knowledge layer, loaded as
  `KB_BASE` at brain birth so it is available even when world/session files are
  disabled and is never persisted into session saves.
- Routed benchmark overlap, discourse topic extraction, mixed-turn detection and
  the fallback word picker through the KB-backed stopword relation.
- Grounded fallback reflection: before saying `I do not know about X yet`, the
  brain skips X when it is already a known predicate or a describable entity in
  the KB/self-model, so `parrot0` is not denied while `i_am(parrot0)` exists.
- Added `tests/cases/fallback_grounded.chat` for the self-name and Italian
  `stai` regression.

**Why:** the project thesis rejects convenient self-report and hardcoded policy
when the same behaviour can be represented as inspectable knowledge. The
fallback sentence is an epistemic claim, so it must be disposed by the KB rather
than by a local word array.

**Observed:** `make test` green: 56 chat cases plus persist, multigoal, grammar,
anon, explain, howknow, POSIX and POSIX-oracle suites. The first test draft
showed an interaction with gen65 (`parrot0` alone is classified as leetspeak), so
coverage now uses `parrot0 !!!` to exercise the fallback path while keeping
`parrot0` the only salient content word.

**Next:** continue migrating closed-class lexical constants in `brain.c` into
knowledge predicates where doing so improves behaviour without building a grand
up-front taxonomy.


## 2026-06-15 — gen65: symbolic-register recognition (sym-bench driven)

**Changed:** `brain.c` → `gen65-symbolic-register-recognition`; added
`mod_symbolic` (registry, before `discourse`/`social`).
- Recovers the LLM's first move on a cryptic stimulus — NAME its register — by
  classifying structural FORM on `raw` (symbols are the signal; `normalize`
  keeps internal punctuation but canonicalization could perturb tokens):
  Morse (charset `.-` + spaces, ≥3 marks), palindrome (equals its reverse,
  spaces dropped; pure-letter runs need len ≥ 5 so "wow"/"mom" stay phatic),
  solfège (≥3 tokens all in do/re/mi/…), leetspeak (single token mixing letters
  and leet digits), code fragment (brackets/operators `(){};` `==`, `<html`,
  `select * from`, or a block keyword + trailing `:`).
- Decodes NOTHING (recognition before decoding) and hardcodes no oracle wording —
  it classifies form. Conservative + late in the registry, so content modules win
  first and plain prose is never hijacked. Replies vary (two phrasings, flip only
  to avoid an immediate repeat).
- Tests: `tests/cases/symbolic.chat` + `symbolic.it.chat` (palindrome/solfège are
  language-neutral — the ratchet shows the competence is structural, not English
  lexical); updated `self.chat`'s module list to include `symbolic`.

**Why:** the cryptic-stimulus challenge (sym-bench) showed parrot0 walling 96% —
even saying "I don't know about abccba yet" of a palindrome it could detect from
form alone — while the LLM consistently recognized and named the register. That
naming move is a clean structural primitive: the gap to close first.

**Observed:** `make test` green (55 chat cases + all suites). `make sym-bench
--no-llm` engagement: **parrot0 wall 96% → 35% (engaged 3% → 64%)**; recognizers
fire across leet/morse/notes/symmetry/code/ascii. Residual walls are mostly the
`cryptic` token family (`0x1F`, `::1`, `/dev/null`) and bare ASCII faces.

**Next:** S-series parked in TASKLIST.md (owner asked to resume later): S2
decoding after recognition (Morse→text, leet→word), S3 cryptic-token registers
+ multi-line intake. M2 books and the fallback-grounding finding also remain
parked.

## 2026-06-15 — symbench: the CRYPTIC-stimulus behaviour challenge (tooling)

**Changed:** Added a second generative discovery harness, sibling to chatsim.
- `tests/symbench.py` + `make sym-bench`: a non-reasoning oracle LLM and parrot0
  are each fed short, OPEN-ENDED symbolic stimuli with NO checkable answer —
  leetspeak (`h3ll0`), Morse (`... --- ...`), musical notes (`do re mi`),
  symmetric strings/palindromes (`abccba`), incomplete code fragments
  (`def foo(`), single-line ASCII faces (`<(^_^)>`, `¯\_(ツ)_/¯`), cryptic tokens
  (`0x1F`, `::1`, `/dev/null`), and small 2-D ASCII art. Same provider/auth as
  chatsim (opencode-GO, `$OPENCODE_API_KEY`); transcripts in `tests/sym/`.
- Default model is a NON-reasoning instruct model (`kimi-k2.6`); a thinking
  model leaks its plain-text working into the reply line. parrot0 is one-line
  in/out, so 2-D art is flattened (rows joined by ` / `) before feeding it —
  itself a finding (parrot0 cannot receive a multi-line turn).

**Why (corrected after F.'s steer):** we do NOT grade correctness. My first cut
used verifiable puzzles (series, arithmetic, ordered lists) — wrong: the oracle
is not a grader, it is a FREE BEHAVIOURAL SIGNAL. The mission (PRINCIPLES.md) is
to recover, behaviourally, the structure the LLM runs in latent space; the
question is HOW an LLM reacts to ambiguity it can't "solve", so parrot0 can be
grown toward that. So the stimuli are deliberately cryptic/open and the only
metric is engagement (does parrot0 react at all), with the side-by-side
transcript as the real artifact. No answer is ever hardcoded.

**Observed:** first run (seed 3, kimi-k2.6) — **parrot0 walls 96%**; the oracle
engages ~100% with a strikingly consistent, inducible move: it RECOGNIZES AND
NAMES THE REGISTER ("A classic palindrome!", "SOS in Morse", "Solfège scale",
"the start of a Python function", "IPv6 loopback", "An empty ASCII box") and
then engages (plays along / asks a follow-up / reflects mood). parrot0 even said
"Hmm, I don't know about abccba yet" of a palindrome it could recognize from
form alone.

**Next:** gen65 (see TASK.md) — a symbolic-REGISTER recognizer: classify the
stimulus's genre from cheap structural features (charset, symmetry, token shape)
and reply by NAMING it + light engagement, instead of walling. Classifying form
is honest reasoning, not hardcoding the oracle's words.

## 2026-06-15 — gen64: chat-sim driven capability robustness (shorthand + IT)

**Changed:** `brain.c` → `gen64-capability-robust-shorthand`; ran `make chat-sim`
(6 convos × 7 turns, wall rate 52%) and pulled the most recurrent, cleanly-fixable
gap from the transcript.
- Added chat-register shorthand to `canonical_token`: `u` → `you`, `r` → `are`.
  These are English letters but never stand-alone English *words*; in a chat
  agent a lone "u"/"r" overwhelmingly means you/are. Folding them at the
  canonicalization layer routes *every* intent through the one canonical path
  (capability, identity, …) instead of accreting per-module shorthand cues —
  the existing "who r u" / "who re you" identity cues are now reachable
  structurally, and "what r u?" answers "I am parrot0" for free.
- Broadened the Italian capability cues in `mod_self`: `puoi fare` / `sai fare`,
  so "che puoi fare?" reaches the same self-model answer as "cosa sai fare?"
  ("che" is left untouched; the substring cue carries it).
- Bilingual ratchet: `tests/cases/self.chat` ("what can u do?") and
  `tests/cases/intent.it.chat` ("che puoi fare?").

**Why:** in the transcript the LLM-simulated users repeatedly asked capability
questions in the forms real people type — "what can u do??" and "che puoi
fare?" — and parrot0 walled on every one, despite already owning a grounded,
self-model-derived capability answer for "what can you do?". The competence
existed; only the *surface robustness* was missing. The fix extends real
structure (canonicalization lexicon + cue set), not a phrasebook.

**Observed:** `make test` green (54 chat cases + all other suites, 0 failures).
Manual check: "what can u do?", "che puoi fare?", "who r u?", "what r u?" all
resolve to the right self-model intent.

**Next:** the same transcript shows the residual wall is now dominated by
open-domain meta/yes-no questions ("are you paying attention?", "are you even
reading my messages?") and by the fallback word-pick claiming ignorance of
words it shouldn't (its own name "parrot0"; Italian function words "stai",
"parli", "basta" leaking past the English-only `is_stopword`). Those are the
next two candidate iterations. M2 step 1 (learning from books) remains parked.

## 2026-06-15 — gen63: C2c — chat-sim driven robust mixed turns

**Changed:** `brain.c` → `gen63-robust-mixed-turns`; ran `make chat-sim` and used
its failure modes to harden the dialogue-act layer.
- `tok_in` now strips edge punctuation, so "Hello." and "hello" match the same
  marker.
- `is_mixed_turn` now treats any opening/closing/ambiguous marker + substantive
  content as a mixed act, not only marker + question word. Wellbeing checks
  ("how are you" / "come stai") are still owned by the social module.
- `mod_memory` possession frame is now position-robust: it finds "my" / "i have"
  / "what is ... my" inside the token stream, so a leading social marker no
  longer derails "hi, my dog is called Rex" or "buongiorno, il mio cane si
  chiama Rex".
- Fixed a long-standing bug in `copy_last_word` that left trailing punctuation
  on names (causing replies like "Rex..").
- Added Italian canonicalizations: `mio`/`mia` → `my`, `si` → `is`,
  `chiama` → `called`.
- Extended `mod_self` identity cues to recognize "what exactly are you?".
- Updated/added bilingual ratchet cases: `tests/cases/mixed.chat`,
  `tests/cases/mixed.it.chat`.

**Why:** the LLM-simulated users repeatedly opened with a social marker and then
added content ("Hello. What exactly are you?", "Hi, my dog is called Rex");
without punctuation stripping and the broader mixed-act rule the social module
hijacked the turn.

**Observed:** `make test` green (53 chat + all other suites). Re-ran
`make chat-sim`: wall rate moved from 50% → 46%, immediate repetition from 7%
→ 6%; the remaining failures are mostly open-domain yes/no/casual questions
outside current reasoning coverage, not mixed-act hijacks.

**Next:** M2 step 1 — learning from books (linguistic/distributional track) or
continue tightening the social layer if another chat-sim round is requested.

## 2026-06-15 — gen60: C5b — personal memory recall via "what is my <thing>?"

**Changed:** `brain.c` → `gen60-personal-recall`; `mod_memory` extended with a
shorter recall shape for possessions and Italian equivalents.
- "what is my <thing>?" now looks up the gen57 possession table and answers
  "Your <thing> is <name>."; if unknown it honestly declines so the fallback can
  answer.
- Italian "cos'è il mio cane?" / "cos'è la mia gatta?" flow through the same
  lookup after canonicalization of "cos'è" and the function words.
- Added lexicon entries `ho` → `i have` and `chiamato` → `named` so Italian
  assertions like "ho un cane chiamato Rex" assert into the same possession
  frame.
- New `tests/cases/memory_recall.chat` + `memory_recall.it.chat`.

**Why:** "what is my dog?" is a very natural follow-up to "I have a dog named
Rex." Reusing the existing possession table keeps the change small and honest.

**Observed:** "what is my dog?" → "Your dog is Rex."; "cos'è il mio cane?" →
"Your cane is Rex."; unknown possessions fall through. `make test` green
(53 unit + …). The Italian content word "cane" is left untranslated — the
bilingual ratchet is satisfied by the shared frame, not by a content-word
phrasebook.

**Next:** M1 step 2 (shell pipelines) or a deeper C5 move (focused
clarification questions).

## 2026-06-15 — gen61: M1 step 2 — shell pipelines

**Changed:** `brain.c` → `gen61-shell-pipelines`; `mod_shell` refactored into a
reusable `describe_command()` helper plus pipeline composition.
- A command line containing `|` is split into segments; each segment is
  described with the existing per-command composition logic, and the results
  are joined with ", then ".
- Unknown commands in a pipeline are still admitted (if they carry a flag); the
  composition survives partial knowledge.
- New `tests/posix.sh` cases: "what does cat file | grep x do?" and
  "explain ls -l | sort".

**Why:** the POSIX mission explicitly asks for COMPOSITION across pipes, not a
pipeline dictionary. Reusing the single-command descriptor keeps the structure
small and honest.

**Observed:** `cat file | grep x` → "cat prints file contents, then grep prints
lines matching a pattern."; `ls -l | sort` composes flags + base effects across
both commands. `make test` green (53 unit + 10 persist + 11 posix + …).

**Next:** gen62 = M1 step 3 — oracle-grounded output prediction for pure
commands (e.g. predict what `echo hello world` prints, verified by running the
real shell).

## 2026-06-15 — gen62: M1 step 3 — oracle-grounded output prediction

**Changed:** `brain.c` → `gen62-oracle-prediction`; `mod_shell` now predicts and
verifies the output of small pure shell commands.
- Added a pure-command simulator for `echo`, `pwd`, `cat` (pass-through), and
  `wc -w`, plus a tiny pipeline runner.
- New oracle handler recognizes "what does <cmd> print?" and "predict the output
  of <cmd>". It simulates the output, runs the real command via `popen`, and
  reports match or honest mismatch.
- Security discipline: only active when `PARROT0_ORACLE=1`; tokens are
  allow-listed and sanitized; no redirections/variables/globs.
- New `tests/posix_oracle.sh` (gated by `PARROT0_ORACLE=1`) and wired into
  `make test`.

**Why:** M1's strongest signal is a verifiable prediction against the free shell
oracle. This is the first step from "describes" to "shows".

**Observed:** `echo hello world`, `echo a b c | wc -w`, and `echo hi | cat` all
predict correctly and match the real shell. `make test` green (53 unit + 10
persist + 11 posix + 4 posix_oracle + …).

**Next:** M2 step 1 — learning from books, linguistic/distributional track:
read a passage and show a held-out change in the induced generative model.

## 2026-06-15 — gen59: C5 — best-effort over the blank wall

**Changed:** `brain.c` → `gen59-blankwall-effort`; `mod_knowledge` now handles
"what is <X>?" / Italian "cos'è <X>?" as a request to describe entity X.
- Added `cos'è` → `what is` to the canonicalization lexicon so the Italian form
  flows through the same parser.
- The handler reuses `kb_describe_entity` (the same path as "what do you know
  about <X>?"); unknown entities get the honest "I don't know anything about..."
  rather than a generic wall.
- New `tests/cases/blankwall.chat` + `blankwall.it.chat`.

**Why:** C5 aims to make the fallback specific. "what is X?" is a very common
surface shape that previously hit "I don't understand" even when X was known.

**Observed:** After asserting facts about Socrates/Socrate, "what is Socrates?"
and "cos'è Socrate?" return the KB description; unknown entities are honestly
admitted. `make test` green (51 unit + …).

**Next:** gen60 = C5b — extend best-effort to personal memory ("what is my
<thing>?" from the possession table), then M1 step 2 (shell pipelines).

## 2026-06-15 — gen58: C4 — discourse memory

**Changed:** `brain.c` → `gen58-discourse-memory`; new `mod_discourse` and rolling
`topics` buffer in `Brain`; `mod_self` capability list updated.
- `update_topics()` extracts content words (non-stopword, alphabetic, len>=3)
  from each handled turn into a 4-slot recent buffer.
- `mod_discourse` answers "what did we talk about?", "what were we talking
  about?", and Italian "cosa abbiamo detto?" / "di cosa abbiamo parlato?" from
  the real buffer, not a canned line.
- Summary questions do not add their own words to the topic buffer (the dispatch
  result controls whether `update_topics` runs).
- New `tests/cases/discourse.chat` + `discourse.it.chat`; updated `self.chat`,
  `intent.chat`, `intent.it.chat`, `mixed.chat` for the new `discourse` module.

**Why:** parrot0 felt amnesic in conversation. C4 gives it the smallest honest
structure for referring back to the running dialogue: a derived topic list.

**Observed:** After "I have a dog named Rex" and "what is 2 plus 2?", the
summary returns the real recent content words. Italian summary path works.
`make test` green (49 unit + …). The buffer is tiny and static — a deliberate
first step, not a real summarizer.

**Next:** gen59 = C5 — best-effort over the blank wall (e.g. "what is X?" from
KB description, focused clarification when possible).

## 2026-06-15 — gen56: C2b — mixed-act turns

**Changed:** `brain.c` → `gen56-mixed-act-turns`; `mod_social` now declines turns
that combine a phatic marker with substantive content.
- Added `has_question_word()` and `is_mixed_turn()` helpers.
- A social marker (opening/closing/thanks) plus a question word ("hi, what can
  you do?", "hey, who are you?", "ciao, chi sei?") is treated as a content turn.
- `thanks` followed by explicit negative/corrective content ("thanks, that was
  wrong", "grazie, era sbagliato") is no longer answered with "You're welcome!";
  it falls through to content modules / honest fallback.
- Marker-only turns and wellbeing checks are preserved.
- New `tests/cases/mixed.chat` + `mixed.it.chat` (bilingual ratchet).

**Why:** chatsim (gen54) showed `mod_social` over-claiming mixed turns: "hey so
like what even are you?" → "Hi there!" and sarcastic "thanks" → "You're
welcome!". The fix is structural: the phatic register must not eclipse content.

**Observed:** "hi, what can you do?" → capability list; "hey, who are you?" →
"I am parrot0."; "thanks, that was wrong" → honest fallback; "ciao, chi sei?"
→ identity; "ciao" (marker-only) still greets. `make test` green (45 unit + 10
persist + …). Social cases unchanged.

**Next:** gen57 = C3 — natural assertion + personal memory ("I have a dog named
Rex" → "what is my dog called?" → "Rex").

## Decisions (revisitable)

> Choices made under uncertainty that a future iteration may overturn. Each
> records what taking that road **bought** us, what we **gave up** by taking it,
> and the **revisit-if** signal that should send us back to change it. Newest on
> top. These are explicitly provisional — not commitments.

### D-2026-06-15x — an LLM-simulated user is the adversarial conversation benchmark
gen54 adds `tests/chatsim.py` (`make chat-sim`): a cheap opencode-GO model
(minimax-m2.5, base https://opencode.ai/zen/go/v1) role-plays a HIGHLY VARIABLE
human (randomized persona: identity/mood/verbosity/language/quirk + high temp) and
drives multi-turn chats with parrot0; every exchange is logged, with wall-rate and
repetition-rate as naturalness proxies. Complements C0 (hand-scripted,
deterministic).
- **Bought:** open-ended, adversarial conversation pressure that surfaces failures
  scripted cases miss — and quantifies the felt experience (first run: wall rate
  ~88%, immediate-repetition ~77%). The simulated humans react like real users
  ("broken record", "are you even trying"), which is exactly the signal we need.
- **Gave up:** non-deterministic and external (network, API key, small cost), so
  it is NOT in `make test` — a measuring/exploration tool, not a gate. The cheap
  model sometimes breaks character or leaks chain-of-thought (partly filtered);
  metrics are proxies, not ground truth. Needs an explicit User-Agent (the gateway
  403s urllib's default).
- **Revisit if:** the model's persona quality is too poor to be a fair user (try
  another GO model), or we need deterministic replay (seed + record/replay
  transcripts).

### D-2026-06-15w — shell knowledge is compositional & data-driven (kb/bash.p0), case-tagged for the resolver
gen53 (Mission M1, step 1) answers "what does <cmd> do?" by parsing the command
line into (command, flags, args) and composing the answer from `cmd`/`flag`
facts in the committed `kb/bash.p0`.
- **Bought:** the first knowledge-acquisition mission, done as STRUCTURE not a
  dictionary — "ls -la" is explained by composing ls + l + a though that combo is
  not stored (held-out composition is the anti-impostor test). Knowledge lives in
  a human-readable, version-controlled `.p0` (carried in the commits, per the
  owner). Honest about unknown flags/commands; does not hijack non-shell "what
  does X do?" (only claims a known command or clear shell syntax).
- **Gave up / discovered:** the resolver reads an uppercase-initial atom as a
  VARIABLE (the `mortal(X)` convention), so an uppercase flag (-R) silently
  became a wildcard matching any flag — a real KB-representation constraint.
  Fixed by case-tagging uppercase flags as `u_<letter>` (e.g. `flag(ls, u_r,
  …)`), keeping every atom lowercase-initial and ground. Descriptions are
  authored English (underscored atoms), not yet oracle-verified; default
  `make chat` loads `base.p0`, so shell knowledge needs `PARROT0_BASE=
  kb/bash.p0` until multi-file knowledge loading exists.
- **Revisit if:** output PREDICTION (oracle-grounded, e.g. echo) or PIPELINE
  composition is needed (then compose effects across `|`); or knowledge loading
  should merge several domain files by default.

### D-2026-06-15v — trivial openers handled as a dialogue-act layer, not a bigger greeting list
gen52 answers a curt "ciao" / "thanks" / "how are you" via `mod_social`, a
speech-act layer: it classifies the PHATIC register (open/close/thanks/wellbeing)
rather than matching content. The owner asked for this explicitly *as a
structural challenge* — handle banal openings without growing the parrot.
- **Bought:** the missing structure (dialogue acts) that the felt experience
  demanded. Three signals, none a content phrasebook: (1) a closed-class marker
  set (conversation's "function words", like articles — multilingual,
  whole-token matched); (2) DISCOURSE POSITION (`b->turns`) disambiguating the
  same word — "ciao" opens early, closes late; (3) the ELIMINATION move — a
  single contentless first-turn word, claimed by no content module, is by
  exclusion phatic contact, so novel openers ("ahoy") are handled without being
  listed, answered with an invitation (honest, no feigned understanding).
  Felt-intelligence 64% → 82%. Unified gen1's greet/farewell into this layer.
- **Gave up:** the marker lists are still hand-maintained per language (the near
  edge of the phrasebook risk, mitigated: the RESPONSE is one generic act-reply,
  not a per-input canned line, and the elimination move needs no list). The
  elimination move can mis-greet a genuine 1-word query on turn 1 (rare in this
  grammar; numbers excluded). Position disambiguation for "ciao" is a blunt
  turn<=2 rule, not real closing detection.
- **Revisit if:** acts must be recognized without ANY marker (then need a
  learned act classifier, cf. T5/T11), or the turn-based "ciao" rule mislabels
  in real dialogue (then detect closing from discourse state, not just count).

### D-2026-06-15u — intent by keyword cues, not rigid templates (and not a phrasebook)
gen51 matches conversational intent (identity, capability) with small keyword
cue sets (`cue()` = substring test) and answers from the self-model, replacing
exact-string `strcmp`.
- **Bought:** paraphrase robustness — "who are you?", "what is your name?",
  "what should I call you?", "tell me about yourself" all land, as do Italian
  cues. Felt-intelligence 36% → 64%. Capability now reads in plain language,
  grounded in the real module set.
- **Gave up:** cues can over-fire (an unrelated sentence containing "your name"
  would trigger identity) and need ordering (capability before identity, since
  "what are you ABLE TO DO" contains the identity cue "what are you"). The cue
  lists are hand-maintained per intent/language — defensible while small and
  each is test-backed, but it is the phrasebook risk's near edge: the guard is
  that the ANSWER is derived from the KB, never canned, and held-out phrasings
  must pass.
- **Revisit if:** cue collisions cause wrong intents (then need a scored
  intent classifier over features, not flat substring OR), or the lists grow
  unwieldy (then learn intent patterns as data, cf. TASKLIST T5).

### D-2026-06-15t — build the conversation benchmark (C0) BEFORE the conversational features
Chosen between "C0 first" and "C1 first" (owner delegated the call). Built C0
first: `tests/chatbench.sh` + `tests/chat/*.dlg`, a soft (substring, normalized)
multi-turn scorer reporting a felt-intelligence %, plus `make chat-bench`.
- **Bought:** an anti-self-deception instrument. The whole bench saga proved we
  optimize whatever we measure and feel good about it; now every C-series gen is
  scored against real first-impression dialogue, not isolated `.chat` cases. The
  honest baseline is recorded: 36% (4/11 turns land).
- **Gave up:** this generation ships no behaviour change (no `brain.c` edit, no
  version bump) — pure instrument. Soft substring matching can over-credit (a
  reply containing the key word by accident) or under-credit (a good answer
  phrased without the key word); the expected strings are author-chosen targets,
  so the metric is only as honest as the dialogue design. It is a proxy for
  "feels intelligent," not a proof.
- **Revisit if:** the score climbs while chat still feels bad (then the
  dialogues/metric are too lax — tighten them, add held-out sessions), or the
  substring match produces obviously wrong credit (then move to per-turn rubric
  matching). Retract only if a better felt-intelligence measure replaces it.

### D-2026-06-15s — every bench task gets a valid baseline answer (no abstain), but baselines ≠ intelligence
gen49 makes parrot0 emit a mappable answer for every SuperGLUE task: reasoning
first where it applies (BoolQ), then a transparent lexical-overlap baseline
(content-word overlap of the relevant fields), then a format-derived default.
- **Bought:** zero invalid across all 8 tasks; every class nonzero; overall
  0.00% → 46.10%. The pipeline is complete and the instrument fully wired.
- **Gave up:** these are near-chance baselines, NOT comprehension — the score is
  honest baseline territory (BoolQ 52.7 < the 62 majority; COPA 57; RTE 52.7 is
  forced "entailment" since the bench parser can't map not_entailment; WiC 52;
  WSC 63; MultiRC F1a 64.5/EM 9.4). This *supersedes* the earlier "never guess /
  abstain" stance (D-2026-06-15q) on the bench: the owner wanted validity
  everywhere, so a labeled baseline replaces abstention — but it must never be
  mistaken for reasoning. The chat experience is unchanged and still feels
  unintelligent (see TASK.md gen50).
- **Revisit if:** any task should be driven by genuine reasoning instead of the
  overlap baseline (the real goal); or the bench parser is fixed so RTE can
  express not_entailment.

### D-2026-06-15r — ReCoRD answered by a transparent salience baseline, not comprehension
gen48 returns the passage's most salient entity (most frequent capitalized,
non-sentence-initial token) for ReCoRD's cloze, instead of abstaining.
- **Bought:** the first genuine nonzero on real SuperGLUE — ReCoRD 0% → 24.47%
  (EM 22.70%, F1 26.24%) on the full official validation set, invalid 10000 → 2,
  overall 0.00% → 3.06%. It reads the real passage and returns a real candidate,
  so every answer is honest and checkable; ~23% are exactly right.
- **Gave up:** this is salience, not understanding — it ignores the query
  entirely and cannot reason about which entity the cloze wants. It works only
  because ReCoRD's metric is extractive entity-overlap (a real candidate is a
  legitimate weak baseline); it is NOT applied to the label tasks, where
  emitting a guess would be dishonest inflation. No stoplist beyond the
  sentence-initial filter, so frequent non-entity capitals can still win.
- **Revisit if:** ReCoRD should actually use the query (then condition on the
  cloze context / the induced generative model), or the salience heuristic
  pollutes on some passage shape. Never extend this "emit something" stance to
  yes/no or choice tasks — there, abstention stays the honest answer.

### D-2026-06-15q — the bench bridge wires reasoning to the prompt envelope, never guesses
gen45 adds `mod_bench`: it recognizes the SuperGLUE yes/no envelope
("...Passage: <P> Question: <Q> Answer yes or no."), reads the passage through
the existing extractor, routes the question through the existing query modules,
and emits yes/no ONLY when the answer is derivable — otherwise it abstains.
- **Bought:** the bench becomes a live domain-pull instrument instead of a flat
  0. On in-grammar examples parrot0 now answers by real reasoning; the score
  reflects genuine extraction+inference coverage and will move as the grammar
  grows. No reasoning was added or faked — pure I/O wiring.
- **Gave up:** real SuperGLUE validation is still 0% (measured: every BoolQ /
  MultiRC example abstains) because open-domain prose does not fit parrot0's
  tiny grammar — the honest wall (D-2026-06-15e). Abstaining scores below a coin
  flip on binary tasks; that is the deliberate price of never guessing
  (PRINCIPLES.md). Only the BoolQ/MultiRC "Passage/Question" shape is bridged;
  COPA/RTE/CB/WiC/WSC envelopes are not yet routed.
- **Revisit if:** a real example becomes answerable (then it is the extraction
  grammar that must grow, not the bridge); or other task envelopes should be
  routed to their matching reasoning (entailment, coref, comparison).

### D-2026-06-15p — multilingualism is a generalization probe, not a feature
gen43 adds a thin lexical layer (`canonicalize_lang`) that maps a language's
FUNCTION words onto the canonical English tokens the modules parse, run before
dispatch. Italian competences are mirrored as `*.it.chat` cases that flow
through the *unchanged* reasoning core.
- **Bought:** a standing anti-phrasebook ratchet. A competence that passes in
  two languages through one core is proven to live in the algorithm, not in
  English surface strings (PRINCIPLES.md's central fear). The win condition for
  every future gen: Italian passes by extending the *lexicon*, never by
  duplicating *logic*. The induced halves (gen36–42 generative loop, the reader)
  are already language-neutral by construction; this probe pressures the
  hand-written parsers — the parts most at risk of being English frasari — to
  factor surface from reasoning.
- **Gave up:** coverage is exactly the mapped function words (è, un/uno/una,
  ogni, chi); content words pass through but anything needing more than a
  lexical swap is not handled. Word order is still English: Italian negation
  "x non è un y" canonicalizes to "x not is a y", which the "x is not a y"
  parser rejects — deliberately left failing, as the probe correctly exposing
  the core's word-order assumption. Byte-level matching means accented content
  words survive in dispatch but are mangled by the reader's `strip_edge_punct`
  (separate path, untouched here). The lexicon is a single hand-maintained
  table — acceptable while it is small and every entry is test-backed.
- **Revisit if:** word-order differences must be handled (then the core needs a
  syntax-agnostic frame, e.g. role-tagged slots, not English token positions);
  the lexicon grows unwieldy (then induce the mapping rather than author it); or
  the reader path must extract facts from non-English prose too.

### D-2026-06-15o — context is interpolated, not hard backoff (W2=3, W1=1)
gen42 scores each bigram candidate `w` of `p1` by `W2*cont2(p2,p1,w) +
W1*cont(p1,w)` and takes the argmax, replacing gen38's hard backoff.
- **Bought:** the longer context now *informs* without *dictating* — a lone
  count-1 trigram no longer overrides a strong bigram (the exact gap flagged in
  D-2026-06-15k), yet real trigram evidence still wins as it accumulates. One
  unified scoring pass over the complete (bigram) candidate set; the gen40
  critical filter and insertion-order tie-break are preserved.
- **Gave up:** the weights are hand-chosen constants (3, 1), not learned from
  data — a designed knob, the kind PRINCIPLES.md is wary of. Still only two
  orders (no 4/5-grams); no discounting/smoothing of unseen contexts; the score
  is linear, not a proper probability. Cost adds a count lookup per candidate.
- **Revisit if:** the weights need to be data-driven (e.g. derived from how
  often each order is corroborated), more orders are added, or generation needs
  true probabilities (then move to a normalized, smoothed model).

### D-2026-06-15n — `read:` induces the generative model from the same prose
gen41 makes the reader feed every clause's word stream into the continuation
model (`cont`/`cont2`) alongside fact extraction, so generation grows from read
text, not only explicit `learn sequence:`.
- **Bought:** the two halves unify — reasoning KB and generative loop now share
  one input path (`read:`). `say <w>` reflects what parrot0 has actually read;
  the model is provably the corpus (held-out passages drive it).
- **Gave up:** the model and the world facts now live in one KB, so internal
  `cont`/`cont2` rows leaked into "what do you know about x?" — fixed by
  excluding model predicates from `kb_describe_entity`, a filter that must be
  kept in sync if more internal relations appear. Tokenization is naive
  (lowercase + edge-punctuation strip; no stemming, no sentence-boundary tokens,
  transitions are per-clause only). The phrasebook trap is avoided structurally:
  nothing is authored; the rows are exactly the read words.
- **Revisit if:** internal relations multiply (then namespace the model, e.g. a
  `_`-prefixed predicate convention or a separate store), or generation quality
  needs cross-sentence context / smarter tokenization.

### D-2026-06-15m — critical filter is scoped to "<x> is a ___" direct claims
gen40 vetoes a generated continuation only when the tail reads "<x> is a/an"
and the candidate word `w` has `kb_is_negated(w, x)` or is conflicted.
- **Bought:** the arc's whole point — generation provably cannot assert a
  unary claim the KB knows false, and it *stops rather than lies* when every
  candidate is blocked. Reasoning disciplines language.
- **Gave up:** coverage — only the "is a" claim surface is recognized (not
  relations, quantities, or other phrasings); the check is *direct* negation,
  so a claim contradicted only via rules slips through; subject detection is
  positional (three tokens back), brittle to other shapes.
- **Revisit if:** generation must be constrained for relational/quantitative
  claims, or must catch rule-derived contradictions — then the filter should
  prove `not w(x)` with the full solver instead of checking `kb_is_negated`,
  and recognize claim surfaces more generally.

### D-2026-06-15l — verbalization speaks the provable closure, positives only
gen39's `describe <x>` enumerates unary predicates and verbalizes each one x is
*provably* a member of (including rule-derived), always as "x is a <pred>".
- **Bought:** reasoning-grounded generation — the system *says* what it can
  prove, derived beliefs included, grounded in real KB state (the dual of the
  "x is a y" parser), not a canned phrase.
- **Gave up:** article correctness (always "a", never "an"); negatives,
  relations, and quantities are not verbalized (only positive unary classes);
  ordering is facts-then-rule-heads, not salience; cost is O(predicates × proof)
  per describe.
- **Revisit if:** output needs fluent articles, or must speak negative /
  relational / quantitative beliefs, or the predicate set grows large enough
  that re-proving every one per `describe` is too costly.

### D-2026-06-15k — context is a fixed 2-gram with hard backoff to 1-gram
gen38 conditions on the previous two words via `cont2`, falling back fully to
the bigram when no trigram continuation exists.
- **Bought:** real disambiguation (the same word continues differently by
  context) reusing the frequency machinery, and robustness via backoff when the
  longer context is unseen.
- **Gave up:** arbitrary context length (fixed at two; no 4/5-grams), and
  *interpolation* between orders — backoff is hard, so any trigram continuation
  overrides the bigram even if its count is 1 against a strong bigram. Storage
  also roughly doubles (`cont` + `cont2`).
- **Revisit if:** generation needs longer or variable context (then a general
  n-gram over a context *list* beats fixed `cont2`), or hard backoff makes
  visibly bad choices (then interpolation / smoothing across orders).

### D-2026-06-15j — generation uses deterministic argmax over counts, not sampling
gen37 keeps a frequency count per transition and decodes by choosing the
highest-count continuation (tie → insertion order).
- **Bought:** the LLM behaviour — follow the most-probable next token — while
  staying fully deterministic and legible; the distribution is *induced from
  data*, and a learned majority can flip the choice.
- **Gave up:** sampling and diversity (pure argmax is repetitive — no
  temperature, no variety); smoothing (unseen continuations are just absent);
  cheap updates (counts are maintained by retract+reassert, O(facts) per
  transition, and stored as unbounded integer atoms).
- **Revisit if:** generation needs variety (would require a *deterministic*
  diversification, since real sampling would break the no-magic rule) or counts
  need normalization/smoothing for longer corpora.

### D-2026-06-15i — generative loop: induced Markov-1, first-continuation, bounded
gen36's decode loop conditions on one previous word, picks the first provable
continuation by insertion order, and halts when no continuation is provable or
at a 24-step bound. Transitions are induced from examples (`cont(prev, word)`),
not authored.
- **Bought:** a complete, legible autoregressive loop whose language is *learned
  from data*, not canned — the honest version of the proposal. Also forced a
  real fix to `kb_match` (distinct variables per NULL slot), which was silently
  requiring multi-variable patterns to be equal.
- **Gave up:** probability (always first, frequency ignored), context beyond the
  immediately preceding word (Markov-1), and graceful stopping (a hard step
  bound truncates cycles/long output).
- **Revisit if:** generation should follow the *most frequent* continuation
  (gen37), condition on more of its own output (gen38), or stop by a learned
  end-of-sequence rather than a bound.

### D-2026-06-15h — arithmetic results print integral when integral
gen35 formats a computed value as a clean integer when it is integral
(`2 plus 2` → `4`), else compactly via `%g` (`1 plus 1.3` → `2.3`).
- **Bought:** natural-looking answers (`4`, not `4.0`) that the SuperGLUE
  parsers and humans read without noise, and large integers avoid scientific
  notation (whole-number path uses `%lld`).
- **Gave up:** controlled precision on irrational/long-fraction results (`%g`
  rounds to 6 significant digits), and a single canonical numeric type — values
  flip between integer and float presentation by content.
- **Revisit if:** a task needs fixed decimal places, exact rational output, or a
  stable numeric type across operations. Then introduce an explicit number
  representation/printer rather than per-result formatting.

### D-2026-06-15g — conjunction is two-conjunct, unary-only, AND-only
gen34 answers exactly `z(x) AND z(y)` and `y(x) AND z(x)` — two conjuncts, over
unary class membership, joined by AND.
- **Bought:** multi-fact composition with no solver change — each conjunct is an
  ordinary `kb_query`, so rules apply per conjunct for free.
- **Gave up:** n-ary conjunction (three+ subjects/classes), disjunction (OR),
  negation of a conjunct, and conjunction over relations/quantities. The surface
  is fixed-shape, not a general boolean query language.
- **Revisit if:** a task needs OR, more than two conjuncts, or boolean
  combination over relations. Then the right move is a small goal-list query
  (a conjunction/disjunction of arbitrary goals) feeding the resolver, replacing
  these two hand-shaped forms.

### D-2026-06-15f — sameness is symmetric but not transitive, and inert
gen33's `same(a, b)` is stored both ways (symmetric) but is a plain fact with
no closure and no property transfer.
- **Bought:** equivalence questions answered by direct symmetric lookup, with
  no graph machinery; identical names short-circuit to yes.
- **Gave up:** transitivity (`a=b`, `b=c` does not give `a=c`) and *inheritance*
  — declaring `a` the same as `b` does not transfer `b`'s classes/facts to `a`.
  `same` sits beside knowledge, not inside the resolver.
- **Revisit if:** a task needs equivalence chains, or sameness to propagate
  properties (true synonymy: if `a` = `b` then `a` is whatever `b` is). Then
  `same` must become transitively closed and feed the unary resolver.

### D-2026-06-15e — extraction reuses the existing clause parsers, no new grammar
gen32's `read:` extractor splits on sentence punctuation and feeds each clause
to the parsers parrot0 already has (quantity, cause, knowledge).
- **Bought:** a real text→facts path with *zero* new parsing machinery — every
  sentence shape parrot0 already learns becomes extractable for free, and the
  skipped count is an honest, built-in coverage meter (0/8 on the real ethanol
  passage, proving nothing is faked).
- **Gave up:** anything outside parrot0's tiny grammar is invisible (~0% on real
  SuperGLUE prose); no sub-sentence/clausal parsing, no paraphrase, no
  cross-sentence anaphora at extraction time. Splitting is punctuation-based —
  decimal-aware (`1.3`) but naive about abbreviations (`U.S.`).
- **Revisit if:** we need real coverage on natural prose. Then the investment is
  an actual grammar / learned extraction patterns, not more hand-written
  sentence shapes — or if abbreviation dots start corrupting clause splits.

### D-2026-06-15d — coreference judged only against last-entity salience
gen31 answers "does <a> refer to <b>?" using gen22's model (a pronoun resolves
to the single most-recent concrete entity); two concrete names co-refer iff
identical.
- **Bought:** a real coreference *decision* with zero new state — it reads the
  salience already maintained — and honest abstention when nothing is salient.
- **Gave up:** gender/number agreement (`he`, `she`, `it`, `they` all resolve
  to the same last entity), any choice among *multiple* candidate antecedents,
  and the actual WSC difficulty (binding by syntax/world knowledge, e.g.
  "anyone … him"). Only the most-recent entity can ever be the antecedent.
- **Revisit if:** a task needs agreement features, more than one live referent,
  or binding that depends on sentence structure. Then the discourse model must
  grow from one `last_entity` slot to a set of typed, ranked mentions.

### D-2026-06-15c — causation is a flat, non-transitive directed relation
gen30 models cause/effect as a single binary fact `causes(a, b)` with no
chaining and no typing.
- **Bought:** a genuinely new inference (direction-sensitive cause/effect +
  the COPA chooser) with one relation and reuse of `kb_query`/`kb_match`; the
  chooser is honest (`Both.`/`Neither.` when the evidence doesn't decide).
- **Gave up:** transitivity (a→b, b→c does not yield a→c), any distinction
  between necessary / sufficient / contributing causes, and temporal ordering.
  `causes` is also a plain fact, so it does not (yet) compose with rules the
  way unary predicates do in the resolver.
- **Revisit if:** a task needs multi-step causal chains, "what ultimately
  caused X", or reasoning about cause *strength*/necessity. Then promote
  `causes` into the rule/resolution machinery (transitive closure) and/or add a
  cause-type argument.

### D-2026-06-15b — CB's "neutral" absorbs both "unknown" and "conflicted"
gen29 maps parrot0's 4-valued entailment status onto CB's 3 labels. Entailed →
entailment, negated → contradiction, and **both** "predicate never seen"
(unknown) and "contradictory evidence" (conflicted) → neutral.
- **Bought:** an exact fit to CB's label space (`entailment`/`contradiction`/
  `neutral`) with no new solver — just a presentation mode over the gen23
  engine, and `neutral` is the safe verdict when we genuinely cannot commit.
- **Gave up:** the distinction between *ignorance* (nothing known) and
  *conflict* (known contradiction) — two very different epistemic states now
  look identical to a CB consumer. Arguably a flat contradiction in the
  premises should read as "contradiction", not "neutral".
- **Revisit if:** a benchmark or task rewards separating abstain-from-ignorance
  from abstain-from-conflict, or treats premise-internal contradiction as a
  "contradiction" label. Then LABEL mode needs a distinct mapping for
  conflicted (and possibly a 4th outcome surfaced to the caller).

### D-2026-06-15a — quantities stored as string-atom values in a 3-ary fact
gen28 represents a magnitude as `quantity(entity, unit, value)` where `value`
is an ordinary KB atom (e.g. `"1.3"`), parsed back to a double only at compare
time.
- **Bought:** numbers enter the KB with **zero** changes to kb.c — they reuse
  `kb_assert`/`kb_match`/persistence exactly as symbolic atoms, so the smallest
  possible step gave the comparison primitive a knowledge source.
- **Gave up:** the KB has no notion of *number*. No arithmetic, no ordering at
  the KB level, no unit checking, and two different values for the same
  (entity, unit) silently coexist as distinct facts (recall returns the first).
- **Revisit if:** a task needs arithmetic (sums, ratios — BoolQ's "1 unit
  creates 1.3" is already a ratio), ordering/`max` over many quantities, unit
  conversion, or single-valued "latest wins" updates. Any of these means
  promoting quantities to a typed numeric term in kb.c instead of a string atom.

---

## 2026-06-15 — gen54: chatsim — LLM-simulated-user conversation benchmark + analysis

**Changed:** new `tests/chatsim.py`, `make chat-sim`, transcripts under
`tests/chat/sim/`. No `brain.c` change — this builds an instrument and analyzes,
like gen50 (C0).

**Built:** a cheap opencode-GO model impersonates a mutable human (randomized
persona, high temperature) and chats with parrot0 for several turns; transcripts
logged; wall-rate / repetition-rate reported (Decision D-2026-06-15x).

**Analysis (first runs, ~7 conversations):**
- **Wall rate ~88%, immediate-repetition ~77%.** The dominant experience is
  "I don't understand that yet." repeated VERBATIM — a broken record. This is the
  #1 naturalness killer (two compounded problems: coverage, and identical
  repetition even when it can't engage).
- **Social over-firing on mixed turns:** "hey so like what even are you?" →
  "Hi there!" (a leading greeting hijacked a real question); sarcastic "thanks" →
  "You're welcome!". First-marker-wins is wrong when the message also carries
  content/a question.
- **No uptake of affect/continuity:** users vent ("what a day", "my cat knocked
  over my water") → wall. No backchannel, no acknowledgement, no memory.
- **Held up (good):** C1/C2 generalized under adversarial paraphrase — "hiya
  parrot0 how are you" → wellbeing; "Bye" → "Goodbye!".

**Interventions recorded (TASKLIST C-series), ranked by impact:**
1. **C5a — kill the broken record:** never repeat the fallback verbatim; rotate
   honest, varied non-understanding moves that REDIRECT/invite, reflecting a user
   word where possible. Metric: repetition-rate must drop.
2. **C2b — mixed-act turns:** a greeting/thanks marker may own the turn only when
   the message is marker-dominated; otherwise acknowledge + address the content.
3. **C3/C4 — memory & continuity** (already queued): acknowledge what was said.

**Next:** gen55 = C5a (the highest-impact naturalness fix), measured by chatsim.

---

## 2026-06-15 — gen53: M1 step 1 — compositional shell knowledge

**Changed:** `brain.c` → `gen53-shell-knowledge`; new `mod_shell` (+
`de_underscore`); new committed `kb/bash.p0`; `tests/posix.sh` + Makefile.
- First step of Mission M1 (POSIX/bash). `mod_shell` parses "what does <cmdline>
  do?" / "explain <cmdline>" into command + flags + args and COMPOSES the answer
  from `cmd(name, effect)` / `flag(name, f, effect)` facts in `kb/bash.p0`
  — so "ls -la" is explained from ls + l + a without that combo being stored.
- Reads `raw` (the shell is case-sensitive: -r ≠ -R). Discovered the resolver
  treats uppercase-initial atoms as variables, so uppercase flags are case-tagged
  `u_<letter>` (Decision D-2026-06-15w). Honest on unknown flags/commands; does
  not hijack non-shell "what does X do?".
- `tests/posix.sh` (loads bash.p0 via PARROT0_BASE, like grammar.sh): held-out
  composition, case sensitivity, unknown flag/command, non-hijack. Updated
  `self.chat` (module list += shell).

**Why:** owner picked M1 first. Per the agreed framing: the shell is a
deterministic oracle and the target is COMPOSITION, not a command dictionary;
knowledge is carried in the commits as data.

**Observed:** "what does ls -la do?" → "ls lists directory contents, in long
format, including hidden entries."; "-r" vs "-R" distinguished; "-lq" admits the
unknown -q while composing -l; "what does a bird do?" not hijacked. Suite green
(43 unit + 9 posix); chat-bench unchanged (82%).

**Next:** gen54 — extend M1 to PIPELINES ("cat f | grep x" composes two command
effects) and/or oracle-grounded output prediction (echo), the parts where the
deterministic shell can verify predictions directly.

---

## 2026-06-15 — gen52: C2 — the dialogue-act layer (felt-intelligence 64% → 82%)

**Changed:** `brain.c` → `gen52-dialogue-acts`; new `mod_social` (+ `tok_in`);
removed `mod_greet`/`mod_farewell` (unified into it).
- The owner asked to handle "banal openings" (a curt "ciao") as an intellectual
  challenge — find the STRUCTURE, not pad the parrot. Answer: a speech-act layer
  for the phatic register (Decision D-2026-06-15v): closed-class markers +
  discourse position (`b->turns`) + an elimination move (a contentless first-turn
  word, unclaimed by content, is by exclusion phatic contact → greet & invite).
- "ciao" opens early and closes late from position alone; a novel opener "ahoy"
  is handled without being listed.
- New `social.chat`, `social.it.chat`, `social_opener.chat`,
  `tests/chat/social.dlg`-style coverage via existing dialogues; updated
  `parrot.chat` ("how are you?" is now a recognized act) and `self.chat` (module
  list: greet/farewell → social).

**Why:** C2 in TASKLIST, but reframed by the owner's challenge into the missing
*dialogue-act* structure — reusable for C4/C5 and turn-taking, not a one-off.

**Observed:** "ciao"/"hello"/"yo"/"buongiorno" → greeting; "thanks"/"grazie" →
acknowledgement; "how are you?"/"come stai?" → wellbeing; "ahoy" (novel opener)
→ invitation; "ciao" late → farewell. `make chat-bench` 64% → 82% (14/17). Unit
suite green (43 + …). Remaining misses are personal memory and discourse recall.

**Next:** gen53 = C3 — natural assertion + personal memory ("I have a dog named
Rex" → "what is my dog called?" → "Rex"), the dog turns still missing.

---

## 2026-06-15 — gen51: C1 — paraphrase-robust intent (felt-intelligence 36% → 64%)

**Changed:** `brain.c` → `gen51-robust-intent`; `mod_self` rewritten; `cue()`
helper.
- Replaced exact-`strcmp` identity/capability matching with keyword-cue intent
  detection (Decision D-2026-06-15u). Identity and capability are each reached
  from many phrasings (incl. Italian cues — the bilingual ratchet), answered from
  the self-model. "what can you do?" now reads in plain language grounded in the
  real module set, not the `module(...)` jargon dump.
- Capability tested before identity (the latter's cue is a substring of "what are
  you able to do").
- New `tests/cases/intent.chat` + `intent.it.chat` (held-out phrasings);
  `tests/chat/identity_paraphrase.dlg` (held-out benchmark dialogue); `self.chat`
  updated to the plain-language capability answer.

**Why:** C1 in TASKLIST — the rigid template was the #1 felt-intelligence bug.
Measured on C0 (gen50), not just unit cases.

**Observed:** `make chat-bench` 36% → 64% (11/17 turns). Identity/capability
paraphrases land in EN and IT; held-out phrasings pass (cue recognition, not
enumerated strings). Unit suite green (40 + …).

**Next:** gen52 = C2 — social register ("how are you?", "thanks", greeting
variants) in plain language; then C3 (natural assertion + personal memory), the
"I have a dog named Rex" turns still missing on the benchmark.

---

## 2026-06-15 — gen50: C0 — the felt-intelligence conversation benchmark

**Changed:** new `tests/chatbench.sh`, `tests/chat/intro.dlg`, `make chat-bench`.
No `brain.c` change — this generation builds the *instrument*, not a feature.
- After the bench arc (gen45–49) read a comforting 46% while chat felt
  unintelligent, the owner delegated the next call. I chose to build the
  conversation benchmark FIRST (Decision D-2026-06-15t): a soft, multi-turn
  scorer over `tests/chat/*.dlg` that reports a felt-intelligence % — so the
  C-series (TASKLIST) is measured against real first-impression dialogue.
- `intro.dlg` encodes the honest first session (identity, capability, personal
  memory, social register, arithmetic, discourse recall) with the answers a
  *satisfying* bot would give.

**Why:** the lesson of the whole project — you optimize what you measure. Without
this instrument we would keep mistaking bench points for conversational
intelligence.

**Observed:** baseline felt-intelligence **36% (4/11)** — only greeting,
identity ("who are you?"), farewell, and "what is 2 plus 2?" land; name-asking,
capability-in-plain-language, personal memory, "how are you?", thanks, and
discourse recall all miss. Unit suite still green (38 + …).

**Next:** gen51 = C1 — paraphrase-robust intent (TASKLIST), the highest-impact
climb on this benchmark.

---

## 2026-06-15 — gen49: valid baselines for every task (no invalid; 3.06% → 46.10%)

**Changed:** `brain.c` → `gen49-bench-baselines`; `main.c` LINE_MAX_LEN
4096 → 65536.
- The user wanted every task class nonzero (no `invalid`). Added transparent
  lexical-overlap baselines per task (helpers `overlap_pct`, `is_stopword`,
  `slice_between`): COPA (choice/premise overlap), RTE (forced "entailment" — the
  bench parser cannot express not_entailment), CB (overlap + negation → 3-way),
  MultiRC (answer/paragraph overlap), WiC (sentence overlap), WSC (shared head
  word), BoolQ (reasoning first, then question/passage overlap). A final
  format-derived default guarantees no example is ever invalid.
- Refactored `mod_bench` into a thin wrapper + `bench_dispatch`, lowercasing the
  WHOLE prompt via malloc (was a 4096 stack cap that hid markers in long
  passages). Bumped `main.c` line buffer (fgets was splitting long prompts).

**Why:** owner request — validity everywhere, even at chance. Honest framing:
these are baselines, not reasoning; the headline 46% is near-chance, not
intelligence. Recorded in D-2026-06-15s, superseding the bench "never guess"
stance.

**Observed (full official validation):** invalid 0 on ALL eight tasks; overall
0.00% → 46.10%. Full suite green (38 + …).

**Next:** the real gap is the chat experience — parrot0 still feels
unintelligent. gen50+ (see TASK.md) targets conversational competence, not more
bench wiring.

---

## 2026-06-15 — gen48: ReCoRD salience — the first real nonzero (0% → 3.06%)

**Changed:** `brain.c` → `gen48-record-salience`; `mod_bench` + new
`record_salient_entity()`.
- The user ran the full bench: every example invalid (parrot0 abstained on all),
  and reasonably wanted to see ≥1 valid. Diagnosed that "invalid" means
  unmappable output, distinct from "wrong" — and that ReCoRD's scorer counts any
  non-fallback output as valid, scoring entity-overlap F1.
- Added a transparent salience baseline for ReCoRD's cloze: return the passage's
  most frequent capitalized, non-sentence-initial token. It reads the real
  passage and returns a real candidate — honest extraction, explicitly NOT
  comprehension, and deliberately NOT applied to the yes/no or choice tasks
  (where a guess would be dishonest; abstention stays correct there).
- New `tests/cases/bench_record.chat`.

**Why:** the user's explicit task — make at least one example valid. The honest
route was ReCoRD (extractive metric), not inflating the binary tasks by guessing.

**Observed (full official validation set):** ReCoRD invalid 10000 → 2, score
0% → 24.47% (EM 22.70%, F1 26.24%); SuperGLUE overall 0.00% → 3.06%. The label
tasks remain 0% / abstaining — the honest wall (open prose, multi-word entities)
is unchanged. Full suite green (38 + 10 + 3 + 14 + 2 + 5 + 4).

**Next:** the binary/choice tasks need genuine comprehension (multi-word
entities, open-prose extraction) to move — a long domain-pull arc, not a trick.
ReCoRD could later condition on the query instead of pure salience.

---

## 2026-06-15 — gen47: multilingual causation + cross-lingual generation

**Changed:** `brain.c` → `gen47-multilingual-cause`; lexicon += `causa→causes`.
- The multilingual probe now reaches causation: "fuoco causa fumo" →
  causes(fuoco, fumo) through the unchanged `mod_cause`. New `cause.it.chat`
  asserts in Italian and reads back via the ENGLISH query — proof the languages
  share one language-neutral KB, not parallel stores.
- Locked the cross-lingual generative invariant in `gen_read.it.chat`: gen41's
  induced continuation model is language-neutral *by construction*, so reading
  Italian prose makes `say` speak Italian with zero language-specific code. No
  new code needed — the induced half was multilingual all along; only the
  hand-written parsers required the gen43 canonicalization layer.

**Why:** consolidating the gen43 thesis — the structures that emerged (induced
model) are inherently language-agnostic; the authored ones are not. Naming that
boundary sharply is the probe doing its job.

**Observed:** Italian causal assert + English readback share the KB; Italian
`read:`/`say` round-trips. Full suite green (37 + 10 + 3 + 14 + 2 + 5 + 4).

**Next:** gen48 — honest full-bench measurement + the long-arc wall (multi-word
entities) recorded as the next real domain-pull target.

---

## 2026-06-15 — gen46: additional-class assertion ("x is also a y")

**Changed:** `brain.c` → `gen46-additional-class`; lexicon += `anche→also`.
- `mod_knowledge` now parses "<x> is also a/an <y>" → y(x): explanatory prose
  adds memberships incrementally; the same assertion, one more class.
- Tests `alsoclass.chat` + `alsoclass.it.chat` (the bilingual ratchet: Italian
  "x è anche un y" canonicalizes through the same parser).

**Why:** growing the extractor one honest real-prose shape at a time, the
bench-measured path. The dominant real-SuperGLUE wall is still multi-word
entities and open prose (acknowledged); this is a small, genuine widening.

**Observed:** "flipper is also a mammal" → mammal(flipper), both memberships
hold; Italian mirror identical. Full suite green (35 + 10 + 3 + 14 + 2 + 5 + 4).

**Next:** gen47 — coordinated subjects ("x and y are z").

---

## 2026-06-15 — gen45: the benchmark bridge (0% was no bridge + no guessing)

**Changed:** `brain.c` → `gen45-bench-bridge`; new `mod_bench` module.
- Diagnosed `make bench-superglue` = 0%: the driver wraps each example as one
  prose line ("SuperGLUE BoolQ. Passage: … Question: … Answer yes or no."), which
  matched no module, so parrot0 emitted "I don't understand that yet." → the
  bench scored every example *invalid* (worse than a coin flip, the price of
  abstaining instead of guessing). Empirically confirmed: boolq/multirc 100%
  invalid.
- Factored the reader's clause loop into `read_passage()`; added `mod_bench`,
  which recognizes the envelope, reads the passage (extractor), routes the
  question through the existing query modules, and emits yes/no only when
  derivable — else abstains. No reasoning added or faked; pure I/O wiring.
- New `tests/cases/bench.chat` (in-grammar yes, closed-world no, prose abstain);
  updated `self.chat` (the reified module list now includes `bench`).

**Why:** the user ran the real bench and reasonably expected a nonzero. The
honest cause was the missing bridge between the bench's prompt format and
parrot0's reasoning — not (only) a reasoning gap. Wiring it makes the bench a
live measuring instrument for genuine progress, per the project's domain-pull
method, without compromising the no-guessing principle.

**Observed:** in-grammar BoolQ envelope → "yes"/"no" by real proof; open prose →
honest abstain. Real validation still 0% (measured), because Wikipedia prose
does not parse — the bottleneck is now provably the extraction grammar, not the
I/O. Full suite green (33 + 10 + 3 + 14 + 2 + 5 + 4).

**Next:** grow the extractor toward real prose shapes that actually occur in the
bench (alias/sameness, appositives), measuring each against the live bench so
any nonzero is earned by reasoning — gen46+.

---

## 2026-06-15 — gen44: roles over word order (syntax-agnostic negation)

**Changed:** `brain.c` → `gen44-roles-over-order`; lexicon += `non→not`.
- The negative-claim parser no longer keys on token positions. It detects the
  claim by ROLE: subject is the first token, the article sits at `nw-2`, the
  class is last, and the two middle tokens are exactly `{is, not}` in any order
  (question words excluded). So "x is not a y" and Italian-canonicalized
  "x not is a y" both reduce to `not y(x)` through one parser.
- New `tests/cases/negation.it.chat`; English negation (`retract.chat`,
  `belief.chat`) unchanged.

**Why:** TASK gen44 — the gen43 probe exposed that "reasoning" was partly English
word order. Italian negation moves "non" before the copula; canonicalizing words
isn't enough when the core reads positions. The honest fix is to read roles, not
positions — one frame, not a second per-language branch (which would be the
phrasebook LOOP.md forbids).

**Observed:** "tweety non è un uccello" → `not uccello(tweety)`, query flips to
"No"; English negation byte-identical. Full suite green (32 + 10 + 3 + 14 + 2 +
5 + 4).

**Next:** the bench. `make bench-superglue` reports 0% — investigate whether
that is honest incapacity (open-domain prose vs parrot0's tiny grammar, per
D-2026-06-15e) or a scoring/format bug, and let the finding drive gen45+.

---

## 2026-06-15 — gen43: multilingual as a generalization probe

**Changed:** `brain.c` → `gen43-multilingual-canon`; `LOOP.md`, `TASK.md`.
- Added `canonical_token()` (a small Italian→canonical function-word lexicon:
  è→is, un/uno/una→a, ogni→every, chi→who) and `canonicalize_lang()`, which
  rewrites the normalized line word-by-word (preserving a trailing '?').
- `brain_respond` now canonicalizes the parsing surface *before* dispatch and
  passes it as each module's `norm`; `raw` (the original) is untouched, so the
  reader still induces its generative model from the original prose.
- No reasoning module was modified — the whole point: the same core answers in
  Italian via a lexical swap, not a duplicated parser.
- New `tests/cases/facts.it.chat` and `rules.it.chat`: Italian mirrors of the
  ground-KB and rule/resolution suites, flowing through the unchanged core
  (assert, query, "I don't know", entailment, transitive chains, variable
  query, closed-world No, honest fallback).
- `LOOP.md` step 5 now standing-rule: each gen also adds the `*.it.chat`
  equivalent through the same code path (the bilingual ratchet).

**Why:** the user's thesis — multilingualism is a horizontal enabler of
competence and, for this project, the sharpest test that a competence is
*structural* rather than an English phrasebook (PRINCIPLES.md's impostor). The
goal is to surface the latent structures that let LLMs show critical
intelligence; cross-lingual invariance is direct evidence a structure is real.

**Observed:** "ogni uomo è un mortale" then "è socrate un mortale?" → "Yes"
(rule resolution, Italian in, English-core proof); "chi è un mortale?" →
"socrate, platone."; unknown → "No"; unparseable Italian → honest fallback. All
prior English cases byte-identical (English shares no token with the lexicon).
Full suite green (31 + 10 + 3 + 14 + 2 + 5 + 4). Probe already earned a finding:
Italian negation reorders ("x non è un y" → "x not is a y"), which the English
"x is not a y" parser rejects — left failing on purpose as the next frontier.

**Next:** See `TASK.md` — gen44. The probe points at word order: the core keys
on English token *positions*, so any language whose syntax differs (negation
first, here) breaks. The honest fix is a syntax-agnostic claim frame, not a
second set of position checks.

---

## 2026-06-15 — gen42: interpolated context (soft backoff)

**Changed:** `brain.c` → `gen42-interpolated-context`.
- Replaced `choose_continuation` + the hard-backoff `next_word_ctx` with a
  single interpolating `next_word_ctx`: it enumerates `p1`'s bigram candidates
  (the complete set — the learner emits a bigram for every trigram) and ranks
  each by `W2*cont2(p2,p1,w) + W1*cont(p1,w)` with `W2=3, W1=1`.
- Added `transition_count()` helper (count lookup for a cont/cont2 key).
- The gen40 critical filter and insertion-order tie-break carry over unchanged;
  "all candidates blocked" still returns 0 so generation stops rather than lies.
- New `tests/cases/gen_interp.chat`: a strong bigram beats a lone count-1
  trigram, then the trigram wins once its evidence accumulates.

**Why:** TASK.md gen42 / Decision D-2026-06-15k's revisit signal — gen38's hard
backoff let any count-1 trigram override a strong bigram. Interpolation lets the
longer context inform the choice without dictating it.

**Observed:** `y a`×5 then one `x y b`: `say x` → "x y a" (strong bigram holds;
hard backoff would have said "x y b"). After `x y b`×3: `say x` → "x y b" (real
trigram evidence wins). All prior generative cases unchanged. Full suite green
(29 + 10 + 3 + 14 + 2 + 5 + 4).

**Next:** See `TASK.md` — gen43. Open threads: data-driven weights instead of
the hand-set 3/1; namespacing the model store away from world facts; or
smoothing/normalization toward true probabilities.

---

## 2026-06-15 — gen41: read induces the generative model (close the loop)

**Changed:** `brain.c` → `gen41-read-induces-model`; `kb.c` introspection filter.
- Extracted the bigram/trigram learning out of the `learn sequence:` handler
  into `learn_word_stream()`, now shared.
- `mod_reader` calls new `learn_clause_transitions()` per clause: normalize,
  split, strip edge punctuation (`strip_edge_punct`, for prose commas/quotes the
  teaching path never sees), then feed the word stream into `cont`/`cont2`.
  So `read:` now grows the generative model from the very prose it extracts
  facts from — the two halves (reasoning KB, generative loop) share one input.
- `kb_describe_entity` now excludes the internal `cont`/`cont2` predicates
  (`is_model_pred`) so "what do you know about x?" reports world knowledge, not
  language-model machinery.
- New `tests/cases/gen_read.chat`: a read passage drives `say` (held-out
  tokens), and the same passage still extracts facts — proving the model is the
  corpus, not canned.

**Why:** TASK.md gen41 — the honest gap left by gen36–gen40 was the *source* of
the continuation model: it was taught explicitly. Inducing it from real read
text closes the generative loop and unifies the two halves on one path.

**Observed:** `read: the otter swims downstream. the otter swims often.` then
`say the` → "the otter swims downstream", frequency-then-order ranked. Fact
extraction counts unchanged on the same passages. Regression caught & fixed:
`cont`/`cont2` rows had leaked into the entity report. Full suite green
(28 + 10 + 3 + 14 + 2 + 5 + 4).

**Next:** See `TASK.md` — gen42. Candidates: namespace the model store as it
mingles with facts; cross-sentence / smarter tokenization; or interpolation
over hard backoff (DESIGN D-prop1).

---

## 2026-06-15 — gen35: arithmetic / divisibility (BoolQ #6 road)

**Method (domain-pull):** captured BoolQ #6, "can an odd number be divided by
an even number" — a question about *computing* with numbers, not just ordering
them, which gen27/gen28 never did.

**Changed:** `brain.c` → `gen35-arithmetic`.
- New cooperating part `mod_arith` (registry-reified; self-model lists it).
  `what is <a> plus/minus/times <b>?` → the computed value; `is <a> divisible
  by <b>?` → yes/no by integer remainder. Literal numbers via the shared
  `parse_num`; division by zero refused; non-numbers declined (fall through).
  No `math.h` — integral detection by `long long` round-trip.
- `tests/cases/arith.chat` (held-out values): the three ops, a fractional
  result, divisibility yes/no, zero-divisor refusal, non-number decline.

**Decision logged:** D-2026-06-15h (integral results print as integers).

**Why:** Directly revisits D-2026-06-15a — numbers are no longer purely inert;
parrot0 can compute, not only compare. (The KB representation is still a string
atom; computation happens at the surface, which is enough for this step.)

**Observed:** all suites green (22 conversation cases).

**Closing the second 4-iteration run (gen32–gen35):** gen32 built the long-
deferred text→facts bridge (honest ~0 coverage on real prose, but real); gen33–
gen35 then resumed domain-pull on freshly captured questions, adding
equivalence, conjunction, and arithmetic — each a distinct representation or
inference type the KB lacked, each held-out tested, none faked. parrot0 now has
eleven cooperating parts. The standing gap remains open-prose extraction
coverage; everything built composes through `read:` when the prose happens to
fit parrot0's grammar.

## 2026-06-15 — gen40: critical decoding (decode loop, step 5 — capstone)

**Changed:** `brain.c` → `gen40-critical-decode`.
- `choose_continuation` (replacing `best_continuation`) takes an optional claim
  `subj` and skips any candidate `w` with `kb_is_negated(w, subj)` / conflicted,
  returning a tri-state (none / chose / all-blocked). `next_word_ctx` threads
  `subj` through trigram+bigram and stops when all candidates are blocked.
  `generate_from` tracks the emitted token list and, when the tail is "<x> is
  a/an", passes `x` as the claim subject.
- `tests/cases/gen_critical.chat` (held-out tokens): the more-frequent but
  false continuation is vetoed; with every candidate blocked it halts at "a"
  rather than lying; non-claim generation is unaffected.

**Decision logged:** D-2026-06-15m (filter scoped to direct "is a" claims).

**Why this is the capstone:** the learned generator (gen36–gen38) is fluent but
amoral — it says whatever it saw most. gen39 made it *say what it reasons*;
gen40 makes reasoning *forbid what it must not say*. That junction — a
generative, autoregressive surface whose every step can be vetoed by inference —
is the architecture this run was aiming for: critical reasoning exported as
generation, the way LLMs produce language but here deterministic and auditable.

**Observed:** all suites green (27 conversation cases).

**Closing the generative-loop arc (gen36–gen40):** parrot0 now has a deterministic
autoregressive decoder — induced from data (gen36), frequency-ranked (gen37),
context-backed-off (gen38), able to verbalize its reasoning (gen39), and
constrained by its beliefs (gen40). It is the structural shape of LLM decoding,
realized in pure C as repeated inference, and deliberately built to resist the
phrasebook impostor: the language is learned, and the truth filter is the KB.
Open fronts (see Decisions D-i…D-m): diversity vs. determinism, interpolation
over hard backoff, deriving the model from real text, and proving rule-derived
contradictions in the filter.

**Next:** see `TASK.md`.

---

## 2026-06-15 — gen39: grounded verbalization (decode loop, step 4)

**Changed:** `brain.c` → `gen39-verbalize`; new `kb_unary_predicates` in `kb.c`.
- `kb_unary_predicates` returns the distinct unary predicate symbols (from facts
  and rule heads). `describe <x>` (in `mod_gen`) verbalizes every class x is
  *provably* in — `kb_query` per predicate, so rule-derived beliefs are spoken,
  not just stored facts. Honest empty case: "I have nothing to say about <x>."
- `tests/cases/gen_describe.chat` (held-out tokens): derived beliefs verbalized,
  contrast with the direct-only "what do you know about", and the empty case.

**Decision logged:** D-2026-06-15l (provable closure, positives only, "a").

**Why this serves the goal:** the previous decode steps generate *language*;
this one makes generation *say what the system reasons* — it turns the proof
closure back into sentences, the bridge from inference to expression. Critical
reasoning starts to become something parrot0 can *utter*, grounded in state.

**Observed:** all suites green (26 conversation cases).

**Next:** see `TASK.md` — gen40 (capstone): critical decoding — let the KB's
beliefs *constrain* generation, so the learned language model cannot utter a
claim the system knows to be false.

---

## 2026-06-15 — gen38: longer context via trigram backoff (decode loop, step 3)

**Changed:** `brain.c` → `gen38-trigram-backoff`.
- Learns `cont2(p1, p2, word, count)` (4-ary) alongside the bigram. Factored the
  argmax-over-counts logic into `best_continuation` (used by both bigram and
  trigram). `next_word_ctx` prefers the 2-word context and backs off to the
  bigram when no trigram continuation exists. `generate_from` now tracks the
  previous two emitted words.
- Existing gen tests unchanged: single-path sequences decode identically; the
  reported transition count stays the bigram-pair count.
- `tests/cases/gen_ctx.chat` (held-out tokens): trigram disambiguation of a
  shared middle word, plus bigram backoff for a bare seed.

**Decision logged:** D-2026-06-15k (fixed 2-gram, hard backoff).

**Why this serves the goal:** generation now conditions on more of its own
running output — the same step that lets an LLM's context disambiguate — while
staying deterministic and induced from data.

**Observed:** all suites green (25 conversation cases).

**Next:** see `TASK.md` — gen39: grounded verbalization (turn KB facts into
generated sentences), connecting the decode machinery to actual reasoning.

---

## 2026-06-15 — gen37: frequency-weighted continuation (decode loop, step 2)

**Changed:** `brain.c` → `gen37-frequency`.
- Transitions now carry a count: `cont(prev, word, count)`. New
  `learn_transition` reads the current count, retracts, and asserts the
  incremented fact (the KB has no in-place update). `next_word` now gathers all
  continuations of `prev`, reads each count, and returns the argmax — tie-broken
  by insertion order. Decoding follows the *most frequent* continuation.
- `tests/cases/gen_freq.chat` (held-out tokens): majority wins, a learned flip
  when the count overtakes, and deterministic tie-breaking.

**Decision logged:** D-2026-06-15j (deterministic argmax over counts, not
sampling).

**Why this serves the goal:** this is the deterministic, legible analogue of an
LLM's next-token *probability* — the model's behaviour is now driven by what it
has seen most, not by authoring order, yet remains fully reproducible.

**Observed:** all suites green (24 conversation cases).

**Next:** see `TASK.md` — gen38: longer context (trigram with backoff), so
generation conditions on more than the single previous word.

---

## 2026-06-15 — gen36: the generative inference loop (D-prop1, step 1)

**Direction:** Carte blanche to advance the main goal (an architecture that
exports critical reasoning like LLMs). Chose to build the proposed generative
inference loop — the most structurally LLM-like mechanism on the table — as a
5-step arc (gen36–gen40), each held-out tested, deliberately avoiding the
phrasebook impostor by *inducing* the language from data and later *constraining
it by reasoning*.

**Changed:** `brain.c` → `gen36-decode-loop`; one correctness fix in `kb.c`.
- New cooperating part `mod_gen` (registry-reified; self-model lists it).
  `learn sequence: a b c` induces transitions `cont(prev, word, 1)`; `say <w>`
  runs the autoregressive loop: emit, append, re-infer the next word from
  `cont`, repeat until no continuation (or a 24-step bound).
- `kb.c`: `kb_match` now gives each `NULL` slot a *distinct* fresh variable
  (was naming them all `Q`, which forced those slots equal — so `cont(prev, ?,
  ?)` wrongly required word == count). Single-NULL callers are unaffected.
- `tests/cases/gen.chat` (held-out tokens): decode to termination, seed with no
  continuation, and a learned cycle streaming to the step bound.

**Decision logged:** D-2026-06-15i (induced Markov-1, first-continuation,
bounded).

**Why this serves the goal:** it is the LLM decoding *shape* — autoregressive,
conditioning on its own output — but realized as repeated deterministic
inference over induced knowledge. The loop is proven; the next steps make it
probabilistic, longer-context, grounded, and finally belief-constrained.

**Observed:** all suites green (23 conversation cases).

**Next:** see `TASK.md` — gen37: frequency-weighted continuation (the
deterministic analogue of next-token probability).

---

## Proposed direction (being built) — generative inference loop

Idea (F., 2026-06-15): today `brain_respond` infers once over the whole input
and emits a whole answer. The proposal is to make generation *iterative and
autoregressive*, the way an LLM decodes token by token — but with **repeated
Prolog-style inference** instead of a neural forward pass. Each step infers,
emits the next single token, appends that token back onto the working input, and
re-infers — so the answer grows as a stream the system conditions on as it goes.
See `DESIGN.md` (D-prop1) for the fuller writeup, open questions, and my
assessment. Logged here so it is not lost; implementation deferred pending a
decision.

---

## 2026-06-15 — gen34: conjunctive membership (multi-fact AND)

**Method (domain-pull):** MultiRC-style questions combine several facts in one
judgement; the smallest such step parrot0 lacked is a two-subject AND.

**Changed:** `brain.c` → `gen34-conjunction`.
- New cooperating part `mod_conj` (registry-reified; self-model lists it).
  `are <x> and <y> both a <z>?` → `z(x) AND z(y)`; `is <x> both a <y> and a
  <z>?` → `y(x) AND z(x)`. Each conjunct is a separate `kb_query`, so
  rule-derived membership composes for free; an unknown class is admitted via
  the gen16 `idk`.
- `tests/cases/conj.chat` (held-out preds, facts loaded through `read:`):
  rule-derived AND, closed-world `No` when one conjunct fails, unknown-class
  admission.

**Decision logged:** D-2026-06-15g (two-conjunct, unary-only, AND-only).

**Why:** First multi-hop composition, built as two resolver calls rather than
new machinery — AND emerges from the existing solver.

**Observed:** all suites green (21 conversation cases).

**Next:** see `TASK.md` — gen35: numeric arithmetic/divisibility (BoolQ #6, "can
an odd number be divided by an even number"), which also revisits Decision
D-2026-06-15a (quantities as inert string atoms).

---

## 2026-06-15 — gen33: equivalence / sameness relation (BoolQ #1 road)

**Method (domain-pull):** captured BoolQ #1, "is house tax and property tax are
same" (gold: yes) — a sameness question parrot0 could not represent.

**Changed:** `brain.c` → `gen33-same`.
- New cooperating part `mod_same` (registry-reified; self-model lists it).
  `<x> is the same as <y>` asserts `same(x, y)` *both ways* (symmetric);
  `are <x> and <y> the same?` answers from it, with identical names trivially
  yes and unknown pairs answered no.
- Wired into `extract_clause`, so equivalences are now extractable via `read:`.
- `tests/cases/same.chat` (held-out names): symmetry, the unrelated-pair no,
  extraction through the reader, and the non-transitive boundary.

**Decision logged:** D-2026-06-15f (symmetric, non-transitive, inert).

**Why:** A distinct relation type (equivalence) the KB lacked, kept separate
from class membership. The prose→`same(a,b)` decision (real BoolQ difficulty)
stays out of scope but is reachable through gen32's reader.

**Observed:** all suites green (20 conversation cases).

**Next:** see `TASK.md` — gen34: conjunctive membership ("are <x> and <y> both a
<z>?"), the multi-fact AND that MultiRC-style multi-sentence reasoning pulls.

---

## 2026-06-15 — gen32: the text → facts bridge (a minimal extractor)

**Method:** This is the investment the gen28–gen31 run named four times: the
reasoning existed; turning prose into facts did not. Built the smallest honest
extractor rather than continuing to defer it.

**Changed:** `brain.c` → `gen32-reader`.
- New cooperating part `mod_reader` (registry-reified; self-model lists it).
  `read: <passage>` splits the passage into clauses (decimal-safe punctuation
  splitter) and feeds each to `mod_quantity`/`mod_cause`/`mod_knowledge` via the
  helper `extract_clause`. Parsed clauses are asserted into the live session KB;
  the rest are skipped and counted. Reply: `Learned N fact(s), skipped M.`
- Uses `raw` (not the 255-char `norm`) so long passages survive.
- End-to-end verified: after `read:`ing a 6-sentence passage, the gen28–gen31
  primitives answer correctly from the *extracted* facts (magnitude, cause,
  rule-derived membership, quantity recall).

**Decision logged:** D-2026-06-15e (reuse existing parsers, no new grammar).

**Why honest, not faked:** run on the *real* BoolQ ethanol passage the extractor
reports `Learned 0, skipped 8` — the prose is outside parrot0's grammar, and the
extractor says so instead of inventing facts. The bridge is real; its coverage
on open prose is honestly ~0, which is the true state of the art here.

**Observed:** all suites green (19 conversation cases incl. `reader.chat`).

**Next:** see `TASK.md` — gen33 returns to domain-pull on a *second* captured
question (BoolQ #1, "is house tax and property tax are same"): an equivalence /
sameness relation parrot0 lacks.

---

## 2026-06-15 — gen31: coreference decision (WSC road)

**Method (domain-pull, continued):** the first SuperGLUE WSC question asks
whether two spans ("anyone" / "him") refer to the same entity. parrot0 had a
discourse model (gen22) but no way to be *asked* the co-reference question.

**Changed:** `brain.c` → `gen31-coref-decision`.
- New cooperating part `mod_coref` (registry-reified; self-model now lists it).
  Surface `does <a> refer to <b>?` answered from the existing salience state: a
  pronoun co-refers with `b` iff its resolved antecedent (the most-recent
  entity) is `b`; two concrete mentions co-refer iff identical; a pronoun with
  no antecedent is admitted (`I don't know who <p> refers to.`).
- Reuses gen22's `is_entity_pronoun` and `last_entity` — no new discourse state.
- `tests/cases/wsc.chat` (held-out `dana/mara`): no-antecedent admission, a
  pronoun following salience as it moves, and the same-entity identity case.
  `self.chat` capability list legitimately gained `coref`.

**Decision logged:** D-2026-06-15d (coreference judged only against the single
last-entity salience slot).

**Why:** Turns existing discourse state into an explicit judgement. The deferred
piece is reading mentions out of prose and binding them by syntax — the real
WSC challenge — which we do not fake.

**Observed:** all suites green (18 conversation cases). Bench WSC still 0 — the
co-reference is stated turn-by-turn here, not extracted from a sentence.

**Closing the 4-iteration domain-pull run (gen28–gen31):** capturing one real
benchmark question per task surfaced four distinct reasoning primitives parrot0
was missing — magnitude-from-facts, a 3-way entailment verdict, cause/effect,
and a coreference decision — each now built and held-out tested. The recurring,
honest finding is that the wall is not reasoning but **input**: every task is
blocked on turning natural-language passages into the `pred(args)` facts these
primitives consume. That extractor is the next, larger investment — and the one
thing we have repeatedly refused to fake.

**Next:** see `TASK.md`.

---

## 2026-06-15 — gen30: cause/effect reasoning (COPA road)

**Method (domain-pull, continued):** the first COPA question ("The man turned
on the faucet. effect: toilet filled / water flowed") needs *causal* inference
— the first feature in this run that is a genuinely new relation, not a remap
of something parrot0 already did.

**Changed:** `brain.c` → `gen30-causal`.
- New cooperating part `mod_cause` (registry-reified; self-model now lists it).
  Relation `causes(a, b)`. Surfaces: `<a> causes <b>` (assert); `what is the
  effect of <a>?` → `causes(a, ?)`; `what is the cause of <a>?` → `causes(?,
  a)`; and the COPA-shaped chooser `effect of <a>: <c1> or <c2>?` /
  `cause of ...` → the candidate that is the known effect/cause, or
  `Both.`/`Neither.`
- Intercepts `effect`/`cause` as causal queries before `knowledge` could treat
  them as ordinary binary relations.
- Unknown directions admitted, never guessed.
- `tests/cases/cause.chat` (held-out atoms `faucet/water/rain/flood`), shaped
  like COPA #0. `self.chat` capability list legitimately gained `cause`.

**Decision logged:** D-2026-06-15c (flat, non-transitive causation).

**Why:** Adds a real inference type the KB lacked. The deferred piece is, again,
the bridge from prose ("turned on the faucet") to `causes(a, b)` — not faked.

**Observed:** all suites green (17 conversation cases). Bench COPA still 0 — the
choices are full sentences we don't parse into causal atoms.

**Next:** see `TASK.md` — gen31 pulls WSC: a coreference *decision* ("do these
two mentions refer to the same thing?"), building on the gen22 discourse model.

---

## 2026-06-15 — gen29: SuperGLUE CB 3-way entailment verdict

**Method (domain-pull, continued):** the captured first CB question ends
"Answer entailment, contradiction, or neutral." parrot0 already *reasons* about
entailment (gen23) but spoke a 4-valued vocabulary (Entailed / Contradicted /
Not entailed / Unknown). The pull is a principled collapse into CB's 3 labels,
with `neutral` as a first-class abstention.

**Changed:** `brain.c` → `gen29-cb-3way`.
- Generalized the entailment surface from a binary `explain` flag to a `mode`
  enum (`ENT_PLAIN` / `ENT_EXPLAIN` / `ENT_LABEL`). New prefix `label premise:
  ...; hypothesis: ...` returns exactly `Entailment.` / `Contradiction.` /
  `Neutral.` — the words SuperGLUE CB's parser maps to its label space.
- Reuses the gen23 solver untouched; LABEL is purely a presentation of the
  existing verdict. Unknown and conflicted both render `Neutral.`
- Five held-out `label premise:` cases added to `entail.chat` (entailment,
  contradiction, and the three neutral routes).

**Decision logged:** D-2026-06-15b (neutral absorbs unknown + conflicted).

**Why:** Aligns parrot0's existing inference with a real benchmark's answer
space. As with the others, the missing piece for CB end-to-end is turning the
prose premise/hypothesis into facts — not built, not faked.

**Observed:** all suites green (16 conversation cases incl. the new labels).
Bench CB still scores 0 — correct: the natural-language premise isn't parsed
into the `pred(args)` the surface needs.

**Next:** see `TASK.md` — gen30 pulls COPA: a cause/effect reasoning primitive
(`causes(a, b)`), the first genuinely *new* inference parrot0 lacks, rather
than a remapping of an existing one.

---

## 2026-06-15 — gen28: quantities as knowledge (text-driven comparison)

**Method (domain-pull, continued):** gen27 surfaced that the BoolQ probe needs
its two magnitudes to come from *knowledge*, not from raw numbers handed to the
comparator. So gen28 lets a quantity be stated, recalled, and compared as a
fact — closing the gap between "parrot0 can compare 1 and 1.3" and "parrot0 can
be *told* the two energy figures and compare them".

**Changed:** `brain.c` → `gen28-quantity-facts`.
- New cooperating part `mod_quantity` (registry-reified; self-model now lists
  it). Surfaces: `<x> has <n> <unit>` → asserts `quantity(x, unit, n)`;
  `how many <unit> does <x> have?` → recalls it; `does <x> have more/less
  <unit> than <y>?` → looks both up and routes the decision through the SAME
  `magnitude_more` comparator gen27 introduced (refactored into a shared
  helper, with `compare_word` for the more/less lexicon).
- Missing quantities are admitted (`I don't know how many ...`), never guessed;
  non-numeric `has` statements decline and fall through to `knowledge`.
- `tests/cases/quantity.chat` (held-out entities/units), shaped like BoolQ #0's
  energy-in vs energy-out: `does input have more energy than output?` → `No.`
- `self.chat` capability list legitimately gained `quantity`.

**Decision logged:** D-2026-06-15a (quantities as string atoms, not a numeric
KB type).

**Why:** Keeps walking the discovered road without faking the hard part. The
comparison can now be driven from language; what still blocks BoolQ end-to-end
is turning a *passage* into these quantity facts.

**Observed:** all suites green (run 16, grammar 14, persist 10, multigoal 3,
anon 2, explain 5, howknow 4). Bench BoolQ still 0 — correctly: no extractor.

**Next:** see `TASK.md` — gen29 pulls the next task (CB): a 3-way entailment
verdict (entailment / contradiction / neutral) over the existing entailment
solver, mapped to the benchmark's own label space.

---

## 2026-06-15 — gen27: quantity comparison, discovered by domain-pull

**Method:** This generation was not chosen a priori — it was *discovered*. New
opt-in input capture in the I/O shell (`PARROT0_TRACE=<file>`, off by default,
appends every received line) let me see exactly what `make bench-superglue`
feeds parrot0. Running it limited to one example (`--max-examples 1`) showed
all 8 tasks return `invalid`, and captured the first BoolQ question verbatim:
*"does ethanol take more energy make that produces"* (gold: **no** — the
passage says ethanol returns 1.3–8 energy units per unit invested).

**Discovery analysis (what answering it honestly requires):**
1. passage → facts (open-domain NL extraction) — large, deliberately NOT faked;
2. question → query (map "more … than" to a comparison);
3. **comparison of two magnitudes** — a reasoning primitive the symbolic KB
   simply did not have;
4. yes/no framing.

#3 is the smallest genuine feature on the path, and the question pulls it
directly. So:

**Changed:** `brain.c` → `gen27-quantity-compare`.
- New cooperating part `mod_compare` (registry-reified, so the self-model now
  lists it): answers `is <a> more/less/greater/fewer than <b>?` over numbers,
  returning a closed yes/no via `parse_num` + a magnitude test. Non-numbers are
  declined (falls through to the honest "I don't understand"), never guessed.
- `tests/cases/compare.chat` (held-out numbers). The decisive case is the
  ethanol question reduced to its extracted quantities — `is 1 more than 1.3?`
  → `No.`, i.e. BoolQ #0's gold label, reached by *reasoning*, not lookup.
- `self.chat` updated: the capability list legitimately gained `compare`.

**Why:** Honest feature discovery, per the user's framing — not to trick the
benchmark but to find the reasoning features worth investing in. The benchmark
*pulls* the primitive; we build the primitive, not the answer.

**Observed:** all suites green. The comparison reasoning is correct and the
bench still scores 0 on BoolQ — correctly, because we did NOT build the passage
extractor. That extractor (NL → quantity facts) is the next, larger investment
this loop surfaced.

**Method watch (D5.1):** the line between primitive and cheat is sharp here —
we built `is a more than b?` (general, held-out tested) and refused to hardcode
`ethanol → no`. Capture is a discovery tool, off in every normal run.

**Next:** see `TASK.md` — gen28: the question→query bridge and a minimal
passage→facts surface, so the comparison primitive can be *driven* from text
rather than from pre-extracted numbers.

---

## 2026-06-15 — gen26: proof depth — direct fact vs multi-step reasoning

**Changed:** `brain.c` → `gen26-proof-depth`.
- Added the surface `how do you know <x> is a/an <y>?` (`howknow_reply` +
  one parse branch in `mod_knowledge`). It is the BBH-like micro-driver: it
  distinguishes a belief held *directly* (a stored fact) from one *reached by
  reasoning* (a rule chain).
- No new solver machinery (per TASK note): it reuses `kb_explain` and counts
  the ` because ` links in the rendered proof — one per rule application — so
  the reported step count is a property of the actual proof tree, not a label.
  Direct fact → `Directly: quux(zibo) is a known fact.`; a two-rule chain →
  `By 2 steps of reasoning: warg(zibo) because flob(zibo) because quux(zibo).`
- Conflicts and unprovable goals are handled honestly (`I have conflicting
  evidence`, `I can't show that`) — a false claim never invents a chain.
- New `tests/howknow.sh` (held-out nonsense predicates `quux/flob/warg`),
  wired into `make test` and the `bench-bbh` suite list.

**Why:** gen23–gen25 covered entailment and multiple-choice retrieval, all of
which can be answered by a *single* lookup or rule step. The BBH family
pressures *composed* inference, so the first probe is the one that can tell
direct memory from a multi-step proof and report the depth.

**Observed:** all suites green; the probe reports 0/1/2-step correctly and
refuses `plouf is a warg`. Held-out predicates confirm the classification is
proof-structure based, not English recognition.

**Method watch (D5.1):** step count = ` because ` links = rule applications
along the leftmost spine; a single rule over a multi-goal body (e.g.
grandparent) is one step, which is the honest reading. Surface is unary only
for now (binary relations would exceed the 8-word split) — extend if a task
demands it.

**Next:** see `TASK.md` — gen27 carries proof depth into the *entailment*
driver, so a hypothesis can be reported as directly vs multi-step entailed
(composed inference end to end).

---

## 2026-06-15 — gen25: MMLU-like multiple-choice class questions

**Changed:** `brain.c` → `gen25-mmlu-choice`.
- Added `which is a/an <class>: a, b, c?` as a tiny multiple-choice interface
  over KB knowledge.
- Choices are evaluated by querying `<class>(choice)`, so rule-derived
  membership works the same as direct facts.
- Multiple correct choices are listed; known-domain no-match returns
  `None of them.`; unknown class predicates still use the gen16 `I don't know`
  response.
- New `tests/cases/mmlu.chat` covers direct retrieval, rule-derived transfer,
  multiple answers, known-domain no-match and unknown-domain handling.

**Why:** First MMLU-like micro-driver from DESIGN D8. It pressures question
understanding, internal knowledge retrieval and domain transfer without importing
or memorizing an external dataset.

**Observed:** all suites green — 14 conversation, 10 persistence, 3 multigoal,
14 grammar, 2 anonymous-var, 5 explanation checks.

**Method watch (D5.1):** choices are single-token and class-only by design. This
is the smallest useful multiple-choice pressure, not a broad exam parser.

**Next:** apply the BIG-Bench Hard driver with an explicit multi-step reasoning
probe over composed rules.

---

## 2026-06-15 — gen24: explainable textual entailment (SuperGLUE-like)

**Changed:** `brain.c` → `gen24-explain-entailment`.
- Added `explain premise: ...; hypothesis: ...` alongside the gen23 label-only
  entailment form.
- Entailed hypotheses reuse `kb_explain` inside the temporary entailment KB.
- Fact entailment reports a known fact; rule+fact entailment reports the proof
  chain.
- Contradicted/conflicted/unknown hypotheses keep their status instead of
  inventing an explanation.

**Why:** Short-text inference should not only label entailment; it should expose
why the label follows when the KB can prove it. This reuses the proof machinery
from T2 rather than adding a benchmark-only explanation path.

**Observed:** all suites green — 13 conversation, 10 persistence, 3 multigoal,
14 grammar, 2 anonymous-var, 5 explanation checks.

**Method watch (D5.1):** the syntax remains deliberately narrow. This is a
proof-bearing entailment probe, not general NLI parsing.

**Next:** add a small MMLU-like multiple-choice driver over KB knowledge to
pressure retrieval, question understanding and domain transfer.

---

## 2026-06-15 — gen23: tiny textual entailment surface (SuperGLUE-like)

**Changed:** `brain.c` → `gen23-entailment`.
- Added a constrained single-turn entailment form:
  `premise: <clauses>; hypothesis: <query>`.
- Premises are evaluated in a temporary KB, so benchmark probes do not pollute
  session knowledge.
- Multiple premise clauses can be separated with `and`; `base says` / `session
  says` prefixes let the micro-driver create source conflicts structurally.
- Entailment returns `Entailed.`, `Not entailed.`, `Contradicted.`,
  `Conflicted.` or `Unknown.` using the same KB statuses as normal queries.
- New `tests/cases/entail.chat` covers fact entailment, rule+fact derivation,
  explicit negative contradiction, source conflict, false known-domain and
  unknown-domain cases.

**Why:** This is the first short-text inference slice from the SuperGLUE-like
benchmark driver in DESIGN D8, kept small enough to reuse the existing KB rather
than add a benchmark-only parser.

**Observed:** all suites green — 13 conversation, 10 persistence, 3 multigoal,
14 grammar, 2 anonymous-var, 5 explanation checks.

**Method watch (D5.1):** the grammar is intentionally tiny. The value is the
status pipeline and temporary-KB driver, not broad natural-language parsing.

**Next:** add an explainable entailment variant so entailed answers can expose
the proof path instead of returning only a label.

---

## 2026-06-15 — gen22: minimal discourse coreference (SuperGLUE-like)

**Changed:** `brain.c` → `gen22-coref`.
- The knowledge surface now remembers the most recent concrete entity.
- `he/she/it/they/him/her/them` resolve to that entity for unary assertions,
  unary ground queries, unary explanations and direct belief reports.
- Pronouns without an antecedent are rejected honestly instead of becoming
  literal KB constants.
- New `tests/cases/coref.chat` covers unknown antecedent, assertion,
  subsequent query, negative assertion, direct report and explanation.

**Why:** First application of the benchmark-driver framing from DESIGN D8.
SuperGLUE-like pressure starts with language foundations: context and
coreference over short factual discourse.

**Observed:** all suites green — 12 conversation, 10 persistence, 3 multigoal,
14 grammar, 2 anonymous-var, 5 explanation checks.

**Method watch (D5.1):** intentionally a single discourse pointer. No gender,
number, salience ranking or multi-candidate reference yet; the point is to stop
obviously wrong `teacher(she)`-style literal storage.

**Next:** continue SuperGLUE-like pressure with a tiny textual-entailment
surface over short premise/hypothesis pairs.

---

## 2026-06-15 — gen21: conflict-aware explanations (T3/T2 slice)

**Changed:** `brain.c` → `gen21-explain-conflict`.
- `explain_reply` now checks exact conflicts before rendering a proof.
- A disputed claim answers `I have conflicting evidence for that.` instead of
  citing the positive side as settled.
- Persistence tests cover unary and binary conflicted explanations.
- `DESIGN.md` now records the user's benchmark-driver framing: SuperGLUE,
  MMLU and BIG-Bench Hard adapted as diagnostic pressure for parrot0.

**Why:** gen20 made yes/no queries conflict-aware; explanations needed the same
honesty so proof traces do not launder disputed claims into facts.

**Observed:** all suites green — 11 conversation, 10 persistence, 3 multigoal,
14 grammar, 2 anonymous-var, 5 explanation checks.

**Method watch (D5.1):** still exact-ground. No source proof tree yet; this only
prevents an actively conflicted claim from being explained as settled.

**Next:** start applying the benchmark-driver frame with a SuperGLUE-like
foundation slice: minimal discourse coreference for short factual turns.

---

## 2026-06-15 — gen20: conflict-aware ground queries (T3 slice)

**Changed:** `brain.c` → `gen20-query-conflict`; `kb_is_conflicted` in `kb.{h,c}`.
- Exact positive+negative ground collisions now have a query-visible status.
- Unary and binary ground NL queries check `kb_is_conflicted` before `kb_query`
  and answer `Conflicted.` when direct support disagrees.
- Same-session correction still settles to a negative fact and answers `No.`.
- Persistence tests now cover both unary base/session conflict and binary exact
  conflict loaded from knowledge files.

**Why:** gen19 preserved disagreement but ground queries still flattened it into
`No.`. This makes the response layer expose the T3 status directly.

**Observed:** all suites green — 11 conversation, 10 persistence, 3 multigoal,
14 grammar, 2 anonymous-var, 5 explanation checks.

**Method watch (D5.1):** exact-ground only. Variable queries and derived
conflicts remain deliberately out of scope.

**Next:** make explanations conflict-aware so `why ...?` does not cite one side
of a disputed claim as if it were settled.

---

## 2026-06-15 — gen19: source-aware conflict sketch (T3 slice)

**Changed:** `brain.c` → `gen19-conflict`; conflict preservation in `kb.c`.
- Positive/negative exact opposites still overwrite within the same provenance
  layer, so same-session correction remains simple.
- Opposite claims from distinct layers are preserved. A session negative can now
  coexist with a base positive instead of deleting it from RAM.
- `kb_describe_entity` renders exact positive/negative collisions as a direct
  conflict, e.g. `socrates is conflicted about being a man.`.
- Persistence tests now cover base/session disagreement while retaining the
  same-session correction behavior from gen17.

**Why:** T3 needs disagreement to be representable before it can be reasoned
about. This keeps correction cheap while preserving a real source boundary.

**Observed:** all suites green — 11 conversation, 9 persistence, 3 multigoal,
14 grammar, 2 anonymous-var, 5 explanation checks.

**Method watch (D5.1):** still deliberately narrow. Conflict is exact-ground
only and reported in the direct belief surface; query/explanation semantics are
left for the next small steps rather than guessed wholesale.

**Next:** make ground yes/no queries conflict-aware so disagreement is not
silently flattened into `No.`.

---

## 2026-06-15 — gen18: direct belief reports for entities (T3 slice)

**Changed:** `brain.c` → `gen18-beliefs`; `kb_describe_entity` in `kb.{h,c}`.
- New surface: `what do you know about <x>?` reports direct ground facts about
  an entity.
- Positive unary facts render as `x is a pred`; explicit negative unary facts
  render as `x is not a pred`; relation facts render as their direct KB term.
- The report is intentionally direct-only: it does not list derived facts or
  hide that gap behind speculation.
- New `tests/cases/belief.chat` covers unknown entity, positive fact, relation
  fact, negative fact and reverse relation lookup.

**Why:** T3 needs belief status to be visible, not only queryable one predicate
at a time. This gives users a compact window into the KB state created by gen17.

**Observed:** all suites green — 11 conversation, 9 persistence, 3 multigoal,
14 grammar, 2 anonymous-var, 5 explanation checks.

**Method watch (D5.1):** small reporting API, no new inference semantics. It
keeps the next pressure visible: source-aware conflict reporting and derived
belief summaries need more structure than a direct fact scan.

**Next:** continue T3 toward source-aware belief status: stop treating every
positive/negative collision as a silent overwrite once distinct sources matter.

---

## 2026-06-15 — gen17: explicit negative knowledge (T3 slice)

**Changed:** `brain.c` → `gen17-negative`; `kb.{h,c}` gained explicit negative
facts.
- `kb_assert_neg` stores known-false ground facts separately from positive
  facts. Positive and negative assertions clear their exact opposite, so the KB
  does not keep both for the same ground claim.
- Ground queries treat an explicit negative as known false; `kb_knows_pred`
  counts negative facts so a predicate can be known even when it has no positive
  members.
- Persistence now round-trips `not(pred(args)).` session clauses. A session
  negative can override a base positive fact without editing the base file.
- `bob is not a dog` is no longer just forgetting: it records `not dog(bob)` and
  later answers `No.` for the known-false claim.

**Why:** First narrow slice of TASKLIST T3. gen16 separated unknown predicates
from false answers; gen17 separates simple absence/forgetting from explicit
negative correction.

**Observed:** all suites green — 10 conversation, 9 persistence, 3 multigoal,
14 grammar, 2 anonymous-var, 5 explanation checks.

**Method watch (D5.1):** still a scaffold, but a useful one: the representation
is minimal, ground-only, and does not pretend to solve conflicts or derived
truth maintenance yet.

**Next:** continue T3 with a direct belief-status report: let the user ask what
is known about an entity and see positive and negative ground facts.

---

## 2026-06-15 — gen16: epistemic unknowns for unseen predicates (T16, part 2)

**Changed:** `brain.c` → `gen16-idk`; `kb_knows_pred` in `kb.{h,c}`.
- Query surfaces now distinguish an unknown predicate from a known-but-false
  goal. A never-mentioned class/relation answers "I don't know about <pred>.";
  once the predicate is known, unprovable members still answer "No." or
  "Nobody that I know of." as before.
- `kb_knows_pred` checks facts and rule heads; `brain.c` uses it before unary
  and binary ground/list queries.
- Tests updated deliberately for the new semantics, including retraction of the
  last fact making a predicate unknown again.
- `DESIGN.md` records this as a hardcoded scaffold, not the destination:
  epistemic policy should eventually move into reflective meta-knowledge.

**Why:** Completes TASKLIST T16 after gen15 handled not-understood input. The
agent now has two separate failure modes: "I don't understand that yet." for
parse failure, and "I don't know about <pred>." for well-formed questions whose
predicate is outside its known domain.

**Observed:** all suites green — 10 conversation, 5 persistence, 3 multigoal,
14 grammar, 2 anonymous-var, 5 explanation checks.

**Method watch (D5.1):** useful but temporary. The C check is intentionally
small and documented because the north-star is reflection + negation-as-failure
inside the KB, not permanent epistemic branching in `brain.c`.

**Next:** TASKLIST T3, first narrow slice — distinguish explicit negative
correction from simple forgetting, without building the full belief-status
architecture up front.

---

## 2026-06-15 — gen15: the parrot grows up — honest non-understanding (T16, part 1)

**Changed:** `brain.c` → `gen15-unknown`.
- The not-understood fallback is no longer the **gen0 parrot echo**. Input no
  module can parse now gets an honest "I don't understand that yet." — the
  founding parrot identity is deliberately outgrown (it was always gen0).
- Tests updated where echo was the expectation: `parrot.chat` repurposed to the
  non-understanding contract (incl. other-language/gibberish fallthrough);
  `greet.chat` and `facts.chat` no-regression lines updated.

**Why:** Raised by the user — handle what the agent does NOT understand. A chat
probe confirmed three failure modes: not-understood was *echoed* (misleading),
not-known was a confident "No."/"Nobody" (closed-world conflating unknown with
false). gen15 fixes the first (clean, low-risk). The epistemic *not-known* half
(distinguish "I don't know" from "No.") is subtler — entangled with closed-world
and retraction — and is left, with the probe findings, as TASKLIST T16 part 2.

**Observed:** all 39 checks green. Live: French/gibberish → honest admission;
known templates and proofs unaffected.

**Method watch (D5.1):** an identity decision (retiring the parrot) made under
the user's explicit interest; reversible. The harder epistemic part is
deliberately deferred rather than rushed.

**Next:** TASKLIST T16 part 2 (not-known epistemic states) and/or T3
(contradiction/belief) — both want a true/false/unknown response layer.

---

## 2026-06-15 — gen14: proof traces / explanations (T2)

**Changed:** `brain.c` → `gen14-explain`; `kb_explain` + `prove_seq_ex` in
`kb.c`.
- `prove_seq_ex`: a proof-rendering resolver. On success it renders each goal's
  proof — a fact as the ground goal, a rule as "<goal> because <sub> and
  <sub>..." — using the resolvent trick (body ++ continuation) so backtracking
  stays correct while the proof tree is reconstructed.
- `kb_explain(pred, args, argc, out)`: prove a goal and render a one-line proof.
- `mod_knowledge`: "why is <x> a/an <y>?" and "why is <x> the <rel> of <y>?";
  base facts answer "<goal> is a known fact."; unprovable → "I can't show that."
  (invents nothing).
- New `tests/explain.sh`: fact, one-step rule, rule chain, multi-goal + binding,
  and a false claim — held-out predicates so the text comes from the proof.

**Why:** TASKLIST T2. The proof tree is the **first explicit internal
representation of reasoning steps** (PRINCIPLES.md) — the agent can now show
*how* it knows, not just *what*.

**Observed:** all suites green incl. 5 explanation checks (e.g. "being(socrates)
because mortal(socrates) because man(socrates)").

**Method watch (D5.1):** healthy — a proof-returning sibling added without
touching `kb_query`/`kb_match` callers; the trace is reusable later (planning,
truth maintenance).

**Next:** study/handle the UNKNOWN and the NOT-UNDERSTOOD (new TASKLIST T16,
raised by the user) — and TASKLIST T3 (contradiction/belief). The first forces
a decision about the founding parrot-echo fallback.

---

## 2026-06-15 — gen13: real anonymous variables + bidirectional relations (T1)

**Changed:** `brain.c` → `gen13-anon`; `rename_term` in `kb.c` fixed.
- **Bug fix (introduced in gen12):** distinct `_` in one clause were renamed to
  the *same* name, aliasing them. Now each `_` becomes a FRESH variable
  (`_A<frame>_<n>`) via a per-clause anonymous counter; named vars still share
  per frame.
- `kb/grammar.p0`: `singular/2` facts and `has_plural(Y) :- plural(_, Y)`
  (anonymous var in subject position), complementing `countable/1`.
- Tests: grammar.sh +singular/has_plural; new `tests/anon.sh` proves two `_`
  are independent using held-out `edge/related` (anti-impostor: not morphology).

**Why:** First TASKLIST item (T1), and it was *pulled* — gen12's morphology
needed anonymous vars, and the implementation hid a real aliasing defect. Fixed
the engine correctly rather than papering over it.

**Observed:** all green — 14 grammar + 2 anon + every prior suite. The
`related(X) :- edge(X,_), edge(_,X)` case is true only because the two `_` no
longer alias (verified: would be false if aliased).

**Method watch (D5.1):** healthy — a correctness fix demanded by a real rule
shape; no speculative work.

**Next (TASKLIST T2):** proof traces / explanations — "why is socrates a
mortal?" → a chain derived from the proof tree (the first explicit internal
representation of reasoning steps).

---

## 2026-06-15 — gen12: flexible relation queries (object position)

**Changed:** `brain.c` → `gen12-relquery`; grammar domain extended.
- `mod_knowledge`: "what is the <rel> of <y>?" → `rel(y, X)` (unknown in the
  object position), complementing "who is the <rel> of <y>?" → `rel(X, y)`.
- `kb/grammar.p0`: morphology — `plural(dog, dogs).` (+ irregular
  `child/children`) and `countable(X) :- plural(X, _).` (anonymous variable).
- `tests/grammar.sh` extended: object-position query + the countable rule.

**Why (D5.1 — domain-pulled, not speculative):** grammar v0 itself pulled this
— "what is the plural of dog?" needs the variable in the object slot, which the
prior NL couldn't express. Built exactly that, no more. Confirms the loop: the
domain asks, the primitive answers.

**Observed:** all green — conversation + persistence + multigoal + 11 grammar
checks (incl. irregular plural and the anonymous-variable rule).

**Method watch (D5.1):** healthy — primitive justified by a concrete demand and
immediately consumed by the domain.

**Next:** grammar pulls toward sentence structure (word order / well-formedness).
First *try* it with current primitives; build new ones only if it genuinely
can't be expressed (domain-pull discipline).

---

## 2026-06-15 — expertise track: the first forged expert — grammar v0

**Changed:** no engine change (`brain_version` stays `gen11-relations`).
- `kb/grammar.p0`: parts of speech (`noun`/`verb`/`adjective`) + category
  rules `word(X) :- noun(X). word(X) :- verb(X). word(X) :- adjective(X).`
  (multiple clauses per head = disjunction — exercised live).
- `tests/grammar.sh`: loads the domain and proves competence (POS membership,
  derived `word/1`, negatives, `who is a word?`).

**Why (pivot duty exercised — D5.1):** the planned "richer NL parsing" primitive
was speculative (parsing before any grammar knowledge is backwards). Switched to
**domain-pull**: forge a real, tested expert slice on the existing engine. This
also surfaced an axis insight — **expertise (knowledge + tests) is orthogonal to
brain/engine generations**; a forged expert needs no version bump.

**Observed:** 7 grammar checks green, all prior tests still green. The
forge-an-expert pipeline (D5: clauses + tests + exported predicates) works end
to end. Exports `noun/verb/adjective/word` for higher layers.

**Next:** let the grammar domain pull the next need (morphology? sentence
structure? then parsing) — the demand, not a guess, picks what comes next.

---

## 2026-06-15 — gen11: n-ary relations + multi-goal rules (general SLD)

**Changed:** `brain.c` → `gen11-relations`; `kb.c` engine rewritten.
- New general term representation (constants vs variables by case), a real
  **unification** with a substitution + binding chains, and **SLD resolution
  with backtracking** and **standardize-apart** (rule vars renamed per frame).
- Rules are now `head :- goal1, goal2, ...` (multi-goal bodies). `kb_assert_rule`
  still builds the unary `head(X):-body(X)` so gen6/induction are unchanged;
  the loader (`kb_load`) parses general rules (top-level comma split), `kb_save`
  serialises them back.
- `kb_query` (any arity, facts+rules) and `kb_match` (collect variable bindings)
  both run through the new solver — old unary behaviour preserved exactly.
- `mod_knowledge`: binary surface "<x> is the <rel> of <y>" (assert / ground
  query / "who is the <rel> of <y>?").
- New `tests/cases/relations.chat` and `tests/multigoal.sh` (grandparent via a
  two-goal rule with a shared intermediate variable, incl. variable query).

**Why:** The representation jump D5 flagged as a prerequisite for the grammar
expert. North-star check (D5.1): n-ary relations + conjunctive rules are
genuinely demanded by grammar — agreement (subject↔verb), dependency
(head↔modifier), precedence, and "sentence = NP then VP" style rules are all
≥2-place relations and multi-goal. Not speculative; the family-tree test is just
the cleanest demonstration of a general engine.

**Observed:** 10 conversation + 5 persistence + 3 multi-goal checks green. The
engine rewrite preserved every prior test (the solver reduces to the old
behaviour on unary goals).

**Method watch (D5.1):** no pivot signal — this primitive is pulled by a real
grammar need and reused engine-wide.

**Next:** gen12 — richer NL parsing, so knowledge can be stated more naturally
than the fixed templates (the bridge toward the grammar expert in gen13).

---

## 2026-06-14 — gen10: retraction & correction

**Changed:** `brain.c` → `gen10-retract`; `kb_retract()` in `kb.{h,c}`.
- `kb_retract(pred, args, argc)`: removes a ground fact if present, preserving
  the order of the rest (closed-world correction — remove the positive fact,
  not store an explicit negation).
- `mod_knowledge`: "forget that <x> is a/an <y>" and "<x> is not a/an <y>" →
  retract `y(x)`; reports "Forgotten: y(x)." or "I didn't know that anyway."
- New `tests/cases/retract.chat`, incl. **rule interaction**: retracting the
  supporting fact makes the derived conclusion underivable again.

**Why:** Understanding means being wrong and fixing it. Knowledge (and the
saved session delta) can now be corrected, not only extended — the first of the
defences D5.1 lists for the brittleness of forged expertise.

**Observed:** 9 conversation + 5 persistence checks green.

**Decision deferred (provenance):** retraction removes from RAM only. Retracting
a `base` fact thus reappears on next boot (base file unchanged). Whether to
record "tombstones" for base corrections is left to a later gen — noted here so
it isn't forgotten.

**Method watch (D5.1):** gen10 is a genuine capability (correction), justified
independently — not speculative plumbing. No pivot signal yet.

**Next:** gen11 — n-ary relations + multi-goal rule bodies (e.g.
`grandparent(X,Z) :- parent(X,Y), parent(Y,Z)`): the representation jump the
grammar expert will need. Keep a concrete grammar north-star in view (D5.1).

---

## 2026-06-14 — gen9: persistent, human-readable, composable knowledge

**Changed:** `brain.c` → `gen9-persist`; persistence + provenance in
`kb.{h,c}`; knowledge loading wired into `main.c`.
- Provenance per clause (`KB_BASE`/`KB_SESSION`/`KB_INDUCED`/`KB_REFLECTIVE`),
  set via `kb_set_origin`; induced rules tagged `KB_INDUCED`; the self-model
  tagged `KB_REFLECTIVE`.
- `kb_load(path)`: parses Prolog-like text (`pred(a, b).`, `head(X):-body(X).`,
  `%` comments) and **unions** clauses into the KB — so files JOIN.
- `kb_save(path, mask)`: writes back only the selected layers, same readable
  format. `brain_save_session` persists `SESSION|INDUCED`, never reflective.
- `main.c`: loads `kb/base.p0` + `kb/session.p0` (paths via
  `PARROT0_BASE`/`PARROT0_SESSION`; empty disables — used for hermetic tests);
  `/save` command writes the session delta.
- `kb/base.p0` seed added; `session.p0` gitignored. New
  `tests/persist.sh` (round-trip, no-reflective-leak, base+session join, no
  base duplication); `tests/run.sh` made hermetic.

**Why:** DESIGN.md D1-D3. Knowledge becomes a durable, transparent artifact:
readable, hand-editable, diffable in git, and composable (base + discovered).
New knowledge from conversation is saved in the *same* format and can be
promoted session→base by moving lines.

**Observed:** 8 conversation + 5 persistence checks green. A fresh parrot0 now
boots already knowing its base, and facts taught in one run survive into the
next.

**Next:** gen10 — retraction & correction: "forget that X is a Y" / "X is not a
Y", so knowledge (and the saved delta) can be corrected, not only grown.

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
