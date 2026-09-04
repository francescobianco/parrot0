#!/usr/bin/env python3
"""Agenti finti per il cricchetto del pilota — nessuna rete, nessun modello.

Esistono per una ragione sola: rendere falsificabili le ipotesi sul PILOTA
(avvio, submit, turni, completamento, blocco) in secondi invece che in minuti,
e senza spendere una sessione di un agente reale.  Ogni modo riproduce una
forma di comportamento che abbiamo davvero osservato sul campo.

    repl            un REPL di riga, come parrot0 in un PTY: prompt `>>>`
    worker          come `repl`, ma il primo turno SCRIVE l'artefatto
    tui             un TUI a schermo alternato che capisce il bracketed paste
    tui_nobracket   lo stesso TUI, che pero' NON lo capisce: il testo entra,
                    l'invio non parte, e lo schermo continua a ridisegnarsi
                    — la forma esatta del sintomo di freebuff (§3-bis, ip. 1)
    silent          non stampa mai niente: `stream_stalled_startup`
    exit            esce subito: `startup_exit`

Il modo si sceglie con ``--mode``; l'artefatto di `worker` con ``--artifact``.
"""

from __future__ import annotations

import argparse
import os
import sys
import time

PROMPT = "\x1b[32m>>>\x1b[0m "
TUI_READY = "Enter a coding task or / for commands"
BRACKET_START = "\x1b[200~"
BRACKET_END = "\x1b[201~"


def out(text: str) -> None:
    sys.stdout.write(text)
    sys.stdout.flush()


def strip_bracket(line: str) -> tuple[str, bool]:
    """Ritorna (testo, era_incollato)."""
    if BRACKET_START in line:
        return line.replace(BRACKET_START, "").replace(BRACKET_END, ""), True
    return line, False


def read_line() -> str | None:
    line = sys.stdin.readline()
    return None if line == "" else line.rstrip("\n").rstrip("\r")


def run_repl(artifact: str | None) -> int:
    out("fake-agent ready\n")
    turn = 0
    while True:
        out(PROMPT)
        line = read_line()
        if line is None or line.strip() in ("/quit", "/exit"):
            return 0
        text, _ = strip_bracket(line)
        turn += 1
        if turn == 1 and artifact:
            with open(artifact, "w", encoding="utf-8") as handle:
                handle.write("/* written by the fake worker */\nint fake(void){return 0;}\n")
            out(f"TASK-RECEIVED {len(text)}B\nwrote {artifact}\n")
        else:
            out(f"TASK-RECEIVED {len(text)}B\nturn {turn} done\n")


def redraw(frame: int, status: str) -> None:
    """Un TUI non aggiunge righe: cancella lo schermo e lo rifa da capo."""
    out(f"\x1b[2J\x1b[H fake-tui frame {frame}\n {status}\n\n {TUI_READY}\n")


def run_tui(understands_bracket: bool) -> int:
    out("\x1b[?1049h")
    frame = 0
    redraw(frame, "idle")
    status = "idle"
    while True:
        line = read_line()
        if line is None:
            break
        if line == "\x03":
            break
        text, pasted = strip_bracket(line)
        if pasted and not understands_bracket:
            # Il testo e' entrato nell'editor ma l'invio non lo consegna: lo
            # schermo si ridisegna e il task non parte mai.  Nessun errore.
            for _ in range(6):
                frame += 1
                redraw(frame, "idle")
                time.sleep(0.05)
            continue
        status = f"TASK-RECEIVED {len(text)}B"
        for _ in range(3):
            frame += 1
            redraw(frame, status)
            time.sleep(0.05)
        redraw(frame, status)
    out("\x1b[?1049l")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--mode", default="repl")
    parser.add_argument("--artifact", default=None)
    parser.add_argument("--cwd", default=None)  # accettato e ignorato, come freebuff
    args = parser.parse_args()
    if args.mode == "silent":
        time.sleep(30)
        return 0
    if args.mode == "exit":
        return 0
    if args.mode == "repl":
        return run_repl(None)
    if args.mode == "worker":
        return run_repl(args.artifact or "artifact.c")
    if args.mode == "tui":
        return run_tui(True)
    if args.mode == "tui_nobracket":
        return run_tui(False)
    print(f"fake-agent: unknown mode {args.mode}", file=sys.stderr)
    return 2


if __name__ == "__main__":
    raise SystemExit(main())
