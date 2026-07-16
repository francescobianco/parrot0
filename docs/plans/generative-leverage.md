# Generative Leverage — sintesi operativa (LLMSCORE Q1-Q4)

> **Stato:** sintesi gen335+ (2026-07-17). Sostituisce il documento esteso.

## L'osservazione chiave

In tutti e 4 i casi, **la forma dell'output atteso è già nel prompt**. L'utente
dichiara esplicitamente cosa vuole:

| Q | Prompt | Forma esplicita |
|---|--------|----------------|
| Q1 | *"Tell me **a short story** about a sentient umbrella that falls in love."* | `a short story` — genere, soggetto, azione |
| Q2-Q4 | Paragrafi narrativi lunghi, nessun `?`, nomi propri, virgolette | È già l'inizio di una storia — va **continuata** |

## Il modello: generazione combinatoria per inferenza vincolata

parrot0 **non** "recupera" passivamente. Data una domanda aperta con vincoli
espliciti, l'inferenza SLD seleziona **una possibilità tra molte coerenti**
in uno spazio combinatorio definito dalla struttura della KB.

Non è "zero generazione". È **generazione per inferenza**: il sistema risolve
un predicato parzialmente istanziato (es. `haiku(ocean, $Open, $Mid, $Close)`)
e trova tutte le tuple valide. Se ci sono 3 open × 3 mid × 3 close = 27
combinazioni, l'output è **una** di esse — selezionata dall'ordine dei fatti
o ruotata dall'anti-repeat instinct. Il testo emesso non è mai stato scritto
in quella forma esatta: è un **assemblaggio combinatorio nuovo**.

Questo modello garantisce per costruzione:
- **Grammatica**: le linee sono pre-composte, quindi sintatticamente corrette.
- **Coerenza semantica**: i fatti KB sono etichettati per concetto (`ocean`,
  `rain`), quindi le linee condividono il tema.
- **Uso appropriato di colore e conoscenza strutturata**: ogni linea è scelta
  da un autore (umano o insegnamento runtime) che conosce il dominio; la KB
  non mescola registri o stili incompatibili perché i fatti sono organizzati
  per predicato e concetto.

### Caso studio: il compositore haiku

> **File:** `kb/core/templates/case_study_haiku.p0`

"Write a haiku about the ocean." → 3×3×3 = 27 haiku validi. L'engine C
(meccanica fissa) estrae il concetto, interroga `haiku_open/2`,
`haiku_mid/2`, `haiku_close/2`, e assembla. Ogni haiku è grammaticalmente
corretto (5-7-5 pre-composto), semanticamente coerente (stesso tema), e
combinatorio (nessuna delle 27 varianti è pre-scritta come intero).

Aggiungere il tema "forest" richiede 9 fatti KB. Zero C. L'engine non cambia.

## Due pattern da implementare

### Pattern A — Richiesta esplicita di storia (Q1)

**Riconoscimento:** `"tell me a short story"`, `"write a story"`, `"tell me a tale"`
→ `intent: story_request`

**Inferenza:** `story_request($Subject, $Action, $Genre, $Output)` dove Subject
e Action sono estratti dal prompt, Genre è `short_story`. La KB ha template
etichettati per combinazioni (soggetto, azione, genere). Il motore seleziona
il primo template compatibile e riempie gli slot.

**Template KB (esempio):**
```
story_template(sentient_object, love, "[Subject] was a [adjective] [object]. One day, [subject] discovered what it meant to [action]. [Pronoun] had never felt this way before — as if the whole world had shifted. And so [subject] waited, patient as only [object]s can be, for the one who would see [pronoun] not as a thing, but as a presence.")
```

### Pattern B — Continuazione implicita (Q2, Q3, Q4)

**Riconoscimento:** input > 120 caratteri, nessun `?`, contiene nomi propri
→ `intent: story_continuation`

**Inferenza:** come il compositore haiku, ma con slot estratti dall'ultima
parte del prompt (nome, verbo, oggetto). Template multipli per lo stesso
intent → rotazione anti-repeat.

**Fix Q3:** guardia in `mod_sequence`: se `nw > 8` e la parola matchata è
un giorno/mese, non claimare.

## Architettura

Un modulo solo: `mod_story` in `src/brain/`. Due gate:

```
Gate A: cue(norm, "tell me a story") ∨ cue(norm, "write a story") → story_request
Gate B: nw > 15 ∧ no "?" ∧ has_proper_nouns(norm) → story_continuation
```

Il C è il motore di slot-filling (meccanica fissa). La superficie linguistica
(template, varianti, soggetti, azioni) vive nella KB. Il C non contiene una
sola frase in linguaggio naturale.

## Implementazione

| Step | Cosa | File | Impatto |
|------|------|------|---------|
| 0 | Fix Q3: guardia `mod_sequence` per giorni in input lunghi | `src/brain/20-math.c` | Q3 |
| 1 | Nuovo modulo `mod_story` con pattern A e B | `src/brain/` nuovo file | Q1, Q2, Q4 |
| 2 | Template KB per storie e continuazioni | `kb/core/templates/story.p0` | superficie |
| 3 | Intenti KB: `story_request`, `story_continuation` | `kb/core/intents.p0` | riconoscimento |
| 4 | Registra `mod_story` prima di `mod_answer_frame` | `src/brain/99-registry.c` | dispatch |

## Target

Da 6/10 → 8/10 (Q1 e Q3 recuperabili; Q2 e Q4 dipendono dalla qualità dei template).
I 2 punti restanti sono il confine onesto del metodo deterministico.
