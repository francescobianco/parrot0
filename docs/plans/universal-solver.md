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