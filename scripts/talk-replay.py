#!/usr/bin/env python3
"""Replay congelato di una conversazione live-talk.

Estrae in ordine i turni `M> ` di un log di scripts/live-talk.py e li rimanda,
con il contesto, a un parrot0 di una radice scelta (KB viva completa, profilo
agi, sessione vuota). Serve a confrontare prima/dopo sugli STESSI ingressi:
una conversazione libera diverge appena cambia una risposta.

    .venv/bin/python scripts/talk-replay.py LOG [--root DIR] [--out FILE]

L'uscita e' un log nello stesso formato (`M>`/`P<` con il tempo), cosi' si
legge e si giudica come le conversazioni libere.
"""
import argparse, importlib.util, os, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(HERE)


def load_parrot(root):
    spec = importlib.util.spec_from_file_location("cb", os.path.join(HERE, "coherence-bench.py"))
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    mod.REPO = root          # il costruttore prende il binario da REPO
    return mod.Parrot


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("log")
    ap.add_argument("--root", default=REPO)
    ap.add_argument("--out")
    ap.add_argument("--timeout", type=float, default=60)
    a = ap.parse_args()
    turns = [l[3:].rstrip("\n") for l in open(a.log, encoding="utf-8") if l.startswith("M> ")]
    root = os.path.abspath(a.root)
    os.environ["PARROT0_SESSION"] = ""
    os.environ["PARROT0_LANG"] = "en"
    Parrot = load_parrot(root)
    out = open(a.out, "w", encoding="utf-8") if a.out else sys.stdout
    print(f"# replay di {a.log} — radice {root}, {len(turns)} turni", file=out, flush=True)
    p = Parrot(root, a.timeout)
    try:
        for t in turns:
            try:
                reply, dt, _ = p.say(t)
            except Exception as e:  # tempo scaduto o uscita: si registra, non si nasconde
                print(f"M> {t}\nP< !! {type(e).__name__}: {e}", file=out, flush=True)
                break
            reply = " ".join(reply.splitlines())
            print(f"M> {t}\nP< {reply}   [{dt:.1f} s]", file=out, flush=True)
    finally:
        p.close()


if __name__ == "__main__":
    main()
