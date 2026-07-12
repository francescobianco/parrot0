#!/usr/bin/env python3
"""gen316 (forge-master-plan W0.5): `make gate` — run every benchmark the
manifest declares with 'gate' semantics and fail if any is red.

This is the ratchet the forge promotes against: a red gate means the baseline
is broken and nothing may be promoted on top of it (§10.1 'baseline-broken').
Discovery/degrade/external benchmarks are listed but never executed here, so
the gate stays offline, deterministic and honest about what it measures.
"""
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MANIFEST = ROOT / "tests" / "benchmarks.json"


def git_commit() -> str:
    try:
        return subprocess.run(
            ["git", "rev-parse", "--short", "HEAD"], cwd=ROOT,
            capture_output=True, text=True, timeout=10,
        ).stdout.strip() or "unknown"
    except Exception:
        return "unknown"


def run_target(target: str) -> tuple[bool, str, list[str]]:
    """Run one make target; return (green, last output line, FAIL lines)."""
    proc = subprocess.run(
        ["make", "-s", target], cwd=ROOT,
        capture_output=True, text=True, timeout=1800,
    )
    lines = [l for l in (proc.stdout + proc.stderr).splitlines() if l.strip()]
    tail = lines[-1] if lines else ""
    fails = [l for l in lines if l.startswith("FAIL")][:10]
    return proc.returncode == 0, tail, fails


def main() -> int:
    manifest = json.loads(MANIFEST.read_text())
    entries = manifest["benchmarks"]
    gates = [e for e in entries if e["semantics"] == "gate"]
    skipped = [e for e in entries if e["semantics"] != "gate"]

    print(f"gate: commit {git_commit()} — {len(gates)} gate benchmarks "
          f"({len(skipped)} non-gate listed, not run)")
    red = []
    for e in gates:
        ok, tail, fails = run_target(e["target"])
        mark = "GREEN" if ok else "RED"
        print(f"  {mark:5} {e['id']:12} make {e['target']:18} {tail[:90]}")
        for f in fails:
            print(f"        {f[:110]}")
        if not ok:
            red.append(e["id"])
    for e in skipped:
        print(f"  -     {e['id']:12} make {e['target']:18} [{e['semantics']}] not gated")

    if red:
        print(f"gate: BASELINE BROKEN — red: {', '.join(red)} "
              f"(no promotion may land on this baseline)")
        return 1
    print("gate: baseline green — promotions may proceed")
    return 0


if __name__ == "__main__":
    sys.exit(main())