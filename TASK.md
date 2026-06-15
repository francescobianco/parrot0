# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen52 — C2: social register (the phatic layer)

Conversational-intelligence pivot continues. gen50 built C0 (`make chat-bench`,
baseline 36%); gen51 (C1) made identity/capability paraphrase-robust → 64%. The
benchmark still MISSes the social turns that shape a first impression: "how are
you?", "thank you", greeting variants, and (later) personal memory.

This is C2 (TASKLIST): handle the phatic layer gracefully, in plain language,
via the same cue approach — not canned scripts, kept minimal and honest.

### Acceptance
- "how are you?" → a friendly, honest status (not the wall).
- "thank you" / "thanks" → a brief acknowledgement.
- Greeting variants beyond "hi/hello/hey" land (e.g. "hey there", "good
  morning"); farewell variants too.
- `make chat-bench` climbs (the "how are you?"/"thank you" turns in intro.dlg
  flip to OK); held-out phrasings pass; English + Italian cues per the ratchet.

### Notes
- No phrasebook: cue-based intent + a short honest response, not a list of
  accepted sentences. Watch for cue collisions (Decision D-2026-06-15u).
- Then C3 (natural assertion + personal memory: "I have a dog named Rex" →
  "what is my dog called?" → "Rex") and C4 (discourse memory) — the remaining
  MISSes on the benchmark.
