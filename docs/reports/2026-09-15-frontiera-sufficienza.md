# Frontiera: sufficienza della prova e lettura della conversazione

Due sonde diagnostiche del 15 settembre 2026 per
[`interlocutore-di-frontiera.md` §9](../plans/interlocutore-di-frontiera.md#9-direzione-di-sviluppo-la-sufficienza-come-oggetto-insegnabile).
Codice a `9e22d09e`, `make build` riuscito, nessuna modifica al motore o alla KB.
Ogni sonda usa un processo MCP fresco, profilo completo `agi`, sessione non
salvata, lingua inglese. Il driver disabilita il fetch Wikipedia; conserva la
KB del profilo. Nessun `PARSE ERROR` nei due log.

Driver: `scripts/p0t-echo.py`. Non interpreta le asserzioni del runner e
**non certifica una suite**. Per riprodurre, mettere le sole righe `> ...` di
ciascun blocco in un file `.p0t` distinto e passarlo al driver; eseguirlo da
`/tmp` se si vogliono tenere lì i file `run-*` e `trace-*`.

## A. Lo stesso ignoto, tre obblighi diversi

Risposte verbatim; tempi di una singola esecuzione, senza valore statistico.

```text
> calculate ready by adding arrival and transfer
  = Held: calculate ready from arrival plus transfer.   [0.68s]
> transfer is never below 0
  = Held: transfer is never below 0.   [2.3s]
> the arrival of train is 10
  = Noted: the arrival of train is 10.   [1.13s]
> for train the ready must be at least 9
  = Held: the requirement concerns ready of train.   [0.83s]
> can I complete train?
  = For train: What is the transfer of train?   [1.61s]
> forget the requirement for train on ready
  = Forgotten: the requirement on ready of train.   [0.81s]
> for train the ready must be at most 11
  = Held: the requirement concerns ready of train.   [1.13s]
> can I complete train?
  = For train: What is the transfer of train?   [1.68s]
> prefer explaining obstacles to asking questions when a requirement fails
  = I have learned that conditional preference.   [1.01s]
> forget the requirement for train on ready
  = Forgotten: the requirement on ready of train.   [0.96s]
> for train the ready must be at most 9
  = Held: the requirement concerns ready of train.   [1.25s]
> can I complete train?
  = For train: No: ready is at least 10; the requirement is at most 9.   [1.65s]
```

**Diagnosi:** per il primo obbligo il limite basta già a confermare:
`10 + transfer ≥ 10 ≥ 9`. La domanda è superflua. Per il secondo, invece,
`transfer ≤ 1` è la condizione residua: chiedere il trasferimento è utile,
anche se una domanda sulla soglia potrebbe costare meno all'utente. Il terzo
mostra la capacità già presente: il limite confuta il requisito e la
preferenza insegnata seleziona la spiegazione.

La lettura del codice spiega l'asimmetria: `decision_met/4` richiede il valore
esatto; `decision_obstacle/4` può usare anche un limite inferiore;
`decision_missing_in/4` raccoglie foglie ignote delle formule. Questa sonda
non misura induzione automatica, soglie interrogabili o trasferimento.

## B. Il collegamento alla conversazione naturale resta aperto

Processo distinto, senza le lezioni della sonda A:

```text
> tomorrow I have an important meeting at 9 am with a client from Milan
  = Hmm, I don't know about important meeting yet. Want me to learn about it?   [1.54s]
> what time is my meeting?
  = Hmm, I don't know about meeting yet. Want me to learn about it?   [1.2s]
> I leave Rome at 7 by train and the trip takes 3 hours. Will I be on time?
  = I don't understand that yet.   [4.85s]
```

Il risultato conferma il gap di lettura e collegamento già registrato nel
piano. Non autorizza a inferire che la riunione sia a Milano: il testo
attribuisce Milano al cliente. Una futura risposta deve conservare questa
distinzione.

## Limiti della verifica

- 15 turni diagnostici complessivi; nessuna nuova esecuzione del banco E1 o
  della suite completa. I 52 assert di E1 sono il conteggio nel file corrente,
  non un nuovo risultato di esecuzione.
- Nessuna misura di accuratezza generale o latenza su una distribuzione.
- `src/kb.h` nel checkout dichiara `KB_MAX_ARGS = 4` e `KB_MAX_BODY = 16`.
  Il valore 8 nelle note precedenti è storico; controllare sempre l'header
  effettivo prima di attribuire un fallimento al limite del corpo.
