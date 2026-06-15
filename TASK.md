# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen65 — symbolic-REGISTER recognition (C3): name it, don't wall

A new discovery channel landed: `make sym-bench` (`tests/symbench.py`) — the
CRYPTIC-stimulus challenge. A non-reasoning oracle LLM (kimi-k2.6) and parrot0
are each fed short, OPEN-ENDED symbolic stimuli with NO checkable answer
(leetspeak, Morse, musical notes, palindromes, incomplete code, ASCII faces,
cryptic tokens). We do NOT grade correctness — the oracle is a free behavioural
signal. First run (2026-06-15): **parrot0 walls 96%**; the oracle engages ~100%
with one consistent, inducible move — it **recognizes and NAMES the register**
("a palindrome", "SOS in Morse", "solfège", "the start of a Python function",
"IPv6 loopback", "an ASCII box"), then engages lightly (plays along / asks).

The structure to extract is therefore a **register classifier over symbolic
form**, not a puzzle solver. Classify the stimulus's genre from cheap, honest
structural features and reply by naming it (and a light, varied engagement),
instead of walling. This classifies FORM — it does not paste the oracle's words.

Detectable registers (start with the highest-signal, structurally cleanest):
1. **Palindrome / symmetry** — token (or char) sequence equals its reverse
   (`abccba`, `12321`, `1 2 3 2 1`). Reply: "That looks like a palindrome." It
   is shameful that parrot0 today says "I don't know about abccba yet" — it has
   the string; symmetry is a pure structural test.
2. **Morse** — input is only `.`, `-`, and spaces. Reply: "That looks like Morse
   code." (Decoding is a later, optional step; recognition comes first.)
3. **Code fragment** — contains code punctuation/keywords (`def `, `for `, `(`,
   `{`, `==`, `SELECT`). Reply: "That looks like a snippet of code."
4. **Solfège / notes** — tokens drawn from {do,re,mi,fa,sol,la,si,ti}. Reply:
   "Those are musical notes (solfège)."
5. **Leetspeak** — a word with digit-for-letter substitutions (`h3ll0`, `1337`).
   Reply: name it as leetspeak (decoding optional later).

### Acceptance
- `abccba` and `1 2 3 2 1` → a palindrome/symmetry recognition (NOT a wall, NOT
  "I don't know about abccba yet").
- `... --- ...` → recognized as Morse.
- `def foo(` → recognized as a code fragment.
- `do re mi` → recognized as musical notes.
- Bilingual ratchet: the same recognizers fire on Italian framing where it
  applies (notes `do re mi` are already language-neutral; a palindrome is
  language-neutral) — proving the competence is structural, not English lexical.
- Plain prose ("the otter swims downstream") must NOT be hijacked by any
  recognizer.

### Notes
- Recognition before decoding: naming the register is the LLM's first move and
  the cleanest structural primitive. Decoding (Morse→text, leet→word,
  palindrome continuation) can follow in later generations if sym-bench shows it
  is worth it.
- Keep replies varied/non-repeating (reuse the non-repetition discipline from
  the fallback) so a session of cryptic inputs doesn't feel canned.

---

## Parked: gen — fallback word-pick must be grounded (don't deny what you know)

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
