#!/usr/bin/env bash
#
# gen194: make swe-bench — the NORTH-STAR harness in DEGRADE (behavioural) mode.
#
# docs/CODE-MASTERY.md §8: SWE-bench is the honest scoreboard for "handles code
# like an LLM coding agent" — real issues where a patch must make a repo's own
# tests pass. A full run needs faculties parrot0 lacks (arbitrary-repo read,
# issue->code localization, multi-file patch, isolated test exec). So per §8 we
# run a DEGRADE PATH first: static, offline instances (no network — PRINCIPLES.md),
# scoring the sub-goals we CAN check and INTERCEPTING where parrot0 fails, so the
# first unsolved instance becomes the next concrete task (the discovery method).
#
# Each instance is tests/swebench/<id>/ with:
#   instance.md   #id #title #repo #problem #gold_file #gold_function
#   repo/*.c      a small repository; one .c with main() is the hidden test
#
# Sub-goal ladder (grounded where an oracle exists):
#   S1 reproduce   — compile+run the repo; the bug is REAL iff the test FAILS.
#   S2 localize    — can parrot0 name the file that defines the gold function?
#   S3 fix         — produce a patch that makes the test pass (the open frontier).
# S1/S2 use real tools; S3 is the pull. The harness never fails the build; it
# prints the first instance whose fix is unsolved as "NEXT PROBLEM TO SOLVE".
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
SWE_DIR="$ROOT/tests/swebench"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

# Hermetic, like codebench: prove code understanding by the brain alone.
export PARROT0_BASE= PARROT0_SESSION=

[ -x "$BIN" ] || { echo "swebench: binary not built ($BIN)" >&2; exit 1; }

field() { sed -n "s/^#$1:[[:space:]]*//p" "$2" | head -1; }

instances=0
next_problem=""
next_id=""

shopt -s nullglob
for inst in "$SWE_DIR"/*/; do
    md="$inst/instance.md"
    [ -f "$md" ] || continue
    instances=$((instances + 1))
    id="$(field id "$md")"
    title="$(field title "$md")"
    problem="$(field problem "$md")"
    repo_rel="$(field repo "$md")"
    gold_file="$(field gold_file "$md")"
    gold_fn="$(field gold_function "$md")"
    repo_abs="$ROOT/$repo_rel"

    echo "# $id — $title"
    echo "    problem: $problem"

    # S1 reproduce: compile+link+run the repo's test; the bug is real iff it fails.
    s1="MISS"
    if cc -w "$repo_abs"/*.c -o "$TMP/out" 2>"$TMP/err"; then
        if "$TMP/out" >/dev/null 2>&1; then
            echo "    S1 reproduce : the test PASSES — no bug to fix here."
        else
            s1="OK"; echo "    S1 reproduce : OK   the test fails (bug reproduced, grounded)."
        fi
    else
        echo "    S1 reproduce : the repo does not build (cc error)."
    fi

    # S2 localize: can parrot0 name the file defining the gold function?
    reply="$(printf 'which file under %s defines %s\n' "$repo_rel" "$gold_fn" | "$BIN" 2>/dev/null | head -1)"
    if printf '%s' "$reply" | grep -q "$gold_file"; then
        s2="OK"; echo "    S2 localize  : OK   parrot0 -> $reply"
    else
        s2="MISS"; echo "    S2 localize  : MISS parrot0 -> $reply"
    fi

    # S3 fix: produce a patch that makes the test pass. Not a faculty yet.
    s3="MISS"
    echo "    S3 fix       : MISS no patch faculty yet (issue -> edit -> test-green)."

    if [ "$s3" = "MISS" ] && [ -z "$next_id" ]; then
        next_id="$id"; next_problem="$problem"
    fi
    echo
done

echo "---"
echo "instances: $instances"
if [ -n "$next_id" ]; then
    echo "NEXT PROBLEM TO SOLVE: $next_id"
    echo "  $next_problem"
    echo "  (gold: fix so the hidden test goes green — pulls a fix-patch transformation + run_execute)"
else
    echo "all instances solved"
fi
echo "PASS swebench: $instances instance(s) probed (degrade mode; never fails the build)"
