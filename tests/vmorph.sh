#!/usr/bin/env bash
#
# gen289 (U5 rule 4, NEXT.md / teach-comprehension-via-mcp.md §5.5) — the FOURTH
# and last glue rule of Secchio B: the English present progressive "is V-ing" ->
# finite verb. The only verb morphology in the C translator was a hardcoded
# strcmp(next, "sleeping") in the FR path; it becomes knowledge — the auxiliary
# is a fact and the "-ing" ending is a real recursive morphology RULE over the
# char list (via chars/2):
#
#   aux_progressive(is).
#   progressive($W) :- chars($W, $L), ends_ing($L).
#   ends_ing(cons(i, cons(n, cons(g, nil)))).
#   ends_ing(cons($H, $T)) :- ends_ing($T).
#
# This generalizes the auxiliary drop to ANY -ing verb (running, reading), not
# just the one hardcoded word. RED before: the predicates do not exist.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "vmorph: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS vmorph: $1"; pass=$((pass+1)); }
no() { echo "FAIL vmorph: $1" >&2; fail=$((fail+1)); }

# ---- GATE A: regression — the progressive auxiliary is dropped, now via the rule ----
out="$(printf '%s\n' \
    'translate into French: "The cat is sleeping on the warm rug."' \
    '/quit' | PARROT0_LANG=en "$BIN" 2>/dev/null)"
got="$(printf '%s\n' "$out" | sed -n '1p')"
if [ "$got" = "Le chat dort sur le tapis chaud." ]; then
    ok "FR present progressive: 'is sleeping' -> finite (Le chat dort ...)"
else
    no "FR progressive: want [Le chat dort sur le tapis chaud.] got [$got]"
fi

# ---- GATE B: the morphology is now a RULE — queryable via MCP (red before) ----
mcp="$(printf '%s\n' \
    '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}' \
    '{"jsonrpc":"2.0","method":"notifications/initialized"}' \
    '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"progressive","args":["sleeping"]}}}' \
    '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"progressive","args":["running"]}}}' \
    '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"progressive","args":["cat"]}}}' \
    '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"aux_progressive","args":["is"]}}}' \
    | PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"
ml() { printf '%s\n' "$mcp" | grep -F "\"id\":$1"; }

if ml 2 | grep -q 'true'; then ok "MCP: progressive(sleeping) via -ing morphology rule"
else no "MCP: progressive(sleeping) wrong: $(ml 2)"; fi
if ml 3 | grep -q 'true'; then ok "MCP: rule GENERALIZES (progressive(running), not hardcoded)"
else no "MCP: progressive(running) wrong: $(ml 3)"; fi
if ml 4 | grep -q 'false'; then ok "MCP: progressive(cat) false (no -ing suffix)"
else no "MCP: progressive(cat) should be false: $(ml 4)"; fi
if ml 5 | grep -q 'true'; then ok "MCP: aux_progressive(is) is knowledge"
else no "MCP: aux_progressive(is) wrong: $(ml 5)"; fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
