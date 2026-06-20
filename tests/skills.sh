#!/usr/bin/env bash
# gen150b: verify skill files provide correct procedural knowledge.
# Covers programming, reasoning, and communication domains.
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "skills: binary not built" >&2; exit 1; }

pass=0 fail=0

expect() { # desc input profile expected
    local got
    got="$(printf '%s\n/quit\n' "$2" \
          | PARROT0_BASE="$ROOT/kb/base.p0" PARROT0_SESSION= PARROT0_PROFILE="$ROOT/kb/$3" "$BIN" 2>/dev/null | head -1)"
    if [ "$got" = "$4" ]; then echo "PASS skill: $1"; pass=$((pass+1))
    else echo "FAIL skill: $1 — want [$4] got [$got]" >&2; fail=$((fail+1)); fi
}

# --- programming skills ---
expect "explain: debug works" \
    "what is wrong with this code: int x = \"hello\";" \
    "skills/programming/explain.p0" \
    "Issue found: Type mismatch: a string is assigned to an int variable. Use char * or char[] for strings."

expect "fix: semicolon fix" \
    "fix this code: int x = 5" \
    "skills/programming/fix.p0" \
    "Fix: add a semicolon at the end of each statement."

expect "generate: debug works" \
    "what is wrong with this code: int x = \"hello\";" \
    "skills/programming/generate.p0" \
    "Issue found: Type mismatch: a string is assigned to an int variable. Use char * or char[] for strings."

# --- reasoning skills ---
expect "deduce: loads" \
    "is socrates a man" \
    "skills/reasoning/deduce.p0" \
    "Yes."

expect "compare: loads" \
    "is socrates a man" \
    "skills/reasoning/compare.p0" \
    "Yes."

# --- communication skills ---
expect "explain_comm: loads" \
    "is socrates a man" \
    "skills/communication/explain.p0" \
    "Yes."

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
