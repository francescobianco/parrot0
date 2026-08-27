# Apprendimento assistito

**Stato:** piano attivo, 2026-08-27  
**Missione:** fare della conversazione il canale primario con cui parrot0 amplia
non soltanto i fatti, ma le forme con cui comprende, compone, interroga e
ragiona.  
**Prima evidenza:**
[`depth-session-01.md`](../labs/apprendimento-assistito/depth-session-01.md).  
**Cancello:** §6 — nessuna sessione lunga prima che gli strati di
meta-comprensione M0–M14 siano chiusi.  
**Ritmo:** §11 — ogni cosa appresa è un piccolo incremento, e si committa e
pusha anche se parziale.

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
4. **costruzioni:** “`X glints Y` esprime `glorphs(X,Y)`”;
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

**Regola.** Le dieci ore del §7 — e a maggior ragione qualunque teacher che giri
come processo continuo — non si avviano finché gli strati di meta-comprensione
elencati in questo capitolo non sono chiusi. Non è un ordine dei lavori
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

**M2 — Uso e menzione.**  
*Manca:* parlare *di* una parola che il motore già usa. `unless is a condition
marker` finisce a muro perché la superficie viene consumata dal proprio ruolo.  
*Atto didattico:* qualunque superficie — piena o funzionale, già attiva o
ignota — deve poter diventare l'oggetto della lezione.  
*Gate:* insegnare una proprietà di una parola-funzione attiva, usarla nello stesso
processo, ritrarla e vedere la capacità sparire.

**M3 — Denotazione ed equivalenza operativa.** *(A1: aperto, non chiuso)*  
*Fatto:* `construction_frame/3` con la vista `extract_frame/2`, pivot insegnabile,
retract parlato.  
*Manca:* solo due slot, allineati per posizione; vista interrogativa limitata al
caso SVO; nessuna inversione dei ruoli; nessuna arità diversa da due; nessuna
catena di costruzioni verificata; nessuna induzione dai soli esempi concreti.  
*Atto didattico:* «`X glints Y` significa `Y glorphs X`», «`X glints Y con Z`
significa …».  
*Gate:* inversione, arità ≠ 2 e catena a due passi, ciascuna con transfer
held-out e ablation.

**M4 — Ruoli nominabili.**  
*Manca:* il teacher può allineare due slot, ma non *nominare* i ruoli, che è
esattamente la frase con cui il §1 apre questo piano: «qui il primo nome è chi
agisce, il secondo è ciò su cui agisce». Non può insegnare un ordine diverso da
quello della lingua corrente, né ruoli oltre soggetto e oggetto (strumento,
luogo, tempo, beneficiario).  
*Atto didattico:* la frase del §1, letteralmente, senza slot espliciti.  
*Gate:* una costruzione a tre ruoli insegnata a voce e interrogabile su ciascun
ruolo; un ordine non canonico insegnato senza toccare il motore.

**M5 — Morfologia e dualità delle domande.** *(A2)*  
*Manca:* `glorphs` non porta `glorph`; `who glorphs X?` non porta `what does X
glorph?`; `organism(algae)` risponde al singolare e non al plurale.  
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
*Manca:* `occupied_square(d2)` non soddisfa `occupied(X), square(X)`. Due
traduzioni ragionevoli della stessa nozione restano conoscenza viva e isolata, e
una sessione lunga moltiplica il debito invece di ridurlo.  
*Atto didattico:* «essere un `occupied_square` vuol dire essere `occupied` ed
essere `square`» deve costruire il ponte nei due versi.  
*Gate:* una domanda posta nella forma composta trova la risposta memorizzata in
quella scomposta e viceversa; l'ablation toglie il ponte e non i fatti.

**M9 — Struttura del discorso in prosa.**  
*Manca:* entrano enumerazioni di membri; restano fuori causalità, finalità,
processo e condizione. E l'estrazione produce atomi falsi — `organism(metabolism)`
da «fuel their metabolism».  
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

### A4 — Procedure insegnabili senza misclaim

Una frase che insegna una procedura deve entrare in quarantena, mai essere
eseguita come domanda. Il piano risultante deve legare i numeri a ruoli e
trasferire a input nuovi.

### A5 — Scacchi come dominio di integrazione

Non una tabella di domande sugli scacchi: rappresentazione di pezzi, mosse,
occupazione, traiettorie, attacco, stato del re ed eccezioni. Il gate usa
posizioni generate dopo le lezioni e richiede spiegazione della legalità.

### A6 — Sessione di dieci ore

Si esegue il curriculum completo solo dopo la chiusura degli strati M0–M14 del
§6, di cui A1–A5 sono le milestone visibili. Le milestone non bastano da sole: un
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
