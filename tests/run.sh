#!/usr/bin/env bash
#
# parrot0 conversation test harness.
#
# Each test lives in tests/cases/*.chat and uses a tiny line protocol:
#   > user input line   (fed to parrot0's stdin, in order)
#   < expected response (matched against parrot0's stdout, in order)
#   # comment / blank    (ignored)
#
# parrot0 emits exactly one stdout line per non-empty input line, so the
# Nth '<' line is compared to the Nth response line.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
CASES_DIR="$ROOT/tests/cases"

if [ ! -x "$BIN" ]; then
    echo "test: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0

shopt -s nullglob
for case_file in "$CASES_DIR"/*.chat; do
    name="$(basename "$case_file")"

    inputs=()
    expected=()
    while IFS= read -r raw || [ -n "$raw" ]; do
        case "$raw" in
            '>'*) inputs+=("${raw:1}") ;;     # strip leading '>'
            '<'*) expected+=("${raw:1}") ;;   # strip leading '<'
            *)    : ;;                          # ignore everything else
        esac
    done < "$case_file"

    # Trim one leading space from each captured field, if present.
    in_lines=""
    for line in "${inputs[@]}"; do
        in_lines+="${line# }"$'\n'
    done

    actual="$(printf '%s' "$in_lines" | "$BIN" 2>/dev/null)"

    # Compare expected response lines against actual stdout lines, in order.
    ok=1
    i=0
    while IFS= read -r got; do
        want="${expected[$i]-__NO_MORE__}"
        want="${want# }"
        if [ "$want" = "__NO_MORE__" ]; then
            break
        fi
        if [ "$got" != "$want" ]; then
            ok=0
            echo "FAIL $name: turn $((i+1)): want [$want] got [$got]" >&2
        fi
        i=$((i+1))
    done <<< "$actual"

    if [ "$i" -lt "${#expected[@]}" ]; then
        ok=0
        echo "FAIL $name: expected ${#expected[@]} responses, saw $i" >&2
    fi

    if [ "$ok" -eq 1 ]; then
        echo "PASS $name"
        pass=$((pass+1))
    else
        fail=$((fail+1))
    fi
done

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]