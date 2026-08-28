# 2026-08-29 — Il gloss è il punto fisso di M15

Sessione condotta secondo [`LEARN_PROTOCOL.md`](../../../LEARN_PROTOCOL.md), su
indicazione di F. di lavorare **sopra il cancello del §6** di
[`apprendimento-assistito.md`](../../plans/apprendimento-assistito.md): far
crescere lo strato di meta-comprensione, non chiudere prompt.

**Stato finale: `meta-capability-only`** — `W = 0` fatti sul mondo (non era
l'obiettivo), `C = 2` capacità nuove, `X = 0`.

## 1. Parametri

| | |
|---|---|
| `DOMINIO` | il metalinguaggio: come si chiede una cosa |
| `OBIETTIVO` | rendere insegnabile parlando la **forma della domanda** (M15/M16) senza nominare lo schema interno |
| `BUDGET` | una sessione |
| `FONTI` | il repository stesso (misure con `lang.canonical`), e conoscenza linguistica italiana verificabile |
| `TARGET_CAPABILITIES` | 1–2 |
| `STOP_CONDITION` | primo misclaim non spiegato |

## 2. Il punto di partenza sbagliato, e come si è visto che era sbagliato

Il piano dichiara M15 «il blocco più urgente»: `answer_frame/2` — la relazione
fra una superficie interrogativa e la relazione da interrogare — era raggiungibile
solo aprendo un file. Il gen457 lo aveva già aperto a metà.

La prima ipotesi era aggiungere una `question_shape` italiana, o una riga
`answer_frame` italiana. F. ha proposto la versione astratta: *«invece di "quale
colore …" insegnare "quale X …" con X = colore, altezza, lato»*. Quella forma
**esisteva già** (`question_shape/1`, gen457, quattro forme inglesi).

Prima di scriverne una italiana ho misurato che cosa il riconoscitore vede
davvero, con `lang.canonical`:

```
"quale colore si usa in chess"   ->  "which color is usa in chess"
"which color is used in chess"   ->  "which color is used in chess"
```

La canonicalizzazione era già arrivata **a un token** dalla forma inglese che
risponde. `quale`→`which`, `colore`→`color`, `si`→`is` c'erano tutti. Mancava
`usa`→`used`.

Non mancava una superficie e non mancava una forma: **mancava una parola**. È il
mantra #3 un gradino più in basso di dove si stava guardando — la domanda
italiana non è una superficie nuova, è la *stessa* superficie sotto una
traduzione.

La verifica del sospetto, con un solo fatto messo a mano in un processo di prova:

```
!clause tr(used, usa).
> quale colore si usa in chess
Bianco e nero.
```

## 3. Perché è il punto fisso e non un'altra riga

`tr/2` sta **a monte di ogni modulo**: la canonicalizzazione lo consulta su ogni
token, prima del dispatch. Chiudere il gloss non apre una famiglia di domande,
le apre tutte insieme. Una riga `answer_frame` italiana avrebbe aperto una
domanda; il gloss apre ogni domanda che contiene quella parola, in ogni
relazione, in ogni modulo.

Il sondaggio su sei domande italiane vere lo mostra — ognuna è a una o due parole
dalla forma che risponde:

| detto | canonicalizzato | manca |
|---|---|---|
| quale fiume attraversa parigi | which river **attraversa** paris | `attraversa` |
| quanti lati ha un triangolo | how many **lati** has a **triangolo** | `lati`, `triangolo` |
| quale organo produce la bile | which **organo** **produce** the bile | `organo`, `produce` |
| quale metallo conduce elettricità | which metal **conduce elettricità** | 2 parole |

## 4. Che cosa mancava davvero: l'atto didattico

`tr/2` non era raggiungibile parlando. Baseline, cinque formulazioni naturali:

```
the italian for used is usa        ->  I don't understand that yet.
in italian used is usa            ->  We can chat in either language …
"usa" is the italian word for "used" ->  Learned: italian_word(usa).      (perde "used")
usa means used in italian         ->  … non riesco ad allineare due variabili …
usa is italian for used           ->  I don't understand that yet.
```

Ma **una** forma naturale il motore la legge già, col frame relazionale binario
che esiste da sempre:

```
usa is the italian of used   ->  Learned: italian(usa, used).
```

Il fatto nasceva e restava morto: nessuno lo leggeva. Il ponte mancante è una
riga di conoscenza, non di motore (`kb/core/gloss.p0`):

```
gloss_language(italian).
tr($English, $Form) :- gloss_language($Language),
                       apply($Language, cons($Form, cons($English, nil))).
```

`apply/2` è il meta-richiamo che c'è dal gen382n: **nessun motore nuovo**.

### Debito dichiarato (mantra #3, non ancora raggiunto)

`tr/2` è **binario**, quindi rappresenta una sola lingua seconda, e per questo
`gloss_language/1` ha un membro solo. Il punto fisso vero è
`translation(Lingua, Inglese, Forma)`: finché `tr` resta binario, aggiungere una
lingua a quell'elenco produrrebbe un fatto **falso** (un gloss francese letto
come italiano), non crescita. La riga si aggiunge il giorno in cui `tr` prende
l'argomento della lingua.

## 5. Gate — capacità 1: il gloss insegnato

`tests/p0t/language/taught_gloss.p0t`, 15/15.

| controllo | esito |
|---|---|
| baseline | muro su «quale colore si usa in chess» |
| lezione | `usa is the italian of used` — lingua naturale, zero schema |
| replay | la stessa frase → **Bianco e nero.** |
| transfer@2 | `lati`+`triangolo` → «quanti lati ha un triangolo» → *A triangle has 3 sides.*; `organo`+`produce` → «quale organo produce la bile» → **Fegato.** |
| contrasto | `rome is the capital of italy` non diventa un gloss: `capital` non è in `gloss_language/1`, e la canonicalizzazione resta invariata |
| ablation | `!forget italian(usa, used)` richiude la domanda nello stesso processo |
| composizione | il gloss si compone con relazioni già possedute (`side_color`, `sides_of`, `produces`) senza una riga `answer_frame` nuova |

**Transfer@3 = 2/3**: due famiglie held-out verdi, la terza (`quale fiume
attraversa parigi`) scartata perché la *risposta inglese* è già sbagliata
(«France. seine runs through it.») — un difetto suo, non del gloss. Quindi la
capacità è promossa come **parziale** sul gate delle capacità generali, non come
chiusa.

## 6. Gate — capacità 2: la forma della domanda senza schema

Residuo di M15: il gen457 rendeva insegnabile `answer_frame` solo nominando la
relazione (`… as a way to ask side_color`), cioè chiedendo a chi insegna di
conoscere il nome interno del predicato — precisamente ciò che il vincolo zero
vieta.

La forma senza schema àncora la formulazione nuova a **una domanda che già
funziona**; relazione e verso si deducono dal modello:

```
> which shades does chess use
Hmm, I don't know about shades yet…
> learn "which shades does" as another way to ask "which colors are used in chess"
Got it - «which shades does» now asks the same thing as «which colors are used in chess».
> which shades does chess use
White and black.
```

`tests/p0t/language/taught_question_form.p0t`, 7/7: replay, ablation, contrasto,
e la prova che la forma storica resta viva accanto alla nuova.

### Il misclaim trovato, e la guardia che ne è nata

Prima versione della guardia: «il modello contiene una superficie dichiarata».
È permissiva come `cue()`, che è substring — mantra #8 — e infatti:

```
learn "the head town of" as another way to ask "what is the flurb of france"
->  Got it …                                    ← FALSO
```

Un modello inesistente veniva accettato e la lezione ereditava una relazione
presa per sbaglio: un frame falso, peggio di un muro. La guardia corretta è
**conclusiva** e la fa il motore vero: il modello deve essere una domanda a cui
`mod_answer_frame` risponde davvero. Non è una duplicazione — il consumer chiede
«quali candidate potrebbero valere» e lascia decidere l'evidenza, chi insegna
deve sapere se il modello *risponde*.

## 7. Il mantra ignorato, e la correzione

F. l'ha fermato a metà: per capire quale relazione interroga il modello avevo
riscritto a mano «trova nel testo la superficie dichiarata più lunga» — che è
**esattamente** la risoluzione già dentro `mod_answer_frame`. Mantra **#5**,
`grep` prima di scrivere, nella stessa forma del `transitive_comparison`
duplicato.

Correzione: la risoluzione è stata estratta in un motore solo,
`answer_frame_surfaces` (`src/brain/00-lex.c`), e ora la usano entrambi i
lettori — chi risponde a un turno e chi insegna una parafrasi. Il codice netto
del gen490 su questo punto è **negativo**.

## 8. Conteggio

Sessione di **meta-capacità**: nessun `/save`, nessun fatto di sessione promosso
(gli unici fatti prodotti nei giri sono glossi veri, ma sono stati usati come
prove di transfer in processi di test e non persistiti — la KB versionata cresce
solo del ponte e delle prove).

```
Nuovi fatti veri del mondo salvati in KB (W): 0
Nuove clausole totali salvate e classificate: 2   (gloss_language/1 + tr/2 il ponte)
Clausole dichiarate da /save (S): —  (nessun /save: sessione meta)
Costruzioni/regole/procedure nuove (C): 2
Clausole invalide (X): 0
```

## 9. Metriche

```
LessonYield            = 2/2
Transfer@3             = 2/3 (gloss) · 1/1 (forma della domanda)
Paraphrase             = n/a
ContrastPrecision      = 2/2  (capital non è un gloss; il modello inesistente è declinato)
Composition            = 1/1
AblationFidelity       = 2/2
FreshProcessRecall     = n/a (nessuna persistenza di sessione da verificare)
FalseUnderstandingRate = 0   (il misclaim del §6 è stato chiuso prima del commit)
```

Gate software: `make soft-test` **rosso su un caso solo**
(`frontier_chat_audit.it.p0t`, «designation»), **rosso già prima** di questa
sessione — verificato su albero pulito. 7,4 s → 8,0 s: il ponte `tr/2` non
sfonda il budget.

## 10. Gap rimasti, tipizzati

1. **M16 resta aperto e ora si vede meglio.** «quanti lati ha un triangolo»
   risponde *A triangle has 3 sides.* — comprensione italiana, voce inglese. Il
   gloss apre la domanda, non la risposta: le famiglie `response_template`
   italiane restano 141 su 854.
2. **`tr/2` è binario** (§4): il punto fisso `translation(Lingua, …)` non è
   raggiunto, ed è la condizione per una terza lingua.
3. **Il gloss non ha morfologia.** `produce`→`produces` e `usa`→`used` sono due
   fatti separati; una lezione sola non apre le forme flesse (è M5).
4. **«quale fiume attraversa parigi»** ha la risposta inglese sbagliata
   («France. seine runs through it.»): un misclaim indipendente, registrato qui
   perché è emerso cercando gli held-out.
