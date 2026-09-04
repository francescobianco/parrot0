#!/usr/bin/env python3
"""Resumable, manual-only runner for the parrotbench .p0t slots."""

import csv
import os
import re
import subprocess
import sys
import time
from datetime import datetime, timezone
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CORPUS = ROOT / "tests" / "parrotbench" / "corpus"
OUT = ROOT / "tests" / "parrotbench" / "results"
STATE = Path(os.environ.get("PARROTBENCH_STATE", OUT / "progress.tsv"))
BIN = ROOT / "bin" / "parrot0"


def now():
    return datetime.now(timezone.utc).isoformat(timespec="seconds")


def norm(value: str) -> str:
    value = value.lower()
    value = re.sub(r"[^\w\s]", " ", value, flags=re.UNICODE)
    return " ".join(value.split())


def load_cases(path):
    cases = []
    name = prompt = expected = None
    for raw in path.read_text(encoding="utf-8").splitlines():
        line = raw.strip()
        if line.startswith("[parrotbench_") and line.endswith("]"):
            if name is not None and prompt is not None and expected is not None:
                cases.append((name, prompt, expected))
            name = line[1:-1]
            prompt = expected = None
        elif line.startswith("> "):
            prompt = line[2:]
        elif line.startswith("<~ "):
            expected = line[3:]
    if name is not None and prompt is not None and expected is not None:
        cases.append((name, prompt, expected))
    return cases


def slots():
    return sorted(CORPUS.rglob("*.p0t"))


def read_state():
    state = {}
    if not STATE.exists():
        return state
    lines = [
        line for line in STATE.read_text(encoding="utf-8").splitlines(True)
        if not line.startswith("#")
    ]
    for row in csv.DictReader(lines, delimiter="\t"):
        state[row["slot"]] = row
    return state


def write_state(rows):
    STATE.parent.mkdir(parents=True, exist_ok=True)
    temp = STATE.with_suffix(".tmp")
    fields = ["slot", "category", "status", "passed", "failed", "total", "percent", "started", "finished"]
    with temp.open("w", newline="", encoding="utf-8") as stream:
        stream.write("# Manual-only progress registry. Never a TDD or regression gate.\n")
        writer = csv.DictWriter(stream, fieldnames=fields, delimiter="\t", lineterminator="\n")
        writer.writeheader()
        for key in sorted(rows):
            writer.writerow({field: rows[key].get(field, "") for field in fields})
    temp.replace(STATE)


def write_histogram(rows):
    path = OUT / "histogram.tsv"
    OUT.mkdir(parents=True, exist_ok=True)
    aggregates = {}
    for row in rows.values():
        if row.get("status") != "complete":
            continue
        category = row["category"]
        item = aggregates.setdefault(category, [0, 0, 0])
        item[0] += int(row["passed"])
        item[1] += int(row["failed"])
        item[2] += int(row["total"])
    temp = path.with_suffix(".tmp")
    with temp.open("w", encoding="utf-8") as stream:
        stream.write("# category\tpassed\tfailed\ttotal\tpercent\n")
        stream.write("category\tpassed\tfailed\ttotal\tpercent\n")
        for category in sorted(aggregates):
            passed, failed, total = aggregates[category]
            percent = passed * 100.0 / total if total else 0.0
            stream.write(f"{category}\t{passed}\t{failed}\t{total}\t{percent:.2f}\n")
    temp.replace(path)


def run_slot(path):
    cases = load_cases(path)
    prompts = "".join(f"{prompt}\n" for _, prompt, _ in cases) + "/quit\n"
    result = subprocess.run(
        [str(BIN)],
        input=prompts,
        text=True,
        capture_output=True,
        env={
            "PARROT0_BASE": "",
            "PARROT0_SESSION": "",
            "PARROT0_LANG": "en",
        },
        check=False,
    )
    replies = [line[6:] for line in result.stdout.splitlines() if line.startswith(">>> ")]
    passed = 0
    for index, (_, _, expected) in enumerate(cases):
        actual = replies[index] if index < len(replies) else ""
        if norm(expected) and norm(expected) in norm(actual):
            passed += 1
    total = len(cases)
    return passed, total - passed, total


def main():
    print("PARROTBENCH: manual-only measurement; never a TDD or regression gate")
    print("PARROTBENCH: progress is recorded in", STATE)
    print("PARROTBENCH: histogram data is recorded in", OUT / "histogram.tsv")
    if not BIN.exists():
        print(f"parrotbench: binary not built: {BIN}", file=sys.stderr)
        return 1

    all_slots = slots()
    rows = read_state()
    max_slots = int(os.environ.get("PARROTBENCH_MAX_SLOTS", "0"))
    selected = 0
    for path in all_slots:
        key = str(path.relative_to(CORPUS))
        old = rows.get(key, {})
        if old.get("status") == "complete":
            continue
        if max_slots and selected >= max_slots:
            break
        selected += 1
        category = str(path.parent.relative_to(CORPUS))
        rows[key] = {
            "slot": key,
            "category": category,
            "status": "running",
            "passed": "0",
            "failed": "0",
            "total": str(len(load_cases(path))),
            "percent": "0.00",
            "started": now(),
            "finished": "",
        }
        write_state(rows)
        print(f"parrotbench: running {key}", flush=True)
        passed, failed, total = run_slot(path)
        percent = passed * 100.0 / total if total else 0.0
        rows[key].update({
            "status": "complete",
            "passed": str(passed),
            "failed": str(failed),
            "total": str(total),
            "percent": f"{percent:.2f}",
            "finished": now(),
        })
        write_state(rows)
        write_histogram(rows)
        print(f"parrotbench: completed {key}: {passed}/{total} ({percent:.2f}%)", flush=True)

    write_histogram(rows)
    passed = sum(int(row.get("passed", 0)) for row in rows.values() if row.get("status") == "complete")
    total = sum(int(row.get("total", 0)) for row in rows.values() if row.get("status") == "complete")
    percent = passed * 100.0 / total if total else 0.0
    completed = sum(row.get("status") == "complete" for row in rows.values())
    print(f"parrotbench: {passed}/{total} ({percent:.2f}%)")
    print(f"parrotbench: slots complete {completed}/{len(all_slots)}")
    if max_slots:
        print(f"parrotbench: limited manual run ({max_slots} slot(s)); full run has no limit")
    print("parrotbench: rerun make parrotbench to resume incomplete/running slots")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
