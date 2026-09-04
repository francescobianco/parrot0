"""Append-only key/value journal (legacy implementation under repair)."""

from __future__ import annotations

import os
from pathlib import Path

from codec import CorruptJournal, decode_record, encode_record
from locking import exclusive

__all__ = ["CorruptJournal", "Journal"]


class Journal:
    def __init__(self, path):
        self.path = Path(path)

    def _events(self):
        if not self.path.exists():
            return []
        events = []
        for line in self.path.read_bytes().splitlines():
            if line:
                events.append(decode_record(line))
        return events

    def load(self):
        state = {}
        for _revision, key, value in self._events():
            state[key] = value
        return state

    def append(self, key, value):
        self.path.parent.mkdir(parents=True, exist_ok=True)
        with exclusive(self.path):
            events = self._events()
            revision = events[-1][0] + 1 if events else 1
            with self.path.open("ab") as stream:
                stream.write(encode_record(revision, key, value))
            return revision

    def compact(self):
        with exclusive(self.path):
            state = self.load()
            temporary = self.path.with_suffix(self.path.suffix + ".tmp")
            with temporary.open("wb") as stream:
                for revision, (key, value) in enumerate(state.items(), 1):
                    stream.write(encode_record(revision, key, value))
            os.replace(temporary, self.path)
