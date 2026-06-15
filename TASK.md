# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen36 — decide the next pivot (extraction coverage vs generative loop)

Two 4-iteration domain-pull runs (gen28–gen35) built eleven cooperating parts
and confirmed the standing wall: open-prose extraction coverage. Two candidate
next directions are on the table — pick one before coding gen36:

1. **Raise extraction coverage** (continue the proven road): add a few more
   real sentence shapes / light paraphrase handling to `mod_reader`'s parser
   chain so `read:` lifts more of actual SuperGLUE prose, measured honestly by
   the skipped count. Low risk, incremental.
2. **Generative inference loop** (DESIGN D-prop1, proposed by F.): pivot toward
   autoregressive generation via repeated inference. Higher value (most
   structurally LLM-like mechanism yet) but a large pivot; start with the
   smallest tested decode loop over one continuation relation, single-shot path
   kept beside it.

### Notes
- Discovery tooling stays (`PARROT0_TRACE` + `--max-examples 1`).
- If (2) is chosen, the crux to protect against is the phrasebook impostor: the
  continuation relation must be derived from real KB state, not hand-authored.
- Do not hardcode benchmark answers. See JOURNAL "Decisions" for the
  provisional choices the recent gens may force a revisit of.
