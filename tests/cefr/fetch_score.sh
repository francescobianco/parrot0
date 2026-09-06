#!/usr/bin/env bash
# Scarica la porzione SCoRE di CEFR-SP, che NON e' versionata qui.
#
# Perche' non e' nel repository: SCoRE e' CC BY-NC-SA 4.0, e la clausola
# NonCommercial si trasmette a chi riceve il repository. Imporre quel vincolo in
# silenzio a chi clona parrot0 non ci e' sembrato corretto; il bench funziona
# senza. Scaricandola te ne assumi i termini — vedi ATTRIBUTION.md.
set -euo pipefail
here="$(cd "$(dirname "$0")" && pwd)"
base="https://raw.githubusercontent.com/yukiar/CEFR-SP/main/CEFR-SP/SCoRE"
for split in train test dev; do
  curl -fsSL "$base/CEFR-SP_SCoRE_${split}.txt" -o "$here/data/score_${split}.tsv"
  printf '%-22s %s righe\n' "score_${split}.tsv" "$(wc -l < "$here/data/score_${split}.tsv")"
done
echo "SCoRE — CC BY-NC-SA 4.0 — Arase, Uchida, Kajiwara (EMNLP 2022)."
