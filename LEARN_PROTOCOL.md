# Protocollo operativo di addestramento di parrot0

Questo file è un comando operativo per un coding agent. L'invocazione prevista è:

> Leggi `LEARN_PROTOCOL.md` ed esegui il protocollo per addestrare parrot0 sul
> dominio `<DOMINIO>`, per un budget di `<TEMPO>`, usando fonti `<FONTI>`.

Il risultato atteso non è una demo, una patch cosmetica o una suite verde. È un
incremento verificato e persistente della conoscenza di parrot0, ottenuto
parlandogli in lingua naturale.

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
