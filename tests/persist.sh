#!/usr/bin/env bash
#
# gen9 persistence round-trip: knowledge survives across runs, layers JOIN,
# and the saved file is the right (readable) delta — no base, no self-model.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"

if [ ! -x "$BIN" ]; then
    echo "persist: binary not built ($BIN)" >&2
    exit 1
fi

tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT
base="$tmp/base.pl"
sess="$tmp/session.pl"

pass=0
fail=0
ok()  { echo "PASS persist: $1"; pass=$((pass+1)); }
no()  { echo "FAIL persist: $1" >&2; fail=$((fail+1)); }

# 1) Teach a fact, then /save -> it lands in the session file, readable.
printf 'socrates is a man\n/save\n/quit\n' \
    | PARROT0_BASE= PARROT0_SESSION="$sess" "$BIN" >/dev/null 2>&1

if grep -Fxq 'man(socrates).' "$sess"; then ok "saved fact is human-readable"
else no "expected 'man(socrates).' in session file"; fi

# 2) The session file must NOT contain the reflective self-model.
if grep -Eq 'i_am|^module\(' "$sess"; then no "reflective facts leaked into save"
else ok "reflective self-model not persisted"; fi

# 3) Re-run with that session file -> the fact is remembered.
out="$(printf 'is socrates a man?\n/quit\n' \
      | PARROT0_BASE= PARROT0_SESSION="$sess" "$BIN" 2>/dev/null)"
if [ "$out" = "Yes." ]; then ok "knowledge survives across runs"
else no "after reload expected 'Yes.' got [$out]"; fi

# 4) JOIN: a base file + the session file unify into one KB.
printf '%% base knowledge\nman(plato).\n' > "$base"
out="$(printf 'who is a man?\n/quit\n' \
      | PARROT0_BASE="$base" PARROT0_SESSION="$sess" "$BIN" 2>/dev/null)"
if [ "$out" = "plato, socrates." ]; then ok "base + session join"
else no "expected 'plato, socrates.' got [$out]"; fi

# 5) Saving again must not duplicate the base clause.
printf '/save\n/quit\n' \
    | PARROT0_BASE="$base" PARROT0_SESSION="$sess" "$BIN" >/dev/null 2>&1
if grep -Fxq 'man(plato).' "$sess"; then no "base clause duplicated into session"
else ok "save writes only the session delta"; fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
