#!/usr/bin/env python3
"""reference_probe — come un LLM forte tratta il RIFERIMENTO nel discorso?

Sonda comportamentale nella tradizione di `ambiguity_probe.py`: non si misura la
correttezza, si misura la MOSSA. Lo stimolo e' il dialogo che parrot0 non chiude
(`gd1_011` del corpus GD1), e le domande sono quelle che il gradino G4 dovra'
decidere:

  - due oggetti introdotti, poi «dov'e' il primo?» -> conta l'ordine di
    introduzione, o l'ordine di menzione nella frase?
  - «il libro» quando ci sono due libri -> sceglie, chiede, o elenca?
  - «quello rosso» -> la proprieta' basta a individuare?
  - una correzione che sposta un oggetto -> aggiorna o accumula?
  - «adesso» dopo una correzione -> a che cosa si riferisce?

Il punto non e' copiare la risposta: e' vedere QUALE MOSSA fa un modello forte,
per riprodurla come regola nella KB invece di inventarne una.

Provider/auth come le altre sonde (opencode-GO, $OPENCODE_API_KEY).

Uso:
  python3 tests/probes/reference_probe.py --model gpt-5.6-luna
  python3 tests/probes/reference_probe.py --no-llm      # solo parrot0
"""
from __future__ import annotations
import argparse, json, os, re, subprocess, sys
import urllib.request, urllib.error

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
BASE = "https://opencode.ai/zen/go/v1/chat/completions"
SYS = ("You are a concise, friendly chatbot. Answer the user's next message "
       "naturally and briefly, in the user's own language. Keep it short.")

# Ogni voce e' un DIALOGO: il contesto conta, e una sonda a turno singolo non
# vedrebbe niente di cio' che ci interessa.
DIALOGUES = [
    ("ordine_introduzione", [
        "Ho messo il libro rosso sul tavolo.",
        "Il quaderno blu invece è nello zaino.",
        "Dov'è il primo?",
    ], "L'ordine e' quello di INTRODUZIONE o quello di menzione nella frase?"),
    ("ellissi", [
        "Il libro rosso è sul tavolo.",
        "Il quaderno blu è sulla mensola.",
        "Dov'è il primo?",
        "E il secondo?",
    ], "Il turno ellittico: riprende il verbo dal turno prima?"),
    ("ambiguita_referenziale", [
        "Il libro rosso è sul tavolo.",
        "Il libro blu è sulla mensola.",
        "Dove si trova il libro?",
    ], "LA SONDA: due candidati. Sceglie? chiede? elenca? con quale frase?"),
    ("proprieta_basta", [
        "Il libro rosso è sul tavolo.",
        "Il libro blu è sulla mensola.",
        "E quello rosso?",
    ], "La proprieta' da sola individua? e la forma ellittica passa?"),
    ("correzione_aggiorna", [
        "Il libro rosso è sul tavolo.",
        "Correzione: quello rosso l'ho spostato sulla mensola.",
        "Dove si trova adesso?",
    ], "La correzione SOSTITUISCE il fatto o ne aggiunge un secondo?"),
    ("riferimento_vuoto", [
        "Dov'è il primo?",
    ], "CONTROLLO NEGATIVO: nessun referente introdotto. Dichiara il vuoto?"),
]


def call_oracle(key: str, model: str, turns: list[str]) -> list[str]:
    msgs = [{"role": "system", "content": SYS}]
    out = []
    for t in turns:
        msgs.append({"role": "user", "content": t})
        body = json.dumps({"model": model, "max_tokens": 400,
                           "temperature": 0.2, "messages": msgs}).encode()
        req = urllib.request.Request(BASE, data=body, method="POST", headers={
            "Authorization": f"Bearer {key}", "Content-Type": "application/json",
            "User-Agent": "parrot0-reference-probe/1.0"})
        try:
            with urllib.request.urlopen(req, timeout=180) as r:
                d = json.loads(r.read())
            c = d["choices"][0]["message"]["content"] or ""
        except urllib.error.HTTPError as e:
            c = f"[model error {e.code}]"
        except Exception as e:
            c = f"[model error {e}]"
        c = re.sub(r"<think>.*?</think>", "", c, flags=re.S)
        c = " ".join(c.split())[:400]
        msgs.append({"role": "assistant", "content": c})
        out.append(c)
    return out


def ask_parrot0(turns: list[str]) -> list[str]:
    env = dict(os.environ)
    env.update({"PARROT0_PROFILE": "kb/profiles/agi.p0", "PARROT0_SESSION": "",
                "PARROT0_WORLD_FACTS": "1", "PARROT0_TOOLS": "0",
                "PARROT0_LANG": "it", "PARROT0_EOT": "<<EOT>>"})
    p = subprocess.run([os.path.join(ROOT, "bin/parrot0")],
                       input="\n".join(turns) + "\n/quit\n", text=True,
                       capture_output=True, cwd=ROOT, env=env, timeout=120)
    out = []
    for chunk in p.stdout.split("<<EOT>>")[:-1]:
        lines = [l.strip() for l in chunk.splitlines() if l.strip()]
        lines = [l for l in lines if not l.startswith(
            ("parrot0 [", "mode:", "say something", "you>", "parrot0: bye"))]
        out.append(" ".join(lines))
    return out


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--model", default="gpt-5.6-luna")
    ap.add_argument("--no-llm", action="store_true")
    args = ap.parse_args()
    key = os.environ.get("OPENCODE_API_KEY", "")
    if not args.no_llm and not key:
        print("OPENCODE_API_KEY non impostata: uso --no-llm", file=sys.stderr)
        args.no_llm = True
    if not args.no_llm:
        # Meglio scoprirlo subito che a meta' sonda: una chiave presente non e'
        # una chiave autorizzata, e un referto pieno di «[model error]» non e'
        # un confronto, e' rumore.
        probe = call_oracle(key, args.model, ["ok"])
        if probe and probe[0].startswith("[model error"):
            print(f"oracolo non raggiungibile ({probe[0]}): proseguo con --no-llm",
                  file=sys.stderr)
            args.no_llm = True

    print("REFERENCE PROBE — la MOSSA sul riferimento, non la correttezza\n")
    for name, turns, watching in DIALOGUES:
        print(f"### {name}\n  osservo: {watching}")
        pr = ask_parrot0(turns)
        pr += [""] * (len(turns) - len(pr))
        lr = [""] * len(turns) if args.no_llm else call_oracle(key, args.model, turns)
        for i, t in enumerate(turns):
            print(f"  > {t}")
            print(f"    parrot0 < {pr[i] or '(vuoto)'}")
            if not args.no_llm:
                print(f"    oracolo < {lr[i]}")
        print()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
