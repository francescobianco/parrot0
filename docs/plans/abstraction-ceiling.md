# Abstraction Ceiling — il tetto raggiungibile con il motore attuale

> **Stato:** aggiornato gen335+ (2026-07-16), dopo l'implementazione dei primitivi "oltre il soffitto".
> 6 dei 7 primitivi sono stati realizzati in C in `src/kb.c`;
> il soffitto è stato alzato. Le nuove strutture sbloccate da ogni primitivo sono
> elencate in §2. Il tabling è rimandato (KB_MAX_DEPTH=64 è sufficiente).
> La rotta ora è: saturare lo spazio L4-L12 col nuovo motore, poi eventualmente
> aggiungere tabling se i loop ciclici diventano un problema misurabile.
>
> Definisce la **soglia massima di astrazione** che parrot0 può raggiungere
> SENZA nuovi primitivi C, e quella ORA raggiungibile con i 6 nuovi primitivi.
> Ogni struttura elencata è implementabile come conoscenza KB (fatti + regole
> Horn + `is/2` + `naf` + liste) sul motore SLD attuale. Le strutture che
> RICHIEDONO un primitivo nuovo sono elencate separatamente come "oltre il
> soffitto". L'obiettivo è saturare questo spazio e poi, solo allora, alzare il
> soffitto con un primitivo mirato.

---

## 0. Il soffitto onesto

Il motore gen335 espone: unificazione, SLD con backtracking, `is/2` (aritmetica
reale), `naf/1` (negazione-per-fallimento su goal ground), `chars/2`
(atomo↔lista-di-caratteri), confronti (`lt/le/gt/ge/eq/ne`), liste
(`cons`/`nil`), e `kb_match` per enumerazione a singolo slot.

**NON espone:** meta-call (`call/1`), ispezione di termini (`functor/3`,
`arg/3`), `assert`/`retract` da dentro le regole, `findall/3`, `bagof/3`,
taglio (`!`), unificazione di termini nested nei tool MCP, `dif/2`.

Il soffitto è: **tutto ciò che si può esprimere come clausole Horn del primo
ordine**. Non possiamo scrivere predicati che iterano su altri predicati per
nome, ma possiamo scrivere fatti che *descrivono* i predicati e query che li
leggono.

---

## 1. LIVELLI DI ASTRAZIONE (dal più basso al soffitto)

### L0 — Fatti atomici
`capital_of_country(france, paris).` — foglie. Già saturo (round 1-2).

### L1 — Operatori transitivi su relazioni specifiche
`is_a_t($X,$Z) :- is_a($X,$Y), is_a($Y,$Z).` — chiusura deduttiva.
Già saturo (12+ operatori transitivi).

### L2 — Composizione relazionale
`uncle_of($X,$Z) :- sibling_of($X,$Y), parent_of($Y,$Z).` — join su più
predicati. Già saturo (kinship, habitat, diet, temporal).

### L2.5 — Default reasoning con eccezioni
`can_fly($X) :- bird($X), naf(flightless($X)).` — non-monotonicità.
Già saturo (can_fly, gives_birth).

### L3 — Ereditarietà di proprietà
`inherits($X,$P) :- is_a_t($X,$C), has_prop($C,$P).` — le proprietà
risalgono la tassonomia. Già saturo (round 2).

### L4 — ONTOLOGIA DELLE RELAZIONI (meta-livello)
Descrivere le relazioni come oggetti di conoscenza:
```
relation_type(is_a, transitive).
relation_type(is_a, hierarchical).
relation_type(part_of, transitive).
relation_type(part_of, mereological).
relation_type(larger_than, transitive).
relation_type(larger_than, comparative).
relation_type(parent_of, asymmetric).
relation_domain(capital_of_country, geography).
relation_arity(capital_of_country, 2).
```
Il sistema sa *di cosa parlano* i suoi predicati — non solo li esegue.

### L5 — RETE SEMANTICA ESPLICITA
Modellare la KB come grafo di nodi e archi:
```
node(dog).   node(mammal).   node(animal).
edge(dog, mammal, is_a).
edge(mammal, animal, is_a).
```
Query possibili: "quali nodi raggiunge dog?", "quale arco connette X e Y?",
"esiste un cammino tra X e Y?" — rispondibili con regole Horn.

### L6 — AUTO-MODELLO STRUTTURATO
La KB contiene fatti su se stessa:
```
kb_has_predicate(is_a, 2).
kb_has_predicate(capital_of_country, 2).
predicate_domain(is_a, taxonomy).
predicate_domain(capital_of_country, geography).
predicate_domain(invented_by, history).
predicate_source(capital_of_country, curated_base).
predicate_source(invented_by, taught_session).
```
Risponde a "cosa sai di geografia?" enumerando i predicati del dominio.

### L7 — TIPOLOGIA DELLE DOMANDE
Classificare le forme di domanda e mapparle a strategie:
```
question_form("who", identity_lookup).
question_form("what is", definition_lookup).
question_form("is a", boolean_subsumption).
question_form("what are the", category_enumeration).
question_form("how many", cardinality_query).
question_form("why", causal_explanation).
question_form("what is the difference", comparative_query).
```

### L8 — ARGOMENTAZIONE E GIUSTIFICAZIONE
Premesse, conclusioni, supporto, attacco:
```
argument(premise, "socrates is a man").
argument(premise, "all men are mortal").
argument(conclusion, "socrates is mortal").
argument_supports("all men are mortal", "socrates is mortal").
argument_attacks("socrates is a god", "socrates is mortal").
```
Ragionamento sillogistico come conoscenza, non solo come esecuzione SLD.

### L9 — PROVENANCE E CONFIDENZA
Ogni fatto porta traccia di origine e affidabilità:
```
fact_source(capital_of_country(france,paris), curated).
fact_source(capital_of_country(brazil,brasilia), taught_by_user).
fact_confidence(capital_of_country(france,paris), high).
fact_confidence(capital_of_country(brazil,brasilia), medium).
```
Risposte pesate: "I'm confident that Paris is the capital of France, and I
think Brasilia is the capital of Brazil."

### L10 — SPIEGAZIONE MULTI-STEP
Catene esplicative che attraversano più regole:
```
explains(inherits(dog,warm_blooded),
         chain([is_a(dog,mammal), is_a(mammal,vertebrate),
                is_a(vertebrate,animal), has_prop(mammal,warm_blooded)])).
```

### L11 — INDUZIONE ED ESTENSIONE
Dati N esempi, inferire la regola generale. Già parzialmente coperto da
`kb.induce`, ma esprimibile anche come conoscenza:
```
induction_hypothesis(all_men_are_mortal, [man(socrates), man(plato)],
                     [mortal(socrates), mortal(plato)]).
```

### L12 — VINCOLI E CONSISTENZA
Regole che controllano la coerenza della KB:
```
contradiction(X, P, V1, V2) :- has_value(X, P, V1), has_value(X, P, V2), ne(V1, V2).
violates_constraint(X) :- contradiction(X, _, _, _).
```

---

## 1b. LIVELLI OLTRE L12 — la portata AMPLIATA (gen338+)

> I livelli L0-L12 sono logica pura: fatti, chiusure, vincoli. I livelli che
> seguono sono la **cognizione sopra la logica**: cosa una forma di superficie
> FA (atto linguistico), cosa IMPLICA (dietro le righe), come i domini si
> INCROCIANO. Ognuno resta clausole Horn + fatti — il soffitto del motore non
> si alza; si alza il livello a cui la conoscenza lavora. Ogni livello è
> tirato da un muro reale misurato (batteria di sonde gen338), mai inventato.

### L13 — PRAGMATICA: leggere dietro le righe **(saturato gen338)**
Atti linguistici indiretti e implicature come conoscenza:
```
pragma_act(indirect_request).                 % il REGISTRO (il C legge questo)
intent_cue(indirect_request, "pass me the").  % la forma di superficie
response_template(indirect_request, "I read that as a request, ...").
pragmatic_meaning(salt, "asking whether you CAN pass the salt is an indirect request ...").
answer_frame("usually mean", pragmatic_meaning).
```
`kb/core/pragmatics.p0` (incluso da social.p0). Un atto nuovo = tre fatti,
zero C: il loop in mod_chitchat cammina il registro `pragma_act/1`.
Comportamento: «can you pass me the salt?» → letta come richiesta (prima veniva
deflessa come smalltalk); «it is cold in here» → l'allusione è nominata; «what
does "…" usually mean?» → risponde dai fatti `pragmatic_meaning` (idiomi
inclusi). Ratchet: `tests/cases/pragmatics.chat` (EN+IT). Meccanica nuova
minima: un pronome DENTRO virgolette è menzione, non uso (il coref-repair non
apre più chiarimenti su «"isn't it"»).

### L14 — META-RAGIONAMENTO INTERROGABILE **(parziale, gen339)**
Le proprietà delle relazioni come fatti interrogabili in conversazione.
**Fatto (gen339, in `meta.p0` + ratchet `tests/cases/meta_reasoning.chat`):**
coppie converse `inverse_of/2` («what is the inverse of before?» → After);
tipologia L7 `question_form/2` servita in chat («what kind of question is
"why …"?»), resa raggiungibile dal **secondo passaggio sulle stopword** del
motore answer_frame (gli interrogativi SONO stopword — pass 1 li rivisita solo
se il pass 0 non trova nulla); contrasti di ragionamento `difference_between/3`
(causa/condizione abilitante, deduzione/induzione/abduzione,
correlazione/causazione, necessario/sufficiente, validità/verità, …).
I fatti `relation_type/2` (transitivo/simmetrico/asimmetrico) sono in KB ma il
loro frame chat NON risponde ancora — vedi HANDOFF.

### L15 — CATENE CROSS-DOMINIO (giro successivo)
Il ragionamento che attraversa domini: requisiti ambientali + geografia
(«why can't a fish live in the desert?»), tempo + invenzioni («which came
first…»), analogie con la relazione GIUSTA (worn_on per hand:glove::foot:?).
Muri misurati nella batteria gen338.

---

## 1c. HANDOFF (2026-07-18, fine sessione gen337-339) — il lavoro che riprende da qui

> Protocollo di ripresa: [[act-as-subagent]] per i round guidati; questa lista
> è l'ordine consigliato. Verifica sempre DIFFERENZIALE (la baseline main ha
> 73 casi `.it` rossi ereditati da gen334 — `b11373b`, canonicalizzazione
> IT→EN via `tr/2`, decisione di F. da sciogliere: o si aggiornano i test o si
> scopa la canonicalizzazione dietro un gate).

1. **L14 residuo — il frame `relation_type` non risponde.** «which relations
   are transitive/symmetric?» mura e «what is the arity of X?» mura ANCHE con
   il frame pre-esistente `answer_frame("arity", relation_arity)`: il percorso
   backward di `mod_answer_frame` (pred(?, v)) non emerge mai in chat — da
   investigare (chi intercetta il turno? il backward kb_match funziona?). I
   fatti sono già in `meta.p0`. Nota: `relation_arity/2` promesso da §1 L4 non
   esiste proprio come fatti.
2. **Contrasti IT murano.** `difference_between(correlazione, causalita, …)` è
   in KB ma «qual e la differenza tra correlazione e causalita?» non arriva al
   consumer (catena IT→canonicalizzazione da tracciare con PARROT0_TRACE).
3. **20 PARSE ERROR al boot (pre-esistenti, conoscenza droppata in silenzio!):**
   template oltre `KB_TERM_LEN`=128 in `lexicon.p0`/`responses.p0` (expl_sky,
   expl_rain, audit_report, …) e la regola `consistent_kb :-
   naf(violates_constraint(_)).` che non parsa ('bad rule'). Accorciare o
   spezzare; il fix della regola sblocca anche il punto 4.
4. **«is your knowledge consistent?» → misclaim** (smalltalk_deflect). Serve il
   consumer derivato su `consistent_kb`/`violates_constraint` (thin hook in
   40-meta + cue KB). Idem «can something be both a bird and a mammal?» su
   `incompatible/2`.
5. **L15 intero:** requires/lacks per «why can't a fish live in the desert?»,
   anni di invenzione per «which came first…», fatti `worn_on` per l'analogia
   hand:glove::foot:? (il motore analogico gen già completa qualunque relazione
   binaria: servono solo i fatti — attenzione: oggi risponde «related by
   incompatible», capire da dove viene quel link).
6. **Coda act-as-subagent §8:** residuo strstr di 60-agent-tools (chiude T16),
   T17 (80-code.c, 31 catene), T18 (25-wordmath: 71 catene, un file per round).
7. **Idea aperta (da F.):** «is your knowledge consistent?» e le meta-domande
   in genere dovrebbero poggiare su fatti L6 (auto-modello) GENERATI dalla
   struttura reale, non curati a mano — verificare cosa produce già
   `kb_has_predicate`/`predicate_domain` al boot riflessivo.

---

## 2. STRUTTURE OLTRE IL SOFFITTO (richiedono primitivi C)

### 2.1 — Primitivi REALIZZATI (gen335+)

| Struttura | Primitivo | Stato | Cosa sblocca |
|-----------|-----------|-------|-------------|
| **Meta-interprete** | `call/1` | **fatto** | Strategia di query, planning, reflection, dispatch dinamico. `call(Goal)` esegue Goal come predicato. |
| **Predicate completion** | `findall/3` | **fatto** | Raccolta di tutte le soluzioni in una lista. `findall($X, goal($X), $L)` costruisce `cons-list`. |
| **Dynamic rule generation** | `assert`/`retract` da regole | **fatto** | Il sistema crea/rimuove conoscenza come *risultato* di un'inferenza. Non-backtrackable (come Prolog). |
| **General inequality** | `dif/2` | **fatto** | Disuguaglianza posticipata fino a quando entrambi i lati sono ground. Sostituisce `naf(eq(...))`. |
| **Probabilistic reasoning** | `prob/2` | **fatto** | `prob(Goal, $P)` legge `fact_confidence(Goal, P)` dalla KB. Default 0.5. Pesi nella KB, non in C. |
| **Temporal constraint** | `ranges_over/3` | **fatto** | `ranges_over(Event, Start, End)` valida Start ≤ End come numeri. Relazioni di Allen come regole Horn su `interval/3`. |

### 2.2 — Primitivi RIMANDATI

| Struttura | Primitivo richiesto | Perché rimandato |
|-----------|--------------------|--------------------|
| **Tabling/memoization** | tabling engine | `KB_MAX_DEPTH=64` previene già i loop. Il tabling serve solo quando i grafi ciclici diventano un problema misurabile. |

### 2.3 — Vecchia tabella (pre-implementazione, per confronto)

| Struttura | Primitivo richiesto | Perché era bloccante |
|-----------|--------------------|--------------------|
| **Meta-interprete** | `call/1` o `clause/2` | Non possiamo iterare su predicati per nome. Serve per: strategia di query, planning, reflection. |
| **Predicate completion** | `findall/3` o `bagof/3` | `kb_match` enumera solo la prima variabile. Non possiamo raccogliere tutte le soluzioni in una lista dentro una regola. |
| **Constraint propagation** | `call/1` + trigger | Senza meta-call, ogni relazione transitiva richiede una regola `X_t` dedicata. Con `call/1`, una regola sola: `closure(R,X,Z) :- call(R,X,Y), closure(R,Y,Z).` |
| **Dynamic rule generation** | `assert`/`retract` da regole | Il sistema non può creare nuova conoscenza come *risultato* di un'inferenza. |
| **Term inspection** | `functor/3`, `arg/3` | Non possiamo esaminare la struttura di un termine (es. "quanti argomenti ha questo predicato?" risolto a runtime). |
| **General unification** | `dif/2` | La diseguaglianza tra atomi richiede `naf(eq(X,Y))` che è fragile (richiede groundness). `dif/2` posticipa il vincolo. |
| **Probabilistic reasoning** | `prob/2` builtin | Pesi, incertezza, ranking di ipotesi. |
| **Temporal constraint solver** | `ranges_over/3` | Vincoli temporali alla Allen generalizzati. |
| **Tabling/memoization** | tabling engine | Evitare loop in grafi ciclici senza limiti di profondità arbitrari. |

---

## 3. ROTTA

1. ~~**Saturare L4-L12** col motore attuale (round 3 e successivi).~~ **Fatto come da piano.**
   Ogni livello è un insieme di fatti + regole Horn, zero C nuovo.

2. ~~**Misurare il guadagno.** Per ogni livello saturato, verificare che il
   sistema risponda a domande che prima fallivano.~~ **Il test è `gen.respond` su domande meta-cognitive.**

3. ~~**Quando lo spazio è saturo**, scegliere UN primitivo dalla lista "oltre
   il soffitto".~~ **Fatto: 6 primitivi implementati in un colpo solo (call/1, findall/3, assert/retract da regole, dif/2, prob/2, ranges_over/3). Il tabling è rimandato.**

4. **NUOVO: Saturare lo spazio L4-L12 col motore esteso.** Con `call/1` +
   `findall/3` + `dif/2` + `assert/retract` + `prob/2` + `ranges_over/3`, lo
   spazio di ciò che è esprimibile come clausole Horn del primo ordine è
   sostanzialmente completo. Il prossimo passo è riempirlo di conoscenza KB.

5. **Se serve, aggiungere tabling** quando i grafi ciclici diventano un collo
   di bottiglia misurabile. Per ora `KB_MAX_DEPTH=64` basta.

**Soffitto alzato. Ora si riempie.**
