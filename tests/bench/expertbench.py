#!/usr/bin/env python3
"""expertbench — parrot0 risponde su cio' che DICHIARA di sapere?

Nasce da una chiacchierata deludente: quattro domande banali sul poker, tre muri.
Eppure il file kb/experts/games/poker.p0 conteneva gia' i ranghi delle mani, le
regole, i consigli. La conoscenza c'era; non era raggiungibile.

E' il difetto piu' insidioso di un sistema KB-first, perche' non si vede
guardando la KB: si vede solo PARLANDO, e allora e' tardi — l'ha trovato
l'utente. Questo bench lo trova prima, e in modo ripetibile.

COME FUNZIONA. Legge dalla KB stessa che cosa parrot0 dichiara di sapere —
`expert(X)`, `category_surface(S, C)`, `magnitude(Dim, Item, _)` — e per ogni
dominio dichiarato pone le FORME DI DOMANDA standard, in italiano e in inglese.
Non inventa domande: le deriva da cio' che il sistema afferma di se'. Percio' non
invecchia — un esperto nuovo entra nel bench il giorno in cui entra nella KB.

IL METRO. Non giudica la verita' della risposta (non serve un oracolo): conta i
MURI. Una dichiarazione di competenza seguita da un muro e' una promessa non
mantenuta, ed e' esattamente cio' che rende deludente una conversazione.

    wall rate per dominio   = risposte a vuoto / domande poste
    domini muti             = quelli in cui NESSUNA forma passa

    python3 tests/bench/expertbench.py [--domains N] [--verbose]
"""
import argparse, glob, re, subprocess, sys

BIN = "./bin/parrot0"
PROFILE = "kb/profiles/agi.p0"

WALL_MARKS = [
    "i don't understand", "i don't know", "non capisco", "non conosco",
    "non ho afferrato", "didn't quite catch", "i have no", "want me to",
    "vuoi che", "beyond me", "non ho una pagina", "i don't have a page",
]

# Le forme di domanda: una CLASSE di richieste, non domande scelte a mano.
# {d} = dominio, {c} = superficie di categoria, {a}/{b} = due membri.
SHAPES_DOMAIN = [
    ("means",        "what is {d}",              "che cos'è {d}"),
    ("game_play",    "how do you play {d}",      "come si gioca a {d}"),
    ("game_goal",    "what is the goal of {d}",  "qual è lo scopo di {d}"),
    ("game_players", "how many players in {d}",  "quanti giocatori servono a {d}"),
]
SHAPES_CATEGORY = [
    ("list",       "what are the {c}",               "quali sono i {c}"),
    ("list-md",    "give me a markdown list of the {c}", "fammi un elenco markdown dei {c}"),
]
SHAPES_COMPARE = [
    ("compare",    "which is stronger, {a} or {b}",  "è più forte {a} o {b}"),
]


def kb_scan():
    """Che cosa parrot0 DICHIARA di sapere, letto dai suoi stessi file.

    Anche QUALI domande abbiano senso per un dominio si legge dalla KB: si chiede
    "come si gioca" solo dove esiste game_play/2, "quanti giocatori" solo dove
    esiste game_players/2. Applicare le stesse forme a tappeto — la prima
    versione di questo bench lo faceva — produce falsi allarmi ("come si gioca a
    algebra") e un wall rate del 100% che non misura parrot0: misura il bench."""
    experts, surfaces, members, dims = [], {}, {}, {}
    declared = {}
    for f in glob.glob("kb/experts/**/*.p0", recursive=True):
        s = open(f, errors="replace").read()
        experts += re.findall(r"^expert\((\w+)\)\.", s, re.M)
        # category_surface(Superficie, Categoria): la SUPERFICIE e' la parola che
        # si dice, la categoria e' la chiave interna. Chiedere con la chiave
        # ("quali sono i backgammon_component") misura di nuovo il bench.
        for m in re.finditer(r"^category_surface\((\w+),\s*(\w+)\)\.", s, re.M):
            surfaces[m.group(1)] = m.group(2)
        for m in re.finditer(r"^category_member\((\w+),\s*(\w+)\)\.", s, re.M):
            members.setdefault(m.group(1), []).append(m.group(2))
        for m in re.finditer(r"^magnitude\((\w+),\s*(\w+),", s, re.M):
            dims.setdefault(m.group(1), []).append(m.group(2))
        for m in re.finditer(r"^(means|game_play|game_goal|game_players)\((\w+),", s, re.M):
            declared.setdefault(m.group(2), set()).add(m.group(1))
    return sorted(set(experts)), surfaces, members, dims, declared


def ask_all(lines):
    script = "".join(l + "\n" for l in lines) + "/quit\n"
    p = subprocess.run([BIN], input=script, capture_output=True, text=True,
                       timeout=120, env={"PARROT0_PROFILE": PROFILE, "PATH": "/usr/bin:/bin",
                                         "HOME": "/tmp"})
    # Il prompt ">>> " va su STDERR e la risposta su STDOUT: filtrare stdout per
    # il prompt non trova nulla e fa sembrare muto tutto. Le risposte sono le
    # righe non vuote di stdout, nell'ordine delle domande.
    return [l.strip() for l in p.stdout.splitlines() if l.strip()]


def is_wall(reply):
    low = reply.lower()
    return (not reply) or any(m in low for m in WALL_MARKS)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--domains", type=int, default=6, help="quanti esperti (0 = tutti)")
    ap.add_argument("--verbose", action="store_true")
    a = ap.parse_args()

    experts, surfaces, members, dims, declared = kb_scan()
    if a.domains:
        experts = experts[:a.domains]
    if not experts:
        print("expertbench: nessun expert/1 nei file", file=sys.stderr)
        return 2

    asked, walls, per_domain = 0, 0, {}
    for d in experts:
        qs, labels = [], []
        # solo le forme che QUESTO dominio dichiara di supportare
        for name, en, it in SHAPES_DOMAIN:
            if name not in declared.get(d, ()):
                continue
            qs.append(it.format(d=d)); labels.append((d, name))
        # la superficie da usare e' quella che il dominio dichiara per se'
        for surf in ([d] if d in surfaces else []):
            for name, en, it in SHAPES_CATEGORY:
                qs.append(it.format(c=surf)); labels.append((d, name))
            ms = members.get(surfaces[surf], [])
            if len(ms) >= 2:
                for name, en, it in SHAPES_COMPARE:
                    qs.append(it.format(a=ms[0], b=ms[-1])); labels.append((d, name))
        if not qs:
            continue
        replies = ask_all(qs)
        dw = 0
        for i, q in enumerate(qs):
            r = replies[i] if i < len(replies) else ""
            asked += 1
            if is_wall(r):
                walls += 1; dw += 1
                if a.verbose:
                    print(f"  MURO  [{labels[i][0]}/{labels[i][1]}] {q}\n        -> {r[:90]}")
            elif a.verbose:
                print(f"  ok    [{labels[i][0]}/{labels[i][1]}] {q}\n        -> {r[:90]}")
        per_domain[d] = (dw, len(qs))

    print("-" * 72)
    for d, (dw, n) in sorted(per_domain.items(), key=lambda kv: -kv[1][0]):
        flag = "  <- MUTO" if dw == n else ""
        print(f"  {d:22s} {dw}/{n} muri{flag}")
    print("-" * 72)
    print(f"domande poste   {asked}")
    print(f"muri            {walls}")
    print(f"WALL RATE       {walls * 100.0 / asked:.1f}%      (da minimizzare)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
