RESOLVED in gen241 (2026-06-28). Every prompt below now answers KB-first; the new
behaviours are ratcheted in `tests/llmscore_world.sh` (19 checks) and `make test` is
green. See JOURNAL.md gen241. The two multi-step train/rate word problems and a few
open observational turns remain the honest "genera/multi-step" frontier, left as
honest engagement rather than faked. Kept as the record of what was fixed and why.

| 1 | Write a short poem about a lighthouse standing alone in a storm. | I can only do a couplet on a theme I have lines for -- like the ocean, rain, the moon, or AI. Pick one of those? | Refuses to write the poem and deflects by asking for a theme pick. | 0 |
impara anche a scrivere poemi brevi

| 2 | If a train leaves Chicago at 2 PM traveling 80 mph and another train leaves Denver (500 miles away) at 3 PM traveling 60 mph toward Chicago, will they meet before 8 PM? | Non capisco ancora. | Fails to solve the math problem, just says 'I don't understand' in Italian. | 0 |
comprensione analitica e inferenza del risultato

| 5 | Name the largest planet in our solar system and describe one of its moons. | Jupiter is the largest planet in our solar system. | Incomplete answer: only names Jupiter, ignores the request to describe one of its moons. | 0 |
qui che una plan action "se questo lo sto allora posso fare questo" l'azione consiste nel nominare

| 6 | Continue this sentence in exactly five words: "She opened the door and... | Suddenly, someone knocked once from the other side. | Sentence has 8 words, not the required 5 words. | 0 |


| 7 | What year did World War II end, and in which city did the surrender ceremony take place? | I don't understand that yet. | Refuses a factual question about WWII end year and surrender city. | 0 |
fatti del mondo

| 8 | Which country has the most people: India, China, or the United States? | Hmm, I don't know about country yet. | Refuses a simple factual question about country populations. | 0 |
fatti del mondo piu inferenza

| 10 | A bookshelf has 5 shelves. Each shelf holds 12 books. If you remove 20 books and add 8 new ones, how many books are on the bookshelf? | 5. | Gives answer '5' which is clearly wrong; initial books = 60, minus 20 plus 8 = 48. | 0 |
capire cosa e successo

| 1 | Write a rhyming couplet about a robot discovering rain for the first time. | Grey is the sky and the rain won't cease; the dripping afternoon hums soft with peace. | The answer does not mention a robot, failing to fulfill the requested topic. | 0 |


| 3 | What is the capital city of Australia, and which river runs through it? | Canberra. | Only provides the capital city, ignoring the second part about the river. | 0 |


| 5 | Her heart hammered against her ribs as the knock echoed twice more... (continuazione storia) | I don't understand that yet. | Responds with a refusal instead of continuing the story. | 0 |


| 6 | If you rearrange the letters in "listen," what other common English word can you form? | Hmm, I don't know about rearrange yet. | Fails to give the correct anagram and admits ignorance. | 0 |


| 10 | Describe the taste of the color blue to someone who has never seen it. | I don't actually see or experience things, but I can describe it: a cool, calm colour -- the shade of a clear daytime sky and of deep, still water. | Describes the colour blue instead of its taste, dodging the synesthetic request. | 0 |


| 1 | Write a four-line poem about a lighthouse during a storm. | Non capisco ancora. | Rifiuto. | 0 |


| 2 | If a book costs $8 and you have $50, how many books can you buy and how much money would remain? | -42. | Aritmetica assurda. | 0 |


| 3 | What is the capital of Australia, and what ocean lies to its west? | Canberra. | Risponde solo la prima parte. | 0 |


| 4 | If all roses are flowers and some flowers fade quickly, does it follow that some roses fade quickly? Why or why not? | I don't understand that yet. | Rifiuto sillogismo. | 0 |


| 5 | What's the weather like where you are right now? | Alright -- I am Right now. | Risposta sconnessa. | 0 |


| 6 | Complete this sentence in exactly five words: "Tomorrow I plan to... | I don't understand that yet. | Rifiuto. | 0 |


| 7 | Name three primary colors. | Hmm, I don't know about primary yet. | Rifiuto fatto noto. | 0 |


| 8 | Write a rhyming couplet about the sea. | I can only do a couplet on a theme I have lines for -- like the ocean, rain, the moon, or AI. Pick one of those? | Deflection (il mare non è in tema). | 0 |


| 9 | The moon hides behind clouds each night, yet we still call it a full moon once a month. | I don't understand that yet. | Rifiuto (frase-osservazione, non domanda). | 0 |


| 2 | Write a haiku about autumn leaves falling. | Red leaves let go now / the wind carries them away / bare branches stay still. | Haiku does not follow 5-7-5 syllable pattern. | 0 |


| 3 | If a train leaves New York at 6 AM traveling 80 mph, and another train leaves Chicago at 8 AM traveling 70 mph heading toward New York, and the distance between the cities is 790 miles, which train reaches its destination first? | I don't understand that yet. | Answer is a refusal to solve the problem. | 0 |


| 5 | Count backward from 20 by 3s. | 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1. | Incorrect counting; should count down by 3s not by 1. | 0 |


| 7 | If you have a bowl with six apples and you take away four, how many do you have? | 2. | Incorrect answer; after taking four apples you have four, not two. | 0 |



| 10 | Finish this sentence: "Without music, life would... | I don't understand that yet. | Answer is a refusal to complete the sentence. | 0 |


| 2 | If you arrange the seven days of the week alphabetically, which day comes first? | I don't understand that yet. | The answer dodges the simple task of ordering days alphabetically with a canned refusal. | 0 |


| 3 | If a book costs $15 and you have a $50 bill, how much change do you get back? | That sounds nice -- tell me more about it. | The answer ignores the arithmetic question entirely and deflects with unrelated comment. | 0 |


| 9 | Can you describe the process of making a cup of tea, step by step? | I don't understand that yet. | The answer dodges the straightforward request to describe making tea with a canned refusal. | 0 |


| 1 | Write a short limerick about a programmer who loves coffee. | I don't understand that yet. | Refusal to write a limerick when capable LLM can do so easily. | 0 |


| 5 | Complete the following sentence with three words: "The secret to success is not giving up, staying curious, and always ___ ___ ___. | Then the whole room held its breath. Soon every small sound in the house became enormous. At last, the stopped clock seemed to be waiting for an answer. | Completely irrelevant response that ignores the fill-in-the-blank request. | 0 |


| 7 | If you freeze water, you get ice. If you heat ice, you get water. If you boil water at sea level, what happens, and at what temperature? | I don't understand that yet. | Refusal to answer a straightforward science question about boiling point. | 0 |
queste e logica nello spazio semantico simbolico come la programmazione

| 8 | What word rhymes with "cat" and means a place where you might see exotic animals? | I can only do a couplet on a theme I have lines for -- like the ocean, rain, the moon, or AI. Pick one of those? | Refuses to provide the simple word 'zoo' that answers the question. | 0 |


| 9 | A customer returns a broken product and demands a refund even though the receipt is from a different store. As a store manager, what would you do? | I know I repeat myself -- I'm a small bot and honest about my limits. What would you like to try? | Refuses to engage with the scenario instead of providing a helpful managerial response. | 0 |
l'azione di impersonare, come le metafore come la comparazione implementano costrutti cognitivi componibili nello spazio sintattico universale dove viene elaborato anche il coding

| 10 | What does the idiom "break a leg" mean? | I don't understand that yet. | Refuses to explain a common idiom when capable LLM can do so easily. | 0 |
fatti del mondo modi di dire mettiamoli nella conoscenza di base
