#!/usr/bin/env bash
#
# gen26: proof depth — a BBH-like probe distinguishing direct facts from
# multi-step (composed) inference. "how do you know <x> is a/an <y>?" reads the
# proof trace and reports whether the belief is held DIRECTLY (a stored fact)
# or reached BY N STEPS of reasoning (rule applications along the chain), and
# refuses what it cannot prove. Held-out nonsense predicates ensure the answer
# comes from the proof structure, not from recognised English.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"

if [ ! -x "$BIN" ]; then
    echo "howknow: binary not built ($BIN)" >&2
    exit 1
fi

tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT
base="$tmp/world.p0"

cat > "$base" <<'EOF'
quux(zibo).
flob($X) :- quux($X).
warg($X) :- flob($X).
EOF

pass=0
fail=0
expect() {
    local got
    got="$(printf '%s\n/quit\n' "$2" \
          | PARROT0_BASE="$base" PARROT0_SESSION= "$BIN" 2>/dev/null | head -1)"
    if [ "$got" = "$3" ]; then echo "PASS howknow: $1"; pass=$((pass+1))
    else echo "FAIL howknow: $1 — want [$3] got [$got]" >&2; fail=$((fail+1)); fi
}

expect "direct fact is direct" "how do you know zibo is a quux?" \
       "Directly: quux(zibo) is a known fact."
expect "one rule is one step" "how do you know zibo is a flob?" \
       "By 1 step of reasoning: flob(zibo) because quux(zibo)."
expect "two-rule chain is multi-step" "how do you know zibo is a warg?" \
       "By 2 steps of reasoning: warg(zibo) because flob(zibo) because quux(zibo)."
expect "no proof, no invented chain" "how do you know plouf is a warg?" \
       "I can't show that."

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]