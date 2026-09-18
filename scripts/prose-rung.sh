#!/bin/bash
# prose-rung.sh — UN PIOLO, UN COMANDO (18 settembre 2026, missione secondaria:
# efficientare il processo, piano lettura-della-prosa.md §4-quater).
#
# Quello che una sessione rifaceva a mano ogni volta: lanciare il banco con la
# colonna dei moduli, salvare il referto con la data, confrontarlo con l'ultimo
# referto dello stesso piolo, e accodare i furti (non muro, non giusta, con il
# modulo) al registro. Ora e' un comando, e lascia tracce che la sessione dopo
# ritrova senza cercare.
#
# Uso:  scripts/prose-rung.sh r340            # ladder inglese
#       scripts/prose-rung.sh it:i100         # ladder italiana (P0LANG=it)
#       scripts/prose-rung.sh r300 r320 r340  # piu' pioli, IN PARALLELO
# Variabili: P0_PROBE_WHO (default 1), P0_PROBE_STEP2 (default 1: solo passo 2).
# Esce 0; i numeri stanno nel referto e nel diff, non nel codice di uscita.
set -u
cd "$(dirname "$0")/.." || exit 1
LAB=docs/labs/prose-ladder
mkdir -p "$LAB/referti"
LEDGER="$LAB/furti.tsv"
[ -f "$LEDGER" ] || printf 'data\tpiolo\tmodulo\tdomanda\trisposta\n' > "$LEDGER"
STAMP=$(date +%Y-%m-%d-%H%M)

one_rung() {
  local spec="$1" lang=en dir=tests/fixtures/prose/ladder
  case "$spec" in it:*) lang=it; dir=tests/fixtures/prose/ladder-it; spec="${spec#it:}";; esac
  local txt="$dir/$spec.txt"
  [ -f "$txt" ] || { echo "prose-rung: non trovo $txt"; return 1; }
  local out="$LAB/referti/$spec-$STAMP.txt"
  local prev; prev=$(ls -t "$LAB/referti/$spec-"*.txt 2>/dev/null | head -1)
  P0LANG="$lang" P0_PROBE_WHO="${P0_PROBE_WHO:-1}" P0_PROBE_STEP2="${P0_PROBE_STEP2:-1}" \
    ./scripts/prose-probe.sh "$txt" > "$out" 2>&1
  echo "═══ $spec → $out"
  grep -E "^\s+(merito|meta|struttura)\s+[0-9]+/|CANCELLO|furti" "$out"
  if [ -n "$prev" ] && [ "$prev" != "$out" ]; then
    echo "--- diff con $(basename "$prev")"
    ./scripts/prose-diff.py "$prev" "$out" | grep -A40 "GUADAGNATE\|PERSE\|NON muri" | head -60
  fi
  # i furti nel registro: ogni riga «·» con modulo che non e' un muro
  awk -v d="$STAMP" -v r="$spec" '
    /^  .{32} +(merito|meta|struttura) +· +[a-z_?]+ / {
      q=substr($0,3,32); sub(/ +$/,"",q)
      rest=$0; sub(/^.*· +/,"",rest); m=rest; sub(/ .*$/,"",m)   # il modulo: il token dopo il marcatore
      ans=rest; sub(/^[a-z_?]+ +/,"",ans)
      if (ans ~ /don.t know|don.t understand|not sure|can.t show|I have no|looked up|couldn.t|beyond me|Ask me whether|didn.t quite|I understood|non so|non capisco|non ho|non riesco/) next
      printf "%s\t%s\t%s\t%s\t%s\n", d, r, m, q, substr(ans,1,80)
    }' "$out" >> "$LEDGER"
  echo "--- registro furti: $LEDGER ($(tail -n +2 "$LEDGER" | wc -l) righe)"
}

if [ $# -gt 1 ]; then
  for s in "$@"; do one_rung "$s" & done; wait
else
  one_rung "${1:?piolo}"
fi
