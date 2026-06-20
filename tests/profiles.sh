#!/usr/bin/env bash
# gen150: verify profile files load and chain experts/skills via :- include.
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "profiles: binary not built" >&2; exit 1; }

pass=0 fail=0

expect() { # desc input expected
    local got
    got="$(printf '%s\n/quit\n' "$2" \
          | PARROT0_BASE= PARROT0_SESSION= PARROT0_PROFILE="$ROOT/kb/$3" "$BIN" 2>/dev/null | head -1)"
    if [ "$got" = "$4" ]; then echo "PASS profile: $1"; pass=$((pass+1))
    else echo "FAIL profile: $1 — want [$4] got [$got]" >&2; fail=$((fail+1)); fi
}

# --- coder profile: should have C + Python + shell + debug + skills ---
expect "coder: C code recognized" \
    "what language is this code: int x;" \
    "profiles/coder.p0" \
    "This looks like C code."

expect "coder: Python code recognized" \
    "what language is this code: def foo(): pass" \
    "profiles/coder.p0" \
    "This looks like Python code."

expect "coder: shell command" \
    "what does ls do?" \
    "profiles/coder.p0" \
    "ls lists directory contents."

expect "coder: debug query" \
    "what is wrong with this code: int x = \"hello\";" \
    "profiles/coder.p0" \
    "Issue found: Type mismatch: a string is assigned to an int variable. Use char * or char[] for strings."

# --- mentor profile: C + Python + algo + explain ---
expect "mentor: C code" \
    "what language is this code: int x;" \
    "profiles/mentor.p0" \
    "This looks like C code."

expect "mentor: Python code" \
    "what language is this code: def foo(): pass" \
    "profiles/mentor.p0" \
    "This looks like Python code."

# --- agi profile: EVERYTHING ---
expect "agi: C code recognized" \
    "what language is this code: int x;" \
    "profiles/agi.p0" \
    "This looks like C code."

expect "agi: Python code recognized" \
    "what language is this code: def foo(): pass" \
    "profiles/agi.p0" \
    "This looks like Python code."

expect "agi: shell command" \
    "what does ls do?" \
    "profiles/agi.p0" \
    "ls lists directory contents."

expect "agi: debug query" \
    "what is wrong with this code: int x = \"hello\";" \
    "profiles/agi.p0" \
    "Issue found: Type mismatch: a string is assigned to an int variable. Use char * or char[] for strings."

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
