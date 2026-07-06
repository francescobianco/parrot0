# parrot0 come motore di inferenza MCP — piano operativo

> **Stato:** scritto a gen275; **gen277 (2026-07-06) ha spedito il motore
> funzionante** — trasporto JSON-RPC-su-stdio + 16 tool (§5) + il driver live
> `scripts/mcp-live.sh`. Gli esperimenti di addestramento dal vivo sono provati
> a mano (assert fatto+regola → parrot0 deriva e verbalizza una conclusione
> nuova; text.extract→kb.save→kb.restore→query). Vedi `docs/use-mcp-engine.md`
> per come guidarlo. Restano da §8: `PARROT0_BARE` (gen F) e l'eventuale
> trasporto HTTP. Il ratchet `tests/mcp.sh` copre solo il TRASPORTO (handshake +
> tools/list), NON l'addestramento — quello si fa dal vivo, non come test.
> **Ruolo:** disegno completo di una NUOVA modalità (`parrot0 --mcp-engine`) che
> espone il motore Prolog-like e le primitive di generazione di parrot0 come
> server MCP (Model Context Protocol) — così un agente esterno (Claude Code, pi,
> qualunque client MCP) può usare la KB di parrot0 come un **database di
> conoscenza + inferenza + generazione**: salvare fatti, interrogarli, farci
> inferenza, e chiamare le stesse primitive di generazione che parrot0 usa
> internamente. Non è un cambio di direzione: è un NUOVO PONTE, sorella di
> `--daemon` (gen221, `src/serve.c`), non un sostituto.

---

## 0. La proposta di F. (testuale, 2026-07-06)

> dobbiamo implementare su parrot0 una nuova modalità: si tratta del servizio di
> inferenza Prolog evoluto con primitive anche di generazione, le stesse che
> parrot0 usa internamente. In pratica se avviamo `parrot0 --mcp-engine` parte un
> server MCP che gli agenti possono usare per salvare informazioni e inferire su
> di esse. Le informazioni del profilo presenti nella KB sono caricate a meno che
> non si avvii con variabili opportune. Lo scopo è avere un agente che pilota il
> motore di inferenza per poterlo potenziare. Questa inferenza sarà tipo un DB:
> ci saranno API di scrittura, di lettura, di inferenza, di generazione, di
> impostazione della temperatura, di parsing di testi e di estrazione di
> conoscenza da testi.

Decomposizione dei sette pezzi richiesti, con la primitiva C che già li implementa
(§2 mappa il dettaglio):

| Pezzo richiesto | Primitiva già esistente |
|---|---|
| scrittura | `kb_assert` / `kb_assert_rule_n` / `kb_retract` / `kb_assert_neg` |
| lettura | `kb_query` / `kb_match` / `kb_describe_entity` / `kb_dump_all` |
| inferenza | `kb_explain` (SLD + proof trace), `kb_induce`, `kb_rule_body_preds` (abduzione) |
| generazione | `brain_respond` (il turno completo), `next_word_ctx` (continuazione grezza) |
| temperatura | `style_temperature(N)` (gen226, già un fatto KB — vedi `kb_response` in `00-lex.c`) |
| parsing di testi | `normalize`/`canonicalize_lang` (NL), `code_ingest`/`code_ingest_py` (codice) |
| estrazione di conoscenza | `extract_clause` (già dietro il prefisso `read:`, gen41/gen121) |

**La scoperta di fondo, che semplifica tutto:** parrot0 non deve inventare
un'API — deve solo **esporre in JSON-RPC quello che il motore già fa**. Ogni
pezzo della lista di F. è una funzione C esistente, testata, con un contratto
scritto in un header (`kb.h`, `brain.h`, `learn.h`, `code.h`). Il lavoro reale è
(a) un layer di trasporto MCP minimale e (b) sette-otto adattatori sottili che
traducono una chiamata JSON-RPC in una chiamata C e il risultato in JSON.

---

## 1. Di cosa si tratta — MCP in breve (per chi non lo conosce)

MCP (Model Context Protocol) è un protocollo JSON-RPC 2.0 con cui un **host**
(l'agente, es. Claude Code) parla a un **server** (qui: parrot0) che espone
`tools` (funzioni chiamabili) e opzionalmente `resources` (dati leggibili). Il
trasporto più comune per un server locale è **stdio**: l'host lancia il processo
come sottoprocesso e scambia messaggi JSON-RPC riga per riga su stdin/stdout —
esattamente la topologia che `main.c` già usa per il REPL di chat (leggi una
riga, scrivi una riga, flush). Non serve un client HTTP, non servono socket:
è un fit strutturale naturale con l'I/O shell esistente di parrot0.

Il ciclo di vita minimo che un server MCP deve rispondere:
1. `initialize` → il server dichiara nome/versione/capabilities.
2. (host manda `notifications/initialized`, nessuna risposta richiesta)
3. `tools/list` → il server elenca i tool disponibili con il loro JSON Schema.
4. `tools/call` → l'host invoca un tool per nome con argomenti; il server
   risponde con un risultato (o un errore).

Questo è TUTTO il protocollo che serve implementare per un server funzionante:
niente resources, niente prompts, niente sampling lato server — quel sottoinsieme
è quanto basta per il caso d'uso di F. (un agente che chiama funzioni sulla KB).

---

## 2. Mappa di ciò che esiste già (non riscrivere nulla)

parrot0 ha già tre pezzi dell'infrastruttura necessaria, pronti al riuso:

| Componente | File | Cosa offre al nuovo modulo |
|---|---|---|
| **Parser JSON ad albero** | `src/serve.c` (righe 54–252, `JVal`/`json_parse`/`jobj_get`) | Già gestisce oggetti, array, stringhe con escape/unicode, numeri, bool, null. **Va promosso da `serve.c` a un file condiviso** (`src/json.c`/`.h`) così sia `serve_http` sia il nuovo `mcp_serve` lo linkano senza duplicarlo. |
| **Escape/emissione JSON** | `src/serve.c` (`json_escape`) | Stessa promozione: serve identico per serializzare i risultati dei tool. |
| **Il ciclo CLI e gli env var di setup** | `src/main.c` (`setup_brain`, il parsing di `argv`) | Il nuovo flag `--mcp-engine` si aggiunge allo stesso `for` che già riconosce `--daemon`/`--port`/`--host`; `setup_brain()` non cambia — la modalità mcp-engine USA la stessa funzione, stesso caricamento base/session/profile. |
| **Il motore KB completo** | `src/kb.h` | `kb_assert`, `kb_query`, `kb_match`, `kb_explain`, `kb_induce`, `kb_rule_body_preds`, `kb_describe_entity`, `kb_nearest_concept`, `kb_dump_all`, `kb_predicates` — tutto già lì, testato da 15+ generazioni di uso interno. |
| **Il turno completo** | `src/brain.h` (`brain_respond`) | La "generazione" più ricca: instrada per NL attraverso ~50 moduli (`src/brain/*.c`), può fare query, insegnare, generare testo, tutto in un colpo. |
| **La continuazione grezza** | `src/brain/25-wordmath-reasoning.c` (`next_word_ctx`, non esportata oggi) | La generazione "di basso livello" (il modello di transizione bigram/trigram appreso). Va promossa a funzione non-static o esposta tramite un piccolo wrapper in `brain.h` se la si vuole raggiungere senza passare da NL. |
| **L'estrazione da testo** | `src/brain/30-generation-reading.c` (`read_passage`/`extract_clause`, dietro `read:`) | Oggi raggiungibile solo via `brain_respond("read: ...")`. Il tool `text.extract` può restare un thin wrapper su QUESTO (nessun C nuovo) oppure — meglio, più pulito per un DB — una funzione dedicata `int brain_extract(Brain*, const char *passage, size_t *learned, size_t *skipped)` che fattorizza `mod_reader`'s body in una funzione richiamabile sia da NL sia da MCP. |
| **La temperatura di stile** | `kb/profiles/llm/deepseek-v4-flash.p0`, `00-lex.c` (`kb_match(..,"style_temperature"..)`) | Già un FATTO KB, non un parametro nascosto: `style.set_temperature` è letteralmente `kb_assert(kb, "style_temperature", {"0"}, 1)`. Zero C nuovo oltre al wrapper. |

**Conseguenza architetturale importante:** questo NON è un motore nuovo da
scrivere. È un **quarto ponte** (dopo REPL stdio di chat, `--daemon` HTTP
OpenAI-like, e i tool locali `PARROT0_TOOLS` del pi-agent) sopra lo STESSO
`Brain*`/`KB*`. La disciplina "engine fixed, knowledge learns" del
[[kb-first-manifesto]] non è toccata: si aggiunge un trasporto, non un motore.

---

## 3. Il gap da colmare prima: "il profilo si carica a meno che" non è ancora vero al 100%

F. chiede che, di default, l'agente MCP trovi la STESSA conoscenza che un utente
di chat trova (lessico, marcatori sociali, ruoli, glosse, intent, risposte,
world-facts, il self-model riflessivo `i_am(parrot0)`/`module(...)`), e che sia
possibile avviare un motore "spoglio" con variabili d'ambiente. Verificando
`brain_create()` (`src/brain/99-registry.c:391`), oggi:

- **Già gate-abile:** il lessico (`PARROT0_LEXICON`, path override, vuoto =
  skip) e i world-facts (`PARROT0_WORLD_FACTS=0` li salta).
- **NON gate-abile oggi (hardcoded, nessun env var):** `kb/core/social.p0`,
  `kb/core/roles.p0`, `kb/core/gloss.p0`, `kb/core/intents.p0`,
  `kb/core/responses.p0`, `kb/core/glue.p0` — sei `kb_load` fissi. Il self-model
  riflessivo (`i_am`, `module`, `glue_faculty`) è sempre asserito (RAM-only,
  mai persistito, ma non disattivabile).
- In `main.c` (`setup_brain`), `PARROT0_BASE`/`PARROT0_SESSION`/`PARROT0_PROFILE`
  sono già stringhe vuote = no-op (il contratto di `kb_load` lo garantisce), e
  `kb/experts/programming/coding.p0` è caricato incondizionatamente come base.

**Prerequisito onesto (da fare PRIMA che "avvia spoglio con variabili opportune"
sia una frase vera):** estendere `brain_create()` con lo stesso pattern già in
uso — un env var per gruppo, default al path attuale, stringa vuota = skip — più
un **interruttore generale** che è quello che un agente MCP userà nel 99% dei
casi:

```c
/* nuovo, additivo, non tocca il comportamento di default */
const char *bare = getenv("PARROT0_BARE");   /* "1" = motore spoglio */
int is_bare = bare && *bare && strcmp(bare, "0") != 0;

const char *social   = is_bare ? "" : (getenv("PARROT0_SOCIAL")   ?: "kb/core/social.p0");
const char *roles    = is_bare ? "" : (getenv("PARROT0_ROLES")    ?: "kb/core/roles.p0");
const char *gloss    = is_bare ? "" : (getenv("PARROT0_GLOSS")    ?: "kb/core/gloss.p0");
const char *intents  = is_bare ? "" : (getenv("PARROT0_INTENTS")  ?: "kb/core/intents.p0");
const char *responses= is_bare ? "" : (getenv("PARROT0_RESPONSES")?: "kb/core/responses.p0");
const char *glue     = is_bare ? "" : (getenv("PARROT0_GLUE")     ?: "kb/core/glue.p0");
/* is_bare inoltre implica PARROT0_WORLD_FACTS=0 e disabilita i_am/module? */
```

Il self-model riflessivo (`i_am`/`module`) è più delicato: SONO fatti che
descrivono onestamente il processo in esecuzione (quali moduli esistono
davvero), non "personalità" — probabilmente vanno mantenuti anche in modalità
bare (un agente che chiede "quali predicati conosci?" deve ricevere una risposta
vera). Da decidere in fase di implementazione, non qui: la scelta di default
sicura è "il self-model resta, il layer sociale/di ruolo/di risposta va via" —
un motore di inferenza puro non ha bisogno di sapere come rispondere a "buongiorno".

Ogni env var nuova è additiva e retrocompatibile: il default di ognuna resta il
path attuale, quindi `make test`/`make chat`/`--daemon` non cambiano
comportamento — l'unico modo di ottenere il motore spoglio è settare
`PARROT0_BARE=1` esplicitamente.

---

## 4. Architettura del trasporto

### 4.1 Nuovo file, non intrusione in `main.c`/`serve.c`

Come `serve.c`/`serve.h` incapsulano `--daemon`, un nuovo **`src/mcp.c` /
`src/mcp.h`** incapsula `--mcp-engine`. `main.c` resta "intenzionalmente noioso"
(commento in testa al file): aggiunge un ramo di 3 righe nel parsing di `argv`,
esattamente parallelo a `daemon_mode`:

```c
int mcp_mode = 0;
...
else if (strcmp(argv[i], "--mcp-engine") == 0) mcp_mode = 1;
...
if (mcp_mode) {
    Brain *brain = setup_brain(NULL);
    if (!brain) { fprintf(stderr, "parrot0: out of memory\n"); return 1; }
    int rc = mcp_serve_stdio(brain);
    brain_destroy(brain);
    return rc;
}
```

`Makefile` non cambia: `SRC := $(wildcard src/*.c)` raccoglie `src/mcp.c` da
solo (nessuna riga da editare in `OBJ`).

### 4.2 Trasporto: stdio prima, HTTP dopo (se mai serve)

**stdio è la scelta giusta per la prima generazione:**
- È il trasporto MCP standard per i server locali (Claude Code, altri host,
  lanciano il sottoprocesso e parlano su stdin/stdout).
- parrot0 è GIÀ un programma line-based (`main.c` legge una riga, scrive una
  riga) — il fit è naturale, zero infrastruttura di rete da scrivere/debuggare.
- Framing: MCP-su-stdio usa un messaggio JSON-RPC per riga (newline-delimited),
  NON il framing Content-Length di LSP — quindi è ANCORA più semplice del
  parsing HTTP che `serve.c` già fa.
- Niente concorrenza: un processo per agente, un `Brain*` per processo, nessun
  bisogno di locking (a differenza di `--daemon`, dove più richieste HTTP
  potrebbero in teoria arrivare — oggi gestite comunque una alla volta,
  `accept()`+`handle_conn()` sequenziale).

**HTTP+SSE come estensione futura, non ora:** se in seguito un host preferisce
il trasporto HTTP di MCP, si riusa `serve.c`'s socket loop con un dispatch
diverso sulle stesse funzioni-tool — ma non è nel perimetro di questo piano
finché nessun pull reale lo richiede (disciplina "un obiettivo per
generazione", niente feature speculative).

### 4.3 Scheletro del loop stdio

```c
/* src/mcp.c - MCP JSON-RPC stdio server (gen276+) */
int mcp_serve_stdio(Brain *brain) {
    char line[MCP_LINE_MAX];   /* 65536, come LINE_MAX_LEN di main.c */
    while (fgets(line, sizeof line, stdin)) {
        JVal *req = json_parse(line, strlen(line));
        if (!req || req->type != J_OBJ) { mcp_send_error(-32700, "parse error", NULL); continue; }
        JVal *method = jobj_get(req, "method");
        JVal *id     = jobj_get(req, "id");        /* assente = notification */
        JVal *params = jobj_get(req, "params");
        if (method && method->type == J_STR)
            mcp_dispatch(brain, method->str, params, id);
        jfree(req);
        fflush(stdout);
    }
    return 0;
}
```

`mcp_dispatch` fa uno `strcmp` su `initialize` / `tools/list` / `tools/call` /
`notifications/initialized` (ignorato) e delega a `mcp_tool_call(brain, name,
args, out)` per il quarto caso, che è a sua volta uno `strcmp` sul nome del
tool verso gli adattatori del §5. Struttura piatta e ispezionabile, coerente
con lo stile "first-match-wins" già usato nella registry dei moduli
(`99-registry.c`).

---

## 5. La superficie dei tool (il "DB")

Ogni tool è: **nome**, **input JSON Schema**, **primitiva C sottostante**,
**provenienza dei fatti scritti**. Convenzione di naming: `kb.*` per lettura/
scrittura/inferenza, `text.*` per parsing/estrazione, `gen.*` per generazione,
`style.*` per temperatura, `session.*` per persistenza — namespace piatto,
coerente con com'è organizzata la KB stessa (predicati, non oggetti).

### 5.1 Scrittura — `kb.assert`, `kb.retract`, `kb.assert_rule`

```jsonc
// kb.assert
{"pred": "likes", "args": ["alice", "cats"]}
→ {"ok": true, "stored": true}          // kb_assert
// kb.assert_rule  (congiuntiva, gen133)
{"head": "goodboy", "body": ["friendly", "dog"]}
→ {"ok": true}                           // kb_assert_rule_n
// kb.retract
{"pred": "likes", "args": ["alice", "cats"]}
→ {"ok": true, "removed": true}         // kb_retract
```

Provenienza: tutto ciò che arriva da MCP è marcato `KB_SESSION` (mai `KB_BASE`,
mai promosso ai file curati) — un agente può costruire conoscenza ma non
riscrive silenziosamente il substrato curato. `kb_set_origin(kb, KB_SESSION)`
prima di ogni assert lato server, simmetrico a come `setup_brain` già separa
base/session.

### 5.2 Lettura — `kb.query`, `kb.match`, `kb.describe`, `kb.predicates`

```jsonc
// kb.query — prova booleana (con risoluzione SLD per gli unari)
{"pred": "mortal", "args": ["socrates"]}
→ {"provable": true}                     // kb_query
// kb.match — pattern con variabili (null = variabile)
{"pred": "likes", "args": ["alice", null]}
→ {"bindings": ["cats", "dogs"]}         // kb_match
// kb.describe — fatti diretti su un'entità
{"entity": "socrates"}
→ {"text": "socrates is a man."}         // kb_describe_entity
// kb.predicates — introspezione
{}
→ {"predicates": ["man", "mortal", "likes", ...]}   // kb_predicates
```

### 5.3 Inferenza — `kb.explain`, `kb.induce`, `kb.abduce`

```jsonc
// kb.explain — prova CON la spiegazione (SLD trace)
{"pred": "mortal", "args": ["socrates"]}
→ {"provable": true, "explanation": "mortal(socrates) because man(socrates)"}
                                          // kb_explain
// kb.induce — induzione di regole dai fatti
{"min_support": 2}
→ {"induced": [{"head": "mortal", "body": "man"}]}   // kb_induce
// kb.abduce — quale premessa manca per un goal (gen131, branching gen134)
{"head": "pet", "args": ["rex"]}
→ {"alternatives": [{"body": ["cat"]}, {"body": ["dog"]}]}
                                          // kb_rules_for_head + kb_nth_rule_body_preds
```

### 5.4 Generazione — `gen.respond`, `gen.continue`

```jsonc
// gen.respond — il turno COMPLETO (instrada per NL come la chat)
{"input": "what is a prime number?"}
→ {"output": "..."}                      // brain_respond
// gen.continue — continuazione grezza dal modello bigram/trigram appreso
{"seed": "the cat sat"}
→ {"output": "the cat sat on the mat"}   // next_word_ctx in loop, da esportare
```

`gen.respond` è deliberatamente il tool più potente e più "im-strutturato": fa
scattare QUALUNQUE modulo di `src/brain/*.c`, incluso assert impliciti via
teach-form NL. Un agente MCP che vuole controllo fine userà `kb.assert`/
`kb.query` diretti; `gen.respond` è lì per chi vuole "parlare" al motore invece
di programmarlo — utile per debug e per replicare esattamente cosa farebbe un
utente umano.

### 5.5 Temperatura — `style.set_temperature`, `style.get_temperature`

```jsonc
{"value": 0}
→ {"ok": true}     // kb_assert(kb, "style_temperature", {"0"}, 1) — sostituisce, non accumula
```

Nota di contratto: `style_temperature` oggi è letto come un fatto UNARIO
singolo (`kb_match(...,tq,1,tv,1)==1`); un secondo `kb.assert` con un valore
diverso deve prima `kb_retract` il vecchio valore (altrimenti `kb_match`
prende il primo trovato, non l'ultimo — comportamento da roundtrippare con un
test, non da assumere).

### 5.6 Parsing di testi — `text.parse`

```jsonc
{"text": "Che cos'è un numero primo?", "kind": "nl"}     // kind: "nl" | "c" | "python"
→ {"language": "it", "normalized": "che cos e un numero primo",
   "is_question": true, "tokens": ["che","cos","e","un","numero","primo"]}
```

Per `kind:"nl"`: wrapper su `normalize`/`canonicalize_lang` + il rilevatore di
lingua già usato da `current_lang`. Per `kind:"c"`/`"python"`: wrapper su
`code_ingest`/`code_ingest_py` — restituisce i fatti STRUTTURALI (funzioni,
chiamate, variabili) che l'ingest scrive in KB, così "parsing di testi" copre
sia NL sia codice con la stessa primitiva già usata da Track 5
(coding-agent-evolution) per leggere sorgenti come corpus.

### 5.7 Estrazione di conoscenza da testi — `text.extract`

```jsonc
{"passage": "Rex is a dog. Rex is friendly. Milo is a cat."}
→ {"learned": 2, "skipped": 1, "facts": ["dog(rex)", "friendly(rex)"]}
```

Wrapper su una nuova funzione factored da `mod_reader`/`read_passage`
(`src/brain/30-generation-reading.c:851`) — oggi `read_passage` è `static` e
raggiungibile solo dietro il prefisso di chat `read:`. Fattorizzarla in
`int brain_extract(Brain*, const char*, size_t*, size_t*)` non tocca il
comportamento NL esistente (mod_reader diventa un thin wrapper sulla stessa
funzione) e la rende chiamabile direttamente da MCP senza passare per
`brain_respond("read: " + testo)` — più pulito, e restituisce anche l'elenco
dei fatti realmente scritti (oggi `read_passage` non lo espone al chiamante,
va aggiunto un array di stringhe in output).

### 5.8 Persistenza — `session.save`, `session.dump`

```jsonc
// session.save — persiste i fatti SESSION+INDUCED su file (come /save in chat)
{"path": "kb/core/session.p0"}
→ {"ok": true, "written": 12}            // brain_save_session
// session.dump — tutti i fatti attivi, per debug/audit
{}
→ {"facts": ["man(socrates).", "mortal(socrates) :- ...", ...]}  // kb_dump_all
```

---

## 6. Modello di concorrenza e stato

- **Un processo, un `Brain*`, un agente.** Lo stdio MCP è 1:1 (l'host lancia UN
  sottoprocesso per sessione) — nessun locking, nessuna race, coerente con come
  `main.c` già tratta `Brain*` come stato mutabile di un solo thread.
- **Lo stato PERSISTE per tutta la sessione MCP**, esattamente come la chat
  interattiva: un `kb.assert` alla chiamata N è visibile a `kb.query` alla
  chiamata N+1 nello stesso processo. Questo è il punto dell'intera proposta
  ("un agente che pilota il motore per poterlo potenziare") — non è
  stateless come una singola richiesta HTTP di `--daemon`.
- **Persistenza su disco è ESPLICITA** (`session.save`), mai automatica: un
  agente che sperimenta non deve temere di sporcare `kb/core/session.p0` senza
  chiedere — stesso principio del comando `/save` in chat (main.c:235).

---

## 7. Sicurezza e onestà (non negoziabili, ereditate dai principi esistenti)

1. **Provenienza sempre `KB_SESSION`** per ciò che arriva da MCP (§5.1) — mai
   `KB_BASE`. Un agente esterno non riscrive il substrato curato che i
   generation journal documentano; costruisce sopra, in un layer che
   `kb_save(kb, path, KB_SESSION|KB_INDUCED)` può scrivere separatamente.
2. **Nessuna promozione silenziosa.** Se in futuro serve un tool che scrive in
   `KB_BASE` (es. per aiutare la CURATION dei file `.p0` stessi), deve essere
   un tool ESPLICITO e distinto (`kb.assert_base`?), mai il default.
3. **`gen.respond` non bypassa l'onestà del motore.** Se `brain_respond`
   risponderebbe "non lo so" a un utente umano, risponde "non lo so" anche a un
   agente MCP — nessuna scorciatoia, nessun "modalità agente = meno onesto".
4. **Il self-model riflessivo non mente.** Se la modalità bare (§3) nasconde il
   layer sociale ma mantiene `i_am`/`module`, un agente che chiede "che
   predicati esistono?" deve vedere esattamente i moduli REALMENTE compilati
   nel binario in esecuzione — mai una lista aspirazionale.
5. **Path traversal e injection.** Ogni tool che accetta un path (`session.save`)
   applica la STESSA disciplina di sandbox già in `code.c` (rifiuta path
   assoluti, `~`, `..` — vedi `orchain_ident_ok`/i controlli in
   `code_orchain_patch`) invece di fidarsi ciecamente dell'input JSON.

---

## 8. Piano a generazioni (un obiettivo per generazione, disciplina di LOOP.md)

| Gen | Obiettivo | Ratchet/test |
|---|---|---|
| **A** | Scheletro trasporto: `src/mcp.c`/`.h`, promozione del parser JSON da `serve.c` a `src/json.c`/`.h` condiviso, `--mcp-engine` in `main.c`, `initialize`/`tools/list` (lista vuota) rispondono. | `tests/mcp.sh`: un handshake scriptato (due righe JSON in, due righe JSON out), verificato campo per campo (niente `jq`: grep/sed come gli altri test .sh, o un piccolo parser python come `BENCH_PY` se serve robustezza). |
| **B** | `kb.assert`/`kb.query`/`kb.match`/`kb.retract` — lettura/scrittura. | Roundtrip test: assert → query (true) → retract → query (false). |
| **C** | `kb.explain`/`kb.induce`/`kb.abduce` — inferenza. | Un caso preso pari pari da `tests/knowledge.sh` (man→mortal) rifatto via JSON-RPC invece che via NL, stessa proof string attesa. |
| **D** | `gen.respond`/`gen.continue`, `style.set_temperature/get_temperature`. | `gen.respond({"input":"2+2"})` → "4" (o equivalente); temperatura 0 rende `gen.respond` deterministico su un caso con `response_template` multi-forma. |
| **E** | `text.parse` (nl+c+python), `text.extract` — richiede fattorizzare `read_passage`/`extract_clause` fuori da `static` in `brain_extract()`. | `text.extract({"passage":"Rex is a dog."})` → `{"learned":1,...}`; poi `kb.query({"pred":"dog","args":["rex"]})` → true — la catena scrittura-da-testo→lettura chiude in un test end-to-end. |
| **F** | Il gap del §3: env var per social/roles/gloss/intents/responses/glue + `PARROT0_BARE=1`. | Un test che avvia con `PARROT0_BARE=1` e verifica che `kb.query({"pred":"social_marker",...})` sia false ma `kb.query({"pred":"module","args":["knowledge"]})` resti true. |
| **G** | Hardening: path-safety sui tool con path, errori JSON-RPC ben formati (-32700/-32600/-32601/-32602), documentazione (`docs/use-mcp-engine.md`, sorella di `docs/use-on-pi-agent.md`). | Test di un client MCP reale se disponibile nell'ambiente (best-effort); altrimenti lo script hand-rolled resta l'oracolo primario, onestamente dichiarato tale. |

Ogni riga è un candidato per **una generazione** del loop di auto-evoluzione
(`TASK.md`), non un blocco monolitico — stessa disciplina delle Track esistenti
(coding-agent-evolution.md). L'ordine A→G è per DIPENDENZA (non si può testare
`kb.assert` senza lo scheletro A), non per priorità: se un pull reale (F. che
prova a usarlo) rivela che D o E servono prima, si riordina.

---

## 9. Cosa NON fare (perimetro esplicito)

- **Non reimplementare il motore d'inferenza.** Zero nuova logica di
  risoluzione/unificazione: `kb.c` esiste, è testato, si chiama e basta.
- **Non aggiungere `resources` o `prompts` MCP** finché nessun pull reale li
  richiede — `tools` è l'unico sottoinsieme del protocollo che serve oggi.
- **Non fondere `--mcp-engine` con `--daemon`.** Sono due ponti con contratti
  diversi (OpenAI chat-completions vs JSON-RPC a grana fine): stesso `Brain*`
  sotto, stessa filosofia, file separati (come `serve.c` è separato da
  `main.c`).
- **Non introdurre concorrenza/thread.** Un processo per sessione è già il
  modello giusto per come MCP-su-stdio funziona; multi-client va HTTP, che è
  fuori perimetro (§4.2).
- **Non rendere `gen.respond` un modo per aggirare l'onestà del motore** — vedi
  §7.3. Se serve un modo per un agente di "spingere" nuova capacità in parrot0,
  la via resta lo stesso loop di auto-evoluzione (LOOP.md), non un tool MCP che
  scrive C da remoto.

---

## 10. Collegamenti

Documenti da leggere insieme a questo: [[kb-first-manifesto]] (la disciplina
che ogni tool nuovo deve rispettare — nessuna frase cablata, solo primitive),
[[dynamic-knowledge-direction]] ("no network = no outsourced intelligence": un
agente MCP che scrive fatti nella KB è knowledge autoctona esplicita, non
un'API che ragiona al posto di parrot0 — la differenza è che il RAGIONAMENTO
resta di `kb.c`, l'agente fornisce solo dati/comandi), `docs/use-on-pi-agent.md`
(il precedente più vicino: come un agente esterno già monta parrot0 oggi, via
`--daemon`), `docs/CODE-MASTERY.md` (perché il motore che legge/scrive codice è
lo stesso che legge/scrive linguaggio naturale — rilevante per `text.parse`
sul ramo `c`/`python`).