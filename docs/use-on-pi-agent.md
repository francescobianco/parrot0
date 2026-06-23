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

## Stato dei test

> **Status: FUNZIONANTE PER PROMPT TESTUALI PICCOLI — endpoint verificato via curl e integrazione `pi --print --no-tools` verificata. Non è ancora un coding agent produttivo perché non emette tool calls OpenAI.**

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
