#!/usr/bin/env bash
#
# gen328 (TODO.md P3 / forge plan C2, W6) — the schema is INDUCED from a page,
# not curated by hand.
#
# `algo_shape(bubblesort, nested_loop_compare_swap)` was written by a human. That
# is the growth LOOP.md calls the impostor: every new algorithm costs a
# generation, so generation n buys capability n and nothing compounds. The forge
# plan's C2 campaign states the gate exactly — "nessuno schema specifico aggiunto
# a mano; senza pagina/candidato il build non avviene; con il candidato property e
# sanitizer passano."
#
# So this file proves the causal chain, in both directions:
#
#   ABLATION   without the page, the algorithm cannot be built at all
#   INDUCTION  the page yields the schema — from what the steps DO
#   BUILD      the schema is instantiated and RUN against the sort oracle
#   HONESTY    a page describing a different structure is DECLINED, not
#              force-fitted into the only schema we happen to have
#
# The decisive test is #4: the algorithm's NAME is generated fresh each run and is
# nonsense. If induction were reading the name rather than the structure, it could
# not possibly work — and if it were force-fitting, #3 would not decline.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "learnbuild: binary not built ($BIN)" >&2; exit 1; }

pass=0
fail=0
ok() { echo "PASS learnbuild: $1"; pass=$((pass+1)); }
no() { echo "FAIL learnbuild: $1" >&2; fail=$((fail+1)); }

# The composer is agent mode (PARROT0_TOOLS=1) — the same channel piagent uses.
sess() { printf '%s\n' "$@" | PARROT0_TOOLS=1 PARROT0_SESSION= \
         PARROT0_PROFILE="$ROOT/kb/profiles/agi.p0" "$BIN" 2>/dev/null; }

# A fresh nonsense name every run: nothing in the KB can recognize it.
nonce() {
    local cons=(b d f g k l m n p r t v z) vow=(a e i o u)
    printf '%s%s%s%s%s' "${cons[RANDOM % 13]}" "${vow[RANDOM % 5]}" \
        "${cons[RANDOM % 13]}" "${vow[RANDOM % 5]}" "${cons[RANDOM % 13]}"
}
N="$(nonce)"
echo "learnbuild: this run's invented algorithm name: $N"

# The page. It never says "bubble", "sort" or anything the KB could recognize —
# it says what the steps DO.
PAGE="repeat passes over the list; compare each adjacent pair; swap them if they are out of order"
# A page for a genuinely different structure: insertion sort shifts and inserts.
# It never sweeps adjacent pairs across repeated passes.
OTHER="take each element in turn; shift the larger elements one place to the right; insert the element into the gap"

# ---- 1. ABLATION: without the page, it cannot be built -----------------------
got="$(sess "write a $N function")"
if printf '%s' "$got" | grep -qi "I will not emit code I cannot check"; then
    ok "ABLATION: without the page, the algorithm cannot be built (honest decline)"
else
    no "it built something with no schema: $got"
fi

# ---- 2. INDUCTION: the page yields the schema --------------------------------
got="$(sess "learn $N from these steps: $PAGE")"
if printf '%s' "$got" | grep -q "nested_loop_compare_swap" \
   && printf '%s' "$got" | grep -q "schema for $N"; then
    ok "INDUCTION: the page yields a schema for an algorithm nobody curated"
else
    no "the page did not induce a schema: $got"
fi

# ---- 3. BUILD: the induced schema is synthesized and RUN ---------------------
# The verdict comes from compiling and executing the candidate on 8 vectors — not
# from inspecting it. A wrong body cannot pass by looking right.
got="$(sess "learn $N from these steps: $PAGE" "write a $N function")"
if printf '%s' "$got" | grep -q "void $N(int a\[\], int n)" \
   && printf '%s' "$got" | grep -q "verified by execution"; then
    ok "BUILD: the induced schema is synthesized and VERIFIED by running it"
else
    no "the induced schema did not build+verify: $got"
fi

# ---- 4. THE NAME IS NOT READ: induction is structural ------------------------
# This is the anti-impostor core. The name is nonsense and appears nowhere in any
# knowledge file; only the STEPS carry the structure. Covered by 2+3 above, which
# used $N throughout — assert it explicitly so a future lexical shortcut is caught.
if ! grep -rq "$N" "$ROOT/kb/" 2>/dev/null; then
    ok "the algorithm's name appears in NO knowledge file — only its steps were read"
else
    no "the name $N leaked into the KB tree; the test is no longer held-out"
fi

# ---- 5. HONESTY: a different structure is DECLINED, not force-fitted ---------
# There is exactly ONE shape the synthesizer can instantiate. The tempting failure
# is to fit every page into it. It must refuse, and name what it could not find.
got="$(sess "learn ${N}two from these steps: $OTHER")"
if printf '%s' "$got" | grep -q "do not describe a structure I can build" \
   && printf '%s' "$got" | grep -q "repeat_passes" \
   && printf '%s' "$got" | grep -q "will not invent a schema"; then
    ok "a page with a DIFFERENT structure is declined, naming the missing step"
else
    no "it force-fitted a foreign structure into the only schema it has: $got"
fi

# ...and having declined, it must still refuse to build it.
got="$(sess "learn ${N}two from these steps: $OTHER" "write a ${N}two function")"
if printf '%s' "$got" | grep -qi "I will not emit code I cannot check"; then
    ok "a declined page leaves the algorithm unbuildable (no half-schema)"
else
    no "it built an algorithm whose schema it had refused: $got"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
