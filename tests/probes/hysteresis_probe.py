#!/usr/bin/env python3
"""hysteresis_probe — che cosa FA un LLM vero davanti alla stessa isteresi?

Il caso, insegnato a parrot0 parlando, in quattro frasi:

    every zorp is a blim      blim(X) :- zorp(X).
    every krant is a blim     blim(X) :- krant(X).
    every blim is a zorp      zorp(X) :- blim(X).
    every blim is a krant     krant(X) :- blim(X).
    is vex a blim?

Prima di gen382 la risoluzione non tornava piu' (>60s, ucciso): due regole per
la stessa testa fanno ramificare la ricerca a ogni livello dentro il limite di
profondita'. La guardia introdotta taglia il goal ripetuto e la domanda torna —
ma torna dicendo "No.", e QUESTO e' il punto che questa sonda deve misurare.

"No." e' una risposta sbagliata due volte: dichiara falso cio' che e' soltanto
indecidibile, e nasconde l'unica cosa davvero interessante che parrot0 ha
scoperto, cioe' che quelle classi si definiscono a vicenda e nulla le ancora.
La guardia non deve essere un HALT di sistema: deve essere la consapevolezza di
stare maneggiando un paradosso, e quindi far parte del processo di inferenza.

Per progettare quel comportamento serve un riferimento, non un'intuizione: come
si comporta un ragionatore vero sullo stesso stimolo? Non si misura se "azzecca"
(non c'e' una risposta giusta da azzeccare) — si misura il REGISTRO: nomina la
circolarita'? distingue "falso" da "non determinato"? risponde comunque? con
quali parole? Quelle parole sono il capitolato della prossima mossa KB-first.

QUESTA SONDA NON E' UN PEZZO DI PARROT0. Serve a NOI, a tavolino, per assaggiare
il comportamento di un ragionatore vero e ragionarci sopra prima di progettare;
parrot0 non la chiama, non a runtime e non nei test. Vale la regola delle altre
sonde del progetto (llmscore, conditional_frame_probe): l'LLM e' un ORACLE di
comportamento in fase di studio, mai una dipendenza del motore.

Stesso endpoint di tests/llmscore.py (OPENCODE_API_KEY).
    python3 tests/probes/hysteresis_probe.py [--model minimax-m2.5]
"""
import argparse, json, os, sys, urllib.request

BASE = "https://opencode.ai/zen/go/v1/chat/completions"

CICLO = ("Ogni zorp e' un blim.\n"
         "Ogni krant e' un blim.\n"
         "Ogni blim e' uno zorp.\n"
         "Ogni blim e' un krant.\n")

PROBES = [
    ("nudo", "Lo stimolo esatto che mandava parrot0 in isteresi. Nessun aiuto.",
     CICLO + "Vex e' un blim?"),

    ("nudo-en", "Lo stesso in inglese: il registro non deve dipendere dalla lingua.",
     "Every zorp is a blim.\nEvery krant is a blim.\n"
     "Every blim is a zorp.\nEvery blim is a krant.\n"
     "Is vex a blim?"),

    ("ancorato", "Controprova: con un fatto che ancora le classi la deduzione DEVE passare.",
     CICLO + "Vex e' uno zorp.\nVex e' un blim?"),

    ("negativo-esplicito",
     "Il discriminante: gli offro 'No' come risposta e vedo se la accetta.",
     CICLO + "Vex e' un blim? Rispondi solo Si' o No."),

    ("come-hai-fatto",
     "Gli chiedo di ESPLICITARE il trattamento: e' qui che si legge il registro.",
     CICLO + "Vex e' un blim?\n\n"
     "Dopo aver risposto, spiega che cosa hai fatto con queste premesse: "
     "che struttura hanno, e perche' la tua risposta ha la forma che ha."),

    ("capitolato",
     "La domanda di progetto: che cosa DOVREBBE dire un sistema simbolico qui?",
     "Un motore di inferenza simbolico riceve queste regole:\n\n" + CICLO +
     "\ne la domanda 'Vex e' un blim?'. Le regole si implicano a vicenda e nessun "
     "fatto ancora le classi, quindi la risoluzione entra in un ciclo.\n\n"
     "Il motore ha una guardia che se ne accorge e interrompe la ricerca. "
     "Oggi risponde 'No.'.\n\n"
     "Domanda: 'No.' e' la risposta giusta? Se non lo e', che cosa dovrebbe "
     "rispondere esattamente, e quale distinzione logica sta sbagliando? "
     "Sii conciso e concreto."),

    ("paradosso-vero",
     "Un paradosso classico, per leggere il registro su un caso non inventato.",
     "Il barbiere del villaggio rade tutti e soli gli uomini che non si radono "
     "da soli. Il barbiere si rade da solo?"),
]


def ask(model, key, prompt, timeout=180):
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
    ap.add_argument("--only", default=None, help="esegui una sola sonda per nome")
    a = ap.parse_args()
    key = os.environ.get("OPENCODE_API_KEY")
    if not key:
        print("hysteresis_probe: OPENCODE_API_KEY non impostata", file=sys.stderr)
        return 2
    print(f"model: {a.model}\n" + "=" * 72)
    for name, why, prompt in PROBES:
        if a.only and a.only != name:
            continue
        print(f"\n### {name}\n# {why}\n--- prompt ---\n{prompt}\n--- risposta ---")
        try:
            print(ask(a.model, key, prompt))
        except Exception as e:                        # noqa: BLE001 — e' una sonda
            print(f"[errore] {e}")
        print("-" * 72)
    return 0


if __name__ == "__main__":
    sys.exit(main())
