#!/usr/bin/env bash
#
# gen327 (TODO.md P4 / forge plan §9.3 "Repair loop B4") — the bounded repair loop.
#
# Before this, a candidate the oracle REFUTED was simply abandoned: "the judge ran
# it but it did not sort every vector", and stop. No diagnosis, no alternative, no
# second attempt. That is a one-shot generator with a test attached, not an agent.
#
# The plan's B4 gate is the contract this file checks, and every step is grounded
# in a real compile-and-run — nothing here is judged by inspecting the source:
#
#   OBSERVE   run the real judge (8 vectors, compiled and executed)
#   DIAGNOSE  a STRUCTURED verdict: not_ordered / not_permutation / crashed /
#             build_failed. "It failed" cannot direct a repair.
#   PROPOSE   repair_rule(Diagnosis, Fix) facts in the KB name what to try. The C
#             knows only HOW to transform, never WHICH — the oracle's verdict picks.
#   SELECT    smallest blast radius first.
#   ACT       apply the transformation to a copy.
#   VERIFY    re-run the SAME judge.
#   STOP      within N attempts, and keep the trace either way.
#
# The two defects below are real bubble-sort bugs, and they are DIFFERENT defects
# with DIFFERENT signatures — which is the whole reason the verdict had to become
# structured. A loop that could only say "it failed" could not tell them apart.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "repair: binary not built ($BIN)" >&2; exit 1; }

pass=0
fail=0
ok() { echo "PASS repair: $1"; pass=$((pass+1)); }
no() { echo "FAIL repair: $1" >&2; fail=$((fail+1)); }

ask() { printf '%s\n' "$1" | PARROT0_SESSION= "$BIN" 2>/dev/null; }

GOOD='void mysort(int a[], int n) { for (int i = 0; i < n; i++) { for (int j = 0; j < n - i - 1; j++) { if (a[j] > a[j+1]) { int t = a[j]; a[j] = a[j+1]; a[j+1] = t; } } } }'
# Defect 1: the comparator is inverted — it sorts DESCENDING. The result is a
# permutation, but not non-decreasing: the oracle must say not_ordered.
BAD_ORDER='void mysort(int a[], int n) { for (int i = 0; i < n; i++) { for (int j = 0; j < n - i - 1; j++) { if (a[j] < a[j+1]) { int t = a[j]; a[j] = a[j+1]; a[j+1] = t; } } } }'
# Defect 2: the inner bound is one too far, so a[j+1] reads past the live range and
# swaps stale data in. The result is ORDERED but an element is gone: not_permutation.
BAD_BOUND='void mysort(int a[], int n) { for (int i = 0; i < n; i++) { for (int j = 0; j < n - i; j++) { if (a[j] > a[j+1]) { int t = a[j]; a[j] = a[j+1]; a[j+1] = t; } } } }'
# Unrepairable by any rule we have: it does nothing at all.
BAD_NOOP='void mysort(int a[], int n) { for (int i = 0; i < n; i++) { a[i] = a[i]; } }'

# ---- 1. the two defects are told APART by the oracle, not by the source ------
got="$(ask "fix this code: $BAD_ORDER")"
if printf '%s' "$got" | grep -q "reported not_ordered"; then
    ok "an inverted comparator is diagnosed not_ordered (the run says so)"
else
    no "the order defect was not diagnosed: $got"
fi
got_order="$got"

got="$(ask "fix this code: $BAD_BOUND")"
if printf '%s' "$got" | grep -q "reported not_permutation"; then
    ok "an off-by-one bound is diagnosed not_permutation (an element is lost)"
else
    no "the bound defect was not diagnosed: $got"
fi
got_bound="$got"

# ---- 2. each is REPAIRED, and the repair is VERIFIED BY RUNNING it -----------
if printf '%s' "$got_order" | grep -q "^Repaired, and verified by running it" \
   && printf '%s' "$got_order" | grep -q "The fix: flip_comparator"; then
    ok "the order defect is repaired, and the fix is re-run through the oracle"
else
    no "the order defect was not repaired: $got_order"
fi
if printf '%s' "$got_bound" | grep -q "^Repaired, and verified by running it" \
   && printf '%s' "$got_bound" | grep -q "The fix: tighten_inner_bound"; then
    ok "the bound defect is repaired, and the fix is re-run through the oracle"
else
    no "the bound defect was not repaired: $got_bound"
fi

# The repaired source must be the CORRECT sort — not merely something the model
# calls fixed. Compare against the known-good body.
if printf '%s' "$got_order" | grep -qF 'if (a[j] > a[j+1])'; then
    ok "the emitted repair is the real corrected source"
else
    no "the emitted source is not the corrected one: $got_order"
fi

# ---- 3. the TRACE is kept (the plan requires the whole attempt history) ------
if printf '%s' "$got_order" | grep -q "Trace: the oracle ran it and reported not_ordered; tried flip_comparator -> the oracle now passes it"; then
    ok "the full observe->diagnose->act->verify trace is reported"
else
    no "the trace was not reported: $got_order"
fi

# ---- 4. ANTI-IMPOSTOR: correct code gets NO invented repair ------------------
got="$(ask "what is wrong with this code: $GOOD")"
if printf '%s' "$got" | grep -q "I found nothing to repair"; then
    ok "correct code is run, passes, and NOTHING is invented to fix"
else
    no "it invented a repair for correct code: $got"
fi

# ---- 5. ANTI-IMPOSTOR: an unrepairable defect is DECLINED, not patched -------
# A patch nobody ran is a fabrication, and a wall is better than that.
got="$(ask "fix this code: $BAD_NOOP")"
if printf '%s' "$got" | grep -q "I can't repair that yet" \
   && printf '%s' "$got" | grep -q "not_ordered" \
   && printf '%s' "$got" | grep -q "I could not verify"; then
    ok "a defect no rule fixes is DECLINED, naming the real diagnosis"
else
    no "it did not decline an unrepairable defect honestly: $got"
fi
if printf '%s' "$got" | grep -q '```'; then
    no "it emitted a patch it could not verify"
else
    ok "no unverified patch is ever emitted"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
