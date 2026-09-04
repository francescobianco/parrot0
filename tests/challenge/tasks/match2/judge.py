#!/usr/bin/env python3
"""Deterministic judge for the crash-tolerant journal challenge."""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


HERE = Path(__file__).resolve().parent
POINTS = {
    "artifact": 5,
    "import_api": 5,
    "roundtrip_revision": 15,
    "restart_monotonicity": 10,
    "unicode_and_newlines": 10,
    "truncated_tail_recovery": 15,
    "complete_corruption_rejected": 10,
    "compaction_atomic_state": 15,
    "concurrent_writers": 10,
    "cli_compatibility": 5,
}


def main() -> int:
    code = Path(sys.argv[1]).resolve()
    source = code / "journal.py"
    outcomes: dict[str, tuple[bool, str]] = {
        "artifact": (source.is_file(), "journal.py exists")
    }
    if source.is_file():
        with tempfile.TemporaryDirectory(prefix="judge-journal-") as tmp_text:
            tmp = Path(tmp_text)
            shutil.copytree(code, tmp, dirs_exist_ok=True)
            shutil.copy2(HERE / "probe.py", tmp / "probe.py")
            try:
                run = subprocess.run(
                    [sys.executable, "probe.py"], cwd=tmp,
                    text=True, capture_output=True, timeout=25,
                )
                for line in run.stdout.splitlines():
                    if not line.startswith("CHECK "):
                        continue
                    _, name, state, *detail = line.split(" ", 3)
                    outcomes[name] = (state == "PASS", detail[0] if detail else "")
            except subprocess.TimeoutExpired:
                outcomes["concurrent_writers"] = (False, "probe exceeded 25 seconds")

    for name in POINTS:
        outcomes.setdefault(name, (False, "not reached"))
    checks = [
        {"name": name, "passed": passed, "points": POINTS[name] if passed else 0,
         "available_points": POINTS[name], "detail": detail[:500]}
        for name, (passed, detail) in outcomes.items()
    ]
    score = sum(check["points"] for check in checks)
    failed = [check["name"] for check in checks if not check["passed"]]
    print(json.dumps({
        "score": score,
        "status": "pass" if score == 100 else "partial" if score else "fail",
        "checks": checks,
        "diagnosis_tags": failed,
    }, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
