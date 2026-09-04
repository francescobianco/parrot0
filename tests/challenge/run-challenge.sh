#!/usr/bin/env bash
# Thin entrypoint for the comparative coding-agent laboratory.  Running it with
# no arguments is deliberately read-only: a paid/networked agent session must
# always be an explicit choice.
set -eu

ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)"
if [ "$#" -eq 0 ]; then
    set -- check
fi
exec python3 "$ROOT/tests/challenge/run_challenge.py" "$@"
