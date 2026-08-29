# SC32 — una lezione che non si può togliere non è una lezione

Data: 2026-08-30 · Protocollo: [`LEARN_PROTOCOL.md`](../../../LEARN_PROTOCOL.md)
· Origine: [§18.31](../../plans/frontier-kb-natural-dialogue.md), ipotesi **D28**

## La non conformità

`AGENTS.md` pone come condizione di rilascio KB-first:

> asserting a new cue must change recognition without rebuilding, and
> **retracting/ablating the cue must remove that recognition**.

Per l'intera classe delle cue insegnate via `learnable/3` la seconda metà **non
era esprimibile**. Misurato il 2026-08-29:

```text
> learn "helyla friend" as a casual opener
< Got it - I'll take «helyla friend» as a way to a casual opener now.
> forget "helyla friend" as a casual opener
< Hey! I'm here. …                     ← la cue si accende dentro il proprio retract
> stop treating "helyla friend" as a casual opener
< Hey! I'm here. …                     ← idem
> forget that the phrase "helyla friend" is a casual opener
< Learned: casual_opener(forget).      ← un fatto FALSO sul verbo di retract
```

Ogni frase che **nomina** la locuzione la **contiene**, e una cue `substring`
non distingue le due cose. L'unico tentativo che sfuggiva alla cue produceva la
specie `hypothesis_report_marker(forget)` che SC2-A aveva già dovuto guardare a
mano su un altro percorso — riaperta qui perché quella guardia è **locale a quel
percorso** invece di essere un invariante.

## Il test di D28

D28 prediceva: *se chiudere questo caso richiede una guardia in `mod_forget` o
in `mod_lone`, l'ipotesi è falsa e la strada dei moduli è inevitabile; se si
chiude con ruolo e scope, è confermata.*

È stata chiusa con due interventi, **nessuno dei quali tocca un modulo**.

### 1. Una cue non guarda dentro una menzione (D27)

Le superfici che aprono una menzione e le relazioni che devono ignorarne il
contenuto sono fatti; il C maschera soltanto i byte.

```prolog
mention_delimiter("\"", "\"").
mention_delimiter("'", "'").
cue_scope(intent_cue, outside_role(mention)).
cue_scope(intent_phrase, outside_role(mention)).
```

`whole_turn` resta il default implicito: una relazione senza riga si comporta
esattamente come prima, quindi il livello è **additivo**. Il costo per un turno
senza citazioni è una `strpbrk`; senza delimitatore presente non si legge
nemmeno la KB. Una citazione aperta e mai chiusa non è una menzione: è testo, e
la maschera non si applica.

### 2. Il retract passa dallo stesso registro del teach (D28)

`try_teach_form` insegna leggendo `learnable/3`. `try_forget_form` è lo **stesso
registro letto nell'altro verso**: stesso gate di menzione quotata, stessa
risoluzione etichetta → (intento, modalità), e il verbo che apre la mossa è un
fatto (`state_move_cue(_, retract)`), come già per `mod_forget`.

Corre **prima** di `try_teach_form` di proposito: «unlearn» contiene «learn», ed
è il mantra #8 un piano più su — misurato, prima di questo ordine
`unlearn "X" as a casual opener` **re-insegnava** la cue.

Solo le modalità che scrivono una superficie riconosciuta hanno un retract
simmetrico ovvio (`exact`, `substring`, `unary`). Le altre — `fill`, `define`,
le maniglie generiche — restano fuori e falliscono onestamente invece di
indovinare quale fatto togliere.

## Misura

```text
> learn "helyla friend" as a casual opener   -> Got it …
> helyla friend                              -> Hey! I'm here. …
> forget "helyla friend" as a casual opener  -> Forgotten - «helyla friend» is no longer a casual opener.
> helyla friend                              -> muro onesto
!query! intent_cue(casual, "helyla friend")  ✓
!query! casual_opener(forget)                ✓  nessun fatto sul verbo di retract
> forget "helyla friend" as a casual opener  -> I wasn't holding «helyla friend» as a casual opener.
```

**Transfer** su una seconda classe mai usata per progettare la correzione
(`mood_tired` invece di `casual`): teach → effetto → retract → effetto perso,
identico. E il retract funziona anche per la cue di un token solo che non
ottiene mai titolo (`mod_lone`): **togliere una lezione non dipende dal fatto
che qualcuno l'abbia ascoltata.**

`D28 confermata` su questo gradino: la chiusura è venuta da una politica di
scope e da un consumatore generico mancante, non da una guardia in un modulo né
da un riordino del registry.

## Che cosa resta aperto

1. `mod_lone` rivendica ancora ogni turno di un token solo: la cue insegnata è
   viva e non ha **titolo**. È SC30/SC31 (registro e ruolo decidono chi ha
   titolo), e resta il caso di prova.
2. Le modalità `fill` e `define` non hanno retract simmetrico.
3. `mention_delimiter/2` copre virgolette dritte e apostrofo; virgolette
   tipografiche, annidate e sequenze di fuga complesse restano il limite già
   dichiarato da SC2-A.
4. La maschera vale per `intent_cue` e `intent_phrase`. Ogni altra relazione di
   cue deve dichiarare il proprio `cue_scope` per ottenerla — additivo per
   scelta, ma è un censimento ancora da fare.

## Verifica (minima e contingente)

- `taught_cue_ladder.p0t`: **21 passed** (era 14, con il contratto rotto).
- Verdi: `mention` 24, `use_mention_lesson` 19, `document_claims` 182,
  `document_argument` 55, `retract` 17, `taught_lexicon` 35.
- `assisted_construction.p0t` 65/1 — rosso preesistente noto (attesa stantia
  sulle virgolette del template `teach_form_ack`).
