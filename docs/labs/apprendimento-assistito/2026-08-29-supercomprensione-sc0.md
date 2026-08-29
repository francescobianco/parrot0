# SC0 — rendere misurabile la supercomprensione

Data: 2026-08-29

Protocollo: [`LEARN_PROTOCOL.md`](../../../LEARN_PROTOCOL.md)

Piani: [`apprendimento-assistito.md`](../../plans/apprendimento-assistito.md),
[`frontier-kb-natural-dialogue.md`](../../plans/frontier-kb-natural-dialogue.md)

## Contratto del ciclo

- **Dominio:** comprensione documentale e scientifica.
- **Obiettivo:** trovare il primo punto strutturale in cui una richiesta di
  lettura perde tesi, supporto, sequenza o limite.
- **Budget:** tre brani principali, massimo dodici mosse diagnostiche.
- **Target:** baseline `W=0`; mappa del gap, nessun fatto di dominio consolidato.
- **Stop:** primo misclaim o meta-gap non spiegato.

Fonti usate per costruire parafrasi brevi e verificabili:

- [NASA, Apollo 13 Mission Details](https://www.nasa.gov/missions/apollo/apollo-13-mission-details/);
- [NASA, Apollo 13 crew returns safely](https://www.nasa.gov/history/apollo-13-crew-returns-safely-to-earth/);
- [USGS, Introduction to Subduction Zones](https://www.usgs.gov/special-topics/subduction-zone-science/science/introduction-subduction-zones-amazing-events).

Le fonti non sono state copiate nella KB e nessuna pagina e' stata salvata come
conoscenza. I passaggi della sonda sono parafrasi; la sessione sperimentale non
e' stata `/save`.

## Baseline: il fallimento era nascosto da una risposta

Input iniziale, quattro frasi vere su Apollo 13:

```text
Leggi questo breve testo: Durante Apollo 13, un serbatoio di ossigeno si ruppe
mentre la navicella era in viaggio verso la Luna. Poiche' l'incidente
disabilito' la normale fornitura di elettricita', luce e acqua, l'allunaggio fu
annullato. I controllori svilupparono un piano per usare il modulo lunare come
scialuppa di salvataggio e l'equipaggio torno' sano e salvo. Sebbene la missione
non fosse atterrata sulla Luna, la NASA la definisce un fallimento riuscito
perche' l'equipaggio sopravvisse.
```

Esito:

```text
Leggi was a mysterious Leggi. Then one day, leggi discovered what it meant to
be seen. ...
```

Classificazione: `IRRELEVANT`, misclaim M1/M19. La successiva domanda «che tipo
di lacuna hai avuto?» ha ricevuto «non ho ancora incontrato una lacuna»:
false-understanding M0. Infine la correzione naturale e' stata interpretata come
un fatto `created_by(...)` inventato. Quella sessione aveva quindi `X=1` ed e'
stata chiusa senza `/save`.

## Gerarchia di crescita provata

### 1. Lessico diretto: necessario, non sufficiente

```text
> "leggi" is an imperative opener
Learned: imperative_opener(leggi).
```

La storia non veniva piu' generata, ma il testo cadeva al muro su `durante`.
L'opener proteggeva dal consumer sbagliato senza ancora nominare quello giusto.

### 2. Nuovo atto naturale senza schema

E' stato aggiunto un modo generale di ancorare una formula introduttiva a una
che funziona gia':

```text
> impara "Leggi questo breve testo:" come un altro modo per introdurre "leggi:"
Ricevuto — «leggi questo breve testo:» d'ora in poi introduce lo stesso tipo di
contenuto di «leggi:».
```

Il teacher non nomina `segment_role`, `prose_source`, `faculty_for`, arita' o
predicati. Il ruolo viene dedotto dal modello con `input_segment`; la lezione e'
rifiutata se il modello non raggiunge una facolta' viva.

### 3. Tre collisioni rese esplicite

Il replay ha rivelato tre decisioni distinte che prima erano accidentalmente
l'ordine del registro:

1. il ruolo della fonte deve possedere tutto il payload;
2. la facolta' dichiarata deve ricevere la prima offerta;
3. «0 acquisiti, N saltati» e' un esito terminale onesto, non una resa da
   sostituire con un altro consumer.

La collisione piu' informativa e' stata `measurement error`: `error` veniva
letto come evidenza di un diagnostico di compilazione dentro un passaggio
scientifico. `segment_extent(Role, whole)` chiude la classe senza cablare quella
parola, un registro o una lingua.

## Replay e transfer

Nel processo finale, dopo una sola lezione della superficie:

| Brano | Route | Fatti | Saltate | Misclaim |
|---|---:|---:|---:|---:|
| Apollo 13: incidente, conseguenza, piano, qualifica | reader | 0 | 4 | 0 |
| Subduzione: processo, accumulo di stress, rilascio | reader | 0 | 3 | 0 |
| Forza epistemica: associazione, causalita', limite | reader | 0 | 2 | 0 |

`Transfer@3(route) = 3/3`; `claim coverage = 0/9`; `X=0` nel processo finale.
Un passaggio procedurale su tre fasi PCR era stato instradato anch'esso, ma il
lettore aveva riportato `0` fatti e soltanto `2` frasi saltate: anche la
contabilita' delle unita' e' quindi un gap da non confondere con D5.

## Ablazione e ratchet

Il test
[`taught_segment_role.p0t`](../../../tests/p0t/language/taught_segment_role.p0t)
usa un solo processo e verifica:

- baseline senza ruolo;
- lezione naturale ancorata a `read:`;
- replay e quattro transfer;
- una collisione con il registro `error`;
- ablazione del solo `segment_role` appreso;
- riapprendimento senza rebuild;
- ablazione e ripristino della policy di dispatch.

Esito mirato: `21 passed`.

Il solo `make soft-test` ammesso dal protocollo e' stato eseguito una volta. Si
e' fermato su un controllo non appartenente a questa modifica:

```text
frontier_chat_audit.it.p0t line 97
expected: I don't know about designation
got: I don't know much about your designation yet. Want me to look it up?
```

Il rosso non e' stato nascosto e il gate non e' stato rilanciato.

## Diagnosi e prossimo incremento

SC0 e' chiusa come diagnosi, non come comprensione scientifica. La capacita'
nuova e' **insegnare e proteggere l'atto di lettura**. Il primo gap semantico e'
ora netto: il lettore non rappresenta la funzione delle unita', lo status dei
claim, il supporto, la qualifica o i passi procedurali.

Il prossimo ciclo e' SC1. Deve insegnare a voce una relazione retorica fra due
span, trasferire su tre generi, conservare provenance, superare un contrasto e
sparire dopo retract. Aggiungere altri sinonimi di `read:` non aumenterebbe la
comprensione.
