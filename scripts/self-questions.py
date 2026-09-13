#!/usr/bin/env python3
"""self-questions.py — IL TESTO COME SUPERVISORE (esperimento E1, H1+H2).

Piano: docs/plans/autoaddestramento-dalla-prosa.md §3.

parrot0 legge un testo; dai fatti che ha scritto (`fact_source/3`, con la frase
sorgente dentro il testo) si generano tre domande per fatto `R(a, b)`:

  T1  sull'oggetto          atteso: b
  T2  sul soggetto          atteso: a
  R1  T1 a ruoli invertiti  atteso: MURO — se risponde con a, e' una bugia di verso

La risposta attesa e' nota perche' il fatto viene dal testo: nessun oracolo.
Le domande nascono qui, da una grammatica inglese minima: e' uno STRUMENTO di
progetto come prose-probe.sh. Se l'ipotesi regge, le forme migrano in KB.

Uso:
  scripts/self-questions.py TESTO.txt [--repo DIR] [--json OUT.json]
"""
import argparse
import json
import os
import re
import select
import subprocess
import sys
import time

PARTICLES = {"of", "by", "from", "at", "as", "in", "for", "to", "into", "on",
             "with", "since", "together", "up", "down"}
SYMMETRIC = {"borders", "sibling_of", "spouse_of"}
SKIP_PREDS = {"relation_verb", "is_unit", "construction_frame", "fact_source",
              "reading_fact", "entity_role", "class_surface", "verb_particle",
              "attenuated_reading", "fact_mention", "utterance"}
WALL = ["i don't", "hmm,", "not sure", "don't understand", "i can't",
        "no module", "didn't quite", "i looked up", "want me to", "network is off",
        "nothing i hold", "i have no definition"]
STOP = {"the", "a", "an", "of", "and", "or", "to", "in", "on", "for", "by",
        "at", "as", "is", "are", "with", "from", "that", "this", "all", "most",
        "some", "least", "less", "than", "percent", "about", "other"}


class Engine:
    def __init__(self, repo):
        env = dict(os.environ, PARROT0_SESSION="", PARROT0_WIKI_FETCH="0",
                   PARROT0_TOOLS="1", PARROT0_LANG="en",
                   PARROT0_PROFILE="kb/profiles/agi.p0")
        self.p = subprocess.Popen([os.path.join(repo, "bin/parrot0"), "--mcp-engine"],
                                  cwd=repo, env=env, stdin=subprocess.PIPE,
                                  stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
                                  text=True, bufsize=1)
        self.n = 0
        self.rpc("initialize", {})

    def rpc(self, method, params, timeout=300):
        self.n += 1
        self.p.stdin.write(json.dumps({"jsonrpc": "2.0", "id": self.n,
                                       "method": method, "params": params}) + "\n")
        self.p.stdin.flush()
        end = time.time() + timeout
        while time.time() < end:
            r, _, _ = select.select([self.p.stdout], [], [], 1.0)
            if not r:
                if self.p.poll() is not None:
                    raise RuntimeError("engine exited")
                continue
            line = self.p.stdout.readline()
            if not line:
                raise RuntimeError("engine closed")
            try:
                msg = json.loads(line)
            except ValueError:
                continue
            if msg.get("id") == self.n:
                return msg
        raise TimeoutError(method)

    def call(self, tool, args, timeout=300):
        msg = self.rpc("tools/call", {"name": tool, "arguments": args}, timeout)
        text = msg["result"]["content"][0]["text"]
        return json.loads(text)

    def respond(self, text, timeout=300):
        return self.call("gen.respond", {"input": text}, timeout).get("output", "")

    def match(self, pred, args, offset=None, limit=None):
        q = {"pred": pred, "args": args}
        if offset is not None:
            q["offset"] = offset
        if limit is not None:
            q["limit"] = limit
        return self.call("kb.match", q)

    def match_tail(self, pred, args, tail=400):
        total = self.match(pred, args, 0, 1).get("total", 0)
        start = max(0, total - tail)
        out = []
        while start < total:
            got = self.match(pred, args, start, 200).get("bindings", [])
            if not got:
                break
            out += got
            start += len(got)
        return out

    def close(self):
        try:
            self.p.stdin.close()
            self.p.wait(timeout=10)
        except Exception:
            self.p.kill()


def words(s):
    # il punto resta dentro un numero («0.1»), non in coda a una parola
    return [w.strip(".-") for w in re.findall(r"[a-z0-9$%.\-]+", s.lower()) if w.strip(".-")]


def lemma(w):
    return w[:-1] if len(w) > 3 and w.endswith("s") and not w.endswith("ss") else w


def content(s):
    return {lemma(w) for w in words(s.replace("_", " "))
            if w not in STOP and (len(w) > 2 or any(c.isdigit() for c in w))}


def parse_fact(repr_):
    m = re.match(r"^([a-z0-9_]+)\((.*)\)$", repr_.strip())
    if not m:
        return None
    pred, inner = m.group(1), m.group(2)
    parts, depth, cur = [], 0, ""
    for ch in inner:
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
        if ch == "," and depth == 0:
            parts.append(cur.strip())
            cur = ""
        else:
            cur += ch
    parts.append(cur.strip())
    return pred, parts


def plural(np):
    last = np.split()[-1] if np.split() else np
    return last.endswith("s") and not last.endswith("ss")


def phrase(atom):
    return atom.strip('"').replace("_", " ")


GENERIC_CUES = {"what is", "what are", "is", "are", "has", "have", "what do", "tell me about"}
IRREGULAR_PARTICIPLES = {"built", "held", "known", "found", "made", "given", "taken", "done", "seen"}


def english_word(w):
    return bool(re.fullmatch(r"[a-z\-]+", w))


def kb_surface(eng, pred):
    """La superficie la dice la KB (`answer_frame/2`) quando il nome del predicato
    non e' gia' una forma inglese (`habitat` -> «lives in»). Cue brevi, con un
    verbo finito in testa e una particella in coda; mai gli opener generici."""
    if "_" in pred or pred.endswith("s") or pred.endswith("e") or pred.endswith("ect"):
        return None
    try:
        cues = eng.match("answer_frame", [None, pred], 0, 64).get("bindings", [])
    except Exception:
        return None
    good = []
    for c in cues:
        c = c.strip('"')
        ws = c.split()
        if not ws or c in GENERIC_CUES or len(ws) > 4 or not all(english_word(w) for w in ws):
            continue
        if ws[0].endswith("s") and ws[-1] in PARTICLES:
            good.append(c)
    return min(good, key=len) if good else None


def verb_base(v):
    if v.endswith("ies") and len(v) > 4:
        return v[:-3] + "y"
    if v.endswith(("sses", "shes", "ches", "xes")):
        return v[:-2]
    if v.endswith("s") and not v.endswith("ss") and len(v) > 3:
        return v[:-1]
    return v


def third_person(base):
    if base.endswith("y") and len(base) > 2 and base[-2] not in "aeiou":
        return base[:-1] + "ies"
    return base + ("es" if base.endswith(("s", "sh", "ch", "x")) else "s")


def questions(pred, a, b, surface=None):
    """T1, T2, R1 — una grammatica inglese minima, dichiarata come strumento."""
    ws = (surface or pred).replace(" ", "_").split("_")
    A, B = phrase(a), phrase(b)
    head = ws[0]
    participle = head.endswith("ed") or head in IRREGULAR_PARTICIPLES
    copular = ws[-1] in PARTICLES and (participle or len(ws) == 2 and not head.endswith("s")
                                       and head not in ("live", "belong", "grow", "cluster", "deliver"))
    if copular or pred.endswith("_of"):
        surf = " ".join(ws)
        t1 = f"what {'are' if plural(A) else 'is'} {A} {surf}?"
        t2 = f"what {'are' if plural(B) else 'is'} {surf} {B}?"
        r1 = f"what {'are' if plural(B) else 'is'} {B} {surf}?"
        return t1, t2, r1
    base = verb_base(head)
    tail = (" " + " ".join(ws[1:])) if len(ws) > 1 else ""
    t1 = f"what {'do' if plural(A) else 'does'} {A} {base}{tail}?"
    t2 = f"what {third_person(base)}{tail} {B}?"
    r1 = f"what {'do' if plural(B) else 'does'} {B} {base}{tail}?"
    return t1, t2, r1


def is_wall(reply):
    r = reply.lower()
    return not r.strip() or any(m in r for m in WALL)


def classify(reply, expected, other):
    if is_wall(reply):
        return "muro"
    got = content(reply)
    if got & content(expected):
        return "ok"
    if other and got & content(other):
        return "bugia"
    return "altro"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("text")
    ap.add_argument("--repo", default=os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    ap.add_argument("--json")
    opt = ap.parse_args()
    text = open(opt.text).read().strip()
    text_words = set(words(text))

    eng = Engine(opt.repo)
    t0 = time.time()
    reading = eng.respond(text, timeout=900)
    facts = []
    seen = set()
    for rep in eng.match_tail("fact_source", [None, None, None]):
        parsed = parse_fact(rep)
        if not parsed or rep in seen:
            continue
        pred, args = parsed
        if pred in SKIP_PREDS or len(args) != 2:
            continue
        subs = eng.match("fact_source", [rep, None, None]).get("bindings", [])
        raw = ""
        if subs:
            raws = eng.match("fact_source", [rep, subs[0], None]).get("bindings", [])
            raw = raws[0].strip('"') if raws else ""
        rw = [w for w in words(raw) if w not in STOP]
        if not rw or sum(1 for w in rw if w in text_words) < 0.7 * len(rw):
            continue
        seen.add(rep)
        facts.append({"fact": rep, "pred": pred, "a": args[0], "b": args[1], "source": raw})

    results = []
    surfaces = {}
    for f in facts:
        if f["pred"] not in surfaces:
            surfaces[f["pred"]] = kb_surface(eng, f["pred"])
        f["surface"] = surfaces[f["pred"]]
        t1, t2, r1 = questions(f["pred"], f["a"], f["b"], f["surface"])
        row = dict(f)
        a1 = eng.respond(t1)
        row["T1"] = {"q": t1, "reply": a1, "esito": classify(a1, f["b"], f["a"])}
        a2 = eng.respond(t2)
        subjects = " ".join(g["a"] for g in facts if g["pred"] == f["pred"] and g["b"] == f["b"])
        row["T2"] = {"q": t2, "reply": a2, "esito": classify(a2, subjects, f["b"])}
        if f["pred"] in SYMMETRIC:
            row["R1"] = {"q": r1, "reply": "", "esito": "simmetrica"}
        else:
            a3 = eng.respond(r1)
            e3 = "muro" if is_wall(a3) else ("bugia" if content(a3) & content(f["a"]) else "altro")
            row["R1"] = {"q": r1, "reply": a3, "esito": e3}
        results.append(row)
    eng.close()

    def count(key, val):
        return sum(1 for r in results if r[key]["esito"] == val)

    n = len(results)
    summary = {
        "text": opt.text, "repo": opt.repo, "facts": n,
        "T1_ok": count("T1", "ok"), "T1_muro": count("T1", "muro"), "T1_altro": count("T1", "altro"),
        "T1_bugia": count("T1", "bugia"),
        "T2_ok": count("T2", "ok"), "T2_muro": count("T2", "muro"), "T2_altro": count("T2", "altro"),
        "T2_bugia": count("T2", "bugia"),
        "R1_bugia": count("R1", "bugia"), "R1_altro": count("R1", "altro"), "R1_muro": count("R1", "muro"),
        "seconds": round(time.time() - t0, 1),
    }
    print(json.dumps(summary, indent=1))
    for r in results:
        flags = [k + ":" + r[k]["esito"] for k in ("T1", "T2", "R1") if r[k]["esito"] not in ("ok", "muro") or k != "R1"]
        print(f"- {r['fact']}")
        for k in ("T1", "T2", "R1"):
            print(f"    {k} {r[k]['esito']:6} {r[k]['q']}  ->  {r[k]['reply'][:90]}")
    if opt.json:
        with open(opt.json, "w") as fh:
            json.dump({"summary": summary, "reading": reading, "results": results}, fh, indent=1)


if __name__ == "__main__":
    main()
