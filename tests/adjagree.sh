#!/usr/bin/env bash
#
# gen287 (U5 rule 2, NEXT.md / teach-comprehension-via-mcp.md §5.5) — the SECOND
# glue rule of Secchio B migrated from C to knowledge: feminine adjective
# agreement. It used to be agree_adj() in C (-o -> -a for feminine, invariant
# adjectives untouched). It is now a COMPOSITIONAL morphology rule in
# kb/core/grammar.p0, reusing U3 (structural cons-list unification) and U4
# (chars/2): swap the LAST character via the fem/2 ending map.
#
#   fem(o, a).
#   agree_f($A,$R) :- chars($A,$L), swap_last($L,$L2), chars($R,$L2).
#   swap_last(cons($H,nil),cons($U,nil)) :- fem($H,$U).
#   swap_last(cons($H,$T), cons($H,$T2)) :- swap_last($T,$T2).
#
# piccolo -> piccola, bianco -> bianca; grande (no fem(e,_)) stays invariant.
# The symmetric twin of U4's capitalize_first (which swaps the FIRST char).
# RED before U5-r2: agree_f is undefined, so the rule fails.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "adjagree: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS adjagree: $1"; pass=$((pass+1)); }
no() { echo "FAIL adjagree: $1" >&2; fail=$((fail+1)); }

# ---- GATE A: regression — feminine adjective agreement in translation ----
out="$(printf '%s\n' \
    'translate to italian: the small house' \
    'translate to italian: a white door' \
    '/quit' | PARROT0_LANG=en "$BIN" 2>/dev/null)"
check_line() { local got; got="$(printf '%s\n' "$out" | sed -n "${1}p")"
    if [ "$got" = "$2" ]; then ok "$3 ($2)"; else no "$3: want [$2] got [$got]"; fi; }
check_line 1 "la piccola casa"   "feminine agreement piccolo->piccola"
check_line 2 "una bianca porta"  "feminine agreement bianco->bianca"

# ---- GATE B: morphology is now a DATA+RULE — queryable via MCP (red before) ----
mcp="$(printf '%s\n' \
    '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}' \
    '{"jsonrpc":"2.0","method":"notifications/initialized"}' \
    '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"agree_f","args":["piccolo",null]}}}' \
    '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"agree_f","args":["bianco",null]}}}' \
    '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"agree_f","args":["grande",null]}}}' \
    | PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"
ml() { printf '%s\n' "$mcp" | grep -F "\"id\":$1"; }

if ml 2 | grep -q 'piccola'; then ok "MCP: agree_f(piccolo,?) is a rule -> piccola"
else no "MCP: agree_f(piccolo,?) wrong: $(ml 2)"; fi
if ml 3 | grep -q 'bianca'; then ok "MCP: same rule generalizes (bianco -> bianca)"
else no "MCP: agree_f(bianco,?) wrong: $(ml 3)"; fi
# invariant adjective: the rule must NOT fire (empty bindings [] ), so it stays.
if ml 4 | grep -q '\[\]' && ! ml 4 | grep -q 'grand'; then
    ok "MCP: invariant adjective yields no match (grande stays)"
else no "MCP: agree_f(grande,?) should be empty: $(ml 4)"; fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
