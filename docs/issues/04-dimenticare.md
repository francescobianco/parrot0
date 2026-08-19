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

## Risolto a gen420 — la mossa prima del contenuto

L'astrazione l'ha data `frontier-kb-natural-dialogue.md`, ed era **pertinente in
pieno**:

- **K3** — *«un turno è prima di tutto una mossa, e la naturalezza va cercata
  prima nella mossa, poi nella frase»*. Era esattamente la diagnosi: parrot0
  leggeva il **contenuto** («my name is franco») e mancava la **mossa**
  (dimentica). Da qui `mod_forget`, registrato **prima** dei lettori di contenuto;
- **K4** — *«un fatto può valere in un contesto senza sparire dagli altri»*. E la
  macchineria **esisteva già**: `holds_in`, `supersedes_in`,
  `context_superseded` in `context-scope.p0`. Dimenticare non cancella: **supera**.

```
> my name is franco          Nice to meet you, franco!
> forget my name             Done — I've let go of your name.
> what is my name            (il nome non viene più detto)

> forget that my name is franco   Done — I've let go of your name.
                                  (prima: «Nice to meet you, franco!»)
```

Il fatto resta in KB con la sua provenienza, marcato superato: chi guarda vede
**sia che il nome c'era sia che è stato ritirato**. È l'unica forma di oblio
compatibile con «non si toglie niente».

Una cosa imparata di traverso: far *cedere il passo* al lettore che sbagliava non
è bastato — l'asserzione dentro la ritrattazione è leggibile da più moduli, e
ognuno se la prendeva a turno («Nice to meet you», poi «Got it: your name is…»).
Inseguirli uno per uno sarebbe stato chiudere casi a mano. **Registrarsi prima**
è l'unico modo, ed è ciò che K3 prescrive.

### Il limite che resta

`user_value_read` consulta il superamento e tace, ma «what is my name» raggiunge
anche **un secondo percorso** che legge lo slot senza passare di lì. Il ritiro è
reale — il fatto è marcato, la ritrattazione non reimpara più — ma un lettore lo
scavalca.

Si chiude instradando **ogni** lettura di slot personale attraverso
`context_effective_belief`, che è precisamente ciò che K4 prescrive e che qui è
stato fatto solo a metà.

## Dove stava l'autocorrezione

**Non lo vede**, ed è il caso che dimostra meglio il limite di gen414. La lacuna
si registra solo per `blind_wall` e `informed_decline`; qui il turno finisce con
«Nice to meet you, franco!», che è una **risposta**, quindi il ciclo la considera
un successo.

È la classe `wrong_answer` di §9.1 — nominata nel 2026, mai implementata — e
richiede il substrato **S3**: una prova che la risposta sia una risposta *a quella
domanda*. Senza, questo prompt resterà invisibile per sempre, e nessun numero
del bilancio lo mostrerà.
