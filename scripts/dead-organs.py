#!/usr/bin/env python3
"""dead-organs — quali motori sono costruiti e mai collegati.

Perche' esiste. Tre volte in quattro giorni parrot0 ha avuto un organo scritto,
documentato, provato in laboratorio e MAI COLLEGATO in produzione:

    gen490  frame_pattern/4, semantic_entity/1, semantic_class/1, phrase_form/4
            -> zero fatti in tutta la KB: il lettore strutturale universale non
               aveva mai letto niente
    gen492  representation_bridge/4
            -> zero fatti: nessuna query attraversava nessun confine
    gen491  compensation_* (dal gen442)
            -> lo strato dichiarativo completo, senza esecutore

In tutti e tre i casi il guadagno e' arrivato da una manciata di fatti, non da
codice nuovo. E nessuna misura di rendimento li avrebbe trovati, perche' **un
organo spento non compare in nessun profilo**: non e' lento, non e' rotto, non
sbaglia. Semplicemente non c'e'.

Che cosa misura: predicati che compaiono nel CORPO di una regola e per cui la KB
non ha ne' fatti ne' regole che li derivino.

⚠ IL NUMERO GREZZO SOVRASTIMA, e va letto sapendolo: molti di questi predicati
sono asseriti a RUNTIME — lo stato del turno, i referenti del discorso, le
osservazioni del codice — e la loro assenza a riposo e' corretta. Questa sonda
non da' un verdetto: da' una LISTA DA TRIAGGIARE, e il triage e' la domanda
«questo chi lo scrive, e quando?». Se la risposta e' «nessuno», l'organo e'
spento.

    python3 scripts/dead-organs.py [--all]
"""
import re, sys, glob, collections

BUILTIN = {
    "is","lt","le","gt","ge","eq","ne","dif","call","naf","not","findall",
    "apply","chars","concat_atoms","app","append_list","cons","nil","assert",
    "machinery","retract",
}

def main():
    heads, bodies = set(), set()
    facts = collections.Counter()
    where = {}
    for f in glob.glob("kb/**/*.p0", recursive=True):
        for line in open(f, encoding="utf-8", errors="replace"):
            t = line.strip()
            if not t or t.startswith("%"):
                continue
            m = re.match(r"^([a-z][a-z0-9_]*)\(", t)
            if not m:
                continue
            if ":-" in t:
                heads.add(m.group(1))
                for b in re.findall(r"([a-z][a-z0-9_]*)\(", t.split(":-", 1)[1]):
                    bodies.add(b)
                    where.setdefault(b, f)
            else:
                facts[m.group(1)] += 1
    dead = sorted(p for p in bodies
                  if p not in BUILTIN and p not in heads and facts[p] == 0)
    print(f"predicati citati da una regola, senza fatti e senza regole: {len(dead)}")
    print("(molti sono asseriti a runtime: la domanda del triage e'")
    print(" «questo chi lo scrive, e quando?» — se nessuno, l'organo e' spento)\n")
    limit = len(dead) if "--all" in sys.argv else 40
    for p in dead[:limit]:
        print(f"  {p:<34} citato in {where.get(p, '?')}")
    if limit < len(dead):
        print(f"\n  … e altri {len(dead) - limit} (--all per vederli tutti)")

if __name__ == "__main__":
    sys.exit(main())
