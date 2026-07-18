# coding-agent TODO — la sequenza operativa

> **Scopo:** la coda ordinata di upgrade per portare parrot0 a operare come coding
> agent. Deriva da `coding-agent-evolution.md` (le facoltà e l'architettura),
> da `TODO.md` (i 30 task con i loro counterexample) e dall'audit KB-first di §0.
> **Direttiva (F., 2026-07-17):** qui NON c'è processo — niente gate, holdout,
> promotion, fucina. È la sequenza delle cose da **mettere dentro**, una dopo
> l'altra: upgrade funzionali, abilità interne, espansione della conoscenza.
> Ogni voce dice *cosa si aggiunge* e *cosa parrot0 sa fare dopo*.
>
> **Ciò che resta (è design, non processo):** KB-first (il vocabolario va in KB, il
> C è meccanica), niente impostori (un artefatto non verificato è un candidato),
> declino onesto con il gap nominato (`unknown`/`ambiguous`/`budget_exhausted` sono
> stati, non fallimenti da nascondere).
>
> **Come si legge:** sei ondate A→F, ogni task ha checkbox, cosa aggiunge, il
> risultato operativo, le dipendenze e la nota KB-first. Le ondate si affrontano in
> ordine; dentro un'ondata l'ordine è quello scritto. Quando un task tocca
> un'obsolescenza di §0.2, la sua nota KB-first la cita.

---

## 0. Audit KB-first gen335 — su cosa poggia questo piano

Verifica di conformità del codice esistente prima di costruirci sopra (regola:
*l'utente potrebbe insegnare la forma nuova a runtime, senza edit di C?* Se no, è
un'obsolescenza).

### 0.1 Già conforme (verificato, ci si può poggiare)

| Componente | Prova |
|---|---|
| Executor `src/exec.c` (gen329) | meccanica pura; l'autorizzazione è KB (`tool_for/2`, `tool_subcmd/2`): un tool nuovo è un fatto |
| Percezione `input_segment` + `kb_hypothesis_best` + `kb/core/input.p0` (gen332) | ruoli/registri/delimitatori/faculty sono fatti; assert/retract runtime cambiano la segmentazione |
| Guardia verbi di insegnamento `00-lex.c` (gen271) | primo sito migrato: `kb_cue_match` + fatti in `kb/core/intents.p0` |
| Teaching generico `learnable/3` (gen214) | un handler solo, guidato da registry KB |
| Proiezione `policy/2` al boot (gen331) | banner/self-model/declini leggono fatti KB, non `getenv` |

### 0.2 Le obsolescenze (da migrare, ondata E)

| # | Dove | Evidenza (gen335) | Perché è un problema |
|---|---|---|---|
| **O1** | `src/brain/60-agent-tools.c` | 48 siti `cue(` con forme EN/IT cablate: `list`+`file`, `find`+`named`/`called`, `call`/`caller`/`uses of`, `learn`+`steps`/`passi`, `write`/`scrivi`+`function`, `start`/`begin`/`parti`/`inizia`; più `strstr("until"/"finch"/"fino a")`, `strstr("even"/"pari"/"odd"/"dispari")` | è il **vocabolario dei tool dell'agente stesso**: non insegnabile a runtime, non estendibile senza ricompilare. Migrazione già iniziata (13 `kb_cue_match`): va chiusa |
| **O2** | `src/brain/80-code.c` | 31 siti `cue(` (`run`/`execute`/`esegui`…), `strstr("in C"/"in Python"/"in programming")` | i verbi del modulo codice — la faculty centrale del coding agent — sono un frasario |
| **O3** | `src/brain/65-induce-verify-shell.c` | phrasebook bilingue `strstr`: `"continue from"/"continua da"`, `"what comes after"/"cosa viene dopo"`, `"apply it to"/"applicala a"`, `"what is the rule"/"qual è la regola"`, `"using"/"usando"`, `"fit"/"consistent"/"coerente"/"rispetta"` | un intero parser di intenti cablato in due lingue: la forma esatta bandita dal manifesto |
| **O4** | `src/brain/85-translate-synth-world.c` | `strstr(low, "without ")` + `strstr(low, "senza ")` (linea ~871), `"say X in Y"`, `"is assumed"` | la **stessa coppia bilingue** che gen332 ha bandito da `code.c`: la malattia è trasversale |
| **O5** | `src/brain/10-memory-knowledge.c` (187 cue), `40-meta-reflection.c` (137), `25-wordmath-reasoning.c` (148), `70-social-pragma.c` (101), `30-generation-reading.c` (97), `20-math.c` (64), `50-self-research-loop.c` (35) | ~790 siti `cue(` complessivi; più preposizioni di slot-binding cablate: `"about "/" su "/" di "/" per "/" for "` (10-memory), che PRINCIPLES dichiara esplicitamente conoscenza | il grosso del vocabolario conversazionale è ancora C; il contatore Track 5 (catene) deve arrivare a zero |
| **O6** | `src/code.c` | `strstr("def ")`, `" is None"`, `"if ("`, `"- i;"` | token di **codice**, non lingua naturale: borderline lecito come meccanica, ma i marker di linguaggio dovrebbero diventare `register_evidence/2` (bassa priorità) |
| **O7** | `src/brain/30-generation-reading.c` | `"passage:"`, `"query:"`, `"choice 1:"`, `"premise:"`… | formati-record di benchmark (SuperGLUE): candidati `register_evidence/2`, non urgenza |

**Totale:** ~873 siti `cue(` + una ventina di frasari `strstr` bilingui. La
migrazione è iniziata (13 file chiamano già `kb_cue_match`, 95 siti) ma non chiusa.
La macchina per migrare esiste (Track 5, gen257–263: detector, piano derivato,
patch, verifica differenziale) — l'ondata E la usa, non inventa altro.

---

## Ondata A — il corpo: il loop agentico opera (T01–T07)

L'obiettivo dell'ondata: chiudere il primo ciclo
`goal → piano → azione → osservazione → verifica → riparazione` su macchina reale,
con gli organi che già esistono (executor gen329, tool KB, percezione gen332,
oracoli compile/run).

- [x] **T01 — Kernel tipizzato minimo: la spina dorsale.**
  **Cosa:** tipi `Task/Goal/Constraint/Action/Observation/Artifact/Verdict/Gap/
  Decision` (più `Segment` dalla percezione) con id stabili, parent, provenance,
  sequenza logica; store in-memory; serializzazione JSONL per audit; proiezione in
  fatti KB (`rec(Id,Kind,Parent,Source,State)`). Modulo senza dipendenza da Brain,
  come `exec.c`. Gli handler legacy restano adapter/fallback.
  **Dopo parrot0 sa:** rappresentare un lavoro di coding lungo più turni e dire
  "dove sono arrivato" dai record, non dal testo renderizzato.
  **Dipende da:** niente (è la radice). **Assorbe:** TODO 12.
  **KB-first:** meccanica pura (tipi, id, serializzazione); il vocabolario degli
  stati segue il precedente `p0_verdict_name` di exec.c. Nessuna forma naturale.
  **Stato:** chiuso (gen336) — `src/agent.h`/`src/agent.c`: 11 record-kind, id
  densi O(1), parent/source/seq, unica mutazione = cambio di stato, `p0a_select`/
  `p0a_last`, JSONL, proiezione `rec/5` + `rec_name/2`. Lo smoke esterno è diventato
  ratchet in-tree: `tests/agentkernel.sh` (in `make test`) compila il modulo reale e
  asserisce le proprietà su cui poggiano T03/T06 — densità degli id verificata per
  ogni id (è la premessa che rende legale l'aritmetica di `p0a_get`), append-only
  (un cambio di stato non tocca nient'altro), `p0a_select` che ritorna il totale
  VERO oltre il cap, il trail JSONL riletto dal json.c reale (nome con virgoletta +
  backslash + newline round-trip), la proiezione KB p0-legale (stato vuoto → `open`,
  virgoletta nel nome → non spezza l'atomo). 33 assert verdi, build senza warning.
  Il consumo da planner (T03) e loop (T06) è lavoro di quei task, non di questo.

- [ ] **T02 — Input multilinea e buffer dinamici.**
  **Cosa:** sostituire i buffer rigidi di turn/task/response con arena o buffer
  bounded dinamici; paste multilinea di issue+diff senza escape terminali; oltre
  budget, `input_too_large` con limite e hash — mai parsing di un prefisso come se
  fosse il task completo.
  **Dopo parrot0 sa:** ricevere una issue vera incollata in chat-agent e lavorarci.
  **Dipende da:** T01 (gli span confluiscono nel kernel). **Assorbe:** TODO 10.
  **KB-first:** meccanica; nessuna forma coinvolta.

- [ ] **T03 — Il piano è conoscenza: action schema del dominio `code_task`.**
  **Cosa:** `action_schema` dichiarativi in KB (argomenti, precondizioni, effetti,
  rischio, costo, tool contract) per `inspect → localize → edit → verify`; il
  planner puro deriva il piano dal goal e sa dire **quale precondizione manca**.
  Estendere il precedente `kb/experts/codebase/actions.p0` (gen258) al dominio
  coding, con il chainer generico — mai una pipeline cablata in C.
  **Dopo parrot0 sa:** dato "sistema X nel repo", produrre da sé la sequenza
  leggi→localizza→patch→compila→testa, e declinare nominando l'anello mancante.
  **Dipende da:** T01. **Assorbe:** TODO 14.
  **KB-first:** è il cuore KB-first del piano: aggiungere un'azione = un fatto;
  ablation di uno schema produce il Gap atteso.

- [ ] **T04 — Budget, stop condition e checkpoint.**
  **Cosa:** budget su passi/tempo/processi/byte/tentativi nel TaskContext;
  `budget_exhausted` tipizzato (mai `false`); event log + hash workspace + goal
  frontier persistiti per riprendere un task ucciso senza rifare azioni già
  attestate (digest cambiato → osservazioni stale invalidate).
  **Dopo parrot0 sa:** lavorare a un task lungo, fermarsi pulito, riprendere.
  **Dipende da:** T01, T03. **Assorbe:** TODO 15.
  **KB-first:** meccanica; le policy di budget come fatti dove diventano scelte.

- [ ] **T05 — `PatchArtifact`: edit transazionale multi-file.**
  **Cosa:** base tree hash, pre/postimage per file, operazioni create/delete/
  rename/edit, unified diff, rollback diff; applicazione prima in worktree
  candidato con `git apply --check` (nuovo `tool_subcmd` KB), poi atomica al
  workspace solo se autorizzata; rollback automatico se il post-check fallisce.
  **Dopo parrot0 sa:** modificare davvero il repo (non solo copie `.p0fix`), multi-
  file, con undo garantito. È la facoltà M che diventa reale.
  **Dipende da:** T01, executor (fatto gen329). **Assorbe:** TODO 16, X7.
  **KB-first:** le operazioni sono dati; l'autorizzazione passa da `tool_for/2`.

- [ ] **T06 — Repair loop bounded nel kernel.**
  **Cosa:** il ciclo `observe → diagnose → propose ≥2 → select (min blast radius)
  → act → verify → replan` come cammino sui record del kernel, entro N tentativi;
  le regole di repair in KB (smell e trasformazioni come fatti/regole), non uno
  schema C per smell; nessuna regola → `Gap`, nessuna patch inventata.
  **Dopo parrot0 sa:** prendere un candidato rosso, capire perché, ripararlo e
  riverificare — il **coding loop** dichiarabile.
  **Dipende da:** T03, T05. **Assorbe:** TODO 17, B4.
  **KB-first:** i diagnostici arrivano dalle span tipizzate (gen332); le regole di
  repair sono conoscenza interrogabile.

- [ ] **T07 — Sessione multi-turno fedele.**
  **Cosa:** adapter OpenAI che ingerisce `system`, tutta la history, messaggi
  assistant, tool calls/results e contenuti strutturati; `SessionContext` per
  conversazione invece di un Brain globale condiviso; via il troncamento a 2000
  byte; transcript con provenance per messaggio.
  **Dopo parrot0 sa:** sostenere un task di tre+ turni usando davvero gli artefatti
  dei turni precedenti, via API come in chat.
  **Dipende da:** T01, T02. **Assorbe:** TODO 11.
  **KB-first:** meccanica di sessione; nessuna forma.

## Ondata B — la comprensione del task (T08–T10)

- [ ] **T08 — Il contratto della issue diventa Goal/Constraint.**
  **Cosa:** dalle span `expected/observed/repro/constraint/non_goal` (segment_role,
  gen332) alle strutture del kernel con source span: la issue intera entra come
  `Task`+`Goal`+`Constraint`, non più `fix the bug in <directory>`.
  **Dopo parrot0 sa:** dire cosa deve valere alla fine (acceptance), cosa è fuori
  scope, e rileggere il repro come input di verifica.
  **Dipende da:** T01. **Assorbe:** TODO 20 (prima metà).
  **KB-first:** i ruoli sono già fatti; insegnare un ruolo nuovo (es. `acceptance`)
  è un assert, non C.

- [ ] **T09 — Diagnostica → Verdict strutturato e selezione dei test.**
  **Cosa:** parsare le span `cc`/`pytest`/`sanitizer` in verdict associati a
  nodo/constraint (consumer `verdict_builder` già dichiarato in `faculty_for/2`);
  scegliere prima i test mirati (test-to-code map), poi la regressione piena;
  ogni controesempio aggiorna il piano.
  **Dopo parrot0 sa:** un test fallito cambia osservabilmente ranking o piano; un
  errore del compilatore punta al nodo, non al buio.
  **Dipende da:** T06, T08. **Assorbe:** TODO 21.
  **KB-first:** nuovi formati di diagnostica entrano come `register_evidence/2` +
  parser del consumer; il C resta muto sui formati.

- [ ] **T10 — Spiegazione semantica del codice dall'IR.**
  **Cosa:** `explain` deriva nome, parametri, state change, loop invariant/
  accumulatore, return e limiti dai fatti IR; ogni frase marcata `parsed/derived/
  hypothesized` — la stessa legge di provenance delle span.
  **Dopo parrot0 sa:** spiegare cosa fa una funzione senza inventare oltre prova,
  e dire cosa NON sa della funzione.
  **Dipende da:** T11. **Assorbe:** TODO 22.
  **KB-first:** la verbalizzazione esce da `response_template`, mai `printf`.

## Ondata C — gli occhi sul repository (T11–T12)

- [ ] **T11 — IR comune C/Python con identità stabili.**
  **Cosa:** vocabolario `file/module/symbol/scope/node/definition/reference/call/
  read/write/branch/loop/return/exception/type` come fatti con span, source hash,
  stable node id, confidence; scope/binding, CFG essenziale, def-use come fatti
  derivati con proof; le differenze di linguaggio come delta (`like(L, abstract)`).
  **Dopo parrot0 sa:** ragionare sul repo, non sul testo; rename/whitespace
  preservano le identità; un edit invalida solo la sua closure.
  **Dipende da:** T01. **Assorbe:** TODO 18, X3–X5.
  **KB-first:** l'IR nasce dalle span tipizzate; gli analizzatori parlano IR;
  marker di linguaggio nuovi = fatti (cura anche O6).

- [ ] **T12 — Indici di repository e localizzazione issue→regione.**
  **Cosa:** symbol/reference/call/import index, diagnostics index, git history,
  test-to-code map; ranking issue→file→symbol con proof (entità del contratto T08
  collegate a simboli/stringhe/test), valutato con decoy, aggiornato dai fallimenti.
  **Dopo parrot0 sa:** ricevere solo la issue e trovare da solo la regione —
  la frontiera F4 affrontata per evidenza, con false positive misurati.
  **Dipende da:** T11, T08. **Assorbe:** TODO 19, X6.
  **KB-first:** nessuna keyword hardcoded nella pipeline; i collegamenti lessicali
  sono fatti KB interrogabili.

## Ondata D — generazione e apprendimento (T13–T15)

- [ ] **T13 — Generazione CEGIS bounded.**
  **Cosa:** spec → constraint/esempi → sketch strutturale → buchi tipizzati →
  enumerazione/riscrittura guidata da KB → candidato → oracolo compile/test/
  property → controesempio → raffinamento bounded. Fonti: procedure apprese,
  analogia strutturale col repo, regole di trasformazione; mai testo libero non
  parseable spacciato come soluzione.
  **Dopo parrot0 sa:** scrivere codice nuovo sotto oracolo, non solo riparare.
  **Dipende da:** T06, T11. **Assorbe:** TODO 29 (prima metà).
  **KB-first:** sketch/buchi/regole sono conoscenza; il motore enumera e verifica.

- [ ] **T14 — Schema induction: markdown → schema eseguibile.**
  **Cosa:** dai passi numerati/pseudocodice di una pagina mai vista a `algo_shape`
  indotto con provenance e source span; esempi e proprietà generati dalla procedura;
  stesso induttore per selection sort, binary search (con not-found) e una
  trasformazione testuale; correzione della fonte → invalidazione del derivato.
  **Dopo parrot0 sa:** la catena learn→build **causale**: impara una procedura
  nuova e la usa per produrre codice verificato. La facoltà L completa.
  **Dipende da:** T13. **Assorbe:** TODO 29 (seconda metà), il ponte di `learn-and-build`.
  **KB-first:** lo schema indotto è un fatto con provenance; zero `algo_shape`
  scritti a mano per nome.

- [ ] **T15 — Needhelp tipizzato.**
  **Cosa:** un `Gap` irriducibile diventa `needhelp.p0` nella root del progetto
  (goal, fatto mancante, contesto — fatti `.p0`); il check periodico è un'azione
  del piano con precondizioni; la risposta dell'aiutante si carica come conoscenza
  nativa e il piano riparte.
  **Dopo parrot0 sa:** chiedere aiuto in forma strutturata invece di fermarsi —
  il muro diventa una richiesta formale e il piano resta vivo.
  **Dipende da:** T03, T06. **Assorbe:** il protocollo needhelp (Track 3).
  **KB-first:** il protocollo stesso è conoscenza (`action_schema`), non un ramo C.

## Ondata E — obsolescenze KB-first: la migrazione (T16–T20)

Ondata trasversale: chiude §0.2 usando la macchina di Track 5 (detector → piano
derivato → patch → giudice differenziale byte-identico + fixture estraneo come
anti-impostor). T16 è indipendente e può essere tirata in qualsiasi momento —
consigliata presto: è il vocabolario dell'agente stesso.

- [x] **T16 — Vocabolario agent-tools → KB (O1).** *(catene cue chiuse gen337;
  residuo strstr sotto)*
  **Cosa:** le 48 catene `cue(` di `60-agent-tools.c` e gli `strstr` di controllo
  (`until/finch/fino a`, `even/pari/odd/dispari`, ` in `) diventano `intent_cue/2`
  + classi lessicali in KB, matchati da `kb_cue_match`; i verbi dei tool diventano
  `learnable/3` dove sensato.
  **Dopo parrot0 sa:** imparare un verbo nuovo per un tool ("cerca" per find) a
  runtime, senza ricompilare — e l'agente smette di violare la propria disciplina.
  **Dipende da:** niente di bloccante. **KB-first:** è il task KB-first per
  definizione; runtime-growth test obbligatorio (assert/retract cambia il riconoscimento).
  **Stato:** chiuso per le catene `cue(` (gen337) — e chiuso **da parrot0 stesso**,
  guidato a prompt in una sessione outer-circle (il protocollo è
  `docs/plans/act-as-subagent.md`): il piano derivato kbfirst_migration
  (scan → vocab → emit → patch) eseguito su `src/brain/60-agent-tools.c` ha
  migrato tutte le 25 catene reali (90 call) in `kb_cue_match` + 90 fatti
  `intent_cue(60_agent_tools_chainNN, …)` ora in `kb/core/intents.p0`; parrot0 si
  è fermato onestamente al passo verify (il file è frammento di TU: giudica la
  suite del progetto). Verifica: build 0 warning, fail-set di `make test`
  byte-identico alla baseline (73 casi, ereditati da gen334, nessuna regressione),
  ratchet `cuechains` 251→**226** (MAX abbassato; ripagato anche il debito
  246→251 di gen334-336). Runtime-growth: `tests/cases/toolvocab_growth.chat`
  (insegnare «letter tally» fa scattare il riconoscitore migrato senza rebuild;
  righe `learnable/3` per chain48 e chain1286, EN+IT). **Residuo aperto:** gli
  `strstr` di controllo (`until/finch/fino a`, `even/pari/odd/dispari`, ` in `)
  non sono catene `cue(` e la macchina Track 5 non li vede: restano da migrare a
  classi lessicali KB in un prossimo round (stessa tecnica di
  `imperative_opener/1`, gen337).

- [ ] **T17 — Vocabolario code → KB (O2, O6).**
  **Cosa:** `80-code.c` (run/execute/esegui, "in C"/"in Python") in `intent_cue`;
  i marker di linguaggio di `code.c` in `register_evidence/2`.
  **Dopo parrot0 sa:** insegnare sinonimi dei comandi codice e riconoscere nuovi
  linguaggi per fatti.
  **Dipende da:** T16 (stessa tecnica). **KB-first:** come T16.

- [ ] **T18 — Phrasebook residui → KB (O3, O4, O5).**
  **Cosa:** `65-induce` (phrasebook bilingue completo), `85` ("without/senza" e
  soci), poi i grandi residui conversazionali (10-memory, 40-meta, 25-wordmath,
  70-social, 30-generation, 20-math, 50-self-research); le preposizioni di
  slot-binding (`about/su/di/per/for`) in classi lessicali KB.
  **Dopo parrot0 sa:** il contatore catene `cue(` scende a zero; tutto il
  vocabolario cresce a runtime. Il ratchet `tests/cuechains.sh` sorveglia.
  **Dipende da:** T16. **KB-first:** forma booleana, ordine dei moduli e ancoraggi
  strutturali restano C (forma, non vocabolario) — migrazione per categorie, mai cieca.

- [ ] **T19 — Proof che si invalidano e conflitti tipizzati.**
  **Cosa:** proof object con dependency id/versioni: retract/correzione/reload/
  patch invalida transitivamente ogni conclusione dipendente; stati
  `proved/refuted/both/unknown/floundered/budget_exhausted` rispettati da solver,
  match, NAF, planner e renderer.
  **Dopo parrot0 sa:** dopo "no, socrates is not a man", `why` cita solo supporti
  attivi; un conflitto non produce conclusioni unilaterali.
  **Dipende da:** T01. **Assorbe:** TODO 24, 25.
  **KB-first:** meccanica del solver; nessuna forma.

- [ ] **T20 — Layer di conoscenza puliti: overlay, memoria, quarantena.**
  **Cosa:** overlay/tombstone scoped per abduction/what-if/ablation (la base non si
  muove, source id immutabile); memoria conversazionale (nome, preferenze, goal,
  entità) come fatti session-scoped con schema e privacy policy — `/save` la
  ricostruisce esatta; acquisizione in quarantena con `trainable_schema`
  (consumer/validator/source dichiarati).
  **Dopo parrot0 sa:** dimenticare e correggere senza sporcare la base; sapere
  sempre da dove viene un fatto.
  **Dipende da:** T19. **Assorbe:** TODO 26, 27, 28.
  **KB-first:** gli schemi trainable sono fatti dichiarativi.

## Ondata F — espansione (T21–T24)

- [ ] **T21 — Proposal routing transazionale.**
  **Cosa:** i moduli emettono `Proposal` con confidence strutturale ed evidence; il
  router confronta (stesso scorer della percezione) e solo il champion commette
  effetti; selezione speculativa senza mutazioni; first-match come policy
  selezionabile per classi semplici.
  **Dopo parrot0 sa:** niente più turni rubati dall'ordine del registry; prompt
  articolati con secondo passo impossibile non lasciano effetti parziali.
  **Dipende da:** T01, T18. **Assorbe:** TODO 08, 23 (resto).
  **KB-first:** le regole di routing sono fatti; la meccanica di confronto è C.

- [ ] **T22 — Libreria di smell: n.5, n.6, …**
  **Cosa:** ogni istanza SWE-bench nuova non coperta tira uno smell generale nuovo
  (strutturale, con oracolo), che entra nella libreria e potenzia il localizzatore.
  **Dopo parrot0 sa:** riparare classi di bug sempre più ampie, repo dopo repo.
  **Dipende da:** T06, T12. **KB-first:** gli smell sono derivazioni su fatti IR.

- [ ] **T23 — Nuovi linguaggi e nuovi registri per fatti.**
  **Cosa:** un linguaggio nuovo come delta sull'IR (frontend + override); formati
  di diagnostica nuovi (altri test runner, linter, sanitizer) come
  `register_evidence/2` + consumer; ruoli di span nuovi (`acceptance`, `workaround`…).
  **Dopo parrot0 sa:** il costo marginale di un linguaggio/formato nuovo tende a
  zero — solo fatti, nessun parser C dedicato.
  **Dipende da:** T11, T09. **KB-first:** è la proprietà cardine di gen332 estesa.

- [ ] **T24 — KB di processo SWE + dogfood.**
  **Cosa:** testing, review, versioning, build system come fatti interrogabili
  (`kb/experts/programming/` cresce); parrot0 lavora su `src/brain` come codebase
  ordinaria (outer circle): migra le proprie catene residue (T18) con la macchina
  Track 5, senza accesso privilegiato.
  **Dopo parrot0 sa:** rispondere a domande di processo di sviluppo e migliorare
  se stesso come un repo qualunque — la prova generale della facoltà.
  **Dipende da:** T18, T12. **KB-first:** tutto ciò che sa di src/brain lo deriva
  dai file che legge.

---

## Il grafo delle dipendenze

```text
T01 kernel ──┬── T02 multilinea ── T07 sessione
             ├── T03 piano-KB ──┬── T04 budget/checkpoint
             │                 ├── T06 repair loop ◄── T05 PatchArtifact
             │                 └── T15 needhelp
             ├── T08 contratto issue ── T09 diagnostica→verdict ◄── T06
             ├── T11 IR ──┬── T10 explain
             │            ├── T12 localizzazione ── T22 smell++
             │            └── T13 CEGIS ── T14 schema induction
             ├── T19 proof/conflitti ── T20 layer/memoria/quarantena
             └── T21 proposal routing ◄── T18 phrasebook→KB ◄── T16 agent-vocab ◄── (subito)
                                                  └────────── T24 dogfood ◄── T12
```

## Tagli rispetto a TODO.md (e perché)

- **TODO 03 (Fast Forge)** e **TODO 30 (envelope/benchmark blind):** infrastruttura
  di misura e processo — fuori dal perimetro di questa lista per direttiva.
- **Linguaggio di gate** (holdout, promotion, SLO): rimosso dai criteri di chiusura;
  resta la regola di design *declino preciso verde, misclaim rosso*.
- Tutte le altre voci TODO.md (08, 10–29) sono qui, riordinate per leva funzionale
  e spogliate del processo: questa lista è il **cosa**, in ordine; il **come si
  certifica** resta al master plan, quando servirà.
