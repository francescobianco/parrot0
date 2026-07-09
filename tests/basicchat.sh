#!/usr/bin/env bash
#
# basic-chat discovery harness (gen189).
#
# docs/plans/basic-chat.md is a catalogue of elementary prompts where parrot0
# fails, grouped into categories (## headings) with one `| N | `prompt` |` row
# each. This bench feeds every prompt to the current generation and measures
# COVERAGE — the fraction that get an *engaged* reply rather than falling through
# to the honest wall ("I don't understand that yet." / empty).
#
# Like chatbench.sh it NEVER fails the build: it prints a coverage score, broken
# down by category, to watch climb as we close categories one generation at a
# time. It is the ratchet that turns the plan file into measurable pressure
# (PRINCIPLES.md: the tests dispose; this one says how much is still open).
#
# Coverage here is a PROXY, not correctness: an "engaged" reply is one the
# dispatcher claimed. An honest "I don't know about X yet" still counts as
# engaged — knowing you don't know is a real answer; walling is the failure.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
PLAN="$ROOT/docs/plans/basic-chat.md"

# Use the real curated KB (these prompts probe world knowledge, not the hermetic
# unit-test surface). Callers may override via the environment.
: "${PARROT0_BASE:=$ROOT/kb/core/base.p0}"
: "${PARROT0_SESSION:=}"
# gen293: pin the default language to English so the coverage reading is
# deterministic regardless of the developer's OS locale (current_language is
# seeded from the launching ENV; on an Italian machine the wall came out in
# Italian and is_wall did not catch it, silently inflating every category's
# coverage). Italian PROMPTS still get Italian replies via per-turn detection —
# this only fixes the ambiguous/wall default, exactly as tests/run.sh does.
: "${PARROT0_LANG:=en}"
export PARROT0_BASE PARROT0_SESSION PARROT0_LANG

if [ ! -x "$BIN" ]; then
    echo "basicchat: binary not built ($BIN)" >&2
    exit 1
fi
if [ ! -f "$PLAN" ]; then
    echo "basicchat: plan file not found ($PLAN)" >&2
    exit 1
fi

is_wall() {
    # An empty reply, or the honest non-understanding wall in any of its forms
    # (EN and IT — the fallback speaks the conversation language, gen55/gen293).
    case "$1" in
        "") return 0 ;;
        "I don't understand that yet."*) return 0 ;;
        "I'm not sure I followed."*) return 0 ;;
        "I didn't quite catch that."*) return 0 ;;
        "Hmm, that's a bit beyond me"*) return 0 ;;
        "Hmm, I don't know about "*" yet."*) return 0 ;;
        "Non capisco ancora."*) return 0 ;;
        "Non sono sicuro di aver seguito."*) return 0 ;;
        "Non ho afferrato bene."*) return 0 ;;
        "Mmh, questo per ora va un po' oltre"*) return 0 ;;
        "Hmm, non so ancora nulla su "*) return 0 ;;
        *) return 1 ;;
    esac
}

cat_name=""
cat_total=0
cat_engaged=0
total=0
engaged=0

flush_cat() {
    [ -z "$cat_name" ] && return
    local pct=0
    [ "$cat_total" -gt 0 ] && pct=$(( cat_engaged * 100 / cat_total ))
    printf '  %3d%% (%2d/%2d)  %s\n' "$pct" "$cat_engaged" "$cat_total" "$cat_name"
}

while IFS= read -r line; do
    case "$line" in
        '## '*)
            flush_cat
            cat_name="${line#### }"
            cat_total=0; cat_engaged=0
            ;;
        '|'*'`'*'`'*)
            # Extract the text between the first pair of backticks.
            prompt="${line#*\`}"
            prompt="${prompt%%\`*}"
            [ -z "$prompt" ] && continue
            reply="$(printf '%s\n' "$prompt" | "$BIN" 2>/dev/null | head -1)"
            total=$((total+1)); cat_total=$((cat_total+1))
            if ! is_wall "$reply"; then
                engaged=$((engaged+1)); cat_engaged=$((cat_engaged+1))
            fi
            ;;
    esac
done < "$PLAN"
flush_cat

echo "---"
if [ "$total" -gt 0 ]; then
    pct=$(( engaged * 100 / total ))
    echo "basic-chat coverage: ${pct}% (${engaged}/${total} prompts engaged)"
else
    echo "basic-chat coverage: n/a (no prompts found)"
fi