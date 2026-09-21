# RI-007 — al posto di un nome può stare una descrizione

**ID, famiglia e contesto umano.** Famiglia: *una descrizione come argomento* —
causa nuova: non una superficie mancante né un verso, ma **un pezzo di turno
letto e buttato via**. Contesto umano: si nomina una cosa dalla relazione che
la individua, perché il nome non si ricorda o non serve: «il fiume della
Germania», «il maestro di Alessandro».

**Commit/stato iniziale.** `ee05216c` (RI-006 chiusa), profilo `agi`, `en`.

**Stimolo congelato.**

```text
Which sea does the river of Germany flow into?
```

**Criterio.** «north sea», passando per il Reno. Non un elenco di mari.

**Risposta iniziale e limite osservato** (`r1-baseline.json`, `r5a-before-lessons.json`):

| turno | risposta |
|---|---|
| **Which sea does the river of Germany flow into?** | **«north sea, adriatic sea, black sea.»** ✘ |
| What is the river of Germany? | «I don't know about river.» (eppure `river_of(germany, rhine)` è in KB) |
| Which sea does the river of Austria flow into? | stesso elenco ✘ |
| Who taught the teacher of Alexander the Great? | **«aristotle.»** ✘ — è il maestro, non il suo |

La specie è sempre la stessa e sempre la peggiore: la descrizione viene
**scartata** e la relazione esterna si applica all'entità interna, con una
risposta sicura e sbagliata. `/debug` lo mostra senza margini:

```text
debug_frame_record   frame(question, binary(teach)), roles(subject(missing), object(teacher))
debug_np_candidate   the teacher   the great
```

Il frame ha letto «chi ha insegnato *teacher*», e «of Alexander the Great» non
è entrato da nessuna parte.

## R4 — la cura

**Una lezione, non una riga di motore, apre la descrizione.** «river is a
relation» esisteva già come forma (`relation_noun/2`, catalogo §B): dopo di
essa «What is the river of Germany?» rende «rhine». Il pezzo mancante era
usarla **come soggetto di un'altra domanda**.

- KB (`messages.p0`): due forme che dicono dove stanno i pezzi —
  `which <classe> does the <nome> of <entità> <relazione> [particella]?`.
  Il nome della relazione resta conoscenza, non una lista.
- C: l'atto `answer_relation_in_class` di RI-001 risolve il soggetto quando al
  suo posto c'è una descrizione, chiedendo a `relation_noun/2` quale relazione
  porti quel nome e leggendone il valore.
- E se la descrizione **non** si risolve, il turno resta comunque suo, con il
  muro onesto: cederlo significava consegnarlo a chi elenca la classe — che
  rispondeva «north sea, adriatic sea, black sea» a una domanda su una città.

## R5 — certificazione (`r5-certification.json`)

| prova | esito |
|---|---|
| prima delle lezioni | l'elenco dei mari, e «I don't know about river» |
| dopo `river is a relation` | «What is the river of Germany?» → **«rhine.»** |
| **stimolo** | **«north sea.»** (descrizione → Reno → mare del Nord) |
| TR1 — altra descrizione, altra catena, **niente insegnato** | `river of Austria` → **«black sea.»** (Danubio → mar Nero) |
| TR2 — con due fatti veri nuovi | `The Nile flows into the Mediterranean Sea.` + `The Mediterranean Sea is a sea.` → `river of Egypt` → **«mediterranean sea.»** |
| TR3 — un fatto vero nuovo | `The Thames flows into the North Sea.` → `river of England` → **«north sea.»** |
| contrasto — la descrizione si risolve ma non c'è nulla da dire | `river of Brazil` → «I don't know: nothing I hold gives amazon a sea there.» |
| contrasto — la descrizione non si risolve | `capital of Germany` → muro onesto, **non** l'elenco dei mari |
| **ablazione** (con il ritiro di RI-006) | `forget that the Nile flows into the Mediterranean Sea.` → `river of Egypt` torna al muro |
| controllo indipendente sotto ablazione | `river of England` → «north sea.» (resta) |

Fonti: `en.wikipedia.org/wiki/Nile` («empties into the Mediterranean Sea»),
`en.wikipedia.org/wiki/River_Thames` (mouth: Thames Estuary, North Sea).

## R6 — conteggio e processo nuovo

```text
parrot0: routed 30 clause(s) into the KB tree
```

| categoria | n | clausole |
|---|---:|---|
| `W` fatti veri | **3** | `flow(nile, mediterranean_sea)`, `flow(thames, north_sea)`, `sea(mediterranean_sea)` |
| `C` | 1 | `relation(river)` — la lezione che apre la descrizione |
| `L` | 1 | `class_surface(relation, relation)` |
| `P`/`O` | resto | provenienza e battute |
| `X` | 0 | — |

**Processo nuovo** (`r6-fresh-process.json`): 7/7 — Germania «north sea»,
Egitto «mediterranean sea», Austria «black sea», `What is the river of France?`
→ «seine», il contrasto onesto, e i replay di RI-006 («adriatic sea») e RI-003
(«Naples»). `make soft-test` verde in 14 s.

## Limiti residui

1. **«Who taught the teacher of Alexander the Great?» resta sbagliata.** Lì il
   nome della relazione è un **agentivo del verbo** (teach → teacher), e
   `relation_noun/2` non lo deriva: la descrizione si apre solo dove il nome ha
   già la sua relazione. La specie è chiusa; questo membro no.
2. Il muro onesto nomina l'entità e non la descrizione («gives germany a sea
   there» invece di «the capital of germany»).
3. `capital_of` non ha fatti (le capitali stanno sotto `capital_of_country`):
   la descrizione «the capital of X» non si risolve per questa via.

**Stato dell'iterazione:** completa.
**Classificazione del training:** `trained` (W = 3, C = 1, L = 1, X = 0).
