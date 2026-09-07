#!/usr/bin/env python3
"""Convert a .p0t hermetic header to the live KB (TEST_TODO §0.0 R1).

    scripts/p0t-live-header.py tests/p0t/…/file.p0t [more files]

Rewrites, in the FIRST `[mock …]` block only:
  [mock hermetic]            -> [mock live]  (+ a dated comment)
  !set PARROT0_BASE=         -> !set PARROT0_BASE=kb/core/base.p0
  !set PARROT0_WORLD_FACTS=0 -> !set PARROT0_WORLD_FACTS=1
  !set PARROT0_PROFILE=      -> !set PARROT0_PROFILE=kb/profiles/agi.p0
and adds the profile line when the block has none. Nothing else is touched:
the file's assertions are the test's business, not the header's.
"""
import re, sys

NOTE = ("# gen505y — R1 (TEST_TODO §0.0): la KB intera, non un contesto amputato.\n")

def convert(path):
    src = open(path).read()
    m = re.search(r'^\[mock [a-z]+\]\n((?:!set [^\n]*\n)*)', src, re.M)
    if not m:
        print(f"{path}: no [mock] block, skipped"); return False
    block = m.group(0)
    new = block.replace('[mock hermetic]\n', '[mock live]\n' + NOTE, 1)
    new = new.replace('[mock core]\n', '[mock live]\n' + NOTE, 1)
    new = re.sub(r'^!set PARROT0_BASE=\n', '!set PARROT0_BASE=kb/core/base.p0\n', new, flags=re.M)
    new = re.sub(r'^!set PARROT0_WORLD_FACTS=0\n', '!set PARROT0_WORLD_FACTS=1\n', new, flags=re.M)
    new = re.sub(r'^!set PARROT0_PROFILE=\n', '!set PARROT0_PROFILE=kb/profiles/agi.p0\n', new, flags=re.M)
    if 'PARROT0_PROFILE' not in new:
        new = new.rstrip('\n') + '\n!set PARROT0_PROFILE=kb/profiles/agi.p0\n'
    if new == block:
        print(f"{path}: already live"); return False
    open(path, 'w').write(src.replace(block, new, 1))
    print(f"{path}: converted"); return True

if __name__ == '__main__':
    for p in sys.argv[1:]: convert(p)
