#!/usr/bin/env bash
#
# T1: real anonymous variables. Distinct "_" occurrences in one clause must be
# INDEPENDENT (not forced to the same value). Uses held-out predicates
# (edge/related, unrelated to morphology) so success proves the feature is
# relational, not hard-coded.
#
#   related(X) :- edge(X, _), edge(_, X).   % X has SOME out-edge and SOME in-edge
#
# If the two "_" aliased, related(a) below would be false (no node V with both
# edge(a,V) and edge(V,a)). With independent anonymous vars it is true.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"

if [ ! -x "$BIN" ]; then
    echo "anon: binary not built ($BIN)" >&2
    exit 1
fi

tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT
base="$tmp/graph.pl"

cat > "$base" <<'EOF'
edge(a, b).
edge(c, a).
edge(d, e).
related(X) :- edge(X, _), edge(_, X).
EOF

pass=0
fail=0
expect() {
    local got
    got="$(printf '%s\n/quit\n' "$2" \
          | PARROT0_BASE="$base" PARROT0_SESSION= "$BIN" 2>/dev/null | head -1)"
    if [ "$got" = "$3" ]; then echo "PASS anon: $1"; pass=$((pass+1))
    else echo "FAIL anon: $1 — want [$3] got [$got]" >&2; fail=$((fail+1)); fi
}

# a: out-edge to b, in-edge from c (different neighbours) -> independent _ => Yes
expect "independent anonymous vars" "is a a related?" "Yes."
# d: out-edge only, no in-edge -> No (and proves the rule isn't trivially true)
expect "missing in-edge -> false"   "is d a related?" "No."

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
