# Motorize the Class — missione LLMSCORE-max

> **Principio unico:** *motorizza la classe, poi nutri il motore su scala — non
> memorizzare istanze.* Ogni classe che parrot0 vince oggi è un MOTORE su una KB
> ampia (rime → `wordquery` + 35k `lexeme`; aritmetica → `wordproblem`). Ogni
> classe che perde è un LOOKUP su una tabella sparsa (`explanation`=4,
> `because`=25, `qa_reply`=6). Il fix è convertire ogni classe fragile da
> *tabella* a *motore + ingestione*, non aggiungere righe alla tabella.

Coerente con lo steer di F.: **meno moduli, più KB**. Un motore causale è UN
motore per l'intera classe "perché" — l'opposto di N moduli-qa. I motori sono
C (motori, non frasi); la conoscenza cresce nella KB per processo.

## Missione aggiornata — massimizzare il punteggio atteso, non il run visto

LLMSCORE non misura una checklist fissa: campiona 20 richieste da una coda libera
di comportamenti LLM-like. Dopo gen350 il probe interno può dire **70/70 answered**
e il run ufficiale può ancora cadere a **4/10**. Questo non contraddice il piano:
dimostra che il vecchio metro primario, il wall-rate, misurava solo la prima
frontiera. La missione ora è più stretta:

> **Massimizzare il valore atteso di LLMSCORE sotto alta varianza, riducendo sia
> muri sia risposte sbagliate, tramite crescita KB-first di classi componibili.**

La domanda guida non è più soltanto "parrot0 sa ingaggiare questa classe?" ma:

1. **completezza multi-intento:** se il prompt chiede A **e** B, la risposta deve
   coprire entrambe le slot, non fermarsi al primo frame che matcha;
2. **fedeltà di formato:** se il prompt chiede due righe, tre modi, una lista, una
   frase semplice, il formato è parte del task, non decorazione;
3. **calcolo semanticamente tipato:** numeri e operazioni non bastano; servono gli
   slot giusti (`price`, `change`, `width`, `length`, `digit_count`);
4. **fatti bundle/compositi:** le domande factuali spesso combinano capitale +
   motivo storico, anno + entità correlate, opera + autore + contesto;
5. **anti-collisione di routing:** il primo modulo che risponde può ancora essere
   quello sbagliato. Il routing deve imparare a declinare quando un cue più
   specifico o compositivo è presente.

Quindi il piano non mira a inseguire il prossimo `LLMSCORE.md`. Usa ogni failure
come campione da una distribuzione latente e chiede: *quale famiglia di prompt ha
appena esposto? quale rappresentazione KB la copre a pioggia? quale test runtime
dimostra che la superficie è insegnabile senza rebuild?*

## Reset gen355 — la varianza si fattorizza, non si insegue

Il run successivo a gen354 ha segnato **0/10** su dieci task nuovi: influenza
storica, sconto concatenato, haiku, spiegazione con esempio, traduzione, contrasto
biologico, moto con partenze sfalsate, raccomandazioni motivate, dialogo e
significato culturale. Nel frattempo la KB aveva gia acquisito molte risposte dei
tail precedenti. Questo e il controesempio decisivo: anche un lookup KB-first e
teachable resta un lookup a misura quasi zero se la sua chiave e l'intero prompt.

La distribuzione del giudice non e pero arbitraria in tutte le dimensioni. Si
fattorizza:

```
richiesta = atto finito x argomento aperto x vincoli finiti x profondita
```

### LLMSCORE-20 — piu varianza osservata, non meno

Da gen357 il campione ufficiale passa da 10 a **20 domande libere**. Non ci sono
classi imposte, quote, stratificazione o template per il generatore: vincolarlo
renderebbe il benchmark prevedibile e strozzerebbe proprio la varianza che deve
esporre. Il prompt remoto chiede invece massima varieta e sorpresa.

La modifica migliora due proprieta senza cambiare la distribuzione:

- la risoluzione del punteggio passa da 10 a 5 punti percentuali;
- nel caso Bernoulli peggiore (`p=0.5`) l'errore standard del campione scende da
  circa `0.158` a `0.112`, una riduzione di circa il 29%.

Il loader conserva il tail unseen da 10 gia generato. Nel solo run di transizione
chiede al giudice, prima dell'intervista, altre 10 domande generate liberamente:
nessun seed fisso e nessuna lista nota entra nel campione. Il tail prodotto dopo
quel run conterra 20 domande interamente nuove. Il totale resta volutamente una
misura non stratificata del comportamento atteso.

La prima esecuzione LLMSCORE-20 ha inoltre esposto un difetto della metrica, non
del campionamento: dopo la preparazione remota, una domanda ha tenuto
`./bin/parrot0` al 99% di un core per oltre quattro minuti; il driver bloccava su
`readline()` e Python bufferizzava perfino i log gia prodotti. Il run e stato
interrotto e non produce un punteggio valido. Il contratto operativo diventa:

- flush immediato di ogni fase, domanda, risposta e durata;
- stampa della domanda prima di consegnarla al soggetto;
- processo locale isolato e timeout di **1 secondo** per domanda: il timeout
  riceve immediatamente voto 0 senza consumare una chiamata al giudice;
- otto domande locali concorrenti: anche venti blocchi patologici consumano circa
  tre ondate da un secondo, non venti secondi in serie;
- checkpoint del tail completo prima dell'intervista, cosi un'interruzione non
  ripaga la chiamata di preparazione e non cambia il campione al retry;
- timer separati per preparazione remota, intervista locale, giudizio remoto e
  totale, con heartbeat ogni 10 secondi durante l'attesa remota.

Il requisito di una scheda entro circa 30 secondi cambia il compromesso remoto.
La singola chiamata combinata minimizzava richieste e costo, ma nel run osservato
ha impiegato `76.7 s`. Il regime rapido usa invece shard concorrenti da cinque
risposte. Le domande restano un campione libero non categorizzato: lo sharding
divide soltanto lavoro di giudizio gia prodotto, non impone classi o quote al
generatore.

Ogni chiamata remota ha una deadline compatibile con un budget totale
predefinito di `30 s`. Un judge shard incompleto viene ritentato sui soli scambi
mancanti. Se anche il retry non produce tutti i verdetti, il run e invalido e
`LLMSCORE.md` precedente non viene sovrascritto: l'indisponibilita del giudice
non e un fallimento di parrot0. La generazione del prossimo tail e un prefetch
separato (`make llmscore-tail`) e non compete piu per rete, modello o deadline
con il giudizio.

### Esito LLMSCORE-20 libero — 1/20 e nuova diagnosi dominante

Il retry valido del 2026-07-24 ha misurato **1/20**. L'unico punto e una
continuazione narrativa pertinente. Il tempo totale e stato `144.2 s`:

- `24.4 s` per completare una tantum il vecchio tail 10→20;
- `43.1 s` per l'intervista locale, di cui circa 40 secondi in cinque timeout;
- `76.7 s` per giudicare 20 scambi e generare i 20 successivi.

Poiche tutte le risposte locali non patologiche sono arrivate entro `0.7 s`, il
timeout definitivo e `1 s`. Ogni domanda usa un processo indipendente: un
timeout viene ucciso, auto-valutato 0 e non puo contaminare gli altri turni.
L'heartbeat ogni 10 secondi rende osservabile la latenza residua del provider.

Le 19 sconfitte non vanno trasformate in diciannove nuove righe di lookup. Il
campione libero rivela invece tre invarianti architetturali:

1. **Misclaim prima della competenza.** Cinque richieste diventano storie in cui
   la prima parola (`Suggest`, `In`, `Explain`, `Provide`, `Translate`) e il
   protagonista. Il composer narrativo deduce un payload da una superficie che
   non ha provato essere narrativa. Un output fluente ma fuori task e peggiore
   di un wall.
2. **Piano incompleto accettato come successo.** Un dialogo vincolato riceve una
   storia sul viaggio nel tempo; una mousse vegana riceve la ricetta del toast.
   La sovrapposizione di un solo cue fa parlare il consumer, senza provare tipo
   di artefatto, topic e vincoli richiesti.
3. **Fallimento negativo non limitato.** Cinque domande tengono il solver
   occupato oltre 8 secondi. Quattro contengono atti semantici generali
   (`explain`, `describe`, `what are`): la proiezione Horn
   `semantic_summary/2`, quando il topic manca, amplia la ricerca invece di
   fallire in costo prevedibile. Il quinto caso geometrico mostra la stessa
   necessita di budget nel routing quantitativo.

La prossima iterazione non aggiunge “filosofia”, “UNESCO”, “Hamlet”, “Congress”
o “cerimonia del te” come categorie. Introduce un unico **contratto di claim**:

```
candidate_plan(Module, Act, Topic, Constraints, Evidence)
plan_required(Act, Slot)
plan_complete(Candidate) :- every_required_slot_bound(Candidate)
```

Ogni modulo puo produrre un candidato, ma non scrivere l'output. Un arbitro
generico sceglie soltanto candidati con tutti gli slot obbligatori e preferisce
la maggiore evidenza; parita o incompletezza significano declino. In
particolare:

- il narratore richiede un atto narrativo KB esplicito, non la sola lunghezza;
- ricette, dialoghi, poesie e analisi dichiarano tipo e vincoli prima del
  rendering;
- le viste semantiche enumerano fonti indicizzate con un budget di lavoro
  costante per token, senza lanciare una regola aperta su tutto il corpus;
- ogni consumer restituisce `matched`, `complete`, `cost` e `reason`, così
  LLMSCORE puo attribuire tempo e misclaim al modulo responsabile.

Questo e il vero moltiplicatore della prossima fase: non aumenta i topic
memorizzati, ma impedisce che qualunque topic futuro venga distrutto da un
consumer sbagliato e rende il costo di un miss indipendente dalla dimensione
della KB.

### Misura LLMSCORE-30s

La validazione finale del run rapido del 2026-07-24 chiude in **23.476
secondi**:

- intervista locale: `2.757 s`;
- timeout automatici oltre un secondo: `12/20`;
- giudizio remoto concorrente: `8.264 s`;
- score comportamentale: **0/20**.

Le otto risposte tempestive sono tutte fallimenti reali: wall espliciti,
deflessioni personali, narrativa fuori task o calcoli non pertinenti. Il nuovo
limite non ha quindi nascosto punti validi; ha convertito dodici esplosioni di
ricerca in un segnale diagnostico immediato. Il primo esperimento rapido con
MiniMax anche come judge aveva rispettato il tempo (`27.868 s`) ma prodotto JSON
incompleto; non e una misura di score. Judge e generatore definitivo sono
`kimi-k2.6`, gia usato nel repository come modello instruct non-reasoning. Il
campione resta libero: il cambio elimina reasoning remoto lento, non introduce
classi o quote.

La generazione concorrente del tail successivo non aveva prodotto 20 domande
entro la deadline e aveva inoltre sottratto capacita remota al giudice. E stata
quindi rimossa dal percorso critico: `make llmscore-tail` prepara in anticipo un
campione libero nuovo, mentre `make llmscore` consuma soltanto un tail completo
gia disponibile. Nessuno dei due comandi legge o classifica semanticamente le
domande per imporre quote.

La verifica transazionale sullo stesso tail, dopo questa separazione, chiude in
**12.257 s**: intervista locale `2.689 s`, nove timeout automatici e giudizio
remoto `9.567 s`. I tre shard hanno portato l'avanzamento da `10/20` a `15/20`
e infine `20/20`; tutti i verdetti tempestivi sono reali e la scheda non contiene
piu fallback “judge unavailable”. Lo score resta **0/20** e non costituisce un
nuovo campione di varianza, perche questo giro riusa intenzionalmente il tail
disponibile per verificare il protocollo. Il prossimo controllo fuori campione
richiede prima `make llmscore-tail`.

Il generatore e il judge hanno ruoli diversi anche a livello di modello:
`minimax-m2.5`, gia usato per le conversazioni libere del repository, prepara
il tail fuori dal budget; `kimi-k2.6` resta il giudice rapido e strutturato.
Kimi aveva restituito due volte una parafrasi del prompt invece dell'array JSON,
quindi usarlo anche come generatore rendeva il prefetch falsamente indisponibile.

### Gen358 — fondazione della nuova sfida

Lo `0/20` transazionale diventa la baseline della nuova sfida LLMSCORE-max. Non
e una lista da memorizzare: rivela tre moltiplicatori indipendenti sui quali una
singola iterazione puo agire prima di ricampionare.

1. **Costo negativo.** Tutti i nove timeout attraversavano lo stesso percorso:
   una forma generale (`what are`, `explain`, `describe`) interrogava
   `semantic_summary/2` una volta per token. Un topic assente pagava ripetutamente
   la ricerca ricorsiva nell'intero corpus.
2. **Densita semantica.** La KB possedeva gia decine di `wiki_concept/3`, ma il
   binder token-per-token non rappresentava topic multi-parola e rendeva
   costoso perfino scoprire che un topic mancava.
3. **Contratto generativo.** Diverse richieste creative complete cadevano in
   deflessioni personali o nel narratore generico. Il sistema aveva testi e
   template, ma non un selettore unico fra atto richiesto e artefatto completo.

La prima fondazione implementa:

- `answer_projection(Relation, EvidenceRelation)` e
  `projection_source(Relation, Source, Mode)` come metadati KB;
- una sola selezione `semantic_topic_cue/2` tramite universal evidence scorer,
  seguita da una lettura diretta della fonte dichiarata; un miss e ora limitato
  e immediato, indipendente dal numero di token;
- cue canonici e alias per il corpus enciclopedico, tutti modificabili a runtime,
  piu concetti trasversali su informazione, crittografia, reti quantistiche,
  etica, medicina, acqua e modelli cognitivi;
- `creative_response(Intent, Frame)`, selettore universale fra famiglie di
  artefatti completi. C non conosce ne cue ne wording: aggiungere o ablare una
  famiglia resta una modifica runtime di `intent_cue`, `creative_response` e
  `response_template`;
- opener imperativi nella KB perche comandi lunghi come proporre, progettare o
  immaginare non siano reinterpretati come continuazioni narrative.

Le famiglie interne sono ipotesi di competenza e non entrano nel generatore
LLMSCORE: il tail ufficiale resta totalmente libero, senza categorie, quote o
stratificazione. La verifica e volutamente una sola: compilare, generare un
nuovo tail senza leggerlo con `make llmscore-tail`, poi eseguire
`make llmscore`. Nessuna sonda intermedia sul vecchio campione viene usata come
oracle di sviluppo.

### Primo controllo fuori campione e seconda iterazione

Il primo tail nuovo dopo la fondazione resta a **0/20**, ma cambia il profilo
misurato:

- tempo totale `22.807 s`, ancora entro budget;
- timeout automatici `4/20`, contro `9/20` nella baseline transazionale;
- tutti i dieci verdetti inizialmente mancanti sono stati recuperati dal retry
  individuale, quindi lo zero e reale e non e indisponibilita del judge.

Il costo negativo e quindi quasi dimezzato, ma non si e ancora trasformato in
punti. Le risposte tempestive espongono collisioni e incompletezza:
`epidemic` attiva il substring `pid`; `Moore's Law` attiva il generico `law`;
una richiesta su rapporto massa/volume sceglie la ricetta del toast perche il
solo token `recipe` basta come topic; un riassunto corretto di blockchain o
scattering non copre tutte le faccette richieste.

La seconda iterazione alza il requisito da “topic trovato” a “piano completo”:

- il PID usa `process_id_request` con `keyword(pid)`, quindi il confine lessicale
  e parte del matcher universale;
- `topic_noise(process_topic, recipe)` impedisce a un topic generico di vincere
  la selezione di una procedura;
- il confronto plurale usa `contrast_request` KB e chiude gli argomenti alla
  punteggiatura;
- gli atti `how would`, `how could`, `propose` e `design` riusano la proiezione
  indicizzata soltanto quando esiste un topic con evidenza;
- i nuovi sommari sono multi-faccetta: contengono meccanismo, confronto,
  implicazione o procedura richiesta, non la sola definizione;
- il conteggio di codici con somma vincolata e ora un vero DP su posizione e
  somma. Tre classi di cue KB devono essere presenti; il risultato non e una
  risposta memorizzata.

Anche questa iterazione sara giudicata soltanto su un altro tail libero
prefetchato e non letto. Il vecchio campione resta diagnosi, non target.

La nuova priorita e quantitativamente netta: prima di aumentare conoscenza o
creativita, eliminare i timeout portando il costo negativo sotto un secondo.
Subito dopo, impedire al composer narrativo di parlare senza un atto narrativo
provato. Queste due correzioni interessano `11 + almeno 3` righe del campione
senza introdurre categorie nel generatore.

- gli **atti** sono pochi e ricorrenti: descrivere, spiegare, confrontare,
  calcolare, tradurre, raccomandare, creare, continuare;
- gli **argomenti** sono aperti, ma condividono faccette: definizione, causa,
  meccanismo, esempio, effetto, rischio, beneficio, contesto, eredita;
- i **vincoli** sono pochi e componibili: conteggio, ordine, formato, lingua,
  lunghezza;
- la **profondita** e il numero di subgoal obbligatori, non una nuova classe.

Ne segue la strategia LLMSCORE-max:

> **Motorizzare gli atti come piani su faccette semantiche e ingerire gli
> argomenti come atomi riusabili. Mai piu associare una domanda intera a una
> risposta intera quando la risposta puo essere proiettata da relazioni.**

La forma bersaglio e una vista comune sopra la KB esistente:

```
semantic_atom(Topic, Facet, Text)
answer_plan(Act, Facet, Order, Required)
topic_alias(Surface, Topic)
format_contract(Constraint, Realizer)
```

`because/2`, `explanation/2`, `wiki_concept/3`, `process_step/3`,
`difference_between/3`, `event_attr/3` e le relazioni future non vanno
riscritte: regole Horn le proiettano in `semantic_atom/3`. Il composer esegue
l'`answer_plan`, prova i required slot e parla solo quando il piano e completo.
Un nuovo fatto alimenta cosi piu atti; un nuovo atto riusa tutti i fatti.

### Disciplina anti-tail

1. Il prossimo `.llmscore_tail.json` non si legge prima dell'iterazione.
2. `qa_cue/qa_reply` e `creative_text_cue/creative_text` restano strutture
   secondarie, ma non ricevono nuovi prompt interi salvo informazione
   genuinamente idiomatica e indivisibile.
3. Ogni incremento deve dichiarare il suo moltiplicatore atteso:
   `nuovi atti x nuovi topic x nuovi vincoli`; se vale circa uno, e overfitting.
4. Il solo controllo dell'iterazione e il LLMSCORE ufficiale post-modifica:
   niente sonde sul tail, niente cucitura dopo averlo visto.

### Prima iterazione — proiezione semantica definizionale

Ipotesi: una quota stabile dei tail chiede `what is / explain / describe / tell
me about / significance` su concetti comuni. Oggi esistono gia
`wiki_concept/3` e un corpus generato, ma il corpus non e caricato nel mondo
ordinario e `answer_frame` interroga solo relazioni binarie. Il primo passo e:

- caricare il corpus statico certificato `kb/learning/learned.p0`;
- proiettare `wiki_concept(Topic,Domain,Text)` in
  `semantic_summary(Topic,Text)` con una regola Horn;
- risolvere i topic multi-parola tramite `semantic_alias/2`;
- collegare piu atti allo stesso predicato binario tramite `answer_frame/2`;
- proteggere i consumer piu specifici (contrasti, processi, codice,
  traduzione, attivita) con `compound_guard/2`.

Moltiplicatore iniziale atteso: **5 atti x circa 50 concetti**, prima ancora
delle varianti di superficie gia assorbite dal matcher. Non e ancora il
`semantic_atom/3` completo: e la prima fetta minima che verifica se condividere
una proiezione aumenta il punteggio fuori campione.

### Esito prima iterazione — 8/10, con attribuzione onesta

`make llmscore` del 2026-07-23 ha compilato lo stato cumulativo e ha portato il
campione da **0/10 a 8/10**:

- corretti e accettati: influenza della stampa, haiku, supply/demand con esempio,
  traduzione FR, mitosi/meiosi, tre raccomandazioni motivate, dialogo AI,
  significato dei samurai;
- errato: lo sconto concatenato risponde `2.` invece di `$54`;
- corretto ma giudicato incompleto: i treni rispondono `11:00 AM`, senza mostrare
  la riduzione dei 200 miles con l'ora di vantaggio e la closing speed.

**Caveat di attribuzione:** il run ha riusato lo stesso tail del precedente
0/10, e il worktree conteneva gia fix gen355 specifici che, essendo `mod_qa`
registrato prima di `answer_frame`, hanno vinto su diversi item. Quindi l'8/10
verifica che lo stato cumulativo ripari quel campione, ma **non e ancora prova
fuori campione** del rendimento di `semantic_summary/2`. Il giudice ha generato
un nuovo tail, che resta intenzionalmente non letto: sara il controllo reale
della prossima iterazione.

I due zero rafforzano il modello fattorizzato:

1. **Routing e parte del piano semantico.** `discount_chain_schema` esiste ed e
   corretto, ma un motore numerico generico precedente lo intercetta. La prossima
   mossa non e un altro calcolo: e un pre-dispatch di schemi a evidenza, dove il
   piano piu specifico vince prima che un operatore scalare possa parlare.
2. **La proof e parte dell'output per i task multi-step.** Un valore corretto non
   basta sempre al giudice. Le procedure devono produrre risultato + ledger dei
   passaggi; un `response_template` KB rende entrambi. Questo vale per sconti,
   moto, eta, rate, miscele e algebra, non solo per i treni.

Prossimo incremento raccomandato: **plan-first quantitative dispatch** con
specificita calcolata da cue KB, slot tipizzati e rendering della proof. Solo
dopo, eseguire il nuovo tail ancora unseen per separare il guadagno sistemico
dai fix del campione precedente.

## Diagnosi gen351 — perché 0% wall non basta

Ultimo campione osservato: **4/10**. I failure non sono "mancano sette risposte":
sono segnali di classi deboli.

| Failure | Sintomo | Classe latente da motorizzare |
|---|---|---|
| Poema due-linee | contenuto buono, formato sbagliato | post-shaper di formato come KB task constraint |
| Rettangolo perimetro/lunghezza doppia | numero arbitrario, slot mancanti | algebra di coppie vincolate: somma raddoppiata + rapporto + ruoli dimensionali |
| Canberra compromesso | prima risposta factual vince e tronca il "why" | domanda composta: answer plan con più subgoal |
| Apples + change | calcola prezzo ma non resto | chain aritmetica multi-step con ledger di quantità |
| WWII + atomic cities | risponde all'anno ma non alle entità correlate | factual bundle/event frame |
| Digit 7 tra 1 e 100 | aritmetica generica sbaglia count combinatorio | enumeratore/count procedure per digit/range |

Pattern comune: parrot0 oggi eccelle nei **single-goal frames**. LLMSCORE campiona
sempre più spesso **compound goals**: task + formato, fatto + spiegazione, calcolo
seguito da secondo calcolo, evento + attributi multipli. La prossima frontiera è quindi una
KB che non memorizza solo fatti atomici, ma anche **piani di risposta**: frame che
dicono quali subgoal devono essere soddisfatti prima di parlare.

## Postmortem gen351 — il sostantivo del prompt non legittima il motore

Il failure del rettangolo ha esposto un buco nel piano stesso. Anche con i mantra
"motorizza la classe" e "astraiti fino al punto fisso", l'implementazione ha
iniziato a nominare il motore dal sostantivo più visibile del prompt:
`rectangle_dimensions_*`. Questo è ancora un fix puntuale travestito da schema.

La scala corretta non parte dall'oggetto, ma dalla struttura computazionale:

`rettangolo -> forma geometrica -> coppia di dimensioni -> due variabili positive
vincolate -> somma raddoppiata + rapporto -> procedura algebrica riusabile`.

Il rettangolo, in questa classe, è solo una **configurazione KB** che dichiara:
"il mio aggregato misurato è una somma raddoppiata dei due lati; i ruoli lessicali
sono length/width; il vincolo lessicale `twice` è un rapporto 2". Il motore non
deve chiamarsi "rettangolo" e non deve contenere conoscenza del rettangolo. Deve
chiamarsi, ad esempio, `paired_dimensions_from_doubled_sum_ratio/4`, e il fatto
`schema_role_class(rectangle_dimension_schema, doubled_sum, rectangle_perimeter_role)`
deve collegare l'istanza alla procedura.

**Nuovo test di astrazione obbligatorio:** se il nome del nuovo predicato/funzione
contiene un sostantivo del prompt (`rectangle`, `apple`, `canberra`, `wwii`,
`robot`) e non è un fatto di KB o un cue-class, fermarsi. Salire di almeno due
livelli e chiedere: *quale procedura resterebbe valida cambiando il sostantivo?*
Solo quel livello può diventare motore C/procedura; il sostantivo resta dato KB.

## Mantra operativi — da eseguire PRIMA di ogni passo

Regole imposte a noi stessi per non ricadere nel fix puntuale. Prima di scrivere
*qualsiasi* riga, si passa questa checklist. Nate dagli errori reali di gen348-349.

1. **"È generalizzabile KB-first?"** — la domanda zero. Non fixare l'istanza,
   motorizza la CLASSE. Se la risposta è "sì ma è più lavoro", si fa il lavoro.
2. **Niente liste di parole nel C.** Trigger, cue, sinonimi, unità, verbi: sono
   fatti KB enumerabili (`causal_process_verb/1`, `verb_syn/2`, `time_unit/1`).
   Test: *"parrot0 può impararne un nuovo membro domani senza ricompilare?"*
   **Estensione procedure:** se il fix introduce una trasformazione o un calcolo
   di classe, non fermarti a mettere cue/template in KB: cerca prima una procedura
   insegnabile in `kb/core/procedures.p0` sopra i primitivi (`is/2`, confronti,
   termini composti). Il C deve restare adattatore NL→goal o primitiva generale,
   non consumer della procedura di dominio.
3. **Astrai fino al punto fisso.** Non moltiplicare *predicati* per una relazione
   vista attraverso verbi diversi: `wrote`/`painted`/`composed` = UNA relazione
   `created_by(Creator, Work, Verb)`, il verbo è un campo. Chiedi: *"relazione
   NUOVA o STESSA relazione sotto un'etichetta diversa?"*
4. **Il sostantivo del prompt è sospetto.** `rectangle`, `apple`, `robot`,
   `Canberra`, `WWII` possono nominare fatti, cue-class, entity/frame in KB; non
   possono nominare un motore se sopra di loro esiste una struttura più generale.
   Prima di scrivere un nome nuovo, costruisci la scala: oggetto → categoria →
   relazione → vincolo → procedura. Il motore prende il nome dalla procedura più
   alta che conserva il comportamento, non dall'oggetto campionato.
5. **Cerca il motore esistente prima di scriverne uno nuovo.** (Avevo duplicato
   `transitive_comparison` senza accorgermene.) `grep` prima di scrivere.
6. **Non estendere per analogia col codice esistente.** Se c'è già `wrote/2`,
   NON aggiungere `painted/2` per riflesso: ri-derivare dallo scheletro, o
   propaghi il debito di disaggregazione.
7. **Uccidi il muro, MAI con una risposta sbagliata.** Un errore factuale è
   peggio di un muro (dottrina no-deception). Nel dubbio, declina.
8. **Attenzione ai cue substring.** `cue()` è substring: "eat" ⊂ "f-EAT-hers".
   Per i cue discriminanti, match a PAROLA INTERA.
9. **Il wall-rate non vede le risposte sbagliate.** Quando tocchi una classe,
   ispeziona a mano anche le risposte marcate "ok".
10. **Nessuna risposta prima di aver soddisfatto il piano.** Se il prompt contiene
   più richieste coordinate, il modulo deve costruire un `answer_plan` o declinare.
   Vietato rispondere al primo subgoal e ignorare il resto.
11. **Il formato è un vincolo semantico.** "two-line", "three ways", "one
    sentence", "simple terms", "as a list" vive in KB come `format_constraint/2`
    o relazione equivalente. Il post-shaper deve provare il formato.
12. **Ogni numero deve avere un ruolo.** Prima di fare aritmetica, lega i numeri a
    slot (`total`, `unit_price`, `paid`, `width`, `length`, `range_low`). Se non
    sai il ruolo, non calcolare.
13. **Preferisci event frames ai fatti sparsi.** Una domanda su WWII non è solo
    `ended_in(world_war_ii, 1945)`: è un evento con anno, luogo, attori, cause,
    conseguenze e oggetti correlati. La KB deve crescere per frame interrogabili.
14. **Ogni collisione diventa una guardia teachable.** Se un frame generico vince
    su uno specifico, non riordinare a mano soltanto: aggiungi un cue/registro KB
    che fa declinare il generico davanti alla classe compositiva.
15. **Un failure LLMSCORE vale come seed di fuzzing.** Dopo il fix, genera varianti
    della classe: sinonimi, ordine invertito, numeri diversi, multiword entities,
    formato diverso. Il test non deve coprire il prompt, ma il fascio.

## Evoluzione KB richiesta per LLMSCORE-max

La KB di parrot0 deve passare da "grande dizionario di fatti interrogabili" a
"substrato di piani, procedure e frame compositivi". Le nuove famiglie di fatti
da far crescere sistematicamente:

| Famiglia KB | Scopo | Esempio |
|---|---|---|
| `format_constraint/2` + `format_realizer/2` | rendere verificabile il formato richiesto | `format_constraint(two_line, "two-line")` |
| `answer_plan/3` | mappare un task composto a subgoal obbligatori | `answer_plan(capital_and_reason, [capital, reason])` |
| `quantity_role/3` | assegnare ruolo ai numeri prima del calcolo | `quantity_role(change_problem, paid, "$20")` |
| `schema_role_class/3` | collegare uno schema concreto a ruoli astratti senza nominare l'oggetto nel motore | `schema_role_class(rectangle_dimension_schema, doubled_sum, rectangle_perimeter_role)` |
| procedure in `procedures.p0` | portare calcoli combinatori fuori dal C | `digit_count_between(D,Lo,Hi,N)` |
| `event_frame/1` e `event_attr/3` | interrogare eventi come pacchetti coerenti | `event_attr(wwii, atomic_bomb_cities, ...)` |
| `relation_registry/*` | dichiarare quali relazioni un motore può usare | `analogy_relation(used_for)` |
| `compound_guard/2` | far declinare frame generici se c'è un task più ricco | `compound_guard(answerframe, border_intersection)` |

Questa crescita è "a pioggia" solo se ogni fatto aumenta il prodotto cartesiano:
un nuovo `format_constraint` vale per poesia, story, explain, advice; una nuova
procedura di range vale per digit-count, letter-count, prime-count; un event frame
vale per anno, luoghi, cause e domande composte.

## Nuovo ciclo operativo LLMSCORE

1. **Leggi il failure come distribuzione.** Non chiedere "come rispondo a questa
   frase?", ma "quale famiglia di frasi avrebbe lo stesso errore?".
2. **Classifica l'errore:** wall, fatto mancante, procedura mancante, formato,
   routing collision, compound incompleto, risposta falsa.
3. **Scala di astrazione prima dei nomi:** scrivi la catena oggetto → categoria
   → relazione → vincolo → procedura. I nomi C/procedure possono nascere solo dal
   livello più alto che continua a risolvere la classe; i nomi-oggetto restano KB.
4. **Disegna il frame KB:** cue, slot, subgoal obbligatori, template, procedure.
5. **Collega il C solo come adattatore:** parse NL→goal, ordering, slot binding,
   primitive generali. Nessuna parola naturale nuova nel C.
6. **Espandi a pioggia:** aggiungi fatti/procedure/registri che coprono varianti
   future della classe, non solo l'istanza.
7. **Ablazione runtime:** assert/retract del cue o registro che prova la crescita
   senza rebuild.
8. **Fuzz focused:** 5-20 varianti della classe, inclusi ordini e sinonimi.
9. **Ricontrollo manuale delle "ok":** wall-rate verde non basta.

## Perché i fix puntuali perdono

Il giudice campiona 10 domande da una distribuzione illimitata. I run osservati
condividono ~0 domande. Una coppia `qa_cue/qa_reply` copre misura-zero →
valore atteso ~0 sul run successivo. Ottimizziamo il **punteggio atteso sulla
distribuzione**, non il prossimo campione di 10. Dopo gen351 questo vale anche
per i **fix di classe troppo stretti**: una batteria interna può diventare verde
mentre il giudice campiona fuori dal bordo. Ogni fix deve allargare il bordo.

## Tassonomia delle classi e stato (baseline)

| Classe | Meccanismo | Copertura | Stato |
|---|---|---|---|
| Lessicale (rime/anagrammi) | motore `wordquery` + `lexeme` | qualsiasi parola | ✅ robusto |
| Aritmetica / word-problem | motore `wordproblem` | qualsiasi calcolo | ✅ robusto |
| Deduzione formale (sillogismi) | motore no-overlap (A+E) | forme A/E | 🟡 parziale |
| Generazione creativa | composer + fatti-tema | solo temi con fatti | 🟡 semi |
| Causale ("perché/come") | lookup `explanation/because` | ~30 istanze | ❌ fragile |
| Enciclopedico (fatti) | fatti a mano | capitali 175, resto sottile | ❌ fragile |
| Riddle laterali | lookup `qa_reply` | 6 istanze | ❌ fragile |
| Pragmatica / persona | insieme di fatti persona | comuni | 🟢 ok |
| Formato/post-shaping | vincoli parziali | alcune forme | ❌ fragile |
| Compound question | primo frame che vince | A o B, raramente A+B | ❌ fragile |
| Algebra schema | arithmetic generica | calcoli scalari | ❌ fragile |
| Event frames | fatti isolati | anno/capitale singoli | ❌ fragile |

**Insight:** vinciamo dove abbiamo un motore su KB ampia; perdiamo dove abbiamo
un lookup su tabella sparsa.

## Leva combinatoria (da `generative-leverage.md`)

Un motore non ha bisogno di *migliaia* di fatti: ha bisogno di **fatti atomici
componibili** il cui prodotto cartesiano copre la classe.

- **La forma dell'output è già nel prompt** → il motore fa *selezione vincolata*,
  non recupero. Risolve un predicato parzialmente istanziato (es.
  `haiku(ocean, $O, $M, $C)`) e sceglie una tupla valida.
- **Prodotto, non somma:** 3 open × 3 mid × 3 close = 27 haiku; ~17 atomi
  narrativi = 486 storie. Aggiungere una variante è UN fatto; aggiungere uno
  slot è un fatto + un arco.
- **Garanzie per costruzione:** grammatica (linee pre-composte), coerenza
  (atomi etichettati per ruolo/tema), novità (l'assemblaggio esatto non è mai
  pre-scritto).
- **Corollario per la Fase 1 causale:** una spiegazione si *compone* da atomi
  relazionali (`cause` + `mechanism` + `purpose` + catena `is_a`), non da una
  stringa monolitica per topic → stessa leva prodotto, stessa economia.

Regola pratica: ogni classe generativa = UN motore + fatti atomici componibili.
L'anti-pattern è un modulo (o una `qa_reply`) per istanza.

## Le tre leve

- **(a) Motori generativi** che assemblano la risposta da relazioni strutturate
  (query sul grafo KB), così *qualsiasi* istanza della classe è tentabile.
- **(b) Ingestione per processo** (pipeline `kb/learning/` già esistente): la
  breadth viene dall'ingestione da Wikipedia statica, non dal digitare fatti.
- **(c) Uccidere il muro.** Ogni "I don't understand that yet" è uno 0 garantito
  *e* un tell squalificante. Tentativo plausibile in-dominio > muro — **senza
  inventare fatti falsi**; si declina solo in modo LLM-plausibile e onesto.

## Le fasi (ordinate per valore atteso)

### Fase 0 — Misura la CLASSE, non il campione *(prerequisito)*
Batteria di sonde per-classe (`tests/llmscore-probes/`), held-out, **mai
fixate singolarmente**. Metro oggettivo e senza-API: **wall-rate per classe**
(rileva le frasi-muro note). Un secondo metro opzionale con giudice-LLM per la
qualità. Deliverable: `make llmscore-probe` → tabella copertura per classe.

Frasi-muro rilevate: `I don't understand`, `I don't know about … yet`,
`Want me to learn`, `not sure`, `no idea`, `haven't learned`.

### Fase 1 — Motore causale "perché/come" *(EV più alto)*
Parser generico `why does X <verb>?` / `how does X work?` → query su
`cause/causes/mechanism/purpose/enables` + catene `is_a/has_part` → compone una
frase causale. Poi estrazione bulk cause→effetto dalle wiki-pages
(`docs/plans/extract-knowledge-from-prose.md`). Da ~30 a migliaia, per processo.

### Fase 2 — Ampiezza enciclopedica per ingestione
Autolearn puntato su classi di entità ad alta frequenza: superlativi
("il più alto/lungo"), autori↔opere, invenzioni↔date, capitali (già). Recall
factuale scala col processo.

### Fase 3 — Completezza deduttiva *(parziale)*
Quadrato delle opposizioni completo (all/some/no × aff/neg) + catene 2-3 passi.
Investimento limitato, alta certezza: copre l'intera classe puzzle-logici.
- [x] **Forma-E con istanza** ("No A are B. Z is a B. Is/Can Z a A? → No"):
      un pattern copre fish/whale, reptiles/snake, cats/Rex. deduction 50%→20%.
- [ ] Barbara con predicato ("All A are B. All B need W. Do A need W? → Yes").
- [ ] Darii ("Some A are B. All B are C. Are some A C? → Yes").
- [x] **RISPOSTE SBAGLIATE corrette**: "birds have feathers → Yes" (il cue
      substring `eat` scattava su "f-EAT-hers" → match a parola intera + Barbara
      con istanza); "taller-than → C/A" (esteso `transitive_comparison` alla
      forma superlativa "who is the shortest/tallest?", estremo per polarità
      dello stem comparativo vs superlativo).

> **Limite del metro (Fase 0):** il wall-rate conta i muri, NON le risposte
> sbagliate. Una risposta errata è *peggio* di un muro (dottrina no-deception)
> ma non appare nel wall-rate. Serve un secondo metro (giudice o oracolo per-
> classe) che marchi le risposte errate. Fino ad allora, ispezionare le
> risposte "ok" a mano quando si tocca una classe.

### Fase 4 — Layer di framing/formato (universal-input)
Verbi di cornice (describe/explain/write/tell) e vincoli (in one sentence /
in N words / as a list) trattati come **modificatori** instradati sul contenuto,
mai come blocchi. Post-shaper generico impone il formato.

### Fase 5 — Fallback anti-muro
Ultimo stadio generativo che ingaggia la domanda in-dominio invece di murare.

### Fase 6 — Answer planner per domande composte *(nuova priorità alta)*
Costruire un planner leggero che riconosce cue coordinati e produce subgoal
obbligatori. Esempi:

- `capital(X)` + `why_created_as_compromise(X)` → risposta con capitale **e**
  motivazione;
- `event_end_year(world_war_ii)` + `event_attr(world_war_ii, atomic_bomb_cities, Cities)`;
- `price_for_quantity` + `change_from_payment`.

Il planner non deve inventare: se un subgoal manca, deve dire quale parte sa e
quale manca, non fingere completezza. La forma della risposta sta in
`response_template` o in un composer di subgoal.

### Fase 7 — Format contract layer *(nuova priorità alta)*
Separare contenuto e rendering. Il motore produce una struttura (`poem_line/2`,
`continuation/3`, `explanation/1`), il post-shaper applica il vincolo:

- due righe = output con newline reale e due unità;
- tre modi = tre elementi distinti;
- one sentence = un solo periodo;
- simple terms = lessico breve, niente prova lunga;
- exactly N words = verificatore già esistente come procedura di formato.

Il contratto deve essere testabile: non basta contenuto corretto.

### Fase 8 — Algebra e word-problem schemas
Portare in KB gli schemi ricorrenti, non solo le operazioni:

- coppia di grandezze con aggregato e rapporto:
  `paired_dimensions_from_doubled_sum_ratio(DoubledSum,K,Small,Large)`;
  il rettangolo è solo una configurazione KB dello schema, non un motore;
- unit price + total quantity + change;
- mixture/ratio/rate multi-step;
- digit/range counting.

Il C lega quantità e ruoli; `procedures.p0` risolve. Ogni schema deve avere prove
con numeri diversi e varianti lessicali insegnabili.

### Fase 9 — Event-frame ingestion
L'enciclopedico LLMSCORE non chiede solo fatti singoli. Serve ingestione verso
frame:

```
event_frame(world_war_ii).
event_attr(world_war_ii, end_year, 1945).
event_attr(world_war_ii, atomic_bomb_cities, cons(hiroshima, cons(nagasaki, nil))).
event_attr(canberra, created_as_compromise_between, cons(sydney, cons(melbourne, nil))).
event_attr(canberra, compromise_reason, "Sydney and Melbourne both wanted the capital, so a planned inland capital was chosen between them").
```

La pipeline deve preferire fatti collegati dallo stesso evento, perché il giudice
campiona domande con più slot correlati.

### Fase 10 — Oracolo qualità e anti-wrong
Il wall-rate resta utile ma subordinato. Servono due metriche nuove:

- **completion-rate:** percentuale di subgoal obbligatori soddisfatti;
- **wrong-rate:** risposte factualmente o semanticamente errate.

Finché non c'è giudice automatico, ogni failure LLMSCORE aggiunge un oracle
deterministico per la classe (`has`, `lacks`, formato, numeri, subgoal).

## Criteri di misura (definizione di "fatto")

- **Wall-rate per classe** ↓ (metro primario Fase 0, senza API).
- **Copertura per classe %** (attempt plausibile) tracciata run-su-run.
- **Completion-rate per compound question** ↑: tutti i subgoal richiesti presenti.
- **Format-pass rate** ↑: il rendering soddisfa il vincolo esplicito.
- **Wrong-rate** ↓: risposte sbagliate contano più dei muri.
- LLMSCORE ufficiale come controllo esterno, NON come metro di sviluppo
  (non-deterministico, campione piccolo).

## Ordine di esecuzione raccomandato

Fase 6 + Fase 7 prima: gli ultimi run mostrano che il collo di bottiglia non è
più solo "muro", ma incompletezza e formato. Fase 8 subito dopo per tagliare le
risposte numeriche sbagliate. Fase 9 in parallelo come crescita KB a pioggia.
Fase 10 deve accompagnare ogni modifica.

## Bug noti (rimandati) — da riprendere

Trovati durante gen349 e consapevolmente NON risolti ora. Ognuno viola almeno un
mantra; annotati per riprenderli senza riscoprirli.

- **B1 — il decomposer spezza unità semantiche su " and ".** "Romeo and Juliet"
  → "romeo" + "juliet"; e i word-problem multi-frase vengono divisi. Aggravante:
  `mod_wordproblem` legge il turno intero da `input`, non il sub-clause → rispose
  due volte ("15 dollars. 15 dollars."); tamponato con un dedup, ma la radice è
  che il decomposer divide ciò che è UNA unità. Serve una guardia: non decomporre
  quando il turno è un problema/entità singola (numeri+cue matematico, o titolo).
- **B2 — RISOLTO gen350: `answer_frame`/`created_by` fallivano sui titoli multi-parola sotto
  "the".** "Mona Lisa", "Starry Night" murano (Guernica, David — parola singola —
  funzionano). Un blocco precedente in `mod_knowledge` intercetta "the <X multi>";
  inoltre il motore `created_by` è piazzato tardi. Serve: estrazione del titolo
  multi-parola + anticipare il motore. Ora `created_by` normalizza le forme del
  verbo via `creation_verb_form/2` e salta i determinanti con `p0_lead_det()`.
- **B3 — `kb_match` arity-1 completamente LEGATO non riporta l'hit.** Una query
  `time_unit(hour)` con arg legato torna 0 anche su match; ho dovuto ENUMERARE e
  confrontare. Quirk del motore KB: verificare se è un bug generale di `kb_match`.
- **B4 — RISOLTO gen350: "make a word from the letters r,e,a,d" va a `mod_compose`** invece di
  `mod_wordquery` (precedenza di routing: "make a word" → code synth). Non è un
  muro (risposta non-muro), ma è una non-risposta. Il routing lessicale copre la
  probe e l'anagramma esplicito `anagram_of/2` vince sull'enumeratore.
- **B5 — `cue()` è substring, ovunque.** Fixato eat/feathers, ma il pattern può
  ripresentarsi su altri cue discriminanti. Valutare un `cue_word()` a parola
  intera come default per i cue discriminanti.
- **B6 — sillogismo affermativo con pronome mangia il testo.** "Every square is a
  rectangle. This shape is a square. Is it a rectangle?" → "Yes -- Is is a
  Rectangle" (giusto ma sgrammaticato: "it" diventa "Is").
- **B7 — RISOLTO gen350: le parole-cue della dieta (eat/eats/diet/food) erano una lista in
  C** (le ho rese whole-word ma restano cablate) → viola mantra #2, portarle in KB.
  Ora sono `diet_cue/1`, con test assert/retract runtime.
- **B8 — il wall-rate non conta le risposte sbagliate** (vedi Fase 0). Serve un
  secondo oracolo per-classe che marchi gli errori.
- **B9 — RISOLTO gen350: estrazione CREATION solo attiva.** "The Odyssey was written by Homer"
  (passivo) non viene estratto (declina, non sbaglia). Aggiungere il frame
  passivo "O was <verb>ed by S" → `created_by(S, O, verb)`. Ora il passivo usa
  `creation_verb_form/2` + `creation_passive_agent_marker/1`.
- **B10 — messaggio "Learned: mammal.(platypus)"** ha un punto spurio nel nome
  predicato (cosmetico, pre-esistente all'estrazione copula).
- **B11 — formato poesia ignorato.** "two-line rhyming poem" genera una sola riga.
  Serve Format contract layer: newline reale + conteggio righe.
- **B12 — wordproblem rettangolo collassa a scalare.** "perimeter 24, length twice
  width" risponde `48`. Serve schema algebraico con ruoli e output dimensionale.
- **B13 — domanda composta capital+why tronca al primo fatto.** `answerframe`
  risponde Canberra e ignora la spiegazione storica. Serve answer planner e guardia
  contro first-frame incomplete.
- **B14 — prezzo+change tronca al primo calcolo.** Serve ledger quantità con
  subgoal obbligatori: prezzo totale e resto.
- **B15 — event bundle WWII incompleto.** L'anno è noto, le città atomiche no o non
  vengono aggregate. Serve `event_frame/event_attr`.
- **B16 — digit count fra range usa aritmetica sbagliata.** Serve procedura
  combinatoria/enumerativa `digit_count_between/4`, non calcolo generico.

## Stato

- [x] Fase 0: batteria sonde + `make llmscore-probe` + baseline wall-rate.
      **Baseline (2026-07-22): 31% wall overall (48/70 risposte).**
      Per classe: deduction 50%, causal 40%, factual 40%, arithmetic 30%,
      creative/lexical/pragma 20%. Conferma le priorità.
- [x] Fase 1: motore causale + ingestione cause→effetto.
      **Diagnosi:** il motore ESISTE (`10-memory-knowledge.c:4302`, key =
      content-word joined → `because/2`/`explanation/2`). I wall causali sono:
      (a) "how does X form/work?" non instrada al motore (solo "why");
      (b) key brittle: `rainbow_form` ≠ `rainbow_appears` (verb-synonymy);
      (c) fatti mancanti (hiccups, tides, stars_twinkle, onions).
      **Cautela:** match a sovrapposizione lasca dà risposte SBAGLIATE
      (moon→`moon_glows` per "tides") — peggio di un muro. Il fix va fatto
      con canonicalizzazione verbo-sinonimi + guardia `make llmscore-probe`,
      NON con overlap ingenuo. Prossimo passo: (1) routing "how does X"
      sicuro; (2) ingestione cause→effetto subject-keyed da `kb/learning/pages`.
- [x] **gen350:** routing "how does X form/cause/work" e causal lookup robusto
      coprono la batteria causale: 10/10 answered, 0% wall. Le forme restano KB:
      `causal_process_verb/1` e `verb_syn/2`; i fatti mancanti sono `because/2`.
- [x] Fase 2: ingestione enciclopedica mirata. **AVVIATA (gen349):** estrazione
      CREATION transitiva KB-first in `extract_class_statement` — la prosa
      "S <creation-verb> O" → `created_by(S,O,Verb)`, verbi da `creation_verb/1`.
      Testato end-to-end: "Claude Monet painted Water Lilies" → fatto estratto →
      "Who painted Water Lilies?" → "Claude monet". La base factual cresce per
      PROCESSO, non a mano. **gen350:** aggiunto passivo `O was <form> by S`
      tramite `creation_verb_form/2` e marker KB; aggiunto `created_by` per
      `romeo_and_juliet`; "Who painted the Mona Lisa?" e "Who wrote Romeo and
      Juliet?" rispondono via relazione astratta.
- [x] Fase 3: quadrato opposizioni completo sulla batteria: deduction 10/10,
      0% wall.
- [x] Fase 4: layer framing/formato sulla batteria: `concise_explain/3` ora copre
      anche richieste vincolate da word-count come "exactly three words"; advice
      usa `intent_cue(activity_request, ...)` + `activity_topic/activity_step`.
- [x] Fase 5: fallback anti-muro sulla batteria: creative/pragmatic gaps chiusi
      con motori KB (`metaphor_line/metaphor_topic`, `story_scene`,
      `continuation_template`, `activity_step`) e test runtime-growth
      `tests/motorize_class.sh`.

**Risultato gen350:** `make llmscore-probe` = **0/70 wall, 70/70 answered**.
Per classe: arithmetic/causal/creative/deduction/factual/lexical/pragma = 0%.

**Aggiornamento gen351:** LLMSCORE ufficiale successivo = **4/10** nonostante il
probe interno verde. Diagnosi: il piano ha vinto la prima frontiera (muri) ma non
ancora la seconda (completezza, formato, wrong-rate). Da qui la missione diventa
LLMSCORE-max sotto alta varianza: answer planner, format contract, algebra schema,
event frames e oracolo qualità.
