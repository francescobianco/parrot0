# La IR e il predicato variabile — come si legge davvero una prosa

> **F., 12 settembre 2026, fissando il cancello nuovo:** «ogni iterazione dovrà
> rispondere con successo ad un numero di domande la cui il conteggio delle
> parole con il quale sono scritte le domande a cui ha successo deve essere più
> lungo del numero di parole del testo stesso, questo per le domande nel merito;
> man mano che cresce il testo crescono le domande a cui avendo successo parrot0
> risponde.»
>
> E, nello stesso giro, due volte: **«non stai lavorando usando la IR
> universale»**, **«ti ricordo sempre il KB-first, i mantra, la IR e la
> comprensione universale»**.

Questo documento esiste per due ragioni. La prima è **dire quanto è difficile
questa sfida**, con i numeri e non con gli aggettivi — perché il modo tipico di
fallirla è non accorgersi di quanto è dura e chiuderla barando. La seconda è
**proporre la forma d'arrivo**: la IR combinata con `apply/2`, cioè la tecnica
del predicato variabile, e le basi su cui la IR può evolvere senza che ogni
evoluzione costi un ramo nel motore.

Il piano operativo della lettura resta
[`lettura-della-prosa.md`](lettura-della-prosa.md); questo è il piano
**strutturale** che gli sta sotto.

---

## 1. Il cancello, e l'aritmetica che lo rende duro

Il cancello non conta le domande: conta **le parole delle domande nel merito a
cui parrot0 ha risposto con quello che il testo dice**, e vuole che quella somma
superi le parole del testo.

È una misura che non si può aggirare, ed è il motivo per cui è quella giusta:

- **non premia un banco corto** — con dieci domande non ci si arriva mai;
- **non premia la domanda facile ripetuta** — ripeterla costa le sue parole ma
  ne porta altrettante, quindi il rapporto non migliora;
- **cresce per forza col testo** — 500 parole esigono *più comprensione*, non la
  stessa percentuale.

### L'aritmetica, misurata il 12 settembre sul piolo 300

| | |
|---|---|
| parole del testo (Coral reef) | **299** |
| domande nel merito scritte | 50 |
| parole totali di quelle domande | 310 |
| domande risolte | **10** |
| **parole delle domande risolte** | **52** |
| **manca al cancello** | **248** |

Con 50 domande da 310 parole il cancello chiede il **97% di successo**. E questo
**non è un difetto della taratura: è la taratura**.

> **F., correggendomi il 12 settembre:** «per me parrot0 deve sapere rispondere
> a **ogni** domanda rispondibile sulla prosa, la percentuale di successo al
> tendere deve essere **100%**. La stima non implica crescere il set di domande
> ma **migliorare la comprensione** — per questo parliamo di comprensione
> universale: fissato il banco e addestrato per le lacune, questo espande la
> comprensione.»

La prima stesura di questo documento diceva che servivano «banchi molto più
larghi», ~120 domande, perché un tasso del 40% bastasse. **Era sbagliato, ed era
sbagliato nel modo peggiore**: allargare il banco abbassa il tasso richiesto
senza aggiungere un grammo di comprensione — è la stessa mossa che due paragrafi
più in basso ho chiamato «barare», applicata senza accorgermene. Resta scritta
qui perché è il riflesso da riconoscere.

**Il banco è FISSO.** Le 50 domande coprono ciò che il testo dice; sono la
domanda giusta. L'unica strada per il cancello è **10/50 → 50/50**:

| | |
|---|---|
| oggi | 10 risolte su 50 · **52 parole** |
| cancello | > 299 parole |
| a tendere | 50 su 50 · 310 parole |

Il cancello è tarato *apposta* perché lo si superi solo quando il tasso è quasi
totale. Ed è la definizione operativa di **comprensione universale**: non «una
buona percentuale», ma *ogni domanda a cui il testo risponde riceve la risposta
del testo*. Le lacune che restano non sono rumore statistico da diluire — sono
l'elenco di ciò che parrot0 non sa ancora leggere, e si chiudono una per una.

> ⛔ **Il modo di barare, scritto qui perché resti riconoscibile.** Si passa il
> cancello aggiungendo domande brevi e facili sulle due o tre frasi che già si
> leggono, oppure scrivendo in C un ramo per ognuna delle forme che il banco
> chiede. Entrambe danno un numero verde e **zero comprensione in più**. La
> misura ha senso solo se le domande coprono *tutto* il testo e la strada che le
> risolve è generale.

---

## 2. Perché oggi non ci si arriva: la diagnosi, non l'opinione

Il referto delle 50 domande dice una cosa più profonda del 10/50. Venti
fallimenti su quaranta hanno la stessa forma:

```
what holds coral polyps together?   ->  Hmm, I don't know about polyps yet.
where do coral reefs flourish?      ->  Hmm, I don't know about flourish yet.
at what depths are reefs found?     ->  Hmm, I don't know about depths yet.
```

Non è la domanda che non si sa fare: **la frase che contiene quel fatto non ha
lasciato niente in KB**.

E il passo 1 del banco — ogni frase da sola, sessione pulita, «che cosa ne hai
capito?» — dice esattamente quali e perché. **Delle 16 frasi, 8 lasciano almeno
un fatto e 7 non lasciano niente. La sedicesima risponde «975.».**

**Le sette che non lasciano niente, nel passo 1:**

| # | frase | che cosa la ferma |
|---|---|---|
| 3 | «**Most** coral reefs are built from stony corals, whose polyps…» | quantificatore non universale |
| 6 | «**Most** reefs grow best in warm, shallow, clear… water» | quantificatore non universale |
| 9 | «They occupy **less than 0.1% of** the world's ocean area…» | oggetto quantificato |
| 12 | «They are most commonly found at shallow depths…» | verbo + avverbio interposto |
| 14 | «They are under threat from **excess nutrients (nitrogen and…)**» | oggetto con parentetica |
| 11 | «Coral reefs **flourish** in ocean waters that provide few nutrients» | verbo non in KB |
| 16 | «**The annual global economic value of** coral reefs has been estimated at…» | soggetto con sintagma dentro |

> ⚠ **Una diagnosi che mi ero sbagliato, e la correzione.** La prima stesura
> attribuiva tre di queste al **soggetto anaforico** («*They* occupy…»). È
> falso, e la prova è differenziale:
>
> ```
> > Coral reefs are underwater ecosystems. They occupy the ocean area.
>   Learned: coral reefs occupy ocean area.      ← il pronome ARRIVA
> > Coral reefs occupy less than 0.1% of the ocean area.
>   Learned: coral reefs occupy 0.               ← l'oggetto NO
> ```
>
> La coreferenza fra frasi **funziona**. Quelle frasi falliscono nel passo 1
> perché lì ogni frase è letta **da sola**, senza antecedente: è un artefatto
> del passo, non un difetto del lettore. Il blocco vero è l'**oggetto
> quantificato** — «less than 0.1% of X», «at least 25% of Y», «by 50% since
> 1950» — che compare in **quattro** frasi ed è il singolo blocco più grosso
> del piolo. Lascio scritto l'errore perché è la specie che il passo 1 induce:
> misurare una frase fuori dal suo testo e attribuire al lettore ciò che manca
> al contesto.

**E la sedicesima è peggio di un muro:**

```
[13] Shallow tropical coral reefs have declined by 50% since 1950, partly
     because they are sensitive to water conditions.
  →  975.
```

Ha preso «50» e «1950» e ha fatto un conto. Nessun fatto sul corallo, e una
risposta **confidente e sbagliata** — la classe peggiore (mantra #7), perché non
si conta come lacuna e non si vede finché non si legge la risposta verbatim. È
il tipo di reperto per cui il passo 1 esiste: il passo 2 l'avrebbe segnato come
un semplice `·`.

**Ma il serbatoio più grande non è nemmeno questo.** Le otto frasi che *leggono*
enunciano ciascuna due, tre o quattro relazioni, e ne lasciano **una**:

| frase | che cosa lascia | che cosa perde |
|---|---|---|
| 2 | `reefs form colonies` | «held together **by calcium carbonate**» — e «formed **of**» è letto come «form» |
| 4 | `coral belong class anthozoa` | «in the animal **phylum Cnidaria**», «**which includes** sea anemones and jellyfish» |
| 5 | `corals secrete hard carbonate exoskeletons` | «that **support and protect** the coral» |
| 7 | `coral reefs first appear 485 million years ago` | «at the dawn of the **Early Ordovician**», «**displacing** the…» |
| 8 | `shallow coral reefs form some` | «**Sometimes called rainforests of the sea**» — e la coda è spazzatura |
| 10 | `specie(fish), specie(mollusks), …` | «**provide a home for at least 25%** of all marine species» |

Sommando: **~7 frasi mute, ~1 frase che mente, e ~8 frasi lette per un terzo.**
Il cancello chiede di risalire da qui, e dice quanto: 52 parole su 299.

### La radice: la IR del testo ha un livello solo

Oggi la IR di un testo letto (`last_text`) ha questa forma:

```prolog
input_node(last_text, Id, node(clause, clause, root), range(S, L))   % una per FRASE
input_node(last_text, Id, node(word,   word,  root), range(S, L))    % una per PAROLA
input_node_surface(last_text, Id, "…")
```

Frasi e parole. **Fra le due non c'è niente** — e tutto quello che il banco non
raggiunge sta lì in mezzo: la proposizione dentro la proposizione, il sintagma
che fa da soggetto, l'apposizione, il participio, il pronome che punta indietro.

Il lettore, non trovando struttura, prova a far combaciare **schemi di
superficie** (`extract_frame/2`) sull'intera frase. Su «Coral reefs deliver
ecosystem services» funziona. Su una frase vera di enciclopedia — che ne contiene
tre — combacia per sbaglio o non combacia, e in entrambi i casi il testo si
perde.

> Questa è la stessa diagnosi che il piano della lettura registra come
> **«strade rotte, non conoscenza mancante»**: insegnare un fatto non la cura,
> perché l'insegnamento percorre la stessa strada rotta.

---

## 3. La forma d'arrivo: la IR è la frase, e il lettore è un programma KB

La tesi è in una riga:

> **Oggi il lettore guarda la superficie e indovina la struttura. Deve guardare
> la struttura e leggere la superficie solo dove la struttura gliela indica.**

Il che significa due lavori distinti, e vanno tenuti distinti:

### 3a. La IR cresce di livelli (il *che cosa*)

```
testo
 └── frase          node(sentence, sentence, root)
      └── clausola  node(clause, <ruolo>, <frase>)      ← IL LIVELLO CHE MANCA
           └── sintagma  node(phrase, <ruolo>, <clausola>)
                └── parola / token
```

Con, accanto ai nodi, gli **archi** che oggi non esistono affatto:

```prolog
input_edge(Scope, coref,        Pronome, Antecedente)
input_edge(Scope, apposition,   Nodo,    Testa)
input_edge(Scope, coordination, Nodo,    Nodo)
input_edge(Scope, modifies,     Modificatore, Testa)
```

Un arco è un fatto come un nodo: si pubblica, si interroga, e chi lo consuma non
sa come è stato trovato.

### 3b. Il lettore diventa una tabella (il *come*) — ed è qui che entra `apply/2`

Oggi ogni forma della lingua che parrot0 sa leggere è, da qualche parte, **un
consumatore scritto apposta**: una regola KB che nomina il suo predicato, o un
ramo in C. Aggiungere una forma costa un consumatore.

`apply/2` — la tecnica del predicato variabile — serve esattamente a togliere
quel costo, e in parrot0 **ha già funzionato una volta**, nel ponte dei modelli
(`kb/core/model-bridge.p0`):

```prolog
model_of($P, $Thing, $Out, $Expr) :-
    model_carrier($P),
    apply($P, cons($Thing, cons($Out, cons($Expr, nil)))).
```

Il ponte **non conosce il nome** di nessun predicato di dominio. Chiede «chi
porta modelli?», e chiama. Per questo `kb/experts/geometry/formulas.p0` ha
aggiunto la geometria al generatore di codice **con una riga**
(`model_carrier(shape_formula)`) e **zero righe nel ponte**.

La stessa mossa, applicata alla lettura:

```prolog
% Una LETTURA è: guarda un nodo della IR, e se il nodo ha questa forma,
% chiama questo predicato con questi pezzi. Il lettore non sa che cosa fa il
% predicato che chiama — sa solo dove prendere i pezzi.
ir_reading(Nome, Ruolo, Predicato, Pezzi).

ir_read($Scope, $Node) :-
    ir_reading($Name, $Role, $Pred, $Parts),
    input_node_role($Scope, $Node, $Role),
    ir_bind($Scope, $Node, $Parts, $Args),
    apply($Pred, $Args).
```

Quello che cambia non è l'eleganza: è **chi paga una forma nuova**.

| | oggi | con `apply` sulla IR |
|---|---|---|
| una relazione nuova | un `extract_frame` + un `answer_frame` | un fatto `ir_reading` |
| una forma sintattica nuova | una regola KB che la nomina, o un ramo C | un fatto `ir_reading` |
| un dominio nuovo che vuole leggere | deve farsi conoscere dal lettore | dichiara il suo predicato e basta |
| il lettore | cresce | **non cambia mai** |

E il test operativo del progetto — *«parrot0 può impararne un nuovo membro
domani, senza ricompilare?»* — diventa vero anche per **le forme della lingua**,
non solo per il vocabolario.

### 3c. Perché questo scioglie il costo, e non solo la forma

C'è un secondo motivo, misurato oggi e scritto in `C_TODO.md`. Gli schemi di
`extract_frame` **si scorrono tutti a ogni turno**: con 272 verbi di relazione in
KB sono migliaia di schemi derivati dal solver, e il turno di `soft-test` era
passato da 0,95 s a 1,50 s. La cura fatta (chiavare la cache su ciò che *genera*
gli schemi) toglie il costo **ripetuto**; non toglie quello **unitario**.

Una lettura guidata dalla IR non ha quel problema per costruzione: **non si
provano tutte le forme su tutta la frase**, si guarda che cosa la frase *è* e si
chiama la lettura di quel ruolo. Il costo passa da «schemi × turni» a «nodi».

---

## 4. Le basi per l'evoluzione della IR

Perché la IR possa crescere senza rompere chi la consuma, servono invarianti
dichiarati **prima** di aggiungere livelli. Sono queste cinque.

### I1 — Un id è dello scope, non della chiamata
Già riparato (gen513): ogni pubblicazione prende un `id_base`. Prima ogni frase
ripartiva da `0` nello stesso scope, e **ogni join per id era un prodotto
cartesiano fra frasi diverse**. È l'invariante zero: senza, niente altro regge.

### I2 — Un livello nuovo è un livello, non un predicato nuovo
Le parole sono `node(word, word, root)`, non `text_surface_token/4`. Il primo
tentativo aveva sbagliato proprio qui, e F. l'ha chiamato per nome. Chi legge i
nodi di clausola legge con le stesse regole anche i nodi di parola; chi domani
vorrà i sintagmi non dovrà inventarsi un terzo posto.

### I3 — Ogni livello dichiara il proprio tetto, e il tetto si vede
`input_structure` si ferma a **64 parole e 128 nodi per frase**, e oltre quel
punto tronca **in silenzio**. `MAX_CLAUSES = 8` faceva sparire due terzi di un
testo da 500 parole senza un muro e senza una traccia. Un tetto che non si
dichiara è una bugia lenta: ogni livello deve dire dove si ferma, e il banco
deve poterlo chiedere.

### I4 — Chi pubblica non interpreta, chi interpreta non pubblica
Il C trova i confini e assegna identità e ordine; **che cosa significhi un nodo
lo dice la KB**. È la ragione per cui `word_separator/1` è un fatto e non un
`isspace()` cablato, e per cui `turn_is_text/1` decide che cosa merita di
restare. Ogni volta che questa riga è stata attraversata è nato un difetto:
`text_topic` calcolato in C, il flusso di parole pubblicato come predicato
proprio.

### I5 — Un arco si giustifica
Un arco di coreferenza è una **ipotesi**, non un'osservazione: «*They* = coral
reefs» può essere sbagliato. Deve portare con sé da dove viene
(`input_edge_because/3`), come `model_provenance/2` fa per i modelli — perché la
cura del mantra #7 non è nascondere il dubbio, è renderlo **ispezionabile**. E
perché un fatto falso entrato in silenzio è la cosa peggiore che questo progetto
possa fare.

---

## 5. L'ordine dei lavori, per resa misurata

Non per eleganza: per quante parole di domande sbloccano sul piolo 300.

| # | lavoro | forma | frasi del piolo 300 |
|---|---|---|---|
| **1** | **oggetto quantificato** — «less than 0.1% **of** X», «at least 25% **of** Y», «by 50% **since** 1950». Oggi lo slot si ferma al primo confine e lascia `occupy(reefs, "less than 0")` | un ruolo nella IR, non uno schema in più | **4** |
| **2** | **livello clausola** — la frase si divide ai confini già dichiarati in KB, e il lettore legge le foglie: è ciò che restituisce la **seconda e terza relazione** delle otto frasi lette per un terzo | nodo `node(clause, …, <frase>)` | **8 (parziali)** |
| **3** | **relazione attenuata** — «*Most* reefs…» oggi è un rifiuto *voluto* (mantra #7: né universale né di un individuo). Serve saperla tenere **come attenuata**, non tenerla come universale | fatto + qualificatore | **2** |
| **4** | **participio in testa e con agente** — «Sometimes called X, …», «held together **by** Y» | `ir_reading` sul ruolo | **2** |
| **5** | **`ir_reading` + `apply`** — le letture qui sopra smettono di essere quattro regole e diventano **quattro fatti** | la tabella di §3b | zero nuove, ma tutte le prossime |

> Il #5 va **per ultimo e non per primo**, ed è una scelta, non una pigrizia: si
> astrae su tre casi che funzionano, non su zero. Un `ir_reading` scritto prima
> di avere le letture sarebbe una cornice vuota — e le cornici vuote in questo
> progetto sono costate più delle righe cablate.

---

## 6. Come si sa se sta funzionando

Due numeri, e nessuno dei due è una percentuale di test:

1. **Il cancello di F.**: parole delle domande nel merito risolte / parole del
   testo. Oggi **52 / 299**. Lo stampa `scripts/prose-probe.sh`.
2. **Le frasi che lasciano un fatto**, dal passo 1 del banco (una sessione per
   frase, «che cosa ne ha capito?»). Oggi **~5 su 16**. È il numero che il
   cancello traduce in parole, ed è quello su cui si lavora davvero.

Se il primo sale senza il secondo, **sto barando** — e questo documento esiste
anche perché quella frase resti scritta prima, non dopo.
