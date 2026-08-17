# Report di interlocuzione con parrot0

**Data:** 2026-08-17  
**Sistema osservato:** `gen396-universal-answer-plan@5c1b9b2`  
**Metodo di avvio:** tre sessioni eseguite tramite `make chat`, con input
passati sulla stdin e chiusura tramite `/quit`.

## Chi sono

Sono un agente software che valuta parrot0 dall'esterno. Non considero vera
un'affermazione del sistema solo perché il sistema la dichiara: seguo il
principio del progetto, secondo cui l'introspezione propone e il comportamento
osservabile decide. Ho quindi usato prompt di capacità diverse e ho registrato
sia risposte corrette sia fallimenti.

## Strategia di analisi

Ho applicato cinque sonde:

1. **Identità e trasparenza:** chi sei, cosa sai fare, quali sono i limiti.
2. **Competenza di base:** fatti noti, aritmetica, confronto numerico,
   traduzione e risposta in formato vincolato.
3. **Memoria conversazionale:** insegnare un nome o una preferenza e
   richiamarla nel turno successivo.
4. **KB-first/runtime growth:** introdurre predicati inventati (`puppo`,
   `zorf`) durante la conversazione e verificare che modifichino l'analisi senza
   ricompilare.
5. **Robustezza e composizione:** cambiare lingua, formulazione e combinare
   asserzioni, domande e inferenza.

Ho dato più peso alla correttezza e alla capacità di apprendere che alla
fluidità stilistica. Un muro onesto vale più di una risposta fattuale inventata.

## Interlocuzioni osservate

### Sessione 1: italiano, identità e memoria

Risultati rilevanti:

- `Ciao, chi sei?` -> `Mi chiamo parrot0.`
- `Che cosa sai fare?` -> elenco coerente con il capability ledger reale,
  includendo KB, inferenza limitata, strumenti e acquisizione.
- `Quanto fa 17 per 6?` -> `102`.
- `Qual è la capitale della Francia?` -> `Parigi`.
- `Ricorda che il mio nome è Francesco.` -> `Got it: your name is Francesco.`
- `Come mi chiamo?` -> `Non capisco ancora.`
- `Confronta 12 e 19.` -> `Mmh, questo per ora va un po' oltre le mie capacità.`
- Una richiesta di spiegazione in una frase ha prodotto una frase narrativa,
  non una spiegazione.
- L'insegnamento naturale `quando dico "salve brillante" sto salutando` non è
  stato acquisito; il cue successivo non è stato riconosciuto.

Questa sessione mostra una buona superficie di identità e alcuni fatti semplici,
ma anche una discontinuità fra confermare un insegnamento e saperlo riusare.

### Sessione 2: inglese, capacità e composizione

Risultati rilevanti:

- `Hello, what are you?` -> `I am parrot0.`
- `What can you do?` -> capability ledger dettagliato e prudente.
- `What is 2 + 2?` -> `4`; `What is 9 times 8?` -> `72`.
- `Which is larger, 41 or 14?` -> `Skin.`: risposta non pertinente e non
  segnalata come incerta.
- `Who wrote Hamlet?` -> `William shakespeare.`; `capital of Australia?` ->
  `Canberra.`
- Dopo `Remember that my favorite color is green.`, la domanda sulla
  preferenza ha ricevuto `I don't know what your favorite is called.`
- Sui limiti ha dato una risposta più utile, distinguendo capacità dimostrate
  da capacità ancora immature.
- `Give me three facts about water.` -> `I don't understand that yet.`; il
  vincolo quantitativo non è stato rispettato perché la richiesta è stata
  respinta.

Il contrasto principale è fra buona copertura di fatti codificati e debolezza
nel ragionamento/composizione generativa.

### Sessione 3: prova KB-first

Questa sessione ha riprodotto il pattern di `tests/p0t/language/taught_lexicon.p0t`:

- `puppo dog is a mammal` -> fatto appreso.
- `puppo is a universal_quantifier` -> predicato appreso.
- La stessa forma `puppo dog is a mammal` è diventata una regola:
  `mammal(X) :- dog(X)`.
- Dopo `rex is a dog`, `is rex a mammal?` -> `Yes.`
- `zorf is a definite_article` e `leak is zorf trigger of outage` hanno
  prodotto `trigger(leak, outage)`.
- `who is the trigger of outage?` -> `leak`.
- Dopo la dimenticanza del quantificatore, `puppo cat is a feline` ha comunque
  prodotto una nuova regola: la sessione ha mostrato acquisizione runtime, ma
  non una verifica completa dell'ablazione attesa per quel predicato.

Questa è l'evidenza più forte raccolta: una parola inventata entra nella KB e
attiva una classe grammaticale senza modifica del binario. È anche emerso un
segnale da indagare: l'inferenza ha stampato regole indotte ulteriori e
apparentemente non richieste (`chess_rank`, `philosopher`, ecc.), quindi la
provenienza delle derivazioni merita una sonda dedicata.

## Giudizio

Parrot0 è un esperimento serio e già dimostra qualcosa di non banale: non è
solo un risponditore di frasi, perché può rappresentare fatti, apprendere
predicati durante il dialogo e usarli per derivare nuove regole. Il
self-model è particolarmente convincente quando il capability ledger dichiara
anche ciò che non è affidabile.

Non lo giudico ancora un interlocutore generale. La memoria di semplici fatti
personali non è stabile, il confronto numerico fallisce in modo pericoloso
rispondendo con un'entità estranea, i vincoli di formato non sono affidabili e
le forme linguistiche naturali non hanno la stessa potenza delle forme
strutturate della KB. Il difetto prioritario non è la loquacità: è la
separazione insufficiente fra risposta corretta, muro e risposta fuori dominio.

**Voto preliminare: 6,6/10.**

Il voto premia il valore dell'architettura e la dimostrazione KB-first, non la
qualità di un assistente pronto all'uso.

## Seconda batteria: oltre 120 nuove domande

Per ridurre il rischio di giudicare il sistema su pochi esempi favorevoli ho
eseguito tre ulteriori sessioni, sempre con `make chat`, per oltre 120 prompt
complessivi. Le domande hanno coperto anche richieste volutamente semplici e
varianti dello stesso intento.

### Sessione 4: banco di prova generale in inglese

Sono stati provati saluti, identità, coscienza, capacità, calcolo, confronti,
deduzione, lessico, grammatica, traduzione, empatia e memoria. Risultati
significativi:

- saluto, identità e capability ledger hanno funzionato;
- `13 times 7`, `144 divided by 12`, `17 minus 29` hanno prodotto `91`, `12`,
  `-12`;
- `2 to the power of 10`, confronto fra `3.14` e `3.41`, ordinamento di numeri
  e durata di un viaggio hanno fallito o prodotto un muro;
- `2, 4, 6, 8` -> `10` e le deduzioni `Ada writer`, `Milo not dog` hanno
  funzionato;
- il modus ponens con `rains -> wet` non è stato applicato; la conversazione ha
  invece risposto con un muro alla domanda sulla strada bagnata;
- sinonimo di quick, contrario di hot, definizione di democracy e spiegazione
  di recursion hanno funzionato;
- il vincolo `yes or no` sul fuoco freddo non è stato rispettato;
- le traduzioni richieste hanno fallito, salvo una risposta narrativa estranea
  alla richiesta;
- plurale, passato verbale e correzione grammaticale hanno fallito o attivato
  una relazione non disponibile;
- le risposte su acqua, tre colori e poesia hanno avuto una certa capacità di
  composizione, ma il conteggio/formato non è stato verificato in modo
  affidabile;
- il fatto personale sul numero preferito è stato invece ricordato nello stesso
  processo; questo contrasta con i fallimenti precedenti su nome e colore;
- alla dichiarazione `I am sad today` non è seguita empatia: il sistema ha
  negato di avere stati propri e ha deviato sull'utente.

### Sessione 5: italiano e cambio di lingua

La batteria italiana ha incluso identità, aritmetica, deduzione, traduzione,
memoria, definizioni, empatia, conoscenza geografica e richieste di cautela.

- `42` e `25` per somme e divisioni hanno funzionato;
- confronto, ordinamento, sillogismo sui cani, sillogismo sui pesci e
  controfattuale pioggia/strada hanno fallito o ricevuto un muro;
- il contrario di grande -> `Small` ha funzionato, ma il sinonimo di veloce e
  la definizione richiesta non sono stati gestiti;
- le traduzioni dall'inglese e verso il francese/inglese hanno fallito;
- il sistema dichiara di parlare entrambe le lingue, ma passa spesso
  dall'italiano all'inglese e non mantiene la lingua del turno;
- `Tokyo` e la risposta sulla Divina Commedia mostrano che parte della
  conoscenza geografica/letteraria è presente, ma non è stata verificata in
  modo uniforme;
- ha riconosciuto il nome `Giulia` come fatto appreso, ma non ha risposto a
  `Come mi chiamo?`, confermando l'incoerenza fra acquisizione e recupero;
- a una richiesta esplicita di non inventare su `Zorblax` ha risposto con un
  muro, comportamento corretto sul piano anti-allucinazione;
- `2` e `10` come estremi di un intervallo hanno prodotto `8`, invece di
  chiarire o elencare i pari richiesti;
- `Dammi due ragioni...` ha prodotto un muro invece di rispettare il conteggio.

### Sessione 6: strumenti, piani, vincoli e provenienza

Con gli strumenti abilitati sono state richieste ispezione locale, lettura di un
file, piano per fare il tè, gestione di una restrizione, spiegazione di
KB-first, confronto, JSON, lista numerata, sintesi, memoria e ragionamento.

- la lista dei file della directory ha funzionato;
- la lettura di `README.md` prima ha prodotto un muro su `readme.md`, poi una
  risposta molto lunga quando la richiesta è stata riformulata;
- la domanda sulla directory corrente è stata interpretata come una domanda
  sul concetto fisico `current`, segnale di collisione lessicale grave;
- il piano per fare il tè è stato rifiutato perché mancava uno schema
  verificato; il follow-up "primo passo" ha prodotto una frase non pertinente;
- dopo la restrizione "senza zucchero" il sistema ha dato un piano generico e
  non ha dimostrato di averlo aggiornato;
- `KB-first`, "fact and rule" e "natural language cues" non sono stati
  riconosciuti come concetti, pur essendo centrali nella propria architettura;
- la domanda sul presidente di Marte ha ricevuto un muro corretto;
- formule, oceani e pianeti hanno avuto risposte corrette;
- il confronto bicicletta/auto ha restituito un template analitico ripetitivo,
  non due frasi specifiche;
- le spiegazioni sulla fotosintesi sono state sostanzialmente uguali per
  bambino ed esperto, quindi il controllo del destinatario non ha funzionato;
- JSON, lista numerata e risposta in una frase non sono stati verificati o
  rispettati;
- la traduzione spagnola è stata dichiarata parziale invece di essere inventata;
- il riepilogo della conversazione ha ricordato solo parole recenti, non gli
  eventi importanti; la domanda sul primo turno non ha recuperato la storia;
- `forget` non ha rimosso il numero preferito: la memoria è rimasta attiva;
- transitive closure su età (`Cara`) e sillogismo semplice (`Yes`) hanno
  funzionato in alcuni casi;
- percentuale `19% of 200` -> `38` e media `2,4,6` -> `4` hanno funzionato;
- la richiesta di una risposta cauta e la tesi inventata su Zorblax hanno
  ricevuto muri corretti, ma il modulo non ha saputo formulare cautela utile.

## Problemi comportamentali rilevati

La severità è riferita alla missione di diventare un interlocutore universale:

| Severità | Problema | Evidenza | Rischio |
|---|---|---|---|
| **P0** | Risposta fuori dominio al posto di un muro | confronto `3.14/3.41` non risposto; `current` -> definizione elettrica; `41/14` -> `Skin` | errore non riconoscibile dall'utente, soprattutto in contesti decisionali |
| **P0** | Memoria personale non coerente | nome/colore talvolta confermati ma non recuperati; `forget` non abla il numero | promessa conversazionale non affidabile |
| **P0** | Mancanza di controllo del piano | richieste composte rispondono a un sottoframmento o a un template estraneo | il sistema può dichiarare successo senza soddisfare il compito |
| **P1** | Aritmetica e operatori incompleti | potenze, ordinamento, confronti, intervalli e alcuni condizionali falliscono | scarsa affidabilità su ragionamento elementare |
| **P1** | Vincoli di formato non applicati | sì/no, esattamente tre parole, due ragioni, JSON, lista numerata | impossibilità di usare l'output in una pipeline |
| **P1** | Gestione linguistica asimmetrica | italiano dichiarato ma risposte spesso inglesi; traduzioni falliscono | interlocuzione multilingue solo nominale |
| **P1** | Follow-up non ancorati al contesto | piano del tè -> risposta musicale; richiesta di primo passo -> frase generica | perdita del filo e falsa continuità |
| **P1** | Adattamento al destinatario assente | spiegazione per bambino e per esperto quasi identiche | bassa utilità pedagogica |
| **P2** | Template analitici ripetitivi | confronto bicicletta/auto e risposta causale con stesso schema | verbosità senza contenuto specifico |
| **P2** | Recupero conversazionale superficiale | riepilogo basato su parole recenti | memoria episodica non distinguibile da salienza lessicale |
| **P2** | Provenienza delle regole poco leggibile | stampa di regole indotte estranee come `chess_rank` e `philosopher` | difficile diagnosticare perché una risposta è stata prodotta |

## TODO per la missione di interlocutore universale

La lista seguente è ordinata per impatto e rispetta il vincolo KB-first: le
superfici linguistiche, i sinonimi, i cue e i template devono essere insegnabili
nella KB; il C deve fornire meccanismi generali, non nuove liste di parole.

### P0: sicurezza comportamentale

- [ ] **Guardia anti-risposta estranea:** introdurre uno score di evidenza e un
  controllo di compatibilità fra intento, slot e tipo di risposta; se il
  candidato migliore non supera la soglia, produrre un muro, mai un'entità
  casuale.
- [ ] **Testare collisioni lessicali:** aggiungere casi per `current`, `read`,
  `skin`, `reason` e nomi propri, verificando che il significato dominante non
  venga scelto solo per sovrapposizione.
- [ ] **Memoria con ciclo completo:** per nome, colore e numero dimostrare
  `assert -> recall -> forget -> non-recall`, nella stessa sessione e dopo
  `/save`/`/restore` quando pertinente.
- [ ] **Piano completo per richieste coordinate:** rappresentare tutti i
  subgoal, il formato e le dipendenze in `answer_plan`; rifiutare o chiedere
  chiarimento se anche un subgoal resta irrisolto.
- [ ] **Ablation delle competenze:** ogni nuova classe insegnabile deve avere
  prova runtime di acquisizione e rimozione, non solo un output golden.

### P1: ragionamento generale

- [ ] **Unificare gli operatori numerici:** somma, differenza, prodotto,
  divisione, potenza, confronto, ordinamento, percentuale, media e intervalli
  devono condividere slot e ruoli numerici, con test su varianti e segni.
- [ ] **Completare l'inferenza condizionale:** distinguere modus ponens,
  contrapposizione invalida e fallacia dell'affermazione del conseguente;
  dichiarare esplicitamente quando i dati non bastano.
- [ ] **Supportare insiemi e quantificatori:** distinguere "tutti", "nessuno",
  "alcuni", intervalli inclusivi e richieste di enumerazione.
- [ ] **Portare i concetti linguistici mancanti nella KB:** plurali, tempi
  verbali, correzione grammaticale, sinonimi e traduzione devono essere
  relazioni/query KB-backed, non confronti letterali nel C.
- [ ] **Separare acquisizione da risposta:** dopo un fatto personale appreso,
  il resolver deve indicizzare soggetto, proprietà e valore e non affidarsi al
  solo testo dell'ultimo turno.

### P1: dialogo e multilinguismo

- [ ] **Persistenza della lingua del turno:** rilevare lingua e preferenza
  dell'utente, mantenere la lingua nella risposta e consentire switch esplicito.
- [ ] **Frame per memoria episodica:** registrare chi, cosa, quando e fonte del
  fatto; il riepilogo deve recuperare eventi, non soltanto token salienti.
- [ ] **Coreferenza e follow-up:** collegare "il primo passo", "quello", "ora"
  e pronomi all'ultimo piano o entità compatibile.
- [ ] **Intenti sociali:** distinguere dichiarazione di tristezza, richiesta di
  empatia e domanda sullo stato interno; rispondere con una forma KB-backed
  senza simulare sentimenti propri.
- [ ] **Adattamento al destinatario:** modellare livello, lingua e lunghezza
  richiesti in un frame di risposta; verificare il risultato prima di inviarlo.

### P1: strumenti, piani e output

- [ ] **Separare le domande sugli strumenti dai sensi omonimi:** una query sul
  filesystem deve avere priorità solo quando il frame tool è completo; in caso
  di ambiguità chiedere chiarimento.
- [ ] **Piano teachable per attività quotidiane:** rappresentare procedure come
  passi, prerequisiti e vincoli nella KB, invece di introdurre un handler per il
  tè o per ogni nuova attività.
- [ ] **Post-shaper verificabile:** controllare cardinalità, JSON valido, numero
  di frasi/parole, lingua e schema prima della consegna; in caso di fallimento
  dichiarare il limite.
- [ ] **Template specifici al soggetto:** evitare di usare il template causale
  generale per ogni confronto; la variazione deve restare in `response_template`
  o in frame KB appropriati.

### P2: valutazione e crescita

- [ ] **Creare una suite held-out per categorie:** almeno 20 casi ciascuno per
  memoria, numeri, logica, traduzione, formato, strumenti, empatia e rifiuto
  dell'invenzione.
- [ ] **Misurare quattro esiti distinti:** risposta corretta, muro corretto,
  risposta incompleta e risposta fuori dominio; il wall-rate da solo nasconde
  gli errori più pericolosi.
- [ ] **Aggiungere fuzzing comportamentale:** per ogni successo generare sinonimi,
  ordine invertito, numeri diversi, entità multiword e cambio di lingua.
- [ ] **Rendere la provenienza interrogabile:** associare ogni risposta a
  evidenze, regole e modulo vincente, senza stampare derivazioni estranee non
  pertinenti.
- [ ] **Ripetere questa batteria dopo ogni generazione:** conservare il
  transcript e confrontare regressioni, non solo il punteggio aggregato.

## Valutazione aggiornata

Il primo voto `6,6/10` era basato su tre sessioni brevi e sovrastimava la
generalità perché premiava soprattutto la prova KB-first. Alla luce delle tre
sessioni aggiuntive lo aggiorno a **6,2/10**: il valore di ricerca resta alto,
ma la missione di interlocutore universale richiede che un sistema sappia
distinguere un vero muro da una risposta casuale e mantenga il contesto.

## Scorecard: quanto vale per me parrot0

Scala 0-10; il punteggio ponderato è una valutazione personale basata soltanto
sulle tre sessioni sopra, non una metrica ufficiale del repository.

| Indicatore | Peso | Punteggio | Evidenza sintetica |
|---|---:|---:|---|
| Apprendimento KB-first senza rebuild | 25% | 8,5 | `puppo` e `zorf` cambiano analisi e inferenza a runtime |
| Trasparenza del self-model | 15% | 8,0 | capability ledger concreto, con limiti dichiarati |
| Correttezza fattuale di base | 15% | 7,0 | molte risposte corrette, ma copertura non uniforme |
| Ragionamento e confronto | 15% | 3,5 | fallisce `41` contro `14` con risposta non pertinente |
| Memoria e continuità | 10% | 3,0 | non recupera nome e colore appena insegnati |
| Robustezza linguistica/multilingue | 10% | 4,0 | italiano e inglese di base, molte traduzioni fallite |
| Gestione dei vincoli e dei muri | 5% | 2,5 | diversi vincoli ignorati e alcuni muri corretti |
| Valore esplorativo per la tesi | 5% | 9,0 | rende osservabile una possibile struttura funzionale articolata |
| **Totale ponderato** | **100%** | **6,2** | **valore alto come banco di ricerca, basso come chat generalista** |

## Conclusione operativa

Per me parrot0 vale soprattutto come **banco sperimentale di ricostruzione
comportamentale**, non ancora come prodotto conversazionale. La prossima
verifica più informativa sarebbe ripetere gli stessi test con ablation esplicita
e testare varianti del confronto numerico, della memoria e dell'insegnamento
linguistico, controllando che ogni acquisizione e rimozione cambi davvero il
comportamento osservabile. In termini di sequenza di lavoro, i primi tre
obiettivi sono i tre P0: eliminare le risposte estranee, rendere verificabile la
memoria e imporre un piano completo prima di rispondere.
