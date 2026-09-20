# The Rational Philosopher — conversare nello spazio logico dell'interlocutore

> **Richiesta di F., 20 settembre 2026.** «Sempre usando la KB, la IR e la
> concezione del mondo allargato espressa in
> [`lettura-della-prosa.md`](lettura-della-prosa.md), costruiamo un piano
> dedicato all'abilità di gestire conversazioni che spaziano nello **spazio
> logico dell'interlocutore** ma che rimangono **razionali**. L'esempio è
> troppo piccolo: concentrarsi sul **carattere della domanda intermedia** fatta
> al secondo turno, e sul concetto di spazio logico. Devono essere domande
> aperte, anche generiche, anche di carattere filosofico — tipo *cosa pensi
> della vita*.»

---

## 1. L'esempio, esteso e misurato

Riprodotto il 20 settembre 2026, profilo `agi`, `PARROT0_LANG=it`. I primi tre
turni sono quelli di F.; gli ultimi due li ho aggiunti per allargare il campione
al registro filosofico che il piano deve reggere.

| # | turno | risposta di oggi | specie del guasto |
|---|---|---|---|
| 1 | «sai parlare italiano» | «Sì, parlo italiano.» | ✅ nessuno |
| 2 | «quali parole conosci che possono sorprendermi» | «Non capisco ancora.» | **muro su una domanda che andava aperta** |
| 3 | «in quale lingua ti ho chiesto quale lingua sai parlare» | «c, python.» | **ancora persa + senso sbagliato, detto con sicurezza** |
| 4 | «cosa pensi della vita?» | «Non so ancora tradurre «pensi».» | **apertura filosofica letta come lacuna lessicale** |
| 5 | «secondo te il tempo esiste davvero?» | «Non ne ho di miei — sono parrot0, un'IA — ma raccontami dei tuoi.» | **posizione nello spazio logico letta come possesso di opinioni** |

**Non sono cinque bug: sono cinque modi di uscire dallo spazio.** Il turno 3 è
il peggiore di tutti sulla scala di F., perché non è un muro ma una risposta
confidente e sbagliata: «lingua» è stata collassata su *linguaggio di
programmazione* e il referente — *un turno di questa conversazione* — è sparito.
L'ispettore lo conferma: `turn_illocution` legge `question`, e
`debug_frame_record`, `turn_focus`, `max_qud` e `topic_read` sono tutti
**niente**. La domanda è riconosciuta come domanda e non è letta come nulla.

---

## 2. Che cos'è lo spazio logico, operativamente

Non è una metafora, ed è già quasi tutto rappresentabile con il mondo allargato
di [`lettura-della-prosa.md`](lettura-della-prosa.md) §0 — **contenuto, atto,
contesto, giudizio, derivazione**.

> **Lo spazio logico dell'interlocutore è l'insieme dei contenuti che possono
> essere coerentemente intrattenuti, dato ciò che lo scambio ha stabilito:**
> ciò che l'altro ha asserito, ciò che ha supposto, ciò che ha presupposto, e
> ciò che ne segue.

Una mossa sta **nello spazio** se il suo contenuto è collegato ad almeno un
**atto** dello scambio. È **razionale** se il suo **giudizio** porta con sé la
**derivazione** e gli assunti su cui poggia. Le due proprietà sono indipendenti,
e i quattro quadranti sono esattamente i modi di sbagliare:

| | nello spazio | fuori dallo spazio |
|---|---|---|
| **razionale** | ✅ la mossa da costruire | il muro corretto ma irrilevante — turno 2 |
| **non razionale** | fluente e insostenibile: il rischio LLM | «c, python» — turno 3, il caso peggiore |

**Perché questo piano non è «fare filosofia».** Una risposta bella su *cosa sia
la vita* non vale niente se non si sa da che cosa viene: sarebbe il frasario
della prosa, misurato al 19% in `lettura-della-prosa.md`, trapiantato nel
registro filosofico. Il criterio resta quello di F.: **da che cosa viene la
risposta**, non quanto suona bene.

---

## 3. La domanda intermedia, e perché è l'unità dell'abilità

F. indica il secondo turno. Ha ragione, e vale la pena dire esattamente perché.

> «quali parole conosci che possono sorprendermi»

Questa domanda ha tre proprietà, e **tutte e tre** la distinguono da una
domanda di conoscenza:

1. **È aperta:** non esiste *la* risposta giusta, esiste una risposta
   giustificabile.
2. **È indicizzata all'altro:** «sorprendente» non è una proprietà delle
   parole, è una relazione fra una parola e **chi ascolta**. La risposta
   dipende da un modello dell'interlocutore che nello scambio **non è stato
   stabilito**.
3. **È intermedia:** non chiude un argomento, lo **apre**. Il suo valore non è
   nella risposta ma nel movimento che provoca.

Ne segue il comportamento corretto, ed è **né il muro né l'elenco**:

> **Quando la risposta dipende da qualcosa che lo scambio non ha stabilito, la
> mossa razionale è nominare quella dipendenza** — e poi, a scelta dichiarata,
> chiedere («che cosa sai già?») oppure assumere esplicitamente («assumo che
> tu non conosca il lessico tecnico: allora…»).

Nominare la dipendenza **è** una mossa nello spazio logico: restringe lo spazio
invece di saltarne fuori. Un muro non lo restringe; un elenco confidente lo
abbandona. Questa è l'abilità che il piano deve costruire, e il resto sono
conseguenze.

---

## 4. Che cosa esiste già, e non va rifatto

Il mondo allargato è stato costruito il 20 settembre 2026 per la prosa. È la
stessa astrazione che serve qui, e **è eseguibile**:

| porta | che cosa dà a questo piano |
|---|---|
| `kb_clause/4`, `kb_clause_arg/4` | un contenuto è un dato: si può **menzionare senza crederlo** — la condizione minima per intrattenere una posizione |
| `kb_act/3` + `act_layer/2` | **chi** ha fatto entrare un contenuto e a quale titolo: detto, supposto, derivato. È l'ancora di ogni mossa |
| `kb_derivation/4` | la prova con le sue dipendenze (AND), le alternative (OR), e `absent(G)` — «lo dico perché non trovo il contrario» è una mossa filosofica legittima **se dichiarata** |
| `supported_from_premises/1` | ciò che segue **dalle sue premesse** e non dal mondo: la differenza fra discutere una tesi e ripetere un fatto |
| `holds_in/2`, `context-scope.p0` | posizioni che convivono senza cancellarsi |
| `epistemic-status.p0` | i cinque stati: positivo, negativo, entrambi, nessuno, ricerca incompleta |
| `turn_illocution`, `faculty_force/2`, `faculty_yield_when/3` | la condotta è conoscenza: chi ha diritto di parlare, su quale forza, e quando cede a una lettura |
| `discourse.p0` (`previous_turn`, `turn_counter`, `turn_retained`) | il filo della conversazione, già parzialmente tenuto |
| `scripts/prose-why.sh`, `/debug` | l'ispettore **nello stato in cui sbaglia** — vedi la regola: una sonda che mostra una differenza trova i difetti da sola |

**Quello che manca non è l'astrazione: è che la conversazione stessa non è
ancora un oggetto di quell'astrazione.** Un turno dell'interlocutore non è un
contenuto con un atto. Per questo il turno 3 non trova la sua ancora.

---

## 5. Le facoltà da costruire, in ordine

Una per volta, ciascuna con la sua condizione di riuscita **misurabile**.

### F1 — Il turno è un contenuto, e il dire è un atto

Ogni turno dello scambio diventa un contenuto con la sua identità e il suo
atto: *chi* l'ha detto, *quando*, *in quale lingua*, *con quale forza*. La
lingua del turno è già osservata (`turn_language_observed/2`); manca che il
turno sia **nominabile** come oggetto e che un riferimento anaforico lo
raggiunga («ti ho chiesto…», «quando hai detto…», «la domanda di prima»).

**Riuscita:** il turno 3 dell'esempio risponde **«in italiano»**. E la prova
che non è un caso: «in che lingua ti ho risposto?» e «che cosa ti ho chiesto
per primo?» rispondono dallo stesso circuito, senza una riga per ciascuna.

**Non barare:** nessuna cue «in quale lingua». La domanda deve leggersi come
riferimento a un atto dello scambio, e la risposta venire da quell'atto.

### F2 — La dipendenza non stabilita si nomina

Quando la risposta dipende da un contenuto che lo scambio non ha stabilito, la
facoltà produce una **mossa** invece di un muro: nomina la dipendenza, e poi
chiede o assume **dichiarandolo**.

**Riuscita:** il turno 2 produce qualcosa come *«dipende da che cosa conosci
già: dimmelo, oppure assumo che… e allora…»*. Con la stessa regola, e senza
scriverne un'altra, «qual è il libro più bello?» e «che musica mi consigli?»
producono la stessa forma di mossa.

**Non barare:** la dipendenza deve essere **calcolata** dalla derivazione
mancante (`kb_derivation` che non trova una prova ammissibile e sa dire di che
cosa avrebbe avuto bisogno), non scelta da una tabella domanda→dipendenza.

### F3 — Una posizione si intrattiene senza asserirla

«cosa pensi della vita?» apre un **contesto**, non chiede un fatto. La risposta
è una posizione che parrot0 **intrattiene**: `holds_in` con un atto della forza
giusta, e la derivazione disponibile.

**Riuscita:** tre domande di controllo funzionano sulla stessa posizione —
«perché lo dici?» restituisce la derivazione; «lo credi davvero?» distingue
l'intrattenuto dall'asserito; «e se invece…?» apre un secondo contesto senza
cancellare il primo.

**Non barare:** vietato l'aforisma preconfezionato per argomento. Il banco
conta il **modulo** che risponde, come `scripts/coefficiente.sh`: una posizione
che viene dal frasario vale zero anche se è vera e bella.

### F4 — Gli assunti cadono insieme a ciò che reggono

Ritirare un assunto fa cadere le posizioni che lo richiedono e **lascia vive**
quelle che hanno un'altra via. È già la meccanica di M2/M3 della prosa: qui
diventa condotta di conversazione.

**Riuscita:** l'ablazione. Stabilito un assunto, derivata una posizione,
ritirato l'assunto: la posizione cade e lo **dice**; una posizione con doppio
sostegno resta e sa dire con quale.

### F5 — Il registro delle mosse

Una conversazione razionale non è un'interrogazione. Le mosse — **domanda di
ritorno, distinzione, controesempio, concessione, riformulazione** — sono
conoscenza con le loro condizioni d'uso, non rami nel C. Una mossa nuova deve
costare **una riga di KB**.

**Riuscita:** si insegna una mossa nuova parlando, si vede usarla nel turno
successivo, si ritira e sparisce. È il criterio del mantra #2 applicato al
discorso.

---

## 6. Il banco, e come si misura

Stesso disegno della scala di prosa, perché ha funzionato: **una colonna che
dice da dove viene la mossa**, e un numero che si legge invece di stimarlo.

Per ogni turno si registrano tre cose:

| colonna | domanda |
|---|---|
| **ancora** | a quale atto dello scambio si attacca questa mossa? (vuoto = fuori dallo spazio) |
| **sostegno** | quale derivazione la regge? (vuoto = non razionale) |
| **mossa** | risposta, domanda di ritorno, assunto dichiarato, distinzione, muro |

e il punteggio è **la frazione di turni che hanno ancora e sostegno**. Un muro
onesto non è un errore: è una mossa con ancora e senza sostegno, e va contato
per quello che è. Una risposta fluente senza sostegno è il caso peggiore e va
contata come tale — **peggio del muro**, come sulla scala di F.

**Il corpus:** aperture filosofiche vere e generiche — *cosa pensi della vita*,
*il tempo esiste*, *che cos'è giusto*, *si può sapere qualcosa con certezza* —
più le metaconversazionali dell'esempio. Devono essere **aperte**: una domanda
con una risposta sola misurerebbe la conoscenza, non questa abilità.

**Anti-inganno, dal piano della prosa:** il banco è fisso, non si allarga per
abbassare il tasso; le entità non si inventano per far passare un turno; e se
una mossa giusta viene dal frasario, il referto lo dice nella colonna del
modulo e quel turno **non** conta come compreso.

---

## 7. Le trappole prevedibili, scritte prima di caderci

- **Il frasario filosofico.** Trenta aforismi indicizzati per argomento
  passerebbero il banco e non sarebbero l'abilità. È lo stesso guasto misurato
  al 19% sulla prosa, e si riconosce con la stessa colonna.
- **La domanda di ritorno come scappatoia.** Rispondere sempre con una domanda
  sembra socratico ed è un muro travestito. La domanda di ritorno è legittima
  **solo** quando nomina la dipendenza che la rende necessaria.
- **Il relativismo di comodo.** «Dipende dai punti di vista» è fuori dallo
  spazio quanto «c, python»: non si attacca a nessun atto.
- **Confondere intrattenere con credere.** Una posizione detta e non creduta
  deve restare distinguibile a ogni turno successivo, altrimenti la
  conversazione accumula fatti falsi — ed è precisamente il difetto che il
  mondo allargato è stato costruito per togliere.
- **Il turno come stringa.** Se il turno non diventa un contenuto con un atto,
  ogni riferimento all'eschange sarà una cue, e ce ne vorrà una nuova per ogni
  formulazione.

---

## 8. Da dove si comincia

**F1, e non un'altra.** È la più piccola, è misurabile in un turno solo, ed è la
condizione delle altre quattro: finché un turno non è un contenuto con un atto,
non c'è nulla a cui ancorare una mossa, e «nello spazio logico» resta una frase.

Il primo comando da eseguire è la riproduzione dell'esempio, perché il piano si
apre con una misura e non con un'intenzione:

```sh
printf '%s\n' 'sai parlare italiano' \
  'quali parole conosci che possono sorprendermi' \
  'in quale lingua ti ho chiesto quale lingua sai parlare' \
  'cosa pensi della vita?' \
  'secondo te il tempo esiste davvero?' '/quit' | \
  PARROT0_SESSION= PARROT0_LANG=it PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0
```

Le cinque righe della tabella §1 sono il punto di partenza. La prima che deve
cambiare è la terza, e deve diventare **«in italiano»**.
