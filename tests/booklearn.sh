#!/usr/bin/env bash
# gen67: reading a new held-out passage must shift the induced language model.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"

if [ ! -x "$BIN" ]; then
    echo "booklearn: binary not built ($BIN)" >&2
    exit 1
fi

out="$(
    {
        printf "%s
" "read: the otter swims downstream. the otter swims often."
        printf "%s
" "say the"
        printf "%s
" "read: the robot walks slowly. the robot walks daily."
        printf "%s
" "say the"
    } | PARROT0_BASE= PARROT0_SESSION= "$BIN" 2>/dev/null
)"

expected="$(printf "%s
%s
%s
%s" \
    "Learned 0 fact(s), skipped 2." \
    "the otter swims downstream" \
    "Learned 0 fact(s), skipped 2." \
    "the robot walks slowly")"

if [ "$out" = "$expected" ]; then
    echo "PASS booklearn: held-out read shifts generation"
    exit 0
fi

echo "FAIL booklearn: held-out read did not shift generation" >&2
echo "expected:" >&2
printf "%s
" "$expected" >&2
echo "actual:" >&2
printf "%s
" "$out" >&2
exit 1
