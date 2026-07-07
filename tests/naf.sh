#!/usr/bin/env bash
#
# gen282 (U6, NEXT.md / teach-comprehension-via-mcp.md §6.2) — negation-as-failure
# in a rule body: naf(G) succeeds iff G is NOT derivable. This makes a DEFAULT
# WITH EXCEPTIONS teachable as knowledge — "birds fly, except flightless ones":
#
#   flies($X)    :- bird($X), naf(abnormal($X)).
#   abnormal($X) :- flightless($X).
#
# Overcomes Secchio D.2. RED before U6: naf is ignored, so flies(penguin) is
# true. GREEN after: flies(eagle) true, flies(penguin) false. Tested both via a
# .p0 file (naf(...) surface syntax) and via MCP kb.assert_clause (a body goal
# with "neg":true), since the whole point is teaching the default over MCP.
#
# Orthogonal to explicit ground negative facts (kb_is_negated / persist.sh):
# naf is derivational, not a stored neg fact.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "naf: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS naf: $1"; pass=$((pass+1)); }
no() { echo "FAIL naf: $1" >&2; fail=$((fail+1)); }

# ---- Path 1: the default taught from a .p0 file (naf(...) surface syntax) ----
base="$(mktemp /tmp/naf.XXXXXX.p0)"
trap 'rm -f "$base"' EXIT
cat > "$base" <<'P0'
bird(eagle).
bird(penguin).
flightless(penguin).
abnormal($X) :- flightless($X).
flies($X) :- bird($X), naf(abnormal($X)).
P0

out1="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"flies","args":["eagle"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"flies","args":["penguin"]}}}'
} | PARROT0_BASE="$base" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l1() { printf '%s\n' "$out1" | grep -F "\"id\":$1"; }

if l1 2 | grep -q 'provable\\":true'; then
    ok ".p0: default holds (flies(eagle) true)"
else
    no ".p0: normal case failed: $(l1 2)"
fi
if l1 3 | grep -q 'provable\\":false'; then
    ok ".p0: exception fires via naf (flies(penguin) false)"
else
    no ".p0: naf did not suppress the exception: $(l1 3)"
fi

# ---- Path 2: the SAME default taught over MCP (kb.assert_clause + neg goal) ----
out2="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"bird","args":["eagle"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"bird","args":["penguin"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"flightless","args":["penguin"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.assert_clause","arguments":{"head":{"pred":"abnormal","args":["$X"]},"body":[{"pred":"flightless","args":["$X"]}]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"kb.assert_clause","arguments":{"head":{"pred":"flies","args":["$X"]},"body":[{"pred":"bird","args":["$X"]},{"pred":"abnormal","args":["$X"],"neg":true}]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"flies","args":["eagle"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"flies","args":["penguin"]}}}'
} | PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l2() { printf '%s\n' "$out2" | grep -F "\"id\":$1"; }

if l2 7 | grep -q 'provable\\":true'; then
    ok "MCP: default holds (flies(eagle) true)"
else
    no "MCP: normal case failed: $(l2 7)"
fi
if l2 8 | grep -q 'provable\\":false'; then
    ok "MCP: exception fires via neg goal (flies(penguin) false)"
else
    no "MCP: neg goal did not suppress the exception: $(l2 8)"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
