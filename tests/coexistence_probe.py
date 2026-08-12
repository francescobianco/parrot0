#!/usr/bin/env python3
"""coexistence_probe — un ragionatore vero ISOLA o fa CONVIVERE?

QUESTA SONDA NON E' UN PEZZO DI PARROT0. Come hysteresis_probe, prose_probe e
premise_frame_probe: serve a tavolino per progettare contro l'evidenza. parrot0
non la chiama, ne' a runtime ne' nei test.

COSA SI SA GIA' (gen371, tests/premise_frame_probe.py). L'LLM **non isola:
DECIDE**. Tiene insieme cio' che le premesse implicano e cio' che e' vero, e
sceglie il livello che la domanda chiede; sui pinguini solleva l'ambiguita' da
solo. Quella sonda ha gia' risposto alla domanda "isola o no?".

COSA NON SI SA, ed e' quello che serve adesso. gen382i ha sostituito
l'amputazione (una KB vuota per le ipotetiche) con la RINOMINAZIONE: i termini
delle premesse diventano token freschi, verificati assenti dalla KB, quindi
nessun fatto esistente puo' unificare con loro. Il mondo e' chiuso per
costruzione e la KB resta intera. Ma resta aperta la domanda di progetto:

    quando una premessa contraddice il mondo, il "gatto" dell'ipotesi e' un
    ALTRO concetto (rinominazione), o lo STESSO concetto con una credenza
    sospesa (convivenza con provenienza)?

Sono due architetture diverse, non due parole per la stessa cosa:
  - rinominare  -> il conflitto sparisce, e con esso la possibilita' di NOTARLO;
  - sospendere  -> il conflitto resta visibile e va gestito, ma serve un modo di
                   dire "questo lo credo per ipotesi" a livello di singolo fatto.

parrot0 oggi fa il primo. gen375 (class_conflict) fa il secondo su un'altra
superficie: accetta "un cane e' un pesce" E NOMINA la tensione. Le due strade
convivono nel sistema senza essersi mai incontrate, ed e' questo che la sonda
deve illuminare.

    python3 tests/coexistence_probe.py [--model minimax-m2.5] [--only NOME]
"""
import argparse, json, os, sys, urllib.request

BASE = "https://opencode.ai/zen/go/v1/chat/completions"

PROBES = [
    ("stesso-concetto",
     "IL CUORE: il gatto dell'ipotesi e' lo stesso gatto del mondo?",
     "Supponi che tutti i gatti siano pesci. Tom e' un gatto.\n\n"
     "1) Tom e' un pesce?\n"
     "2) E la domanda che mi interessa davvero: mentre ragionavi, la parola "
     "\"gatto\" dell'ipotesi denotava LO STESSO concetto del gatto che conosci, "
     "oppure un concetto diverso che porta per caso lo stesso nome? "
     "Rispondi a questa seconda domanda con cura."),

    ("persistenza",
     "Dopo l'ipotesi, la supposizione resta o se ne va?",
     "Supponi che tutti i gatti siano pesci. Tom e' un gatto. Tom e' un pesce?\n\n"
     "Ora, finita quella supposizione: un gatto e' un mammifero? E Tom, che cosa "
     "e' davvero, per quanto ne sai?"),

    ("tre-livelli",
     "Premessa ipotetica, mondo, e una regola insegnata che contraddice entrambi.",
     "Ti dico una regola da tenere per vera d'ora in poi: ogni animale che vive "
     "nell'acqua e' un pesce.\n"
     "Supponi inoltre che le balene vivano nell'acqua.\n\n"
     "Una balena e' un pesce? Elenca quali livelli hai dovuto tenere insieme per "
     "rispondere e come li hai distinti."),

    ("capitolato",
     "La domanda di progetto, posta come tale.",
     "Sto costruendo un motore di inferenza simbolico (non un LLM: tabelle e "
     "unificazione). Deve rispondere a domande ipotetiche del tipo \"se tutti i "
     "gatti sono pesci e tom e' un gatto, tom e' un pesce?\" SENZA che la sua "
     "conoscenza del mondo interferisca.\n\n"
     "Ho due architetture possibili:\n"
     "A) RINOMINARE: i termini dell'ipotesi diventano simboli freschi "
     "(gatto -> gatto_h1) che nessun fatto esistente menziona. Il mondo e' chiuso "
     "per costruzione, la base di conoscenza resta intera, ma il conflitto fra "
     "l'ipotesi e cio' che si sa DIVENTA INVISIBILE.\n"
     "B) SOSPENDERE: i fatti dell'ipotesi entrano nella stessa base con una "
     "provenienza \"ipotetico\", e il risolutore preferisce quel livello. Il "
     "conflitto resta visibile e si puo' dichiarare, ma ogni passo di inferenza "
     "deve sapere a che livello sta lavorando.\n\n"
     "Quale delle due sceglieresti, e soprattutto: quale CAPACITA' si perde "
     "scegliendo l'altra? Conciso e concreto."),

    ("cosa-fai-tu",
     "Come lo fa un ragionatore vero, descritto da lui.",
     "Quando ragioni su una premessa che sai falsa (\"supponi che il cielo sia "
     "verde\"), che cosa succede alla tua conoscenza del cielo mentre lo fai? "
     "La metti da parte, la tieni accanto, o la sostituisci? E come fai a non "
     "\"restare\" nella supposizione quando la conversazione va avanti?"),
]


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
        print("coexistence_probe: OPENCODE_API_KEY non impostata", file=sys.stderr)
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
