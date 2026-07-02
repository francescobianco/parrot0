#!/usr/bin/env bash
#
# gen200: end-to-end — parrot0 DERIVES a patch for a real SWE-bench instance, and
# the REAL test suite decides (no deception, PRINCIPLES.md).
#
#   1. parrot0 (pure C) is handed the repo excerpt DIRECTORY — not a file. It
#      walks the tree itself, localizes the buggy file with its structural-smell
#      chain (code_smell_tree, gen256; before that this script pre-localized the
#      file with `find | head -1`, which was the harness's intelligence, not
#      parrot0's), derives the fix purely from STRUCTURE, and writes a patched
#      copy. It names no file/function/answer up front.
#   2. We mechanically turn parrot0's patched copy into a unified diff (plumbing,
#      not intelligence).
#   3. tests/swebench/oracle.sh runs the official SWE-bench image: checkout base,
#      apply test_patch, apply parrot0's diff, run FAIL_TO_PASS + PASS_TO_PASS.
#
# parrot0 never sees the gold patch or the hidden tests; the oracle is the judge.
set -u

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BIN="$ROOT/bin/parrot0"
INST="${1:-astropy__astropy-12907}"
EXC="$ROOT/tests/swebench/lite/$INST/repo_excerpt"
[ -x "$BIN" ] || { echo "need bin/parrot0 (make)" >&2; exit 2; }
[ -d "$EXC" ] || { echo "no repo_excerpt for $INST" >&2; exit 2; }

# parrot0 gets only the tree, relative to the repo root (its file sandbox).
EXCREL="${EXC#"$ROOT"/}"                     # tests/swebench/lite/<inst>/repo_excerpt

echo "== parrot0 sweeps $EXCREL (self-localization) =="
find "$EXC" -name '*.p0fix' -delete
reply="$(printf 'fix the bug in %s\n' "$EXCREL" | (cd "$ROOT" && PARROT0_BASE= PARROT0_SESSION= "$BIN" 2>/dev/null))"
echo "parrot0: $reply"

FIX="$(find "$EXC" -name '*.p0fix' | head -1)"
[ -n "$FIX" ] || { echo "parrot0 produced no patch — STOP (honest miss)"; exit 1; }
SRC="${FIX%.p0fix}"
REL="${SRC#"$EXC"/}"                         # e.g. astropy/modeling/separable.py
echo "== parrot0 localized $REL =="

cand="$(mktemp)"; trap 'rm -f "$cand" "$FIX"' EXIT
diff -u --label "a/$REL" --label "b/$REL" "$SRC" "$FIX" > "$cand" || true
echo "== parrot0's candidate patch =="
cat "$cand"

echo "== handing parrot0's patch to the REAL test oracle =="
"$ROOT/tests/swebench/oracle.sh" "$cand" "$INST"