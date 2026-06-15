# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen53 — C3: natural assertion + personal memory ("listen to me")

Conversational-intelligence pivot. felt-intelligence (`make chat-bench`) has
climbed 36% (gen50) → 64% (gen51, C1 intent) → 82% (gen52, C2 dialogue acts).
The remaining benchmark MISSes are the turns that make a bot feel like it is
*listening*: "I have a dog named Rex" then "what is my dog called?" → "Rex"
(C3), and "what did we just talk about?" (C4).

This is C3: parse first-person / natural-shape facts about the user and answer
from them.

### Design question
Can the existing memory + KB carry first-person possessions/attributes —
"I have a dog named Rex", "my dog is Rex", "call me Sam", "I like jazz" — as
relations (e.g. `has(user, dog)`, `named(dog, rex)` or `my(dog, rex)`), so
"what is my dog called?" resolves from them? Keep it structural: a small set of
possessive/naming patterns mapped to relations, not canned Q→A pairs.

### Acceptance
- "I have a dog named Rex" (and "my dog is Rex") remembered; "what is my dog
  called?" / "what is the name of my dog?" → "Rex".
- "call me Sam" then "what is my name?" distinguishes the USER's name from the
  bot's identity (gen51).
- Held-out possessions/attributes and varied phrasing work (cue/role based, not
  fixed strings); the dog turns in `tests/chat/intro.dlg` flip to OK.
- English + Italian per the bilingual ratchet.

### Notes
- Builds on the memory module and overlaps T6 (working memory). Watch the
  first-person/second-person distinction (my vs your) — pronoun roles matter.
- Then C4 (discourse memory: "what did we just talk about?") and C5 (retire the
  blank wall) close out the first felt-intelligence pass.
