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
