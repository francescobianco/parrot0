#!/usr/bin/env bash
#
# gen335+ — assert and retract from rule bodies.
# Rules can create and remove facts as part of inference — dynamic clause
# generation without a C edit. Non-backtrackable (like Prolog).
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "assertretract: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS assertretract: $1"; pass=$((pass+1)); }
no() { echo "FAIL assertretract: $1" >&2; fail=$((fail+1)); }

# ---- Path 1: assert from a rule body ----
base="$(mktemp /tmp/assert_test.XXXXXX.p0)"
trap 'rm -f "$base"' EXIT
cat > "$base" <<'P0'
has_hair(dog).
derive_warm_blooded($X) :- has_hair($X), assert(warm_blooded, $X).
P0

out1="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"warm_blooded","args":["dog"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"derive_warm_blooded","args":["dog"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"warm_blooded","args":["dog"]}}}'
} | PARROT0_BASE="$base" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l1() { printf '%s\n' "$out1" | grep -F "\"id\":$1"; }

if l1 2 | grep -q 'provable\\":false'; then
    ok "assert: warm_blooded(dog) unknown before rule fires"
else
    no "assert: warm_blooded(dog) should be unknown: $(l1 2)"
fi
if l1 3 | grep -q 'provable\\":true'; then
    ok "assert: derive_warm_blooded(dog) true"
else
    no "assert: derive_warm_blooded(dog) not provable: $(l1 3)"
fi
if l1 4 | grep -q 'provable\\":true'; then
    ok "assert: warm_blooded(dog) now known — asserted by rule"
else
    no "assert: warm_blooded(dog) not asserted: $(l1 4)"
fi

# ---- Path 2: retract from a rule body ----
base2="$(mktemp /tmp/retract_test.XXXXXX.p0)"
trap 'rm -f "$base" "$base2"' EXIT
cat > "$base2" <<'P0'
is_active(alice).
cleanup($X) :- is_active($X), retract(is_active, $X).
P0

out2="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"is_active","args":["alice"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"cleanup","args":["alice"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"is_active","args":["alice"]}}}'
} | PARROT0_BASE="$base2" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l2() { printf '%s\n' "$out2" | grep -F "\"id\":$1"; }

if l2 2 | grep -q 'provable\\":true'; then
    ok "retract: is_active(alice) true before cleanup"
else
    no "retract: is_active(alice) should be true: $(l2 2)"
fi
if l2 3 | grep -q 'provable\\":true'; then
    ok "retract: cleanup(alice) fires"
else
    no "retract: cleanup(alice) not provable: $(l2 3)"
fi
if l2 4 | grep -q 'provable\\":false'; then
    ok "retract: is_active(alice) gone after cleanup"
else
    no "retract: is_active(alice) should be gone: $(l2 4)"
fi

# ---- Path 3: multi-arg assert/retract ----
base3="$(mktemp /tmp/assert3_test.XXXXXX.p0)"
trap 'rm -f "$base" "$base2" "$base3"' EXIT
cat > "$base3" <<'P0'
edge(a, b).
materialize($X, $Y) :- edge($X, $Y), assert(reaches, $X, $Y).
P0

out3="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"reaches","args":["a","b"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"materialize","args":["a","b"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"reaches","args":["a","b"]}}}'
} | PARROT0_BASE="$base3" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l3() { printf '%s\n' "$out3" | grep -F "\"id\":$1"; }

if l3 2 | grep -q 'provable\\":false'; then
    ok "multi: reaches(a,b) unknown before materialize"
else
    no "multi: reaches(a,b) should be unknown: $(l3 2)"
fi
if l3 3 | grep -q 'provable\\":true'; then
    ok "multi: materialize(a,b) fires"
else
    no "multi: materialize(a,b) failed: $(l3 3)"
fi
if l3 4 | grep -q 'provable\\":true'; then
    ok "multi: reaches(a,b) asserted as 2-ary fact from rule body"
else
    no "multi: reaches(a,b) not asserted: $(l3 4)"
fi

# ---- Path 4: assert taught via MCP ----
base4="$(mktemp /tmp/assert4_test.XXXXXX.p0)"
trap 'rm -f "$base" "$base2" "$base3" "$base4"' EXIT
cat > "$base4" <<'P0'
color(sky, blue).
P0

out4="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.assert_clause","arguments":{"head":{"pred":"cache_color","args":["$X","$Y"]},"body":[{"pred":"color","args":["$X","$Y"]},{"pred":"assert","args":["known_color","$X","$Y"]}]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"cache_color","args":["sky","blue"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"known_color","args":["sky","blue"]}}}'
} | PARROT0_BASE="$base4" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l4() { printf '%s\n' "$out4" | grep -F "\"id\":$1"; }

if l4 2 | grep -q '\\"ok\\":true'; then
    ok "MCP: kb.assert_clause with assert body accepted"
else
    no "MCP: assert body clause rejected: $(l4 2)"
fi
if l4 3 | grep -q 'provable\\":true'; then
    ok "MCP: cache_color(sky,blue) true"
else
    no "MCP: cache_color(sky,blue) failed: $(l4 3)"
fi
if l4 4 | grep -q 'provable\\":true'; then
    ok "MCP: known_color(sky,blue) asserted by rule body via MCP"
else
    no "MCP: known_color(sky,blue) not asserted: $(l4 4)"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
