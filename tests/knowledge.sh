#!/usr/bin/env bash
# gen151: the gen150 expert/skill knowledge must be SPOKEN, not dumped as raw
# Prolog. These tests query the domain knowledge conversationally and assert a
# natural sentence — proving the description facts verbalize through one general
# path (no per-concept phrasebook) and that no clause leaks as "pred(arg, ...)".
# EN + IT exercise the SAME render path, the bilingual ratchet (LOOP.md step 6).
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "knowledge: binary not built" >&2; exit 1; }

pass=0 fail=0

expect() { # desc input profile expected
    local got
    got="$(printf '%s\n/quit\n' "$2" \
          | PARROT0_BASE="$ROOT/kb/base.p0" PARROT0_SESSION= PARROT0_PROFILE="$ROOT/kb/$3" "$BIN" 2>/dev/null | head -1)"
    if [ "$got" = "$4" ]; then echo "PASS knowledge: $1"; pass=$((pass+1))
    else echo "FAIL knowledge: $1 — want [$4] got [$got]" >&2; fail=$((fail+1)); fi
}

# --- mathematics: concepts speak, not dump ---
expect "arithmetic: what is addition" \
    "what is addition" \
    "experts/mathematics/arithmetic.p0" \
    "addition is combining two numbers to get their sum."

expect "arithmetic: bare concept (no article)" \
    "what is prime" \
    "experts/mathematics/arithmetic.p0" \
    "prime is a number greater than 1 divisible only by 1 and itself."

# --- medicine: definite article + multiword topic reach the concept ---
expect "anatomy: what is the heart" \
    "what is the heart" \
    "experts/medicine/anatomy.p0" \
    "heart is muscular pump circulating blood through the body."

expect "anatomy: what is the femur" \
    "what is the femur" \
    "experts/medicine/anatomy.p0" \
    "femur is thigh bone -- longest and strongest bone in the body."

expect "anatomy: tell me about the brain" \
    "tell me about the brain" \
    "experts/medicine/anatomy.p0" \
    "brain is control center processing sensory information and directing r."

# --- bilingual ratchet: the IT question hits the SAME render path ---
expect "IT: cos'e addition" \
    "cos'è addition" \
    "experts/mathematics/arithmetic.p0" \
    "addition is combining two numbers to get their sum."

expect "IT: cos'e multiplication" \
    "cos'è multiplication" \
    "experts/mathematics/arithmetic.p0" \
    "multiplication is repeated addition of a number."

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
