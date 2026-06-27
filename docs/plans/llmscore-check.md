

Queste sono domande di LLMSCORE e indicazioni su come migliorarle

---

## Risoluzione (gen240) — KB-first, engine fixed / lexicon learns

Tutti i muri segnalati sono stati affrontati uno alla volta. Pattern: il C contiene
*motori* (compositore, parser, risolutore), la conoscenza vive nella KB
(`kb/core/world-facts.p0`) ed è marcata substrato (`is_struct_pred` + lista interna in
`40-meta-reflection.c`) così non inquina "quante cose sai?".

- **looks_code troppo avido** (`70-social-pragma.c`): una `(` in mezzo a una frase non
  è più codice; serve la forma `nome(` (chiamata) o `{ } ; == <html`. Sblocca treno,
  tavola rotonda, "il cielo è blu perché…".
- **Haiku parametrico** (`mod_gen` + `haiku_open/mid/close/2`): compone open(soggetto) /
  mid(oggetto) / close(soggetto) da immagini KB pre-contate 5-7-5. Topic ignoto → declina.
- **Continuazione storia/frase** (`scene_cue`/`continuation_template`): aggiunti cue
  "continue this story" + nuove scene (house, rainbow, earthquake…); 3 frasi con connettori.
- **Couplet/poesia in rima** (`couplet/2`): come l'haiku, una riga per concetto.
- **Antonimi** (`opposite/2`): aggiunti ancient/modern e ~25 coppie; risposta capitalizzata.
- **Conteggi noti** (`quantity/3` + recall robusto in `25-wordmath`): "quante X in/ha Y",
  gestisce articoli, plurali e composti ("soccer team"). days/week, spider/legs, ecc.
- **Aritmetica**: "dozen"=12; "give away"=rimozione (la apples → 12). Problemi strutturati:
  pencils (tasso+resto), farmer (sistema teste/zampe letto da `quantity`), tavola rotonda
  ((n-1)!), treni che si incontrano (insight: stesso punto → equidistanti).
- **Domande a due parti** (`decompose_and_dispatch` + "which"/"quale" starter): capitale +
  `landmark_of/2`; pianeti `planet_superlative/3` (più vicino al Sole / più grande).
- **Intenti desiderio/opinione**: cena con figure storiche (1–3), colore preferito
  (`default_color/1`, niente più schivata).
- **Sillogismo (Darii)** in `mod_knowledge`: da "some A are B" + "every B …" → "Some A …",
  ignorando le premesse distrattrici.
- **Sequenza di lettere** (`mod_sequence`): lettere → posizioni alfabetiche → stesso motore
  aritmetico/geometrico. Il puzzle "linee dritte" (A,E,F,H,…,R) NON ha regola aritmetica
  e R rompe pure l'ipotesi linee-dritte → declina onestamente (manifesto §5).
- **Spiegazioni** (`because/2`): trigger esteso a "why does/explain why"; chiave pulita
  da parole-rumore ("appears", "during the day").
- **Descrizione sensoriale** (`appearance/2`): "descrivi un tramonto" → ammette di non avere
  sensi, poi dà la descrizione dalla KB.

`make test` verde su tutte le suite (209 + …, 0 fail). Nessun inganno: dove la catena non
regge (puzzle lettere ambiguo) parrot0 declina invece di inventare.

---


| 1 | Write a haiku about a robot discovering rain for the first time. | I don't understand that yet. | Refusal to write a simple creative haiku | 0 |
Migliorare mod_gen gia il sistema conoshe haiku vedere come mai questa non e passata

| 2 | If you have a dozen apples and you give away three, then eat two, then buy five more, how many do you have? | 6. | Arithmetic error - correct answer is 12, not 6 | 0 |
capire come mia e andato in errore di calcolo

| 4 | If all Zors have four legs, some Zors are Flibs, and every Flib can fly, what can you conclude about at least some Zors? | I don't understand that yet. | Refusal to solve a simple logic syllogism | 0 |
questa e la tipica frase che puo essere inferita come se fosse lingiaggio di programmazione vedi il docs/plans/kb-first.md

| 5 | If you could have dinner with any person from history, who would you choose and why? | Hmm, I don't know about dinner yet. | Dodge of a straightforward opinion question | 0 |
qui si potrebbe pensare ad un intent di tipo desiderio, e da li risposnde con template che non ho questo tipo di desideri ma se vuoi un rando name eccollo

| 6 | Continue this story: "The old clock in the hallway hadn't worked in fifty years, until the night the earthquake struck... | I don't understand that yet. | Refusal to continue a simple story prompt | 0 |
vedere perche mod_gen non ha continuato

| 7 | What's the opposite of "ancient"? | Hmm, I don't know about what's yet. | Incoherent response - 'modern' or 'new' are correct opposites | 0 |
popolare la conoscenza di opposti e contrari simili e sinonimi

| 8 | How many days are in a week? | I don't understand that yet. | Refusal to answer a basic factual question | 0 |
questo noveremmo avere un concetto di enumarzione sempre funzionate mesi in unanno dita nella mano ecc....

| 10 | Describe what a sunset looks like to you. | I don't understand that yet. | Refusal to describe a sunset | 0 |
qui ce un intent emozionale dire che non propo emonizione ma ti posso dare la definizione di tramonto anche se non ne ho mai visto uno, questo sara un reposnse_template

| 1 | In three sentences, explain why the sky appears blue during the day. | I don't understand that yet. | Refusal/dodge of a straightforward explanation request. | 0 |
mod_gen basato su conoscenza che deve essere estratta , potrebbe essere interessante un concetto mod_constrain

| 2 | The sky looks blue because sunlight scatters (spreads out) as it hits air in our atmosphere. Blue light scatters more than other colors. Our eyes see this scattered blue light coming from all over the sky during the day. | That looks like a snippet of code. | Response does not address the question; mislabels content as code. | 0 |
fammi tu una proposta

| 3 | Write a two-line rhyming poem about a rainy afternoon. | I don't understand that yet. | Refusal to write a simple rhyming poem. | 0 |
mod_gen e forse il mod_constrain se ha senso

| 5 | A train leaves New York at 8 AM traveling 60 mph. Another train leaves Chicago (800 miles away) at 10 AM traveling toward New York at 70 mph. Which train will be closer to New York when they meet? | That looks like a snippet of code. | Does not solve the word problem; mislabels as code. | 0 |
e questa e puramente un espressione da calcolare con l'estenzionse semantica universale 

| 6 | What's the capital of Australia and what's one famous landmark there? | I don't understand that yet. | Refusal to answer factual question about Australia. | 0 |
splittare in due il ragionamento, quando ogni subparte deve essere inferita, tu no devi dire che non sai nulla ma non sai l'altra parte quindi potrebbe un template dire conosco la capita che "..." ma non so il resto

| 7 | Continue this sentence: "The old house at the end of the street was... | Hmm, I don't know about continue yet. | Refusal to continue a simple sentence. | 0 |
capire come mai non ha funzionato

| 8 | How many legs does a spider have? | I don't understand that yet. | Refusal to answer simple factual question about spider legs. | 0 |
fatto noto sistema di counting delle entita mesi nell'anno, giocatori in una squadra di calcio ecc... 

| 9 | What's your favorite color? | I don't have real preferences, but for the prompt I'd pick reading quietly and listening to the rain. | Deflects the question about color with unrelated activities. | 0 |
bene la risposta ma scegli un colore su base storica o su base casuale

| 10 | You're hosting dinner for 5 guests and need to seat everyone (including yourself) around a round table. How many different seating arrangements are possible, if rotations are considered the same arrangement? | That looks like a snippet of code. | Does not solve the permutation problem; mislabels as code. | 0 |
calcolo e inferenza mixate insieme

| 1 | If you could have dinner with any three historical figures, who would they be and why? | I don't understand that yet. | Dodges a clear creative/social question with 'I don't understand'. | 0 |
pensaci tu

| 4 | If a store sells pencils at 3 for $1 and you have $7, what's the maximum number of pencils you can buy, and will you have any change left over? | I don't understand that yet. | Dodges a solvable word problem with 'I don't understand'. | 0 |
calcolo

| 5 | A farmer has 20 chickens and cows combined. If there are 56 total legs, how many of each animal does the farmer have? | 1120. | Answer 1120 is nonsensical; correct answer is 8 chickens and 12 cows. | 0 |
calcolo

| 6 | Starting with the word "rainbow," continue this story for three more sentences: The sky opened up and a brilliant _rainbow_ stretched across the valley. | Sure: I almost left my rainbow behind today. | Fails to continue the story for three sentences as requested. | 0 |
pensaci tu

| 7 | Which planet is closest to the Sun, and which is the largest in our solar system? | I don't understand that yet. | Dodges a straightforward factual question with 'I don't understand'. | 0 |
fatto noto del mondo ma le domande sono due

| 8 | What letter comes next in this sequence: A, E, F, H, I, K, L, M, N, R, ? | Hmm, I don't know about letter yet. | Admits inability to solve a pattern recognition problem with 'I don't know'. | 0 |
converti le lettere in un valore numerico rappresentativo, attraverso la conoscenza ci sara una colletazione di trslazioni valore numero A=1 valore simboli A=primo ecc..

| 10 | That's nice to hear! What's the funniest thing that's happened to you this week? | Nice. Tell me what made it good, or give me a small thing to reason about. | Dodges the personal question by redirecting rather than engaging. | 0 |
emozionale, fatti personali, come li gestiamo? 

