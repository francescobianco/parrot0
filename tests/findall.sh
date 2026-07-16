#!/usr/bin/env bash
#
# gen335+ — findall/3 solution collection.
# findall(Template, Goal, List) collects all solutions to Goal into a cons-list.
# This is the missing piece for full predicate completion and aggregation.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "findall: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS findall: $1"; pass=$((pass+1)); }
no() { echo "FAIL findall: $1" >&2; fail=$((fail+1)); }

# ---- Path 1: basic findall — collect all mammals ----
base="$(mktemp /tmp/findall_test.XXXXXX.p0)"
trap 'rm -f "$base"' EXIT
cat > "$base" <<'P0'
is_a(dog, mammal).
is_a(cat, mammal).
is_a(sparrow, bird).
all_mammals($L) :- findall($X, is_a($X, mammal), $L).
P0

out1="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"all_mammals","args":[null]}}}'
} | PARROT0_BASE="$base" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l1() { printf '%s\n' "$out1" | grep -F "\"id\":$1"; }

# The list should contain dog and cat (order depends on fact order in KB)
if l1 2 | grep -q 'dog' && l1 2 | grep -q 'cat' && ! l1 2 | grep -q 'sparrow'; then
    ok "basic: all_mammals collects dog and cat, not sparrow"
else
    no "basic: all_mammals result wrong: $(l1 2)"
fi

# ---- Path 2: findall with no solutions — empty list ----
base2="$(mktemp /tmp/findall_test2.XXXXXX.p0)"
trap 'rm -f "$base" "$base2"' EXIT
cat > "$base2" <<'P0'
is_a(dog, mammal).
no_birds($L) :- findall($X, is_a($X, bird), $L).
P0

out2="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"no_birds","args":[null]}}}'
} | PARROT0_BASE="$base2" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l2() { printf '%s\n' "$out2" | grep -F "\"id\":$1"; }

if l2 2 | grep -q 'nil'; then
    ok "empty: no_birds collects nil (empty list)"
else
    no "empty: no_birds should be nil: $(l2 2)"
fi

# ---- Path 3: findall with rule expansion ----
base3="$(mktemp /tmp/findall_test3.XXXXXX.p0)"
trap 'rm -f "$base" "$base2" "$base3"' EXIT
cat > "$base3" <<'P0'
parent(ann, bob).
parent(bob, carl).
ancestor($X, $Y) :- parent($X, $Y).
ancestor($X, $Z) :- parent($X, $Y), ancestor($Y, $Z).
all_ancestors($X, $L) :- findall($Y, ancestor($X, $Y), $L).
P0

out3="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"all_ancestors","args":["ann",null]}}}'
} | PARROT0_BASE="$base3" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l3() { printf '%s\n' "$out3" | grep -F "\"id\":$1"; }

if l3 2 | grep -q 'bob' && l3 2 | grep -q 'carl'; then
    ok "recursive: all_ancestors(ann) collects bob and carl"
else
    no "recursive: all_ancestors(ann) wrong: $(l3 2)"
fi

# ---- Path 4: findall taught via MCP ----
base4="$(mktemp /tmp/findall_test4.XXXXXX.p0)"
trap 'rm -f "$base" "$base2" "$base3" "$base4"' EXIT
cat > "$base4" <<'P0'
color(apple, red).
color(banana, yellow).
color(cherry, red).
P0

out4="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.assert_clause","arguments":{"head":{"pred":"red_fruits","args":["$L"]},"body":[{"pred":"findall","args":["$X","color($X, red)","$L"]}]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"red_fruits","args":[null]}}}'
} | PARROT0_BASE="$base4" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l4() { printf '%s\n' "$out4" | grep -F "\"id\":$1"; }

if l4 2 | grep -q '\\"ok\\":true'; then
    ok "MCP: kb.assert_clause with findall body accepted"
else
    no "MCP: findall clause rejected: $(l4 2)"
fi
if l4 3 | grep -q 'apple' && l4 3 | grep -q 'cherry'; then
    ok "MCP: red_fruits collects apple and cherry (both red)"
else
    no "MCP: red_fruits result wrong: $(l4 3)"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
