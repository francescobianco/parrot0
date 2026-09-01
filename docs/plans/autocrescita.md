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

> **Aggiornamento 2026-09-01 — l'assenza utile è un bisogno nominato.** La
> distinzione fra «non ho capito» e «ho capito ma mi manca un oggetto» è ora
> operativa come `information_need(Turno, Specie, Mancante, Goal)`. Un valore del
> mondo assente e un antecedente discorsivo assente condividono la stessa forma,
> ma non lo stesso rimedio: il primo chiede conoscenza, il secondo contesto. La
> strategia scala perché non enumera prompt o domini; qualunque relazione
> interrogabile e qualunque superficie ordinale aggiunta alla KB entra nello
> stesso ciclo. È il ponte minimo fra arresto in-linea e domanda fertile: il
> sistema non cerca tutte le assenze possibili, nomina solo quella che ha
> bloccato il turno reale e rende il replay l'oracolo della riparazione.

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

### 0a-bis. La soglia minima è la comprensione insegnabile

Il giro supervisionato di `--dream water` (gen436) rende operativa una
distinzione che prima restava implicita. Il dream legge la prima frase e ottiene
`inorganic_compound(water)`, poi si ferma sulla frase:

```text
It is a transparent, tasteless, odorless, and nearly colorless chemical substance.
```

Provare a insegnare parlando `adjective(transparent)`, `adjective(tasteless)` e
le altre parole cambia il lessico, ma non insegna la forma compositiva della
frase: `water is transparent` resta incompresa. Il reperto è importante perché
separa **dato mancante** da **forma mancante**.

La soglia minima non è quindi «la KB contiene abbastanza righe per rispondere»;
è lo stato in cui una nuova forma necessaria può essere insegnata attraverso il
canale naturale di parrot0, poi usata dal dream, dalla lettura della prosa,
dall'autocorrezione e dalla ripresa dello stesso turno. Il dream supervisionato
serve a raggiungere questa soglia: espone la frase, arresta il primo scarto,
permette l'insegnamento, riavvia il topic e non consente di nascondere il difetto
con profondità o riassunti.

Questo introduce una misura più severa della fertilità: **la teachability della
forma minima**. Se per far riconoscere una classe occorre aggiungere a mano una
riga in un file `.p0`, quella riga è ancora un intervento di sviluppo, non una
crescita fertile; indica che il canale di insegnamento non sa esprimere quella
struttura. La riga `.p0` può essere ammessa soltanto come ultima promozione della
forma più astratta che rende poi la classe insegnabile via prompt, dream, lettura
della prosa o discovery/autocorrezione delle lacune.

Il protocollo diventa:

1. dream supervisionato espone la prima frase e il primo arresto;
2. si tenta l'insegnamento parlando, non si aggiunge subito un fatto curato;
3. si riavvia lo stesso dream e si verifica che la forma abbia aperto il passo;
4. se il prompt non può insegnare la forma, si astrae il motore necessario;
5. solo allora si promuove una riga `.p0`, una sola, come seme riusabile per
   tutti i canali di crescita.

La comprensione totale è il limite di questo processo: non significa aver
memorizzato ogni pagina, ma non perdere la sostanza di una frase perché manca
una forma che il sistema non sa né riconoscere né insegnare.

### ⛔ IL PERIMETRO — che cosa questo piano NON è (F., gen435)

> *«Il meccanismo di colmare le lacune staticamente con un tool dedicato non è
> accettato e non ci serve. Le lacune vanno individuate A TEMPO DI INFERENZA: la
> scoperta delle lacune è parte dell'inferenza stessa. Tu hai travisato questo
> piano con una sorta di DEFRAG di memoria della KB. Stiamo parlando di una KB di
> frontiera capace di essere AUTOCORRETTIVA PER STATUS DI KB, non di colmare
> meccanicamente tutte le lacune.»*

**La distinzione, in una riga:**

> **Una lacuna non è un'assenza nella KB: è un ARRESTO nell'inferenza.**

Non è una sfumatura, e le tre conseguenze si misurano:

| | assenza *(fuori perimetro)* | **arresto** *(il piano)* |
|---|---|---|
| quante ne esistono | **illimitate**, e crescono con la KB | **una per turno**, e solo quando un turno c'è |
| che rimedio chiede | indeterminato: «riempi» | **determinato e tipato**: quella posizione, quel pezzo |
| che oracolo ha | nessuno — come sai se colmarla è servito? | **riponi il turno**: o si legge o non si legge |
| che cosa produce | un **elenco** | una **mossa** |

**«Autocorrettiva per status di KB»** è la parte che avevo perso, e va detta per
esteso: la capacità di correggersi è una proprietà della **configurazione** della
conoscenza — quali relazioni esistono, quali superfici le raggiungono, quali
regole sanno dire *dove si sono fermate* — **non** di un programma che la
ispeziona. Un tool esterno che ripara lascia la KB **nello stato di prima**: la
volta dopo serve ancora il tool, e quella non è autocorrezione, è manutenzione.
È anche il motivo per cui la domanda giusta è quella di §0c: **si cerca lo
STATO in cui il meccanismo si accende, non la procedura che lo sostituisce.**

**Le tre domande da fare a ogni proposta** — comprese le mie, e soprattutto le
mie:

1. **gira DENTRO un turno, o accanto a un turno?** Accanto → fuori perimetro;
2. **quella lacuna esisterebbe se nessuno avesse parlato?** Se sì, non è una
   lacuna di questo piano: è un'assenza, e le assenze sono infinite;
3. **produce un elenco o una mossa?** Elenco → fuori perimetro.

**Il verdetto su ciò che è stato costruito a gen435**, scritto qui perché resti
come esempio e non come precedente: `make audit` risponde *«accanto»*, *«sì»* e
*«elenco»* — **tre su tre fuori**. Era un defrag della KB travestito da domanda
autoriflessa. È stato retrocesso a misura dei **banchi** e rinominato
(`make bench-coverage`), e **non rientra nel ciclo**. La sorgente 3f resta a
memoria del ragionamento sbagliato, con la misura che l'ha smentita.

### La KB non si completa: si anima (F., gen435)

Va scritto qui perché è il modo più facile di sbagliare tutto il piano, ed è
capitato **subito**, alla seconda generazione.

> *«Quando la KB è grande questa cosa di fare autoscan non serve, i buchi ci
> saranno: noi dobbiamo far funzionare la KB e animarla, mica compattarla. Questa
> mi sembra la solita "riempiamo tutti i buchi così non manca nulla".»*

Fertile **non vuol dire completa**. Su una KB grande la conoscenza inutilizzata è
illimitata e in gran parte sana — i fatti di mondo *sono* lo spazio delle
risposte, non buchi da tappare — e un censimento dell'assenza spinge esattamente
nella direzione opposta a quella di §0a: si comincia a colmare invece che a far
scorrere.

**Il criterio per riconoscere la deriva**, e va applicato a ogni generazione di
questo piano: *questa cosa aumenta il FLUSSO — quanta conoscenza entra davvero in
un turno, e quanto si tiene insieme nel tempo — o aumenta soltanto la
COPERTURA?* Se la risposta è la seconda, è la strada sbagliata anche quando i
numeri salgono.

### Il leitmotiv: **il set minimo di innesco** (F., gen435)

> *«Ti vorrei più sul fronte scientifico: trovare il set minimo di conoscenza per
> cui si inneschi un meccanismo. Tu sei sul fronte "build with blocks".»*

È la critica giusta al modo in cui questo piano stava procedendo, e diventa **il
metodo di tutte le generazioni che seguono**.

| | *build with blocks* | **fronte scientifico** |
|---|---|---|
| la domanda | «che cosa aggiungo perché funzioni?» | «qual è la conoscenza **minima** perché si accenda da solo?» |
| il metodo | si aggiunge finché il caso passa | si parte dal minimo e si cerca **la soglia** |
| la verifica | il caso passa | **per sottrazione**: si toglie finché smette |
| il risultato | un pezzo in più | **un numero**, riproducibile e falsificabile |
| il rischio | crescere per accumulo, e non sapere che cosa serviva | nessuno: anche il fallimento è un dato |

**La forma di un risultato**, e da qui in avanti ogni generazione deve produrne
uno così:

> *Il meccanismo M si innesca a partire da S — |S| = n righe, di forma A/B/C —
> e togliendo una qualunque riga di S non si innesca più.*

Non «M funziona», non «ho aggiunto k righe»: **S, con la sua cardinalità e la sua
prova di minimalità**. Un set minimo dice qualcosa *sul sistema*; un pezzo in più
fa funzionare un caso.

**Perché non è un vezzo metodologico.** Tutto il piano poggia sull'ipotesi della
massa critica (§10), che è *esattamente* un set minimo — quello del ciclo intero.
Se le generazioni intermedie non producono soglie, alla fine non ci sarà nessuna
curva da guardare: ci sarà una KB più grande e nessun modo di dire perché si è
accesa, né a partire da cosa. **Un piano che accumula non può misurare
l'accensione.**

**Il protocollo, uguale per ogni generazione:**

1. si sceglie un meccanismo che *non* si innesca oggi, e si scrive il segnale
   osservabile che dirà che si è acceso;
2. si costruisce il **candidato minimo** — non quello comodo: quello piccolo;
3. si prova. Se non si accende, si aggiunge **una riga sola** e si riprova. Il
   numero di giri è già un dato;
4. acceso, si **toglie ogni riga a turno**: quella la cui rimozione non spegne
   niente non faceva parte del set, e va tolta per sempre;
5. si registra `S`, la sua cardinalità, e la forma di ogni riga (§7). Le forme
   che ricorrono attraverso meccanismi diversi sono il vero raccolto.

**E il criterio di anti-deriva** che ne segue, da applicare a ogni proposta —
compresa una mia: *stai cercando una soglia o stai aggiungendo un pezzo?* Le tre
generazioni fatte finora (433, 434, 435) erano costruzione, non ricerca: hanno
lasciato strumenti utili e **nessun numero**. È il debito da cui riparte la
prossima.

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

## 0. LA DEFINIZIONE (F., gen436) — e i quattro vincoli che porta con sé

> **«Autocorrezione è la capacità dello STATO della KB di indirizzare una lacuna
> attraverso l'unica sorgente esterna — la connessione con Wikipedia — e di
> portare ciò che manca per finire il turno.»**

Questa frase non è un riassunto del piano: è il piano. Ogni parola porta un
vincolo, e i quattro insieme escludono quasi tutto quello che si potrebbe fare.

**1. «lo stato della KB» — il soggetto non è un programma.** A indirizzare è la
*configurazione* della conoscenza: quali relazioni esistono, quali superfici le
raggiungono, quali regole sanno dire dove si sono fermate. Un tool che ispeziona
e ripara lascia la KB nello stato di prima, e la volta dopo serve ancora
(§0a, il perimetro). **Il lavoro di questo piano è trovare gli STATI, non
scrivere le procedure.**

**2. «indirizzare una lacuna» — non trovarla: ADDRESSARLA.** Una lacuna serve
solo se diventa un *indirizzo*: un posto preciso dove andare a prendere il pezzo
mancante, e la forma in cui riportarlo. *«Non so»* non è un indirizzo. *«Mi manca
il valore di `R` per `X`, e `X` sta alla pagina `P`»* lo è. Fra le due c'è tutto
il lavoro.

**3. «l'unica sorgente esterna» — una, e dichiarata.** Wikipedia (locale, o
scaricata con consenso esplicito). Non l'utente — quello è addestramento
supervisionato, un'altra cosa. Non l'invenzione — quella è il misclaim. Una
sorgente sola rende il ciclo **verificabile**: ogni riga che entra ha una pagina
da citare, e chi non ce l'ha non entra.

**3b. E della sorgente non si conserva NIENTE (F., gen436).** La pagina non si
scarica su disco, non si mette in cache, non si archivia: **si legge**. Ciò che
resta è quello che parrot0 ha **imparato** leggendola — fatti nella KB, con la
loro provenienza — e non un ritaglio di enciclopedia in un file `.md`. Un file
del genere non è interrogabile, non è revocabile, non è collegato al resto: è
conoscenza *archiviata*, che è il contrario di conoscenza *appresa*.

> **Deve restare uno che, non sapendo, studia e poi risponde** — e il «poi
> risponde» dev'essere un **effetto dello stato della KB e dell'inferenza**, non
> un ramo di C che sa già che cosa fare con quel testo.

Da cui due conseguenze operative, e la seconda è un vincolo di progetto:

- la prosa arriva **in memoria** e passa dal percorso di apprendimento che già
  esiste — lo stesso che legge un testo incollato da una persona. Non c'è un
  «lettore di Wikipedia»: c'è **il lettore**;
- **niente C nuovo per il ciclo.** Se per far funzionare il giro servisse un ramo
  di codice che riconosce la situazione, allora non sarebbe lo stato della KB a
  indirizzare — sarebbe il C, e saremmo tornati al punto 1.

**4. «per finire il turno» — lo scopo non è la KB, è IL TURNO.** Non si colma per
avere una KB migliore: si colma **per poter finire di rispondere**. Da cui il
criterio di promozione più stretto e più semplice che ci sia:

> **La conoscenza portata si tiene se il turno finisce. Se il turno non finisce,
> si butta.**

La crescita della KB è un **effetto collaterale** di turni completati. Detta
così, sparisce da sola la tentazione della completezza: nessuno colma una lacuna
che nessun turno ha aperto, perché non ci sarebbe niente da finire.

### Che cosa questa definizione rende falsificabile

> Esiste uno **stato della KB** in cui un turno che si arresta produce un
> **indirizzo**, l'indirizzo produce una **pagina**, la pagina produce la
> **conoscenza mancante**, e il turno **finisce** — senza che nessuno intervenga.

E la misura è il turno stesso: prima non finiva, adesso finisce. Non serve un
banco per dirlo; i banchi (§0b) servono a dire che non si è rotto altro.

### Il pezzo che manca è più piccolo di quanto sembri

Quasi tutta la catena esiste già, e **nessuno l'ha mai chiusa dentro un turno**:

| pezzo della definizione | stato | dove |
|---|---|---|
| il turno si arresta, e l'arresto è tipato | **c'è** (gen434) | `turn_gap_kind/2` |
| la connessione con Wikipedia | **c'è** | `page_prose()`, `wiki_fetch_topic()` |
| la prosa diventa fatti | **c'è** | `extract_frame/2` e i lettori |
| il turno si ripone | **c'è** | il ricovero della riparazione |
| **l'INDIRIZZO** — dall'arresto alla pagina | **manca** | *questo piano* |
| **la ripresa** — la conoscenza torna e il turno finisce | **manca** | *questo piano* |

`--dream` fa già tutto questo — **accanto** al turno, come esplorazione a
batch. Autocorrezione è la stessa catena **dentro** il turno, guidata da un
indirizzo invece che da una lista di argomenti. È la differenza fra leggere
l'enciclopedia e andare a cercare la voce che ti serve adesso.

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

## 2. Il ciclo, dentro il turno

```
    TURNO
      │
      │  l'inferenza prova a comporre e SI ARRESTA
      ▼
  (1) ARRESTO          non «il turno e' fallito», ma DOVE si e' fermata
      │                e CHE COSA le mancava            → turn_arrest/3
      ▼
  (2) INDIRIZZO        lo STATO della KB trasforma il pezzo mancante in un
      │                posto dove andarlo a prendere    → address/3
      │                (se non ci riesce, il ciclo si ferma qui: e' onesto)
      ▼
  (3) SORGENTE         una pagina, e una sola sorgente  → page_prose / fetch
      │
      ▼
  (4) LETTURA          la prosa diventa fatti con i lettori che gia' esistono
      │                                                  → extract_frame/2
      ▼
  (5) RIPRESA          il turno si ripone, con la conoscenza appena arrivata
      │
      ▼
  (6) VERDETTO         il turno FINISCE?
                         si'  → la conoscenza resta, con la pagina che la cita
                         no   → si butta tutto, e resta la lacuna (indirizzata)
```

**Il punto (2) è il piano.** Gli altri cinque sono costruiti o quasi. Un arresto
che non sa diventare indirizzo non è un fallimento del ciclo: è la misura di uno
stato della KB che non basta ancora — ed è esattamente il numero che il leitmotiv
(§0c) chiede di cercare.

**Il punto (6) sostituisce il gate.** Non serve chiedersi «questa riga è utile?»:
o il turno finisce o non finisce. È l'oracolo che `autocorrezione.md` §5 cercava,
ed è gratis perché il turno è già lì.

**E il punto (1) è il perimetro**: nessuno di questi passi esiste senza un turno.
Non c'è un momento in cui parrot0 «cerca lacune»: c'è un turno che si ferma.

## 3. L'ARRESTO — come si fa a dire dove ci si è fermati

Le cinque sorgenti «a freddo» della prima stesura (asimmetria fra fratelli, frame
senza dati, entità opache, regole morte, conoscenza mai toccata) sono **fuori
perimetro**: calcolano assenze senza che nessuno abbia parlato. Restano scritte
in fondo, come ragionamento, con il verdetto accanto.

Dentro il perimetro c'è una sorgente sola — **il turno che si arresta** — e il
lavoro è renderla *loquace*. Un arresto utile dice tre cose:

| | esempio (turno reale) |
|---|---|
| **dove** si è fermato | «ho letto la richiesta, non ho il valore» |
| **che cosa** mancava | il valore di `wiki_concept` per `fotosintesi` |
| **come** si chiama ciò che manca | una *voce*, non una superficie, non una regola |

Oggi parrot0 sa dire la prima e mezza (gen434: `turn_gap_kind/2` distingue
`knowledge`, `reach`, `surface`, `blind`). Le altre due sono il lavoro di gen436.

**La verifica che un arresto è abbastanza loquace** è meccanica e non richiede
giudizio: *da questo arresto si riesce a costruire un indirizzo?* Se sì, era
abbastanza; se no, manca un pezzo di stato — e quel pezzo è il dato che si cerca.

## 4. L'INDIRIZZO — le strategie di localizzazione, tutte da provare

Qui stanno le strategie parallele, e sono varianti di **una domanda sola**: dato
ciò che manca, *dove* lo vado a prendere? Ognuna è uno stato della KB diverso, e
ognuna ha il suo set minimo.

### A1 — L'indirizzo per identità
Il termine del turno *è* il titolo della pagina. Stato minimo: nessuno.
*«tell me about photosynthesis»* → pagina `photosynthesis`.
**Da provare per primo perché costa zero**, e dice quanta parte del problema è
già risolta senza fare niente.

### A2 — L'indirizzo per traduzione
Il termine è in un'altra lingua: lo stato che serve è **una traduzione**.
*«raccontami la fotosintesi»* → `tr(photosynthesis, fotosintesi)` → pagina
`photosynthesis`. È il caso misurato a gen434, ed è interessante perché la KB
**ha già** la traduzione: l'indirizzo è derivabile *oggi*, e nessuno lo deriva.

### A3 — L'indirizzo per alias
Il termine è un altro nome della stessa cosa: `also_known_as/2`, i redirect.
Stato: una relazione di alias, che la KB ha e che nessuno usa per localizzare.

### A4 — L'indirizzo per relazione
Non manca una voce ma un **valore**: `answer_frame(Cue, Pred)` dice quale
relazione la domanda chiedeva, e l'indirizzo diventa *(pagina del soggetto,
relazione da estrarre)*. È la forma più preziosa perché **la domanda è già il
fatto mancante con un buco al posto del valore**.

### A5 — L'indirizzo per categoria
Non si sa quale pagina, ma si sa il **tipo**: «un uccello che non vola» →
pagina della categoria, e poi si cerca dentro. È l'unico che chiede una ricerca
invece di una localizzazione, ed è probabilmente il più fragile: va provato per
ultimo e con la prova di minimalità più severa.

> **Come si confrontano.** Non per eleganza: per **quanti turni finiscono**. Ogni
> strategia si misura con lo stesso numero — *quanti arresti diventano indirizzi,
> e quanti di quegli indirizzi finiscono il turno* — e le due frazioni si tengono
> separate, perché sono due difetti diversi.

## 4b. LA STRUTTURA DELLA KB che serve al ciclo

Cinque relazioni, e quattro delle cinque esistono già in qualche forma. Sono
**bersagli di progetto**: la loro cardinalità minima è ciò che gen436-439 devono
misurare.

```prolog
% (1) L'ARRESTO, reificato. Non «e' fallito»: dove, e che cosa mancava.
%     `Kind` viene da gen434; `Missing` e' la novita' — il pezzo, non il turno.
turn_arrest($Turn, $Kind, $Missing).

% (2) CHE COSA E' UN PEZZO MANCANTE. Tre specie, e nessun'altra per ora:
missing_kind(entry).            % una voce: manca cio' che una pagina dice
missing_kind(value($Relation)). % un valore: manca R($Soggetto, ?)
missing_kind(surface).          % una forma: la conoscenza c'e', non la si raggiunge

% (3) L'INDIRIZZO: dal pezzo al posto. E' QUI che sta lo stato della KB, ed e'
%     qui che le strategie A1-A5 si distinguono — ognuna e' una clausola.
address($Missing, page($Title), $Extract).

address(entry($Term), page($Term), wiki_concept) :- naf(needs_translation($Term)).
address(entry($Term), page($En), wiki_concept)   :- tr($En, $Term).
address(entry($Term), page($Alias), wiki_concept) :- also_known_as($Alias, $Term).
address(value($Rel), page($Subj), $Rel)          :- turn_topic(current_turn, $Subj).

% (4) LA SORGENTE, dichiarata e unica. Che sia una sola e' conoscenza, non una
%     costante del C: il giorno che se ne aggiunge un'altra si vede in un diff.
external_source(wikipedia).
source_reach(wikipedia, local).      % una copia locale, gia' presente
source_reach(wikipedia, fetch).      % o scaricata, con consenso esplicito

% (5) LA PROVENIENZA, che e' la condizione perche' una riga possa entrare.
learned_from($Row, page($Title), $Turn).
```

E la regola che tiene insieme il tutto, che è quella che oggi non esiste:

```prolog
% Un turno arrestato che sa dove andare. Se questa fallisce, il ciclo non parte —
% e il perche' e' il dato: manca lo stato, non la volonta'.
turn_addressable($Turn) :- turn_arrest($Turn, $Kind, $Missing), address($Missing, $Where, $What).
```

## 5. Il verdetto — il turno come unico gate

Una riga portata da fuori attraversa **una domanda sola**:

> **il turno che si era fermato, adesso finisce?**

Se sì, resta — con `learned_from(Riga, page(P), Turno)`, e in quarantena
(`KB_INDUCED`, revocabile in blocco). Se no, si butta **tutta**: la lacuna resta
aperta, ma adesso è aperta *con un indirizzo che non ha funzionato*, che è un
dato migliore di prima.

I banchi (`make test`, `hundred`, `measure`) non sono il gate: sono il
**controllo di non-regressione** che si fa dopo, e servono a una domanda diversa —
non «questa riga serve» ma «questa riga ha rotto qualcosa».

Tre note che il ciclo impone, e che valgono come vincoli di progetto:

- **la quarantena non è un ripostiglio.** Una riga che ha finito un turno e non
  ne finisce mai un altro va tolta: il suo posto è la pagina da cui viene;
- **una pagina letta non è conoscenza acquisita.** Entra ciò che è servito a
  finire *quel* turno, non tutto ciò che la pagina diceva — altrimenti si è
  tornati a `--dream`, cioè accanto al turno;
- **se l'indirizzo non si costruisce, il ciclo si ferma e lo dice.** Non c'è un
  ripiego che «cerca comunque»: cercare senza indirizzo è il defrag.

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

## 7. La mappa: pezzo mancante → indirizzo → forma della riga

| pezzo mancante | strategia di indirizzo | che cosa torna | forma della riga |
|---|---|---|---|
| una **voce** (`entry`) | A1 identità · A2 traduzione · A3 alias | la definizione della pagina | **B** — `wiki_concept/3` |
| un **valore** (`value(R)`) | A4 relazione | la frase che porta il valore | **B** — `R(Soggetto, Valore)` |
| una **forma** (`surface`) | *nessuna*: la sorgente esterna non ce l'ha | — | **A** — e **non è di questo ciclo** |

**La terza riga è la più istruttiva**, e va tenuta in vista: Wikipedia porta
*fatti*, non *modi di chiedere*. Un arresto di specie `surface` — «il fatto ce
l'ho e non ci arrivo», il caso della fotosintesi in italiano — **non si chiude
con la sorgente esterna**: si chiude con una forma, che è conoscenza sulla
lingua e non sul mondo.

Il che dà una previsione netta, e falsificabile in un giro di campagna:

> **La frazione di turni che l'autocorrezione può finire è limitata dalla
> frazione di arresti di specie `entry` e `value`.** Se i corpora sono dominati
> da arresti di specie `surface`, il ciclo con Wikipedia chiuderà pochi turni **e
> non sarà colpa sua.**

Ed è anche il legame con la colla (§6): le superfici si imparano parlando o
leggendo *come le cose si dicono*, i fatti si prendono dalla sorgente. Due
canali, due specie di lacuna, e confonderli è il modo più rapido di concludere
che «non funziona».

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

## 9. Le generazioni, come SOGLIE (non come pezzi)

Ogni riga produce **un numero**, nella forma del leitmotiv (§0c): *il meccanismo
si innesca a partire da `S`, |S| = n, e togliendo una riga di `S` non si innesca
più.* Se una generazione finisce senza numero, non è finita.

| gen | la soglia da trovare | il numero che deve uscire |
|---|---|---|
| **436** | **l'arresto sa dire che cosa manca** — non solo di che specie è | su N turni arrestati, quanti producono un `Missing` utilizzabile |
| **437** | **il `Missing` diventa un indirizzo** con la strategia più economica (A1/A2) | `|S|` per la prima strategia che accende, e la frazione di arresti indirizzati |
| **438** | **l'indirizzo porta la pagina e il turno RIPRENDE** | quanti turni indirizzati finiscono davvero |
| **439** | **la minimalità**: si toglie da `S` finché si spegne | `|S|` finale, e l'elenco delle righe che *non* servivano |
| **440** | **la generalizzazione**: lo stesso `S` accende una seconda specie di arresto, o una seconda lingua? | quante specie/lingue accende lo stesso `S` |
| **441** | **R** (§10), misurato su turni veri | la curva |

**Perché quest'ordine.** 436 e 437 sono i due punti che non esistono (§0); 438
chiude l'anello e dà il primo turno finito da solo — **è il risultato**; 439 lo
trasforma da aneddoto in misura; 440 dice se è una soglia o una coincidenza; 441
guarda se si autosostiene.

**Il gate di ogni generazione è lo stesso**: `make test` verde, e nessun banco
sceso. Ma il *risultato* non è il banco: è `S`.

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

## 12. La strada, generazione per generazione

Otto generazioni, una per fase. Per ognuna: **che cosa costruisce**, la
**conoscenza** che la esprime (gli snippet sono nel dialetto reale — variabili
`$X`, liste `cons/nil`, `naf/1`, `findall/3`, arità ≤ 4, corpo ≤ 8 goal),
**l'arco riparativo** con un caso che si può seguire a mano, e il **cricchetto**
che la chiude.

> Gli snippet sono **bersagli di progetto**, non codice già in KB: si leggono
> come si legge una dimostrazione — se il ragionamento non regge sulla carta, non
> reggerà nemmeno compilato. Ogni esempio è però tratto da un **caso misurato**
> di questa settimana, quindi la verifica è possibile: si sa già che cosa
> rispondeva parrot0 prima e dopo.

---

### gen434 — La lacuna smette di essere un messaggio e diventa un oggetto  ✅ FATTA

> **Spedita il 20 agosto 2026.** `kb/core/gap-kinds.p0`, ratchet
> `tests/p0t/meta/gap_kinds.p0t` (9 assert, dentro `make test`), visibile da
> `/debug`. `make test` 2578 verdi, `hundred` 99/100, `measure` 48/48.
>
> Lo schema qui sotto e' quello progettato; l'implementazione lo segue con due
> differenze imparate strada facendo, ed entrambe valgono per le generazioni che
> seguono:
>
> 1. **gli esiti insoddisfacenti erano tre, non due.** «That looks like a X
>    problem» — il registro riconosciuto — era l'esito piu' informativo e l'unico
>    che non lasciava traccia, perche' nessuna riga lo dichiarava insoddisfacente;
> 2. **un muro cieco puo' essere un `reach`.** «raccontami la fotosintesi» mura
>    mentre «tell me about photosynthesis» risponde: il fatto c'e', raggiunto
>    dalla traduzione, e la strada italiana non ci arriva. Il primo criterio —
>    «esiste un fatto qualunque su una parola del turno» — tipizzava `reach`
>    anche «zorkuz mivvel taranto», perche' Taranto e' una citta' nota: **un
>    criterio che dice si' a tutto non distingue niente**, e ora si chiede se
>    esista *una voce che avrei potuto dire*.

**Costruisce:** i quattro difetti di `question-emergence.md` §9.1 come classi
dichiarate, e le regole che li derivano dall'esito del turno. Oggi
`unsatisfying_outcome/2` conosce solo `reachability`: si completa.

```prolog
% I QUATTRO MODI DI NON RISPONDERE. Il motore pubblica che cosa e' successo;
% quale LACUNA sia lo decide la KB — e una quinta specie, domani, e' una riga.
gap_kind(knowledge).   % so la forma della domanda, non ho il valore
gap_kind(reach).       % ho il valore, e non ci sono arrivato
gap_kind(surface).     % non ho riconosciuto che me lo stavi chiedendo
gap_kind(wrong).       % ho risposto, e la risposta non e' pertinente

% Un turno lascia il suo esito come fatto di sessione (gia' oggi: gap_record_as).
% Da li' le quattro regole distinguono, e la distinzione e' TUTTO il valore:
% quattro lacune diverse hanno quattro rimedi diversi e quattro sorgenti diverse.

gap(knowledge, $Subject, $Relation, value) :-
    turn_outcome(current_turn, $Outcome), unsatisfying_outcome($Outcome, $Any),
    turn_topic(current_turn, $Subject),
    answer_frame($Cue, $Relation), turn_cue_present(current_turn, $Cue),
    naf(kb_fact($Relation, cons($Subject, $Rest))).

gap(reach, $Subject, $Relation, consumer) :-
    turn_outcome(current_turn, $Outcome), unsatisfying_outcome($Outcome, $Any),
    turn_topic(current_turn, $Subject),
    kb_fact($Relation, cons($Subject, $Rest)),
    naf(turn_consumer(current_turn, $Relation)).

gap(surface, $Subject, $Register, cue) :-
    turn_outcome(current_turn, $Outcome), unsatisfying_outcome($Outcome, $Any),
    turn_register(current_turn, $Register), faculty_for($Register, $Faculty),
    naf(turn_cue_matched(current_turn, $Register)).
```

**L'arco riparativo, su un caso misurato.** Prima del gen427, `9:15` riceveva il
muro. Ma il trascodificatore pubblicava già, ogni turno, il fatto
`transcoded("9:15", time(9, 15))`: il valore c'era e nessuno lo consumava.

```
turno          9:15
esito          blind_wall                          → unsatisfying_outcome ✓
turn_topic     "9:15"
kb_fact        transcoded("9:15", time(9,15))      → il valore ESISTE
turn_consumer  (nessuno per transcoded)            → naf ✓
──────────────────────────────────────────────────────────────────────
gap(reach, "9:15", transcoded, consumer)
```

E la differenza che questa riga fa: la lacuna **non dice** «non conosco 9:15» —
dice *«ho un fatto su 9:15 che nessuno legge»*. È una frase su cui si può agire,
e il rimedio è di forma **A** (una superficie, cioè `lone_literal_say`), non di
forma B. Nel gen427 quel rimedio è stato scritto a mano; qui la lacuna lo nomina.

**Cricchetto:** i cento falliti producono un numero di lacune tipate **stabile
fra due esecuzioni**, e nessun prompt cambia `Kind` a seconda di ciò che è
successo prima (è il difetto misurato in `autocorrezione.md` §3).

---

### gen435 — L'audit a freddo  ⛔ RESPINTA: era un defrag della KB

> **Spedita come `make audit` e ridimensionata lo stesso giorno.** Vale come
> esempio di generazione che produce più conoscenza dal proprio fallimento che
> dal proprio successo: §3f per il merito, §0a per il principio che ne è uscito.
> Lo strumento resta con l'intestazione onesta — misura i **banchi**, non la KB.
> Il disegno originale, per memoria:

**Costruisce:** la sorgente 3f. Il motore aggrega le impronte di inferenza su un
corpus e pubblica `fired/1`; la KB deriva il resto.

```prolog
% Il motore pubblica solo un fatto per predicato TOCCATO. Tutto il giudizio
% e' qui, in tre regole — quindi «dormiente per disegno» e' una riga, non un ramo.
never_fired($Pred) :- declared_predicate($Pred), naf(fired($Pred)).

% Non tutto cio' che tace e' morto: qualcosa aspetta un turno che non e' arrivato.
dormant_by_design(needhelp).
dormant_by_design(capability_wall).

suspect_dead($Pred) :- never_fired($Pred), naf(dormant_by_design($Pred)).

% E la domanda che parrot0 si pone DA SOLO, senza che nessuno parli:
self_question(audit, $Pred) :- suspect_dead($Pred).
```

**L'arco riparativo, su tutti e sette i difetti del gen427-432.** Il più netto:

```
declared_predicate(currency_char)      % la riga c'era dal gen427
fired(currency_char)                   % MAI: il confronto era su un carattere
                                       % solo, e «£» ne occupa due
─────────────────────────────────────────────────────────────────────
suspect_dead(currency_char)  →  self_question(audit, currency_char)
```

La domanda che ne esce è la frase che ho scritto tre volte in tre commit senza
accorgermi che era un pattern: **un fatto che non combacia non si lamenta.** Da
qui in poi si lamenta.

**Nota di progetto, e non è un dettaglio:** questa sorgente **non propone
righe**. Produce una domanda il cui destinatario può essere il motore (un
difetto) o il revisore (conoscenza da ritirare). È l'unica sorgente che
attraversa il confine di §8.3, ed è per questo che lo attraversa **solo con le
parole**.

**Cricchetto:** l'elenco contiene almeno uno dei sette difetti noti,
riprodotto **senza** che nessuno lo indichi.

---

### gen436 — L'arresto sa dire CHE COSA manca

**Costruisce:** il terzo campo dell'arresto. Oggi `turn_gap_kind/2` dice la
*specie* (gen434); qui nasce il **pezzo**.

```prolog
% Il pezzo mancante, con la sua specie. Tre sole per ora, e la terza e' quella
% che la sorgente esterna NON puo' chiudere (§7) — dirlo e' meta' del valore.
turn_arrest($T, knowledge, entry($Term)) :-
    turn_gap_kind($T, knowledge), turn_topic($T, $Term).
turn_arrest($T, reach, surface($Term)) :-
    turn_gap_kind($T, reach), turn_topic($T, $Term).
turn_arrest($T, knowledge, value($Rel)) :-
    turn_gap_kind($T, knowledge), turn_question_relation($T, $Rel).

% E la domanda che il turno chiedeva, quando l'ha chiesta con un frame noto:
turn_question_relation($T, $Rel) :- turn_cue_form($T, answer_frame, $Cue), answer_frame($Cue, $Rel).
```

**L'arco, sul caso misurato a gen434:**

```
turno    raccontami la fotosintesi
gap_kind reach                                  (gen434: il fatto c'e', non ci arrivo)
topic    fotosintesi
─────────────────────────────────────────────────────────────────────
turn_arrest(current_turn, reach, surface(fotosintesi))
                                   └── e §7 dice: NON e' roba per Wikipedia.
                                       Serve una forma, non un fatto.
```

**Il numero:** su N turni arrestati dei banchi, quanti producono un `Missing`
utilizzabile — e come si distribuiscono fra `entry`, `value`, `surface`. Quella
distribuzione **è la previsione di §7**, misurata.

---

### gen437 — Il `Missing` diventa un INDIRIZZO

**Costruisce:** `address/3`, con la strategia più economica per prima.

```prolog
% A1 — identita': il termine E' il titolo. Costo: zero righe di stato.
address(entry($Term), page($Term), wiki_concept).

% A2 — traduzione: lo stato che serve e' UNA riga, e la KB ce l'ha gia'.
address(entry($Term), page($En), wiki_concept) :- tr($En, $Term).

% A4 — relazione: la domanda e' gia' il fatto mancante col buco al posto del valore.
address(value($Rel), page($Subj), $Rel) :- turn_topic(current_turn, $Subj).
```

**L'arco:**

```
turn_arrest(T, knowledge, entry(okapi))
address(entry(okapi), page(okapi), wiki_concept)      ← A1, senza stato
─────────────────────────────────────────────────────────────────────
turn_addressable(T)   →  il ciclo puo' partire
```

**Il numero:** `|S|` della prima strategia che accende, e la frazione di arresti
che diventano indirizzi. **Con la prova di minimalità**: si toglie A1 e si guarda
se A2 basta; si toglie `tr/2` e si guarda quanti indirizzi restano.

---

### gen438 — La pagina arriva e IL TURNO RIPRENDE

**Costruisce:** l'anello. È la generazione che produce il primo turno finito
senza che nessuno intervenga.

```prolog
% Che cosa si porta indietro, e in che forma. Non «tutta la pagina»: il pezzo.
bring($Turn, $Row) :- address($Missing, page($P), $Extract),
                      source_gives(page($P), $Extract, $Row).

% E il verdetto, che e' l'unico gate (§5).
keep($Row) :- bring($Turn, $Row), resumed($Turn, finished).
drop($Row) :- bring($Turn, $Row), resumed($Turn, stuck).
```

**L'arco, per intero, su un turno che oggi mura:**

```
> quanto pesa un okapi
  arresto     turn_arrest(T, knowledge, entry(okapi))
  indirizzo   address(entry(okapi), page(okapi), wiki_concept)      A1
  sorgente    page_prose("okapi") → prosa
  lettura     extract_frame → wiki_concept(okapi, biology, "…")
  ripresa     si ripone «quanto pesa un okapi»
  verdetto    finisce?  → si': la riga resta, con learned_from(…, page(okapi))
                        → no:  si butta, e la lacuna resta indirizzata
```

**Il numero:** quanti turni indirizzati **finiscono davvero**. Ed è qui che si
scopre la cosa che questo piano non sa ancora: *una voce d'enciclopedia basta a
finire un turno, o serve altro?* Il verdetto è meccanico e non opinabile.

---

### gen439 — La minimalità: si toglie finché si spegne

**Costruisce:** niente. **Sottrae**, ed è il primo risultato scientifico del
piano.

Protocollo (§0c): per ogni riga di `S`, la si toglie, si rifà il giro dei turni
che finivano, e si guarda se finiscono ancora. Chi non spegne niente **non era
nel set** e va tolto per sempre.

**Il numero:** `|S|` finale, e l'elenco delle righe che *sembravano* servire e
non servivano. Il secondo elenco vale quanto il primo.

---

### gen440 — La generalizzazione: è una soglia o una coincidenza?

Lo stesso `S` accende una **seconda specie** di arresto (`value` dopo `entry`) o
una **seconda lingua**? Se sì, `S` è uno stato; se no, era un caso.

**Il numero:** quante specie e quante lingue accende lo stesso `S`, senza
aggiungere righe.

---

### gen441 — R: la fertilità, misurata

Come nel disegno originale (§10), ma ora `R` ha una definizione operativa che
non richiede di censire niente: **quante lacune indirizzabili apre, in media, un
turno finito dal ciclo.** Si conta sui turni, non sulla KB.

```prolog
r_round($Round, $R) :-
    finished_by_cycle($Round, $C), gt($C, 0),
    findall($Q, addressable_opened($Round, $Q), $L), count_list($L, $N),
    is($R, div($N, $C)).
fertile($Round) :- r_round($Round, $R), ge($R, 1), bench_delta_nonneg($Round).
```

---

### La lettura d'insieme

| gen | che cosa cambia nel regime |
|---|---|
| **434** ✅ | il fallimento smette di essere un messaggio: diventa un oggetto con un tipo |
| **435** ⛔ | *respinta*: era una scansione a freddo, cioè un defrag della KB |
| **436** | l'arresto smette di dire «di che specie» e comincia a dire **che cosa** manca |
| **437** | il pezzo mancante diventa un **indirizzo**: un posto dove andare |
| **438** | il turno **riprende e finisce** — il primo turno chiuso senza nessuno |
| **439** | il set si **riduce**: da aneddoto a soglia |
| **440** | la soglia si prova altrove: è uno **stato** o era un caso? |
| **441** | la fertilità ha un numero, e l'ipotesi si può perdere |

Le prime due sono **percezione** (l'arresto che sa dirsi), 437-438 sono
**azione** (l'indirizzo e la ripresa), 439-441 sono **misura** (la soglia, la sua
generalità, la sua fertilità). Nessuna aggiunge una facoltà conversazionale:
tutte aggiungono lo **stato** in cui un turno arrestato sa finire da solo — che è
la definizione di §0, presa alla lettera.

E il debito da cui si riparte, scritto perché non si perda: **433, 434 e 435 sono
state costruzione, non ricerca.** Hanno lasciato strumenti utili e nessun numero.
Da gen436 ogni generazione consegna un `S`.

## 12. Strategia ratificata: riparazione causale per famiglie, non per prompt

Il caso reale `quanot fa 2 +3` fissa un metodo che questo piano adotta come
vincolo permanente. La superficie contiene almeno due anomalie candidate: il
refuso `quanot` e la fusione `+3`. Provare una sola correzione e vedere una
risposta migliore non basta. Le due controprove decisive sono:

```text
quanot fa 2 + 3   -> il calcolo e' raggiungibile: il refuso non e' necessario
quanto fa 2 +3    -> il muro resta: correggere il refuso non chiude il turno
```

La causa minima e' quindi la segmentazione contestuale del token, non una voce
`quanot -> quanto`. Promuovere anche il refuso avrebbe aumentato `S` senza
aumentarne il potere e avrebbe violato la minimalita' di gen439.

Da questo testimone segue il protocollo generale **CADRE** (*Causal Ablation,
Declarative Repair, Exogenous transfer*):

1. costruire il reticolo delle perturbazioni indipendenti osservate nel turno;
2. ripetere lo stesso obbligo cambiando una sola coordinata per volta;
3. conservare soltanto le trasformazioni controfattualmente necessarie;
4. rappresentare la licenza della trasformazione nella KB e lasciare al C solo
   la meccanica cieca;
5. registrare superficie originale, normalizzazione consumata, classe e
   operazione, affinche' la riparazione non sia invisibile;
6. trasferire la classe su valori, operatori, cue, lingue e membri aggiunti a
   runtime; poi eseguire retrazione e reinsegnamento;
7. usare negativi vicini (segno unario, telefono, codice, unita', simboli non
   aritmetici) per misurare le collisioni.

Il gate non e' “il prompt iniziale ora passa”. Una famiglia e' accettabile solo
se una matrice preregistrata copre almeno due lingue, piu' cue, valori mai usati
nel caso docente, tutti gli operatori della classe e un nuovo membro insegnato
interamente a runtime. L'ablazione di una sola licenza deve spegnere tutte e
sole le celle dipendenti; il ripristino deve riaccenderle senza rebuild.

Le misure aggiunte sono:

```text
causal_precision = riparazioni necessarie / riparazioni proposte
family_transfer  = celle esogene chiuse / celle esogene ammesse
collision_rate   = negativi catturati / negativi eseguiti
```

Il gate richiede `causal_precision=1`, `family_transfer=1` sulla matrice
preregistrata e `collision_rate=0`. Queste uguaglianze dimostrano la famiglia
sperimentata, non autorizzano a dichiarare robustezza universale a ogni rumore.
Ogni nuova classe di malformazione deve attraversare lo stesso protocollo.

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
