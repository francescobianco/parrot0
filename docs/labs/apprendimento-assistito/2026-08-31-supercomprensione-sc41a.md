# SC41-A — una lettura ricorda perche' e' stata possibile

**Stato:** `meta-capability-only`, taglio `passive_core` chiuso; SC41 globale
resta aperta.

**Data:** 2026-08-31

**Obiettivo:** fare in modo che una lezione morfologica successiva rilegga
retroattivamente prosa scientifica passiva gia' osservata, e che ogni versione
conservi non soltanto il frame ma le coordinate che lo hanno autorizzato e
selezionato.

**TARGET_WORLD_FACTS:** 0. Le due lezioni `relation_verb(bound)` e
`irregular_participle(bound)` sono sonde linguistiche vere; nessun `/save`.

**Profilo:** `kb/profiles/agi.p0`, inglese, strumenti e rete disattivati nel
processo. La rete e' stata usata fuori dal processo soltanto per verificare le
fonti prima del referto.

**B0/R0:** `36757 / 2536`; **B1/R1:** `36783 / 2582`.

## Fonti e statuto delle tre frasi

Le frasi date a parrot0 sono parafrasi controllate, non citazioni:

- [NCBI Bookshelf, Oxygen Transport](https://www.ncbi.nlm.nih.gov/books/NBK54103/):
  circa il 98% dell'ossigeno e' trasportato reversibilmente legato
  all'emoglobina;
- [NCBI Bookshelf, antibody–antigen interaction](https://www.ncbi.nlm.nih.gov/books/NBK27160/):
  gli anticorpi legano gli antigeni mediante il proprio sito di legame;
- [NCBI Bookshelf, cell-surface receptors](https://www.ncbi.nlm.nih.gov/books/NBK9866/):
  i ligandi di segnalazione legano recettori sulla superficie della cellula.

Il controllo mantiene deliberatamente uguali costruzione e verbo, variando
dominio e argomenti. Misura transfer morfologico, non memoria della prima frase.

## Baseline causale

Prima di SC41, dopo aver osservato il documento una sola volta:

```text
bound is a relation verb
read: ... oxygen was bound by hemoglobin.
was it observed that oxygen was bound by hemoglobin?
-> I recognize this as a observed verification question, but I cannot
   normalize its proposition yet.

bound is an irregular participle
-> Learned: irregular_participle(bound).
```

La seconda domanda non recuperava il testo. Il dump conservava soltanto:

```prolog
claim_reading_record(Claim, reading(1),
    signature(gap(no_reading), proposition(none), extent(unknown))).
reading_depends_on(reading(1), unresolved).
```

Mancavano sia l'evento morfologico nella vista di delta sia l'identita' dello
schema selezionato. Rileggere manualmente avrebbe nascosto entrambi i difetti.

## Risultato naturale: un insegnamento, tre revisioni del passato

Nel processo finale sono state osservate, prima della lezione, tre fonti:

```text
read: NBK54103  The evidence shows that oxygen was bound by hemoglobin.
read: NBK27160  The evidence shows that antigen was bound by antibody.
read: NBK9866   The evidence shows that ligand was bound by receptor.
-> tre claim riportate, tre gap, zero fatti del mondo

bound is an irregular participle
-> Learned: irregular_participle(bound).

was it observed that oxygen was bound by hemoglobin?
-> Yes ... (from .../NBK54103/).
```

Non e' comparso un secondo `read:`. L'unico evento ha pubblicato:

```prolog
last_revision_pass(
    license(irregular_participle(bound)),
    scope(selective),
    outcome(visited(3), changed(3))).
```

I tre frame correnti sono rispettivamente:

```prolog
bound(hemoglobin, oxygen)
bound(antibody, antigen)
bound(receptor, ligand)
```

Il passivo non produce quindi l'ordine della superficie: la selezione dei ruoli
resta nella KB.

## Genealogia completa del taglio passivo

`P0FrameReading` conserva ora il termine dello schema realmente scelto. Il C lo
riconsegna a `frame_reading_dependency/2` e materializza i termini restituiti
senza interpretarli. Per la prima claim il dump contiene:

```prolog
reading_depends_on(reading(4), relation_verb(bound)).
reading_depends_on(reading(4),
    license(frame_pattern("@O was bound by @S", bound))).
reading_depends_on(reading(4), license(past_participle(bound))).
reading_depends_on(reading(4), license(irregular_participle(bound))).
reading_depends_on(reading(4), license(passive_auxiliary(was))).
reading_depends_on(reading(4), license(passive_agent_marker(by))).
reading_depends_on(reading(4), selection(frame_role_order(s, 1))).
reading_depends_on(reading(4), selection(frame_role_order(o, 2))).
reading_depends_on(reading(4),
    selection(normalization_policy(reported, normalized))).
```

La distinzione e' intenzionale:

- `license(...)`: senza quella coordinata lo schema non esiste;
- `selection(...)`: la coordinata decide ordine o accettazione fra esiti;
- l'indice delle opportunita' resta separato e trova gap che non possiedono
  ancora una prova riuscita.

La morfologia non e' duplicata in C. La vista di evento fotografa la radice
insegnabile `irregular_participle(bound)`, mentre `past_participle(bound)` resta
la vista derivata usata dallo schema. Fotografare anche la vista derivata per i
verbi regolari avrebbe prodotto due eventi per una sola lezione
`relation_verb(slowed)`.

## Stessa proposizione, prova diversa

Prima, `document_claim_revision/3` considerava idempotente ogni firma uguale.
Questo confondeva:

```text
stesso frame  !=  stessa genealogia di supporto
```

Il ratchet insegna una costruzione `glints -> glorphs`, legge una claim e poi
ritratta `relation_verb(glorphs)`. La costruzione continua a produrre
`glorphs(mira,kora)`, quindi la semantica non cambia; cambia la licenza primaria
da `relation_verb(glorphs)` a `frame_predicate(glorphs)`. Il puntatore passa a
una nuova versione, la vecchia resta stale e il receipt misura `visited(1),
changed(1)`. Una proof-carrying interpretation ha identita' `(semantica,
supporto)`, non soltanto `semantica`.

## Ablation, reteach e simmetria

Il retract parlato della sola radice morfologica produce:

```text
forget that bound is an irregular participle
-> I no longer treat «bound» as a irregular participle.

was it observed that oxygen was bound by hemoglobin?
-> ... cannot normalize its proposition yet.
```

Tutte e tre le versioni normalizzate diventano storiche; tre nuove versioni
`gap(no_reading)` diventano correnti. Superficie, fonte e status restano vivi.
Il reteach, ancora senza replay, ricostruisce tre successori normalizzati e la
domanda sul terzo dominio risponde dalla fonte NCBI corretta.

```text
AddRetractReteach = 3/3 + 3/3 + 3/3
StaleLeak         = 0
WorldCommitLeak   = 0
```

## DependencyCompleteness e limiti onesti

Il denominatore `reading_dependency_requirement(passive_core, Coordinate)` e'
ora enumerabile. Il ratchet verifica sette famiglie coperte su sette richieste:

```text
predicate_license, selected_schema, morphology, auxiliary,
agent_marker, role_order, extent_policy

DependencyCompleteness(passive_core) = 7/7
```

Questo numero e' **locale**. Non significa che il lettore intero sia completo.
Restano fuori dal taglio corrente:

1. marker epistemico e sua classificazione (oggi gia' live, ma fuori dalla
   genealogia della versione di lettura);
2. modalita' come versione con supporto, non sola vista viva sul token;
3. ellissi, coreferenza, determinanti, confini di sintagma e question word;
4. precedenza fra schemi concorrenti e spiegazione di perche' uno ha vinto;
5. propagazione claim -> argomento -> metodo -> modello -> sintesi;
6. receipt monotoni per eventi multipli e fault injection del fallback.

Per `construction_frame` e policy di copertura l'indice conserva per ora un
fronte conservativo dichiarato su tutte le letture correnti: recall prima della
precisione. Il prossimo incremento deve materializzare i literal del pattern
con provenance, non nascondere un matcher di lingua nel C.

## Ratchet e verifica

- `document_revision.p0t`: **118/118**; aggiunge transfer passivo 3/3,
  grow/retract/reteach, sette requisiti e coperture, isolamento dal mondo e
  sostituzione same-frame/support-changed;
- `document_claims.p0t`: **182/182**;
- `document_method.p0t`: **25/25**;
- unico `make soft-test` del ciclo: **55 passati, 1 fallito** nel rosso storico
  `frontier_chat_audit.it.p0t` riga 97. Attesa:
  `I don't know about designation`; risposta corrente piu' ricca:
  `I don't know much about your designation yet. Want me to look it up?`.
  Nessuna attesa e nessun codice sono stati cambiati per mascherarlo.

La prima sessione diagnostica e quella finale non sono state salvate. Il dump
finale contiene **204 clausole**:

| categoria | conteggio | contenuto |
|---|---:|---|
| W | 0 | nessun fatto del mondo |
| L | 2 | `relation_verb(bound)`, `irregular_participle(bound)` |
| C | 0 | nessuna costruzione di sessione |
| P | 3 | due `fact_source`, un `reading_fact` |
| O | 199 | document IR, versioni, dipendenze, receipt e turni |
| X | 0 | nessuna fixture promossa |

`S=0`: il risultato durevole e' la meta-capacita' versionata in core/C/test,
non il lessico della sonda.

## Prossimo punto di ripresa

SC41-B deve prendere una claim **modale con ellissi** e chiudere lo stesso
contratto su marker, forza modale, recupero dell'agente e scelta fra letture.
Prima, per richiesta esplicita del teacher, il prossimo checkpoint operativo e'
l'asse autocorrezione/autocrescita sul gate `quanot fa 2 +3`, seguito da un
ciclo LEARN_PROTOCOL su corpus dialogico ampio e stratificato. L'handoff
eseguibile e' in `LEARN_TODO.md`.
