#!/bin/sh
# L3/I2 — persistenza: impara -> /save -> processo nuovo -> controesempio ->
# /save -> processo nuovo. Solo `make chat` e lingua naturale (LEARN_PROTOCOL,
# §«make chat»): niente MCP, niente !assert. Gira su una COPIA COMPLETA del
# repo (§14.6): il ponte Geburtsort e' la relazione di controllo dei banchi e
# non deve entrare nella KB curata. Stampa osservazioni, non certifica.
# Uso: sh docs/labs/l3/I2/persistenza.sh   (dopo `make build`)
set -e
ROOT=$(cd "$(dirname "$0")/../../../.." && pwd)
WORK=$(mktemp -d "${TMPDIR:-/tmp}/parrot0-l3-persistenza-XXXXXX")
rsync -a --exclude .git "$ROOT/" "$WORK/"
echo "Copia completa: $WORK"
chat() {
    echo "--- $1 ---"; shift
    printf '%s\n' "$@" '/quit' | (cd "$WORK" && env -u PARROT0_SESSION \
        PARROT0_LANG=en PARROT0_WIKI_FETCH=0 make -s chat 2>&1) \
        | grep -v '^say something' | grep -E '^>>>|PARSE ERROR|saved|Saved|salvat' || true
}
chat learn \
    'Einstein was born in Ulm, so his Geburtsort is Ulm.' \
    'What is the Geburtsort of Napoleon?' '/save'
chat withdraw \
    'What is the Geburtsort of Napoleon?' \
    'Marie Curie was born in Warsaw, but Warsaw is not her Geburtsort.' \
    'What is the Geburtsort of Napoleon?' '/save'
chat reopened 'What is the Geburtsort of Napoleon?'
echo "--- diff della KB nella copia ---"
diff -r "$ROOT/kb" "$WORK/kb" | grep -i 'contact\|geburtsort' || true
