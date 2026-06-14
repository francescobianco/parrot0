#!/usr/bin/env bash
#
# Grammar expert v0 (DESIGN.md D5): the first forged domain. Loads
# knowledge/grammar.pl and proves competence — parts of speech, the derived
# word/1 category (via disjunctive rules), negatives, and a variable query.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
GRAMMAR="$ROOT/knowledge/grammar.pl"

if [ ! -x "$BIN" ]; then
    echo "grammar: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
expect() { # <description> <input> <expected first stdout line>
    local got
    got="$(printf '%s\n/quit\n' "$2" \
          | PARROT0_BASE="$GRAMMAR" PARROT0_SESSION= "$BIN" 2>/dev/null | head -1)"
    if [ "$got" = "$3" ]; then echo "PASS grammar: $1"; pass=$((pass+1))
    else echo "FAIL grammar: $1 — want [$3] got [$got]" >&2; fail=$((fail+1)); fi
}

expect "noun membership"        "is dog a noun?"   "Yes."
expect "verb membership"        "is run a verb?"   "Yes."
expect "POS is exclusive"       "is dog a verb?"   "No."
expect "derived word (noun)"    "is dog a word?"   "Yes."
expect "derived word (verb)"    "is run a word?"   "Yes."
expect "non-word"               "is dog a banana?" "No."
expect "all words (disjunction)" "who is a word?" \
       "dog, cat, language, run, speak, red, quick."

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
