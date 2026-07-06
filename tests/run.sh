#!/usr/bin/env bash
#
# parrot0 conversation test harness.
#
# Each test lives in tests/cases/*.chat and uses a tiny line protocol:
#   > user input line   (fed to parrot0's stdin, in order)
#   < expected response (matched against parrot0's stdout, in order)
#   # comment / blank    (ignored)
#
# Matching is over the whole stdout LINE SEQUENCE: most replies are one line,
# but a reply may span several lines (gen269: markdown-fenced code) — write one
# '<' line per output line, consecutive '<' lines belong to the same turn (a
# bare '<' matches an empty line). Interactive drivers that need per-turn
# framing use the PARROT0_EOT end-of-turn marker instead (see tests/*.py).
#
# gen278 (docs/plans/optimize-the-tests.md): the cases are INDEPENDENT and each
# still runs in its OWN parrot0 process (full hermetic isolation preserved), so
# they are run in PARALLEL — this was 67% of `make test`, almost all of it
# repeated process startup / KB loading, not inference. Degree of parallelism is
# $PARROT0_TEST_JOBS (default: nproc). Set it to 1 for the old serial behaviour.
# Output order is made deterministic by aggregating in sorted case order after
# the parallel phase, so PASS/FAIL lines and the summary read identically.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
CASES_DIR="$ROOT/tests/cases"

# Hermetic: disable knowledge-file loading so cases don't depend on kb/.
# gen240: pin the language to English so cases are deterministic regardless of the
# developer's OS locale (current_language is seeded from the launching ENV). Cases
# that exercise Italian replies switch via clear in-turn Italian markers.
export PARROT0_BASE= PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_LANG=en

if [ ! -x "$BIN" ]; then
    echo "test: binary not built ($BIN)" >&2
    exit 1
fi

# gen270: cases that exercise real file CREATION use p0tmp_* names; remove
# leftovers so the create/refuse pair stays deterministic across runs. (The two
# such cases use distinct names — p0tmp_pupo / p0tmp_pupo2 — so parallel runs do
# not race on the filesystem.)
rm -f "$ROOT"/p0tmp_* 2>/dev/null

JOBS="${PARROT0_TEST_JOBS:-$(nproc 2>/dev/null || echo 4)}"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
export BIN WORK

# Run ONE case in its own process; write its human-readable lines to
# $WORK/<name>.out (stdout-bound) and $WORK/<name>.err (stderr-bound), and its
# verdict to $WORK/<name>.status. Pure function of the case file — safe to run
# many at once.
run_one() {
    local case_file="$1"
    local name; name="$(basename "$case_file")"

    local inputs=() expected=()
    local raw
    while IFS= read -r raw || [ -n "$raw" ]; do
        case "$raw" in
            '>'*) inputs+=("${raw:1}") ;;     # strip leading '>'
            '<'*) expected+=("${raw:1}") ;;   # strip leading '<'
            *)    : ;;                          # ignore everything else
        esac
    done < "$case_file"

    # Trim one leading space from each captured field, if present.
    local in_lines="" line
    for line in ${inputs[@]+"${inputs[@]}"}; do
        in_lines+="${line# }"$'\n'
    done

    local actual
    actual="$(printf '%s' "$in_lines" | "$BIN" 2>/dev/null)"

    # Compare expected response lines against actual stdout lines, in order.
    local ok=1 i=0 got want errout=""
    while IFS= read -r got; do
        want="${expected[$i]-__NO_MORE__}"
        want="${want# }"
        if [ "$want" = "__NO_MORE__" ]; then
            break
        fi
        if [ "$got" != "$want" ]; then
            ok=0
            errout+="FAIL $name: turn $((i+1)): want [$want] got [$got]"$'\n'
        fi
        i=$((i+1))
    done <<< "$actual"

    if [ "$i" -lt "${#expected[@]}" ]; then
        ok=0
        errout+="FAIL $name: expected ${#expected[@]} responses, saw $i"$'\n'
    fi

    printf '%s' "$errout" > "$WORK/$name.err"
    if [ "$ok" -eq 1 ]; then
        printf 'PASS %s\n' "$name" > "$WORK/$name.out"
        printf 'pass' > "$WORK/$name.status"
    else
        : > "$WORK/$name.out"
        printf 'fail' > "$WORK/$name.status"
    fi
}
export -f run_one

# Parallel phase: feed every case to `run_one` across $JOBS workers. Each worker
# is a fresh bash that spawns its own parrot0 — the isolation the serial harness
# gave, without the serial cost.
shopt -s nullglob
cases=("$CASES_DIR"/*.chat)
printf '%s\0' "${cases[@]}" \
    | xargs -0 -P "$JOBS" -I{} bash -c 'run_one "$@"' _ {}

# Aggregate in sorted case order so the printed log is deterministic.
pass=0
fail=0
for case_file in "${cases[@]}"; do
    name="$(basename "$case_file")"
    [ -f "$WORK/$name.err" ] && cat "$WORK/$name.err" >&2
    [ -s "$WORK/$name.out" ] && cat "$WORK/$name.out"
    if [ "$(cat "$WORK/$name.status" 2>/dev/null)" = pass ]; then
        pass=$((pass+1))
    else
        fail=$((fail+1))
    fi
done

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
