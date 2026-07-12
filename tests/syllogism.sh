#!/usr/bin/env bash
#
# gen326 (TODO.md P5 / TASKLIST C8) — the UNIVERSAL conclusion.
#
# What I got wrong first, and it is the point of this file. TODO.md P5 said "a
# nonce-word syllogism walls" and I started building a syllogism module. parrot0
# has had one since gen231: premises inside the turn, invented vocabulary,
# multi-link chains, honest refusals — all of it already worked. I nearly shipped
# a SECOND reasoner alongside the first, which is exactly the duplication
# PRINCIPLES.md forbids.
#
# The real gap was one question form. gen231 could only resolve a GROUND
# conclusion ("is socrates a mortal?"). Asked the UNIVERSAL one the same premises
# entail —
#
#   if all bloops are razzies and all razzies are lazzies, are all bloops lazzies?
#
# — it walled, because there is no individual to look up. So a universal is now
# proved the way a universal is actually proved: assert an ARBITRARY WITNESS of
# the subject class into the scratch KB the core already builds, and ask the
# SAME query path about it. Nothing distinguishes the witness, so what holds for
# it holds for all. One helper, both surfaces (packed and multi-sentence); no new
# module, no second inference path.
#
# C8's anti-impostor clause: "use fresh nonsense predicates each test run; the
# capability must be structural, not lexical." So the classes are GENERATED per
# run — a phrasebook cannot pass this file twice.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "syllogism: binary not built ($BIN)" >&2; exit 1; }

pass=0
fail=0
ok() { echo "PASS syllogism: $1"; pass=$((pass+1)); }
no() { echo "FAIL syllogism: $1" >&2; fail=$((fail+1)); }

ask() { printf '%s\n' "$@" | PARROT0_BASE= PARROT0_SESSION= PARROT0_WORLD_FACTS=0 \
        PARROT0_LANG=en "$BIN" 2>/dev/null; }

# Fresh nonce classes every run. Plural is the bare -s form, so singularize()
# maps them back unambiguously (an "-ies" ending would legitimately singularize
# to "-y", which is English, not a bug).
nonce() {
    local cons=(b d f g k l m n p r t v z) vow=(a e i o u)
    printf '%s%s%s%s%s' \
        "${cons[RANDOM % 13]}" "${vow[RANDOM % 5]}" \
        "${cons[RANDOM % 13]}" "${vow[RANDOM % 5]}" "${cons[RANDOM % 13]}"
}
A="$(nonce)"; B="$(nonce)"; C="$(nonce)"; D="$(nonce)"
echo "syllogism: this run's invented classes: $A $B $C (unrelated: $D)"

# ---- 1. the universal conclusion, packed form (the wall this closes) ---------
got="$(ask "if all ${A}s are ${B}s and all ${B}s are ${C}s, are all ${A}s ${C}s?")"
if [ "$got" = "Yes." ]; then
    ok "a UNIVERSAL conclusion is derived from a one-turn chain"
else
    no "the universal conclusion did not derive: $got"
fi

# ---- 2. the same, in the multi-sentence surface form (gen290's shape) --------
# One helper serves both surfaces: there is no second code path to drift.
got="$(ask "all ${A}s are ${B}s. all ${B}s are ${C}s. are all ${A}s ${C}s?")"
if [ "$got" = "Yes." ]; then
    ok "the universal conclusion reaches the multi-sentence form too"
else
    no "the multi-sentence universal did not derive: $got"
fi

# ---- 3. ANTI-IMPOSTOR: the same shape with the chain cut ---------------------
# A module that matched the sentence pattern instead of reasoning would say Yes.
got="$(ask "if all ${A}s are ${B}s and all ${D}s are ${C}s, are all ${A}s ${C}s?")"
if [ "$got" = "No." ]; then
    ok "a broken chain is refused, not rubber-stamped (same shape, one link cut)"
else
    no "it claimed a conclusion that does not follow: $got"
fi

# ---- 4. the GROUND conclusion (gen231) must not regress ----------------------
got="$(ask "if all ${A}s are ${B}s and zorb is a ${A}, is zorb a ${B}?")"
if [ "$got" = "Yes." ]; then
    ok "the ground conclusion still works (gen231 untouched)"
else
    no "the ground syllogism regressed: $got"
fi

# ---- 5. ZERO POLLUTION: the invented world must not survive the turn ---------
# TASKLIST C8: facts introduced for a question may be used for that answer and
# must pollute nothing. The premises live in a scratch KB destroyed with the turn.
got="$(ask "if all ${A}s are ${B}s and all ${B}s are ${C}s, are all ${A}s ${C}s?" \
           "what do you know about $A?" \
           "how many facts do you know?")"
if printf '%s' "$got" | grep -q "I don't know anything about $A" \
   && printf '%s' "$got" | grep -q "I know 0 fact(s)"; then
    ok "the invented world leaves NO trace in the live KB"
else
    no "the nonce vocabulary leaked into the KB: $got"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
