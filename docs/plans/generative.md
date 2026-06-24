# Parrot0 Generative Architecture Notes

## Premessa

Parrot0 non dovrebbe imitare gli LLM.

Gli LLM generano testo a partire da distribuzioni probabilistiche sui token.

Parrot0 può seguire una strada diversa:

* conoscenza simbolica
* inferenza logica
* pianificazione
* composizione di artefatti

L'obiettivo non è generare dal probabile.

L'obiettivo è generare dal conosciuto.

---

# Principio fondamentale

Non:

Truth -> Tokens

Ma:

Knowledge -> Plan -> Expression

---

# Nuovo modulo: Composer

Parrot0 necessita di un nuovo modulo generativo.

Nome suggerito:

composer

Responsabilità:

* riconoscere richieste produttive
* costruire un piano
* interrogare la knowledge base
* produrre un artefatto

Esempio:

Utente:
"Scrivi una newsletter HTML su Parrot0"

Flusso:

1. riconoscimento intento
2. costruzione ArtifactRequest
3. interrogazione KB
4. costruzione piano
5. rendering finale

---

# Artifact Request

Ogni richiesta produttiva viene trasformata in una struttura.

Esempio:

kind = newsletter
format = html
topic = parrot0
tone = professional
audience = developers

---

# Knowledge Driven Generation

La knowledge base diventa sorgente della generazione.

Esempio:

feature(parrot0, pure_c).
feature(parrot0, no_llm).
feature(parrot0, symbolic_reasoning).

Il composer interroga la KB e recupera fatti verificabili.

La KB non produce testo.

Produce conoscenza.

---

# Livelli della generazione

Livello 1

Knowledge Layer

Risponde:

"Cosa è vero?"

---

Livello 2

Planning Layer

Risponde:

"Cosa devo raccontare?"

---

Livello 3

Rhetorical Layer

Risponde:

"In quale ordine lo racconto?"

---

Livello 4

Rendering Layer

Risponde:

"Come lo scrivo?"

---

# Piano retorico

Un artefatto non è testo.

È una struttura.

Esempio newsletter:

header
hero
introduction
sections
call_to_action
footer

Il composer deve prima generare il piano.

Solo dopo il testo.

---

# Grammatica produttiva

Esempio:

NEWSLETTER

-> HEADER
-> INTRO
-> SECTION+
-> CTA
-> FOOTER

BLOG_POST

-> TITLE
-> INTRO
-> BODY
-> CONCLUSION

EMAIL

-> SUBJECT
-> OPENING
-> BODY
-> CLOSING

---

# Temperatura

Negli LLM la temperatura influenza i token.

In Parrot0 dovrebbe influenzare le scelte.

Mai la verità.

Principio:

Temperature affects expression.
Never truth.

---

# Temperature Layer

structure_temp

sceglie schemi alternativi

---

rhetoric_temp

sceglie tono e stile

---

lexical_temp

sceglie parole differenti

---

exploration_temp

esplora piani meno comuni

---

# Esempio

Fatto KB:

feature(parrot0, no_llm).

Temperatura bassa:

"Parrot0 è un agente conversazionale senza LLM."

Temperatura media:

"Parrot0 esplora una strada alternativa ai moderni LLM."

Temperatura alta:

"Parrot0 nasce dall'idea che il dialogo possa emergere da conoscenza simbolica e inferenza logica."

La verità è la stessa.

Cambia solo la forma.

---

# Generazione guidata da prove

Ogni affermazione generata dovrebbe poter essere collegata a:

fact
rule
proof

Esempio:

sentence
-> generated_from
-> feature(parrot0,no_llm)

Questo rende possibile:

* spiegazioni
* audit
* debug
* verificabilità

---

# Coding Agent Evolution

La stessa architettura può essere applicata al codice.

Knowledge Layer:

repository facts

function_defined(...)
function_called(...)
module_depends(...)

---

Planning Layer:

individuazione intervento

fix compiler error
fix test
refactor
generate boilerplate

---

Patch Layer:

genera modifiche

---

Execution Layer:

compila
testa
valida

---

Repair Layer:

analizza fallimento
nuova ipotesi
nuova patch

---

# Visione finale

Parrot0 non deve diventare un piccolo LLM.

Può diventare qualcosa di diverso.

Un sistema composto da:

Knowledge Base
+
Inference Engine
+
Planner
+
Composer
+
Repair Loop

dove la generazione è una conseguenza della conoscenza e non della probabilità.

Formula finale:

Knowledge
-> Inference
-> Plan
-> Composition
-> Artifact

non

Prompt
-> Probability
-> Tokens

Secondo me questa direzione è particolarmente interessante perché si allinea con la tua idea originaria: esplorare fino a che punto un agente conversazionale possa emergere da strutture simboliche, memoria e inferenza, senza dipendere da una rete neurale generativa.

---

# Critica e direzione reale (gen206)

> Lettura critica del piano sopra alla luce di `PRINCIPLES.md`, e di come la
> generazione di codice dovrebbe *davvero* funzionare. La visione è giusta nello
> spirito ma le manca il pezzo che la rende onesta.

## Il difetto: manca la VERIFICA

La formula `Knowledge -> Plan -> Expression` (vs `Prompt -> Probability -> Tokens`)
è suggestiva ma **incompleta**. Tra "Expression" e "artefatto accettato" deve
esserci un passo che il piano non nomina: la **verifica con un oracolo
deterministico**. Senza di esso, "generare dal conosciuto" non è verificabile più
di "generare dal probabile" — è solo un *template engine con i sinonimi*. È
esattamente l'impostore che `PRINCIPLES.md` rifiuta: un sistema che *sembra*
competente perché segue una grammatica fissa, ma non ha nulla che possa
*smentirlo*. "Introspection proposes; the tests dispose" vale anche qui, un piano
più sotto: **il generatore propone, l'oracolo dispone.**

La formula corretta è:

```
Knowledge -> Plan(checkable) -> Synthesis(structure) -> ORACLE -> Repair
```

## Due domini, epistemologie opposte

Il piano mette sullo stesso piano newsletter e codice. Non lo sono.

- **Prosa** (newsletter / blog / email): **non ha verità di base**. La grammatica
  fissa `HEADER -> INTRO -> SECTION+ -> CTA` più la "temperatura" che sceglie
  sinonimi è mimica senza giudice. È la parte **debole** del piano — il rischio
  impostore puro. Va tenuta come gradino *tardo* e *minore*, e anche lì resa onesta
  solo dall'aggancio alle prove (`generated_from -> fact`, sezione "Generazione
  guidata da prove" — l'unica idea della parte prosa che salva il resto).
- **Codice**: **ha un oracolo deterministico** — compila? linka? il test passa? la
  funzione calcola la relazione voluta? Qui "genera dal conosciuto, verificato" ha
  i denti. **Si parte da qui**, non dalla newsletter.

## Come deve funzionare la generazione di codice

Non è sintesi free-form. È **trasformazione di struttura sotto verifica** — la
stessa spina dorsale di `make swe-solve`, generalizzata da "ripara" a "componi":

1. **Knowledge** — fatti AST-as-KB (`code_function`/`code_calls` da `code_ingest`)
   più conoscenza di dominio queryabile (`code_operator/2`). La KB è la sorgente,
   non il testo.
2. **Plan** — non "cosa raccontare" ma un **predicato controllabile**: *"una
   funzione `add(a,b)` che ritorna `a+b`"*, *"questo file compila"*, *"questo test
   passa"*. Il piano è una *condizione di successo*, non una scaletta retorica.
3. **Synthesis** — composizione da **primitive strutturali** (scheletro di firma +
   corpo, `code_replace_expr`, i localizzatori/fixer strutturali `code_symmetry_fix`
   ecc.), **mai** token da una distribuzione. La generazione è **ricerca limitata
   su edit strutturali**, non autoregressione.
4. **Oracle** — `code_eval` / `code_compile` / `code_build` / `code_run` / il test
   reale. **Nessun artefatto è "fatto" finché l'oracolo non lo ha passato.** Questa
   è la regola anti-impostore per la generazione.
5. **Repair** — leggi la diagnostica, nuova ipotesi, nuovo tentativo; **limitato**
   (un cap onesto invece di girare a vuoto).

### La "temperatura", corretta

Il principio "Temperature affects expression, never truth" è giusto ma va affilato:
nel codice la temperatura è **quale struttura-candidata provare per prima** (ordine
di ricerca sullo spazio dei piani/edit). **L'oracolo, non la temperatura, è il
filtro di verità.** Una temperatura senza oracolo è solo drift non falsificabile.

## Cosa è già realizzato (il primo gradino onesto)

`mod_compose` (gen206, `src/brain.c`, gated `PARROT0_TOOLS=1`) è la più piccola
istanza *verificata* di tutto questo:

- **Knowledge-first**: legge l'operatore da `kb/experts/programming/compose.p0`
  (`code_operator/2`, caricato pigramente nel layer riflessivo solo in agent mode —
  non inquina la conversazione né il boot KB), non da una tabella in C.
- **Synthesis**: compone `int NAME(int a, int b) { return a OP b; }` dalla struttura.
- **Oracle**: verifica con `code_eval` (esecuzione simbolica) su input campione e
  riporta il codice **solo se** calcola davvero la relazione voluta.
- **Anti-impostore**: ciò che non sa comporre-e-verificare lo **rifiuta onestamente**
  ("the next generative faculty to pull"), non lo allucina.

Coperto da `make piagent-bench`: `gen-sum`, `gen-product`, `gen-difference` (PASS) e
`gen-gap-honest` (il rifiuto onesto è esso stesso un gate).

## Compiti articolati (gen207) — il planner sequenziale

La generazione reale non è mai un singolo atto: un prompt vero è **articolato e
lungo** — *"scrivi `add`, **e poi** usala per calcolare `add(3,4)`, **e poi** scrivi
anche la **variante** `mul`"*. Qui la formula `Plan → Synthesis → Oracle` diventa un
**piano sequenziale**: parrot0 spezza il prompt in sotto-obiettivi **ordinati** (sui
connettori forti `and then` / `after that` / `also` / `then` / `e poi` / `dopo` /
`infine` — **mai** sul debole ` and ` che vive dentro `a and b`), esegue ogni passo
con le stesse facoltà compose/verify/eval, e **infila gli artefatti** tra i passi: il
passo che dice "usala per calcolare `add(3,4)`" valuta (`code_eval`) **la funzione che
il passo precedente ha davvero composto**. Ogni passo è controllato dall'oracolo; la
risposta è una trascrizione numerata e fondata. Un passo non ancora supportato (es.
"ordina un array") viene **rifiutato onestamente** mentre gli altri proseguono — niente
corpo allucinato. Coperto da `articulated-write-use-variant`, `articulated-four-steps`,
`articulated-partial-honest` in `make piagent-bench`. È il primo gradino verso il
Repair Layer: un piano che esegue passo-passo è anche il punto dove, in futuro, un
passo fallito può innescare una nuova ipotesi invece di fermarsi.

## La scala dei prossimi pull (ognuno oracle-gated)

1. funzione con control-flow da spec, verificata da `code_run` su esempi I/O;
2. **completare il corpo di una funzione per far passare un test che fallisce** — la
   spina di `swe-solve` generalizzata da fix a sintesi;
3. boilerplate verificato da `code_build` (linka);
4. solo dopo, e solo se agganciata alle prove, la generazione di **prosa**.

Regola che tiene insieme tutto: **parrot0 non consegna codice che non ha fatto
passare a un oracolo.** È la stessa disciplina del resto del progetto, un livello più
in profondità — dove la generazione è conseguenza della conoscenza *verificata*, non
della probabilità.
