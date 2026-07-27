#!/usr/bin/env python3
"""Measure the dependency graph encoded by parrot0's .p0 knowledge base.

The default report mirrors docs/plans/the-model-plan.md:

* facts and declared predicates are counted across the whole KB tree;
* reasoning clauses are counted in kb/core, the always-mounted model;
* procedures.p0 and meta.p0 are reported separately as engine machinery;
* a predicate is "connected" when a core rule body can consume it.

The scanner follows the .p0 lexical rules that matter for measurement: `%`
comments, quoted strings, balanced compound terms, multiline clauses, and
multiple clauses on one physical line.  It deliberately does not try to solve
or type-check clauses; the runtime remains the authority for that.
"""

from __future__ import annotations

import argparse
import json
import re
from collections import Counter, defaultdict
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, Iterator


PREDICATE_RE = re.compile(r"^([a-z][a-z0-9_]*)\s*(?:\(|$)")
MACHINERY_FILES = {"meta.p0", "procedures.p0"}


@dataclass(frozen=True)
class Clause:
    path: Path
    line: int
    text: str
    head: str
    body: str | None


def iter_clause_text(path: Path) -> Iterator[tuple[int, str]]:
    """Yield logical clauses, not physical lines, from a .p0 file."""
    text = path.read_text(encoding="utf-8")
    buf: list[str] = []
    depth = 0
    quote = ""
    escaped = False
    comment = False
    line = 1
    start_line = 1
    has_content = False

    for char in text:
        if comment:
            if char == "\n":
                comment = False
                if buf and buf[-1] != " ":
                    buf.append(" ")
                line += 1
            continue

        if quote:
            buf.append(char)
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == quote:
                quote = ""
            if char == "\n":
                line += 1
            continue

        if char == "%":
            comment = True
            continue
        if char == '"':
            if not has_content:
                start_line = line
                has_content = True
            quote = char
            buf.append(char)
            continue
        if char in "([{":
            if not has_content:
                start_line = line
                has_content = True
            depth += 1
            buf.append(char)
            continue
        if char in ")]}":
            depth = max(0, depth - 1)
            buf.append(char)
            continue
        if char == "." and depth == 0:
            clause = "".join(buf).strip()
            if clause:
                yield start_line, clause
            buf.clear()
            has_content = False
            continue
        if char == "\n":
            if buf and buf[-1] != " ":
                buf.append(" ")
            line += 1
            continue
        if not char.isspace() and not has_content:
            start_line = line
            has_content = True
        buf.append(char)


def find_rule_separator(text: str) -> int:
    depth = 0
    quote = ""
    escaped = False
    for index, char in enumerate(text[:-1]):
        if quote:
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == quote:
                quote = ""
            continue
        if char == '"':
            quote = char
        elif char in "([{":
            depth += 1
        elif char in ")]}":
            depth = max(0, depth - 1)
        elif char == ":" and text[index + 1] == "-" and depth == 0:
            return index
    return -1


def parse_clause(path: Path, line: int, text: str) -> Clause | None:
    stripped = text.strip()
    if not stripped or stripped.startswith(":-"):
        return None
    sep = find_rule_separator(stripped)
    head_text = stripped if sep < 0 else stripped[:sep].strip()
    match = PREDICATE_RE.match(head_text)
    if not match:
        return None
    body = None if sep < 0 else stripped[sep + 2 :].strip()
    return Clause(path, line, stripped, match.group(1), body)


def load_clauses(root: Path) -> list[Clause]:
    clauses: list[Clause] = []
    for path in sorted(root.rglob("*.p0")):
        for line, text in iter_clause_text(path):
            clause = parse_clause(path, line, text)
            if clause is not None:
                clauses.append(clause)
    return clauses


def split_top_level(text: str, separator: str = ",") -> list[str]:
    pieces: list[str] = []
    depth = 0
    quote = ""
    escaped = False
    start = 0
    for index, char in enumerate(text):
        if quote:
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == quote:
                quote = ""
            continue
        if char == '"':
            quote = char
        elif char in "([{":
            depth += 1
        elif char in ")]}":
            depth = max(0, depth - 1)
        elif char == separator and depth == 0:
            pieces.append(text[start:index].strip())
            start = index + 1
    pieces.append(text[start:].strip())
    return pieces


def body_calls(body: str) -> set[str]:
    """Return predicates invoked as goals, excluding nested data functors."""
    calls: set[str] = set()
    for goal in split_top_level(body):
        match = PREDICATE_RE.match(goal)
        if not match:
            continue
        pred = match.group(1)
        calls.add(pred)
        # Negation contains a goal; arithmetic/list compound terms are data.
        if pred == "naf":
            left = goal.find("(")
            right = goal.rfind(")")
            if left >= 0 and right > left:
                calls.update(body_calls(goal[left + 1 : right]))
    return calls


def relpath(path: Path, base: Path) -> str:
    try:
        return str(path.relative_to(base))
    except ValueError:
        return str(path)


def build_report(kb_root: Path, core_root: Path, limit: int) -> dict[str, object]:
    all_clauses = load_clauses(kb_root)
    core_prefix = core_root.resolve()
    core_clauses = [
        clause
        for clause in all_clauses
        if clause.path.resolve().is_relative_to(core_prefix)
    ]
    facts = [clause for clause in all_clauses if clause.body is None]
    rules = [clause for clause in core_clauses if clause.body is not None]
    machinery_rules = [
        clause for clause in rules if clause.path.name in MACHINERY_FILES
    ]
    world_rules = [
        clause for clause in rules if clause.path.name not in MACHINERY_FILES
    ]

    declared = {clause.head for clause in all_clauses}
    consumed: set[str] = set()
    for clause in rules:
        consumed.update(body_calls(clause.body or "") & declared)
    inert = declared - consumed

    fact_counts = Counter(clause.head for clause in facts)
    files_by_predicate: dict[str, set[str]] = defaultdict(set)
    for clause in facts:
        files_by_predicate[clause.head].add(relpath(clause.path, kb_root.parent))

    ranked = sorted(
        inert,
        key=lambda pred: (-fact_counts[pred], pred),
    )
    inert_rows = [
        {
            "predicate": pred,
            "facts": fact_counts[pred],
            "files": sorted(files_by_predicate[pred]),
        }
        for pred in ranked[:limit]
    ]

    return {
        "kb_root": str(kb_root),
        "core_root": str(core_root),
        "facts": len(facts),
        "lexeme_facts": fact_counts["lexeme"],
        "core_clauses": len(rules),
        "machinery_clauses": len(machinery_rules),
        "world_clauses": len(world_rules),
        "predicates": len(declared),
        "consumed_predicates": len(consumed),
        "inert_predicates": len(inert),
        "inert": inert_rows,
    }


def print_report(report: dict[str, object]) -> None:
    print(f"facts: {report['facts']}")
    print(f"lexeme facts: {report['lexeme_facts']}")
    print(f"core clauses: {report['core_clauses']}")
    print(f"machinery clauses: {report['machinery_clauses']}")
    print(f"world clauses: {report['world_clauses']}")
    print(f"predicates: {report['predicates']}")
    print(f"consumed predicates: {report['consumed_predicates']}")
    print(f"inert predicates: {report['inert_predicates']}")
    print()
    print("top inert predicates by fact count:")
    for row in report["inert"]:
        files = ", ".join(row["files"])
        print(f"{row['predicate']:<32} {row['facts']:>6}  {files}")


def main(argv: Iterable[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--kb-root", type=Path, default=Path("kb"))
    parser.add_argument("--core-root", type=Path, default=Path("kb/core"))
    parser.add_argument("--limit", type=int, default=30)
    parser.add_argument("--json", action="store_true")
    args = parser.parse_args(argv)

    report = build_report(args.kb_root, args.core_root, max(0, args.limit))
    if args.json:
        print(json.dumps(report, indent=2, sort_keys=True))
    else:
        print_report(report)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
