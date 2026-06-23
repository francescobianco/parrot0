#!/usr/bin/env bash
#
# gen195: CURATION step (uses the NETWORK) — fetch REAL SWE-bench_Lite instances
# from the HuggingFace datasets-server and commit them as STATIC fixtures under
# tests/swebench/lite/<instance_id>/. This is run by the agent/maintainer, exactly
# like the Wikipedia corpus under kb/learning/: the network is touched ONCE, here,
# at curation time — parrot0 itself never touches the net (PRINCIPLES.md). The
# committed JSON is the static snapshot the offline harness (swebench.sh) reads.
#
# Dataset: https://huggingface.co/datasets/SWE-bench/SWE-bench_Lite (300 real
# GitHub issues; the gold patch + FAIL_TO_PASS tests are included for reference).
#
# Usage: tests/swebench/fetch_lite.sh [N] [--force]   (default N=5, "per iniziare")
#
# CACHING: the raw datasets-server response is cached under the project's gitignored
# HF cache (.cache/huggingface/datasets/swebench, the same convention BENCH_CACHE
# uses). A re-run with the same N reuses the cache and touches NO network; pass
# --force to refetch. This is why re-running does not re-download the dataset.
set -eu

N="5"
FORCE=0
for a in "$@"; do
    case "$a" in
        --force|-f) FORCE=1 ;;
        ''|*[!0-9]*) : ;;        # ignore non-numeric / flags already handled
        *) N="$a" ;;
    esac
done

DATASET="SWE-bench/SWE-bench_Lite"
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
OUT="$HERE/lite"
CACHE="${SWEBENCH_CACHE:-$ROOT/.cache/huggingface/datasets/swebench}"
mkdir -p "$OUT" "$CACHE"

cache_file="$CACHE/lite_test_0_${N}.json"
url="https://datasets-server.huggingface.co/rows?dataset=${DATASET}&config=default&split=test&offset=0&length=${N}"

if [ "$FORCE" -eq 0 ] && [ -s "$cache_file" ]; then
    echo "using cached rows: $cache_file (no network; --force to refetch)"
else
    echo "fetching $N instance(s) of $DATASET (network) ..."
    curl -sS -m 90 "$url" -o "$cache_file"
fi
tmp="$cache_file"

count="$(jq '.rows | length' "$tmp")"
[ "$count" -gt 0 ] || { echo "no rows returned" >&2; exit 1; }

for i in $(seq 0 $((count - 1))); do
    id="$(jq -r ".rows[$i].row.instance_id" "$tmp")"
    d="$OUT/$id"; mkdir -p "$d"
    jq -S ".rows[$i].row" "$tmp" > "$d/instance.json"
    jq -r ".rows[$i].row.problem_statement" "$tmp" > "$d/problem.md"
    echo "  committed lite/$id"
done
echo "done: $count instance(s) under $OUT"
