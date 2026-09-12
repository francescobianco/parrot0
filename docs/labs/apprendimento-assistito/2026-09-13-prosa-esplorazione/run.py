#!/usr/bin/env python3
"""Run diagnostic dialogues on the complete live KB; never save a session.

Usage: python3 PATH/run.py PATH/01-parafrasi.json
Factorized source sentences are diagnostics, never benchmark improvements.
"""
import json
import os
from pathlib import Path
import subprocess
import sys
import time

manifest = Path(sys.argv[1]).resolve()
repo = Path(__file__).resolve().parents[4]
cases = json.loads(manifest.read_text())
env = dict(os.environ, PARROT0_SESSION="", PARROT0_WIKI_FETCH="0",
           PARROT0_TOOLS="1", PARROT0_LANG="en",
           PARROT0_PROFILE="kb/profiles/agi.p0")
results = []
for case in cases:
    assert all(turn.strip() != "/save" for turn in case["turns"])
    started = time.monotonic()
    process = subprocess.run(
        ["./bin/parrot0"], cwd=repo, env=env,
        input="\n".join(case["turns"] + ["/quit"]) + "\n",
        text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
        timeout=600, check=True)
    raw = process.stdout
    replies = raw.split(">>> ")[1:]
    if len(replies) != len(case["turns"]) + 1:
        raise RuntimeError(f"Unaligned transcript: {case['id']}")
    result = dict(id=case["id"], kind=case["kind"],
                  elapsed=round(time.monotonic() - started, 3),
                  turns=[dict(prompt=prompt, reply=reply.strip())
                         for prompt, reply in zip(case["turns"], replies)])
    results.append(result)
    manifest.with_name(manifest.stem + "-results.json").write_text(
        json.dumps(results, indent=2, ensure_ascii=False) + "\n")
    manifest.with_name(case["id"] + "-transcript.txt").write_text(raw)
    print(case["id"], result["elapsed"], "seconds", flush=True)
