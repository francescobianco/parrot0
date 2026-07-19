#!/usr/bin/env bash
# T06 structural ratchet: runtime rule growth, minimum-blast choice, replanning,
# budget, retraction and typed gaps in one persistent Brain/MCP process.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
BAD_BOUND='void mysort(int a[], int n) { for (int i = 0; i < n; i++) { for (int j = 0; j < n - i; j++) { if (a[j] > a[j+1]) { int t = a[j]; a[j] = a[j+1]; a[j+1] = t; } } } }'
BAD_NOOP='void mysort(int a[], int n) { for (int i = 0; i < n; i++) { a[i] = a[i]; } }'

call() { printf '%s\n' "$1"; }
out="$({
  call '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  call '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  call '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"repair_rule","args":["not_permutation","flip_comparator"]}}}'
  call '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"repair_rule","args":["not_ordered","tighten_inner_bound"]}}}'
  call "{\"jsonrpc\":\"2.0\",\"id\":4,\"method\":\"tools/call\",\"params\":{\"name\":\"gen.respond\",\"arguments\":{\"input\":\"fix this code: $BAD_BOUND\"}}}"
  call '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_decision","args":[null,null,"flip_comparator","1"]}}}'
  call '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_candidate","args":[null,null,"flip_comparator","1"]}}}'
  call '{"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_candidate","args":[null,null,"tighten_inner_bound","4"]}}}'
  call '{"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_attempt","args":[null,"1","not_permutation","not_ordered"]}}}'
  call '{"jsonrpc":"2.0","id":9,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_attempt","args":[null,"2","not_ordered","not_ordered"]}}}'
  call '{"jsonrpc":"2.0","id":10,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_attempt","args":[null,"3","not_ordered","verified"]}}}'
  call '{"jsonrpc":"2.0","id":11,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_terminal","args":[null,"verified","3","0"]}}}'
  call '{"jsonrpc":"2.0","id":12,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"rec","args":[null,"task","0","done"]}}}'
  call '{"jsonrpc":"2.0","id":13,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"rec_name","args":[null,"\"repair_task\""]}}}'
  call '{"jsonrpc":"2.0","id":14,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"rec_source","args":[1,0]}}}'
  call '{"jsonrpc":"2.0","id":15,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"repair_budget","args":["sort_contract",2]}}}'
  call "{\"jsonrpc\":\"2.0\",\"id\":16,\"method\":\"tools/call\",\"params\":{\"name\":\"gen.respond\",\"arguments\":{\"input\":\"fix this code: $BAD_BOUND\"}}}"
  call '{"jsonrpc":"2.0","id":17,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_terminal","args":[null,"budget_exhausted","2",null]}}}'
  call '{"jsonrpc":"2.0","id":18,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"repair_rule","args":["not_permutation","flip_comparator"]}}}'
  call '{"jsonrpc":"2.0","id":19,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"repair_rule","args":["not_ordered","tighten_inner_bound"]}}}'
  call '{"jsonrpc":"2.0","id":20,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"repair_budget","args":["sort_contract",2]}}}'
  call "{\"jsonrpc\":\"2.0\",\"id\":21,\"method\":\"tools/call\",\"params\":{\"name\":\"gen.respond\",\"arguments\":{\"input\":\"fix this code: $BAD_BOUND\"}}}"
  call '{"jsonrpc":"2.0","id":22,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_terminal","args":[null,"verified","1","0"]}}}'
  call '{"jsonrpc":"2.0","id":23,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"repair_rule","args":["not_permutation","tighten_inner_bound"]}}}'
  call "{\"jsonrpc\":\"2.0\",\"id\":24,\"method\":\"tools/call\",\"params\":{\"name\":\"gen.respond\",\"arguments\":{\"input\":\"fix this code: $BAD_BOUND\"}}}"
  call '{"jsonrpc":"2.0","id":25,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_terminal","args":[null,"zero_rule","0",null]}}}'
  call '{"jsonrpc":"2.0","id":26,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"rec_name","args":[null,"\"repair_zero_rule\""]}}}'
  call '{"jsonrpc":"2.0","id":27,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"repair_rule","args":["not_permutation","tighten_inner_bound"]}}}'
  call "{\"jsonrpc\":\"2.0\",\"id\":28,\"method\":\"tools/call\",\"params\":{\"name\":\"gen.respond\",\"arguments\":{\"input\":\"fix this code: $BAD_NOOP\"}}}"
  call '{"jsonrpc":"2.0","id":29,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_terminal","args":[null,"no_applicable","0",null]}}}'
  call '{"jsonrpc":"2.0","id":30,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"rec_name","args":[null,"\"repair_no_applicable\""]}}}'
} | PARROT0_SESSION= PARROT0_BASE= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

line() { printf '%s\n' "$out" | grep -F "\"id\":$1,"; }
fail=0
expect() { if ! line "$1" | grep -qF "$2"; then echo "FAIL agentrepair: id=$1 lacks $2" >&2; fail=1; fi; }

for id in 5 6 7 8 9 10 11; do expect "$id" 'bindings\":[\"'; done
expect 12 'bindings\":[\"1\"]'
expect 13 'bindings\":[\"1\"]'
expect 14 'provable\":true'
expect 17 'bindings\":[\"'
expect 22 'bindings\":[\"'
expect 25 'bindings\":[\"'
expect 26 'bindings\":[\"'
expect 29 'bindings\":[\"'
expect 30 'bindings\":[\"'

if [ "$fail" -eq 0 ]; then
  echo "PASS agentrepair: live rules drive min-blast replan, budget and typed gaps in one process"
fi
exit "$fail"
