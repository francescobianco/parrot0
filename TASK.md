# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

> gen67 (M2 step 1, measurable book learning shift) is DONE — see JOURNAL.
> gen68 (meta-conversation wall: attention/reading/understanding/repetition) is DONE — see JOURNAL.
> gen65 symbolic challenge remains parked as the S-series in TASKLIST.md.

## Goal: C6 step 2 — answer simple polar meta-questions without pretending

Latest chat-sim transcripts and gen68 showed that parrot0 can now answer a
small set of meta-conversation questions from local state: attention, reading,
understanding limits, current activity, and repetition. The next wall is broader
polar conversational questions that are not facts about the world and should not
fall into the knowledge fallback.

### Design question
Can parrot0 classify a small yes/no conversational act such as "are you there?",
"can you hear me?", "are you a bot?", or Italian mirrors, and answer from its
self/conversation state without claiming human senses or open-domain knowledge?

### Acceptance
- Add held-out English and Italian cases for presence/channel questions.
- Answers must be honest and state-grounded: e.g. text input is available, audio
  hearing is not, identity comes from `i_am(parrot0)`.
- Avoid duplicating content logic in an Italian-only branch; extend cues or the
  canonicalization layer only where the same module owns both languages.
- `make test` must pass.

### Notes
- M2 propositional book learning remains parked until the multi-word-entity /
  open-prose extraction wall is addressed.
- The next chat-sim round should measure whether these meta/casual walls are
  actually reduced before expanding the class further.
