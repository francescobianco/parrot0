#!/usr/bin/env bash
# Full living KB; no save, no external services, no fabricated comprehension score.
set -euo pipefail
cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.."
make build >&2
probe_dir=$(mktemp -d /tmp/parrot0-kb-abstraction.XXXXXX)
trap 'rm -rf -- "$probe_dir"' EXIT
objects=()
for object in obj/*.o; do
    [[ "$object" == obj/main.o ]] || objects+=("$object")
done
curl_libs=()
if nm -u obj/*.o | rg ' U curl_' > /dev/null; then
    curl_libs=(-l:libcurl.so.4)
fi
"${CC:-cc}" -std=c11 -Wall -Wextra -Wpedantic -O2 -Isrc \
    tests/probes/kb_abstraction_probe.c "${objects[@]}" "${curl_libs[@]}" \
    -o "$probe_dir/probe"
PARROT0_SESSION= PARROT0_PROFILE=kb/profiles/agi.p0 PARROT0_LANG=en \
    "$probe_dir/probe"
