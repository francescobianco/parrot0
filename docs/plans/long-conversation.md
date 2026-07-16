# Conversazione lunga — missione 100 turni coerenti

> **Stato:** esperimento iniziale gen335+ (2026-07-17).
> **Missione:** 100 turni consecutivi coerenti.
> **Partenza:** 3 turni.
> **Riferimenti:** [[the-linguistic-glue]] (la colla come tessuto connettivo),
> [[kb-first-manifesto]] (engine fixed, knowledge learns),
> [[universal-comprehension]] (nessun muro cieco).

## L'esperimento

17 turni in inglese con PARROT0_WORLD_FACTS=1. Risultato:

```
✓ ✓ ✓   greeting → nome → recall              (3 coerenti)
✗       "Remember what I told you?"            (crollo)
✓       aritmetica isolata
✗       "and plus 3?"                          (continuazione fallita)
✓ ✗     fatto OK / "and of Italy?"             
✓ ✗     fatto OK / "and the light bulb?"
✗ ✗     Einstein (routing KB) / "he" (coref)
✓       barzelletta isolata
✗       nome dimenticato dopo 11 turni
✓ ✓ ✓   meta: turn count + first utterance + bye (3 coerenti)
```

**Soffitto attuale: 3-5 turni coerenti consecutivi.** La conversazione non muore mai
(il motore non crasha), ma la coerenza collassa rapidamente. Fatti isolati sempre
recuperabili in qualunque momento.

## Baseline gold — un LLM che parla con sé stesso (il target, misurabile)

> **Aggiunto gen335 (2026-07-16).** Prima di evolvere la colla su parrot0 serve un
> **riferimento attendibile** di come *è fatta* una conversazione lunga coerente. Non
> lo inventiamo: lo **catturiamo** facendo parlare un LLM con sé stesso e conservando i
> transcript come baseline. `longtalk_bench.py` misura parrot0 *contro* un LLM; queste
> baseline sono l'altra metà — **cosa deve assomigliare** parrot0.

- **Generatore:** `tests/longchat/selfchat_baseline.py` (stesso framework di
  `llmscore.py`: opencode-GO, `$OPENCODE_API_KEY`). Due "sedie" A/B, **lo stesso
  modello** su entrambe, ciascuna con una persona minima che dà una spina alla
  conversazione (altrimenti due modelli identici collassano in un loop di cortesia).
- **Baseline conservate:** `tests/longchat/baselines/selfchat-<scenario>-<model>.{md,json}`,
  3 scenari × 18 turni, `minimax-m2.5`. Ognuna porta un **profilo di dinamica** (euristico).

| Scenario | Turni | Parole/turno | Coref cross-turn | Domande | Dinamica dominante |
|---|---|---|---|---|---|
| `acquaintance` (conoscersi) | 19 | 62 | **73** | 18 | memoria di dettagli condivisi + callback |
| `trip` (pianificare un viaggio) | 19 | 31 | 17 | 9 | continuazione + goal pendente ("book by mid-week") |
| `opinions` (chiacchiere che derivano) | 19 | 64 | **81** | 12 | topic-drift + correzione + coreferenza densa |

**L'osservazione che ridisegna le priorità del piano.** In tutte e tre le baseline i
*connettori a inizio turno* ("and of Italy?") sono **rari** (metrica `continuation_openers`
≈ 0): l'LLM tiene la coerenza **soprattutto per COREFERENZA e callback topicale**
(73-81 riferimenti cross-turn per sessione), non con frammenti-"and". La continuazione
esiste ma è **mid-turn** ("Cabin is the move. *And for food*, should we…"). Conseguenza:
il pull più redditizio non è `fragment_resolve` (R1, connettore iniziale) ma **la
coreferenza cross-turn + la CONSISTENZA del contesto** (R2 + "un solo interlocutore") —
è lì che vive il grosso della colla di un LLM. R1 resta valido, ma è una fetta minore di
quanto il piano assumeva.

**Caveat onesti.** (a) Il profilo è euristico (conta pronomi/`?`), caratterizza la
baseline, non la *giudica*. (b) `continuation_openers` è turn-initial-only, quindi
sotto-conta le continuazioni mid-turn — da migliorare. (c) Nello scenario `opinions` una
sedia ha rivelato "you're an AI?" a metà: è un **artefatto reale del self-chat** (i due
modelli a volte rompono la finzione), conservato onestamente come parte della dinamica.

**Uso previsto.** Queste baseline sono il **metro di paragone** per far crescere la colla
di parrot0: per ogni sintomo chiuso (R1-R4, vincoli, ecc.) si confronta il comportamento
di parrot0 sullo **stesso scenario** con la baseline gold, e si misura quanto della
dinamica (coref, callback, consistenza) parrot0 riproduce. Rigenerare/estendere:
`python tests/longchat/selfchat_baseline.py --scenario all --turns 30`.

## La colla linguistica come framework

La [colla linguistica](the-linguistic-glue.md) non è un modulo. È il **tessuto
connettivo** che permette a ragionamento, memoria e pianificazione di apparire
come un interlocutore unico. Non produce inferenze: tiene insieme quelle che
esistono già.

I **5 sintomi di colla assente** (dal glue-bench):

| # | Sintomo | Esempio nell'esperimento | Stato glue-bench |
|---|---------|--------------------------|-----------------|
| 1 | Fuori-contesto | "and of Italy?" dopo "capital of France?" → muro | qualitativo |
| 2 | Riferimento implicito | "he" (Einstein) non risolto attraverso turni | **3/3 HELD** |
| 3 | Letteralità eccessiva | "and plus 3?" non continua "2+2=4" | **2/2 HELD** |
| 4 | Correzione non integrata | "Remember what I told you?" → perso | **2/2 HELD** |
| 5 | Più sistemi indipendenti | memoria + aritmetica non si parlano | **2/2 HELD** |

**La colla esiste già, sparsa.** Meccanismi già costruiti e funzionanti:

| Meccanismo | Cosa fa | File |
|-----------|---------|------|
| `coref_resolve` | Risolve "it"/"his" all'entità recente e ri-dispatcha | `99-registry.c:958` |
| `continue_resolve` | "and times 3" continua il calcolo col `last_result` KB | `99-registry.c:1264` |
| `memref_resolve` | "my favorite number + 3" risolve dalla KB `user_value/2` | `99-registry.c:1257` |
| `correction_peel` | "no, X non è Y" → retract + ri-deriva | `99-registry.c:1250` |
| `pragma_peel` | "anyway, ..." → sbuccia l'apertura e ri-dispatcha | `99-registry.c:1244` |
| `utterance/3` | Log conversazionale queryable | `99-registry.c:1171` |
| `topics` / `last_entity` | Stato di discorso persistente | `brain.c` |

## KB-first: la regola cardinale applicata alla conversazione

> **Ogni stato conversazionale è un fatto KB. Il C ospita motori; la KB ospita
> conoscenza. Nessun nuovo campo C per ricordare cose sull'utente o sulla sessione.**

Cosa significa in pratica:

| Stato | Oggi (da migrare o già KB) | Direzione |
|-------|---------------------------|-----------|
| Nome utente | `b->name` (campo C) — debito | `user_value(name, "Francesco")` nella KB |
| Entità recente | `b->last_entity` (campo C) — già funziona | `entity(last, "Einstein")` come fatto KB |
| Topic corrente | `b->topics` (buffer C) — già funziona | `topic(current, "geography")` come fatto KB |
| Ultimo risultato | `last_result/1` (KB, REFLECTIVE) — ✅ KB-first | Già fatto. `note_arith_result` asserisce/retract |
| Valori utente | `user_value/2` (KB, SESSION) — ✅ KB-first | Già fatto. Persiste su `/save` |
| Turni conversazione | `utterance/3` (KB, REFLECTIVE) — ✅ KB-first | Già fatto. Queryable per "prima cosa che hai detto" |
| Vincoli attivi | ❌ non esistono | `active_constraint(brevity, max_words(30))` come fatto KB |
| Goal pendente | ❌ non esiste | `pending_goal(capital_of_country(italy, ?))` come fatto KB |

**Test di compliance KB-first per ogni nuovo meccanismo:**

> *Può l'utente ispezionare lo stato con una query KB? Può ritrattarlo? Può
> insegnare una nuova variante senza ricompilare?*

Se la risposta a una di queste è no, il meccanismo non è KB-first.

## I 4 punti di rottura (diagnosi attraverso la lente della colla)

### R1 — Continuazione attraverso turni (sintomo #3: letteralità)

| Esempio | Sintomo colla | Meccanismo esistente | Perché fallisce |
|---------|--------------|---------------------|-----------------|
| "2+2?" → "4." poi "and plus 3?" | Letteralità | `continue_resolve` funziona per aritmetica pura | "and of Italy?" non è aritmetica — il meccanismo copre solo operatori matematici |
| "Capital of France?" → "Paris." poi "and of Italy?" | Letteralità | Nessuno | `decompose_and_dispatch` splitta "and" solo DENTRO lo stesso turno |

**Strategia KB-first:** `fragment_resolve` come pre-dispatch transform. Se il turno
inizia con "and"/"e"/"also"/"what about", recupera l'**intent** dell'ultimo turno
dalla KB:

```
% KB: mappa un intent a un predicato e al suo slot variabile
intent_continuation("capital of", capital_of_country, 1).
intent_continuation("who invented", invented_by, 0).

% KB: l'ultimo intent risolto, asserito a fine turno
last_intent(capital_of_country(france, paris)).
```

Il motore C: recupera `last_intent/1`, estrae il predicato, sostituisce l'entità
con quella nuova dal turno corrente ("Italy"), ri-dispatcha.

### R2 — Coreferenza attraverso turni (sintomo #2: riferimento implicito)

| Esempio | Sintomo colla | Meccanismo esistente | Perché fallisce |
|---------|--------------|---------------------|-----------------|
| "What year did Einstein die?" → muro → "What year was he born?" | Riferimento implicito | `coref_resolve` esiste | Cerca antecedente solo nel turno corrente |

**Strategia KB-first:** estendere `coref_resolve` a cercare nel turno N-1 tramite
`utterance/3`. Le entità sono già nella KB come fatti:

```
% KB: entità menzionate con genere
entity_mentioned(einstein, male, 15).   % turno 15
entity_mentioned(france, neuter, 7).    % turno 7
```

Il motore C: se un pronome ("he"/"she"/"it"/"they") non è risolto nel turno
corrente, cerca l'ultimo `entity_mentioned` compatibile per genere nei turni
precedenti (da `utterance/3` + `entity_mentioned/3`), sostituisci, ri-dispatcha.

### R3 — Recall astratto / meta-contesto (sintomo #1: fuori-contesto)

| Esempio | Sintomo colla | Perché fallisce |
|---------|--------------|-----------------|
| "Remember what I just told you?" | Fuori-contesto | Nessun intento per richieste meta-contestuali |
| "Do you still remember my name?" | Fuori-contesto | Pattern matching rigido: "remember my name" ≠ "what is my name" |

**Strategia KB-first:** `intent_phrase` per richieste meta-contestuali. La risposta
è derivata dalla KB, non da un template C:

```
% KB: frasi che significano "recall what I told you"
intent_phrase(meta_recall, "remember what i told you").
intent_phrase(meta_recall, "do you still remember").
intent_phrase(meta_recall, "what did i just say").
intent_phrase(meta_recall, "tell me what i mentioned").

% KB: il motore query l'ultimo turno utente
% e risponde con il fatto appreso in quel turno
```

Il motore C: matcha `meta_recall` via `kb_intent_match`, recupera `utterance(N-1,
user, $Text)`, cerca fatti appresi in quel turno, risponde.

### R4 — Degrado del contesto nel tempo (sintomo #5: sistemi indipendenti)

Dopo 11 turni, "Do you still remember my name?" fallisce. Il nome è appreso al
turno 2 ma la formulazione della domanda non matcha i pattern esistenti.

**Strategia KB-first:** routing flessibile. Non un array di pattern C, ma fatti KB:

```
% KB: tutte le varianti di "what is my name" mappate allo stesso predicato
intent_phrase(memory_recall_name, "what is my name").
intent_phrase(memory_recall_name, "do you still remember my name").
intent_phrase(memory_recall_name, "what's my name again").
intent_phrase(memory_recall_name, "who am i").
intent_phrase(memory_recall_name, "tell me my name").
intent_phrase(memory_recall_name, "come mi chiamo").

% KB: lo slot mappa a un predicato
memory_slot(name, user_value(name, $V)).
memory_slot(age, user_value(age, $V)).
memory_slot(favorite_number, user_value(favorite_number, $V)).
```

Il motore C: matcha l'intent, risolve lo slot, query `user_value/2`, risponde.
Aggiungere una variante = un fatto KB. Zero C.

## Le 4 strategie (con vincolo KB-first)

### S1 — Fragment resolve (attacca R1)

```
Meccanismo: pre-dispatch transform "fragment_resolve"
Stato KB: last_intent(Pred, Args), intent_continuation(Cue, Pred, SlotIdx)
Gate C: turno inizia con "and", "e", "also", "what about"
Azione C: recupera last_intent dalla KB, matcha intent_continuation,
          sostituisci l'entità nello slot SlotIdx, ri-dispatcha
Nuovi fatti KB: 1 intent_continuation per ogni predicato continuabile
Test: "capital of France?" → "and of Italy?" → "and of Germany?" (3 continuazioni)
```

### S2 — Coref cross-turn (attacca R2)

```
Meccanismo: estendere coref_resolve a leggere entity_mentioned/3
Stato KB: entity_mentioned(Name, Gender, TurnSeq), utterance/3
Gate C: pronome non risolto nel turno corrente
Azione C: cerca entity_mentioned per genere nei turni N-1, N-2, ...
          sostituisci il pronome, ri-dispatcha
Nuovi fatti KB: entity_mentioned asserito a ogni turno dal motore (REFLECTIVE)
Test: "When was Einstein born?" → "When did he die?" (pronome attraverso turni)
```

### S3 — Meta-context recall (attacca R3)

```
Meccanismo: nuovo intent_schema meta_recall in mod_knowledge
Stato KB: utterance/3 (già esistente), intent_phrase(meta_recall, ...)
Gate C: kb_intent_match(b, "meta_recall", norm)
Azione C: recupera utterance(N-1, user, Text), cerca user_value asseriti,
          rispondi "You told me that [name] is [value]."
Nuovi fatti KB: intent_phrase per ogni variante linguistica di meta_recall
Test: "my name is X" → (3 turni dopo) → "remember what I told you" → "You told me your name is X"
```

### S4 — Routing flessibile (attacca R4)

```
Meccanismo: memory recall con memory_slot/2
Stato KB: user_value/2 (già esistente), memory_slot/2, intent_phrase(memory_recall_name, ...)
Gate C: kb_intent_match per memory_recall_name
Azione C: risolvi memory_slot($Slot, user_value($Slot, $V)), query user_value/2, rispondi
Nuovi fatti KB: intent_phrase per ogni variante, memory_slot per ogni slot
Test: "what's my name again?" / "do you still remember my name?" / "who am i?" → stesso risultato
```

## La disciplina (ereditata dalla colla linguistica)

1. **Un meccanismo per generazione, tirato da un fallimento.** Non si pre-progetta
   l'architettura della conversazione lunga. Si parte dal primo caso che fallisce
   nel `glue-bench` (o in un test di conversazione), si costruisce il meccanismo
   minimo, si verifica, si committa. Poi il prossimo.

2. **Deterministico e ispezionabile.** Ogni meccanismo opera su stato KB reale.
   "Perché hai risposto così?" deve avere una risposta: "perché `user_value(name,
   "Francesco")` era nella KB."

3. **EN+IT su ogni caso.** Il ratchet bilingue si applica a ogni nuovo meccanismo.
   Se funziona in inglese ma non in italiano, non è fatto.

4. **Nessun campo C nuovo per conoscenza.** Se un meccanismo ha bisogno di
   ricordare qualcosa, lo mette nella KB. Il C fornisce solo il motore di
   risoluzione e sostituzione.

5. **Ogni gap chiuso guadagna un test `.chat` in `make test` e un caso HELD
   in `glue-bench`.** La misura è binaria: GAP → HELD. Non ci sono sfumature.

## Lo stato della colla oggi (eredità da G0-G2)

| Livello glue-bench | Casi crisp | Stato |
|-------------------|-----------|-------|
| Riferimento implicito | 3/3 HELD | `coref_resolve` + `pro-drop` IT |
| Correzione | 2/2 HELD | `correction_peel` |
| Un solo interlocutore | 2/2 HELD | `memref_resolve` (memoria→aritmetica) |
| Letteralità | 2/2 HELD | `continue_resolve` (precisazione aritmetica) |
| Fuori-contesto | 0/0 crisp | **Unico gap rimasto** — da rendere crisp |

## Traiettoria

```
3-5 turni   →  oggi (fatti isolati + memoria nome + glue-bench 9/9 crisp)
8-10 turni  →  dopo M1 (fragment_resolve: continuazione fatti attraverso turni)
12-15 turni →  dopo M2 (coref cross-turn: pronomi risolti attraverso turni)
18-25 turni →  dopo M4 (routing flessibile: recall memoria in tutte le varianti)
30+ turni  →  dopo M3 (meta-contesto: "remember what I told you 5 turns ago")
50+ turni  →  composizione dei 4 moti + vincoli attivi + goal pendenti
100 turni  →  missione: summarization, forgetting selettivo, topic switching,
              self-correction, contesto compresso ma queryable
```

## Il salto da 50 a 100

Oltre i 50 turni servono capacità che oggi non esistono, tutte KB-first:

- **Summarization:** dopo N turni, `utterance/3` va compresso. Un fatto KB
  `conversation_summary(TurnRange, "l'utente ha chiesto fatti su geografia e storia")`
  generato ogni K turni. Il contesto non si perde — si condensa.

- **Forgetting selettivo:** `entity_mentioned/3` con peso decrescente nel tempo.
  Entità non referenziate per >20 turni → peso zero. La KB può crescere ma i fatti
  vecchi vengono ritratti. Meccanismo: `entity_decay/1` come regola KB.

- **Topic switching:** `topic(current, $T)` già esiste come buffer. Va reso
  persistente come fatto KB e queryable. "Torniamo a parlare di geografia" →
  recupera il topic precedente e le entità associate.

- **Self-correction:** "no, mi chiamo Marco non Francesco" → `correction_peel`
  già funziona per fatti semplici. Va esteso a: retract di `user_value(name,
  "Francesco")`, assert di `user_value(name, "Marco")`, ri-derivazione di tutte
  le risposte che dipendevano dal nome. Il goal `last_intent/1` viene rieseguito
  con i nuovi fatti.

- **Vincoli attivi persistenti:** "keep it short" → `active_constraint(brevity,
  max_words(30))` come fatto KB. Ogni risposta viene troncata dal motore se
  supera il limite. Il vincolo sopravvive ai turni finché non viene ritrattato.
  Questo chiude l'ultimo sintomo `out-of-context` del glue-bench.

## Ordine di attacco (dalla mappa dei fallimenti)

| # | Moto | Cosa | Impatto | Sforzo | Tira da |
|---|------|------|---------|--------|---------|
| 1 | M1 | Fragment resolve (continuazione fatti) | Sblocca "and of Italy?" | Medio | Esperimento R1 |
| 2 | M2 | Coref cross-turn | Sblocca "he" dopo 1+ turni | Medio | Esperimento R2 |
| 3 | M4 | Routing flessibile memoria | Sblocca tutte le varianti di "what is my name" | Basso | Esperimento R4 |
| 4 | M3 | Meta-context recall | Sblocca "remember what I told you" | Medio | Esperimento R3 |
| 5 | — | Vincoli attivi KB-first | Chiude `out-of-context` (ultimo sintomo glue-bench) | Medio | glue-bench gap #1 |
| 6 | — | Goal pendenti | Rende la conversazione orientata a obiettivi | Alto | 50+ turni |
| 7 | — | Summarization + forgetting | Permette 100 turni senza overflow | Alto | 100 turni |
