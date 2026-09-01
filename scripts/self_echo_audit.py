#!/usr/bin/env python3
"""L'ECO: parrot0 capisce cio' che parrot0 dice?

Non misura la correttezza — misura una CHIUSURA. Si insegna un fatto, si prende
la frase con cui parrot0 conferma di averlo imparato, e le si rimanda indietro
quella stessa frase. Se muro, la sua uscita non e' nella sua lingua d'ingresso.

Perche' e' un difetto piu' insidioso di un muro qualunque: il turno rubato che
ne nasce e' INATTRIBUIBILE su una misura del corpus, perche' sembra una frase
dell'utente come tutte le altre. Solo l'eco lo isola.

Uso:  python3 scripts/self_echo_audit.py [--lang it]
"""
import argparse, os, subprocess, sys, re

# Stimoli scelti per COPRIRE RESE DIVERSE, non per essere tanti: ogni riga deve
# far pronunciare a parrot0 una famiglia di risposta differente.
STIMULI_IT = [
    "il libro è sul tavolo",
    "il quaderno è nello zaino",
    "roma è in italia",
    "il mio libro si chiama Moby Dick",
    "una balena è un mammifero",
    "il gatto è nero",
    "parigi è la capitale della francia",
    "il ferro è un metallo",
    "marco è alto",
    "il sole è una stella",
]
STIMULI_EN = [
    "the book is on the table",
    "rome is in italy",
    "a whale is a mammal",
    "the cat is black",
    "iron is a metal",
    "the sun is a star",
]

WALL = re.compile(r"non capisco|non sono sicuro|non ho afferrato|"
                  r"i don't understand|i'm not sure|didn't catch", re.I)


def talk(binary, lines, lang, env_extra=None):
    env = dict(os.environ)
    env["PARROT0_LANG"] = lang
    if env_extra:
        env.update(env_extra)
    p = subprocess.run([binary], input="\n".join(lines) + "\n",
                       capture_output=True, text=True, env=env, timeout=120)
    # Il banner e il prompt vanno su stderr; stdout porta soltanto le risposte.
    return [l.strip() for l in p.stdout.splitlines() if l.strip()]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--lang", default="it")
    ap.add_argument("--binary", default="./bin/parrot0")
    a = ap.parse_args()
    stimuli = STIMULI_IT if a.lang == "it" else STIMULI_EN

    broken, ok, skipped = [], [], []
    for s in stimuli:
        said = talk(a.binary, [s], a.lang)
        if not said:
            skipped.append((s, "(nessuna risposta)")); continue
        reply = said[0]
        if WALL.search(reply):
            # Non e' un caso d'eco: parrot0 non ha capito lo STIMOLO. E' un muro
            # ordinario, e confonderlo con un difetto d'eco gonfierebbe la misura.
            skipped.append((s, reply)); continue
        # L'eco: la frase di parrot0 ripulita del prefisso di conferma.
        echo = re.sub(r"^(imparato|learned|ricevuto|got it|ok)\s*[:,]\s*", "",
                      reply, flags=re.I).rstrip(".")
        back = talk(a.binary, [echo], a.lang)
        got = back[0] if back else "(nessuna risposta)"
        (broken if WALL.search(got) else ok).append((s, echo, got))

    print("ECO — parrot0 capisce cio' che parrot0 dice?\n")
    print(f"lingua={a.lang}  stimoli={len(stimuli)}  "
          f"chiusi={len(ok)}  ROTTI={len(broken)}  non applicabili={len(skipped)}\n")
    if broken:
        print("⛔ L'USCITA NON RIENTRA (parrot0 non rilegge se stesso)\n")
        for s, echo, got in broken:
            print(f"  stimolo : {s}")
            print(f"  ha detto: {echo}")
            print(f"  rileggendo: {got}\n")
    if ok:
        print("✔ chiusi\n")
        for s, echo, got in ok:
            print(f"  {echo}  →  {got}")
    if skipped:
        print("\n· non applicabili (lo stimolo stesso e' un muro)\n")
        for s, r in skipped:
            print(f"  {s}  →  {r}")
    return 1 if broken else 0


if __name__ == "__main__":
    sys.exit(main())
