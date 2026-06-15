#!/usr/bin/env bash
#
# Local benchmark driver for parrot0.
#
# These are benchmark-driver slices adapted to parrot0's deterministic KB/C
# architecture. They are not the official external SuperGLUE/MMLU/BBH datasets;
# they are executable local probes that measure the current binary against the
# capabilities those benchmark families pressure.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
CASES_DIR="$ROOT/tests/cases"

if [ ! -x "$BIN" ]; then
    echo "bench: binary not built ($BIN)" >&2
    exit 1
fi

usage() {
    echo "usage: $0 {superglue|mmlu|bbh|all}" >&2
}

bench="${1:-}"
if [ -z "$bench" ]; then usage; exit 2; fi

case "$bench" in
    superglue)
        title="SuperGLUE-like local benchmark"
        description="language understanding, coreference, short-text inference"
        chat_cases=(coref.chat entail.chat)
        suites=()
        ;;
    mmlu)
        title="MMLU-like local benchmark"
        description="knowledge retrieval, domain reasoning, transfer"
        chat_cases=(mmlu.chat)
        suites=(grammar.sh)
        ;;
    bbh)
        title="BIG-Bench Hard-like local benchmark"
        description="multi-step reasoning, proof chains, composed inference"
        chat_cases=(rules.chat entail.chat)
        suites=(multigoal.sh explain.sh)
        ;;
    all)
        "$0" superglue
        sg=$?
        echo
        "$0" mmlu
        mm=$?
        echo
        "$0" bbh
        bh=$?
        [ "$sg" -eq 0 ] && [ "$mm" -eq 0 ] && [ "$bh" -eq 0 ]
        exit $?
        ;;
    *) usage; exit 2 ;;
esac

now_ns() { date +%s%N; }
ms_between() { echo $(( ($2 - $1) / 1000000 )); }
percent() {
    awk -v p="$1" -v t="$2" 'BEGIN { if (t == 0) printf "0.00"; else printf "%.2f", (100.0 * p / t) }'
}

pass_total=0
fail_total=0
start_all="$(now_ns)"

run_chat_case() {
    local case_name="$1"
    local case_file="$CASES_DIR/$case_name"
    local start end elapsed
    local inputs=()
    local expected=()
    local actual=()
    local pass=0
    local fail=0

    if [ ! -f "$case_file" ]; then
        echo "FAIL chat $case_name: missing case file" >&2
        fail_total=$((fail_total + 1))
        return
    fi

    while IFS= read -r raw || [ -n "$raw" ]; do
        case "$raw" in
            '>'*) inputs+=("${raw:1}") ;;
            '<'*) expected+=("${raw:1}") ;;
            *) : ;;
        esac
    done < "$case_file"

    local in_lines=""
    local line
    for line in "${inputs[@]}"; do
        in_lines+="${line# }"$'\n'
    done

    start="$(now_ns)"
    mapfile -t actual < <(printf '%s' "$in_lines" | PARROT0_BASE= PARROT0_SESSION= "$BIN" 2>/dev/null)
    end="$(now_ns)"
    elapsed="$(ms_between "$start" "$end")"

    local n="${#expected[@]}"
    local i want got
    for ((i = 0; i < n; i++)); do
        want="${expected[$i]# }"
        got="${actual[$i]-__NO_OUTPUT__}"
        if [ "$got" = "$want" ]; then
            pass=$((pass + 1))
        else
            fail=$((fail + 1))
            echo "  FAIL $case_name turn $((i + 1)): want [$want] got [$got]" >&2
        fi
    done
    if [ "${#actual[@]}" -gt "$n" ]; then
        local extra=$(( ${#actual[@]} - n ))
        fail=$((fail + extra))
        echo "  FAIL $case_name: $extra unexpected extra output line(s)" >&2
    fi

    pass_total=$((pass_total + pass))
    fail_total=$((fail_total + fail))

    local total=$((pass + fail))
    local acc="$(percent "$pass" "$total")"
    if [ "$fail" -eq 0 ]; then
        printf 'PASS chat  %-18s %3d/%-3d %6s%% %4d ms\n' "$case_name" "$pass" "$total" "$acc" "$elapsed"
    else
        printf 'FAIL chat  %-18s %3d/%-3d %6s%% %4d ms\n' "$case_name" "$pass" "$total" "$acc" "$elapsed"
    fi
}

run_suite() {
    local suite_name="$1"
    local suite="$ROOT/tests/$suite_name"
    local start end elapsed output summary pass fail acc total

    if [ ! -x "$suite" ]; then
        echo "FAIL suite $suite_name: missing or not executable" >&2
        fail_total=$((fail_total + 1))
        return
    fi

    start="$(now_ns)"
    output="$($suite 2>&1)"
    end="$(now_ns)"
    elapsed="$(ms_between "$start" "$end")"

    summary="$(printf '%s\n' "$output" | awk '/passed:/ && /failed:/ { gsub(",", "", $2); print $2, $4 }' | tail -1)"
    if [ -z "$summary" ]; then
        echo "$output" >&2
        echo "FAIL suite $suite_name: could not parse summary" >&2
        fail_total=$((fail_total + 1))
        return
    fi

    read -r pass fail <<EOF_SUMMARY
$summary
EOF_SUMMARY

    pass_total=$((pass_total + pass))
    fail_total=$((fail_total + fail))
    total=$((pass + fail))
    acc="$(percent "$pass" "$total")"

    if [ "$fail" -eq 0 ]; then
        printf 'PASS suite %-18s %3d/%-3d %6s%% %4d ms\n' "$suite_name" "$pass" "$total" "$acc" "$elapsed"
    else
        printf 'FAIL suite %-18s %3d/%-3d %6s%% %4d ms\n' "$suite_name" "$pass" "$total" "$acc" "$elapsed"
        printf '%s\n' "$output" >&2
    fi
}

echo "$title"
echo "Driver: $description"
echo "Dataset: local parrot0 benchmark slices (not official external corpus)"
echo "Binary:  $BIN"
echo

case_name=""
for case_name in "${chat_cases[@]}"; do
    run_chat_case "$case_name"
done

suite_name=""
for suite_name in "${suites[@]}"; do
    run_suite "$suite_name"
done

end_all="$(now_ns)"
elapsed_all="$(ms_between "$start_all" "$end_all")"
total=$((pass_total + fail_total))
acc="$(percent "$pass_total" "$total")"

echo "---"
printf 'score:    %s%%\n' "$acc"
printf 'passed:   %d\n' "$pass_total"
printf 'failed:   %d\n' "$fail_total"
printf 'total:    %d\n' "$total"
printf 'elapsed:  %d ms\n' "$elapsed_all"

[ "$fail_total" -eq 0 ]
