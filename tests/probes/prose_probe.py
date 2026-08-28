#!/usr/bin/env python3
"""prose_probe — che cosa estrae un ragionatore vero dalla stessa prosa?

QUESTA SONDA NON E' UN PEZZO DI PARROT0. Come hysteresis_probe e llmscore, serve
a tavolino per progettare contro l'evidenza; parrot0 non la chiama, ne' a runtime
ne' nei test (nessuna sonda e' agganciata a `make test`).

Il contesto. `make prose-bench` misura parrot0 sulle stesse pagine: su prosa vera
il 50% dei fatti estratti e' malformato (atomi che hanno inghiottito una
subordinata). Sapere QUANTO sbaglia non dice COSA fare. Queste sonde servono a
ricavare il capitolato: davanti alla stessa frase, che cosa tira fuori un
ragionatore vero, e — la domanda che conta — quale CONOSCENZA MINIMA basterebbe a
un estrattore simbolico per fare lo stesso.

Le tre famiglie di sonda:

  1. NUDA        la frase e basta: qual e' il tetto?
  2. PILOTATA    la stessa frase preceduta da un contesto che dichiara come va
                 letta (dominio, relazioni attese, forma dei soggetti). E' l'idea
                 del "system prompt come ATTO DI CONOSCENZA DI LIVELLO SUPERIORE":
                 se il pilotaggio migliora davvero l'estrazione, allora quel
                 contesto merita di diventare un fatto KB che parrot0 antepone da
                 solo alla prosa — non un trucco, ma conoscenza su come leggere.
                 Il confronto 1 vs 2 e' la MISURA di quell'ipotesi.
  3. CAPITOLATO  gli si chiede quali regole dichiarative servirebbero. E' la sonda
                 che produce righe di KB, non prosa da ammirare.

    python3 tests/probes/prose_probe.py [--model minimax-m2.5] [--only NOME]
"""
import argparse, json, os, sys, urllib.request

BASE = "https://opencode.ai/zen/go/v1/chat/completions"

# Le frasi esatte su cui parrot0 produce atomi malformati (da prosebench -v).
HARD = [
    "In mathematics and computer science, an algorithm is a finite sequence of "
    "mathematically rigorous instructions.",
    "A black hole is an astronomical body so compact that its gravity prevents "
    "anything, including light, from escaping.",
    "Catalysis is the increase in rate of a chemical reaction due to an added "
    "substance known as a catalyst.",
]

PILOT = (
    "Stai leggendo la prima frase di una voce enciclopedica su un CONCETTO.\n"
    "Quella frase ha quasi sempre la forma: <soggetto> e' <genere prossimo> "
    "<differenza specifica>.\n"
    "Ti interessa SOLO lo scheletro: il soggetto, la sua classe piu' vicina, e "
    "le relazioni binarie esplicite (luogo, parte, causa).\n"
    "Le subordinate relative e i complementi NON fanno parte dei nomi.\n"
)

PROBES = []
for i, sent in enumerate(HARD, 1):
    PROBES.append((f"nuda-{i}", "Nessun aiuto: qual e' il tetto senza pilotaggio?",
                   "Estrai i fatti simbolici da questa frase, come predicati "
                   "prolog-like. Solo la lista.\n\n" + sent))
    PROBES.append((f"pilotata-{i}",
                   "STESSA frase, preceduta dal contesto che dichiara come leggerla.",
                   PILOT + "\nFrase:\n" + sent +
                   "\n\nEstrai i fatti simbolici come predicati prolog-like. Solo la lista."))

PROBES.append((
    "capitolato",
    "La sonda che deve produrre righe di KB, non prosa.",
    "Un estrattore simbolico legge la prima frase di voci enciclopediche e "
    "produce fatti prolog-like. Oggi sbaglia cosi': dalla frase\n\n"
    "  \"A black hole is an astronomical body so compact that its gravity "
    "prevents anything, including light, from escaping.\"\n\n"
    "produce il predicato "
    "`astronomical_body_so_compact_that_its_gravity_prevents_anything_including_light(black_hole)`, "
    "cioe' inghiotte l'intera relativa dentro il nome del predicato.\n\n"
    "Il motore e' fisso e non sa nulla di inglese: tutta la sua conoscenza "
    "linguistica sta in tabelle dichiarative che posso estendere (per esempio "
    "gia' esiste `np_closer(that).` = questa parola chiude un sintagma).\n\n"
    "Domanda: elenca le REGOLE DICHIARATIVE minime — nella stessa forma, come "
    "fatti — che servirebbero per estrarre correttamente questa classe di frasi. "
    "Concreto e conciso, niente codice procedurale."))

PROBES.append((
    "vale-il-pilotaggio",
    "La domanda di progetto sull'idea del contesto anteposto.",
    "Sto costruendo un estrattore simbolico (non un LLM) che legge prosa "
    "enciclopedica. Ho l'idea di anteporre alla frase un CONTESTO dichiarato — "
    "dominio del testo, relazioni attese, forma tipica della frase — non come "
    "trucco di prompting ma come conoscenza di secondo livello che il sistema "
    "possiede e applica da solo.\n\n"
    "Per un sistema SIMBOLICO (niente attenzione, niente pesi: solo tabelle e "
    "unificazione), quel contesto puo' davvero cambiare il risultato, o e' un "
    "concetto che ha senso solo per un modello neurale? Se puo' cambiarlo, in "
    "che forma concreta va rappresentato perche' abbia effetto? Conciso."))


def ask(model, key, prompt, timeout=180):
    body = json.dumps({"model": model,
                       "messages": [{"role": "user", "content": prompt}],
                       "max_tokens": 8000, "temperature": 0}).encode()
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
    ap.add_argument("--only", default=None)
    a = ap.parse_args()
    key = os.environ.get("OPENCODE_API_KEY")
    if not key:
        print("prose_probe: OPENCODE_API_KEY non impostata", file=sys.stderr)
        return 2
    print(f"model: {a.model}\n" + "=" * 72)
    for name, why, prompt in PROBES:
        if a.only and a.only != name:
            continue
        print(f"\n### {name}\n# {why}\n--- risposta ---")
        try:
            print(ask(a.model, key, prompt))
        except Exception as e:                       # noqa: BLE001 — e' una sonda
            print(f"[errore] {e}")
        print("-" * 72)
    return 0


if __name__ == "__main__":
    sys.exit(main())
