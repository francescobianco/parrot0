#!/usr/bin/env bash
# gen235: common world facts must improve llmscore without destroying dynamic
# learning tests. Prove the layer can be present, suppressed, and relearned.
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "llmscore-world: binary not built" >&2; exit 1; }

pass=0 fail=0

expect() { # desc env_flag input expected_first_line
    local desc="$1" world="$2" input="$3" want="$4" got
    got="$(printf '%s\n/quit\n' "$input" | PARROT0_BASE= PARROT0_SESSION= PARROT0_WORLD_FACTS="$world" "$BIN" 2>/dev/null | head -1)"
    if [ "$got" = "$want" ]; then
        echo "PASS llmscore-world: $desc"; pass=$((pass+1))
    else
        echo "FAIL llmscore-world: $desc - want [$want] got [$got]" >&2; fail=$((fail+1))
    fi
}

expect "base world knows france capital" "1" \
    "what is the capital of france?" \
    "Paris."

expect "world layer can be suppressed" "0" \
    "what is the capital of france?" \
    "I do not know the relation capital yet, so I cannot answer the capital of france. You can teach me with thing is the capital of france, or give facts/rules to reason from."

learned="$(printf '%s\n%s\n/quit\n' \
    "paris is the capital of france" \
    "what is the capital of france?" \
    | PARROT0_BASE= PARROT0_SESSION= PARROT0_WORLD_FACTS=0 "$BIN" 2>/dev/null | tail -1)"
if [ "$learned" = "Paris." ]; then
    echo "PASS llmscore-world: suppressed world can relearn capital"
    pass=$((pass+1))
else
    echo "FAIL llmscore-world: suppressed world can relearn capital - want [Paris.] got [$learned]" >&2
    fail=$((fail+1))
fi

# --- gen241 (docs/plans/llmscore-check.md): sweep of bench prompts scored 0, each
# fixed KB-first against world-facts.p0. All single-line, deterministic on a fresh
# session (no response rotation in these paths). ---
expect "name three primary colors" "1" \
    "Name three primary colors." \
    "Red, yellow, and blue."
expect "idiom break a leg" "1" \
    'What does the idiom "break a leg" mean?' \
    '"break a leg" means good luck -- it'"'"'s a way to wish someone a great performance, especially on stage.'
expect "boiling point at sea level" "1" \
    "If you boil water at sea level, what happens, and at what temperature?" \
    "It boils at 100 degrees Celsius (212 degrees Fahrenheit) at sea level -- it turns into steam (water vapour)."
expect "wwii end year and place" "1" \
    "What year did World War II end, and in which city did the surrender ceremony take place?" \
    "World War II ended in 1945; Japan's surrender was signed on Sept 2, 1945, aboard the USS Missouri in Tokyo Bay."
expect "most populous country" "1" \
    "Which country has the most people: India, China, or the United States?" \
    "India."
expect "largest planet and a moon" "1" \
    "Name the largest planet in our solar system and describe one of its moons." \
    "Jupiter is the largest planet in our solar system. One of its moons is Io, the most volcanically active body in the solar system, dotted with hundreds of erupting volcanoes."
expect "capital and its river" "1" \
    "What is the capital city of Australia, and which river runs through it?" \
    "Canberra. The Molonglo River runs through it."
expect "capital and ocean to its west" "1" \
    "What is the capital of Australia, and what ocean lies to its west?" \
    "Canberra. To its west lies the Indian Ocean."
expect "bookshelf multi-step" "1" \
    "A bookshelf has 5 shelves. Each shelf holds 12 books. If you remove 20 books and add 8 new ones, how many books are on the bookshelf?" \
    "48."
expect "buy with remainder" "1" \
    'If a book costs $8 and you have $50, how many books can you buy and how much money would remain?' \
    'You can buy 6, with $2 left over.'
expect "change from a bill" "1" \
    'If a book costs $15 and you have a $50 bill, how much change do you get back?' \
    '$35.'
expect "count backward by 3s" "1" \
    "Count backward from 20 by 3s." \
    "20, 17, 14, 11, 8, 5, 2."
expect "undistributed-middle syllogism" "1" \
    "If all roses are flowers and some flowers fade quickly, does it follow that some roses fade quickly? Why or why not?" \
    'No -- that doesn'"'"'t follow. From "all roses are flowers" and "some flowers ...", nothing follows about roses: the flowers in question need not be roses (the middle term is undistributed).'
expect "anagram of listen" "1" \
    'If you rearrange the letters in "listen," what other common English word can you form?' \
    '"silent".'
expect "days alphabetical first" "1" \
    "If you arrange the seven days of the week alphabetically, which day comes first?" \
    "Friday."
expect "place riddle: zoo" "1" \
    'What word rhymes with "cat" and means a place where you might see exotic animals?' \
    "A zoo."
expect "couplet about the sea" "1" \
    "Write a rhyming couplet about the sea." \
    "The sea rolls in with silver and with foam; it sings the oldest song of going home."
expect "exact five-word completion" "1" \
    'Continue this sentence in exactly five words: "She opened the door and...' \
    "A cold wind rushed in."
expect "weather where you are (honest)" "1" \
    "What's the weather like where you are right now?" \
    "I don't have a body or a place to be, so I can't feel the weather -- but I'm here and ready whenever you are."

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
