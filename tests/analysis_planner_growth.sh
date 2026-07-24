#!/usr/bin/env bash
# Runtime-growth and completeness contract for the gen360 analytical planner.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "analysis_planner_growth: binary not built ($BIN)" >&2; exit 1; }

out="$({
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'

  # Domain vocabulary is live KB evidence.
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How would quorvex change under constraints and feedback over a long period?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"analysis_domain_cue","args":["complex_system","quorvex"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How would quorvex change under constraints and feedback over a long period?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"analysis_domain_cue","args":["complex_system","quorvex"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How would quorvex change under constraints and feedback over a long period?"}}}'

  # Act vocabulary is independently teachable.
  printf '%s\n' '{"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Ponderize a complex system with interacting constraints and feedback over time."}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"analysis_act_cue","args":["impact_analysis","ponderize"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":9,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Ponderize a complex system with interacting constraints and feedback over time."}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":10,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"analysis_act_cue","args":["impact_analysis","ponderize"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":11,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Ponderize a complex system with interacting constraints and feedback over time."}}}'

  # Format request vocabulary changes only the realizer, not the plan/content.
  printf '%s\n' '{"jsonrpc":"2.0","id":12,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How would a complex system change under feedback over time in gridform?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":13,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"format_constraint","args":["numbered","gridform"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":14,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How would a complex system change under feedback over time in gridform?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":15,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"format_constraint","args":["numbered","gridform"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":16,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How would a complex system change under feedback over time in gridform?"}}}'

  # A recognized candidate with one absent required slot must emit no partial plan.
  printf '%s\n' '{"jsonrpc":"2.0","id":17,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"analysis_act_cue","args":["incomplete_analysis","audit-alpha"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":18,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"answer_plan","args":["incomplete_analysis","context","1","domain_required"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":19,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"answer_plan","args":["incomplete_analysis","absent_slot","2","required"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":20,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Audit-alpha a complex system with interacting constraints and feedback over time."}}}'

  # A broad field family changes the composed knowledge without replacing the act.
  printf '%s\n' '{"jsonrpc":"2.0","id":21,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How would zentropic effects change outcomes across several stages?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":22,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"analysis_family_cue","args":["runtime_family","zentropic"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":23,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How would zentropic effects change outcomes across several stages?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":24,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"analysis_family_cue","args":["runtime_family","zentropic"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":25,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How would zentropic effects change outcomes across several stages?"}}}'

  # A specific domain with a declared gate cannot claim from support alone.
  # Teaching and retracting the discriminating surface changes that live.
  printf '%s\n' '{"jsonrpc":"2.0","id":26,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"analysis_domain_cue","args":["guarded_system","gatecase"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":27,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"analysis_domain_gate","args":["guarded_system","sealedmark"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":28,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"semantic_atom","args":["guarded_system","context","Guarded domain selected."]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":29,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How would gatecase change outcomes across several stages?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":30,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"analysis_domain_gate","args":["guarded_system","several stages"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":31,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How would gatecase change outcomes across several stages?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":32,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"analysis_domain_gate","args":["guarded_system","several stages"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":33,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How would gatecase change outcomes across several stages?"}}}'

  # A richer consumer can teach the planner to decline without C routing edits.
  printf '%s\n' '{"jsonrpc":"2.0","id":34,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Transmuteform: how does a number behave under operations across several stages?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":35,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["runtime_guard_request","transmuteform"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":36,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"compound_guard","args":["analysis_plan","runtime_guard_request"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":37,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Transmuteform: how does a number behave under operations across several stages?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":38,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["runtime_guard_request","transmuteform"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":39,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Transmuteform: how does a number behave under operations across several stages?"}}}'

  # Families can require their own discriminating evidence at runtime.
  printf '%s\n' '{"jsonrpc":"2.0","id":40,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"analysis_family_cue","args":["runtime_family","zentropic"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":41,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"analysis_family_gate","args":["runtime_family","sealedfamily"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":42,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How would zentropic effects change outcomes across several stages?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":43,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"analysis_family_gate","args":["runtime_family","several stages"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":44,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How would zentropic effects change outcomes across several stages?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":45,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"analysis_family_gate","args":["runtime_family","several stages"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":46,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How would zentropic effects change outcomes across several stages?"}}}'
} | PARROT0_SESSION= PARROT0_PROFILE= PARROT0_WORLD_FACTS=1 "$BIN" --mcp-engine 2>/dev/null)"

line() { printf '%s\n' "$out" | grep -F "\"id\":$1,"; }
pass=0 fail=0
ok() { echo "PASS analysis_planner_growth: $1"; pass=$((pass+1)); }
no() { echo "FAIL analysis_planner_growth: $1" >&2; fail=$((fail+1)); }
has() {
  if line "$1" | grep -Fq "$2"; then ok "$3"; else no "$3: $(line "$1")"; fi
}
lacks() {
  if line "$1" | grep -Fq "$2"; then no "$3: $(line "$1")"; else ok "$3"; fi
}

rpc_ok=1
for id in $(seq 1 46); do
  row="$(line "$id" || true)"
  rows="$(printf '%s\n' "$row" | sed '/^$/d' | wc -l)"
  if [ "$rows" -ne 1 ] || printf '%s\n' "$row" | grep -Fq '"isError":true' ||
     printf '%s\n' "$row" | grep -Fq '"error":'; then
    rpc_ok=0
    break
  fi
done
[ "$rpc_ok" -eq 1 ] && ok "every mutation/probe returned one successful RPC response" ||
  no "missing or errored RPC response at id $id"

lacks 2  'A complex system is shaped' "domain cue absent before teaching"
has   4  'A complex system is shaped' "domain cue learned without rebuild"
lacks 6  'A complex system is shaped' "domain cue retraction removes recognition"
lacks 7  'A complex system is shaped' "act cue absent before teaching"
has   9  'A complex system is shaped' "act cue learned without rebuild"
lacks 11 'A complex system is shaped' "act cue retraction removes recognition"
lacks 12 '1. A complex system is shaped' "format cue absent before teaching"
has   14 '1. A complex system is shaped' "format cue learned without rebuild"
lacks 16 '1. A complex system is shaped' "format cue retraction restores paragraph"
lacks 20 'A complex system is shaped' "incomplete required plan emits no partial answer"
lacks 21 'Runtime family selected' "unknown family cue does not claim"
has   23 'Runtime family selected' "family cue learned without rebuild"
lacks 25 'Runtime family selected' "family cue retraction removes recognition"
lacks 29 'Guarded domain selected' "specific domain support cannot bypass its gate"
has   31 'Guarded domain selected' "domain gate cue learned without rebuild"
lacks 33 'Guarded domain selected' "domain gate retraction removes the claim"
has   34 'A proposed mathematical object needs explicit axioms' "planner claims before a richer guard is taught"
lacks 37 'A proposed mathematical object needs explicit axioms' "runtime guard cue makes the planner decline"
has   39 'A proposed mathematical object needs explicit axioms' "guard cue retraction restores planner routing"
lacks 42 'Runtime family selected' "family support cannot bypass its gate"
has   44 'Runtime family selected' "family gate cue learned without rebuild"
lacks 46 'Runtime family selected' "family gate retraction removes the claim"

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
