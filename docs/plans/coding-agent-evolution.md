# parrot0 coding agent — il piano organico di evoluzione

> **Stato:** scritto a gen256 (2026-07-02), subito dopo la quarta istanza REALE di
> SWE-bench risolta e l'auto-localizzazione nell'albero (`code_smell_tree`).
> **Ruolo:** questo è il piano OPERATIVO unificato. Mette ordine tra i documenti
> esistenti (§0), fotografa le capacità misurate (§1), definisce le sei facoltà
> target chieste da F. (§2), elenca i vantaggi strutturali sui coding agent basati
> su LLM remoti (§3), e ordina i binari di evoluzione con i loro benchmark (§4–§6).
> **Subordinato a `docs/CODE-MASTERY.md`** (la teoria: codice = corpus nella stessa
> KB; struttura + verifica grounded al posto della compressione statistica) e alla
> disciplina di `LOOP.md` / `PRINCIPLES.md` (un obiettivo per generazione, pull da
> benchmark reali, niente impostori).

---

## 0. Mettere ordine — la mappa dei documenti

| Documento | Ruolo | Stato |
|---|---|---|
| `docs/CODE-MASTERY.md` | **Teoria**: perché un LLM è coerente su NL+codice e come ri-esprimerlo (F1–F5, §7b language-as-delta, §8 swe-bench) | VIVO — la direzione |
| `docs/plans/coding-agent.md` | Inventario delle parti (gen148): gap G1–G13, fasi A–E, metriche M-C1–6 | STORICO — utile come catalogo; l'ordine a fasi è superato dal pull (questo doc lo sostituisce come piano) |
| `docs/plans/learn-and-build.md` | La catena LEARN→BUILD→TEST (sortlearn-bench, algo_shape, code_check_sort) | VIVO — Track 2 di questo piano |
| `docs/plans/generative.md` | Generazione = struttura sotto oracolo; Composer/Planner/Repair; critica gen206 | VIVO — principi della sintesi |
| `docs/plans/generative-prolog.md` | La visione: Prolog genera PERCORSI di ragionamento, il linguaggio è l'ultimo passo | VIVO — orizzonte del reasoning |
| `docs/plans/the-agency.md` | Teoria dell'agency: il loop minimo goal→observe→act→verify | VIVO — fondamento di Track 3 |
| `docs/plans/kb-first.md` | Il test deduci·abduci·genera: cosa è derivabile KB-first | VIVO — disciplina trasversale |
| `TASKLIST.md` X-series | X1–X7: i task di code mastery (run, patch, node vocabulary, mirror languages, localizzazione, diff multi-file) | VIVO — il backlog puntuale |
| `docs/use-on-pi-agent.md`, `scripts/pi_server.py` | Il ponte agent (tool locali, multi-step) | VIVO — infrastruttura Track 3 |
| `tests/swebench/README.md` | L'harness SWE-bench Lite reale, offline, oracolo Docker ufficiale | VIVO — lo scoreboard Track 1 |

Ciò che era in `code-generation.md` (template/phrasebook) resta rimosso: i template
sono il motore sbagliato (CODE-MASTERY §4).

---

## 1. Stato misurato a gen256 — cosa il coding agent parrot0 SA GIÀ fare

Tutto ciò che segue è verificato da test ripetibili, non dichiarato.

**Leggere e rappresentare codice (F2 — AST-as-KB).**
- Ingest deterministico di C e Python nella STESSA KB del linguaggio naturale:
  `code_function/1`, `code_calls/2`, defs/vars (gen173–196).
- Localizzazione per nome su un albero: `code_locate` ("which file defines X"),
  `code_find_callers` (reverse call-graph), gen182–185.

**Ragionare sul codice (F3).**
- Semantica Python derivata per DELTA da C su un substrato condiviso (gen199).
- Esecuzione simbolica di corpi aritmetici (`code_eval`).

**Verificare per davvero (F5 — il vantaggio strutturale).**
- `code_compile` / `code_build` / `code_run`: compilatore e runtime reali,
  sandbox, exit code osservato — mai "secondo me compila".
- `code_check_sort`: giudice run-grounded che ricalcola ordinamento E permutazione
  da sé (un candidato non può "passare" stampando).

**Trasformare codice (F5 edit).**
- `code_rename`, `code_delete_function`, `code_replace_expr` (comment/string-aware,
  originale mai toccato, copia patchata).

**Riparare bug REALI (il risultato da generalizzare — Track 1).**
- 4 istanze REALI di SWE-bench_Lite RISOLTE dall'oracolo Docker ufficiale:
  - 12907 — symmetry break (gen200)
  - 6938 — discarded result (gen201)
  - 14995 — condition asymmetry (gen204)
  - 14365 — case folding (gen210)
- gen256: **auto-localizzazione** — parrot0 riceve solo la DIRECTORY del repo,
  fa lo sweep dell'albero con la catena di smell e trova da solo file+statement+fix
  (`code_smell_tree`); l'harness non pre-localizza più. Ri-verificato RESOLVED
  end-to-end (12907: 2/2+13/13, 6938: 2/2+11/11, 14995: 1/1+179/179). Nessun falso
  positivo su alberi puliti (incluso `src/brain`, il proprio sorgente).

**Sintetizzare codice sotto oracolo (Track 2).**
- Composizione aritmetica verificata (`mod_compose` + `code_eval`, gen206).
- Sort sintetizzato da schema KB (`algo_shape` → `code_synth_from_shape` →
  `code_check_sort`), mai hardcoded (gen209): `make sortlearn-bench` 9/9.

**Imparare (la L di §2).**
- Ricerca su markdown statici (`PARROT0_WIKI_DIR`) → `wiki_concept/3` persistito →
  richiamo dal KB. La catena FORGET→RELEARN→RECALL→BUILD→TEST è dimostrata
  end-to-end nel sortlearn-bench.

**Agire (Track 3).**
- Tool read-only locali in un turno (gen205, `PARROT0_TOOLS`): `make piagent-bench` 14/14.
- Planner articolato (gen207): un prompt lungo → sotto-obiettivi ordinati, artefatti
  passati tra i passi, ogni passo sotto oracolo.
- `make game-bench`: ledger onesto dei gap sul task e2e "costruisci tic-tac-toe".

**Onestà (trasversale, non negoziabile).**
- Ogni capacità mancante è un DECLINO dichiarato + gap ledger, mai una simulazione.
- Proof trace: "why did you answer that way?" ha una risposta strutturale.

---

## 2. Le sei facoltà del coding agent target (la richiesta di F.)

F. (2026-07-02): *reasoning, planning, controllo dei tool, potente capacità d'uso
della macchina, abilità di programmazione e conoscenza utile allo sviluppo, e in
più l'apprendimento: analizza sorgenti markdown remote, impara, e dopo produce e
modifica codice applicando le conoscenze acquisite in un quadro ampio e generale,
senza limiti di contesti applicativi.*

Ogni facoltà qui sotto ha: cosa esiste, il gap, e da quale benchmark viene tirata.

### R — Reasoning (ragionamento sul codice)
- **C'è:** motore SLD unico su fatti NL+codice; derivazioni strutturali (smell,
  simmetrie, dataflow parziale); semantica per delta; proof trace.
- **Gap:** ragionamento sul COMPORTAMENTO oltre l'aritmetica (loop, stato,
  aliasing); il percorso generativo alla generative-prolog (il ragionamento come
  cammino nel grafo KB) applicato al codice.
- **Pull:** istanze swe-bench il cui bug richiede capire un'esecuzione, non solo
  una forma; `code-bench` semantici.

### P — Planning
- **C'è:** planner articolato (gen207) con artefatti tra i passi; `mod_plan`
  su grafi di dipendenza; il loop agent perceive→decide→act→observe (gen116–120).
- **Gap:** il planning domain `code_task` (leggi issue → localizza → patch →
  compila → testa → itera); oggi il ciclo edit→verify→retry non ITERA sul
  fallimento (il repair loop B4 è deliberatamente rinviato finché un rosso reale
  non lo tira). Inoltre (steer F. 2026-07-02) i piani devono essere INFERITI da
  un dominio di azioni dichiarato in KB (pre/postcondizioni come fatti), mai
  cablati come pipeline in C: gen207 ordina passi scritti nel prompt, il salto
  è DERIVARLI dal goal + conoscenza detta (Track 5).
- **Pull:** game-bench (multi-prompt e2e) e il primo schema `algo_shape` errato.

### T — Tool control (controllo degli strumenti)
- **C'è:** tool locali read-only in un turno (gen205); oracolo POSIX simulato;
  compilatore/runtime come tool deterministici; il pi agent come guscio.
- **Gap:** git come tool (log/diff/blame → fatti KB); esecuzione guardata di
  comandi arbitrari whitelisted (il C2 dell'inventario); scelta del tool derivata
  dal KB (`tool_for/2`) invece che cablata.
- **Pull:** game-bench e piagent-bench quando un passo richiede un tool nuovo.

### M — Machine use (uso potente della macchina)
- **C'è:** filesystem sandboxed (lettura alberi, scrittura `.p0fix`/temp);
  build+run reali; sweep ricorsivo di un repo (gen256).
- **Gap:** write-back controllato (X7: applicare una patch al posto giusto sotto
  guardia, non solo copie); multi-file; processi con osservazione di stdout/stderr
  come fatti; il "computer use" resta sempre sandbox-first.
- **Pull:** la prima istanza swe-bench multi-file; il game-bench che deve scrivere
  un progetto.

### K — Knowledge (abilità di programmazione e conoscenza SWE)
- **C'è:** `kb/experts/programming/` (algoritmi, complessità, `faster_than/2`),
  `kb/bash.p0` (comandi), schemi `algo_shape`; i fatti-linguaggio (metodi puri
  Python come fatti, non phrasebook).
- **Gap:** X3–X5 — il vocabolario di nodi astratto (le analisi non devono parlare
  C), i linguaggi come mirror concept (`like(python, <abstract>)` + override),
  la conoscenza SWE di processo (test, review, versioning) come fatti interrogabili.
- **Pull:** la prima istanza non-astropy / non-Python; le domande del llmscore su
  concetti di sviluppo.

### L — Learning (il vincolo di F.: markdown remoti → conoscenza → codice)
- **C'è:** la catena è GIÀ dimostrata su un caso: pagina markdown → `wiki_concept`
  → schema KB → sintesi → verifica per esecuzione (sortlearn-bench 9/9). "Remote"
  nel senso del progetto: snapshot statici curati (dynamic-knowledge: la rete solo
  in curation, mai intelligenza esternalizzata a runtime).
- **Gap (il ponte cruciale):** oggi lo schema `algo_shape` è curato a mano; la
  pagina imparata resta PROSA. Il passo mancante è l'**induzione di struttura**:
  estrarre dai passi numerati/pseudocodice di una pagina markdown lo schema
  eseguibile (`algo_shape`, `algo_io`, parametri), così che "impara e poi
  costruisci" valga per una pagina MAI vista, senza limiti di contesto applicativo.
- **Pull:** un secondo e terzo algoritmo imparati da pagine fresche (selection
  sort, binary search) con lo STESSO induttore; il ratchet è che nessun nuovo
  schema venga scritto a mano.

**Sul "senza limiti di contesti applicativi":** la generalità onesta viene da
CATEGORIE e SCHEMI (smell strutturali, `algo_shape`, vocabolario di nodi astratto,
mirror concept), mai da risposte per-contesto. Dove la categoria non copre, parrot0
declina e il gap ledger nomina il prossimo pull. L'ampiezza cresce per selezione,
generazione dopo generazione — è la stessa legge di basic-chat e llmscore
(categories-not-prompts), applicata al codice.

---

## 3. I vantaggi strutturali sui coding agent basati su LLM remoti

Questi non sono aspirazioni: discendono dall'architettura e vanno PRESERVATI da
ogni generazione futura (sono i criteri di review di ogni nuova feature).

1. **Privacy totale / offline.** Il codice non lascia mai la macchina. Nessun
   token inviato a terzi, nessuna dipendenza da un servizio, funziona in air-gap.
2. **Determinismo e riproducibilità.** Stesso input → stessa patch, sempre.
   Un fix è riproducibile in CI e bisecabile; niente temperature, niente drift
   di modello remoto.
3. **Verifica grounded per costruzione.** parrot0 non DICHIARA mai un fix: lo
   propone da struttura e lo fa giudicare da compilatore/test reali (l'oracolo
   decide). Un LLM può allucinare "i test passano"; qui il verdetto è meccanico.
4. **Spiegabilità totale (proof trace).** Ogni patch ha una RAGIONE strutturale
   nominabile ("rompe la simmetria col ramo gemello") e ogni risposta un percorso
   di derivazione ispezionabile. "Perché hai risposto così?" ha sempre risposta.
5. **Niente allucinazioni per costruzione.** Ciò che non è derivabile viene
   declinato onestamente con il gap nominato — mai codice inventato con tono
   fluente. Per un agente che scrive nel tuo repo, questo è il requisito n.1.
6. **Conoscenza ispezionabile, editabile, versionata.** Il "modello" è la KB:
   file di testo diffabili in git. Correggere una conoscenza è un commit, non un
   fine-tuning. L'apprendimento è permanente e locale.
7. **Costo ~zero e latenza ms.** Un binario C su CPU; nessun costo per token,
   nessun rate limit; gira su un Raspberry Pi (il pi agent è già il guscio).
8. **Sandbox per costruzione.** Path relativi sotto la working dir, originali mai
   toccati (`.p0fix`), niente rete a runtime per PRINCIPIO, non per policy.
9. **Contesto = filesystem, non finestra.** Non c'è context window da saturare:
   l'albero si interroga per query (locate, callers, sweep), la memoria è la KB
   di sessione. Un repo da milioni di righe non "trabocca".
10. **Onestà come contratto.** Il gap ledger è parte dell'output: l'utente sa
    esattamente cosa l'agente non sa fare — nessun LLM remoto offre questo.

**Il contro-lato onesto (la frontiera, CODE-MASTERY §4):** l'LLM resta superiore
su F4 — il ponte associativo intent↔codice (dal testo libero di una issue alla
regione giusta) e la sintesi open-domain. Non lo simuliamo con phrasebook: lo
misuriamo, e ogni istanza reale tira o un meccanismo strutturale nuovo o la prova
di un muro. Quel confine misurato È il deliverable dell'esperimento.

---

## 4. I binari di evoluzione (pull-based: nessun ordine cronologico rigido)

Ogni generazione prende UN obiettivo da UN binario, tirato da un test rosso reale.
I binari avanzano in parallelo per selezione, non per fasi.

### Track 1 — Repair su repo reali (`make swe-bench` / `swe-solve`)
Il binario dei 4 RESOLVED. Prossimi pull, in ordine di pressione:
1. **Più istanze** (curation: `fetch_lite.sh N` + snapshot `repo_excerpt` a
   base_commit): ogni istanza nuova o (a) è coperta da uno smell esistente —
   ratchet gratis, o (b) tira lo smell generale n.5, n.6, … La libreria di smell
   È il localizzatore (gen256), quindi ogni smell nuovo potenzia anche X6.
2. **Excerpt multi-file con decoy** — più file reali per istanza, così lo sweep
   deve discriminare (già dimostrato su 4 file + alberi puliti; va reso la norma).
3. **X7 — patch multi-file + write-back guardato**: dal `.p0fix` al diff applicato
   sotto guardia esplicita.
4. **X6 associativo — issue text → restrizione della regione**: SOLO come
   esperimento misurato (es. parole della issue → nomi di funzione via KB
   lessicale), mai phrasebook; se non regge, il muro va documentato.
5. **14182 (feature add)**: il primo caso oltre la riparazione — richiede Track 2.

### Track 2 — BUILD: sintesi sotto oracolo (learn-and-build)
1. **B4 — repair loop** tirato da un rosso reale (schema volutamente errato o
   secondo algoritmo con prima emissione buggata): fallimento → ipotesi → retry
   bounded. È il pezzo che chiude il loop agentico edit→verify→ITERATE.
2. **Secondo/terzo `algo_shape`** (selection/insertion, binary search) per provare
   il riuso dello schema, non aggiungere un caso.
3. **Il ponte L — induzione markdown→schema**: da una pagina con passi strutturati
   allo `algo_shape` indotto (il pull principale della facoltà L, §2). Ratchet:
   il sortlearn-bench con una pagina FRESCA e nessuno schema curato.
4. **Composizione**: due funzioni sintetizzate indipendenti devono comporre
   (anti-phrasebook del piano gen148, ancora valido).

### Track 3 — Agente: planning + tool + macchina

> **Il protocollo needhelp (steer F., 2026-07-02 — pattern pubblico).** Quando
> nel planning/reasoning parrot0 capisce che una cosa NON la sa e non può
> procedere, **non si ferma**: crea un file in formato `.p0` nella ROOT del
> progetto su cui sta lavorando (nome tipo `needhelp.p0`) dove esprime ciò che
> gli manca; chiunque (umano o altro agente) può scrivere dentro la risposta;
> periodicamente, come passo del piano, parrot0 torna a controllarlo per vedere
> se qualcuno lo ha aiutato. È il suo canale di aiuto esterno asincrono.
>
> **Vincolo: abilità intelligente, non fallback cablato.** Il protocollo stesso
> è CONOSCENZA in KB (fatti che descrivono l'azione "chiedi aiuto": quando un
> goal è bloccato da un fatto mancante → scrivi la richiesta; il check
> periodico è un passo con precondizioni, inferito dal planner come qualunque
> altra azione) — mai un ramo C "se fallisci scrivi il file". La realizzazione
> naturale è KB-first fino in fondo: la RICHIESTA è fatti `.p0` (il goal, il
> fatto mancante, il contesto), la RISPOSTA dell'aiutante è altri fatti `.p0`,
> e "ricevere aiuto" = caricare quel file come conoscenza nativa — nessun
> parser nuovo, il gap si chiude per assimilazione di fatti. Si sposa con il
> declino onesto: invece di fermarsi al muro, il muro diventa una richiesta
> formale e il piano resta vivo.

1. **Repair loop nel planner** (condiviso con Track 2 B4): un passo fallito
   propone il fix invece di fermarsi.
2. **game-bench come pull e2e**: il ledger dice quale facoltà manca per costruire
   un progetto vero in 3 prompt; ogni voce del ledger è una generazione.
3. **git-as-tool** (D4 dell'inventario): log/diff/blame → fatti KB interrogabili.
4. **Esecuzione guardata generalizzata** (C2): whitelist+timeout+sandbox, output
   → fatti di sessione (session-is-knowledge, gen240).

### Track 4 — Conoscenza: linguaggi e SWE come KB
1. **X3 — vocabolario di nodi astratto**: le analisi smettono di parlare C;
   un front-end nuovo riusa gli analizzatori invariati. È il prerequisito per
   "senza limiti di contesti applicativi" sul lato linguaggi.
2. **X4/X5 — mirror concept**: `like(L, <abstract>)` + override, misurando il
   rapporto trasferito/specifico.
3. **KB SWE di processo** (A5/A6 dell'inventario, KB-first): tool, testing,
   versioning come fatti — utile anche a llmscore e alle domande di sviluppo.

### Track 5 — Lavorare su una codebase (outer circle) — la conoscenza dirige il comportamento

> **Steer F. (2026-07-02):** questa NON è riflessività. La facoltà è generale:
> parrot0 studia una codebase e se ne fa un'idea, poi interviene — e una di
> quelle codebase *guarda caso* è la sua. Nessun accesso privilegiato, nessun
> self-model, nessuna auto-riparazione per natura e per design: è dogfood
> **outer circle**. Tutto ciò che sa di src/brain deve derivarlo dai file che
> legge, come fece con astropy.

**L'arco guida: cue-chains → KB.** I moduli brain contengono ~268 catene
`if (cue(..) || cue(..) …)` (misura gen256): vocabolario C che per il principio
cardinale va portato in regime KB-first — `intent_cue/2` + `kb_cue_match`, il
precedente è gen223 (verbi pi-act migrati). Il contatore delle catene residue è
il ratchet: deve solo scendere. Le CLASSI di parole vanno in KB; la forma
booleana, l'ordine dei moduli e gli ancoraggi strutturali restano C (forma, non
vocabolario) — una migrazione cieca regredirebbe la precisione.

**Il metodo (vincolo di F.): il piano è INFERITO, mai cablato.**
- In C solo primitive generiche e mute: scandire un albero, trovare catene OR di
  chiamate a una funzione *F* (F è un parametro, mai "cue" cablato), estrarre
  stringhe, emettere fatti, sostituire espressioni, compilare, eseguire test.
- La conoscenza DETTA a runtime ("le catene di cue sono vocabolario", "il
  vocabolario vive come intent_cue/2", "la forma C diventa kb_cue_match") viene
  sintetizzata in fatti/regole Prolog.
- Il planner INFERISCE la sequenza da azioni con pre/postcondizioni dichiarate
  in KB, verso il goal espresso ("porta il modulo X in regime kb-first").
- **Senza quella conoscenza: declino onesto.** La KB è l'interruttore del
  comportamento (kb-first.md; comportamento diretto dalla conoscenza).

**Oracolo.** Il refactor è behavior-preserving: `make build` + `make test`
byte-identico è un giudice meccanico più severo di SWE-bench.

**Anti-impostor.** La stessa conoscenza insegnata, applicata a una codebase
ESTRANEA con catene analoghe (fixture), deve produrre lo stesso lavoro. Se
funziona solo su src/brain, abbiamo barato.

**Scala dei pull:**
1. ✅ gen257 — Percezione: detector generico di catene OR di chiamate
   (report-only, F parametrica, file/albero; contatore vivo: 341 catene di
   `cue` in src/brain, fixture estraneo come anti-impostor).
2. ✅ gen258 — Dominio di planning in KB (`kb/experts/codebase/actions.p0`):
   azioni + pre/postcondizioni come fatti, chainer generico all'indietro,
   piano DERIVATO dal goal; il needhelp è il secondo dominio (conoscenza, non
   fallback); goal ignoto → fall-through, buco nella catena → declino che
   nomina l'artefatto orfano.
3. Esecuzione del piano derivato: bind dei passi alle primitive esistenti
   (scan_chains = detector gen257; run_test_suite = make test), poi il primo
   sito rifattorizzato da conoscenza detta, giudicato dai test byte-identici.
4. Migrazione per categorie (il contatore scende, una gen alla volta).
5. Demo di crescita a runtime: parola nuova insegnata → un ramo prima sordo
   aggancia, senza rebuild (il "gioco dinamico con la conoscenza").

---

## 5. Benchmark e ratchet (gli strumenti di misura, tutti esistenti)

| Bench | Misura | Stato gen256 | Flip che segna progresso |
|---|---|---|---|
| `make swe-solve INSTANCE=…` | patch derivata + oracolo Docker ufficiale | 4/5 RESOLVED, self-localized | istanza n.5+ RESOLVED; primo multi-file |
| `make swe-bench` | mappa dei blocker per istanza (degrade mode) | 5 istanze committate | ogni blocker che sparisce |
| `make code-bench` | comportamento su stimoli di codice | 21 gate | gate nuovi per smell/semantica |
| `make sortlearn-bench` | LEARN→BUILD→TEST end-to-end | 9/9 (schema curato) | schema INDOTTO da pagina fresca |
| `make piagent-bench` | tool locali + multi-step | 14/14 | nuovi tool sotto gate |
| `make game-bench` | progetto e2e in 3 prompt | ledger onesto | ogni voce del ledger chiusa |
| `make llmscore` | comportamento da LLM (giudice esterno) | rotante | item di coding chiusi per categoria |
| `make test` | l'intero ratchet ermetico | verde (215+) | resta verde SEMPRE |

---

## 6. Disciplina (invariata, e vincolante per ogni binario)

- **Un obiettivo per generazione**, il più piccolo passo che generalizza (LOOP.md).
- **Pull, non push**: si costruisce solo ciò che un test rosso reale richiede
  (pivot duty); l'ordine dei binari lo detta la pressione dei bench.
- **KB-first**: forme linguistiche, schemi, fatti di linguaggio → KB; il C è
  macchinario fisso (kb-first.md: deduci·abduci·genera).
- **Categorie, non prompt** (steer gen254): si chiudono CLASSI con regole del
  motore; un misclaim è peggio di un muro.
- **Anti-impostor**: nessun corpo di codice pre-scritto nel brain; il generatore
  propone, l'oracolo dispone; declino onesto dove manca la categoria.
- **Strutture secondarie ridondanti si tengono** (steer F.): evoluzione per
  selezione, cambi additivi, niente potature premature.
- **Bilingue** EN/IT sull'interfaccia; cross-linguaggio sul codice (stesso
  meccanismo, non secondo path).
- **Rete solo in curation** (snapshot statici committati); il binario a runtime
  non tocca mai la rete.

---

## 7. Le prossime ~10 generazioni (proposta concreta, rivedibile a ogni pull)

1. **Track 5.1 — detector generico di catene OR** (gen257): percezione
   outer-circle su qualsiasi codebase, F parametrica, fixture estraneo.
2. **fetch_lite espanso** (curation): +10–20 istanze reali committate con
   `repo_excerpt` auto-snapshot dei file toccati dal gold patch (+decoy).
3. Prima istanza nuova coperta dagli smell esistenti → RESOLVED n.5 (ratchet).
4. Prima istanza NON coperta → smell generale n.5 (il pull dice quale).
5. **B4 repair loop** da un rosso reale (schema errato deliberato).
6. **Induzione markdown→algo_shape** su una pagina fresca (facoltà L completa).
7. **X3 audit**: vocabolario di nodi astratto; un front-end toy riusa le analisi.
8. **X7**: diff multi-file + write-back guardato.
9. **git-as-tool**: `git log/diff` → fatti KB, domande sul repo reale.
10. **game-bench**: chiudere la prima voce del ledger col repair loop del planner.
11. **Track 5.2–5.3**: dominio di planning in KB (azioni+pre/postcondizioni) e
    primo sito cue-chain rifattorizzato da conoscenza detta, `make test`
    byte-identico come giudice, fixture estraneo come anti-impostor.

Ogni voce resta subordinata al pull: se un bench rosso indica altro, quello vince.