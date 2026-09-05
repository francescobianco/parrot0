# HANDOFF — gen503, 2026-09-05

Sessione lunga. Qui c'è **dove siamo**, **che cosa è vero e misurato**, e **da
dove si riprende**. Chi riprende legga questo file per intero prima di toccare
qualsiasi cosa: metà delle scoperte di oggi sono *correzioni di cose che i file
davano per vere*.

---

## 1. In una riga

Il ciclo di un coding agent **si chiude**: su un progetto rotto vero, in un
turno, parrot0 esegue il build, legge cosa manca, legge il contratto dalla
**prosa** di un header, sceglie una forma, la emette, **scrive il file**,
rilegge e ricompila — e il `make` passa con `-Werror` e l'eseguibile esce 0.

Cricchetto: `tests/p0t/code/build_repair_cycle.p0t` — 15 passed, col compilatore
vero dentro.

⚠ **Questo NON è la parità con freebuff**, e chiamarla così sarebbe l'inganno che
`PRINCIPLES.md` vieta. Il punteggio del banco non si è ancora mosso: su match0
nessuna forma dichiarata soddisfa il contratto di `strjoin.h` (manca
`empty_on_zero`), e dichiararne una che combaci sarebbe scrivere la risposta del
banco in KB. Vedi `CHALLENGE_TODO.md` §6.5-bis.

---

## 2. La forma comune di quasi tutti i difetti trovati oggi

**Osservazioni buttate via**, o **tetti fissi su liste che crescono**. Non erano
capacità mancanti: era roba già prodotta e poi persa.

| difetto | sintomo |
|---|---|
| uno strumento dichiarato scartato da una catena di `strcmp` | «non so scrivere un file» |
| `ident(...);` fuori da una funzione ignorato dallo scanner | un header usciva con **zero nodi** |
| il verdetto di un build stampato e dimenticato | `run make` nominava il file mancante, e un turno dopo non si sapeva più |
| i commenti cancellati prima dello scanner | il contratto non si poteva leggere |
| impronta a 128 nomi, `mod_toolplan` a 1024 byte, `learnable` a 96 righe | **troncamenti silenziosi**, e i tre si spegnevano proprio sui turni per cui esistono |
| `kb_root_prefix` che teneva in cache «la cwd va già bene» | caricamento pigro rotto **solo dentro una sandbox**, cioè solo nei test |

⭐ **Regola operativa che ne esce, ed è la cosa più utile del giorno:** *prima di
costruire una capacità, guardare se il dato che le serve viene già prodotto e
scartato.* È così che A3 si è chiusa senza scrivere un parser di Makefile.

---

## 3. Che cosa è stato costruito (tutto con cricchetto e ablazione)

```text
scrivere un file        write_tool.p0t                    4
A1 dichiarazioni        header_declarations.p0t           4
A3 obbligo dal build    tool_result_becomes_knowledge.p0t 3
A2 contratto dalla prosa header_contract.p0t              5
A6 il turno arriva      repair_broken_build.p0t          12
A5 emettitore KB        kb_code_emitter.p0t              18
M4 forma dal contratto  contract_selects_shape.p0t       13
il ciclo                build_repair_cycle.p0t           15
superfici insegnate     taught_tool_surface.p0t          12
aree logiche            crossing/ (4 file)               18
```

Piani vivi: `docs/plans/kb-code-emitter.md` (M1–M5 ✅), `CHALLENGE_TODO.md` §6.

---

## 4. ⛔ LA VOCE PIÙ IMPORTANTE APERTA — A8

**F., 2026-09-05:** *«non esiste assolutamente questa rottura del paradigma
KB-first. Gli operatori devono essere apprendibili a runtime. Anche gli
operatori devono essere KB. Presto dovrà essere possibile addestrare a runtime
un TOOL nuovo.»*

Lo **schema** di un piano è conoscenza; i **verbi** che può usare sono `else if`
in `src/brain/60-agent-tools.c`. È il mantra #19 un piano più su.

```text
rami compilati:  5 → 4      (`run_build` è diventato una frase)
passi dello schema repair_broken_build: 5 operatori → 2
```

`plan_utterance/3` è la strada e **funziona già**: un passo è una frase che
parrot0 dice a se stesso e rientra a digerire, e ciò che quel turno lascia in KB
è la connessione col passo dopo. Due passi (`list_sources`+`read_each`) sono
diventati una frase sola — *«analyze all the sources»* — che invoca uno schema
che parrot0 aveva già.

Dettaglio, misura e ordine di lavoro: `C_TODO.md` §«gli operatori di un piano
sono compilati», `CHALLENGE_TODO.md` §6.2 voce A8, e il `TODO(kb-first)` sopra
la catena stessa.

⛔ **Il confine vero, da disegnare prima di scrivere:** `local_tool/3` +
`tool_argv/2` sono già conoscenza, ma il loro argv finisce in `p0_exec` — cioè
**l'unica azione insegnabile oggi è «esegui un programma con i suoi
argomenti»**. Scrivere, ingerire, ritrattare non hanno nessuna forma
dichiarativa. Finché quel confine non è disegnato, migrare gli operatori sposta
il problema invece di chiuderlo.

---

## 5. ⬅ DA DOVE SI RIPRENDE, in ordine

### R1. Le forme di insegnamento che non funzionano — **lasciata a metà, di proposito**

**F.:** *«le forme di insegnamento che non funzionano vanno comunque sistemate,
perché massimizzare la comprensione e la metacomprensione è pure un obiettivo di
addestramento».*

Misurato: delle sei forme naturali con cui una persona insegna una parafrasi,
**una** funziona.

```text
✅ learn "X" as another way to say "Y"
❌ "X" means "Y"                              → la lezione di COSTRUZIONE la reclama
❌ "X" is another way to say "Y"
❌ saying "X" is like saying "Y"
❌ when I say "X" I mean "Y"
❌ "X" is the same as "Y"
```

**Tentativo fatto e RITIRATO oggi**, e va saputo perché: le quattro forme senza
«means» sono state aggiunte come righe di KB (cue di guardia in
`00_lex_chain332` + righe `learnable` con modo `cue_like`) e **funzionavano
tutte e quattro**. Ma `taught_cue_ladder.p0t` è passato da 23 a 15/8. Non ho
avuto il tempo di isolare quale riga rompe cosa — il sospetto è `" i mean "`,
che è una sottostringa frequentissima — e ho preferito ritirare tutto piuttosto
che lasciare un rosso.

**Come riprendere:** riaggiungere **una riga alla volta**, con
`taught_cue_ladder.p0t` come cricchetto dopo ognuna. Il diff ritirato è in
questo commit (`f8c5280`) nel messaggio, e le righe erano:

```prolog
intent_cue(00_lex_chain332, " is another way to say ").
intent_cue(00_lex_chain332, " is the same as ").
intent_cue(00_lex_chain332, " is like saying ").
intent_cue(00_lex_chain332, " i mean ").          % ← il sospetto
learnable("is another way to say", intent_cue, cue_like).
learnable("is the same as",        intent_cue, cue_like).
learnable("is like saying",        intent_cue, cue_like).
learnable("i mean",                intent_cue, cue_like).
```

Per `"X" means "Y"` la nota che serve: ha già un proprietario legittimo — la
lezione di **costruzione** (`x y means x <verbo> y`). Le due forme si
distinguono per **struttura**, non per parola: la parafrasi ha entrambi i lati
fra virgolette, la costruzione no, e `try_teach_form` esce subito senza due
stringhe citate. La convivenza è possibile, va solo verificata.

### R2. A8 — gli operatori diventano KB

L'ordine è in `C_TODO.md`. Il primo passo **non è scrivere C**: è insegnare le
superfici che mancano, ora che si può farlo parlando (§3, `cue_like`).

### R3. A6 — la fragilità dell'arbitrato, causa già nominata

`kb_cue_match` **non è un test di sottostringa**: passa da `kb_hypothesis_best`,
cioè da un punteggio di evidenza, e la cue dichiarata di un piano può **perdere**
dentro un turno che porta altra evidenza. Misurato, e non monotono:

```text
the project does not build: file            → lo schema parte
the project does not build: missing         → lo schema parte
the project does not build: file missing    → NON parte
```

⚠ È esattamente la classe di turni del banco. Che il prompt di match0 funzioni
è, con questa spiegazione, **una fortuna e non una garanzia**. Dettagli in
`CHALLENGE_TODO.md` §6.7.
⛔ Il rimedio NON è aggiungere cue finché la frase passa: sarebbe la colonna
destra di §6.5-bis. Il lavoro è sul meccanismo.

### R4. I rossi che restano, e sono di prima

```text
toolexec.p0t          19/3   (uno è marcato «ROSSO da prima» nel file stesso)
agentcommit.p0t       11/16
codeast.p0t           15/5
taught_lexicon.p0t    18/17
taught_gloss.p0t      13/2
frontier_chat_audit   53/3   (soft-test è rosso su questi tre)
```

Tutti **verificati con `git stash` come identici a prima** delle modifiche di
oggi. `check_sort.p0t` era 0/5 e ora è 5/5: non era rotto il giudice, era il
test che usava `<~` (sorgente *turn*) su un'uscita *mcp*.

---

## 6. Strumenti nuovi che chi riprende deve conoscere

- **`!cwd DIR` / `!cwd off`** nei `.p0t`: sposta la directory in cui parrot0
  lavora. Serve ai casi che **sono** una proprietà della directory (un progetto
  che non compila). ⚠ E previene un danno reale: `!exec` gira nella radice del
  repository, e un `.p0t` che scriveva e poi cancellava `Makefile` credendo di
  essere in una sandbox **ha cancellato il Makefile di questo repository**.
  Documentato in `TEST_TODO.md` §I.
- **`code.synth_shape`** su MCP, accanto a `code.check_sort`: emette una forma
  dichiarata. Comporre e giudicare restano due atti separati.
- **`make repair-check`** non esiste più: quel caso è diventato un `.p0t` vero
  grazie a `!cwd`.

---

## 7. Le regole di condotta fissate oggi, da non perdere

1. **La conoscenza operativa in KB è una conquista** (F.), e il confine è: *«serve
   anche a un compito che non è nel banco?»*. Tabella in `CHALLENGE_TODO.md`
   §6.5-bis, con il corollario sui cricchetti — un `.p0t` che asserisce le
   clausole invece di leggerle è barare travestito da verde.
2. **Un cricchetto senza ablazione non è un cricchetto.** Tutti quelli scritti
   oggi hanno la loro: prima dell'azione, la domanda non deve sapere.
3. **Codice non provato si toglie**, anche quando esprime un principio giusto.
   Oggi è successo una volta (la regola «un turno che dichiara un piano non si
   segmenta»: non serviva, il turno non era segmentato).
4. **Un test si aggiorna, non si forza.** `check_sort` era stantio, non rotto.

---

# APPENDICE — seconda metà della sessione gen503

## A. Le forme di insegnamento: bisezione fatta, colpevole trovato

`HANDOFF §R1` diceva di riaggiungerle **una alla volta**. Fatto, ed è servito:

```text
"X" is another way to say "Y"      ✅ ladder 23, tool 16   → TENUTA
"X" is the same as "Y"             ✅ ladder 23, tool 16   → TENUTA
saying "X" is like saying "Y"      ⛔ ladder 15/8          → RIFIUTATA
```

⚠ **Il sospetto scritto ieri era sbagliato, e va detto:** non è `" i mean "`, è
**`is like saying`**. Non ho ancora capito *perché* faccia cadere il retract di
`taught_cue_ladder` (il sintomo è che `forget "…" as a casual opener` smette di
essere riconosciuto e finisce a muro). **Questa è la voce aperta**, ora col nome
giusto invece che col sospetto.

Restano da provare, sempre una alla volta:

```prolog
intent_cue(00_lex_chain332, " i mean ").      learnable("i mean", intent_cue, cue_like).
intent_cue(00_lex_chain332, " means ").       learnable("means",  intent_cue, cue_like).
```

Per `means` la nota che serve: ha già un proprietario legittimo — la lezione di
**costruzione** (`x y means x <verbo> y`). Si distinguono per **struttura**: la
parafrasi ha entrambi i lati fra virgolette, la costruzione no, e
`try_teach_form` esce subito senza due stringhe citate. La convivenza è
possibile, va verificata.

Il cricchetto `taught_tool_surface.p0t` è ora 16 e copre entrambe le forme nuove,
la seconda su un file **mai nominato nella lezione** — una parafrasi di strumento
deve generalizzare, non essere una frase sola.

## B. Il banco di sonde di F. — `var/probe/`

Dieci famiglie di prompt lunghi, un generatore di item `.p0t`, un runner che
avvia un demone fresco per famiglia e registra **latenza e risposta verbatim**.
È lo strumento giusto per misurare la comprensione dove finora si guardava a
occhio. Committato: harness, generatore e item (il pannello congelato). Le
misure no — si rigenerano, e `results/` è gitignorato.

⛔ **Non misura ancora niente.** I 16 item di `f01` danno tutti `rc=2`, e i
3,06 s per item sono esattamente il budget di retry del client
(300 × 10 ms) → *«test-send: cannot reach engine»*. **A mano funziona:** avviato
il demone con lo stesso `--sock` e mandato `i07.p0t`, la risposta arriva
(`«Fair enough — tell me where I went wrong…»`). Quindi non è il trasporto in sé:
è qualcosa nel modo in cui `probe_one.py` avvia o attende il demone. Indagine
interrotta qui su richiesta di F.

⚠ E una nota sul disegno degli item: usano `< __NEVER__` come sentinella per
farsi stampare la risposta. Funziona, ma significa che **ogni item è rosso per
costruzione** e il codice di uscita del client non distingue «il test è fallito
come previsto» da «non ho raggiunto il demone». Vale la pena separarli, o
leggere la risposta da un canale che non passi dal verdetto.

## C. Misurare il lato di parrot0 da solo — bloccato, e si sa da cosa

Il piano (§6.5) dice che ogni gradino si misura sul banco, e dopo A5 il punteggio
dovrebbe potersi muovere. Ho provato a costruire una league col solo parrot0
(`--league` esiste apposta) e mi sono fermato su un limite reale del pilota:

1. pretende **esattamente due agenti** — aggirabile con un no-op dichiarato
   (`fake_agent.py --mode silent`), e l'avevo fatto;
2. ma poi `order` sta in **`tasks/matchN/match.json`**, non nella league, e
   nomina `parrot0` e `freebuff`. Per correre da soli servirebbe **un override
   di `order` nella league**, che oggi non esiste.

È un pezzo piccolo e nominato: *permettere alla league di sovrascrivere l'ordine
di un match*. Senza, ogni misura del nostro lato richiede la presenza (e il
modello concordato) di freebuff — cioè resta bloccata dietro C0.

**Il file `league-parrot0.json` NON è stato lasciato nel repo**: era a metà, e un
artefatto a metà nel banco è peggio della sua assenza.

## D. Da dove si riprende, aggiornato

```text
R1  finire le forme di insegnamento — colpevole noto: «is like saying»
R2  A8: gli operatori diventano KB (C_TODO.md), partendo dalle superfici
R3  A6: la fragilità di kb_cue_match (CHALLENGE_TODO §6.7)
R5  far misurare la sonda di F. (§B) — a mano funziona, dal runner no
R6  override di `order` nella league, per misurare un lato solo (§C)
```
