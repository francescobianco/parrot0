#!/usr/bin/env bash
#
# gen14: proof traces / explanations. "why ...?" renders the proof tree —
# facts, rule chains, and multi-goal bodies with their bindings — and admits
# when there is no proof. Uses held-out predicates so the explanation must
# come from the proof, not canned English.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"

if [ ! -x "$BIN" ]; then
    echo "explain: binary not built ($BIN)" >&2
    exit 1
fi

tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT
base="$tmp/world.p0"

cat > "$base" <<'EOF'
man(socrates).
mortal(X) :- man(X).
being(X) :- mortal(X).
parent(tom, bob).
parent(bob, ann).
grandparent(X, Z) :- parent(X, Y), parent(Y, Z).
EOF

pass=0
fail=0
expect() {
    local got
    got="$(printf '%s\n/quit\n' "$2" \
          | PARROT0_BASE="$base" PARROT0_SESSION= "$BIN" 2>/dev/null | head -1)"
    if [ "$got" = "$3" ]; then echo "PASS explain: $1"; pass=$((pass+1))
    else echo "FAIL explain: $1 — want [$3] got [$got]" >&2; fail=$((fail+1)); fi
}

expect "rule one step"  "why is socrates a mortal?" \
       "mortal(socrates) because man(socrates)."
expect "rule chain"     "why is socrates a being?" \
       "being(socrates) because mortal(socrates) because man(socrates)."
expect "base fact"      "why is socrates a man?" \
       "man(socrates) is a known fact."
expect "multi-goal + binding" "why is tom the grandparent of ann?" \
       "grandparent(tom, ann) because parent(tom, bob) and parent(bob, ann)."
expect "no proof, no invention" "why is plato a mortal?" \
       "I can't show that."

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
