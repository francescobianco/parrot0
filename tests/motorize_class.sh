#!/usr/bin/env bash
# Focused KB-first regressions for docs/plans/motorize-the-class.md.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "motorize_class: binary not built ($BIN)" >&2; exit 1; }

pass=0 fail=0
ok() { echo "PASS motorize_class: $1"; pass=$((pass+1)); }
no() { echo "FAIL motorize_class: $1" >&2; fail=$((fail+1)); }
call() { printf '%s\n' "$1"; }

out="$( {
  call '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'

  # The diet recognizer is a fixed engine over diet_cue/1, matched as whole tokens.
  call '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"diet_cue","args":["eat"]}}}'
  call '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"what does a lion eat?"}}}'
  call '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"diet_cue","args":["eat"]}}}'
  call '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"what does a lion eat?"}}}'
  call '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"diet_cue","args":["eat"]}}}'
  call '{"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"what does a lion eat?"}}}'

  # Passive creation extraction is also KB-first: written -> wrote and by-marker
  # are facts. Removing either one removes recognition in the same process.
  call '{"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"creation_verb","args":["wrote"]}}}'
  call '{"jsonrpc":"2.0","id":9,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"creation_verb_form","args":["wrote","written"]}}}'
  call '{"jsonrpc":"2.0","id":10,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"creation_passive_agent_marker","args":["by"]}}}'
  call '{"jsonrpc":"2.0","id":11,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"The Odyssey was written by Homer."}}}'
  call '{"jsonrpc":"2.0","id":12,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Who wrote the Odyssey?"}}}'
  call '{"jsonrpc":"2.0","id":13,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"creation_verb_form","args":["wrote","written"]}}}'
  call '{"jsonrpc":"2.0","id":14,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"The Iliad was written by Homer."}}}'
  call '{"jsonrpc":"2.0","id":15,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Who wrote the Iliad?"}}}'

  # Request forms stay KB-side too: removing the cue removes routing, while
  # topic/content facts remain available.
  call '{"jsonrpc":"2.0","id":16,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Give me a poetic comparison for time passing."}}}'
  call '{"jsonrpc":"2.0","id":17,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["creative_metaphor_request","poetic comparison"]}}}'
  call '{"jsonrpc":"2.0","id":18,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Give me a poetic comparison for time passing."}}}'
  call '{"jsonrpc":"2.0","id":19,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["creative_metaphor_request","poetic comparison"]}}}'
  call '{"jsonrpc":"2.0","id":20,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Give me a poetic comparison for time passing."}}}'
  call '{"jsonrpc":"2.0","id":21,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Can you offer career counsel for someone starting a new job?"}}}'
  call '{"jsonrpc":"2.0","id":22,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["activity_request","career counsel"]}}}'
  call '{"jsonrpc":"2.0","id":23,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Can you offer career counsel for someone starting a new job?"}}}'
  call '{"jsonrpc":"2.0","id":24,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["activity_request","career counsel"]}}}'
  call '{"jsonrpc":"2.0","id":25,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Can you offer career counsel for someone starting a new job?"}}}'
} | PARROT0_SESSION= PARROT0_PROFILE= PARROT0_WORLD_FACTS=1 "$BIN" --mcp-engine 2>/dev/null)"

line() { printf '%s\n' "$out" | grep -F "\"id\":$1,"; }
has() {
  local id="$1" pattern="$2" label="$3"
  if line "$id" | grep -Fqi "$pattern"; then ok "$label"; else no "$label: $(line "$id")"; fi
}
lacks() {
  local id="$1" pattern="$2" label="$3"
  if line "$id" | grep -Fqi "$pattern"; then no "$label: $(line "$id")"; else ok "$label"; fi
}

rpc_ok=1
for id in $(seq 1 25); do
  row="$(line "$id" || true)"
  rows="$(printf '%s\n' "$row" | sed '/^$/d' | wc -l)"
  if [ "$rows" -ne 1 ] || printf '%s\n' "$row" | grep -Fq '"isError":true' ||
     printf '%s\n' "$row" | grep -Fq '"error":'; then
    rpc_ok=0
    break
  fi
done
if [ "$rpc_ok" -eq 1 ]; then
  ok "every mutation/probe returned exactly one successful RPC response"
else
  no "missing or errored RPC response at id $id"
fi

lacks 3 'A lion eats zebra.' "diet cue absent before teaching"
has   5 'A lion eats zebra.' "diet cue taught at runtime"
lacks 7 'A lion eats zebra.' "diet cue retract removes recognition"

has  11 'Learned: created_by(homer, odyssey, wrote).' "passive creation prose extracts created_by/3"
has  12 'Homer.' "canonical created_by answers active query"
 lacks 14 'Learned: created_by(homer, iliad, wrote).' "passive verb form retract removes extraction"
lacks 15 'Homer.' "unextracted passive fact is not hallucinated"
lacks 16 'Time is a river' "metaphor request cue absent before teaching"
has   18 'Time is a river carrying yesterday out of reach.' "metaphor request cue taught at runtime"
lacks 20 'Time is a river' "metaphor request cue retract removes recognition"
lacks 21 'Learn the team' "activity request cue absent before teaching"
has   23 'Learn the team' "activity request cue taught at runtime"
lacks 25 'Learn the team' "activity request cue retract removes recognition"

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
