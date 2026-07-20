#!/usr/bin/env bash
# T06 bridge (dossier §10.4) brain-side ratchet: a repository-bound repair in
# one live MCP session per phase.  The KB drives every boundary — tool
# contract, repair rules, commit policy, even the recognition cue — so each
# ablation is a runtime retract, never a rebuild:
#   A  live policy + green oracle -> verified_candidate -> commit -> canonical
#      oracle verdict, with build/run and canonical_build/canonical_run as
#      distinct repair_observation facts;
#   B  policy absent -> Gap(commit_policy), the winner stays in state
#      verified_candidate, and the workspace is byte-identical;
#   C  tool_for retracted -> missing_tool before any staging; then a cue
#      TAUGHT at runtime routes the same binary to a committed repair.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "agentcommit: binary not built ($BIN)" >&2; exit 1; }

WS=".p0-bridge-$$"
REL="$WS/sort_broken.c"
trap 'rm -rf "$ROOT/$WS"' EXIT

BAD='void mysort(int a[], int n) { for (int i = 0; i < n; i++) { for (int j = 0; j < n - i; j++) { if (a[j] > a[j+1]) { int t = a[j]; a[j] = a[j+1]; a[j+1] = t; } } } }'

pass=0
fail=0
ok() { echo "PASS agentcommit: $1"; pass=$((pass + 1)); }
no() { echo "FAIL agentcommit: $1" >&2; fail=$((fail + 1)); }
call() { printf '%s\n' "$1"; }

write_broken() {
    mkdir -p "$ROOT/$WS"
    printf '%s\n' "$BAD" >"$ROOT/$REL"
    chmod 0644 "$ROOT/$REL"
}

mcp() { PARROT0_SESSION= PARROT0_BASE= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= \
        "$BIN" --mcp-engine 2>/dev/null; }

have() { # have <output> <id> <needle>
    if printf '%s\n' "$1" | grep -F "\"id\":$2," | grep -qF "$3"; then
        ok "$4"
    else
        no "$4 (id=$2 lacks $3)"
    fi
}

# ---------------------------------------------------------------- phase A --
write_broken
outA="$( {
  call '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  call '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  call "{\"jsonrpc\":\"2.0\",\"id\":2,\"method\":\"tools/call\",\"params\":{\"name\":\"kb.assert\",\"arguments\":{\"pred\":\"commit_policy\",\"args\":[\"edit\",\"$REL\"]}}}"
  call "{\"jsonrpc\":\"2.0\",\"id\":3,\"method\":\"tools/call\",\"params\":{\"name\":\"gen.respond\",\"arguments\":{\"input\":\"fix the code in $REL\"}}}"
  call '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_terminal","args":[null,"committed",null,null]}}}'
  call '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_commit","args":[null,null,"committed"]}}}'
  call '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_verdict","args":[null,null,null,"canonical_oracle"]}}}'
  call '{"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_observation","args":[null,null,"build",null]}}}'
  call '{"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_observation","args":[null,null,"run",null]}}}'
  call '{"jsonrpc":"2.0","id":9,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_observation","args":[null,null,"canonical_build",null]}}}'
  call '{"jsonrpc":"2.0","id":10,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_observation","args":[null,null,"canonical_run",null]}}}'
  call '{"jsonrpc":"2.0","id":11,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"rec","args":[null,"task","0","done"]}}}'
  call '{"jsonrpc":"2.0","id":12,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"rec_name","args":[null,"\"commit_candidate\""]}}}'
  call '{"jsonrpc":"2.0","id":13,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_decision","args":[null,null,"tighten_inner_bound",null]}}}'
} | mcp )"

have "$outA" 4  'bindings\":[\"' 'a live policy plus a green oracle ends in terminal committed'
have "$outA" 5  'bindings\":[\"' 'the commit is recorded as a repair_commit fact, not a sentence'
have "$outA" 6  'bindings\":[\"' 'the canonical oracle verdict is recorded after the commit'
have "$outA" 7  'bindings\":[\"' 'candidate build is a distinct observation'
have "$outA" 8  'bindings\":[\"' 'candidate run is a distinct observation'
have "$outA" 9  'bindings\":[\"' 'canonical build is a distinct observation after commit'
have "$outA" 10 'bindings\":[\"' 'canonical run is a distinct observation after commit'
have "$outA" 11 'bindings\":[\"' 'the task reaches done only through the commit'
have "$outA" 12 'bindings\":[\"' 'the commit_candidate action is in the record graph'
have "$outA" 13 'bindings\":[\"' 'the min-blast decision names the winning transform'
if grep -qF 'n - i - 1' "$ROOT/$REL"; then
    ok "the canonical file now carries the verified fix"
else
    no "the canonical file was not updated by the commit"
fi
if ! ls -d "$ROOT"/.p0-sort-* >/dev/null 2>&1; then
    ok "the sort oracle no longer stages through private .p0-sort-* trees"
else
    no "a private .p0-sort-* tree survived the bridge migration"
fi

# ---------------------------------------------------------------- phase B --
write_broken
cp "$ROOT/$REL" "$ROOT/$WS/reference.c"
outB="$( {
  call '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  call '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  call "{\"jsonrpc\":\"2.0\",\"id\":2,\"method\":\"tools/call\",\"params\":{\"name\":\"gen.respond\",\"arguments\":{\"input\":\"fix the code in $REL\"}}}"
  call '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_terminal","args":[null,"policy_gap",null,null]}}}'
  call '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"rec_name","args":[null,"\"commit_policy\""]}}}'
  call '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_commit","args":[null,null,"policy_denied"]}}}'
  call '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"rec","args":[null,"artifact",null,"verified_candidate"]}}}'
} | mcp )"

have "$outB" 3 'bindings\":[\"' 'a green candidate without a live policy stops at policy_gap'
have "$outB" 4 'bindings\":[\"' 'the stop is a named Gap(commit_policy) record'
have "$outB" 5 'bindings\":[\"' 'the denied commit is recorded as an outcome fact'
have "$outB" 6 'bindings\":[\"' 'the winner waits in verified_candidate, never done without the commit'
if cmp -s "$ROOT/$WS/reference.c" "$ROOT/$REL"; then
    ok "policy denial moved zero bytes in the workspace"
else
    no "policy denial still changed the canonical file"
fi

# ---------------------------------------------------------------- phase C --
write_broken
outC="$( {
  call '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  call '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  call '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"tool_for","args":["build","cc"]}}}'
  call "{\"jsonrpc\":\"2.0\",\"id\":3,\"method\":\"tools/call\",\"params\":{\"name\":\"gen.respond\",\"arguments\":{\"input\":\"fix the code in $REL\"}}}"
  call '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_terminal","args":[null,"missing_tool_contract",null,null]}}}'
  call '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"tool_for","args":["build","cc"]}}}'
  call "{\"jsonrpc\":\"2.0\",\"id\":6,\"method\":\"tools/call\",\"params\":{\"name\":\"kb.assert\",\"arguments\":{\"pred\":\"commit_policy\",\"args\":[\"edit\",\"$REL\"]}}}"
  call '{"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["code_fix","ripara il sorgente in"]}}}'
  call "{\"jsonrpc\":\"2.0\",\"id\":8,\"method\":\"tools/call\",\"params\":{\"name\":\"gen.respond\",\"arguments\":{\"input\":\"ripara il sorgente in $REL\"}}}"
  call '{"jsonrpc":"2.0","id":9,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"repair_terminal","args":[null,"committed",null,null]}}}'
} | mcp )"

have "$outC" 4 'bindings\":[\"' 'retracting tool_for stops verification before any staging or run'
have "$outC" 9 'bindings\":[\"' 'a cue taught at runtime routes the same binary to a committed repair'
if grep -qF 'n - i - 1' "$ROOT/$REL"; then
    ok "the runtime-taught cue ends in a real canonical commit"
else
    no "the runtime-taught cue did not reach the commit"
fi

echo "agentcommit: $pass passed, $fail failed"
[ "$fail" -eq 0 ]
