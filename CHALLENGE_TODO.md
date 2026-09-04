# CHALLENGE_TODO — far funzionare il banco di gara, e farlo scoprire cose

> **HANDOFF completo del 2026-09-04.** Sessione interrotta perché F. doveva
> andare. Qui c'è dove siamo arrivati, **che cosa ho scoperto guardando i byte
> veri**, e l'ordine esatto in cui riprendere.
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

## §1. ⛔ LO STATO VERO: il pilota ora avvia gli agenti, ma nessuno dei due lavora

Ultima corsa (`gen501-pilot-b`, timeout ridotto a 45s per iterare in fretta):

| agente | durata | fine | turni | ha toccato `code/` |
|---|---|---|---|---|
| parrot0 | 32,4 s | `completion_marker` | 12 | **no** |
| freebuff | 49,3 s | `timeout` | 1 | **no** |

`result_tree_sha256 == seed_sha256` per entrambi: **la cartella è intatta.** Il
banco è ancora a zero scoperte perché nessuno dei due ha prodotto un artefatto.

### 1.1 I tre difetti del pilota che ho chiuso oggi

1. **⭐ `ready_pattern` era `you>`, ma in un PTY parrot0 stampa `>>>`.**
   Il pilota aspettava un prompt che **in un terminale non arriva mai**: la gara
   veniva annullata come *«stream fermo all'avvio»*, e sembrava che parrot0
   fosse rotto. È il difetto più banale e il più costoso della serie — tre
   corse buttate prima che qualcuno guardasse i byte veri.
   **Causa a monte, ora rimossa:** parrot0 aveva *due* prompt diversi, `>>>`
   col colore e `you> ` senza. F.: *«mi sembra una cazzata, il prompt è `>>>`,
   che sarà colorato o no a seconda dell'ENV»*. Unificato in `prompt_str()`.
   ⚠ **Fallout da sorvegliare:** nove script/bench che spogliavano `you> ` sono
   stati aggiornati meccanicamente. Possono essercene altri (`.p0t`, README,
   docs): `grep -rn 'you>'` prima di fidarsi.

2. **La fine sessione scattava sul prompt, non sulla quiete.** `done_pattern`
   per parrot0 *è* il suo prompt: bastava che riapparisse dopo il submit perché
   il pilota dichiarasse «finito» — 7 secondi, un turno solo, zero file.
   Ora servono **due condizioni insieme**: il marcatore in **coda** a ciò che si
   vede, **e** lo stream fermo per `idle_settle_seconds`. Un prompt ristampato
   in mezzo al lavoro non chiude più niente.

3. **«Stream fermo» non distingueva *in attesa* da *bloccato*.** Ora se il
   prompt è in coda il silenzio è attesa, non blocco, e la gara non viene più
   annullata a torto.

### 1.2 E il quarto, che è un ADATTATORE e va dichiarato

Un agente REPL non finisce un compito in un turno. Ho aggiunto `max_turns` +
`continue_text` (per parrot0: 12 turni, continuazione neutra `continue`).

⚠ **Questa è la scelta più discutibile del banco e va difesa o cambiata, non
lasciata implicita.** La difesa: il **testo del compito** resta identico byte per
byte e la continuazione **non aggiunge informazione**; è l'interfaccia a
differire, non ciò che l'agente sa. Il rischio: dodici «continue» sono dodici
occasioni che l'altro non ha. `turns_used` finisce nel `result.json` proprio
perché un confronto in cui un lato ha avuto più giri **deve dirlo**.

---

## §2. ⭐ LE SCOPERTE SU PARROT0 — questo è il prodotto del banco

Già utili, e sono la ragione per cui il banco vale anche mentre è rotto.

### S1. Un task di coding lungo viene rubato dal generatore di storie

Transcript `gen500-stream-v3`, prompt da 1485 byte su quicksort:

```text
> [task di coding, 1485 byte]
    It was a mysterious it. Then one day, it discovered what it meant to be
    seen. it had never felt this way before — as if the whole world had shifted…
```

Un modulo creativo rivendica il turno. È **mantra #17** su un input reale: una
facoltà che non sa onorare la richiesta non tace, produce. Peggiore di un muro.

### S2. Il thinking peggiorava la risposta, in diretta

Nello stesso transcript, il pensiero 2 ha trasformato l'uscita in *«What number
should I use for «it»?»*. È la voce **A0** di `THINKING_TODO.md`: la guardia
prende i muri, non le degradazioni.
**Azione presa:** `PARROT0_THINKING=0` nella league, con la ragione scritta nel
config. *Misurare parrot0 con una funzione nota-rotta non è misurare parrot0.*
Si riaccende quando A0 è chiusa — ed è allora un ottimo esperimento (E4).

### S3. Dodici turni, nessun file toccato

Con il pilota corretto parrot0 ha avuto 12 turni e non ha creato nulla. **Non è
un difetto del banco: è il risultato.** parrot0 non ha il ciclo
`esplora → decidi → scrivi → verifica`; ha un piano su strumenti (gen495) che è
*read-only* e non sa scrivere file.
→ **è la voce B3 di `THINKING_TODO.md`, e questo banco la conferma sul campo.**

---

## §3. ⛔ L'ORDINE IN CUI RIPRENDERE

### C1. Far lavorare freebuff (senza questo, non c'è confronto)

Va per timeout senza toccare niente, con 738 eventi di stream: **il TUI ridisegna
ma il task non parte**. Da verificare, in quest'ordine:
- il submit con bracketed paste arriva nell'editor vero o nel selettore modello?
  (`logic-actions.jsonl` dice che l'ENTER del selettore parte; non dice che
  l'editor abbia ricevuto il testo);
- il prompt da 1485 byte incollato in un TUI: viene troncato? va a capo? serve
  un invio finale separato?
- `freebuff --help`: esiste una modalità **non interattiva** (`-p`, `--prompt`,
  headless)? **Se esiste, si usa quella**: pilotare un TUI via PTY è la fonte di
  fragilità numero uno di tutto il banco.

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

### C5. Il judge deve dare punteggio parziale

Oggi `total_points` è `None` quando la gara è annullata, e i check sono
tutto-o-niente. Servono **check nominati a punti parziali** (compila / esporta il
simbolo / passa il caso base / passa il caso avverso), così un agente che arriva
a metà lo si vede. Senza gradazione non c'è curva di apprendimento fra i match.

### C6. Cose piccole e vere

- `cancel_reason` è `null` anche quando la gara è annullata: il campo non viene
  scritto.
- `stream_watch_seconds` è per-agente ma i valori sono stati messi a occhio
  (parrot0 8s, freebuff 20s): **vanno misurati**, non indovinati.
- Nessun cricchetto sul pilota stesso: un `--dry-run` con un agente finto
  (`cat`/`echo`) che verifichi avvio, submit, turni e completamento **senza rete
  e senza agenti veri** renderebbe iterabile tutto il resto in secondi invece
  che in minuti. **Probabilmente è la cosa da fare per prima**, perché rende
  tutte le altre più veloci.

---

## §3-bis. ⭐ LEZIONI DA `pilot.go` DI OPENCOLA — e la questione tmux

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
  run_challenge.py       il pilota — corretto oggi su prompt/quiete/turni
  run-challenge.sh       check | run | scoreboard
  league.json            agenti, pattern, env, budget di turni
  tasks/match1/          C: quicksort three-way su codebase esistente (diff. 4)
  tasks/match2/          Python: journal/locking/codec (diff. 6)
  freebuff-*/runs/       transcript, artefatti archiviati, result.json
```

Corsa di riferimento più recente: `gen501-pilot-b` — **la prima in cui entrambi
gli agenti partono davvero.** È il punto da cui misurare i progressi.
