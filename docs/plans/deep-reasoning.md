# Deep reasoning — un piano parallelo (upfront design)

## ⇢ HANDOFF / ripartenza (aggiornato 2026-07-09, gen300)

**Dove siamo.** Piano concordato con F. (§4-§4bis: estrazione ampia, fatti
difettibili auto-correttivi, comprensione KB-first). **M0 (estendere la comprensione
per l'estrazione fatti-da-prosa) è DI FATTO COMPLETO** — l'estrazione riusa il parser
di comprensione (`extract_class_statement` in `src/brain/10-memory-knowledge.c`),
EN+IT, gate `tests/cases/prosefact.chat`/`.it.chat`. Frame chiusi:
- gen296 frame 1 — articolo di testa (`A whale is a mammal`→`mammal(whale)`)
- gen298 frame 2 — copula `was/were` / IT `era` (guardata dal noun)
- gen299 frames 3/4/6 — multi-parola, **PP in coda→2 fatti** (`country`+`located_in`),
  locativi (`located_in`/`part_of`)
- gen300 frame 5 — congiunzione di classi (`a Y and a Z` / IT `e`)
- (side) gen297 — fix rosso pre-esistente `family.chat` t5 (seconda persona `you have`)

Tutto verde: `make test` 0 FAIL su run pulito, run.sh 235/235. **Tutto committato e
pushato** (HEAD `5420927`).

**Residui minori M0 (non bloccanti):** apposizione ("the blue whale, a marine mammal,
…") e conjunction relazionale; IT locativo "si trova in"; migrare i frame in una
tabella KB-first `extract_frame(...)` (lever A).

**PROSSIMO (verso il loop — qui riparti):**
1. **M1 — provenienza `fact_source/3`**: ogni fatto estratto conserva il frammento-
   fonte grezzo. È il prerequisito dell'auto-correzione (§4bis). Gate: estrai un
   fatto e interroga la sua fonte. *(il passo giusto da cui ripartire)*
2. **M3 — il loop `deep_reason` + budget + traccia** (`mod_deep_reason`, trigger
   `deep_reason_fresh` "pensaci a fondo"): frontiera, acquire→infer→espandi, cap
   wall-clock (`time(NULL)`), output conclusione+traccia. Studi 1/2 red→green.
3. **M4 — auto-correzione** (contraddizione→ritorno alla fonte). Poi M5/M6.

**Nota infra (non deep-reasoning):** flake intermittente `artfres` nel harness
parallelo (gen278) — passa standalone 7/0, riappare a volte nel run pieno. Pre-
esistente, da guardare separatamente.

Memoria: `~/.claude/.../memory/parrot0-deep-reasoning.md`. Milestone dettagliate §8.

---


> Iniziato 2026-07-09 (dopo gen292). Decisioni di design con F. registrate in §4-§4bis
> (estrazione ampia + fatti difettibili auto-correttivi + comprensione KB-first).
> Piano PARALLELO alla linea principale
> (`NEXT.md`): il deep reasoning non è un bolt-on, è **un piano speciale** che
> chiude un loop interno di inferenza sotto un budget di tempo. Come ogni cosa in
> parrot0 dev'essere **KB-first**: la capacità stessa è conoscenza ispezionabile,
> così parrot0 è *consapevole* di come ragiona a fondo (come già `mod_strategy` /
> `mod_self` descrivono il dispatch reale). Design discusso e concordato con F.
> (2026-07-09).

## 1. La visione (parole di F.)

Dato un input/prompt, parrot0 avvia un **processo continuo** che si ferma dopo **1
minuto di default** (di più se il prompt lo chiede). In quel tempo: esplora la
questione, ci ragiona, **estrae dati da Wikipedia** (come già sa fare), impara
fatti nuovi e **li conserva con sé**, e cerca di *portarsi avanti nella conoscenza*
per far **emergere un risultato** — una sorta di **inferenza rinforzata e
continuata** (multi-hop, non un singolo lookup). Triggerato da solleciti tipo
"pensaci a fondo" o, in seconda battuta su un primo prompt, "pensa ancora".

## 2. Perché è un PIANO (non un modulo nuovo qualsiasi)

Il piano in parrot0 è già **inferito e KB-first** (Track 5.2/5.3): un goal si
espande in step derivati da fatti `action_impl(Action, Primitive)`, che
`plan_execute_goal` cammina fermandosi onestamente sul primo primitivo mancante.
Il deep reasoning è esattamente questo, con **due aggiunte**:

1. un **loop** con **budget di tempo** attorno alla catena di step (invece di una
   passata singola), e
2. una **frontiera** (coda di sotto-concetti) che il loop espande, così
   l'esplorazione si *porta avanti* da sola.

Dichiarare `deep_reason` come un piano in KB (`plan_goal`, `action_impl`, la sua
`intent_cue`) rende la capacità **consapevolezza**: "come pensi a fondo?" →
parrot0 elenca i propri step reali, non una stringa incollata. Substrato riusato,
funzione articolata (PRINCIPLES: *uniform substrate, articulated function*).

## 3. Il loop (deterministico, budgetato)

parrot0 è puro C, niente LLM: il "deep reasoning" NON è chain-of-thought
generativa. È **inferenza iterata deterministica**:

```
deep_reason(Question, Budget):
  target   ← parse_goal(Question)          # es. located_in(paris, europe)
  frontier ← seed_concepts(Question)       # es. {paris, europe}
  trace    ← []
  while not timed_out(Budget):
    if provable(target): break             # EMERSA la conclusione
    c ← pop(frontier)                       # niente più concetti → break (convergenza)
    facts ← acquire_knowledge(c)            # RAM → corpus → Wikipedia; asserisce fatti
    if facts == ∅: continue
    new ← forward_infer()                   # kb_induce + SLD (transitività, ecc.)
    trace ← trace ++ (c, facts, new)
    frontier ← frontier ++ concepts_of(facts)   # PORTARSI AVANTI
  report(target, trace)                     # conclusione + CATENA leggibile
```

**Stop** su, in ordine: (a) target dimostrabile; (b) frontiera vuota
(convergenza); (c) nessun fatto nuovo per K iterazioni (convergenza); (d) budget
esaurito. Budget: **60 s default**; "pensaci per N minuti" / "for N minutes" →
N·60 s.

**Emergenza = derivazione multi-hop**: la conclusione è un fatto *derivato*
concatenando fatti acquisiti **da fonti separate**, che nessuna singola fonte
afferma. È il criterio che distingue il deep reasoning da un lookup.

## 4. Il CUORE: estrazione fatti-da-prosa (decisione F., 2026-07-09)

L'inferenza multi-hop richiede **fatti** (`located_in(paris, france)`), ma
Wikipedia dà **prosa**. L'estrazione è il pezzo centrale, non un accessorio.

### 4.1 Motore: RIUSA il parser di comprensione (niente NLP-engine nuovo)

parrot0 ha già un parser di comprensione: `mod_knowledge` capisce "X is a Y", "is
X the Y of Z" ecc. quando le dice l'utente. L'estrazione **segmenta la prosa in
frasi e le passa a quello stesso parser**: ciò che parrot0 sa *capire*, sa
*estrarre*. KB-first, nessuna logica duplicata.

**Ancoraggio sul titolo pagina**: nella pagina "Paris" il soggetto della frase-
guida *è* `paris` — noto a priori. Così "…is the capital of France" →
`capital(france, paris)` senza risolvere il soggetto.

### 4.2 Baseline misurato (gen292, 2026-07-09) — il gap concreto

Frasi-guida reali date al parser ATTUALE:

| Frase | Oggi |
|---|---|
| `Paris is the capital of France` | ✅ `capital(paris, france)` |
| `Paris is the capital and most populous city of France` | ❌ congiunzione/apposizione |
| `A whale is a mammal` | ❌ articolo sul soggetto |
| `The blue whale is a marine mammal` | ❌ soggetto/classe multi-parola |
| `France is a country in Western Europe` | ❌ PP in coda |
| `France is located in Western Europe` | ❌ frame `is located in` |
| `Socrates was a Greek philosopher` | ❌ copula `was` + agg+nome |

Solo il frame `X is the N of Y` passa. Il lavoro di comprensione da fare
(ognuno un pull gate-first, EN+IT):

1. **spogliare l'articolo di testa** del soggetto (`A whale` → `whale`);
2. **copula `was`/`were`** oltre a `is`/`are`;
3. **soggetto/classe multi-parola** (`blue whale`, `marine mammal`);
4. **PP in coda** (`a country IN western europe` → `is_a(france,country)` +
   `located_in(france, western_europe)`);
5. **congiunzioni/apposizioni** (`the capital AND most populous city` → due fatti);
6. **frame locativo** `is located in`, `is part of`, ecc.

### 4.3 La comprensione dev'essere ADDESTRABILE (KB-first) — annotazione F.

I frame di comprensione **non sono (solo) C hardcoded**: sono **conoscenza**. Come
`intent_cue`/`intent_phrase` (Track 5) e i pattern relazionali, un frame di
estrazione dev'essere una regola/tabella nel KB, *insegnabile a runtime* ("quando
leggi «X si trova in Y», impara `located_in(X,Y)»"). L'estensione della
comprensione è **ingestione di conoscenza**, non solo scrittura di C (lever A,
LOOP.md): è ciò che fa scalare l'estrazione oltre i pochi frame che scriviamo a
mano. Obiettivo: una tabella `extract_frame(Pattern, RelationTemplate)` che
l'estrattore interroga, così nuovi pattern si aggiungono come dati.

### 4.4 Portata: AMPIA, con fatti DIFETTIBILI e auto-correzione (decisione F.)

**NON** precision-first. Si estrae **da tutto il paragrafo lead**, molte
relazioni, **accettando di sbagliare**: la deep research è *resiliente*. Chiave:
ogni fatto estratto conserva il **frammento-fonte grezzo** da cui viene
(provenienza). Quando l'inferenza produce una **contraddizione**, il loop **torna
al frammento**, lo ri-parsa, e **cortocircuita l'informazione sbagliata
riderivandola correttamente**. I fatti sono *defeasible* e *tracciabili*: un fatto
falso è tollerabile perché è ri-controllabile alla fonte. Vedi §4bis.

Le relazioni transitive/gerarchiche (`located_in`, `is_a`, `part_of`) portano la
clausola di transitività sul solver — riuso del keystone **gen291**.

## 4bis. Fatti difettibili + provenienza + auto-correzione (il pilastro di resilienza)

L'estrazione ampia produrrà alcuni fatti **sbagliati** (un `of` partitivo letto
come relazione, un'apposizione mal segmentata). Invece di prevenirlo con la
precisione (che limita la copertura), lo si **rende recuperabile**:

1. **Provenienza obbligatoria.** Ogni fatto estratto è accompagnato dal frammento-
   fonte grezzo: `fact_source(Fact, Concept, "raw sentence")` (o un id di
   frammento). Il "fatto grezzo" e la sua frase vivono insieme nel KB.
2. **Rilevazione di contraddizione.** Durante il loop, un fatto derivato che
   confligge (via `kb_is_conflicted` / naf / una regola di incompatibilità, es.
   `located_in` aciclico, un `capital` funzionale a valore unico) segnala un
   sospetto.
3. **Ritorno alla fonte (il cortocircuito).** Il loop **ri-apre il frammento**
   sospetto e lo ri-parsa con più contesto (o con un frame più stretto),
   **corregge o ritira** il fatto grezzo, e **rideriva**. Questo è il "controprova
   da solo" richiesto: non fidarsi ciecamente del fatto estratto, ma poterlo
   *rivedere alla fonte*.
4. **Rinforzo.** Un fatto **confermato** da più frammenti/fonti indipendenti sale
   di supporto (verso `kb_induce`/certezza); uno **isolato e contraddetto** viene
   ritirato. È l'"inferenza rinforzata e continuata": il KB *migliora* di qualità,
   non solo di quantità, iterando.

Design onesto: la contraddizione non è un errore fatale, è il **segnale** che
guida l'auto-correzione — la resilienza È la feature. Il gate lo dimostra con uno
studio in cui l'estrazione ampia apprende un fatto sbagliato e il loop, tornando
alla fonte, lo corregge e arriva comunque alla conclusione giusta (Studio 4, §7).

## 5. Trigger (KB-first, insegnabili)

Fatti `intent_cue` in `kb/core/intents.p0` (come tutte le cue migrate in Track 5):

- `deep_reason_fresh`: "think deeply", "pensaci a fondo", "reason it through",
  "ragionaci su" → avvia `deep_reason` sull'ultimo/questo prompt.
- `deep_reason_more`: "think more", "pensa ancora", "keep going", "vai avanti" →
  **continua** il loop dal turno precedente (riusa la frontiera salvata in
  sessione — inferenza *continuata*).
- Budget: bigramma "for N minutes" / "per N minuti" → budget = N·60 s.

## 6. Output = conclusione + TRACCIA d'inferenza (scelta di F.)

parrot0 risponde con la conclusione emersa PIÙ la catena leggibile che l'ha
prodotta, es.:

> **Sì — Paris è in Europa.** L'ho derivato: ho appreso *Paris è in Francia*
> (fonte: Paris), poi *la Francia è in Europa* (fonte: France), e per transitività
> di "si trova in" ne segue *Paris è in Europa*. [2 hop, 2 fonti]

La traccia è ciò che **prova** che il ragionamento è profondo. Onestà: se il loop
non deriva la conclusione entro il budget, **declina** dicendo cosa ha appreso e
cosa manca (mai inventare la conclusione — PRINCIPLES: no impostor).

## 7. Le piccole PROVE (studi red→green)

Ogni studio è una domanda la cui risposta richiede **≥2 hop su fatti acquisiti da
fonti separate**: un singolo query **mura**, il deep reasoning **deriva** — con
traccia. Metodologia gate: **corpus locale** (deterministico, ermetico) come gate
primario; **un gate live** separato colpisce Wikipedia davvero ma è network-gated
(skip pulito senza rete: `PARROT0_WIKI_FETCH` off → skip, così `make test` resta
verde offline).

- **Studio 1 — transitività geografica.** "Is Paris in Europe?" → `located_in`:
  Paris→France (fonte Paris), France→Europe (fonte France). Muro senza deep
  reasoning; `Yes` + traccia dopo.
- **Studio 2 — catena di categoria.** "Is a whale a vertebrate?" → `is_a`:
  whale→mammal (fonte whale), mammal→vertebrate (fonte mammal). Muro→`Yes`+traccia.
- **Studio 3 — fatto acquisito + regola.** Una domanda che richiede un fatto
  appreso (es. capitale) PIÙ una regola già nel motore, così l'emergenza combina
  acquisizione e inferenza, non due sole acquisizioni.
- **Studio 4 — auto-correzione (il pilastro §4bis).** L'estrazione ampia apprende
  un fatto SBAGLIATO da un frammento ambiguo; l'inferenza lo mette in
  contraddizione; il loop torna al frammento-fonte, corregge il fatto grezzo e
  arriva comunque alla conclusione giusta — con una traccia che *mostra* la
  correzione. È la prova che la deep research è resiliente, non solo accumulativa.

Ogni studio ha il suo controllo negativo (una conclusione che NON segue → `No`/
declino onesto) e il suo gemello IT (ratchet bilingue).

## 8. Milestone (gate-first, una generazione per milestone)

> Ordine rivisto (decisione F.): l'**estrazione fatti-da-prosa è il cuore** e viene
> per prima, addestrando la comprensione KB-first. Il loop/budget la avvolge dopo.

- **M0 — estendere la COMPRENSIONE sulle frasi-guida (il primo lavoro reale).**
  Chiudere il gap misurato in §4.2, un frame per generazione, EN+IT, gate-first,
  riusando `mod_knowledge`: **(a) articolo di testa — FATTO gen296** (`A whale is a
  mammal`→`mammal(whale)`; gate `tests/cases/prosefact.chat`+`.it.chat`; sblocca la
  catena whale→mammal→vertebrate); **(b) copula `was/were` — FATTO gen298** (IT
  `era/erano` guardato dal noun-omonimo); **(c) soggetto/classe multi-parola —
  FATTO gen299**; **(d) PP in coda `is_a` + `located_in` — FATTO gen299** (due fatti
  da una frase); **(f) frame locativi — FATTO gen299** (`located_in`/`part_of`);
  **(e) congiunzione di classi — FATTO gen300** ("a Y and a Z" / IT "e"; la parte
  RELAZIONALE e l'apposizione restano il residuo). Metrica: 6/7 frasi-guida di §4.2
  ora estratte; **M0 comprensione di fatto completo.** **Annotazione KB-first:**
  man mano, migrare i frame in una tabella `extract_frame(...)` insegnabile, non solo
  rami C. NB: l'estrattore gen299 è volutamente AMPIO (§4.4) — sovra-estrae, e va
  bene: serve M1 (`fact_source`) + M4 (auto-correzione) per la resilienza.
- **M1 — provenienza dei fatti** (`fact_source/3`): ogni estrazione conserva il
  frammento grezzo. Prerequisito dell'auto-correzione. Gate: estrai un fatto e
  interroga la sua fonte.
- **M2 — l'estrattore su prosa**: sentence-splitter dell'`## Extract`/`## Page
  Text`, passa ogni frase al parser esteso (M0), asserisce i fatti + `fact_source`.
  Ancoraggio sul titolo. Gate su una pagina reale del corpus (`paris.md`,
  `whale.md`): conta i fatti corretti estratti. Gate live network-gated separato.
- **M3 — il loop `deep_reason` + budget + traccia**: frontiera, acquire→infer→
  espandi, cap wall-clock (`time(NULL)`), output conclusione+traccia. `mod_deep_reason`
  registrato + reificato `module(...)`. Trigger `deep_reason_fresh`. Studio 1 e 2
  red→green.
- **M4 — auto-correzione (§4bis)**: rilevazione contraddizione + ritorno alla
  fonte + ritiro/correzione + rinforzo. Studio 4 red→green. È il pilastro di
  resilienza.
- **M5 — continuazione "pensa ancora"** (frontiera persistita) e **sintassi budget**
  "per N minuti"/"for N minutes".
- **M6 — introspezione**: `deep_reason` come piano ispezionabile ("come pensi a
  fondo?" → gli step reali); reified `plan_goal`/`action_impl`.

## 9. Onestà / limiti (nel doc e nei commit)

- **Deterministico, non generativo.** "Deep" = inferenza multi-hop iterata su fatti
  acquisiti, non CoT di un LLM. Nessuna speculazione: si deriva o si declina.
- **Blocking turn.** Il loop gira dentro `brain_respond` con un cap wall-clock; un
  turno "pensaci a fondo" può bloccare fino al budget. Coerente con la shell REPL
  di `main.c` (una riga → una risposta). Il default 60 s è per il deep reasoning,
  NON per i turni normali.
- **M1 usa fatti curati**; l'estrazione-da-prosa (M2) è dichiarata come milestone
  a sé — non si finge che M1 estragga da prosa libera.
- **Budget onesto**: se scade prima dell'emergenza, si riporta il parziale e cosa
  manca; non si allunga di nascosto né si inventa.

## 10. Riferimenti di substrato (già esistente, da riusare)

- `src/brain/25-wordmath-reasoning.c`: `plan_goal_steps`, `plan_execute_goal`,
  `action_impl` (il piano inferito e camminato).
- `src/brain/50-self-research-loop.c`: `acquire_knowledge` (RAM→corpus→Wikipedia,
  già un passo di piano riusabile), `mod_learn`.
- `src/learn.c`: `learn_topic` (pagine `.md` → `wiki_concept`; campo `Domain:`
  strutturato), `wiki_fetch_topic` (HTTPS certificato, gated).
- Motore: SLD `kb_query`/`kb_match`, transitività binaria sul solver (**gen291**),
  `kb_induce` (analogo deterministico del "training").
- `kb_save(KB_SESSION|KB_INDUCED)`: i fatti appresi *restano con sé*.
