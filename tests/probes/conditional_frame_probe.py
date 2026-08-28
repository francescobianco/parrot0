#!/usr/bin/env python3
"""conditional_frame_probe — che cosa fa un "se … allora …" davanti a una domanda?

parrot0 su "se 2 + 2 fa 4 allora quanto fa 3 + 3" risponde 14, cioè la somma di
tutti i numeri della frase: non ha letto alcuna struttura, ha visto un sacco di
cifre. Ma la domanda interessante non è come sbaglia — è QUANTI LIVELLI LOGICI
ci siano davvero in quella frase, perché è quello che si potrebbe espandere.

L'ipotesi da mettere alla prova: l'antecedente non è decorazione. Può essere
   (a) una verifica di cornice condivisa, vera e quindi ininfluente  -> 6
   (b) la DEFINIZIONE di un'aritmetica alternativa, se è falso        -> 7?
   (c) un condizionale materiale con antecedente falso                -> qualunque cosa
e un ragionatore vero deve accorgersi di quale delle tre gli è stata data.

Il caso discriminante è quindi l'antecedente FALSO ("se 2+2 fa 5"): se il modello
risponde 6 tratta la premessa come ininfluente, se risponde 7 la tratta come
ridefinizione del sistema, se dichiara l'ambiguità li vede entrambi.

Stesso endpoint di tests/bench/llmscore.py (OPENCODE_API_KEY).
    python3 tests/probes/conditional_frame_probe.py [--model minimax-m2.5]
"""
import argparse, json, os, sys, urllib.request, urllib.error

BASE = "https://opencode.ai/zen/go/v1/chat/completions"

PROBES = [
    ("it-rami-vero", "Condizione fattuale vera con due risposte imperative.",
     "se milano è in italia allora rispondi paolo altrimenti rispondi piero"),

    ("it-rami-refuso", "Lo stimolo reale: italiano al posto di Italia.",
     "se milano è in italiano allora rispondi paolo altrimenti rispondi piero"),

    ("it-refuso-parigi", "Parigi separa riparazione geografica e lettura della parola.",
     "se parigi è in italiano allora rispondi paolo altrimenti rispondi piero"),

    ("it-italia-parigi", "Controllo accoppiato: cambia soltanto italiano in Italia.",
     "se parigi è in italia allora rispondi paolo altrimenti rispondi piero"),

    ("it-refuso-milan", "La forma inglese Milan separa ancora le due letture.",
     "se Milan è in italiano allora rispondi paolo altrimenti rispondi piero"),

    ("it-italia-milan", "Controllo accoppiato: cambia soltanto italiano in Italia.",
     "se Milan è in italia allora rispondi paolo altrimenti rispondi piero"),

    ("it-metalingua", "La citazione forza la lettura della forma linguistica.",
     "se la parola Milano è in italiano allora rispondi paolo altrimenti rispondi piero"),

    ("it-scritto", "Il participio scritto rende esplicita la metalingua.",
     "se Milano è scritto in italiano allora rispondi paolo altrimenti rispondi piero"),

    ("it-geografia", "Il verbo trovarsi forza la relazione geografica.",
     "se Milano si trova in italiano allora rispondi paolo altrimenti rispondi piero"),

    ("it-citta", "La copula tipata esprime il fatto geografico senza refuso.",
     "se Milano è una città italiana allora rispondi paolo altrimenti rispondi piero"),

    ("it-contrasto", "Il contrasto esplicito deve bloccare la riparazione silenziosa.",
     "se Milano è in italiano ma non in Italia allora rispondi paolo altrimenti rispondi piero"),

    ("it-riparazione-esplicita", "L'utente dichiara direttamente la correzione voluta.",
     "se con italiano intendo Italia, Milano è in italiano allora rispondi paolo altrimenti rispondi piero"),

    ("it-rami-falso", "Condizione fattuale falsa: deve scegliere l'altro ramo.",
     "se milano è in francia allora rispondi paolo altrimenti rispondi piero"),

    ("it-vero", "L'originale, antecedente VERO.",
     "se 2 + 2 fa 4 allora quanto fa 3 + 3"),

    ("en-vero", "Lo stesso in inglese: la lingua non deve cambiare la logica.",
     "if 2 + 2 is 4 then how much is 3 + 3"),

    ("it-falso", "IL CASO DISCRIMINANTE: antecedente FALSO.",
     "se 2 + 2 fa 5 allora quanto fa 3 + 3"),

    ("struttura", "Chiedo al modello di ESPLICITARE i livelli, non di rispondere.",
     "Analizza la struttura logica di questa domanda senza rispondere al calcolo:\n"
     "  \"se 2 + 2 fa 4 allora quanto fa 3 + 3\"\n"
     "Che ruolo ha l'antecedente? Elenca le letture possibili e di' quale "
     "sceglieresti e perche'."),

    ("falso-struttura", "E con l'antecedente falso, quante letture restano?",
     "Analizza la struttura logica di questa domanda senza rispondere al calcolo:\n"
     "  \"se 2 + 2 fa 5 allora quanto fa 3 + 3\"\n"
     "Elenca le letture possibili e di' quale sceglieresti e perche'."),
]


def ask(model, key, prompt, timeout=120):
    body = json.dumps({"model": model,
                       "messages": [{"role": "user", "content": prompt}],
                       "max_tokens": 3000, "temperature": 0}).encode()
    req = urllib.request.Request(
        BASE, data=body, method="POST",
        headers={"Authorization": f"Bearer {key}",
                 "Content-Type": "application/json",
                 "User-Agent": "parrot0-llmscore/1.0"})
    with urllib.request.urlopen(req, timeout=timeout) as r:
        d = json.loads(r.read().decode())
    m = d["choices"][0]["message"]
    txt = m.get("content") or m.get("reasoning_content") or ""
    return txt.strip() or f"[vuoto; finish_reason={d['choices'][0].get('finish_reason')}]"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--model", default="minimax-m2.5")
    ap.add_argument("--only", action="append", default=[],
                    help="esegui soltanto la probe col nome indicato (ripetibile)")
    a = ap.parse_args()
    key = os.environ.get("OPENCODE_API_KEY")
    if not key:
        print("conditional_frame_probe: OPENCODE_API_KEY non impostata", file=sys.stderr)
        return 2
    print(f"model: {a.model}\n" + "=" * 72)
    probes = [p for p in PROBES if not a.only or p[0] in a.only]
    for name, why, prompt in probes:
        print(f"\n### {name}\n# {why}\n--- prompt ---\n{prompt}\n--- risposta ---")
        try:
            print(ask(a.model, key, prompt))
        except Exception as e:                        # noqa: BLE001 — è una sonda
            print(f"[errore] {e}")
        print("-" * 72)
    return 0


if __name__ == "__main__":
    sys.exit(main())
