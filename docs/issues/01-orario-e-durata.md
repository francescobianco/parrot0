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

## Il ponte, chiuso a gen419c

Restava il punto 2 — i ruoli — e ora c'è, in `kb/core/time-questions.p0`, con lo
stesso disegno di `numeric-questions.p0`: **una cue nomina un operatore,
l'operatore è una procedura**, e nessun consumatore cambia.

```
what is 2 hours 45 minutes after 14:30   →  17:15
what is 30 minutes before 09:15          →  08:45
what is 3 hours after 22:00              →  01:00
what is 5 minutes after 09:00            →  09:05
what is 14:30                            →  14:30 is a time of day.
```

Tre cose imparate scrivendolo, tutte pagate:

1. **Il token deve sopravvivere alla tokenizzazione.** `14:30` si spezzava in
   `14` e `30`, quindi il transcoder non lo vedeva mai. gen399 aveva già la
   regola giusta su un carattere (un punto fra due cifre appartiene al numero);
   ora vale per **ogni separatore dichiarato**, quindi una notazione nuova non
   tocca il C.
2. **Il MANTRA #12 preso in castagna nel file scritto per applicarlo.** La prima
   stesura leggeva i numeri nudi in sequenza: `30 minutes before 09:15` diventava
   30 *ore* e rispondeva 03:15. Numero giusto, ruolo sbagliato.
3. **`naf` su una cue non è affidabile.** Le cue sono sottostringhe, quindi
   «hours» risultava presente dove non c'era. Si discrimina sulla **forma della
   lista dei numeri**, che è un dato certo.

## Cosa resta aperto

**Il prompt originale**, che è ancora tre frasi in un turno solo:

```
A train leaves at 14:30 and travels for 2 hours 45 minutes. When does it arrive?
→ «That looks like a arithmetic problem, and I cannot solve it yet.»
```

Le tre frasi separate ora funzionano; il periodo no. È lo **strato 3** di
[02](02-contrapposizione.md) e di tutti gli altri — un turno con più atti — e
resta il blocco più trasversale dei nove.

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
