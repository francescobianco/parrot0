# Guidare parrot0 come motore di inferenza MCP (dal vivo)

`parrot0 --mcp-engine` (gen277) avvia un server **MCP** (Model Context Protocol)
JSON-RPC 2.0 su **stdio**: espone il motore Prolog-like e le primitive di
generazione come *tool* chiamabili. Un agente ci scrive conoscenza, la interroga,
ci fa inferenza, genera, e — il punto della missione — **addestra parrot0 dal
vivo** senza riavviare il processo. Piano completo in
[docs/plans/mcp-engine.md](plans/mcp-engine.md).

> Non è un LLM e non è il daemon OpenAI (`--daemon`, `docs/use-on-pi-agent.md`):
> è un ponte a grana fine sul motore. Stesso `Brain`/`KB`, file separato
> (`src/mcp.c`).

## Avvio grezzo (un colpo solo)

Il server legge un messaggio JSON-RPC per riga da stdin e scrive una riga di
risposta su stdout. Una sessione one-shot (lo stato vive finché il processo vive):

```bash
printf '%s\n' \
  '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{}}' \
  '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"kb.assert","arguments":{"pred":"man","args":["diogenes"]}}}' \
  '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"kb.assert_rule","arguments":{"head":"philosopher","body":["man"]}}}' \
  '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"kb.query","arguments":{"pred":"philosopher","args":["diogenes"]}}}' \
  | bin/parrot0 --mcp-engine 2>/dev/null
```

L'ultima query risponde `{"provable":true}`: parrot0 ha **derivato** la
conclusione dalla regola appena insegnata, non l'ha memorizzata.

## Sessione PERSISTENTE per esperimenti dal vivo (`scripts/mcp-live.sh`)

Per addestrare a più comandi separati serve che il motore resti **acceso** tra
una chiamata e l'altra. Il driver tiene vivo il server (richieste su una FIFO
tenuta aperta, risposte su un file che il server appende) e permette chiamate
singole:

```bash
scripts/mcp-live.sh start PARROT0_WORLD_FACTS=0     # accende il motore
scripts/mcp-live.sh call kb.query   '{"pred":"philosopher","args":["diogenes"]}'   # {"provable":false}
scripts/mcp-live.sh call kb.assert  '{"pred":"man","args":["diogenes"]}'           # {"ok":true,"stored":true}
scripts/mcp-live.sh call kb.assert_rule '{"head":"philosopher","body":["man"]}'    # {"ok":true}
scripts/mcp-live.sh call kb.query   '{"pred":"philosopher","args":["diogenes"]}'   # {"provable":true}  ← imparato
scripts/mcp-live.sh call gen.respond '{"input":"is diogenes a philosopher?"}'      # {"output":"Yes."}
scripts/mcp-live.sh stop                            # spegne il motore
```

Ogni `call` è un'invocazione separata dello script, ma lo stato del motore
persiste: la conoscenza scritta da un comando è viva per il successivo. Comandi
utili: `scripts/mcp-live.sh tools` (elenca i tool), `... raw '<json>'` (richiesta
JSON-RPC arbitraria). La cartella di stato è `$PARROT0_MCP_DIR`
(default `/tmp/parrot0-mcp`).

## I tool (19)

| Tool | Cosa fa | Primitiva |
|---|---|---|
| `kb.assert` `{pred,args}` | asserisce un fatto (layer sessione) | `kb_assert` |
| `kb.assert_rule` `{head,body[]}` | asserisce `head(X) :- b0(X),…` (**solo unario**, vedi limiti) | `kb_assert_rule_n` |
| `kb.assert_clause` `{head,body[]}` | asserisce clausola n-aria piena `head:-body0,…,bodyN` con variabili `$` distinte/condivise (U2 gen281) | `kb_assert_clause` |
| `kb.retract` `{pred,args}` | ritratta un fatto | `kb_retract` |
| `kb.query` `{pred,args}` | prova per risoluzione SLD (n-aria, ricorsiva) | `kb_query` |
| `kb.match` `{pred,args}` | pattern con `null`=variabile → binding (**prima var / lista piatta**) | `kb_match` |
| `kb.explain` `{pred,args}` | prova + spiegazione di una riga | `kb_explain` |
| `kb.describe` `{entity}` | fatti diretti su un'entità | `kb_describe_entity` |
| `kb.dump` `{}` | tutti i fatti, leggibili | `kb_dump_all` |
| `kb.induce` `{min_support}` | induce regole dai fatti | `kb_induce` |
| `kb.stats` `{}` | quanti fatti | `kb_size` |
| `kb.save` `{path}` | persiste il delta di sessione su file | `brain_save_session` |
| `kb.restore` `{}` | dimentica il non salvato, ricarica i file da disco | `brain_reload` (gen276) |
| `input.segment` `{text}` | span con offset, ruolo aperto, registro, score, proof e faculty; pareggi espliciti | `input_segment` (gen332) |
| `input.classify` `{relation,text,candidates?}` | confronta ipotesi KB → winner/gap/ambiguous con gli stessi supporti di intenti e registri | `kb_hypothesis_best` (gen332) |
| `gen.respond` `{input}` | turno completo attraverso il brain | `brain_respond` |
| `text.extract` `{passage}` | legge un passaggio ed estrae fatti nella KB | `mod_reader` |
| `style.set_temperature` `{value}` | temperatura di scelta della forma | fatto `style_temperature` |
| `style.get_temperature` `{}` | legge la temperatura corrente | — |

Il risultato di ogni tool arriva come blocco `content` di testo il cui testo è un
piccolo oggetto JSON (es. `{"provable":true}`).

## Il loop "scrivi su file → /restore → interroga"

`kb.assert`/`kb.assert_rule` scrivono nella KB **in RAM** — già interrogabili
subito. Per farlo diventare persistente (e per raccogliere modifiche fatte al
file da fuori), c'è il loop che la missione descrive:

1. aggiungi conoscenza (`kb.assert`, `text.extract`, o edita un `.p0` a mano);
2. `kb.save {path}` la scrive su un file `.p0`;
3. `kb.restore` ricarica **tutti** i file da disco, in place, senza riavviare;
4. interroghi la conoscenza nuova.

Perché il passo 3 la ripeschi, avvia il motore con `PARROT0_SESSION` puntato al
file che `kb.save` scrive (così `brain_reload` lo ricarica).

## Limiti noti dei tool (gen281, misurati dal vivo)

> **Risolto dopo gen280:** i letterali insegnati via MCP ora round-trippano
> fedelmente — un nome proprio (`Madrid`), un template sentence-case, un valore
> `$HOME` non vengono più scambiati per variabili (U1 gen279 quota i letterali al
> bordo; U1b gen280 introduce `$` come marcatore di variabile esplicito). Vedi
> `tests/mcp-teach.sh`.
>
> **Risolto in gen281:** `kb.assert_clause` (U2) colma il buco n-ario — regole
> con join a più variabili (`grandparent(X,Z) :- parent(X,Y), parent(Y,Z)`) ora
> si insegnano via MCP senza file `.p0`. Vedi `tests/assertclause.sh`. Resta il
> limite strutturale sotto.

Il **motore** dietro MCP è un Prolog n-ario completo (join a più variabili,
ricorsione, backtracking, cycle-guard — vedi
[il protocollo prolog-like](prolog-like-engine.md)). Un **tool** però ne
espone solo un sottoinsieme; la differenza è nell'adattatore, non nel motore:

- **`kb.match` torna la prima variabile, appiattita.** Con più `null`,
  `kb.match parent(?,?)` dà `["tom","bob","ann"]` (lista de-duplicata), non le
  **tuple** `[["tom","bob"],["bob","ann"]]`. Il join fra i due slot va perso. Le
  tuple sono il passo **P2** dello stesso piano.

Nessuno dei due è un bug del motore: `kb.query`/`kb.explain` risolvono già join e
ricorsione se la clausola arriva da un `.p0`. Sono i due adattatori di scrittura/
lettura da estendere.

## Sicurezza

I fatti scritti via MCP sono marcati `KB_SESSION` (mai `KB_BASE`): un agente
costruisce conoscenza in un layer separato, non riscrive il substrato curato.
`kb.save` rifiuta path assoluti, `~` e `..`. `gen.respond` non scavalca
l'onestà del motore: se parrot0 murerebbe con un utente, mura anche via MCP.
