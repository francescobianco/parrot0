# PARROTBENCH

> ## GRANDE DISCLAIMER: QUESTO NON E' UN TEST
>
> `parrotbench` e' una collezione universale di prompt e un benchmark di
> misura. **Non deve essere eseguito da `make test`, `make soft-test`, dalla
> suite TDD, da gate di regressione o da qualsiasi controllo automatico di
> qualita'.**
>
> La suite e' intenzionalmente grande, lenta e non bloccante. Deve essere
> eseguita manualmente dall'intente quando viene richiesta una misurazione per
> una nuova release di parrot0. Il numero prodotto da `make parrotbench` e' un
> indicatore comparativo fra release, non una condizione per dichiarare verde
> il progetto.

La root `tests/parrotbench/` contiene obbligatoriamente directory di slot con
nomi `slot-XXX`; ogni slot contiene le categorie e piccoli file `.p0t`, con nomi
auto-descrittivi come
`logic-causality-batch-001.p0t`. Il formato `.p0t` e' processato dal daemon nativo
`parrot0 --bench-engine`, variante di `--test-engine`: non esiste un runner
Python alternativo e i file non vengono interpretati da un harness parallelo.

Il bench-engine riusa il parser e i meccanismi di verifica del test-engine:

- sezioni `[test ...]`;
- turni `>` e aspettative `<`, `<~`, `<!`;
- multi-turno e multi-linea;
- `!assert`, `!forget`, `!reset`, `!reload`, `!timeout`;
- stesso Brain persistente durante il daemon.

Il contratto differente e' statistico: un fallimento viene registrato ma non
ferma il benchmark, perche' `parrotbench` misura una release e non e' un gate.
Il corpus **non viene passato a `--test`**, non viene aggiunto alla lista dei
test del Makefile e non viene eseguito da `make test`.

## Esecuzione manuale

```sh
make parrotbench
```

Il risultato riporta:

```text
parrotbench: 1234/10000 (12.34%)
```

Il `bench-engine` riceve uno slot alla volta dal client nativo `parrot0 --bench`
e aggiorna dopo ogni slot
`tests/parrotbench/results/progress.tsv`. Uno slot marcato `complete` viene
saltato alla successiva esecuzione; uno slot marcato `running` viene ripetuto,
cosi' un'interruzione non perde il lavoro gia' completato. Il registro e'
leggibile mentre il processo e' in corso. Per ripartire da zero si puo'
rimuovere manualmente `tests/parrotbench/results/progress.tsv`.

Il file `tests/parrotbench/results/histogram.tsv` aggrega per slot
`passed`, `failed`, `total` e `percent`: e' il dataset per istogrammi delle
abilita' in cui parrot0 eccelle o e' carente. Il file viene aggiornato dopo ogni
slot, quindi anche una run interrotta lascia una fotografia parziale esplicita.
La directory `results/` e' stato locale generato e ignorato da git: il corpus
dei prompt e' condiviso, mentre ogni release mantiene il proprio registro e la
propria misura.

Il punteggio usa matching morbido: una risposta passa se contiene il frammento
atteso dopo normalizzazione di maiuscole, punteggiatura e spazi. Il benchmark
non pretende di catturare tutta la qualita' di una risposta e non sostituisce
la revisione qualitativa dei transcript.

## Categorie

Il corpus comprende 56 categorie tra conoscenza, aritmetica, logica, causalita',
analogia, incertezza, salienza, identita', memoria, empatia, italiano,
traduzione, grammatica, formati, pianificazione, strumenti, codice, KB-first,
ontologia, creativita', sicurezza e input universale. Ogni categoria contiene
prompt semanticamente diversi e varianti di registro, lingua, vincolo e ruolo;
non e' una semplice tabella con un numero sostituito da un altro.

Per rigenerare il corpus dopo una modifica deliberata al benchmark:

```sh
python3 tests/parrotbench/generate.py
```

La rigenerazione e' un'operazione manuale del benchmark e non deve essere
collegata a build, test o hook automatici. Il generatore e' solo uno strumento
di manutenzione del corpus; l'esecuzione passa sempre dal `--bench-engine` C.

## Protocollo nativo

Il target avvia:

```sh
parrot0 --bench-engine --sock obj/bench-engine.sock \
  --bench-stats tests/parrotbench/results/progress.tsv
```

Poi invia ogni slot con:

```sh
parrot0 --bench tests/parrotbench/slot-001/logic/logic-causality-batch-001.p0t \
  --sock obj/bench-engine.sock
```

`--bench PATH` accetta anche una directory e la percorre ricorsivamente, oppure
un glob di directory/file `.p0t`:

```sh
parrot0 --bench tests/parrotbench --sock obj/bench-engine.sock
parrot0 --bench 'tests/parrotbench/slot-*' --sock obj/bench-engine.sock
```

La forma raccomandata e' la root `tests/parrotbench`: il client verifica che
esistano directory `slot-XXX`, invia ogni slot come un'unita' e rifiuta una
root senza slot con `bench: no test slots found`. Un glob di slot invia anch'esso
un gruppo alla volta. Non occorre scrivere una riga di shell per ogni categoria
o batch.

Al termine chiede il riepilogo e chiude il daemon:

```sh
parrot0 --bench-report --sock obj/bench-engine.sock
```

Il client e' un relay socket leggero, come `--test`; il Brain e il caricamento
della KB esistono solo nel daemon. Il daemon legge il registro all'avvio,
indicizzato per directory slot:

- `complete`: lo slot viene saltato;
- `running`: lo slot viene ripetuto, quindi un'interruzione non lo considera
  completato;
- assente: lo slot viene eseguito e registrato.

Se uno slot contiene categorie gia' presenti in un altro slot non c'e' collisione:
la chiave di ripresa e' il nome completo `slot-XXX`, non il nome della categoria.
Se l'esecuzione si interrompe durante uno slot, al riavvio viene rifatto l'intero
slot incompleto, non viene dichiarato completato a meta'.

Il record `running` viene scritto prima dell'elaborazione e il record
`complete` solo dopo aver verificato tutte le asserzioni dello slot. Il registro
e l'istogramma sono scritti in modo sostitutivo tramite file temporaneo, così
restano leggibili mentre il processo e' in corso.
