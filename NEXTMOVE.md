# NEXTMOVE — resume point

> Handoff note to pick up exactly where we stopped. Read **PROMPT.md** (the
> standing directive) and **PRINCIPLES.md** (the why) first; this file is the
> *where we are + what's next*.

Last updated: 2026-06-21. Branch: `main` (all work committed + pushed).
Current generation: **gen155** (`brain_version()` -> `gen155-similarity-recall`).
Build/test: `make test` is GREEN (157 chat cases + all `.sh` suites).

## How to run

```sh
make            # build bin/parrot0  (header deps are tracked now — gen152)
make test       # full suite: hermetic .chat cases + KB-loaded .sh suites
make chat       # interactive, loads the agi profile (kb/profiles/agi.p0)
# probe a single profile:
printf 'what is the longest bone in the body\n/quit\n' \
  | PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0
```
NOTE: `tests/cases/*.chat` run HERMETIC (`PARROT0_BASE=` empty) — they do NOT
load `kb/`. Knowledge behavior is tested in the KB-loading `.sh` suites
(`tests/knowledge.sh`, `experts.sh`, `profiles.sh`, `skills.sh`).

## What this session did (gen151–gen155, all pushed)

The owner flagged the gen150/150b expert/skill/profile knowledge as *fatta male*:
loaded but inert. The audit found it violated PRINCIPLES.md three ways and the
fixes followed:

| gen | what |
|-----|------|
| 151 | Knowledge SPEAKS: `render_fact_direct` verbalizes `pred(key,"desc")`; broadened "what is X / tell me about X" intake. No more raw Prolog dumps. |
| 152 | Engine holds STRING LITERALS: quote-aware `parse_term` (commas inside quotes are content), `KB_TERM_LEN` 64->128. ~280 silently-dropped/mangled facts now load. Also fixed a Makefile ABI footgun (objects now depend on `$(HDR)`). |
| 153 | Restored hand-truncated descriptions in math/medicine/programming; rewrote PLACEBO tests (`is socrates a man -> Yes`) into real domain assertions. |
| 154 | Reorganized `kb/`: new `kb/core/` for boot substrate (base, session, lexicon, gloss, social, roles); domain files into `experts/`. No loose top-level files. |
| 155 | Concept recall by STRUCTURAL SIMILARITY (`kb_nearest_concept`): cognate-tolerant overlap, recall-by-description, cross-lingual (IT "circonferenza/diametro" -> pi). The first brick of a similarity space. |

## The thread we're on: closing the LLM gap

The root gap vs an LLM is a **continuous similarity space**: parrot0 looks up
discrete symbols; every unanticipated phrasing hits the wall. gen155 is the first
deterministic-C brick of that space (overlap-based concept recall). See the
gen155 JOURNAL entry for the full assessment.

### Immediate next pulls (smallest-first, pick ONE, drive from a held-out test)

1. **idf-like weighting for `kb_nearest_concept`** (`src/kb.c`). The 10x stress
   showed discrete overlap is noisy at agi scale: common words ("number",
   "blood", "system") cause ties/abstentions and loose cognates can mislead.
   Weight each query word by inverse frequency across concept descriptions so
   discriminative words dominate. Held-out test: "the bone that protects the
   brain" should resolve to **skull** (currently a 2-2 skull/skeletal tie ->
   abstains). Keep the hedge + margin; do NOT turn it into a phrasebook.
2. **Bilingual descriptions** so *semantic* (not only cognate) recall crosses
   languages — the real blocker for IT. This is also a knowledge-data task.
3. Then continue widening intent-matching beyond knowledge recall (the same
   similarity idea applied to MODULE dispatch, not just concept lookup) — the
   biggest long-term lever, but design it carefully against the no-phrasebook
   discipline.

## Outstanding debt from the KB audit (not yet done)

- **Truncated descriptions** still cut mid-word in the secondary domains: under
  `kb/experts/{physics,chemistry,biology,linguistics,business,philosophy,music,
  psychology}/` and most of `kb/skills/`. The engine can hold full text now
  (gen152), so they just need restoring. Find them:
  ```sh
  grep -rnoE '"[^"]{56,63}"' kb/   # candidates near the OLD 64 cap; read each,
                                   # the truncated ones end mid-word
  ```
  Done so far: math (arithmetic, algebra), medicine (anatomy, pharmacology),
  programming (algo, c, debug).
- **Malformed fact**: `flag(ls, u_r, descending_into_subdirectories)` in
  `kb/experts/programming/shell.p0` — no such `ls` flag (should be `R`). Left
  untouched to avoid perturbing the posix tests; fix deliberately with a test
  check.
- Duplicate concept across files: `binary_search` is described in both
  `experts/programming/algo.p0` (algorithm/3) and `skills/programming/explain.p0`
  (concept/2). Harmless (complementary facets), noted.

## Key code pointers

- Similarity recall: `kb_nearest_concept` + `word_sim` + `concept_tokens` in
  `src/kb.c` (just below `kb_describe_entity`); wired in `mod_knowledge`
  (`src/brain.c`, the "what is X / tell me about X" block — exact key wins, fuzzy
  is the fallback).
- Description rendering: `render_fact_direct` in `src/kb.c` (verbalizes a quoted
  LAST arg). `is_struct_pred` there filters structural-metadata predicates out of
  descriptions.
- Module registry + dispatch: bottom of `src/brain.c` (`registry[]`,
  `brain_respond`). `mod_knowledge` runs LATE, so broadening its intake is safe.
- Boot KB loads: `main.c` (base/session via env, then
  `kb/experts/programming/coding.p0`); `brain_create` in `brain.c` (lexicon,
  social, roles, gloss from `kb/core/`).

## LOOP discipline reminders (LOOP.md)

Every generation: smallest tested step; bump `brain_version()` if behavior
changed; add an EN ratchet AND the IT equivalent through the SAME path (extend
`canonicalize_lang`, never duplicate logic); run a ~10x stress; append a dated
JOURNAL.md entry (newest at top); commit+push with
`convcommit -t <type> -s <scope> -m "genN - summary" -a -p`. Tests are the
ratchet; emergence over design; stay pure C.
