# Gold standard KB-first — migrazione completa dei percorsi linguistici C

**Stato:** norma attiva, 25 agosto 2026
**Ambito:** `src/brain/**`, `src/code.c` e ogni futuro producer/consumer di
linguaggio
**Decisione:** ogni percorso linguistico nel C è debito da convertire o
rimuovere; la compatibilità non è una destinazione architetturale

## 0. La decisione

Un percorso è linguistico quando il suo esito cambia perché nel C compare una
forma di una lingua o di un registro: parola, locuzione, affisso, preposizione,
connettivo, pronome, ausiliare, domanda, sinonimo, nome di operazione, keyword
di un linguaggio di programmazione o frase da emettere.

Tutti questi percorsi sono **obsoleti per definizione**, anche quando oggi
producono una risposta corretta. Vanno portati su conoscenza interrogabile,
insegnabile e retraibile a runtime, oppure rimossi. Non basta spostare una lista
in un altro file C e non basta consultare la KB dopo che un `if` linguistico ha
già scelto la facoltà.

La domanda di accettazione è una sola:

> parrot0 può imparare domani un nuovo membro della stessa classe, usarlo e
> dimenticarlo, senza rebuild e senza che il C conosca la sua lingua?

Se la risposta è no, il percorso non è gold.

Questa norma rende superata la precedente prudenza “non cancellare il C
esistente”. Un fallback può sopravvivere soltanto durante una migrazione
misurata; deve essere escluso dai testimoni gold, osservabile come debito e
avere una condizione esplicita di rimozione.

## 1. Confine normativo

### Il C può fare soltanto meccanica fissa

- leggere byte e preservare offset;
- tokenizzare senza attribuire significato ai token;
- costruire intervalli, ordine, parentela e annidamento;
- bilanciare delimitatori ricevuti come dati;
- unificare, ordinare, calcolare e gestire memoria;
- eseguire transazioni, assert, retract, replay e rollback richiesti dalla KB;
- eseguire una primitiva atomica di I/O, incluso il fetch di una revisione già
  indirizzata dalla KB;
- invocare protocolli semantici aperti come `input_frame_observe/2`, senza
  conoscere alcun loro membro linguistico.

### La KB deve decidere

- lingua di input, sorgente e output;
- forme, lemmi, morfologia, sinonimi, locuzioni e multiword;
- categorie grammaticali, confini di sintagma e ordine dei ruoli;
- intenti, atti dialogici, registri e facoltà;
- operatori semantici e viste asserzione/domanda;
- strategie di ricerca di prove e piani;
- dipendenze, precedenze, tempi logici, budget e condizioni di arresto;
- scelta fra risposta, chiarimento, acquisizione, compensazione e declino;
- struttura proposizionale e template della realizzazione.

La distinzione non è “dati in KB, algoritmo in C”. Una strategia di ricerca o
una policy di budget è conoscenza eseguibile e resta in KB; il C implementa
soltanto le primitive attraverso cui quella derivazione opera.

## 2. Forme vietate nel percorso gold

Un nuovo diff non è accettabile se introduce una delle forme seguenti con
valore naturale nel C:

```text
cue(text, "...")
strstr(text, "...")
strcmp(token, "...")
strncmp(text, "...", n)
switch(language) / strcmp(language, "it"|"en"|...)
array di articoli, connettivi, ausiliari, keyword o sinonimi
printf/snprintf con una frase rivolta all'interlocutore
```

Il divieto riguarda il **valore naturale**, non le operazioni di stringa in sé.
Confrontare un identificatore di protocollo, un nome di predicato, una modalità
meccanica o una firma interna può essere corretto; confrontare `between`,
`senza`, `what`, `read:` o `return` per attribuirgli un ruolo non lo è.

Anche questi tre impostori sono vietati:

1. **KB tardiva:** il C riconosce la frase e interroga la KB soltanto per
   completare la risposta;
2. **intent chiuso:** le cue sono in KB ma il C le traduce in un enum finito di
   significati linguistici;
3. **pivot obbligatorio:** una lingua viene riscritta in inglese nel C prima
   che il frame sia osservabile, perdendo forma e provenienza originali.

## 3. Contratto del percorso gold

Il percorso completo è:

```text
byte
  -> nodi e span meccanici
  -> lingua e forme con evidenza KB
  -> frame semantico con ruoli
  -> proposizione canonica + provenienza
  -> prova / obbligo / piano KB
  -> piano proposizionale
  -> realizzazione KB nella lingua di output
```

Ogni confine materializza un oggetto interrogabile. Nessun consumer deve
riaprire la stringa grezza per riconoscere di nuovo ciò che il producer aveva
già compreso.

La IR ricca e gli indici di consumo hanno funzioni diverse:

- `semantic_proposition/1` conserva significato completo;
- `proposition_source/4` conserva superficie, lingua e span;
- proiezioni come `semantic_binding/3` rendono la stessa proposizione
  efficientemente consumabile dal solver;
- i record osservati, come `input_frame_record/3`, dichiarano quale lettura
  unica è stata ammessa al confine del turno.

Una proiezione non è un secondo parser: deve essere derivata dalla stessa IR e
deve scomparire con la sua sorgente.

## 4. Definizione operativa di “migrato”

Un percorso C è migrato soltanto se soddisfa insieme le condizioni seguenti:

1. **residenza:** ogni forma naturale e ogni scelta di categoria risiede in
   relazioni KB;
2. **porta comune:** il C invoca un matcher o protocollo generico e non nomina
   i membri della classe;
3. **crescita:** asserire a runtime un membro nuovo abilita il comportamento;
4. **ablazione:** ritrarre quel membro rimuove il comportamento senza rebuild;
5. **multilingua aperto:** almeno un idioma non enumerato dal motore entra e
   si ritrae dalla stessa porta;
6. **dualità:** quando pertinente, prosa e domanda producono/consumano la stessa
   firma canonica;
7. **provenienza:** superficie, lingua e byte span restano interrogabili;
8. **ambiguità:** due letture sostenute non vengono risolte dal primo match;
9. **realizzazione:** il testo emesso deriva da forme/template KB;
10. **esclusione legacy:** il testimone non attraversa il vecchio recognizer;
11. **rimozione:** quando tutti i consumer della famiglia sono migrati, il ramo
    precedente viene eliminato, non mantenuto “per sicurezza”.

Una risposta golden o il solo fatto che una frase funzioni non dimostrano
nessuno di questi punti.

## 5. Censimento riproducibile del 25 agosto 2026

Il primo censimento conservativo usa:

```sh
rg -n '(strstr|strcmp|strncmp|strcasecmp|strncasecmp|cue|starts_with|startswith|ends_with|endswith|word_eq|token_eq)[^;\n]*"[A-Za-z][^"]*"' src/brain src/code.c
```

Il primo snapshot del lavoro Gen438 ha prodotto **2040 siti sospetti**. La
migrazione del guard `read:` a `decompose_span_policy/2` porta il checkpoint
corrente a **2039** (`delta = -1`, nessuna aggiunta). È un limite superiore,
non un numero di violazioni già giudicate:
include identificatori tecnici e può non vedere frasi generate senza un
confronto. La misura serve come rete di cattura e baseline riproducibile; ogni
sito resta rosso finché classificato come meccanica o migrato.

| File | Siti sospetti | Prima famiglia da isolare |
|---|---:|---|
| `10-memory-knowledge.c` | 598 | canonicalizzazione, domande, preposizioni, estrattori |
| `25-wordmath-reasoning.c` | 403 | verbi di problemi, connettivi, parser di relazioni |
| `40-meta-reflection.c` | 188 | intenti meta/sociali e risposte |
| `30-generation-reading.c` | 187 | estrazione di prosa e realizzazione |
| `20-math.c` | 181 | nomi di operatori e forme interrogative |
| `80-code.c` | 93 | superfici di specifica e categorie |
| `50-self-research-loop.c` | 74 | indirizzi, forme di ricerca e lettura |
| `85-translate-synth-world.c` | 54 | traduzione, comandi e ausiliari |
| `99-registry.c` | 52 | fallback, dispatch e glue dialogico |
| `70-social-pragma.c` | 50 | assenso, incertezza, contrasto, topic |
| `60-agent-tools.c` | 49 | sequenziatori, stopword, tipi di file |
| `code.c` | 42 | keyword e registri di linguaggi di programmazione |
| `90-repair-robust-abduce.c` | 37 | supposizione, sorpresa e riparazione |
| `65-induce-verify-shell.c` | 22 | sequenze, regole e intenti di verifica |
| `00-lex.c` | 9 | morfologia e classificazione lessicale |

Separatamente esistono **46 annotazioni `TODO(kb-first)`/occorrenze
esplicative** in `src/brain` e `src/code.c`. Non rappresentano la copertura del
debito: mostrano soltanto i casi già riconosciuti. Il divario 46/2040 è il
motivo per cui il processo non può dipendere dai TODO manuali.

### Classi di migrazione

| Codice | Classe | Esempi osservati | Destinazione |
|---|---|---|---|
| C0 | ingresso e lingua | cue, prefissi, `it/en`, segment role | `language_marker`, `segment_role`, evidenza universale |
| C1 | lessico e morfologia | articoli, plurali, ausiliari, stopword | `concept_label`, classi grammaticali, regole morfologiche |
| C2 | sintassi e binding | preposizioni, congiunzioni, ordine, wh | nodi/span, `frame_pattern`, ruoli e composizione |
| C3 | semantica | nomi di operatori, estrattori per relazione | frame/operatori/proposizioni KB |
| C4 | dialogo e meta | intenti sociali, memoria, chiarimenti | atti, obblighi, policy e cue KB |
| C5 | pianificazione | sequenze, ricerca, costi, budget | stati, azioni, dipendenze e policy KB |
| C6 | realizzazione | `tput`, frasi fallback, summary | `response_template`, frame proposizionali |
| C7 | linguaggi tecnici | keyword C/Python, log, estensioni | register evidence e grammatiche KB |

Ogni sito verificato riceverà classe, producer attuale, consumer, relazione KB
bersaglio, testimone causale e generazione di rimozione. Le righe non si migrano
in ordine di file: si migrano per **taglio verticale**, così la nuova conoscenza
ha subito sia un producer sia un consumer.

## 6. Misure e gate

Per una famiglia verticale `W` si registrano:

```text
legacy_hits(W)       numero di decisioni linguistiche C attraversate
kb_forms(W)          forme KB effettivamente consumate dalla proof
runtime_growth(W)    membri nuovi abilitati da assert senza rebuild
runtime_ablation(W)  membri rimossi da retract senza rebuild
language_growth(W)   idiomi nuovi entrati dalla stessa porta
ir_identity(W)       uguaglianza del grafo canonico fra lingue/modi
provenance_loss(W)   coordinate lingua/superficie/span mancanti
unsupported(W)       proposizioni o testo senza supporto
```

Il gate locale è una congiunzione, non una media:

```text
legacy_hits(W) = 0
runtime_growth(W) >= 1
runtime_ablation(W) = runtime_growth(W)
language_growth(W) >= 1 quando la famiglia è linguistica
ir_identity(W) = 1
provenance_loss(W) = 0
unsupported(W) = 0
```

Il gate globale di ogni generazione aggiunge:

1. nessun nuovo sito sospetto non classificato rispetto allo snapshot iniziale;
2. tutte le famiglie dichiarate migrate hanno ramo legacy irraggiungibile nei
   testimoni e rimosso appena non ha consumer residui;
3. il censimento viene rieseguito da albero pulito e il delta è pubblicato;
4. italiano e inglese non hanno privilegi nel C;
5. almeno un testimone usa una lingua aggiunta interamente a runtime.

## 7. Ordine della bonifica

La priorità segue la catena causale, non il numero di literal:

1. **ingresso, lingua e span** — ogni lavoro successivo dipende da coordinate
   corrette;
2. **prosa e domanda sulla stessa IR** — impedisce due parser divergenti;
3. **lessico, morfologia e composizione** — rende produttive forme nuove;
4. **indirizzamento e lettura Wikipedia** — solo dopo che il gap è linguistico
   o fattuale in modo distinguibile;
5. **dialogo, meta e socialità** — atti e obblighi al posto del first-match;
6. **matematica, ragionamento e word problem** — operatori canonici, non verbi;
7. **ricerca e piani** — policy, dipendenze e budget interamente KB;
8. **realizzazione** — eliminazione di `tput` e delle frasi C;
9. **registri tecnici** — keyword e grammatiche insegnabili;
10. **rimozione degli adapter** — zero percorso linguistico legacy nelle
    campagne finali Gen446.

Questo ordine non autorizza nuovo debito nelle classi basse mentre si lavora su
quelle alte. Dal presente documento ogni nuova funzionalità linguistica nasce
gold oppure non entra.

## 8. Primo taglio gold: Gen438

Il percorso ora ammesso come riferimento è:

```text
read:/leggi:/ingest:       segment_role/2 nella KB
clausola                   input_node + superficie + range
lingua sorgente            osservatore n-lingue da language_marker/2
frame                      input_semantic_frame/4
commit                     semantic_proposition/1 + semantic_binding/3
provenienza                proposition_source/4 + reading_fact/2
domanda in altra lingua    stesso operatore, ruolo missing
risposta                   lexical_output + value_statement KB
```

Il C in questo taglio misura, pubblica e invoca i protocolli
`input_frame_commit/2` e `input_frame_observe/2`. Il dispatcher eredita da
`decompose_span_policy/2` che una sorgente va mantenuta intera. Nessuna
decisione del verticale ispeziona `read:`, `leggi:`, `ingest:`, nomi di lingue,
verbi di relazione o wording del riepilogo.

L'esperimento `gen438-prose-dialogue.p0t` dimostra 57 proprietà strutturali:

- fonte inglese interrogata in italiano;
- fonte italiana interrogata in inglese;
- Esperanto aggiunto interamente a runtime;
- crescita e ablazione del trigger del lettore;
- crescita e ablazione della forma dell'operatore;
- rifiuto di due letture concorrenti;
- conservazione di lingua, superficie e byte span;
- stessa proposizione ricca e indice semantico consumabile.
- due clausole nella stessa fonte con span globale distinto (`0,19` e `21,21`).

Il vecchio blocco di estrazione sotto `input_frame_commit/2` è ancora un
fallback transitorio e resta fuori dal gold. La sua presenza tiene aperta la
bonifica C2/C3; il suo mancato attraversamento nel testimone prova però che il
nuovo percorso è già autonomo rispetto a esso.

La prima prova Gen439 (`gen439-phrases.p0t`) aggiunge una locuzione insegnata a
runtime come nodo `phrase`: 14 proprietà passano includendo riconoscimento,
ordine per span, composizione nella proposizione, consumo nella domanda e
ablazione della forma. Il confine verbale usato dalla segmentazione è anch'esso
un fatto KB (`pos/2`), non una parola incorporata nel C. Coordinazioni e
locuzioni annidate restano il prossimo incremento del gold.

## 9. Rapporto con le dieci generazioni

- **438:** definisce il gold e porta il primo taglio prosa/domanda end-to-end;
- **439:** estende composizione e rimuove gli estrattori di prosa equivalenti;
- **440:** migra canonicalizzazione e indirizzamento; il campione
  `gen440-addressing.p0t` passa 15 proprietà con crescita/ablazione runtime;
- **441:** introduce la sorgente mirata in memoria; `gen441-wikipedia-memory.p0t`
  passa 7 proprietà di selezione, provenance e replay senza corpo persistente;
- **442:** collega gli arresti al piano di compensazione KB; il campione
  `gen442-compensation.p0t` passa 9 proprietà su passi, costi, dipendenze,
  lingue e crescita/ablazione runtime;
- **443:** introduce quarantena e promozione minima; `gen443-quarantine.p0t`
  passa 8 proprietà di replay causale e rollback per campagna;
- **441:** applica il gold alla prosa Wikipedia con provenance di revisione;
- **442:** migra scelta dialogica e piani di compensazione;
- **443:** rende transazionali dipendenze, promozione e rollback;
- **444:** dimostra insegnamento assistito di classi senza nuovo C;
- **445:** migra ricerca, budget e pianificazione causalmente;
- **446:** esegue campagne L0–L8 con `legacy_hits=0` e rimuove gli adapter
  ancora raggiungibili.

Il censimento globale può arrivare a zero solo progressivamente. Il requisito
immediato è più severo e più utile di una promessa finale: ogni taglio che viene
dichiarato nuovo standard ha già `legacy_hits=0`, crescita, ablazione,
multilingua e provenienza, e nessun nuovo literal linguistico è accettato nel C.
