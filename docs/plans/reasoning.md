# Reasoning in parrot0

Il reasoning in parrot0 non deve essere inteso come una proprietà interna di un LLM, né come una semplice estensione del tempo di inferenza. Parrot0 è un motore Prolog-like e il reasoning deve quindi essere costruito come una capacità architetturale autonoma, governata dalla Knowledge Base e applicabile anche quando il modello linguistico viene usato soltanto come componente ausiliaria.

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

Ogni nodo può corrispondere a un'attività differente: una risoluzione simbolica, una query alla KB, una chiamata a uno strumento, una inferenza LLM, una verifica, una trasformazione oppure un nuovo goal.

Questo permette di separare il concetto di reasoning dall'LLM.

L'LLM diventa eventualmente uno degli operatori disponibili:

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

oppure:

```text
reasoning step
→ LLM(prompt)
```

Il reasoning appartiene quindi a parrot0, non all'LLM.

Un LLM può essere utilizzato per svolgere uno specifico passaggio, ma è il motore di reasoning di parrot0 a decidere perché quel passaggio deve essere eseguito, quali informazioni deve ricevere e come il suo risultato deve essere utilizzato successivamente.

Questo consente anche di riprodurre esternamente alcune delle capacità che nei reasoning model vengono ottenute attraverso una maggiore computazione durante l'inferenza.

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

> In parrot0 il reasoning non è una sequenza predefinita di operazioni e non coincide con la Chain-of-Thought di un modello linguistico. È la capacità di costruire ed eseguire pipeline di elaborazione seriali, parallele o ibride, descritte dalla Knowledge Base, nelle quali risultati intermedi possono essere riesaminati, criticati, verificati, combinati o trasformati attraverso nuovi goal, meta-prompt, strumenti e regole. Gli stessi schemi di reasoning sono conoscenza e devono quindi poter essere insegnati, modificati e riutilizzati.

La distinzione fondamentale è quindi:

```text
LLM reasoning
=
più computazione/token all'interno
del processo generativo del modello
```

mentre:

```text
parrot0 reasoning
=
orchestrazione KB-first
di processi cognitivi espliciti
```

Il primo produce una catena di elaborazione interna al modello.

Il secondo produce un **grafo di ragionamento esplicito, modificabile e apprendibile**.

Ed è proprio questo che permette a parrot0 di trattare il reasoning non come una proprietà speciale di uno specifico modello, ma come una capacità generale del sistema.
