# parrot0 coding agent — il piano organico di evoluzione

> **Revisione gen335 (2026-07-17) — seconda fondazione.** Fino all'handoff gen312
> questo file era l'inventario storico dei track e della baseline gen256, con
> [`parrot0-forge-master-plan.md`](parrot0-forge-master-plan.md) come piano operativo
> canonico. Da allora tre documenti hanno cambiato le fondamenta del coding agent, e
> questa revisione li assorbe tutti — tecnologia, design, logica e architettura:
>
> 1. **[`universal-input.md`](universal-input.md)** (chiuso a gen332): la percezione
>    dell'input è KB-first. Nessun flusso di testo si classifica nel C; le span hanno
>    ruoli aperti, il registro è un'ipotesi a confronto di evidenza, ogni
>    tipizzazione porta provenance ed è ritrattabile. È la **facoltà zero** del
>    coding agent (§1).
> 2. **[`kb-first.md`](kb-first.md)**: il test **deduci · abduci · genera** diventa
>    la procedura di ammissione obbligatoria di ogni capacità nuova, su ogni track
>    (§4), con le sue regole di onestà non negoziabili.
> 3. **[`parrot0-forge-master-plan.md`](parrot0-forge-master-plan.md)** (revisione
>    Fast Forge): la crescita non è più lineare per casi ma per **fucina** —
>    esperimenti economici, candidate in competizione, promotion atomica dietro
>    oracolo. Questo file ne assorbe le conseguenze di architettura sulle facoltà e
>    sui track (§6–§10).
>
> **Ruolo:** possiede le sei facoltà R/P/T/M/K/L (§3), i vantaggi strutturali (§5),
> l'architettura target (§6–§9), la logica di crescita (§10), i track coding con i
> benchmark (§11–§12) e la disciplina (§13).
> **Subordinato a** `PRINCIPLES.md`, `LOOP.md`, `DESIGN.md`, `docs/CODE-MASTERY.md`
> e ai tre documenti sopra. Sullo **stato corrente** e sull'**ordine dei lavori**
> prevale il master plan; su *che cosa* il coding agent deve diventare, questo file
> è la fonte.

---

## 0. Mettere ordine — la mappa dei documenti

| Documento | Ruolo | Stato |
|---|---|---|
| `docs/CODE-MASTERY.md` | **Teoria**: codice = corpus nella stessa KB; struttura + verifica grounded al posto della compressione statistica | VIVO — la direzione |
| `docs/plans/kb-first.md` | Il test deduci·abduci·genera: cosa è derivabile KB-first | VIVO — disciplina trasversale, qui §4 |
| `docs/plans/universal-input.md` | L'input è UNO: percezione KB-first, span tipizzate, registro come ipotesi | CHIUSO gen332 — assorbito in §1 |
| `docs/plans/universal-comprehension.md` | Niente muro cieco: la forma si estrae sempre, le forme vivono nella KB | VIVO — premessa di §1 |
| `docs/plans/universal-solver.md` | Un solo motore su ogni superficie: prosa, teorema, spec di codice, paper | VIVO — orizzonte di §9 |
| `docs/plans/parrot0-forge-master-plan.md` | Piano canonico: fucina, trust mode, kernel, ondate W0–W9, milestone M-1a–M6 | VIVO — autorità su processo e ordine |
| `docs/plans/teachable-procedures.md` | Fatti + trasformazioni come conoscenza; native code no | VIVO — confine della facoltà L |
| `docs/plans/the-agency.md` | Teoria dell'agency: goal, osservazione, azione, differenza | VIVO — fondamento del kernel (§6) |
| `docs/plans/deep-reasoning.md` | Loop budgetato, estrazione, provenance, correzione | VIVO — substrato della facoltà R |
| `docs/plans/learn-and-build.md` | La catena LEARN→BUILD→TEST (sortlearn-bench, algo_shape) | VIVO — Track 2 |
| `docs/plans/generative.md` / `generative-prolog.md` | Generazione = struttura sotto oracolo; il ragionamento come cammino nel grafo KB | VIVO — principi di §9 |
| `docs/plans/mcp-engine.md` | Canale di teaching e inferenza persistente | VIVO — porta della fucina knowledge |
| `docs/plans/the-linguistic-glue.md` | Continuità tra moduli e turni | VIVO — interfaccia conversazionale |
| `docs/plans/coding-agent.md` | Inventario delle parti (gen148): gap G1–G13, fasi A–E, metriche M-C1–6 | STORICO — catalogo; l'ordine a fasi è superato dal pull |
| `TASKLIST.md` X-series | X1–X7: run, patch, node vocabulary, mirror languages, localizzazione, diff multi-file | VIVO — assorbita in §8 (X3–X5) e §7 (X7) |
| `tests/swebench/README.md` | Harness SWE-bench Lite reale, offline, oracolo Docker ufficiale | VIVO — scoreboard Track 1 |

Ciò che era in `code-generation.md` (template/phrasebook) resta rimosso: i template
sono il motore sbagliato (CODE-MASTERY §4). Da gen332 anche i *classificatori* di
testo nel C sono la stessa forma sbagliata, un livello più in basso (§1).

---

## 1. Facoltà zero — la percezione dell'input è KB-first

Prima delle sei facoltà c'è l'organo che le nutre tutte. gen332 lo ha chiuso; questa
sezione ne fissa le conseguenze sul coding agent.

### 1.1 La tesi

> **Un messaggio, un test, un sorgente, un diff, un log, una traccia di stack, un
> JSON: sono lo stesso oggetto — un flusso di testo. parrot0 non li *classifica*:
> li *comprende*. E la comprensione è KB-first.**
>
> Corollario che morde: **nessuna decisione su "che cosa è questo pezzo di testo"
> può vivere nel C.**

Il counterexample fondante (TODO 01): «fix this code: `int absval…` — It should
return the absolute value but…» produceva un fix inventato («add a semicolon»)
perché il C definiva il codice come "tutto dopo il primo due punti" e compilava
anche la prosa dell'utente. La diagnosi vera: **parrot0 non aveva un modello
dell'input**. E la prima cura era stata peggiore della malattia: un frasario
bilingue cablato (`strstr(low,"without")||strstr(low,"senza")||…`), un enum chiuso
di ruoli, un parser che conosceva solo le graffe del C. Un classificatore nel C è
**un frasario travestito da architettura** — peggio di un `printf` impostore,
perché si nasconde dentro un motore e sembra struttura.

### 1.2 Il modello (implementato, gen332)

```
   FLUSSO GREZZO (un turno: prosa, codice, output, diff, log — mescolati)
            │
            │  MOTORE (C, fisso):  delimitatori bilanciati, indentazione,
            │                      fence, righe, offset di byte
            ▼
   SPAN [start, len]  — pezzi, ancora senza nome
            │
            │  CONOSCENZA (KB):  register_evidence/2  → che registro è
            │                    segment_role/2       → che ruolo ha
            ▼
   SPAN TIPIZZATE  (instruction | code:c | expected | constraint | repro | log …)
            │
            │  CONOSCENZA (KB):  faculty_for/2  → chi la serve
            ▼
   FACULTY (compilatore, checker, planner, reasoner…) — vede SOLO la sua span
```

La riga che separa i due mondi, e va difesa a ogni generazione:

| **MOTORE — sta nel C** | **CONOSCENZA — sta nella KB** |
|---|---|
| bilanciare una coppia di delimitatori | *quali* delimitatori chiudono un registro |
| misurare indentazione, righe, fence | che Python chiude a indentazione e il C a graffe |
| tenere gli offset di byte (span, non copie) | che una span può avere il ruolo `repro` |
| confrontare più ipotesi per evidenza | quale evidenza vale per quale registro |
| dire «non chiude» → `ambiguous_input` | come si chiamano i ruoli, in quante lingue |

```prolog
% --- che registro è questa span? (evidenza, non certezza) ---
register_evidence(c,      keyword(int)).     register_evidence(diff,   line_prefix("@@")).
register_evidence(python, block(indent)).    register_evidence(pytest, line_prefix("E   ")).
% --- che RUOLO ha la prosa attorno al codice? ---
segment_role(expected,   "it should").       segment_role(constraint, "senza").
segment_role(observed,   "it returns").      segment_role(repro,      "to reproduce").
segment_role(non_goal,   "out of scope").
% --- chi serve un ruolo? ---
faculty_for(code(c), compiler_oracle).  faculty_for(expected, contract_builder).
```

Tutta la tassonomia vive in `kb/core/input.p0`. `kb_hypothesis_best` è **l'unico
scorer** per `intent_cue`, `register_evidence` e qualunque futura relazione di
evidenza: `winner`, `gap` e `ambiguous` hanno la stessa forma e conservano i fatti
di supporto.

### 1.3 Le proprietà che il coding agent eredita

1. **Un registro nuovo costa un fatto.** JSON, diff, pytest, cc, traceback sono
   entrati con zero righe di C specifiche; il test aggiunge `notice` e una lingua a
   delimitatori `< >` (`anglelang`) solo asserendo fatti. È il test dell'architettura:
   il costo marginale di un registro tende a zero.
2. **La segmentazione è interrogabile.** «Perché hai pensato che quella frase fosse
   un vincolo?» → «`segment_role(constraint, "senza")`». Una decisione con proof è
   una decisione che si può **correggere e ritrattare a runtime**: lo stesso
   processo MCP asserisce un ruolo, vede la nuova span, ritratta il fatto e vede
   sparire la tipizzazione — senza rebuild né restart.
3. **Il registro è un'ipotesi, non un `if`.** «`for`» è C, Python e preposizione
   inglese; un'ipotesi va sostenuta da evidenza e confrontata con le alternative.
   Quando due registri restano equiprobabili la risposta è `ambiguous_input`, mai un
   tiebreak cablato: **un declino preciso è verde**, applicato alla percezione e non
   solo all'azione. Le due keyhole disease (il registro di TODO 01, la lingua di
   TODO 23) erano la stessa malattia in due organi; un solo confronto di ipotesi le
   cura entrambe.
4. **Il contratto della issue è già percezione.** `expected`, `observed`, `repro`,
   `constraint`, `non_goal` sono `segment_role/2`: il contratto che il Verified Task
   Completion (§10.6) richiede di comprendere entra dalla porta della percezione,
   a costo di fatti — non da un modulo C nuovo.
5. **La diagnostica è un registro.** Output di pytest, cc, sanitizer incollati nel
   turno vengono isolati e tipizzati, e `faculty_for(log, verdict_builder)` li
   consegna alla faculty verdict: TODO 21 entra dalla stessa porta di tutto il resto.
6. **L'IR comune (§8) nasce dalle span tipizzate**, non da un parser per linguaggio.
7. **`Segment` è il primo tipo del kernel tipizzato (§6.2)**, a monte di
   `Task`/`Goal`: il kernel agentico non ingesta testo grezzo, ingesta span con
   provenance.

### 1.4 Oracolo

`tests/universal-input.sh` 64/64; `tests/kb-evidence-scale.sh` 11/11 (257
ipotesi/supporti, truncation esplicita, fast-path strutturali, anche sotto
ASan+UBSan); `tests/segment.sh` 28/28; `codebench` 25/25 (73/73 turni); payload MCP
oltre 95 KB con 128 span JSON complete (`tests/mcp-input-payload.sh`). Nessuna
regressione di comportamento in cambio della forma giusta.

---

## 2. Stato misurato a gen335 — cosa il coding agent SA GIÀ fare, e cosa no

Tutto ciò che segue è verificato da test ripetibili, non dichiarato. La fotografia
separa prove vive da narrazione storica e va rigenerata da macchina (capability
ledger, master plan §3/§4), non mantenuta a mano.

### 2.1 Cosa regge (con oracolo)

**Percepire (§1).** Segmentazione KB-first di flussi misti con ruoli aperti,
registri multipli (C, Python, JSON, diff, pytest, cc, traceback), provenance e
ritrattazione runtime; esposto via MCP (`input.segment`, `input.classify`).

**Leggere e rappresentare codice (F2 — AST-as-KB).** Ingest deterministico di C e
Python nella STESSA KB del linguaggio naturale (`code_function/1`, `code_calls/2`,
defs/vars); localizzazione per nome su un albero (`code_locate`,
`code_find_callers`).

**Ragionare (F3).** Motore SLD unico su fatti NL+codice: clausole n-arie, termini
composti, NAF, backward chaining, proof, provenance, persistenza stratificata.
Semantica Python derivata per DELTA da C; esecuzione simbolica di corpi aritmetici
(`code_eval`); deep reasoning con loop budgetato, proof e auto-correzione.

**Verificare per davvero (F5 — il vantaggio strutturale).** `code_compile` /
`code_build` / `code_run`: compilatore e runtime reali, exit code osservato — mai
"secondo me compila". `code_check_sort`: giudice run-grounded che ricalcola
ordinamento E permutazione da sé.

**Trasformare codice.** `code_rename`, `code_delete_function`, `code_replace_expr`
(comment/string-aware, originale mai toccato, copia patchata).

**Riparare bug REALI (Track 1).** 4 istanze REALI di SWE-bench_Lite RISOLTE
dall'oracolo Docker ufficiale (12907, 6938, 14995, 14365) con auto-localizzazione
nell'albero (`code_smell_tree`, gen256); nessun falso positivo su alberi puliti,
incluso `src/brain`.

**Sintetizzare sotto oracolo (Track 2).** Composizione aritmetica verificata
(gen206); sort da schema KB (`algo_shape` → synth → `code_check_sort`), sortlearn
9/9.

**Imparare (L).** Ricerca su markdown statici → `wiki_concept/3` persistito →
richiamo; FORGET→RELEARN→RECALL→BUILD→TEST dimostrata end-to-end. Procedure
insegnabili via clausole MCP: rewrite di token e computazioni ricorsive (`add/3`,
`len/3`, `app/3`).

**Agire (Track 3).** Tool read-only locali (gen205, `piagent-bench` 14/14); planner
articolato con artefatti tra i passi (gen207); `compose-bench` 47/47.

**Lavorare su una codebase — outer circle (Track 5).** L'arco completo su fixture
estraneo: detector generico di catene OR (gen257), dominio di planning in KB con
piano DERIVATO dal goal (gen258), esecuzione completa del piano su primitive mute
(gen259–263), primo sito reale migrato da conoscenza detta (gen271, ratchet
`tests/cuechains.sh`: il contatore delle catene `cue` può solo scendere), crescita
a runtime come puri dati (gen272: un verbo insegnato governa l'insegnamento
stesso).

**Onestà (trasversale, non negoziabile).** Ogni capacità mancante è un declino
dichiarato + gap ledger; proof trace su "why did you answer that way?";
`unknown`, `not-understood`, `ambiguous`, `conflicted`, `unsafe`, `timeout`,
`not-proven` e `budget_exhausted` sono stati distinti, mai collassati in `false`.

### 2.2 Cosa NON è ancora dimostrato (onestà, eredità del master plan §3.2)

- **Nessun loop generale** `goal → action → observation → verify → repair`: il
  kernel tipizzato di §6 è il target, non un risultato.
- **I 4 RESOLVED sono smell/localizzazione strutturale, non agency**: la issue non
  è compresa (`parrot_solve.sh` passa solo `fix the bug in <directory>`);
  `make swe-bench` resta degrade **0/5 engaged**. Smell benchmark e agent benchmark
  sono due misure diverse e non vanno confuse.
- `make game-bench`: **1/3** passi costruiti; planning e funzione `winner` restano
  decline.
- **Il ponte markdown → schema eseguibile → codice non esiste**: `algo_shape` è
  curato. Learn e build convivono, ma non sono causalmente collegati su un
  algoritmo nuovo — non chiamare "learn→build" una catena con schema scritto a mano.
- L'adapter OpenAI conserva solo l'ultimo messaggio user, rende opzionale il
  system, ignora history/tool schema e tronca a 2000 byte.
- Array fissi troppo piccoli per repo e sessioni reali; il parser codice è un
  insieme utile di scanner, non un IR con scope, CFG, dataflow, tipi, macro e id
  stabili; l'esecuzione usa `popen` + whitelist di prefissi — **non una sandbox**
  sufficiente per repair autonomo (§7 è la cura).
- Debito di governance in chiusura W0a: semantica gate/discovery di `glue`, `mimic`,
  MMLU, BBH sotto audit; `LLMSCORE` è discovery, non verità (un LLM judge ha già
  accettato un fatto falso).

---

## 3. Le sei facoltà del coding agent target (la richiesta di F.)

F. (2026-07-02): *reasoning, planning, controllo dei tool, potente capacità d'uso
della macchina, abilità di programmazione e conoscenza utile allo sviluppo, e in
più l'apprendimento: analizza sorgenti markdown remote, impara, e dopo produce e
modifica codice applicando le conoscenze acquisite in un quadro ampio e generale,
senza limiti di contesti applicativi.*

Ogni facoltà ha: cosa esiste, il gap, il pull, la **maturità** (scala §10.3:
ABSENT/SEED/TRANSFER/FIELD/HARDENED) e **l'upgrade** che le tre fondamenta (§1, §4,
§6–§10) le hanno dato.

| Facoltà | Maturità | Muro per il livello successivo |
|---|---|---|
| Percezione (§1) | TRANSFER | consumer dei registri (`verdict_builder`), registri su repo freddi |
| R — Reasoning | TRANSFER (inferenza KB: FIELD) | comportamento oltre aritmetica: loop, stato, aliasing; IR §8 |
| P — Planning | SEED | action model in KB + replan su osservazione (kernel §6) |
| T — Tool control | TRANSFER (su fixture) | executor sicuro §7.1, tool contract, repo freddi |
| M — Machine use | SEED | PatchArtifact multi-file, containment, rollback (§7, W2) |
| K — Knowledge | TRANSFER (dominio curato) | X3 vocabolario nodi astratto, mirror languages, SWE KB |
| L — Learning | SEED | markdown→schema indotto; quarantena/holdout/promotion (W1/W6) |

### R — Reasoning (ragionamento sul codice)
- **C'è:** motore SLD unico su fatti NL+codice; derivazioni strutturali (smell,
  simmetrie, dataflow parziale); semantica per delta; proof trace; loop budgetato
  con auto-correzione.
- **Gap:** ragionamento sul COMPORTAMENTO oltre l'aritmetica (loop, stato,
  aliasing); il percorso generativo alla generative-prolog applicato al codice.
- **Upgrade:** la semantica incrementale di §8.5 fissa l'ordine tirato dai task
  (scope → CFG → def-use → loop/stato → exceptions → alias → summary
  interprocedurali); ogni nuova analisi deve chiudere un task reale e trasferire su
  un secondo caso mai visto. Ogni fatto IR porta confidence `parsed | derived |
  hypothesized` — la stessa legge di provenance delle span (§1) e delle frasi
  (TODO 22).
- **Pull:** istanze swe-bench il cui bug richiede capire un'esecuzione, non solo
  una forma; `code-bench` semantici.

### P — Planning
- **C'è:** planner articolato (gen207) con artefatti tra i passi; `mod_plan` su
  grafi di dipendenza; il loop perceive→decide→act→observe (gen116–120).
- **Gap:** il planning domain `code_task`; oggi il ciclo edit→verify→retry non
  ITERA sul fallimento. I piani devono essere INFERITI da un dominio di azioni
  dichiarato in KB (pre/postcondizioni come fatti), mai cablati come pipeline in C.
- **Upgrade:** gli `action/5` schema con precondizioni, effetti, rischio e costo
  vivono in KB (§6.5); il planner lavora su Observation **replayate** da subito —
  l'executor sicuro è prerequisito per agire live, non per esplorare il core
  (ports/adapters, §10.9). Il replan scatta su `DIAGNOSE`: minimizza il primo
  contratto rotto, poi ripara, cambia ipotesi o acquisisce il fatto mancante
  (needhelp, Track 3).
- **Pull:** game-bench e il primo schema `algo_shape` errato (repair loop B4, §9.3).

### T — Tool control
- **C'è:** tool locali read-only in un turno (gen205); compilatore/runtime come
  tool deterministici; il pi agent come guscio.
- **Gap:** git come tool; esecuzione guardata di comandi arbitrari; scelta del tool
  derivata dal KB.
- **Upgrade:** ogni tool ha un **contratto versionato** (§7.4): argv schema, input
  kinds, output parser, side effects, risk tier, timeout, max output, capability
  richiesta, ruolo-oracolo. Git entra come tool strutturato
  (`status/diff/log/show/blame/apply --check`); il planner interroga
  `tool_for(goal, tool)`, il C esegue il contratto sicuro. I livelli di rischio
  R0–R4 (§7.5) decidono autonomia e approvazioni.
- **Pull:** game-bench e piagent-bench quando un passo richiede un tool nuovo.

### M — Machine use (uso potente della macchina)
- **C'è:** filesystem sandboxed (lettura alberi, scrittura `.p0fix`/temp);
  build+run reali; sweep ricorsivo di un repo (gen256).
- **Gap:** write-back controllato; multi-file; processi con osservazione di
  stdout/stderr come fatti.
- **Upgrade:** §7 intera — executor argv-based senza shell implicita, containment
  con `dirfd`/`openat`, limiti di risorse, `PatchArtifact` transazionale con
  rollback automatico. Il risultato di un edit non è più una scrittura ad hoc ma un
  artefatto con base/preimage/postimage hash e rollback diff. Sandbox per
  costruzione, non per policy.
- **Pull:** la prima istanza swe-bench multi-file; il game-bench che deve scrivere
  un progetto.

### K — Knowledge (abilità di programmazione e conoscenza SWE)
- **C'è:** `kb/experts/programming/` (algoritmi, complessità, `faster_than/2`),
  `kb/bash.p0`, schemi `algo_shape`; i fatti-linguaggio come fatti, non phrasebook.
- **Gap:** X3–X5 — vocabolario di nodi astratto, linguaggi come mirror concept,
  conoscenza SWE di processo come fatti interrogabili.
- **Upgrade:** X3 è assorbito dall'IR comune (§8.1: `file, module, symbol, scope,
  node, definition, reference, call, read, write, assign, branch, loop, return,
  exception, type, value_shape, control_edge, data_edge, test, diagnostic`) e X4/X5
  dai frontend per delta (§8.2). Anche la *conoscenza di percezione* (registri,
  ruoli, consumer) è knowledge del coding agent: `kb/core/input.p0` è il modello —
  un formato di diagnostica nuovo è un gruppo di fatti, non un parser.
- **Pull:** la prima istanza non-astropy / non-Python; le domande llmscore su
  concetti di sviluppo.

### L — Learning (il vincolo di F.: markdown remoti → conoscenza → codice)
- **C'è:** la catena è dimostrata su un caso (sortlearn 9/9); procedure insegnabili
  via MCP; autolearn con ledger delle failed lesson.
- **Gap (il ponte cruciale):** l'**induzione di struttura** — dai passi di una
  pagina markdown mai vista allo schema eseguibile, con lo STESSO induttore e
  nessuno schema curato.
- **Upgrade doppio.** (a) Il **trust mode ACQUIRE** (§10.5) risolve la vecchia
  tensione "mai rete vs markdown remoti": acquisizione da allowlist fattuale in
  quarantena dati, l'output entra come `observation`/`candidate_fact`, mai come
  verità ufficiale; la rete resta vietata in RUNTIME. (b) La **fucina knowledge**
  (§10.8) rende sicuro ciò che autolearn provava a fare: schema registry,
  quarantena, holdout, ablation, promotion atomica, rollback. E l'onestà resta:
  fatti e procedure dichiarative sono conoscenza; **native code via MCP no** — una
  primitiva C nuova passa dalla fucina engine (W7), non dal canale di teaching.
- **Pull:** selection sort e binary search da pagine fresche (campagna C2); il
  ratchet è che nessun nuovo schema venga scritto a mano.

**Sul "senza limiti di contesti applicativi":** la generalità onesta viene da
CATEGORIE e SCHEMI (smell strutturali, `algo_shape`, IR astratto, mirror concept,
registri/ruoli come fatti), mai da risposte per-contesto. Dove la categoria non
copre, parrot0 declina e il gap ledger nomina il prossimo pull. L'ampiezza cresce
per selezione, generazione dopo generazione — categories-not-prompts, applicata al
codice.

---

## 4. Il test di ammissione di ogni capacità: deduci · abduci · genera

Da `kb-first.md`: **la bussola**. Prima di qualsiasi generazione su qualsiasi
track, la capacità richiesta passa questo esame. Non è una preferenza estetica: è
la differenza tra un sistema che impara una forma nuova con **un fatto** e uno che
la impara con **una generazione**.

**La tesi fondante:** lingua e codice/logica abitano la stessa struttura
elaborativa. Sotto la superficie di una frase — anche metafora, analogia, aforisma
— c'è un'**anima logica** derivabile. Scartare una domanda come "generativa, fuori
portata" è lecito **solo** dopo aver provato a derivarne lo scheletro e aver
dimostrato che non c'è. Quasi sempre c'è.

**Passo 1 — Decomponi la superficie nello scheletro.** Quali simboli? quali
relazioni? che *tipo di conclusione* serve (sì/no, un valore, una scelta A/B, una
variazione di grandezza)?

**Passo 2 — Classifica la mappa fra superficie e struttura:**

| Classe | Quando | Mossa |
|---|---|---|
| **Deduci** | struttura e mappa note o derivabili | catena di regole + risoluzione → **costruisci** |
| **Abduci** | la struttura c'è, la mappa va *ipotizzata/scelta* | abduzione + oracolo di plausibilità → **costruisci** (più costoso/incerto) |
| **Genera** | produzione aperta senza struttura verificabile (poesia, prosa originale) | **soffitto onesto**: ammetti, non fingere |

Solo la terza classe è il limite no-LLM. La distinzione vera non è "simbolico vs
LLM": è **mappa nota vs mappa da abdurre vs generazione irriducibile**.

**Passo 3 — Inventario degli organi** (`kb_assert_rule`+SLD, `mod_analogy`,
`mod_abduce`, chooser, chiusure in C sopra i fatti, e da gen332 il confronto di
ipotesi su evidenza di `kb_hypothesis_best`).
**Passo 4 — Costruisci il più piccolo incremento** che generalizza (mai 1 frase =
1 `printf`), con un bench oracolare che studia il comportamento, non cuce frasi.
**Passo 5 — Onestà (sotto).**

**Il pattern di costruzione:** Parse robusto (declina se non riconosce) → fatti KB
che reificano la conoscenza → resolver (SLD per le catene unarie, chiusura in C per
le binarie/transitive) → decisione del chooser → verbalizzazione da
`response_template` → bench oracolare.

**Le regole di onestà (non negoziabili):**
- **Declina quando la catena non regge.** Se manca un fatto o un anello: "non lo so
  / non capisco ancora", con il primo anello mancante nominato — mai inventare.
- **Quando la nuova conoscenza rende rispondibile un test di umiltà, sposta il
  test, non falsificare il muro.**
- **Non spacciare generazione per conoscenza.** Un artefatto non verificato si
  chiama **candidato**, mai soluzione.
- **Runtime-growth test obbligatorio** (PRINCIPLES, regola cardinale): ogni nuovo
  riconoscitore linguistico deve provare che asserire un fatto a runtime cambia il
  riconoscimento senza rebuild, e che ritrattarlo/ablatirlo lo rimuove. Un golden
  fisso da solo prova comportamento, non architettura KB-first.

---

## 5. I vantaggi strutturali sui coding agent basati su LLM remoti

Discendono dall'architettura e vanno PRESERVATI da ogni generazione (sono criteri
di review di ogni feature). I primi dieci sono la lista storica; gli ultimi due
sono gli upgrade di questa revisione.

1. **Privacy totale / offline.** Il codice non lascia mai la macchina; funziona in
   air-gap. La rete esiste solo nei trust mode ACQUIRE/FORGE (§10.5), mai in
   RUNTIME.
2. **Determinismo e riproducibilità.** Stesso input → stessa patch, sempre; un fix
   è riproducibile in CI e bisecabile.
3. **Verifica grounded per costruzione.** parrot0 non DICHIARA un fix: lo propone
   da struttura e lo fa giudicare da compilatore/test reali. Fabricated success = 0
   è un hard invariant (§10.7), non un augurio.
4. **Spiegabilità totale (proof trace).** Ogni patch ha una ragione strutturale
   nominabile e ogni risposta un percorso di derivazione ispezionabile.
5. **Niente allucinazioni per costruzione.** Ciò che non è derivabile viene
   declinato con il gap nominato — mai codice inventato con tono fluente.
6. **Conoscenza ispezionabile, editabile, versionata.** Il "modello" è la KB: file
   di testo diffabili in git; correggere una conoscenza è un commit, non un
   fine-tuning.
7. **Costo ~zero e latenza ms.** Un binario C su CPU; gira su un Raspberry Pi.
8. **Sandbox per costruzione.** Executor argv-based, containment `openat`,
   transazioni con rollback (§7) — per principio, non per policy.
9. **Contesto = filesystem, non finestra.** L'albero si interroga per query; un repo
   da milioni di righe non "trabocca".
10. **Onestà come contratto.** Il gap ledger è parte dell'output.
11. **Percezione interrogabile (upgrade gen332).** Ogni decisione di comprensione —
    registro, ruolo, intento — cita il fatto che l'ha prodotta e si ritratta con un
    fatto. Nessun agente LLM può mostrare *perché* ha "visto" codice in un
    messaggio; parrot0 risponde con `register_evidence(...)` / `segment_role(...)`,
    e l'utente lo corregge a runtime senza rebuild.
12. **Crescita selezionata, non accumulo (upgrade fucina).** Ogni capacità entra da
    una campagna con ablation, holdout e rollback testato; una activation policy
    decide champion/challenger/shadow (§10.3). Conservare informazione non
    significa conservare interferenza: il set di abilità dell'agente è auditabile
    quanto la KB.

**Il contro-lato onesto (la frontiera, CODE-MASTERY §4):** l'LLM resta superiore su
F4 — il ponte associativo intent↔codice e la sintesi open-domain. Non lo simuliamo
con phrasebook: lo misuriamo con la pipeline issue→regione di §8.4 e la CEGIS di
§9, e ogni istanza reale tira o un meccanismo strutturale nuovo o la prova di un
muro. Quel confine misurato È il deliverable dell'esperimento.

---

## 6. Architettura: il kernel agentico tipizzato

Il salto mancante non è un altro `mod_*`: è un kernel che possa rappresentare un
lavoro di coding lungo. (Design: master plan §6, qui fuso con §1.)

### 6.1 Perché: il contratto attuale non basta

```c
typedef int (*Handler)(Brain *, const char *norm, const char *raw,
                       char *out, size_t out_size);
```

Ogni modulo può solo vincere e scrivere testo, oppure perdere. Il registry
first-match è utile struttura secondaria conversazionale, ma il suo ordine è
diventato una **semantica nascosta** e causa regressioni di routing (l'aritmetica
che ruba `agent_search`, gen313–315). Un task di coding lungo non ci sta in un
turno vinto/perso.

### 6.2 I tipi interni minimi

```text
Segment     span tipizzata del flusso di input, con ruolo e provenance (§1) — IL PRIMO TIPO
Event       fatto osservato, immutabile, con sorgente e timestamp logico
Task        richiesta esterna, vincoli, workspace, budget, policy
Goal        stato desiderato, parent, precondizioni, stato epistemico
Constraint  proprietà che una soluzione deve soddisfare
Action      schema + argomenti + rischio + costo previsto
Observation risultato reale di un'azione, stdout/stderr/status/hash
Artifact    file, diff, patch, indice, build o report con content hash
Verdict     oracle, esito, evidenza e copertura
Gap         stadio, requisito mancante, trace causale, severità
Decision    candidati considerati e ragione della scelta
```

Ogni tipo: rappresentazione C interna; forma serializzabile JSONL per audit;
proiezione in fatti KB per query e planning; id stabili e riferimenti, non copie di
stringhe sparse; provenance dal raw input fino al verdetto. `Segment` sta a monte:
il kernel non ingesta testo grezzo, ingesta span già tipizzate dalla percezione
KB-first — e il `Gap` tipizzato è anche il minerale della fucina (§10.2) e il
contenuto del `needhelp.p0` (Track 3).

### 6.3 Il ciclo operativo target

```text
INGEST    conserva messaggi, issue, repository, policy e budget
INTERPRET produce goal, constraint, ambiguità e gap iniziali (sulle span di §1)
OBSERVE   interroga workspace e raccoglie fatti con fonte
PLAN      deriva passi da action_schema + pre/postcondizioni
PROPOSE   genera più azioni o patch candidate
SELECT    ordina per evidenza, rischio, costo e informazione attesa
ACT       esegue una primitiva sicura e produce Observation/Artifact
VERIFY    chiama l'oracolo appropriato
DIAGNOSE  minimizza il primo contratto rotto
REPLAN    ripara, cambia ipotesi o acquisisce il fatto mancante
RENDER    restituisce risultato, prova, diff e limiti
```

Il loop è budgetato per passi, wall clock, processi, byte letti/scritti e tentativi.
Quando esaurisce una risorsa restituisce `budget_exhausted`, **non** `false`.

### 6.4 Moduli come propositori, non solo vincitori

Migrazione additiva: (1) gli handler correnti restano il fallback; (2) i moduli
possono emettere `Proposal` con confidence strutturale ed evidence; (3) un router
dichiarativo confronta più proposal — lo stesso confronto di ipotesi della
percezione (§1), applicato all'azione; (4) un composer può sceglierne più di una se
gli effetti non confliggono; (5) first-match resta una policy selezionabile per
classi semplici; (6) le regressioni vengono confrontate in shadow mode prima del
cambio champion.

### 6.5 Il piano è conoscenza

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

Precondizioni, effetti, rischio e costo non sono cablati in una pipeline C
per-task. L'ablation di un action schema deve rendere il gap esplicito (gate di
uscita di W3).

### 6.6 Stato scalabile e articolazione reale

`Brain` resta facade e possiede: **`BrainImage`** frozen (config, registry, indici,
KbImage condivisibile), **`SessionContext`** (lingua, memoria cross-turn, mondi,
KbOverlay/tombstone), **`TaskContext`** (arena dinamica, workspace, budget,
action/event log, candidate artifacts). Query union, provenance, override, negativi
e retract devono restare differenzialmente equivalenti. I limiti `KB_MAX_ARGS`,
`KB_MAX_BODY`, `KB_TERM_LEN` non si alzano per speculazione: diventano errori/gap
osservabili, rimossi quando una campagna reale li colpisce. Indici, tabling e cache
arrivano dopo profiling su scala dichiarata. Fino alla migrazione dei buffer
statici nei context, parrot0 è dichiarato non-reentrant: parallelismo tra processi,
non thread su un Brain condiviso.

---

## 7. Architettura: la macchina sicura e transazionale

Le facoltà T e M prendono questa forma (master plan §7; ondata W2). Prerequisito
per **agire live**, non per esplorare: planner e repair lavorano da subito sugli
stessi tool contract contro adapter in-memory o Observation replayate (§10.9).

### 7.1 Executor strutturato
`fork`+`execve`/`posix_spawn` con `argv[]`, **mai una shell implicita**; cwd
esplicita e verificata; environment allowlist; timeout wall-clock e CPU; process
group terminabile; limiti memoria/file size/open files/nproc; cap e hash di
stdout/stderr; exit status, signal e durata come `Observation`; network off in
RUNTIME e nei test non autorizzati. `popen(command_string)` e la whitelist a
prefisso vengono aboliti.

### 7.2 Contenimento filesystem
Root aperta con `dirfd`; accesso con `openat` e `O_NOFOLLOW`; path canonico sempre
dentro root; symlink/device/FIFO/socket rifiutati salvo policy; test e oracle
read-only o verificati con hash pre/post; nessuna lettura automatica di `.env`,
chiavi, home o file fuori workspace.

### 7.3 Edit transazionale — `PatchArtifact`
Il risultato di un edit è un artefatto: base tree hash, preimage hash per file,
operazioni strutturate, unified diff, postimage hash, file creati/eliminati/
modificati, rollback diff. Flusso: worktree usa-e-getta → applicazione candidate →
parse/compile veloci → test mirati → regressioni pertinenti → presentazione del
diff → applicazione atomica al workspace autorizzato → **rollback automatico** se
il post-check fallisce. X7 (patch multi-file + write-back guardato) diventa questa
transazione.

### 7.4 Tool registry
Ogni tool ha un contratto versionato: `name, argv schema, input kinds, output
parser, side effects, risk tier, timeout, max output, required capability, oracle
role`. Git entra come tool strutturato (`status`, `diff`, `log`, `show`, `blame`,
`apply --check`) — il vecchio D4 "git-as-tool" è una riga di questo registry.

### 7.5 Approvazioni

| Rischio | Esempio | Default |
|---|---|---|
| R0 | query KB, parse, read file | autonomo |
| R1 | grep, git diff/log, test read-only | autonomo con budget |
| R2 | scrittura in worktree candidato | autonomo, rollback obbligatorio |
| R3 | applicazione al workspace canonico | approvazione o policy esplicita |
| R4 | rete, install, publish, secret | negato salvo autorizzazione specifica |

---

## 8. Architettura: repository intelligence

Il principio di CODE-MASTERY resta: AST e repository sono KB. Va completato con un
vocabolario non legato al C (master plan §8; assorbe X3–X5 e la frontiera F4).

### 8.1 IR comune

```text
file, module, symbol, scope, node, definition, reference,
call, read, write, assign, branch, loop, return, exception,
type, value_shape, control_edge, data_edge, test, diagnostic
```

Ogni fatto porta: language e frontend version; file + span byte/line/column;
stable node id; parent/scope; source hash; confidence `parsed | derived |
hypothesized`. L'IR nasce dalle **span tipizzate** di §1, non da un parser per
linguaggio: il percepito e il rappresentato obbediscono alla stessa legge di
provenance.

### 8.2 Frontend per delta
C e Python emettono lo stesso IR; le differenze sono override/delta (indentazione,
macro, dynamic typing, comprehension, generator…). Gli analizzatori generali
parlano IR, non sintassi concreta. Il compilatore/interprete reale resta oracolo
quando la semantica è incerta. Un parser deterministico esterno può essere un tool
— runtime autonomo, output normalizzato nel nostro IR, dependency dichiarata; il
fallback C puro resta per i sistemi minimi.

### 8.3 Indici di repository
Symbol index e reverse reference; call graph e import/include graph; test-to-code
map (nomi, import, coverage); diagnostics index da build/test; change history via
git; cache per hash file; invalidazione incrementale dopo patch.

### 8.4 Issue → regione tramite evidenza (la cura di F4, senza keyword hardcoded)
1. estrarre entità, comportamenti attesi, errori, nomi e constraint dalla issue —
   sono le span `expected/observed/repro/constraint` di §1;
2. collegarli a simboli, stringhe, test, docs e diagnostics;
3. costruire candidati di localizzazione con proof;
4. raccogliere nuova evidenza solo dove discrimina le ipotesi;
5. valutare top-k su repo con file decoy;
6. usare il fallimento dei test per aggiornare il ranking.

Metriche: recall top-1/top-5, righe lette prima della localizzazione, false
positive su repo puliti, robustezza a paraphrase della issue. (Sostituisce il
vecchio X6 "associativo solo come esperimento misurato": l'esperimento ora ha una
pipeline e una metrica.)

### 8.5 Semantica incrementale
Ordine tirato dai task, non enciclopedia upfront: scope e binding → CFG essenziale
→ def-use e side effects → loop e stato → exceptions e resources → alias/value
domains quando una issue li richiede → interprocedural summaries →
language-specific delta. Ogni nuova analisi deve risolvere almeno un task reale e
trasferire su un secondo caso mai visto.

---

## 9. Architettura: generazione e repair — dal template a CEGIS

La generazione open-domain non può essere un `printf` più grande. Il codice è un
candidato in uno spazio di programmi, guidato da constraint e disposto da un
oracolo (master plan §9).

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
Procedure e schemi appresi; analogia strutturale con codice del repo; regole di
trasformazione; componenti già verificati; enumerazione limitata da
tipi/constraint; repair di un candidato vicino; **mai testo libero non parseable
presentato come soluzione**.

### 9.3 Repair loop B4 (il gate del coding loop)
Un candidato volutamente errato deve: fallire con un verdict strutturato →
collegare il diagnostic a nodo/constraint (il diagnostic è una span tipizzata, §1)
→ proporre almeno due fix → scegliere quello con minore blast radius → riapplicare
e rieseguire → fermarsi entro N tentativi → conservare tutta la trace. Solo dopo
questo gate il progetto può dichiarare di avere un coding loop.

### 9.4 Il vero learn → build
La catena è valida solo se una fonte mai vista causa lo schema usato dal builder:
pagina markdown fresca → passi/constraint estratti → schema candidato con
provenance → esempi e proprietà generati → codice sintetizzato → oracle →
**ablation: senza pagina/schema il task torna rosso**. Gate iniziali: selection
sort da pagina assente dal KB; binary search con caso not-found; una trasformazione
di testo con proprietà verificabili; stesso induttore, nessun `algo_shape` nuovo
scritto a mano.

### 9.5 RULESCORE riposizionato
RULESCORE resta una **miniera di task**, non un gate di promotion finché un giudice
LLM assegna il voto. Ogni mini-gioco va tradotto in: input grammar, transition
relation, state invariant, termination/win condition, property test o transcript
oracle. I punti reali provengono dall'esecuzione di quelle proprietà; il declino
onesto vale 0 ma è nominato come onesto; la fabbricazione è flaggata ed è peggio di
un muro; i punti salgono solo per CATEGORIE chiuse nel motore (loop di input,
stato, terminazione, condizioni di vittoria), mai inseguendo il singolo gioco.

---

## 10. La logica di crescita: la fucina

Come ogni facoltà avanza di livello senza degradare il kernel. Questa sezione è la
condensa operativa del master plan (§1, §4, §10–§12, §18) applicata ai track
coding; il master plan resta l'autorità.

### 10.1 Il feedback è parte dell'architettura

```text
il Tick orienta · il Turn seleziona · la Promotion dimostra · il Nightly certifica
```

**Un esperimento non è una generazione.** Molti esperimenti economici competono e
muoiono in locale; una generazione nasce solo quando una candidata sopravvissuta
viene promossa. Il rigore resta intero, ma viene pagato una volta dal champion, non
da ogni ipotesi. Ogni livello restituisce presto il **primo contratto rotto**, non
solo un PASS/FAIL finale; il tempo da edit al primo controesempio è una metrica di
prodotto.

### 10.2 I verbi della fucina

- **Seal:** sigilla l'esperimento (base commit, tree hash, semantic payload digest,
  toolchain, policy, budget, challenge pack separato). Nessun edit post-gate.
- **Mine:** il minerale sono failed lesson, regressioni, misclaim, gap di
  code/game/SWE/RULESCORE/basic-chat, task reali non chiusi, `needhelp.p0`
  strutturati, counterexample di campagne promosse — raggruppati per **trace
  causale** (data, schema, representation, parsing, inference, routing, planning,
  tool, workspace, synthesis, oracle, rendering, safety), non per parole del prompt.
- **Assay:** ogni fallimento produce minimo riproduttore rosso, atteso/osservato,
  primo contratto rotto, componente proprietaria, oracolo disponibile, rischio di
  overfit, negativi necessari. Un miglioramento del solo harness è marcato
  `harness`, non conta come capacità.
- **Alloy:** ogni candidata vive in overlay/worktree usa-e-getta, classificata
  `knowledge | procedure | policy | engine | agent | harness`.
- **Hammer:** ciclo bounded `propose → parse/build → execute → observe → diagnose
  → repair`; si ferma al primo controesempio; più candidate competono con
  **successive halving**, non con una full matrix ciascuna.
- **Temper:** le corsie (sotto).
- **Quench:** promotion atomica — candidato, test guadagnati, manifest, scorecard,
  provenance, decision record, activation change, rollback testato, aggiornamento
  capability ledger, in **una** generazione piccola, reversibile e bisecabile.
- **Anneal:** replay periodico, champion/challenger in shadow sulle stesse trace,
  rollback che riattiva lo snapshot, cluster di rollback che diventano engine gap.

### 10.3 Maturità e attivazione: due assi indipendenti

| Livello | Significato | Gate minimo |
|---|---|---|
| ABSENT | nessun comportamento utile | gap riproducibile |
| SEED | un dimostratore curato | red→green + ablation |
| TRANSFER | generalizza oltre il caso di sviluppo | mutazioni, negativi, EN/IT o equivalente, 10x |
| FIELD | chiude task freddi in ambienti diversi | holdout nascosto, più fixture/repo, budget rispettato |
| HARDENED | affidabile per uso produttivo | replay, sicurezza, zero regressioni, rollback provato |

Stati di attivazione: `inactive → shadow → scoped_canary → active → disabled |
rolled_back`. Un package può essere `TRANSFER + shadow` o `FIELD + active`:
`hardened` non è sinonimo di attivazione, e una variante superata resta archiviata
con la sua evidenza — **keep-secondary con activation policy**, non accumulo
globalmente attivo.

L'unità di selezione è il **capability package**: identity+version, implementation
(fatti, procedure, policy o C), predicati/azioni esportati, dipendenze e
incompatibilità, train/variant/holdout/negative test, oracle e provenance,
resource/safety envelope, scorecard e maturity, activation policy, rollback
artifact. È la forma concreta degli expert componibili di DESIGN.md.

### 10.4 Le corsie (Temper) e la regola dell'holdout

| Corsia | Chi la paga | Contenuto | Budget P95 iniziale |
|---|---|---|---:|
| S0 Spark | ogni edit | parse/schema, build dev, fixture/oracle hash | ≤ 2 s warm, ≤ 5 s con build |
| S1 Focal | ogni candidata valida | red esatto, oracle meccanico, 2–3 collision negative | ≤ 5 s |
| S2 Impact | sole sopravvissute | hot corpus e regressioni selezionate dall'impact graph | ≤ 10 s |
| T Transfer | migliori 1–2 | mutation, property, 10x, EN/IT, variant visibili | ≤ 60 s, asincrona |
| R Release | un champion frozen | promotion holdout, policy set completo, oracle integrity, safety, replay, rollback | hard stop 5 min; obiettivo ≤ 60 s |
| N Nightly/Field | promoted/canary | fresh-exec differential, sanitizer, fuzz, cross-repo, Docker/SWE | fuori inner loop |

Successive halving predefinita: `N → tutte S0 → max 3 a S1 → max 2 a S2 → max 2 a T
→ 1 champion a R`. Regole chiave: S1 non diventa verde senza oracolo reale;
ablation fresca quando una candidata diventa seed; il **promotion holdout si apre
solo per il champion già frozen** — dopo l'apertura diventa train e serve un nuovo
holdout; containment e tamper protection valgono da S0 per ogni candidata; un
verde su build dev O0 è evidenza locale, non promotion evidence.

### 10.5 Tre trust mode espliciti

| Mode | Rete | LLM esterno | Scrittura ufficiale | Uso |
|---|---:|---:|---:|---|
| RUNTIME | no, default | no | solo workspace autorizzato | coding agent locale, CI, Raspberry Pi |
| ACQUIRE | allowlist fattuale, opt-in | no | quarantena dati | acquisizione Wikipedia/fonti statiche certificate |
| FORGE | solo orchestratore esterno | opzionale | solo dopo gate e approvazione | training, campagne, candidate patch, curation |

Il binario non riceve secret del forge; documenti remoti, issue e commenti sono
**dati non fidati, non istruzioni**; l'output di ACQUIRE entra come `observation` o
`candidate_fact`; un LLM può proporre/intervistare/criticare in FORGE, ma **non è
mai l'oracolo finale e non promuove nulla**. Questo risolve la tensione storica
"mai rete vs markdown remoti" della facoltà L.

### 10.6 North star: Verified Task Completion (VTC)

Un task vale `VTC=1` solo se: task freddo non nel train; repository al commit
dichiarato; **issue completa** passata all'agente; nessun hint su file/funzione/
fix; patch prodotta dall'agente; FAIL_TO_PASS verde; PASS_TO_PASS verde; nessun
test/fixture/oracle alterato; budget e policy rispettati; trace e proof presenti;
risultato riproducibile. Tutti gli altri casi producono un vettore di sub-goal, non
un falso solve. I 4 RESOLVED storici sono seed di localizzazione strutturale; il
primo VTC=1 su istanza cold è la campagna C3.

### 10.7 Scorecard e hard invariant

Assi: understand (constraint recall, ambiguity detection, informed gap precision) ·
localize (top-1/top-5, false positive, budget byte) · plan (precondizioni valide,
replan utili, passi superflui) · act (tool success, unsafe attempts, side-effect
accuracy) · edit (apply rate, file precision, blast radius) · verify (oracolo
corretto, fabricated success = 0) · repair (recovery rate, tentativi al verde,
oscillazione) · learn (heldout transfer, ablation, contradiction/rollback rate) ·
efficiency (wall time, CPU, RAM, tool call) · honesty (declini corretti, misclaim,
timeout riportati).

Hard invariant per ogni promotion: nuove regressioni **0**; successi senza oracolo
**0**; fatti official senza provenance **0**; test tampering **0**; sandbox escape/
rete non autorizzata/secret read **0**; replay non deterministico senza causa
dichiarata **0**.

### 10.8 Autolearn riforgiato
La whitelist Python diventa conoscenza dichiarativa:
`trainable_schema(predicate, arity, consumer, validator, source_policy,
conflict_policy, holdout_generator, promotion_policy)`. Un predicato non registrato
non può essere promosso; un fatto mai letto dal consumer fallisce S0. Layer di
provenance distinti: `OBSERVATION → CANDIDATE → OFFICIAL`, più `REFLECTIVE`
(derivato dallo stato runtime) e `REJECTED` (conservato per non riapprenderlo). Una
correzione invalida le conclusioni che dipendono dal fatto, non solo la riga.

### 10.9 Counterexample capsule e replay
Ogni failure costoso viene ridotto a capsula: input minimo, Action/Observation con
request hash, exit/signal, stdout/stderr e truncation, proprietà rotta, owner,
oracle — sanificata da secret e path host. Il record/replay non è un giudice
simulato: la stessa ToolPort alimenta fake, replay e live; una deviazione produce
`replay_miss`, mai la risposta canned successiva e mai un fallback live automatico.
Livelli di evidenza distinti: `model/fake` (invarianti del planner) → `strict
replay` (decisioni sugli archi osservati) → `live hermetic` (adapter e side effect
reali) → `field` (envelope esterno). Un pass replay non diventa claim di agency
live.

### 10.10 Self-model e capability truth
Il self-model non deriva più solo da `module(name)`: fatti riflessivi
(`capability(X, transfer)`, `supported_by(X, campaign)`, `verified_at(X, commit,
ts)`, `oracle_for(X, bench)`, `known_gap(X, …)`) sono **generati dal capability
manifest al boot**, mai persistiti come conoscenza di sessione né scritti a mano.
Disabilitare un package rimuove la capacità dichiarata; una regressione abbassa
automaticamente la maturità. "Perché?" cita test e commit. (PRINCIPLES:
l'autodescrizione si deriva dallo stato reale — lo stato ora include il ledger.)

---

## 11. I binari di evoluzione (pull-based: nessun ordine cronologico rigido)

Ogni generazione prende UN obiettivo da UN binario, tirato da un test rosso reale.
I binari avanzano in parallelo per selezione. Ogni track dichiara la propria
classe d'ammissione (§4) e la propria campagna forge (§10).

### Track 1 — Repair su repo reali (`make swe-bench` / `swe-solve`)
Il binario dei 4 RESOLVED (smell + auto-localizzazione). Onestà: seed di
localizzazione, non agency (§2.2). Prossimi pull, in ordine di pressione:
1. **Più istanze** (curation `fetch_lite.sh N` + snapshot `repo_excerpt`): ogni
   istanza nuova o è coperta da uno smell esistente (ratchet gratis) o tira lo
   smell generale n.5, n.6, … La libreria di smell È il localizzatore.
2. **Excerpt multi-file con decoy** come norma.
3. **X7 → PatchArtifact (§7.3):** dal `.p0fix` alla transazione con rollback.
4. **Issue → regione via §8.4** (pipeline a evidenza con metriche), alimentata
   dalla percezione di §1: il contratto della issue sono span `segment_role/2`.
5. **Campagna C3 — prima issue fredda VTC=1** (§10.6): holdout = stessa bug class
   in repo diverso + issue vicina senza bug per i false positive.
6. **14182 (feature add)**: il primo caso oltre la riparazione — richiede Track 2.

### Track 2 — BUILD: sintesi sotto oracolo (learn-and-build)
1. **B4 repair loop (§9.3)** tirato da un rosso reale: è il pezzo che chiude il
   loop agentico edit→verify→ITERATE e il gate per dichiarare un coding loop.
2. **Secondo/terzo `algo_shape`** (selection/insertion, binary search) per provare
   il riuso dello schema.
3. **Campagna C2 — il ponte L:** pagina fresca → constraint → schema candidato con
   provenance → codice → property verdict; holdout = binary search o insertion con
   lo stesso estrattore, pagina mutata; promotion solo se nessuno schema specifico
   è scritto a mano e senza pagina/candidato il build non avviene (W6).
4. **Composizione:** due funzioni sintetizzate indipendenti devono comporre.
5. **RULESCORE** come miniera (§9.5): ogni gioco tradotto in grammar/transizione/
   invariante/terminazione/property test.

### Track 3 — Agente: planning + tool + macchina

> **Il protocollo needhelp (steer F., 2026-07-02 — pattern pubblico).** Quando nel
> planning/reasoning parrot0 capisce che una cosa NON la sa e non può procedere,
> **non si ferma**: crea un `needhelp.p0` nella ROOT del progetto su cui lavora,
> dove esprime ciò che gli manca; chiunque (umano o altro agente) può scrivere la
> risposta; periodicamente, come passo del piano, parrot0 torna a controllarlo.
>
> **Vincolo: abilità intelligente, non fallback cablato.** Il protocollo stesso è
> CONOSCENZA in KB: la RICHIESTA è fatti `.p0` (il goal, il fatto mancante, il
> contesto — la forma è il `Gap` tipizzato di §6.2), la RISPOSTA è altri fatti
> `.p0`, e "ricevere aiuto" = caricare quel file come conoscenza nativa. Il check
> periodico è un passo con precondizioni, inferito dal planner come qualunque altra
> azione — mai un ramo C "se fallisci scrivi il file". I `needhelp.p0` strutturati
> sono anche minerale della fucina (§10.2).

1. **Repair loop nel planner** (condiviso con Track 2 B4): un passo fallito propone
   il fix invece di fermarsi (il ciclo DIAGNOSE→REPLAN di §6.3).
2. **game-bench come pull e2e:** ogni voce del ledger è una generazione.
3. **git-as-tool → tool registry (§7.4):** log/diff/blame come osservazioni.
4. **Esecuzione guardata → executor (§7.1):** output → `Observation` (più fatti di
   sessione, gen240).
5. **Adapter multi-turn:** system, history, tool results, input multiline —
   l'interfaccia del kernel verso l'esterno (W3).

### Track 4 — Conoscenza: linguaggi e SWE come KB
1. **X3 → IR comune (§8.1):** le analisi smettono di parlare C; un front-end nuovo
   riusa gli analizzatori invariati.
2. **X4/X5 → frontend per delta (§8.2):** `like(L, <abstract>)` + override,
   misurando il rapporto trasferito/specifico.
3. **KB SWE di processo:** tool, testing, versioning come fatti interrogabili.
4. **Conoscenza di percezione come knowledge coding:** registri, ruoli e consumer
   nuovi (un formato di log, un dialetto di diff) entrano come fatti in
   `kb/core/input.p0` — il costo marginale tende a zero ed è il ratchet di §1.

### Track 5 — Lavorare su una codebase (outer circle) — la conoscenza dirige il comportamento

> **Steer F. (2026-07-02):** NON è riflessività. La facoltà è generale: parrot0
> studia una codebase e interviene — e una di quelle codebase *guarda caso* è la
> sua. Nessun accesso privilegiato, nessun self-model, nessuna auto-riparazione per
> design: dogfood **outer circle**. Tutto ciò che sa di src/brain lo deriva dai
> file che legge, come fece con astropy.

**Stato dell'arco (chiuso):** percezione detector OR-chain (gen257) → dominio di
planning in KB con piano DERIVATO (gen258) → esecuzione completa su primitive mute
con giudice differenziale byte-identico (gen259–263) → primo sito reale migrato da
conoscenza detta (gen271) → crescita runtime come puri dati (gen272). Il fixture
estraneo attraversa l'intero arco: **anti-impostor** — se funziona solo su
src/brain, abbiamo barato.

**Prossimi pull:**
1. Siti cue-chain per categorie; il contatore in `tests/cuechains.sh` può solo
   scendere (CLASSI di parole in KB; forma booleana, ordine dei moduli e ancoraggi
   strutturali restano C — forma, non vocabolario).
2. Il dogfood entra in fucina: ogni migrazione è un **capability package** (§10.3)
   con la sua campagna, il suo holdout e il suo rollback — non più solo un ratchet
   di conteggio (W7/W8).
3. Engine gap ripetuti → clustering automatico → red test minimizzato → candidate
   patch C in worktree isolati → promotion con approvazione esterna obbligatoria
   (W7: il soggetto non modifica mai direttamente il proprio branch).

---

## 12. Benchmark e ratchet (gli strumenti di misura)

| Bench | Misura | Stato | Flip che segna progresso |
|---|---|---|---|
| `tests/universal-input.sh` | percezione: span, registri, ruoli, provenance, ablation live | 64/64 (gen332) | un registro nuovo entra con soli fatti |
| `tests/kb-evidence-scale.sh` | scorer di evidenza a scala (257 ipotesi, truncation, fast-path) | 11/11, ASan+UBSan | cresce con la KB senza soglie nascoste |
| `tests/mcp-input-payload.sh` | payload MCP > 95 KB, 128 span JSON | verde | nessuna truncation silenziosa |
| `tests/segment.sh` | segmentazione codice/prosa | 28/28 | ruoli nuovi a runtime, zero C |
| `make code-bench` | comportamento su stimoli di codice | 25/25 (73/73 turni) | gate nuovi per smell/semantica |
| `make swe-solve INSTANCE=…` | patch derivata + oracolo Docker | 4/5 RESOLVED, self-localized (smell) | istanza n.5+; primo multi-file; primo VTC=1 cold |
| `make swe-bench` | mappa dei blocker (degrade mode) | 0/5 engaged | ogni blocker che sparisce; harness che passa la issue completa |
| `make sortlearn-bench` | LEARN→BUILD→TEST end-to-end | 9/9 (schema curato) | schema INDOTTO da pagina fresca (C2) |
| `make piagent-bench` | tool locali + multi-step | 14/14 | nuovi tool sotto contract §7.4 |
| `make game-bench` | progetto e2e in 3 prompt | 1/3 ledger onesto | ogni voce del ledger chiusa |
| `make compose-bench` | dialoghi compositivi | 47/47 | resta verde |
| `make glue-bench` | continuità conversazionale | 9/9, semantica discovery sotto audit W0a | ratchet reale dopo l'audit di classificazione |
| `make llmscore` | sonda di breadth (giudice esterno) | 4/10, discovery — non gate | item di coding chiusi per categoria |
| `make rulescore` | testo→regole→codice | miniera (§9.5) | categorie di regola codificate, non singoli giochi |
| `tests/cuechains.sh` | contatore catene `cue` in src/brain | 341 → in discesa (Track 5) | solo giù, per categorie |
| `make test` | l'intero ratchet ermetico | verde | resta verde SEMPRE |

Nota di governance: i golden black-box stanno migrando verso la piramide del master
plan §10.13 (pure/unit → brain contract → adapter contract → e2e → cold/field) con
risposta strutturata (`text, status, winner, declined_trace, goals, proof, actions,
artifacts, oracle_verdict, gap`): una modifica di phrasing non deve rilanciare
centinaia di prove di planning, e una collisione di routing deve nominare subito il
modulo che ha rubato il turno.

---

## 13. Disciplina (invariata, e vincolante per ogni binario)

- **Un obiettivo per generazione**, il più piccolo passo che generalizza (LOOP.md);
  **un esperimento non è una generazione** — candidate molte, promotion una.
- **Pull, non push**: si costruisce solo ciò che un test rosso reale richiede;
  l'ordine dei binari lo detta la pressione dei bench (pivot duty: tre primitive
  senza task reale → torna domain-pull; tre task con lo stesso workaround → engine
  pull).
- **KB-first con il test di §4**: deduci·abduci·genera prima di ogni capacità;
  ordine di chiusura dei gap: fatto/lessico → schema dichiarativo → clausola/
  riscrittura → action schema → primitiva C generale → modifica architetturale
  (solo se più gap indipendenti mostrano lo stesso muro).
- **Nessuna classificazione di testo nel C** (§1): registri, ruoli, intenti e cue
  sono fatti con evidenza e proof; il C bilancia, misura, pesa ipotesi.
- **Categorie, non prompt** (steer gen254): si chiudono CLASSI con regole del
  motore; un misclaim è peggio di un muro.
- **Anti-impostor**: il generatore propone, l'oracolo dispone; *introspection
  proposes, tests dispose*; un artefatto non verificato è un **candidato**, mai una
  soluzione.
- **Keep-secondary con activation policy**: cambi additivi, varianti archiviate con
  evidenza, champion/challenger/shadow — conservare informazione ≠ conservare
  interferenza.
- **Runtime-growth test** per ogni riconoscitore nuovo: assert/retract a runtime
  prova la forma KB-first; un golden fisso non basta.
- **Onestà degli stati**: `unknown`, `not-understood`, `ambiguous`, `conflicted`,
  `unsafe`, `timeout`, `not-proven`, `budget_exhausted` distinti; `timeout`/
  `infra_error`/`flaky` non sono FAIL funzionali e non diventano PASS per rerun.
- **Bilingue** EN/IT sull'interfaccia; cross-linguaggio sul codice (stesso
  meccanismo, non secondo path).
- **Trust mode** (§10.5): rete solo in ACQUIRE/FORGE, mai in RUNTIME; un LLM solo
  in FORGE come proposer/critic, mai oracolo, mai promotion.

---

## 14. Le prossime generazioni (proposta concreta, rivedibile a ogni pull)

Le dodici promotion infrastrutturali di Fast Forge (contratto per id con streaming,
manifest/exit semantics, build dev/release separate, dipendenze `.d`, version TU
isolata, gate-result attestato, capability-report come trasformazione pura,
isolamento porte/tmp, sharding, scheduler globale) sono **owned dal master plan
§15** e qui solo ereditate: ogni track sotto ne consuma il feedback (focale ≤ 5 s,
primo evento ≤ 500 ms). Poi la frontiera coding, scelta per evidenza — frequenza
del gap, qualità dell'oracolo, riuso atteso, rischio, effort:

1. **Kernel tipizzato MVP (W3):** `Segment`+`Event`+…+`Gap`, event log JSONL,
   action schema in KB, planner che deriva `inspect → edit → verify` su trace
   replayate; ablation di uno schema rende il gap esplicito.
2. **Repair loop bounded (§9.3)** su candidato volutamente errato — il primo
   "coding loop" dichiarabile.
3. **Executor + containment + PatchArtifact (W2):** X7 diventa transazione; suite
   adversarial safety verde.
4. **Adapter multi-turn:** history, system, tool results, input multiline.
5. **IR comune + indici (W4):** X3 assorbito; localizzazione top-5 ≥ 80% su
   campagna fredda multi-repo, senza hint.
6. **Campagna C2:** pagina fresca → schema → codice verificato; facoltà L completa.
7. **Campagna C3:** prima issue fredda con VTC=1; PASS_TO_PASS intatto; false
   positive misurati.
8. **git-as-tool** nel registry (§7.4): domande sul repo reale come osservazioni.
9. **game-bench:** prima voce del ledger chiusa col repair loop del planner.
10. **Track 5:** prossimi siti cue-chain per categorie; il dogfood come capability
    package in fucina (W7/W8).
11. **Replay/capsule:** i failure costosi di Track 1–2 ridotti a counterexample
    capsule; il core decisionale esplora policy in millisecondi su Observation
    sigillate.

Ogni voce resta subordinata al pull: se un bench rosso indica altro, quello vince.

---

> **Frase guida (ereditata dalla fucina):** scaldare un fallimento reale, batterlo
> in più varianti, temprarlo su casi che non ha visto, e promuoverlo solo quando
> l'oracolo dimostra che è diventato una capacità. La percezione (§1) decide *cosa*
> il fallimento dice; il test di ammissione (§4) decide *se* la cura è conoscenza;
> la fucina (§10) decide *quando* è diventata una capacità. Il coding agent è ciò
> che queste tre discipline, applicate alle sei facoltà, lasciano standing.
