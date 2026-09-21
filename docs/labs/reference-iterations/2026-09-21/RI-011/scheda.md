# RI-011 — un termine tecnico di più parole è un soggetto come gli altri

**Lotto `2026-09-21`** — conoscenza tecnica da domini disparati, per istruzione
di F.: niente capitali, fiumi o filosofi.

**ID, famiglia e contesto umano.** Famiglia: *il soggetto di più parole*.
Contesto umano: in ogni mestiere le cose si chiamano con tre o quattro parole —
«austenitic stainless steel», «shielded metal arc welding» — e sono i soggetti
normali di quelle conversazioni, non casi limite.

**Commit/stato iniziale.** `fa254ec8`, profilo `agi`, lingua `en`.

**Stimolo congelato.**

```text
Is austenitic stainless steel magnetic?
```

**Criterio.** Un «no» **guadagnato** dalla negazione detta, non un muro e non un
sì. Chi lavora l'acciaio usa la calamita per distinguere le serie: la risposta
sbagliata qui porta a scartare un pezzo buono.

**Fonti** (consultate il 21 settembre 2026):

| ID | proposizione | fonte |
|---|---|---|
| T1 | austenitic stainless steel is not magnetic | en.wikipedia.org/wiki/Austenitic_stainless_steel — «essentially non-magnetic» |
| T2 | ferritic stainless steel is magnetic | en.wikipedia.org/wiki/Ferritic_stainless_steel — «Ferritic stainless steels are magnetic.» |
| T3 | shielded metal arc welding is a process | en.wikipedia.org/wiki/Shielded_metal_arc_welding — «is a manual arc welding process» |

**Risposta iniziale e limite osservato** (`r1-baseline.json`): il termine di tre
parole entra da **una sola** porta e non esce da nessuna.

| turno | risposta |
|---|---|
| Austenitic stainless steel is a steel. | «Learned…» ✔ |
| What is austenitic stainless steel? | «classed as steel» ✔ |
| **Is austenitic stainless steel a steel?** | «Hmm, I don't know about **austenitic** yet.» ✘ |
| **Is austenitic stainless steel magnetic?** | idem ✘ |
| Austenitic stainless steel is ductile. | idem ✘ |
| Austenitic stainless steel is not magnetic. | «I looked up «austenitic» but found nothing.» ✘ |

`/debug` sul turno che fallisce nomina la cosa meglio di qualunque grep:

```text
debug_turn_entity   steel  austenitic_stainless_steel
turn_class_read     niente
turn_gap_remedy     manca la STRADA: una superficie che porti il fatto che gia' possiedo al turno
```

**La IR lega già il termine intero**, e parrot0 stesso dice che manca la strada.
È il cassetto senza maniglia, sulla classe di soggetti più comune del mondo
tecnico.

## R4 — la cura

- **KB**: `turn_entity_named/2` — come si scrive, a parole staccate, un'entità
  che la IR ha **già legato in questo turno**. Nessuna lista, nessuna scansione:
  il pezzo `named` delle forme di turno prende la superficie più lunga che
  questa relazione riconosce.
- **KB**: quattro forme — la lezione di proprietà («X is <proprietà>»), la
  polare sulla proprietà, e la polare di classe con l'articolo come **ancora**
  (`span`), perché con quattro parole la IR lega solo dei pezzi e `named` non
  avrebbe che cosa riconoscere.
- **C**: l'atto `answer_unary_polar`, con la scala di sempre — sì dai fatti, no
  solo se **guadagnato**, altrimenti l'onestà.
- **C**: quando la IR incolla soggetto e proprietà in un'entità sola
  (`austenitic_stainless_steel_ductile`, perché fra le due non c'è marcatore),
  **il confine lo cerca la KB**: si taglia da destra e si tiene il primo taglio
  in cui a sinistra c'è qualcosa di cui la KB sa qualcosa e a destra un
  predicato che sa interrogare. Nessuna parola nel C.
- **C**: la frase di una forma può nominare **qualunque suo pezzo** (prima ne
  arrivavano tre: soggetto, relazione, oggetto — e il resto restava scritto
  `{class}` dentro la risposta).
- **C**: gli **atti** di una forma asseriscono nello strato che si salva. È la
  stessa cosa che RI-004 ha trovato nel lettore delle classi, qui nel
  linguaggio degli atti: «Austenitic stainless steel is a steel.» rispondeva
  «Learned» e dopo il `/save` in KB non c'era.

**Il confine di questa forma, trovato da due regressioni.** Su un soggetto di
**una parola sola** la forma **cede**: il lettore storico sa di più — distingue
il «no» guadagnato dal **conflitto** («is socrates a man?» con il fatto e la sua
negazione entrambi tenuti → «Conflicted.») e dice «non provato non è lo stesso
che falso». Misurato: senza quel confine `persist.p0t` andava 31/6 → **30/7** e
`facts.p0t` perdeva due righe. Peggiorare per uniformità è comunque peggiorare.
E lo `span` non salta l'articolo come fa `slot`: «is a tiger a mammal?» chiedeva
`mammal(a_tiger)` (rosso di `basics.p0t`, preso da `make soft-test`).

## R5 — certificazione (`r5-certification.json`)

| prova | esito |
|---|---|
| **stimolo** | «Is austenitic stainless steel magnetic?» → **«No.»** |
| polare di classe sullo stesso termine | «Is austenitic stainless steel a steel?» → «Yes.» |
| **contrasto** — la negazione non deborda sul vicino | prima della lezione «Is ferritic stainless steel magnetic?» → onesto «non so» |
| TR1 — stesso dominio, proprietà opposta | insegnato T2, «Is ferritic stainless steel magnetic?» → «Yes.» |
| TR2 — **altro dominio, quattro parole** | «Shielded metal arc welding is a process.» → «Is shielded metal arc welding a process?» → «Yes.» |
| **ablazione** (ritiro di RI-006) | «forget that shielded metal arc welding is a process.» → la polare torna onesta |
| controllo indipendente | «Is austenitic stainless steel a steel?» → «Yes.» |

## R6 — conteggio e processo nuovo

```text
parrot0: routed 17 clause(s) into the KB tree
```

| categoria | n | clausole |
|---|---:|---|
| `W` fatti veri | **4** | `steel(austenitic_stainless_steel)`, `not(magnetic(austenitic_stainless_steel))`, `magnetic(ferritic_stainless_steel)`, `process(shielded_metal_arc_welding)` |
| `L` | 1 | `class_surface(process, process)` |
| `X` | 0 | — |

**Processo nuovo** (`r6-fresh-process.json`): 5/5, compreso «Is a tiger a
mammal?» → «Yes.» (il lettore storico intatto). `make soft-test` verde in 12 s;
`persist.p0t` torna a 31/6, cioè la misura di prima.

## Limiti residui

1. La IR non lega il termine di **quattro** parole come un'entità sola: la
   polare di classe funziona per l'ancora dell'articolo, non per `named`.
2. Il taglio dell'entità incollata prova solo i confini di parola da destra:
   una proprietà di due parole («not heat treatable») non è stata provata.
3. La lezione di proprietà scrive `{proprietà}({soggetto})`: una proprietà con
   un valore («has a tensile strength of 520 MPa») è un'altra forma.

**Stato dell'iterazione:** completa.
**Classificazione del training:** `trained` (W = 4, L = 1, X = 0).
