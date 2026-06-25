# Unification: il cuore logico di Prolog e una possibile direzione per Parrot0

Uno dei concetti più potenti della programmazione logica è l'**unification** (unificazione). Si tratta dell'algoritmo alla base di Prolog e rappresenta un modo completamente diverso di interpretare l'informazione rispetto alla ricerca per stringhe o alla corrispondenza di pattern tipica di molti sistemi.

L'idea è semplice: invece di chiedersi se due espressioni siano uguali, l'unification si chiede:

> *"Esiste un insieme di sostituzioni delle variabili che rende queste due strutture identiche?"*

Ad esempio:

```prolog
padre(francesco, tancredi)
padre(francesco, X)
```

vengono unificate producendo:

```text
X = tancredi
```

Oppure:

```prolog
ama(X, pizza)
ama(luca, Y)
```

produce:

```text
X = luca
Y = pizza
```

L'algoritmo non confronta stringhe, ma **strutture ad albero**.

Ad esempio:

```prolog
persona(
    nome(francesco),
    vive(citta(roma))
)
```

può essere unificata con

```prolog
persona(
    nome(X),
    vive(citta(Y))
)
```

ottenendo

```text
X = francesco
Y = roma
```

L'unification quindi è un algoritmo di **matching strutturale**.

---

## Perché è così potente

La cosa interessante è che l'algoritmo non "capisce" il significato delle parole.

Capisce la **forma della conoscenza**.

Non cerca testo.

Non cerca parole.

Non cerca vettori.

Cerca strutture compatibili.

In pratica è una sorta di "incastro logico".

Questo permette di costruire motori di inferenza estremamente piccoli ma sorprendentemente espressivi.

L'algoritmo classico di Robinson (1965), che ancora oggi è alla base di Prolog, può essere implementato in poche centinaia di righe di codice C.

---

# Come potrebbe essere utilizzato in Parrot0

Parrot0 potrebbe utilizzare l'unification come **motore centrale della comprensione**, separando completamente la comprensione dalla generazione del linguaggio.

Un possibile flusso potrebbe essere:

```
Testo
    │
    ▼
Tokenizzazione
    │
    ▼
Parser linguistico
    │
    ▼
Rappresentazione logica
    │
    ▼
Unification
    │
    ▼
Inferenza
    │
    ▼
Planner
    │
    ▼
Generazione della risposta
```

In questo modello il parser non produce direttamente testo, ma costruisce una rappresentazione interna.

Ad esempio la frase

> "Vorrei comprare una casa."

potrebbe diventare

```prolog
intenzione(
    utente,
    compra,
    casa
)
```

Il motore di conoscenza potrebbe contenere regole come

```prolog
azione(compra, richiede, denaro).

azione(denaro, possibile_tramite, mutuo).

mutuo(richiede, banca).
```

L'unification e il motore di inferenza potrebbero allora dedurre automaticamente che:

* comprare una casa implica disponibilità economica;
* la disponibilità economica può essere ottenuta tramite un mutuo;
* un mutuo richiede una banca.

La risposta non sarebbe recuperata da un archivio di frasi, ma costruita seguendo la catena logica.

---

# L'unification come memoria

Normalmente un database funziona così:

```
chiave
    ↓
valore
```

L'unification invece funziona così:

```
struttura
        ↓
strutture compatibili
```

Non si cerca una parola.

Si cerca una forma.

Questo significa che la memoria diventa una gigantesca rete di relazioni che possono essere interrogate anche in modo parziale.

Ad esempio

```prolog
persona(X, Y, ingegnere)
```

potrebbe restituire automaticamente tutte le persone che soddisfano quella struttura senza conoscere in anticipo il loro nome.

---

# Perché è interessante per un agente conversazionale

Un agente conversazionale non deve necessariamente "prevedere la parola successiva".

Può invece:

* riconoscere l'intenzione;
* trasformarla in rappresentazione logica;
* applicare regole;
* verificare vincoli;
* dedurre conseguenze;
* costruire un piano;
* generare il testo finale.

L'unification diventa quindi il meccanismo con cui tutta la conoscenza viene "messa in contatto".

---

# Un possibile passo ulteriore

La parte più interessante è che Parrot0 non sarebbe costretto a limitarsi all'unification classica.

Potrebbe introdurre una **unificazione estesa**, dove due strutture non devono essere perfettamente uguali, ma possono essere considerate compatibili secondo criteri più ricchi.

Ad esempio:

* sinonimi;
* concetti equivalenti;
* relazioni ontologiche;
* similarità semantica;
* trasformazioni consentite;
* inferenze contestuali.

In questo modo l'unification smetterebbe di essere un semplice algoritmo di matching e diventerebbe il **motore cognitivo** del sistema.

---

# Una possibile visione per Parrot0

In questa architettura il linguaggio naturale non è il luogo in cui avviene il ragionamento.

È semplicemente l'interfaccia di ingresso e di uscita.

Il vero ragionamento avviene su rappresentazioni logiche interne, dove l'unification collega fatti, regole e concetti in modo deterministico. La generazione del testo diventa l'ultimo passaggio del processo, non il meccanismo con cui il sistema "pensa".

Questa separazione è interessante perché rende il motore di ragionamento indipendente dalla lingua utilizzata, più facilmente ispezionabile, spiegabile e verificabile. Inoltre si adatta bene agli obiettivi di Parrot0: un agente conversazionale leggero, implementabile in C, che punta a ottenere capacità di ragionamento attraverso algoritmi e strutture dati piuttosto che attraverso enormi modelli neurali.
