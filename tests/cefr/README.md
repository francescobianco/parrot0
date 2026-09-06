# cefr-bench — che cosa parrot0 sa fare, per livello CEFR

```bash
make cefr-bench                                  # inglese, 25 frasi per banda
make cefr-bench CEFR_ARGS="--lang both"          # inglese + specchio italiano
make cefr-bench CEFR_ARGS="--per-band 40 --split test,dev,train"
make cefr-fetch-score                            # porzione SCoRE (NC), non versionata
```

## I dati non sono nostri

**CEFR-SP** — Yuki Arase, Satoru Uchida, Tomoyuki Kajiwara (EMNLP 2022).
Attribuzione, licenze, citazione e limiti: **[`ATTRIBUTION.md`](ATTRIBUTION.md)**.
Va letto prima di usare o ridistribuire i dati.

## Che cosa misura, e che cosa no

⛔ **Non** misura «quanto bene parrot0 parla inglese». CEFR-SP annota la
**difficoltà** di una frase, non la sua correttezza: ricavarne un voto di
competenza sarebbe un errore di categoria.

✅ Misura due cose, **stratificate per banda**, e il risultato è una **curva**:

- **lettura** — la frase produce una risposta o un muro;
- **giudizio** — parrot0 trova una regola che la decide, rifiuta onestamente, va
  a muro, o risponde **fuori tema**.

«Legge il 90% di A1 e il 40% di C1» dice dove intervenire. «6,3 su 10» no. E la
curva è anche il **curriculum**: si insegna in ordine di livello e si guarda dove
si muove.

⚠ **La colonna «lettura» è ottimista**: conta come letta ogni risposta che non
sia un muro, quindi *include le risposte fuori tema*. La colonna «fuori tema» del
giudizio è la correzione da leggere accanto.

⚠ **Il punteggio italiano non è veritiero** — frasi tradotte, etichette ereditate
— **ed è comunque utile** come indicatore di scostamento a parità di frase. Il
valore di riferimento citabile resta quello **inglese**. Vedi
[`data/it/PROVENANCE.md`](data/it/PROVENANCE.md).

## Il primo difetto che ha trovato

Nelle prime dodici frasi: *«There are four games in the series»* dichiarata
sbagliata, perché «there» sembrava singolare e «are» plurale. È il **soggetto
esistenziale**, dove il verbo concorda con ciò che segue. Curato come conoscenza
(`expletive_subject/1`), non togliendo la regola.

È il ciclo per cui il bench esiste: **bench → lacuna → conoscenza → bench**.
