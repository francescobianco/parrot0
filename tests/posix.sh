#!/usr/bin/env bash
#
# Mission M1, step 1 (gen53): POSIX/shell knowledge. Loads knowledge/bash.pl and
# proves COMPOSITIONAL understanding of a command line — base effect + each flag,
# composed even for combinations not stored — plus case-sensitivity (-r vs -R),
# honest unknown flags/commands, and that it does not hijack non-shell questions.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
BASH_KB="$ROOT/knowledge/bash.pl"

if [ ! -x "$BIN" ]; then
    echo "posix: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
expect() { # <description> <input> <expected first stdout line>
    local got
    got="$(printf '%s\n/quit\n' "$2" \
          | PARROT0_BASE="$BASH_KB" PARROT0_SESSION= "$BIN" 2>/dev/null | head -1)"
    if [ "$got" = "$3" ]; then echo "PASS posix: $1"; pass=$((pass+1))
    else echo "FAIL posix: $1 — want [$3] got [$got]" >&2; fail=$((fail+1)); fi
}

# base command effect
expect "single command" "what does ls do?" \
    "ls lists directory contents."
# HELD-OUT composition: "ls -la" is never stored; composed from ls + l + a
expect "composed flags (held-out)" "what does ls -la do?" \
    "ls lists directory contents, in long format, including hidden entries."
# case sensitivity: -r and -R are different flags (the shell is case-sensitive)
expect "lowercase flag -r" "what does ls -r do?" \
    "ls lists directory contents, in reverse order."
expect "uppercase flag -R" "what does ls -R do?" \
    "ls lists directory contents, descending into subdirectories."
# another command, another composition
expect "grep with flag" "what does grep -i do?" \
    "grep prints lines matching a pattern, ignoring case."
expect "explain form, two flags" "explain rm -rf" \
    "rm removes files, recursively, without prompting."
# honest about an unknown flag, while still composing the known ones
expect "unknown flag is admitted" "what does ls -lq do?" \
    "ls lists directory contents, in long format. I don't know the option -q."
# unknown command, but clearly shell syntax (a flag present) -> honest
expect "unknown command (shell syntax)" "what does foobar -x do?" \
    "I don't know the command foobar."
# a non-shell question is NOT hijacked (no flag, first token not a command)
expect "does not hijack natural question" "what does a bird do?" \
    "I don't understand that yet."
# gen61: pipelines are composed from per-command descriptions (held-out combos)
expect "pipeline composition" "what does cat file | grep x do?" \
    "cat prints file contents, then grep prints lines matching a pattern."
expect "explain pipeline with flags" "explain ls -l | sort" \
    "ls lists directory contents, in long format, then sort sorts lines of text."

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
