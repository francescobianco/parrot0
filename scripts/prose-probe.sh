#!/bin/bash
# prose-probe.sh — LEGGO UNA PROSA CHE NON CONOSCO, POI RISPONDO SU QUELLA.
# (gen513, chiesto da F.: «vorrei allenare il lettore di prosa rendendolo
#  potentissimo; dammi un esperimento semplice che mi mostra un limite attuale»)
#
# ── PERCHE' E' FATTO COSI' ──────────────────────────────────────────────────
#
# La prosa deve essere ESTERNA alla KB, altrimenti una risposta giusta non prova
# niente: potrebbe venire da cio' che parrot0 sapeva gia'. I testi in
# `tests/fixtures/prose/` sono lead veri di Wikipedia su argomenti che la KB non
# contiene (vedi SOURCES.md), e le domande hanno la risposta SCRITTA NEL TESTO.
#
# Il banco fa due cose, e la prima e' quella che insegna di piu':
#
#   PASSO 1 — ogni frase, da sola, in una sessione pulita. Non «ha risposto?»
#             ma «CHE COSA ne ha capito?». E' qui che si vede la specie del
#             limite: una frase letta male e' peggio di una non letta, perche'
#             mette in KB un fatto storto e non si lamenta.
#   PASSO 2 — tutta la prosa in un turno, poi le domande. La colonna a destra
#             dice se la risposta contiene cio' che il testo dice.
#
# Il numero da guardare e' l'ultimo: quante domande, la cui risposta E' nel
# testo, restano senza risposta. Deve scendere man mano che il lettore cresce.
#
# Uso:  ./scripts/prose-probe.sh tardigrade
#       ./scripts/prose-probe.sh quipu
#       ./scripts/prose-probe.sh tests/fixtures/prose/mio.txt
#       make prose-probe                 (tutti i testi presenti)
set -u
cd "$(dirname "$0")/.." || exit 1

NAME="${1:-tardigrade}"
TXT="$NAME"; [ -f "$TXT" ] || TXT="tests/fixtures/prose/$NAME.txt"
[ -f "$TXT" ] || { echo "prose-probe: non trovo «$TXT»."; exit 1; }
QF="${TXT%.txt}.q"

run() {  # una sessione pulita, le righe passate come argomenti; una risposta per riga
  # ⚠ fuori da un terminale parrot0 stampa le risposte su stdout e il prompt
  # «>>> » su stderr: si uniscono, e il marcatore e' quello che separa un turno
  # dal successivo. (Una risposta su piu' righe — un blocco di codice — perde le
  # righe di continuazione: qui le risposte sono verbali, quindi a una riga.)
  printf '%s\n' "$@" '/quit' | \
    PARROT0_SESSION= PARROT0_WIKI_FETCH=0 PARROT0_TOOLS=1 PARROT0_LANG="${P0LANG:-en}" \
    PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0 2>&1 | \
    grep '>>>' | sed 's/^.*>>> //'
}
cut_to() { cut -c1-"${1:-104}"; }

echo
echo "═══ PROSA: $TXT ═══"
sed 's/^/    /' "$TXT"

# ── PASSO 1 — che cosa capisce di ogni frase, presa da sola ─────────────────
echo
echo "─── PASSO 1 · una frase per volta, sessione pulita: CHE COSA NE CAPISCE ───"
python3 - "$TXT" <<'PY' > /tmp/.pp_sents.$$
import sys, re
t = open(sys.argv[1]).read().strip()
for s in re.split(r'(?<=[.!?])\s+', t):
    if s.strip(): print(s.strip())
PY
i=0
while IFS= read -r s; do
  i=$((i+1))
  printf '\n  [%d] %s\n' "$i" "$(printf '%s' "$s" | cut_to 100)"
  printf '   →  %s\n' "$(run "$s" | head -1 | cut_to 100)"
done < /tmp/.pp_sents.$$
rm -f /tmp/.pp_sents.$$

# ── PASSO 2 — legge tutto, poi risponde ────────────────────────────────────
[ -f "$QF" ] || { echo; echo "(nessun file di domande «$QF»: mi fermo al passo 1)"; exit 0; }
echo
echo "─── PASSO 2 · legge tutta la prosa, poi risponde. La risposta E' nel testo ───"
PROSE=$(cat "$TXT")
mapfile -t QS < <(cut -f1 "$QF")
mapfile -t AS < <(cut -f2 "$QF")
mapfile -t REPLIES < <(run "$PROSE" "${QS[@]}" | tail -n +2)

ok=0; n=0
printf '\n  %-40s %-8s %s\n' "DOMANDA" "ESITO" "RISPOSTA"
printf '  %s\n' "────────────────────────────────────────────────────────────────────────────────"
for idx in "${!QS[@]}"; do
  n=$((n+1))
  q="${QS[$idx]}"; want="${AS[$idx]}"; got="${REPLIES[$idx]:-}"
  if printf '%s' "$got" | grep -qi -- "$want"; then verdict="✓"; ok=$((ok+1)); else verdict="·"; fi
  printf '  %-40s %-8s %s\n' "$(printf '%s' "$q" | cut -c1-38)" "$verdict" "$(printf '%s' "$got" | cut_to 72)"
done
printf '  %s\n' "────────────────────────────────────────────────────────────────────────────────"
printf '\n  %d domande su %d hanno ricevuto quello che il testo dice.\n' "$ok" "$n"
printf '  ⛔ %d restano senza: la risposta E'"'"' nel testo, e il lettore non la porta.\n\n' "$((n-ok))"
