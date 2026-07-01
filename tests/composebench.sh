#!/usr/bin/env bash
#
# gen160 (E1): compositional emergence benchmark.
#
# Each tests/compose/*.dlg is ONE persistent session whose success requires
# THREE OR MORE independently-evolved subsystems to cooperate in the same
# conversation, over FRESH held-out vocabulary (anti-impostor: names, predicates
# and orderings never appear in tests/cases). The point is not to add behaviour
# but to measure whether the structure already emerged COMPOSES — and to record,
# per dialogue, whether it:
#
#   composes-unchanged : every turn lands on the current brain, no edit needed
#   generic-parser     : would land after a NON-task-specific parser improvement
#   special-case       : would need a bespoke handler (a smell; the worst grade)
#
# Header metadata in each .dlg drives the score:
#   #subsystems: a, b, c, ...   (the cooperating parts; >=3 required)
#   #expect: pass | gap         (pass = ratcheted gate; gap = recorded growth edge)
#   #class: composes-unchanged | generic-parser | special-case
#
# Turn protocol (same as longchatbench):
#   > user turn
#   < satisfying substring, soft-normalized
#
# A `#expect: pass` dialogue that MISSES any turn fails the whole bench (the
# ratchet: composition that was earned never silently regresses). A `#expect:
# gap` dialogue tolerates misses and reports the first one — that is the next
# pull, not a failure.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
DLG_DIR="$ROOT/tests/compose"

# Hermetic, like the case harness: no kb/ files, so composition is proven by the
# brain alone, not by pre-loaded knowledge.
export PARROT0_BASE= PARROT0_SESSION= PARROT0_LANG=en

if [ ! -x "$BIN" ]; then
    echo "composebench: binary not built ($BIN)" >&2
    exit 1
fi

norm() {
    printf "%s" "$1" | tr "[:upper:]" "[:lower:]" \
        | tr -d "[:punct:]" | tr -s " " | sed "s/^ *//;s/ *$//"
}

dialogues=0
composed=0          # dialogues classified composes-unchanged that fully landed
gaps=0
gate_fail=0
total_turns=0
landed_turns=0
subsystem_coverage=0

shopt -s nullglob
for dlg in "$DLG_DIR"/*.dlg; do
    dialogues=$((dialogues + 1))
    name="$(basename "$dlg" .dlg)"

    inputs=()
    expected=()
    subsystems=""
    expect="pass"
    class="composes-unchanged"

    while IFS= read -r raw || [ -n "$raw" ]; do
        case "$raw" in
            "#subsystems:"*) subsystems="${raw#\#subsystems:}" ;;
            "#expect:"*)     expect="$(printf "%s" "${raw#\#expect:}" | tr -d ' ')" ;;
            "#class:"*)      class="$(printf "%s" "${raw#\#class:}" | tr -d ' ')" ;;
            ">"*) inputs+=("${raw#> }") ;;
            "<"*) expected+=("${raw#< }") ;;
            *) : ;;
        esac
    done < "$dlg"

    if [ "${#inputs[@]}" -ne "${#expected[@]}" ]; then
        echo "composebench: $name has ${#inputs[@]} inputs but ${#expected[@]} expectations" >&2
        exit 1
    fi

    # Count declared subsystems (comma-separated).
    nsub=0
    OLDIFS="$IFS"; IFS=','
    for _s in $subsystems; do
        _s="$(printf "%s" "$_s" | sed 's/^ *//;s/ *$//')"
        [ -n "$_s" ] && nsub=$((nsub + 1))
    done
    IFS="$OLDIFS"
    if [ "$nsub" -lt 3 ]; then
        echo "composebench: $name composes only $nsub subsystems (E1 requires >=3)" >&2
        exit 1
    fi

    in_lines=""
    for line in "${inputs[@]}"; do
        printf -v in_lines "%s%s\n" "$in_lines" "$line"
    done
    actual="$(printf "%s" "$in_lines" | "$BIN" 2>/dev/null)"
    mapfile -t outs <<< "$actual"

    echo "# $name [$class, expect=$expect]"
    echo "  composes $nsub subsystems: $(printf "%s" "$subsystems" | sed 's/^ *//')"
    file_landed=0
    first_miss=""
    for i in "${!expected[@]}"; do
        want="${expected[$i]}"
        got="${outs[$i]-}"
        nw="$(norm "$want")"
        ng="$(norm "$got")"
        total_turns=$((total_turns + 1))
        if [ -n "$nw" ] && printf "%s" "$ng" | grep -qF -- "$nw"; then
            landed_turns=$((landed_turns + 1))
            file_landed=$((file_landed + 1))
            status="OK  "
        else
            status="MISS"
            [ -z "$first_miss" ] && first_miss="\"${inputs[$i]}\" -> \"$got\" (wanted ~ \"$want\")"
        fi
        printf "    %s %s\n" "$status" "$got"
    done

    full="$([ "$file_landed" -eq "${#expected[@]}" ] && echo yes || echo no)"
    if [ "$full" = "yes" ]; then
        echo "  result: COMPOSES (all ${#expected[@]} turns landed)"
        subsystem_coverage=$((subsystem_coverage + nsub))
        if [ "$class" = "composes-unchanged" ]; then
            composed=$((composed + 1))
        fi
    else
        echo "  result: GAP — first miss: $first_miss"
        gaps=$((gaps + 1))
    fi

    if [ "$expect" = "pass" ] && [ "$full" != "yes" ]; then
        echo "  !! REGRESSION: a ratcheted composition broke."
        gate_fail=1
    fi
    echo
done

if [ "$dialogues" -eq 0 ]; then
    echo "composebench: no dialogues in $DLG_DIR" >&2
    exit 1
fi

echo "---"
echo "dialogues: $dialogues"
echo "composing unchanged: $composed"
echo "open gaps (recorded growth edges): $gaps"
echo "subsystem-composition coverage (turns landed across composing dialogues): $subsystem_coverage"
if [ "$total_turns" -gt 0 ]; then
    echo "turn landing: $((landed_turns * 100 / total_turns))% ($landed_turns/$total_turns)"
fi

if [ "$gate_fail" -ne 0 ]; then
    echo "FAIL composebench: a composes-unchanged dialogue regressed"
    exit 1
fi
echo "PASS composebench: $composed compositions hold unchanged; $gaps gaps recorded"
