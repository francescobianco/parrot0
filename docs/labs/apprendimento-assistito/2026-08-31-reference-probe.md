# Sonda sul riferimento — che cosa fa un LLM, e che cosa ne prendiamo

Data: 2026-08-31 · Sonda: `tests/probes/reference_probe.py` ·
Trascritto: `tests/sym/reference-2026-08-31-minimax-m2.5.md`

> **Richiesta di F.:** «usa le sonde con LLM reali per capire le scelte fatte
> dall'LLM e indirizzarci anche noi» — e poi: «preferirei un modello **senza
> reasoning**, per vedere l'LLM puro come risolve».

## Nota sull'oracolo, prima dei risultati

Il primo giro e' fallito con `403 Forbidden` su ogni modello, e l'avevo letto
come chiave non autorizzata. Era sbagliato: i 403 erano **nomi di modello non
ammessi**. `kimi-k2.6` rispondeva, ma con `content: null` — e' un modello
*reasoning*, spende il budget nel pensiero e non arriva a rispondere.

Su indicazione di F. la sonda usa ora **`minimax-m2.5`**, che restituisce
`content` direttamente. Due accortezze sono finite nel codice perche' costate
misurando: un `max_tokens` stretto su un modello che pensa non produce una
risposta breve, produce **nessuna** risposta — e la si scambierebbe per un
rifiuto; e l'endpoint da 403 in modo intermittente, quindi un tentativo solo
registrerebbe un errore di trasporto come se fosse una mossa del modello.

## Le cinque mosse osservate

### 1. «Il primo» conta l'ordine di INTRODUZIONE

```text
> Ho messo il libro rosso sul tavolo.
> Il quaderno blu invece è nello zaino.
> Dov'è il primo?
oracolo < Il libro rosso è sul tavolo, come hai detto prima.
```

Non l'ordine delle parole nella frase: l'ordine in cui le cose sono **entrate
nel discorso**. È esattamente la scelta che avevamo fatto con
`referent_arg_position(1)` e l'ordine di introduzione — **già allineati**, e ora
non per intuizione.

### 2. L'ellissi eredita la RELAZIONE, non solo l'entità

```text
> Dov'è il primo?     oracolo < È sul tavolo, come hai detto prima!
> E il secondo?       oracolo < È sulla mensola!
```

«E il secondo?» non contiene nessun verbo. Il modello riporta **la relazione
della domanda precedente**. Regola di progetto per G4: un turno ellittico eredita
il predicato dell'ultima domanda, non solo il referente. parrot0 oggi risponde
alla prima e fa muro alla seconda: è il punto esatto in cui manca.

### 3. Davanti all'ambiguità non TRATTIENE — risponde e poi chiede

```text
> Dove si trova il libro?
oracolo < Ci sono due libri: il rosso è sul tavolo, il blu sulla mensola.
          Quale intendi?
parrot0 < Quale? book red, book blue                    (prima)
```

Questa è la mossa che ci mancava, ed è una lezione di merito: chiedere e basta è
**corretto e avaro**. Il modello dà entrambe le risposte *e poi* chiede, così chi
ha posto la domanda ha già l'informazione anche se non disambigua.

**Adottata oggi**: `Ce n'è più di uno — book red: table; book blue: mensola.
Quale intendi?`

### 4. Dimostrativo più proprietà risolvono

```text
> E quello rosso?
oracolo < Il libro rosso è sul tavolo, come hai detto prima.
parrot0 < Non capisco ancora.
```

Ellissi + dimostrativo + proprietà, tutto insieme. È G4, e la proprietà la
sappiamo già ricavare (G2): manca il dimostrativo come riferimento.

### 5. Il riferimento a vuoto si NOMINA

```text
> Dov'è il primo?        (nessun referente introdotto)
oracolo < Potresti darmi più contesto? Non sono sicuro a cosa ti riferisci
          con "il primo".
parrot0 < Non capisco ancora.
```

Il controllo negativo. Il modello **non inventa** — come noi — ma dice *che cosa*
non ha saputo risolvere, citando l'espressione. Un muro che nomina l'espressione
irrisolta è una richiesta di chiarimento; un muro generico è un vicolo cieco.

### 6. La correzione SOSTITUISCE, non accumula

```text
> Correzione: quello rosso l'ho spostato sulla mensola.
oracolo < Ah, capito! Ora è sulla mensola.
> Dove si trova adesso?
oracolo < È sulla mensola.
```

Il fatto vecchio viene **superato**, non affiancato. È `supersedes_in/3` — che
parrot0 ha già per il retract parlato — applicato al riferimento.

## Che cosa ne prendiamo, in ordine

| # | mossa | stato |
|---|---|---|
| 1 | ordine di introduzione | ✅ già allineati (G3) |
| 3 | l'ambiguità risponde e poi chiede | ✅ adottata oggi |
| 5 | il riferimento irrisolto si nomina | da fare, piccola |
| 2 | l'ellissi eredita la relazione | G4 |
| 4 | dimostrativo + proprietà | G4 |
| 6 | la correzione supersede | G4 |

## Nota di metodo

Il valore della sonda non è che l'oracolo «vinca» — vince, ed è ovvio. È che
**tre delle sei mosse erano decisioni che avremmo dovuto prendere a intuito**, e
due di quelle l'avevamo già presa uguale. La sonda le ha rese *verificate*
invece che plausibili, e ha aggiunto la sola che non avremmo trovato da soli:
non trattenere l'informazione mentre si chiede.
