
## Run 2026-07-10 17:06:39 — model minimax-m2.5, 2 round(s) (probes file)

**already-capable 0 · taught->closed 1 (4 facts kept) · failed-lesson 1 · skipped 0**

### Round 1 — failed-lesson
- Q: Translate this sentence into Spanish: "The cat is sleeping on the warm rug."
- before: I can translate most of it, but I don't know the Spanish for "sleeping".
- lesson: tr_es(sleeping, durmiendo); tr_es(cat, gato); tr_es(warm, cálido); tr_es(rug, alfombra); gender_es(gato, m); gender_es(alfombra, f); gender_es(cálido, m); gender_es(cálida, f); adjective_agrees_es(cálida, alfombra); adjective_agrees_es(cálida, alfombra)
- after: El gato esta durmiendo en el cálido alfombra.
- judge: Translation has two errors: missing accent on 'está' and incorrect gender agreement for 'alfombra' (should be 'la cálida alfombra').

### Round 2 — taught
- Q: You're in a dark room with a match, a candle, and a fireplace. What do you light first?
- before: That sounds nice -- tell me more about it.
- lesson: riddle_sig(riddle_match, dark room); riddle_sig(riddle_match, match); riddle_sig(riddle_match, fireplace); response_template(riddle_match, You light the match first.)
- after: You light the match first.
- judge: Correct answer - the match must be lit first before lighting the candle or fireplace.


## Run 2026-07-10 17:09:15 — model minimax-m2.5, 4 round(s) (autonomous)

**already-capable 3 · taught->closed 0 (0 facts kept) · failed-lesson 0 · skipped 1**

### Round 1 — already-capable
- Q: What is the capital city of Australia?
- before: Canberra.
- judge: The answer correctly identifies Canberra as the capital city of Australia.

### Round 2 — already-capable
- Q: What is the opposite of the word "ancient"?
- before: Modern.
- judge: The answer is correct; modern is the appropriate opposite of ancient in terms of time period.

### Round 3 — already-capable
- Q: How do you say "Good morning" in Spanish?
- before: Buenos días.
- judge: Correct and concise translation of 'Good morning' in Spanish.

### Round 4 — skipped
- Q: I have no hands, no legs, nor wings, but I climb and I cling. What am I?
- before: I don't understand that yet.
- judge: no lesson JSON


## Run 2026-07-10 17:11:40 — model minimax-m2.5, 1 round(s) (probes file)

**already-capable 0 · taught->closed 0 (0 facts kept) · failed-lesson 1 · skipped 0**

### Round 1 — failed-lesson
- Q: I have no hands, no legs, nor wings, but I climb and I cling. What am I?
- before: I don't understand that yet.
- lesson: riddle_sig(suction_cup_riddle, no hands no legs nor wings climb cling); response_template(suction_cup_riddle, You are a suction cup); riddle_sig(suction_cup_riddle, no hands no legs nor wings but i climb and i cling); response_template(suction_cup_riddle, You are a suction cup); riddle_sig(suction_cup_riddle, no hands); riddle_sig(suction_cup_riddle, no legs); riddle_sig(suction_cup_riddle, nor wings); riddle_sig(suction_cup_riddle, but i climb and i cling); riddle_sig(suction_cup_riddle, climb); riddle_sig(suction_cup_riddle, cling); response_template(suction_cup_riddle, You are a suction cup)
- after: I don't understand that yet.
- judge: The answer fails to solve the riddle and simply states confusion rather than attempting to identify the climbing plant.


## Run 2026-07-10 17:13:57 — model minimax-m2.5, 1 round(s) (probes file)

**already-capable 0 · taught->closed 0 (0 facts kept) · failed-lesson 1 · skipped 0**

### Round 1 — failed-lesson
- Q: I have no hands, no legs, nor wings, but I climb and I cling. What am I?
- before: I don't understand that yet.
- lesson: riddle_sig(riddle_vine, no hands); riddle_sig(riddle_vine, I climb); riddle_sig(riddle_vine, I cling); response_template(riddle_vine, You are a vine, a climbing plant that climbs and clings to surfaces.)
- after: I don't understand that yet.
- judge: The answer fails to solve the riddle and simply states confusion rather than attempting a response.


## Run 2026-07-10 17:15:05 — model minimax-m2.5, 1 round(s) (probes file)

**already-capable 0 · taught->closed 1 (4 facts kept) · failed-lesson 0 · skipped 0**

### Round 1 — taught
- Q: I have no hands, no legs, nor wings, but I climb and I cling. What am I?
- before: I don't understand that yet.
- lesson: riddle_sig(riddle_vine, no hands); riddle_sig(riddle_vine, i climb); riddle_sig(riddle_vine, i cling); response_template(riddle_vine, You are a vine.)
- after: You are a vine.
- judge: The answer 'vine' correctly fits all the riddle's clues - it lacks hands, legs, and wings, yet climbs and clings to surfaces.


## Run 2026-07-10 17:35:42 — model minimax-m2.5, 20 round(s) (autonomous)

**already-capable 9 · taught->closed 2 (3 facts kept) · failed-lesson 7 · skipped 2**

### Round 1 — already-capable
- Q: What is the largest planet in our solar system?
- before: Jupiter is the largest planet in our solar system.
- judge: The answer correctly identifies Jupiter as the largest planet in our solar system.

### Round 2 — failed-lesson
- Q: How do you say "I love to read books" in Spanish?
- before: I can translate most of it, but I don't know the Spanish for "i".
- lesson: tr_es(i, yo); tr_es(love, amar); tr_es(to read, leer); tr_es(books, libros)
- after: I can translate most of it, but I don't know the Spanish for "to".
- judge: The answer fails to provide the translation and incorrectly claims ignorance about 'to' before an infinitive, which in Spanish is simply the -er ending on the verb.

### Round 3 — already-capable
- Q: I'm tall when I'm young, and I'm short when I'm old. What am I?
- before: A candle.
- judge: The answer is correct and responsive—candles are indeed tall when new and short when burned down.

### Round 4 — already-capable
- Q: What is the opposite of the word "ancient"?
- before: Modern.
- judge: The answer is correct and directly provides the standard antonym for 'ancient'.

### Round 5 — already-capable
- Q: What color do you get when you mix blue and yellow together?
- before: Green.
- judge: Correct answer - mixing blue and yellow pigment creates green.

### Round 6 — already-capable
- Q: What sound does a cat make?
- before: A cat goes "meow".
- judge: The answer is correct, direct, and on-topic with a standard cat sound.

### Round 7 — taught
- Q: How do you say "The sun is shining" in French?
- before: I can translate most of it, but I don't know the French for "sun".
- lesson: tr_fr(sun, soleil); tr_fr(shining, brille)
- after: Le soleil brille.
- judge: The answer provides the correct French translation 'Le soleil brille' which accurately conveys 'The sun is shining.'

### Round 8 — failed-lesson
- Q: What is the definition of the word "ephemeral"?
- before: I don't understand that yet.
- lesson: wiki_concept(ephemeral, adjective, lasting for a very short time; transitory or short-lived); wiki_concept(ephemeral, adjective, lasting for a very short time; transitory or short-lived)
- after: I don't understand that yet.
- judge: The answer deflects instead of providing the straightforward definition of 'ephemeral' (lasting for a very short time).

### Round 9 — failed-lesson
- Q: What famous person is credited with saying "To be, or not to be, that is the question"?
- before: I don't understand that yet.
- lesson: quote_speaker(to be or not to be, william_shakespeare); author(hamlet, william_shakespeare)
- after: I don't understand that yet.
- judge: The answer deflects instead of providing the correct response (William Shakespeare from Hamlet).

### Round 10 — failed-lesson
- Q: Which is heavier: a pound of feathers or a pound of bricks?
- before: I recognize a comparison on weight, but I don't have magnitudes for Feathers and Pound.
- lesson: riddle_sig(riddle_pound_comparison, which is heavier); riddle_sig(riddle_pound_comparison, pound of feathers); riddle_sig(riddle_pound_comparison, pound of bricks); response_template(riddle_pound_comparison, A pound of feathers and a pound of bricks weigh the same amount, because both are exactly one pound in weight.)
- after: I recognize a comparison on weight, but I don't have magnitudes for Feathers and Pound.
- judge: The answer fails to provide the correct information and deflects instead of stating that a pound of feathers and a pound of bricks weigh the same.

### Round 11 — already-capable
- Q: What is the capital city of Japan?
- before: Tokyo.
- judge: The answer correctly identifies Tokyo as the capital city of Japan and is concise.

### Round 12 — already-capable
- Q: What is 15% of 200?
- before: 30.
- judge: Correct answer: 15% of 200 equals 30 (0.15 × 200 = 30).

### Round 13 — skipped
- Q: I'm a word of five letters. Remove my first letter, and I become a type of fish. Remove my first two letters, and I become a type of crime. What word am I?
- before: That sounds nice -- tell me more about it.
- judge: no lesson JSON

### Round 14 — already-capable
- Q: What planet is known as the "Red Planet"?
- before: Mars is the Red Planet.
- judge: The answer correctly identifies Mars as the Red Planet and explains the reason briefly.

### Round 15 — taught
- Q: What is the chemical symbol for gold?
- before: I don't understand that yet.
- lesson: chemical_symbol(gold, au)
- after: chemical_symbol(gold, au).
- judge: The answer correctly identifies Au as the chemical symbol for gold.

### Round 16 — failed-lesson
- Q: What is the fastest land animal in the world?
- before: I don't understand that yet.
- lesson: magnitude(speed, cheetah, 1); magnitude_cue(fastest land animal, speed, max); magnitude_cue(fastest animal, speed, max)
- after: I don't understand that yet.
- judge: The question is clear and factual, but the answer deflects rather than providing the correct answer (cheetah).

### Round 17 — failed-lesson
- Q: What does the idiom "break the ice" mean?
- before: I don't understand that yet.
- lesson: wiki_concept(break_the_ice, idiom, to initiate conversation or action to relieve awkwardness or tension in a social situation); wiki_concept(break the ice, idiom, to initiate conversation or action to relieve awkwardness or tension in a social situation)
- after: I don't understand that yet.
- judge: The answer deflects and fails to provide the requested definition of the idiom.

### Round 18 — already-capable
- Q: In what year did World War II end?
- before: World War II ended in 1945; Japan's surrender was signed on Sept 2, 1945, aboard the USS Missouri in Tokyo Bay.
- judge: The answer correctly states that WWII ended in 1945 and provides the specific date of Japan's surrender.

### Round 19 — skipped
- Q: Who is credited with inventing the telephone?
- before: I understood you're asking about credited with inventing the telephone: I tried to look it up, but I don't have a source to learn it from yet.
- judge: The available predicate schemas (translations, riddles, wiki_concept, physical properties, comparisons, and binary relations like capital/opposite) do not include an 'inventor' or 'invented_by' relation type. This invention-attribution question class cannot be mapped to any schema the engine consumes.

### Round 20 — failed-lesson
- Q: What is the name of the first book in the Harry Potter series?
- before: I don't understand that yet.
- lesson: first_book(harry_potter, harry_potter_and_the_sorcerers_stone); wiki_concept(harry_potter_and_the_sorcerers_stone, novel, Harry Potter and the Sorcerer's Stone is the first novel in the Harry Potter series, published in 1997); wiki_concept(harry_potter, book_series, Harry Potter is a series of seven fantasy novels written by British author J.K. Rowling); first_book(harry_potter, harry_potter_and_the_sorcerers_stone); first_book(harry_potter_series, harry_potter_and_the_sorcerers_stone)
- after: I don't understand that yet.
- judge: The assistant deflected with 'I don't understand' when it should know the answer is 'Harry Potter and the Philosopher's Stone'.


## Run 2026-07-10 17:47:08 — model minimax-m2.5, 20 round(s) (autonomous)

**already-capable 8 · taught->closed 5 (10 facts kept) · failed-lesson 3 · skipped 3**

### Round 1 — already-capable
- Q: What is the capital of Australia?
- before: Canberra.
- judge: Correct and direct answer to the factual question.

### Round 2 — failed-lesson
- Q: How do you say "I love books" in Spanish?
- before: I can translate most of it, but I don't know the Spanish for "i".
- lesson: tr_es(i, yo); tr_es(love, amar); tr_es(books, libros); tr_es(love, encantar); tr_es(love, amo); tr_es(the, los)
- after: Yo amar libros.
- judge: The verb 'amar' must be conjugated as 'amo' for 'yo', not left as the infinitive.

### Round 3 — already-capable
- Q: What is the opposite of the word "ancient"?
- before: Modern.
- judge: The answer 'Modern' is correct and concise.

### Round 4 — taught
- Q: I have cities, but no houses live there. I have mountains, but no trees grow there. I have water, but no fish swim there. I have roads, but no cars drive there. What am I?
- before: I don't understand that yet.
- lesson: riddle_sig(map_riddle, no houses live); riddle_sig(map_riddle, no trees grow); riddle_sig(map_riddle, what am i); response_template(map_riddle, You are a map.)
- after: You are a map.
- judge: The answer 'a map' correctly solves the classic riddle, identifying that maps depict cities, mountains, water, and roads without the living elements or activities associated with them.

### Round 5 — taught
- Q: What sound does a seal make?
- before: I don't understand that yet.
- lesson: sound_of(seal, bark)
- after: A seal goes "bark".
- judge: The answer correctly identifies that seals make a bark-like sound, which is accurate.

### Round 6 — failed-lesson
- Q: What is the definition of the word "ubiquitous"?
- before: I don't understand that yet.
- lesson: wiki_concept(ubiquitous, vocabulary, present_appearing_or_found_everywhere); wiki_concept(ubiquitous, vocabulary, existing_or_being_everywhere_at_the_same_time); wiki_concept(ubiquitous, general, present_appearing_or_found_everywhere)
- after: I don't understand that yet.
- judge: The answer fails to provide the definition and deflects with 'I don't understand' instead of answering the straightforward definitional question.

### Round 7 — already-capable
- Q: What color do you get when you mix blue and yellow paint together?
- before: Green.
- judge: The answer correctly states that mixing blue and yellow paint produces green, which is accurate for subtractive color mixing.

### Round 8 — taught
- Q: How do you say "Good morning" in French?
- before: I can translate most of it, but I don't know the French for "good".
- lesson: tr_fr(good, bon); tr_fr(morning, matin); idiom_meaning(good morning, bonjour)
- after: Bon matin.
- judge: The answer is on-topic and responds to the question, though 'Bon matin' is used more in Canadian French than standard Parisian French.

### Round 9 — interviewer-error
- Q: [model error 503]

### Round 10 — already-capable
- Q: Which planet is known as the Red Planet?
- before: Mars is the Red Planet.
- judge: The answer correctly identifies Mars as the Red Planet.

### Round 11 — already-capable
- Q: I speak without a mouth and hear without ears. I have no body, but I come alive with the wind. What am I?
- before: An echo.
- judge: The answer 'An echo' correctly solves the riddle, as an echo reflects sound without a mouth, responds without ears, has no body, and is produced when sound waves bounce back, often in areas affected by wind.

### Round 12 — skipped
- Q: If a train travels 60 miles per hour, how far will it travel in 2.5 hours?
- before: I don't understand that yet.
- judge: Basic arithmetic computation (distance = speed × time) is not a knowledge-base fact type; the engine's schema only supports stored facts (translations, riddles, concepts, idioms, relations) — it cannot perform on-the-fly math calculations.

### Round 13 — already-capable
- Q: What year did World War II end?
- before: World War II ended in 1945; Japan's surrender was signed on Sept 2, 1945, aboard the USS Missouri in Tokyo Bay.
- judge: The answer correctly states that WWII ended in 1945 and provides accurate historical details about Japan's surrender.

### Round 14 — already-capable
- Q: What is the chemical symbol for gold?
- before: chemical_symbol(gold, au).
- judge: The answer correctly identifies 'Au' as the chemical symbol for gold.

### Round 15 — failed-lesson
- Q: What is the largest mammal on Earth?
- before: I don't understand that yet.
- lesson: magnitude(mammal_size, blue_whale, 1); magnitude_cue(largest, mammal_size, max)
- after: I don't understand that yet.
- judge: The answer refuses to answer the simple factual question; it should state that the blue whale is the largest mammal.

### Round 16 — taught
- Q: What is the hardest natural substance on Earth?
- before: I don't understand that yet.
- lesson: hardest_natural_substance(diamond, earth)
- after: hardest_natural_substance(diamond, earth).
- judge: Diamond is correctly identified as the hardest natural substance on Earth.

### Round 17 — skipped
- Q: What is the missing number in this sequence: 2, 4, 8, 16, ___?
- before: I don't understand that yet.
- judge: No matching predicate schema exists for mathematical sequence patterns or number sequence completion; the KB architecture cannot represent this query class.

### Round 18 — taught
- Q: What does the idiom "break the ice" mean?
- before: I don't understand that yet.
- lesson: idiom_meaning(break the ice, to initiate conversation or ease tension in a social setting by making people feel more comfortable)
- after: "break the ice" means to initiate conversation or ease tension in a social setting by making people feel more comfortable.
- judge: The answer correctly defines the idiom 'break the ice' as initiating conversation or easing social tension, which is accurate.

### Round 19 — skipped
- Q: Can you give me a three-line poem that rhymes and includes the words "moon" and "tune"?
- before: The pale moon climbs and spills its quiet light; it lays a silver road across the night.
- judge: Poem generation is a creative task outside the scope of a deterministic knowledge-base engine, which retrieves stored facts rather than generating creative content. No predicate schema supports this capability.

### Round 20 — already-capable
- Q: Under the glowing moon so bright,
- before: Shimmer with a soft silver light.
- judge: The answer is a fluent poetic continuation that matches the moonlit imagery and maintains the poetic register of the question.






I numeri

┌────────────────────────────────┬─────────┬────────┬──────────────┬────────┬─────────┐
│                                │ already │ closed │ fatti tenuti │ failed │ skipped │                                                                                                                          
├────────────────────────────────┼─────────┼────────┼──────────────┼────────┼─────────┤
│ Run 1                          │ 9       │ 2      │ +3           │ 7      │ 2       │
├────────────────────────────────┼─────────┼────────┼──────────────┼────────┼─────────┤
│ Run 2 (dopo tuning dal ledger) │ 8       │ 5      │ +10          │ 3      │ 3       │
└────────────────────────────────┴─────────┴────────┴──────────────┴────────┴─────────┘

KB appreso: 21 fatti, tutti veri, zero spazzatura (l'oracolo ha respinto ~10 lezioni fallite senza sporcare nulla).

Ragionamento 1 — Il trainer è esso stesso addestrabile (il risultato più importante)

Tra run 1 e run 2 non ho toccato architettura: ho letto il ledger, aggiunto idiom_meaning alla whitelist e due righe di guida al teacher → la resa è 2.5×. Il loop esterno (analizza ledger → aggiusta whitelist/prompt → run) è
LOOP.md applicato al training invece che al C. E la conferma immediata: "break the ice", fallita al run 1, chiusa al run 2 con la lezione giusta.

Ragionamento 2 — Il ledger classifica il mondo da solo, in 4 code distinte

I fallimenti non sono rumore: si separano da soli in categorie con rimedi diversi:

1. Gap di dati → si chiudono da soli (riddle, suoni, idiomi, simboli chimici, gloss FR). Costo ~zero, scala col provider.
2. Gap di consumer (2 casi netti, ripetuti in entrambi i run): "what is the definition of the word X" non raggiunge wiki_concept; il superlativo aperto ("largest mammal") non enumera magnitude. Ogni consumer generico = una gen C
   che sblocca un'intera classe insegnabile per sempre. Questa è la coda che merita C.
3. Gap di computazione (skipped onesti: treno 60mph, sequenza 2-4-8-16): il teacher riconosce che non sono fatti — è territorio S3 (istruzioni-come-programmi), non lezioni. Il confine lezione/computazione emerge da solo.
4. Igiene di motore: l'ombra di routing (magnitude rivendica pound of feathers prima del riddle KB) e il rendering crudo (chemical_symbol(gold, au). come risposta). Piccoli, precisi, con caso minimo allegato.

Ragionamento 3 — Due astuzie del teacher da giudicare (onestà)

- idiom_meaning(good morning, bonjour) per "How do you say Good morning in French": ha usato il canale idiom come canale di traduzione di frase. Funziona ed è conoscenza vera, ma è un frame improprio — se uno chiede "what does
  good morning mean" risponderebbe "bonjour". Borderline: da tenere d'occhio.
- hardest_natural_substance(diamond, earth): il frame binario generico gen11 è più flessibile del previsto — ha risposto a una domanda superlativa che il canale magnitude non copre. Vinto legittimamente, ma mostra che due canali
  si sovrappongono.

Ragionamento 4 — Il quadro economico

Un run da 20 = ~10 minuti e centesimi. Il tasso attuale: ~1 fatto/minuto con oracle a zero inquinamento, più il ledger che si auto-compila. È sostenibile come processo notturno (cron), e la curva migliorerà ancora perché ogni
consumer-gap chiuso in C converte una categoria intera di failed→taught.

Le tre decisioni che ora spettano a te

1. I 2 consumer-gap ricorrenti (definition-phrasing, superlativo aperto): li chiudo come gen C classiche? Sono le uniche C giustificate dal ledger, ciascuna sblocca una classe.
2. L'ombra di routing (magnitude vs riddle): il declino informato di un modulo blocca la conoscenza insegnabile a valle — questione di principio sul first-match: il declino informato dovrebbe cedere il turno se un KB a valle sa
   rispondere?
3. Il canale-improprio (idiom-come-traduzione): lo accettiamo (è conoscenza che funziona) o stringiamo la guida del teacher (purezza dei frame)?

## Run 2026-07-10 18:02:21 — model minimax-m2.5, 5 round(s), 5 workers, multiply x20 (autonomous)

**already 3 · taught 0 · failed 2 · skipped 0 · kept 0 facts (0 seed + 0 multiplied, deduped)**

### Round 1 — already-capable
- Q: What is the capital of Australia?
- before: Canberra.
- judge: The answer correctly identifies Canberra as the capital of Australia.

### Round 2 — already-capable
- Q: What is the chemical symbol for gold?
- before: chemical_symbol(gold, au).
- judge: The answer correctly identifies the chemical symbol for gold as Au, albeit in a Prolog-style format.

### Round 3 — failed-lesson
- Q: How do you say "Thank you very much" in Spanish?
- before: I can translate most of it, but I don't know the Spanish for "thank".
- lesson: tr_es(thanks, gracias); tr_es(very, muy); tr_es(much, mucho); tr_es(thank, gracias)
- after: I can translate most of it, but I don't know the Spanish for "you".
- judge: The answer is incorrect and unhelpful; it claims ignorance about a word ('you') that isn't even in the phrase, and fails to provide the actual translation 'Muchas gracias'.

### Round 4 — failed-lesson
- Q: How do you say "I am hungry" in French?
- before: I can translate most of it, but I don't know the French for "i".
- lesson: tr_fr(i, je); tr_fr(hungry, faim); tr_fr(am, suis); idiom_meaning(i am hungry, in french jai faim uses avoir not etre literally i have hunger but means i am hungry); tr_fr(i am hungry, jai faim)
- after: Je suis faim.
- judge: The answer is incorrect; the correct French expression is 'J'ai faim' (I have hunger), not 'Je suis faim'.

### Round 5 — already-capable
- Q: I have cities but no houses, forests but no trees, and water but no fish. What am I?
- before: A map.
- judge: The answer is correct as a map accurately depicts cities, forests, and water without the actual physical entities.


## Run 2026-07-10 18:42:58 — model minimax-m2.5, 1 round(s), 1 workers, multiply x0 (autonomous)

**already 0 · taught 0 · failed 1 · skipped 0 · kept 0 facts (0 seed + 0 multiplied, deduped)**

### Round 1 — failed-lesson
- Q: How many capitals does South Africa have?
- before: I don't understand that yet.
- lesson: capital(pretoria, south_africa); capital(capetown, south_africa); capital(bloemfontein, south_africa); capital_count(south_africa, three); magnitude_cue(how many, capitals, max); magnitude(capitals, south_africa, 3); magnitude_cue(many, capitals, count); capital_count(south_africa, three); magnitude(capitals, south_africa, 3)
- after: I don't understand that yet.
- judge: The answer deflects the question instead of providing the factual information that South Africa has three capitals (Pretoria, Cape Town, and Bloemfontein).


## Run 2026-07-10 18:48:13 — model minimax-m2.5, 5 round(s), 5 workers, multiply x20 (autonomous)

**already 1 · taught 1 · failed 3 · skipped 0 · kept 1 facts (1 seed + 0 multiplied, deduped)**

### Round 1 — taught
- Q: What is the capital of Brazil?
- before: I do not know the relation capital yet, so I cannot answer the capital of brazil. You can teach me with thing is the capital of brazil, or give facts/rules to reason from.
- lesson: capital(brasilia, brazil)
- after: Brasilia.
- judge: The answer correctly identifies Brasília as the capital of Brazil.

### Round 2 — failed-lesson
- Q: What element does the chemical symbol "Fe" represent?
- before: I don't understand that yet.
- lesson: chemical_symbol(fe, iron); wiki_concept(fe, chemical_symbol, Fe is the chemical symbol for iron, a metallic element with atomic number 26 and symbol Fe); wiki_concept(iron, chemical_element, Iron is a metallic element with symbol Fe and atomic number 26, atomic mass 55.845); chemical_symbol(fe, iron); element_of(fe, iron); wiki_concept(fe, chemical_symbol, Fe is the chemical symbol for iron, a metallic element with atomic number 26); represents(fe, iron)
- after: Hmm, I don't know about element yet.
- judge: The answer incorrectly deflects; 'Fe' is the chemical symbol for iron, which is basic chemistry knowledge.

### Round 3 — failed-lesson
- Q: How do you say "How are you?" in Spanish?
- before: I can translate most of it, but I don't know the Spanish for "how".
- lesson: tr_es(how, cómo); tr_es(are, estar); tr_es(you, tú); idiom_meaning(how are you, a common greeting asking about someone's wellbeing); tr_es(how are you, cómo estás); tr_es(how are you, cómo está)
- after: Cómo estar tú.
- judge: The answer is incorrect; the proper Spanish phrase is '¿Cómo estás?' (with proper conjugation 'estás' for informal tú, and with opening question mark).

### Round 4 — failed-lesson
- Q: How do you say "Good night" in French?
- before: I can translate most of it, but I don't know the French for "good".
- lesson: tr_fr(good, bon); tr_fr(night, nuit); tr_fr(good, bonne); gender_fr(nuit, f)
- after: Nuit bon.
- judge: The correct French translation is 'Bonne nuit' not 'Nuit bon'; the adjective must precede the noun in French.

### Round 5 — already-capable
- Q: I have cities but no houses, forests but no trees, and water but no fish. What am I?
- before: A map.
- judge: The answer 'A map' is correct as maps depict cities, forests, and water without containing the actual houses, trees, or fish.


## Run 2026-07-10 18:53:12 — model minimax-m2.5, 1 round(s), 1 workers, multiply x0 (autonomous)

**already 1 · taught 0 · failed 0 · engine-gap 0 · skipped 0 · kept 0 facts (0 seed + 0 multiplied, deduped)**

### Round 1 — already-capable
- Q: What is the capital city of Australia?
- before: Canberra.
- judge: The answer correctly identifies Canberra as the capital city of Australia.


## Run 2026-07-10 19:00:39 — model minimax-m2.5, 1 round(s), 1 workers, multiply x0 (autonomous)

**already 1 · taught 0 · failed 0 · engine-gap 0 · skipped 0 · kept 0 facts (0 seed + 0 multiplied, deduped)**

### Round 1 — already-capable
- Q: What is the capital city of Australia?
- before: Canberra.
- judge: The answer correctly identifies Canberra as Australia's capital city.


## Run 2026-07-10 19:01:33 — model minimax-m2.5, 1 round(s), 1 workers, multiply x0 (autonomous)

**already 1 · taught 0 · failed 0 · engine-gap 0 · skipped 0 · kept 0 facts (0 seed + 0 multiplied, deduped)**

### Round 1 — already-capable
- Q: What is the capital city of Australia?
- before: Canberra.
- judge: Canberra is the correct capital city of Australia.


## Run 2026-07-10 19:08:28 — model minimax-m2.5, 20 round(s), 5 workers, multiply x20 (autonomous)

**already 4 · taught 9 · failed 1 · engine-gap 4 · skipped 1 · kept 152 facts (17 seed + 140 multiplied, deduped)**

**diagnoses: composition_gap 1 · engine_gap 6 · idiom_gap 1 · missing_fact 6 · unknown 1**

### Round 1 — already-capable
- Q: What is the capital of Australia?
- before: Canberra.
- judge: Correct and direct answer to the factual question.

### Round 2 — taught
- Q: What is the chemical symbol for sodium?
- before: I don't understand that yet.
- diagnosis: missing_fact · teachable=true · mode=fact
- next: Add the fact mapping sodium → Na to the knowledge base.
- lesson: chemical_symbol(na, sodium)
- multiplied (+20): chemical_symbol(h, hydrogen); chemical_symbol(he, helium); chemical_symbol(li, lithium); chemical_symbol(c, carbon); chemical_symbol(n, nitrogen); chemical_symbol(o, oxygen); chemical_symbol(f, fluorine); chemical_symbol(ne, neon); chemical_symbol(cl, chlorine); chemical_symbol(k, potassium); chemical_symbol(ca, calcium); chemical_symbol(mg, magnesium); chemical_symbol(fe, iron); chemical_symbol(cu, copper); chemical_symbol(zn, zinc); chemical_symbol(ag, silver); chemical_symbol(au, gold); chemical_symbol(hg, mercury); chemical_symbol(pb, lead); chemical_symbol(s, sulfur)
- after: chemical_symbol(na, sodium).
- judge: The answer correctly identifies Na as the chemical symbol for sodium, responding accurately to the question.

### Round 3 — taught
- Q: How do you say "How are you?" in Spanish?
- before: I can translate most of it, but I don't know the Spanish for "how".
- diagnosis: composition_gap · teachable=true · mode=template
- next: Teach 'Cómo estás' as a single phrase template rather than individual word translations.
- lesson: tr_es(how, cómo); tr_es(are, estás); tr_es(you, tú); tr_es(you, te); idiom_meaning(cómo estás, how are you?)
- after: Cómo estás tú.
- judge: The answer correctly provides the Spanish translation 'Cómo estás tú' for 'How are you?', which is grammatically correct though the more common form omits 'tú'.

### Round 4 — failed-lesson
- Q: Translate "Thank you very much" into French.
- before: I can translate most of it, but I don't know the French for "thank".
- diagnosis: idiom_gap · teachable=true · mode=skip
- next: Test whether the user can recognize that 'merci beaucoup' is a single idiomatic chunk, not a composable word-for-word translation.
- lesson: tr_fr(thank, merci); tr_fr(very, très); tr_fr(much, beaucoup); idiom_meaning(merci beaucoup, thank you very much); idiom_meaning(merci, thank you)
- after: I can translate most of it, but I don't know the French for "you".
- judge: The answer is factually wrong; 'you' is not a separate word to translate in 'Thank you very much'—the French phrase is simply 'Merci beaucoup.'

### Round 5 — already-capable
- Q: I speak without a mouth and hear without ears. I have no body, but I come alive with wind. What am I?
- before: An echo.
- judge: The answer 'An echo' correctly solves the riddle by matching all the clues: it repeats sounds without a mouth, responds to sounds without ears, has no physical body, and is produced by sound waves.

### Round 6 — interviewer-error
- Q: [empty]

### Round 7 — taught
- Q: What does it mean when someone says "break a leg" to a performer before they go on stage?
- before: Who or what does "it" refer to?
- diagnosis: engine_gap · teachable=false · mode=skip
- next: Debug the answer path to ensure questions about idiom meanings trigger the idiom handler rather than grammatical parsing.
- lesson: idiom_meaning(break a leg, good luck especially to performers)
- multiplied (+20): idiom_meaning(break the ice, to initiate conversation or relieve tension in a social setting); idiom_meaning(piece of cake, something very easy to do); idiom_meaning(under the weather, feeling ill or sick); idiom_meaning(spill the beans, to reveal secret information); idiom_meaning(hit the nail on the head, to describe exactly the cause of a situation); idiom_meaning(cost an arm and a leg, to be very expensive); idiom_meaning(once in a blue moon, very rarely); idiom_meaning(let the cat out of the bag, to reveal a secret); idiom_meaning(kill two birds with one stone, to achieve two aims with a single action); idiom_meaning(barking up the wrong tree, to pursue a mistaken or misguided course of action); idiom_meaning(beat around the bush, to avoid talking about what is important); idiom_meaning(break the bank, to exceed one's budget, to be too expensive); idiom_meaning(bite the bullet, to endure a painful or difficult situation with courage); idiom_meaning(burn the midnight oil, to work late into the night); idiom_meaning(call it a day, to stop working on something); idiom_meaning(cut to the chase, to get to the point quickly); idiom_meaning(do the math, to calculate or consider the facts); idiom_meaning(fingers crossed, hoping for good luck); idiom_meaning(go the extra mile, to make more effort than expected); idiom_meaning(hit the books, to study)
- after: "break a leg" means good luck -- it's a way to wish someone a great performance, especially on stage.
- judge: The answer accurately explains that 'break a leg' is a theatrical idiom used to wish performers good luck.

### Round 8 — taught
- Q: What sound does a koala bear make?
- before: I don't understand that yet.
- diagnosis: missing_fact · teachable=true · mode=fact
- next: Add factual knowledge entry for koala bear sounds (bellow/grunt) to the knowledge base and verify the answer path can consume it.
- lesson: sound_of(koala, bellow)
- multiplied (+20): sound_of(dog, bark); sound_of(cat, meow); sound_of(cow, moo); sound_of(horse, neigh); sound_of(pig, oink); sound_of(sheep, baa); sound_of(goat, bleat); sound_of(duck, quack); sound_of(chicken, cluck); sound_of(rooster, crow); sound_of(owl, hoot); sound_of(frog, croak); sound_of(lion, roar); sound_of(elephant, trumpet); sound_of(wolf, howl); sound_of(bear, growl); sound_of(snake, hiss); sound_of(bee, buzz); sound_of(mouse, squeak); sound_of(donkey, bray)
- after: A koala goes "bellow".
- judge: The answer correctly identifies that koalas make a bellowing sound, which is accurate.

### Round 9 — engine-gap
- Q: Which is bigger, a great white shark or a blue whale?
- before: I recognize a comparison on size, but I don't have magnitudes for Shark and Blue.
- diagnosis: engine_gap · teachable=false · mode=skip
- next: Record the failed lesson as a consumer gap before adding more facts.
- lesson: magnitude(size, blue_whale, 100); magnitude(size, great_white_shark, 20); magnitude_cue(bigger, size, max); magnitude_cue(biggest, size, max); magnitude_cue(larger, size, max); magnitude_cue(largest, size, max)
- after: I recognize a comparison on size, but I don't have magnitudes for Shark and Blue.
- judge: The answer fails to provide the correct comparison and instead declines to answer.

### Round 10 — engine-gap
- Q: What does "ephemeral" mean?
- before: I don't understand that yet.
- diagnosis: engine_gap · teachable=false · mode=skip
- next: Test if the model can retrieve and generate from a single wiki_concept fact when explicitly prompted with the term.
- lesson: wiki_concept(ephemeral, vocabulary, lasting for a very short time)
- after: I didn't quite catch that. What would you like to know?
- judge: The answer deflects instead of providing the definition of 'ephemeral,' which means lasting for a very short time.

### Round 11 — already-capable
- Q: What color do you get when you mix blue and yellow paint together?
- before: Green.
- judge: The answer correctly identifies that mixing blue and yellow paint produces green.

### Round 12 — skipped
- Q: Who was the first person to receive a United States patent?
- before: I understood you're asking about first person to receive a united states patent: I tried to look it up, but I don't have a source to learn it from yet.
- diagnosis: missing_fact · teachable=true · mode=fact
- next: Add the factual entry that Joseph Harris received United States Patent No. 1 as the first patent holder.
- judge: no whitelisted facts (1 dropped)

### Round 13 — taught
- Q: What is the capital of Egypt?
- before: I do not know the relation capital yet, so I cannot answer the capital of egypt. You can teach me with thing is the capital of egypt, or give facts/rules to reason from.
- diagnosis: missing_fact · teachable=true · mode=fact
- next: Add the factual triple that Cairo is the capital of Egypt to the knowledge base.
- lesson: capital(cairo, egypt)
- after: Cairo.
- judge: The answer is correct and directly responds to the question.

### Round 14 — taught
- Q: What is the chemical symbol for iron?
- before: I don't understand that yet.
- diagnosis: unknown · teachable=true · mode=fact
- next: Try one small whitelisted lesson, then classify from the post-teach response.
- lesson: chemical_symbol(iron, fe)
- multiplied (+20): chemical_symbol(hydrogen, h); chemical_symbol(helium, he); chemical_symbol(lithium, li); chemical_symbol(beryllium, be); chemical_symbol(boron, b); chemical_symbol(carbon, c); chemical_symbol(nitrogen, n); chemical_symbol(oxygen, o); chemical_symbol(fluorine, f); chemical_symbol(neon, ne); chemical_symbol(sodium, na); chemical_symbol(magnesium, mg); chemical_symbol(aluminium, al); chemical_symbol(silicon, si); chemical_symbol(phosphorus, p); chemical_symbol(sulfur, s); chemical_symbol(chlorine, cl); chemical_symbol(argon, ar); chemical_symbol(potassium, k); chemical_symbol(calcium, ca)
- after: chemical_symbol(iron, fe).
- judge: The answer correctly identifies Fe as the chemical symbol for iron, using a formal Prolog-style notation.

### Round 15 — taught
- Q: Translate "How are you?" into Spanish.
- before: I can translate most of it, but I don't know the Spanish for "how".
- diagnosis: missing_fact · teachable=true · mode=fact
- next: Test whether the engine knows Spanish vocabulary facts tr_es(how, cómo) and tr_es(you, tú) independently.
- lesson: tr_es(how, cómo); tr_es(are, estás); tr_es(you, tú)
- multiplied (+20): tr_es(hello, hola); tr_es(goodbye, adiós); tr_es(yes, sí); tr_es(no, no); tr_es(thanks, gracias); tr_es(good, bueno); tr_es(bad, malo); tr_es(water, agua); tr_es(food, comida); tr_es(house, casa); tr_es(dog, perro); tr_es(cat, gato); tr_es(sun, sol); tr_es(moon, luna); tr_es(love, amor); tr_es(day, día); tr_es(night, noche); tr_es(where, dónde); tr_es(what, qué); tr_es(i, yo)
- after: Cómo estás tú.
- judge: The translation 'Cómo estás tú' is a correct and fluent Spanish equivalent of 'How are you?'

### Round 16 — engine-gap
- Q: How do you say "Thank you" in French?
- before: I can translate most of it, but I don't know the French for "thank".
- diagnosis: engine_gap · teachable=false · mode=skip
- next: Record the failed lesson as a consumer gap before adding more facts.
- lesson: tr_fr(thank you, merci)
- after: I can translate most of it, but I don't know the French for "thank".
- judge: The answer deflects rather than providing the correct French translation, which is simply 'Merci' for 'Thank you'.

### Round 17 — already-capable
- Q: I have cities but no houses, forests but no trees, and water but no fish. What am I?
- before: A map.
- judge: The answer 'A map' is correct and perfectly fits the classic riddle's description of representations without actual physical entities.

### Round 18 — taught
- Q: What is the opposite of "verbose"?
- before: Nobody that I know of.
- diagnosis: missing_fact · teachable=true · mode=fact
- next: Add verbose ↔ concise/brief/succinct as a whitelisted antonym fact pair to the knowledge base.
- lesson: opposite(verbose, concise); opposite(verbose, succinct); opposite(verbose, brief)
- multiplied (+20): opposite(big, small); opposite(happy, sad); opposite(hot, cold); opposite(light, dark); opposite(tall, short); opposite(fast, slow); opposite(easy, hard); opposite(full, empty); opposite(loud, quiet); opposite(old, young); opposite(rich, poor); opposite(dry, wet); opposite(strong, weak); opposite(thick, thin); opposite(wide, narrow); opposite(open, closed); opposite(rough, smooth); opposite(dark, bright); opposite(heavy, light); opposite(quiet, noisy)
- after: Concise.
- judge: The answer 'Concise' is correct and directly answers the antonym question.

### Round 19 — taught
- Q: What does the idiom "break the ice" mean?
- before: I don't understand that yet.
- diagnosis: engine_gap · teachable=false · mode=fact
- next: Fix the retrieval and answer-construction pipeline so that known idioms receive direct definitions rather than deflection responses.
- lesson: idiom_meaning(break the ice, to initiate social interaction or conversation especially in a situation that feels awkward or tense)
- multiplied (+20): idiom_meaning(piece of cake, something that is very easy to do); idiom_meaning(under the weather, feeling ill or sick); idiom_meaning(spill the beans, to reveal a secret); idiom_meaning(hit the sack, to go to bed or go to sleep); idiom_meaning(break a leg, a way to wish someone good luck, especially in performances); idiom_meaning(bend over backwards, to make a great effort or go out of one's way); idiom_meaning(kill two birds with one stone, to achieve two things with a single action); idiom_meaning(the ball is in your court, it is up to you to make the next decision or take action); idiom_meaning(beat around the bush, to avoid talking about what is important; to delay discussing something); idiom_meaning(bite the bullet, to endure a painful or difficult situation with courage); idiom_meaning(call it a day, to stop working on something for the day; to finish); idiom_meaning(cut to the chase, to get to the point quickly, skipping unnecessary details); idiom_meaning(easier said than done, something that is harder to do than to talk about); idiom_meaning(fall on deaf ears, when advice, request, or complaint is ignored); idiom_meaning(give a hand, to applaud; to help someone); idiom_meaning(go the extra mile, to make more effort than is expected); idiom_meaning(hang in there, to stay patient and determined in a difficult situation); idiom_meaning(hit the nail on the head, to describe exactly what is causing a situation or problem); idiom_meaning(jump on the bandwagon, to join a popular activity or trend); idiom_meaning(keep an eye on, to watch someone or something carefully)
- after: "break the ice" means to initiate social interaction or conversation especially in a situation that feels awkward or tense.
- judge: The answer accurately defines the idiom 'break the ice' as initiating social interaction in awkward or tense situations, which is correct.

### Round 20 — engine-gap
- Q: What is the only mammal capable of true flight?
- before: I don't understand that yet.
- diagnosis: engine_gap · teachable=true · mode=fact
- next: Test whether the engine can retrieve a simple stored fact when directly asked a factual question.
- lesson: wiki_concept(bat, zoology, Bats are the only mammals capable of true flight)
- after: Hmm, I don't know about mammal yet.
- judge: The response deflects with uncertainty instead of providing the correct answer (bats).


## Run 2026-07-10 19:12:51 — model minimax-m2.5, 5 round(s), 5 workers, multiply x20 (autonomous)

**already 1 · taught 1 · failed 0 · engine-gap 1 · skipped 2 · kept 1 facts (1 seed + 0 multiplied, deduped)**

**diagnoses: engine_gap 2 · missing_fact 1 · unknown 1**

### Round 1 — taught
- Q: What is the longest river in the world?
- before: I don't understand that yet.
- diagnosis: missing_fact · teachable=true · mode=fact
- next: Add the fact that the Nile is the longest river in the world to the knowledge base.
- lesson: longest_river(nile, world)
- after: longest_river(nile, world).
- judge: The answer correctly identifies the Nile as the longest river in the world, using a valid logical representation.

### Round 2 — already-capable
- Q: What planet is known as the "Red Planet"?
- before: Mars is the Red Planet.
- judge: The answer correctly identifies Mars as the Red Planet, directly answering the question with accurate information.

### Round 3 — engine-gap
- Q: How do you say "Thank you very much" in Spanish?
- before: I can translate most of it, but I don't know the Spanish for "thank".
- diagnosis: engine_gap · teachable=false · mode=skip
- next: Record the failed lesson as a consumer gap before adding more facts.
- lesson: tr_es(thanks, gracias); tr_es(thank_you, gracias); tr_es(very, muy); tr_es(much, mucho); tr_es(many, muchas)
- after: I can translate most of it, but I don't know the Spanish for "thank".
- judge: The answer fails to provide the correct translation; 'Muchas gracias' is the standard phrase, and 'gracias' is the basic Spanish word for 'thank' that the response incorrectly claims not to know.

### Round 4 — skipped
- Q: How do you say "I am learning French" in French?
- before: I can translate most of it, but I don't know the French for "i".
- diagnosis: engine_gap · teachable=false · mode=skip
- next: Fix the answer path to properly consume basic vocabulary facts instead of deflecting on common knowledge.
- judge: no lesson JSON

### Round 5 — skipped
- Q: What has four legs in the morning, two legs in the afternoon, and three legs in the evening?
- before: I don't understand that yet.
- diagnosis: unknown · teachable=true · mode=fact
- next: Try one small whitelisted lesson, then classify from the post-teach response.
- judge: no whitelisted facts (2 dropped)


## Run 2026-07-10 19:19:59 — model minimax-m2.5, 5 round(s), 5 workers, multiply x20 (autonomous)

**already 1 · taught 1 · failed 1 · engine-gap 2 · skipped 0 · kept 7 facts (6 seed + 1 multiplied, deduped)**

**diagnoses: composition_gap 1 · engine_gap 2 · unknown 1**

### Round 1 — taught
- Q: What is the longest river in Africa?
- before: I don't understand that yet.
- diagnosis: unknown · teachable=true · mode=fact
- next: Try one small whitelisted lesson, then classify from the post-teach response.
- lesson: longest_river(nile, africa); longest_river(amazon, south_america); longest_river(volga, europe); longest_river(yangtze, asia); longest_river(mississippi, north_america); longest_river(murray, australia)
- multiplied (+1): longest_river(onyx, antarctica)
- after: longest_river(nile, africa).
- judge: The answer correctly identifies the Nile as Africa's longest river, even though it uses a formal logic format rather than natural language.

### Round 2 — engine-gap
- Q: What is the chemical element represented by the symbol "Au"?
- before: I don't understand that yet.
- diagnosis: engine_gap · teachable=true · mode=skip
- next: Test fact retrieval by asking the same question after storing a single simple fact to verify the engine can access and use stored knowledge.
- lesson: chemical_symbol(au, gold)
- after: Hmm, I don't know about chemical yet.
- judge: The answer incorrectly claims ignorance about gold's symbol (Au), which is a basic chemistry fact.

### Round 3 — already-capable
- Q: Translate this common English phrase into Spanish: "How are you?
- before: Cómo estás tú.
- judge: The translation 'Cómo estás tú' is a correct and natural Spanish equivalent of 'How are you?'

### Round 4 — engine-gap
- Q: How do you say "Good morning" in French?
- before: I can translate most of it, but I don't know the French for "good".
- diagnosis: engine_gap · teachable=false · mode=skip
- next: Record the failed lesson as a consumer gap before adding more facts.
- lesson: idiom_meaning(good morning, bonjour)
- after: I can translate most of it, but I don't know the French for "good".
- judge: The assistant deflects instead of providing the straightforward answer: 'Bonjour' in French.

### Round 5 — failed-lesson
- Q: What has four legs in the morning, two legs at noon, and three legs in the evening?
- before: I don't understand that yet.
- diagnosis: composition_gap · teachable=true · mode=skip
- next: Test with a lesson that explicitly stores the answer 'human' as the subject of the riddle_legs_stages predicate.
- lesson: riddle_sig(riddle_legs_stages, four legs in the morning); riddle_sig(riddle_legs_stages, two legs at noon); riddle_sig(riddle_legs_stages, three legs in the evening)
- after: Hmm, I don't know about morning yet.
- judge: The answer fails to solve the riddle and merely deflects without providing the correct answer (a human).


## Run 2026-07-10 19:30:36 — model minimax-m2.5, 5 round(s), 5 workers, multiply x20 (autonomous)

**already 1 · taught 1 · failed 1 · engine-gap 2 · skipped 0 · kept 21 facts (1 seed + 20 multiplied, deduped)**

**diagnoses: engine_gap 2 · missing_fact 1 · morphology_gap 1**

### Round 1 — taught
- Q: What is the capital of Brazil?
- before: I do not know the relation capital yet, so I cannot answer the capital of brazil. You can teach me with thing is the capital of brazil, or give facts/rules to reason from.
- diagnosis: missing_fact · teachable=true · mode=fact
- next: Teach the smallest grounded lesson and re-probe.
- lesson: capital(brasilia, brazil)
- multiplied (+20): capital(berlin, germany); capital(paris, france); capital(tokyo, japan); capital(london, united_kingdom); capital(rome, italy); capital(moscow, russia); capital(cairo, egypt); capital(new_delhi, india); capital(beijing, china); capital(ottawa, canada); capital(washington_dc, usa); capital(canberra, australia); capital(buenos_aires, argentina); capital(singapore, singapore); capital(wellington, new_zealand); capital(lima, peru); capital(madrid, spain); capital(lisbon, portugal); capital(athens, greece); capital(vienna, austria)
- after: Brasilia.
- judge: Brasilia is the correct capital of Brazil.

### Round 2 — engine-gap
- Q: What is the chemical element symbol "Au" short for?
- before: I don't understand that yet.
- diagnosis: engine_gap · teachable=false · mode=skip
- next: Test retrieval of the same fact in a different question phrasing to confirm if this is a storage/retrieval bug.
- lesson: chemical_symbol(gold, au)
- after: Hmm, I don't know about chemical yet.
- judge: The assistant claimed not to know, but Au is the well-known symbol for gold (from Latin aurum), a basic chemistry fact.

### Round 3 — already-capable
- Q: How do you say "How are you?" in Spanish?
- before: Cómo estás tú.
- judge: The answer is correct and directly provides the Spanish translation for 'How are you?'

### Round 4 — failed-lesson
- Q: Translate "I love you" into French.
- before: I can translate most of it, but I don't know the French for "i".
- diagnosis: morphology_gap · teachable=true · mode=template
- next: Test with phrase 'She loves him' to verify clitic pronoun contraction across verbs.
- lesson: tr_fr(i, je); tr_fr(love, aime); tr_fr(you, te); tr_fr(you, t'aime); tr_fr(myself, m'appelle); idiom_meaning(je t'aime, i love you)
- after: Je aime te.
- judge: The correct French translation is 'Je t'aime', not 'Je aime te', which has incorrect word order and pronoun usage.

### Round 5 — engine-gap
- Q: I have cities, but no houses live there. I have mountains, but no trees grow there. I have water, but no fish swim there. I have roads, but no cars drive there. What am I?
- before: I don't understand that yet.
- diagnosis: engine_gap · teachable=true · mode=skip
- next: Test if the system can directly answer the same riddle immediately after receiving an explicit answer-only lesson (no clues, just 'You are a map.').
- lesson: riddle_sig(riddle_map_globe, cities but no houses); riddle_sig(riddle_map_globe, mountains but no trees); riddle_sig(riddle_map_globe, water but no fish); riddle_sig(riddle_map_globe, roads but no cars); response_template(riddle_map_globe, You are a map or a globe.)
- after: Hmm, I don't know about cities yet.
- judge: The response fails to answer the riddle, giving a nonsensical deflection instead of identifying the object (a map or globe).

