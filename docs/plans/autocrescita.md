# L'autocrescita — le domande che parrot0 si fa da solo, e come diventano KB

> **Stato:** aperto a gen433 (20 agosto 2026), su richiesta di F.
> **La tesi di F., testuale:** *«parrot0 cresce solo con miglioramento
> supervisionato; invece sono convinto che possa crescere da solo, perché
> attraverso la wiki può coprire gli archi mancanti: deve accorgersi, porsi delle
> domande e produrre dall'interno un upgrade della sua KB tale da superare la
> missione — e tutto questo è guidato dalla colla linguistica e dal reasoning.»*
>
> **Che cosa aggiunge questo piano** ai cinque che lo precedono: non una facoltà
> nuova, ma **un solo ciclo** in cui le parti già costruite si chiudono ad anello,
> e un **portafoglio di strategie parallele** che corrono dentro quel ciclo e si
> misurano sullo stesso banco. Le idee non sono tutte mie: `question-emergence.md`
> §4 aveva già calcolato cinque sorgenti di lacuna, `autocorrezione.md` §13 aveva
> già spostato la riparazione dal monologo al dialogo, e `fix-patterns.md` ha
> contato che forma hanno le riparazioni vere. Qui si mettono in fila.

---

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

**Il criterio dell'intero piano**, e vale la pena scriverlo come una scommessa
falsificabile: *dopo F6, una campagna di crescita autonoma di una settimana muove
`make hundred` o `make measure` più di quanto li muova una settimana di lavoro
supervisionato.* Se non succede, la tesi di F. è sbagliata **per questo sistema**,
e va detto.

---

## 10. Come questo piano può fallire (e come ce ne accorgiamo)

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
- `docs/plans/parrot0-100-failures.md`, `docs/measured-classes.md` — i banchi che
  fanno da giudice
