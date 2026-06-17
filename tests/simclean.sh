#!/usr/bin/env bash
#
# simclean — replay the chatsim transcripts in tests/chat/sim/ against the
# CURRENT parrot0 and prune the ones that have graduated.
#
# Rationale (see JOURNAL / the discovery harness): a sim log earns its keep by
# exposing a GAP — a turn where parrot0 falls back to its honest "I don't
# understand" wall (any of the rotating not-understood variants). parrot0 evolves
# over generations, so a log recorded long ago may no longer wall at all when the
# inputs are replayed against today's brain. Such a log has nothing left to teach
# the project and is deleted. A log with no real turns (model-error stubs) is
# likewise useless and deleted. A log that still walls is KEPT, and its failing
# inputs are printed — those are the live growth edges.
#
# Each "=== conversation N ===" block is replayed as a FRESH parrot0 session
# (independent persona, no state leak). One stdout line per input line.
#
# Usage:
#   tests/simclean.sh            # autonomous: delete graduated/empty logs
#   tests/simclean.sh -n         # dry run: report only, delete nothing
#
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
SIM="$ROOT/tests/chat/sim"

APPLY=1
case "${1:-}" in
  -n|--dry-run) APPLY=0 ;;
  "") ;;
  *) echo "usage: $0 [-n|--dry-run]" >&2; exit 2 ;;
esac

# Build the current brain so the replay reflects today's parrot0.
make -s -C "$ROOT" build || { echo "simclean: build failed" >&2; exit 1; }
[ -x "$BIN" ] || { echo "simclean: no binary at $BIN" >&2; exit 1; }

# True if a parrot0 reply is a not-understood fallback (the whole rotating
# family from brain.c's not_understood(), not only the classic line).
is_wall() {
  case "$1" in
    "I don't understand that yet.") return 0 ;;
    "I'm not sure I followed. Can you say it another way?") return 0 ;;
    "I didn't quite catch that. What would you like to know?") return 0 ;;
    "Hmm, that's a bit beyond me right now.") return 0 ;;
    "Hmm, I don't know about "*" yet.") return 0 ;;
  esac
  return 1
}

shopt -s nullglob
logs=("$SIM"/*.log)
if [ ${#logs[@]} -eq 0 ]; then
  echo "simclean: no logs in $SIM"
  exit 0
fi

deleted=0
kept=0
declare -A gap_count   # failing input -> times seen (across kept logs)

for log in "${logs[@]}"; do
  name="$(basename "$log")"
  nconv="$(grep -c '^=== conversation' "$log")"
  [ "$nconv" -eq 0 ] && nconv=1

  total=0
  walls=0
  fails=()

  for ((c = 1; c <= nconv; c++)); do
    mapfile -t inputs < <(awk -v target="$c" '
      /^=== conversation/ { cur++; next }
      cur == target && /^  human> / { sub(/^  human> /, ""); print }
    ' "$log")
    [ ${#inputs[@]} -eq 0 ] && continue

    mapfile -t replies < <(printf '%s\n' "${inputs[@]}" | "$BIN" 2>/dev/null)

    for i in "${!inputs[@]}"; do
      total=$((total + 1))
      reply="${replies[$i]-}"
      if is_wall "$reply"; then
        walls=$((walls + 1))
        fails+=("${inputs[$i]}")
      fi
    done
  done

  if [ "$total" -eq 0 ]; then
    printf 'EMPTY  %-22s (no real turns)            -> delete\n' "$name"
    [ "$APPLY" -eq 1 ] && rm -f "$log"
    deleted=$((deleted + 1))
  elif [ "$walls" -eq 0 ]; then
    printf 'CLEAN  %-22s (%2d inputs, 0 walls)        -> delete\n' "$name" "$total"
    [ "$APPLY" -eq 1 ] && rm -f "$log"
    deleted=$((deleted + 1))
  else
    printf 'KEEP   %-22s (%2d inputs, %2d walls)\n' "$name" "$total" "$walls"
    for f in "${fails[@]}"; do
      printf '         wall <- %s\n' "$f"
      gap_count["$f"]=$(( ${gap_count["$f"]:-0} + 1 ))
    done
    kept=$((kept + 1))
  fi
done

echo "---"
if [ "$APPLY" -eq 1 ]; then
  echo "deleted $deleted log(s), kept $kept."
else
  echo "(dry run) would delete $deleted log(s), keep $kept."
fi
exit 0
