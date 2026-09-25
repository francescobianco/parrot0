#!/bin/sh
# L3/I5 — uno strumento del contatto appreso sopravvive al riavvio, e cosi' la sua
# correzione. Solo `make chat` e lingua naturale; COPIA COMPLETA del repo (le
# parole di controllo non entrano nella KB curata). Stampa osservazioni.
# Uso: sh docs/labs/l3/I5/persistenza.sh   (dopo `make build`)
set -e
ROOT=$(cd "$(dirname "$0")/../../../.." && pwd)
WORK=$(mktemp -d "${TMPDIR:-/tmp}/parrot0-l3-i5-XXXXXX")
rsync -a --exclude .git "$ROOT/" "$WORK/"
echo "Copia completa: $WORK"
chat() {
    echo "--- $1 ---"; shift
    printf '%s\n' "$@" '/quit' | (cd "$WORK" && env -u PARROT0_SESSION \
        PARROT0_LANG=en PARROT0_WIKI_FETCH=0 make -s chat 2>&1) \
        | grep -v '^say something' | grep -E '^>>>|PARSE ERROR' || true
}
chat learn \
    'Einstein was born in Ulm; in other words, his Geburtsort is Ulm.' \
    'Steel is made of iron; in other words, its Werkstoff is iron.' \
    'Rome is in Italy; moreover, Rome is its capital.' \
    'Galileo Galilei was born in Pisa; moreover, Pisa is his university.' \
    'Milan is in Italy, but Milan is not its capital.' \
    'Einstein was born in Ulm, but Ulm is not his university.' '/save'
chat reopened \
    'France borders Spain; in other words, its Nachbar is Spain.' \
    'What is the Nachbar of Italy?' \
    'Napoleon was born in Ajaccio; moreover, his Heimatort is Ajaccio.' \
    'What is the Heimatort of Galileo Galilei?' \
    'What is the Geburtsort of Napoleon?'
echo "--- diff della KB nella copia ---"
diff "$ROOT/kb/learning/learned.p0" "$WORK/kb/learning/learned.p0" | grep -E 'instrument|via|near' || true
