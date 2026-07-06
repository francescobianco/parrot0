#!/usr/bin/env bash
#
# mcp-live.sh — drive a PERSISTENT parrot0 --mcp-engine for hands-on training
# experiments (gen277). The engine stays up across separate invocations of this
# script, so knowledge asserted by one call is visible to the next — the live
# "add knowledge → query it" loop, without restarting parrot0.
#
# Transport design (robust): requests go in on a FIFO held open by a background
# "holder" (so the engine's stdin never EOFs between calls); responses come out
# to a regular FILE the engine appends to (a regular file never blocks the
# writer, unlike a response FIFO with no reader). Each `call` remembers the
# file's line count, sends its request, then polls for the new reply line.
#
# Usage:
#   scripts/mcp-live.sh start [ENV=VAL ...]        # boot the engine (sends initialize)
#   scripts/mcp-live.sh call <tool> '<json-args>'  # one tools/call, prints the result payload
#   scripts/mcp-live.sh raw  '<json-rpc line>'     # arbitrary request, prints the raw reply
#   scripts/mcp-live.sh tools                      # list tool names
#   scripts/mcp-live.sh stop                       # shut the engine down
#
# State dir (fifo + response log + pids) is $PARROT0_MCP_DIR (default /tmp/parrot0-mcp).
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
DIR="${PARROT0_MCP_DIR:-/tmp/parrot0-mcp}"
REQ="$DIR/req"; RES="$DIR/res"; LOG="$DIR/log"
SRV_PID="$DIR/server.pid"; HOLD_PID="$DIR/holder.pid"

# Send one request line, wait for the next response line to appear in RES, print it.
rpc() {
    local before after
    before="$(wc -l < "$RES" 2>/dev/null || echo 0)"
    printf '%s\n' "$1" > "$REQ"
    for _ in $(seq 1 300); do        # up to ~15s
        after="$(wc -l < "$RES" 2>/dev/null || echo 0)"
        [ "$after" -gt "$before" ] && { sed -n "$((before+1))p" "$RES"; return 0; }
        sleep 0.05
    done
    echo "<no response within timeout>"; return 1
}
# notifications carry no id and get no reply — send without waiting.
notify() { printf '%s\n' "$1" > "$REQ"; }

case "${1:-}" in
start)
    shift
    [ -x "$BIN" ] || { echo "mcp-live: build parrot0 first (make)"; exit 1; }
    mkdir -p "$DIR"; rm -f "$REQ" "$RES" "$LOG"; mkfifo "$REQ"; : > "$RES"
    env_pairs=("PARROT0_SESSION=" "PARROT0_PROFILE=")
    [ "$#" -gt 0 ] && env_pairs+=("$@")
    # engine: stdin <- REQ (fifo), stdout -> RES (regular file, appended), stderr -> LOG
    ( cd "$ROOT" && env "${env_pairs[@]}" "$BIN" --mcp-engine < "$REQ" >> "$RES" 2>> "$LOG" ) &
    echo $! > "$SRV_PID"
    # a writer that never writes keeps REQ's write end open across our calls.
    ( exec sleep 100000 ) > "$REQ" &
    echo $! > "$HOLD_PID"
    rpc '{"jsonrpc":"2.0","id":0,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}' >/dev/null
    notify '{"jsonrpc":"2.0","method":"notifications/initialized"}'
    echo "mcp-live: engine up (pid $(cat "$SRV_PID"))"
    ;;
call)
    tool="${2:?tool name}"; args="${3:-{}}"
    line="$(rpc "$(printf '{"jsonrpc":"2.0","id":1,"method":"tools/call","params":{"name":"%s","arguments":%s}}' "$tool" "$args")")"
    # unwrap the MCP content text (the tool's JSON payload) for readability.
    printf '%s\n' "$line" | sed -n 's/.*"text":"\(.*\)"}],"isError.*/\1/p' \
        | sed 's/\\"/"/g;s/\\\\/\\/g'
    ;;
raw)
    rpc "${2:?json}" ;;
tools)
    rpc '{"jsonrpc":"2.0","id":1,"method":"tools/list"}' \
        | grep -o '"name":"[^"]*"' | sed 's/"name":"//;s/"//' ;;
stop)
    [ -f "$HOLD_PID" ] && kill "$(cat "$HOLD_PID")" 2>/dev/null
    [ -f "$SRV_PID" ]  && kill "$(cat "$SRV_PID")"  2>/dev/null
    rm -f "$REQ" "$SRV_PID" "$HOLD_PID"
    echo "mcp-live: engine stopped" ;;
*)
    echo "usage: $0 {start [ENV=VAL...]|call <tool> <json>|raw <json>|tools|stop}"; exit 2 ;;
esac
