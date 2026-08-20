# Autocrescita, seconda edizione

## Un protocollo sperimentale per coltivare una KB autonoma

**Stato:** piano di ricerca, 20 agosto 2026  
**Edizione:** 2  
**Unica sorgente esterna ammessa:** Wikipedia, tramite la connessione gia'
presente in parrot0.

> La domanda non e' se parrot0 sappia aggiungere righe. La domanda e':
> **partendo da una KB congelata, puo' osservare le proprie lacune, formulare
> domande, usare soltanto Wikipedia per colmarle, verificare le conseguenze e
> scegliere da solo quali aggiunte conservare, senza regressioni?**

Questa edizione separa due problemi che nella prima erano mescolati:

1. **Meccanismo:** mettere in funzione il ciclo di autocrescita: lacuna,
   domanda, candidato, prova, ablazione, gate e promozione.
2. **Coltivazione:** progettare esperimenti ripetibili che facciano crescere la
   KB in modo misurabile fino a verificare, o smentire, l'autonomia.

La crescita non e' dimostrata dal numero di fatti scaricati. E' dimostrata da un
saldo positivo su domande nuove, da candidati necessari e revocabili, e dal fatto
che il processo continui quando nessun umano aggiunge conoscenza.

## 1. Ipotesi e definizioni operative

### H1: la KB fertile chiude lacune proprie

Una KB fertile non contiene soltanto fatti. Contiene connessioni tra fatti,
superfici linguistiche, frame e procedure dichiarate. Una riga promossa deve
quindi produrre una o piu' nuove lacune che il sistema sa classificare e per cui
esiste un candidato ottenibile da Wikipedia o da una forma gia' osservata.

Definiamo:

```text
R = nuove lacune autochiudibili / lacune chiuse
```

La massa critica non e' una dimensione fissa della KB. E' uno stato in cui la
media mobile di `R` resta almeno 1 mentre i banchi di comportamento non
peggiorano.

### H2: la coltivazione mista batte la sola acquisizione di fatti

Wikipedia puo' fornire fatti e concetti, ma una KB composta solo da forma B
resta piena di conoscenza irraggiungibile. L'ipotesi e' che il trattamento
`fatti + superfici + audit + gate` produca piu' crescita utile del trattamento
`soli fatti`, a parita' di pagine e budget.

### H3: l'autonomia e' trasferimento, non memoria

Una campagna non e' autonoma se chiude soltanto le stesse domande usate per
coltivarla. La crescita deve trasferirsi a:

- entita' e pagine Wikipedia non viste durante la coltivazione;
- superfici linguistiche con ordine e sinonimi diversi;
- prompt holdout, compresi i 100 nuovi prompt di
  `docs/plans/parrot0-100-failures.md`;
- domande che emergono da connessioni create dopo la prima acquisizione.

### H4: Wikipedia e' una fonte, non un oracolo della verita'

Con una sola fonte esterna non si puo' dimostrare la verita' assoluta di un
fatto. Si puo' dimostrare soltanto che il fatto e' sostenuto da una specifica
revisione di una pagina e che non rompe i test interni. Il sistema deve dire
`source_supported`, non trasformare automaticamente la prosa di Wikipedia in
certezza universale.

### Definizione di autonomia

Una KB e' dichiarata **autonoma nel dominio sperimentato** solo se, per almeno
tre campagne consecutive e su almeno due seed iniziali, soddisfa tutte queste
condizioni:

1. nessuna modifica umana a C o a `kb/core/` durante la campagna;
2. l'unica scrittura esterna e' una pagina Wikipedia con URL, titolo e revisione;
3. ogni riga promossa ha provenienza, motivo, prova e risultato dell'ablazione;
4. il punteggio del holdout non diminuisce e almeno un caso nuovo viene chiuso;
5. `R >= 1` come media mobile sui giri, non come picco isolato;
6. la quarantena non accumula righe inutili o non raggiungibili oltre una soglia
   dichiarata;
7. rimuovendo l'ultima campagna, il comportamento torna al baseline misurato.

Se una condizione fallisce, il risultato e' **crescita assistita o non ancora
autonoma**, non “quasi autonomia”.

## 2. Il ciclo unico di autocrescita

Tutte le strategie condividono lo stesso ciclo. Cambia la sorgente del
candidato, non il criterio con cui viene accettato.

```text
  1. SEGNALE       fallimento, fatto non consumato o lacuna calcolata
          |
  2. DOMANDA       gap(Kind, Subject, Relation, Position)
          |
  3. CANDIDATO     forma A, B o C con una fonte
          |
  4. PROVA         asserzione in sessione + ripetizione dello stesso turno
          |
  5. ABLAZIONE     rimozione + ripetizione; la riga deve essere necessaria
          |
  6. GATE          test, holdout e saldo non negativo
          |
  7. PROMOZIONE    quarantena KB_INDUCED -> KB ufficiale, revocabile
          |
  8. AUDIT         nuove lacune, lacune morte, R e deriva
```

### 2.1 Segnale

Il motore pubblica l'esito del turno. La KB deriva il tipo di lacuna, senza
mettere nel C una lista di parole:

| `Kind` | Significato | Prima domanda |
|---|---|---|
| `knowledge` | manca un valore o un concetto | quale fatto citabile manca? |
| `reach` | il valore esiste ma nessun consumer lo usa | quale superficie o ponte non lo raggiunge? |
| `surface` | la facolta' esiste ma la forma non e' riconosciuta | quale forma espressiva ricorrente la attiva? |
| `wrong` | c'e' una risposta ma non affronta il compito | quale vincolo o slot e' stato perso? |
| `dead_rule` | una regola dichiarata non viene mai attivata | e' conoscenza dormiente, dialetto o difetto del motore? |

`dead_rule` e' una domanda di audit, non un permesso di modificare il motore.

### 2.2 Candidato

Sono ammesse solo tre forme automatiche:

- **A:** cue, `intent_phrase`, `extract_frame`, forma di domanda o superficie
  osservata;
- **B:** fatto, concetto, alias o frame estratto da Wikipedia;
- **C:** appartenenza a una classe KB, con token osservato nel turno.

La **forma D** e' testo nuovo inventato per sembrare una risposta. Non ha
generatore automatico. La **forma E** e' una nuova procedura, per esempio un
algoritmo numerico o una regola di pianificazione: puo' essere proposta dal
sistema come lacuna del motore, ma richiede modifica generale supervisionata e
non entra nella campagna autonoma Wikipedia-only.

### 2.3 Prova e ablation

Ogni candidato e' un record, non una riga anonima:

```text
candidate(Row, Gap, Source, Turn, Generation)
```

La prova deve usare un cervello nuovo con lo stesso snapshot iniziale e la sola
riga candidata aggiunta in sessione. L'ablazione deve ripetere:

1. il prompt che ha generato la lacuna;
2. almeno tre varianti della stessa classe;
3. un controllo negativo in cui la riga non dovrebbe attivarsi;
4. il prompt senza la riga candidata.

Se il prompt passa senza la riga, la riga viene scartata e nasce una nuova
domanda di **misdiagnosis**. Anche un candidato scartato e' informazione utile:
indica dove il sistema stava guardando nel posto sbagliato.

## 3. Le strategie da mettere in coltivazione

Le strategie non vanno eseguite in sequenza scegliendo quella che sembra
promettente. Vanno eseguite come trattamenti comparabili, con lo stesso seed,
budget di pagine e holdout.

### S1: acquisizione del fatto da Wikipedia

**Input:** entita' opache, frame senza valore, asimmetrie tra fratelli.  
**Output:** forma B con provenienza Wikipedia.

Procedura:

1. la lacuna produce un soggetto e una relazione, non una parola casuale;
2. il resolver cerca una pagina Wikipedia candidata;
3. il fetch salva pagina, URL, titolo, revisione, data, lingua e hash del testo;
4. l'estrattore produce fatti solo da frame dichiarati e verificabili;
5. i fatti restano in `KB_INDUCED` fino a prova e ablazione.

S1 non deve scaricare pagine per ogni parola del prompt. La domanda deve essere
generata dallo spazio negativo della KB e la pagina deve essere pertinente alla
lacuna. Una pagina di disambiguazione, una relativa inghiottita in un atom o un
fatto senza soggetto sono rifiutati.

### S2: acquisizione della superficie

**Input:** una facolta' esiste, ma nessuna forma raggiunge il turno.  
**Output:** forma A, raccolta da ricorrenze o da una variante del turno.

Wikipedia serve anche come corpus di forme: “known as”, “located in”, “in
contrast to”, “such as”, marcatori temporali e costruzioni di definizione. La
forma entra solo se:

- ricorre sopra una soglia nel corpus selezionato;
- le entita' ai suoi lati hanno gia' relazioni compatibili;
- l'ablazione mostra che la forma chiude il caso senza collisioni;
- il pattern conserva gli span e non introduce vocabolario nel C.

La frequenza da sola non basta: una forma frequente ma ambigua e' un candidato
negativo, non una scoperta.

### S3: apprendimento di classe

**Input:** un token viene usato come connettore, quantificatore, marcatore,
operatore o formato ma non appartiene alla classe KB.  
**Output:** forma C, insegnabile e revocabile.

Il token e' proposto dalla traccia del turno, mai da una lista hardcoded. La
prova obbligatoria e' doppia: prima il turno fallisce, dopo `assert` passa,
dopo `retract` torna a fallire. Un controllo negativo deve dimostrare che il
token non ha acquisito un significato globale non richiesto.

### S4: audit a freddo

**Input:** impronte aggregate di `hundred`, `measure`, `make test` e campagne
Wikipedia.  
**Output:** domanda su di se', nessun candidato automatico quando la lacuna e'
procedurale.

L'audit distingue:

- predicato mai attivato ma legittimamente dormiente;
- regola morta perche' manca un produttore;
- dialetto privato non collegato a una relazione generale;
- procedura mancante che richiede lavoro sul motore;
- conoscenza viva ma irraggiungibile per mancanza di superficie.

S4 e' l'unica strategia che puo' far crescere direttamente la capacita' di
decidere **che cosa non imparare**. Non e' autorizzata a riscrivere C.

### S5: dialogo come controllo positivo

S5 riceve una forma dall'utente e la usa nel turno successivo. E' crescita
assistita, non autocrescita, ma serve come controllo superiore: se S1-S4 non
raggiungono almeno la resa di S5 sulla stessa classe, il problema e' nel ciclo
autonomo e non nella facolta' che si sta misurando.

## 4. Wikipedia come ambiente di coltivazione

### 4.1 Confini della sorgente

Durante una campagna autonoma sono vietati:

- interventi umani nella KB, nel C o nei template;
- LLM o giudici generativi usati per inventare candidati;
- fonti diverse da Wikipedia;
- ricerca libera non registrata;
- promozione di una riga soltanto perche' produce testo.

Sono ammessi:

- API o dump Wikipedia gia' disponibili al progetto;
- parser ed estrattori deterministici dichiarati nella KB o nel motore generale;
- il corpus di prompt congelato e i suoi oracoli meccanici;
- le tracce di provenienza, gli hash e i risultati dei test.

La connessione con Wikipedia e' una sorgente di dati, non una scorciatoia per
delegare a un servizio esterno la decisione di successo.

### 4.2 Colture, non un download continuo

Una coltura e' un esperimento con:

```text
seed_KB + seed_prompt + budget + politica_di_acquisizione + durata
```

Ogni coltura ha tre insiemi di pagine:

| Insieme | Uso | Deve essere visibile durante la crescita? |
|---|---|---:|
| `train-pages` | candidate e forme da poter acquisire | si' |
| `validation-pages` | scelta della politica e tuning delle soglie | solo tra campagne |
| `test-pages` | misura finale di trasferimento | no |

Il test-page set viene congelato con hash prima della campagna. Il sistema puo'
visitare una pagina solo se la domanda tipata la seleziona; non puo' pre-caricare
l'intera Wikipedia. Questo impedisce che “autonomia” significhi memorizzazione
del corpus.

### 4.3 Unita' di coltivazione

Il ciclo non cresce per numero arbitrario di pagine. Cresce per **giri**:

1. calcola le lacune dalla KB e dal banco;
2. sceglie al massimo `B` domande tipate con una politica dichiarata;
3. interroga Wikipedia solo per quelle domande;
4. produce candidati in quarantena;
5. prova e abla ogni candidato;
6. promuove soltanto il sottoinsieme che supera il gate;
7. ricostruisce lo spazio negativo sulla KB risultante.

`B`, timeout, lingua, numero massimo di pagine e soglia di ricorrenza sono
parametri registrati e non possono cambiare a meta' campagna per migliorare il
risultato.

## 5. Disegno sperimentale

### 5.1 Baseline e trattamenti

Per ogni seed iniziale si eseguono almeno questi trattamenti:

| Trattamento | Wikipedia | Strategie | Scopo |
|---|---:|---|---|
| **C0** controllo congelato | no | nessuna | misura deriva e variabilita' del banco |
| **C1** fatti | si | S1 | misura il contributo della conoscenza B |
| **C2** fatti + superfici | si | S1+S2 | misura il rapporto fatti/superfici |
| **C3** ciclo completo | si | S1+S2+S3+S4 | misura l'autocrescita candidata |
| **C4** controllo assistito | no | S5 | upper bound di crescita con aiuto umano |

C0 e C4 non sono decorativi. C0 controlla che un miglioramento non sia rumore
di sessione; C4 mostra quanto costa ancora l'intervento umano. C3 deve battere
C1 e avvicinarsi a C4 senza usare input umano.

L'ordine dei trattamenti e' randomizzato per seed e ogni trattamento parte da
uno snapshot pulito. Non si riusa una quarantena fra condizioni.

### 5.2 Seed e replicazione

Un singolo seed puo' produrre una tasca fortunata. Si usano almeno:

- tre snapshot di KB iniziale con stessa versione del motore;
- tre ordini diversi delle domande;
- almeno cinque giri per trattamento;
- una seconda esecuzione indipendente dopo il reset completo della sessione.

Si riportano mediana, intervallo e distribuzione dei risultati, non il run
migliore. Un timeout e' un esito sperimentale e non va nascosto aumentando il
budget senza dichiararlo.

### 5.3 Controlli anti-leakage

Prima di ogni campagna si registrano:

- hash dello snapshot iniziale;
- lista degli URL e revisioni di `train-pages`;
- hash dei prompt train, validation e test;
- hash delle pagine che la campagna ha realmente letto;
- versione del parser e del motore;
- seed casuale e budget.

Un prompt test viene escluso se la sua risposta o una parafrasi della risposta
e' stata acquisita durante la coltivazione. Se una pagina test e' stata letta
per errore, il run e' invalidato e non si seleziona soltanto il risultato
favorevole.

## 6. Metriche e registro della crescita

Ogni giro produce un record machine-readable, oltre al log umano:

```text
round, seed, treatment, gap, source_page, candidate, outcome,
ablation, provenance, bench_before, bench_after, promoted, reason
```

### Metriche primarie

| Metrica | Definizione | Cosa dimostra |
|---|---|---|
| `P` | prompt pertinenti dopo la campagna | comportamento, non testo prodotto |
| `K` | candidati con fonte sopravvissuti all'ablazione | crescita reale e necessaria |
| `N` | nuove lacune tipate aperte dalla crescita | fertilita' della KB |
| `R` | `N / lacune_chiuse` per giro | autosostentamento |
| `T` | performance sul test-pages/holdout | trasferimento fuori dal train |
| `D` | righe promosse ma mai raggiunte | conoscenza morta |

### Metriche di sicurezza

- **precisione di promozione:** righe promosse che passano il loro controllo
  negativo;
- **regression delta:** variazione su `make test`, `make hundred` e `measure`;
- **contaminazione:** fatti di un seed o utente che compaiono in un altro;
- **deriva di dominio:** crescita su Wikipedia senza aumento del banco generale;
- **resa per pagina:** candidati utili per pagina letta;
- **costo per chiusura:** tempo e pagine per ogni lacuna chiusa;
- **superficie/fatto:** rapporto tra forme A promosse e fatti B promossi.

Una riga che aumenta `P` ma aumenta anche `D` o le regressioni non e' un
successo. Una campagna che aumenta `R` in un unico dominio ma lascia `T` fermo
e' una tasca, non massa critica.

## 7. Gate, quarantena e revoca

### Gate obbligatorio

Ogni candidato attraversa tutti i passaggi:

1. **forma:** la riga e' A, B o C; E viene segnalata, D rifiutata;
2. **fonte:** Wikipedia, URL, titolo, revisione, span e hash;
3. **prova locale:** il turno motivante passa;
4. **ablazione:** il turno e le varianti peggiorano senza la riga;
5. **controllo negativo:** la riga non cattura una classe estranea;
6. **non-regressione:** i banchi congelati non calano;
7. **raggiungibilita':** esiste almeno una superficie che porta la riga a un
   turno;
8. **revocabilita':** la riga puo' essere rimossa senza modifiche manuali.

### Quarantena

Le righe in quarantena risiedono in uno strato separato, con predicati di
provenienza e generazione. Possono essere usate per la prova ma non possono
alterare silenziosamente la KB base. La promozione ufficiale avviene solo dopo
`N` giri consecutivi senza regressioni e con almeno una variante trasferita.

La quarantena deve avere un tetto. Se cresce oltre il limite senza muovere i
banchi, il coltivatore si ferma e produce un audit: altrimenti l'autonomia si
riduce a una discarica di ipotesi.

### Revoca sperimentale

Ogni campagna deve poter essere rimossa per intero:

```text
baseline -> baseline + campaign_001 -> baseline
```

Se il reset non restituisce il baseline, c'e' contaminazione o memoria nascosta
e la campagna non e' valida.

## 8. Implementazione in fasi

Le fasi sono esperimenti chiusi, non promesse di feature indefinite.

### F0: osservabilita'

Costruire il registro dei turni, delle impronte, delle lacune tipate, delle
fonti e delle ablazioni. Il cricchetto e' la ripetibilita': due run sullo stesso
snapshot producono gli stessi tipi di lacuna e identificano gli stessi
consumatori mancanti.

### F1: audit a freddo

Calcolare `fired/1`, `never_fired/1`, `dead_rule/1` e `dormant_by_design/1` dalla
KB. L'audit deve trovare almeno un difetto noto senza che un umano lo nomini.
Non promuove fatti.

### F2: primo coltivatore Wikipedia

Implementare S1 con un budget piccolo e pagine registrate. L'estrattore deve
produrre candidati B con provenienza completa e rifiutare disambiguazioni,
span rotti e fatti non supportati dal frame dichiarato.

**Cricchetto:** almeno un'entita' opaca viene colmata, passa l'ablazione e
chiude un prompt holdout senza modifica manuale.

### F3: gate e quarantena

Separare `KB_INDUCED` dalla KB ufficiale e automatizzare prova, ablazione,
controllo negativo, reset e revoca.

**Cricchetto:** almeno un candidato inutile viene scartato e registrato come
misdiagnosis.

### F4: superfici dalla prosa

Implementare S2: contare forme ricorrenti fra entita' gia' note, proporre
`extract_frame` o cue e provarle su casi nuovi.

**Cricchetto:** una superficie raccolta da Wikipedia chiude un caso di reach o
surface e torna a fallire dopo ablazione.

### F5: coltivazione comparativa

Eseguire C0-C4 su seed randomizzati e produrre il primo rapporto con `K`, `P`,
`R`, `T`, `D`, costo e regressioni per trattamento.

**Cricchetto:** il sistema sa indicare quale strategia porta resa e quale crea
una tasca; non si sceglie una strategia dal numero grezzo di righe.

### F6: ciclo autonomo completo

Collegare S1-S4: il sistema calcola lacune, sceglie domande, visita Wikipedia,
propone, prova, abla, promuove e ricomincia senza input umano.

**Cricchetto:** tre giri consecutivi producono almeno una promozione necessaria
e il holdout non peggiora.

### F7: test di autonomia

Bloccare C, `kb/core/`, soglie e holdout. Lasciare attiva soltanto la
connessione Wikipedia e il coltivatore. Eseguire tre campagne indipendenti.

**Cricchetto:** tutte le condizioni operative di autonomia della sezione 1
sono soddisfatte, oppure l'ipotesi viene dichiarata non supportata.

## 9. Previsioni e risultati che falsificano il piano

Il piano e' scientifico solo se specifica in anticipo cosa lo smentisce:

1. **S1 cresce ma il holdout resta fermo:** i fatti non sono il collo di bottiglia
   o non sono raggiungibili; aumentare Wikipedia sarebbe una cura sbagliata.
2. **S2 produce molte collisioni:** la frequenza della prosa non basta a imparare
   superfici; alzare soglie o abbandonare la forma raccolta.
3. **S3 passa i turni ma fallisce i negativi:** il token e' un sintomo, non una
   classe; la proposta viene respinta.
4. **S4 trova migliaia di predicati dormienti:** manca la distinzione tra attesa
   legittima e conoscenza morta; nessuna riga viene ritirata automaticamente.
5. **R >= 1 ma T e P non salgono:** fuga in una tasca. L'autonomia e' falsificata
   sul banco largo anche se la KB continua a fare domande su se stessa.
6. **P sale ma il controllo negativo peggiora:** il sistema sta imparando a
   rispondere in modo troppo largo o sta generando misclaim.
7. **Il reset non ripristina il baseline:** c'e' contaminazione, persistenza non
   dichiarata o memoria nascosta; il run e' nullo.
8. **C3 non supera C1 dopo tre seed e cinque giri:** la parte procedurale della
   coltivazione non porta beneficio misurabile; la tesi H2 non e' supportata.

Nessuno di questi risultati autorizza una lista di eccezioni in C. Il risultato
negativo deve restringere l'ipotesi o cambiare il trattamento sperimentale.

## 10. Protocollo operativo di una campagna

```text
freeze(engine, kb_seed, train_pages, test_pages, prompts, parameters)
record_hashes_and_seed()

for round in budget:
    gaps = derive_gaps(kb, traces, audit)
    questions = select_bounded_questions(gaps)
    pages = wikipedia_fetch_only(questions)
    candidates = extract_or_propose_A_B_C(pages, questions)

    for candidate in candidates:
        prove(candidate)
        ablate(candidate)
        run_negative_controls(candidate)
        if gate(candidate, tests, holdout, provenance, reachability):
            quarantine(candidate)

    promote_survivors_after_stability_window()
    recompute_footprints_gaps_and_R()
    stop_on_budget_timeout_regression_or_quarantine_overflow()

report(K, P, N, R, T, D, regressions, provenance, cost)
```

Il codice sopra e' un protocollo, non una licenza per aggiungere un orchestratore
con vocabolario naturale hardcoded. Le parole, le classi e i frame restano nella
KB; il C coordina tokenizzazione, fetch, limiti, snapshot, confronto e primitive
generali.

## 11. Deliverable e criterio di chiusura

Ogni fase deve lasciare artefatti versionati:

- snapshot iniziale e finale della KB;
- manifest delle pagine Wikipedia lette con revisione e hash;
- log dei giri e delle domande tipate;
- candidati promossi, scartati e motivi;
- report di ogni ablazione e controllo negativo;
- risultati separati per seed e trattamento;
- grafico di `R` e saldo dei banchi;
- elenco delle regressioni e delle righe revocate.

La seconda edizione e' completata solo quando esiste un run riproducibile che
mostra il passaggio:

```text
KB statica -> lacune osservabili -> coltivazione Wikipedia-only
           -> promozioni necessarie -> nuove lacune autochiudibili
           -> R >= 1 con holdout stabile
```

Se il passaggio non avviene, il piano non ha fallito come documentazione: ha
prodotto una misura della distanza tra processo di autoapprendimento e regime
autonomo. Quella distanza e' il risultato scientifico da cui partire per la
terza edizione.

## Riferimenti

- [`../MANTRA.md`](../../MANTRA.md) — KB-first, ablazione e anti-impostore;
- [`../PRINCIPLES.md`](../../PRINCIPLES.md) — conoscenza nella KB e crescita
  senza phrasebook nel C;
- [`question-emergence.md`](question-emergence.md) — sorgenti dello spazio
  negativo e lacune tipate;
- [`autocorrezione.md`](../autocorrezione.md) — teoria della riparazione e
  distinzione tra muro, reach e risposta sbagliata;
- [`fix-patterns.md`](fix-patterns.md) — forme empiriche delle riparazioni;
- [`parrot0-100-failures.md`](parrot0-100-failures.md) — banco holdout e classi
  di prompt difficili;
- [`extract-knowledge-from-prose.md`](extract-knowledge-from-prose.md) — frame
  estraibili e limiti dell'acquisizione da prosa.
