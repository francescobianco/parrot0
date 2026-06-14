#!/usr/bin/env bash
#
# gen11 multi-goal resolution: a rule whose body is a conjunction of goals
# with a shared intermediate variable, resolved by backward chaining.
#   grandparent(X, Z) :- parent(X, Y), parent(Y, Z).
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"

if [ ! -x "$BIN" ]; then
    echo "multigoal: binary not built ($BIN)" >&2
    exit 1
fi

tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT
base="$tmp/family.pl"

cat > "$base" <<'EOF'
% a two-goal rule with a shared intermediate variable Y
parent(tom, bob).
parent(bob, ann).
grandparent(X, Z) :- parent(X, Y), parent(Y, Z).
EOF

pass=0
fail=0
expect() { # <description> <input> <expected first stdout line>
    local got
    got="$(printf '%s\n/quit\n' "$2" \
          | PARROT0_BASE="$base" PARROT0_SESSION= "$BIN" 2>/dev/null | head -1)"
    if [ "$got" = "$3" ]; then echo "PASS multigoal: $1"; pass=$((pass+1))
    else echo "FAIL multigoal: $1 — want [$3] got [$got]" >&2; fail=$((fail+1)); fi
}

expect "grandparent via two-goal rule" "is tom the grandparent of ann?" "Yes."
expect "non-grandparent is false"      "is tom the grandparent of bob?" "No."
expect "variable query through rule"   "who is the grandparent of ann?" "tom."

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
