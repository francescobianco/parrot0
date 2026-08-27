# Apprendimento assistito

**Stato:** piano attivo, 2026-08-27  
**Missione:** fare della conversazione il canale primario con cui parrot0 amplia
non soltanto i fatti, ma le forme con cui comprende, compone, interroga e
ragiona.  
**Prima evidenza:**
[`depth-session-01.md`](../labs/apprendimento-assistito/depth-session-01.md).

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

## 6. Esperimento di dieci ore

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

## 7. Il teacher come processo lungo

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

## 8. Metriche che sostituiscono il conteggio delle generazioni

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

## 9. Tracce che non si cancellano

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

## 10. Milestone

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

Si esegue il curriculum completo solo dopo A1–A4. Il risultato è un checkpoint
della KB con genealogia, non una promozione opaca di tutto ciò che è stato detto.

## 11. Criterio di missione

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
