# Le variabili d'ambiente di parrot0

Censimento completo, al `gen449`. Ricavato leggendo il sorgente, non la
memoria: ogni voce cita il file e la riga che la consuma, cosi' che quando una
riga si sposta si veda subito che questa pagina e' invecchiata.

**Totale: 32 nomi letti dal binario**, piu' 5 usati dagli strumenti di test e
di build, piu' 1 che parrot0 *scrive* (non legge) nell'ambiente dei processi
figli.

---

## 0. Come vanno lette — lo strato `p0env`

Non tutte le variabili si leggono allo stesso modo, e la differenza conta.

`src/env.c` e' uno **strato di configurazione runtime**: `p0env(name)`
restituisce prima un eventuale *override* impostato a caldo, e solo dopo
l'ambiente del processo. Serve al motore di test, che con `!set NOME=VALORE`
dentro un `.p0t` pilota la configurazione **senza rilanciare il processo**.

Da cui tre categorie, che si vedono nelle tabelle sotto nella colonna «letta
via»:

| letta via | significa |
|---|---|
| `p0env` | pilotabile da un `.p0t` con `!set` |
| `getenv` | **non** pilotabile: si legge l'ambiente del processo e basta |

E una seconda distinzione, la colonna «boot»:

- **boot = si'** — la variabile e' consumata *mentre il cervello nasce*
  (decide quali file di conoscenza si caricano, quale politica vale, quale
  lingua). Cambiarla significa che la conoscenza caricata va riderivata: il
  motore di test **ricarica il cervello**, ma solo se il valore effettivo e'
  davvero cambiato (`p0env_mem_signature`, `src/env.c:86`).
- **boot = no** — si legge al momento dell'uso, quindi ha effetto dalla
  lettura successiva senza nessuna ricarica.

L'elenco autorevole delle variabili «boot» e' `MEMORY_VARS` in
`src/env.c:22`.

> ⚠️ Alcune variabili sono lette con `getenv` diretto pur essendo di
> configurazione (`PARROT0_TOOLS` in `60-agent-tools.c:1192`,
> `PARROT0_DEEP_BUDGET`, `PARROT0_WIKI_DIR` in `50-self-research-loop.c:284`).
> E' un'incoerenza reale, non una scelta: quelle porte restano chiuse a chi le
> prova da un `.p0t`. La lezione e' gia' scritta nel codice a
> `65-induce-verify-shell.c:364` — *«un permesso che non si vede e' peggio di
> un permesso negato»* — e la stessa nota vale qui.

---

## 1. Quale conoscenza si carica

Sono le variabili che decidono **che cosa parrot0 sa** all'avvio. Tutte
`p0env`, tutte boot: sono la definizione stessa dell'istanza.

| variabile | default | letta via | boot | a che serve |
|---|---|---|:--:|---|
| `PARROT0_BASE` | `kb/core/base.p0` | `p0env` | si' | Il file d'ingresso della conoscenza. `99-registry.c:1109` |
| `PARROT0_PROFILE` | *(nessuno; `--daemon` mette `kb/profiles/agi.p0`)* | `p0env` | si' | Profilo caricato sopra la base. `99-registry.c:1110`, `main.c:939` |
| `PARROT0_LEXICON` | `kb/core/lexicon.p0` | `p0env` | si' | Il lessico curato del kernel. **Stringa vuota = non caricarlo.** `99-registry.c:670` |
| `PARROT0_WORLD_FACTS` | *caricato* | `p0env` | si' | `=0` non carica `kb/core/world-facts.p0`. `99-registry.c:779` |
| `PARROT0_KB_ROOT` | `kb` | `p0env` | si' | Radice dell'albero in cui `/save` instrada cio' che si e' imparato. `99-registry.c:910` |

Esempi:

```bash
# il caso normale: chat con il profilo completo
PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0

# un cervello NUDO, per dimostrare che una cosa si impara e non e' precaricata
PARROT0_WORLD_FACTS=0 PARROT0_LEXICON= ./bin/parrot0

# dentro un .p0t, senza rilanciare il processo
!set PARROT0_WORLD_FACTS=0
!reset
```

> La riga `PARROT0_WORLD_FACTS=0` e' quella che rende onesta una prova di
> apprendimento: senza, non si distingue cio' che parrot0 ha imparato da cio'
> che gli era stato messo in tasca.

---

## 2. Politica e capacita' — che cosa gli e' permesso fare

Due variabili, e insieme decidono la **modalita'** che il banner annuncia.
Sorgente unica in `brain_policy()`, `99-registry.c:1065`.

| variabile | default | letta via | boot | a che serve |
|---|---|---|:--:|---|
| `PARROT0_TOOLS` | *off* | `p0env` **e** `getenv` | si' | `=1` permette filesystem e comandi. `99-registry.c:1067`, `60-agent-tools.c:1192` |
| `PARROT0_WIKI_FETCH` | *off* | `p0env` | si' | `=1` permette di andare in rete. `99-registry.c:1068`, `learn.c:273` |

La combinazione determina `policy(mode, …)`:

| `PARROT0_TOOLS` | `PARROT0_WIKI_FETCH` | modalita' annunciata |
|:--:|:--:|---|
| — | — | `conversational` |
| `1` | — | `agent` |
| `1` | `1` | `acquire` |
| — | `1` | `acquire` ⚠️ |

L'ultima riga e' verificata, non dedotta: `brain_policy` calcola la modalita'
come `net ? "acquire" : (tools ? "agent" : "conversational")`, quindi **la rete
da sola basta per annunciare `acquire`** anche con gli strumenti spenti. Detto
in chat, «acquire» promette piu' di quel che quella configurazione puo' fare —
e' proprio la cosa che il commento sopra `brain_policy` dice di non voler far
succedere. Non e' stato toccato qui perche' cambiare una modalita' annunciata
non e' lavoro di documentazione; e' segnalato e basta.

```bash
make chat        # = PARROT0_WIKI_FETCH=1 PARROT0_TOOLS=1 → mode: acquire
PARROT0_TOOLS=1 ./bin/parrot0          # mode: agent, niente rete
./bin/parrot0                          # mode: conversational
```

> Una sola sorgente di verita' perche' **il banner non possa promettere cio'
> che un declino nega** (`99-registry.c:1064`). Se si aggiunge una capacita',
> va aggiunta qui, non nel modulo che la usa.

---

## 3. La lingua

| variabile | default | letta via | boot | a che serve |
|---|---|---|:--:|---|
| `PARROT0_LANG` | — | `p0env` | si' | Lingua della sessione. Vince su tutte. `99-registry.c:823` |
| `LANG` | *(dal sistema)* | `p0env` | si' | Primo ripiego. `99-registry.c:824` |
| `LC_ALL` | *(dal sistema)* | `p0env` | si' | Secondo ripiego. `99-registry.c:825` |
| `LC_MESSAGES` | *(dal sistema)* | `p0env` | si' | Terzo ripiego. `99-registry.c:826` |

L'ordine e' esattamente quello: si prende il primo valore non vuoto e se ne
tiene il prefisso fino a `_`, `-` o `.` — quindi `it_IT.UTF-8` diventa `it`.

```bash
PARROT0_LANG=it ./bin/parrot0
PARROT0_LANG=en ./bin/parrot0        # forza l'inglese su una macchina italiana
```

> La lingua **scelta qui** e' quella con cui `kb_response_slots` cerca un
> `response_template/3`. Se la famiglia non ha la riga in quella lingua si
> ricade sulla `/2`, cioe' sull'inglese — ed e' la ragione del difetto
> descritto in `C_TODO.md` §7.2: il meccanismo funziona, e' la copertura che
> manca (137 famiglie su 829).

---

## 4. Dove parrot0 scrive

Nessuna di queste e' «boot»: si leggono quando serve scrivere.

| variabile | default | letta via | a che serve |
|---|---|---|---|
| `PARROT0_SESSION_DUMP` | `$PARROT0_RUNTIME_DIR/parrot0-session-<pid>.p0` | `p0env` | Fotografia leggibile dello stato di runtime. Non viene **mai** riletta. `99-registry.c:925` |
| `PARROT0_RUNTIME_DIR` | `/tmp` | `p0env` | Cartella degli artefatti di runtime. `99-registry.c:928` |
| `PARROT0_SESSION_FALLBACK` | `kb/learning/learned.p0` | `p0env` | Ricaduta per cio' che l'instradamento non sa dove mettere. `main.c:198` |
| `PARROT0_GAPS` | `kb/learning/gaps.p0` | `p0env` | Dove si salvano i `machinery_gap` — le lacune che parrot0 si e' visto addosso. `99-registry.c:3230` |
| `PARROT0_BRIDGES` | `kb/learning/bridges.p0` | `p0env` | I ponti **indotti** dal ciclo, tenuti separati da cio' che una persona ha deciso. `99-registry.c:3276` |

```bash
# due parrot0 sulla stessa macchina non si sovrascrivono: il PID e' nel nome
./bin/parrot0 &
./bin/parrot0 &

# ma se lo si vuole in un posto preciso:
PARROT0_SESSION_DUMP=/tmp/mio-dump.p0 ./bin/parrot0
```

> Il dump ha il PID nel nome **di proposito**. Prima esisteva un default fisso
> condiviso, ed era insieme sorgente e destinazione: due istanze si
> sovrascrivevano a vicenda e cio' che si salvava rientrava dalla porta del
> boot (`99-registry.c:915`).

---

## 5. Acquisizione — leggere e imparare

| variabile | default | letta via | a che serve |
|---|---|---|---|
| `PARROT0_WIKI_DIR` | `kb/learning/pages` | `p0env` **e** `getenv` | Cartella delle pagine locali (`<chiave>.md`). `learn.c:42`, `50-self-research-loop.c:284` |
| `PARROT0_LEARN_KB` | *(nessuno = non persiste)* | `p0env` | File in cui accodare i `wiki_concept` imparati. **Assente nei test**, cosi' la suite ermetica non scrive niente. `learn.c:91` |
| `PARROT0_DREAM_KB` | `kb/learning/learned.p0` | `p0env` | Destinazione di cio' che `--dream` ha imparato. `dream.c:440` |
| `PARROT0_DEEP_BUDGET` | `60` (secondi) | `getenv` | Tetto di tempo del ragionamento profondo. `50-self-research-loop.c:676` |
| `PARROT0_ORACLE` | *off* | `p0env` | `=1` abilita l'oracolo di shell che dispone le ipotesi eseguendole. `65-induce-verify-shell.c:368` |

```bash
# leggere da pagine locali, senza rete
PARROT0_WIKI_DIR=./mie-pagine ./bin/parrot0

# un sogno che persiste cio' che ha imparato
PARROT0_DREAM_KB=kb/learning/sogno.p0 ./bin/parrot0 --dream "vulcani" --persist

# ragionamento profondo con piu' respiro
PARROT0_DEEP_BUDGET=300 ./bin/parrot0
```

> `--dream` **forza** `PARROT0_WIKI_FETCH=1` se non e' impostata
> (`dream.c:370`): un sogno su un argomento acquisisce la prosa direttamente in
> RAM, e non ha un ripiego locale.

---

## 6. La superficie di chat

| variabile | default | letta via | a che serve |
|---|---|---|---|
| `PARROT0_MULTILINE` | *off* | `getenv` | `=1` (e stdin e' un TTY) attiva il lettore multi-riga: Shift+Enter, incolla, `\` finale. `main.c:1011` |
| `PARROT0_EOT` | *off* | `getenv` | Stampa la riga indicata dopo ogni turno, per i driver che accoppiano una riga di stdout per turno. `main.c:1135` |

```bash
PARROT0_MULTILINE=1 make chat
PARROT0_EOT='<<<END>>>' ./bin/parrot0 < turni.txt
```

> Il default e' il lettore semplice **di proposito**: il lettore multi-riga
> inciampa su un `<` in certi terminali. E l'input da pipe usa sempre il
> percorso semplice, cosi' la suite resta identica byte per byte
> (`main.c:1005`).
>
> Nota: nessuna delle due da' la history con le frecce su/giu' — e' un TODO
> aperto, `C_TODO.md` §7.1.

---

## 7. Diagnostica e tracciamento

| variabile | default | letta via | a che serve |
|---|---|---|---|
| `PARROT0_TRACE` | *off* | `getenv` | Percorso di un log JSONL delle osservazioni (comandi eseguiti e verdetti). `exec.c:521`, `main.c:88` |
| `PARROT0_ROBUST_DEBUG` | *off* | `getenv` | Qualunque valore: traccia le iterazioni del modulo `robust`. `90-repair-robust-abduce.c:462` |
| `PARROT0_PI_LOG` | *off* | `getenv` | Percorso del log del traffico del server HTTP. `serve.c:46` |
| `PARROT0_PI_INCLUDE_SYSTEM` | *off* | `getenv` | `=1` antepone il prompt di sistema al turno dell'utente, in modalita' server. `serve.c:119` |

```bash
PARROT0_TRACE=/tmp/parrot0-trace.jsonl PARROT0_TOOLS=1 ./bin/parrot0
PARROT0_ROBUST_DEBUG=1 ./bin/parrot0 --test tests/p0t/repair/robust.p0t
```

---

## 8. Il motore di test

| variabile | default | letta via | a che serve |
|---|---|---|---|
| `PARROT0_TE_DEBUG` | *off* | `getenv` | Qualunque valore: diagnostica del demone `.p0t`. `testeng.c:237` |
| `PARROT0_TE_SLOW` | `0` (spento) | `getenv` | Soglia in secondi: ogni caso piu' lento finisce su stderr col suo tempo. `testeng.c:425` |
| `PARROT0_PID` | *(il vero `getpid()`)* | `p0env` | **si' (boot)** — congela il PID, per una risposta deterministica. `99-registry.c:819` |
| `TMPDIR` | `/tmp` | `getenv` | Dove nasce la sandbox dei casi `.p0t`. `testeng.c:325` |

```bash
# dopo qualunque modifica alla dimensione della KB o al percorso di risoluzione
PARROT0_TE_SLOW=0.2 make test
```

---

## 9. Usate dagli strumenti, non dal binario

Non le legge `parrot0`: le leggono gli script di test e di build.

| variabile | default | dove | a che serve |
|---|---|---|---|
| `PARROT0_TEST_JOBS` | `nproc` | `tests/tools/run.sh:46` | Parallelismo della suite. `=1` per il comportamento seriale storico. |
| `PARROT0_CONTRACTS` | — | `tests/tools/checkfocal.sh:79` | Catalogo di fixture per il controllo dei contratti. |
| `PARROT0_MANIFEST` | — | `tests/tools/manifest_audit.py:40` | Manifest di fixture per l'audit. |
| `PARROT0_MCP_DIR` | `/tmp/parrot0-mcp` | `scripts/mcp-live.sh:26` | Cartella di stato (fifo, log, pid) del ponte MCP. |

```bash
PARROT0_TEST_JOBS=1 make test          # seriale, per leggere l'output in ordine
```

---

## 10. Scritta da parrot0, non letta

| variabile | dove | a che serve |
|---|---|---|
| `PARROT0_SANDBOX=1` | `exec.c:377` | parrot0 la **mette** nell'ambiente di ogni processo figlio, cosi' che un comando possa accorgersi di essere stato lanciato dal sandbox. Il binario non la rilegge mai. |

L'ambiente del figlio e' ripulito e vale la pena guardarlo per intero
(`exec.c:369-379`): `PATH`, `HOME`, `LANG=C`, `LC_ALL=C`, `PARROT0_SANDBOX=1`,
e nient'altro.

> `LANG=C` non e' cosmetico. Al gen328 un `make` fallito si riportava in
> italiano, e nessun parser di verdetti sapeva leggerlo: **un'osservazione
> dev'essere in una lingua che l'osservatore conosce.**

`HOME` e' l'unica variabile di sistema che parrot0 legge (`exec.c:371`) per
poi riscriverla nell'ambiente del figlio; se manca, il figlio riceve `/tmp`.

---

## 11. Obsoleta ma ancora onorata

| variabile | stato |
|---|---|
| `PARROT0_SESSION` | **Obsoleta dal gen382g.** La sessione non e' piu' un input e il boot non la carica. Resta nell'elenco `MEMORY_VARS` (`src/env.c:29`) solo perche' centinaia di test la impostano e cambiarne il valore deve continuare a contare come «la configurazione si e' mossa», su cui si appoggia il reset intelligente. Per le sostitute vedi §4. |

---

## 12. Nomi che compaiono nei documenti ma non esistono

Citati in `docs/plans/mcp-engine.md` come proposta, **mai implementati**. Sono
elencati qui perche' cercarli nel sorgente e non trovarli e' una perdita di
tempo che capita una volta a testa:

`PARROT0_BARE`, `PARROT0_GLOSS`, `PARROT0_GLUE`, `PARROT0_INTENTS`,
`PARROT0_RESPONSES`, `PARROT0_ROLES`, `PARROT0_SOCIAL`,
`PARROT0_RESEARCH_QUEUE`.

Da non confondere con le **macro di compilazione**, che hanno lo stesso
prefisso ma non sono variabili d'ambiente: `PARROT0_GEN`, `PARROT0_COMMIT`
(generate in `obj/version_stamp.h` dal file `VERSION`),
`PARROT0_HAVE_CURL`, e tutte le guardie di header `PARROT0_*_H`.

---

## Come rifare questo censimento

Perche' la prossima persona non debba fidarsi di questa pagina:

```bash
# 1. i nomi letti dal binario
grep -rhoE '(getenv|p0env|p0env_set)\s*\(\s*"[A-Z0-9_]+"' --include=*.c --include=*.h src/ \
  | grep -oE '"[A-Z0-9_]+"' | tr -d '"' | sort -u

# 2. tutto cio' che PORTA il prefisso, ovunque (per trovare i nomi solo-documentati)
grep -rhoE '\bPARROT0_[A-Z0-9_]+' src/ Makefile scripts/ tests/ docs/ *.md kb/ | sort -u

# 3. quali sono consumate al boot (la lista autorevole)
sed -n '/MEMORY_VARS\[\]/,/NULL/p' src/env.c
```

Il primo comando deve dare 32 nomi. Se ne da' di piu', questa pagina e'
vecchia.
