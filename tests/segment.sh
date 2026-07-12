#!/usr/bin/env bash
#
# gen330 (TODO.md P0 / items 01 and 02) — a prompt is not a source file.
#
# The counterexample, on the real gen328/gen329 binary:
#
#   you> fix this code: int absval(int x) { if (x < 0) { return -x; } return x; }
#        It should return the absolute value but it returns a negative number for x = -5
#   parrot0> Fix: add a semicolon at the end of each statement.
#
# The C is flawless. The missing semicolon was in the ENGLISH — find_code_section()
# defined the code as "everything after the first colon", so the user's description
# of the bug was compiled as part of the program. cc rejected the paragraph, the
# syntax veto therefore never engaged, and a hand-rolled scanner invented a defect
# in correct code. Asked about a semantic bug, parrot0 answered with a fabricated
# syntactic one: the impostor failure, in the faculty that is supposed to be the
# most grounded.
#
# This file is the honesty sweep TODO 02 asks for, and it has three halves that
# must ALL hold — any one alone is trivially gameable:
#
#   A. NO FALSE POSITIVES. Correct code, wrapped in every mixed EN/IT layout we
#      could think of, must never produce a syntax finding. (A checker that says
#      nothing would pass A alone.)
#   B. NO LOST TRUE POSITIVES. Really broken snippets must still be caught, by the
#      compiler. (A checker that says nothing fails B — which is why A and B are
#      tested together.)
#   C. AMBIGUITY IS DECLARED. A paste whose braces never close gets
#      `ambiguous_input`, not a diagnosis of a program we had to guess the shape of.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "segment: binary not built ($BIN)" >&2; exit 1; }
cd "$ROOT" || exit 1

pass=0
fail=0
ok() { echo "PASS segment: $1"; pass=$((pass+1)); }
no() { echo "FAIL segment: $1" >&2; fail=$((fail+1)); }

ask() { printf '%s\n' "$1" | PARROT0_SESSION= "$BIN" 2>/dev/null; }

# A guard against the trap this file fell into while it was being written: in
# hermetic mode parrot0 prints no banner, so an `ask` that sliced line 2 returned
# the EMPTY STRING for every prompt — and the twenty "no false positive" checks all
# passed, on nothing. A test whose assertions are satisfied by silence is not a
# test. So: silence is a failure, and we check that before anything else.
probe="$(ask 'fix this code: int f(int x) { return x }')"
[ -n "$probe" ] || { echo "FAIL segment: the harness itself is broken (empty answer)" >&2; exit 1; }

# A syntax FINDING is a CLAIM ABOUT THE CODE. Note what is deliberately not in
# this pattern: the word "brace" on its own. The honest decline ("its braces never
# close, so I cannot tell where the program ends") contains it, and a decline is
# not a finding — matching it would have made parrot0's honesty look like a
# fabrication. The distinction this whole generation is about applies to the test
# that measures it, too.
syntax_claim() {
    printf '%s' "$1" | grep -qiE "semicolon|unclosed string|unbalanced|paren|compiler rejects|issue found"
}

# ---- A. twenty mixed layouts over CORRECT code: no fabricated syntax finding ----
#
# The code in each is valid C. The prose around it is what used to get compiled.
GOOD='int absval(int x) { if (x < 0) { return -x; } return x; }'
GOOD2='int add(int a, int b) { return a + b; }'
GOOD3='int sum(int n) { int s = 0; for (int i = 1; i <= n; i++) { s += i; } return s; }'

declare -a LAYOUTS=(
  "fix this code: $GOOD It should return the absolute value but it returns a negative number for x = -5"
  "fix this code: $GOOD The expected output is 5 for x = -5"
  "what is wrong with this: $GOOD it gives the wrong answer on negatives"
  "what is wrong with this code: $GOOD2 it should add but the result is off by one"
  "fix this code: $GOOD2 Expected: add(2,3) = 5. Observed: 6."
  "fix this code: $GOOD3 the sum of 1..n is wrong for n = 0"
  "can you fix this: $GOOD it must handle INT_MIN as well"
  "please fix this code: $GOOD2 without using recursion"
  "sistema questo codice: $GOOD Dovrebbe restituire il valore assoluto ma per x = -5 restituisce un numero negativo"
  "correggi questo codice: $GOOD2 Il risultato atteso e 5 ma ottengo 6"
  "cosa c'e che non va in questo codice: $GOOD3 la somma e sbagliata per n = 0"
  "sistema questo codice: $GOOD2 senza usare la ricorsione"
  "fix this code: $GOOD This is used in a hot loop, so it must not allocate."
  "fix this code: $GOOD2 Note: the caller passes negative values too."
  "what is wrong with this: $GOOD3 It should return 15 for n = 5. It returns 10."
  "fix this code: $GOOD The bug appeared after we changed the sign handling."
  "fix this code: $GOOD2 It works for positive numbers. It fails for zero."
  "what is wrong with this code: $GOOD Constraint: do not use the abs() function."
  "fix this code: $GOOD3 Expected 15, got 10, for n = 5. Do not change the signature."
  "correggi questo codice: $GOOD3 Deve restituire 15 per n = 5, invece restituisce 10."
)

i=0
for L in "${LAYOUTS[@]}"; do
    i=$((i+1))
    got="$(ask "$L")"
    if syntax_claim "$got"; then
        no "layout $i FABRICATED a syntax finding on correct code: $got"
    else
        ok "layout $i: no fabricated syntax finding"
    fi
done

# ---- B. the same checkers must still CATCH real defects ----------------------
#
# If A passed because the module fell silent, these fail. That is the point.
declare -a BROKEN=(
  "fix this code: int f(int x) { return x }"
  "fix this code: int g(void) { int y = 1 return y; }"
  "what is wrong with this: int x = 5"
  "fix this code: int h(int a) { if (a > 0 { return 1; } return 0; }"
)
j=0
for B in "${BROKEN[@]}"; do
    j=$((j+1))
    got="$(ask "$B")"
    if syntax_claim "$got"; then
        ok "broken $j is still caught by the compiler"
    else
        no "broken $j was MISSED (the honesty gate went too far): $got"
    fi
done

# ...and the true positive survives even when the same prose is wrapped around it:
# the defect is in the CODE span, and segmentation must not hide it.
got="$(ask 'fix this code: int f(int x) { return x } It should return x but it does not compile')"
syntax_claim "$got" \
    && ok "a real defect inside a mixed prompt is still found" \
    || no "segmentation hid a real defect: $got"

# ---- C. an unclosed paste is DECLARED ambiguous, never diagnosed -------------
got="$(ask 'fix this code: int f(int x) { if (x) { return 1; }')"
printf '%s' "$got" | grep -q "ambiguous_input" \
    && ok "a paste whose braces never close is ambiguous_input" \
    || no "an unclosed paste was not declared ambiguous: $got"
syntax_claim "$got" && no "the ambiguous paste was ALSO diagnosed: $got" \
                    || ok "and it is not diagnosed at the same time"

# ---- the prose is not silently swallowed: prove the span was found -----------
# `explain` on a mixed prompt must describe the CODE, not the sentence after it.
got="$(ask "explain what this does: $GOOD3 It should return 15 for n = 5")"
printf '%s' "$got" | grep -qi "loop\|return\|C code" \
    && ok "explain reads the code span, not the trailing prose" \
    || no "explain lost the code span: $got"

echo "segment: $pass passed, $fail failed"
[ "$fail" -eq 0 ]