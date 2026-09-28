# Dai problemi alle procedure di diagnosi e crescita

Analisi del 28 settembre 2026 sul repository a `ad58a63d`.
Fonte principale: [train-the-learning-process.md](../../plans/train-the-learning-process.md).
Questo documento propone un metodo; non avvia un lotto R1–R8 e non dichiara
nuove capacità di parrot0. Le evidenze comportamentali citate sono quelle
registrate nelle sessioni precedenti. In questa analisi non sono stati
rieseguiti i prompt né modificati il motore o la KB.

**La risposta è sì, con una condizione verificabile:** il lavoro costoso deve
essere ammortizzato su una causa condivisa, e ogni intervento deve dichiarare
prima quali altri episodi dovrebbe cambiare. Il corpus offre buoni motivi per
provare questo metodo; non dimostra ancora quanto tempo faccia risparmiare.
L'autocorrezione generale a partire dal solo prompt resta un obiettivo, non
una capacità ottenuta raggruppando i log.

## 1. Che cosa contiene davvero la collezione

L'[inventario estratto](inventory.jsonl) conserva **174 righe di segnalazione**:

| blocco | ID | righe |
|---|---|---:|
| agente situazionale | SA1–SA10 | 10 |
| tedesco | DE1–DE11 | 11 |
| presa sull'interlocutore | PR1–PR14 | 14 |
| grammatica descritta | G1–G25 | 25 |
| meccanica | M001–M100, corrispondenti a #1–#100 | 100 |
| PHP | P1–P14 | 14 |

Sono segnalazioni storiche, **non 174 bug ancora aperti, né 174 cause**.
Una riga può contenere più lezioni e più difetti. Le 33 verifiche indirette e
le 38 inverse della meccanica si sovrappongono: non si sommano come cause.
La sessione meccanica si fermava al centesimo problema, dunque non è un
campione casuale. Le correzioni successive vanno lette accanto al reperto
originario: per esempio P6 passa da rosso nella seconda passata a corretto
nel riepilogo più recente; SA2 conserva un confronto A/B ancora incompleto.

L'estrazione ha trovato **17 righe meccaniche con più colonne dell'intestazione**:
M073, M075, M081–M093, M096, M100. Il separatore `|` compare anche fra lezioni;
non è lecito ricostruire automaticamente i campi per posizione. Inoltre
diverse frasi sono troncate. L'inventario conserva la riga verbatim e lascia
`fields: null` nei 17 casi ambigui. Anche le altre righe richiedono il
transcript per diventare episodi eseguibili, specialmente quando contano
ordine delle lezioni, antecedenti o contaminazioni.

Ogni record porta fonte, riga, hash del documento, revisione di estrazione,
categoria/diagnosi dell'autore quando estraibile, e campi causali **non
misurati**. Nessuna etichetta dell'autore è promossa a causa verificata.
Le tabelle di stato successive e gli handoff restano nella fonte; il JSONL
non pretende di risolverne automaticamente la cronologia.

Rigenerazione documentale, senza avviare parrot0:

```sh
python3 docs/labs/train-learning-process-differential/extract_inventory.py
```

Le RI-001…RI-023 e il censimento storico sono ulteriori evidenze sul processo,
non altre segnalazioni da sommare alle 174.

## 2. Le lezioni generali che emergono

Queste sono **famiglie di ipotesi**, con sovrapposizioni intenzionali. Due casi
appartengono alla stessa causa soltanto quando un intervento e i suoi
contrasti sostengono quella spiegazione.

| famiglia | casi che la rendono plausibile | distinzione da rendere osservabile e insegnabile |
|---|---|---|
| Atto del turno e diritto di impegnare conoscenza | SA1, SA3, PR2/4/5/7/13, lezioni G2–G4 e G22, P11, M097 | menzionare, domandare, riferire una situazione, insegnare ed eseguire richiedono impegni diversi; la pretesa del consumer deve poggiare sulla lettura |
| Conservazione della struttura e della portata | SA4/5/8/10, P1–P4/P8/P13, M020/M027–M033/M050/M058/M062/M086/M091 | che cosa è stato letto, a quale soggetto appartiene, quali complementi e clausole restano aperti; una lettura parziale non autorizza un fatto completo |
| Accordo fra chi apprende e chi usa | SA7(a)/9, PR1/6/10, DE4/5/7/9, P5/7/14, M007/M034–M039/M044–M046/M051/M084–M085, G1/14/20 | nome, ruolo, verso, morfologia e identità della relazione devono essere condivisi; un fatto acquisito deve essere raggiungibile dal consumatore pertinente |
| Portata di una generalizzazione | contaminazione gauge/lead, DE1/6/8, G5–G9/G15–G18/G24–G25, P8/12, SA6/7, M081–M090 | proprietà della parola, ruolo di questa occorrenza, lingua, variabile e vincolo di classe sono distinzioni diverse; un esempio non autorizza una sostituzione universale |
| Versione, origine e momento d'uso | SA2, PR3/8/11, P1/9, G17/18, RI-004/006/022/023, classi dopo riavvio | una conoscenza può esistere nello strato sbagliato, essere letta troppo presto, restare nella cache o riferirsi a un antecedente non più attivo |
| Composizione e interrogazione delle procedure | M063–M080/M092–M100, G10–G13, PR3 | distinguere descrizione da esecuzione; preservare passi e dipendenze; interrogare una procedura richiede precondizioni e semantica dell'operatore |
| Integrità e costo delle meccaniche | `naf` libero nelle conversioni, `eq/ne` su atomi, virgoletta PR14, viste ricostruite, SA3/4 lenti | errore di caricamento, risultato vuoto, budget esaurito e conoscenza mancante devono produrre evidenze distinguibili |

Il punto comune più forte è **la mancata composizione fra acquisizione e
uso**. Un componente sa fare qualcosa ma il successivo non ne riceve
l'oggetto, il ruolo, la versione o l'esito. Il sintomo «Learned, poi non so»
attraversa unità, alias, verbi, classi, regole e condotta. Questo orienta la
diagnosi; non prova che tutti quei casi abbiano lo stesso fix.

Tre precedenti sostengono concretamente l'ammortamento:

1. Il riepilogo della meccanica attribuisce la riparazione della composizione
   alla distinzione fra **lezione di procedura e ordine**. Le 33 indirette
   fallite erano un motivo per controllare quel confine condiviso, non per
   scrivere 33 interpreti. Le inverse annidate conservano un residuo diverso.
2. La testa della classe composta riguarda sia strumenti meccanici sia P1
   (Xdebug/extension). La riparazione riportata attraversa quei domini.
   Ciò non autorizza a trattare ogni composto linguistico come intersezione
   delle sue parole: portata ed eccezioni restano da provare.
3. [RI-023](../reference-iterations/2026-09-24/RI-023/scheda.md) ritrova due
   difetti già incontrati: cessione decisa prima delle forme e scrittura
   nello strato non persistente. Qui il risparmio nasce anche dal rendere
   riconoscibile una ricorrenza, non soltanto dal chiudere più prompt insieme.

Esiste anche un controesempio alla generalizzazione affrettata: l'archivio
del §6.5 documenta che uniformare globalmente la tokenizzazione aveva rotto
lettori dipendenti dalla vecchia semantica. Quell'esperimento è ritirato e il
suo banco non fa parte del metodo vigente. La lezione utile è chiedere un
**contratto al confine dei consumatori**, prima di migrare tutti i siti.

## 3. Il cambio di unità di lavoro

Il §0 del piano contiene già trace, ipotesi falsificabile, transfer, ritiro e
persistenza. MANTRA #22 e #23 già chiedono di estendere il circuito e cercare
la distinzione condivisa mancante. Riscrivere questi consigli non cambierebbe
il costo. Serve renderli utilizzabili per scegliere e riusare un intervento.

L'unità proposta è una **ipotesi causale con insieme di effetti previsto**:

```text
episodi osservati → ipotesi concorrenti → sonda discriminante
                → intervento generale → effetti previsti / misurati
                → nuova frontiera degli episodi ancora aperti
```

Un caso può avere più ostacoli. Se la correzione fa passare la lezione ma la
domanda resta rossa, si registra un confine superato e il nuovo arresto; non
si dichiara né chiusura né intervento inutile. È questo aggiornamento della
frontiera, dopo ogni delta, che rende la procedura **differenziale e adattiva**.

Una matrice semplice basta per cominciare: righe = episodi, colonne =
interventi, celle = effetto previsto e poi osservato. Un albero di dipendenze
collega prerequisiti e consumatori. Le famiglie si uniscono o si dividono
quando la misura smentisce l'ipotesi iniziale. Nessuna tassonomia va difesa
contro i dati.

## 4. Manuale procedurale per il coding agent

1. **Rendere ripetibile l'episodio.** Fissare motore, KB completa `agi`, stato
   locale, preambolo, lezioni nell'ordine e criterio semantico. Per una
   contaminazione conservare anche la lezione che la precede. Controllare
   caricamento e timeout. Un vecchio rosso oggi verde diventa controllo.
2. **Trovare l'ultima evidenza corretta e la prima discordanza osservata.**
   Seguire superficie → lettura/atto → arbitraggio → modifica KB → consumo →
   risposta. L'assenza nel trace non dimostra assenza nel motore: marcare
   `unknown` dove l'osservabilità non basta, e ampliare il trace unico lì.
3. **Scrivere almeno due spiegazioni concorrenti.** Per «imparato senza
   effetto»: scrittura sbagliata, consumatore diverso, vista non invalidata,
   oppure pretesa di un altro modulo. Cercare una sonda economica i cui esiti
   cambino la prossima azione. Evitare dieci parafrasi equivalenti.
4. **Applicare una cura al confine verificato.** Prima una lezione naturale;
   poi composizione/remediation disponibile; supporto generale KB/C soltanto
   quando manca la meccanica. Specificare la nuova distinzione insegnabile e
   gli episodi che dovrebbero beneficiarne, più un contrasto che deve restare
   invariato. Un modulo legacy che pretende senza lettura richiede il
   trattamento di MANTRA #21, non una cessione ad hoc per ogni frase.
5. **Misurare il delta su più episodi.** Confrontare significato, fatti attivi,
   scelta e costi, non solo testo della risposta. I casi previsti ma ancora
   rossi identificano un'altra causa o un altro consumer. Registrare anche
   miglioramenti non previsti e regressioni; un'astensione onesta e una
   capacità acquisita sono esiti distinti.
6. **Certificare R5–R7.** Meccanismo fermo, lezioni naturali, transfer fissati
   prima, contrasto, ritiro selettivo, reinsegnamento e persistenza quando
   richiesta. Conservare controlli precedenti. La dipendenza alternativa
   dalla KB viva non si elimina per forzare un'ablazione. Se cambia il motore,
   si applica il controllo software previsto dal piano.
7. **Conservare una procedura riusabile.** Firma dell'osservazione,
   precondizioni, sonda discriminante, intervento, portata dimostrata,
   controesempi, costi e criteri di arresto. Al prossimo caso la procedura
   deve ridurre le decisioni da rifare. Se lascia aperta la stessa domanda
   diagnostica, la documentazione non ha ancora ammortizzato nulla.

### Sonde che separano le cause

| osservazione | confronto da fare | conseguenza diagnostica |
|---|---|---|
| Lezione accettata, domanda fallita | fatto/effetto presente? domanda canonica e nuova consultano lo stesso oggetto? | separa scrittura, accesso e arbitraggio; «Learned» da solo non decide |
| Verbo alla radice inefficace | stessa lezione con forma flessa, trace di entrambe | distingue licenza morfologica da lettore che non consuma la licenza |
| Semplice verde, composto rosso | clausole individuali e composto con identico contenuto | verifica confini, soggetti e portata; le clausole separate sono sonde, non sostituti del bersaglio |
| Fallisce dopo un'altra lezione | stesso episodio con e senza quella lezione, base completa identica | identifica interferenza; poi distinguere alias globale, antecedente e cache |
| Relazione appresa ma domanda inversa fallita | goal legato vs goal con slot libero, proof e budget | separa errore di verso, limite di enumerazione e mancata lettura della domanda |
| Ritiro inefficace | supporti alternativi, versione visibile, invalidazione e nuovo processo | separa persistenza da sostegni legittimi; non cancellare conoscenza estranea |
| Regola in prosa riconosciuta «già nota» | effetto nativo prima; lezione di regola diversa dopo | distingue riconoscimento di una regola presente da capacità di apprenderne una nuova |

Per le inverse non assumere che ogni procedura abbia un unico inverso:
rami e cicli possono avere zero, uno o più antecedenti. Una ricerca limitata
senza soluzione non prova impossibilità. Registrare dominio, vincoli,
limiti e stato di completezza della ricerca.

## 5. Esempio concreto: SA8 come primo esperimento

Il [§8 del piano situazionale](../../plans/train-the-smart-agent.md) localizza
un confine promettente: il frame porta già `covered(N), of(M)` e il resto
dopo la prima clausola non viene riletto. È una diagnosi registrata, da
riprodurre sulla revisione scelta per il pilota.

La lezione «x gets me home at y means the arrival time of x is y» apre una
costruzione. Sul turno con bus e treno il requisito è conservare **due tempi
legati a due soggetti**, non ottenere una frase di conferma più lunga.
Le sonde confrontano prima ciascuna clausola isolata, poi la coordinazione;
SA10 verifica il caso con soggetto condiviso. Un contrasto deve impedire di
spezzare ogni `and`: P8 e M062 coordinano oggetti, non due proposizioni
autonome; nomi composti e negazioni aggiungono altri vincoli.

La possibile espansione cognitiva è una **lettura con residui espliciti**:
conservare le interpretazioni parziali e far decidere a politiche KB come
completarle o chiedere chiarimenti. Il C offre meccaniche di span e replay;
la KB insegna condizioni, ruolo del resto e conservazione del soggetto.
Il conteggio dei token è un sensore, non una prova di copertura semantica.

Prima previsione limitata: beneficio su SA8/SA10 se il confine è davvero
comune. P8/M062 sono inizialmente contrasti e possibili estensioni successive,
**non successi promessi**. Dopo la cura va ricostruita la frontiera: eventuali
problemi di stato, anafora o risposta sociale restano distinti.

## 6. Quali metadati raccogliere e come usarli in un prompt

Un dump completo per ogni prompt sarebbe soprattutto una ripetizione della
KB. Conservare una base identificata e delta/eventi per episodio è più utile
per il confronto. Il contesto originale va mantenuto: il solo prompt finale
non riproduce PR8, la contaminazione gauge/lead o un'ellissi.

| livello | dati minimi |
|---|---|
| Identità dell'esperimento | commit, diff locale, hash binario e KB caricata, profilo, modalità, ambiente, diagnostica abilitata |
| Episodio | testo integrale, preambolo ordinato, lezioni e provenienza, dipendenze, criterio semantico, controllo positivo e negativo |
| Osservazioni | canone e passaggi, atto, span/ruoli/residui, candidati e ragioni di cessione/rifiuto, goal effettivi e risultati |
| Modifiche | fatti/regole/supporti aggiunti o ritirati, origine, contesto, versione, invalidazioni, prove delle conclusioni |
| Limiti | query/costo per fase, budget raggiunto, righe di trace perse, errori di caricamento; distinguere `false`, `unknown`, `timeout` |
| Esperimento causale | ipotesi alternative, sonda, previsione prima dell'intervento, delta reale, nuovi arresti, controlli, ritiro e persistenza |

Il repository ha già il trace unico in `src/brain.c` e
`src/brain/99-registry.c`, oltre a `/debug on`, `/debug trace`,
`/debug depth N`, `/debug pred NOME` e `/debug dump`. Quest'ultimo enumera
anche alcune viste derivate: non è una fotografia passiva o necessariamente
completa. `PARROT0_TURN_LOG` salva il trace per turno e abilita diagnostica
profonda. I costi con e senza diagnostica vanno separati; il trace ha un tetto
e dichiara righe perse. I fallimenti dei rami non eseguiti richiedono sonde,
non possono essere ricavati dal solo percorso vincente.

I numeri dell'ambiente vanno letti dal codice corrente: a questa revisione
`KB_MAX_ARGS=4`, `KB_MAX_BODY=16`; la nota in AGENTS che riporta 8 è storica.
Anche questa differenza mostra perché una diagnosi deve portare la revisione.
Lo scanner `scripts/naf-free-var-scan.py` è già disponibile per un controllo
statico mirato: non occorre riscoprire quelle trappole per ogni dominio.

**Un prompt globale può essere utile per formulare ipotesi**, contenendo:
indice completo delle segnalazioni, correzioni documentate, matrice delle
dipendenze e piccoli confronti fra trace. I dettagli integrali restano
referenziati e si aprono dove discriminano le ipotesi. Chiedere al ragionatore
per ogni proposta: casi spiegati, caso che la smentirebbe, sonda più economica,
predizione fuori dal caso iniziale, distinzione insegnabile, costo previsto.

Una grande finestra di contesto non trasforma correlazioni fra log in cause.
Il passo utile successivo è eseguire la sonda proposta e aggiornare l'ipotesi.
L'analisi aggregata e il ciclo sperimentale si alternano.

## 7. Che cosa può diventare autocorrezione di parrot0

Vanno misurati separatamente tre risultati:

| risultato | dove avviene la decisione | prova necessaria |
|---|---|---|
| Agent più veloce | il coding agent riusa evidenze e procedure diagnostiche | costo totale minore per capacità certificata, a parità di qualità |
| Più casi riparabili insegnando | parrot0 acquisisce una distinzione naturale consumata da più facoltà | stesso binario, effetto su casi diversi, contrasto e ritiro |
| Autocorrezione | parrot0 riconosce l'arresto, sceglie una mossa disponibile, ne verifica l'effetto | ciclo end-to-end senza agente che selezioni la cura al suo posto |

Il secondo è il ponte fra il primo e il terzo. Il coding agent può scoprire
una procedura diagnostica; dopo prove su cause ricorrenti, quella procedura
diventa conoscenza insegnabile a parrot0. La meccanica di esecuzione resta
generica. Non basta caricare un manuale in KB: occorrono sensori interrogabili,
operatori effettivamente eseguibili e criteri che i consumatori usino.

Esistono punti da riusare: `turn_arrest` e dipendenze in
`kb/core/arrests.p0`, candidati/replay in `assisted-learning.p0`, supporti e
ritiri, e il lavoro su **grafo di supporto e grafo delle opportunità** in
[apprendimento-assistito.md §§14.5–14.7](../../plans/apprendimento-assistito.md).
Quest'ultimo è particolarmente vicino all'idea differenziale: chi dipende
dalla conoscenza cambiata, e chi potrebbe diventare leggibile grazie ad essa.
Il risultato locale documentato non certifica copertura di ogni lettore;
va verificato cosa esponga già per il caso scelto prima di aggiungere modelli.

Il solo prompt non determina sempre la cura: può mancare una parola, una
regola, un antecedente o un fatto del mondo; letture diverse possono essere
ugualmente compatibili. In questi casi la mossa corretta è una domanda
discriminante o un'acquisizione verificabile. Autocorreggersi non autorizza
inventare premesse per rendere verde la risposta. Se manca una primitiva
generale, il sistema deve nominare il confine e l'intervento resta tecnico.

## 8. Come verificare che cambia il costo

Modello di costo proposto, non misura ottenuta:

```text
costo totale = raccolta + diagnosi delle cause + interventi generali
             + verifica degli episodi + regressioni + manutenzione
```

Se N episodi dipendono da K cause riusabili, con K molto minore di N, la parte
di diagnosi/sviluppo può spostarsi da N investimenti a K. La verifica sui N
episodi resta: il risparmio viene dal riuso e dalla selezione informata delle
sonde. Non si può dedurre una riduzione 10× dal solo conteggio delle righe.

Il primo pilota dovrebbe essere piccolo: SA8/SA10 come coppia di sviluppo,
P8/M062 come contrasti strutturali, controlli già verdi, e nuovi episodi
riservati dopo aver congelato la cura. Tutti i 174 casi qui letti sono ormai
materiale di sviluppo/regressione, non una valutazione indipendente. Se la
riproduzione smentisce il confine presunto, si registra l'esito e si cambia
ipotesi prima di scrivere la cura.

Registrare per ogni tentativo:

- minuti e turni di diagnosi, sviluppo e certificazione; token/costo dell'agente;
- costo iniziale di raccolta/strumentazione, incluso nel totale;
- episodi chiusi, famiglie causali dimostrate, confini superati senza chiusura;
- predizioni confermate/smentite sugli altri episodi e regressioni;
- quota di casi successivi gestibili con sole lezioni naturali;
- costo runtime, ritiro, interferenza, persistenza e casi ancora aperti.

Per un confronto credibile fra metodi, assegnare nuove famiglie comparabili
a una procedura ordinaria o a quella differenziale, conservando i rispettivi
stati iniziali completi e registrando i costi senza selezionare solo i successi.
Evitare che una soluzione già vista su un braccio venga contata come scoperta
veloce nell'altro. I «30 minuti a problema» sono l'ipotesi di costo riferita
dall'operatore: non sono stati rimisurati in questa analisi.

Un primo criterio operativo di utilità: una diagnosi deve prevedere e spiegare
il miglioramento di almeno un altro episodio, con contrasto preservato; il
costo medio comprensivo di raccolta e verifica deve scendere nel lotto.
Questo certifica utilità locale del metodo, non un fattore universale.
Se la raccolta cresce ma non riduce decisioni o lavoro ripetuto, va ridotta.

## 9. Prompt operativo proposto

Questo testo è una proposta d'invocazione futura, non un'istruzione già
eseguita dall'analisi:

```text
Esegui un pilota di diagnosi differenziale sul confine SA8/SA10 secondo
docs/labs/train-learning-process-differential/README.md, conservando i
criteri di certificazione di train-the-learning-process.md.

Riproduci gli episodi con la KB agi completa e il loro preambolo. Fissa
controlli e criteri semantici prima di modificare. Parti dal trace unico.
Formula ipotesi concorrenti e scegli la sonda che distingue le prossime cure.
Prima di intervenire dichiara gli effetti attesi sugli altri episodi.
Ripara una meccanica generale soltanto se una lezione naturale non basta.
Misura il delta, aggiorna gli arresti residui e conserva la procedura causale.
Certifica apprendimento, transfer, contrasto, ritiro e persistenza separando
quanto fa la patch da quanto fanno le lezioni. Conta anche costi e fallimenti.
Non eseguire il banco della prosa e non costruire un nuovo framework prima
di avere dimostrato il primo riuso diagnostico.
```
