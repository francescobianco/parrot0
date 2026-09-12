# Lettura della prosa — il miglioramento continuo della comprensione

> **Piano vivo.** Non si chiude: si cricchetta. La misura è
> [`scripts/prose-probe.sh`](../../scripts/prose-probe.sh) (`make prose-probe`),
> e ogni giro deve farne scendere l'ultimo numero.
>
> **Aperto il 2026-09-12 (gen513)** su indicazione di F., dopo il primo referto
> del banco: **0 risposte su 7** e **0 su 5** su due prose vere ed esterne alla KB.
>
> **Sta sotto:** [`kb-first.md`](kb-first.md) (la bussola),
> [`universal-comprehension.md`](universal-comprehension.md) (il contratto di
> comprensione), [`universal-input.md`](universal-input.md) (la prosa è un
> registro fra altri), [`extract-knowledge-from-prose.md`](extract-knowledge-from-prose.md)
> (i frame di estrazione come fatti).
> **Porta avanti:** [`the-linguistic-glue.md`](the-linguistic-glue.md),
> [`inferenza-compositiva.md`](inferenza-compositiva.md).

---

## 0. La tesi (F., 12 settembre 2026)

> «Il concetto generale è che dovrebbe essere usata la **comprensione universale**
> e il **modello IR** per incamerare la prosa e poi poterla lavorare in tutte le
> infinite forme che una prosa può rappresentare: codice, documenti, teorie
> matematiche, ecc. Sono infinite le cose che una prosa può rappresentare, e
> anche in maniera **mixata** una stessa prosa può spaziare. Quindi il concetto è
> che **la IR deve essere generale**.»

Detto come contratto operativo:

> **Una prosa si incamera UNA VOLTA, in una rappresentazione intermedia che non
> sa a che cosa servirà. Chi la lavora — il lettore di fatti, il ponte verso il
> codice, il costruttore di documenti, il solutore matematico, il pianificatore —
> è un CONSUMATORE di quella rappresentazione, non un secondo lettore della
> stringa.**

E il corollario che rende la tesi falsificabile:

> **Una forma nuova che la prosa può rappresentare deve costare un CONSUMATORE
> dell'IR, mai uno scanner nuovo della superficie.** Se per leggere le teorie
> matematiche bisogna riscrivere un tokenizzatore, la IR non era generale.

Perché è la mossa giusta e non una preferenza estetica: le forme che una prosa
può rappresentare sono **infinite e miste** — lo stesso paragrafo di un manuale
porta una definizione, una formula, un vincolo di sicurezza e un passo di
procedura. Un lettore per forma moltiplica per il numero delle forme; un'IR
sola, con N consumatori, somma.

---

## 1. Dove siamo davvero (misurato il 12 settembre, non ipotizzato)

### 1.1 L'IR **esiste già**, ed è meglio di come viene usata

`kb/core/input-structure.p0` (gen438) dichiara una gerarchia osservata
dell'input:

```prolog
input_node(Scope, Id, node(Level, Kind, Parent), range(Start, Len))
input_node_surface(Scope, Id, "Surface")
input_node_role(Scope, Id, Role)
```

con sopra una cinquantina di viste interrogabili — entità, classi, unità,
operatori, coordinazioni, lacune, numeri, ordine fra nodi, frame semantici,
asserzioni binarie, provenienza di clausola. Non è un abbozzo: è un albero con
range di byte e ruoli, che conserva la catena *superficie + lingua → concetto*
prima che la canonicalizzazione perda la forma.

### 1.2 Il problema non è che manca: è che **quasi nessuno la consuma**

| | |
|---|---|
| file di KB che consumano l'IR | **7** |
| `split_words(...)` — riscansioni della stringa grezza nei tre lettori maggiori | **217** (`10-memory-knowledge.c` 130, `25-wordmath-reasoning.c` 54, `30-generation-reading.c` 33) |

Ogni `split_words` è un lettore che **riapre la frase da capo** con la propria
idea di dove finiscono le cose. Non è duplicazione innocua: è il motivo per cui
due lettori dello stesso turno sono in disaccordo su che cosa c'è scritto, e
perché il primo che afferra il turno vince.

### 1.3 Il referto del banco, il 12 settembre

`make prose-probe` su due lead veri di Wikipedia, esterni alla KB
(`tests/fixtures/prose/`, fonti in `SOURCES.md`), con domande la cui risposta è
**scritta nel testo**:

```text
tardigrade   0 risposte su 7
quipu        0 risposte su 5
```

Il numero zero non è l'informazione utile. L'informazione utile è il **passo 1**
del banco — ogni frase da sola, e non «ha risposto?» ma «**che cosa ne ha
capito?**».

---

## 2. Le quattro malattie, e perché sono la stessa

| # | la frase (prosa vera) | che cosa ne fa oggi |
|---|---|---|
| **M1** | «Tardigrades, **also known as** water bears or moss piglets, **are a phylum of eight-legged segmented micro-animals**.» | `Learned: tardigrades also known as "water bears or moss piglets are a phylum of eight-legged segmented micro-animals"` |
| **M2** | «They were **first** described by the German zoologist … in 1773» | «I am not sure what you mean by «first».» |
| **M3** | «…named them Tardigrada, **which means** "slow walkers".» | «I found the teaching pivot, but I cannot align the same variables…» |
| **M4** | «Quipu, also spelled khipu, are record-keeping devices…» | «**Quipu was a mysterious Quipu. Then one day, quipu discovered what it meant to be seen…**» |

Sono quattro sintomi e una malattia sola: **nessuno di questi lettori ha
consultato l'IR.**

- **M1 — l'apposizione non ha una fine.** Chi legge «also known as» prende tutto
  il resto della riga come valore. L'IR sa dove finisce l'apposizione: le due
  virgole sono un nodo con il suo `range`. Il lettore non gliel'ha chiesto.
  **È la specie peggiore**, perché un fatto storto entra in KB e non si lamenta —
  un fatto invece di quattro, e nessun segnale.
- **M2 — una parola prende il turno.** Un ordinale dentro la frase («first»)
  dirotta il turno sul chiarimento, e una frase attributiva al passato non viene
  letta mai. L'IR sa che «first» è dentro un avverbiale di una passiva, non è la
  domanda del turno.
- **M3 — una relativa letta come lezione.** «which means» è il perno di una
  lezione sulle parole **quando è il turno a essere una lezione**; dentro una
  frase dichiarativa è una relativa. Chi l'ha presa non ha guardato in che nodo
  si trovava.
- **M4 — chi PRODUCE prosa risponde a chi la PORTA.** Il generatore di racconti
  si prende un turno che portava un testo da leggere. Non è un difetto di
  lettura: è una **condotta di cessione mancante**, ed è il più economico da
  chiudere perché è KB pura.

> **La regola che ne esce, e che vale per ogni giro futuro:** quando un lettore
> sbaglia un confine — dove finisce un valore, dove comincia una clausola, a che
> cosa si riferisce un pronome — **la cura non è aggiustare quel lettore: è
> fargli chiedere all'IR il confine che l'IR conosce già.** Aggiustare il lettore
> è il modo in cui si arriva a 217 scanner.

---

## 3. La forma d'arrivo

```text
                       ┌──────────────────────────────┐
   prosa  ──────────▶  │  UNA lettura, UNA IR         │
   (detta, letta,      │  input_node/4 + ruoli + range│
    scaricata,         └──────────────┬───────────────┘
    da un file)                       │
                    ┌─────────────────┼─────────────────┬──────────────┐
                    ▼                 ▼                 ▼              ▼
              fatti e classi      modelli/codice     documenti      piani, prove,
              (mod_knowledge)     (model-bridge)     (document-*)   matematica, …
```

Tre proprietà che la rendono *generale* invece che *ampia*:

1. **L'IR non sa a che cosa servirà.** Registra struttura — nodi, ruoli, range,
   lingua — non intenzioni. Nel momento in cui l'IR contiene un nodo `formula`
   perché qualcuno vuole generare codice, ha smesso di essere generale.
2. **I consumatori si dichiarano, non si compilano.** Un consumatore nuovo è un
   insieme di regole KB sopra le viste dell'IR, come
   `kb/core/model-lesson.p0` è un consumatore per i modelli. Il gen513 lo ha già
   dimostrato: quel lettore è **tutto in KB, zero righe di C**, agganciato a
   `bookkeeper/1`.
3. **Le forme miste non sono un caso speciale.** Se la stessa frase porta una
   definizione e una formula, due consumatori la leggono e ne ricavano due cose
   diverse. È possibile solo se nessuno dei due «consuma» il turno impedendo
   all'altro di vederlo — cioè solo se leggere non è dispatchare.

> **Il confine (mantra #7, PRINCIPLES anti-inganno).** Una comprensione parziale
> si DICHIARA. Un'IR che riempie i buchi per essere completa produce fatti
> plausibili e non verificabili, che è il danno peggiore di un muro. Il banco
> misura le risposte giuste, mai «quanto sembra aver capito».

---

## 4. Il metodo: come si cricchetta

1. **Il banco prima della cura.** Ogni giro comincia da `make prose-probe`, e
   il testo su cui si lavora va aggiunto a `tests/fixtures/prose/` **con la
   fonte**. Prosa vera ed esterna alla KB: una risposta giusta su prosa che
   parrot0 già conteneva non prova niente.
2. **Il passo 1 vale più del passo 2.** «Che cosa ne ha capito» dice la specie
   del limite; «ha risposto» dice solo che c'è.
3. **Una malattia per giro**, con la prova di chiusura scritta prima.
4. **Ogni cura si chiede: è un consumatore dell'IR o è uno scanner nuovo?** Se è
   uno scanner, si è appena aggiunto il 218°.
5. **Un fatto storto conta come un fallimento più grave di un muro**, e va
   contato a parte: M1 è peggio di M2.
6. **Niente suite.** Il banco è una sonda e si lancia a mano (politica dei test
   di F.).

---

## 4-bis. E QUESTO PIANO E' IL VEICOLO PER PORTARE IL C IN KB (F., 12 settembre)

> F.: «ricordiamo che questo piano deve essere anche un'occasione per migrare
> codice C in equivalente addestrabile su KB con il principio KB-first».

Non è un requisito aggiunto: è **la stessa cosa detta dall'altro lato**. I 217
`split_words` non sono soltanto la causa delle quattro malattie — sono anche
**l'inventario del C da portare via**. Ogni lettore che smette di riscandire la
stringa e comincia a consultare l'IR è, nello stesso atto, una riga di motore in
meno e una regola insegnabile in più.

Il gen513 lo ha già dimostrato due volte, e le due prove vanno tenute come
modello di misura:

| cosa | com'era | com'è |
|---|---|---|
| il lettore dei modelli (`model-lesson.p0`) | non esisteva; sarebbe stato un ramo in C | **un lettore intero in KB, zero righe di C** — copula, foglie, operatori infissi e prefissi, fold, risposta — agganciato a `bookkeeper/1`, che era già una porta |
| il ponte modello→codice | 4 righe in C**…**in KB che sapevano il nome `law_formula` | `model_carrier/1` + `apply/2`: non conosce nessun predicato |

### La disciplina, per ogni giro

1. **Prima si cerca la porta che esiste già.** `bookkeeper/1`, `turn_response/2`,
   `turn_plan_candidate/1`, `faculty_yield*`, `answer_frame/2`,
   `turn_pattern/3` sono porte KB già aperte: il C le interroga e non sa che cosa
   ci passi dentro. Una cura che entra da lì costa **zero** righe di motore.
   (Mantra #5: grep prima.)
2. **Se la porta non c'è, se ne apre UNA, sottile.** Una primitiva, non un ramo:
   deve dire *come* si fa una cosa, mai *quali parole*. `kb_rule_body/2` e
   `kb_journal_refused/4` di questo mese sono della taglia giusta.
3. **Il test del mantra, ogni volta:** *«parrot0 può impararne un nuovo membro
   domani, senza ricompilare?»* Se la risposta è no, la conoscenza è nel posto
   sbagliato — anche se il banco è diventato verde.
4. **Il debito si misura, non si racconta.** Ogni giro riporta qui sotto due
   numeri: quanti `split_words` restano nei tre lettori maggiori, e quante righe
   di C sono uscite. Senza il conteggio, «abbiamo migrato» è un'opinione.
5. **Additivo, mai sostitutivo** ([[keep-secondary-structures]]): il lettore
   vecchio resta come struttura secondaria finché il nuovo non si dimostra
   prevalente. Si toglie quando dà fastidio, non in campagna.

### Le prede grosse, già identificate

- **`split_words` nei tre lettori** — 130 / 54 / 33. È la misura di partenza.
- **Le catene compilate di `&&`** — `TODO(kb-first, gen489)`: 213 istruzioni con
  due o più `kb_cue_match` in congiunzione, fino a quindici congiunti. La forma
  d'arrivo esiste già (`turn_pattern/3` + `turn_pattern_intent/2`, motore in
  `00-lex.c`, esempio in `taught_turn_form.p0t`): ogni malattia curata qui è
  un'occasione per spostarne una.
- **I `mod_*` obsoleti** ([[mod-star-legacy]]): pre-segmentazione, e sono
  proprio i lettori che riscandiscono. Si aggiornano quando danno fastidio — e
  una malattia di questo piano È il fastidio.
- **Le 30 voci di [`kb-first-audit.md`](kb-first-audit.md)**: quando una tocca la
  lettura, si chiude qui.

---

## 5. I giri, in ordine, con la prova di chiusura

| giro | malattia | cura prevista | prova |
|---|---|---|---|
| ~~G1~~ ✅ | **M4** cessione | condotta KB: una facoltà che PRODUCE prosa cede un turno che PORTA prosa (`faculty_yield_force`) | «Quipu, also spelled khipu, …» non riceve più un racconto |
| ~~G2~~ ✅ | **M1** apposizione | il valore chiede all'IR dove finisce il suo nodo (`np_closer`, le virgole come confine) | la frase 1 lascia in KB i fatti giusti, non uno storto |
| **G3** | **M2** ordinale | «first» dentro un avverbiale non è la domanda del turno | la frase 2 si legge; «who described tardigrades?» risponde |
| **G4** | **M3** relativa | «which/who» aprono una relativa sul nodo precedente, non una lezione | la frase 3 lascia `means(tardigrada, "slow walkers")` |
| **G5** | consumo | portare UN lettore grosso a consumare l'IR invece di `split_words` | il conteggio 217 scende, e il banco non peggiora |
| **G6** | **M5** preposizione orfana | «What are X also known **as**?» — l'oggetto e' in testa, la preposizione resta in coda e viene presa per oggetto | la domanda trova il fatto che «What is another name for X?» trova gia' |

**Ogni riga porta anche il suo conto KB-first**: quale porta KB è stata usata (o
aperta), quanti `split_words` restano, quante righe di C sono uscite.

Dopo G1-G4 il referto del tardigrade deve passare da 0/7 a qualcosa; il numero
esatto non si promette, si misura.

---

## 6. Registro dei giri

### Giro 0 — 12 settembre 2026 (gen513): il banco e la diagnosi

Creato `scripts/prose-probe.sh` + `make prose-probe`; due prose esterne con le
fonti. Referto di partenza **0/7** e **0/5**, quattro malattie isolate e
attribuite. Nessuna cura in questo giro: prima la misura.

Debito di partenza: **`split_words` 130 / 54 / 33 = 217** nei tre lettori
maggiori. Righe di C uscite: 0.

### ✅ LA SCALA OLTRE LE 500 PAROLE (12 settembre, iterazioni 12-18)

F.: «ti fermi quando dimostri che parrot0 e' in grado di comprendere una prosa
lunga almeno 500 parole», con prose sempre diverse e un commit per iterazione.

| piolo | testo | parole | esito |
|---|---|---|---|
| r300 | barriera corallina | 299 | 3/4 |
| r356 | magnete | 356 | **2/2** |
| r497 | acciaio | 497 | **2/2** |
| r508 | satellite | 508 | 0/2 |
| **r621** | **foresta** | **621** | **2/2** |

```text
r621 — foresta, 621 parole (lead di https://en.wikipedia.org/wiki/Forest)
  a freddo, prima di leggere:  «I don't know much about forest yet»
  dopo aver letto il testo:
    What is a forest?   ->  forest is an ecosystem.                 (frase 1)
    What are forests?   ->  forests is a largest terrestrial ecosystems.  (frase 5)
  2 domande su 2.
```

> ⚠ Onesto su che cosa prova: le due risposte vengono da due frasi DIVERSE del
> testo, ma sono tutte e due DEFINIZIONALI. Le domande non definizionali su
> questo testo («dove si formano le foreste?», «che cosa succede quando…») non
> rispondono ancora: quelle forme non hanno un lettore. La comprensione
> dimostrata a 621 parole e' quella della classe, non ancora quella piena.

**Quattro cure di SCALA**, tutte invisibili su una frase e fatali su un
paragrafo — ed e' la specie che questo piano esiste per trovare:

1. **Il turno era lungo 255 byte e la prosa no** (`P0_TURN_MAX`, 4096): a 255
   byte il paragrafo arrivava mozzato e nessuna facolta' riconosceva piu' niente.
2. **«Dovunque stia» vale per una frase, non per un paragrafo**: un interrogativo
   seguito dalla copola dentro una subordinata («…explains how it works») faceva
   leggere il TURNO INTERO come domanda, e il testo non veniva piu' diviso.
3. **«Nessuno lo precede» non regge su un testo lungo**: il primo nodo si
   chiedeva per negazione, e su centinaia di nodi l'enumerazione dentro il `naf`
   non arriva in fondo. Ora si chiede direttamente (`input_node_first/2`).
4. **La forza del turno si ri-derivava a ogni domanda**: la stessa domanda dava
   due risposte diverse a due momenti dello stesso turno — `compound_statement`
   valeva subito dopo la pubblicazione e non valeva piu' trenta righe dopo. Ora
   si materializza una volta, sul turno appena pubblicato.

**Il blocco che resta, diagnosticato con precisione.** Una superficie insegnata
che legge un MODIFICATORE («@S characterized by @O») si prende la frase
principale: «A forest is an ecosystem characterized by a dense community of
trees» diventa `has(forest_is_an_ecosystem, …)`. La guardia del verbo finito la
RIFIUTA — giustamente — ma dopo il rifiuto il turno viene perso invece di
tornare al lettore di classe, e la definizione sparisce. Quindi oggi si puo'
avere la definizione **o** la relazione del modificatore, non tutte e due.
E' il primo lavoro del prossimo giro, e vale per ogni frase d'enciclopedia.

Aperto anche: il **soggetto coordinato** («A satellite **or an artificial
satellite** is an object») blocca la lettura di classe — r508 e' a 0/2 per
questo.

### La SCALA (F., 12 settembre): 10 → 150 parole, una prosa per piolo

`scripts/prose-ladder.sh` · `tests/fixtures/prose/ladder/` — quindici prose vere
ed esterne alla KB (lead di Wikipedia, fonti in `ladder/SOURCES.md`), una per
piolo, **intere e coerenti, mai troncate**: leggere un frammento e' un problema
diverso e si affronta dopo questo traguardo.

> ## ✅ TRAGUARDO — 12 settembre 2026: **153 parole, 4 domande su 4**
>
> ```text
> tests/fixtures/prose/ladder/r150.txt — meridiana, 153 parole
> (lead di https://en.wikipedia.org/wiki/Sundial, esterno alla KB)
>
>   What is a sundial?                        ->  sundial is a horological device.
>   What does a sundial consist of?           ->  Flat plate.
>   What does the gnomon cast?                ->  Broad shadow.
>   What does the shadow of the style show?   ->  time.
> ```
>
> Le quattro risposte vengono da **quattro frasi diverse** del testo, non dal
> solo incipit; il banco rifiuta i muri che contengono la parola attesa, quindi
> il 4/4 non e' regalato; la KB e' quella viva e intera; la prosa e' quella che
> Wikipedia pubblica, senza troncamenti.
>
> **Che cosa e' servito, in undici iterazioni** — e nessuna di queste era
> «conoscenza mancante»: erano tutte strade rotte, e quattro erano difetti
> introdotti dal lavoro dei giorni prima.
>
> | # | la cura |
> |---|---|
> | 1 | «What is **a** X?» non rispondeva nemmeno per `dog` (disattivata di proposito per la lettura di appartenenza, che pero' non aveva niente da elencare) |
> | 2 | la relativa ridotta e' una **seconda proposizione** sullo stesso soggetto |
> | 3 | la definizione **con la coda** («a device used to weave cloth») veniva delegata a valle, dove la coda non si legge |
> | 4 | il **passivo** insegnato come inverso dell'attivo |
> | 5 | il **passato** di un verbo di relazione chiede la stessa cosa (`past/2` era un fatto senza consumatori) |
> | 6 | ⚠ un sintagma nominale **non contiene un verbo finito** — chiuso un FATTO FALSO |
> | 7 | le superfici di **domanda**, e il classificatore che non e' la classe |
> | 8 | il tetto del nome di una classe (3 → 4 parole), e la prosa lunga senza copula |
> | 9 | l'avverbiale che apre la frase non e' il soggetto |
> | 10 | ⭐ il **soggetto con un sintagma dentro** — «the shadow of the style» |
> | 11 | ⭐ l'**inciso** che apre la frase non e' la frase (`adjunct_peel`) |
>
> E il banco stesso e' stato corretto due volte: rifiutava i muri solo dopo
> l'iterazione 5, e prima regalava un ✓ a una risposta che non c'era.

> ### ⚠ E QUELLO CHE IL TRAGUARDO **NON** DICE
>
> Il 4/4 e' su QUEL testo. Misurato subito dopo, sugli altri pioli alti:
> r080 2/4 · r090 2/4 · r110 0/4 · r120 1/3 · r130 0/3 · r140 0/3 · **r150 4/4**.
>
> E la differenza non e' la difficolta' delle frasi: e' il **paragrafo**.
>
> ```text
> «A mangrove is a shrub or tree that grows mainly in coastal saline water.»
>   letta DA SOLA          ->  Learned: mangrove is a shrub.   ✓
>   dentro il paragrafo    ->  «I don't know much about mangrove yet»
> ```
>
> La stessa frase si legge o si perde a seconda di quanto testo le sta intorno.
> Il sospetto ha gia' un nome e un TODO scritto nel motore — `canon[256]` tronca
> la prosa lunga prima del dispatch (99-registry.c, `prose_learn_lead`) — e ora
> ha anche una misura: e' il **prossimo blocco**, e vale per ogni testo lungo.
>
> Detto in modo che non si possa fraintendere: parrot0 sa comprendere UNA prosa
> di 153 parole, non ancora QUALUNQUE prosa di 150 parole.

Stato misurato dopo otto iterazioni (12 settembre, PRIMA delle iterazioni 9-11):

| piolo | testo | parole | esito |
|---|---|---|---|
| 10 | incudine | 24 | 1/3 |
| 20 | cairn | 21 | 1/3 |
| 30 | quipu | 31 | 1/3 |
| 40 | kelp | 35 | 0/3 |
| 50 | telaio | 51 | 1/3 |
| 60 | mulino a vento | 57 | 0/3 |
| 70 | pomice | 54 | 1/3 |
| 80 | savana | 82 | **2/4** |
| 90 | abaco | 89 | **2/4** |
| 100 | anfora | 100 | 0/3 |
| 110 | ossidiana | 109 | 0/4 |
| 120 | clavicembalo | 126 | 1/3 |
| 130 | basalto | 128 | 0/3 |
| 140 | mangrovia | 139 | 0/3 |
| **150** | **meridiana** | **153** | **2/4** |

Il piolo 150 risponde alla definizione **e** a una relazione presa da una frase
in mezzo al testo:

```text
What is a sundial?           ->  sundial is a horological device.
What does the gnomon cast?   ->  Broad shadow.
```

**Non e' ancora il traguardo** — comprendere una prosa di 150 parole vuol dire
rispondere a tutto cio' che dice, non a meta'. Ma e' la prima volta che una
prosa vera di quella lunghezza risponde su piu' di un fronte.

### I blocchi che restano, in ordine di quante domande sbloccano

1. **Il soggetto con un sintagma dentro.** «The shadow of the style shows the
   time» non si legge affatto: il soggetto ha un «of» in mezzo, e il lettore lo
   taglia. E' la forma piu' comune di soggetto della prosa d'enciclopedia.
2. **L'acquisizione si prende i turni di prosa.** «In the narrowest sense of the
   word, it consists of…» riceve «I looked up «narrowest» but found nothing»:
   la cessione `faculty_yield_force(learn, open, prose_carried)` e' dichiarata
   ma quel percorso non passa dal registro, quindi non la consulta.
3. **La domanda a oggetto in testa** (M5): «What else is an abacus called?».
4. **La coreferenza dentro un paragrafo**: «It is an igneous rock» dopo
   «Obsidian is…» — il pronome non arriva al soggetto della frase precedente.
5. **Il conteggio** («How many savanna forms exist?») e le relazioni dentro una
   subordinata («until largely replaced by…»).

### Giro 1 — 12 settembre 2026 (gen513): M4 e M1 chiuse

**G1 · M4 — la cessione (KB pura, zero righe di C).** Il turno di prosa non
dichiarava **nessuna forza**: `turn_declared_act($T, assertion)` pretende un
frame dichiarativo completo, e una frase vera di enciclopedia — apposizione,
coordinazione, participio — non lo lega. Cosi' il turno usciva dalla lettura
senza etichetta e la prima facolta' che sapeva dire qualcosa lo prendeva.
Aggiunta la lettura piu' DEBOLE che basta: `turn_declared_act($T, prose_carried)`
— c'e' una copula, non c'e' il punto interrogativo, non si apre con una
richiesta. Non pretende di aver capito la frase: dichiara che non e' un ordine.
Con `faculty_yield_force(gen, open, prose_carried)`.

> «Quipu, also spelled khipu, are record-keeping devices…» non riceve piu' un
> racconto inventato. Il generatore di storie e' invariato (differenziale).

**G2 · M1 — l'apposizione (la peggiore: un fatto storto, in silenzio).** La
regola che chiude uno slot su una virgola **c'era gia'** (gen505y) e non poteva
scattare: quando il legatore dei frame riceve i token, **le virgole sono gia'
state tolte in place** da un chiamante piu' a monte. Una guardia giusta e cieca —
la stessa specie delle radici morte del gen512. Il turno originale invece non e'
stato toccato: ora la virgola si chiede a `active_turn_norm`, camminando sul
turno in ordine insieme ai token.

> «Tardigrades, also known as water bears or moss piglets, are …» →
> `Learned: tardigrades also known as "water bears or moss piglets".`
> E il fatto e' RAGGIUNGIBILE: «What is another name for tardigrades?» →
> **«water bears or moss piglets.»**

**Conto KB-first, onesto.** G1: **0 righe di C**, +35 di KB — porta gia' aperta
(`faculty_yield_force`). G2: **+20 righe di C** in un punto solo, e nessuna
uscita. Non e' una migrazione: e' un ponte. La forma giusta e' che il confine
venga dall'IR come vista KB, ed e' il giro **G5** — finche' il legatore dei
frame e' in C, la sua virgola resta in C. **Debito invariato: 217.**
Sonda nuova: `P0_FRAME_TRACE=1` mostra pattern, token, virgole e slot legati —
e' quella che ha trovato che la guardia era cieca.

**Aperta in questo giro — M5, la preposizione orfana.** «What are tardigrades
also known **as**?» → *«nothing I hold says tardigrades also known as **as**»*:
l'oggetto e' in testa (e' il pronome interrogativo) e la preposizione resta
sospesa in coda, dove il lettore la prende per oggetto. Il fatto c'e' e un'altra
superficie lo trova: e' un difetto di forma della DOMANDA, non della lettura.
Da mettere in coda ai giri.

### Giro 2 — 12 settembre 2026 (gen513): rispondere *sul* testo, non solo *dal* testo

F. ha alzato il livello di verifica: «ogni iterazione deve rispondere a 20
domande mixate tra nel merito del testo e meta domande tipo di cosa parla e
anche domande di struttura come è composto il testo».

Il banco ora conta **tre colonne**, perché sono tre strade diverse e un totale
unico le mescolerebbe (13/20 non dice se il lettore ha capito il testo o se ha
soltanto saputo contarne le frasi):

| specie | che cosa chiede | da dove viene la risposta |
|---|---|---|
| **merito** | ciò che il testo dice | lettura + fatti in KB |
| **meta** | di che cosa parla | il testo come oggetto |
| **struttura** | quante frasi, come comincia | la IR del testo trattenuto |

Le risposte attese di meta e struttura **non vengono da parrot0**: le calcola
uno script indipendente dal testo. Se parrot0 dicesse un altro numero, il banco
lo deve dire — ed è successo due volte, vedi sotto.

#### La correzione di rotta: **usare la IR, non affiancarla**

La prima stesura faceva asserire al lettore composto in C una *seconda*
struttura del testo (`text_sentence/2`, `text_sentence_count/1`,
`text_word_count/1`, `text_topic/1`). F.: **«non stai lavorando usando la IR
universale»**. Era vero, ed era il difetto peggiore: conoscenza nuova nel
motore, parallela a quella che l'IR già aveva. Quei fatti sono spariti.

Per arrivarci l'IR andava riparata in tre punti, e sono **tre difetti veri**:

1. **Gli id erano della chiamata, non dello scope.** Ogni
   `input_structure_publish` ripartiva da `0`, e un turno di più frasi pubblica
   una volta per frase nello stesso scope: `input_node(current_turn, 0, …)`
   aveva una soluzione per frase, e **ogni join per id era un prodotto
   cartesiano fra frasi diverse**. L'IR universale era illeggibile proprio
   sulla prosa lunga.
2. **Il testo non restava.** «Di che cosa parlava?» arriva *dopo* la prosa, e a
   quel punto `current_turn` descrive la domanda: «il testo ha 1 frase e 7
   parole» — la domanda stessa. Ora la IR di un testo letto resta in
   `last_text`; che cosa *meriti* di restare lo dice la KB (`turn_is_text/1`).
3. **Le frasi non finivano nella IR.** Il lettore composto aveva già in mano la
   segmentazione vera e la teneva per sé; `turn_publish` pubblica gli *span*,
   che non sono le frasi. «Da quante frasi è composto?» diceva **1**.

#### I tre tetti muti che mangiavano la prosa vera

- **`MAX_CLAUSES = 8`.** Al nono confine il ciclo usciva e **il resto del turno
  non lo leggeva nessuno**: nessun muro, nessuna traccia. Su 500 parole (~25
  frasi) due terzi del testo sparivano, e la scala misurava la comprensione di
  un terzo di testo credendo di misurarla tutta. Ora è `turn_max_clauses/1`.
- **`span_atom/2` è `chars/2` andata e ritorno**: la stringa passa per una
  lista di caratteri dentro un termine da 512 byte, e oltre ~50 caratteri
  fallisce **in silenzio**. Era il motivo per cui «come comincia?» rispondeva
  sulla prosa corta e murava su quella vera.
- **`list_len/2` è ricorsiva** e il motore si ferma a `KB_MAX_DEPTH` (64):
  contare 300 parole non arrivava in fondo.

#### Due difetti della capacità nuova, trovati dai pioli

- **Il tema murava su metà dei testi veri** (piolo 320). Si leggeva dal
  sintagma che apre il testo, e un sintagma lo delimita un determinante: «A
  coral reef is…» sì, «Compost is a mixture…» no. Terzo strato: la parola con
  cui il testo comincia, che in un lead è il definiendum.
- **«Quante parole?» rispondeva 311 su 299** (piolo 320). Il flusso di token
  spezza dove la lingua non spezza («0.1%», e la normalizzazione stacca «20%»
  in «20 %») perché quel flusso serve a *leggere*. Chi chiede quante parole ci
  sono intende le parole: ora si pubblica un flusso di **parole**, sul turno
  come l'ha scritto l'interlocutore, e il separatore è un fatto
  (`word_separator/1`).

#### ✅ Il difetto che teneva ferma la scala da tre giri (piolo 340)

> «Charcoal is a lightweight black residue made of carbon.» → **«Steel.»**
> — e la definizione non entrava affatto.

Non era un errore di lettura, era un errore di **turno**: lo schema
«@O is made of @S» combacia, lo slot @O prende «charcoal is a lightweight black
residue», la guardia lo rifiuta giustamente (verbo finito, mantra #7) — ma a
quel punto il turno era già di chi **risponde alle domande**.
`turn_declared_act(assertion)` non aiuta: nasce da `turn_reading`, e questa
frase una lettura non ce l'ha. L'evidenza che non è una domanda è *precedente*
a qualunque lettura:

```prolog
turn_declared_act($T, unasked) :- turn_prose_copula($T),
    naf(turn_has_question_mark($T)), naf(turn_opens_question($T)),
    naf(turn_opens_request($T)).
faculty_yield_force(answer_frame, open, unasked).
```

Adesso la stessa frase lascia **due** fatti invece di zero: definizione *e*
modificatore convivono. Era il conflitto che aveva fatto ritirare la lezione
«characterized by» al giro scorso.

#### La crescita laterale della KB (F.: «un obbiettivo che dobbiamo sempre avere»)

**+138 verbi di relazione**, insegnati parlando e salvati con `/save`, e
**15 particelle** (`relation_particle/2`) — appartiene *a*, consiste *di*,
deriva *da*, cresce *in*: una delle forme più comuni della prosa
d'enciclopedia, e non ne esisteva il lettore. Una riga per verbo, non il
prodotto cartesiano con tutte le preposizioni: gli schemi si scorrono tutti a
ogni turno (gen459).

#### ⛔ I blocchi che restano, riordinati per quante domande sbloccano

1. **La relazione dentro una subordinata.** «…because composting reduces
   methane emissions due to…», «…called charcoal burning, often by forming a
   charcoal kiln, the heat is supplied by…». È il caso dominante sui pioli
   340 e oltre: dodici domande su tredici.
2. **Il passivo.** «Most coral reefs **are built from** stony corals» →
   risposta confidente e **sbagliata** («Colonies.», presa dalla frase prima).
   Peggio di un muro.
3. **Il qualificatore della domanda ignorato.** «what **phylum** does coral
   belong to?» → «Class anthozoa.» Giusto il verbo, sbagliato il valore.
4. **Il soggetto coordinato.** «Aerobic bacteria **and** fungi manage…» — lo
   stesso difetto che tiene r508 a zero.
5. **Il participio in testa** («Sometimes called rainforests of the sea, …»)
   e **con agente** («held together **by** calcium carbonate»).
6. **L'anafora fra frasi** («**They** occupy less than 0.1%…»).
7. **Le forme di domanda non definitorie**: «where do X grow best?», «when
   did X first appear?», «how much of Y…?».
8. **La particella di due parole**: «break **down into**» —
   `relation_particle/2` ne regge una sola.

Le due **risposte confidenti e sbagliate** (2 e 3) vanno prima di tutto il
resto: un muro si conta, una bugia no.

#### La scala 300 → 500, misurata per intero (12 settembre 2026)

| piolo | testo | parole | merito | meta | struttura | tot |
|---|---|---|---|---|---|---|
| 300 | Coral reef | 299 | 6/13 | 2/2 | 5/5 | **13/20** |
| 320 | Compost | 319 | 3/13 | 2/2 | 5/5 | **10/20** |
| 340 | Charcoal | 338 | 1/13 | 2/2 | 5/5 | **8/20** |
| 360 | Magnet | 356 | 2/13 | 2/2 | 5/5 | **9/20** |
| 380 | Sugar | 374 | 3/13 | 2/2 | 5/5 | **10/20** |
| 400 | Concrete | 395 | 1/13 | 2/2 | 5/5 | **8/20** |
| 420 | Erosion | 427 | 1/13 | 2/2 | 5/5 | **8/20** |
| 440 | Violin | 440 | 1/13 | 2/2 | 5/5 | **8/20** |
| 460 | Clock | 464 | 1/13 | 2/2 | 5/5 | **8/20** |
| 480 | Thunderstorm | 485 | 2/13 | 2/2 | 5/5 | **9/20** |

**Le due colonne nuove sono chiuse su tutta la scala: meta 2/2 e struttura 5/5,
da 300 a 500 parole.** parrot0 sa dire di che cosa parla un testo che non
conosce, da quante frasi e quante parole è fatto, come comincia e come finisce
— e lo sa leggendo la IR, non un riassunto che qualcuno gli ha messo accanto.

**E la lunghezza non è più la variabile.** Il merito non scende salendo la
scala: 300 fa 6/13 e 440 ne fa 1/13, ma non perché il testo sia più lungo —
perché è scritto in un modo che il lettore non attraversa. I tre tetti muti del
giro 2 (`MAX_CLAUSES`, `span_atom`, `list_len`) erano la lunghezza, e sono
chiusi. Quello che resta è la **forma**: subordinate incassate, apposizioni,
soggetti coordinati, participi. È una notizia buona travestita da numero basso:
si sa che cosa misurare adesso.

⚠ Nessun piolo ha segnalato domande «già rispondibili a freddo»: dopo la
rimozione di `made_of(reefs, colonies)` non resta contaminazione, e il passo di
calibrazione lo dirà da solo se ne rientrasse.
