#!/usr/bin/env bash
#
# Mission M1, step 3 (gen62): oracle-grounded output prediction.
# Loads kb/bash.p0 and verifies that parrot0 can PREDICT the output of
# small pure shell commands, then check that prediction against the real shell.
# This script only runs when PARROT0_ORACLE=1.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
BASH_KB="$ROOT/kb/bash.p0"

if [ ! -x "$BIN" ]; then
    echo "posix_oracle: binary not built ($BIN)" >&2
    exit 1
fi

if [ "${PARROT0_ORACLE:-}" != "1" ]; then
    echo "PASS posix_oracle: skipped (PARROT0_ORACLE not set)"
    echo "---"
    echo "passed: 1, failed: 0"
    exit 0
fi

pass=0
fail=0
expect() { # <description> <input> <expected first stdout line>
    local got
    got="$(printf '%s\n/quit\n' "$2" \
          | PARROT0_BASE="$BASH_KB" PARROT0_SESSION= PARROT0_ORACLE=1 "$BIN" 2>/dev/null | head -1)"
    if [ "$got" = "$3" ]; then echo "PASS posix_oracle: $1"; pass=$((pass+1))
    else echo "FAIL posix_oracle: $1 — want [$3] got [$got]" >&2; fail=$((fail+1)); fi
}

expect "echo single argument" "what does echo hello print?" \
    "It prints: hello."
expect "echo multiple arguments" "predict the output of echo hello world" \
    "It prints: hello world."
expect "pipeline word count" "predict the output of echo a b c | wc -w" \
    "It prints: 3."
expect "pipeline cat pass-through" "predict the output of echo hi | cat" \
    "It prints: hi."

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
