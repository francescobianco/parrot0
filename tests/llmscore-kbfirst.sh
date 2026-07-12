#!/usr/bin/env bash
# Runtime-growth oracle for the LLMSCORE counterexamples. One MCP process owns
# one live KB: red -> assert cue -> green -> retract -> red proves the compiled
# engine contains mechanics, not the tested surface vocabulary.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "llmscore-kbfirst: binary not built ($BIN)" >&2; exit 1; }
cd "$ROOT" || exit 1

pass=0 fail=0
ok() { echo "PASS llmscore-kbfirst: $1"; pass=$((pass+1)); }
no() { echo "FAIL llmscore-kbfirst: $1" >&2; fail=$((fail+1)); }
call() { printf '%s\n' "$1"; }

out="$( {
  call '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'

  # Q8: "amid" is absent, becomes a range opener live, then disappears.
  call '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How many prime numbers are there amid 1 and 10?"}}}'
  call '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["arith_range_between_open","keyword(amid)"]}}}'
  call '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How many prime numbers are there amid 1 and 10?"}}}'
  call '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["arith_range_between_open","keyword(amid)"]}}}'
  call '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How many prime numbers are there amid 1 and 10?"}}}'

  # Q4: the projection operator is new; wordwise scope is existing KB data.
  call '{"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Take the acrostic heads of each word in \"Red green blue\"."}}}'
  call '{"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["initials_projection","acrostic heads"]}}}'
  call '{"jsonrpc":"2.0","id":9,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Take the acrostic heads of each word in \"Red green blue\"."}}}'
  call '{"jsonrpc":"2.0","id":10,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["initials_projection","acrostic heads"]}}}'
  call '{"jsonrpc":"2.0","id":11,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Take the acrostic heads of each word in \"Red green blue\"."}}}'

  # Q6: a new relationship marker feeds the same structural pair checker.
  call '{"jsonrpc":"2.0","id":12,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"If no Glarps are Flims, what follows about the connection between Glarps and Flims?"}}}'
  call '{"jsonrpc":"2.0","id":13,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["logic_relationship_marker","keyword(connection)"]}}}'
  call '{"jsonrpc":"2.0","id":14,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"If no Glarps are Flims, what follows about the connection between Glarps and Flims?"}}}'
  call '{"jsonrpc":"2.0","id":15,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["logic_relationship_marker","keyword(connection)"]}}}'
  call '{"jsonrpc":"2.0","id":16,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"If no Glarps are Flims, what follows about the connection between Glarps and Flims?"}}}'

  # Q10: self-preference routing is also live KB knowledge, not a C question list.
  call '{"jsonrpc":"2.0","id":17,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What pursuits have occupied you lately?"}}}'
  call '{"jsonrpc":"2.0","id":18,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["self_preference","occupied you lately"]}}}'
  call '{"jsonrpc":"2.0","id":19,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What pursuits have occupied you lately?"}}}'
  call '{"jsonrpc":"2.0","id":20,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["self_preference","occupied you lately"]}}}'
  call '{"jsonrpc":"2.0","id":21,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What pursuits have occupied you lately?"}}}'

  # Q4 collision control: restoring "letters of" must preserve implicit anagrams.
  call '{"jsonrpc":"2.0","id":22,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What word can you form from the letters of listen?"}}}'
} | PARROT0_BASE= PARROT0_SESSION= PARROT0_PROFILE= PARROT0_WORLD_FACTS=1 \
    "$BIN" --mcp-engine 2>/dev/null)"

line() { printf '%s\n' "$out" | grep -F "\"id\":$1,"; }
has() {
  local id="$1" pattern="$2" label="$3"
  if line "$id" | grep -Fq "$pattern"; then ok "$label"; else no "$label: $(line "$id")"; fi
}
lacks() {
  local id="$1" pattern="$2" label="$3"
  if line "$id" | grep -Fq "$pattern"; then no "$label: $(line "$id")"; else ok "$label"; fi
}

# A negative content assertion is meaningful only if every request actually
# produced one successful JSON-RPC response. Missing/error rows make the oracle
# red instead of letting lacks() turn infrastructure failure into fake ablation.
rpc_ok=1
for id in $(seq 1 22); do
  row="$(line "$id" || true)"
  rows="$(printf '%s\n' "$row" | sed '/^$/d' | wc -l)"
  if [ "$rows" -ne 1 ] || printf '%s\n' "$row" | grep -Fq '"isError":true' ||
     printf '%s\n' "$row" | grep -Fq '"error":'; then
    rpc_ok=0
    break
  fi
done
if [ "$rpc_ok" -eq 1 ]; then
  ok "every mutation/probe returned exactly one successful RPC response"
else
  no "missing or errored RPC response at id $id"
fi

lacks 2  'There are 4 prime numbers' "Q8 novel opener is absent before teaching"
has   4  'There are 4 prime numbers between 1 and 10 (inclusive).' "Q8 learns a range opener without rebuild"
lacks 6  'There are 4 prime numbers' "Q8 retract removes the learned opener"

lacks 7  'RGB' "Q4 novel projection cue is absent before teaching"
has   9  'RGB' "Q4 learns a projection cue without rebuild"
lacks 11 'RGB' "Q4 retract removes the learned projection cue"

lacks 12 'Those classes do not overlap' "Q6 novel relation marker is absent before teaching"
has   14 'Those classes do not overlap -- the explicit no-overlap premise states that directly.' "Q6 learns a relation marker without rebuild"
lacks 16 'Those classes do not overlap' "Q6 retract removes the learned marker"

lacks 17 'preferences' "Q10 novel self cue is absent before teaching"
has   19 'preferences' "Q10 learns a self-preference cue without rebuild"
lacks 21 'preferences' "Q10 retract restores conversational routing"

has   22 'silent' "Q4 initials exclusion preserves the implicit anagram surface"

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
