#!/usr/bin/env bash
#
# gen325 (TODO.md P6, forge plan §18) — parrot0 knows where its envelope ENDS.
#
# The defect, from the 2026-07-12 conversation:
#
#   you> what are you unable to do?
#   parrot0> I am parrot0.                       (the identity module took the turn)
#   you> cosa non sai fare?
#   parrot0> I can answer questions about facts and rules, …   (the CAPABILITY list)
#
# The question was inverted and the answer was still a brochure. parrot0 had no
# notion of its own limits, because the self-model derived only from module(name)
# — a module either exists or it does not, which says nothing about how FAR it
# reaches.
#
# The answer is now DERIVED from capability/2 + capability_wall/2, projected by
# tests/capability_facts.py from the ledger the gates verify. This test proves it
# is derived and not written, which is the whole point:
#
#   1. an ADDED faculty appears in the answer with no code change;
#   2. a REMOVED faculty disappears — the ablation. A self-description that
#      cannot shrink is a brochure (PRINCIPLES.md: self-description must be
#      derived from real state and shrink when that state does).
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "selflimits: binary not built ($BIN)" >&2; exit 1; }

pass=0
fail=0
ok() { echo "PASS selflimits: $1"; pass=$((pass+1)); }
no() { echo "FAIL selflimits: $1" >&2; fail=$((fail+1)); }

ask() { printf '%s\n' "$1" | (cd "$2" && PARROT0_SESSION= "$BIN" 2>/dev/null); }

# ---- 1. the real ledger answers, in both languages and both polarities -------
for q in "what are you unable to do?" "what can't you do?" "cosa non sai fare?"; do
    got="$(ask "$q" "$ROOT")"
    if printf '%s' "$got" | grep -q "I cannot do multi file editing"; then
        ok "\"$q\" is answered from the capability ledger"
    else
        no "\"$q\" did not reach the ledger: $got"
    fi
done

# The POSITIVE question must NOT be stolen by the new intent — the negation is
# the whole difference, and normalization erases it (which is why the branch
# reads the raw turn).
got="$(ask "what can you do?" "$ROOT")"
if printf '%s' "$got" | grep -q "I can answer questions about facts and rules" \
   && ! printf '%s' "$got" | grep -q "cannot do"; then
    ok "\"what can you do?\" still gets the capability list, not the limits"
else
    no "the limits intent stole the positive question: $got"
fi
got="$(ask "cosa sai fare?" "$ROOT")"
if ! printf '%s' "$got" | grep -q "cannot do"; then
    ok "\"cosa sai fare?\" still gets the capability list (the IT polarity pair)"
else
    no "the limits intent stole the positive Italian question: $got"
fi

# ---- 2. DERIVATION: the answer follows the facts, in both directions ---------
# parrot0 reads kb/ relative to the working directory, so a mirror tree with a
# modified ledger is a clean ablation — no code path is special-cased for tests.
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
cp -r "$ROOT/kb" "$WORK/kb"

# (a) ADD a faculty that does not exist: it must appear, with its wall, and no C
#     was touched — the faculty names live only in the KB.
{
    grep -v '^capability(multi_file_editing' "$ROOT/kb/core/capabilities.p0" \
        | grep -v '^capability_wall(multi_file_editing'
    printf 'capability(time_travel, absent).\n'
    printf 'capability_wall(time_travel, "a time machine").\n'
} > "$WORK/kb/core/capabilities.p0"

got="$(ask "what are you unable to do?" "$WORK")"
if printf '%s' "$got" | grep -q "time travel" \
   && printf '%s' "$got" | grep -q "a time machine"; then
    ok "a faculty ADDED to the ledger appears in the answer (no code change)"
else
    no "an added faculty did not reach the answer: $got"
fi

# (b) the ABLATION: multi_file_editing was removed above, so it must be GONE.
#     This is the property that separates a derived self-model from a paragraph.
if ! printf '%s' "$got" | grep -q "multi file editing"; then
    ok "a faculty REMOVED from the ledger disappears — the answer shrinks"
else
    no "the answer still claims a faculty the ledger no longer lists: $got"
fi

# (c) with NO ledger at all, parrot0 must not invent limits — it declines the
#     intent rather than fabricating an envelope.
: > "$WORK/kb/core/capabilities.p0"
got="$(ask "what are you unable to do?" "$WORK")"
if ! printf '%s' "$got" | grep -q "capability ledger"; then
    ok "with no ledger, parrot0 claims no limits it cannot evidence"
else
    no "parrot0 cited a ledger that is empty: $got"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
