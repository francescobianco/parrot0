# L'interlocutore di frontiera — che cosa manca a parrot0 per esserlo

> **Domanda di F. (14 settembre 2026):** «cerca di capire se parrot0 è un
> interlocutore naturale ed intelligente […] un Jarvis di Iron Man non nel senso
> dello scherzo o della fantascienza ma nel senso cognitivo esteso: cosa gli manca
> per essere classificato un'AI di frontiera».

Questo documento nasce come **diagnosi**: elenca le
capacità mancanti, ognuna con la prova verbatim presa da una conversazione reale
e con il piano esistente che dovrebbe possederla. Le cure restano soggette ai
MANTRA: ogni voce va chiusa **KB-first**, insegnabile parlando, sulla KB viva.
Il §7 aggiunge un'ipotesi di lavoro e un esperimento discriminante; non dichiara
risolti i limiti del §6.

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
- una **verifica di pertinenza semantica**: la risposta deve soddisfare la
  richiesta sul suo oggetto. La mancanza delle entità della domanda è un indizio,
  non il criterio: «391» risponde alla moltiplicazione senza ripeterne i numeri,
  mentre il repertorio sopra ripete il tema senza rispondere. Il riferimento
  quantitativo sui turni rubati resta `quanto-manca.md`.

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

1. **Contratto di risposta** (M6/M4): rendere interrogabili l'oggetto, le
   richieste e i vincoli del turno; verificare quali obblighi una risposta
   soddisfa e quali lascia aperti. La verifica è semantica, non una conta di
   parole condivise. Un residuo deve poter orientare una domanda o una ricerca,
   oltre al rifiuto onesto. Regole nella KB, misurate sul banco di
   `quanto-manca.md`; il §7 propone come farne anche un segnale d'apprendimento.
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

Questa mappa esprime ipotesi di difficoltà, non risultati d'impossibilità:
«serve un principio nuovo» significa che i meccanismi descritti finora non
hanno dimostrato di bastare. Il §7 propone un principio da mettere alla prova.

Gli ultimi cinque hanno la stessa radice: **servono giudizi graduati e
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

Per onestà simmetrica: il progetto persegue anche proprietà che renderebbero
parrot0 interessante nel confronto con la frontiera. Vanno preservate e
verificate mentre si affrontano i residui:

- **derivazioni ispezionabili**: la provenienza permette di contestare premesse
  e passaggi. Non garantisce la verità delle fonti, la correttezza della lettura
  o la pertinenza della risposta;
- **apprendimento senza riaddestramento** per le forme supportate, con
  persistenza quando la lezione viene salvata;
- **correzione localizzabile** attraverso le dipendenze: la propagazione
  completa resta un obbligo di R8, non una proprietà già garantita;
- **ignoranza e ricerca incompleta rappresentabili esplicitamente**, da
  mantenere distinte;
- **esecuzione locale**: costo e latenza sulla KB crescente restano da misurare,
  soprattutto per R4.

Il criterio di `CLAUDE.md` vale anche per i residui: qualunque via verso R2–R7
è nella direzione giusta solo se **aumenta ciò che parrot0 vede e la sua capacità
di decidere**, e sbagliata se introduce un componente opaco che decide al suo
posto.

## 7. Ipotesi: imparare le distinzioni che cambiano una decisione

> **F., 14 settembre 2026:** questo piano è molto critico; trovare
> un'intuizione che metta ordine e permetta di traguardare l'obiettivo generale.
>
> **Stato:** proposta architetturale, non capacità implementata o misurata.

### 7.1 L'oggetto che manca fra conoscere e scegliere

**La KB deve poter imparare perché, in una situazione, una possibilità è
preferibile a un'altra.** L'unità di crescita proposta è una *distinzione*:
una condizione che cambia la scelta, insieme ai casi che la sostengono e a
quelli che la smentiscono. Una parola, un fatto e una regola restano conoscenza;
lo diventa anche l'esperienza del loro impiego.

Esempio di lezione, in lingua naturale:

> «Quando ti chiedo se riesco a fare qualcosa, controlla prima ciò che potrebbe
> impedirlo. Se manca un dato che cambia la risposta, chiedimi quello.»

Il contenuto insegnato è il rapporto fra **scopo, impedimento e informazione
mancante**. Deve valere per una coincidenza, una consegna e un appuntamento,
quando la KB sa rappresentarne i vincoli. Non è una priorità assegnata alla
parola «treno» o alla facoltà che la riconosce.

Il punto del mantra #23 si ripresenta: lettura, memoria, ragionamento e
generazione devono consultare lo stesso oggetto. Qui è la **scelta situata**:

| parte interrogabile | che cosa contiene |
|---|---|
| situazione e scopo | frame, mossa aperta, versione delle credenze, obblighi da soddisfare |
| possibilità | una lettura, un passo, una domanda o una formulazione candidata |
| ragioni | supporti, obiezioni, condizioni d'applicazione, fonte e dipendenze |
| aspettativa | quale obbligo dovrebbe chiudersi, quale informazione dovrebbe arrivare |
| esito | che cosa è accaduto o è stato verificato, costo e residui |

È una descrizione del contratto, non un nuovo schema `.p0` da copiare. Le
identità devono riusare IR, contesti e piani esistenti. Non basta registrare il
vincitore: senza alternative e aspettativa non si può capire che cosa imparare.

### 7.2 Un ciclo comune, con giudizi di specie diversa

```text
turno + situazione + mossa aperta
                 ↓
possibilità parziali nella IR comune
                 ↓
ragioni a favore/contro + obblighi ancora aperti
                 ↓
scelta del prossimo passo → esito osservato
                 ↑                 ↓
           criterio KB ← distinzione candidata e verifica su altri casi
```

Il ciclo può servire i cinque residui senza fingere che siano la stessa misura:

| residuo | confronto da imparare |
|---|---|
| R2 plausibilità | quale ipotesi regge meglio alle prove e alle eccezioni disponibili |
| R3 salienza | quale informazione può cambiare una decisione relativa allo scopo |
| R4 ricerca | quale passo ha chiuso obblighi simili, con quale costo e quali fallimenti |
| R5 espressione | quale formulazione conserva il contenuto e soddisfa meglio destinatario, tono e intento |
| R7 lettura parziale | quali parti sono sostenute e quale domanda distingue le alternative residue |

**Non serve decretare un punteggio universale.** Verità, utilità, stile e costo
restano dimensioni distinte. Un testo elegante non compensa un fatto inventato;
una derivazione costosa non diventa falsa. I vincoli obbligatori vengono
verificati prima delle preferenze; incertezza, conflitto e incomparabilità sono
esiti leciti. Anche quali criteri applicare e come risolverne i conflitti devono
essere conoscenza correggibile.

R2 richiede un esperimento proprio sulla combinazione delle prove: provenienze
comuni non contano come conferme indipendenti, le eccezioni devono poter
sconfiggere una regola tipica, e un punteggio di ordinamento non è una
probabilità calibrata. Il contratto comune rende questi problemi esprimibili;
non ne fornisce per decreto la semantica.

### 7.3 Come una scelta diventa apprendimento, senza il maestro nascosto

Due ingressi allo stesso processo: una correzione parlata può proporre una
distinzione; un esito osservato può far cercare la condizione che mancava.
In entrambi i casi il criterio ha contesto, sostegno e possibilità di revisione.

1. Conservare il contesto della scelta e l'aspettativa prima dell'esito.
2. Confrontare aspettativa ed esito. «L'utente non protesta» non prova che la
   risposta fosse corretta; una preferenza di tono non certifica un fatto.
3. Proporre una condizione strutturale che distingua successi e fallimenti,
   usando relazioni e operatori insegnabili. Sostituire i nomi propri con ruoli
   non basta: occorrono casi in cui la condizione manca o si inverte.
4. Provare la distinzione su casi tenuti fuori dalla sua costruzione, contro
   il comportamento precedente. Conservare controesempi e ambito di validità.
5. Promuoverla come criterio rivedibile solo per ciò che il confronto sostiene;
   una correlazione osservata non diventa una legge universale del mondo.

La fonte del riscontro deve essere esplicita: verifica meccanica su premesse
date, esito di uno strumento, fatto successivo, confronto fra formulazioni
valutato dall'utente. Il testo successivo può smentire un'interpretazione, ma il
solo fatto che una frase generata si rilegga bene non la convalida. È il limite
già misurato in `autoaddestramento-dalla-prosa.md` §4.3.

Il ciclo deve imparare anche **a proporre** possibilità, altrimenti ordina un
repertorio chiuso. Costruzioni linguistiche, trasformazioni e operatori
candidati restano estensibili nella KB. Prima si misura il selettore su
possibilità disponibili; poi si verifica separatamente se l'apprendimento
amplia ciò che riesce a proporre. Questa seconda prova non può essere
sostituita dal miglioramento della prima.

### 7.4 Perché potrebbe ridurre il costo, e dove può fallire

La leva cercata è **riusare il risultato di una ricerca come conoscenza per
orientare ricerche successive**. Se ogni turno ricomincia da tutte le
possibilità, il ciclo aggiunge costo. Se un'esperienza produce un criterio
strutturale trasferibile, il turno seguente può trovare prima il passo utile.

Il criterio ordina l'esplorazione; non cancella dalla KB le alternative e non
le dichiara false. Una ricerca interrotta conserva una frontiera riprendibile
e dichiara il budget esaurito. Occorre misurare anche i rami utili ritardati dal
criterio, altrimenti una ricerca più veloce può nascondere una perdita di
capacità. Il costo del criterio stesso fa parte della misura.

Questo non elimina l'esplosione combinatoria nel caso generale. L'ipotesi
utile è più precisa: **sulla distribuzione di conversazioni affrontata, le
distinzioni apprese fanno trovare più spesso una buona mossa a parità di
budget**. Se non succede, R4 resta esattamente dov'era.

### 7.5 Gli appigli esistono, il collegamento va costruito

Ispezione del repository in questa revisione, senza nuova prova di esecuzione:

| appiglio | riuso possibile | ciò che non dimostra ancora |
|---|---|---|
| `kb/core/dialogue-frames.p0`: letture, evidenze, residui | identità delle alternative parziali | che tutta la conversazione le produca e le usi |
| `kb/core/situation.p0`, `context-scope.p0` | stato contestuale, precondizioni, revisione locale | propagazione completa delle revisioni |
| `kb/core/thinking.p0`: `step(Operatore, Pre, Effetto, meta(Costo, Supporto))` | aspettativa ed esecuzione del prossimo passo | apprendimento del criterio che sceglie il passo |
| `src/kb.c`: `kb_hypothesis_best`; `kb/core/input.p0`: pesi delle evidenze | confronto ispezionabile, pareggio esplicito | plausibilità graduata o pesi appresi dagli esiti |
| `autoaddestramento-dalla-prosa.md` | lacune, rimedi candidati, verifica e regressioni | correttezza indipendente del contenuto letto |

Il lavoro iniziale è chiudere una di queste connessioni in verticale. Non
riscrivere le facoltà, né introdurre un secondo lettore per far riuscire la
dimostrazione. Se la IR perde una premessa, quel gap va misurato e riparato lì.

### 7.6 Primo esperimento: imparare quale domanda serve davvero

**E1 — A parità di conoscenze del dominio e possibilità, una lezione su ciò che conta
migliora la prossima mossa anche in una situazione diversa?** Questo misura
insieme un primo pezzo di M1/M3/M6 e la connessione R3/R7. Non chiude R2, R4 o R5.

Il caso guida resta il viaggio della diagnosi. Con partenza alle 7, durata
tre ore e appuntamento alle 9 nella città di arrivo, parrot0 ha già un
impedimento sufficiente: l'arrivo alle 10. Chiedere il tragitto dalla stazione
non serve a risolvere quel confronto. Se l'appuntamento è alle 11 e quel
tragitto non è noto, invece, il dato può cambiare la risposta. «Il cliente è
di Milano» da solo non stabilisce che la riunione si tenga a Milano: anche
questo deve restare distinguibile.

**Banco fissato prima della cura:** conversazioni italiane e inglesi, con
orari diversi, due impegni concorrenti, distrattori, un dato già comunicato,
un dato mancante decisivo, un dato mancante irrilevante, correzioni e il caso
semplice già risolvibile. Trasferimento su consegne e coincidenze; almeno una
famiglia resta esclusa dalla costruzione del criterio. Le premesse arrivano
parlando nella KB completa del profilo `agi`.

Tre condizioni sullo stesso eseguibile e sulla stessa base completa:

- **prima** della lezione: registrare risposta, possibilità prodotte e costo;
- **dopo** la lezione di §7.1, senza predicati o API nel prompt: ripetere e
  affrontare i casi esclusi;
- **dopo ritrattazione mirata** della lezione e delle sue conseguenze apprese:
  verificare che il cambiamento dipendente da essa scompaia.

Fra le condizioni si azzera lo stato conversazionale necessario a ripetere il
banco, conservando la base. Le preferenze apprese non devono trapelare nella
condizione «prima». I casi con premesse fornite dimostrano crescita e
trasferimento della condotta; la capacità di collegare conoscenza del mondo
preesistente richiede inoltre un banco distinto che non ne inietti gli archi.

Misure: risposte utili corrette, domande che discriminano le alternative,
domande superflue, affermazioni senza sostegno, passi del solver e latenza.
Contare soltanto i rifiuti corretti premierebbe un sistema che non fa nulla.
L'esperimento passa se il vantaggio raggiunge la famiglia esclusa, scompare
con l'ablazione e non aumenta gli errori sul banco di regressione. Pubblicare
i conteggi e il costo, anche quando il campione è piccolo.

**Falsificazione:** cambia solo il viaggio insegnato; bisogna scrivere regole
separate per i nomi dei domini; la domanda utile non viene neppure proposta;
la spiegazione cita un criterio che l'ablazione lascia ininfluente; oppure la
selezione costa quanto la ricerca risparmiata. Sono fallimenti diversi: il
referto deve dire quale collegamento non ha retto.

Se E1 regge, il circuito si massimizza prima di aprirne un altro (mantra #22).
Il seguito discriminante è apprendere una distinzione dagli esiti senza la
lezione esplicita; poi riusarla per ordinare passi di ricerca e confrontare
formulazioni. Ogni estensione richiede il suo riscontro indipendente.

### 7.7 Che cosa mette in ordine, senza promettere la frontiera

M1/M3 costruiscono l'oggetto condiviso; M6 espone obblighi ed errori; il ciclo
proposto trasforma quell'esperienza in condotta appresa. R3/R7 offrono il primo
esperimento; R4 ne verifica il rendimento; R2 e R5 richiedono prove specifiche
sulla plausibilità e sulla qualità. R1 può proporre astrazioni che distinguono
casi prima indistinguibili, ma l'induzione di nuovi schemi resta da costruire.
R6 dipende dalla disponibilità di riscontri informativi; R8 deve mantenere
valide le dipendenze; R9 deve fornire osservazioni e azioni reali.

La scommessa diventa così verificabile: **ogni esperienza utile lascia una
distinzione ispezionabile che migliora una scelta futura, anche fuori dal caso
che l'ha prodotta**. Se questo trasferimento cresce e il suo costo resta
sostenibile sulla KB viva, abbiamo una via verso l'obiettivo generale. Se
produce soltanto criteri locali da insegnare uno per uno, abbiamo rinominato
il collo di bottiglia del maestro e l'ipotesi va respinta.

## 8. Handoff operativo — E1 avviato, da completare (14 settembre 2026)

**F. ha autorizzato l'implementazione («mettilo in pratica»), poi ha chiesto
questo handoff per fine contesto. Ci si ferma qui su sua richiesta.** Il
circuito ha dato un primo risultato comportamentale; **E1 non è chiuso**, il
banco completo non è stato certificato e non è stato fatto alcun commit.

### 8.1 Stato del workspace

| file | modifica |
|---|---|
| questo documento | §7 della sessione precedente, correzioni alla diagnosi e questo handoff |
| `kb/core/decisions.p0` (nuovo) | formule additive, quantità e limiti inferiori, requisiti, candidati motivati, preferenze condizionali, selezione e resa |
| `kb/core/decision-language.p0` (nuovo) | insegnamento e ritrattazione con il `turn_form` comune; domanda di fattibilità e descrizioni numeriche |
| `kb/core/procedures.p0` | include di `decisions.p0` dopo `situation.p0` |
| `src/brain/10-memory-knowledge.c` | due punti del matcher comune: togliere il punto interrogativo/finale solo in coda, conservando il punto interno di un decimale |
| `tests/p0t/reasoning/taught_decision.p0t` (nuovo) | banco scritto prima dell'implementazione, con premesse e preferenza insegnate in lingua naturale |

**Nessun motore C di dominio aggiunto.** Il piccolo intervento C corregge la
meccanica dei due slot (`span` e `slot`/`rest`) che trasformava `2.5` in `2`.
Va ancora verificato con casi decimali e con i consumatori preesistenti.

### 8.2 L'esperimento che ha effettivamente funzionato

Processo indipendente `bin/parrot0 --mcp-engine`, profilo completo `agi`,
`PARROT0_SESSION=`; chiamate `gen.respond` tramite la classe `Engine` di
`scripts/self-questions.py`. Niente fatti inseriti via `kb.assert`.

```text
calculate arrival by adding departure and duration
calculate ready by adding arrival and transfer
transfer is never below 0
the departure of train is 7
the duration of train is 3
for train the ready must be at most 9

can I complete train?
→ For train: I need to choose between explaining an obstacle and asking for missing information.

prefer explaining obstacles to asking questions when a requirement fails
→ I have learned that conditional preference.

can I complete train?
→ For train: No: ready reaches 10; the requirement is at most 9.

forget the requirement for train on ready
for train the ready must be at most 11
can I complete train?
→ For train: What is the transfer of train?

the transfer of train is 0
can I complete train?
→ For train: The stated requirements are satisfied.
```

Stesso processo e stessa preferenza: `load = cargo + packaging`, minimo di
`packaging` insegnato pari a zero, `cargo` del pacco pari a 12, requisito sul
carico massimo 10 → ostacolo spiegato. Ritrattando **parlando** la preferenza:

```text
forget that preference for explaining obstacles over asking questions when a requirement fails
can I complete parcel?
→ For parcel: I need to choose between explaining an obstacle and asking for missing information.
```

Reinsegnandola, torna la spiegazione `12` contro `10`. Osservati anche un
requisito `at least` (carica 3 contro minimo 5) e un caso già completo che
conferma i requisiti senza chiedere un dato irrilevante. I turni di verifica
di questa corsa costavano circa **1,6–2,1 s**, non meno di un secondo; nessuna
conclusione sulla scalabilità.

Questi sono **riscontri manuali sulle risposte**, non il risultato del runner
`.p0t`. Il driver temporaneo leggeva le righe `>` del banco e stampava risposta
e tempo; non controllava le aspettative `<`. Il banco non va dichiarato verde.

### 8.3 Architettura presente e due difetti già diagnosticati

- I dati numerici entrano con `state_commit`, quindi in
  `holds_in(described_situation, st(...))`; `role/3` li vede attraverso
  `situation_state/4`. Non c'è una seconda tabella di valori.
- `decision_formula/3`, `decision_requirement/4`, `decision_minimum/2` e
  `decision_preference/3` vengono insegnati dalle forme comuni. La scelta è
  una vista: `decision_candidate/3`, `decision_dominated/2`,
  `decision_selected/2`. I candidati scartati restano interrogabili.
- Il minimo di una quantità sconosciuta **non è implicitamente zero**: lo
  deve dichiarare una lezione. Le somme propagano quel limite e il residuo
  segue le dipendenze fino al dato mancante.
- `ability_polar` prendeva prima ogni «can I complete …?» e rispondeva
  «I don't know whether complete can train». La cessione usa l'esistente
  `turn_form_yield` con le ancore di `decision_question_en`.
  **Trappola:** quel consumer consulta una relazione di arità **2**; la prima
  prova con `decision_question_surface/1` non poteva funzionare. Ora è `/2`.
- `op(match, …)` necessita di un posto libero per produrre una risposta.
  La prima `decision_store/3` eseguiva gli effetti ma non rendeva nulla e
  lasciava continuare il vecchio lettore. Ora `decision_store/4` restituisce
  il valore nel quarto argomento; il turno risponde «Noted: …».

**Ultima modifica prima dell'handoff:** rimossa la regola
`role_name($R) :- situation_state(described_situation, $O, $R, $V).`
La corsa completa si fermava per timeout di 30 s alla lezione
`is x feasible means can I complete x`. Dopo questa rimozione, su un caso
ridotto con una formula e un valore, la stessa lezione è riuscita in **1,482 s**
e `is train feasible?` è stata riletta correttamente in **2,089 s**. Questo
localizza un sospetto forte, **non prova ancora** che il timeout sulla storia
completa sia risolto. Non ripristinare la regola senza profilare: enumerare i
ruoli dalla situazione viva rischia di far rileggere lo stato al lessico.

### 8.4 Il lavoro ancora necessario, nell'ordine

1. **Ripetere il banco completo sullo stato attuale**, prima di estendere.
   Verificare la lezione di parafrasi dopo tutta la storia, poi la ritrattazione.
   Il file `.p0t` e le sonde devono usare sempre la KB completa. Aggiungere
   italiano e parafrasi dei dati/requisiti; non basta la domanda italiana
   `posso completare …?` già dichiarata.
2. **Correggere la resa del limite inferiore.** Oggi `ready reaches 10` viene
   detto anche quando 10 è soltanto il minimo e il trasferimento è ignoto.
   La ragione deve distinguere valore esatto e limite inferiore, e dire
   esplicitamente «almeno 10». La conclusione negativa è sostenuta; quella
   formulazione della premessa è troppo ambigua.
3. **Verificare la custodia dei valori.** La forma numerica è generale e
   anticipata: controllare che una domanda non insegni uno stato, che
   decimali e correzioni funzionino, e che domande ordinarie sui valori
   continuino a raggiungerli. I valori sono nel contesto mentre alcuni
   consumer chiedono ancora `Ruolo_of/2`: possibile cassetto senza maniglia.
   Verificare anche contraddizioni, due formule dello stesso ruolo e cicli.
4. **Distinguere pareggio e ricerca non conclusa.** Ora il ramo senza scelta
   usa sempre una frase che nomina spiegazione e domanda: con nessun
   candidato, o con ricerca incompleta, potrebbe raccontare alternative
   che non sono state prodotte. Correggere la distinzione nella KB.
5. **Chiudere il collegamento alla conversazione e alla traccia.** Le
   alternative sono viste interrogabili, ma non esistono ancora episodi
   persistenti con contesto, aspettativa, criterio usato ed esito. La domanda
   sul trasferimento non apre ancora una questione sul tabellone comune:
   `it is 0` non è stato implementato; il caso osservato ripeteva l'entità e
   il ruolo e poi la domanda. Riusare `open_issue`, `issue_topic`,
   `issue_relation`, `issue_turn` e `max_qud`, senza inventare un `pending_*`
   separato. Riusare anche le identità `frame_*`/`reading_*` per il tracciato.
6. **Massimizzare la stessa distinzione**: dato già noto, dato irrilevante,
   più impegni, distrattori, preferenze confliggenti, ablazione del minimo
   e della formula oltre che della preferenza, famiglia esclusa dalla cura.
   Un apprendimento automatico dagli esiti non è stato implementato.
7. Registrare conteggi semantici e latenza; eseguire controlli puntuali sul
   matcher modificato e il controllo rapido appropriato. Non lanciare la
   suite completa lunga senza seguire la politica del repository.

### 8.5 Artefatti diagnostici e ripartenza

Al momento dell'handoff esistono in `/tmp` (temporanei, non versionati):

- `p0-decision-probe.py`: driver MCP, importa `Engine` da `self-questions.py`,
  avvia un processo fresco e legge le righe `>` del file passato come argomento;
- `p0-decision-results2.txt`: corsa con i risultati riportati in §8.2,
  interrotta prima della parafrasi;
- `p0-decision-trace.log`: **sovrascritto dall'ultima sonda ridotta**, non è più
  il log della corsa completa;
- `p0-decision-small.p0t`: i quattro turni della sonda ridotta riuscita.

Se sono ancora disponibili, la prima ripetizione è:

```sh
python3 /tmp/p0-decision-probe.py tests/p0t/reasoning/taught_decision.p0t
```

Quel driver ha timeout di 30 s e non è un benchmark definitivo: va dotato di
chiusura del processo in `finally`, conservazione dei log per corsa e verifica
delle aspettative prima di usarlo per certificare E1. Se `/tmp` è perso, la
classe `Engine` in `scripts/self-questions.py` e il banco versionabile
contengono quanto serve per ricostruire la sonda.

Verifiche già eseguite: **`make build` riuscito** con il piccolo cambiamento C;
un controllo di boot senza `PARSE ERROR` in una revisione intermedia;
**`git diff --check` pulito** allo stato dell'handoff. Nessuna suite completa,
nessun risultato del runner `.p0t`, nessuna promessa di E1 concluso.

**Confine dell'avanzamento:** il prompt originale «domani ho una riunione…
parto da Roma… arrivo in tempo?» e la lunga lezione iniziale del §7.1 hanno
murato nella baseline. Non sono stati chiusi. La prova attuale dimostra la
prima scelta insegnabile su formule, valori e requisiti detti attraverso le
forme indicate sopra; non certifica ancora comprensione libera della
conversazione originale, conoscenza del mondo o autonomia d'apprendimento.
