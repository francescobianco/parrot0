te# Learning Mesh — catene di addestramento su una KB condivisa

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
- **L'indice `<root>/savemap.tsv`** (4 colonne: `predicato ⇥ token ⇥ file ⇥ riga`) è
  **ricostruito a ogni save** scandendo il tree (transiente → mai stale) e scritto su
  disco per ispezione.
- **Opt-in via `PARROT0_KB_ROOT`.** Senza, `brain_save_session` fa il vecchio save a file
  singolo — motivo per cui il primo esperimento (§4) ha *spillato tutto* nel mount invece
  di instradarlo. I test ermetici restano così di proposito.

**Prova dal vivo (root di scratch, tree curato reale non toccato):** dato
`kb/experts/bio/taxonomy.p0` che già contiene `is_a(cat,mammal)`, `is_a(cow,mammal)`:

```
teach is_a(dog, mammal)     → ha parenti (stesso pred is_a)  → ROUTED in taxonomy.p0 (riga 3)
teach habitat(dog, house)   → nessun parente                 → SPILL (session)
teach la REGOLA is_a/2      → è una clausola, mai instradata  → SPILL (session)
savemap.tsv:  is_a  dog  …/bio/taxonomy.p0  3   ← indice ricostruito
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

- **[MESH-L1 · copertura, non muro] Non ogni superficie è già rispondibile-come-fatto.**
  La comprensione è conoscenza e la mesh la fa crescere: congelare `answer_frame/2` porta
  il caso *lookup* fino alla risposta NL, propagata tra i nodi (§4.1). Ma il **sì/no su una
  relazione derivata** (*"is a dog an animal?"*, §4) resta `kb.query`-only: manca un
  *consumer* insegnabile che verbalizzi *Yes/No* chiamando `kb.query` sull'operatore.
  **Da evolvere:** tirare, un frame alla volta, i consumer di comprensione mancanti come
  fatti KB ([[universal-comprehension]], [[universal-input]], [[the-linguistic-glue]]) — a
  partire da un `answer_frame`-per-relazioni-booleane che risponda *Yes/No* su una
  `kb.query(is_a, X, Y)`. Finché quel frame non c'è, quella *specifica* superficie va
  misurata con `kb.query`; non è un limite della mesh ma la sua prossima commessa.

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

- **[MESH-L5 · minore, da confermare] `kb.match` con variabile su un predicato che ha anche
  una regola.** In un passaggio `kb.match is_a(dog, null)` non ha enumerato i binding attesi
  (timeout / vuoto) mentre `kb.query`/`kb.explain` funzionavano. Non ho isolato la causa
  (transport vs enumerazione sotto clausola). Per l'honesty-check ho usato il grep sul file,
  più diretto. **Da confermare:** se `kb.match` degrada quando il predicato porta una
  clausola ricorsiva, è un buco di lettura da chiudere (cfr. "tuple di lettura" in
  [[generative-prolog-manifesto]]).

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

> **La lezione più importante del 6/10:** gli operatori di ordine superiore congelati
> in §4.4 (kind_of/prop_of/larger_than/forebear) **non hanno mosso il punteggio**, perché
> il giudice parla in NL e quegli operatori **non sono rispondibili** (MESH-L1, `kb_match`
> si impianta sulle regole, MESH-L5). Quindi il collo di bottiglia **non è più conoscenza
> di ordine superiore — è la RAGGIUNGIBILITÀ** di quella che già esiste. Il moltiplicatore
> vero è rendere risponibili gli operatori, non aggiungerne altri.

**Il lavoro-abilitante n°1 (sblocca tutto): `kb_match` che enumera SICURO sulle regole.**
Oggi `kb_match` su un predicato con regola va in loop; per questo `answer_frame` non può
verbalizzare una conclusione *derivata*. Un `kb_match` depth-bounded (come `kb_query`)
rende **rispondibile ogni operatore già congelato** — è la leva a più alto ritorno.

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

## 9. Collegamenti

[docs/prolog-like-engine.md](../prolog-like-engine.md) (§5 — insidie di sintassi + fact
routing), [docs/use-mcp-engine.md](../use-mcp-engine.md) (il canale MCP),
[[kb-first-manifesto]], [[universal-comprehension]] (la comprensione è conoscenza, §4.1),
[[teachable-comprehension-answer-frame]] (`answer_frame/2`),
[[autolearn-knowledge-is-official]], [[universal-input]], [[the-linguistic-glue]],
[[generative-prolog-manifesto]], [[rulescore-harness]], [[llmscore-harness]].