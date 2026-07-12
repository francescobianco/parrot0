# PARROT0 FORGE - piano maestro verso un coding agent verificabile

> **Stato:** piano canonico, revisione **Fast Forge** 2026-07-12. La fotografia
> iniziale era HEAD `93511ba`; W0 truth e poi avanzata fino a gen317. Questa
> revisione corregge il difetto emerso durante l'esecuzione: il vecchio piano
> faceva pagare quasi l'intera certificazione a ogni ipotesi e rendeva il
> feedback lento, opaco e poco direzionale.
>
> **Missione:** portare parrot0 a competere con un coding agent basato su LLM
> nel perimetro in cui il risultato puo essere verificato, senza inserire un LLM
> nel runtime e senza rinunciare a determinismo, spiegabilita, privacy e onesta.
>
> **Metodo:** sostituire la crescita lineare per casi con una **fucina**:
> fallimenti reali diventano candidati isolati; fatti, regole, procedure, policy
> o primitive C vengono promossi solo dopo ablation, holdout, oracoli meccanici,
> regressioni e prova di sicurezza.
>
> **Autorita:** questo documento ordina i piani operativi esistenti, ma resta
> subordinato a `PRINCIPLES.md`, `LOOP.md` e `DESIGN.md`. Quando un piano storico
> contraddice questo documento sullo stato corrente o sull'ordine dei lavori,
> prevale questo documento. I principi fondanti non vengono sovrascritti.

> **Correzione di rotta:** un esperimento non e una generazione. Molti
> esperimenti economici possono competere e morire in locale; una generazione
> nasce solo quando un candidato sopravvissuto viene promosso. Il rigore resta
> intero, ma viene pagato una volta dal champion, non da ogni ipotesi.

---

## 0. Verdetto strategico

parrot0 non deve diventare un LLM in miniatura e non deve inseguire la fluidita
token-per-token di un modello statistico. Il progetto ha un bersaglio piu netto:

> **parita, e dove possibile superiorita, sugli esiti di coding verificabili.**

Un task conta come risolto solo se parrot0:

1. comprende il contratto del task e conserva vincoli e contesto;
2. raccoglie evidenza dal repository reale;
3. localizza la regione rilevante senza che l'harness gliela suggerisca;
4. formula un piano derivato da azioni disponibili e precondizioni;
5. produce una patch applicabile, anche multi-file;
6. esegue build e test reali in una sandbox;
7. osserva un eventuale fallimento e ripara in modo budgetato;
8. conclude solo con evidenza, oppure declina nominando il gap preciso;
9. lascia una traccia riproducibile di fonti, azioni, artefatti e verdetti.

Questo e un obiettivo piu rigoroso di "sembrare intelligente". Nel dominio del
codice esistono compilatori, test, property checker e diff: l'impostore non puo
nascondersi dietro una risposta plausibile.

La tesi del piano e che le linee gia vive nel progetto convergono in una sola
macchina:

```text
comprensione strutturale
        +
fatti e procedure insegnabili
        +
solver con proof e provenienza
        +
planner su azioni dichiarative
        +
macchina transazionale e sandboxata
        +
oracoli reali e repair bounded
        =
coding agent verificabile
```

Il salto mancante non e un altro `mod_*`. E un **kernel agentico tipizzato** e
un processo di selezione che faccia crescere quel kernel senza degradarlo.

### 0.1 Il feedback e parte dell'architettura

La prima versione del piano trattava i test soprattutto come certificazione.
Questo produceva una dinamica sbagliata: si modifica, si attende una matrice
ampia e solo alla fine si scopre se la direzione era utile. Un risultato binario
tardivo fa sembrare ogni iterazione una ruota della fortuna e riduce il numero di
ipotesi che possiamo esplorare.

La nuova tesi operativa e:

```text
il Tick orienta
il Turn seleziona
la Promotion dimostra
il Nightly certifica
```

Ogni livello deve restituire presto il **primo contratto rotto**, non soltanto un
PASS/FAIL finale. Se un controllo e costoso, non viene abolito: viene eseguito
piu tardi e solo sulle poche candidate che hanno guadagnato quel costo.

---

## 1. Costituzione della fucina

Questi invarianti sono criteri di review, non aspirazioni.

### 1.1 Nessun LLM nel runtime

- Il binario distribuito resta C deterministico.
- Nessuna API di intelligenza esterna decide una risposta, un piano o una patch.
- Un LLM puo essere usato fuori dal runtime, nella fucina, come proposer,
  intervistatore o critico. Non e mai l'oracolo finale e non promuove nulla.
- Compilatori, test, interpreti, parser, git e documenti statici sono strumenti o
  dati, non intelligenza presa in prestito.

### 1.2 Introspection proposes; tests dispose

- Ogni nuova ipotesi deve diventare un artefatto eseguibile o interrogabile.
- Una risposta dello stesso teacher sullo stesso prompt non e una verifica.
- Quando esiste un oracolo meccanico, il voto linguistico non conta come prova.
- Ogni successo deve superare un'ablation: togliendo il candidato, il caso deve
  tornare rosso.

### 1.3 Engine fixed, knowledge learns

L'ordine preferito per chiudere un gap e:

1. fatto o lessico nel KB;
2. schema dichiarativo;
3. clausola o procedura di riscrittura;
4. action schema con precondizioni ed effetti;
5. nuova primitiva C generale, solo se i livelli precedenti non possono
   esprimere il comportamento;
6. modifica architetturale, solo se piu gap indipendenti mostrano lo stesso muro.

"Engine fixed" significa che durante una sessione di insegnamento il meccanismo
non cambia per inseguire il prompt. Il kernel puo evolvere tra generazioni, ma
solo dopo una failed lesson riproducibile che dimostri un limite espressivo.

### 1.4 Fatti e procedure sono conoscenza, native code no

`teachable-procedures.md` supera il vecchio divieto generico di "insegnare
algoritmi via MCP" nel modo corretto:

- sono lecite clausole, riscritture, constraint e action schema dichiarativi;
- sono eseguiti da un interprete generico gia presente nel kernel;
- non e lecito inviare C arbitrario via MCP e trattarlo come conoscenza;
- una nuova primitiva C passa dalla fucina engine, non dal canale di teaching.

### 1.5 Additivita con selezione, non accumulo globalmente attivo

Il principio keep-secondary resta valido, ma viene precisato:

- una variante superata non viene cancellata;
- puo restare in shadow mode, archiviata e riattivabile;
- non deve restare necessariamente attiva nel dispatch globale;
- una activation policy decide champion, challenger e fallback;
- consolidamento o pensionamento richiedono evidenza differenziale.

Conservare informazione non significa conservare interferenza.

### 1.6 Onesta operativa

- `unknown`, `not-understood`, `ambiguous`, `conflicted`, `unsafe`, `timeout` e
  `not-proven` sono stati diversi.
- Un decline utile include il primo anello mancante e l'evidenza che lo dimostra.
- Un artefatto non verificato viene chiamato candidato, mai soluzione.
- Una suite degradata non viene presentata come benchmark risolto.

### 1.7 Una promozione, un contratto

- Un **esperimento** risponde a una sola domanda e puo essere scartato senza
  versione, journal o full regression.
- Un **candidato** e un esperimento che ha chiuso il caso focale e i negativi
  diretti; resta in overlay o worktree.
- Una **generazione** e una sola promotion atomica che chiude un contratto rosso.
- La generazione deve essere piccola, reversibile e bisecabile, ma puo essere
  preceduta da molte candidate concorrenti.
- Test 10x, EN/IT, holdout e replay restano obbligatori quando pertinenti, ma non
  devono essere rilanciati dopo ogni edit.
- Il piano e a dipendenze, ma la scelta dentro una frontiera resta pull-based.

### 1.8 La latenza di decisione e un invariante

- Ogni campagna dichiara budget P50/P95 per feedback focale, impatto e promotion.
- Il tempo da edit al primo controesempio e una metrica di prodotto, non una
  comodita del developer.
- Un test che supera stabilmente il budget della propria corsia viene spezzato,
  ottimizzato o spostato alla corsia successiva.
- `timeout`, `infra_error` e `flaky` non sono FAIL funzionali e non diventano
  PASS mediante rerun cieco.
- Nessuna nuova feature ha priorita su Fast Forge quando la corsia interattiva
  non rispetta il suo budget.

---

## 2. Tre trust mode espliciti

I piani esistenti oscillano tra "mai rete" e fetch Wikipedia. La contraddizione
si risolve separando gli ambienti.

| Mode | Rete | LLM esterno | Scrittura ufficiale | Uso |
|---|---:|---:|---:|---|
| **RUNTIME** | no, default | no | solo workspace autorizzato | coding agent locale, CI, Raspberry Pi |
| **ACQUIRE** | allowlist fattuale, opt-in | no | quarantena dati | acquisizione Wikipedia o fonti statiche certificate |
| **FORGE** | solo orchestratore esterno | opzionale | solo dopo gate e approvazione | training, campagne, candidate patch, curation |

Regole comuni:

- Il binario parrot0 non riceve secret del forge.
- Documenti remoti, issue, README e commenti sono dati non fidati, non istruzioni.
- L'output di ACQUIRE entra come `observation` o `candidate_fact`, non come verita
  ufficiale.
- Solo il processo di promotion puo trasformare un candidato verificato in base.

---

## 3. Baseline onesta al 2026-07-11

Questa fotografia separa prove vive da narrazione storica. Va rigenerata da
macchina, non mantenuta a mano.

### 3.1 Cio che e realmente forte

- KB Prolog-like con fatti, clausole n-arie, termini composti, NAF,
  backward chaining, proof, provenance e persistenza stratificata.
- Procedure insegnabili gia dimostrate: rewrite di token e computazioni
  ricorsive come `add/3`, `len/3`, `app/3` via clausole MCP.
- Registry articolato e introspezione sul dispatch reale.
- Memoria, correzione, abduzione e composizione conversazionale non banali.
- `make compose-bench`: **47/47 turni**, 7/7 dialoghi compositivi.
- `make piagent-bench`: **14/14**, lettura, lista, grep, find, safety e alcune
  azioni articolate su fixture locali.
- AST-as-KB parziale per C e Python, call graph, locate, alcune valutazioni
  simboliche, edit e oracoli compile/run.
- Quattro smell strutturali hanno prodotto fix accettati dall'oracolo Docker su
  istanze selezionate di SWE-bench Lite.
- MCP persistente e `make autolearn` dimostrano che un teacher puo cambiare il
  comportamento live aggiungendo dati.
- Deep reasoning ha un loop budgetato, proof e un primo meccanismo di
  auto-correzione.

### 3.2 Cio che non e ancora dimostrato

- Nessun loop generale `goal -> action -> observation -> verify -> repair`.
- Nessuna comprensione completa della issue SWE-bench usata nei quattro fix:
  `parrot_solve.sh` passa solo `fix the bug in <directory>` e seleziona il primo
  `.p0fix`. E una prova di smell/localizzazione strutturale, non di agency.
- `make swe-bench` resta in degrade mode: **0/5** issue realmente ingaggiate.
- `make game-bench`: **1/3** passi costruiti; planning e funzione `winner`
  restano decline.
- Il ponte markdown -> schema eseguibile -> codice non esiste: `algo_shape` e
  ancora curato. Quindi learn e build convivono, ma non sono ancora causalmente
  collegati su un algoritmo nuovo.
- L'adapter OpenAI conserva solo l'ultimo messaggio user, rende opzionale il
  system, ignora history/tool schema e tronca a 2000 byte.
- Lo stato di conversazione e task usa molti array fissi troppo piccoli per repo
  e sessioni reali.
- Il parser codice e un insieme utile di scanner, non ancora un IR con scope,
  CFG, dataflow, tipi, macro e identita stabili dei nodi.
- L'esecuzione usa whitelist di prefissi e `popen`; non e una sandbox sufficiente
  per repair autonomo.

### 3.3 Regressioni vive rilevate durante questa analisi

| Comando | Risultato | Regressione |
|---|---:|---|
| `make test` | 241 pass, 2 fail | `agent_search` EN/IT rubati dall'aritmetica composta |
| `make code-bench` | 23/25 gate | no-functions e locate Python misrouted |
| `make sortlearn-bench` | 8/9 | il caso forget non e piu un miss pulito |
| `make piagent-bench` | 14/14 | nessuna |
| `make game-bench` | 1/3 built | planner e winner ancora aperti |
| `make compose-bench` | 47/47 | nessuna |
| `make glue-bench` | 9/9 gate | il caso qualitativo brevity IT mura ancora |
| `make swe-bench` | 0/5 engaged | harness degradata, non solve |
| `LLMSCORE.md` | 4/10 | misura comportamentale, non gate di verita |

Queste regressioni hanno tirato gen313-gen315 e sono ora chiuse. La lezione non
e introdurre un blocco globale: core trust e capability toccata devono essere
verdi, mentre i rossi noti non correlati sono fingerprinted e non impediscono
esperimenti in shadow (§10.1).

### 3.4 Debito di verita e governance

- `brain_version()` riportava `gen300-class-conjunction` mentre HEAD dichiarava
  gen312; gen317 ha chiuso il claim drift, ma il commit nell'header causa ora un
  rebuild/cache debt (§3.5).
- Molti piani includono handoff storici e stati superati senza un indice
  machine-readable.
- `make test` non include `code-bench`, `sortlearn-bench`, `game-bench` e SWE.
- Un `PASS` in degrade mode misura esecuzione dell'harness, non capacita.
- LLMSCORE ha gia accettato almeno un fatto falso: un LLM judge e discovery,
  non verita.
- Il manifest chiama `glue-bench` un gate, ma lo script dichiara discovery e
  termina sempre 0 anche con GAP; oggi spendiamo circa 12.81 s senza ottenere il
  ratchet dichiarato.
- `mimic` e marcato external ma gira offline; `bench-mmlu` e `bench-bbh` puntano
  a fixture locali pur essendo marcati external. La semantica machine-readable
  non e ancora la fonte di verita che dichiara di essere.

### 3.5 Debito di feedback rilevato al 2026-07-12

Misure su host di sviluppo a 22 core, binario caldo e output soppresso:

| Percorso | Wall time | Causa dominante |
|---|---:|---|
| `make build`, no-op | 0.02 s | nessuna |
| rebuild completo `-O2` | 12.37 s | `brain.c` e i 13 frammenti sono una sola TU |
| `tests/run.sh` | 21.37 s | 243 processi, 1.479 turni, circa 352 CPU-s |
| `make test` | 171.11 s | 38 harness seriali, circa 504 CPU-s |
| `make gate` | 233.52 s | sei target seriali, circa 562 CPU-s |
| `make gate` + `make capability-report` | circa 467 s | gli stessi sei target eseguiti due volte |
| `llmscore_world.sh` | 68.95 s | molti boot e probe black-box |
| `knowledge.sh` | 15.30 s | boot/probe black-box ripetuti |

Il vecchio dato di circa 51 secondi in `optimize-the-tests.md` non descrive piu
la suite corrente. Il solo parallelismo di `run.sh` non basta: consuma oltre
cinque minuti di CPU e nasconde tutti i verdetti fino all'aggregazione finale.

Inoltre:

- `make gate` esegue i target di area in sequenza e ne cattura l'output;
- per i primi circa 171 secondi di `make gate` non appare alcun progresso e un
  rosso non interrompe i target successivi;
- `make capability-report` riesegue gli stessi gate invece di consumare la loro
  evidenza sigillata;
- il commit hash generato in `version.h`, unito alla dipendenza di ogni object da
  tutti gli header, forza rebuild ampi dopo un commit e invalida cache che non
  dipendono dal comportamento; la versione va isolata in una piccola TU;
- `tests/benchmarks.json` cataloga target interi, non singoli contratti, owner,
  dipendenze, costo o requisito di isolamento;
- quasi tutti i test attraversano testo, dispatch, KB, rendering e processo
  insieme: il fallimento arriva tardi e non nomina automaticamente il livello
  proprietario.

Questa e ora una regressione architetturale prioritaria. Tre minuti per un solo
ratchet non sono compatibili con esplorazione rapida; aggiungere altri test alla
stessa forma peggiorerebbe la capacita di apprendere del progetto.

---

## 4. Livelli di maturita delle capacita

Ogni capacita usa cinque stati. La parola "fatto" da sola viene abolita.

| Livello | Significato | Gate minimo |
|---|---|---|
| **ABSENT** | nessun comportamento utile | gap riproducibile |
| **SEED** | un dimostratore curato | red->green + ablation |
| **TRANSFER** | generalizza oltre il caso di sviluppo | mutazioni, negativi, EN/IT o equivalente, 10x |
| **FIELD** | chiude task freddi in ambienti diversi | holdout nascosto, piu fixture/repo, budget rispettato |
| **HARDENED** | affidabile per uso produttivo | replay, sicurezza, zero regressioni, rollback provato |

Baseline indicativa:

| Facolta | Stato | Evidenza | Muro per il livello successivo |
|---|---|---|---|
| inferenza KB | FIELD | compose, proof, clausole, NAF | scala, tabling, conflitti generali |
| procedure insegnabili | SEED | rewrite + Peano via MCP | proof P3, holdout cross-domain |
| comprensione conversazionale | TRANSFER | glue/compose | schema intenti universale e gap tipizzati |
| lettura codice | TRANSFER | 23/25 code gates | IR, scope, CFG/dataflow, regressioni |
| tool locali | TRANSFER | 14/14 fixture | executor sicuro e repo freddi |
| repair strutturale | SEED | 4 smell selezionati | issue->regione, decoy, casi freddi |
| planning agentico | SEED | piani articolati curati | action model e replan su osservazione |
| editing multi-file | ABSENT | solo copie `.p0fix` | patch transaction + rollback |
| learn->build causale | SEED | sort con shape curato | markdown nuovo -> shape indotto |
| autolearn | SEED | flip stesso probe | quarantena, holdout, promotion sicura |
| API coding-agent | SEED | adapter compatibile minimo | history, tool calls, artefatti, streaming reale |
| sicurezza autonoma | ABSENT | filtri parziali | isolamento processo/fs/rete/risorse |

Il capability ledger ufficiale deve essere generato dai risultati e contenere
per ogni riga: stato, commit, test, holdout, oracle, ultima verifica, costo,
regressioni note e rollback.

---

## 5. Mappa dei piani esistenti

### 5.1 Costituzione

| Documento | Ruolo |
|---|---|
| `PRINCIPLES.md` | tesi, anti-impostor, articolazione, keep-secondary, KB-first |
| `LOOP.md` | disciplina di una generazione, ratchet e stress |
| `DESIGN.md` | decisioni architetturali e forging experts |
| `docs/plans/kb-first.md` | test deduci, abduci, genera |
| `docs/plans/the-agency.md` | goal, osservazione, azione e differenza |

### 5.2 Direzioni vive da fondere

| Documento | Contributo alla fucina |
|---|---|
| `teachable-procedures.md` | fatti + trasformazioni come conoscenza |
| `universal-solver.md` | prosa -> vincoli -> soluzione/proof -> verifica |
| `deep-reasoning.md` | loop budgetato, estrazione, provenance, correzione |
| `coding-agent-evolution.md` | facolta R/P/T/M/K/L e track coding |
| `learn-and-build.md` | sintesi sotto oracolo, con gap schema-induction |
| `generative.md` | generazione di codice come proposta verificata |
| `generative-prolog.md` | ragionamento come percorso generativo nel KB |
| `mcp-engine.md` | canale di teaching e inferenza persistente |
| `llmscore-strategies.md` | trainer loop e failed-lesson come pressione |
| `universal-comprehension.md` | forma != risposta, decline informato e acquire |
| `the-linguistic-glue.md` | continuita tra moduli e turni |

### 5.3 Documenti subordinati o diagnostici

- `coding-agent.md`: inventario storico G1-G13 e M-C1-M-C6, non ordine corrente.
- `unification.md` e `unification-assessment.md`: storia utile; molti limiti sono
  gia superati.
- `basic-chat.md`: corpus di sonde, non roadmap.
- `llmscore-check.md` e `mimic-llm.md`: misure di comportamento/stile, non target
  di correttezza.
- `optimize-the-tests.md`: infrastruttura abilitante della fucina.

### 5.4 Tensioni risolte da questo piano

| Tensione | Risoluzione |
|---|---|
| no rete vs Wikipedia live | trust mode RUNTIME, ACQUIRE, FORGE |
| MCP non insegna algoritmi vs procedure insegnabili | regole dichiarative si; native code no |
| MCP session-only vs promozione ufficiale | candidate -> quarantine -> verify -> promote |
| fatti difettibili vs verify-before-persist | observation, belief candidate e official sono layer diversi |
| keep-secondary vs interferenza | archive/shadow/activation policy |
| learn->build dichiarato completo | resta SEED finche manca markdown->schema |
| 4 SWE solved vs 0/5 engaged | smell benchmark separato da agent benchmark |
| fasi upfront vs emergence | DAG di dipendenze, scelta pull-based nella frontiera |

---

## 6. Il kernel agentico che manca

Il contratto corrente di un modulo e:

```c
typedef int (*Handler)(Brain *, const char *norm, const char *raw,
                       char *out, size_t out_size);
```

Quindi ogni parte puo solo vincere e scrivere testo, oppure perdere. Il registry
first-match e utile come struttura secondaria conversazionale, ma non puo
rappresentare un lavoro di coding lungo. L'ordine del registry e diventato una
semantica nascosta e causa regressioni di routing.

### 6.1 Tipi interni minimi

Il nuovo kernel introduce strutture esplicite, senza rimuovere subito gli handler:

```text
Event       fatto osservato, immutabile, con sorgente e timestamp logico
Task        richiesta esterna, vincoli, workspace, budget, policy
Goal        stato desiderato, parent, precondizioni, stato epistemico
Constraint  proprieta che una soluzione deve soddisfare
Action      schema + argomenti + rischio + costo previsto
Observation risultato reale di un'azione, stdout/stderr/status/hash
Artifact    file, diff, patch, indice, build o report con content hash
Verdict     oracle, esito, evidenza e copertura
Gap         stadio, requisito mancante, trace causale, severita
Decision    candidati considerati e ragione della scelta
```

Questi tipi devono avere:

- rappresentazione C interna;
- forma serializzabile JSONL per audit;
- proiezione in fatti KB per query e planning;
- id stabili e riferimenti, non copie di stringhe sparse;
- provenance dal raw input fino al verdetto.

### 6.2 Il ciclo operativo target

```text
INGEST
  conserva messaggi, issue, repository, policy e budget
INTERPRET
  produce goal, constraint, ambiguita e gap iniziali
OBSERVE
  interroga workspace e raccoglie fatti con fonte
PLAN
  deriva passi da action_schema + pre/postcondizioni
PROPOSE
  genera piu azioni o patch candidate
SELECT
  ordina per evidenza, rischio, costo e informazione attesa
ACT
  esegue una primitiva sicura e produce Observation/Artifact
VERIFY
  chiama l'oracolo appropriato
DIAGNOSE
  minimizza il primo contratto rotto
REPLAN
  ripara, cambia ipotesi o acquisisce il fatto mancante
RENDER
  restituisce risultato, prova, diff e limiti
```

Il loop e budgetato per passi, wall clock, processi, byte letti/scritti e tentativi.
Quando esaurisce una risorsa restituisce `budget_exhausted`, non `false`.

### 6.3 Moduli come propositori, non solo vincitori

Migrazione additiva:

1. gli handler correnti restano il fallback;
2. i moduli possono emettere `Proposal` con confidence strutturale ed evidence;
3. un router dichiarativo confronta piu proposal;
4. un composer puo sceglierne piu di una se gli effetti non confliggono;
5. first-match resta una policy selezionabile per classi semplici;
6. le regressioni vengono confrontate in shadow mode prima del cambio champion.

### 6.4 Piano come conoscenza

Le azioni vivono in KB:

```prolog
action(read_file, [path],
       pre([inside_workspace(path)]),
       eff([observed(file,path)]),
       risk(read_only), cost(low)).

action(run_tests, [target],
       pre([workspace_ready, allowed_target(target)]),
       eff([observed(test_verdict,target)]),
       risk(exec), cost(high)).

action(apply_patch, [patch],
       pre([candidate(patch), clean_hashes]),
       eff([workspace_changed, rollback_available]),
       risk(write), cost(medium)).
```

La sintassi concreta potra essere piu semplice, ma precondizioni, effetti,
rischio e costo non devono essere cablati in una pipeline C per-task.

### 6.5 Stato scalabile e articolazione reale

Oggi i frammenti `src/brain/*.c` sono inclusi in una sola translation unit e
condividono `Brain`, helper statici e molti array a capacita fissa. La separazione
fisica aiuta la navigazione, ma non e ancora un confine architetturale.

La migrazione deve essere incrementale:

1. `Brain` resta facade e puo riferire un `BrainImage` frozen con config,
   registry, indici e base KB;
2. ogni conversazione riceve un `SessionContext`: lingua, memoria cross-turn,
   mondi e overlay KB;
3. ogni lavoro riceve un `TaskContext` con arena dinamica, workspace, budget,
   action/event log e candidate artifacts;
4. sessione, workspace facts e candidate artifacts hanno scope e lifecycle
   distinti;
5. i moduli nuovi dipendono da interfacce esplicite, non da ogni helper statico;
6. un frammento passa a translation unit separata solo quando un contratto/test
   rende chiaro il suo confine;
7. il vecchio path resta fallback finche il confronto differenziale e verde.

I limiti correnti del KB (`KB_MAX_ARGS`, `KB_MAX_BODY`, `KB_TERM_LEN`) non vanno
alzati per speculazione. Vanno resi errori/gap osservabili e poi rimossi o resi
dinamici quando una campagna reale li colpisce. Indici, tabling e cache arrivano
solo dopo profiling su KB e repository di scala dichiarata.

---

## 7. Workspace e macchina: produttivita senza perdita di controllo

### 7.1 Executor strutturato

Prima del repair autonomo, `popen(command_string)` e la whitelist a prefisso
devono essere sostituiti da:

- `fork` + `execve`/`posix_spawn` con `argv[]`, mai una shell implicita;
- cwd esplicita e verificata;
- environment allowlist;
- timeout wall-clock e CPU;
- process group terminabile;
- limiti memoria, file size, open files e numero processi;
- cap e hash di stdout/stderr;
- exit status, signal e durata come Observation;
- network off in RUNTIME e nei test non autorizzati.

### 7.2 Contenimento filesystem

- Root aperta con `dirfd`.
- Accesso con `openat` e `O_NOFOLLOW` dove disponibile.
- Path canonico sempre dentro root.
- Symlink, device, FIFO e socket rifiutati salvo policy esplicita.
- Test e oracle montati/read-only o verificati con hash pre/post.
- Nessuna lettura automatica di `.env`, chiavi, home o file fuori workspace.

### 7.3 Edit transazionale

Il risultato di un edit e un `PatchArtifact`, non una scrittura ad hoc:

```text
base tree hash
preimage hash per file
operazioni strutturate
unified diff
postimage hash
file creati/eliminati/modificati
rollback diff
```

Flusso:

1. copia/worktree usa-e-getta;
2. applicazione candidate;
3. parse e compile veloci;
4. test mirati;
5. regressioni pertinenti;
6. presentazione del diff;
7. applicazione atomica al workspace autorizzato;
8. rollback automatico se il post-check fallisce.

### 7.4 Tool registry

Ogni tool ha un contratto versionato:

```text
name, argv schema, input kinds, output parser, side effects,
risk tier, timeout, max output, required capability, oracle role
```

Git entra come tool strutturato: `status`, `diff`, `log`, `show`, `blame`,
`apply --check`. Il planner interroga `tool_for(goal, tool)`; il C esegue il
contratto sicuro.

### 7.5 Approvazioni

| Rischio | Esempio | Default |
|---|---|---|
| R0 | query KB, parse, read file | autonomo |
| R1 | grep, git diff/log, test read-only | autonomo con budget |
| R2 | scrittura in worktree candidato | autonomo e rollback obbligatorio |
| R3 | applicazione al workspace canonico | approvazione o policy esplicita |
| R4 | rete, install, publish, secret | negato salvo autorizzazione specifica |

---

## 8. Workspace intelligence: codice e linguaggio nello stesso substrato

### 8.1 IR comune

Il principio di `CODE-MASTERY.md` resta: AST e repository sono KB. Va completato
con un vocabolario non legato al C:

```text
file, module, symbol, scope, node, definition, reference,
call, read, write, assign, branch, loop, return, exception,
type, value_shape, control_edge, data_edge, test, diagnostic
```

Ogni fatto porta:

- language e frontend version;
- file + span byte/line/column;
- stable node id;
- parent/scope;
- source hash;
- confidence: parsed, derived o hypothesized.

### 8.2 Frontend per delta

- C e Python emettono lo stesso IR condiviso.
- Le differenze sono override/delta: indentazione, macro, dynamic typing,
  comprehension, generator, ecc.
- Gli analizzatori generali parlano IR, non sintassi concreta.
- Il compilatore/interprete reale resta oracle quando la semantica e incerta.

Non serve necessariamente scrivere ogni parser a mano. Un parser deterministico
esterno puo essere un tool, purche il runtime resti autonomo, l'output venga
normalizzato nel nostro IR e la dependency sia dichiarata. Il fallback C puro
resta utile sui sistemi minimi.

### 8.3 Indici di repository

Il modello di workspace deve offrire:

- symbol index e reverse reference;
- call graph e import/include graph;
- test-to-code map derivata da nomi, import e coverage quando disponibile;
- diagnostics index da build/test;
- change history via git;
- cache per hash file;
- invalidazione incrementale dopo patch.

### 8.4 Issue -> regione tramite evidenza

La frontiera F4 non si chiude con keyword hardcoded. Pipeline proposta:

1. estrarre entita, comportamenti attesi, errori, nomi e constraint dalla issue;
2. collegarli a simboli, stringhe, test, docs e diagnostics;
3. costruire candidati di localizzazione con proof;
4. raccogliere nuova evidenza solo dove discrimina le ipotesi;
5. valutare top-k su repo con file decoy;
6. usare il fallimento dei test per aggiornare il ranking.

Metriche: recall top-1/top-5, righe lette prima della localizzazione, false
positive su repo puliti, robustezza a paraphrase della issue.

### 8.5 Semantica incrementale

Ordine tirato dai task, non enciclopedia upfront:

1. scope e binding;
2. CFG essenziale;
3. def-use e side effects;
4. loop e stato;
5. exceptions e resources;
6. alias/value domains quando una issue li richiede;
7. interprocedural summaries;
8. language-specific delta.

Ogni nuova analisi deve risolvere almeno un task reale e trasferire su un
secondo caso mai visto.

---

## 9. Generazione e repair: dal template a CEGIS

La generazione open-domain non puo essere un `printf` piu grande. Il codice e un
candidato in uno spazio di programmi, guidato da constraint e disposto da un
oracolo.

### 9.1 Pipeline

```text
spec/problema
  -> constraint e esempi
  -> sketch strutturale
  -> buchi tipizzati
  -> enumerazione/riscrittura guidata da KB
  -> candidato
  -> compile/test/property oracle
  -> controesempio
  -> raffinamento bounded
  -> patch verificata o gap
```

### 9.2 Fonti dei candidati

- procedure e schemi appresi;
- analogia strutturale con codice del repo;
- regole di trasformazione;
- componenti gia verificati;
- enumerazione limitata da tipi/constraint;
- repair di un candidato vicino;
- mai testo libero non parseable presentato come soluzione.

### 9.3 Repair loop B4

Primo obiettivo: un candidato volutamente errato deve:

1. fallire con un verdict strutturato;
2. collegare diagnostic a nodo/constraint;
3. proporre almeno due fix;
4. scegliere il fix con minore blast radius;
5. riapplicare e rieseguire;
6. fermarsi entro `N` tentativi;
7. conservare tutta la trace.

Solo dopo questo gate il progetto puo dichiarare di avere un coding loop.

### 9.4 Il vero learn -> build

La catena e valida solo se una fonte mai vista causa lo schema usato dal builder:

```text
pagina markdown fresca
  -> passi/constraint estratti
  -> schema candidato con provenance
  -> esempi e proprieta generate
  -> codice sintetizzato
  -> oracle
  -> ablation: senza pagina/schema il task torna rosso
```

Gate iniziali:

- selection sort da pagina non presente nel KB;
- binary search con caso not-found;
- una trasformazione di testo con proprieta verificabili;
- stesso induttore, nessun nuovo `algo_shape` scritto a mano.

### 9.5 RULESCORE riposizionato

RULESCORE resta una miniera di task. Non e un gate di promotion finche un giudice
LLM assegna il voto. Ogni mini-gioco deve essere tradotto in:

- input grammar;
- transition relation;
- state invariant;
- termination/win condition;
- property test o transcript oracle.

I punti reali provengono dall'esecuzione di quelle proprieta.

---

## 10. `make forge`: la nuova fucina

Interfaccia proposta:

```sh
make check TEST=<contract-id>              # Tick: caso focale, streaming
make impact CHANGED=<file-or-package>       # Turn: slice impattata
make forge CAMPAIGN=<id> MODE=knowledge|procedure|engine|agent BUDGET=<n>
make forge-replay CAMPAIGN=<id>
make forge-promote RUN=<run-id>
make forge-rollback CAPABILITY=<id>
make capability-report
make soak                                  # nightly/fresh-exec/sanitizer
```

`autolearn` resta disponibile come esperimento storico finche `forge knowledge`
non lo sostituisce. Il default sicuro di `autolearn` deve diventare
`MULTIPLY=0` nel frattempo.

### 10.1 Seal: sigillare l'esperimento

Prima di ogni run:

- base commit e tree hash;
- semantic payload digest di source, test, KB, policy e manifest gia finalizzati;
- hash binario, KB e tool;
- compiler/runtime version;
- trust mode e policy;
- proposer, judge e seed, se esterni;
- budget e timeout;
- capability baseline;
- challenge pack gia separato.

Per una promotion Forge, tutti i file tracciati e il pre-run decision record
vengono finalizzati **prima** di Release. Si crea poi un candidate commit su una
ref temporanea, senza attivarlo sul branch canonico. Release fa checkout,
costruisce e verifica quell'esatto commit: anche il commit stamp osservabile e
quello definitivo. Il controller conserva l'attestation fuori dal worktree,
indicizzata da candidate commit, release-binary digest e semantic payload digest.
Se verde, il branch viene fast-forwardato a quello stesso commit; se rosso, la
ref viene rifiutata/archiviata. Nessun edit o relink post-gate e ammesso. Un
candidate commit non e una generazione finche non viene attivato.

La baseline non e piu un unico interruttore globale:

- il **core trust set** deve essere verde; se e rosso sono ammessi solo fix e
  lavoro di harness;
- la baseline della capability toccata deve essere verde per attivarla;
- failure preesistenti non pertinenti vivono in un known-red ledger con
  fingerprint, owner e scadenza;
- una candidata puo essere esplorata e restare `shadow` anche durante una
  failure non correlata, ma non puo introdurre alcun nuovo rosso.

### 10.2 Mine: estrarre il minerale

Fonti della frontiera:

- failed lesson di autolearn;
- regressioni e misclaim;
- gap di code/game/SWE/RULESCORE/basic-chat;
- task reali dell'utente non chiusi;
- `needhelp.p0` strutturati;
- counterexample di campagne gia promosse.

La fucina raggruppa per trace causale, non per parole del prompt:

```text
data, schema, representation, parsing, inference, routing,
planning, tool, workspace, synthesis, oracle, rendering, safety
```

### 10.3 Assay: saggiare il fallimento

Output obbligatorio:

- minimo riproduttore rosso;
- stato atteso e osservato;
- primo contratto rotto;
- componente proprietaria;
- materiale minimo ammesso;
- oracle disponibile;
- rischio di overfit;
- casi negativi necessari.

Un miglioramento del solo harness viene marcato `harness`, non conta come nuova
capacita di parrot0.

### 10.4 Alloy: produrre varianti

Ogni candidata vive in overlay o worktree usa-e-getta:

| Classe | Artefatto |
|---|---|
| knowledge | fatti `.p0` con fonte e validator |
| procedure | clausole/rewrite con proof atteso |
| policy | action schema, routing/ranking facts |
| engine | patch C generale + red test |
| agent | patch al kernel loop/tool/workspace |
| harness | oracle o benchmark, contato separatamente |

Il proposer dovrebbe generare piu varianti. Il teacher esterno non vede gli
holdout e non puo modificare oracle o test.

### 10.5 Hammer: ciclo candidato bounded

```text
propose -> parse/build -> execute -> observe -> diagnose -> repair
```

- massimo tentativi esplicito per Tick e Turn;
- ogni tentativo ha parent id;
- nessuna scrittura ufficiale;
- ogni azione e registrata;
- il fallimento e un risultato utile, viene minimizzato e resta nel ledger;
- il ciclo si ferma al primo controesempio: non esegue livelli piu costosi su
  una candidata gia morta;
- piu candidate competono con **successive halving**, non con una full matrix
  ciascuna.

### 10.6 Temper: funnel progressivo, non gauntlet piatta

I vecchi G0-G9 restano requisiti di promotion, ma non sono piu una lista che
ogni edit deve attraversare. Sono distribuiti in corsie con costo e frequenza
espliciti. I budget iniziali si riferiscono all'host di sviluppo dichiarato nel
manifest; ogni device ha un profilo calibrato, non timeout arbitrari.

| Corsia | Chi la paga | Contenuto | Budget P95 iniziale |
|---|---|---|---:|
| **S0 Spark** | ogni edit | parse/schema, build dev incrementale, fixture/oracle hash | <= 2 s warm, <= 5 s con build |
| **S1 Focal** | ogni candidata valida | red esatto, oracle meccanico, 2-3 collision negative | <= 5 s |
| **S2 Impact** | sole sopravvissute | hot corpus e regressioni selezionate dall'impact graph | <= 10 s |
| **T Transfer** | migliori 1-2 | mutation, property, 10x, EN/IT e validation variant visibili | <= 60 s, asincrona |
| **R Release** | un champion frozen | promotion holdout, full offline policy set, oracle integrity, safety, replay, rollback | hard stop 5 min; obiettivo <= 60 s |
| **N Nightly/Field** | promoted/canary | fresh-exec differential, sanitizer, fuzz, cross-repo, Docker/SWE | fuori inner loop |

Regole:

1. S1 non puo diventare verde senza un oracle reale. Il vecchio G6 non arriva
   piu tardi: in Release resta solo la prova che candidato e harness non abbiano
   alterato oracle, test o fixture.
2. Ablation fresca viene eseguita quando una candidata diventa seed; la baseline
   sigillata e content-addressed non viene ricalcolata a ogni edit.
3. Determinismo a tre replay e full safety appartengono al solo champion o al
   nightly, non a ogni sibling.
4. Il promotion holdout viene aperto solo dal champion gia scelto e frozen, mai
   per scegliere tra challenger. Dopo l'apertura diventa train e serve un nuovo
   holdout per la promotion successiva.
5. Il risultato non mescola funzione, esecuzione e cache:
   `execution_state={ran, timeout, infra_error, skipped_policy}`,
   `verdict={pass, fail, unknown}`, `stability={stable, flaky, quarantined}` ed
   `evidence_origin={fresh, exact_cache, selective_cache}`.
6. S0-S2 sono fail-fast e ordinano i casi per: rosso corrente, regressioni
   recenti, collisioni probabili, costo crescente.
7. Un verde su build dev O0 e evidenza locale, non promotion evidence. Prima di
   Transfer il survivor viene ricostruito O2 e il corpus hot deve restare
   differenzialmente equivalente.
8. S0-S2 sono fail-fast per dare direzione. Release/Nightly continuano dopo un
   failure funzionale per produrre la mappa completa, ma streammano subito; una
   safety violation puo fermare l'intera run.
9. Il Release policy set e il suo digest vengono sigillati prima del run. I 5
   minuti sono l'hard timeout iniziale; <= 60 s e l'obiettivo prestazionale di
   M-1b. Riclassificare test dopo il risultato non soddisfa lo SLO.
10. Containment, resource limit, worktree isolation e tamper protection valgono
    da S0 per ogni candidata eseguita. Al champion si rinvia solo la campagna
    adversarial/full-safety, mai il confine di sicurezza di base.

Successive halving predefinita:

```text
N candidate -> tutte S0
            -> massimo 3 a S1
            -> massimo 2 a S2
            -> massimo 2 a T
            -> 1 champion a R
```

Le soglie possono essere alzate per campagne ad alto rischio. Non possono
essere allargate dopo aver visto il risultato. Un sibling non eredita il
successo del seed, ma non e costretto a pagare certificazione completa prima di
aver dimostrato di meritare il livello successivo.

### 10.7 Quench: promotion atomica

Stati di evidenza:

```text
red_reproduced -> local_green -> collision_green -> impact_green
               -> transfer_green -> promotable
```

La maturita resta l'asse ABSENT/SEED/TRANSFER/FIELD/HARDENED di §4. Gli stati
di attivazione sono un asse indipendente:

```text
inactive -> shadow -> scoped_canary -> active
         -> disabled | rolled_back
```

Un package puo quindi essere `TRANSFER + shadow` o `FIELD + active`; `hardened`
non viene usato come sinonimo di attivazione.

Una promotion contiene in un'unica generazione:

- candidato;
- counterexample minimizzati e test guadagnati;
- manifest;
- scorecard;
- provenance;
- decision record;
- activation change;
- rollback testato al livello richiesto dalla policy;
- aggiornamento capability ledger.

Per il KB servono lock, temp file, `fsync`, rename atomico e journal multi-file.
Il save-map suggerisce la destinazione; non e il transaction manager.

### 10.8 Anneal: selezione nel tempo

- Nightly/periodic replay delle campagne promosse.
- Champion e challenger girano in shadow mode sulle stesse trace.
- Una regressione disattiva il candidato e riattiva lo snapshot precedente.
- La variante resta archiviata con la sua evidenza.
- Cluster di rollback diventano nuovi engine gap.
- Consolidamento solo quando due strutture risultano equivalenti su copertura,
  costo e proof.

### 10.9 Artefatti

```text
tests/forge/campaigns/<id>/challenge.json
tests/forge/campaigns/<id>/fixtures/
tests/forge/campaigns/<id>/oracle/
.forge/runs/<run-id>/manifest.json
.forge/runs/<run-id>/candidate.p0 | candidate.patch
.forge/runs/<run-id>/actions.jsonl
.forge/runs/<run-id>/evidence/
.forge/runs/<run-id>/scorecard.json
.forge/runs/<run-id>/decision.json
kb/forge/quarantine/<candidate-id>.p0
docs/forge/ledger.jsonl
capabilities/manifest.json
```

`.forge/` e effimera e dovra diventare gitignored. Campaign, policy e schema del
ledger sono versionati nel candidate commit. Il capability manifest target e
una proiezione generata dall'attestation, indicizzata dal candidate commit e
pubblicata come artifact/annotated ref senza modificare il commit Release-tested;
il file tracciato corrente resta una compatibility view durante la migrazione.
Evidence pesante puo essere content-addressed fuori git, ma il suo digest resta
nell'attestation.

### 10.10 Challenge pack

Ogni campagna dichiara:

```json
{
  "id": "coding.repair.routing-preemption.v1",
  "capability": "agent.routing",
  "source": "regression",
  "base_commit": "...",
  "task": "...",
  "allowed_actions": ["read", "parse", "test"],
  "oracle": "tests/run.sh:agent_search",
  "splits": {
    "train": ["..."],
    "variant": ["..."],
    "holdout": ["hidden/..."],
    "negative": ["..."]
  },
  "mutations": ["rename", "numbers", "language", "decoy"],
  "budget": {
    "steps": 8,
    "spark_seconds": 2,
    "focal_seconds": 5,
    "impact_seconds": 10,
    "transfer_seconds": 60
  },
  "promotion_policy": "agent-routing-v1"
}
```

Gli expected degli holdout non sono disponibili al proposer.

### 10.11 Capability package

L'unita di selezione non e una singola riga KB o una patch isolata. E un package:

```text
identity + version
implementation (facts, procedures, policy o C)
exported predicates/actions
dependencies e incompatibilita
train/variant/holdout/negative tests
oracle e provenance
resource/safety envelope
scorecard e maturity
activation policy
rollback artifact
```

Il package rende concreto il principio di `DESIGN.md` sugli expert composabili:
puo essere caricato, interrogato, messo in shadow, promosso, disattivato e
riattivato senza perdere la storia che lo ha prodotto.

### 10.12 Catalogo granulare dei contratti

`tests/benchmarks.json` resta il catalogo delle aree, ma ogni test eseguibile
deve avere un record granulare:

```json
{
  "id": "routing.agent-search.en",
  "capability": "agent.routing",
  "owner": ["mod_piact", "router"],
  "depends_on": ["src/brain/60-agent-tools.c", "intent_phrase/2"],
  "boot_profile": "hermetic-en",
  "tier": "focal",
  "isolation": "fork",
  "oracle": "response.status+winner+text",
  "timeout_ms": 800,
  "mutators": ["punctuation", "numbers", "language", "decoy"]
}
```

Il catalogo registra anche durata storica P50/P95, risorse esclusive, fixture,
hash dell'oracolo e ultimo failure. Deve essere possibile eseguire un contratto
per id senza conoscere quale shell script lo contiene.

### 10.13 Piramide: semantica prima, processo quando serve

La suite corrente e quasi una piramide rovesciata: molti golden black-box
avviano un processo completo e confrontano testo, anche quando il contratto e
una decisione di routing o inferenza.

La forma target e:

1. **pure/unit:** KB, parser, ranking, planner, patch model e validator senza
   processo o filesystem reale;
2. **Brain contract:** il vero dispatch con stato isolato e risultato
   strutturato;
3. **adapter contract:** pochi test per CLI, HTTP, MCP, executor e persistenza;
4. **end-to-end:** un piccolo insieme verticale per capability;
5. **cold/field:** repository e oracle esterni solo dopo la selezione.

Il kernel dovra esporre, senza duplicare il percorso produttivo, una risposta
strutturata:

```text
text, status, winner, declined_trace, goals, proof,
actions, artifacts, oracle_verdict, gap
```

I test di semantica verificheranno questi campi. I golden testuali resteranno per
il renderer e per pochi contratti pubblici EN/IT. Una modifica di phrasing non
dovra rilanciare o rompere centinaia di prove di planning; una collisione di
routing dovra dire immediatamente quale modulo ha rubato il turno.

### 10.14 Forkserver: ipotesi preferita, non dogma

Il persistent test-engine con `brain_reload()` e utile come fallback, ma non e
piu l'endgame assunto su POSIX. Il candidato principale da benchmarkare e un
forkserver, confrontato con fresh-exec batch e persistent reset su un campione
identico. Il target, se vince, funziona cosi:

1. un parent rigorosamente single-threaded carica un Brain pristine una volta e
   non risponde mai direttamente;
2. per ogni dialogo crea un child con `fork()`;
3. un hook post-fork aggiorna PID/clock, seed e fatti riflessivi dipendenti dal
   processo, chiude descriptor ereditati non ammessi, poi esegue la sessione e
   restituisce un verdict strutturato;
4. l'uscita del child elimina tutto lo stato mutabile;
5. piu parent pre-riscaldati servono i diversi boot profile.

Copy-on-write evita exec, dynamic load e parsing ripetuto, ma mantiene un
confine RAM di processo: un leak di sessione non puo contaminare il test
successivo. Non isola da solo filesystem o processi figli. Ogni child riceve
cwd/tmp propri, umask, process group, timeout+`killpg`, RLIMIT, signal reset e FD
allowlist/`closefrom`. Persist/restart, daemon, MCP transport e casi con side
effect restano `fresh_exec`.

Finche i buffer statici mutabili non migrano nei context, parrot0 e dichiarato
non-reentrant: parallelismo tra processi/pool di parent single-thread, non thread
che condividono un Brain.

Prima della promotion del runner si misurano wall time, CPU, RSS/COW fault e
divergenza su almeno 50 casi rappresentativi. Kill criterion: se entro due
generazioni non riduce almeno 2x il costo del campione senza divergenze rispetto
al fresh-exec, si mantiene il batching/runner piu semplice e il forkserver torna
candidate. Il differential richiede equivalenza **canonica** dei campi stable e
degli hash dei side effect; PID, clock, temp path e durata sono volatile, non
byte-equivalenti.

Il forkserver puo iniziare clonando via COW l'intero Brain; non deve aspettare un
refactor del KB. Solo quando task reali lo tirano, lo stato target si separa in:

```text
BrainImage      config, registry, indici e KbImage frozen condivisibili
SessionContext  lingua, memoria cross-turn, mondi e KbOverlay/tombstone
TaskContext     budget, workspace, event log, candidate e artifact del job
```

Il KB attuale mescola base e sessione negli stessi array: `KbImage/KbOverlay`
richiede query union, provenance, override, negativi e retract differentialmente
equivalenti. `Brain` puo restare la facade che possiede image, sessione e task
attivo. Questa separazione puo poi accelerare test/runtime e giustificare lo
split graduale della grande TU, ma non e un prerequisito del primo speedup.

### 10.15 Impact graph, corpus a due temperature e canary

La regression pertinente non e una scelta intuitiva. Il selettore unisce:

- ownership dichiarata di file, simboli, predicate e action schema;
- trace dinamica di tutti i moduli consultati, winner, branch/coverage, proof,
  oracle e anche lookup/cue/predicati falliti;
- reverse dependencies tra capability package;
- storia dei test che hanno fallito su cambi simili;
- un canary globale piccolo e stabile.

Il target raggruppa i 243 dialoghi per trace/decision fingerprint. Il corpus **hot**
mantiene per capability almeno: positivo, collisione negativa, caso stateful,
EN/IT quando pertinente, 10x e regressioni recenti. Il corpus **cold** conserva
copertura completa, mutation ampia e holdout per Release/Nightly.

Se cambia il fingerprint di un rappresentante, il cluster viene espanso. Il
cluster include anche capability/mutator class e un campione rotante
deterministico, perche winner/proof da soli non distinguono branch interni. Se la
modifica non ha ownership nota, il selettore scala conservativamente. Il nightly
audita il selettore contro la full matrix: un miss e una regressione del runner,
non una scusa per rendere il mapping ottimistico.

Un selector miss osservato e un test che fallisce nel full audit di una patch
rossa storica o mutation ma che non apparteneva all'Impact set predetto. Si
pubblicano numeratore, denominatore e recall; una full verde da sola non prova
soundness. Shared helper, registry order, schema KB, config o dependency
negative sconosciuta fanno fallback conservativo alla matrice pertinente.

### 10.16 Cache di evidenza, non cache di speranza

Esistono due namespace che non vanno confusi:

- **exact evidence:** `test-id + full-semantic-tree/release-artifact + KB +
  fixture + oracle + env/toolchain + policy`; serve a trasformazioni pure come
  capability-report e a non rieseguire due volte lo stesso gate artifact;
- **selective advisory:** `test-id + dependency-closure + candidate-package +
  KB-slice + fixture/oracle + dev-profile`; accelera solo S0-S2 e non autorizza
  una promotion.

Per S0-S2 il fingerprint semantico puo escludere la sola metadata di versione;
per Release resta l'hash dell'artefatto esatto. Isolare `version.c` evita che un
commit semanticamente neutro sporchi tutti gli object. Inizialmente ogni
champion esegue fresca l'intera ratchet offline sigillata; si riusano soltanto
acquisizioni immutabili o un gate-result sull'identico digest.

- Il replay di determinismo e sempre fresco.
- `make gate` dovra scrivere un `gate-result.json` attestato dai digest di input.
- `make capability-report` dovra consumare quell'artefatto quando coincide,
  invece di rieseguire tutti i target.
- L'ablation confronta il candidate verdict con la baseline gia sigillata e
  ricalcola solo il lato modificato.

"Attestato dai digest" non significa firma crittografica. Il controller fidato,
fuori dal worktree candidato, produce l'artefatto in uno store append-only; il
candidato non puo scriverlo. Si parla di firma solo se esiste davvero una chiave
del controller.

### 10.17 Scheduler e feedback visibile

Il runner target possiede parallelismo e risorse. Non si annidano 22 processi per
target dentro altri target paralleli senza controllo. Ogni test dichiara token
CPU, porta, directory mutabile, rete e isolamento; lo scheduler bilancia shard
per durata storica, non per semplice numero di file.

Ogni run riceve tmp/log/workspace e porte effimere proprie. `p0tmp_*`, log
globali, fixture riscritte e output di build condivisi devono essere eliminati o
protetti da una risorsa esclusiva. La build avviene una volta prima degli shard;
due `make` non scrivono contemporaneamente gli stessi object o `version.h.tmp`.

L'output interattivo dovra essere streaming:

```text
[S1 1/4 START] routing.agent-search.en
[S1 1/4 PASS ] 184 ms  winner=piact
[S1 2/4 FAIL ] 121 ms  expected=piact observed=compound_math
first broken contract: router.ownership
next: stop (candidate rejected)
```

Il report finale resta ordinato e riproducibile, ma non trattiene il primo
verdetto. Sono sempre visibili queue time, build time, test time, cache status,
ETA e ragione di escalation.

### 10.18 Replay degli oracle costosi

Planner, repair e localizzazione devono poter esplorare su Observation e
diagnostic sigillati gia raccolti. Il record/replay non e un giudice simulato:

- il trace nasce da compiler, pytest, filesystem o Docker reale;
- la stessa `ToolPort`/vtable alimenta fake, replay e live;
- ogni Action deve combaciare esattamente per schema/versione/argomenti/input
  digest con un arco della capsule; una deviazione produce `replay_miss`, mai la
  prossima risposta canned e mai un fallback live automatico;
- fixture, working-tree snapshot, tool, OS/env o container image invalidano il
  replay;
- il core decisionale puo provare molte policy in millisecondi;
- solo il survivor torna all'adapter/oracle reale.

I livelli di evidenza restano distinti:

```text
model/fake       prova solo invarianti del planner
strict replay    prova decisioni sugli archi osservati
live hermetic    prova adapter, filesystem/compiler e side effect reali
field            prova l'envelope esterno
```

Un transcript lineare non supporta una policy che sceglie un'azione nuova: la
capsule deve essere un grafo keyed by Action, oppure la nuova azione resta
simulation-only e scala esplicitamente a live. Un pass replay non diventa claim
di agency live.

Ogni failure costoso viene ridotto a un **counterexample capsule**: input minimo,
Action/Observation con request hash, exit/signal, stdout/stderr e truncation,
proprieta rotta, owner e oracle. Le capsule vengono sanificate da secret e path
host non necessari. Non si riapre Docker o una API per
rediscoverire a ogni edit lo stesso fatto.

---

## 11. Autolearn: cosa conservare e cosa riforgiare

### 11.1 Parti buone da riusare

- sessione MCP per-worker isolata;
- probe prima/dopo;
- sanitizer di fatti;
- ledger delle failed lesson;
- persistenza separata dalla fase di tentativo;
- parallelismo dei round;
- diagnosi lezione vs engine gap.

### 11.2 Difetti da chiudere

- stesso modello come interviewer, judge, teacher, multiplier e diagnoser;
- verifica sullo stesso prompt visto;
- whitelist che accetta predicati binari arbitrari;
- multiplier senza re-probe;
- batch truth audit solo linguistico;
- persistenza diretta nell'albero curato;
- nessuna full regression prima del persist;
- successi rimossi dalla frontiera senza replay;
- nessun controllo del risultato della persistenza;
- nessuna transazione multi-file.

I dati mostrano gia danni concreti: cue accumulate in un riddle all-cues,
`tr_*_phrase` usati per frasi composizionali e predicati senza consumer promossi.

### 11.3 Schema registry unico

La whitelist Python deve diventare conoscenza/manifest dichiarativo:

```text
trainable_schema(
  predicate, arity, consumer, validator, source_policy,
  conflict_policy, holdout_generator, promotion_policy
)
```

Un predicato non registrato non puo essere promosso. Un fatto registrato ma mai
letto dal consumer fallisce S0. Un validator deve controllare semantica locale,
non solo forma.

### 11.4 Provenance e verita

Layer distinti:

```text
OBSERVATION  testo o output grezzo, immutabile
CANDIDATE    interpretazione/relation con supporto
OFFICIAL     verificato e promosso
REFLECTIVE   derivato dallo stato runtime
REJECTED     conservato per evitare di riapprenderlo
```

Ogni clause mantiene source id, extractor, proposer, validator, support set,
counterexamples e activation state. Una correzione invalida le conclusioni che
dipendono dal fatto, non solo la riga stessa.

---

## 12. Metriche: cosa significa non essere da meno

### 12.1 North star: Verified Task Completion

Un task vale `VTC=1` solo se:

- task freddo, non nel train;
- repository al commit dichiarato;
- issue completa passata all'agente;
- nessun hint su file/funzione/fix;
- patch prodotta dall'agente;
- FAIL_TO_PASS verde;
- PASS_TO_PASS verde;
- nessun test/fixture/oracle alterato;
- budget e policy rispettati;
- trace e proof presenti;
- risultato riproducibile.

Tutti gli altri casi producono un vettore di sub-goal, non un falso solve.

### 12.2 Scorecard coding

| Asse | Metriche |
|---|---|
| understand | constraint recall, ambiguity detection, informed gap precision |
| localize | top-1/top-5, false positives, byte/file read budget |
| plan | valid preconditions, replans utili, passi superflui |
| act | tool success, unsafe attempts, side-effect accuracy |
| edit | apply rate, file precision, diff size, blast radius |
| verify | correct oracle chosen, fabricated success = 0 |
| repair | failure recovery rate, attempts to green, oscillation rate |
| learn | heldout transfer, ablation, contradiction/rollback rate |
| efficiency | wall time, CPU, RAM, tool calls, external cost in forge |
| honesty | correct decline, misclaim, timeout reported correttamente |

### 12.3 Hard invariant

- nuove regressioni rispetto alla baseline fingerprinted: **0** per promotion;
- successi dichiarati senza oracle disponibile: **0**;
- fatti official senza provenance: **0**;
- test tampering: **0**;
- sandbox escape/rete non autorizzata/secret read: **0**;
- replay non deterministico senza causa dichiarata: **0**.

### 12.4 Metrica di accelerazione

```text
leverage = heldout_edges_closed
           / (new_engine_LOC + weighted_new_schemas + maintenance_cost)

learning_velocity = useful_directional_decisions / interactive_wall_clock

promotion_velocity = heldout_edges_closed / promotion_cpu_seconds
```

Non serve ottimizzare letteralmente una formula. Serve impedire che 50 nuovi
fatti vengano confusi con una nuova facolta. Le metriche da osservare sono:

- task holdout chiusi per generazione;
- gap chiusi senza C;
- riuso di una procedura in domini diversi;
- costo marginale di aggiungere un linguaggio/repo;
- regressioni evitate dalla promotion policy;
- tempo tra failed lesson ed engine pull verificato;
- edit-to-first-verdict P50/P95;
- time-to-first-counterexample P50/P95;
- candidate valutate e scartate per ora;
- percentuale di candidate che raggiunge ogni livello del funnel;
- cache hit valida, selector miss e tempo speso in queue/build/test;
- CPU-secondi per controesempio nuovo e per promotion.

SLO iniziali sull'host di riferimento:

| Segnale | Obiettivo |
|---|---:|
| exact/focal warm P95 | <= 5 s |
| impact P95 | <= 10 s |
| primo evento visibile | <= 500 ms |
| full offline Temper | <= 60 s in background |
| selector miss osservati su corpus storico+mutation | 0, con denominatore pubblicato |

Finche questi SLO non tengono, il numero di test o feature non e una misura di
progresso. Il runner e il collo di bottiglia della crescita e ha priorita.

### 12.5 LLMSCORE, basic-chat e mimic

- sono sonde di breadth, naturalezza e discovery;
- alimentano la miniera della fucina;
- non promuovono fatti o codice;
- non sono la north star del coding agent;
- un aumento ottenuto con phrasebook vale zero leverage.

---

## 13. Dependency DAG

```text
W0 truth ledger (gia avviato)
          |
          v
FAST FORGE: focal runner + forkserver + trace + impact + cache
      |                    |                         |
      |                    |                         +--> promotion/nightly
      |                    |
      |                    +--> counterexample capsule + record/replay
      |                                      |
      v                                      v
typed decision core <---------------- simulated/replayed observations
      |                                      |
      +--> planner/action schemas             +--> code IR/index fixtures
      |            |                                 |
      |            +--> repair/CEGIS                  +--> issue -> region
      |                         |                              |
      +-------------------------+------------------------------+
                                |
                                v
                    secure live adapters + PatchArtifact
                                |
                                v
                     cold task / VTC / field oracle

Forge knowledge --> procedure proof --> markdown/spec -> schema -> builder
                                                        |
                                                        +--> Forge engine
                                                                |
                                                                +--> dogfood
```

Il planner non deve aspettare che l'intero executor sicuro sia finito per poter
essere esplorato: lavora contro tool contract e Observation replayate. L'executor
sicuro resta un prerequisito per l'attivazione live, non per i test del core.
Simmetricamente, localizer e repair possono evolvere su IR e diagnostic
sigillati; il survivor viene poi ancorato al parser, filesystem e oracle reali.

Questa separazione ports/adapters rompe la vecchia catena tutta seriale senza
indebolire la prova finale. Dentro ogni nodo, il prossimo incremento e scelto
dal primo gap con migliore combinazione di frequenza, oracle, riuso, rischio e
**informazione attesa per secondo**.

---

## 14. Roadmap a ondate

Le ondate non assegnano date o un numero rigido di generazioni. Ogni riga resta
una generazione distinta e viene saltata se la pressione reale indica un
prerequisito diverso.

### W0a - Verita della baseline (prima parte completata)

**Obiettivo:** sapere cosa misura ogni benchmark e impedire claim falsi.

Gen313-gen317 hanno gia chiuso le regressioni iniziali, il primo manifest delle
semantiche, il capability ledger e il version drift. Restano hash granulari di
oracle/fixture, known-red ledger e l'audit delle classificazioni: `glue`,
`mimic`, MMLU e BBH mostrano che il manifest corrente non coincide ancora con
il comportamento degli script. `make gate` ha l'intento corretto, ma la sua
forma seriale e opaca non e il loop interattivo.

**Gate di uscita corretto:** una baseline core/coding/agent sigillata e
fingerprinted. I tre replay globali vengono rimossi da W0: appartengono al
champion Release/Nightly e non devono essere duplicati.

### W0b - Fast Forge, direzione prima della certificazione

**Obiettivo:** rendere un esperimento economico, progressivo e attribuibile
prima di aggiungere altre feature o altra burocrazia di promotion.

Deliverable:

1. profiler granulare e protocollo eventi streaming con first-failure immediato;
2. `make check TEST=<id>` e catalogo incrementale con owner, costo, tier,
   isolamento, dipendenze e oracle; M-1a richiede un solo contratto reale, poi
   ogni area migra quando viene toccata;
3. build tree separati `obj/dev` + `bin/dev` e `obj/release` + `bin/release`, dipendenze reali
   `-MMD -MP` e benchmark O0/Og/O1 su **compile + focal runtime**; oggi il solo
   TU brain costa 1.92 s a O0 contro 9.33 s a O2, ma vince il profilo totale piu
   rapido, non O0 per decreto;
4. versione/commit isolati in una piccola TU/header nel build dir, senza
   ricompilare ogni object o cambiare il fingerprint semantico dell'engine;
5. `gate-result.json` content-addressed; capability report e ablation consumano
   evidenza valida senza rilanciare la stessa matrice;
6. scheduler centrale fail-fast con resource token, porte effimere e shard per
   durata storica;
7. benchmark fresh-exec batching vs persistent reset vs forkserver; implementare
   il vincitore con differential fresh-exec e kill criterion;
8. response/trace strutturata e separazione dei golden di rendering;
9. impact graph dichiarativo + dinamico, corpus hot/cold e canary globale;
10. cache CAS con audit dei miss del selettore;
11. counterexample capsule e strict record/replay per oracle costosi.

Questa non e una mega-fase da completare tutta prima di tornare alle capability.
Si promuove in tre milestone utilizzabili subito:

- **M-1a Direction:** item 1-2, un contratto focale reale con streaming e
  fail-fast entro 5 s. Dopo questo W1-W9 possono riprendere con slice verticali;
- **M-1b Throughput:** item 3-6, niente doppio gate/report e full offline <= 60 s
  in background;
- **M-1c Scale:** item 7-11 solo quando il profilo li tira: runner vincitore,
  trace/impact/cache/replay, ciascuno con speedup o decisione reale misurata.

Gate cumulativo di M-1: per 20 modifiche consecutive, focal P95 <= 5 s, impact
P95 <= 10 s, primo evento <= 500 ms; selector con zero miss **osservati** sul
corpus rosso dichiarato; equivalenza canonica col fresh-exec. Non serve attendere
M-1c per esplorare, ma nessuna wave puo aggiungere un nuovo black-box sincrono o
peggiorare gli SLO senza prima ripagare il costo.

### W1 - Forge knowledge v1

**Obiettivo:** rendere sicuro cio che autolearn prova gia a fare.

Deliverable:

1. estrarre sanitizer/diagnosi/persist in componenti testabili offline;
2. default `MULTIPLY=0`;
3. schema registry unico;
4. full audit ledger anche per i successi;
5. overlay KB di quarantena;
6. challenge pack con variant/negative/holdout;
7. ablation automatica;
8. promotion atomica + rollback;
9. revalidation periodica.

**Gate di uscita:** una campagna fatti e una campagna procedure raggiungono
TRANSFER su holdout; un candidato cattivo viene respinto; un canary viene
rollbackato correttamente.

### W2 - Macchina sicura e transazionale

**Obiettivo:** dare mani reali senza trasformare il prompt in shell.

Deliverable:

1. executor argv-based senza `popen`;
2. path containment con `dirfd/openat`;
3. resource/network isolation;
4. tool contracts;
5. PatchArtifact multi-file;
6. apply-check, atomic apply e rollback;
7. git status/diff/log/blame come osservazioni;
8. test tamper detection.

**Gate di uscita:** suite adversarial safety verde; edit multi-file in worktree,
test e rollback riproducibili.

W2 e prerequisito per **agire live**, non per esplorare W3: planner e repair
usano da subito gli stessi tool contract contro adapter in-memory o Observation
replayate. Il passaggio al vero executor avviene solo dopo i contract test.

### W3 - Agent kernel MVP

**Obiettivo:** il primo workflow completo su un task single-repo.

Deliverable:

1. Event/Task/Goal/Action/Observation/Artifact/Verdict/Gap;
2. event log JSONL;
3. action schemas nel KB;
4. planner puro che deriva `inspect -> edit -> verify` su trace replayate;
5. repair loop bounded su candidato volutamente errato;
6. renderer finale con diff, test e limiti;
7. adapter multi-turn: system, history, tool results, input multiline;
8. checkpoint/resume del task.

**Gate di uscita:** prima lo stesso core chiude rapidamente un counterexample
capsule, poi su una fixture live mai vista localizza, modifica, fallisce una
prima verifica, ripara e passa; ablation di un'action schema rende il gap
esplicito.

### W4 - Repository intelligence

**Obiettivo:** lavorare su codebase, non su snippet.

Deliverable:

1. IR comune C/Python con stable node id;
2. symbol/reference/call/import index;
3. scope + CFG + def-use essenziali;
4. incremental invalidation;
5. issue constraint extraction;
6. ranking issue->file/symbol con proof;
7. repo con decoy e clean negatives;
8. test-to-code map runtime per la selezione dei test del repository target; la
   mappa dei test di parrot0 stesso e gia anticipata in W0b.

**Gate di uscita:** localizzazione top-5 >= 80% su una campagna fredda multi-repo,
top-1 misurato e nessun hint dell'harness; i numeri sono iniziali e vanno
inaspriti dopo il primo baseline.

### W5 - Repair freddo e feature piccole

**Obiettivo:** passare da smell curati a task reali completi.

Deliverable:

1. corpus SWE statico con train/validation/holdout per repo e bug class;
2. issue completa al kernel;
3. patch multi-file;
4. test selection e full regression;
5. diagnostica -> controesempio -> replan;
6. prime feature-add, non solo bug smell;
7. linguaggi come delta sullo stesso IR.

**Gate di uscita:** primi VTC=1 su istanze cold non usate per costruire smell;
PASS_TO_PASS intatto; replay identico; decline corretti sulle istanze fuori
envelope.

### W6 - Learn -> build causale

**Obiettivo:** imparare una procedura nuova e usarla per produrre codice.

Deliverable:

1. estrazione di passi/constraint da markdown;
2. schema candidato con proof e source span;
3. esempi/property generati dalla procedura;
4. CEGIS bounded;
5. selection sort e binary search da pagine fresche;
6. stessa pipeline su una trasformazione non algoritmica;
7. correzione della fonte e invalidazione del codice derivato.

**Gate di uscita:** tre procedure nuove, zero `algo_shape` curati per esse,
holdout e ablation positivi, codice verificato.

### W7 - Forge engine

**Obiettivo:** trasformare engine gap ripetuti in primitive generali selezionate.

Deliverable:

1. clustering automatico dei Gap;
2. minimizzazione del red test;
3. candidate patch C in worktree isolati;
4. proposer e critic separati;
5. build, sanitizer, gate, performance e differential test;
6. canary e activation registry;
7. approvazione esterna obbligatoria alla promotion;
8. rollback e postmortem automatici.

**Gate di uscita:** una failed lesson produce un red test, almeno due candidate
patch, una promotion che chiude un holdout e una candidata respinta per
regressione. Il soggetto non modifica mai direttamente il proprio branch.

### W8 - Dogfood e campo

**Obiettivo:** parrot0 tratta se stesso come una codebase ordinaria.

Deliverable:

1. cold task su `src/` senza accesso privilegiato;
2. proposta di patch + proof + benchmark delta;
3. review esterna e promotion forge;
4. sessioni lunghe checkpointable;
5. profiling e indici KB;
6. modularizzazione restante delle translation unit quando tirata da confini
   runtime; la prima estrazione guidata dalla latenza appartiene a W0b;
7. campagne su repository, linguaggi e build system diversi.

**Gate di uscita:** una generazione utile di parrot0 nasce dalla pipeline forge
end-to-end, viene verificata, approvata e puo essere rollbackata.

### W9 - Espansione dell'envelope

**Obiettivo:** avvicinare progressivamente la breadth di un coding agent LLM.

Frontiere:

- debugging di stato, concurrency e resource lifecycle;
- refactor con equivalenza differenziale;
- test generation e mutation testing;
- code review con proof e severity;
- documentazione derivata dal codice;
- nuovi linguaggi tramite frontend delta;
- package/build ecosystems;
- universal solver su specifiche matematiche e tecniche;
- collaborazione asincrona via `needhelp.p0` tipizzato.

Ogni nuova frontiera entra solo con corpus freddo e oracle. Nessuna espansione
giustifica la perdita degli hard invariant.

---

## 15. Le prime 12 generazioni proposte

Le prime cinque generazioni della lista originale sono state sostanzialmente
chiuse da gen313-gen317. Hanno anche mostrato il problema: `make test` impiega
circa 171 s, l'insieme dei sei gate 233.52 s e capability-report puo pagarlo di
nuovo: gate + report arrivano a circa 7m47s. La sequenza viene quindi
sostituita. Queste sono le prossime dodici promotion piccole; tra una promotion
e l'altra sono ammessi molti esperimenti
scartabili.

| # | Contratto singolo | Gate |
|---:|---|---|
| 1 | indirizzare `routing.agent-search.en` per id con START/PASS/FAIL streaming | quel contratto focale < 5 s, fail-fast reale |
| 2 | esporre le righe `expect` di `llmscore_world` per id, provandone una | identico oracle; un probe scelto gira da solo |
| 3 | rendere manifest e exit semantics coerenti per glue/mimic/MMLU/BBH | audit automatico rosso sulla classificazione falsa |
| 4 | separare `obj/dev`+`bin/dev` da `obj/release`+`bin/release` | cambiare CFLAGS non riusa object incompatibili |
| 5 | scegliere O0/Og/O1 sul tempo compile+focal, non per ipotesi | profilo dev vincente e hot differential O2 verde |
| 6 | generare dipendenze `.d` precise | header locale ricompila solo la closure reale |
| 7 | isolare commit/version in TU e build dir | commit neutro ricompila version object + link, non il brain |
| 8 | fare emettere al gate corrente un artifact exact con digest | un run produce verdict riusabile e auditabile |
| 9 | rendere capability-report trasformazione pura dell'artifact | gate+report eseguono i sei target una volta sola |
| 10 | isolare porte/tmp/log di piagent e sortlearn | due run concorrenti non collidono |
| 11 | shardare i 125 casi di `llmscore_world` sotto un budget globale | 68.95 s ridotti senza nested oversubscription |
| 12 | scheduler globale per gli atomici estratti, senza nested parallelism | full policy set <= 60 s oppure nuovo profilo rosso preciso |

Ogni riga deve ridurre una latenza misurata o chiudere nello stesso incremento
un counterexample reale. Infrastruttura che non migliora un numero o una
decisione non merita una generazione.

Il primo pull M-1c e poi il torneo su 50 casi tra fresh batch, reset e
forkserver; il vincitore viene implementato solo se il gate #12 mostra che serve
ancora e se supera il kill criterion di §10.14.

Poi la frontiera sceglie tra:

- typed agent kernel;
- PatchArtifact multi-file;
- repair loop su candidato volutamente errato;
- adapter multi-turn;
- IR/repository index.

La scelta usa evidenza: frequenza del gap, qualita dell'oracolo, riuso atteso,
rischio e effort. Non usa il fascino della feature.

---

## 16. Tre campagne fondatrici

### C1 - Routing senza collisioni (campagna storica da replay)

**Minerale storico:** `agent_search` veniva risolto da aritmetica; due code
request venivano risolte da knowledge. I fix sono atterrati in gen313-gen315;
ora la campagna serve come corpus rosso per validare selector e trace.

**Scopo:** introdurre ownership/proposal evidence senza hardcodare i prompt.

**Holdout:** stessi intenti con numeri, nomi, lingua e ordine differenti; negativi
di aritmetica pura e query knowledge pure.

**Promotion:** il champion passa la Release policy corrente, ablation positiva e
trace mostra la ragione della selezione; non si invoca una lista stale di
"vecchi gate" durante ogni Tick.

### C2 - Procedura fresca -> codice

**Minerale:** learn-and-build usa uno shape curato.

**Scopo:** una pagina selection-sort mai vista produce constraint, schema,
candidato C e property verdict.

**Holdout:** binary search o insertion sort con lo stesso estrattore; nomi e
formato della pagina mutati.

**Promotion:** nessun schema specifico aggiunto a mano; senza pagina/candidato il
build non avviene; con il candidato property e sanitizer passano.

### C3 - Prima issue fredda

**Minerale:** una issue Python completa con repo multi-file e decoy.

**Scopo:** interpret -> localize -> patch -> pytest -> repair.

**Holdout:** stessa bug class in un repo diverso e una issue semanticamente
vicina ma senza bug, per misurare false positive.

**Promotion:** VTC=1, PASS_TO_PASS verde, nessun hint su file, patch o smell.

---

## 17. Scheduler della crescita

La fucina puo proporre il prossimo pull, non promuoverlo autonomamente.

Per ogni cluster di gap registra:

```text
frequency       quante volte ricorre
severity        quanto blocca un task utile
oracle_quality  quanto il verdetto e meccanico
reuse           quanti assi/campagne puo sbloccare
transfer        probabilita motivata di generalizzazione
risk            sicurezza e blast radius
effort          generazioni e costo di verifica
verification_ms costo storico per ottenere il prossimo verdetto
information     quante ipotesi il prossimo test puo discriminare
```

Priorita indicativa:

```text
priority = frequency * severity * oracle_quality * reuse
           * information
           / (risk * effort * verification_ms)
```

Non e una funzione da ottimizzare ciecamente. Serve a rendere esplicita la
decisione e a rispettare la pivot duty:

- se tre primitive non avanzano un task reale, tornare domain-pull;
- se tre task richiedono lo stesso workaround, promuovere il gap a engine pull;
- se il C cresce piu velocemente dei predicati/regole riusabili, fermarsi;
- se un benchmark puo essere vinto con una scorciatoia, rafforzare prima l'oracolo;
- se i test rallentano la selezione, ottimizzare il feedback prima delle feature.

La scelta di **cosa provare subito** e distinta da cosa promuovere. Per orientare
il prossimo Tick vince il test economico che separa meglio le ipotesi vive. Per
la promotion continuano a vincere correttezza, hard invariant, transfer e minor
blast radius; la velocita non puo compensare una risposta falsa.

---

## 18. Self-model e capability truth

Il self-model non deve piu derivare solo da `module(name)`.

Fatti riflessivi target:

```text
capability(code_localization, transfer).
supported_by(code_localization, campaign_id).
verified_at(code_localization, commit, timestamp).
oracle_for(code_localization, code_bench).
known_gap(code_localization, issue_to_symbol).
budget_profile(code_localization, profile_id).
```

Regole:

- lo stato viene generato dal capability manifest al boot;
- non viene persistito come conoscenza di sessione;
- disabilitare un capability package rimuove la capacita dichiarata;
- una regressione abbassa automaticamente la maturita;
- "cosa sai fare?" distingue seed, transfer, field e hardened;
- "perche?" cita test e commit, non una descrizione scritta a mano.

---

## 19. Criteri di milestone

### M-1a - Direzione immediata

- almeno un contratto reale indirizzabile per id, risultato streaming e
  fail-fast;
- focal P95 <= 5 s e primo evento <= 500 ms;
- timeout, flaky e infra error distinti dai fallimenti funzionali.

### M-1b - Throughput di promotion

- build dev/release separate e dipendenze incrementali corrette;
- capability report non riesegue gate gia attestati;
- full offline policy set <= 60 s in background sull'host di riferimento;
- runner senza collisioni di porta/workspace o nested oversubscription.

### M-1c - Scala selettiva

- il runner persistente/forkserver scelto dal benchmark e canonicamente
  equivalente al fresh-exec sul campione;
- impact selector con zero miss osservati sul corpus rosso storico+mutation,
  recall e denominatore pubblicati, fallback full su ownership ignota;
- strict replay rifiuta Action non presenti e non produce claim live.

M-1a e la milestone corrente; abilita subito nuove slice verticali. M-1b e M-1c
vengono tirate dai profili, non diventano mesi di infrastruttura upfront. Senza
M-1a ogni nuovo ratchet aumenta il tempo di apprendimento del sistema.

### M0 - Baseline affidabile

- tutti i ratchet core/coding/agent verdi;
- capability report riproducibile;
- degrade e gate non confusi;
- version e manifest coerenti.

### M1 - Forge sicuro per conoscenza e procedure

- quarantena, holdout, ablation, promotion e rollback;
- nessun multiplier non verificato;
- provenance official al 100%;
- almeno due capability TRANSFER senza C specifico.

### M2 - Coding agent single-repo produttivo

- workflow tipizzato completo;
- edit transazionale;
- test e repair bounded;
- API multi-turn;
- decline/gap precisi.

### M3 - Repair su repository freddi

- issue completa;
- localizzazione senza hint;
- patch multi-file;
- primi VTC su holdout;
- false positive misurati su repo puliti.

### M4 - Builder che impara

- documentazione fresca -> procedure/constraint -> codice;
- almeno tre domini/schema mai curati;
- CEGIS e property oracle;
- correzione della fonte propaga invalidazione.

### M5 - Self-forge supervisionato

- engine gap -> red test -> candidate patch C -> gate -> canary;
- parrot0 e una codebase ordinaria;
- approvazione esterna finale;
- rollback dimostrato.

### M6 - Parita misurata nell'envelope

- envelope pubblicato: linguaggi, tool, task e budget supportati;
- confronto blind con coding agent LLM sugli stessi task freddi;
- VTC, tempo, costo, regressioni e safety pubblicati;
- parrot0 non e inferiore nel solve rate dell'envelope dichiarato;
- mantiene i suoi vantaggi: offline, deterministicita, proof, costo locale e
  zero fabricated success.

M6 non dichiara AGI ne equivalenza universale. Dichiara una parita concreta,
riproducibile e allargabile.

---

## 20. Cosa non fare

- Non aggiungere un LLM al runtime, neppure come fallback.
- Non promuovere sulla stessa domanda usata per insegnare.
- Non usare lo stesso LLM come unica fonte e unico giudice.
- Non moltiplicare fatti senza probe e validator propri.
- Non accettare predicati senza consumer registrato.
- Non persistere direttamente dalla sessione nella base ufficiale.
- Non usare prefix matching come sandbox di comandi.
- Non lasciare che il candidato modifichi test, oracle o holdout.
- Non chiamare SWE solve un harness che omette la issue.
- Non chiamare learn->build una catena con schema curato a mano.
- Non fare un modulo C per ogni costrutto che puo essere una regola.
- Non lasciare tutte le varianti attive solo per rispettare keep-secondary.
- Non eseguire la full matrix dopo ogni edit o per ogni candidata.
- Non usare golden testuali per verificare logica che puo avere un verdict
  strutturato.
- Non usare un reset parziale in-process come sostituto dell'isolamento senza
  differential fresh-exec.
- Non fidarsi dell'impact selector per una promotion senza audit/full Release.
- Non riaprire un oracle esterno per ogni tentativo quando una Observation
  sigillata puo guidare il core.
- Non rieseguire in capability-report gli stessi gate appena certificati.
- Non nascondere output e primo failure fino al termine della matrice.
- Non rendere il grand plan una coda rigida: i gate devono poter cambiare il
  prossimo pull.
- Non ottimizzare LLMSCORE a scapito di verita, agency e coding VTC.
- Non auto-committare o auto-promuovere dal soggetto sperimentale.

---

## 21. Frase guida

> **Scaldare un fallimento reale, batterlo in piu varianti, temprarlo su casi che
> non ha visto, e promuoverlo solo quando l'oracolo dimostra che e diventato una
> capacita.**

Il Tick deve dare direzione prima che si raffreddi l'ipotesi; il martello grande
cade una volta sola sul metallo che ha gia superato i colpi piccoli.

Il compito della fucina non e produrre piu codice. E trasformare la pressione in
struttura riusabile. Quando un fatto basta, imparare il fatto. Quando serve una
procedura, insegnare la procedura. Quando il linguaggio non puo esprimerla,
allargare una volta il kernel. Quando il kernel sa agire, lasciare che proponga
la propria evoluzione, ma fare in modo che test, holdout, sandbox e approvazione
esterna restino il martello che decide.
