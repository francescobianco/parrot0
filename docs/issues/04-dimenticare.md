# 04 — Dimenticare, e il fallimento peggiore dei nove

> **Prompt (#23 dei cento).**
> `Forget my name.`
>
> **Esito:** «Hmm, I don't know about **forget** yet.»

## La traccia — e qui c'è di peggio del muro

| prova | esito |
|---|---|
| `my name is franco` | «Nice to meet you, franco!» ✔ |
| `Forget my name.` | «I don't know about forget yet» ✘ |
| **`forget that my name is franco`** | **«Nice to meet you, franco!»** ✘✘ |

L'ultima riga è il reperto. Una richiesta di **ritrattazione** viene letta come
**un'asserzione**: parrot0 non solo non dimentica — *reimpara la stessa cosa* e
conferma allegramente. Chi legge crede di essere stato ascoltato.

È il caso peggiore per il MANTRA #7: non un muro, un **successo apparente** nella
direzione opposta a quella richiesta.

## Perché non viene processato

1. **`forget` non è un verbo di atto.** Il turno non chiede un'informazione:
   chiede a parrot0 di **cambiare il proprio stato**. Non esiste una classe di
   verbi performativi, quindi il turno cade nei lettori di asserzione.
2. **La ritrattazione non ha una porta dialogica.** `kb_retract` esiste, e
   `!forget` esiste come primitiva **di test** — con tanto di commento che dice
   la cosa giusta: *«ciò che un test ha bisogno che sia assente è compito del
   test, non del caricamento»*. Ma quel ragionamento non è mai stato portato
   nella conversazione, dove è ancora più vero.
3. **La frase di ritrattazione contiene l'asserzione.** «forget that my name is
   franco» ha dentro «my name is franco», che è una forma che parrot0 legge
   benissimo — e la legge. È il verbo esterno a essere ignorato.

## Cosa manca

- una classe `performative(forget)` / `retract_marker/1` in KB — parole che
  segnalano un atto sullo stato invece di un contenuto;
- il consumatore generico: un turno con quel marcatore prende la sua parte
  interna, la legge come farebbe normalmente, e **ritratta invece di asserire**.

Il secondo è il pezzo di motore, ed è piccolo: la lettura interna esiste già,
cambia solo il verso dell'operazione finale.

## Dove sta l'autocorrezione

**Non lo vede**, ed è il caso che dimostra meglio il limite di gen414. La lacuna
si registra solo per `blind_wall` e `informed_decline`; qui il turno finisce con
«Nice to meet you, franco!», che è una **risposta**, quindi il ciclo la considera
un successo.

È la classe `wrong_answer` di §9.1 — nominata nel 2026, mai implementata — e
richiede il substrato **S3**: una prova che la risposta sia una risposta *a quella
domanda*. Senza, questo prompt resterà invisibile per sempre, e nessun numero
del bilancio lo mostrerà.
