# 03 — La traduzione, e il caso più netto di tutti

> **Prompt (#28 dei cento).**
> `Translate the dog runs into Spanish.`
>
> **Esito:** «That looks like a **translation** problem, and I cannot solve it yet.»

## La traccia — e qui la diagnosi salta agli occhi

| prova | esito |
|---|---|
| `Translate the dog runs into Spanish.` | ✘ |
| `translate cane into spanish` | ✘ |
| **`how do you say dog in spanish`** | **«Perro.»** ✔ |
| `come si dice cane in spagnolo` | ✘ |

**La capacità c'è ed è ottima.** Il vocabolario è in KB (`tr/2` in `gloss.p0`), il
modulo esiste (`mod_translate`), e una forma di domanda ci arriva. Le altre tre no.

## Perché non viene processato

1. **È un *surface gap* puro** — la categoria che §9.1 di `question-emergence.md`
   aveva nominato nel 2026 e che nessuno ha mai attaccato. `R(q) ∧ ¬R(muta(q))`:
   la domanda raggiunge il consumatore in una formulazione e non nell'altra.
2. **L'imperativo non è una forma riconosciuta.** `how do you say X in Y` è una
   domanda; `translate X into Y` è un ordine. Il secondo non ha nessun lettore —
   ed è la forma più naturale in cui la richiesta arriva.
3. **La forma italiana non c'è affatto**, benché il vocabolario sia bilingue.

## Cosa manca

**Una riga di KB**, ed è il caso in cui il costo del rimedio e il costo del
fallimento sono più sproporzionati di tutto il documento:

```prolog
phrase_canon("translate", "how do you say").      % l'imperativo → la domanda
phrase_canon("come si dice", "how do you say").   % l'italiano → la stessa porta
```

`phrase_canon/2` esiste dal gen395 ed è esattamente il meccanismo per questo. La
resa esatta va provata (l'ordine degli argomenti cambia fra «translate X into Y»
e «how do you say X in Y»), ma la classe è quella e non richiede C.

## Dove sta l'autocorrezione

**Questo è il caso in cui il ciclo dovrebbe farcela da solo**, e vale la pena
dirlo perché è il più promettente dei nove:

- la lacuna è registrata, col suo registro (`translation`);
- il rimedio è una **riscrittura di superficie**, cioè `phrase_canon` — un fatto
  binario con un pattern, quindi **oggi non proponibile** (la diagonale);
- ma il candidato **si legge nel turno**: la parte da riscrivere è una
  sottostringa (`translate`), e la destinazione è una forma che parrot0 **sa già
  gestire**.

È l'unico dei nove in cui esiste un oracolo interno: parrot0 potrebbe provare una
riscrittura e verificare che la forma riscritta *ora risponde*. Non serve
indovinare il pattern — serve provare a sostituire un pezzo del turno con una
forma nota e vedere se il muro cade. Questo il ciclo lo sa già fare, in un'altra
veste, con le cue.
