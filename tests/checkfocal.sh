#!/usr/bin/env bash
#
# gen318 (forge-master-plan W0b, M-1a §19) — the ratchet for the FOCAL runner
# (`make check TEST=<id>`, tests/check.py).
#
# A runner that can only say PASS is worthless, so this proves the three
# properties M-1a actually claims, against a FIXTURE catalog ($PARROT0_CONTRACTS)
# holding a known-red contract:
#
#   1. the real contract is addressable by id and green, inside its lane budget;
#   2. fail-fast is REAL — a red contract exits 1, names the first broken turn
#      and its declared owner, and the contract queued behind it is NOT run;
#   3. execution and function stay separate (§1.8/§10.6) — a broken harness is
#      execution_state=infra_error / verdict=unknown and exits 2, never a
#      functional FAIL that a blind re-run could turn green.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "checkfocal: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS checkfocal: $1"; pass=$((pass+1)); }
no() { echo "FAIL checkfocal: $1" >&2; fail=$((fail+1)); }

PY="$(test -x "$ROOT/.venv/bin/python" && echo "$ROOT/.venv/bin/python" || echo python3)"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

# ---- 1. the REAL contract, addressed by id, is green -------------------------
out="$(cd "$ROOT" && TEST=routing.agent-search.en "$PY" tests/check.py 2>&1)"; rc=$?
if [ "$rc" -eq 0 ] && printf '%s\n' "$out" | grep -q '^\[S1 1/1 PASS'; then
    ok "routing.agent-search.en addressable by id and green"
else
    no "the real focal contract did not run green (rc=$rc): $out"
fi
if printf '%s\n' "$out" | grep -q 'within focal budget'; then
    ok "focal contract inside its 5 s lane budget"
else
    no "focal contract outside its lane budget: $out"
fi

# ---- 2. fail-fast is real: a red contract stops the run ----------------------
# The red case asserts a WRONG answer on turn 2. The sentinel behind it must
# never start (fail-fast), and the run must exit 1 (a functional red).
cat > "$WORK/red.chat" <<'CASE'
> what is 2 plus 2?
< 4.
> what is 10 minus 3?
< 999.
CASE
cat > "$WORK/sentinel.chat" <<'CASE'
> what is 2 plus 2?
< 4.
CASE
cat > "$WORK/catalog.json" <<JSON
{
  "boot_profiles": {
    "hermetic-en": {"env": {"PARROT0_BASE": "", "PARROT0_SESSION": "",
                            "PARROT0_WORLD_FACTS": "0", "PARROT0_LANG": "en"}}
  },
  "contracts": [
    {"id": "fixture.red", "capability": "fixture", "owner": ["mod_math"],
     "depends_on": [], "boot_profile": "hermetic-en", "tier": "focal",
     "isolation": "process", "oracle": "$WORK/red.chat",
     "oracle_kind": "golden-turn-text", "timeout_ms": 4000, "mutators": []},
    {"id": "fixture.sentinel", "capability": "fixture", "owner": ["mod_math"],
     "depends_on": [], "boot_profile": "hermetic-en", "tier": "focal",
     "isolation": "process", "oracle": "$WORK/sentinel.chat",
     "oracle_kind": "golden-turn-text", "timeout_ms": 5000, "mutators": []}
  ]
}
JSON

out="$(cd "$ROOT" && PARROT0_CONTRACTS="$WORK/catalog.json" TEST=fixture \
        "$PY" tests/check.py 2>&1)"; rc=$?
if [ "$rc" -eq 1 ]; then
    ok "a red contract exits 1 (functional red)"
else
    no "a red contract did not exit 1 (rc=$rc): $out"
fi
if printf '%s\n' "$out" | grep -q '^\[S1 1/2 FAIL'; then
    ok "the red contract streams a FAIL verdict"
else
    no "no FAIL verdict streamed: $out"
fi
# It must name the TURN that broke and the OWNER the catalog declares — not
# merely print a wrong sentence.
if printf '%s\n' "$out" | grep -q "turn 2" \
   && printf '%s\n' "$out" | grep -q 'expected: 999\.' \
   && printf '%s\n' "$out" | grep -q 'observed: 7\.' \
   && printf '%s\n' "$out" | grep -q 'first broken contract: fixture.red (owner: mod_math)'; then
    ok "the red names the broken turn, expected/observed and the owning module"
else
    no "the red did not name turn/expected/observed/owner: $out"
fi
# Fail-fast: the contract queued behind the red one must never have started.
if ! printf '%s\n' "$out" | grep -q 'fixture.sentinel'; then
    ok "fail-fast is real: the contract behind the red one never ran"
else
    no "fail-fast did NOT stop the run — the sentinel ran anyway: $out"
fi

# ---- 3. a broken harness is not a functional FAIL (§1.8) ---------------------
cat > "$WORK/broken.json" <<JSON
{
  "boot_profiles": {
    "hermetic-en": {"env": {"PARROT0_BASE": "", "PARROT0_SESSION": ""}}
  },
  "contracts": [
    {"id": "fixture.missing-oracle", "capability": "fixture", "owner": ["none"],
     "depends_on": [], "boot_profile": "hermetic-en", "tier": "focal",
     "isolation": "process", "oracle": "$WORK/does-not-exist.chat",
     "oracle_kind": "golden-turn-text", "timeout_ms": 4000, "mutators": []}
  ]
}
JSON

out="$(cd "$ROOT" && PARROT0_CONTRACTS="$WORK/broken.json" TEST=fixture \
        "$PY" tests/check.py 2>&1)"; rc=$?
if [ "$rc" -eq 2 ]; then
    ok "a broken harness exits 2 (not-run), not 1 (functional red)"
else
    no "a broken harness did not exit 2 (rc=$rc): $out"
fi
if printf '%s\n' "$out" | grep -q 'execution_state=infra_error' \
   && printf '%s\n' "$out" | grep -q 'verdict=unknown'; then
    ok "a broken harness reports infra_error/unknown, never PASS or FAIL"
else
    no "a broken harness was not reported as infra_error/unknown: $out"
fi

# ---- 4. an unknown id is refused, not silently green -------------------------
out="$(cd "$ROOT" && TEST=no.such.contract "$PY" tests/check.py 2>&1)"; rc=$?
if [ "$rc" -eq 2 ] && printf '%s\n' "$out" | grep -q 'no contract matches'; then
    ok "an unknown contract id is refused (exit 2), not reported green"
else
    no "an unknown contract id was not refused (rc=$rc): $out"
fi

# ---- 5. gen319: a script-row contract runs ALONE (§15 row 2) -----------------
# llmscore_world is 127 fresh boots / ~70 s; a single probe must be addressable
# without paying for the 126 nobody asked about. The full-suite oracle itself is
# ratcheted where it always was — `make test` runs the whole script.
out="$(cd "$ROOT" && TEST=world.tallest-mountain.en "$PY" tests/check.py 2>&1)"; rc=$?
if [ "$rc" -eq 0 ] && printf '%s\n' "$out" | grep -q '^\[S1 1/1 PASS'; then
    ok "a script-row probe is addressable alone and green"
else
    no "the script-row probe did not run green (rc=$rc): $out"
fi

# Every probe must be addressable, and no two may share a slug — a duplicate id
# would silently shadow a probe, and a shadowed probe is an unratcheted one.
ids="$(cd "$ROOT" && ./tests/llmscore_world.sh --list)"
n_ids="$(printf '%s\n' "$ids" | wc -l)"
n_uniq="$(printf '%s\n' "$ids" | sort -u | wc -l)"
n_rows="$(cd "$ROOT" && grep -cE '^expect(_full|_turns)? "' tests/llmscore_world.sh)"
if [ "$n_ids" -eq "$n_rows" ] && [ "$n_uniq" -eq "$n_ids" ]; then
    ok "all $n_ids llmscore_world probes are addressable, ids unique"
else
    no "probe ids not 1:1 with rows (ids=$n_ids uniq=$n_uniq rows=$n_rows)"
fi

# A mistyped probe id is NOT-RUN, never green.
(cd "$ROOT" && ./tests/llmscore_world.sh --id no-such-probe >/dev/null 2>&1); rc=$?
if [ "$rc" -eq 2 ]; then
    ok "a mistyped probe id exits 2 (not-run), never 0"
else
    no "a mistyped probe id did not exit 2 (rc=$rc)"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]