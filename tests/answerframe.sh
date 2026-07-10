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

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
