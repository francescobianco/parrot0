#!/usr/bin/env bash
#
# gen309 (docs/plans/teach-comprehension-via-mcp.md) — SUPERLATIVE AGGREGATION as
# teachable knowledge. The class autolearn flagged "Stored facts did not
# materially change the answer path": "which river runs through the MOST capital
# cities?" is not a stored value but a FOLD over capital_on_river/2 (group by
# river, count capitals, take the max). The knowledge is one datum,
#   aggregate_frame(Cue, Pred, ReturnArg, Mode),
# and mod_aggregate supplies only the bounded counting driver (grammar is
# knowledge, the fold is C — the same split as answer_frame).
#
# GATE A (e2e): "the most capital cities" -> Danube. (4 capitals, the real max),
#   with no false positive (capital of France -> Paris.).
# GATE B (data-ness): a fresh relation + aggregate_frame taught over MCP composes,
#   and Mode is data too (max -> Spain, min -> Italy over the same facts).
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "aggregate: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS aggregate: $1"; pass=$((pass+1)); }
no() { echo "FAIL aggregate: $1" >&2; fail=$((fail+1)); }

# ---- GATE A: e2e over the shipped world facts ----
out="$(printf '%s\n' \
    'Which river runs through the most capital cities in the world?' \
    'What is the capital of France?' \
    '/quit' | PARROT0_LANG=en "$BIN" 2>/dev/null)"
check_line() { local got; got="$(printf '%s\n' "$out" | sed -n "${1}p")"
    if [ "$got" = "$2" ]; then ok "$3 ($2)"; else no "$3: want [$2] got [$got]"; fi; }
check_line 1 "Danube."  "superlative fold: most capitals -> Danube"
check_line 2 "Paris."   "no false positive: capital-of stays a lookup"

# ---- GATE B: a fresh aggregate is teachable over MCP, Mode included ----
mcp="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"won_by","args":["m1","spain"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"won_by","args":["m2","spain"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"won_by","args":["m3","italy"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"aggregate_frame","args":["most matches","won_by","second","max"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"aggregate_frame","args":["fewest matches","won_by","second","min"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Which team won the most matches?"}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Which team won the fewest matches?"}}}'
} | PARROT0_SESSION= PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"
ml() { printf '%s\n' "$mcp" | grep -F "\"id\":$1"; }

if ml 7 | grep -qi 'spain'; then ok "MCP: taught aggregate composes (max -> Spain)"
else no "MCP: most-matches wrong: $(ml 7)"; fi
if ml 8 | grep -qi 'italy'; then ok "MCP: Mode is data too (min -> Italy)"
else no "MCP: fewest-matches wrong: $(ml 8)"; fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
