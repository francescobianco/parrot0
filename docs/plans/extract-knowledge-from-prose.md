# Extract Knowledge from Prose

> **Stato:** aperto a gen335 (2026-07-14). Il sistema sa già estrarre fatti da
> frasi copulari semplici e locative, ma solo attraverso frame cablati in C (M0).
> Il prossimo salto è rendere i frame di estrazione **KB-first** — una tabella
> `extract_frame(Pattern, Predicate)` che il motore interroga, così un nuovo
> pattern di estrazione è un fatto, non una riga di C.
> **Subordinato a** [[kb-first]] (la bussola), [[universal-comprehension]]
> (comprendere la forma), [[universal-input]] (la prosa è un registro tra altri),
> [[deep-reasoning]] (estrazione ampia + auto-correzione).
> **Disciplina:** un obiettivo per generazione, pull da counterexample reale,
> niente fabbricazione.

---

## 0. La tesi

> **La prosa non è "roba da LLM". Sotto la superficie di ogni frase ben formata
> c'è uno scheletro logico estraibile — soggetto, predicato, oggetto, relazione.
> parrot0 già lo estrae per le frasi copulari semplici. Il salto è renderlo
> generico e KB-first: i frame di estrazione diventano conoscenza, non codice.**

Il corollario operativo:

> **Aggiungere un pattern di estrazione deve costare un fatto KB, non una
> generazione di C. Il motore di estrazione è fisso; i pattern crescono.**

---

## 1. Cosa esiste già (lo stato attuale)

### 1.1 Il parser di comprensione riusato per l'estrazione

Il principio architetturale è: **lo stesso parser che capisce l'input interattivo
(`mod_knowledge`) estrae fatti dalla prosa.** Ciò che parrot0 sa *comprendere*,
lo sa *estrarre* (`deep-reasoning.md` §3.4).

Il flusso attuale:

```
prosa grezza
  → split in frasi (su '.')
  → normalize + canonicalize_lang (IT→EN)
  → extract_class_statement()  ← il parser di comprensione in modalità extract
  → kb_assert + fact_source/3  ← fatto + provenienza
```

**Entry point:** `read_passage()` in `src/brain/30-generation-reading.c:861`
**Core extractor:** `extract_class_statement()` in `src/brain/10-memory-knowledge.c:2156`
**Page extractor:** `extract_page_facts()` in `src/brain/50-self-research-loop.c:266`

### 1.2 I frame M0 — ciò che il sistema sa estrarre oggi

Tutti cablati in C (`src/brain/10-memory-knowledge.c`), chiusi a gen296-300:

| Frame | Pattern | Esempio | Linea |
|---|---|---|---|
| **F1** — articolo iniziale | `A/An/The <X> is a <Y>` | "A whale is a mammal" → `mammal(whale)` | 5820 |
| **F2** — copula passata | `X was/were a Y` | "Paris was a city" → `city(paris)` | 5153 |
| **F3** — soggetto multi-parola | `<X Y> is a <Z>` | "The blue whale is a marine mammal" → `marine_mammal(blue_whale)` | 2124 |
| **F4** — PP in coda | `X is a Y in Z` | "France is a country in Europe" → `country(france)` + `located_in(france, europe)` | 2124 |
| **F5** — congiunzione di classi | `X is a Y and a Z` | "A dog is a mammal and a pet" → `mammal(dog)` + `pet(dog)` | 5845 |
| **F6** — locativo diretto | `X is located in Y` / `X is part of Y` | → `located_in(X, Y)` / `part_of(X, Y)` | 2189 |

**Limiti dei frame M0:**
- Sono cablati in C: aggiungere "X borders Y" richiede una riga di C
- Coprono solo la copula `is`/`are` + locativi semplici
- Non coprono verbi transitivi ("X eats Y"), possesso ("X has Y"), attributi ("X weighs Y kg")
- Non esiste una tabella `extract_frame` nella KB

### 1.3 Provenienza e auto-correzione (M1, M4)

Ogni fatto estratto conserva la **fonte grezza**:

```prolog
fact_source("located_in(france, europe)", france, "France is a country in western Europe").
```

Questo abilita il loop di auto-correzione di `deep-reasoning.md` §4bis: se
l'inferenza produce una contraddizione, il sistema **torna alla fonte**, ri-analizza
il frammento sospetto, e corregge o ritratta il fatto.

### 1.4 Il flusso completo (dalla prosa al fatto)

```
TESTO GREZZO
    │
    ├── read_passage(buf, &learned, &skipped)
    │       │
    │       ├── split in frasi su '.'  (src/brain/30-generation-reading.c:861)
    │       │
    │       ├── per ogni frase:
    │       │       normalize() → canonicalize_lang(b, ...)
    │       │       extract_clause(b, clause)
    │       │           ├── mod_quantity()   → quantity/4
    │       │           ├── mod_cause()      → because/2
    │       │           ├── mod_same()       → same/2
    │       │           └── mod_knowledge()  → extract_class_statement()
    │       │               ├── F1-F6 frame → kb_assert + fact_source
    │       │               └── "Learned: ..." or skipped
    │       │
    │       └── store_proposition(b, clause)  ← ricorda la frase originale
    │
    └── trigger: "read the page on X" / chat prefix "read:"
```

---

## 2. Cosa manca (i gap)

### 2.1 Frame di estrazione KB-first

Il gap principale: i frame F1-F6 sono `if` in C, non fatti in KB. Il piano
`deep-reasoning.md` §4.5 lo riconosce:

> "Comprehension frames are NOT (only) hardcoded C: they are **knowledge**. An
> extraction frame must be a rule/table in the KB, *teachable at runtime*."

La forma bersaglio:

```prolog
% extract_frame(Pattern, Predicate, ArgSlots)
% Pattern: struttura superficiale della frase
% Predicate: predicato KB da asserire
% ArgSlots: quali token della frase vanno in quali posizioni degli argomenti

extract_frame("$X is a $Y",            is_a,        [subject, class]).
extract_frame("$X is located in $Y",   located_in,  [subject, location]).
extract_frame("$X eats $Y",            eats,        [subject, food]).
extract_frame("$X has $N $Y",          quantity,    [subject, unit, number]).
extract_frame("$X borders $Y",         borders,     [subject, neighbor]).
```

Il motore C legge `extract_frame/3` dalla KB, matcha il pattern contro la frase
canonica, estrae i token negli slot, e asserisce il predicato. **Zero C per ogni
nuovo pattern.**

### 2.2 Estrazione da verbi non-copulari

Oggi solo `is`/`are`/`was`/`were` e locativi. Manca:
- **Transitivi:** "X eats Y", "X borders Y", "X produces Y"
- **Possesso:** "X has Y", "X contains Y"
- **Attributi:** "X weighs Y kg", "X is Y years old"
- **Definizioni:** "X means Y", "X is defined as Y"

### 2.3 Estrazione wide con tolleranza agli errori

`deep-reasoning.md` §4.4: estrarre **dall'intero paragrafo introduttivo**, molte
relazioni, **accettando errori**. La resilienza viene dopo (provenance +
auto-correction). Oggi l'estrazione è conservativa: solo frasi che matchano
esattamente i frame M0.

### 2.4 `intent_schema` per la comprensione (da universal-comprehension)

Il piano `universal-comprehension.md` §4.2 descrive `intent_schema(Pattern, Intent)`
— riconoscere l'intento dalla struttura, non dal lessico. Questo meccanismo,
combinato con `extract_frame`, permetterebbe a parrot0 di:

1. Riconoscere che una frase ha *intento dichiarativo* (asserzione)
2. Estrarre la struttura soggetto-predicato-oggetto
3. Mapparla al predicato KB corrispondente
4. Asserire il fatto con provenienza

Tutto KB-first, tutto con lo stesso motore C.

### 2.5 Fatti temporanei e stratificazione

I fatti estratti dalla prosa sono `KB_SESSION` (non persistono oltre la sessione
a meno che non venga fatto `/save`). Per il deep-reasoning loop, servono fatti
con `KB_REFLECTIVE` che vivono solo per la durata dell'inferenza. Questa
stratificazione esiste già (`kb_set_origin`) ma non è ancora usata
sistematicamente nell'estrazione.

---

## 2bis. Il piano è KB-driven: `plan_goal` in `actions.p0`

Il meccanismo per orchestrare l'estrazione **non è un pipeline C cablato**.
È un **piano dichiarativo in KB**, interpretato dal backward chainer generico
di `mod_plan` (`src/brain/25-wordmath-reasoning.c:130`).

### Il dominio plan in `kb/experts/codebase/actions.p0`

```prolog
plan_goal(extract_knowledge, kb_facts).

action_yields(read_page_file, raw_page).
action_needs(read_page_file, page_path).
action_impl(read_page_file, prose_read_page).

action_yields(split_sentences, sentences).
action_needs(split_sentences, raw_page).
action_impl(split_sentences, prose_split_sent).

action_yields(extract_frames, candidate_facts).
action_needs(extract_frames, sentences).
action_impl(extract_frames, prose_extract).

action_yields(assert_extracted, kb_facts).
action_needs(assert_extracted, candidate_facts).
action_impl(assert_extracted, prose_assert).

plan_given(page_path).
```

### Come funziona

1. **L'utente** dice "extract knowledge from sicily" o "leggi e impara la sicilia"
2. **`goal_cue`** mappa la frase al goal `extract_knowledge`
3. **`plan_chain`** (C, generico) cammina all'indietro da `kb_facts` attraverso
   `action_needs`/`action_yields` e produce l'ordine: `read_page_file` →
   `split_sentences` → `extract_frames` → `assert_extracted`
4. **`plan_execute_goal`** esegue ogni step cercando `action_impl` e dispatchando
   al primitivo C corrispondente (`prose_read_page`, `prose_split_sent`, ...)
5. **L'artefatto finale** `kb_facts` è raggiunto: i fatti sono nella KB

### Proprietà anti-impostor

- **Cambiare il piano** (aggiungere un passo, cambiare ordine) = editare fatti in
  `actions.p0`. Zero C nel chainer.
- **Insegnare un nuovo goal** = aggiungere `plan_goal` + `goal_cue`. Zero C.
- **Aggiungere un'azione** = `action_yields` + `action_needs` + `action_impl`
  (quest'ultimo richiede ~20 righe di C nel dispatch dei primitivi, ma è un
  motore atomico, non conoscenza di dominio)
- Il chainer **non sa nulla** di prosa, estrazione, o fatti KB. Se scambi i
  fatti con quelli di `external_help`, lo stesso C pianifica "chiedere aiuto".

Questo è il principio **"plan by KB"** che governa tutta l'architettura di
parrot0: il piano è conoscenza, il motore è fisso.

### Stato attuale dei primitivi

I primitivi `prose_read_page`, `prose_split_sent`, `prose_extract`, `prose_assert`
sono stub che restituiscono successo senza fare lavoro reale. Vanno implementati
con la logica di `read_passage`/`extract_clause` già esistente in
`src/brain/30-generation-reading.c`. Questa è la parte C rimanente — ma è
**motore atomico**, non conoscenza di dominio.

---

## 3. Il piano concreto (roadmap)

Ogni passo è un obiettivo per generazione, con un counterexample reale e un
oracolo misurabile.

### E1 — `extract_frame/3` come tabella KB (il motore)

**Counterexample:** "The cheetah is the fastest land animal" → oggi walla; dovrebbe
estrarre `fastest_land_animal(cheetah)` o simile.

**Cosa fare:**
1. Aggiungere `extract_frame/3` in `kb/core/world-facts.p0` con i pattern M0
   esistenti (F1-F6) come fatti iniziali
2. Scrivere un consumer C (~60 righe) in `mod_knowledge` che:
   - Legge `extract_frame(Pattern, Pred, Slots)` dalla KB
   - Matcha `Pattern` contro la frase canonica (con `$X` come variabili di slot)
   - Estrae i token negli slot
   - Chiama `kb_assert` + `fact_source`
3. Verifica che i test `prosefact.chat` e `prosepage.chat` restino verdi
4. Aggiungere un test runtime-growth: asserire un nuovo `extract_frame` via MCP
   deve abilitare l'estrazione di quel pattern senza rebuild

**Misura:** `tests/cases/prosefact.chat` passa con frame KB invece che C.
Un nuovo pattern aggiunto a runtime estrae fatti senza toccare il C.

### E2 — Verbi transitivi e relazionali

**Counterexample:** "Italy borders France" → oggi walla; dovrebbe asserire
`borders(italy, france)`.

**Cosa fare:**
1. Aggiungere pattern `"$X borders $Y"` → `borders` in `extract_frame`
2. Aggiungere pattern `"$X eats $Y"` → `eats`, `"$X has $Y"` → `has`
3. Aggiungere fatti `borders/2` nel KB per testare
4. Verificare che la prosa produca i fatti corretti

**Misura:** Una frase "Italy borders France" letta via `read:` produce
`borders(italy, france)` interrogabile con "does italy border france?".

### E3 — Attributi e quantità dalla prosa

**Counterexample:** "An octopus has eight arms" → oggi walla; dovrebbe estrarre
`quantity(octopus, arms, 8)`.

**Cosa fare:**
1. Aggiungere pattern `"$X has $N $Y"` con parsing del numero
2. Aggiungere pattern `"$X weighs $N $U"`, `"$X is $N $U long"`
3. Integrare con `mod_quantity` per il parsing numerico

**Misura:** La frase produce fatti `quantity/4` interrogabili.

### E4 — Estrazione wide con tolleranza

**Counterexample:** Un paragrafo Wikipedia di 5 frasi su "Paris" — oggi estrae
solo le frasi copulari; dovrebbe estrarre tutte le relazioni riconoscibili,
accettando qualche falso positivo.

**Cosa fare:**
1. Abbassare la soglia di confidenza: estrarre da ogni frase che matcha
   *qualsiasi* `extract_frame`, non solo copule
2. Ogni fatto estratto conserva `fact_source/3` con la frase grezza
3. Il loop di auto-correzione (`deep-reasoning` M4) pulisce dopo

**Misura:** `tests/cases/prosepage.chat` cresce nel numero di fatti estratti
da una pagina nota, senza introdurre falsi positivi non correggibili.

### E5 — `intent_schema/2` (da universal-comprehension §4.2)

**Counterexample:** Frasi con struttura nota ma lessico sconosciuto ("Zembla is
a country in Asia") → oggi walla; dovrebbe riconoscere l'intento dichiarativo
ed estrarre `country(zembla)` + `located_in(zembla, asia)` anche senza conoscere
"Zembla".

**Cosa fare:**
1. Implementare `intent_schema(Pattern, Intent)` nella KB
2. Il motore C matcha il pattern contro la struttura della frase (pos,
   copula, complementi), NON contro il lessico
3. Le parole sconosciute riempiono gli slot come atomi
4. Il declino è informato ("ho capito che Zembla è un paese in Asia, ma non
   conosco Zembla") invece del muro cieco

**Misura:** Una frase con soggetto sconosciuto produce un fatto con quell'atomo,
non un muro. Il test di umiltà si sposta sul dato mancante, non sulla struttura.

---

## 4. L'architettura bersaglio

```
PROSA GREZZA
    │
    ├── read_passage()
    │     │
    │     ├── split in frasi (su '.')
    │     │
    │     └── per ogni frase:
    │           normalize() → canonicalize_lang(b, ...)
    │           │
    │           ├── intent_schema(Pattern, Intent)  ← KB: che intenzione ha?
    │           │     └── se INTENT = assertion:
    │           │
    │           ├── extract_frame(Pattern, Pred, Slots)  ← KB: come si estrae?
    │           │     │
    │           │     ├── match pattern → fill slots
    │           │     ├── kb_assert(Pred, slot_values)
    │           │     └── fact_source(fact, subject, raw_sentence)
    │           │
    │           └── se nessun pattern matcha → skip (non è un muro: è prosa
    │               che non sappiamo ancora estrarre)
    │
    └── post-estrazione:
          ├── kb_induce()  ← indurre regole dai nuovi fatti
          ├── contradiction detection  ← M4 auto-correzione
          └── persist (KB_SESSION → /save)
```

**Il confine C/KB:**

| C (motore fisso) | KB (conoscenza che cresce) |
|---|---|
| `split` in frasi, `normalize`, `canonicalize_lang` | `extract_frame(Pattern, Pred, Slots)` |
| Match di pattern con `$X` variabili | I pattern stessi (F1-F6, + tutti i nuovi) |
| `kb_assert` + `fact_source` | I predicati e i fatti del dominio |
| `kb_induce`, contradiction detection | Le regole di inferenza |
| Il loop di auto-correzione | `fact_source/3` per tornare alla fonte |

---

## 5. Invarianti non negoziabili

1. **KB-first.** Ogni nuovo pattern di estrazione è un fatto KB. Il C non
   contiene frasi, verbi, o pattern specifici di linguaggio naturale.
2. **Provenance obbligatoria.** Ogni fatto estratto ha `fact_source/3` con
   la frase grezza da cui proviene. Senza provenance, l'auto-correzione è
   impossibile.
3. **Tolleranza + correzione, non precisione.** Estrarre ampio, accettare
   errori, correggere dopo. Il modello è `deep-reasoning.md` §4.4.
4. **Nessuna fabbricazione.** Se nessun pattern matcha, si skippa la frase.
   Non si inventano fatti.
5. **Substrato.** I fatti estratti dalla prosa sono `KB_SESSION`. I fatti
   curati nel repository sono `KB_BASE`. La distinzione è marcata
   (`is_internal_pred`) e non inquina "quante cose sai?".
6. **Test offline deterministici.** `prosefact.chat` e `prosepage.chat`
   usano fixture locali, mai rete.

---

## 6. Il caso concreto che traina il piano

La domanda "sono nato in sicilia quindi sono un ___" ha mostrato che parrot0 sa
*rispondere* a completamenti se i fatti ci sono. Ma quei fatti (`demonym/2`) sono
stati scritti a mano.

Con l'estrazione dalla prosa, parrot0 potrebbe **imparare `demonym/2` da solo**,
leggendo una pagina Wikipedia sulla Sicilia:

```
> read the page on sicily
parrot0:   [legge kb/learning/pages/sicily.md]
           "Sicily is the largest island in the Mediterranean Sea."
           "Sicily is an autonomous region of Italy."
           "The capital of Sicily is Palermo."
           "People born in Sicily are called Sicilians."
           → extracted: island(sicily), located_in(sicily, mediterranean_sea),
             autonomous_region(sicily), capital_of(sicily, palermo),
             demonym(sicily, sicilian)
```

E il turno dopo:

```
> sono nato in sicilia quindi sono un
parrot0:   Sicilian.   (dal fatto demonym(sicily, sicilian) appena estratto)
```

Questo è il ciclo completo: **leggere prosa → estrarre fatti → usarli per
rispondere**. Ed è esattamente ciò che `extract_frame` KB-first abilita.

---

## 7. Riferimenti

- [[kb-first]] — il manifesto: engine fixed, knowledge learns
- [[universal-comprehension]] — comprendere la forma; intent_schema/2
- [[universal-input]] — la prosa è un registro; segmentazione KB-first
- [[universal-solver]] — prosa → vincoli → deduzione/abduzione
- [[deep-reasoning]] — estrazione ampia, provenance, auto-correzione
- [[teach-comprehension-via-mcp]] — insegnare pattern di comprensione via MCP
- [[teachable-procedures]] — procedure come conoscenza KB, non C
- [[the-linguistic-glue]] — colla coesiva tra comprensione e conversazione
- [[llmscore-strategies]] — training loop che usa extract_page_facts
- [[learn-and-build]] — wiki_concept come prosa, non struttura eseguibile
