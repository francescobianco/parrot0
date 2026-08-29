# SC2-A — claim riportate, fonte e forza epistemica

Data: 2026-08-29

Protocollo: [`LEARN_PROTOCOL.md`](../../../LEARN_PROTOCOL.md)

Piani: [`apprendimento-assistito.md`](../../plans/apprendimento-assistito.md),
[`frontier-kb-natural-dialogue.md`](../../plans/frontier-kb-natural-dialogue.md)

## Parametri

| Parametro | Valore |
|---|---|
| `DOMINIO` | lettura epistemica di prosa scientifica breve |
| `OBIETTIVO` | separare superficie, claim attribuita, status epistemico, contesto, commitment e fonte |
| `BUDGET` | quattro lezioni naturali, replay, `Transfer@3`, parafrasi, contrasto, ablation, reteach, save e fresh process |
| `FONTI` | NASA Technical Reports Server, PubMed Central, GMD/Copernicus |
| `TARGET_WORLD_FACTS` | 0; il ciclo mira a una meta-capacita' |
| `TARGET_CAPABILITIES` | classi multi-parola di marker epistemici consumate dal Document IR senza rebuild |
| `STOP_CONDITION` | promozione di un claim riportato a fatto del mondo, perdita di fonte/span o falso claim salvato |

Boot prima della promozione: `36345 facts, 2433 rules`, profilo
`kb/profiles/agi.p0`, nessun parse error. Dopo il save, un processo nuovo parte
da `36365 facts, 2433 rules`.

Stato dichiarato: **meta-capability-only; SC2-A chiusa, SC2-B aperta**. Questo
ciclo non autorizza ancora a dire che parrot0 abbia normalizzato semanticamente
le proposizioni o sappia rispondere in lingua naturale a domande sul loro
contenuto.

## Fonti e uso sperimentale

| ID | Evidenza vera usata | Fonte | Uso |
|---|---|---|---|
| F1 | DART ha validato la tecnica dell'impatto cinetico e ha accorciato il periodo orbitale di Dimorphos | [NASA, *Double Asteroid Redirection Test (DART) Mission: Impact Effects on the Orbit and Shape of Asteroid Dimorphos*](https://ntrs.nasa.gov/citations/20230015804) | transfer esperimentale: ipotesi contro osservazione riportata |
| F2 | associazioni osservazionali e stime randomizzate possono divergere; il disegno osservazionale ha limiti causali | [PMC, *Observational Research Rigor*](https://pmc.ncbi.nlm.nih.gov/articles/PMC5118066/) | transfer osservazionale e contrasto fra associazione e causa |
| F3 | nello studio simulativo su Salmonella, gli esiti erano fortemente correlati alla carica iniziale e l'ipotesi legava carica alta a shock settico/morte | [PMC, *An Agent-Based Model of Salmonella Infection*](https://pmc.ncbi.nlm.nih.gov/articles/PMC4996536/) | baseline lunga, replay e transfer di simulazione |
| F4 | lo studio WRF ipotizzava maggiore skill con alta risoluzione e riportava miglioramenti della previsione delle precipitazioni | [GMD, *High-resolution convection-permitting simulations with WRF*](https://gmd.copernicus.org/articles/13/1959/2020/) | transfer di simulazione e parafrasi |

I periodi passati a parrot0 sono parafrasi brevi, dichiarative e conservatrici
di queste fonti. Le quattro lezioni salvate sono fatti linguistici veri sulle
locuzioni inglesi; nessun risultato scientifico e nessuna frase diagnostica e'
stata promossa come fatto del mondo.

## Baseline naturale

Prima della lezione:

```text
you> read: The authors hypothesize that higher initial Salmonella counts
     increase the likelihood of septic shock and death. The simulations show
     that outcomes were highly correlated with initial Salmonella counts.
parrot0> Learned 0 fact(s), skipped 2.
```

Le domande naturali successive non raggiungevano il testo:

```text
What did the authors hypothesize?  -> muro
Was the likelihood observed?       -> muro
What did the simulations show?     -> muro
```

Nel dump sopravviveva soltanto il secondo periodo. Il primo conteneva sedici
token: la materializzazione SC1 attraversava una lista ricorsiva e superava il
limite pratico del solver. Questo e' un fallimento importante: una frase lunga
non puo' diventare meno osservabile proprio quando la prosa diventa scientifica.

## Sonde diagnostiche abortite

La prima lezione multi-parola:

```text
"the authors hypothesize that" is an author hypothesis marker.
```

fu compressa nel falso fatto
`author_hypothesis_marker(authors_hypothesize)`. Una correzione parziale rese
possibile l'ingresso, ma il retract eliminava il determinante quotato e produceva
`hypothesis_report_marker(forget)`. Le sessioni sono state chiuse con `/quit`,
mai con `/save`.

Queste sonde valgono `X=1` ciascuna come diagnostica storica. Il run finale ha
`X=0`: la metrica finale non cancella i fallimenti che hanno imposto la
simmetria learn/query/retract.

## Incremento implementato

### 1. Documenti source-addressed

Quando il payload contiene una coordinata meccanica `scheme://...`, il reader
la separa dal testo e costruisce:

```text
document_<hash-source>_<hash-content>
document_source(Document, URI)
document_fingerprint(Document, fnv1a64_<hash-content>)
```

FNV-1a e' soltanto meccanica deterministica sui byte. Non canonizza la fonte,
non decide l'autorevolezza e non interpreta il contenuto. Lo stesso URI e gli
stessi byte producono lo stesso handle fra processi; URI o contenuto diversi
restano distinti. Canonicalizzazione di URI, versione editoriale e collisioni
sono gate ancora aperti.

### 2. Unita' lunghe senza ricorsione di lista

`document_unit_observe/4` conserva unita', superficie e range. Il C enumera gli
ID dei nodi gia' pubblicati e invoca `document_unit_node_observe/3`; la KB
seleziona soltanto i nodi token e li copia. Il C non conosce parole, categorie o
status, ma una frase non dipende piu' dalla profondita' di una lista Prolog.

Il ratchet verifica esplicitamente il token 16, `death`, nella frase di
baseline. Entrambe le unita' sopravvivono anche quando nessun marker e' noto.

### 3. Menzioni quotate multi-parola simmetriche

Il parser metalinguistico raccoglie l'intera superficie fra virgolette e la
conserva come termine quotato. La stessa lettura pura serve learn, query e
retract. Un determinante dentro le virgolette non viene trattato come stopword:

```text
"the investigators predict that" is a hypothesis report marker. -> Learned
Is "the investigators predict that" ...?                         -> Yes
forget that "the investigators predict that" is ...              -> removed
Is "the investigators predict that" ...?                         -> No
```

### 4. Claim di superficie e viste epistemiche vive

`kb/core/document-claims.p0` introduce una classe aperta:

```prolog
claim_marker_class(
    hypothesis_report_marker,
    reading(hypothesized, reported_belief),
    attribution(document_authors),
    extent(remainder)).
```

Le classi `observation_report_marker` e `simulation_result_marker` hanno status
e attribuzioni diversi. `claim_status_evidence/2` usa `apply/2`: una superficie
nuova insegnata come membro viene consumata subito dal reader esistente.

Il producer C usa lo scorer universale, copia il range esatto del marker e il
remainder e consegna alla KB un record opaco. Non contiene letterali come
`hypothesize`, `show`, `authors`, `data`, `observed` o `simulated`.

La KB materializza soltanto lo strato evidenziale debole:

```prolog
document_claim(Document, Claim, proposition(surface(Text))).
claim_marker_observation(Claim, Class, marker(Cue, Range)).
claim_attributed_to(Claim, Agent).
claim_source_record(Claim, source(Document, Unit), provenance(Range)).
```

`claim_status/2`, `claim_context/2`, `holds_in(context(...), ...)` e
`claim_commitment/2` sono viste derivate dalla membership **viva** della cue.
Il retract toglie quindi classificazione e commitment attribuito anche da un
documento gia' letto, preservando superficie, fonte, span e attribuzione.

Il contesto e' `reported_belief` e il commitment e' `attributed_only`. Non viene
asserito alcun `holds_in(world, proposition(surface(...)))`.

## Lezioni naturali e replay

Le quattro forme promosse sono state insegnate parlando:

```text
"the authors hypothesize that" is a hypothesis report marker.
"the data show that" is an observation report marker.
"the simulations show that" is a simulation result marker.
"the researchers hypothesize that" is a hypothesis report marker.
```

Il replay Salmonella e il replay DART hanno prodotto in ciascun caso:

```text
Mapped 2 document unit(s), 2 reported claim(s), and
2 live epistemic status(es); learned 0 world fact(s), skipped 2.
```

Per DART sono distinti:

- il claim `kinetic impact can alter an asteroid orbit`, attribuito agli autori
  e `hypothesized`;
- il claim sul periodo di Dimorphos, attribuito ai dati e `observed`;
- la fonte NASA e il fingerprint del contenuto;
- l'assenza del primo claim dal contesto `world`.

## Transfer, parafrasi, contrasto e composizione

| Prova | Esito |
|---|---|
| studio osservazionale PMC | 2 unita', 2 claim, `hypothesized` + `observed` |
| simulazione GMD | 2 unita', 2 claim, `hypothesized` + `simulated` |
| simulazione Salmonella | 2 unita', 2 claim, `hypothesized` + `simulated` |
| parafrasi `the researchers hypothesize that` | stessa lettura di ipotesi su GMD |
| contrasto `the authors demonstrate that` | nessuna falsa ipotesi; la sola cue nota produce 1 claim/status |
| composizione DART | due status opposti, stessa fonte, nessun commitment del mondo |

`Transfer@3=3/3`; parafrasi `1/1`; contrast precision `1/1`.

Il test persistente non riusa le forme appena salvate. Parte da tre locuzioni
held-out — `the investigators predict that`, `the measurements indicate that`,
`the model outputs show that` — e ripete baseline, lezione, transfer e
ablation. Questo impedisce alla crescita della KB di rendere verde il proprio
ratchet per memoria accidentale.

## Ablation e reteach

Dopo:

```text
forget that "the authors hypothesize that" is a hypothesis report marker
```

la query sulla classe risponde `No`; la forma parafrasata `the researchers
hypothesize that` resta `Yes`. Sul documento GMD gia' letto restano entrambe le
claim, ma soltanto lo status simulato e' vivo. Fonte, span e attribuzione del
claim ipotetico restano interrogabili; context e commitment dipendenti
scompaiono.

Rinsegnare la forma riattiva lo status e il commitment attribuito sul documento
gia' osservato, senza rileggerlo. `AblationFidelity=1/1`, reteach `1/1`.

## Persistenza pulita

E' stata salvata la sessione delle sole quattro lezioni; i documenti scientifici
sono stati usati in sessioni non promosse. `/save` ha dichiarato:

```text
routed 20 clause(s) into the KB tree
```

| Categoria | Conteggio | Contenuto |
|---|---:|---|
| `W` fatti veri sul mondo | 0 | — |
| `L` fatti linguistici | 4 | due marker di ipotesi, uno osservativo, uno simulativo |
| `C` costruzioni/procedure | 0 | — |
| `P` provenienza | 4 | `fact_source(...)` |
| `O` tracce di ordine superiore | 12 | quattro `reading_fact(...)`, otto `utterance(...)` |
| `X` invalide nel run promosso | **0** | — |

Totali richiesti dal protocollo:

```text
Nuovi fatti veri del mondo salvati in KB: 0
Nuove clausole totali salvate e classificate: 20
Clausole dichiarate da /save: 20
Clausole invalide: 0
```

`kb/learning/learned.p0` non e' cambiato: ogni classe ha una casa semantica in
`kb/core/document-claims.p0`. `B1-B0=20` e il numero di regole resta `2433`,
coerente con il diff e con `S=20`.

In un processo nuovo, tutte e quattro le query sulle locuzioni rispondono
`Yes`; la rilettura DART ricostruisce lo stesso ID
`document_0246e706600fd069_23b06a3c98bb9351`, la fonte, il fingerprint, due
claim e due status. `FreshProcessRecall=5/5`.

## Metriche

| Metrica | Risultato |
|---|---:|
| LessonYield promosso | 4/4 |
| Replay | pass |
| Transfer@3 | 3/3 |
| Paraphrase | 1/1 |
| ContrastPrecision | 1/1 |
| Composition fonte/status/contesto | 1/1 |
| AblationFidelity | 1/1 |
| Reteach | 1/1 |
| FreshProcessRecall | 5/5 |
| Status fidelity su quattro documenti a due claim | 8/8 |
| Source/span fidelity nel ratchet | 8/8 |
| Semantic proposition normalization | **0/8** |
| Natural claim-question coverage | **0/3** |
| FalseUnderstandingRate finale | 0 |

`proposition(surface(Text))` e' una quarantena strutturata, non una
normalizzazione logica. La claim coverage strutturale e' quindi 8/8, ma la
coverage di proposizioni semantiche resta 0/8. Tenere separate queste metriche
evita di scambiare un buon envelope epistemico per comprensione del contenuto.

## Verifica software

- `make build`: verde;
- `document_claims.p0t`: **50 passed**;
- `document_rhetoric.p0t`: **33 passed** con cue held-out;
- `mention.p0t`: **24 passed**;
- `retract.p0t`: **17 passed**;
- `taught_segment_role.p0t`: **21 passed**.

La crescita della KB ha fatto apparire in `retract.p0t` una coda lecita
`Induced: ...` dopo la regola esplicitamente insegnata. Il runner documentava
gia' che questi test devono usare `<^` per la testa stabile: il ratchet e' stato
allineato a quel contratto senza nascondere la coda o cambiare il comportamento.

Il solo `make soft-test` del ciclo e' stato eseguito una volta: **55 passati,
1 fallito**. E' lo stesso rosso preesistente di
`frontier_chat_audit.it.p0t`, riga 97:

```text
expected contains: I don't know about designation
got: I don't know much about your designation yet. Want me to look it up?
```

Non e' stato rilanciato.

## Stato e prossimo confine: SC2-B

SC2-A chiude l'envelope source-addressed e la separazione fra frase, claim,
status e commitment. SC2-B deve trasformare il remainder di superficie nello
**stesso frame semantico** usato dalle asserzioni e dalle query normali, senza
costruire un parser documentale parallelo.

Il gate minimo successivo e':

1. leggere «the authors hypothesize that X; the data show that Y»;
2. ottenere per X e Y proposizioni normalizzate, non soltanto stringhe;
3. rispondere naturalmente a «what did the authors hypothesize?», «was X
   observed?» e «what did the simulations show?»;
4. mostrare proof fino a documento, unita', span, cue e fonte;
5. ritrarre la cue e perdere soltanto status/context/commitment, non la
   proposizione attribuita;
6. trasferire a tre predicati e strutture mai usati nelle lezioni;
7. fallire onestamente quando il remainder contiene coordinazione, negazione o
   riferimento che il parser semantico comune non sa ancora normalizzare.

Vincoli da preservare:

- niente parser specializzato che riconosca verbi scientifici in C;
- niente promozione automatica delle claim riportate nel mondo;
- niente ID globale basato sul solo contatore;
- niente save di documenti finche' canonicalizzazione/versione e politica di
  volume non sono esplicite;
- niente risposta naturale costruita da stringhe C: frame e verbalizzazione
  devono restare nella KB;
- una sola claim per unita' e `extent(remainder)` sono limiti attuali, non
  assunzioni universali da consolidare.
