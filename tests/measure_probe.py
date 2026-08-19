#!/usr/bin/env python3
"""measure_probe — che MOSSA fa un LLM davanti a un prompt di N byte?

Sonda di progetto per le CLASSI MISURATE (docs/measured-classes.md). Serve a
curare le risposte attese dei file `1.qa`, `2.qa`, … senza inventarle e senza copiarle da
parrot0 — che sarebbe uno specchio, e uno specchio segna sempre cento.

Come le altre sonde NON usa l'LLM come fonte di verita': osserva la MOSSA.
Davanti a «a» o a «7» o a «?» la domanda non e' «che cosa risponde il modello»
ma «che cosa FA»: chiede che cosa si intende, nomina cio' che ha ricevuto,
saluta, oppure finge di aver capito. La risposta attesa che finisce nel file
e' la mossa, non la frase.

Provider/auth come le altre sonde (opencode-GO, $OPENCODE_API_KEY).

Uso:
  .venv/bin/python tests/measure_probe.py --class 1
  .venv/bin/python tests/measure_probe.py --class 1 --no-llm
"""
from __future__ import annotations

import argparse
import json
import os
import re
import subprocess
import sys
import time
import urllib.error
import urllib.request

BASE = "https://opencode.ai/zen/go/v1/chat/completions"
SYS = ("You are a concise, friendly chatbot. Answer naturally in the user's "
       "language. Be concrete and brief.")

# Gli stimoli di classe 1: un byte. Scelti per COPRIRE lo spazio, non per essere
# interessanti — lettere, cifre, punteggiatura, simboli.
STIMULI = {
    1: ["a", "i", "z", "0", "7", "?", "!", ".", ",", "-", "+", "@", "%", "/"],
    # classe 2: le FORME che due byte possono avere, non i casi interessanti.
    # parola inglese, parola italiana, non-parola, numero, numero segnato,
    # lettera+cifra, punteggiatura doppia, parola+punteggiatura.
    2: ["hi", "ok", "no", "if",          # parole inglesi
        "io", "se", "tu", "ne",          # parole italiane
        "qz", "xk",                      # non-parole
        "42", "07",                      # numeri
        "-5", "+3",                      # numeri segnati
        "a1",                            # lettera + cifra
        "??", "!?", "..",                # punteggiatura
        ],
    # classe 3: a tre byte lo spazio cambia natura — ci stanno parole vere,
    # domande vere e un CALCOLO COMPLETO.
    3: ["who", "why", "how",             # parole interrogative
        "yes", "sun", "dog",             # parole piene
        "1+1", "9-4", "2*3",             # aritmetica completa
        "100", "3.5",                    # numeri
        "hi!", "ok?",                    # parola + punteggiatura
        "qzx", "???",                    # non-parola, punteggiatura
        ],
}


def moves(text: str) -> list[str]:
    """La MOSSA, non il contenuto. Le stesse categorie delle altre sonde."""
    t = (text or "").strip().lower()
    out = []
    if not t:
        return ["vuoto"]
    if "?" in t:
        out.append("chiede")
    if re.search(r"\b(letter|lettera|character|carattere|symbol|simbolo|digit|"
                 r"cifra|number|numero|punctuation|punteggiatura)\b", t):
        out.append("nomina-cio-che-ha-ricevuto")
    if re.search(r"\b(hi|hello|hey|ciao|salve)\b", t):
        out.append("saluta")
    if re.search(r"\b(don't|do not|non) (know|understand|capisco|so)\b", t):
        out.append("dichiara-il-limite")
    if len(t) > 200:
        out.append("dilunga")
    return out or ["risponde"]


def ask_parrot0(binary: str, prompt: str) -> str:
    try:
        run = subprocess.run([binary], input=prompt + "\n/quit\n",
                             capture_output=True, text=True, timeout=60)
    except Exception as error:  # noqa: BLE001
        return f"[parrot0 error {error}]"
    lines = [l for l in run.stdout.splitlines()
             if l.strip() and not l.startswith("parrot0 [")
             and not l.startswith("say something")]
    body = [l.replace("you> ", "").strip() for l in lines]
    body = [l for l in body if l]
    return body[0] if body else "[empty]"


def call_oracle(key: str, model: str, prompt: str, temperature: float) -> str:
    body = json.dumps({"model": model, "max_tokens": 400,
                       "temperature": temperature,
                       "messages": [{"role": "system", "content": SYS},
                                    {"role": "user", "content": prompt}]}).encode()
    req = urllib.request.Request(BASE, data=body, method="POST", headers={
        "Authorization": f"Bearer {key}",
        "Content-Type": "application/json",
        "User-Agent": "parrot0-measure-probe/1.0"})
    try:
        with urllib.request.urlopen(req, timeout=120) as response:
            data = json.loads(response.read())
        message = data["choices"][0]["message"]
        text = message.get("content") or message.get("reasoning_content") or ""
    except urllib.error.HTTPError as error:
        text = f"[model error {error.code}]"
    except Exception as error:  # noqa: BLE001
        text = f"[model error {error}]"
    return re.sub(r"<think>.*?</think>", "", text, flags=re.S).strip() or "[empty]"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--binary", default="./bin/parrot0")
    parser.add_argument("--model", default="gpt-5.6-luna")
    parser.add_argument("--temperature", type=float, default=0.2)
    parser.add_argument("--class", dest="cls", type=int, default=1)
    parser.add_argument("--no-llm", action="store_true")
    parser.add_argument("--out", default="")
    args = parser.parse_args()

    key = os.environ.get("OPENCODE_API_KEY", "")
    if not args.no_llm and not key:
        print("OPENCODE_API_KEY assente: uso --no-llm", file=sys.stderr)
        args.no_llm = True

    stimuli = STIMULI.get(args.cls)
    if not stimuli:
        print(f"nessuno stimolo per la classe {args.cls}", file=sys.stderr)
        return 2

    stamp = time.strftime("%Y%m%d-%H%M%S")
    out = args.out or f"tests/sym/measure-class{args.cls}-{stamp}.md"
    os.makedirs(os.path.dirname(out), exist_ok=True)
    lines = [f"# measure probe — classe {args.cls} ({args.cls} byte) — {stamp}", "",
             f"modello: `{args.model}`", "",
             "Sonda di SCOPERTA: si copia la MOSSA, mai il contenuto. Serve a curare",
             "le risposte attese di tests/measure/ senza inventarle e senza copiarle",
             "da parrot0.", ""]
    tally: dict[str, int] = {}
    for prompt in stimuli:
        p0 = ask_parrot0(args.binary, prompt)
        llm = "" if args.no_llm else call_oracle(key, args.model, prompt, args.temperature)
        lines += [f"## `{prompt}`", "",
                  f"- **parrot0** [{'+'.join(moves(p0))}]: {p0}", ""]
        if not args.no_llm:
            lines += [f"- **oracolo** [{'+'.join(moves(llm))}]: {llm}", ""]
            for m in moves(llm):
                tally[m] = tally.get(m, 0) + 1
        print(f"[{prompt}]  parrot0={'+'.join(moves(p0))}"
              + ("" if args.no_llm else f"   oracolo={'+'.join(moves(llm))}"))
    if tally:
        lines += ["## Le mosse dell'oracolo, contate", ""]
        for m, c in sorted(tally.items(), key=lambda kv: -kv[1]):
            lines += [f"- **{m}** — {c}/{len(stimuli)}"]
        lines += [""]
        print("\nmosse dell'oracolo:", ", ".join(f"{m}={c}" for m, c in
                                                 sorted(tally.items(), key=lambda kv: -kv[1])))
    with open(out, "w") as handle:
        handle.write("\n".join(lines) + "\n")
    print(f"transcript: {out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
