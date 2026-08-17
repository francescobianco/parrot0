# Breakthrough Limits

## Premessa

Questo documento parte da una domanda precisa:

> Se Parrot0 riuscisse a completare il piano `frontier-kb-natural-dialogue.md`, cosa lo renderebbe ancora non paragonabile a un LLM nell'efficacia sui task per i quali oggi utilizziamo gli LLM?

L'obiettivo non è stabilire se Parrot0 possa "imitare" un LLM.

La domanda più interessante è individuare quali capacità degli LLM siano realmente necessarie per una macchina intelligente e quali siano invece conseguenze contingenti dell'architettura neurale attuale.

Nel perimetro di questa analisi consideriamo **Parrot0 come agente conversazionale senza tool**.

Per definizione, in questa modalità:

* Parrot0 possiede la propria KB;
* può interrogare Wikipedia come fonte enciclopedica esterna;
* non possiede accesso generale al Web;
* non interroga database esterni;
* non utilizza API esterne;
* non accede a file o servizi esterni come fonte di conoscenza.

L'eventuale sistema di tool costituisce un livello differente e non viene considerato in questo documento.

Durante questa analisi due presunti limiti di Parrot0 sono stati sostanzialmente rimossi o ridimensionati:

1. conoscenza enciclopedica interna;
2. analogia profonda come capacità necessariamente neurale.

Altri limiti risultano già parzialmente attaccati dall'architettura esistente.

Rimane quindi un insieme molto più piccolo e interessante di **breakthrough limits**.

---

# 1. Assunzione fondamentale: conoscenza ≠ intelligenza

Un primo apparente vantaggio degli LLM è l'enorme quantità di conoscenza del mondo compressa nei pesi.

Ma questa non deve necessariamente essere considerata una proprietà dell'intelligenza.

È possibile distinguere:

* conoscere un fatto;
* sapere che quel fatto manca;
* sapere che deve essere cercato;
* recuperarlo da Wikipedia;
* valutarne la pertinenza;
* integrarlo con altra conoscenza;
* ragionare su di esso.

Per esempio:

> Qual è la popolazione di Tokyo?

non è necessariamente un test di intelligenza.

Parrot0 potrebbe arrivare a:

```prolog
goal(population(tokyo, X)).

knowledge_missing(population(tokyo, _)).

knowledge_source(wikipedia).

retrieve(wikipedia, population(tokyo, X)).

evidence(E, population(tokyo, X)).
```

La parte cognitivamente interessante emerge con una domanda come:

> Tokyo sta diventando più o meno importante demograficamente rispetto alle altre grandi città asiatiche?

Adesso bisogna:

1. capire quali informazioni sono necessarie;
2. individuare quali possano essere recuperate da Wikipedia;
3. recuperarle;
4. riconciliare definizioni differenti;
5. confrontare periodi differenti;
6. costruire relazioni;
7. formulare una conclusione.

La conoscenza enciclopedica può quindi essere **esternalizzata**, senza concedere all'agente un accesso arbitrario a fonti esterne.

L'architettura diventa:

```text
                 ┌─────────────────────┐
                 │      Wikipedia      │
                 │                     │
                 │ external encyclopedic
                 │ knowledge           │
                 └─────────┬───────────┘
                           │
                        retrieve
                           │
                           ▼
┌──────────┐      ┌───────────────────┐
│ dialogue │ ───► │      Parrot0      │
└──────────┘      │                   │
                  │ KB                │
                  │ semantic model    │
                  │ working memory    │
                  │ inference         │
                  │ planning          │
                  │ learning          │
                  └───────────────────┘
```

Ne consegue un principio importante:

> **La KB non dovrebbe necessariamente contenere ciò che Parrot0 sa enciclopedicamente del mondo. Dovrebbe soprattutto contenere ciò che Parrot0 sa fare con la conoscenza che possiede e con quella che può acquisire da Wikipedia.**

La KB interna dovrebbe quindi privilegiare:

```text
conoscenza enciclopedica           → Wikipedia

come confrontare                   → KB
come verificare                    → KB
come classificare                  → KB
come formulare una ricerca         → KB
come interpretare il risultato     → KB
come risolvere contraddizioni      → KB
come costruire ipotesi             → KB
come pianificare                   → KB
come apprendere                    → KB
come sapere che manca qualcosa     → KB
```

Questo modifica anche l'interpretazione del rapporto **regole/fatti** della KB.

Una KB enormemente grande perché contiene milioni di fatti non rappresenterebbe necessariamente una macchina più intelligente.

Una KB relativamente piccola ma estremamente ricca di regole generative, strategie epistemiche e capacità di acquisizione potrebbe essere cognitivamente superiore.

---

# 2. Wikipedia non è un tool

All'interno di questa analisi è importante mantenere una distinzione precisa.

Wikipedia fa parte del modello cognitivo base di Parrot0 come **memoria enciclopedica esternalizzata**.

Non deve essere confusa con un generico sistema di tool.

Possiamo rappresentare il confine così:

```text
PARROT0 CONVERSATIONAL AGENT

        ┌─────────────┐
        │  Wikipedia  │
        └──────┬──────┘
               │
               ▼
        ┌─────────────┐
input → │   Parrot0   │ → output
        └─────────────┘
```

Il modello discusso in questo documento termina qui.

Una futura architettura agentica potrebbe invece essere:

```text
        PARROT0
           │
     ┌─────┴─────┐
     │           │
 Wikipedia     Tools
                 │
          external actions
```

Ma questa seconda architettura introduce capacità ulteriori e non deve essere utilizzata per giustificare le capacità cognitive di Parrot0.

Quando quindi affermiamo:

> Parrot0 può acquisire conoscenza esterna

nel contesto di questo documento significa esclusivamente:

> **Parrot0 può interrogare Wikipedia.**

---

# 3. Knowledge-gap decomposition

Se la conoscenza enciclopedica è esternalizzata su Wikipedia, emerge un problema più profondo.

Non basta possedere:

```text
wikipedia_search()
```

Bisogna sapere **cosa cercare**.

Consideriamo:

> Perché alcune città costiere stanno sprofondando più velocemente dell'innalzamento del mare?

Una macchina intelligente dovrebbe autonomamente arrivare a qualcosa come:

```text
need:
    sea_level_rise
    land_subsidence

possible causes of subsidence:
    groundwater_extraction
    sediment_compaction
    tectonics
    construction_load
```

A questo punto può interrogare Wikipedia sui concetti che non possiede.

Durante la ricerca potrebbe inoltre incontrare fenomeni che inizialmente non aveva previsto e incorporarli nel modello del problema.

Questa capacità può essere definita:

## Knowledge-gap decomposition

> Trasformare un problema che non si sa risolvere nell'insieme delle conoscenze che sarebbe necessario acquisire per poterlo risolvere.

Il gap non dovrebbe quindi essere soltanto diagnosticato quando Parrot0 fallisce.

Dovrebbe diventare un **motore cognitivo**:

```text
goal
 ↓
attempt
 ↓
knowledge gap
 ↓
decomposition
 ↓
information requirements
 ↓
Wikipedia
 ↓
new knowledge
 ↓
reasoning
 ↓
solution
```

Il meccanismo di gap analysis previsto dal Frontier Plan può diventare la base di questa facoltà.

---

# 4. Universal Input cambia il problema dell'analogia

Un secondo presunto limite era:

> Gli LLM sono capaci di analogie profonde tra domini differenti grazie alle rappresentazioni distribuite; un sistema simbolico avrebbe difficoltà a farlo.

Ma `docs/plans/universal-input.md` suggerisce una strada diversa.

Il principio fondamentale di Universal Input è che l'input non possiede necessariamente una tipologia a priori.

Messaggio, codice sorgente, diff, log, stack trace, JSON ecc. partono dallo stesso oggetto:

```text
raw stream
```

Dal flusso vengono estratte strutture.

La KB attribuisce loro significato.

Il kernel dovrebbe rimanere il più possibile **ignorante del dominio**.

Possiamo schematizzare:

```text
ENGINE
  ↓
struttura

KB
  ↓
interpretazione

FACULTY
  ↓
operazione
```

La classificazione stessa non è necessariamente una proprietà assoluta dell'input, ma un'ipotesi sostenuta da evidenza.

Interpretazioni concorrenti possono quindi essere mantenute e confrontate.

---

# 5. Dalla segmentazione all'analogia strutturale

Consideriamo due domini apparentemente lontanissimi.

### Dominio A

```text
una persona richiede una risorsa
la risorsa è posseduta da un'altra persona
la prima persona attende la seconda
la seconda attende la prima
```

### Dominio B

```text
un processo richiede un lock
il lock è posseduto da un altro processo
il primo processo attende il secondo
il secondo attende il primo
```

A livello lessicale:

```text
persona != processo
risorsa != lock
```

Ma dopo interpretazione strutturale possiamo ottenere:

```text
DOMAIN A                  DOMAIN B

X ─requires→ Y            P ─requires→ Q
Y ─held_by──→ Z           Q ─held_by──→ R
X ─waits_for→ Z           P ─waits_for→ R
Z ─waits_for→ X           R ─waits_for→ P
```

Il dominio scompare quasi completamente.

Rimane la struttura.

L'analogia potrebbe quindi essere ottenuta non attraverso:

```text
embedding(A) ≈ embedding(B)
```

ma attraverso:

```text
segmentation
     ↓
semantic relations
     ↓
relational graph
     ↓
structural normalization
     ↓
graph A ≈ graph B
```

Questo porta a un principio importante:

> **L'analogia può essere interpretata come ricerca di invarianti strutturali anziché come similarità nello spazio degli embedding.**

---

# 6. Universal Input + Frontier KB Natural Dialogue

Universal Input da solo non implementa necessariamente un motore analogico.

Fornisce però qualcosa di precedente e fondamentale:

> la possibilità di portare superfici molto differenti verso rappresentazioni confrontabili.

Il Frontier KB Natural Dialogue Plan aggiunge un altro elemento:

> frame semantici che dovrebbero essere invarianti rispetto alla parafrasi e alla superficie linguistica.

Le due architetture possono quindi convergere:

```text
          UNIVERSAL INPUT
                 │
                 ▼
         structural spans
                 │
                 ▼
       semantic interpretation
                 │
                 ▼
        FRONTIER KB FRAMES
                 │
                 ▼
       relational structure
                 │
          ┌──────┴──────┐
          ▼             ▼
       DOMAIN A       DOMAIN B
          │             │
          └──────┬──────┘
                 ▼
         structural match
                 │
                 ▼
              ANALOGY
```

Quindi l'analogia profonda non dovrebbe più essere considerata un limite fondamentale.

La formulazione più precisa è:

> **Parrot0 non ha ancora dimostrato analogical transfer su domini semanticamente distanti, ma Universal Input + frame semantici invarianti forniscono un'architettura plausibile per ottenerlo senza embedding neurali.**

Il fatto che `Segment` venga concepito come tipo fondamentale a monte di `Task` e `Goal` è particolarmente significativo.

Non si tratta soltanto di parsing.

Significa tentare di costruire una **rappresentazione intermedia prima che il sistema abbia deciso quale tipo di problema sta osservando**.

---

# 7. Few-shot learning: forse non è un limite fondamentale

Un altro limite inizialmente attribuito a Parrot0 era:

> un LLM può apprendere/generalizzare qualcosa dopo pochissimi esempi.

Anche questo punto va analizzato più attentamente.

Se Universal Input permette di insegnare a runtime una nuova interpretazione attraverso conoscenza, senza modificare il codice C, abbiamo già una forma particolare di:

## One-shot symbolic learning

La differenza con un LLM sarebbe importante.

Un LLM potrebbe modificare implicitamente il proprio comportamento all'interno del contesto:

```text
examples
   ↓
latent adaptation
   ↓
new behaviour
```

Parrot0 potrebbe invece fare:

```text
example
   ↓
interpretation
   ↓
explicit hypothesis
   ↓
new fact/rule
   ↓
KB
   ↓
new behaviour
```

Questa seconda forma possiede una caratteristica molto interessante:

**l'apprendimento diventa ispezionabile.**

Possiamo sapere:

* cosa è stato imparato;
* da quale evidenza;
* con quale confidence;
* quale comportamento modifica;
* come correggerlo;
* come dimenticarlo.

Quindi anche il few-shot learning potrebbe non essere un limite fondamentale.

Rimane però da dimostrare una capacità più forte:

> estrarre autonomamente una regola generale da pochissimi esempi senza che la forma della regola sia stata anticipata.

---

# 8. Il problema dell'induzione di ontologie

Questo rimane uno dei candidati più seri.

Consideriamo un dominio completamente inventato:

> Esistono tre oggetti chiamati Tars.
> Ogni Tar possiede due Vel.
> Un Vel può attraversare un Nur soltanto quando il Tar associato è nello stato Kel.

Parrot0 può non avere precedentemente:

```text
tar
vel
nur
kel
```

Il problema non consiste soltanto nell'apprendere nuovi valori.

Bisogna capire autonomamente che probabilmente esistono concetti come:

```text
entity
ownership
association
state
transition
constraint
```

e costruire una rappresentazione temporanea utile al problema.

Questa facoltà può essere chiamata:

## Schema induction / ontology emergence

Non semplicemente:

```text
imparare nuovi fatti
```

ma:

```text
inventare la forma della rappresentazione necessaria
```

Una possibile pipeline sarebbe:

```text
unknown domain
     ↓
Universal Input
     ↓
repeated structural patterns
     ↓
candidate relations
     ↓
candidate concepts
     ↓
temporary ontology
     ↓
reasoning
```

Questo potrebbe essere uno dei breakthrough successivi al Frontier Plan.

---

# 9. Ragionamento graduato

Gli LLM operano naturalmente in uno spazio nel quale moltissimi segnali possono contribuire debolmente alla risposta.

Una domanda tipica è:

> Secondo te questa è una buona idea?

Potrebbero esserci contemporaneamente:

```text
costo abbastanza alto
beneficio probabilmente modesto
rischio basso
alternativa disponibile
tempo limitato
preferenza dell'utente per semplicità
incertezza sui dati
```

Nessuno di questi elementi determina necessariamente la risposta.

La conclusione potrebbe essere:

> probabilmente non ne vale la pena.

Un sistema simbolico classico tende invece naturalmente verso categorie come:

```text
true
false
proved
not_proved
candidate
```

Parrot0 può introdurre:

* confidence;
* evidence;
* competing hypotheses;
* scoring;
* provenance;
* weighted rules.

Ma resta da verificare quanto possa emergere un **reasoning graduato ricco** senza trasformare il sistema in una gigantesca collezione di euristiche numeriche.

Questo rimane un breakthrough limit plausibile.

---

# 10. Salienza automatica

Un altro problema è decidere:

> **quale parte dell'input conta?**

Un input reale può contenere migliaia di elementi.

Soltanto pochi possono essere determinanti.

Per esempio:

```text
200 righe di log
1 errore apparentemente innocuo
50 messaggi successivi
```

La macchina deve capire che una particolare riga è causalmente molto più importante delle altre.

Universal Input risolve una parte del problema attraverso segmentazione e classificazione.

Ma rimane una domanda più profonda:

> Come stabilire quali strutture meritano attenzione rispetto all'obiettivo corrente?

Possiamo chiamarla:

## Goal-conditioned salience

```text
input
 ↓
segments
 ↓
semantic structures
 ↓
goal
 ↓
relevance estimation
 ↓
attention candidates
```

Gli LLM possiedono un meccanismo neurale di attention, anche se "attention" non coincide semanticamente con la salienza cognitiva.

Parrot0 dovrebbe produrre qualcosa di funzionalmente equivalente attraverso conoscenza, goal, evidenza e ricerca.

---

# 11. Esplosione combinatoria

Questo probabilmente è uno dei limiti più classici e seri dei sistemi simbolici.

Se abbiamo:

```text
10 interpretazioni
×
10 possibili relazioni
×
10 ipotesi
×
10 azioni
×
10 stati
```

lo spazio delle possibilità cresce rapidamente.

Una macchina puramente simbolica che tenta di esplorare tutto può diventare inutilizzabile.

Gli LLM fanno qualcosa di molto differente: non enumerano esplicitamente tutte le alternative, ma comprimono enormi quantità di struttura nella computazione distribuita.

Parrot0 deve quindi possedere strategie efficaci di:

* pruning;
* ranking;
* beam search;
* confidence;
* relevance;
* heuristics;
* goal-directed reasoning;
* caching;
* abstraction;
* forgetting.

La questione non è soltanto:

> può trovare la soluzione?

ma:

> può trovarla senza esplorare una quantità astronomica di stati?

Questo rimane probabilmente uno dei **breakthrough limits più importanti**.

---

# 12. Creatività generativa aperta

Esistono task nei quali non c'è una conclusione da dimostrare.

Per esempio:

> Inventami dieci nomi divertenti per un database distribuito per pappagalli.

Oppure:

> Scrivi una storia completamente nuova.

Un LLM può campionare uno spazio linguistico enorme.

La qualità deriva anche dalla vastità dello spazio delle possibilità che il modello può generare.

Parrot0 potrebbe utilizzare:

* combinazione;
* trasformazione;
* analogia;
* variazione;
* lessico;
* registri;
* constraints;
* evaluation.

La generazione completamente aperta rimane probabilmente un dominio nel quale gli LLM possiedono un vantaggio architetturale molto forte.

A differenza della conoscenza enciclopedica, però, **non possiamo risolvere questo punto concedendo a Parrot0 un generatore esterno**, perché ciò falserebbe il perimetro dell'esperimento.

Come agente conversazionale senza tool, Parrot0 deve generare la propria risposta attraverso le proprie faculty.

La domanda diventa quindi:

> Quanto può emergere una capacità creativa sufficientemente ricca da combinazione, analogia, trasformazione e ricerca simbolica?

---

# 13. Una distinzione fondamentale: memoria parametrica vs capacità cognitiva

Un LLM moderno tende a fondere molte cose nei pesi:

```text
world knowledge
language
heuristics
patterns
analogies
style
priors
reasoning traces
```

Parrot0 tenta invece di separarle:

```text
input
 ↓
segmentation
 ↓
interpretation
 ↓
knowledge
 ↓
goal
 ↓
reasoning
 ↓
planning
 ↓
answer
```

Possiamo rappresentare grossolanamente le due architetture:

```text
LLM

text
 ↓
huge latent representation
 ↓
probabilistic distributed computation
 ↓
text
```

contro:

```text
PARROT0

text
 ↓
Universal Input
 ↓
candidate interpretations
 ↓
semantic frames
 ↓
context
 ↓
dialogue state
 ↓
situation model
 ↓
goal
 ↓
proof / plan
 ↓
propositional answer
 ↓
realization
```

La seconda pipeline possiede una caratteristica fondamentale:

> **è potenzialmente ispezionabile quasi in ogni punto.**

---

# 14. Un possibile principio architetturale

Da questa discussione emerge una possibile formulazione generale:

> **Non bisogna replicare dentro Parrot0 tutto ciò che un LLM contiene nei pesi. Bisogna identificare il minimo insieme di facoltà necessarie affinché Parrot0 possa acquisire da Wikipedia, utilizzare, verificare e trasformare ciò che gli serve.**

Questo cambia completamente il benchmark.

Non bisogna chiedere:

> Parrot0 conosce quello che conosce GPT?

Bisogna chiedere:

> Posto davanti a un problema nuovo, Parrot0 riesce autonomamente a determinare ciò che deve sapere, recuperare da Wikipedia ciò che gli manca quando disponibile, integrarlo nella propria rappresentazione e utilizzarlo per raggiungere il goal?

Questa è una proprietà molto più interessante.

---

# 15. Il benchmark dei mondi inventati

Un test particolarmente importante dopo il completamento del Frontier Plan potrebbe utilizzare **domini completamente inventati**.

Per esempio vengono definiti nel prompt:

```text
entities
relations
states
constraints
actions
goals
```

che non esistevano precedentemente nella KB e, soprattutto, **non possono essere recuperati da Wikipedia**.

Parrot0 deve:

```text
read
 ↓
segment
 ↓
interpret
 ↓
induce schema
 ↓
build temporary world model
 ↓
reason
 ↓
solve
 ↓
explain
```

Il benchmark dovrebbe deliberatamente utilizzare parole prive di significato precedente:

```text
Tar
Vel
Nur
Kel
```

Questo è particolarmente importante perché neutralizza contemporaneamente:

* la conoscenza interna preesistente;
* Wikipedia;
* le associazioni semantiche pregresse legate ai nomi.

Il sistema dispone esclusivamente delle definizioni fornite nel problema.

Questo misurerebbe:

> **quanto velocemente Parrot0 riesce a costruire un nuovo mondo cognitivo che prima dell'input non esisteva nella sua KB.**

Se riuscisse sistematicamente in questo compito, il confronto con gli LLM diventerebbe molto più interessante.

Non dimostrerebbe soltanto che una KB può sostenere una conversazione naturale.

Dimostrerebbe che:

> **un sistema simbolico può acquisire al volo nuove rappresentazioni e utilizzarle produttivamente senza addestramento dei pesi e senza ricorrere a conoscenza esterna.**

---

# 16. Stato attuale dei presunti limiti

Dopo questa analisi possiamo classificare i limiti inizialmente identificati.

### Conoscenza enciclopedica

**Non fondamentale.**

È esternalizzata attraverso Wikipedia.

Il vero problema diventa knowledge-gap decomposition e acquisizione epistemicamente controllata.

### Analogia profonda

**Probabilmente non fondamentale.**

Universal Input + semantic frames + structural matching offrono una possibile strada simbolica basata sull'invarianza strutturale.

Rimane da dimostrare sperimentalmente.

### Few-shot learning

**Probabilmente parzialmente risolto.**

La KB dinamica permette forme di one-shot symbolic learning.

Rimane aperta l'induzione autonoma della regola generale.

### Induzione di ontologie

**Aperto e importante.**

Il sistema deve essere capace non soltanto di acquisire valori nuovi, ma di inventare le rappresentazioni necessarie per descrivere un dominio nuovo.

### Ragionamento graduato

**Aperto.**

Bisogna verificare se evidence/confidence/scoring possano ottenere una ricchezza paragonabile alla computazione distribuita degli LLM.

### Salienza

**Aperto.**

Serve una capacità goal-conditioned di selezionare ciò che è cognitivamente rilevante.

### Esplosione combinatoria

**Aperto e probabilmente fondamentale.**

Serve una strategia generale per evitare che interpretazione, reasoning e planning producano spazi ingestibili.

### Creatività generativa

**Aperto.**

Nel perimetro dell'agente conversazionale puro non può essere risolto delegando a un LLM o a un generatore esterno.

---

# 17. Breakthrough limits candidati

Dopo aver eliminato o ridimensionato i falsi problemi, rimangono quindi cinque candidati principali:

```text
1. ONTOLOGY EMERGENCE
   inventare rappresentazioni per domini nuovi

2. GRADED REASONING
   integrare grandi quantità di evidenze deboli

3. GOAL-CONDITIONED SALIENCE
   determinare autonomamente cosa conta

4. COMBINATORIAL CONTROL
   ragionare senza esplodere nello spazio delle possibilità

5. OPEN GENERATION
   generare efficacemente in spazi privi di una soluzione determinata
```

A questi possiamo aggiungere una capacità trasversale:

```text
KNOWLEDGE-GAP DECOMPOSITION

capire autonomamente quale conoscenza manca
e trasformare il gap in interrogazioni utili a Wikipedia
```

Quest'ultima non è semplicemente retrieval.

È parte del ragionamento stesso.

---

# 18. Ipotesi di lavoro

L'ipotesi da verificare potrebbe quindi essere:

> **Se Parrot0 possiede Universal Input, rappresentazioni semantiche invarianti, induzione di schema, knowledge-gap decomposition, salienza goal-conditioned e controllo combinatorio, allora la conoscenza enciclopedica può essere esternalizzata su Wikipedia senza compromettere la natura intelligente del sistema.**

In questo scenario Parrot0 non avrebbe bisogno di diventare un LLM.

Avrebbe bisogno di diventare una macchina capace di:

```text
PERCEIVE
   ↓
INTERPRET
   ↓
MODEL
   ↓
NOTICE WHAT IS MISSING
   ↓
QUERY WIKIPEDIA
   ↓
INTEGRATE
   ↓
ABSTRACT
   ↓
REASON
   ↓
PLAN
   ↓
RESPOND
   ↓
LEARN
```

E quando Wikipedia non può contenere la risposta — come nei mondi artificiali inventati nel prompt — deve essere capace di:

```text
PERCEIVE
   ↓
INDUCE
   ↓
BUILD A NEW MODEL
   ↓
REASON INSIDE THAT MODEL
```

La domanda finale non sarebbe più:

> Quanto Parrot0 assomiglia a un LLM?

Ma:

> **Qual è il minimo kernel cognitivo necessario affinché una macchina possa costruire autonomamente i modelli di cui ha bisogno per raggiungere un goal, disponendo della propria KB e di Wikipedia come unica memoria enciclopedica esterna?**

Questo potrebbe essere il vero significato dei *breakthrough limits* di Parrot0.
