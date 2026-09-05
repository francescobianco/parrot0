# Inferenza compositiva — la risposta lunga è un albero di inferenza appiattito

> **Stato:** aperto il 2026-09-05 su indicazione di F.: *«il testo lungo presente
> come template di risposta sarà frammentato e le parti devono essere inferenze
> intermedie — non il concetto dei segnaposto, ma il concetto che in maniera
> costruttiva, a nested stage, a cipolla, costruisco la stringa»*.
>
> **Ruolo:** è il pezzo che manca fra [[generative-prolog]] (il motore costruisce
> *percorsi*, la lingua è l'ultimo passo) e [[generation-kb-first]] (39 modi di
> prendere la parola, uno solo che *compete*). Quei due piani chiedono che il
> ragionamento preceda il testo. Questo dice **come il testo nasce dal
> ragionamento invece che accanto ad esso**.
>
> **Perimetro:** nessun C nuovo di risoluzione. La composizione è conoscenza; il
> motore chiede un goal e stampa ciò che torna — cosa che **fa già**.

---

## 0. La misura, prima di ogni opinione

| cosa | quanti |
|---|---:|
| fatti `response_template/2,3` in KB | **1179** |
| di cui **lunghi** (riga > 120 char) | **465** |
| di cui con **≥ 2 segnaposto** | 389 |
| **lunghi con ZERO segnaposto** (prosa monolitica pura) | **176** |
| template che contengono un **connettivo di relazione** (`and`, `so`, `because`, `instead`, `quindi`, `invece`…) | **334**, di cui 264 lunghi |
| fatti `answer_content/4` (la risposta **composta**) | **87**, in **23** famiglie |
| regole `turn_response/2` | 49 |

Due letture di questa tabella, e sono entrambe il documento:

1. **La composizione esiste già e funziona**: 23 famiglie di risposta sono
   costruite pezzo per pezzo (`answer_content/4` → `answer_text/2` →
   `turn_response/2`), e il C che le stampa non sa niente di nessuna
   (`src/brain/99-registry.c:4312`, un solo `kb_match`). Non c'è niente da
   inventare: c'è da **generalizzare**.
2. **Copre il 2% della superficie.** 1179 template contro 23 famiglie composte.
   E i 264 template lunghi con un connettivo dentro sono la parte più
   interessante, per il motivo del §1.

---

## 1. Il reperto: `undetermined_cycle`, letto come albero appiattito

```prolog
response_template(undetermined_cycle, "I cannot settle that: {subject} is not
tied to {klass} by any fact I hold, and the rules around {klass} lead back into
each other, so the search closes on itself instead of reaching an answer.
Not proved is not the same as false.").
```

Sito di consumo, `src/brain/10-memory-knowledge.c:1082-1088`:

```c
kb_inference_report(b->kb, &rep);
if (!yes && (rep.loops_cut > 0 || rep.budget_hit)) → undetermined_cycle
```

### 1a. Cinque tesi, due segnaposto

La frase **afferma cinque cose distinte**, e il motore gliene passa due (`subject`,
`klass`):

| # | tesi | dimostrata? |
|---|---|---|
| 1 | «non riesco a stabilirlo» | ✅ lo stato 5 è stato raggiunto |
| 2 | «nessun fatto lega {subject} a {klass}» | ⚠️ *parziale* — la ricerca è stata **tagliata**, quindi l'esaurimento non è stato provato: è proprio ciò che il taglio impedisce di sapere |
| 3 | «le regole **intorno a {klass}** si rimandano a vicenda» | ❌ `loops_cut` è un **contatore globale della ricerca**: dice che *qualche* goal si è ripetuto *da qualche parte*, non che il ciclo riguardi `{klass}` |
| 4 | «la ricerca si chiude su se stessa» | ❌ **falsa quando è vero solo `budget_hit`**: lì la ricerca non si è chiusa su se stessa, ha finito il budget |
| 5 | «non dimostrato non vuol dire falso» | ✅ è una massima, sempre vera |

Le tesi 3 e 4 sono raggiungibili come **falsità dette con sicurezza**: basta un
`budget_hit` con `loops_cut == 0` — una catena profonda e *acidica* (nessuna
ripetizione di goal) che sfonda `KB_MAX_DEPTH`/`KB_MAX_GOALS`/il budget di passi
(`src/kb.c:1635,1638,1954,1979,2288`).

E non è un caso di laboratorio: con la KB che cresce, `budget_hit` si avvicina da
solo (vedi la memoria *«KB growth degrades the engine»*, gen382: lookup O(n)).
Il giorno in cui una domanda polare ordinaria sfora il budget su una KB
perfettamente aciclica, parrot0 risponde **che le sue regole si rimandano a
vicenda**. È esattamente la specie di falsità che la dottrina no-deception mette
sopra ogni muro onesto.

> ⚠️ Questa è una **previsione dalla lettura del codice**, non un comportamento
> osservato: non ho eseguito nulla. Il §8 la trasforma in un gate rosso, che è
> il modo giusto di scoprire se ho ragione.

### 1b. La cosa che il template promette e il motore non ha

«*the rules around {klass} lead back into each other*» — quali regole?
`KbInferenceReport` (`src/kb.h:406-421`) porta `steps`, `budget_hit`,
`loops_cut`, `goal`: **contatori, non il percorso**. Il ciclo `blim ⇄ zorp` è la
cosa più informativa scoperta nel turno, ed è l'unica che non esce.

È lo stesso identico buco che [[generative-prolog]] chiama P2 (tuple di binding:
senza percorso non c'è passeggiata sul grafo) e che
[[question-emergence]] §4d chiama spazio negativo (regole che si rimandano e non
deducono niente). **Tre piani, un solo oggetto mancante: il percorso.**

### 1c. Il centro del documento: i connettivi sono cicatrici

Nel template ci sono `:` , `, and` , `, so` , `.` — e non sono punteggiatura.
Sono **relazioni retoriche** fra tesi:

```
tesi 1  ──frames──▶  tesi 2  ──elaborates──▶  tesi 3  ──concludes──▶  tesi 4
                                                          │
                                                     qualifies
                                                          ▼
                                                       tesi 5
```

Quell'albero **è esistito** — nella testa di chi ha scritto la frase — ed è stato
appiattito in una stringa. I connettivi sono ciò che resta della struttura
perduta: le cicatrici.

E c'è la prova che le tesi sono oggetti reali e non pezzi di stringa: la tesi 5
compare **identica** in due template diversi —

```prolog
undetermined_cycle    → "… Not proved is not the same as false."
no_support_either_way → "… so not proved is not the same as false. …"
```

— per due volte in inglese e due in italiano: **quattro copie di un pensiero
solo**. Un frammento che ricorre in template diversi non è un frammento: è uno
**stadio** a cui nessuno ha ancora dato un nome.

K6 di [[frontier-kb-natural-dialogue]] aveva già nominato quelle relazioni
(`answer_relation(P, Q, elaborates | contrasts | qualifies | justifies)`) e
previsto che «*i template diventano micro-frame riusabili per relazioni
retoriche, non contenitori di risposte*». Questo documento è quel programma preso
sul serio su un caso vero.

---

## 2. Che cos'è l'inferenza compositiva (e che cosa NON è)

### 2a. L'inversione

|  | **segnaposto** | **stadio** |
|---|---|---|
| che cosa è fisso | la **forma** (la frase esiste prima dei valori) | il **contenuto** (esiste perché dimostrato) |
| che cosa varia | il contenuto dei buchi | la forma: quanti strati ci sono |
| chi ha scritto la struttura | un umano, una volta | la risoluzione, adesso |
| se manca il pezzo | resta un buco, o una bugia | lo strato **non c'è**: la frase è più corta |
| annidamento | impossibile (un valore è terminale) | è la regola: lo strato avvolge una composizione |
| interrogabile | no: «perché hai detto quella frase?» → «era il template» | sì: ogni strato ha la sua prova |

**La regola in una riga:** *un segnaposto è un buco che aspetta un valore; uno
stadio è un'inferenza che, se riesce, avvolge dentro di sé ciò che ha trovato — e
se non riesce, non esiste.*

### 2b. La cipolla: un filo solo, molte prove

Ogni stadio riceve **una sola cosa** dall'interno: il **testo già composto**.
Tutto il resto se lo dimostra da sé. Questo vincolo è ciò che tiene la
composizione un'inferenza invece di un albero di sostituzioni:

- non c'è un contesto globale di slot che passa di mano in mano;
- ogni strato è **indipendentemente vero o falso**, e lo si può interrogare da
  solo (è la proprietà che [[generative-leverage]] aveva già visto nei
  `story_atom`: *«ogni atomo ha senso compiuto indipendente»*);
- l'ordine non è un intero scritto a mano: è la **relazione** fra strato e
  interno, e da lì si legge il connettivo.

### 2c. I segnaposto non spariscono: smettono di essere arbitrari

Onestà: uno strato deve pur nominare le entità («*no fact I hold decides whether
**vex** is a **blim***»). Il criterio che li rende legittimi è netto:

> **Un buco è lecito se e solo se nomina un argomento della tesi che quello
> stadio ha dimostrato.** Un buco che nomina qualcosa che lo stadio non ha
> provato è un template travestito.

Oggi `undetermined_cycle` viola questo criterio due volte: `{klass}` compare
nella tesi 3 (il ciclo) senza che nessuno abbia provato che il ciclo riguardi
`klass`.

### 2d. Che cosa si guadagna, concretamente

1. **Ogni proposizione della risposta ha una prova.** È l'oracolo che
   [[generative]] (critica gen206) dichiara mancante per la prosa: *«il
   generatore propone, l'oracolo dispone»*. Per il codice l'oracolo è esterno
   (`code_eval`); per la prosa **l'oracolo è la dimostrazione di ogni strato**.
   Senza composizione, la prosa non è falsificabile — con essa sì, riga per riga.
2. **Uno strato scritto per una risposta serve tutte le risposte** che
   dimostrano la stessa tesi (§5: tre combinazioni che nessuno ha scritto
   compaiono da sole).
3. **Si insegna e si ritira uno strato alla volta.** Oggi «*rispondi più corto*»
   richiede di insegnare un template intero; domani è il ritiro di un fatto.
   Test del mantra — *può impararne un nuovo membro domani, senza ricompilare?*
   — **sì**: uno strato nuovo è un fatto.

---

## 3. Ciò che esiste già, e la delta esatta

Esiste (`kb/core/procedures.p0:262-265`), ed è già il fold:

```prolog
answer_has_content($A, $P) :- answer_content($A, $P, $Role, $Text).
answer_tail($A, $P, $Piece) :- answer_content($A, $P, $R, $Piece), is($N, add($P,1)), naf(answer_has_content($A, $N)).
answer_tail($A, $P, $Text)  :- answer_content($A, $P, $R, $Piece), is($N, add($P,1)), answer_has_content($A, $N), answer_tail($A, $N, $Rest), concat_atoms($Piece, $Rest, $Text).
answer_text($A, $Text) :- answer_tail($A, 0, $Text).
```

Con 23 famiglie vive (`risk.p0`, `situation.p0`, `honest-limits.p0`,
`discourse.p0`, `code-plans.p0`, `inquiry.p0`, `state-description.p0`).

**Quattro cose che questo strato non sa fare**, ed è tutta la delta:

| | oggi | inferenza compositiva |
|---|---|---|
| ordine | intero scritto a mano (`0,1,2,…`) | relazione dichiarata (`elaborates`, `concludes`, `frames`) |
| forma della risposta | **arietà fissa**: `explain($Action,$Effect,$Change,$Lang)` decide *prima della prova* quanti pezzi ci saranno | l'insieme degli strati che reggono è **il risultato** della prova |
| esistenza di un pezzo | tutti i pezzi devono provarsi, o `answer_text` fallisce e il turno **cade in silenzio** al percorso storico | uno strato che non regge **sparisce**, e gli altri restano |
| annidamento | un pezzo è una stringa; non può essere un'altra risposta condizionale | uno strato avvolge una composizione, ricorsivamente |

Il punto due è il più grave: `explain/4` è una **forma decisa prima del
ragionamento** — cioè un template, scritto in Prolog invece che fra virgolette.

---

## 4. La KB che mi aspetto

File proposto: `kb/core/composition.p0`. Tre parti: il motore (nessuna parola), i
sensori riflessivi (dal C), il caso di studio.

### 4a. Il motore — sei clausole, zero vocabolario

```prolog
:- file_attribute(machinery).

% Uno stadio esiste se la sua tesi e' dimostrata. Uno stadio senza tesi e'
% incondizionato: il nucleo di solito lo e'.
stage_has_claim($S) :- stage_claim($S, $C).
stage_holds($S) :- stage_claim($S, $C), call($C).
stage_holds($S) :- naf(stage_has_claim($S)).

% (1) IL NUCLEO — nessuno strato piu' interno.
composed($S, $Lang, $Text) :-
    naf(stage_wraps($S, $Any)), stage_holds($S), stage_text($S, $Lang, $Text).

% (2) LO STRATO CHE REGGE — avvolge il testo interno secondo la RELAZIONE che
%     ha con esso. Il connettivo non e' scritto nello strato: e' la resa della
%     relazione nella lingua del turno.
composed($S, $Lang, $Text) :-
    stage_wraps($S, $Inner), composed($Inner, $Lang, $In),
    stage_holds($S), stage_around($S, $Lang, $In, $Text).

% (3) LO STRATO CHE NON REGGE NON INTERROMPE: sparisce, e la risposta e' piu'
%     corta. E' l'intero punto del documento.
composed($S, $Lang, $Text) :-
    stage_wraps($S, $Inner), naf(stage_holds($S)), composed($Inner, $Lang, $Text).

% Avvolgere: dopo (elaborazione, conseguenza) o prima (cornice, apertura).
stage_around($S, $L, $In, $T) :-
    stage_side($S, after), stage_relation($S, $R), relation_connective($R, $L, $C),
    stage_text($S, $L, $Own), concat_atoms($In, $C, $A), concat_atoms($A, $Own, $T).
stage_around($S, $L, $In, $T) :-
    stage_side($S, before), stage_relation($S, $R), relation_connective($R, $L, $C),
    stage_text($S, $L, $Own), concat_atoms($Own, $C, $A), concat_atoms($A, $In, $T).
```

I connettivi, che sono conoscenza di lingua e non punteggiatura (i nomi delle
relazioni sono quelli già previsti da K6):

```prolog
relation_connective(frames,     en, ": ").      relation_connective(frames,     it, ": ").
relation_connective(elaborates, en, ", and ").  relation_connective(elaborates, it, ", e ").
relation_connective(concludes,  en, ", so ").   relation_connective(concludes,  it, ", quindi ").
relation_connective(qualifies,  en, ". ").      relation_connective(qualifies,  it, ". ").
relation_connective(offers,     en, " ").       relation_connective(offers,     it, " ").
```

### 4b. I sensori riflessivi — la sola delta in C

Il C non impara a parlare: **deposita in KB ciò che ha visto**, esattamente come
già fa per il muro (`machinery_gap`) e per la lettura troncata
(`saturated_read/3`, committata al confine del turno da `kb_saturation_commit`,
`src/brain/99-registry.c:2921`). Stessa figura, terza istanza:

```prolog
machinery(turn_goal).             % turn_goal(current_turn, Soggetto, Classe)
machinery(inference_cycle).       % inference_cycle(current_turn, cons(P1, cons(P2, nil)))
machinery(inference_incomplete).  % inference_incomplete(current_turn, budget | cycle)
```

`inference_cycle/2` è **il pezzo che oggi non esiste**: chiede che
`KbInferenceReport` porti i predicati del ciclo tagliato e non solo il conteggio
(`src/kb.c:2254`, dove il taglio avviene e l'informazione viene buttata). È lo
stesso percorso che [[generative-prolog]] P2/P3 chiede per camminare sul grafo:
**una volta reso, serve due piani.**

### 4c. Il caso di studio — `undetermined_cycle` sfogliato

```prolog
% ── NUCLEO: la proposizione sotto esame. E' gia' esso stesso una composizione,
%    e usa la copula che `code-plans.p0` interroga gia' (`state_copula/2`) e
%    l'articolo indeterminativo che il gen504 ha reso conoscenza.
stage_text(goal_proposition, $L, $T) :-
    turn_goal(current_turn, $S, $K), state_copula($L, $Cop), class_phrase($K, $L, $Cls),
    concat_atoms($S, $Cop, $A), concat_atoms($A, $Cls, $T).

% ── 1. NESSUN FATTO ANCORA IL SOGGETTO ALLA CLASSE (cornice, prima)
stage_wraps(no_anchor, goal_proposition).
stage_claim(no_anchor, goal_unanchored(current_turn)).
stage_relation(no_anchor, frames).   stage_side(no_anchor, before).
stage_text(no_anchor, en, "no fact I hold decides whether").
stage_text(no_anchor, it, "nessun fatto che ho decide se").
goal_unanchored($T) :- turn_goal($T, $S, $K), naf(kb_fact($K, cons($S, nil))).

% ── 2. IL CICLO, CON I SUOI MEMBRI (elaborazione, dopo)
%    Esiste solo se un ciclo e' stato davvero tagliato, e NOMINA le classi
%    invece di alludervi: e' cio' che il template promette e non mantiene.
stage_wraps(cycle_named, no_anchor).
stage_claim(cycle_named, inference_cycle(current_turn, $Loop)).
stage_relation(cycle_named, elaborates).  stage_side(cycle_named, after).
%    Nessun segnaposto: i membri del ciclo sono un ELENCO, e l'elenco si
%    realizza con il fold che esiste gia' (`list_text/3`, discourse.p0:52).
stage_text(cycle_named, $L, $T) :-
    inference_cycle(current_turn, $Loop), list_text($L, $Loop, $Names),
    cycle_prefix($L, $P), cycle_suffix($L, $Sx),
    concat_atoms($P, $Names, $A), concat_atoms($A, $Sx, $T).
cycle_prefix(en, "the rules for ").  cycle_suffix(en, " lead back into each other").
cycle_prefix(it, "le regole di ").   cycle_suffix(it, " si rimandano a vicenda").

% ── 3. LA CONSEGUENZA (conclusione, dopo) — dipende dal ciclo, non dal budget
stage_wraps(search_closed, cycle_named).
stage_claim(search_closed, inference_incomplete(current_turn, cycle)).
stage_relation(search_closed, concludes).  stage_side(search_closed, after).
stage_text(search_closed, en, "the search closes on itself instead of reaching an answer").
stage_text(search_closed, it, "la ricerca si chiude su se stessa invece di arrivare a una risposta").

% ── 3bis. L'ALTRA CAUSA DI ARRESTO, che oggi indossa le parole della prima
stage_wraps(budget_spent, search_closed).
stage_claim(budget_spent, inference_incomplete(current_turn, budget)).
stage_relation(budget_spent, concludes).  stage_side(budget_spent, after).
stage_text(budget_spent, en, "the search ran past its budget before finishing").
stage_text(budget_spent, it, "la ricerca ha superato il suo budget prima di finire").

% ── 4. IL VERDETTO EPISTEMICO (cornice, prima)
stage_wraps(verdict, budget_spent).
stage_claim(verdict, answer_status(current_turn, undetermined)).
stage_relation(verdict, frames).  stage_side(verdict, before).
stage_text(verdict, en, "I cannot settle that").
stage_text(verdict, it, "Non posso stabilirlo").

% ── 5. LA MASSIMA (qualificazione, dopo) — UNA volta per lingua, non quattro
stage_wraps(maxim, verdict).
stage_claim(maxim, unearned_negation_risk(current_turn)).
stage_relation(maxim, qualifies).  stage_side(maxim, after).
stage_text(maxim, en, "Not proved is not the same as false.").
stage_text(maxim, it, "Non dimostrato non vuol dire falso.").
unearned_negation_risk($T) :- turn_goal($T, $S, $K), naf(closed_world_answer($K, $S)).

% ── 6. L'OFFERTA (dopo) — oggi vive SOLO in no_support_either_way
stage_wraps(offer, maxim).
stage_claim(offer, gap_is_tellable(current_turn)).
stage_relation(offer, offers).  stage_side(offer, after).
stage_text(offer, en, "Tell me either way and I will hold it.").
stage_text(offer, it, "Dimmelo in un verso o nell'altro e lo terro'.").

% ── L'AGGANCIO: una sola regola, la stessa forma delle altre 49.
turn_response($T, $Text) :-
    turn_goal($T, $S, $K), naf(polar_settled($T)),
    current_language($L), composed(offer, $L, $Text).
```

### 4d. Che cosa rende, nei tre stati raggiungibili

**A — ciclo tagliato** (`inference_cycle` regge):

> I cannot settle that: no fact I hold decides whether vex is a blim, **and the
> rules for blim and zorp lead back into each other**, so the search closes on
> itself instead of reaching an answer. Not proved is not the same as false.
> Tell me either way and I will hold it.

**B — budget esaurito, nessun ciclo** (`cycle_named` e `search_closed` non
reggono: **spariscono**; entra `budget_spent`):

> I cannot settle that: no fact I hold decides whether vex is a blim, **so the
> search ran past its budget before finishing**. Not proved is not the same as
> false. Tell me either way and I will hold it.

**C — nessun sostegno, ricerca completa** (nessuno strato di arresto regge):

> I cannot settle that: no fact I hold decides whether magnetite is an iron oxide
> mineral. Not proved is not the same as false. Tell me either way and I will
> hold it.

**E lo stato C è `no_support_either_way`.** Cioè: i due template non erano due
risposte. Erano **una cipolla osservata in due stati**, e nessuno poteva
accorgersene finché la struttura restava appiattita in due stringhe.


### 4e. Legenda — che cosa esiste e che cosa è da scrivere

Perché nessuno debba fidarsi della sintassi qui sopra:

| esiste, verificato | dove |
|---|---|
| `concat_atoms/3`, `naf/1`, `call/1`, `kb_fact/2`, `findall/3`, `is/2` | builtin del solver, `src/kb.c:1832,6045` |
| `list_text/3` (fold di un elenco con separatore di lingua) | `kb/core/discourse.p0:52` |
| `answer_content/4` → `answer_tail/3` → `answer_text/2` | `kb/core/procedures.p0:262-265` |
| `turn_response/2` chiesto dal C con un solo `kb_match` | `src/brain/99-registry.c:4312` |
| `state_copula/2`, `list_separator/2`, `sentence_terminator/2`, `current_language/1`, `proposition_label/3` | convenzioni vive in `code-plans.p0`, `situation.p0`, `inquiry.p0` |
| `closed_world_answer/2` (il «no» guadagnato) | `kb/core/epistemic-status.p0` |
| l'articolo indeterminativo come conoscenza | `p0_indef_article`, gen504 |

| **da scrivere** | natura |
|---|---|
| `composed/3`, `stage_holds/1`, `stage_around/4` | il motore del §4a — **KB, non C** |
| `stage_wraps/2`, `stage_claim/2`, `stage_relation/2`, `stage_side/2`, `stage_text/3` | il vocabolario della cipolla |
| `relation_connective/3` | resa dei connettivi per lingua |
| `class_phrase/2,3`, `cycle_prefix/2`, `cycle_suffix/2` | superfici, banali |
| `answer_status/2`, `polar_settled/1`, `gap_is_tellable/1`, `goal_unanchored/1`, `unearned_negation_risk/1` | tesi degli stadi: **viste**, non fatti |
| `turn_goal/3`, `inference_cycle/2`, `inference_incomplete/2` | sensori riflessivi — **l'unica delta in C** (§4b) |

---

## 5. Il guadagno, contato

Materiale in ingresso: 2 template × 2 lingue = **4 stringhe monolitiche**
(~940 caratteri), 2 stati coperti.

Materiale in uscita: 7 stadi, 3 dei quali condizionati ⇒ fino a **8 risposte
distinte**, di cui almeno **3 che nessuno ha scritto**:

1. `undetermined` **+ offerta di rimedio** — oggi l'offerta esiste solo nel
   fratello: un turno indeterminato non chiede mai aiuto, e potrebbe;
2. `budget senza ciclo` — oggi indossa le parole del ciclo (§1a, tesi 4);
3. `ciclo con i membri nominati` — oggi impossibile (§1b).

Più la deduplicazione: la massima passa da **4 copie a 2** (una per lingua), e
diventa disponibile a *qualunque* risposta dimostri
`unearned_negation_risk/1` — per esempio i piani condizionali di
`conditional-plans.p0`, che la stessa dottrina la enunciano per conto loro.

### 5bis. L'insieme degli strati è APERTO, e questo cambia il conto

> Correzione di F., 2026-09-05: *«la cipolla non sarà mai un template più
> costoso, per il motivo che la complessità linguistica è quasi infinita: una
> cipolla ha per definizione infiniti altri inferitori, magari oggi non presenti
> in KB, ma potenzialmente arriveranno»*.

Ha ragione, e la mia prima versione del §7 misurava l'asse sbagliato. Il conto
del §5 confronta due strutture **a coperture pari**, ma le due non si estendono
allo stesso modo:

| | **template** | **cipolla** |
|---|---|---|
| che cos'è, strutturalmente | una stringa **chiusa** | un termine **aperto**: qualunque cosa può avvolgerlo |
| aggiungere una qualificazione | si riscrive la stringa, in **ogni** lingua e in **ogni** variante che la conteneva | un fatto: `stage_wraps($Nuovo, $Esistente)` |
| costo dell'estensione | proporzionale a **ciò che c'è già** | proporzionale a **ciò che si aggiunge** |
| chi può estenderla domani | chi può ricompilare la frase | chiunque, parlando |

Un template è una frase finita in una lingua che non lo è. Ogni stadio è un
**punto di attacco libero**: un inferitore che oggi non esiste — una cautela, una
provenienza, una stima di confidenza, un registro, una ripresa di ciò che si è
detto tre turni fa — potrà avvolgerlo domani **senza toccarlo**. La stringa non
offre punti di attacco: offre solo se stessa.

Questo è anche il motivo per cui il mio criterio di riuso era in contraddizione
con il mantra del progetto: contare i consumatori **oggi** è misurare una
struttura KB-first con una metrica a mondo chiuso. Il test è
*«parrot0 può impararne un nuovo membro domani, senza ricompilare?»* — la cipolla
lo passa, il template no. Uno stadio con un consumatore solo non è uno spreco: è
capienza non ancora usata.

**La conseguenza, però, è un requisito nuovo, non uno sconto.** Se gli strati
candidati sono un insieme aperto, prima o poi due vorranno la stessa posizione
con la stessa relazione — e a quel punto **la scelta non può essere l'ordine
delle clausole nel file**, che è precisamente il difetto che
[[generation-kb-first]] misura (39 rivendicazioni posizionali, una sola che
compete). Gli strati devono **gareggiare su evidenza dichiarata**
(`kb_hypothesis_best`), come già fa `creative_response`: candidati dichiarati,
vincitore unico, prova ispezionabile. Un insieme aperto senza arbitrato non è
ricchezza, è la stessa politica-per-ordine-degli-`if` spostata dal C alla KB.

### 5ter. Una condizione mai alternata è capienza, non decorazione

> Seconda correzione di F., 2026-09-05: *«anche la condizionalità ha un effetto
> potenziale. Una condizione identificata storicamente ma mai alternata è un
> fatto del mondo che prima o poi servirà: è meglio averlo che non averlo, e
> segna elasticità mentale»*.

Regge, e per tre ragioni distinte — la terza è la più forte.

**1. La prova è nel gen504.** Lo stato «nessun sostegno in nessun verso» esisteva
logicamente da sempre e non era mai stato *distinto*: finché una classe non
esisteva, la risposta era «non so»; appena esisteva **un** membro positivo, ogni
altro diventava «No.». La condizione non si era mai alternata — fino al giorno in
cui F. ha insegnato i minerali di ferro **un membro alla volta**, che è il modo
KB-first di crescere. Allora si è alternata, e l'assenza della distinzione non
era neutra: **produceva una falsità** (`kb/core/epistemic-status.p0`, intestazione).
Il costo di non averla tracciata prima è stato una bugia; il costo di averla
tracciata prima sarebbe stato zero.

**2. Nominare la condizione cambia l'asserzione anche se non si alterna mai.**
Uno strato con tesi `C` dice «affermo questo *perché* `C`». Uno strato
incondizionato dice «affermo questo». Le due frasi possono coincidere per anni e
non sono la stessa cosa: la prima è vera **e sa perché**, la seconda è vera **per
caso**. È esattamente la distinzione che `undetermined_cycle` esiste per fare —
e vale per uno stato che nel traffico normale non si presenta quasi mai.

**3. Elasticità = capacità non ancora consumata.** Un sistema in cui ogni
condizione è sempre vera è un sistema che ha aderito esattamente alla propria
storia: non ha margine per essere sorpreso. Le condizioni non esercitate sono i
gradi di libertà che gli permettono di rispondere a qualcosa di nuovo **senza
essere riscritto** — cioè il contrario di ciò che il progetto chiama «una classe
popolata dai sintomi» ([[generation-kb-first]] §3). Il mio falsificatore era un
**criterio di potatura**, e cozzava con una regola già stabilita: *tenere le
strutture ridondanti e secondarie, evolvere per selezione, nessuna potatura
prematura*. Avrei dovuto verificarlo prima di scriverlo.

**La conseguenza, di nuovo un requisito e non uno sconto.** Se le condizioni
inutilizzate vanno tenute, il loro **costo di valutazione** non può crescere con
il loro numero: la famiglia composta è chiesta a ogni turno e `turn_response`
costa ~20× un passo di `segment_role` (`src/kb.c:85`). Una tesi va provata **una
volta per turno e depositata** come fatto riflessivo, non riprovata da ogni
strato che la nomina. Tenere tutto è giusto; ripagarlo a ogni strato no.

---

## 6. Vincoli meccanici, misurati

| vincolo | valore | conseguenza |
|---|---|---|
| `KB_TERM_LEN` | **512** (`src/kb.h:21`) | il testo composto **e ogni suo intermedio** stanno sotto 512 char. Lo stato A misura ~265: c'è margine, non abbondanza. |
| `concat_atoms/3` in overflow | **fallisce** (`src/kb.c:1832-1849`, `snprintf >= sizeof` → `return 0`) | una cipolla troppo lunga non tronca: **perde tutta la risposta in silenzio** e il turno cade al percorso storico. È la stessa specie di guasto che ha *causato* `concat_atoms` al gen395 (il fold su liste di caratteri sfondava a ~58 char). **Va reso un fatto**, come `saturated_read/3`: `composition_truncated/2`, non un fallimento muto. |
| `KB_MAX_DEPTH` 64, `loops_cut` | `src/kb.c:40,2254` | una cipolla insegnata male (`stage_wraps` ciclico) finisce nella guardia anti-isteresi: la composizione non termina in loop, si taglia. Il caso limite si racconta da sé — parrot0 non riesce a stabilire come rispondere, e lo dice con lo stesso strato. |
| costo di `turn_response` | ~20× un passo di `segment_role` (`src/kb.c:85`) | la famiglia composta è chiesta **a ogni turno**: ogni strato nuovo costa budget. Da misurare con `PARROT0_TE_SLOW`, non da stimare. |

---

## 7. Che cosa renderebbe falsa questa tesi

Il criterio va fissato **adesso**, prima di avere ragione per costruzione:

> ⚠️ **Due criteri ritirati da F.**, e sbagliavano allo stesso modo: misuravano
> **a mondo chiuso** una struttura che esiste per restare aperta.
>
> - *«meno del 30% degli stadi con due consumatori ⇒ è solo un template più
>   costoso»* — contava i consumatori a un istante (§5bis);
> - *«se gli strati condizionati reggono sempre, il guadagno è cosmetico»* —
>   contava le alternanze in una finestra di storia (§5ter).
>
> In entrambi i casi il criterio chiedeva a una capacità di **giustificarsi con
> l'uso già fatto**. Nessun criterio in questo progetto può assumere che la KB
> sia finita. Ciò che resta è falsificabile senza quell'assunzione: l'arbitrato,
> l'ancoraggio, la resa, il costo, il bilancio del C.

1. **Arbitrato.** Se, cresciuti gli strati, la scelta fra due candidati alla
   stessa posizione finisce per essere l'ordine delle clausole nel file, la
   cipolla ha importato in KB il difetto che [[generation-kb-first]] misura nel C,
   e la tesi è falsa nel modo peggiore: sembra KB-first e non lo è.
2. **Condizione non ancorata.** *Non* «la condizione non si è mai alternata»:
   quella è capienza (§5ter). Il difetto è la tesi che **non potrebbe alternarsi
   comunque vada** — un claim che nessun sensore e nessuna vista su fatti
   mutevoli può rendere falso. Quella non è elasticità, è decorazione: uno strato
   incondizionato travestito da condizionato. È un controllo **strutturale**,
   eseguibile oggi, non un'attesa: ogni `stage_claim/2` deve risalire a un
   `machinery/1` riflessivo o a una vista su fatti che la conversazione può
   cambiare.
3. **Resa delle combinazioni.** Tre strati condizionati sono 8 rese; dieci
   sono 1024, e **nessun umano le legge**. La prova di ogni strato garantisce che
   nessuna resa affermi qualcosa di non dimostrato — la *verità* è coperta — ma
   la **fluidità** non ha oracolo. Se le combinazioni non lette producono
   periodi sgrammaticati o qualificazioni che si accavallano, serve un gate sulle
   rese, non fiducia. È il residuo onesto: la composizione rende la prosa
   falsificabile nel contenuto, non nella forma.
4. **Costo.** Se la composizione porta il turno oltre il budget su una KB di
   dimensione corrente, va fermata: una risposta giusta che non arriva è peggio
   di una imprecisa che arriva.
5. **Ammortamento del C** (mantra #18). Il C aggiunto qui è solo il deposito di
   `inference_cycle/2`; se per far reggere la cipolla servisse un lettore in C
   per stadio, siamo tornati ai moduli e la direzione è sbagliata.

---

## 8. Il primo gate rosso (si comincia da qui, non dal refactor)

Non si riscrive niente finché un gate non è rosso. Il gate è la previsione del
§1a:

```
tests/p0t/reasoning/incomplete_not_cyclic.p0t
```

- **stimolo:** conoscenza **aciclica** abbastanza profonda/ramificata da mettere
  `budget_hit` con `loops_cut == 0`. La forma esatta va **trovata**, non
  supposta: si costruisce con `/debug` e la si verifica prima di scrivere
  l'assert (regola U3 di `C_TODO.md`: se per capirlo serve un esperimento a
  mano, quell'esperimento appartiene a `/debug`).
- **assert:** `<! lead back into each other` (parrot0 non deve attribuire un
  ciclo che non c'è) e `<~ budget` (deve dire perché si è fermato davvero).
- **oggi, per come leggo il C, questo gate è rosso.**

Poi, in ordine di dipendenza — un passo per generazione:

| passo | cosa | gate |
|---|---|---|
| **C1** | `inference_cycle/2` riflessivo: il ciclo tagliato entra in KB con i suoi membri | il gate sopra passa; `/debug` mostra il ciclo |
| **C2** | il motore `composed/3` in `kb/core/composition.p0` + `undetermined_cycle` sfogliato negli stadi del §4c | i 3 stati del §4d, tre assert; `inference_guard.p0t` resta verde parola per parola |
| **C3** | `no_support_either_way` **eliminato** come template e ottenuto dallo stato C | il gate del gen504 resta verde senza il suo template |
| **C4** | `composition_truncated/2` (la saturazione della cipolla è un fatto, non un silenzio) | una cipolla oltre 512 char risponde corto e **dice** che ha tagliato |
| **C5** *(solo su pressione)* | seconda famiglia sfogliata, scelta fra i 264 template lunghi con connettivo | due strati candidati alla stessa posizione **gareggiano** e la prova dice perché uno ha vinto (§7.1); nessuna resa combinatoria sgrammaticata (§7.3) |

---

## 9. Collegamenti

- [[generative-prolog]] — la visione «percorso logico → realizzazione
  linguistica». Questo documento ne è la **realizzazione dal basso**: gli strati
  sono clausole, quindi la pipeline è *risolta*, non cablata — che è la ragione
  per cui [[unification-assessment]] scarta l'architettura a 8 stadi top-down.
  `inference_cycle/2` (§4b) è il primo percorso reso, e serve anche P2/P3.
- [[generative]] §gen206 — «il generatore propone, l'oracolo dispone». Per la
  prosa l'oracolo è la prova di ogni strato (§2d.1).
- [[generation-kb-first]] — *«quale lettura del turno non è stata fatta?»*. Qui:
  quale **relazione fra tesi** non è stata nominata.
- [[frontier-kb-natural-dialogue]] §K6 — `answer_relation/3`, `answer_order/3`,
  `realization_candidate/3`: il vocabolario era già previsto lì.
- [[question-emergence]] §4d — un ciclo che non deduce è spazio negativo: la
  stessa cipolla che lo racconta può generarne la domanda.
- `kb/core/epistemic-status.p0` — i cinque stati; questo documento fa sì che gli
  stati 3 e 5 smettano di essere due template e tornino a essere due stati.
- `kb/core/procedures.p0:262-265`, `kb/core/discourse.p0:52` — il fold che c'è già.
