#!/usr/bin/env python3
"""Teach a variety lexicon to parrot0 by TALKING, in one live session.

Every line becomes an ordinary chat turn — `learn "<form>" as <label>` — the
same act a person performs, driven by the `learnable/3` registry. Nothing is
asserted through a side channel: if a form does not enter this way, it does not
enter (LEARN_PROTOCOL §1.1).

The forms must be REAL colloquial usage, never nonce words: this is training,
not fixture-building. `--save` sends `/save` at the end so the lesson survives
the process; without it the run is a dry probe.
"""

from __future__ import annotations
import argparse, csv, os, re, subprocess, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("corpus", type=Path)
    ap.add_argument("--binary", type=Path, default=ROOT / "bin/parrot0")
    ap.add_argument("--save", action="store_true")
    ap.add_argument("--lang", default="en")
    args = ap.parse_args()

    rows = list(csv.DictReader(args.corpus.open(encoding="utf-8"), delimiter="\t"))
    if not rows:
        raise SystemExit(f"{args.corpus}: empty")
    for r in rows:
        if '"' in r["form"] or not r["form"].strip():
            raise SystemExit(f"bad form: {r['form']!r}")

    turns = [f'learn "{r["form"]}" as {r["label"]}' for r in rows]
    if args.save:
        turns.append("/save")
    env = os.environ.copy()
    env.update({
        "PARROT0_PROFILE": "kb/profiles/agi.p0", "PARROT0_SESSION": "",
        "PARROT0_WORLD_FACTS": "1", "PARROT0_TOOLS": "0",
        "PARROT0_WIKI_FETCH": "0", "PARROT0_LANG": args.lang,
        "PARROT0_EOT": "<<EOT>>",
    })
    proc = subprocess.run(
        [str(args.binary)], input="\n".join(turns) + "\n/quit\n", text=True,
        capture_output=True, cwd=ROOT, env=env, timeout=900, check=False)

    replies = []
    for chunk in proc.stdout.split("<<EOT>>")[:-1]:
        lines = [l.strip() for l in chunk.splitlines() if l.strip()]
        lines = [l for l in lines if not l.startswith(
            ("parrot0 [", "mode:", "say something", ">>>", "parrot0: bye"))]
        replies.append(" ".join(lines))

    taken = refused = 0
    ok = re.compile(r"(?:i'll take|prendo|ricevuto)", re.I)
    for r, reply in zip(rows, replies):
        if ok.search(reply):
            taken += 1
        else:
            refused += 1
            if refused <= 12:
                print(f"NOT TAKEN  {r['label']:<24} {r['form']!r}\n           < {reply}",
                      file=sys.stderr)
    print(f"forms={len(rows)} taken={taken} not_taken={refused}")
    routed = [l for l in replies if "routed" in l]
    if routed:
        print(routed[-1])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
