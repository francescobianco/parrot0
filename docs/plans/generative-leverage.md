# Generative Leverage — narrative and continuation (LLMSCORE Q1-Q4)

> **Stato:** piano scritto a gen335+ (2026-07-16), dopo l'analisi LLMSCORE.
> Definisce la strategia per risolvere i 4 fallimenti di scrittura creativa
> e continuazione narrativa, il dominio dove un sistema deterministico (KB +
> regole Horn) è strutturalmente svantaggiato rispetto a un LLM.

## I 4 casi falliti

| # | Prompt | Errore parrot0 | Problema |
|---|--------|---------------|----------|
| Q1 | Tell me a short story about a sentient umbrella that falls in love. | "Rings widen where each drop lands..." — frammento poetico non pertinente | Generazione creativa ex-novo |
| Q2 | [Continuazione] The umbrella—his name was Marcus... | Stesso verso criptico di Q1 | Non riconosce il prompt come continuazione |
| Q3 | [Continuazione] She arrived on a Tuesday... | "Monday." — parola singola dal testo | Pattern-matching su giorno invece che continuazione |
| Q4 | [Continuazione] By Monday, Marcus had memorized... | "I don't understand that yet." | Nessun modulo matcha |

## Perché è difficile per parrot0

parrot0 non genera testo — lo recupera. Il suo motore è SLD con backtracking
su fatti + regole Horn. Non ha un modello linguistico, non campiona token, non
completa sequenze probabilisticamente. Ogni output è deterministico: una risposta
da `response_template`, un fatto dalla KB, o un fallback.

La scrittura creativa richiede **generazione combinatoria** di superficie
linguistica nuova, non recupero. Questo è fuori dal perimetro architetturale.

## La scommessa: "generative leverage"

Invece di generare, parrot0 può **sfruttare il testo fornito dall'utente** per
costruire una continuazione coerente. Il prompt contiene già personaggi, nomi,
ambientazioni, stile. parrot0 non deve inventare — deve **riassemblare**.

La strategia è a tre livelli:

### Livello 1: Template narrativi KB (Q1)

Per prompt ex-novo come "Tell me a short story about X that Y":

```
story_template(sentient_object, "[Name] was a [object] who lived in [place]. One day, [pronoun] discovered that [object]s could [action], and everything changed. From that moment, [name] knew that [pronoun] would never be [adjective] again.").
```

Il template ha slot da riempire con:
- `[Name]` — generato o chiesto all'utente
- `[object]` — estratto dal prompt (umbrella)
- `[place]` — da un set di default KB
- `[action]` — da `can_do/1` KB o default
- `[pronoun]` — accordo di genere
- `[adjective]` — da default o opposto di una proprietà

Il risultato è template-based, non generativo, ma **coerente con il prompt**
e molto meglio del frammento poetico casuale.

### Livello 2: Continuazione per estrazione (Q2, Q4)

Quando il prompt è una continuazione (input lungo, narrativo, senza punto
interrogativo), il sistema:

1. Riconosce il pattern: input > 100 caratteri, contiene virgolette/nomi propri,
   non è una domanda → `intent: story_continuation`
2. Estrae entità nominate (Marcus, Elena, umbrella, Tuesday, Fifth...)
3. Seleziona l'ultima entità o azione menzionata
4. Produce una continuazione template-based che riprende gli elementi estratti

Esempio per Q2:
```
Input: "The umbrella—his name was Marcus, though no one had called him that
in years—opened himself slowly as the first clouds gathered..."

Output: "Marcus felt the familiar weight of the coming rain. He had waited
so long for this moment — for someone who would notice him, not as an
object but as a presence."
```

Il template usa `[Name]` e `[object]` estratti, più un `response_template`
per `story_continuation` che varia a ogni chiamata (anti-repeat instinct).

### Livello 3: Anti-pattern per Q3 (false match su giorni)

Q3 è un bug specifico: il modulo `sequence` o `continuation` matcha "Tuesday"
e risponde "Monday." — una sequenza giorni. Il fix è:

1. Nel modulo `sequence`, aggiungere un guard: se l'input contiene più di N
   parole e la parola matchata è un giorno della settimana, **non** claimare
   (la probabilità che sia una richiesta di sequenza giorni in un prompt
   narrativo è zero).
2. Alternativa: abbassare la priorità del modulo `sequence` nel registry.

Questo è un fix puntuale, non architetturale.

## Piano di implementazione

### Step 1 — Fix Q3 (il più facile, massimo ritorno)

- In `mod_sequence` (o il modulo `continuation`), aggiungere guardia:
  se `nw > 8` e la parola matchata è un giorno/mese, non claimare.
- File: `src/brain/20-math.c` o `src/brain/30-generation-reading.c`
- Test: Q3 deve restituire una continuazione, non "Monday."

### Step 2 — Story continuation module (Q2, Q4)

- Nuovo modulo `mod_story` in `src/brain/`:
  - Gate: input > 100 chars, no `?`, contiene nomi propri o virgolette
  - Estrazione: `split_words`, identifica capitalized words come entità
  - Output: `response_template(story_continuation, ...)` con slot filling
- KB: `kb/core/responses.p0` — nuovi `response_template` per narrativa
- Intenti: `intent_cue(story_continuation, ...)` per riconoscere prompt narrativi

### Step 3 — Story generation from scratch (Q1)

- KB: `kb/core/templates/story.p0` — template narrativi con slot
- Riconoscimento: `intent_phrase("tell me a story about", story_request)`
- Modulo: estende `mod_story` o nuovo `mod_storygen`
- Riempimento slot: entità dal prompt, proprietà dalla KB, default creativi

### Step 4 — KB-first compliance

Tutti i template, le frasi di continuazione, e i cue di riconoscimento vivono
nella KB come:
```
response_template(story_continuation, "[Name] felt the familiar weight...")
intent_cue(story_continuation, keyword(umbrella))
story_template(sentient_object, "[Name] was a [object] who...")
```

Il C fornisce solo il motore di slot-filling (meccanica fissa). La superficie
linguistica è conoscenza.

## Misurazione

Dopo ogni step, rieseguire LLMSCORE. Il target:
- Step 1: Q3 passa (score +1)
- Step 2: Q2, Q4 passano (score +2)
- Step 3: Q1 passa (score +1)
- Target finale: 8/10 (dal 4/10 attuale)

I restanti 2 punti (Q5 trade, Q6 titanic) sono coperti separatamente da
questo stesso ciclo di lavoro.

## Onestà

La scrittura creativa di parrot0 non sarà mai indistinguibile da un LLM.
Il punto non è fingere — è dimostrare che un sistema deterministico,
KB-first, può produrre输出 testuale coerente sfruttando il testo fornito
dall'utente come àncora. Non è generazione: è **reassembly**.
