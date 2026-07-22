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
  call '{"jsonrpc":"2.0","id":21,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What word in the English language has three consecutive double letters?"}}}'
  call '{"jsonrpc":"2.0","id":22,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What word has three paired-letter streaks?"}}}'
  call '{"jsonrpc":"2.0","id":23,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["word_double_runs","paired-letter streaks"]}}}'
  call '{"jsonrpc":"2.0","id":24,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What word has three paired-letter streaks?"}}}'
  call '{"jsonrpc":"2.0","id":25,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["word_double_runs","paired-letter streaks"]}}}'
  call '{"jsonrpc":"2.0","id":26,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What word has three paired-letter streaks?"}}}'
  call '{"jsonrpc":"2.0","id":27,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Describe the taste of a color to someone who has never seen."}}}'
  call '{"jsonrpc":"2.0","id":28,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"synesthetic_default","args":["synesthetic_description","blue"]}}}'
  call '{"jsonrpc":"2.0","id":29,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Describe the taste of a color to someone who has never seen."}}}'
  call '{"jsonrpc":"2.0","id":30,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"synesthetic_default","args":["synesthetic_description","blue"]}}}'
  call '{"jsonrpc":"2.0","id":31,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Flavor-imagine blue for someone who cannot see it."}}}'
  call '{"jsonrpc":"2.0","id":32,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["synesthetic_description","flavor-imagine"]}}}'
  call '{"jsonrpc":"2.0","id":33,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Flavor-imagine blue for someone who cannot see it."}}}'
  call '{"jsonrpc":"2.0","id":34,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["synesthetic_description","flavor-imagine"]}}}'
  call '{"jsonrpc":"2.0","id":35,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Flavor-imagine blue for someone who cannot see it."}}}'
  call '{"jsonrpc":"2.0","id":36,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Write a short opening line for a mystery story set in a place that has never seen the sun."}}}'
  call '{"jsonrpc":"2.0","id":37,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What is the capital of the country that borders both Poland and Ukraine?"}}}'
  call '{"jsonrpc":"2.0","id":38,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"If you rearrange the letters in \"FLUSTER,\" you can also form which other common English word?"}}}'
  call '{"jsonrpc":"2.0","id":39,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Complete this analogy: Book is to Reading as Fork is to ______."}}}'
  call '{"jsonrpc":"2.0","id":40,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"All cats are mammals. Some mammals are pets. Therefore, what can we definitely conclude?"}}}'
  call '{"jsonrpc":"2.0","id":41,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"A train leaves New York at 60 mph. Another train leaves Los Angeles at 80 mph. They are 2,800 miles apart. How long before they meet?"}}}'
  call '{"jsonrpc":"2.0","id":42,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What is the past participle of the verb \"to forego\"?"}}}'
  call '{"jsonrpc":"2.0","id":43,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"meet_time","args":["2800","60","80",null]}}}'
  call '{"jsonrpc":"2.0","id":44,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Incipit hook for a mystery set in a place that has never seen the sun."}}}'
  call '{"jsonrpc":"2.0","id":45,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["story_opening_request","incipit hook"]}}}'
  call '{"jsonrpc":"2.0","id":46,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Incipit hook for a mystery set in a place that has never seen the sun."}}}'
  call '{"jsonrpc":"2.0","id":47,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["story_opening_request","incipit hook"]}}}'
  call '{"jsonrpc":"2.0","id":48,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Incipit hook for a mystery set in a place that has never seen the sun."}}}'
  call '{"jsonrpc":"2.0","id":49,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What is the third form of forego?"}}}'
  call '{"jsonrpc":"2.0","id":50,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["participle_query","third form"]}}}'
  call '{"jsonrpc":"2.0","id":51,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What is the third form of forego?"}}}'
  call '{"jsonrpc":"2.0","id":52,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["participle_query","third form"]}}}'
  call '{"jsonrpc":"2.0","id":53,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What is the third form of forego?"}}}'
  call '{"jsonrpc":"2.0","id":54,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Two trains travel at 60 mph and 80 mph, 2800 miles apart. How many hours rendezvous after?"}}}'
  call '{"jsonrpc":"2.0","id":55,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["train_meet_time","rendezvous after"]}}}'
  call '{"jsonrpc":"2.0","id":56,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Two trains travel at 60 mph and 80 mph, 2800 miles apart. How many hours rendezvous after?"}}}'
  call '{"jsonrpc":"2.0","id":57,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["train_meet_time","rendezvous after"]}}}'
  call '{"jsonrpc":"2.0","id":58,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Two trains travel at 60 mph and 80 mph, 2800 miles apart. How many hours rendezvous after?"}}}'
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
for id in $(seq 1 58); do
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
has  21 'bookkeeper' "three consecutive double letters answered by lexical constraint"
lacks 22 'bookkeeper' "novel double-run cue absent before teaching"
has  24 'bookkeeper' "novel double-run cue taught at runtime"
lacks 26 'bookkeeper' "novel double-run cue retract removes recognition"
has  27 'Imagine blue as' "generic color synesthesia uses KB default"
has  27 'cool water' "synesthetic description is rendered from KB"
lacks 29 'cool water' "synesthetic default retract removes generic color answer"
lacks 31 'cool water' "novel synesthetic cue absent before teaching"
has  33 'cool water' "novel synesthetic cue taught at runtime"
lacks 35 'cool water' "novel synesthetic cue retract removes recognition"
has  36 'footprint' "mystery opening line uses story KB instead of synesthesia"
lacks 36 'cool water' "story opening no longer hijacked by synesthetic cue"
has  37 'Minsk' "border intersection capital resolves through borders plus capital facts"
has  38 'restful' "full anagram uses all FLUSTER letters"
has  39 'eating' "analogy relation registry reaches used_for facts"
has  40 'not that cats are pets' "undistributed middle states the limited conclusion"
has  41 '20 hours' "train meet time computed from combined speed"
has  42 'foregone' "past participle fact answers forego"
has  43 '20' "meet_time procedure computes D/(V1+V2)"
lacks 44 'footprint' "novel story-opening cue absent before teaching"
has  46 'footprint' "novel story-opening cue taught at runtime"
lacks 48 'footprint' "novel story-opening cue retract removes recognition"
lacks 49 'foregone' "novel participle cue absent before teaching"
has  51 'foregone' "novel participle cue taught at runtime"
lacks 53 'foregone' "novel participle cue retract removes recognition"
lacks 54 '20 hours' "novel train-meet cue absent before teaching"
has  56 '20 hours' "novel train-meet cue taught at runtime"
lacks 58 '20 hours' "novel train-meet cue retract removes recognition"

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
