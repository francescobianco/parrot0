#!/usr/bin/env bash
#
# gen279 (U1, docs/plans/teach-comprehension-via-mcp.md §5.5) — teach-via-MCP
# FIDELITY test. Proves that a literal taught through MCP round-trips faithfully:
# what you assert is what you match/query back, even when it starts uppercase or
# contains spaces/punctuation.
#
# Root cause this guards against: kb.assert passed argument strings raw, and the
# engine's variable convention (is_var: leading uppercase/'_' = variable,
# src/kb.c) then mistook "Madrid" for a VARIABLE — so the fact stored was
# capital(spain, <var>): match returned nothing and a wrong-value query was a
# false positive. U1 makes the MCP boundary apply the .p0 quoting discipline:
# literals are literals.
#
# State persists within ONE engine process (like the interactive session), so a
# single ordered batch exercises assert -> match -> query.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "mcp-teach: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS mcp-teach: $1"; pass=$((pass+1)); }
no() { echo "FAIL mcp-teach: $1" >&2; fail=$((fail+1)); }

call() { printf '%s\n' "$1"; }   # emit one JSON-RPC line

out="$( {
  call '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  call '{"jsonrpc":"2.0","method":"notifications/initialized"}'
  # lowercase control — works today
  call '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"capital","args":["spain","madrid"]}}}'
  call '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"capital","args":["spain",null]}}}'
  # uppercase proper noun — the bug
  call '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"capital","args":["france","Paris"]}}}'
  call '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"capital","args":["france",null]}}}'
  call '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"capital","args":["france","Paris"]}}}'
  call '{"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"capital","args":["france","Berlin"]}}}'
  # response template — uppercase + spaces + comma + {slot}
  call '{"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"response_template","args":["greet","Welcome, {name}!"]}}}'
  call '{"jsonrpc":"2.0","id":9,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"response_template","args":["greet",null]}}}'
} | PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

line() { printf '%s\n' "$out" | grep -F "\"id\":$1"; }

# control: lowercase literal round-trips (regression guard)
if line 3 | grep -q 'madrid'; then
    ok "lowercase literal round-trips (madrid)"
else
    no "lowercase control broke: $(line 3)"
fi

# GATE A: uppercase proper noun round-trips through match
if line 5 | grep -q 'Paris'; then
    ok "uppercase proper noun round-trips (Paris)"
else
    no "match lost the uppercase value: $(line 5)"
fi

# GATE B: exact-value query is provable
if line 6 | grep -q 'provable\\":true'; then
    ok "query with taught value is provable"
else
    no "query of taught value not provable: $(line 6)"
fi

# GATE C: a DIFFERENT value must NOT be provable (no variable-unification false positive)
if line 7 | grep -q 'provable\\":false'; then
    ok "wrong value is correctly not provable (no false positive)"
else
    no "wrong value falsely provable (uppercase treated as variable): $(line 7)"
fi

# GATE D: a display template (spaces + comma + {slot} + uppercase) round-trips
if line 9 | grep -q 'Welcome, {name}!'; then
    ok "response template round-trips verbatim"
else
    no "template lost/garbled: $(line 9)"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
