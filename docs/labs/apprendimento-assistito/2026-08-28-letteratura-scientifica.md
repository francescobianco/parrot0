# 2026-08-28 — Letteratura scientifica: le forme della prosa e i suoi strumenti

Sessione condotta secondo [`LEARN_PROTOCOL.md`](../../../LEARN_PROTOCOL.md).

**Stato finale: `partial`** — sei fatti veri acquisiti e persistiti, e il muro
che conta trovato e *nominato da parrot0 stesso*: la costruzione a tre ruoli.

## 1. Parametri

| Parametro | Valore |
|---|---|
| `DOMINIO` | forme della letteratura scientifica e strumenti logico-retorici della sua prosa |
| `OBIETTIVO` | parrot0 sa dire che cosa fa uno strumento della prosa scientifica, e regge la forma articolata (più di due ruoli) |
| `FONTI` | Swales, *Genre Analysis* (1990); Hyland, *Hedging in Scientific Research Articles* (1998); Sollaci & Pereira, JMLA 92(3) (2004); Lakoff & Johnson, *Metaphors We Live By* (1980) |
| `TARGET_WORLD_FACTS` | 3 |
| `TARGET_CAPABILITIES` | 1 — la costruzione a tre ruoli |
| `STOP_CONDITION` | primo misclaim non spiegato |

Preflight pulito, boot senza `PARSE ERROR`. **`B0` = 34976, `R0` = 2391.**

## 2. Baseline (§6.1)

| turno | esito |
|---|---|
| «what structure does a research article use?» | `IRRELEVANT` — «paste it here and I'll work on it»: il lettore rivendica la parola *article* |
| «what is IMRAD?» | `WALL` |
| «what is hedging in scientific writing?» | `IRRELEVANT` — risponde sul metodo scientifico |
| «what does a counterexample do?» | `WALL` |
| «what is a thought experiment?» | `WALL` |
| «what does a metaphor do?» | `WALL` |
| «what is abduction?» | `KNOWN_CORRECT` |

Due `IRRELEVANT` in baseline, entrambi con causa leggibile (una parola del
dominio è rivendicata da un modulo che parla d'altro). Nessun `WRONG`.

## 3. Che cosa è stato acquisito — `W = 6`

Tutti detti in lingua naturale, tutti veri, tutti interrogabili:

| fatto | fonte |
|---|---|
| un controesempio confuta un'affermazione universale | logica standard |
| l'hedging indebolisce un'asserzione | Hyland 1998 |
| una review article passa in rassegna il lavoro pubblicato | Swales 1990 |
| un esperimento mentale mette alla prova una teoria | uso standard |
| IMRaD organizza un articolo di ricerca | Sollaci & Pereira 2004 |
| un preprint precede la revisione paritaria | uso standard |

`L = 7`: i verbi relazionali aperti parlando — `refutes`, `weakens`, `surveys`,
`tests`, `organizes`, `precedes`, `maps`.

## 4. Il muro che conta: la forma articolata (M3/M4)

È il cuore della richiesta — «le forme più articolate, metafore e altri
strumenti logici» — ed è dove parrot0 si ferma.

La proposizione vera da insegnare era, in Lakoff & Johnson: *una metafora
proietta un dominio sorgente su un dominio bersaglio*. **Tre ruoli**: lo
strumento, ciò da cui parte, ciò che spiega.

```
> a metaphor maps a source domain onto a target domain
parrot0: Scartato: maps(metaphor, source_domain_onto_a_target_domain)
         non e' fatto di concetti.
```

La frase collassa in una relazione binaria il cui oggetto inghiotte tutto il
resto — e **il cancello la respinge**, che è il comportamento giusto: mantra #7,
meglio un rifiuto che un fatto inventato. Nessun misclaim.

Tentato allora l'atto didattico di M4, nominando i ruoli a parole:

```
> in «x maps y onto z», x is the device, y is what it starts from and z is
  what it explains
parrot0: Scartato: maps(in_x, y_onto_z_x_is_the_device_y_is_what_it_starts)
         non e' fatto di concetti.
> x maps y onto z means x maps y
parrot0: I found the teaching pivot, but I cannot align exactly two shared
         variables on both sides. I have not learned the construction.
```

L'ultima riga è il risultato della sessione. **parrot0 tipizza da solo la
propria lacuna**, e la tipizza correttamente: l'allineatore di costruzioni è
fermo a *esattamente due* variabili condivise. È
[`docs/plans/apprendimento-assistito.md`](../../plans/apprendimento-assistito.md)
§6.2 **M3** — «*Manca: arità diversa da due*» — non dedotto da noi ma
pronunciato dal sistema, che è la condizione che M13 chiede al gap perché il
teacher automatico sia avviabile.

Il fatto ternario **non è stato scritto a mano** per aggirare il muro. La sua
assenza da `kb/core/facts/scientific-discourse.p0` è documentata nel file.

## 5. Altri gap tipati

**L1 — soggetto multiparola in posizione di domanda.** L'asserzione unisce
(«a review article surveys published work» → `surveys(review_article,
published_work)`), la domanda no:

```
> the review article surveys what?
parrot0: Hmm, I don't know about review yet. […] if article is something one
         thing does to another, say «article is a relation verb»
> review_article surveys what?
parrot0: Published work.
```

Asimmetria solo sul soggetto: l'oggetto multiparola è già gestito e viene reso
correttamente («Universal claim.», «Peer review.»).

**L2 — collisione di verbo con perdita del template.** `explains` è già una
superficie di `answer_frame`, e insegnarlo come verbo relazionale fa stampare il
template grezzo:

```
> explains is a relation verb
parrot0: explains is {subject} explains {object}..
```

Le graffe non sostituite sono un messaggio rotto che arriva all'interlocutore —
mantra #16. Il verbo è stato abbandonato e il fatto sulla metafora non è stato
forzato per altra via.

**L3 — due `IRRELEVANT` in baseline** (§2), da collisione di modulo sulle parole
*article* e *hedging*. Stessa famiglia di G1/G2 del report elettronica:
`answerframe` e i lettori rivendicano prima di aver capito.

## 6. `/save` e conteggio

```
parrot0: routed 136 clause(s) into the KB tree
```

| Categoria | Conteggio |
|---|---:|
| fatti veri del mondo `W` | **6** |
| fatti linguistici `L` | 7 |
| costruzioni/regole `C` | 0 |
| provenienza/genealogia/gap `P` | 33 |
| log dei turni `O` | 90 |
| false/non verificate `X` | **0** |

Nessuna clausola di scratch è finita nell'albero: `turn_scratch/1` fa il suo
lavoro dal commit precedente.

## 7. Sparpagliamento

Sei predicati non avevano parenti. `precedes/2` ne ha trovati in
`kb/core/world-facts.p0` e ci è andato — è una relazione temporale generica, e
la sua famiglia è lì. Gli altri cinque sono una specie nuova, e hanno avuto la
casa che mancava:

```
kb/core/facts/scientific-discourse.p0
```

Gli altri quattro flussi sono andati da soli nelle case aperte dal commit
precedente: `taught-lexicon.p0` per i sette verbi, `fact-provenance.p0`,
`gap-registry.p0`, `transcripts.p0`. **`kb/learning/learned.p0` è tornato a zero
clausole senza interventi a mano** — cioè le case reggono.

## 8. Verifica in processo nuovo

**`B1` = 35112**, nessun `PARSE ERROR`. Senza ripetere alcuna lezione:

| domanda | risposta |
|---|---|
| the counterexample refutes what? | Universal claim. |
| hedging weakens what? | Claim. |
| thought_experiment tests what? | Theory. |
| imrad organizes what? | Research article. |
| the preprint precedes what? | Peer review. |

**`FreshProcessRecall` = 5/5 = 100%.**

## 9. Metriche

```text
LessonYield            = 6/8   (metafora ternaria e `explains` non promossi)
Transfer@3             = 3/3   sulla forma binaria
FreshProcessRecall     = 5/5 = 100%
FalseUnderstandingRate = 0     (nessuna affermazione falsa; tre IRRELEVANT tipizzati)
WorldKnowledgeGain     = 6
TotalPersistedClauses  = 136
```

`trained` sulla forma binaria; **`partial` sull'obiettivo dichiarato**, perché
`TARGET_CAPABILITIES` era la costruzione a tre ruoli e quella non è stata
acquisita. Il prossimo lavoro è M3: allineare più di due variabili condivise,
che è ciò che serve per dire una metafora — e per la maggior parte degli
strumenti logici articolati che questo dominio usa.
