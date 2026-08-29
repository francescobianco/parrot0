# SC2-C — equivalenza, residuo tipato e prova pronunciata

Data: 2026-08-29

Protocollo: [`LEARN_PROTOCOL.md`](../../../LEARN_PROTOCOL.md) ·
Predecessore: [SC2-B](2026-08-29-supercomprensione-sc2b.md)

## Parametri

| Parametro | Valore |
|---|---|
| `DOMINIO` | forme diverse della stessa proposizione riportata, e confini della lettura |
| `OBIETTIVO` | (1) attivo e passivo convergono su un frame; (2) una lettura fallita dice **contro che cosa** si è fermata; (3) la prova si pronuncia |
| `FONTI` | NASA NTRS (DART), WHO (vaiolo), PRL 116 061102 (LIGO), GMD 13/1959/2020, PMC5118066 |
| `TARGET_WORLD_FACTS` | 0 |
| `TARGET_CAPABILITIES` | 3 |
| `STOP_CONDITION` | un frame passivo con i ruoli invertiti, o un `Yes` costruito su una lettura non giustificata |

Boot: `36477 facts, 2463 rules`. Stato: **meta-capability-only**.

## I tre muri della baseline

Processo fresco, prima di ogni modifica:

```text
> read: … The data show that DART shortened the orbital period.
< … 1 proposition(s) normalized in quarantine.
> read: … The data show that the orbital period was shortened by DART.
< … learned 0 world fact(s), skipped 1.           ← nessuna normalizzazione
> read: … The authors hypothesize that a kinetic impactor can shorten an asteroid orbit.
< … learned 0 world fact(s), skipped 1.           ← gap generico `no_reading`
> how do you know?
< I haven't answered a knowledge-based question yet, so I don't have a proof to share.
```

Tre perdite indipendenti, tutte invisibili nel sommario: **la stessa
proposizione produceva un frame e un buco a seconda dello stile dell'autore**;
un modale faceva fallire la lettura senza dire di essere un modale; e una
risposta giustificata non sapeva pronunciare la propria giustificazione.

## 1. Il passivo non è una seconda lezione

Il rimedio non è insegnare una costruzione per verbo. Si dichiara che una
relazione **ha** una forma passiva e lo schema lo costruisce una regola — la
stessa mossa del gen429 sul verbo di relazione, un gradino più su.

Il passivo si forma sul **participio**: generare «was produces by» sporcherebbe
lo spazio degli schemi senza mai combaciare. Quali forme siano participi è
morfologia, e la morfologia è conoscenza — la regola generale è il suffisso, le
eccezioni sono membri insegnabili:

```prolog
past_participle($V) :- relation_verb($V), chars($V, $L), ends_ed($L).
past_participle($V) :- irregular_participle($V).

extract_frame($Pat, $Pred) :-
    past_participle($Pred), passive_prefix($Prefix), passive_suffix($Suffix),
    concat_atoms($Prefix, $Pred, $Mid), concat_atoms($Mid, $Suffix, $Pat).
```

### Il prerequisito nascosto: i ruoli uscivano nell'ordine sbagliato

Il passivo non poteva funzionare finché il legatore di schemi leggeva la lettera
dopo `@` e la buttava via. Per «@S governs @O» posizione e ruolo coincidono e
nessuno se ne accorgeva; per «@O was shortened by @S» no, e il fatto usciva
rovesciato. Era lo stesso bug che teneva rosso da tempo
`assisted_construction.p0t` sull'inversione dei ruoli di una costruzione
insegnata, e che faceva produrre silenziosamente fatti scambiati alla
costruzione spedita `construction_frame("@O is home to @S", …)`.

`frame_role_order/2` è ora un fatto (`s`=1, `o`=2, `t`=3) e la fase pura di
SC2-B riordina i ruoli prima di esporli. Uno schema che non dichiara i propri
ruoli conserva la posizione, quindi la modifica è additiva; due ruoli con lo
stesso rango non sono un ordine e la posizione viene conservata invece di
sceglierne uno. `assisted_construction.p0t`: **60/6 → 65/1**.

Il posto giusto era la fase pura: è lì che gli slot escono, ed è per questo che
il bug si è potuto chiudere in un punto solo.

### Misura

```text
> read: … The measurements indicate that DART slowed the orbital period.
!query claim_frame(…, slowed, dart, orbital_period)                     ✓
> was it observed that the orbital period was slowed by DART?
< Yes - that claim is reported as observed (from …).

> read: … The measurements indicate that the orbital period was slowed by DART.
!query claim_frame(…, slowed, dart, orbital_period)                     ✓
!query! claim_frame(…, slowed, orbital_period, dart)                    ✓ (non invertito)
> was it observed that DART slowed the orbital period?
< Yes - that claim is reported as observed (from …).
!query! slowed(dart, orbital_period)                                    ✓ (nessun commit)
```

**Transfer a costo zero.** In un processo nuovo, senza una sola lezione, i verbi
persistiti da SC2-B leggono la propria forma passiva:

```text
> read: https://www.who.int/…/smallpox The evidence shows that smallpox was eradicated by vaccination.
< … 1 proposition(s) normalized in quarantine
> was it observed that vaccination eradicated smallpox?      -> Yes
> read: …PhysRevLett.116.061102 The data show that gravitational waves were detected by LIGO.
> was it observed that LIGO detected gravitational waves?    -> Yes
> /debug eradicated                                          -> nessuna clausola
```

Questo è il segnale che conta: una lezione data ieri (`eradicated is a relation
verb`) ha acquisito oggi una capacità nuova senza essere ripetuta.

## 2. Il residuo dice contro che cosa si è fermato

`no_reading` è vero e inutile. Le superfici che spiegano l'arresto stanno in KB e
passano dallo stesso scorer universale delle cue epistemiche; il match è a
**parola intera**, perché «can» non deve accendersi dentro «scan».

```prolog
normalization_gap_evidence(modal_operator, keyword(can)).
normalization_gap_evidence(negated_reading, keyword(not)).
normalization_gap_evidence(coordinated_reading, keyword(and)).
normalization_gap_evidence(comparative_reading, keyword(than)).
```

| Frase reale | Residuo |
|---|---|
| «a kinetic impactor **can** shorten an asteroid orbit» | `modal_operator` |
| «randomization does **not** remove all bias» | `negated_reading` |
| «resolution matters **more than** parameterization» | `comparative_reading` |
| «DART slowed the period **and** raised the plume» | `partial_reading` (SC2-B) |
| «DART slowed the orbital period **of** Dimorphos» | `partial_reading` (SC2-B) |

Se le evidenze pareggiano il residuo resta `no_reading`: un'ambiguità non si
risolve scegliendo. Una classe nuova di residuo costa dei fatti; il lettore non
cambia.

Questo è il primo passo concreto verso **D14/SC18**: la somma dei residui di un
corpus è l'elenco ordinabile di ciò che parrot0 non sa ancora leggere, cioè il
curriculum che oggi scriviamo a mano.

## 3. La prova si pronuncia

`claim_proof/2` si interrogava e non si diceva. Ora la risposta su una claim
registra la propria traccia nel registro che «come lo sai?» consuma già dal
gen76, con le coordinate prese una per una dalla KB e la **frase** in un
`response_template`:

```text
> what did the measurements indicate?
< The source reports it as observed: DART slowed the orbital period (from …).
> how do you know?
< Because I read it in document_0246e706600fd069_5d809f6f7baf11bd_unit_0 of
  https://ntrs.nasa.gov/citations/20230015804, at range(32, 30), and
  «the measurements indicate that» marks it as observed.
```

Documento, unità, span di byte, cue viva e status. Ritraendo la cue la traccia
non si può più costruire, perché `claim_live_cue/2` passa da `apply/2`.

## Lezioni naturali e persistenza

Le lezioni di questo ciclo sono i **participi irregolari**: la classe che rende
il passivo disponibile per i verbi che non finiscono in `-ed`.

```text
shown is an irregular participle      -> Learned: irregular_participle(shown).
found / given / seen / written        -> idem
known is an irregular participle      -> MURO, registrato come gap
```

Cinque su sei. La sesta è un fallimento reale e conservato: `/save` ha scritto
`machinery_gap("known is an irregular participle")` con tre `gap_opaque` e un
`gap_kind(reachability)`. Non è rumore da ripulire — è la diagnosi di
un'interferenza fra `known` e un consumer esistente, e resta nel repository.

Composizione verificata: `found is an irregular participle` +
`found is a relation verb` → «a merger signal was found by the pipeline» viene
letto, la domanda attiva «was it observed that the pipeline found a merger
signal?» risponde `Yes`, e la prova nomina la fonte PRL.

Notevole anche il muro **prima** della seconda lezione: parrot0 ha declinato la
domanda attiva e ha proposto da sé la lezione mancante — «if found is something
one thing does to another, say *found is a relation verb*». È D20 in embrione.

### Conteggio

```text
parrot0: routed 33 clause(s) into the KB tree
```

| Categoria | Conteggio | Contenuto |
|---|---:|---|
| `W` | 0 | — |
| `L` | 5 | `irregular_participle/1`: shown, found, given, seen, written |
| `C` | 0 | — |
| `P` | 5 | `fact_source(...)` |
| `O` | 23 | 5 `reading_fact`, 12 `utterance`, 6 clausole del gap-registry |
| `X` | **0** | — |

```text
Nuovi fatti veri del mondo salvati in KB: 0
Nuove clausole totali salvate e classificate: 33
Clausole dichiarate da /save: 33
Clausole invalide: 0
```

**Routing.** `irregular_participle` non aveva una casa e il router lo ha
depositato in `kb/learning/learned.p0`, il fallback. Le cinque righe sono state
spostate a mano in `kb/learning/taught-lexicon.p0` — che è precisamente il file
della *classe grammaticale di una parola imparata parlando* — e `learned.p0` è
tornato vuoto, che è il suo stato giusto. È una decisione di persistenza, non di
conoscenza: la conoscenza è arrivata parlando, e dal prossimo `/save` la classe
si instrada da sola.

## Verifica software

- `document_claims.p0t`: **156 passed** (50 SC2-A + 74 SC2-B + 32 SC2-C).
- `assisted_construction.p0t`: 60/6 → **65/1**. Il rosso residuo è un'attesa
  stantia del test (`"denotes"` contro `«denotes»` del template
  `teach_form_ack`), non un comportamento sbagliato: annotato, non toccato.
- Verdi: `document_rhetoric` 33, `taught_lexicon` 35, `retract` 17, `mention` 24,
  `taught_segment_role` 21, `booklearn` 4, `howknow` 4, `motorize_class` 24.
- Unico `make soft-test` del ciclo: 55 verdi, 1 rosso — lo stesso preesistente
  di `frontier_chat_audit.it.p0t` riga 97.
- Rossi preesistenti verificati **identici** sul binario precedente alla
  modifica, quindi non introdotti qui: `repair` (l'oracolo non compila in questo
  ambiente), `check_sort`, `forget_move`, `greet`, `games`,
  `faceted_enumeration`, `foundational_concepts`, `gap_dialogue`,
  `name_is_knowledge`, `reactions_are_knowledge`, `gap_is_a_fact`, `gap_anchor`.

## Metriche

| Metrica | Risultato |
|---|---:|
| LessonYield | 5/6 (la sesta è un gap classificato) |
| Convergenza attivo/passivo | 4/4 (testo→domanda e domanda→testo, in entrambi i versi) |
| Inversione di ruolo su passivo | 0 |
| Transfer passivo a costo zero su verbi già persistiti | 2/2 |
| Residui tipati corretti | 5/5 (modale, negazione, comparazione, 2× parziale) |
| ProofSpoken | 2/2 |
| `WorldCommitLeak` | 0 |
| `FalseUnderstandingRate` | 0 |

## Limiti che restano

1. Il modale è **tipizzato**, non compreso: «can shorten» resta un gap, non
   diventa una claim di possibilità con il proprio status. È il prossimo gate.
2. L'equivalenza copre attivo/passivo. Nominalizzazione («the shortening of the
   period by DART»), diatesi media e parafrasi lessicale restano fuori.
3. `past_participle` usa il suffisso `-ed` più una classe di eccezioni. Non
   distingue il participio dal passato semplice: «slowed» è entrambi, e
   nessuna delle due letture lo sa.
4. La domanda di contenuto resta limitata al documento corrente (SC2-B).
5. La traccia della prova nomina l'ID interno dell'unità. È corretto e
   illeggibile: la verbalizzazione di un'unità documentale è un debito aperto.
