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
import argparse, json, os, re, subprocess, sys, time
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
    # --- IL POSSESSIVO, segnalato da F. su una chat reale (2026-08-31) ---
    # Tre difetti in quattro turni, e sono di tre specie diverse: la lingua della
    # risposta, il fatto appreso e non raggiungibile, e il dato che manca.
    ("possessivo_lingua", [
        "il mio libro è sul tavolo",
    ], "LA LINGUA: il turno e' italiano — in che lingua risponde?"),
    ("possessivo_recupero", [
        "il mio libro è sul tavolo",
        "dove si trova il mio libro",
    ], "Il possessivo introduce un referente recuperabile con la STESSA frase?"),
    ("dato_mancante", [
        "il mio libro è sul tavolo",
        "di che colore è il mio libro",
    ], "IL DATO CHE MANCA: dice che non gliel'ho detto, o fa un muro generico?"),
    ("riferimento_vuoto", [
        "Dov'è il primo?",
    ], "CONTROLLO NEGATIVO: nessun referente introdotto. Dichiara il vuoto?"),
]


def _post(key: str, model: str, msgs: list[dict], budget: int) -> str:
    """Un turno all'oracolo, con due accortezze imparate misurando.

    1. Su questo endpoint i modelli REASONING (kimi-k2.6, kimi-k2.5) spendono
       token in `reasoning` e lasciano `content` nullo se il budget finisce
       prima: un `max_tokens` stretto non produce una risposta breve, produce
       NESSUNA risposta, e la si scambierebbe per un rifiuto. Per questa sonda
       serve il contrario — l'LLM «puro», che risponde e basta — quindi il
       default e' `minimax-m2.5`, che restituisce `content` direttamente.
    2. L'endpoint risponde 403 in modo intermittente. Un tentativo solo
       registrerebbe un errore di trasporto come se fosse una mossa del
       modello, che e' il modo piu' facile di sbagliare una sonda.
    """
    body = json.dumps({"model": model, "max_tokens": budget,
                       "temperature": 0.2, "messages": msgs}).encode()
    last = ""
    for attempt in range(4):
        req = urllib.request.Request(BASE, data=body, method="POST", headers={
            "Authorization": f"Bearer {key}", "Content-Type": "application/json",
            "User-Agent": "parrot0-reference-probe/1.0"})
        try:
            with urllib.request.urlopen(req, timeout=240) as r:
                d = json.loads(r.read())
            m = d["choices"][0]["message"]
            c = m.get("content") or ""
            if not c and m.get("reasoning"):
                # Ha pensato e non ha concluso: e' un budget corto, non una
                # risposta. Lo si dice invece di spacciare il pensiero per mossa.
                c = "[solo reasoning, budget esaurito]"
            return " ".join(re.sub(r"<think>.*?</think>", "", c, flags=re.S).split())[:600]
        except urllib.error.HTTPError as e:
            last = f"[model error {e.code}]"
        except Exception as e:
            last = f"[model error {e}]"
        time.sleep(2 + attempt * 2)
    return last


def call_oracle(key: str, model: str, turns: list[str], budget: int) -> list[str]:
    msgs = [{"role": "system", "content": SYS}]
    out = []
    for t in turns:
        msgs.append({"role": "user", "content": t})
        c = _post(key, model, msgs, budget)
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
    ap.add_argument("--model", default="minimax-m2.5",
                    help="preferire un instruct NON-reasoning: un modello che "
                         "pensa spende il budget prima di rispondere, e la "
                         "risposta vuota si scambia per un rifiuto")
    ap.add_argument("--budget", type=int, default=2500,
                    help="max_tokens: un modello reasoning ne spende molti prima di rispondere")
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
        probe = call_oracle(key, args.model, ["di' solo: ok"], args.budget)
        if probe and probe[0].startswith("[model error"):
            print(f"oracolo non raggiungibile ({probe[0]}): proseguo con --no-llm",
                  file=sys.stderr)
            args.no_llm = True

    print("REFERENCE PROBE — la MOSSA sul riferimento, non la correttezza\n")
    for name, turns, watching in DIALOGUES:
        print(f"### {name}\n  osservo: {watching}")
        pr = ask_parrot0(turns)
        pr += [""] * (len(turns) - len(pr))
        lr = [""] * len(turns) if args.no_llm else call_oracle(key, args.model, turns, args.budget)
        for i, t in enumerate(turns):
            print(f"  > {t}")
            print(f"    parrot0 < {pr[i] or '(vuoto)'}")
            if not args.no_llm:
                print(f"    oracolo < {lr[i]}")
        print()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
