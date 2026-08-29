# SC3 — il grafo argomentativo, e che cosa cade con una premessa

Data: 2026-08-29 · Protocollo: [`LEARN_PROTOCOL.md`](../../../LEARN_PROTOCOL.md)
· Predecessore: [SC2-D](2026-08-29-supercomprensione-sc2d.md)

## Perché adesso

Il report SC2-A si chiudeva con una condizione: *«Solo allora SC3 potrà collegare
premesse e conclusioni senza ragionare direttamente su stringhe.»* Quella
condizione è soddisfatta. SC1 ha dato al documento unità e archi retorici; SC2
ha dato a ogni unità una claim con status, attribuzione, fonte e proposizione
normalizzata. L'argomento è **il ponte fra i due** e non richiede un terzo
estrattore: una conclusione è una claim, una premessa è una claim, e ciò che le
lega è l'arco retorico già osservato.

## La proprietà che conta

Non è «so estrarre un argomento». È più stretta, e molto più utile:

> ritrarre la premessa ritira **soltanto** le conclusioni che ne dipendevano, e
> non tocca né la loro superficie né la loro fonte.

Perciò il supporto è una vista **viva**, non un fatto materializzato: dipende
dallo status della premessa, che dipende dalla cue viva (SC2-A). Un argomento
materializzato sopravviverebbe alla propria giustificazione — che è il modo
esatto in cui un lettore comincia a credere cose che non ha più ragione di
credere.

## Baseline

```text
> read: … The measurements indicate that DART slowed the orbital period.
        Therefore the authors conclude that kinetic impact can deflect an asteroid.
< … 2 proposition(s) normalized in quarantine
/debug rhetorical_edge   -> 0 bindings
> why?                   -> «That's the start of a question — what would you like to know?»
> what supports that?    -> «I don't understand that yet.»
```

Le due claim ci sono, con status opposti e fonte. Ciò che le lega, no.

## L'incremento

Tre righe di politica e una di struttura:

```prolog
rhetorical_marker_class(consequence_connector, consequence, previous_to_current).
rhetorical_marker_class(ground_connector, ground, current_to_previous).
argument_relation(consequence, conclusion_is(right)).
argument_relation(ground, conclusion_is(left)).
```

Quale lato porti la conclusione dipende dalla **relazione**, non dall'ordine
delle frasi: «A. Therefore B.» conclude a destra, «A. This is because B.»
conclude a sinistra. L'arco resta l'ordine del testo; chi legge decide.

E il livello vivo:

```prolog
claim_support_live($C, $P, $S) :- claim_supported_by($C, $P), claim_status($P, $S).
unsupported_conclusion($C) :-
    claim_supported_by($C, $P), naf(claim_has_live_support($C)).
```

Una conclusione rimasta senza premessa viva **non è falsa**: è rimasta senza
appoggio, e dirlo è diverso sia dal tacere sia dal continuare ad affermarla.

## Misura

```text
> therefore is a consequence connector
< Learned: consequence_connector(therefore).
> read: …   (REPLAY del turno che prima non produceva alcun arco)
!query argument_edge(…, …_unit_1_claim_0, …_unit_0_claim_0, consequence)   ✓
!query claim_support_live(…_unit_1_claim_0, …_unit_0_claim_0, observed)     ✓
> what supports that?
< It rests on: DART slowed the orbital period - reported as observed
  (from https://ntrs.nasa.gov/citations/20230015804).
> how do you know?
< Because I read it in …_unit_0 of https://ntrs.nasa.gov/…, at range(32, 30),
  and «the measurements indicate that» marks it as observed.
```

E la conclusione resta una claim riportata e modale: l'argomento **non la
promuove**. `holds_in(world, …)` è falso; `claim_modality(…, possible)` è vero.

### Contrasto

Lo stesso testo con `Nevertheless` al posto di `Therefore` produce un
`rhetorical_edge(…, contrast)` e **nessun** `argument_edge`. Un connettivo
avversativo non è un argomento, e la distinzione vive nella classe della
superficie, non in un ramo di C.

### Transfer

Due connettivi (`hence`, `consequently`) e due fonti mai usate per la lezione:

| Fonte | Premessa | Conclusione |
|---|---|---|
| WHO, vaiolo | «vaccination eradicated smallpox» (`observed`) | «vaccination eradicated a disease» (`hypothesized`) |
| PRL 116 061102, LIGO | «LIGO detected gravitational waves» (`observed`) | «a merger can produce a detectable signal» (`hypothesized`, `possible`) |

Notare `produce`: il verbo non è mai stato insegnato in quella forma — la radice
arriva da `produces` attraverso la morfologia KB di SC2-D.

### Ablation — il gate duro

Ritraendo il marker della premessa LIGO:

```text
!query unsupported_conclusion(…LIGO…_unit_1_claim_0)                        ✓
!query! claim_support_live(…LIGO…_unit_1_claim_0, …_unit_0_claim_0, observed) ✓
!query claim_supported_by(…)              ✓  l'arco resta
!query claim_surface(…, "LIGO detected gravitational waves")   ✓  la frase resta
!query claim_frame(…, detected, ligo, gravitational_waves)     ✓  il frame resta
!query claim_status(…_unit_1_claim_0, hypothesized)            ✓  la conclusione resta una claim
# E L'ARGOMENTO WHO, INDIPENDENTE, RESTA INTERO:
!query claim_support_live(…WHO…_unit_1_claim_0, …_unit_0_claim_0, observed)   ✓
!query! unsupported_conclusion(…WHO…_unit_1_claim_0)                          ✓
> what supports that?
< Nothing supports it any more - I no longer have a status for: LIGO detected
  gravitational waves.
```

Cade **solo** la conclusione dipendente. Ritraendo invece il *connettivo*, cade
l'arco e restano entrambe le claim con i loro status. Due ablation diverse
spengono due cose diverse: è la prova che i livelli non sono impilati a caso.

## Lezioni naturali e persistenza

```text
thus is a consequence connector        -> Learned: consequence_connector(thus).
accordingly is a consequence connector -> Learned: consequence_connector(accordingly).
since is a ground connector            -> Learned: ground_connector(since).
"on which evidence" is a claim support question -> Learned.
because is a ground connector          -> MURO, conservato come gap
```

Quattro su cinque. `because` è caduto perché la parola **«ground»** viene
intercettata da un altro consumatore — lo stesso quadro di `claim`, `shall` e
`known`: è D22, e il gap è nel registro.

```text
parrot0: routed 27 clause(s) into the KB tree
```

| Categoria | Conteggio | Contenuto |
|---|---:|---|
| `W` | 0 | — |
| `L` | 4 | 2 connettivi di conseguenza, 1 di motivo, 1 forma interrogativa |
| `C` | 0 | — |
| `P` | 4 | `fact_source(...)` |
| `O` | 19 | 4 `reading_fact`, 10 `utterance`, 5 clausole del gap-registry |
| `X` | **0** | — |

I connettivi sono stati instradati a mano in `kb/learning/taught-lexicon.p0`
(non avevano casa); `claim_support_question` ha invece trovato da sola la
propria, in `kb/core/document-argument.p0`, perché quel file porta il primo
membro della classe come seme di persistenza.

**Fresh process**, senza reinsegnare nulla: `thus` costruisce l'arco su una
fonte WHO e `«on which evidence?»` risponde con premessa, status e fonte.

## Due muri misurati, aperti

1. **Una menzione quotata perde copula e dimostrativo.**
   `"what is that based on" is a claim support question` produce
   `claim_support_question("what based on")`. La canonicalizzazione della
   domanda toglie parole *dentro le virgolette*.
   `canonicalization_exempt(mention)` esiste già in `kb/core/input.p0` ma non
   raggiunge questo percorso. È un muro sull'**addestrabilità** — non si può
   insegnare una forma interrogativa che contenga una copula — e vale una voce
   di coda propria. Il ratchet usa quindi una forma held-out senza copula
   (`"which evidence backs it"`), e il muro resta scritto.
2. **Collisione di dispatch** su `ground`, e in questa serie già su `claim`,
   `shall`, `known`. Quattro casi, sempre lo stesso quadro: dall'esterno
   indistinguibili da «non lo so». È D22/SC26.

## Verifica (minima e contingente, per richiesta)

- `document_argument.p0t` (nuovo): **55 passed**.
- `document_claims.p0t` 182/182, `document_rhetoric.p0t` 33/33.

## Metriche

| Metrica | Risultato |
|---|---:|
| LessonYield | 4/5 (la quinta è un gap classificato) |
| Replay | pass |
| Transfer (connettivi × fonti) | 2/2 |
| ContrastPrecision (avversativo ≠ argomento) | 1/1 |
| Ablation della premessa (solo la dipendente cade) | 1/1 |
| Ablation del connettivo | 1/1 |
| Crescita a runtime della forma interrogativa | 1/1 |
| FreshProcessRecall | 2/2 |
| `WorldCommitLeak` | 0 |
| `FalseUnderstandingRate` | 0 |

## Limiti che restano

1. Una premessa per conclusione. I **supporti congiunti** — «A e B insieme
   sostengono C» — sono il gate esplicito di SC3 nel piano e **non** sono
   chiusi: oggi due premesse produrrebbero due archi indipendenti, che è
   esattamente l'indebolimento che il piano vieta.
2. La direzione `ground` è dichiarata e non misurata: la cue retorica guarda il
   **token iniziale** dell'unità, e «This is because …» non comincia con il
   connettivo.
3. Obiezioni, qualificatori e rebuttal non esistono ancora: SC3 ha solo l'arco
   di supporto.
4. «Che cosa la confuterebbe?» non ha una porta.
5. L'ancora di «what supports that?» è il documento corrente, non un
   riferimento risolto: con due argomenti nello stesso documento risponde sul
   primo.
