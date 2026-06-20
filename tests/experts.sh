#!/usr/bin/env bash
# gen150: verify expert domain files load correctly and provide expected facts.
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "experts: binary not built" >&2; exit 1; }

pass=0 fail=0

expect() { # desc input expected
    local got
    got="$(printf '%s\n/quit\n' "$2" \
          | PARROT0_BASE="$ROOT/kb/base.p0" PARROT0_SESSION= PARROT0_PROFILE= "$BIN" 2>/dev/null | head -1)"
    if [ "$got" = "$3" ]; then echo "PASS expert: $1"; pass=$((pass+1))
    else echo "FAIL expert: $1 — want [$3] got [$got]" >&2; fail=$((fail+1)); fi
}

expect_profile() { # desc input profile expected
    local got
    got="$(printf '%s\n/quit\n' "$2" \
          | PARROT0_BASE="$ROOT/kb/base.p0" PARROT0_SESSION= PARROT0_PROFILE="$ROOT/kb/$3" "$BIN" 2>/dev/null | head -1)"
    if [ "$got" = "$4" ]; then echo "PASS expert: $1"; pass=$((pass+1))
    else echo "FAIL expert: $1 — want [$4] got [$got]" >&2; fail=$((fail+1)); fi
}

# --- C expert loads and works ---
expect_profile "c: language detected" \
    "what language is this code: int x" \
    "experts/c.p0" \
    "This looks like C code."

# --- Python expert loads and works ---
expect_profile "py: language detected" \
    "what language is this code: def foo(): pass" \
    "experts/python.p0" \
    "This looks like Python code."

# --- Shell expert loads and works ---
expect_profile "shell: cmd effect" \
    "what does ls do?" \
    "experts/shell.p0" \
    "ls lists directory contents."

expect_profile "shell: composed flags" \
    "what does ls -la do?" \
    "experts/shell.p0" \
    "ls lists directory contents, in long format, including hidden entries."

# --- Debug expert loads ---
expect_profile "debug: error fix exists" \
    "fix this code: int x = \"hello\"" \
    "experts/debug.p0" \
    "Fix: add a semicolon at the end of each statement."

# --- Algo expert loads ---
expect_profile "algo: facts loaded" \
    "what language is this code: int x" \
    "experts/algo.p0" \
    "This looks like C code."

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
