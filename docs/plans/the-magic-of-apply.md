# The magic of `apply` — il predicato variabile, e perché rende viva la KB

> **F., 12 settembre 2026:** «spiega la `apply`, tutte le cose che ci ha
> permesso di fare, le sue basi teorico-pratiche, e quali possibili altre
> applicazioni potrebbe avere nell'espansione dei circuiti di inferenza e
> nell'aumento dell'accessibilità alla KB. Io sono convinto che la `apply`
> assieme ad altre tecniche possa davvero rendere la KB viva.»

La missione è la **KB viva**: apprendere deve cambiare ciò che parrot0 riesce
a fare, trovare, spiegare e correggere. Questo documento distingue il meccanismo,
le capacità esistenti, l'incremento eseguibile e le capacità ancora mancanti.

**Revisione operativa, 12 settembre 2026.** La prima stesura proponeva di
costruire strumenti che in parte esistevano già. `between_rel/3`, `about/3`,
`holds/3`, le definizioni componibili e alcune verifiche dei piani erano già in
KB. Il lavoro qui descritto estende quel circuito: catalogo interrogabile,
ricerca oltre i soli verbi, nomi composti nelle domande e spiegazione di `apply`.
Non certifica abduzione generale o autonomia nell'apprendimento.


## Handoff — riprendere da qui

**Stato al 12 settembre 2026, seconda sessione.** La prima sessione aveva
consegnato la patch su `main` (`632a070e`) senza `soft-test` né test adiacenti
e con un difetto aperto: la lettura di una frase che usa un nome relazionale
**insegnato**. Questa sessione ha eseguito il piano di verifica e chiuso il
difetto. La missione resta la KB viva; non ricominciare dal progetto di un
secondo solver o di un secondo lettore.

### Che cosa è stato verificato (punti 2–4 del vecchio handoff)

- **Nessuna regressione della patch `apply`.** `make soft-test` è rosso, ma
  identico sul commit precedente `742327d6`: `basics.p0t` sfora il limite di
  1 s su due turni. Stesso esito per `howknow.p0t`, `taught_lesson_form.p0t`
  115/131, `deepreason.p0t`, `taught_lexicon.p0t`. Dettaglio in `TEST_TODO.md`
  in testa. Verdi: `facts`, `model_graph`, `materialized_view`,
  `higher_order_lesson`, `structural_reader_live`, `taught_turn_form`.
- **Ablazione isolata del modo `atom`.** Con la forma, la precedenza e la
  risposta intatte, ritirare soltanto `turn_form_slot_form(F, subject, atom)`
  fa tornare «haven't established» su *calcium carbonate* e *william
  shakespeare*. Il contratto è necessario, non decorativo.
- **Portatori reali via `capability_call`.** `shape_formula`, `law_formula`,
  `law_prose` e `mechanics_concept` rispondono attraverso il contratto; famiglia
  o arità sbagliata non producono prove. `taught_formula` è registrato e vuoto
  al boot, come deve: nasce dalle lezioni.

### Il difetto nominale: chiuso, ed era di classe

Il vecchio handoff chiedeva di «trovare chi non usa lo schema». Lo schema
`the weft of @S is @O` esisteva e `!query` lo derivava, ma il **lettore** non lo
vedeva. `p0_frame_patterns` (`src/brain/10-memory-knowledge.c`) tiene in cache
gli schemi di `extract_frame/2`. La chiave era la **stazza di quattro famiglie
scritte nel C**: `relation_verb`, `verb_particle`, `irregular_verb_form`,
`past`. «weft is a relation» genera lo schema passando per `relation/1` e
`relation_noun/2`, assenti dall'elenco. La cache non cadeva, e la frase finiva
nel lettore delle classi: `weft(flax)`. Contare i fatti, inoltre, non vede un
ritiro seguito da un'aggiunta.

Non era un buco di `weft`: valeva per **ogni famiglia generatrice non elencata**,
compresi i fatti `extract_frame` insegnati e le regole future. Era la stessa
lista che il gen510 aveva già sbagliato una volta, sull'arità di `relation_verb`.

**Il rimedio non aggiunge `relation` alla lista: la toglie.** Il motore mantiene
già, per la vista `materialized_view(extract_frame, 2)`, il grafo delle
dipendenze derivato dai corpi delle regole più `view_depends/2`, e la invalida
sul cambiamento. Ora ogni invalidazione stampa un orologio monotono
(`KbView.stamp`, `kb_view_stamp` in `src/kb.h`), e la cache del Brain usa quel
timbro. Una famiglia generatrice nuova, fatto o regola, la fa cadere senza
toccare il C. Senza la vista dichiarata la cache non ha chiave e si rideriva:
più lento, mai cieco. Misurato: una sola ricostruzione per `!reset`, nessuna
per turno; tempi di `basics.p0t` invariati.

**Secondo difetto trovato nel percorso, stessa classe «dire ≠ scrivere»:** il
lettore posizionale `X is the R of Y` salvava `currency_of(ruritania, zlot)`
nel verso dichiarato, ma annunciava «the currency of zlot is ruritania». Ora
annuncia gli argomenti scritti. È un misclaim su ciò che si è appena imparato,
cioè il caso peggiore del mantra #7.

Prova in `living_capabilities.p0t`, ora **55 asserti**: lettura multiparola,
domanda, ricerca della relazione, ritiro con caduta della cache (una frase
nuova non si legge più come `weft_of`), conferma nel verso del fatto. **I
blocchi nuovi falliscono 9 asserti sul C precedente**: la verifica differenziale
è stata eseguita, non presunta.

### Reperti lasciati aperti (non inseguiti: un circuito per sessione)

1. **La lezione di formula apprende e risponde con un muro.** «the power is
   work divided by time» asserisce `taught_formula(power, power, …)`, ma la
   risposta è «Hmm, I don't know about divided yet…»; «the momentum is mass
   times velocity», l'esempio canonico di `model-lesson.p0`, risponde «I don't
   understand that yet». Il file dichiara `turn_response/2` proprio per evitare
   questo muro: la risposta si perde nell'arbitrato. «forget that …» non ritira
   la formula. **Nessun `.p0t` copre `model_lesson`**, quindi la regressione è
   passata in silenzio. Chiuderla richiede prima di tutto un cricchetto.
2. **`make soft-test` rosso per tempo** (preesistente): profilare
   «is a tiger a mammal» con `/debug` prima di toccare altro.
3. **La resa vuota mostra la chiave**: a modo `atom` ritirato, `between_unknown`
   dice «from calcium_carbonate to oxygen», con il trattino basso.
4. **Esiti epistemici ed albero di prova strutturato** (Parte V §5.1, §5.3):
   restano il prossimo incremento del motore. `kb_rule_body/2` da solo **non**
   dimostra che una premessa sbloccherebbe una conclusione.

### Prossime azioni, in ordine

1. Leggere `MANTRA.md`, poi `git log -3` e `git show` per questa consegna.
2. Reperto 1: scrivere il `.p0t` della lezione di formula (lezione, risposta,
   codice, ritiro), vederlo rosso, poi cercare chi ruba il turno. Per il #21
   la domanda è se il ladro è un modulo immaturo, da retrocedere, o maturo, a
   cui insegnare una cessione.
3. **Massimizzare il circuito nominale** (mantra #22): altri nomi relazionali
   insegnati in forme diverse (plurale, italiano con `relation_noun_it`,
   prefisso di stipulazione, `relation_value_first` insegnato), su prosa vera
   con `read:`, controllando sempre la resa.
4. Poi la Parte V: esiti epistemici e prove strutturate, quindi la vista delle
   clausole complete.

### Note pratiche per ripetere i test

Il test engine usa `obj/test-engine.sock`. Se il processo non sopravvive
alla fine della chiamata shell, avviare engine e test nella stessa chiamata:

```sh
make test-engine && ./bin/parrot0 --test tests/p0t/reasoning/living_capabilities.p0t
```

Per confrontare con il commit precedente senza toccare l'albero:
`git stash push src/…` + `make build` + `make test-engine`, oppure un
`git worktree` nello scratchpad. Ogni worktree lascia un proprio demone:
va ucciso.

`!timeout` vale per il singolo blocco `[test …]`, non per tutto il file. Il test
engine è fail-fast **dentro il blocco**: un turno che sfora nasconde gli
asserti successivi, ed è così che il rosso di `taught_lexicon` 89 sembrava
nuovo. `[mock live]` mantiene la KB completa; non svuotare profilo o mondo per
far sparire interferenze. `kb.match` via `!mcp` con argomenti `$X` ha tornato
`bindings: []` anche su fatti presenti: per le prove usare `!query`/`!query!`.

---

## Parte I — Che cos'è

### 1.1 Venti righe di motore

```prolog
apply(NomeDelPredicato, cons(Arg1, cons(Arg2, … nil)))
```

Chiama un predicato **il cui nome è arrivato come dato**. L'implementazione in
`src/kb.c` è tutta qui, e vale la pena di guardarla perché la sua piccolezza è
il punto:

```c
if (strcmp(g->pred, "apply") == 0 && g->argc == 2) {
    deep_resolve(s, g->args[0], pred, …);          /* il nome, risolto */
    if (is_var(pred) || !term_ok(pred)) return 0;   /* dev'essere legato */
    Term called; snprintf(called.pred, …, "%s", pred);
    list_to_args(list, called.args, &called.argc);  /* la lista → argomenti */
    /* … si mette `called` in testa ai goal rimasti e si risolve … */
}
```

Nessuna tabella, nessun elenco, nessuna conoscenza. Prende una stringa, la
tratta come un funtore, e rimette il goal in cima al resolvente. È **meno di un
centesimo del motore**, e ha cambiato la forma di diciotto file di KB.

### 1.2 La base teorica: secondo ordine su un motore del primo

Nella logica del primo ordine si quantifica sugli **individui**: *per ogni X…*.
Nel secondo ordine si quantifica sulle **relazioni**: *esiste una relazione R
tale che…*. Un motore Prolog-like è del primo ordine, e `apply` non lo
trasforma in uno del secondo — ma gli dà la mossa che serve quasi sempre:

> **il nome di una relazione può essere il valore di una variabile.**

È il `call/N` di Prolog, ed è la stessa idea di `funcall` in Lisp, di un
puntatore a funzione in C, del dispatch dinamico in un linguaggio a oggetti. Ma
in parrot0 pesa più che altrove, e per una ragione strutturale: **qui la KB è
insieme il programma e i dati**. Un puntatore a funzione in C punta a codice
scritto da un programmatore; `apply` in parrot0 punta a una relazione che
**qualcuno può aver insegnato parlando, dieci minuti fa**.

### 1.3 La base pratica: l'inversione della dipendenza

Questa è la parte che conta davvero, e non è logica: è architettura.

**Senza `apply`**, chi consuma deve nominare chi produce:

```prolog
% il consumatore sa che esistono law_formula e shape_formula, e li elenca
artifact_shape_for($X, formula_function) :- law_formula($X, $O, $E).
artifact_shape_for($X, formula_function) :- shape_formula($X, $O, $E).
% … e domani, per la chimica, una terza riga QUI DENTRO
```

**Con `apply`**, chi produce si dichiara e chi consuma chiede *chi*:

```prolog
model_of($P, $Thing, $Out, $Expr) :-
    model_carrier($P),
    apply($P, cons($Thing, cons($Out, cons($Expr, nil)))).
```

La freccia della dipendenza si gira. Il consumatore **non conosce nessun nome**;
il produttore ne dichiara uno. Chiamo questo schema **la porta del «chi»**, e
ricorre identico in tutta la KB:

```prolog
qualcosa_carrier(P).                       % il produttore alza la mano
tesi(…) :- qualcosa_carrier($P), apply($P, cons(…)).   % il consumatore chiede chi
```

Il test operativo del progetto — *«parrot0 può impararne un nuovo membro domani,
senza ricompilare?»* — con `apply` smette di valere solo per il **vocabolario** e
comincia a valere per le **capacità**.

---

## Parte II — Che cosa ci ha già permesso di fare

La prima ricognizione contava **59 chiamate in 18 file** di `kb/core/`
(fotografia precedente a questo incremento, non un conteggio mantenuto). Non è una curiosità di
nicchia — è diventata una delle forme portanti della KB. I casi che contano:

### 2.1 Il modello da un predicato qualsiasi — e la prova a costo zero

`kb/core/model-bridge.p0`. Il TODO prioritario di F. era: *«far scrivere a
parrot0 qualsiasi codice in qualsiasi linguaggio, prendendo il modello dalla
memoria profonda, encodato con un predicato di legge, di schema, di regola — non
lo sappiamo»*.

Il ponte non sa niente di fisica né di geometria:

```prolog
model_of($P, $Thing, $Out, $Expr) :-
    model_carrier($P), apply($P, cons($Thing, cons($Out, cons($Expr, nil)))).
```

**La prova di chiusura** è `kb/experts/geometry/formulas.p0`: nove formule vere,
più **una riga** — `model_carrier(shape_formula).` — e *«show me the python code
that computes the area of a circle»* risponde, in Python, C, Java e JavaScript,
**senza una riga aggiunta al ponte né al motore**. Un dominio nuovo costa un
fatto.

### 2.2 Un «no» guadagnato da un'esclusione

`kb/core/epistemic-status.p0`:

```prolog
closed_world_answer($Class, $Subject) :-
    class_excludes($Other, $Class), apply($Other, cons($Subject, nil)).
```

Per rispondere *no* a «un pinguino è un mammifero?» non serve conoscere tutti i
mammiferi: basta sapere che *uccello* esclude *mammifero*, e che il pinguino è un
uccello. `apply` serve perché `$Other` — la classe che esclude — **arriva come
dato**. Una sola esclusione insegnata risponde per tutti i membri presenti *e
futuri* di entrambe le classi.

È l'esempio che preferisco, perché mostra che `apply` non serve solo a
*collegare* moduli: serve a **guadagnare inferenza**.

### 2.3 Le altre, in breve

| file | che cosa diventa possibile |
|---|---|
| `conditional-plans.p0` | una condizione su una relazione **il cui nome è nella lezione** («se X contiene Y allora…») |
| `dialogue-frames.p0` | il frame di dialogo interroga la relazione che il turno ha nominato |
| `code-plans.p0`, `code-quality.p0` | la misura di qualità è un predicato dichiarato, non un elenco nel controllore |
| `document-claims.p0` | la classe di una cue è chiamata per nome |
| `discourse.p0` | il richiamo cerca la relazione in entrambe le direzioni |
| `issues.p0` | il tabellone delle lacune lavora su relazioni che non conosce |
| `composition.p0`, `situation.p0`, `procedures.p0` | composizione e piani su relazioni arrivate come dato |

**Il denominatore comune**: in ognuno di questi, prima di `apply`, ci sarebbe
stato un elenco — e ogni elenco è un posto dove domani bisogna tornare.

### 2.4 I parenti stretti già in casa

`apply` non è sola. Il motore ha già altre porte dello stesso tipo, e
insieme formano l'attrezzatura riflessiva:

| porta | che cosa dà |
|---|---|
| `call/1` | eseguire un goal costruito |
| `kb_fact(Pred, Args)` | **guardare** i fatti di un predicato arrivato come dato |
| `kb_rule(Pred, ArgsTesta)` | enumerare le **teste** delle regole; non espone il corpo |
| `kb_rule_body(Testa, PredDelCorpo)` (gen512) | sapere **da chi dipende** una regola |

`apply` *chiama*; `kb_fact`/`kb_rule` *ispezionano*. Chiamare e ispezionare sono
le due metà dell'essere vivi, e ci sono entrambe. Quello che manca è cucirle —
vedi §3.4.

---

## Parte III — L'incremento eseguibile

I file dell'incremento sono
[`inference-capabilities.p0`](../../kb/core/inference-capabilities.p0),
[`procedures.p0`](../../kb/core/procedures.p0),
[`messages.p0`](../../kb/core/messages.p0),
[`10-memory-knowledge.c`](../../src/brain/10-memory-knowledge.c) e
[`src/kb.c`](../../src/kb.c). Le prove stanno in
[`living_capabilities.p0t`](../../tests/p0t/reasoning/living_capabilities.p0t).

### 3.1 Un catalogo sopra le registrazioni esistenti

`provides/2`, proposto nella prima stesura, **ha già un altro significato**:
`provides(ocean, dissolved_oxygen)` partecipa alle inferenze sugli habitat.
Usarlo per le capacità mescolerebbe due contratti. La forma introdotta è:

```prolog
capability_registry(Capacita, RegistroUnario, AritaDelPortatore).
capability_origin(Capacita, Predicato, Arita, Registro).
capability_provider(Capacita, Predicato, Arita).
capability_call(Capacita, Predicato, Argomenti).
```

La regola centrale è piccola:

```prolog
capability_origin($Kind, $P, $Arity, $Registry) :-
    capability_registry($Kind, $Registry, $Arity),
    apply($Registry, cons($P, nil)).
```

Il catalogo **deriva** le registrazioni mentre viene interrogato. Non duplica
`model_carrier` o `relation_verb` in una tabella da sincronizzare. Ritirare una
registrazione locale o l'intera sorgente modifica subito il catalogo.

| capacità | sorgenti iniziali | contratto |
|---|---|---|
| `binary_relation` | verbi di relazione, nomi relazionali, definizioni, registrazioni esplicite | due argomenti ordinati |
| `model` | `model_carrier/1` | cosa, uscita, espressione |
| `model_prose` | `model_prose_carrier/1` | cosa, prosa |

Per nomi e definizioni due viste unarie adattano `relation_noun/2` e
`relation_def/2`. Nessun elenco di singoli predicati viene aggiunto al C.
Una nuova famiglia può dichiarare un altro registro unario a runtime.
L'arità è verificata da `capability_call/3` prima dell'invocazione; non è ancora
un sistema di tipi né una prova di purezza, terminazione o correttezza del
portatore. Registrare un predicato non garantisce che produrrà un risultato.

**Due livelli di crescita, da non confondere.** Una nuova relazione nominale
entra nel catalogo già parlando: «weft is a relation». Dalla seconda sessione
anche la lettura che la usa è chiusa: «flax is the weft of linen cloth» scrive
`weft_of(linen_cloth, flax)`, la domanda risponde, la ricerca trova «weft» e il
ritiro fa cadere sia il candidato sia lo schema del lettore (vedi l'handoff:
il difetto era una cache chiavata su una lista C). Il ciclo completo di fatto,
ricerca e ritrattazione è verificato sia con un nome sia con un verbo insegnato
parlando.
La nuova famiglia di capacità è invece verificata mediante asserzioni
strutturali. È una porta del motore aperta a runtime; manca ancora la lezione
naturale generale per dichiarare contratti e registri senza conoscerne lo schema.

### 3.2 Cercare una relazione che non era un verbo

La prima versione di `between_rel` enumerava solo `relation_verb/1`. Questo
lasciava fuori conoscenze nominate da sostantivi, pur presenti e interrogabili
con domande specifiche. Ora:

```prolog
between_rel($X, $Y, $V) :-
    capability_provider(binary_relation, $V, 2), holds($V, $X, $Y).
```

Si usa **`holds/3`**, non soltanto `apply/2`: `holds` conosce anche definizioni,
composizioni e ponti. Un predicato definito da `relation_def` può non avere
nessuna clausola propria da invocare direttamente.

La domanda esistente «what holds between … and …» resta disponibile. Le nuove
forme «what is the relation between … and …» e «che relazione c'è tra … e …»
usano il medesimo matcher di `turn_form`. `span(subject)` chiude il primo
sintagma sul connettivo dichiarato, quindi può legare **William Shakespeare**
o **calcium carbonate**, non soltanto un token. Anche il connettivo è KB.

Lo span da solo non bastava: il matcher lo marcava come testo e `op` trasformava
`calcium_carbonate` nella stringa citata `"calcium carbonate"`. Il nuovo modo
`turn_form_slot_form(Forma, Slot, atom)` conserva la chiave dell'entità.
`relation_discovery_form/1` dichiara quali forme lo usano e ne deriva la
precedenza `early`; la forma italiana usa anche la vista `said`.

La risposta nuova presenta il nome relazionale disponibile, per esempio
«currency» al posto di `currency_of`. Questo riusa `relation_noun`, lo stesso
oggetto che serve per leggere e insegnare la relazione.

**Limiti semantici espliciti:**

- La ricerca rispetta l'ordine degli argomenti. `currency_of(ghana, cedi)` non
  autorizza `currency_of(cedi, ghana)`. Inversioni e simmetrie vanno dichiarate.
- Cerca relazioni dimostrabili, incluse quelle composte già definite; non
  inventa un percorso arbitrario fra due entità.
- Cerca nel catalogo dichiarato, non nell'intero universo dei predicati.
- Zero risultati produce «non ho stabilito una relazione», mai «non esiste
  alcuna relazione». Il consumatore `op(match,…)` non espone ancora alla KB
  l'intero referto dei limiti della ricerca.
- Una lista può essere parziale: il consumatore esistente raccoglie al massimo
  64 righe e usa un buffer di resa finito. Completezza e paginazione restano
  lavoro del motore, non proprietà acquisite dal catalogo.

Questa è **scoperta relazionale per enumerazione e deduzione**. L'abduzione
vera deve proporre premesse non note che renderebbero spiegabile un risultato,
conservandole come ipotesi. Nessuna guardia sul nome libero di `apply` è stata
rimossa: prima si lega il candidato attraverso la KB, poi lo si chiama.

### 3.3 Provenienza della selezione e prova del risultato

```prolog
relation_discovery_source($X, $Y, $P, $Registry) :-
    capability_origin(binary_relation, $P, 2, $Registry),
    holds($P, $X, $Y).
```

Questa relazione dice **perché il candidato era accessibile**, richiedendo
anche che il risultato sia dimostrabile. Non finge che rinominare
`between_rel` in `relation_provenance` produca una spiegazione: non contiene
ancora il fatto sorgente, l'identità della regola, il documento o l'intero
albero di derivazione.

Un buco concreto era nel C: la risoluzione ordinaria eseguiva `apply/2`, ma il
percorso di `kb_explain` trattava dinamicamente solo `call/1`. Ora entrambi
costruiscono il goal invocato e attraversano lo stesso percorso di prova.
Le variabili legate dalla chiamata restano disponibili ai goal successivi,
anche quando la soluzione richiede backtracking. Nomi liberi e liste improprie
continuano a essere rifiutati.

La prova di regressione chiede la spiegazione di una regola con un arco
invocato tramite `apply` e un secondo arco che ne usa il risultato. Poi ritira
l'arco necessario e verifica che non venga più annunciata una dimostrazione.
Questo **non rende `kb_explain` completo rispetto a ogni builtin**: riflessione,
aritmetica, budget e identificatori di supporto richiedono ancora allineamento.

### 3.4 Le prove richieste all'incremento

Il test usa `[mock live]`, profilo AGI e KB completa. Le due parti hanno scopi
diversi:

| prova | ciò che dimostra |
|---|---|
| Ghana → cedi; William Shakespeare → Hamlet | accesso a conoscenza relazionale già presente, tramite prompt naturale |
| calcium carbonate → oxygen | la domanda esistente conserva i verbi e acquisisce soggetti composti |
| nome relazionale insegnato e ritirato | la registrazione nel catalogo cresce parlando e scompare dopo la ritrattazione |
| nome insegnato, frase multiparola, domanda, ricerca, ritiro | lo schema derivato arriva al lettore nel turno stesso e ne esce col ritiro; la conferma dice il fatto nel verso scritto |
| modo `atom` ritirato da solo | il contratto di rappresentazione dello slot è necessario: senza, i sintagmi tornano testo e la ricerca fallisce |
| portatori reali di `model` e `model_prose` | le famiglie iniziali passano dallo stesso contratto; famiglia o arità sbagliata non prova |
| verbo nuovo, fatto naturale, ricerca e ritrattazione | il fatto rimane, ma la ricerca perde il candidato quando si ritira la dichiarazione del verbo |
| nuova famiglia e nuovo registro, poi ablazione | crescita meccanica del catalogo; non certifica insegnamento naturale dei contratti |
| arità errata e argomenti invertiti | una registrazione non autorizza chiamate fuori contratto |
| nuova superficie e ritrattazione | il riconoscimento dipende dai fatti KB, non da una stringa compilata |
| prova dinamica, backtracking e ritrattazione del supporto | `apply` partecipa alla spiegazione della derivazione |
| definizione senza clausola propria | la ricerca conserva la semantica di `holds/3` |
| domanda italiana e chiamate malformate | resa italiana effettiva; nomi liberi, liste improprie e arità eccessive non producono prove |

L'esempio «coral → calcium carbonate» della prima stesura non è un risultato
acquisito: la ricognizione non ha trovato `made_of(coral, calcium_carbonate)`.
Non si aggiunge quel fatto a un test per dichiarare riuscito il collegamento.
Occorre prima acquisirlo da una fonte, con i ruoli e la provenienza corretti.

Comandi di verifica:

```sh
make test-engine
./bin/parrot0 --test tests/p0t/reasoning/living_capabilities.p0t
rg 'PARSE ERROR' obj/test-engine.log
make soft-test
```

Il documento distingue l'esistenza della primitiva, la sua integrazione e la
prova conversazionale. Una query strutturale verde non sostituisce una risposta
utile alla domanda naturale.

---

## Parte IV — Le altre ipotesi, confrontate con il motore reale

### 4.1 La strategia come dato: il nome non basta

La proposta iniziale conteneva `strategy_for(Tipo, Predicato, Priorita)` ma
non usava `$Priorita`: l'ordine sarebbe rimasto quello delle soluzioni.
Il catalogo introdotto permette di enumerare e invocare portatori; **non è
ancora uno scheduler di strategie**.

Il contratto da costruire deve includere almeno:

```prolog
strategy_for(Tipo, Predicato, Priorita).
strategy_requires(Predicato, Requisito).
strategy_attempt(Obiettivo, Predicato, Esito, Costo).
```

La KB decide pertinenza, precedenza, alternative e condizioni di arresto; il
motore ordina, alloca lavoro e restituisce l'esito effettivo. Una strategia
nuova deve poter vincere per una lezione di politica, fallire senza occultare
le alternative, ed essere rimossa senza ricompilazione. Il costo osservato
non è la fiducia epistemica nel risultato.

**Prova di arrivo:** due strategie valide per lo stesso obiettivo; invertire
la priorità insegnandolo cambia quella tentata per prima; un fallimento o un
budget esaurito attiva l'alternativa consentita dalla politica. Nessun numero
nel fatto vale come prova che quell'ordine sia già rispettato.

### 4.2 Leggere la prosa tramite IR e contratti

Il disegno resta quello di
[`ir-e-predicato-variabile.md`](ir-e-predicato-variabile.md): forma, ruoli,
legami e consumatore devono essere dati condivisi fra lettura e risposta.
Il catalogo non sostituisce `ir_bind`: invocare il predicato giusto con gli
argomenti sbagliati produce conoscenza falsa.

Occorrono ruoli obbligatori, identità dei referenti, coordinate della fonte,
negazione e contesto. Il lettore può smettere di crescere **per le strutture
che il suo linguaggio di schemi sa già esprimere**. Dire che qualsiasi nuova
lingua o costruzione diventerà automaticamente un fatto sarebbe prematuro.

**Prova di arrivo:** insegnare una forma in lingua naturale, leggere prosa
nuova con essa, interrogare il contenuto, ritirare la forma e rivedere la stessa
lettura. Esistono già revisioni in `document-claims.p0`: va estesa quella
catena, non introdotto un secondo archivio indipendente.

### 4.3 Dalle dipendenze ai residui di una dimostrazione

`kb_rule_body(Testa, PredDelCorpo)` espone **nomi di predicati**. Non mantiene
argomenti, legami fra variabili, polarità o identità della singola clausola.
Dunque:

```prolog
would_unlock($Y, $Head) :- kb_rule_body($Head, $Y).
```

significherebbe soltanto «una regola con questa testa menziona Y». Non prova
che sapere Y sbloccherebbe un obiettivo: possono mancare altri congiunti,
esserci un'eccezione, una diversa clausola o valori incompatibili.

La prossima primitiva utile è una **vista delle clausole complete**, con
variabili fresche condivise fra testa e corpo e polarità conservata:

```prolog
% Proposta, NON builtin attuale.
kb_clause(Id, Testa, Corpo).
```

Da lì un interprete KB può produrre residui **istanziati**, ad esempio
«manca la temperatura di questo campione», anziché «manca temperature/2».
L'identità della clausola deve restare distinta dal suo predicato di testa.

**Prova di arrivo:** su una regola con due premesse, una sola mancante viene
esposta con il valore già legato; insegnarla chiude lo stesso obiettivo;
ritirare una premessa riapre il residuo. Una negazione sconosciuta o una ricerca
troncata non deve diventare una domanda su un fatto dichiarato falso.

### 4.4 Azioni, verifiche e riparazioni: ciò che già c'è

`kb/core/situation.p0` contiene `action_precondition`, `action_effect`,
`applicable`, `plan_ok`, precondizioni mancanti e rivalutazione dei piani.
`code-quality.p0` usa già predicati dichiarati per le verifiche. Il ponte dei
modelli realizza codice in linguaggi dichiarati. Queste porte non sono tutte
da inventare: il lavoro è renderle accessibili con contratti compatibili.

| famiglia | incremento necessario | prova di arrivo |
|---|---|---|
| azioni | collegare piano, osservazione ed esecutore con esiti tipati | una precondizione cambia e il piano viene rivalutato prima dell'azione |
| verifiche | catalogare i checker con tipo di artefatto e significato dell'esito | un checker nuovo rileva un difetto in un artefatto preesistente |
| realizzazione | ruoli e linguaggio condivisi con chi legge | insegnare una resa cambia l'uscita e la sua rilettura conserva il significato |
| normalizzazione | trasformazioni componibili e precondizioni esplicite | un formato nuovo passa dagli stessi consumatori dopo la normalizzazione |
| riparazione | proposta, prova sul fallimento, controllo delle regressioni e promozione | il rimedio chiude il caso originale senza essere assunto vero prima della verifica |

`apply` esegue un goal: non conferisce da sola autorizzazione a un'azione
esterna né garantisce che una riparazione sia corretta. Gli effetti logici e
quelli nel mondo devono restare distinguibili nel contratto.

---

## Parte V — Che cosa manca ancora al motore per una KB viva

La soglia non è «quanti fatti contiene», ma se l'aggiunta o la ritrattazione
di conoscenza raggiunge tutte le conclusioni e decisioni che ne dipendono.
Questo è l'ordine di lavoro proposto, con criteri verificabili.

### 5.1 Risoluzione, spiegazione e revisione sullo stesso oggetto

Oggi `solve` e `prove_seq_ex` sono percorsi separati. Il supporto a `apply`
chiude un disallineamento; lascia aperta la possibilità di averne altri.
Servono **nodi di prova strutturati prodotti dalla risoluzione**, con identità
di fatti e clausole, sostituzioni, dipendenze e stato di completezza.

La resa in lingua deve leggere quei nodi; la revisione deve invalidare i
supporti effettivamente usati. Due supporti indipendenti richiedono che la
ritrattazione del primo non cancelli una conclusione ancora dimostrabile dal
secondo. Le genealogie dei documenti e le viste materializzate sono precedenti,
non ancora un truth maintenance system generale.

**Criterio:** con due prove dello stesso risultato, ritirare un supporto
mantiene il risultato e aggiorna la spiegazione; ritirarli entrambi lo rimuove.
Il ciclo comprende anche negazioni, contesti e chiamate dinamiche.

### 5.2 Tabling e ricerca equa

Esistono indici per predicato, viste materializzate, report dei costi e tagli
su cicli ground. Non equivalgono a **tabling generale**: una ricorsione con
variabili libere può ripercorrere rami, saturare il budget o dipendere molto
dall'ordine delle clausole.

Il motore dovrebbe memorizzare le varianti dei sottogoal, sospendere i
consumatori ricorsivi e riprenderli quando arrivano risposte nuove. La KB può
dichiarare politiche; un'inferenza positiva non deve cambiare significato
perché una vista viene attivata o rimossa.

**Criterio:** una chiusura ricorsiva su un grafo ciclico restituisce gli stessi
risultati cambiando l'ordine dei fatti e delle regole. Aggiunta e ritrattazione
di archi aggiornano la tabella. Per la negazione ricorsiva serve una semantica
esplicita; una sospensione non è una falsità.

### 5.3 Esiti epistemici interrogabili e conflitti

`KbInferenceReport` distingue già un budget esaurito e registra tagli di cicli.
`epistemic-status.p0` tratta classi aperte, esclusioni e chiusure. Manca la
trasmissione uniforme di questi stati fino a ogni consumatore dinamico.

Un protocollo di risultato dovrebbe distinguere almeno **dimostrato**,
**confutato con supporto**, **sconosciuto**, **incompleto** e **in conflitto**.
NAF resta fallimento finito in un ambito appropriato, non negazione universale.
Un limite di tempo non deve alimentare una conclusione negativa o un falso gap.

**Criterio:** lo stesso obiettivo, interrotto prima del termine, rimane
incompleto; con budget sufficiente può diventare dimostrato. Due fonti
contraddittorie conservano i propri supporti senza rendere dimostrabile tutto.

### 5.4 Abduzione vincolata e domande utili

Una volta disponibili clausole complete, residui e stati epistemici, la KB
può dichiarare quali premesse è lecito ipotizzare, con quali vincoli e costi.
Le ipotesi devono vivere in un contesto separato dalle asserzioni acquisite.

Non basta trovare un insieme di premesse che funziona. Occorre confrontare
alternative, rilevare incompatibilità e scegliere **quale domanda discrimina
fra esse**. `kb_dead_rules` e `frame_gap` mostrano lacune strutturali già oggi;
non sono ancora questa abduzione su un obiettivo concreto.

**Criterio:** due spiegazioni compatibili con l'osservazione producono due
ipotesi, non una certezza arbitraria. La risposta a una domanda elimina una
sola alternativa. Il risultato finale indica quali premesse sono state
osservate e quali rimangono assunte.

### 5.5 Vincoli ritardati e modi di chiamata

`dif/2` è già un vincolo ritardato. L'aritmetica e molti builtin hanno invece
modi di esecuzione più stretti: un argomento non legato può impedire una
chiamata che diventerebbe risolvibile dopo un altro goal.

Servono contratti sui modi, vincoli numerici propagabili e sospensione dei
goal non ancora pronti. Riordinare i congiunti è lecito soltanto quando sono
puri e il riordino conserva negazione, effetti e dipendenze.

**Criterio:** due ordini equivalenti di un vincolo puro danno le stesse
soluzioni; una variabile ancora ignota resta un vincolo residuo. Le azioni con
effetti non vengono riordinate come equazioni.

### 5.6 Contratti insegnabili e composizione verificata

L'arità del catalogo è un primo controllo strutturale. Mancano tipi dei ruoli,
modi ingresso/uscita, contesto, purezza, versione e requisiti dei portatori.
La KB possiede `relation_signature/3`, ma dichiarare una firma non significa
che tutti gli esecutori la verifichino.

**Criterio:** una lezione naturale introduce un portatore e il suo contratto;
un consumatore esistente lo trova e lo usa. Un portatore con arità o ruoli
incompatibili viene escluso con un residuo comprensibile, non chiamato a caso.
Un nuovo tipo deve poter essere definito in KB senza una nuova enum C.

### 5.7 Analogia strutturale senza promuovere somiglianze a fatti

`relation_def`, `norm_expr` e `same_relation` permettono già confronti di
alcune definizioni normalizzate. La prima stesura presentava erroneamente il
riconoscimento di relazioni equivalenti come interamente assente.

Mancano un trasferimento generale con corrispondenze di ruoli, controllo dei
vincoli e ricerca di controesempi. Due relazioni che coincidono sui fatti
osservati non sono per questo equivalenti in tutti i mondi.

**Criterio:** la struttura di una procedura suggerisce un'applicazione in un
altro dominio; un vincolo incompatibile la blocca. Un controesempio ritira la
corrispondenza ipotizzata senza cancellare i fatti dei due domini.

### 5.8 Contesti, tempo e revisioni atomiche

Esistono già fatti contestuali e simulazione degli effetti nei piani. Servono
però transazioni uniformi per apprendimento e revisione: nessun consumatore
dovrebbe vedere metà della sostituzione di una lettura o di una definizione.
Una conoscenza temporalmente superata non è necessariamente falsa per il suo
intervallo di validità.

**Criterio:** aggiornare una definizione e le sue dipendenze cambia insieme
risposte, catalogo e spiegazioni; il rollback ripristina il comportamento.
Interrogare un contesto ipotetico o un tempo passato non contamina quello
corrente. Le viste si invalidano sulle dipendenze, non su ogni traccia di turno.

### 5.9 Misurare la fertilità del ciclo completo

Il cricchetto finale deve attraversare tutta la catena:

```text
lezione naturale → contratto/forma → lettura con fonte → inferenza
       ↑                                                ↓
correzione e ritrattazione ← verifica ← risposta e spiegazione
```

Per ogni nuova famiglia si misurano: acquisizione parlando, uso su prosa nuova,
risposta a un caso non insegnato, spiegazione, ritrattazione, costo al crescere
della KB e capacità di riaprire lo stesso fallimento dopo una correzione.
Le prove meccaniche con simboli inventati restano necessarie, ma non sostituiscono
il collegamento fra conoscenze reali.

**Ordine operativo:** completare esiti e prove; esporre clausole e residui;
rendere incrementale e più equa la ricerca; usare residui e contratti per
abduzione, domande e riparazioni. Il catalogo costruito qui è una base comune,
non una certificazione anticipata di queste capacità.

La tesi pratica è verificabile: **una KB diventa più viva quando una lezione
cambia insieme ciò che sa fare, ciò che trova e ciò che sa spiegare; una
ritrattazione deve percorrere la stessa strada in senso inverso.** `apply` è
una delle meccaniche che lo rende possibile, insieme a unificazione,
composizione, controllo della ricerca e revisione. Non è da sola una teoria
dell'intelligenza né l'unico meccanismo possibile di dispatch dinamico.
