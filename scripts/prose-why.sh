#!/bin/bash
# prose-why.sh — DAL BANCO ROSSO AL TURNO ISPEZIONATO, IN UN COMANDO
# (20 settembre 2026, da una domanda di F.: «come mai il dump della IR e
#  /debug non ci permettono di trovare il problema?»)
#
# Perche' no: `/debug` ispeziona UN turno, e una domanda del banco non e' un
# turno isolato — arriva dopo che sedici frasi sono state lette, e lo stato che
# la fa sbagliare e' quello. Chi rifaceva la domanda da sola ispezionava un
# altro turno e non riproduceva niente. Questo e' il ponte che mancava: la
# stessa prosa, la stessa domanda, e l'ispettore addosso.
#
#   scripts/prose-why.sh r300 "what are coral reefs sensitive to?"
#   scripts/prose-why.sh r300 "..." --dump     # anche il dump della IR
set -u
cd "$(dirname "$0")/.." || exit 1
RUNG="${1:?uso: prose-why.sh PIOLO \"domanda\" [--dump]}"
Q="${2:?serve la domanda}"
DUMP="${3:-}"
DIR=tests/fixtures/prose/ladder
case "$RUNG" in it:*) DIR=tests/fixtures/prose/ladder-it; RUNG="${RUNG#it:}";; esac
TXT="$DIR/$RUNG.txt"
[ -f "$TXT" ] || { echo "prose-why: non trovo $TXT"; exit 1; }

{ cat "$TXT"; printf '%s\n' "$Q" '/debug'; [ "$DUMP" = --dump ] && echo '/debug dump'; echo '/quit'; } | \
  PARROT0_SESSION= PARROT0_WIKI_FETCH=0 PARROT0_LANG="${P0LANG:-en}" \
  PARROT0_PROFILE=kb/profiles/agi.p0 timeout 600 ./bin/parrot0 2>&1 | \
  sed -n '/^>>> /h; /TURNO/,$p' | head -120
