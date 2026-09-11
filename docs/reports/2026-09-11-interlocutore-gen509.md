# gen509 — Addestrare parrot0 come interlocutore: i limiti dell'apprendimento

**Data:** 2026-09-11
**Sistema:** `gen508@faf665f`, poi le due riparazioni del gen509 (§6)
**Metodo:** soltanto `make chat`, con i turni mandati sulla stdin e le risposte
lette una per una. Nessuna suite, nessun `.p0t`, nessun `!assert`.
**Stato finale:** `partial` — 19 fatti veri del mondo salvati e riletti da un
processo nuovo, una domanda di verifica su dodici fallita (§5). Le riparazioni
del gen510 sono nel §7.

## 1. Che cosa si è cercato di fare

L'obiettivo era avvicinare parrot0 a un interlocutore paragonabile a un LLM,
insegnandogli **parlando** e usando le abilità più recenti: le superfici di
ordine superiore del gen507 e le due strutture del gen508 (la definizione come
espressione, il fatto a ruoli aperti).

In questo report **un limite non è un prompt a cui parrot0 non ha saputo
rispondere**. È una di due cose:

- **una lezione che non ha potuto imparare** (sigla `NL`): la frase del maestro
  non entra, entra deformata, o viene presa da un altro lettore;
- **una lezione imparata e poi non messa in pratica** (sigla `NA`): il fatto è
  nella KB, ma la domanda naturale che dovrebbe usarlo non ci arriva, o ci
  arriva e sbaglia.

Tutte le lezioni usano fatti veri e stabili. Le fonti di riferimento sono le
voci enciclopediche indicate nel §3; non sono state riscaricate durante la
sessione, perché i fatti usati sono date e attori consolidati.

## 2. Le sessioni

| Sessione | Contenuto | Salvata |
|---|---|---|
| 1 | parentele della famiglia reale britannica con nomi completi, definizioni di parentela | no |
| 2 | quattro battaglie come eventi a ruoli, la proiezione «defeated» | no |
| 3 | condotta da interlocutore: sigla, definizione, composizione, piano, antonimo, ripresa del discorso | no |
| 4 | parentele con nomi di una parola, per separare il difetto dei nomi da quello delle definizioni | no |
| 5 | nomi di battaglia completi; ritrattazione dopo la riparazione | no |
| 6 | lezioni prima delle domande; nomi di più parole in posizione di valore | no |
| 7 | parentele reali della famiglia Curie con nomi completi | no |
| 8 | **la sessione di lezione**: soltanto lezioni già viste apprese e usate, poi `/save` | **sì** |
| 9 | verifica in un processo nuovo, senza ripetere lezioni | no |

Le sessioni esplorative non si salvano per una ragione precisa: una domanda che
fallisce, con `make chat`, fa partire una ricerca su Wikipedia, e ciò che la
ricerca estrae entra nella sessione. Nella sessione 1 la ricerca ha appreso
`located_in(charles, english_and_french_speaking_countries)` da «Charles is a
male given name predominantly found in English and French speaking
countries»: un fatto falso. Salvare quella sessione avrebbe persistito il falso.

## 3. Che cosa parrot0 ha imparato e usato

### Il fatto a ruoli aperti

```text
> a battle involves a winner / a loser / a year · a battle requires a winner
> waterloo is a battle
> the winner of waterloo is arthur wellesley
> the winner of waterloo is gebhard von blucher
> the loser of waterloo is napoleon
> the year of waterloo is 1815
> defeated links the winner of a battle to its loser
> does arthur wellesley defeated napoleon?        Yes.
> does gebhard von blucher defeated napoleon?     Yes.
> does napoleon defeated mikhail kutuzov?         Yes.   (Austerlitz)
> does napoleon defeated arthur wellesley?        I don't know …
> what does a battle involve?                     A battle I hold 3: has a loser, has a year, requires a winner.
```

La proiezione **conserva l'identità dell'istanza**: Napoleone vince ad
Austerlitz e perde a Waterloo, e i due ruoli non si mescolano. «napoleon
defeated napoleon» resta senza risposta, come deve. Il ruolo del vincitore
accetta due valori, e il nome di più parole in posizione di valore si legge.

### La definizione come espressione

```text
> parent holds wherever mother · parent holds wherever father
> grandparent is parent twice
> grandchild is grandparent read backwards
> great_grandparent is grandparent followed by parent
> does elizabeth great_grandparent george?      Yes.     (tre definizioni annidate)
> sibling is child followed by parent
> tell me about the relation sibling
  About «sibling» I hold 2: goes both ways, defined as child followed by parent.
> forget that grandparent is parent twice       Forgotten: grandparent is no longer defined that way.
```

La simmetria di «sibling» **non è stata insegnata**: segue dalla forma
`then(flip(parent), parent)`. Le definizioni si annidano a tre livelli, e
«what can you say about charles?» vede anche le relazioni definite. Queste
prove usavano nomi di una parola (sessione 4); i fatti di quella sessione non
sono stati salvati, per le ragioni del limite NL1.

### Composizione di un materiale

`bronze contains copper`, `bronze contains tin` → «what is part of bronze?»
risponde «Copper and tin.», anche nel processo nuovo.

### Fonti

Voci enciclopediche di riferimento: *Battle of Waterloo*, *Battle of
Austerlitz*, *Battle of Trafalgar*, *Battle of Hastings*, *Bronze*. Per le
lezioni non salvate: *Elizabeth II*, *Charles III*, *William, Prince of Wales*,
*Marie Curie*, *Irène Joliot-Curie*, *Ève Curie*, *Pierre Joliot*, *NASA*,
*Photon*.

## 4. I limiti

### 4.1 Lezioni che non ha potuto imparare

| | Lezione del maestro | Che cosa succede |
|---|---|---|
| **NL1** | «charles iii is the father of william» · «king charles is the father of william» | il soggetto di due parole perde l'oggetto: diventa `father(charles_iii)`, «charles iii is a father». Con nomi di una parola la stessa forma funziona. Le parentele reali si possono dire solo accorciando i nomi, e un nome accorciato («charles») è ambiguo |
| **NL2** | «the battle of waterloo is a battle» · «the winner of the battle of waterloo is arthur wellesley» | «of» dentro il nome viene letto come una relazione, oppure parte una ricerca su «battle». Le battaglie si possono salvare solo con il nome breve, `waterloo`, che è anche il nome di una città: un'identità sola per due cose |
| **NL3** | «a battle involves a winner», detta dopo un turno fallito | un chiarimento rimasto aperto su «winner» (Reality Winner, Michael Winner …) si prende la lezione e le due successive. Detta come primo turno, la stessa lezione entra. Così anche «lost_to is defeated read backwards» è stata presa dal chiarimento su «defeat». **L'apprendimento dipende dall'ordine dei turni** |
| **NL4** | «photon is defined as the elementary particle of light» | la definizione del maestro non viene salvata: parte una ricerca, e al suo posto entrano quattro fatti estratti dalla prosa, fra cui `massless_particle(photons)`. Subito dopo, «what is the definition of photon?» risponde «definition is state what the concept is in one clear sentence.» |
| **NL5** | «sibling is child followed by parent» | non esiste una superficie per dire «purché siano persone diverse». La definizione tiene, ma rende ognuno fratello di sé stesso: «does william sibling william?» → «Yes.». Per questo non è stata salvata |
| **NL6** | «the mother of irene joliot-curie is marie curie», dopo «mother is a relation» | la lezione entra, come `mother_of(irene_joliot-curie, marie_curie)`. Ma «marie curie is the mother of …» produce un altro predicato, `mother`, e «parent holds wherever mother» raggiunge solo quello. Chi insegna non può nominare `mother_of`. Due dialetti per la stessa relazione, e nessuna lezione naturale per unirli |
| **NL7** | «when you don't have the steps then say what it is made of» | le situazioni di un piano sono chiuse: ne esiste una sola, e questa mossa c'era già. La lezione accoda un doppione come mossa 3, e la conferma arriva in italiano |
| **NL8** | qualunque lezione con una parola nuova, in `make chat` | la ricerca automatica può apprendere dalla prosa un fatto falso (il caso di «charles», §2). Il maestro non vede l'errore, perché la risposta dice solo «I extracted 1 facts» |

### 4.2 Lezioni imparate e poi non messe in pratica

| | Appreso | Uso che fallisce |
|---|---|---|
| **NA1** | `winner_of(waterloo, arthur_wellesley)` | «who is the winner of waterloo?» → «I don't know about winner». «what is the year of waterloo?» → «1815.»: lo stesso lettore risponde per un ruolo e non per l'altro |
| **NA2** | grandparent come «parent twice» | «is elizabeth the grandparent of william?» → **«No.»**, mentre «does elizabeth grandparent william?» → «Yes.». Anche «is william a sibling of harry?» → «No.». La forma di domanda più naturale non consulta la definizione e **risponde il falso**. È il limite più grave del report |
| **NA3** | grandparent, grandchild | «who is the grandparent of william?» → «Nobody that I know of». «who are the grandchildren of elizabeth?» → non conosce «grandchildren». Le domande aperte e i plurali non raggiungono le definizioni |
| **NA4** | la relazione «defeated» | «did wellington defeat napoleon?» → non conosce «defeat». Il nome della relazione è la parola esatta della lezione; nessuna forma verbale la ritrova |
| **NA5** | `winner_of(hastings, william_the_conqueror)` | nel processo nuovo, «does william the conqueror defeated harold godwinson?» → «I don't know about william». Un nome con «the» dentro si salva come valore e non si legge come soggetto |
| **NA6** | «nasa is short for national aeronautics and space administration» | «what does nasa stand for?» cerca «national»; «what is nasa?» non usa la sigla |
| **NA7** | `part_of(copper, bronze)`, `part_of(tin, bronze)` | «what is bronze made of?» → non conosce «bronze». Con la lezione di piano, «how is bronze made?» fa partire una ricerca invece di eseguire la mossa «say what it is made of». «what is part of bronze?» invece risponde |
| **NA8** | «hot is the opposite of cold», e `opposite(cold, hot)` era già in KB | «what is the opposite of hot?» → «Cold.»; «what is the opposite of cold?» → un paragrafo sulla Guerra fredda |
| **NA9** | il piano per `steps_missing` | «your plan when you don't have the steps?» → sei punti generici sulla progettazione di sistemi: un altro lettore prende la domanda e risponde qualcosa di non pertinente |
| **NA10** | lezioni in inglese a ruoli | «a battle involves a year» → «Tengo: un battle ha un anno.»: la conferma passa all'italiano e traduce il nome del ruolo. La trascrizione salvata conserva le conferme italiane |
| **NA11** | fatti a ruoli su Napoleone | «what can you say about napoleon?» → «defeated» nella sessione 2, e nella sessione 6 → «I don't have any of my own -- I'm parrot0, an AI»: la stessa domanda cambia lettore secondo i turni precedenti |

### 4.3 Che cosa hanno in comune

Tre cause spiegano quasi tutti i limiti.

1. **Il nome di più parole.** In posizione di valore si legge («arthur
   wellesley», «pierre-charles villeneuve»); in posizione di soggetto o di
   istanza no (NL1, NL2, NA5). È la stessa lacuna del «iron ore» del
   2026-09-05: il referente di più parole non ha un'identità stabile.
2. **Le domande consultano un sottoinsieme delle viste.** La polare «does X V
   Y?» passa da `holds/3` e vede definizioni e proiezioni; la polare «is X the
   R of Y?», le domande aperte e i plurali no (NA1–NA4). La conoscenza è salva e
   la maggior parte delle domande non la raggiunge. Dove la vista mancante
   produce «No.» invece di «non so» (NA2), il limite diventa un falso.
3. **Il turno lo prende chi arriva primo.** Chiarimenti rimasti aperti,
   ricerche, lettori di registro e di chiacchiera catturano lezioni e domande
   secondo l'ordine della conversazione (NL3, NL4, NA9, NA11). Una lezione
   funziona o no a seconda di che cosa è stato detto prima.

## 5. La sessione salvata e la verifica

**Boot della sessione di lezione:** `B0 = 44391` fatti, `R0 = 3050` regole.
**`/save`:** `parrot0: routed 131 clause(s) into the KB tree` → `S = 131`.

| Categoria | Conteggio | Clausole |
|---|---:|---|
| fatti veri del mondo `W` | **19** | `part_of` ×2, `battle` ×4, `winner_of` ×5, `loser_of` ×4, `year_of` ×4 |
| linguistici `L` | 7 | `relation_verb` ×6, `class_surface(battle, battle)` |
| costruzioni `C` | 11 | `relation_role` ×3, `required_role`, `relation_projection(defeated, …)`, `relation_or` ×2, `relation_twice`, `relation_reverse` ×2, `relation_chain` |
| provenienza `P` | 34 | `fact_source` ×17, `reading_fact` ×17 |
| altre `O` | 60 | `utterance` della trascrizione |
| invalide `X` | **0** | |

Totale classificato: 19 + 7 + 11 + 34 + 60 = 131 = `S`.

```text
Nuovi fatti veri del mondo salvati in KB: 19
Nuove clausole totali salvate e classificate: 131
Clausole dichiarate da /save: 131
Clausole invalide: 0
```

**Collocazione.** `/save` ha messo le due clausole del bronzo in
`kb/experts/medicine/anatomy.p0`, il primo file che usa `part_of/2`. Sono
state spostate a mano, identiche, in `kb/core/world-facts.p0`. È il difetto
già descritto in LEARN_PROTOCOL.md, «conoscenza vera nel posto sbagliato».

**Processo nuovo:** `B1 = 44588`, `R1 = 3050`. Dodici domande sui fatti salvati
e un contrasto, senza ripetere lezioni:

| Domanda | Risposta |
|---|---|
| what does a battle involve? | has a loser, has a year, requires a winner |
| describe waterloo | waterloo is a battle. |
| what is the year of trafalgar? | 1805. |
| does arthur wellesley defeated napoleon? | Yes. |
| does napoleon defeated mikhail kutuzov? | Yes. |
| does horatio nelson defeated pierre-charles villeneuve? | Yes. |
| does william the conqueror defeated harold godwinson? | **fallisce** (NA5) |
| what is part of bronze? | Tin and copper. |
| is copper part of bronze? | Yes. |
| tell me about the relation great-grandparent | defined as grandparent followed by parent |
| tell me about the relation grandchild | defined as grandparent read backwards |
| how is grandparent defined? | parent followed by parent |
| contrasto: does napoleon defeated arthur wellesley? | I don't know … (corretto) |

```text
FreshProcessRecall     = 11/12
ContrastPrecision      = 1/1
AblationFidelity       = 1/1   (sessione 5, dopo la riparazione del §6)
FalseUnderstandingRate = 0 nella sessione salvata; nelle esplorative «No.» falsi in NA2
```

La recall non è al 100%, quindi lo stato è `partial`, non `trained`.

**Un limite della crescita salvata.** Le definizioni di parentela sono vere e
sono state viste funzionare nella sessione 4. Oggi la KB non contiene fatti
reali di madre o padre che esse possano usare, e i fatti reali con nomi
completi non si possono insegnare (NL1, NL6).

## 6. Due difetti del gen508, trovati insegnando e riparati nel gen509

| Difetto | Come si è visto | Riparazione |
|---|---|---|
| i predicati d'appoggio del gen508 non erano dichiarati di macchina | «describe waterloo» → «waterloo is a base_expr. waterloo is a occurrence_complete.» | `machinery/1` per i diciassette aiutanti, in `kb/core/procedures.p0` |
| lo slot `expr` univa più parole in un nome solo | «forget that grandparent is parent twice» → «Held: forget that grandparent is parent applied twice.», con la relazione `forget_that_grandparent` | un nome nudo in un'espressione è una parola sola, come lo `slot` del gen507 (`p0_relation_expr`) |

Dopo le riparazioni, «describe waterloo» → «waterloo is a battle.», e la
ritrattazione della definizione funziona (sessione 5).

## 7. gen510 — che cosa è stato riparato, e che cosa resta

Dopo il report, le tre cause del §4.3 sono state affrontate nel gen510. Lo
stato di ogni limite è stato misurato con `make chat` in due sessioni di
verifica non salvate; il dettaglio operativo e le prove da rifare sono
nell'handoff in testa a `TEST_TODO.md`.

| Limite | Stato | Come |
|---|---|---|
| NL1 soggetto di più parole | **riparato, verificato** | cornici `@S is the R of @O` derivate da `family_relation/1` e da `relation_noun/2` (grammar.p0) |
| NL3 lezione presa da un'offerta aperta | **riparato** | un'offerta si accetta nominandone il tema solo con una risposta breve (`offer_reply_max_words/1`, network.p0) |
| NL4 definizione presa dalla ricerca | **riparato, verificato**; resta NA | «photon is defined as …» ora si salva; «what is a photon?» non la usa ancora |
| NL5 nessuna superficie per «persone diverse» | **riparato** | «V never holds of itself» (irriflessiva) + `dif` in `sibling_of/2`; da riverificare |
| NL6 due dialetti `mother` / `mother_of` | **riparato, verificato** | ponte in `holds/3`: «X is the R of Y» = `R_of(Y, X)` nel verso dichiarato |
| NL8 fatto falso da una frase su un nome | **riparato, non verificato** | `metalinguistic_head/1` spegne il luogo nell'estrattore delle classi |
| NA1 «who is the winner of waterloo?» | **riparato, verificato** | la domanda aperta chiede a `holds/3` quando il predicato nudo tace |
| NA2 «No.» falso su «is X the R of Y?» | **aperto** | il lettore «R of» ora usa la scala del verdetto condivisa, ma la domanda con soggetto di più parole la prende un altro lettore che risponde ancora «No.» |
| NA3 domande aperte e plurali | **riparato, verificato** | «who are the grandchildren of elizabeth ii?» → «william, harry.» |
| NA5 soggetto con «the» interno | **riparato, verificato** | `known_referent/1` prima della guardia per parola |
| NA6 sigla non usata | **riparato, verificato** | forma «what does X stand for?», anche sulla sigla già espansa |
| NA7 «what is bronze made of?» | **riparato, verificato** | `has_part` è `part_of` visto dal contenitore; cornice «made of» |
| NA8 antonimo → Guerra fredda | **aperto** | la guardia `compound_guard(semantic_summary, lexical_relation_request)` non ha effetto |
| NA9 piano raccontato come saggio | **aperto** | le guardie su `analysis_family` e `analysis_plan` non hanno effetto |
| NA10 conferma in italiano | **riparato, verificato** | marcatori d'inglese per il lessico delle lezioni |
| NA11 «what can you say about X?» | **riparato, verificato** | `about/3` legge i ruoli; la forma dice «non tengo niente» invece di cedere |
| NL2, NL7, NA4 | **aperti** | nome con «of» interno; situazioni di un piano chiuse; forme verbali del nome di relazione |

Anche un rosso della suite, preesistente, è stato chiuso: «what is the
opposite of hot», senza «?», veniva letta come lezione (`basics.p0t`). Il
modo del turno ora consulta la lettura pubblicata dell'illocuzione.

## 7-bis. gen510, secondo giro — i muri trovati da F. e lo strato delle domande

F. ha trovato due muri in `make chat`: «di dove sei» e «se sto cadendo da un
burrone senza paracadute cosa succedera». La domanda che ne è seguita — *come è
possibile, dopo tanti livelli di astrazione?* — ha una risposta misurata.

**Le astrazioni sono motori; i muri erano a monte e a valle dei motori.**
Con `/debug` e con la forma canonica ora tracciata (`P0_READ_TRACE=1`):

| Turno | Forma canonica | Forza del turno | Che cosa mancava |
|---|---|---|---|
| «di dove sei» | «of where are» | `expressive` | la domanda non era riconosciuta come domanda, la frase non aveva traduzione, parrot0 non sapeva nulla della propria origine |
| «se sto cadendo da un burrone senza paracadute cosa succedera» | «se sto cadendo from a burrone senza paracadute what succedera» | `expressive` | domanda non riconosciuta, nessuna conoscenza causale sulle cadute, nessun lettore «what happens if X» da `causes/2` |

Le definizioni componibili e i ruoli non possono lavorare su un turno che non
arriva a nessun lettore, né su una catena causale che la KB non contiene.

**Riparato in questo giro**

| Che cosa | Come | Addestrabile parlando? |
|---|---|---|
| Una domanda aperta da una preposizione («di dove», «da dove», «from where») | regola strutturale in grammar.p0 + `question_preposition/1` | sì, la classe è KB |
| Una condizione con un interrogativo («se …, cosa succede») | regola in turn-frames.p0 sullo span `condition` | la classe `interrogative_pronoun/1` è KB |
| «Tieni conto che se un turno contiene X allora è una domanda» | forme `teach_question_cue` (inglese e italiano) → `taught_question_cue/1` | **sì**: è la lezione chiesta da F. |
| Le lezioni raggiungono il loro lettore | `mod_lesson_form` + `turn_form_priority/2`: le forme dichiarate si leggono prima dei lettori generici | la priorità è un fatto KB |
| L'origine di parrot0 | `self_origin` + l'elenco delle domande su di sé portato dall'array C a `self_question_intent/1` | le forme italiane si insegnano: «"di dove sei" is another way to say "where are you from"» → verificato, risponde in italiano |
| Rese naturali | `say_frame_preferred/2`: «tom is the parent of bob» | KB |
| «Kim» e il «No.» senza licenza | `answer_frame_defers/1`; scala del verdetto condivisa | KB |
| Il costo di un turno qualunque | `construction_frame` raccolto una volta per chiamata | — |

**Resta aperto**: il lettore delle conseguenze (il burrone è ora una domanda, ma
parrot0 non sa che cosa succede cadendo da un'altezza, e non ha un lettore che
risponda da `causes/2`); il piano di traduzione del muro proposto da F.; i rossi
preesistenti elencati in `TEST_TODO.md`. Il concetto delle **radici
dell'insegnabilità** nato in questo giro è in
[docs/plans/radici-insegnabilita.md](../plans/radici-insegnabilita.md), con
questi casi come esempi lavorati.

## 7-ter. gen510, terzo e quarto giro — le conseguenze, e un anello rotto nel lettore delle forme

**Il burrone ha una risposta.** Nel processo nuovo, senza ripetere lezioni:

```text
> se sto cadendo da un burrone senza paracadute cosa succedera
  Da quello che ho imparato: a fall from height causes a violent impact, e a
  violent impact causes serious injury.
> what happens if you fall from a cliff?
  From what I've learned: a fall from height causes a violent impact, and a
  violent impact causes serious injury.
> what happens if you eat a cloud?
  I don't know yet what happens in that case. If you tell me what it causes --
  «X causes Y» -- I'll hold it and use it next time.
```

La catena attraversa soltanto lezioni dette: «cadendo» e «cado» sono *fall*
(`tr/2`, ora una forma di `linguistic_form/4`), «burrone» è *cliff*, *cliff* è
una specie di *height* (`kind_of`), e `causes` a due passi. La risposta usa le
frasi con cui le cause sono state insegnate (`fact_source/3`).

| Pezzo | Dove | Nota |
|---|---|---|
| Produttore delle conseguenze | situation.p0, contratto `turn_plan_candidate/1` + `turn_response/2` | indizi EN/IT; evento riconosciuto se ogni sua parola compare nel turno, come forma o per specie |
| Risposta onesta senza conoscenza | situation.p0 | invita alla lezione «X causes Y» |
| `atom_words/2` | kb.c, primitiva del solver | divide un atomo nelle sue parole: una **radice**, nel senso di radici-insegnabilita.md |
| L'analisi cede alle affermazioni e alle domande di conseguenza | intents.p0 | una lezione causale non riceve piu' un saggio |
| Preposizione interna nei sintagmi | guardia del soggetto e `p0_atom_is_concept` | «fall from height» e' un concetto |
| **Il limite dello slot dopo l'articolo** | lettore delle forme | **preesistente**: ogni forma che apre con uno slot falliva davanti a un soggetto con l'articolo («a cliff is a kind of height») |

**Crescita salvata** (`/save`: 25 clausole): W = 3 (`causes` ×2,
`kind_of(cliff, height)`), L = 4 (`tr` per burrone, cado, cadendo,
paracadute), P = 4, O = 14, X = 0.

**Un test cambiato con motivo**: `higher_order_lesson.p0t` usa «encloses»
invece di «contains». Passava solo grazie al difetto dello slot: con la KB
completa «X contains Y» ha gia' la lettura `part_of(Y, X)` (gen507/76).

**Limiti aperti di questo tratto**: la risposta italiana cita frasi inglesi,
perche' le cause sono state insegnate in inglese; «senza paracadute» non entra
ancora nella risposta (manca la relazione di attenuazione); gli atomi di una
causa restano entro tre parole.

## 8. Da dove ripartire

Le tre cause del §4.3 sono tre classi di lavoro, non trentatré prompt:

1. un'identità per il referente di più parole, in posizione di soggetto e di
   istanza;
2. le domande «is X the R of Y?», «who is the R of X?» e i plurali devono
   passare dalla stessa vista `holds/3` della polare, e un'assenza deve dire
   «non so», non «No.»;
3. un chiarimento o una ricerca rimasti aperti non devono prendersi il turno
   di una lezione.

Lo spazio negativo di questa sessione — le lezioni che non entrano — è la
lista in §4.1. Nessuno di questi limiti è stato trasformato in un test.
