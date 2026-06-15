# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen55 — C5a: kill the broken record

gen54 built chatsim (`make chat-sim`): a cheap opencode-GO model role-plays a
mutable human and chats with parrot0. It exposed the dominant naturalness killer:
parrot0 repeats "I don't understand that yet." VERBATIM — wall rate ~88%,
immediate-repetition ~77% — and the simulated users call it out ("broken
record", "are you even trying"). This is the highest-impact fix.

### Design question
Without faking understanding, can the fallback stop being a robotic constant?
Structural approach (not a phrasebook): the brain tracks its LAST reply and never
repeats the fallback verbatim; it rotates a few honest non-understanding MOVES
that REDIRECT or invite ("I didn't catch that — can you say it another way?",
"I'm not sure I followed. What would you like to know?"), and where possible
REFLECTS a content word from the user's message so the reply feels heard.

### Acceptance
- The same fallback is never emitted twice in a row; consecutive unparseable
  inputs get different, honest, non-understanding responses.
- Where the message has a salient content word, the fallback reflects it
  ("Hmm, I don't know about <word> yet.").
- chatsim repetition-rate drops materially vs the gen54 baseline (~77%); the wall
  no longer dominates verbatim. Add a deterministic `tests/cases/*.chat` locking
  the no-verbatim-repeat behaviour; English + Italian.
- Still honest: it must not pretend to understand; it admits the gap, just not
  robotically.

### Notes
- Needs a tiny bit of brain state (last response). Keep it minimal.
- Then C2b (mixed-act turns), then C3/C4 (memory) — the remaining chatsim gaps.
- Parked: M1 step 2 (shell pipelines + oracle output) from the POSIX arc.
