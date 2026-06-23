#!/usr/bin/env bash
#
# gen200: the REAL SWE-bench test oracle for one instance — the grounded verifier
# (CODE-MASTERY §4: a runtime is a deterministic tool, not outsourced intelligence;
# §6: code is the domain where "understood" has a mechanical oracle).
#
# It runs the OFFICIAL SWE-bench evaluation image (pulled once — a curation step,
# like fetching Wikipedia; parrot0 itself never touches the network). Given a
# CANDIDATE patch, it reproduces the published protocol exactly:
#   checkout base_commit -> apply the instance test_patch (adds FAIL_TO_PASS)
#   -> apply the candidate -> run FAIL_TO_PASS and PASS_TO_PASS.
# Verdict RESOLVED only if every FAIL_TO_PASS now passes AND every PASS_TO_PASS
# still passes — exactly SWE-bench's definition. The candidate is decided by the
# real test, never by a hardcoded answer: that is what makes a fix non-deceptive.
#
# Usage: tests/swebench/oracle.sh <candidate_patch_file> [instance_id]
#        candidate "" or omitted -> RED baseline (no fix) check.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)/.."
ROOT="$(cd "$ROOT" && pwd)"
CAND="${1:-}"
INST="${2:-astropy__astropy-12907}"
JS="$ROOT/tests/swebench/lite/$INST/instance.json"

command -v docker >/dev/null || { echo "oracle: docker required" >&2; exit 2; }
command -v jq     >/dev/null || { echo "oracle: jq required"     >&2; exit 2; }
[ -f "$JS" ] || { echo "oracle: no instance.json for $INST" >&2; exit 2; }

# SWE-bench image name: astropy__astropy-12907 -> astropy_1776_astropy-12907.
img_key="$(printf '%s' "$INST" | sed 's/__/_1776_/')"
IMG="swebench/sweb.eval.x86_64.${img_key}:latest"

base="$(jq -r '.base_commit' "$JS")"
mapfile -t F2P < <(jq -r '.FAIL_TO_PASS | fromjson | .[]' "$JS")
mapfile -t P2P < <(jq -r '.PASS_TO_PASS | fromjson | .[]' "$JS")

docker image inspect "$IMG" >/dev/null 2>&1 || {
    echo "oracle: pulling $IMG (once) ..." >&2
    docker pull "$IMG" >&2 || { echo "oracle: pull failed" >&2; exit 2; }
}

tmp="$(mktemp -d)"; trap 'rm -rf "$tmp"' EXIT
jq -r '.test_patch' "$JS" > "$tmp/test_patch.diff"
: > "$tmp/candidate.diff"
[ -n "$CAND" ] && cp "$CAND" "$tmp/candidate.diff"

cid="$(docker create "$IMG" sleep 3600)"; docker start "$cid" >/dev/null
trap 'docker rm -f "$cid" >/dev/null 2>&1; rm -rf "$tmp"' EXIT
docker cp "$tmp/test_patch.diff" "$cid:/tmp/test_patch.diff"
docker cp "$tmp/candidate.diff"  "$cid:/tmp/candidate.diff"

docker exec "$cid" bash -lc "cd /testbed && git checkout -f $base >/dev/null 2>&1 && git apply /tmp/test_patch.diff" \
    || { echo "oracle: could not checkout base + apply test_patch" >&2; exit 2; }

if [ -s "$tmp/candidate.diff" ]; then
    if ! docker exec "$cid" bash -lc 'cd /testbed && git apply /tmp/candidate.diff 2>/tmp/applyerr'; then
        echo "RESULT: candidate patch DID NOT APPLY"
        docker exec "$cid" bash -lc 'cat /tmp/applyerr' 2>/dev/null | head -5
        echo "verdict: FAIL"
        exit 1
    fi
    echo "candidate patch applied."
else
    echo "no candidate (RED baseline check)."
fi

# Run a whole set of tests in ONE pytest invocation (fast, the standard way).
run() { docker exec "$cid" bash -lc "cd /testbed && source /opt/miniconda3/bin/activate testbed && python -m pytest -q -p no:cacheprovider \"\$@\"" bash "$@" 2>&1; }
strip_ansi() { sed -r 's/\x1b\[[0-9;]*[a-zA-Z]//g'; }
num_before() { grep -oE "[0-9]+ $1" | grep -oE '[0-9]+' | head -1; }   # "$1" = passed|failed|error(s)

# How many of `set` pass with no failures/errors. Echoes "<passed> <failed>".
counts() {
    local clean p f e
    clean="$(printf '%s\n' "$1" | strip_ansi)"
    p="$(printf '%s\n' "$clean" | num_before passed)";  [ -z "$p" ] && p=0
    f="$(printf '%s\n' "$clean" | num_before failed)";  [ -z "$f" ] && f=0
    e="$(printf '%s\n' "$clean" | grep -oE '[0-9]+ errors?' | grep -oE '[0-9]+' | head -1)"; [ -z "$e" ] && e=0
    echo "$p $((f + e))"
}

echo "--- FAIL_TO_PASS (${#F2P[@]}) ---"
read f2p_pass f2p_bad < <(counts "$(run "${F2P[@]}")")
echo "  passed $f2p_pass / ${#F2P[@]}   (failed/error: $f2p_bad)"
fails=$(( ${#F2P[@]} - f2p_pass )); [ "$f2p_bad" -gt 0 ] && fails=$(( fails > f2p_bad ? fails : f2p_bad ))

echo "--- PASS_TO_PASS (${#P2P[@]}) ---"
p2p_fail=0
if [ "${#P2P[@]}" -gt 0 ]; then
    read p2p_pass p2p_bad < <(counts "$(run "${P2P[@]}")")
    echo "  passed $p2p_pass / ${#P2P[@]}   (failed/error: $p2p_bad)"
    p2p_fail=$(( ${#P2P[@]} - p2p_pass )); [ "$p2p_fail" -lt "$p2p_bad" ] && p2p_fail=$p2p_bad
fi

echo "---"
if [ "$fails" -eq 0 ] && [ "$p2p_fail" -eq 0 ] && [ -s "$tmp/candidate.diff" ]; then
    echo "verdict: RESOLVED"
    exit 0
elif [ -s "$tmp/candidate.diff" ]; then
    echo "verdict: FAIL ($fails FAIL_TO_PASS unmet, $p2p_fail PASS_TO_PASS regressed)"
    exit 1
else
    echo "verdict: RED baseline ($fails/${#F2P[@]} FAIL_TO_PASS fail as expected with no fix)"
    exit 1
fi