# Procedure insegnabili — la conoscenza è fatti **E** trasformazioni

> **Stato:** aperto a gen311 (2026-07-11) come SCELTA STRATEGICA di F., dopo che una
> sessione intera di "costrutti grammaticali" (coniugazione, pro-drop, aux-drop,
> fronting ES, ser/estar…) ha reso evidente il muro: ogni costrutto nuovo = un
> consumer C nuovo. F.: *"andiamo a marcia dritta verso l'interprete di riscrittura
> generico. Lo scopo è vedere funzionare il meccanismo che insegna a FARE cose — la
> grammatica, i calcoli, e tutto ciò che servirà. La conoscenza non è solo fatti, ma
> anche procedure per la trasformazione dei fatti. KB-first nella sua essenza massima."*
> **Ruolo:** questo documento è la DECISIONE e la rotta. Chiude il ciclo "un C per
> costrutto" sostituendolo con **un interprete generico + regole di trasformazione
> insegnate**.
> **Subordinato a** [[teach-comprehension-via-mcp]] (§2 Secchio B/D — la diagnosi da
> cui nasce), [[generative-prolog]] (il motore come generatore di percorsi),
> [[universal-solver]] (di cui questo è il motore-gemello: inferenza + riscrittura su
> strutture), [[kb-first-manifesto]] (la bussola).

---

## 0. La tesi, portata al massimo

Fin qui KB-first ha voluto dire: **le FORME sono dati** (lessico, idiomi, cue,
template, fatti di mondo) e il motore fisso le interroga. Ma una parte della
comprensione non è una forma, è una **procedura**: come si riordina una domanda,
come si sceglie ser/estar, come si coniuga, come si somma, come si riscrive un
termine. Oggi quelle procedure vivono in C — un consumer per costrutto — perché il
linguaggio KB (fatti + clausole di Horn su **atomi piatti**) sa esprimere il *cosa*,
non il *come computare* (teach-comprehension §2, Secchio B e D.1).

**La svolta:** portare anche le PROCEDURE dentro il KB. Non "insegnare un algoritmo"
a mano, ma dare al motore un **substrato di riscrittura** che interpreta regole di
trasformazione **espresse come dato**. Allora:

> conoscenza = **fatti** (cosa è vero) **+ procedure** (come trasformare ciò che è
> vero in altro). Entrambe insegnabili a runtime via MCP, entrambe KB. Il motore
> resta fisso: diventa un **interprete di regole**, non un catalogo di consumer.

Un costrutto nuovo — grammaticale, aritmetico, di codice — diventa **una regola
insegnata**, non C nuovo. È l'essenza massima di "engine fixed, knowledge learns".

---

## 1. Perché oggi non si può (la diagnosi che diventa rotta)

Da teach-comprehension §2, misurato dal vivo:

- **Secchio B — la colla grammaticale è C per disegno.** Un fatto descrive *cosa*,
  non *come computare*; fronting/inversione/ser-estar sono **algoritmi di
  ristrutturazione**, non tabelle. Il piano stesso ammette: *potrebbero migrare a
  forma dichiarativa interrogata da un kernel generico* — è precisamente questa rotta.
- **Secchio D.1 — non puoi insegnare a CALCOLARE, solo a RICORDARE.** Il motore non
  ha **termini composti** (liste/strutture): `sum(2,1,3)` si memorizza, l'addizione
  no; `add(s(z),s(z),?)` non scatta perché `s(z)` è un atomo piatto, non `s($N)` che
  unifica. Senza strutture, una regola non può **matchare e riscrivere** una forma.

Le due cose sono la stessa mancanza: **manca un linguaggio per esprimere
trasformazioni su strutture, e un interprete che le esegua.** Ecco cosa costruiamo.

---

## 2. L'architettura: un interprete di riscrittura KB-driven

Tre strati, additivi sul motore esistente.

### 2.1 Strato T — termini strutturati (il prerequisito, "U3")
Dare al KB **termini composti**: liste `[a, b, $X]` e funtori `s($N)`, `conj($S, are)`.
Unificazione che scende nella struttura (un `$X` lega un sotto-termine, incluso uno
span). È la porta di D.1: senza questo, non si esprime né grammatica-come-regola né
computazione. Incrementale: prima le LISTE (bastano alla riscrittura di token), poi i
funtori generici (bastano a Peano/computazione).

### 2.2 Strato R — regole di trasformazione come DATO
Una procedura è una regola `rewrite(Pattern, Result)` (o `Head :- Body`) dove Pattern
e Result sono termini con variabili. Esempi che oggi sono C e domani sono **fatti
insegnati**:

```
% grammatica: fronting + de-inversione di "where are you from" -> "de dónde eres"
rewrite([where, are, $S, from], [de, dónde, conj($S, are)]).
% ser/estar come regola, non ternario C
copula_es($N, is, es)   :- ser_noun($N).
copula_es($N, is, está) :- loc_noun($N).
% computazione: addizione di Peano (D.1) come regola ricorsiva
add(z, $N, $N).
add(s($M), $N, s($R)) :- add($M, $N, $R).
```

Ogni regola è asseribile via MCP (`kb.assert_clause`), quindi **insegnabile**.

### 2.3 Strato I — l'interprete generico (l'unico C che cresce)
Un kernel fisso che: prende un input (token list, o un goal), **cerca la regola che
unifica**, applica la sostituzione, valuta ricorsivamente il Result, e restituisce
l'output + un **proof** (la sequenza di regole applicate). Un solo motore per
grammatica, calcolo, codice. I consumer-per-costrutto di oggi diventano casi di
questo interprete; restano come **struttura secondaria** (keep-and-select) finché la
regola equivalente non è insegnata e verificata.

---

## 3. La rotta (milestone, uno per generazione, tirato da un fallimento reale)

- **P0 — dimostratore di riscrittura su liste (FATTO, gen311).** `apply_token_rewrite`
  in `85-translate-synth-world.c` interpreta `rewrite_es(LHS, RHS)` (fatti insegnati,
  pattern di token con var). Il fronting `rewrite_es("where are $s from", "from where
  $s are")` -> "de dónde eres" gira ORA via la regola, non il C (che resta fallback
  keep-and-select). Whitelisted per autolearn.
- **P1 — variabili su SPAN (FATTO, gen311).** Un `$x` lega ≥1 token, ancorato dai
  letterali vicini. `rewrite_es("what's $x", "what is $x")` espande la contrazione e il
  resto ($x="your name") aggancia la frase esistente -> "Cómo te llamas". Lunghezze
  diverse permesse.
- **P2 — funtori + ricorsione (DIMOSTRATO, gen311).** SCOPERTA: il motore ha GIÀ i
  termini composti (U3 landato dopo gen280). Misurato dal vivo: `add/3` di Peano e
  `len/3` (curate in grammar.p0) **COMPUTANO** — `add(s(z),s(z),?) -> s(s(z))` (1+1=2),
  `len(cons(a,cons(b,cons(c,nil))),?) -> s(s(s(z)))` (conta 3), `app/3` concatena. Si
  insegna a CALCOLARE, non solo a ricordare. Var-sigil: `$`/`_` (kb.c:95). **Bucket A
  CHIUSO (gen311):** `kb.assert_clause` ora parsa i termini composti negli args
  (`build_clause_args` passa as-is ogni stringa con `$`) e accetta un body vuoto
  (fatto-con-variabili, il caso base `add(z,$N,$N)`); `kb.assert` resta per i fatti
  ground ($-literal preservati). Dimostrato: `add/3` INSEGNATO via MCP calcola
  `add(s(s(s(z))),s(s(z)),?) -> s(s(s(s(s(z)))))` (3+2=5). Quindi **autolearn può
  insegnare CALCOLO a runtime.** RESTA solo (b): collegare le domande NL (word-problem)
  al motore di calcolo (routing/parsing NL->termini).
- **P3 — proof e provenienza:** ogni trasformazione porta la sua derivazione
  (`fact_source`), ritrattabile (auto-correzione, deep-reasoning M4).
- **P4 — migrazione:** i costrutti C di questa sessione (conj/fronting/ser-estar)
  ri-espressi come regole insegnate; il consumer C resta secondario finché la regola
  non è verificata dall'oracolo.
- **P5 — cross-dominio:** stesso interprete su codice e scienza (aggancio a
  [[universal-solver]]: inferenza + riscrittura sono lo stesso motore).

---

## 4. Invarianti (immutati)

1. **Motore fisso, conoscenza che cresce** — ora "conoscenza" include le procedure.
2. **Proof o niente** — ogni trasformazione porta la catena di regole.
3. **Verify-before-persist** — una regola insegnata entra ufficiale solo se l'oracolo
   di dominio la valida.
4. **Keep-and-select** — i consumer C restano come fallback secondario durante la
   migrazione, mai rimossi prematuramente.
5. **Niente algoritmi a mano via MCP travestiti** — si insegnano REGOLE dichiarative
   che l'interprete esegue, non si scrive C da remoto (mcp-engine §7.3/§9).

---

## 5. Perché è IL passo (non una deviazione)

teach-comprehension chiude dicendo che il Secchio B "potrebbe migrare a forma
dichiarativa interrogata da un kernel generico" e che D.1 si sblocca con i termini
composti (U3, "la porta per insegnare *computazione*"). Questo documento prende quelle
due frasi e le rende **la rotta principale**. Il super-solver universale
([[universal-solver]]) ne è l'applicazione: inferenza e riscrittura su strutture sono
lo stesso interprete. Realizzare P0→P2 significa che parrot0 **impara a fare cose** —
grammatica, calcolo, e la coda lunga futura — ricevendo **regole**, non ricompilando.
È KB-first portato al suo estremo: anche il *come* è conoscenza.

---

## 6. Il metodo, raccontato — come si RAGIONA per applicare KB-first

Questa decisione è nata da un caso concreto ed è il **modello riusabile** del
ragionamento KB-first ([[kb-first-manifesto]] è la bussola; questo ne è un esempio
vivo). La sequenza — da tenere a mente ogni volta che parrot0 mura una capacità:

1. **Sintomo.** autolearn (il ledger dei gap) continua a fallire su una classe:
   frasi interrogative, coniugazioni, ser/estar. Il teacher o *inventa predicati
   morti* o *memorizza la frase intera* — due scorciatoie che tradiscono la tesi.
2. **Scavo.** Chiedendo a parrot0 stesso *"perché hai risposto così?"* e leggendo il
   C, si scopre il vero motivo: **conoscenza estesa CABLATA nel C**. Ogni costrutto
   grammaticale era un *consumer C* con una tabella hardcoded (la mappa
   soprannome→pianeta, il drop dell'ausiliare, la selezione ser/estar). Non era
   motore fisso: era *conoscenza travestita da codice*.
3. **Prima mossa (giusta ma parziale).** Si astrae il PEZZO-DATO: la tabella diventa
   fatti (`aux_question/1`, `conj_es/3`, `wh_front_es/2`, `planet_superlative/3`), il
   C resta solo come *interprete di quei fatti*. autolearn ora insegna quei fatti.
   Abbiamo spostato il confine: la FORMA è dato.
4. **Il muro che emerge.** Ma ogni costrutto NUOVO richiede ancora un *consumer C
   nuovo*, perché ciò che resta in C non è una tabella: è la **procedura** (riordina,
   coniuga, seleziona). E il linguaggio KB (atomi piatti, Horn) sa esprimere il
   *cosa*, non il *come* (teach-comprehension §2, Secchio B/D.1). Il confine si è
   spostato ma non è sparito.
5. **La decisione (astrarre il KB per OSPITARE anche le procedure).** Invece di
   posare all'infinito un mattone-C per costrutto, si **estende la KB stessa** perché
   possa ospitare le procedure: termini composti (U3) + un **interprete di riscrittura
   generico**. Allora la procedura diventa una *regola insegnata*, e il C smette di
   crescere per-costrutto. È lo stesso passo del punto 3, applicato un livello più su:
   *prima abbiamo astratto le forme, ora astraiamo le trasformazioni.*

**La legge del metodo:** quando trovi conoscenza cablata in C, non chiederti "come
aggiungo il prossimo caso in C?" ma *"quale rappresentazione KB ospiterebbe questa
CLASSE, lasciando in C solo un interprete generico?"*. Se la classe è di FORME →
tabella di fatti. Se è di PROCEDURE → regole di trasformazione + interprete. Il C
fisso si restringe, la conoscenza (fatti **e** procedure) cresce. Questo è il cuore
di "engine fixed, knowledge learns", portato fino alle procedure.

---

## 7. Gap ledger — cosa manca (aggiornato gen311)

Registro onesto dei buchi, per non perderli. Due famiglie.

### 7a. Costrutti grammaticali ancora FUORI dal toolkit (frontiera del ledger autolearn)
Ognuno oggi murerebbe o servirebbe un consumer C; il pivot (regole `rewrite_*` +
P2) è la via per renderli dato insegnabile.

- **Fronting + INVERSIONE francese** — "Where are you from?" FR → "d'où venez-vous?".
  Il francese non ha pro-drop: serve l'inversione soggetto-verbo, non solo il
  riordino. `rewrite_fr` + una regola d'inversione.
- **ser/estar (ES)** — "My name is X" → "mi nombre es X" (ser, identità) vs "está"
  (estar, luogo/stato). Selezione del copula per contesto: oggi solo `tr_es(is, esta)`.
  Candidato a regola `copula_es($N, is, es) :- ser_noun($N).`
- **Clitico oggetto in DOMANDA (FR)** — "Can you help me?" → "pouvez-vous m'aider"
  (aux + infinito + clitico `m'`). La macchina clitica FR esiste per un'altra posizione.
- **Contrazione wh (ES)** — "What's your email?" → "cuál es tu correo". P1 espande
  "what's"→"what is"; manca il mapping "what is"→"cuál es" + lessico email.
- **Pass-through dei nomi propri** — il compositore declina su una parola ignota (un
  nome proprio "John") invece di lasciarla passare. Serve una regola di default.

### 7b. Categorie deboli di LLMSCORE (misurato: 5/10, gen311) — l'obiettivo ≥8/10
Le domande RUOTANO: si chiudono CATEGORIE, non prompt ([[categories-not-prompts]]).

- **Word-problem matematici** (Q2 "22" invece di 24; Q4 treni, declinato) — richiede
  CALCOLO, non ricordo: è **P2** (termini composti + ricorsione, D.1 di
  teach-comprehension). La leva più grande e più lontana.
- **Completamento di frase** ("...she reached for her ___" → umbrella) — buon senso
  generativo; candidato a template di completamento + collocazioni insegnabili.
- **Puzzle laterali** ("il bicchiere trabocca ma nulla è stato aggiunto") — abduzione
  su modello fisico; è il [[universal-solver]] (inferenza), gemello del rewrite.
- **Aperte / etiche** ("cosa fai se trovi un portafoglio?") — risposta sensata
  soggettiva; la più lontana dalla portata KB-first onesta, forse un template di
  ragionamento pratico.

**Priorità verso 8/10:** (1) misurare il baseline ORA (post-miglioramenti); (2)
chiudere le categorie più economiche che il draw pesca spesso (traduzione, riddle,
superlativi, definizioni — già irrobustite; completamento-frase è il prossimo
economico); (3) P2 per i word-problem, la leva strutturale.
