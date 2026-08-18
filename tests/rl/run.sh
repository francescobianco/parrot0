#!/usr/bin/env bash
#
# run.sh — gira la batteria e SCRIVE il ledger.
#
# I numeri del ledger non si trascrivono a mano: erano gia' stati sbagliati una
# volta (iterazione 4), ed e' il modo piu' silenzioso di corrompere una misura.
# Qui li scrive la corsa, e l'unica cosa che resta umana e' la NOTA — cioe' la
# sola colonna che una macchina non puo' compilare.
set -u
cd "$(dirname "$0")/../.." || exit 1
BIN=./bin/parrot0
GEN=${RL_GEN:-gen412}
OUT=tests/rl/ledger/$GEN.tsv
NOTES=tests/rl/ledger/$GEN.note.tsv

tmp=$(mktemp)
for f in $(find tests/rl/episodes -name '*.p0t' | sort); do
    res=$($BIN --test "$f" 2>&1 | tail -1)
    base=$(basename "$f" .p0t)
    # `ok NAME — N passed`  |  `FAIL NAME — N passed, M failed`
    p=$(echo "$res" | sed -n 's/.*— \([0-9]*\) passed.*/\1/p'); p=${p:-0}
    q=$(echo "$res" | sed -n 's/.*, \([0-9]*\) failed.*/\1/p'); q=${q:-0}
    id=$(grep -P "^[^\t]*/$base\t" tests/rl/manifest.tsv | cut -f1 | head -1)
    [ -n "$id" ] || id="?/$base"
    printf '%s\t%s\t%s\t%s\n' "$GEN" "$id" "$p" "$q" >> "$tmp"
done

{
  echo "# ledger — SCRITTO DALLA CORSA (tests/rl/run.sh), non a mano."
  echo "# Le note stanno in $(basename "$NOTES"): sono l'unica colonna che una macchina"
  echo "# non puo' compilare, e per questo l'unica che si scrive."
  echo "#"
  printf 'generazione\tid\tpassati\tfalliti\n'
  sort -k2 "$tmp"
} > "$OUT"
rm -f "$tmp"

awk -F'\t' '$1!~/^#/ && $1!="generazione" {p+=$3; q+=$4; n++}
     END {printf "batteria: %d episodi, %d passati, %d falliti\n", n, p, q}' "$OUT"
echo "ledger: $OUT"
