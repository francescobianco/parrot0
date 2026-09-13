#!/usr/bin/env python3
"""Normalize legacy .p0t setup to the complete profile KB.

    scripts/p0t-live-header.py tests/p0t/…/file.p0t [more files]

All setup blocks are normalized. This does not validate legacy expectations:
results obtained with a reduced KB are obsolete and must be measured anew.
The engine also treats empty selectors as defaults, never as a KB off switch.
"""
from pathlib import Path
import re
import sys


def convert(path):
    path = Path(path)
    src = path.read_text()
    new = re.sub(r'^\[mock (?:hermetic|core)\]$', '[mock live]', src, flags=re.M)
    defaults = {
        'PARROT0_BASE': 'kb/core/base.p0',
        'PARROT0_PROFILE': 'kb/profiles/agi.p0',
        'PARROT0_LEXICON': 'kb/core/lexicon.p0',
    }
    for name, value in defaults.items():
        new = re.sub(r'^!set ' + name + r'=[ \t]*$',
                     '!set ' + name + '=' + value, new, flags=re.M)
    if new == src:
        print(f'{path}: already uses the profile KB')
        return False
    path.write_text(new)
    print(f'{path}: setup normalized; legacy expectations need validation')
    return True


if __name__ == '__main__':
    for p in sys.argv[1:]:
        convert(p)
