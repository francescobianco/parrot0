#!/usr/bin/env bash
#
# gen306 (teach-comprehension-via-mcp.md) — COMPREHENSION as teachable KNOWLEDGE.
#
# The autolearn ledger's dominant "engine gap" was the CONSUMER gap: the teacher
# can assert any new relation (alloy_of, capital_on_river, chemical_symbol...),
# but parrot0 had no C module that parses the matching QUESTION and queries that
# predicate — so the fact sat inert. mod_answer_frame closes this with one datum:
#
#   answer_frame("<cue substring>", "<pred>").
#
# When a turn contains the cue, the module scans the turn's content words w and
# tries pred(w, ?) then pred(?, w), verbalizing the hit. Teaching a brand-new
# question FORM is now one fact over MCP — no C edit, no new module.
#
# RED before gen306: alloy_of(bronze, copper) asserted alone answers nothing
# ("what is bronze an alloy of?" is inert). GREEN: + answer_frame => "Copper."
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "answerframe: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS answerframe: $1"; pass=$((pass+1)); }
no() { echo "FAIL answerframe: $1" >&2; fail=$((fail+1)); }

# ---- Path 1 (RED baseline): the relation alone is inert ----
# Assert only alloy_of; ask the question. Without an answer_frame the module
# declines and no module knows how to consume the novel predicate.
red="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"alloy_of","args":["bronze","copper"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"what is bronze an alloy of?"}}}'
} | PARROT0_SESSION= PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

if printf '%s\n' "$red" | grep -F '"id":3' | grep -qi 'copper'; then
    no "RED baseline should NOT answer without an answer_frame: $(printf '%s\n' "$red" | grep -F '"id":3')"
else
    ok "RED: novel relation alone is inert (no consumer)"
fi

# ---- Path 2 (GREEN): teach relation + answer_frame => comprehension ----
green="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"alloy_of","args":["bronze","copper"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"answer_frame","args":["alloy of","alloy_of"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"what is bronze an alloy of?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"what is the capital of france?"}}}'
} | PARROT0_SESSION= PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

gl() { printf '%s\n' "$green" | grep -F "\"id\":$1"; }

if gl 4 | grep -qi 'copper'; then
    ok "GREEN: taught answer_frame answers the novel question form (-> Copper)"
else
    no "GREEN: expected 'Copper', got: $(gl 4)"
fi

# ---- Path 3 (no false positive): unrelated question keeps its normal answer ----
if gl 5 | grep -qi 'paris' && ! gl 5 | grep -qi 'copper'; then
    ok "no false positive: an unrelated turn is not hijacked (-> Paris)"
else
    no "false-positive guard failed: $(gl 5)"
fi

# ---- Path 4: one cue can grow more than one candidate predicate -----------
# `answer_frame` is a registry, not a function.  The first mapping below is a
# deliberate sterile collision: transitive/1 cannot answer the binary lookup
# performed by the generic frame.  Adding relation_type/2 under the SAME cue
# must become live immediately; retracting only that mapping must remove it.
#
# The same live MCP process also proves that the pre-existing arity frame was
# never the problem: a relation_arity/2 fact makes it answer immediately, and
# retracting the fact removes the answer again.
multi="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"relation_type","args":["mirror_link","involutive"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"answer_frame","args":["involutive","transitive"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"which relations are involutive?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"answer_frame","args":["involutive","relation_type"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"which relations are involutive?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"answer_frame","args":["involutive","relation_type"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"which relations are involutive?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":9,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"what is the arity of ternary_bridge?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":10,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"relation_arity","args":["ternary_bridge",3]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":11,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"what is the arity of ternary_bridge?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":12,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"relation_arity","args":["ternary_bridge",3]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":13,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"what is the arity of ternary_bridge?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":14,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"which relations are transitive?"}}}'
} | PARROT0_SESSION= PARROT0_PROFILE= PARROT0_WORLD_FACTS=0 "$BIN" --mcp-engine 2>/dev/null)"

ml() { printf '%s\n' "$multi" | grep -F "\"id\":$1,"; }

rpc_ok=1
for id in $(seq 1 14); do
    [ "$(printf '%s\n' "$multi" | grep -Fc "\"id\":$id,")" -eq 1 ] || rpc_ok=0
done
if [ "$rpc_ok" -eq 1 ]; then
    ok "many-to-one/arity runtime-growth RPC sequence completed"
else
    no "many-to-one/arity runtime-growth RPC sequence was incomplete"
fi

if ! ml 4 | grep -Fqi 'Mirror link.'; then
    ok "collision RED: the sterile first mapping cannot answer"
else
    no "collision RED unexpectedly reached the later relation: $(ml 4)"
fi
if ml 6 | grep -Fqi 'Mirror link.'; then
    ok "collision GREEN: a second predicate under the same cue becomes live"
else
    no "collision GREEN did not reach the second predicate: $(ml 6)"
fi
if ! ml 8 | grep -Fqi 'Mirror link.'; then
    ok "collision ablation: retracting the second mapping removes recognition"
else
    no "collision mapping survived retraction: $(ml 8)"
fi

if ! ml 9 | grep -Fq '\"output\":\"3.\"'; then
    ok "arity RED: no answer exists before relation_arity/2 is taught"
else
    no "arity RED unexpectedly answered: $(ml 9)"
fi
if ml 11 | grep -Fq '\"output\":\"3.\"'; then
    ok "arity GREEN: a runtime relation_arity/2 fact is consumed"
else
    no "arity GREEN did not answer 3: $(ml 11)"
fi
if ! ml 13 | grep -Fq '\"output\":\"3.\"'; then
    ok "arity ablation: retracting relation_arity/2 removes the answer"
else
    no "arity fact survived retraction: $(ml 13)"
fi

if ml 14 | grep -Fqi 'is a' && ml 14 | grep -Fqi 'part of'; then
    ok "L14 regression: the built-in transitive collision reaches relation_type/2"
else
    no "L14 transitive frame still walled: $(ml 14)"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
