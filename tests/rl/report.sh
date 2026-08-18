#!/usr/bin/env bash
#
# report.sh — la matrice, letta.
#
# Il totale di `make rl-bench` e' il numero meno interessante della batteria.
# Quello che serve a decidere sono i TAGLI: per famiglia, per macro-area, e
# soprattutto per DIMENSIONE del vettore di complessita' — perche' e' li' che si
# vede se i falliti sono tanti problemi o uno solo travestito.
#
# Legge il manifest (cos'e' ogni episodio) e l'ultimo ledger (com'e' andato), e
# li unisce per id. Nessuno stato proprio: se i due file discordano, si vede.
set -u
cd "$(dirname "$0")/.." || exit 1
MAN=rl/manifest.tsv
LED=$(ls -1 rl/ledger/*.tsv 2>/dev/null | sort | tail -1)
[ -f "$MAN" ] && [ -n "$LED" ] || { echo "report: manifest o ledger mancanti" >&2; exit 1; }

echo "batteria di rinforzo — $(basename "$LED" .tsv)"
echo

join_tsv() {
  awk -F'\t' '
    NR==FNR { if ($0 ~ /^#/ || $1=="id") next; fam[$1]=$2; area[$1]=$3; grad[$1]=$5; vet[$1]=$6; next }
    { if ($0 ~ /^#/ || $1=="generazione") next
      id=$2; if (!(id in fam)) next
      print id "\t" fam[id] "\t" area[id] "\t" grad[id] "\t" vet[id] "\t" $3 "\t" $4 }
  ' "$MAN" "$LED"
}

cut_by() { # $1 = campo su cui raggruppare, $2 = titolo
  echo "── $2"
  join_tsv | awk -F'\t' -v f="$1" '
    { k=$f; p[k]+=$6; q[k]+=$7; n[k]++ }
    END { for (k in p) printf "  %-22s %3d/%-3d  episodi %2d  %s\n", k, p[k], p[k]+q[k], n[k], (q[k]==0 ? "VERDE" : "") }
  ' | sort
  echo
}

cut_by 2 "per famiglia"
cut_by 3 "per macro-area"
cut_by 4 "per gradino"

# Il taglio che conta: ogni dimensione del vettore, al suo livello.
echo "── per dimensione del vettore (livello: passati/totali)"
join_tsv | awk -F'\t' '
  BEGIN { split("profondita distanza ambiguita composizione rumore apertura novita", nome, " ") }
  { split($5, v, "-")
    for (i=1; i<=7; i++) { k=nome[i] "=" v[i]; p[k]+=$6; q[k]+=$7 } }
  END { for (k in p) printf "  %-18s %3d/%-3d\n", k, p[k], p[k]+q[k] }
' | sort
echo
echo "── totale"
join_tsv | awk -F'\t' '{p+=$6; q+=$7; n++} END {printf "  %d episodi, %d passati, %d falliti\n", n, p, q}'
