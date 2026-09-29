#!/usr/bin/env bash
# live-talk.sh — un modello locale e parrot0 parlano fra loro, e F. guarda.
#
# Sorella di live-teach.sh: la' l'agente insegna, qui un LLM locale (LM Studio)
# CONDUCE una chiacchierata e parrot0 risponde. Serve a vedere come regge una
# conversazione che non ha scritto nessuno: dove si capiscono, dove si perdono.
#
#   scripts/live-talk.sh start [TURNI] [MODELLO] [LINGUA]   avvia (default: 12, liquid/lfm2.5-1.2b, en)
#   scripts/live-talk.sh watch                              segue il transcript dal vivo
#   scripts/live-talk.sh stop                               ferma e archivia in docs/sessions/talk/
#
# Il modello deve essere servito da LM Studio: `lms server start` e `lms load MODELLO`
# (lo script prova a farlo da solo). Nel transcript: "M> " il modello, "P< " parrot0
# con il tempo del turno, "# " la sessione.
set -euo pipefail
cd "$(dirname "$0")/.."

TALK=var/talk
LOG=$TALK/transcript.log
PID=$TALK/pid
LMS=${LMS:-$HOME/.lmstudio/bin/lms}

case "${1:-}" in
start)
    turns=${2:-12}; model=${3:-liquid/lfm2.5-1.2b}; lang=${4:-en}
    mkdir -p "$TALK"
    if [ -s "$PID" ] && kill -0 "$(cat "$PID")" 2>/dev/null; then
        echo "gia' attiva (pid $(cat "$PID")): scripts/live-talk.sh watch | stop"; exit 1
    fi
    if [ -x "$LMS" ]; then
        "$LMS" server start >/dev/null 2>&1 || true
        "$LMS" ps 2>/dev/null | grep -q "^$model " || "$LMS" load "$model" -y >/dev/null 2>&1 || true
    fi
    : > "$LOG"
    printf '# live-talk — %s — %s vs parrot0, %s scambi, %s\n' "$(date '+%Y-%m-%d %H:%M')" "$model" "$turns" "$lang" >> "$LOG"
    nohup .venv/bin/python scripts/live-talk.py --model "$model" --turns "$turns" --lang "$lang" \
        --log "$LOG" > "$TALK/driver.err" 2>&1 &
    echo $! > "$PID"
    echo "avviata. Guarda: scripts/live-talk.sh watch"
    ;;
watch)
    [ -f "$LOG" ] || { echo "nessuna conversazione: scripts/live-talk.sh start"; exit 1; }
    tail -n +1 -f "$LOG"
    ;;
stop)
    [ -s "$PID" ] && kill "$(cat "$PID")" 2>/dev/null || true
    : > "$PID"
    mkdir -p docs/sessions/talk
    out=docs/sessions/talk/$(date +%Y-%m-%d-%H%M).log
    [ -s "$LOG" ] && cp "$LOG" "$out" && echo "archiviato: $out"
    ;;
*)
    sed -n '2,16p' "$0" | sed 's/^# \{0,1\}//'
    exit 1
    ;;
esac
