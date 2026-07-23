#!/usr/bin/env bash
# Runtime-growth contract for the KB-owned digit-sequence request vocabulary.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "digit_sum_growth: binary not built ($BIN)" >&2; exit 1; }

out="$({
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"A 4-slot vault-alpha uses digits 0-9. The sum of the digits is 10. How many combinations are possible?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["digit_sequence_container","vault-alpha"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"A 4-slot vault-alpha uses digits 0-9. The sum of the digits is 10. How many combinations are possible?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["digit_sequence_container","vault-alpha"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"A 4-slot vault-alpha uses digits 0-9. The sum of the digits is 10. How many combinations are possible?"}}}'
} | PARROT0_SESSION= PARROT0_PROFILE= PARROT0_WORLD_FACTS=1 "$BIN" --mcp-engine 2>/dev/null)"

line() { printf '%s\n' "$out" | grep -F "\"id\":$1,"; }
fail=0
line 2 | grep -Fq '282.' &&
  { echo "FAIL digit_sum_growth: cue active before assertion" >&2; fail=1; }
line 4 | grep -Fq '282.' ||
  { echo "FAIL digit_sum_growth: cue assertion had no effect" >&2; fail=1; }
line 6 | grep -Fq '282.' &&
  { echo "FAIL digit_sum_growth: cue retraction had no effect" >&2; fail=1; }
[ "$fail" -eq 0 ] && echo "PASS digit_sum_growth"
exit "$fail"
