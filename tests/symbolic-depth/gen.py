#!/usr/bin/env python3
"""Generatore del banco di profondita' simbolica (docs/plans/long-symbolic-reasoning.md).

Ogni problema e' detto a parrot0 PARLANDO, nelle forme che la comprensione
esistente legge gia' (lezione condizionale con variabili, fatto relazionale,
domanda aperta e polare): il banco misura il percorso che c'e', non ne apre uno.
Parole senza senso per relazioni ed entita' (verificate assenti dalla KB), cosi'
nessun fatto del mondo puo' rispondere al posto della catena.

Famiglie:
  lin   catena lineare: D applicazioni di regola, ogni conclusione intermedia e'
        premessa della successiva; W-1 alternative per posizione (predecessori e
        successori morti, regole valide ma inutili).
  dag   strati di W nodi collegati tutti con tutti fra strati vicini, una sola
        radice collegata al primo strato: W^(D-1) cammini verso la fine, una sola
        risposta. Senza memoria degli stati intermedi (tabling) la ricerca
        all'indietro li visita tutti; e' la cella che misura il controllo.
  join  due rami dalla stessa radice, profondita' D-1 in totale, e una regola di
        join che vuole entrambi i risultati intermedi.

Le attese NON vengono dalla costruzione: le calcola `oracle()` per concatenazione in
avanti sulle stesse lezioni che riceve parrot0. (La prima griglia le prendeva dalla
costruzione, e un distrattore in posizione 0 era una seconda soluzione vera: parrot0 la
trovava e il banco la chiamava misclaim.)

Uscita: JSON, un problema per riga, con lezioni, domande e attese.
"""
import argparse
import json
import os
import random
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

ONSETS = ["b", "d", "f", "g", "k", "l", "m", "n", "p", "t", "v", "z", "br", "gl", "tr", "pl", "dr", "kl"]
VOWELS = ["a", "e", "i", "o", "u"]
CODAS = ["", "", "n", "m", "k", "p", "t", "l"]
# lettere-variabile, forme SMS e desinenze che la morfologia legge come flessione
BAD_END = ("s", "ed", "er", "ly", "ing", "en")


def kb_words():
    words = set()
    for dirpath, _, files in os.walk(os.path.join(ROOT, "kb")):
        for f in files:
            if f.endswith(".p0"):
                with open(os.path.join(dirpath, f), encoding="utf-8", errors="ignore") as fh:
                    words.update(re.findall(r"[a-z]+", fh.read().lower()))
    return words


class Names:
    def __init__(self, rng, taken):
        self.rng, self.taken, self.used = rng, taken, set()

    def fresh(self, syllables=2):
        for _ in range(10000):
            w = "".join(self.rng.choice(ONSETS) + self.rng.choice(VOWELS) for _ in range(syllables - 1))
            w += self.rng.choice(ONSETS) + self.rng.choice(VOWELS) + self.rng.choice(CODAS)
            if len(w) < 4 or w.endswith(BAD_END) or w in self.taken or w in self.used:
                continue
            self.used.add(w)
            return w
        raise RuntimeError("nomi finiti")


def fact(rel, a, b):
    return f"{a} is the {rel} of {b}"


def rule(head, b1, b2):
    return f"if x is the {b1} of y and y is the {b2} of z then x is the {head} of z"


FACT_RE = re.compile(r"^(\w+) is the (\w+) of (\w+)$")
RULE_RE = re.compile(r"^if (\w) is the (\w+) of (\w) and (\w) is the (\w+) of (\w) then (\w) is the (\w+) of (\w)$")


def oracle(lessons):
    """Chiusura in avanti: l'insieme di tutti i fatti derivabili, (rel, a, b)."""
    facts, rules = set(), []
    for s in lessons:
        m = FACT_RE.match(s)
        if m:
            facts.add((m.group(2), m.group(1), m.group(3)))
            continue
        m = RULE_RE.match(s)
        if not m:
            raise ValueError("lezione che l'oracolo non legge: " + s)
        v1, r1, v2, v3, r2, v4, h1, rh, h2 = m.groups()
        rules.append(((r1, v1, v2), (r2, v3, v4), (rh, h1, h2)))
    changed = True
    while changed:
        changed = False
        for (r1, a1, b1), (r2, a2, b2), (rh, ha, hb) in rules:
            for (f1, x1, y1) in [f for f in facts if f[0] == r1]:
                for (f2, x2, y2) in [f for f in facts if f[0] == r2]:
                    env = {}
                    ok = True
                    for var, val in ((a1, x1), (b1, y1), (a2, x2), (b2, y2)):
                        if env.setdefault(var, val) != val:
                            ok = False
                            break
                    if ok and ha in env and hb in env:
                        new = (rh, env[ha], env[hb])
                        if new not in facts:
                            facts.add(new)
                            changed = True
    return facts


def questions_for(lessons, rel, end, true_name, near_names, entities):
    closure = oracle(lessons)
    answers = sorted(a for (r, a, b) in closure if r == rel and b == end)
    if true_name not in answers:
        raise AssertionError(f"la costruzione non deriva {true_name}: {answers}")
    wrong = next((n for n in near_names + entities if n not in answers and n != end), None)
    return [
        {"text": f"who is the {rel} of {end}", "yes": answers,
         "no": sorted(set(entities) - set(answers) - {end})},
        {"text": f"is {true_name} the {rel} of {end}?", "yes": ["Yes"], "no": ["I don't know", "No"]},
        {"text": f"is {wrong} the {rel} of {end}?", "yes": [], "no": ["Yes"]},
    ]


def entities_of(lessons):
    out = []
    for s in lessons:
        m = FACT_RE.match(s)
        if m:
            for n in (m.group(1), m.group(3)):
                if n not in out:
                    out.append(n)
    return out


def lin(D, W, rng, names):
    base = [names.fresh() for _ in range(D + 1)]        # a1..a(D+1)
    comp = [base[0]] + [names.fresh() for _ in range(D)]  # c1=a1, c2..c(D+1)
    ent = [names.fresh(3) for _ in range(D + 2)]          # n0..n(D+1)
    rules = [rule(comp[k + 1], comp[k], base[k + 1]) for k in range(D)]
    facts = [fact(base[i], ent[i], ent[i + 1]) for i in range(D + 1)]
    decoys, near = [], None
    for i in range(D + 1):
        for w in range(W - 1):
            if w % 2 == 0:
                # predecessore morto: entra nella catena, ma nessuno lo raggiunge
                # (in posizione 0 la relazione giusta farebbe di m una seconda
                # soluzione vera, perche' c1 = a1: li' entra con la relazione dopo)
                m = names.fresh(3)
                facts.append(fact(base[i] if i else base[1], m, ent[i + 1]))
                decoys.append(m)
                if i == D:
                    near = m  # arriva alla fine con l'ultima relazione, senza la catena
            else:
                # successore morto: esce dalla catena e prosegue 1..3 passi, poi si ferma
                prev = ent[i]
                for j in range(i, min(D + 1, i + rng.randint(1, 3))):
                    m = names.fresh(3)
                    facts.append(fact(base[j], prev, m))
                    decoys.append(m)
                    prev = m
        if W > 1 and i < D:
            # regola valida ma inutile sulle stesse relazioni
            rules.append(rule(names.fresh(), base[i], base[i + 1]))
    rng.shuffle(rules)
    rng.shuffle(facts)
    lessons = rules + facts
    near_names = ([near] if near else []) + [ent[1]]
    return {
        "family": "lin", "D": D, "W": W,
        "lessons": lessons,
        "questions": questions_for(lessons, comp[D], ent[D + 1], ent[0], near_names, entities_of(lessons)),
        "min_steps": D,
        "solution": [f"{comp[k + 1]}({ent[0]}, {ent[k + 2]})" for k in range(D)],
    }


def dag(D, W, rng, names, roots=1):
    base = [names.fresh() for _ in range(D + 1)]
    comp = [base[0]] + [names.fresh() for _ in range(D)]
    end = names.fresh(3)
    root_names = [names.fresh(3) for _ in range(roots)]
    root = root_names[0]
    layers = [[names.fresh(3) for _ in range(W)] for _ in range(D)]
    rules = [rule(comp[k + 1], comp[k], base[k + 1]) for k in range(D)]
    # piu' radici: ciascuna entra nel primo strato da un nodo diverso, quindi ogni
    # radice e' una risposta vera e la domanda aperta le vuole tutte
    facts = [fact(base[0], r, layers[0][-1 - j]) for j, r in enumerate(root_names)]
    for i in range(D - 1):
        facts += [fact(base[i + 1], a, b) for a in layers[i] for b in layers[i + 1]]
    facts += [fact(base[D], a, end) for a in layers[D - 1]]
    rng.shuffle(rules)
    rng.shuffle(facts)
    lessons = rules + facts
    return {
        "family": "dag", "D": D, "W": W,
        "lessons": lessons,
        "questions": questions_for(lessons, comp[D], end, root, [layers[0][0]], entities_of(lessons)),
        "min_steps": D,
        "paths": W ** (D - 1),
        "roots": roots,
    }


def join(D, W, rng, names):
    """D = inferenze minime: (d1 - 1) + (d2 - 1) + 1 regole, rami di d1 e d2 fatti."""
    steps = D - 1
    s1 = steps // 2
    s2 = steps - s1
    root = names.fresh(3)
    lessons_r, lessons_f, heads = [], [], []
    for s in (s1, s2):
        base = [names.fresh() for _ in range(s + 1)]
        comp = [base[0]] + [names.fresh() for _ in range(s)]
        ent = [root] + [names.fresh(3) for _ in range(s + 1)]
        lessons_r += [rule(comp[k + 1], comp[k], base[k + 1]) for k in range(s)]
        lessons_f += [fact(base[i], ent[i], ent[i + 1]) for i in range(s + 1)]
        # rami morti dalla radice sulla prima relazione
        for _ in range(W - 1):
            lessons_f.append(fact(base[0], root, names.fresh(3)))
        heads.append((comp[s], ent[s + 1]))
    final = names.fresh()
    # final(Y, Z) :- q(X, Y) ... espresso con l'inverso: y is the final of z
    # se x e' la q di y e x e' la s di z. Le variabili seguono la lezione.
    (q, yend), (s_, zend) = heads
    lessons_r.append(f"if x is the {q} of y and x is the {s_} of z then y is the {final} of z")
    lessons = lessons_r + lessons_f
    rng.shuffle(lessons)
    return {
        "family": "join", "D": D, "W": W,
        "lessons": lessons,
        "questions": questions_for(lessons, final, zend, yend, [root], entities_of(lessons)),
        "min_steps": D,
    }


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--family", choices=["lin", "join", "dag"], default="lin")
    ap.add_argument("--depths", default="1,2,3,5")
    ap.add_argument("--widths", default="1")
    ap.add_argument("--n", type=int, default=1, help="problemi per cella")
    ap.add_argument("--roots", type=int, default=1, help="dag: radici, cioe' risposte vere")
    ap.add_argument("--seed", type=int, default=14092026)
    a = ap.parse_args()
    rng = random.Random(a.seed)
    names = Names(rng, kb_words())
    gen = {"lin": lin, "join": join, "dag": dag}[a.family]
    for D in map(int, a.depths.split(",")):
        for W in map(int, a.widths.split(",")):
            if a.family == "join" and D < 3:
                continue
            for k in range(a.n):
                p = gen(D, W, rng, names, a.roots) if a.family == "dag" else gen(D, W, rng, names)
                p["id"] = f"{a.family}-D{D}-W{W}-{k}"
                print(json.dumps(p))


if __name__ == "__main__":
    sys.exit(main())
