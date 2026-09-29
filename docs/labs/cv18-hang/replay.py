#!/usr/bin/env python3
"""Replay a live conversation one turn at a time; never leave a hung child.

The complete agi KB stays loaded. --lines selects input turns, for reduction
of a state-dependent failure without amputating the subject's knowledge.
"""
import argparse
import json
import os
from pathlib import Path
import selectors
import subprocess
import sys
import time


def replay(binary, turns, timeout=12, emit=print):
    env = dict(os.environ, PARROT0_PROFILE="kb/profiles/agi.p0",
               PARROT0_LANG="it", PARROT0_SESSION="")
    child = subprocess.Popen([str(Path(binary).resolve())], env=env,
                             stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT)
    selector = selectors.DefaultSelector()
    selector.register(child.stdout, selectors.EVENT_READ, "out")
    pending = b""

    def answer(seconds):
        nonlocal pending
        end = time.monotonic() + seconds
        while b">>> " not in pending:
            left = end - time.monotonic()
            if left <= 0:
                raise TimeoutError
            events = selector.select(left)
            for key, _ in events:
                data = os.read(key.fd, 65536)
                if not data:
                    selector.unregister(key.fileobj)
                    if key.data == "out":
                        raise RuntimeError("parrot0 exited before the next prompt")
                elif b"PARSE ERROR" in data:
                    raise RuntimeError(data.decode(errors="replace"))
                else:
                    pending += data
        reply, pending = pending.split(b">>> ", 1)
        return reply.decode(errors="replace").strip()

    rows = []
    try:
        answer(90)  # boot has no conversational deadline
        for index, turn in turns:
            started = time.monotonic()
            child.stdin.write((turn + "\n").encode())
            child.stdin.flush()
            try:
                reply = answer(timeout)
            except TimeoutError:
                row = dict(line=index, turn=turn, seconds=round(time.monotonic()-started, 3),
                           timeout=True)
                rows.append(row)
                emit(json.dumps(row, ensure_ascii=False))
                return rows
            row = dict(line=index, turn=turn, seconds=round(time.monotonic()-started, 3),
                       reply=reply)
            rows.append(row)
            emit(json.dumps(row, ensure_ascii=False))
        return rows
    finally:
        selector.close()
        if child.poll() is None:
            child.terminate()
        try:
            child.wait(timeout=3)
        except subprocess.TimeoutExpired:
            child.kill()
            child.wait()
        child.stdin.close()
        child.stdout.close()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--binary", default="bin/parrot0")
    parser.add_argument("--input", default="docs/labs/cv18-hang/in95.txt")
    parser.add_argument("--lines", help="comma-separated original line numbers")
    parser.add_argument("--timeout", type=float, default=12)
    args = parser.parse_args()
    selected = set(map(int, args.lines.split(","))) if args.lines else None
    turns = [(i, line) for i, line in enumerate(Path(args.input).read_text().splitlines(), 1)
             if selected is None or i in selected]
    result = replay(args.binary, turns, args.timeout, lambda s: print(s, flush=True))
    sys.exit(any(row.get("timeout") for row in result))
