#!/usr/bin/env bash
#
# gen200: end-to-end — parrot0 DERIVES a patch for a real SWE-bench instance, and
# the REAL test suite decides (no deception, PRINCIPLES.md).
#
#   1. parrot0 (pure C) reads the committed repo file and proposes a fix purely
#      from STRUCTURE (code_symmetry_fix: a sibling branch assigns a literal where
#      the analogous variable belongs). It names no file/function/answer up front.
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

# The repo file parrot0 will examine (the single source under repo_excerpt/).
SRC="$(find "$EXC" -name '*.py' | head -1)"
[ -n "$SRC" ] || { echo "no repo_excerpt source for $INST" >&2; exit 2; }
REL="${SRC#"$EXC"/}"                         # e.g. astropy/modeling/separable.py
RELP="$(python3 -c "import os,sys;print(os.path.relpath(sys.argv[1], sys.argv[2]))" "$SRC" "$ROOT")"

echo "== parrot0 examines $REL =="
rm -f "$SRC.p0fix"
reply="$(printf 'fix the symmetry bug in %s\n' "$RELP" | (cd "$ROOT" && PARROT0_BASE= PARROT0_SESSION= "$BIN" 2>/dev/null))"
echo "parrot0: $reply"
[ -f "$SRC.p0fix" ] || { echo "parrot0 produced no patch — STOP (honest miss)"; exit 1; }

cand="$(mktemp)"; trap 'rm -f "$cand" "$SRC.p0fix"' EXIT
diff -u --label "a/$REL" --label "b/$REL" "$SRC" "$SRC.p0fix" > "$cand" || true
echo "== parrot0's candidate patch =="
cat "$cand"

echo "== handing parrot0's patch to the REAL test oracle =="
"$ROOT/tests/swebench/oracle.sh" "$cand" "$INST"