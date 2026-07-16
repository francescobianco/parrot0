#!/usr/bin/env bash
#
# gen335+ — ranges_over/3 temporal constraint.
# ranges_over(Event, Start, End) validates Start <= End as numbers.
# Allen interval relations are KB Horn rules over the validated ranges.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "ranges: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS ranges: $1"; pass=$((pass+1)); }
no() { echo "FAIL ranges: $1" >&2; fail=$((fail+1)); }

# ---- Path 1: valid range — Start <= End ----
base="$(mktemp /tmp/ranges_test.XXXXXX.p0)"
trap 'rm -f "$base"' EXIT
cat > "$base" <<'P0'
valid_range($E, $S, $E2) :- ranges_over($E, $S, $E2).
P0

out1="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"valid_range","args":["ww2","1939","1945"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"valid_range","args":["bad","1945","1939"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"valid_range","args":["point","2000","2000"]}}}'
} | PARROT0_BASE="$base" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l1() { printf '%s\n' "$out1" | grep -F "\"id\":$1"; }

if l1 2 | grep -q 'provable\\":true'; then
    ok "valid: 1939 <= 1945 true"
else
    no "valid: 1939-1945 should be true: $(l1 2)"
fi
if l1 3 | grep -q 'provable\\":false'; then
    ok "invalid: 1945 > 1939 false"
else
    no "invalid: 1945-1939 should be false: $(l1 3)"
fi
if l1 4 | grep -q 'provable\\":true'; then
    ok "point: 2000 <= 2000 true"
else
    no "point: 2000-2000 should be true: $(l1 4)"
fi

# ---- Path 2: ranges_over with arithmetic expressions ----
base2="$(mktemp /tmp/ranges_test2.XXXXXX.p0)"
trap 'rm -f "$base" "$base2"' EXIT
cat > "$base2" <<'P0'
check_range($E, $S, $D, $E2) :- is($E2, add($S, $D)), ranges_over($E, $S, $E2).
P0

out2="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"check_range","args":["event","10","5","15"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"check_range","args":["event","10","-20","-10"]}}}'
} | PARROT0_BASE="$base2" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l2() { printf '%s\n' "$out2" | grep -F "\"id\":$1"; }

if l2 2 | grep -q 'provable\\":true'; then
    ok "arith: 10+5=15, 10<=15 true"
else
    no "arith: 10+5=15 should be true: $(l2 2)"
fi
if l2 3 | grep -q 'provable\\":false'; then
    ok "arith: 10 > -10 — invalid range, fails correctly"
else
    no "arith: 10 > -10 should fail: $(l2 3)"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
