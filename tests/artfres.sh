#!/usr/bin/env bash
#
# gen288 (U5 rule 3, NEXT.md / teach-comprehension-via-mcp.md §5.5) — the THIRD
# glue rule of Secchio B migrated from C to knowledge: the FRENCH and SPANISH
# definite article. In the current translator FR/ES depend only on gender (no
# elision or indefinite, unlike Italian), so the tables are per-language,
# matching the tr_fr/gender_fr and tr_es/gender_es naming already in use:
#
#   article_fr(m, le).  article_fr(f, la).
#   article_es(m, el).  article_es(f, la).
#
# mod_translate queries these instead of the C ternary. RED before: the
# article_fr/article_es predicates do not exist.
#
# Note: the FR/ES lexicon (tr_es/gender_es) lives in world-facts.p0, so the
# translation regression runs the DEFAULT brain (not the hermetic PARROT0_BASE=
# env); only the MCP data-ness part is hermetic.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "artfres: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS artfres: $1"; pass=$((pass+1)); }
no() { echo "FAIL artfres: $1" >&2; fail=$((fail+1)); }

# ---- GATE A: regression — FR/ES article in translation, now via the tables ----
out="$(printf '%s\n' \
    'translate into French: "The cat is sleeping on the warm rug."' \
    'translate to spanish: the cat' \
    'translate to spanish: the house' \
    '/quit' | PARROT0_LANG=en "$BIN" 2>/dev/null)"
check_line() { local got; got="$(printf '%s\n' "$out" | sed -n "${1}p")"
    if [ "$got" = "$2" ]; then ok "$3 ($2)"; else no "$3: want [$2] got [$got]"; fi; }
check_line 1 "Le chat dort sur le tapis chaud." "FR masculine -> le"
check_line 2 "El gato."                          "ES masculine -> el"
check_line 3 "La casa."                          "ES feminine -> la"

# ---- GATE B: the FR/ES article is now DATA — queryable via MCP (red before) ----
mcp="$(printf '%s\n' \
    '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}' \
    '{"jsonrpc":"2.0","method":"notifications/initialized"}' \
    '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"article_fr","args":["m",null]}}}' \
    '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"article_fr","args":["f",null]}}}' \
    '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"article_es","args":["m",null]}}}' \
    '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"article_es","args":["f",null]}}}' \
    | PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"
ml() { printf '%s\n' "$mcp" | grep -F "\"id\":$1"; }

if ml 2 | grep -q '\\"le\\"'; then ok "MCP: article_fr(m,?) is knowledge -> le"
else no "MCP: article_fr(m,?) wrong: $(ml 2)"; fi
if ml 3 | grep -q '\\"la\\"'; then ok "MCP: article_fr(f,?) is knowledge -> la"
else no "MCP: article_fr(f,?) wrong: $(ml 3)"; fi
if ml 4 | grep -q '\\"el\\"'; then ok "MCP: article_es(m,?) is knowledge -> el"
else no "MCP: article_es(m,?) wrong: $(ml 4)"; fi
if ml 5 | grep -q '\\"la\\"'; then ok "MCP: article_es(f,?) is knowledge -> la"
else no "MCP: article_es(f,?) wrong: $(ml 5)"; fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
