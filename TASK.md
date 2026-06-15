# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen32 — the missing bridge: a minimal text → facts extractor

The gen28–gen31 domain-pull run reached one conclusion four times: parrot0 now
has the *reasoning* primitives the first SuperGLUE questions need (magnitude,
3-way entailment, cause/effect, coreference), but every task still scores 0
because nothing turns a natural-language passage into the `pred(args)` facts
those primitives consume. Input, not inference, is the wall. This is the
deferred investment all four iterations named — now the goal.

### Design question
What is the smallest *honest* extractor that lifts explicit, well-formed
statements out of a multi-sentence passage into KB facts — without pretending
to do open-domain understanding?

Candidate: a sentence splitter + a pass that feeds each clause to the existing
`mod_knowledge` / `mod_quantity` / `mod_cause` parsers, asserting whatever
parses and ignoring (counting) what does not. Then a benchmark prompt can be
fed whole: extract from the passage, answer from the question. Start with
synthetic passages built only from already-parseable sentence shapes; real
SuperGLUE prose stays mostly unparseable and that must show honestly as a low,
non-zero extraction rate — never a hardcoded answer.

### Acceptance
- A passage of several known-shape sentences populates the KB with the right
  facts; unparseable sentences are skipped and counted, not invented.
- A question answered after extraction matches what the same facts would give
  if asserted one per turn.
- Held-out content proves it is extraction, not memorization.
- Honest reporting: extraction coverage on real prose is measured, not faked.

### Notes
- Discovery tooling stays (`PARROT0_TRACE` + `--max-examples 1`).
- This is the line we kept refusing to cross by faking; cross it by *building*
  a real (if minimal) extractor, never by hardcoding benchmark labels.
  See JOURNAL "Decisions" for the provisional choices this may force.
