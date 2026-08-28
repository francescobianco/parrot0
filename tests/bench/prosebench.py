#!/usr/bin/env python3
"""prosebench — quanto bene parrot0 estrae conoscenza da PROSA VERA.

Perche' serve. L'abilita' di apprendere da Wikipedia a runtime vale esattamente
quanto l'estrazione e' precisa: una KB che cresce di fatti malformati non e' una
KB piu' grande, e' una KB peggiore. Ma finora l'estrazione era misurata solo dai
.p0t su `## Extract` scritti a mano in frasi gia' semplificate ("Paris is the
capital of France"), cioe' su prosa preparata per essere estraibile. Le pagine
del corpus contengono anche il lead REALE di Wikipedia, e li' il comportamento e'
un altro. Questo bench misura quello.

Come misura, senza oracolo e senza LLM (deterministico, offline, ripetibile —
stessa disciplina di `make llmscore-probe`: metro oggettivo, nessun giudice).
Non pretende di giudicare la VERITA' di un fatto: verifica una condizione
NECESSARIA perche' un fatto sia conoscenza, cioe' che i suoi atomi siano concetti
e non frasi. Un atomo e' MALFORMATO quando:

  * e' lungo piu' di MAX_ATOM_WORDS parole unite da underscore
    -> `astronomical_body_so_compact_that_its_gravity_prevents_anything_
       including_light` non e' un predicato, e' una relativa inghiottita intera;
  * oppure contiene al proprio interno un marcatore di subordinata o una parola
    funzionale (that, which, who, of, for, with, ...)
    -> l'atomo ha attraversato un confine sintattico che doveva chiuderlo.

Un fatto e' USABILE se nessuno dei suoi atomi e' malformato. Le due metriche:

    malformed rate   = fatti malformati / fatti estratti     (da minimizzare)
    usable per page  = fatti usabili / pagine lette          (da massimizzare)

Nota importante sul senso: alzare il solo "usable per page" estraendo di piu' non
e' un progresso se il malformed rate sale con lui. Il bersaglio e' estrarre MENO
spazzatura, non piu' roba.

    python3 tests/bench/prosebench.py [--pages N] [--verbose] [--json FILE]
"""
import argparse, json, os, re, subprocess, sys

PAGES_DIR = "kb/learning/pages"
BIN = "./bin/parrot0"

MAX_ATOM_WORDS = 3          # oltre questo, l'atomo ha inghiottito una frase
# Parole che NON possono stare DENTRO un concetto: se compaiono come componente
# di un atomo, l'atomo ha superato un confine sintattico. Sono deliberatamente
# poche e indiscutibili — il bench misura, non interpreta.
CLAUSE_MARKERS = {
    "that", "which", "who", "whom", "whose", "because", "although", "while",
    "of", "for", "with", "from", "into", "about", "over", "under",
    "is", "are", "was", "were", "has", "have", "and", "or", "but",
}

FACT_RE = re.compile(r"I extracted \d+ facts?: (.+?)\.?$")
ATOM_RE = re.compile(r"([a-z0-9_]+)")


def page_names():
    out = []
    for f in sorted(os.listdir(PAGES_DIR)):
        if f.endswith(".md"):
            out.append(f[:-3])
    return out


def split_facts(blob):
    """Split 'a(x), b(y, z)' into ['a(x)', 'b(y, z)'] — commas inside parens
    do not separate facts."""
    facts, depth, cur = [], 0, ""
    for ch in blob:
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
        if ch == "," and depth == 0:
            if cur.strip():
                facts.append(cur.strip())
            cur = ""
            continue
        cur += ch
    if cur.strip():
        facts.append(cur.strip())
    return facts


def atoms_of(fact):
    """Predicate name plus each argument, as raw atoms."""
    m = re.match(r"([a-z0-9_]+)\s*\((.*)\)\s*$", fact)
    if not m:
        return [fact]
    out = [m.group(1)]
    depth, cur = 0, ""
    for ch in m.group(2):
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
        if ch == "," and depth == 0:
            out.append(cur.strip()); cur = ""; continue
        cur += ch
    if cur.strip():
        out.append(cur.strip())
    return out


def atom_malformed(atom):
    parts = [p for p in atom.split("_") if p]
    if len(parts) > MAX_ATOM_WORDS:
        return f"{len(parts)} parole"
    for p in parts:
        if p in CLAUSE_MARKERS:
            return f"contiene '{p}'"
    return None


def run(pages, verbose):
    script = "".join(f"read the page on {p}\n" for p in pages) + "/quit\n"
    # Tetto duro: se sfora, e' un difetto da guardare, non un'attesa da subire.
    proc = subprocess.run([BIN], input=script, capture_output=True, text=True,
                          timeout=60)
    lines = proc.stdout.splitlines() + proc.stderr.splitlines()

    read_pages = 0
    facts_total, malformed_total, usable_total, rejected_total = 0, 0, 0, 0
    detail = []
    for line in lines:
        line = line.strip()
        if line.startswith("you> "):
            line = line[5:].strip()
        m = FACT_RE.search(line)
        if not m:
            continue
        read_pages += 1
        for fact in split_facts(m.group(1)):
            # gen382: un "Scartato:" NON e' un fatto malformato estratto — e' il
            # cancello che ha fatto il suo lavoro. Contarlo come malformato
            # premia il silenzio e punisce l'onesta': un sistema che respinge
            # sembrerebbe peggio di uno che tace.
            if fact.startswith("Scartato"):
                rejected_total += 1
                detail.append(("respinto", fact, ""))
                continue
            facts_total += 1
            bad = None
            for a in atoms_of(fact):
                why = atom_malformed(a)
                if why:
                    bad = (a, why); break
            if bad:
                malformed_total += 1
                detail.append(("MALFORMED", fact, f"{bad[0]} ({bad[1]})"))
            else:
                usable_total += 1
                detail.append(("usable", fact, ""))
    return read_pages, facts_total, malformed_total, usable_total, rejected_total, detail


def main():
    ap = argparse.ArgumentParser()
    # Limitato PER DEFAULT: una misura deve dare il segnale in pochi secondi ed
    # essere interrompibile. Le cose emergono presto — un campione di 12 pagine
    # muove le stesse cifre di 49, e chi vuole il giro intero lo chiede
    # esplicitamente con --pages 0.
    ap.add_argument("--pages", type=int, default=12,
                    help="quante pagine (default 12; 0 = tutte)")
    ap.add_argument("--verbose", action="store_true")
    ap.add_argument("--json", default=None)
    a = ap.parse_args()

    pages = page_names()
    if a.pages:
        pages = pages[:a.pages]
    if not pages:
        print("prosebench: nessuna pagina in " + PAGES_DIR, file=sys.stderr)
        return 2

    rp, ft, mf, us, rej, detail = run(pages, a.verbose)
    rate = (mf / ft * 100.0) if ft else 0.0
    per_page = (us / len(pages)) if pages else 0.0

    if a.verbose:
        for kind, fact, why in detail:
            print(f"  {kind:9s} {fact}" + (f"   <- {why}" if why else ""))
        print("-" * 72)
    print(f"pagine offerte     {len(pages)}")
    print(f"pagine con fatti   {rp}")
    print(f"fatti estratti     {ft}")
    print(f"  malformati       {mf}")
    print(f"  usabili          {us}")
    print(f"respinti dal gate  {rej}      (il cancello ha lavorato, non e' spreco)")
    print(f"MALFORMED RATE     {rate:.1f}%      (da minimizzare)")
    print(f"USABLE PER PAGE    {per_page:.2f}      (da massimizzare)")

    if a.json:
        with open(a.json, "w") as fh:
            json.dump({"pages_offered": len(pages), "pages_with_facts": rp,
                       "facts": ft, "malformed": mf, "usable": us,
                       "malformed_rate": rate, "usable_per_page": per_page},
                      fh, indent=2)
    return 0


if __name__ == "__main__":
    sys.exit(main())
