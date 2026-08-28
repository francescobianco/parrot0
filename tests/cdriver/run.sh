#!/usr/bin/env sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
if [ "$#" -lt 1 ]; then
    printf 'usage: %s driver.c [source.c ...]\n' "$0" >&2
    exit 2
fi

tmp=$(mktemp -d "${TMPDIR:-/tmp}/parrot0-cdriver.XXXXXX")
trap 'rm -rf "$tmp"' EXIT HUP INT TERM

sources=
for source in "$@"; do
    sources="$sources $ROOT/$source"
done

# The driver is checked in; this runner only compiles the requested sources.
# shellcheck disable=SC2086
${CC:-cc} -std=c11 -Wall -Wextra -Wpedantic -O2 -I"$ROOT/src" \
    -o "$tmp/driver" $sources
exec "$tmp/driver"
