#!/bin/bash
# fenomeni.sh — LE SPECIE DEI DIFETTI, CERCATE TUTTE IN UN COLPO
# (20 settembre 2026, chiesto da F. partendo dal caso «2.7 al quadrato»).
#
# Un difetto trovato quando appare si chiude uno alla volta, e le altre istanze
# della STESSA specie restano vive e invisibili. Qui si fa il contrario: si
# nomina la specie (kb/core/fenomenologia.p0), si scrive il suo rilevatore, e
# lo si passa su tutto il corpus di prosa reale.
#
#   scripts/fenomeni.sh flussi    dove due flussi di token divergono
#   scripts/fenomeni.sh furti     quale facolta' risponde a un'ASSERZIONE
#   scripts/fenomeni.sh           tutte e due
#
# Il corpus e' `tests/fixtures/prose/ladder/*.txt`: prosa vera, esterna alla KB.
# Le righe si leggono a mano — un disaccordo non e' per forza un difetto, e una
# facolta' che risponde a un'asserzione puo' averne diritto. Il rilevatore
# DICHIARA, non giudica.
set -u
cd "$(dirname "$0")/.." || exit 1
WHAT="${1:-tutto}"
MAX_TEXTS="${P0_FEN_TEXTS:-12}"
MAX_SENT="${P0_FEN_SENT:-8}"
OUT=$(mktemp -d); trap 'rm -rf -- "$OUT"' EXIT

feed() {  # $1 = file di testo; stampa le righe di /debug richieste
  python3 - "$1" "$MAX_SENT" <<'PY' > "$OUT/turns"
import sys, re
txt = open(sys.argv[1], encoding='utf-8').read()
sents = [s.strip() for s in re.split(r'(?<=[.!?])\s+', txt) if len(s.strip()) > 20]
for s in sents[:int(sys.argv[2])]:
    print(s)
    print('/debug')
PY
  printf '%s\n' "$(cat "$OUT/turns")" '/quit' | \
    PARROT0_SESSION= PARROT0_WIKI_FETCH=0 PARROT0_LANG=en \
    PARROT0_PROFILE=kb/profiles/agi.p0 timeout 600 ./bin/parrot0 2>&1
}

# i pioli lunghi per primi: la prosa densa e' dove le specie si vedono
texts=$(ls -S tests/fixtures/prose/ladder/*.txt | head -"$MAX_TEXTS")

if [ "$WHAT" = flussi ] || [ "$WHAT" = tutto ]; then
  echo "═══ specie: due strade che divergono (flussi di token)"
  : > "$OUT/flussi"
  for t in $texts; do
    feed "$t" | grep -o "at([0-9]*, span([^)]*), ir([^)]*))" >> "$OUT/flussi"
  done
  if [ -s "$OUT/flussi" ]; then
    sed -E 's/at\([0-9]+, //' "$OUT/flussi" | sort | uniq -c | sort -rn | head -30
    echo "  ── $(wc -l < "$OUT/flussi") disaccordi, $(sort -u "$OUT/flussi" | wc -l) distinti"
  else
    echo "  nessun disaccordo sul corpus (non prova che non ce ne siano altrove)"
  fi
fi

if [ "$WHAT" = furti ] || [ "$WHAT" = tutto ]; then
  echo
  echo "═══ specie: furto di turno (chi risponde a un'ASSERZIONE di prosa)"
  : > "$OUT/mod"
  for t in $texts; do
    # la frase e la facolta' che l'ha presa, appaiate: una riga si legge
    feed "$t" | awk '/^  TURNO/ { $1=""; $2=""; s=substr($0,3,78) }
                   /^  modulo/ { print $2 "\t" s }' >> "$OUT/mod"
  done
  cut -f1 "$OUT/mod" | sort | uniq -c | sort -rn
  echo
  echo "  ── le righe delle facolta' che non leggono e non registrano:"
  grep -vE "^(knowledge|fallback|input|reader|discourse|coref)\t" "$OUT/mod" | sort | sed "s/^/     /"
  echo "  ── leggono/registrano di diritto: knowledge, input, reader, discourse, coref."
  echo "     ogni ALTRA facolta' in questa lista e' una candidata al furto."
fi
