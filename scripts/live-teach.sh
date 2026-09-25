#!/usr/bin/env bash
# live-teach.sh — una sessione di addestramento VIVA (docs/plans/live-teaching.md).
#
# Un solo processo parrot0, la KB viva completa, aperto dentro tmux. L'insegnante
# (l'agente) gli parla con `say`, annota il ragionamento con `think`; F. guarda
# tutto dal vivo con `watch` (file o socket). Niente motore di test, niente
# processi paralleli, niente rebuild: si impara parlando.
#
#   scripts/live-teach.sh start [TITOLO]   avvia parrot0 in tmux e il canale per chi guarda
#   scripts/live-teach.sh say "frase"      manda una riga a parrot0 e aspetta la risposta
#   scripts/live-teach.sh think "nota"     il ragionamento dell'insegnante, nel transcript
#   scripts/live-teach.sh watch            (per F.) segue il transcript dal vivo
#   scripts/live-teach.sh steer "nota"     (per F.) un indirizzo per l'insegnante, dal vivo
#   scripts/live-teach.sh stop             /save, archivia il transcript, chiude
#
# Per F.: `scripts/live-teach.sh watch`, oppure `socat - UNIX-CONNECT:var/live/watch.sock`
# (o `nc -U var/live/watch.sock`), oppure `tmux attach -t parrot0-live` (sola lettura: -r).
set -euo pipefail
cd "$(dirname "$0")/.."

LIVE=var/live
IN=$LIVE/in.txt
RAW=$LIVE/parrot0.out
LOG=$LIVE/transcript.log
SOCK=$LIVE/watch.sock
STEER=$LIVE/steer.txt          # le note di F. non ancora lette dall'insegnante
DEBUG_ON=$LIVE/debug_on        # non vuoto mentre il profilo di /debug e' acceso
SESSION=parrot0-live
WAIT=${LIVE_TEACH_WAIT:-180}   # secondi massimi per una risposta

# il formato del transcript (F., 25 settembre 2026):
#   "> " il prompt dell'insegnante   "< " la risposta di parrot0
#   "! " il ragionamento che guida il prompt successivo
#   "F: " un indirizzo di F.         "# " la sessione e il sistema
note() { printf '%s\n' "$*" >> "$LOG"; }

# la risposta e' completa quando parrot0 torna al prompt: il file finisce con ">>> "
wait_reply() {
    local before=$1 t=0
    while :; do
        local size; size=$(stat -c %s "$RAW")
        if [ "$size" -gt "$before" ] && [ "$(tail -c 4 "$RAW")" = ">>> " ]; then return 0; fi
        sleep 0.3; t=$((t + 1))
        if [ $t -gt $((WAIT * 3)) ]; then note "# nessuna risposta entro ${WAIT}s"; return 1; fi
    done
}

case "${1:-}" in
start)
    title=${2:-sessione}
    mkdir -p "$LIVE"
    tmux has-session -t "$SESSION" 2>/dev/null && { echo "gia' attiva: tmux attach -r -t $SESSION"; exit 1; }
    : > "$IN"; : > "$RAW"; : > "$LOG"; : > "$STEER"; : > "$DEBUG_ON"
    note "# sessione: $title — $(date '+%Y-%m-%d %H:%M') — KB viva, profilo agi, un solo processo"
    # parrot0: legge le righe che l'insegnante aggiunge, risponde riga per riga
    tmux new-session -d -s "$SESSION" -n parrot0 \
        "tail -n +1 -f $IN | PARROT0_PROFILE=kb/profiles/agi.p0 PARROT0_LANG=\${PARROT0_LANG:-en} stdbuf -oL ./bin/parrot0 > $RAW 2>&1"
    # le risposte entrano nel transcript da `say`, che sa quali tenere per se' (i /debug)
    # il canale per chi guarda: ogni connessione riceve il transcript dall'inizio e poi dal vivo
    rm -f "$SOCK"
    tmux new-window -d -t "$SESSION" -n watch \
        "socat UNIX-LISTEN:$SOCK,fork SYSTEM:'tail -n +1 -f $LOG'"
    tmux new-window -d -t "$SESSION" -n transcript "tail -n +1 -f $LOG"
    printf 'avvio (il boot della KB completa richiede ~20 s)...\n'
    wait_reply 0 && { sed -e 's/^\(>>> \)*//' -e '/^$/d' -e 's/^/# /' "$RAW" >> "$LOG"; note "# parrot0 pronto"; }
    echo "pronto. F.: scripts/live-teach.sh watch  |  socat - UNIX-CONNECT:$SOCK  |  tmux attach -r -t $SESSION"
    ;;
say)
    shift; line="$*"
    [ -n "$line" ] || { echo "say: frase vuota"; exit 1; }
    before=$(stat -c %s "$RAW")
    note "> $line"
    printf '%s\n' "$line" >> "$IN"
    wait_reply "$before" || true
    # la risposta, per l'insegnante: cio' che parrot0 ha scritto dopo la domanda
    reply=$(tail -c +$((before + 1)) "$RAW" | sed -e 's/^\(>>> \)*//' -e '/^$/d')
    printf '%s\n' "$reply"
    # per chi guarda: i dump di /debug sono per l'insegnante, non per il transcript (F., 26 settembre);
    # con il profilo acceso restano fuori anche le righe rientrate che lo accompagnano
    case "$line" in
    "/debug off"*) : > "$DEBUG_ON"; note "# /debug spento" ;;
    /debug*) echo on > "$DEBUG_ON"
             note "# /debug: $(printf '%s\n' "$reply" | wc -l) righe lette dall'insegnante, fuori dal transcript" ;;
    *) if [ -s "$DEBUG_ON" ]; then printf '%s\n' "$reply" | grep -v -e '^[[:space:]]' -e '^\[debug\]' | sed 's/^/< /' >> "$LOG"
       else printf '%s\n' "$reply" | sed 's/^/< /' >> "$LOG"; fi ;;
    esac
    # e le note di F. arrivate nel frattempo: l'insegnante le legge prima del prossimo passo
    if [ -s "$STEER" ]; then
        echo "--- indirizzo di F. ---"; cat "$STEER"; : > "$STEER"
    fi
    ;;
steer)
    shift; note "F: $*"; printf '%s\n' "$*" >> "$STEER"
    ;;
think)
    shift; note "! $*"
    ;;
watch)
    exec tail -n +1 -f "$LOG"
    ;;
stop)
    if tmux has-session -t "$SESSION" 2>/dev/null; then
        before=$(stat -c %s "$RAW")
        note "> /save"
        printf '/save\n' >> "$IN"
        wait_reply "$before" || true
        sleep 1
        note "# sessione chiusa $(date +%H:%M)"
        mkdir -p docs/sessions/live
        out=docs/sessions/live/$(date +%Y-%m-%d-%H%M).log
        cp "$LOG" "$out"
        echo "transcript archiviato: $out"
        tmux kill-session -t "$SESSION"
    fi
    rm -f "$SOCK"
    ;;
*)
    sed -n '2,17p' "$0"; exit 1 ;;
esac
