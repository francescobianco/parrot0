#!/usr/bin/env python3
"""Riscrive la colonna centrale (la FIRMA) dei file tests/measure/N.qa con la
firma che parrot0 produce adesso.

Serve quando un cambiamento del motore sposta le firme di tutti i turni: la
colonna registra la strada percorsa, e una strada cambiata va REGISTRATA — non
inseguita riga per riga a mano. Il prompt puo' contenere un «|» (a lunghezza 1
c'e' proprio quel byte), quindi si spezza sul primo e sul secondo " | ", mai su
tutti.
"""
import glob, os, re, subprocess, sys

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
env = dict(os.environ, PARROT0_LANG="en")
for path in sorted(glob.glob("tests/measure/[0-9]*.qa"),
                   key=lambda p: int(re.findall(r"(\d+)\.qa$", p)[0])):
    rows = []
    for line in open(path).read().splitlines():
        if not line.strip():
            continue
        q, _, rest = line.partition(" | ")
        _fp, _, want = rest.partition(" | ")
        rows.append((q, want))
    stdin = "\n".join(q for q, _ in rows) + "\n"
    run = subprocess.run(["./bin/parrot0", "--footprint"], input=stdin,
                         capture_output=True, text=True, env=env)
    fps = [l.split("\t")[0] for l in run.stdout.splitlines() if l.strip()]
    if len(fps) != len(rows):
        print(f"{path}: {len(fps)} firme per {len(rows)} righe", file=sys.stderr)
        return_code = 1
        sys.exit(1)
    with open(path, "w") as out:
        for (q, want), fp in zip(rows, fps):
            out.write(f"{q} | {fp} | {want}\n")
    print(f"{path}: {len(rows)} firme riscritte")
