# 2026-08-28 — Elettronica: le unità di misura come relazione insegnata

Sessione condotta secondo [`LEARN_PROTOCOL.md`](../../../LEARN_PROTOCOL.md).

**Stato finale: `trained`** — con due gap tipati non chiusi e un bug di
persistenza trovato *durante* la sessione e corretto prima del commit.

## 1. Parametri della sessione

| Parametro | Valore |
|---|---|
| `DOMINIO` | elettronica — unità di misura SI delle grandezze elettriche |
| `OBIETTIVO` | parrot0 sa dire quale grandezza misura una data unità, e impara una unità nuova senza che gli si rispieghi la forma |
| `BUDGET` | una unità causale (una lezione + il suo transfer) |
| `FONTI` | BIPM, *SI Brochure* 9ª ed. (2019), tabella delle unità derivate con nome proprio |
| `TARGET_WORLD_FACTS` | 3 |
| `TARGET_CAPABILITIES` | 0–1 |
| `STOP_CONDITION` | primo misclaim non spiegato |

Preflight: `git status --short` vuoto, `git diff --check` pulito, boot senza
`PARSE ERROR`. **`B0` = 34784 fatti, `R0` = 2391 regole**, profilo `agi.p0`.

## 2. Fonti e proposizioni candidate

Tutte dalla stessa fonte primaria (BIPM, *SI Brochure* 9ª ed., §2.3.4 — unità
derivate coerenti con nome speciale), valide alla revisione 2019.

| ID | proposizione | certezza | scope |
|---|---|---|---|
| E1 | l'ohm misura la resistenza elettrica | definizionale | unità SI derivata |
| E2 | il farad misura la capacità | definizionale | unità SI derivata |
| E3 | l'henry misura l'induttanza | definizionale | unità SI derivata |
| E4 | il volt misura la tensione | definizionale | unità SI derivata |
| E5 | il watt misura la potenza | definizionale | unità SI derivata |

Nessuna è inventata, nessuna è destinata alla cancellazione, nessuna era già
nella KB: `grep` su `kb/` mostrava solo glosse `circuit_element/2` in
`kb/experts/physics/electromagnetism.p0`, cioè prosa descrittiva e nessuna
relazione interrogabile fra unità e grandezza.

## 3. Baseline (§6.1)

| turno | esito |
|---|---|
| «what is the SI unit of electrical resistance?» | `WALL` |
| «which unit measures resistance?» | `WALL` |
| «what is a resistor?» → *limits current -- dissipates energy as heat* | `KNOWN_CORRECT` (glossa preesistente) |
| «what does a capacitor store?» | `WALL` |
| «is a diode a passive component?» | `WALL` |

La glossa esisteva, la relazione no: la baseline conferma che E1–E5 erano
conoscenza assente e non un ripasso.

## 4. Lezioni pronunciate

Il primo tentativo diretto ha prodotto una **lettura degenere**:

```
> the ohm is the SI unit of electrical resistance
parrot0: Learned: is_unit(ohm).
```

«SI» è stato normalizzato in «is», e la frase è collassata in un predicato
`is_unit/1` che non esiste da nessuna parte nell'albero (`grep` su `kb/` e
`src/`: zero occorrenze) e che perde del tutto «of electrical resistance». Il
fatto risultante era vero ma inutile, con provenienza corrotta
(`"the ohm is the is unit of electrical resistance"`). Non è stato promosso.

La lezione è stata allora data nella forma che parrot0 stesso suggerisce come
metalinguaggio naturale nel suo muro:

```
> measures is a relation verb
parrot0: Learned: relation_verb(measures).
> the ohm measures electrical resistance
parrot0: Learned: measures(ohm, electrical_resistance).
```

Nessun nome di predicato, arità, tupla o `!assert` è stato pronunciato dal
teacher: «*measures is a relation verb*» è vocabolario grammaticale che un
esperto di dominio userebbe, ed è la formulazione che parrot0 offre da sé.

## 5. Replay, transfer, parafrasi, contrasto, composizione

**Replay** — `pass`. La superficie interrogativa che funziona è
`<unità> measures what?`:

```
> the ohm measures what?
parrot0: Electrical resistance.
```

**Transfer@3 = 3/3.** Tre held-out reali, mai pronunciati nella spiegazione,
con entità e valori diversi:

| held-out | risposta |
|---|---|
| farad → capacitance | `Capacitance.` |
| henry → inductance | `Inductance.` |
| volt → voltage | `Voltage.` |

**Paraphrase = 2/2.** «which quantity does the farad measures?» → *Capacitance*;
«tell me what the henry measures» → *Inductance*. La forma passiva «what is
measured by the farad?» va a muro (gap M5, registrato, non risposta falsa).

**Contrast = 0/1 — fallito.** Il quasi-esempio è la forma polare su un valore
sbagliato:

```
> does the ohm measures capacitance?
parrot0: Electrical resistance.
> does the farad measures inductance?
parrot0: Capacitance.
```

Non è una risposta falsa — nessuna delle due frasi afferma il falso — ma è
`IRRELEVANT`: a una domanda sì/no risponde riempiendo lo slot oggetto invece di
confrontarlo con il valore chiesto. Deterministico e riproducibile.

**Composition = 0/1 — fallito.** «what is a capacitor and what does the farad
measures?» risponde alla prima metà dalla glossa preesistente e manda la seconda
al modulo dei nomi di modulo (violazione del mantra #10).

**Ablation = pass, con messaggio sbagliato.** «forget that ohm is a is_unit»
*ha* ritratto `is_unit(ohm)` — il dump lo conferma — ma la risposta stampata era
«Electrical resistance.», perché `answerframe` aveva già rivendicato il turno.
Lo stato cambia correttamente; la frase mente sull'esito. `fact_source` e
`reading_fact` della lezione ritratta restano come genealogia, che è il
comportamento voluto da M14.

**Retention = pass** dopo oltre cinque turni diversi.

## 6. Gap tipati e non chiusi

Trovati e conservati, non aggirati.

**G1 — la forma polare di un verbo relazionale insegnato non è duale con la
forma-wh (M5 + M0).** `answerframe` rivendica `does <X> measures <Y>?` e riempie
lo slot invece di verificare l'uguaglianza. La sonda `why did you answer that
way?` lo dice esplicitamente: «*then 'answerframe' claimed it — so the modules
after 'answerframe' were never consulted*». È un gap generale, non di dominio:
riguarda ogni relazione insegnata, non le unità di misura.

**G2 — `answerframe` sovra-rivendica anche le mosse (M12).** Una volta esistente
un `relation_verb`, turni che *non* sono domande — comprese le richieste di
ritrattazione — finiscono ad `answerframe` e ricevono una risposta di contenuto.
È la ragione per cui il messaggio `forgotten_fact` non si vede mai.

**G3 — persistenza dello scratch di turno.** `/save` instrada
`turn_counter/1` dentro `kb/core/discourse.p0` e `turn_span_token/4` nella
ricaduta. Non sono provenienza né stato dialogico: sono lo scratch del turno
corrente, e `turn_counter(42)` accanto a `turn_counter(0)`/`(1)` corromperebbe
al boot la regola `turn_bookkeeping`, che fa `retract`/`assert` su
`turn_counter($N)`. Escluse a mano dal commit, classe per classe e con motivo
dichiarato — non con un filtro automatico (§8). Il gap resta aperto: il
save-map non ha una casa corretta per lo scratch di turno.

**G4 — regressione preesistente, non di questa sessione.** `make soft-test` è
rosso su `tests/p0t/health.p0t:20` («what is 2 plus 2?» → «I don't understand
that yet.»). Bisezionata a `704f955`, che è già su `origin/main`; a `4208257`
la stessa sonda passava. Non toccata qui.

## 7. Il bug di persistenza trovato durante la sessione

La verifica §10 in processo nuovo ha insegnato `measures(watt, power)` senza
rispiegare la forma. `/save` ha risposto «*routed 20 clause(s) into the KB
tree*», ma il fatto **non era su disco in nessun file dell'albero**, mentre era
presente nel dump di sessione.

Causa, in `kb_save_routed` (`src/kb.c`): il confronto fra la casa risolta dal
save-map e la ricaduta era per stringa. Sotto `PARROT0_PROFILE`, cioè sotto
`make chat`, la mappa risolve `kb/profiles/../learning/learned.p0` mentre il
chiamante passa `kb/learning/learned.p0` — **lo stesso file, due stringhe**. Il
fatto veniva quindi contato come «sistemato altrove», inserito da `sm_insert`, e
subito dopo tolto dalla ricaduta e cancellato nella riscrittura del file.

Si manifesta solo dalla *seconda* sessione in poi su un predicato: alla prima il
predicato non ha ancora una casa e passa dalla ricaduta. Cioè colpisce
esattamente la crescita incrementale che questo protocollo prescrive, e la
nasconde dietro un conteggio che dichiara successo.

Rimedio: `sm_same_file/2` confronta `st_dev`/`st_ino`, non il testo del
percorso. Nessun vocabolario coinvolto — è meccanica di filesystem, non
conoscenza, quindi il C è il posto giusto (mantra #2 non si applica: non c'è un
«membro nuovo» da imparare domani).

Verificato dopo il fix, in un processo nuovo:

```
> the watt measures power
parrot0: Learned: measures(watt, power).
> /save
parrot0: routed 9 clause(s) into the KB tree
$ grep -n "measures(watt" kb/learning/learned.p0
118:measures(watt, power).
```

`make soft-test` eseguito una volta (§7.3): invariato rispetto alla baseline —
stesso unico fallimento G4, preesistente.

## 8. `/save` e conteggio

```
parrot0: routed 164 clause(s) into the KB tree
```

| Categoria | Conteggio | Clausole |
|---|---:|---|
| fatti veri del mondo `W` | **5** | `measures/2` per ohm, farad, henry, volt, watt |
| fatti linguistici `L` | 1 | `relation_verb(measures)` |
| costruzioni/regole/procedure `C` | 0 | — |
| provenienza/genealogia/gap `P` | 76 | `fact_source` 8, `reading_fact` 8, `bridged` 1, `machinery_gap` 10, `gap_anchor` 15, `gap_opaque` 12, `gap_kind` 11, `gap_source` 11 |
| altre clausole spiegate `O` | 82 | `utterance/3` — il log di conversazione come KB di sessione |
| false/non verificate/test `X` | **0** | — |

```text
Nuovi fatti veri del mondo salvati in KB: 5
Nuove clausole totali salvate e classificate: 164
Clausole dichiarate da /save (S): 164
Clausole invalide: 0
```

Escluse a mano dal commit e non conteggiate: 1 × `turn_counter/1`,
4 × `turn_span_token/4` — scratch di runtime, gap G3.

Fra le `P` restano due `fact_source`/`reading_fact` che riferiscono
`is_unit(ohm)` e `is_unit(forget)`: sono la **genealogia della lezione
ritrattata**, non asserzioni sul mondo. I fatti corrispondenti non sono attivi.

## 9. Verifica in processo nuovo (§10)

**`B1` = 34943, `R1` = 2391.** `B1 - B0` = 159, che è esattamente il conteggio
classificato al netto delle 5 clausole di scratch escluse. Nessun `PARSE ERROR`.

Senza ripetere alcuna lezione, con formulazioni diverse da quelle di
insegnamento:

| domanda | risposta | esito |
|---|---|---|
| the ohm measures what? | Electrical resistance. | ✓ |
| which quantity does the henry measures? | Inductance. | ✓ |
| tell me what the volt measures | Voltage. | ✓ |
| the farad measures what? | Capacitance. | ✓ |

**`FreshProcessRecall` = 4/4 = 100%.**

E la prova che conta per il mantra — *«parrot0 può impararne un nuovo membro
domani, senza ricompilare?»* — è passata nel processo nuovo, senza rispiegare la
forma:

```
> the watt measures power
parrot0: Learned: measures(watt, power).
> the watt measures what?
parrot0: Power.
```

## 10. Metriche

```text
LessonYield            = 1/2   (la lezione «SI unit» ha prodotto una lettura degenere, scartata)
Transfer@3             = 3/3
Paraphrase             = 2/2
ContrastPrecision      = 0/1   ← gap G1
Composition            = 0/1   ← gap G2
AblationFidelity       = 1/1 sullo stato, 0/1 sul messaggio ← gap G2
Retention              = pass
FreshProcessRecall     = 4/4 = 100%
FalseUnderstandingRate = 0     (nessuna affermazione falsa; due risposte IRRELEVANT, tipizzate)
WorldKnowledgeGain     = 5
TotalPersistedClauses  = 164
```

Gate `trained`: `W >= 1` ✓, `X = 0` ✓, fonti registrate ✓,
`FreshProcessRecall = 100%` ✓, `FalseUnderstandingRate = 0` ✓, nessun parse
error ✓, ogni modifica del diff spiegata ✓.

Gate per **nuova capacità generale**: **non superato** — `Contrast` e
`Composition` sono 0. I cinque fatti sono promossi *come fatti*; la relazione
`measures` **non** è dichiarata capacità generale chiusa. Lo strato resta
`partial` e i gap G1/G2 sono il lavoro successivo.

## 11. File toccati

- `kb/learning/learned.p0` — i fatti, la provenienza, il log
- `kb/core/meta.p0` — `gap_source/2` dei muri incontrati
- `src/kb.c` — `sm_same_file/2`, il fix di persistenza del §7
