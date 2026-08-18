# 01 — Sommare una durata a un orario

> **Prompt (#4 dei cento).**
> `A train leaves at 14:30 and travels for 2 hours 45 minutes. When does it arrive?`
>
> **Esito, prima di gen419:** «Hmm, I don't know about **travels** yet.»
> **Esito, dopo gen415:** «That looks like a **arithmetic** problem, and I cannot solve it yet.»
> **Risposta giusta:** 17:15.

## La traccia

Smontato pezzo per pezzo, il risultato è stato controintuitivo: **c'erano tutti i
mattoni**.

| pezzo | prova | esito |
|---|---|---|
| aritmetica pura | `what is 870 plus 165` | **1035** ✔ |
| unità di tempo | `time_unit(hour)` in `world-facts.p0` | presente ✔ |
| relazione fra unità | `how many minutes are in 2 hours` | «A hour has 60 minutes» ✔ |
| il token orario | `what is 14:30` | «I don't understand that yet» ✘ |
| la somma | `what is 2 hours 45 minutes after 14:30` | «2 is a is_prime» ✘ |

L'ultima riga è la diagnosi in una riga: davanti a un turno con tre numeri,
parrot0 **si aggrappa al primo che riconosce** e ne dice qualcosa di vero e
inutile.

## Perché non viene processato

1. **`14:30` è un token opaco.** Non è un numero, e nessun lettore lo scompone in
   (ore, minuti). Finché resta opaco, tutto il resto è irrilevante.
2. **I numeri non hanno ruolo.** Tre numeri nel turno — `14:30`, `2`, `45` — e
   nessuno è la risposta. Servono gli slot *istante di partenza*, *durata*, e il
   ruolo derivato *istante di arrivo*. È il MANTRA #12 non applicato al tempo.
3. **Manca il modulo 60.** `14:30 + 2h45m` non è `14+2` e `30+45`: i minuti
   traboccano e riportano un'ora. `convert 165 minutes to hours and minutes`
   falliva, quindi la conversione non esisteva da nessuna parte.
4. **`div` è divisione reale.** `div(165,60)` dà `2.75`. Il quoziente intero non
   era una primitiva — e la lezione è che **non andava aggiunta al C**: si
   compone da quelle che ci sono.

## Cosa mancava — conoscenza, non motore

Tutto e quattro sono stati chiusi a **gen419** senza toccare il ragionatore:

- `int_div/3` composta da `sub` e `mod`;
- `minutes_of`, `time_of_minutes`, `time_plus`, `time_minus`, `time_diff` in
  `procedures.p0` — regole, interrogabili e insegnabili;
- il **transcoder**: `transcode_shape(time, ":", 2)` in `grammar.p0` fa di
  `14:30` il termine `time(14, 30)`, e il motore non sa che cosa sia un orario —
  sa spezzare su un separatore e contare i pezzi.

La catena intera gira, dal token letto nel turno alla risposta calcolata:

```prolog
arrival($T) :- transcoded("14:30", $T0), time_plus($T0, dur(2, 45), $T).
→ time(17, 15)
```

## Cosa resta aperto

**Il ponte fra la frase e la procedura.** Le regole ci sono e il token è
strutturato, ma nessuno lega *«travels for 2 hours 45 minutes»* allo slot
`durata` e *«when does it arrive»* alla domanda `time_plus`. È il punto 2 —
i ruoli — ed è l'unico dei quattro ancora scoperto.

## Dove sta l'autocorrezione

Il turno **registra la lacuna** (gen414: `gap_kind = reachability`; gen415:
`gap_register = arithmetic`), quindi il ciclo lo vede. Ma il rimedio dichiarato
per `reachability` è una **cue**, e nessuna sottostringa di quel turno lo rende
risolvibile: il problema non è agganciare un modulo, è che manca il legame fra
frase e ruolo.

La riga che servirebbe non è ancora scrivibile:

```prolog
remedy_for(reachability, role_binding).   % ← e dietro non c'è ancora niente
```
