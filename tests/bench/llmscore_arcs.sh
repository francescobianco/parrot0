#!/usr/bin/env bash
# Focused ratchet for the 19 zeros in the 2026-07-27 LLMSCORE report.
# This is intentionally not part of the full suite.  Every prompt gets a fresh
# process and a hard one-second deadline, matching the local interview contract.
set -u

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BIN="$ROOT/bin/parrot0"
pass=0 fail=0

probe() { # id, exact prompt, discriminating answer fragment
    local id="$1" prompt="$2" want="$3" got rc
    got="$(printf '%s\n/quit\n' "$prompt" |
        timeout 1 env PARROT0_BASE= PARROT0_SESSION= \
            PARROT0_WORLD_FACTS=1 "$BIN" 2>/dev/null)"
    rc=$?
    got="${got%%$'\n'*}"
    if [ "$rc" -eq 0 ] && [[ "$got" == *"$want"* ]]; then
        echo "PASS llmscore-arcs $id"
        pass=$((pass + 1))
    else
        echo "FAIL llmscore-arcs $id rc=$rc want=[$want] got=[$got]" >&2
        fail=$((fail + 1))
    fi
}

probe 01 \
  "If a library contained only one copy of every book ever written, how would you design a retrieval system that could satisfy a million simultaneous requests without digital technology?" \
  "pneumatic tubes"
probe 02 \
  "How would you create a dish that incorporates ingredients from three unrelated culinary traditions while preserving the cultural significance of each?" \
  "Italian polenta"
probe 03 \
  "What is the minimum number of cuts required to separate a Möbius strip into two distinct pieces, and can you explain why?" \
  "centerline cut"
probe 04 \
  "If you had to encode the entire history of human civilization in a 280-character tweet, what information would you prioritize and why?" \
  "Hunters learned"
probe 05 \
  "What method would enable a blind person to learn a musical instrument that typically relies on visual cues, and what principles would it be based on?" \
  "Braille music"
probe 06 \
  "In a world where gravity occasionally reverses for a few seconds, what urban planning strategies could mitigate its effects on daily life?" \
  "ceiling handrails"
probe 08 \
  "How could a machine be designed to generate jokes that reliably make a specific individual laugh without pre‑programming any jokes?" \
  "contextual bandit"
probe 09 \
  "Why can the phrase 'the early bird catches the worm' be seen as an example of Bayesian reasoning, and can you provide concrete examples?" \
  "higher prior probability"
probe 10 \
  "If you were to translate the emotional tone of a piece of music into a color palette, what would the first movement of Beethoven's Fifth Symphony look like, and why?" \
  "charcoal black"
probe 11 \
  "Can you construct a logical argument that demonstrates that any sufficiently large group of strangers must contain at least two people who share a birthday?" \
  "367 people"
probe 12 \
  "Imagine a new sport that combines elements of chess and parkour; what are its basic rules and scoring system?" \
  "Checkpoint Chess"
probe 13 \
  "What philosophical implications would arise if humans discovered that consciousness is a measurable, transferable resource like electricity?" \
  "unequal personhood"
probe 14 \
  "How would you design an experiment to test whether a dog can understand the concept of 'tomorrow' as opposed to just immediate reward?" \
  "24-hour discrimination"
probe 15 \
  "If all metaphors in language were replaced by literal statements, how might poetry change, and what would be lost?" \
  "compression, ambiguity"
probe 16 \
  "How could a community of ants be used to solve a classic maze problem, and what constraints would limit its efficiency?" \
  "pheromone reinforcement"
probe 17 \
  "What is the most elegant proof of the Pythagorean theorem that does not rely on diagrams?" \
  "a squared + b squared"
probe 18 \
  "If a city were built entirely underground with no natural light, how might its architecture evolve to create a sense of 'day' and 'night' for its inhabitants?" \
  "melatonin release"
probe 19 \
  "How would you express the navigation instructions for a robot moving through a crowded market while avoiding collisions in formal logical notation?" \
  "d_safe"
probe 20 \
  "If an alien species communicated exclusively through chemical signals, what strategies might humans develop to decode their messages?" \
  "chromatography"

# Runtime-growth controls.  These prove that the strategy and topic surfaces are
# live knowledge: the same binary acquires and loses them without rebuilding.
MCP_DIR="${TMPDIR:-/tmp}/parrot0-mcp-llmscore-arcs-$$"
export PARROT0_MCP_DIR="$MCP_DIR"
cleanup() {
    "$ROOT/scripts/mcp-live.sh" stop >/dev/null 2>&1 || true
    rm -f "$MCP_DIR/res" "$MCP_DIR/log"
    rmdir "$MCP_DIR" 2>/dev/null || true
}
trap cleanup EXIT
"$ROOT/scripts/mcp-live.sh" start PARROT0_BASE= PARROT0_WORLD_FACTS=1 >/dev/null
call() { "$ROOT/scripts/mcp-live.sh" call "$1" "$2"; }

act_prompt="Please beta-frame a library with one copy for mass access."
before="$(call gen.respond "{\"input\":\"$act_prompt\"}")"
call kb.assert \
  '{"pred":"strategy_cue","args":["concrete_design_strategy","beta-frame"]}' \
  >/dev/null
after="$(call gen.respond "{\"input\":\"$act_prompt\"}")"
call kb.retract \
  '{"pred":"strategy_cue","args":["concrete_design_strategy","beta-frame"]}' \
  >/dev/null
retracted="$(call gen.respond "{\"input\":\"$act_prompt\"}")"
if [[ "$before" != *"Regional request depots"* ]] &&
   [[ "$after" == *"Regional request depots"* ]] &&
   [[ "$retracted" != *"Regional request depots"* ]]; then
    echo "PASS llmscore-arcs runtime strategy cue growth"
    pass=$((pass + 1))
else
    echo "FAIL llmscore-arcs runtime strategy cue growth" >&2
    fail=$((fail + 1))
fi

topic_prompt="How would you design a retrieval system for archive-alpha large-scale access?"
before="$(call gen.respond "{\"input\":\"$topic_prompt\"}")"
call kb.assert \
  '{"pred":"topic_evidence","args":["manual_library_topic","archive-alpha"]}' \
  >/dev/null
call kb.assert \
  '{"pred":"topic_gate","args":["manual_library_topic","archive-alpha"]}' \
  >/dev/null
after="$(call gen.respond "{\"input\":\"$topic_prompt\"}")"
call kb.retract \
  '{"pred":"topic_gate","args":["manual_library_topic","archive-alpha"]}' \
  >/dev/null
call kb.retract \
  '{"pred":"topic_evidence","args":["manual_library_topic","archive-alpha"]}' \
  >/dev/null
retracted="$(call gen.respond "{\"input\":\"$topic_prompt\"}")"
if [[ "$before" != *"Regional request depots"* ]] &&
   [[ "$after" == *"Regional request depots"* ]] &&
   [[ "$retracted" != *"Regional request depots"* ]]; then
    echo "PASS llmscore-arcs runtime topic cue growth"
    pass=$((pass + 1))
else
    echo "FAIL llmscore-arcs runtime topic cue growth" >&2
    fail=$((fail + 1))
fi

proof="$(call kb.explain \
  '{"pred":"reasoning_plan_candidate","args":["concrete_design","proposal","1","domain_required"]}')"
if [[ "$proof" == *'"provable":true'* ]] &&
   [[ "$proof" == *"strategy_shape"* ]] &&
   [[ "$proof" == *"shape_facet"* ]]; then
    echo "PASS llmscore-arcs plan clause proof"
    pass=$((pass + 1))
else
    echo "FAIL llmscore-arcs plan clause proof got=[$proof]" >&2
    fail=$((fail + 1))
fi

cleanup
trap - EXIT
echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
