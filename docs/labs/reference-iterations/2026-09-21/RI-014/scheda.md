# RI-014 — una parola è insieme nome e verbo, e a dirlo è il maestro

**ID, famiglia e contesto umano.** Famiglia: *un cancello compilato che rifiuta
ciò che il maestro ha appena detto*. Contesto umano: mezzo lessico tecnico
inglese è insieme nome e verbo — *pump, plate, drive, mount, coat, weld, press,
mill, file* — e quale dei due sia lo decide la frase.

**Commit/stato iniziale.** `108d4e23` (RI-013 chiusa), profilo `agi`, `en`.

**Stimolo congelato.**

```text
Pumps move fluid.
```

**Criterio.** Il fatto entra, e «What do pumps move?» risponde «Fluid».

**Risposta iniziale e limite osservato** (`r1-baseline.json`):

```text
> Pumps move fluid.
I didn't keep that: «pumps» e «fluid» don't read as things I can hold a fact about.
```

**La misura che isola la causa** — e che è quasi comica:

| frase | esito |
|---|---|
| **Compressors** move fluid. | Learned ✔ |
| **Pumps** move fluid. | respinta ✘ |
| **Welders** strike an arc. | Learned ✔ |
| **Engineers** direct the current. | respinta ✘ |

Stesso verbo, stesso oggetto, stessa forma. Cambia solo che «pumps» è anche la
forma verbale di *pump*, e «compressors» non è una forma di *compress*.

Il sito è `p0_atom_is_concept`: un atomo che contiene un chiusore di sintagma
non è un concetto, e un verbo di relazione è un chiusore. **E la dichiarazione
del maestro non poteva correggerlo**: dopo «A pump is a device.» la frase
successiva era respinta lo stesso.

## R4 — la cura, sotto il criterio del §0.5-bis

Il cancello decideva da solo; ora **chiede**. Una parola di cui il maestro ha
detto **che cos'è** — appartenenza a una classe con un nome pronunciato
(`class_surface/2`) e non di macchina — è una cosa, anche se è anche un verbo.
Il plurale si porta al singolare con la KB, come ovunque.

**Due strade scartate, e vale la pena averle scritte** (stanno nel commento in
`grammar.p0`, perché il prossimo non le riprovi):

1. `known_referent/1` — include già le entità che la IR lega **nel turno
   corrente** (`entity_role/2`): il cancello avrebbe accettato qualunque
   soggetto. Misurato: lo stimolo passava **anche senza la lezione**, e
   l'ablazione non mostrava più niente. Una cura che cancella la propria prova.
2. `fact_source($F, $A, $S)` da sola — bastava a rendere «pumps» una cosa,
   perché in KB c'è `fact_source(relation_verb(pumps), pumps, "pumps is a
   relation verb")`: una lezione **sulla parola**, non sulla cosa. La
   distinzione fra dire *che cos'è* e dire *come si comporta la parola* è
   esattamente il punto.

## R5 — certificazione (`r5-certification.json`)

| turno | esito |
|---|---|
| 1 | «Pumps move fluid.» → **respinta** |
| 2 | **la lezione**: «A pump is a device.» → «Learned» |
| 3 | **la stessa frase** → «Learned: pumps move fluid.» |
| 4 | «What do pumps move?» → «Fluid.» |
| 5-6 | controlli: «Compressors move fluid.», «Welders strike an arc.» invariati |
| 7-9 | **transfer, altro dominio** (strutture): «A plate is a part.» → «Plates carry load.» → «Load.» — *plate* è anche un verbo |
| 10 | **ablazione**: «forget that a pump is a device.» → «Forgotten» |
| 11 | la stessa frase → **respinta di nuovo** |
| 12 | controllo indipendente: «What do plates carry?» → «Load.» |

## R6 — conteggio e processo nuovo

```text
parrot0: routed 33 clause(s) into the KB tree
```

| categoria | n | clausole |
|---|---:|---|
| `W` fatti veri | **4** | `device(pump)`, `move(pumps, fluid)`, `part(plate)`, `carry(plates, load)` |
| `L` | 1 | `class_surface(part, part)` |
| `X` | 0 | — |

**Processo nuovo** (`r6-fresh-process.json`): «What do pumps move?» → «Fluid.»,
«What do plates carry?» → «Load.», e il replay di RI-011. `make soft-test`
verde in 13 s (il cancello enumera le classi con nome: costo misurato, nessun
aumento); `facts.p0t` e `basics.p0t` verdi.

## Limiti residui

1. La dichiarazione vale per il **singolare**: «Pumps move fluid.» funziona
   perché la KB porta «pumps» a «pump». Un plurale irregolare non provato.
2. Il cancello accetta ora ciò che il maestro ha dichiarato; **non** ciò che
   parrot0 ha letto da solo in un documento.
3. Restano respinte le frasi il cui soggetto non è stato dichiarato e non è nel
   lessico: è il comportamento voluto, ma significa che la prosa tecnica va
   preceduta da una presentazione delle cose di cui parla.

**Stato dell'iterazione:** completa.
**Classificazione del training:** `trained` (W = 4, L = 1, X = 0).
