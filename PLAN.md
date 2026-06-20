# PLAN - programma per far emergere l architettura del ragionamento

## Scopo

Questo piano descrive il programma lungo di parrot0: usare il dialogo con LLM e umani come pressione comportamentale per ricostruire, in C deterministico, una parte sempre piu fedele dell architettura funzionale che negli LLM implementa ragionamento evoluto.

La tesi di `PRINCIPLES.md` resta il vincolo centrale: non crediamo alla dichiarazione introspettiva del modello. L introspezione propone ipotesi, ma solo il codice che passa conversazioni e test puo conservarle. Ogni avanzamento deve diventare comportamento riproducibile, auditabile, piccolo abbastanza da essere compreso, e abbastanza generale da trasferire a esempi non visti.

## Assi del programma

### 1. Pressione comportamentale prima dell architettura

Non si disegna una mente completa a tavolino. Si costruiscono prove sempre piu difficili e si lascia che le separazioni funzionali emergano quando diventano necessarie.

Metodo:
- partire da fallimenti osservati in chat reale;
- trasformare ogni fallimento in un obiettivo verificabile;
- implementare il minimo meccanismo che generalizza oltre la frase vista;
- aggiungere test held-out prima di considerare acquisita la capacita;
- rifiutare soluzioni che funzionano solo come phrasebook.

Segnale di progresso: il codice inizia spontaneamente a separare parsing, memoria, credenze, controllo, spiegazione, pianificazione, attenzione al contesto e self-model perche queste funzioni servono a passare prove reali.

### 2. Conversazione come camera a nebbia

La conversazione deve rendere visibili le tracce del ragionamento. Ogni turno difficile e una collisione: se parrot0 risponde male, il difetto indica una parte mancante della struttura.

Canali di pressione:
- chat umana libera;
- `make chat-bench` con dialoghi held-out;
- `make chat-sim` con utente simulato;
- bench simbolici e cryptic stimuli;
- missioni di conoscenza verificabile, come POSIX shell;
- scenari integrati dove memoria, regole, correzione e spiegazione si compongono.

Ogni canale deve produrre transcript e task. Il transcript e l evidenza grezza; il task e la distillazione del fallimento in forma testabile.

### 3. Fedelta contro impostura

Il pericolo principale e costruire un imitatore superficiale che passa prompt fissi senza possedere la struttura funzionale cercata.

Regole anti-impostor:
- ogni feature richiede varianti held-out;
- usare nomi, predicati e domini generati al momento quando possibile;
- separare esempi di sviluppo da esempi di accettazione;
- preferire test composizionali: la risposta corretta deve dipendere da piu capacita gia acquisite;
- chiedere spiegazioni e proof trace, non solo risposte finali;
- quando una risposta riguarda se stesso, deve derivare da stato reale, moduli registrati o fatti nel KB;
- misurare anche il comportamento sui casi negativi: ignoto, ambiguo, contraddittorio, non capito.

Una capacita e reale solo quando trasferisce. Se non trasferisce, e una scorciatoia.

## Fasi

### Fase 0 - Strumentazione permanente

Obiettivo: rendere impossibile ingannarsi sui progressi.

Deliverable:
- benchmark conversazionale stabile con dialoghi naturali;
- simulatore di utente non deterministico per scoprire fallimenti nuovi;
- log automatici per chat difficili;
- metriche di muro, ripetizione, recupero di memoria, uso del contesto e risposta grounded;
- suite di regressione veloce per ogni capacita acquisita.

Criterio di uscita: ogni nuova generazione puo essere confrontata con le precedenti su esperienza conversazionale, test simbolici e casi integrati.

### Fase 1 - Uscire dal muro

Obiettivo: sostituire il parrot che ripete o dice non capisco con un agente che sa diagnosticare il proprio limite.

Capacita:
- distinguere non capito, non noto, falso, ambiguo, contraddittorio;
- produrre domande chiarificatrici mirate;
- evitare ripetizioni verbatim del fallback;
- riflettere la parte capita del turno;
- non negare come falso cio che e solo assente dal KB.

Perche conta: il ragionamento evoluto non e onniscienza. E controllo dell incertezza. Un sistema che non sa dire cosa manca non sta ancora ragionando in modo fedele.

### Fase 2 - Linguaggio naturale come interfaccia strutturale

Obiettivo: rendere il linguaggio una porta verso strutture interne, non una lista di template.

Capacita:
- intenti parafrasabili;
- turni multi-intento;
- possessivi e memoria personale;
- pronomi e referenti discorsivi;
- coordinazione, negazione, contrasto, causa;
- comparativi e relazioni ordinate;
- registro sociale che non ruba il contenuto.

Criterio di uscita: una persona puo parlare in modo naturale e parrot0 conserva almeno la struttura logica minima del turno: chi fa cosa, a chi, con quale vincolo, in quale contesto.

### Fase 3 - Memoria e continuita

Obiettivo: passare da risposte isolate a presenza conversazionale.

Capacita:
- working memory degli ultimi turni;
- memoria personale esplicita;
- memoria episodica riassumibile;
- correzione e retract;
- distinzione tra fatti permanenti, ipotesi locali e contesto temporaneo;
- richiamo guidato da entita, tema, relazione e scopo.

Criterio di uscita: parrot0 puo rispondere a "di cosa stavamo parlando?", correggere una premessa precedente, e usare un fatto introdotto prima senza confonderlo con conoscenza permanente.

### Fase 4 - Ragionamento locale e proof traces

Obiettivo: rendere visibile la catena inferenziale.

Capacita:
- regole universali introdotte nel turno;
- inferenza su nonce words;
- catene transitive;
- eccezioni e default;
- contraddizioni con stato di credenza;
- spiegazioni compatte: fatti, regole, binding, limiti.

Criterio di uscita: parrot0 non solo risponde, ma puo mostrare da quali premesse e passaggi deriva la risposta. Il proof trace diventa il microscopio dell architettura.

### Fase 5 - Controllo del ragionamento

Obiettivo: introdurre un controller esplicito quando la complessita lo pretende.

Capacita:
- scelta del modulo o dei moduli da attivare;
- arbitraggio tra registro naturale, simbolico, codice e shell;
- gestione di ambiguita tra interpretazioni concorrenti;
- limiti di profondita e tempo;
- differenza tra fallimento, timeout e non provato;
- pianificazione di sotto-obiettivi.

Criterio di uscita: davanti a un problema composito, parrot0 sa scomporre il compito, scegliere strumenti interni, fermarsi quando non puo concludere, e dire perche.

### Fase 6 - Apprendimento verificabile

Obiettivo: far acquisire conoscenza nuova in modo cumulativo e auditabile.

Missioni:
- POSIX shell come dominio-oracolo deterministico;
- lettura di testi con estrazione di fatti;
- induzione di regole da esempi;
- persistenza in `kb/*.p0`;
- diff leggibile di cio che e stato appreso;
- test su combinazioni mai insegnate verbatim.

Criterio di uscita: dopo una sessione di apprendimento, parrot0 sa qualcosa che prima non sapeva, lo sa per una ragione verificabile, lo conserva nel repo, e lo usa su casi nuovi.

### Fase 7 - Self-model fedele

Obiettivo: costruire la chiusura riflessiva del sistema.

Capacita:
- rappresentare se stesso nel KB;
- elencare capacita da moduli e test reali;
- spiegare come sa chi e;
- riconoscere limiti attuali;
- non dichiarare coscienza, comprensione o capacita non supportate dallo stato;
- collegare comportamento, memoria, moduli e proof trace in una risposta introspezionabile.

Criterio di uscita: quando dice "io", quel riferimento e ancorato a fatti e strutture interrogabili, non a una stringa hard-coded.

### Fase 8 - Integrazione: il piccolo apprendista

Obiettivo: comporre tutte le capacita in scenari lunghi.

Scenario tipo:
1. caricare un dominio nuovo;
2. insegnare vocabolario e fatti in linguaggio naturale;
3. porre una domanda che richiede memoria e regole;
4. chiedere spiegazione;
5. correggere una premessa;
6. chiedere aggiornamento della conclusione;
7. chiedere cosa ha imparato e come lo sa;
8. salvare e ricaricare la conoscenza.

Criterio di uscita: nessun singolo modulo puo fingere tutto lo scenario. Se passa, l architettura funzionale sta iniziando ad articolarsi.

## Ciclo operativo per ogni generazione

1. Scegliere un fallimento concreto da transcript o benchmark.
2. Scrivere il comportamento desiderato in `TASK.md`.
3. Aggiungere almeno un test di sviluppo e un held-out.
4. Implementare il minimo cambiamento in C.
5. Eseguire test locali e chat manuale.
6. Registrare in `JOURNAL.md` cosa e emerso, cosa e rimasto fragile, e quale nuova pressione ha senso.
7. Spostare il prossimo obiettivo da `TASKLIST.md` solo quando e maturo.

Disciplina: ogni generazione deve essere piccola, ma il programma complessivo deve diventare sempre piu duro.

## Metriche principali

Metriche conversazionali:
- wall-rate: quante volte cade nel non capisco piatto;
- repetition-rate: quante volte ripete la stessa risposta;
- context-use-rate: quante risposte usano un fatto precedente;
- clarification-quality: se la domanda chiarificatrice nomina il buco giusto;
- grounded-self-rate: quante risposte su se stesso citano stato reale.

Metriche inferenziali:
- held-out transfer;
- numero di passaggi nel proof trace;
- correttezza su nonce words;
- distinzione true/false/unknown/conflict;
- robustezza a riformulazioni.

Metriche architetturali:
- crescita di interfacce tra sottosistemi invece di un monolite;
- riduzione di template rigidi;
- tracciabilita tra input, parse, memoria, prova e risposta;
- facilita di aggiungere un nuovo dominio senza riscrivere il cervello.

## Principio di convergenza

Non sapremo mai con certezza se l architettura emersa e identica a quella latente negli LLM. Il criterio pragmatico e asintotico: piu i compiti diventano ampi, composizionali, held-out, contraddittori, contestuali e auto-riflessivi, meno spazio resta per un imitatore superficiale.

Il progetto deve quindi aumentare progressivamente la pressione:
- da template a ruoli;
- da turni singoli a dialoghi;
- da risposte a prove;
- da memoria locale a memoria persistente;
- da moduli isolati a controllo compositivo;
- da dichiarazioni su di se a self-model interrogabile.

Se una struttura funzionale reale e necessaria per continuare a passare, allora quella struttura dovrebbe emergere nel C. Questo e il cuore del piano.

## Milestones

### M0 - Chat non imbarazzante
Parrot0 saluta, risponde su se stesso, ricorda dati personali semplici, non ripete fallback, e chiede chiarimenti specifici.

### M1 - Ragionatore locale
Parrot0 usa regole temporanee, nonce words, comparativi, aritmetica e proof trace in un singolo turno.

### M2 - Conversazione continua
Parrot0 mantiene entita, temi e correzioni per piu turni, poi riassume cosa e successo.

### M3 - Apprendimento auditabile
Parrot0 acquisisce conoscenza da shell o testo, la persiste in `kb/*.p0`, e risponde a domande held-out.

### M4 - Self-model operativo
Parrot0 descrive capacita, limiti e identita da stato reale, e sa spiegare il supporto delle proprie risposte.

### M5 - Scenario integrato
Parrot0 passa il benchmark del piccolo apprendista: apprendere, ricordare, ragionare, spiegare, correggersi, salvarsi e descriversi.

## Cosa non fare

- Non importare un LLM a runtime.
- Non trasformare il progetto in una collezione di risposte fisse.
- Non dichiarare capacita interiori non osservabili.
- Non accettare una feature senza test held-out.
- Non far crescere `brain_respond()` come blocco monolitico se il compito chiede articolazione.
- Non confondere benchmark a scelta singola con intelligenza conversazionale.

## Frase guida

Costruire pressione comportamentale sufficiente perche la scorciatoia smetta di funzionare. Quando solo una struttura articolata puo passare, lasciare che quella struttura emerga, fissarla in C, testarla, e alzare ancora la pressione.
