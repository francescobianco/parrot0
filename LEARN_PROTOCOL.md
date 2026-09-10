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
> | [**G. Ordine superiore**](#g-superfici-di-ordine-superiore--insegnare-sulle-relazioni) | `in <contesto> V stands for W` · `V behaves like W` · `V is W followed by Z` — **insegnare *sulle* relazioni: è qui che la conoscenza scala** |
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
conta come addestramento riuscito. Con zero nuovi fatti veri del mondo (`W = 0`)
la sessione è `diagnostic`, oppure `meta-capability-only` se ha acquisito una
capacità verificata e persistente; mai `trained`. Un ponte utile può rientrare
nel secondo caso: il suo valore non va nascosto né gonfiato contando deduzioni
come nuove lezioni di fatti (§G.11).

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

Se l'obiettivo include **ordine superiore**, aggiungere `TARGET_BRIDGES`
(legami da insegnare), `TARGET_REUSE` (domande reali prima irraggiungibili) e
`TARGET_CONSUMERS` (capacità che dovranno usarli). Sono obiettivi dichiarati,
non nuove soglie universali né moltiplicatori di `W`. Preparare la scheda di
§G.7: verso, premesse, scope, fonte e contrasto sono parte della lezione.

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

Per i legami di ordine superiore la classificazione è **semantica**:
`relation_chain(...)` è una clausola ground che descrive una regola, dunque
conta in `C`. Registrazioni linguistiche accessorie possono contare in `L`.
Ogni clausola entra in una sola categoria. Le risposte derivate dal ponte
misurano riuso e guadagno sul campione (§G.11), non nuovi fatti salvati `W`.

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

Per un ponte distinguere premesse già nella KB e fatti veri introdotti **dopo**
la lezione. Riservare una domanda che combini conoscenze preesistenti senza
insegnarne la conclusione. La fonte deve sostenere anche il ponte con il suo
verso e i suoi limiti: un'associazione plausibile fra parole non basta.

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

Per un ponte, premesse `KNOWN_CORRECT` sono proprio la base da riusare: non
scartarle. Registrare separatamente la baseline delle premesse e quella della
conseguenza. Una conseguenza già corretta può avere un supporto indipendente,
che rende necessario un altro caso per attribuire causalmente il guadagno.

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

Per un ponte si ritratta **il legame**, conservando i fatti sorgenti. Devono
scomparire gli usi sostenuti soltanto da quel legame; una risposta sostenuta
anche da un'altra prova può restare corretta (§G.8–G.9). Non cancellare altre
conoscenze per ottenere artificialmente la scomparsa attesa.

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
> **Come si aggiunge una riga.** Si trova un muro, si chiude con una *forma*, si
> verifica sul prompt che l'ha scoperta, e si aggiorna questa lista. Non si
> aggiunge una riga per un fatto: un fatto non è una forma.
>
> **E dal gen507/62 una forma non costa C.** L'atto è un *termine*, non
> un'etichetta che il motore smista:
>
> ```prolog
> turn_form(forget_fact, 1, text("forget that")).
> turn_form(forget_fact, 2, slot(subject)).
> turn_form(forget_fact, 3, relation(relation)).
> turn_form(forget_fact, 4, rest(object)).
> turn_form_act(forget_fact, "op(retract, relation, [subject, object])").
> turn_form_reply(forget_fact, forgot_fact).
> ```
>
> **Operazioni:** `assert` · `assert_neg` · `retract` · `retract_all` · `match` ·
> `count`. **Argomenti:** il nome di uno slot · `free` (il posto della risposta)
> · `next` (il prossimo indice libero, per tutto ciò che è ordinato).
> Nel template si possono usare gli slot della forma più `{result}` e `{count}`.
>
> **Una forma può dichiarare più operazioni**, che si eseguono in ordine — e la
> resa è quella della forma. *Correggere* non è *asserire*: è togliere ciò che
> c'era e mettere il nuovo, e sono due righe.

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
| `X is the capital of Y` | la **capitale** di un paese (gen507/78) |
| `X goes <verso>` | il **verso** di un animale (gen507/78) |
| `X lies in the continent Y` | il **continente** di un paese (gen507/78) |
| `X is there to <scopo>` | lo **scopo**, seconda superficie accanto a «X is used for Y» (gen507/78) |
| `X wrote Y` | l'**autore** di un'opera (gen507/77) |
| `X borders Y` | il **confine**, asserito nei due versi perché confinare è reciproco (gen507/77) |
| `X pays in Y` | la **valuta** di un paese (gen507/77) |
| `X is the symbol for Y` | il **simbolo chimico** di un elemento (gen507/77) |
| `X contains Y` | `part_of/2` detta dal verso del contenitore (gen507/76) |
| `X is caused by Y` | `causes/2` detta dal verso dell'effetto (gen507/76) |
| `X is a kind of Y` | `kind_of/2`, la **catena dei tipi**: da lì `X` eredita le proprietà di ogni tipo sopra di sé (gen507/76) |
| `X is a member of <classe>` | mette X nella **tassonomia percorribile** *e* nella classe: due conoscenze diverse sullo stesso fatto, una frase sola (gen507/73) |
| `<classe> are typically P` | una proprietà della **specie**: ogni membro presente e futuro la eredita (gen507/73, forma #78) |
| `what does X get from the kind?` | che cosa X **eredita** — `inherits/2` esisteva come regola e nessuna superficie ci arrivava (gen507/73) |
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
| `V chains with opposite W` | **tutta la scheda di una relazione in una frase**: entrambe sono verbi di relazione, entrambe transitive, e l'una è l'inverso dell'altra — cinque fatti (gen507/79) |
| `X comes before Y` | l'**ordine nel tempo**: pone il fatto *e* dichiara che la catena si percorre e che il verso opposto si chiama `follows` — tre fatti in una frase (gen507/75, forma #17) |
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
| `X has <dimensione> N` | una **misura su una scala** (`magnitude/3`): da lì X entra nei confronti e nelle classifiche (gen507/69, forma #13) |
| `<parola> is the most <dimensione>` · `is the least <dimensione>` | la **parola del confronto** (`compare_cue/3`): senza, la scala esiste e nessuna domanda ci arriva (gen507/69) |
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
| `move N for <situazione> is now <mossa>` | **corregge** una mossa del piano invece di accodarla (gen507/72, forma #93) |
| `your plan when <situazione>?` | parrot0 **racconta la propria condotta**: chi insegna verifica che la lezione sia arrivata senza aprire uno strumento (gen507/61, forma #91). ⚠ non `what do you do when …`: quella è del registro delle capacità |
| `what situations do you know?` | le situazioni per cui ha un piano (gen507/61, forma #92) |
| `step for X is <cosa fare>` | un PASSO della procedura per X, in coda ai precedenti (gen507/38). ⚠ non `to make X you …`: quella superficie è del sintetizzatore di artefatti |
| `forget that X is a Y` | ritratta: la lezione si disfa come si è fatta |
| `who appears in V?` | **chi compare** in una relazione: dopo una lezione si vuole vedere che cosa c'è dentro, non solo che il singolo fatto risponde (gen507/65, forma #63) |
| `X is defined as <testo>` | una **definizione a parole** (`means/2`), resa da `definition of X` · `meaning of X` (gen507/64, forma #26). ⚠ una frase sola: il divisore di clausole taglia su «che», «so», «not only» |
| `X is short for Y` | una **sigla**: la forma corta che nei testi compare più del nome esteso, e senza il ponte ogni frase che la usa è un muro (gen507/74, forma #27) |
| `X is the opposite of Y` | l'**antonimia**, asserita nei due versi con una sola frase (gen507/71, forma #10) |
| `X and Y name the same thing` | **identità fra due nomi**: da lì ciò che si dice di uno vale per l'altro (gen507/63, forma #8) |
| `forget that X V Y` | ritratta un **fatto binario** (gen507/62, forma #66) — ed è la prima forma costata **zero righe di C** |

### G. Superfici di ordine superiore — insegnare *sulle* relazioni

> **Questa è la sezione che fa scalare la conoscenza, e vale la pena capire
> perché prima di usarla.**

**Rilettura del 2026-09-11, base `8e33072`.** I riferimenti gen507/80–87
identificano la campagna originaria. L'espansione distingue il meccanismo
presente, le sue condizioni d'uso e le implicazioni da verificare: è una lettura
del codice, non una nuova campagna di training né una certificazione runtime.
Il corrispettivo progettuale è il
[quadro preliminare della KB viva](docs/plans/quadro-preliminare-kb-viva.md).
Gli esempi schematici illustrano la forma; per eseguire il protocollo vanno
sostituiti con conoscenze reali verificate secondo il §4.

#### G.0 Il meccanismo: il predicato variabile

Il solver ha `apply/2` — il confine dove **il nome di una relazione è un dato,
non codice** ([`src/kb.c`](src/kb.c), documentato in [`docs/plans/thinking.md`
§0.1](docs/plans/thinking.md) come *«il pezzo più prezioso che già c'è»*):

```prolog
criterion_finding($S, $C, evidence($Measure, $V, above, $B)) :-
    quality_criterion($C, $D, threshold($Measure, above, $B)),
    apply($Measure, cons($S, cons($V, nil))),   % ← $Measure è un NOME
    gt($V, $B).
```

Da lì la conclusione che regge tutta la sezione: **non serve un dispatcher di
operatori, serve dichiararli.**

Sopra `apply/2` stanno tre predicati che il motore interroga al posto di quello
nudo:

| | domanda | perché
|---|---|---|
| `holds(V, X, Y)` | «questa relazione vale fra questi due?» | la **relazione è un argomento** |
| `holds1(C, X)` | «questo appartiene a questa classe?» | la **classe è un argomento** |
| `relation_note(V, Nota)` | «che cosa so *di* questa relazione?» | la scheda, come vista |

**La conseguenza operativa:** un lettore che chiede `holds/3` può beneficiare
di ogni nuova clausola di quella vista senza un ramo C dedicato al ponte.
Questo è il punto di estensione; non significa che tutti i lettori lo usino
già, né che ogni ponte richiami ricorsivamente gli altri (§G.5).

`apply/2` risolve **fatti e regole**, ma richiede il nome del predicato già
legato: non cerca da solo quale relazione usare. La scelta, l'enumerazione
dei candidati e i vincoli devono provenire da altra conoscenza. «Ordine
superiore» designa qui relazioni trattate come dati in un interprete a
clausole; non promette quantificazione arbitraria su tutti i predicati.

Una famiglia nuova può richiedere una clausola generale in `.p0`; un nuovo
membro di una famiglia esistente si insegna parlando. Vale la gerarchia dei
mantra: **prompt → prosa → autocorrezione → promozione manuale del solo
meccanismo mancante**. L'esempio Prolog sopra serve a studiare l'architettura,
non è una lezione da incollare nella chat di addestramento.

#### G.1 Perché scala

Il contrasto è con l'accumulo di fatti isolati, non con tutta l'inferenza
ordinaria: anche una regola del primo ordine moltiplica le conseguenze. Qui
la conoscenza cresce **per legami fra relazioni**: una riga su due relazioni
può rendere utilizzabili fatti presenti *e futuri*, nel verso e nelle
condizioni che il legame dichiara.

Tre moltiplicatori riportati nell'elaborato della campagna gen507:

1. **Un ponte vale per i fatti che non esistono ancora.** `in sport plays stands
   for belongs` non converte i fatti già detti: fa sì che *ogni* «X plays Y»
   futuro possa rispondere anche a «belongs» tramite la vista. È costante la
   dimensione della dichiarazione; il costo di risoluzione può crescere. Il
   limite contestuale di questa superficie è precisato nel §G.5.
2. **Le proprietà si ereditano.** Se `ancestor` è dichiarata transitiva,
   `covers behaves like ancestor` rende transitiva anche `covers`, e la
   scheda lo dichiara:
   ```
   > tell me about the relation covers
     About «covers» I hold 2: chains, behaves like ancestor.
   ```
   Le proprietà ereditate oggi sono **quattro**: transitività, simmetria,
   riflessività e funzionalità. Inverso, implicazione ed esclusione compaiono
   anch'essi nella scheda, ma non sono ereditati da `relation_like/2`.
3. **Le relazioni si generano.** `grandparent is parent followed by parent`
   definisce una relazione **che non ha un solo fatto proprio** e risponde. Da
   *k* relazioni popolate se ne ottengono molte di più senza popolarne nessuna.

#### G.2 Dove si incastra con il resto

Questa sezione non è un'isola: è il tessuto che lega le altre.

| si combina con | base presente | implicazione da aprire o verificare |
|---|---|---|
| **i piani** (§E) | `plan_move/3` rende insegnabile una sequenza; il thinking espone operatori come dati | una mossa che interroga la vista condivisa può riusare ponti insegnati dopo il piano; non basta che il piano ne conosca il nome |
| **il ragionamento** (`/34`, `/35`) | `list_composition` enumera `composition_relation/1`, poi interroga direttamente ciascun predicato | raggiungere anche relazioni che contano come composizione richiede che questo consumo attraversi i ponti; oggi non è automatico |
| **le procedure** (§F) | `keep/drop` consulta `char_class/2` con la classe come dato | generalizzare il filtro a relazioni insegnate richiede contratto di argomenti e consumer adatto; non è già un filtro universale |
| **l'inferenza logica** | congiunzione, inclusione e default con eccezione sono dichiarati in KB | più inclusioni verso la stessa relazione producono un'unione; l'eccezione usa mancata derivabilità, non negazione esplicita (§G.6) |
| **le misure** (`/69`) | `magnitude/3`, `compare_cue/3` e i criteri di qualità trattano dimensioni o misure come dati | collegare scale richiede anche unità, verso, trasformazione e validità; una somiglianza non è una conversione |
| **lettura e rappresentazioni** | `ir_domain_claim/3` applica ponti dichiarati fra rappresentazioni | un nuovo legame può rendere leggibile conoscenza già presente; occorre conservarne ruoli e provenienza |
| **gap e autocorrezione** | `gap_source/3` sceglie obblighi e coperture applicati con `apply/2` | il sistema può imparare anche che cosa cercare e quando considerarlo coperto; le coperture devono vedere le stesse conseguenze delle risposte |

#### G.3 La regola di prudenza

**La somiglianza non autorizza una fusione.** Un'inclusione universale può
essere corretta e resta direzionale; non rende intercambiabili due relazioni.
Quando la fonte pone un limite, il ponte deve conservarlo. Le superfici con
vincolo — `for a <classe> V counts as W`, oppure
`V holds where W holds except where Z` — permettono distinzioni diverse,
da non confondere. La prova comprende una domanda vicina per la quale il ponte
non deve produrre una conclusione:

```
> anna leads team   →  does anna guides team?   Yes.     (anna è una persona)
> river leads sea   →  does river guides sea?   I don't know…
```

---

*Le superfici. Il nucleo insegna **legami fra predicati**; le forme complementari
ne dichiarano proprietà, contesti e firme o permettono di interrogarli.*

| si dice | parrot0 ne ricava |
|---|---|
| `in <contesto> V stands for W` | dichiara un ponte locale→generale con un'etichetta di contesto (gen507/80). **Oggi la regola non richiede il contesto attivo**: non usarla per promettere isolamento fra mondi. ⚠ non `means`: è il pivot del maestro delle costruzioni |
| `for a <classe> V counts as W` | il ponte **stretto di un grado**: vale solo quando il soggetto è di quella classe — «dirige» vale come «guida» per una persona, non per un fiume (gen507/81) |
| `whoever V is a <classe>` | una **classe definita da una regola** invece che elencata: «chi cresce qualcuno è un tutore» dice che cos'è un tutore *prima* che se ne conosca uno (gen507/82) |
| `V holds where both W and Z` | una **relazione definita da due**: insegnare una *regola*, non un fatto — il mantra #19 applicato alle relazioni (gen507/82) |
| `V is W followed by Z` | la **composizione**: «nonno» è «genitore» seguito da «genitore». Due relazioni note ne definiscono una terza che nessuno popola (gen507/83) |
| `V holds where W holds except where Z` | l'**eccezione detta insieme alla regola**, invece di una regola falsa che qualcuno correggerà poi (gen507/83) |
| `in <contesto> X V Y` | un **fatto sospeso**: vale quando quel contesto è attivo, e tace altrimenti (gen507/85) |
| `inside <contesto> X is Y` | un **nome che vale solo dentro un contesto**: «il capitano» è una persona precisa solo dentro una squadra (gen507/87) |
| `V is W read backwards` | il verso rovescio dentro `holds/3`, applicando direttamente `W` agli argomenti scambiati (gen507/87); l'annidamento con altre definizioni non è garantito |
| `V is a relation of kind <famiglia>` | la **famiglia** di una relazione — temporale, spaziale, sociale, causale (gen507/87) |
| `forget that V behaves like W` | **disfa** un legame di ordine superiore: una somiglianza dichiarata per sbaglio propaga proprietà che nessuno voleva (gen507/87) |
| `assume <contesto>` · `stop assuming <contesto>` | **entrare e uscire da un mondo**: senza, i fatti sospesi restano sospesi per sempre (gen507/85) |
| `what holds between X and Y?` | esplora i verbi registrati applicati alla coppia; oggi non enumera tutte le conseguenze di `holds/3` (gen507/86) |
| `V links a <classe> to a <classe>` | la **firma** di una relazione, oggi esposta nella scheda; non è ancora un controllo automatico sui fatti (gen507/86) |
| `V is W twice` | la composizione di una relazione con **sé stessa**, detta in breve: è il caso più frequente, e scriverla due volte è ciò che fa sbagliare (gen507/86) |
| `what can you say about X?` | esplora i verbi registrati con X come **soggetto**, tramite `apply/2`; non include automaticamente archi entranti o soli ponti (gen507/85) |
| `tell me about the relation V` | la **scheda** derivata da `relation_note/2`: proprietà e alcune dichiarazioni, con omissioni da conoscere (§G.5), non ancora la prova completa (gen507/84) |
| `what relations do you know?` | l'elenco di `relation_verb/1`, non il censimento di ogni predicato risolvibile (gen507/84) |
| `V holds wherever W` | un'inclusione `W → V`; ripetendola con sorgenti diverse si definisce la loro unione (gen507/82) |
| `V behaves like W` | `V` eredita le quattro proprietà supportate di `W`, non i suoi fatti né tutta la scheda (gen507/81) |

```
> in sport plays stands for belongs
> messi plays miami
> does messi belongs miami?     Yes.
> does neymar belongs miami?    I don't know…      ← nessun supporto in questo esempio
```

Il transcript è storico e schematico: non certifica appartenenze sportive
attuali. Il «non so» riguarda il supporto disponibile, non la verità nel mondo.

#### G.4 La mappa nella codebase: dalla frase all'effetto

La catena da seguire durante la diagnosi è questa:

```text
lezione naturale
  → forma e slot dichiarati in messages.p0
  → atto parametrico del lettore (assert/retract/match…)
  → dichiarazione fra relazioni nella KB
  → regole di procedures.p0, risolte mediante apply/2
  → consumer che interroga quella vista
  → risposta, decisione o nuova domanda
```

Ogni freccia può essere il punto d'arresto. Una dichiarazione salvata dimostra
il passaggio alla KB; non dimostra ancora l'uso da parte dell'ultimo consumer.

| Punto | Riferimento e simboli da cercare | Che cosa controllare |
|---|---|---|
| Risoluzione parametrica | [`src/kb.c`](src/kb.c), ramo `apply`, `solve_frame`, `list_to_args` | nome già legato, arità corretta, fatti **e** regole, budget e cicli |
| Lessico e atti didattici | [`kb/core/messages.p0`](kb/core/messages.p0), `teach_ctx_alias` fino a `unteach_like` | `turn_form`, `turn_form_act`, `turn_form_reply`; slot, argomenti e registrazioni accessorie |
| Interpretazione degli atti | [`src/brain/10-memory-knowledge.c`](src/brain/10-memory-knowledge.c), `p0_turn_form_reader`, `p0_run_op_named` | esecuzione delle operazioni e resa effettiva; una forma dichiarata deve anche raggiungere il proprio lettore |
| Semantica dei ponti | [`kb/core/procedures.p0`](kb/core/procedures.p0), blocco gen507/80–87 | `holds/3`, `holds1/2`, `relation_note/2`, `about/3`, `between_rel/3` |
| Consumo delle risposte | [`src/brain/10-memory-knowledge.c`](src/brain/10-memory-knowledge.c), `polar_class_answer`, chiamata a `holds` nel percorso polare relazionale | quali domande raggiungono la vista e quali usano percorsi diretti |
| Precedente sulle misure | [`kb/core/code-quality.p0`](kb/core/code-quality.p0), `criterion_finding/3` | il criterio nomina una misura, la regola la applica |
| Precedente fra rappresentazioni | [`kb/core/code-ir.p0`](kb/core/code-ir.p0), `ir_domain_claim/3`, `ir_domain_claim_basis/4` | un ponte opt-in conserva il proprio fondamento; non basta uguagliare etichette |
| Precedente sul metodo | [`kb/core/thinking.p0`](kb/core/thinking.p0), `thinking_operator/3`; [`thinking.md` §0.1](docs/plans/thinking.md) | operatore estratto dalla descrizione di un passo; distinguere esecutore presente e ipotesi del piano |
| Rilevamento dei bisogni | [`kb/core/procedures.p0`](kb/core/procedures.p0), `gap_source/3`, `gap_covered/3`, `gap_record/4` | anche obbligo e criterio di copertura sono predicati scelti dalla KB |
| Persistenza | [`docs/session-and-provenance.md`](docs/session-and-provenance.md) | `/save` instrada; il dump non è un archivio; casa e provenienza vanno verificate |

Un precedente distinto è
[`tests/p0t/language/higher_order_lesson.p0t`](tests/p0t/language/higher_order_lesson.p0t):
esercita «if x contains y then y is part of x», il binding degli argomenti e
il retract di una regola. Non verifica l'intero catalogo gen507/80–87. È inoltre
un test legacy marcato `mock hermetic`: non prova trasferimento nella KB viva.
Per nuove verifiche di sviluppo valgono la KB completa e la distinzione dei
mantra fra meccanica sintetica e comprensione su conoscenza reale. Qui è un
riscontro storico, non una suite eseguita né un curriculum da salvare.

**Attenzione alle omonimie tecniche:** `holds/1` è anche usato per proposizioni
atomiche; non è `holds/3`. Analogamente `apply/2` del solver e «apply X to Y»
delle procedure testuali non hanno automaticamente lo stesso contratto.
La somiglianza del nome non dimostra una condivisione del percorso.

Se la diagnosi richiede una nuova regola generale, rispettare i limiti di
[`src/kb.h`](src/kb.h): quattro argomenti per goal e otto goal nel corpo.
Spezzare una regola troppo grande in aiutanti nominati; il numero di variabili
non è il limite. Per `naf` legare prima tutti gli argomenti o usare un aiutante
ground che racchiuda l'esistenziale. Controllare gli errori di parsing al boot:
una regola scartata può sembrare una relazione senza conseguenze.

#### G.5 Il confine attuale: vista estendibile e chiusura compositiva

`apply(W, …)` risolve il predicato **W** con tutte le sue regole.
`holds(W, X, Y)` consulta anche i ponti dichiarati per **W**. Se W esiste solo
come risultato di una clausola di `holds/3`, chiamare W direttamente non
attraversa quella clausola. Dichiarare una catena che usa una relazione a sua
volta definita solo da un ponte non basta necessariamente a concatenare i due
ragionamenti. Non è un difetto di `apply`: sono due viste diverse.

| Famiglia presente | Limite osservato nella lettura statica | Conseguenza didattica |
|---|---|---|
| `context_alias/3` | `$Context` non viene confrontato con `active_context/1` | il nome del contesto non è oggi una guardia: verificare anche fuori contesto |
| `class_scoped_alias/3` | relazione locale e classe sono interrogate con `apply` | una classe nota soltanto tramite `holds1` può rispondere alla domanda di appartenenza e non attivare il ponte |
| `class_from_relation/2` | cerca la relazione sorgente direttamente | una relazione derivata soltanto da `holds` non genera automaticamente i membri della classe |
| `relation_and/or/chain/unless/reverse` | gli operandi passano da `apply`, non da `holds` | le famiglie coesistono nella vista; la loro nidificazione generale resta da verificare e aprire |
| `relation_like/2` | quattro regole ereditano quattro proprietà | non importa fatti, firma, famiglia, inversi o definizioni; non rende identiche le relazioni |
| Proprietà delle relazioni | simmetria, inverso, transitività e altri percorsi sono consumati anche da helper C nella risposta polare | un «sì» polare non prova che una catena dentro `holds` veda la stessa chiusura |
| `context_fact/4`, `context_name/3` | richiedono `active_context`; i nomi riscritti passano poi al predicato diretto | attivazione reale presente, ma niente prova automatica di annidamento o precedenze fra mondi |
| `about/3`, `between_rel/3` | enumerano `relation_verb`, poi chiamano il predicato diretto | possono omettere conseguenze visibili alla domanda polare; `about` considera solo X soggetto |
| `relation_signature/3`, `relation_family/2` | alimentano la scheda | non attivano da sole validazione, inferenza di tipi o una politica di ricerca |
| `relation_note/2` | la nota del ponte omette il contesto; catena e congiunzione non rendono entrambi gli operandi; manca una nota dedicata al ponte di classe | la scheda non è ancora inventario completo né prova di una risposta |

**Il passo ulteriore è rendere riutilizzabile anche il risultato di un ponte.**
È una classe di lavoro generale; non si chiude aggiungendo una regola per ogni
coppia incontrata. Sostituire indiscriminatamente ogni `apply` con `holds`
resta però una proposta da dimostrare: introduce ricorsione, cicli, percorsi
equivalenti e interazioni con la negazione. Deve preservare supporti, scope,
terminazione osservabile e distinzione fra fallimento e ricerca incompleta.
Il protocollo registra il confine; non autorizza fix di dominio per mascherarlo.

#### G.6 Che cosa significa ciascun legame

La fonte deve sostenere **il legame**, non soltanto due esempi compatibili.
Alcuni casi comuni non autorizzano a far passare tutti i fatti da una relazione
all'altra.

| Lezione | Impegno semantico | Inferenza da non aggiungere |
|---|---|---|
| `V holds wherever W` | ogni coppia sostenuta da W sostiene V | V non implica W; W non è sinonimo di V |
| Due inclusioni verso V | W oppure Z sono supporti sufficienti separati | non sono richiesti entrambi |
| `V holds where both W and Z` | stessa coppia X,Y sostenuta da entrambi | W(X,Y) e Z(X,T) non bastano se T è diverso da Y |
| `V is W followed by Z` | esiste un intermedio M condiviso da W(X,M) e Z(M,Y) | non si scambia l'ordine e non si conclude senza l'intermedio |
| `V is W twice` | due passi della stessa relazione | non è transitività illimitata, né distanza minima di due passi |
| `V is W read backwards` | gli argomenti si scambiano | non rende W simmetrica |
| `whoever V is a C` | avere almeno un oggetto nella relazione V è sufficiente per appartenere a C | non dice che ogni C abbia un tale oggetto; non è un bicondizionale |
| `V behaves like W` | eredita le proprietà espressamente supportate | non importa le coppie di W e non prova analogia in ogni dimensione |
| `for a C V counts as W` | appartenenza del soggetto a C come premessa aggiuntiva | non vincola la classe dell'oggetto e non equivale a uno scope temporale |

**Le clausole si aggiungono: una nuova definizione non cancella altri supporti.**
Se V ha già fatti diretti o altre regole, la nuova congiunzione non li restringe.
Se si aggiunge un'eccezione a un ramo, un fatto diretto di V può ancora renderlo
vero. Definire, specializzare e sostituire una definizione errata sono atti
diversi; la correzione richiede una ritrattazione mirata, non l'accumulo di
dichiarazioni incompatibili.

**L'eccezione attuale è un default sulla conoscenza disponibile.**
`relation_unless(V,W,Z)` cerca W(X,Y) e poi `naf(Z(X,Y))` tramite `apply`.
Significa «W è sostenuta e Z non è derivabile», non «Z è dimostrata falsa».
Su una KB incompleta la distinzione cambia la risposta. Se la lezione richiede
certezza dell'assenza occorre una negazione esplicita o una copertura dichiarata
adeguata: questa superficie da sola non basta.

Nel solver la negazione richiede un goal ground; una ricerca esaurita per
budget non viene trattata come assenza. Anche la resa finale deve conservare
la distinzione. Una campagna non promuove un default come universale perché
nessuno ha ancora insegnato l'eccezione. La crescita può **togliere** una
conclusione correttamente: l'aggiunta dell'eccezione rende inapplicabile quel
ramo, pur lasciando eventuali supporti indipendenti.

#### G.7 Scegliere una lezione che moltiplica conoscenza reale

Prima di selezionare un ponte, cercare nella KB viva le due parti che
potrebbero beneficiarne. La domanda è: **quali fatti già appresi restano
separati da una distinzione insegnabile?** Il lessico suggerisce un candidato;
soltanto il significato e le fonti possono giustificarlo.

Preparare, oltre ai parametri del §2, una scheda didattica in prosa:

```text
Relazioni o classi coinvolte e significato nel dominio:
Che cosa permette il legame, e in quale verso:
Premesse, intermedio, scope ed eccezioni richiesti:
Fonte del legame e fonti dei fatti che lo useranno:
Domanda reale prima irraggiungibile:
Domanda vicina che deve restare esclusa o incerta:
Conoscenza preesistente da riusare:
Fatto vero da insegnare dopo il ponte:
Consumer da osservare e superficie naturale disponibile:
Come correggere o ritrattare la lezione, se supportato:
```

Questa è documentazione del teacher, non uno schema interno da far digitare
al discente. Chi insegna deve spiegare i ruoli con parole del dominio.

Tre obiettivi diversi meritano campagne diverse:

1. **Connettere:** far comunicare conoscenze reali già nella KB con un legame
   nuovo. Non si insegna direttamente la conclusione attesa.
2. **Generalizzare nel tempo:** insegnare il ponte, poi un nuovo fatto vero
   nella relazione sorgente e verificare la conseguenza senza ripetere il ponte.
3. **Comporre strumenti:** usare il risultato di una lezione in un'altra
   relazione, classe, procedura o decisione. Se il consumer non vede la vista,
   si registra un gap di integrazione, non un fallimento del fatto sorgente.

Un curriculum fertile alterna questi obiettivi e torna sulle stesse letture
con atti differenti. Molte relazioni senza consumer producono un catalogo;
molte repliche dello stesso caso non dimostrano trasferimento del metodo.

#### G.8 Il ciclo causale applicato ai ponti

I passi del §6 restano obbligatori. Cambia l'unità osservata: la lezione causa
un **insieme di conseguenze con condizioni**, non una sola risposta.

1. **Prima del ponte:** verificare i fatti sorgenti e registrare la risposta
   alla domanda derivata. Se è già corretta, cercarne il supporto: il ponte
   candidato potrebbe essere ridondante.
2. **Dopo la lezione:** interrogare la conseguenza e, dove disponibile, la
   scheda. L'eco della lezione non dimostra l'uso.
3. **Fatti successivi:** insegnare un fatto reale verificato che non fosse
   presente al momento del ponte; interrogare la conseguenza senza insegnarla.
4. **Contrasto:** cambiare verso, classe, contesto o presenza dell'intermedio.
   Non si pretende «non so» quando altre prove giustificano una risposta: si
   verifica che il ponte non sia usato fuori dalle sue condizioni.
5. **Composizione:** far usare la conseguenza a un'altra capacità. Annotare
   separatamente domanda polare, enumerazione, appartenenza, spiegazione e uso
   in un piano; non sono sostituti l'uno dell'altro.
6. **Ablazione:** ritrattare il legame con una superficie naturale supportata.
   I fatti sorgenti devono restare; le conseguenze sostenute **solo** da quel
   legame devono cessare. Con un'altra prova la risposta può restare corretta:
   il report deve spiegarlo, non contare un fallimento fittizio.
7. **Ripristino e retention:** reinsegnare la stessa verità e ripetere un uso
   dopo altri turni. Solo allora preparare checkpoint e processo nuovo.

La ritrattazione di `relation_like` è dichiarata nel catalogo. Non assumere
che «forget that …» ritratti qualunque definizione, contesto o firma: ogni
famiglia richiede il proprio riscontro. Se manca il canale, registrare
`Ablation = unavailable` e capacità `partial`, senza sostituirlo con `!assert`,
editing del dump o rimozione manuale nel training.

Per trasferire fra domini non basta sostituire i nomi: occorre riusare la
struttura su un secondo insieme di conoscenze vere, con vincoli appropriati.
Inventare i dati per costruire il percorso dimostra al più una meccanica di
sviluppo, non connecting dots della KB viva.

#### G.9 La correzione riguarda il punto che propaga l'errore

Un errore moltiplicato da un ponte costa quanto molte risposte sbagliate.
Prima di correggere, distinguere dove nasce:

| Punto dell'errore | Correzione pertinente | Che cosa non dimostra la riparazione |
|---|---|---|
| Fatto sorgente falso | correggere quel fatto e verificarne le dipendenze | non invalida automaticamente il significato del ponte |
| Ruoli scambiati nella lettura | correggere costruzione o binding generale | una risposta memorizzata lascia il difetto intatto |
| Ponte troppo ampio | restringerne il dominio, sostituirlo o ritrattarlo | aggiungere eccezioni per ogni vittima non identifica il limite generale |
| Proprietà ereditata impropria | correggere somiglianza o proprietà sorgente, secondo la causa | non serve cancellare i fatti veri delle due relazioni |
| Conseguenza non raggiunta | individuare quale consumer usa una vista diversa | re-insegnare il fatto già noto non integra le viste |
| Conseguenza vecchia ancora attiva | distinguere supporto indipendente, materializzazione e lettura non rivista | una risposta rimasta uguale non identifica da sola la causa |

Conservare frase didattica, fonte, condizioni e usi campionati.
`relation_note/2` orienta; non è una genealogia completa delle risposte.
«So che V è una catena» e «questa risposta dipende da questi due archi e da
questo intermedio» sono conoscenze differenti.

La cancellazione di una dichiarazione non prova la revisione di ogni
conseguenza copiata o materializzata. Supporti e invalidazione devono coprire
anche cache, tracce e riletture. Distinguere ciò che si è osservato da ciò che
la derivazione su richiesta farebbe in assenza di copie persistenti.

#### G.10 Contesti: conservare la condizione insieme alla conoscenza

Quattro nozioni non sono intercambiabili: contesto di validità del fatto,
significato locale di una parola, ipotesi attiva e provenienza di
un'affermazione. Una fonte può parlare di un mondo senza renderlo quello
attivo; attivare un'ipotesi non la promuove a verità del mondo reale.

`assume` asserisce un contesto attivo e `stop assuming` lo ritratta: queste
operazioni non mostrano uno stack di mondi, esclusività o precedenza. Più
contesti attivi e nomi locali omonimi richiedono una distinzione verificata.
Per gli alias di entità, la sostituzione del soggetto e quella dell'oggetto
sono rami separati sul predicato diretto; non si promette la risoluzione
completa di catene di nomi locali.

Prima del salvataggio distinguere la **regola condizionale da conservare**
dallo **stato di assunzione della conversazione**. La verifica fresh-process
deve controllare che il fatto sospeso resti tale fuori contesto e diventi
utilizzabile attivando il contesto pertinente, senza reinsegnare la regola.
Un contesto involontariamente persistito può simulare un ricordo corretto.
Non eliminarlo con un filtro indiscriminato: spiegare routing e stato secondo
[`session-and-provenance.md`](docs/session-and-provenance.md).

Per `context_alias` questo isolamento non è oggi implementato. Se la verità
della lezione dipende dall'isolamento, il gap impedisce di promuoverla nella
forma attuale. Il nome del contesto nel messaggio «Held» non è una garanzia.

#### G.11 Il guadagno si misura su due assi

I contatori `W/L/C/P/O` misurano ciò che viene salvato; non misurano da soli
ciò che diventa utilizzabile. Una dichiarazione `relation_chain` è un fatto
ground nel file e una **definizione eseguibile** nel significato: si classifica
in `C`, senza ricontarla come fatto del mondo `W`. Per gli altri legami decidere
fra `L` e `C` in base alla funzione e spiegarlo nel report.

**Un ponte salvato non equivale a tutti i suoi risultati salvati.** Le
conseguenze derivate vengono contate separatamente sul campione di domande;
non incrementano `W` soltanto perché ora sono interrogabili. Eventuali
materializzazioni si distinguono dalle nuove lezioni e non si ricontano a
ogni ricostruzione della vista.

Per campagne che includono ordine superiore riportare anche:

| Misura | Definizione operativa |
|---|---|
| `BridgeLessons` | lezioni di legame tentate/promosse, con famiglia e scope |
| `ExistingKnowledgeReuse` | domande prima irraggiungibili ora corrette usando premesse già nella KB / domande campionate |
| `FutureFactTransfer` | conseguenze corrette su fatti veri appresi dopo il ponte / casi campionati |
| `ScopeContrast` | contrasti rispettati per classe, verso, contesto ed eccezione / contrasti campionati, distinti per tipo |
| `ConsumerReach` | elenco dei consumer verificati con esito, non un generico «funziona» |
| `BridgeAblation` | usi esclusivamente dipendenti rimossi, fatti sorgenti conservati e supporti alternativi spiegati |
| `DerivedAnswerGain` | nuove risposte corrette nel campione tenuto da parte, senza insegnare quelle risposte |
| `BridgeRetention` | legame e condizioni ancora operanti nel processo nuovo |

Ogni rapporto espone numeratore e denominatore; per un canale assente scrivere
`unavailable`, non 100%. Le misure non sostituiscono i gate del §11. Se `W = 0`
ma cresce una capacità verificata, l'esito resta `meta-capability-only`: può
essere importante senza cambiare il significato di `trained` nel protocollo.

Il guadagno resta **misurato sul campione**. Non si contano prodotti cartesiani
come conoscenza acquisita e non si deduce crescita esponenziale dal numero di
relazioni dichiarate.

#### G.12 Massimizzare e declinare il circuito

Il mantra #22 si applica soprattutto qui. Dopo il primo ponte riuscito,
massimizzare **la distinzione**: fatti anteriori e posteriori alla lezione,
ruoli diversi, casi dentro e fuori scope, domande alternative, correzione,
retention e consumer differenti. Si amplia la KB con lezioni vere e utili,
non con combinazioni generate solo per saturare una tabella.

Poi cercare una declinazione che riusi la lettura: una relazione può definire
una classe; quella classe può vincolare un'altra relazione; un risultato può
alimentare un piano o un criterio. È una direzione di crescita, non la
promessa che ogni catena del genere funzioni già (§G.5).

La condizione d'arresto resta comportamentale: i nuovi casi non cambiano più
l'esito del transcript tenuto da parte, oppure il prossimo passo richiede un
meccanismo diverso. Nel secondo caso si registra il circuito successivo.
Non si moltiplicano adapter privati per simulare continuità.

Per la sostenibilità registrare latenza e ricerca incompleta quando crescono
ramificazione o annidamento. Prima di ridurre la conoscenza, verificare
riletture, enumerazioni ripetute e dipendenze delle viste materializzate
secondo il mantra #20. Una cache che ignora un nuovo ponte, una ritrattazione
o un cambio di contesto cambia il significato; non è un'accelerazione corretta.

#### G.13 Il gradino riflessivo: imparare strumenti per imparare

La conseguenza più ampia non è soltanto rispondere a più domande. Se anche
una forma, un criterio di copertura, un obbligo o un operatore di thinking
viene selezionato da relazioni insegnabili, una lezione può cambiare **quali
lezioni successive il sistema riesce a comprendere e quali lacune vede**.
I precedenti di §G.4 rendono concreta questa ipotesi; non la provano per
l'intero apprendimento.

La prova forte ha tre momenti: una seconda lezione prima non era utilizzabile;
si insegna un legame generale; la seconda lezione viene compresa e usata senza
che il teacher fornisca la rappresentazione interna o la risposta finale.
Poi serve il contrasto: una lezione vicina ma fuori dalle condizioni non deve
essere assorbita. Sarebbe un incremento locale del metodo, non comprensione
universale né scoperta autonoma dei legami.

Davanti a un muro, chiedersi quindi se manchi un fatto, il suo nome, il ponte
che lo rende pertinente, il consumer che lo attraversa o il criterio che ne
riconosce il bisogno. La risposta decide che cosa insegnare e che cosa
lasciare come gap generale.

#### G.14 Due estensioni del substrato, distinte dalle capacità dialogiche

Il predicato variabile apre uno spazio rappresentativo: la relazione è
scegliibile dalla KB. La ricerca di altre leve dello stesso livello è
sviluppata in [Due strutture abilitanti](docs/plans/due-strutture-kb-viva.md).
**Le due proposte seguenti non sono nuove superfici già operative del catalogo.**
Sono criteri per riconoscere quale struttura manca quando una classe di
insegnamenti non entra.

| Struttura proposta | Che cosa diventa dato | Che cosa dovrebbe poter insegnare il maestro |
|---|---|---|
| **Il fatto come nodo a ruoli aperti** | identità dell'istanza, ruoli dei partecipanti e mappe fra prospettive | un nuovo ruolo o una qualificazione senza cambiare la firma della relazione e il codice dei suoi consumer |
| **La definizione come espressione componibile** | struttura della relazione derivata, parametri e leggi sui costruttori | una combinazione annidata o una costruzione parametrica senza un ramo specifico per ogni combinazione |

La prima estende il principio dei frame oltre un insieme di slot consumati
in modo fisso: una domanda seleziona ruoli, mantiene l'identità dell'istanza
e verifica le condizioni richieste. I fatti parziali possono essere arricchiti
senza inventare valori per i ruoli ignoti. Proiettare due ruoli non cancella
gli altri e non autorizza a fondere due eventi diversi.

La seconda estende G dal nome della relazione alla forma della sua
definizione. Se ogni operando può essere un'altra espressione, le combinazioni
non richiedono una famiglia nuova per ciascun annidamento. Inoltre la KB può
inferire **sulla definizione**: per relazioni binarie pure, l'intersezione di
R con il suo inverso è simmetrica per costruzione. Non occorre dichiarare
questa proprietà per ogni nuova relazione costruita così.

Per il training futuro, distinguere tre risultati che oggi sarebbe facile
confondere:

1. **Salvataggio:** il nuovo ruolo o la definizione compaiono nel dump.
2. **Uso:** una domanda o un'altra regola li impiegano senza un adattamento
   specifico del consumer.
3. **Fertilità strutturale:** una lezione successiva combina quel ruolo o
   quella definizione in un modo non predisposto per il caso iniziale.

Soltanto il terzo risultato prova la leva cercata. Per i ruoli controllare
istanze distinte, valori ignoti e aggiunte senza perdita di quelli precedenti.
Per le definizioni controllare annidamento, scope delle variabili, proprietà
dedotte e costruzioni vicine alle quali la proprietà non si applica. In
entrambi i casi servono crescita via prompt, ablazione e KB reale completa.

Un nuovo contenitore generico dietro lettori rigidi non supera il criterio.
Un AST archiviato che nessuna regola può analizzare non lo supera. Né è
sufficiente insegnare al sistema a chiedere meglio o a riprendere un turno:
quelle sono possibili conseguenze, mentre qui si valuta **l'apertura della
struttura su cui ogni capacità opera**.

### F. Procedure eseguibili

*KB-first non è solo fatti: sono anche **procedure e processi** nella KB. Una
procedura complessa non è un operatore complesso — è una **catena di operatori
semplici**, ed è la catena che si insegna.*

| si dice | parrot0 ne ricava |
|---|---|
| `rule for X is <operatore>` | un passo della procedura X, in coda ai precedenti (gen507/43) |
| `apply X to <testo>` · `applica X a <testo>` | esegue la catena e rende il risultato |
| `what procedures do you know?` · `che procedure sai?` | **elenca** ciò che sa fare: senza, chi insegna non sa che cosa ha già dato (gen507/59, forma #83) |
| `step N of X is now <passo>` | **corregge** un passo invece di accodarne uno (gen507/66, forma #84) |
| `remove step N of X` | **toglie** un passo (gen507/66, forma #86) |
| `forget the procedure X` | **disfa** la procedura, tutta: una lezione che non si può ritrattare non è una lezione, è un vincolo (gen507/60, forma #87) |
| `why?` dopo un `apply` | la **traccia**: ogni passo applicato e il valore che ne è uscito, in `/debug` e a parole (gen507/58, forma #88) |

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
| `sort` · `unique` | ordina i pezzi / toglie i doppioni (gen507/56, forma #47) |
| `split on <c>` · `join with <c>` | da stringa a lista e ritorno: la lista **non è un tipo nuovo**, è la stessa stringa guardata a pezzi (gen507/57, forma #45). ⚠ il separatore dev'essere una lettera: la punteggiatura viene tolta prima che la lezione arrivi |
| `repeat <passo> until stable` | **il ciclo**: rifà il passo finché il valore smette di cambiare — il punto fisso, non un numero di giri deciso a caso (gen507/54, forma #41) |
| `repeat <passo> until <condizione>` | il ciclo con un arresto **dichiarato** (gen507/55, forma #41b) |
| `if <condizione> then <passo>` | **il ramo**: se la condizione non regge, il passo non si fa — e non è un fallimento, è la procedura che ha deciso (gen507/55, forma #42) |

**Condizioni disponibili** (valgono sia per `until` sia per `if`, perché sono la
stessa cosa): `empty` · `any` · `has <classe>` · `length N` · `shorter N` ·
`longer N` · `starts <testo>` · `is <testo>`.

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

⚠ **Ciclo, arresto dichiarato e ramo ci sono** (gen507/54-55). Alla radice
quadrata a mano manca ancora altro: gli operatori sono **su testo**, non su
numeri, e una procedura ha **un solo registro** — l'iterazione di Newton
(`x ← (x + n/x)/2`) ne vuole due. Sono due primitive del motore, non due forme.

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
| 1 | `V chains with opposite W` | **tutta la scheda di una relazione in una frase**: entrambe sono verbi di relazione, entrambe transitive, e l'una è l'inverso dell'altra — cinque fatti (gen507/79) |
| `X comes before Y` | l'**ordine nel tempo**: pone il fatto *e* dichiara che la catena si percorre e che il verso opposto si chiama `follows` — tre fatti in una frase (gen507/75, forma #17) |
| `V holds of itself` | riflessiva: `X V X` vale sempre | ✅ gen507/48 |
| 2 | `V has one value` | funzionale: un secondo valore è una **correzione o un conflitto**, non un fatto in più — estende a ogni relazione ciò che il giro /16 fa per gli attributi | ✅ gen507/45 |
| 3 | `V implies W` | se `X V Y` allora `X W Y`: sussunzione fra relazioni | ✅ gen507/46 |
| 4 | `V excludes W` — reso `V rules out W` | se `X V Y` allora **non** `X W Y`: un «no» guadagnato senza elencare | ✅ gen507/47 |
| 5 | `V goes from <classe> to <classe>` | vincolo di tipo: parrot0 può **rifiutare** un fatto assurdo invece di tenerlo | 🔴 |
| 6 | `V is measured in <unità>` | l'unità del valore, per la resa e per i confronti | 🔴 |

### H. Tassonomia e insiemi

| # | si dice | ne ricava | |
|---|---|---|---|
| 7 | `the X are A, B and C` | estensione **dichiarata completa** → da lì il «no» è chiuso, non «non l'ho derivato» | ✅ gen507/44 |
| 8 | `X and Y are the same thing` — reso `X and Y name the same thing` | identità fra entità | ✅ gen507/63 |
| 9 | `every X is either Y or Z` | partizione: se non è Y allora è Z | 🔴 |
| 10 | `X is short for Y` | una **sigla**: la forma corta che nei testi compare più del nome esteso, e senza il ponte ogni frase che la usa è un muro (gen507/74, forma #27) |
| `X is the opposite of Y` | antonimia fra concetti | ✅ gen507/71 |
| 11 | `most X are Y` | tipicità: vera in generale, **non** universale — e la risposta deve dirlo | 🔴 |
| 12 | `X is a Y except when Z` | l'eccezione dichiarata, invece di una regola falsa | 🔴 |

### I. Quantità, misure, conversioni

| # | si dice | ne ricava | |
|---|---|---|---|
| 13 | `X is N <unità> long` — reso `X has <dimensione> N` | misura su una scala | ✅ gen507/69 · resta l'unità di misura |
| 14 | `N <unità> is M <unità>` | conversione: apre i confronti fra misure dette in unità diverse | 🔴 |
| 15 | `X is N times bigger than Y` | rapporto, non solo ordine | 🔴 |
| 16 | `X is between A and B` | intervallo | 🔴 |

### L. Tempo

| # | si dice | ne ricava | |
|---|---|---|---|
| 17 | `X happened before Y` — reso `X comes before Y` | ordine temporale, con transitività e inverso dichiarati insieme | ✅ gen507/75 |
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
| 26 | `X means Y` — reso `X is defined as <testo>` | glossa: una definizione a parole | ✅ gen507/64 |
| 27 | `X is short for Y` | sigle e abbreviazioni | ✅ gen507/74 |
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
| 38 | `forget the plan for <situazione>` | ritrattare un piano | ⛔ dichiarata e mai raggiunta: tre superfici provate, tre lettori diversi le prendono prima (gen507/72). La regione attorno a «piano» è satura |
| 39 | `if you are not sure, <mossa>` | condotta sull'incertezza, che oggi è cablata nel declino | 🔴 |
| 40 | `X is uncertain` | marcare conoscenza dubbia: la risposta deve **dirlo**, non tacerlo | 🔴 |

### Q. Procedure — cicli, condizioni, composizione

*Il giro /43 ha aperto le catene di operatori. Senza queste, la radice quadrata a
mano resta inesprimibile.*

| # | si dice | ne ricava | |
|---|---|---|---|
| 41 | `rule for X is repeat <op> until stable` | il **ciclo**, con la condizione del punto fisso | ✅ gen507/54 · restano le condizioni diverse da `stable` |
| 42 | `rule for X is if <cond> then <op>` | la **condizione** | ✅ gen507/55 |
| 43 | `rule for X is apply Y` | chiamare una procedura da un'altra: la composizione vera | ✅ gen507/51 |
| 44 | `<classe> contains <caratteri>` — reso `the <nome> letters are …` | insegnare una classe di caratteri parlando | ✅ gen507/53 |
| 45 | `rule for X is split on <char>` · `join with <char>` | dalle stringhe alle **liste** | ✅ gen507/57 |
| 46 | `rule for X is replace <a> with <b>` | sostituzione | ✅ gen507/52 |
| 47 | `rule for X is sort` · `unique` | operatori su liste | ✅ gen507/56 |
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
| 63 | enumerare tutti i fatti | reso `who appears in V?` | ✅ gen507/65 |
| 64 | contare | `how many things does X V?` | 🔴 |
| 65 | correggere il secondo termine | `X V Z, not Y` | 🔴 |
| 66 | ritrattare un fatto | `forget that X V Y` — esiste per le classi, non per le relazioni | ✅ gen507/62 |
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
| 78 | valore di default | reso `<classe> are typically P` | ✅ gen507/73 |
| 79 | intervallo di validità | `X is red in <periodo>` | 🔴 |
| 80 | provenienza del valore | `<entità> is <valore> according to <fonte>` — **chi lo dice**: apre il disaccordo fra fonti | 🔴 |

### V. Procedura × atti

*La famiglia più giovane (giro /43) e quella con più celle vuote.*

| # | atto | si dice | |
|---|---|---|---|
| 81 | chiedere i passi | `how does X work?` — recitare la procedura invece di eseguirla | 🔴 |
| 82 | chiedere il risultato | `apply X to Y` ✅ (/43) | ✅ |
| 83 | enumerare le procedure | `what procedures do you know?` | ✅ gen507/59 |
| 84 | correggere un passo | reso `step N of X is now <passo>` | ✅ gen507/66 |
| 85 | inserire un passo | `before step 2 of X do <op>` | 🔴 |
| 86 | togliere un passo | `remove step N of X` | ✅ gen507/66 |
| 87 | ritrattare la procedura | `step N of X is now <passo>` | **corregge** un passo invece di accodarne uno (gen507/66, forma #84) |
| `remove step N of X` | **toglie** un passo (gen507/66, forma #86) |
| `forget the procedure X` | ✅ gen507/60 |
| 88 | spiegare l'esecuzione | `why did X give that?` — la **traccia**, passo per passo | ✅ gen507/58 |
| 89 | verificare | `test: apply X to <in> gives <out>` (= 49) | 🔴 |
| 90 | generalizzare | `X is like Y but <differenza>` — definire una procedura per differenza da un'altra | 🔴 |

### Z. Condotta × atti

| # | atto | si dice | |
|---|---|---|---|
| 91 | chiedere il piano | reso `your plan when <situazione>?` — parrot0 recita la propria condotta | ✅ gen507/61 |
| 92 | enumerare le situazioni | `what situations do you know?` | ✅ gen507/61 |
| 93 | correggere una mossa | reso `move N for <situazione> is now <mossa>` | ✅ gen507/72 |
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

### ⛔ La lezione che si lascia in KB dev'essere VERA e UTILE

*Errore commesso e corretto al gen507/67. Vale come regola, non come aneddoto.*

Quando si prova una forma nuova si usano entità inventate — `smith`, `zelnik`,
`grum` — perché una prova non deve dipendere da ciò che parrot0 sa già. **Ma
quelle non si salvano.** Un `/save` dopo una sessione di prova instrada nella KB
curata cose come:

```prolog
fact_source(owner(smith, zelnik), smith, "smith owner zelnik").
discourse_referent(12, smith).
```

che non sono conoscenza: sono i resti di un banco di prova. Tre danni distinti,
e il terzo è il peggiore:

1. **Dati falsi** — `owner(smith, zelnik)` non è vero di niente.
2. **Fatti mozzati** — `means(sonder, realisation)`: la definizione era stata
   tagliata dal divisore di clausole, e ciò che resta *non è una definizione
   sbagliata, è una parola*. Peggio di un'assenza, perché risponde.
3. **Conoscenza vera nel posto sbagliato** — le nove definizioni erano finite in
   `kb/experts/games/risk.p0`, perché il save-map cercava un file che avesse già
   un `means/2` e quello era il primo. Una definizione di *kintsugi* dentro il
   file del Risiko si trova solo per caso.

**Il posto sbagliato non è casuale: è sempre il primo che capita.** Quando un
predicato non ha una casa dichiarata nel `savemap.tsv`, `/save` lo instrada nel
primo file che quel predicato lo usa già. È così che le definizioni sono finite
nel file del Risiko e le profondità delle grotte in quello del poker — che ha
`magnitude/3` per i punti delle mani. **Prima di insegnare una classe nuova di
fatti, si dichiara la sua casa**, o si controlla il diff subito dopo.

**La regola.** La sessione di prova e la sessione di lezione sono due sessioni.
Si prova con entità inventate e **non si salva**; si insegna con conoscenza vera
e **si salva**. E dopo un `/save` si legge il diff: `git diff kb/` dice sempre
che cosa è entrato e dove — se una riga non si vorrebbe difendere fra un mese,
non è una lezione.

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

Per l'ordine superiore aggiungere il punto della catena di §G.4 in cui il
percorso si arresta: forma non riconosciuta, dichiarazione assente, premessa
irraggiungibile, ponte non componibile, consumer diretto, scope perso,
spiegazione incompleta o persistenza incoerente. Sono annotazioni diagnostiche:
non sostituiscono i tipi M0–M14 con una tassonomia concorrente.

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

Per una lezione di ordine superiore inventariare anche il legame, le
registrazioni accessorie, i supporti sorgenti e lo stato dei contesti. Non
salvare una regola più ampia della proposizione verificata; non confondere
l'ipotesi attiva nella chat con la condizione permanente del fatto (§G.10).

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

Per un ponte verificare nel processo nuovo almeno una conseguenza derivata e
un contrasto, non soltanto la presenza della dichiarazione nella scheda.
Per fatti contestuali osservare il comportamento fuori contesto, durante
l'assunzione pertinente e dopo la sua revoca. Riattivare il contesto previsto
non è reinsegnare il legame; trovarlo attivo senza volerlo è un possibile gap
di persistenza da spiegare.

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

Per una capacità di ordine superiore aggiungere il riscontro di §G.11:
riuso di conoscenza preesistente, transfer su fatti successivi, contrasti di
scope e raggiungibilità nei consumer dichiarati come obiettivo. Ogni canale
non verificato resta tale. Una scheda corretta non sostituisce l'uso; un uso
polare non certifica enumerazione, spiegazione o impiego in un piano.

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

Per lezioni sulle relazioni includere la scheda di §G.7, le metriche di §G.11,
i consumer raggiunti e i supporti alternativi incontrati nell'ablazione.
Separare nel report: dichiarazioni persistite, conseguenze dimostrate nel
campione e implicazioni ancora ipotetiche. Conservare la fonte del ponte oltre
alle fonti dei singoli fatti.

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

Se sono stati insegnati ponti, aggiungere sinteticamente: legami promossi,
nuove risposte derivate sul campione, consumer verificati e limiti di scope o
ritrattazione. Non sostituire con questi dati il conteggio ufficiale `W`.

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
- [ ] Per i ponti ho fontato anche il legame, dichiarato verso/scope e scelto conoscenze preesistenti da riusare.
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
- [ ] Per i ponti ho distinto uso su fatti preesistenti, fatti successivi e consumer differenti.

### Prima di salvare

- [ ] Ho ispezionato `/session`.
- [ ] Tutti i fatti candidati sono veri e fontati.
- [ ] Nessuna fixture o nonce fact è attiva.
- [ ] Eventuale ablation è stata ripristinata.
- [ ] Ho distinto dichiarazioni, conseguenze derivate e contesti attivi; gli eventuali supporti alternativi sono spiegati.
- [ ] `X = 0`.

### Dopo `/save`

- [ ] Ho registrato `S`.
- [ ] Ho letto tutto il diff della KB.
- [ ] Ho contato semanticamente `W/L/C/P/O/X`.
- [ ] Ho scritto il numero esplicito dei nuovi fatti veri salvati.
- [ ] Un processo nuovo raggiunge la conoscenza senza reinsegnamento.
- [ ] Per i ponti il processo nuovo conserva anche i limiti, non soltanto una risposta positiva.
- [ ] Ho creato il report permanente.
- [ ] Ho committato soltanto l'incremento del checkpoint causale.
- [ ] Ho pushato il commit prima di iniziare un incremento indipendente.

Se una casella critica resta vuota, lo stato non è `trained`.
