#!/usr/bin/env bash
#
# gen195: make swe-bench — the REAL SWE-bench_Lite north star (CODE-MASTERY.md §8),
# run OFFLINE in DEGRADE / behavioural mode.
#
# The instances under tests/swebench/lite/ are REAL SWE-bench_Lite rows
# (https://huggingface.co/datasets/SWE-bench/SWE-bench_Lite), fetched once by
# tests/swebench/fetch_lite.sh and committed as STATIC snapshots — exactly like the
# Wikipedia corpus under kb/learning/. parrot0 itself never touches the network
# (PRINCIPLES.md); this harness only reads the committed JSON.
#
# HONESTY (read this): a full SWE-bench run needs faculties parrot0 does NOT have —
# the repos are PYTHON (parrot0's code engine is C-only), they are not checked out
# at the base commit, and the hidden pytest suite needs a Python+Docker env. So we
# CANNOT reproduce or patch these yet. This harness therefore measures the only
# honest thing available now: it feeds parrot0 the real issue and reports, per
# instance, exactly which faculty blocks it — the intercepted-failure map (§8) that
# names the next pulls. The score is NOT comparable to published SWE-bench numbers.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
LITE="$ROOT/tests/swebench/lite"
export PARROT0_BASE= PARROT0_SESSION=

[ -x "$BIN" ] || { echo "swebench: binary not built ($BIN)" >&2; exit 1; }
command -v jq >/dev/null || { echo "swebench: jq required to read instance.json" >&2; exit 1; }

echo "REAL SWE-bench_Lite, offline static snapshot, behavioural degrade mode."
echo "parrot0 is C-only with no repo checkout / Python exec — see the blockers below."
echo

instances=0
engaged=0
first_id=""
first_repo=""
first_problem=""

shopt -s nullglob
for d in "$LITE"/*/; do
    js="$d/instance.json"
    [ -f "$js" ] || continue
    instances=$((instances + 1))

    id="$(jq -r '.instance_id' "$js")"
    repo="$(jq -r '.repo' "$js")"
    f2p="$(jq -r '.FAIL_TO_PASS | fromjson | .[0] // "?"' "$js" 2>/dev/null || echo "?")"
    title="$(jq -r '.problem_statement' "$js" | sed '/^[[:space:]]*$/d' | head -1)"

    # Language of the hidden test → which front-end parrot0 would need.
    lang="unknown"
    case "$f2p" in *.py::*|*.py:*) lang="Python" ;; esac

    echo "# $id   ($repo)"
    echo "    issue : $title"
    echo "    test  : $f2p"

    # Behavioural probe: feed the real issue title; capture what parrot0 does.
    reply="$(printf '%s\n' "$title" | "$BIN" 2>/dev/null | head -1)"
    echo "    parrot0: $reply"

    # Honest blocker analysis (no fabricated success).
    if [ "$lang" = "Python" ]; then
        echo "    blocker: gen196 reads Python STRUCTURE (defs/calls/locate-by-name), but"
        echo "             cannot localize from the issue (X6), reason Python semantics, patch"
        echo "             (X7), or run pytest — see docs/swebench/<id>-distance.md."
    else
        echo "    blocker: repo not checked out + no test-exec env (needs repo checkout + X1 run)."
    fi

    [ -z "$first_id" ] && { first_id="$id"; first_repo="$repo"; first_problem="$title"; }
    echo
done

echo "---"
echo "instances: $instances   engaged (real solve): $engaged"
if [ -n "$first_id" ]; then
    echo "FIRST REAL PROBLEM: $first_id   ($first_repo)"
    echo "  $first_problem"
    echo "  Ordered pulls before this is solvable (TASKLIST X-series):"
    echo "    X3/X4  a Python front-end emitting the shared abstract node vocabulary"
    echo "    (repo) check out the repo at base_commit as static files (curation step)"
    echo "    X1     build+run the project's own test suite, read the real verdict"
    echo "    X6     issue -> file/function localization across the repo"
    echo "    X7     produce a patch that makes FAIL_TO_PASS go green"
fi
echo "PASS swebench: $instances real instance(s) probed (degrade mode; never fails the build)"
