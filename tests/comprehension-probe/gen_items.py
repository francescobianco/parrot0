#!/usr/bin/env python3
"""Genera gli item .p0t del banco di comprensione da src/fXX.txt.

Famiglie a turno singolo: una riga -> un item. Famiglia multi-turno (f05):
blocchi separati da riga vuota -> un item per blocco. Gli item sono il PANNELLO
CONGELATO: si rigenerano solo se cambia una sorgente, e si committano.

La sentinella `< __NEVER__` serve a farsi stampare la risposta verbatim dal
verificatore .p0t: ogni item e' quindi «rosso per costruzione», e il runner
(probe.py) separa da solo il verdetto («got:» c'e') dal trasporto («cannot
reach engine»).
"""
import pathlib, re

HERE = pathlib.Path(__file__).resolve().parent
SRC = HERE / "src"
OUT = HERE / "items"

def main():
    OUT.mkdir(parents=True, exist_ok=True)
    for src in sorted(SRC.glob("f*.txt")):
        fam = src.stem
        famdir = OUT / fam
        famdir.mkdir(exist_ok=True)
        text = src.read_text(encoding="utf-8")
        if fam == "f05":
            blocks = [b.strip() for b in re.split(r"\n\s*\n", text) if b.strip()]
            items = [(i, [ln.strip() for ln in b.splitlines() if ln.strip()])
                     for i, b in enumerate(blocks, 1)]
        else:
            lines = [ln.rstrip("\n") for ln in text.splitlines()]
            items = [(i, [ln]) for i, ln in enumerate(lines, 1) if ln.strip()]
        for i, turns in items:
            body = [f"[item {i}]", "!reset", "!timeout 0"]
            for t in turns:
                body.append(f"> {t}")
                body.append("< __NEVER__")
            (famdir / f"i{i:02d}.p0t").write_text("\n".join(body) + "\n", encoding="utf-8")
        print(f"{fam}: {len(items)} items -> {famdir}")

if __name__ == "__main__":
    main()
