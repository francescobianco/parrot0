# RI-008 — l'insistenza non è un turno più difficile

**ID, famiglia e contesto umano.** Famiglia: *condotta — l'arresto*. Atto nuovo
nel lotto: non una domanda, non un'asserzione, non una richiesta di fare, ma la
richiesta di **smettere**. Contesto umano: si cambia idea, e lo si dice con
forza. «Stop, never mind.»

**Commit/stato iniziale.** `f326f6ac` (RI-007 chiusa), profilo `agi`, `en`.

**Candidato precedente scartato, con motivazione.** Il primo candidato era il
confronto su una misura («Which is longer, the Po or the Tiber?»). Misurato:
bastano i due fatti veri — «The Po has length 652.», «The Tiber has length
406.» — e il turno risponde «Po.», e anche «Is the Po longer than the Tiber?»
rende «Yes.» La capacità **c'è già**; mancavano i dati. Insegnarli sarebbe
training di contenuti, che il §0.1 non conta come iterazione: candidato
scartato invece di fabbricare un rosso (`r1-baseline.json`).

**Stimolo congelato** (il preambolo è parte dello stimolo):

```text
Tell me about flurbles.
Stop, never mind.
```

**Criterio.** Si ferma, e lo dice. E il turno dopo riprende normale.

**Risposta iniziale e limite osservato:**

| turno | risposta |
|---|---|
| `never mind` | «All right. I will stop this proposal…» ✔ |
| `stop` | idem ✔ |
| **`Stop, never mind.`** | **«I don't understand that yet.»** ✘ |
| `forget it` | «Hmm, I don't know about forget yet.» (membro mancante della classe) |

Il difetto è **fenomenologicamente rovesciato**: le stesse due parole, dette con
più forza, si capiscono di meno. Chi insiste viene capito **meno** di chi dice
la cosa una volta sola.

## R4 — la diagnosi e la cura

`/debug` distingue i due turni in una riga: «never mind» esce da `turn_plan`
(il circuito dell'iniziativa), «Stop, never mind.» dal `fallback`. La causa è
nel contratto delle forme: `dialogue_shape(reject, whole)` — un atto di forma
`whole` vuole che il turno sia **soltanto** la sua apertura
(`dialogue_payload(…, whole, unspecified) :- naf(turn_word_at(…))`), e chi
insiste ne mette due.

La cura non allarga la forma: **riconosce la ripetizione** (KB, una regola in
`dialogue-initiative.p0`). La seconda apertura deve essere dello **stesso**
atto, cominciare dove finisce la prima e chiudere il turno. Vale per ogni atto
`whole` dichiarato — oggi `reject`, `accept`, `continue`, `overload`,
`first_step` — e non tocca gli atti che una coda ce l'hanno per contratto.

Una strada sbagliata, percorsa e abbandonata: la prima ipotesi era
l'autoesclusione del circuito sul confine di frase
(`dialogue_excluded :- turn_cue(sentence_boundary_cue)`). Misurata: le cue sono
`". "`, `"! "`, `"; "` — **non** la virgola, quindi non era quella. La regola
scritta su quell'ipotesi è stata tolta.

## R5 — certificazione (`r5-certification.json`)

| prova | esito |
|---|---|
| **stimolo** | «All right. I will stop this proposal; you can choose whether to return to it.» |
| conoscenza vera attorno | `The Rhone flows into the Mediterranean Sea.` → `Which sea does the Rhone flow into?` → «mediterranean sea.» |
| TR1 — ordine inverso, niente insegnato | `never mind, stop` → si ferma |
| TR2 — **altro atto** `whole`, niente insegnato | `continue, go on` → «I do not have an active proposal to continue.» (la risposta propria di quell'atto) |
| contrasto — due atti **diversi** non si fondono | `Stop, tell me about Berlin.` → non si ferma: prosegue su Berlino |
| il sapere sopravvive all'arresto | dopo l'arresto, `Which sea does the Rhone flow into?` → «mediterranean sea.» |

Fonte: `en.wikipedia.org/wiki/Rhône` — «discharging into the Mediterranean Sea
(Gulf of Lion)».

## R6 — conteggio e processo nuovo

| categoria | n | clausole |
|---|---:|---|
| `W` fatti veri | **1** | `flow(rhone, mediterranean_sea)` |
| `P`/`O` | resto | provenienza e battute |
| `X` | 0 | — |

**Processo nuovo** (`r6-fresh-process.json`): 7/7 — il fatto persiste,
l'arresto funziona e il turno dopo riprende, il transfer su `continue` risponde
nel merito, e i replay di RI-007 («north sea») e RI-005 («150 minutes»).
`make soft-test` verde in 12 s.

## Limiti residui

1. **`forget it` non è un arresto**: manca come membro della classe delle
   aperture, e per giunta collide con `forget that …` di RI-006. Un membro è
   una riga, ma la collisione va decisa prima.
2. L'insistenza è riconosciuta solo con **due** aperture adiacenti: «stop,
   stop, stop» non è stato provato.
3. Il contrasto passa perché i due atti sono diversi; un turno come «stop,
   never mind, tell me about Berlin» non è stato provato.

**Stato dell'iterazione:** completa.
**Classificazione del training:** `trained` (W = 1, X = 0).
