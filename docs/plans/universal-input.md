# L'input è UNO — nessun flusso di testo si classifica nel C

> **Stato storico:** il primo livello è stato chiuso a gen332 (2026-07-12), ma
> la decisione del 25 agosto 2026 riapre la migrazione completa: tutti i percorsi
> linguistici C residui sono obsoleti. La norma operativa e il censimento sono in
> [`kb-first-c-gold-standard.md`](kb-first-c-gold-standard.md).
> Il debito era stato riconosciuto a
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

> ## ✅ TRAGUARDO — LO SPAZIO DEL DISCORSO (2026-08-31)
>
> **Da conservare: è la prima volta che parrot0 ricorda *che cosa* è stato
> nominato e *in che ordine*, e che un'espressione può riferirsi a quella
> memoria invece che a una parola.**
>
> ```text
> > Il libro rosso è sul tavolo.      ->  located_in(book_red, tavolo)
> > Il quaderno blu è sulla mensola.  ->  located_in(quaderno_blue, mensola)
> > Dov'è il primo?                   ->  tavolo        (prima: muro)
> > Dov'è il secondo?                 ->  mensola       (prima: muro)
> ```
>
> ### Che cos'è, esattamente
>
> Poco, di proposito: `discourse_referent(Ordine, Chiave)`. Una cosa nominata e
> la sua posizione nel discorso. Testa e proprietà si ricavano dalla chiave (G2);
> determinante, span e superficie originale non ci sono ancora (G5).
>
> Ma è la prima **memoria del dialogo che non è una lista di frasi**: è una lista
> di *cose*. Fino a ieri parrot0 conservava turni; ora conserva referenti.
>
> ### A che cosa serve, e che cosa abilita
>
> Non è una funzione in più: è il posto a cui si attaccano cose che prima non
> avevano appiglio.
>
> | abilita | perché prima non si poteva |
> |---|---|
> | **coreferenza** — «quello», «l'altro», «quello di prima» | non esisteva l'oggetto a cui riferirsi: F03 era la famiglia peggiore del corpus, 24 muri su 30 |
> | **ellissi** — «E il secondo?» senza ripetere il verbo | il turno ellittico non ha entità da nominare: deve prenderla dal discorso |
> | **correzione** — «no, quello rosso l'ho spostato» | correggere richiede di individuare *che cosa* si corregge, non solo che si corregge |
> | **ambiguità dicibile** — «Quale? …» invece di un muro | serve saper elencare i candidati, cioè averli |
> | **il soggetto eliso** (SC5) e **l'apposizione** | entrambi recuperano un ruolo da qualcosa già introdotto |
> | **la domanda di seguito** — «e dove si trova adesso?» | «adesso» presuppone una cosa di cui si stava parlando |
>
> Ed è la precondizione dichiarata di **GD4** (riferimento cross-turn) e di
> **D37/G4-G5**: il referente con proprietà, e il referente che sa ridirsi.
>
> ### Come è stato costruito — le tre cose che hanno deciso l'esito
>
> Vale più del risultato, perché sono riusabili:
>
> 1. **Il punto di strozzatura condiviso.** Le vie che imparano un fatto sono
>    più d'una — lo schema dichiarato, la copula binaria, il locativo — e la
>    prima versione agganciava i referenti a *una*. Misurato: il locativo
>    italiano non registrava niente, e metà del dialogo restava senza memoria.
>    L'osservazione sta ora in `p0_learn_source`, che **tutte** attraversano
>    perché registrare la provenienza è ciò che ogni via fa comunque. *Un
>    referente è esattamente questo: una cosa nominata, e quando.* Cercare il
>    punto che tutti attraversano invece di enumerare i chiamanti è la stessa
>    mossa della fase pura di SC2-B.
> 2. **Quale posizione introduca un referente è una politica, non una scelta del
>    C.** Registrando *ogni* argomento, «il secondo» diventava il **tavolo**
>    invece del quaderno: in «Il libro rosso è sul tavolo» sono nominati due
>    oggetti, ma quello di cui si parla è il primo. `referent_arg_position/1` lo
>    dichiara, e una relazione con un'altra geometria costa una riga.
> 3. **La superficie da dichiarare è quella che sopravvive al percorso.** «primo»
>    arriva al matcher come «prime» — la canonicalizzazione lo traduce — e la
>    forma col determinante non combacia più. È la **terza volta** che questa
>    lezione si presenta (dopo le cue di SC2-B e le locuzioni di SC2-D): finché
>    la canonicalizzazione non conserva anche l'originale (G5), una classe di
>    superfici deve tenere *entrambe* le forme.
>
> ### Il limite onesto
>
> È la **prima forma**, non la forma finale. Non c'è ancora il determinante (che
> distingue introdurre da riprendere), non c'è lo span, non c'è la superficie
> originale — quindi parrot0 sa che il libro rosso è stato nominato per primo e
> non sa ancora ridirlo «il libro rosso». E due referenti con la stessa testa si
> distinguono per proprietà (G2) ma non hanno ancora identità propria.


> ## ⛔⛔ IL CASSETTO SENZA MANIGLIA — il problema che teneva ferma la comprensione universale
>
> **Scoperto e misurato il 2026-08-31. Scritto in tutti i piani perché è la
> giunzione da cui dipendono `universal-input`, `universal-comprehension` e il
> fronte SC/GD insieme.**
>
> ### Il problema, in cinque righe di transcript
>
> ```text
> > Il libro rosso è sul tavolo.   ->  Learned: located_in(book_red, tavolo).
> > dove si trova il libro rosso   ->  muro
> > dove si trova book_red         ->  Tavolo.      ← solo col nome INTERNO
> > Il gatto è sul tetto.
> > dove si trova il gatto         ->  Tetto.       ← una parola sola: funziona
> ```
>
> Il **lettore** lega un *sintagma*: unisce i token fino al confine di sintagma e
> produce una chiave sola (`book_red`). La **domanda** provava un token alla
> volta — `located_in(il,?)`, `located_in(libro,?)`, `located_in(rosso,?)` — e
> non provava **mai** la frase intera. Il fatto c'era e non era raggiungibile.
>
> > **parrot0 imparava sotto un nome che non sapeva più pronunciare**, e ogni
> > entità di più di una parola finiva in un cassetto senza maniglia.
>
> ### Perché era il collo di tutto
>
> Due giri di insegnamento massiccio — 276 forme reali entrate parlando,
> verificate in processo nuovo — avevano mosso **+11 turni su 360**. Non perché
> il metodo fosse debole: perché un turno riesce solo se tengono **insieme**
> superficie, forma della domanda, nome dell'entità, riferimento, fatto e
> realizzazione, e il nome dell'entità cedeva sempre. I referenti multi-parola
> («il libro rosso», «il quaderno blu», «il treno notturno») sono la norma del
> parlato, non un caso limite — ed è anche il motivo per cui la coreferenza era
> la famiglia peggiore del corpus: non aveva **niente a cui attaccarsi**.
>
> `book_red` non è un nome scomodo: è un nome che ha **perso informazione**.
> Testa fusa col modificatore, determinante buttato, ordine invertito, lingua
> cambiata a metà — e ogni perdita chiude una porta diversa (chiedere «quale
> libro?», distinguere «un libro» da «il libro», risolvere «il primo»,
> ripronunciarlo come è stato detto).
>
> ### Il piano di soluzione — la giunzione in cinque gradini
>
> L'invariante che li governa tutti:
>
> > **ciò che si impara da una frase dev'essere interrogabile con la stessa
> > frase, e ridicibile come è stato detto.**
>
> | # | gradino | stato |
> |---|---|---|
> | **G1** | **La domanda prova i sintagmi che il lettore ha costruito.** Non un secondo indice: le *stesse tre cose* del lettore — confine di sintagma (`np_closer/1`, conoscenza), caduta del determinante, la stessa `p0_join`. Additivo: i passaggi per token restano. | ✅ **FATTO** 2026-08-31 |
> | **G2** | **Testa e proprietà.** «il libro rosso» → testa `libro` + proprietà `rosso`, così «il libro» combacia e «di che colore è il libro» risponde. Dov'è la testa è **conoscenza** (`noun_phrase_head_position(Language, first \| last)`), non una regola nel C. | aperto |
> | **G3** | **Il referente.** Un'entità introdotta occupa una **posizione nel discorso**: parrot0 ricorda che cosa è stato nominato e in che ordine, e «il primo»/«il secondo» ci si attaccano. Quale posizione di un fatto introduca un referente è una **politica** (`referent_arg_position/1`), non una scelta del C. | ✅ **FATTO** 2026-08-31 (prima forma: ordine + chiave; determinante e span restano G5) |
> | **G4** | **La coreferenza si attacca al referente.** «il primo», «quello», «l'altro» diventano `referent_same/3` — una **relazione**, non una fusione. | aperto |
> | **G5** | **Il referente sa ridirsi.** `referent_surface/3`: rispondere «il tavolo» come è stato detto, non `tavolo`. | aperto |
>
> ### Come si misura che funziona
>
> **Non con una percentuale sul totale: con una famiglia che chiude.** Il gate è
> che il dialogo `gd1_011` — cinque turni, due oggetti, un riferimento — passi
> **da capo a fondo**. Una catena che regge vale più di dieci punti sparsi,
> perché dieci punti sparsi non provano che nessuna catena regga.
>
> ### La forma ricorrente, che è la lezione vera
>
> È la **terza volta** che compare lo stesso difetto sotto un vestito diverso:
> D33 (un'interpretazione congelata perché la KB non può richiamare la lettura),
> D35 (una chiave costruita da un percorso e non dall'altro), D37 (una struttura
> costruita da un percorso e ignorata dall'altro).
>
> > **Due percorsi che devono accordarsi, e non condividono l'oggetto su cui
> > accordarsi.**
>
> Prima di aggiungere una capacità, la domanda da farsi è: *chi altro deve
> accordarsi con questa, e su che cosa?*
>
> E la stessa mossa, un piano più su, è **l'unificazione fra zone della KB**
> (D38, §18.43): far parlare aritmetica e sociale, prosa e geografia, non è
> ingegneria di dettaglio — è la condizione perché emergano abilità che nessuno
> ha progettato. Differenziarsi non basta: le parti differenziate devono potersi
> parlare.
>
> Dettaglio completo: `docs/plans/frontier-kb-natural-dialogue.md` §18.40 (D35),
> §18.42 (D37) · referto
> `docs/labs/apprendimento-assistito/2026-08-31-perche-non-cresceva.md` · coda
> `LEARN_TODO.md` GD9.


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

## 4bis. Dalle span ai sintagmi: la struttura interna resta universale

La segmentazione del registro è il primo livello, non l'ultimo. Una span marcata
`prose` deve poter essere scomposta senza introdurre un parser per ogni dominio:

```text
flusso
  -> segmento/register span
  -> token span
  -> sintagmi (NP, VP, PP, clause)
  -> relazioni fra sintagmi (subject, predicate, object, modifier)
  -> frame o intent schema della KB
```

Il C produce soltanto la struttura meccanica: token, offset, confini candidati,
nesting e legame fra span. La KB decide come interpretarla:

```prolog
pos(elephant, noun).
pos(weighs, verb).
phrase_form(np, "the @N").
phrase_form(pp, "in @NP").
phrase_role(subject, np).
phrase_role(object, np).
intent_schema("@NP @V @NP", assertion).
```

Questa è la giunzione con [[universal-comprehension]]: il suo `skeleton
sintattico`, gli `intent_schema/2` e l'assegnazione dei ruoli operano sulle span
gerarchiche prodotte da questo documento. `extract_frame/2` consuma la stessa
struttura per trasformare una frase in fatto; `answer_frame/2` usa il frame
opposto per trasformare una domanda in query.

Il nuovo livello non significa che ogni token debba essere già conosciuto: una
parola ignota conserva la propria posizione e può riempire uno slot. La KB può
poi insegnare `pos/2`, una forma di sintagma o un collegamento e la stessa frase
può essere risegmentata senza ricompilare.

### Perché evolvere così la KB

- La stessa segmentazione serve a prosa, domande, codice, log e diff.
- Le forme nuove costano fatti KB, non rami C.
- Un fatto estratto conserva gli offset e la frase sorgente, quindi può essere
  verificato, corretto o ritratto.
- Relative, apposizioni e misure diventano composizioni di sintagmi, non casi
  speciali del dominio.
- Una forma appresa migliora sia la lettura della prosa sia la comprensione delle
  domande che interrogano il fatto.
- Le ambiguità restano ipotesi confrontabili con proof, invece di essere risolte
  dal primo `if`.

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
- **Il C linguistico esistente non è una seconda struttura permanente.** La
  migrazione può essere additiva soltanto finché serve a confrontare i due
  percorsi. Dopo crescita, ablazione, multilingua e provenance, l'adapter viene
  rimosso. `code_segment()` può restare come meccanica sopra `input_segment()`;
  un recognizer che nomina parole, keyword o ruoli non può restare come fallback.
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

- [ ] **U9 — Struttura gerarchica condivisa.** Esporre token e sintagmi come span
      annidate, con POS, ruoli e legami dichiarati dalla KB, e far consumare la
      stessa IR a `intent_schema/2` ed `extract_frame/2`. Oracolo: una forma
      insegnata a runtime cambia la struttura e l'estrazione, mentre la sua
      retract la rimuove senza toccare il C.

## 8bis. Piano di migrazione: C come kernel, KB come grammatica

Questo è il piano operativo per portare il massimo della comprensione nella KB
senza trasformare il C in un parser di dominio. Il documento è collegato a
[[universal-comprehension]]: quella pagina definisce come la struttura rivela
l'intento; questa definisce come la struttura viene materialmente costruita.

### Confine definitivo

Il C conserva soltanto:

- lettura dei byte e tokenizzazione;
- offset, lunghezze e nesting delle span;
- struttura dell'IR e collegamenti parent/child;
- unificazione, binding degli slot e iterazione sui candidati KB;
- scoring generico, provenance e gestione di tie/declino.

La KB contiene:

- POS, classi lessicali, opener/closer e marcatori di clausola;
- forme `NP`, `VP`, `PP`, apposizione, relativa, passiva e coordinazione;
- ruoli `subject`, `predicate`, `object`, `modifier` e vincoli di binding;
- `intent_schema/2`, `extract_frame/2` e mapping verso predicati;
- regole di composizione e trasformazione, sopra i primitivi del motore;
- le forme equivalenti con cui lo stesso fatto viene chiesto o dichiarato.

Il C non deve sapere che una parola è un verbo, che una preposizione chiude un
NP o che una struttura è una relativa: deve chiedere alla KB quali ipotesi
strutturali provare.

### Incrementi

- [x] **M0 — IR gerarchica minima.** Pubblicare `clause`, `phrase` e `token` con
      offset, superficie e parent; mantenere `turn_span_token/4` come vista
      compatibile.
- [ ] **M1 — Confini dichiarativi.** Sostituire ogni query diretta a classi
      sintattiche con relazioni KB generiche (`phrase_boundary/3`, `pos/2`,
      `clause_marker/2`).
- [ ] **M2 — Matcher strutturale.** Far combaciare `intent_schema/2` ed
      `extract_frame/2` contro nodi e ruoli dell'IR, non contro una nuova lista
      di parole in C.
- [ ] **M3 — Grammatica compositiva.** Aggiungere in KB regole per apposizioni,
      relative, passive, coordinazioni e quantità; il C esegue solo unificazione
      e binding.
- [ ] **M4 — Doppio uso.** Usare lo stesso frame per estrarre un fatto dalla
      prosa e costruire la query corrispondente; `answer_frame/2` resta una vista
      interrogativa del frame.
- [ ] **M5 — Apprendimento runtime.** Insegnare/retrarre POS, forma o frame in
      una sessione e osservare il cambiamento della IR e dell'estrazione senza
      rebuild.

### Vantaggio misurabile

Una nuova lingua, forma o relazione deve costare fatti KB e non codice C; una
nuova struttura deve riusare lo stesso kernel per prosa, domande, log e codice.
Il progresso si misura su un corpus annotato: span corrette, slot corretti,
fatti corretti e provenance completa, con holdout separato per evitare una KB
che memorizza soltanto gli esempi.

## 9. L'oracolo del piano intero

Il piano è chiuso quando **tutte** queste sono vere insieme:

1. `grep -rn 'strstr(low, "' src/code.c` sui ruoli/registri → **zero righe**.
2. Un registro nuovo (JSON, diff, log) entra con **soli fatti**, provato da un test
   che non tocca il C.
3. `tests/segment.sh` (28/28) e `codebench` (25/25) restano verdi: **nessuna
   regressione di comportamento** in cambio della forma giusta.
4. Ogni span risponde a «perché?» con un fatto, e una correzione la ritratta.
5. U9 è verde: token, sintagmi e clausole condividono una IR con
   `universal-comprehension`, e una forma KB insegnata o ritratta cambia davvero
   sia la struttura sia l'estrazione.

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

## 10. gen396: dalle span alla memoria di lavoro universale

La segmentazione non termina quando ha assegnato un registro. Ogni turno viene
ora proiettato nella KB con tre viste meccaniche:

```prolog
turn_span(current_turn, 0, condition, "milano").
turn_span_cue(current_turn, 0, keyword(se)).
turn_span_surface(current_turn, 0, "se milano").
```

Il C conserva offset, ordine e l'evidenza esatta scelta dallo scorer; non
conosce `condition`, `reply`, `se`, il predicato della proposizione o il modo di
rispondere. Questi fatti sono memoria di lavoro riflessiva, sostituita al turno
successivo e marcata `machinery/1`.

Il primo consumer completo e' il piano condizionale in
`kb/core/conditional-plans.p0`. E' deliberatamente un esempio, non una nuova
grammatica privilegiata: le stesse viste devono comporre input misti come:

```text
turn_span(T, 0, code(c),    "i=0; i++;")
turn_span(T, 1, query,      "i")

turn_span(T, 0, code(c),    "...")
turn_span(T, 1, observed,   "ritorna 0")
turn_span(T, 2, expected,   "dovrebbe ritornare 1")
turn_span(T, 3, constraint, "senza cambiare l'API")
```

Il contratto per l'evoluzione NL -> coding e' quindi:

1. **percezione:** delimitare e tipizzare senza consumare la domanda;
2. **denotazione:** legare superfici e simboli a relazioni candidate;
3. **semantica operativa:** derivare statement, stato, transizioni, effetti e
   vincoli con regole KB sopra primitive fisse;
4. **piano:** conservare tutti gli obblighi del turno e le loro dipendenze;
5. **verifica:** chiudere ogni obbligo con proof, gap tipizzato o chiarimento;
6. **realizzazione:** scegliere lingua e registro senza cambiare i claim.

Un nuovo costrutto linguistico o di codice deve entrare aggiungendo membri a
queste relazioni e superare crescita + ablazione runtime. Il motore puo' offrire
tokenizzazione, ordering, binding, aritmetica e inferenza; non puo' sapere che
una parola o un operatore concreto significa uno di questi atti. In particolare
`register(code(c))` resta un'osservazione, mai una risposta sufficiente: uno span
`query` mantiene aperta l'obbligazione finche' il piano non ne prova il valore.
