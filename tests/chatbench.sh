#!/usr/bin/env bash
#
# C0 (gen50): the felt-intelligence conversation benchmark.
#
# Unlike tests/run.sh (exact-match pass/fail unit cases), this MEASURES how
# conversational parrot0 feels. Each tests/chat/*.dlg is a multi-turn session:
#   > user input line   (fed to parrot0's stdin, in order, one session per file)
#   < expected substring (what a *satisfying* answer would contain)
#   # comment / blank    (ignored)
# A turn "lands" if parrot0's reply for that line CONTAINS the expected text
# after normalization (lowercase, strip punctuation, collapse spaces) -- soft on
# purpose, since many phrasings are acceptable in dialogue.
#
# It NEVER fails the build: it prints a felt-intelligence score (turns landed /
# total) to watch climb as the C-series lands. The whole point is to stop
# mistaking bench points for conversational intelligence.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
DLG_DIR="$ROOT/tests/chat"

export PARROT0_BASE= PARROT0_SESSION=

if [ ! -x "$BIN" ]; then
    echo "chatbench: binary not built ($BIN)" >&2
    exit 1
fi

norm() { printf '%s' "$1" | tr '[:upper:]' '[:lower:]' \
    | tr -d '[:punct:]' | tr -s ' ' | sed 's/^ *//;s/ *$//'; }

total=0
landed=0

shopt -s nullglob
for dlg in "$DLG_DIR"/*.dlg; do
    name="$(basename "$dlg")"
    inputs=(); expected=()
    while IFS= read -r raw || [ -n "$raw" ]; do
        case "$raw" in
            '>'*) inputs+=("${raw#> }") ;;
            '<'*) expected+=("${raw#< }") ;;
            *)    : ;;
        esac
    done < "$dlg"

    in_lines=""
    for line in "${inputs[@]}"; do in_lines+="$line"$'\n'; done
    actual="$(printf '%s' "$in_lines" | "$BIN" 2>/dev/null)"

    mapfile -t outs <<< "$actual"
    echo "# $name"
    i=0
    for want in "${expected[@]}"; do
        got="${outs[$i]-}"
        nw="$(norm "$want")"; ng="$(norm "$got")"
        total=$((total+1))
        if [ -n "$nw" ] && printf '%s' "$ng" | grep -qF -- "$nw"; then
            landed=$((landed+1))
            printf '  OK   [%s] <- "%s"\n' "$want" "$got"
        else
            printf '  MISS [%s] <- "%s"\n' "$want" "$got"
        fi
        i=$((i+1))
    done
done

echo "---"
if [ "$total" -gt 0 ]; then
    pct=$(( landed * 100 / total ))
    echo "felt-intelligence: ${pct}% (${landed}/${total} turns landed)"
else
    echo "felt-intelligence: n/a (no dialogues)"
fi
