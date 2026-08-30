# SC40-A — rileggere alla luce di cio' che si e' imparato

**Stato:** `meta-capability-only`

**Data:** 2026-08-30

**Obiettivo:** fare della semantica di una claim una vista versionata e
ritrattabile, affinche' una lezione successiva cambi la comprensione di un testo
gia' osservato senza chiedere al teacher di ripresentarlo.

**TARGET_WORLD_FACTS:** 0 — il ciclo promuove una facolta' metacognitiva e tre
fatti linguistici, non fatti del mondo.

**Profilo:** `kb/profiles/agi.p0`, inglese, strumenti attivi, rete non usata.

**B0/R0:** `36715 / 2529`; **B1/R1:** `36750 / 2529`.

## Fonti e statuto del testo

- [NCBI Bookshelf, PCR](https://www.ncbi.nlm.nih.gov/books/NBK21154/): metodo a
  cicli; `warm the tube` e' una parafrasi controllata usata per isolare un
  verbo procedurale ignoto, non una citazione.
- [NASA NTRS, DART](https://ntrs.nasa.gov/citations/20230015804): cambiamento
  dell'orbita dopo l'impatto; la frase del test e' una proposizione controllata.
- [GMD 13, 1959–2020](https://gmd.copernicus.org/articles/13/1959/2020/):
  interazione fra schema/aerosol e radiazione; usata per un transfer di
  simulazione.
- [PMC10589119](https://pmc.ncbi.nlm.nih.gov/articles/PMC10589119/):
  confondimento negli studi osservazionali; usata per un transfer di evidenza.

I documenti sono rimasti runtime. `/save` ha ricevuto soltanto lezioni
linguistiche vere (`modified`, `modulates`, `biases` sono verbi relazionali) e
la loro provenienza conversazionale.

## Baseline causale

Prima della modifica, la stessa claim poteva avere contemporaneamente due
interpretazioni correnti:

```text
read: ... We then warm the tube.  -> gap(no_reading)
warm is a relation verb           -> lezione acquisita
read: ... We then warm the tube.  -> normalized(reported)
claim_normalization_gap(..., no_reading) -> ancora vero
```

Rileggere accumulava; non rivedeva. Nella baseline naturale del ciclo corrente,
prima della lezione:

```text
was it described that the authors warm the tube?
-> What number should I use for «it»?
```

Il pronome impersonale della verifica veniva inoltre rubato da `repair`. La sua
finestra consumava il primo turno `warm is a relation verb`, che rispondeva con
smalltalk; la lezione funzionava soltanto al secondo tentativo. Entrambi gli
esiti sono stati conservati: erano due difetti distinti.

## Lezione naturale, replay e risposta pronunciata

Dopo il taglio SC40, in un processo nuovo e senza replay del documento:

```text
read: ... Before this ... We then warm the tube. This repeats ...
-> Mapped 3 document unit(s), 3 reported claim(s), and 3 live epistemic
   status(es); learned 0 world fact(s), skipped 3.

was it described that the authors warm the tube?
-> I recognize this as a described verification question, but I cannot
   normalize its proposition yet.

warm is a relation verb
-> Learned: relation_verb(warm).

was it described that the authors warm the tube?
-> Yes - that claim is reported as described
   (from https://www.ncbi.nlm.nih.gov/books/NBK21154/).
```

La domanda iniziale ora esprime esattamente il livello raggiunto: atto e status
sono compresi, proposition no. La prima lezione non viene piu' intercettata e
innesca il pass di revisione dichiarato dalla KB.

## Rappresentazione e meccanica

La superficie, la fonte e lo span restano osservazioni. L'interpretazione e'
ora una materialized view versionata:

```prolog
claim_reading_record(Claim, Reading, Signature).
claim_current_reading(Claim, Reading).
reading_depends_on(Reading, relation_verb(warm)).
reading_stale(Old, superseded_by(New)).
revision_effect(Claim, before(Old), after(New)).
```

`claim_proposition`, `claim_normalization` e `claim_reading_extent` sono viste
sulla sola versione corrente e viva. La vecchia firma non viene cancellata.
Una nuova firma sostituisce soltanto il puntatore e conserva la genealogia. Un
pass idempotente non moltiplica versioni uguali.

La KB decide quando la facolta' puo' pagare la revisione con
`revision_schedule/2` e `revision_trigger_module/2`. Il C esegue meccaniche
fisse: enumera le claim osservate e richiama la stessa fase pura usata dalla
prima lettura. Non contiene `warm`, marker scientifici, generi documentali o
template naturali.

## Transfer, contrasto, composizione, ablation e retention

Il ratchet `document_revision.p0t` chiude **55/55** asserzioni:

- **Transfer@3 3/3:** NASA `altered(impact, orbit)`, GMD
  `perturbs(scheme, radiation_field)`, PMC
  `distorts(confounding, association)`. Ogni documento parte da
  `gap(no_reading)`; una lezione one-shot lo normalizza senza secondo `read:`.
- **Contrasto 1/1:** insegnare `catalyzes` non inventa una claim che non lo
  contiene.
- **Composizione 1/1:** il passo `warm` porta lo stesso metodo da
  `blocked(unbound_step)` a `ready`, senza ricostruire il documento.
- **Ablation 1/1:** `forget that warm is a relation verb` ritira il frame,
  conserva superficie/fonte/status e riporta il metodo al blocco.
- **Reteach 1/1:** la stessa lezione ricostruisce una nuova versione corrente.
- **Retention pass:** dopo due turni estranei frame e metodo restano vivi.
- **WorldCommitLeak 0:** nessuno dei quattro frame riportati entra nel mondo.

Compatibilita': `document_method.p0t` **25/25** e
`document_claims.p0t` **182/182**. Il primo non contiene piu' il replay manuale
che nascondeva il difetto.

L'unico `make soft-test` del ciclo: **55 passati, 1 fallito** nel rosso
preesistente `frontier_chat_audit.it.p0t` riga 97 (`designation`: cambia la
forma del muro). Non e' stata cambiata l'attesa e il test non e' stato
rieseguito.

## Persistenza, quarantena e conteggi

Dump pre-save: `/tmp/parrot0-session-2.p0`, 41 clausole runtime ispezionate.
Nessun documento, claim, fixture o fatto falso era presente. Output esatto:

```text
parrot0: routed 35 clause(s) into the KB tree
```

| Categoria | Conteggio | Contenuto |
|---|---:|---|
| W | 0 | nessun fatto del mondo |
| L | 3 | `relation_verb(modified/modulates/biases)` |
| C | 0 | la facolta' e' codice/KB curata, non una costruzione di sessione |
| P | 6 | tre `fact_source` e tre `reading_fact` |
| O | 26 | 24 utterance vere; `exchange` + `exchange_turn` della domanda su Paris |
| X | 0 | nessuna clausola falsa, ambigua o di test |

`S=35` e `B1-B0=35`. `kb/learning/learned.p0` contiene i due record di
exchange: sono veri ma il save-map non conosce una casa per la specie. Non sono
stati cancellati a mano; il debito di routing e' nel handoff.

Nel processo nuovo, senza reinsegnare: membership **3/3**; la composizione GMD
produce una claim simulata normalizzata e una risposta con fonte/prova. Una
prima composizione NASA con il sostantivo `impact` e' stata rubata dal modulo di
impact analysis: risposta irrilevante, non salvata, registrata come collisione
SC26/SC30. Separata quella collisione, persistenza e consumer documentale sono
verdi.

## Metriche finali

```text
LessonYield            = 3/3
Transfer@3             = 3/3
Paraphrase              = non isolata in SC40-A; resta gate aperto
ContrastPrecision       = 1/1
Composition             = 1/1 sul percorso senza collisione; 1 collisione registrata
AblationFidelity        = 1/1
Retention               = pass
FreshProcessRecall      = 3/3 lessico; 1/1 composizione GMD
FalseUnderstandingRate = 0
WorldKnowledgeGain      = 0
TotalPersistedClauses   = 35
StaleLeak               = 0
RetroactiveTransfer     = 3/3
```

## Limiti e prossime falsificazioni

SC40-A e' chiusa, non l'intero problema della revisione:

1. il pass corrente riesamina tutte le claim dopo un modulo dichiarato; manca
   l'indice selettivo per dipendenza/evento;
2. la dipendenza e' esatta per `relation_verb(P)`, ma per schemi curati resta
   `frame_predicate(P)`: manca l'identita' completa dello schema/morfologia;
3. modalita', marker, coreferenza, argomenti, metodi e sintesi devono dichiarare
   dipendenze transitive e revisionarsi nello stesso DAG;
4. una domanda di status normalizzabile ma assente non ha ancora un `No`
   dedicato; qui e' stato chiuso il solo caso **non normalizzabile**;
5. collisione `impact` e routing di `exchange` restano visibili e prioritari.

File KB modificati dal save: `kb/learning/taught-lexicon.p0`,
`kb/machinery/fact-provenance.p0`, `kb/machinery/transcripts.p0` e la ricaduta
`kb/learning/learned.p0`. I file core, C, test e documentazione elencati nel
commit costituiscono la meta-capacita' SC40-A.

**Commit:** il checkpoint che contiene questo report (hash nel handoff e nella
consegna finale). **Push:** obbligatorio su `origin/main`.
