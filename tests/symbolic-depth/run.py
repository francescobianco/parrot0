#!/usr/bin/env python3
"""Misura il banco di profondita' simbolica (docs/plans/long-symbolic-reasoning.md §5).

Legge i problemi da gen.py (stdin o file), li dice a parrot0 dal demone .p0t
(`make test-engine`, KB intera del profilo agi) e giudica ogni risposta:

  lettura    ogni lezione deve tornare «Learned…»: se una non si legge, il
             problema non misura il ragionamento ma l'ingresso (esito READ).
  risposta   domanda aperta: il nome giusto, nessun nome sbagliato;
             polare vera: «Yes»; polare falsa: mai «Yes» (un «Yes» e' MISCLAIM).
             Un'aperta con solo una parte delle risposte vere e' PARTIAL: la
             lista si presenta come intera.

Con --steps ripete le sole domande in chat con /debug e riporta i passi del
solver del turno (tutto il turno, non solo la dimostrazione: vedi §5.3).
"""
import argparse
import json
import os
import re
import subprocess
import sys
import tempfile
import time

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
BIN = os.path.join(ROOT, "bin", "parrot0")
SENT = "ZZZ_SYMBOLIC_DEPTH"


def p0t_for(p, lang="en"):
    lines = ["[mock live]", "!set PARROT0_SESSION=", "!set PARROT0_PROFILE=kb/profiles/agi.p0",
             f"!set PARROT0_LANG={lang}", "!reset", "", f"[test {p['id']}]", "!reset", "!timeout 600"]
    turns = []
    for s in p["lessons"]:
        lines.append(f"> {s}")
        lines.append(f"<~ {SENT}")
        turns.append(("lesson", s, len(lines)))
    for q in p["questions"]:
        lines.append(f"> {q['text']}")
        lines.append(f"<~ {SENT}")
        turns.append(("question", q, len(lines)))
    return "\n".join(lines) + "\n", turns


def run_p0t(text):
    with tempfile.NamedTemporaryFile("w", suffix=".p0t", delete=False, dir=os.environ.get("TMPDIR")) as f:
        f.write(text)
        path = f.name
    t0 = time.time()
    out = subprocess.run([BIN, "--test", path], cwd=ROOT, capture_output=True, text=True, timeout=3600).stdout
    el = time.time() - t0
    os.unlink(path)
    if not re.search(r"\d+ passed", out):
        raise SystemExit("il demone .p0t non ha risposto (make test-engine?):\n" + out[-400:])
    got, cur = {}, None
    for ln in out.splitlines():
        m = re.match(r"\s*FAIL\s+\[.*\] line (\d+)(.*)", ln)
        if m:
            cur = int(m.group(1))
            if "turn" in m.group(2):
                got[cur] = "<TIMEOUT> " + m.group(2)
            continue
        m = re.match(r"\s*got:\s+(.*)", ln)
        if m and cur is not None:
            got[cur] = m.group(1)
    return got, el


def judge(q, ans):
    a = ans.strip()
    low = a.lower()
    words = set(re.findall(r"[a-z']+", low))
    if q["yes"] == ["Yes"]:
        return "ok" if a.startswith("Yes") else ("no-answer" if not a.startswith("No") else "wrong")
    if not q["yes"]:  # polare falsa
        return "misclaim" if a.startswith("Yes") else "ok"
    if any(n in words for n in q["no"]):
        return "misclaim"
    found = [n for n in q["yes"] if n in words]
    if found and len(found) < len(q["yes"]):
        # una lista incompleta detta come se fosse intera (§7.3): non e' un muro
        return "partial"
    return "ok" if found else "no-answer"


def steps_for(p):
    turns = list(p["lessons"]) + ["/debug"] + [q["text"] for q in p["questions"]]
    env = dict(os.environ, PARROT0_SESSION="", PARROT0_LANG="en")
    r = subprocess.run([BIN], cwd=ROOT, input="\n".join(turns) + "\n", capture_output=True,
                       text=True, env=env, timeout=3600)
    return [int(x) for x in re.findall(r"\[debug\] [\d.]+ ms turno .* · (\d+) passi", r.stderr)]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("problems", nargs="?", help="JSONL da gen.py (default stdin)")
    ap.add_argument("--steps", action="store_true")
    ap.add_argument("--verbose", action="store_true")
    a = ap.parse_args()
    src = open(a.problems) if a.problems else sys.stdin
    rows = []
    for line in src:
        if not line.strip():
            continue
        p = json.loads(line)
        text, turns = p0t_for(p)
        got, el = run_p0t(text)
        unread = []
        verdicts = []
        for kind, item, ln in turns:
            ans = got.get(ln, "<PASS?>")
            if kind == "lesson":
                if not ans.startswith("Learned"):
                    unread.append((item, ans))
            else:
                verdicts.append((item["text"], judge(item, ans), ans))
        steps = steps_for(p) if a.steps else []
        if unread:
            outcome = "READ"
        elif any(v == "misclaim" for _, v, _ in verdicts):
            outcome = "MISCLAIM"
        elif any(v == "partial" for _, v, _ in verdicts):
            outcome = "PARTIAL"
        elif all(v == "ok" for _, v, _ in verdicts):
            outcome = "SOLVED"
        else:
            outcome = "UNSOLVED"
        row = {"id": p["id"], "family": p["family"], "D": p["D"], "W": p["W"], "turns": len(turns),
               "seconds": round(el, 1), "outcome": outcome, "verdicts": [v for _, v, _ in verdicts],
               "answers": [ans for _, _, ans in verdicts], "unread": unread[:3], "steps": steps}
        rows.append(row)
        print(json.dumps(row), flush=True)
        if a.verbose:
            for t, v, ans in verdicts:
                print(f"    {v:9} {t}  ->  {ans[:160]}", file=sys.stderr)
            for s, ans in unread[:3]:
                print(f"    UNREAD   {s}  ->  {ans[:160]}", file=sys.stderr)


if __name__ == "__main__":
    sys.exit(main())
