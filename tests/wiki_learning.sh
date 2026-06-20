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
