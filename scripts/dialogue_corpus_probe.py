#!/usr/bin/env python3
"""Run a high-variety dialogue corpus through real parrot0 REPL sessions.

This is a diagnostic instrument for LEARN_PROTOCOL campaigns, not a test suite
and not a trainer.  The input TSV groups turns into persistent conversations;
one fresh parrot0 process is used per dialogue.  No /save is sent.  The tool
reports broad observable properties and representative failures without
pretending that heuristics are semantic ground truth.
"""

from __future__ import annotations

import argparse
import collections
import csv
import os
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


@dataclass(frozen=True)
class Turn:
    dialogue: str
    number: int
    family: str
    language: str
    expected_move: str
    depth: int
    variation: str
    critical: str
    prompt: str


WALL = re.compile(
    r"(?:don't understand|not sure i followed|didn't quite catch|beyond me|"
    r"don't know (?:much )?about|non capisco|non sono sicuro|non ho capito|"
    r"non so (?:molto )?di|oltre le mie capacita)",
    re.I,
)
CLARIFY = re.compile(
    r"(?:do you mean|which one|what do you mean|can you clarify|can you tell me|"
    r"could you specify|intendi|quale dei|puoi chiarire|puoi specificare|"
    r"puoi dirmi|che cosa vuoi)",
    re.I,
)
DECLINE = re.compile(
    r"(?:i can't|i cannot|i don't have|non posso|non ho abbastanza|"
    r"non dispongo|non sarebbe sicuro)",
    re.I,
)


def read_corpus(path: Path) -> list[Turn]:
    rows: list[Turn] = []
    with path.open(encoding="utf-8", newline="") as handle:
        reader = csv.DictReader(handle, delimiter="\t")
        required = {
            "dialogue", "turn", "family", "language", "expected_move",
            "depth", "variation", "critical", "prompt",
        }
        if set(reader.fieldnames or ()) != required:
            raise SystemExit(
                f"{path}: columns must be exactly {', '.join(sorted(required))}"
            )
        for line, row in enumerate(reader, 2):
            try:
                item = Turn(
                    dialogue=row["dialogue"], number=int(row["turn"]),
                    family=row["family"], language=row["language"],
                    expected_move=row["expected_move"], depth=int(row["depth"]),
                    variation=row["variation"], critical=row["critical"],
                    prompt=row["prompt"],
                )
            except (TypeError, ValueError) as exc:
                raise SystemExit(f"{path}:{line}: {exc}") from exc
            if not item.dialogue or not item.prompt or "\t" in item.prompt:
                raise SystemExit(f"{path}:{line}: empty id/prompt or tab in prompt")
            rows.append(item)
    if not rows:
        raise SystemExit(f"{path}: empty corpus")
    return rows


def extract_replies(stdout: str) -> list[str]:
    replies: list[str] = []
    for chunk in stdout.split("<<EOT>>")[:-1]:
        lines = [line.strip() for line in chunk.splitlines() if line.strip()]
        lines = [line for line in lines if not line.startswith((
            "parrot0 [", "mode:", "say something", "you>", "parrot0: bye"
        ))]
        if lines:
            replies.append(" ".join(lines))
        else:
            replies.append("")
    return replies


def run_dialogue(binary: Path, turns: list[Turn]) -> tuple[list[str], str]:
    transcript = "\n".join(turn.prompt for turn in turns) + "\n/quit\n"
    env = os.environ.copy()
    env.update({
        "PARROT0_PROFILE": "kb/profiles/agi.p0",
        "PARROT0_SESSION": "",
        "PARROT0_WORLD_FACTS": "1",
        "PARROT0_TOOLS": "0",
        "PARROT0_WIKI_FETCH": "0",
        "PARROT0_LANG": turns[0].language if turns[0].language in {"it", "en"} else "en",
        "PARROT0_EOT": "<<EOT>>",
    })
    proc = subprocess.run(
        [str(binary)], input=transcript, text=True, capture_output=True,
        cwd=ROOT, env=env, timeout=45, check=False,
    )
    return extract_replies(proc.stdout), proc.stderr


def move_matches(expected: str, reply: str) -> bool:
    wall = bool(WALL.search(reply))
    clarify = bool(CLARIFY.search(reply)) or ("?" in reply and not wall)
    decline = wall or bool(DECLINE.search(reply))
    if expected == "clarify":
        return clarify
    if expected == "decline":
        return decline
    if expected in {"answer", "ack", "continue", "explain", "plan", "repair"}:
        return bool(reply) and not wall
    return False


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("corpus", type=Path)
    parser.add_argument("--binary", type=Path, default=ROOT / "bin/parrot0")
    parser.add_argument("--examples", type=int, default=3)
    args = parser.parse_args()
    rows = read_corpus(args.corpus)
    grouped: dict[str, list[Turn]] = collections.OrderedDict()
    for turn in rows:
        grouped.setdefault(turn.dialogue, []).append(turn)
    for dialogue, turns in grouped.items():
        numbers = [turn.number for turn in turns]
        if numbers != list(range(1, len(turns) + 1)):
            raise SystemExit(f"{dialogue}: turns must be consecutive from 1")

    stats: dict[str, collections.Counter[str]] = collections.defaultdict(collections.Counter)
    examples: dict[str, list[tuple[Turn, str]]] = collections.defaultdict(list)
    all_pairs: list[tuple[Turn, str]] = []
    mismatched = 0
    for index, (dialogue, turns) in enumerate(grouped.items(), 1):
        replies, stderr = run_dialogue(args.binary, turns)
        if len(replies) != len(turns):
            mismatched += 1
            print(
                f"warning: {dialogue}: {len(turns)} prompts, {len(replies)} replies; "
                f"stderr tail={stderr[-160:]!r}", file=sys.stderr,
            )
        replies += [""] * max(0, len(turns) - len(replies))
        previous = ""
        for turn, reply in zip(turns, replies):
            fam = turn.family
            counter = stats[fam]
            counter["turns"] += 1
            counter["wall"] += int(bool(WALL.search(reply)))
            counter["clarify"] += int(bool(CLARIFY.search(reply)) or "?" in reply)
            counter["move_match"] += int(move_matches(turn.expected_move, reply))
            counter["depth2plus"] += int(turn.depth >= 2)
            normalized = re.sub(r"\W+", " ", reply.lower()).strip()
            counter["repeat"] += int(bool(previous) and normalized == previous)
            counter["empty"] += int(not reply)
            previous = normalized
            all_pairs.append((turn, reply))
            if (WALL.search(reply) or not move_matches(turn.expected_move, reply)) and \
                    len(examples[fam]) < args.examples:
                examples[fam].append((turn, reply))
        if index % 10 == 0:
            print(f"progress: {index}/{len(grouped)} dialogues", file=sys.stderr)

    print("GD1 CORPUS PROBE")
    print(f"corpus={args.corpus}")
    print(f"dialogues={len(grouped)} turns={len(rows)} reply_alignment_errors={mismatched}")
    print()
    print("family\tturns\twall\tmove_match\tclarify\trepeat\tempty\tdepth2+")
    for family in sorted(stats):
        c = stats[family]
        print(
            f"{family}\t{c['turns']}\t{c['wall']}\t{c['move_match']}\t"
            f"{c['clarify']}\t{c['repeat']}\t{c['empty']}\t{c['depth2plus']}"
        )
    total = collections.Counter()
    for counter in stats.values():
        total.update(counter)
    print(
        f"TOTAL\t{total['turns']}\t{total['wall']}\t{total['move_match']}\t"
        f"{total['clarify']}\t{total['repeat']}\t{total['empty']}\t{total['depth2plus']}"
    )
    print("\nREPRESENTATIVE FAILURES (heuristic; inspect semantically)")
    for family in sorted(examples):
        print(f"\n[{family}]")
        for turn, reply in examples[family]:
            print(
                f"{turn.dialogue}.{turn.number} expected={turn.expected_move} "
                f"variation={turn.variation}\n  > {turn.prompt}\n  < {reply}"
            )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
