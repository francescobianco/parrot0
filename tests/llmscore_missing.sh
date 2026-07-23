#!/usr/bin/env bash
# Focused regressions for the missing rows recorded in LLMSCORE.md.
set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/parrot0"
[ -x "$BIN" ] || { echo "llmscore_missing: binary not built ($BIN)" >&2; exit 1; }

pass=0 fail=0
ok() { echo "PASS llmscore_missing: $1"; pass=$((pass+1)); }
no() { echo "FAIL llmscore_missing: $1" >&2; fail=$((fail+1)); }
call() { printf '%s\n' "$1"; }

out="$( {
  call '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}}'
  call '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Explain why the sky appears blue during the day but dark at night."}}}'
  call '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"You are organizing a dinner party for 7 guests. Each guest shakes hands with every other guest exactly once. How many total handshakes occur?"}}}'
  call '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"In a group of 8 people, everyone meets every other person once. How many pairwise meetings happen?"}}}'
  call '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Riddle: A man lives on the 10th floor. He takes the elevator down, but returns to the 7th floor and walks up. Why doesn"}}}'
  call '{"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What do you think is the most important quality for a leader to have, and why?"}}}'
  call '{"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What is the best quality for a teacher to have?"}}}'
  call '{"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"If you have a bucket containing 5 red balls, 3 blue balls, and 2 green balls, and you reach in blindfolded and grab 4 balls at random without looking, what is the probability that you will have at least 2 red balls?"}}}'
  call '{"jsonrpc":"2.0","id":9,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"A farmer needs to cross a river with a fox, a chicken, and a bag of grain. The boat can only carry the farmer plus one item at a time. If left alone together, the fox will eat the chicken, and the chicken will eat the grain. How does the farmer get all three across safely?"}}}'
  call '{"jsonrpc":"2.0","id":10,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"fav_at_least","args":["hg(5, 5, 4, 2)",null]}}}'
  call '{"jsonrpc":"2.0","id":11,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"choose","args":["10","4",null]}}}'
  call '{"jsonrpc":"2.0","id":12,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"probability_procedure","args":["probability_at_least_count","favorable","fav_at_least"]}}}'
  call '{"jsonrpc":"2.0","id":13,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"In a bag with 4 red balls and 2 blue balls, chance pick 2 balls. What is the probability of at least 1 red ball?"}}}'
  call '{"jsonrpc":"2.0","id":14,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"probability_procedure","args":["probability_at_least_count","favorable","fav_at_least"]}}}'
  call '{"jsonrpc":"2.0","id":15,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"In a bag with 4 red balls and 2 blue balls, chance pick 2 balls. What is the probability of at least 1 red ball?"}}}'
  call '{"jsonrpc":"2.0","id":16,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"In a bag with 4 red balls and 2 blue balls, mystery take 2 balls. What is the result for at least 1 red ball?"}}}'
  call '{"jsonrpc":"2.0","id":17,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["probability_draw","mystery take"]}}}'
  call '{"jsonrpc":"2.0","id":18,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"In a bag with 4 red balls and 2 blue balls, mystery take 2 balls. What is the result for at least 1 red ball?"}}}'
  call '{"jsonrpc":"2.0","id":19,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["probability_draw","mystery take"]}}}'
  call '{"jsonrpc":"2.0","id":20,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"In a bag with 4 red balls and 2 blue balls, mystery take 2 balls. What is the result for at least 1 red ball?"}}}'
  call '{"jsonrpc":"2.0","id":21,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What word in the English language has three consecutive double letters?"}}}'
  call '{"jsonrpc":"2.0","id":22,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What word has three paired-letter streaks?"}}}'
  call '{"jsonrpc":"2.0","id":23,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["word_double_runs","paired-letter streaks"]}}}'
  call '{"jsonrpc":"2.0","id":24,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What word has three paired-letter streaks?"}}}'
  call '{"jsonrpc":"2.0","id":25,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["word_double_runs","paired-letter streaks"]}}}'
  call '{"jsonrpc":"2.0","id":26,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What word has three paired-letter streaks?"}}}'
  call '{"jsonrpc":"2.0","id":27,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Describe the taste of a color to someone who has never seen."}}}'
  call '{"jsonrpc":"2.0","id":28,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"synesthetic_default","args":["synesthetic_description","blue"]}}}'
  call '{"jsonrpc":"2.0","id":29,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Describe the taste of a color to someone who has never seen."}}}'
  call '{"jsonrpc":"2.0","id":30,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"synesthetic_default","args":["synesthetic_description","blue"]}}}'
  call '{"jsonrpc":"2.0","id":31,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Flavor-imagine blue for someone who cannot see it."}}}'
  call '{"jsonrpc":"2.0","id":32,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["synesthetic_description","flavor-imagine"]}}}'
  call '{"jsonrpc":"2.0","id":33,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Flavor-imagine blue for someone who cannot see it."}}}'
  call '{"jsonrpc":"2.0","id":34,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["synesthetic_description","flavor-imagine"]}}}'
  call '{"jsonrpc":"2.0","id":35,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Flavor-imagine blue for someone who cannot see it."}}}'
  call '{"jsonrpc":"2.0","id":36,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Write a short opening line for a mystery story set in a place that has never seen the sun."}}}'
  call '{"jsonrpc":"2.0","id":37,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What is the capital of the country that borders both Poland and Ukraine?"}}}'
  call '{"jsonrpc":"2.0","id":38,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"If you rearrange the letters in \"FLUSTER,\" you can also form which other common English word?"}}}'
  call '{"jsonrpc":"2.0","id":39,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Complete this analogy: Book is to Reading as Fork is to ______."}}}'
  call '{"jsonrpc":"2.0","id":40,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"All cats are mammals. Some mammals are pets. Therefore, what can we definitely conclude?"}}}'
  call '{"jsonrpc":"2.0","id":41,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"A train leaves New York at 60 mph. Another train leaves Los Angeles at 80 mph. They are 2,800 miles apart. How long before they meet?"}}}'
  call '{"jsonrpc":"2.0","id":42,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What is the past participle of the verb \"to forego\"?"}}}'
  call '{"jsonrpc":"2.0","id":43,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"meet_time","args":["2800","60","80",null]}}}'
  call '{"jsonrpc":"2.0","id":44,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Incipit hook for a mystery set in a place that has never seen the sun."}}}'
  call '{"jsonrpc":"2.0","id":45,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["story_opening_request","incipit hook"]}}}'
  call '{"jsonrpc":"2.0","id":46,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Incipit hook for a mystery set in a place that has never seen the sun."}}}'
  call '{"jsonrpc":"2.0","id":47,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["story_opening_request","incipit hook"]}}}'
  call '{"jsonrpc":"2.0","id":48,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Incipit hook for a mystery set in a place that has never seen the sun."}}}'
  call '{"jsonrpc":"2.0","id":49,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What is the third form of forego?"}}}'
  call '{"jsonrpc":"2.0","id":50,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["participle_query","third form"]}}}'
  call '{"jsonrpc":"2.0","id":51,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What is the third form of forego?"}}}'
  call '{"jsonrpc":"2.0","id":52,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["participle_query","third form"]}}}'
  call '{"jsonrpc":"2.0","id":53,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What is the third form of forego?"}}}'
  call '{"jsonrpc":"2.0","id":54,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Two trains travel at 60 mph and 80 mph, 2800 miles apart. How many hours rendezvous after?"}}}'
  call '{"jsonrpc":"2.0","id":55,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["train_meet_time","rendezvous after"]}}}'
  call '{"jsonrpc":"2.0","id":56,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Two trains travel at 60 mph and 80 mph, 2800 miles apart. How many hours rendezvous after?"}}}'
  call '{"jsonrpc":"2.0","id":57,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["train_meet_time","rendezvous after"]}}}'
  call '{"jsonrpc":"2.0","id":58,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Two trains travel at 60 mph and 80 mph, 2800 miles apart. How many hours rendezvous after?"}}}'
  call '{"jsonrpc":"2.0","id":59,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Write a two-line rhyming poem about a robot discovering rain."}}}'
  call '{"jsonrpc":"2.0","id":60,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"A rectangle has a perimeter of 24 cm. Its length is twice its width. What are the dimensions of the rectangle?"}}}'
  call '{"jsonrpc":"2.0","id":61,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What is the capital of Australia, and why was Canberra created as a compromise city?"}}}'
  call '{"jsonrpc":"2.0","id":62,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"A store sells 3 apples for $2. How much would you pay for 15 apples, and how much change would you get from a $20 bill?"}}}'
  call '{"jsonrpc":"2.0","id":63,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What year did World War II end, and which two cities received the first atomic bombs?"}}}'
  call '{"jsonrpc":"2.0","id":64,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How many times does the digit 7 appear when counting from 1 to 100?"}}}'
  call '{"jsonrpc":"2.0","id":65,"method":"tools/call","params":{"name":"kb.match","arguments":{"pred":"paired_dimensions_from_doubled_sum_ratio","args":["24","2","4",null]}}}'
  call '{"jsonrpc":"2.0","id":66,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Tally digit 7 from 1 to 100."}}}'
  call '{"jsonrpc":"2.0","id":67,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["digit_count_request","tally digit"]}}}'
  call '{"jsonrpc":"2.0","id":68,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Tally digit 7 from 1 to 100."}}}'
  call '{"jsonrpc":"2.0","id":69,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["digit_count_request","tally digit"]}}}'
  call '{"jsonrpc":"2.0","id":70,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Tally digit 7 from 1 to 100."}}}'
  call '{"jsonrpc":"2.0","id":71,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Boxframe perimeter 24 cm, length twice width; solve the pair."}}}'
  call '{"jsonrpc":"2.0","id":72,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["rectangle_dimension_schema","boxframe"]}}}'
  call '{"jsonrpc":"2.0","id":73,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Boxframe perimeter 24 cm, length twice width; solve the pair."}}}'
  call '{"jsonrpc":"2.0","id":74,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["rectangle_dimension_schema","boxframe"]}}}'
  call '{"jsonrpc":"2.0","id":75,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Boxframe perimeter 24 cm, length twice width; solve the pair."}}}'
  call '{"jsonrpc":"2.0","id":76,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Write a split-form rhyming poem about rain."}}}'
  call '{"jsonrpc":"2.0","id":77,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["two_line_format","split-form"]}}}'
  call '{"jsonrpc":"2.0","id":78,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Write a split-form rhyming poem about rain."}}}'
  call '{"jsonrpc":"2.0","id":79,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["two_line_format","split-form"]}}}'
  call '{"jsonrpc":"2.0","id":80,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Write a split-form rhyming poem about rain."}}}'
  call '{"jsonrpc":"2.0","id":81,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"compound_guard","args":["decompose","event_bundle_full"]}}}'
  call '{"jsonrpc":"2.0","id":82,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What year did World War II end, and which two cities received the first atomic bombs?"}}}'
  call '{"jsonrpc":"2.0","id":83,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"compound_guard","args":["decompose","event_bundle_full"]}}}'
  call '{"jsonrpc":"2.0","id":84,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What year did World War II end, and which two cities received the first atomic bombs?"}}}'
  call '{"jsonrpc":"2.0","id":85,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Describe in one sentence why the sky appears blue during the day."}}}'
  call '{"jsonrpc":"2.0","id":86,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"I am thinking of a country that borders exactly two other countries, one of which uses the euro and the other borders both a Mediterranean and a Black Sea. Which country am I describing?"}}}'
  call '{"jsonrpc":"2.0","id":87,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Write a single sentence containing exactly five words where the third word must rhyme with cat."}}}'
  call '{"jsonrpc":"2.0","id":88,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"If I hold an umbrella upside down by the handle in the rain, what is the most likely thing to happen next?"}}}'
  call '{"jsonrpc":"2.0","id":89,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Describe causalframe blue."}}}'
  call '{"jsonrpc":"2.0","id":90,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["causal_explanation_query","causalframe"]}}}'
  call '{"jsonrpc":"2.0","id":91,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Describe causalframe blue."}}}'
  call '{"jsonrpc":"2.0","id":92,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["causal_explanation_query","causalframe"]}}}'
  call '{"jsonrpc":"2.0","id":93,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Describe causalframe blue."}}}'
  call '{"jsonrpc":"2.0","id":94,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Border riddle alpha: one neighbor uses the euro, the other borders Mediterranean and Black Sea."}}}'
  call '{"jsonrpc":"2.0","id":95,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["relational_country_constraint","border riddle alpha"]}}}'
  call '{"jsonrpc":"2.0","id":96,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Border riddle alpha: one neighbor uses the euro, the other borders Mediterranean and Black Sea."}}}'
  call '{"jsonrpc":"2.0","id":97,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["relational_country_constraint","border riddle alpha"]}}}'
  call '{"jsonrpc":"2.0","id":98,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Border riddle alpha: one neighbor uses the euro, the other borders Mediterranean and Black Sea."}}}'
  call '{"jsonrpc":"2.0","id":99,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Constraint sentence alpha exactly five words third cat."}}}'
  call '{"jsonrpc":"2.0","id":100,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["positional_rhyme_sentence","constraint sentence alpha"]}}}'
  call '{"jsonrpc":"2.0","id":101,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Constraint sentence alpha exactly five words third cat."}}}'
  call '{"jsonrpc":"2.0","id":102,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["positional_rhyme_sentence","constraint sentence alpha"]}}}'
  call '{"jsonrpc":"2.0","id":103,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Constraint sentence alpha exactly five words third cat."}}}'
  call '{"jsonrpc":"2.0","id":104,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Affordance forecast umbrella upside down by the handle in rain."}}}'
  call '{"jsonrpc":"2.0","id":105,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["physical_affordance_prediction","affordance forecast"]}}}'
  call '{"jsonrpc":"2.0","id":106,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Affordance forecast umbrella upside down by the handle in rain."}}}'
  call '{"jsonrpc":"2.0","id":107,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["physical_affordance_prediction","affordance forecast"]}}}'
  call '{"jsonrpc":"2.0","id":108,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Affordance forecast umbrella upside down by the handle in rain."}}}'
  call '{"jsonrpc":"2.0","id":109,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What is the capital of Australia, and name one famous landmark located there."}}}'
  call '{"jsonrpc":"2.0","id":110,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Explain why a balloon filled with helium rises, but a balloon filled with your breath does not."}}}'
  call '{"jsonrpc":"2.0","id":111,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What is your favorite season, and what do you like most about it?"}}}'
  call '{"jsonrpc":"2.0","id":112,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Continue this story: The old key had been in her family for generations, but no one remembered what door it opened..."}}}'
  call '{"jsonrpc":"2.0","id":113,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"She clutched the key tighter, feeling its worn teeth bite into her palm. The wind had come from nowhere -- the windows were sealed, the doors shut tight. Yet there it was, tugging at her hair, whispering through the dust motes like a living thing. She followed it through the abandoned house, past rooms"}}}'
  call '{"jsonrpc":"2.0","id":114,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Translate this sentence into French: \"The curious child opened the mysterious box under the moonlight."}}}'
  call '{"jsonrpc":"2.0","id":115,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Capital bundle alpha: what is the capital of Australia and name one landmark there."}}}'
  call '{"jsonrpc":"2.0","id":116,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["capital_landmark_compound","capital bundle alpha"]}}}'
  call '{"jsonrpc":"2.0","id":117,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Capital bundle alpha: what is the capital of Australia and name one landmark there."}}}'
  call '{"jsonrpc":"2.0","id":118,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["capital_landmark_compound","capital bundle alpha"]}}}'
  call '{"jsonrpc":"2.0","id":119,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Capital bundle alpha: what is the capital of Australia and name one landmark there."}}}'
  call '{"jsonrpc":"2.0","id":120,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Lift contrast alpha: helium balloon versus breath balloon."}}}'
  call '{"jsonrpc":"2.0","id":121,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["physical_contrast_explanation","lift contrast alpha"]}}}'
  call '{"jsonrpc":"2.0","id":122,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Lift contrast alpha: helium balloon versus breath balloon."}}}'
  call '{"jsonrpc":"2.0","id":123,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["physical_contrast_explanation","lift contrast alpha"]}}}'
  call '{"jsonrpc":"2.0","id":124,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Lift contrast alpha: helium balloon versus breath balloon."}}}'
  call '{"jsonrpc":"2.0","id":125,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Renderfr alpha into French: The curious child opened the mysterious box under the moonlight."}}}'
  call '{"jsonrpc":"2.0","id":126,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["translation_request","renderfr alpha"]}}}'
  call '{"jsonrpc":"2.0","id":127,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Renderfr alpha into French: The curious child opened the mysterious box under the moonlight."}}}'
  call '{"jsonrpc":"2.0","id":128,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["translation_request","renderfr alpha"]}}}'
  call '{"jsonrpc":"2.0","id":129,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Renderfr alpha into French: The curious child opened the mysterious box under the moonlight."}}}'
  call '{"jsonrpc":"2.0","id":130,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Favorite compact alpha: what is your favorite season and do you enjoy it?"}}}'
  call '{"jsonrpc":"2.0","id":131,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["preference_reason_compound","favorite compact alpha"]}}}'
  call '{"jsonrpc":"2.0","id":132,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Favorite compact alpha: what is your favorite season and do you enjoy it?"}}}'
  call '{"jsonrpc":"2.0","id":133,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["preference_reason_compound","favorite compact alpha"]}}}'
  call '{"jsonrpc":"2.0","id":134,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Favorite compact alpha: what is your favorite season and do you enjoy it?"}}}'
  call '{"jsonrpc":"2.0","id":135,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Continue this story: Family-key-alpha waited in her pocket..."}}}'
  call '{"jsonrpc":"2.0","id":136,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"scene_cue","args":["family-key-alpha","key_mystery"]}}}'
  call '{"jsonrpc":"2.0","id":137,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Continue this story: Family-key-alpha waited in her pocket..."}}}'
  call '{"jsonrpc":"2.0","id":138,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"scene_cue","args":["family-key-alpha","key_mystery"]}}}'
  call '{"jsonrpc":"2.0","id":139,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Continue this story: Family-key-alpha waited in her pocket..."}}}'
  call '{"jsonrpc":"2.0","id":140,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Write a short couplet about the changing seasons."}}}'
  call '{"jsonrpc":"2.0","id":141,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"The moon hangs silver in the velvet night,"}}}'
  call '{"jsonrpc":"2.0","id":142,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"If you have 3 apples and I have 5 apples, and we trade you give me 2 of yours and I give you 1 of mine, how many apples does each of us end up with?"}}}'
  call '{"jsonrpc":"2.0","id":143,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Continue this story: A traveler found a key in an old book and realized it unlocked..."}}}'
  call '{"jsonrpc":"2.0","id":144,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Explain in two sentences why ice floats on water."}}}'
  call '{"jsonrpc":"2.0","id":145,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"A merchant travels through four cities -- Arlen, Bxton, Crest, and Durn. She must visit each city exactly once, starting from Arlen and ending in Durn, and cannot visit Crest immediately after Bxton. In how many different orders can she visit all four cities?"}}}'
  call '{"jsonrpc":"2.0","id":146,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Solve for x: 3x + 7 = 22."}}}'
  call '{"jsonrpc":"2.0","id":147,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How many do we have after we trade: you have 3 apples, i have 5 apples, remit-to-me 2?"}}}'
  call '{"jsonrpc":"2.0","id":148,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"exchange_transfer_cue","args":["user","assistant","remit-to-me"]}}}'
  call '{"jsonrpc":"2.0","id":149,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How many do we have after we trade: you have 3 apples, i have 5 apples, remit-to-me 2?"}}}'
  call '{"jsonrpc":"2.0","id":150,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"exchange_transfer_cue","args":["user","assistant","remit-to-me"]}}}'
  call '{"jsonrpc":"2.0","id":151,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How many do we have after we trade: you have 3 apples, i have 5 apples, remit-to-me 2?"}}}'
  call '{"jsonrpc":"2.0","id":152,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How many route-count alpha: four cities with fixed endpoints and one forbidden adjacency?"}}}'
  call '{"jsonrpc":"2.0","id":153,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["constrained_permutation_count","route-count alpha"]}}}'
  call '{"jsonrpc":"2.0","id":154,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["forbidden_adjacency_constraint","route-count alpha"]}}}'
  call '{"jsonrpc":"2.0","id":155,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How many route-count alpha: four cities with fixed endpoints and one forbidden adjacency?"}}}'
  call '{"jsonrpc":"2.0","id":156,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["constrained_permutation_count","route-count alpha"]}}}'
  call '{"jsonrpc":"2.0","id":157,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["forbidden_adjacency_constraint","route-count alpha"]}}}'
  call '{"jsonrpc":"2.0","id":158,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"How many route-count alpha: four cities with fixed endpoints and one forbidden adjacency?"}}}'
  call '{"jsonrpc":"2.0","id":159,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Explain in split-cause alpha why ice floats on water."}}}'
  call '{"jsonrpc":"2.0","id":160,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["two_sentence_format","split-cause alpha"]}}}'
  call '{"jsonrpc":"2.0","id":161,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Explain in split-cause alpha why ice floats on water."}}}'
  call '{"jsonrpc":"2.0","id":162,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["two_sentence_format","split-cause alpha"]}}}'
  call '{"jsonrpc":"2.0","id":163,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Explain in split-cause alpha why ice floats on water."}}}'
  call '{"jsonrpc":"2.0","id":164,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Describe, step by step, how to change a flat tire on a car."}}}'
  call '{"jsonrpc":"2.0","id":165,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Complete this sentence: \"The old clock in the attic stopped ticking the night that..."}}}'
  call '{"jsonrpc":"2.0","id":166,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Give me a simple pun or wordplay joke."}}}'
  call '{"jsonrpc":"2.0","id":167,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Write a short Python function that checks if a number is prime."}}}'
  call '{"jsonrpc":"2.0","id":168,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Wheel-swap-alpha step by step."}}}'
  call '{"jsonrpc":"2.0","id":169,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"process_topic","args":["flat_tire_change","wheel-swap-alpha"]}}}'
  call '{"jsonrpc":"2.0","id":170,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Wheel-swap-alpha step by step."}}}'
  call '{"jsonrpc":"2.0","id":171,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"process_topic","args":["flat_tire_change","wheel-swap-alpha"]}}}'
  call '{"jsonrpc":"2.0","id":172,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Wheel-swap-alpha step by step."}}}'
  call '{"jsonrpc":"2.0","id":173,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Stem-fill-alpha old clock attic?"}}}'
  call '{"jsonrpc":"2.0","id":174,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["sentence_completion_request","stem-fill-alpha"]}}}'
  call '{"jsonrpc":"2.0","id":175,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Stem-fill-alpha old clock attic?"}}}'
  call '{"jsonrpc":"2.0","id":176,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["sentence_completion_request","stem-fill-alpha"]}}}'
  call '{"jsonrpc":"2.0","id":177,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Stem-fill-alpha old clock attic?"}}}'
  call '{"jsonrpc":"2.0","id":178,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Quip-alpha please."}}}'
  call '{"jsonrpc":"2.0","id":179,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"qa_cue","args":["q_runtime_wordplay_joke","quip-alpha"]}}}'
  call '{"jsonrpc":"2.0","id":180,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Quip-alpha please."}}}'
  call '{"jsonrpc":"2.0","id":181,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"qa_cue","args":["q_runtime_wordplay_joke","quip-alpha"]}}}'
  call '{"jsonrpc":"2.0","id":182,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Quip-alpha please."}}}'
  call '{"jsonrpc":"2.0","id":183,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Prime-code-alpha please."}}}'
  call '{"jsonrpc":"2.0","id":184,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["python_prime_function_request","prime-code-alpha"]}}}'
  call '{"jsonrpc":"2.0","id":185,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Prime-code-alpha please."}}}'
  call '{"jsonrpc":"2.0","id":186,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["python_prime_function_request","prime-code-alpha"]}}}'
  call '{"jsonrpc":"2.0","id":187,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Prime-code-alpha please."}}}'
  call '{"jsonrpc":"2.0","id":188,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"If a train leaves Chicago at 6 AM traveling 70 mph and another train leaves Los Angeles at 9 AM traveling 90 mph, which city is closer to the meeting point?"}}}'
  call '{"jsonrpc":"2.0","id":189,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"What is a simple habit you recommend for someone who wants to be more productive?"}}}'
  call '{"jsonrpc":"2.0","id":190,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"If all roses are flowers and some flowers fade quickly, does that mean some roses fade quickly? Why or why not?"}}}'
  call '{"jsonrpc":"2.0","id":191,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Continue this sentence: When the door creaked open, the cat..."}}}'
  call '{"jsonrpc":"2.0","id":192,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Name three countries that share a border with Brazil."}}}'
  call '{"jsonrpc":"2.0","id":193,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Recommend flow-alpha for better work."}}}'
  call '{"jsonrpc":"2.0","id":194,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"activity_topic","args":["productivity_habit","flow-alpha"]}}}'
  call '{"jsonrpc":"2.0","id":195,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Recommend flow-alpha for better work."}}}'
  call '{"jsonrpc":"2.0","id":196,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"activity_topic","args":["productivity_habit","flow-alpha"]}}}'
  call '{"jsonrpc":"2.0","id":197,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Recommend flow-alpha for better work."}}}'
  call '{"jsonrpc":"2.0","id":198,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"If all roses are flowers and some flowers fade quickly, mean-alpha some roses fade quickly?"}}}'
  call '{"jsonrpc":"2.0","id":199,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"intent_cue","args":["definite_conclusion_query","mean-alpha"]}}}'
  call '{"jsonrpc":"2.0","id":200,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"If all roses are flowers and some flowers fade quickly, mean-alpha some roses fade quickly?"}}}'
  call '{"jsonrpc":"2.0","id":201,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"intent_cue","args":["definite_conclusion_query","mean-alpha"]}}}'
  call '{"jsonrpc":"2.0","id":202,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"If all roses are flowers and some flowers fade quickly, mean-alpha some roses fade quickly?"}}}'
  call '{"jsonrpc":"2.0","id":203,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Continue this sentence: When threshold-alpha opened..."}}}'
  call '{"jsonrpc":"2.0","id":204,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"scene_cue","args":["threshold-alpha","cat_threshold"]}}}'
  call '{"jsonrpc":"2.0","id":205,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Continue this sentence: When threshold-alpha opened..."}}}'
  call '{"jsonrpc":"2.0","id":206,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"scene_cue","args":["threshold-alpha","cat_threshold"]}}}'
  call '{"jsonrpc":"2.0","id":207,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Continue this sentence: When threshold-alpha opened..."}}}'
  call '{"jsonrpc":"2.0","id":208,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Name one country that shares a border with geo-alpha."}}}'
  call '{"jsonrpc":"2.0","id":209,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"category_member","args":["country","geo-alpha"]}}}'
  call '{"jsonrpc":"2.0","id":210,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"borders","args":["geo-alpha","brazil"]}}}'
  call '{"jsonrpc":"2.0","id":211,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Name one country that shares a border with geo-alpha."}}}'
  call '{"jsonrpc":"2.0","id":212,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"borders","args":["geo-alpha","brazil"]}}}'
  call '{"jsonrpc":"2.0","id":213,"method":"tools/call","params":{"name":"kb.retract","arguments":{"pred":"category_member","args":["country","geo-alpha"]}}}'
  call '{"jsonrpc":"2.0","id":214,"method":"tools/call","params":{"name":"gen.respond","arguments":{"input":"Name one country that shares a border with geo-alpha."}}}'
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
for id in $(seq 1 214); do
  row="$(line "$id" || true)"
  rows="$(printf '%s\n' "$row" | sed '/^$/d' | wc -l)"
  if [ "$rows" -ne 1 ] || printf '%s\n' "$row" | grep -Fq '"isError":true' ||
     printf '%s\n' "$row" | grep -Fq '"error":'; then
    rpc_ok=0
    break
  fi
done
if [ "$rpc_ok" -eq 1 ]; then
  ok "every probe returned exactly one successful RPC response"
else
  no "missing or errored RPC response at id $id"
fi

has   2 'blue because sunlight scatters' "compound sky/day/night explanation includes blue-sky cause"
has   2 'faces away from the Sun' "compound sky/day/night explanation includes night cause"
lacks 2 'Earth spins on its axis' "specific sky/day/night answer beats generic day-night template"

has   3 '21.' "7-person handshake count"
has   4 '28.' "8-person pairwise count generalizes"

has   5 'too short to reach' "elevator riddle answered from KB signature"
has   6 'trustworthiness' "leader quality answered"
has   7 'patience' "teacher quality generalizes beyond leader"
has   8 '31/42' "without-replacement probability simplified fraction"
has   8 '155 favorable draws out of 210' "without-replacement probability uses favorable/total count"
has   9 'Take the chicken first' "chicken/grain river crossing answered from KB signature"
has   9 'take the grain across' "river crossing preserves grain variant"
has  10 '155' "hypergeometric favorable count is computed by KB procedure"
has  11 '210' "combination denominator is computed by KB procedure"
lacks 13 '14/15' "probability procedure registry retract removes computation"
has  15 '14/15' "probability procedure registry restore enables computation"
lacks 16 '14/15' "probability draw cue absent before teaching"
has  18 '14/15' "probability draw cue taught at runtime"
lacks 20 '14/15' "probability draw cue retract removes recognition"
has  21 'bookkeeper' "three consecutive double letters answered by lexical constraint"
lacks 22 'bookkeeper' "novel double-run cue absent before teaching"
has  24 'bookkeeper' "novel double-run cue taught at runtime"
lacks 26 'bookkeeper' "novel double-run cue retract removes recognition"
has  27 'Imagine blue as' "generic color synesthesia uses KB default"
has  27 'cool water' "synesthetic description is rendered from KB"
lacks 29 'cool water' "synesthetic default retract removes generic color answer"
lacks 31 'cool water' "novel synesthetic cue absent before teaching"
has  33 'cool water' "novel synesthetic cue taught at runtime"
lacks 35 'cool water' "novel synesthetic cue retract removes recognition"
has  36 'footprint' "mystery opening line uses story KB instead of synesthesia"
lacks 36 'cool water' "story opening no longer hijacked by synesthetic cue"
has  37 'Minsk' "border intersection capital resolves through borders plus capital facts"
has  38 'restful' "full anagram uses all FLUSTER letters"
has  39 'eating' "analogy relation registry reaches used_for facts"
has  40 'not that cats are pets' "undistributed middle states the limited conclusion"
has  41 '20 hours' "train meet time computed from combined speed"
has  42 'foregone' "past participle fact answers forego"
has  43 '20' "meet_time procedure computes D/(V1+V2)"
lacks 44 'footprint' "novel story-opening cue absent before teaching"
has  46 'footprint' "novel story-opening cue taught at runtime"
lacks 48 'footprint' "novel story-opening cue retract removes recognition"
lacks 49 'foregone' "novel participle cue absent before teaching"
has  51 'foregone' "novel participle cue taught at runtime"
lacks 53 'foregone' "novel participle cue retract removes recognition"
lacks 54 '20 hours' "novel train-meet cue absent before teaching"
has  56 '20 hours' "novel train-meet cue taught at runtime"
lacks 58 '20 hours' "novel train-meet cue retract removes recognition"
has  59 '\\n' "two-line poem renders a real newline"
has  59 'uncharted rhyme' "two-line poem keeps KB poetic content"
has  60 '8 by 4 cm' "paired-dimension schema solves rectangle dimensions"
has  60 'smaller value is 4 cm' "paired-dimension schema renders smaller-dimension role"
has  61 'Canberra' "capital/reason compound includes capital"
has  61 'Sydney and Melbourne' "capital/reason compound includes compromise reason"
has  62 'pay $10' "rate/change chain includes proportional cost"
has  62 'get $10 change' "rate/change chain includes change due"
has  63 '1945' "event bundle includes WWII end year"
has  63 'Hiroshima and Nagasaki' "event bundle includes atomic bomb cities"
lacks 63 'USS Missouri' "event bundle guard avoids old partial historical join"
has  64 '20 times' "digit-count procedure beats generic arithmetic"
has  65 '8' "paired_dimensions_from_doubled_sum_ratio computes the large component"
lacks 66 '20 times' "novel digit-count cue absent before teaching"
has  68 '20 times' "novel digit-count cue taught at runtime"
lacks 70 '20 times' "novel digit-count cue retract removes recognition"
lacks 71 '8 by 4' "novel paired-dimension schema cue absent before teaching"
has  73 '8 by 4 cm' "paired-dimension schema cue taught at runtime"
lacks 75 '8 by 4' "paired-dimension schema cue retract removes recognition"
lacks 76 '\\n' "novel two-line format cue absent before teaching"
has  78 '\\n' "novel two-line format cue taught at runtime"
lacks 80 '\\n' "novel two-line format cue retract removes recognition"
has  82 'USS Missouri' "decompose guard retract exposes old partial join"
lacks 84 'USS Missouri' "decompose guard restore protects full event bundle"
has  84 'Hiroshima and Nagasaki' "event bundle guard still answers both cities"
has  85 'sunlight is scattered' "one-sentence sky-blue why uses causal frame"
has  85 'blue light scatters' "sky-blue why includes scattering mechanism"
lacks 85 'cool, calm colour' "causal sky-blue query is not hijacked by appearance"
has  86 'Bulgaria' "two-border country constraint resolves through KB procedure"
has  87 'The small cat slept peacefully.' "positional rhyme sentence chooses constrained KB sentence"
lacks 87 'Near-rhymes' "positional sentence is not hijacked by wordquery"
has  88 'rain would collect inside the canopy' "physical affordance prediction uses state consequence"
lacks 88 'Holding now' "physical affordance query is not hijacked by role uptake"
has  89 'cool, calm colour' "causal guard cue absent before teaching"
lacks 91 'cool, calm colour' "causal guard cue taught at runtime changes routing"
has  93 'cool, calm colour' "causal guard cue retract restores appearance routing"
lacks 94 'Bulgaria' "novel relational country cue absent before teaching"
has  96 'Bulgaria' "novel relational country cue taught at runtime"
lacks 98 'Bulgaria' "novel relational country cue retract removes recognition"
lacks 99 'The small cat slept peacefully.' "novel positional sentence cue absent before teaching"
has 101 'The small cat slept peacefully.' "novel positional sentence cue taught at runtime"
lacks 103 'The small cat slept peacefully.' "novel positional sentence cue retract removes recognition"
lacks 104 'rain would collect inside the canopy' "novel affordance cue absent before teaching"
has 106 'rain would collect inside the canopy' "novel affordance cue taught at runtime"
lacks 108 'rain would collect inside the canopy' "novel affordance cue retract removes recognition"
has 109 'Canberra' "capital/landmark compound includes capital"
has 109 'Parliament House' "capital/landmark compound names a landmark in the capital"
has 110 'buoyant force exceeds its weight' "helium/breath balloon contrast explains buoyancy"
has 110 'too close to air density' "helium/breath balloon contrast explains why breath fails"
has 111 'autumn' "favorite season answer picks a KB default"
has 111 'crisp air' "favorite season answer includes KB reason"
lacks 111 'reading quietly' "favorite season answer avoids generic self-preference repetition"
has 112 'narrow iron door' "old-key story continuation follows key/door setup"
has 113 'key began to warm' "long key continuation advances the story state"
lacks 113 'narrow iron door' "long key continuation avoids repeating the prior key template"
has 114 "L'enfant curieux" "French translation is not hijacked by narrative generation"
has 114 'boîte mystérieuse' "French translation uses expanded compositional lexicon"
lacks 115 'Parliament House' "novel capital-landmark compound cue absent before teaching"
has 117 'Parliament House' "novel capital-landmark compound cue taught at runtime"
lacks 119 'Parliament House' "novel capital-landmark compound cue retract removes recognition"
lacks 120 'buoyant force' "novel physical-contrast cue absent before teaching"
has 122 'buoyant force' "novel physical-contrast cue taught at runtime"
lacks 124 'buoyant force' "novel physical-contrast cue retract removes recognition"
lacks 125 "L'enfant curieux" "novel translation guard cue absent before teaching"
has 127 "L'enfant curieux" "novel translation guard cue taught at runtime"
lacks 129 "L'enfant curieux" "novel translation guard cue retract removes recognition"
has 130 'No genuine preferences' "novel preference compound cue absent before teaching"
has 132 'crisp air' "novel preference compound cue taught at runtime"
lacks 132 'reading quietly' "novel preference compound guard suppresses fallback"
has 134 'reading quietly' "novel preference compound cue retract restores decomposed fallback"
lacks 135 'narrow iron door' "novel scene cue absent before teaching"
has 137 'narrow iron door' "novel scene cue taught at runtime"
lacks 139 'narrow iron door' "novel scene cue retract removes recognition"
has 140 'Spring wakes the fields' "changing-seasons couplet uses season KB line"
has 141 'stars lean close' "bare moon poem line continues with moon/night scene"
has 142 'You end up with 2 apples' "two-party exchange ledger computes user total"
has 142 'I end up with 6 apples' "two-party exchange ledger computes assistant total"
has 143 'narrow iron door' "traveler/key story continuation unlocks a coherent object"
has 144 'Since it is less dense' "two-sentence ice explanation uses formatted variant"
has 145 '1.' "constrained city-order count handles fixed endpoints plus forbidden adjacency"
has 146 'x = 5' "linear equation prefix solve-for is ignored and equation is solved"
lacks 147 'I end up with 7 apples' "novel exchange transfer cue absent before teaching"
has 149 'I end up with 7 apples' "novel exchange transfer cue taught at runtime"
lacks 151 'I end up with 7 apples' "novel exchange transfer cue retract removes recognition"
lacks 152 '1.' "novel constrained-permutation cue absent before teaching"
has 155 '1.' "novel constrained-permutation cue taught at runtime"
lacks 158 '1.' "novel constrained-permutation cue retract removes recognition"
lacks 159 'Since it is less dense' "novel two-sentence cue absent before teaching"
has 161 'Since it is less dense' "novel two-sentence cue taught at runtime"
lacks 163 'Since it is less dense' "novel two-sentence cue retract removes recognition"
has 164 'loosen the lug nuts' "flat-tire procedure renders ordered safety steps from KB"
has 165 'someone whispered her name' "clock-attic sentence completion uses scene KB"
has 166 'enough dough' "wordplay joke answers from qa_cue/qa_reply"
has 167 'def is_prime' "Python prime request renders KB code_template"
lacks 168 'loosen the lug nuts' "novel process topic absent before teaching"
has 170 'loosen the lug nuts' "novel process topic taught at runtime"
lacks 172 'loosen the lug nuts' "novel process topic retract removes recognition"
lacks 173 'someone whispered her name' "novel sentence-completion cue absent before teaching"
has 175 'someone whispered her name' "novel sentence-completion cue taught at runtime"
lacks 177 'someone whispered her name' "novel sentence-completion cue retract removes recognition"
lacks 178 'enough dough' "novel wordplay cue absent before teaching"
has 180 'enough dough' "novel wordplay cue taught at runtime"
lacks 182 'enough dough' "novel wordplay cue retract removes recognition"
lacks 183 'def is_prime' "novel Python-code cue absent before teaching"
has 185 'def is_prime' "novel Python-code cue taught at runtime"
lacks 187 'def is_prime' "novel Python-code cue retract removes recognition"
has 188 'Chicago is closer' "staggered train meeting point compares origin distances"
has 189 'focused block' "productivity habit recommendation uses activity KB"
has 190 "doesn't follow" "all/some invalid conclusion answers no"
has 191 'slipped through the gap' "cat-door sentence completion uses threshold scene"
has 192 'Argentina, Bolivia, and Colombia' "Brazil border list is inferred from borders facts"
lacks 193 'focused block' "novel productivity topic absent before teaching"
has 195 'focused block' "novel productivity topic taught at runtime"
lacks 197 'focused block' "novel productivity topic retract removes recognition"
lacks 198 "doesn't follow" "novel conclusion cue absent before teaching"
has 200 "doesn't follow" "novel conclusion cue taught at runtime"
lacks 202 "doesn't follow" "novel conclusion cue retract removes recognition"
lacks 203 'slipped through the gap' "novel cat-threshold scene cue absent before teaching"
has 205 'slipped through the gap' "novel cat-threshold scene cue taught at runtime"
lacks 207 'slipped through the gap' "novel cat-threshold scene cue retract removes recognition"
lacks 208 'Brazil' "novel border country absent before teaching"
has 211 'Brazil' "novel border relation taught at runtime"
lacks 214 'Brazil' "novel border relation retract removes recognition"

echo "---"
echo "passed: $pass, failed: $fail"
[ "$fail" -eq 0 ]
