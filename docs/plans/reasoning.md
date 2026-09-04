# Reasoning in parrot0

Il reasoning in parrot0 non deve essere inteso come una proprietà interna di un modello, né come una semplice estensione del tempo di inferenza. Parrot0 è un motore Prolog-like e il reasoning deve quindi essere costruito come una capacità architetturale autonoma, governata dalla Knowledge Base.

**Non c'è nessun modello esterno in questo documento.** Ciò che chiamiamo `process(P)` è l'inferenza di parrot0 stesso: un passo di reasoning è un **rientro nella sua pipeline**, con un turno composto a partire dal risultato precedente e da un meta-prompt dichiarato in KB. I flussi stanno nella Knowledge Base, ma ciò che eseguono sono **inferenze successive dello stesso motore**.

Nella modalità più semplice, senza reasoning, il comportamento rimane diretto:

```text
input
→ interpretazione
→ goal
→ risoluzione
→ output
```

Quando il reasoning è attivo, invece, l'output ottenuto da una prima elaborazione non deve necessariamente essere considerato conclusivo. Può diventare l'input di ulteriori passaggi guidati da meta-prompt, nuovi goal, verifiche, critiche, riformulazioni, raccolta di evidenza o sintesi.

Il principio generale diventa quindi:

```text
input
→ prima elaborazione
→ risultato intermedio
→ reasoning pipeline
→ risultato finale
```

La reasoning pipeline non deve però essere una sequenza rigida definita nel codice del motore.

Non vogliamo stabilire a priori qualcosa come:

```text
draft
→ critique
→ verify
→ rewrite
→ answer
```

Questo sarebbe soltanto un workflow hardcoded.

In parrot0 gli schemi di reasoning devono essere **KB-first**.

La Knowledge Base deve poter descrivere quali operazioni di reasoning esistono, quando devono essere applicate, quali condizioni le attivano, quali risultati producono e quale passaggio deve essere eseguito successivamente.

Uno schema potrebbe, per esempio, descrivere:

```text
candidate answer
→ critique
→ revision
→ final answer
```

Un altro:

```text
candidate claim
→ search evidence
→ search counterevidence
→ judge
→ answer
```

Un altro ancora:

```text
problem
        ├→ structural analysis
        ├→ causal analysis
        └→ alternative hypothesis analysis
                 ↓
              synthesis
                 ↓
               answer
```

Gli schemi possono quindi essere **seriali, paralleli o ibridi**.

Un flusso seriale può essere:

```text
A → B → C → D
```

Un flusso parallelo:

```text
       ┌→ B ─┐
A ─────┼→ C ─┼→ E
       └→ D ─┘
```

Un flusso ibrido può combinare entrambe le forme:

```text
A
↓
B
├→ C
├→ D
└→ E
    ↓
    F
    ↓
G
```

L'aspetto fondamentale è che queste strutture non devono essere incorporate permanentemente nel programma.

Devono essere rappresentabili come conoscenza.

Parrot0 deve quindi poter apprendere uno schema di reasoning così come apprende una nuova regola.

Per esempio, attraverso un prompt potrebbe essergli insegnato:

> Quando devi produrre una risposta tecnica complessa, genera prima una soluzione candidata. Poi analizzala cercando errori e assunzioni non dimostrate. Successivamente produci una seconda risposta usando sia la soluzione iniziale sia la critica. Restituisci soltanto la risposta finale.

Questo insegnamento dovrebbe poter essere trasformato in una rappresentazione persistente nella KB.

Concettualmente:

```text
reasoning_scheme(technical_review)

step(generate_candidate)
step(criticize_candidate)
step(revise_candidate)
step(render_final)

after(generate_candidate, criticize_candidate)
after(criticize_candidate, revise_candidate)
after(revise_candidate, render_final)
```

Da quel momento lo schema non è più soltanto un prompt occasionale: è una capacità acquisita.

Lo stesso principio può essere applicato a schemi più complessi, nei quali diversi meta-prompt vengono eseguiti in parallelo.

Per esempio:

```text
                     candidate
                        │
          ┌─────────────┼─────────────┐
          ↓             ↓             ↓
      correctness    evidence      alternatives
          │             │             │
          └─────────────┼─────────────┘
                        ↓
                     synthesis
                        ↓
                      output
```

Ogni nodo può corrispondere a un'attività differente: una risoluzione simbolica, una query alla KB, una chiamata a uno strumento, una **nuova inferenza sulla pipeline stessa**, una verifica, una trasformazione oppure un nuovo goal.

Gli operatori disponibili sono quindi:

```text
reasoning step
→ symbolic resolver
```

oppure:

```text
reasoning step
→ KB query
```

oppure:

```text
reasoning step
→ external tool
```

oppure — ed è quello che rende il grafo un ragionamento e non solo un piano:

```text
reasoning step
→ process(meta_prompt, risultati precedenti)
```

dove `process` è **la stessa pipeline di parrot0, rientrata**. Il risultato del
primo giro non viene consegnato: viene agganciato a un meta-prompt dichiarato in
KB e reimmesso come nuovo turno. Non c'è nessuna intelligenza esterna a cui
chiedere: c'è lo stesso motore che gira di nuovo, su un input che il grafo ha
costruito.

Il reasoning appartiene quindi interamente a parrot0. È il suo motore a decidere
perché un passaggio debba essere eseguito, quali informazioni debba ricevere e
come il suo risultato debba essere utilizzato successivamente — e a eseguirlo.

Una prima risposta può essere ritratta, criticata e trasformata:

```text
R0 = process(P)

C0 = process(
    critique_prompt,
    P,
    R0
)

R1 = process(
    synthesis_prompt,
    P,
    R0,
    C0
)
```

Ma in parrot0 questo meccanismo può essere generalizzato.

Non deve necessariamente esistere soltanto:

```text
prompt → answer → critique → answer
```

Può esistere un grafo arbitrario di elaborazioni:

```text
P
│
├→ R1
│    └→ R4
│
├→ R2 ─────┐
│          ↓
└→ R3 → R5 → R6
```

dove ogni arco e ogni nodo sono descritti dalla KB.

Il reasoning può inoltre essere condizionale.

Un passaggio successivo può essere eseguito soltanto se il precedente risultato soddisfa una determinata condizione.

Per esempio:

```text
candidate
↓
evaluate
│
├── sufficient
│      ↓
│    output
│
└── insufficient
       ↓
   identify_gap
       ↓
   new reasoning step
       ↓
     evaluate
```

In questo modo il sistema non continua semplicemente a "pensare più a lungo".

Continua perché esiste una ragione rappresentabile:

```text
missing evidence
contradiction
unresolved goal
alternative hypothesis
unsupported claim
insufficient confidence
```

Il reasoning diventa quindi deliberativo.

Parrot0 può produrre un risultato provvisorio, analizzarne lo stato epistemico e decidere se è necessario aprire nuovi goal.

Per esempio:

```text
claim(C)
status(C, suspected)
requires(C, evidence(E))
missing(E)
```

può attivare:

```text
resolve(E)
```

Il risultato ottenuto modifica lo stato:

```text
evidence(E)
supports(E, C)
```

e permette una nuova valutazione:

```text
status(C, supported)
```

oppure:

```text
status(C, refuted)
```

oppure ancora:

```text
status(C, unknown)
```

Il reasoning può quindi terminare non soltanto perché è stata prodotta una stringa finale, ma perché è stata raggiunta una condizione epistemica soddisfacente oppure perché non esistono ulteriori operazioni utili.

Possibili condizioni di arresto possono essere:

```text
proved
refuted
supported
both
unknown
budget_exhausted
```

Anche queste condizioni devono poter essere descritte e modificate attraverso la KB.

## Reasoning acceso e spento

Dal punto di vista dell'utente, il comportamento può essere molto semplice.

Con reasoning disattivato:

```text
input
→ normale pipeline parrot0
→ output
```

Con reasoning attivato:

```text
input
→ normale pipeline
→ risultato candidato
→ selezione dello schema di reasoning
→ esecuzione del reasoning graph
→ risultato finale
```

Lo stesso input può quindi produrre due comportamenti differenti senza modificare la capacità fondamentale del resolver.

Il reasoning è uno strato aggiuntivo di deliberazione.

Potrebbe inoltre esistere più di un livello:

```text
reasoning = off
reasoning = light
reasoning = standard
reasoning = deep
```

ma tali livelli non dovrebbero necessariamente corrispondere semplicemente a un numero maggiore di iterazioni.

Potrebbero scegliere differenti schemi, budget, criteri di verifica o profondità di esplorazione.

## Schemi di reasoning insegnabili

La caratteristica più importante è che gli schemi di reasoning devono essere **addestrabili nel senso parrot0 del termine**.

Non training dei pesi.

Training della Knowledge Base.

L'utente dovrebbe poter dire, per esempio:

> Da ora in poi, quando analizzi un possibile bug, prima formula l'ipotesi principale, poi cerca una spiegazione alternativa, quindi cerca evidenza per entrambe e infine scegli quella maggiormente supportata.

Parrot0 dovrebbe poter trasformare questa istruzione in una struttura del tipo:

```text
bug_analysis
     ↓
primary_hypothesis
     ├→ evidence_primary
     │
     └→ alternative_hypothesis
             ↓
       evidence_alternative
             ↓
          compare
             ↓
          conclude
```

Il nuovo comportamento diventa quindi riutilizzabile.

Un altro insegnamento potrebbe aggiungere un ramo:

> Prima di concludere, prova sempre a falsificare l'ipotesi scelta.

Lo schema può diventare:

```text
hypothesis
↓
evidence
↓
comparison
↓
best_candidate
↓
falsification
│
├── falsified → reconsider
│
└── survives  → conclude
```

Il reasoning stesso diventa quindi una forma di conoscenza modificabile.

## Meta-reasoning

Il passo successivo è permettere a parrot0 non soltanto di eseguire schemi di reasoning, ma di ragionare su quale schema utilizzare.

La KB potrebbe contenere conoscenza del tipo:

```text
task(debugging)
→ prefer(debug_reasoning)

task(explanation)
→ prefer(causal_reasoning)

task(uncertain_claim)
→ prefer(evidence_reasoning)

task(complex_decision)
→ prefer(multi_hypothesis_reasoning)
```

Parrot0 può quindi scegliere dinamicamente il reasoning graph appropriato.

A un livello ancora superiore, potrebbe modificare un reasoning graph durante la sua stessa esecuzione.

Per esempio:

```text
current scheme
↓
unexpected contradiction
↓
insert falsification branch
↓
continue
```

A questo punto il reasoning non sarebbe più soltanto un workflow.

Sarebbe una struttura dinamica deliberativa governata dalla conoscenza.

## Principio architetturale

Il principio fondamentale può essere espresso così:

> In parrot0 il reasoning non è una sequenza predefinita di operazioni. È la capacità di costruire ed eseguire pipeline di elaborazione seriali, parallele o ibride, descritte dalla Knowledge Base, nelle quali risultati intermedi possono essere riesaminati, criticati, verificati, combinati o trasformati attraverso nuovi goal, meta-prompt, strumenti e regole. Gli stessi schemi di reasoning sono conoscenza e devono quindi poter essere insegnati, modificati e riutilizzati.

La distinzione fondamentale non è fra parrot0 e qualcos'altro: è fra due modi di
ottenere più deliberazione.

```text
più deliberazione per accumulo
=
più computazione dentro un processo opaco,
che non si può ispezionare né ritrattare
```

```text
parrot0 reasoning
=
orchestrazione KB-first di inferenze successive,
ciascuna esplicita, attribuibile e ritrattabile
```

Il primo produce una catena che si può solo rigenerare. Il secondo produce un
**grafo di ragionamento esplicito, modificabile e apprendibile**, in cui ogni
nodo è un rientro dichiarato nella pipeline dello stesso motore.

Ed è proprio questo che permette a parrot0 di trattare il reasoning non come una
proprietà speciale di un componente, ma come una capacità generale del sistema —
e di farlo senza delegare a nessuno il pensiero.


---
---

# PARTE II — Le basi: che cosa esiste già, e che cosa manca davvero

> Aggiunta del 2026-09-04, dopo un audit del repository. La Parte I è la
> visione; questa è il **ponte con la realtà**, più gli esperimenti che voglio
> provare per primi. Il criterio che governa questa parte è la leva #1 misurata
> in `universal-code-comprehension.md` §7-bis: *tre volte in quattro giorni il
> guadagno più grosso è venuto dal collegare un organo già scritto, mai dal
> costruire il quinto sottosistema.*

## 0. ⛔ Il verdetto dell'audit: il substrato del reasoning ESISTE, ed è spento

Questa è la scoperta che deve governare l'intero piano, perché cambia
completamente il primo passo. parrot0 non ha bisogno di un motore di reasoning:
ne ha **tre dialetti**, tutti KB-first, e due sono in gran parte inerti.

| organo | dove | stato misurato |
|---|---|---|
| `compensation_plan_step(Turn, Index, step(Action, Pre, Effect, meta(Cost, Support)))` | `kb/core/arrests.p0` | **la struttura è già quella che la Parte I chiede** — passi con precondizioni, effetti, costo e supporto |
| `arrest_dag/1`, `arrest_rank/2` | idem | **un DAG con rango topologico**, con guardia esplicita sui cicli |
| `compensation_stop(Turn, Reason)` | idem | condizioni d'arresto **dichiarate**, non cablate |
| `compensation_step/3` | idem | **0 fatti** — il piano esiste, i passi non sono mai stati dichiarati |
| `compensation_alternative/…` | idem | **0 fatti** |
| `answer_plan(Act, Facet, Order, Requirement)` | `kb/core/procedures.p0` | **75 fatti, e gira in produzione**: reasoning seriale ordinato, con requisiti tipati (`required`/`optional`) e rifiuto se manca una faccetta |
| `turn_plan/2` + `plan_step/3` | `kb/core/capabilities.p0` | gen495: piano seriale su strumenti, 2 passi, **vivo** |
| `action_schema(Domain, Action)` + `applicable/2`, `missing_precondition_set/3` | `kb/core/situation.p0` | pianificazione con precondizioni, **4 fatti** |
| `gap_kind/2` | `kb/core/gap-kinds.p0` | 23 fatti: **la specie epistemica di una lacuna**, già tipata |

**Conclusione operativa: il primo lavoro del reasoning non è scrivere un motore.
È unificare tre dialetti in uno e accendere ciò che è già dichiarato.** Un quarto
vocabolario di piani sarebbe il difetto che questo documento dice di voler
evitare, commesso mentre lo si evita.

### 0.1 Il pezzo più prezioso che già c'è: l'operatore a predicato variabile

La Parte I chiede che un nodo possa essere «una risoluzione simbolica, una query
alla KB, una chiamata a uno strumento, una nuova inferenza». Il meccanismo per
farlo **esiste ed è già in uso in due punti**:

```prolog
apply($Predicate, cons($Entity, cons($Value, nil)))
```

`apply/2` è il confine a predicato variabile del solver. Regge il ponte fra
rappresentazioni (`ir_domain_claim/3`) e il motore dei criteri di qualità
(`criterion_finding/3`), dove **la misura è il NOME di una relazione, non una
funzione compilata**. Un nodo di reasoning il cui operatore è una variabile è
esattamente la stessa mossa: `reasoning_step(Schema, N, op(Predicato, Args))`.

Ne segue che **non serve un dispatcher di operatori**: serve dichiararli.

## 1. Ipotesi di soluzione

### H1 — Un vocabolario solo, derivato dai tre esistenti

Non `reasoning_scheme` accanto a `compensation_plan` e `answer_plan`, ma **una
firma comune di cui i tre diventino viste**. La firma minima, presa dalla più
ricca (quella degli arresti) e non inventata:

```prolog
reasoning_scheme(Nome, Scopo).
reasoning_step(Nome, Id, step(Operatore, Pre, Effetto, meta(Costo, Supporto))).
reasoning_after(Nome, Prima, Dopo).          % l'arco: da qui il DAG
reasoning_stop(Nome, Condizione).
```

Il test di questa ipotesi non è che sia elegante: è che
`answer_plan/4` e `compensation_plan/3` si possano **derivare** da essa senza
perdere nulla. Se non si può, la firma è sbagliata e va cambiata, non forzata.

### H2 — L'ordine si deriva dal grafo, non dall'indice

`arrest_rank/2` calcola già un rango topologico su `arrest_depends_on/2`, con la
guardia sui cicli (`arrest_cycle/1`) che impedisce di eseguire un grafo malato.
Il reasoning riusa quello: **l'esecutore non ordina, chiede il rango.** Un ramo
parallelo è semplicemente un insieme di nodi con lo stesso rango, e la sintesi è
il primo nodo di rango superiore che dipende da tutti.

Vantaggio non ovvio: la *parallelizzazione* diventa una proprietà osservabile
della conoscenza, non una scelta dell'esecutore. Si può chiedere a parrot0
«perché hai fatto queste tre cose insieme?» e la risposta è un fatto.

### H3 — Lo stato epistemico esiste già, sotto altri nomi

La Parte I chiede `status(C, supported | refuted | unknown)`. Oggi parrot0 ha:

- `criterion_finding/3` — una pretesa **con la sua evidenza** (che cosa è stato
  misurato, quanto, contro quale soglia);
- `criterion_waiver/3` + `criterion_counterevidence/3` — **la controevidenza
  dichiarata**, cioè ciò che neutralizza una pretesa;
- `ir_domain_claim_basis/3` — **il ponte che ha autorizzato** una pretesa
  cross-dominio, quindi la sua attribuibilità;
- `gap_kind/2` — la specie di ciò che manca;
- e `naf/1` che **rifiuta un goal non ground** invece di indovinare
  (floundering: declina onestamente).

Ipotesi: `status/2` non è un predicato nuovo, è una **vista** su questi. Se non
lo è, significa che uno di questi cinque è mal formato — ed è più interessante
scoprirlo che aggiungere il sesto.

### H4 — Il budget è una condizione d'arresto di prima classe, non un guardrail

Il solver conta già `steps`, `budget_hit` e `loops_cut`, e li **espone** invece
di nasconderli (`kb_inference_report`). Un grafo di reasoning moltiplica
l'inferenza per il numero di nodi: al gen491 un singolo predicato riderivato
442 volte per turno era l'83% del tempo.

Quindi: `reasoning_stop(Schema, budget_exhausted)` non è un ripiego. È
**l'unica condizione d'arresto che il sistema può garantire sempre**, e va
progettata per prima, non aggiunta dopo. Corollario dal §L: prima di attivare il
reasoning si dichiarano le viste materializzate dei predicati che il grafo
rileggerà, o il grafo pagherà il costo di riderivarli a ogni nodo.

### H5 — ⭐ L'operatore che rende il grafo un ragionamento: il RIENTRO

> F., 2026-09-04: *«non c'è nessun LLM, è un refuso. Il processo intendo:
> pipeline di inferenza, l'output del primo giro agganciato a un meta-prompt e
> reinserito. I flussi sono in KB ma di fatto sono inferenze successive.»*

Questa è la chiave dell'intero piano, e semplifica invece di complicare.
`process(P)` **è `brain_respond`**: la pipeline di parrot0, rientrata. Un passo
di reasoning non chiama niente di esterno — compone un nuovo turno dal risultato
precedente più un meta-prompt dichiarato in KB, e lo rimette nella stessa porta
da cui entrano i turni dell'interlocutore.

```prolog
reasoning_step(S, 2, step(reenter(critique_prompt), r0, c0, meta(1, local))).
meta_prompt(critique_prompt, "trova le assunzioni non dimostrate in: {prev}").
```

Ne discendono quattro cose, e nessuna è ovvia:

1. **La primitiva dell'esecutore è UNA sola**, non due come avevo scritto:
   *comporre un turno e rientrare*. `list_sources`/`read_each` del gen495 sono
   casi particolari di rientro con un turno costruito.
2. **Il meta-prompt è conoscenza come tutto il resto**, quindi si insegna
   parlando e si ritratta. È la parte insegnabile di ciò che oggi, in altri
   sistemi, è un prompt di sistema nascosto.
3. **La ricorsione è reale e va limitata sul serio.** Un rientro può a sua volta
   attivare uno schema: senza `reasoning_stop` e senza budget, un grafo si
   mangia il turno. Il solver conta già `steps`/`budget_hit`; il grafo deve
   contare i **rientri**, che sono molto più cari di un passo di inferenza.
4. **Il rischio nuovo è il loop di specchio**: parrot0 che critica la propria
   risposta con la stessa conoscenza che l'ha prodotta può solo confermarla.
   Un nodo di critica che non porta conoscenza *nuova* — una query, uno
   strumento, un'altra zona della KB — è il caso peggiore di E8: non è teatro
   che non cambia niente, è teatro che si dà ragione. **Un passo di critica
   deve dichiarare da dove viene ciò che lo rende diverso dal passo criticato.**

E questo chiude anche la questione di principio senza doverla discutere: non c'è
nessuna intelligenza delegata, perché non c'è nessun altro. `PRINCIPLES.md`
resta intatto — l'unico modello che tocca questo repository è l'agente che lo
*costruisce*, non un organo che parrot0 interroga.

## 2. Vantaggi, e come si distinguono da quelli dichiarati

Il documento dice che il grafo è «esplicito, modificabile e apprendibile». Vero,
ma sono proprietà di progetto. Questi invece sono vantaggi **verificabili**, e
ciascuno ha già il suo meccanismo:

| vantaggio | perché è verificabile qui, e non in una catena opaca |
|---|---|
| **Attribuibile** | ogni passo porta la sua basis (`ir_domain_claim_basis`, `store_proof`): si può chiedere *perché* un nodo è stato eseguito, e la risposta è un fatto, non una ricostruzione a posteriori |
| **Ritrattabile** | togliere un arco con `!forget` cambia il ragionamento nello stesso turno. Una catena interna non si può ritrattare: si può solo rigenerare |
| **Ablatable** | è il criterio anti-impostore: se togliere un nodo non cambia **niente**, quel nodo era teatro |
| **Insegnabile** | canale #1 della Gerarchia di Crescita, e già dimostrato altrove: al gen493 una parafrasi insegnata parlando ha **trasferito** a un soggetto mai nominato nella lezione |
| **Ispezionabile a costo zero** | il grafo è interrogabile prima di eseguirlo: si può chiedere «che cosa faresti» senza farlo |
| **Riproducibile** | stesso input + stessa KB = stesso grafo, e lo stesso esito |

## 3. ⭐ Gli esperimenti che voglio provare per primi

Ognuno ha una **misura** e una **falsificazione**. Un esperimento senza
falsificazione non è un esperimento: è una demo, e nella demo del gen493 ho già
imparato che cosa vale una demo i cui prompt li sceglie chi la scrive.

### E1 — Lo schema è conoscenza (il test minimo, da fare per primo)

Insegnare a runtime uno schema di due passi e vedere la risposta cambiare.

- **Misura:** la risposta cambia dopo l'insegnamento, torna dopo `!forget`.
- **Falsificazione forte:** lo schema insegnato su un soggetto deve valere su un
  soggetto **mai nominato nella lezione**. Se non trasferisce, è un frasario.
- **Perché per primo:** se questo non passa, tutto il resto è architettura senza
  fondamenta.

### E2 — Il grafo non è una catena

Uno schema con tre rami paralleli e una sintesi.

- **Misura:** i tre rami hanno lo stesso `arrest_rank`, la sintesi rango
  superiore, e l'esecuzione rispetta il rango.
- **Falsificazione:** **invertire gli archi `reasoning_after/2` deve invertire
  l'ordine di esecuzione.** Se l'ordine non cambia, l'esecutore sta usando
  l'indice e il grafo è decorativo.

### E3 — L'arresto ha una ragione dichiarata

Lo stesso input, due profili: uno che si ferma su `supported`, uno su
`budget_exhausted`.

- **Misura:** parrot0 **dice perché** si è fermato, e le due ragioni sono
  diverse a parità di input.
- **Falsificazione:** se la ragione è sempre la stessa, la condizione non è letta.

### E4 — ⭐ Il reasoning migliora davvero? (l'esperimento onesto)

Applicare uno schema `finding → critica → revisione` ai criteri di qualità del
codice (`code_finding/3`), che già producono pretese con evidenza.

- **Misura:** *findings ritirati dalla critica / findings prodotti*. Con la
  demo attuale il numero atteso è basso — `wide_fanout` su un dispatcher è
  proprio il caso che `criterion_waiver` dovrebbe cancellare.
- **Falsificazione, ed è quella che conta:** **se la critica non ritira mai
  niente, il passo è decorativo** e va tolto. Un passo di reasoning che non può
  cambiare la conclusione non è reasoning.

### E5 — Quanto costa (da misurare prima di crederci)

Stesso input con `reasoning = off | light | deep`, con `/debug`.

- **Misura:** ms per turno, passi del solver, e i tre predicati più cari.
- **Predizione da falsificare:** il costo cresce **più che linearmente** nel
  numero di nodi, perché i nodi rileggono gli stessi predicati. Se è lineare,
  la mia lettura del §L è sbagliata e va corretta.
- **Contromisura da provare nello stesso esperimento:** dichiarare una
  `materialized_view` sui predicati riletti e rimisurare.

### E6 — L'operatore è una variabile

Uno schema in cui un nodo cambia comportamento **cambiando un solo fatto**,
senza toccare né lo schema né il C.

- **Misura:** `reasoning_step(S, 2, op(P, …))` con `P` diverso → risultato
  diverso.
- **Falsificazione:** se serve un ramo nel C per il nuovo operatore, `apply/2`
  non sta reggendo il confine e l'ipotesi H1 è sbagliata.

### E7 — Meta-reasoning: scegliere lo schema

`prefer(Task, Scheme)` come fatto, cambiato **parlando**.

- **Misura:** lo stesso input esegue un grafo diverso dopo una frase.
- **Falsificazione:** se la scelta resta la stessa, la preferenza non è letta —
  ed è esattamente il difetto che il mantra #17 descrive per il dispatch.

### E8 — L'anti-impostore, da eseguire su TUTTI gli altri

Per ogni schema attivo: togliere **un nodo alla volta** e verificare che la
risposta cambi.

- **Misura:** numero di nodi la cui rimozione non cambia niente.
- **Soglia:** quel numero deve essere **zero**. Ogni nodo sopra zero è teatro
  cognitivo, e va tolto o giustificato.

## 4. Il primo incremento concreto (R1), e perché è piccolo

Non «implementare il reasoning». Il passo esatto, in verticale:

1. Dichiarare `reasoning_scheme/2`, `reasoning_step/3`, `reasoning_after/3`,
   `reasoning_stop/2` **riusando la quadrupla `step(Azione, Pre, Effetto,
   meta(Costo, Supporto))`** già in `arrests.p0` — non una firma nuova.
2. Derivare `turn_plan`/`plan_step` (gen495) dalla nuova firma: se i due passi
   del piano sugli strumenti non si esprimono con essa, la firma è sbagliata.
   **È il primo test, e costa un pomeriggio.**
3. Un esecutore con **UNA sola primitiva**, sulla forma di `p0_compensate`:
   **comporre un turno e rientrare nella pipeline** (H5). Non sceglie: chiede
   alla KB. `list_sources`/`read_each` del gen495 diventano casi particolari di
   rientro con un turno costruito, non primitive a sé.
4. E1 ed E8 come cricchetti, nello stesso commit.

Se il punto 2 fallisce si è imparato qualcosa di vero sulla firma; se riesce, si
ha un vocabolario solo invece di quattro. **In nessuno dei due casi si è scritto
un motore nuovo.**

## 4-bis. ✅ FATTO — R1 punti 1 e 2, e la firma ha retto il test

`kb/core/reasoning.p0`. La firma **non è inventata**: è la quadrupla più ricca
dei tre dialetti — `step(Operatore, Pre, Effetto, meta(Costo, Supporto))` da
`arrests.p0` — promossa a vocabolario comune, con il rango topologico costruito
come `arrest_rank/2` e la guardia sui cicli come `arrest_cycle/1`.

**Il test che poteva falsificarla, e non l'ha fatto:** il piano *vivo* del
gen495 — quello che risponde a «analizza tutti i file sorgenti che trovi» — è
stato riscritto con la firma comune, e `turn_plan/2` e `plan_step/3` sono
diventate **viste derivate**. L'esecutore in `60-agent-tools.c` **non è stato
toccato** e continua a funzionare: è la condizione che rende la migrazione
onesta invece di una riscrittura con un nome nuovo (mantra #18a).

    > analizza tutti i file sorgenti che trovi
        Ho letto 2 sorgenti in struttura — 5 funzioni in tutto: …

Ratchet: `tests/p0t/reasoning/reasoning_graph.p0t` (11 assert), che esegue E1 ed
E2 con schemi dichiarati **a runtime** — nessuno esiste nella KB curata.

⚠ **E2b ha trovato un errore mio, non del motore**, ed è la ragione per cui gli
esperimenti hanno una falsificazione: la prima versione toglieva *un* arco su
tre e pretendeva che il rango della sintesi scendesse, mentre gli altri due la
tenevano su. L'esperimento ha funzionato come doveva — ha detto che avevo torto
io.

### Che cosa resta di R1

Il punto 3 — **l'esecutore generico, una primitiva sola: il rientro** — non è
fatto. Oggi il
grafo si può *dichiarare, ordinare e interrogare*, ma a eseguirlo è ancora
l'esecutore specializzato degli strumenti. Finché quel pezzo manca, il reasoning
è una **rappresentazione**, non ancora una capacità: E3, E4, E6 e E7 non si
possono nemmeno provare.

**È il primo lavoro del prossimo giro**, e la forma è già decisa: `p0_compensate`
(gen442), che legge dalla KB quali azioni esistono e possiede solo le primitive.

---

## 5. Le condizioni di stop di questo piano

Il reasoning si dichiara fallito, e si torna indietro, se:

- un nodo produce testo che nessun nodo successivo consuma (**teatro**);
- l'ablazione di un nodo non cambia la risposta (E8 > 0);
- la ragione dell'arresto è sempre la stessa (E3 fallita);
- un nodo di critica non dichiara da dove viene la conoscenza che lo rende
  diverso dal nodo che critica (H5.4: il loop di specchio);
- il costo per turno supera il budget e la contromisura è **alzare il budget**
  invece di abbassare il costo (§L, e F. l'ha già detto una volta: *«un timeout
  di dieci secondi è già un sintomo»*).
