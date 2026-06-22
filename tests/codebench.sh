#!/usr/bin/env bash
#
# gen173: code-mastery discovery harness (docs/CODE-MASTERY.md).
#
# Each tests/code/*.code is ONE session that submits code snippets and observes
# parrot0's behaviour — does the structural-deductive engine (AST-as-KB) answer,
# and where does it stop? The point is to PULL the next faculty from real
# pressure, not to hardcode a roadmap (see superglue-discovery-harness).
#
# Turn protocol (soft substring match, like composebench):
#   > user turn
#   < satisfying substring (lowercased, punctuation-stripped)
# Header metadata:
#   #expect: pass | gap     pass = ratcheted gate; gap = recorded growth edge
#   #faculty: <name>        which code faculty this stimulus exercises
#
# A `#expect: pass` file that misses ANY turn fails the bench (earned structure
# never silently regresses). A `#expect: gap` file tolerates misses and reports
# the first one — that is the next pull, not a failure.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
CODE_DIR="$ROOT/tests/code"

# Hermetic: no kb/ files, so code understanding is proven by the brain alone.
export PARROT0_BASE= PARROT0_SESSION=

if [ ! -x "$BIN" ]; then
    echo "codebench: binary not built ($BIN)" >&2
    exit 1
fi

norm() {
    printf "%s" "$1" | tr "[:upper:]" "[:lower:]" \
        | tr -d "[:punct:]" | tr -s " " | sed "s/^ *//;s/ *$//"
}

files=0
gates=0
gate_fail=0
gaps=0
total_turns=0
landed_turns=0

shopt -s nullglob
for cf in "$CODE_DIR"/*.code; do
    files=$((files + 1))
    name="$(basename "$cf" .code)"

    inputs=()
    expected=()
    expect="pass"
    faculty="?"
    while IFS= read -r raw || [ -n "$raw" ]; do
        case "$raw" in
            "#expect:"*)  expect="$(printf "%s" "${raw#\#expect:}" | tr -d ' ')" ;;
            "#faculty:"*) faculty="$(printf "%s" "${raw#\#faculty:}" | sed 's/^ *//;s/ *$//')" ;;
            ">"*) inputs+=("${raw#> }") ;;
            "<"*) expected+=("${raw#< }") ;;
            *) : ;;
        esac
    done < "$cf"

    if [ "${#inputs[@]}" -ne "${#expected[@]}" ]; then
        echo "codebench: $name has ${#inputs[@]} inputs but ${#expected[@]} expectations" >&2
        exit 1
    fi

    in_lines=""
    for line in "${inputs[@]}"; do
        printf -v in_lines "%s%s\n" "$in_lines" "$line"
    done
    actual="$(printf "%s" "$in_lines" | "$BIN" 2>/dev/null)"
    mapfile -t outs <<< "$actual"

    echo "# $name [faculty=$faculty, expect=$expect]"
    file_landed=0
    first_miss=""
    for i in "${!expected[@]}"; do
        want="${expected[$i]}"
        got="${outs[$i]-}"
        nw="$(norm "$want")"
        ng="$(norm "$got")"
        total_turns=$((total_turns + 1))
        if [ -n "$nw" ] && printf "%s" "$ng" | grep -qF -- "$nw"; then
            landed_turns=$((landed_turns + 1))
            file_landed=$((file_landed + 1))
            printf "    OK   %s\n" "$got"
        else
            [ -z "$first_miss" ] && first_miss="\"${inputs[$i]}\" -> \"$got\" (wanted ~ \"$want\")"
            printf "    MISS %s\n" "$got"
        fi
    done

    if [ "$file_landed" -eq "${#expected[@]}" ]; then
        echo "  result: HOLDS (all ${#expected[@]} turns landed)"
        [ "$expect" = "pass" ] && gates=$((gates + 1))
    else
        if [ "$expect" = "pass" ]; then
            echo "  !! REGRESSION: a ratcheted code gate broke. first miss: $first_miss"
            gate_fail=1
        else
            echo "  gap (next pull): $first_miss"
            gaps=$((gaps + 1))
        fi
    fi
    echo
done

if [ "$files" -eq 0 ]; then
    echo "codebench: no stimuli in $CODE_DIR" >&2
    exit 1
fi

echo "---"
echo "stimuli: $files"
echo "ratcheted gates holding: $gates"
echo "open gaps (next pulls): $gaps"
if [ "$total_turns" -gt 0 ]; then
    echo "turn landing: $((landed_turns * 100 / total_turns))% ($landed_turns/$total_turns)"
fi

if [ "$gate_fail" -ne 0 ]; then
    echo "FAIL codebench: a ratcheted code gate regressed"
    exit 1
fi
echo "PASS codebench: $gates gates hold; $gaps gaps recorded"
