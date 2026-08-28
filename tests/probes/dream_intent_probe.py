#!/usr/bin/env python3
"""dream_intent_probe — la prosa da sola non e' un atto: cosa cambia l'INTENZIONE?

QUESTA SONDA NON E' UN PEZZO DI PARROT0. Come le altre, serve a tavolino per
progettare contro l'evidenza: parrot0 non la chiama, ne' a runtime ne' nei test.

L'osservazione di F. (gen405), che e' una riformulazione del sogno e non un suo
miglioramento: quando a un modello passi della prosa E BASTA, lui non la
apprende — la enuncia, la commenta, la confronta. Se invece la stessa prosa
arriva dentro «acquisisci queste informazioni», la tratta come conoscenza da
trattenere. La prosa e' identica; a cambiare e' l'ATTO in cui e' inserita.

La conseguenza per parrot0 e' diretta. Oggi `--dream` chiede sempre la stessa
cosa — «read the page on X» — a qualunque pagina, per qualunque motivo, e la
lettura e' uguale a se stessa. Se l'intenzione cambia davvero cio' che si
trattiene, allora l'intenzione va DICHIARATA e va scelta da parrot0: leggo per
sapere di piu', oppure leggo PER colmare questa lacuna, e le due letture non
devono rendere lo stesso.

`prose_probe.py` ha gia' misurato l'asse vicino — la stessa frase nuda contro la
stessa frase preceduta da istruzioni su COME leggerla. Qui l'asse e' un altro:
non come leggere, ma PERCHE'. Le tre cornici:

  1. NUDA        solo la prosa, nessuna istruzione. Cosa fa un modello quando
                 non gli si chiede niente? La domanda vera e': cosa fa parrot0
                 oggi, che e' la stessa cosa senza saperlo.
  2. ACQUISIZIONE «acquisisci queste informazioni». L'atto e' dichiarato, il
                 bersaglio no.
  3. MIRATA      «acquisisci queste informazioni per poter rispondere a: <D>».
                 L'atto E il bersaglio. E' esattamente il sogno guidato dalle
                 lacune del gen405: leggere per chiudere un ponte che manca.

Cosa si osserva, e non e' la qualita' della risposta: se cio' che viene
TRATTENUTO cambia fra le tre. Se 3 trattiene cose diverse da 2, allora la lacuna
non e' solo un'agenda di COSA leggere — e' parte di COME si legge, e va passata
all'estrattore.

    python3 tests/probes/dream_intent_probe.py [--model gpt-5.6-luna] [--only NOME]
"""
from __future__ import annotations

import argparse
import json
import os
import re
import sys
import time
import urllib.error
import urllib.request

BASE = "https://opencode.ai/zen/go/v1/chat/completions"

# La prosa e' quella vera del corpus di parrot0, non un esempio costruito: e'
# la pagina su cui il sogno del gen405 ha lasciato per terra tre frasi su sei.
PHOTOSYNTHESIS = (
    "Photosynthesis is a system of biological processes by which "
    "photopigment-bearing autotrophic organisms, such as most plants, algae and "
    "cyanobacteria, convert light energy typically from sunlight into the "
    "chemical energy necessary to fuel their metabolism. The term photosynthesis "
    "usually refers to oxygenic photosynthesis, a process that releases oxygen as "
    "a byproduct of water splitting. Photosynthetic organisms store the converted "
    "chemical energy within the bonds of intracellular organic compounds, "
    "typically carbohydrates like sugars, starches, phytoglycogen and cellulose."
)

DNA = (
    "Deoxyribonucleic acid is a polymer composed of two polynucleotide chains "
    "that coil around each other to form a double helix. DNA and ribonucleic "
    "acid are nucleic acids. The polymer carries genetic instructions for the "
    "development, functioning, growth and reproduction of all known organisms."
)

# nome -> (prosa, domanda-lacuna per la cornice MIRATA, cosa osserviamo)
STIMULI = [
    ("fotosintesi",
     PHOTOSYNTHESIS,
     "Does photosynthesis release oxygen?",
     "La cornice 3 trattiene la frase sull'ossigeno, che nella 1 e nella 2 e' "
     "una fra tante? E' la prova diretta che la lacuna cambia la LETTURA."),
    ("dna",
     DNA,
     "Is DNA a nucleic acid?",
     "«DNA and ribonucleic acid are nucleic acids» e' la forma che oggi "
     "l'estrattore di parrot0 non legge (plurale senza articolo). Con la "
     "domanda davanti, viene trattenuta?"),
]

FRAMES = [
    ("nuda", None),
    ("acquisizione", "Acquire the following information."),
    ("mirata", "Acquire the following information so that you can answer this "
               "question: «{q}»"),
]

# La stessa richiesta finale per tutte e tre, DOPO la prosa: cosa e' rimasto.
PROBE = ("Now, without looking at the text again, list what you retained as "
         "discrete facts, one per line, in the form CLASS(member) or "
         "RELATION(a, b). Only what you would keep.")


def call(key: str, model: str, messages: list[dict], temperature: float) -> str:
    body = json.dumps({"model": model, "max_tokens": 900,
                       "temperature": temperature, "messages": messages}).encode()
    req = urllib.request.Request(BASE, data=body, method="POST", headers={
        "Authorization": f"Bearer {key}",
        "Content-Type": "application/json",
        "User-Agent": "parrot0-dream-intent-probe/1.0"})
    try:
        with urllib.request.urlopen(req, timeout=180) as response:
            data = json.loads(response.read())
        message = data["choices"][0]["message"]
        text = message.get("content") or message.get("reasoning_content") or ""
    except urllib.error.HTTPError as error:
        text = f"[model error {error.code}]"
    except Exception as error:  # noqa: BLE001 — sonda esterna
        text = f"[model error {error}]"
    return re.sub(r"<think>.*?</think>", "", text, flags=re.S).strip()


def fact_lines(reply: str) -> list[str]:
    """Le righe che HANNO la forma di un fatto. Grezzo e falsificabile."""
    out = []
    for line in reply.splitlines():
        line = line.strip(" -*\t")
        if re.match(r"^[A-Za-z_][A-Za-z0-9_ ]*\([^)]+\)\s*\.?$", line):
            out.append(line.rstrip("."))
    return out


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--model", default="gpt-5.6-luna")
    parser.add_argument("--temperature", type=float, default=0.2)
    parser.add_argument("--only", default="")
    parser.add_argument("--out", default="")
    args = parser.parse_args()

    key = os.environ.get("OPENCODE_API_KEY", "")
    if not key:
        print("OPENCODE_API_KEY assente: la sonda ha bisogno dell'oracolo",
              file=sys.stderr)
        return 2

    chosen = [s for s in STIMULI if not args.only or s[0] == args.only]
    if not chosen:
        print(f"nessuno stimolo di nome {args.only!r}", file=sys.stderr)
        return 2

    stamp = time.strftime("%Y%m%d-%H%M%S")
    out = args.out or f"tests/sym/dream-intent-{stamp}.md"
    os.makedirs(os.path.dirname(out), exist_ok=True)

    lines = [f"# dream intent probe — {stamp}", "",
             f"modello: `{args.model}`", "",
             "Stessa prosa, tre cornici. Si osserva cosa viene TRATTENUTO, non",
             "la qualita' della risposta. Nessuna affermazione del modello entra",
             "nella KB senza una fonte propria.", ""]

    for name, prose, question, watch in chosen:
        lines += [f"## {name}", "", f"*cosa osserviamo:* {watch}", ""]
        kept = {}
        for frame_name, frame_tpl in FRAMES:
            messages = []
            if frame_tpl:
                messages.append({"role": "user",
                                 "content": frame_tpl.format(q=question)})
                messages.append({"role": "assistant", "content": "Go ahead."})
            messages.append({"role": "user", "content": prose})
            first = call(key, args.model, messages, args.temperature)
            messages.append({"role": "assistant", "content": first})
            messages.append({"role": "user", "content": PROBE})
            second = call(key, args.model, messages, args.temperature)
            facts = fact_lines(second)
            kept[frame_name] = facts
            lines += [f"### cornice: {frame_name}", "",
                      "**reazione immediata alla prosa** (senza chiedere nulla):",
                      "", "```", first[:900], "```", "",
                      f"**cosa ha trattenuto** ({len(facts)} righe in forma di fatto):",
                      "", "```", "\n".join(facts) if facts else second[:600], "```", ""]
            print(f"{name}/{frame_name}: {len(facts)} fatti trattenuti")

        # la domanda che conta: la cornice mirata trattiene cose che le altre no?
        only_targeted = [f for f in kept.get("mirata", [])
                         if f not in kept.get("acquisizione", [])]
        lines += ["### solo nella cornice MIRATA", "",
                  "Se questa lista non e' vuota, la lacuna non e' solo un'agenda",
                  "di cosa leggere: e' parte di COME si legge.", "",
                  "```", "\n".join(only_targeted) if only_targeted else "(vuota)",
                  "```", ""]
        print(f"{name}: {len(only_targeted)} fatti solo nella cornice mirata")

    with open(out, "w") as handle:
        handle.write("\n".join(lines) + "\n")
    print(f"\ntranscript: {out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
