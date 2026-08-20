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

### 3f. Conoscenza mai toccata — **provata a gen435 e ridimensionata**
*Segnale:* l'aggregato delle impronte di inferenza su un corpus dà le righe che
nessun turno ha mai attivato.
*Domanda:* «quali cose che dico di sapere non hanno mai fatto niente?»

**È stata costruita (`make audit`, gen435) e la misura ha smentito il disegno.**
Due cose, e vanno tenute perché costano meno che riscoprirle:

1. **un conteggio su un corpus non prova che una riga sia morta.** «Mai chiesta»
   e «non può combaciare» danno lo stesso identico segnale: `currency_sign` fira
   per `$` e mai per `£` **esattamente come** `stopword` fira per dieci parole e
   mai per le altre 284. La sterlina — il caso che aveva motivato la sorgente —
   **non è distinguibile** da una parola che nessuno ha pronunciato. Per provarla
   serve un controllo *statico* fra consumatore e riga (quel confronto guarda un
   carattere, quella riga ne occupa due): un'altra cosa, che vive altrove;
2. **il numero che ne esce è un censimento dell'assenza** — «13.935 fatti mai
   usati, 85%» — ed è il numero che spinge verso la completezza invece che verso
   il flusso. Vedi §0a.

**Quello che lo strumento misura davvero, ed è utile**, è un'altra cosa: *quali
meccaniche dichiarate questi banchi non esercitano mai*. È una **misura dei
banchi**, non della KB: dice dove le prove sono strette. `make audit` è stato
tenuto con quella intestazione.

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

Le otto fasi hanno una generazione ciascuna, con la conoscenza che le esprime e
un arco riparativo verificabile a mano: **§12**.

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

### gen435 — L'audit a freddo  ⚠️ FATTA, E HA SMENTITO SE STESSA

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

### gen436 — La domanda si pone da sola: lo spazio negativo si calcola

**Costruisce:** le sorgenti 3a, 3b, 3c come regole — non come scansioni in C.

```prolog
% 3c — ENTITA' OPACA: compare come argomento, mai come soggetto.
has_subject_fact($E) :- kb_fact($Pred, cons($E, $Rest)).
appears_as_object($E) :- kb_fact($Pred, cons($X, cons($E, nil))).
opaque_entity($E) :- appears_as_object($E), naf(has_subject_fact($E)).
self_question(concept, $E) :- opaque_entity($E), naf(wiki_concept($E, $D, $G)).

% 3b — FRAME DICHIARATO SENZA DATI: la domanda E' il fatto mancante, col buco
% al posto del valore. E' la sorgente piu' pulita che esista.
self_question(value($Relation), $Subject) :-
    answer_frame($Cue, $Relation), known_entity($Subject),
    naf(kb_fact($Relation, cons($Subject, $Rest))).

% 3a — ASIMMETRIA FRA FRATELLI: il profilo atteso lo dice la MAGGIORANZA,
% nessuno scrive uno schema.
sibling_profile($Class, $Relation, $N) :-
    findall($M, class_member_with($Class, $Relation, $M), $L), count_list($L, $N).
expected_of($Class, $Relation) :-
    sibling_profile($Class, $Relation, $N), class_size($Class, $S),
    is($Half, div($S, 2)), gt($N, $Half).
self_question(value($Relation), $Member) :-
    expected_of($Class, $Relation), category_member($Class, $Member),
    naf(kb_fact($Relation, cons($Member, $Rest))).
```

**L'arco riparativo, verificabile a mano.** Nella KB di oggi
`poker_hand(straight_flush)` esiste e `straight_flush` non è soggetto di nulla:

```
appears_as_object(straight_flush)      % poker_hand(straight_flush)
has_subject_fact(straight_flush)       % nessuno
──────────────────────────────────────────────────────────────
opaque_entity(straight_flush)  →  self_question(concept, straight_flush)
```

E l'asimmetria, con i numeri della KB reale: se `game_players/2` è dichiarata da
9 giochi su 15, `expected_of(game, game_players)` tiene (9 > 7), e ogni gioco
senza quella relazione diventa una domanda **tipata e già formata**.

**Cricchetto:** le tre regole producono un elenco non vuoto e **stabile** su una
KB ferma; e almeno una domanda dell'elenco corrisponde a un prompt dei cento che
oggi fallisce — cioè lo spazio negativo *predice* un fallimento osservato.

---

### gen437 — Il candidato, con la sua fonte

**Costruisce:** il ciclo S1. La domanda diventa una riga proposta, e la
provenienza è parte della proposta, non un'aggiunta successiva.

```prolog
% Un candidato e' una TERNA: la riga, la domanda che l'ha chiesta, la fonte.
% Senza fonte non entra (perimetro §8.2) — e la regola lo dice, non un commento.
candidate($Row, $Question, $Source) :-
    self_question(concept, $E), source_defines($Source, $E, $Gloss),
    concept_row($E, $Gloss, $Row), eq($Question, concept($E)).

concept_row($E, $Gloss, wiki_concept($E, $Domain, $Gloss)) :-
    source_domain($E, $Domain).

% La promozione e' un ATTO SEPARATO, e ha tre condizioni, non una.
promotable($Row) :-
    candidate($Row, $Question, $Source),
    proved($Row, $Turn), ablation_fails($Row, $Turn), bench_delta_nonneg.
```

**L'arco riparativo, dal caso del gen432 rifatto al contrario.** `epistemic
injustice` era una domanda che parrot0 capiva e non sapeva; la riga
`wiki_concept(epistemic_injustice, philosophy, "…")` l'ha chiusa. Il ciclo
rifarebbe la stessa cosa senza che nessuno la scriva:

```
self_question(concept, epistemic_injustice)          % 3c, dallo spazio negativo
source_defines(page(1873), epistemic_injustice, G)   % dal corpus, citabile
candidate(wiki_concept(epistemic_injustice, philosophy, G), …, page(1873))
proved                → «what is epistemic injustice» risponde
ablation_fails        → tolta la riga, torna a fallire   ✓ serviva
bench_delta_nonneg    → test verde, hundred +1
──────────────────────────────────────────────────────────────────────
promotable ✓  → quarantena con learned_from(Row, page(1873))
```

**Cricchetto:** K entità opache colmate **con provenienza**, `make test` verde,
saldo `hundred` ≥ 0. E la riga di quarantena si può togliere in blocco.

---

### gen438 — L'ablazione diventa il cancello

**Costruisce:** il punto (5) del ciclo. È la difesa contro il difetto che
`fix-patterns.md` ha trovato sette volte, e la sua forma è banale — il che è il
bello.

```prolog
% Una riga entra solo se la sua ASSENZA si vede. Il motore esegue i due turni;
% la KB dice che cosa significa il risultato.
ablation_fails($Row, $Turn) :- without($Row, $Turn, fail).
useless_row($Row) :- without($Row, $Turn, pass).

% E cio' che non serve non si butta in silenzio: si registra, perche' un
% candidato inutile e' una LACUNA MAL DIAGNOSTICATA, ed e' un'informazione.
self_question(misdiagnosis, $Question) :-
    candidate($Row, $Question, $Source), useless_row($Row).
```

**L'arco riparativo — il caso che insegna di più è quello che *scarta*.** Se
parrot0 propone `content_kind(receipt)` per chiudere *«explain this receipt»* e,
tolta la riga, il turno passa lo stesso, allora la lacuna non era la classe:
qualcosa d'altro stava già rispondendo. La riga si scarta e nasce
`self_question(misdiagnosis, …)` — cioè il ciclo impara **dove non guardare**.

**Cricchetto:** il numero di righe scartate dall'ablazione è **> 0**. Se è zero,
il cancello non sta misurando niente ed è decorativo.

---

### gen439 — La superficie raccolta dalla prosa

**Costruisce:** il ciclo S2, cioè la classe più numerosa (§7). Il precedente è
il gen382: le forme non si immaginano, si **contano**.

```prolog
% Una FORMA e' una sequenza di parole che ricorre fra due entita' note. Il
% motore misura le ricorrenze; la KB decide quando una ricorrenza e' una forma.
form_support($Between, $N) :-
    findall($P, corpus_between($P, $Between, $A, $B), $L), count_list($L, $N).
form_min_support(5).

proposed_frame($Pattern, $Relation) :-
    form_support($Between, $N), form_min_support($Min), ge($N, $Min),
    pair_relation($Between, $Relation),
    frame_pattern($Between, $Pattern), naf(extract_frame($Pattern, $Relation)).
```

**L'arco riparativo, con i numeri veri del gen382.** Nel corpus di 49 pagine,
«known as» compare **15 volte** fra due entità che la KB già collega con
`also_known_as/2`:

```
form_support("known as", 15)        ≥ form_min_support(5)     ✓
pair_relation("known as", also_known_as)                      % le coppie coincidono
frame_pattern("known as", "@S is known as @O")
naf(extract_frame("@S is known as @O", also_known_as))        % non c'era ancora
──────────────────────────────────────────────────────────────────────
proposed_frame("@S is known as @O", also_known_as)
```

Questa è la riga che nel gen382 ho scritto a mano **dopo** aver contato. Il
punto del ciclo è che il conteggio e la scrittura sono lo stesso atto.

**Cricchetto:** almeno una `extract_frame` raccolta dal corpus chiude un turno
che prima falliva, e sopravvive all'ablazione.

---

### gen440 — Le cinque strategie in parallelo, e il saldo per sorgente

**Costruisce:** il registro che permette di confrontarle invece di preferirle.

```prolog
% Ogni riga promossa ricorda da quale ciclo viene. Il saldo si calcola per
% sorgente: quante righe ha aggiunto, e quanto ha mosso i banchi.
yield($Source, $Added, $Delta) :-
    findall($R, promoted_by($Source, $R), $L), count_list($L, $Added),
    bench_delta($Source, $Delta).

% Una sorgente che aggiunge molto e muove poco sta lavorando in una tasca.
pocket($Source) :- yield($Source, $Added, $Delta), gt($Added, 20), le($Delta, 0).
```

**L'arco riparativo — questo gen non ripara un turno, ripara il PIANO.** Se dopo
un giro `pocket(opaque_entities)` tiene, la strategia S1 sta colmando entità
opache di un angolo marginale: il ciclo funziona e il piano no. È il fallimento
che assomiglia di più al successo (§10), e qui diventa una riga interrogabile.

**Cricchetto:** si può dire, **con i numeri**, quale sorgente ha portato più
crescita per riga aggiunta.

---

### gen441 — R: la fertilità, misurata

**Costruisce:** la curva di §10, che è la verifica dell'ipotesi di F.

```prolog
% R di un giro: quante lacune AUTOCHIUDIBILI ha aperto ogni lacuna chiusa.
% «Autochiudibile» non e' un giudizio: e' avere un generatore di candidati (§7).
self_closable($Q) :- self_question($Kind, $X), kind_has_generator($Kind).
kind_has_generator(concept).
kind_has_generator(value($Relation)).
kind_has_generator(surface).

r_round($Round, $R) :-
    closed_in($Round, $C), gt($C, 0),
    findall($Q, opened_in($Round, $Q), $L), count_list($L, $N),
    is($R, div($N, $C)).

% E la soglia, che e' OSSERVATA e non decisa.
fertile($Round) :- r_round($Round, $R), ge($R, 1), bench_delta_nonneg($Round).
```

**L'arco riparativo — la domanda finale che parrot0 si pone su di sé:**

```
closed_in(7, 12)                       % dodici lacune chiuse nel giro 7
opened_in(7, …) × 14                   % quattordici nuove, tutte con generatore
r_round(7, 1.16)                       % R ≥ 1
bench_delta_nonneg(7)                  % e i banchi non sono scesi
──────────────────────────────────────────────────────────────────────
fertile(7)   → la KB ha attraversato la soglia in quel giro
```

**Cricchetto — ed è il criterio dell'intero piano:** la curva si può disegnare, e
si vede se la media mobile sale verso 1. Se resta sotto in ogni configurazione
delle cinque strategie, **l'ipotesi della massa critica è falsa per questo
sistema**, e il piano è servito a mostrarlo con dei numeri.

---

### La lettura d'insieme

| gen | che cosa cambia nel regime |
|---|---|
| **434** | il fallimento smette di essere un messaggio: diventa un oggetto con un tipo |
| **435** | la conoscenza morta comincia a lamentarsi |
| **436** | lo spazio negativo si calcola, quindi le domande esistono prima dei turni |
| **437** | una domanda diventa una riga, con la sua fonte |
| **438** | una riga inutile non entra, e il suo rifiuto insegna |
| **439** | la prosa smette di dare solo fatti e comincia a dare **forme** |
| **440** | le strategie si confrontano invece di essere preferite |
| **441** | la fertilità ha un numero, e l'ipotesi si può perdere |

Le prime tre sono **percezione** (vedere le proprie lacune), le tre di mezzo sono
**azione** (colmarle senza mentire), le ultime due sono **misura** (sapere se sta
funzionando). Nessuna delle otto aggiunge una facoltà conversazionale: tutte
aggiungono **conoscenza sulla propria conoscenza**, che è la definizione più
stretta di ciò che questo piano vuole.

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
