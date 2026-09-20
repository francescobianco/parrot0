#!/bin/bash
# coefficiente.sh — DA CHE COSA VENGONO LE RISPOSTE (la scala di F., misurata)
#
# «0 = legge ma non capisce; 100 = risponderebbe a qualsiasi domanda,
#  cognitivamente. Non misura quante domande passano: misura DA CHE COSA
#  vengono le risposte.» (docs/plans/lettura-della-prosa.md §0-bis)
#
# Finche' era un giudizio si scriveva «12-15», «13-16». E' una misura: il
# referto del banco porta gia' la colonna del modulo, e basta contare quante
# risposte GIUSTE vengono da un circuito di lettura e quante da un frasario
# ritrovato per cue.
#
#   scripts/coefficiente.sh                      l'ultimo referto di ogni piolo
#   scripts/coefficiente.sh referti/r300-*.txt   referti scelti
#
# I moduli di LETTURA sono quelli che compongono la risposta dalla IR del testo
# o dal piano del turno; `answerframe` e' il frasario: pretende su una cue e
# prova sulla superficie del valore, non sul frame della lettura. La lista sta
# qui e non in KB apposta: e' un giudizio sul progetto, non conoscenza di
# parrot0, e va discussa a mano quando un modulo cambia natura.
set -u
cd "$(dirname "$0")/.." || exit 1
LETTURA='turn_plan|reader|input|discourse|coref|knowledge'
FRASARIO='answerframe'
files=("$@")
if [ ${#files[@]} -eq 0 ]; then
  mapfile -t files < <(for r in docs/labs/prose-ladder/referti/*-*.txt; do
      echo "${r%-*-*-*-*}"; done | sort -u | while read -r p; do
      ls -t "$p"-*.txt 2>/dev/null | head -1; done)
fi
tot_l=0; tot_f=0
printf '%-10s %8s %8s %8s   %s\n' piolo lettura frasario altro coefficiente
for f in "${files[@]}"; do
  [ -f "$f" ] || continue
  mods=$(grep -oE "✓\s+«?[a-z_]+" "$f" | awk '{print $2}' | tr -d '«')
  l=$(echo "$mods" | grep -cE "^($LETTURA)$")
  fr=$(echo "$mods" | grep -cE "^($FRASARIO)$")
  a=$(echo "$mods" | grep -vcE "^($LETTURA|$FRASARIO)$")
  n=$((l + fr + a)); [ "$n" -gt 0 ] || continue
  tot_l=$((tot_l + l)); tot_f=$((tot_f + fr))
  printf '%-10s %8d %8d %8d   %3d%%\n' "$(basename "$f" | cut -d- -f1)" "$l" "$fr" "$a" $((100 * l / n))
done
tot=$((tot_l + tot_f))
[ "$tot" -gt 0 ] && printf '\n  COEFFICIENTE: %d%% delle risposte giuste viene da una lettura (%d su %d).\n' \
  $((100 * tot_l / tot)) "$tot_l" "$tot"
echo "  Il numero sale spostando il turno sulla lettura, non insegnando parole:"
echo "  una riga di lessico in piu' alza il conteggio e lascia il coefficiente dov'e'."
