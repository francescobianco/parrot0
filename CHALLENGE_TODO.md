# CHALLENGE_TODO — far funzionare il banco di gara, e farlo scoprire cose

> **Aggiornato il 2026-09-04 (gen502).** La sessione precedente lasciava scritto
> che «nessuno dei due agenti lavora». **Era falso, ed è la scoperta principale
> di oggi**: freebuff lavorava benissimo e non lo vedevamo. Qui c'è lo stato
> corretto, l'evidenza che lo corregge, e l'ordine in cui riprendere.
>
> Il banco vive in [`tests/challenge/`](tests/challenge/); il protocollo di
> equità è in `tests/challenge/README.md`. Questo file è la **coda di lavoro**,
> non la documentazione.

## §0. Perché esiste, in una riga

Mettere parrot0 e un coding agent reale sullo **stesso compito e sugli stessi
byte**, in sequenza, e ricavarne **indicazioni su che cosa parrot0 ancora non sa
fare**. Non è un test di release, non sta in `make test`, e non deve mai
diventare una vetrina: se un match non produce una scoperta, quel match è
sprecato.

---

## §1. ⭐ LO STATO VERO — e le due cose che il banco misurava male

La corsa di riferimento `gen501-pilot-b` diceva questo:

| agente | durata | fine | turni | ha toccato `code/` |
|---|---|---|---|---|
| parrot0 | 32,4 s | `completion_marker` | 12 | no |
| freebuff | 49,3 s | `timeout` | 1 | no |

E se ne concludeva: *«nessuno dei due lavora»*. **Sbagliato su entrambi i
fronti**, e per due ragioni diverse che non c'entrano niente con gli agenti.

### 1.1 ⛔ freebuff LAVORAVA. Non lo vedevamo, e l'abbiamo ucciso a 45 secondi

`raw.log` di quella corsa, rigiocato dentro un pane tmux 160×45 e letto con
`capture-pane -p`, mostra lo schermo che c'era davvero:

```text
  You are working in the existing multi-file C11 codebase in code/… ⎘   ← 1485 byte, INTERI
  • Thinking
    Let me start by exploring the codebase to understand the structure.
  • List .
  • Read quicksort.h, records.h, main.c, Makefile
  • Read records.c, csv.h, csv.c
  • Thinking
    …the budget caps at 2*log2(n). With 2*log2(n)+2 units, after burning
    through them, heapsort kicks in. Standard introsort. Wait, but with
    over-counting on the iterative path
 working…                                                    43s  ■ Esc
```

Aveva ricevuto il task intero, letto sette file, e al secondo **43** stava
ragionando sul budget di profondità dell'introsort. Il timeout era **45**.

Ne seguono tre correzioni al file precedente:

1. **L'ipotesi «bracketed paste» (§3-bis.1) è FALSIFICATA.** Il paste
   funzionava: il prompt da 1485 byte è tutto nell'editor. Non era il submit.
2. **Il `logic_action` sul selettore del modello non c'entrava.**
3. **Il difetto era la LETTURA**, cioè esattamente ciò che il §3-bis aveva
   identificato come «il vero argomento per tmux» e messo in fondo alla lista.
   Andava messo in cima: non era un'opzione, era la causa.

**Perché la lettura non poteva funzionare, in un numero.** Un TUI a schermo
alternato non aggiunge byte: **ridisegna**. Su 591 KB di `raw.log`, la riga
`Enter a coding task` compare **tre volte, tutte nei primi 92 KB**. Il
`clean_transcript` che ne usciva era **169 KB contenenti un solo glifo
visibile** (`✕`). Nessun `done_pattern` poteva matchare lì dentro, mai.

### 1.2 ⛔ E parrot0 gareggiava con un binario di gen459

`bin/parrot0` era fermo al 28 agosto: **gen459, 13519 fatti**. Il repo era a
gen501: **24692 fatti, 1601 regole**. Metà della conoscenza in meno — e il
`result.json` scriveva serenamente `f745000-dirty`, perché `version_argv` è
`git describe`, cioè la versione del **repository**, non del programma avviato.

Un binario più vecchio dei suoi sorgenti è l'inganno peggiore che questo banco
possa subire: **la gara sembra regolare e misura un altro programma.** È anche
la spiegazione più semplice delle stranezze sul prompt della sessione scorsa —
il binario che correva era antecedente all'unificazione di `prompt_str()`.

---

## §2. ⭐ LE SCOPERTE — questo è il prodotto del banco

### S1. Un task di coding lungo viene rubato dal generatore di storie

**Confermata anche col binario gen501 fresco.** Prompt da 1485 byte su
quicksort, risposta:

```text
It was a mysterious it. Then one day, it discovered what it meant to be seen.
it had never felt this way before — as if the whole world had shifted…
```

Un modulo creativo rivendica il turno. È **mantra #17** su un input reale: una
facoltà che non sa onorare la richiesta non tace, produce. Peggiore di un muro.

### S2. Il thinking peggiorava la risposta, in diretta

Nel transcript `gen500-stream-v3` il pensiero 2 ha trasformato l'uscita in
*«What number should I use for «it»?»*. È la voce **A0** di
`THINKING_TODO.md`: la guardia prende i muri, non le degradazioni.
**Azione presa:** `PARROT0_THINKING=0` nella league, con la ragione scritta nel
config. *Misurare parrot0 con una funzione nota-rotta non è misurare parrot0.*
Si riaccende quando A0 è chiusa — ed è allora un ottimo esperimento (E4).

### S3. Dodici turni, nessun file toccato

parrot0 non ha il ciclo `esplora → decidi → scrivi → verifica`; ha un piano su
strumenti (gen495) che è *read-only* e non sa scrivere file.
→ **è la voce B3 di `THINKING_TODO.md`, e questo banco la conferma sul campo.**

### S4. ⭐ `continue` è una parola che parrot0 non conosce — e l'adattatore lo spreca

L'adattatore `max_turns`+`continue_text` (§1.2 della sessione scorsa) era
difeso così: *«la continuazione non aggiunge informazione»*. Vero, ma
**sottrae attenzione**. Ecco il turno 2, col binario fresco:

```text
>>> continue
Hmm, I don't know about continue yet. Want me to learn about it? Or teach me:
if it is a kind of thing, say «something is a continue»…
```

Undici turni su dodici sono spesi a chiedere a parrot0 che cosa significhi una
parola che gli abbiamo mandato noi. L'adattatore non sta dando a parrot0 undici
occasioni in più: gliene sta **togliendo undici**.

**Misurato, non scelto a occhio.** Sette continuazioni sotto il profilo della
league:

| frase | risposta |
|---|---|
| `continue`, `next` | *Hi there! What would you like to talk about?* |
| `go on`, `carry on`, `go ahead`, `what next?` | *I don't understand that yet.* |
| `keep going` | *Thanks — I'm learning as we go.* |

**Nessuna è una continuazione.** Non è la parola sbagliata: è che la nozione
«vai avanti col lavoro» **non esiste nella KB**. L'adattatore non si può rendere
equo scegliendone una migliore.

⭐ **E questa è la scoperta che vale più del banco.** F.: *«se gli LLM e i
coding agent hanno la feature continue anche parrot0 la deve avere»*. Cercando
dove metterla si è visto che **era già richiesta da tre punti dei piani, con
tre nomi diversi, e nessuno costruito**: `issue_status(_, open)` (K3),
`plan_unresolved`/i residui (K11), `budget_exhausted` (UC §4 — *«il cammino
esiste ma non è stato completato»*). Design, gate e ordine di lavoro in
[`docs/plans/continue-as-resumption.md`](docs/plans/continue-as-resumption.md),
ipotesi **D49**. Il banco ha fatto il suo mestiere: non ha misurato una
mancanza di parrot0, ne ha nominata una dei piani.

**Chiuso così:** `max_turns: 1` anche per parrot0, `continue_text` vuoto.
L'adattatore sparisce del tutto — anche freebuff è a 1 — quindi la regola §4.2
vale ora senza eccezioni: identico il testo del compito **e** l'interfaccia.
Quando ci sarà B3 (scrivere file) la continuazione tornerà ad avere senso, e
allora va **insegnata KB-first**, non messa nella config del banco.

### S5. Il banco non sapeva contro chi gareggiava — da due lati

- Il `settings.json` di freebuff dice oggi **`minimax/minimax-m3`**, mentre la
  league si chiama `freebuff-deepseek-flash` e l'appendice §3-ter riportava
  `deepseek/deepseek-v4-flash`. Il preflight ora lo vede e **ferma la gara**.
- Il binario di parrot0 era di due generazioni prima (§1.2).

*Un confronto che non sa contro chi ha gareggiato non è un confronto* — e
valeva per entrambi i lati, non solo per l'avversario.

---

## §2-bis. CHE COSA È STATO FATTO OGGI

| | |
|---|---|
| **`selftest`** | `run_challenge.py selftest` — 11 casi contro `fake_agent.py`, **23 s, zero rete, zero agenti veri**. Copre avvio, submit, turni, completamento, blocco, muto, uscita, e i due driver. È il cricchetto chiesto da C6: ogni ipotesi sul pilota ora si falsifica in secondi. |
| **`fake_agent.py`** | Agenti finti nei modi `repl`, `worker`, `tui`, `tui_nobracket`, `silent`, `exit` — ognuno riproduce una forma osservata sul campo, incluso il TUI che riceve il testo ma non lo consegna. |
| **driver `tmux`** | `capture-pane` invece della concatenazione dei byte. Dichiarato per agente (`"driver": "tmux"`); **il PTY resta il default**, perché a un REPL di riga non serve. `meta` ha la stessa forma per entrambi: archivio, judge e scoreboard non sanno quale driver ha corso. |
| **schermate archiviate** | `screens.jsonl` (digest+tempo) e `screens.log` (ogni schermata distinta per intero, con tetto in byte). Per un TUI **il transcript è la successione delle schermate**, non i byte: la regola §4.6 diventa finalmente applicabile. |
| **quiete = schermo immobile** | Segnale migliore dei byte, e non per eleganza: lo schermo di freebuff ha un **cronometro che ticchetta mentre lavora e si ferma quando ha finito**. |
| **preflight** | Dichiarato nella league, non cablato: `must_exist` (login — non ne legge mai il contenuto), `json_key`+`equals` (modello, registrato nel `result.json`), `newer_than_glob` (binario non più vecchio dei sorgenti). Un controllo che fallisce **prima** di avviare vale più di venti `logic_actions` che indovinano dallo schermo. |
| **`argv0_search`** | `$FREE_BUFF_BIN` → `~/.config/manicode/freebuff` → `PATH`, dichiarato invece che indovinato. |
| **`--league FILE`** | `tasks/` e archivi si risolvono accanto alla league: si può tenere una league finta accanto a quella vera senza copiare il pilota. |
| **rimosso** | La `logic_action` `select_default_deepseek_model`: indovinava dallo schermo uno stato che sta scritto in un file. |

**Verifica fatta, non dichiarata:** A/B dei due driver sullo **stesso agente
vero** (parrot0 contro se stesso, `--timeout 45`) — stesso esito, stesso
punteggio, stessi turni. Il driver tmux dà in più il transcript pulito, senza il
rumore dello spinner (`[|] [/] [-] [\]`) che nel PTY riempie il transcript.

---

## §3. ⛔ L'ORDINE IN CUI RIPRENDERE

### C1. ⬅ **PROSSIMO PASSO** — rimisurare freebuff, con tempi veri

Chiuso come diagnosi: freebuff lavora (§1.1). Resta da **correre davvero** con
il driver tmux e il `timeout_seconds: 600` del match — i 45 s erano una
scorciatoia per iterare, e hanno prodotto tre conclusioni sbagliate.
⚠ Prima serve la decisione di §C0.

### C0. ⛔ IL MODELLO — blocca ogni corsa vera, e la palla è a F.

Il preflight ferma la gara: settings dice `minimax/minimax-m3`, la league pinna
`deepseek/deepseek-v4-flash`. **Deciso il 2026-09-04: il pin resta deepseek**, e
va riportato il `settings.json` di freebuff su
`"freebuffModel": "deepseek/deepseek-v4-flash"`. Finché non combaciano,
`run_challenge.py check` lo dice e nessuna gara parte.

### C2. Dare a parrot0 la capacità di scrivere un file

Oggi non può, quindi non può vincere nessun match, mai. È B3 di
`THINKING_TODO.md`. Finché manca, ogni match finisce 0-0 e il banco misura solo
il pilota.
⚠ **E va fatto KB-first**: l'azione «scrivi questo contenuto in questo file» è
uno strumento dichiarato (`local_tool/3` + `tool_argv/2`, gen494), non un ramo
nel C — e passa dal gate del workspace come tutti gli altri.

### C3. Chiudere S1 (il turno rubato)

Un task di coding lungo non deve poter essere rivendicato dal generatore
creativo. È `faculty_yield` — conoscenza, zero C — ma va trovata la **classe**
giusta: non «questo prompt», ma «un turno che porta un compito su una codebase».

### C4. Un match che qualcuno possa vincere

`match1` è difficoltà 4 e chiede quicksort three-way + fallback a profondità
limitata + integrazione Makefile + `-Werror`. Giusto come bersaglio, **inutile
come primo segnale**: se entrambi fanno 0 non si impara niente.
Serve un **match0 di calibrazione** — un solo file, un solo comportamento
verificabile — che freebuff vinca facilmente e che dica *dove esattamente*
parrot0 si ferma. Un banco che dà sempre 0-0 non è un banco.

### ✅ C5. `continue_text` — chiuso

`max_turns: 1` per entrambi, `continue_text` vuoto. Vedi S4 per la misura che
lo motiva. **Ne resta una coda KB, non di banco:** parrot0 non riconosce nessuna
forma di «vai avanti col lavoro». Quando ci sarà B3 quella nozione andrà
insegnata — è una voce per `LEARN_TODO.md` (voce scritta), non per `league.json` — e la lezione da sola
non basta: serve prima la vista `resumable/1` di D49, altrimenti il `continue`
rifà l'ultimo input invece di riprendere il lavoro.

### C6. Cose piccole, con lo stato aggiornato

- ✅ **`cancel_reason`**: verificato, `cancellation` viene scritto in tutti i
  `result.json` archiviati. La voce era stantia.
- ✅ **judge a punteggio parziale**: già così — check nominati con
  `points`/`available_points`, `score` è la somma. Quel che manca è che una gara
  **annullata** non pubblica i parziali, che però restano nel `result.json`.
- ✅ **cricchetto sul pilota**: fatto, `selftest`, 11 casi in 23 s.
- ⛔ **`stream_watch_seconds` ancora a occhio.** Col driver tmux il numero ha
  cambiato significato (schermo immobile, non byte fermi) e i valori messi oggi
  — freebuff 45 s / `idle_settle` 6 s — **sono stime, non misure**. Vanno
  ricavati da una corsa vera lunga.
- ⛔ **`message-history.json` e `projects/`**: freebuff porta stato fra una gara
  e l'altra. Il protocollo azzera `code/` ma non lo stato dell'agente. Da
  decidere: azzerarlo (più equo, meno realistico) o registrarne il digest prima
  e dopo (più onesto, e almeno lo si vede).
- ⛔ **45×160 non è mai stato misurato.** Se il TUI si comporta diversamente a
  160 colonne è una variabile nascosta. Ora almeno è dichiarato per agente
  (`rows`/`cols`) e finisce nel `result.json`.
- ⛔ **`tmux attach` durante una gara**: il driver lo rende possibile ma nessuno
  lo ha ancora usato. Il socket è privato (`-L challenge-<pid>-<ts>`), quindi
  serve `tmux -L <socket> attach`. Vale scriverlo nel README.

### C7. Fuori dal banco, ma trovato dal banco

`make soft-test` è **rosso su HEAD** una volta che `bin/parrot0` è davvero
compilato da HEAD: `frontier_chat_audit.it.p0t`, 53 passati / 3 falliti — tre
casi che attendono *«I'm not sure I followed»* e ricevono *«Hey! I'm here. Ask
me something…»*. Il binario vecchio lo mascherava. Non toccato qui perché è
conoscenza, non banco, ma **va guardato prima di fidarsi di qualunque misura**.

---

## §3-bis. LEZIONI DA `pilot.go` DI OPENCOLA — e la questione tmux

> ⚠ **Sezione storica, con un esito.** L'ipotesi 1 (bracketed paste) è stata
> **falsificata** (§1.1): il paste funzionava. L'argomento per tmux in fondo
> alla sezione era invece quello giusto, e la raccomandazione di provarlo per
> ultimo era sbagliata — **il driver tmux è stato implementato oggi** ed è la
> ragione per cui il banco ora vede qualcosa. Il resto resta valido.

F.: *«appuntati se è il caso di pilotare i coding agent via TMUX, che forse è
più potente e già ci sono riuscito in altri progetti; dai un occhio a
`pilot.go`»*. Letto: `/home/francesco/Develop/_/opencola/cmd/pilot.go`, 426
righe, e pilota freebuff davvero.

### ⚠ Prima correzione: `pilot.go` NON usa tmux

Usa un **PTY** (`github.com/creack/pty`), esattamente come il nostro pilota.
`grep -rn tmux` sul repository non trova niente. Quindi «ci sono già riuscito
via tmux» va letto come «ci sono già riuscito», e il come è lo stesso nostro.
**Questo è utile**: significa che la strada PTY è praticabile e che i nostri
problemi non vengono dal trasporto.

### Le tre cose che `pilot.go` fa meglio, e sono adottabili subito

1. **⭐ Il submit è nudo: `write(testo)` poi `write("\r")`. Nessun bracketed
   paste, nessun ritardo.**

   ```go
   func (p *pilot) injectPrompt(text string) {
       p.ptmx.Write([]byte(text)); p.ptmx.Write([]byte{'\r'})
   }
   ```

   Il nostro fa `\x1b[200~ … \x1b[201~` più `submit_delay_seconds`. **È la
   prima ipotesi da falsificare** per C1: se freebuff non gestisce il bracketed
   paste, il testo può finire in uno stato in cui l'Enter non invia — che è
   esattamente il sintomo (738 redraw, task mai partito, ritorno al prompt).
   *Provare `bracketed_paste: false` costa trenta secondi.*

2. **⭐ Modello, login e binario si leggono dai FILE, non si negoziano col TUI.**

   ```text
   ~/.config/manicode/credentials.json    login fatto?
   ~/.config/manicode/settings.json       "freebuffModel": "deepseek/deepseek-v4-flash"
   ~/.config/manicode/freebuff            il binario
   FREE_BUFF_BIN                          override
   ```

   Verificato ora: **il modello è già impostato nel settings.json.** Quindi la
   nostra `logic_action` che manda ENTER sul selettore modello è, nel migliore
   dei casi, inutile — e nel peggiore manda un invio dove non serve. Da
   rimuovere e sostituire con una **verifica preventiva**: login presente, modello
   atteso, binario trovato. Un preflight che fallisce prima di avviare vale più
   di venti `logic_actions` che indovinano lo stato dallo schermo.

   E il modello va **registrato nel `result.json`**: oggi la league si chiama
   `freebuff-deepseek-flash` e nessuno verifica che sia davvero quel modello.
   Un confronto che non sa contro chi ha gareggiato non è un confronto.

3. **Ricerca del binario robusta** (`FREE_BUFF_BIN` → `~/.config/manicode/` →
   `PATH`), invece del nostro `command_exists("freebuff")`.

### ⭐ E QUI STA IL VERO ARGOMENTO PER TMUX — che non è il pilotaggio

`pilot.go` gestisce `\x1b[?1049h` / `\x1b[?1049l`: freebuff usa lo **schermo
alternato**. Verificato nei nostri log: la sequenza c'è.

**Ne segue che il nostro modo di leggere lo stato è sbagliato per costruzione.**
Noi facciamo `clean_transcript(tutti i byte concatenati)` e cerchiamo un pattern
dentro. Ma in un TUI a schermo pieno **il testo visibile non è la
concatenazione dei byte**: è il risultato di movimenti di cursore, cancellazioni
e ridisegni. Cercare «Enter a coding task» nella concatenazione può trovare un
frame vecchio, e non trovare quello attuale. Con 738 eventi di ridisegno, questa
non è una possibilità teorica.

Per sapere che cosa c'è *davvero* sullo schermo servono due strade:

| strada | costo | cosa dà |
|---|---|---|
| un emulatore di terminale in-process (`pyte`) | una dipendenza Python | lo schermo renderizzato, controllabile |
| **tmux** + `capture-pane -p` | un binario già installato (3.4) | **lo schermo renderizzato, gratis** |

**Il motivo per passare a tmux non è pilotare meglio: è LEGGERE.** `send-keys`
non è più potente di `os.write` su un PTY, ma `capture-pane` risolve esattamente
il problema che abbiamo, e senza scriverci un emulatore. In più regala:
sessione ispezionabile dal vivo (`tmux attach` mentre la gara corre, che per il
debug vale molto), sopravvivenza al crash del pilota, e `wait-for` per la
sincronizzazione.

**Raccomandazione, e va decisa prima di scrivere altro codice:** provare in
quest'ordine, perché il primo costa quasi nulla e potrebbe chiudere C1 da solo.

1. `bracketed_paste: false` sul PTY attuale (30 secondi, falsifica l'ipotesi 1);
2. preflight da file per login/modello/binario, e via la `logic_action` del
   selettore (mezz'ora, toglie una classe intera di fragilità);
3. **se e solo se il riconoscimento dello stato resta inaffidabile**, spostare la
   lettura su `tmux capture-pane` — tenendo il resto del pilota com'è.

⛔ **Da non fare:** riscrivere il pilota in tmux «perché è più potente». Il
trasporto non è il problema — lo dimostra `pilot.go`, che con un PTY nudo
funziona. Il problema è che leggiamo uno schermo come se fosse un flusso.

---

## §3-ter. APPENDICE DI RIFERIMENTO — tutto ciò che serve senza riaprire opencola

Estratto il 2026-09-04 da `/home/francesco/Develop/_/opencola/cmd/pilot.go`
(426 righe) e verificato sul sistema. **Scritto qui perché una sessione futura
non debba dipendere da un altro repository.**

### A. ⛔ freebuff NON ha una modalità headless — verificato

```text
$ ~/.config/manicode/freebuff --help
Usage: freebuff [options] [command]
Arguments:
  command                       Command to run (choices: "login")
Options:
  -v, --version
  --continue [conversation-id]
  --cwd <directory>
  -h, --help
```

**Nessun prompt posizionale, nessun `--api`, nessun `-p`.**

⚠ E qui ho quasi preso un abbaglio, che vale la pena registrare: in
`pilot_test.go` compare `runOptions{apiMode: true, cwd: …, prompt: "fix the
bug"}`, e sembra che freebuff accetti un prompt come argomento. **Sono le
opzioni di opencola, non di freebuff** — `runOptions` è il parser di *quel*
programma. Ho controllato l'`--help` vero prima di scriverlo nella coda.

**Conseguenza sulla raccomandazione del §3-bis:** non esiste una via d'uscita
non interattiva. Il task *deve* passare dal TUI, quindi il problema di **leggere
lo schermo** non è aggirabile e `tmux capture-pane` sale da «opzione 3» a
**strada principale**, se il passo 1 (bracketed paste) non risolve.

### B. Come `pilot.go` avvia e parla con freebuff

```go
cmd := exec.Command(bin, "--cwd", opts.cwd)      // + "--continue" se richiesto
ptmx, _ := pty.Start(cmd)                        // PTY nudo, niente tmux
term.MakeRaw(os.Stdin.Fd())                      // raw mode sul terminale vero
pty.Setsize(ptmx, &pty.Winsize{Rows: rows, Cols: w})

func (p *pilot) injectPrompt(text string) {      // ← il submit, per intero
    p.ptmx.Write([]byte(text))
    p.ptmx.Write([]byte{'\r'})
}
```

Il prompt viene iniettato **subito dopo `resize()`+`drawChrome()`**, senza
attendere nessun pattern di ready. Noi aspettiamo `ready_pattern`: più prudente,
ma se il pattern è sbagliato (come lo era) blocca tutto.

### C. Le sequenze che contano, e perché

```text
\x1b[?1049h   entra nello schermo alternato   ← freebuff lo usa (verificato)
\x1b[?1049l   esce
\x1b[2J       cancella lo schermo
```

`pilot.go` ridisegna la propria barra ogni volta che ne vede una: sono i tre
segnali che «lo schermo è stato rifatto da capo». Per noi sono i punti in cui
**la concatenazione dei byte smette di rappresentare ciò che si vede.**

### D. I file di configurazione di freebuff (stato reale, oggi)

```text
~/.config/manicode/freebuff                 il binario (139 MB)
~/.config/manicode/credentials.json         presente ⇒ login fatto
~/.config/manicode/settings.json            {"freebuffModel": "deepseek/deepseek-v4-flash", …}
~/.config/manicode/message-history.json     storico dei messaggi
~/.config/manicode/projects/                stato per progetto
FREE_BUFF_BIN                               override del binario
```

Ricerca del binario, in ordine: `$FREE_BUFF_BIN` → `~/.config/manicode/freebuff`
→ `PATH`.

⚠ **NON PIÙ VERO al 2026-09-04:** il settings dice `minimax/minimax-m3`. Vedi
S5 e C0. Resta vero che la
`logic_action` che manda ENTER sul selettore è inutile — e il nome della league
(`freebuff-deepseek-flash`) è finalmente verificabile invece che dichiarato.

⚠ `message-history.json` e `projects/` significano che **freebuff porta stato fra
una gara e l'altra**. Il nostro protocollo azzera `code/` ma non lo stato
dell'agente: due match di fila non partono dalle stesse condizioni. Da decidere
— azzerarlo (più equo, meno realistico) o registrarne il digest prima e dopo
(più onesto, e almeno lo si vede).

### E. Cosa fa il nostro pilota che `pilot.go` non fa (e che teniamo)

Non è tutto da copiare: il nostro fa cose che servono a una **gara** e che a un
pilota interattivo non servono.

- `raw.log` + `transcript.txt` + `stream.jsonl` (offset, tempo, digest per chunk);
- `logic-actions.jsonl`: ogni reazione automatica è registrata e limitata negli usi;
- reset di `code/` da `seed/` con digest prima e dopo;
- archiviazione dell'intero albero, non del solo artefatto;
- judge deterministici con check nominati.

`pilot.go` invece dà due cose che noi non abbiamo e che varrebbero: **il
`--continue`** (riprendere una conversazione, utile per i match a più fasi) e
la **visione dal vivo** (guardare mentre lavora), che con tmux verrebbe gratis.

### F. Terminale: i numeri che usiamo

Noi: `45 righe × 160 colonne` fisse (`struct.pack("HHHH", 45, 160, 0, 0)`), scelte
per ridurre il wrapping di freebuff. `pilot.go` usa invece la dimensione reale del
terminale meno 2 righe di cornice. **Le nostre non sono state misurate**: se il
TUI si comporta diversamente a 160 colonne, è una variabile nascosta della gara.

---

## §4. Le regole che questo banco non deve perdere

1. **Nessun nome di agente nel prompt, nel cwd o negli artefatti.** L'artefatto
   si chiama `quicksort.c` per tutti e due.
2. **Il testo del compito è identico byte per byte.** Ciò che può differire è
   solo l'adattatore dell'interfaccia, e va dichiarato nel `result.json`.
3. **Sequenziali, mai simultanei**, e `code/` ricostruita da `seed/` prima di
   ciascuno, con digest registrato.
4. **Si archivia tutto l'albero**, non solo l'artefatto: il come conta quanto il
   cosa.
5. **Un match che nessuno può vincere non misura niente**, e uno che tutti
   vincono nemmeno.
6. **Il transcript è la parte preziosa.** Il punteggio dice chi ha vinto; il
   transcript dice *perché* — ed è per quello che il banco esiste.

---

## §5. Stato dei file

```text
tests/challenge/
  README.md              il protocollo di equità (documentazione, non coda)
  run_challenge.py       il pilota — driver pty|tmux, preflight, selftest
  fake_agent.py          gli agenti finti del selftest (repl/worker/tui/silent/exit)
  run-challenge.sh       check | run | scoreboard | selftest
  league.json            agenti, driver, pattern, preflight, budget di turni
  tasks/match1/          C: quicksort three-way su codebase esistente (diff. 4)
  tasks/match2/          Python: journal/locking/codec (diff. 6)
  freebuff-*/runs/       transcript, schermate, artefatti archiviati, result.json
```

Per agente, dentro `runs/<run-id>/<agente>/`:

```text
  raw.log            i byte, lossless, con entrambi i driver
  transcript.txt     pty: i byte ripuliti — tmux: lo SCHERMO finale renderizzato
  screen.txt         (solo tmux) lo schermo nell'istante in cui la gara è finita
  screens.jsonl      (solo tmux) digest + tempo di ogni schermata distinta
  screens.log        (solo tmux) ogni schermata distinta per intero, con tetto
  stream.jsonl       (solo pty) offset, tempo e digest per chunk
  logic-actions.jsonl ogni reazione automatica, con lo stato che l'ha innescata
  code/              l'albero risultante, archiviato per intero
```

### Come si verifica il banco stesso

```sh
python3 tests/challenge/run_challenge.py selftest     # 11 casi, ~23 s, zero rete
python3 tests/challenge/run_challenge.py check        # config + preflight, non avvia niente
tmux -L challenge-<pid>-<ts> attach                   # guardare una gara mentre corre
```

⚠ **Nessuna corsa di riferimento valida, oggi.** `gen501-pilot-b` va considerata
**non attendibile**: leggeva lo schermo di freebuff come se fosse un flusso
(§1.1) e faceva correre un binario di parrot0 di gen459 (§1.2). Il prossimo run
vero — dopo la decisione C0 sul modello — sarà il primo punto da cui misurare
davvero.
