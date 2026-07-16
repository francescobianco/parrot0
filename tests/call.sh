#!/usr/bin/env bash
#
# gen335+ — call/1 meta-call primitive.
# call/1 executes a single goal dynamically: call(p($X)) proves p($X).
# It enables meta-interpreters, strategy dispatch, and constraint propagation
# without new C.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "call: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS call: $1"; pass=$((pass+1)); }
no() { echo "FAIL call: $1" >&2; fail=$((fail+1)); }

# ---- Path 1: call/1 from a .p0 file, direct KB query ----
base="$(mktemp /tmp/call_test.XXXXXX.p0)"
trap 'rm -f "$base"' EXIT
cat > "$base" <<'P0'
is_a(dog, mammal).
is_a(cat, mammal).
is_a(sparrow, bird).
direct($X) :- call(is_a($X, mammal)).
P0

out1="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"direct","args":["dog"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"direct","args":["sparrow"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"direct","args":["rock"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"direct","args":[null]}}}'
} | PARROT0_BASE="$base" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l1() { printf '%s\n' "$out1" | grep -F "\"id\":$1"; }

if l1 2 | grep -q 'provable\\":true'; then
    ok ".p0: direct(dog) via call(is_a(dog,mammal))"
else
    no ".p0: direct(dog) not provable: $(l1 2)"
fi
if l1 3 | grep -q 'provable\\":false'; then
    ok ".p0: direct(sparrow) not mammal"
else
    no ".p0: direct(sparrow) should be false: $(l1 3)"
fi
if l1 4 | grep -q 'provable\\":false'; then
    ok ".p0: direct(rock) unknown"
else
    no ".p0: direct(rock) should be false: $(l1 4)"
fi
if l1 5 | grep -q 'dog' && l1 5 | grep -q 'cat'; then
    ok ".p0: kb.match collect mammals via call"
else
    no ".p0: kb.match direct: $(l1 5)"
fi

# ---- Path 2: call/1 taught over MCP ----
base2="$(mktemp /tmp/call_test2.XXXXXX.p0)"
trap 'rm -f "$base" "$base2"' EXIT
cat > "$base2" <<'P0'
can_fly(sparrow).
can_fly(eagle).
P0

out2="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.assert_clause","arguments":{"head":{"pred":"flies","args":["$X"]},"body":[{"pred":"call","args":["can_fly($X)"]}]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"flies","args":["sparrow"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"flies","args":["penguin"]}}}'
} | PARROT0_BASE="$base2" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l2() { printf '%s\n' "$out2" | grep -F "\"id\":$1"; }

if l2 2 | grep -q '\\"ok\\":true'; then
    ok "MCP: kb.assert_clause with call/1 body"
else
    no "MCP: call/1 clause rejected: $(l2 2)"
fi
if l2 3 | grep -q 'provable\\":true'; then
    ok "MCP: flies(sparrow) via call(can_fly(sparrow))"
else
    no "MCP: flies(sparrow) not provable: $(l2 3)"
fi
if l2 4 | grep -q 'provable\\":false'; then
    ok "MCP: flies(penguin) false"
else
    no "MCP: flies(penguin) wrong: $(l2 4)"
fi

# ---- Path 3: runtime-growth test (KB-first compliance) ----
base3="$(mktemp /tmp/call_test3.XXXXXX.p0)"
trap 'rm -f "$base" "$base2" "$base3"' EXIT
cat > "$base3" <<'P0'
has_feature(dog, fur).
has_feature(snake, scales).
indirect($X) :- call(has_feature($X, fur)).
P0

# Before adding the fact, dog should resolve, snake should not
out3a="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"indirect","args":["dog"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"indirect","args":["snake"]}}}'
} | PARROT0_BASE="$base3" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l3a() { printf '%s\n' "$out3a" | grep -F "\"id\":$1"; }

if l3a 2 | grep -q 'provable\\":true'; then
    ok "growth: indirect(dog) true (fur fact present)"
else
    no "growth: indirect(dog) should be true: $(l3a 2)"
fi
if l3a 3 | grep -q 'provable\\":false'; then
    ok "growth: indirect(snake) false (scales != fur)"
else
    no "growth: indirect(snake) should be false: $(l3a 3)"
fi

# Add has_feature(snake, fur) via another MCP connection
out3b="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"has_feature","args":["snake","fur"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"indirect","args":["snake"]}}}'
} | PARROT0_BASE="$base3" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l3b() { printf '%s\n' "$out3b" | grep -F "\"id\":$1"; }

if l3b 2 | grep -q '\\"ok\\":true'; then
    ok "growth: new fur fact asserted"
else
    no "growth: assert failed: $(l3b 2)"
fi
if l3b 3 | grep -q 'provable\\":true'; then
    ok "growth: indirect(snake) NOW true — runtime growth via call/1"
else
    no "growth: indirect(snake) should be true after growth: $(l3b 3)"
fi

# Retract and verify ablation
out3c="$( {
  printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  printf '%s\n' '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"has_feature","args":["snake","fur"]}}}'
  printf '%s\n' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"indirect","args":["snake"]}}}'
} | PARROT0_BASE="$base3" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l3c() { printf '%s\n' "$out3c" | grep -F "\"id\":$1"; }

if l3c 3 | grep -q 'provable\\":false'; then
    ok "growth: indirect(snake) false after retraction — runtime ablation"
else
    no "growth: indirect(snake) should be false after ablation: $(l3c 3)"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
