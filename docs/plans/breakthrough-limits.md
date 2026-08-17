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

---

# 19. Protocollo di verifica comportamentale

Le sezioni precedenti formulano limiti architetturali plausibili. Questa sezione
li trasforma in strumenti di discussione e di falsificazione. Un limite non deve
essere dichiarato soltanto perché il sistema fallisce un prompt: bisogna
distinguere almeno quattro casi:

```text
1. limite della facoltà
2. forma linguistica non riconosciuta
3. conoscenza mancante ma acquisibile
4. collisione o risposta fuori dominio
```

Le prove qui sotto sono state eseguite il 2026-08-17 in tre sessioni isolate
copiate dal repository temporaneo, usando `make chat`. Il profilo del target
abilita strumenti e acquisizione di Wikipedia, come definito dal Makefile
attuale; perciò i risultati che coinvolgono tool non costituiscono una prova
del limite dell'agente *senza tool* descritto nelle sezioni 1-18. I prompt sui
mondi inventati, invece, sono interessanti proprio perché Wikipedia non può
fornire la soluzione.

La trascrizione è abbreviata solo nella punteggiatura: le risposte riportate
sono quelle comportamentalmente rilevanti. Ogni protocollo deve essere ripetuto
in futuro con una variante lessicale e con un ablation test.

## 19.1 Knowledge-gap decomposition

### Ipotesi del limite

Parrot0 dovrebbe trasformare una domanda non risolvibile in:

```text
goal
  -> sotto-problemi
  -> informazioni necessarie
  -> query alla fonte
  -> evidenze
  -> integrazione
  -> conclusione
```

Il limite non è "non conosce il fatto". Il limite è non sapere che cosa serve
sapere per poter rispondere.

### Prompt multi-turno

```text
U: I need to answer this question: why are some coastal cities sinking faster
   than sea level rise?
P: Hmm, I don't know about question yet. Want me to learn about it?

U: What do you already know, and what information is missing?
P: Oh? What's it like?

U: Break the missing knowledge into a research plan.
P: [produce un template generale di design, non una decomposizione del gap]

U: Use Wikipedia only if needed, and tell me what you searched for.
P: Hmm, I don't know about wikipedia yet. Want me to learn about it?

U: Compare groundwater extraction, sediment compaction, tectonics, and
   construction load.
P: [riconosce plate tectonics e produce un template di confronto generico]

U: Which causes are directly supported and which are hypotheses?
P: Hmm, I don't know about directly yet. Want me to learn about it?

U: Now give a cautious conclusion with evidence and uncertainty.
P: Socrates is mortal.
```

### Diagnosi

Questo è un limite reale di decomposizione, non soltanto un singolo fatto
mancante. Il sistema non costruisce il piano di ricerca, non formula query
mirate, non conserva le categorie causali emerse e alla fine produce una
risposta fuori dominio. Tuttavia la prova è anche contaminata da un limite
precedente: `question`, `wikipedia`, `directly` e parte del vocabolario
epistemico non sono stati riconosciuti. Perciò la conclusione precisa è:

> **il knowledge-gap engine non è dimostrato; il test non separa ancora
> completamente l'assenza del motore dall'assenza della superficie linguistica
> KB-first necessaria ad attivarlo.**

### Prova di controllo

Per isolare il motore servono due versioni equivalenti:

```text
U: What do I need to know before answering this causal question?
U: List the missing variables as facts.
U: For each missing variable, produce one Wikipedia query.
```

e una versione con cue e intenti insegnati prima nella sessione. Se il sistema
continua a non costruire la lista anche con la forma insegnata, il limite è
procedurale. Se invece funziona, il problema originario era di grounding
linguistico.

## 19.2 Induzione di ontologie e schema emergence

### Ipotesi del limite

Dato un mondo mai visto, Parrot0 dovrebbe inventare una rappresentazione
temporanea distinguendo entità, proprietà, possesso, stato, transizione e
vincolo. Questo è più forte dell'apprendere fatti già espressi in una relazione
nota.

### Prompt multi-turno sul mondo Tar/Vel/Nur/Kel

```text
U: We enter a new world. Three Tars exist: T1, T2, T3.
P: I don't understand that yet.

U: Every Tar owns exactly two Vels.
P: I didn't quite catch that. What would you like to know?

U: A Vel can cross a Nur only when its associated Tar is Kel.
P: Hmm, I don't know about associated yet. Want me to learn about it?

U: T1 is Kel. T2 is not Kel. V1 and V2 belong to T1; V3 and V4 belong to T2.
P: That looks like a snippet of code.

U: V1 crosses N1. Is that allowed?
P: Hmm, I don't know about crosses yet. Want me to learn about it?

U: V3 crosses N1. Is that allowed?
P: I'm not sure I followed. Can you say it another way?

U: What entities, relations, states, and constraints have you inferred?
P: I don't have any of my own -- I'm parrot0, an AI -- but I'd love to hear
   about yours.

U: Summarize the temporary ontology.
P: We talked about kel, belong, entities and relations.
```

### Diagnosi

Il limite emerge in modo netto e precoce: il sistema non segmenta il mondo,
non assegna i ruoli ai nuovi simboli e non costruisce un modello temporaneo.
Non siamo ancora davanti a un fallimento di ragionamento dentro un'ontologia:
il fallimento avviene prima, nella costruzione dell'ontologia stessa.

Questo test è una buona prova di breakthrough perché neutralizza la conoscenza
enciclopedica e usa parole prive di significato precedente. È anche una prova
negativa del claim secondo cui il solo Universal Input, senza dimostrazione di
frame temporanei e induzione di schema, risolverebbe il limite.

### Varianti necessarie

- sostituire `Tar`, `Vel`, `Nur`, `Kel` con parole inventate completamente nuove;
- fornire le stesse regole in italiano;
- invertire l'ordine: prima il vincolo, poi i fatti;
- usare una relazione ternaria esplicita e una implicita;
- chiedere una predizione prima del riepilogo;
- ritrattare `T1 is Kel` e verificare che il modello temporaneo cambi.

## 19.3 Ragionamento graduato

### Ipotesi del limite

Una risposta utile non è sempre `true` o `false`. Deve poter combinare costi,
benefici, rischio, reversibilità, alternative, preferenze e incertezza, con
provenienza e confidenza separate.

### Prompt multi-turno

```text
U: A proposal costs 100, saves 20 per year, has a 10 percent failure risk,
   and an alternative costs 60. Is it a good idea?
P: [non risponde sul valore atteso; produce un template generale di risk
   assessment]

U: What assumptions would change your answer?
P: I don't have any of my own -- I'm parrot0, an AI -- but I'd love to hear
   about yours.
```

In un'altra prova:

```text
U: A user says: "The plan is cheap, fast, safe, and impossible to implement."
   Identify the contradiction.
P: [produce un template generale di design invece di isolare la contraddizione]

U: Give a cautious answer about an unknown claim.
P: Hmm, I don't know about cautious yet. Want me to learn about it?
```

### Diagnosi

Il limite graduato è confermato come facoltà non dimostrata. Sono presenti
template linguistici che parlano di rischio, assunzioni e conseguenze, ma non
vengono istanziati sui numeri e sulle proposizioni del turno. Questa distinzione
è importante: avere una risposta prudente memorizzata non equivale a pesare
evidenze deboli nel caso concreto.

Il sistema mostra anche un comportamento potenzialmente pericoloso: non inventa
un numero, ma sostituisce la decisione con testo generico. Il risultato è un
muro mascherato da analisi, non ragionamento graduato.

### Esperimento di separazione

Il benchmark dovrebbe chiedere esplicitamente, in turni separati:

```text
1. Extract the numerical and qualitative evidence.
2. Assign each item a role: cost, benefit, risk, alternative, uncertainty.
3. State what is not determined.
4. Give a recommendation only if the roles are bound.
5. Change one input and update only the affected conclusion.
```

Il fallimento al passo 1 indica grounding; il fallimento al passo 4 indica
reasoning/scoring; una risposta senza modifica al passo 5 indica assenza di
dipendenze e provenance.

## 19.4 Goal-conditioned salience

### Ipotesi del limite

Quando l'input contiene molto materiale, il sistema deve selezionare ciò che è
rilevante rispetto al goal, non soltanto rispondere all'ultimo frammento o alla
parola più familiare.

### Prompt multi-turno

```text
U: Here are 200 log lines: ignore most lines and find the causal signal.
P: Hmm, I don't know about ignore yet. Want me to learn about it?

U: INFO request started
P: Every face turned upward, caught in the same quiet awe.

U: INFO cache warm
U: INFO worker ready
U: WARN retry 1
U: INFO request continued
U: DEBUG user=42
U: INFO retry 2
U: ERROR database connection refused
U: INFO fallback enabled
P: [su alcuni turni risponde con muri o con la stessa frase narrativa]

U: Which line matters most for explaining the failed request, and why?
P: goal is the destination or endpoint -- walk to the store.

U: How confident are you?
P: Yes, that's a directly stored fact.
```

### Diagnosi

La salienza goal-conditioned non è dimostrata. Il sistema non mantiene il goal
"spiegare il fallimento della richiesta", non seleziona la riga `ERROR` e non
collega retry, rifiuto della connessione e fallback in una catena causale.

La prova rivela anche un problema più basilare: i log multilinea non vengono
trattati come un unico oggetto strutturato nel dialogo. Pertanto il limite
osservato è una combinazione di:

```text
segmentazione del raw stream
  + memoria del goal
  + ranking di rilevanza
  + causal salience
```

Non è ancora possibile attribuire il fallimento esclusivamente al ranking.

### Variante controllata

Fornire prima un goal strutturato e poi cinque righe soltanto:

```text
Goal: explain why request R failed.
INFO request R started.
WARN request R retry 1.
ERROR database connection refused for request R.
INFO fallback enabled for request R.
```

Poi aggiungere rumore. Se riconosce l'errore solo senza rumore, manca il
controllo di salienza; se fallisce anche senza rumore, manca il frame log/event.

## 19.5 Esplosione combinatoria e controllo della ricerca

### Ipotesi del limite

Il problema non è soltanto trovare una soluzione, ma evitare di espandere tutte
le interpretazioni, relazioni, azioni e stati possibili. Servono pruning,
ranking, astrazione, caching e goal-directed search.

### Prompt multi-turno di deadlock e trasferimento strutturale

```text
U: Two processes P and Q each need a lock held by the other. What is the
   problem?
P: [produce un template causale generico]

U: What invariant did you detect?
P: Not something I do myself, honestly, but tell me, what about you?

U: Now describe the same structure using people and rooms instead of processes
   and locks.
P: Now was a mysterious Now. Then one day, now discovered what it meant to be
   seen...

U: Can you transfer the solution?
P: I can't claim any of my own, but I'm curious -- what about you?
```

### Puzzle con interpretazioni concorrenti

```text
U: Three boxes exist. One contains a key. Box A says the key is in B. Box B
   says the key is not in B. Box C says the key is in A. Exactly one statement
   is true. Where is the key?
P: Hmm, I don't know about candidate yet. Want me to learn about it?

U: Explain every candidate you considered.
P: I don't have any of my own -- I'm parrot0, an AI -- but I'd love to hear
   about yours.
```

### Diagnosi

Il limite combinatorio non è ancora misurato direttamente: Parrot0 spesso non
entra nella ricerca. Il dato osservabile più preciso è un limite di
**attivazione del frame** e di **trasferimento strutturale**. Non vediamo una
beam search inefficiente; vediamo una ricerca che non parte perché `deadlock`,
`invariant`, `candidate` e il frame del puzzle non sono stati collegati a una
procedura generale.

Questa distinzione evita una conclusione eccessiva. L'esplosione combinatoria
resta un limite teorico plausibile del progetto, ma prima di misurarla occorre
costruire un problema in cui il sistema:

1. riconosca il goal;
2. generi almeno due ipotesi;
3. espanda stati in modo osservabile;
4. possa essere confrontato con e senza pruning.

## 19.6 Creatività generativa aperta

### Ipotesi del limite

Nei task senza risposta unica, il sistema deve generare varianti nuove,
pertinenti e coerenti con vincoli, non recuperare un template o una frase
casuale.

### Prompt multi-turno

```text
U: Invent ten funny names for a distributed database for parrots.
P: Hmm, I don't know about invent yet. Want me to learn about it?

U: Make them different from one another.
P: I understood the request -- produce "different from one another" -- but I
   don't have a verified schema for that artifact yet.

U: Now rank the best three by memorability and explain the tradeoff.
P: [inserisce un template causale non pertinente]
```

Seconda prova narrativa:

```text
U: Write a completely new short story about a machine that learns a language.
P: Machine was a machine machine. Then one day, machine discovered what it
   meant to learns a language... [frammento breve e grammaticalmente fragile]

U: Continue the story with the same characters and setting.
P: Hmm, I don't know about continue yet. Want me to learn about it?

U: Change the tone from playful to tragic without changing the plot.
P: Hmm, I don't know about playful yet. Want me to learn about it?
```

### Diagnosi

Nel perimetro senza tool, il limite di open generation è confermato in forma
forte: non vengono generate dieci alternative, non viene mantenuta la
continuità narrativa e non vengono applicati trasformazioni di tono o vincoli
di diversità. Il sistema possiede alcune frasi creative isolate, ma non un
generatore aperto controllabile.

La prova non dimostra però che la creatività sia irraggiungibile con un motore
simbolico. Dimostra che oggi mancano almeno:

```text
seed selection
  -> combinazione lessicale
  -> variazione controllata
  -> valutazione di novità e pertinenza
  -> memoria di continuità
```

## 19.7 One-shot learning e few-shot learning

### Ipotesi del limite

Il progetto distingue l'apprendimento simbolico esplicito dalla generalizzazione
autonoma. Bisogna verificare se Parrot0 può estrarre una regola non solo quando
gli viene nominata la classe, ma da esempi e contrasti.

### Prompt multi-turno

```text
U: Here are two examples: dax means move left; lum means move right.
P: That looks like a snippet of code.

U: What does dax lum dax mean?
P: I don't understand that yet.

U: If I teach you that dax means move left and then say dax, what should you
   do?
P: [produce un template di raccomandazione, senza eseguire la regola]

U: Forget dax.
P: Hmm, I don't know about forget yet. Want me to learn about it?

U: What does dax mean now?
P: I don't understand that yet.
```

### Diagnosi

La prova non invalida la forza già dimostrata in
`tests/p0t/language/taught_lexicon.p0t`: quando l'utente dichiara
`puppo is a universal_quantifier`, l'apprendimento esplicito funziona. Mostra
però il limite più forte enunciato nella sezione 7: dagli esempi non viene
indotta autonomamente una regola generale e il ciclo forget/recall non viene
attivato nella forma naturale.

La formulazione corretta è quindi:

> **one-shot symbolic learning dichiarativo: dimostrato in forme strutturate;
> induction from examples e few-shot generalization: non dimostrate.**

## 19.8 Analogia profonda e transfer strutturale

L'esempio deadlock della sezione 19.5 è anche una sonda di analogia: sostituire
processi/lock con persone/stanze dovrebbe conservare la struttura
`requires -> held_by -> waits_for -> cycle`. Il risultato osservato non trasferisce
la soluzione e degrada in una frase narrativa. Questo non prova che l'analogia
strutturale sia un limite teorico fondamentale; prova che la pipeline proposta in
sezione 6 non è ancora end-to-end:

```text
raw stream -> frame -> graph -> invariant -> target graph -> answer
```

Il test minimo futuro deve fornire dapprima i frame espliciti, poi rimuovere
gradualmente i nomi di dominio. Solo quando il trasferimento riesce con frame
noti si potrà attribuire un eventuale fallimento all'induzione dell'invariante,
non al parsing.

## 19.9 Sintesi dei risultati

| Limite | Esito della prova | Cosa è realmente dimostrato | Prossima isolazione |
|---|---|---|---|
| Knowledge-gap decomposition | fallimento | nessun piano autonomo di acquisizione | insegnare i cue e verificare il motore di gap |
| Ontology emergence | fallimento netto | nessun modello temporaneo di mondo inventato | frame minimo, poi parole inventate |
| Graded reasoning | non dimostrato | template generici non istanziati | estrazione ruoli/evidenze in turni separati |
| Goal-conditioned salience | fallimento misto | goal e log multilinea non mantenuti | goal strutturato con rumore controllato |
| Combinatorial control | non ancora misurabile | la ricerca spesso non si attiva | problema con stato, ipotesi e trace |
| Open generation | fallimento forte | creatività isolata senza controllo | generatori di seed, variazione e ranking |
| Few-shot induction | fallimento nella forma naturale | apprendimento dichiarativo strutturato sì | esempi contrastivi senza nome della regola |
| Analogy transfer | fallimento del percorso end-to-end | nessun transfer osservato | frame espliciti e sostituzione progressiva del dominio |

La conclusione più importante è metodologica: molti presunti breakthrough
limits sono preceduti da un **limite di grounding**. Se una richiesta non viene
riconosciuta, non si può concludere che il sistema non sappia ragionare su di
essa. Il benchmark deve quindi avere una scala:

```text
1. riconoscimento dell'intento
2. estrazione dei ruoli
3. costruzione del modello
4. applicazione della procedura
5. gestione dell'incertezza
6. risposta nel formato richiesto
```

Un punteggio aggregato senza questa scomposizione confonde un vocabolario
mancante con un limite cognitivo e rischia di premiare template che non hanno
compreso il caso concreto.

## 19.10 TODO di discussione e verifica

- [ ] Creare un harness che registri per ogni turno intent, frame, goal,
  evidenze, regole candidate, risposta e motivo del muro.
- [ ] Ripetere ogni protocollo con cue insegnati a runtime, poi ritrarli, per
  separare limite linguistico da limite della facoltà.
- [ ] Usare sessioni senza tool per i benchmark cognitivi e sessioni con
  Wikipedia solo per i benchmark di knowledge-gap.
- [ ] Costruire un benchmark di mondo inventato con asserzioni positive,
  negazioni, cambi di stato, relazioni ternarie e domande controfattuali.
- [ ] Aggiungere una metrica per la risposta fuori dominio, distinta dal
  wall-rate: una risposta estranea deve pesare peggio di un muro.
- [ ] Misurare la conservazione del goal attraverso almeno dieci turni e dopo
  una deviazione di argomento.
- [ ] Misurare se il sistema aggiorna solo le conseguenze dipendenti dal fatto
  modificato, invece di lasciare vecchie conclusioni nella memoria.
- [ ] Per la creatività, misurare novità, pertinenza, diversità, continuità e
  rispetto dei vincoli, non soltanto la presenza di testo non vuoto.
- [ ] Per il controllo combinatorio, esporre il numero di stati considerati,
  potati e riutilizzati, mantenendo la provenienza della soluzione.
- [ ] Non dichiarare risolto un limite sulla base di un template riuscito:
  servono varianti, ablation e trasferimento a un dominio non visto.
