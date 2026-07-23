#!/usr/bin/env bash
# Runtime-growth contract for KB-owned creative request and topic vocabulary.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "creative_composer_growth: binary not built ($BIN)" >&2; exit 1; }

out="$({
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Dialog-alpha about orbital gardens."}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["creative_text_request","dialog-alpha"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["generic_dialogue_request","dialog-alpha"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Dialog-alpha about orbital gardens."}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["generic_dialogue_request","dialog-alpha"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["creative_text_request","dialog-alpha"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Dialog-alpha about orbital gardens."}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":9,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Write a dialogue concerning-alpha orbital gardens."}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":10,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["creative_topic_marker","concerning-alpha"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":11,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Write a dialogue concerning-alpha orbital gardens."}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":12,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["creative_topic_marker","concerning-alpha"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":13,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Write a dialogue concerning-alpha orbital gardens."}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":14,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Artifact-alpha please."}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":15,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["runtime_artifact","artifact-alpha"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":16,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"creative_response","args":["runtime_artifact","historical_riddle_answer"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":17,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Artifact-alpha please."}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":18,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"creative_response","args":["runtime_artifact","historical_riddle_answer"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":19,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["runtime_artifact","artifact-alpha"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":20,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Artifact-alpha please."}}}'
} | PARROT0_SESSION= PARROT0_PROFILE= PARROT0_WORLD_FACTS=1 "$BIN" --mcp-engine 2>/dev/null)"

line() { printf '%s\n' "$out" | grep -F "\"id\":$1,"; }
fail=0
line 2 | grep -Fq 'Mara:' &&
  { echo "FAIL creative_composer_growth: request cue active before assertion" >&2; fail=1; }
line 5 | grep -Fq 'Mara:' ||
  { echo "FAIL creative_composer_growth: request cue assertion had no effect" >&2; fail=1; }
line 8 | grep -Fq 'Mara:' &&
  { echo "FAIL creative_composer_growth: request cue retraction had no effect" >&2; fail=1; }
line 9 | grep -Fq 'Mara:' &&
  { echo "FAIL creative_composer_growth: topic marker active before assertion" >&2; fail=1; }
line 11 | grep -Fq 'Mara:' ||
  { echo "FAIL creative_composer_growth: topic marker assertion had no effect" >&2; fail=1; }
line 13 | grep -Fq 'Mara:' &&
  { echo "FAIL creative_composer_growth: topic marker retraction had no effect" >&2; fail=1; }
line 14 | grep -Fq 'footprints where no rain' &&
  { echo "FAIL creative_composer_growth: artifact class active before assertion" >&2; fail=1; }
line 17 | grep -Fq 'footprints where no rain' ||
  { echo "FAIL creative_composer_growth: artifact class assertion had no effect" >&2; fail=1; }
line 20 | grep -Fq 'footprints where no rain' &&
  { echo "FAIL creative_composer_growth: artifact class retraction had no effect" >&2; fail=1; }
[ "$fail" -eq 0 ] && echo "PASS creative_composer_growth"
exit "$fail"
