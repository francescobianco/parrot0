#!/usr/bin/env bash
# gen271 — the cue-chain migration ratchet (Track 5.4 of
# docs/plans/coding-agent-evolution.md): the number of hardcoded
# `cue(..) || cue(..)` OR-chains in src/brain is measured by parrot0's OWN
# outer-circle scanner (the same derived-plan primitive that migrates them)
# and MUST ONLY DESCEND. When a generation migrates a site, lower MAX to the
# new count; a generation that ADDS a chain breaks this gate on purpose —
# recognized vocabulary belongs in the KB (the cardinal KB-first rule), so
# either put the words in intent_cue facts or consciously justify the bump.
#
# gen271 baseline: 341 chains at gen256 -> 340 after the first real migration
# (src/brain/00-lex.c teach-verb guard -> kb_cue_match + kb/core/intents.p0).
# gen273: 85-translate-synth-world migrated whole -> 338. gen274: five of the
# six chains of 65-induce-verify-shell migrated (the wellbeing one skipped by
# per-chain applicability: `b` is not in scope there) -> 333. gen275: all 16
# chains of 50-self-research-loop migrated (zero skips) -> 317.
# gen333: LLMSCORE range/initial/anagram/relationship vocabulary moved behind
# kb_cue_match -> 246. Freeze the actual post-migration count: a new C OR-chain
# now fails instead of hiding inside the old 71-chain allowance.
# gen337 (T16): all 25 chains of 60-agent-tools.c migrated BY PARROT0 ITSELF
# (derived kb-first plan, outer-circle session; facts in kb/core/intents.p0)
# -> 226. NOTE: the gen334-336 generations had pushed the count to 251 with the
# ratchet red; this migration paid that debt back below the old 246 line too.
set -u
MAX=226
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "cuechains: binary not built" >&2; exit 1; }

cd "$ROOT"
reply="$(printf 'execute the kb-first plan for chains of calls to cue in src/brain\n/quit\n' \
         | "$BIN" 2>/dev/null | head -1)"
count="$(printf '%s\n' "$reply" | sed -n 's/.*orchain_scan found \([0-9][0-9]*\) OR-chains.*/\1/p')"

if [ -z "$count" ]; then
    echo "FAIL cuechains: could not read the chain count from: $reply"
    exit 1
fi
if [ "$count" -gt "$MAX" ]; then
    echo "FAIL cuechains: $count cue chains in src/brain, ratchet allows at most $MAX"
    echo "      (new vocabulary belongs in KB facts, not C cue chains — see the header)"
    exit 1
fi
echo "PASS cuechains: $count cue chains in src/brain (ratchet max $MAX, only descends)"
