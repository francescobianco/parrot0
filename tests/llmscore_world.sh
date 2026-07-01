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

expect_full() { # desc env_flag input expected_full_response
    local desc="$1" world="$2" input="$3" want="$4" got
    got="$(printf '%s\n/quit\n' "$input" | PARROT0_BASE= PARROT0_SESSION= PARROT0_WORLD_FACTS="$world" "$BIN" 2>/dev/null)"
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
expect "moonlight water couplet" "1" \
    "Write a short, two-line rhyming poem about moonlight on water." \
    "Moonlight trembles where the dark waves gleam; water holds the sky as if in dream."
expect "library couplet" "1" \
    "Write a short couplet about a quiet library." \
    "The quiet library keeps the dust of light; its pages bloom like lanterns in the night."
expect "exact five-word completion" "1" \
    'Continue this sentence in exactly five words: "She opened the door and...' \
    "A cold wind rushed in."
expect "weather where you are (honest)" "1" \
    "What's the weather like where you are right now?" \
    "I don't have a body or a place to be, so I can't feel the weather -- but I'm here and ready whenever you are."
expect "library haiku from KB images" "1" \
    "Write a haiku about a quiet library." \
    "Pages breathe softly / Quiet shelves hold hidden worlds / Dust glows in moonlight."
expect "short joke template" "1" \
    "Tell me a short joke." \
    "Why did the function return early? It had a break."
expect "planet with most moons" "1" \
    "Which planet has the most moons?" \
    "Saturn is the planet with the most known moons."
expect "capital and ocean borders" "1" \
    "What is the capital city of Australia, and which two oceans border it?" \
    "Canberra; Australia borders the Indian Ocean and the Pacific Ocean."
expect "name countries bordering france uses relation" "1" \
    "Name three countries that border France." \
    "France borders Italy, Spain, and Germany."
expect "reverse animal sound" "1" \
    "What animal is known for saying meow?" \
    "A cat."
expect "difference frame ram rom" "1" \
    "What is the difference between RAM and ROM?" \
    "RAM is fast volatile working memory; ROM is non-volatile memory for firmware or fixed data"
expect "difference frame animal pair" "1" \
    "What is the difference between a crocodile and an alligator?" \
    "Crocodiles usually have V-shaped snouts; alligators have broader U-shaped snouts"
expect "difference frame contraction slot" "1" \
    "What is the difference between its and it's?" \
    "Its means belonging to it; it's is a contraction of it is or it has"
expect "difference frame informed gap" "1" \
    "What is the difference between chalk and cheese?" \
    "You're asking for a distinction between Chalk and Cheese, but I don't have that contrast fact yet."
expect "magnitude which faster" "1" \
    "Which is faster, a car or a bicycle?" \
    "Car."
expect "magnitude yes no country size" "1" \
    "Is Canada bigger than the USA?" \
    "Yes."
expect "magnitude yes no material weight" "1" \
    "Is silver heavier than gold?" \
    "No."
expect "magnitude sun moon" "1" \
    "What is bigger, the sun or the moon?" \
    "Sun."
expect "voyage sentence continuation" "1" \
    'Continue this sentence: "Before the sun rose, the old sailor stepped onto the deck and...' \
    "felt the first pale light touch the rail."
expect "reveal sentence continuation" "1" \
    "Finish this sentence: The old door creaked open to reveal..." \
    "a dusty hallway bathed in pale candlelight."
expect "bare narrative continuation" "1" \
    "...a dusty hallway bathed in pale candlelight, the air thick with the scent of forgotten years." \
    "At the far end, a small brass key glinted on the floor."
expect "three-word cold morning description" "1" \
    "Describe the feeling of stepping outside on a cold winter morning in three words." \
    "Crisp cold air."
expect "exact three-word sky explanation" "1" \
    "Explain in exactly three words why the sky appears blue during the day." \
    "Air scatters blue."
expect "clockmaker mystery-box continuation" "1" \
    'Continue this story: "The old clockmaker opened the mysterious box and found...' \
    "Suddenly, a tiny clockwork bird with emerald eyes."
expect "bear no teeth joke riddle" "1" \
    "What do you call a bear with no teeth?" \
    "A gummy bear."
expect "ripe banana taste" "1" \
    "Describe the taste of aripe banana to someone who has never tasted one." \
    "I don't actually taste things, but it is often described as sweet, soft, and creamy, with a mellow fruit flavour."
expect "rubber band stretch release" "1" \
    "Describe, in one or two sentences, what happens to a rubber band when you stretch it and then let go." \
    "It stretches longer while you pull it. When you let go, elasticity pulls it back toward its original shape."
expect "map riddle" "1" \
    "Riddle me this: I have cities but no houses, forests but no trees, and water but no fish. What am I?" \
    "A map."
expect "recent movie without fake viewing" "1" \
    "What's the best movie you've watched recently, and what did you like most about it?" \
    "I don't watch movies or have recent viewing experiences, but for the prompt I'd pick an old mystery for its careful clues. Who or what does \"it\" refer to?"
expect_full "bread rises process from KB topic schema" "1" \
    "Describe the process of how bread rises when yeast is added to dough." \
    $'1. Yeast consumes sugars in the dough and releases carbon dioxide gas\n2. Gluten strands trap that gas in small bubbles, stretching the dough\n3. Warmth and time let fermentation continue, so the bubbles expand\n4. During baking, the gas expands further and the crumb sets into a risen loaf'
expect_full "rainy Sunday activity recommendation" "1" \
    "What's the best way to spend a rainy Sunday afternoon?" \
    $'From what I know, a good plan is:\n1. Choose one indoor activity that fits the weather, such as reading, cooking, or a quiet film\n2. Make the room comfortable with warm light, a drink, and a clear place to sit\n3. Do one small useful task first so the rest of the afternoon feels unhurried\n4. Leave time for rest rather than packing the day with errands'
expect_full "rainy afternoon favorite without fake preference" "1" \
    "What's your favorite thing to do on a rainy afternoon?" \
    "I don't have real favorites, but a good plan is:
1. Choose one indoor activity that fits the weather, such as reading, cooking, or a quiet film
2. Make the room comfortable with warm light, a drink, and a clear place to sit
3. Do one small useful task first so the rest of the afternoon feels unhurried
4. Leave time for rest rather than packing the day with errands"

follow="$(printf '%s\n%s\n/quit\n' \
    "Write a short, two-line rhyming poem about glass." \
    "Moon." \
    | PARROT0_BASE= PARROT0_SESSION= PARROT0_WORLD_FACTS=1 "$BIN" 2>/dev/null | tail -1)"
if [ "$follow" = "The pale moon climbs and spills its quiet light; it lays a silver road across the night." ]; then
    echo "PASS llmscore-world: creative follow-up picks offered theme"
    pass=$((pass+1))
else
    echo "FAIL llmscore-world: creative follow-up picks offered theme - want [The pale moon climbs and spills its quiet light; it lays a silver road across the night.] got [$follow]" >&2
    fail=$((fail+1))
fi

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
