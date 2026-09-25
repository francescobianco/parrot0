#!/bin/sh
# L3/R3 — un nome di relazione di piu' parole appreso per contatto sopravvive al
# riavvio. Solo `make chat` e lingua naturale; COPIA COMPLETA del repo (le
# parole di controllo non entrano nella KB curata). Stampa osservazioni.
# Uso: sh docs/labs/l3/R3/persistenza.sh   (dopo `make build`)
set -e
ROOT=$(cd "$(dirname "$0")/../../../.." && pwd)
WORK=$(mktemp -d "${TMPDIR:-/tmp}/parrot0-l3-r3-XXXXXX")
rsync -a --exclude .git "$ROOT/" "$WORK/"
echo "Copia completa: $WORK"
chat() {
    echo "--- $1 ---"; shift
    printf '%s\n' "$@" '/quit' | (cd "$WORK" && env -u PARROT0_SESSION \
        PARROT0_LANG=en PARROT0_WIKI_FETCH=0 make -s chat 2>&1) \
        | grep -v '^say something' | grep -E '^>>>|PARSE ERROR' || true
}
chat learn \
    'Einstein was born in Ulm, so his home town is Ulm.' \
    'Steel is made of iron, so its raw material is iron.' '/save'
chat reopened \
    'What is the home town of Galileo Galilei?' \
    'What is the raw material of glass?'
echo "--- diff della KB nella copia ---"
diff "$ROOT/kb/learning/learned.p0" "$WORK/kb/learning/learned.p0" | grep -E 'home_town|raw_material' || true
