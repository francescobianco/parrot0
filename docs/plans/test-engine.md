# The test-engine — one live instance validates `.p0t` suites

> **Stato:** base minima, gen345. `parrot0 --test-engine` valida uno stream di
> testo `.p0t` contro UN solo brain vivo. `make test` è il nuovo entrypoint; il
> vecchio `make test` è diventato `make legacy-test` e le sue suite `.chat` vanno
> migrate in file `.p0t`. Le primitive di mock oltre a `> / <` sono minime e
> crescono **su richiesta** (F.: "ti dico io cosa moccare").

---

## 0. Perché

Il vecchio harness (`tests/run.sh`, file `.chat`) avviava **un processo per caso**
e ricaricava tutta la KB ogni volta — quasi tutto il costo di `make test` era
startup ripetuto, non inferenza. Il test-engine avvia **una** istanza e le invia
tutti i file: la KB si carica una volta sola.

## 1. Avvio — demone + client

L'engine è un **demone** su socket Unix; i file si mandano dopo, uno per volta.

```
parrot0 --test-engine [--sock PATH]     # demone: UN brain, in ascolto sul socket
parrot0 --test-send FILE [--sock PATH]  # client: manda un file .p0t al demone
parrot0 --test-report      [--sock PATH]# client: chiede il totale e ferma il demone
```

Il **client non carica nulla** (né brain né KB): è solo un relay sul socket, così
ogni riga di `make test` costa quasi zero. Lo stato (brain, variabili, conteggi,
conoscenza insegnata) **persiste tra i file** perché è un solo demone. Il socket
di default è `obj/test-engine.sock` (override con `--sock`).

`--test-report` stampa il totale e fa uscire il demone con codice `1` se qualche
asserzione è fallita, `0` altrimenti — così `make test` fallisce di conseguenza.
Il report è una riga `PASS`/`FAIL` per asserzione più il totale finale.

## 2. La sintassi `.p0t` (base minima)

```
# commento                  riga di commento (anche le righe vuote) — ignorata
[test NOME]                 apre una sezione con nome (per il report)

> testo                     invia `testo` a parrot0 come un turno utente
< testo                     asserisce che la risposta è `testo`
```

Una sezione è **multi-turno**: più coppie `> / <` girano in ordine sullo stesso
brain vivo (quindi la coreferenza e lo stato tra turni sono testabili). Una
risposta è **multi-linea**: si scrive un `<` per ogni riga di output e i `<`
consecutivi vengono uniti e confrontati con l'intera risposta multi-riga.

**Solo questo, per adesso.** Mock, stub, flag e controllo dello stato della KB
**non** fanno parte di questa base: la loro sintassi verrà definita da F. più
avanti. `!shutdown` è l'unica riga di controllo interna (la manda
`--test-report`) e non è una primitiva di test.

## 3. `make test`

Legge, visivamente e logicamente, come «avvia engine, manda primo file, manda
secondo file, …, chiedi il totale». Ogni `--test-send` è una riga esplicita
(niente `for`): per saltare una suite basta commentarne la riga.

```make
test: build
	@./$(BIN) --test-engine &
	@./$(BIN) --test-send tests/p0t/basics.p0t
	@./$(BIN) --test-send tests/p0t/mocks.p0t
	@./$(BIN) --test-report
```

## 4. Fail-fast

`--test-send` esce con `1` non appena il suo file ha un'asserzione fallita, quindi
`make` si ferma su quella riga (il report del file, con la riga `FAIL`, viene
stampato prima). Il target `test-engine` possiede il ciclo di vita del demone in
background: uccide un'istanza rimasta viva (via pidfile) e ne avvia una fresca,
così un run fallito si rilancia pulito.

## 5. Cosa NON fa (ancora)

- Niente mock / stub / flag / controllo stato KB: **la loro sintassi la definisce
  F.** Per ora l'engine fa solo l'assert della risposta attesa, multi-turno e
  multi-linea.
- La migrazione delle suite `legacy-test` in `.p0t` è incrementale.

## 6. Rotta

1. Definire (con F.) la sintassi di mock / stub / flag / controllo stato KB.
2. Migrare le suite `.chat` deterministiche in `tests/p0t/*.p0t`.
3. Aggiungere il cambio di working directory per i test del coding-agent
   (fixtures) quando serve.