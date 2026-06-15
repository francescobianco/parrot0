# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen44 — a syntax-agnostic claim frame (word order is not meaning)

gen43 made multilingualism a standing generalization probe (Decision
D-2026-06-15p): each gen mirrors its competence in Italian through the same core
via a thin function-word lexicon. The probe immediately earned a finding it was
built to find: the reasoning core keys on English token *positions*, so a
language whose syntax differs breaks even when every word is mapped.

The sharp case: negation. English "x is not a y" → the parser checks
`w[1]=="is" && w[2]=="not"`. Italian is "x non è un y", which canonicalizes to
"x not is a y" — same words, different order — and the position check rejects it.
Adding a second position check for the Italian order would be exactly the
phrasebook LOOP.md forbids. The honest fix is to stop treating word order as
meaning.

### Design question
Can claim parsing recognize the *roles* (subject, copula, polarity, class)
rather than fixed positions — so "x is not a y" and "x not is a y" (and other
orderings) reduce to the same `not y(x)`? Candidate: a small role-tagging pass
that classifies each token (copula `is`, polarity `not`, article, else
content/subject/class by position relative to the copula), then builds the claim
from roles. Keep it minimal — cover the affirmative and negative unary claim
first, the shapes already tested.

### Acceptance
- `tests/cases/retract.it.chat` (or a negation `.it.chat`) proves "x non è un y"
  asserts `not y(x)`, through the same core, with NO new position-based branch
  per language.
- All existing English cases (incl. negation in `retract.chat`/`belief.chat`)
  behave identically.
- The role frame is shared: adding the next language's negation needs only its
  function words in the lexicon, not new parsing logic.

### Notes
- This is the core becoming *syntax-agnostic* where it was English-bound — the
  deeper structure the probe is pulling toward (PRINCIPLES.md: structure appears
  because the task demanded it).
- Scope discipline: do unary affirmative + negative claims first. Relations,
  quantities, questions come later, one probe-driven step at a time.
- Do not hardcode outputs; do not add a per-language parser. Extend the frame.
