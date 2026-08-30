# SC40-B — il delta della KB rivede soltanto il fronte dipendente

**Stato:** `meta-capability-only`

**Data:** 2026-08-30

**Obiettivo:** sostituire il full scan post-lezione di SC40-A con un evento
semantico estensionale e un indice inverso dichiarativo, senza perdere la
capacita' di aprire gap che non possiedono ancora una dipendenza riuscita.

**TARGET_WORLD_FACTS:** 0. Il verbo held-out e i documenti di stress sono
sonde; la crescita durevole e' la facolta' KB-first committata.

**Profilo:** `kb/profiles/agi.p0`, inglese, strumenti attivi, rete non usata.

**B0/R0:** `36750 / 2529`; **B1/R1:** `36757 / 2536`. L'incremento viene da
policy, viste e receipt core; nessun `/save` e' stato eseguito in SC40-B.

## Baseline e ipotesi falsificata

SC40-A rivedeva correttamente il passato, ma decideva il lavoro dal nome del
modulo vincitore:

```text
knowledge/mention/forget/teachconstruction
        -> enumera tutte le claim
        -> document_claim_interpret su ciascuna
```

Questo confondeva due eventi diversi: «il modulo `knowledge` ha risposto» e
«una conoscenza da cui dipende una lettura e' cambiata». Anche una domanda
ordinaria poteva quindi pagare un full scan. D35 prevedeva invece che la
revisione fosse il taglio inverso della conoscenza realmente mutata.

Il primo tentativo ha inoltre reso visibile un limite meccanico del dialetto:
un `assert` di una relazione a quattro argomenti richiederebbe cinque argomenti
nel goal built-in. Il receipt e' stato quindi rappresentato senza perdita come
tre coordinate, impacchettando i contatori in `outcome/2`.

## Architettura risultante

Prima del dispatch il motore fotografa questa vista KB:

```prolog
revision_dependency_member(document_claim, relation_verb($Predicate)) :-
    relation_verb($Predicate).
```

Dopo il dispatch fotografa la stessa vista e confronta i termini opachi. Il C
non contiene il nome `relation_verb`: domani una seconda famiglia puo' entrare
aggiungendo regole alla vista e alla selezione, senza cambiare il diff.

Per ogni membro aggiunto o rimosso la KB sceglie le candidate in due modi:

```prolog
revision_candidate_claim(document_claim, Claim, relation_verb(P)) :-
    claim_token_observation(Claim, P).       % gap ancora unresolved

revision_candidate_claim(document_claim, Claim, Dependency) :-
    claim_current_reading(Claim, Reading),
    reading_depends_on(Reading, Dependency). % frame gia' riuscito/retract
```

La doppia strada e' necessaria. Usare soltanto `reading_depends_on` avrebbe
precisione alta e recall zero proprio sui testi che una nuova lezione deve
sbloccare. L'indice di token e' un'osservazione meccanica della superficie;
non afferma che il token sia un verbo o che cosa significhi.

Prima lettura e revisione continuano a passare entrambe da
`document_claim_interpret`. Un pass selettivo pubblica un receipt interrogabile:

```prolog
last_revision_pass(
    relation_verb(solvates),
    scope(selective),
    outcome(visited(1), changed(1))).
```

`visited` conta record completi realmente consegnati alla fase pura;
`changed` confronta l'identita' della versione corrente prima/dopo. Un membro
irrilevante produce `visited(0), changed(0)`. Un modulo autorizzato che non
cambia alcun membro non produce neppure un pass vuoto. Se una fotografia
dinamica fallisce, resta un fallback `scope(full)` conservativo: l'allocazione
non puo' trasformarsi silenziosamente in una revisione persa.

## Sessione naturale held-out

Il documento e' comparso una sola volta e conteneva due claim; `solvates` non
era presente nella KB:

```text
read: https://example.org/heldout-solvation
      The data show that the ligand solvates the ion.
      The data show that pressure controls volume.
-> Mapped 2 document unit(s), 2 reported claim(s) ... skipped 2.

was it described that the ligand solvates the ion?
-> I recognize this as a described verification question, but I cannot
   normalize its proposition yet.

solvates is a relation verb
-> Learned: relation_verb(solvates).

was it observed that the ligand solvates the ion?
-> Yes - that claim is reported as observed
   (from https://example.org/heldout-solvation).
```

Il retract parlato ha riaperto il gap; il reteach ha ricostruito una nuova
versione e il dump ha mostrato `visited(1), changed(1)` in entrambi i versi.
La seconda claim non e' stata visitata. La domanda intermedia ha anche esposto
un debito di template (`a observed verification question`): e' wording KB,
non un difetto della revisione, e non e' stato mascherato con una stringa C.

## Gate di scala: 100 claim

Un secondo processo fresco ha osservato un documento sintetico di **100
claim**, tutte inizialmente `gap(no_reading)` e una sola contenente `solvates`.
Il reader ha dichiarato esattamente:

```text
Mapped 100 document unit(s), 100 reported claim(s), and
100 live epistemic status(es); learned 0 world fact(s), skipped 100.
```

Dopo la lezione, il dump conteneva 100 puntatori correnti, una sola nuova
lettura normalizzata e:

```prolog
last_revision_pass(relation_verb(solvates), scope(selective),
                   outcome(visited(1), changed(1))).
```

La domanda naturale ha risposto `Yes` con la fonte. Dopo retract, soltanto
quella versione e' diventata stale, il gap e' tornato corrente e il receipt e'
rimasto `visited(1), changed(1)`. Il caso e' ora permanente in
`tests/p0t/language/document_revision_scale.p0t`; resta separato dal target
rapido perche' l'ingestione di 100 unita' impiega circa un minuto e mezzo sul
solver corrente. Questo costo appartiene a SC43, non al fan-out della revisione.

## Ratchet e compatibilita'

- `document_revision.p0t`: **63/63**. Oltre ai gate SC40-A misura ora
  add/retract/reteach selettivi, nessun pass su un fatto ordinario e candidato
  irrilevante `0/0`.
- `document_revision_scale.p0t`: **11/11** su socket isolato, 100 claim.
- `document_method.p0t`: **25/25**.
- `document_claims.p0t`: **182/182**.
- `make build`: pulito, nessun warning.
- unico `make soft-test` SC40-B: **55 passati, 1 fallito**, rosso storico
  `frontier_chat_audit.it.p0t` riga 97 (`designation`: attesa corta contro
  risposta di recovery piu' ricca). Nessuna attesa e' stata cambiata.

Una prima corsa dei tre ratchet documentali in parallelo non e' stata usata
come evidenza: i client condividono il socket e uno e' rimasto in attesa. La
prova registrata e' la ripetizione sequenziale su daemon fresco; lo stress usa
un socket nominato dedicato.

## Quarantena, persistenza e conteggi

La sessione held-out principale e' stata ispezionata in
`/tmp/parrot0-session-2.p0`: **105 clausole**.

| Categoria | Conteggio | Contenuto |
|---|---:|---|
| W | 0 | nessun fatto del mondo |
| L | 1 | `relation_verb(solvates)`, sonda finale |
| C | 0 | nessuna costruzione di sessione |
| P | 2 | `fact_source` e `reading_fact` della sonda |
| O | 102 | document IR, versioni, receipt e utterance runtime |
| X | 0 | nessuna falsita' o fixture da promuovere |

`/save` **non invocato**, quindi `S=0`: documenti sintetici, handle locali e
verbo held-out non devono contaminare la KB. La meta-capacita' e' persistente
attraverso core/C/test versionati; il ciclo precedente aveva gia' provato il
save/fresh recall di tre verbi relazionali veri.

## Metriche

```text
RetroactiveTransfer      = 3/3 ratchet + 1/1 naturale
RevisionPrecision        = 1/1 sul caso utile; 0 visite sul contrasto
RevisionRecall           = 1/1 sul taglio controllato da 100 claim
RevisionFanout(claim)     = 1 su 100
AddRetractSymmetry        = 2/2
UnrelatedKnowledgePasses  = 0
StaleLeak                 = 0
WorldCommitLeak           = 0
FalseUnderstandingRate    = 0
TotalPersistedClauses     = 0 (meta-capability-only)
```

`RevisionRecall=1/1` e' deliberatamente locale al predicato esatto provato.
Non dimostra ancora recall su morfologia, composti, modalita' o coreferenza.

## Limiti che definiscono SC41–SC43

1. `revision_dependency_member` espone per ora soltanto `relation_verb/1`.
   Marker, modalita', ellissi, schema e policy di copertura non sono ancora
   identita' di dipendenza complete.
2. L'indice iniziale copre token atomici normalizzati. `warm` contro `warmed`,
   forme composte e punteggiatura interna chiedono un indice lessicale derivato
   e con provenance, non stemming nascosto nel C.
3. Il metodo deriva la readiness live dalla claim e quindi si aggiorna, ma non
   esiste ancora un unico DAG versionato claim -> argomento -> metodo -> modello
   -> sintesi. E' SC42.
4. Ogni turno con documenti fotografa i membri dichiarati. L'insieme e' oggi
   piccolo; costo, budget, batching e coda `pending_revision` sono SC43.
5. `last_revision_pass` conserva l'ultimo evento, non un log temporale. Prima
   di revisioni multi-membro serve un'identita' di evento e receipt per evento.
6. Il fallback full e' una guardia di correttezza su errore di snapshot; manca
   ancora un fault-injection ratchettato.

SC40-B chiude quindi D35 per il membro esatto e per il problema fondamentale
dei gap senza dipendenza riuscita. Non chiude la dipendenza completa, la
propagazione transitiva o il governo del costo.
