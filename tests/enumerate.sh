#!/usr/bin/env bash
#
# gen294 (basic-chat cat.52 "Elencazione") — enumerate the members of a category.
# mod_namestart already answered the COUNTED pick ("name three animals"); this
# adds the WHOLE-SET form ("list the days of the week", "what are the continents")
# and curates the closed sets (days, months, seasons, senses, continents,
# directions, planets, vegetables, rainbow, musical notes, noble gases) as
# category_member/2 facts. KB-first: the enumerator is one engine mechanism; a new
# set is just data. The members live in kb/core/world-facts.p0, so this gate runs
# the DEFAULT full KB (not the hermetic run.sh surface).
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
if [ ! -x "$BIN" ]; then
    echo "enumerate: binary not built ($BIN)" >&2
    exit 1
fi

pass=0
fail=0
ok() { echo "PASS enumerate: $1"; pass=$((pass+1)); }
no() { echo "FAIL enumerate: $1" >&2; fail=$((fail+1)); }

# ask a prompt against the default (full-KB) brain, English default for determinism
ask() { printf '%s\n/quit\n' "$1" | PARROT0_LANG=en PARROT0_SESSION= "$BIN" 2>/dev/null | head -1; }

# $1 = prompt, $2 = substring the reply must contain, $3 = must NOT wall
expect_has() {
    local got; got="$(ask "$1")"
    if printf '%s' "$got" | grep -qi -- "$2"; then ok "$1"
    else no "$1 -> [$got] (missing '$2')"; fi
}

# ---- counted pick still works (regression) ----
expect_has "name three planets"        "mercury"
expect_has "name the five senses"      "hearing"     # singularize senses->sens fix

# ---- whole-set form (the new capability) ----
expect_has "list the days of the week"        "wednesday"
expect_has "name the months of the year"      "december"
expect_has "list the continents"              "antarctica"
expect_has "what are the cardinal directions" "north"
expect_has "list the primary colors"          "yellow"
expect_has "what are the colors of the rainbow" "indigo"
expect_has "name the notes in a musical scale" "sol"
expect_has "list the noble gases"             "argon"
expect_has "list the seasons"                 "autumn"
expect_has "what are the seven wonders"       "colossus"

# ---- a qualified category beats the generic one (honesty: no misclaim) ----
# "Chinese zodiac animals" must give the zodiac (via the zodiac_animal compound),
# NOT the generic category_member(animal, …) list.
got="$(ask "list the Chinese zodiac animals")"
if printf '%s' "$got" | grep -qi "dragon" && ! printf '%s' "$got" | grep -qi "elephant"; then
    ok "qualified 'zodiac animals' beats generic animal (dragon, no elephant)"
else
    no "zodiac animals -> [$got] (should be the zodiac, not generic animals)"
fi

# ---- multi-word member is dequoted for display ----
got="$(ask "list the continents")"
if printf '%s' "$got" | grep -q 'north america' && ! printf '%s' "$got" | grep -q '"north'; then
    ok "multi-word member dequoted (north america, no quotes)"
else
    no "continents dequote -> [$got]"
fi

# ---- a non-category 'what are ...' turn is NOT stolen by the enumerator ----
got="$(ask "what are you")"
if printf '%s' "$got" | grep -qi "parrot0"; then ok "non-category 'what are you' not stolen"
else no "'what are you' -> [$got] (should be identity, not a list)"; fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
