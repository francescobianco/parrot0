#!/usr/bin/env bash
#
# gen277 — the MCP engine TRANSPORT smoke test. This proves only that
# `parrot0 --mcp-engine` speaks MCP JSON-RPC: initialize returns serverInfo, and
# tools/list advertises the toolbox. It deliberately does NOT drive a
# train-via-MCP scenario — those are live experiments run by hand against the
# engine (see docs/plans/mcp-engine.md), not a canned test.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "mcp: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS mcp: $1"; pass=$((pass+1)); }
no() { echo "FAIL mcp: $1" >&2; fail=$((fail+1)); }

out="$(printf '%s\n' \
  '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}' \
  '{"jsonrpc":"2.0","method":"notifications/initialized"}' \
  '{"jsonrpc":"2.0","id":2,"method":"tools/list"}' \
  '{"jsonrpc":"2.0","id":3,"method":"ping"}' \
  | PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

line_init="$(printf '%s\n' "$out" | sed -n '1p')"
line_tools="$(printf '%s\n' "$out" | sed -n '2p')"
line_ping="$(printf '%s\n' "$out" | sed -n '3p')"

# 1) initialize echoes the protocol version and names the server.
if printf '%s' "$line_init" | grep -q '"id":1' \
   && printf '%s' "$line_init" | grep -q '"serverInfo":{"name":"parrot0"' \
   && printf '%s' "$line_init" | grep -q '"protocolVersion":"2024-11-05"'; then
    ok "initialize returns serverInfo + protocolVersion"
else
    no "initialize malformed: $line_init"
fi

# 2) tools/list advertises the engine's tools (spot-check a few by name).
if printf '%s' "$line_tools" | grep -q '"id":2' \
   && printf '%s' "$line_tools" | grep -q '"name":"kb.assert"' \
   && printf '%s' "$line_tools" | grep -q '"name":"kb.query"' \
   && printf '%s' "$line_tools" | grep -q '"name":"kb.restore"' \
   && printf '%s' "$line_tools" | grep -q '"name":"gen.respond"'; then
    ok "tools/list advertises the toolbox"
else
    no "tools/list malformed: $line_tools"
fi

# 3) ping is answered (liveness).
if printf '%s' "$line_ping" | grep -q '"id":3' \
   && printf '%s' "$line_ping" | grep -q '"result":{}'; then
    ok "ping answered"
else
    no "ping malformed: $line_ping"
fi

# 4) an unknown method gets a JSON-RPC 'method not found' error, not a crash.
err="$(printf '%s\n' '{"jsonrpc":"2.0","id":9,"method":"no.such.method"}' \
      | PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"
if printf '%s' "$err" | grep -q '"code":-32601'; then
    ok "unknown method returns -32601"
else
    no "expected -32601 for unknown method, got: $err"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]