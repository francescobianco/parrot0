# TEST_TODO — le decisioni aperte della migrazione a `.p0t`

Coda delle cose che **non decido da solo** e di quelle che restano da fare nella
migrazione delle suite shell verso il test-engine
([`docs/plans/test-engine.md`](docs/plans/test-engine.md)).

Stato: **77/113 file sistemati (68%)** alla radice di `tests/`.

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

### 1.5 I prompt di `basic-chat` restano in un file di piano

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
