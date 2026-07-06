#!/usr/bin/env bash
#
# gen276 — the /restore chat command: forget the UNSAVED session and reload every
# KB file from disk IN PLACE, so knowledge written to a .p0 file (by hand or by an
# MCP-engine agent) goes live WITHOUT restarting parrot0. This is the keystone of
# docs/plans/mcp-engine.md: add knowledge -> /restore -> query it, same process.
#
# Three things are proven:
#   A. an unsaved session fact is DROPPED by /restore (nothing leaks across it);
#   B. a fact written to a KB file mid-session is PICKED UP after /restore;
#   C. a RULE written mid-session makes parrot0 DERIVE a new conclusion after
#      /restore — behaviour evolves by inference, not just fact lookup (kb-first).
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "restore: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS restore: $1"; pass=$((pass+1)); }
no() { echo "FAIL restore: $1" >&2; fail=$((fail+1)); }

# --- A. simple pipe: /restore forgets an unsaved session fact ---------------
# Teach a novel fact (never /save'd), confirm it, /restore, confirm it is gone.
out="$(printf 'zorp is a gizmo\nis zorp a gizmo?\n/restore\nis zorp a gizmo?\n/quit\n' \
      | PARROT0_BASE= PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" 2>/dev/null)"
before="$(printf '%s\n' "$out" | sed -n '2p')"    # answer before /restore
after="$(printf '%s\n' "$out" | sed -n '3p')"     # answer after  /restore
if [ "$before" = "Yes." ] && [ "$after" != "Yes." ]; then
    ok "unsaved session fact is dropped by /restore"
else
    no "expected known-before / unknown-after; got before=[$before] after=[$after]"
fi

# --- coprocess harness for the mid-session file-edit tests (B, C) -----------
# parrot0 flushes stdout per turn and sends prompts/messages to stderr (discarded),
# so stdout is exactly one line per substantive response. /restore and /quit emit
# nothing on stdout, so we only read after a real query.
run_reload_case() {
    # $1 = label, $2 = knowledge appended to the base file, $3 = the query,
    # $4 = expected answer AFTER the write + /restore
    local label="$1" knowledge="$2" query="$3" want="$4"
    local tmp base p_in p_out reply_before reply_after
    tmp="$(mktemp -d)"; base="$tmp/base.p0"; : > "$base"

    coproc P { PARROT0_BASE="$base" PARROT0_SESSION= PARROT0_WORLD_FACTS=0 \
               PARROT0_PROFILE= "$BIN" 2>/dev/null; }
    p_in="${P[1]}"; p_out="${P[0]}"

    printf '%s\n' "$query" >&"$p_in"
    IFS= read -r -t 10 reply_before <&"$p_out" || reply_before="<timeout>"

    # an external agent (the MCP write primitive, simulated) adds knowledge:
    printf '%s\n' "$knowledge" >> "$base"

    printf '/restore\n' >&"$p_in"      # no stdout — message goes to stderr
    printf '%s\n' "$query" >&"$p_in"
    IFS= read -r -t 10 reply_after <&"$p_out" || reply_after="<timeout>"

    printf '/quit\n' >&"$p_in"
    wait "$P_PID" 2>/dev/null
    rm -rf "$tmp"

    if [ "$reply_before" != "Yes." ] && [ "$reply_after" = "$want" ]; then
        ok "$label"
    else
        no "$label: before=[$reply_before] after=[$reply_after] want=[$want]"
    fi
}

# B. a FACT written to disk mid-session is live after /restore (no restart).
run_reload_case "fact written to disk goes live after /restore" \
    'widget(zorp).' 'is zorp a widget?' 'Yes.'

# C. a RULE written to disk mid-session makes parrot0 DERIVE a new conclusion —
#    behaviour evolves by resolution, the kb-first promise.
run_reload_case "rule written to disk makes a new conclusion derivable" \
    'man(diogenes).
philosopher(X) :- man(X).' 'is diogenes a philosopher?' 'Yes.'

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]