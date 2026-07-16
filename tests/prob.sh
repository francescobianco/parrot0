#!/usr/bin/env bash
#
# gen335+ — prob/2 KB-backed confidence.
# prob(Goal, $P) reads fact_confidence(Goal, P) from the KB.
# If no confidence fact exists, defaults to 0.5 (neutral). KB-first: the
# probabilities live in the KB, not in C.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "prob: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS prob: $1"; pass=$((pass+1)); }
no() { echo "FAIL prob: $1" >&2; fail=$((fail+1)); }

# ---- Path 1: prob reads fact_confidence from KB ----
base="$(mktemp /tmp/prob_test.XXXXXX.p0)"
trap 'rm -f "$base"' EXIT
cat > "$base" <<'P0'
capital_of_country(france, paris).
capital_of_country(brazil, brasilia).
fact_confidence(capital_of_country(france, paris), high).
fact_confidence(capital_of_country(brazil, brasilia), medium).
how_confident($Goal, $P) :- prob($Goal, $P).
P0

out1="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"how_confident","args":["capital_of_country(france, paris)",null]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"how_confident","args":["capital_of_country(brazil, brasilia)",null]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"how_confident","args":["capital_of_country(italy, rome)",null]}}}'
} | PARROT0_BASE="$base" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l1() { printf '%s\n' "$out1" | grep -F "\"id\":$1"; }

if l1 2 | grep -q 'high'; then
    ok "KB: prob reads fact_confidence high for france/paris"
else
    no "KB: prob for france/paris wrong: $(l1 2)"
fi
if l1 3 | grep -q 'medium'; then
    ok "KB: prob reads medium for brazil/brasilia"
else
    no "KB: prob for brazil/brasilia wrong: $(l1 3)"
fi

if l1 4 | grep -q '0.5'; then
    ok "default: prob defaults to 0.5 for unknown fact"
else
    no "default: prob for italy/rome should be 0.5: $(l1 4)"
fi

# ---- Path 3: prob via MCP-taught rule ----
base3="$(mktemp /tmp/prob_test3.XXXXXX.p0)"
trap 'rm -f "$base" "$base3"' EXIT
cat > "$base3" <<'P0'
fact_confidence(has_fur(dog), 0.9).
fact_confidence(has_fur(cat), 0.9).
P0

out3="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.assert_clause","arguments":{"head":{"pred":"confidence_of","args":["$X","$P"]},"body":[{"pred":"prob","args":["has_fur($X)","$P"]}]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"confidence_of","args":["dog",null]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"confidence_of","args":["snake",null]}}}'
} | PARROT0_BASE="$base3" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l3() { printf '%s\n' "$out3" | grep -F "\"id\":$1"; }

if l3 2 | grep -q '\\"ok\\":true'; then
    ok "MCP: prob/2 clause accepted"
else
    no "MCP: prob clause rejected: $(l3 2)"
fi
if l3 3 | grep -q '0.9'; then
    ok "MCP: confidence_of(dog) reads 0.9 from KB"
else
    no "MCP: confidence_of(dog) wrong: $(l3 3)"
fi
if l3 4 | grep -q '0.5'; then
    ok "MCP: confidence_of(snake) defaults to 0.5"
else
    no "MCP: confidence_of(snake) wrong: $(l3 4)"
fi

# ---- Path 4: runtime growth — adding confidence changes prob result ----
base4="$(mktemp /tmp/prob_test4.XXXXXX.p0)"
trap 'rm -f "$base" "$base3" "$base4"' EXIT
cat > "$base4" <<'P0'
color(sky, blue).
get_prob($X, $Y, $P) :- prob(color($X, $Y), $P).
P0

out4a="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"get_prob","args":["sky","blue",null]}}}'
} | PARROT0_BASE="$base4" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l4a() { printf '%s\n' "$out4a" | grep -F "\"id\":$1"; }

if l4a 2 | grep -q '0.5'; then
    ok "growth: prob defaults to 0.5 before fact_confidence exists"
else
    no "growth: prob default wrong: $(l4a 2)"
fi

# Now add confidence via a new session
out4b="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"fact_confidence","args":["color(sky, blue)","high"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"get_prob","args":["sky","blue",null]}}}'
} | PARROT0_BASE="$base4" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l4b() { printf '%s\n' "$out4b" | grep -F "\"id\":$1"; }

if l4b 2 | grep -q '\\"ok\\":true'; then
    ok "growth: new confidence fact asserted"
else
    no "growth: assert failed: $(l4b 2)"
fi
if l4b 3 | grep -q 'high'; then
    ok "growth: prob NOW reads high — KB growth, zero C change"
else
    no "growth: prob should read high: $(l4b 3)"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
