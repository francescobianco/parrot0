#!/usr/bin/env python3
"""useful-probe.py — il banco dell'utilita' (docs/plans/assistente-utile.md).

Ogni arco in una sessione pulita di `parrot0 --mcp-engine` con la KB viva, come
`make chat` (lingua non forzata, profilo agi) ma senza rete; niente viene salvato. Stampa le risposte verbatim: il giudizio (scheda §2, rubrica §3)
lo da' chi supervisiona il giro, perche' «utile» non si decide con una regex.

Uso:  scripts/useful-probe.py [--use base|held|contrast|all] [--class C] [--json OUT]
"""
import argparse, concurrent.futures as cf, importlib.util, json, os, time

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
spec = importlib.util.spec_from_file_location("sq", os.path.join(REPO, "scripts/self-questions.py"))
sq = importlib.util.module_from_spec(spec)
spec.loader.exec_module(sq)


def arcs(path, use, klass):
    for line in open(path, encoding="utf-8"):
        if not line.strip() or line.startswith("#"):
            continue
        c, u, turns = line.rstrip("\n").split("\t", 2)
        if (use == "all" or u == use) and (not klass or c == klass):
            yield c, u, [t.strip() for t in turns.split(" || ")]


def run(arc):
    c, u, turns = arc
    e = sq.Engine(REPO, lang=None)
    out = []
    try:
        for t in turns:
            t0 = time.time()
            try:
                r = e.respond(t, timeout=90)
            except Exception as ex:
                r = "<<%s>>" % ex
            out.append({"input": t, "output": r, "seconds": round(time.time() - t0, 1)})
    finally:
        e.close()
    return {"class": c, "use": u, "turns": out}


def boot_errors():
    """La sonda parla col motore via MCP e scarta stderr: un PARSE ERROR del
    caricamento (una regola oltre i goal ammessi, un predicato oltre gli
    argomenti) resterebbe invisibile. Si guarda il boot una volta, prima."""
    import subprocess
    env = dict(os.environ, PARROT0_SESSION="", PARROT0_WIKI_FETCH="0",
               PARROT0_PROFILE="kb/profiles/agi.p0")
    p = subprocess.run([os.path.join(REPO, "bin/parrot0")], input="/quit\n", cwd=REPO,
                       env=env, capture_output=True, text=True, timeout=120)
    return [l for l in (p.stdout + p.stderr).splitlines() if "PARSE ERROR" in l]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--prompts", default=os.path.join(REPO, "tests/fixtures/usefulness/prompts.tsv"))
    ap.add_argument("--use", default="base")
    ap.add_argument("--class", dest="klass")
    ap.add_argument("--json")
    ap.add_argument("-j", type=int, default=4)
    a = ap.parse_args()
    errs = boot_errors()
    for e in errs:
        print("BOOT", e)
    if errs:
        raise SystemExit("the KB does not load cleanly: fix the PARSE ERRORs first")
    with cf.ThreadPoolExecutor(a.j) as ex:
        res = list(ex.map(run, list(arcs(a.prompts, a.use, a.klass))))
    for r in res:
        for t in r["turns"]:
            print("[%s/%s] (%ss)\n  > %s\n  < %s\n" % (r["class"], r["use"], t["seconds"], t["input"], t["output"]))
    if a.json:
        json.dump(res, open(a.json, "w"), indent=1, ensure_ascii=False)


if __name__ == "__main__":
    main()
