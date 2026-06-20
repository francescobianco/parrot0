#!/usr/bin/env bash
# gen150: verify skill files provide correct procedural knowledge.
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

# --- explain skill: provides code analysis capability ---
expect "explain: debug works with explain" \
    "what is wrong with this code: int x = \"hello\";" \
    "skills/explain.p0" \
    "Issue found: Type mismatch: a string is assigned to an int variable. Use char * or char[] for strings."

# --- fix skill: provides fix suggestions ---
expect "fix: semicolon fix" \
    "fix this code: int x = 5" \
    "skills/fix.p0" \
    "Fix: add a semicolon at the end of each statement."

# --- generate skill: loads code generation targets ---
expect "generate: debug still works" \
    "what is wrong with this code: int x = \"hello\";" \
    "skills/generate.p0" \
    "Issue found: Type mismatch: a string is assigned to an int variable. Use char * or char[] for strings."

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
