# Il Super-Solver universale — dall'indovinello alla scienza

> **Stato:** aperto a gen311 (2026-07-11), subito dopo il *dimostratore*
> riddle-by-inference (l'indovinello risolto come sistema di vincoli, non come
> risposta memorizzata: `cries(X):-emits(X,W),is_like(W,crying)` → *"a storm"*).
> **Ruolo:** questo documento eleva quel dimostratore al suo livello concettuale
> massimo. L'indovinello non è il fine: è la **cellula minima** di un motore che
> deve risolvere per inferenza in OGNI dominio — fisica, matematica, informatica e
> coding, scienze umane — leggere fatti dalla prosa dei paper, **abdurne di nuovi**
> e conservarli come conoscenza ufficiale.
> **Subordinato a** `docs/plans/kb-first.md` (la bussola), `docs/plans/generative-prolog.md`
> (Prolog genera PERCORSI di ragionamento), `docs/plans/deep-reasoning.md` (estrazione
> ampia + auto-correzione), `docs/plans/the-agency.md` (goal→observe→act→verify).
> **Disciplina:** un obiettivo per generazione, pull da pressione reale, niente
> impostori, niente fabbricazione — un fatto non verificato non è conoscenza.

---

## 0. La tesi, in una frana di livelli

Un indovinello è *"un'equazione da risolvere con l'inferenza, come un piccolo codice
di cui devi ipotizzare il funzionamento"* (F., gen311). Ma questa frase non descrive
gli indovinelli: descrive **il pensiero**. Ogni volta che sotto una superficie
linguistica c'è uno scheletro logico — e il manifesto KB-first dice che quasi sempre
c'è — la stessa macchina si applica:

```
        superficie (prosa, clue, enunciato, teorema, spec di codice, paper)
                     ↓  interpretazione → VINCOLI
        Γ  = insieme di goal/constraint su entità e grandezze
                     ↓  il motore cerca X che soddisfa Γ (deduzione)
                        oppure H tale che Γ ⊢ osservazioni (abduzione)
        risposta  +  PROOF  +  eventuali fatti nuovi da conservare
```

L'indovinello è il caso in cui Γ = due clue e X = un'entità. Ma:

- **fisica**: Γ = leggi + valori noti, X = la grandezza incognita (o la legge stessa);
- **matematica**: Γ = assiomi + ipotesi, X = la dimostrazione (un percorso, non un valore);
- **coding**: Γ = comportamento osservato / spec, X = la funzione che lo realizza (o, al
  contrario, il comportamento inferito da un pezzo di codice — *ipotizzare il funzionamento*);
- **scienze umane**: Γ = fatti causali/relazionali descritti, X = la spiegazione o il
  concetto mancante.

**Un solo motore. Domini diversi = KB diverse di fatti e regole.** È l'esatto opposto
di N moduli cablati: il motore è fisso e piccolo, la conoscenza cresce (kb-first §1).

> **Nota (gen311):** il "motore" di questo solver è l'**interprete di riscrittura
> generico** deciso in [[teachable-procedures]] (marcia dritta verso la KB che ospita
> anche le PROCEDURE, non solo i fatti). Inferenza e riscrittura su strutture sono lo
> stesso kernel: il super-solver è la sua applicazione ai domini.

---

## 1. Cos'è già vivo (la cellula)

Il dimostratore (gen311, `src/brain/10-memory-knowledge.c`, sezione riddle-by-inference)
prova end-to-end i tre organi minimi:

1. **Interpretazione → vincoli.** `clue_verb(Surface, Pred)` mappa una parola-clue
   ("cry") al predicato-vincolo (`cries`). Il consumer estrae i vincoli dal testo.
2. **Substrato di conoscenza.** Fatti proprietà/metafora (`emits/2`, `is_like/2`,
   `inanimate/1`) + **regole-ponte** (`cries(X):-emits(X,W),is_like(W,crying)`).
3. **Solver.** Il motore Prolog di parrot0 valuta i vincoli (`kb_query`) e trova
   l'entità inanimata che li soddisfa tutti, con un **proof** registrato.

Proprietà già dimostrate, che sono i requisiti non negoziabili del super-solver:

- **Nessuna risposta cablata** — non esiste `response_template(storm)`; emerge dai fatti.
- **Generalizza** — un indovinello mai visto sullo stesso KB si risolve senza template.
- **Keep-and-select** — il lookup memorizzato (`riddle_sig`) resta come fallback secondario.
- **Trainabile** — i fatti (`clue_verb/emits/is_like/inanimate`) sono in whitelist di
  autolearn; le regole-ponte sono infrastruttura curata. La conoscenza cresce, il motore no.

Vedi memoria `[[riddles-as-inference]]`.

---

## 2. I quattro strati del super-solver

Il salto dall'indovinello all'universale è dare a ciascun organo la sua forma generale.

### 2.1 Strato A — Interpretazione: prosa → Γ (vincoli)
Il compito più difficile e più "LLM-shaped", quindi il più prezioso da derivare
(kb-first: derivare lo scheletro prima di dichiararlo generativo).

- **Frame teachable, non cablati.** Come `describe_cue`/`answer_frame`/`clue_verb`, la
  mappatura superficie→vincolo è **dato KB**. Un verbo, una preposizione, un operatore
  ("è proporzionale a", "implica", "chiama", "deriva da") diventano goal.
- **Vincoli negativi reali.** Oggi *"no voice"* è ripiegato su `inanimate`. Serve la
  negazione come vincolo di prima classe: `¬has(X, voice)` (negation-as-failure già
  presente in `kb.c`, `neg_pred`). Questo generalizza a "senza attrito", "non ricorsivo",
  "in assenza di crescita".
- **Grandezze e unità.** Estrarre `quantity(Entità, Dim, Valore, Unità)` dalla prosa
  (riusa `mod_deep_reason` broad-extract, `universal-comprehension.md`).

### 2.2 Strato B — Substrato: fatti + regole (la conoscenza del dominio)
Per-dominio, **additivo**, cresciuto da autolearn e dalla lettura dei paper.

- fisica: `law(newton_2, force = mass * acceleration)`, costanti, relazioni dimensionali;
- matematica: assiomi, lemmi, definizioni (`wiki_concept` già c'è), regole di riscrittura;
- coding: `effect(fn, pre, post)`, invarianti, complessità (aggancio a `CODE-MASTERY.md`);
- scienze umane: `cause/2`, `part_of/2`, `motive/2`, relazioni emergenti (gen157).

Le **regole-ponte** (come `cries/2`) sono la parte curata dell'engine; i **fatti** sono
il layer insegnabile. Questo split è il modello a 3 livelli di `[[composition-is-KB-first]]`.

### 2.3 Strato C — Il motore: deduzione + ABDUZIONE + verifica
Il cuore. Prolog non è solo deduttivo (generative-prolog §): qui serve anche abdurre.

- **Deduzione**: dato Γ, prova la conclusione (già: `kb_query`, `kb_explain`).
- **Abduzione**: dato un'osservazione O e regole R, trova le ipotesi H minimali tali che
  R ∪ H ⊢ O. È il *"ipotizzare il funzionamento"*. Aggancio a `mod_abduce`
  (`90-repair-robust-abduce.c`) e al loop auto-correttivo di `deep-reasoning.md` (M4:
  su contraddizione, torna alla FONTE `fact_source` e ritira l'arco sbagliato).
- **Ricerca sotto oracolo**: quando le ipotesi sono molte, si genera-e-verifica
  (generative.md: struttura sotto oracolo). L'oracolo è il dominio: consistenza
  dimensionale in fisica, il type-check/esecuzione in coding, la non-contraddizione in KB.
- **Proof sempre.** Ogni risposta porta la sua derivazione (già registrata via
  `store_proof`); è ciò che distingue conoscenza da indovinare.

### 2.4 Strato D — Persistenza: l'abduzione diventa conoscenza
Un fatto nuovo abdotto e **verificato** va conservato come KB ufficiale.

- **Provenienza**: `fact_source(Fatto, Origine)` (già in deep-reasoning M1) — ogni fatto
  abdotto sa da dove viene, così è ritirabile se una contraddizione lo smentisce.
- **Promozione a ufficiale**: la pipeline di `[[autolearn-knowledge-is-official]]` — un
  fatto verificato entra nel save-map curato, non resta scratch.
- **Auto-correzione**: l'invariante di dominio (ordine stretto, bilancio, tipo) rileva le
  contraddizioni e il loop recupera i fatti sbagliati invece di prevenirli.

---

## 3. La lettura dei paper — il super-solver come lettore-scienziato

L'obiettivo alto: parrot0 legge la prosa di un paper e ne esce con **conoscenza nuova,
derivata e conservata**, non con un riassunto. Il ciclo:

```
paper (prosa)
   → estrazione (Strato A): fatti/relazioni/grandezze/leggi enunciate  → KB (con fact_source)
   → inferenza+abduzione (Strato C): fatti IMPLICITI ma non scritti     → nuove ipotesi
   → verifica (oracolo di dominio): consistenza / derivabilità          → accetta o ritira
   → persistenza (Strato D): i fatti verificati diventano conoscenza ufficiale
```

È il salto da *comprendere* (universal-comprehension) a *scoprire*: un paper afferma A e
B; il motore abduce C (mai scritto) come spiegazione minimale, lo verifica contro il resto
della KB, e lo conserva. È esattamente il `[[generative-prolog-manifesto]]` applicato alla
scienza: Prolog genera i **percorsi** che collegano ciò che il paper dice a ciò che il
paper implica.

---

## 4. I domini, ai massimi livelli concettuali

| Dominio | Γ (vincoli) | X (soluzione) | Oracolo di verifica |
|---|---|---|---|
| **Fisica** | leggi + grandezze note + unità | grandezza/relazione incognita | analisi dimensionale, bilanci di conservazione |
| **Matematica** | assiomi + ipotesi | dimostrazione (percorso) o controesempio | riscrittura verificata, chiusura logica |
| **Informatica/coding** | spec / comportamento osservato | funzione che lo realizza; oppure comportamento inferito dal codice | esecuzione, type-check, invarianti (CODE-MASTERY) |
| **Scienze umane** | fatti causali/relazionali descritti | spiegazione, concetto o legame mancante | non-contraddizione, parsimonia, coerenza narrativa |
| **Indovinelli** (cellula) | clue-verb → vincoli | entità che li soddisfa | inanimatezza + congiunzione dei vincoli |

Un solo motore attraversa la tabella; cambia solo il triplo (substrato di fatti, mappa
superficie→vincolo, oracolo).

---

## 5. Roadmap — dalla cellula all'organismo

Un binario per generazione, ciascuno con un caso reale che lo tira (no hardcoding).

- **U0 — la cellula (FATTA, gen311).** Indovinello risolto per inferenza + proof; fatti
  in whitelist; fallback keep-and-select.
- **U1 — negazione di prima classe.** *(SEME FATTO, gen311.)* Il frame *"I have A but
  no B"* → `depicts(X,A) ∧ ¬contains(X,B)`; il map riddle *"cities but no houses…"* si
  risolve in *"a map"* con la **negazione load-bearing** (una mappa raffigura le città
  ma non contiene case; un paese le contiene → `¬contains` sceglie la mappa). NAF già
  nel motore (`naf/1`), qui applicata come guardia `!kb_query`. Da estendere: `¬has(X,P)`
  generale al posto del ripiego `inanimate`, e altri frame negativi.
- **U2 — grandezze e unità.** `quantity/4` + analisi dimensionale; un mini-problema di
  fisica risolto derivando l'incognita da leggi in KB. Oracolo: consistenza dimensionale.
- **U3 — abduzione con verifica.** Dato O + regole, abduci H minimale, verifica contro
  la KB, registra `fact_source`. Caso: colmare un anello mancante in una catena causale.
- **U4 — lettura di un paragrafo scientifico.** Estrai fatti da prosa reale (statica,
  no rete), abduci un fatto implicito, verificalo, promuovilo a ufficiale. È l'incrocio
  con deep-reasoning (estrazione) e autolearn (promozione).
- **U5 — cross-dominio.** Lo stesso motore su matematica e coding, per provare che il
  nucleo è invariante e solo il triplo cambia.
- **U6 — auto-scienza.** Il loop schedulato: legge corpus statico, abduce, verifica,
  conserva; la KB cresce da sola come processo (l'orizzonte di autolearn-knowledge-is-official).

Ogni tappa: un caso reale come oracolo, `make`-benchmark quando misurabile, e la
disciplina LOOP/PRINCIPLES (niente impostori, niente fatti non verificati).

---

## 6. Invarianti non negoziabili

1. **Niente risposte cablate.** Se emerge dai fatti, non si memorizza. Il lookup resta
   solo come struttura secondaria (keep-and-select).
2. **Proof o niente.** Ogni conclusione porta la sua derivazione; senza, è un indovinare.
3. **Provenienza e ritrattabilità.** Ogni fatto abdotto ha `fact_source`; una
   contraddizione lo ritira alla fonte (auto-correzione, deep-reasoning M4).
4. **Verifica prima di conservare.** Un fatto non verificato dall'oracolo di dominio NON
   è conoscenza e non entra nella KB ufficiale.
5. **Motore fisso, conoscenza che cresce.** Ogni nuovo dominio = fatti + mappa
   superficie→vincolo + oracolo, mai un modulo cablato per la singola domanda.
6. **Onestà.** In assenza di derivazione o di fatti, si declina nominando il gap — mai si
   finge (la regola no-deception di tutta parrot0).

---

## 7. Dove attaccare per primo (handoff)

Il gancio è pronto: `src/brain/10-memory-knowledge.c`, sezione riddle-by-inference, e i
fatti in `kb/core/world-facts.p0`. Il primo passo concreto (U1) è portare la negazione a
vincolo di prima classe, perché sblocca la maggior parte delle clue reali e degli enunciati
scientifici ("in assenza di", "senza", "non"). Da lì U2 (grandezze) è il primo dominio
"duro" e misurabile. Tutto il resto pende da questi due.

> La cellula c'è e respira. Questo documento è la mappa del corpo che dovrà diventare.
---

# ⛔ ARCHI CONNETTIVI DINAMICI — la conoscenza detta in una forma, letta in un'altra

*Aperto il 2026-09-02 su indicazione di F. Questa sezione è condivisa da
`universal-input.md`, `universal-solver.md`, `frontier-kb-natural-dialogue.md` e
`apprendimento-assistito.md`: è una sola meccanica, e i quattro piani la usano da
lati diversi.*

## 1. Il difetto che lo ha reso necessario, misurato

```text
> teflon is a molecule              ->  Learned: teflon is a molecule.
> teflon contains fluorine          ->  Learned: teflon contains fluorine.
> tell me a molecule with fluorine  ->  «non riesco a verificare»
```

Tre turni andati a buon fine e nessuno servito a niente. La causa non è un muro
di comprensione né un vocabolario mancante:

- il percorso che **insegna** memorizza il fatto **unario** `molecule(teflon)`;
- il percorso che **legge** interroga la relazione **binaria**
  `category_member(molecule, teflon)`.

Due forme della stessa pretesa. parrot0 dichiara di aver imparato, il fatto è
davvero in KB, e la domanda formata con le stesse parole non lo trova. È **il
cassetto senza maniglia** nella sua forma più pura, e la sua gravità è che non si
vede: nessun errore, nessun avviso, solo una capacità che non si raggiunge.

## 2. La cura sbagliata, e perché lo era

La prima cura fu **una regola scritta a mano per quella coppia di forme**:

```prolog
category_member($C, $M) :- category($C), kb_fact($C, cons($M, nil)).
```

Funzionava. F. l'ha fermata:

> *«sei sicuro che la soluzione sia una "regola sola"? Secondo me la soluzione è
> un arco connettivo dinamico che possa connettere parti della conoscenza
> espresse in una forma a parti della conoscenza espresse in un'altra, attraverso
> un meccanismo di predicate-join e inferenza con predicati variabili.»*

Aveva ragione, ed è la stessa critica del mantra #2 **un livello più su**: una
regola per coppia di forme è *l'elenco degli incidenti* fatto di regole invece
che di parole. Domani la stessa frattura ricompare fra `contains` e `has_part`,
fra `located_in` e `in`, fra un'unaria e una ternaria — un ponte a mano per
ciascuna, e nessuno dei ponti si accorge degli altri.

> **Il test:** una cura per *questa* coppia non è una cura per la classe. Se la
> forma nuova costa una regola nuova, la conoscenza non si sta connettendo: si
> sta ricucendo a mano.

## 3. La forma giusta — l'arco è un FATTO, i predicati sono ARGOMENTI

```prolog
knowledge_arc(category_member, 0, category).
```

Si legge: *«nella relazione `category_member(A0, A1)`, l'argomento in posizione 0
**è il predicato** di un fatto che porta gli argomenti rimanenti»* — cioè
`A0(A1)`. Il terzo argomento è la **guardia**: dice quali predicati possono
essere promossi, perché non ogni unaria è una categoria (`stopword/1` e
`machinery/1` non lo sono).

```prolog
knowledge_alias(contains, has_part).
```

Si legge: *«le due relazioni portano la stessa pretesa»*.

Il motore (`kb_join_match` in `src/brain/10-memory-knowledge.c`) legge **una
posizione e una guardia**, e non nomina nessuna relazione. Un arco nuovo è **una
riga di KB** e vale per ogni consumatore insieme.

### Perché il join non può essere una regola KB

Vincolo reale del motore, verificato: la testa di clausola **non ammette un
predicato variabile** — `solve()` in `src/kb.c` seleziona le regole con
`strcmp(R->head.pred, g->pred)`. Si può scrivere `apply($P, $Args)` nel **corpo**
di una clausola, mai `$P($X)` in **testa**.

Perciò l'arco *dichiarato* sta in KB e il *percorso* sta in C — ed è la divisione
giusta secondo `PRINCIPLES.md`: motore fisso, conoscenza che cresce. Ciò che il C
sa fare è «promuovi l'argomento in posizione N a predicato»; **quali** relazioni,
in **quale** posizione, sotto **quale** guardia è interamente KB.

> Se un giorno il motore ammetterà una testa variabile, l'arco potrà migrare in
> KB senza cambiare una riga di conoscenza: i fatti sono già scritti nella forma
> giusta. È il criterio per capire se una meccanica è nel posto sbagliato *per
> ora* o *per sempre*.

## 4. Che cosa ha sbloccato, misurato

Il corpus **cresce parlando**, dal turno alla domanda:

```text
> tell me a molecule with fluorine   ->  «non riesco a verificare»
> teflon is a molecule               ->  Learned.
> teflon contains fluorine           ->  Learned.
> tell me a molecule with fluorine   ->  Teflon.
```

E una **categoria del tutto nuova** si apre in una frase che nessuno deve
imparare a formulare — `category/1` è nominata così apposta:

```text
> tell me a widget                ->  (muro onesto)
> widget is a category            ->  Learned.
> a florn is a widget             ->  Learned.
> tell me a widget                ->  Florn.
> florn contains quartz           ->  Learned.
> tell me a widget with quartz    ->  Florn.
```

**Effetto composto, non previsto e istruttivo:** `tell me an animal that lives in
water` prima murava; ora risponde **Amphibian**, ed è vero
(`habitat(amphibian, water)`). Metà dei membri di `animal` era scritta in una
forma che l'enumerazione non leggeva. Profondità e ampiezza si compongono: più
candidati visibili ⇒ più vincoli verificabili. Un arco non aggiunge una
capacità, **moltiplica quelle che ci sono**.

## 5. Il costo, e l'ottimizzazione che ne è nata

Un arco allarga i candidati, e ciò che era tollerabile diventa quadratico.
Misurato subito dopo: `tell me an animal that lives in lava` — un vincolo che
**nessuno** poteva soddisfare — **7,9 s**.

La causa è precisa: `member_satisfies(Membro, Valore)` chiede a `kb_fact/2` se
*qualche* relazione lega i due, e con il **predicato non legato** ogni chiamata è
una scansione dell'intera KB. Moltiplicata per ogni membro × ogni coda del
residuo: ~160 scansioni da 37 000 fatti.

Due uscite tentate, e solo la seconda è quella giusta:

1. ❌ **Pre-controllo «il valore compare da qualche parte?»** — anch'esso una
   scansione con predicato non legato: *aggiunge* lavoro invece di risparmiarne.
   Ritirato. È la trappola di questa classe: un'exit condition che costa quanto
   ciò che evita non è un'ottimizzazione.
2. ✅ **Invertire il join.** La domanda «questo membro è legato al valore?» posta
   N volte diventa «**chi** è legato a questo valore?» posta **una** volta, più
   un'intersezione in memoria:

   ```prolog
   related_to($Subject, $Value) :- kb_fact($Pred, cons($Subject, cons($Value, nil))).
   ```

   **7,9 s → sotto 1 s**, stessa risposta.

> **La regola che ne esce:** in un join con predicato variabile, il verso della
> domanda *è* la complessità. Non è una cache e non è una soglia — è la stessa
> domanda posta dove l'indice della KB può lavorare.

**Resta aperto (§L di `LEARN_TODO.md`):** `kb_fact/2` con predicato non legato è
O(fatti) per costruzione. L'inversione toglie il fattore N, non l'O(n). La cura
strutturale che F. ha chiesto — **indice per termine / kv hashing** — non è
ancora fatta: il censimento in `src/kb.c` indicizza per *predicato*, non per
*argomento*.

## 6. Il livello successivo — L'ADDESTRAMENTO DI ORDINE SUPERIORE

> F.: *«attraverso insegnamenti di ordine superiore spiegare cose come quella
> emersa, e in certi contesti che una cosa la contiene vuol dire che ne è una
> parte — così, di alto livello, sempre sfruttando il processo di inferenza
> prolog-like.»*

Gli archi del §3 li dichiara oggi un file `.p0`. Il passo successivo è che li
dichiari **chi parla**, in una frase. Non si insegna un fatto: si insegna una
**relazione fra relazioni**, e vale per inferenza dal turno dopo.

### Stato misurato (2026-09-02) — l'ordine superiore NON c'è, e mente

```text
> every knight is a noble             ->  Learned rule: noble(X) :- knight(X).   ✅
> if x contains y then y is part of x ->  Learned rule: part($V2) :- holds(x_contains_y).   ⛔
> contains means has_part             ->  «I cannot anchor that lesson yet»   (muro onesto)
> a container of something is a part of it  ->  «Subject.»   ⛔
```

- L'implicazione **unaria** fra classi è insegnabile e funziona.
- L'implicazione **binaria fra relazioni** non lo è — e il secondo turno è il
  difetto peggiore dei quattro: dichiara **«Learned rule»** per una regola priva
  di senso. Un misclaim su ciò che si è appena imparato è mantra #7, ed è peggio
  di un muro perché il turno dopo nessuno lo cerca.

### La forma da costruire, e perché è quasi tutta già lì

«se X contiene Y allora Y è parte di X» è **due frame** con **variabili
condivise**. Il legatore che serve esiste già ed è lo stesso del lettore:

```text
lato sinistro  -> p0_frame_bind -> contains(@S, @O)
lato destro    -> p0_frame_bind -> part_of(@O, @S)
variabili condivise -> i ruoli si corrispondono per NOME, non per posizione
                    -> assert  part_of($Y, $X) :- contains($X, $Y).
```

Gli invarianti del checkpoint ternario (§3 dell'handoff in `LEARN_TODO.md`)
valgono qui **immutati**, e non è un caso: è la stessa lezione a un ordine più
alto.

1. **Nessuna arità linguistica nel C.**
2. **Nessun ruolo per posizione** — il frame già compreso nomina i ruoli.
3. **Nessun vocabolo del gate nel C**: `contains`, `part of`, `se`, `allora`
   compaiono solo nel test.
4. **Target noto e univoco**: due letture ⇒ resta un gap, non si prende la prima.
5. **Il retract toglie la capacità, non la storia**: i fatti dedotti mentre la
   regola era viva restano; la regola sparisce.
6. **⛔ Nessun «Learned rule» senza una regola.** Se la lezione non si àncora, si
   dichiara il gap. Questa è la riga da chiudere per prima, perché oggi è attiva
   e mente.

### Il contesto, che è la parte che F. ha nominata per ultima e pesa di più

*«in certi contesti»* non è una sfumatura: è la differenza fra una regola e una
**regola con dominio**. «contenere vuol dire essere parte» è vero per una scatola
e i suoi oggetti, falso per un fiume e i pesci. La forma generale non è

```prolog
part_of($Y, $X) :- contains($X, $Y).
```

ma

```prolog
part_of($Y, $X) :- contains($X, $Y), <il contesto vale qui>.
```

dove il contesto è **a sua volta conoscenza interrogabile**, non una condizione
cablata. È il punto di contatto con `context-scope.p0` e con l'anti-isteresi:
una regola che vale ovunque è una regola che nessuno può correggere parlando.

### Ordine di lavoro proposto

1. **Togliere il misclaim** — «if X … then Y …» che non si àncora deve dichiarare
   il gap, non annunciare una regola. *Prima* di aggiungere capacità.
2. **L'implicazione binaria fra relazioni note**, con i ruoli per nome e il
   retract simmetrico.
3. **L'arco insegnato**: «contenere vuol dire essere parte» ⇒ asserisce
   `knowledge_alias/2`, cioè l'ordine superiore che *scrive gli archi del §3*.
4. **Il dominio del contesto**, come argomento in più e non come eccezione.
5. **L'indice per termine**, perché ogni arco in più moltiplica il join (§5).

### Il gate minimo, e nessuna scorciatoia

Una lezione di ordine superiore è chiusa solo se:

- vale su relazioni **held-out** (non `contains`/`part_of`, che sono l'esempio);
- vale in **entrambe le lingue**, perché la canonicalizzazione è l'unica via;
- si **ritratta** parlando, e ciò che aveva dedotto resta;
- **non** produce un «Learned rule» quando non ha ancorato niente;
- il contesto dichiarato **restringe** davvero: fuori dal dominio la regola non
  deve concludere.
