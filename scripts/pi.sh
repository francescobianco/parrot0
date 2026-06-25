#!/usr/bin/env bash
#
# gen223: `make pi` — one command to talk to parrot0 from the `pi` coding agent.
#   1) free the port (kill any previous parrot0 daemon),
#   2) prepare ~/.pi/agent/models.json (merge, never clobber other providers),
#   3) start the parrot0 daemon in the background (coding tools on by default),
#   4) launch `pi` with the parrot0 provider/model already selected.
# The daemon is stopped automatically when `pi` exits, so the port is clean next time.
#
# Env overrides: PI_PORT (9902), PI_HOST (127.0.0.1), PI_MODELS_JSON.
# Extra args after `make pi ARGS="..."` are forwarded to `pi`.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"                                   # daemon resolves kb/ via relative paths
BIN="$ROOT/bin/parrot0"
PORT="${PI_PORT:-9902}"
HOST="${PI_HOST:-127.0.0.1}"
BASEURL="http://$HOST:$PORT/v1"

[ -x "$BIN" ] || { echo "make pi: binary not built ($BIN) — run 'make build'" >&2; exit 1; }
command -v pi >/dev/null 2>&1 || { echo "make pi: 'pi' not found on PATH (install @earendil-works/pi-coding-agent)" >&2; exit 1; }

# 1) free the port: stop any previous parrot0 daemon
pkill -f "parrot0 --daemon" 2>/dev/null || true
sleep 0.5

# 2) prepare the pi provider config (merges in place)
python3 "$ROOT/scripts/pi_setup.py" "$BASEURL" || exit 1

# 3) start the daemon in the background
nohup "$BIN" --daemon --port "$PORT" --host "$HOST" >/tmp/parrot0-daemon.stderr 2>&1 &
DPID=$!
trap 'kill "$DPID" 2>/dev/null' EXIT         # stop the daemon when pi exits

# 4) wait for it to listen (no curl dependency)
ready=0
for _ in $(seq 1 40); do
    if python3 -c "import urllib.request,sys; urllib.request.urlopen('http://$HOST:$PORT/health', timeout=1).read()" 2>/dev/null; then
        ready=1; break
    fi
    sleep 0.25
done
if [ "$ready" != 1 ]; then
    echo "make pi: daemon did not come up on $BASEURL" >&2
    cat /tmp/parrot0-daemon.stderr >&2
    exit 1
fi
echo "parrot0 daemon up on $BASEURL (pid $DPID) — launching pi…"

# 5) launch pi with parrot0 already selected (not exec'd, so the trap can stop the
#    daemon when pi exits and leave the port clean)
pi --provider parrot0 --model parrot0 --api-key parrot0 "$@"