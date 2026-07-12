#!/usr/bin/env python3
"""gen317 (forge-master-plan W0.3): `make capability-report` — the capability
ledger is GENERATED from real benchmark results, never hand-written.

Runs every 'gate' benchmark from tests/benchmarks.json, then verifies each
faculty maturity claim against its evidence:
  - verified    every evidence benchmark is a green gate
  - REGRESSED   an evidence gate is red — the claimed maturity is not supported
  - claimed     evidence exists only as discovery/degrade/external (not offline-
                gated), or the claim is ABSENT (nothing to verify)
Writes capabilities/manifest.json (versioned, §10.9) and prints the table.
"""
import json
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tests"))
from gate import git_commit, run_target  # noqa: E402


def brain_version() -> str:
    try:
        proc = subprocess.run(
            [str(ROOT / "bin" / "parrot0")], input="/quit\n",
            capture_output=True, text=True, timeout=30, cwd=ROOT,
        )
        out = proc.stdout + proc.stderr  # banner goes to stderr in pipes
        if out.startswith("parrot0 [") and "]" in out:
            return out[len("parrot0 ["):out.index("]")]
    except Exception:
        pass
    return "unknown"


def main() -> int:
    manifest = json.loads((ROOT / "tests" / "benchmarks.json").read_text())
    bench_by_id = {b["id"]: b for b in manifest["benchmarks"]}

    status: dict[str, str] = {}
    for b in manifest["benchmarks"]:
        if b["semantics"] != "gate":
            status[b["id"]] = "not-run"
            continue
        ok, _, _ = run_target(b["target"])
        status[b["id"]] = "green" if ok else "red"

    rows = []
    for c in manifest["capabilities"]:
        ev = c["evidence"]
        gate_ev = [e for e in ev if bench_by_id[e]["semantics"] == "gate"]
        if c["maturity"] == "ABSENT" or not ev:
            verdict = "claimed"
        elif any(status[e] == "red" for e in gate_ev):
            verdict = "REGRESSED"
        elif gate_ev and all(status[e] == "green" for e in gate_ev):
            verdict = "verified"
        else:
            verdict = "claimed"
        rows.append({**c, "verdict": verdict,
                     "evidence_status": {e: status[e] for e in ev}})

    report = {
        "commit": git_commit(),
        "brain_version": brain_version(),
        "generated": time.strftime("%Y-%m-%d %H:%M:%S"),
        "benchmarks": [{"id": b["id"], "semantics": b["semantics"],
                        "status": status[b["id"]]} for b in manifest["benchmarks"]],
        "capabilities": rows,
    }
    out = ROOT / "capabilities" / "manifest.json"
    out.parent.mkdir(exist_ok=True)
    out.write_text(json.dumps(report, indent=2) + "\n")

    print(f"capability-report: commit {report['commit']} "
          f"brain {report['brain_version']}")
    for b in report["benchmarks"]:
        print(f"  {b['status']:7} [{b['semantics']:9}] {b['id']}")
    print("faculties (forge plan §4):")
    for r in rows:
        ev = ", ".join(f"{k}={v}" for k, v in r["evidence_status"].items()) or "-"
        print(f"  {r['verdict']:9} {r['maturity']:8} {r['id']:28} {ev}")
    print(f"written: {out.relative_to(ROOT)}")
    return 1 if any(r["verdict"] == "REGRESSED" for r in rows) else 0


if __name__ == "__main__":
    sys.exit(main())