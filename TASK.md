# Current task

> One goal at a time. When it is done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions. TASKLIST.md is the longer proving ground.

## Goal: gen54 — M1 step 2: pipelines + oracle-grounded output

Mission M1 (POSIX/bash) is underway. gen53 gave compositional single-command
understanding ("ls -la" composed from ls + l + a) from committed
`knowledge/bash.pl`, case-sensitive and honest on unknowns (`tests/posix.sh`).

Two natural next steps, both deepening COMPOSITION and bringing in the
deterministic shell ORACLE:

1. **Pipelines.** "what does `cat f | grep x` do?" composes the two commands'
   effects: "prints file contents, then keeps the lines matching a pattern."
   Parse on `|`, describe each stage from the gen53 `cmd`/`flag` facts, join.
2. **Oracle-grounded output prediction.** For pure deterministic commands
   (start with `echo`), predict the actual output and VERIFY it by running the
   real shell in the test — the purest embodiment of the oracle (no authored
   description, truth is computed and checked).

### Acceptance
- A pipeline never stored verbatim is explained by composing its stages
   (held-out); the order is preserved.
- "what is the output of echo hello world?" → "hello world", and the test
   confirms it equals the real `echo hello world`.
- Existing gen53 behaviour and all suites unchanged; new cases in
   `tests/posix.sh`; knowledge stays in committed `knowledge/bash.pl`.

### Notes
- Keep it structural and oracle-checkable; do not author per-command outputs.
- C-series (conversational intelligence) is parked at gen52/C2 with C3 (personal
  memory: "dog named Rex") next when we return to it — felt-intelligence 82%.
- Later M1: intent → command ("how would you do this in POSIX?"), and
  multi-file knowledge loading so `make chat` sees bash.pl by default.
