#!/bin/bash
# prose-session.sh — LA RIPRESA IN UN COMANDO (18 settembre 2026, missione
# secondaria del piano lettura-della-prosa.md §4-quater, gradino 1).
#
# Quello che ogni sessione rifaceva a mano prima della prima misura, e che il
# 18 settembre e' costato 40 minuti: ricostruire lo stato dall'ultimo handoff,
# ricompilare, riavviare il demone, rilanciare l'ultimo piolo certificato. Qui
# e' un comando, e il gate della missione e': una sessione nuova arriva alla
# prima misura in 5 minuti di parete.
#
# Uso:  scripts/prose-session.sh                 # stato + build + demone + rimisura r300 r340 in background
#       scripts/prose-session.sh r300 r320 it:i100   # i pioli da rimisurare (in parallelo, in background)
#       P0_SESSION_NOBENCH=1 scripts/prose-session.sh   # solo stato, build e demone
set -u
cd "$(dirname "$0")/.." || exit 1
PLAN=docs/plans/lettura-della-prosa.md
LAB=docs/labs/prose-ladder
RUNGS=("$@"); [ ${#RUNGS[@]} -gt 0 ] || RUNGS=(r300 r340)

echo "═══ 1. DOVE SIAMO (git, handoff, ultimi referti, furti) ═══"
git log --oneline -3 | cut -c1-120
echo
echo "--- handoff del piano ($PLAN): la tabella «dove siamo»"
awk '/^## ⛔ HANDOFF/{p=1} p&&/^\| piolo/{t=1} t&&/^\|/{print "  "$0} t&&!/^\|/{exit}' "$PLAN"
echo
echo "--- ultimo giro del registro (§6)"
awk '/^## 6\. Registro/{p=1;next} p&&/^### /{print "  "$0; exit}' "$PLAN"
echo
echo "--- ultimi referti ($LAB/referti)"
ls -t "$LAB/referti" 2>/dev/null | head -6 | sed 's/^/  /'
echo
echo "--- ultimi furti ($LAB/furti.tsv)"
[ -f "$LAB/furti.tsv" ] && tail -n 8 "$LAB/furti.tsv" | cut -c1-140 | sed 's/^/  /'
echo
echo "═══ 2. BINARIO FRESCO E DEMONE ═══"
make build 2>&1 | grep -E "error|warning" ; make test-engine 2>&1 | tail -1
[ "${P0_SESSION_NOBENCH:-0}" = 1 ] && exit 0
echo
echo "═══ 3. RIMISURA IN BACKGROUND: ${RUNGS[*]} (misura prima di toccare — mossa M1) ═══"
LOG="$LAB/referti/session-$(date +%Y-%m-%d-%H%M).log"
nohup scripts/prose-rung.sh "${RUNGS[@]}" > "$LOG" 2>&1 &
echo "  log: $LOG   (tail -f per seguirlo; ~10-20 min per piolo, in parallelo)"
echo "  intanto: la sonda per frase (mossa M3) e «who answered?» (M2), mai il C prima di M2"
