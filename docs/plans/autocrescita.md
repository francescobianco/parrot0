# L'autocrescita — le domande che parrot0 si fa da solo, e come diventano KB

> **Stato:** aperto a gen433 (20 agosto 2026), su richiesta di F.
> **La tesi di F., testuale:** *«parrot0 cresce solo con miglioramento
> supervisionato; invece sono convinto che possa crescere da solo, perché
> attraverso la wiki può coprire gli archi mancanti: deve accorgersi, porsi delle
> domande e produrre dall'interno un upgrade della sua KB tale da superare la
> missione — e tutto questo è guidato dalla colla linguistica e dal reasoning.»*
>
> **La bussola (F., gen433):** *«il piano ci serve per sbloccare parrot0 da una
> KB statica a una KB fertile»*. Non un punteggio: un **cambiamento di regime**
> della conoscenza — §0a lo definisce con cinque proprietà verificabili, e §10
> gli dà il numero che lo misura.
>
> **Che cosa aggiunge questo piano** ai cinque che lo precedono: non una facoltà
> nuova, ma **un solo ciclo** in cui le parti già costruite si chiudono ad anello,
> e un **portafoglio di strategie parallele** che corrono dentro quel ciclo e si
> misurano sullo stesso banco. Le idee non sono tutte mie: `question-emergence.md`
> §4 aveva già calcolato cinque sorgenti di lacuna, `autocorrezione.md` §13 aveva
> già spostato la riparazione dal monologo al dialogo, e `fix-patterns.md` ha
> contato che forma hanno le riparazioni vere. Qui si mettono in fila.

---

## 0a. Il punto del piano: da KB statica a KB FERTILE (F., gen433)

> *«Non fa nulla che siano discordanti: per adesso il piano ci serve per
> sbloccare parrot0 da una KB statica a una KB fertile.»*

Questa frase è la bussola, e riordina tutto ciò che segue. L'obiettivo non è un
punteggio: è un **cambiamento di stato della conoscenza**. I banchi di §0b
servono a *dimostrare* che il cambiamento è avvenuto — non a guidarlo. Che due
banchi premino cose diverse è un fatto da tenere a mente al momento di leggere i
numeri, non un ostacolo da risolvere prima di partire.

**Statica** e **fertile** non sono aggettivi: sono due regimi con sintomi
distinguibili.

| | KB **statica** (oggi) | KB **fertile** (bersaglio) |
|---|---|---|
| chi aggiunge righe | una persona | anche il sistema, sotto gate |
| che cosa produce una riga nuova | una risposta in più | **una risposta in più e domande nuove** |
| conoscenza che non fa niente | resta lì e nessuno lo sa | **si segnala da sola** (§3f) |
| lo spazio negativo | calcolabile ma non calcolato | calcolato a ogni giro, ed è il motore |
| il limite alla crescita | il tempo di chi scrive | la densità della KB stessa |

### Che cosa rende fertile una KB — cinque proprietà, tutte verificabili

Non sono desiderata: sono le condizioni che, mancando, hanno prodotto i difetti
misurati in `fix-patterns.md`.

1. **Raggiungibile.** Ogni fatto ha almeno una superficie che lo porta a un
   turno. Un fatto senza superficie non risponde *e* non genera domande: è peso
   morto due volte. È la ragione per cui la wiki da sola non basta (§10).
2. **Connessa.** Le entità compaiono come *soggetti* e non solo come argomenti;
   i frame dichiarati hanno soggetti; i fratelli hanno profili confrontabili. È
   la condizione perché lo spazio negativo sia **calcolabile**: le sorgenti 3a,
   3b, 3c sono tutte deduzioni sopra questa connessione.
3. **Autodescritta.** La meccanica si dichiara (`machinery/1`), quindi è
   separabile dal mondo. Senza, l'audit annega nel rumore e l'induzione riempie
   i suoi sedici posti con `content_kind(X) :- countable_opener(X)` — misurato al
   gen432.
4. **Tracciabile e revocabile.** Ogni riga porta provenienza e origine. Senza,
   una crescita automatica non si può disfare, e nessuno la lascerà girare.
5. **Interrogante.** La sintesi delle prime quattro: **una KB fertile fa
   domande.** Se lo spazio negativo calcolato è vuoto, la KB non è completa —
   è cieca.

### Come si vede che siamo ancora nel regime statico

Tre sintomi, tutti osservati questa settimana:

- **la crescita è proporzionale al tempo di una persona.** 959 righe in sei
  generazioni, tutte scritte a mano;
- **la conoscenza morta non si lamenta.** Sette difetti su sette erano righe
  dichiarate che non potevano funzionare, e sono emersi per caso;
- **una riga nuova produce una risposta, non una domanda.** Nessuna delle 507
  righe aggiunte ha fatto nascere una domanda che parrot0 si sia posto da solo.

> **Il piano è finito quando il terzo sintomo si rovescia**: una riga aggiunta
> genera, in media, almeno una domanda che parrot0 sa aggredire. Quel numero ha
> un nome in §10 e si chiama **R** — la fertilità *è* R, e la massa critica è il
> punto in cui la fertilità si autosostiene.

## 0. La riformulazione verificabile

La tesi, detta in modo che si possa falsificare:

> **Esiste un ciclo — lacuna, domanda, candidato, prova, promozione — in cui
> parrot0 aggiunge alla propria KB conoscenza che NESSUNO gli ha dato, e il saldo
> sui banchi di misura è positivo dopo il giro.**

Le tre parole che portano il peso:

- **nessuno gli ha dato**: il candidato non arriva da un turno di F. né da un
  file curato. Arriva da un corpus statico, dal proprio stato, o dalla forma del
  proprio fallimento;
- **saldo positivo**: non «ha aggiunto righe», ma *`make test` resta verde e
  `make hundred` / `make measure` salgono*. Una riga che chiude un prompt e ne
  rompe un altro non è crescita;
- **dopo il giro**: la promozione è un atto separato e revocabile, non l'effetto
  collaterale di aver risposto.

E la contro-tesi, che va tenuta in vista perché è il modo naturale in cui questo
piano fallisce:

> Un sistema che si autocorregge senza vincolo diventa un sistema che **si
> convince**. Produrre righe plausibili è facile; produrne di vere è il problema.
> Il perimetro di §8 non è prudenza: è la parte del progetto.

---

## 0b. La condizione che chiude il piano (F., gen433)

La bussola è §0a — il passaggio da statica a fertile. Questa sezione è la
**prova esterna** che il passaggio è avvenuto: serve a non poterselo raccontare.
Il piano non è finito quando i cicli girano; è finito quando vale **questa
congiunzione**, e le tre parti vanno insieme:

> **K — crescita.** Un prompt lascia dietro di sé una **riga di KB** che è
> sopravvissuta all'ablazione e al gate, con la sua provenienza.
>
> **P — risposta.** *Lo stesso prompt* riceve una **risposta plausibile**: non un
> muro, non un template fuori bersaglio, non un'invenzione.
>
> **S — LLMSCORE ≥ 80%.** Il giudizio esterno di somiglianza comportamentale sta
> a quattro quinti, e ci resta su più esecuzioni.

**K e P sullo stesso turno** è la parte esigente, ed è voluta: è *«un solo atto di
apprendimento — detto e letto coincidono»* (gen407). Un sistema che cresce in un
processo batch e risponde bene in un'altra sessione non ha dimostrato l'anello:
ha dimostrato due metà che non si toccano.

### La morsa — perché S da solo sarebbe una trappola

`LLMSCORE.md` all'ultima esecuzione dice **0/20**, e le motivazioni del giudice
sono quasi tutte la stessa: *«a generic template that completely ignores the
question»*. È la classe D dei fallimenti, la peggiore, quella che **sembra** una
risposta. Il che rende evidente il modo sbagliato di arrivare all'80%:
**inventare**. Un generatore di prosa plausibile salirebbe in fretta.

E c'è di più, e va detto perché è scomodo: **i due banchi non sono d'accordo su
che cosa sia un successo.** Il disclaimer dei cento conta come risposta il
*limite dichiarato con precisione*; il giudice di LLMSCORE dà **0** a
*«the answer explicitly refuses to perform the task»* — è il voto della domanda
14 dell'ultima esecuzione. Quindi:

> **L'80% di LLMSCORE non si raggiunge dichiarando meglio i limiti. Si raggiunge
> sapendo fare le cose.**

Da qui il vincolo che tiene onesto l'obiettivo, e che chiamo **la morsa**:

| bene | che cosa premia | come si bara |
|---|---|---|
| **LLMSCORE** | fare la cosa chiesta | inventare testo plausibile |
| **i cento** (`make hundred`) | rispondere *quello che va risposto* | dichiarare limiti a tappeto |

**Per adesso la discordanza non blocca niente** (F.): si prende nota e si
guardano i due numeri accanto, invece di fermarsi a conciliarli. Ma quando si
leggeranno, la regola è una: **S vale solo se sale INSIEME a `make hundred`.** Le attese curate dei cento sono
costruite perché nessun ripiego le passi: un'invenzione plausibile fallisce lì
mentre passa a LLMSCORE. Le due misure in opposizione formano il rilevatore che
il perimetro di §8 non può dare da solo — §8 impedisce di scrivere frasi nuove,
la morsa mostra se qualcuno ha trovato un modo di farlo comunque.

Tre note di misura, perché la condizione sia usabile e non un'aspirazione:

1. **su più esecuzioni.** La coda di LLMSCORE è di 20 domande generate ogni volta:
   un 80% singolo è rumore. Serve la mediana su almeno cinque giri.
2. **i timeout contano.** Nell'ultima esecuzione tre zeri su venti erano
   *«local timeout after 1.0s»*: parte della distanza non è comprensione, è
   tempo di risposta, e va misurata come tale invece di essere confusa con essa.
3. **K si conta, non si dichiara.** «Il prompt ha prodotto crescita» significa:
   la riga esiste, ha una provenienza, e togliendola quel prompt torna a
   fallire. Le prime due sono registrazione; la terza è l'ablazione di §5.

## 1. Che cosa è già vero (inventario, con i numeri)

| pezzo | stato | dove |
|---|---|---|
| il ciclo di riparazione (muro → lacuna → ponte → verifica) | **c'è**, e si attiva su 3 fallimenti su 88 | `question-emergence.md`, gen406-411 |
| le cinque sorgenti di spazio negativo calcolabili dalla KB | **progettate**, mai messe in ciclo | `question-emergence.md` §4 |
| le regole morte come fatti interrogabili | **c'è** il lato statico | `kb_dead_rules/4` → `dead_rule/2`, `inert_rule/1` |
| l'impronta dell'inferenza per turno | **c'è**, usata solo per misurare | gen422, `kb_footprint*` |
| l'insegnamento a voce, con effetto immediato | **c'è** | gen427-429, `literal_forms.p0t` (49 assert) |
| il muro che consegna la frase con cui insegnargli | **c'è** | gen430, `word_teaching_offer/2` |
| due banchi che producono lacune tipate | **ci sono** | `make hundred -v`, `make measure` |
| la colla linguistica come metrica | **c'è**, 11/11 | `make glue-bench` |
| l'ingestione di prosa con forme dichiarate | **c'è** | `extract_frame/2`, raccolti misurando il corpus (gen382) |
| **il ciclo che li lega** | **manca** | *questo piano* |

> Il punto da non perdere: **quasi tutto esiste già e non è collegato.** Questo
> piano è per il 90% cablaggio, e per il 10% due meccanismi nuovi (§3f, §4).

---

## 2. Il ciclo unico

Tutte le strategie di §4 sono **istanze dello stesso ciclo**. È una scelta di
progetto, non un'estetica: se ogni strategia avesse il suo ciclo, avremmo cinque
modi di sbagliare e nessun modo di confrontarli.

```
  (1) SEGNALE          una lacuna si manifesta o si calcola
        │              — un turno che si ferma, o un buco nello spazio negativo
        ▼
  (2) DOMANDA TIPATA   la lacuna diventa un OGGETTO, non un messaggio:
        │              gap(Kind, Subject, Relation, Position)
        ▼
  (3) CANDIDATO        una riga di KB proposta, di forma A, B o C (mai D)
        │              generata dalla sorgente che compete a quel Kind
        ▼
  (4) PROVA            si asserisce in sessione e SI RIPONE IL TURNO
        │              — l'oracolo è la ripetizione, non un giudizio
        ▼
  (5) ABLAZIONE        si toglie e si ripone: se passa lo stesso, non serviva
        │
        ▼
  (6) GATE             `make test` verde E saldo ≥ 0 su hundred/measure
        │
        ▼
  (7) PROMOZIONE       sessione → quarantena (`KB_INDUCED`) → KB ufficiale
                       con provenienza, e revocabile in un colpo
```

**Il punto (4) è quello che rende il ciclo possibile oggi.** Il criterio di
accettazione attuale — *«la risposta è diversa dal muro?»* — è il buco che tiene
spento l'interruttore (`autocorrezione.md` §5). Riporre il turno lo sostituisce
con qualcosa che non richiede il substrato della pertinenza: o la lacuna si
chiude o non si chiude. E il punto (5) è ciò che impedisce alla KB di gonfiarsi
di righe che non fanno niente — la prima difesa contro il difetto che
`fix-patterns.md` §4 ha trovato sette volte.

---

## 3. Le sorgenti di lacuna, e la domanda che ognuna produce

Sei sorgenti. Le prime cinque sono di `question-emergence.md` §4, qui rilette con
la domanda che generano; la sesta è nuova e viene da `fix-patterns.md`.

### 3a. Asimmetria fra fratelli
*Segnale:* quindici membri di un tipo dichiarano una relazione, uno no.
*Domanda:* «di **questo** membro, quanto vale la relazione che tutti gli altri
hanno?» → `gap(fact, poker, game_players, _)`.
*Candidato:* dal corpus. **Forma B.**

### 3b. Frame dichiarati senza dati
*Segnale:* `answer_frame(Cue, Pred)` esiste, `Pred` non ha fatti per quel
soggetto. *Domanda:* già nella forma esatta del fatto che la colma.
*Candidato:* dal corpus. **Forma B.** È la sorgente più pulita che esista: la
domanda **è** il fatto mancante, con un buco al posto del valore.

### 3c. Entità opache
*Segnale:* un termine compare solo come argomento, mai come soggetto.
*Domanda:* «che cos'è?» → `gap(concept, straight_flush, _, _)`.
*Candidato:* `wiki_concept` dal corpus. **Forma B.**

### 3d. Regole morte e dialetti privati
*Segnale:* `inert_rule/1` (statico, già pubblicato).
*Domanda:* «questa regola non può dedurre niente: le manca un produttore, o parla
un dialetto che solo lei capisce?»
*Candidato:* un ponte fra il dialetto privato e la relazione generale. **Forma E**
— quindi **non automatica**: si propone, non si promuove (vedi §8).

### 3e. Superficie irraggiungibile
*Segnale:* una facoltà dichiarata potrebbe servire il turno e **nessuna cue
combacia**. *Domanda:* «con quali parole me lo stai chiedendo?»
*Candidato:* una sottostringa del turno, oppure — ed è la parte nuova — **una
forma espressiva raccolta dalla prosa** (§4.1). **Forma A.** È la classe più
numerosa: ~100 righe su 507 (`fix-patterns.md` §1).

### 3f. Conoscenza mai toccata *(nuova)*
*Segnale:* l'aggregato delle **impronte di inferenza** su un corpus dà i
predicati — e, scendendo, le righe — che nessun turno ha mai attivato.
*Domanda:* **«quali cose che dico di sapere non hanno mai fatto niente?»**
*Candidato:* nessuno da generare. La risposta è o un difetto del motore o
conoscenza da ritirare. **È l'unica sorgente che non richiede un turno**: parrot0
può porsela a freddo, e i sette difetti del gen427-432 sarebbero caduti tutti
qui (la sterlina confrontata su un carattere, i frame al passato uccisi dalla
copula, i registri letti a 16 su 18…).

> Le sei si dividono in due famiglie, ed è la divisione che conta per il piano:
> **a, b, c, f si calcolano dalla KB a freddo** — parrot0 può porsele mentre
> nessuno parla, che è la definizione operativa di *crescere da solo*; **d, e
> nascono da un turno**, e sono il ponte con il dialogo.

---

## 4. Le strategie da sperimentare in parallelo

Cinque cicli. Tutti validi, tutti da provare, **tutti con lo stesso oracolo e lo
stesso gate** — così si possono far correre insieme e confrontare sul saldo.

### S1 — Il ciclo del fatto (la wiki, e la scommessa di F.)
*Sorgenti:* 3a, 3b, 3c. *Forma:* B. *Fonte:* corpus statico.
La lacuna nomina la relazione e il soggetto; il corpus si indicizza per nome; il
candidato è **citabile** — e questa è la sua forza: la promozione può richiedere
la provenienza, quindi un fatto entra solo se si può dire da dove viene.
*Esperimento minimo:* prendere le entità opache che la KB già dichiara, generare
le domande, cercarle nel corpus, promuovere solo quelle con fonte, e misurare il
saldo su `hundred`.
*Come falsisce:* se le entità opache colmate non muovono nessun banco, la
scommessa «i fatti mancanti sono il collo di bottiglia» è sbagliata — ed è
esattamente ciò che `fix-patterns.md` sospetta (i fatti sono la terza classe per
numerosità, non la prima).

### S2 — Il ciclo della superficie (la più numerosa, e la meno ovvia)
*Sorgenti:* 3e. *Forma:* A. *Fonte:* il turno, **e la prosa**.
Il precedente vivo è il gen382: le forme `extract_frame("@S is known as @O", …)`
non sono state immaginate, sono state **raccolte misurando il corpus** («known
as» ricorre 15 volte in 49 pagine). Generalizzato: leggere la prosa non per i
fatti ma per **come i fatti vengono detti**, e proporre le forme ricorrenti come
`extract_frame` o `intent_cue`.
*Esperimento minimo:* far girare il raccoglitore su un blocco di pagine, tenere
le forme che ricorrono sopra una soglia, provarle con l'ablazione su `hundred`.
*Come falsisce:* se le forme raccolte sono rumore (combaciano su turni che non
sono di quella relazione), il ciclo va gated con l'ablazione più severa o
abbandonato.

### S3 — Il ciclo della classe (il più economico, il più rischioso)
*Sorgenti:* un motore consulta una classe unaria e il token non è membro.
*Forma:* C. *Fonte:* il token stesso.
*Esperimento minimo:* registrare per un giorno tutte le interrogazioni di classe
fallite, proporre il token, e verificare con l'**ablazione doppia**: senza la
riga il turno fallisce, con la riga passa, e nessun altro turno cambia.
*Come falsisce:* se l'ablazione doppia scarta quasi tutto, la classe non è la
lacuna e il segnale era un sintomo.

### S4 — Il ciclo dell'audit (il solo che non aspetta nessuno)
*Sorgenti:* 3d, 3f. *Forma:* nessuna riga proposta — **una domanda su di sé**.
*Esperimento minimo:* aggregare le impronte su `hundred` + `measure` + `make
test`, produrre l'elenco dei predicati mai toccati, e leggerlo. Si prevede che
contenga: difetti del motore, dialetti privati, e conoscenza legittimamente
dormiente (che va marcata, non ritirata).
*Come falsisce:* se l'elenco è dominato dalla terza categoria, serve prima un
modo di dichiarare «dormiente per disegno», altrimenti il segnale annega.

### S5 — Il ciclo del dialogo (quello che già gira)
*Sorgenti:* 3e in conversazione. *Forma:* A, C.
È il gen430: il muro consegna la frase con cui glielo si insegna, e la risposta
dell'interlocutore ha effetto nel turno dopo. Non è autocrescita — è crescita
**assistita a costo minimo**, e serve da controllo: se S1-S4 non battono S5, il
piano non ha dimostrato niente.

> **Perché in parallelo e non in sequenza.** Le cinque non competono per lo
> stesso buco: competono per il **saldo**. Farle correre insieme sullo stesso
> banco è l'unico modo di scoprire quale sorgente porta crescita vera — ed è
> anche l'unico modo onesto, perché nessuno di noi sa in anticipo la risposta.

---

## 5. Il gate unico

Una riga proposta da qualunque ciclo attraversa gli stessi quattro cancelli:

1. **prova**: asserita in sessione, il turno che l'ha motivata passa;
2. **ablazione**: tolta, quel turno torna a fallire (se non torna a fallire, la
   riga non serviva: si scarta, e si registra che la lacuna era altrove);
3. **non-regressione**: `make test` verde, e saldo ≥ 0 su `make hundred` e `make
   measure` — i due banchi sono giudici, non pagelle;
4. **provenienza**: la riga entra come `KB_INDUCED` con la sorgente che l'ha
   prodotta e il turno che l'ha motivata, e resta distinguibile da ciò che una
   persona ha deciso (`kb/learning/`, versionato a parte, revocabile in blocco).

La promozione da quarantena a KB ufficiale è **un secondo atto**, con la sua
soglia: una riga in quarantena che sopravvive a N giri di banco senza essere
scartata diventa ufficiale. Fino ad allora risponde, ma si sa che è in prova.

---

## 6. Perché la colla linguistica è il driver, e non un ornamento

F. dice che il processo è guidato dalla colla. Ha ragione in un senso preciso, e
vale la pena renderlo operativo: **i cinque sintomi dell'assenza di colla sono
cinque rilevatori di lacuna**, e sono già misurati (`make glue-bench`, 11/11).

| sintomo (essay) | che lacuna segnala | sorgente |
|---|---|---|
| risposta corretta ma **fuori contesto** | il vincolo attivo non è stato letto | 3e |
| **riferimenti impliciti persi** | manca l'antecedente, o la regola che lo porta | 3d |
| **letteralità eccessiva** | la forma è stata letta, il seguito no | 3e |
| **correzioni non integrate** | manca la supersessione, o la mossa non è riconosciuta | 3d/3e |
| **più sistemi invece di un interlocutore** | due facoltà non condividono lo stato | 3d |

Il legame profondo è questo: **la colla è ciò che rende una discontinuità
visibile**. Senza continuità dichiarata, un turno che va storto è solo un turno
che va storto; con la continuità, il punto in cui la storia si spezza *è* la
domanda. È la stessa idea di `autocorrezione.md` §0 — l'inferenza che riporta
dove si è fermata — detta al livello del discorso invece che della clausola.

E il reasoning è l'altra metà: la domanda non si **estrae**, si **deriva**. Le
sorgenti 3a-3c sono tutte deduzioni sopra la KB (asimmetria fra fratelli, prodotto
frame × entità, chiusura degli argomenti), cioè `findall` e regole — non
scansioni scritte in C. Ogni sorgente nuova deve poter essere aggiunta come
**regola**, altrimenti abbiamo un rilevatore di lacune che non sa imparare a
vedere lacune nuove, e sarebbe ironico.

---

## 7. La mappa sorgente → forma → fonte (il cuore operativo)

| sorgente | forma della riga | chi genera il candidato | automatica? |
|---|---|---|---|
| 3a asimmetria | **B** fatto | corpus, indicizzato per nome | **sì** |
| 3b frame senza dati | **B** fatto | corpus | **sì** |
| 3c entità opaca | **B** concetto | corpus | **sì** |
| 3d regola morta | **E** procedura | — (proposta al revisore) | no |
| 3e superficie | **A** cue | il turno, o la prosa | **sì**, con ablazione |
| 3f mai toccata | — | — (audit) | **sì**, ma non propone righe |
| — | **C** classe | il token del turno | **sì**, con ablazione doppia |
| — | **D** frase | **nessuno** | **mai** (§8) |

---

## 8. Il perimetro anti-impostore

Tre regole, e la prima non è negoziabile.

**8.1 — La forma D non ha un generatore.** 66 righe delle 507 contate in
`fix-patterns.md` sono *cosa dire*: `response_template`, `own_method`. È la classe
che un generatore libero produrrebbe **bene e falsamente**, ed è esattamente il
*misclaim* che i cento chiamano la classe peggiore — quella che sembra una
risposta. Il ciclo può comporre da template già in KB; non può scriverne di
nuovi. Se una lacuna richiede parole nuove, il ciclo **la dichiara e si ferma**.

**8.2 — Un candidato senza fonte non entra.** Per la forma B la provenienza è
citabile e va citata. Per A e C la «fonte» è il turno o la ricorrenza nel corpus,
e va registrata con la stessa serietà: `learned_from(Riga, Turno|Pagina)`.

**8.3 — Il ciclo non tocca il motore.** Le sorgenti 3d e 3f *trovano* difetti del
C: li **riportano**, non li riparano. La via per le facoltà resta `LOOP.md`; la
via per la conoscenza è questo piano. Confonderle significherebbe un processo
automatico che riscrive il proprio motore mentre lo usa per giudicarsi.

> E una quarta, che è una lezione pagata questa settimana: **una riga che non fa
> niente è peggio di una riga che manca**, perché non si lamenta. L'ablazione di
> §5.2 e l'audit di 3f esistono per questo.

---

## 9. Le fasi, con il criterio che le chiude

Una per generazione, ognuna col suo cricchetto. L'ordine non è per importanza: è
per **quanto ciascuna rende possibile la successiva**.

| fase | che cosa costruisce | criterio (una misura che cambia) |
|---|---|---|
| **F0** | **La lacuna diventa un oggetto tipato.** `gap(Kind, Subject, Relation, Position)` con i quattro Kind di §9.1 di question-emergence, e i banchi che li producono: `make hundred -v` scrive lacune invece di righe di testo | i cento falliti producono N lacune tipate stabili fra due esecuzioni, e lo stesso prompt non cambia tipo a seconda di ciò che è successo prima |
| **F1** | **L'audit a freddo (3f).** Aggregazione delle impronte su tre corpora, elenco dei predicati mai toccati, `never_fired/1` pubblicato come fatto | l'elenco contiene almeno uno dei sette difetti noti del gen427-432, riprodotto **senza** che nessuno lo indichi |
| **F2** | **Il ciclo del fatto (S1) su 3c.** Entità opache → domanda → corpus → quarantena | K entità opache colmate con provenienza, `make test` verde, saldo `hundred` ≥ 0 |
| **F3** | **L'ablazione come cancello.** Ogni riga in quarantena viene tolta e riprovata | il numero di righe scartate dall'ablazione è > 0 — se è zero, il cancello non sta misurando niente |
| **F4** | **Il ciclo della superficie (S2).** Raccolta di forme espressive dalla prosa | almeno una `extract_frame` raccolta dal corpus chiude un turno che prima falliva |
| **F5** | **Il parallelo.** I cicli corrono insieme, ognuno tiene il proprio saldo | si può dire, con i numeri, quale sorgente ha portato più crescita per riga aggiunta |
| **F6** | **La promozione.** Quarantena → ufficiale dopo N giri puliti | esiste conoscenza in `kb/core/` che nessuno ha scritto a mano, e il progetto sa dire da dove viene |

**Il criterio dell'intero piano** è la congiunzione di §0b — **K, P e S
insieme**, con la morsa che tiene onesto S. E una fase in più, che si può aprire
solo dopo F6 perché prima non ci sarebbe niente da contare:

| fase | che cosa costruisce | criterio |
|---|---|---|
| **F7** | **La curva di R** (§10): a ogni giro, lacune chiuse e nuove lacune autochiudibili, misurate sulle lacune dei banchi larghi | si può disegnare la curva, e si vede se la media mobile sale verso 1 |

La scommessa, scritta per poter essere persa: *dopo F6, una campagna di crescita
autonoma di una settimana muove `make hundred` o `make measure` più di quanto li
muova una settimana di lavoro supervisionato, e la curva di R non scende.* Se non
succede, la tesi è sbagliata **per questo sistema**, e va detto.

---

## 10. L'ipotesi della massa critica (F.)

> *«Secondo me esiste una massa critica di KB che garantisce l'autoapprendimento:
> parrot0 è un processo autoaddestrante a prescindere, ma esiste uno stato della
> KB dopo il quale l'autoaccrescimento è garantito, almeno in spazi
> conversazionali ampi.»*

La distinzione che F. fa è giusta e conviene renderla netta, perché cambia che
cosa si misura:

- **processo**: parrot0 *già* si autoaddestra a ogni turno — scrive fatti di
  sessione, registra lacune, lascia un'impronta. Questo è vero da molte
  generazioni e non è ciò che manca;
- **regime**: ciò che cambia alla massa critica non è che *cominci* a imparare,
  è che **smette di dover essere alimentato**. Il processo passa da *nutrito* a
  *autosostenuto*.

### Che cosa sarebbe, meccanicamente

Ogni lacuna chiusa modifica lo spazio negativo: un fatto nuovo rende applicabili
frame che prima non avevano soggetto, introduce argomenti che diventano entità
(alcune opache), completa o rompe simmetrie fra fratelli. Cioè **una chiusura
genera altre lacune** — e la domanda è quante di quelle nuove parrot0 sappia
chiudere **da solo**.

> **R = numero medio di lacune AUTOCHIUDIBILI aperte da ogni lacuna chiusa.**
>
> **R è la fertilità della KB, misurata** (§0a): quanto una riga aggiunta
> *produce lavoro che il sistema sa fare*, invece di produrre solo una risposta.

- **R < 1** — il processo si spegne: ogni giro produce meno lavoro di quanto ne
  consumi, e la crescita deve essere alimentata da fuori. **È il regime di oggi**,
  ed è precisamente il significato di *«cresce solo con miglioramento
  supervisionato»*;
- **R ≥ 1** — il processo si sostiene: non finisce le domande che sa aggredire
  con le proprie sorgenti. **La massa critica è lo stato della KB in cui R
  attraversa 1.**

È una soglia su uno **stato**, non su una **quantità**, e questo spiega perché
l'intuizione di F. non è «serve una KB grande»: R dipende dalla *densità delle
connessioni* — frame × entità, fratelli, entità opache, superfici — non dal
numero di fatti.

### La previsione che rende l'ipotesi falsificabile

Da `fix-patterns.md` esce una conseguenza non ovvia, e vale come predizione:

> **La massa critica non è una massa di FATTI: è un rapporto fra fatti e
> SUPERFICI.** Un fatto che nessuna superficie sa raggiungere non apre nessuna
> domanda nuova — è conoscenza morta (§3f) — quindi **pompare solo la wiki
> abbassa R** invece di alzarlo: aggiunge numeratore senza denominatore.

Se questa previsione è giusta, si vede: una campagna che aggiunge solo forma B
fa salire il conto dei fatti e **non** il numero di lacune autochiudibili per
giro. Se è sbagliata, R sale comunque, e allora la strategia S1 da sola basta e
S2 è un lusso. **In entrambi i casi lo si misura in un giro di campagna.**

### Come si osserva l'attraversamento

R non va stimato: si conta, e la strumentazione è la stessa dei cicli.

1. a ogni giro si registrano **lacune chiuse** (righe promosse) e **lacune nuove
   autochiudibili** (quelle le cui sorgenti hanno un generatore di candidati,
   §7);
2. `R_giro = nuove_autochiudibili / chiuse`, e si guarda la **curva su più
   giri**, non il singolo valore;
3. la massa critica è **osservata**, non decisa: è il primo giro da cui la media
   mobile di R resta ≥ 1 mentre il saldo sui banchi resta ≥ 0.

Le due condizioni insieme, e nessuna delle due da sola:

- **R ≥ 1 con saldo piatto** = *fuga in una tasca*. Il caso classico: parrot0
  scopre trecento entità opache di un dominio marginale, le colma tutte, si pone
  altre trecento domande, e nessun banco si muove. È autosostentamento senza
  crescita, ed è il fallimento più insidioso perché **assomiglia al successo**;
- **saldo positivo con R < 1** = crescita vera ma alimentata: è il regime di
  oggi, ed è esattamente ciò che il piano vuole superare.

### «In spazi conversazionali ampi»

La clausola di F. non è un ammorbidimento: è la definizione del banco. R va
misurato **sulle lacune che nascono dai corpora larghi** — i cento, le classi
misurate, la coda di LLMSCORE — non su quelle che il ciclo si genera da solo
esplorando la KB. La differenza fra le due misure di R **è** la misura della
tasca: se R è alto sulle proprie e basso su quelle esterne, il sistema si sta
parlando addosso.

> **La forma finale dell'ipotesi, come scommessa che si può perdere:** esiste
> uno stato della KB, raggiungibile per aggiunte, in cui `R ≥ 1` misurato sulle
> lacune dei banchi larghi, con saldo non negativo. Se dopo F6 la curva di R
> resta sotto 1 in ogni configurazione delle cinque strategie, l'ipotesi della
> massa critica è **falsa per questo sistema** — e il valore del piano diventa
> quello di averlo mostrato con dei numeri invece che averlo supposto.

## 11. Come questo piano può fallire (e come ce ne accorgiamo)

- **Fallimento silenzioso**: il ciclo gira, aggiunge righe, i banchi non si
  muovono. Sintomo: molte promozioni, saldo piatto. Rimedio: l'ablazione di §5.2
  è già il rilevatore — se scarta quasi tutto, la sorgente è sbagliata.
- **Deriva del corpus**: parrot0 impara i fatti di un'enciclopedia e diventa
  bravo a rispondere a domande da enciclopedia, mentre i cento non si muovono.
  Sintomo: `hundred` fermo, `measure` fermo, e una KB che cresce. È il rischio
  principale di S1, e il motivo per cui S2 esiste.
- **Il generatore di frasi che rientra dalla finestra**: qualcuno, per chiudere
  una lacuna, aggiunge un `response_template` «solo questa volta». Sintomo: le
  righe di forma D in quarantena. Rimedio: il gate le rifiuta per costruzione.
- **L'autoconvinzione**: il ciclo verifica con l'oracolo che ha scelto. Rimedio:
  l'oracolo non è scelto dal ciclo — è *riporre lo stesso turno* più i banchi,
  che il ciclo non controlla.
- **La fuga in una tasca** (§10): R sale, il saldo no. È il fallimento che
  assomiglia di più al successo — un sistema che si pone domande sempre più
  numerose su un angolo sempre più stretto. Rilevatore: la differenza fra R
  misurato sulle lacune proprie e R misurato su quelle dei banchi larghi.
- **La scalata a LLMSCORE per invenzione**: S sale e `make hundred` scende.
  Rilevatore: la morsa di §0b, che è l'unico posto dove le due misure vengono
  confrontate invece che lette separatamente.

---

## Riferimenti

- [`../autocorrezione.md`](../autocorrezione.md) — la teoria; §0 preliminare vs
  postuma, §6 il substrato, §13 l'autocorrezione fatta in due
- [`fix-patterns.md`](fix-patterns.md) — le sette forme di una riparazione,
  contate; la mappa forma → sorgente che questo piano usa in §7
- [`question-emergence.md`](question-emergence.md) — §4 le cinque sorgenti di
  spazio negativo; §9.1 i quattro difetti (knowledge, reachability, surface,
  wrong-answer)
- [`universal-input.md`](universal-input.md) — la linea fra motore e conoscenza,
  che §8.3 non attraversa
- [`teach-comprehension-via-prompt.md`](teach-comprehension-via-prompt.md) —
  l'inventario delle superfici, cioè il magazzino di S2/S5
- [`frontier-kb-natural-dialogue.md`](frontier-kb-natural-dialogue.md) — K0-K4:
  forma/senso, frame del turno, stato dialogico, contesti
- [`the-linguistic-glue.md`](the-linguistic-glue.md) — i cinque sintomi che §6
  trasforma in rilevatori
- `docs/plans/parrot0-100-failures.md`, `docs/measured-classes.md`, `LLMSCORE.md`
  — i tre banchi che fanno da giudice, e le due misure che formano la morsa (§0b)
