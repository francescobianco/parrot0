#!/usr/bin/env bash
# gen150b: verify profile files load and chain experts/skills via :- include.
# Tests profiles across programming, science, and agi (all domains).
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "profiles: binary not built" >&2; exit 1; }

pass=0 fail=0

expect() { # desc input profile expected
    local got
    got="$(printf '%s\n/quit\n' "$2" \
          | PARROT0_BASE="$ROOT/kb/base.p0" PARROT0_SESSION= PARROT0_PROFILE="$ROOT/kb/$3" "$BIN" 2>/dev/null | head -1)"
    if [ "$got" = "$4" ]; then echo "PASS profile: $1"; pass=$((pass+1))
    else echo "FAIL profile: $1 — want [$4] got [$got]" >&2; fail=$((fail+1)); fi
}

# --- programming/coder ---
expect "coder: C code recognized" \
    "what language is this code: int x;" \
    "profiles/programming/coder.p0" \
    "This looks like C code."

expect "coder: Python code recognized" \
    "what language is this code: def foo(): pass" \
    "profiles/programming/coder.p0" \
    "This looks like Python code."

expect "coder: shell command via include" \
    "what does ls do?" \
    "profiles/programming/coder.p0" \
    "ls lists directory contents."

expect "coder: debug query" \
    "what is wrong with this code: int x = \"hello\";" \
    "profiles/programming/coder.p0" \
    "Issue found: Type mismatch: a string is assigned to an int variable. Use char * or char[] for strings."

# --- programming/mentor ---
expect "mentor: C code" \
    "what language is this code: int x;" \
    "profiles/programming/mentor.p0" \
    "This looks like C code."

# --- programming/debugger ---
expect "debugger: debug query" \
    "what is wrong with this code: int x = \"hello\";" \
    "profiles/programming/debugger.p0" \
    "Issue found: Type mismatch: a string is assigned to an int variable. Use char * or char[] for strings."

# --- programming/reviewer ---
expect "reviewer: C code" \
    "what language is this code: int x;" \
    "profiles/programming/reviewer.p0" \
    "This looks like C code."

# --- science/mathematician ---
expect "mathematician: loads without error" \
    "is socrates a man" \
    "profiles/science/mathematician.p0" \
    "Yes."

# --- science/doctor ---
expect "doctor: loads without error" \
    "is socrates a man" \
    "profiles/science/doctor.p0" \
    "Yes."

# --- agi: all domains ---
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
