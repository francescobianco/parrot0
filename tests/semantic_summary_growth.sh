#!/usr/bin/env bash
# Runtime-growth contract for teachable semantic-summary surface aliases.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "semantic_summary_growth: binary not built ($BIN)" >&2; exit 1; }

out="$({
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Explain gravitas-alpha."}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"semantic_topic_cue","args":["gravity","keyword(gravitas-alpha)"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Explain gravitas-alpha."}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"semantic_topic_cue","args":["gravity","keyword(gravitas-alpha)"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Explain gravitas-alpha."}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"semantic_topic_cue","args":["gravity","keyword(gateprobe-alpha)"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Explain gateprobe-alpha."}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":9,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"semantic_topic_gate","args":["gravity","sealedgravity"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":10,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Explain gateprobe-alpha."}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":11,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"semantic_topic_gate","args":["gravity","keyword(gateprobe-alpha)"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":12,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Explain gateprobe-alpha."}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":13,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"semantic_topic_gate","args":["gravity","keyword(gateprobe-alpha)"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":14,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Explain gateprobe-alpha."}}}'
} | PARROT0_SESSION= PARROT0_PROFILE= PARROT0_WORLD_FACTS=1 "$BIN" --mcp-engine 2>/dev/null)"

line() { printf '%s\n' "$out" | grep -F "\"id\":$1,"; }
fail=0
line 2 | grep -Fqi 'draws objects toward its center' &&
  { echo "FAIL semantic_summary_growth: alias active before assertion" >&2; fail=1; }
line 4 | grep -Fqi 'draws objects toward its center' ||
  { echo "FAIL semantic_summary_growth: assertion did not enable alias" >&2; fail=1; }
line 6 | grep -Fqi 'draws objects toward its center' &&
  { echo "FAIL semantic_summary_growth: retraction did not remove alias" >&2; fail=1; }
line 8 | grep -Fqi 'draws objects toward its center' ||
  { echo "FAIL semantic_summary_growth: canonical topic missing before gate" >&2; fail=1; }
line 10 | grep -Fqi 'draws objects toward its center' &&
  { echo "FAIL semantic_summary_growth: unsupported topic bypassed its gate" >&2; fail=1; }
line 12 | grep -Fqi 'draws objects toward its center' ||
  { echo "FAIL semantic_summary_growth: runtime gate cue did not enable topic" >&2; fail=1; }
line 14 | grep -Fqi 'draws objects toward its center' &&
  { echo "FAIL semantic_summary_growth: gate retraction did not remove claim" >&2; fail=1; }
[ "$fail" -eq 0 ] && echo "PASS semantic_summary_growth"
exit "$fail"
