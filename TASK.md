# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

> gen65 (symbolic-register recognition) is DONE — see JOURNAL. The whole
> SYMBOLIC challenge is parked as the S-series in TASKLIST.md (owner asked to
> resume it later: S2 decoding, S3 cryptic-token registers + multi-line intake).

## Goal: gen66 — fallback word-pick must be grounded (don't deny what you know)

(chat-sim finding) gen64 made the capability intent robust to chat-register
shorthand ("what can u do?" via `u`→`you`) and the Italian "che puoi fare?"
variant. The fresh transcript exposes the next clean gap: the not-understood
fallback reflects a "salient content word" ("Hmm, I don't know about X yet.")
but picks words it should never disclaim —
- its OWN NAME ("Hmm, I don't know about parrot0 yet.") while it demonstrably
  knows `i_am(parrot0)`; a self-contradiction;
- Italian function words ("stai", "parli", "basta") that leak past the
  English-only `is_stopword`.

### Design question
The fallback's word reflection is a *claim about self-knowledge*. Before
asserting ignorance of word X it should consult the KB / self-model: skip X if
it is the agent's own name (`i_am`) or a known subject, and skip mere function
words in any *supported* language (the bilingual probe shows `is_stopword` is
English-only — extend it, don't duplicate). If no genuinely-unknown content
word remains, fall back to a neutral non-repeating admission.

### Acceptance
- After the agent has its birth self-model, a turn whose only salient word is
  "parrot0" must NOT produce "...I don't know about parrot0 yet."
- An Italian turn like "ma stai scherzando?" must not pick "stai" as the
  unknown word.
- `make test` stays green; add cases capturing both.

---

## Parked: M2 step 1 — learning from books (linguistic track)

M2: demonstrate that parrot0 has *learned* from reading a book or passage.

M2 has two honest tracks:
- (a) **Linguistic/distributional** — already possible: `read:` induces the
  continuation model (`cont`/`cont2`) from prose, so reading book A vs book B
  changes what `say` produces. This generation makes that learning *measurable*
  and *held-out*.
- (b) **Propositional** — gated by the open-prose extraction wall; left for a
  later generation.

This generation targets track (a).

### Design question
How can we prove, with a test, that reading a passage changes the induced
language model in the expected direction? Approach: a new `tests/booklearn.sh`
that loads a hermetic passage, runs `say <seed>`, checks that the continuation
matches the passage; then loads a *different* passage, runs the same `say`, and
checks that the continuation shifted. Both passages are short, test-committed
prose so the result is deterministic and reproducible.

### Acceptance
- After `read: the otter swims downstream. the otter swims often.`, `say the`
  produces "the otter swims ..." (already true; this generation locks it as a
  regression test).
- After a second `read:` with a disjoint passage (`the robot walks slowly. the
  robot walks daily.`), `say the` produces "the robot walks ..." instead.
- Add `tests/booklearn.sh` to `make test`.

### Notes
- Track (b) remains parked until the multi-word-entity / open-prose extraction
  wall is addressed.
- If another `make chat-sim` round is requested, the next improvement should
  probably target generic yes/no/casual questions or better wellbeing handling
  (e.g. "what are you up to?"), because the latest transcript shows the
  remaining wall-rate is dominated by those, not by mixed-act hijacks.
