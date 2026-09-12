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
BUDGET="${2:-0}"          # 0 = tutto il testo; N = i primi N token (al confine di frase)
TXT="$NAME"; [ -f "$TXT" ] || TXT="tests/fixtures/prose/$NAME.txt"
[ -f "$TXT" ] || { echo "prose-probe: non trovo «$TXT»."; exit 1; }
QF="${TXT%.txt}.q"

# ── IL PIOLO DELLA SCALA (gen513) ───────────────────────────────────────────
# Con un budget si prende il PREFISSO del testo che non supera N parole,
# tagliato a un confine di frase — mai a meta' di una proposizione, che sarebbe
# misurare la comprensione di un frammento. Le domande hanno un terzo campo: il
# numero di parole a cui la loro risposta compare. A un piolo si chiedono solo
# quelle gia' rispondibili: chiedere il resto misurerebbe l'indovinare.
CUT="$TXT"
if [ "$BUDGET" -gt 0 ]; then
  CUT=$(mktemp); trap 'rm -f "$CUT"' EXIT
  python3 - "$TXT" "$BUDGET" > "$CUT" <<'PYCUT'
import sys, re
t = open(sys.argv[1]).read().strip(); n = int(sys.argv[2])
out, cum = [], 0
for s in re.split(r'(?<=[.;]) ', t):
    w = len(s.split())
    if cum + w > n and out: break
    out.append(s); cum += w
print(' '.join(out))
PYCUT
fi
WORDS=$(wc -w < "$CUT")

# ⛔ LA KB E' QUELLA VIVA, INTERA (F., 12 settembre 2026): «non accettiamo piu'
# che si lavori con KB sintetica — la KB va usata per com'e'; l'abilita' di
# lettura della prosa ha a che fare con lo STATO della KB, e senza una KB viva
# non si puo' leggere e comprendere la prosa».
# Qui si carica `kb/profiles/agi.p0` come `make chat`, senza nessuna amputazione
# e senza nessun contesto ermetico. L'unica cosa azzerata fra una sonda e
# l'altra e' la SESSIONE — cioe' quello che vede un interlocutore nuovo — mentre
# tutto cio' che una lezione ha messo in KB con `/save` resta, ed e' proprio il
# motivo per cui il banco migliora quando la KB cresce.
run() {  # sessione nuova, KB viva; le righe come argomenti, una risposta per riga
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
echo "═══ PROSA: $TXT — $WORDS parole${BUDGET:+ (piolo $BUDGET)} ═══"
fold -s -w 96 "$CUT" | sed 's/^/    /' 

# ── PASSO 1 — che cosa capisce di ogni frase, presa da sola ─────────────────
echo
echo "─── PASSO 1 · una frase per volta, sessione pulita: CHE COSA NE CAPISCE ───"
python3 - "$CUT" <<'PY' > /tmp/.pp_sents.$$
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
PROSE=$(cat "$CUT")
# solo le domande la cui risposta e' gia' dentro il prefisso
mapfile -t QS < <(awk -F'\t' -v w="$WORDS" '($3==""||$3+0<=w){print $1}' "$QF")
mapfile -t AS < <(awk -F'\t' -v w="$WORDS" '($3==""||$3+0<=w){print $2}' "$QF")
[ "${#QS[@]}" -gt 0 ] || { echo; echo "(nessuna domanda rispondibile entro $WORDS parole)"; exit 0; }
mapfile -t REPLIES < <(run "$PROSE" "${QS[@]}" | tail -n +2)

ok=0; n=0
printf '\n  %-40s %-8s %s\n' "DOMANDA" "ESITO" "RISPOSTA"
printf '  %s\n' "────────────────────────────────────────────────────────────────────────────────"
for idx in "${!QS[@]}"; do
  n=$((n+1))
  q="${QS[$idx]}"; want="${AS[$idx]}"; got="${REPLIES[$idx]:-}"
  # il campo atteso puo' portare piu' risposte VERE separate da «|»: «What is
  # obsidian?» ha due risposte giuste nel testo, e accettarne una sola
  # misurerebbe quale frase e' stata letta, non se la domanda ha avuto risposta.
  if printf '%s' "$got" | grep -qiE -- "$want"; then verdict="✓"; ok=$((ok+1)); else verdict="·"; fi
  printf '  %-40s %-8s %s\n' "$(printf '%s' "$q" | cut -c1-38)" "$verdict" "$(printf '%s' "$got" | cut_to 72)"
done
printf '  %s\n' "────────────────────────────────────────────────────────────────────────────────"
printf '\n  %d domande su %d hanno ricevuto quello che il testo dice.\n' "$ok" "$n"
printf '  ⛔ %d restano senza: la risposta E'"'"' nel testo, e il lettore non la porta.\n\n' "$((n-ok))"
