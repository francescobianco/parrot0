#!/usr/bin/env python3
"""Extract historical incident rows without turning them into runnable tests.

Run from any directory. Writes inventory.jsonl next to this script. The source
row is preserved verbatim; malformed Markdown rows have no inferred fields.
This is a documentary extractor, not a parser for parrot0 or its language.
"""

import hashlib
import json
from pathlib import Path
import re
import subprocess


ROOT = Path(__file__).resolve().parents[3]
SOURCE = Path("docs/plans/train-the-learning-process.md")
SPECS = (
    (
        "SA", "situational_agent", "### §SA —", "### ▶ Stato dopo",
        ("id", "reported_status", "utterance", "observed", "author_diagnosis", "related_gap"),
        10,
    ),
    (
        "DE", "german", "### T. Una lingua sconosciuta:", "### 0. Reattività",
        ("id", "author_category", "utterance", "observed", "author_diagnosis"),
        11,
    ),
    (
        "PR", "italian_dialogue", "### 0. Reattività", "### 1. Grammatica inglese",
        ("id", "author_category", "utterance", "observed", "author_diagnosis"),
        14,
    ),
    (
        "G", "english_grammar", "### 1. Grammatica inglese", "### Indice degli altri",
        ("id", "lesson_summary", "existing_support", "before", "lesson_effect", "after", "author_diagnosis"),
        25,
    ),
    (
        "M", "precision_mechanics", "### Le 100 lezioni", "## 🟠 FORME D'INSEGNAMENTO CHE NON FUNZIONANO — sessione live «debug",
        ("id", "author_category", "utterance", "observed", "failed_check", "author_diagnosis"),
        100,
    ),
    (
        "P", "php", "| # | lezione detta | che cosa è entrato |", "### Il coefficiente della sessione",
        ("id", "utterance", "observed", "failed_check", "author_diagnosis"),
        14,
    ),
)


def main():
    data = (ROOT / SOURCE).read_bytes()
    lines = data.decode("utf-8").splitlines()
    digest = hashlib.sha256(data).hexdigest()
    revision = subprocess.check_output(
        ["git", "rev-parse", "HEAD"], cwd=ROOT, text=True
    ).strip()
    records = []
    counts = {}
    for prefix, cohort, begin, end, columns, expected in SPECS:
        start = next(i for i, line in enumerate(lines) if line.startswith(begin))
        stop = next(i for i in range(start + 1, len(lines)) if lines[i].startswith(end))
        id_pattern = r"\d+" if prefix == "M" else prefix + r"\d+"
        row_pattern = re.compile(r"^\| (" + id_pattern + r") \|")
        cohort_records = []
        for i in range(start, stop):
            match = row_pattern.match(lines[i])
            if not match:
                continue
            original_id = match.group(1)
            record_id = f"M{int(original_id):03d}" if prefix == "M" else original_id
            cells = [cell.strip() for cell in lines[i].strip("|").split("|")]
            valid_shape = len(cells) == len(columns)
            cohort_records.append({
                "schema_version": 1,
                "record_type": "historical_observation",
                "id": record_id,
                "original_id": original_id,
                "cohort": cohort,
                "source": {"path": str(SOURCE), "line": i + 1, "sha256": digest},
                "extracted_at_revision": revision,
                "raw_row": lines[i],
                "table_shape_valid": valid_shape,
                "observed_column_count": len(cells),
                "fields": dict(zip(columns, cells)) if valid_shape else None,
                "episode_reconstruction_required": True,
                "current_replay_status": "unmeasured",
                "causal_boundary": None,
                "causal_family": None,
                "new_trace_paths": [],
            })
        # Fail rather than silently changing the cohort/denominator on a rerun.
        if len(cohort_records) != expected:
            raise ValueError(f"{cohort}: expected {expected} rows, found {len(cohort_records)}")
        records.extend(cohort_records)
        counts[cohort] = len(cohort_records)
    if len({record["id"] for record in records}) != len(records):
        raise ValueError("Duplicate incident IDs")
    output = Path(__file__).with_name("inventory.jsonl")
    output.write_text(
        "".join(json.dumps(record, ensure_ascii=False) + "\n" for record in records),
        encoding="utf-8",
    )
    print(json.dumps({
        "records": len(records),
        "cohorts": counts,
        "ambiguous_table_rows": sum(not record["table_shape_valid"] for record in records),
        "current_replays": 0,
        "source_sha256": digest,
        "output": str(output.relative_to(ROOT)),
    }, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
