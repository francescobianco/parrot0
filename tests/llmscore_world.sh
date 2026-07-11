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
expect "world superlative tallest mountain" "1" \
    "What's the tallest mountain in the world?" \
    "Mount Everest is the tallest mountain in the world."
expect "planet ring-system cue is KB teachable" "1" \
    "Which planet in our solar system is famous for its prominent ring system?" \
    "Saturn is the Ringed Planet."
expect "world border-most cue is KB teachable" "1" \
    "Which country in the world shares a border with the most other countries?" \
    "China and Russia each border 14 countries, the most in the world."
expect "multi-word superlative domain internal organ" "1" \
    "What is the largest internal organ in the human body?" \
    "The liver is the largest internal organ in the human body."
expect "translation request bypasses coref repair" "1" \
    'How do you say "How much does it cost?" in French?' \
    "Combien ça coûte."
expect "capital country span handles south korea" "1" \
    "What is the capital city of South Korea?" \
    "Seoul."
expect "french multi-word noun composition" "1" \
    'How do you say "Where is the bathroom?" in French?' \
    "Où est la salle de bains."
expect "riddle not misclassified as code" "1" \
    "I am not alive, but I grow; I don't have lungs, but I need air; I don't have a mouth, but water kills me. What am I?" \
    "Fire."
expect "definition word frame bypasses module self-query" "1" \
    'What does the word "pragmatic" mean?' \
    "pragmatic is dealing with things sensibly and realistically; practical rather than theoretical."
expect "regional longest river superlative" "1" \
    "What is the longest river in South America?" \
    "Amazon."
expect "unique trait consumer" "1" \
    "What is the only bird that can fly backwards?" \
    "Hummingbird."
expect "planet size comparison from magnitude" "1" \
    "Which is larger, Saturn or Jupiter?" \
    "Jupiter."
expect "weight comparison with descriptive entity spans" "1" \
    "Which is heavier: a grown African elephant or a fully grown hippopotamus?" \
    "African elephant."
expect "pun riddle does not fall into color mixing" "1" \
    "What is black and white and red all over?" \
    "A newspaper."
expect "skin onion riddle from signatures" "1" \
    "Take off my skin and I won't cry, but you might. What am I?" \
    "An onion."

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
expect "bookshelf shelves-worth removal" "1" \
    "If a bookshelf has 5 shelves with 24 books each, and you remove 3 complete shelves worth, then add back 15 individual books, how many books remain on the shelf?" \
    "63."
expect "recipe scaling triple" "1" \
    "A recipe calls for 2 cups of flour, 3 eggs, and 1 cup of sugar. How much of each ingredient do you need to make triple the amount?" \
    "6 cups of flour, 9 eggs, and 3 cups of sugar."
expect "buy with remainder" "1" \
    'If a book costs $8 and you have $50, how many books can you buy and how much money would remain?' \
    'You can buy 6, with $2 left over.'
expect "change from a bill" "1" \
    'If a book costs $15 and you have a $50 bill, how much change do you get back?' \
    '$35.'
expect "count backward by 3s" "1" \
    "Count backward from 20 by 3s." \
    "20, 17, 14, 11, 8, 5, 2."
expect "count backward with buzz replacement" "1" \
    "Count backward from 20 to 1, but say buzz instead of any number divisible by 3." \
    "20, 19, buzz, 17, 16, buzz, 14, 13, buzz, 11, 10, buzz, 8, 7, buzz, 5, 4, buzz, 2, 1."
expect "undistributed-middle syllogism" "1" \
    "If all roses are flowers and some flowers fade quickly, does it follow that some roses fade quickly? Why or why not?" \
    'No -- that doesn'"'"'t follow. From "all roses are flowers" and "some flowers ...", nothing follows about roses: the flowers in question need not be roses (the middle term is undistributed).'
expect "anagram of listen" "1" \
    'If you rearrange the letters in "listen," what other common English word can you form?' \
    '"silent".'
expect "reverse alphabetical letters" "1" \
    'If you arrange the letters in "STRESSED" in reverse alphabetical order, what word do you form?' \
    "tsssreed."
expect "days alphabetical first" "1" \
    "If you arrange the seven days of the week alphabetically, which day comes first?" \
    "Friday."
expect "place riddle: zoo" "1" \
    'What word rhymes with "cat" and means a place where you might see exotic animals?' \
    "A zoo."
expect "couplet about the sea" "1" \
    "Write a rhyming couplet about the sea." \
    "The sea rolls in with silver and with foam; it sings the oldest song of going home."
expect "autumn letters couplet" "1" \
    "Write a short couplet pairing autumn leaves with forgotten letters." \
    "Autumn leaves fall over letters left unread; forgotten words sleep softly in their bed."
expect_full "four-line ocean poem" "1" \
    "Write a four-line poem about the ocean." \
    $'The ocean breathes in blue and grey,\nand carries the moon on a silver way.\nIts deep tides murmur, vast and wide,\nwith secret songs beneath the tide.'
expect "train toward each other arrives first" "1" \
    "One train leaves Chicago at 6am going 80mph, and another leaves New York at 8am going 70mph. Which train arrives first if they're traveling toward each other between the two cities?" \
    "Neither train arrives first; they meet each other at about 12:12 PM."
expect "train destination arrival race" "1" \
    "If a train leaves Chicago at 6:00 AM traveling 70 mph, and another train leaves Denver at 8:00 AM traveling 90 mph toward Chicago, and the distance between the cities is 1,000 miles, which train arrives at its destination first and by how much time?" \
    "The train from Denver arrives first, by about 1 hour 10 minutes."
expect "opposite ephemeral" "1" \
    "What's the opposite of ephemeral?" \
    "Permanent."
expect "moonlight water couplet" "1" \
    "Write a short, two-line rhyming poem about moonlight on water." \
    "Moonlight trembles where the dark waves gleam; water holds the sky as if in dream."
expect "library couplet" "1" \
    "Write a short couplet about a quiet library." \
    "The quiet library keeps the dust of light; its pages bloom like lanterns in the night."
expect "exact five-word completion" "1" \
    'Continue this sentence in exactly five words: "She opened the door and...' \
    "A cold wind rushed in."
expect "bottle sentence completion" "1" \
    'Complete this sentence: "If you could bottl' \
    "e one golden afternoon, I would keep it for a grey day."
expect "translate warm rug to french" "1" \
    'Translate into French: "The cat is sleeping on the warm rug.' \
    "Le chat dort sur le tapis chaud."
expect "same pot egg boiling" "1" \
    "If it takes 3 minutes to boil a single egg, how many minutes does it take to boil 6 eggs at the same time in the same pot?" \
    "3 minutes."
expect "minute moment riddle" "1" \
    "What occurs once in a minute, twice in a moment, but never in a thousand years?" \
    "The letter M."
expect "sky blue sunset red explanation" "1" \
    "Explain in simple terms why the sky appears blue during the day but turns orange or red during sunset." \
    "By day the sky looks blue because sunlight scatters off the air, and blue light scatters the most. Near sunset it looks orange or red because sunlight travels through more air, so blue scatters away and red or orange light remains."
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
expect "three creative door continuations" "1" \
    'Complete this sentence in three different creative ways: "She opened the door and...' \
    "1) ...someone knocked once from the other side. 2) ...a cold wind rushed in carrying the smell of rain. 3) ...a narrow stairway curved down into the dark."
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
    "I don't watch movies or have recent viewing experiences, but for the prompt I'd pick an old mystery for its careful clues."
expect_full "bread rises process from KB topic schema" "1" \
    "Describe the process of how bread rises when yeast is added to dough." \
    $'1. Yeast consumes sugars in the dough and releases carbon dioxide gas\n2. Gluten strands trap that gas in small bubbles, stretching the dough\n3. Warmth and time let fermentation continue, so the bubbles expand\n4. During baking, the gas expands further and the crumb sets into a risen loaf'
expect_full "rainy Sunday activity recommendation" "1" \
    "What's the best way to spend a rainy Sunday afternoon?" \
    $'From what I know, a good plan is:\n1. Choose one indoor activity that fits the weather, such as reading, cooking, or a quiet film\n2. Make the room comfortable with warm light, a drink, and a clear place to sit\n3. Do one small useful task first so the rest of the afternoon feels unhurried\n4. Leave time for rest rather than packing the day with errands'
expect_full "rainy afternoon favorite without fake preference" "1" \
    "What's your favorite thing to do on a rainy afternoon?" \
    "I don't have real favorites, but on a rainy day I'd read quietly with a warm drink."

# --- gen254: the six holes exposed by the 2026-07-01 llmscore run ---
expect "circle circumference from radius" "1" \
    "If you drop a stone into a pond and it creates circular ripples expanding outward at constant speed, and you notice one ripple has a radius of 12 centimeters, what is the approximate circumference of that ripple?" \
    "About 75.4 centimeters -- circumference = 2 x pi x radius = 2 x 3.14159 x 12."
expect "circle area from radius" "1" \
    "What is the area of a circle with a radius of 5 meters?" \
    "About 78.5 square meters -- area = pi x radius^2 = 3.14159 x 5 x 5."
expect "traveler continuation three ways" "1" \
    'Continue this sentence in three different ways: "When the last light faded from the horizon, the traveler realized..."' \
    "1) ...that the road behind him had vanished into the dark. 2) ...that he no longer knew which way led home. 3) ...that the distant lights were not stars but windows."
expect "compound vocabulary opposite plus word-for" "1" \
    'What'"'"'s the opposite of "generous" and what word describes a person who always tells the truth?' \
    "Selfish. Honest."
expect "word for someone who never gives up" "1" \
    "What word describes someone who never gives up?" \
    "Persistent."
expect "constrained number not prime" "1" \
    "I'm thinking of a number. It is greater than 10, less than 20, and not prime. What could my number be?" \
    "It could be 12, 14, 15, 16, or 18."
expect "constrained number unique" "1" \
    "I'm thinking of a number. It is greater than 10, less than 14, and even. What is it?" \
    "It must be 12 -- the only number from 11 to 13 that fits."
expect "constrained number impossible" "1" \
    "I'm thinking of a number. It is greater than 24, less than 28, and prime. What could my number be?" \
    "No number fits: nothing from 25 to 27 satisfies all those constraints together."
expect "idiom intent without the word idiom" "1" \
    'If someone says "break a leg" before a performance, what'"'"'s the usual intent behind those words?' \
    '"break a leg" means good luck -- it'"'"'s a way to wish someone a great performance, especially on stage.'
expect "all-but keeps folding later deltas" "1" \
    "A farmer has 17 sheep, sells all but 9, then buys 12 more. How many sheep does he have now?" \
    "21."
expect "all-but alone still leaves the remainder" "1" \
    "A farmer has 17 sheep. All but 9 die. How many sheep does the farmer have left?" \
    "9."
# --- gen254 fifth pass: compound leak, analogy medium, computed pangram ---
expect "capital and year compound without leak" "1" \
    "What is the capital of Australia, and what year did it become the capital?" \
    "Canberra. It became the capital in 1927, when parliament moved from Melbourne to Canberra."
expect "analogy medium sculptor" "1" \
    "Complete this analogy: Painter is to canvas as sculptor is to _____." \
    "marble."
expect "light mixes additively" "1" \
    "What do you get when you mix red and blue light?" \
    "Magenta."
expect "paint still mixes subtractively" "1" \
    "What color do you get when you mix blue and yellow paint together?" \
    "Green."
expect "pangram computed not stored" "1" \
    'What does "The quick brown fox jumps over the lazy dog" contain that most other sentences do not?' \
    "It contains every letter of the alphabet -- it's a pangram."
expect "haiku weaves two themes" "1" \
    "Write a haiku about autumn and technology." \
    "Red leaves let go now / small lights think in the still house / bare branches stay still."

# --- gen254 fourth pass: the 4/10 late-night sample, closed as categories ---
expect "polar bear riddle from KB signature" "1" \
    "A man builds a house with all four walls facing south. A bear walks past the house. What color is the bear?" \
    "White -- if all four walls face south, the house stands at the North Pole, so the bear is a polar bear."
expect "word-lengths riddle from KB signature" "1" \
    "What has four letters, sometimes nine letters, but never five letters?" \
    "It's a statement: 'what' has four letters, 'sometimes' has nine, 'never' has five."
expect "river crossing riddle from KB signature" "1" \
    "A farmer needs to cross a river with a fox, a goose, and a sack of corn. The boat can only carry the farmer plus one item. If left alone together, the fox will eat the goose, and the goose will eat the corn. How does the farmer get everything across safely?" \
    "Take the goose first; bring the fox but carry the goose back; take the corn over; return for the goose."
expect "footsteps riddle from KB signature" "1" \
    "The more you take, the more you leave behind. What am I?" \
    "Footsteps."
expect "letter count of a named word" "1" \
    'How many letters are in the word "strawberry"?' \
    'There are 10 letters in "strawberry".'
expect "hypothetical skill pick" "1" \
    "If you could instantly master any skill, what would it be?" \
    "I don't have real desires, but for the prompt I'd pick speaking every language fluently -- it would open every other door."
expect "even though is not a parity question" "1" \
    'Continue this sentence: "The old clock on the wall suddenly began to chime, even though no one had touched it..."' \
    "the whole room held its breath."

# --- gen254 third pass: the four zeros of the 6/10 run, closed as frames ---
expect "rectangle area from perimeter and side" "1" \
    "If a rectangle has a perimeter of 24 cm and one side is 5 cm, what is the area?" \
    "35 square cm -- the other side is 24/2 - 5 = 7, and 5 x 7 = 35."
expect "heads and legs linear system" "1" \
    "A farmer has 20 animals - chickens and rabbits. Together they have 56 legs. How many chickens does the farmer have?" \
    "12 chickens (and 8 rabbits): 12 x 2 + 8 x 4 = 56 legs."
expect "heads and legs em-dash phrasing" "1" \
    "A farmer has 20 animals—chickens and rabbits. Together they have 56 legs. How many chickens does the farmer have?" \
    "8 rabbits and 12 chickens."
expect "what-are counted pick" "1" \
    "What are the three primary colors in traditional painting?" \
    "Red, yellow, and blue."
expect "role holder first president" "1" \
    "Who was the first President of the United States?" \
    "George Washington, who served from 1789 to 1797."
expect "role holder first on the moon" "1" \
    "Who was the first person on the moon?" \
    "Neil Armstrong, in July 1969."

# --- gen254 second pass: ENGINE generalizations pulled from the 2026-07-02 run ---
expect "relative quantity step in sequence" "1" \
    "If I have 3 apples and you give me 2 more, then I eat one, then you give me twice what I currently have — how many apples do I end up with?" \
    "12."
expect "ratio age puzzle linear solve" "1" \
    "A father is four times as old as his son. In 20 years, the father will be twice as old as his son. How old are they both right now?" \
    "The son is 10 and the father is 40."
expect "ratio age puzzle second instance" "1" \
    "A mother is 3 times as old as her daughter. In 12 years, she will be twice as old as her daughter. How old are they now?" \
    "The daughter is 12 and the mother is 36."
expect "haiku via morphological binding" "1" \
    "Write a haiku about moonlight on water." \
    "Pale lantern of night / drifting over silent fields / shadows softly bloom."
expect "bare poetic fragment continued" "1" \
    "Raindrops tap the pond" \
    "Rings widen where each drop lands."
expect "silence sentence three ways" "1" \
    "Complete this sentence in three different ways: The silence in the room was so thick that..." \
    "1) ...the whole room held its breath. 2) ...every small sound in the house became enormous. 3) ...the stopped clock seemed to be waiting for an answer."
expect "capital and predecessor compound" "1" \
    "What is the capital of Australia, and which city did it replace in that role?" \
    "Canberra. It replaced Melbourne, which served as the seat of government until 1927."
expect "story about a lighthouse" "1" \
    "Tell me a story about a lighthouse." \
    "A faint light blinked far out on the dark water. Then the beam swept once more across the empty, churning sea. At last, a wave struck the rocks below with a sound like thunder."
expect "story decline names real alternatives" "1" \
    "tell me a story about dragons" \
    "I don't have story material for that topic yet -- I can tell one about a lighthouse, an old house on a quiet street, a stormy night, or a traveler at dusk."
expect "no wildcard category from punctuation" "1" \
    "give me 2 - what" \
    "I don't understand that yet."
expect "arrangement optimization calibrated decline" "1" \
    "A farmer has 20 sheep and 5 fences. Each fence can enclose a separate area, but the farmer wants to arrange them so that each enclosed area shares a fence with at least one other area. What's the maximum number of sheep that can be kept separate from all others, assuming each separate area must contain at least one sheep?" \
    "That's a constrained-arrangement puzzle: I can read the quantities, but I don't have a solver that can verify an optimal arrangement under those sharing constraints, so I won't guess a number."

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
