#!/usr/bin/env python3
"""Deterministic judge for match0; emits one machine-readable line.

Gradato di proposito (CHALLENGE_TODO C5): un agente che arriva a meta' lo si
deve vedere. Le soglie salgono da «il file esiste» a «rifiuta l'overflow»,
cosi' il punteggio dice DOVE si e' fermato, non solo che non ha finito.
"""

from __future__ import annotations

import hashlib
import json
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
HEADER = HERE / "seed" / "strjoin.h"
MAIN = HERE / "seed" / "main.c"
POINTS = {
    "artifact": 5,
    "seed_integrity": 5,
    "compile_clean": 20,
    "basic_join": 15,
    "separator_shapes": 10,
    "empty_and_single": 10,
    "invalid_arguments": 15,
    "overflow_rejected": 5,
    "integrated_build": 5,
    "cli_contract": 10,
}


def digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def probe_lines(out: str) -> dict[str, str]:
    return dict(
        line.split("=", 1) for line in out.splitlines() if "=" in line
    )


def main() -> int:
    code = Path(sys.argv[1]).resolve()
    source = code / "strjoin.c"
    checks: dict[str, tuple[bool, str]] = {}
    checks["artifact"] = (source.is_file(), "strjoin.c exists")

    untouched = True
    for supplied in (HEADER, MAIN):
        candidate = code / supplied.name
        if not candidate.is_file() or digest(candidate) != digest(supplied):
            untouched = False
    checks["seed_integrity"] = (untouched, "supplied strjoin.h and main.c are unchanged")

    if source.is_file():
        with tempfile.TemporaryDirectory(prefix="judge-strjoin-") as tmp_text:
            tmp = Path(tmp_text)
            shutil.copytree(code, tmp, dirs_exist_ok=True)
            shutil.copy2(HEADER, tmp / "strjoin.h")
            shutil.copy2(HERE / "probe.c", tmp / "probe.c")
            compiler = shutil.which("cc") or shutil.which("gcc")
            if not compiler:
                checks["compile_clean"] = (False, "no C compiler available")
            else:
                build = subprocess.run(
                    [compiler, "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2",
                     "strjoin.c", "probe.c", "-o", "probe"],
                    cwd=tmp, text=True, capture_output=True, timeout=25,
                )
                clean = build.returncode == 0
                checks["compile_clean"] = (
                    clean, "builds with -Wall -Wextra -Werror" if clean
                    else (build.stderr.strip().splitlines() or ["build failed"])[-1][:200]
                )
                if clean:
                    try:
                        run = subprocess.run([str(tmp / "probe")], cwd=tmp, text=True,
                                             capture_output=True, timeout=15)
                        got = probe_lines(run.stdout)
                    except subprocess.TimeoutExpired:
                        got = {}
                        checks["basic_join"] = (False, "probe exceeded 15 seconds")
                    if got:
                        checks["basic_join"] = (
                            got.get("basic") == "[a-bb-ccc]",
                            f"join with a one-char separator -> {got.get('basic')!r}",
                        )
                        checks["separator_shapes"] = (
                            got.get("empty_sep") == "[abbccc]"
                            and got.get("long_sep") == "[a<->bb<->ccc]",
                            f"empty and multi-char separators -> "
                            f"{got.get('empty_sep')!r}, {got.get('long_sep')!r}",
                        )
                        checks["empty_and_single"] = (
                            got.get("single") == "[solo]"
                            and got.get("zero") == "[]"
                            and got.get("empty_parts") == "[,,]",
                            f"count 0 is a malloc'd empty string, count 1 has no "
                            f"separator -> {got.get('zero')!r}, {got.get('single')!r}",
                        )
                        checks["invalid_arguments"] = (
                            got.get("null_sep") == "NULL"
                            and got.get("null_parts") == "NULL"
                            and got.get("null_elem") == "NULL",
                            f"NULL sep/parts/element all refused -> "
                            f"{got.get('null_sep')!r}, {got.get('null_parts')!r}, "
                            f"{got.get('null_elem')!r}",
                        )
                        # L'overflow non si prova con un allocazione vera: si legge
                        # la fonte. Un totale che non puo' esistere deve essere
                        # RIFIUTATO, e il rifiuto e' un confronto scritto.
                        text = source.read_text(encoding="utf-8", errors="replace")
                        guards = ("SIZE_MAX" in text or "PTRDIFF_MAX" in text
                                  or "overflow" in text.lower())
                        checks["overflow_rejected"] = (
                            guards, "the source states an overflow guard"
                        )

            # La build integrata usa il Makefile del progetto, non il nostro cc.
            with tempfile.TemporaryDirectory(prefix="judge-strjoin-make-") as mk_text:
                mk = Path(mk_text)
                shutil.copytree(code, mk, dirs_exist_ok=True)
                shutil.copy2(HEADER, mk / "strjoin.h")
                shutil.copy2(MAIN, mk / "main.c")
                try:
                    made = subprocess.run(["make"], cwd=mk, text=True,
                                          capture_output=True, timeout=25)
                    built = made.returncode == 0 and (mk / "join").is_file()
                    checks["integrated_build"] = (
                        built, "make builds the whole project" if built
                        else (made.stderr.strip().splitlines() or ["make failed"])[-1][:200]
                    )
                    if built:
                        cli = subprocess.run([str(mk / "join"), ", ", "x", "y", "z"],
                                             cwd=mk, text=True, capture_output=True,
                                             timeout=10)
                        usage = subprocess.run([str(mk / "join")], cwd=mk, text=True,
                                               capture_output=True, timeout=10)
                        checks["cli_contract"] = (
                            cli.stdout == "x, y, z\n" and cli.returncode == 0
                            and usage.returncode == 2,
                            f"./join printed {cli.stdout!r} and bare invocation "
                            f"exited {usage.returncode}",
                        )
                except subprocess.TimeoutExpired:
                    checks["integrated_build"] = (False, "make exceeded 25 seconds")

    for name in POINTS:
        checks.setdefault(name, (False, "not reached"))
    rendered = [
        {"name": name, "passed": passed, "points": POINTS[name] if passed else 0,
         "available_points": POINTS[name], "detail": detail}
        for name, (passed, detail) in checks.items()
    ]
    score = sum(item["points"] for item in rendered)
    report = {
        "score": score,
        "total_points": sum(POINTS.values()),
        "status": "pass" if score == 100 else "partial" if score else "fail",
        "checks": rendered,
        "diagnosis_tags": [i["name"] for i in rendered if not i["passed"]],
    }
    print(json.dumps(report, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
