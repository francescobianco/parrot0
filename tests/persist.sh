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
base="$tmp/base.p0"
sess="$tmp/session.p0"
neg="$tmp/negative.p0"
corrbase="$tmp/correction-base.p0"
corrsess="$tmp/correction-session.p0"
binbase="$tmp/binary-conflict-base.p0"
binsess="$tmp/binary-conflict-session.p0"
export PARROT0_WORLD_FACTS=0

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

# 6) Explicit negative knowledge persists and reloads as known false.
printf 'zara is not a quoll\n/save\n/quit\n' \
    | PARROT0_BASE= PARROT0_SESSION="$neg" "$BIN" >/dev/null 2>&1
if grep -Fxq 'not(quoll(zara)).' "$neg"; then ok "saved negative fact is human-readable"
else no "expected negative quoll fact in session file"; fi

out="$(printf 'is zara a quoll?\n/quit\n' \
      | PARROT0_BASE= PARROT0_SESSION="$neg" "$BIN" 2>/dev/null)"
if [ "$out" = "No." ]; then ok "negative knowledge survives across runs"
else no "after negative reload expected No. got [$out]"; fi

# 7) A later positive assertion clears the matching persisted negative fact.
printf 'zara is a quoll\n/save\n/quit\n' \
    | PARROT0_BASE= PARROT0_SESSION="$neg" "$BIN" >/dev/null 2>&1
if grep -Fxq 'quoll(zara).' "$neg" && ! grep -Fxq 'not(quoll(zara)).' "$neg"; then
    ok "positive assertion clears saved negative fact"
else
    no "expected positive quoll fact without matching negative"
fi

# 8) A session negative fact can correct a base positive fact without editing base.
printf 'man(socrates).
' > "$corrbase"
printf 'socrates is not a man
/save
/quit
' \
    | PARROT0_BASE="$corrbase" PARROT0_SESSION="$corrsess" "$BIN" >/dev/null 2>&1
out="$(printf 'is socrates a man?
/quit
' \
      | PARROT0_BASE="$corrbase" PARROT0_SESSION="$corrsess" "$BIN" 2>/dev/null)"
report="$(printf 'what do you know about socrates?
/quit
' \
      | PARROT0_BASE="$corrbase" PARROT0_SESSION="$corrsess" "$BIN" 2>/dev/null)"
why="$(printf 'why is socrates a man?
/quit
' \
      | PARROT0_BASE="$corrbase" PARROT0_SESSION="$corrsess" "$BIN" 2>/dev/null)"
if [ "$out" = "Conflicted." ] && [ "$report" = "socrates is conflicted about being a man." ] && [ "$why" = "I have conflicting evidence for that." ] && grep -Fxq 'not(man(socrates)).' "$corrsess" && grep -Fxq 'man(socrates).' "$corrbase"; then
    ok "session negative exposes conflict with base fact"
else
    no "expected session correction to preserve and report base conflict"
fi

# 9) Binary ground queries also expose exact persisted conflicts.
printf 'parent(ada, bob).
' > "$binbase"
printf 'not(parent(ada, bob)).
' > "$binsess"
out="$(printf 'is ada the parent of bob?
/quit
' \
      | PARROT0_BASE="$binbase" PARROT0_SESSION="$binsess" "$BIN" 2>/dev/null)"
why="$(printf 'why is ada the parent of bob?
/quit
' \
      | PARROT0_BASE="$binbase" PARROT0_SESSION="$binsess" "$BIN" 2>/dev/null)"
if [ "$out" = "Conflicted." ] && [ "$why" = "I have conflicting evidence for that." ]; then
    ok "binary ground conflict is query-visible"
else
    no "expected binary conflict query/explanation to be conflicted. got [$out] / [$why]"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
