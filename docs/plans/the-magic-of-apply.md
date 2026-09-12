# The magic of `apply` — il predicato variabile, e perché rende viva la KB

> **F., 12 settembre 2026:** «spiega la `apply`, tutte le cose che ci ha
> permesso di fare, le sue basi teorico-pratiche, e quali possibili altre
> applicazioni potrebbe avere nell'espansione dei circuiti di inferenza e
> nell'aumento dell'accessibilità alla KB. Io sono convinto che la `apply`
> assieme ad altre tecniche possa davvero rendere la KB viva.»

Questo documento è in tre parti: **che cos'è** (e perché è piccola), **che cosa
ci ha già permesso di fare** (verificato, con i file), e **che cosa potrebbe
permetterci** (ipotesi, dichiarate come tali). Alla fine c'è la parte che mi
interessa di più: **perché la tesi di F. è giusta**, e qual è il pezzo che
ancora manca perché lo diventi davvero.

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

Verificato: **59 chiamate in 18 file** di `kb/core/`. Non è una curiosità di
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

`apply` non è sola. Il motore ha già altre tre porte dello stesso tipo, e
insieme formano l'attrezzatura riflessiva:

| porta | che cosa dà |
|---|---|
| `call/1` | eseguire un goal costruito |
| `kb_fact(Pred, Args)` | **guardare** i fatti di un predicato arrivato come dato |
| `kb_rule(Testa, Corpo)` | guardare le **regole** |
| `kb_rule_body(Testa, PredDelCorpo)` (gen512) | sapere **da chi dipende** una regola |

`apply` *chiama*; `kb_fact`/`kb_rule` *ispezionano*. Chiamare e ispezionare sono
le due metà dell'essere vivi, e ci sono entrambe. Quello che manca è cucirle —
vedi §3.4.

---

## Parte III — Che cosa potrebbe permetterci

> ⚠ Da qui in avanti sono **ipotesi**, non cose misurate. Le scrivo con la loro
> forma concreta perché una proposta senza forma non si può né provare né
> confutare — ma nessuna di queste righe è stata eseguita.

### 3.1 Leggere la prosa (il lavoro in corso)

Il lettore ha oggi un consumatore scritto apposta per ogni forma della lingua
che sa leggere. Con la IR a livelli e `apply`, una forma nuova diventa **un
fatto**:

```prolog
ir_reading(Nome, Ruolo, Predicato, Pezzi).

ir_read($Scope, $Node) :-
    ir_reading($Name, $Role, $Pred, $Parts),
    input_node_role($Scope, $Node, $Role),
    ir_bind($Scope, $Node, $Parts, $Args),
    apply($Pred, $Args).
```

Il dettaglio sta in [`ir-e-predicato-variabile.md`](ir-e-predicato-variabile.md).
Qui importa la conseguenza: **il lettore smette di crescere**. Cresce la tabella.

### 3.2 Circuiti di inferenza: la strategia come dato

Oggi *quale* ragionamento provare è scritto nell'ordine delle regole. Con
`apply`, la strategia diventa interrogabile:

```prolog
% chi sa attaccare questo tipo di obiettivo, e con quanta fiducia
strategy_for(Tipo, Predicato, Priorita).

solve_goal($G) :-
    goal_kind($G, $K), strategy_for($K, $S, $P), apply($S, cons($G, nil)).
```

Il guadagno non è la velocità: è che **parrot0 può dire perché ha provato quella
strada**, e che una strategia nuova si insegna. Si lega alla domanda che F. fa
sempre: *«perché hai risposto così?»* — una strategia che è un fatto ha un nome
da dire.

### 3.3 Abduzione sulle relazioni: la mossa che oggi è vietata

Questa è, secondo me, **la più grossa**, ed è anche la più onesta da dichiarare
perché oggi il motore la rifiuta per costruzione:

```c
if (is_var(pred) || !term_ok(pred)) return 0;   /* il nome dev'essere legato */
```

Con il nome **libero**, `apply($R, cons(a, cons(b, nil)))` diventa la domanda
**«che relazione c'è fra a e b?»** — cioè abduzione sullo spazio delle
relazioni. È il cuore di tre cose che oggi non sappiamo fare:

- *«che c'entra il corallo con il carbonato di calcio?»*
- trovare il ponte mancante fra due fatti letti in frasi diverse;
- accorgersi che **due predicati diversi dicono la stessa cosa**.

Non si può però semplicemente togliere la guardia: senza un limite, un nome
libero enumererebbe l'universo. La forma prudente è enumerare **solo ciò che è
dichiarato**:

```prolog
relation_between($A, $B, $R) :-
    relational_carrier($R), apply($R, cons($A, cons($B, nil))).
```

Cioè: **la guardia resta nel motore, e il confine lo mette la KB**. È lo stesso
schema di `model_carrier`, applicato all'inferenza invece che ai modelli. Questo
si può provare domani, e sarebbe il primo esperimento che farei.

### 3.4 Accessibilità: cucire `apply` con l'ispezione

`kb_fact`/`kb_rule`/`kb_rule_body` guardano; `apply` chiama. Cucirle dà a
parrot0 la possibilità di **usare una regola che ha trovato**, non solo di
descriverla:

```prolog
% «che cosa sai fare con X?» — non «che cosa sai di X»
can_say_about($X, $R) :-
    relational_carrier($R), apply($R, cons($X, cons($V, nil))).

% «se sapessi Y, che cosa potresti concludere?»
would_unlock($Y, $Head) :- kb_rule_body($Head, $Y).
```

La seconda è lo **spazio negativo della KB** — il tema di
`question-emergence`: non «che cosa so», ma «che cosa mi manca per concludere
qualcosa». `kb_rule_body/2` esiste dal gen512 e non ha ancora un consumatore che
lo usi per **fare domande**. Questo è il buco più evidente della casa.

### 3.5 Le altre porte, in forma breve

| idea | forma | che cosa dà |
|---|---|---|
| **azioni** | `precondition_of(A,P)`, `effect_of(A,P)` + `apply` | un piano che impara un'azione nuova come fatti |
| **verifica** | `checker_for(Tipo, P)` + `apply` | un oracolo per artefatto, dichiarato dal dominio |
| **realizzazione** | `realizer(Lingua, P)` + `apply` | una lingua nuova senza toccare il generatore |
| **normalizzazione** | `normalizer_for(Tipo, P)` | un formato nuovo come fatto |
| **auto-riparazione** | `repair_for(Sintomo, P)` | la cura di un difetto ricorrente diventa insegnabile |

---

## Parte IV — Perché la tesi di F. è giusta, e che cosa manca

> «Io sono convinto che la `apply` assieme ad altre tecniche possa davvero
> rendere la KB viva.»

Sono d'accordo, e provo a dire **in che senso preciso**, perché «viva» è una
parola che si può usare per non dire niente.

Una KB **morta** è un archivio: la si legge, e chi la legge decide che farne.
Aggiungere conoscenza aggiunge *cose sapute*. Una KB **viva** è un organo:
aggiungere conoscenza aggiunge *comportamento*, e chi la esegue non ha bisogno di
sapere che cosa è stato aggiunto.

`apply` è **la condizione necessaria** di quel passaggio, perché è la sola cosa
che permette a un fatto nuovo di farsi **chiamare** senza che nessuno l'abbia
previsto. Senza, ogni conoscenza nuova ha bisogno di un consumatore che la
nomini — e un consumatore è codice, cioè una ricompilazione, cioè l'esatto
contrario dell'esperimento.

Ma non è **sufficiente**, e le condizioni che mancano sono quattro. Le scrivo
perché sono il lavoro vero, non il documento:

1. **Un catalogo, non un'usanza.** Oggi la porta del «chi» è una *convenzione*
   ripetuta 59 volte con **un nome diverso ogni volta**: `model_carrier`,
   `model_prose_carrier`, `class_excludes`, `condition_closed_world`,
   `definition_relation`, `relation_value_first`, `directive_opener_kind`,
   `gloss_language`, `role_name`, `active_context`… Ognuna è una porta, e
   nessuna sa che le altre esistono. Perché la KB sia viva serve **un posto solo
   dove si chiede chi sa fare che cosa** — `provides(Predicato, Capacita)` —
   altrimenti aprire una porta nuova è un'invenzione e non una chiamata, e
   nessuno può enumerare che cosa parrot0 *sappia fare*.
2. **L'abduzione sulle relazioni** (§3.3). Finché il nome dev'essere legato, la
   KB risponde ma non **cerca**. Una KB che non cerca non è viva: è consultabile.
3. **La provenienza.** Una capacità chiamata dinamicamente può fare una cosa
   sbagliata in silenzio — un `model_carrier` dichiarato male produce un modello
   plausibile e falso. `model_provenance/2` è il precedente giusto: **ogni
   chiamata dinamica deve poter dire da quale fatto è nata.** È il mantra #7
   applicato alla riflessività, ed è la differenza fra una KB viva e una KB
   imprevedibile.
4. **Il costo.** Una porta del «chi» enumera i candidati. Oggi `extract_frame`
   insegna la lezione: gli schemi si scorrono **tutti a ogni turno**, e con 272
   verbi il turno era passato da 0,95 s a 1,50 s (vedi `C_TODO.md`). Un
   `provides/2` senza indice ripeterebbe l'errore su scala maggiore. **La KB viva
   ha bisogno di indici quanto di porte.**

### Il primo esperimento che farei

Piccolo, misurabile, e chiude la #2 con la #3:

```prolog
relational_carrier(made_of).  relational_carrier(located_in).  …

relation_between($A, $B, $R) :-
    relational_carrier($R), apply($R, cons($A, cons($B, nil))).

answer_frame("what is the relation between", relation_between).
relation_provenance($A, $B, $R) :- relation_between($A, $B, $R).
```

Domanda di prova: *«what is the relation between coral and calcium carbonate?»*
— oggi muro, e non perché il fatto manchi: perché **nessuno può chiedere senza
sapere già come si chiama la relazione**. Se questa quindicina di righe risponde,
la tesi di F. ha la sua prima prova sperimentale: la KB avrà **cercato** invece
di essere stata consultata.

---

### Nota di metodo

Tutto quello che sta nella Parte II è verificato e ha un file. Tutto quello che
sta nella Parte III è **ipotesi**, e l'ho scritta in forma eseguibile apposta:
in questo progetto una proposta che non si può provare è peggio di una proposta
sbagliata, perché non lascia un referto.
