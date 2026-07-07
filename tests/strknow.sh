#!/usr/bin/env bash
#
# gen285 (U4, NEXT.md / teach-comprehension-via-mcp.md §5.3/§6.1) — a string
# ACTION as KNOWLEDGE. "Show a word with its first letter capitalized" is not a C
# primitive: it is a Horn rule over a list of characters, plus an upper/2 fact
# table (the case map). The only fixed, operation-BLIND piece is chars/2, the
# bidirectional atom<->char-list (de)serialization:
#
#   upper(m, M). upper(a, A). ...                       % the map = knowledge
#   cap_first($S,$R) :- chars($S,cons($H,$T)), upper($H,$U), chars($R,cons($U,$T)).
#
# cap_first(madrid, $R)  =>  $R = Madrid  — computed by rules+facts, not C.
# (Post-flip the char "M" is a constant, so no quoting games.)
#
# RED before U4: chars/2 is undefined, so the body fails. Tested via .p0 AND MCP.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "strknow: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS strknow: $1"; pass=$((pass+1)); }
no() { echo "FAIL strknow: $1" >&2; fail=$((fail+1)); }

# ---- Path 1: taught from a .p0 file ----
base="$(mktemp /tmp/strknow.XXXXXX.p0)"
trap 'rm -f "$base"' EXIT
cat > "$base" <<'P0'
upper(m, M).
upper(a, A).
upper(d, D).
cap_first($S, $R) :- chars($S, cons($H, $T)), upper($H, $U), chars($R, cons($U, $T)).
P0

out1="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"cap_first","args":["madrid",null]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"cap_first","args":["apple",null]}}}'
} | PARROT0_BASE="$base" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l1() { printf '%s\n' "$out1" | grep -F "\"id\":$1"; }

if l1 2 | grep -q 'Madrid'; then
    ok ".p0: capitalize_first as a RULE (madrid -> Madrid)"
else
    no ".p0: cap_first(madrid,?) wrong: $(l1 2)"
fi
if l1 3 | grep -q 'Apple'; then
    ok ".p0: same rule generalizes (apple -> Apple)"
else
    no ".p0: cap_first(apple,?) wrong: $(l1 3)"
fi

# ---- Path 2: the SAME action taught over MCP ----
out2="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"upper","args":["h","H"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.assert_clause","arguments":{"head":{"pred":"cap_first","args":["$S","$R"]},"body":[{"pred":"chars","args":["$S","cons($H, $T)"]},{"pred":"upper","args":["$H","$U"]},{"pred":"chars","args":["$R","cons($U, $T)"]}]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"cap_first","args":["hi",null]}}}'
} | PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l2() { printf '%s\n' "$out2" | grep -F "\"id\":$1"; }

if l2 4 | grep -q 'Hi'; then
    ok "MCP: string action taught via kb.assert_clause (hi -> Hi)"
else
    no "MCP: cap_first(hi,?) wrong: $(l2 4)"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
