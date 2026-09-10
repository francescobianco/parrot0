# Due strutture abilitanti oltre il predicato variabile

11 settembre 2026. Proposte di design della KB richieste da F., sviluppate
dalla lettura della codebase a `8e33072`. Si affiancano a
[LEARN_PROTOCOL.md §G](../../LEARN_PROTOCOL.md#g-superfici-di-ordine-superiore--insegnare-sulle-relazioni)
e al [quadro preliminare della KB viva](quadro-preliminare-kb-viva.md).
**Sono ipotesi strutturali, non implementazioni o prestazioni certificate.**

## 1. Il criterio: aprire uno spazio, non aggiungere un comportamento

`apply/2` ha un effetto maggiore di una singola feature perché sposta un
confine: il nome della relazione può essere scelto dalla conoscenza. Le
operazioni che prima avrebbero richiesto rami distinti possono ora interrogare
relazioni sconosciute al momento della compilazione.

Cerco due spostamenti dello stesso livello:

1. **Anche la struttura degli argomenti diventa conoscenza:** una relazione
   concreta è un nodo con ruoli aperti, non soltanto una tupla posizionale.
2. **Anche la definizione diventa un oggetto componibile:** una relazione
   derivata è descritta da un'espressione sulla quale la KB può inferire,
   non soltanto da clausole che il solver esegue ma gli altri consumer vedono
   come un nome opaco.

Il primo spostamento apre la struttura del fatto. Il secondo apre la struttura
del significato derivato. Insieme al predicato variabile formano tre assi:

| Che cosa può variare | Prima dello spostamento | Dopo lo spostamento |
|---|---|---|
| La relazione | il consumer decide quale predicato chiamare | il predicato è un dato: `apply/2` |
| I ruoli del fatto | il consumer conosce firma e posizioni | ruoli, partecipanti e proiezioni sono relazioni interrogabili |
| La costruzione del significato | ciascuna famiglia ha una forma e un percorso proprio | le definizioni sono espressioni componibili, interpretabili e analizzabili |

Non rivendico come nuove in assoluto la reificazione o la composizione di
espressioni. L'intuizione da valutare riguarda **quale confine aprire in questa
KB**, con i meccanismi che già possiede, e quante distinzioni oggi separate
possano diventare casi dello stesso oggetto.

## 2. Prima struttura — Il fatto come nodo a ruoli aperti

### 2.1 Il limite nascosto che il predicato variabile non rimuove

Rendere variabile R in R(X,Y) lascia fissi numero, posizione e significato
degli argomenti. Per interrogare una relazione diversa basta conoscerne il
nome soltanto se il consumer conosce già la sua firma.

Ma molta conoscenza reale non arriva tutta insieme in una coppia. Di uno
scambio si possono conoscere prima chi dà e che cosa, poi chi riceve, quando
e a quale titolo. Di una misura, prima il valore e poi unità, metodo e
condizioni. Aggiungere un dettaglio non dovrebbe obbligare a creare
`scambio_con_data`, `scambio_con_data_e_luogo` e una nuova famiglia di lettori.

La proposta è dare un'identità all'istanza relazionale e collegarle i
partecipanti **attraverso ruoli che sono anch'essi dati**. Il punto non è
mettere un dizionario dentro un fatto: è consentire alle regole di interrogare
quale ruolo sia presente, che cosa lo riempia e come corrisponda a un altro
schema.

### 2.2 La forma minima e il suo significato

Notazione illustrativa di progetto; questi predicati non sono dichiarati
come un'API già esistente né sono lezioni da incollare nella chat:

```prolog
relation_instance($Occurrence, $Relation).
role_value($Occurrence, $Role, $Value).
role_correspondence($View, $SourceRole, $TargetRole).
```

Una transazione può avere ruoli come cedente, destinatario, oggetto e momento.
Una proiezione binaria può selezionare due ruoli senza eliminare gli altri.
Una nuova lezione può aggiungere un ruolo allo stesso nodo; una domanda nuova
può selezionarlo senza modificare la firma dei fatti già appresi.

I nomi sono provvisori. Prima di introdurli occorre confrontarli con
`frame_slot/3`, con le proposizioni reificate e con le identità delle letture
già presenti. La proposta è un contratto comune di ruoli, non un altro
archivio in concorrenza con gli oggetti esistenti.

L'identità del nodo è essenziale. Se una persona cede due oggetti in due
occasioni, non si devono combinare il destinatario della prima e l'oggetto
della seconda. Il nodo conserva **la coappartenenza dei ruoli a quella
istanza**. Una raccolta di archi binari che perde questa identità non realizza
la struttura proposta.

### 2.3 Il moltiplicatore: una proiezione vale per ogni nuovo riempimento

Il ponte G mette in relazione due predicati. Un ponte sui ruoli può dichiarare
come una configurazione si legge da un altro punto di vista. Se il dominio
giustifica due descrizioni dello stesso trasferimento, i ruoli possono
corrispondere senza che ogni fatto venga copiato e senza scambiare posizioni
nel C.

Il collegamento contiene due conoscenze distinte:

- quali istanze sono ammesse nella vista, con le condizioni di dominio;
- quali ruoli diventano quali ruoli nella vista risultante.

La sola corrispondenza fra ruoli non prova che la vista sia semanticamente
valida: destinatario di una comunicazione e destinatario di un pagamento
possono condividere un ruolo astratto senza descrivere lo stesso evento.
Serve una licenza semantica, come per i ponti G.

Il vantaggio è che l'algoritmo di proiezione e unificazione non deve conoscere
in anticipo la cardinalità dello schema. Le restrizioni su ruoli obbligatori,
facoltativi, ripetibili, incompatibili o dipendenti possono essere dichiarate
sullo schema e riusate dai consumer. Anche un nuovo genere di ruolo entra
come conoscenza, se il canale linguistico sa insegnarlo.

Una stessa struttura abilita allora, senza motori per ciascun dominio:

| Conseguenza | Perché discende dalla rappresentazione |
|---|---|
| Arricchire un fatto incompleto | aggiungere un ruolo non cambia l'identità dell'istanza né la firma del contenitore |
| Interrogare da più prospettive | la domanda sceglie ruoli e vincoli, invece di presumere soggetto e oggetto universali |
| Comporre relazioni con molti partecipanti | il join lega ruoli dichiarati e conserva l'identità dei testimoni |
| Trasferire uno schema fra domini | la mappa dei ruoli diventa un oggetto insegnabile, soggetto a condizioni |
| Qualificare una misura o un evento | unità, tempo e metodo hanno un posto senza moltiplicare varianti del predicato |
| Parlare di un'affermazione | un'identità citabile può ricevere supporti, contesti e revisioni senza decomporre il suo testo |

L'ultima riga richiede una distinzione: l'evento descritto, il contenuto di
una proposizione sull'evento e l'atto di una fonte che la afferma non sono
la stessa identità. Condividere ruoli non autorizza a confonderli.

### 2.4 Conoscenza parziale senza fatti falsi

Un ruolo assente significa non ancora conosciuto, non inesistente. Una
domanda su due ruoli disponibili può essere risolvibile mentre quella che
ne richiede un terzo resta aperta. La completezza è relativa al contratto
della domanda o della regola, non un flag globale sull'evento.

Due record parziali non si fondono perché hanno alcuni valori uguali.
La loro identità comune deve avere evidenza. In caso contrario si conservano
due candidati: unificare arbitrariamente oggetti incompleti inventerebbe
cooccorrenze tanto quanto attribuire un fatto falso.

Anche la molteplicità è conoscenza. Un ruolo può ammettere più partecipanti;
un valore nuovo può affinare, sostituire o contraddire quello precedente.
La scelta non può essere «ultimo valore vince» per ogni ruolo. La revisione
deve riusare i supporti e i contesti, preservando ciò che era stato affermato
e distinguendo la vista corrente dalla storia.

### 2.5 Il precedente nella codebase e la novità effettiva

| Sorgente | Struttura già presente | Confine da aprire |
|---|---|---|
| [`dialogue-frames.p0`](../../kb/core/dialogue-frames.p0), `frame_slot/3` | ruolo e valore sono argomenti | `frame_complete` e `frame_answer` consumano ancora un nucleo specifico relazione/entità e una domanda binaria |
| [`grammar.p0`](../../kb/core/grammar.p0), `frame_role_order/2` | l'ordine dei ruoli è dichiarato | un ordine insegnabile non equivale a un insieme aperto di ruoli semantici per ogni relazione |
| [`conditional-plans.p0`](../../kb/core/conditional-plans.p0), `proposition_arg_kind/3`, `proposition_arg_form/3` | tipi e forma degli argomenti sono conoscenza | i percorsi letti compongono esplicitamente primo e secondo argomento |
| [`situation.p0`](../../kb/core/situation.p0), `state_prop/4` | una proposizione ha identità, entità, proprietà e valore | una reificazione locale non rende automaticamente uniforme ogni firma del mondo |
| [`context-scope.p0`](../../kb/core/context-scope.p0), `holds_in/2`, `proposition_signature/4` | contenuti citabili e contesti | occorre conservare identità e supporti attraversando le proiezioni |
| [`document-claims.p0`](../../kb/core/document-claims.p0), `claim_current_reading`, `reading_depends_on` | osservazione e lettura corrente sono distinte | riusare questa distinzione per arricchimento e revisione senza creare identità incompatibili |

La novità non è «aggiungere frame»: esistono. È rendere **la forma del fatto
indipendente dal numero di posizioni che il consumer conosce**, con ruoli e
proiezioni interrogabili. Se dopo aver aggiunto il contenitore ogni lettore
continua a distinguere a mano tre casi di arità, abbiamo solo cambiato formato.

I predicati ordinari possono restare come viste e strutture efficienti.
Un adattatore dichiarato in KB può associare le loro posizioni ai ruoli.
Non si propone di riscrivere tutta la KB in una tabella universale: sarebbe
un costo enorme senza prova di beneficio. Si cerca un'interfaccia che renda
visibili attraverso lo stesso contratto rappresentazioni già esistenti.

### 2.6 Perché può scalare e dove può costare

Con k qualificazioni facoltative esistono fino a 2^k combinazioni di presenza.
Non occorre costruire uno schema e un parser per ciascuna: il nodo conserva
soltanto i ruoli conosciuti e i vincoli ne governano le combinazioni ammissibili.
Questo è un conto dello spazio rappresentativo, non una promessa di velocità
né l'affermazione che oggi la codebase contenga 2^k varianti.

Il costo si sposta sui join e sugli indici: trovare un'istanza per relazione,
ruolo e valore può costare più di una query binaria ben indicizzata. La
proiezione deve usare gli indici esistenti dove possibile; una vista può essere
materializzata con dipendenze corrette. La rappresentazione più aperta non
autorizza una scansione integrale per ogni domanda.

Non si elimina il limite fisico di `KB_MAX_ARGS`: i fatti di ruolo hanno
arità piccola e una struttura può essere distribuita su più fatti. Restano
i limiti di memoria, termini, ricerca e budget. La promessa è la crescita
dello schema senza nuove firme C, non risorse illimitate.

La prova forte è insegnare un ruolo semantico prima sconosciuto su conoscenze
reali, usarlo in una domanda e in una proiezione, poi ritrattarne la licenza.
La vecchia conoscenza deve restare leggibile; il nuovo uso deve dipendere
dalla lezione; due istanze simili non devono mescolarsi. Avere salvato una
coppia chiave/valore non sarebbe sufficiente.

## 3. Seconda struttura — Le definizioni come espressioni su cui inferire

### 3.1 Il limite nascosto del catalogo degli operatori

G rende insegnabili composizione, intersezione, inclusione, inverso ed
eccezione. Ma ogni famiglia ha la propria dichiarazione e molti operandi
vengono risolti con `apply` sul predicato diretto. Il fatto che due operatori
esistano non garantisce che il risultato dell'uno sia un operando dell'altro.

La proposta è trattare **la definizione stessa come un valore strutturato**.
Il nome di una relazione può denotare un'espressione; un'espressione può
contenere nomi o altre espressioni; le regole possono interrogarne la struttura
per ricavarne significato, proprietà e condizioni d'uso.

Il cambio di livello è questo: non soltanto «quale relazione applico?» ma
«di quale costruzione è fatta questa relazione, e che cosa segue da quella
costruzione?». Non è un nuovo operatore isolato. È una chiusura del linguaggio
delle definizioni sotto composizione.

### 3.2 Una rappresentazione omogenea

Notazione concettuale, non sintassi attualmente disponibile nella chat:

```text
nome(R)
inversa(E)
entrambe(E1, E2)
almeno_una(E1, E2)
seguita_da(E1, E2)
nel_contesto(C, E)
con_ruoli(Mappa, E)
```

La lettera E può essere **una qualunque espressione ammessa**, non soltanto
il nome di un predicato nudo. Una definizione insegnata oggi può comparire
dentro una definizione insegnata domani senza una nuova famiglia C per la
combinazione.

Questo non richiede un parser C per ciascuna forma composta: la lingua che
costruisce l'espressione è conoscenza, e le meccaniche di legame, ricorsione
e unificazione sono generiche. Il nucleo semantico di un costruttore può
essere dichiarato con regole su termini, sopra le primitive già presenti.

Il caso `nome(R)` deve raggiungere anche la definizione dichiarata per R;
altrimenti il linguaggio si interrompe a ogni nome derivato, riproducendo
il confine attuale di `holds`. I fatti diretti e le definizioni possono
essere supporti distinti della stessa domanda. Il contratto deve esplicitare
quando una definizione si aggiunge e quando ne sostituisce una precedente.

La KB non dovrebbe duplicare i dati attuali. `relation_chain`, `relation_and`
e gli altri predicati G possono diventare viste sulla descrizione comune,
o fornire descrizioni alla stessa interpretazione. La direzione va scelta
sulla possibilità di preservare semantica e insegnamenti esistenti.

### 3.3 Il moltiplicatore più sottile: proprietà dedotte dalla struttura

Una definizione leggibile dalla KB permette di dedurre proprietà senza
insegnarle per ogni relazione. Questo è un passo oltre `behaves like`:
la proprietà può seguire da **come la relazione è costruita**, non da una
somiglianza dichiarata fra nomi.

Esempi matematici, riferiti a relazioni binarie pure interpretate come insiemi
di coppie, con dominio e contesto fissi:

| Struttura | Proprietà o equivalenza che segue | Che cosa basta insegnare una volta |
|---|---|---|
| intersezione di R e inversa(R) | è simmetrica | la legge sulla costruzione, valida per ogni R |
| inversa di inversa(R) | equivale a R | la legge del doppio inverso |
| `inversa(seguita_da(R, S))` | equivale a inversa(S) seguita da inversa(R) | la legge con l'ordine scambiato |
| intersezione di due relazioni transitive | è transitiva | la regola sulle proprietà degli operandi |

La terza riga si legge senza ambiguità come:

```text
inversa(seguita_da(R, S)) = seguita_da(inversa(S), inversa(R))
```

Il risultato non nasce da esempi coincidenti. Segue dal significato della
costruzione. Se una nuova relazione viene definita come intersezione di una
relazione e del suo inverso, la scheda può sapere che è simmetrica prima che
esista un solo fatto della nuova relazione.

La ragione è interamente strutturale: la coppia (x,y) appartiene a quella
intersezione esattamente quando valgono R(x,y) e R(y,x). Scambiare x e y
richiede le stesse due premesse. Una sola legge su questa forma rende la
proprietà disponibile per tutte le relazioni che vi verranno sostituite.

Anche l'assenza di una legge è informativa: l'unione di due relazioni
transitive non è in generale transitiva. Il sistema non deve distribuire
proprietà per somiglianza dei nomi. La proprietà derivata richiede una regola
giustificata sul costruttore e sulle premesse, come ogni altra inferenza.

Le stesse definizioni possono quindi alimentare più viste: risposte, scheda,
dipendenze, requisiti di ruoli e possibili trasformazioni equivalenti. Non
serve descriverle separatamente in ciascun consumer.

### 3.4 Anche nuove famiglie possono essere definizioni

Una costruzione nuova esprimibile con quelle note può essere insegnata come
abbreviazione parametrica. Per esempio, «la parte reciproca di una relazione
è ciò che vale in entrambi i versi» descrive la costruzione generale
`entrambe(R, inversa(R))`, lasciando R da istanziare.

Non si insegna soltanto che una specifica relazione è reciproca: si insegna
**come costruire una relazione reciproca a partire da qualunque relazione**.
Da quel momento il nome della costruzione può essere riusato su nuovi domini.
Questa è la scala più vicina all'effetto di `apply/2`: un'intera famiglia
diventa un oggetto parametrico della KB.

Il parametro deve essere rappresentato esplicitamente e avere scope.
Un simbolo che nomina un parametro persistente non è una variabile Prolog
libera da lasciare accidentalmente dentro un fatto. Istanziare una definizione
deve rinominare correttamente le variabili locali e preservare quelle
condivise. Testimoni esistenziali di due rami diversi non possono essere fusi
perché hanno lo stesso nome tipografico.

La generalità ha un confine: un costruttore non esprimibile nel nucleo esistente
può richiedere una nuova primitiva generale. Non si dichiara apprendibile
qualunque computazione perché alcuni costruttori si compongono. Ma non serve
una nuova primitiva per ogni combinazione delle costruzioni già interpretabili.

### 3.5 Il significato deve precedere le ottimizzazioni

Una struttura interrogabile permette di scegliere forme equivalenti più
economiche: spingere un vincolo verso una sorgente selettiva, riusare una
sottoespressione, evitare una doppia inversione. La scelta può dipendere da
leggi in KB e dai costi osservati, anziché da un catalogo C di casi speciali.

Le equivalenze non sono universali fra tutti i modi di eseguire un termine.
Le leggi della tabella assumono relazioni pure e semantica insiemistica.
Operazioni con effetti, molteplicità di prove, ordine dei risultati, default,
contesti differenti e ricerca incompleta richiedono contratti ulteriori.
Una riscrittura non può usare una legge valida sui fatti puri per spostare
un'asserzione o cambiare la portata di una negazione.

Un caso cruciale è la posizione del contesto. Valutare l'intera composizione
in C non equivale automaticamente a mettere in C un solo operando. Anche
due relazioni sostenute in tempi diversi non formano una prova simultanea
senza una regola che ne giustifichi la compatibilità.

L'analisi delle proprietà deve essere prudente: non derivare «non simmetrica»
perché manca una prova di simmetria; restituire proprietà sostenute e residui
quando la struttura non basta. Le definizioni ricorsive richiedono una
semantica e un controllo dei cicli; il budget del solver non è una prova di
aver raggiunto il punto fisso.

### 3.6 Il precedente nella codebase e la novità effettiva

| Sorgente | Meccanismo da riusare | Confine osservato |
|---|---|---|
| [`src/kb.c`](../../src/kb.c), `apply/2`, `call/1`, unificazione di termini | interpretazione di nomi, termini e regole | `apply` applica un predicato; da solo non interpreta ogni definizione G come espressione annidabile |
| [`procedures.p0`](../../kb/core/procedures.p0), blocco `holds` | famiglie semantiche dichiarate | molti operandi chiamano il predicato diretto; manca una chiusura compositiva uniforme |
| [`procedures.p0`](../../kb/core/procedures.p0), `relation_note`, `relation_like` | schede derivate e proprietà come conoscenza | l'ereditarietà segue nomi e quattro proprietà; non è un'analisi generale delle definizioni |
| [`composition.p0`](../../kb/core/composition.p0), `composed/3`, `stage_wraps/2` | una struttura ricorsiva è già interpretata da poche clausole | riguarda composizione della resa e condizioni degli stadi, non l'algebra generale delle relazioni |
| [`thinking.p0`](../../kb/core/thinking.p0), `thinking_step`, `thinking_operator` | operazioni rappresentate da termini | descrivere un passo non unifica automaticamente i significati di tutte le espressioni |
| [`messages.p0`](../../kb/core/messages.p0), `turn_form_act` | atti e argomenti sono già dati | la costruzione linguistica di definizioni annidate e parametriche va verificata, non presunta |

La novità non è «aggiungere un interprete» in astratto: il solver lo è già.
È rendere **le definizioni apprese disponibili come struttura alla stessa
inferenza che le usa**, con una semantica compositiva condivisa. Un AST
salvato ma mai interrogato sarebbe soltanto una serializzazione.

Il limite di quattro argomenti e otto goal per clausola non impedisce di
rappresentare un'espressione con nodi e archi o con termini/lista adeguati.
La scelta fra albero e grafo condiviso dipende anche dai limiti dei termini
e dal costo della risoluzione; non si nasconde una nuova firma senza limiti
all'interno di una stringa opaca.

### 3.7 Perché può scalare e che cosa la smentirebbe

Se ogni operatore accetta soltanto relazioni nude, ogni composizione fra
operatori può richiedere un raccordo apposito. Se tutti accettano espressioni
dello stesso linguaggio, una regola di interpretazione vale a ogni profondità
ammessa. La crescita è nelle combinazioni costruibili, non nel numero di
rami aggiunti al consumer.

Una legge sulle proprietà o un'equivalenza può inoltre valere per ogni
definizione futura della stessa forma. Si moltiplicano **le conseguenze sulle
relazioni**, non solo i fatti raggiunti attraverso di esse. Questa è la parte
più promettente della proposta: la KB può sapere qualcosa su una capacità
appena definita grazie alla sua costruzione.

Il prezzo è l'interpretazione e l'analisi strutturale. Senza condivisione,
indici e dipendenze, la stessa sottoespressione può essere rivalutata molte
volte. La semplificazione può avere costi maggiori dell'esecuzione diretta.
Il beneficio va misurato su carichi reali; non segue automaticamente
dall'eleganza della rappresentazione.

La prova forte usa una definizione annidata nuova, insegnata naturalmente,
senza aggiungere clausole specifiche per quella combinazione. Ne verifica
sia una risposta sia una proprietà che segue dalla forma. Una costruzione
vicina che non giustifica la proprietà deve restare distinta. Ritirare una
definizione o la legge che ne autorizza l'analisi deve cambiare l'uso senza
ricompilare, mantenendo eventuali supporti indipendenti.

## 4. Perché insieme cambiano il design della KB

La prima struttura rende aperto **di quali parti è fatto un fatto**.
La seconda rende aperto **di quali operazioni è fatto un significato**.
Il predicato variabile rende aperto **quale relazione interrogare**.

Una definizione potrebbe allora nominare ruoli invece di presumere soltanto
due posizioni, comporre viste su istanze con molti partecipanti e derivare
le proprie proprietà dalla forma. Una nuova qualificazione del dominio
potrebbe diventare un vincolo di una definizione senza cambiare la firma
del fatto né il codice di ogni consumer.

Esempio concettuale: una relazione derivata seleziona due ruoli di un evento,
richiede che un terzo soddisfi una classe e compone il risultato con un'altra
vista. Domani viene insegnato un quarto ruolo e una diversa definizione lo
usa. Non occorre allargare un array di argomenti semantici nel C o scrivere
la variante dell'operatore per quel dominio. Occorre conoscere la nuova
distinzione e saperla costruire attraverso il canale linguistico.

Questo non produce da solo curiosità, memoria dialogica o scelta di una
domanda. Produce il substrato sul quale molte di quelle condotte possono
essere descritte e corrette nello stesso linguaggio della KB. Le condotte
diventano consumatori di una struttura più fertile, anziché sostituti della
struttura mancante.

## 5. Il discrimine operativo della proposta

Non proporrei una migrazione indiscriminata della KB. Cercherei un confine
reale per ciascuna struttura e verificherei la promessa minima:

| Struttura | Nuova libertà che deve essere dimostrata | Falso successo da escludere |
|---|---|---|
| Nodi a ruoli aperti | insegnare un ruolo prima sconosciuto e usarlo con gli stessi consumer e proiettori | una tabella generica dietro lettori che continuano a conoscere firme fisse |
| Definizioni come espressioni | comporre definizioni nuove e inferire sulle loro proprietà con leggi generali | un termine salvato che richiede un ramo speciale per ogni combinazione |
| Le due insieme | estendere ruoli e costruzioni conservando identità, scope e supporti | un grafo più grande che perde le condizioni o mescola istanze |

I nomi dei domini devono comparire nelle lezioni, non nei meccanismi. Le
superfici naturali devono crescere nella KB e raggiungere l'interpretazione
condivisa; nessun esperto deve conoscere `role_value` o la sintassi dell'AST
per insegnare. Le prove di meccanica restano distinte dal trasferimento su
conoscenze reali della KB completa.

La seconda struttura è più direttamente contigua alla tesi di G: chiude
l'annidamento e rende il significato analizzabile. La prima agisce su un
limite più profondo della rappresentazione dei fatti, con maggiore costo di
integrazione potenziale. Non è ancora dimostrato quale dia più rendimento.

Il criterio per chiamarle avanzamenti verso una KB viva è che una lezione
modifichi **le strutture che altre lezioni possono usare**, e non soltanto
una risposta o una condotta. Finché il consumer richiede una nuova eccezione
compilata per ogni nuova combinazione, il confine non è stato aperto.
