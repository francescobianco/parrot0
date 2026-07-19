#!/usr/bin/env bash
#
# coding-agent T03: the declarative code_task plan is consumed by the real
# planner, materialized as typed agent records, and remains live-teachable.
# One MCP process is essential: plan -> inspect projected records -> retract a
# CONSUMED action_yields fact -> observe a typed Gap -> reassert -> plan again.
# A golden sentence alone cannot pass this test; it queries rec/4, rec_source/2
# and rec_name/2 from the same live KB that mod_plan projected into.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "code-task-agent: binary not built ($BIN)" >&2; exit 1; }

pass=0
fail=0
ok() { echo "PASS code-task-agent: $1"; pass=$((pass + 1)); }
no() { echo "FAIL code-task-agent: $1" >&2; fail=$((fail + 1)); }
call() { printf '%s\n' "$1"; }

# Exercise the public Brain transition seam directly. Link the already-built
# engine objects (make test depends on build); add libcurl only when one of this
# driver's engine objects actually references it.
TMP="${TMPDIR:-/tmp}/parrot0-code-task-agent.$$"
trap 'rm -rf "$TMP"' EXIT
mkdir -p "$TMP"
curl_lib=()
if nm -u "$ROOT/obj/brain.o" "$ROOT/obj/learn.o" 2>/dev/null | grep -q 'curl_'; then
  curl_lib=(-l:libcurl.so.4)
fi
if cc -std=c11 -Wall -Wextra -Wpedantic -O2 -I"$ROOT/src" -I"$ROOT/obj" \
      -o "$TMP/brain-agent-state" "$ROOT/tests/brain-agent-state.c" \
      "$ROOT/obj/agent.o" "$ROOT/obj/brain.o" "$ROOT/obj/code.o" \
      "$ROOT/obj/exec.o" "$ROOT/obj/json.o" "$ROOT/obj/kb.o" "$ROOT/obj/patch.o" \
      "$ROOT/obj/learn.o" "$ROOT/obj/version.o" "${curl_lib[@]}"; then
  if PARROT0_SESSION= PARROT0_BASE= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= \
       "$TMP/brain-agent-state"; then
    ok "the public Brain state API keeps store and rec/4 projection atomic"
  else
    no "the public Brain state API failed its structural transition driver"
  fi
else
  no "could not build the structural Brain state transition driver"
fi

out="$( {
  call '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  call '{"jsonrpc":"2.0","method":"notifications/initialized"}'

  # The first plan loads the action domain and must append Task, Goal, 4 Actions.
  call '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"make a plan for a code task"}}}'
  call '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"rec","args":[null,"task",null,"active"]}}}'
  call '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"rec","args":[null,"action",null,"candidate"]}}}'
  call '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"rec_source","args":[3,2]}}}'

  # action_yields is a fact plan_chain actually consumes, not inert metadata.
  call '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"action_yields","args":["localize_change","target_region"]}}}'
  call '{"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"make a plan for a code task"}}}'
  call '{"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"rec","args":[null,"gap",null,"open"]}}}'
  call '{"jsonrpc":"2.0","id":9,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"rec_name","args":[null,"\"target_region\""]}}}'

  # Restore the very same fact without rebuilding; the four-action plan returns.
  call '{"jsonrpc":"2.0","id":10,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"action_yields","args":["localize_change","target_region"]}}}'
  call '{"jsonrpc":"2.0","id":11,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"make a plan for a code task"}}}'
  call '{"jsonrpc":"2.0","id":12,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"rec","args":[null,"action",null,"candidate"]}}}'
  call '{"jsonrpc":"2.0","id":13,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"action_risk","args":["edit_candidate","workspace_write"]}}}'
  call '{"jsonrpc":"2.0","id":14,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"action_cost","args":["edit_candidate",3]}}}'
  call '{"jsonrpc":"2.0","id":15,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"action_tool","args":["verify_candidate","test"]}}}'
  call '{"jsonrpc":"2.0","id":16,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"rec","args":[null,"goal",null,"blocked"]}}}'
} | PARROT0_SESSION= PARROT0_BASE= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= \
    "$BIN" --mcp-engine 2>/dev/null)"

line() { printf '%s\n' "$out" | grep -F "\"id\":$1,"; }

if line 2 | grep -qF 'My derived plan for code task' \
   && line 2 | grep -qF 'candidate patch' \
   && line 2 | grep -qF 'relevant oracle'; then
  ok "the KB domain derives inspect -> localize -> edit candidate -> verify"
else
  no "the initial code_task plan was not derived: $(line 2)"
fi

if line 3 | grep -qF 'bindings\":[\"1\"]' \
   && line 4 | grep -qF 'bindings\":[\"3\",\"4\",\"5\",\"6\"]'; then
  ok "the live KB contains one Task and four candidate Action records"
else
  no "typed Task/Action projection missing: $(line 3) $(line 4)"
fi

if line 5 | grep -qF 'provable\":true'; then
  ok "rec_source/2 preserves Action -> Goal provenance in the live KB"
else
  no "rec_source/2 was not queryable: $(line 5)"
fi

if line 6 | grep -qF 'removed\":true'; then
  ok "a consumed action_yields fact can be retracted at runtime"
else
  no "action_yields ablation did not happen: $(line 6)"
fi

if line 7 | grep -qF 'nothing yields target region' \
   && line 8 | grep -qF 'bindings\":[\"9\"]' \
   && line 9 | grep -qF 'bindings\":[\"9\"]' \
   && line 16 | grep -qF 'bindings\":[\"8\"]'; then
  ok "consumed action-fact ablation blocks the Goal and appends a named Gap"
else
  no "ablation did not block the Goal with target_region Gap: $(line 7) $(line 8) $(line 9) $(line 16)"
fi

if line 10 | grep -qF 'stored\":true' \
   && line 11 | grep -qF 'My derived plan for code task' \
   && line 12 | grep -qF 'bindings\":[\"3\",\"4\",\"5\",\"6\",\"12\",\"13\",\"14\",\"15\"]'; then
  ok "reasserting the action fact restores the four-step plan without rebuilding"
else
  no "runtime reassert did not restore plan + records: $(line 10) $(line 11) $(line 12)"
fi

if line 13 | grep -qF 'provable\":true' \
   && line 14 | grep -qF 'provable\":true' \
   && line 15 | grep -qF 'provable\":true'; then
  ok "risk/cost/tool action metadata is loaded with the consumed plan domain"
else
  no "action metadata is missing: $(line 13) $(line 14) $(line 15)"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
