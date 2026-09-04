"""Wire encoding for journal records."""

from __future__ import annotations

import json
import zlib


class CorruptJournal(RuntimeError):
    pass


def _canonical(payload: dict) -> bytes:
    return json.dumps(
        payload, sort_keys=True, ensure_ascii=False, separators=(",", ":")
    ).encode("utf-8")


def encode_record(revision: int, key: str, value: object) -> bytes:
    payload = {"revision": revision, "key": key, "value": value}
    record = dict(payload)
    record["crc32"] = f"{zlib.crc32(_canonical(payload)) & 0xffffffff:08x}"
    return _canonical(record) + b"\n"


def decode_record(line: bytes) -> tuple[int, str, object]:
    """Legacy decoder: the incident report says its validation is incomplete."""
    try:
        record = json.loads(line)
        return int(record["revision"]), str(record["key"]), record["value"]
    except (KeyError, TypeError, ValueError, json.JSONDecodeError) as exc:
        raise CorruptJournal(str(exc)) from exc
