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

`parrotbench/corpus/` contiene almeno 10.000 prompt organizzati in categorie e
slot `.p0t` piccoli. Il formato `.p0t` e' usato come contenitore leggibile per prompt,
aspettative e categorie; **non viene passato a `--test`** e non viene aggiunto
alla lista dei test del Makefile.

## Esecuzione manuale

```sh
make parrotbench
```

Il risultato riporta:

```text
parrotbench: 1234/10000 (12.34%)
```

Il runner esegue uno slot alla volta e aggiorna dopo ogni slot
`tests/parrotbench/results/progress.tsv`. Uno slot marcato `complete` viene
saltato alla successiva esecuzione; uno slot marcato `running` viene ripetuto,
cosi' un'interruzione non perde il lavoro gia' completato. Il registro e'
leggibile mentre il processo e' in corso. Per ripartire da zero si puo'
rimuovere manualmente `tests/parrotbench/results/progress.tsv`.

Il file `tests/parrotbench/results/histogram.tsv` aggrega per categoria
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

Per verificare il runner senza lanciare l'intera misura si puo' usare un
limite esplicito, che non fa parte della normale misurazione di release:

```sh
PARROTBENCH_MAX_SLOTS=1 make parrotbench
```

La rigenerazione e' un'operazione manuale del benchmark e non deve essere
collegata a build, test o hook automatici.
