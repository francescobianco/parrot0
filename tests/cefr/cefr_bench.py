#!/usr/bin/env python3
"""cefr-bench — che cosa parrot0 sa fare, per livello CEFR.

Dati: CEFR-SP (Arase, Uchida, Kajiwara). Attribuzione e licenze in
tests/cefr/ATTRIBUTION.md — vanno lette prima di usare o ridistribuire i dati.

⛔ NON misura «quanto bene parrot0 parla inglese», e la ragione sta nei dati:
CEFR-SP annota la DIFFICOLTA' di una frase, non la sua correttezza. Da un corpus
di difficolta' non si ricava un voto di competenza senza commettere un errore di
categoria.

Misura invece due cose oneste, entrambe stratificate per livello:

  LETTURA   la frase produce una risposta, o un muro?
  GIUDIZIO  parrot0 trova una regola che decida la frase, o rifiuta onestamente?

Il risultato e' una CURVA per banda, non un numero: «legge il 90% di A1 e il 40%
di C1» dice dove intervenire; «6,3 su 10» non dice niente. E la curva e' anche il
curriculum: si insegna in ordine di livello e si guarda dove si muove.

⚠ I due annotatori concordano nel 41,4% dei casi. Il livello di una frase e'
quindi una BANDA [min(A,B), max(A,B)], non un punto: qui si stratifica sulla
banda, e le frasi su cui gli esperti non concordano non vengono contate come
fallimenti di nessuno.
"""
import argparse, collections, os, random, subprocess, sys

LV = {1: "A1", 2: "A2", 3: "B1", 4: "B2", 5: "C1", 6: "C2"}
HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))


def load(paths):
    rows = []
    for p in paths:
        if not os.path.exists(p):
            continue
        for line in open(p, encoding="utf-8"):
            f = line.rstrip("\n").split("\t")
            if len(f) < 3:
                continue
            try:
                a, b = int(f[1]), int(f[2])
            except ValueError:
                continue
            s = f[0].strip()
            if s:
                rows.append((s, min(a, b), max(a, b)))
    return rows


def ask(lines, thinking=False):
    """Un solo processo per lotto: il boot costa piu' delle domande."""
    env = dict(os.environ, PARROT0_PROFILE="kb/profiles/agi.p0")
    if thinking:
        env["PARROT0_THINKING"] = "1"
    p = subprocess.run([os.path.join(ROOT, "bin", "parrot0")],
                       input="\n".join(lines) + "\n", env=env, cwd=ROOT,
                       capture_output=True, text=True, timeout=900)
    out = [l for l in p.stdout.splitlines() if l.strip()]
    # la prima riga utile arriva dopo il banner: si allinea dalla coda
    return out[-len(lines):] if len(out) >= len(lines) else out


WALL = ("i don't know", "i do not know", "non capisco", "non lo so",
        "i don't understand", "i'm not sure", "want me to look it up",
        "hmm, i don't know", "i can't", "i cannot", "non sono sicuro",
        "non riesco", "vuoi che")
# ⚠ Le classi devono conoscere le rese di ENTRAMBE le lingue, altrimenti il giro
# italiano finisce tutto in «fuori tema» e la colonna misura il classificatore
# invece del motore. E' successo alla prima esecuzione: 48 su 48.
REFUSED = ("don't have a rule", "won't guess",
           "non ho una regola", "non tiro a indovinare")
FOUND   = ("cannot go together", "should be",
           "non possono stare insieme", "forma attesa")
OK      = ("agrees", "well formed", "l'accordo c'è", "e' ben formata",
           "è ben formata")


def classify_read(r):
    low = r.lower()
    return "muro" if any(w in low for w in WALL) else "risposta"


def classify_judge(r):
    low = r.lower()
    if any(w in low for w in REFUSED):
        return "rifiuto onesto"
    # ⚠ Un MURO non e' una risposta fuori tema, ed e' la distinzione che conta:
    # «non capisco» e' onesto, «Londoner.» a una domanda di grammatica no. La
    # prima esecuzione italiana le confondeva e riportava 48 «fuori tema» dove
    # erano 48 muri — cioe' accusava il motore della colpa sbagliata.
    if any(w in low for w in WALL):
        return "muro"
    if any(w in low for w in FOUND):
        return "errore trovato"
    if any(w in low for w in OK):
        return "accordo verificato"
    return "fuori tema"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--per-band", type=int, default=25,
                    help="frasi per banda CEFR (default 25)")
    ap.add_argument("--seed", type=int, default=7)
    ap.add_argument("--split", default="test,dev",
                    help="quali file usare: train,test,dev")
    ap.add_argument("--lang", default="en",
                    help="en, it, oppure both — lo specchio italiano e' un "
                         "campione tradotto: vedi data/it/PROVENANCE.md")
    args = ap.parse_args()

    langs = ["en", "it"] if args.lang == "both" else [args.lang]
    for lang in langs:
        run_one(lang, args)


def paths_for(lang, split):
    d = os.path.join(HERE, "data", lang)
    if lang == "it":
        return [os.path.join(d, "wikiauto_mirror.tsv")]
    return ([os.path.join(d, "wikiauto_%s.tsv" % s) for s in split] +
            [os.path.join(d, "score_%s.tsv" % s) for s in split])


def run_one(lang, args):
    rows = load(paths_for(lang, args.split.split(",")))
    if not rows:
        sys.exit("nessun dato per «%s»: vedi tests/cefr/ATTRIBUTION.md" % lang)

    by = collections.defaultdict(list)
    for s, lo, hi in rows:
        by[lo].append((s, lo, hi))
    random.seed(args.seed)

    print("cefr-bench [%s] — dati: CEFR-SP (Arase, Uchida, Kajiwara, EMNLP 2022)" % lang)
    print("            attribuzione e licenze: tests/cefr/ATTRIBUTION.md")
    print("            frasi caricate: %d" % len(rows))
    if lang == "it":
        print("            ⚠ specchio TRADOTTO, etichette EREDITATE dall'inglese:")
        print("              non sono un'annotazione CEFR dell'italiano.")
        print("              vedi data/it/PROVENANCE.md")
    print()
    hdr = ("%-5s %6s | %-9s %-6s | %-15s %-15s %-15s %-7s %s"
           % ("banda", "frasi", "lette", "%", "accordo verif.",
              "errore trovato", "rifiuto onesto", "muro", "fuori tema"))
    print(hdr); print("-" * len(hdr))

    tot_read = tot_n = 0
    grand = collections.Counter()
    for lv in sorted(by):
        pick = by[lv][:]
        random.shuffle(pick)
        pick = pick[:args.per_band]
        if not pick:
            continue
        sents = [s for s, _, _ in pick]
        reads = ask(sents)
        cue = "dove è l'errore qui: " if lang == "it" else "where is the error here: "
        judges = ask([cue + s for s in sents])
        rc = collections.Counter(classify_read(r) for r in reads)
        jc = collections.Counter(classify_judge(r) for r in judges)
        grand.update(jc)
        n = len(pick)
        tot_read += rc["risposta"]; tot_n += n
        print("%-5s %6d | %-9d %-6.0f | %-15d %-15d %-15d %-7d %d"
              % (LV[lv], n, rc["risposta"], 100.0 * rc["risposta"] / n,
                 jc["accordo verificato"], jc["errore trovato"],
                 jc["rifiuto onesto"], jc["muro"], jc["fuori tema"]))

    print()
    print("LETTURA   %d/%d (%.0f%%) delle frasi ricevono una risposta, non un muro"
          % (tot_read, tot_n, 100.0 * tot_read / max(tot_n, 1)))
    dec = grand["accordo verificato"] + grand["errore trovato"]
    print("GIUDIZIO  %d decise da una regola, %d rifiutate onestamente, %d a muro, %d fuori tema"
          % (dec, grand["rifiuto onesto"], grand["muro"], grand["fuori tema"]))
    print()
    print("Le «fuori tema» sono il debito vero: una risposta che non risponde e'")
    print("peggio di un muro. Le «rifiutate» non sono un fallimento — sono la")
    print("misura di quante regole grammaticali mancano ancora.")
    if lang != "en":
        print()
        print("⚠ QUESTO PUNTEGGIO NON E' VERITIERO, ED E' COMUNQUE UTILE.")
        print("  Le frasi sono TRADOTTE e le etichette EREDITATE dall'inglese: la")
        print("  banda dice «in inglese era di livello X», non «in italiano è di")
        print("  livello X». Il numero non misura quindi la competenza in italiano.")
        print("  Serve come INDICATORE di scostamento: a parità di frase, quanto")
        print("  cade la capacità cambiando lingua. Il valore di RIFERIMENTO — l'unico")
        print("  che si può citare come misura — resta quello INGLESE.")


if __name__ == "__main__":
    main()
