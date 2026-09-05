#!/usr/bin/env python3
"""Generate per-item .p0t probe files from var/probe/src/fXX.txt.

Single-turn families: one prompt per line -> one item.
Multi-turn family (f05): blocks separated by blank lines -> one item per block.
"""
import pathlib, re

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent  # repo root
SRC = ROOT / "var/probe/src"
OUT = ROOT / "var/probe/items"

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
