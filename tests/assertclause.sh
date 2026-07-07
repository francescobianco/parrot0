#!/usr/bin/env bash
#
# gen281 (U2, NEXT.md) — kb.assert_clause MCP tool: assert a FULL definite clause
# with n-ary head and body goals carrying distinct/shared variables.
#
# GATE: teach grandparent($X,$Z) :- parent($X,$Y), parent($Y,$Z) via MCP
# (no .p0 file), then query grandparent(tom,ann) → true,
# grandparent(tom,bob) → false.
# RED before the change: kb.assert_clause does not exist → the test is a wall.
# GREEN after: it rounds trips n-ary joins through the engine.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "assertclause: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS assertclause: $1"; pass=$((pass+1)); }
no() { echo "FAIL assertclause: $1" >&2; fail=$((fail+1)); }

call() { printf '%s\n' "$1"; }

out="$( {
  call '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  call '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  # teach parent facts
  call '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"parent","args":["tom","bob"]}}}'
  call '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"parent","args":["bob","ann"]}}}'
  # teach the grandparent RULE via kb.assert_clause
  call '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.assert_clause","arguments":{"head":{"pred":"grandparent","args":["$X","$Z"]},"body":[{"pred":"parent","args":["$X","$Y"]},{"pred":"parent","args":["$Y","$Z"]}]}}}'
  # query: grandparent(tom,ann) should be true
  call '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"grandparent","args":["tom","ann"]}}}'
  # query: grandparent(tom,bob) should be false (join is real)
  call '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"grandparent","args":["tom","bob"]}}}'
  # idempotent: assert the same clause again
  call '{"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"kb.assert_clause","arguments":{"head":{"pred":"grandparent","args":["$X","$Z"]},"body":[{"pred":"parent","args":["$X","$Y"]},{"pred":"parent","args":["$Y","$Z"]}]}}}'
} | PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

line() { printf '%s\n' "$out" | grep -F "\"id\":$1"; }

# GATE A: grandparent(tom,ann) is provable (the join works)
if line 5 | grep -q 'provable\\":true'; then
    ok "grandparent(tom,ann) provable via kb.assert_clause rule"
else
    no "grandparent(tom,ann) not provable: $(line 5)"
fi

# GATE B: grandparent(tom,bob) is NOT provable (join discriminates)
if line 6 | grep -q 'provable\\":false'; then
    ok "grandparent(tom,bob) correctly false (join real, not vacuous)"
else
    no "grandparent(tom,bob) false-positive: $(line 6)"
fi

# GATE C: assert_clause is idempotent (ok:true for repeated assertion)
if line 7 | grep -q 'ok\\":true'; then
    ok "kb.assert_clause idempotent (repeated assertion ok)"
else
    no "kb.assert_clause not idempotent: $(line 7)"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
