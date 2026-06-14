# Current task

> One goal at a time. When it's done, replace this with the next one.
> See LOOP.md for how to work a task, PRINCIPLES.md for why, DESIGN.md for
> architectural decisions.

## Goal: gen9 — persistent, human-readable, composable knowledge

Today the KB is session-only: it lives in RAM and dies at `/quit`. Give parrot0
durable knowledge as **human-readable Prolog-like text** (DESIGN.md D1–D3),
loaded fully into RAM, composable from layers, with correct save.

### Idea
- A text format that round-trips: `man(socrates).` and `mortal(X) :- man(X).`,
  one clause per line, comments with `%` ignored.
- `kb_load(path)`: parse a file and union its clauses into the KB (so multiple
  files **join** — e.g. a base file + a session file).
- `kb_save(path, which)`: serialise clauses back to text. Provenance per clause
  (`base` / `session` / `induced` / `reflective`); save writes only the chosen
  layer (default: `session` + `induced`), and NEVER the `reflective` self-model.
- Wire into the app: load `knowledge/base.pl` (+ `knowledge/session.pl` if
  present) at startup; a `/save` command (or save on `/quit`) writes the session
  delta back. Keep `main.c` changes minimal.

### Acceptance
- A fact taught in one run is present in the next run (after save), proven by a
  test that runs the binary twice against a temp knowledge file.
- Loading base + session yields the union (a "join" test).
- Saving never duplicates base clauses and never writes reflective facts.
- All existing tests still pass; new `tests/cases/*` (or a small script) covers
  load/save round-trip.
- Bump `brain_version()` to `gen9-...`.

### Notes
- Parser stays small but forgiving (trim, skip blanks/comments); reject
  malformed lines without crashing.
- Defer any indexing (DESIGN.md D4) — this task is about persistence, not speed.
