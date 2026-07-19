# Dossier operativo — round guidati verso un coding agent usabile

> **Stato:** handoff aggiornato in `gen341`; ciclo guidato chiuso a T06 e
> ritaratura semantica della suite avviata il 2026-07-19. La missione
> complessiva resta aperta e riparte dai residui ordinati nelle sezioni 10-11.
> **Scopo:** conservare le scoperte prodotte mentre un supervisore usa parrot0
> come subagent, e ordinare le leve logiche che trasformano i suoi fallimenti in
> facoltà generali. Questo dossier affianca
> [[act-as-subagent]], [[coding-agent-todo]] e [[abstraction-ceiling]]; non li
> sostituisce.

## 1. Missione e criterio di onestà

La missione non è far produrre a parrot0 una patch fortunata. È portarlo a
chiudere, su un repository ordinario, il ciclo:

```
task -> goal/constraint -> piano derivato -> azione -> osservazione
     -> patch candidata -> oracolo -> repair/replan -> verdetto
```

Una facoltà è acquisita soltanto se:

1. cambia il comportamento su un task reale;
2. trasferisce a un caso non nominato nel codice;
3. espone artefatti, osservazioni e gap strutturati;
4. lascia decidere a build/test/oracolo, mai alla propria prosa;
5. se riconosce linguaggio naturale, cresce e si abla a runtime tramite KB;
6. non amplia il fail-set ereditato.

Il test pratico di usabilità è un task dato senza hint su file o funzione, con
patch applicabile, verifica reale, almeno un fallimento intermedio recuperato e
nessuna affermazione di successo prima del verdetto dell'oracolo.

## 2. Baseline sigillata

Stato iniziale osservato su `main` (`1f39125`, `gen339-meta-reasoning-l14`):

- worktree pulito;
- build senza warning;
- harness conversazionale: **180 pass / 73 fail**;
- i 73 nomi falliti sono identici a `gen337` (che aveva 178 pass / 73 fail):
  gen338-gen339 hanno aggiunto due pass e nessuna nuova fixture rossa;
- la descrizione storica «73 casi `.it`» è imprecisa: sono 50 fixture `.it.chat`
  e 23 non italiane;
- `tests/cuechains.sh`: **226/226**, quindi 226 è il ratchet iniziale;
- al boot del profilo AGI vengono scartate **20 clausole** con parse error:
  2 template `audit_report`, 9 template lunghi, 8 regole in `procedures.p0` e
  `consistent_kb` in `meta.p0`;
- due suite conversazionali lanciate contemporaneamente interferiscono sui
  temporanei e producono 74 fail: il confronto differenziale va eseguito in
  serie.

## 3. Round 1 — diagnosi L14 affidata a parrot0

### Incarico

Dopo gli otto insegnamenti di `scripts/prompts/kbfirst-preamble.txt`, parrot0 ha
ricevuto un task già rilevante: investigare perché «which relations are
transitive?» non raggiunge il consumer `relation_type`, identificare la causa e
proporre fix KB-first più runtime-growth test.

### Risultato osservato

Il preambolo è attecchito (recall corretto di `kb-first` e `cue-migration`), ma
il task aperto ha prodotto soltanto:

```
I read docs/plans/abstraction-ceiling.md but found no structural bug to fix.
```

Una richiesta successiva di fare un piano è stata rubata dal gate narrativo e
trasformata in una storia. Anche fornendo esplicitamente i due fatti collidenti,
parrot0 non ha derivato la causa. Il round misura quindi due gap distinti:

- **planning gap:** esiste un planner generico, ma non un dominio `code_task`;
- **reasoning/localization gap:** il testo letto non diventa ancora IR/fatti su
  cui il motore possa confrontare cardinalità, call site e alternative.

### Causa tecnica trovata dal supervisore

Il motore `mod_answer_frame` consente una cue associata a un solo predicato
effettivo: interroga `answer_frame(Cue, Pred)` con cap 1. Nel KB esistono invece
associazioni many-to-one additive:

```
answer_frame("transitive", transitive).
answer_frame("transitive", relation_type).
```

Il primo predicato è sterile sulla domanda e il secondo non viene mai provato.
La stessa forma compare per `symmetric` e `asymmetric`; nell'intero registro ci
sono altri duplicati con predicati diversi. La leva corretta è quindi generale:
enumerare tutti i consumer associati alla cue e scegliere il primo che produce
evidenza, preservando l'accumulo additivo richiesto da PRINCIPLES.

`relation_arity` ha una causa distinta: il frame esiste, ma non esistono fatti
`relation_arity/2`. La direzione onesta è derivarli dalla struttura reale della
KB, non curare a mano un elenco destinato a divergere.

### Oracolo del round

Il ratchet deve coprire:

- collisione su una cue nuova: primo consumer sterile, secondo consumer verde;
- retract del secondo consumer: ritorno al rosso senza rebuild;
- caso reale `which relations are transitive?`;
- `relation_arity` con assert/retract del fatto, separando il fix del consumer
  dalla futura proiezione riflessiva delle arità.

## 4. Scoperta architetturale: collegare gli organi prima di ampliarli

La ricognizione iniziale aveva trovato organi separati: il kernel tipizzato T01
in `src/agent.c`, il planner KB-first, trasformazioni di codice non
transazionali, compile/run legacy e nessun retry dopo un verdetto rosso. Aveva
anche smascherato una falsa proiezione `rec/5`: il motore ammette al massimo
quattro argomenti e il vecchio test controllava una stringa senza caricarla nel
KB.

Il collegamento acquisito usa `rec/4` per `Id,Kind,Parent,State` e
`rec_source/2` per la provenance. Il Brain possiede ora il `P0AStore`, e una
transizione attraversa `brain_agent_set_state`: store tipizzato e fatto `rec/4`
nel KB vivo vengono aggiornati come un'unica operazione osservabile. Se la
proiezione KB non riesce, lo store non resta avanti con una copia epistemica
vecchia; il ratchet `tests/brain-agent-state.c` verifica anche questo cammino.

T06 consuma infine lo stesso substrato per registrare `Observation`, `Verdict`,
`Decision` e `Gap`. La scoperta generale rimane valida: aggiungere altri smell
o prompt prima di collegare store, patch, executor e policy aumenterebbe la
copertura locale, non l'agency.

## 5. Round 2 — il piano di coding diventa stato

### Incarico crescente

Con il medesimo preambolo, parrot0 ha ricevuto prima:

```
make a plan for a code task
```

e poi:

```
execute the plan for a code task
```

### Comportamento osservato

Il primo comando ha derivato dal KB, senza una pipeline C di vocabolario
dedicata:

```
inspect_workspace -> localize_change -> edit_candidate -> verify_candidate
```

e ha materializzato un `Task`, un `Goal` e quattro `Action(candidate)`. Il
secondo comando non ha millantato letture o modifiche: si è fermato al primo
passo con «no action_impl fact names its primitive».

Il ratchet causale usa una singola sessione MCP: retrae
`action_yields(localize_change,target_region)`, osserva `Goal(blocked)` più un
`Gap(target_region)`, poi riafferma lo stesso fatto e riottiene le quattro
azioni senza rebuild. Sono inoltre interrogabili rischio, costo e tool
dichiarati per le azioni. L'esito consolidato è **8/8**, con
`tests/agentkernel.sh` a **34/34**.

### Cosa ha tirato il round

- Il planner ora sa **cosa** dovrebbe accadere, ma non sa ancora eseguire il
  contratto generico `inspect/localize/edit/verify`.
- Il record store e `rec/4` condividono ora la transizione atomica del Brain;
  non resta più una futura promessa architetturale.
- Un `Action` di edit non può essere reso eseguibile prima di avere
  `PatchArtifact`: collegarlo direttamente a scritture live renderebbe il piano
  più attivo ma meno sicuro.

## 6. Precondizione d'ingresso — nessun task per prefisso (T02, slice API)

Il daemon OpenAI normalizzava correttamente CRLF e contenuto strutturato, poi
tagliava in silenzio il prompt al byte 2000. Per un coding agent questo è un
errore semantico: acceptance, traceback o coda del diff possono sparire mentre
il prefisso rimasto sembra ancora una issue completa.

Il confine dell'adapter è ora esplicito e coerente con il massimo corrente del
Brain: fino a **65 535 byte** il prompt normalizzato viene consegnato intero;
oltre, il daemon risponde HTTP 413 con:

```
input_too_large + bytes + limit + fnv1a64:<digest>
```

Il ratchet end-to-end prova due proprietà che un unit test sul buffer non
coprirebbe:

1. un array OpenAI di parti strutturate, con CRLF e fatto decisivo oltre byte
   2000, raggiunge il Brain e il fatto di coda è richiamabile;
2. un input oltre budget che inizia con un side effect conversazionale viene
   rifiutato prima di `brain_respond`: nome e stato precedenti restano invariati.

Esito: **7/7**. Questo chiude il troncamento HTTP, non T07: l'adapter continua a
usare l'ultimo messaggio user e non rappresenta ancora l'intera history, tool
call e tool result. Restano inoltre da rendere dinamici altri buffer interni
quando una prova oltre l'attuale budget li tirerà.

### 6.1 Precondizione epistemica — il KB deve caricare ciò che dichiara

Il boot AGI scartava silenziosamente proprio conoscenza necessaria ai livelli
alti: clausole multilinea, template oltre 127 byte e il fatto zero-arity
`consistent_kb`. Un coding loop governato da policy e action schema KB-first non
può essere più affidabile del suo loader.

Il loader ora accumula **clausole logiche**, non righe fisiche: rispetta quote ed
escape, commenti, più clausole sulla stessa riga, regole multilinea e arità zero.
Un frammento non terminato a EOF, una clausola oltre il budget derivato di 23 296
byte o una body oltre gli 8 goal non viene mai caricata per prefisso: viene
rifiutata intera con diagnostica e lo scanner riprende dalla clausola successiva.
Il boot AGI passa così da **20 a 0 parse error**; il ratchet focal è **15/15**.

Il primo tentativo di crescita del termine da 128 a 512 byte ha però rivelato un
controesempio cruciale: i grandi `Subst` e resolvent erano automatici nei frame
ricorsivi, quindi `factorial(15)` — verde sulla baseline — moriva prima della
risposta. La correzione non è stata abbassare di nuovo il tetto testuale: gli
scratch logici bounded sono ora posseduti sullo heap per frame e sempre liberati.
Il frame C massimo misurato scende da 160 704 byte sulla baseline a 50 896 byte,
`factorial(15)` torna verde anche sotto ASAN/UBSAN, e il costo AGI osservato è
esplicito: circa 14,0→33,6 MiB RSS e 0,14→0,18 s al boot. La leva generale è:
**alzare una dimensione rappresentazionale non deve abbassare la profondità di
ragionamento**; entrambe vanno ratchettate nello stesso test.

## 7. Leve logiche, in ordine di dipendenza

### Leva A — Rendere il piano uno stato interrogabile (T03)

Collegare `P0AStore` al Brain; materializzare ogni piano derivato come record
tipizzati; descrivere il dominio `code_task` in KB con precondizioni, effetti,
rischio, costo e contratto tool. Ablando un action schema deve comparire un
`Gap` che nomina l'artefatto mancante. Aggiungendo lo schema a runtime, lo stesso
binario deve completare il piano.

**Perché viene prima:** senza questo, parrot0 non sa dove si è fermato e ogni
round deve re-interpretare prosa effimera.

**Stato consolidato:** la slice fondativa è chiusa. Il Brain possiede lo store,
la proiezione `rec/4` + `rec_source/2` è caricata davvero nel KB, gli stati sono
aggiornati atomicamente e il loop T06 materializza nello stesso grafo record di
osservazione, verdetto, decisione e gap. Restano il journal durable e il
collegamento alla policy di commit, non più la consistenza store/KB.

### Leva B — Trasformare l'input della issue in Goal/Constraint (T02/T08)

Eliminare il limite di una riga/2000 byte e proiettare gli span già prodotti da
universal-input in Task, Goal, Constraint e non-goal con provenance. Il piano
deve perseguire l'acceptance, non una keyword come `fix`.

**Perché abilita trasferimento:** una issue nuova differisce nei contenuti, non
nella struttura expected/observed/repro/constraint.

**Stato:** il trasporto API non tronca più e rifiuta atomicamente l'oversize;
la proiezione semantica degli span in `Goal/Constraint` (T08) e la history
fedele (T07) restano aperte.

### Leva C — PatchArtifact transazionale (T05)

Ogni edit diventa un candidato con base hash, pre/postimage, diff, target
espliciti e rollback. Prima si applica/verifica in candidato isolato; il
workspace cambia solo sotto autorizzazione e soltanto per l'artefatto giudicato.

**Perché abilita autonomia sicura:** l'agente può tentare senza trasformare un
errore di diagnosi in perdita di dati.

**Stato tirato dal round 2:** il tipo ora esiste come dato persistibile e
immutabile, non come nome dato a una copia. Possiede pre/postimage complete,
mode e SHA-256 per `edit/create/delete/rename`, diff avanti/indietro e un formato
binario che viene rihashato al caricamento. La preparazione costruisce una tree
candidata indipendente e bounded, vi applica l'intero batch, verifica tutte le
postimage e lascia decidere a un checker meccanico. Il commit canonico richiede
una callback di policy, rivalida *tutte* le preimage prima della prima scrittura,
usa installazioni no-replace per create/rename e, su un post-check rosso,
ripristina e verifica byte, path e mode. Il ratchet è **8/8** e include
persistenza cross-process, stale base a zero scritture, policy negata,
rollback multi-file, mutazione fraudolenta da parte del checker, adapter per
sorgente detached e corruzione su disco rilevata.

Il nome corretto della garanzia è però **transazione recuperabile**, non
atomicità assoluta: non c'è journal durable e un crash può lasciare un prefisso
multi-file; un `EDIT` ha ancora una finestra revalidate→rename contro un editor
concorrente e anche il rollback può collidere con modifiche esterne. Lo staging
omette `.git/.cache/.venv`, è limitato a 1 GiB/200k file/128 livelli e rifiuta
symlink, special file e touched hardlink. Questi limiti sono parte della
capability truth, non note da nascondere.

### Leva D — Osservazione e verdetto unificati

Compile, run, test e git devono passare dall'executor argv-based e produrre
`P0Obs`; parsare l'esito in `Verdict` collegato alla Constraint. Timeout,
infra-error, fail funzionale e output troncato restano stati distinti.

**Perché impedisce l'impostore:** nessuna stringa «still compiles» può sostituire
l'osservazione reale che la giustifica.

**Ponte candidate-root acquisito:** `p0_exec_at(rootfd, ...)` risolve la cwd
componente per componente con `openat(O_NOFOLLOW)` e porta il child nell'oggetto
validato con `fchdir`; `p0_exec` usa lo stesso core. Il focal **14/14** prova cwd
nested, scrittura relativa solo nella tree candidata, verdetti zero/nonzero,
timeout e trace, e rifiuta assoluti, `..`, symlink e rootfd non-directory.
`rootfd` è una capability trusted, non un `chroot`: un processo autorizzato può
ancora usare path assoluti. La prossima connessione deve quindi far eseguire
l'oracolo PatchArtifact tramite questa capability senza attribuirle isolamento
che il kernel non offre.

### Leva E — Repair/replan bounded (T06)

Su un verdetto rosso: diagnosticare il primo contratto rotto, produrre almeno
due trasformazioni applicabili dichiarate in KB, scegliere il blast radius
minore, agire, riverificare e aggiornare la frontier. Dopo N tentativi: Gap o
budget_exhausted, mai successo narrato.

**Questa è la soglia del coding loop:** prima di E parrot0 produce candidati;
dopo E può recuperare da un proprio errore.

**Stato consolidato:** la soglia è stata attraversata su un adapter reale, ma
non ancora generalizzata alla repository. Il controller in
`src/brain/75-agent-repair.c` è domain-neutral: legge esaustivamente dal KB vivo
`repair_rule(Diagnosis,Transform)` e `repair_budget(Domain,N)`, materializza ogni
trasformazione applicabile come `PatchArtifact`, calcola il blast radius dai
byte dell'artefatto, scarta stati già visti e sceglie meccanicamente il minimo.
Ogni candidato viene nuovamente osservato; una diagnosi diversa restringe la
frontier del tentativo successivo.

L'adapter sort dimostra il ciclo causale completo. Una regola errata aggiunta a
runtime ma con blast minore viene scelta per prima; il suo verdetto rosso cambia
la diagnosi, provoca replan e il terzo tentativo è verde. Con budget 2 la stessa
traccia termina `budget_exhausted`. Retraendo le regole, lo stesso binario
produce i terminali distinti `zero_rule` e `no_applicable`; ripristinando la
regola corretta torna verde in un tentativo. Il controller registra nel P0A
store e nel KB fatti `repair_observation`, `repair_verdict`,
`repair_candidate`, `repair_decision`, `repair_attempt` e `repair_terminal`,
con `Gap` tipizzati per gli stop.

Build e run sono osservazioni separate: una build rossa non inventa un run. Il
tool `cc` è autorizzato dal fatto vivo `tool_for(build,cc)`; l'ablazione del
fatto produce un missing-tool tipizzato, non un fallback shell. I ratchet
`tests/agentrepair.sh` e `tests/repair.sh` sono verdi (quest'ultimo **9/9**).

Il limite onesto è nel bordo adapter: solo il contratto sort è stato provato e
la sua tree privata non attraversa ancora il ponte comune
`PatchArtifact -> p0_exec_at -> policy -> commit/rollback`. T06 è quindi un
controller riusabile con una sola istanza dimostrata, non ancora un coding
agent repository-general.

### Leva F — IR e localizzazione issue -> regione (T11/T12)

Portare file, simboli, reference, call, branch, def-use e span in un IR comune
con identità stabili e provenance. Il ranking issue->file->symbol deve citare
l'evidenza e sopravvivere a decoy.

**Perché serve al task aperto:** il fallimento L14 mostra che leggere testo e
codice senza convertirli in relazioni interrogabili non consente diagnosi.

### Leva G — Generazione sotto oracolo (T13/T14)

Solo dopo il loop: schema o procedura appresa -> sketch -> candidati -> oracolo
-> controesempio -> raffinamento. Il ponte markdown->schema deve essere causale;
un `algo_shape` curato non prova apprendimento.

## 8. Sequenza dei prossimi incarichi a parrot0

I primi quattro incarichi del vecchio piano sono stati assorbiti dai round
chiusi. La nuova sequenza parte quindi da complessità rilevante e cresce sul
numero di decisioni autonome e di feedback chiusi, non sulla lunghezza del
prompt.

1. **Chiudere il ponte candidato-oracolo-policy.** Far preparare al controller
   un `PatchArtifact` persistente, aprire la sua root candidata come capability,
   eseguire build e test con `p0_exec_at`, registrare i due `P0Obs`, e consentire
   il commit canonico soltanto a una policy KB viva. Ablazione di tool o policy
   deve lasciare il workspace byte-identico e produrre il `Gap` corrispondente.
2. **Proiettare un'issue completa (T08).** Da expected/observed/repro/
   acceptance/constraint produrre `Task`, `Goal`, `Constraint` e non-goal con
   span e provenance lossless. Il piano deve inseguire l'acceptance, non la
   parola `fix`.
3. **Rappresentare history e tool trace (T07).** Conservare turni, tool call,
   tool result e causalità senza ridurli all'ultimo messaggio user; verificare
   budget e troncamento come stati espliciti.
4. **Costruire localizzazione evidence-backed (T11/T12).** Indicizzare
   file/simboli/reference/call/branch/def-use con identità stabili; su una issue
   con decoy, il ranking deve citare evidenza e trovare la regione senza hint.
5. **Chiudere un task single-file non-sort.** Parrot0 deve localizzare,
   proporre almeno due alternative KB-backed, scegliere per rischio/blast,
   fallire una volta, ripianificare, verificare e chiedere/applicare policy di
   commit senza adapter dedicato al bug.
6. **Chiudere un task multi-file.** Acceptance e regressione devono essere
   verdi dopo commit, oppure il sistema deve fermarsi con un residuo tipizzato
   e riproducibile. Questo è il prossimo vero gate di “coding agent usabile”.

## 9. Registro chiuso dei round guidati

| Round | Incarico | Autonomia osservata | Gap tirato | Esito consolidato |
|---|---|---|---|---|
| 1 | diagnosticare L14 `relation_type` | legge il documento, ma non localizza né pianifica | T03 + T11/T12; collisione many-to-one nel consumer | consumer generalizzato; **11/11** focali e golden L14 verdi |
| 2 | pianificare ed eseguire un generico `code_task` | deriva 4 passi, crea Task/Goal/Action e rifiuta di simulare primitive assenti | T05 prima della scrittura; executor e action implementation | piano KB-first **8/8**, stato Brain atomico, agentkernel **34/34** |
| 2→3, integrità epistemica | caricare policy e procedure realmente dichiarate | prima 20 clausole AGI venivano scartate | limite rappresentazionale e stack ricorsivo | loader a **0 parse error**, multigoal **15/15**, factorial(15) verde anche sanitizzato |
| 2→3, artefatto | preparare una modifica verificabile senza toccare il workspace | batch edit/create/delete/rename isolato, persistibile e rivalidato | commit policy e checker rooted nel loop | T05 `PatchArtifact` **8/8**, transazione recuperabile con limiti dichiarati |
| 2→3, executor | eseguire un oracolo dentro la tree candidata | cwd risolta da rootfd e osservazione tipizzata | non è confinement; manca il collegamento al controller | `p0_exec_at` **14/14**, legacy toolexec **25/25** |
| 3 | recuperare da una prima patch volutamente errata | sceglie min-blast, osserva nuova diagnosi, ripianifica e chiude al terzo tentativo | adapter repository-general e bridge di commit | T06 live-rule/budget/ablation verde; repair legacy **9/9** |

Il registro separa deliberatamente ciò che il controller ha eseguito da ciò che
il supervisore ha implementato per tirare la facoltà successiva. Non attribuisce
a parrot0 localizzazione o commit autonomi che non ha ancora dimostrato.

## 10. Handoff operativo

### 10.1 Capacità disponibili al punto di consegna

- L'input HTTP non viene più troncato a 2000 byte: fino a 65 535 byte arriva
  intero; l'oversize è un 413 atomico con lunghezza, limite e digest (**7/7**).
- Il loader comprende clausole logiche multilinea, quote/escape/commenti,
  clausole multiple per riga e fatti zero-arity; una clausola invalida o fuori
  budget non viene caricata per prefisso. Il profilo AGI passa da 20 a 0 errori.
- Il Brain possiede il P0A store e mantiene stato tipizzato e `rec/4` nel KB come
  una sola transizione osservabile.
- Il piano `code_task` è conoscenza KB-first: `inspect -> localize -> edit ->
  verify`, con rischio, costo, tool ed effetti ablabili a runtime.
- `PatchArtifact` offre candidati persistenti anti-tamper con SHA-256,
  pre/postimage, mode, diff, preparazione isolata, policy callback, commit,
  post-check e rollback per edit/create/delete/rename.
- `p0_exec_at` esegue argv senza shell da una cwd risolta sotto una rootfd
  validata e restituisce `P0Obs` per exit, segnale, timeout e trace.
- Il controller T06 chiude observe/diagnose/propose/select/verify/replan entro
  budget, guidato da regole vive e con terminali/gap tipizzati.

### 10.2 Evidenza di verifica già raccolta

Al punto di handoff risultano verdi, eseguiti in serie:

```text
make build                         build senza warning
tests/answerframe.sh               11/11
tests/multigoal.sh                 15/15
tests/agentkernel.sh               34/34
tests/code-task-agent.sh            8/8
tests/patch-artifact.sh              8/8
tests/exec-dirfd.sh                 14/14
tests/agentrepair.sh               ratchet strutturale verde
tests/repair.sh                      9/9
tests/toolexec.sh                   25/25
tests/openai-input-limit.py          7/7
```

Il baseline conversazionale sigillato resta **180 pass / 73 fail**. Il gate
pre-push è confrontare il fail-set, non soltanto il totale, e lanciare gli
harness conversazionali in serie perché i loro temporanei interferiscono in
parallelo.

### 10.3 Limiti che il prossimo agente non deve reinterpretare come capacità

- Il controller T06 è generico, ma solo l'adapter sort è provato. Non sa ancora
  indicizzare una repository, localizzare il file o costruire una
  trasformazione da una issue aperta.
- L'adapter sort usa ancora una propria tree `.p0-sort-*`: non consegna il
  candidato al comune `p0_exec_at(rootfd,...)`, né al callback di policy e al
  commit canonico di `PatchArtifact`.
- `tool_for(build,cc)` governa davvero l'esecuzione, ma non esiste ancora una
  policy KB completa che autorizzi target, checker e commit. Nessuna
  autorizzazione va implicata dal solo esito verde.
- `p0_exec_at` rende la cwd una capability, non crea un `chroot`: un tool
  autorizzato può usare path assoluti o produrre side effect esterni.
- `PatchArtifact` è recuperabile, non crash-atomic. Restano finestre contro
  editor concorrenti tra revalidate e installazione/rollback; symlink, special
  file e touched hardlink sono rifiutati. Il checker può inoltre avere side
  effect fuori dai path toccati che l'artefatto non sa annullare.
- Il digest di de-duplicazione dello stato nel controller è un fingerprint
  FNV-1a a 16 cifre; lo SHA-256 autorevole appartiene al `PatchArtifact`. Per
  loop multi-file ostili va promosso anche il digest di post-stato.
- T07, T08, T11 e T12 sono aperti: ultimo messaggio user non equivale a history,
  trasporto lossless non equivale a Goal/Constraint, e lettura non equivale a
  localizzazione strutturale.

### 10.4 Prima slice da assegnare senza altra ricognizione

Il prossimo incarico deve essere il bridge
`PatchArtifact -> p0_exec_at -> P0Obs -> policy KB -> commit/rollback`, con
questi acceptance test minimi:

1. preparare e persistere un candidato single-file senza modificare il
   workspace canonico;
2. aprire la root candidata e svolgere build e run come due osservazioni
   distinte via `p0_exec_at`;
3. retrarre `tool_for` nella stessa sessione e ottenere missing-tool senza run
   sintetico né scritture canoniche;
4. con oracolo verde ma policy retratta, ottenere `Gap(policy)` e zero
   differenze nel workspace;
5. con policy presente, rivalidare la base, committare, ripetere l'oracolo sul
   canonico e registrare il verdetto;
6. forzare il post-check rosso e provare rollback byte/path/mode;
7. svolgere tutto senza literal linguistici nuovi in C: nomi di tool, regole e
   forme restano fatti KB ablabili.

Solo dopo questo ponte il round successivo deve combinare T08 e T11/T12 su una
issue single-file non-sort. Questo ordine evita di costruire un localizzatore
che sappia trovare modifiche ma non possa provarle e governarle in sicurezza.

### 10.5 Regola di ripartenza

Ogni nuovo round deve lasciare qui: incarico esatto, decisioni autonome
osservate, artefatti e `P0Obs`, primo controesempio, fatto KB aggiunto/retratto,
fail-set differenziale e residuo tipizzato. Se manca uno di questi elementi, il
round può aver aggiunto codice, ma non ha ancora dimostrato una nuova facoltà.

## 11. Handoff gen341 — ritaratura semantica, non fotografia dei bug

### 11.1 Diagnosi del debito

Il fail-set ereditato non è omogeneo. Il commit `b11373b` (`gen334`) introdusse
la canonicalizzazione IT→EN KB-first, centinaia di fatti e risposte localizzate,
ma non modificò alcun fixture. Questo rende obsolete molte rappresentazioni
attese; non rende però corretto ogni output nuovo.

Il triage riga-per-riga della baseline `gen340` ha classificato:

- 50 fixture `*.it.chat`, 148 mismatch: 14 golden-only sicure (37 righe), una
  condizionale, 12 miste e 23 regressioni core pure; 75 righe sono equivalenze
  dell'interlingua e 6 localizzazioni migliori, mentre 65 sono regressioni;
- 23 fixture senza suffisso `.it.chat`: 8 golden obsolete, 14 regressioni core
  e una mista (`priority.chat`).

Golden-only IT sicure: `cause`, `coref_prodrop`, `facts`, `memory_recall`,
`mixed`, `negation`, `possession_dinome`, `prosefact`, `rederive`, `repair`,
`robust`, `rules`, `teachverb`, `whatifnot`. Golden-only senza suffisso:
`apology`, `correction`, `lexicon_it`, `parrot`, `self`, `social`, `strategy`,
`teachverb`. `counterfactual.it` richiede prima di rendere linguistico il
verdetto interno; `priority` contiene insieme `gatto→cat` corretto e una
duplicazione di dispatch errata.

La regola di prosecuzione è: aggiornare solo equivalenze semantiche e
localizzazioni coerenti con `current_language`; non ratificare `Non capisco`,
fatti malformati, metadati interni, perdita di source text, duplicazioni o
routing verso una faculty più debole. Nomi propri, stringhe citate, frammenti
di sorgente, esempi few-shot e sequenze generative restano opachi anche quando
i concetti ordinari entrano nell'interlingua.

### 11.2 Due regressioni generali già chiuse

Senza cambiare un solo golden sono state corrette due cause ad alta leva:

1. `entity_mentioned/2` e `entity_seq_max/1` sono ora dichiarati
   `machinery/1`; la salienza riflessiva non compare più come proprietà del
   soggetto nelle risposte utente.
2. `mod_initiative` riconosce un loop soltanto se l'intero input è un'eco
   case-insensitive dell'ultima risposta. Il vecchio confronto sui primi 12
   byte rubava fatti distinti con lo stesso prefisso e rompeva i named world.

Build: zero warning. Harness conversazionale: **187 pass / 66 fail**, contro
**180/73** prima della correzione. Sono diventati verdi `belief`, `blankwall`,
`compose_coref`, `coref`, `gen_describe`, `proof_trace` e `world_stress` (fixture
non-IT), mostrando che erano regressioni e non golden da aggiornare.

### 11.3 Coda suite, in ordine di leva

1. Filtrare `kb_user_facts/predicates/dump` per origine utente e machinery;
   separare i fatti dimostrativi `progenitor/2` dal core conversazionale.
2. Passare raw-span distinti ai due rami di `decompose_and_dispatch`, invece
   dell'intero input a entrambi.
3. Preservare raw/source per generation, few-shot, strategy e translation;
   ripristinare gender e morfologia inversa come fatti KB non order-dependent.
4. Far consultare raw normalizzato ai consumer IT persi (meta, module tracking,
   pragma, role, user model), con assert/retract runtime dei cue.
5. Solo dopo questi fix applicare i golden-only sopra elencati e pretendere
   `tests/run.sh` completamente verde; il vecchio criterio del fail-set
   differenziale è ritirato per `gen341`.

### 11.4 Iterazione successiva progettata, non implementata

La ricognizione del bridge richiesto dalla sezione 10 è completa. Il seam minimo
è una callback PatchArtifact basata su `rootfd` borrowed e un
`p0_patch_check()` che rimaterializzi un artefatto persistito, verifichi la
postimage, esegua l'oracolo con `p0_exec_at`, riverifichi i touched path e
distrugga la stage. L'oracolo sort deve abbandonare `.p0-sort-*`; build e run
restano due `P0Obs`. Il commit richiede una policy KB per ogni operazione/path e
un post-check canonico; policy assente produce `Gap(policy)` e zero scritture,
post-check rosso produce rollback verificato. Un candidato verde non rende il
Task `done`: serve lo stato intermedio `verified_candidate` fino al commit.

Ratchet da costruire: artefatto repository-bound persistito/riletto; tool
ablation; policy ablation; commit verde con nuovo oracolo canonico; post-check
rosso con rollback byte/path/mode; fatti P0A e digest come assert, mai wording.
Restano invariati i limiti: `p0_exec_at` non è chroot, PatchArtifact non è
crash-atomic, output non-touched del checker non è coperto dal rollback, dedup
multi-file T06 usa ancora FNV64 e il P0A store non è durable.
