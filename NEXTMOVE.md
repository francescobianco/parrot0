# NEXTMOVE - Wikipedia learning handoff

Last updated: 2026-06-21. This file replaces the older gen160 resume note for now.
The active task is the autonomous Wikipedia learning pipeline requested today.

## User Goal

Build a deterministic self-learning workflow, without AI, that runs hourly on GitHub Actions:

1. Read a curated, organized list of Wikipedia pages.
2. Pick one page per run, round-robin.
3. Download the page as Markdown.
4. Extract acquired concepts deterministically.
5. Update `kb/` with the learned knowledge.
6. Write operational logs for the learning event.
7. Commit and push the new knowledge, Markdown snapshot, state, index, and logs.

Additional requirement added by the user: operational logs must be committed and pushed together with the learned knowledge. They are not just debug output; they are calibration data for improving the continuous learning process.

Additional semantic requirement: create a logical connection with the learning event, so after a learning run parrot0 can be prompted to extract everything learned in that session. The intended shape is an event id like `learn_2026_06_21t..._prime_number`, with KB facts linking:

- event -> concept key
- event -> domain
- event -> Markdown snapshot
- event -> operational log
- event -> Wikipedia revision/time

A prompt like `what do you know about <event_id>` should surface the session summary through the normal KB description path.

## Important Instruction

The user explicitly asked to edit local files with `rap --help` / `rap`, not `apply_patch`.
Use `rap` for remaining local edits.

Useful commands:

```sh
rap --help
rap inv list
rap inv get learn_py
rap inv path learn_py
```

## Current Worktree State

The tree is dirty and only partially prepared. Do not assume anything is finished.

Current untracked paths seen during handoff:

```text
?? .github/
?? kb/learning/
?? scripts/
?? tests/fixtures/
?? tests/wiki_learning.sh
```

Existing untracked user content before this task:

```text
kb/learning/sources.tsv
```

Do not delete `kb/learning/sources.tsv`; it already contains the curated starter page list organized by domain.

Files/directories created during the interrupted implementation:

```text
.github/workflows/wikipedia-learn.yml       # currently empty
kb/learning/learned.p0                      # currently empty or header-only depending on resume point
kb/learning/sources.tsv                     # existing source list, preserve it
scripts/learn.py                            # currently only placeholder text
/tests/fixtures/wiki/prime_number.json      # currently empty
tests/wiki_learning.sh                      # currently empty
```

`rap inv list` currently includes:

```text
codex_test
learn_py
plan_md
nextmove_wiki
```

The complete draft implementation of `scripts/learn.py` was stored in rap inventory as `learn_py`, but was not yet applied to `scripts/learn.py` when the user paused.

Inventory path observed:

```text
/home/francesco/.rap/project/-home-francesco-Develop-_-parrot0/inventory/learn_py
```

## Resume Steps

### 1. Apply the Python learner draft

`scripts/learn.py` currently has a small placeholder. Replace the whole file with the stored inventory:

```sh
rap lr scripts/learn.py 1 6 @inv:learn_py
```

If the placeholder line count changed, check it first:

```sh
wc -l scripts/learn.py
sed -n '1,20p' scripts/learn.py
```

The draft script is pure Python stdlib. It is designed to:

- read `kb/learning/sources.tsv`
- maintain `kb/learning/state.json`
- maintain `kb/learning/index.json`
- fetch Wikipedia REST summary and action API wikitext
- write Markdown snapshots under `kb/learning/pages/`
- write operational logs under `kb/learning/logs/`
- regenerate `kb/learning/learned.p0`
- create event facts in `learned.p0`
- support `--fixture` for offline tests

Expected generated KB predicates include:

```prolog
wiki_concept(prime_number, mathematics, "natural number greater than 1 that is not a product of two smaller natural numbers").
learning_event(learn_..., "learned prime_number: ...").
learning_event_time(learn_..., "2026-...").
learning_event_domain(learn_..., mathematics).
learning_event_concept(learn_..., prime_number).
learning_event_revision(learn_..., rev_123456789).
learning_event_markdown(learn_..., "kb/learning/pages/prime_number.md").
learning_event_log(learn_..., "kb/learning/logs/learn_....json").
```

This event linkage is the answer to the user's newest requirement: a session can be queried through the event entity.

### 2. Fill the workflow

Create `.github/workflows/wikipedia-learn.yml` with:

```yaml
name: Wikipedia learner

on:
  schedule:
    - cron: "17 * * * *"
  workflow_dispatch:

permissions:
  contents: write

concurrency:
  group: wikipedia-learner
  cancel-in-progress: false

jobs:
  learn:
    runs-on: ubuntu-latest
    steps:
      - name: Checkout
        uses: actions/checkout@v4
        with:
          fetch-depth: 0

      - name: Set up Python
        uses: actions/setup-python@v5
        with:
          python-version: "3.x"

      - name: Process one Wikipedia page
        run: python3 scripts/learn.py

      - name: Commit and push learning updates
        run: |
          if [ -z "$(git status --porcelain)" ]; then
            echo "No learning updates to commit."
            exit 0
          fi
          git config user.name "github-actions[bot]"
          git config user.email "41898282+github-actions[bot]@users.noreply.github.com"
          git add kb/learning
          git commit -m "learn wikipedia page"
          git push
```

Note: the workflow commits only `kb/learning` at runtime. That includes learned facts, Markdown pages, state, index, and logs. The script/workflow/test files themselves should be committed manually once implementation is complete.

### 3. Seed the generated KB file

`kb/learning/learned.p0` should start as:

```prolog
% Generated by scripts/learn.py from curated Wikipedia pages.
% Do not hand-edit generated facts; edit kb/learning/sources.tsv instead.
% No AI is used: each wiki_concept comes from a deterministic summary rule.
% Learning events connect concepts, Markdown snapshots, and operational logs.
```

After the first real or fixture run, `learn.py` will regenerate it with facts.

### 4. Add the offline fixture

Fill `tests/fixtures/wiki/prime_number.json` with:

```json
{
  "summary": {
    "content_urls": {
      "desktop": {
        "page": "https://en.wikipedia.org/wiki/Prime_number"
      }
    },
    "description": "integer greater than 1 that has no positive divisors other than 1 and itself",
    "extract": "A prime number is a natural number greater than 1 that is not a product of two smaller natural numbers. Prime numbers are central objects in number theory.",
    "pageid": 247,
    "revision": 123456789,
    "title": "Prime number"
  },
  "page": {
    "fullurl": "https://en.wikipedia.org/wiki/Prime_number",
    "pageid": 247,
    "revisions": [
      {
        "revid": 123456789,
        "timestamp": "2026-01-01T00:00:00Z"
      }
    ]
  },
  "wikitext": "{{Short description|Natural number greater than 1 with no divisors other than 1 and itself}}\n'''Prime numbers''' are studied in [[number theory]].\n\n== Properties ==\n* A prime has exactly two positive divisors.\n"
}
```

### 5. Add the offline regression test

Fill `tests/wiki_learning.sh` with:

```sh
#!/usr/bin/env bash
# Offline regression for the deterministic Wikipedia learner.
set -eu

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TMP="${TMPDIR:-/tmp}/parrot0-wiki-learning.$$"
trap 'rm -rf "$TMP"' EXIT

mkdir -p "$TMP/pages" "$TMP/logs"
cat >"$TMP/sources.tsv" <<'EOF'
# domain<TAB>title
mathematics	Prime number
EOF

python3 "$ROOT/scripts/learn.py" \
    --sources "$TMP/sources.tsv" \
    --kb "$TMP/learned.p0" \
    --state "$TMP/state.json" \
    --manifest "$TMP/index.json" \
    --pages-dir "$TMP/pages" \
    --logs-dir "$TMP/logs" \
    --source-index 0 \
    --fixture "$ROOT/tests/fixtures/wiki/prime_number.json" >/dev/null

grep -F 'wiki_concept(prime_number, mathematics, "natural number greater than 1 that is not a product of two smaller natural numbers").' "$TMP/learned.p0" >/dev/null
grep -F 'learning_event_concept(' "$TMP/learned.p0" >/dev/null
grep -F 'learning_event_log(' "$TMP/learned.p0" >/dev/null
grep -F '# Prime number' "$TMP/pages/prime_number.md" >/dev/null
grep -F '## Properties' "$TMP/pages/prime_number.md" >/dev/null
python3 -m json.tool "$TMP/state.json" >/dev/null
python3 -m json.tool "$TMP/index.json" >/dev/null
test -n "$(find "$TMP/logs" -type f -name 'learn_*.json' -print -quit)"

echo "PASS wiki_learning: deterministic Wikipedia learner"
```

Then make it executable:

```sh
chmod +x tests/wiki_learning.sh
```

### 6. Load learned knowledge in the AGI profile

In `kb/profiles/agi.p0`, after the learning skills block:

```prolog
% --- learning skills ---
:- include(../skills/learning/memorize.p0).
:- include(../skills/learning/research.p0).

% --- autonomous Wikipedia learning ---
:- include(../learning/learned.p0).
```

This makes learned `wiki_concept(...)` and `learning_event(...)` facts available when running:

```sh
PARROT0_PROFILE=kb/profiles/agi.p0 make chat
```

### 7. Add the test to Makefile

In the `test:` target, after `@./tests/booklearn.sh`, add:

```make
	@./tests/wiki_learning.sh
```

### 8. Verify locally

Run:

```sh
python3 -m py_compile scripts/learn.py
bash -n tests/wiki_learning.sh
./tests/wiki_learning.sh
make test
```

A real Wikipedia fetch can be tested with:

```sh
python3 scripts/learn.py --source-index 0
```

Network may be restricted locally. If it fails with DNS/network sandboxing, request escalation or leave the real fetch to GitHub Actions. The offline fixture test should be enough for deterministic behavior.

### 9. Inspect generated learning output after a fixture or real run

Expected files after a run:

```text
kb/learning/learned.p0
kb/learning/index.json
kb/learning/state.json
kb/learning/pages/<concept>.md
kb/learning/logs/<event_id>.json
```

Check the event linkage:

```sh
rg -n "learning_event|wiki_concept" kb/learning/learned.p0
python3 -m json.tool kb/learning/state.json
python3 -m json.tool kb/learning/index.json
```

The event id in `state.json:last_processed.event_id` is the prompt handle for the session.

Example prompt target after a real run:

```text
what do you know about learn_2026_06_21t012345z_prime_number
```

Because `learning_event(<event>, "...").` and related event facts mention the event id, `kb_describe_entity` should be able to surface the session summary and the paths to the Markdown/log files.

## Source List Already Present

`kb/learning/sources.tsv` is a curated starter list. It already contains domains such as:

- mathematics
- physics
- chemistry
- biology
- medicine
- computer_science
- astronomy

The script should preserve comments and use only non-comment rows in `<domain><TAB><Wikipedia title>` format.

## Design Notes

No AI is involved. Extraction is deterministic:

- summary comes from Wikipedia REST `page/summary`
- page text comes from MediaWiki API revisions
- Markdown conversion strips common wikitext structures with simple deterministic rules
- concept definition comes from the first summary sentence and a small title/verb pattern
- logs are JSON and committed for later calibration

Keep the pipeline idempotent where practical:

- `learned.p0` is regenerated from `index.json`
- one run appends one learning event
- one page key maps to one Markdown snapshot path
- repeated pages update the concept entry but preserve event history

## Final Commit Scope Tomorrow

When complete and verified, commit these groups together:

```text
.github/workflows/wikipedia-learn.yml
scripts/learn.py
kb/learning/sources.tsv
kb/learning/learned.p0
kb/profiles/agi.p0
Makefile
tests/wiki_learning.sh
tests/fixtures/wiki/prime_number.json
```

Do not commit local temporary test output from `/tmp`.

Do not run destructive cleanup. The tree was already dirty with untracked `kb/learning/`; treat it as user content.

## Current Status

Paused intentionally at the user's request. No final verification has been run yet. The next action is to apply `@inv:learn_py` into `scripts/learn.py`, then fill the remaining empty files with the contents above using `rap`.
