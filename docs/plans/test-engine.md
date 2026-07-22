# The test-engine — one live instance validates `.p0t` suites

> **Stato:** gen346. `make test` = **1357 passed, 0 failed** (~44s). Migrati anche
> gli 83/91 casi `.it` (40 as-is + 43 allineati) dopo le fix lingua A/D/E (vedi
> sotto). **Restano 23 `.chat`, TUTTI perf-held** (15 EN + 8 IT): calibrate\*,
> conjunction, deepreason\*, proof_trace, robust, compose, fewshot, \*_stress,
> reflexive_selftest\*/audit — un turno supera il budget `!timeout` di 1s (causa
> comune pre-esistente: lo sweep di ablazione O(n) di `mod_robust`). Vanno
> migrati DOPO aver ottimizzato lo sweep, o con `!timeout N` per-test.
>
> **Fix lingua fatte (decisione F. = ibrido):** A) l'input ambiguo ("ciao")
> deferisce al seed `PARROT0_LANG` (tolto `language_marker(it, ciao)`); D) le
> risposte italiane usano il termine italiano (fratello, non brother — ritradotto
> via `tr/2`); E) `canonicalize_lang` fa lo strip di QUALSIASI punteggiatura finale
> (non solo `?`), così le parole prima di un punto in una frase multipla italiana
> si canonicalizzano (syllogism.it → Yes). B/C (predicati salvati e messaggi
> d'errore anglicizzati) accettati e allineati nei test. Il caso ambiguo/lingua è
> blindato da `tests/p0t/language/language.p0t` (6/6).
>
> NOTA: i 43 casi `.it` **allineati** catturano il comportamento corrente via
> snapshot — vanno riguardati (potrebbero enshrinare qualche forma anglicizzata
> B/C come "attesa").
>
> [Stato storico:] gen346. `make test` era 1034. `parrot0
> --test-engine` è un demone su socket Unix che tiene UN brain vivo; i file `.p0t`
> gli si mandano con `--test-send`. Il vecchio harness è `make legacy-test` (file
> `.chat` in `tests/cases/`, girati da `tests/run.sh`).
> **~148 casi migrati** in `tests/p0t/<categoria>/`. **106 `.chat` restano**, e si
> dividono in due gruppi che richiedono una DECISIONE prima di procedere:
> - **91 `.it`** → dipendono dalla semantica della lingua (matrice
>   `tests/p0t/language/language.p0t`, canonicalizzazione IT→EN gen334). Da
>   sciogliere con F. prima di migrarli.
> - **15 perf-held** (calibrate\*, conjunction, deepreason\*, proof_trace, robust,
>   compose, \*_stress, reflexive_selftest\*/audit) → un turno supera 1s e il
>   `!timeout` li blocca. Causa comune PRE-ESISTENTE: `mod_robust`
>   (`90-repair-robust-abduce.c`) fa uno sweep di ablazione O(fatti-unari ×
>   ri-derivazione); e il ragionamento profondo. Opzioni: (a) ottimizzare lo sweep,
>   (b) migrarli con `!timeout N` per-test (costo sul wall di `make test`).
>
> La sezione **§9 HANDOFF** ha tutto il necessario per riprendere.

---

## 0. Perché

Il vecchio harness avviava **un processo per caso** e ricaricava tutta la KB ogni
volta — quasi tutto il costo era startup ripetuto, non inferenza. Il test-engine
avvia **una** istanza e le manda tutti i file: la KB si carica una volta sola.

## 1. Avvio — demone + client

```
parrot0 --test-engine  [--sock PATH]   # DEMONE: un brain, in ascolto sul socket
parrot0 --test-send FILE [--sock PATH] # CLIENT: manda un file .p0t al demone
parrot0 --test-report    [--sock PATH] # CLIENT: chiede il totale e ferma il demone
```

Il **client non carica nulla** (né brain né KB): è solo un relay sul socket, così
ogni riga di `make test` costa quasi zero. Il demone tiene UN brain; lo stato
(conoscenza insegnata, lingua, ecc.) **persiste tra i file** salvo `!reset`. Ogni
connessione riparte però dall'**ambiente di default** (gli override `!set` sono
azzerati a inizio file), così un file ermetico non "sporca" il successivo. Socket
di default: `obj/test-engine.sock`.

**Report per-file:** ogni `--test-send` stampa UNA riga — `ok NOME — N passed` —
oppure, se qualcosa fallisce, prima il dettaglio (`FAIL [sezione] linea`,
`expected:`/`got:`) e poi `FAIL NOME — N passed, M failed`, uscendo con `1`
(fail-fast: `make` si ferma lì). `--test-report` chiude il demone e stampa
`total: N passed, M failed`, uscendo `1` se qualcosa è fallito nell'intera
sessione.

## 2. La sintassi `.p0t`

```
# commento                  riga di commento (anche vuote) — ignorata
[test NOME]                 apre una sezione con nome (per il report)

> testo                     invia `testo` a parrot0 come un turno utente
< testo                     asserisce che la risposta è `testo`

!set NAME=VALUE             pilota una variabile di runtime (env.h): PARROT0_BASE,
                            PARROT0_WORLD_FACTS, PARROT0_LANG, PARROT0_ORACLE,
                            HOME, PARROT0_PID, …
!reload                     applica ora un cambio di config (no-op se non è cambiato)
!reset                      brain vergine con la config corrente (isolamento, opt-in)
!timeout SECONDI            budget per-test di un turno (default 1s, reset a ogni
                            [test]); un turno più lento è un FAIL (guardia anti-
                            regressione di performance); 0 lo disabilita
```

- **Multi-turno:** più coppie `> / <` in una sezione girano in ordine sullo stesso
  brain (coreferenza e stato tra turni testabili).
- **Multi-linea:** un `<` per riga di output; i `<` consecutivi si confrontano con
  l'intera risposta multi-riga.
- `!shutdown` è l'unica riga di controllo interna (la manda `--test-report`), non
  una primitiva di test.

**Ancora da progettare (F.):** mock, stub, flag e altre forme di controllo dello
stato della KB. Per ora l'engine fa solo l'assert atteso + il pilotaggio env.

## 3. `!set` e il modello di reload/reset (env layer)

`src/env.{c,h}` è il layer di config: ogni variabile che parrot0 legge a runtime
passa da `p0env(name)` (override sopra `getenv`, congelato in globali pilotabili).
`!set NAME=VALUE` imposta un override; `PARROT0_PID` sostituisce `getpid()`.

Lo stato del brain è firmato su **due assi** (così `!reload` e `!reset` sanno cosa
rifare, e nulla di ridondante):

| asse | cos'è | chi lo consuma |
|---|---|---|
| **firma di config** (`p0env_mem_signature`) | il *footprint di caricamento*: valori effettivi di `MEMORY_VARS` (BASE/SESSION/PROFILE/LEXICON/WORLD_FACTS/KB_ROOT, LANG/LC_*, TOOLS, WIKI_FETCH, PID) | un cambio → serve `brain_reload` da disco |
| **delta appreso** (`kb_size` vs baseline) | la KB è cresciuta da un turno che ha *imparato* | un reset deve azzerarlo |

- `!reload` (e l'auto prima di ogni turno): ricarica **solo** se la firma di config
  è cambiata. `!set` che non cambiano il valore effettivo (ripetizioni, o valore
  uguale a quello reale corrente) → nessun reload.
- `!reset`: dà un brain vergine, ma è **smart** — se la config è invariata **e**
  non è stato imparato nulla, la salta (il brain è già vergine per quella config).
- Le `MEMORY_VARS` sono in `src/env.c`. Variabili per-turno (PARROT0_ORACLE, HOME,
  WIKI_DIR, DEEP_BUDGET) sono pilotabili ma **non** in quella lista: cambiano senza
  reload.
- Debug: avvia il demone con `PARROT0_TE_DEBUG=1` per stampare su stderr ogni
  reload/reset/skip.

## 4. `make test` e fail-fast

Il target `test-engine` **possiede** il ciclo di vita del demone in background:
uccide l'istanza vecchia (pidfile `obj/test-engine.pid`) e ne avvia una fresca.
`test` lo usa come prerequisito, poi manda ogni file su una riga esplicita
(niente `for`; per saltare una suite si commenta la riga), infine `--test-report`.

```make
TEST_SOCK := obj/test-engine.sock
TEST_PID  := obj/test-engine.pid

test-engine: build
	@-test -f $(TEST_PID) && kill `cat $(TEST_PID)` 2>/dev/null || true
	@rm -f $(TEST_SOCK) $(TEST_PID)
	@./$(BIN) --test-engine & echo $$! > $(TEST_PID)

test: test-engine
	@./$(BIN) --test-send tests/p0t/basics.p0t
	@./$(BIN) --test-send tests/p0t/…          # una riga per file
	@./$(BIN) --test-report
```

**Fail-fast:** un `--test-send` esce con `1` appena il suo file ha un `FAIL` (il
report del file esce prima), quindi `make` si ferma lì; il prossimo `make test`
uccide il demone rimasto e rilancia pulito.

---

## 9. HANDOFF — riprendere la migrazione (leggere questo per primo)

> Punto di ripresa: **continuare a migrare `tests/cases/*.chat` → `tests/p0t/*.p0t`,
> uno alla volta, cancellando il legacy man mano.** Fermarsi e parlarne quando un
> caso richiede un *cambio di contesto / avvio controllato* non ancora coperto.

### 9a. Mappa del codice
- `src/testeng.{c,h}` — demone, client, interprete `.p0t`. Assi di stato
  `loaded_sig` + `clean_kb_size`; `te_apply_config` (reload smart), `!reset` smart,
  `te_flush` (assert). Il client `test_engine_send` fa il relay e legge il trailer
  `EXIT n`.
- `src/env.{c,h}` — `p0env`/`p0env_set`/`p0env_clear`/`p0env_mem_signature` +
  la lista `MEMORY_VARS`. **Qui** si aggiunge una variabile memory-affecting.
- `src/main.c` — parsing `--test-engine` / `--test-send FILE` / `--test-report` /
  `--sock`; i client ritornano **prima** di `setup_brain` (footprint zero).
- `src/brain/99-registry.c` — `brain_create`/`brain_boot`/`brain_policy` leggono
  la config via `p0env(...)` (non più `getenv`). PID pilotabile via `PARROT0_PID`.
- `src/kb.c` — `kb_load_clause(kb, "pred(a).")` già pronto per un futuro `!fact`.
- `Makefile` — target `test` / `test-engine` / `legacy-test` (banner di migrazione).
- `tests/p0t/*.p0t` — suite migrate. `tests/cases/*.chat` — legacy (globato da
  `tests/run.sh`, quindi **cancellare il `.chat` lo toglie da legacy-test**).

### 9b. Il fatto chiave che sblocca la migrazione di massa
**Tutti i `.chat` di `run.sh` sono validati ERMETICI** — `run.sh` esporta
`PARROT0_BASE= PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_LANG=en`, tools OFF,
network OFF. Quindi la migrazione uniforme e affidabile è: **preambolo ermetico +
`!reset` + il corpo del `.chat` verbatim**. I casi che dipendono dalla lingua/mondo
funzionano perché i loro turni italiani ribaltano la lingua per-turno.

### 9c. Ricetta per migrare UN caso (bash già usato)
```bash
gen_p0t() {  # crea tests/p0t/NAME.p0t da tests/cases/NAME.chat
  local f="$1"
  { printf '# migrated from tests/cases/%s.chat (hermetic + isolated)\n' "$f"
    printf '[mock hermetic]\n!set PARROT0_BASE=\n!set PARROT0_SESSION=\n!set PARROT0_WORLD_FACTS=0\n!set PARROT0_LANG=en\n!reset\n\n'
    printf '[test %s]\n' "$f"; cat "tests/cases/$f.chat"; printf '\n'
  } > "tests/p0t/$f.p0t"
}
```
Poi: (1) verifica isolata —
```
./bin/parrot0 --test-engine & sleep 0.3
./bin/parrot0 --test-send tests/p0t/NAME.p0t | grep -c FAIL   # deve essere 0
./bin/parrot0 --test-report >/dev/null; kill %1
```
(2) aggiungi una riga `@./$(BIN) --test-send tests/p0t/NAME.p0t` nel target `test`;
(3) `make test` (deve restare verde); (4) `git rm tests/cases/NAME.chat`.

Nota: un `.chat` è già `.p0t` valido (stessi `>`/`<`/`#`), quindi lo si può anche
mandare direttamente al demone per categorizzarlo prima di aggiungere il preambolo.

### 9d. Come categorizzare (evitare i falsi negativi)
- **Bleed di stato:** mandare più casi allo STESSO demone senza `!reset` fa
  inquinare l'uno con l'altro (lingua, nome, fatti). Isolare sempre.
- **Full-KB vs ermetico:** un caso che interroga fatti base (`man(socrates)` è in
  `base.p0`) o world-facts (`because(grass_green,…)`) fallisce a KB piena → serve
  ermetico. `greet/arith/world` passano anche a KB piena (migrati senza preambolo);
  il default sicuro resta comunque ermetico.
- **Confondente posizione discorsiva:** `ciao` a turno 1 è saluto, tardi è congedo.
  Non è lingua. Isolare i prompt sensibili alla posizione.

### 9e. STOP / decisioni aperte (di F.)
1. **Lingua (aperto).** `tests/p0t/language.p0t` è la matrice ambiente×prompt.
   Trovato: **il seed d'ambiente `PARROT0_LANG` è ignorato per il prompt ambiguo**
   ("ciao" risponde sempre in italiano); i prompt chiari seguono la lingua del
   prompt. 1 cella rossa (env inglese + "ciao" → dà "Ciao!", atteso "Hi there!").
   Il file è **fuori da `make test`** finché non si decide la semantica e si
   sistema. Da fare PRIMA di migrare i casi con problemi di lingua.
2. **Rossi pre-esistenti nel legacy** (NON regressioni): `apology.chat`,
   `compose_social.it.chat`, `self.chat`, e ~73 casi `.it` (canonicalizzazione
   IT→EN da gen334, vedi memoria `abstraction-ceiling`). Verificarli con
   `PARROT0_TEST_JOBS=1 ./tests/run.sh | grep FAIL`. Non migrarli verdi: o si
   saltano (restano legacy) o si sistema la causa.
3. **`tests/syllogism.sh`** (script dedicato, non `.chat`): genera classi nonce
   CASUALI a ogni run — serve una primitiva generativa/stub non ancora progettata.
   Resta in `legacy-test`. Idem altri script dedicati non-conversazionali
   (`toolexec.sh`, `mcp*.sh`, `patch*.sh`, …): richiedono tool/rete/contesti che
   il `.p0t` base non esprime.
4. **Mock / stub / flag / stato-KB:** sintassi da definire con F. (`kb_load_clause`
   è già pronto per `!fact`).
5. **Limite dell'isolamento `!reset`:** lo smart-skip guarda solo l'*appreso*
   (`kb_size`), NON lo stato di sessione C (contatore turni, `current_language`,
   nome). Su uno skip quello stato non si azzera. Conta solo per casi che dipendono
   dalla posizione-turno fresca dopo un altro caso; workaround: mettere per primi i
   prompt sensibili, o forzare un reset reale (cambio config o apprendimento
   precedente). Da valutare se aggiungere lo stato-sessione come terzo asse.

### 9e-bis. Organizzazione dei file
`tests/p0t/` è suddiviso in **sottocartelle per categoria**: `conversation/`,
`reasoning/`, `math/`, `knowledge/`, `language/`, `meta/`, `generation/`,
`intent/`, `planning/`, `bench/`. Un nuovo `.p0t` va nella cartella della sua
categoria; la ricetta §9c scrive in `tests/p0t/<categoria>/<nome>.p0t` e la riga
`--test-send` nel Makefile usa lo stesso path. Il report mostra comunque il solo
basename (es. `ok reqgen.p0t — 8 passed`).

### 9f. Già migrati (in `tests/p0t/<categoria>/`, cablati in `make test`)
`basics`, `conversation` (demo nuove, full-KB) · `greet`, `arith`, `world`
(full-KB) · `facts`, `syllogism`, `casefold`, `contractions`, `numwords`,
`initials`, `social_reaction`, `social_opener`, `smalltalk`, `chitchat`,
`arith_flex`, `arith_nl` (ermetici + `!reset`).
Non cablato: `language.p0t` (matrice/spec, §9e-1).

### 9g. Comandi utili
```
make test                          # suite p0t (verde atteso)
make legacy-test                   # vecchio harness (contiene i rossi noti)
PARROT0_TEST_JOBS=1 ./tests/run.sh # solo i .chat, in serie, con PASS/FAIL per caso
PARROT0_TE_DEBUG=1 ./bin/parrot0 --test-engine   # demone con log reload/reset
# caso legacy alla maniera vecchia (processo fresco, ermetico):
printf 'PROMPT\n' | PARROT0_BASE= PARROT0_SESSION= PARROT0_WORLD_FACTS=0 PARROT0_LANG=en ./bin/parrot0
```

## 10. Rotta
1. Sistemare la semantica della lingua (§9e-1), poi cablare `language.p0t` verde e
   migrare i casi lingua-dipendenti (`social`, `apology`, i `.it`).
2. Continuare la migrazione dei `.chat` deterministici (§9c), a lotti.
3. Definire con F. mock/stub/flag/stato-KB e la primitiva generativa (per
   `syllogism.sh` e simili).
4. Cambio di working directory (`!cd` era prototipato e poi rimosso) per le
   fixtures del coding-agent, quando serve.