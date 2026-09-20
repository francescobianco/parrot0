# RI-002 — una relazione ha un verso, e la domanda polare non lo inverte

**ID, famiglia e contesto umano.** Famiglia: *direzione di una relazione nella
domanda polare*. Contesto umano: chi studia storia della filosofia confonde
regolarmente chi ha insegnato a chi, e chiede in tutti e due i versi. La
risposta sbagliata qui non è un muro: è un «sì» su persone vere.

Il limite viene da RI-001, che lo ha misurato e dichiarato invece di nasconderlo.

**Commit/stato iniziale, profilo e hash del binario.** commit `1df94622`
(RI-001 chiusa), profilo `kb/profiles/agi.p0`, lingua `en`.

**Stimolo e preambolo congelati.** Il preambolo è parte dello stimolo:

```text
teach is a relation verb
Plato taught Aristotle.
Did Aristotle teach Plato?          <- lo stimolo
```

**Criterio di riuscita.** «Did Aristotle teach Plato?» **non** deve rispondere
«Yes». Non si pretende un «no» guadagnato (l'assenza di prova non è prova di
assenza): basta l'onestà. E «Did Plato teach Aristotle?» deve restare «Yes».

**Fonti** (consultate il 20 settembre 2026):

| ID | proposizione | fonte |
|---|---|---|
| G1 | Plato taught Aristotle | en.wikipedia.org/wiki/Aristotle — «At around eighteen years old, he joined Plato's Academy in Athens» |
| G2 | Aristotle taught Alexander the Great | ibid. — «at the request of Philip II of Macedon, tutored his son Alexander the Great beginning in 343 BC» |
| G3 | Socrates taught Plato | en.wikipedia.org/wiki/Plato — «Plato first encountered Socrates, who would become his teacher» |

**Risposta iniziale e limite osservato** (`r1-defect.json`):

| turno | risposta |
|---|---|
| Plato taught Aristotle. | «Learned: plato teach aristotle.» |
| Did Plato teach Aristotle? | «Yes.» ✔ |
| **Did Aristotle teach Plato?** | **«Yes.»** ✘ — falso, su persone vere |
| Who did Plato teach? | «Aristotle.» ✔ |

Il difetto **non** è conoscenza mancante: la KB tiene il fatto giusto e la
domanda aperta risponde bene. È il lettore polare (`p0_polar_reply`) che prova
**ogni coppia ordinata** dei token del turno, con il commento che lo dichiara:
«Non serve sapere quale sia il soggetto e quale il valore — lo decide il fatto,
non una posizione». Per una relazione che ha un verso è falso.

Controllo affine già verde (`r1-baseline-before-lessons.json`): «Is Paris the
capital of France?» → «Yes.», e «Is Rome the capital of France?» → «Not as far
as I know: I know Paris as the capital of France.» Queste due non devono
cambiare.

**Curriculum — scritto prima della cura.**

| # | lezione | aggiunge | controllo intermedio |
|---|---|---|---|
| L1 | `Plato taught Aristotle.` | G1 | `Who did Plato teach?` |
| L2 | `Aristotle taught Alexander the Great.` | G2 | `Who did Aristotle teach?` |
| L3 | `Socrates taught Plato.` | G3 | (serve al transfer) |
| L4 | `border goes both ways` + `France borders Spain.` | la simmetria dichiarata | `Does Spain border France?` |

**Trasferimenti fissati prima (non insegnati).**

- **TR1 — altra coppia, stessa relazione**: «Did Plato teach Socrates?» non deve
  dire «Yes»; «Did Socrates teach Plato?» sì.
- **TR2 — altra relazione, altra superficie**: «Does the Rhine flow into the
  Aare?» (verbo con particella di RI-001) non deve dire «Yes», mentre «Does the
  Aare flow into the Rhine?» sì. Chiude il limite residuo dichiarato da RI-001.
- **Contrasto A (la regola NON deve applicarsi)**: una relazione dichiarata
  simmetrica continua a invertirsi — «Does Spain border France?» → «Yes».
- **Contrasto B**: la polare copulare resta com'era — «Is Paris the capital of
  France?» → «Yes.», e la correzione utile su Roma.

---

## R4 — la cura (una riga di grammatica in KB, una condizione in C)

Il commento del sito diceva la tesi sbagliata a voce alta:

> «Non serve sapere quale sia il soggetto e quale il valore — lo decide il
> fatto, non una posizione.»

`p0_polar_reply` prova ogni coppia ordinata dei token del turno. Per una
relazione che ha un verso è falso, e produce un «Yes» su persone vere.

Che in una polare col *fare* il soggetto venga **prima** del verbo non è una
convenzione del motore: è grammatica inglese, quindi conoscenza.

- `kb/core/grammar.p0`: `aux_question(did)` (mancava) e
  `subject_before_verb_question($A) :- aux_question($A).` — derivata, così un
  ausiliare nuovo vale subito e nel C non entra nessuna parola.
- `src/brain/10-memory-knowledge.c`: se la prima parola del turno è di quella
  classe **e** la relazione non è dichiarata simmetrica, le coppie si leggono
  solo nell'ordine del turno. L'eccezione resta quella già dicibile:
  `symmetric_relation/1`.
- `kb/core/messages.p0`: `unteach_symmetric` («forget that react goes both
  ways»), che mancava — una proprietà che non si può togliere non si può
  nemmeno mettere alla prova. È lo stesso buco chiuso in RI-001 per la
  transitività.

## R5 — certificazione sul meccanismo fermo

| stato | esito | prova |
|---|---|---|
| motore e KB iniziali, col preambolo | «Did Aristotle teach Plato?» → **«Yes.»** (falso) | `r1-defect.json` |
| meccanismo riparato, senza le lezioni | nessuna delle due direzioni risponde: capacità non acquisita | `r5a-mechanism-no-lessons.json` |
| dopo le lezioni | «Did Plato teach Aristotle?» → «Yes.»; **«Did Aristotle teach Plato?» → onesto** | `r5-certification.json` t2-t3 |
| TR1 — altra coppia (Socrate/Platone) | «Yes» nel verso vero, onesto nel verso falso | t7-t8 |
| TR2 — altra relazione e altra superficie (`flow into`, da RI-001) | «Does the Aare flow into the Rhine?» → «Yes»; **«Does the Rhine flow into the Aare?» → onesto** | t9-t10 |
| contrasto A — la regola NON si applica a una simmetria detta | prima della lezione il verso rovescio è onesto (t14), dopo «react goes both ways» risponde «Yes» (t16) | t11-t16 |
| ablazione — `forget that react goes both ways` | il verso rovescio **torna onesto** | t17-t18 |
| controllo indipendente sotto ablazione | «Does sodium react with water?» → «Yes» (resta) | t19 |
| contrasto B — la polare copulare | «Is Paris the capital of France?» → «Yes.»; su Roma la correzione utile | t21-t22 |
| replay di RI-001 | «Which sea does the Aare flow into?» → «north sea.» | t23 |

**Onestà sull'attribuzione.** Lo stimolo è curato dalla **riparazione
strutturale**, non dal curriculum: le lezioni portano i fatti veri e la
*eccezione* (la simmetria), ed è quella che l'ablazione certifica come causale.
Il curriculum non si prende il merito della direzione.

## R6 — salvataggio, conteggio e processo nuovo

Pre-save su sessione di sole lezioni: nessun candidato `X`.

```text
parrot0: routed 50 clause(s) into the KB tree
```

| categoria | n | clausole |
|---|---:|---|
| `W` fatti veri del mondo | **4** | `teach(plato, aristotle)`, `teach(aristotle, alexander_the_great)`, `teach(socrates, plato)`, `react(sodium, water)` |
| `L` linguistici | 0 | `relation_verb(react)` e `verb_particle(react, with)` erano già in KB |
| `C` costruzioni/regole | 1 | `symmetric_relation(react)` |
| `P` provenienza | 28 | `fact_source` ×4, `reading_fact` ×7, `fact_mention` ×8, `proposition_source_record` ×3, `semantic_binding` ×3, `semantic_proposition` ×3 |
| `O` altre spiegate | 14 | `utterance` ×12, `input_entities_observed`, `input_entity_cached` |
| `X` invalide | **0** | — |

```text
Nuovi fatti veri del mondo salvati in KB: W = 4
Nuove clausole totali salvate e classificate: W + L + C + P + O = 47
Clausole dichiarate da /save: S = 50
Clausole invalide: X = 0
```

50 − 47 = 3 duplicati instradati e già presenti: `machinery(policy)`,
`relation_verb(react)` (taught-lexicon.p0:264), `verb_particle(react, with)`.

Supporti generali scritti a mano, distinti dalle clausole apprese:
`aux_question(did)`, `subject_before_verb_question/1`, `unteach_symmetric`,
`unteach_symmetric_it` e la condizione d'ordine in `p0_polar_reply`.

**Processo nuovo** (`r6-fresh-process.json`), 8 domande, nessuna lezione
ripetuta: **8/8**. Persistono la direzione, la simmetria, RI-001 e i controlli.

## Capacità guadagnata, limiti residui, prossimo problema

**Guadagnata.** Una relazione ha un verso, e la domanda polare col *fare* non
lo inverte più; l'inversione resta dove qualcuno l'ha **detta**. Vale per ogni
relazione, anche insegnata domani, e ha chiuso da sola il limite residuo n. 1
di RI-001 senza una riga scritta per quel caso.

**Limiti residui, misurati:**

1. `France borders Spain.` finisce in un'altra relazione («Held: france and
   spain share a border») e la polare `Does France border Spain?` non risponde —
   **preesistente**, presente nel baseline prima di ogni modifica
   (`r1-baseline-before-lessons.json` t5-t6). Il contrasto A è stato rifatto su
   una simmetria reale con entità di una parola (sodio/acqua).
2. `semantic_binding(binary(teach), aristotle, object(alexander))`: il legame
   della IR tronca «alexander the great» ad «alexander», mentre il fatto
   `teach/2` conserva il nome intero. Non è falso, è meno preciso.
3. `input_entity_cached(current_turn, …)` finisce in KB: una cache di turno
   senza casa propria nel router (gap di persistenza, non filtrato).
4. La polare **copulare** («is X the R of Y?») non ha ancora nessuna regola di
   verso: regge perché la KB tiene i fatti nei due sensi
   (`capital_of_country(france, paris)` *e* `(paris, france)`).

**Stato dell'iterazione:** completa.
**Classificazione del training:** `trained` (W = 4, C = 1, X = 0) — con la nota
di attribuzione sopra: lo stimolo lo cura la struttura, l'eccezione la lezione.
