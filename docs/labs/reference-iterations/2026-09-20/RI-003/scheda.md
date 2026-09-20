# RI-003 — una richiesta può escludere, e una classe insegnata dev'essere scegliibile

**ID, famiglia e contesto umano.** Famiglia: *vincolo negativo dentro una
richiesta* (condotta + mondo allargato), causa di fallimento diversa da RI-001
(superficie mancante) e RI-002 (verso sbagliato in un consumatore). Contesto
umano: si chiede un esempio e si esclude quello ovvio, perché lo si conosce
già. È il modo normale di chiedere «un altro».

**Commit/stato iniziale.** `450a5ba8` (RI-002 chiusa), profilo `agi`, lingua `en`.

**Stimolo congelato.**

```text
Tell me a country in Asia, but do not mention China.
```

**Criterio di riuscita.** Nomina un paese asiatico **diverso** dalla Cina, e
**non asserisce niente**: il vincolo non è una cosa da credere.

**Risposta iniziale e limite osservato** (`r1-baseline.json`):

| turno | risposta |
|---|---|
| Tell me a country in Asia. | «China.» (il positivo funziona) |
| **Tell me a country in Asia, but do not mention China.** | **«Held: do does not mention china.»** |
| Name three Italian cities, but do not mention Rome. | «Held: do does not mention rome.» |

Non è un muro: è una **clausola falsa** — soggetto «do», relazione «mention» —
scritta in sessione e annunciata come appresa. Con un `/save` finirebbe in KB.

**Curriculum — scritto prima della cura** (fonti: en.wikipedia.org/wiki/List_of_cities_in_Italy,
tabella «Rank 1 Rome, 2 Milan, 3 Naples, 4 Turin»):

| # | lezione | aggiunge |
|---|---|---|
| L1-L4 | `Milan/Naples/Turin/Rome is a city.` | la classe `city`, insegnata parlando |
| L5-L8 | `Milan/Naples/Turin/Rome is in Italy.` | il vincolo positivo verificabile |

**Trasferimenti fissati prima.** TR1: `Tell me a mammal, but do not mention the
dog.` (altra classe, nome escluso con articolo). TR2: `Tell me a city in Italy,
but do not mention Milan.` (vincolo positivo **e** esclusione insieme, su una
classe insegnata nella sessione). Contrasto: `Tell me a country in Asia.` →
la Cina resta ammessa, l'esclusione non è permanente.

## R4 — quattro siti, e il quinto trovato con `/debug`

La ricerca è nata dalla prima lezione inefficace e ha attraversato la catena
riconoscimento → arbitraggio → scrittura:

1. **`negate_relation` leggeva la richiesta come lezione.** Il modo dichiarato
   di una forma conosceva due forze su tre; la terza — `directive` — la KB la
   pubblica già (`turn_illocution`, `illocution_cue`) e nessuno gliela chiedeva.
   Ora una forma `statement` non legge un turno direttivo.
2. **`p0_negation_lead` faceva lo stesso** un livello più in basso: usciva sulle
   domande e non sulle richieste. Una riga, la stessa conoscenza.
3. **`adjunct_peel` staccava il vincolo dalla richiesta.** «tell me a country in
   asia,» ha sei parole prima della virgola: l'inciso veniva sbucciato e il
   residuo «but do not mention china» ridispacciato **da solo** — un turno senza
   contenuto, che il registro sociale prendeva con «Fair enough…». Ora un
   residuo che si apre con un'esclusione tiene il turno intero:
   `exclusion_marker/1` è KB, e nel C non entra nessuna parola.
4. **Chi sceglie il membro non sapeva escludere.** Il residuo dopo la categoria
   era già un vincolo da soddisfare (`member_satisfies/2`): mancava il suo verso
   negativo. Ora si toglie il membro nominato e il vincolo positivo si legge
   solo su ciò che precede l'esclusione. Se l'esclusione toglie l'ultimo membro,
   lo si dice (`instance_all_excluded`) invece di tacere.
5. **Una classe insegnata parlando non era scegliibile.** «Milan is a city.»
   entrava e «tell me a city» restava un muro: `category_member/2` era scritto a
   mano. Ora un fatto unario È un'appartenenza (`kb_fact/2`), con la guardia
   della superficie pronunciata e del non-essere-macchina — una classe nuova
   vale dal turno dopo.

**`/debug` ha nominato il sito che i grep non trovavano.** Sul turno che
resisteva («tell me a city, but do not mention Rome», con la categoria ancora
ignota):

```text
turn_illocution          directive
debug_frame_record       frame(assertion, binary(mention)), roles(subject(city), object(rome))
debug_scope_requirement  scope(span(7,7), polarity, negated)
debug_grammatical_cue    evidence(span(5,5), coordination, contrast) …
```

La IR **vede** il contrasto e la polarità negata, dichiara che quello span
«richiede un ambito prima di asserire», e il frame viene impegnato lo stesso
come **assertion** su un turno di forza **directive**. `scope_requirement/4` è
prodotto (english-grammar/reading.p0:260) e **non è consumato da nessuno** prima
del commit semantico: il commento del file lo dice — «ispezionabili prima del
commit semantico» — ma nessuno ispeziona. Due guardie tentate in
`extract_class_statement` e `p0_try_extract_frames_only` non hanno cambiato
nulla di misurabile e sono state **tolte** invece di restare come decorazione.

## R5 — certificazione (`r5-certification.json`)

| stato | esito |
|---|---|
| iniziale | «Held: do does not mention china.» (clausola falsa) |
| meccanismo riparato, senza lezioni | stimolo **«India.»**; `tell me a city in Italy` ancora muro |
| dopo le lezioni | `Tell me a city in Italy.` → «Milan.» |
| TR2 (vincolo positivo + esclusione) | `…, but do not mention Milan.` → **«Naples.»** |
| TR1 (altra classe, articolo) | `Tell me a mammal, but do not mention the dog.` → «Elephant.» |
| contrasto | `Tell me a country in Asia.` → «China.» — l'esclusione non è permanente |
| ablazione (`r5-ablation.json`) | `forget that milan is a city` → «Forgotten: city(milan).»; `Tell me a city.` → «Naples.»; `Is Milan a city?` → non so |
| controllo indipendente | Naples resta dopo l'ablazione di Milan |
| replay RI-002 / RI-001 | «Yes.» / «north sea.» |

## R6 — salvataggio: il gap di persistenza, misurato

```text
parrot0: routed 49 clause(s) into the KB tree
```

| categoria | n | clausole |
|---|---:|---|
| `W` fatti veri | **3** | `located_in(milan|naples|turin, italy)` (`rome` già presente) |
| `P` provenienza | 28 | `fact_source` ×8, `reading_fact` ×8, `fact_mention` ×12 |
| `O` altre | 16 | `utterance` |
| `X` | 0 | — |
| **perse** | **4+** | `city(milan)`, `city(naples)`, `city(turin)`, `city(rome)`, `class_surface(city, city)` |

**Il difetto, isolato su un caso solo:** una sessione con la sola lezione
«Verona is a city.» dichiara `routed 6` e ne scrive **5** — provenienza,
menzione e due battute. Il fatto `city(verona)` non finisce in nessun file:
non in `learning/learned.p0` (la ricaduta di chi non ha casa), non altrove.
`grep -rn "^city(" kb/` rende **zero**. Quindi **ogni classe insegnata
parlando si perde al salvataggio**, e con lei l'ingresso del ponte
`category_member/2` aperto qui.

**Processo nuovo** (`r6-fresh-process.json`): 4/5.
`Tell me a country in Asia, but do not mention China.` → «India.» (persiste),
`Where is Turin?` → «Italy and piedmont.» (persiste), ma
`Tell me a city in Italy.` → muro, e `Is Milan a city?` → non so.
`FreshProcessRecall` **non** è 100%: per `LEARN_PROTOCOL` §10 questo non si
committa come training riuscito.

## Stato e prossimo problema

**Stato dell'iterazione:** `partial`. Il nucleo — un vincolo negativo dentro una
richiesta — è curato, certificato e **persiste** (è meccanismo). Le lezioni di
classe non sopravvivono al `/save`, quindi la seconda metà della capacità non è
ancora conservata.

**Classificazione del training:** `partial` (W = 3 salvati e verificati nel
processo nuovo; 4 clausole apprese perse in salvataggio).

**Prossimo problema (RI-004):** il router del `/save` dichiara instradata una
clausola che poi non scrive. È il candidato migliore che questa iterazione
lascia: senza, «una classe si insegna parlando» resta vera solo dentro la
sessione — e il test del mantra si ferma lì.
