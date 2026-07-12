# L'input è UNO — nessun flusso di testo si classifica nel C

> **Stato:** **chiuso a gen332 (2026-07-12)**. Il debito era stato riconosciuto a
> gen331, subito dopo TODO.md 01: il comportamento era verde ma viveva in un enum,
> in un parser C-only e in un frasario bilingue. Ora `InputSpan` ha ruoli aperti,
> `input_segment(KB*, …)` confronta evidenze con proof e senza tiebreak, e tutta la
> tassonomia vive in `kb/core/input.p0`. L'oracolo dedicato
> `tests/universal-input.sh` regge 64/64, `kb-evidence-scale` 11/11 e il payload
> MCP oltre 95 KB resta parseabile, insieme a `segment` 28/28 e `codebench` 25/25
> (73/73 turni).
> **Ruolo:** questo documento nomina il concetto, spiega perché il C non ha diritto
> di classificare un flusso di testo, e traccia la rotta per rifare TODO 01 nella
> forma giusta.
> **Subordinato a** [[kb-first-manifesto]] (la bussola: *engine fixed, knowledge
> learns*), [[universal-comprehension]] (niente muro cieco: la forma si estrae
> sempre, e le forme vivono nella KB), [[universal-solver]] (un solo motore su ogni
> superficie: prosa, teorema, **spec di codice**, paper).
> **Disciplina:** un obiettivo per generazione, pull da un counterexample reale,
> niente frasari, niente fabbricazione.

---

## 0. La tesi

> **Un messaggio, un test, un sorgente, un diff, un log, una traccia di stack, un
> JSON: sono lo stesso oggetto — un flusso di testo. parrot0 non li *classifica*:
> li *comprende*. E la comprensione è KB-first.**

Il corollario, che è la parte che morde:

> **Nessuna decisione su "che cosa è questo pezzo di testo" può vivere nel C.**
> Il C ospita il *motore* che estrae struttura da un flusso. Quali strutture
> esistano, come si chiudano, che *ruolo* abbia ciascuna span e quale faculty la
> serva: tutto questo è **conoscenza**, e la conoscenza sta nella KB.

Non è una preferenza estetica. È la differenza tra un sistema che impara un registro
nuovo con **un fatto** e uno che lo impara con **una generazione**.

## 1. Il counterexample che ha imposto il tema (e quello che ho scritto io)

Il primo, su binario gen328 — è la voce 01 della TODO:

```
you> fix this code: int absval(int x) { if (x < 0) { return -x; } return x; }
     It should return the absolute value but it returns a negative number for x = -5
parrot0> Fix: add a semicolon at the end of each statement.
```

Il C è impeccabile. **Il punto e virgola mancante era nell'inglese**: `find_code_section()`
definiva il codice come "tutto dopo il primo due punti", quindi la descrizione del bug
scritta dall'utente veniva compilata insieme al programma. cc rifiutava il paragrafo,
il veto sintattico non scattava, e uno scanner fatto a mano inventava un difetto dentro
codice corretto.

La diagnosi vera non è "manca un estrattore di codice". È: **parrot0 non aveva un
modello dell'input**. Riceveva un flusso misto e lo trattava come se fosse omogeneo,
perché *nessuno gli aveva mai detto che un turno può contenere registri diversi*.

Poi c'è il secondo counterexample, ed è mio (gen330, `src/code.c`):

```c
int constraint = strstr(low, "without") || strstr(low, "senza") ||
                 strstr(low, "must not") || strstr(low, "non deve") ||
                 strstr(low, "do not use") || strstr(low, "non usare");
```

Ho chiuso un problema di comprensione universale con **un frasario bilingue cablato**,
una tassonomia chiusa in un `enum` C (`CODE_SEG_INSTRUCTION|CODE|EXPECTED|CONSTRAINT`)
e un parser che conosce solo le graffe del C. Funziona, ed è la forma sbagliata:

- «a condizione che», «purché», «sans», «avoid using» → **ricompilazione**;
- il ruolo `repro` o `non-goal` che TODO 20 chiede già → **una generazione**;
- Python, un diff, un log di pytest → **nessun percorso**.

La causa onesta non è nemmeno un vincolo tecnico: `src/code.c` include già `kb.h` e
`code_ingest(KB *kb, …)` prende la KB. **Potevo passargliela e non l'ho fatto.**

## 2. Perché il C non ha diritto di classificare

Il divieto non è un'opinione: è scritto, e in tre posti.

| fonte | riga | cosa dice |
|---|---|---|
| [[kb-first-manifesto]] | `kb-first.md:21` | «**Lingua e codice/logica abitano la stessa struttura elaborativa.**» |
| [[kb-first-manifesto]] | `kb-first.md:35` | «*Engine fixed, knowledge learns.* Il C contiene **motori**, non **frasi**. Ogni forma di superficie che parrot0 riconosce vive nella KB, **mai come `printf` cablato**.» |
| [[universal-comprehension]] | `universal-comprehension.md:54` | «parrot0 sa estrarre grammaticalmente la struttura di **qualsiasi** frase; le **strutture** (schemi sintattici, ruoli, schemi d'intento) vivono **nella KB, non cablate nel C**.» |
| [[universal-solver]] | `universal-solver.md:28` | la superficie è indifferentemente «prosa, clue, enunciato, teorema, **spec di codice**, paper» → *un solo motore*, KB diverse. |

Il ragionamento, per esteso:

**(a) Un classificatore nel C è un frasario travestito da architettura.** Un
`printf("...")` che simula conoscenza è l'impostore che PRINCIPLES rifiuta; un
`strstr("senza")` che simula *comprensione* è lo stesso impostore, un livello più
in basso — peggiore, perché si nasconde dentro un motore e sembra struttura.

**(b) La classificazione è una domanda a cui si può *rispondere male*, e allora deve
essere interrogabile.** Se parrot0 decide che «It should return 5» è `expected`, deve
poter dire **perché** (`segment_role(expected, "it should")`), e io devo poterlo
**correggere con un fatto**. Una decisione presa da un `||` in C non ha proof, non ha
provenance, non si ritratta, non si insegna. È esattamente ciò che TODO 24-25
chiedono di abolire ovunque.

**(c) Il registro NON è una proprietà del flusso: è un'ipotesi sul flusso.** «`for`»
è C, è Python, ed è una preposizione inglese. gen323 ha già pagato questo prezzo
(«what language is this: the sky is blue» → *"This looks like Python code"*, perché
`is`/`in`/`for` sono keyword Python **e** parole inglesi). Un'ipotesi va **sostenuta
da evidenza e confrontata con le alternative**, non decisa da un `if` che vede la
prima parola che gli piace. Questo è lo stesso identico difetto di TODO 23 (il keyhole
linguistico: `.conf` letto come `.c` per substring) — **la stessa malattia in due
organi diversi**.

**(d) Il costo marginale di un registro nuovo deve tendere a zero.** È l'unica prova
che l'architettura sia giusta. Oggi: un diff, un traceback Python, un log di pytest,
un JSON, un messaggio di errore del linker — **cinque registri, cinque parser C**.
Domani devono essere **cinque gruppi di fatti**.

## 3. Il modello

```
   FLUSSO GREZZO (un turno: prosa, codice, output, diff, log — mescolati)
            │
            │  MOTORE (C, fisso):  delimitatori bilanciati, indentazione,
            │                      fence, righe, offset di byte
            ▼
   SPAN  [start, len]  — pezzi, ancora senza nome
            │
            │  CONOSCENZA (KB):  register_evidence/2  → che registro è
            │                    segment_role/2       → che ruolo ha
            ▼
   SPAN TIPIZZATE  (instruction | code:c | expected | constraint | repro | log …)
            │
            │  CONOSCENZA (KB):  faculty_for/2  → chi la serve
            ▼
   FACULTY  (compilatore, checker, planner, reasoner…) — vede SOLO la sua span
```

La riga che separa i due mondi, e va difesa:

| **MOTORE — sta nel C** | **CONOSCENZA — sta nella KB** |
|---|---|
| bilanciare una coppia di delimitatori | *quali* delimitatori chiudono un registro |
| misurare indentazione, righe, fence | che Python chiude a indentazione e il C a graffe |
| tenere gli offset di byte (span, non copie) | che una span può avere il ruolo `repro` |
| confrontare più ipotesi per evidenza | quale evidenza vale per quale registro |
| dire «non chiude» → `ambiguous_input` | come si chiamano i ruoli, in quante lingue |

Il motore non sa **nulla** di C, Python, inglese o italiano. Sa solo *bilanciare*,
*misurare* e *pesare ipotesi*. Se sa qualcos'altro, è un bug di progetto.

## 4. Lo schema KB (la forma bersaglio)

L'infrastruttura esiste già: `kb_cue_match` regge gli `intent_cue` da gen323, ed è
esattamente il meccanismo che serve qui.

```prolog
% --- che registro è questa span? (evidenza, non certezza) ---
register_evidence(c,        balanced("{","}")).
register_evidence(c,        keyword(int)).
register_evidence(python,   block(indent)).
register_evidence(python,   keyword(def)).
register_evidence(diff,     line_prefix("@@")).
register_evidence(pytest,   line_prefix("E   ")).
register_evidence(prose,    default).

% --- come si chiude una span di quel registro? ---
delim_pair(brace,  "{", "}").
delim_pair(paren,  "(", ")").
code_register(c,      brace).
code_register(python, indent).
code_register(json,   brace).

% --- che RUOLO ha la span di prosa attorno al codice? ---
segment_role(expected,   "it should").      segment_role(expected,   "dovrebbe").
segment_role(expected,   "expected").       segment_role(expected,   "mi aspetto").
segment_role(observed,   "it returns").     segment_role(observed,   "invece restituisce").
segment_role(constraint, "without").        segment_role(constraint, "senza").
segment_role(constraint, "do not use").     segment_role(constraint, "non deve").
segment_role(repro,      "to reproduce").   segment_role(repro,      "per riprodurre").
segment_role(non_goal,   "out of scope").

% --- chi serve un ruolo, una volta riconosciuto? (il consumer) ---
faculty_for(code(c),   compiler_oracle).
faculty_for(expected,  contract_builder).
faculty_for(constraint, planner).
```

Tre proprietà da leggere con attenzione, perché sono il **rendimento** dell'operazione:

1. **`segment_role(repro, …)` è un fatto** → TODO 20 (contratto della issue: expected,
   observed, repro, constraint, non-goal) costa **fatti**, non un modulo C.
2. **`register_evidence(pytest, …)` è un fatto** → TODO 21 (parsare pytest/sanitizer
   in verdict strutturati) entra dalla stessa porta di tutto il resto.
3. **Nessuna riga di C nuova** per una lingua nuova, un ruolo nuovo, un registro nuovo.

## 5. I vantaggi (perché vale la generazione)

- **Un registro nuovo costa un fatto.** È il test dell'architettura, e oggi lo
  falliamo cinque volte su cinque (diff, traceback, log, JSON, linker).
- **La segmentazione diventa interrogabile.** «Perché hai pensato che quella frase
  fosse un vincolo?» → «`segment_role(constraint, "senza")`». Una decisione con una
  proof è una decisione che si può **correggere** e **ritrattare** (TODO 24-25).
- **Chiude 01 e 23 con lo stesso motore.** Il keyhole linguistico (23) e il
  keyhole di registro (01) sono **la stessa malattia**: una decisione strutturale
  presa per substring dal primo `if` che matcha. Un solo confronto-di-ipotesi le cura
  entrambe.
- **`ambiguous_input` smette di essere un caso speciale del C.** Diventa la risposta
  generale del motore: *nessuna ipotesi domina* → dillo, non indovinare. È la legge
  «un declino preciso è verde» applicata alla percezione, non solo all'azione.
- **Sblocca l'IR comune (TODO 18) e il VTC (TODO 20).** Entrambi presuppongono che
  parrot0 sappia *dove* comincia il codice e *cosa* dice la prosa attorno. Oggi lo
  sa per il C, a graffe, in due lingue.
- **È la riga 21 del manifesto, resa vera.** «Lingua e codice abitano la stessa
  struttura elaborativa» oggi è un'aspirazione contraddetta dal codice; qui diventa
  un meccanismo.

## 6. Implicazioni (chi eredita questa scelta)

| voce TODO | come cambia |
|---|---|
| **01** | riaperta: comportamento verde, **forma in debito**. Va rifatta su schema KB. |
| **02** | l'honesty sweep resta valido (l'oracolo veta), ma il *gate* diventa per-registro: un checker può parlare solo della span del suo registro. |
| **18** (IR C/Python) | l'IR nasce dalle span tipizzate, non da un parser per linguaggio. |
| **20** (contratto issue) | expected/observed/repro/constraint/non-goal = `segment_role/2`. Diventa quasi gratis. |
| **21** (diagnostica → verdict) | pytest/cc/sanitizer = tre `register_evidence/2`, non tre parser. |
| **22** (explanation semantica) | ogni frase marcata parsed/derived/hypothesized: è la stessa provenance delle span. |
| **23** (keyhole → intenti) | **si fonde**: stesso confronto di ipotesi, stesso `kb_cue_match`, stessa proof. |
| **12** (kernel tipizzato) | `Segment` è il primo tipo del kernel, a monte di `Task`/`Goal`. |

## 7. Cosa questo piano NON dice

Per non farne un dogma:

- **Non dice «tutto nella KB».** Il motore resta nel C, ed è giusto così: bilanciare
  delimitatori e pesare ipotesi sono *motori*, come l'unificazione. Il confine è
  quello della tabella §3, non «zero C».
- **Non dice di cancellare il C esistente.** Vale la regola di F.
  ([[keep-secondary-structures]]): cambi **additivi**. `code_segment()` resta come
  adapter compatibile sopra `input_segment()`; il vecchio estrattore dopo i due
  punti interviene soltanto su un Gap, mai per sciogliere un'ambiguità KB.
- **Non promette che l'evidenza basti sempre.** Quando due registri restano
  equiprobabili, la risposta corretta è `ambiguous_input` — non un tiebreak cablato
  per far passare un test.

## 8. La TODO list conseguente (rif. TODO.md **01**)

Ordinata: ogni voce parte da un counterexample e si chiude con un oracolo. Una per
generazione.

- [x] **U1 — La KB entra nel segmentatore.** `code_segment(KB*, …)`: passare la KB
      (già inclusa in `code.c`) e sostituire il frasario di `src/code.c:2743` con
      `segment_role/2` letto via `kb_match`/`kb_cue_match`. **Done:** insegnare
      «a condizione che» come `constraint` a **runtime** cambia la segmentazione,
      senza ricompilare; `tests/segment.sh` resta 28/28.

- [x] **U2 — I ruoli diventano aperti.** Eliminare il vincolo dell'`enum` chiuso:
      il ruolo è un termine KB, non una costante C. **Done:** `segment_role(repro, …)`
      asserito a runtime produce una span tipizzata `repro` interrogabile, con **zero**
      righe di C nuove.

- [x] **U3 — Il registro è un'IPOTESI, non un `if`.** `register_evidence/2` +
      confronto di ipotesi con punteggio e proof; nessun tiebreak cablato.
      **Done:** «what language is this: the sky is blue» resta prosa (regressione
      gen323 blindata), `.conf` non è più `.c` (il counterexample di TODO 23), e due
      registri equiprobabili producono `ambiguous_input`.

- [x] **U4 — Il motore smette di conoscere il C.** Le graffe escono dal C e
      diventano `code_register(c, brace)` + `delim_pair(brace,"{","}")`; il motore
      bilancia una coppia *qualsiasi*. **Done:** aggiungere il registro `json`
      (`delim_pair` già esistente) è **zero C**, e un blocco JSON dentro un turno
      viene isolato correttamente.

- [x] **U5 — Python entra dalla stessa porta.** `code_register(python, indent)`:
      il motore misura l'indentazione, la KB dice che è Python a usarla.
      **Done:** una `def` con prosa in coda produce le stesse span tipizzate del C;
      nessun ramo Python dedicato nel dispatcher.

- [x] **U6 — Registri non-sorgente.** `diff`, `pytest`, `cc`, `traceback` come
      `register_evidence/2`. **Done:** un output di pytest incollato nel turno è
      isolato e tipizzato e `faculty_for(log, verdict_builder)` consegna la stessa
      span al futuro verdict di TODO 21, che resta una faculty da implementare.

- [x] **U7 — Le span portano provenance.** Ogni span cita l'evidenza che l'ha
      tipizzata (`because register_evidence(...)` / `segment_role(...)`).
      **Done:** «perché hai pensato che fosse un vincolo?» risponde con il fatto
      esatto, e una correzione dell'utente ritratta la tipizzazione (TODO 24).

- [x] **U8 — Fusione con TODO 23.** Intento e registro usano lo **stesso**
      confronto di ipotesi e la stessa proof. **Done:** una sola implementazione
      serve entrambi; l'ablation di un `intent_cue` e quella di un
      `register_evidence` producono Gap della stessa forma.

## 9. L'oracolo del piano intero

Il piano è chiuso quando **tutte** queste sono vere insieme:

1. `grep -rn 'strstr(low, "' src/code.c` sui ruoli/registri → **zero righe**.
2. Un registro nuovo (JSON, diff, log) entra con **soli fatti**, provato da un test
   che non tocca il C.
3. `tests/segment.sh` (28/28) e `codebench` (25/25) restano verdi: **nessuna
   regressione di comportamento** in cambio della forma giusta.
4. Ogni span risponde a «perché?» con un fatto, e una correzione la ritratta.

### Esito gen332

- `kb_hypothesis_best` è l'unico scorer per `intent_cue`, `register_evidence` e
  qualunque futura relazione di evidenza: `winner`, `gap` e `ambiguous` hanno la
  stessa forma e conservano i fatti di supporto.
- `input.segment` e `input.classify` espongono il meccanismo via MCP. Lo stesso
  processo può asserire `segment_role(repro, …)`, vedere la nuova span, ritrarre il
  fatto e vedere sparire la tipizzazione senza restart o rebuild.
- Fence, coppie di delimitatori arbitrarie, indentazione e run di righe sono
  meccaniche fisse; C, Python, JSON, diff, pytest, cc, traceback e i loro consumer
  sono fatti. Il test aggiunge anche `notice` con un solo fatto e `anglelang` con
  delimitatori `< >`, più una coppia simmetrica `| |`, senza una riga di C
  specifica.
- L'output MCP delle span resta JSON completo anche oltre il vecchio scratch da
  70 KB (`tests/mcp-input-payload.sh`: 128 span e payload da oltre 95 KB).
- Discovery e proof crescono con la KB senza soglie semantiche nascoste:
  `tests/kb-evidence-scale.sh` copre 257 ipotesi/supporti, truncation esplicita e
  fast-path strutturali (11/11, anche sotto ASan+UBSan).
- La parte di TODO 23 qui chiusa è il confronto intent/register (incluso il routing
  degli intenti code e il match esatto delle estensioni). Il proposal routing
  transazionale globale resta nella sua voce. Analogamente TODO 24 resta owner
  dell'invalidazione di proof *già emesse*; la risegmentazione live non usa cache.

> Il codice giusto era già stato scritto: `kb-first.md:21` lo diceva a gen~180.
> Quello che mancava era accorgersi che **anche la percezione dell'input** è una
> capacità, e che una capacità cablata nel C è una capacità che non cresce.
