#!/bin/bash
# prose-ladder.sh — LA SCALA: 10, 20, 30 … 150 parole di prosa vera.
# (gen513, obiettivo di F.: «ti fermi quando dimostri che parrot0 e' in grado di
#  comprendere una prosa lunga almeno 150 parole; lavora per iterazioni
#  successive, prima 10 parole poi 20 poi 30 fino a 150»)
#
# ⚠ Ogni piolo ha la SUA prosa: intera, coerente, diversa da quella del piolo
# precedente. Nessun troncamento — leggere un frammento e' un problema diverso,
# e si affronta dopo questo traguardo (F., 2026-09-12).
# I testi sono lead veri di Wikipedia: tests/fixtures/prose/ladder/SOURCES.md.
#
# Un piolo e' SUPERATO quando tutte le sue domande — la cui risposta e' scritta
# nel testo — ricevono quello che il testo dice.
#
# Uso:  ./scripts/prose-ladder.sh          (tutti i pioli con le domande scritte)
#       ./scripts/prose-ladder.sh 30       (fino al piolo 30)
set -u
cd "$(dirname "$0")/.." || exit 1
TOP="${1:-150}"
printf '\n  PIOLO  PAROLE  TESTO         DOMANDE  RISPOSTE  ESITO\n'
printf '  ──────────────────────────────────────────────────────────────\n'
pass=0; seen=0
for f in tests/fixtures/prose/ladder/r*.txt; do
  n=$(basename "$f" .txt); n=${n#r}; n=$((10#$n))
  [ "$n" -le "$TOP" ] || continue
  [ -f "${f%.txt}.q" ] || { printf '  %5s  %6s  %-12s  %7s  %8s  %s\n' "$n" "$(wc -w < "$f")" "-" "-" "-" "(domande da scrivere)"; continue; }
  seen=$((seen+1))
  out=$(./scripts/prose-probe.sh "$f" 2>/dev/null)
  words=$(wc -w < "$f")
  line=$(printf '%s' "$out" | grep -o '[0-9]* domande su [0-9]*' | head -1)
  ok=${line%% *}; tot=$(printf '%s' "$line" | awk '{print $4}')
  topic=$(grep -o "r$(printf '%03d' "$n")\.txt.*" tests/fixtures/prose/ladder/SOURCES.md | awk '{print $4}')
  if [ "${ok:-x}" = "${tot:-y}" ]; then v="✅ superato"; pass=$((pass+1)); else v="⛔ $((tot-ok)) senza risposta"; fi
  printf '  %5s  %6s  %-12s  %7s  %8s  %s\n' "$n" "$words" "${topic:--}" "${tot:--}" "${ok:--}" "$v"
done
printf '  ──────────────────────────────────────────────────────────────\n'
printf '  %d pioli su %d superati.\n\n' "$pass" "$seen"
