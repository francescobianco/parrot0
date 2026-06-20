#!/usr/bin/env bash
#
# gen127 (L12): program synthesis — the INVERSE of mod_shell. Loads the SAME
# kb/bash.p0 the interpreter reads and proves we can go from a natural
# spec to a one-line command: the action picks the command, the object noun
# picks the flag. Held-out specs over known commands; honest decline otherwise.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
BASH_KB="$ROOT/kb/bash.p0"

if [ ! -x "$BIN" ]; then
    echo "synth: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
expect() { # <description> <input> <expected first stdout line>
    local got
    got="$(printf '%s\n/quit\n' "$2" \
          | PARROT0_BASE="$BASH_KB" PARROT0_SESSION= "$BIN" 2>/dev/null | head -1)"
    if [ "$got" = "$3" ]; then echo "PASS synth: $1"; pass=$((pass+1))
    else echo "FAIL synth: $1 — want [$3] got [$got]" >&2; fail=$((fail+1)); fi
}

# The documented L12 example: action "count" picks wc, object "lines" picks -l.
expect "count lines" "what command counts the lines in a file?" \
    "wc -l <file>"
# Held-out flag through the SAME path: the object noun flips -l to -w. wc's
# description literally contains lines/words/bytes, so this proves the verb is
# dropped before flag selection and the surviving object noun disambiguates.
expect "count words" "write a command to count the words in a file" \
    "wc -w <file>"
# A different command + flag, different action.
expect "remove recursively" "what command removes files recursively" \
    "rm -r <file>"
expect "sort numerically" "which command sorts lines numerically" \
    "sort -n"
# grep: object "pattern" emits a <pattern> operand, recursively -> -r.
expect "grep recursively" "give me a command to search for a pattern recursively" \
    "grep -r <pattern>"
# A bare command with no flag.
expect "print working dir" "what command prints the working directory" \
    "pwd"
# Honest decline when no known command matches the spec.
expect "unknown spec" "write a command to fly to the moon" \
    "I don't know a command for that yet."

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
