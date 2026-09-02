# Comprensione universale

*Estensione operativa del manifesto [kb-first](kb-first.md): parrot0 non dovrebbe
mai murare una frase **ben formata** con un "non capisco" cieco. Anche quando le
parole sono ignote, la **struttura grammaticale** è estraibile, e da quella si
legge l'**intento**. La grammatica è motore fisso; le strutture e i ruoli vivono
nella KB.*

> **Stato (2026-07-02, gen267–268):** il primo incremento (§2, declino informato)
> è VIVO per la forma request/generate — `mod_reqgen` legge "[V-imp] [NP]
> (in <lang>)" con verbo e lingue come classi `intent_cue`, registrato in coda
> (non ruba mai a un modulo competente, cattura solo gli ex-muri). E il primo
> gradino di §8 è VIVO: `program_shape(print_message)` + `program_output/2`
> (hello_world come conoscenza del mondo) → "crea un semplice hello world in c"
> produce codice VERIFICATO dall'esecuzione (stdout esatto, exit 0). La scala
> completa è dimostrata su un prompt: muro cieco → declino informato → artefatto
> verificato. Prossimi gradini: `intent_schema/2` oltre la forma imperativa
> (§4.2), più program_shape tirati da RULESCORE/game-bench, il declino informato
> come default per le ALTRE forme (§5c).

> **⛔ PROTOCOLLO OPERATIVO — vedi [§10](#10-protocollo--le-tre-specie-di-lacuna-e-quali-si-chiudono-prima-che-qualcuno-chieda).**
> Le tre specie di lacuna (variante di superficie / costruzione mancante / forma
> telegrafica), il test diagnostico che le distingue in tre turni, e le tre
> strategie corrispondenti. **Due specie su tre si chiudono senza mai vedere una
> chat**; la terza non si anticipa, si rende insegnabile. Comprensione universale
> non vuol dire aver previsto tutto: vuol dire che ciò che è generabile dalla
> struttura è già chiuso, e ciò che non lo è si chiude parlando.

> **Principio evolutivo (F., gen240): tieni le strutture secondarie.** Finché
> qualcosa non si dimostra *prevalente*, tutto è secondario e **attivabile**: non
> si rimpiazza né si pota una struttura solo perché *sembra* ridondante o legata a
> casi minori — domani il secondario può tornare importante. È un organismo che
> evolve per selezione naturale: si accumulano le varianti, e col tempo prendono
> forma migliore. Conseguenza pratica: cambi **additivi** (un nuovo modulo che si
> *stratifica* sopra, mai che cancella). Il dispatch first-match rende innocua la
> ridondanza — il più generale vince quando si applica, lo specifico resta
> fallback. (gen240 `mod_smalltalk` è registrato per ultimo e non tocca
> `mod_chitchat`.) Vedi [[keep-secondary-structures]].

---

> ## ✅ TRAGUARDO — LO SPAZIO DEL DISCORSO (2026-08-31)
>
> **Da conservare: è la prima volta che parrot0 ricorda *che cosa* è stato
> nominato e *in che ordine*, e che un'espressione può riferirsi a quella
> memoria invece che a una parola.**
>
> ```text
> > Il libro rosso è sul tavolo.      ->  located_in(book_red, tavolo)
> > Il quaderno blu è sulla mensola.  ->  located_in(quaderno_blue, mensola)
> > Dov'è il primo?                   ->  tavolo        (prima: muro)
> > Dov'è il secondo?                 ->  mensola       (prima: muro)
> ```
>
> ### Che cos'è, esattamente
>
> Poco, di proposito: `discourse_referent(Ordine, Chiave)`. Una cosa nominata e
> la sua posizione nel discorso. Testa e proprietà si ricavano dalla chiave (G2);
> determinante, span e superficie originale non ci sono ancora (G5).
>
> Ma è la prima **memoria del dialogo che non è una lista di frasi**: è una lista
> di *cose*. Fino a ieri parrot0 conservava turni; ora conserva referenti.
>
> ### A che cosa serve, e che cosa abilita
>
> Non è una funzione in più: è il posto a cui si attaccano cose che prima non
> avevano appiglio.
>
> | abilita | perché prima non si poteva |
> |---|---|
> | **coreferenza** — «quello», «l'altro», «quello di prima» | non esisteva l'oggetto a cui riferirsi: F03 era la famiglia peggiore del corpus, 24 muri su 30 |
> | **ellissi** — «E il secondo?» senza ripetere il verbo | il turno ellittico non ha entità da nominare: deve prenderla dal discorso |
> | **correzione** — «no, quello rosso l'ho spostato» | correggere richiede di individuare *che cosa* si corregge, non solo che si corregge |
> | **ambiguità dicibile** — «Quale? …» invece di un muro | serve saper elencare i candidati, cioè averli |
> | **il soggetto eliso** (SC5) e **l'apposizione** | entrambi recuperano un ruolo da qualcosa già introdotto |
> | **la domanda di seguito** — «e dove si trova adesso?» | «adesso» presuppone una cosa di cui si stava parlando |
>
> Ed è la precondizione dichiarata di **GD4** (riferimento cross-turn) e di
> **D37/G4-G5**: il referente con proprietà, e il referente che sa ridirsi.
>
> ### Come è stato costruito — le tre cose che hanno deciso l'esito
>
> Vale più del risultato, perché sono riusabili:
>
> 1. **Il punto di strozzatura condiviso.** Le vie che imparano un fatto sono
>    più d'una — lo schema dichiarato, la copula binaria, il locativo — e la
>    prima versione agganciava i referenti a *una*. Misurato: il locativo
>    italiano non registrava niente, e metà del dialogo restava senza memoria.
>    L'osservazione sta ora in `p0_learn_source`, che **tutte** attraversano
>    perché registrare la provenienza è ciò che ogni via fa comunque. *Un
>    referente è esattamente questo: una cosa nominata, e quando.* Cercare il
>    punto che tutti attraversano invece di enumerare i chiamanti è la stessa
>    mossa della fase pura di SC2-B.
> 2. **Quale posizione introduca un referente è una politica, non una scelta del
>    C.** Registrando *ogni* argomento, «il secondo» diventava il **tavolo**
>    invece del quaderno: in «Il libro rosso è sul tavolo» sono nominati due
>    oggetti, ma quello di cui si parla è il primo. `referent_arg_position/1` lo
>    dichiara, e una relazione con un'altra geometria costa una riga.
> 3. **La superficie da dichiarare è quella che sopravvive al percorso.** «primo»
>    arriva al matcher come «prime» — la canonicalizzazione lo traduce — e la
>    forma col determinante non combacia più. È la **terza volta** che questa
>    lezione si presenta (dopo le cue di SC2-B e le locuzioni di SC2-D): finché
>    la canonicalizzazione non conserva anche l'originale (G5), una classe di
>    superfici deve tenere *entrambe* le forme.
>
> ### Il limite onesto
>
> È la **prima forma**, non la forma finale. Non c'è ancora il determinante (che
> distingue introdurre da riprendere), non c'è lo span, non c'è la superficie
> originale — quindi parrot0 sa che il libro rosso è stato nominato per primo e
> non sa ancora ridirlo «il libro rosso». E due referenti con la stessa testa si
> distinguono per proprietà (G2) ma non hanno ancora identità propria.


> ## ⛔⛔ IL CASSETTO SENZA MANIGLIA — il problema che teneva ferma la comprensione universale
>
> **Scoperto e misurato il 2026-08-31. Scritto in tutti i piani perché è la
> giunzione da cui dipendono `universal-input`, `universal-comprehension` e il
> fronte SC/GD insieme.**
>
> ### Il problema, in cinque righe di transcript
>
> ```text
> > Il libro rosso è sul tavolo.   ->  Learned: located_in(book_red, tavolo).
> > dove si trova il libro rosso   ->  muro
> > dove si trova book_red         ->  Tavolo.      ← solo col nome INTERNO
> > Il gatto è sul tetto.
> > dove si trova il gatto         ->  Tetto.       ← una parola sola: funziona
> ```
>
> Il **lettore** lega un *sintagma*: unisce i token fino al confine di sintagma e
> produce una chiave sola (`book_red`). La **domanda** provava un token alla
> volta — `located_in(il,?)`, `located_in(libro,?)`, `located_in(rosso,?)` — e
> non provava **mai** la frase intera. Il fatto c'era e non era raggiungibile.
>
> > **parrot0 imparava sotto un nome che non sapeva più pronunciare**, e ogni
> > entità di più di una parola finiva in un cassetto senza maniglia.
>
> ### Perché era il collo di tutto
>
> Due giri di insegnamento massiccio — 276 forme reali entrate parlando,
> verificate in processo nuovo — avevano mosso **+11 turni su 360**. Non perché
> il metodo fosse debole: perché un turno riesce solo se tengono **insieme**
> superficie, forma della domanda, nome dell'entità, riferimento, fatto e
> realizzazione, e il nome dell'entità cedeva sempre. I referenti multi-parola
> («il libro rosso», «il quaderno blu», «il treno notturno») sono la norma del
> parlato, non un caso limite — ed è anche il motivo per cui la coreferenza era
> la famiglia peggiore del corpus: non aveva **niente a cui attaccarsi**.
>
> `book_red` non è un nome scomodo: è un nome che ha **perso informazione**.
> Testa fusa col modificatore, determinante buttato, ordine invertito, lingua
> cambiata a metà — e ogni perdita chiude una porta diversa (chiedere «quale
> libro?», distinguere «un libro» da «il libro», risolvere «il primo»,
> ripronunciarlo come è stato detto).
>
> ### Il piano di soluzione — la giunzione in cinque gradini
>
> L'invariante che li governa tutti:
>
> > **ciò che si impara da una frase dev'essere interrogabile con la stessa
> > frase, e ridicibile come è stato detto.**
>
> | # | gradino | stato |
> |---|---|---|
> | **G1** | **La domanda prova i sintagmi che il lettore ha costruito.** Non un secondo indice: le *stesse tre cose* del lettore — confine di sintagma (`np_closer/1`, conoscenza), caduta del determinante, la stessa `p0_join`. Additivo: i passaggi per token restano. | ✅ **FATTO** 2026-08-31 |
> | **G2** | **Testa e proprietà.** «il libro rosso» → testa `libro` + proprietà `rosso`, così «il libro» combacia e «di che colore è il libro» risponde. Dov'è la testa è **conoscenza** (`noun_phrase_head_position(Language, first \| last)`), non una regola nel C. | aperto |
> | **G3** | **Il referente.** Un'entità introdotta occupa una **posizione nel discorso**: parrot0 ricorda che cosa è stato nominato e in che ordine, e «il primo»/«il secondo» ci si attaccano. Quale posizione di un fatto introduca un referente è una **politica** (`referent_arg_position/1`), non una scelta del C. | ✅ **FATTO** 2026-08-31 (prima forma: ordine + chiave; determinante e span restano G5) |
> | **G4** | **La coreferenza si attacca al referente.** «il primo», «quello», «l'altro» diventano `referent_same/3` — una **relazione**, non una fusione. | aperto |
> | **G5** | **Il referente sa ridirsi.** `referent_surface/3`: rispondere «il tavolo» come è stato detto, non `tavolo`. | aperto |
>
> ### Come si misura che funziona
>
> **Non con una percentuale sul totale: con una famiglia che chiude.** Il gate è
> che il dialogo `gd1_011` — cinque turni, due oggetti, un riferimento — passi
> **da capo a fondo**. Una catena che regge vale più di dieci punti sparsi,
> perché dieci punti sparsi non provano che nessuna catena regga.
>
> ### La forma ricorrente, che è la lezione vera
>
> È la **terza volta** che compare lo stesso difetto sotto un vestito diverso:
> D33 (un'interpretazione congelata perché la KB non può richiamare la lettura),
> D35 (una chiave costruita da un percorso e non dall'altro), D37 (una struttura
> costruita da un percorso e ignorata dall'altro).
>
> > **Due percorsi che devono accordarsi, e non condividono l'oggetto su cui
> > accordarsi.**
>
> Prima di aggiungere una capacità, la domanda da farsi è: *chi altro deve
> accordarsi con questa, e su che cosa?*
>
> E la stessa mossa, un piano più su, è **l'unificazione fra zone della KB**
> (D38, §18.43): far parlare aritmetica e sociale, prosa e geografia, non è
> ingegneria di dettaglio — è la condizione perché emergano abilità che nessuno
> ha progettato. Differenziarsi non basta: le parti differenziate devono potersi
> parlare.
>
> Dettaglio completo: `docs/plans/frontier-kb-natural-dialogue.md` §18.40 (D35),
> §18.42 (D37) · referto
> `docs/labs/apprendimento-assistito/2026-08-31-perche-non-cresceva.md` · coda
> `LEARN_TODO.md` GD9.


## 0. I due rami della comprensione universale

Una frase compresa non sempre richiede un *fatto*. Due rami, stessa radice ("capisci
la forma → niente muro cieco"):

- **Ramo conoscenza** (§1–§8): la forma chiede un dato. Se manca → declino informato
  e, se possibile, **acquire-knowledge** (corpus/fetch wiki). *Engine + KB facts.*
- **Ramo sociale** (gen240, `mod_smalltalk`): la forma è una mossa
  **conversazionale** (esperienza, opinione, domanda di rimando). Non serve un
  fatto: serve **continuità** — riconoscere la mossa e rilanciare (acknowledge +
  domanda aperta), onestamente, senza inventare esperienze. Cue e template di
  continuità vivono nella KB (`intent_cue(smalltalk_continue,…)`,
  `response_template(smalltalk_continue,…)`); il modulo è l'ultima rete prima del
  muro. È la stessa legge ("niente muro cieco") applicata al *register sociale*.

## 1. L'idea (gli appunti, sistemati)

In regime di comprensione universale vorremmo che **non esistano frasi su cui
parrot0 va a muro** per pura incomprensione. L'ipotesi:

- parrot0 sa **estrarre grammaticalmente la struttura di qualsiasi frase**;
- le **strutture** (schemi sintattici, ruoli, schemi d'intento) vivono **nella
  KB**, non cablate nel C;
- così una frase **ben formata ma con parole sconosciute** può comunque essere
  **interpretata nel suo intento**;
- è un *meccanismo analitico*: la lettura del significato si appoggia alla KB.

In una riga: **capire la forma è sempre possibile; la forma rivela l'intento;
l'intento dice cosa servirebbe sapere per rispondere.**

## 2. La distinzione che rende l'idea sana (e onesta)

L'idea così com'è rischia di collidere con la regola d'onestà dei PRINCIPLES
("declina quando la catena non regge"). La riconciliazione è una **distinzione
netta**, ed è il cuore della proposta:

> **Comprendere la FORMA ≠ saper RISPONDERE.**
> La comprensione strutturale è (quasi) sempre possibile. La risposta resta
> *gated* dalla conoscenza nella KB.

Quindi "non andare a muro" **non** significa "rispondere sempre" (sarebbe
l'impostore). Significa **eliminare il muro cieco** `"I don't understand that
yet."` e sostituirlo con un **declino informato** che *dimostra* di aver capito
la domanda e nomina esattamente l'anello mancante:

```
Q: What's the capital of Zembla?
muro cieco (oggi):   I don't understand that yet.
declino informato:   You're asking for the capital of a country (Zembla),
                     but I have no fact about Zembla yet.
```

Il declino informato è ancora un declino onesto — ma è *competente*: prova che la
struttura è stata letta. È lo stesso salto di qualità di gen237 (l'opposto), solo
generalizzato: il motore riconosce **la domanda** anche quando non possiede **il
dato**.

## 3. Perché è coerente col progetto

- **Engine fixed, lexicon learns.** La grammatica (estrazione struttura + ruoli)
  è un motore fisso; gli schemi sintattici, le parti del discorso e gli schemi
  d'intento sono **fatti KB** (come già `conjunction/1`, `stopword/1`,
  `intent_phrase/2`, `intent_cue/2`, `social_marker/2`). Aggiungere uno schema =
  un fatto, non codice.
- **È la chiusura del manifesto kb-first.** kb-first dice "prima di dire *serve un
  LLM*, decomponi e prova a derivare la struttura". La comprensione universale è
  quella decomposizione resa **default del parser**, non un'eccezione per-frase.
- **È colla linguistica** ([the-linguistic-glue](the-linguistic-glue.md)): leggere
  l'intento di una frase incompleta o con parole ignote è esattamente il "tessuto
  connettivo" descritto lì.

## 4. Il meccanismo (sketch a piccoli passi)

1. **Skeleton gerarchico.** Il motore di [[universal-input]] produce token span
   e sintagmi annidati (`NP`, `VP`, `PP`, `clause`); la KB assegna le parti del
   discorso (`pos/2`), i confini e i ruoli. Le parole ignote restano `unknown`
   ma **tengono il loro slot posizionale**.
2. **Schema della frase.** Riconosci lo *schema d'intento* dalla struttura dei
   sintagmi, non dal lessico: `[WH] [VP] [NP]` → domanda su una relazione;
   `[V-imp] [NP]` → intento `request/generate`; `[if] [clause], [clause]` →
   condizionale. Gli schemi sono fatti KB (`intent_schema/2`).
3. **Assegnazione ruoli.** Lega i sintagmi ai ruoli dell'intento (soggetto,
   relazione, oggetto, vincolo). Un content-word ignoto **può** riempire un ruolo:
   "capital of Zembla" → `relation=capital, object=Zembla` anche se `Zembla` è
   ignota.
4. **Risoluzione KB.** Prova a soddisfare l'intento dai fatti. Se la catena
   regge → rispondi. Se manca un anello → **declino informato** che nomina
   l'anello (passo 2 di onestà).
5. **Abduzione (costo/limite).** Per parole ignote in posizione di *predicato/
   relazione* nuova, la mappa va **abdotta** (ipotizzata + oracolo di
   plausibilità). Questo è il pezzo realmente difficile e va trattato come la
   classe "abduci" del manifesto, non come magia.

> **AGGIORNAMENTO gen412 — cosa manca, misurato.** «Le strutture vivono nella KB,
> non cablate nel C» è vero e verificato: `extract_frame/2` in grammar.p0 dice
> che aggiungere una forma estraibile costa **un fatto**. Ma questo piano assume
> anche il passo successivo — che le strutture ci si possa **arrivare parlando**
> — e lì la batteria di rinforzo ha misurato il taglio: la porta dialogica
> esiste solo per i fatti **unari** (una parola in una classe), mentre ogni
> struttura con **slot** è un fatto binario con pattern citato che nessun turno
> di conversazione sa dire. Il dettaglio, le tre strade provate e la direzione
> (indurre il pattern da un esempio invece di dettarlo) stanno in
> [`reinforcement-suite.md`](reinforcement-suite.md) §«Il meta-problema», e i
> casi rossi in `tests/rl/episodes/meta/`.

### 4bis. La stessa IR per comprendere e imparare dalla prosa

La span tipizzata `prose` non viene passata a un secondo parser speciale: viene
esplosa nella stessa gerarchia `token -> sintagma -> clausola` dell'input
universale. La comprensione interroga `intent_schema/2`; la lettura della prosa
interroga `extract_frame/2`; entrambe ricevono gli stessi slot, ruoli, offset e
provenance.

```text
"The okapi, a giraffid mammal, lives in Congo"
  -> NP(subject) + apposition(NP) + VP + PP
  -> extract_frame(...) dalla KB
  -> is_a(okapi, giraffid_mammal)
     located_in(okapi, congo)
```

Il motore deve conoscere solo tokenizzazione, nesting, matching strutturale e
binding; la KB deve poter aggiungere POS, chiusure di sintagma, marcatori di
apposizione e frame. Una nuova forma insegnata a runtime deve quindi cambiare
sia l'interpretazione sia l'estrazione, e la sua ablation deve farle sparire.

## 5. Cosa esiste già e cosa manca

- **Esiste:** classi chiuse in KB (`conjunction`, `stopword`), `intent_phrase`/
  `intent_cue`, dispatch first-match, coref/pragma/repair (la colla), il declino
  onesto. La materia prima c'è.
> ⛔ **Aggiornato il 2026-08-31.** Il punto (a) **non manca piu'**: la proiezione
> gerarchica esiste dal gen438 ed e' misurabile — per «Il libro rosso e' sul
> tavolo» parrot0 costruisce clausola radice, nodi sintagma e token con gli
> offset (`input_node/4`, `input_node_atom/3`). Quello che manca e' la
> **giunzione**: `extract_frame/2` non legge quella gerarchia, ricostruisce una
> stringa piatta con `p0_join` e produce `book_red` — testa e modificatore fusi,
> determinante perso, ordine invertito. Due percorsi paralleli, e vive quello
> che butta la struttura. Vedi `frontier-kb-natural-dialogue.md` §18.42 (D37).

- **Manca:** (a) ~~la proiezione gerarchica dell'`InputSpan` in token e sintagmi~~
  **fatta**; ma la giunzione fra quella gerarchia e il produttore di fatti no;
  (b) un vero `pos/2` lessicale in KB e un assegnatore di ruoli generico; (c)
  `intent_schema/2` applicato alla struttura, non solo a cue/frasi; (d) il
  **declino informato** come output di default al posto del muro cieco; (e)
  l'abduzione della mappa per predicati ignoti.

## 6. Verdetto — ha senso?

**Sì, con una correzione.** Il nucleo è giusto e profondamente in linea col
progetto: **separare struttura (grammatica) da contenuto (lessico)**, mettere la
struttura nella KB, e leggere l'intento dalla forma. È la generalizzazione
naturale di kb-first.

La correzione necessaria: "nessuna frase va a muro" va inteso come **"nessun muro
cieco"**, non "rispondi sempre". L'asticella onesta si sposta da *"capisci la
domanda?"* a *"hai il dato per rispondere?"*. Senza questa distinzione l'idea
diventerebbe l'impostore che i PRINCIPLES rifiutano.

Il valore concreto è alto e misurabile: oggi moltissimi 0 di LLMSCORE sono muri
ciechi su frasi *che parrot0 ha capito benissimo* ma per cui mancava un fatto.
Trasformarli in declini informati è onesto **e** alza la qualità percepita.

Rischi/limiti (onestà): il passo 5 (abduzione della mappa per predicati inediti)
resta il soffitto difficile; e un `pos/2` esteso è "substrato" da marcare per non
inquinare "quante cose sai?". Da costruire a piccoli incrementi, ciascuno con un
bench oracolare — la stessa disciplina del resto del progetto.

### Primo incremento consigliato

Il **declino informato** (§2, §4.4) prima di tutto: è il pezzo a più alto
rapporto valore/rischio e non richiede ancora `pos/2` né abduzione. Basta che,
quando un riconoscitore d'intento identifica lo schema ma la KB non ha il dato,
si emetta "ho capito che chiedi X su Y, ma non conosco Y" invece di
`"I don't understand that yet."`. Da lì si sale verso schemi e ruoli generici.

## 7. Dall'incomprensione all'agency: il piano di auto-documentazione

Il declino informato (§2) è il **primo gradino**. Il secondo, e qui sta la forza:
quando parrot0 capisce l'intento ma **non ha il dato**, non si limita a dichiararlo
— **innesca un'azione per procurarselo**. Si *documenta*.

```
Q: conosci la pizza margherita?
turno 1:  No, non ancora — ma mi sto documentando.   (e AVVIA il processo)
turno 2 (stesso "conosci la pizza margherita?"):
          Sì: la pizza margherita è …                (ora il dato c'è)
```

Nessun impostore: è **trasparente e sincero** — *"non conosco, ma posso
apprendere"*. La risposta **cambia nel tempo** perché lo **stato** è cambiato, non
perché ha finto.

### Il punto cruciale: non è un fetch solitario, è un PIANO

Il processo di auto-documentazione **non** è un hack una-tantum incollato al
modulo di ricerca. È un **passo del planner** attivato dall'**intento di
informarsi**. Realizzare "non so" **genera un goal**, e il planner lo persegue:

```
goal:   answer(about(pizza_margherita))
  └─ precond mancante: know(pizza_margherita)
       └─ sub-goal: acquire(know(pizza_margherita))
            └─ azione: fetch+learn wiki(pizza_margherita)  → asserisce wiki_concept/2
  └─ poi: resolve answer dai fatti ora presenti
```

Questa è esattamente la **forza dell'agency** ([the-agency](the-agency.md)): i
piani che *si attivano per fare cose*. "Capire di non sapere" diventa un
**innesco di pianificazione**, componibile con gli altri goal (non un ramo
isolato). È lo stesso planner di `compose_plan`/`mod_plan`, esteso con
l'azione `acquire-knowledge`.

### Rispetta i principi? Sì — è DATO, non intelligenza esternalizzata

Il vincolo dei PRINCIPLES è **"niente intelligenza esternalizzata"**: nessun LLM,
nessuna API che *ragiona* al posto suo. Scaricare **markdown fattuale da una fonte
certificata** (solo Wikipedia) è **acquisizione di DATI**, non di ragionamento: il
passaggio da markdown → `wiki_concept/2` → risposta resta **interamente di
parrot0**. È la stessa distinzione di [dynamic-knowledge](kb-first.md): parrot0 si
documenta da una fonte statica/certificata; il motore inferenziale non è
outsourced. Questo **raffina** la direzione "solo file statici": la fonte può
essere **remota** purché *certificata e ristretta a wiki*, perché ciò che entra è
conoscenza dichiarativa, non una mente in prestito.

### Cosa esiste già e cosa aggiungere

- **Esiste (gen171):** `mod_learn` (ex `mod_research`) legge markdown **locale**
  (`PARROT0_WIKI_DIR`), impara `wiki_concept/2` in RAM, e l'apprendimento
  **persiste** (re-ask risponde dal dato). Registrato per ultimo: solo il gap def.
- **Esiste (gen240): fetch remoto certificato, tutto in C.** Su un gap senza pagina
  locale, `mod_learn` chiama `wiki_fetch_topic` (`src/learn.c`): scarica la summary
  di **Wikipedia** via HTTPS (libcurl, ABI dichiarata a mano e linkata per SONAME —
  niente header dev, niente dipendenza dura) e **scrive la stessa `pages/<key>.md`**
  che spediremmo a-priori, poi `learn_topic` la impara. Doppio gate: build
  (`PARROT0_HAVE_CURL`) + runtime (`PARROT0_WIKI_FETCH`), così default e test restano
  **offline**. Solo Wikipedia: niente motori di ricerca, niente API d'intelligenza.
  **Invariante dimostrabile**: ciò che scarica è identico a ciò che avremmo
  distribuito a priori (stesso formato, stesso `wiki_concept`) — nessun imbroglio,
  solo la scelta di non spedire una KB enorme dal giorno zero.
- **Esiste (gen240):** il **declino informato** (§2) come testo del turno di gap.
- **Esiste (gen240): `acquire_knowledge` come azione di prima classe.** Estratta in
  una funzione C riusabile (`50-self-research-loop.c`): RAM → corpus locale → fetch
  on-demand. È invocabile (a) come **comando esplicito** — "learn about X",
  "research X", "look up X", "study X", IT "impara/studia/informati su X" — e (b)
  come **passo composto del planner**: "look up X **and then** tell me about X"
  esegue acquire(X) e poi il recall nello **stesso turno** (decompose peela il
  sequencer "then" e dispaccia le due sotto-frasi sullo stesso brain).
- **Aggiungere:** la precondizione **automatica** — un goal qualsiasi che *nomina*
  un concetto ignoto dovrebbe innescare acquire(X) da solo (oggi serve il comando
  esplicito o la composizione "…and then…"); e l'azione `produce-artifact` (§8).

### Caveat onesti (non negoziabili)

- **Determinismo dei test.** La rete è non-deterministica e introduce un confine
  di fiducia. `make test` e i bench restano **offline su fixture**
  (`PARROT0_WIKI_DIR`); il fetch remoto è una **feature runtime** dietro env, mai
  nel percorso dei test. (Stessa disciplina di `chat-sim`/`llmscore`, esterni.)
- **Una sola fonte, certificata.** Solo Wikipedia, solo markdown fattuale,
  parsing nostro. Allargare le fonti è un'altra decisione, da prendere qui.
- **Verità di stato.** "Mi sto documentando" si può dire **solo** se il piano è
  stato realmente avviato; altrimenti è il declino informato secco. Mai promettere
  un apprendimento che non avviene.
- **UX a due turni.** Turno 1 dichiara+avvia, turno 2 conosce. Onesto e
  sufficiente; un fetch sincrono in-turno è un'ottimizzazione successiva, non un
  requisito.

### Verdetto (estensione §6)

**Ha senso, ed è il completamento naturale.** Informed decline + `mod_research`
esistente + planner = un **declino che si auto-ripara**: transitorio, onesto,
agente. La cornice giusta è quella dell'utente: *non* un fetch isolato ma un
**piano** innescato dall'intento di sapere — è lì che vive l'agency. Resta
principiato finché ciò che si scarica è **dato certificato** e il ragionamento
resta in casa, e finché i test non dipendono dalla rete.

## 8. Lo stesso loop per il CODICE: scoperta → creazione

La stessa agency vale per una richiesta di **produrre**, non solo di sapere.

```
Q: scrivi un algoritmo di bubblesort
turno 1:  Ho capito la richiesta (scrivere bubblesort), ma non so cos'è
          bubblesort — mi documento.        (lancia il PLAN di scoperta wiki)
turno 2 (stessa richiesta):
          Ora so cos'è. Ecco l'implementazione, verificata, scritta nei file.
                                             (lancia il PLAN di creazione)
```

Lo stesso prompt ripetuto **avanza il piano di uno stadio**, perché tra un turno e
l'altro è cambiato lo **stato** (prima `know(bubblesort)`, poi l'artefatto). Non è
una risposta diversa per capriccio: è il planner che sale la sua **scala di
precondizioni**.

### La scala di precondizioni (è una sola, generale)

```
goal: produce(code(bubblesort))
  ├─ precond: know(bubblesort)
  │     └─ se manca → PLAN scoperta:  fetch+learn wiki(bubblesort)   [§7]
  └─ precond: verified_impl(bubblesort)
        └─ se manca → PLAN creazione: synth → oracle(code_eval) → write file(s)
```

È **lo stesso meccanismo** di §7, con un anello in più. "Capire l'intento" resta
sempre possibile; ciò che il planner insegue sono le **precondizioni mancanti**,
una per turno, nominandole onestamente. Knowledge-gap → piano di scoperta;
artifact-gap → piano di creazione. La medesima agency, due azioni diverse
(`acquire-knowledge`, `produce-artifact`).

### Quanto è già reale

- **Scoperta:** reale. `mod_research` impara da wiki; `sortlearn-bench` mostra già
  *forget → relearn-from-wiki(bubble_sort) → sintesi di un sort verificato*
  ([learn-and-build](learn-and-build.md)). Lo stadio 1 esiste.
- **Sintesi + verifica:** parziale. `mod_compose`/`code_synth` compongono e
  **verificano con oracolo** (`code_eval`) piccole funzioni (add, e il sort da
  schema). Il *no-fabrication* è già la regola (vedi `game-bench`).
- **Creazione dei file:** è **il gap**. I tool `piact` sono **read-only**;
  scrivere `.c` su disco e *buildare un programma* non esiste ancora. È esattamente
  ciò che `make game-bench` registra come gradino aperto (P3: assemble+build).

### Cosa aggiungere

1. **Stato del piano** come precondizioni interrogabili: `know(X)?`,
   `verified_impl(X)?` — così lo *stesso* prompt sa a che stadio si trova.
2. **Azione `produce-artifact`**: synth → oracolo → **scrittura file** in una
   **workspace sandboxata** (mai sull'albero del repo), bounded, e **solo se
   l'oracolo verifica** (altrimenti declino, mai codice non verificato spacciato
   per fatto — la regola di `game-bench`).
3. **Risposte veritiere di stadio**: turno 1 "mi documento" *solo* se la scoperta
   parte davvero; turno 2 "ecco, verificato e scritto in `…`" *solo* se l'oracolo
   è passato e il file esiste.

### Caveat onesti

- **Scrivere file è un'azione esterna/side-effecting**: va sandboxata (workspace),
  limitata, e i test restano ermetici (nessuna scrittura fuori da tmp/fixtures).
- **L'oracolo è il guardiano**: la creazione è lecita solo su artefatti
  *verificabili*. Un programma intero (es. il tris di `game-bench`) ha un oracolo
  debole/assente → resta soffitto finché non sappiamo verificarlo; lì il declino
  informato è ancora la risposta giusta.

### Verdetto (estensione §6–§7)

**Ha senso ed è la stessa idea, completa.** Una sola agency — *capisci l'intento,
insegui le precondizioni mancanti una alla volta* — copre sia il sapere (pizza
margherita) sia il fare (bubblesort: prima impara, poi crea). Lo stadio *scoperta*
è già reale; lo stadio *creazione-file* è il prossimo grande gap, già misurato da
`game-bench`. Principiato finché: dati certificati per imparare, **oracolo** per
creare, scrittura **sandboxata**, e test offline.

---

## Appendice — appunti originali

> in regime di comprensione universale (kb-first) sarebbe interessante pensare che
> non esistano frasi di cui parrot0 non capisce / va a muro. ma immaginavo che lui
> grammaticalmente sappia estrarre la struttura di qualsiasi frase e le strutture
> siano dentro la KB. in questo modo frasi ben formate ma con parole che non
> conosce possono essere interpretate nell'intento — quel meccanismo analitico, la
> lettura del significato appoggiata alla KB.
## 9. La sessione stessa è conoscenza (log conversazionale + artefatti)

Estensione (F., gen240): **tutto ciò che ci diciamo, e tutto ciò che parrot0 crea,
diventa conoscenza di sessione interrogabile** — non in variabili C, ma come fatti
nella KB temporanea di sessione.

- **Log conversazionale.** Ogni turno è registrato come `utterance(Seq, Speaker,
  "testo")` (Seq monotòno; Speaker `user`|`self`), a fine-turno (così una domanda di
  recall vede fino al turno PRECEDENTE). Diventano inferibili: "qual è l'ultima cosa
  che ho/hai detto", "la prima frase che ho detto", "l'ultima parola che hai detto".
  Finestra limitata (`CONV_LOG_WINDOW`): i più vecchi vengono ritratti.
- **Artefatti.** Quando parrot0 crea un file (oggi: la pagina wiki scaricata dal
  piano di scoperta), lo registra come `artifact(file, "path")` → "cosa hai creato
  in questa sessione?". Cresce man mano che arriveranno la creazione-file della
  codegen (§8) e altri artefatti (dir, ecc.).

Tutto KB-first e **substrato** (`utterance`, `artifact` marcati): è stato di
sessione, non "cose che sai" del mondo, e non inquina "quante cose sai?". Persiste
con `/save` come il resto della sessione. La lingua corrente (§0, `current_language/1`)
è lo stesso principio applicato a un'altra proprietà della sessione.


---

# §10. PROTOCOLLO — le tre specie di lacuna, e quali si chiudono *prima* che qualcuno chieda

*Aperto il 2026-09-02 su domanda di F.: «giudica il tipo di gap che derivava
dalla non comprensione di questi prompt, e dimmi se era possibile mettere in
campo strategie per evitarle, o se invece siamo in un regime di varianza così
elevato che non possiamo ipotizzare ciò che manca e colmarlo prima ancora che
qualcuno lo chieda».*

È la domanda centrale di questo piano, perché «comprensione universale» non
significa *aver previsto tutto*: significa sapere **che cosa è prevedibile e che
cosa no**, e usare due strategie diverse per le due metà.

## 10.0 Il campione che ha generato il protocollo

Chat reale, tre frasi elementari, tre muri:

```text
>>> ho un libro nello zaino      Non capisco ancora.
>>> il mio libro e nello zaino   Non sono sicuro di aver seguito.
>>> libro in zaino               Non capisco ancora.
```

Sembrano lo stesso difetto. **Sono tre specie diverse**, e la distinzione decide
se erano evitabili.

## 10.1 ⛔ IL TEST DIAGNOSTICO — si esegue PRIMA di diagnosticare

> **Davanti a un muro, prova la frase MINIMAMENTE DIVERSA. La distanza fra la
> frase che funziona e quella che non funziona *è* il tipo di lacuna.**

| distanza | specie | esempio misurato |
|---|---|---|
| **un carattere** | variante di superficie | `libro è nello zaino` ✅ / `libro e nello zaino` ⛔ |
| **una parola piena** | costruzione mancante | `mary have a book in the backpack` ✅ / `i have …` ⛔ |
| **strutturale** (nessuna variante vicina funziona) | forma telegrafica | `libro in zaino` ⛔ |

Il test costa tre turni e sostituisce ore di ipotesi. È lo stesso principio del
differenziale sui rossi: **non attribuire una lacuna prima di aver misurato la
sua distanza da ciò che funziona.**

## 10.2 Specie 1 — VARIANTE DI SUPERFICIE → prevedibile, e si genera dalla classe

`e` per `è`, apostrofi, elisioni, maiuscole, accenti mancanti.

**Non è varianza: è una collisione fra due parole di una classe chiusa.** Le
classi chiuse — articoli, copule, preposizioni, clitici, congiunzioni — sono
finite e già in KB. **Le loro varianti di scrittura reale sono enumerabili a
priori dalla classe stessa**, senza aspettare che qualcuno le scriva.

⛔ **Ma la cura non è piegare la variante sulla forma piena**: `e` → `è`
distruggerebbe la congiunzione. La cura è **disambiguare per contesto con una
regola di grammatica vera**:

```text
Una congiunzione non può unire un sintagma NOMINALE a un sintagma PREPOSIZIONALE.
  ⇒ «e» seguita da una preposizione è la copula.
```

Realizzata come `copula_variant/3` + `copula_variant_context/1`, quest'ultima
**derivata** dalle 32 preposizioni articolate più le locative: nessun membro da
mantenere, e una preposizione nuova si porta dietro anche questa lettura.

> **Costo di averla mancata:** questa lacuna era già a registro **due volte**
> («GD3 — le varianti ortografiche azzerano una lezione»), la seconda trovata da
> me stesso mentre scrivevo una sonda. È stata *annotata* due volte invece che
> chiusa, e la terza l'ha incontrata l'utente su una delle frasi più ordinarie
> dell'italiano scritto.

**⇒ Regola 1: CHIUSURA AL SECONDO AVVISTAMENTO.** Un rosso che compare due volte
non è un incidente: è una classe. **Il registro dei rossi è una coda di lavoro,
non un archivio.**

## 10.3 Specie 2 — COSTRUZIONE MANCANTE → prevedibile per SIMMETRIA

```text
X è in Y            (copula + luogo)          ✅
ho messo X in Y     (trasferimento + luogo)   ✅
ho un X in Y        (possesso + luogo)        ⛔
```

**Non è varianza: è un buco in una tabella già piena per due terzi.** Non serviva
indovinare che cosa qualcuno avrebbe detto — bastava guardare le altre due
caselle. Il verbo mancante (`have`) era anche il più frequente dei tre.

È esattamente la forma del difetto già registrato per `imperative_opener/1`, che
conteneva *execute, run, migrate, scan* — i verbi che avevano causato un bug — e
non conteneva *tell, write, describe*, cioè quelli con cui si chiede davvero
qualcosa.

> **Una classe popolata dai sintomi non è una classe: è l'elenco degli
> incidenti.** Il test è meccanico e non richiede un utente: *i membri descrivono
> la classe o la storia dei bug?*

**⇒ Regola 2: COMPLETAMENTO DELLA CLASSE, NON DEL CASO.** Ogni volta che una
costruzione o una classe chiusa entra in KB, si chiede **quali altri membri
reggono la stessa struttura** e li si aggiunge nello stesso turno di lavoro.
Generabile a tavolino.

## 10.4 Specie 3 — FORMA TELEGRAFICA → **non** prevedibile, e va bene così

`libro in zaino`. Nessun articolo, nessuna copula, nessun verbo.

**Qui sì siamo nel regime di varianza alta di cui parla F.**, e affrontarlo *a
priori* sarebbe un errore di metodo: ogni ipotesi su come qualcuno abbrevia è un
frasario, e un frasario è ciò che questo progetto vieta.

> **La risposta giusta a questa specie non è anticiparla: è renderla
> INSEGNABILE.** Non «indovinare la forma», ma «garantire che chi la usa possa
> insegnarla in un turno e che valga dal turno dopo».

È la direttiva di F. del 2026-09-02: *dove la capacità c'è e manca la forma, si
deve garantire l'insegnabilità* — con il suo gate:

```text
> sbloccami x + 1 = 6              (muro)
> sbloccami is an equation filler  Learned.
> sbloccami x + 1 = 6              x = 5.
> !forget equation_filler(…)       (muro di nuovo)
```

**⇒ Regola 3: INSEGNABILITÀ COME RETE PER IL RESTO.** È la sola strategia che
**scala** con la varianza invece di inseguirla: non copre una forma, copre
*tutte* le forme che qualcuno si prenderà la briga di insegnare.

## 10.5 Il giudizio, e perché non è una risposta di comodo

**Due delle tre lacune erano prevedibili, con strategie diverse; la terza no, e
per la terza la strategia giusta non è la previsione ma l'insegnabilità.**

La risposta comoda sarebbe stata «è tutta varianza, non si può prevedere» — ed è
falsa: due terzi del campione erano chiudibili senza mai vedere la chat. La
risposta opposta — «si può prevedere tutto» — porta dritta al frasario.

> **Comprensione universale non vuol dire aver previsto tutto. Vuol dire che ciò
> che è generabile dalla struttura è già chiuso, e ciò che non lo è si chiude
> parlando.**

## 10.6 La checklist operativa

Da eseguire quando entra una costruzione, una classe chiusa o un rosso:

1. **Misura la distanza.** Prova la frase minimamente diversa (§10.1). Non
   diagnosticare prima.
2. **Se la distanza è un carattere** → genera le varianti di scrittura *dalla
   classe*, e disambigua per **contesto**, mai piegando la variante sulla forma
   piena.
3. **Se la distanza è una parola piena** → completa la classe: quali altri membri
   reggono questa struttura? Applica il test «sintomi o classe?».
4. **Se nessuna variante vicina funziona** → non anticipare. Verifica invece che
   la forma sia **insegnabile in un turno**, e che il retract la tolga.
5. **Se il rosso era già a registro** → chiudilo adesso. Il secondo avvistamento
   è la scadenza, non il terzo.
6. **Registra la specie nel commit.** Un rosso senza la sua specie non è
   annotato: è rimandato.
