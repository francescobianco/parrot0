# SC2-B — la normalizzazione semantica è una fase pura e contestuale

Data: 2026-08-29

Protocollo: [`LEARN_PROTOCOL.md`](../../../LEARN_PROTOCOL.md)

Piani: [`apprendimento-assistito.md`](../../plans/apprendimento-assistito.md),
[`frontier-kb-natural-dialogue.md`](../../plans/frontier-kb-natural-dialogue.md)

Predecessore: [SC2-A](2026-08-29-supercomprensione-sc2.md)

## Parametri

| Parametro | Valore |
|---|---|
| `DOMINIO` | contenuto proposizionale di claim riportate in prosa scientifica |
| `OBIETTIVO` | sostituire `proposition(surface("..."))` con un frame normalizzato **senza** cambiare chi ci crede, e rendere quella claim interrogabile in lingua naturale con prova |
| `BUDGET` | un incremento motore, cinque lezioni naturali, ratchet completo, save e fresh process |
| `FONTI` | NASA NTRS (DART), GMD/Copernicus (WRF ad alta risoluzione), WHO (vaiolo), PRL (LIGO GW150914), PMC (ricerca osservazionale, Salmonella ABM) |
| `TARGET_WORLD_FACTS` | 0 — il ciclo mira a una meta-capacità; promuovere una claim riportata a fatto del mondo è la stop condition |
| `TARGET_CAPABILITIES` | 2 (normalizzazione in quarantena; interrogazione naturale delle claim) |
| `STOP_CONDITION` | promozione al mondo, perdita di fonte/span, o un `Yes` costruito su una lettura parziale |

Boot prima della promozione: `36406 facts, 2454 rules`, profilo `kb/profiles/agi.p0`,
nessun parse error. Dopo il save, un processo nuovo parte da `36431 facts, 2454 rules`.

Stato dichiarato: **meta-capability-only; SC2-B chiusa nei suoi sette gate, SC2-C aperta**.

## Il collo di bottiglia, e perché era un accoppiamento

SC2-A conservava chi ha riportato che cosa, con quale status, dove e sotto quale
fonte. Il «che cosa» restava byte. La causa non era una mancanza di parser: la
pipeline semantica esisteva già, ma *analizzare* una clausola e *commettere* il
fatto che ne esce erano lo stesso corpo di funzione. Chi voleva sapere come
parrot0 legge una frase non poteva chiederlo senza che la frase diventasse
conoscenza — quindi il remainder di una claim riportata si poteva conservare
come superficie, mai normalizzare, perché normalizzarlo avrebbe voluto dire
crederci.

L'incremento è la separazione:

```text
ANALIZZARE una clausola dichiarativa  ->  candidato semantico   (fase pura)
COMMETTERE quel candidato nel mondo   ->  conoscenza di parrot0 (fase impura)
```

Il lettore esegue soltanto la prima, sul remainder, con la **stessa** pipeline
che legge una frase detta in chat. Non c'è un secondo parser documentale e nel C
non compare nessun verbo scientifico. Che cosa possa succedere al candidato non
lo decide il parser: lo decide `normalization_origin/2` in KB.

## Fonti e proposizioni

| ID | Proposizione usata | Fonte | Uso |
|---|---|---|---|
| F1 | DART ha accorciato il periodo orbitale di Dimorphos | [NASA NTRS 20230015804](https://ntrs.nasa.gov/citations/20230015804) | baseline, lezione, replay |
| F2 | l'alta risoluzione ha migliorato le previsioni di precipitazione | [GMD 13/1959/2020](https://gmd.copernicus.org/articles/13/1959/2020/) | transfer (simulato ≠ osservato) |
| F3 | la vaccinazione ha eradicato il vaiolo | [WHO, Smallpox fact sheet](https://www.who.int/news-room/fact-sheets/detail/smallpox) | transfer + parafrasi |
| F4 | LIGO ha rilevato onde gravitazionali | [PRL 116, 061102](https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.116.061102) | transfer + fresh process |
| F5 | il confondimento indebolisce l'evidenza osservazionale | [PMC5118066](https://pmc.ncbi.nlm.nih.gov/articles/PMC5118066/) | transfer nel ratchet |
| F6 | una carica iniziale più alta aumenta la mortalità (studio simulativo) | [PMC4996536](https://pmc.ncbi.nlm.nih.gov/articles/PMC4996536/) | transfer nel ratchet |

Le frasi passate a parrot0 sono parafrasi brevi e conservatrici. **Nessun
risultato scientifico è stato promosso a fatto del mondo**: è precisamente ciò
che questo strato esiste per impedire.

## Baseline naturale (processo fresco, prima di ogni lezione)

```text
you> what did the data show?
parrot0> I don't understand that yet.                                    WALL

you> was it observed that DART shortened the orbital period?
parrot0> What number should I use for «it»?                              IRRELEVANT

you> read: https://ntrs.nasa.gov/citations/20230015804 The authors hypothesize
     that a kinetic impactor shortens an asteroid orbit. The data show that
     DART shortened the orbital period.
parrot0> Mapped 2 document unit(s), 2 reported claim(s), and 2 live epistemic
         status(es); learned 0 world fact(s), skipped 2.

you> what did the data show?
parrot0> The source reports it as observed: DART shortened the orbital period
         (from https://ntrs.nasa.gov/citations/20230015804).

you> was it observed that DART shortened the orbital period?
parrot0> What number should I use for «it»?                              IRRELEVANT
```

Due cose separate emergono già qui, e vanno tenute separate anche nelle
metriche: la **domanda di contenuto** si soddisfa dalla superficie (`0/3 -> 2/3`
appena esiste un consumer), mentre la **verifica di status** richiede davvero il
frame normalizzato e resta a muro. Zero proposizioni normalizzate.

## Lezioni naturali

Cinque, tutte nella forma più bassa della scala metalinguistica, e tutte fatti
veri sull'inglese:

```text
shortens is a relation verb
shortened is a relation verb
improved is a relation verb
detected is a relation verb
eradicated is a relation verb
```

I marker epistemici **non** sono stati reinsegnati: SC2-B usa quelli persistiti
da SC2-A (`the authors hypothesize that`, `the data show that`,
`the evidence shows that`, `the model results show that`). Questa è la prova che
i due strati compongono invece di duplicarsi.

## Replay

Lo stesso turno che prima lasciava un gap:

```text
you> read: https://ntrs.nasa.gov/citations/20230015804 The authors hypothesize
     that a kinetic impactor shortens an asteroid orbit. The data show that
     DART shortened the orbital period.
parrot0> Mapped 2 document unit(s), 2 reported claim(s), and 2 live epistemic
         status(es); 2 proposition(s) normalized in quarantine; learned 0 world
         fact(s), skipped 2.

you> was it observed that DART shortened the orbital period?
parrot0> Yes - that claim is reported as observed
         (from https://ntrs.nasa.gov/citations/20230015804).

you> was it hypothesized that DART shortened the orbital period?
parrot0> No - that claim is reported as observed, not hypothesized.
```

Il conteggio delle proposizioni normalizzate è una coordinata **separata** da
quella degli status vivi, di proposito: un frame in quarantena non è uno status,
e confonderli sarebbe il misclaim che questo strato esiste per impedire.

## La prova che conta: normalizzare non è credere

```text
you> /debug shortened
  PREDICATO  shortened
  (nessuna clausola: ne' fatti ne' regole)

you> what did DART shorten?
parrot0> Hmm, I don't know about shorten yet. …
```

Il frame `slowed/shortened(dart, orbital_period)` è interrogabile **dentro il
contesto della claim** e invisibile al mondo:

```prolog
claim_frame(…_unit_1_claim_0, shortened, dart, orbital_period).          % vero
holds_in(context(…_unit_1_claim_0), proposition(frame(shortened, …))).   % vero
holds_in(world, proposition(frame(shortened, …))).                       % falso
shortened(dart, orbital_period).                                         % falso
```

## Transfer, parafrasi, contrasto, composizione

| Prova | Esito |
|---|---|
| GMD (simulazione) | `simulated`, `improved(higher_resolution, precipitation_forecasts)`, fonte corretta |
| WHO (osservazione) | `observed`, `eradicated(vaccination, smallpox)`, fonte corretta |
| PRL/LIGO (osservazione) | `observed`, `detected(ligo, gravitational_waves)`, fonte corretta |
| parafrasi `the evidence shows what?` | stessa risposta di `what did the evidence show?` |
| contrasto `was it observed that … precipitation forecasts?` | `No - … reported as simulated, not observed` |
| contrasto `was it hypothesized that DART …?` | `No - … reported as observed, not hypothesized` |
| composizione fonte+status+span+cue | `claim_proof/2` risale a documento, URI, unità, range e cue viva |

`Transfer@3 = 3/3` (tre fonti reali mai usate per progettare la lezione,
tre predicati diversi, due status diversi). `Paraphrase = 1/1`,
`ContrastPrecision = 3/3`, `Composition = 1/1`.

## La forma interrogativa non è una seconda lezione

`«the investigators predict that»` è la locuzione insegnata;
`«what did the investigators predict?»` è la stessa locuzione **senza il
complementatore** che la apre sulla proposizione. `claim_question_evidence/2` la
deriva togliendo il suffisso, esattamente come `answer_frame/2` deriva la porta
di risposta da un verbo di relazione. E `claim_status_question_evidence/2` deriva
`«observed that»` dallo **status dichiarato nella politica di classe**, non da
una parola scritta nel C.

Conseguenza misurata: chi insegna un marker nuovo domani apre insieme la sua
domanda, senza dire una seconda frase e senza ricompilare. Nel ratchet le tre
locuzioni held-out (`the investigators predict that`, `the measurements indicate
that`, `the model outputs show that`) entrano parlando e le loro domande
funzionano subito.

## Il fallimento onesto che ha cambiato il progetto

La prima versione accettava qualunque schema combaciasse. Misurato su

```text
The measurements indicate that DART slowed the orbital period of Dimorphos.
```

produceva `slowed(dart, orbital_period)`: lo schema combacia e **perde di chi sia
il periodo**. Nella chat quella perdita esiste da sempre e nessuno la vedeva; su
una claim riportata è peggio, perché una verifica di status confronta *frame con
frame*, e due proposizioni su due oggetti diversi collasserebbero nella stessa.
Un `Yes` così è peggio di un muro (mantra #7).

La correzione non è una condizione cablata: la fase pura riporta **quanto** della
frase ha consumato, e la decisione è una policy KB.

```prolog
normalization_extent_policy($Origin, covered($N), of($M), normalized) :-
    normalization_origin($Origin, quarantine), eq($N, $M).
normalization_extent_policy($Origin, covered($N), of($M), partial) :-
    normalization_origin($Origin, quarantine), lt($N, $M).
```

Oggi una claim riportata pretende copertura piena; domani una fonte fidata o un
dominio tollerante ha una riga diversa, senza toccare il lettore. Il risultato:

```text
> read: … The measurements indicate that DART slowed the orbital period of Dimorphos.
!query claim_normalization_gap(…, partial_reading)      % gap tipato
!query document_claim(…, proposition(surface("DART slowed the orbital period of Dimorphos")))
> what did the measurements indicate?
< The source reports it as observed: DART slowed the orbital period of Dimorphos (…).
> was it observed that DART slowed the orbital period of Dimorphos?
<! Yes -                                                % nessun si' inventato
```

La stessa disciplina vale per la coordinazione (`… slowed the period and raised
the plume`): `partial_reading`, superficie intera, nessuna risposta costruita su
metà proposizione. Il gate 7 del checkpoint SC2-A è quindi soddisfatto **per
politica**, non per elenco di casi.

## Ablation e reteach

```text
> forget that "the model results show that" is a simulation result marker
< I no longer treat «"the model results show that"» as a simulation result marker.
> what did the model results show?          -> muro onesto
> was it simulated that higher resolution improved precipitation forecasts?
                                            -> muro onesto
/debug claim_frame                          -> il frame normalizzato è ancora lì
> "the model results show that" is a simulation result marker.
> what did the model results show?
< The source reports it as simulated: higher resolution improved precipitation
  forecasts (from https://gmd.copernicus.org/articles/13/1959/2020/).
```

Cade lo strato **forte** (status, contesto, commitment, e con essi la risposta) e
resta il **debole** (superficie, frame normalizzato, attribuzione, fonte, span).
Il reticolo di evidenza è monotono nella direzione giusta.
`AblationFidelity = 1/1`, reteach `1/1`, e il reteach non richiede rilettura.

## Retention

Nel ratchet, dopo cinque turni pertinenti ma diversi (capitale della Francia,
aritmetica, Amleto, classificazione, traduzione), la stessa verifica di status
risponde ancora correttamente. `Retention = pass`.

## Persistenza

Sessione promossa: **solo le cinque lezioni linguistiche**. I documenti
scientifici sono stati usati in sessioni non promosse — la politica di versione,
deduplicazione, volume e invalidazione del corpus resta aperta (limite SC2-A 6,
non chiuso qui).

```text
parrot0: routed 25 clause(s) into the KB tree
```

| Categoria | Conteggio | Contenuto |
|---|---:|---|
| `W` fatti veri sul mondo | 0 | — |
| `L` fatti linguistici | 5 | `relation_verb/1`: shortens, shortened, improved, detected, eradicated |
| `C` costruzioni/procedure | 0 | — |
| `P` provenienza | 5 | `fact_source(...)` |
| `O` tracce di ordine superiore | 15 | cinque `reading_fact(...)`, dieci `utterance(...)` |
| `X` invalide | **0** | — |

```text
Nuovi fatti veri del mondo salvati in KB: 0
Nuove clausole totali salvate e classificate: 25
Clausole dichiarate da /save: 25
Clausole invalide: 0
```

`B1 - B0 = 36431 - 36406 = 25 = S`; `R1 = R0 = 2454`. Coerente con il diff.
File KB toccati dal save: `kb/learning/taught-lexicon.p0` (5),
`kb/machinery/fact-provenance.p0` (10), `kb/machinery/transcripts.p0` (10).

## Verifica in un processo nuovo

Senza ripetere alcuna lezione, con formulazioni diverse da quelle usate per
insegnare:

```text
> read: https://www.who.int/…/smallpox The evidence shows that vaccination eradicated smallpox.
< … 1 proposition(s) normalized in quarantine; learned 0 world fact(s), skipped 1.
> the evidence shows what?
< The source reports it as observed: vaccination eradicated smallpox (from …).
> was it observed that vaccination eradicated smallpox?     -> Yes
> was it simulated that vaccination eradicated smallpox?    -> No - … observed, not simulated
> read: …PhysRevLett.116.061102 The data show that LIGO detected gravitational waves.
> what did the data show?                                   -> observed, fonte corretta
> was it observed that LIGO detected gravitational waves?   -> Yes
> /debug eradicated                                         -> nessuna clausola
```

`FreshProcessRecall = 7/7`.

## Metriche

| Metrica | Risultato |
|---|---:|
| LessonYield | 5/5 |
| Replay | pass |
| `Transfer@3` | 3/3 |
| Paraphrase | 1/1 |
| ContrastPrecision | 3/3 |
| Composition (fonte+status+span+cue) | 1/1 |
| AblationFidelity | 1/1 |
| Reteach | 1/1 |
| Retention | pass |
| FreshProcessRecall | 7/7 |
| `SurfaceClaimCoverage` | 9/9 |
| `NormalizedClaimCoverage` (copertura piena) | 9/9 |
| `NormalizedClaimCoverage` (copertura parziale, rifiutata) | 0/2 — **per policy** |
| `StatusPrecision` (positivi + negativi) | 10/10 |
| `QuestionAnswerCoverage` | 3/3 (era 0/3) |
| `ProofCompleteness` | 1/1 |
| `WorldCommitLeak` | **0** |
| `FalseUnderstandingRate` | 0 |

`NormalizedClaimCoverage` è deliberatamente misurata su due righe: nove
proposizioni normalizzate e due rifiutate perché la lettura non copriva la
frase. Il secondo numero non è un fallimento da nascondere — è la ragione per
cui il primo può essere creduto.

## Verifica software

- `make build`: verde, nessun warning nuovo.
- `document_claims.p0t`: **124 passed** (50 di SC2-A + 74 di SC2-B).
- `document_rhetoric.p0t` 33, `mention.p0t` 24, `retract.p0t` 17,
  `taught_segment_role.p0t` 21, `taught_lexicon.p0t` 35: tutti verdi.
- `assisted_construction.p0t`: 60 passed / 6 failed — **rosso preesistente**,
  verificato identico su `HEAD` prima della modifica (inversione dei ruoli di
  una costruzione insegnata e forma di una citazione). Non introdotto qui.
- L'unico `make soft-test` del ciclo: **55 passati, 1 fallito**, lo stesso rosso
  preesistente di `frontier_chat_audit.it.p0t` riga 97 (`designation`) già
  registrato da SC2-A. Non rilanciato.

## Che cosa è cambiato nel motore, e perché è generale

1. **`P0FrameReading` + `p0_frame_bind` + `p0_frame_reading`**
   (`src/brain/10-memory-knowledge.c`). La fase pura è ora un'entità con un
   nome: predicato, slot legati, slot interrogativo, **copertura**. Il consumer
   storico `p0_try_extract_frames_only` la usa e conserva soltanto ciò che la
   fase pura non deve fare — rispondere, asserire, annunciare. Nessuna
   duplicazione: c'è un solo legatore di schemi.
2. **Il lettore chiama la fase pura sul remainder** e consegna alla KB un
   candidato con origine e copertura (`src/brain/30-generation-reading.c`). Non
   nomina nessun verbo, nessuno status, nessuna fonte.
3. **`mod_claim_question`**: due forme interrogative, entrambe derivate da
   conoscenza già presente, entrambe verbalizzate da `response_template`.
4. **`kb/core/document-claims.p0`**: origine, policy di copertura, gap tipati,
   viste `claim_frame`/`claim_proof`/`claim_surface`, evidenza interrogativa
   derivata.

Il test del mantra #2 in entrambi i sensi: una locuzione nuova, un verbo nuovo,
uno status nuovo e persino una **politica di copertura** diversa si ottengono
parlando o cambiando una riga di KB; nessuna di queste cose richiede una
ricompilazione.

## Limiti da non coprire con una risposta elegante

1. La domanda di contenuto è limitata al **documento corrente**. Con più
   documenti letti, «what did the authors hypothesize?» non disambigua la fonte.
   È un limite dichiarato, misurato nel ratchet, non un difetto nascosto.
2. Una claim per unità e `extent(remainder)` restano i limiti di SC2-A.
3. La normalizzazione copre `binary(Relation)` a due ruoli. Misura, classe,
   relazione ternaria, negazione, modalità (`can`, `may`) e coordinazione non
   producono ancora un frame: producono un gap tipato.
4. Il confronto fra frame è **strutturale**. Due parafrasi della stessa
   proposizione (`DART shortened the period` / `the period was shortened by
   DART`) restano due frame diversi. L'equivalenza semantica è SC2-C.
5. Lo status question richiede il complementatore esplicito (`observed that`).
   `«Did anyone observe X?»` non è ancora una sua parafrasi.
6. `claim_proof/2` esiste ma non è ancora *pronunciata* su richiesta: si
   interroga, non si chiede a voce «come lo sai?».
7. I documenti non vengono salvati. Versione, licenza, volume e invalidazione
   restano una decisione esplicita non presa.
8. Il rosso di `assisted_construction.p0t` sull'inversione dei ruoli di una
   costruzione insegnata è **precedente e aperto**: un pattern `@O … @S` produce
   ancora il fatto con i ruoli posizionali. La fase pura ora espone gli slot con
   il loro ordine di apparizione, quindi è il posto naturale in cui chiudere quel
   bug — ma chiuderlo qui avrebbe mescolato due cause.

## Prossimo confine: SC2-C

1. **Equivalenza di frame**: due superfici diverse che esprimono la stessa
   proposizione devono convergere sullo stesso frame o dichiarare perché no.
2. **Più di un ruolo**: misura, classe e relazione ternaria dentro una claim
   riportata, con la stessa policy di copertura.
3. **Negazione e modalità come operatori dello status**, non come parole che
   fanno fallire la lettura: `«can alter»` è una claim modale, non un gap.
4. **La prova pronunciata**: «come lo sai?» deve restituire documento, unità,
   span e cue viva in lingua naturale.
5. **Più documenti nello stesso discorso**: la domanda di contenuto deve poter
   nominare la fonte, o dichiarare l'ambiguità invece di scegliere l'ultimo letto.
