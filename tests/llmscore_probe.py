#!/usr/bin/env python3
"""llmscore_probe — Fase 0 di "Motorize the Class" (docs/plans/motorize-the-class.md).

Misura la CLASSE, non il campione. Feeds a held-out battery of questions per
class through parrot0 (same invocation as tests/llmscore.py) and reports the
WALL-RATE per class: the fraction of answers that are a deflection ("I don't
understand that yet", "Want me to learn about it", ...). No judge API needed —
the wall is an objective, disqualifying tell, so its rate is our primary
development metric. A fresh parrot0 process per class avoids cross-turn bleed.

Usage: .venv/bin/python tests/llmscore_probe.py [--dir tests/llmscore-probes]
"""
import os, sys, subprocess, glob, re

P0_EOT = "\x1e"

# Deflection markers taken verbatim from the C sources (grep of src/brain).
WALL_PATTERNS = [
    "i don't understand", "i dont understand",
    "i don't know about", "i dont know about",
    "want me to learn", "haven't learned", "havent learned",
    "not sure i followed", "no idea what to say",
    "say it another way", "not sure which function",
]


def is_wall(ans):
    a = ans.strip().lower()
    if not a:
        return True  # empty reply is as bad as a wall
    return any(p in a for p in WALL_PATTERNS)


def read_reply(proc):
    lines = []
    while True:
        ln = proc.stdout.readline()
        if not ln:
            break
        ln = ln.rstrip("\n")
        if ln == P0_EOT:
            break
        lines.append(ln)
    return "\n".join(lines)


def run_class(path):
    with open(path) as f:
        qs = [ln.strip() for ln in f if ln.strip()]
    proc = subprocess.Popen(["./bin/parrot0"], stdin=subprocess.PIPE,
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True, bufsize=1,
        env={**os.environ, "PARROT0_BASE": "", "PARROT0_SESSION": "",
             "PARROT0_EOT": P0_EOT})
    rows = []
    try:
        for q in qs:
            proc.stdin.write(q + "\n")
            proc.stdin.flush()
            a = read_reply(proc)
            rows.append((q, a, is_wall(a)))
    finally:
        try:
            proc.stdin.close()
        except Exception:
            pass
        proc.terminate()
    return rows


def main():
    d = "tests/llmscore-probes"
    if "--dir" in sys.argv:
        d = sys.argv[sys.argv.index("--dir") + 1]
    files = sorted(glob.glob(os.path.join(d, "*.txt")))
    if not files:
        print(f"no probe files in {d}", file=sys.stderr)
        return 1
    print(f"# llmscore-probe — {len(files)} classes\n")
    total_w = total_n = 0
    summary = []
    for path in files:
        cls = os.path.splitext(os.path.basename(path))[0]
        rows = run_class(path)
        w = sum(1 for _, _, wall in rows if wall)
        n = len(rows)
        total_w += w
        total_n += n
        rate = 100.0 * w / n if n else 0.0
        summary.append((cls, w, n, rate))
        print(f"## {cls}: {n - w}/{n} answered  ({rate:.0f}% wall)")
        for q, a, wall in rows:
            mark = "WALL" if wall else "ok  "
            print(f"  [{mark}] {q}")
            if wall:
                print(f"         -> {a[:90]}")
        print()
    print("## summary (wall-rate per class, lower is better)")
    for cls, w, n, rate in sorted(summary, key=lambda r: -r[3]):
        bar = "#" * int(rate / 5)
        print(f"  {cls:12s} {w:2d}/{n:2d}  {rate:5.0f}%  {bar}")
    overall = 100.0 * total_w / total_n if total_n else 0.0
    print(f"\n  OVERALL      {total_w:2d}/{total_n:2d}  {overall:5.0f}% wall  "
          f"({total_n - total_w}/{total_n} answered)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
