#!/usr/bin/env bash
#
# mimic-llm discovery harness (gen226, primo giro). See docs/plans/mimic-llm.md.
#
# This is PURE BEHAVIORAL STYLING, not knowledge or reasoning. It does two things,
# both offline (no network, no opencode):
#
#  (A) CATALOGA la reazione di parrot0 a SONDE MINIME/CRIPTICHE — una lettera, un
#      numero, una parola strana, pura punteggiatura — nello spirito del sym-bench
#      ([[oracle-is-behavioural-signal]]): stimoli aperti che mostrano *come*
#      reagisce, non se "risolve". E' la baseline comportamentale che una futura
#      suite simchat confronterebbe con un modello-maestro.
#
#  (B) MOSTRA il meccanismo dello stiling: con un profilo di stile che fissa
#      style_temperature(0), la selezione fra FORME alternative diventa argmax
#      (forma canonica, deciso/conciso); senza profilo, la rotazione anti-ripetizione
#      gen55 alterna le forme. Stesso CONTENUTO, voce diversa.
#
# Like the other discovery benches it NEVER fails the build: it prints, it does
# not gate. The teacher-comparison step is future and gated by the disamina's
# paletto (confine stile/contenuto).
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
PROFILE="$ROOT/kb/profiles/llm/deepseek-v4-flash.p0"

: "${PARROT0_BASE:=$ROOT/kb/core/base.p0}"
export PARROT0_BASE
export PARROT0_SESSION=""

if [ ! -x "$BIN" ]; then echo "mimic: binary not built ($BIN)" >&2; exit 1; fi

ask() {  # ask <profile-or-empty> <prompt>
    local prof="$1"; shift
    if [ -n "$prof" ]; then
        PARROT0_PROFILE="$prof" printf '%s\n' "$1" | PARROT0_PROFILE="$prof" "$BIN" 2>/dev/null | head -1
    else
        printf '%s\n' "$1" | "$BIN" 2>/dev/null | head -1
    fi
}

echo "=== (A) catalogo reazioni a sonde minime/criptiche ==="
for p in "a" "z" "7" "42" "qwerty" "asdfghjkl" "?" "!@#" "xyzzy" "blorp"; do
    printf '  %-10s -> %s\n' "$p" "$(ask "" "$p")"
done

echo
echo "=== (B) temperatura sullo stile: stessa forma o rotazione? ==="
echo "-- senza profilo (rotazione anti-ripetizione gen55): due 'congratulations' di fila --"
printf 'congratulations\ncongratulations\n' | "$BIN" 2>/dev/null | head -2 | sed 's/^/  /'
echo "-- con profilo deepseek-v4-flash, style_temperature(0) (argmax, forma canonica) --"
printf 'congratulations\ncongratulations\n' | PARROT0_PROFILE="$PROFILE" "$BIN" 2>/dev/null | head -2 | sed 's/^/  /'

echo
echo "mimic: primo giro — meccanismo di stiling attivo (selezione della FORMA);"
echo "       cataloguing via sonde minime pronto; teacher-comparison: futuro."
