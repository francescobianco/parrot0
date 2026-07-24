#!/usr/bin/env bash
# Runtime-growth contract for inverse without-replacement probability wording.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || {
  echo "probability_inverse_growth: binary not built ($BIN)" >&2
  exit 1
}

out="$({
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"The probability of pairprobe-alpha is 0.2. What follows?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["probability_inverse_draw","pairprobe-alpha"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"The probability of pairprobe-alpha is 0.2. What follows?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["probability_inverse_draw","pairprobe-alpha"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"The probability of pairprobe-alpha is 0.2. What follows?"}}}'
} | PARROT0_SESSION= PARROT0_PROFILE= PARROT0_WORLD_FACTS=1 \
    "$BIN" --mcp-engine 2>/dev/null)"

line() { printf '%s\n' "$out" | grep -F "\"id\":$1,"; }
fail=0
line 2 | grep -Fq 'does not determine a unique total' &&
  { echo "FAIL probability_inverse_growth: cue active before assertion" >&2; fail=1; }
line 4 | grep -Fq 'R = 3 and N = 6' ||
  { echo "FAIL probability_inverse_growth: assertion did not enable inverse solver" >&2; fail=1; }
line 6 | grep -Fq 'does not determine a unique total' &&
  { echo "FAIL probability_inverse_growth: retraction did not remove recognition" >&2; fail=1; }
[ "$fail" -eq 0 ] && echo "PASS probability_inverse_growth"
exit "$fail"
