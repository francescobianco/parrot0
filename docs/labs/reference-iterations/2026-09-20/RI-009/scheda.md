# RI-009 — una cue dice che domanda è, non di che cosa parla

**ID, famiglia e contesto umano.** Famiglia: *pertinenza* — una risposta giusta
data alla situazione sbagliata. Contesto umano: si chiede aiuto per una cosa
concreta e si riceve un consiglio che vale per un'altra.

**Commit/stato iniziale.** `3522504e` (RI-008 chiusa), profilo `agi`, `en`.

**Stimolo congelato.**

```text
I am planning a three-day trip to Berlin: where do I start?
```

**Criterio.** Non il piano dello studio. Meglio: qualcosa che serva a chi parte.

**Risposta iniziale e limite osservato** (`r1-baseline.json`):

| turno | risposta |
|---|---|
| **I am planning a three-day trip to Berlin: where do I start?** | «Start small and regular: a short session three times a week…» ✘ |
| I want to learn Spanish: where do I start? | **la stessa identica risposta** — e lì è giusta |
| Where do I start? | la stessa |

È questo che rende la specie riconoscibile: **la stessa risposta a tre turni
diversi**, pertinente in uno solo. «Una sessione breve tre volte a settimana»
per tre giorni a Berlino non vuol dire niente, e non è un muro: è un consiglio
detto con sicurezza. La causa sta in una riga:

```prolog
direct_situation($Turn, user_getting_started) :- turn_cue_form($Turn, getting_started_cue, $Cue).
```

«where do i start» è la **forma** della domanda, e da sola decideva anche il
**tema**.

## R4 — la cura (tutta in KB)

1. **La guardia di pertinenza.** La cue della forma vale solo quando il turno
   non nomina già una situazione sua (`specific_situation_cue/1`, una lista che
   cresce di una riga per situazione nuova). Chi chiede «where do I start» e
   basta non perde niente.
2. **Il nome della situazione nuova** — `user_trip`, le sue cue e una mossa con
   il suo testo: è il supporto generale che **riapre la via alla lezione**.
3. **Il piano si insegna parlando**: «when someone is planning a trip then ask
   what they want to see» → `plan_move(user_trip, 1, ask_trip_goal)`.

**Una strada sbagliata, percorsa e corretta.** La prima guardia era scritta
sulle *situazioni derivate* (`naf(direct_situation(...))` dentro una clausola
di `direct_situation`): ricorsione negativa, e il solver declina tutto.
Misurato dalla regressione che ha prodotto: «I want to learn Spanish: where do
I start?» finiva al **traduttore** («I don't know the Spanish for «start»»).
Riscritta sulle **cue**, che sono una lettura di base: nessuna ricorsione.

## R5 — certificazione (`r5-certification.json`)

| prova | esito |
|---|---|
| meccanismo riparato, **senza** la lezione | lo stimolo non riceve più il piano dello studio: «That sounds nice — tell me more about it.» |
| lezione | «Held: in that situation, move 1 is «Start from what you want to see…».» |
| **stimolo** | **la risposta pertinente** |
| TR1 — altro viaggio, altra formulazione, niente insegnato | «I am planning a trip to Lisbon. Where do I start?» → la stessa mossa |
| **contrasto** — la regola non deve applicarsi | «I want to learn Spanish: where do I start?» → il piano dello studio, **intatto** |
| conoscenza vera attorno | `The Brandenburg Gate is in Berlin.` → `Where is the Brandenburg Gate?` → «berlin.» |
| **ablazione** | «unlearn what you do when someone is planning a trip» → «Forgotten: the plan for user trip.», e lo stimolo torna alla risposta generica |
| controllo indipendente sotto ablazione | il turno sullo spagnolo non cambia |

Fonte: `en.wikipedia.org/wiki/Brandenburg_Gate` — «a monument in Berlin, Germany».

## R6 — conteggio e processo nuovo

```text
parrot0: routed 18 clause(s) into the KB tree
```

| categoria | n | clausole |
|---|---:|---|
| `W` fatti veri | **2** | `located_in(brandenburg_gate, berlin)`, `located_in(reichstag, berlin)` |
| `C` | 1 | `plan_move(user_trip, 1, ask_trip_goal)` — il piano insegnato |
| `P`/`O` | resto | provenienza e battute |
| `X` | 0 | — |

**Processo nuovo** (`r6-fresh-process.json`): 5/5 — il piano insegnato
persiste, i fatti su Berlino rispondono, il contrasto regge, e i replay di
RI-008 (l'arresto) e RI-007 («north sea»). `make soft-test` verde.

## Limiti residui

1. La mossa del viaggio ha un **testo scritto a mano**: si insegna
   l'associazione situazione→mossa, non ancora le parole della mossa.
2. Le cue del viaggio sono una lista (`trip_cue`), non una lettura: «vado a
   Berlino per tre giorni» non è fra loro.
3. La risposta generica prima della lezione («That sounds nice — tell me more
   about it») è onesta ma non dice che **non sa** come si prepara un viaggio.

**Stato dell'iterazione:** completa.
**Classificazione del training:** `trained` (W = 2, C = 1, X = 0).
