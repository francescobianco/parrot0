#!/usr/bin/env bash
#
# gen286 (U5, NEXT.md / teach-comprehension-via-mcp.md §5.5) — the FIRST glue rule
# of Secchio B migrated from C to knowledge: the Italian article form. It used to
# be a C ternary cascade in src/brain/85-translate-synth-world.c (il/la/un/una and
# the elided l'/un'); it is now an article/4 fact table (kb/core/grammar.p0) that
# mod_translate queries. The fixed engine still classifies definiteness, gender
# and vowel-initial noun; the FORM it selects is now inspectable/teachable data.
#
#   article(Definiteness, Gender, VowelInitial, Form)
#
# RED before U5: kb.match article(...) returns nothing (the form lived only in C).
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "article: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS article: $1"; pass=$((pass+1)); }
no() { echo "FAIL article: $1" >&2; fail=$((fail+1)); }

# ---- GATE A: regression — translation output is unchanged, now via the table ----
# Each case exercises a different article form: il (def m), la (def f), una (indef
# f), and l' + un (def+vowel elision, indef m) in one clause.
out="$(printf '%s\n' \
    'translate to italian: the dog runs' \
    'translate to italian: the small house' \
    'translate to italian: a white door' \
    'translate to italian: the man reads a book' \
    '/quit' | PARROT0_LANG=en "$BIN" 2>/dev/null)"

check_line() { # $1 = 1-based line, $2 = expected, $3 = label
    local got; got="$(printf '%s\n' "$out" | sed -n "${1}p")"
    if [ "$got" = "$2" ]; then ok "$3 ($2)"; else no "$3: want [$2] got [$got]"; fi
}
check_line 1 "il cane corre"         "def masculine -> il"
check_line 2 "la piccola casa"       "def feminine -> la"
check_line 3 "una bianca porta"      "indef feminine -> una"
check_line 4 "l'uomo legge un libro" "def+vowel -> l', indef masc -> un"

# ---- GATE B: the grammar is now DATA — queryable via MCP (red before U5) ----
mcp="$(printf '%s\n' \
    '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}' \
    '{"jsonrpc":"2.0","method":"notifications/initialized"}' \
    '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"article","args":["def","m","no",null]}}}' \
    '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"article","args":["def","f","yes",null]}}}' \
    '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"article","args":["indef","f","yes",null]}}}' \
    | PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"
ml() { printf '%s\n' "$mcp" | grep -F "\"id\":$1"; }

if ml 2 | grep -q '\\"il\\"'; then ok "MCP: article(def,m,no,?) is knowledge -> il"
else no "MCP: article(def,m,no,?) wrong: $(ml 2)"; fi
if ml 3 | grep -q "l'"; then ok "MCP: elided article(def,f,yes,?) is knowledge -> l'"
else no "MCP: article(def,f,yes,?) wrong: $(ml 3)"; fi
if ml 4 | grep -q "un'"; then ok "MCP: elided article(indef,f,yes,?) is knowledge -> un'"
else no "MCP: article(indef,f,yes,?) wrong: $(ml 4)"; fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
