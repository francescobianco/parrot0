# Verso una KB viva — quadro preliminare di progetto

9 settembre 2026. Documento pre-operativo derivato dal
[report sull'architettura e la missione](../reports/2026-09-09-architettura-e-missione.md),
con le osservazioni e le ipotesi di F. sulla crescita della KB, sulla colla
linguistica e sull'apprendimento del comportamento del maestro.

**Stato: proposta di inquadramento, non piano esecutivo.** Questo documento
prepara le decisioni dalle quali potrà nascere un piano. Non assegna lavori,
non stabilisce una sequenza di implementazione, non autorizza addestramento
e non dichiara risolte le questioni che espone. Il nucleo originario deriva
dalla lettura statica del report al commit `4360751`.

**Espansione dell'11 settembre 2026:** l'elaborato sulle superfici di ordine
superiore di [LEARN_PROTOCOL.md §G](../../LEARN_PROTOCOL.md#g-superfici-di-ordine-superiore--insegnare-sulle-relazioni)
aggiunge una base concreta alla tesi: insegnare relazioni *fra* relazioni.
I nuovi riscontri sono una lettura statica della codebase a `8e33072`, non una
nuova campagna sperimentale. La mappa operativa e i limiti sono nel protocollo;
qui se ne sviluppano le implicazioni per lettura, metodo e crescita cumulativa.

Le [due ulteriori intuizioni strutturali](due-strutture-kb-viva.md), sintetizzate
nel §3.6, riguardano il design della KB: aprire i ruoli dei fatti e rendere
le definizioni oggetti componibili dell'inferenza. Le condotte dialogiche
restano conseguenze possibili del substrato, non il livello della proposta.

## 1. Perché un quadro preliminare

La difficoltà descritta da F. è la distanza fra crescita locale e crescita
del soggetto nel suo insieme. Ogni integrazione può aprire una capacità
interessante, ma il territorio utilizzabile resta discontinuo: una pennellata
in una tela ancora prevalentemente nera. Aggiungere conoscenza, completare
un percorso e migliorare la capacità di apprendere non sono necessariamente
lo stesso avanzamento.

Il problema da porre prima di un nuovo piano è quindi:

> Quale organizzazione permette a una lezione di diventare comportamento
> riutilizzabile e, successivamente, strumento per comprendere altre lezioni?

Un piano che partisse direttamente da «aggiungere attenzione» o «avviare
addestramento continuo» avrebbe già scelto una risposta senza esplicitare
il meccanismo atteso. Questo quadro serve a precisare quel meccanismo e
le alternative ancora aperte. Non sostituisce l'indagine sulle capacità
reali con una nuova terminologia.

La missione resta acquisire capacità di ragionamento e proprietà di
linguaggio comparabili a quelle del maestro LLM. «Comprensione universale»
indica qui l'ambizione di estendere e comporre la comprensione attraverso
domini e situazioni, non il possesso di ogni fatto né l'infallibilità.

## 2. Le distinzioni da conservare

Il report, soprattutto nelle sezioni 10–19, suggerisce di separare tre
livelli di affermazione.

**Base osservata nell'implementazione.** La KB può contenere conoscenza
eseguibile: procedure apprese determinano sequenze di operazioni. Esistono
anche forme linguistiche, politiche, questioni e letture rivedibili.
Questa capacità è locale e disomogenea; non equivale ancora a un metodo
generale per acquisire e integrare nuove condotte.

**Ipotesi di progetto.** Rappresentazioni condivise, relazioni contestuali
apprendibili e procedure riutilizzabili potrebbero trasformare l'accumulazione
in crescita cognitiva cumulativa. L'addestramento diventerebbe produttivo
anche perché modifica gli strumenti con cui il sistema apprende ancora.

**Questione aperta.** Non sappiamo se questi meccanismi, resi sufficientemente
generali e sostenibili, possano portare parrot0 a una competenza ampia
comparabile al maestro. L'esistenza degli LLM rende concreta la possibilità
computazionale di tali capacità; non dimostra che qualsiasi architettura
o qualsiasi processo didattico possa acquisirle.

Non si attende una quantità magica di fatti. Si cerca una struttura che
possa rendere molte conoscenze reciprocamente utilizzabili. La distinzione
è compatibile con la scommessa di [PRINCIPLES.md](../../PRINCIPLES.md),
senza trasformarla in una promessa di convergenza.

## 3. La tesi che questo documento propone di maturare

> Un interprete generale può sostenere una KB viva se la conoscenza appresa
> modifica le operazioni con cui il sistema comprende il contesto, sceglie,
> ragiona, si esprime, si corregge e acquisisce altra conoscenza.

«Viva» qualifica il rapporto fra KB ed esecutore: non il file da solo e
non una presunta esperienza soggettiva. La conoscenza è operativa quando
partecipa al lavoro, riceve correzioni e cambia le possibilità successive.

In questa prospettiva, trasferire conoscenza e trasferire comportamento
non sono alternative. Una procedura, una regola di riferimento o un criterio
di scelta sono conoscenza che può diventare condotta. Il confine decisivo
è fra una descrizione di ciò che il maestro fa e una rappresentazione che
parrot0 sa effettivamente usare per farlo.

La forma più esigente della tesi è riflessiva: il sistema deve poter usare
parte di ciò che apprende per interpretare e acquisire nuove lezioni.
Non richiede che riscriva il proprio C. Richiede che l'interprete renda
componibili e apprendibili trasformazioni abbastanza espressive, comprese
quelle che partecipano all'apprendimento.

Resta distinta la qualità delle premesse. Un'inferenza corretta su un mondo
ipotetico è ragionamento; la falsità di una premessa non confuta da sola
la capacità inferenziale. Anche assumendo una KB vera, però, occorre
spiegare come il sistema selezioni e combini le relazioni pertinenti alla
richiesta. È questo il problema architetturale qui in discussione.

### 3.1 Il passaggio decisivo: una relazione può essere oggetto di lezione

`apply/2`, in [`src/kb.c`](../../src/kb.c), prende il nome già legato di un
predicato e ne risolve fatti e regole sugli argomenti ricevuti. La KB può quindi
scegliere che cosa interrogare. Sopra questo meccanismo,
[`procedures.p0`](../../kb/core/procedures.p0) dichiara `holds/3`, `holds1/2`
e `relation_note/2`: rispettivamente relazione fra cose, appartenenza a una
classe e conoscenza sulla relazione stessa.

La distinzione può essere espressa senza parlare dell'implementazione:

| Livello | Che cosa viene insegnato | Che cosa può cambiare |
|---|---|---|
| Fatto | due cose stanno in una certa relazione | una premessa disponibile |
| Proprietà | una relazione si concatena o vale nei due versi | il modo di usare molti suoi fatti |
| Legame | una relazione implica, compone o restringe un'altra | quali conoscenze diventano reciprocamente utilizzabili |
| Metodo | una situazione richiede un certo criterio o una mossa | quali conoscenze il sistema cerca e come le impiega |
| Metodo di apprendimento | una distinzione aiuta a comprendere una nuova lezione | ciò che potrà essere appreso in seguito |

I primi tre livelli hanno superfici concrete nel catalogo G; gli ultimi due
hanno precedenti parziali e una pretesa più forte da verificare. La tabella
non è una scala già percorsa interamente.

Il valore del predicato come dato è che un nuovo legame non richiede un
consumer che ne conosca il nome in anticipo. Se un lettore interroga la vista
condivisa, una nuova clausola di quella vista può cambiare le sue risposte.
Si apre così una separazione fertile fra meccanica di esecuzione e conoscenza
che determina quali trasformazioni siano pertinenti.

«Ordine superiore» ha qui un significato operativo: i nomi delle relazioni
sono argomenti di altre relazioni. Non equivale a logica di ordine superiore
senza restrizioni, né a generazione automatica di qualunque programma.
`apply/2` non sceglie un predicato ignoto: servono una dichiarazione, un
registro o un'altra inferenza che ne leghi il nome.

### 3.2 La crescita per legami cambia l'economia dell'apprendimento

Un fatto isolato aggiunge una premessa. Un ponte parametrico può rendere
utilizzabile un insieme di premesse già presenti e continuare a valere per
premesse apprese domani. La lezione non deve conoscere tutti gli oggetti ai
quali si applicherà. Il suo costo descrittivo può restare piccolo mentre cresce
il numero di domande alle quali contribuisce.

La conseguenza è che **quantità acquisita e capacità resa accessibile sono
assi diversi**. Una KB può crescere molto senza diventare più connessa; può
anche acquisire poche dichiarazioni e rendere utile molta conoscenza che già
aveva. I fatti non perdono valore: sono ciò su cui lavorano i legami. Senza
premesse, un repertorio di operatori resta potenziale; senza legami, molte
premesse restano separate.

Questo non contrappone ordine superiore e inferenza ordinaria: anche le
regole del primo ordine moltiplicano conseguenze. La novità qui è che il
legame nomina le relazioni e diventa a sua volta interrogabile, correggibile
e riusabile come conoscenza.

Si possono distinguere quattro moltiplicatori:

- **Estensione nel tempo:** un ponte appreso prima dei fatti nuovi li rende
  utilizzabili quando arriveranno, senza essere reinsegnato.
- **Riutilizzo strutturale:** una proprietà dichiarata su una relazione può
  essere ereditata da altre, nei limiti del meccanismo disponibile.
- **Costruzione di viste:** una relazione derivata può avere risposte senza
  contenere fatti propri; la sua definizione è la conoscenza da conservare.
- **Riutilizzo del risultato:** una conseguenza può diventare premessa per
  un'altra capacità. È il moltiplicatore più esigente, perché richiede che i
  consumer condividano davvero il significato delle viste.

Non segue una crescita esponenziale garantita. Ponti equivalenti possono
essere ridondanti; premesse insufficienti non producono risposte; combinazioni
formalmente possibili possono non avere senso. Inoltre il costo descrittivo
costante non implica costo computazionale costante: aumentano ricerca,
ramificazione e gestione delle dipendenze.

### 3.3 Una piccola algebra insegnabile, con impegni diversi

Inclusione, intersezione, composizione, inversione e default non sono nomi
intercambiabili per una generica «associazione». Conservare queste differenze
è ciò che permette di trasferire senza inventare conclusioni.

Un'inclusione trasporta supporto in un verso; non rende sinonime le due
relazioni. Un'intersezione richiede entrambi i supporti sulla stessa coppia.
Una composizione richiede un intermedio comune e ne conserva l'ordine.
Un'inversione scambia i ruoli; non afferma simmetria. Una proprietà ereditata
modifica il modo di interrogare una relazione; non ne importa i fatti.

La forma «whoever V is a C» aggiunge una condizione sufficiente di appartenenza:
non afferma anche che ogni membro di C debba avere un oggetto in V. La firma
«V links a C to a D» descrive i ruoli previsti; nella codebase attuale non
li impone automaticamente. Le due dichiarazioni hanno contenuto diverso
anche se nominano le stesse classi.

L'eccezione richiede un'ulteriore distinzione epistemica. Il ramo
`relation_unless` usa negazione per fallimento: il supporto è W e l'assenza
di una derivazione di Z, non la prova esplicita della falsità di Z. Aggiungere
Z può quindi ritirare una conclusione. Questo è un comportamento non monotono
legittimo per un default, ma non una verità universale su un mondo incompleto.

Infine le dichiarazioni sono additive: una nuova definizione non elimina
fatti diretti o supporti alternativi della relazione. Una restrizione su un
ramo non è un divieto globale. La futura apprendibilità della correzione deve
conservare questa differenza, altrimenti «precisare» una regola si riduce ad
accumularne altre senza rimuovere l'errore.

### 3.4 La frontiera reale: essere nella stessa KB non basta a comporre

La lettura del codice mostra un'apertura importante e una discontinuità
precisa. Le clausole dei ponti interrogano prevalentemente i loro operandi
con `apply`, sul predicato diretto, anziché richiamare `holds`.

Se una relazione è definita solo nella vista `holds/3`, il suo nome passato
a `apply` non rende automaticamente disponibile quella definizione. Anche
le proprietà consumate dagli helper della risposta polare non vengono per
questo viste da ogni regola. Dunque una catena semplice può funzionare mentre
la stessa catena con un operando derivato da un altro ponte resta muta.

**L'estendibilità del punto d'ingresso e la chiusura sotto composizione sono
due proprietà diverse.** La prima consente nuovi rami senza aggiornare il
lettore; la seconda consente di riusare i risultati dei rami fra loro.
Una KB viva ha bisogno di entrambe nei percorsi che dichiara integrati.

Altri confini confermano la stessa lezione:

| Riscontro | Implicazione progettuale |
|---|---|
| `context_alias` porta un nome di contesto ma non richiede che sia attivo | il contesto deve partecipare alla prova, non soltanto alla descrizione |
| `class_scoped_alias` applica la classe direttamente, mentre la domanda di appartenenza può usare `holds1` | riconoscere una classe e usarla come condizione devono accordarsi |
| `about` e `between_rel` applicano direttamente i verbi registrati | esplorazione e domanda puntuale possono vedere insiemi diversi di conseguenze |
| `relation_like` eredita quattro proprietà | l'ereditarietà della scheda non è universale; i metadati non sono tutti operativi |
| `relation_note` omette parti di alcune definizioni | introspezione descrittiva e spiegazione del supporto non coincidono |
| `list_composition` enumera relazioni dalla KB ma ne legge i predicati diretti | scegliere dinamicamente un nome non significa già attraversare i ponti |

Questi sono limiti delle implementazioni lette, non ragioni per rinunciare
alla tesi. Rendono più precisa l'ipotesi: la fertilità cresce quando le
conseguenze apprese attraversano i confini fra consumer mantenendo il loro
contratto. Non basta collocare tutto nello stesso albero di file.

Non ne segue una sostituzione indiscriminata di `apply` con una vista ricorsiva.
La scelta deve spiegare cicli, duplicati, negazione, budget e supporti. È
un problema di significato condiviso, prima che una scelta di chiamata.

### 3.5 I precedenti mostrano che il principio attraversa già più domini

Il circuito G non nasce isolato. La stessa struttura è rintracciabile in
punti che trattano oggetti apparentemente lontani:

| Luogo nella codebase | Oggetto reso dato | Conseguenza già motivata dal meccanismo |
|---|---|---|
| [`code-quality.p0`](../../kb/core/code-quality.p0), `criterion_finding` | il nome della misura in un criterio | cambiare il criterio può cambiare che cosa si misura senza un dispatcher per ogni misura |
| [`code-ir.p0`](../../kb/core/code-ir.p0), `ir_domain_claim`, `ir_domain_claim_basis` | il ponte fra rappresentazioni e il suo fondamento | rendere una vista utilizzabile da un altro dominio senza confondere identità e somiglianza |
| [`procedures.p0`](../../kb/core/procedures.p0), `gap_source`, `gap_covered`, `gap_record` | obblighi e criteri di copertura | la conoscenza può stabilire che cosa manchi e quale rimedio cercare |
| [`thinking.p0`](../../kb/core/thinking.p0), `thinking_operator` | l'operatore di un passo | una descrizione di condotta può diventare operazione dell'interprete |
| [`messages.p0`](../../kb/core/messages.p0), `turn_form_act` | l'atto associato a una forma e i suoi argomenti | forme diverse possono insegnare, interrogare o ritrattare usando meccaniche comuni |

La mappa puntuale dei lettori e dei limiti resta in
[LEARN_PROTOCOL.md §G.4–G.5](../../LEARN_PROTOCOL.md#g4-la-mappa-nella-codebase-dalla-frase-alleffetto).
Il [precedente di insegnamento fra relazioni](../../tests/p0t/language/higher_order_lesson.p0t)
è un test legacy su regole dette a voce, non una dimostrazione della chiusura
del catalogo G o della KB completa. Il suo risultato non viene esteso per
analogia a ogni nuovo percorso.

Questa ricorrenza suggerisce una direzione più economica di un nuovo sistema
parallelo: rendere coerenti i contratti nei punti in cui il predicato è già
un dato. La riusabilità va cercata fra rappresentazioni esistenti prima di
inventare un altro linguaggio interno per le stesse distinzioni.

### 3.6 Due aperture dello stesso livello del predicato variabile

> **gen508:** entrambe le aperture hanno una prima realizzazione, descritta in
> [due-strutture-kb-viva.md §0](due-strutture-kb-viva.md) e in
> LEARN_PROTOCOL.md §G.7–G.8. Le prove restano da eseguire.

Il criterio per cercare altre intuizioni non è «quale capacità utile possiamo
aggiungere?». È **quale parte della rappresentazione oggi fissa può diventare
conoscenza e aprire una classe di costruzioni prima inaccessibile?** La
distinzione è sostanziale: una condotta usa gli oggetti disponibili; una
struttura abilitante cambia quali oggetti le condotte possono usare.

Il documento [Due strutture abilitanti](due-strutture-kb-viva.md) sviluppa due
ipotesi, con meccanismi, appigli nel sorgente, costi e criteri di falsificazione.

**Prima: il fatto come nodo a ruoli aperti.** La relazione variabile conserva
ancora una firma posizionale. Dare identità all'istanza e rendere i ruoli
interrogabili permetterebbe di aggiungere partecipanti e qualificazioni senza
moltiplicare firme. Una proiezione sceglierebbe ruoli dichiarati, conservando
il legame fra tutti i partecipanti dello stesso evento. Il trasferimento
potrebbe riguardare mappe fra ruoli, oltre a nomi di predicati.

Non è l'invenzione dei frame: `frame_slot/3`, `state_prop/4` e le proposizioni
reificate ne sono precedenti concreti. Il gradino è far sì che il nuovo ruolo
entri nei consumer senza estenderne a mano lo schema. Un contenitore generico
che viene riletto attraverso due posizioni fisse non ha aperto quel confine.

**Seconda: la definizione come espressione componibile e analizzabile.**
Una relazione derivata potrebbe denotare una struttura i cui operandi sono
altre definizioni. La stessa inferenza potrebbe eseguirla e interrogarne la
forma, ricavando proprietà, dipendenze e trasformazioni equivalenti.

Per esempio, su relazioni binarie pure e a contesto fisso, l'intersezione di
una relazione e del suo inverso è simmetrica. Una legge della costruzione
permetterebbe di saperlo di ogni nuova relazione definita in quel modo,
anche senza fatti propri. È diverso dall'ereditare la proprietà da un nome
tramite `behaves like`: la proprietà segue dalla struttura del significato.

Il gradino rispetto a G è duplice: le definizioni possono contenere altre
definizioni, e le regole possono ragionare su questa composizione. Le famiglie
`relation_chain`, `relation_and` e simili sono il punto di partenza; il loro
annidamento oggi non uniforme è un confine concreto da aprire. Non si richiede
un dispatcher per ogni combinazione, né si assume che un termine salvato sia
già interpretabile e insegnabile naturalmente.

Insieme, le due ipotesi aprono la forma dei fatti e la forma delle definizioni.
La prima permette di estendere che cosa viene detto di una relazione concreta;
la seconda permette di estendere che cosa si può costruire con quelle
relazioni. Il predicato variabile apre quale relazione scegliere. Sono tre
gradi dello stesso criterio: spostare struttura dal presupposto del consumer
alla conoscenza che il consumer interroga.

La promessa riguarda lo spazio esprimibile e riutilizzabile. Il rendimento
computazionale resta da verificare: ruoli generici possono aumentare i join,
espressioni annidate possono aumentare la ricerca. Servono indici, condivisione
e dipendenze coerenti; non basta chiamare «universale» una rappresentazione.

La prova decisiva è una nuova lezione che estende ruoli o costruzioni e viene
usata da una lezione successiva senza un raccordo specifico. Riprendere una
domanda, scegliere un chiarimento o memorizzare una procedura possono
beneficiare di questa struttura, ma non ne costituiscono da soli la prova.

## 4. Attenzione e colla linguistica: una direzione funzionale

L'ipotesi di F. è che si stia sottovalutando il ruolo delle relazioni fra
parole e contesto. Il report (§§17–18, con i riferimenti tecnici) distingue
l'attenzione del Transformer da un semplice inventario di associazioni:
il suo interesse per questa discussione è l'elaborazione di rappresentazioni
dipendenti dal contesto e l'apprendimento delle trasformazioni coinvolte.

La domanda per parrot0 non è ancora se introdurre un particolare algoritmo
di attenzione. È se il contesto possa cambiare la lettura sulla quale il
sistema lavora, oltre a selezionare una facoltà che risponde.

La [colla linguistica](the-linguistic-glue.md) dà un nome alla continuità
ricercata fra referenti, memoria, inferenza, intenzioni e correzioni.
La possibile relazione con l'attenzione è funzionale, non un'identità
architetturale: entrambe suggeriscono di rendere produttivi collegamenti
che dipendono dalla situazione. La continuità dialogica richiede inoltre
scopi e dipendenze che attraversano i turni.

Questa direzione lascia aperte due questioni inseparabili:

- Come rappresentare e rivedere i collegamenti mentre la comprensione
  è ancora in corso, senza fissare prematuramente un'unica lettura?
- Come una lezione o una correzione modifica il criterio che ha costruito
  quei collegamenti, invece di registrare soltanto la risposta corretta?

La seconda questione impedisce di attribuire tutto a un nuovo meccanismo
di selezione. Un sistema che può collegare molti oggetti ma non può
apprendere quali collegamenti servano lascia irrisolto il problema della
crescita. Analogamente, chiamare «coerenza» il risultato desiderato non
fornisce ancora le operazioni che lo producono.

La formulazione utile dell'ipotesi di F. è dunque una coerenza semantica
attiva: conservare referenti e scopi, soddisfare richieste, incorporare
distinzioni e propagare correzioni. La sua sufficienza come guida
all'apprendimento resta da argomentare; la sola assenza di contraddizioni
non determina quale passo compiere.

### 4.1 I ponti possono cambiare la lettura, non solo la risposta

L'ordine superiore precisa l'ipotesi della colla linguistica. Una parola può
attivare una relazione locale; un legame appreso può renderla pertinente a
una richiesta formulata con un'altra relazione; una condizione può impedire
quel trasferimento in un contesto vicino. Non serve appiattire tutto in un
sinonimo globale per ottenere continuità.

La trasformazione desiderata riguarda ciò che il sistema **vede nel turno**.
Se un testo contiene una relazione locale che soddisfa un bisogno generale,
il ponte può permettere di riconoscere che il testo contiene una risposta.
Se un nuovo legame arriva dopo la lettura, può rendere pertinente un passaggio
prima inutilizzabile. Questa seconda possibilità richiede però una rilettura
con dipendenze: accumulare una seconda interpretazione senza rivedere la prima
non realizza la revisione cercata.

L'oggetto condiviso deve quindi conservare almeno l'identità dei partecipanti,
i ruoli, il legame usato, le condizioni, il supporto e lo stato della lettura.
È un'esigenza semantica da confrontare con gli oggetti esistenti, non la
proposta di aggiungere un nuovo record che li duplichi.

La pluralità di letture è informazione utile. Due ponti possono rendere una
frase pertinente a scopi diversi senza essere equivalenti. L'inferenza deve
conservare le alternative abbastanza a lungo da poter chiedere chiarimento
o scegliere con un criterio insegnabile. Il primo ponte trovato non acquista
per questo un diritto permanente sul significato della frase.

### 4.2 Un contesto operativo è una premessa, non un'etichetta

L'elaborato G suggerisce domini, classi, fatti sospesi e nomi locali. Sono
strumenti complementari: il dominio riguarda un'interpretazione, la classe
una condizione sui partecipanti, l'assunzione lo stato della conversazione,
la provenienza chi sostiene una proposizione. Confonderli crea trasferimenti
apparentemente produttivi che perdono il limite della conoscenza.

La discontinuità di `context_alias` rende osservabile il problema: registrare
il contesto e mostrarlo nella conferma non basta se la derivazione non lo
consulta. Per contro `context_fact` e `context_name` richiedono un contesto
attivo, ma questo non costituisce ancora una teoria di contesti annidati,
priorità, incompatibilità o identità attraverso i mondi.

La questione da maturare è come una lettura porti con sé la propria condizione
anche dopo che il risultato viene usato da un'altra capacità. Non basta che
il lettore iniziale conosca lo scope, se il piano riceve poi una conclusione
priva di quella condizione. Lo stesso vale per persistenza e spiegazione.

Una «conoscenza sospesa» deve poter restare disponibile senza diventare una
verità incondizionata. Questa è una forma di memoria più ricca della scelta
fra tenere e cancellare: conserva il contenuto insieme al criterio di
riattivazione. L'uscita dal contesto non deve essere confusa con l'oblio del
fatto condizionale.

### 4.3 Attenzione come ricerca pertinente fra legami apprendibili

Il predicato variabile sposta una domanda dal C alla conoscenza: quale
relazione interrogare? Non risolve da solo come sceglierla. Enumerare tutto
può produrre molte vie irrilevanti; fissare una lista privata per facoltà
richiude lo spazio di apprendimento.

Una direzione da argomentare è che domanda, ruoli, firme, famiglie, scope e
supporti contribuiscano a selezionare i legami pertinenti. Firma e famiglia
sono oggi soprattutto descrittive nel circuito G: il loro possibile uso
come criteri di ricerca resta un'implicazione, non una capacità attestata.

Il criterio di pertinenza dovrebbe a sua volta essere correggibile. Dire che
un certo collegamento non aiuta questa richiesta deve poter cambiare il
metodo di scelta, senza cancellare il collegamento dagli altri contesti in
cui è valido. Qui la colla linguistica incontra il mantra sulla condotta:
il sistema deve poter apprendere anche quando usare ciò che sa.

Questo chiarisce l'analogia funzionale con l'attenzione senza identificarla
con il Transformer. Si cerca un'elaborazione dipendente dal contesto e
modificabile con l'apprendimento; un elenco di relazioni o un cambio di
priorità nel dispatch, da soli, non soddisfano questa descrizione.

## 5. La continuità da spiegare prima di progettarla

Il rapporto fra le componenti candidate può essere rappresentato così:

```text
bisogno e contesto
        ↓
lettura rivedibile ↔ relazioni e procedure pertinenti
        ↓                         ↑
conseguenza, risposta o residuo    │
        ↓                         │
lezione e correzione → conoscenza riutilizzabile
                              ↓
                  comprensione di bisogni e lezioni successive
```

È uno schema di dipendenze, non una pipeline da implementare né una
successione di milestone. L'attenzione è rivolta a ciò che attraversa i
passaggi: identità dei referenti, condizioni, alternative, origine della
conoscenza, scopo ancora aperto. Il report individua già oggetti pertinenti;
non propone di sostituirli con un secondo sistema parallelo.

La chiusura cercata è locale e riapribile: comprendere abbastanza da usare
una conoscenza, conservarne il limite e poterla correggere. Non è la
completezza di un dominio infinito. Una struttura parametrica può servire
molti casi senza enumerarli, ma la sua utilità dipende anche dai percorsi
che sanno riconoscerla e usarla.

Questo consente di immaginare un apprendimento continuo senza attendere
prima la comprensione universale. Un nucleo limitato può accogliere lezioni
e costruire strumenti più ampi. Rimane da chiarire dove finisca questa
estensione attraverso insegnamento e dove manchi invece una meccanica
generale che richiede sviluppo.

### 5.1 Il risultato di una capacità deve poter diventare premessa di un'altra

Il ciclo descritto sopra acquista un criterio più preciso con le superfici G:
una capacità cresce con il soggetto quando il suo risultato è utilizzabile
anche fuori dal percorso che l'ha prodotto. Una risposta «sì» non dovrebbe
essere l'unica forma in cui la conoscenza derivata esiste per il resto del
sistema.

Consideriamo come schema concettuale, non come transcript già funzionante:

```text
fatti reali + legame insegnato
        → relazione derivata
        → appartenenza riconosciuta
        → condizione di una mossa
        → bisogno o domanda ulteriore
```

Ogni passaggio conserva partecipanti, condizioni e supporti. Se la classe
derivata risponde a una domanda ma non può vincolare il ponte seguente, la
catena si interrompe proprio dove la conoscenza avrebbe dovuto diventare
strumento. È ciò che rende sostanziale la differenza fra `apply` diretto e
vista condivisa rilevata nel §3.4.

La generalità richiesta riguarda il contratto di lettura, non l'uniformità
di ogni algoritmo. Un piano, un enumeratore e un verificatore possono fare
lavori diversi mantenendo lo stesso significato di una conseguenza e delle
sue condizioni. Non serve eliminarne le differenze funzionali.

### 5.2 Il metodo del maestro deve essere acquisito come trasformazione

L'LLM maestro può spiegare un caso, ma può anche esplicitare una distinzione:
quale relazione cercare, quali ruoli conservare, quale premessa controllare,
quale eccezione impedisce il trasferimento. La ricostruzione comportamentale
diventa più esigente quando il discente usa quella distinzione su casi che
il maestro non ha già risolto per lui.

Questo suggerisce tre oggetti didattici da distinguere:

- **Risposta:** sapere che una certa conclusione è vera.
- **Derivazione:** sapere quali premesse e quale legame la sostengono.
- **Criterio di costruzione della derivazione:** sapere quando cercare quel
  genere di legame e come verificarne l'applicabilità.

Non ogni lezione deve raggiungere il terzo livello. Ma un progetto che vuole
apprendere il comportamento del maestro deve spiegare dove il terzo livello
diventa conoscenza eseguibile, anziché lasciare al maestro tutte le scelte
e conservare soltanto i risultati.

Le superfici di ordine superiore offrono una via concreta per trasmettere
alcune di queste trasformazioni. Non dimostrano ancora l'acquisizione libera
di qualunque metodo, perché le famiglie insegnabili e i consumer disponibili
restano delimitati.

### 5.3 La riflessività utile: una lezione apre una lezione successiva

La forma più forte della tesi non è «una regola produce molti fatti», ma
«una regola acquisita permette di comprendere una nuova spiegazione».
Il cambiamento riguarda la capacità di apprendere, non soltanto il dominio
delle risposte.

Per esempio, sul piano concettuale, un legame può rendere una relazione
locale riconoscibile come evidenza richiesta da una costruzione; la costruzione
può allora leggere una frase successiva che prima non sapeva acquisire.
Oppure un criterio di copertura può rendere visibile che mancano le condizioni
di una regola, facendo emergere una domanda didattica più precisa.

`gap_source` applica già obblighi e coperture scelti dalla KB. È un precedente
per la seconda possibilità. Ma una copertura che interroga una vista più
povera della risposta può dichiarare lacune inesistenti; una copertura troppo
ampia può nascondere bisogni reali. Anche «so abbastanza per proseguire» deve
essere una conclusione sostenuta e correggibile.

La distinzione sperimentale da cercare è se la seconda lezione viene usata
senza che il teacher scriva lo schema interno o fornisca la conclusione.
Un risultato positivo mostrerebbe crescita locale del metodo. Non basterebbe
a dimostrare comprensione universale, introspezione completa o capacità di
scoprire autonomamente tutti i legami utili.

### 5.4 Proporre un ponte e autorizzarne l'uso sono capacità diverse

Confrontare firme, famiglie e casi comuni può suggerire candidati. Non
dimostra che una relazione implichi un'altra: due descrizioni possono avere
la stessa struttura e significati incompatibili. Una scoperta utile deve
produrre anche l'obbligo di giustificazione.

La possibile autonomia didattica comprende quindi un passaggio intermedio:
riconoscere che un legame aiuterebbe a soddisfare una richiesta, ma conservarlo
come ipotesi finché fonte, condizioni o conferma del maestro non ne sostengano
l'uso. Il bisogno incompleto può sopravvivere fra turni senza diventare una
regola attiva per semplice plausibilità.

Qui il rapporto con il maestro cambia: non deve più scegliere dall'esterno
ogni prossimo fatto, ma può rispondere a una lacuna formulata dal discente.
È un'ipotesi di sviluppo della continuità già descritta nel quadro, non
un'autorizzazione ad avviare acquisizioni automatiche.

### 5.5 Una correzione deve seguire le dipendenze e conservare le alternative

Se un ponte sostiene molte risposte, correggerlo può ripararle insieme.
La stessa moltiplicazione che rende economica la lezione amplifica il danno
di un legame falso. Ne segue un requisito di attribuzione: sapere se l'errore
sta nella premessa, nei ruoli, nello scope, nel ponte o nel consumer.

La ritrattazione corretta elimina un supporto, non necessariamente una
conclusione. Con un'altra prova indipendente la conclusione può restare vera.
Senza altri supporti deve cessare anche se era stata materializzata. Questo
impedisce due errori opposti: cancellare troppa conoscenza o conservare come
autonome conclusioni che dipendevano dal legame ritirato.

La scheda della relazione e la prova di una risposta servono a scopi diversi.
La prima orienta l'insegnante; la seconda identifica la catena effettiva,
gli intermedi e le condizioni. `relation_note/2` è un inizio di introspezione
descrittiva, non ancora una ricostruzione completa della derivazione.
`ir_domain_claim_basis/4` mostra un precedente di fondamento dichiarato da
confrontare prima di introdurre una seconda genealogia.

La persistenza deve mantenere queste distinzioni. Definizione appresa,
supporti, origine, conseguenza materializzata e stato di assunzione non
sono cinque modi di dire «fatto salvato». Il
[documento sulla sessione e provenienza](../session-and-provenance.md)
lascia già aperto il significato delle tracce di sessione: qui si aggiunge
la necessità di non perdere le condizioni dei risultati riusati.

### 5.6 La sostenibilità è parte della fertilità

La crescita per legami può costare poco in dichiarazioni e molto in ricerca.
Composizioni annidate aumentano gli intermedi; più percorsi possono provare
lo stesso risultato; ereditarietà ciclica e negazione richiedono trattamenti
espliciti. La KB non smetterà di crescere per rendere comodo il motore.

Il mantra #20 offre il criterio: osservare riletture e dipendenze prima di
ridurre ciò che il sistema vede. Una vista materializzata può accelerare,
ma deve essere invalidata da ciò che ne cambia il significato: nuovi fatti,
nuovi ponti, correzioni e, quando pertinente, contesto. Il nome del predicato
non basta sempre a descrivere le dipendenze di una chiamata dinamica.

Il solver distingue ricerca incompleta e assenza in punti come la negazione
per fallimento. Tale distinzione deve arrivare fino a copertura, decisione
e risposta: esaurire il lavoro disponibile non autorizza un «no» né un default
che richiede assenza provata dalla ricerca. Una KB teoricamente espressiva
ma incapace di dichiarare questo limite non sarebbe una KB viva affidabile.

### 5.7 Le radici dell'insegnabilità: riapprendere ciò che si sa fare

Il §5.3 chiede che una lezione apra una lezione successiva. Il concetto posto
da F. il 2026-09-11 dà a questa richiesta una direzione di lavoro: per ogni
abilità si risale alla superficie che la insegna, poi alla superficie che
insegna quella superficie, fino a una **radice** (una primitiva del motore), a
un **circolo** (una lezione che estende la propria forma) o a una **riga a
mano** (un buco). Il punto di partenza è fissato da ciò che parrot0 già sa
fare: la suite e i piani non sono solo un elenco di capacità da proteggere, ma
l'elenco di ciò che parrot0 dovrebbe poter **riapprendere** da lezioni di
ordine superiore. Una KB viva è quella in cui ogni catena finisce in una radice
o in un circolo. Sviluppo in [radici-insegnabilita.md](radici-insegnabilita.md).

## 6. Le decisioni che precedono un eventuale piano

Le domande seguenti non sono una coda di task. Identificano scelte ancora
da motivare prima di assegnare modifiche a file o moduli.

| Nodo | Decisione da chiarire | Rischio di una scelta prematura |
|---|---|---|
| Oggetto condiviso | Quali identità, ruoli, scope e alternative devono restare gli stessi fra lettura, inferenza, memoria e generazione? | Creare una nuova rappresentazione che duplica quelle esistenti |
| Elaborazione contestuale | Quali contributi possono cambiare una lettura prima della risposta, e come sono selezionati? | Rinominare il dispatch come attenzione senza cambiarne la funzione |
| Lezione eseguibile | Quali forme di regola, procedura e politica il learner può costruire e comporre parlando? | Confondere ciò che è scrivibile in `.p0` con ciò che è apprendibile |
| Correzione del metodo | Come attribuire una correzione alla lettura, al legame o alla scelta responsabile? | Conservare la risposta corretta e lasciare invariata la causa dell'errore |
| Continuità didattica | Che cosa conserva il bisogno incompleto e che cosa riattiva il suo apprendimento, anche fra sessioni? | Automatizzare l'acquisizione senza aumentare la capacità di comprendere |
| Consolidamento | Quando una soluzione diventa una procedura parametrica e come conserva condizioni, eccezioni e dipendenze? | Moltiplicare casi o promuovere generalizzazioni prive dei loro limiti |
| Sostenibilità | Come limitare il lavoro e riusare risultati senza perdere conoscenza pertinente? | Rendere la crescita possibile in teoria ma impraticabile nell'uso |
| Significato delle viste | Quali conseguenze devono essere condivise fra domanda polare, appartenenza, enumerazione, lettura e piano? | Dichiarare integrazione perché tutti accedono alla stessa KB, pur interrogando viste diverse |
| Chiusura compositiva | Quando il risultato di un ponte può diventare operando di un altro, e con quali vincoli su cicli e negazione? | Sommare famiglie locali e chiamare universale la loro composizione |
| Scope operativo | Come attraversano i consumer condizioni di classe, contesto, validità e assunzione? | Conservare l'etichetta ma perdere la premessa che limita l'inferenza |
| Descrizione ed esecuzione | Quali metadati di firma, famiglia e proprietà sono descrittivi e quali governano davvero un comportamento? | Scambiare una scheda corretta per un controllo già attivo |
| Supporti e revisione | Come distinguere conclusioni dipendenti, prove alternative e materializzazioni? | Ritirare anche conclusioni valide oppure conservare risultati ormai senza supporto |
| Fertilità didattica | Quale acquisizione rende comprensibile una lezione successiva che prima non lo era? | Misurare soltanto nuove risposte e attribuirle a un miglioramento del metodo |
| Proposta di legami | Che cosa rende un ponte candidato abbastanza giustificato da essere usato? | Promuovere somiglianza lessicale o strutturale a implicazione vera |

Non serve sciogliere ogni questione sull'intelligenza prima di un piano.
Serve invece che il perimetro scelto abbia una spiegazione del passaggio
da lezione a condotta e da condotta a ulteriore apprendimento. Una domanda
rimasta aperta deve restare visibile come ipotesi, non diventare una
dipendenza tacita dell'implementazione.

Le nuove righe non chiedono un interprete completamente nuovo. Chiedono di
scegliere il confine nel quale una promessa di riuso sia effettiva. La mappa
di §3.4 mostra che il primo oggetto da chiarire può essere un disaccordo fra
viste già presenti, senza aggiungere un'altra famiglia di rappresentazioni.

Una scelta resta aperta ma non neutra: centralizzare una semantica comune o
mantenerne più viste con contratti espliciti. In entrambi i casi devono essere
spiegate le differenze osservabili. Un consumer che usa deliberatamente una
vista di soli fatti diretti non deve descriverla come l'insieme di tutto ciò
che parrot0 sa derivare.

## 7. I vincoli che una futura proposta deve ereditare

[MANTRA.md](../../MANTRA.md) e [PRINCIPLES.md](../../PRINCIPLES.md)
restano i riferimenti normativi. Questo documento non li sostituisce.

La conoscenza linguistica e di condotta deve essere insegnabile in KB:
non soltanto parole e risposte, ma condizioni, combinazioni e criteri di
scelta. Le meccaniche generali possono restare nel motore; una politica
cognitiva non diventa meccanica soltanto perché oggi è compilata.

L'insegnamento naturale non deve presupporre che il maestro conosca
predicati o formati interni. Un esperto che spiega una distinzione insegna;
un agente che scrive direttamente la sua rappresentazione interna svolge
un intervento diverso. Questa differenza va preservata anche quando
l'orchestrazione è automatica.

La KB è parte del soggetto intero. L'integrazione deve aumentare ciò che
le capacità possono vedere e distinguere; non ottenere coerenza togliendo
loro informazione. Le strutture secondarie restano risorse: uno spazio
di lavoro comune non impone un algoritmo monolitico né autorizza a
eliminare facoltà funzionanti sulla sola base di questa analisi.

La correzione deve poter raggiungere le conseguenze dipendenti; l'incertezza
non va promossa a fatto per chiudere un ciclo. Separare verità delle
premesse e capacità inferenziale non sospende la disciplina anti-inganno.

L'autonomia del discente non esclude il maestro. Esclude che, dopo ogni
lezione, sia ancora il maestro a costruire dall'esterno la soluzione di
ogni nuovo caso. L'apprendimento continuo non è sinonimo di esecuzione
ininterrotta né autorizzazione a campagne massive.

### 7.1 I vincoli specifici della crescita per legami

**Insegnabilità a due livelli.** Poter aggiungere un ponte appartenente a una
famiglia esistente e poter insegnare una nuova famiglia di trasformazioni
sono capacità distinte. Il primo livello è già aperto dalle superfici G;
il secondo non si dichiara ottenuto solo perché un programmatore può scrivere
una clausola `.p0`. Vale la gerarchia prompt, prosa, autocorrezione e soltanto
poi promozione manuale del meccanismo generale mancante.

**Verità del legame.** Le fonti dei fatti non dimostrano automaticamente la
regola che li collega. Verso, quantificazione, ruoli, condizioni ed eccezioni
fanno parte della proposizione da giustificare. La capacità di comporre non
autorizza a identificare relazioni soltanto perché producono una risposta
desiderata.

**Generalità senza perdita di distinzione.** Un motore comune deve aumentare
ciò che i consumer vedono e sanno distinguere. Rendere tutti i ponti globali,
eliminare alternative o ridurre la KB per evitare conflitti viola il criterio
di evoluzione anche se rende più semplici i casi campionati.

**Verifica del soggetto intero.** Un micro-mondo può illustrare una meccanica;
il trasferimento nella KB viva richiede conoscenze reali preesistenti e
domande naturali. La conclusione attesa non deve essere iniettata prima di
annunciare che i punti sono stati collegati. I test legacy non sospendono
questa disciplina per le nuove evidenze.

**Misura distinta del guadagno.** Dichiarazioni persistite, conseguenze
raggiunte e capacità di apprendere ancora sono risultati diversi. Nessuno
si sostituisce agli altri. Una sola definizione può avere alto valore senza
essere contata come cento fatti salvati; un insieme di cento fatti veri non
dimostra da solo crescita del metodo.

**Crescita revocabile.** Correggere una lezione deve cambiare gli usi che ne
dipendono, conservando premesse vere e supporti alternativi. Il semplice
retract di una riga non certifica revisione di memoria, cache, letture e
risposte già derivate. Questo vincolo riguarda la continuità del soggetto,
non soltanto il solver.

## 8. Rapporto con i documenti già presenti

Il [report](../reports/2026-09-09-architettura-e-missione.md) conserva
l'argomentazione, i riscontri nel codice e i riferimenti scientifici.
Questo quadro ne seleziona le conseguenze progettuali, mantenendo distinta
la proposta dalla constatazione.

[The linguistic glue](the-linguistic-glue.md) descrive la continuità
ricercata. [Apprendimento assistito](apprendimento-assistito.md) tratta
la KB viva e il canale didattico. Il
[piano di integrazione cognitiva](integrazione-cognitiva-operativa.md)
contiene già oggetti comuni e percorsi operativi: questo quadro non li
annulla e non introduce una roadmap concorrente. Evidenzia il problema
trasversale da rendere esplicito quando quei percorsi vengono discussi:
quali strumenti acquisiti entrano anche nell'apprendimento successivo?

[Procedura di crescita KB](procedura-crescita-kb.md) disciplina il riuso
delle distinzioni e dei circuiti. Qui se ne approfondisce la motivazione:
la crescita utile dipende dalla capacità di riutilizzare e comporre ciò
che si è aperto, non dalla sola quantità di integrazioni.

[LEARN_PROTOCOL.md §G](../../LEARN_PROTOCOL.md#g-superfici-di-ordine-superiore--insegnare-sulle-relazioni)
è il complemento operativo: conserva superfici, mappa dei consumer, semantica,
limiti, ciclo causale e conteggio del guadagno. Qui il suo contributo è una
tesi sul riuso: una lezione può rendere accessibile altra conoscenza e, in
prospettiva, rendere possibili altre lezioni. Il protocollo non viene eseguito
per il solo fatto di espandere questo quadro.

[Thinking §0.1](thinking.md) motiva il predicato variabile e distingue il
substrato presente dagli schemi proposti. Il
[documento su una sola KB](one-kb.md) sostiene la continuità fra viste;
[sessione e provenienza](../session-and-provenance.md) chiarisce perché
stato, tracce e apprendimento persistente non siano intercambiabili. Questi
riferimenti pongono vincoli al riuso, non autorizzano a dichiararlo già completo.

[Due strutture abilitanti](due-strutture-kb-viva.md) sviluppa la ricerca di
leve dello stesso livello di `apply/2`. Il suo oggetto è la rappresentazione:
istanze a ruoli aperti e definizioni componibili su cui inferire. Va letto
prima di tradurre queste aperture in una lista di capacità o condotte.

## 9. Quando questo quadro potrà generare un piano

Un futuro piano avrà una base sufficientemente chiara quando potrà nominare
un passaggio cognitivo circoscritto, mostrarne la continuità con gli oggetti
esistenti e spiegare quale trasformazione diventerà apprendibile. Dovrà
distinguere la meccanica da costruire dalla conoscenza da insegnare e
indicare perché l'acquisizione sarà riutilizzabile oltre il caso iniziale.

Solo in quel documento successivo avranno posto ordine degli interventi,
responsabilità, dettagli implementativi e modalità di verifica. Qui non
si scelgono soglie, benchmark, programmi di test o calendari. Questa
sospensione dell'operatività non è attesa della completezza: è il tempo
necessario a dichiarare quale ipotesi un intervento intende rendere concreta.

Il discrimine da portare alla discussione è uno:

> Dopo questa acquisizione, parrot0 dispone soltanto di una risposta in più,
> oppure di uno strumento in più per comprendere, ragionare e imparare?

Non tutte le lezioni devono modificare il metodo: anche i fatti servono.
Ma un progetto di KB viva deve spiegare come possano avvenire entrambe
le crescite e come si sostengano. È questo il preludio al piano, non ancora
il piano.

### 9.1 Che cosa l'elaborato G aggiunge al criterio di maturazione

Un futuro piano può ora formulare la propria ipotesi con maggiore precisione:
**quale legame appreso diventerà utilizzabile da quale altra capacità, con
quali condizioni conservate?** La risposta deve identificare sia ciò che si
amplia sia ciò che resta legittimamente escluso.

Prima di trasformare questa domanda in lavori serve distinguere almeno tre
tipi di avanzamento:

| Avanzamento proposto | Evidenza che lo renderebbe credibile | Conclusione che resterebbe ingiustificata |
|---|---|---|
| Nuovo membro di una famiglia già insegnabile | una lezione naturale cambia l'uso su casi successivi e conserva il contrasto | l'interprete può apprendere qualunque nuova famiglia |
| Integrazione fra consumer | una conseguenza già disponibile in una vista diventa utilizzabile nell'altra mantenendo condizioni e supporti | tutte le capacità condividono ormai ogni inferenza |
| Crescita del metodo | una distinzione appresa consente di comprendere e usare una lezione successiva prima inaccessibile | comprensione universale o convergenza dimostrata verso il maestro |

Sono discriminanti concettuali, non soglie o programmi di test. La scelta
dei transcript, dei controlli e dell'ordine degli interventi appartiene al
piano successivo e al protocollo operativo appropriato.

Una proposta dovrebbe anche dire che cosa la smentirebbe: se occorre ancora
insegnare ogni conclusione, il ponte non ha trasferito; se il risultato non
mantiene lo scope, ha trasferito troppo; se soltanto il teacher sa quale
nuova lezione scegliere e come renderla leggibile, l'autonomia del metodo
non è ancora dimostrata. Questi esiti non vanno nascosti dietro il numero di
clausole acquisite.

Il contributo più fertile dell'ordine superiore è rendere concreta la
domanda originaria del quadro: una KB cresce come soggetto quando ciò che
apprende diventa parte degli strumenti con cui legge, decide e apprende
ancora. La codebase contiene già punti di questa forma; chiarirne i contratti,
le connessioni e i limiti è il passaggio necessario per discuterne lo sviluppo.
