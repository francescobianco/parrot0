# TODO — 30 attività per rendere parrot0 un coding agent competitivo

Questa è la coda **attiva**, ordinata per rischio e leva. Deriva dal piano canonico
`docs/plans/parrot0-forge-master-plan.md`, dall'ispezione del codice a gen328 e da
sessioni reali eseguite il 2026-07-12 con `make chat` e
`PARROT0_TOOLS=1 make chat`. Non è una lista di feature decorative: ogni voce
parte da un counterexample, preferisce KB/schema/procedure a nuovo C specifico e
si chiude solo con un oracolo, negativi, holdout e ablation quando pertinenti.

Regola comune: **un decline preciso è verde; un successo non provato o un fatto
inventato è rosso**. Ogni attività deve produrre un contratto focale indirizzabile,
una trace strutturata e l'aggiornamento automatico del capability ledger.

## P0 — verità operativa, feedback e sicurezza

- [ ] **01 — Separare issue, prosa e sorgente prima di diagnosticare codice.** Counterexample reale: `fix this code: int absval(...) ... It should return ...` su C sintatticamente valido risponde falsamente “add a semicolon”. Introdurre segmenti tipizzati con span (`instruction`, `constraint`, `code`, `expected`) e compilare soltanto il sorgente estratto; se l'estrazione è ambigua, restituire `ambiguous_input`, non una diagnosi. **Done:** 20 layout misti EN/IT e input multilinea non producono falsi syntax finding; i corrispondenti snippet realmente rotti restano rilevati dal compilatore.

- [ ] **02 — Fare l'honesty sweep di tutti i checker.** Per ogni `check_*`/diagnostica aggiungere almeno un programma corretto sul quale deve restare silente, un difetto vero, mutazioni di whitespace/commenti e un oracle meccanico (compiler, sanitizer, test o property), evitando che un pattern scanner possa emettere da solo un claim. **Done:** false-positive rate 0 sul corpus correct/clean e ablation di ogni veto riaccende il suo counterexample.

- [ ] **03 — Completare Fast Forge come inner loop direzionale.** Estendere l'embrione `make check TEST=<id>` a tutti i nuovi counterexample di questa lista, con catalogo atomico, owner, tier, timeout, oracle, START/PASS/FAIL streaming e primo contratto rotto; generare evidence artifact content-addressed riusabile dal capability report. **Done:** 20 modifiche consecutive con focal P95 <= 5 s, primo evento <= 500 ms, nessuna riesecuzione gate→report e timeout/infra/flaky distinti da FAIL.

- [x] **04 — Eliminare shell injection e prefix-whitelist dall'executor.** Counterexample reale: `run git status; printf P0_CHAINED` viene accettato ed esegue il secondo comando. Sostituire `popen(command_string)` con tool contract versionati e `posix_spawn`/`execve` su `argv[]`, cwd ed environment espliciti; il planner sceglie il tool tramite fatti `tool_for/2`, il prompt non costruisce shell. **Done:** suite con `; | && $() backtick newline` non esegue mai un secondo processo, mentre i comandi ammessi conservano output equivalente.

- [x] **05 — Imporre il contenimento reale del workspace.** Counterexample reale: in agent mode `read /etc/os-release` legge fuori repository; oggi `safe_pathish()` accetta assoluti e `..`. Aprire la root con `dirfd`, usare `openat`/`fstatat` e `O_NOFOLLOW`, canonicalizzare ogni accesso e rifiutare symlink escape, device, FIFO e socket. **Done:** assoluti, traversal, symlink chain e rename race sono `unsafe_path`; read/list/find validi dentro una fixture continuano a funzionare.

- [x] **06 — Sandboxare compilazione, esecuzione e test di codice non fidato.** Eseguire candidati in process group isolati con rete off, environment allowlist, cwd usa-e-getta, limiti CPU/wall/RSS/fsize/nofile/nproc, timeout+`killpg` e filesystem oracle/test non modificabile. **Done:** fork bomb, loop infinito, allocazione eccessiva, network probe e test tampering terminano con verdict tipizzato senza processi orfani o modifiche esterne.

- [x] **07 — Rendere ogni tool result un'Observation, non testo ottimistico.** Registrare argv, input digest, cwd, exit code, signal, stdout/stderr separati, truncation, durata, side effect e artifact hash; oggi `run_shell()` ignora lo status e non ha timeout. **Done:** un `make` fallito non può essere descritto come successo, output vuoto/fallimento/timeout sono distinti, e `how do you know?` cita l'Observation esatta.

- [ ] **08 — Rendere dispatch e decomposizione transazionali.** La selezione speculativa non deve mutare KB, memoria, filesystem o rieseguire tool: i moduli prima producono `Proposal`, poi il router seleziona e solo il champion commette effetti; rollback completo su sottogoal successivo fallito. **Done:** prompt articolati con secondo passo impossibile lasciano zero effetti parziali, e replay/shadow non duplicano action o fatti.

### Chiuso in gen329 (`src/exec.c`, oracolo `tests/toolexec.sh`, 25/25)

04/05/06/07 cadono insieme perché erano lo stesso strato: parrot0 *parlava* al
sistema operativo in una lingua (una stringa di shell, un path scritto come testo)
invece di consegnargli oggetti tipizzati. Ora `p0_exec` esegue un `argv[]` con
`execvpe` (nessuna shell: `;` è solo un carattere dentro `argv[1]`), ogni path passa
per `openat`/`O_NOFOLLOW` componente per componente a partire da un dirfd sulla root
— quindi assoluti, `..`, catene di symlink e rename race sono `unsafe_path` per
costruzione, e l'fd restituito È l'oggetto validato, non un nome da ririsolvere. Il
figlio è un process group con rlimit e wall clock, ucciso con `killpg`. Ogni atto
torna come `P0Obs` (argv, cwd, exit, signal, stdout/stderr separati, truncation,
durata, digest): un `make` fallito ora *non può* essere raccontato come risultato,
perché la frase per un fallimento è una frase diversa, e `how do you know?` cita
l'Observation. L'autorizzazione è KB (`tool_for/2`, `tool_subcmd/2`): `git status`
sì, `git push` no, e un tool nuovo è un fatto, non una ricompilazione.

Due bug miei, trovati eseguendo e non leggendo: `RLIMIT_NPROC` è per-UID (64 ha
negato le fork all'intera sessione desktop dell'utente) e `unshare(CLONE_NEWUSER)`
qui *riesce* ma il write su `uid_map` è negato da AppArmor, lasciando il figlio
arenato come overflow-uid — il che rendeva `/bin/sh` ineseguibile. Il figlio ora
riporta l'arenamento e muore, il padre riprova una volta *con* rete e lo dichiara:
`net_isolated=0` non è mai una bugia.

## P1 — un vero kernel da coding agent

- [ ] **09 — Allineare `make chat` ai capability realmente disponibili.** Counterexample reale: `make chat` carica il profilo AGI ma non `PARROT0_TOOLS=1`, quindi gli stessi prompt read/list/where verdi in `piagent-bench` murano; inoltre abilita implicitamente il fetch Wikipedia. Offrire target/modi espliciti (`chat`, `chat-agent`, `chat-acquire`), rete off nel runtime safe, banner e self-model derivati dalla policy effettiva. **Done:** la chat agent esegue gli stessi contract dell'API, quella conversazionale dichiara i tool disabilitati e soltanto ACQUIRE abilita rete/quarantena, senza claim discordanti.

- [ ] **10 — Eliminare troncamenti silenziosi e supportare input coding multilinea.** Sostituire i buffer rigidi di turn/task/response con arena o buffer bounded dinamici; multiline paste deve funzionare senza escape terminali, conservando byte e span. **Done:** issue+diff da almeno 64 KiB e sorgente multilinea attraversano CLI/API senza perdita; oltre budget arriva `input_too_large` con limite e hash, mai parsing di un prefisso come task completo.

- [ ] **11 — Rifare l'adapter OpenAI come sessione multi-turn fedele.** Ingerire `system`, tutta la history, messaggi assistant, tool calls/results e contenuti strutturati; isolare un `SessionContext` per request/conversation invece di condividere un Brain globale e rimuovere il limite implicito di 2000 byte. **Done:** contract concorrenti non contaminano stato, un task di tre turni usa davvero gli artefatti precedenti, system/policy non è ignorato e il transcript ha provenance per messaggio.

- [ ] **12 — Introdurre il kernel tipizzato minimo.** Implementare `Event`, `Task`, `Goal`, `Constraint`, `Action`, `Observation`, `Artifact`, `Verdict`, `Gap` e `Decision` con ID stabili, provenance e serializzazione JSONL; gli handler legacy restano adapter/fallback. **Done:** un workflow completo è interrogabile dal raw input al verdict senza inferire stato da frasi renderizzate, e un campo obbligatorio mancante fallisce il contract.

- [ ] **13 — Separare `BrainImage`, `SessionContext` e `TaskContext`.** Spostare base KB/registry/config frozen nell'image, memoria e overlay nella sessione, budget/workspace/action log/candidati nel task; rendere limiti KB osservabili prima di dinamizzarli. **Done:** due sessioni e due task concorrenti non condividono lingua, proof, entità, tool result o candidate artifact; reset e lifecycle sono differentialmente equivalenti al fresh exec.

- [ ] **14 — Fare del piano conoscenza KB-first.** Definire `action_schema` dichiarativi con argomenti, precondizioni, effetti, rischio, costo e tool contract; un planner puro deriva `inspect -> localize -> edit -> verify` e spiega quale precondizione manca, lavorando prima su Observation replayate. **Done:** ablation di uno schema produce il Gap atteso, aggiungerne uno valido non richiede C e holdout con ordine/nomi diversi conserva il piano.

- [ ] **15 — Aggiungere budget, checkpoint/resume e stop condition al planner.** Budgetare passi, tempo, processi, byte e tentativi; persistere event log, hash del workspace e goal frontier per riprendere senza rifare azioni già attestabili. **Done:** kill+resume di un task riparte dall'ultimo checkpoint valido, digest cambiato invalida le osservazioni stale e budget esaurito rende `budget_exhausted`, non un falso risultato.

- [ ] **16 — Implementare `PatchArtifact` multi-file transazionale.** Modellare base tree hash, pre/postimage per file, operazioni create/delete/rename/edit, unified diff, blast radius e rollback; applicare prima in worktree candidato con `git apply --check`, poi atomicamente solo se autorizzato. **Done:** patch multi-file, collisione su preimage e post-check fallito hanno esiti riproducibili; test/oracle non sono modificabili e rollback ripristina byte e status esatti.

- [ ] **17 — Generalizzare il repair loop oltre bubble-sort.** Portare `observe -> diagnose -> propose >=2 -> select min blast radius -> act -> verify -> replan` nel kernel, usando verdict e regole di repair nel KB invece di uno schema per smell. **Done:** almeno tre bug class fredde (semantica, resource lifecycle, cross-file) richiedono un primo candidato rosso e diventano verdi entro N; difetto senza regola produce Gap e nessuna patch.

- [ ] **18 — Costruire un IR comune C/Python con identità stabili.** Normalizzare file/modulo/simbolo/scope/node/reference/call/read/write/branch/loop/return, source hash e span; aggiungere scope/binding, CFG essenziale e def-use come fatti derivati con proof. **Done:** rename/whitespace preservano le identità previste, edit invalida solo la closure, e compilatore/interprete risolve le incertezze senza claim inventati.

- [ ] **19 — Indicizzare repository e localizzare issue per evidenza.** Creare symbol/reference/call/import index, diagnostics, git history e test-to-code map; rankare issue→file→symbol con proof comprendendo lookup falliti e decoy. **Done:** campagna cold multi-repo con top-5 >=80%, top-1 e byte letti pubblicati, nessun hint di file/funzione e false positive misurati su repo puliti.

- [ ] **20 — Estrarre il contratto completo della issue e chiudere il primo VTC.** Convertire expected/observed, repro, constraint, non-goal e acceptance in strutture con source span; eseguire interpret→observe→plan→patch→test→repair su issue intera, non `fix the bug in <directory>`. **Done:** almeno una fixture fredda con decoy raggiunge `VTC=1`, FAIL_TO_PASS e PASS_TO_PASS verdi, trace completa e nessuna alterazione dell'oracle.

- [ ] **21 — Collegare diagnostica, selezione test e replan.** Parsare compiler/pytest/sanitizer in verdict strutturati associati a node/constraint; scegliere prima test mirati dalla test-to-code map, poi full regression, aggiornando ipotesi dopo ogni controesempio. **Done:** un test fallito cambia osservabilmente ranking o piano, evita oscillazioni entro N e la full suite conferma senza regressioni il survivor.

- [ ] **22 — Portare lettura ed explanation dal lessico alla semantica.** Counterexample reale: `explain what this does` rende solo “returns a value; contains a loop”, mentre una paraphrase italiana/inglese mura. Derivare nome funzione, parametri, state change, loop invariant/accumulatore, return e limiti dall'IR, marcando ogni frase come parsed/derived/hypothesized. **Done:** spiegazioni di 30 snippet C/Python sono verificate contro trace/esecuzione o golden strutturali e non descrivono semantica oltre prova.

- [ ] **23 — Sostituire il keyhole linguistico con intenti dichiarativi e proposal routing.** Counterexample reali: `explain what this does` passa ma `can you explain what this function computes` e `spiegami in dettaglio` no; `locate the definition...` viene rubato da `definition`; `.conf` viene interpretato come `.c` per substring. Estendere `intent_cue`/classi lessicali nel KB, usare token/extension match esatto e selezionare su registro+struttura+evidence confrontando più Proposal, non first-match. **Done:** 10x paraphrase EN/IT per intent raggiungono la stessa faculty, negativi prose/code e suffissi sovrapposti non collidono, cue nuova è insegnabile senza ricompilare.

## P2 — fedeltà KB-first e crescita che compone

- [ ] **24 — Invalidare proof e cache quando cambia il support set.** Counterexample reale: dopo `no, socrates is not a man`, la query dice `No` ma `how do you know?` può rispondere ancora `Because man(socrates)`; un proof resta anche dopo forget. Conservare proof object con dependency IDs/versioni, non una stringa globale. **Done:** retract/correction/reload/patch invalida transitivamente ogni proof dipendente e `why` cita soltanto supporti attivi del verdict corrente.

- [ ] **25 — Propagare conflitti, negazioni e incompletezza nelle regole.** Oggi `man(socrates)` + `not(man(socrates))` può risultare conflicted sul fatto ma consentire comunque `mortal(socrates) :- man(socrates)`; depth/goal/binding limit collassano inoltre nello stesso `0` di “non provato”, rendendo NAF potenzialmente falsa-onesta. Definire `proved/refuted/both/unknown/floundered/budget_exhausted` e farli rispettare da solver, match, NAF, planner e renderer. **Done:** conflitti diretti/derivati e ricerche oltre budget non producono conclusioni unilaterali; proof mostra supporti o primo limite e una policy esplicita è necessaria per agire.

- [ ] **26 — Rendere layer, provenance e ablation non distruttivi.** What-if/robustezza non deve ritirare un fatto BASE e reinserirlo come SESSION né farlo finire in `/save`; usare overlay/tombstone scoped con source ID immutabile. **Done:** hash e origin della base sono identici prima/dopo abduction, what-if, replay e rollback; ablation spegne il candidato causalmente senza riclassificare conoscenza.

- [ ] **27 — Unificare memoria conversazionale e persistenza nel KB.** Nome, animali, preferenze, goal, entità salienti e topic non devono vivere in array C che `/save` non sa ricostruire; modellarli come fatti session-scoped con schema/versione e privacy policy. **Done:** save+restart recupera esattamente le memorie autorizzate e non PID/utterance/reflective facts; forget e correzione persistono e `what do you remember?` enumera tutte e sole le memorie attive.

- [ ] **28 — Mettere apprendimento/acquisizione in quarantena con provenance verificabile.** Ogni observation/fatto/procedura deve avere source digest/span, extractor, validator, supporti, conflitti e activation state; niente fetch o autolearn direttamente in KB_BASE/curata. Un `trainable_schema` dichiara consumer, arity, validator, holdout e promotion policy. **Done:** una fonte locale/remota è citabile, un predicato senza consumer è respinto, variant/negative/holdout+ablation precedono promotion atomica e rollback del canary è dimostrato.

- [ ] **29 — Generalizzare learn→build con schema induction e CEGIS KB-first.** Gen328 riconosce causalmente una sola shape (`nested_loop_compare_swap`), ma insertion sort e artifact ordinari restano fuori envelope. Estrarre passi/constraint con provenance, generare sketch e property, enumerare/riscrivere buchi tipizzati e usare controesempi per raffinare; zero `algo_shape` per nome. **Done:** almeno selection sort, binary search con not-found e una trasformazione testuale da pagine fresche passano holdout e ablation usando lo stesso induttore; fonte corretta invalida e rigenera il derivato.

## P3 — prova di competitività nel campo

- [ ] **30 — Pubblicare envelope e benchmark blind di Verified Task Completion.** Sostituire lo SWE harness che lascia `engaged=0` costante e passa solo il titolo con task che consegnano issue e repo completi; creare train/validation/hidden-holdout per repo/bug class, clean negatives e feature-add, poi confrontare parrot0 e un coding agent LLM sugli stessi budget/oracle. **Done:** capability ledger e scorecard VTC/tempo/costo/diff/regressioni/safety sono generati da evidence attestata e replayabile; parrot0 raggiunge parità nel perimetro dichiarato senza LLM runtime, hint, fabricated success o test tampering.
