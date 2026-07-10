#!/usr/bin/env bash
#
# savemap.sh — the SOFT save-map (organic placement of learned facts).
#
# When PARROT0_KB_ROOT is set, brain_save_session routes each newly learned ground
# fact into the curated topic file that already holds its nearest kin, instead of
# dumping everything into one session blob. Coordinate = (predicate, first-arg);
# tiers: exact pair -> same predicate -> a default fallback file. This proves all
# three tiers land where the design says, and that the default file keeps only the
# leftovers. Hermetic: builds a throwaway KB tree UNDER the repo root (MCP kb.save
# only accepts repo-relative paths), and cleans it up.
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "savemap: binary not built" >&2; exit 1; }
cd "$ROOT" || exit 1

pass=0 fail=0
ok() { echo "PASS savemap: $1"; pass=$((pass+1)); }
no() { echo "FAIL savemap: $1" >&2; fail=$((fail+1)); }

TREE="p0tmp_savemap_$$"
trap 'rm -rf "$TREE"' EXIT
mkdir -p "$TREE/experts/psychology" "$TREE/core"

cat > "$TREE/experts/psychology/cognitive.p0" <<'P0'
% cognitive psychology
learning_type(observational_learning, "learning by watching others").
learning_type(classical_conditioning, "learning by association").
P0

cat > "$TREE/core/animals.p0" <<'P0'
sound_of(dog, woof).
sound_of(cat, meow).
P0

DEF="$TREE/core/session.p0"
: > "$DEF"

mcp() {
  {
    printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
    printf '%s\n' '{"jsonrpc":"2.0","method":"notifications/initialized"}'
    cat
    printf '%s\n' "{\"jsonrpc\":\"2.0\",\"id\":9,\"method\":\"tools/call\",\"params\":{\"name\":\"kb.save\",\"arguments\":{\"path\":\"$DEF\"}}}"
  } | PARROT0_BASE= PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= \
      PARROT0_KB_ROOT="$TREE" "$BIN" --mcp-engine 2>/dev/null
}

asrt() { # pred arg1 arg2
  printf '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"%s","args":["%s","%s"]}}}\n' "$1" "$2" "$3"
}

{
  asrt learning_type social_learning "learning in groups"   # tier2 -> cognitive.p0
  asrt sound_of dog bark                                     # tier1 -> after dog line
  asrt sound_of cow moo                                      # tier2 -> animals.p0
  asrt gadget widget shiny                                   # tier3 -> default file
} | mcp >/dev/null

COG="$TREE/experts/psychology/cognitive.p0"
ANI="$TREE/core/animals.p0"

grep -q 'learning_type(social_learning' "$COG" \
  && ok "tier2: new subject routed to the predicate's file (cognitive.p0)" \
  || { no "tier2: social_learning not in cognitive.p0"; cat "$COG" >&2; }

grep -q 'social_learning' "$DEF" && no "tier2: leaked into default file" \
  || ok "tier2: not duplicated in the default file"

line_woof=$(grep -n 'sound_of(dog, woof)' "$ANI" | head -1 | cut -d: -f1)
line_bark=$(grep -n 'sound_of(dog, bark)' "$ANI" | head -1 | cut -d: -f1)
if [ -n "$line_woof" ] && [ -n "$line_bark" ] && [ "$line_bark" -eq "$((line_woof+1))" ]; then
    ok "tier1: exact-pair kin -> inserted on the very next line"
else
    no "tier1: dog/bark not right after dog/woof (woof=$line_woof bark=$line_bark)"; cat "$ANI" >&2
fi

grep -q 'sound_of(cow, moo)' "$ANI" && ok "tier2: sound_of(cow) routed to animals.p0" \
  || no "tier2: cow not in animals.p0"

if grep -q 'gadget(widget' "$DEF" && ! grep -rq 'gadget(widget' "$COG" "$ANI"; then
    ok "tier3: unknown predicate fell back to the default file"
else
    no "tier3: gadget(widget) misrouted"
fi

[ -f "$TREE/savemap.tsv" ] && ok "index savemap.tsv written" || no "no savemap.tsv"

before=$(wc -l < "$ANI")
printf '%s\n' "$(asrt sound_of cow moo)" | mcp >/dev/null
after=$(wc -l < "$ANI")
[ "$before" -eq "$after" ] && ok "idempotent: re-saving a known fact does not duplicate" \
  || no "idempotent: animals.p0 grew ($before -> $after)"

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
