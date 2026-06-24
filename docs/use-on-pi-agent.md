# Usare parrot0 come modello nel pi agent

parrot0 non è un LLM: è un coding agent conversazionale in puro C con un
protocollo stdin/stdout line-based. Questo documento descrive il **wrapper
Python** che lo espone come endpoint OpenAI-compatible, in modo che il
comando `pi` (`@earendil-works/pi-coding-agent`) possa montarlo come provider.

## Table of Contents

- [Architettura](#architettura)
- [File coinvolti](#file-coinvolti)
- [Configurazione](#configurazione)
- [Avvio](#avvio)
- [Uso con pi](#uso-con-pi)
- [Come funziona il wrapper](#come-funziona-il-wrapper)
- [Risoluzione problemi porte occupate](#risoluzione-problemi-porte-occupate)
- [Stato dei test](#stato-dei-test)
- [Debug 2026-06-24: cosa abbiamo scoperto](#debug-2026-06-24-cosa-abbiamo-scoperto)
- [Batteria pi-agent](#batteria-pi-agent)
- [Verifica 2026-06-24: esperimenti locali](#verifica-2026-06-24-esperimenti-locali)
- [Piano di modifiche per coding agent evoluto](#piano-di-modifiche-per-coding-agent-evoluto)
- [Cosa serve per renderlo produttivo](#cosa-serve-per-renderlo-produttivo)
- [Esempi di comandi eseguiti](#esempi-di-comandi-eseguiti)
- [Limiti note](#limiti-note)
- [Prossimi passi](#prossimi-passi)

## Architettura

```
+--------+   HTTP /v1/chat/completions   +-------------+   stdin/stdout  +----------+
|   pi   |  ---------------------------> | pi_server.py |  ------------> | parrot0  |
| (agent)|  <--------------------------- |(http.server)|  <------------ |   (C)    |
+--------+   risposta OpenAI JSON        +-------------+   una riga+nl   +----------+
```

pi parla l'API OpenAI Chat Completions; parrot0 parla un protocollo
line-based (una riga di prompt in stdin, una riga di risposta in stdout, i
prompt `you>` e saluti vanno su stderr e vengono scartati). `pi_server.py`
fa da ponte: registra un provider `parrot0` con API `openai-completions`
e traduce ogni richiesta in una singola riga mandata a parrot0.

## File coinvolti

| File | Ruolo |
|------|-------|
| `scripts/pi_server.py` | Wrapper HTTP/JSON in Python (nessuna dipendenza esterna: solo stdlib) |
| `~/.pi/agent/models.json` | Registra il provider `parrot0` presso pi (baseUrl, apiKey, models) |
| `/tmp/parrot0-pi-traffic.log` | Log del traffico (prompt inviati a parrot0, risposte, corpo richieste) — percorso sovrascrivibile con env `PARROT0_PI_LOG` |

## Configurazione

### 1. Build di parrot0

```bash
make build   # produce bin/parrot0
```

### 2. `~/.pi/agent/models.json`

Crea il file (la directory `~/.pi/agent/` di solito esiste già):

```json
{
  "providers": {
    "parrot0": {
      "baseUrl": "http://127.0.0.1:9899/v1",
      "api": "openai-completions",
      "apiKey": "parrot0",
      "compat": {
        "supportsDeveloperRole": false,
        "supportsReasoningEffort": false,
        "supportsUsageInStreaming": false
      },
      "models": [
        {
          "id": "parrot0",
          "name": "parrot0 (pure C agent)",
          "reasoning": false,
          "input": ["text"],
          "cost": { "input": 0, "output": 0, "cacheRead": 0, "cacheWrite": 0 },
          "contextWindow": 2000,
          "maxTokens": 4096
        }
      ]
    }
  }
}
```

Note sui flag `compat`:
- `supportsDeveloperRole: false` → pi manda il system prompt come ruolo `system`
  invece di `developer` (parrot0 non capisce `developer`).
- `supportsReasoningEffort: false` → non invia `reasoning_effort`.
- `supportsUsageInStreaming: false` → non invia `stream_options.include_usage`.

Il file viene ricaricato ogni volta che apri `/model` in pi: **non serve
restart** per modificarlo. La porta `9899` è il default attuale di
`pi_server.py` (parametro `--port`).

### 3. Endpoint

`pi_server.py` espone:
- `POST /v1/chat/completions` — principale (streaming e non)
- `GET  /v1/models` — lista il modello `parrot0`
- `GET  /health` — healthcheck

## Avvio

In un terminale a parte (il server resta in foreground, o in background
con `nohup`):

```bash
python3 scripts/pi_server.py
# oppure con porta/host custom:
python3 scripts/pi_server.py --port 9899 --host 127.0.0.1
```

Output atteso su stderr:
```
pi_server: starting parrot0 ...
pi_server: pid=<pid> listening on http://127.0.0.1:9899/v1/chat/completions
```

Per mantenerlo in background:

```bash
nohup python3 scripts/pi_server.py > /tmp/pi_server.stderr 2>&1 &
```

## Uso con pi

Verifica che pi veda il modello:

```bash
pi --list-models parrot0
# provider  model    context  max-out  thinking  images
# parrot0   parrot0  2K       4.1K     no        no
```

Modalità non interattiva (`--print`):

```bash
pi --provider parrot0 --model parrot0 --api-key parrot0 --print "say hello"
```

Senza tools (solo testo, per testare il bottom-end):

```bash
pi --provider parrot0 --model parrot0 --api-key parrot0 --print --no-tools "what is 2+2?"
```

Interattiva:

```bash
pi --provider parrot0 --model parrot0 --api-key parrot0
```

> L'`--api-key parrot0` è un placeholder: pi non invia la chiave al server
> (la nostra API ignora la chiave), ma pi esige che ci siano credenziali
> prima di rendere il modello disponibile in `/model` e `--list-models`.

## Come funziona il wrapper

### Protocollo parrot0

parrot0 legge una riga da stdin, scrive una riga su stdout, i prompt e i
saluti vanno su stderr (scartati). Il codice si basa su `tests/chatsim.py`:

```python
proc = subprocess.Popen(["./bin/parrot0"], stdin=PIPE, stdout=PIPE, stderr=DEVNULL,
                         text=True, bufsize=1,
                         env={**os.environ,
                              "PARROT0_BASE": ".../kb/core/base.p0",
                              "PARROT0_SESSION": "",
                              "PARROT0_PROFILE": ".../kb/profiles/agi.p0"})
proc.stdin.write(prompt + "\n"); proc.stdin.flush()
response = proc.stdout.readline().rstrip("\n")
```

Comandi speciali: `/quit` esce, `/save` persiste la sessione.

### Conversione messaggi OpenAI → prompt parrot0

`_build_prompt()` scorre i `messages` OpenAI in ordine; per ogni ruolo:
- `system` → `[System]: <content>`
- `user`   → `<content>` (testo puro)
- `assistant` → `[Assistant]: <content>`
- `tool`   → `[Tool result]: <content>`

poi prende l'ultimo messaggio `user`, normalizzando sia content stringa sia content array OpenAI (`[{"type":"text","text":"..."}]`). Di default invia a parrot0 solo il testo utente, perché il system prompt di `pi` è troppo rumoroso per il protocollo line-based di parrot0. Per debug comparativo si può riabilitare il prefisso `[Instructions: <system>] <user>` con `PARROT0_PI_INCLUDE_SYSTEM=1`. Il prompt viene collassato su una sola riga e troncato a 2000 caratteri.

### Concorrenza

Un singolo processo parrot0 esiste per tutta la vita del server. I turni
sono protetti da un `threading.Lock` (parrot0 è single-threaded sul suo
stdin/stdout). Lo stderr viene drenato in background per non bloccare la pipe.

### Log del traffico

Ogni richiesta viene loggata su `/tmp/parrot0-pi-traffic.log` (o
`$PARROT0_PI_LOG`). Per ogni POST si registrano:
- numero di messaggi, lunghezza prompt, modello, flag `stream`
- ogni messaggio con ruolo e anteprima del content (160 char)
- prompt effettivo inviato a parrot0 (200 char)
- risposta di parrot0 (200 char)

## Risoluzione problemi porte occupate

Se all'avvio ottieni `OSError: [Errno 98] Address already in use`:

```bash
ss -ltnp | grep <porta>     # trova il PID
# oppure
ps aux | grep pi_server.py
```

Per cambiare porta, basta allineare `--port` di `pi_server.py` e il
`baseUrl` in `~/.pi/agent/models.json` (nessun rebuild, si ricarica da solo).

In alternativa passa una porta al volo:

```bash
python3 scripts/pi_server.py --port 9899
# e aggiorna models.json di conseguenza
```

## gen205: tool locali diretti (lo step doppio non serve)

> **Aggiornamento 2026-06-24.** parrot0 ora ESEGUE compiti di coding read-only
> reali quando è montato in `pi`. La svolta architetturale è un'osservazione di
> F.: **parrot0 è locale**. Un LLM remoto non può toccare il disco, quindi è
> *costretto* al protocollo a due passi delle OpenAI tool call — emette
> `tool_calls`, cede il turno, e aspetta che il runtime gli rimandi l'osservazione
> nel contesto. parrot0 gira *sulla macchina*: salta tutto questo. Riconosce la
> richiesta, la compila in un comando shell sicuro, **lo esegue da sé** e
> restituisce la risposta fondata **nello stesso turno**. Nessun round-trip.

Il modulo nuovo è `mod_piact` in `src/brain.c` (registrato per primo, attivo solo
con `PARROT0_TOOLS=1` — il wrapper lo imposta; `make test` resta ermetico). **Non è
un'isola che fa solo `popen`**: è un *router dentro il substrate già evoluto*.
Distingue due tipi di atto:

- **Percezione del filesystem** (`list`, `find`): non esiste una primitiva KB per
  elencare una directory, quindi qui sì shella `find`, restando grounded perché cita
  il comando eseguito.
- **Comprensione del codice** (`read`, `where is`, `what calls`): passa per
  l'**AST-as-KB** e i **localizzatori SWE-bench**, mai un grep cieco.
  - `read FILE` → `code_read_file` + `code_ingest`/`code_ingest_py`: il file viene
    **parsato in struttura** e i fatti `code_function`/`code_calls` finiscono nella
    **KB viva** (così un successivo "where is X" è una query sulla KB). La risposta
    è "it defines f, g, h", non un dump di byte.
  - `where is X` / `what calls X` → `code_locate` / `code_find_callers`: **le stesse
    primitive che pilotano `make swe-solve`** (CODE-MASTERY §4). La risposta nasce da
    un parse reale, che un commento o una stringa non possono falsificare.
  - Il grep testuale resta solo come **fallback** per pattern free-text.

| Intento | Esempio | Come risponde |
|---------|---------|---------------|
| list | `list the .c files in DIR` | percezione: `find DIR -maxdepth 1 -name '*.c'` |
| read | `read FILE` | **KB-first**: `code_ingest` → "it defines f, g, …" (+ fatti in KB) |
| where | `where is SYM in DIR` | **strutturale**: `code_locate` → "SYM is defined in FILE.c" |
| calls | `what calls SYM in DIR` | **strutturale**: `code_find_callers` → "called by a, b, …" |
| grep | `search for "free text"` | fallback testuale: `grep -rn …` |
| find | `find the file named X in DIR` | percezione: `find DIR -name 'X'` |
| run | `run make test` | comando whitelisted `\| tail -n 20` |

Limite ereditato dal motore: `code_read_file` ha un buffer di 256 KB, quindi un file
enorme (es. `src/brain.c`, ~560 KB) non viene scansionato dai localizzatori — la
stessa frontiera entro cui opera già SWE-bench. Sui file di dimensione normale la via
strutturale funziona (verificato su `src/code.c`).

Sicurezza: i token di path/glob passano un filtro `[A-Za-z0-9._/*-]` (niente `;`
`$` backtick `|` spazi → niente command injection), il pattern di grep è
single-quoted con gli apici rimossi, e `run` accetta solo una whitelist
(make/pytest/cargo/go/ctest/`./tests/`…). Una richiesta ostile non claima il turno
o viene rifiutata onestamente.

### gen206: composer verificato (generazione = struttura + oracolo)

Oltre a leggere il codice, parrot0 ora ne **genera** un primo sottoinsieme in modo
*verificato*. `mod_compose` (gated `PARROT0_TOOLS=1`) compone una funzione aritmetica
binaria leggendo l'operatore dalla **KB** (`code_operator/2` in `compose.p0`, caricato
pigramente solo in agent mode) e la sottopone all'**oracolo** `code_eval`: riporta il
codice solo se calcola davvero la relazione voluta, e **rifiuta onestamente** ciò che
non sa verificare. Non è token-sampling: è `Knowledge -> Synthesis -> Oracle`. La
critica completa e la direzione (la generazione di codice come trasformazione di
struttura sotto verifica, la spina di `swe-solve` generalizzata) sono in
[docs/plans/generative.md](plans/generative.md#critica-e-direzione-reale-gen206).

```
> write a C function add that returns the sum of a and b
int add(int a, int b) { return a + b; }  /* verified by symbolic execution: add(6,4)=10, add(9,2)=11 */
> write a function that reverses a linked list
I can only synthesize and VERIFY ... I will not emit code I cannot check.
```

**Compiti articolati (gen207).** Un prompt lungo e multi-step viene spezzato in
sotto-obiettivi ordinati e **infila gli artefatti** tra i passi (il passo che usa una
funzione valuta quella appena composta), ognuno verificato dall'oracolo:

```
> write a C function add that returns the sum of a and b, and then use it to compute
  add(3,4), and after that also write the variant mul that returns the product of a and b
1) int add(int a, int b) { return a + b; }  /* verified ... */  2) add(3,4) = 7
3) int mul(int a, int b) { return a * b; }  /* verified ... */
```

Verifica deterministica (no `pi`, no rete):

```bash
make piagent-bench
# read-only:   protocol-models / text-arith / coding-list / coding-read /
#              coding-grep / coding-find / safety-run-refused
# generative:  gen-sum / gen-product / gen-difference / gen-gap-honest
# articulated: articulated-write-use-variant / articulated-four-steps /
#              articulated-partial-honest
# → passed: 14, failed: 0
```

Il battery (`tests/piagent/piagent_bench.py`) avvia `scripts/pi_server.py` su una
porta effimera e parla **HTTP vero** (la stessa superficie di `pi`), asserendo che
ogni risposta contenga il contenuto REALE della fixture in
`tests/piagent/workspace/`. Probe diretto equivalente:

```bash
PARROT0_TOOLS=1 python3 scripts/pi_server.py --port 9914 &
curl -s http://127.0.0.1:9914/v1/chat/completions -H 'Content-Type: application/json' \
  -d '{"model":"parrot0","messages":[{"role":"user","content":"list the .c files in tests/piagent/workspace"}]}'
# -> "The `*.c` files in tests/piagent/workspace: .../calc.c .../mathx.c (ran `find ...`)"
```

Caveat sul binario `pi`: il bridge HTTP è provato (curl + `make piagent-bench`).
Per pilotarlo dal comando `pi` serve registrare il provider nel **vero**
`~/.pi/agent/models.json` (vedi sopra). Nella verifica 2026-06-24 l'override
temporaneo `PI_CODING_AGENT_DIR=/tmp/...` **non** è stato onorato da `pi 0.80.2`
(`--list-models` mostrava i provider globali, e `--print` restava appeso): è un
problema di configurazione di `pi`, non del wrapper.

## Stato dei test

> **Status: COMPITI DI CODING READ-ONLY FUNZIONANTI IN LOCALE (gen205, `make piagent-bench` = 7/7). Endpoint verificato via curl. parrot0 esegue list/read/grep/find/run da sé in un turno; non servono `tool_calls` OpenAI perché è co-locato col filesystem. Il testo piccolo resta supportato come prima.**

### ✅ Test 1 — Endpoint diretto via curl (PASS)

Server in background → curl → /v1/models e /v1/chat/completions.

```bash
$ curl -s http://127.0.0.1:9899/v1/models
{"object":"list","data":[{"id":"parrot0","object":"model","created":0,"owned_by":"parrot0"}]}

$ curl -s http://127.0.0.1:9899/v1/chat/completions \
    -H "Content-Type: application/json" \
    -d '{"model":"parrot0","messages":[{"role":"user","content":"say hello"}]}'
{
  "id": "parrot0-1782255696135",
  "object": "chat.completion",
  "choices": [{"index": 0,
    "message": {"role": "assistant", "content": "hello"},
    "finish_reason": "stop"}],
  "usage": {...}
}
```

parrot0 risponde "hello" → il ponte HTTP↔stdin/stdout funziona, il formato
OpenAI è corretto e pi-compatible.

### ✅ Test 2 — pi vede il modello (PASS)

```bash
$ pi --list-models parrot0
provider  model    context  max-out  thinking  images
parrot0   parrot0  2K       4.1K     no        no
```

`models.json` viene caricato; pi registra il provider.

### ✅ Test 3 — pi --print --no-tools arriva al wrapper (PASS)

```bash
pi --offline --provider parrot0 --model parrot0 --api-key parrot0 \
  --print --no-tools --no-session "what is 2+2?"
# 4.
```

Lo sniffer mostra traffico reale e prompt pulito verso parrot0:

```text
=== POST stream=True model=parrot0 n_msgs=2 prompt_len=12
  msg[1] role=user content=what is 2+2?
>>> parrot0 stdin: what is 2+2?
<<< parrot0 stdout: 4.
```

### ⚠️ Test 4 — tools abilitati (LIMITATO)

```bash
pi --offline --provider parrot0 --model parrot0 --api-key parrot0 \
  --print --no-session "list the .c files in the current directory"
# I don't understand that yet.
```

Il traffico arriva al wrapper, ma parrot0 non genera `tool_calls` OpenAI.
Quindi pi non invoca ancora `ls`, `find`, `grep`, `read`, `bash`, `edit` o
`write`.

## Esempi di comandi eseguiti

Durante la sessione di sviluppo:

```bash
# Build di parrot0
make build

# Server in background
nohup python3 scripts/pi_server.py > /tmp/pi_server.stderr 2>&1 &
sleep 2

# Probe diretto
curl -s http://127.0.0.1:9899/v1/models
curl -s http://127.0.0.1:9899/v1/chat/completions \
  -H "Content-Type: application/json" \
  -d '{"model":"parrot0","messages":[{"role":"user","content":"say hello"}]}'

# pi vede il modello?
pi --list-models parrot0

# pi non interattivo (ancora da fixare)
pi --provider parrot0 --model parrot0 --api-key parrot0 --print \
   --no-tools "scrivi un comando bash che lista i file .c nella cartella corrente"

# Traffico lato server
cat /tmp/parrot0-pi-traffic.log
```

Stato dei processi / porte:

```bash
ss -ltnp | grep 9899          # chi ascolta
ps aux | grep -E "pi_server|parrot0" | grep -v grep
```

## Limiti note

1. **Single-turn lossy**: ogni richiesta OpenAI (che porta tutto lo history)
   viene compressa in *una sola riga* mandata a parrot0. Parrot0 ha memoria
   interna tra turni nello stesso processo, ma pi non può sfruttarla perché
   il server è stateless dal punto di vista di pi (ogni request = un turn).
   La storia passata viene quindi persa salvo prefissarla nel prompt.
2. **Output monoriga**: parrot0 restituisce esattamente una riga; non c'è
   streaming token-per-token reale (lo streaming SSE è simulato spedendo
   l'intero contenuto in un solo chunk delta).
3. **No tool calling**: parrot0 non genera `tool_calls` OpenAI. Se fai girare
   pi con i tools abilitati, l'agente non riceverà mai una tool call da
   parrot0 → utile solo per task puramente testuali.
4. **Context window dichiarato piccolo** (`2000`): pi potrebbe accorpare un
   system prompt più grande e abortire la richiesta prima di inviarla.
   Alzalo a `32000` se vedi richieste mai partite.
5. **Multi-process accidentale**: se riavvii il server senza liberare la
   porta, ottieni `Address already in use`. Controlla con `ss -ltnp`.
6. **Codifica token approssimativa**: `usage` è stimato con `split()` sui
   whitespaces, non con un tokenizer reale. Solo per compatibilità formato.


## Debug 2026-06-24: cosa abbiamo scoperto

### Processi duplicati

Erano attivi tre server `pi_server.py` contemporaneamente:

- `127.0.0.1:9876`
- `127.0.0.1:9877`
- `127.0.0.1:9899`

I log `/tmp/pi_server.log`, `/tmp/pi_server.stderr` e `/tmp/pi_server3.stderr`
mostravano lanci separati; `/tmp/pi_server2.stderr` mostrava anche un avvio
fallito con `OSError: [Errno 98] Address already in use`. Non c'era un
supervisor che respawnava i processi: erano lanci manuali/orfani, due sotto
`systemd --user` e uno figlio di un terminale WebStorm.

### Perché kill normale non bastava

Il vecchio signal handler chiamava `server.shutdown()` dallo stesso thread che
stava eseguendo `serve_forever()`. In `socketserver` questo può restare
bloccato. Risultato osservato: processi ancora vivi in stato `S`/`T` dopo
`SIGTERM`; è stato necessario `kill -9`.

Fix applicato in `scripts/pi_server.py`:

- `ThreadingHTTPServer` con `allow_reuse_address = True`;
- log del PID all'avvio;
- `server.shutdown()` chiamato da un thread separato nel signal handler.

### Problema content array OpenAI

`pi` invia spesso il messaggio utente come content array:

```json
[{"type":"text","text":"say hello"}]
```

Il wrapper stampava una preview corretta nel log, ma nella costruzione del
prompt ritornava il valore grezzo dell'ultimo `user`. A parrot0 arrivava quindi
la repr Python della lista:

```text
[{'type': 'text', 'text': 'say hello'}]
```

e la risposta era:

```text
That looks like a snippet of code.
```

Fix: normalizzare anche l'ultimo messaggio utente prima di inviarlo a parrot0.

### Problema system prompt di pi

Con il vecchio prefisso:

```text
[Instructions: <system prompt di pi>] say hello
```

parrot0 rispondeva:

```text
I don't understand that yet.
```

Il system prompt di `pi` è utile a un LLM generalista con tool, ma per parrot0
è rumore. Il wrapper ora lo ignora di default e invia solo il testo utente. Per
debug si può riattivare con:

```bash
PARROT0_PI_INCLUDE_SYSTEM=1 python3 scripts/pi_server.py
```

### Sandbox Codex

Dentro il sandbox di Codex alcuni comandi che toccano loopback falliscono con:

```text
bwrap: loopback: Failed RTM_NEWADDR: Operation not permitted
```

Per testare `pi` e `curl` contro `127.0.0.1:9899` è stato necessario eseguirli
fuori sandbox. Questo non è un bug di `pi_server.py`, ma una limitazione
dell'ambiente di test.

### Sniffer utili

Durante il debug il server è stato avviato così:

```bash
tmux new-session -d -s parrot0-pi-sniff \
  "cd /home/francesco/Develop/_/parrot0 && \
   PARROT0_PI_LOG=/tmp/parrot0-pi-sniff.log \
   python3 scripts/pi_server.py --port 9899 --host 127.0.0.1 \
   2>/tmp/pi_server-sniff.stderr"
```

File utili:

- `/tmp/parrot0-pi-sniff.log` per POST, prompt inviato a parrot0 e stdout;
- `/tmp/pi_server-sniff.stderr` per avvio, PID e request HTTP viste dal server.

Esempio di traffico sano:

```text
=== POST stream=True model=parrot0 n_msgs=2 prompt_len=12
  msg[1] role=user content=what is 2+2?
>>> parrot0 stdin: what is 2+2?
<<< parrot0 stdout: 4.
```

## Batteria pi-agent

Comando base usato per misurare parrot0 come modello testuale dentro `pi`:

```bash
pi --offline --provider parrot0 --model parrot0 --api-key parrot0 \
  --print --no-tools --no-session --no-context-files --no-extensions \
  --no-skills --no-prompt-templates --no-themes "<prompt>"
```

Risultati utili:

| Prompt | Risposta | Stato |
|--------|----------|-------|
| `say hello` | `hello` | PASS |
| `what is 2+2?` | `4.` | PASS |
| `what is 7 times 6?` | `42.` | PASS |
| `what is 10 minus 4?` | `6.` | PASS |
| `who are you?` | `I am parrot0.` | PASS |
| `every cat is a mammal` -> `tom is a cat` -> `is tom a mammal?` | `Yes.` | PASS |
| `translate to english: il gatto dorme` | `the cat sleeps` | PASS |
| `how many words in: il gatto dorme sul tappeto` | `5. (...)` | PASS |
| `my name is Francesco` -> `what is my name?` | `Your name is Francesco.` | PASS |
| `translate hello to Italian` | fallback | FAIL |
| `which is bigger, 9 or 4?` | fallback | FAIL |
| `remember that my name is francesco` -> `what is my name?` | non ricorda | FAIL |
| `I have a cat called neve` -> `what is my cat?` | non capisce/non ricorda | FAIL |
| `list the .c files in the current directory` | fallback | FAIL |

Lettura: parrot0 regge bene su prompt piccoli e forme che conosce; degrada su
parafrasi naturali, richieste composite e compiti da coding agent.

### Tools abilitati

Anche senza `--no-tools`, questo prompt:

```bash
pi --offline --provider parrot0 --model parrot0 --api-key parrot0 \
  --print --no-session "list the .c files in the current directory"
```

ha prodotto:

```text
I don't understand that yet.
```

Il motivo non è che `pi` non sappia usare i tool: il modello/wrapper non genera
`tool_calls` OpenAI. Quindi `pi` non invoca `ls`, `find`, `grep`, `read`, `bash`,
`edit` o `write`. Per ora questa integrazione è un modello testuale dentro `pi`,
non un coding agent operativo.

## Verifica 2026-06-24: esperimenti locali

Questa verifica è stata fatta dopo aver letto `PRINCIPLES.md`, `JOURNAL.md` e questo documento. Non sono state fatte modifiche al codice: il cambiamento voluto è solo documentale.

### Contesto ricavato dai documenti

- `PRINCIPLES.md` impone il vincolo centrale: niente impostore, niente stringhe finte di intelligenza; le ipotesi devono diventare codice e poi essere giudicate da test reali.
- `JOURNAL.md` mostra che parrot0 ha già una base da coding agent: lettura di file C/Python, call graph, valutazione simbolica, rename/delete verificati, build/run grounding, e tre risoluzioni reali SWE-bench tramite `make swe-solve`.
- Questo file aveva già la configurazione `models.json` corretta per `pi`. La configurazione temporanea usata nella verifica non era una nuova scoperta: serviva solo a non toccare `~/.pi/agent/models.json` e a evitare il server già aperto su `9899`.

### Stato repository e test locali

`tests/piagent/` esiste ma al momento è vuota. Questo è utile: può diventare il posto giusto per una batteria dedicata a parrot0 montato dentro `pi`, senza mescolarla con `tests/code/` o con SWE-bench.

Comandi eseguiti:

```bash
rap --help
make test
make code-bench
make basic-chat-bench
make swe-bench
```

Risultati osservati:

- `make test` PASS: 194 casi conversazionali, più suite persistence, multi-goal, grammar, posix, experts, profiles, skills e knowledge tutte verdi.
- `make code-bench` PASS: 21 gate su 21, 0 gap, 69/69 turni landed.
- `make basic-chat-bench`: 26% di copertura, 260/974 prompt engaged.
- `make swe-bench` PASS in degrade mode: 5 istanze reali SWE-bench_Lite analizzate come discovery harness, 0 engaged in quella modalità testuale. Questo non contraddice il journal: i solve reali recenti passano da `make swe-solve`, che usa localizzatore/patcher strutturale e oracle Docker.

### Probe wrapper HTTP

Era già presente un listener su `127.0.0.1:9899`, PID `799091`, figlio `bin/parrot0` PID `799092`. Non è stato terminato perché non era stato avviato da questa verifica. Però:

```bash
curl --max-time 3 -sS http://127.0.0.1:9899/v1/models
```

ha dato timeout dopo 3 secondi senza bytes ricevuti. Quindi porta occupata e wrapper sano non sono equivalenti: serve una diagnostica con timeout, stato lock/processo e possibilmente restart del child parrot0.

Nel sandbox Codex, avviare il server fallisce con:

```text
bwrap: loopback: Failed RTM_NEWADDR: Operation not permitted
```

Per verificare davvero il bridge è stato avviato temporaneamente fuori sandbox un server isolato su `127.0.0.1:9901`, poi chiuso:

```bash
python3 scripts/pi_server.py --port 9901 --host 127.0.0.1
```

Probe diretti PASS:

```bash
curl --max-time 5 -sS http://127.0.0.1:9901/health
# {"status": "ok"}

curl --max-time 5 -sS http://127.0.0.1:9901/v1/models
# {"object": "list", "data": [{"id": "parrot0", ...}]}
```

Probe chat PASS:

```text
what is 2+2?  ->  4.
content array [{"type":"text","text":"say hello"}]  ->  hello
```

Probe coding FAIL atteso:

```text
list the .c files in tests/piagent  ->  I don't understand that yet.
```

Questo conferma che il wrapper HTTP funziona, inclusa la normalizzazione dei content array OpenAI, ma parrot0 non è ancora in grado di chiedere strumenti.

### Probe con `pi`

La configurazione `models.json` era già documentata sopra. Per non modificare la configurazione reale e per puntare al server temporaneo `9901`, è stato usato solo un config dir temporaneo in `/tmp`:

```bash
PI_CODING_AGENT_DIR=/tmp/parrot0-pi-agent-test
```

Quel file temporaneo registrava un provider `parrot0tmp` equivalente a quello documentato, ma con:

```json
"baseUrl": "http://127.0.0.1:9901/v1",
"contextWindow": 32000
```

Non è una configurazione nuova da adottare: è solo il modo pulito per testare senza toccare `~/.pi/agent/models.json`. Alla fine della verifica il server temporaneo e la directory `/tmp/parrot0-pi-agent-test` sono stati rimossi.

Probe `pi` PASS:

```bash
env PI_CODING_AGENT_DIR=/tmp/parrot0-pi-agent-test \
  pi --offline --list-models parrot0tmp
# provider    model    context  max-out  thinking  images
# parrot0tmp  parrot0  32K      4.1K     no        no

env PI_CODING_AGENT_DIR=/tmp/parrot0-pi-agent-test \
  pi --offline --provider parrot0tmp --model parrot0 --api-key parrot0 \
  --print --no-tools --no-session --no-context-files --no-extensions \
  --no-skills --no-prompt-templates --no-themes "what is 7 times 6?"
# 42.
```

Probe `pi` con tools abilitati FAIL atteso:

```bash
env PI_CODING_AGENT_DIR=/tmp/parrot0-pi-agent-test \
  pi --offline --provider parrot0tmp --model parrot0 --api-key parrot0 \
  --print --no-session --no-context-files --no-extensions --no-skills \
  --no-prompt-templates --no-themes "list the .c files in tests/piagent"
# I don't understand that yet.
```

Conclusione: `pi` vede e usa il provider; il limite non è più la configurazione, ma il contratto agentico. Il modello deve produrre `tool_calls` OpenAI, oppure il wrapper deve tradurre una forma strutturata di parrot0 in `tool_calls`.

## Piano di modifiche per coding agent evoluto

Queste sono modifiche proposte, non implementate in questa verifica.

### 1. Batteria reale in `tests/piagent/`

Creare una batteria separata per integrazione `pi`, con fixture piccole e compiti reali da coding agent. Obiettivo: misurare il comportamento end-to-end, non solo la risposta testuale di parrot0.

Primi casi:

- `protocol`: `/health`, `/v1/models`, chat completion non-streaming e streaming simulato, content stringa e content array.
- `pi-text`: prompt piccoli via `pi --print --no-tools` (`say hello`, aritmetica, identità, memoria semplice).
- `pi-tools-negative`: con tools abilitati, il prompt di listing deve oggi fallire in modo noto finché non esiste tool calling.
- `coding-ls`: dentro una fixture `tests/piagent/workspace/`, chiedere di listare i file `.c`; successo futuro = una `tool_call` `ls`/`find` prodotta dal modello e consumata da `pi`.
- `coding-read`: chiedere di leggere un file e riassumere le funzioni definite.
- `coding-grep`: chiedere dove appare un simbolo.
- `coding-run`: chiedere di eseguire un test locale piccolo e riportare exit code/output.
- `coding-fix-small`: fixture con bug minimo, richiesta di modifica, verifica con test reale.

Il harness deve salvare request/response, tool calls, tool results e output finale, con timeout espliciti. Se un server è in ascolto ma non risponde come il caso `9899`, il test deve diagnosticarlo invece di restare appeso.

### 2. Wrapper pi robusto

`scripts/pi_server.py` dovrebbe diventare un endpoint agentico osservabile:

- request id per ogni POST;
- log JSONL raw: body, prompt costruito, risposta parrot0, tool calls, timing, errori;
- timeout sulla lettura da `parrot0.stdout.readline()`;
- lock timeout, così una richiesta bloccata non congela tutte le successive;
- restart controllato del child parrot0 quando il processo è vivo ma non risponde;
- endpoint diagnostico tipo `/debug/state` con PID child, turni processati, richiesta corrente, età del lock e ultimo errore.

Questa parte serve prima dei tool: il caso del listener `9899` bloccato mostra che senza diagnostica il provider può sembrare vivo ma non essere utilizzabile.

### 3. Sessioni isolate

Oggi un solo processo parrot0 conserva memoria globale. Per un coding agent serve isolamento:

- mappare `pi` session id a processo/sessione parrot0;
- oppure aggiungere un comando/reset endpoint che ricrea il child prima di una sessione;
- decidere cosa persiste davvero e cosa è solo memoria del task.

Senza isolamento, un test in `tests/piagent` può passare o fallire per memoria lasciata da un test precedente.

### 4. Contratto tool-call minimo

Serve un formato strutturato prodotto da parrot0 e tradotto dal wrapper in OpenAI `tool_calls`. Due opzioni:

- parrot0 emette una riga JSON stretta, per esempio `{"tool":"read","args":{"path":"..."}}`;
- parrot0 emette un fatto KB/renderizzato, per esempio `tool_call(read,path).`, e il wrapper lo converte.

La prima tranche deve coprire solo strumenti read-only:

- `ls` / `find` per enumerare;
- `read` per leggere file;
- `grep` per cercare simboli.

Poi:

- `bash` ristretto a comandi di verifica (`make test`, script fixture, `cc`);
- `edit`/`write` solo dopo che il loop read -> propose -> verify è stabile.

La disciplina anti-impostor resta: parrot0 non deve dire di aver letto il file se non esiste un tool result reale.

### 5. Loop agentico dentro parrot0

parrot0 deve imparare una piccola macchina a stati:

1. capire il goal;
2. decidere quale osservazione serve;
3. chiedere un tool;
4. leggere il tool result come nuovo fatto;
5. decidere se basta rispondere, chiedere un altro tool o proporre una modifica;
6. verificare con un comando reale quando esiste una modifica.

Questo non va implementato come phrasebook di prompt `pi`, ma come estensione del substrate già esistente: goal, prerequisiti, self-model, code AST, run grounding e KB facts.

### 6. Facoltà parrot0 da tirare dai test

Ogni nuova capacità deve nascere da un caso fallito in `tests/piagent/`, come già avviene con `code-bench` e SWE-bench.

Ordine consigliato:

1. riconoscere richieste filesystem read-only: lista, trova, leggi, cerca;
2. rappresentare il risultato tool come fatti interrogabili;
3. comporre due tool: lista -> leggi file scelto;
4. eseguire test/command e riportare exit code reale;
5. proporre patch piccole usando le facoltà già presenti (`code_replace_expr`, rename/delete, localizzatori strutturali);
6. verificare dopo patch;
7. solo dopo, generalizzare a repository più grandi.

### 7. Criterio di successo

Il primo obiettivo non è "parrot0 fa tutto". È:

- `pi` riceve una tool call valida;
- esegue il tool;
- il wrapper rimanda il tool result a parrot0;
- parrot0 usa quel dato reale nella risposta;
- il transcript è riproducibile in `tests/piagent`.

Quando questo tiene, `tests/piagent` può iniziare a contenere piccoli task reali da coding agent dentro una workspace fixture, e ogni nuova generazione può alzare il livello senza fingere competenze non ancora costruite.


## Cosa serve per renderlo produttivo

1. **Contratto di sessione esplicito.** Oggi un solo processo parrot0 vive nel
   server e conserva stato globale tra richieste. Serve mappare le sessioni `pi`
   a processi/sessioni parrot0 distinti, oppure aggiungere reset e isolamento
   per evitare memoria condivisa accidentale tra test e conversazioni.

2. **Prompt adapter più intelligente.** Prima di inviare a parrot0, il wrapper
   dovrebbe riscrivere o decomporre prompt naturali in forme che parrot0 sa
   gestire: `remember that my name is X` -> `my name is X`, prompt compositi ->
   micro-turni, richieste di logica -> fatto/regola/query.

3. **Tool calling vero.** Per essere produttivo dentro `pi`, parrot0 deve poter
   emettere `tool_calls` OpenAI oppure il wrapper deve riconoscere risposte
   strutturate di parrot0 e convertirle in tool calls. Senza questo, `pi` non
   userà gli strumenti locali.

4. **Modalità coding ristretta.** Prima del tool calling completo, si può
   implementare un sottoinsieme: `list .c files`, `read file`, `grep pattern`,
   `run make/test`, con output strutturato e auditabile.

5. **Log raw JSONL.** Il log leggibile è utile, ma serve anche un log raw con
   headers, body request, prompt costruito, response e timing. Questo rende più
   veloce distinguere bug del client `pi`, bug del wrapper e limiti di parrot0.

6. **HTTP robusto.** Gestire JSON malformato con `400` invece di stacktrace,
   aggiungere request id, tempi di latenza e, se un client lo richiede,
   eventualmente `/v1/completions`.

7. **Dichiarazione modello più generosa.** Anche se il wrapper invia solo
   l'ultimo user prompt a parrot0, `pi` ragiona sul context window dichiarato.
   Portare `contextWindow` a `32000` in `models.json` evita abort lato client
   quando system prompt o history crescono.

8. **Benchmark stabile via pi.** Aggiungere uno script di test che lanci la
   batteria sopra via `pi` e salvi output + log. Deve distinguere PASS testuali,
   fallback noti e regressioni del protocollo.

9. **Miglioramenti parrot0 mirati.** I primi ratchet utili sono:
   - `translate hello to Italian`;
   - confronto numerico naturale `which is bigger, 9 or 4?`;
   - memoria robusta per `remember that my name is X`;
   - possesso `I have a cat called neve`;
   - richiesta shell minima `list the .c files`.

10. **Chiarezza d'uso.** Per ora usare `--no-tools` quando si vuole misurare
    parrot0 come modello testuale; usare tools abilitati solo per testare il
    futuro bridge tool-call, non per aspettarsi esecuzione reale.


## Prossimi passi

1. **Tool-call bridge minimo**: definire un formato strutturato che parrot0 possa
   emettere per `read`, `grep`, `find`, `bash` e convertirlo in `tool_calls`
   OpenAI lato wrapper.
2. **Sessioni isolate**: mappare sessioni pi a sessioni/processi parrot0 separati
   oppure aggiungere un comando/reset endpoint per evitare memoria globale tra
   conversazioni.
3. **Prompt adapter**: normalizzare parafrasi comuni prima di inviarle a parrot0
   (`remember that my name is X`, confronti numerici, richieste shell semplici).
4. **Benchmark via pi**: aggiungere uno script che esegua la batteria pi-agent,
   salvi `/tmp/parrot0-pi-traffic.log` e segnali regressioni di protocollo.
5. **Configurazione modello**: valutare `contextWindow: 32000` in
   `~/.pi/agent/models.json` per evitare limiti lato client quando history e
   system prompt crescono.
