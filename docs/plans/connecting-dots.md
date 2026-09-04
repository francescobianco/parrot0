# Connecting the dots — *split and cross*, o come si moltiplicano gli archi

> **gen502, su indicazione di F.** Un metodo per costruire la conoscenza che
> risponde alla **connessione fra le isole**, invece di aggiungere isole. Non una
> matrice forgiata a monte: un procedimento **progressivo**, che a ogni
> iterazione raddoppia la granularità e produce nuovi archi — e che porta con sé
> **un indice**, così l'espansione è certificata invece che dichiarata.
>
> Deve diventare possibile incrociare **scacchi e informatica**, **matematica e
> chimica**, **i pronomi personali inglesi e gli animali**. Tutto con tutto — ma
> ⚠ **non incrociando temi a caso**: un cross è sempre fra due regioni di una
> partizione **completa** (§3.0-bis), e i temi diventano regioni solo in fondo
> all'albero. Prima di allora sono **domande testimone**, non risultati.
>
> ⭐ **E la domanda per cui il metodo esiste (§5-bis):** per ogni cross che
> garantiamo funzionante, **quanti se ne autodeterminano?** Se il rapporto è
> stabilmente sopra uno, la conoscenza **compone** e l'espansione ha un
> interesse; se è zero, ogni arco costa quanto vale e va detto.

---

## 1. L'origine — un caso vero, di oggi

Il 2026-09-04 il banco di gara diceva che parrot0 non sa fare un compito di
coding: 5/100 su difficoltà 1, 5/100 su difficoltà 4, cartella intatta. Ho
passato una giornata a inseguire chi rubava il turno — dispatch, finestre di
buffer, meccanica di strumenti. Ogni difetto trovato era reale, e **nessuno era
la ragione.**

Poi, cercando altro:

```text
> make a plan to fix the repository
  My derived plan for code task: 1) inspect the workspace and collect grounded
  repository evidence [cost 1] [needs …]
```

In `kb/experts/codebase/actions.p0` **c'era già la spina dorsale di un coding
agent** — `inspect_workspace → localize_change → edit_candidate →
verify_candidate`, con rischio, costo e ordine derivato. Era lì da prima che
cominciassi.

E dall'altra parte, misurato lo stesso giorno: parrot0 sa **elencare** (`run
ls`), **leggere in struttura** (`read main.c`), **rispondere con le prove**
(span, digest, provenienza) ed **eseguire il build leggendone il verdetto**
(`run make` → *«No rule to make target 'strjoin.c'»*).

⛔ **Le due isole non si toccavano.** Il piano non sapeva che gli strumenti
esistevano (`action_impl` = 0 su tutte e quattro le azioni); gli strumenti non
sapevano che esisteva un piano. Nessuna delle due parti era rotta, mancava
**l'arco** — e un arco mancante non somiglia a un guasto: somiglia a due cose che
funzionano.

> **La conoscenza c'era. Il coding non se n'era accorto.**

Questo documento generalizza quel caso in un metodo.

---

## 2. La tesi

Una KB cresce per **isole** — e «isola» qui non vuol dire *argomento*: vuol dire
un insieme di clausole di qualunque **specie** (§2-bis). Un esperto di algoritmi,
uno di chimica, la morfologia, il discorso, il dominio dei piani, l'IR del
codice, ma anche *l'ordine con cui le facoltà parlano* e *le preferenze con cui
si sceglie una risposta*. Ogni isola,
presa da sola, è verificabile e cresce bene. **Il valore però non sta nelle
isole: sta negli archi fra loro** — perché un'inferenza interessante quasi
sempre attraversa un confine.

`THINKING_TODO` H11 lo dice già come **Leva 1** — *«ponti prima di nuove
isole»* — e misura la cosa giusta (`cold tasks unlocked / new curated clauses`
deve crescere più di uno-a-uno). Ma **non dà un metodo**: dice che i ponti
valgono di più, non *quali* costruire, in che ordine, né come sapere quanto
manca.

⭐ **`split and cross` è quel metodo**, e la sua proprietà principale è che
produce **un numero**: a ogni iterazione si sa quanti archi sono possibili,
quanti sono certificati, e quindi quanto della propria conoscenza parrot0 sta
davvero *usando insieme*.

---

---

## 2-bis. ⛔ CHE COSA È UN'«ISOLA» — la KB è molto più che fatti

> F.: *«attenzione che quando parliamo di KB parliamo della sua concezione super
> estesa, in cui come conoscenza ci sono processi, procedure, regole,
> ordinamenti, preferenze di comportamento — non solo fatti.»*

**Questa precisazione cambia il metodo, non il suo lessico.** Se le isole fossero
argomenti (scacchi, chimica, grammatica), la partizione taglierebbe per *tema* e
non separerebbe mai una procedura da un fatto — e il cross più prezioso di questo
documento, quello del §1, è esattamente un cross **fra specie diverse**: un
piano da una parte, delle capacità dall'altra, e `action_impl/2` in mezzo.

### 2-bis.1 Le specie che ci sono davvero — contate

Su `kb/core` + `kb/experts`, **64 685 clausole**:

| specie | forma tipica | quante |
|---|---:|---:|
| **regole** (inferenza) | `testa :- corpo` | **2 822** |
| **preferenze di condotta** | `response_template`, `register`, `thinking_outcome_policy`, `appropriate_move` | **1 305** |
| **macchina dichiarata** | `machinery`, `turn_scratch`, `bookkeeper`, `debug_probe`, `learnable` | **1 090** |
| **archi fra rappresentazioni** | `representation_bridge`, `answer_frame`, `ir_denotation` | **282** |
| **processi e piani** | `plan_goal`, `action_yields`, `action_needs`, `action_impl` | **175** |
| **ordinamenti e precedenze** | `faculty_yield`, `module_claim_right`, `action_cost`, `severity` | **46** |
| **fatti** | tutto il resto | il grosso |

⭐ **Si legga la colonna dal basso.** Le specie più *scarse* sono quelle che
governano il comportamento: **46 ordinamenti** per settantotto facoltà, **175
clausole di processo** per l'intero dominio dei piani. E il difetto misurato in
questa sessione — `action_impl` = 0 sulle quattro azioni di `code_task` — sta
proprio in quella colonna sottile. **La scarsità non è casuale: la conoscenza di
condotta è la meno popolata perché è la meno somigliante a un fatto**, e quindi
la meno naturale da scrivere.

### 2-bis.2 Una regione è un insieme di CLAUSOLE, non un argomento

Ne segue la definizione operativa, e va tenuta larga di proposito:

> **Una regione è un insieme di clausole**, identificato da un criterio
> dichiarato. Il criterio può essere un **tema** (chimica), una **specie**
> (le procedure), un **livello** (la condotta di dispatch) o una **loro
> combinazione**. Non deve essere una tassonomia del mondo: deve essere una
> partizione completa e disgiunta di ciò che parrot0 sa.

### 2-bis.3 I due assi, e i tre tipi di cross

Da qui i cross non sono tutti la stessa cosa, e distinguerli è ciò che rende κ
interpretabile:

```text
                    ┌── stessa SPECIE ──┬── specie DIVERSA ──┐
   stesso TEMA      │   (raro, quasi    │  ⭐ CROSS DI SPECIE │
                    │    sempre già     │   il piano e i suoi │
                    │    dentro l'isola)│   strumenti (§1)    │
   tema DIVERSO     │  CROSS DI TEMA    │  CROSS PIENO        │
                    │  scacchi ✕ info   │  una procedura di   │
                    │  pronomi ✕ animali│  un dominio applica-│
                    │                   │  ta a un altro tema │
                    └───────────────────┴─────────────────────┘
```

| tipo | esempio | perché conta |
|---|---|---|
| **cross di tema** | scacchi ✕ informatica | è quello che si immagina per primo, ed è il più visibile |
| ⭐ **cross di specie** | `code_task` ✕ gli strumenti | **è quello che è mancato oggi**, ed è invisibile dal basso: due cose che funzionano e non si parlano |
| **cross pieno** | «bilancia questa reazione» — una *procedura* algebrica su un *tema* chimico | è il più raro e il più potente: trasporta un modo di fare, non un contenuto |

### 2-bis.4 ⭐ La predizione che ne segue — H-κ4

Aggiungo alle ipotesi del §5-bis.4 quella che questa sezione genera, ed è la più
forte perché rischia di più:

> **H-κ4 — un cross di SPECIE produce più spontanei di un cross di TEMA.**
>
> La ragione: un tema si applica al suo tema; **una procedura si applica a molti
> temi**. Legare `edit_candidate` a uno strumento di scrittura non apre solo
> `code_task`: apre *ogni piano futuro che debba produrre un artefatto*. Un
> cross di tema apre una coppia; un cross di specie apre una colonna.
>
> **Falsificabile:** κ misurato separatamente per i tre tipi. Se i cross di
> tema producono altrettanti spontanei, H-κ4 cade — e con essa l'idea che
> convenga cominciare dalla condotta invece che dai domini.

Se H-κ4 regge, l'ordine di costruzione cambia: **prima le specie sottili**
(ordinamenti, processi, condotta), che sono anche le meno popolate — cioè si
lavora dove c'è meno, non dove è più facile.

---

## 3. Il metodo

### 3.0 Passo zero — la conoscenza è una

Si parte da un insieme unico: tutto ciò che parrot0 sa. Nessun arco, nessun
indice. È lo stato di riferimento.

### 3.0-bis ⛔ CHE COSA RENDE UNA DIVISIONE VALIDA — e perché «X vs non-X» è barare

> F.: *«il metodo della divisione della KB è procedurale e logico: la prima
> divisione divide la KB in due parti la cui somma logica torna a essere la KB.
> Chiaramente non valgono le divisioni banali tipo SCACCHI vs NON-SCACCHI, sono
> barare. Divisioni valide sono ad esempio REGOLE vs FATTI, oppure COMPRENSIONE
> e CONOSCENZA DATA. E poi da lì sotto-divisioni coerenti la cui somma deve
> sempre dare il tutto.»*

Questa è la parte procedurale del metodo, e senza di essa tutto il resto è
aneddotica. **Non si incrociano due temi a caso: si incrociano due regioni di
una partizione completa.**

#### Le quattro condizioni di una divisione

| | condizione | |
|---|---|---|
| **D1** | **completezza** | `A ∪ B` = esattamente la regione divisa. Niente resta fuori. |
| **D2** | **disgiunzione** | `A ∩ B = ∅`. Nessuna clausola in entrambe. |
| **D3** | ⭐ **intensione positiva bilaterale** | **entrambi** i lati hanno una definizione propria. Nessuno dei due è «il resto». |
| **D4** | **decidibilità locale** | data una clausola **da sola**, il criterio dice da che parte va — senza guardare l'altro lato, e anche per una clausola che non esisteva quando la divisione è stata fatta. |

⭐ **D4 è il test che smaschera l'inganno**, ed è meccanico. *«Questa clausola è
una regola o un fatto?»* si decide guardando la clausola. *«Questa clausola è
scacchi o non-scacchi?»* si decide guardando la clausola **solo per il lato
"scacchi"**: l'altro lato non ha nessuna proprietà, si abita per esclusione. Una
partizione in cui un lato è definito dalla negazione dell'altro **soddisfa D1 e
D2 e fallisce D3 e D4** — cioè è formalmente corretta e conoscitivamente vuota.

E la conseguenza pratica: un lato «resto» **non si può sottodividere**. Non
avendo intensione, non ha struttura interna da tagliare — e il metodo si ferma
lì, che è il modo in cui l'inganno si paga.

#### Divisioni valide, e sono per SPECIE

Le prime divisioni sono per **specie** (§2-bis), non per tema, perché solo la
specie è decidibile su una clausola isolata. Candidate:

```text
  REGOLE            vs   FATTI               (come si deriva  /  che cosa vale)
  COMPRENSIONE      vs   CONOSCENZA DATA     (come si legge   /  che cosa si sa)
  PRESENTAZIONE     vs   SOSTANZA            (come si dice    /  che cosa è vero)
  CONDOTTA          vs   CONTENUTO           (chi parla e quando / che cosa dice)
```

Ognuna soddisfa D3 e D4: entrambi i lati si riconoscono guardando una clausola.
La scelta fra loro **non è indifferente** e va dichiarata nel registro (§8),
perché decide che cosa sarà incrociabile ai passi successivi.

#### E i temi arrivano in fondo, non in cima

Un tema — scacchi, chimica, i pronomi — **non è una regione di primo livello**.
Diventa una regione solo dopo molte iterazioni, quando gli altri frammenti
attorno sono a loro volta partizionati, e la somma logica torna a essere la KB.

> **Un cross fra due temi è legittimo solo alla profondità in cui entrambi sono
> regioni di una partizione completa.** Prima di quel livello, «incrociare
> scacchi e informatica» non è un cross del metodo: è un esempio di inferenza
> interessante, e chiamarlo cross confonde il risultato con l'aneddoto.

### 3.1 Passo 1 — la bipartizione, e il primo cross

Si divide la conoscenza in **due supergruppi**, con due vincoli non negoziabili:

1. **completi**: la loro unione è tutta la conoscenza — niente resta fuori;
2. **disgiunti**: nessuna clausola sta in entrambi.

Poi si costruisce **almeno un cross**: un'inferenza che, per produrre il suo
risultato, **deve passare per entrambe le regioni**. Non due fatti che si
citano: una derivazione che *cade* se una delle due sparisce (§4).

```text
   ┌───────────── A ─────────────┐   ┌───────────── B ─────────────┐
   │                             │ ✕ │                             │
   └─────────────────────────────┘   └─────────────────────────────┘
   regioni: 2      cross possibili: 1      nuovi a questo passo: 1
```

### 3.2 Passo 2 — si divide **uno** dei due

`A` si divide in `A1` e `A2`. I cross da costruire sono quelli dei due nuovi
contro l'altro grosso — `A1 ✕ B`, `A2 ✕ B` — più il fratello `A1 ✕ A2`.

```text
   ┌──── A1 ────┐┌──── A2 ────┐   ┌───────────── B ─────────────┐
   regioni: 3      cross possibili: 3      nuovi a questo passo: 2
```

### 3.3 Passo 3 — tocca all'altro

`B` si divide in `B1` e `B2`, e si incrociano contro `A1` e `A2` e fra loro.

```text
   ┌── A1 ──┐┌── A2 ──┐   ┌── B1 ──┐┌── B2 ──┐
   regioni: 4      cross possibili: 6      nuovi a questo passo: 3
```

### 3.4 E così via — **alternando**, con il registro

Ogni iterazione sceglie una regione e la divide a metà. **L'alternanza non è
estetica**: dividere sempre lo stesso lato produrrebbe un albero sbilanciato in
cui una metà della conoscenza resta un blocco opaco, e i suoi archi non si
raffinano mai.

Il **registro della suddivisione** è parte del piano, non un sottoprodotto: dice
in ogni momento qual è l'albero delle regioni, chi è stato diviso e quando,
quali cross esistono e quali sono deliberatamente non costruiti.

---

## 4. Che cos'è un **cross**, in forma certificabile

Questo è il punto in cui il metodo o diventa rigoroso o diventa fuffa.

> **Definizione.** Esiste un cross fra le regioni `R` e `S` quando esiste una
> domanda la cui derivazione **tocca clausole di entrambe**, e che **non regge**
> se una delle due regioni viene ablata.

Due condizioni, e servono entrambe:

| | condizione | perché |
|---|---|---|
| **C1** | l'impronta della derivazione contiene predicati di `R` **e** di `S` | senza questo il cross è una coincidenza lessicale |
| **C2** | ablando `R` (o `S`) la risposta **cade o degrada** | senza questo una delle due regioni era decorativa |

⭐ **E il meccanismo per verificarlo esiste già.** `kb_footprint_reset()` azzera
a ogni turno, `kb_footprint_width()` dice quanti predicati distinti sono stati
toccati e `kb_footprint_pred(i)` li nomina, uno per volta. **C1 è misurabile
oggi**, senza scrivere niente: basta chiedere a quale regione appartiene ogni
predicato dell'impronta. E C2 è l'ablazione, che è già la regola di chiusura di
ogni voce di questo repository — *«una voce si chiude quando un cricchetto la
tiene ferma e un'ablazione la fa cadere»*.

⛔ **Che cosa NON è un cross:**

- un fatto che nomina parole di due regioni (`related(chess, computer_science)`)
  — è un'asserzione sulla connessione, non una connessione;
- una regola che sta in `R` e cita un predicato di `S` senza che nessuna domanda
  la percorra — è un arco disegnato, non attraversato;
- un cross che regge anche togliendo una delle due regioni — allora l'inferenza
  passava da un'altra parte, e il cross è un'illusione di provenienza.

---

## 5. La misura, e perché è la parte che serve davvero

Sia **n** il numero di regioni foglia (il **grado di frammentazione**).

| | | |
|---|---|---|
| **cross possibili** | tutte le coppie | `n(n−1)/2` |
| **nuovi a questo passo** | dividendo una regione si arriva a `n` | `n−1` |
| **cross certificati** | quelli che passano C1 **e** C2 | misurati |
| ⭐ **indice** | | **certificati / possibili** |

```text
   n      possibili      nuovi
   2              1          1
   3              3          2
   4              6          3
   5             10          4
   8             28          7
  16            120         15
  32            496         31
 110          5 995        109
```

### 5.1 ⚠ A scala i cross non si potranno costruire tutti — ed è il punto

Con 110 regioni servirebbero **5 995** cross. Non si faranno mai tutti, e non è
un difetto del metodo: **è la ragione per cui serve l'indice.** Un numero che si
può solo far salire è una vanità; questo si può far **scendere**, e questa è la
proprietà che lo rende onesto:

> **Dividere una regione senza costruire i cross nuovi ABBASSA l'indice.**

Cioè: raffinare la conoscenza senza connetterla è registrato come un
peggioramento. È esattamente l'incentivo giusto, ed è l'opposto di quello che una
KB che cresce per isole produce da sola.

### 5.2 Le tre letture dell'indice

| lettura | domanda a cui risponde |
|---|---|
| **globale** | quanto della mia conoscenza so usare insieme? |
| **per regione** | quale isola è più sola? (poche coppie certificate sulle sue) |
| **per coppia** | questo confine è attraversabile, o l'ho solo disegnato? |

La seconda è quella operativa: **l'isola più sola è il prossimo lavoro**, e la
sceglie il numero invece dell'intuizione.

---

---

## 5-bis. ⭐ LA DOMANDA VERA — quanto la conoscenza si connette DA SOLA

> F.: *«quello che dobbiamo studiare e dimostrare con questo meccanismo è quale
> sia il rapporto fra i cross costruiti e testati e quelli che spontaneamente si
> connettono come side effect della conoscenza stessa: il grado di crescita
> spontanea. Per ogni cross che noi garantiamo funzionante, quanti nuovi cross si
> autodeterminano.»*

**Questa è la misura per cui il metodo esiste.** L'indice del §5 dice *quanto*
siamo connessi; questo dice **se la connessione si propaga da sola** — cioè se
la conoscenza è un investimento che compone o un lavoro lineare.

### 5-bis.1 Le due popolazioni, e come si distinguono

| | | |
|---|---|---|
| **curato** | un cross che qualcuno ha **deciso di costruire** e ha certificato | il costo |
| **spontaneo** | una coppia `(R,S)` che diventa certificata **senza che nessuno l'abbia costruita**, dopo che un altro cross è entrato | il ricavo |

La distinzione non è di intenzione ma di **procedura**, e va tenuta rigida:
è spontaneo solo ciò che si certifica su una **domanda mai usata per costruire
niente** (§9.5). Se la domanda l'abbiamo scritta noi pensando a quella coppia,
quel cross è curato — anche se ci sembra emerso.

### 5-bis.2 Il coefficiente

Dopo ogni cross curato si **ricensisce l'intero insieme delle coppie** e si
contano quelle diventate certificate senza intervento:

```text
        Δ cross spontanei certificati
  κ  =  ─────────────────────────────
             1 cross curato
```

| κ | che cosa significa |
|---|---|
| **κ = 0** | la conoscenza è inerte: ogni arco costa quanto vale, per sempre |
| **0 < κ < 1** | attrito: si progredisce, ma il lavoro non si ripaga da solo |
| **κ = 1** | pareggio: un arco costruito ne regala uno |
| ⭐ **κ > 1** | **la conoscenza compone**: l'indice sale più in fretta di quanto si costruisce, e la crescita ha un interesse |

**È una predizione falsificabile su parrot0, non una speranza**, ed è la ragione
per cui il registro (§8) tiene lo stato di *ogni* coppia e non solo di quelle su
cui si lavora: senza il censimento completo, κ non è calcolabile.

### 5-bis.3 ⛔ La distinzione che rende la misura seria: transitivo ≠ emergente

Un cross spontaneo può nascere in due modi profondamente diversi, e confonderli
gonfierebbe κ senza dire niente.

```text
  κ_transitivo   A✕B e B✕C esistono, e A✕C si certifica passando per B.
                 È vero, è utile, ed è ATTESO: la catena c'era già.

  κ_emergente    A✕C si certifica e la derivazione NON passa per B.
                 Due regioni si sono trovate per una via che nessuno aveva
                 previsto. È questo il risultato interessante.
```

**Si separano meccanicamente**, senza giudizio: l'impronta della derivazione
(`kb_footprint_pred`) dice per quali regioni è passata. Se contiene `B`, è
transitivo; se non lo contiene, è emergente.

> ⭐ **La tesi da dimostrare, in una riga:** `κ_emergente > 0`, stabilmente.
> Un κ fatto solo di transitività dice che la KB è un grafo ben collegato — cosa
> buona ma prevedibile. Un κ_emergente positivo dice che **aggiungere un arco
> rende visibili strade che non esistevano**, ed è l'unica evidenza che
> giustifichi «espansione mentale» invece di «manutenzione di un indice».

### 5-bis.4 Che cosa fa salire κ, e quindi che cosa conviene costruire

κ non è una proprietà della KB: è una proprietà **dei cross che scegliamo**. Da
qui l'uso operativo, che è il vero prodotto di questa sezione:

> **Fra due cross candidati si costruisce quello con il κ atteso più alto, non
> quello più facile.**

Tre ipotesi su che cosa lo alzi, da falsificare con i dati e non da assumere:

| ipotesi | perché | come si falsifica |
|---|---|---|
| **H-κ1** | un cross fra regioni **lontane** produce più spontanei di uno fra vicine | misurare κ per distanza nell'albero delle suddivisioni |
| **H-κ2** | un cross che passa da un **arco a predicato variabile** (`representation_bridge/4`) produce più spontanei di uno cablato su due predicati fissi, perché è riusabile da chiunque | confrontare κ delle due forme |
| **H-κ3** | un cross su una **relazione generale** (contenimento, causa, parte-tutto) produce più spontanei di uno su una relazione di dominio | classificare i curati e confrontare |
| ⭐ **H-κ4** | un cross di **specie** (§2-bis) produce più spontanei di un cross di **tema**: un tema si applica al suo tema, una procedura si applica a molti temi | κ misurato separatamente per i tre tipi di cross |

Se H-κ2 regge, ne segue una regola forte: **un cross va costruito nella forma più
generale che lo sostiene**, perché la forma decide il ricavo, non solo il costo.

### 5-bis.5 Il rapporto con la Leva 1, che diventa un caso particolare

`THINKING_TODO` H11 chiedeva già che
`cold tasks unlocked / new curated clauses` crescesse più di uno-a-uno. **κ è la
stessa idea sull'oggetto giusto**: non i task sbloccati (che dipendono da quali
task capitano) ma gli **archi** che si aprono, che è una proprietà della
conoscenza e non del campione di domande. La vecchia metrica resta valida e
diventa una sua conseguenza osservabile.

### 5-bis.6 ⚠ I due modi di barare, e le guardie

1. **Contare come spontaneo ciò che era latente.** Una coppia già certificabile
   *prima* del cross curato, e che nessuno aveva censito, non è spontanea: è
   arretrato. **Guardia:** il censimento completo va rifatto **prima** di ogni
   cross curato, non solo dopo, e κ si calcola sulla differenza.
2. **Gonfiare con la frammentazione.** Dividere molto crea molte coppie, e
   qualcuna si certifica per caso. **Guardia:** κ si misura **a frammentazione
   costante** — fra due divisioni, mai a cavallo di una.

---

## 6. I fenomeni bersaglio — e perché NON sono cross

> ⛔ **Correzione, 2026-09-04.** La prima stesura di questa sezione presentava
> «scacchi ✕ informatica», «matematica ✕ chimica» e «pronomi ✕ animali» come i
> cross di collaudo del metodo. **Era sbagliato e fuorviante**, e F. l'ha
> fermato: quelli sono **temi presi a coppie**, non regioni di una partizione.
> Un cross è sempre fra due regioni tali che, sommate a tutte le altre, danno la
> KB intera (§3.0-bis). Prima di quella profondità, incrociare due temi non è il
> metodo — è un esempio di inferenza interessante travestito da risultato.

Restano però **i fenomeni bersaglio**: le inferenze che il metodo deve
finire per produrre. Cambiano di ruolo — da *prove del metodo* a **domande
testimone**, cioè le domande su cui un cross si certifica quando la
frammentazione ci arriva.

| fenomeno | domanda testimone | che cosa serve perché sia un cross |
|---|---|---|
| scacchi / informatica | *«il giro del cavallo su 8×8 è un problema difficile?»* | che `scacchi` e `complessità` siano **due regioni**, con il resto della KB partizionato attorno |
| matematica / chimica | *«bilancia questa reazione»* | idem — e qui il cross è **pieno** (una *procedura* algebrica su un *tema* chimico, §2-bis.3) |
| pronomi / animali | *«il cane è "it" o "he"?»* | il più superficiale dei tre: due regioni vicine nell'albero, raggiungibili prima |

⭐ **E la loro utilità vera è un'altra, ed è migliore:** ognuno di questi
fenomeni dice **quanto in profondità bisogna scendere** perché diventi
esprimibile. La posizione nell'albero delle suddivisioni in cui una domanda
testimone diventa un cross legittimo **è una misura del metodo**, non un
aneddoto: un fenomeno che richiede otto iterazioni per diventare un cross ci
dice che quella conoscenza è sepolta otto livelli sotto la superficie.

### 6.1 ⭐ Il caso di casa — ed è l'unico che è già un cross vero

Quello del §1, che è il motivo per cui questo documento esiste, **e che regge il
criterio del §3.0-bis** dove gli altri tre non lo reggono ancora:

```text
  divisione valida per SPECIE:   PROCESSO   vs   CAPACITÀ
     (che cosa si deve fare)          (che cosa si sa fare)
  entrambi hanno intensione propria; una clausola isolata si classifica; e la
  loro somma, con il resto partizionato attorno, è la KB.

  il cross:  action_impl(Passo, Primitiva)
  lo stato:  9 azioni su 28 lo hanno — ZERO sulle quattro di `code_task`
```

Non è ipotizzato: è **misurato mancante**. Applicato ieri, il metodo avrebbe
indicato quel confine come il più povero del sistema e avrebbe risparmiato una
giornata di caccia al dispatch. Ed è un **cross di specie**, cioè proprio la
classe che H-κ4 (§2-bis.4) predice essere la più fruttuosa.

---

## 7. La prima bipartizione di parrot0

Il passo 1 chiede due supergruppi che passino **D1-D4** (§3.0-bis). Fra le
candidate elencate lì — regole/fatti, comprensione/conoscenza data,
presentazione/sostanza, condotta/contenuto — la proposta, che va discussa prima
di essere eseguita:

```text
A — PRESENTAZIONE      come si dice, si riconosce, si dispone una risposta
                       forme, intenti, morfologia, discorso, registro, messaggi
B — SOSTANZA           che cosa è vero, e come si deriva
                       domini, esperti, IR del codice, piani, procedure
```

⚠ **È un taglio per SPECIE, non per tema** (§2-bis), ed è deliberato: è la
stessa linea che F. ha già tracciato come direzione (`substance ⟂ presentation`),
quindi la bipartizione **mette alla prova una tesi che già esiste** invece di
inventarne una nuova.

⛔ **E lascia scoperta una terza specie, che va collocata prima di cominciare: la
CONDOTTA.** `faculty_yield`, `module_claim_right`, `appropriate_move`,
`thinking_outcome_policy` non dicono né *come si dice* né *che cosa è vero*:
dicono **chi parla, quando, e con quale precedenza**. Sono 46 clausole di
ordinamento più 1 305 di preferenza, e sono la specie più scarsa e più decisiva.
Delle tre opzioni — dentro A, dentro B, o un terzo supergruppo che rompe la
bipartizione — la prima iterazione deve **sceglierne una e dichiararla nel
registro**, perché da quella scelta dipende quali cross saranno possibili al
passo 2. Se
la frontiera è quella giusta, il primo cross sarà facile da trovare e difficile
da ablare; se non lo è, si vedrà subito — ed è informazione.

Verifica delle condizioni, che va fatta e non assunta:

| | |
|---|---|
| **D1/D2** | ogni clausola sta in uno e uno solo dei due — da provare sul censimento, non a occhio |
| **D3** | entrambi hanno intensione propria: *«dice come si dice»* e *«dice che cosa è vero»*. Nessuno dei due è «il resto» ✓ |
| **D4** | data una clausola isolata si decide: `response_template(...)` è presentazione, `located_in(paris, france)` è sostanza ✓ |

Il primo cross candidato: **una risposta la cui *forma* dipende da un fatto di
dominio** (per esempio: dire un numero con la sua unità solo quando il dominio
dichiara che l'unità è obbligatoria). Tocca A e B, e cade se si toglie uno dei
due.

---

## 8. Il registro, come conoscenza

Il registro non è un allegato del documento: è **conoscenza in KB**, perché
altrimenti l'indice non è interrogabile e il metodo torna a essere una promessa.

```prolog
kb_region(Regione, Genitore).        % l'albero delle suddivisioni
region_criterion(Regione, tema | specie | livello | misto).  % §2-bis.2
region_holds(Regione, Predicato).    % quali predicati vivono in una regione
region_split(Genitore, Figlio1, Figlio2, Iterazione).
cross(RegioneA, RegioneB).           % dichiarato: questo confine ci interessa
cross_status(RegioneA, RegioneB, certified | attempted | deferred | refuted).
cross_witness(RegioneA, RegioneB, "la domanda che lo attraversa").
cross_ablation(RegioneA, RegioneB, "che cosa cade togliendo A").

% ── e cio' che serve a κ (§5-bis): senza questi il coefficiente non esiste ──
cross_origin(RegioneA, RegioneB, curated | spontaneous).
cross_kind(RegioneA, RegioneB, tema | specie | pieno).   % §2-bis.3 — serve a H-κ4
cross_lineage(RegioneA, RegioneB, ViaRegione).   % transitivo: per dove passa
cross_certified_at(RegioneA, RegioneB, Censimento).  % QUANDO si e' certificato
census(Numero, Frammentazione, Certificati).     % il censimento completo
```

`cross_status(_, _, deferred)` è **obbligatorio e non è una scusa**: a scala i
cross non si fanno tutti, e un confine deliberatamente non costruito va
*dichiarato*, non dimenticato. La differenza fra un piano onesto e un backlog è
tutta lì.

---

## 9. Gate anti-impostore

1. **Un cross senza ablazione non è un cross.** C1 senza C2 è una coincidenza.
2. **Nessuna matrice a monte.** Le regioni si dividono una per iterazione, e i
   cross si costruiscono dopo la divisione. Generare `n(n−1)/2` righe vuote e
   chiamarle archi è il camuffamento del mantra #18(b) — *un template vuoto non è
   una resa*.
3. **L'indice deve poter scendere.** Se una divisione non abbassa l'indice
   finché i cross nuovi non esistono, il numero è truccato.
4. **Le regioni sono complete e disgiunte a ogni passo.** Una clausola che non
   sta in nessuna regione è un buco che l'indice non vede — e quindi mente.
5. ⛔ **Nessun lato «resto».** Ogni divisione passa D3 (intensione positiva
   bilaterale) e D4 (decidibilità su una clausola isolata): «X vs non-X»
   soddisfa completezza e disgiunzione ed è **barare**, perché un lato senza
   intensione non ha struttura interna e non si può sottodividere. Il metodo si
   fermerebbe lì, ed è il modo in cui l'inganno si paga.
6. **Un cross fra temi vale solo alla profondità in cui entrambi sono regioni.**
   Prima di quel livello è un esempio, non un risultato (§6).
5. **Un cross si prova su una domanda che nessuno ha usato per costruirlo.**
   Altrimenti si è insegnato quel caso, non l'arco.
7. **κ si misura a frammentazione costante**, e il censimento completo si fa
   **prima e dopo** ogni cross curato — altrimenti si conta come spontaneo
   l'arretrato che nessuno aveva guardato (§5-bis.6).
8. **Uno spontaneo si certifica solo su una domanda mai usata per costruire
   niente.** Se la domanda l'abbiamo scritta pensando a quella coppia, il cross
   è curato anche se ci sembra emerso.
9. **Il metodo si applica a se stesso.** La regione «piani» e la regione
   «strumenti» sono il primo banco: se `split and cross` non avesse trovato
   `action_impl` mancante, non serve.

---

## 10. Ordine di lavoro

| | | gate |
|---|---|---|
| **S0** | il registro come KB (`kb_region`, `cross`, `cross_status`) e l'indice calcolato dall'impronta | l'indice si stampa e vale **0** |
| **S1** | bipartizione presentazione/sostanza + il primo cross certificato | 1 cross su 1 possibile, e l'ablazione lo fa cadere |
| **S2** | dividere la sostanza; i due cross nuovi | indice su 3 possibili, e **scende** prima di risalire |
| **S3** | dividere la presentazione; i tre cross nuovi | indice su 6 |
| **S4** | portare la frammentazione fino a dove una **domanda testimone** (§6) diventa esprimibile come cross fra due regioni vere | la profondità richiesta è **registrata**: è la misura di quanto quella conoscenza era sepolta |
| **S5** | il caso di casa: `action_impl` per le quattro azioni di `code_task` | il piano si cammina fino in fondo su match0 |
| ⭐ **S6** | **la misura di κ**: censimento completo prima e dopo ogni cross curato, con la separazione transitivo/emergente dall'impronta | κ ha un valore, e `κ_emergente` è **> 0** almeno una volta — altrimenti la tesi del §5-bis è falsa e va detto |
| **S7** | H-κ1/H-κ2/H-κ3 sui dati raccolti da S1-S6 | almeno una delle tre cade, oppure una regge con un margine che cambia l'ordine di costruzione |

---

## 11. Stato di partenza, misurato

| | |
|---|---|
| file di conoscenza | **110** (58 in `kb/core`, 52 in `kb/experts`) |
| ponti a predicato variabile vivi (`representation_bridge/4`) | **10** |
| cross possibili se ogni file fosse una regione | **5 995** |
| indice, con quella lettura grezza | **≈ 0,17 %** |

⚠ Il numero va letto per quello che è: `representation_bridge` non è l'unica
forma di arco, e «un file = una regione» non è la partizione che il metodo
prescrive. **È un fondo scala, non una diagnosi** — serve a dire che il margine
è quasi tutto, non a giudicare quello che c'è.

---

## 12. Che cosa questo metodo non è

- **Non è un'ontologia.** Non impone una tassonomia del sapere: la partizione è
  strumentale e si rifà a ogni iterazione. E non è nemmeno una tassonomia dei
  *temi*: una regione può essere una **specie** — le procedure, gli ordinamenti,
  la condotta — e i cross più preziosi attraversano proprio quel confine (§2-bis).
- **Non è un grafo di similarità.** Due regioni possono essere lontanissime e
  avere un cross fortissimo — pronomi e animali.
- **Non è completo, e lo dichiara.** A scala i cross si scelgono; il metodo
  serve a scegliere *sapendo che cosa si sta rinunciando a fare*.
- **Non sostituisce la crescita delle isole.** Dice solo che, a parità di
  sforzo, un arco vale più di un'isola — e ora c'è un numero che lo dimostra
  invece di una convinzione.

⭐ **E se κ_emergente risultasse stabilmente nullo, il metodo avrebbe comunque
fatto il suo lavoro**: avrebbe dimostrato che questa conoscenza non compone, che
ogni arco costa quanto vale, e che l'unica strada è costruirli a uno a uno con un
ordine scelto dall'indice. Sarebbe un risultato negativo **utile**, e va
dichiarato in anticipo che lo accetteremmo — altrimenti la misura è una
cerimonia.
