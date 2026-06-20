#!/usr/bin/env bash
# gen150b: verify expert domain files load correctly.
# Covers programming, mathematics, and medicine domains.
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "experts: binary not built" >&2; exit 1; }

pass=0 fail=0

expect_profile() { # desc input profile expected
    local got
    got="$(printf '%s\n/quit\n' "$2" \
          | PARROT0_BASE="$ROOT/kb/base.p0" PARROT0_SESSION= PARROT0_PROFILE="$ROOT/kb/$3" "$BIN" 2>/dev/null | head -1)"
    if [ "$got" = "$4" ]; then echo "PASS expert: $1"; pass=$((pass+1))
    else echo "FAIL expert: $1 — want [$4] got [$got]" >&2; fail=$((fail+1)); fi
}

# --- programming experts ---
expect_profile "c: language detected" \
    "what language is this code: int x" \
    "experts/programming/c.p0" \
    "This looks like C code."

expect_profile "py: language detected" \
    "what language is this code: def foo(): pass" \
    "experts/programming/python.p0" \
    "This looks like Python code."

expect_profile "shell: cmd effect" \
    "what does ls do?" \
    "experts/programming/shell.p0" \
    "ls lists directory contents."

expect_profile "shell: composed flags" \
    "what does ls -la do?" \
    "experts/programming/shell.p0" \
    "ls lists directory contents, in long format, including hidden entries."

expect_profile "debug: fix exists" \
    "fix this code: int x = \"hello\"" \
    "experts/programming/debug.p0" \
    "Fix: add a semicolon at the end of each statement."

# --- mathematics experts: assert the actual DOMAIN KNOWLEDGE, spoken (gen151+),
#     not the placebo "is socrates a man -> Yes" that any profile passes. ---
expect_profile "arithmetic: defines addition" \
    "what is addition" \
    "experts/mathematics/arithmetic.p0" \
    "addition is combining two numbers to get their sum."

expect_profile "algebra: defines variable" \
    "what is variable" \
    "experts/mathematics/algebra.p0" \
    "variable is a symbol representing an unknown value."

expect_profile "geometry: states pythagorean theorem" \
    "what is pythagorean" \
    "experts/mathematics/geometry.p0" \
    "pythagorean is in a right triangle, a^2 + b^2 = c^2."

# --- medicine experts ---
expect_profile "anatomy: defines the heart" \
    "what is the heart" \
    "experts/medicine/anatomy.p0" \
    "heart is muscular pump circulating blood through the body."

expect_profile "pharmacology: defines half_life" \
    "what is half_life" \
    "experts/medicine/pharmacology.p0" \
    "half_life is time for the drug concentration in blood to halve."

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
