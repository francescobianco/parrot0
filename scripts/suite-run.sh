#!/usr/bin/env bash
# suite-run.sh — la suite intera, IN ORDINE, su UN demone, SENZA fail-fast.
#
# `make test` e' fail-fast: mostra un rosso e nasconde gli altri. E i file
# dipendono dallo stato dei precedenti (TEST_TODO H.1a: un sweep alfabetico e
# isolato dava il 46% di rossi che il motore non ha). Questo script legge
# l'ordine dal target `test:` del Makefile, avvia un demone DEDICATO (cosi' il
# demone di `make test-engine` resta libero per indagare) e scrive un report
# con una riga per file, verde o rosso che sia.
#
#   scripts/suite-run.sh [REPORT]        (default: docs/reports/suite-run.txt)
#
# Il binario deve essere gia' aggiornato: `make build` prima, sempre (la
# trappola del binario stale vale anche per il demone).
set -u
cd "$(dirname "$0")/.."
REPORT=${1:-docs/reports/suite-run.txt}
BIN=./bin/parrot0
SOCK=obj/suite-engine.sock
PID=obj/suite-engine.pid
LOG=obj/suite-engine.log

test -f "$PID" && kill "$(cat "$PID")" 2>/dev/null
rm -f "$SOCK" "$PID"
PARROT0_TOOLS=1 PARROT0_SESSION= PARROT0_PROFILE=kb/profiles/agi.p0 \
  $BIN --test-engine --sock "$SOCK" >"$LOG" 2>&1 &
echo $! >"$PID"
trap 'kill "$(cat "$PID")" 2>/dev/null; rm -f "$PID" "$SOCK"' EXIT INT TERM
i=0; while [ ! -S "$SOCK" ] && [ $i -lt 300 ]; do sleep 0.1; i=$((i+1)); done
if [ ! -S "$SOCK" ]; then echo "suite-run: no socket at $SOCK; see $LOG" >&2; exit 1; fi

files=$(sed -n '/^test: test-engine/,/^[a-z][a-z-]*:/p' Makefile \
        | grep -o -- '--test tests/[^ ]*' | sed 's/--test //')
n=$(echo "$files" | wc -l)
: >"$REPORT"
echo "# suite-run $(date '+%F %T') — $(git rev-parse --short HEAD) — $n file" >>"$REPORT"
k=0
for f in $files; do
  k=$((k+1))
  start=$(date +%s)
  out=$($BIN --test "$f" --sock "$SOCK" 2>&1 | tail -1)
  el=$(( $(date +%s) - start ))
  printf '%s  [%3d/%d %4ds] %s\n' "$out" "$k" "$n" "$el" "$f" >>"$REPORT"
done
echo "# done $(date '+%F %T')" >>"$REPORT"
grep -c '^ok' "$REPORT" | sed 's/^/ok:   /'
grep -c '^FAIL' "$REPORT" | sed 's/^/FAIL: /'
