#!/usr/bin/env python3
"""I CENTO — quanti dei cento prompt di docs/plans/parrot0-100-failures.md
parrot0 risolve DAVVERO.

Una riga per prompt: `numero | prompt | attesa`. L'attesa e' cio' che una
risposta corretta deve CONTENERE (minuscole ignorate), curata perche' non possa
combaciare con nessun ripiego — muro cieco, declino su parola opaca, «looks like
a X problem», template causale o progettuale. Un ripiego che passa la misura la
renderebbe inutile, ed e' esattamente il difetto che questi cento documentano.

Ogni prompt gira in un CERVELLO NUOVO: i cento non sono una conversazione, e il
retest del 2026-08-18 aveva gia' misurato che lo stesso prompt cambia categoria a
seconda di cosa e' successo prima.
"""
import os, subprocess, sys

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
env = dict(os.environ, PARROT0_LANG="en")
rows = []
for line in open("tests/hundred/hundred.qa"):
    if not line.strip():
        continue
    num, _, rest = line.partition(" | ")
    prompt, _, want = rest.rpartition(" | ")
    rows.append((num.strip(), prompt.strip(), want.strip()))

ok, misses = 0, []
for num, prompt, want in rows:
    run = subprocess.run(["./bin/parrot0"], input=(prompt + "\n/quit\n").encode(),
                         capture_output=True, env=env, timeout=180)
    txt = run.stdout.decode("utf-8", "replace")
    body = [l.replace("you> ", "").strip() for l in txt.splitlines()
            if l.strip() and not l.startswith("parrot0 [")
            and not l.startswith("say something")]
    reply = " ".join(body)
    if want.lower() in reply.lower():
        ok += 1
    else:
        misses.append((num, prompt, want, reply[:100]))

if "-v" in sys.argv:
    for num, prompt, want, got in misses:
        print(f"#{num:>3}  want={want!r}\n      {prompt}\n      got: {got}")
print(f"hundred {ok}/{len(rows)}")
