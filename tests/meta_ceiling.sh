#!/usr/bin/env bash
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "no binary" >&2; exit 1; }
pass=0; fail=0
ok() { echo "PASS meta: $1"; pass=$((pass+1)); }
no() { echo "FAIL meta: $1" >&2; fail=$((fail+1)); }

base="$(mktemp /tmp/meta_test.XXXXXX.p0)"
trap 'rm -f "$base"' EXIT
cat > "$base" <<'P0'
is_a(dog, mammal).  is_a(cat, mammal).  is_a(sparrow, bird).
is_a(salmon, fish). is_a(shark, fish).
located_in(paris, france).  located_in(lyon, france).
capital_of_country(france, paris).
larger_than(france, italy).  larger_than(italy, belgium).
can_fly(sparrow).  can_fly(eagle).
flightless(penguin).
category_member(dog, mammal).  category_member(cat, mammal).
category_member(sparrow, bird).
fact_confidence(capital_of_country(france, paris), high).
argument(premise, "socrates is a man").
argument(premise, "all men are mortal").
argument(conclusion, "socrates is mortal").
argument_supports("socrates is a man", "socrates is mortal").
argument_supports("all men are mortal", "socrates is mortal").
P0

out="$( {
  printf '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}\n'
  printf '{"jsonrpc":"2.0","method":"notifications/initialized"}\n'
  printf '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"relation_domain","args":["is_a",null]}}}\n'
  printf '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"edge","args":["paris","france","located_in"]}}}\n'
  printf '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"incompatible","args":["bird","mammal"]}}}\n'
  printf '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"confidence_weight","args":["capital_of_country(france, paris)",null]}}}\n'
  printf '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"justified","args":["socrates is mortal"]}}}\n'
} | PARROT0_BASE="$base" PARROT0_SESSION="" PARROT0_WORLD_FACTS=0 PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

l() { printf '%s\n' "$out" | grep -F "\"id\":$1"; }

l 2 | grep -q 'taxonomy' && ok "L4: relation_domain(is_a) = taxonomy" || { no "L4: $(l 2)"; }
l 3 | grep -q 'provable.*true' && ok "L5: edge(paris,france,located_in)" || { no "L5: $(l 3)"; }
l 4 | grep -q 'provable.*true' && ok "L12: incompatible(bird,mammal)" || { no "L12: $(l 4)"; }
l 5 | grep -q 'high' && ok "L9: confidence_weight = high (prob/2)" || { no "L9: $(l 5)"; }
l 6 | grep -q 'provable.*true' && ok "L8: justified(socrates is mortal)" || { no "L8: $(l 6)"; }

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]