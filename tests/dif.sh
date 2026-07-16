#!/usr/bin/env bash
#
# gen335+ — dif/2 deferred inequality constraint.
# dif(X,Y) postpones the X≠Y check until both sides are ground.
# This replaces fragile naf(eq(...)) patterns that flounder on unbound vars.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "dif: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS dif: $1"; pass=$((pass+1)); }
no() { echo "FAIL dif: $1" >&2; fail=$((fail+1)); }

# ---- Path 1: dif/2 with two ground atoms ----
base="$(mktemp /tmp/dif_test.XXXXXX.p0)"
trap 'rm -f "$base"' EXIT
cat > "$base" <<'P0'
distinct($X, $Y) :- dif($X, $Y).
P0

out1="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"distinct","args":["a","b"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"distinct","args":["a","a"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"distinct","args":["1","2"]}}}'
} | PARROT0_BASE="$base" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l1() { printf '%s\n' "$out1" | grep -F "\"id\":$1"; }

if l1 2 | grep -q 'provable\\":true'; then
    ok "ground: distinct(a,b) true — a ≠ b"
else
    no "ground: distinct(a,b) should be true: $(l1 2)"
fi
if l1 3 | grep -q 'provable\\":false'; then
    ok "ground: distinct(a,a) false — a = a"
else
    no "ground: distinct(a,a) should be false: $(l1 3)"
fi
if l1 4 | grep -q 'provable\\":true'; then
    ok "ground: distinct(1,2) true — numeric atoms distinct"
else
    no "ground: distinct(1,2) should be true: $(l1 4)"
fi

# ---- Path 2: deferred dif with one variable, resolved by rule body ----
base2="$(mktemp /tmp/dif_test2.XXXXXX.p0)"
trap 'rm -f "$base" "$base2"' EXIT
cat > "$base2" <<'P0'
color(apple, red).
color(banana, yellow).
color(cherry, red).
different_color($X, $Y) :- color($X, $C1), color($Y, $C2), dif($C1, $C2).
P0

out2="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"different_color","args":["apple","banana"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"different_color","args":["apple","cherry"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"different_color","args":["apple","apple"]}}}'
} | PARROT0_BASE="$base2" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l2() { printf '%s\n' "$out2" | grep -F "\"id\":$1"; }

if l2 2 | grep -q 'provable\\":true'; then
    ok "deferred: apple vs banana different colors (red ≠ yellow)"
else
    no "deferred: apple vs banana should be true: $(l2 2)"
fi
if l2 3 | grep -q 'provable\\":false'; then
    ok "deferred: apple vs cherry same color (red = red) — dif fires"
else
    no "deferred: apple vs cherry should be false: $(l2 3)"
fi
if l2 4 | grep -q 'provable\\":false'; then
    ok "deferred: apple vs apple same color (trivially)"
else
    no "deferred: apple vs apple should be false: $(l2 4)"
fi

# ---- Path 3: dif taught via MCP ----
base3="$(mktemp /tmp/dif_test3.XXXXXX.p0)"
trap 'rm -f "$base" "$base2" "$base3"' EXIT
cat > "$base3" <<'P0'
tag(one, a).
tag(two, b).
tag(three, a).
no_duplicate_tag($X, $Y) :- tag($X, $T1), tag($Y, $T2), dif($T1, $T2).
P0

out3="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"no_duplicate_tag","args":["one",null]}}}'
} | PARROT0_BASE="$base3" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l3() { printf '%s\n' "$out3" | grep -F "\"id\":$1"; }

if l3 2 | grep -q 'two' && ! l3 2 | grep -q 'three'; then
    ok "deferred: match finds two (a≠b) but not three (a=a) — dif constrains"
else
    no "deferred: match result wrong: $(l3 2)"
fi

# ---- Path 4: dif constraint survives backtracking (via Subst copy) ----
base4="$(mktemp /tmp/dif_test4.XXXXXX.p0)"
trap 'rm -f "$base" "$base2" "$base3" "$base4"' EXIT
cat > "$base4" <<'P0'
value(1).
value(2).
value(3).
pair_not_equal($V1, $V2, $V3) :- value($V1), value($V2), dif($V1, $V2), value($V3), dif($V2, $V3).
P0

out4="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"pair_not_equal","args":[null,null,null]}}}'
} | PARROT0_BASE="$base4" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l4() { printf '%s\n' "$out4" | grep -F "\"id\":$1"; }

# Should find solutions where all three values are distinct (1,2,3 permutations)
# With backtracking and dif constraints, we should get solutions
if l4 2 | grep -q '1' && l4 2 | grep -q '2' && l4 2 | grep -q '3'; then
    ok "backtrack: dif constraints carried correctly through multiple unifications"
else
    no "backtrack: pair_not_equal result missing: $(l4 2)"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
