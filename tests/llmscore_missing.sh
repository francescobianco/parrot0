#!/usr/bin/env bash
# Focused regressions for the missing rows recorded in LLMSCORE.md.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "llmscore_missing: binary not built ($BIN)" >&2; exit 1; }

pass=0 fail=0
ok() { echo "PASS llmscore_missing: $1"; pass=$((pass+1)); }
no() { echo "FAIL llmscore_missing: $1" >&2; fail=$((fail+1)); }
call() { printf '%s\n' "$1"; }

out="$( {
  call '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  call '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Explain why the sky appears blue during the day but dark at night."}}}'
  call '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"You are organizing a dinner party for 7 guests. Each guest shakes hands with every other guest exactly once. How many total handshakes occur?"}}}'
  call '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"In a group of 8 people, everyone meets every other person once. How many pairwise meetings happen?"}}}'
  call '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Riddle: A man lives on the 10th floor. He takes the elevator down, but returns to the 7th floor and walks up. Why doesn"}}}'
  call '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What do you think is the most important quality for a leader to have, and why?"}}}'
  call '{"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What is the best quality for a teacher to have?"}}}'
  call '{"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"If you have a bucket containing 5 red balls, 3 blue balls, and 2 green balls, and you reach in blindfolded and grab 4 balls at random without looking, what is the probability that you will have at least 2 red balls?"}}}'
  call '{"jsonrpc":"2.0","id":9,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"A farmer needs to cross a river with a fox, a chicken, and a bag of grain. The boat can only carry the farmer plus one item at a time. If left alone together, the fox will eat the chicken, and the chicken will eat the grain. How does the farmer get all three across safely?"}}}'
  call '{"jsonrpc":"2.0","id":10,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"fav_at_least","args":["hg(5, 5, 4, 2)",null]}}}'
  call '{"jsonrpc":"2.0","id":11,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"choose","args":["10","4",null]}}}'
  call '{"jsonrpc":"2.0","id":12,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"probability_procedure","args":["probability_at_least_count","favorable","fav_at_least"]}}}'
  call '{"jsonrpc":"2.0","id":13,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"In a bag with 4 red balls and 2 blue balls, chance pick 2 balls. What is the probability of at least 1 red ball?"}}}'
  call '{"jsonrpc":"2.0","id":14,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"probability_procedure","args":["probability_at_least_count","favorable","fav_at_least"]}}}'
  call '{"jsonrpc":"2.0","id":15,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"In a bag with 4 red balls and 2 blue balls, chance pick 2 balls. What is the probability of at least 1 red ball?"}}}'
  call '{"jsonrpc":"2.0","id":16,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"In a bag with 4 red balls and 2 blue balls, mystery take 2 balls. What is the result for at least 1 red ball?"}}}'
  call '{"jsonrpc":"2.0","id":17,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["probability_draw","mystery take"]}}}'
  call '{"jsonrpc":"2.0","id":18,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"In a bag with 4 red balls and 2 blue balls, mystery take 2 balls. What is the result for at least 1 red ball?"}}}'
  call '{"jsonrpc":"2.0","id":19,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["probability_draw","mystery take"]}}}'
  call '{"jsonrpc":"2.0","id":20,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"In a bag with 4 red balls and 2 blue balls, mystery take 2 balls. What is the result for at least 1 red ball?"}}}'
} | PARROT0_SESSION= PARROT0_PROFILE= PARROT0_WORLD_FACTS=1 "$BIN" --mcp-engine 2>/dev/null)"

line() { printf '%s\n' "$out" | grep -F "\"id\":$1,"; }
has() {
  local id="$1" pattern="$2" label="$3"
  if line "$id" | grep -Fqi "$pattern"; then ok "$label"; else no "$label: $(line "$id")"; fi
}
lacks() {
  local id="$1" pattern="$2" label="$3"
  if line "$id" | grep -Fqi "$pattern"; then no "$label: $(line "$id")"; else ok "$label"; fi
}

rpc_ok=1
for id in $(seq 1 20); do
  row="$(line "$id" || true)"
  rows="$(printf '%s\n' "$row" | sed '/^$/d' | wc -l)"
  if [ "$rows" -ne 1 ] || printf '%s\n' "$row" | grep -Fq '"isError":true' ||
     printf '%s\n' "$row" | grep -Fq '"error":'; then
    rpc_ok=0
    break
  fi
done
if [ "$rpc_ok" -eq 1 ]; then
  ok "every probe returned exactly one successful RPC response"
else
  no "missing or errored RPC response at id $id"
fi

has   2 'blue because sunlight scatters' "compound sky/day/night explanation includes blue-sky cause"
has   2 'faces away from the Sun' "compound sky/day/night explanation includes night cause"
lacks 2 'Earth spins on its axis' "specific sky/day/night answer beats generic day-night template"

has   3 '21.' "7-person handshake count"
has   4 '28.' "8-person pairwise count generalizes"

has   5 'too short to reach' "elevator riddle answered from KB signature"
has   6 'trustworthiness' "leader quality answered"
has   7 'patience' "teacher quality generalizes beyond leader"
has   8 '31/42' "without-replacement probability simplified fraction"
has   8 '155 favorable draws out of 210' "without-replacement probability uses favorable/total count"
has   9 'Take the chicken first' "chicken/grain river crossing answered from KB signature"
has   9 'take the grain across' "river crossing preserves grain variant"
has  10 '155' "hypergeometric favorable count is computed by KB procedure"
has  11 '210' "combination denominator is computed by KB procedure"
lacks 13 '14/15' "probability procedure registry retract removes computation"
has  15 '14/15' "probability procedure registry restore enables computation"
lacks 16 '14/15' "probability draw cue absent before teaching"
has  18 '14/15' "probability draw cue taught at runtime"
lacks 20 '14/15' "probability draw cue retract removes recognition"

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
