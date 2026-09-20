# The Rational Philosopher — conversare nello spazio logico dell'interlocutore

## HANDOFF PRIORITARIO — dal dialogo interrogativo all'iniziativa motivata (20 settembre 2026)

**Leggere questa sezione prima del piano iniziale più sotto.** La richiesta
attuale di F. è più ampia: trasformare il mondo allargato in un framework
operativo per i contesti umani, comprese aperture senza domanda, desideri,
incertezze, vissuti, creazione, collaborazione e continuazioni. Quando manca
una direzione, parrot0 deve poter proporre un seguito pertinente di propria
iniziativa, lasciando correggibile la propria interpretazione.

F. chiede ora un passaggio di consegne utilizzabile anche da un coding agent
meno potente. **Non ripartire dall'implementazione di F1 alla cieca:** prima
riprodurre il circuito e i difetti riportati qui. La nuova consegna è un
**prototipo KB-only funzionante su un sottoinsieme, non il completamento del
piano né comprensione universale certificata**. Le modifiche sono nel working
tree; nessun commit o push è stato eseguito per questo lavoro.

### A. Che cosa è stato costruito e dove

| file | responsabilità |
|---|---|
| `kb/core/dialogue-initiative.p0` | lettura dei token della IR condivisa; candidati; confronto delle letture; atti citati; contesti di lavoro; proposta, continuazione, aggiornamento, rifiuto e spiegazione |
| `kb/core/dialogue-initiative/forms.p0` | **279 aperture, 21 atti**; forma intera o con complemento; nomi pronunciabili degli atti; interpretazione dei seguiti |
| `kb/core/dialogue-initiative/moves.p0` | **10 itinerari, 40 mosse** con bisogno dichiarato e resa EN/IT; il metodo è proposto, non spacciato per un risultato ottenuto |
| `kb/core/dialogue-initiative/teaching.p0` | 16 forme di lezione/ritiro: apertura → atto; nome della mossa → bisogno; mossa → parole; attività → prima mossa |
| `kb/core/procedures.p0` | include del circuito dopo `discourse.p0` |
| `tests/probes/rational_philosopher.it.p0t` | transcript diagnostico: aperture, continuità, vincoli, motivazione, arresto, controlli e caso storico F1 |
| `tests/probes/rational_philosopher.en.p0t` | transcript diagnostico: crescita per lezione, trasferimento, ritiro, policy e controesempi |
| `MANTRA.md`, #26 | una soluzione acquista valore se amplia ciò che si può insegnare; replay e ritrattazione restano obbligatori |
| `LEARN_PROTOCOL.md`, aggiornamento sul dialogo | forme nuove con grado di verifica e limiti, non promessa indiscriminata di apprendimento |

Sono dati di metodo e un nuovo consumatore KB delle strutture comuni: **zero
modifiche a `src/`**. La quantità di aperture misura il vocabolario, non il
grado di comprensione. Non promuovere queste cifre a punteggio cognitivo.

### B. Misure e limiti: che cosa sappiamo davvero

Baseline fresca, profilo `agi`, sessione vuota ma **KB completa**:
«vorrei imparare a dipingere», «non so da dove cominciare», «mi interessa il
tempo» ricevevano muri. Dopo il primo circuito:

| prova | risultato osservato |
|---|---|
| `vorrei imparare a dipingere` | proposta di partire da un esempio e dal livello già posseduto; tema conservato come `a dipingere` |
| `continua` | passa all'esercizio minimo sullo stesso tema |
| `no grazie` → `continua` | interrompe la proposta; il seguito non la riattiva di nascosto e chiede un'ancora |
| `i want to learn to paint` | proposta di un esempio concreto, dopo la correzione del confronto delle letture descritta sotto |
| insegnare una mossa e usarla come prima mossa dell'apprendimento | il turno successivo usa la nuova resa; `why` nomina il bisogno insegnato; ritirando la policy torna la proposta di base |
| insegnare `let us study` come apertura di apprendimento | il replay `let us study pottery` entra in `turn_plan`; dopo il ritiro questa lettura sparisce |
| dopo il ritiro di `let us study` | il percorso legacy può dire `Learned: let study pottery.`: la perdita della forma è provata, **l'innocuità del fallback no** |
| insegnare `i am keen to` con lo stesso atto | lettura, `dialogue_seen`, candidatura e `turn_priority_response` sono corrette; quest'ultima restituisce `On draw: Let us use one concrete example…`, ma il turno reale è risposto da `smalltalk`: **difetto fra decisione e arbitrato**, non una forma ancora da riconoscere |
| insegnare una resa con `and` | `Show one example and explain what is unclear` viene resa come `show one example`: **perdita della seconda parte da diagnosticare** |
| `make soft-test` dopo le aggiunte | **verde in 11 s**, budget 15 s; suite completa non eseguita |

Il ciclo italiano è stato osservato prima della modifica che separa `update`
da `advance`: il difetto scoperto era che «ho solo dieci minuti» avanzava
come se il lavoro fosse già stato svolto. La separazione e la memoria del
dettaglio ora esistono nel codice KB; **il replay finale dell'intero circuito
è una verifica da completare**, non una misura implicita nella modifica.
Le sonde versionate raccolgono anche i casi ancora rossi. Non sono golden e
il driver `p0t-echo.py` non verifica automaticamente attese.

### C. I comandi di ripartenza, in quest'ordine

```sh
git status --short
git diff -- MANTRA.md LEARN_PROTOCOL.md kb/core/procedures.p0
LANGX=it python3 scripts/p0t-echo.py tests/probes/rational_philosopher.it.p0t
python3 scripts/p0t-echo.py tests/probes/rational_philosopher.en.p0t
rg 'PARSE ERROR' logs/p0t-echo/trace-rational_philosopher*
```

I file nuovi non compaiono nel normale `git diff`: aprirli direttamente.
Il driver avvia un motore fresco con l'intero profilo e conserva transcript e
stderr in `logs/p0t-echo/`. Leggere le risposte intere. Eseguire una sonda alla
volta: non misurare latenza mentre un altro banco lavora. **Non rilanciare
subito la suite:** `soft-test` è già verde; ripeterlo dopo cambiamenti che lo
giustificano. Non alzare budget e non togliere conoscenza.

### D. Ordine di lavoro obbligatorio: un problema strutturale alla volta

1. **Rendere affidabile il giro insegnamento → lettura → scelta → resa.**
   Partire dalle due lesioni concrete: `i am keen to` e la resa tagliata su
   `and`. Confrontare il valore appena insegnato, `dialogue_raw`,
   `dialogue_seen`, `dialogue_candidate`, `turn_priority_response` e
   `turn_module`. Annotare il primo confine dove il dato o la decisione cambia.
   Non aggiungere un'eccezione per `keen` o una frase senza congiunzioni.
2. **Trasformare le sonde in regressioni con attese semantiche.** Per ogni
   nuova forma: prima, lezione naturale, stesso turno, altro argomento,
   ritiro, stesso turno. Per una resa: conservare entrambe le parti. Per una
   policy: mostrare la mossa selezionata e il bisogno, non solo l'acknowledgement.
   Tenere separato il difetto legacy dopo ablazione dall'effetto della lezione.
3. **Correggere la semantica degli aggiornamenti.** Un vincolo restringe una
   proposta, un criterio cambia il confronto, un'osservazione porta evidenza,
   una correzione può cambiare il referente: non sono quattro modi di dire
   «avanti». Oggi `dialogue_detail` conserva il dato e `dialogue_update_move`
   sceglie una mossa distinta, ma il vincolo non governa ancora tutti i passi
   successivi e `correction` non ricostruisce il tema. Chiudere questo prima
   di aggiungere altre aperture.
4. **Collegare la memoria al tabellone unico.** `dialogue_scope` e
   `dialogue_emitted` sono un primo registro del circuito; non inventare un
   secondo `pending_*`. Farne contenuti/atti riferiti da `open_issue`,
   `issue_turn`, `issue_topic` e viste di stato. Aprire la questione per una
   proposta effettivamente emessa, non soltanto candidata. Oggi il contabile
   registra la candidatura prima della resa: una resa fallita può lasciare
   una falsa memoria di proposta. **Misurato proprio su `i am keen to draw`:**
   `dialogue_emitted(2, …)` esiste pur avendo parlato `smalltalk`. Non è un
   rischio solo teorico: riparare la registrazione dell'emissione insieme al
   confine di arbitrato. Questo è debito esplicito.
5. **Sostituire l'avanzamento numerico con progresso verificato.** I quattro
   passi per attività sono bootstrap di procedure insegnabili. `continua`
   autorizza a spiegare il passo successivo; **non prova** che il precedente
   sia stato eseguito. Un esito deve venire da un atto o da un'osservazione.
   Collegare `dialogue_move_need` a prerequisiti, dati disponibili e residui:
   prima riusare `frame_residue`, `dialogue-policy`, `situation` e `inquiry`.
6. **Solo sul circuito affidabile, espandere la composizione.** Due intenzioni
   nello stesso turno, interruzione e ripresa, contesti annidati, esempi citati,
   dissenso e correzione della policy. Ogni espansione deve aprire subito una
   lezione pronunciabile e aggiornare il catalogo del protocollo.

**Gate di consegna:** un turno non interrogativo produce un passo motivato;
il seguito conserva l'ancora; un dato nuovo cambia il passo pertinente; un
rifiuto lo ferma; la motivazione rimanda all'atto e al bisogno effettivi;
la lezione cambia condotta e il ritiro ne toglie l'effetto. Dichiarare i casi
non letti e quelli non eseguibili. Non dire «qualunque prompt è supportato».

### E. Insight verificati e leve per il prossimo passaggio

**1. La difficoltà non era solo leggere una domanda: mancava l'oggetto
«proposta che attende un seguito».** Una risposta e una proposta producono
obblighi conversazionali diversi. Il passo proposto deve avere identità,
ancora, scopo, stato e modalità di ripresa. Altrimenti «sì» e «continua» sono
token isolati e l'iniziativa diventa un nuovo frasario.

**2. Separare la lettura dal confronto delle letture ha sbloccato l'inglese.**
La prima versione faceva `naf(dialogue_ambiguous(...))` sopra regole che
rileggevano ricorsivamente tutti i prefissi e i payload. `dialogue_best`
trovava la lettura, ma `dialogue_reading` falliva: la negazione non riusciva a
concludere entro i limiti del solver. Dopo la pubblicazione di
`dialogue_raw(N, Act, payload(Text, End))`, il confronto delle alternative è
finito e poco profondo; `i want to learn to paint` risponde. **Lezione generale:
non negare una nuova esecuzione di un parser per dire che una sua alternativa
manca. Pubblicare le evidenze una volta, poi ragionare sulle evidenze.**

**3. Il ponte più fertile è «nome pronunciabile → ruolo → procedura».**
La lezione non deve nominare `dialogue_move_need`. Dice «the conversational
move careful beginning addresses a concrete example». Una seconda lezione
ne insegna le parole, una terza quando usarla. Questo rende correggibile la
condotta invece di accumulare risposte. Un nuovo predicato senza una forma
di insegnamento è un cassetto senza maniglia; mantra #26.

**4. Una proposta razionale non è necessariamente una conclusione dedotta.**
La prova di un fatto e la giustificazione di un'azione sono oggetti diversi.
«Propongo di confrontare due esempi perché manca il criterio» richiede una
ragione operativa, non la prova che una delle opzioni sia vera. La distinzione
permette iniziativa onesta senza fingere di avere opinioni o dati mancanti.
Oggi `why` espone il bisogno della mossa: non è ancora un certificato della
correttezza di tutte le premesse o dell'utilità futura della proposta.

**5. Il tema non è la verità del tema.** Il complemento di «supponiamo che…»
o «mi interessa…» viene conservato come contenuto attribuito. Non chiamare
`assert(P)` sul contenuto per poterne discutere. La stessa distinzione deve
reggere desideri, citazioni, vissuti, piani, finzioni e negazioni.

**6. Massimizzare gli ingressi prima di stabilizzare gli effetti è pericoloso.**
279 aperture moltiplicano l'uso delle stesse poche procedure, ma moltiplicano
anche un errore nella gestione di un vincolo o di un rifiuto. Ora investire
nelle distinzioni e nella continuità, non in un'altra lista di sinonimi.

### F. Domande da porsi quando ci si blocca

| sintomo | prima domanda | esperimento discriminante |
|---|---|---|
| dice «imparato» ma nulla cambia | la lezione è entrata? chi legge quel predicato? | leggere il fatto; replay identico; osservare lettura, candidatura e vincitore |
| la vista positiva funziona, la sua negazione no | è assenza o ricerca incompleta? | interrogare separatamente gli ausiliari; pubblicare i candidati prima del confronto; non aumentare il budget |
| una frase più lunga rompe il caso breve | quale informazione si perde, e a quale confine? | confrontare payload, clausole della lezione, resa registrata e testo emesso |
| il problema si sposta a un'altra facoltà | la comprensione è condivisa o locale? | leggere `turn_module`; classificare il concorrente con mantra #21; migrare la specie, non mettere una cue di cessione per il caso |
| la replica parla ancora del vecchio tema | la correzione cambia contenuto, atto o contesto? | due temi reali; correzione esplicita; `continua`; verificare quale identità è rimasta attiva |
| un «sì» scatena qualcosa di inatteso | a quale proposta e a quale forza si è legato? | interporre un turno estraneo, un rifiuto o una proposta concorrente; mai trattare l'assenso come autorizzazione globale |
| aggiungere conoscenza peggiora le risposte | viene scelta una lettura o solo la prima? | inserire una lettura concorrente e invertire l'ordine; il conflitto deve restare interrogabile |
| sembra tutto corretto ma è generico | quale dato dell'utente cambia davvero la decisione? | mantenere le parole quasi uguali e cambiare vincolo, criterio o commitment; la mossa deve cambiare per quella ragione |

### G. Ipotesi da sperimentare, non capacità già consegnate

- **Una mossa come procedura con prerequisiti ed effetti.** Rappresentare
  `mossa → bisogno → dato richiesto → effetto atteso`, usando procedure e
  inferenza comuni. L'assenza di un ruolo produce il residuo; il residuo
  propone una domanda o una prova discriminante. Confrontare con la sequenza
  numerica attuale su input dove il dato è già presente: deve evitare domande
  ridondanti. Non aggiungere un planner C per la conversazione.
- **Un atto pragmatico può avere più letture senza decidere subito.** «Sono
  stanco» può informare, cercare ascolto o chiedere aiuto. Conservare candidate
  con evidenza e commitment; preferire un passo reversibile che non richieda
  di sceglierne una arbitrariamente. Prova: ritiro/correzione dell'intenzione
  nel turno dopo, senza perdita del vissuto già detto.
- **Il progresso è un cambiamento dello stato epistemico o pratico.**
  Distinguere dato acquisito, opzione eliminata, ipotesi riformulata, azione
  tentata ed esito osservato. Un contatore di turni non misura nessuna di
  queste cose. Prova: ripetere «continua» non deve certificare lavoro eseguito.
- **Contesti come scopi e assunti, non etichette di dominio.** Apprendere,
  negoziare e creare possono coesistere sullo stesso contenuto. Le dieci
  attività bootstrap non sono dieci menti. Prova: un racconto usato per
  esplorare una tesi etica, con la finzione mantenuta distinta dagli assunti
  personali e dai fatti del mondo.
- **La ragione della proposta deve sopravvivere alla resa.** Un oggetto di
  mossa scelto dalla KB dovrebbe essere lo stesso che il contabile registra
  come emesso. Cercare prima un osservabile comune già presente in
  `turn_done`; un eventuale gancio C deve pubblicare l'esito, senza sapere
  quali attività, parole o mosse esistano.

## Il framework da costruire: ogni contesto umano, senza fingere onniscienza

La generalità richiesta è del **contratto**, non di una lista esaustiva di
argomenti. Il soggetto resta una KB unica. Per ogni turno distinguere:

```text
espressione letta → letture candidate → atto attribuito → contesto e commitment
                  → obiettivo / bisogno / vincoli / evidenze
                  → mosse ammissibili → proposta motivata → uptake → revisione
```

Questi collegamenti non cancellano gli oggetti a sinistra. La lingua originale,
la fonte, le alternative scartate e gli assunti restano consultabili. Riutilizzare
IR, `context-scope`, contenuti e derivazioni del mondo allargato. L'introspezione
di una clausola già asserita non è, da sola, la capacità di introdurre una
citazione senza crederla; `kb_act` per livello non equivale a un'identità per
ogni fonte e ogni atto conversazionale. Verificare questi ponti prima di
prometterli, anche se una API porta il nome giusto.

**Famiglie da coprire con la stessa struttura:** domanda chiusa e aperta;
affermazione; desiderio; progetto incompleto; richiesta indiretta; dubbio;
vissuto; contrasto di valori; disaccordo; vincolo; preferenza; osservazione;
esempio; citazione; ipotesi; controesempio; correzione; rifiuto; ripresa;
creazione e valutazione di un artefatto; resoconto di un'azione; richiesta di
iniziativa. Un turno può contenere più famiglie. Un nuovo membro deve costare
conoscenza e una lezione, non un recognizer C.

**Confini della prima versione:** la lettura è ancorata all'inizio del turno;
citazioni e più frasi vengono escluse, non comprese; l'uptake richiede la
proposta del turno immediatamente precedente; mancano ripresa dopo digressione,
vincoli compositivi, revisione piena del tema e collegamento operativo al
tabellone. I bisogni sono dichiarati dalle procedure: non sono ancora ricavati
da una ricerca generale delle premesse mancanti. Questi limiti costituiscono
il banco successivo, non scelte da congelare come architettura definitiva.

**Misura su quattro assi, senza media che nasconda un danno:** pertinenza
dell'ancora; fedeltà a contenuto e commitment; utilità e rivedibilità della
mossa; continuità dopo un seguito. Aggiungere costo e crescita per lezione.
Una mossa pertinente ma infondata non è razionale; una domanda di ritorno
sempre uguale non è iniziativa; una risposta corretta senza un seguito
interpretabile non chiude questa capacità.

---

## Piano iniziale e baseline storica — da leggere con le rettifiche sopra

> **Richiesta di F., 20 settembre 2026.** «Sempre usando la KB, la IR e la
> concezione del mondo allargato espressa in
> [`lettura-della-prosa.md`](lettura-della-prosa.md), costruiamo un piano
> dedicato all'abilità di gestire conversazioni che spaziano nello **spazio
> logico dell'interlocutore** ma che rimangono **razionali**. L'esempio è
> troppo piccolo: concentrarsi sul **carattere della domanda intermedia** fatta
> al secondo turno, e sul concetto di spazio logico. Devono essere domande
> aperte, anche generiche, anche di carattere filosofico — tipo *cosa pensi
> della vita*.»

---

## 1. L'esempio, esteso e misurato

Riprodotto il 20 settembre 2026, profilo `agi`, `PARROT0_LANG=it`. I primi tre
turni sono quelli di F.; gli ultimi due li ho aggiunti per allargare il campione
al registro filosofico che il piano deve reggere.

| # | turno | risposta di oggi | specie del guasto |
|---|---|---|---|
| 1 | «sai parlare italiano» | «Sì, parlo italiano.» | ✅ nessuno |
| 2 | «quali parole conosci che possono sorprendermi» | «Non capisco ancora.» | **muro su una domanda che andava aperta** |
| 3 | «in quale lingua ti ho chiesto quale lingua sai parlare» | «c, python.» | **ancora persa + senso sbagliato, detto con sicurezza** |
| 4 | «cosa pensi della vita?» | «Non so ancora tradurre «pensi».» | **apertura filosofica letta come lacuna lessicale** |
| 5 | «secondo te il tempo esiste davvero?» | «Non ne ho di miei — sono parrot0, un'IA — ma raccontami dei tuoi.» | **posizione nello spazio logico letta come possesso di opinioni** |

**Non sono cinque bug: sono cinque modi di uscire dallo spazio.** Il turno 3 è
il peggiore di tutti sulla scala di F., perché non è un muro ma una risposta
confidente e sbagliata: «lingua» è stata collassata su *linguaggio di
programmazione* e il referente — *un turno di questa conversazione* — è sparito.
L'ispettore lo conferma: `turn_illocution` legge `question`, e
`debug_frame_record`, `turn_focus`, `max_qud` e `topic_read` sono tutti
**niente**. La domanda è riconosciuta come domanda e non è letta come nulla.

---

## 2. Che cos'è lo spazio logico, operativamente

Non è una metafora, ed è già quasi tutto rappresentabile con il mondo allargato
di [`lettura-della-prosa.md`](lettura-della-prosa.md) §0 — **contenuto, atto,
contesto, giudizio, derivazione**.

> **Lo spazio logico dell'interlocutore è l'insieme dei contenuti che possono
> essere coerentemente intrattenuti, dato ciò che lo scambio ha stabilito:**
> ciò che l'altro ha asserito, ciò che ha supposto, ciò che ha presupposto, e
> ciò che ne segue.

Una mossa sta **nello spazio** se il suo contenuto è collegato ad almeno un
**atto** dello scambio. È **razionale** se il suo **giudizio** porta con sé la
**derivazione** e gli assunti su cui poggia. Le due proprietà sono indipendenti,
e i quattro quadranti sono esattamente i modi di sbagliare:

| | nello spazio | fuori dallo spazio |
|---|---|---|
| **razionale** | ✅ la mossa da costruire | il muro corretto ma irrilevante — turno 2 |
| **non razionale** | fluente e insostenibile: il rischio LLM | «c, python» — turno 3, il caso peggiore |

**Perché questo piano non è «fare filosofia».** Una risposta bella su *cosa sia
la vita* non vale niente se non si sa da che cosa viene: sarebbe il frasario
della prosa, misurato al 19% in `lettura-della-prosa.md`, trapiantato nel
registro filosofico. Il criterio resta quello di F.: **da che cosa viene la
risposta**, non quanto suona bene.

---

## 3. La domanda intermedia, e perché è l'unità dell'abilità

F. indica il secondo turno. Ha ragione, e vale la pena dire esattamente perché.

> «quali parole conosci che possono sorprendermi»

Questa domanda ha tre proprietà, e **tutte e tre** la distinguono da una
domanda di conoscenza:

1. **È aperta:** non esiste *la* risposta giusta, esiste una risposta
   giustificabile.
2. **È indicizzata all'altro:** «sorprendente» non è una proprietà delle
   parole, è una relazione fra una parola e **chi ascolta**. La risposta
   dipende da un modello dell'interlocutore che nello scambio **non è stato
   stabilito**.
3. **È intermedia:** non chiude un argomento, lo **apre**. Il suo valore non è
   nella risposta ma nel movimento che provoca.

Ne segue il comportamento corretto, ed è **né il muro né l'elenco**:

> **Quando la risposta dipende da qualcosa che lo scambio non ha stabilito, la
> mossa razionale è nominare quella dipendenza** — e poi, a scelta dichiarata,
> chiedere («che cosa sai già?») oppure assumere esplicitamente («assumo che
> tu non conosca il lessico tecnico: allora…»).

Nominare la dipendenza **è** una mossa nello spazio logico: restringe lo spazio
invece di saltarne fuori. Un muro non lo restringe; un elenco confidente lo
abbandona. Questa è l'abilità che il piano deve costruire, e il resto sono
conseguenze.

---

## 4. Che cosa esiste già, e non va rifatto

Il mondo allargato è stato costruito il 20 settembre 2026 per la prosa. È la
stessa astrazione che serve qui, e **è eseguibile**:

| porta | che cosa dà a questo piano |
|---|---|
| `kb_clause/4`, `kb_clause_arg/4` | un contenuto è un dato: si può **menzionare senza crederlo** — la condizione minima per intrattenere una posizione |
| `kb_act/3` + `act_layer/2` | **chi** ha fatto entrare un contenuto e a quale titolo: detto, supposto, derivato. È l'ancora di ogni mossa |
| `kb_derivation/4` | la prova con le sue dipendenze (AND), le alternative (OR), e `absent(G)` — «lo dico perché non trovo il contrario» è una mossa filosofica legittima **se dichiarata** |
| `supported_from_premises/1` | ciò che segue **dalle sue premesse** e non dal mondo: la differenza fra discutere una tesi e ripetere un fatto |
| `holds_in/2`, `context-scope.p0` | posizioni che convivono senza cancellarsi |
| `epistemic-status.p0` | i cinque stati: positivo, negativo, entrambi, nessuno, ricerca incompleta |
| `turn_illocution`, `faculty_force/2`, `faculty_yield_when/3` | la condotta è conoscenza: chi ha diritto di parlare, su quale forza, e quando cede a una lettura |
| `discourse.p0` (`previous_turn`, `turn_counter`, `turn_retained`) | il filo della conversazione, già parzialmente tenuto |
| `scripts/prose-why.sh`, `/debug` | l'ispettore **nello stato in cui sbaglia** — vedi la regola: una sonda che mostra una differenza trova i difetti da sola |

**Quello che manca non è l'astrazione: è che la conversazione stessa non è
ancora un oggetto di quell'astrazione.** Un turno dell'interlocutore non è un
contenuto con un atto. Per questo il turno 3 non trova la sua ancora.

---

## 5. Le facoltà da costruire, in ordine

Una per volta, ciascuna con la sua condizione di riuscita **misurabile**.

### F1 — Il turno è un contenuto, e il dire è un atto

Ogni turno dello scambio diventa un contenuto con la sua identità e il suo
atto: *chi* l'ha detto, *quando*, *in quale lingua*, *con quale forza*. La
lingua del turno è già osservata (`turn_language_observed/2`); manca che il
turno sia **nominabile** come oggetto e che un riferimento anaforico lo
raggiunga («ti ho chiesto…», «quando hai detto…», «la domanda di prima»).

**Riuscita:** il turno 3 dell'esempio risponde **«in italiano»**. E la prova
che non è un caso: «in che lingua ti ho risposto?» e «che cosa ti ho chiesto
per primo?» rispondono dallo stesso circuito, senza una riga per ciascuna.

**Non barare:** nessuna cue «in quale lingua». La domanda deve leggersi come
riferimento a un atto dello scambio, e la risposta venire da quell'atto.

### F2 — La dipendenza non stabilita si nomina

Quando la risposta dipende da un contenuto che lo scambio non ha stabilito, la
facoltà produce una **mossa** invece di un muro: nomina la dipendenza, e poi
chiede o assume **dichiarandolo**.

**Riuscita:** il turno 2 produce qualcosa come *«dipende da che cosa conosci
già: dimmelo, oppure assumo che… e allora…»*. Con la stessa regola, e senza
scriverne un'altra, «qual è il libro più bello?» e «che musica mi consigli?»
producono la stessa forma di mossa.

**Non barare:** la dipendenza deve essere **calcolata** dalla derivazione
mancante (`kb_derivation` che non trova una prova ammissibile e sa dire di che
cosa avrebbe avuto bisogno), non scelta da una tabella domanda→dipendenza.

### F3 — Una posizione si intrattiene senza asserirla

«cosa pensi della vita?» apre un **contesto**, non chiede un fatto. La risposta
è una posizione che parrot0 **intrattiene**: `holds_in` con un atto della forza
giusta, e la derivazione disponibile.

**Riuscita:** tre domande di controllo funzionano sulla stessa posizione —
«perché lo dici?» restituisce la derivazione; «lo credi davvero?» distingue
l'intrattenuto dall'asserito; «e se invece…?» apre un secondo contesto senza
cancellare il primo.

**Non barare:** vietato l'aforisma preconfezionato per argomento. Il banco
conta il **modulo** che risponde, come `scripts/coefficiente.sh`: una posizione
che viene dal frasario vale zero anche se è vera e bella.

### F4 — Gli assunti cadono insieme a ciò che reggono

Ritirare un assunto fa cadere le posizioni che lo richiedono e **lascia vive**
quelle che hanno un'altra via. È già la meccanica di M2/M3 della prosa: qui
diventa condotta di conversazione.

**Riuscita:** l'ablazione. Stabilito un assunto, derivata una posizione,
ritirato l'assunto: la posizione cade e lo **dice**; una posizione con doppio
sostegno resta e sa dire con quale.

### F5 — Il registro delle mosse

Una conversazione razionale non è un'interrogazione. Le mosse — **domanda di
ritorno, distinzione, controesempio, concessione, riformulazione** — sono
conoscenza con le loro condizioni d'uso, non rami nel C. Una mossa nuova deve
costare **una riga di KB**.

**Riuscita:** si insegna una mossa nuova parlando, si vede usarla nel turno
successivo, si ritira e sparisce. È il criterio del mantra #2 applicato al
discorso.

---

## 6. Il banco, e come si misura

Stesso disegno della scala di prosa, perché ha funzionato: **una colonna che
dice da dove viene la mossa**, e un numero che si legge invece di stimarlo.

Per ogni turno si registrano tre cose:

| colonna | domanda |
|---|---|
| **ancora** | a quale atto dello scambio si attacca questa mossa? (vuoto = fuori dallo spazio) |
| **sostegno** | quale derivazione la regge? (vuoto = non razionale) |
| **mossa** | risposta, domanda di ritorno, assunto dichiarato, distinzione, muro |

e il punteggio è **la frazione di turni che hanno ancora e sostegno**. Un muro
onesto non è un errore: è una mossa con ancora e senza sostegno, e va contato
per quello che è. Una risposta fluente senza sostegno è il caso peggiore e va
contata come tale — **peggio del muro**, come sulla scala di F.

**Il corpus:** aperture filosofiche vere e generiche — *cosa pensi della vita*,
*il tempo esiste*, *che cos'è giusto*, *si può sapere qualcosa con certezza* —
più le metaconversazionali dell'esempio. Devono essere **aperte**: una domanda
con una risposta sola misurerebbe la conoscenza, non questa abilità.

**Anti-inganno, dal piano della prosa:** il banco è fisso, non si allarga per
abbassare il tasso; le entità non si inventano per far passare un turno; e se
una mossa giusta viene dal frasario, il referto lo dice nella colonna del
modulo e quel turno **non** conta come compreso.

---

## 7. Le trappole prevedibili, scritte prima di caderci

- **Il frasario filosofico.** Trenta aforismi indicizzati per argomento
  passerebbero il banco e non sarebbero l'abilità. È lo stesso guasto misurato
  al 19% sulla prosa, e si riconosce con la stessa colonna.
- **La domanda di ritorno come scappatoia.** Rispondere sempre con una domanda
  sembra socratico ed è un muro travestito. La domanda di ritorno è legittima
  **solo** quando nomina la dipendenza che la rende necessaria.
- **Il relativismo di comodo.** «Dipende dai punti di vista» è fuori dallo
  spazio quanto «c, python»: non si attacca a nessun atto.
- **Confondere intrattenere con credere.** Una posizione detta e non creduta
  deve restare distinguibile a ogni turno successivo, altrimenti la
  conversazione accumula fatti falsi — ed è precisamente il difetto che il
  mondo allargato è stato costruito per togliere.
- **Il turno come stringa.** Se il turno non diventa un contenuto con un atto,
  ogni riferimento all'eschange sarà una cue, e ce ne vorrà una nuova per ogni
  formulazione.

---

## 8. Da dove si comincia

**F1, e non un'altra.** È la più piccola, è misurabile in un turno solo, ed è la
condizione delle altre quattro: finché un turno non è un contenuto con un atto,
non c'è nulla a cui ancorare una mossa, e «nello spazio logico» resta una frase.

Il primo comando da eseguire è la riproduzione dell'esempio, perché il piano si
apre con una misura e non con un'intenzione:

```sh
printf '%s\n' 'sai parlare italiano' \
  'quali parole conosci che possono sorprendermi' \
  'in quale lingua ti ho chiesto quale lingua sai parlare' \
  'cosa pensi della vita?' \
  'secondo te il tempo esiste davvero?' '/quit' | \
  PARROT0_SESSION= PARROT0_LANG=it PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0
```

Le cinque righe della tabella §1 sono il punto di partenza. La prima che deve
cambiare è la terza, e deve diventare **«in italiano»**.
