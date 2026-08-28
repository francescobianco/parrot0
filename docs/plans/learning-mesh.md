# Learning Mesh — catene di addestramento su una KB condivisa

> **Stato:** primo esperimento eseguito dal vivo a **gen335 (2026-07-16)**.
> Teacher LLM (Claude) → parrot0 A → parrot0 B, con **una sola KB montata** via
> `PARROT0_SESSION=kb/core/mesh-session.p0`, giudice LLM sullo stesso canale.
> **Ruolo:** nomina il concetto di *mesh di addestramento*, ne fissa il meccanismo
> minimo (mount condiviso + `kb.save`/`kb.restore`), riporta il transcript reale
> del primo giro e ne dà un giudizio critico onesto.
> **Subordinato a** [[kb-first-manifesto]] (*engine fixed, knowledge learns*),
> [[autolearn-knowledge-is-official]] (la conoscenza verificata diventa KB ufficiale),
> [[universal-input]] + [[the-linguistic-glue]] (il gap che ne limita la portata).
> **Riferimento tecnico:** [docs/prolog-like-engine.md](../prolog-like-engine.md)
> (§5 — insidie di sintassi scoperte in questo esperimento) e
> [docs/use-mcp-engine.md](../use-mcp-engine.md).

---

## ⇢ HANDOFF / ripartenza (2026-07-28, gen367)

### Dove siamo davvero

La base consegnata è il commit `e1920fd` (`gen367: generalize reasoning over
open concepts`), costruito sopra `4a3f3be` (`gen366: add transferable reasoning
operators`). Al momento dell'handoff `main` era pulito e allineato con
`origin/main`.

Il risultato **non è 20/20** e non va presentato come tale. Dopo gen366/gen367
non è stato eseguito un altro LLMSCORE remoto: l'ultimo report committato,
precedente agli operatori, resta 0/20. Il solo gate recente e pertinente è:

- `tests/reasoning_operators.sh`: **43/43**;
- `make build`: pulito, senza warning;
- nessuna suite completa eseguita, per richiesta esplicita di F.

Il problema aperto non è «quale altra risposta manca?». È: **quale operazione di
ragionamento manca per ricondurre prompt nuovi a una struttura già nota?** Ogni
agente che riparte deve resistere alla tentazione di leggere una domanda fallita
e insegnarne il contenuto. Le code LLMSCORE ruotano: una correzione che nomina il
tema corrente non guadagna terreno sulla coda successiva.

### Punto fisso raggiunto: R1–R4

Il lavoro già presente non è un phrasebook. Quattro famiglie usano relazioni
generali, termini aperti e clausole condivise:

1. **R1 — confronto orientato a un obiettivo.**
   `effective_property`, `effective_goal_prefers`, `task_difference` e
   `task_goal_match` confrontano alternative registrate o composti mai
   registrati. `linen shirt`/`wool coat` e `paper carton`/`plastic crate`
   attraversano lo stesso operatore.
2. **R2 — spiegazione causale di un fallimento.**
   `effective_system_relies_on`, `effective_phenomenon_exploits` ed
   `effective_phenomenon_example` compongono parti, dipendenze e fenomeni.
   Gli esempi sono eventi strutturati
   `example_event(Example, Subject, Relation, Object)`, non frasi salvate.
3. **R3 — procedura ordinata sotto vincoli.**
   `process_product`, `product_input`, `action_consumes`,
   `action_semantics(Action, Verb, Patient)`, `action_parameter`,
   `action_requires`, `action_produces` e `process_input_covered/4`
   costituiscono un piccolo calcolo di azioni. Il C fa solo ordinamento
   topologico, binding, verifica di copertura e resa dei campi semantici.
   Nessun mondo operatore contiene `action_instruction/2`.
4. **R4 — sintesi per copertura di requisiti.**
   `task_candidate`, `task_requirement` e `task_feature_match` scelgono
   caratteristiche che coprono tutti i requisiti estratti da termini aperti.
   Le stesse `action_semantics` rendono le caratteristiche.

Il `ReasoningTask` è la Task IR comune. Operazione, deliverable, entità,
obiettivo, risorse, vincoli, focus e deadline vengono legati attraverso
registri KB-first. `task_span_pattern/4` e `task_boundary_cue/2` delimitano
span aperti; `task_term_concept/2` proietta gli n-grammi concettuali
contigui. `task_entity_cue` è un alias opzionale, **non** la lista delle cose
che il sistema può ragionare.

File da leggere prima di toccare il comportamento:

- `PRINCIPLES.md`;
- `docs/plans/the-model-plan.md`, soprattutto §§12.5–12.6;
- `src/brain/10-memory-knowledge.c`;
- `kb/core/procedures.p0`, `kb/core/intents.p0`,
  `kb/core/presentation.p0`;
- `kb/facts/operator-worlds.p0`;
- `tests/reasoning_operators.sh`.

### Invarianti: «Astrai fino al punto fisso»

Prima di aggiungere un predicato, chiedere:

> È una relazione nuova, oppure la stessa relazione vista attraverso un altro
> verbo, sostantivo o dominio?

`wrote`/`painted`/`composed` non sono tre predicati: sono valori del campo
`Verb` di una sola relazione. Analogamente, birra, caffè, release e kit non sono
quattro recognizer: sono mondi di fatti attraversati dal medesimo calcolo di
azioni. Il test del punto fisso è sostituire tutti i nomi del dominio: se
l'operatore resta identico, la scomposizione è sufficientemente astratta.

Vincoli non negoziabili:

- niente fatto, cue o entità preso dalla coda LLMSCORE corrente o passata;
- niente payload di risposta in `claim_text`, `scenario_text`,
  `action_instruction` o equivalenti;
- niente predicato per verbo o per argomento;
- niente literal NL in C per riconoscere connettivi, preposizioni, question
  form, sinonimi o boundary: tutto ciò è conoscenza interrogabile nella KB;
- il C può implementare solo meccanica fissa: tokenizzazione, slot binding,
  ordinamento, deduplica, ricerca limitata, aritmetica e inferenza;
- ogni recognizer linguistico nuovo deve avere un test assert/retract a runtime;
- un test golden da solo non dimostra KB-first;
- una spiegazione deve poter esibire gli archi/clausole usati, non soltanto una
  frase plausibile.

Smell check rapido:

```sh
rg 'action_instruction\(|example_observation\(' kb/facts/operator-worlds.p0
rg 'task_entity_cue' kb/facts/operator-worlds.p0
```

Il primo comando deve restare vuoto. Il secondo richiede una giustificazione
come alias reale; non si aggiunge un'entità solo per far passare un prompt.

### Ripartenza esatta: R5, proiezione causale/counterfactual

Il prossimo passo non è un'altra tassonomia. È un unico operatore che, dato uno
stato iniziale, percorre conseguenze a profondità maggiore di uno. Specifica
minima consigliata:

```prolog
causal_transition(Prior, Relation, Next).
scenario_seed(Concept, State).

scenario_effect(Scenario, State) :-
    scenario_seed(Scenario, State).
scenario_effect(Scenario, Next) :-
    scenario_effect(Scenario, Prior),
    causal_transition(Prior, Relation, Next).
```

La sintassi concreta dovrà rispettare il dialect e le salvaguardie di
terminazione già documentate; il frammento descrive l'IR, non autorizza
left-recursion incontrollata. `Relation` è un campo: causa, impedisce, sposta,
riduce o amplifica non diventano cinque predicati.

Procedura per un coding agent:

1. **Scomporre il prompt senza rispondergli.** Annotare operazione richiesta,
   stato mutato, dominio, vincoli, orizzonte e deliverable. Se una parola del
   prompt compare nella proposta di nome di un predicato, fermarsi e tentare
   un'astrazione ulteriore.
2. **Disegnare il grafo.** Ogni sostantivo è candidato a stato/entità; ogni
   cambiamento è un arco con `Relation` come dato; adattamenti e feedback sono
   altri archi, non paragrafi. Separare la selezione degli archi dalla loro
   verbalizzazione.
3. **Derivare la chiusura.** La KB deve contenere la regola ricorsiva generale;
   il C può fare una BFS limitata soltanto per evitare cicli, ordinare per
   profondità e deduplicare.
4. **Rendere da campi.** Aggiungere frame KB-backed per rendere
   `{prior} {relation} {next}` e la catena. Non salvare la catena già scritta
   in prosa.
5. **Addestrare su mondi disgiunti.** Prima del codice fissare due mondi di
   training e un terzo held-out, tutti inventati localmente e non presi da
   LLMSCORE. Esempi accettabili: interruzione elettrica, ingresso scolastico
   posticipato, rimozione dei parcheggi. I nomi non devono entrare nel C.
6. **Ablare l'operatore, non decorare l'output.** Rimuovere la clausola generale
   deve rompere tutti e tre i mondi; rimuovere un arco locale deve rompere solo
   il mondo che lo usa. Se non accade, il test non sta misurando trasferimento.

Gate puntuale di R5:

- lo span nuovo arriva nella Task IR senza un nuovo `task_entity_cue`;
- il proof contiene almeno due `causal_transition`;
- ablazione della clausola generale: falliscono training e held-out;
- ablazione di un arco locale: fallisce un solo mondo;
- assert/retract di una nuova cue di richiesta cambia il riconoscimento senza
  rebuild;
- ogni probe locale termina entro un secondo.

Solo questi comandi sono autorizzati durante questa fase:

```sh
make build
make reasoning-operators
```

Non lanciare `make test`. Non lanciare `make llmscore` finché R5 non supera il
gate e non sono state affrontate almeno anche le famiglie successive:

- **argomentazione:** conflitto e composizione fra principi, evidenze e
  conseguenze, non una raccolta di opinioni;
- **composizione creativa:** ricerca di una combinazione che soddisfa vincoli
  simultanei, non una ricetta o un dialogo pre-scritto.

Il remoto va usato alla fine come misura su una coda sempre nuova, mai come
dataset. Non aprire `.llmscore_tail.json` per addestrare. Un aumento credibile è
trasferimento su prompt ignoti; 20 fix locali non sono 20 punti.

### Criterio di chiusura

Questo fronte è chiuso soltanto quando code indipendenti mostrano che le stesse
clausole servono domini e formulazioni non presenti nei mondi di training,
senza crescita prompt-specifica della KB o del C. Fino ad allora riportare:
famiglie implementate, prove di ablazione, latenza e score reale. Non dichiarare
«LLM-like» né 20/20 in anticipo.

## 0. Giudizio critico e sintetico (in testa, come richiesto)

**La mesh amplifica conoscenza composizionale in modo moltiplicativo e verificabile —
e, poiché la comprensione stessa è conoscenza, amplifica anche l'abilità di rispondere,
non solo la chiusura dimostrabile. Il limite onesto non è categoriale: è di copertura e
di disciplina del mount.**

Cosa è genuinamente vero, verificato dal vivo (§4, §4.1):

1. **La "KB unica montata" è reale e a costo quasi-zero.** Un file condiviso via
   `PARROT0_SESSION` + `kb.save`/`kb.restore` basta perché ciò che un'istanza congela
   diventi conoscenza di ogni altra istanza. Nessun nuovo codice, nessun protocollo:
   è il tree su disco usato come substrato comune.
2. **L'effetto moltiplicativo esiste, ed è misurabile.** Congelare **un operatore**
   di ordine superiore (la regola transitiva `is_a/2`) trasforma *N fatti lineari* in
   *fino a O(N²) conclusioni derivabili*. Nel giro reale: 3 fatti + 1 regola → 3
   conclusioni derivate oltre ai 3 archi. Questo è "aumento dell'ordine di complessità"
   in senso letterale, non retorico.
3. **La composizione attraversa i nodi.** Il teacher ha dato l'operatore, A un pezzo
   di catena, B un altro: sulla KB condivisa è emersa `is_a(dog, living_thing)` — una
   conclusione a 3 passi **che nessun singolo nodo ha mai ricevuto**. La mesh ha
   prodotto conoscenza che nessuno dei suoi membri possedeva da solo.
4. **La spina dorsale è onesta.** Ogni conclusione derivata porta la sua *proof*
   (`kb.explain`), e il giudice verifica che sia **derivata e non memorizzata**
   (`is_a(dog, living_thing)` non è tra i fatti del file). È la regola no-deception
   applicata alla mesh: si moltiplica solo ciò che si può dimostrare.

5. **La comprensione si insegna e si propaga come conoscenza (§4.1).** Congelando il
   fatto di comprensione `answer_frame("discovered", discovered_by)` accanto al fatto di
   dominio, *"who discovered polonium?"* passa da muro cieco a *"Marie_curie."* — e dopo
   `kb.restore` la **stessa risposta NL** compare sul nodo B, mai istruito. Il
   moltiplicatore arriva alla conversazione, restando KB-first ([[universal-comprehension]]).

Cosa **non** è vero — e va detto per non spacciare l'esperimento per più di quel che è:

- **Non ogni superficie è ancora rispondibile-come-fatto.** Ho chiuso in NL il caso
  *lookup* (§4.1); il **sì/no su una relazione derivata** del §4 (*"is a dog an animal?"*)
  resta `kb.query`-only, perché manca il *consumer* insegnabile che verbalizzi *Yes/No* su
  una `kb.query`. Non è un muro categoriale (la comprensione è conoscenza): è **copertura
  mancante**, il fronte di [[universal-input]]/[[the-linguistic-glue]], da tirare un frame
  alla volta (§6, MESH-L1).
- **Ho montato la mesh sullo spill, non sul tree instradato.** `kb.save` ha un **router**
  (il save-map, §3.1) che homa ogni fatto nel file curato dei suoi parenti per coppia
  `(predicato, token)`; `session.p0` è solo il residuo non-instradato. Il primo esperimento
  non ha impostato `PARROT0_KB_ROOT`, quindi ha spillato tutto — rumore di sessione incluso,
  duplicato per nodo. La forma giusta della mesh monta il **tree instradato**. Restano due
  debiti reali: le **regole (gli operatori) non si instradano mai** e non c'è disciplina di
  **merge/lock** (due save concorrenti si clobberano) → oggi la mesh regge in sequenza, non
  in parallelo (§6, MESH-L2/L3).
- **Teacher e giudice sono lo stesso LLM (io).** Un anello chiuso che *corregge i
  propri compiti*. Un valore di mesh credibile richiede un giudice indipendente e
  sonde held-out — altrimenti misura la propria coerenza, non un guadagno reale.

**In sintesi:** la learning mesh è un *harness sperimentale valido e a basso costo per
far crescere la KB in modo composizionale e verificabile* — e il guadagno **arriva alla
conversazione** quando si congela anche la comprensione (che è conoscenza, §4.1). **Non è
ancora** un moltiplicatore autonomo di capacità per tre ragioni oneste, tutte di
copertura/disciplina e non categoriali: (a) solo alcune superfici sono già rispondibili
come fatto (il lookup sì, il sì/no derivato no ancora); (b) il mount va montato sul tree
**instradato** e il router non homa ancora gli operatori; (c) mancano merge/lock e un
giudice indipendente. La rotta giusta è tirare i frame di comprensione mancanti, far
tendere il router a instradare tutto (regole incluse), e dare al mount merge + un giudice
terzo: allora l'ordine superiore congelato è insieme *chiusura dimostrabile* **e**
*abilità conversazionale*, moltiplicata su ogni nodo.

---

## 0.1 Scoperte da tenere in EVIDENZA (per non ripassarci)

*Indice delle cose apprese in questa sessione (gen335). Ognuna rimanda al dettaglio;
sono qui in alto proprio perché la prossima volta non si riscoprano da zero.*

1. **`machinery/1` è il marcatore interno KB-first — NON editare la lista C.** Un
   predicato-substrato non deve inquinare "quante cose sai?". Il modo giusto è
   dichiarare **`machinery(pred).` nella KB, nello stesso file del fatto**;
   `is_internal_pred` (`40-meta-reflection.c:507`) interroga `machinery/1` *prima*
   della sua lista C, che è solo **fallback legacy**. Il commento a `:521` dice che è
   già successo 3 volte che un predicato-machinery "leakasse" nel conteggio per una
   dimenticanza sulla lista C — `present_rule` è stata la 4ª, e la cura è stata
   `machinery(present_rule).` in `presentation.p0`, **zero righe di C**. (§4.3)
2. **Il layer di PRESENTAZIONE è KB-first** (steer di F.): togliere il trattino,
   capitalizzare i nomi propri, decorare l'output è **conoscenza**. Meccanismo in C
   (`present_atom`), conoscenza in KB (`present_rule/1` + `proper_name/1` teachable).
   "Marie_curie."→"Marie Curie.", "South_america."→"South America.", e teachable dal
   vivo. (§4.3)
3. **La comprensione è conoscenza e la mesh la fa crescere** (§4.1): congelare
   `answer_frame/2` porta un intero registro di domande dal muro cieco alla risposta
   NL, propagata tra i nodi. Due giri fatti (paternità, geografia) — §4.2.
4. **Insidie di sintassi Prolog-like via MCP** (dettaglio in
   [prolog-like-engine.md §5](../prolog-like-engine.md)): regole n-arie →
   `kb.assert_clause` (non `kb.assert_rule`); variabili col prefisso **`$`** (maiuscole
   nude = atomi, footgun silenzioso); chiavi `gen.respond{input}`,
   `kb.query/match{pred,args}`. (§5)
5. **Fact routing (save-map)**: `kb.save` è un *router* per `(predicato, token)`, non
   un dump; le **regole non si instradano mai**; opt-in `PARROT0_KB_ROOT`. (§3.1)
6. **⚠️ Stato del tree: `make test` è 189/60 (ROSSO) già su `main`**, PRIMA di ogni
   modifica di questa sessione. Le 60 failure sono test italiani `.it.chat` che si
   aspettano lemmi IT (uomo, cane, gatto) mentre parrot0 emette i lemmi EN (man, dog,
   cat) — es. `mortale(X) :- man(X)` vs atteso `:- uomo(X)`. **Regressione preesistente,
   non introdotta qui** (il gate di questa sessione è "non aumentare il fail-count",
   rispettato: 60→60). Da investigare a parte. (§6, MESH-L6)
7. **Proposta aperta — invertire `is_internal_pred`** (domanda di F.): oggi il default
   è *pubblico*, si flagga l'*interno*; l'alternativa è default *interno*, si flagga il
   *pubblico*. Analisi e raccomandazione in §6, MESH-L7.
8. **Il parser `.p0` ora legge PIÙ clausole per riga + ERRORE rumoroso.** Prima
   `a(1). b(2). c(3).` su una riga caricava solo la prima e scartava il resto **in
   silenzio** (≥5 volte). Curato in `src/kb.c`: split sui `.` di livello 0 + un errore su
   stderr per ogni clausola non-vuota che non parsa. (§4.4, [prolog-like-engine.md §1](../prolog-like-engine.md))
9. **Closure transitiva LEFT-recursive = timeout.** `p($X,$Z):-p($X,$Y),p($Y,$Z)` va in
   loop; usa la forma a **predicato separato right-recursive** (primo goal = fatto), fatti
   prima delle regole. (§4.4)
10. **`KB_TERM_LEN=128` scarta i template lunghi.** L'errore rumoroso ha scovato 13
    `response_template` morti (~200-356 char) mai caricati. Fix a sé. (§6, MESH-L9)

---

## 1. La tesi

> Una singola coppia teacher↔allievo (è ciò che fa già `autolearn`, [[rulescore-harness]]
> e [[llmscore-harness]]) insegna *fatti*. Una **mesh** — più istanze di parrot0 che si
> interrogano su **una KB condivisa**, con un LLM che fa da teacher e uno da giudice —
> punta a insegnare *operatori*: strutture di ordine superiore che, congelate una volta,
> moltiplicano ciò che ogni nodo può derivare.

Il salto non è "più dati": è **più ordine**. Un fatto aggiunge una foglia; un operatore
(una regola n-aria) aggiunge una *funzione sui fatti*. Su una KB condivisa, contributi
indipendenti di nodi diversi si incontrano sotto lo stesso operatore e la chiusura
derivabile cresce super-linearmente. Questa è la scommessa della mesh: **effetto
moltiplicativo per composizione, non additivo per accumulo.**

È KB-first fino in fondo ([[kb-first-manifesto]]): il motore (unificazione, SLD) resta
fisso; ciò che la mesh fa crescere è *conoscenza* — fatti **e** regole — nel tree su disco.

## 2. L'architettura

```
        ┌─────────────┐   insegna operatore+semi (kb.assert / kb.assert_clause)
        │  TEACHER LLM │ ─────────────────────────────────────────────┐
        │  (Claude)    │                                              ▼
        └─────────────┘                                       ┌──────────────┐
              │ giudica (kb.explain, held-out probe)          │  parrot0  A  │
              │                                               └──────┬───────┘
              ▼                                          kb.save     │  kb.restore
        ┌─────────────┐                                  ┌───────────▼───────────┐
        │  JUDGE  LLM │ ◀────── proof, provenance ────── │  MOUNT UNICO (on-disk) │
        │  (Claude)   │                                  │ kb/core/mesh-session.p0│
        └─────────────┘                                  └───────────▲───────────┘
                                                          kb.restore  │  kb.save
                                                                ┌──────┴───────┐
                                                                │  parrot0  B  │
                                                                └──────────────┘
```

- **Teacher LLM** — decide *quale* struttura di ordine superiore congelare e la insegna
  a un nodo via MCP (`kb.assert`, `kb.assert_clause`, `kb.assert_rule`).
- **Mesh di interlocutori** — 2+ istanze di parrot0, ognuna un `--mcp-engine` separato,
  che condividono **lo stesso file di sessione**. Ogni nodo può contribuire fatti propri.
- **Mount unico** — il file `PARROT0_SESSION` comune. È la "KB unica per la mesh"
  richiesta: chi `kb.save` congela lì, chi `kb.restore` propaga da lì.
- **Judge LLM** — verifica che il guadagno sia *derivato e dimostrabile*, non memorizzato
  né inventato (no-deception).

## 3. Il meccanismo (il pezzo che rende la mesh possibile)

Scoperta centrale di questo esperimento, dal sorgente + prove dal vivo:

| Operazione | Cosa fa davvero |
|---|---|
| `PARROT0_SESSION=<file>` | `brain_boot` (`src/brain/99-registry.c:643`) carica quel file come layer di sessione. Impostarlo **uguale su tutte le istanze** = una sola KB montata per la mesh. |
| `kb.save {"path":<file>}` | `brain_save_session` (`99-registry.c:560`) persiste il delta `SESSION|INDUCED`. **Se `PARROT0_KB_ROOT` è impostato instrada** ogni fatto nel file curato dei suoi parenti (§3.1, `kb_save_routed`); **altrimenti** scrive tutto in `<file>` (spill legacy). `path_ok`: solo path relativi, in-repo, senza `..`. |
| `kb.restore {}` | `brain_reload` ricostruisce il brain **rieseguendo `brain_boot`** → ricarica base + tree + `PARROT0_SESSION` da disco. Ciò che un altro nodo ha appena salvato **va live senza restart**. |

Quindi la propagazione nella mesh è: **A `kb.save` → (tree/mount condiviso) → B `kb.restore`**.
Non serve rete, non serve un broker: il tree su disco *è* il canale.

### 3.1 Fact routing — il save-map (il pezzo che avevo sottovalutato)

> **`kb.save` non è un dump: è un ROUTER.** Questo è il meccanismo che decide *dentro
> quale file* del tree curato finisce ogni conoscenza congelata, ed è il cuore di come
> una mesh mantiene una KB **organizzata** invece di un blob che cresce. `session.p0`
> non è "la KB salvata": è solo lo **spill del non-instradato**.

`kb_save_routed` (`src/kb.c:2092`, commento canonico lì; header `src/kb.h:179-186`):

- **Coordinata di un fatto = `(predicato, primo-argomento)`.** È la chiave con cui il
  router cerca i "parenti" del fatto nel tree.
- **Tre tier di match, dal più preciso al fallback:**
  1. **coppia esatta** `(predicato, token)` già presente in un file → il fatto va **lì**;
  2. altrimenti **stesso predicato** in un file → va in quel file;
  3. altrimenti → **default/spill** (`PARROT0_SESSION`).
- **Che cosa si instrada e cosa no** (`sm_parse`, `kb.c:2114`): **solo fatti ground
  positivi**. **Regole (`:-`), negativi (`not(...)`), direttive e commenti NON si
  instradano mai** → finiscono sempre nello spill.
- **L'indice operativo e' in memoria** ed e' ricostruito a ogni save scandendo il
  tree. `<root>/savemap.tsv` ne e' soltanto un dump ispettivo mai riletto: non e'
  una cache ed e' deprecato. La registry del loader dovra' sostituire la scansione
  con la coppia esatta e un indice per predicato valido soltanto se la casa e'
  univoca.
- **Opt-in via `PARROT0_KB_ROOT`.** Senza, `brain_save_session` fa il vecchio save a file
  singolo — motivo per cui il primo esperimento (§4) ha *spillato tutto* nel mount invece
  di instradarlo. I test ermetici restano così di proposito.

**Prova dal vivo (root di scratch, tree curato reale non toccato):** dato
`kb/experts/bio/taxonomy.p0` che già contiene `is_a(cat,mammal)`, `is_a(cow,mammal)`:

```
teach is_a(dog, mammal)     → ha parenti (stesso pred is_a)  → ROUTED in taxonomy.p0 (riga 3)
teach habitat(dog, house)   → nessun parente                 → SPILL (session)
teach la REGOLA is_a/2      → è una clausola, mai instradata  → SPILL (session)
indice in memoria:  is_a  dog  …/bio/taxonomy.p0  3
```

**La visione (steer di F.):** il save-map è la forma corretta della persistenza, e va
fatto *tendere a instradare tutto*. `session.p0` deve restare **solo per il residuo che
ha davvero senso di stare in sessione** (roba effimera, senza casa nel tree). Man mano che
il router migliora, sempre più conoscenza trova la sua topica curata e lo spill si
assottiglia. Per la mesh questo è cruciale: **la "KB unica montata" giusta è il tree
curato instradato (`PARROT0_KB_ROOT`), non lo spill di sessione.** Congelare un'abilità =
instradarla nella sua topica, dove ogni nodo la ricarica *già organizzata*.

**Il buco che questo apre per la mesh:** l'unità di ordine superiore della mesh è la
**regola** (l'operatore), e le regole **non si instradano mai** — spillano sempre in
sessione. Cioè oggi il router homa i *fatti* ma non gli *operatori*, che sono proprio la
cosa più preziosa da congelare. Vedi MESH-L2 (§6).

## 4. Il primo esperimento (gen335) — transcript reale

**Abilità di ordine superiore da congelare:** la regola transitiva
`is_a($X,$Z) :- is_a($X,$Y), is_a($Y,$Z)`. Non è un fatto: è un **operatore sui fatti**
(un fatto aggiunge un arco; questo operatore aggiunge la *chiusura transitiva* di tutti
gli archi presenti e futuri). Script riproducibile:
[`scripts` — vedi §5](#5-procedura-riproducibile).

```
BASELINE  — nessuna istanza deriva is_a(dog, living_thing)
  A: {"provable":false}   B: {"provable":false}
  A in NL "is a dog an animal?" → "I don't know about animal."

TEACHER → A  — congela l'OPERATORE (kb.assert_clause) + 2 semi (kb.assert)
  is_a($X,$Z) :- is_a($X,$Y), is_a($Y,$Z).   is_a(dog,mammal).   is_a(mammal,animal).
  A prova dog→animal: {"provable":true}
  A proof: "is_a(dog, animal) because is_a(dog, mammal) and is_a(mammal, animal)"

FREEZE  — A kb.save nel mount condiviso  → written:12 (regola + fatti + rumore di sessione)

PROPAGATION  — B kb.restore  → guadagna un'abilità mai insegnata a B
  B prima: {"provable":false}   B dopo: {"provable":true}
  B proof: "is_a(dog, animal) because is_a(dog, mammal) and is_a(mammal, animal)"

MESH INTERLOCUTION  — B contribuisce una frontiera nuova (né teacher né A la conoscevano)
  is_a(animal, living_thing).   B kb.save

ORDER INCREASE  — A kb.restore  → deriva una conclusione a 3 hop che NESSUN nodo ha ricevuto
  A dog→living_thing: {"provable":true}
  A proof: "is_a(dog, living_thing) because is_a(dog, mammal)
            and is_a(mammal, living_thing) because is_a(mammal, animal)
            and is_a(animal, living_thing)"

JUDGE (onestà)  — derivata o memorizzata?
  is_a(dog, living_thing) NON compare tra i fatti del file (grep): è PROVATA, non stored.
  3 fatti-arco → chiusura: {dog→animal, dog→living_thing, mammal→living_thing} derivati.
```

Il risultato chiave è l'ultima proof: una catena a tre passi, assemblata da tre
contributi **indipendenti e di provenienza diversa** (operatore dal teacher, `dog→mammal`
e `mammal→animal` da A, `animal→living_thing` da B), che nessun nodo avrebbe potuto
provare da solo. È l'effetto-mesh reso concreto, con la sua proof.

## 4.1 La comprensione È conoscenza — quindi la mesh cresce anche quella

> **Integrazione (steer di F.):** il §4 fa crescere la *chiusura dimostrabile*, ma un
> guadagno che resta `kb.query`-only è mezza mesh. Il punto di
> [[universal-comprehension]] chiude il cerchio: **capire una frase è conoscenza**, non
> una facoltà cablata. Le strutture — `intent_cue`, `answer_frame`, `intent_schema`,
> `register_evidence`, `segment_role` — vivono nella KB. Perciò la mesh non deve solo
> congelare *operatori* e *fatti di dominio*: deve congelare anche i **fatti di
> comprensione** che rendono una classe di domande *rispondibile in linguaggio naturale*.
> Allora il moltiplicatore arriva alla conversazione, restando KB-first.

**Dimostrato dal vivo (gen335), stesso ciclo freeze→propagate:**

```
BASELINE  — A in NL "who discovered polonium?" → "I don't understand that yet."   (muro CIECO = fallimento di COMPRENSIONE, non solo dato mancante)

TEACHER → A  — congela DUE cose insieme:
  fatto di dominio:      discovered_by(polonium, marie_curie).
  fatto di COMPRENSIONE: answer_frame("discovered", discovered_by).   ← la mappa superficie→goal
  A in NL "who discovered polonium?" → "Marie_curie."                 ← ora COMPRENDE e risponde

FREEZE + PROPAGATE  — A kb.save → B kb.restore
  B prima:  "I don't understand that yet."
  B dopo:   "Marie_curie."                                            ← la COMPRENSIONE si è propagata nella mesh
```

`answer_frame(Cue, Pred)` (gen306, [[teachable-comprehension-answer-frame]]) è la
*consumer-gap reduction*: `mod_answer_frame` (`10-memory-knowledge.c:2447`) riconosce il
cue nella frase e risolve `Pred(token, ?)`, **senza un modulo C dedicato per quella
domanda**. Insegnare il frame *è* insegnare la comprensione. E poiché è un fatto, si
congela e si propaga come qualunque altra conoscenza della mesh.

**La frontiera onesta (dove finisce ciò che ho dimostrato).** Ho chiuso il caso *lookup*
("chi/che cos'è la X di Y"). Il caso del §4 — il **sì/no su una relazione derivata**
("is a dog an animal?") — richiede un *consumer* diverso (un ragionatore sì/no che chiama
`kb.query` sull'operatore e verbalizza *Yes/No*): non ho dimostrato che *quella* superficie
sia già insegnabile come puro fatto KB. È esattamente il fronte che
[[universal-input]]/[[the-linguistic-glue]] tracciano. Quindi la tesi giusta, e onesta,
è: **la comprensione è conoscenza e la mesh la cresce — dimostrato per il frame di lookup;
la copertura delle altre superfici (sì/no, imperativo, ecc.) è il lavoro aperto, non un
muro categoriale.** La mesh è il posto naturale per tirarlo, un frame per volta.

## 4.2 Due giri di learning per la coerenza tipo-LLM (comprensione = conoscenza)

Obiettivo di F.: rendere parrot0 **più simile a un LLM nella coerenza linguistica**. Un
LLM quasi mai risponde "I don't understand that yet" a una domanda ben formata; parrot0
sì, quando manca il *frame di comprensione* (non il dato). Ogni giro converte un registro
di muri ciechi in risposte coerenti, KB-first, e propaga sulla mesh.

- **Giro 1 — paternità ("who wrote X?").** Prima: muro cieco. Insegnati
  `answer_frame("wrote", wrote)` + 4 fatti `wrote/2` → *"who wrote Hamlet?"* → **Shakespeare.**,
  generalizza a *1984* → Orwell, *Macbeth* → Shakespeare. Freeze → `kb.restore` su B →
  B risponde uguale. Onestà preservata: *"who wrote War and Peace?"* (non insegnato) → muro,
  **non** inventato.
- **Giro 2 — copertura relazionale (currency, continent).** `currency` era già *compreso*
  (declino informato che nomina la relazione) → bastano i fatti (`currency(japan, yen)`);
  `continent` mancava il frame → `answer_frame("continent", continent_of)` + fatti. Risultati:
  *"currency of Japan?"* → yen, *"continent is Egypt in?"* → Africa, *Brazil* → South America.
  *Canada* (non insegnato) → muro onesto.

Due tipi di gap, entrambi KB: **frame presente, fatti mancanti** (currency) e **frame
mancante** (continent, paternità). Il gap che NON è KB e resta debito C: la frase
**composta** *"my dog is Rex and he is brown"* perde il nome (parsing di congiunzione,
§6 MESH-L8).

## 4.3 Il layer di PRESENTAZIONE è conoscenza (steer di F.) + la cura del leak `machinery/1`

> **Steer di F.:** «tra tutti gli strati d'inferenza nella KB ci deve essere quello di
> **presentation** — quello che sa che i nomi di persona importanti si scrivono in
> maiuscolo, che sa togliere il trattino, che sa mettere decorazioni (virgolette,
> grassetto). Anche la presentazione e la manipolazione del dato in output è KB-first.»

**Realizzato (gen335), C = meccanismo, KB = conoscenza:**
- `kb/core/presentation.p0` (nuovo layer, caricato dopo `morphology.p0`): `present_rule/1`
  attiva una regola di superficie; `proper_name/1` (in `morphology.p0`, **teachable**) dice
  quali atomi sono nomi propri da Capitalizzare.
- `present_atom(Brain*, in, out, n)` (`10-memory-knowledge.c`): l'unico meccanismo — toglie
  il separatore se `present_rule(strip_underscore)`, e Title-Case se `proper_name(in)`.
  Applicato ai choke point di verbalizzazione (`mod_answer_frame` + il modulo-relazione).
  **Nessuna lista di nomi cablata, nessun "South America" cablato.**

Dal vivo: `"Marie_curie."→"Marie Curie."`, `"South_america."→"South America."`, e
**teachable in diretta**: `"New york."` → dopo `kb.assert proper_name(new_york)` →
`"New York."` — la conoscenza di CHI è nome proprio è un fatto KB che si propaga sulla mesh.

> **⚠️ SCOPERTA da tenere in evidenza — `machinery/1`, non la lista C.** Aggiungendo
> `present_rule` il conteggio "quante cose sai?" è passato da 0 a 1 fatto: un
> predicato-substrato **leakato** nel conteggio (`introspect.chat` rotto). La cura NON è
> editare la lista C `is_internal_pred`: quella lista è *conoscenza cablata nel C* ed è
> il **fallback legacy**. `is_internal_pred` (`40-meta-reflection.c:507`) interroga prima
> **`machinery(pred)`** nella KB — quindi basta dichiarare **`machinery(present_rule).`
> nello stesso file del fatto** (`presentation.p0`) e il leak sparisce con **zero righe di
> C**. Il commento a `:521` lo dice: è già successo 3 volte per dimenticanza sulla lista;
> questa (la 4ª) è stata chiusa alla radice, in KB. *Regola d'ora in poi: ogni predicato
> nuovo che è machinery/substrato si auto-dichiara `machinery(...)` accanto ai suoi fatti.*

## 4.4 Tre giri di conoscenza di ORDINE SUPERIORE (congelata in `kb/core/mesh-knowledge.p0`)

Obiettivo di F.: *massimizzare la conoscenza ad ordini superiori*, poi misurare (LLMSCORE
come specchio, non come target). Un fatto è una foglia; un **operatore** (regola n-aria) è
una funzione sui fatti. Tre giri, ognuno un operatore composizionale (ordine-2: una regola
che ragiona su ciò che un'altra ha reso derivabile), con proof da `kb.explain`:

- **Giro 1 — ereditarietà lungo la tassonomia.** `kind_of` transitivo + `prop_of`/`able_to`
  che ereditano lungo `kind_of`. `prop_of(dog, breathes)` si deriva a **4 livelli**:
  `kind_of(dog,mammal) … kind_of(animal,living_thing) … has_own_prop(living_thing,breathes)`.
- **Giro 2 — ordinamenti transitivi.** `larger_than`, `warmer_than`, `before_in_time`.
  `larger_than(elephant, ant)` = catena a **5 passi** sulla scala di taglia.
- **Giro 3 — parentela: join + ricorsione.** `grandparent` (join) e `forebear` (ricorsivo).
  `forebear(tom, kim)` attraversa **4 generazioni**.
- **Copertura rispondibile in NL** (ordine-1, `answer_frame` su predicati piatti): paternità
  ("who wrote Frankenstein?" → *Mary Shelley.*, col layer di presentazione) e geografia.

> ⚠️ **SCOPERTA 1 — closure LEFT-recursive = ricorsione patologica.** Scrivere una closure
> transitiva come `p($X,$Z):-p($X,$Y),p($Y,$Z)` (stesso predicato, primo goal ricorsivo) manda
> l'SLD in loop/timeout — **peggio se la regola precede i fatti nel file**. (L'esperimento
> is_a del §4 reggeva solo perché lì i fatti erano asseriti *prima* della regola.) La forma
> ROBUSTA è una closure a **predicato separato, right-recursive** col primo goal = FATTO:
> `closure($X,$Y):-base($X,$Y). closure($X,$Z):-base($X,$Y),closure($Y,$Z).`, e i **fatti prima
> delle regole**.
>
> ⚠️ **SCOPERTA 2 — il parser `.p0` non leggeva più clausole per riga (perdita SILENZIOSA).**
> `a(1). b(2). c(3).` su una riga caricava solo la prima e scartava il resto senza un fiato —
> per questo `mesh-knowledge.p0` all'inizio "non caricava". **Curato in `src/kb.c`** (gen335):
> il loader splitta la riga sui `.` di livello 0 (fuori da parentesi/virgolette) **e ogni
> clausola non-vuota che non parsa emette un ERRORE su stderr** (mai più perdite silenziose —
> era già successo ≥5 volte). Vedi [prolog-like-engine.md §1](../prolog-like-engine.md).
>
> ⚠️ **SCOPERTA 3 — l'errore rumoroso ha scovato 13 `response_template` MORTI.** Argomenti
> più lunghi di `KB_TERM_LEN=128` char (spiegazioni di ~200-356 char: cielo blu, stagioni,
> pioggia…) erano rifiutati da `parse_term` **da sempre**, in silenzio. Fix aperto (§6,
> MESH-L9): alzare `KB_TERM_LEN` (costo memoria) o accorciare quei template.

## 5. Procedura riproducibile

Le procedure di questo primo giro, così come sono state eseguite:

1. **Monta la KB unica.** Scegli un file relativo in-repo (`path_ok`), es.
   `kb/core/mesh-session.p0`, e avvia ogni istanza con quel `PARROT0_SESSION`. Ogni
   istanza vuole un `PARROT0_MCP_DIR` distinto (stato del transport in
   `scripts/mcp-live.sh`), stesso mount:
   ```bash
   : > kb/core/mesh-session.p0                      # mount pulito
   PARROT0_MCP_DIR=/tmp/mesh-a scripts/mcp-live.sh start PARROT0_SESSION=kb/core/mesh-session.p0
   PARROT0_MCP_DIR=/tmp/mesh-b scripts/mcp-live.sh start PARROT0_SESSION=kb/core/mesh-session.p0
   ```
2. **Baseline.** Interroga entrambi con `kb.query` sull'obiettivo: deve fallire.
3. **Teacher congela l'operatore** su un nodo. Regola n-aria → `kb.assert_clause` con
   **variabili `$`** (vedi insidia §5.1); fatti-seme → `kb.assert`.
4. **Verifica locale** su quel nodo (`kb.query` + `kb.explain` per la proof).
5. **Freeze:** `kb.save {"path":"kb/core/mesh-session.p0"}`. Per instradare i fatti nel
   tree curato invece di spillare tutto, avvia con `PARROT0_KB_ROOT=<root>` (§3.1); così i
   fatti-arco vanno nella loro topica e solo il residuo (la regola, il rumore) resta nello
   spill. Nell'esperimento §4 `PARROT0_KB_ROOT` era **non** impostato → tutto nello spill.
6. **Propaga:** sull'altro nodo `kb.restore`, poi ri-verifica: ora prova ciò che non gli
   è mai stato insegnato.
7. **Interlocuzione:** un nodo aggiunge un fatto-frontiera (`kb.assert` + `kb.save`).
8. **Order increase:** l'altro nodo `kb.restore` e deriva la conclusione composta;
   `kb.explain` ne dà la proof multi-hop.
9. **Giudice (onestà):** conferma che la conclusione **non è un fatto memorizzato**
   (assente dal file / da `kb.match`), ma derivata.
10. **Cleanup:** ferma le istanze; rimuovi `kb/core/mesh-session.p0` se l'esperimento è
    esplorativo (non inquinare il tree curato). Se l'operatore è conoscenza ufficiale da
    tenere ([[autolearn-knowledge-is-official]]), instradalo in un file curato, non nel
    mount effimero.

### Insidie di sintassi scoperte (annotate anche in [prolog-like-engine.md §5](../prolog-like-engine.md))

**5.1 — `kb.assert_clause` vuole variabili `$`; le MAIUSCOLE nude sono ATOMI (footgun
silenzioso).** `args:["X","Z"]` asserisce `is_a('X','Z') :- …` — inutile — e restituisce
comunque `{"ok":true}`: **nessun errore visibile**, la query poi fallisce senza spiegazione.
Corretto: `args:["$X","$Z"]`. È la regola `is_var` (gen284: solo `$`/`_` è variabile,
maiuscole = costanti), ma via MCP morde forte perché non c'è feedback.

**5.2 — `kb.assert_rule` NON basta per regole con join.** Appiattisce ogni goal del body
a unario (`src/kb.c` `kb_assert_rule_n`). Una transitiva va con `kb.assert_clause`
(head/body come oggetti `{"pred","args"}`). Questo *colma* il divario che
`prolog-like-engine.md §5` dava ancora come "roadmap": è già realtà (gen311, verificata qui).

**5.3 — Chiavi dei tool facili da sbagliare.** `gen.respond` → `{"input":…}` (non `text`);
`kb.query`/`kb.explain`/`kb.match` → `{"pred":…,"args":[…]}` (non `goal:"is_a(dog,animal)"`);
`null` in `args` = variabile di query.

**5.4 — Provabile ≠ raggiungibile in NL, ma la mappa È insegnabile.**
`kb.query is_a(dog,living_thing)` → `provable:true`, ma `gen.respond "is a dog a
living_thing?"` non risponde: manca la mappa superficie→goal. **La mappa però è
conoscenza:** congelare `answer_frame/2` porta il caso *lookup* fino alla risposta NL,
propagato tra i nodi (§4.1). Il sì/no derivato attende ancora il suo frame (§6, MESH-L1).

## 6. Limiti scoperti — da sistemare o evolvere

*(esplicitamente richiesti: ciò che compromette la generalità del progetto.)*

- **[MESH-L1 · RISOLTO nel caso a slot-singolo — NON era un muro] La comprensione è
  conoscenza, quindi la raggiungibilità si INSEGNA.** *(Correzione, gen335, su obiezione di
  F.: avevo inquadrato la "raggiungibilità" come collo di bottiglia — sbagliato, contraddice
  "la comprensione è conoscenza".)* Gli operatori **derivati** sono rispondibili in NL
  insegnando **solo un `answer_frame`**, zero C — dimostrato dal vivo:
  ```
  answer_frame("can", able_to). answer_frame("properties", prop_of). answer_frame("ancestors", forebear).
    "what can a dog do?"              → "Move."                       (able_to ereditato)
    "what properties does a dog have?"→ "Warm blooded, has hair, moves, eats, breathes, grows and needs water."
    "who are the ancestors of tom?"   → "Bob, liz, ann, zoe, kim and max."  (forebear ricorsivo)
  ```
  `answer_frame` fa `kb_match(pred, token, ?)`, e sulle closure **right-recursive** `kb_match`
  enumera le risposte derivate senza impiantarsi (vedi MESH-L5, risolto). Quindi il "sì/no
  derivato" e le domande a token-singolo **non sono muri**: sono un frame da insegnare.
  **Residuo onesto (comprensione più espressiva, non un muro):** il *word-problem* (Q10) e i
  puzzle estraggono **più quantità/ruoli** dalla prosa e li legano agli argomenti di una
  procedura — cosa che `answer_frame` (single-slot) non fa. Serve un frame **multi-slot**
  (estrazione → binding di più argomenti), ancora conoscenza KB ([[universal-input]]
  `segment_role`), ma con un consumer più espressivo del single-token attuale.

- **[MESH-L2 · alto] Il router di salvataggio non homa gli OPERATORI, e la mesh deve
  montare il tree instradato — non lo spill.** Il fact routing (§3.1) è la forma giusta
  della persistenza, ma con due debiti che mordono proprio la mesh: **(a)** le **regole
  (gli operatori) non si instradano mai** (`sm_parse` scarta le clausole) → l'unità di
  ordine superiore finisce sempre nello spill di sessione, mescolata al rumore
  (`process_pid`, `policy`, `utterance`, duplicati per nodo). **(b)** Senza
  `PARROT0_KB_ROOT` `kb.save` fa il dump legacy: il primo esperimento (§4) ha spillato
  tutto perché non l'ha impostato. **Da evolvere:** far *tendere il router a instradare
  tutto* (steer di F., §3.1) — dare una casa curata anche alle regole (una topica per gli
  operatori, es. `kb/core/rules/…`), escludere i predicati runtime/`machinery` dallo spill
  (come `autolearn.py::persist_facts` già striscia a mano), e montare la mesh sul tree
  instradato così che ogni nodo ricarichi conoscenza *già organizzata*, con `session.p0`
  ridotto al solo residuo effimero.

- **[MESH-L3 · alto] Nessuna disciplina di merge / lock.** Due `kb.save` concorrenti sullo
  stesso file si sovrascrivono (last-writer-wins); la mesh regge solo in sequenza. **Da
  evolvere:** append+dedup invece di rewrite, oppure un merge transazionale del delta.
  Senza questo, una mesh *parallela* (il caso interessante) perde contributi.

- **[MESH-L4 · medio] Teacher e giudice sono lo stesso LLM.** Anello chiuso che corregge i
  propri compiti. **Da evolvere:** giudice indipendente (altro provider/istanza) e sonde
  held-out generate prima dell'insegnamento, così il punteggio misura un guadagno reale e
  non la coerenza interna del teacher.

- **[MESH-L5 · RISOLTO — era un artefatto della LEFT-recursion] `kb.match` sulle regole.**
  Il timeout di `kb.match is_a(dog,null)` NON era un limite del motore: era la closure
  **left-recursive** `p($X,$Z):-p($X,$Y),p($Y,$Z)` che manda l'SLD in loop. Con le closure in
  forma **right-recursive a predicato separato** (§4.4, SCOPERTA 1), `kb.match` enumera le
  risposte derivate senza impiantarsi — verificato: `kind_of_t(dog,?)`→mammal/animal/living_thing,
  `prop_of(dog,?)`→7 proprietà, `forebear(tom,?)`→tutti i discendenti, `able_to(eagle,?)`→fly/move.
  Questo è ciò che rende MESH-L1 insegnabile: il lettore dietro `answer_frame` **funziona** sugli
  operatori ben formati.

- **[MESH-L6 · da investigare a parte] `make test` è ROSSO su `main` (189/60), prima di
  questa sessione.** Le 60 failure sono `.it.chat` che attendono i lemmi italiani (uomo,
  cane, gatto, animale) mentre parrot0 emette i lemmi inglesi (man, dog, cat, animal) —
  es. `Learned rule: mortale(X) :- man(X)` vs atteso `:- uomo(X)`, e alcune catene IT che
  ora cadono in "Non capisco ancora". **Non causata da questa sessione** (`git diff src/`
  era vuoto quando l'ho misurato; il gate qui è stato "non aumentare il fail-count",
  rispettato). Sembra una regressione del canonicalizzatore IT→EN che non ri-mappa i lemmi
  in output. Da tirare come fronte suo, con un giro di learning/fix dedicato.

- **[MESH-L7 · proposta aperta di F.] Invertire il default di `is_internal_pred`.** Oggi:
  default **pubblico** (conta come conoscenza), si flagga l'**interno** (via `machinery/1`
  in KB, o la lista C di fallback). Il difetto che F. nota: quella lista è *conoscenza
  cablata nel C* e ogni substrato nuovo che si dimentica di flaggare **leaka** nel conteggio
  (già 4 volte). Proposta: **default interno, si flagga il pubblico.** *Analisi:* il
  `machinery/1` dichiarativo esiste già ed è la mossa KB-first (§4.3); ma l'inversione pura
  *per-nome* romperebbe il caso base — un fatto insegnato a runtime con predicato arbitrario
  (`dog(rex)`) DEVE contare, e non è pre-flaggabile. **L'asse pulito è l'ORIGINE, non il
  nome:** un fatto *asserito dall'utente in sessione* è pubblico; un fatto *caricato da un
  file-substrato* è interno di default. Così l'inversione elimina la lista C **senza**
  nascondere ciò che l'utente insegna. *Raccomandazione:* farlo come cambiamento a sé
  (tocca la semantica dell'introspezione e vari test), non dentro questa sessione; nel
  frattempo la disciplina `machinery(...)`-accanto-al-fatto (§4.3) già previene i leak.

- **[MESH-L8 · debito C, non KB] Parsing di frasi composte.** *"my dog is Rex and he is
  brown"* registra il colore ma **perde il nome** (poi *"what is his name?"* → "brown").
  L'atomico *"my dog is Rex"* funziona: il buco è lo split della congiunzione in due fatti.
  È meccanismo (C), non conoscenza — fuori dalla leva della mesh; da tirare come pull di
  colla linguistica ([[the-linguistic-glue]]).

- **[MESH-L9 · debito, scoperto dall'errore rumoroso] `KB_TERM_LEN=128` taglia i template
  lunghi.** `parse_term` rifiuta ogni argomento (virgolette incluse) più lungo di 128 char.
  L'errore di parse gen335 ha reso visibili **13 `response_template` morti** in
  `responses.p0`/`lexicon.p0` (spiegazioni di ~200-356 char), silenziosamente non caricati
  da sempre. **Da decidere (cambio a sé):** alzare `KB_TERM_LEN` (es. 384 — costo memoria su
  ~7000 fatti e sui buffer di stack, da verificare) oppure accorciare i template sotto i 128
  char. Nel frattempo l'errore appare a ogni boot: è corretto, segnala dati reali persi.

## 7. Teoria — perché una mesh può essere moltiplicativa

Un giro è additivo se congela *fatti*: N insegnamenti → N foglie. Diventa moltiplicativo
quando congela *operatori*: una regola n-aria è una funzione che, applicata a K fatti,
genera fino a Θ(chiusura) conclusioni. Per la transitività su un DAG di M nodi la chiusura
è O(M²). Ecco l'"aumento dell'ordine di complessità": **il costo marginale di una
conclusione derivata tende a zero una volta pagato l'operatore.**

La mesh amplifica questo in due dimensioni ortogonali:

1. **Spaziale (tra nodi).** Contributi indipendenti (`dog→mammal` da A, `animal→living_thing`
   da B) si compongono solo perché condividono il mount e l'operatore. Più nodi
   eterogenei → più archi indipendenti → chiusura più grande. La mesh è un modo per
   *raccogliere frontiere diverse sotto lo stesso operatore*.
2. **Gerarchica (ordini che si impilano).** Il prossimo giro non insegna un fatto: insegna
   un operatore che opera su ciò che l'operatore precedente ha reso derivabile (es. una
   regola che ragiona su `is_a` per inferire proprietà ereditate). Le abilità congelate
   diventano il substrato del livello successivo — è la catena "di ordine superiore dopo
   di che" richiesta.

Il vincolo di onestà che tiene tutto: **si moltiplica solo ciò che ha una proof.** Ogni
conclusione derivata è auditata da `kb.explain`; una regola che genera conclusioni non
dimostrabili (o false rispetto a un oracolo) va ritirata, non congelata. La mesh eredita
la disciplina no-deception di swe-bench/autolearn: *un guadagno non verificabile non è un
guadagno.*

## 8. Rotta

1. **Chiudi MESH-L1** (comprensione come conoscenza): un `answer_frame`-booleano che
   verbalizzi *Yes/No* su `kb.query(is_a, X, Y)`, così congelare `is_a/2` rende anche
   rispondibile *"is X a Y?"* — il caso sì/no del §4 portato in conversazione come già
   fatto per il lookup (§4.1).
2. **Il router homa anche gli operatori + montare sul tree instradato** (MESH-L2): una
   topica curata per le regole, i predicati runtime esclusi dallo spill, append/dedup sul
   mount (MESH-L3) — così il mount della mesh è conoscenza pura, organizzata, che
   sopravvive a nodi concorrenti.
3. **Giudice indipendente + probe held-out** (MESH-L4): trasforma questo harness da
   "auto-coerenza" a misura di guadagno.
4. **Secondo giro, un ordine più su:** congela un operatore che *ragiona sull'operatore
   precedente* (proprietà ereditate lungo `is_a`), e misura se la chiusura cresce come la
   teoria (§7) prevede.
5. **Automazione:** una volta stabile, la mesh è uno scheduled KB-growth process
   ([[autolearn-knowledge-is-official]]) — più nodi, un teacher, un giudice indipendente,
   conoscenza ufficiale committata solo quando la proof regge.

## 8.1 Cosa massimizza il risultato — lettura del LLMSCORE 6/10

> **La lezione del 6/10, corretta (gen335, su obiezione di F.):** gli operatori di §4.4 non
> hanno mosso il punteggio perché il giudice parla in NL e quegli operatori non *erano*
> rispondibili — ma **NON perché "raggiungibilità" sia un collo di bottiglia diverso dalla
> conoscenza.** La raggiungibilità **È conoscenza**: si insegna un `answer_frame` e
> l'operatore derivato risponde in NL (dimostrato, §6 MESH-L1). Avevo inquadrato male
> chiamandolo "il vero collo di bottiglia": contraddiceva la tesi. Non c'è un muro — c'è un
> frame da insegnare.

**Cosa serviva davvero (e non era un "consumer C"):** *(1)* le closure in forma
**right-recursive** (già fatto, §4.4) — così `kb_match`, il lettore dietro `answer_frame`,
enumera le risposte derivate senza impiantarsi (MESH-L5 era un artefatto della left-recursion,
risolto); *(2)* **insegnare i frame** che mappano la superficie NL all'operatore — pura
conoscenza. Fatto entrambi, "what can a dog do?" → "Move.", "properties of a dog" → le 7
ereditate, "ancestors of tom" → i discendenti. **Nessun C nuovo.**

**Mappa dei 4 punti persi → conoscenza di ordine superiore + lavoro di mesh:**

| Perso | Cosa serve (conoscenza di ordine superiore) | Lavoro di mesh |
|---|---|---|
| Q3 *"tre stati della materia"* (muro) | **operatore di ENUMERAZIONE/aggregazione**: "quali sono gli X" su una categoria (`aggregate_frame` esiste già). Generalizza a "nomina i pianeti / i colori primari / i continenti" — una classe intera con **un** operatore + fatti di appartenenza. **Il target a più alta leva.** | teacher congela `category_member/2` + il frame di enumerazione; giudice con probe held-out di "elenca gli X" |
| Q10 *problema dei treni* (muro) | **operatore word-problem/algebrico**: mappare una storia tasso-tempo-distanza a un'equazione e risolverla (`catch_up = head_start/(v2−v1)`). Gli schemi sono conoscenza, KB-first e rispondibili. | teacher congela gli schemi rate/time/distance come regole di riscrittura + il consumer che li risolve |
| Q7 *puzzle delle scatole* (non-sequitur "clockwork bird") | **il difetto peggiore, ed è ROUTING non conoscenza**: un modulo creativo ha reclamato un turno di logica. Serve disciplina di dispatch (colla linguistica) + un operatore di **soddisfazione di vincoli**. L'incoerenza è l'opposto di un LLM. | non è mesh-di-conoscenza: è un pull di [[the-linguistic-glue]] (il modulo giusto reclama il turno giusto) |
| Q2 *opening line di un giallo* (deviato) | **soffitto generativo onesto** (classe "genera" del [[kb-first-manifesto]]). Un banco di template di apertura (come `haiku_open`) lo renderebbe rispondibile, ma è il limite vero. | opzionale: teacher congela schemi di apertura narrativa; altrimenti si ammette |

**La forma di mesh che massimizza:** il teacher congela **operatore + il suo CONSUMER
insieme** (l'operatore da solo non si vede in conversazione), su predicati che un
`kb_match` sicuro sa enumerare; il giudice indipendente sonda le **classi** esposte dal
punteggio (enumerazione, word-problem, sì/no derivato), non domande singole. Priorità:
**(1)** `kb_match` sicuro sulle regole → **(2)** frame enumerazione (Q3, massima
generalizzazione) → **(3)** schemi word-problem (Q10) → **(4)** routing per Q7. Solo così
la conoscenza di ordine superiore già congelata *e* quella nuova diventano punteggio.

### 8.2 Correzione (steer di F.): il SOLVER è una PROCEDURA insegnabile, non un consumer C

> **Sopra ho sottovalutato [[teachable-procedures]].** Ho scritto "operatore + CONSUMER":
> ma il consumer scritto in C è proprio ciò che quel pivot (gen311) abolisce. F.: *«grazie
> al KB-first anche le procedure sono conoscenza — saper ignorare i dispari, fare la somma
> deve essere conoscenza, non motore C; in tutti i casi in cui ha fallito si doveva
> insegnare ampia conoscenza PROCEDURALE.»* Ha ragione, ed è già la rotta documentata.

**La conoscenza è fatti + PROCEDURE**, entrambe KB, entrambe insegnabili
([[teachable-procedures]] §0). E non è teoria: è **già parzialmente vivo** —
`apply_token_rewrite` interpreta regole `rewrite_es(LHS,RHS)` insegnate (grammar.p0), e
**la somma è già conoscenza in Peano**: `add(z,$N,$N). add(s($M),$N,s($R)):-add($M,$N,$R).`
(grammar.p0:124, `kb.query add(s(s(z)),s(z),s(s(s(z))))` → `provable:true`). Il motore SLD
calcola già da knowledge.

Quindi i fallimenti LLMSCORE si rileggono così — **niente consumer C, si insegna la
procedura**:

| Perso | La PROCEDURA da insegnare (non C) |
|---|---|
| Q10 treni | lo *schema* rate·time=distance e la regola di catch-up come **clausole/rewrite** (`catch_up_time`), non un modulo word-problem in C |
| Q3 enumerazione | il *fold* "raccogli tutti gli X membri di C" come **clausola ricorsiva** (come `add`/liste Peano), non un aggregate-consumer C |
| "ignorare i dispari" (esempio di F.) | un **filtro** ricorsivo sui numeri = clausola, non un ramo C |
| Q7 puzzle | la **deduzione a vincoli** come procedura di riscrittura/inferenza (stesso interprete di [[universal-solver]]) |

**Il caveat onesto (dove il C resta legittimo).** Il motore tiene i *primitivi* (unificazione,
SLD): quelli sì in C, come dice il manifesto. Oggi però **manca un primitivo di valutazione
numerica dentro le clausole** (un `is/2` alla Prolog): la computazione procedurale gira solo
in **Peano/successore**, impraticabile per numeri reali (60, 80, 120 miglia). L'aritmetica
reale vive ancora in `20-math.c` (un consumer C). Perciò l'abilitatore a più alta leva **non
è un consumer**: è **un solo primitivo generale** — valutazione numerica invocabile nel corpo
di una clausola — dopo il quale **ogni** procedura di calcolo (somme, tassi, confronti, filtri)
diventa conoscenza insegnabile, e `20-math.c` diventa migrabile a procedure insegnate. Un
primitivo (motore), non un consumer per compito: è la riga giusta del confine KB-first.

**Priorità corretta:** **(0)** il primitivo `eval`/`is` nel corpo delle clausole (motore,
generale) → **(1)** `kb_match` sicuro sulle regole (raggiungibilità) → **(2)** la mesh
insegna PROCEDURE (rate/time, fold-enumerazione, filtri) come regole `rewrite`/Horn, non
moduli C. Questa è "engine fixed, knowledge learns" alla sua **essenza massima**
([[teachable-procedures-pivot]]): il C smette di crescere in consumer, la conoscenza —
fatti *e* procedure — cresce sulla mesh.

### 8.3 Fatto (gen335): il primitivo `is/2` + le procedure astratte + lo scatter

Il passo **(0)** è **implementato**. `src/kb.c` ora ha i primitivi di valutazione nel corpo
delle clausole: `is($R, Expr)` (`add/sub/mul/div/mod`, annidabili, su numeri **reali**) +
i confronti `lt/le/gt/ge/eq/ne` (valutano entrambi i lati). Nomi riservati come `chars`/`naf`.
Da qui le procedure si insegnano come clausole — dimostrato dal vivo, il **più astratto
possibile** (non "i treni"):

- **`even/odd`** (l'esempio di F.): `even($X):-eq(mod($X,2),0)` — "ignorare i dispari" è conoscenza.
- **fold**: `sum_list`, `count_list`, `max_list` — aggregazione su lista. `sum_list([3,4,5])→12`.
- **filtro**: `drop_odds([1,2,3,4])→[2,4]` — map/filter astratto.
- **relazione lineare ASTRATTA**: `product(A,B,C)`/`factor(C,B,A)` (A·B=C in ogni direzione) —
  copre tasso·tempo, prezzo·quantità, ecc.; e `catch_up(vantaggio,v1,v2,T)` generale
  (`catch_up(120,60,80)→6`), **non** una regola sui treni.

Vivono in `kb/core/procedures.p0` (nuovo home caricato al boot). Il primitivo è un **motore**
generale: `20-math.c` (aritmetica in C) diventa migrabile a procedure insegnate.

**Raggiungibilità in NL — chiarita (gen335):** gli operatori/procedure a **token-singolo**
sono già rispondibili insegnando un `answer_frame` (dimostrato, §6 MESH-L1; `kb_match` sulle
closure right-recursive enumera le risposte derivate, MESH-L5 risolto). L'unico residuo, e
**non è un muro**, è il *word-problem multi-slot* (estrarre più quantità dalla prosa e legarle
agli argomenti di una procedura): serve un frame più espressivo del single-token, ma è ancora
conoscenza KB ([[universal-input]] `segment_role`), non un consumer C.

**Scatter (steer di F.):** `kb/core/mesh-knowledge.p0` è stato **sciolto** nei file dei
parenti così il save-map instrada la crescita futura accanto: tassonomia + ordinamenti +
geografia + paternità → `world-facts.p0` (kin: `is_a`, `answer_frame`, `capital_of_country`);
parentela → `social.p0` (kin: `family_relation`); nomi propri → `presentation.p0`; procedure
→ `procedures.p0`. Restano aperti **(1)** `kb_match` sicuro (raggiungibilità NL degli
operatori) e il **routing NL** dei word-problem verso le procedure.

> **Bonus dell'errore rumoroso:** lo scatter ha fatto ripassare `world-facts.p0` dal parser
> maturato, che ha scovato **un altro** difetto silenzioso preesistente — un commento in
> stile C `/* … */` (il char di commento `.p0` è `%`) su `sound_of(seal,bark)`. Corretto.
> Sesta perdita silenziosa intercettata da quando l'errore è rumoroso.

## 9. Collegamenti

[docs/prolog-like-engine.md](../prolog-like-engine.md) (§5 — insidie di sintassi + fact
routing), [docs/use-mcp-engine.md](../use-mcp-engine.md) (il canale MCP),
[[kb-first-manifesto]], [[universal-comprehension]] (la comprensione è conoscenza, §4.1),
[[teachable-comprehension-answer-frame]] (`answer_frame/2`),
[[autolearn-knowledge-is-official]], [[universal-input]], [[the-linguistic-glue]],
[[generative-prolog-manifesto]], [[rulescore-harness]], [[llmscore-harness]].

## 10. Secondo giro di addestramento — ordine superiore (gen335, round 2)

> **Eseguito dal vivo 2026-07-16.** 4 batch di costrutti astratti: ereditarietà,
> default reasoning con `naf`, composizione relazionale, procedure matematiche.

### 10.1 Nuove scoperte da tenere in EVIDENZA

1. **`kb.assert_clause` MCP non supporta nested expressions (gen335).** `naf(goal)`,
   `is($R, expr)`, `cons($H,$T)`, e confronti con espressioni composte
   (`eq(mod($N,2),0)`) restituiscono `{"ok":false}` silenzioso via MCP. **Questi
   costrutti vanno scritti direttamente nei file `.p0`**, dove il parser li gestisce
   correttamente. Il motore li esegue; è l'adattatore JSON→termine che non li
   serializza. Dettaglio in [prolog-like-engine.md §5.1](../prolog-like-engine.md).

2. **`ne/2` è solo numerico — non esiste `dif/2` builtin (gen335).** `ne(luca, sofia)`
   fallisce perché gli atomi non sono espressioni numeriche. `naf(eq(X,Y))` non è
   parsabile via MCP (vedi punto 1). Per clausole caricate da `.p0`, `naf(eq(X,Y))`
   funziona ma richiede che $X e $Y siano ground. **Non esiste una `dif/2` builtin.**
   Il workaround pragmatico: omettere il check di diseguaglianza e accettare falsi
   positivi (`sibling(X,X)`).

3. **`PARROT0_WORLD_FACTS=0` salta `world-facts.p0` (gen335).** La variabile
   d'ambiente è usata per "learning from empty world". Nei test senza questo flag,
   la conoscenza scatterata nei file curati viene caricata normalmente (5188 fatti).

4. **`answer_frame` matching è lessicale, non semantico (gen335).** Il cue deve essere
   un token esatto nella frase canonicalizzata. "how tall" non matcha "height"; "longest
   river" non matcha "longest_river". Frame funzionanti: definition, largest, founded,
   born, symbol, capital, currency, continent, population, author, discovered, invented,
   color, sound, language, sides, prevents. Frame non funzionanti per questo motivo:
   height, longest_river, tallest_mountain, speed, age, requires, causes.

### 10.2 Costrutti di ordine superiore insegnati

| Categoria | Operatori | Verificato |
|-----------|-----------|------------|
| Property inheritance | `inherits(X,P) :- is_a(X,C), has_prop(C,P)` + varianti transitive | dog→warm_blooded (9 proprietà ereditate) |
| Inverse relations | `child_of`, `smaller_than`, `lighter_than`, `slower_than`, `south_of`, `west_of`, `owned_by` | Tutti verificati |
| Kinship composition | `uncle_of`, `aunt_of`, `nephew_of`, `niece_of`, `cousin_of` | uncle/nephew verificati |
| Directional spatial | `north_of_t`, `east_of_t` (transitivi) + inverse | sweden→italy (3 hop) |
| Containment | `contains_t` (transitivo), `contained_in` (inverso) | library→page (2 hop) |
| Default reasoning | `can_fly(X) :- bird(X), naf(flightless(X))` | sparrow→true, penguin→false |
| Non-monotonic birth | `gives_birth(X) :- mammal(X), naf(egg_laying(X))` | dog→true, platypus→false |
| Relational composition | `lives_in`, `diet` (join is_a + habitat/eats) | salmon→water, dog→land |
| Temporal Allen | `period_contains_t`, `happened_during` | moon_landing→modern_era |
| Math procedures | `factorial`, `gcd`, `lcm`, `is_prime`, `power`, `is_even`, `is_odd` | 5!=120, gcd(12,8)=4, is_prime(11) |
| List ops | `member` | — |

### 10.3 Il pattern è confermato

Il secondo giro conferma la tesi del §7: un operatore di ordine superiore (es.
`inherits`) insegnato **una volta** moltiplica ogni nuovo `has_prop` e ogni nuovo
`is_a` in conclusioni derivate. 24 `has_prop` + il bridge category_member→is_a +
180 membri di categoria = ogni animale eredita automaticamente le proprietà della
sua classe. Il costo marginale di una conclusione tende a zero.

Il limite onesto è il **canale di insegnamento**, non il motore: `kb.assert_clause`
MCP copre solo clausole Horn pure; nested expressions, `naf`, `is/2` e liste
richiedono il canale `.p0`. Finché l'adattatore JSON→termine non supporta
argomenti nested, la via `.p0` resta il canale principale per la conoscenza di
ordine superiore.

## 11. Terzo giro — il teacher massimizza archi sul corpus reale (gen365)

> **Eseguito dal vivo il 2026-07-28.** Questo giro applica la correzione di
> [the-model-plan](the-model-plan.md): il target non è il numero di predicati o
> fatti ma il numero di dipendenze che compongono fatti già presenti.

### 11.1 Round teacher e promozione

Il teacher ha prima asserito via MCP i candidati su una normale istanza, con
sonda rossa prima e verde dopo. Solo i candidati che hanno retto query,
controesempio e — dove il proof engine lo consente — `kb.explain` sono stati
promossi nei file dei parenti:

- storia → `kb/facts/history.p0`;
- geografia → `kb/facts/geography-world.p0`;
- scienza → `kb/facts/science-nature.p0`;
- relazioni lessicali → `kb/facts/vocabulary-extra.p0`.

Risultato misurato da `scripts/kb_graph.py`: clausole mondiali effettive
**50→85 (+70%)**, machinery **192→192**, righe C **invariate**; i predicati
consumati in un corpo passano **123→148**. Dettaglio e proof in
[the-model-plan §10](the-model-plan.md#10-prima-sessione-operativa--misurare-e-collegare-il-corpus-gen365).

### 11.2 Freeze→propagate reale A→B

Il round successivo è stato eseguito con due processi persistenti e lo stesso
mount effimero:

```
B prima:  shared_color(banana, lemon, yellow)  → false
A teach:  shared_color($X,$Y,$C) :-
              color_of($X,$C), color_of($Y,$C)
A proof:  ... because color_of(banana,yellow)
                    and color_of(lemon,yellow)
A save:   mount condiviso, 8 righe
B prima del restore: false
B restore: 11.873 clausole
B dopo:   stessa proof → true
```

La conclusione ground non compariva nel mount: vi era soltanto la regola
generale, oltre a sette fatti runtime. Dopo la verifica il mount effimero è
stato rimosso e l'operatore è stato promosso nel file scientifico curato.

### 11.3 Il controllo deve arrivare al prompt

Su richiesta di F. il gate non è l'intera suite, ma sonde puntuali sui prompt
che devono consumare la conoscenza appena inserita:

```
"what continent is paris in?"
  senza frame  → muro
  assert answer_frame(continent, capital_in_continent)
               → "Europe."
  retract      → muro

"what language is associated with paris?"
  senza frame  → muro
  assert answer_frame(language, capital_language)
               → "French."
  retract      → muro

"moon orbit"
  senza frame  → muro
  assert answer_frame(orbit, orbits_t)
               → "Earth and sun."  (closure a due hop)
  retract      → muro
```

Questo distingue tre risultati che non vanno confusi: la clausola è
**provabile**, porta una **proof**, ed è **raggiungibile da un prompt** tramite
comprensione insegnata a runtime.

### 11.4 Limite emerso e chiuso; residuo di routing

Il primo tentativo col cue nuovo `orbit → orbits_t` non veniva visto:
`mod_answer_frame` materializzava solo 128 cue e la registry attuale è più
grande. **Chiuso nello stesso round:** cue e predicati candidati ora vengono
enumerati con `kb_match_all`, senza cap scelto dal consumer. La terza sonda del
§11.3 è il gate rosso→assert→verde→retract→rosso oltre il vecchio elemento 128.

Il prompt completo *“what does the moon orbit?”* resta invece oscurato da un
consumer precedente che lo interpreta come introspezione sui moduli. La forma
stretta *“moon orbit”* raggiunge l'operatore e restituisce la closure. Quindi il
limite di enumerazione è risolto; il residuo è **routing/dispatch**, la stessa
conoscenza cablata nella posizione del registry diagnosticata da
[the-model-plan §5](the-model-plan.md#5-il-motore-c-minimale).

## 12. Playbook operativo: dal prompt perso al sottografo insegnabile

Questa sezione è deliberatamente prescrittiva. Serve a un coding agent che non
veda spontaneamente la differenza fra «aggiungere una buona risposta» e
«insegnare la struttura che rende deducibili molte buone risposte».

> **Rettifica gen366, dopo il tail remoto 0/20.** La prima versione di questo
> playbook fermava il frasario al livello sbagliato. Spezzare un paragrafo in
> `claim_edge(S,R,O)` rende il contenuto interrogabile, ma non lo rende dedotto.
> Se `S-R-O` è già la conclusione necessaria a uno dei prompt di training, il
> sistema continua a scegliere una risposta scritta da noi: ha soltanto una
> serializzazione più pulita. Le sezioni seguenti sostituiscono quel criterio.

### 12.1 La regola che evita il frasario

> **Una risposta non è conoscenza. È una vista verbalizzata su un sottografo.**

La regola vieta due forme, non una:

```prolog
% Vietato: risposta intera.
claim_text(alien_strategy,
  "Look for recurring units ... then replay purified signals ...").

% Vietato come prova di ragionamento: la stessa risposta, tagliata in pezzi.
reasoning_edge(alien_chemical_language, sampling, alien_sampling_1).
claim_edge(alien_sampling_1,
           "Signal sampling",
           should_use,
           "chromatography, mass spectrometry, timing, location, and response").
reasoning_edge(alien_chemical_language, experiments, alien_experiment_1).
claim_edge(alien_experiment_1,
           "Controlled replay",
           should_test,
           "one component, order, concentration, and composition at a time").
```

La seconda forma è migliore come storage, ma `reasoning_claim_candidate/5`
esegue soltanto una join: non produce alcuna proposizione che non fosse già
presente in `claim_edge/4`. **Atomicità è organizzazione, non inferenza.**

La forma cercata separa fatti osservabili e trasformazioni riusabili:

```prolog
% Fatti del mezzo: veri anche senza il prompt LLMSCORE.
observable_dimension(chemical_signal, compound_identity).
observable_dimension(chemical_signal, temporal_order).
observable_dimension(chemical_signal, concentration).
manipulable_dimension(chemical_signal, compound_identity).
manipulable_dimension(chemical_signal, temporal_order).
manipulable_dimension(chemical_signal, concentration).
transport_risk(chemical_signal, diffusion).
transport_risk(chemical_signal, persistence).

% Regole generali: valgono anche per suono, gesti, luce o radio.
candidate_code_dimension($Carrier, $Dimension) :-
    observable_dimension($Carrier, $Dimension),
    manipulable_dimension($Carrier, $Dimension).

discriminating_intervention($Carrier, $Dimension, controlled_replay) :-
    candidate_code_dimension($Carrier, $Dimension).

decoding_confound($Carrier, $Risk) :-
    transport_risk($Carrier, $Risk).
```

Ora `controlled_replay` e i limiti sono conseguenze del modello del carrier e
di regole che non nominano alieni né il benchmark. La lingua realizza la proof
alla fine. I fatti di dominio restano necessari: il ragionamento non può
inventare che una sostanza diffonde. Ma un fatto terminale non va contato come
una regola.

**Test di review:** nascondere il nome del prompt e chiedere «quale conclusione
nuova produce questa clausola?». Se la risposta è «nessuna, recupera una tripla
già scritta», non è un arco di ragionamento.

### 12.2 Prima diagnosi: classificare lo zero

Per ogni riga LLMSCORE persa, copiare in una scheda:

```text
PROMPT:
RISPOSTA:
VERDETTO:
TEMPO LOCALE:
CONSUMER/PERCORSO:
```

Poi assegnare **una causa primaria**, senza correggere ancora:

| Sintomo | Diagnosi probabile | Prima mossa |
|---|---|---|
| timeout locale | percorso troppo costoso o loop di candidati | cronometrare le fasi; nessuna conoscenza nuova |
| muro | nessun atto, topic o consumer raggiungibile | trovare l'arco di comprensione mancante |
| risposta fluente ma generica | atto trovato, nessun operatore ha prodotto conclusioni | cercare fatti di mondo e regole trasferibili; non scrivere il claim finale |
| fatto corretto ma compito non svolto | semantic lookup ha preceduto proof/design/format | correggere act e routing |
| contenuto giusto, formato sbagliato | piano/faccetta/formato incompleto | estendere shape o realizer |
| risposta di un altro tema | cue largo senza gate discriminante | aggiungere o stringere `topic_gate` |

Il **verdetto del judge** è evidenza diagnostica. Frasi come “generic templated
response”, “never addresses X” e “merely states the theorem” indicano difetti
diversi. Non vanno tutti curati aggiungendo più facts.

### 12.3 Scomporre il prompt in una Task IR, non in una risposta ideale

Non compilare colonne `output facets` e `claim da dire`: è il percorso che ha
prodotto il fit 22/22 e il tail 0/20. Estrarre invece soltanto ciò che il turno
fornisce o richiede:

| Campo IR | Domanda | Esempio astratto |
|---|---|---|
| **operazione** | quale trasformazione è richiesta? | confrontare, spiegare, progettare, comporre |
| **deliverable** | quale artefatto deve esistere alla fine? | scelta motivata, proof, procedura, poema |
| **argomenti** | su quali entità/processi opera? | X e Y; causa ed effetto; mezzo e ricevente |
| **premesse** | quali fatti o controfattuali dà il prompt? | tempo non lineare; niente diagrammi |
| **vincoli** | che cosa è vietato/obbligatorio? | 280 caratteri; tre portate; parole escluse |
| **criterio** | come si riconosce una soluzione riuscita? | distingue X/Y; rispetta risorse; predice un esito |

Forma concettuale:

```text
task(Id,
     operation(Op),
     deliverable(Type),
     arguments(Args),
     premises(Premises),
     constraints(Constraints),
     success(Criteria))
```

La superficie che riempie questi ruoli resta KB-first. Il motore può
tokenizzare e legare slot; `compare`, `without`, `why`, `under an hour` e le
loro parafrasi sono conoscenza insegnabile. Il topic serve a recuperare fatti,
non a scegliere una risposta privata.

La pipeline target è:

```text
prompt -> Task IR -> operator schema -> subgoal
                         |
                         v
              fatti del mondo + regole
                         |
                         v
                 proof / candidati
                         |
                         v
              verifica -> realizzazione
```

### 12.4 Come trovare le regole, senza distillare un LLM

Partire dal verbo dell'operazione, non da una risposta di riferimento.

1. Scrivere la **precondizione**: quali fatti rendono applicabile l'operatore?
2. Scrivere l'**effetto**: quale nuovo goal, candidato o relazione produce?
3. Separare i fatti del dominio dalla trasformazione. La regola deve avere
   variabili nei ruoli che cambiano fra domini.
4. Cercare almeno tre mondi non correlati in cui la stessa trasformazione abbia
   senso.
5. Solo dopo aggiungere i fatti mancanti di ciascun mondo.
6. Derivare una proof; il renderer non può introdurre una proposizione assente
   dalla proof.

Scheletri iniziali, volutamente indipendenti dal topic:

```prolog
% Confronto: scopre dimensioni diverse; non contiene sonetti o haiku.
difference($X, $Y, $Dimension, $VX, $VY) :-
    property($X, $Dimension, $VX),
    property($Y, $Dimension, $VY),
    ne($VX, $VY).

% Scelta condizionata allo scopo.
satisfies_goal($Candidate, $Goal, $Dimension) :-
    goal_requires($Goal, $Dimension, $Threshold),
    property($Candidate, $Dimension, $Value),
    meets($Value, $Threshold).

% Spiegazione causale: la conclusione può attraversare più fatti.
explains($Cause, $Effect) :-
    causes_t($Cause, $Effect).

% Esperimento discriminante: varia una causa e predice una differenza.
discriminates($Intervention, $H1, $H2, $Observation) :-
    predicts($H1, $Intervention, $Observation),
    predicts_not($H2, $Intervention, $Observation).

% Procedura: un passo è eseguibile soltanto quando le precondizioni tengono.
ready_step($Step, $State) :-
    requires_state($Step, $Required),
    holds($State, $Required).
```

Per composizione creativa la deduzione non basta da sola: serve ricerca
vincolata. Ma anche qui il sistema non deve conservare il poema finale:
`candidate_fragment` nasce da associazioni/sensazioni note,
`violates(Fragment, Constraint)` lo elimina, un ordinatore costruisce
l'artefatto e un oracolo controlla lunghezza, parole vietate e forma.

Un agente non deve chiedere a un LLM «qual è la buona risposta?» e poi
atomizzarla. Può usarlo per proporre **ipotesi di operatori**, ma accetta
l'ipotesi soltanto se passa il test cross-domain e produce conclusioni nuove.

### 12.5 Riutilizzare prima di creare

Ordine obbligatorio:

1. cercare un **operatore** già esistente;
2. cercare clausole che ne producano i subgoal o i candidati;
3. cercare fatti del mondo già disponibili per gli argomenti;
4. aggiungere fatti mancanti senza formulare la conclusione del prompt;
5. usare shape e frame soltanto dopo che esiste una proof completa;
6. creare un nuovo operatore solo se precondizioni/effetti sono realmente
   diversi;
7. creare C solo dopo aver dimostrato che la meccanica generale non è
   esprimibile o non rispetta il budget.

Esempio: compleanno e teorema di Pitagora riusano entrambi
`proof_exposition`. Non servono `birthday_answerer` e
`pythagorean_answerer`: cambiano i quattro archi della prova, non la procedura
“contesto → costruzione → invariante → conclusione”.

Al contrario, una palette sinestetica non è una proof. Ha una shape propria
`palette → mapping → rationale`; forzarla dentro `design_analysis` genera
proprio il boilerplate bocciato dal judge.

### 12.6 Cue e gate: evitare overfitting e collisioni

Un cue non deve essere l'intera domanda. Deve nominare una classe insegnabile:

```prolog
strategy_cue(experiment_design_strategy, "design an experiment").
topic_evidence(dog_tomorrow_topic, keyword(tomorrow)).
topic_gate(dog_tomorrow_topic, keyword(tomorrow)).
```

Pratiche:

- preferire 2–4 evidenze indipendenti a una frase completa;
- usare un gate raro che debba comparire davvero;
- aggiungere una parafrasi tenuta fuori dal cue come prova di generalizzazione;
- aggiungere un vicino negativo: `dog + immediate reward` senza `tomorrow` non
  deve autorizzare l'esperimento temporale;
- se apostrofi o punteggiatura spezzano una phrase cue, aggiungere un
  `keyword(...)` semanticamente corretto, non la domanda completa;
- ogni nuovo recognizer deve avere assert → verde → retract → rosso sullo
  stesso binario.

Questi test dimostrano crescita della **comprensione superficiale**, non
trasferimento del ragionamento. Una parafrasi che colpisce lo stesso topic
resta nello stesso dominio; non sostituisce il terzo mondo held-out del §12.8.

### 12.7 Quando il C è ammesso

Il C può implementare soltanto operazioni che restano identiche cambiando
lingua, topic e dominio:

- enumerare candidati;
- eseguire join e unificazione;
- ordinare faccette;
- verificare completezza;
- riempire slot da frame KB;
- fare caching/materializzazione;
- applicare limiti di tempo e memoria.

Prima di modificare C, la scheda deve contenere:

```text
GRAFO COMPLETO?                 sì/no
QUERY DIRETTA PROVABILE?        sì/no
PROMPT RAGGIUNGE IL GRAFO?      sì/no
COSTO MISURATO PER FASE:
MECCANICA MANCANTE:
ALMENO 3 DOMINI FUTURI SERVITI:
LETTERALI LINGUISTICI IN C:     devono essere zero
```

Il round gen365 fornisce un esempio ammesso **solo di ottimizzazione dei join e
di separazione KB/C, non di ragionamento generalizzante**. Le clausole che derivavano
registri caldi e non vincolati erano corrette, ma la sola enumerazione di
`analysis_act_cue` derivato costava circa **736 ms**, prima di topic, piano e
rendering. Il fix C non contiene `library`, `Möbius`, `tomorrow` o altre parole:
esegue direttamente i join generali
`strategy_cue→strategy_act`, `reasoning_topic→topic_evidence`,
`strategy_shape→shape_facet` e `reasoning_edge→claim_edge`. Le clausole
equivalenti restano interrogabili con `kb.explain`. I prompt specifici scendono
a circa **90–120 ms**.

Un ramo C che riconosce `birthday`, `chemical signals` o `between` è invece
vietato: quella è conoscenza. Anche un `printf` con la risposta resta vietato.

### 12.8 Protocollo di sessione per il teacher

Ogni sessione deve lasciare questa sequenza:

1. **FREEZE:** conservare il prompt perso come eval; non leggere o scrivere la
   sua risposta ideale durante lo sviluppo.
2. **TRACE:** annotare Task IR, consumer e tempo; non editare.
3. **OPERATOR:** scegliere una trasformazione con precondizioni ed effetti.
4. **TRAIN WORLDS:** costruire due o più casi indipendenti che non appartengono
   al topic LLMSCORE.
5. **HELD-OUT WORLD:** scegliere prima un terzo mondo non usato per inventare
   la regola.
6. **TEACH:** aggiungere clausole generali e soli fatti di mondo.
7. **PROVE:** ottenere una conclusione assente dai fatti terminali.
8. **ABLATE RULE:** ritrarre la regola deve rompere tutti i mondi; ripristinarla
   deve recuperarli.
9. **ABLATE FACT:** ritrarre un fatto locale deve rompere soltanto il suo mondo.
10. **GROWTH:** assert/retract di una surface cue prova KB-first, non capacità
    di ragionamento.
11. **FROZEN PROMPT:** soltanto ora rieseguire il prompt reale e una parafrasi.
12. **PROMOTE:** promuovere l'operatore solo se migliora il caso tenuto fuori.
13. **LOG:** registrare transfer fan-out, proof, C, tempi e limiti onesti.

Non eseguire l'intera suite per una sessione di teaching. Usare il ratchet
puntuale; la suite completa appartiene al gate di integrazione successivo.

### 12.9 Organizzazione dei file e naming

- Lessico e surface evidence: `kb/core/intents.p0` oppure il file tematico che
  possiede la strategy.
- Procedure e shape riusabili: `kb/core/procedures.p0`.
- Fatti di dominio: file del dominio sotto `kb/facts/`; non creare un file
  per la domanda o per la sua risposta.
- Operatori e clausole cross-domain: `kb/core/procedures.p0` o un file di
  reasoning comune, mai il file del benchmark.
- Relazioni di rendering corte: presentation/procedure knowledge, mai C.
- Test del corpus perso: file indirizzabile dedicato sotto `tests/`.

Nomi consigliati:

```text
<facoltà>_strategy
<facoltà>_shape
<tema>_topic
<tema>                        % domain
<tema>_<facet>_<n>            % edge id
```

Non chiamare un dominio con il numero del benchmark (`question_19`) e non usare
hash o intere frasi come identità. Il nome deve spiegare quale capacità potrà
riusare un prompt futuro.

### 12.10 Checklist di consegna

Un agent non dichiara chiuso il round finché non può rispondere:

- Quale verdetto preciso sto correggendo?
- Quale Task IR è stata estratta senza anticipare la risposta?
- Quale operatore, con quali precondizioni ed effetti, è stato scelto?
- Quale nuova conclusione produce ogni clausola?
- La clausola è stata inventata e verificata su almeno tre domini indipendenti?
- La risposta finale esiste in qualche singolo fatto? Deve essere **no**.
- Le sue proposizioni decisive esistono già come `claim_edge` terminali? Per
  rivendicare ragionamento deve essere **no**.
- Posso mostrare una proof della procedura?
- Posso insegnare e ritrarre una superficie a runtime?
- Il prompt esatto termina entro il budget?
- Un vicino negativo resta fuori?
- Ogni riga C è meccanica generale e priva di vocabolario?
- Quanti archi nuovi consumano conoscenza prima inerte?

Se una risposta manca, il lavoro non è pronto: non compensare con altra prosa.

### 12.11 Esperimento gen365, poi falsificato fuori campione

La prima versione del playbook è stata applicata alle 19 righe a zero del
report LLMSCORE 2026-07-27:

- 12 strategie/atti collegati a 11 shape;
- 19 topic e 19 gate;
- 82 `reasoning_edge` e 82 `claim_edge`;
- quattro clausole di audit/join;
- zero `claim_text` e zero risposte finali memorizzate.

`make llmscore-arcs` esegue le 19 domande esatte, due controlli di crescita
runtime e una proof del piano. Stato al termine del round: **22/22**. Il tail
remoto immediatamente successivo, composto da venti temi nuovi, ha ottenuto
**0/20**. Quindi 22/22 misura il fit sul corpus visto, non il trasferimento.

Audit della struttura:

| componente gen365 | quantità | classificazione corretta |
|---|---:|---|
| `strategy_cue` + `topic_evidence` + gate | 122 | routing/comprensione |
| `shape_facet` | 43 | organizzazione retorica |
| `reasoning_edge` + `claim_edge` | 164 | indice + conclusioni già scritte |
| `relation_frame` | 32 | presentazione |
| clausole | 4 | join/proiezione, nessuna conclusione nuova |

Il nome `reasoning_edge` non rende deduttivo un arco. Questo batch resta utile
come corpus diagnostico e come prova del renderer KB-first, ma **non va esteso
con i venti nuovi prompt** e non conta verso la massa critica di ragionamento.

### 12.12 Metriche che non possono essere vinte atomizzando risposte

Ogni nuovo operatore riporta almeno:

- **transfer fan-out:** quanti domini tenuti fuori migliora una sola regola;
- **novel conclusion rate:** quante proposizioni della proof non esistono come
  fatti terminali;
- **proof depth:** numero di trasformazioni, esclusi routing e rendering;
- **ablation fan-out:** rimuovere la regola rompe più domini, rimuovere un fatto
  rompe solo il dominio che lo usa;
- **prompt leakage:** frasi/cue/entità presi dal benchmark nel file
  dell'operatore; deve essere zero;
- **latenza:** prompt isolato e, separatamente, carico concorrente.

Un incremento di `claim_edge` senza aumento di transfer fan-out è
arricchimento di conoscenza o storage; può essere legittimo, ma non è un
avanzamento delle regole di ragionamento.

### 12.13 Correzione gen367: astrai fino al punto fisso

Il primo R3 superava l'ablazione ma non il criterio più importante. La regola
ordinava `process_action`, `action_requires` e `action_produces`, però ogni
passo veniva poi letto da:

```prolog
action_instruction(mash_grain,
  "Hold crushed malt in measured hot water ...").
```

È ancora distillazione: il payload è una riga della risposta. Chiamarlo “fatto
atomico” non cambia la sua capacità di ricombinazione, che è zero. Anche un
ratchet 28/28 può quindi essere tautologico: prova che una frase registrata
viene recuperata, non che la procedura viene costruita.

Il mantra operativo diventa un gate esplicito:

> **Astrai fino al punto fisso.** Se due predicati differiscono soltanto per il
> verbo o per il topic, sono la stessa relazione con un campo diverso.

Esempi:

```prolog
% NO: un predicato per etichetta
brewed(beer, brewer).
assembled(table, worker).
deployed(release, agent).

% SÌ: verbo e ruolo sono dati
action_semantics($Action, $Verb, $Patient).
product_input($Product, $Input, $Role).
action_consumes($Action, $Input).
action_produces($Action, $State).
```

“Birra” non autorizza un template birra. È un prodotto con input (acqua,
malto, luppolo, lievito, attrezzatura), trasformazioni, precondizioni, effetti e
parametri. Lo stesso calcolo deve coprire un mobile, una release software e un
pasto. La clausola `process_input_covered/4` deriva quale azione consuma ciascun
input; il planner rifiuta il piano se un input dichiarato resta scoperto. La
superficie è composta da verbo, paziente, parametri e stato prodotto. Nei mondi
del nuovo operatore non è ammesso `action_instruction/2`; resta solo come
fallback legacy per strutture secondarie non ancora migrate.

Il punto fisso vale anche all'ingresso. Una `Task IR` che lega un argomento solo
quando esiste `task_entity_cue(Entity, Form)` è un lessico chiuso travestito da
parser. Le cue note devono risolvere alias, non definire l'universo degli
argomenti. Il motore deve poter delimitare uno span nuovo tramite marcatori
grammaticali KB-first, canonizzarlo come entità locale del turno e proiettarlo
nella IR senza rebuild. La conoscenza di mondo potrà mancare; l'argomento non
deve mancare.

Nuovi controlli obbligatori per ogni operatore:

1. **payload audit:** nessuna frase che realizzi da sola un passo o una
   conclusione nel file dei mondi di controllo;
2. **verb-field audit:** verbi paralleli sono valori di una relazione comune,
   non nuovi predicati;
3. **input coverage:** ogni ingrediente/componente/risorsa dichiarato è
   consumato da almeno un'azione provabile;
4. **novel-span probe:** un'entità inventata dopo il build compare correttamente
   nella Task IR anche se non ha ancora fatti di mondo;
5. **separate gaps:** IR presente + nessun piano significa gap di conoscenza;
   IR assente significa gap di comprensione. Non confonderli con un template
   generico.

### 12.14 Checkpoint gen367: dall'entità censita al concetto composto

La correzione è ora eseguibile in `make reasoning-operators`:

- la Task IR delimita argomenti nuovi con `task_span_pattern/4` e
  `task_boundary_cue/2`;
- `zorbium_cup`, `flaxen_flask`, `gps_receiver` e
  `repotting_an_orchid` entrano nella IR senza `task_entity_cue`;
- ogni termine viene scomposto in n-grammi concettuali riflessivi tramite
  `task_term_concept/2`;
- `effective_property` ed `effective_goal_prefers` fanno ereditare a un composto
  soltanto fatti espliciti dei suoi concetti;
- `effective_system_relies_on` ed
  `effective_phenomenon_exploits` applicano lo stesso meccanismo alle
  spiegazioni di vulnerabilità;
- `example_event(Example, Subject, Relation, Object)` sostituisce
  `example_observation` nei mondi dell'operatore: anche l'esempio è composto da
  campi, non letto come frase terminale.

Questo produce trasferimento che il lessico chiuso non poteva dare:
`linen_shirt` eredita l'isolamento di `linen`, `wool_coat` quello di `wool`;
`paper_carton` e `plastic_crate` vengono confrontati per protezione dalla
pioggia; `gps_receiver` e `multipath_interference` attivano il meccanismo di
ranging tramite i concetti `gps` e `multipath`. Nessuna delle entità composte è
un fatto del mondo o una cue.

R4 porta lo stesso principio dalla scelta alla costruzione. La sintesi sotto
vincoli non possiede template “notifica”, “rete”, “subacqueo” o “magnetico”.
Deriva:

```prolog
task_candidate(Context, Feature)
task_requirement(Context, Dimension, Value)
task_feature_match(Context, Feature, Dimension, Value)
```

da `candidate_for`, `goal_prefers`, `property` e dai concetti del turno. Il
consumer seleziona un insieme di feature soltanto se la loro unione copre ogni
requisito. Le feature sono realizzate con lo stesso
`action_semantics(Feature, Verb, Patient)` usato dalle procedure: il verbo resta
un campo. Notifiche silenziose, reti a connettività intermittente, navigazione
subacquea e segnali magnetici attraversano un'unica trasformazione.

Il ratchet puntuale è **43/43**: include proof, ablazione multi-dominio,
ablazione di un input locale e crescita runtime sia delle cue operative sia dei
separatori grammaticali. Non è ancora una prova di 20/20 LLMSCORE: causalità
controfattuale, argomentazione e composizione creativa restano famiglie
scoperte. Il valore del checkpoint è che una nuova entità non azzera più il
parser né le relazioni ereditabili.
