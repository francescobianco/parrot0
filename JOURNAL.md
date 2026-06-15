# parrot0 evolution journal

Newest entries on top. One entry per iteration of the loop (see LOOP.md).

## Decisions (revisitable)

> Choices made under uncertainty that a future iteration may overturn. Each
> records what taking that road **bought** us, what we **gave up** by taking it,
> and the **revisit-if** signal that should send us back to change it. Newest on
> top. These are explicitly provisional — not commitments.

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
- `knowledge/grammar.pl`: `singular/2` facts and `has_plural(Y) :- plural(_, Y)`
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
- `knowledge/grammar.pl`: morphology — `plural(dog, dogs).` (+ irregular
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
- `knowledge/grammar.pl`: parts of speech (`noun`/`verb`/`adjective`) + category
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
- `main.c`: loads `knowledge/base.pl` + `knowledge/session.pl` (paths via
  `PARROT0_BASE`/`PARROT0_SESSION`; empty disables — used for hermetic tests);
  `/save` command writes the session delta.
- `knowledge/base.pl` seed added; `session.pl` gitignored. New
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