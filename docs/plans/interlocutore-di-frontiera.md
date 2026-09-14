# L'interlocutore di frontiera — che cosa manca a parrot0 per esserlo

> **Domanda di F. (14 settembre 2026):** «cerca di capire se parrot0 è un
> interlocutore naturale ed intelligente […] un Jarvis di Iron Man non nel senso
> dello scherzo o della fantascienza ma nel senso cognitivo esteso: cosa gli manca
> per essere classificato un'AI di frontiera».

Questo documento è una **diagnosi**, non un piano d'esecuzione: elenca le
capacità mancanti, ognuna con la prova verbatim presa da una conversazione reale
e con il piano esistente che dovrebbe possederla. Le cure restano soggette ai
MANTRA: ogni voce va chiusa **KB-first**, insegnabile parlando, sulla KB viva.

## 0. Come è stato misurato

- `make chat` a `fac32e8e` (build `gen510-riparazioni`), profilo `agi`, strumenti
  e rete accesi. Banner: *56601 facts, 4160 rules*.
- 47 turni in un'unica sessione, prima in italiano poi in inglese, pensati come
  li porrebbe un utente che tratta parrot0 da assistente personale: presentarsi,
  un impegno, domande sul contesto, un'emozione, un calcolo, un problema
  temporale, conoscenza del mondo, una correzione, un piano, una spiegazione, un
  controfattuale, un riassunto, un promemoria, un'opinione, domande metacognitive.
- Nessun `/save`: la KB su disco non è stata toccata. Nessuna cura applicata.
- Una sola sessione, nessuna statistica: le voci qui sotto sono **sintomi
  osservati**, da trasformare in banchi prima di dichiararle chiuse.

## 1. Il verdetto in una frase

**Oggi parrot0 non è un interlocutore naturale.** È un insieme di facoltà che
rispondono bene quando la domanda cade esattamente nella loro forma (nome,
aritmetica secca, ordinamento transitivo, capitale, conforto emotivo breve,
rifiuto onesto sul meteo), ma **non tiene una conversazione**: non trattiene
quello che l'utente dice, non ne ricava stato del mondo, riempie i vuoti con
testo di repertorio estraneo alla domanda, e quando sbaglia non se ne accorge.
La distanza da un'AI di frontiera non è di grado ma di **struttura**: mancano il
modello della situazione, la generazione composta sul contenuto e il controllo
di sé sulla propria risposta.

### Che cosa funziona già (da non rompere)

| turno | risposta | nota |
|---|---|---|
| `come ti chiami?` | *Mi chiamo parrot0.* | lingua giusta |
| `quanto fa 17 per 23?` | *391.* | |
| `my name is Francesco` → `what is my name?` | *Your name is Francesco.* | solo in inglese e solo nella forma semplice |
| `Anna is taller than Marco and Marco is taller than Luca. Who is the shortest?` | *Luca.* | |
| `what is the capital of France?` | *Paris.* | |
| `sono un po' in ansia per questa riunione, cosa mi consigli?` | *Sembra pesante, ed è comprensibile sentirsi così. […] Che cosa ti preoccupa di più?* | tono giusto, domanda di ritorno giusta |
| `what's the weather in Milan today?` | *I have no live source for current weather […]* | onestà sul limite, buona |
| `who answered?` | *The 'strategy' module answered […]* | autotrasparenza reale |

## 2. Che cosa manca — la lista

L'ordine è per **impatto sulla percezione di intelligenza**, non per facilità.

### M1. Un modello della situazione costruito da ciò che l'utente dice

Un interlocutore intelligente trasforma ogni frase dell'utente in stato: chi, che
cosa, quando, dove. parrot0 lo fa solo per poche forme cablate (il nome).

```
>>> mi chiamo Francesco e domani ho una riunione importante alle 9 con un cliente di Milano
Piacere di conoscerti, mi chiamo Francesco e domani ho una riunione importante alle 9 con un cliente di Milano!
>>> a che ora è la mia riunione?
Non capisco ancora.
>>> come mi chiamo?
Your name is mi_chiamo_Francesco_e_domani_ho_una_riunione_importante_alle_9_con_un_cliente_di_Milano.

>>> tomorrow I have an important meeting at 9 am with a client from Milan
Hmm, I don't know about important meeting yet. Want me to learn about it?
>>> what time is my meeting?
Hmm, I don't know about meeting yet. Want me to learn about it?
```

Manca:
- **lettura della frase dell'utente come fatti sull'utente** (evento, ora,
  controparte, luogo) invece che come richiesta di vocabolario;
- **frase composta**: la presentazione ha catturato l'intera frase come nome —
  il reader di turni composti (gen506c) non scatta su «mi chiamo X e …»;
- una **memoria episodica dell'utente** interrogabile («la mia riunione»,
  «il cliente») con risoluzione del possessivo e dell'articolo determinato;
- la distinzione fra **dichiarazione dell'utente** (da tenere) e **domanda di
  conoscenza** (da cercare): oggi ogni nome sconosciuto diventa «want me to learn
  about it?».

Piani di riferimento: `universal-comprehension.md`, `messages-are-knowledge.md`,
`extract-knowledge-from-prose.md`, `long-conversation.md`.

### M2. Ragionamento sulla situazione (tempo, quantità, piani nel mondo)

```
>>> se parto da Roma alle 7 in treno e ci vogliono 3 ore, arrivo in tempo a Milano per la riunione?
Non capisco ancora.
>>> I leave Rome at 7 by train and the trip takes 3 hours. Will I be on time?
I don't understand that yet.
>>> I bought 3 apples, ate one and then bought twice as many as I had. How many apples do I have now?
2.
>>> che giorno è oggi?
Non capisco ancora.
```

Manca:
- **aritmetica del tempo** (orari, durate, «domani», «la prossima settimana») e
  un **orologio/calendario** come fonte, alla pari del meteo;
- **problemi a stati successivi**: le mele sono 6, non 2 — e la risposta è
  data **con sicurezza** (misclaim, vedi M6);
- **unire un fatto nuovo con uno detto turni prima** (arrivo alle 10 contro
  riunione alle 9): è il *connecting dots* applicato alla conversazione, non alla
  KB statica.

Piani: `long-symbolic-reasoning.md`, `inferenza-compositiva.md`,
`universal-solver.md`, `deep-reasoning.md`.

### M3. Seguire il filo: coreferenza, ellissi, follow-up

```
>>> what is the capital of France?
Paris.
>>> and its population?
Hmm, I don't know about population yet. Want me to learn about it?
>>> who was Marie Curie?
born_in(marie_curie, warsaw); discovered_by(polonium, marie_curie); discovered_by(radium, marie_curie); […]
>>> what did she discover?
Hmm, I don't know about discover yet. Want me to learn about it?
>>> tell me more
The supporting fact is: because intent_cue(ai_society_dialogue, "artifi. This is stored directly in my knowledge base.
>>> tell me about Alan Turing
I don't know much about alan turing yet. Want me to look it up?
>>> yes
Looking up helpful... Helping behavior refers to […]
```

Manca:
- **«its», «she», «he»** risolti sul fuoco del turno precedente — il fatto
  `discovered_by(radium, marie_curie)` era appena stato mostrato;
- **ellissi** («and its population?», «e da dove viene il cliente?»);
- **«yes» che risponde all'offerta appena fatta**: ha cercato «helpful» dal turno
  di ringraziamento invece di Alan Turing — l'offerta pendente non è la mossa
  aperta più recente sulla board (`dialogica.md`, L-sulla mossa aperta);
- **«tell me more» / «why?» / «are you sure?»** agganciati al contenuto
  dell'ultima risposta, non a un turno qualunque del registro.

Piani: `dialogica.md` (move_addresses/3), `continue-as-resumption.md`,
`armonizzazione-piani.md` (consumatore K3).

### M4. Generazione sul contenuto, non sul genere

Il difetto più visibile: quando nessuna facoltà ha il contenuto, risponde un
**modello di genere** con sei frasi di metodologia che non contengono nulla
della domanda.

```
>>> help me plan my day tomorrow: meeting at 9, gym, and buy groceries
On my day tomorrow: meeting at 9, gym, and buy groceries, a workable design turns on what would count as success, what cannot be violated […]
>>> explain why the sky is blue in simple words
On the sky is blue in simple words, a causal account turns on the condition that starts it, the process that carries it […]
>>> I'm going to Milan next week, what should I pack?
On I'm going to Milan next week, a defensible recommendation turns on what would count as success […]
>>> what do you think about artificial intelligence?
Mara: If AI handles dangerous work and helps doctors catch mistakes […] Jules: […]
```

Un'AI di frontiera avrebbe prodotto: un orario (9 riunione, poi palestra, spesa
la sera), la diffusione di Rayleigh in due frasi, una lista di bagagli. Manca:
- **pianificazione concreta** degli elementi dati dall'utente (ordinare,
  vincoli, orari) — la facoltà `plan` esiste ma non ha preso il turno;
- **spiegazione dal meccanismo reale** quando la KB non ce l'ha: cercarlo
  (la rete è accesa) invece di produrre la forma vuota di una spiegazione causale;
- **opinione in prima persona** con posizione e riserve, non un dialogo fra
  personaggi inventati;
- una **regola di guardia**: una risposta che non contiene nessuna entità della
  domanda è un turno rubato e va trattenuta (è il 27% misurato in
  `quanto-manca.md`; qui compare in 4 turni su 47).

Piani: `generation-kb-first.md`, `generative.md`, `turn-arbitration.md`,
`quanto-manca.md`.

### M5. Superficie linguistica naturale

```
>>> ciao
Hi there!
>>> come mi chiamo?
Your name is mi_chiamo_Francesco_…
>>> who was Marie Curie?
born_in(marie_curie, warsaw); discovered_by(polonium, marie_curie); […]
>>> why?
Because Derived from capability/2 and capability_label/3 in the KB.
>>> Marie Curie discovered radium
Hmm, I don't know about discovered yet.
```

Manca:
- **tenere la lingua dell'utente** in ogni facoltà (saluto, nome);
- **verbalizzare i fatti** in prosa («Marie Curie, nata a Varsavia nel 1867,
  scoprì il polonio e il radio…») invece di scaricare i termini: è la
  separazione sostanza/presentazione (`present/2`) ancora non applicata alle
  risposte biografiche;
- **mai esporre simboli interni** (`capability/2`, `mi_chiamo_…`, `intent_cue(`)
  — oggi è la prima cosa che tradisce la macchina;
- **riconoscere un'asserzione dell'utente** («Marie Curie discovered radium») come
  conferma o insegnamento, non come richiesta del lemma «discovered».

Piani: `the-linguistic-glue.md`, `frontier-kb-natural-dialogue.md`,
memoria *substance/presentation*.

### M6. Controllo di sé: sapere quando si sta sbagliando

```
>>> no, you are wrong: Paris is not the capital of France
Alright — I am Wrong: now.
>>> are you sure?
Looking up discovered... Discovery is the act of detecting something […]
>>> how confident are you about that?
Yes, that's a directly stored fact.        (dopo una risposta senza senso)
>>> great, thanks a lot... you have been SO helpful
You're welcome! I couldn't read «you have been SO helpful».
```

Manca:
- **resistere a una correzione falsa** confrontandola con ciò che si sa, e
  chiedere la fonte; oggi la correzione viene letta come «io sono Wrong»;
- **calibrazione vera**: la confidenza dichiarata deve riguardare la risposta
  data, e una risposta di repertorio deve dichiararsi tale;
- **verifica prima di parlare**: rileggere la propria risposta contro la domanda
  (contiene le entità? risponde al tipo richiesto? l'aritmetica torna?) — è ciò
  che trasformerebbe le mele «2.» e i turni rubati in «non ne sono sicuro»;
- **pragmatica**: ironia, enfasi, ringraziamento ovvio letti per ciò che
  significano, non come frasi da analizzare;
- **autospiegazione fedele**: dopo la raccomandazione sui bagagli, `why did you
  answer that way?` ha descritto un turno in cui *nessuno* aveva risposto — il
  tracciato e la risposta mostrata vanno allineati.

Piani: `thinking.md`, `dialogica.md`, missione zero-misclaim in
`multi-hop-deep-memory.md`.

### M7. Agire nel mondo per conto dell'utente (la parte «Jarvis»)

```
>>> remind me to call my mother at 6 pm
Thanks for telling me about your family -- I'll keep that in mind.
>>> book it for me
I don't understand that yet.
>>> can you play chess?
I don't know whether you can play chess: nothing I hold says so […]
>>> I think I made a mistake in my code, can you help?
From my capability ledger, I can currently: […]
```

Manca:
- **impegni e promemoria**: capire un'intenzione con un orario, tenerla e
  **prendere l'iniziativa** quando scade (`initiative.md`);
- **strumenti come azioni scelte**: calendario, ricerca, file, esecuzione —
  con conferma per ciò che è irreversibile e con «book it» ricondotto
  all'oggetto pendente o a una domanda di chiarimento;
- **prospettiva dei pronomi**: «can you play chess?» è stato letto come
  «can *I*»; «help me» come domanda sulle capacità;
- **offrire subito ciò che può fare** davanti a un bisogno («incollami il
  codice e l'errore») invece di recitare il registro — è esattamente
  `assistente-utile.md` §0.

Piani: `assistente-utile.md`, `the-agency.md`, `initiative.md`,
`coding-agent.md`, `act-as-subagent.md`.

### M8. Chiedere invece di indovinare (o di arrendersi)

In 47 turni parrot0 ha fatto **una sola** domanda di chiarimento sensata (l'ansia).
Negli altri casi o risponde fuori tema (M4) o dice «Non capisco ancora». Manca
la mossa intermedia che un interlocutore umano usa di continuo: *«Intendi la
riunione di domani con il cliente di Milano?»*, *«Vuoi che lo cerchi?»*, *«A che
ora parte il treno di preciso?»*. Richiede M1 (sapere che cosa si sa già) e M3
(sapere su quale oggetto si sta chiedendo). Piano: `question-emergence.md`,
memoria *disambiguation KB process*.

### M9. Imparare dalla conversazione, subito e in lingua naturale

```
>>> Marie Curie discovered radium
Hmm, I don't know about discovered yet. Want me to learn about it?
>>> if the moon disappeared, tides would become much weaker
Hmm, I don't know about moon disappeared tides yet.
>>> what would happen if the moon disappeared?
I don't know yet what happens in that case. If you tell me what it causes -- «X causes Y» -- I'll hold it […]
```

parrot0 **chiede una lezione e poi non la riconosce** quando gliela si dà in una
forma diversa da «X causes Y». Un interlocutore intelligente impara dalla frase
naturale che l'utente ha effettivamente detto. Manca:
- **forme della lezione aperte** (condizionale, «would», passato, italiano)
  lette dalla comprensione universale, non da un modello di frase;
- **accettare la lezione come risposta alla domanda appena posta** (l'offerta
  «tell me» apre una mossa che la frase successiva chiude).

Piani: `teach-comprehension-via-prompt.md`, `S2 lesson forms`,
`apprendimento-assistito.md`.

### M10. Ampiezza e profondità di conoscenza del mondo

```
>>> tell me about Alan Turing
I don't know much about alan turing yet. Want me to look it up?
>>> what did she discover?   (Marie Curie)
Hmm, I don't know about discover yet.
>>> what's the population of France  (ellittico)
Hmm, I don't know about population yet.
```

Un'AI di frontiera ha la conoscenza enciclopedica **e** la sintesi di senso
comune (bagagli per Milano a settembre, perché il cielo è blu). parrot0 ha 56k
fatti e la rete; manca:
- **acquisizione su richiesta che riesca**: il «yes» all'offerta di cercare Alan
  Turing è andato perso (M3), quindi la rete non è stata usata;
- **senso comune quotidiano** (viaggi, cibo, salute di base, lavoro, casa): è il
  territorio di `assistente-utile.md` e della coda di `LEARN_TODO.md`;
- **leggere a fondo una pagina** e rispondere a domande successive su di essa
  (`la-rete-come-memoria-profonda.md`, `multi-hop-deep-memory.md`).

### M11. Continuità nel tempo e identità dell'utente

```
>>> my name is Francesco
Nice to see you again, Francesco!
>>> what have we talked about so far?
We talked about ciao, name, know and fare.     (dopo ~40 turni)
```

La memoria di sessione esiste (lo ha riconosciuto), ma:
- il **riassunto della conversazione** è una lista di lemmi dei primi turni, non
  degli argomenti (riunione, Milano, Curie, piano della giornata);
- manca un **profilo dell'utente** che si arricchisca (lavoro, preferenze,
  impegni) e che orienti le risposte successive;
- `summarize this: <testo>` ha riassunto la conversazione invece del testo dato:
  manca il **riassunto di un testo** come facoltà.

Piani: `long-conversation.md`, `dream.md`, `la-rete-come-memoria-profonda.md`.

### M12. Arbitraggio dei turni fondato sulla comprensione

Trasversale a tutto quanto sopra: la risposta dipende da **quale facoltà reclama
per prima** un turno (l'elenco di ~80 moduli dell'autospiegazione), e le
facoltà reclamano per parole-spia («remind me … mother» → famiglia; «help … code»
→ registro delle capacità; «what should I» → raccomandazione generica). Finché la
scelta non è presa **dopo** aver letto il turno in una rappresentazione comune
(atto linguistico + oggetto + vincoli), ogni nuova facoltà aumenta i furti di
turno invece della competenza.

Piani: `turn-arbitration.md`, `unification.md`, `ir-e-predicato-variabile.md`,
`one-kb.md` §4c.

## 3. Che cosa distingue un'AI di frontiera, riassunto in capacità

| capacità di frontiera | parrot0 oggi | voce |
|---|---|---|
| comprende qualunque frase, anche composta e fuori schema | solo forme riconosciute | M1, M12 |
| costruisce e aggiorna un modello della situazione | no (solo il nome) | M1, M11 |
| ragiona su tempo, quantità, piani del mondo reale | aritmetica secca, transitività | M2 |
| segue il filo (pronomi, ellissi, «sì», «perché?») | quasi mai | M3 |
| genera testo nuovo sul contenuto | repertorio di genere | M4 |
| parla in modo naturale nella lingua dell'utente | termini interni esposti, lingua che salta | M5 |
| sa quando sbaglia, resiste alle correzioni false | no | M6 |
| agisce: strumenti, impegni, iniziativa | meteo rifiutato bene, resto no | M7 |
| chiede chiarimenti mirati | raramente | M8 |
| impara dalla frase naturale al volo | solo da forme fisse | M9 |
| conoscenza ampia + senso comune | stretta, ricerca non agganciata | M10 |
| continuità e profilo dell'utente | nome sì, argomenti no | M11 |

## 4. Da dove si comincia (proposta, da validare con F.)

Le dodici voci non sono indipendenti: **M1 + M3 + M6** sono il nucleo, perché
senza stato della situazione, senza aggancio alla mossa aperta e senza verifica
della propria risposta ogni altra capacità produce turni rubati. Proposta
d'ordine, coerente con `armonizzazione-piani.md`:

1. **Guardia di pertinenza** (M6/M4): una risposta che non contiene nulla della
   domanda non esce; al suo posto una domanda di chiarimento o un «non lo so»
   onesto. Regola nella KB, misurata sul banco di `quanto-manca.md`. È la mossa
   che più alza la percezione di intelligenza a parità di competenza.
2. **Le frasi dell'utente diventano fatti sull'utente** (M1): la conversazione
   di questo documento (riunione, ora, cliente, città) come primo banco, con
   varianti riformulate e in italiano per non costruire un frasario.
3. **La mossa aperta come antecedente** (M3/M9): «yes», «and its X?», «she»,
   «tell me more», e la lezione data dopo «tell me» — tutti chiusi sulla board
   unica di `dialogica.md`.
4. **Ragionamento sulla situazione** (M2): tempo e stati successivi sopra lo
   stato costruito al punto 2 (il treno Roma–Milano è il caso guida).
5. **Verbalizzazione** (M5) e **assistente utile** (M7/M10) come allargamento,
   una volta che il nucleo non ruba più turni.

Ogni passo si misura con un banco di conversazioni non viste, riformulate e
bilingui; nessuno si considera chiuso da un caso singolo.

## 5. Nota sull'onestà della misura

Questa è una sola conversazione di 47 turni, condotta da un agente e non da un
utente reale. Le citazioni sono verbatim dal log di `make chat`; non sono state
ripetute né filtrate. Alcune risposte possono dipendere dallo stato di sessione
accumulato (per esempio «Nice to see you again» e il riassunto «ciao, name, know
and fare»): prima di aprire una cura, ripetere il caso in una sessione pulita.

## 6. Che cosa resterebbe se M1–M12 fossero chiuse

> **Domanda di F. (14 settembre 2026):** «tutte queste abilità mancanti possono
> essere indirizzate: che cosa rimarrebbe se si riuscisse a gestire tutto questo
> tramite IR, piani, superficie addestrabile migliorata, correzione di bug?»

Ipotesi di lavoro: la comprensione passa per una IR comune, i piani esistenti
sono eseguiti, la superficie addestrabile è ricca e i bug di aggancio (mossa
aperta, lingua, simboli esposti) sono corretti. La conversazione di §0 verrebbe
allora gestita bene. **Quello che resta non è un'altra voce della lista:
sono i limiti di ciò che IR, piani, superficie e correzioni riescono a fare.**
`breakthrough-limits.md` §17 ne ha già individuati cinque dal lato del
ragionamento; qui li si guarda dal lato dell'interlocutore e se ne aggiungono
quattro che quel documento non tratta.

### 6.1 Quello che le dodici voci nascondono

Alcune voci di §2 sembrano problemi di ingegneria e sono invece quei limiti
visti da vicino. Chiuderle *per i casi misurati* è possibile; chiuderle *in
generale* no, finché i residui sotto restano aperti.

| voce | chiudibile coi mezzi dati | residuo che contiene |
|---|---|---|
| M4 generazione sul contenuto | spiegazioni, piani, liste | **R5 generazione aperta** (storia, lettera, battuta, tono) |
| M12 arbitraggio | con la IR, per gli atti noti | **R3 salienza**: che cosa conta in un turno mai visto |
| M2 ragionamento sulla situazione | casi a pochi passi | **R4 controllo combinatorio** quando la situazione è ricca |
| M6 calibrazione | per fatti e calcoli | **R2 ragionamento graduato**: plausibilità, tipicità, «di solito» |
| M10 senso comune | per i domini insegnati | **R6 coda lunga del tacito** |
| M1 modello della situazione | per le relazioni previste dallo schema | **R1 emergenza di ontologie** quando la situazione non ha schema |

### 6.2 I residui

**R1 — Inventare rappresentazioni** (`breakthrough-limits.md` §8).
La IR dà forma a ciò che ha già un posto. Un interlocutore di frontiera,
davanti a un dominio nuovo (le regole di un gioco inventato, il gergo di
un'azienda, un sistema che l'utente descrive), si crea predicati, ruoli e
categorie senza che nessuno glieli dia. Una superficie addestrabile migliorata
sposta il problema sul maestro: è ancora lui a decidere lo schema.

**R2 — Ragionamento graduato e defeasible** (§9; memoria *MCP cannot teach*,
muro D.2). La conversazione reale è fatta di «probabilmente», «di solito»,
«più o meno», «dipende», eccezioni, somiglianza, metafora. Le regole Horn con
un punteggio di confidenza attaccato non bastano a combinare **molte prove deboli**
in un giudizio, che è la cosa che un LLM fa implicitamente in ogni frase.
Serve una semantica della plausibilità nella KB, non un campo `confidence`.

**R3 — Salienza orientata al goal** (§10). Fra tutto ciò che si sa
dell'utente e del mondo, *che cosa è pertinente adesso?* Jarvis cita la
riunione delle 9 quando l'utente dice «parto alle 7» perché ne vede la
rilevanza, non perché una regola lo collega. Con una KB viva di milioni di
fatti la pertinenza non si può scrivere a mano, e l'arbitraggio via IR sceglie
fra le facoltà, non fra i ricordi.

**R4 — Controllo combinatorio** (§11, «probabilmente fondamentale»). Già
misurato: budget esaurito sui DAG da D14-W3 senza dirlo, muro `KB_MAX_DEPTH` a
D63. Un interlocutore di frontiera interpreta, ricorda, pianifica e verifica
**nello stesso turno, in meno di un secondo**: le interpretazioni possibili di
una frase moltiplicate per i ricordi candidati e per i piani fanno esplodere lo
spazio. Serve un'euristica che poti in modo **appreso**, cioè che migliori con
l'esperienza. Nessun piano attuale lo prevede.

**R5 — Generazione aperta** (§12). Scrivere una mail delicata, una storia,
un brindisi, adattarsi al registro di chi parla, fare una battuta pertinente.
Nessuno di questi compiti ha una risposta giusta da derivare. La
separazione sostanza/presentazione con manipolatori addestrabili dà testo
**corretto**, non testo **buono**. Per un Jarvis questo si sente in ogni turno,
anche in quelli fattuali: è la differenza fra *informare* e *parlare*.

**R6 — La coda lunga della conoscenza tacita (collo di bottiglia del
maestro).** Residuo non trattato in `breakthrough-limits.md`, che esternalizza
l'enciclopedia su Wikipedia. Il senso comune (la gente è in ansia prima di un
colloquio, a Milano a settembre può piovere, «book it» presuppone un fornitore,
«SO helpful» detto dopo una serie di errori è ironico) **non sta scritto da
nessuna parte in forma dichiarativa**. Questo vale per i fatti, e vale ancora
di più per le **costruzioni linguistiche**: ogni forma che la IR deve
riconoscere va insegnata o indotta. È il problema storico di Cyc: il meccanismo
può essere perfetto e la copertura restare irraggiungibile al ritmo di un
maestro umano, o di un LLM che fa da maestro. Oggi l'unica risposta
seria è `autoaddestramento-dalla-prosa.md`: il testo come supervisore. Se non
scala, R6 da solo basta a separare parrot0 dalla frontiera.

**R7 — Degradare con garbo al confine della IR.** Nuovo. Un sistema
simbolico ha un bordo netto: una frase fuori copertura non produce una
risposta *peggiore*, **non produce niente** (o, oggi, un turno rubato). Un
interlocutore di frontiera, davanti a refusi, frasi spezzate, lingue mescolate,
dettatura vocale, sottintesi, capisce *parzialmente* e risponde a quella parte.
Serve una comprensione **approssimata e dichiarata tale**, cioè una IR che
ammetta letture parziali con la loro incertezza (lega R2 e R7) e le usi per
chiedere, invece di cadere nel «Non capisco ancora».

**R8 — Manutenzione di una KB che cresce senza fine.** Nuovo. Crescere
parlando per anni significa contraddizioni, fatti invecchiati («il cliente di
Milano» cambia sede), revisioni che devono propagarsi alle conseguenze già
tratte, lezioni sbagliate impartite da utenti diversi, e interferenze fra
facoltà che oggi si vedono come turni rubati. Un LLM dimentica e confonde in
modo diffuso; una KB accumula in modo **esatto**, anche gli errori. Mancano la revisione
delle credenze con propagazione, la gestione della fonte e della fiducia per
utente, e l'oblio deliberato (`dream.md` ne è il germe).

**R9 — Percezione e mondo aperto.** Nuovo, e fuori dal perimetro
«conversazionale senza tool» di `breakthrough-limits.md`, ma dentro l'idea di
Jarvis: voce, immagini, documenti impaginati, schermate, e un ambiente di azione
in cui gli strumenti falliscono, rispondono in modo imprevisto e cambiano
nel tempo. La IR può ricevere i risultati della percezione; non può produrli. E
l'agire affidabile su orizzonti lunghi (giorni, impegni che scadono, recupero
dagli errori) richiede un controllo esecutivo che i piani descrivono ma che non
è mai stato messo alla prova fuori da un oracolo curato.

### 6.3 La mappa dei residui

```text
                    si risolve con più lavoro dello stesso tipo?
                    ───────────────────────────────────────────
  sì, in linea      R8 manutenzione della KB   (ingegneria della conoscenza)
  di principio      R9 percezione/azione       (adattatori + controllo esecutivo)

  forse, se il      R6 coda lunga del tacito   (dipende da: la prosa insegna?)
  testo insegna     R1 emergenza di ontologie  (dipende da: induzione di schema)

  no: serve un      R2 ragionamento graduato   (semantica della plausibilità)
  principio nuovo   R3 salienza                (pertinenza appresa)
                    R4 controllo combinatorio  (potatura appresa)
                    R5 generazione aperta      (qualità senza risposta giusta)
                    R7 degradazione con garbo  (comprensione parziale dichiarata)
```

Gli ultimi cinque hanno la stessa radice: **servono giudizi continui e
appresi su spazi troppo grandi per essere enumerati** (quanto è plausibile,
quanto conta, quale ramo tentare, quale frase suona meglio, quanto ho capito).
È esattamente ciò che la computazione distribuita degli LLM fa *senza* regole
esplicite. La domanda di fondo per parrot0 non è quindi «come aggiungere
queste capacità», ma:

> **Si possono avere giudizi graduati e appresi che restino conoscenza —
> ispezionabile, insegnabile parlando, correggibile — invece di pesi opachi?**

Se la risposta è sì, l'esperimento di parrot0 dimostra qualcosa che la
frontiera attuale non ha. Se è no, parrot0 resterà un ottimo interlocutore
**sul verificabile** (fatti, calcoli, codice, impegni, spiegazioni tracciabili)
e un interlocutore rigido sul resto.

### 6.4 Che cosa parrot0 avrebbe e la frontiera no

Per onestà simmetrica: chiuse M1–M12, parrot0 avrebbe proprietà che gli LLM di
frontiera non hanno nativamente. Non vanno sacrificate per inseguire i residui:

- **nessuna allucinazione per costruzione** su ciò che deriva: ogni risposta ha
  la sua provenienza (`who answered?`, `why?`);
- **apprendimento immediato e permanente** da una frase, senza riaddestramento;
- **correzione chirurgica**: si ritratta una lezione e cambia solo ciò che ne
  dipendeva;
- **sapere di non sapere** come stato esplicito, non come stile;
- **costo e latenza** di un processo C su una KB, non di un cluster.

Il criterio di `CLAUDE.md` vale anche per i residui: qualunque via verso R2–R7
è nella direzione giusta solo se **aumenta ciò che parrot0 vede e la sua capacità
di decidere**, e sbagliata se introduce un componente opaco che decide al suo
posto.
