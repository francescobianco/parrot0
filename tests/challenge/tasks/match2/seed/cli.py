#!/usr/bin/env python3
"""Minimal CLI retained for operators and recovery scripts."""

from __future__ import annotations

import json
import sys

from journal import Journal


def main(argv: list[str]) -> int:
    if len(argv) < 3:
        print("usage: cli.py PATH append KEY JSON | cli.py PATH dump", file=sys.stderr)
        return 2
    journal = Journal(argv[1])
    if argv[2:] == ["dump"]:
        print(json.dumps(journal.load(), ensure_ascii=False, sort_keys=True))
        return 0
    if len(argv) == 5 and argv[2] == "append":
        revision = journal.append(argv[3], json.loads(argv[4]))
        print(revision)
        return 0
    print("invalid command", file=sys.stderr)
    return 2


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
