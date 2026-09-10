# Protocollo operativo di addestramento di parrot0

Questo file è un comando operativo per un coding agent. L'invocazione prevista è:

> Leggi `LEARN_PROTOCOL.md` ed esegui il protocollo per addestrare parrot0 sul
> dominio `<DOMINIO>`, per un budget di `<TEMPO>`, usando fonti `<FONTI>`.

Il risultato atteso non è una demo, una patch cosmetica o una suite verde. È un
incremento verificato e persistente della conoscenza di parrot0, ottenuto
parlandogli in lingua naturale.

> ## 📖 [→ IL CATALOGO DELLE FORME DI APPRENDIMENTO (§6-bis)](#6-bis-forme-di-apprendimento--che-cosa-si-può-insegnare-parlando)
>
> **Che cosa si può insegnare a parrot0 parlando, oggi.** È la prima cosa da
> leggere prima di aprire una chat: si sceglie la *forma* da lì, invece di
> scoprire a metà lezione che quella frase non entra da nessuna parte.
>
> | | |
> |---|---|
> | [A. Classi e appartenenza](#a-classi-e-appartenenza) | `X è un Y` · `ogni Y è P` · `nessun A è un B` |
> | [B. Relazioni](#b-relazioni) | `V is a relation verb` · `V chains` · `V is the inverse of W` · `V goes both ways` |
> | [C. Attributi e valori](#c-attributi-e-valori) | `X è rosso` · `correction: X è Y` · `X pesa N` |
> | [D. Parole, forme e ruoli](#d-parole-forme-e-ruoli) | `"superficie" è un <classe>` · `the italian for X is Y` |
> | [E. Condotta e ragionamento](#e-condotta-e-ragionamento) | `quando <situazione> allora <mossa>` · `step for X is …` |
> | [F. Procedure eseguibili](#f-procedure-eseguibili) | `rule for X is <operatore>` · `apply X to <testo>` |
>
> **[→ §6-ter: le 50 forme DA IMPLEMENTARE](#6-ter-le-50-forme-da-implementare--la-mappa-in-anticipo)** — la mappa
> scritta in anticipo: quali forme mancano, che cosa aprirebbero, e in che ordine
> conviene farle. Ogni riga implementata si sposta in §6-bis col numero del giro.
>
> In coda al catalogo ci sono le **trappole misurate**: superfici che sembrano
> giuste e vengono intercettate da un altro lettore. Leggerle prima costa un
> minuto e fa risparmiare una lezione.

## ⛔ Disclaimer: addestramento reale, mai testing

Questo protocollo si usa **soltanto per insegnare conoscenza vera sul mondo
reale**. Non si usa per fixture, entità inventate, nonce words, fatti sintetici,
prompt giocattolo o dati creati per vedere se un meccanismo funziona.

- Ogni fatto candidato deve essere vero, utile oltre la sessione e sostenuto da
  una fonte identificabile.
- Ogni fatto appreso e promosso è destinato a restare nella KB versionata.
- È vietato inserire fatti “di prova” pensando di cancellarli alla fine.
- È vietato usare risposte memorizzate come sostituto della comprensione.
- Replay, transfer, contrasto e ablation sono verifiche causali
  dell'apprendimento su **altri fatti veri**, non casi di test da persistere.
- Se servono dati artificiali o una fixture, fermare questo protocollo e aprire
  un'attività di sviluppo/testing separata. Non contaminare la KB di training.

Un turno può essere utile come diagnosi anche se non produce conoscenza, ma non
conta come addestramento riuscito. Una sessione con zero nuovi fatti veri deve
essere riportata come `diagnostic`, mai come `trained`.

## 1. Vincoli assoluti

Questi vincoli prevalgono sulla voglia di far diventare verde un prompt.

1. **Lingua naturale, non API travestita.** Il teacher non usa Prolog/P0,
   `!assert`, MCP, JSON di tool, nomi di predicati interni, arità o tuple nella
   chat. Un esperto del dominio che ignora lo schema della KB deve poter
   formulare la lezione.
2. **KB-first.** Lessico, cue, sinonimi, forme interrogative, frame, risposte e
   conoscenza di dominio vivono nella KB. Il C può eseguire soltanto meccaniche
   stabili come tokenizzazione, ordinamento, binding, aritmetica e inferenza.
3. **Nessun fatto di dominio scritto a mano.** Non modificare `.p0` per inserire
   direttamente ciò che parrot0 avrebbe dovuto imparare parlando. Una modifica
   manuale è ammessa solo per aprire un meccanismo generale di
   meta-comprensione, mai per chiudere il caso corrente.
4. **Nessun successo apparente.** Una risposta plausibile, l'eco della lezione o
   il solo fatto aggiunto non provano comprensione. Replay e transfer sono
   obbligatori.
5. **No deception.** Una risposta falsa o non pertinente è peggiore di “non
   capisco”. Ogni misclaim invalida la promozione finché non è spiegato.
6. **Conservare le strutture secondarie.** Gap, ipotesi perdenti, provenienza,
   candidati `partial`/`failed` e tracce non si cancellano perché sembrano
   rumore. Si classificano.
7. **Persistenza esplicita.** Nessuna conoscenza è dichiarata acquisita prima di
   `/save`, del diff riga per riga e della rilettura in un processo nuovo.
8. **Piccoli incrementi, sempre versionati.** Ogni incremento acquisito nella
   KB si committa e si pusha, anche se piccolo, parziale o incompleto. Non
   aspettare la fine del dominio o della sessione. Questo non impone un commit
   per turno o per singolo fatto: il checkpoint segue un'unità causale leggibile
   — una lezione verificata o un piccolo gruppo inseparabile — e non una
   frequenza rituale. Non c'è merito nel numero dei commit; c'è un errore nel
   lasciare fuori dal repository un guadagno già osservabile.
9. **Nessuna suite come surrogato del training.** Questo protocollo non è un
   processo di testing. Per una sessione KB-only non si lanciano suite. Se il
   coding agent modifica il motore per chiudere un gap generale, il solo gate
   software ammesso durante questo lavoro è `make soft-test`, una volta, salvo
   istruzione esplicita diversa dell'operatore.

Prima di agire leggere integralmente:

1. `MANTRA.md`;
2. `PRINCIPLES.md`;
3. `docs/plans/apprendimento-assistito.md`;
4. per routing e persistenza, `docs/session-and-provenance.md`.

> **Per una sessione MISTA** — comprensione, metacomprensione, KB viva, cioe'
> quella che facciamo davvero, con il motore che si muove insieme alla KB — il
> protocollo qui sotto non basta: serve `docs/plans/procedura-crescita-kb.md`,
> che misura quanto costa un giro e in che ordine conviene farlo. In una riga:
> **guarda prima di ipotizzare, prova in KB prima di compilare, e data ogni
> misura.** Questo file resta la procedura per l'insegnamento puro.

## 2. Parametri obbligatori della sessione

Il coding agent deve fissare e riportare questi parametri prima di aprire la
chat:

| Parametro | Significato |
|---|---|
| `DOMINIO` | area reale e circoscritta da insegnare |
| `OBIETTIVO` | che cosa parrot0 dovrà sapere o comprendere a fine sessione |
| `BUDGET` | durata massima o numero massimo di lezioni |
| `FONTI` | fonti autorevoli da cui derivano i fatti |
| `TARGET_WORLD_FACTS` | numero minimo di nuovi fatti veri desiderati |
| `TARGET_CAPABILITIES` | eventuali nuove forme/costruzioni da acquisire |
| `STOP_CONDITION` | quando fermarsi anche se resta tempo |

Valori predefiniti prudenti, se l'invocazione non li specifica:

- `BUDGET`: 15 minuti;
- `TARGET_WORLD_FACTS`: 3;
- `TARGET_CAPABILITIES`: 0 o 1;
- `STOP_CONDITION`: primo misclaim non spiegato, fonte insufficiente, oppure
  meta-gap non chiudibile naturalmente.

## 3. Definizioni di conteggio

Il protocollo usa contatori diversi. Non fonderli in un solo numero.

| Simbolo | Conteggio |
|---|---|
| `B0` | fatti totali mostrati all'avvio del processo iniziale |
| `R0` | regole totali mostrate all'avvio del processo iniziale |
| `S` | clausole che `/save` dichiara di avere instradato |
| `W` | nuovi fatti ground veri sul mondo reale nel diff della KB |
| `L` | nuovi fatti linguistici/metalinguistici acquisiti parlando |
| `C` | nuove costruzioni, regole o procedure persistite |
| `P` | nuove clausole di provenienza, genealogia o gap |
| `O` | altre clausole persistite e classificate esplicitamente |
| `X` | clausole false, non verificate, di test o inspiegate |
| `B1` | fatti totali mostrati da un processo nuovo dopo il salvataggio |
| `R1` | regole totali mostrate da un processo nuovo dopo il salvataggio |

`S` è un dato del router, non il guadagno semantico. Il numero richiesto come
risultato di training è soprattutto `W`, accompagnato da `L`, `C`, `P` e `O`.

La riconciliazione obbligatoria è:

```text
clausole aggiunte e classificate nel diff = W + L + C + P + O
clausole invalide                          = X = 0
```

Se il totale classificato non coincide con ciò che si osserva nel diff o con
`S`, non indovinare: ispezionare routing, duplicati e clausole multi-linea e
spiegare la differenza. Le righe Git non equivalgono automaticamente a fatti:
una clausola può occupare più righe.

## 4. Gate di verità prima della chat

### Step 4.1 — Scegliere conoscenza persistibile

Preparare da 3 a 10 proposizioni vere, piccole e interrogabili. Preferire fatti
che si compongono fra loro: identità, classe, quantità, luogo, causa, parte,
regola o procedura verificabile.

Non scegliere:

- dati inventati o adattati per semplificare il parser;
- opinioni presentate come fatti;
- valori rapidamente variabili senza data o contesto;
- informazioni mediche, legali o finanziarie prive di fonte primaria;
- frasi copiate in massa da una fonte;
- fatti che parrot0 conosce già, salvo servano a comporre una relazione nuova.

### Step 4.2 — Verificare le fonti

Per ogni proposizione registrare:

```text
ID | proposizione | fonte | data/revisione | grado di certezza | note di scope
```

Regole:

- privilegiare fonti primarie, istituzionali o enciclopediche autorevoli;
- per fatti instabili registrare “valido al YYYY-MM-DD”;
- per fatti contestati conservare attribuzione e contesto, non un assoluto;
- se la fonte non sostiene esattamente la proposizione, scartarla;
- non insegnare ciò che il coding agent “ricorda” senza verifica quando c'è un
  rischio ragionevole di errore o mutamento.

### Step 4.3 — Preparare held-out reali

Per ogni forma o relazione che si vuole insegnare preparare:

- 1 frase reale per la lezione;
- 1 replay della frase che prima falliva;
- almeno 3 esempi reali held-out con nomi e valori diversi;
- almeno 2 parafrasi naturali;
- almeno 1 quasi-esempio reale che non deve essere assorbito;
- 1 composizione con una capacità già posseduta.

Gli held-out non vanno pronunciati nella spiegazione. Se sono asserzioni, devono
essere fatti veri destinati a restare; se sono domande, devono interrogare fatti
veri già insegnati.

## 5. Preflight del repository

Eseguire e annotare, senza alterare modifiche preesistenti:

```sh
git status --short
git diff --check
```

Poi:

1. identificare i file già modificati e considerarli proprietà dell'operatore;
2. non ripristinare, riordinare o includere nel commit modifiche estranee;
3. aprire `make chat` in un processo nuovo;
4. verificare che non compaia alcun `PARSE ERROR`;
5. registrare `B0`, `R0`, versione e profilo mostrati all'avvio.

Se il boot ha errori di parsing, il training non parte. Correggere prima il
caricamento senza inserire la conoscenza del dominio che si voleva insegnare.

## 6. Ciclo didattico, una lezione alla volta

Non inviare un curriculum intero in blocco. Dopo ogni turno leggere la risposta
e classificare l'esito.

### Step 6.1 — Baseline naturale

Porre una domanda normale sul primo fatto, in almeno due formulazioni. Non dire
ancora la risposta.

Classificare entrambe le risposte:

- `KNOWN_CORRECT`: il fatto è già conosciuto; scegliere un altro fatto;
- `WALL`: non sa o chiede chiarimento;
- `WRONG`: risposta falsa;
- `IRRELEVANT`: risposta grammaticalmente valida ma non ancorata alla domanda;
- `PARTIAL`: risponde solo a una parte;
- `AMBIGUOUS`: espone più letture senza scelta giustificata.

Una risposta `WRONG` o `IRRELEVANT` attiva subito la stop condition finché non è
stata capita la causa. Non correggerla semplicemente fornendo una risposta da
memorizzare.

### Step 6.2 — Lezione in lingua naturale

Spiegare il fatto come lo spiegherebbe un docente umano. Se manca una forma,
spiegare forma e ruoli con parole normali ed esempi veri, per esempio:

> In questa frase il luogo viene prima della cosa ospitata; “è sede di” indica
> che la seconda cosa si trova nella prima.

Sono ammessi citazione/menzione di parole e schemi discorsivi naturali. Non sono
ammessi simboli della rappresentazione interna o istruzioni su quale predicato
scrivere.

Dopo la lezione chiedere a parrot0, in lingua naturale:

1. che cosa ha capito;
2. quale parte della frase svolge ciascun ruolo;
3. che cosa gli manca ancora, se non può applicarla.

Non contare un “ho capito” come evidenza.

### Step 6.3 — Replay immediato

Riproporre il turno originale senza suggerimenti. Il replay passa soltanto se:

- la lettura è corretta;
- la risposta usa davvero la nuova conoscenza;
- non viene restituita una risposta preconfezionata;
- su richiesta parrot0 può indicare la lezione o il fatto che la sostiene.

Se fallisce, classificare il gap secondo M0–M14 di
`docs/plans/apprendimento-assistito.md`. Non adattare la frase finché entra in un
frame già noto: quello misura la pazienza del teacher, non l'apprendimento.

### Step 6.4 — Transfer reale

Presentare i tre held-out reali, uno per volta. Cambiare almeno:

- nomi delle entità;
- valori;
- ordine o superficie della frase, quando semanticamente lecito;
- uno dei contesti in cui la forma compare.

Registrare `Transfer@3 = corretti / 3`. Per promuovere una capacità nuova è
richiesto `3/3`. Un fatto isolato può essere promosso come fatto, ma non deve
essere descritto come nuova capacità generale.

### Step 6.5 — Parafrasi, contrasto e composizione

Eseguire nell'ordine:

1. due parafrasi equivalenti;
2. un quasi-esempio che deve essere escluso;
3. una domanda che combina la nuova conoscenza con una relazione già nota.

Una capacità generale passa con:

```text
Paraphrase = 2/2
Contrast   = 1/1
Composition = 1/1
```

### Step 6.6 — Ablation e ripristino

Se parrot0 supporta il retract parlato per quella lezione:

1. chiedere in lingua naturale di dimenticare/correggere la lezione;
2. verificare che la capacità collegata scompaia e che il resto rimanga vivo;
3. insegnare di nuovo la stessa conoscenza vera;
4. rieseguire un held-out;
5. verificare che lo stato finale contenga la verità ripristinata.

Non introdurre mai un fatto fittizio per rendere facile l'ablation. Se il retract
non è disponibile, segnare `Ablation = unavailable` e non dichiarare chiuso lo
strato metalinguistico.

### Step 6.7 — Retention breve

Dopo almeno cinque turni pertinenti ma diversi, interrogare nuovamente il fatto
o usare la costruzione su un altro fatto vero. Registrare `Retention = pass/fail`.

### Step 6.8 — Checkpoint causale: save, verifica, commit e push

Quando una lezione o un piccolo gruppo inseparabile supera i gate, non passare a
un nuovo incremento indipendente lasciando il precedente soltanto nella
sessione. Eseguire subito, nell'ordine:

1. inventario e quarantena del §8;
2. `/save` e conteggio del §9;
3. verifica con un processo nuovo del §10;
4. aggiornamento del report del §12;
5. commit e push del §13.

Poi si può continuare il curriculum. Il processo originale può restare aperto;
il processo nuovo serve a provare la persistenza e viene chiuso dopo la
verifica.

Non creare checkpoint meccanici dopo ogni turno. Crearlo quando il diff racconta
una causa riconoscibile: “questa lezione ha prodotto questi fatti/capacità”. Non
rimandarlo soltanto perché l'ora, il dominio o la sessione non sono finiti.

## 6-bis. FORME DI APPRENDIMENTO — che cosa si può insegnare parlando

*Il catalogo delle superfici con cui una lezione entra in KB senza toccare il C.
È la lista da cui si sceglie prima di aprire una chat, ed è anche il metro del
progresso: **una forma nuova qui vale più di cento fatti**, perché apre una
classe intera invece di un membro.*

> **Come si legge.** Ogni riga è una superficie che parrot0 accetta e ciò che ne
> ricava. Dove c'è ⚠, è una trappola misurata: la forma esiste ma qualcosa la
> intercetta, e chi insegna deve saperlo.
>
> **Come si aggiunge una riga.** Si trova un muro, si chiude con una *forma*
> (`turn_form/3` + `turn_form_act/2`, o una `learnable/3`, o una regola in
> `.p0`), si verifica sul prompt che l'ha scoperta, e si aggiorna questa lista.
> Non si aggiunge una riga per un fatto: un fatto non è una forma.

### A. Classi e appartenenza

| si dice | parrot0 ne ricava |
|---|---|
| `X è un Y` / `X is a Y` | `Y(X)` — appartenenza |
| `X non è un Y` | il fatto negativo: un «no» guadagnato, non dedotto dal fallimento |
| `A, B e C sono Y` | tre fatti, con il predicato portato al singolare (gen507/4) |
| `un Y è uno Z` | la specie dentro la specie: da lì l'appartenenza è transitiva (gen507/1) |
| `ogni Y è P` | una REGOLA che il risolutore concatena — non un fatto |
| `ogni Y ha Z` | un fatto sulla SPECIE; i membri lo ereditano (gen507/15) |
| `nessun A è un B` | esclusione fra classi: chiude il mondo su entrambe |
| `the <classe>s are A, B and C` | **l'estensione dichiarata completa**: pone i membri *e* dichiara che sono tutti. Da lì «no» è una risposta guadagnata, non un'alzata di spalle (gen507/44, forma #7) |

### B. Relazioni

| si dice | parrot0 ne ricava |
|---|---|
| `V is a relation verb` | apre `X V Y`, la domanda polare, l'enumerazione |
| `X V Y` | il fatto binario |
| `X does not V Y` | il fatto negativo (gen507/19) |
| `x V y means x W y` | una COSTRUZIONE: la superficie si riscrive in una che già funziona, e da gen507/14 vale anche in interrogazione |
| `X is a relation` | `relation_noun`: apre insieme `il V di X è Y` **e** `che V è X?` (gen507/5) |
| `V chains` | la relazione è transitiva: la catena si percorre (gen507/27) |
| `V is the inverse of W` | un fatto solo, visto dai due lati: asserire `X V Y` risponde anche a `Y W X`, e si legge nei due sensi (gen507/40) |
| `V goes both ways` | la relazione è simmetrica: `X V Y` risponde anche a `Y V X` (gen507/41) |
| `V has one value` | funzionale: se il posto è occupato da un altro valore, il «no» è **guadagnato** senza elencare il mondo (gen507/45, forma #2) |
| `V implies W` | sussunzione: chi tiene `X V Y` risponde anche a `X W Y`, **in un verso solo** (gen507/46, forma #3) |
| `V holds of itself` | riflessiva: `X V X` è vero senza che nessuno lo dica — e resta falso per le relazioni che non lo sono (gen507/48, forma #1) |
| `V rules out W` | esclusione fra relazioni: chi tiene `X V Y` ha già il «no» su `X W Y`. Una sola riga risponde per tutte le coppie, presenti e future (gen507/47, forma #4) |

> **Le tre proprietà di una relazione** — transitiva (`V chains`), inversa (`V is
> the inverse of W`), simmetrica (`V goes both ways`) — sono le tre domande che
> conviene farsi su ogni relazione nuova appena la si insegna. Il motore sa
> *percorrere la catena* e *scambiare i posti*; non sa **quando**, e quel «quando»
> è esattamente la conoscenza che si dice.
| `X è più V di Y` | il comparativo, una volta che `V` è un verbo di relazione (gen507/8) |
| `X ha un Y` | `has_part` (gen507/14) |

### C. Attributi e valori

| si dice | parrot0 ne ricava |
|---|---|
| `X è rosso` | l'attributo, sotto la relazione che il GENERE del valore dichiara |
| `correction: X è Y` · `actually X is Y` | SOSTITUISCE invece di aggiungere (gen507/16-17) |
| `X pesa N` · `X costa N` · `X è stato costruito nel N` | forme dichiarate: peso, prezzo, anno (gen507/20-22) |
| `X viene da Y` · `X serve a Y` · `X può Y` · `X somiglia a Y` | origine, scopo, abilità, somiglianza (gen507/21-25) |

### D. Parole, forme e ruoli

| si dice | parrot0 ne ricava |
|---|---|
| `"superficie" è un <classe>` | la superficie entra nella classe come MENZIONE, verbatim (gen507/1 sulle virgolette) |
| `la parola X è un …` | la stessa cosa, senza virgolette |
| `X è un altro modo per dire Y` | `intent_cue`: una formulazione in più per un intento |
| `X è un modo per chiedere Y` | `answer_frame`: una FORMA DI DOMANDA in più |
| `X è un altro modo per introdurre Y` | `segment_role`: un introduttore di span |
| `X è un marcatore di condizione` | la classe grammaticale di una parola |
| `the plural of X is Y` | `plural_of/2` — un plurale irregolare, e da lì l'enumerazione e il conteggio lo riconoscono (gen507/49, forma #24) |
| `the past of X is Y` · `another form of X is Y` | `irregular_verb_form/2` — una forma che nessuna regola di suffisso produce, e da lì la frase che la usa si legge (gen507/50, forma #25) |
| `the <primato> <classe> is <risposta>` | un primato del mondo: `the deepest cave is krubera` → `which cave is the deepest?` risponde (gen507/42) |
| `the italian for X is Y` | `tr/2` — e da lì la parola italiana entra in **tutte** le classi inglesi che X tocca (gen507/39). ⚠ non `X in italian is Y`: quella la prende il lettore che cambia lingua |

### E. Condotta e ragionamento

| si dice | parrot0 ne ricava |
|---|---|
| `quando <situazione> allora <mossa>` | un PIANO: `plan_move(Situazione, Ordine, Mossa)`. L'ordine è quello in cui le mosse vengono insegnate (gen507/36) |
| `step for X is <cosa fare>` | un PASSO della procedura per X, in coda ai precedenti (gen507/38). ⚠ non `to make X you …`: quella superficie è del sintetizzatore di artefatti |
| `forget that X is a Y` | ritratta: la lezione si disfa come si è fatta |

### F. Procedure eseguibili

*KB-first non è solo fatti: sono anche **procedure e processi** nella KB. Una
procedura complessa non è un operatore complesso — è una **catena di operatori
semplici**, ed è la catena che si insegna.*

| si dice | parrot0 ne ricava |
|---|---|
| `rule for X is <operatore>` | un passo della procedura X, in coda ai precedenti (gen507/43) |
| `apply X to <testo>` · `applica X a <testo>` | esegue la catena e rende il risultato |

**Operatori disponibili** (il motore li esegue; le *classi di caratteri* sono KB):

| operatore | effetto |
|---|---|
| `keep <classe>` · `drop <classe>` | tiene / toglie i caratteri della classe |
| `reverse` | rovescia |
| `count` | sostituisce col numero di caratteri |
| `upper` · `lower` | maiuscolo / minuscolo |
| `first N` · `last N` | i primi / gli ultimi N caratteri |
| `apply <altra procedura>` | **chiama** un'altra procedura: una lezione si costruisce su quelle già date invece di ripeterne i passi (gen507/51, forma #43) |
| `replace <a> with <b>` | sostituzione (gen507/52, forma #46) |

| `the <nome> letters are A, B, C` | **insegna una classe di caratteri**, e da subito è un operatore in più da comporre (gen507/53, forma #44) |

Classi già in KB: `vowel`, `consonant`, `digit`. Una classe nuova — le consonanti
di un'altra lingua, le cifre pari — è **una riga di `.p0`**, e da subito un
operatore in più da comporre.

```
> rule for vowels is keep vowel          Held: for vowels, 1 is «keep vowel».
> apply vowels to parrot                 vowels(«ao»)
> rule for shout is keep consonant
> rule for shout is upper
> apply shout to parrot                  shout(«PRRT»)
> rule for howmany is keep vowel
> rule for howmany is count
> apply howmany to parrot                howmany(«2»)
```

⚠ **Non ancora esprimibile:** una procedura con **cicli o condizioni** — la radice
quadrata a mano, per dire. Serve un operatore che ripeta finché una condizione
regge, e va progettato come primitiva del motore (un modo nuovo di *fare*, non un
ordine nuovo): è il prossimo gradino di questa sezione.

⚠ **Non `step for X is …`** per le procedure: quella superficie è dei passi di una
*ricetta* (`process_step`, gen507/38). La distinzione è anche giusta nel merito —
un passo di ricetta è un'istruzione a una persona, un passo di procedura è una
**regola di trasformazione** che parrot0 esegue.

---

## 6-ter. IL CATALOGO ESTESO — le forme da implementare, scritte in anticipo

*Questa è la lista delle forme che **non** esistono ancora e che intendo
aggiungere, scritta prima di scriverle così si può discutere l'ordine, togliere
quelle sbagliate e aggiungerne di migliori. Ogni riga implementata si sposta nel
catalogo §6-bis con il numero del giro.*

> ### Il bersaglio e il metodo
>
> **Oggi §6-bis conta 33 forme** (39 righe meno 6 che sono *operatori*, non
> forme). Il bersaglio è **33 × 50 = 1650**.
>
> Non ci si arriva scrivendo 1650 righe a mano: si arriva **riconoscendo che lo
> spazio è generativo**. Una forma è il prodotto di assi indipendenti —
>
> | asse | valori |
> |---|---|
> | **famiglia semantica** | classe, relazione, attributo, quantità, tempo, spazio, parola, discorso, condotta, procedura, verifica… |
> | **atto** | asserire · negare · chiedere (sì/no) · chiedere (quale) · enumerare · contare · correggere · ritrattare · confrontare · ordinare · esemplificare · spiegare |
> | **portata** | un membro · una specie · tutti · la maggior parte · con eccezione |
> | **lingua** | EN · IT (e ogni lingua che `tr/2` apre, giro /39) |
>
> Il prodotto degli assi **è** il catalogo: 11 famiglie × 12 atti × 5 portate ×
> 2 lingue supera già il bersaglio. Ma un prodotto cartesiano scritto per esteso
> sarebbe un elenco morto — **la maggior parte delle celle non ha senso**
> («ritratta la maggior parte dei tempi in italiano»).
>
> Quindi il catalogo si costruisce così, ed è anche il motivo per cui vale la
> pena costruirlo: **si scrive la matrice di una famiglia per volta, e si tiene
> solo ciò che qualcuno direbbe davvero.** Ogni cella scartata è
> un'informazione — dice che quella combinazione non è una domanda che una
> persona fa — e ogni cella tenuta è una forma da implementare.
>
> **Lo si riempie a lotti**, un commit per lotto, così si può fermare o
> correggere la rotta prima che diventi lavoro sprecato. Il lotto 1 (§G–R, le
> prime 50) è qui sotto; i successivi si aggiungono in coda con l'intestazione
> del lotto.

**Legenda:** 🔴 da implementare · 🟡 esiste in parte, va aperta alla voce ·
✅ fatta (allora sta anche in §6-bis)

**Notazione.** In queste tabelle `X`, `Y`, `V`, `<valore>`, `<fonte>` sono
**segnaposto**: vanno sostituiti. Dove compare una parola concreta — `red`,
`vowels` — è solo un esempio di riempimento, non un letterale che parrot0
cerca. La regola per leggere una riga: *se la parola potrebbe essere un'altra
senza cambiare la forma, è un segnaposto.*

### G. Altre proprietà di una relazione

Il motore sa già *percorrere una catena* e *scambiare i posti*. Queste dicono
altre cose che sa fare e non sa **quando**.

| # | si dice | ne ricava | |
|---|---|---|---|
| 1 | `V holds of itself` | riflessiva: `X V X` vale sempre | ✅ gen507/48 |
| 2 | `V has one value` | funzionale: un secondo valore è una **correzione o un conflitto**, non un fatto in più — estende a ogni relazione ciò che il giro /16 fa per gli attributi | ✅ gen507/45 |
| 3 | `V implies W` | se `X V Y` allora `X W Y`: sussunzione fra relazioni | ✅ gen507/46 |
| 4 | `V excludes W` — reso `V rules out W` | se `X V Y` allora **non** `X W Y`: un «no» guadagnato senza elencare | ✅ gen507/47 |
| 5 | `V goes from <classe> to <classe>` | vincolo di tipo: parrot0 può **rifiutare** un fatto assurdo invece di tenerlo | 🔴 |
| 6 | `V is measured in <unità>` | l'unità del valore, per la resa e per i confronti | 🔴 |

### H. Tassonomia e insiemi

| # | si dice | ne ricava | |
|---|---|---|---|
| 7 | `the X are A, B and C` | estensione **dichiarata completa** → da lì il «no» è chiuso, non «non l'ho derivato» | ✅ gen507/44 |
| 8 | `X and Y are the same thing` | identità fra entità: ciò che vale per uno vale per l'altro | 🔴 |
| 9 | `every X is either Y or Z` | partizione: se non è Y allora è Z | 🔴 |
| 10 | `the opposite of X is Y` | antonimia fra concetti (oggi c'è `opposite` come dominio, non come lezione) | 🟡 |
| 11 | `most X are Y` | tipicità: vera in generale, **non** universale — e la risposta deve dirlo | 🔴 |
| 12 | `X is a Y except when Z` | l'eccezione dichiarata, invece di una regola falsa | 🔴 |

### I. Quantità, misure, conversioni

| # | si dice | ne ricava | |
|---|---|---|---|
| 13 | `X is N <unità> long` | misura con unità, non un numero nudo | 🔴 |
| 14 | `N <unità> is M <unità>` | conversione: apre i confronti fra misure dette in unità diverse | 🔴 |
| 15 | `X is N times bigger than Y` | rapporto, non solo ordine | 🔴 |
| 16 | `X is between A and B` | intervallo | 🔴 |

### L. Tempo

| # | si dice | ne ricava | |
|---|---|---|---|
| 17 | `X happened before Y` | ordine temporale (transitivo, con l'inverso `after`) | 🔴 |
| 18 | `X lasts N <unità di tempo>` | durata | 🔴 |
| 19 | `X happens every <periodo>` | ricorrenza | 🔴 |
| 20 | `X was true until Y` | validità che finisce: un fatto con una scadenza | 🔴 |

### M. Spazio

| # | si dice | ne ricava | |
|---|---|---|---|
| 21 | `X is next to Y` | adiacenza (simmetrica: si dichiara con la forma del giro /41) | 🟡 |
| 22 | `X is north of Y` | direzione, con l'inverso implicito | 🔴 |
| 23 | `X is inside Y` | contenimento transitivo, distinto da `part_of` | 🟡 |

### N. Lingua e forme delle parole

| # | si dice | ne ricava | |
|---|---|---|---|
| 24 | `the plural of X is Y` | `plural_of/2`: oggi le irregolari si scrivono in un file | ✅ gen507/49 |
| 25 | `the past of X is Y` | `irregular_verb_form/2`: idem — ed è ciò che manca a «owns/own» del giro /14 | ✅ gen507/50 |
| 26 | `X means Y` | glossa: una definizione a parole | 🟡 |
| 27 | `X is short for Y` | sigle e abbreviazioni | 🔴 |
| 28 | `in <lingua> X is Y` | traduzione **per qualunque lingua** (oggi solo italiano, giro /39) | 🔴 |
| 29 | `X is a <parte del discorso>` | classe grammaticale generica, oltre le poche classi aperte oggi | 🟡 |
| 30 | `X and Y are the same word` | varianti ortografiche (`colour`/`color` — il difetto del giro /5) | 🔴 |

### O. Discorso, interlocutore, registro

| # | si dice | ne ricava | |
|---|---|---|---|
| 31 | `when I say X I mean Y` | un alias **dell'interlocutore**, non del mondo | 🔴 |
| 32 | `X is a polite way to say Y` | registro: due superfici, stesso contenuto, tono diverso | 🔴 |
| 33 | `don't call me X` | ritrattare una convenzione sociale | 🔴 |
| 34 | `answer in <lingua>` | preferenza di resa persistente | 🟡 |

### P. Condotta, piani, incertezza

*Il giro /36 ha aperto i piani. Queste li rendono governabili.*

| # | si dice | ne ricava | |
|---|---|---|---|
| 35 | `"<frase>" means <situazione>` | **meta-forma**: insegnare una SITUAZIONE nuova, non solo una mossa dentro una situazione che esiste | 🔴 |
| 36 | `never <mossa> when <situazione>` | un divieto: la condotta si dice anche in negativo | 🔴 |
| 37 | `first <mossa A> then <mossa B>` | riordinare un piano già insegnato senza rifarlo | 🔴 |
| 38 | `forget the plan for <situazione>` | ritrattare un piano | 🔴 |
| 39 | `if you are not sure, <mossa>` | condotta sull'incertezza, che oggi è cablata nel declino | 🔴 |
| 40 | `X is uncertain` | marcare conoscenza dubbia: la risposta deve **dirlo**, non tacerlo | 🔴 |

### Q. Procedure — cicli, condizioni, composizione

*Il giro /43 ha aperto le catene di operatori. Senza queste, la radice quadrata a
mano resta inesprimibile.*

| # | si dice | ne ricava | |
|---|---|---|---|
| 41 | `rule for X is repeat <op> until <cond>` | il **ciclo**: è il gradino che manca a ogni procedura vera | 🔴 |
| 42 | `rule for X is if <cond> then <op>` | la **condizione** | 🔴 |
| 43 | `rule for X is apply Y` | chiamare una procedura da un'altra: la composizione vera | ✅ gen507/51 |
| 44 | `<classe> contains <caratteri>` — reso `the <nome> letters are …` | insegnare una classe di caratteri parlando | ✅ gen507/53 |
| 45 | `rule for X is split on <char>` · `join with <char>` | dalle stringhe alle **liste** | 🔴 |
| 46 | `rule for X is replace <a> with <b>` | sostituzione | ✅ gen507/52 |
| 47 | `rule for X is sort` · `unique` | operatori su liste | 🔴 |
| 48 | `rule for X takes <n> inputs` | procedure con più di un ingresso | 🔴 |

### R. Verifica — insegnare come si controlla

| # | si dice | ne ricava | |
|---|---|---|---|
| 49 | `test: apply X to <input> gives <output>` | un **oracolo insegnato**: la procedura si verifica da sola, e un passo sbagliato si scopre subito invece che al primo uso | 🔴 |
| 50 | `X is wrong because Y` | correggere una *derivazione*, non solo un fatto: dice **quale passo** ha sbagliato | 🔴 |

---

---

## Lotto 2 — la matrice degli ATTI, applicata alle famiglie che esistono già

*Metodo: si prende una famiglia che parrot0 già capisce e le si passa accanto la
colonna degli **atti**. Ogni cella che una persona direbbe davvero è una forma.
Le celle scartate sono scritte anch'esse, perché dicono che quella combinazione
non è una domanda che qualcuno fa.*

### S. Classe e appartenenza × atti

| # | atto | si dice | |
|---|---|---|---|
| 51 | chiedere quale | `which Y is X?` — di quale classe fa parte | 🔴 |
| 52 | enumerare le classi | `what is X?` inteso come *tutte le sue classi*, non un membro | 🔴 |
| 53 | contare le classi | `how many kinds of Y are there?` | 🔴 |
| 54 | esemplificare al negativo | `name something that is not a Y` | 🔴 |
| 55 | confrontare | `is X more of a Y than Z?` | ⚪ scartata: non è una domanda che si fa a un sistema simbolico |
| 56 | correggere | `X is not a Y, it is a Z` — correzione **e** sostituzione in un turno | 🔴 |
| 57 | ritrattare la classe | `Y is not a class` — smontare una classe inventata per errore | 🔴 |
| 58 | spiegare | `why is X a Y?` esiste; manca `why is X not a Y?` | 🔴 |
| 59 | portata «la maggior parte» | `most Y are Z` (già in §H come 11, qui è la stessa cella) | 🟡 |
| 60 | portata «con eccezione» | `every Y is Z except X` | 🔴 |

### T. Relazione × atti

| # | atto | si dice | |
|---|---|---|---|
| 61 | chiedere quale (soggetto) | `who V Y?` — funziona per il verbo nudo, **non** per le forme flesse (il debito del giro /14) | 🟡 |
| 62 | chiedere quale (oggetto) | `what does X V?` esiste; manca `what is V-ed by X?` (passivo) | 🔴 |
| 63 | enumerare tutti i fatti | `list everything you know about V` | 🔴 |
| 64 | contare | `how many things does X V?` | 🔴 |
| 65 | correggere il secondo termine | `X V Z, not Y` | 🔴 |
| 66 | ritrattare un fatto | `forget that X V Y` — esiste per le classi, non per le relazioni | 🔴 |
| 67 | confrontare | `does X V more things than Z?` | 🔴 |
| 68 | ordinare | `order the Y by V` | 🔴 |
| 69 | spiegare | `why does X V Y?` — deve dire **quale regola o catena** l'ha prodotto | 🔴 |
| 70 | portata «ogni» | `everything that V Y is Z` — una regola con la relazione nel corpo | 🔴 |

### U. Attributo × atti

| # | atto | si dice | |
|---|---|---|---|
| 71 | chiedere il valore | `what colour is X?` ✅ (/5) — manca per gli attributi **insegnati a voce** | 🟡 |
| 72 | chiedere la proprietà | `what do you know about the colour of X?` | 🔴 |
| 73 | enumerare i portatori | `what is red?` — chi ha quel valore | 🔴 |
| 74 | contare | `how many red things do you know?` | 🔴 |
| 75 | negare il valore | `X is not red` — oggi si nega la classe, non l'attributo | 🔴 |
| 76 | confrontare | `is X redder than Y?` — solo per attributi **ordinabili**, e chi lo sia si dichiara | 🔴 |
| 77 | ordinare | `sort the Y by colour` | ⚪ scartata: ordinare per un valore non ordinabile non ha senso — è la 76 che va dichiarata prima |
| 78 | valore di default | `Y are usually red` — il valore che vale se nessuno dice altro | 🔴 |
| 79 | intervallo di validità | `X is red in <periodo>` | 🔴 |
| 80 | provenienza del valore | `<entità> is <valore> according to <fonte>` — **chi lo dice**: apre il disaccordo fra fonti | 🔴 |

### V. Procedura × atti

*La famiglia più giovane (giro /43) e quella con più celle vuote.*

| # | atto | si dice | |
|---|---|---|---|
| 81 | chiedere i passi | `how does X work?` — recitare la procedura invece di eseguirla | 🔴 |
| 82 | chiedere il risultato | `apply X to Y` ✅ (/43) | ✅ |
| 83 | enumerare le procedure | `what procedures do you know?` | 🔴 |
| 84 | correggere un passo | `step 2 of X is <altro>` — sostituire, non accodare | 🔴 |
| 85 | inserire un passo | `before step 2 of X do <op>` | 🔴 |
| 86 | togliere un passo | `remove step 2 of X` | 🔴 |
| 87 | ritrattare la procedura | `forget the procedure X` | 🔴 |
| 88 | spiegare l'esecuzione | `why did X give that?` — la **traccia**, passo per passo (`procedure_apply_steps` la produce già e nessuno la rende) | 🔴 |
| 89 | verificare | `test: apply X to <in> gives <out>` (= 49) | 🔴 |
| 90 | generalizzare | `X is like Y but <differenza>` — definire una procedura per differenza da un'altra | 🔴 |

### Z. Condotta × atti

| # | atto | si dice | |
|---|---|---|---|
| 91 | chiedere il piano | `what do you do when <situazione>?` — parrot0 recita la propria condotta | 🔴 |
| 92 | enumerare le situazioni | `what situations do you know?` | 🔴 |
| 93 | correggere una mossa | `move 1 for <situazione> is <altra>` | 🔴 |
| 94 | condizionare | `when <situazione> and <condizione> then <mossa>` — due condizioni | 🔴 |
| 95 | dare una ragione | `when <situazione> then <mossa> because <ragione>` — la condotta con il suo perché, e il perché è interrogabile | 🔴 |
| 96 | portata | `always <mossa>` · `never <mossa>` | 🔴 |
| 97 | priorità | `<mossa A> is more important than <mossa B>` | 🔴 |
| 98 | ammettere il limite | `if none of your moves work, say so` — la condotta terminale, dichiarata invece che cablata | 🔴 |
| 99 | osservare sé stesso | `tell me what you did in the last turn` — il piano del giro /35, chiesto a parole invece che con `/debug` | 🔴 |
| 100 | imparare dalla correzione | `that was wrong, next time <mossa>` — la correzione che **cambia la condotta**, non il fatto | 🔴 |

---

> **Stato del catalogo esteso:** 100 forme proposte su 1650. Prossimi lotti: le
> famiglie che oggi non esistono affatto (tempo, spazio, misura, discorso,
> verifica) moltiplicate per la stessa colonna di atti, poi l'asse della
> **lingua** e quello della **portata**.
>
> **Come si legge il progresso.** Il numero che conta non è quante righe ci sono
> qui, ma quante si spostano in §6-bis. Una forma proposta è un'ipotesi; una
> forma implementata è una cosa che parrot0 sa imparare.

> **Come scegliere l'ordine.** Le più cariche di conseguenze sono la **35**
> (senza situazioni nuove i piani restano confinati all'unico caso che il C
> conosce), la **41–43** (senza cicli, condizioni e chiamate le procedure sono
> catene lineari), la **7** (senza estensione completa ogni «no» resta un «non
> l'ho derivato») e la **2** (la funzionalità di una relazione è ciò che rende
> una correzione possibile ovunque, non solo sugli attributi).
>
> Le meno urgenti sono quelle di dominio (L, M, I): utili, ma aggiungono
> *membri*, non *classi*.

### Trappole misurate

- ⚠ **Le lettere singole che sono anche parole del registro chat spariscono.**
  In `the vowels are a, e, i, o and u` la «u» diventa `you` (`function_word(u,
  "you")`) e non entra come membro; la «e» sopravvive solo perché una lettera
  sola non viene mai letta come congiunzione. Per elencare lettere, usarne
  nomi (`the letter u`) o classi già in KB (`char_class`).
- ⚠ **Le cue con «it» dentro non arrivano.** Un lettore che cerca un numero per
  il pronome prende il turno («What number should I use for «it»?»). Usare
  formulazioni senza pronome.
- ⚠ **Una cue scritta in italiano che comincia per «come» è morta.** Il matcher
  confronta il turno CANONICALIZZATO, dove `come` è già `how`. Vale per ogni
  parola funzione: si dichiara su ciò che il lettore vede, non su ciò che si
  scrive (gen507/22, /33).
- ⚠ **`la R di X è Y` NON è un'asserzione**, è un'interrogazione. Per asserire
  si usa il triplo nudo `X R Y`.
- ⚠ **Una superficie già letta da un altro modulo non si ruba.** Il dispatch è
  first-match: se «X is transitive» o «a cosa serve X» sono già di qualcuno, si
  sceglie un'altra superficie e si scrive la lacuna.

---

## 7. Quando la lingua naturale non basta

Un fallimento di insegnabilità è un risultato diagnostico, non il permesso di
scrivere il fatto a mano.

### Step 7.1 — Arresto e tipizzazione

Conservare:

- input originale;
- risposta e modulo vincitore, se osservabile;
- spiegazione naturale tentata;
- tipo di gap M0–M14;
- perché le forme già note non bastano;
- classe di frasi e domini che il rimedio potrebbe liberare.

### Step 7.2 — Criterio per modificare il motore

Il coding agent può aprire un'attività di sviluppo soltanto se il rimedio:

1. è generale e non nomina il dominio corrente;
2. consente a parrot0 di imparare il membro successivo senza ricompilare;
3. mantiene tutto il vocabolario naturale nella KB;
4. conserva candidati, provenienza e letture concorrenti;
5. può essere attivato e ritratto a runtime;
6. viene accettato attraverso una spiegazione naturale, non `.p0` travestito;
7. trasferisce ad almeno un secondo dominio reale non usato per progettarlo.

È vietato aggiungere manualmente cue, frame, risposta o fatto che rendano verde
solo il prompt corrente.

### Step 7.3 — Separare sviluppo e training

Se si modifica codice o KB di infrastruttura:

1. terminare la sessione senza `/save` se contiene candidati falsi o ambigui;
2. implementare la meccanica generale secondo `MANTRA.md` e `AGENTS.md`;
3. eseguire una sola volta `make soft-test`;
4. aprire un processo `make chat` completamente nuovo;
5. ripetere il protocollo dall'inizio con fatti veri;
6. conteggiare soltanto ciò che il processo nuovo impara parlando.

Il passaggio di `make soft-test` è un gate software separato. Non aumenta `W`,
non dimostra apprendimento e non trasforma il protocollo in una suite.

## 8. Pre-save: inventario e quarantena

Prima di `/save`:

1. eseguire `/session` e annotare il percorso del dump runtime;
2. ispezionare il dump senza trattarlo come archivio o input;
3. elencare tutti i fatti e le regole prodotti dalla sessione;
4. associare ogni fatto del mondo alla fonte preparata al §4;
5. verificare che ogni asserzione usata nel transfer sia anch'essa vera;
6. verificare che non esistano fixture, nonce facts, risposte esatte
   memorizzate, fatti falsi o candidati ambigui attivi;
7. verificare che le lezioni fallite siano classificate, non promosse;
8. assicurarsi che l'ultimo stato dopo eventuale ablation contenga di nuovo le
   conoscenze vere da conservare.

Se compare anche un solo candidato `X`, non eseguire `/save`. Correggere o
abbandonare la sessione. Non affidarsi a una pulizia manuale successiva.

Non eliminare automaticamente provenienza, gap o stato dialogico perché sembrano
rumore. Se il router non ha una casa corretta per una classe di tracce, fermarsi
e registrare un gap di persistenza; non inventare un filtro distruttivo.

## 9. Salvataggio e conteggio dei fatti

### Step 9.1 — Salvataggio

Nella stessa chat eseguire:

```text
/save
```

Registrare esattamente il messaggio:

```text
parrot0: routed S clause(s) into the KB tree
```

`S` non è ancora il risultato finale.

### Step 9.2 — Diff semantico della KB

Fuori dalla chat eseguire:

```sh
git status --short
git diff -- kb
git diff --check
```

Leggere il diff riga per riga e compilare questa tabella:

| Categoria | Conteggio | Clausole |
|---|---:|---|
| fatti veri del mondo `W` | | |
| fatti linguistici/metalinguistici `L` | | |
| costruzioni/regole/procedure `C` | | |
| provenienza/genealogia/gap `P` | | |
| altre clausole spiegate `O` | | |
| false/non verificate/test `X` | **deve essere 0** | |

Contare clausole logiche, non righe aggiunte. Segnalare duplicati e aggiornamenti
separatamente. Non attribuire al training modifiche che esistevano prima.

### Step 9.3 — Numero ufficiale del guadagno

Riportare sempre queste due righe, senza sostituirle con formule vaghe:

```text
Nuovi fatti veri del mondo salvati in KB: W
Nuove clausole totali salvate e classificate: W + L + C + P + O
```

Poi riportare:

```text
Clausole dichiarate da /save: S
Clausole invalide: X (deve essere 0)
```

Se `W = 0`, l'esito non è `trained`. Può essere `diagnostic` oppure
`meta-capability-only`, purché dichiarato onestamente.

## 10. Verifica in un processo nuovo

Chiudere la prima chat e avviare di nuovo:

```sh
make chat
```

Registrare `B1` e `R1`. Senza ripetere alcuna lezione:

1. porre domande sui fatti salvati con formulazioni diverse da quelle usate per
   insegnare;
2. verificare almeno un fatto per ogni file KB modificato;
3. verificare una composizione, se è stata promossa una capacità;
4. chiedere provenienza o spiegazione dove il consumer lo consente;
5. verificare che non siano ricomparsi errori di parsing.

La persistenza passa soltanto con `FreshProcessRecall = risposte corrette /
domande = 100%` sulle conoscenze campionate. Se fallisce, non committare come
training riuscito: classificare il problema come routing, salvataggio,
raggiungibilità o rappresentazione.

`B1 - B0` è una misura diagnostica globale. Non deve sostituire `W`, perché può
includere metadati, deduzioni materializzate o altre clausole.

## 11. Metriche e gate di promozione

Calcolare e riportare:

```text
LessonYield            = lezioni promosse / lezioni tentate
Transfer@3             = held-out corretti / 3 per capacità
Paraphrase             = parafrasi corrette / parafrasi provate
ContrastPrecision      = quasi-esempi esclusi / quasi-esempi provati
Composition            = composizioni corrette / composizioni provate
AblationFidelity       = capacità rimosse correttamente / ablation tentate
Retention              = capacità ancora attive / capacità ricontrollate
FreshProcessRecall     = risposte corrette nel nuovo processo / domande
FalseUnderstandingRate = falsi “capito/appreso” / dichiarazioni di successo
WorldKnowledgeGain     = W
TotalPersistedClauses  = W + L + C + P + O
```

Gate minimo per una sessione `trained`:

- `W >= 1`;
- `X = 0`;
- tutte le fonti registrate;
- `FreshProcessRecall = 100%` sul campione;
- `FalseUnderstandingRate = 0`;
- nessun errore di parsing;
- ogni modifica nel diff spiegata.

Gate aggiuntivo per dichiarare una nuova capacità generale:

- replay verde;
- `Transfer@3 = 3/3`;
- `Paraphrase = 2/2`;
- `ContrastPrecision = 1/1`;
- `Composition = 1/1`;
- ablation verde oppure dichiarata indisponibile, nel qual caso lo strato resta
  `partial`;
- provenienza visibile;
- transfer su un secondo dominio reale se il motore è stato modificato.

## 12. Report permanente

Creare un report sotto:

```text
docs/labs/apprendimento-assistito/YYYY-MM-DD-<dominio>.md
```

Il report deve contenere:

1. parametri della sessione;
2. fonti e proposizioni candidate;
3. baseline con transcript essenziale;
4. lezioni naturali pronunciate;
5. replay, transfer, parafrasi, contrasto, composizione e ablation;
6. misclaim e gap, inclusi quelli non risolti;
7. output esatto di `/save`;
8. elenco e conteggio `W/L/C/P/O/X`;
9. `B0/R0` e `B1/R1`;
10. verifica nel processo nuovo;
11. metriche finali;
12. stato finale: `trained`, `partial`, `diagnostic` oppure
    `meta-capability-only`;
13. file KB modificati e commit prodotto.

Non inserire transcript enormi: conservare i turni causalmente rilevanti. Non
omettere risposte sbagliate perché il risultato finale è corretto.

## 13. Commit e push — step obbligatorio e ripetibile

Questo step si esegue a ogni checkpoint causale del §6.8, non soltanto alla fine
della sessione. Il §11 di `docs/plans/apprendimento-assistito.md` è vincolante:
la conoscenza e le regole acquisite si committano e si pushano sempre, anche
quando rappresentano un avanzamento parziale.

La frequenza non è temporale e non è “un commit per fatto”. Una unità può
contenere più fatti se dipendono dalla stessa lezione e separandoli si perderebbe
la genealogia. Al contrario, due lezioni indipendenti non vanno trattenute per
costruire un lotto più grande.

Prima del commit:

```sh
git diff --check
git status --short
git diff -- kb docs/labs/apprendimento-assistito
```

Regole di consegna:

1. mettere in staging soltanto i file prodotti da questa sessione;
2. non includere modifiche preesistenti dell'operatore;
3. non committare la KB come training riuscito se `X > 0` o la verifica
   fresh-process fallisce;
4. il messaggio deve dichiarare il guadagno, non solo i file toccati;
5. un report `partial` o `diagnostic` può essere committato e pushato come tale,
   senza includere fatti KB invalidi e senza fingere un incremento di
   conoscenza;
6. eseguire il push subito dopo ogni commit del checkpoint;
7. se il push fallisce, non ometterlo silenziosamente: riportare comando, errore
   e commit locale rimasto da pubblicare, poi fermare il checkpoint.

Sequenza minima:

```sh
git add <solo-i-file-del-checkpoint>
git commit -m "learn(kb): add <W> verified facts about <dominio>"
git push
```

Formato consigliato:

```text
learn(kb): add <W> verified facts about <dominio>
```

oppure:

```text
learn(meta): record partial <strato> boundary from <dominio>
```

## 14. Output finale obbligatorio del coding agent

La risposta conclusiva deve essere breve ma deve contenere tutti questi campi:

```text
Stato: trained | partial | diagnostic | meta-capability-only
Dominio: ...
Nuovi fatti veri del mondo salvati in KB (W): ...
Nuove clausole totali salvate e classificate: ...
Clausole dichiarate da /save (S): ...
Costruzioni/regole/procedure nuove (C): ...
Clausole invalide (X): 0
LessonYield: ...
Transfer@3: ...
FreshProcessRecall: ...
FalseUnderstandingRate: ...
File KB modificati: ...
Report: ...
Commit: ...
Push: pubblicato | fallito (con motivo)
Gap rimasti: ...
```

Il coding agent non deve dire “parrot0 ha imparato” se manca uno di questi tre
elementi: verità verificata, persistenza osservata e uso corretto in un processo
nuovo.

## 15. Checklist esecutiva compatta

### Prima

- [ ] Ho letto `MANTRA.md`, `PRINCIPLES.md` e il piano.
- [ ] Il dominio è reale e circoscritto.
- [ ] Ogni proposizione ha una fonte.
- [ ] Non esistono fatti inventati o destinati alla cancellazione.
- [ ] Ho preparato held-out reali.
- [ ] Ho registrato lo stato Git senza toccare modifiche altrui.

### Durante

- [ ] `make chat` parte senza parse error.
- [ ] Ho registrato `B0/R0`.
- [ ] Ho misurato la baseline prima di insegnare.
- [ ] Ho parlato soltanto in lingua naturale.
- [ ] Ho classificato muri, errori e risposte irrilevanti.
- [ ] Replay e transfer usano fatti veri.
- [ ] Nessun “ho capito” è stato contato senza prova.
- [ ] Se ho incontrato un meta-gap, non ho scritto il fatto a mano.

### Prima di salvare

- [ ] Ho ispezionato `/session`.
- [ ] Tutti i fatti candidati sono veri e fontati.
- [ ] Nessuna fixture o nonce fact è attiva.
- [ ] Eventuale ablation è stata ripristinata.
- [ ] `X = 0`.

### Dopo `/save`

- [ ] Ho registrato `S`.
- [ ] Ho letto tutto il diff della KB.
- [ ] Ho contato semanticamente `W/L/C/P/O/X`.
- [ ] Ho scritto il numero esplicito dei nuovi fatti veri salvati.
- [ ] Un processo nuovo raggiunge la conoscenza senza reinsegnamento.
- [ ] Ho creato il report permanente.
- [ ] Ho committato soltanto l'incremento del checkpoint causale.
- [ ] Ho pushato il commit prima di iniziare un incremento indipendente.

Se una casella critica resta vuota, lo stato non è `trained`.
