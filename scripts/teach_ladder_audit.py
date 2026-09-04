#!/usr/bin/env python3
"""Audit which `learnable/3` labels actually change the next turn.

Teaching a form that no consumer reads writes into a drawer nobody opens: the
fact is true in the KB and invisible in behaviour (the historical
`greeting(ahoy)` smell). Before teaching a lexicon at scale, this instrument
measures, per label, whether a taught member is EFFECTIVE — the
`taught_member_effective` coordinate of D28.

Diagnostic only: nothing is saved, and every probe form is a real colloquial
form, never a nonce word.
"""

from __future__ import annotations
import csv, os, re, subprocess, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BIN = ROOT / "bin/parrot0"


def chat(turns: list[str], lang: str) -> list[str]:
    env = os.environ.copy()
    env.update({
        "PARROT0_PROFILE": "kb/profiles/agi.p0", "PARROT0_SESSION": "",
        "PARROT0_WORLD_FACTS": "1", "PARROT0_TOOLS": "0",
        "PARROT0_WIKI_FETCH": "0", "PARROT0_LANG": lang,
        "PARROT0_EOT": "<<EOT>>",
    })
    proc = subprocess.run(
        [str(BIN)], input="\n".join(turns) + "\n/quit\n", text=True,
        capture_output=True, cwd=ROOT, env=env, timeout=60, check=False)
    out = []
    for chunk in proc.stdout.split("<<EOT>>")[:-1]:
        lines = [l.strip() for l in chunk.splitlines() if l.strip()]
        lines = [l for l in lines if not l.startswith(
            ("parrot0 [", "mode:", "say something", ">>>", "parrot0: bye"))]
        out.append(" ".join(lines))
    return out


def norm(s: str) -> str:
    return re.sub(r"\W+", " ", s.lower()).strip()


def main() -> int:
    rows = list(csv.DictReader(open(sys.argv[1], encoding="utf-8"), delimiter="\t"))
    print("label\tlang\tform\tbefore_changed\tteach_ok\teffective\tretract_ok")
    eff = tot = 0
    for r in rows:
        form, label, lang, probe = r["form"], r["label"], r["language"], r["probe"]
        teach = f'learn "{form}" as {label}'
        forget = f'forget "{form}" as {label}'
        before, ack, after, ack2, restored = chat(
            [probe, teach, probe, forget, probe], lang)
        teach_ok = "learn" not in norm(ack) and bool(ack) and (
            "take" in norm(ack) or "prendo" in norm(ack) or "ricevuto" in norm(ack))
        effective = norm(after) != norm(before) and bool(after)
        retract_ok = norm(restored) == norm(before)
        tot += 1
        eff += int(effective)
        print(f"{label}\t{lang}\t{form}\t{norm(before)[:28]}\t"
              f"{int(teach_ok)}\t{int(effective)}\t{int(retract_ok)}")
    print(f"\nTaughtMemberEffective = {eff}/{tot}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
