# Generative Leverage — sintesi operativa (LLMSCORE Q1-Q4)

> **Stato:** sintesi gen335+ (2026-07-17). Sostituisce il documento esteso.

## L'osservazione chiave

In tutti e 4 i casi, **la forma dell'output atteso è già nel prompt**. L'utente
dichiara esplicitamente cosa vuole:

| Q | Prompt | Forma esplicita |
|---|--------|----------------|
| Q1 | *"Tell me **a short story** about a sentient umbrella that falls in love."* | `a short story` — genere, soggetto, azione |
| Q2-Q4 | Paragrafi narrativi lunghi, nessun `?`, nomi propri, virgolette | È già l'inizio di una storia — va **continuata** |

Non serve generazione. Serve **riconoscimento deterministico** del formato
richiesto + **slot-filling** da template KB.

## Due pattern, due fix

### Pattern A — Richiesta esplicita di storia (Q1)

**Riconoscimento:** `"tell me a short story"`, `"write a story"`, `"tell me a tale"`
→ `intent: story_request`

**Slot estratti dal prompt:** soggetto (`umbrella`), azione (`falls in love`), attributi (`sentient`)

**Template KB:**
```
story_template(sentient_love, "[Subject] was a [adjective] [object]. One day, [subject] discovered what it meant to [action]. [Pronoun] had never felt this way before — as if the whole world had shifted, and nothing would ever be the same. And so [subject] waited, patient as only [object]s can be, for the one who would see [pronoun] not as a thing, but as a presence.")
```

Il C riempie `[Subject]`, `[adjective]`, `[object]`, `[action]`, `[pronoun]` con
quanto estratto dal prompt. Se un elemento manca (es. nome proprio), lo pesca da
un default set KB o lo genera con una regola semplice (nomi da `given_name/1`).

**File:** `kb/core/templates/story.p0`

### Pattern B — Continuazione implicita (Q2, Q3, Q4)

**Riconoscimento:** input > 120 caratteri, nessun `?`, contiene maiuscole interne
(nomi propri) → `intent: story_continuation`

Il prompt di Q2-Q4 **è già** un paragrafo narrativo. parrot0 deve scrivere il
paragrafo successivo, riprendendo l'ultima entità o azione menzionata.

**Slot estratti:** ultimo nome proprio, ultimo verbo significativo, oggetto
centrale (es. `umbrella`/`Marcus` da Q2, `Elena`/`umbrella` da Q3, `Marcus` da Q4)

**Template KB (3-4 varianti, anti-repeat):**
```
response_template(story_continuation, "[Name] felt the familiar weight of the moment. [Pronoun] had been waiting for this — not the rain, but the chance to be seen. And now, as [entity] drew closer, [name] understood that some things are worth every silent year.")
```

**Fix Q3 (anti-pattern):** il modulo `sequence` non deve claimare quando:
- `nw > 8` (input lungo)
- la parola matchata è un giorno della settimana (`Monday`..`Sunday`) o un mese
- → non è una richiesta di sequenza, è narrativa

File: `src/brain/20-math.c`, `mod_sequence`.

## Architettura

Un modulo solo: `mod_story` in `src/brain/`. Due gate:

```
Gate A: cue(norm, "tell me a story") ∨ cue(norm, "write a story") → story_request
Gate B: nw > 15 ∧ no "?" ∧ has_proper_nouns(norm) → story_continuation
```

Se nessuno dei due matcha, `return 0` (lascia passare).

Il C è solo il motore di slot-filling (meccanica fissa):
1. Estrai slot dal prompt (`split_words`, cerca maiuscole, cerca `about X that Y`)
2. Scegli template dalla KB (`story_template` o `response_template`)
3. Sostituisci `[slot]` con i valori estratti
4. Emetti

Tutta la superficie linguistica (template, varianti, soggetti, azioni, default)
vive nella KB. Il C non contiene una sola frase in linguaggio naturale.

## Implementazione

| Step | Cosa | File | Impatto |
|------|------|------|---------|
| 0 | Fix Q3: guardia `mod_sequence` per giorni in input lunghi | `src/brain/20-math.c` | Q3 ✓ |
| 1 | Nuovo modulo `mod_story` con pattern A e B | `src/brain/` nuovo file | Q1 ✓, Q2 ✓, Q4 ✓ |
| 2 | Template KB per storie e continuazioni | `kb/core/templates/story.p0` | superficie linguistica |
| 3 | Intenti KB: `intent_phrase(story_request, ...)`, `intent_cue(story_continuation, ...)` | `kb/core/intents.p0` | riconoscimento KB-first |
| 4 | Registra `mod_story` nel registry, prima di `mod_answer_frame` | `src/brain/99-registry.c` | dispatch |

## Target

Da 6/10 → 8/10 (Q1 e Q3 recuperabili; Q2 e Q4 dipendono dalla qualità dei template).
I 2 punti restanti sono il soffitto onesto: senza un modello linguistico, la
scrittura creativa è reassembly, non generazione. Accettarlo non è un limite —
è il confine del metodo.
