# TEST_TODO — le decisioni aperte della migrazione a `.p0t`

Coda delle cose che **non decido da solo** e di quelle che restano da fare nella
migrazione delle suite shell verso il test-engine
([`docs/plans/test-engine.md`](docs/plans/test-engine.md)).

Stato: **113/113 file sistemati (100%)** alla radice di `tests/`.

---

# ⛔ HANDOFF — 2026-09-03, gen491. LEGGERE PRIMA DI RIPRENDERE.

> Sessione interrotta perché F. doveva andare. Qui c'è dove siamo arrivati, che
> cosa è già verde, e **l'ordine esatto** in cui continuare.

## H.1 — Il metodo, prima dei numeri (non ripetere i miei due errori)

**(a) Non si misura un rosso girando i file isolati.** `make test` li manda
**in ordine, su UN SOLO demone**, e alcuni dipendono dallo stato dei precedenti.
Il mio primo sweep, alfabetico e isolato, dava il 46% di rossi: accusava il
motore di difetti che non ha. Lo strumento giusto è in
`scripts/`-style ma vive ancora in scratchpad — **va promosso**: legge l'ordine
dal target `test:` del Makefile e non si ferma al primo rosso (`make test` è
fail-fast, quindi mostra un rosso e nasconde gli altri).

**(b) Ogni rosso va confrontato col commit di partenza.** Un worktree
(`git worktree add <dir> <sha>`) sullo stesso file è l'unico modo onesto per
separare *«l'ho rotto io»* da *«era già rosso»*. Su 17 rossi: **3 miei, 14
preesistenti, di cui 5 migliorati** durante la sessione. Le due corse sono in
`docs/reports/gen491-suite-order-run.txt` e `gen491-red-at-base.txt`.

**(c) ⛔ Non assecondare i tempi lunghi con timeout più grandi.** È la
correzione che F. ha dato a metà sessione: *«non mi piace che lavori con questi
timeout che ti chiami da solo; un timeout di 10 secondi è già un sintomo, anche
il modo come indaghiamo»*. Un turno lento **si profila** (`/debug`), non si
aspetta. Il `!timeout` è ammesso **solo quando la causa è nota e nominata** e la
misura è scritta accanto — F. l'ha confermato per il caso della vista
invalidata.

## H.2 — Che cosa è già chiuso

| | |
|---|---|
| `health.p0t` | la suite **non partiva più** (60% di rossi sul primo turno): firma della vista che contava i fatti congelati di un'altra vista. 891 ms → **131 ms** |
| `literal_forms.p0t` | 49/0 → 38/11 → **49/0**: la vista troncava a 256 schemi su 359, in silenzio |
| `contractions.p0t` | **8/0**, e primo file convertito da ermetico a KB piena (R1) |
| `TEST_TODO §0.0` + `docs/plans/test-engine.md` | le due regole R1/R2 di F., scritte dove si vengono a cercare |

## H.3 — ⛔ L'ORDINE IN CUI CONTINUARE

**1. Finire la corsa completa della suite.** La mia si è fermata al file 59
(`meta/self_repair.p0t`) perché quel file contiene un turno da **135 secondi**
(`prova a ripararti`: fino a 240 replay completi del turno, ~340 ms l'uno). Non
è un blocco e **non è una regressione** — è così anche al commit di partenza, e
i miei `timeout 90` lo troncavano facendolo *sembrare* un blocco, con 281
«cannot reach engine» a valle. Quindi: **i 17 rossi noti sono su 59 file, non su
353. I restanti 294 non sono mai stati misurati.** È la prima cosa da fare.

**2. I tre rossi rimasti fra quelli noti**, in ordine di chiarezza:
   - **`mcp/aggregate.p0t`** (4/0 → 2/2, **regressione mia, non diagnosticata**):
     «chi ha vinto di più» risponde `M1.` invece di `spain`. Il file usa
     `PARROT0_PROFILE=` vuota e `WORLD_FACTS=0`: **va prima convertito (R1)**,
     perché metà della diagnosi potrebbe essere il contesto amputato.
   - **`conversation/forget_move.p0t`** (3/3, preesistente): il messaggio di
     `forget` è *formattato e mai emesso* — `answerframe` ruba il turno. È
     `LEARN_TODO` P4.3, ed è mantra #17 puro: si chiude con un `faculty_yield`,
     non con una riga di C.
   - **`conversation/greet.p0t`** (7/1, preesistente ma è **R2 da manuale**):
     `tell me about C` asseriva ignoranza, e il corpus del gen490 ha insegnato
     il carbonio. Va spostato il confine, non rimessa l'ignoranza. ⚠ Nella
     risposta c'è anche un difetto vero da annotare: *«c is a chess_file»*
     appende trivia di scacchi a una definizione di chimica — collisione di
     classe che il mantra #14 vuole chiusa con una guardia teachable.

**3. Gli 11 preesistenti già identificati** (dettaglio in
`docs/reports/gen491-red-at-base.txt`): `motorize_class` (2, forma del messaggio
migrata in KB — probabile R2), `repair` (9, l'oracolo riporta `build_failed`:
verificare se è ambiente o difetto), `check_sort` (5, `expected source: turn, got
source: mcp` — è la migrazione `!expect` del §0.1 di questo file, non un difetto
del motore), `games`, `faceted_enumeration`, `foundational_concepts`,
`gap_dialogue`, `class_conflict`, `name_is_knowledge`,
`reactions_are_knowledge`, `gap_is_a_fact`, `gap_anchor`.

**4. La ricostruzione INCREMENTALE della vista materializzata.** Oggi insegnare
un verbo di relazione invalida `extract_frame` e la ricostruisce **intera**:
1406 ms contro 224 di regime, 715.931 passi. È l'unico `!timeout` che ho dovuto
dichiarare, ed è il candidato numero uno del §L.

**5. La conversione R1, 288 file.** Non si fa in campagna: **si converte ogni
file che si tocca**. Chi arriva a zero cancella il paragrafo in §0.0.

---

## ⛔ 0.0 LE DUE REGOLE CHE NON SI DISCUTONO (F., 2026-09-03)

Vengono prima di ogni altra voce di questo file, e prima di scrivere o
correggere qualunque `.p0t`.

### R1. Il contesto ermetico NON ESISTE PIÙ

> F.: *«non esiste più e non accettiamo uso di contesto ermetico: la KB è parte
> del progetto e non può essere spenta durante i test né frazionata».*

Sono vietati, in ogni file nuovo e in ogni file che si tocca:

```
[mock hermetic]
!set PARROT0_BASE=          ← una KB di sole regole, senza conoscenza
!set PARROT0_WORLD_FACTS=0  ← il mondo spento
```

**Il perché, e non è una preferenza di stile.** La KB non è un volume montato
sotto parrot0: *è* parrot0. Un test che la spegne non misura parrot0 con meno
rumore — misura **un altro sistema**, che non esiste e a cui nessuno parlerà mai.
E il costo si è già visto due volte: `frontier_chat_audit.it` misurava una KB
amputata e dava 31 rossi su 56 **per costruzione** (`830bc59`), e il gen459 ha
perso dieci turni su un difetto che in KB piena non c'era.

**Si testa sempre con la KB al massimo.** Se serve forzare una non-conoscenza,
si spegne *quella cosa lì* con gli strumenti che il framework già offre —
`!forget`, e la si rimette dopo — oppure si usa un'entità nuova che nessuno può
conoscere (`zorbles`, `puppo`, `nivora`). Mai spegnere il mondo per far tacere
una frase.

**Debito misurato al 2026-09-03: 288 file su 441 usano `[mock hermetic]`** (268
con `PARROT0_BASE=` vuota, 291 con `WORLD_FACTS=0`), cioè il 65% della suite. È
troppo per una sessione: la regola è **si converte ogni file che si tocca**, e
nessun file nuovo lo usa. Chi finisce la coda cancella questo paragrafo.

### R2. Un'ignoranza resa falsa dalla KB si chiude cambiando il TEST

> F.: *«le ignoranze rese false vanno colmate cambiando il test, man mano che la
> KB cresce, essendo essa stessa parte del progetto. I test perdono di
> significato e vanno ripensati per individuare un confine nuovo di
> testabilità».*

Un test che asserisce *«di questo parrot0 non sa niente»* ha una scadenza: il
giorno in cui glielo si insegna, quel rosso **non è una regressione, è una
crescita**. Esempio reale di oggi — `greet.p0t`:

```
> tell me about C
< I don't understand that yet.        ← vero fino al gen489
                                      ← falso dal gen490: C è il carbonio
```

La riparazione **non è** rimettere parrot0 nell'ignoranza: è chiedersi *qual è
adesso il confine della testabilità*, e spostare il test lì — con un'entità che
nessuno può conoscere, o forzando l'oblio di quella specifica cosa. L'intento del
caso («ciò che non è riconosciuto riceve un non-capisco onesto») si conserva; il
campione con cui lo si prova cambia, perché il campione è invecchiato.

⚠ **Questo NON autorizza a cambiare un'attesa per far passare un rosso.** Resta
la regola del §5 di `LEARN_TODO.md`: *prima si capisce chi ha torto fra il test e
il codice*. R2 vale solo quando ciò che il test asseriva **è diventato falso
perché parrot0 ha imparato** — e in quel caso il commit deve dirlo.

### R3. Un cricchetto che INSEGNA deve ritirare ciò che insegna

> Scoperta al gen492, e il costo era già stato pagato senza accorgersene.

`universal_code_ir.p0t` provava il canale #1 della Gerarchia di Crescita:
*prima* la parafrasi non funziona, si insegna, *dopo* funziona. Ma non ritirava
la forma insegnata, e il file conteneva anche un `kb.save`. Risultato: il
save-map ha instradato la frase inventata dal test —
`answer_frame("observation how from", code_definition_evidence)` — **dentro la
KB curata** (`kb/core/discourse.p0`), dove è stata committata.

Da quel momento il «prima» del caso era falso **per sempre**, e il file passava
solo su un albero che non l'aveva mai eseguito. Il rosso sembrava una
regressione del motore: era il test che aveva sporcato il progetto.

**La regola:** un ratchet che insegna **si chiude ritirando** (`!forget` di
tutti i fatti che ha creato), e nessun ratchet chiama `kb.save` su una KB in cui
ha appena insegnato qualcosa. Il residuo trovato è stato rimosso da
`discourse.p0`; il file ora è **idempotente** — passa 61/61 anche alla seconda
corsa sullo stesso demone, che è il vero criterio.

**Come si riconosce il sintomo:** un `.p0t` che passa la prima volta e fallisce
la seconda sullo stesso demone *sta insegnando senza ritirare*. È diverso da un
timeout, e diverso da una regressione: si controlla eseguendolo due volte prima
di accusare il motore.

---

## 0. HANDOFF — da leggere per primo

### 0.1 `!expect` — engine implementato, conversione suite in corso

`!mcp` ed `!exec` mettevano il loro risultato **dove va la risposta di parrot0**, e
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

Sono implementate le tre varianti che `<` ha già: contiene (`!expect`), non contiene
(`!expect!`), uguale esatto — e la sorgente da riconoscere è almeno `mcp` ed
`exec`.

Restano da aggiornare i file già convertiti che usano `<~` dopo `!mcp`:

`p0t/mcp/*.p0t` (6), `p0t/engine/naf.p0t`,
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

### 1.2 `learnbuild`: allineato; debito residuo documentato

Lo **stesso identico prompt** riceve due risposte diverse:

| | risposta a `write a vunder function` |
|---|---|
| binario | «I can only synthesize and VERIFY the sum, product, or difference of two integers so far — I will not emit code I cannot check.» |
| test-engine | «I understood the request … but I don't have a verified schema for that artifact yet; I only synthesize what an oracle can check (a sort from a learned shape, arithmetic composition, a count-to-threshold game).» |

Riprodotto su un demone appena avviato, stesso env, senza sandbox. Sono due
declini entrambi onesti, ma **non sono la stessa frase**: qualcosa fa vincere un
modulo diverso sotto `--test-engine`.

Il disallineamento era nel boot del test-engine: il demone non partiva con il
profilo AGI e quindi il test-engine selezionava un modulo diverso dal processo
legacy. Ora il demone usa gli stessi default (`PARROT0_TOOLS=1`, sessione vuota,
profilo `kb/profiles/agi.p0`) e `learnbuild.p0t` passa insieme al test legacy.

Debito futuro: il test-engine deve poter esprimere nonce generati e verifiche di
induzione/build senza dipendere da fixture statiche; per ora il `.p0t` conserva
il caso held-out con `vunder` e il controllo meccanico del profilo.

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

Non restano file `.sh` alla radice di `tests/`. I runner shell sotto
`tests/cdriver/` sono invece l'adapter generico per driver C diretti e non fanno
parte della migrazione conversazionale.

### 2.1 Conversione automatica tentata e SCARTATA (storico)

Generate e buttate perché non riproducevano l'originale — vanno rifatte a mano:

Le conversioni automatiche storiche furono scartate perché perdevano asserzioni;
le suite interessate sono state poi convertite manualmente.

### 2.2 Ibride: conversazione + driver C

I casi ibridi mantengono la parte conversazionale in `.p0t` e i controlli API
diretti in `tests/cdriver/integration/`.

### 2.3 Grosse, da leggere prima di toccarle

Il ciclo candidato → oracolo → policy → commit è coperto dai driver C dedicati.

### 2.4 Il resto

Le restanti voci storiche sono state assorbite nelle suite `.p0t` o nei driver
API dedicati.

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
