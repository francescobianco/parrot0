#!/usr/bin/env python3
"""Black-box probe copied beside the candidate by judge.py."""

from __future__ import annotations

import json
import multiprocessing as mp
import os
import subprocess
import sys
import tempfile
import zlib
from pathlib import Path


def report(name: str, passed: bool, detail: str = "") -> None:
    clean = " ".join(str(detail).split())[:300]
    print(f"CHECK {name} {'PASS' if passed else 'FAIL'} {clean}", flush=True)


def worker(path: str, prefix: str, count: int) -> None:
    from journal import Journal

    journal = Journal(path)
    for index in range(count):
        journal.append(f"{prefix}-{index}", {"owner": prefix, "n": index})


def canonical(payload: dict) -> bytes:
    return json.dumps(
        payload, sort_keys=True, ensure_ascii=False, separators=(",", ":")
    ).encode("utf-8")


def main() -> None:
    try:
        from journal import CorruptJournal, Journal

        api_ok = isinstance(CorruptJournal, type) and callable(Journal)
    except Exception as exc:
        report("import_api", False, repr(exc))
        return
    report("import_api", api_ok)

    with tempfile.TemporaryDirectory(prefix="journal-data-") as tmp_text:
        path = Path(tmp_text) / "events.jl"
        try:
            journal = Journal(path)
            r1 = journal.append("alpha", {"n": 1})
            r2 = journal.append("beta", [1, 2, 3])
            r3 = journal.append("alpha", {"n": 3})
            state = journal.load()
            report("roundtrip_revision", (r1, r2, r3) == (1, 2, 3) and
                   state == {"alpha": {"n": 3}, "beta": [1, 2, 3]})
        except Exception as exc:
            report("roundtrip_revision", False, repr(exc))

        try:
            restarted = Journal(path)
            r4 = restarted.append("gamma", True)
            report("restart_monotonicity", r4 == 4 and restarted.load()["gamma"] is True)
        except Exception as exc:
            report("restart_monotonicity", False, repr(exc))

        try:
            value = {"line": "prima\nseconda", "emoji": "🦜", "city": "Łódź"}
            restarted.append("unicode-chiave", value)
            report("unicode_and_newlines", restarted.load()["unicode-chiave"] == value)
        except Exception as exc:
            report("unicode_and_newlines", False, repr(exc))

        try:
            with path.open("ab") as stream:
                stream.write(b'{"revision":999,"key":"crash"')
                stream.flush()
                os.fsync(stream.fileno())
            recovered = Journal(path).load()
            report("truncated_tail_recovery", "crash" not in recovered and "alpha" in recovered)
        except Exception as exc:
            report("truncated_tail_recovery", False, repr(exc))

        # Use a separate journal: only an incomplete *tail* may be ignored.
        corrupt = Path(tmp_text) / "corrupt.jl"
        try:
            payload = {"revision": 1, "key": "x", "value": 7}
            record = dict(payload)
            record["crc32"] = f"{zlib.crc32(canonical(payload)) & 0xffffffff:08x}"
            corrupt.write_text(json.dumps(record, ensure_ascii=False) + "\n", encoding="utf-8")
            data = bytearray(corrupt.read_bytes())
            pos = data.index(b'7')
            data[pos] = ord('8')
            corrupt.write_bytes(data)
            rejected = False
            try:
                Journal(corrupt).load()
            except CorruptJournal:
                rejected = True
            report("complete_corruption_rejected", rejected)
        except Exception as exc:
            report("complete_corruption_rejected", False, repr(exc))

        compact_path = Path(tmp_text) / "compact.jl"
        try:
            compact = Journal(compact_path)
            for i in range(40):
                compact.append("same", {"i": i, "padding": "x" * 50})
            compact.append("other", 9)
            before = compact_path.stat().st_size
            compact.compact()
            after = compact_path.stat().st_size
            state = compact.load()
            next_revision = compact.append("after", 42)
            leftovers = list(compact_path.parent.glob(compact_path.name + ".tmp*"))
            report("compaction_atomic_state",
                   state == {"same": {"i": 39, "padding": "x" * 50}, "other": 9}
                   and after < before and next_revision > 41 and not leftovers)
        except Exception as exc:
            report("compaction_atomic_state", False, repr(exc))

        concurrent_path = Path(tmp_text) / "concurrent.jl"
        try:
            processes = [mp.Process(target=worker, args=(str(concurrent_path), f"p{i}", 25))
                         for i in range(4)]
            for process in processes:
                process.start()
            for process in processes:
                process.join(10)
            alive = [process for process in processes if process.is_alive()]
            for process in alive:
                process.kill()
                process.join()
            state = Journal(concurrent_path).load()
            lines = concurrent_path.read_text(encoding="utf-8").splitlines()
            revisions = [json.loads(line)["revision"] for line in lines]
            report("concurrent_writers", not alive and
                   all(process.exitcode == 0 for process in processes) and
                   len(state) == 100 and sorted(revisions) == list(range(1, 101)))
        except Exception as exc:
            report("concurrent_writers", False, repr(exc))

        cli_path = Path(tmp_text) / "cli.jl"
        try:
            append = subprocess.run(
                [sys.executable, "cli.py", str(cli_path), "append", "hello", '{"n":7}'],
                text=True, capture_output=True, timeout=5,
            )
            dump = subprocess.run(
                [sys.executable, "cli.py", str(cli_path), "dump"],
                text=True, capture_output=True, timeout=5,
            )
            rendered = json.loads(dump.stdout)
            report("cli_compatibility", append.returncode == 0 and dump.returncode == 0 and
                   rendered == {"hello": {"n": 7}})
        except Exception as exc:
            report("cli_compatibility", False, repr(exc))


if __name__ == "__main__":
    main()
