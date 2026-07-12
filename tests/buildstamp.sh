#!/usr/bin/env bash
#
# gen321 (forge-master-plan §15 row 7) — the build stamp is isolated.
#
# The debt (§3.5): the commit hash was generated into src/version.h, and every
# object depends on every header, so a SEMANTICALLY NEUTRAL commit invalidated
# all objects and rebuilt the whole brain — 12.4 s of compile buying nothing.
# Worse, it changed the build fingerprint of an engine that had not changed, so
# no content-addressed cache could ever hit across a commit (§10.16).
#
# Two properties are ratcheted here, and they pull against each other — which is
# why both must be pinned:
#
#   1. DERIVATION (gen317's property, must not be lost): brain_version() reports
#      the VERSION file's label and the real git HEAD. A hand-maintained string
#      that drifts from the repo is the exact lie gen317 abolished.
#   2. ISOLATION (gen321's property): when the stamp changes, ONLY version.o is
#      recompiled. The brain TU must not be in the rebuild set.
#
# Isolation is proved against a stamp that really differs, not by reading the
# Makefile: a fake commit is written into the build dir and `make -n` is asked
# what it WOULD rebuild.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
STAMP="$ROOT/obj/version_stamp.h"
[ -x "$BIN" ] || { echo "buildstamp: binary not built ($BIN)" >&2; exit 1; }

pass=0
fail=0
ok() { echo "PASS buildstamp: $1"; pass=$((pass+1)); }
no() { echo "FAIL buildstamp: $1" >&2; fail=$((fail+1)); }

# ---- 1. the stamp is DERIVED from VERSION + git, never hand-written ----------
want_gen="$(cat "$ROOT/VERSION")"
want_commit="$(cd "$ROOT" && git rev-parse --short HEAD 2>/dev/null || echo nogit)"
got="$(printf '/quit\n' | "$BIN" 2>&1 | head -1 | sed -n 's/^parrot0 \[\([^]]*\)\].*/\1/p')"
if [ "$got" = "$want_gen@$want_commit" ]; then
    ok "brain_version derived from VERSION + git HEAD ($got)"
else
    no "version drifted: want [$want_gen@$want_commit] got [$got]"
fi

# ---- 2. a changed stamp rebuilds ONLY the version TU -------------------------
# Write a stamp with a commit that cannot be the real one, then ask make what it
# would rebuild. Before gen321 this listed every object, brain.c included.
trap 'cd "$ROOT" && make -s build >/dev/null 2>&1' EXIT
printf '#define PARROT0_GEN "%s"\n#define PARROT0_COMMIT "deadbee"\n' \
    "$want_gen" > "$STAMP"

plan="$(cd "$ROOT" && make -n build 2>/dev/null | grep -E '^\s*cc .* -c -o ')"
n_compiles="$(printf '%s\n' "$plan" | grep -c . )"

if [ "$n_compiles" -eq 1 ] && printf '%s\n' "$plan" | grep -q 'src/version\.c'; then
    ok "a changed stamp recompiles exactly one TU, and it is src/version.c"
else
    no "a changed stamp recompiles $n_compiles TU(s): $plan"
fi

if printf '%s\n' "$plan" | grep -q 'src/brain\.c'; then
    no "the brain TU is still in the rebuild set of a neutral commit"
else
    ok "the brain TU is NOT rebuilt by a neutral commit (the 12.4 s debt)"
fi

# ---- 3. the generated stamp does not live under src/ -------------------------
# A build artifact under src/ is what dragged the commit into every object's
# header closure. Keep it in the build dir, where a cache can ignore it.
if [ ! -e "$ROOT/src/version_stamp.h" ] && [ -e "$STAMP" ]; then
    ok "the generated stamp lives in the build dir, not in src/"
else
    no "the generated stamp is back under src/ — every object will depend on the commit again"
fi

# src/version.h must be a STABLE tracked declaration, not the generated macros.
if grep -q 'parrot0_version' "$ROOT/src/version.h" \
   && ! grep -q 'define PARROT0_COMMIT' "$ROOT/src/version.h"; then
    ok "src/version.h is a stable declaration, and carries no commit"
else
    no "src/version.h carries the commit again"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
