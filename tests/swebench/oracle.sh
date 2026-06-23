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

run() { docker exec "$cid" bash -lc "cd /testbed && source /opt/miniconda3/bin/activate testbed && python -m pytest -q -p no:cacheprovider \"\$@\"" bash "$@" 2>&1; }

fails=0
echo "--- FAIL_TO_PASS (${#F2P[@]}) ---"
for t in "${F2P[@]}"; do
    if run "$t" | tail -1 | grep -qE '^[0-9]+ passed'; then echo "  PASS  $t"
    else echo "  FAIL  $t"; fails=$((fails+1)); fi
done
echo "--- PASS_TO_PASS (${#P2P[@]}) ---"
p2p_fail=0
for t in "${P2P[@]}"; do
    if run "$t" | tail -1 | grep -qE '^[0-9]+ passed'; then :; else echo "  REGRESSED  $t"; p2p_fail=$((p2p_fail+1)); fi
done
[ "$p2p_fail" -eq 0 ] && echo "  all ${#P2P[@]} still pass"

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