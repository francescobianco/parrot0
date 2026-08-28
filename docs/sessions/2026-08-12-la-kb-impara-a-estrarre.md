# Sessione 2026-08-12 — la KB impara a estrarre, e il motore smette di degradare con lei

*gen378 → gen382b. Due commit, una sessione lunga. Cominciata con "studia la
memoria e i commit sulla conversione del C", finita con parrot0 che sogna
Wikipedia e raccoglie le forme in cui la conoscenza viene detta. In mezzo, la
scoperta che dava senso a tutto il resto: **la KB era cresciuta e il motore era
peggiorato con lei**, che è precisamente il fallimento che un sistema KB-first
non può permettersi.*

---

## 0. Come è cominciata, e la cosa trovata prima di toccare nulla

Richiesta iniziale: studiare la memoria di progetto e i commit recenti sulla
conversione di pezzi di C che non rispettano il principio KB-first. Lo studio ha
prodotto una ricetta ripetibile in cinque passi (trova il letterale → chiedi se è
classe chiusa o procedura → fatti in `grammar.p0`/`intents.p0` o clausola in
`procedures.p0` → **un solo** motore generico condiviso → prova per contrasto a
runtime), e l'osservazione che il caso più istruttivo dei commit letti era un
**rifiuto**: `KB_MACHINERY` (gen374), scritto, testato e buttato lo stesso
giorno, perché rendeva il sandbox più comodo mentre l'obiettivo è farlo sparire.

Poi, al primo `make soft-test`, l'albero pulito era **rosso**:

```
FAIL  [antonym] line 22 — turn took 1.97s (timeout 1.00s)
```

`make test` moriva sul primo file. Non un test sbagliato: un turno che costava
due secondi.

## 1. La crescita della KB degradava il motore (il cuore della sessione)

Il profilo ha dato una risposta netta. `kb_query` scandiva **tutti** i fatti a
ogni lookup — anche, e soprattutto, quando il predicato non esisteva affatto — e
il pass di boot `kb_derive_part_of` chiede `is_model_pred()` una volta per fatto.
Costo quadratico: 13k × 15k. Invisibile a 3k fatti, due secondi a 13k dopo la
crescita di gen380/381.

> Non è "la KB è grande". È che **la KB cresce e il motore peggiora**, cioè
> esattamente il fallimento che un sistema KB-first non può avere: renderebbe la
> conoscenza un costo invece che una risorsa.

Il rimedio è meccanismo puro, nessuna conoscenza nel C: un **censimento per
predicato** in `kb.c` (nome → numero di fatti, quanti non-ground, posizioni),
con invalidazione pigra sulle rimozioni. Letto da `kb_query` (miss O(1), hit
ground via l'indice hash che già esisteva), `kb_match`, `kb_knows_pred` e dalle
due scansioni del solver.

**3.09s → 0.11s, 28×.** ASAN/UBSAN puliti.

### 1b. E i `!timeout` erano cerotti

F. l'ha nominato prima che lo cercassi: *«lo sviluppatore precedente ha aggiunto
per errore dei `!timeout` a 8 secondi, convinto che la lentezza non fosse un
problema»*. Un budget alzato **nasconde ciò che copre**, ed è il contrario del
mantra ("se sfora si tolgono casi, non si alza il budget").

Per verificarlo serviva uno strumento che non c'era: `PARROT0_TE_SLOW=<sec>`
nomina ogni turno sopra soglia **qualunque sia il suo budget**. Misura su tutta
la suite: **un solo turno supera 1s**, ed è `rulespec` a 2.57s, che *compila
davvero* un programma C. I `!timeout 8` di gen379 coprivano turni da 0.31s.

23 override su 24 rimossi, e corretti i commenti che dicevano "lento per NATURA"
— non lo era.

## 2. L'isteresi: quattro frasi bastano a fermare il processo

`KB_MAX_DEPTH` limitava quanto **a fondo** andava una derivazione, non quanto
**lavoro** faceva. Con due regole per la stessa testa la ricerca si ramifica a
ogni livello dentro il limite di profondità:

```
every zorp is a blim / every krant is a blim / every blim is a zorp / every blim is a krant
is vex a blim?
```

Quattro frasi di conversazione normale. Misurato: **>60s, ucciso**. Con le due
guardie (taglio del goal ground già aperto sul cammino, tetto di passi): 0.4s.

### 2b. Ma una guardia non è un halt — la correzione di F.

> *«La condizione di isteresi non deve essere una sorta di halt di sistema ma più
> come la consapevolezza di stare maneggiando un paradosso.»*

Rispondere `No.` a una ricerca **interrotta** è negation-as-failure applicata a
un mondo mai chiuso: dichiara falso ciò che è solo indeterminato, e butta via la
cosa più informativa scoperta nel turno.

Per progettare contro l'evidenza invece che contro l'intuizione, una sonda su un
ragionatore vero (`tests/probes/hysteresis_probe.py`, che **non gira a runtime**). Tre
risultati:

1. non dice mai "No": dice *«con le sole informazioni fornite non possiamo
   stabilirlo»*;
2. deriva la struttura: le quattro premesse dicono che **le tre classi sono la
   stessa classe**;
3. nomina l'errore del motore: *«confonde mancanza di prova con verità del
   negativo»*.

Ora parrot0 dice che le classi si rimandano a vicenda e che **non dimostrato non
vuol dire falso** — con il testo in KB (`response_template(undetermined_cycle)`,
EN+IT) e i due siti polari unificati in un solo meccanismo.

*Aperto:* leggere il ciclo come **equivalenza di classi**, che è conoscenza
derivabile e non un guasto.

## 3. Quattro classi lessicali fuori dal C

- **`indefinite_article/1`** — 55 call site, il sito più esteso, escluso da
  gen369 per l'ampiezza. L'ostacolo vero non era il numero ma i sandbox senza
  conoscenza, e gen371 l'aveva già tolto. Il compilatore ha isolato da solo i 5
  punti senza `Brain`.
- **`np_opener/1`** — `p0_lead_det` e la macro `P0_LEAD_DET` elencavano
  *ciascuna* gli articoli: due copie nel motore di una classe che la KB aveva già
  dal gen363. `p0_is_prep` è **sparito**, non sostituito: faceva solo il confine.
- **il plurale** (`plural_of/2` + `plural_suffix/2`) — otto irregolari e quattro
  regole di morfologia inglese. Il guadagno non è lo spostamento: `-ses` è
  ambiguo fra *bus+es* e *sense+s*, nessuna regola di suffisso può deciderlo, e
  il C dava `singularize("senses") = "sens"` (annotato in `20-math.c` come
  difetto da aggirare). Come eccezione **dichiarata** è una riga.
- **`compare_entity_token`** — quindici letterali, di cui **quattordici già
  fatti `stopword/1`**, con il lettore di quella classe pure già scritto. Il
  quindicesimo era `planet`, e non era una parola funzionale: **nomina una
  categoria**. Detto così vale per ogni categoria.

Su quest'ultimo ho scritto in un commento che i fiumi avrebbero funzionato "di
conseguenza". **Non era vero, e misurandolo l'ho scoperto**: era bloccato altre
due volte, entrambe da conoscenza — il reticolo `magnitude_cue/3` aveva solo i
superlativi per `length`, e lo stesso fiume viveva sotto due identificatori
(`nile` nelle relazioni, `the_nile` nelle magnitudini) così i suoi fatti non
componevano. Commento corretto a ciò che è stato misurato.

## 4. L'estrazione da prosa: prima misurarla

Direttiva di F.: *«la comprensione della prosa e i processi di gestione di essa
devono essere KB-first: l'abilità di comprensione è essa stessa uno stato della
KB che può comprendere, non un processo C.»*

Il primo fatto scoperto è che l'estrazione era misurata **solo su prosa
preparata**: le sezioni `## Extract` del corpus sono scritte a mano in frasi già
semplificate ("Paris is the capital of France"). Su prosa vera di Wikipedia il
comportamento è un altro:

```
astronomical_body_so_compact_that_its_gravity_prevents_anything_including_light(black_hole)
```

Un'intera relativa diventata nome di predicato. Serviva un numero: **`make
prose-bench`**, offline e senza giudice, che non giudica la *verità* di un fatto
ma una condizione **necessaria** — che i suoi atomi siano concetti e non frasi.

| | malformati | usabili/pagina |
|---|---|---|
| partenza | **58.7%** | 0.51 |
| fine sessione | **~11–25%** | ~0.9 |

Cosa l'ha mosso, tutto KB:

- **`np_closer/1`** — dove finisce un sintagma, applicato ai **tre** lati:
  classe, soggetto, e cornice iniziale («*In mathematics and computer science,* an
  algorithm is…», che è il gen378 riletto da un altro lato: un segmento può avere
  il ruolo "non fa parte del dato");
- **il generico plurale** — «whales are mammals» andava a muro; ora produce
  `mammal(X) :- whale(X)`. F.: *«dalla prosa si devono imparare sia fatti che
  regole, se no la KB non cresce»*. Il lettore di pagine le contava come fatti e
  ne stampava il messaggio dentro l'elenco: si approfondiva **senza poterlo
  vedere**;
- **`extract_frame/2`** — il salto che il piano chiedeva dal gen335: dodici
  relazioni dichiarate, e il motore non sa che «located in» parli di luoghi.
  Aggiungere una forma estraibile costa **un fatto**. Ha anche corretto
  `capital(paris)`, che il test chiamava "lettura partitiva" e *tollerava*, in
  `capital_of(paris, france)`.

## 5. `--dream`

`parrot0 --dream <topic> [--depth=N] [--nodes=N] [--fetch] [--persist]`.

La mia prima versione usava una **coda** (breadth-first): leggeva una prosa,
accodava tutte le sue parole, e studiava tutto il primo livello prima di
scendere. F. l'ha corretto: *«le prose si studiano e poi si procede
nell'albero»*. È una **pila** — e la differenza non è implementativa: con la coda
l'esplorazione è decisa in anticipo da un solo testo; con la pila ogni passo è
deciso da ciò che si è appena capito. Ed è l'unica delle due su cui abbia senso
innestare, più avanti, una potatura alfa-beta: si pota un *ramo*, e un ramo
esiste solo se si scende.

Il trace nomina ciò che ha **perso**, non solo ciò che ha imparato — ed è quella
metà che ha fatto emergere tre difetti nel primo giro vero:

1. **trappola degli omonimi**: sognando il modale «may» ha scaricato il **mese di
   maggio** e imparato `fifth_month(may)`;
2. **fuga di routing**: «read the page on is» finisce al lettore di **file**
   (`tools_disabled: …`), e «on found» a un modulo di generazione, che ha
   risposto *«A tiny clockwork bird with emerald eyes»*;
3. **il costo misurato delle stopword**: ~50% del budget di nodi, zero
   conoscenza. F. le voleva nell'esplorazione; ora il costo è un numero, non
   un'opinione.

Persisteva in `kb/core/session.p0`, **che è in `.gitignore`**: imparava davvero e
perdeva tutto al commit. Una memoria che non sopravvive al commit non è memoria,
è una cache. Ora instrada nell'albero curato col save-map.

## 6. Il cancello, e la distinzione che ho dovuto imparare rompendo due test

Senza un cancello, sognare scrive spazzatura nell'albero curato — ed era già
successo: `located_in(mitochondrion, cells_of_most_eukaryotes_such_as_animals_
plants_and_fungi)` era stato instradato in `world-facts.p0`.

Il criterio **non introduce vocabolario**: un atomo è rotto quando ha inghiottito
un `np_closer`. La stessa conoscenza che dice all'estrattore dove fermarsi dice
al cancello quando non si è fermato.

Prima versione sbagliata: respingeva `located_in(france, europe)`, perché
`located_in` *contiene* `in`. Ma quel nome non è uno span — è l'identificatore di
una relazione **dichiarata**, scelto, non ritagliato dalla prosa. Quindi: agli
argomenti il test pieno, al nome della relazione il solo tetto di lunghezza. Non
è un'eccezione di comodo, è la differenza fra un nome scelto e un nome ritagliato.

## 7. Le forme espressive — il secondo raccolto

F., sul pruning alfa-beta: *«il dominio mi piace e non mi piace come principio.
Sognare non significa solo imparare in profondità un topic ma anche imparare
nuove forme espressive della conoscenza stessa… non solo dominio, ma anche
dialettica di come la prosa trasmette la conoscenza.»*

È corretto e cambia il progetto: potare per dominio poterebbe via esattamente le
parole che portano la **struttura** della trasmissione — «known as», «such as»,
«instead of», «due to». Quelle non sono conoscenza su un topic: sono conoscenza
su **come** la conoscenza viene detta. E sono il raccolto più prezioso perché è
**ricorsivo sul sistema**: una forma raccolta diventa un `np_closer`, un
`extract_frame`, una regola di plurale — e da quel momento *ogni sogno successivo
estrae meglio*. È il ciclo che rende il sognare auto-migliorante.

Il giro dedicato è partito da `definition` (dove i due raccolti coincidono) e ha
prodotto il suo risultato utile **fallendo**: il sogno cerca una *pagina* per
ogni parola, ma «known as» non ha una pagina — la sua conoscenza sta nel
**ricorrere**. Quindi le ho cercate dove stanno, misurando il corpus:

| forma | ricorrenze | trasmette |
|---|---|---|
| `such as` | 10 | esemplificazione |
| `known as` | 15 | alias |
| `is called` | ~10 | denominazione |
| **`may refer to`** | **8** | **non è conoscenza: è una disambiguazione** |

Le prime tre sono cinque `extract_frame` → `also_known_as/2`. La quarta è la
**spiegazione della trappola degli omonimi**: una pagina che dichiara di non
descrivere un significato non va letta, e riconoscerla è conoscenza sulla forma
del testo (`disambiguation_marker/1`). Il declino è informato: *«ho una pagina su
X **ma** disambigua più significati invece di descriverne uno»*.

## 8. Tre difetti della stessa famiglia

Trovati chiudendo il punto 7, e vale la pena tenerli insieme perché sono **lo
stesso errore**: un risultato riportato come il suo contrario.

- il rifiuto del cancello (`2`) veniva appiattito su `1`: un fatto **respinto**
  usciva come fatto **imparato**;
- il `continue` sul rifiuto saltava l'avanzamento in fondo al ciclo delle frasi →
  **loop infinito**, due pagine su dodici non tornavano più;
- `prosebench` contava `Scartato: …` come fatto malformato, cioè **puniva
  l'onestà**: un sistema che respinge sembrava peggio di uno che tace.

Lo stesso della famiglia di gen376 (le regole nascoste dentro l'elenco dei
fatti). Vale come classe da sorvegliare: ogni volta che si aggiunge un esito, va
verificato che il *riporto* lo distingua, o l'esito nuovo viene letto come quello
vecchio.

## 9. Metodo — tre direttive di F., tutte in memoria

1. **Tutto il prompting nato per gli LLM deve valere per parrot0**, come atto di
   conoscenza e non come stratagemma. Il caso concreto: un "system prompt" che
   precede la prosa e ne garantisce il pilotaggio. La sonda `prose_probe.py` l'ha
   **misurato**: senza contesto l'LLM produce una decomposizione skolemizzata,
   con il contesto dichiarato produce `is_a(black_hole, astronomical_body)` —
   esattamente lo scheletro che serve. L'ipotesi è fondata; l'implementazione in
   parrot0 resta da fare.
2. **Le sonde LLM non sono pezzi di parrot0.** Verificato: l'unica rete a runtime
   è `learn.c` → Wikipedia; i riferimenti "OpenAI" nel codice sono parrot0 che
   **serve** quell'API (è il modello, non il client); nessuna sonda è agganciata
   a `make test`, zero Python nella suite.
3. **Passi corti e limitati.** Critica giusta al mio modo di procedere: mi ero
   immerso in un filo lungo invece di chiudere passi verificabili. `prose-bench`
   è ora limitato per default (12 pagine, timeout 60s) — una misura deve dare il
   segnale in secondi ed essere interrompibile, e le cose emergono presto.

## 10. Aperto

- **Gli scratch brain.** F.: i sillogismi non devono girare su una KB amputata;
  parrot0 dovrebbe *scaricare* i token jolly che vuole usare e verificare che
  nessuna conoscenza li intercetti — KB sempre piena. Sono 5 siti, è un cambio
  architetturale che merita un passaggio suo.
- **Il ciclo come equivalenza di classi** (§2b).
- **Il system prompt come conoscenza** in parrot0 (§9.1): un `read_frame(Dominio,
  …)` che parrot0 antepone da sé alla prosa.
- **Il secondo estrattore per le forme**: raccogliere gli operatori dal
  *ricorrere* nella prosa, invece di cercare una pagina per parola (§7).
- **La fuga di routing** di `--dream` (§5.2) e lo span che scavalca una
  preposizione invece di fermarcisi (asserito con diagnosi in
  `magnitude_compare.p0t`).

---

**Bilancio.** `make test` da bloccata al primo file a **1685 asserzioni verdi**;
soft-test 2s su 15; estrazione da prosa vera da 58.7% a ~11–25% di malformati;
sette classi/procedure uscite dal C verso la KB; due strumenti nuovi
(`prose-bench`, `PARROT0_TE_SLOW`), due sonde di progetto, un modo nuovo di
esplorare (`--dream`), e 12 pagine di Wikipedia acquisite e committate.
