#!/usr/bin/env bash
# Runtime-growth contract for KB-owned plan-first arithmetic routing.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "arith_guard_growth: binary not built ($BIN)" >&2; exit 1; }

out="$({
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What is 8 plus 4 guard-alpha?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["discount_chain_schema","guard-alpha"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What is 8 plus 4 guard-alpha?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["discount_chain_schema","guard-alpha"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What is 8 plus 4 guard-alpha?"}}}'
} | PARROT0_SESSION= PARROT0_PROFILE= PARROT0_WORLD_FACTS=1 "$BIN" --mcp-engine 2>/dev/null)"

line() { printf '%s\n' "$out" | grep -F "\"id\":$1,"; }
fail=0
line 2 | grep -Fq '12.' || { echo "FAIL arith_guard_growth: baseline" >&2; fail=1; }
line 4 | grep -Fq '12.' && { echo "FAIL arith_guard_growth: asserted cue did not guard scalar arithmetic" >&2; fail=1; }
line 6 | grep -Fq '12.' || { echo "FAIL arith_guard_growth: retraction did not restore scalar arithmetic" >&2; fail=1; }
[ "$fail" -eq 0 ] && echo "PASS arith_guard_growth"
exit "$fail"
