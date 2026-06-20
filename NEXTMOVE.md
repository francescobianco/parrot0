# NEXTMOVE — resume point

> Handoff to resume cleanly. Read **PROMPT.md** (standing directive) and
> **PRINCIPLES.md** (the why) first. Delete this file once its plan is folded
> into the JOURNAL / done.

Last updated: 2026-06-21. Branch: `main`, all committed + pushed.
HEAD: `gen159` (`brain_version()` -> `gen159-typed-relations`).
Build/test: `make test` GREEN (157 chat cases + all `.sh` suites; `knowledge.sh`
has 22 cases). Working tree is CLEAN — nothing half-done.

## How to run
```sh
make            # build (headers tracked since gen152)
make test       # full suite
make chat       # interactive, loads kb/profiles/agi.p0
printf 'is the heart part of the circulatory system\n/quit\n' \
  | PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0
```
Hermetic `tests/cases/*.chat` run with `PARROT0_BASE=` (NO kb/). Knowledge
behavior is tested in the KB-loading `.sh` suites (knowledge/experts/profiles/
skills). kb/ is now organized: `kb/core/` (boot substrate), `kb/experts/`,
`kb/skills/`, `kb/profiles/` — no loose top-level files.

## >>> RESUME HERE: gen160 — multi-hop part_of chains (IN PROGRESS, not started in code)

The session built a derived, typed knowledge graph (gen155–159, see below). The
graph is currently ONE level deep: organ -> system (heart -> circulatory). The
next ambitious step the owner approved is to **enrich the KB with a higher level
so `part_of` chains transitively** through the resolution engine:
`heart -> circulatory -> human_body`.

I had fully designed it before stopping. Three changes, in order:

### 1. Enrich `kb/experts/medicine/anatomy.p0` with the organism level
Add (after the body-systems section), naming the 8 system keys exactly:
```
% --- the organism: the level above the systems ---
organism(human_body, "the circulatory, respiratory, nervous, digestive, skeletal, muscular, immune and endocrine systems").
```
- KEY must be `human_body`, NOT `body` and NOT `organism`-as-key. Reason: `body`
  collides (it appears in many descriptions like "longest bone in the body", and
  as a concept key it would hijack the gen155 femur similarity test). `human_body`
  is distinctive, stays one token through `split_words` (underscores are kept —
  `half_life` works), and appears in no other description, so no false mentions.
- Predicate is `organism` (≠ `body_system`), so the gen159 TYPE GATE
  (`valid_member`: member and container must have different predicates) lets
  `part_of(circulatory, human_body)` through while still blocking sibling junk.

### 2. Fix the container threshold in `kb_derive_part_of` (`src/kb.c`)
Currently a predicate is a "container" only if **>=2 of its FACTS** name another
concept key (`pcnt[p]++` once per fact). A single high-level `organism` fact names
8 systems but is only ONE fact -> would fail. Change to count **total mentions**:
in the per-predicate loop (around line 1231–1243, the `names_other`/`pcnt[p]++`
block), count how many valid keys each fact names and do `pcnt[p] += kcount`
(threshold stays `>= 2`). Verify number_property stays NON-container (composite
names prime = 1 mention < 2) and body_system / organism become containers. After
the change, scan what got materialized to be sure no junk predicate crossed the
threshold:
```sh
# quick check: print derived part_of by instrumenting, or just probe queries
```

### 3. Add the transitivity rule (binary, 3 vars) — load it from a file
The `kb_assert_rule*` C API is UNARY-only; it CANNOT express
`part_of(X,Z) :- part_of(X,Y), part_of(Y,Z)`. But the gen11 resolver + the `.p0`
loader handle general n-ary multi-var rules (cf. the grandparent rule in T2). So
add this line to `kb/core/base.p0`:
```
part_of(X, Z) :- part_of(X, Y), part_of(Y, Z).
```
base.p0 is loaded by `make chat` and the `.sh` suites (PARROT0_BASE=
kb/core/base.p0); hermetic `.chat` tests don't load base (harmless there). The
materialized `part_of` facts + this rule make `kb_query` chain.

### Expected results to ratchet (in tests/knowledge.sh, anatomy profile)
- `is the heart part of human_body` -> Yes  (CHAINED: heart->circulatory->human_body via the rule + kb_query)
- `is circulatory part of human_body` -> Yes  (direct)
- `what is part of human_body` -> "human_body contains: circulatory, respiratory, nervous, digestive, skeletal, muscular, immune, endocrine."  (kb_match is facts-only = direct members; that's fine)
- `is the heart part of the digestive system` -> still No  (no path; transitivity must not create false chains)

### Watchouts
- Recursion: the transitivity rule is recursive; the resolver has KB_MAX_DEPTH=64.
  The graph is shallow (2 hops) so Yes-queries resolve fast; No-queries exhaust
  but are bounded. If a query loops or is slow, reconsider.
- `kb_match` ("what is part of Y", the members query in mod_knowledge) is likely
  facts-only (no rules) — so members stays DIRECT (systems for human_body, organs
  for a system). That's the intended behavior; don't "fix" it.
- The relational query parser lives in `mod_knowledge` (src/brain.c, the gen157/
  gen158 block ~line 1317+). It already skips category nouns (system/group/...);
  `human_body` is a plain key so it just works. Two-key proof needs `w[0]=="is"`.

## Session arc so far (gen151–159, all pushed, green)

| gen | what |
|-----|------|
| 151 | knowledge SPEAKS — verbalize `pred(key,"desc")`, broaden "what is X" intake |
| 152 | engine holds STRING LITERALS — quote-aware parse_term, KB_TERM_LEN 64->128; ~280 dead facts recovered; Makefile header-dep ABI fix |
| 153 | restored truncated descriptions (math/medicine/programming); placebo tests -> real domain assertions |
| 154 | reorganized kb/ (kb/core + experts taxonomy, no loose files) |
| 155 | concept recall by structural SIMILARITY (cognate-tolerant, cross-lingual) |
| 156 | idf weighting — rare words dominate, breaks agi-scale ties |
| 157 | EMERGENT relations from text ("heart part of?" -> circulatory, never asserted) |
| 158 | relations as PROVABLE facts — materialize part_of/2, prove/members/container queries |
| 159 | TYPE GATE — part_of only crosses taxonomic levels; kills skeletal/muscular false positive |
| 160 | **(NEXT)** multi-hop chains — add organism level + transitivity rule |

Key code: similarity recall + relation derivation all in `src/kb.c`
(`kb_nearest_concept`, `kb_concept_mentioning`, `kb_derive_part_of`,
`concept_pred`/`valid_member`, `word_sim`/`concept_tokens`); query wiring in
`mod_knowledge` (src/brain.c). part_of materialized once on first `brain_respond`
(guard `b->relations_derived`), tagged KB_REFLECTIVE (never persisted).

## Standing debt (from the KB audit, still open)
- Truncated descriptions in the OTHER expert/skill domains: `kb/experts/{physics,
  chemistry,biology,linguistics,business,philosophy,music,psychology}/` and most
  of `kb/skills/`. The engine holds full text now (gen152), so just restore them.
  Find: `grep -rnoE '"[^"]{56,63}"' kb/` then read each (truncated ones end
  mid-word). Done: math, medicine, programming-core.
- Malformed fact `flag(ls, u_r, ...)` in `kb/experts/programming/shell.p0` (no
  such ls flag; should be R) — left untouched to avoid perturbing posix tests.

## The deeper frontier (the LLM gap, after gen160)
- **Bilingual descriptions**: cross-lingual recall (gen155) only works for
  cognates because descriptions are English. Italian/English `gloss`-style
  descriptions would let SEMANTIC (not only cognate) similarity cross languages.
- **A sliver of syntax**: the modifier-vs-head problem (gen158's skeletal case,
  worked around by the type gate) and descriptive queries that embed a concept
  name as a modifier ("the bone that protects the brain" -> wants skull, gets
  brain) need light POS/dependency signal — the recurring wall PRINCIPLES.md
  predicts: the missing learned metric/structure.

## LOOP discipline (LOOP.md)
Smallest tested step; bump `brain_version()` if behavior changed; EN ratchet AND
the IT equivalent through the SAME path (extend `canonicalize_lang`, never
duplicate logic); ~10x stress after it works; dated JOURNAL.md entry (newest at
top); commit+push with `convcommit -t <type> -s <scope> -m "genN - summary" -a -p`.
Tests are the ratchet; emergence over design; stay pure C. Use `rap` for literal
edits (not sed/perl): `rap -no-backup s [-all] FILE OLD NEW`.
