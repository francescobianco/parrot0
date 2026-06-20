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

# --- reasoning skills: assert the procedural knowledge they carry, spoken ---
expect "deduce: knows modus ponens" \
    "what is modus_ponens" \
    "skills/reasoning/deduce.p0" \
    "modus_ponens is if A is true and A implies B, then B is true."

expect "compare: knows the complexity dimension" \
    "what is complexity" \
    "skills/reasoning/compare.p0" \
    "complexity is number of parts or steps — simpler, more complex."

# --- communication skills ---
expect "explain_comm: knows the definition method" \
    "what is definition" \
    "skills/communication/explain.p0" \
    "definition is state what the concept is in one clear sentence."

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
