#!/usr/bin/env bash
# live-teach.sh — una sessione di addestramento VIVA (docs/plans/live-teaching.md).
#
# Un solo processo parrot0, la KB viva completa, aperto dentro tmux. L'insegnante
# (l'agente) gli parla con `say`, annota il ragionamento con `think`; F. guarda
# tutto dal vivo con `watch` (file o socket). Niente motore di test, niente
# processi paralleli, niente rebuild: si impara parlando.
#
#   scripts/live-teach.sh start [TITOLO]   avvia parrot0 in tmux e il canale per chi guarda
#   scripts/live-teach.sh say "frase"      manda una riga a parrot0, aspetta la risposta e la cronometra:
#                                          oltre LIVE_TEACH_SLOW (10 s) e' LENTO, oltre LIVE_TEACH_WAIT (30 s)
#                                          e' BLOCCATO (exit 2) e nessun turno si accoda finche' non risponde
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
WAIT=${LIVE_TEACH_WAIT:-30}    # secondi dopo i quali un turno e' BLOCCATO e `say` ritorna
SLOW=${LIVE_TEACH_SLOW:-10}    # secondi oltre i quali un turno e' LENTO: un difetto, non un'attesa
PENDING=$LIVE/pending          # il turno mandato e non ancora risposto (offset dell'uscita, ora d'invio)

# il formato del transcript (F., 25 settembre 2026):
#   "> " il prompt dell'insegnante   "< " la risposta di parrot0
#   "! " il ragionamento che guida il prompt successivo
#   "F: " un indirizzo di F.         "# " la sessione e il sistema
note() { printf '%s\n' "$*" >> "$LOG"; }

# la risposta e' completa quando parrot0 torna al prompt: il file finisce con ">>> "
replied() { [ "$(stat -c %s "$RAW")" -gt "$1" ] && [ "$(tail -c 4 "$RAW")" = ">>> " ]; }
wait_reply() {
    local before=$1 t=0
    while ! replied "$before"; do
        sleep 0.3; t=$((t + 1))
        if [ $t -gt $((WAIT * 3)) ]; then return 1; fi
    done
}
now_ms() { date +%s%3N; }
# il processo parrot0 della sessione: per dire, di un turno bloccato, se sta calcolando o e' fermo
parrot_cpu() {
    local pid; pid=$(pgrep -f -n "^./bin/parrot0" -P "$(tmux list-panes -t "$SESSION:parrot0" -F '#{pane_pid}' 2>/dev/null | head -1)" 2>/dev/null || true)
    [ -n "$pid" ] && ps -o etime=,time=,stat= -p "$pid" | awk '{print "pid '"$pid"' da " $1 ", CPU " $2 ", stato " $3}'
}

case "${1:-}" in
start)
    title=${2:-sessione}
    mkdir -p "$LIVE"
    tmux has-session -t "$SESSION" 2>/dev/null && { echo "gia' attiva: tmux attach -r -t $SESSION"; exit 1; }
    : > "$IN"; : > "$RAW"; : > "$LOG"; : > "$STEER"; : > "$DEBUG_ON"; : > "$PENDING"
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
    # un turno ancora senza risposta: non se ne accoda un altro a un processo bloccato
    if [ -s "$PENDING" ]; then
        read -r pbefore pt0 pline < "$PENDING"
        if ! replied "$pbefore"; then
            echo "BLOCCATO: parrot0 non ha ancora risposto a «$pline» ($(( ($(now_ms) - pt0) / 1000 )) s; $(parrot_cpu))."
            echo "Non accodo altri turni: annota il difetto e chiudi (stop, o tmux kill-session senza /save)."
            exit 2
        fi
        : > "$PENDING"
    fi
    before=$(stat -c %s "$RAW")
    note "> $line"
    t0=$(now_ms)
    printf '%s %s %s\n' "$before" "$t0" "$line" > "$PENDING"
    printf '%s\n' "$line" >> "$IN"
    if ! wait_reply "$before"; then
        msg="BLOCCATO: nessuna risposta a «$line» entro ${WAIT} s ($(parrot_cpu))"
        note "# $msg"; echo "$msg"; exit 2
    fi
    : > "$PENDING"
    ms=$(( $(now_ms) - t0 ))
    secs=$(awk -v m="$ms" 'BEGIN{printf "%.1f", m/1000}')
    # il tempo di ogni turno e' un dato (live-teaching.md §3 regola 7): nel transcript e per l'insegnante
    if [ "$ms" -gt $((SLOW * 1000)) ]; then tmsg="LENTO: ${secs} s (soglia ${SLOW} s)"; else tmsg="${secs} s"; fi
    echo "[$tmsg]"
    # la risposta, per l'insegnante: cio' che parrot0 ha scritto dopo la domanda
    reply=$(tail -c +$((before + 1)) "$RAW" | sed -e 's/^\(>>> \)*//' -e '/^$/d')
    printf '%s\n' "$reply"
    # per chi guarda: i dump di /debug sono per l'insegnante, non per il transcript (F., 26 settembre);
    # con il profilo acceso restano fuori anche le righe rientrate che lo accompagnano
    case "$line" in
    "/debug off"*) : > "$DEBUG_ON"; note "# /debug spento" ;;
    "/debug on"*) echo on > "$DEBUG_ON"; note "# /debug acceso" ;;
    /debug*) note "# ${line%% *} ${line#/debug }: $(printf '%s\n' "$reply" | wc -l) righe lette dall'insegnante, fuori dal transcript" ;;
    *) if [ -s "$DEBUG_ON" ]; then printf '%s\n' "$reply" | grep -v -e '^[[:space:]]' -e '^\[debug\]' | sed 's/^/< /' >> "$LOG"
       else printf '%s\n' "$reply" | sed 's/^/< /' >> "$LOG"; fi ;;
    esac
    case "$tmsg" in LENTO*) note "# $tmsg" ;; esac   # nel transcript solo i turni lenti (F.)
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
