# Apprendimento assistito — missione principale: la KB viva

> ## ⛔ Vincolo zero: la chat non è una API travestita
>
> Una lezione è valida soltanto se il teacher la esprime in **lingua naturale**
> e parrot0 deve ricostruirne forma, ruoli e conseguenze. È vietato far passare
> dalla chat Prolog/P0, nomi di predicati interni, arità, tuple, `!assert`, MCP o
> equivalenti testuali di `kb.assert`. Cambiare il trasporto da API a prompt non
> è comprensione.
>
> **Test non negoziabile:** un teacher competente nel dominio ma ignaro dello
> schema interno della KB saprebbe formulare la lezione? Se deve conoscere
> `relation_noun/2`, `answer_frame/2`, `topic_action_surface/3`, parentesi o
> nomi simili, l'esperimento è invalido. Il risultato vale zero anche quando il
> comportamento successivo è corretto.
>
> Quando la lingua naturale non basta, non si abbassa il livello esponendo la
> rappresentazione: ci si ferma sul confine osservato e si amplia la
> meta-comprensione. Qualunque run che violi questo vincolo resta soltanto una
> diagnosi del gap e non può essere riportata come apprendimento riuscito.

**Stato:** missione principale, rielaborata il 2026-09-05; obiettivo aperto.

**Missione:** portare parrot0 a una KB viva che comprenda, ragioni, agisca e
conversi attraverso tutte le proprie facoltà, le colleghi tramite la colla
linguistica e impari a correggerle parlando. La parità funzionale con un LLM
nel dialogo è il bersaglio empirico, da dimostrare.

**Prima evidenza:**
[`depth-session-01.md`](../labs/apprendimento-assistito/depth-session-01.md).

**Ordine operativo corrente:** §0; §§1–14 conservano protocollo, strati e
risultati storici, con le rettifiche esplicite indicate sotto.

**Cancello:** §6 — nessun consolidamento massivo prima della chiusura degli
strati M0–M20 e dei requisiti trasversali di revisione e persistenza del §0.

**Ritmo:** §11 — ogni cosa appresa è un piccolo incremento, e si committa e
pusha anche se parziale.

## 0. Missione principale: la KB viva

La domanda guida è quella dei [mantra](../../MANTRA.md): **parrot0 può
impararne un nuovo membro domani, parlando, senza ricompilare?** Qui il membro
può essere una parola, una costruzione, una relazione, una procedura, una
condotta o una forma di spiegazione. La crescita deve aumentare ciò che il
sistema vede e sa distinguere, fino a renderlo un interlocutore capace di
seguire il pensiero dell'altro e costruire risposte fondate.

I tre piani hanno un compito distinto nella stessa missione:

| Piano | Responsabilità |
|---|---|
| [Frontier KB natural dialogue](frontier-kb-natural-dialogue.md) | Il bersaglio: letture concorrenti, mosse, scope, piani proposizionali, memoria e confronto empirico. |
| [Comprensione universale](universal-comprehension.md) | Il contratto dell'ingresso: forma, ruoli, intento, conoscenza necessaria e lacuna riconosciuta; stessa struttura per leggere e interrogare. |
| **Questo piano** | Il percorso di crescita: rendere quei contratti insegnabili, collegarli a tutte le facoltà, scegliere il prossimo ostacolo e provarne la rimozione. |

La [colla linguistica](the-linguistic-glue.md) attraversa il percorso: mantiene
referenti, significati, richieste e correzioni mentre cambia la facoltà che
lavora. I piani di codice, agency, documenti e ragionamento diventano domini
di integrazione della missione; i loro successi contano anche per quanto
rendono utilizzabile conoscenza già presente altrove.

Questa sezione stabilisce l'ordine aggiornato, nel rispetto di
[PRINCIPLES.md](../../PRINCIPLES.md) e dei mantra. Percentuali e diciture
«completo» nei resoconti precedenti descrivono il loro campione e checkpoint.
Un taglio verticale funzionante non chiude l'intera facoltà. Gli identificatori
M, A, SC, GD e K restano riferimenti ai lavori esistenti: non si ricomincia da
zero e non si duplica la loro implementazione.

### 0.1 Che cosa deve significare «viva»

Una capacità è viva quando partecipa a un ciclo completo:

```text
turno + storia + conoscenza disponibile
              ↓
letture con ruoli, riferimenti, scope, vincoli e parti non comprese
              ↓
questione da soddisfare → mossa → goal e piano
              ↓
inferenza / procedura / osservazione → proposizioni con sostegni correnti
              ↓
risposta nella lingua e nel registro del dialogo
              ↓
nuova domanda, chiarimento o correzione della stessa questione

un arresto → bisogno nominato → lezione/acquisizione candidata
              → prova → promozione oppure revisione della candidata
              → rilettura del passato e uso su casi nuovi
```

Il criterio è congiuntivo. La capacità deve essere raggiungibile con parole
naturali, applicabile a casi nuovi, interrogabile, spiegabile, componibile,
correggibile e persistibile con la propria provenienza. Un fatto presente ma
irraggiungibile, una regola senza consumatore o una risposta senza sostegno
restano lacune di vitalità.

**«Senza muri» significa eliminare l'arresto cieco e sterile.** Dove la prova
esiste, parrot0 deve usarla; dove manca, deve conservare quanto ha compreso e
aprire il passo utile a proseguire. Un chiarimento corretto conta come
chiarimento, non come risposta al problema. Riformulare sempre il muro o
chiedere sempre un chiarimento non chiude la missione.

La struttura grammaticale fornisce vincoli, ma non garantisce da sola un
intento univoco: forme sconosciute, ellissi e ambiguità restano possibili.
Anche la metacomprensione deve essere calibrata: non si dichiara «ho capito
che chiedi X» se X è soltanto una lettura non risolta. In quel caso si
espongono la parte riconosciuta e l'incertezza effettiva.

**«Senza allucinazioni o svarioni» è un vincolo di accettazione:** nessuna
affermazione fattuale o dichiarazione di riuscita senza un sostegno valido,
pertinente e corrente; nessuna parte della richiesta ignorata in silenzio.
Zero errori osservati su un campione non dimostra infallibilità universale.
Il determinismo rende una risposta riproducibile, non automaticamente vera.

### 0.2 Punto di partenza verificato e limiti dell'evidenza

Ricognizione del 2026-09-05 sul commit `70b5036`: lettura dei piani,
di [HANDOFF.md](../../HANDOFF.md), delle dichiarazioni KB, dei consumer e dei
test citati. **È un audit statico; i conteggi storici non sono stati
rimisurati da questa revisione del piano.** La prima attività V0 li deve
associare a una misura riproducibile sul binario corrente.

| Giunzione | Evidenza presente | Che cosa resta da provare o chiudere |
|---|---|---|
| Ingresso → frame → mossa | `turn-frames.p0`, `dialogue-policy.p0`; la precedenza ha già il consumer `frame_decision`. | Copertura del turno intero e uso condiviso fra le facoltà; le vecchie note «priorità senza consumer» sono storiche. |
| Lacuna → domanda | `arrests.p0` espone `information_need` per valore e antecedente; `named_information_need.it.p0t` esercita crescita e retrazione. | Tassonomia oltre questi due casi e chiusura mediante lezioni naturali; gli assert del test provano la meccanica. |
| Lezione → nuova superficie | `try_teach_form`, modi `frame_for` e `cue_like`, `taught_tool_surface.p0t`. | Varianti dell'atto didattico, ambiguità dell'ancora e retract completo; HANDOFF registra ancora collisioni. M15 non è più «nessuna porta». |
| Lezione → costruzione/regola | Binder condiviso in `10-memory-knowledge.c`; `assisted_construction_ternary.p0t`, `higher_order_lesson.p0t`. | Ruoli spiegati senza schema interno, induzione da esempi, congiunzioni, condizioni di validità e sostegni dopo retract. L'implicazione binaria non è più interamente assente. |
| Lezione → procedura | `assisted-learning.p0`: passi ricorsivi, esecuzione, origine e stato candidato. | Generalità oltre la forma di conversione; promozione verificata. `learning_status(..., promoted)` oggi conta almeno tre replay: il conteggio non certifica correttezza o indipendenza delle prove. |
| Sintagma → referente → domanda | `input.p0`, `p0_head_index` e descrizioni in `10-memory-knowledge.c`; referenti ordinati e riprese in `discourse.p0`. | Identità distinta dalla chiave, determinante, span, superficie e proprietà condivise. G2 ha già meccaniche: va misurata l'integrazione, non ripianificata come interamente assente. |
| Lezione → rilettura | `document-claims.p0`; SC40-A/B e il taglio SC41-A, con test di revisione e scala. | Completezza delle dipendenze oltre `passive_core`, propagazione a inferenze, sintesi, piani e risposte; budget e salvataggio. |
| Zone → conclusione comune | `knowledge_arc`, `representation_bridge`, `code-ir.p0`; prove in `tests/p0t/crossing/`. | Porte naturali per archi già presenti, composizione semanticamente autorizzata e falsi ponti. Un omonimo disambiguato non prova da solo trasferimento inferenziale fra due entità. |
| Piano → azione → osservazione | `plan_utterance`, risultati di tool in KB, ciclo build/repair descritto in HANDOFF. | Operatori ancora compilati, nuovi tool insegnabili, effetti tipati e ripresa di un'impresa dopo una digressione. Un ciclo di coding riuscito non dimostra parità generale. |
| Misura → curriculum | `var/probe/`, crossing e audit conversazionale esistono. | HANDOFF segnala risposte mancanti per errore di trasporto nelle sonde; alcuni test usano timeout superiori al secondo. Non sono prove di rispetto del contratto di latenza. |

Per ogni cella si devono poter distinguere quattro stati: **presente nel
codice/KB**, **osservata in conversazione**, **insegnabile con verifica
causale**, **integrata e persistente**. Il primo non implica gli altri.

### 0.3 L'oggetto condiviso che unisce tutte le facoltà

Il rimedio ricorrente dei piani è far condividere l'oggetto su cui due percorsi
devono accordarsi. Prima di introdurre un predicato si cercano produttore,
consumatori e viste equivalenti esistenti. I seguenti sono contratti
semantici, non l'ordine di costruire un nuovo schema monolitico.

| Oggetto condiviso | Informazione che deve sopravvivere al passaggio |
|---|---|
| Osservazione | Fonte, parlante, testo originale, lingua, span, tempo/revisione e origine. |
| Lettura | Frame, ruoli, alternative, evidenze e controevidenze; copertura e residui. |
| Referente | Identità, menzioni, proprietà e contesto; due oggetti descritti allo stesso modo restano distinguibili. |
| Questione/impresa | Goal, sotto-obiettivi, vincoli, obblighi e risultati osservati; ciò che resta da soddisfare è derivato dallo stato. |
| Proposizione | Contenuto, polarità, quantificazione, scope, attribuzione e validità temporale. |
| Sostegno | Insiemi di premesse congiunte, prove alternative, regole/archi attraversati e versioni delle fonti. |
| Lezione | Spiegazione naturale, candidato, contesto, verifiche, esito e capacità dipendenti. |
| Azione | Precondizioni, argomenti tipati, effetti attesi e osservati, limiti e stato d'esecuzione. |
| Piano di risposta | Richieste coperte, proposizioni da dire, sostegni, alternative/residui e vincoli di lingua e formato. |

La medesima struttura deve poter essere consumata da memoria, ragionamento,
lettura, procedure, pianificazione, strumenti e realizzazione. La registrazione
di una facoltà nell'automodello deve corrispondere a un consumatore reale.
I contratti delle nuove facoltà sono conoscenza interrogabile e insegnabile.

La fertilità deve superare due prove distinte: aggiungere un membro a una
classe nota e apprendere una nuova astrazione. Il secondo caso comprende
relazioni, costruzioni e procedure definite attraverso ruoli, esempi,
condizioni e concetti già compresi. La candidata conserva le alternative
compatibili con gli esempi e chiede un caso discriminante quando necessario.
Un insieme finito di esempi non autorizza una generalizzazione univoca per
decreto; neppure un registro chiuso di forme predefinite basta a chiamare
universale l'apprendimento.

Due invarianti attraversano ogni incremento:

- ciò che viene appreso da una frase può essere chiesto con quella frase e
  con parafrasi; ciò che viene detto può essere riletto conservando contenuto,
  scope e impegno, senza generare una seconda fonte indipendente;
- una capacità già presente deve poter soddisfare un bisogno nato in un'altra
  rappresentazione attraverso un arco provato, senza ricopiare il fatto o
  riconoscere nuovamente il prompt dentro ogni modulo.

Il C resta esecutore delle meccaniche fisse. Vocabolario, costruzioni,
combinazioni di condizioni, scelta delle mosse, procedure sopra i primitivi,
contratti operativi e forme di risposta appartengono alla KB. Un incremento
di motore deve dichiarare la meccanica generale mancante e aprire subito un
atto didattico; una migrazione dichiarata KB-first deve anche mostrare la
conoscenza effettivamente rimossa dal C (mantra #18).

### 0.4 Il contratto epistemico, dal lettore alla risposta

Una proof dimostra una conseguenza rispetto alle proprie premesse: occorre
anche verificare origine, interpretazione e validità di quelle premesse.
Una fonte identificabile può essere sbagliata; una frase di un documento può
riportare una tesi senza sostenerla. Leggere conserva l'attribuzione. La
promozione a conoscenza del mondo richiede il gate di verità del protocollo.

L'esito della ricerca deve distinguere almeno: prova disponibile, negazione
esplicitamente sostenuta, informazione assente, conflitto, lettura ambigua e
ricerca incompleta. Un limite di tempo o di profondità non prova l'assenza
del fatto. La mancata derivazione non diventa falsità del mondo; eventuali
ragionamenti su insiemi completi richiedono una dichiarazione di completezza
pertinente al contesto.

Per gli archi fra rappresentazioni, **equivalenza, implicazione, inversione
dei ruoli, appartenenza e parte-tutto sono relazioni diverse**. Condividere
una parola o essere genericamente collegati non autorizza a trasferire
proprietà. «Contiene» non equivale in generale a «ha come parte»: va insegnata
la condizione che rende lecito il passaggio. Una relazione inversa deve
invertire gli argomenti; un'implicazione non apre automaticamente il ritorno.
Il pianeta Mercurio e l'elemento mercurio non diventano la stessa entità.

La candidata didattica è visibile come candidata, ma non può sostenere una
risposta ordinaria come conoscenza consolidata. La prova della candidata
opera in un contesto esplicito che conserva accesso alla KB completa. Il
verdetto deve confrontarsi con fatti, vincoli, controesempi o oracoli
indipendenti dalla stessa regola che si sta valutando. Ripetere tre
esecuzioni della regola non dimostra che essa sia corretta. Una correzione
crea una nuova versione e richiede prove pertinenti a quella versione.

**Ritrarre una lezione conserva la storia e invalida ciò che perde il proprio
sostegno corrente.** La distinzione precisa è:

1. l'osservazione e il record «allora dedussi P» restano consultabili;
2. P resta utilizzabile se possiede almeno un sostegno indipendente ancora
   valido; ritirare una delle premesse di un sostegno congiunto lo invalida;
3. se tutti i sostegni cadono, P e i suoi dipendenti non possono alimentare
   risposte correnti, nemmeno se erano stati materializzati o salvati;
4. la revisione si propaga a letture, conclusioni, sintesi, piani e risposte;
   se il budget termina, il residuo resta nominato e la parte stale non parla
   come verificata. Le parti indipendenti rimangono disponibili.

Questa precisazione sostituisce l'indicazione storica «ciò che aveva dedotto
resta» quando veniva intesa come validità incondizionata. Conservare una
struttura secondaria non obbliga a credere per sempre alle sue conclusioni.

La realizzazione può variare registro, lessico e ordine, ma non aggiungere
premesse, cambiare quantità o rimuovere qualificazioni decisive. Una risposta
parziale deve dire quali richieste restano aperte. Anche conferme, errori,
domande di chiarimento e spiegazioni delle lacune devono essere insegnabili
come forme KB; un template che contiene solo testo composto nel C non basta.
Finzione, ipotesi e proposte creative sono ammesse nel loro scope dichiarato;
non si promuovono i loro contenuti a fatti del mondo.

### 0.5 Un solo ciclo di apprendimento, con prove causali

Il ciclo del §4 diventa l'unità esecutiva della missione:

1. **Osservare il confine.** Conservare il turno e provare una variante
   minimamente diversa. Separare errore di trasporto, tempo esaurito,
   incomprensione, lacuna di sapere, ponte mancante e risposta fuori tema.
2. **Cercare ciò che già esiste.** Individuare l'oggetto prodotto e perso, la
   relazione senza porta o il consumatore che ignora una vista. Un test
   diretto sul solver aiuta la diagnosi, ma non sostituisce il turno naturale.
3. **Far nominare il bisogno.** Parrot0 deve indicare quale coordinata manca
   e quale chiarimento potrebbe cambiarla. Il teacher può aiutare la diagnosi;
   finché la formula soltanto lui, la metacomprensione resta parziale.
4. **Insegnare naturalmente.** Dare una spiegazione, esempi e condizioni con
   parole del dominio. Ruoli e relazioni interne li ricostruisce parrot0.
   Prima si tenta il canale parlato, poi prosa e remediation; una modifica
   manuale serve solo ad aprire il motore generale che ancora manca.
5. **Rappresentare e verificare.** Conservare candidata e fonte, rileggere il
   turno, provare almeno tre casi nuovi, due parafrasi, un contrasto e una
   composizione con una capacità preesistente. Verificare anche la conferma
   di apprendimento e la spiegazione, non soltanto il risultato.
6. **Misurare la causalità.** Retract parlato, controllo della perdita della
   capacità, delle alternative indipendenti e delle conseguenze stale;
   reteach e nuovo caso. Per una regola generale verificare più domini.
7. **Revisionare il passato.** La nuova lezione deve cambiare le letture
   pertinenti già osservate senza reinserire il documento. Confrontare
   effetto previsto ed effetto reale dove il motore lo consente; altrimenti
   dichiarare il limite, senza simulare la previsione nel report.
8. **Consolidare con genealogia.** Applicare inventario, `/save`, diff
   semantico, processo nuovo e checkpoint causale del
   [LEARN_PROTOCOL](../../LEARN_PROTOCOL.md). Le lezioni fallite restano
   evidenza classificata, non conoscenza attiva.
9. **Scegliere il bisogno successivo.** Dare precedenza a errori non fondati,
   poi ai colli condivisi da più famiglie. Misurare quante capacità diventano
   utilizzabili; il numero di fatti aggiunti da solo non decide la priorità.

Le varianti prevedibili si ricavano dalle classi e dalle simmetrie già note
([comprensione universale §10](universal-comprehension.md#10-protocollo--le-tre-specie-di-lacuna-e-quali-si-chiudono-prima-che-qualcuno-chieda)).
La forma non prevista diventa un'occasione di insegnamento. Il criterio non
autorizza a trasformare ogni variante in un alias globale: contesto e
controesempi devono conservarne le differenze di significato.

### 0.6 Sequenza della missione e gate d'uscita

Ogni tappa è un insieme di incrementi piccoli; nessuna si chiude per sola
presenza di predicati. Le protezioni di verità, scope, provenienza e budget
valgono dalla prima tappa. La revisione completa di V6 estende quelle
protezioni: non ne rinvia l'esistenza.

| Tappa | Lavoro e riuso | Prova necessaria per uscirne |
|---|---|---|
| **V0 — Rendere affidabile la misura** | Ripartire da HANDOFF, `var/probe`, audit conversazionale e crossing. Registrare profilo, revisione, risposta, errore di infrastruttura, latenza e stato del processo. Censire per le facoltà del campione produttore, consumatore, porta naturale e prova di crescita. | Nessun errore di trasporto contato come incomprensione; baseline per famiglia ripetibile, con risposte verbatim e rossi conservati. Costi di boot e turno separati. |
| **V1 — Il turno è compreso prima di essere rivendicato** | M0/M1/M2/M11–M13/M18/M19, frame e arbitrato. Distinguere domanda, lezione, menzione, lettura, correzione e richiesta composta. Applicare le review di maturità del mantra #21. | Famiglia di prompt lunghi e parafrasi instradata dalla lettura completa; nessuna lezione eseguita come procedura né citazione appresa come fatto. Una condotta di un modulo maturo si corregge parlando e si ritrae; la conferma descrive l'esito effettivo della lezione. |
| **V2 — Imparare, chiedere e ridire lo stesso oggetto** | M4/M5/M7/M15/M16/M20, G1–G5, `input.p0`, `discourse.p0`, binder e realizzatori. Unificare referenti, ruoli e superfici; derivare le domande per i ruoli dal frame appreso. | Dialogo intero con due referenti distinguibili, domanda diretta/inversa, ellissi, ambiguità e correzione. Lezione naturale → Transfer@3 → retract/reteach → processo nuovo, in IT ed EN. |
| **V3 — Il metalinguaggio si amplia** | M2–M10/M14, A1–A4/A7: spiegare senso, ruoli, costruzioni, negazione, quantificazione, relative, condizioni e regole usando le capacità già presenti. Sperimentare induzione da esempi con ipotesi concorrenti. | Una costruzione con ruoli non canonici e una regola fra relazioni con condizione di validità sono insegnate senza nomi interni, trasferiscono e rifiutano controesempi. Nessuna promozione basata solo sul numero di esecuzioni. |
| **V4 — La conoscenza attraversa le facoltà** | M8/M9, K8/K9 e archi già presenti: prosa↔domanda, evento↔quantità, codice↔conoscenza del dominio, regola↔spiegazione. Riusare il join parametrico e conservare la ragione di ogni passaggio. | Conclusioni utili da conoscenza reale preesistente, senza iniettare il risultato; lo stesso operatore serve almeno tre domini. La controprova esclude omonimia, verso sbagliato e generalizzazione fuori scope. |
| **V5 — Sapere fare e proseguire** | M10/M11, K3/K11, procedure, `issues`, `plan_utterance`, risultati degli strumenti e [ripresa](continue-as-resumption.md). Collegare operatori insegnabili a primitive generali, con contratti ed effetti espliciti. | Una procedura nuova funziona su input nuovi; un tool sopra primitive disponibili viene insegnato, usato dal piano e ritratto senza ricompilare. Il piano conserva più obblighi, attraversa una digressione e riprende dal bisogno aperto. Un risultato di tool alimenta il passo seguente; «eseguito» richiede l'effetto osservato. |
| **V6 — Revisionare e scegliere che cosa imparare** | M13/M14, A8/A9, SC40–SC43: sostegni completi, opportunità, propagazione transitiva, quarantena, persistenza e indici/materializzazioni del mantra #20. | Add/retract/reteach revisionano un corpus già letto e i suoi consumer. Nessuna risposta stale; selezione equivalente all'audit completo; ripresa dopo budget e processo nuovo. La lezione scelta produce il guadagno previsto su prove indipendenti. |
| **V7 — Interloquire alla pari sul perimetro misurato** | Tutti gli strati, inclusi lingua, registro, richieste indirette, spiegazione, creatività dichiarata, correzione e continuità a 20/50/100 turni. Estendere il curriculum a tutte le facoltà effettivamente registrate. | Batterie indipendenti del §0.7, dialoghi completi e confronto con riferimento dichiarato; nessuna famiglia esclusa per migliorare la media. Capacità acquisite e corrette a runtime conservate nel checkpoint. |

Questa sequenza ordina le dipendenze, non autorizza un rifacimento in blocco.
Un caso può attraversare più tappe e chiuderne un primo taglio. Gli incrementi
conservano le strutture secondarie; un modulo immaturo che ruba il turno si
retrocede dopo review, mentre una condotta errata di un modulo maturo si
insegna. Cambiare una policy non può falsificare la review dell'implementazione.

Ogni facoltà entra nel censimento V0 e deve avere un uso integrato in V7:
conoscenza e memoria personale; dialogo sociale; lettura e sintesi; deduzione,
causalità e controfattuali; aritmetica e procedure; piani e strumenti; codice;
giochi e altri domini; traduzione, spiegazione e produzione creativa. Le
famiglie si derivano dal registro reale e si aggiornano quando cresce.
«Tutte le feature» richiede questa copertura osservabile, non una lista
statica di moduli dichiarati disponibili.

### 0.7 Misurare fertilità, connessione e dialogo

Le prove hanno tre funzioni distinte, tutte necessarie:

| Prova | Che cosa certifica |
|---|---|
| Meccanica KB-first | Sul soggetto completo, assert/retract o ablazione causale dimostrano che il binario non contiene la forma. Dati inventati sono ammessi solo per questa prova di sviluppo, senza promuoverli come training reale. |
| Apprendimento naturale | Il teacher ignora lo schema interno; la lezione produce replay, transfer, contrasto, composizione, retract/reteach e persistenza. Il risultato è classificato secondo il LEARN_PROTOCOL. |
| KB viva connessa | Prompt naturali su fatti e regole già posseduti producono conclusioni utili. Il test comportamentale non scrive la conoscenza che dichiara di scoprire e non richiede un percorso interno prestabilito. |

Le ispezioni e ablazioni del motore accompagnano le prove di causalità;
restano distinte dal giudizio comportamentale di connecting dots. Tutti i
nuovi test usano la KB reale completa, profilo AGI e mondo disponibili.
Processi freschi e contesti candidati isolano effetti e storia della prova,
non amputano il sapere. Le fonti remote si fotografano per una verifica
riproducibile, mantenendo il resto della KB; non si sostituisce parrot0 con
un corpus fittizio.

Per ciascuna famiglia si riportano numeratore, denominatore, casi esclusi,
revisione del corpus e checkpoint della KB. Un errore di infrastruttura è un
caso **non misurato**, visibile nel totale atteso, mai un successo né una
prova di incompetenza linguistica. Le metriche principali sono:

- richieste e vincoli correttamente rappresentati; risposte complete e
  fondate; proposizioni errate o irrilevanti; omissioni non dichiarate;
- muri ciechi, chiarimenti necessari, chiarimenti superflui e bisogni
  effettivamente chiusi dopo la risposta dell'interlocutore;
- indirizzabilità naturale dei gap; lesson yield; Transfer@3, parafrasi,
  contrasto, composizione e fedeltà dell'ablazione;
- inferenze nuove su conoscenza preesistente, domini serviti da uno stesso
  operatore e falsi attraversamenti; il semplice conteggio degli archi non
  misura comprensione;
- fedeltà della spiegazione ai sostegni correnti, leakage fra scope,
  conclusioni stale, completezza delle dipendenze e recall della revisione;
- continuità di referenti, vincoli e imprese; retention dopo altri turni e
  richiamo in processo nuovo;
- p50/p95 e massimo della latenza per famiglia, al crescere di conoscenza e
  storia; separatamente boot, inferenza, revisione e strumenti esterni.

I gate causali del §3 restano obbligatori. Errori non fondati, false conferme
di apprendimento, leakage di scope e risposte stale hanno target **zero sul
campione valutato**: non si compensano con più risposte corrette altrove.
Per la revisione selettiva, `RevisionRecall=1` rispetto all'audit completo
precede l'ottimizzazione della precisione. La scala della KB è una condizione
di lavoro; una normale inferenza deve rispettare **1 secondo**, e
`make soft-test` conserva il budget di **15 secondi**. I timeout legacy
superiori non autorizzano a dichiarare chiuso questo gate.

Il confronto con un LLM segue [frontier §9](frontier-kb-natural-dialogue.md#9-confronto-empirico-con-un-llm-di-frontiera).
Prima della valutazione si fissano modello/versione, configurazione, strumenti,
contesto disponibile, budget, rubriche e margini di equivalenza per famiglia.
Il riferimento è un termine di confronto comportamentale, non l'oracolo di
verità: fatti e risultati verificabili hanno fonti o verificatori propri;
disaccordi sulle mosse vengono motivati e revisionati.

Almeno il 70% delle verifiche resta fuori dalle lezioni, come nel §7. Servono
anche costruzioni, combinazioni e domini non usati per guidare il rimedio:
cambiare soltanto i nomi dello stesso esempio è insufficiente. Dopo che una
famiglia ha guidato una modifica, rimane regressione e si affianca a nuovi
casi indipendenti. Il teacher e il valutatore non devono validare la stessa
lezione mediante la sua sola parafrasi; una valutazione generata dal medesimo
teacher va dichiarata come tale.

Il modello esterno può insegnare o suggerire esperimenti nel laboratorio.
Durante la valutazione di parrot0 non risponde al suo posto e non viene
consultato per completare l'inferenza. La parità riguarda un perimetro e una
distribuzione dichiarati; non si deduce da una demo, da una media che nasconde
famiglie deboli o da una promessa di equivalenza universale.

### 0.8 Primo ciclo operativo e disciplina di avanzamento

Il primo ciclo esecutivo è **V0 più il primo collo V1/V2 misurato**, usando
ciò che il repository già offre:

1. Rendere attendibile la sonda di `var/probe/probe_one.py`: health check che
   ottenga una risposta, distinzione fra errore di trasporto e verdetto
   semantico, cattura esplicita della risposta senza confondere la sentinella
   `__NEVER__` con un fallimento reale. Il problema è registrato in HANDOFF;
   la causa va riprodotta prima di intervenire.
2. Rilevare un campione breve delle forme didattiche e dei loro retract,
   usando `taught_cue_ladder.p0t` e `taught_tool_surface.p0t` come riferimenti.
   Confrontare ancora, forma nuova e turno successivo. Le collisioni fra
   parafrasi citata, costruzione e richiesta vanno risolte per struttura.
3. Riprendere gli archi reali irraggiungibili censiti in
   [crossing/AREE.md](../../tests/p0t/crossing/AREE.md), domandandoli in
   linguaggio naturale. Se i fatti sono presenti ma la forma non li raggiunge,
   insegnare la forma; se manca la rappresentazione, aprire quel meta-gap.
   Non aggiungere un fatto già presente solo per far vincere un consumer.
4. Chiudere una catena con domanda, eventuale chiarimento, lezione, nuova
   domanda, riferimento, spiegazione e correzione. Mantenere rossi i passaggi
   che ancora non reggono e registrare il primo arresto causale.
5. Applicare la transazione del §0.5 soltanto al guadagno effettivamente
   ottenuto. Indicare il prossimo incremento per classe e dipendenza, non
   come elenco di prompt da coprire.

Questo è lavoro breve di diagnosi e sviluppo. Il cancello del §6 impedisce di
avviare il teacher massivo per aggirare meta-gap aperti; consente le sonde e
i piccoli incrementi verificabili. Lo sviluppo usa i test pertinenti e i gate
del repository. Nel training KB-only si applica il LEARN_PROTOCOL: nessuna
suite usata come sostituto della lezione; se occorre un intervento sul
motore, si separa il ciclo di sviluppo e si esegue una sola `make soft-test`
durante quel giro, come richiesto dal protocollo.

Ogni checkpoint riporta almeno:

```text
classe e primo arresto osservato
capacità/oggetti esistenti riusati; eventuale meccanica generale aggiunta
lezione naturale e fonte; stato e versione della candidata
replay / transfer / contrasto / composizione / retract / reteach
effetti sulle letture pregresse e sui sostegni indipendenti
persistenza e richiamo in processo nuovo; latenza e condizioni della misura
nuovi fatti veri e nuove capacità, separati dalle prove di sviluppo
rossi residui, controesempi e prossimo bisogno della stessa missione
```

### 0.9 Quando si può dichiarare raggiunto un traguardo

- **Capacità viva locale:** il ciclo naturale completo è verificato per una
  classe dichiarata, con provenienza, ablation e persistenza. Questo non
  certifica tutte le costruzioni dello stesso strato.
- **KB viva integrata:** le capacità attraversano rappresentazioni e facoltà,
  le nuove lezioni cambiano passato e futuro, il dialogo lungo mantiene il
  filo e il curriculum sa scegliere bisogni utili; tutti i gate V0–V6 sono
  sostenuti da prove e la copertura delle facoltà è esplicita.
- **Interlocuzione alla pari:** oltre all'integrazione, V7 raggiunge i criteri
  preregistrati del confronto, senza debiti di verità nascosti e con
  latenza sostenibile. Si dichiara esattamente dove la parità è osservata e
  dove la missione resta aperta.

La crescita superlineare ipotizzata nei piani è un'ipotesi da misurare, non una
condizione necessaria di ogni incremento: anche un arco generale utile può
aprire pochi casi nel campione corrente. Il criterio è l'uso trasferibile e
causalmente dimostrato. **La rielaborazione di questo piano non dichiara
raggiunta la KB viva:** fissa il percorso e le prove che possono autorizzare
quella dichiarazione.

---

Le sezioni che seguono conservano la storia sperimentale. Le diagnosi datate
vanno confrontate con il checkpoint del §0.2; i contratti corretti nel §0
prevalgono sulle indicazioni storiche incompatibili.

> ## ✅ TRAGUARDO — IL BISOGNO D'INFORMAZIONE È NOMINABILE (2026-09-01)
>
> «Non ho capito» e «ho capito, ma non possiedo il dato» non collassano più
> nello stesso muro. Il turno produce ora un `information_need` tipato:
> `knowledge/value(Relazione, Entita)` oppure
> `reference/antecedent(Superficie, Ordine)`. La risposta cita ciò che è stato
> riconosciuto e chiede esattamente ciò che manca. Superfici interrogative,
> ordinali, strategie e pezzi di resa sono KB e superano prove di
> addizione/retrazione a runtime. Il risultato è **meta-capability-only**: i
> `!assert` dei test provano l'architettura KB-first, non sono lezioni valide del
> `LEARN_PROTOCOL`; il prossimo training reale dovrà fornire fatti veri in
> lingua naturale e verificare replay, transfer e persistenza.
>
> Ipotesi nuova: la supercomprensione non richiede di scegliere subito una
> lettura completa; richiede di modellare nello spazio logico la **differenza
> minima fra stato corrente e goal**. Quando quella differenza è un oggetto,
> comprensione, metacomprensione e autocrescita diventano lo stesso ciclo:
> prova → arresto → bisogno → domanda/acquisizione → replay.

> ## ✅ TRAGUARDO — LO SPAZIO DEL DISCORSO (2026-08-31)
>
> **Da conservare: è la prima volta che parrot0 ricorda *che cosa* è stato
> nominato e *in che ordine*, e che un'espressione può riferirsi a quella
> memoria invece che a una parola.**
>
> ```text
> > Il libro rosso è sul tavolo.      ->  located_in(book_red, tavolo)
> > Il quaderno blu è sulla mensola.  ->  located_in(quaderno_blue, mensola)
> > Dov'è il primo?                   ->  tavolo        (prima: muro)
> > Dov'è il secondo?                 ->  mensola       (prima: muro)
> ```
>
> ### Che cos'è, esattamente
>
> Poco, di proposito: `discourse_referent(Ordine, Chiave)`. Una cosa nominata e
> la sua posizione nel discorso. Testa e proprietà si ricavano dalla chiave (G2);
> determinante, span e superficie originale non ci sono ancora (G5).
>
> Ma è la prima **memoria del dialogo che non è una lista di frasi**: è una lista
> di *cose*. Fino a ieri parrot0 conservava turni; ora conserva referenti.
>
> ### A che cosa serve, e che cosa abilita
>
> Non è una funzione in più: è il posto a cui si attaccano cose che prima non
> avevano appiglio.
>
> | abilita | perché prima non si poteva |
> |---|---|
> | **coreferenza** — «quello», «l'altro», «quello di prima» | non esisteva l'oggetto a cui riferirsi: F03 era la famiglia peggiore del corpus, 24 muri su 30 |
> | **ellissi** — «E il secondo?» senza ripetere il verbo | il turno ellittico non ha entità da nominare: deve prenderla dal discorso |
> | **correzione** — «no, quello rosso l'ho spostato» | correggere richiede di individuare *che cosa* si corregge, non solo che si corregge |
> | **ambiguità dicibile** — «Quale? …» invece di un muro | serve saper elencare i candidati, cioè averli |
> | **il soggetto eliso** (SC5) e **l'apposizione** | entrambi recuperano un ruolo da qualcosa già introdotto |
> | **la domanda di seguito** — «e dove si trova adesso?» | «adesso» presuppone una cosa di cui si stava parlando |
>
> Ed è la precondizione dichiarata di **GD4** (riferimento cross-turn) e di
> **D37/G4-G5**: il referente con proprietà, e il referente che sa ridirsi.
>
> ### Come è stato costruito — le tre cose che hanno deciso l'esito
>
> Vale più del risultato, perché sono riusabili:
>
> 1. **Il punto di strozzatura condiviso.** Le vie che imparano un fatto sono
>    più d'una — lo schema dichiarato, la copula binaria, il locativo — e la
>    prima versione agganciava i referenti a *una*. Misurato: il locativo
>    italiano non registrava niente, e metà del dialogo restava senza memoria.
>    L'osservazione sta ora in `p0_learn_source`, che **tutte** attraversano
>    perché registrare la provenienza è ciò che ogni via fa comunque. *Un
>    referente è esattamente questo: una cosa nominata, e quando.* Cercare il
>    punto che tutti attraversano invece di enumerare i chiamanti è la stessa
>    mossa della fase pura di SC2-B.
> 2. **Quale posizione introduca un referente è una politica, non una scelta del
>    C.** Registrando *ogni* argomento, «il secondo» diventava il **tavolo**
>    invece del quaderno: in «Il libro rosso è sul tavolo» sono nominati due
>    oggetti, ma quello di cui si parla è il primo. `referent_arg_position/1` lo
>    dichiara, e una relazione con un'altra geometria costa una riga.
> 3. **La superficie da dichiarare è quella che sopravvive al percorso.** «primo»
>    arriva al matcher come «prime» — la canonicalizzazione lo traduce — e la
>    forma col determinante non combacia più. È la **terza volta** che questa
>    lezione si presenta (dopo le cue di SC2-B e le locuzioni di SC2-D): finché
>    la canonicalizzazione non conserva anche l'originale (G5), una classe di
>    superfici deve tenere *entrambe* le forme.
>
> ### Il limite onesto
>
> È la **prima forma**, non la forma finale. Non c'è ancora il determinante (che
> distingue introdurre da riprendere), non c'è lo span, non c'è la superficie
> originale — quindi parrot0 sa che il libro rosso è stato nominato per primo e
> non sa ancora ridirlo «il libro rosso». E due referenti con la stessa testa si
> distinguono per proprietà (G2) ma non hanno ancora identità propria.


> ## ⛔⛔ IL CASSETTO SENZA MANIGLIA — il problema che teneva ferma la comprensione universale
>
> **Scoperto e misurato il 2026-08-31. Scritto in tutti i piani perché è la
> giunzione da cui dipendono `universal-input`, `universal-comprehension` e il
> fronte SC/GD insieme.**
>
> ### Il problema, in cinque righe di transcript
>
> ```text
> > Il libro rosso è sul tavolo.   ->  Learned: located_in(book_red, tavolo).
> > dove si trova il libro rosso   ->  muro
> > dove si trova book_red         ->  Tavolo.      ← solo col nome INTERNO
> > Il gatto è sul tetto.
> > dove si trova il gatto         ->  Tetto.       ← una parola sola: funziona
> ```
>
> Il **lettore** lega un *sintagma*: unisce i token fino al confine di sintagma e
> produce una chiave sola (`book_red`). La **domanda** provava un token alla
> volta — `located_in(il,?)`, `located_in(libro,?)`, `located_in(rosso,?)` — e
> non provava **mai** la frase intera. Il fatto c'era e non era raggiungibile.
>
> > **parrot0 imparava sotto un nome che non sapeva più pronunciare**, e ogni
> > entità di più di una parola finiva in un cassetto senza maniglia.
>
> ### Perché era il collo di tutto
>
> Due giri di insegnamento massiccio — 276 forme reali entrate parlando,
> verificate in processo nuovo — avevano mosso **+11 turni su 360**. Non perché
> il metodo fosse debole: perché un turno riesce solo se tengono **insieme**
> superficie, forma della domanda, nome dell'entità, riferimento, fatto e
> realizzazione, e il nome dell'entità cedeva sempre. I referenti multi-parola
> («il libro rosso», «il quaderno blu», «il treno notturno») sono la norma del
> parlato, non un caso limite — ed è anche il motivo per cui la coreferenza era
> la famiglia peggiore del corpus: non aveva **niente a cui attaccarsi**.
>
> `book_red` non è un nome scomodo: è un nome che ha **perso informazione**.
> Testa fusa col modificatore, determinante buttato, ordine invertito, lingua
> cambiata a metà — e ogni perdita chiude una porta diversa (chiedere «quale
> libro?», distinguere «un libro» da «il libro», risolvere «il primo»,
> ripronunciarlo come è stato detto).
>
> ### Il piano di soluzione — la giunzione in cinque gradini
>
> L'invariante che li governa tutti:
>
> > **ciò che si impara da una frase dev'essere interrogabile con la stessa
> > frase, e ridicibile come è stato detto.**
>
> | # | gradino | stato |
> |---|---|---|
> | **G1** | **La domanda prova i sintagmi che il lettore ha costruito.** Non un secondo indice: le *stesse tre cose* del lettore — confine di sintagma (`np_closer/1`, conoscenza), caduta del determinante, la stessa `p0_join`. Additivo: i passaggi per token restano. | ✅ **FATTO** 2026-08-31 |
> | **G2** | **Testa e proprietà.** «il libro rosso» → testa `libro` + proprietà `rosso`, così «il libro» combacia e «di che colore è il libro» risponde. Dov'è la testa è **conoscenza** (`noun_phrase_head_position(Language, first \| last)`), non una regola nel C. | aperto |
> | **G3** | **Il referente.** Un'entità introdotta occupa una **posizione nel discorso**: parrot0 ricorda che cosa è stato nominato e in che ordine, e «il primo»/«il secondo» ci si attaccano. Quale posizione di un fatto introduca un referente è una **politica** (`referent_arg_position/1`), non una scelta del C. | ✅ **FATTO** 2026-08-31 (prima forma: ordine + chiave; determinante e span restano G5) |
> | **G4** | **La coreferenza si attacca al referente.** «il primo», «quello», «l'altro» diventano `referent_same/3` — una **relazione**, non una fusione. | aperto |
> | **G5** | **Il referente sa ridirsi.** `referent_surface/3`: rispondere «il tavolo» come è stato detto, non `tavolo`. | aperto |
>
> ### Come si misura che funziona
>
> **Non con una percentuale sul totale: con una famiglia che chiude.** Il gate è
> che il dialogo `gd1_011` — cinque turni, due oggetti, un riferimento — passi
> **da capo a fondo**. Una catena che regge vale più di dieci punti sparsi,
> perché dieci punti sparsi non provano che nessuna catena regga.
>
> ### La forma ricorrente, che è la lezione vera
>
> È la **terza volta** che compare lo stesso difetto sotto un vestito diverso:
> D33 (un'interpretazione congelata perché la KB non può richiamare la lettura),
> D35 (una chiave costruita da un percorso e non dall'altro), D37 (una struttura
> costruita da un percorso e ignorata dall'altro).
>
> > **Due percorsi che devono accordarsi, e non condividono l'oggetto su cui
> > accordarsi.**
>
> Prima di aggiungere una capacità, la domanda da farsi è: *chi altro deve
> accordarsi con questa, e su che cosa?*
>
> E la stessa mossa, un piano più su, è **l'unificazione fra zone della KB**
> (D38, §18.43): far parlare aritmetica e sociale, prosa e geografia, non è
> ingegneria di dettaglio — è la condizione perché emergano abilità che nessuno
> ha progettato. Differenziarsi non basta: le parti differenziate devono potersi
> parlare.
>
> Dettaglio completo: `docs/plans/frontier-kb-natural-dialogue.md` §18.40 (D35),
> §18.42 (D37) · referto
> `docs/labs/apprendimento-assistito/2026-08-31-perche-non-cresceva.md` · coda
> `LEARN_TODO.md` GD9.


## 1. Il pensiero da rendere vero

La KB è davvero viva quando, davanti a una frase che non sa comprendere, si può
dire a parrot0:

> Vedi, questa forma significa quest'altra cosa; qui il primo nome è chi agisce,
> il secondo è ciò su cui agisce.

e parrot0 non si limita a memorizzare quelle parole. Deve:

1. proporre una nuova lettura candidata;
2. ripetere con parole proprie che cosa ha capito;
3. rileggere la frase che prima falliva;
4. riuscire su frasi nuove con la stessa struttura;
5. distinguere un controesempio;
6. conservare la lezione con provenienza;
7. poterla correggere o dimenticare parlando.

Questa è la forma operativa di “bene, ho capito”. Un riconoscimento verbale, un
fatto aggiunto o la risposta esatta all'esempio insegnato non bastano.

La scommessa del progetto resta plausibile: il C può essere l'esecutore minimo
di meccaniche stabili e una KB viva può contenere lessico, costruzioni, ruoli,
regole, procedure e criteri di scelta. Ma una KB grande non è automaticamente
una mente generale. La capacità decisiva è **acquisire nuove astrazioni e farle
comporre tramite il dialogo**.

## 2. Che cosa ha stabilito l'esperimento iniziale

Il primo esperimento teacher→parrot0 ha usato soltanto turni di chat normali,
in processi isolati, senza `!assert`, senza MCP, senza modifiche a C o `.p0` e
senza salvare la sessione nella KB del repository.

Il risultato non è né “funziona” né “non funziona”. La frontiera è precisa:

| Livello insegnato parlando | Esito attuale |
|---|---|
| membro di una classe grammaticale | forte: uso immediato, transfer e retract |
| verbo relazionale nuovo | forte nel frame soggetto-verbo-oggetto già noto |
| regola unaria e catena inferenziale | forte |
| regola relazionale con tre variabili | forte nel registro `if … then …` noto |
| congiunzione nuova | forte: cambia il parsing al turno successivo |
| nuova frase per un intento noto | forte |
| nuovo verbo con cui insegnare | forte e riflessivo |
| nuova risposta per un intento noto | forte |
| prosa lunga | parziale e dipendente dall'instradamento e dalle forme |
| parafrasi interrogative | fragile |
| “X significa Y” come equivalenza operativa | assente |
| nuova costruzione con ruoli | assente come atto didattico generale |
| procedura descritta in prosa | assente; oggi può produrre un misclaim |
| ragionamento situazionale sugli scacchi | assente nei casi provati |

La scoperta centrale è questa:

> parrot0 sa imparare quando la lezione è già espressa in un metalinguaggio che
> conosce; non sa ancora apprendere liberamente un nuovo pezzo di metalinguaggio.

Dire `glorphs is a relation verb` funziona perché “relation verb” è già un ponte
fertile. Dire `glints means glorphs` non rende `glints` utilizzabile come
`glorphs`. Dire una nuova procedura di conversione non la compila: la frase
viene scambiata per una richiesta di calcolo e produce numeri errati.

Questa frontiera impedisce oggi di promettere che ogni fallimento sia
indirizzabile semplicemente parlando. È esattamente il motore che questo piano
deve costruire.

## 3. Comprensione, non frasario

Una lezione supera il gate di comprensione soltanto se passa tutti questi
controlli:

- **replay:** la frase originale prima rossa ora viene letta correttamente;
- **transfer:** almeno tre esempi mai pronunciati dal teacher usano la stessa
  struttura;
- **parafrasi:** cambia la superficie, resta il significato;
- **controesempio:** una frase quasi uguale ma semanticamente diversa non viene
  assorbita dalla regola;
- **composizione:** la nuova abilità funziona insieme ad almeno una vecchia;
- **ablation:** dimenticando la lezione, il comportamento acquisito scompare;
- **retention:** dopo altri turni la capacità resta disponibile;
- **provenienza:** parrot0 sa quale lezione sostiene quella lettura;
- **onestà:** nessun “ho capito” prima che replay e transfer siano verdi.

L'esatto prompt→risposta può essere utile come memoria episodica, ma vale zero
come comprensione universale finché non produce transfer. Nell'esperimento la
risposta insegnata per `c1→h6` non si è trasferita a `c1→g5`: è frasario, non una
regola degli scacchi.

## 4. L'unità di crescita: una transazione didattica

Ogni episodio di apprendimento assistito segue un solo ciclo osservabile:

1. **Fallimento conservato.** Input, risposta, modulo vincitore, letture
   concorrenti e stato della KB vengono registrati; nulla viene sovrascritto.
2. **Tipizzazione del gap.** Il sistema distingue almeno lessico, denotazione,
   costruzione, legame dei ruoli, composizione, coreferenza, scope, regola,
   procedura, risposta e conflitto fra intenti.
3. **Domanda utile.** Parrot0 non chiede genericamente “vuoi insegnarmelo?”: dice
   quale informazione gli manca e in quale forma sa riceverla.
4. **Spiegazione del teacher.** La lezione arriva come lingua naturale normale,
   eventualmente con esempi e controesempi; non come `.p0` travestito.
5. **Candidato in quarantena.** La nuova conoscenza non entra subito nella KB
   consolidata. Vive in un contesto temporaneo e conserva la spiegazione fonte.
6. **Rilettura.** Il turno originale viene riprocessato con il candidato.
7. **Prove nascoste.** Il teacher genera esempi di transfer, parafrasi e casi
   negativi che non erano contenuti nella spiegazione.
8. **Confronto.** Parrot0 espone lettura precedente, lettura nuova e motivo della
   scelta. Un pareggio resta ambiguità, non diventa certezza arbitraria.
9. **Promozione o conservazione del fallimento.** Solo una lezione verificata
   entra nella KB viva. Se fallisce, candidato, trace e ipotesi restano nel lab.
10. **Retract parlato.** Il teacher deve poter revocare la generalizzazione e
    osservare che la capacità collegata scompare.

La transazione deve essere atomica: una lezione non può lasciare mezza grammatica
attiva dopo avere fallito i controlli.

## 5. La scala del metalinguaggio insegnabile

Il bootstrap non si risolve con un dizionario enorme. Ogni lezione nuova deve
potersi ancorare a qualcosa che parrot0 sa già distinguere. La scala da aprire è:

1. **membri:** “`movo` è un quantificatore universale”;
2. **denotazioni:** “`glints` qui ha lo stesso senso relazionale di `glorphs`”;
3. **ruoli:** “prima di `glints` c'è l'agente, dopo c'è l'oggetto”;
4. **costruzioni:** «in questa frase il primo nome indica chi agisce e il
   secondo chi riceve l'azione»;
5. **composizione:** coordinazione, modificatori, relative e apposizioni;
6. **scope e riferimento:** negazione, condizionali, quantificatori, pronomi;
7. **regole:** antecedenti multipli, eccezioni, causalità e vincoli;
8. **procedure:** una descrizione diventa un piano eseguibile con slot tipati;
9. **atti dialogici:** cosa una formulazione chiede di fare, ricordare o
   correggere;
10. **criteri di scelta:** come preferire una lettura senza nascondere le altre.

Oggi i gradini 1 e alcune isole dei gradini 4, 7 e 9 sono vivi. Il ponte generale
fra 2–6 manca. È il prossimo nucleo, non un altro lotto di risposte di dominio.
Il §6 traduce questa scala nell'inventario operativo di ciò che manca, e ne fa la
precondizione di qualunque sessione lunga.

## 6. Il cancello duro: nessuna sessione lunga prima del metalinguaggio

> ⛔ **PRIORITÀ, indicata da F. il 2026-08-28.** Fra tutti gli strati di questo
> capitolo, **M15 — le forme della domanda — va sbloccato per primo e il prima
> possibile** (§6.2b). Non è una preferenza di ordine: è che M15 è l'unico che
> rende *invisibili* i risultati di tutti gli altri. Si possono insegnare fatti
> nuovi tutto il giorno, ma se non si può insegnare **come si chiedono**, quei
> fatti non rispondono a nessuno — e il giro di addestramento sembra fallito
> quando invece era riuscito.
>
> La misura che lo dice: `270` formulazioni interrogative scritte a mano coprono
> `136` relazioni, quasi tutte in inglese. `which colors are used in chess`
> risponde; `which color is used in chess` no. Oggi la differenza si colma
> **editando un file**, e questo contraddice direttamente il criterio del
> progetto: *l'addestramento via prompt è lo standard e non va mai bypassato.*

**Regola.** Le dieci ore del §7 — e a maggior ragione qualunque teacher che giri
come processo continuo — non si avviano finché gli strati di meta-comprensione
elencati in questo capitolo non sono chiusi: **M0–M20**, incluse le aggiunte
del §6.2b, con i gate di revisione e persistenza del §0. Le indicazioni
storiche M0–M14 non escludono gli strati scoperti successivamente. Non è un
ordine dei lavori
preferibile: è una conseguenza di che cosa una sessione lunga può misurare. Prima
del cancello, dieci ore non misurano l'apprendimento; misurano la copertura delle
forme che il motore già supportava, moltiplicata per la pazienza del teacher nel
tradurre tutto in quelle forme.

Le tre ragioni per cui il cancello è duro:

1. **La durata non crea astrazione.** Se un fallimento non è indirizzabile
   parlando, resta non indirizzabile alla decima ora come alla prima. Aggiungere
   turni aggiunge fatti, non forme.
2. **Ciò che non è insegnabile viene sostituito da frasario.** Il teacher, per
   non fermarsi, ripiega sulla risposta esatta: è il caso `c1→h6` che non
   trasferisce a `c1→g5`. Dieci ore così producono una KB grande e una
   comprensione ferma, e la crescita del numero di fatti nasconde esattamente
   questo.
3. **Senza tipizzazione del gap e quarantena, l'errore si consolida.** Una
   sessione lunga scrive. `organism(metabolism)` e la risposta `4` sull'arrocco
   mostrano che oggi il sistema non distingue da sé un'estrazione falsa da una
   vera, né un muro da una risposta irrilevante. Il costo di accorgersene cresce
   con la durata della sessione.

### 6.1 Che cosa vuol dire «addestrabile a voce su tutto»

L'obiettivo non è un elenco di argomenti: è la chiusura di cinque modi di
fallire. Per ciascuno deve esistere un atto didattico pronunciabile in chat
normale che produca una capacità generale e verificabile.

| Il fallimento | Che cosa il teacher deve poter dire | Strati |
|---|---|---|
| **non sa** un fatto | l'asserzione, in prosa normale | chiuso (A0) |
| **non conosce** una forma | «questa forma significa quest'altra», «qui il primo nome è chi agisce» | M2–M6 |
| **non comprende** una struttura | «qui questo si riferisce a quello», «questo vale solo se…», «X perché Y» | M7–M9 |
| **non sa fare** qualcosa | «per fare X, prima Y poi Z», «quando dico così ti sto chiedendo di…» | M10, M11 |
| **non sa di non sapere** | «quella risposta non c'entrava», «qui non stai scegliendo, stai indovinando» | M0, M12, M13 |

L'ultima riga è quella che manca di più ed è quella che rende sicure tutte le
altre: finché parrot0 non sa dire *che tipo* di lacuna ha, la diagnosi resta a
carico del teacher — e un teacher automatico, che è il punto d'arrivo del §8, non
può che sbagliarla su scala.

### 6.2 Inventario degli strati mancanti

Ogni voce dichiara che cosa manca, l'atto didattico che deve funzionare e il gate
che ne autorizza la chiusura. Nessun gate si supera con gli esempi usati per
implementarlo.

**M0 — Onestà del turno: muro, risposta, falso.**  
*Manca:* la distinzione fra «non so», «so e rispondo» e «ho prodotto qualcosa che
non è ancorato a nulla». Evidenze: `4` per l'arrocco, `-16.6667` per la
conversione, la descrizione generale degli scacchi data alla domanda sul cavallo,
la prosa didattica trasformata in racconto.  
*Atto didattico:* «quella risposta non c'entrava con la domanda» deve produrre un
fatto sul turno — tipo di errore, modulo vincitore, letture concorrenti — non una
scusa.  
*Gate:* nessuna risposta priva di ancoraggio; ogni risposta sa esibire su
richiesta la conoscenza che l'ha prodotta; false-understanding rate a zero su una
batteria nascosta. **È il prerequisito di tutti gli altri: senza M0 ogni metrica
successiva è inaffidabile.**

**M1 — Un solo atto di lettura, deciso da conoscenza.**  
*Manca:* la priorità fra consumer è oggi l'ordine dei moduli. Lo stesso testo
finisce al lettore o al generatore di racconto a seconda del contenuto, e persino
`read:` viene rubato nel profilo integrato.  
*Atto didattico:* «questo è un testo da leggere, non da raccontare» — e la
preferenza deve valere per la *classe* del testo, non per quel testo.  
*Gate:* route stability pari a 1 sulla batteria di prosa, indipendentemente da
contenuto e profilo; ciò che non viene estratto è riportato come frase saltata
con il tipo di gap, mai sostituito da narrativa.

**M2 — Uso e menzione.** *(aperto — l'atto esiste, il gate non è chiuso)*  
*Fatto:* la menzione è un atto con due superfici, le virgolette e un marcatore
dichiarato (`mention_marker/1`). Il motore non nomina marcatori, classi,
articoli né copule. L'atto estende il proprio vocabolario: `"lemma" is a
mention_marker` abilita `the lemma albeit is a concession marker`, e l'ablazione
del marcatore lo richiude.  
*Manca:* la forma interrogativa (`is unless a condition marker?`); e una classe
dichiarata su una parola-funzione resta spesso conoscenza morta per i consumer,
che è il problema `greeting(ahoy)` e appartiene a M12.  
*Gate:* insegnare una proprietà di una parola-funzione attiva, usarla nello stesso
processo, ritrarla e vedere la capacità sparire.

**M3 — Denotazione ed equivalenza operativa.** *(A1: aperto — arita' e catena
sono chiuse operativamente; l'induzione no)*
*Fatto:* `construction_frame/3` con la vista `extract_frame/2`, pivot insegnabile,
retract parlato.  
*Fatto anche:* l'**inversione dei ruoli** — i nomi degli slot li dà il lato già
compreso, quindi «`X glints Y` significa `Y glorphs X`» si conserva come
`construction_frame("@O glints @S", "@S glorphs @O", glorphs)`. E l'alfabeto
delle variabili è conoscenza: `a is a rule_variable` abilita nuovi slot nella
stessa sessione.  
*Fatto, corretto dall'audit del 2026-09-02:* il **lettore** aveva davvero perso
l'arita' fissa, ma l'**atto didattico** no: `p0_explicit_pattern` conservava
`vars[2]` e rigettava ogni parafrasi ternaria. Ora anche la lezione usa il
limite condiviso `P0_MAX_SLOTS` e ricava i ruoli dal frame bersaglio attraverso
lo stesso binder del lettore, invece di assegnarli per ordine. Il verbo ternario
e la parola che collega il terzo ruolo restano conoscenza
(`ternary_relation_verb/1`, `link_word/1`), quindi una relazione a tre ruoli
nuova costa due frasi dette in chat e una parafrasi costa una terza frase. La
vista interrogativa usa lo stesso frame su ciascun ruolo. Gate corpus-driven:
`33/33`, `Transfer@3=3/3`, target non canonico, catena di due costruzioni,
contrasto e retract parlato.
Vedi [`2026-08-28-confine-addestrabilita.md`](../labs/apprendimento-assistito/2026-08-28-confine-addestrabilita.md).

*Manca ancora:* induzione dai soli esempi concreti.

*E una scoperta che cambia la mappa:* M3 e M8 erano **lo stesso muro**, e la
sonda lo ha mostrato facendolo nominare a parrot0 in due domini diversi con la
stessa frase — «I cannot align exactly two shared variables on both sides». Una
sola condizione in `p0_explicit_pattern` (`*nvars != 2 || seen[0] != 1 ||
seen[1] != 1`) bloccava il conteggio dei ruoli per M3 e l'unicita' per M8. La
meta' di M3 e' caduta; la meta' di M8 — il bersaglio congiuntivo — no.  
*Atto didattico:* «`X glints Y con Z` significa …».  
*Gate:* arità ≠ 2 e catena a due passi sono verdi; resta l'induzione held-out da
esempi che non esplicitano le variabili.

**M4 — Ruoli nominabili.**  
*Manca:* il teacher può allineare due slot, ma non *nominare* i ruoli, che è
esattamente la frase con cui il §1 apre questo piano: «qui il primo nome è chi
agisce, il secondo è ciò su cui agisce». Non può insegnare un ordine diverso da
quello della lingua corrente, né ruoli oltre soggetto e oggetto (strumento,
luogo, tempo, beneficiario).  
*Atto didattico:* la frase del §1, letteralmente, senza slot espliciti.  
*Gate:* una costruzione a tre ruoli insegnata a voce e interrogabile su ciascun
ruolo; un ordine non canonico insegnato senza toccare il motore.

**M5 — Morfologia e dualità delle domande.** *(A2 — parzialmente aperto)*  
*Fatto:* la dualità c'è già per la forma flessa — `who brinks X?` e `what does Y
brinks?` rispondono entrambe dallo stesso verbo insegnato. E la forma base è
*addressable*: `krell means brinks` la apre in una frase.  
*Manca:* la forma base non emerge da sola dalla lezione, e resta il divario
singolare/plurale (`organism(algae)` risponde a una formulazione e non alla sua
variante).  
*Atto didattico:* una sola lezione su un verbo relazionale deve aprire
dichiarativa, domanda su ogni slot e varianti flesse; le irregolarità si insegnano
come eccezioni, non come seconda abilità.  
*Gate:* da una lezione, quattro superfici held-out verdi; una flessione
irregolare insegnata separatamente e composta con la prima.

**M6 — Composizione.**  
*Fatto:* la coordinazione dell'oggetto (gen439) è composizione KB, e produce un
bundle di proposizioni invece di un'ambiguità.  
*Manca:* relative, apposizioni, modificatori, coordinazione sugli altri ruoli e
sulle relazioni.  
*Atto didattico:* «`A, che è un B, R C` dice due cose».  
*Gate:* una frase produce più proposizioni corrette, ciascuna con la sua
provenienza, su forme mai pronunciate dal teacher.

**M7 — Scope e riferimento.**  
*Manca:* negazione, condizionali, quantificatori, pronomi, ellissi, e il
riferimento che attraversa i turni.  
*Atto didattico:* «qui "esso" è la luna», «questo vale solo se…», «nessun X è Y».  
*Gate:* paragrafi mai visti con pronomi ed ellissi risolti correttamente; una
negazione insegnata non deve invertire la lettura di una frase che le somiglia
soltanto in superficie.

**M8 — Ricongiungimento delle rappresentazioni.**  
*Nota (2026-08-28):* condivide la CAUSA con M3 — la stessa condizione in
`p0_explicit_pattern`, e la stessa frase quando parrot0 nomina il muro. Metà è
caduta con l'arità; il bersaglio congiuntivo no. Riprodotto su conoscenza vera:
«basalt is an igneous rock» memorizza `igneous_rock(basalt)` e «is basalt
igneous?» va a muro, cioè `occupied_square(d2)` alla lettera.  
*Manca:* `occupied_square(d2)` non soddisfa `occupied(X), square(X)`. Due
traduzioni ragionevoli della stessa nozione restano conoscenza viva e isolata, e
una sessione lunga moltiplica il debito invece di ridurlo.  
*Atto didattico:* «una casella occupata è una casella su cui si trova un pezzo;
quando un pezzo è su una casella, quella casella è occupata». Il teacher
spiega le due direzioni in lingua naturale; i predicati interni li ricostruisce
parrot0. Un solo verso insegnato non autorizza a inventare l'altro.

*Gate:* una domanda posta nella forma composta trova la risposta memorizzata in
quella scomposta e viceversa; l'ablation toglie il ponte e non i fatti.

**M9 — Struttura del discorso in prosa.**  
*Manca:* entrano enumerazioni di membri; restano fuori causalità, finalità,
processo e condizione. E l'estrazione produce atomi falsi — `organism(metabolism)`
da «fuel their metabolism».  
*Evidenza 2026-08-29, SC1-A:* il testo possiede ora unita' ordinate con span e
un primo arco di contrasto derivato da una classe insegnabile a voce;
`Transfer@3=3/3`, retract e reteach sono causali. Il gate M9 resta aperto:
SC2-A porta la coverage delle claim **di superficie** a `8/8`, con fonte,
attribuzione e status vivi, ma la normalizzazione proposizionale resta `0/8` e
le domande naturali sul contenuto `0/3`. Nessun atto di metodo/risultato/limite
e nessuna causalita' sono ancora rappresentati.
*Atto didattico:* «in un testo, "X perché Y" dice che Y causa X» — la lezione
riguarda la forma del discorso, non il tema.  
*Gate:* precisione dell'estrazione misurata, non solo la quantità: «Learned N
facts» non è una frase ammessa finché la precisione non è nota; e domande causali
sul testo, non solo di appartenenza.

**M10 — Procedure insegnabili.** *(A4)*  
*Manca:* menzionare una procedura non è distinguibile dall'eseguirla; niente slot
tipati; niente transfer su numeri nuovi.  
*Atto didattico:* «per convertire A in B, moltiplica per k e aggiungi h» non deve
produrre un numero.  
*Gate:* la frase didattica non calcola; il piano risultante lega i numeri a ruoli
e si applica a input nuovi; l'esecuzione sa esibire i passi.

**M11 — Atti dialogici.**  
*Manca:* che cosa una formulazione *chiede* di fare — rispondere, ricordare,
correggere, chiedere chiarimento, rifiutare — è oggi cablato nella scelta dei
moduli.  
*Atto didattico:* «quando dico "…" ti sto chiedendo di correggere ciò che hai
appena detto».  
*Gate:* un atto nuovo insegnato cambia il comportamento su formulazioni non
insegnate, e si ritrae.

**M12 — Criteri di scelta fra letture.**  
*Manca:* quando due letture competono, la scelta la fa l'ordine dei moduli. Il
teacher non può insegnare una preferenza, e un pareggio non è visibile.  
*Atto didattico:* «in un testo così, preferisci questa lettura» — senza cancellare
l'altra, coerentemente con la regola di non potare le strutture secondarie.  
*Gate:* la preferenza è un fatto ritrattabile; un pareggio resta ambiguità
dichiarata e mai certezza arbitraria.

**M13 — Tipizzazione del gap e domanda utile.**  
*Manca:* la classificazione del fallimento (§4, punto 2) e la domanda che ne
consegue (§4, punto 3). Oggi il muro sa suggerire un metalinguaggio preesistente
— «dì «X è un relation verb»» — e non sa dire «mi manca la costruzione», «mi manca
il referente di questo pronome», «mi manca la procedura», «ho due letture pari».  
*Atto didattico:* nessuno: qui è parrot0 che deve parlare per primo.  
*Gate:* su una batteria di fallimenti eterogenei il tipo di gap dichiarato è
corretto, e per ogni tipo dichiarato esiste almeno un atto didattico che lo
chiude. **Senza M13 il teacher automatico del §8 non è avviabile**, perché
tenterebbe la lezione sbagliata sul gap sbagliato.

**M14 — Quarantena, promozione e genealogia.** *(meccanismo trasversale)*  
*Manca:* il candidato in contesto temporaneo, le prove nascoste generate dopo la
lezione, la promozione solo se verde, il rollback atomico, e la genealogia degli
stati `active`/`superseded`/`failed`/`partial`.  
*Gate:* una lezione che fallisce le prove non lascia mezza grammatica attiva; ogni
capacità sa dire da quale lezione discende; il retract di quella lezione la fa
sparire e non tocca il resto.  
*Nota:* i fatti che servono a questo strato — `fact_source/3`, `reading_fact/2`,
`utterance/3`, i registri di gap — sono esattamente quelli che oggi finiscono
nella ricaduta di `/save` e che verrebbe facile scambiare per rumore. M14 non si
costruisce sopra a un filtro che li butta.

### 6.2b Ciò che oggi NON si può insegnare parlando — misurato al gen456

Le voci sopra sono strati progettati. Questa sezione è diversa: è ciò che è
**emerso provando**, in una sessione di giri `LEARN_PROTOCOL` fatta apposta per
far fallire l'addestramento via prompt su frasi articolate. Ogni riga ha una
misura, non un'impressione.

Va letta con il criterio di F.: *l'addestramento via prompt è lo standard e non
andrà mai bypassato.* Quindi ogni voce qui è un punto in cui, oggi, per insegnare
qualcosa **bisogna aprire un file** — e finché è così quella capacità è fuori dal
protocollo.

#### M15 — Le forme della DOMANDA non sono insegnabili. **È il blocco più urgente.**

**Fotografia storica del gen456, parzialmente superata:** `frame_for` è ora
presente, e `cue_like` estende anche superfici di condotta da un'ancora
naturale. Il gate generale resta da verificare per famiglia; vedi §0.2.

*Manca:* `answer_frame/2` — la relazione fra una superficie interrogativa e la
relazione che deve interrogare — non è raggiungibile da nessun atto didattico.
`learnable/3` ha quattro modi (`exact`, `substring`, `fill`, e la maniglia
generica `cue_for`/`reply_for` del M11) e **nessuno arriva a `answer_frame`**.

*Misura:* `270` righe `answer_frame` per `136` relazioni distinte — due
formulazioni a testa in media, quasi tutte inglesi. La conseguenza si tocca con
mano:

```
which colors are used in chess   ->  White and black.      (c'è la riga)
which color  is  used in chess   ->  I don't understand.   (manca la riga)
quale colore ha legami con gli scacchi -> Non capisco ancora.
```

La conoscenza c'è — `side_color(chess, white)` è nella KB — ma è raggiungibile
solo da quattro frasi inglesi al plurale. **Una variante singolare/plurale
richiede di editare un file.** Il tentativo di insegnarla parlando fallisce:

```
teach "quali colori" as which colors   ->  Hmm, I don't know about colors yet.
```

*Perché è il più urgente:* è il collo di bottiglia di ogni altro giro. Si possono
insegnare fatti nuovi tutto il giorno, ma se non si può insegnare **come si
chiedono**, restano invisibili — ed è esattamente la forma di conoscenza morta
che il M11 nomina («un fatto vero in KB, invisibile al comportamento»). Colpisce
per intero le lingue diverse dall'inglese.

*Atto didattico che deve funzionare:* «quando chiedo quali colori si usano in
un gioco, voglio sapere con quali colori giocano i partecipanti», oppure
insegnare una parafrasi di una domanda già compresa. La relazione e il verso
si ricavano dall'ancora e dai suoi ruoli. Se l'ancora non è capita o ha più
letture incompatibili, parrot0 deve chiedere il chiarimento pertinente.

**Rettifica del 2026-09-05:** la vecchia proposta di far nominare al teacher
`side_color` è ritirata: violava il vincolo zero di questo piano. `frame_for`
e `answer_frame_input_arg` sono dettagli dell'implementazione, non parole che
chi insegna deve conoscere. Il motore della porta esiste; la missione verifica
che sia raggiungibile con spiegazioni naturali e ruoli corretti.

*Gate:* una relazione qualunque fra le 136 diventa interrogabile in una lingua
nuova senza toccare un file, e la prova si fa su una relazione che chi implementa
non ha usato per implementare.

#### M16 — La voce di parrot0 è insegnabile solo al 16,5%

*Manca:* le forme italiane. `kb_response_slots` preferisce già
`response_template/3` per la lingua del turno e ricade sulla `/2` inglese — il
meccanismo è a posto. È la copertura a non esserci.

*Misura:* `141` famiglie su `854` hanno una forma italiana. In una sessione
italiana **più di quattro famiglie su cinque rispondono in inglese**, ed è ciò
che F. ha visto:

```
you> come stai
Sto bene, grazie. Come posso aiutarti?
you> genera un template html
I understood the request — produce «template html» — but I don't have …
```

*Nota di metodo, perché è il modo sbagliato più tentante:* la correzione **non**
è un ramo `it ? "…" : "…"` nel C. Al gen450 ne è stato tolto uno; riaggiungerlo
per andare più veloce farebbe regredire l'esperimento anche a test verdi.

#### M17 — Un periodo articolato perde le sue subordinate

*Misura:* baseline su quattro frasi vere, quattro modi diversi di fallire.

| frase | esito | stato |
|---|---|---|
| `water boils …, but it boils lower … because …` | **racconto inventato** | chiuso al gen454 |
| `although mercury is a metal, it is liquid …` | muro, perdeva `metal(mercury)` | chiuso al gen455 |
| `copper is a metal that conducts electricity` | muro, perde `metal(copper)` | **aperto** |
| `if a substance is a noble gas then …` | `holds(it_does_not_react_easily) :- …` | **aperto** |

*Resta aperto:* la **relativa** (`… that conducts …`) non viene scomposta, e con
essa si perde anche la classe che stava in chiaro nella principale. E la
condizionale su classi produce atomi opachi congelati: la regola è vera come
stringa e inapplicabile a `helium`, cioè non è una regola.

*Nota:* la concessiva italiana si scompone (il meccanismo del gen455 è neutro
rispetto alla lingua) ma nessuna delle due metà viene poi capita, perché il
congiuntivo «sia» non ha lettore.

#### M18 — Una lezione può andare a segno e non vedersi

*Era:* il ramo generico di `try_teach_form` componeva la conferma e non la
scriveva mai in `out`. Chi insegnava riceveva la risposta del turno *precedente*:

```
you> ciao
Ciao!
you> learn "sono a pezzi" as a cue for mood_tired
Ciao!                                    <- la lezione era andata a segno
```

*Perché va tenuto scritto anche se è chiuso (gen456):* è il difetto peggiore per
questo piano in particolare. Un protocollo che verifica l'apprendimento
**leggendo le risposte** non può funzionare se l'atto didattico è muto: ogni
lezione sembra fallita, e chi insegna corregge una cosa che non era rotta. La
stessa forma — un messaggio composto e mai emesso — è stata trovata due volte in
due punti diversi (`mod_role` al gen452, qui al gen456), quindi vale come classe
di difetto da cercare, non come incidente.

#### M19 — Moduli che rubano il turno a una lezione

*Misura:* tre casi trovati provando, tutti della stessa forma — un modulo
risponde plausibilmente a un turno che non ha capito, e la lezione è persa.

- `spanish is a romance language` → «*I can translate most of it, but I don't
  know the Spanish for «romance»*». La parola *spanish* fa vincere il traduttore
  su un'asserzione di classe. **Aperto.**
- `what is your designation` → «*I don't have any of my own — I'm parrot0, an
  AI*». Chiuso al gen453 con `self_attribute_request`.
- qualunque frase `X, but it … because …` → un racconto. Chiuso al gen454.

*Gate:* un turno che asserisce un fatto non può essere vinto da un modulo che non
lo asserisce. Finché non c'è, ogni giro di addestramento paga un pedaggio
casuale che dipende dalle parole scelte.

#### M20 — parrot0 mostra la propria forma interna

`what is white` risponde:

```
side_color(chess, white); chess_pawn_direction(white, north); white is a light_color.
```

Non è sbagliato ed è persino informativo, ma è la KB nuda con il punto e virgola.
Per questo piano conta perché **chi insegna non deve dover leggere predicati**:
il vincolo n.1 del `LEARN_PROTOCOL` dice che un esperto del dominio che ignora lo
schema della KB deve poter formulare la lezione — e deve poterne leggere la
verifica. **Aperto.**

### 6.3 Quando uno strato si può dichiarare chiuso

I nove controlli del §3 valgono per la singola lezione. Uno **strato** è chiuso
solo quando quei controlli passano su una lezione che chi ha implementato lo
strato non aveva mai visto: forma nuova, dominio nuovo, e lingua diversa dove ha
senso. Finché il gate è verde soltanto sugli esempi che hanno guidato
l'implementazione, si sta misurando l'implementazione e non la capacità.

Vale anche il verso opposto, e non è negoziabile: uno strato non si chiude
cablando in C il vocabolario che gli serve. La domanda del progetto — «parrot0 può
impararne un nuovo membro domani, senza ricompilare?» — è parte del gate, non un
commento a margine.

### 6.4 Che cosa resta lecito prima del cancello

Il cancello vieta il **consolidamento** su sessione lunga, non la misura. Restano
autorizzati, e sono anzi il lavoro corrente:

- sonde brevi e limitate nel tempo, per fare emergere il gap successivo;
- sessioni di profondità come
  [`depth-session-01.md`](../labs/apprendimento-assistito/depth-session-01.md),
  che raccolgono fallimenti senza promuovere nulla nella KB del repository;
- la coda dei residui, ordinata per livello di astrazione e non per fastidio;
- uno strato alla volta, con il suo test e la sua evidenza.

Non sono autorizzati prima della chiusura: un teacher che gira a lungo scrivendo
nella KB, l'estrazione massiva da dizionari o corpora, e qualunque dichiarazione
di avanzamento basata sul numero di fatti appresi.

## 7. Esperimento di dieci ore

Questo capitolo descrive una sessione che si esegue **dopo** il cancello del §6.
Prima di quel punto il curriculum resta una batteria di misura da eseguire a
sonde brevi, senza consolidare nulla nella KB del repository.

Le dieci ore non sono dieci “generazioni” e non hanno un traguardo cosmetico.
Sono una singola sessione longitudinale: stesso processo, stessa KB di sessione,
curriculum progressivo, checkpoint e prove nascoste.

| Ora | Lavoro del teacher | Gate dell'ora |
|---:|---|---|
| 0 | baseline stratificata: prosa, scacchi, istruzioni, anafora, domande | mappa dei fallimenti e nessuna risposta falsa ignorata |
| 1 | lessico, denotazioni, sinonimi e morfologia | uso/menzione, varianti flesse e retract |
| 2 | domande e slot inversi | stessa relazione interrogabile in entrambi i versi e in parafrasi |
| 3 | asserzione↔domanda e relazioni | verbi nuovi trasferiscono a entità e tempi nuovi |
| 4 | coordinazione, relative, apposizioni | una costruzione insegnata produce più proposizioni corrette |
| 5 | pronomi, ellissi, quantificatori, negazione e scope | riferimenti corretti su paragrafi mai visti |
| 6 | regole, eccezioni, causalità e spiegazioni | conclusione più proof trace, incluso controllo negativo |
| 7 | procedure insegnate | la frase didattica non viene eseguita; il piano trasferisce a numeri nuovi |
| 8 | acquisizione da prosa reale | fatti, relazioni e regole con provenance; domande sul testo |
| 9 | dominio integrato, iniziando dagli scacchi | posizioni nuove risolte dalle regole, non da risposte memorizzate |
| 10 | replay ostile, ablation e consolidamento | checkpoint riproducibile e genealogia delle capacità |

Almeno il 70% delle verifiche deve essere held-out: numeri, nomi, ordine delle
frasi e formulazioni non usati dal teacher. Ogni ora conserva anche le lezioni
fallite; sono l'inventario dei motori ancora mancanti.

### Che cosa produrrebbero davvero dieci ore oggi

Con i meccanismi attuali, dieci ore potrebbero accumulare molti fatti, regole
nel registro già noto, nuovi membri lessicali, intenti e risposte. Sarebbe una KB
più capace e utile. Non diventerebbe però equivalente a un LLM, perché i
fallimenti fuori dal metalinguaggio noto resterebbero non compilabili; il teacher
sarebbe costretto a tradurre tutto nelle poche forme già supportate o a creare
frasario.

La sessione lunga diventa l'acceleratore decisivo **dopo** che una spiegazione di
costruzione, denotazione e procedura può creare un candidato verificabile. Prima
di quel gate, aumentarne la durata misura soprattutto la copertura delle forme
esistenti.

## 8. Il teacher come processo lungo

Il teacher non deve attendere che qualcuno scelga dieci prompt a mano. Il lavoro
continuo autorizzato da questo piano è:

- campionare prosa e dialoghi ammessi dalle fonti del progetto;
- presentare un testo a parrot0 e porre domande di comprensione;
- classificare ogni risposta in corretta, muro, irrilevante o falsa;
- tentare una lezione parlata, mai una patch nascosta;
- generare transfer e controesempi;
- consolidare soltanto ciò che supera i gate;
- mettere i residui in una coda ordinata per livello di astrazione;
- preferire il gap che, chiuso, libera più famiglie di frasi;
- fermare una pista che accumula risposte esatte senza transfer.

Un grande dizionario può fornire esempi, definizioni, flessioni e contesti. Non
deve diventare il surrogato della comprensione. L'estrazione massiva comincia
solo quando un campione di forme supera replay, transfer e controllo di qualità;
altrimenti moltiplica errori come `organism(metabolism)` osservato nella prima
sessione.

## 9. Metriche che sostituiscono il conteggio delle generazioni

- **Lesson yield:** lezioni consolidate / lezioni tentate.
- **Transfer@N:** successo su N esempi nuovi per ogni lezione.
- **Paraphrase invariance:** quota di formulazioni equivalenti con stessa
  lettura.
- **Contrast precision:** quota di quasi-esempi correttamente esclusi.
- **Ablation fidelity:** quota di capacità che scompare ritraendo solo la sua
  causa.
- **Retention:** capacità ancora attiva dopo 100, 1.000 e 10.000 turni.
- **Addressability:** fallimenti per cui il teacher può formulare una lezione che
  parrot0 sa almeno rappresentare come candidato.
- **False-understanding rate:** “appreso/capito” senza prova di uso; obiettivo 0.
- **Route stability:** stesso testo acquisito indipendentemente dal modulo che
  tenta di reclamarlo.
- **Inference depth:** conclusioni nuove ottenute per composizione, con proof
  trace, non presenti come fatti.
- **Compression:** quante famiglie held-out copre una sola lezione; è la misura
  diretta dell'astrazione.

Nessun numero di generazione chiude la missione. Un milestone passa soltanto se
una batteria nascosta cresce in capacità e non in frasi memorizzate.

## 10. Tracce che non si cancellano

Ogni episodio conserva:

- testo originale e risposta originale;
- ipotesi semantiche provate, anche quelle perdenti;
- spiegazione del teacher;
- candidato KB e sua provenienza;
- replay, transfer, parafrasi, controesempi e ablation;
- motivo della promozione, del rollback o del rinvio;
- dipendenze fra la lezione e le capacità nate dopo.

Le strutture secondarie non vengono eliminate quando una strada migliore
emerge. Si marcano come `superseded`, `failed` o `partial` e restano consultabili.
Un tentativo incompleto che alza il livello di astrazione è materiale di ricerca,
non spazzatura.

## 11. Ogni incremento è committabile

**Regola.** La conoscenza e le regole che parrot0 apprende durante una sessione
si committano e si pushano **sempre**, anche parziali, anche incomplete. Non
esiste il contributo compatto: non è ammesso tenere l'apprendimento in staging
finché la sessione non è completa.

Il principio è quello dei piccoli incrementi. **Ogni cosa appresa è un piccolo
incremento**, e in qualunque momento è legittimo committare e pushare un piccolo
avanzamento della KB. Non serve che chiuda uno strato, una milestone o un'ora di
curriculum. Non serve nemmeno che sia elegante.

Questo **non** significa commit compulsivo. Non c'è un obbligo di frequenza e non
c'è un merito nel numero di commit. Significa soltanto togliere il permesso
implicito che si dava all'attesa: nessun avanzamento resta fuori dal repository
perché «è ancora poco», «lo committo quando ho finito il resto» o «da solo non si
capisce». Un incremento parziale nel repository vale più di un incremento
completo che vive soltanto nella sessione di chi lo ha prodotto.

Le ragioni sono le stesse del §10:

- una lezione non committata è una traccia persa, e le tracce non si cancellano;
- un incremento visibile può essere contraddetto, superato o ritratto da
  chiunque; uno invisibile no;
- la genealogia delle capacità si costruisce dai passi, non dal risultato;
- un lotto grande nasconde quale lezione ha prodotto quale capacità, che è
  esattamente ciò che il gate di provenienza del §3 chiede di sapere.

Conseguenze operative:

- ciò che è parziale si dichiara parziale nel messaggio di commit, non si
  trattiene fino a diventare completo;
- una lezione che ha fallito i gate si committa come `failed` o `partial` con la
  sua evidenza, coerentemente con il §10: è inventario, non spazzatura;
- una suite rossa preesistente non è motivo per trattenere un incremento
  indipendente; si dichiara il rosso, non lo si usa come lucchetto;
- il messaggio di commit dice che cosa parrot0 ha imparato e che cosa non ha
  ancora imparato, non soltanto quali file sono cambiati.

E una precisazione che vale quanto la regola: **committare tutto non autorizza a
filtrare il resto.** Una sessione salva anche fatti che non parlano del mondo —
il registro della conversazione, la provenienza delle letture, i gap aperti, i
contatori. La tentazione è chiamarli rumore e toglierli automaticamente al
salvataggio; è vietato, perché ciò che oggi sembra rumore può essere conoscenza
di ordine superiore, e un filtro la distrugge prima che qualcuno capisca a che
cosa serviva. La questione è aperta e sta in
[`session-and-provenance.md`](../session-and-provenance.md#6-il-rumore-di-sessione--questione-aperta-da-non-chiudere-con-un-filtro):
finché è aperta, ciò che sembra fuori posto si annota, non si scarta.

## 12. Milestone

### A0 — Misura iniziale (completato)

- sessione isolata teacher→parrot0;
- successo forte su classi lessicali, relazioni, regole e intenti;
- fallimenti riproducibili su equivalenza operativa, procedure, prosa integrata,
  parafrasi e scacchi situazionali.

### A1 — “Questo significa questo”

Una lezione parlata deve poter creare una trasformazione semantica candidata da
una forma con slot a un frame noto. Gate minimo:

> `X glints Y` significa `X glorphs Y`

seguito da replay, tre coppie held-out, una parafrasi, un controesempio e retract.
La regola deve conservare ruoli e provenienza; non può essere un alias di stringa
globale.

### A2 — Domande duali e morfologia

Da un verbo relazionale insegnato devono emergere forma dichiarativa, domanda sul
soggetto, domanda sull'oggetto e varianti flesse. `studies`/`study` non possono
diventare due abilità scollegate.

### A3 — Un solo atto di lettura

Qualunque input dichiarativo multi-frase deve essere offerto allo stesso lettore
prima dei generatori di racconto o analisi. Se non estrae nulla, deve riportare
le frasi saltate e i gap, non produrre narrativa. `read:` non può essere rubato
da un altro intento nel profilo integrato.

*Stato 2026-08-29:* il routing e' chiuso da D0/SC0; SC1-A conserva inoltre le
unita' oltre il workspace transiente e deriva un primo arco retorico retraibile.
SC2-A aggiunge identita' source-addressed, claim di superficie, span,
attribuzione e status/context/commitment derivati da cue multi-parola
insegnabili. A3 non equivale ancora a comprensione proposizionale: il prossimo
confine e' normalizzare il contenuto con la pipeline semantica comune e renderlo
interrogabile naturalmente senza promuovere il riportato a fatto del mondo.

### A4 — Procedure insegnabili senza misclaim

Una frase che insegna una procedura deve entrare in quarantena, mai essere
eseguita come domanda. Il piano risultante deve legare i numeri a ruoli e
trasferire a input nuovi.

### A5 — Scacchi come dominio di integrazione

Non una tabella di domande sugli scacchi: rappresentazione di pezzi, mosse,
occupazione, traiettorie, attacco, stato del re ed eccezioni. Il gate usa
posizioni generate dopo le lezioni e richiede spiegazione della legalità.

### A6 — Sessione di dieci ore

Si esegue il curriculum completo solo dopo la chiusura degli strati M0–M20 del
§6 e dei gate trasversali del §0. A1–A5 ne sono alcune milestone visibili,
non l'intero cancello. Le milestone non bastano da sole: un
gate di milestone verde sugli esempi che l'hanno guidata non chiude lo strato
corrispondente (§6.3). Il risultato è un checkpoint della KB con genealogia, non
una promozione opaca di tutto ciò che è stato detto.

## 13. Criterio di missione

Non si può essere sicuri in anticipo di eguagliare un LLM. Si può però evitare di
auto-convincersi con traguardi minimi: la missione resta falsificabile e ogni
milestone deve aumentare l'insieme delle strutture che parrot0 può apprendere
domani parlando, senza ricompilare.

Il segnale che stiamo davvero convergendo non sarà “conosce più risposte”, ma:

> davanti a un fallimento nuovo, parrot0 sa indicare il tipo di lacuna, ricevere
> una spiegazione, trasformarla in una capacità generale, provarla su casi nuovi
> e conservarla senza che il teacher tocchi il motore.

Finché questo ciclo non vale per costruzioni e procedure, la comprensione
universale è la missione aperta, non un risultato già dichiarabile.

## 14. La scala ha una dimensione che mancava: leggere senza credere

> Aggiunto il 2026-08-29, dal ciclo SC2-B
> ([report](../labs/apprendimento-assistito/2026-08-29-supercomprensione-sc2b.md),
> evidenza in [`frontier-kb-natural-dialogue.md` §18.19](frontier-kb-natural-dialogue.md)).

I dieci gradini del §5 misurano **quanta struttura** parrot0 può ricevere
parlando. SC2-B ha mostrato che quella scala ha un asse implicito, e che l'asse
era rimasto attaccato al gradino sbagliato:

> per ogni gradino della scala, esiste la domanda separata **«sotto quale
> origine vale ciò che hai appena capito?»**

Fino a SC2-B ogni facoltà che *leggeva* anche *credeva*, perché analizzare una
clausola e commettere il fatto che ne esce erano lo stesso corpo di funzione.
Non era un difetto del lettore documentale: era la forma di tutta la pipeline.
La conseguenza per l'apprendimento assistito è diretta e sgradevole — un teacher
non poteva chiedere a parrot0 *come leggeresti questa frase?* senza che la frase
diventasse conoscenza. Cioè: **non si poteva insegnare a leggere senza insegnare
a credere**, e quindi non si poteva correggere una lettura sbagliata prima che
avesse già inquinato la KB.

Con la fase pura questo diventa possibile, ed è il vero sblocco didattico del
ciclo. Tre atti d'insegnamento che prima non esistevano:

1. **Provare una lettura senza conseguenze.** Il teacher può far analizzare una
   frase, vedere il candidato, correggerlo e solo dopo decidere se vale.
2. **Insegnare la differenza fra dire e sostenere.** «Lui sostiene che X»,
   «supponiamo che X», «l'articolo riporta X» sono origini diverse dello stesso
   contenuto. Oggi ne esiste una sola implementata (`reported`); le altre sono
   SC17.
3. **Insegnare dove finisce una lettura.** La copertura è ora una coordinata:
   una frase compresa a metà non è più indistinguibile da una compresa. Il
   teacher può indicare *quale parte* non è stata letta, e quella parte ha uno
   span.

### 14.1 Il gradino 0 della scala

Il §5 comincia dai membri. In realtà sotto ai membri c'è un gradino che nessuna
lezione poteva raggiungere:

> **0. origine e copertura:** sotto quale impegno vale questa lettura, e quanta
> parte della frase ha davvero consumato.

È il gradino che rende *falsificabili* tutti quelli sopra. Un membro insegnato
male, una costruzione parziale o una procedura con uno slot non legato oggi
producono un candidato tipato invece di un fatto silenzioso — quindi il teacher
può vederli e correggerli, e il protocollo può contarli.

### 14.2 Due corollari per il metodo

**Il residuo è il curriculum.** Se ogni frase non compresa lascia un oggetto con
span, superficie e classe, allora la somma dei residui su un corpus reale *è*
l'elenco di ciò che va insegnato, ordinato per frequenza. La coda di
`LEARN_TODO.md` smette di essere solo un'ipotesi nostra e diventa una misura.
Questo è SC18/SC24 nella seconda coda.

**Ciò che si insegna deve aprire la propria domanda.** SC2-B ha derivato
«what did the investigators predict?» dalla locuzione insegnata togliendone il
complementatore, e «observed that» dallo status dichiarato nella politica di
classe. È la terza volta che il buco del consumatore (gen306) viene chiuso a un
livello nuovo con la stessa forma. La regola generale, da qui in avanti: **una
lezione non è completa finché la sua domanda non è derivabile**. Se serve un
modulo per interrogare ciò che si è appena insegnato, la lezione ha creato un
fatto morto. Questo è SC20.

### 14.3 Milestone A7 — insegnare una lettura prima di consentirla

Gate minimo, sopra A1-A5:

1. il teacher fa analizzare una frase reale e ottiene un candidato **senza**
   che la KB cambi;
2. la lettura sbagliata viene corretta parlando, e la correzione cambia il
   candidato successivo;
3. il teacher indica quale parte della frase non è stata letta, e quella parte è
   localizzabile;
4. soltanto un atto esplicito promuove il candidato, e ritrarre l'origine
   spegne esattamente ciò che ne dipendeva;
5. la stessa sequenza funziona su una seconda origine reale (discorso citato o
   ipotesi controfattuale) senza un nuovo parser.

A7 non è una milestone di conoscenza: è la milestone che rende *sicuro*
insegnare tutto il resto.

### 14.4 Milestone A8 — una lezione rivede il passato senza riscriverlo

SC40-A aggiunge la coordinata che mancava alla transazione didattica del §4.
Prima, il punto 6 «Rilettura» significava in pratica ripresentare l'input e
accumulare un altro risultato. Ora una claim conserva:

- l'osservazione immutabile (superficie, fonte, span);
- versioni interpretative con firma;
- la conoscenza da cui ogni versione dipende;
- una sola versione corrente;
- la genealogia delle revisioni.

Il gate operativo e' stato chiuso sullo stesso testo:

```text
gap -> lezione naturale -> frame -> retract parlato -> gap -> reteach -> frame
```

Nessun passo richiede un secondo `read:`. Il metodo che consuma la claim passa
da bloccato a eseguibile e ritorno; il frame riportato non entra mai nel mondo.
Una domanda prima della lezione non chiede piu' un numero per il pronome
impersonale: dichiara «riconosco la verifica, non normalizzo la proposition».
Questo e' un esempio concreto di M13 e M14 che si compongono.

A8 non chiude ancora M14: la dipendenza e' esatta per un verbo relazionale
appreso ma grossolana per schemi curati. SC40-B ha pero' chiuso la prima parte
operativa: il pass non e' piu' globale. Una vista semantica fotografata
prima/dopo produce il termine cambiato e un indice KB sceglie soltanto letture
dipendenti o gap candidati. Su 100 claim una lezione ne visita una. Restano
dipendenza completa, propagazione transitiva e budget: rispettivamente
SC41–SC43.

### 14.5 Metriche della comprensione revisionabile

Alle metriche del §9 si aggiungono:

- **StaleLeak:** versioni stale che soddisfano una vista o risposta corrente.
  Deve essere `0`.
- **RetroactiveTransfer:** testi held-out gia' osservati che diventano
  correttamente leggibili dopo una lezione / testi osservati provati.
- **RevisionRecall:** nodi cambiati dal full scan che il pass selettivo ha
  raggiunto / nodi che il full scan avrebbe cambiato. Deve essere `1` prima di
  promuovere la selezione.
- **RevisionPrecision:** nodi effettivamente cambiati / nodi visitati. Si
  ottimizza soltanto mantenendo recall 1.
- **RevisionFanout:** nodi downstream ricalcolati per lettura cambiata; va
  separato per claim, argomento, metodo, modello e sintesi.
- **PredictedRevisionAccuracy:** effetti di un candidato in quarantena previsti
  correttamente / effetti misurati dopo promozione o rollback.
- **RetroactiveGain:** aumento pesato del livello di comprensione sul corpus
  gia' letto, al netto di costo e overclaim.
- **DependencyCompleteness:** coordinate di lettura registrate come dipendenze /
  coordinate effettivamente consultate dalla fase pura. Un indice perfetto su
  un grafo incompleto non conta come recall.
- **OpportunityRecall:** gap che una lezione candidata potrebbe sbloccare e che
  l'indice delle opportunita' raggiunge / gap che il full audit sbloccherebbe.
- **EventIdempotence:** atti che non cambiano la vista e non producono revisioni
  / atti estensionalmente idempotenti provati.
- **IngestionCost vs RevisionCost:** tempo e nodi visitati per osservare il
  documento vanno separati dal costo del pass retroattivo. SC40-B ha mostrato
  un fan-out `1/100` ma un'ingestione di 100 unita' ancora costosa; sommare i
  due numeri nasconderebbe quale facolta' va migliorata.

Queste metriche impediscono due successi apparenti: chiamare «intelligente» un
full scan illimitato, e chiamare «efficiente» un indice che perde le letture che
avrebbe dovuto rivedere.

### 14.6 Milestone A9 — addestrare il passato e il futuro con due grafi

L'apprendimento assistito classico osserva soltanto il futuro: dopo una lezione
presenta tre esempi nuovi. La letteratura scientifica e la prosa complessa
rendono questo criterio insufficiente. Una definizione appresa a pagina 20 puo'
cambiare una premessa a pagina 3, la readiness di un metodo, l'interpretazione
di una figura e la validita' di una sintesi gia' prodotta.

SC40-B suggerisce una transazione didattica a due grafi:

1. il **grafo di supporto** conserva per ogni lettura corrente le conoscenze che
   la autorizzano; serve a proof, retract e propagazione dello stale;
2. il **grafo delle opportunita'** collega una conoscenza candidata ai residui
   che potrebbe sbloccare; serve a scegliere lezioni, prevederne l'effetto e
   rileggere senza replay.

Il gate A9 deve usare un mini-corpus scientifico gia' osservato, non esempi
scritti dopo la lezione:

1. accumulare almeno cento claim con residui tipati, provenienza e importanza
   distinta (premessa, passo critico, dettaglio laterale);
2. proporre tre lezioni candidate: una frequente ma laterale, una rara che
   sblocca un metodo, una che produrrebbe soltanto letture parziali;
3. prima di promuoverle, prevedere candidate raggiunte, versioni cambiate,
   consumer riaperti e costo;
4. promuovere e poi ritrarre ogni candidata, confrontando previsione ed effetto
   reale senza ripresentare il corpus;
5. scegliere la lezione successiva per guadagno retroattivo pesato, non per
   frequenza nuda;
6. conservare `pending_revision` quando il budget termina e impedire ai
   consumer stale di parlare come correnti.

A9 e' chiusa soltanto se la lezione rara ma strutturalmente decisiva batte
quella frequente quando riabilita piu' ragionamento, e se la candidata parziale
resta un gap nonostante compaia nel grafo delle opportunita'. Questo e' il ponte
fra comprensione, metacomprensione e curriculum autonomo: parrot0 non sa
soltanto *che cosa non capisce*, ma *quale insegnamento cambierebbe quali parti
del proprio modello* e puo' verificarlo contro la propria genealogia.

### 14.7 Risultato A9.1 — la lettura porta la propria prova

SC41-A aggiunge un vincolo al protocollo: non basta misurare se una claim passa
da gap a frame. La versione deve conservare **perche'** quel frame e' stato
possibile. Sul taglio `passive_core` il denominatore e' ora esplicito:

```text
predicate_license, selected_schema, morphology, auxiliary,
agent_marker, role_order, extent_policy
```

Una lezione naturale sul participio irregolare `bound` ha riletto tre claim
scientifiche pregresse (`visited(3), changed(3)`); retract e reteach hanno
percorso lo stesso fronte. La sessione non ha committato fatti del mondo. Il
report completo e' in
[`2026-08-31-supercomprensione-sc41a.md`](../labs/apprendimento-assistito/2026-08-31-supercomprensione-sc41a.md).

Da questo risultato segue una modifica dei gate futuri:

1. **SameMeaning/DifferentProof:** sostituire il supporto mantenendo il frame
   deve produrre una nuova versione, non essere scambiato per idempotenza;
2. **RootEventMinimality:** una lezione produce un solo evento sulla radice
   causale, anche quando cambiano piu' viste derivate;
3. **TypedDependencyAblation:** licenza, selezione e opportunita' devono avere
   esiti diversi e predicibili;
4. **LocalCompleteness:** ogni rapporto di completezza nomina costruzione e
   strato; `7/7 passive_core` non autorizza a dire `reader complete`;
5. **Trace-to-denominator:** il denominatore deve convergere verso le
   consultazioni realmente eseguite, non restare una lista scritta dopo il
   codice.

Il prossimo incremento A9.2 applica lo stesso contratto a marker epistemico,
modalita' ed ellissi. Prima, per richiesta del teacher, viene aperto un ciclo
ortogonale su autocorrezione KB-first e varieta' dialogica massiva: anche li'
ogni normalizzazione proposta deve portare prova, contrasto e retract, perche'
correggere un typo senza genealogia e' soltanto un altro default invisibile.

---

# ⛔ ARCHI CONNETTIVI DINAMICI — la conoscenza detta in una forma, letta in un'altra

**Nota di lettura, 2026-09-05:** questa appendice conserva esperimenti e
proposte del 2026-09-02. L'implicazione binaria ha successivamente ricevuto
un'implementazione e un test (`higher_order_lesson.p0t`); non è una prova di
chiusura di tutto l'ordine superiore. I transcript qui sotto sono evidenze di
meccanica, non fonti di verità per un nuovo training. Equivalenza, inversione,
condizioni di validità e retract seguono il contratto aggiornato del §0.4.

*Aperto il 2026-09-02 su indicazione di F. Questa sezione è condivisa da
`universal-input.md`, `universal-solver.md`, `frontier-kb-natural-dialogue.md` e
`apprendimento-assistito.md`: è una sola meccanica, e i quattro piani la usano da
lati diversi.*

## 1. Il difetto che lo ha reso necessario, misurato

```text
> teflon is a molecule              ->  Learned: teflon is a molecule.
> teflon contains fluorine          ->  Learned: teflon contains fluorine.
> tell me a molecule with fluorine  ->  «non riesco a verificare»
```

Tre turni andati a buon fine e nessuno servito a niente. La causa non è un muro
di comprensione né un vocabolario mancante:

- il percorso che **insegna** memorizza il fatto **unario** `molecule(teflon)`;
- il percorso che **legge** interroga la relazione **binaria**
  `category_member(molecule, teflon)`.

Due forme della stessa pretesa. parrot0 dichiara di aver imparato, il fatto è
davvero in KB, e la domanda formata con le stesse parole non lo trova. È **il
cassetto senza maniglia** nella sua forma più pura, e la sua gravità è che non si
vede: nessun errore, nessun avviso, solo una capacità che non si raggiunge.

## 2. La cura sbagliata, e perché lo era

La prima cura fu **una regola scritta a mano per quella coppia di forme**:

```prolog
category_member($C, $M) :- category($C), kb_fact($C, cons($M, nil)).
```

Funzionava. F. l'ha fermata:

> *«sei sicuro che la soluzione sia una "regola sola"? Secondo me la soluzione è
> un arco connettivo dinamico che possa connettere parti della conoscenza
> espresse in una forma a parti della conoscenza espresse in un'altra, attraverso
> un meccanismo di predicate-join e inferenza con predicati variabili.»*

Aveva ragione, ed è la stessa critica del mantra #2 **un livello più su**: una
regola per coppia di forme è *l'elenco degli incidenti* fatto di regole invece
che di parole. Domani la stessa frattura ricompare fra `contains` e `has_part`,
fra `located_in` e `in`, fra un'unaria e una ternaria — un ponte a mano per
ciascuna, e nessuno dei ponti si accorge degli altri.

> **Il test:** una cura per *questa* coppia non è una cura per la classe. Se la
> forma nuova costa una regola nuova, la conoscenza non si sta connettendo: si
> sta ricucendo a mano.

## 3. La forma giusta — l'arco è un FATTO, i predicati sono ARGOMENTI

```prolog
knowledge_arc(category_member, 0, category).
```

Si legge: *«nella relazione `category_member(A0, A1)`, l'argomento in posizione 0
**è il predicato** di un fatto che porta gli argomenti rimanenti»* — cioè
`A0(A1)`. Il terzo argomento è la **guardia**: dice quali predicati possono
essere promossi, perché non ogni unaria è una categoria (`stopword/1` e
`machinery/1` non lo sono).

```prolog
knowledge_alias(contains, has_part).
```

Si legge: *«le due relazioni portano la stessa pretesa»*.

Il motore (`kb_join_match` in `src/brain/10-memory-knowledge.c`) legge **una
posizione e una guardia**, e non nomina nessuna relazione. Un arco nuovo è **una
riga di KB** e vale per ogni consumatore insieme.

### Perché il join non può essere una regola KB

Vincolo reale del motore, verificato: la testa di clausola **non ammette un
predicato variabile** — `solve()` in `src/kb.c` seleziona le regole con
`strcmp(R->head.pred, g->pred)`. Si può scrivere `apply($P, $Args)` nel **corpo**
di una clausola, mai `$P($X)` in **testa**.

Perciò l'arco *dichiarato* sta in KB e il *percorso* sta in C — ed è la divisione
giusta secondo `PRINCIPLES.md`: motore fisso, conoscenza che cresce. Ciò che il C
sa fare è «promuovi l'argomento in posizione N a predicato»; **quali** relazioni,
in **quale** posizione, sotto **quale** guardia è interamente KB.

> Se un giorno il motore ammetterà una testa variabile, l'arco potrà migrare in
> KB senza cambiare una riga di conoscenza: i fatti sono già scritti nella forma
> giusta. È il criterio per capire se una meccanica è nel posto sbagliato *per
> ora* o *per sempre*.

## 4. Che cosa ha sbloccato, misurato

Il corpus **cresce parlando**, dal turno alla domanda:

```text
> tell me a molecule with fluorine   ->  «non riesco a verificare»
> teflon is a molecule               ->  Learned.
> teflon contains fluorine           ->  Learned.
> tell me a molecule with fluorine   ->  Teflon.
```

E una **categoria del tutto nuova** si apre in una frase che nessuno deve
imparare a formulare — `category/1` è nominata così apposta:

```text
> tell me a widget                ->  (muro onesto)
> widget is a category            ->  Learned.
> a florn is a widget             ->  Learned.
> tell me a widget                ->  Florn.
> florn contains quartz           ->  Learned.
> tell me a widget with quartz    ->  Florn.
```

**Effetto composto, non previsto e istruttivo:** `tell me an animal that lives in
water` prima murava; ora risponde **Amphibian**, ed è vero
(`habitat(amphibian, water)`). Metà dei membri di `animal` era scritta in una
forma che l'enumerazione non leggeva. Profondità e ampiezza si compongono: più
candidati visibili ⇒ più vincoli verificabili. Un arco non aggiunge una
capacità, **moltiplica quelle che ci sono**.

## 5. Il costo, e l'ottimizzazione che ne è nata

Un arco allarga i candidati, e ciò che era tollerabile diventa quadratico.
Misurato subito dopo: `tell me an animal that lives in lava` — un vincolo che
**nessuno** poteva soddisfare — **7,9 s**.

La causa è precisa: `member_satisfies(Membro, Valore)` chiede a `kb_fact/2` se
*qualche* relazione lega i due, e con il **predicato non legato** ogni chiamata è
una scansione dell'intera KB. Moltiplicata per ogni membro × ogni coda del
residuo: ~160 scansioni da 37 000 fatti.

Due uscite tentate, e solo la seconda è quella giusta:

1. ❌ **Pre-controllo «il valore compare da qualche parte?»** — anch'esso una
   scansione con predicato non legato: *aggiunge* lavoro invece di risparmiarne.
   Ritirato. È la trappola di questa classe: un'exit condition che costa quanto
   ciò che evita non è un'ottimizzazione.
2. ✅ **Invertire il join.** La domanda «questo membro è legato al valore?» posta
   N volte diventa «**chi** è legato a questo valore?» posta **una** volta, più
   un'intersezione in memoria:

   ```prolog
   related_to($Subject, $Value) :- kb_fact($Pred, cons($Subject, cons($Value, nil))).
   ```

   **7,9 s → sotto 1 s**, stessa risposta.

> **La regola che ne esce:** in un join con predicato variabile, il verso della
> domanda *è* la complessità. Non è una cache e non è una soglia — è la stessa
> domanda posta dove l'indice della KB può lavorare.

**Resta aperto (§L di `LEARN_TODO.md`):** `kb_fact/2` con predicato non legato è
O(fatti) per costruzione. L'inversione toglie il fattore N, non l'O(n). La cura
strutturale che F. ha chiesto — **indice per termine / kv hashing** — non è
ancora fatta: il censimento in `src/kb.c` indicizza per *predicato*, non per
*argomento*.

## 6. Il livello successivo — L'ADDESTRAMENTO DI ORDINE SUPERIORE

> F.: *«attraverso insegnamenti di ordine superiore spiegare cose come quella
> emersa, e in certi contesti che una cosa la contiene vuol dire che ne è una
> parte — così, di alto livello, sempre sfruttando il processo di inferenza
> prolog-like.»*

Gli archi del §3 li dichiara oggi un file `.p0`. Il passo successivo è che li
dichiari **chi parla**, in una frase. Non si insegna un fatto: si insegna una
**relazione fra relazioni**, e vale per inferenza dal turno dopo.

### Stato misurato (2026-09-02) — l'ordine superiore NON c'è, e mente

```text
> every knight is a noble             ->  Learned rule: noble(X) :- knight(X).   ✅
> if x contains y then y is part of x ->  Learned rule: part($V2) :- holds(x_contains_y).   ⛔
> contains means has_part             ->  «I cannot anchor that lesson yet»   (muro onesto)
> a container of something is a part of it  ->  «Subject.»   ⛔
```

- L'implicazione **unaria** fra classi è insegnabile e funziona.
- L'implicazione **binaria fra relazioni** non lo è — e il secondo turno è il
  difetto peggiore dei quattro: dichiara **«Learned rule»** per una regola priva
  di senso. Un misclaim su ciò che si è appena imparato è mantra #7, ed è peggio
  di un muro perché il turno dopo nessuno lo cerca.

### La forma da costruire, e perché è quasi tutta già lì

«se X contiene Y allora Y è parte di X» è **due frame** con **variabili
condivise**. Il legatore che serve esiste già ed è lo stesso del lettore:

```text
lato sinistro  -> p0_frame_bind -> contains(@S, @O)
lato destro    -> p0_frame_bind -> part_of(@O, @S)
variabili condivise -> i ruoli si corrispondono per NOME, non per posizione
                    -> assert  part_of($Y, $X) :- contains($X, $Y).
```

Gli invarianti del checkpoint ternario (§3 dell'handoff in `LEARN_TODO.md`)
valgono qui **immutati**, e non è un caso: è la stessa lezione a un ordine più
alto.

1. **Nessuna arità linguistica nel C.**
2. **Nessun ruolo per posizione** — il frame già compreso nomina i ruoli.
3. **Nessun vocabolo del gate nel C**: `contains`, `part of`, `se`, `allora`
   compaiono solo nel test.
4. **Target noto e univoco**: due letture ⇒ resta un gap, non si prende la prima.
5. **Il retract toglie la capacità, non la storia**: i record delle deduzioni
   restano; la loro utilizzabilità corrente richiede un sostegno ancora valido.
   Se la regola ritratta era l'unico sostegno, si invalidano anche le
   conseguenze dipendenti (§0.4).
6. **⛔ Nessun «Learned rule» senza una regola.** Se la lezione non si àncora, si
   dichiara il gap. Questa è la riga da chiudere per prima, perché oggi è attiva
   e mente.

### Il contesto, che è la parte che F. ha nominata per ultima e pesa di più

*«in certi contesti»* non è una sfumatura: è la differenza fra una regola e una
**regola con dominio**. Il semplice contenimento non dimostra una relazione
parte-tutto, nemmeno fra una scatola e un oggetto ospitato: occorre una
condizione semantica verificata che autorizzi il passaggio. La forma generale non è

```prolog
part_of($Y, $X) :- contains($X, $Y).
```

ma

```prolog
part_of($Y, $X) :- contains($X, $Y), <il contesto vale qui>.
```

dove il contesto è **a sua volta conoscenza interrogabile**, non una condizione
cablata. È il punto di contatto con `context-scope.p0` e con l'anti-isteresi:
una regola che vale ovunque è una regola che nessuno può correggere parlando.

### Ordine di lavoro proposto

1. **Togliere il misclaim** — «if X … then Y …» che non si àncora deve dichiarare
   il gap, non annunciare una regola. *Prima* di aggiungere capacità.
2. **L'implicazione binaria fra relazioni note**, con i ruoli per nome e il
   retract simmetrico.
3. **L'arco insegnato**: una spiegazione fra relazioni costruisce il mapping
   dei ruoli e la direzione dichiarata. `knowledge_alias/2` è adatto soltanto
   a equivalenze con la stessa disposizione degli argomenti; inversioni e
   implicazioni condizionate richiedono la regola corrispondente.
4. **Il dominio del contesto**, come argomento in più e non come eccezione.
5. **L'indice per termine**, perché ogni arco in più moltiplica il join (§5).

### Il gate minimo, e nessuna scorciatoia

Una lezione di ordine superiore è chiusa solo se:

- vale su relazioni **held-out** (non `contains`/`part_of`, che sono l'esempio);
- vale in **entrambe le lingue**, perché la canonicalizzazione è l'unica via;
- si **ritratta** parlando: la storia resta, le conclusioni senza più sostegno
  non valgono come conoscenza corrente, quelle sostenute indipendentemente sì;
- **non** produce un «Learned rule» quando non ha ancorato niente;
- il contesto dichiarato **restringe** davvero: fuori dal dominio la regola non
  deve concludere.
