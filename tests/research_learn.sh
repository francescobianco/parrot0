#!/usr/bin/env bash
# Offline regression for the in-C dynamic learner used by mod_research.
set -eu

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TMP="${TMPDIR:-/tmp}/parrot0-research-learn.$$"
trap 'rm -rf "$TMP"' EXIT

mkdir -p "$TMP/pages"
cat >"$TMP/pages/foo.md" <<'EOMD'
# Foo

- Domain: `testing`

## Learned Concept

small deterministic test concept

## Extract

Foo is a fixture used by the parrot0 research learner test.
EOMD

out="$(
    PARROT0_BASE= PARROT0_SESSION= \
    PARROT0_WIKI_DIR="$TMP/pages" \
    PARROT0_LEARN_KB="$TMP/learned.p0" \
    "$ROOT/bin/parrot0" 2>/dev/null <<'EOIN'
tell me about foo
EOIN
)"

test "$out" = "I didn't know about foo, so I just read it up: small deterministic test concept."
grep -F 'wiki_concept(foo, testing, "small deterministic test concept").' "$TMP/learned.p0" >/dev/null

echo "PASS research_learn: in-C learner reads local markdown and persists learned KB"
