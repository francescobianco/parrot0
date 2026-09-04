"""Small Unix advisory-lock adapter used by the journal."""

from __future__ import annotations

import fcntl
from contextlib import contextmanager
from pathlib import Path


@contextmanager
def exclusive(path: Path):
    # BUG CA-219: callers currently pass the journal itself.  Compaction replaces
    # that inode, so a stable sibling lock file is required by the new contract.
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("a+b") as stream:
        fcntl.flock(stream.fileno(), fcntl.LOCK_EX)
        try:
            yield
        finally:
            fcntl.flock(stream.fileno(), fcntl.LOCK_UN)
