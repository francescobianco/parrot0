# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen51 — paraphrase-robust intent (start killing the "I don't understand" wall)

PIVOT (in progress). The SuperGLUE bench reads 46% with zero invalid
(gen45–gen49) but that is single-turn classification + shallow baselines — in
real chat parrot0 feels immediately unintelligent. gen50 built C0, the
felt-intelligence conversation benchmark (`make chat-bench`); honest baseline
**36% (4/11)**. The loop now optimizes that number, not the bench.

This is C1 (TASKLIST), the highest-impact climb: the same intent must be
reachable from many phrasings, not one rigid template. Today "who are you?" works
but "what is your name?" / "what should I call you?" hit the wall.

### Design question
Can intent be matched by ROLE/KEYWORDS rather than exact token positions —
generalizing the gen44 "roles over order" lesson — so a small set of identity
and capability phrasings all resolve to the same answer, derived from the real
self-model (not canned strings)?

### Acceptance
- Identity intent: "who are you?", "what is your name?", "what should I call
  you?", "what are you?" all answer from the self-model (`i_am(parrot0)`).
- Capability intent: "what can you do?" answers in plain language (capabilities,
  not the `module(...)` jargon list).
- Held-out phrasings of these intents (not in any test) also resolve — proof it
  is keyword/role recognition, not enumerated strings.
- A `tests/cases/*.chat` (and, once C0 exists, a dialogue case) locks it in;
  English + Italian per the bilingual ratchet.

### Notes
- Measure every step with `make chat-bench` (C0, built gen50). The identity and
  capability turns in `tests/chat/intro.dlg` should flip from MISS to OK.
- No phrasebook: robustness from keyword/role matching + the KB, never a list of
  fixed accepted sentences. Add held-out phrasings to a sibling `.dlg` to prove
  it generalizes.
- Then C2 (social register) and C3 (natural assertion + personal memory) — the
  next felt-intelligence wins.
