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

## 4-ter. I VINCOLI DI VALIDAZIONE DEL BANCO (F., 12–18 settembre 2026)

Sono le regole con cui un piolo della scala si dice **compreso**. Stavano
sparse fra gli handoff di `LEARN_TODO.md` e la testa di `scripts/prose-probe.sh`;
qui sono il contratto, e il banco le stampa tutte.

1. **Tre colonne, mai un totale unico.** *Merito* (ciò che il testo dice),
   *meta* (di che cosa parla), *struttura* (quante frasi, come comincia e come
   finisce). Un 14/20 non dice se il lettore ha capito il testo o se ha solo
   saputo contarne le frasi; le attese di meta e struttura le calcola uno script
   indipendente, non parrot0.

2. **⛔ Il cancello (F., 12 settembre).** *«Ogni iterazione dovrà rispondere con
   successo a un numero di domande tale che il conteggio delle parole con cui
   sono scritte le domande a cui ha successo sia più lungo del numero di parole
   del testo stesso; man mano che cresce il testo crescono le domande a cui,
   avendo successo, parrot0 risponde.»* Cioè: **Σ parole delle domande nel
   merito risolte > parole del testo.** È dura e onesta per costruzione: non
   premia un banco corto, non premia una domanda facile ripetuta, e cresce per
   forza col testo — un testo di 500 parole esige più comprensione di uno di
   300, non la stessa percentuale.

3. **⛔ Il banco deve POTER passare il cancello (F., 18 settembre).** Un banco
   le cui domande nel merito, *tutte* risolte, non arrivano alle parole del
   testo non misura la comprensione: misura la propria taglia. Quindi **Σ
   parole di TUTTE le domande nel merito > parole del testo, con margine**: la
   soglia è `P0_BENCH_MARGIN` (default **1,25**), il banco la stampa prima dei
   conti e dichiara `BANCO INSUFFICIENTE` o `BANCO STRETTO` quando non regge.
   Senza margine il cancello coincide con l'obiettivo del 100% (il piolo 300 a
   50 domande, 310 parole, passava solo a 49/50) invece di essere un gradino
   verso di esso.

   | piolo | testo | domande merito | Σ parole | margine 1,25 |
   |---|---|---|---|---|
   | r300 | 299 | 50 → **62** | 310 → **417** | 374 ✓ |
   | r320 | 319 | 13 → **68** | 68 → **435** | 399 ✓ |
   | r340 | 338 | 13 → **65** | 72 → **450** | 423 ✓ |
   | r356 | 356 | 13 → **68** | 74 → **510** | 445 ✓ |
   | r374 | 374 | 13 → **72** | 63 → **488** | 468 ✓ |
   | r395 | 395 | 13 → **72** | 67 → **517** | 494 ✓ |
   | r420 | 427 | 13 → **80** | 69 → **571** | 534 ✓ |
   | r440 | 440 | 13 → **80** | 84 → **584** | 550 ✓ |
   | r464 | 464 | 13 → **85** | 71 → **601** | 580 ✓ |
   | r485 | 485 | 13 → **95** | 71 → **666** | 607 ✓ |
   | r497 | 497 | 13 → **87** | 72 → **651** | 622 ✓ |
   | r010 … r250, r362, r508, r621 | 21–621 | 1–4 | 4–23 | ⛔ banchi diagnostici, senza cancello |

   (Stato al 18 settembre 2026. I pioli sotto le 300 parole sono nati come
   scala «una prosa per piolo, tre domande» prima del cancello: restano
   diagnostici finché non vengono estesi.)

4. **Il banco è FISSO, e il successo tende al 100%.** *«parrot0 deve saper
   rispondere a ogni domanda rispondibile sulla prosa; la stima non implica
   crescere il set di domande ma migliorare la comprensione»* (F., 12
   settembre). **Allargare il banco per abbassare il tasso richiesto è barare;
   allargare un banco che non poteva passare il cancello non lo è**: il tasso
   si calcola su tutte le domande, e le nuove sono altrettante da rispondere.
   Le domande vecchie restano identiche e in testa al file, così i referti
   restano confrontabili sul sottoinsieme storico.

5. **La calibrazione a freddo.** Le stesse domande, in una sessione pulita,
   *senza* la prosa: ciò che riceve risposta lì non è lettura ed esce dal conto
   (`già`). Il banco resta onesto per costruzione anche quando la KB cresce di
   sotto — è successo due volte (`made_of(reefs, colonies)`,
   `located_in(satellite, orbit)` depositati da un `/save`).

6. **Un muro non è mai una risposta**, nemmeno quando contiene la parola
   attesa; e **una risposta falsa conta più di un muro** e si dichiara a parte
   (regola 5 del §4). La revisione a mano delle ✓ è parte della misura
   (mantra #9): il runner non vede una risposta sbagliata che contiene la
   parola attesa.

7. **Prosa vera, esterna alla KB, con la fonte** (`ladder/SOURCES.md`), intera e
   coerente, mai troncata. Nessun fatto del brano si persiste con `/save`
   durante un giro: W=0, L=0.

---

## 5. I giri, in ordine, con la prova di chiusura

| giro | malattia | cura prevista | prova |
|---|---|---|---|
| ~~G1~~ ✅ | **M4** cessione | condotta KB: una facoltà che PRODUCE prosa cede un turno che PORTA prosa (`faculty_yield_force`) | «Quipu, also spelled khipu, …» non riceve più un racconto |
| ~~G2~~ ✅ | **M1** apposizione | il valore chiede all'IR dove finisce il suo nodo (`np_closer`, le virgole come confine) | la frase 1 lascia in KB i fatti giusti, non uno storto |
| ~~G3~~ ✅ | **M2** ordinale | «first» dentro un avverbiale non è la domanda del turno | «when did coral reefs first appear?» → «485 million years ago» (piolo 300, 13 settembre) |
| ~~G4~~ ✅ | **M3** relativa | «which/who» aprono una relativa sul nodo precedente, non una lezione (`relative_opener/1`, antecedente dall'IR) | «what does the phylum cnidaria include?» → «sea anemones» (piolo 300) |
| **G5** | consumo | portare UN lettore grosso a consumare l'IR invece di `split_words` | il conteggio 217 scende, e il banco non peggiora |
| ~~G6~~ ✅ | **M5** preposizione orfana | «What are X also known **as**?» — l'oggetto e' in testa, la preposizione resta in coda | «what are shallow coral reefs sometimes called?» → «rainforests of the sea» (piolo 300) |
| **G7** | furto di turno | un lettore nuovo di un'altra missione si prende una frase di prosa e dice un fatto storto (18 settembre: E3 e «from») | ogni piolo certificato si rimisura dopo ogni missione che tocca la lettura; il `.p0t` del piolo porta il contrasto |

**Ogni riga porta anche il suo conto KB-first**: quale porta KB è stata usata (o
aperta), quanti `split_words` restano, quante righe di C sono uscite.

Dopo G1-G4 il referto del tardigrade deve passare da 0/7 a qualcosa; il numero
esatto non si promette, si misura.

---

## 6. Registro dei giri

### 18 settembre 2026 — la regressione di tre giorni, il banco che può passare il cancello

**Misurato prima di toccare qualcosa.** Il piolo 300, certificato **49/50 con
cancello passato** la notte del 13 (vedi sotto), ritorna oggi a **45/50**,
cancello riaperto (274 < 299), sulla stessa KB viva più tre giorni di altre
missioni (mix di capacità, interlocutore di frontiera E1–E4, insegnamento
super-umano: +833 righe in `grammar.p0`, +978 in `10-memory-knowledge.c`,
+1005 in `99-registry.c`). Le quattro domande perse:

```text
Most coral reefs are built from stony corals, whose polyps cluster in groups.
  → «Noted: The built is from stony.»           (13 settembre: Learned … stony corals)
Coral reefs are under threat from excess nutrients (nitrogen and phosphorus), …
  → «Noted: The threat is from excess.»         (13 settembre: Learned … threaten)
```

Non un muro: **un fatto storto detto con sicurezza** — la specie peggiore
(§4, regola 5). Il ladro è la lettura E3 dell'interlocutore di frontiera
(`kb/core/event-time.p0`): «X from Y = l'origine di X» scattava su qualunque
«from» di qualunque turno, e il turno di prosa diventava un evento descritto
con il suo `turn_response`. È la forma del mantra #21: un modulo **maturo**
(tutto in KB, addestrabile) che ruba per una lettura troppo larga — quindi
**si insegna**, non si retrocede.

**La cura, KB pura (0 righe di C).** Alla lettura mancava la dimensione *che
cosa può avere un'origine* (mantra #23): un evento con orario, il viaggio, o
la controparte letta nello stesso turno («a client from Milan»).
`ev_origin_bearer/2`, tre righe; un portatore nuovo è una riga. Contrasto nel
banco `tests/p0t/language/prose_triage.p0t` (le due frasi: mai «Noted», e le
quattro domande rispondono); E3 intatto (`frontier_transcript.p0t` 21/21,
`origo.p0t` 20/20; `prose_triage.p0t` 80/80).

**Il banco, esteso perché possa passare il cancello** (§4-ter, regola 3):
r300 50→62 domande nel merito (310→417 parole), r320 13→68 (68→435), r340
13→65 (72→450), r356 13→68 (74→510), r374 13→72 (63→488), r395 13→72
(67→517), r420 13→80 (69→571), r440 13→80 (84→584), r464 13→85 (71→601),
r485 13→95 (71→666), r497 13→87 (72→651): **+697 domande nel merito**, tutte
con la risposta scritta nel testo. Le domande storiche restano identiche in
testa ai file. Il banco
stampa da oggi la propria raggiungibilità (`P0_BENCH_MARGIN`, 1,25).

**Misure dopo la cura** (referti in
`docs/labs/apprendimento-assistito/2026-09-18-regressione-e-banco/`):

| piolo | storiche | nuove | merito | meta | struttura | cancello |
|---|---|---|---|---|---|---|
| r300 | **49/50** (come il 13) | 1/12 | **50/62** | 2/2 | 5/5 | ✅ **312 > 299** |
| r320 | **12/13** (come il 13) | 7/55 | **19/68** | 2/2 | 5/5 | ⛔ 105/319 |
| r340 | 3/13 (era 1/13 il 12) | 1/52 | **4/65** | 2/2 | 5/5 | ⛔ 19/338 |

Le domande storiche tornano dove erano: **la regressione è chiusa** e il
cancello di r300 è di nuovo passato. Le domande nuove dicono il resto, ed è
la ragione per cui il banco andava esteso: sul piolo 300 ne passa una su
dodici, sul 320 sette su cinquantacinque. Le forme che le tengono aperte, in
ordine di frequenza sui due pioli:

1. **Il qualificatore ignorato** — due risposte confidenti e sbagliate, quindi
   prima di tutto: «what was the economic value of coral reefs estimated at
   **in 2020**?» → «anywhere from US$30–375 billion» (la cifra del 1997);
   «what area do coral reefs occupy **about half of**?» → «less than 0.1 percent
   of the world's ocean area». La relazione è giusta, il vincolo della domanda
   non viene provato.
2. **Il turno rubato da un'altra facoltà**, due misclaim: «how much of the
   waste in landfills…?» → un elenco di salvataggio di rame e magneti (una
   procedura); «what is added to the plant matter…?» → la definizione di
   *matter* dal mondo. Mantra #21: prima classificare se il ladro è maturo.
3. **La domanda «what kind of / in what kind of»** sull'aggettivo o sul
   modificatore («in what kind of water», «what kind of process», «what kind
   of reclamation», «in what kind of farming»): manca il lessico degli
   aggettivi come valore, già l'unica aperta del 13.
4. **Il sostantivo composto della domanda che non trova la chiave** («brown
   waste», «brown materials», «compost rich», «turned regularly»): la domanda
   costruisce un nome di più parole che la lettura non ha lasciato.
5. **La parentetica come definizione** («green waste (nitrogen-rich
   materials such as …)», «brown waste (woody materials …)»): la parentesi
   dopo un nome dice che cos'è, e oggi si legge solo come esempi.
6. **«What did X displace / lead to / aim to maintain»**, «at the dawn of
   which period», «where do … exist on smaller scales»: verbi letti ma non
   interrogati da quella forma, e avverbiali di tempo dentro un inciso.

**Il piolo 340 (Charcoal), prima misura con queste classi: 4/65.** Il
trasferimento che r320 aveva mostrato (4/13 a freddo) qui quasi non c'è: le
frasi del carbone sono relative ridotte lunghe («a lightweight black residue
made of carbon that is produced by strongly heating wood (or other …) in
minimal oxygen to remove …»), participi con agente, «involves + gerundio»,
«led to», «aimed to maintain», e **sei risposte confidenti e sbagliate** su
52 domande nuove, tutte furti di turno di facoltà che non leggono la prosa:
il generatore di saggi causali («On charcoal used as in chemical, a causal
account turns on…»), la definizione del mondo di *temperature* e del *carbon
cycle*, «Ask me whether something holds first», e «in which regions did
charcoal production contribute to deforestation?» → «america, africa» (la
frase sbagliata: quella è la produzione illegale; il testo dice Central
Europe). È la classe 2 dell'elenco, e sul piolo nuovo è la più numerosa:
**prima di ogni cura di lettura, i furti** (mantra #21) — un muro onesto vale,
una bugia no. Referto: `r340-prima-misura.txt`.


Conto KB-first: C **0 righe**; KB +14 (event-time.p0), banco +1 caso.
`split_words` invariato: 132/54/33 = **219**.

### 13 settembre 2026, notte — piolo 300 da 17/50 a **49/50 = 98%, cancello 301 > 299**; piolo 320 da 4 a 12/13

> ⚠ Questo registro era rimasto fermo a 17/50: i gradini della notte del 13
> stanno negli handoff di `LEARN_TODO.md` («HANDOFF 2026-09-13 (gen514,
> notte)», cinque voci) e nei referti di
> `docs/labs/apprendimento-assistito/2026-09-13-direzione-della-domanda/`
> (`bench-27 … bench-49-cancello.txt`) e `…/2026-09-13-piolo-320/`. Qui il
> riassunto, perché il piano vivo deve dire dove si è.

| gradino | merito | cancello | che cosa ha aperto la strada |
|---|---|---|---|
| triage delle 33 aperte | 17 → 19 | 107 | due fortunate e quattro bugie chiuse |
| attenuazione, avverbi del verbo, relative possessive e con `that`, coordinati, domande locative | 19 → 27 | 156 | `attenuating_quantifier`, `verb_adverb`, `bare_relative_opener`, `relative_clause_verb` |
| gli esempi di «including», costruzioni a ruoli invertiti interrogabili | 27 → 30 | 184 | `participial_opener/2`, `frame_role_order/2` |
| «what is X?» dice la classe o ciò che ha letto, e solo su X | 30 → 35 | 209 | la lettura citata («I have no definition of it, but I read: …») |
| costruzione con copula chiesta senza copula; participio anteposto | 35 → 37 | — | `fronted_participle/1` |
| perfetto con particella, subordinatore in coda, predicati aggettivali | 37 → 40 | — | `perfect_auxiliary`, `trailing_subordinator`, `subordinator_modifier`, `adjective_relation` |
| la parentetica misurata; «since when» | 40 → 41 | 248 | `parenthetical_relation/1`, `particle_question_word/2` |
| il tipo chiesto decide fra due descrizioni; alternanza di voce; scopo dell'agente; parentetiche di annotazione; nomi d'attributo partitivi; valuta; complemento con particella dopo l'oggetto; ranghi tassonomici e catena; composti agentivi | 41 → **49** | **301 ✅** | `active_agent_surface`, `purpose_by_agency`, `attribute_noun`, `value_relation`, `particle_surface_for`, `rank_noun`, `membership_chain` |

Revisione a mano delle 49: nessuna risposta falsa; deboli «Warm.» (elenco di
aggettivi troncato) e «coral reefs first.» (resa). **L'unica aperta:** «in what
kind of water do reefs grow best?» — manca un lessico degli aggettivi per
tenere «warm, shallow, clear, sunny, and agitated water» come un solo valore.

**Piolo 320 (Compost), subito dopo:** al primo passaggio, senza toccare niente,
4/13 — il trasferimento c'è. Poi 4 → 7 → 11 → **12/13**, ogni gradino
riverificato su r300 (sempre 49/50). Cure: `includes` canonico, «such as» come
esempi, «because» anche senza virgola e «since» solo con, un turno inglese non
traduce le sue parole (`content_translation_source/1`), soggetti coordinati
distribuiti, `verb_particle(break, up)` e `(use, as)`, la ridotta dopo un
predicato nominale, nomi relazionali con articolo, `np_opener(those/these)`.
Aperta: «what can compost be used for?» (il modale con «be» e `use for`).
Il banco a 13 domande (68 parole) non poteva passare il cancello delle 319:
è il reperto da cui nasce la regola 3 del §4-ter.

Trappole pagate (valgono per il prossimo): `naf` su goal non ground (due
volte), `snprintf` su se stesso, un «of» partitivo preso per particella
(bugia chiusa prima del commit).

### 13 settembre 2026 — partitivo e verifica delle definizioni: 17/50

Baseline confermata 16/50, risultato **17/50**, cancello **97/299**, meta 2/2
e struttura 5/5. `some of ... ecosystems` resta intero; una domanda con
preposizione in coda non riceve piu' una semplice definizione. Il partitivo
riusa i due consumatori esistenti; la verifica definitoria consuma i token IR.
Sonda runtime: **31/31**. Soft-test ancora rosso per timeout di `basics.p0t`.
C +33/-45 (in buona parte commenti), KB +26, `split_words` invariato a
132/54/33 = **219**. Nessuna risposta del banco salvata nella KB.
[Report, limiti e prossimo circuito](../labs/apprendimento-assistito/2026-09-13-prosa-partitivi.md).


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
