# PARROT0 FORGE - piano maestro verso un coding agent verificabile

> **Stato:** piano canonico, 2026-07-11, baseline HEAD `93511ba` (gen312 nei
> commit; `brain_version()` e ancora fermo a gen300).
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

### 1.7 Una generazione, un contratto

- Ogni generazione chiude un solo contratto rosso.
- Deve essere piccola, reversibile e bisecabile.
- Deve guadagnare test mirati, held-out e negativi.
- La prova circa 10x piu complessa e il ratchet EN/IT restano obbligatori quando
  il dominio e linguistico.
- Il piano e a dipendenze, ma la scelta dentro una frontiera resta pull-based.

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

Prima di accelerare la crescita, la fucina deve rendere impossibile promuovere
su una baseline rossa.

### 3.4 Debito di verita e governance

- `brain_version()` riporta `gen300-class-conjunction`, mentre HEAD dichiara
  gen312.
- Molti piani includono handoff storici e stati superati senza un indice
  machine-readable.
- `make test` non include `code-bench`, `sortlearn-bench`, `game-bench` e SWE.
- Un `PASS` in degrade mode misura esecuzione dell'harness, non capacita.
- LLMSCORE ha gia accettato almeno un fatto falso: un LLM judge e discovery,
  non verita.

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

1. `Brain` conserva identita, registry e servizi permanenti;
2. ogni lavoro riceve un `TaskContext` con arena dinamica, budget e event log;
3. memoria conversazionale, workspace facts e candidate artifacts hanno scope e
   lifecycle distinti;
4. i moduli nuovi dipendono da interfacce esplicite, non da ogni helper statico;
5. un frammento passa a translation unit separata solo quando un contratto/test
   rende chiaro il suo confine;
6. il vecchio path resta fallback finche il confronto differenziale e verde.

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
make forge CAMPAIGN=<id> MODE=knowledge|procedure|engine|agent BUDGET=<n>
make forge-replay CAMPAIGN=<id>
make forge-promote RUN=<run-id>
make forge-rollback CAPABILITY=<id>
make capability-report
```

`autolearn` resta disponibile come esperimento storico finche `forge knowledge`
non lo sostituisce. Il default sicuro di `autolearn` deve diventare
`MULTIPLY=0` nel frattempo.

### 10.1 Seal: sigillare l'esperimento

Prima di ogni run:

- base commit e tree hash;
- hash binario, KB e tool;
- compiler/runtime version;
- trust mode e policy;
- proposer, judge e seed, se esterni;
- budget e timeout;
- capability baseline;
- challenge pack gia separato.

Se la baseline obbligatoria e rossa, il run viene classificato `baseline-broken`
e non puo promuovere.

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

- massimo tentativi esplicito;
- ogni tentativo ha parent id;
- nessuna scrittura ufficiale;
- ogni azione e registrata;
- il fallimento e un risultato utile e resta nel ledger.

### 10.6 Temper: i gate

| Gate | Domanda | Condizione |
|---|---|---|
| **G0 Integrita** | il candidato e valido? | parse, schema, arita, consumer, fonte |
| **G1 Red->green** | chiude il contratto? | minimo caso passa |
| **G2 Ablation** | e causalmente responsabile? | senza torna rosso |
| **G3 Negative** | ruba casi non suoi? | controlli negativi restano invariati |
| **G4 Transfer** | generalizza? | mutation, paraphrase, fresh names, 10x, EN/IT |
| **G5 Holdout** | funziona fuori train? | set nascosto, diverso file/repo/forma |
| **G6 Oracle** | il risultato e reale? | compiler/test/property/differential/SWE |
| **G7 Regression** | rompe il ratchet? | matrice completa pertinente verde |
| **G8 Determinism** | e riproducibile? | almeno 3 replay equivalenti |
| **G9 Safety** | rispetta confini? | zero escape, tampering, rete, budget violation |

Un sibling prodotto dal multiplier e un candidato indipendente: non eredita il
successo del seed. Deve almeno passare G0-G3 e un probe proprio; per entrare in
base deve passare l'intera policy della sua classe.

### 10.7 Quench: promotion atomica

Stati:

```text
observed -> candidate -> quarantined -> verified -> canary
         -> promoted | rejected | rolled_back
```

Una promotion contiene in un'unica generazione:

- candidato;
- test guadagnati;
- manifest;
- scorecard;
- provenance;
- decision record;
- activation change;
- rollback testato;
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

`.forge/` e effimera e gitignored. Campaign, ledger e capability manifest sono
versionati. Evidence pesante puo essere content-addressed fuori git, ma il suo
hash resta nel manifest.

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
  "budget": {"steps": 8, "seconds": 30},
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
letto dal consumer fallisce G0. Un validator deve controllare semantica locale,
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

- regressioni ratcheted: **0** per promotion;
- successi dichiarati senza oracle disponibile: **0**;
- fatti official senza provenance: **0**;
- test tampering: **0**;
- sandbox escape/rete non autorizzata/secret read: **0**;
- replay non deterministico senza causa dichiarata: **0**.

### 12.4 Metrica di accelerazione

```text
leverage = heldout_edges_closed
           / (new_engine_LOC + weighted_new_schemas + maintenance_cost)
```

Non serve ottimizzare letteralmente una formula. Serve impedire che 50 nuovi
fatti vengano confusi con una nuova facolta. Le metriche da osservare sono:

- task holdout chiusi per generazione;
- gap chiusi senza C;
- riuso di una procedura in domini diversi;
- costo marginale di aggiungere un linguaggio/repo;
- regressioni evitate dalla promotion policy;
- tempo tra failed lesson ed engine pull verificato.

### 12.5 LLMSCORE, basic-chat e mimic

- sono sonde di breadth, naturalezza e discovery;
- alimentano la miniera della fucina;
- non promuovono fatti o codice;
- non sono la north star del coding agent;
- un aumento ottenuto con phrasebook vale zero leverage.

---

## 13. Dependency DAG

```text
W0 truth ledger + baseline verde + benchmark semantics
 |\
 | +--> secure executor + transactional workspace
 |
 +----> Forge knowledge: quarantine + holdout + promotion
          |
          +--> structured Gap + provenance + procedure proof
                  |
                  +--> typed Agent kernel + action schemas
                           |
                           +--> inspect/edit/verify/replan loop
                                  |              |
                                  |              +--> multi-file + git
                                  |
                                  +--> common code IR + repository index
                                                 |
                                                 +--> issue -> region
                                                        |
                                                        +--> cold SWE repair

procedure learning + solver + code IR
                 |
                 +--> markdown/spec -> constraints/schema
                                  |
                                  +--> CEGIS synthesis + feature tasks
                                                 |
                                                 +--> Forge engine
                                                        |
                                                        +--> dogfood parrot0
```

Le frecce sono prerequisiti reali. Dentro ogni nodo, il prossimo incremento e
scelto dal primo gap con migliore combinazione di frequenza, oracle, riuso e
rischio.

---

## 14. Roadmap a ondate

Le ondate non assegnano date o un numero rigido di generazioni. Ogni riga resta
una generazione distinta e viene saltata se la pressione reale indica un
prerequisito diverso.

### W0 - Verita prima della velocita

**Obiettivo:** nessuna crescita su baseline falsa.

Deliverable:

1. riparare le regressioni correnti `agent_search`, code `defines/locate` e
   sortlearn, senza inseguire gli output specifici;
2. manifest machine-readable dei benchmark con semantica `gate`, `discovery`,
   `degrade`, `external`;
3. `make capability-report` con stato e commit;
4. `brain_version` derivato dal build/commit o verificato automaticamente;
5. suite `make gate` che include tutti i ratchet obbligatori per area;
6. test-engine persistente/differenziale per feedback rapido;
7. hash degli oracle e dei fixture.

**Gate di uscita:** tre replay puliti della matrice core/coding/agent; zero
regressioni; report identico.

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

### W3 - Agent kernel MVP

**Obiettivo:** il primo workflow completo su un task single-repo.

Deliverable:

1. Event/Task/Goal/Action/Observation/Artifact/Verdict/Gap;
2. event log JSONL;
3. action schemas nel KB;
4. planner che deriva `inspect -> edit -> verify`;
5. repair loop bounded su candidato volutamente errato;
6. renderer finale con diff, test e limiti;
7. adapter multi-turn: system, history, tool results, input multiline;
8. checkpoint/resume del task.

**Gate di uscita:** su una fixture mai vista il sistema localizza, modifica,
fallisce una prima verifica, ripara e passa; ablation di un'action schema rende
il gap esplicito.

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
8. test-to-code map.

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
6. modularizzazione reale delle translation unit quando tirata da test/build;
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

Questo e l'ordine concreto iniziale, rivedibile solo se un gate mostra un
prerequisito diverso.

| # | Contratto singolo | Gate |
|---:|---|---|
| 1 | ripristinare `agent_search` EN/IT senza rompere aritmetica composta | `make test` verde |
| 2 | chiudere i due routing regressi del code-bench | 25/25 code gate |
| 3 | ripristinare l'isolamento forget di sortlearn | 9/9 sortlearn |
| 4 | manifest dei benchmark e semantica gate/degrade | report verificabile |
| 5 | capability ledger generato + version drift gate | stato derivato dai test |
| 6 | autolearn multiplier 0 + schema registry stretto | facts morti respinti |
| 7 | ledger completo e candidate id/provenance | replay di ogni decisione |
| 8 | overlay quarantine + promotion/rollback atomici | nessuna write diretta |
| 9 | challenge pack + ablation + negative | cattivo seed respinto |
| 10 | holdout nascosto + metamorphic EN/IT | prima capability TRANSFER |
| 11 | executor argv-based con timeout/process group | shell injection impossibile |
| 12 | Gap tipizzato emesso dal repair workflow | niente decline opaco |

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

### C1 - Routing senza collisioni

**Minerale:** `agent_search` viene risolto da aritmetica; due code request vengono
risolte da knowledge.

**Scopo:** introdurre ownership/proposal evidence senza hardcodare i prompt.

**Holdout:** stessi intenti con numeri, nomi, lingua e ordine differenti; negativi
di aritmetica pura e query knowledge pure.

**Promotion:** tutti i vecchi gate verdi, ablation positiva, trace mostra la
ragione della selezione.

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
```

Priorita indicativa:

```text
priority = frequency * severity * oracle_quality * reuse
           / (risk * effort)
```

Non e una funzione da ottimizzare ciecamente. Serve a rendere esplicita la
decisione e a rispettare la pivot duty:

- se tre primitive non avanzano un task reale, tornare domain-pull;
- se tre task richiedono lo stesso workaround, promuovere il gap a engine pull;
- se il C cresce piu velocemente dei predicati/regole riusabili, fermarsi;
- se un benchmark puo essere vinto con una scorciatoia, rafforzare prima l'oracolo;
- se i test rallentano la selezione, ottimizzare il feedback prima delle feature.

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
- Non rendere il grand plan una coda rigida: i gate devono poter cambiare il
  prossimo pull.
- Non ottimizzare LLMSCORE a scapito di verita, agency e coding VTC.
- Non auto-committare o auto-promuovere dal soggetto sperimentale.

---

## 21. Frase guida

> **Scaldare un fallimento reale, batterlo in piu varianti, temprarlo su casi che
> non ha visto, e promuoverlo solo quando l'oracolo dimostra che e diventato una
> capacita.**

Il compito della fucina non e produrre piu codice. E trasformare la pressione in
struttura riusabile. Quando un fatto basta, imparare il fatto. Quando serve una
procedura, insegnare la procedura. Quando il linguaggio non puo esprimerla,
allargare una volta il kernel. Quando il kernel sa agire, lasciare che proponga
la propria evoluzione, ma fare in modo che test, holdout, sandbox e approvazione
esterna restino il martello che decide.
