#!/usr/bin/env python3
"""Deterministic judge for match1; emits one machine-readable line."""

from __future__ import annotations

import hashlib
import json
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


HERE = Path(__file__).resolve().parent
HEADER = HERE / "seed" / "quicksort.h"
POINTS = {
    "artifact": 5,
    "seed_integrity": 5,
    "compile_clean": 15,
    "basic_order": 10,
    "generic_records": 10,
    "duplicate_permutation": 5,
    "comparator_context": 10,
    "invalid_arguments": 10,
    "overflow_rejected": 5,
    "adversarial_growth": 10,
    "integrated_build": 5,
    "cli_contract": 10,
}


def digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main() -> int:
    code = Path(sys.argv[1]).resolve()
    source = code / "quicksort.c"
    checks: dict[str, tuple[bool, str]] = {}
    checks["artifact"] = (source.is_file(), "quicksort.c exists")
    candidate_header = code / "quicksort.h"
    header_ok = candidate_header.is_file() and digest(candidate_header) == digest(HEADER)
    checks["seed_integrity"] = (header_ok, "supplied quicksort.h is unchanged")

    if source.is_file():
        with tempfile.TemporaryDirectory(prefix="judge-quicksort-") as tmp_text:
            tmp = Path(tmp_text)
            shutil.copytree(code, tmp, dirs_exist_ok=True)
            shutil.copy2(HEADER, tmp / "quicksort.h")
            shutil.copy2(HERE / "probe.c", tmp / "probe.c")
            compiler = shutil.which("cc") or shutil.which("gcc")
            if not compiler:
                checks["compile_clean"] = (False, "no C compiler available")
            else:
                build = subprocess.run(
                    [compiler, "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2",
                     "quicksort.c", "probe.c", "-o", "probe"],
                    cwd=tmp, text=True, capture_output=True, timeout=20,
                )
                checks["compile_clean"] = (
                    build.returncode == 0,
                    "clean C11 build" if build.returncode == 0 else build.stderr[-600:],
                )
                if build.returncode == 0:
                    try:
                        run = subprocess.run(
                            [str(tmp / "probe")], cwd=tmp, text=True,
                            capture_output=True, timeout=12,
                        )
                        seen = {
                            parts[1]: parts[2] == "PASS"
                            for line in run.stdout.splitlines()
                            if len(parts := line.split()) == 3 and parts[0] == "CHECK"
                        }
                        for name in POINTS:
                            if name in ("artifact", "seed_integrity", "compile_clean"):
                                continue
                            checks[name] = (
                                run.returncode == 0 and seen.get(name, False),
                                f"probe exit={run.returncode}",
                            )
                    except subprocess.TimeoutExpired:
                        checks["adversarial_growth"] = (False, "probe exceeded 12 seconds")

            make = shutil.which("make")
            if make:
                try:
                    built = subprocess.run(
                        [make, "clean", "all"], cwd=tmp, text=True,
                        capture_output=True, timeout=20,
                    )
                    checks["integrated_build"] = (
                        built.returncode == 0,
                        "whole codebase builds" if built.returncode == 0 else
                        (built.stdout + built.stderr)[-600:],
                    )
                    if built.returncode == 0:
                        sample = "3,2,zeta\n1,2,alpha\n4,-1,beta\n2,9,alpha\n"
                        priority = subprocess.run(
                            [str(tmp / "record-sort"), "--key", "priority"],
                            cwd=tmp, input=sample, text=True, capture_output=True, timeout=5,
                        )
                        names = subprocess.run(
                            [str(tmp / "record-sort"), "--key", "name"],
                            cwd=tmp, input=sample, text=True, capture_output=True, timeout=5,
                        )
                        checks["cli_contract"] = (
                            priority.returncode == 0 and names.returncode == 0 and
                            priority.stdout == "4,-1,beta\n1,2,alpha\n3,2,zeta\n2,9,alpha\n" and
                            names.stdout == "1,2,alpha\n2,9,alpha\n4,-1,beta\n3,2,zeta\n",
                            "priority/name ordering and id tie-breaking",
                        )
                except subprocess.TimeoutExpired:
                    checks["integrated_build"] = (False, "whole build exceeded 20 seconds")

    for name in POINTS:
        checks.setdefault(name, (False, "not reached"))
    rendered = [
        {"name": name, "passed": passed, "points": POINTS[name] if passed else 0,
         "available_points": POINTS[name], "detail": detail}
        for name, (passed, detail) in checks.items()
    ]
    score = sum(item["points"] for item in rendered)
    failed = [item["name"] for item in rendered if not item["passed"]]
    report = {
        "score": score,
        "status": "pass" if score == 100 else "partial" if score else "fail",
        "checks": rendered,
        "diagnosis_tags": failed,
    }
    print(json.dumps(report, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
