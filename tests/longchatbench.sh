#!/usr/bin/env bash
#
# gen147 (E6): deterministic long casual conversation benchmark.
#
# Each tests/longchat/*.dlg file is one persistent session:
#   > user turn
#   < satisfying substring to look for, soft-normalized
#   @ optional metric tags: repair continuity contradiction user_model boredom topic
#
# This is a benchmark, not a gate. It prints naturalness and capability metrics
# so long-form conversation can be tracked without pretending the current brain
# already passes every turn.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
DLG_DIR="$ROOT/tests/longchat"

export PARROT0_BASE= PARROT0_SESSION=

if [ ! -x "$BIN" ]; then
    echo "longchatbench: binary not built ($BIN)" >&2
    exit 1
fi

norm() {
    printf "%s" "$1" | tr "[:upper:]" "[:lower:]" \
        | tr -d "[:punct:]" | tr -s " " | sed "s/^ *//;s/ *$//"
}

is_wall() {
    n="$(norm "$1")"
    case "$n" in
        *"i dont understand that yet"*|*"im not sure i followed"*|*"i didnt quite catch that"*|*"thats a bit beyond me right now"*|*"hmm i dont know about "*)
            return 0 ;;
        *)
            return 1 ;;
    esac
}

pct() {
    num="$1"; den="$2"
    if [ "$den" -gt 0 ]; then
        printf "%d%% (%d/%d)" "$((num * 100 / den))" "$num" "$den"
    else
        printf "n/a"
    fi
}

metric_label() {
    case "$1" in
        repair) printf "%s" "repair success" ;;
        continuity) printf "%s" "continuity references" ;;
        contradiction) printf "%s" "contradiction handling" ;;
        user_model) printf "%s" "user-model precision" ;;
        boredom) printf "%s" "boredom handling" ;;
        topic) printf "%s" "topic changes" ;;
        *) printf "%s" "$1" ;;
    esac
}

declare -A metric_total=()
declare -A metric_land=()
metric_keys=(repair continuity contradiction user_model boredom topic)

total=0
landed=0
wall=0
repeat=0
files=0
stress_turns=0

shopt -s nullglob
for dlg in "$DLG_DIR"/*.dlg; do
    files=$((files + 1))
    name="$(basename "$dlg")"
    inputs=()
    expected=()
    tags=()

    while IFS= read -r raw || [ -n "$raw" ]; do
        case "$raw" in
            ">"*) inputs+=("${raw#> }") ;;
            "<"*) expected+=("${raw#< }"); tags+=("") ;;
            "@"*)
                if [ "${#tags[@]}" -eq 0 ]; then
                    echo "longchatbench: $name has tag before expectation" >&2
                    exit 1
                fi
                tags[$((${#tags[@]} - 1))]="${raw#@ }"
                ;;
            *) : ;;
        esac
    done < "$dlg"

    if [ "${#inputs[@]}" -ne "${#expected[@]}" ]; then
        echo "longchatbench: $name has ${#inputs[@]} inputs but ${#expected[@]} expectations" >&2
        exit 1
    fi
    if [ "${#inputs[@]}" -ge 50 ]; then
        stress_turns=$((stress_turns + ${#inputs[@]}))
    fi

    in_lines=""
    for line in "${inputs[@]}"; do
        printf -v in_lines "%s%s\n" "$in_lines" "$line"
    done
    actual="$(printf "%s" "$in_lines" | "$BIN" 2>/dev/null)"
    mapfile -t outs <<< "$actual"

    echo "# $name (${#expected[@]} turns)"
    prev_norm=""
    file_landed=0
    file_wall=0
    file_repeat=0
    for i in "${!expected[@]}"; do
        want="${expected[$i]}"
        got="${outs[$i]-}"
        nw="$(norm "$want")"
        ng="$(norm "$got")"
        total=$((total + 1))

        ok=0
        if [ -n "$nw" ] && printf "%s" "$ng" | grep -qF -- "$nw"; then
            ok=1
            landed=$((landed + 1))
            file_landed=$((file_landed + 1))
            status="OK"
        else
            status="MISS"
        fi

        if is_wall "$got"; then
            wall=$((wall + 1))
            file_wall=$((file_wall + 1))
        fi
        if [ -n "$prev_norm" ] && [ -n "$ng" ] && [ "$ng" = "$prev_norm" ]; then
            repeat=$((repeat + 1))
            file_repeat=$((file_repeat + 1))
        fi
        prev_norm="$ng"

        for tag in ${tags[$i]}; do
            metric_total[$tag]=$(( ${metric_total[$tag]:-0} + 1 ))
            if [ "$ok" -eq 1 ]; then
                metric_land[$tag]=$(( ${metric_land[$tag]:-0} + 1 ))
            fi
        done

        printf "  %-4s [%s] %s <- \"%s\"\n" "$status" "${tags[$i]}" "$want" "$got"
    done
    echo "  landed: $(pct "$file_landed" "${#expected[@]}")"
    echo "  wall rate: $(pct "$file_wall" "${#expected[@]}")"
    echo "  immediate repetition rate: $(pct "$file_repeat" "${#expected[@]}")"
    echo
done

if [ "$files" -eq 0 ]; then
    echo "longchatbench: no dialogues in $DLG_DIR" >&2
    exit 1
fi

echo "---"
echo "dialogues: $files"
echo "turns: $total"
echo "50-turn stress coverage: $stress_turns turns"
echo "felt landing rate: $(pct "$landed" "$total")"
echo "wall rate: $(pct "$wall" "$total")"
echo "immediate repetition rate: $(pct "$repeat" "$total")"
for key in "${metric_keys[@]}"; do
    den="${metric_total[$key]:-0}"
    num="${metric_land[$key]:-0}"
    echo "$(metric_label "$key"): $(pct "$num" "$den")"
done
