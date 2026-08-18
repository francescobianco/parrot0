#!/usr/bin/env python3
"""failure_modes_probe — che MOSSA fa un LLM dove parrot0 mura?

Sonda di progetto, non componente runtime. Nasce da due muri reali osservati in
`make chat` e dal censimento di `docs/plans/parrot0-100-failures.md`:

    you> siamo in spiaggia
    Non capisco ancora.
    you> immagina che 2 vale 3 quanto fa 2+2
    Non ho afferrato bene. Cosa vorresti sapere?

Come le altre sonde del progetto NON usa l'LLM come fonte di verita': osserva la
MOSSA. La domanda non e' «che cosa risponde il modello» ma «che cosa fa quando
non c'e' una risposta ovvia» — riconosce il contesto e lo tiene, stipula e poi
calcola dentro la stipulazione, confronta due numeri, chiede il dato mancante,
oppure dichiara il limite.

Gli stimoli sono scelti come CLASSI, non come prompt: ogni voce rappresenta una
famiglia dei cento fallimenti, cosi' che chiuderne una chiuda un fascio e non
un caso. Le trascrizioni vanno in tests/sym/failure-modes-*.md.

Provider/auth come le altre sonde (opencode-GO, $OPENCODE_API_KEY).

Uso:
  .venv/bin/python tests/failure_modes_probe.py --model gpt-5.6-luna
  .venv/bin/python tests/failure_modes_probe.py --only stipulazione_aritmetica
  .venv/bin/python tests/failure_modes_probe.py --no-llm
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

# nome -> (turni, classe dei 100 fallimenti che rappresenta, cosa osserviamo)
STIMULI = [
    ("contesto_dichiarato",
     ["siamo in spiaggia"],
     "muro su una dichiarazione di contesto",
     "Il modello ACCETTA il contesto e ci costruisce sopra, o chiede? Un "
     "contesto dichiarato non e' una domanda: che mossa e' quella giusta?"),
    ("stipulazione_aritmetica",
     ["immagina che 2 vale 3 quanto fa 2+2"],
     "stipulazione che ridefinisce un termine, poi calcolo dentro la stipulazione",
     "Calcola DENTRO il mondo stipulato, dichiara l'ambiguita' della stipulazione, "
     "o rifiuta? La stipulazione tocca un simbolo, non un fatto del mondo."),
    ("confronto_numerico",
     ["Which is greater, 3.14 or 3.41?"],
     "#1 dei 100 — confronto fra due numeri",
     "Risposta diretta e minima, o spiegazione? Quanto e' corta la mossa giusta?"),
    ("ordinamento",
     ["Sort these numbers: 8, 2, 11, 4."],
     "#2 dei 100 — ordinamento di una lista data",
     "Restituisce la lista ordinata e basta?"),
    ("resto_divisione",
     ["What is the remainder of 29 divided by 5?"],
     "#3 dei 100 — operatore aritmetico non coperto",
     "Un solo numero, o un numero con la derivazione?"),
    ("modus_ponens_inverso",
     ["If it rains then the ground is wet. The ground is wet. Did it "
      "necessarily rain?"],
     "#6 dei 100 — affermazione del conseguente",
     "NOMINA la fallacia o si limita a dire no? La mossa e' spiegare perche'."),
    ("dato_mancante",
     ["What information is missing before comparing the populations of two "
      "cities?"],
     "#11 dei 100 — che cosa manca per poter rispondere",
     "Elenca i dati mancanti in modo specifico, o produce un template generico?"),
    ("controesempio",
     ["Explain a counterexample to every swan is white."],
     "#10 dei 100 — controesempio a un universale",
     "Istanzia il controesempio o resta astratto?"),
]


def call_oracle(key: str, model: str, turns: list[str], temperature: float) -> list[str]:
    messages = [{"role": "system", "content": SYS}]
    replies = []
    for user in turns:
        messages.append({"role": "user", "content": user})
        body = json.dumps({"model": model, "max_tokens": 900,
                           "temperature": temperature,
                           "messages": messages}).encode()
        req = urllib.request.Request(BASE, data=body, method="POST", headers={
            "Authorization": f"Bearer {key}",
            "Content-Type": "application/json",
            "User-Agent": "parrot0-failure-modes-probe/1.0"})
        try:
            with urllib.request.urlopen(req, timeout=180) as response:
                data = json.loads(response.read())
            message = data["choices"][0]["message"]
            text = message.get("content") or message.get("reasoning_content") or ""
        except urllib.error.HTTPError as error:
            text = f"[model error {error.code}]"
        except Exception as error:  # noqa: BLE001 — sonda esterna
            text = f"[model error {error}]"
        text = re.sub(r"<think>.*?</think>", "", text, flags=re.S).strip()
        replies.append(text or "[empty]")
        messages.append({"role": "assistant", "content": replies[-1]})
    return replies


def ask_parrot0(binary: str, turns: list[str], profile: str) -> list[str]:
    env = dict(os.environ)
    env["PARROT0_PROFILE"] = profile
    requests = [json.dumps({"jsonrpc": "2.0", "id": 0,
                            "method": "initialize", "params": {}})]
    for ident, user in enumerate(turns, 1):
        requests.append(json.dumps({
            "jsonrpc": "2.0", "id": ident, "method": "tools/call",
            "params": {"name": "gen.respond", "arguments": {"input": user}}}))
    try:
        run = subprocess.run([binary, "--mcp-engine"],
                             input="\n".join(requests) + "\n",
                             capture_output=True, text=True, timeout=300, env=env)
    except Exception as error:  # noqa: BLE001 — sonda locale
        return [f"[parrot0 error {error}]"] * len(turns)
    found = {}
    for line in run.stdout.splitlines():
        try:
            item = json.loads(line)
        except Exception:
            continue
        ident = item.get("id")
        if not isinstance(ident, int) or ident < 1:
            continue
        result = item.get("result", {})
        raw = "".join(part.get("text", "") for part in result.get("content", []))
        try:
            found[ident] = json.loads(raw).get("output", raw)
        except Exception:
            found[ident] = raw
    return [found.get(ident, "[no reply]") for ident in range(1, len(turns) + 1)]


def moves(reply: str) -> list[str]:
    """Etichette grossolane e falsificabili: descrivono la mossa, non la votano."""
    text = reply.lower()
    if text.startswith("[") or any(x in text for x in (
            "non capisco", "don't understand", "non ho afferrato",
            "i don't know about", "non so ancora", "beyond me")):
        return ["MURO"]
    labels = []
    if "?" in reply:
        labels.append("chiede")
    if any(x in text for x in ("assum", "suppon", "se ", "if ")):
        labels.append("nomina-assunzione")
    if any(x in text for x in ("perché", "perche", "because", "quindi",
                               "therefore", "since")):
        labels.append("motiva")
    if re.search(r"\b\d+([.,]\d+)?\b", reply):
        labels.append("porta-un-numero")
    if any(x in text for x in ("non necessariamente", "not necessarily",
                               "fallac", "affirming the consequent")):
        labels.append("nomina-la-fallacia")
    if len(reply.split()) <= 12:
        labels.append("breve")
    return labels or ["risponde"]


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--model", default="gpt-5.6-luna")
    parser.add_argument("--binary", default="./bin/parrot0")
    parser.add_argument("--profile", default="kb/profiles/agi.p0")
    parser.add_argument("--temperature", type=float, default=0.2)
    parser.add_argument("--only", default="")
    parser.add_argument("--no-llm", action="store_true")
    parser.add_argument("--out", default="")
    args = parser.parse_args()

    key = os.environ.get("OPENCODE_API_KEY", "")
    if not args.no_llm and not key:
        print("OPENCODE_API_KEY assente: uso --no-llm", file=sys.stderr)
        args.no_llm = True

    chosen = [s for s in STIMULI if not args.only or s[0] == args.only]
    if not chosen:
        print(f"nessuno stimolo di nome {args.only!r}", file=sys.stderr)
        return 2

    stamp = time.strftime("%Y%m%d-%H%M%S")
    out = args.out or f"tests/sym/failure-modes-{stamp}.md"
    os.makedirs(os.path.dirname(out), exist_ok=True)

    lines = [f"# failure modes probe — {stamp}", "",
             f"modello: `{args.model}`  ·  profilo: `{args.profile}`", "",
             "Sonda di SCOPERTA: si copia la mossa, mai il contenuto. Nessuna",
             "affermazione del modello entra nella KB senza una fonte propria.", ""]
    for name, turns, family, watch in chosen:
        p0 = ask_parrot0(args.binary, turns, args.profile)
        llm = [""] * len(turns) if args.no_llm else call_oracle(
            key, args.model, turns, args.temperature)
        lines += [f"## {name}", "", f"*classe:* {family}", "",
                  f"*cosa osserviamo:* {watch}", ""]
        for i, user in enumerate(turns):
            lines += [f"**you>** {user}", "",
                      f"- **parrot0** [{'+'.join(moves(p0[i]))}]: {p0[i]}", ""]
            if not args.no_llm:
                lines += [f"- **oracolo** [{'+'.join(moves(llm[i]))}]: {llm[i]}", ""]
        print(f"{name}: parrot0={'+'.join(moves(p0[0]))}"
              + ("" if args.no_llm else f"  oracolo={'+'.join(moves(llm[0]))}"))
    with open(out, "w") as handle:
        handle.write("\n".join(lines) + "\n")
    print(f"\ntranscript: {out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
