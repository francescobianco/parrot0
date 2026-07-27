#!/usr/bin/env bash
# Deterministic ratchet for the living-model clauses introduced in gen365.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "model_graph: binary not built ($BIN)" >&2; exit 1; }

call() { printf '%s\n' "$1"; }

out="$({
  call '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'

  call '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"happened_in","args":["first_moon_landing","1969"]}}}'
  call '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"historical_before","args":["battle_of_waterloo","first_moon_landing"]}}}'
  call '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"lives_overlap","args":["charles_darwin","abraham_lincoln"]}}}'
  call '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"country_profile","args":["france","paris","europe","french"]}}}'
  call '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"shared_official_language","args":["australia","united_kingdom","english"]}}}'
  call '{"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"element_identity","args":["hydrogen","1","H"]}}}'
  call '{"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"element_before","args":["hydrogen","helium"]}}}'
  call '{"jsonrpc":"2.0","id":9,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"orbits_t","args":["moon","sun"]}}}'
  call '{"jsonrpc":"2.0","id":10,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"same_quantity","args":["rectangle","square","sides","4"]}}}'
  call '{"jsonrpc":"2.0","id":11,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"more_of","args":["jupiter","mars","moons"]}}}'
  call '{"jsonrpc":"2.0","id":12,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"greater_magnitude","args":["size","elephant","cat"]}}}'
  call '{"jsonrpc":"2.0","id":13,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"shared_sound","args":["dog","seal","bark"]}}}'
  call '{"jsonrpc":"2.0","id":14,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"word_relation","args":["quick","fast","synonymy"]}}}'
  call '{"jsonrpc":"2.0","id":15,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"shared_color","args":["banana","lemon","yellow"]}}}'

  # Direction and join discriminators: these must not become vacuous matches.
  call '{"jsonrpc":"2.0","id":20,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"historical_before","args":["first_moon_landing","battle_of_waterloo"]}}}'
  call '{"jsonrpc":"2.0","id":21,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"country_profile","args":["france","paris","asia","french"]}}}'
  call '{"jsonrpc":"2.0","id":22,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"element_before","args":["helium","hydrogen"]}}}'
  call '{"jsonrpc":"2.0","id":23,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"more_of","args":["mars","jupiter","moons"]}}}'

  # Proof-carrying joins: verify conclusions are derived, not stored answers.
  call '{"jsonrpc":"2.0","id":30,"method":"tools/call","params":{"name":"kb.explain","arguments":{"pred":"historical_lifespan","args":["albert_einstein","1879","1955"]}}}'
  call '{"jsonrpc":"2.0","id":31,"method":"tools/call","params":{"name":"kb.explain","arguments":{"pred":"country_profile","args":["france","paris","europe","french"]}}}'
  call '{"jsonrpc":"2.0","id":32,"method":"tools/call","params":{"name":"kb.explain","arguments":{"pred":"element_identity","args":["hydrogen","1","H"]}}}'
  call '{"jsonrpc":"2.0","id":33,"method":"tools/call","params":{"name":"kb.explain","arguments":{"pred":"orbits_t","args":["moon","sun"]}}}'
  call '{"jsonrpc":"2.0","id":34,"method":"tools/call","params":{"name":"kb.explain","arguments":{"pred":"same_quantity","args":["rectangle","square","sides","4"]}}}'
  call '{"jsonrpc":"2.0","id":35,"method":"tools/call","params":{"name":"kb.explain","arguments":{"pred":"word_relation","args":["quick","fast","synonymy"]}}}'
  call '{"jsonrpc":"2.0","id":36,"method":"tools/call","params":{"name":"kb.explain","arguments":{"pred":"shared_color","args":["banana","lemon","yellow"]}}}'

  # Prompt controls: the derived relations stay unreachable until their
  # comprehension frames are taught, then disappear again on retraction.
  call '{"jsonrpc":"2.0","id":40,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"what continent is paris in?"}}}'
  call '{"jsonrpc":"2.0","id":41,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"answer_frame","args":["continent","capital_in_continent"]}}}'
  call '{"jsonrpc":"2.0","id":42,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"what continent is paris in?"}}}'
  call '{"jsonrpc":"2.0","id":43,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"answer_frame","args":["continent","capital_in_continent"]}}}'
  call '{"jsonrpc":"2.0","id":44,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"what continent is paris in?"}}}'

  call '{"jsonrpc":"2.0","id":45,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"what language is associated with paris?"}}}'
  call '{"jsonrpc":"2.0","id":46,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"answer_frame","args":["language","capital_language"]}}}'
  call '{"jsonrpc":"2.0","id":47,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"what language is associated with paris?"}}}'
  call '{"jsonrpc":"2.0","id":48,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"answer_frame","args":["language","capital_language"]}}}'
  call '{"jsonrpc":"2.0","id":49,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"what language is associated with paris?"}}}'

  # A completely new cue must remain reachable beyond the old 128-frame cap.
  call '{"jsonrpc":"2.0","id":50,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"moon orbit"}}}'
  call '{"jsonrpc":"2.0","id":51,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"answer_frame","args":["orbit","orbits_t"]}}}'
  call '{"jsonrpc":"2.0","id":52,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"moon orbit"}}}'
  call '{"jsonrpc":"2.0","id":53,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"answer_frame","args":["orbit","orbits_t"]}}}'
  call '{"jsonrpc":"2.0","id":54,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"moon orbit"}}}'
} | PARROT0_SESSION= PARROT0_PROFILE= "$BIN" --mcp-engine 2>/dev/null)"

line() { printf '%s\n' "$out" | grep -F "\"id\":$1,"; }
pass=0
fail=0
ok() { echo "PASS model_graph: $1"; pass=$((pass+1)); }
no() { echo "FAIL model_graph: $1" >&2; fail=$((fail+1)); }

for id in 2 3 4 5 6 7 8 9 10 11 12 13 14 15; do
    if line "$id" | grep -q 'provable\\":true'; then
        ok "derived query $id"
    else
        no "derived query $id: $(line "$id")"
    fi
done

for id in 20 21 22 23; do
    if line "$id" | grep -q 'provable\\":false'; then
        ok "negative discriminator $id"
    else
        no "negative discriminator $id: $(line "$id")"
    fi
done

for id in 30 31 32 33 34 35 36; do
    if line "$id" | grep -q 'provable\\":true' &&
       line "$id" | grep -q 'explanation'; then
        ok "proof-carrying query $id"
    else
        no "missing proof $id: $(line "$id")"
    fi
done

if ! line 40 | grep -Fq 'Europe.' &&
   line 42 | grep -Fq 'Europe.' &&
   ! line 44 | grep -Fq 'Europe.'; then
    ok "prompt control: continent frame acquires and loses the derived Paris answer"
else
    no "continent prompt runtime growth: $(line 40) / $(line 42) / $(line 44)"
fi

if ! line 45 | grep -Fq 'French.' &&
   line 47 | grep -Fq 'French.' &&
   ! line 49 | grep -Fq 'French.'; then
    ok "prompt control: language frame acquires and loses the derived Paris answer"
else
    no "language prompt runtime growth: $(line 45) / $(line 47) / $(line 49)"
fi

if ! line 50 | grep -Fq 'Earth and sun.' &&
   line 52 | grep -Fq 'Earth and sun.' &&
   ! line 54 | grep -Fq 'Earth and sun.'; then
    ok "prompt control: a new cue beyond frame 128 acquires and loses orbit closure"
else
    no "new-cue prompt runtime growth: $(line 50) / $(line 52) / $(line 54)"
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
