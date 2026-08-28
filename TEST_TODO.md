# TEST_TODO — le decisioni aperte della migrazione a `.p0t`

Coda delle cose che **non decido da solo** e di quelle che restano da fare nella
migrazione delle suite shell verso il test-engine
([`docs/plans/test-engine.md`](docs/plans/test-engine.md)).

Stato: **82/113 file sistemati (73%)** alla radice di `tests/`.

---

## 0. HANDOFF — da leggere per primo

### 0.1 ⚠️ `!expect` — il debito da saldare PRIMA di convertire altro

`!mcp` ed `!exec` mettono il loro risultato **dove va la risposta di parrot0**, e
si verificano con `<~` / `<!`. **È sbagliato**, e l'ha detto F.: `<` è
l'asserzione su *ciò che parrot0 ha risposto in un turno di conversazione*, e
l'uscita di una primitiva di test non è parrot0 che parla. Confonderle fa passare
per parola dell'agente quella che è uscita dello strumento.

**La forma decisa (F.): `!expect <sorgente> <testo>`.**

```
!mcp kb.query {"pred":"dog","args":["rex"]}
!expect mcp "provable":true

!exec make build
!expect exec exit 0
```

Due proprietà la rendono molto più di un `<~` rinominato, e vanno implementate
entrambe o non serve a niente:

1. **La sorgente è una GUARDIA, non decorazione.** Se l'ultima primitiva è stata
   `!exec` e il test scrive `!expect mcp`, il caso deve **fallire**. È un errore
   del test — la stessa classe di errore che questa migrazione ha prodotto più
   volte: asserire la cosa giusta sull'output sbagliato. Senza questo controllo
   `!expect` è solo `<~` con una parola in più.
2. **`<` deve RIFIUTARSI di asserire dopo una primitiva.** Se l'ultimo output non
   viene da un turno, `<` deve dire «questo non è parrot0 che parla». Altrimenti
   la forma vecchia continua a funzionare in silenzio e il debito resta soltanto
   scoraggiato, mai chiuso.

Con entrambe, la coppia diventa una **tipizzazione**: `<` è il canale
conversazionale, `!expect <sorgente>` quello degli strumenti, e mescolarli è un
errore *rilevato* invece che una svista.

Servono le tre varianti che `<` ha già: contiene (`!expect`), non contiene
(`!expect!`), uguale esatto — e la sorgente da riconoscere è almeno `mcp` ed
`exec`.

Poi vanno aggiornati i file già convertiti che usano `<~` dopo `!mcp`:

`p0t/mcp/*.p0t` (6), `p0t/growth/*.p0t` (4), `p0t/engine/naf.p0t`,
`p0t/engine/dif.p0t`, `p0t/engine/dollarvar.p0t`, `p0t/lang/*.p0t` (3),
`p0t/code/check_sort.p0t`, `p0t/input/universal-input.p0t`,
`p0t/tools/toolexec.p0t` (solo la parte `!mcp`), `p0t/save/savemap.p0t`.

Va fatto **prima** di convertire altro, o il debito cresce con ogni file nuovo.

### 0.2 Come si riprende

1. `bash <(echo 'ls -1 tests/*.sh tests/*.py 2>/dev/null | wc -l')` dice quanti
   file restano alla radice (erano 113 all'inizio).
2. Si sceglie un file dalla §2, si legge la sua forma di asserzione, si converte
   **a mano**, e si verifica con il criterio del §3: *stesso risultato dello
   script, verde o rosso che sia*.
3. Si cancella lo script, si toglie la sua riga dal `Makefile`, si aggiunge il
   `.p0t` a `make test` **solo se è verde**.

### 0.3 Che cosa è già stato fatto

- **`tests/probes/`** (12), **`tests/bench/`** (22), **`tests/tools/`** (12),
  **`tests/cdriver/`** (2) — spostamenti, non conversioni.
- **31 suite convertite** in `tests/p0t/{engine,mcp,expert,growth,lang,code,input,tools,proof,repair,save,oracle}/`.
- **Primitive nuove nel test-engine**: `!mcp`, `!sandbox`, `!symlink`, `!exec`,
  `!fileexists`/`!filemissing`, `!direxists`/`!dirmissing`,
  `!filehas`/`!filelacks`, `!fileclean`.
- **Riparazioni al motore trovate convertendo**: `mcp_tool_invoke` non
  raggiungeva `input.segment`; la risposta del test-engine era di 4 KB e
  troncava i payload; `!sandbox` rompeva il caricamento della KB (percorsi
  relativi); `PARROT0_ORACLE` era letto con `getenv` invece che con `p0env`,
  quindi il test-engine non poteva accenderlo.

### 0.4 La regola che F. ha posto, e che vale su tutto

**Mai più test con una KB vuota o vergine.** La KB è parte di ciò che si testa;
le variabili che la ri-basano per farla apparire vuota perdono di significato.
Nei test nuovi si usano **entità inventate** sulla KB reale, non l'amputazione.

Le conversioni fatte prima di questa regola ricopiano l'amputazione dagli script
(`!set PARROT0_BASE=`, `PARROT0_PROFILE=`, `PARROT0_WORLD_FACTS=0`) e **vanno
ripassate**. `p0t/oracle/posix.p0t` è il primo fatto con il criterio giusto — e
convertendolo è saltato fuori che `kb/experts/programming/bash.p0` è un
**duplicato orfano** di `shell.p0`, incluso da nessuno: il test passava solo
perché lo montava a mano.

---

## 1. Decisioni che aspettano una risposta

### 1.1 Esporre l'esecutore su MCP? — *bloccante per 2 file*

`tests/cdriver/exec_kernel.sh` e `tests/cdriver/exec_dirfd.sh` restano script
perché provano `p0_exec` e `p0_exec_at` **direttamente**: timeout tipizzato,
uccisione del gruppo di processi senza orfani, `spawn_failed`, cwd fuori dal
workspace, e per il secondo l'ancoraggio a un descrittore di directory.

Per convertirli servirebbe uno strumento MCP che esegue argv arbitrari — e per
`p0_exec_at` che accetta anche un **fd di directory**. È una decisione di
sicurezza: darebbe quel permesso a *qualunque* client MCP, non solo ai test.

Il precedente opposto è già preso: `code.check_sort` è esposto perché è un
**giudice** su un sorgente — dispone un candidato, non esegue ciò che gli si
chiede.

- **se sì** → due strumenti nuovi, i due file diventano `.p0t`, `tests/cdriver/`
  sparisce;
- **se no** → restano dove sono, e la categoria è definitiva.

### 1.2 `learnbuild`: il test-engine e il binario non concordano

Lo **stesso identico prompt** riceve due risposte diverse:

| | risposta a `write a vunder function` |
|---|---|
| binario | «I can only synthesize and VERIFY the sum, product, or difference of two integers so far — I will not emit code I cannot check.» |
| test-engine | «I understood the request … but I don't have a verified schema for that artifact yet; I only synthesize what an oracle can check (a sort from a learned shape, arithmetic composition, a count-to-threshold game).» |

Riprodotto su un demone appena avviato, stesso env, senza sandbox. Sono due
declini entrambi onesti, ma **non sono la stessa frase**: qualcosa fa vincere un
modulo diverso sotto `--test-engine`.

Finché non si sa perché, una conversione sarebbe verde per caso o rossa per la
ragione sbagliata. Va indagato prima di migrare `learnbuild.sh` — e il difetto,
se c'è, non è del test: è di uno dei due percorsi.

### 1.3 `autolearn` legge file che non esistono più

`tests/tools/autolearn.py` e `scripts/learn.py` leggono `kb/learning/sources.tsv`,
`state.json`, `index.json` e scrivono in `logs/` — tutti rimossi quando
`kb/learning/` è stato ridotto ai soli `.p0`. Il target `make autolearn` passa
ledger e skip-list, anch'essi rimossi.

Quei due flussi sono **rotti**. Vanno rifatti o ritirati; il contenuto è
recuperabile da `git log --all`.

### 1.4 Le suite convertite ma ROSSE non entrano in `make test`

`make test` è fail-fast: una suite rossa fermerebbe tutte le successive. Queste
sono convertite fedelmente (stesso risultato dello script) ma restano fuori:

| file | stato |
|---|---|
| `p0t/expert/grammar.p0t` | 13 passed, 1 failed |
| `p0t/expert/knowledge.p0t` | 9 passed, 13 failed |
| `p0t/expert/profiles.p0t` | 9 passed, 4 failed |
| `p0t/expert/synth.p0t` | 6 passed, 1 failed |
| `p0t/expert/skills.p0t` | 5 passed, 1 failed |
| `p0t/lang/article.p0t` | 4 passed, 3 failed |
| `p0t/lang/adjagree.p0t` | 3 passed, 2 failed |
| `p0t/lang/vmorph.p0t` | 4 passed, 1 failed |
| `p0t/tools/toolexec.p0t` | 21 passed, 1 failed |

Serve una casa: un target `make test-red` che li esegue senza fermare la build,
oppure la scelta di chiuderne il debito prima di cablarli. Molti di questi
fallimenti sono **M0** — vedi `LEARN_TODO.md` P0.4.

### 1.5 `savemap` chiede due primitive nuove per un solo test

`tests/savemap.sh` prova il contratto centrale del save-map — *un fatto appreso
finisce accanto ai suoi simili* — e per farlo costruisce un albero `.p0` finto,
lo carica come `PARROT0_BASE`/`PARROT0_KB_ROOT`, salva, e poi **verifica in quale
file** ogni fatto è atterrato.

In `.p0t` mancano due cose, entrambe di filesystem:

- **scrivere un file di fixture** (l'albero finto, con i suoi `include`);
- **asserire il contenuto di un file** — qualcosa come `!filehas PATH pattern`.

Non le aggiungo da solo: sono due primitive di DSL per una suite sola, e la
seconda apre la porta a test che guardano il disco invece del comportamento.
L'alternativa — far scrivere il `/save` nell'albero VERO durante i test — è
peggio.

Il contratto però è importante, ed è quello che ho esercitato a mano tutta la
sessione con lo sparpagliamento della ricaduta.

### 1.6 I prompt di `basic-chat` restano in un file di piano

`tests/bench/basicchat.sh` misura la copertura leggendo i prompt da
`docs/plans/basic-chat.md`. Il TODO nella sua testa dice di portare in `.p0t` i
prompt rappresentabili e lasciare lì solo l'adapter di misura.

È un lavoro a sé, e si somma bene alla collezione `docs/llmscores/`.

---

## 2. Da migrare, senza decisioni aperte

Restano **36 file** alla radice di `tests/`. La conversione automatica non regge
su nessuno di questi: ognuno ha la propria forma di asserzione, e un
convertitore che non la riconosce produce un file **verde perché vuoto**, che è
peggio di rosso.

### 2.1 Conversione automatica tentata e SCARTATA

Generate e buttate perché non riproducevano l'originale — vanno rifatte a mano:

`answerframe` (6/14 contro 14/0), `aggregate` (perdeva metà asserzioni),
`assertclause`, `mcp-teach`, `model_graph`, `motorize_class`, `cliticfr`,
`explain` (1/4 contro 2/3), `posix_oracle` (0/4 contro 3/1),
`enumerate` (2/10 contro 3/12), `meta_ceiling` (2/3 contro 5/0).

### 2.2 Ibride: conversazione + driver C

`multigoal` (3 turni conversazionali + 12 controlli sul caricatore scritti come
driver C), `kb-evidence-scale`, `agentkernel`. La parte C va in
`tests/cdriver/`, il resto in `.p0t` — due file da uno.

### 2.3 Grosse, da leggere prima di toccarle

`reasoning_operators` (548 righe), `patch-artifact` (399), `patch-check` (353).
Pilotano il ciclo candidato → oracolo → policy → commit.

### 2.4 Il resto

`agentcommit`, `agentrepair`, `archetype`, `artfres`, `booklearn`, `buildstamp`,
`code-task-agent`, `experts`, `impersonate`(già spostata), `mcp`,
`mcp-input-payload`, `persist`, `repair`, `research_learn`, `restore`, `savemap`,
`segment`, `selflimits`, `simclean`(già spostata), `strknow`(fatta), `syllogism`,
`wiki_learning`.

---

## 3. Il metodo, che vale per ogni voce qui

1. **L'equivalenza, non il verde.** Il criterio non è «il `.p0t` passa» ma «il
   `.p0t` dà lo stesso risultato dello script, verde o rosso che sia». Metà di
   queste suite era già rossa, e un file che diventa verde convertendolo ha
   perso delle asserzioni.
2. **Non si perdono i casi che falliscono.** Due volte la mia conversione ha
   omesso proprio quelli, risultando verde. Rimetterli fa tornare il file rosso,
   ed è giusto: *un test che lascia fuori il caso che fallisce è un test che
   mente*.
3. **Le negazioni si guardano.** `! lN id | grep -q 'x'` è un'asserzione di
   **assenza**: tradotta come presenza, il test è verde e sbagliato.
4. **Gli id si ripetono fra blocchi.** Ogni blocco ha il suo `l1`/`l2`; cercare
   «id 2» su tutto il file raccoglie asserzioni di blocchi diversi.
5. **Le asserzioni guardano spesso il formato di TRASPORTO.** Il payload viaggia
   come stringa JSON dentro l'envelope JSON-RPC, con le virgolette protette;
   `!mcp` restituisce il payload grezzo. I backslash vanno tolti.
6. **Lo script cancella, non archivia.** Uno script che resta accanto alla sua
   conversione è un doppione che fa crescere male la suite.
