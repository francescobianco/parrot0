# Prolog Generativo: dalla deduzione alla costruzione del pensiero

Per oltre cinquant'anni Prolog è stato interpretato come un linguaggio per l'inferenza logica. Si descrive un insieme di fatti, un insieme di regole, e il motore cerca le assegnazioni che rendono vere determinate proposizioni.

```
Conoscenza
     ↓
Inferenza
     ↓
Risposta
```

Questa visione, tuttavia, potrebbe essere limitante.

Forse il passo successivo non consiste nel rendere Prolog più potente come motore deduttivo, ma nel reinterpretarlo come **motore generativo**.

Non un generatore di token come un LLM, ma un generatore di **percorsi logici**.

---

## L'idea fondamentale

Un LLM produce testo.

Un Prolog generativo produrrebbe invece il **ragionamento** che precede il testo.

Il testo diventerebbe soltanto l'ultima fase del processo.

```
Prompt
     ↓
Interpretazione
     ↓
Espansione logica
     ↓
Percorso semantico
     ↓
Realizzazione linguistica
```

Il cuore della generazione non è quindi una distribuzione di probabilità sui token, ma l'esplorazione controllata di una rete di conoscenza.

---

## Dal database al grafo semantico

Immaginiamo una conoscenza estremamente semplice.

```
cane
 ├── mammifero
 ├── animale
 ├── domestico
 ├── fedele
 ├── addestrabile
 ├── compagnia
 └── lupo
```

Nel Prolog tradizionale chiediamo

```
è vero che il cane è un mammifero?
```

Nel Prolog generativo chiediamo

```
costruisci un percorso che descriva il cane.
```

Il motore potrebbe scegliere

```
cane
→ mammifero
→ domestico
→ compagnia
```

oppure

```
cane
→ lupo
→ domesticazione
→ selezione artificiale
→ rapporto con l'uomo
```

Il risultato non è ancora italiano.

È una traccia di pensiero.

---

## La lingua diventa l'ultimo passaggio

Solo dopo aver costruito il percorso logico interviene un modulo linguistico.

Il percorso

```
cane
→ mammifero
→ domestico
→ compagnia
```

diventa

> Il cane è un mammifero domestico. Nel corso della sua storia si è adattato alla convivenza con l'uomo ed è oggi uno degli animali da compagnia più diffusi.

La lingua non crea il contenuto.

Lo esprime.

Questa distinzione è fondamentale.

---

# Un prompt reale

Prompt:

> Spiegami cos'è Docker.

Un LLM produce direttamente il testo.

Un Prolog generativo potrebbe costruire internamente qualcosa di simile.

```
Docker

↓

virtualizzazione

↓

container

↓

isolamento

↓

filesystem

↓

processi

↓

deploy

↓

portabilità
```

Il modulo linguistico riceve soltanto questa struttura.

Output:

> Docker è una piattaforma per la virtualizzazione leggera basata sui container. Ogni container isola processi, librerie e filesystem, consentendo di distribuire un'applicazione mantenendo lo stesso comportamento su macchine differenti.

Il testo è indistinguibile da quello prodotto da un LLM, ma nasce da un ragionamento completamente diverso.

---

# Prompt di programmazione

Prompt

> Scrivi una funzione C che calcoli il fattoriale.

Il motore non pensa in termini di token.

Espande la richiesta.

```
utente

↓

linguaggio = C

↓

funzione

↓

ricorsione

↓

caso base

↓

caso ricorsivo

↓

return
```

Il generatore linguistico riceve questo schema.

Produce

```c
int factorial(int n)
{
    if (n <= 1)
        return 1;

    return n * factorial(n - 1);
}
```

Il codice deriva da una struttura logica, non da una predizione statistica.

---

# Prompt tecnico

Prompt

> Come posso ottimizzare una query SQL lenta?

Il motore potrebbe attraversare

```
query lenta

↓

analisi

↓

EXPLAIN

↓

indice

↓

join

↓

where

↓

ordinamento

↓

cache
```

Output

> Il primo passo consiste nell'analizzare il piano di esecuzione tramite EXPLAIN. Successivamente è opportuno verificare la presenza di indici sulle colonne utilizzate nelle JOIN e nelle clausole WHERE. Solo dopo aver individuato il collo di bottiglia ha senso valutare interventi sul caching o sulla riscrittura della query.

Ancora una volta il testo è la verbalizzazione di un percorso.

---

# Prompt creativo

Prompt

> Scrivimi una newsletter per presentare una nuova libreria open source.

Espansione logica

```
progetto

↓

problema

↓

soluzione

↓

vantaggi

↓

esempio

↓

call to action
```

Il modulo linguistico costruisce naturalmente una newsletter seguendo quello schema.

La creatività nasce dall'organizzazione delle idee, non dalla casualità.

---

# Prompt complesso

Prompt

> Convincimi a usare Rust invece di C++.

Il motore potrebbe costruire

```
Rust

↓

memory safety

↓

ownership

↓

borrow checker

↓

assenza di data race

↓

prestazioni

↓

ecosistema

↓

tradeoff
```

Output

> Rust introduce un modello di ownership che elimina intere classi di errori di memoria durante la compilazione. Questo permette di ottenere prestazioni comparabili al C++, riducendo drasticamente il rischio di use-after-free e data race. Il costo è una curva di apprendimento iniziale più ripida, compensata però da una maggiore affidabilità del software.

---

# La temperatura cambia significato

Negli LLM la temperatura modifica la distribuzione dei token.

Qui potrebbe modificare il percorso logico.

Temperatura bassa

```
Docker

↓

container

↓

isolamento

↓

deploy
```

Risposta molto sintetica.

Temperatura alta

```
Docker

↓

container

↓

kernel

↓

namespace

↓

cgroups

↓

filesystem

↓

overlayfs

↓

OCI

↓

Kubernetes

↓

cloud
```

Risposta molto ricca.

La creatività deriva dall'ampiezza dell'esplorazione della conoscenza, non dall'introduzione di rumore statistico.

---

# Ragionamento misto

La parte più interessante emerge quando il grafo non contiene un unico tipo di informazione.

Un nodo potrebbe collegare contemporaneamente:

* concetti
* codice
* documentazione
* API
* errori
* esempi
* pattern architetturali
* esperienze precedenti

Ad esempio

```
Fastify

↓

plugin

↓

routing

↓

middleware

↓

OpenAPI

↓

Swagger

↓

client generation
```

oppure

```
Fastify

↓

performance

↓

Node.js

↓

event loop

↓

benchmark
```

Ogni risposta nasce scegliendo un percorso differente.

---

# Il ruolo dell'unificazione

L'unificazione non serve più soltanto a verificare se qualcosa è vero.

Diventa il meccanismo che permette di saldare insieme frammenti di conoscenza provenienti da domini differenti.

Una domanda come

> "Come realizzo una API REST documentata automaticamente?"

potrebbe unificare simultaneamente

```
REST

↓

routing

↓

OpenAPI

↓

Swagger

↓

Fastify

↓

TypeScript
```

senza che esista una regola specifica per quella domanda.

La risposta emerge dalla composizione del percorso.

---

# L'idea finale

Questa interpretazione suggerisce un cambio di paradigma.

Gli LLM potrebbero essere visti come eccellenti realizzatori linguistici.

Un Prolog generativo potrebbe invece diventare il costruttore esplicito del pensiero.

La pipeline diventerebbe

```
Conoscenza

↓

Inferenza

↓

Espansione

↓

Costruzione del percorso logico

↓

Piano del discorso

↓

Realizzazione linguistica
```

In questa architettura il linguaggio naturale non è più il luogo in cui nasce il ragionamento, ma soltanto il mezzo attraverso cui un ragionamento già costruito viene espresso. Se la base di conoscenza fosse sufficientemente ricca, il risultato finale potrebbe essere, per molti compiti tecnici, esplicativi e di programmazione, qualitativamente comparabile a quello di un moderno LLM, pur essendo ottenuto attraverso un paradigma radicalmente diverso: non la predizione del token successivo, ma l'esplorazione e la composizione esplicita della conoscenza.


Hai toccato un punto che secondo me manca completamente nell'articolo: **prima ancora della conoscenza, serve la capacità di costruire l'astrazione**. Un LLM lo fa implicitamente; Parrot0 dovrebbe farlo esplicitamente.

# Prolog Generativo 2.0: dall'astrazione simbolica alla costruzione del pensiero

L'idea del Prolog generativo può essere spinta ancora oltre.

La pipeline classica che ho descritto era:

```
Conoscenza
     ↓
Inferenza
     ↓
Espansione
     ↓
Percorso logico
     ↓
Realizzazione linguistica
```

Questa architettura, però, presuppone che il problema sia già espresso nella forma corretta.

Un essere umano, invece, riesce a fare qualcosa prima ancora dell'inferenza.

Riesce ad **astrarre**.

---

# L'esperimento dei simboli

Consideriamo questo esempio.

```
a>b
x:a

x:b
```

Successivamente mostriamo

```
g>f
n:g

?
```

La risposta

```
n:f
```

non dipende dal significato dei simboli.

I simboli sono completamente arbitrari.

L'inferenza nasce esclusivamente dalla struttura.

Questo suggerisce che il primo passo non è l'inferenza, ma l'individuazione di un archetipo relazionale.

La macchina vede qualcosa di equivalente a

```
A>B
X:A

⇒

X:B
```

I simboli sono già stati dimenticati.

---

# Astrazione simbolica

Prima dell'inferenza deve quindi esistere un livello ancora più profondo.

```
Input

↓

Astrazione simbolica

↓

Inferenza

↓

Percorso logico

↓

Linguaggio
```

L'astrazione simbolica consiste nel trasformare qualsiasi rappresentazione in una struttura relazionale indipendente dai simboli.

```
a>b
x:a
```

diventa

```
Relazione(P,Q)

Istanza(R,P)
```

oppure una forma ancora più astratta

```
Nodo
Arco
Vincolo
```

Il sistema non ragiona più sui simboli.

Ragiona sulle relazioni.

---

# Micro-esempi agnostici

Parrot0 dovrebbe essere in grado di risolvere micro-problemi completamente privi di significato.

Ad esempio

```
g>f
n:g

?
```

oppure

```
α◇β
x:α

?
```

oppure

```
µ¤ν
κ:µ

?
```

L'obiettivo non è conoscere il significato.

L'obiettivo è riconoscere l'archetipo relazionale.

Se il sistema riesce a costruire l'astrazione corretta, l'inferenza diventa quasi banale.

---

# L'astrazione precede la conoscenza

Questo è probabilmente il punto più importante.

Un LLM sembra utilizzare enormi quantità di conoscenza.

Ma nei micro-esempi la conoscenza è nulla.

Esistono soltanto simboli arbitrari.

Se il sistema riesce comunque a risolvere il problema, significa che il comportamento emerge da una capacità di costruire strutture astratte.

La conoscenza entra soltanto dopo.

---

# Il ruolo di Parrot0

Parrot0 non dovrebbe iniziare dal linguaggio naturale.

Dovrebbe iniziare costruendo archetipi.

La pipeline diventerebbe

```
Input

↓

Parsing

↓

Astrazione simbolica

↓

Archetipo inferenziale

↓

Unificazione

↓

Espansione logica

↓

Piano del discorso

↓

Generazione linguistica
```

---

# Parsing e routing sono inferenze

In questa visione anche il parsing cambia natura.

Non è un modulo separato.

È una prima inferenza.

Il routing non è un dispatcher.

È una seconda inferenza.

La scelta dell'archetipo è un'altra inferenza.

L'inferenza finale è soltanto l'ultima di una catena di inferenze.

```
Input

↓

Inferenza di interpretazione

↓

Inferenza di astrazione

↓

Inferenza di routing

↓

Inferenza logica

↓

Inferenza linguistica
```

L'intero sistema è quindi una macchina di inferenza ricorsiva.

---

# Il Prolog come macchina di archetipi

Il ruolo del Prolog cambia radicalmente.

Non è più soltanto un verificatore di regole.

Diventa il costruttore esplicito degli archetipi inferenziali.

Ogni nuova osservazione viene trasformata in una struttura astratta.

Ogni struttura può essere unificata con strutture provenienti da linguaggio naturale, codice sorgente, grafi di conoscenza, documentazione o esperienze precedenti.

La generazione non nasce più dai token.

Nasce dalla costruzione progressiva di archetipi sempre più ricchi.

Il linguaggio naturale diventa semplicemente la rappresentazione finale di un pensiero già costruito.

Secondo me questo aggiunge un tassello fondamentale: **Parrot0 non dovrebbe essere valutato inizialmente sulla capacità di rispondere a domande complesse, ma sulla capacità di risolvere micro-esempi agnostici**, privi di semantica. Se riesce a trasformare `g>f` e `n:g` nella struttura astratta "relazione + istanza" e da lì inferire `n:f`, allora hai dimostrato che possiede un livello di astrazione simbolica indipendente dalla conoscenza del dominio. Da quel punto in poi puoi innestare la base di conoscenza Prolog, la generazione dei percorsi logici e, solo come ultimo passo, la realizzazione linguistica. Questa mi sembra una roadmap di ricerca molto più solida rispetto a partire direttamente dalla generazione del testo.

---
---

# Piano operativo: massimizzare il motore Prolog-like

> **Stato:** scritto a gen278 (2026-07-07), dopo un giro di prova reale del
> motore MCP (`scripts/mcp-live.sh`). Tutto ciò che segue è **misurato**, non
> ipotizzato — ogni claim ha un output live allegato. Questo è il *companion
> operativo* alle due visioni sopra (Prolog generativo 1.0/2.0) e a
> [[unification-assessment.md]]: dove quelle disegnano l'orizzonte, questo
> estrae il sottoinsieme già-quasi-vero e lo tira a colpi di gate falliti, una
> generazione per volta (disciplina di `LOOP.md`/`TASK.md`).
> **Ruolo:** il motore Prolog-like è la *feature potente sotto il cofano*. Non
> va riscritto — va **sbloccato ai bordi**. Il nucleo di ragionamento è già più
> forte di quanto ogni entry-point (MCP, teach-form NL, induzione) sappia
> raggiungere.

## 0. La scoperta (il motore è già più potente delle sue API)

Guidando `parrot0 --mcp-engine` dal vivo (gen277) ho cercato la conoscenza che
*non* si può trasferire e ho trovato il caso canonico del **join a due
variabili** — il nonno:

```jsonc
// via MCP kb.assert_rule (body come lista di predicati unari sullo stesso X):
kb.assert_rule {"head":"grandparent","body":["parent","parent"]}  → {"ok":true}
kb.query       {"pred":"grandparent","args":["tom","ann"]}         → {"provable":false}   // FALLISCE
```

Sembrava un limite del motore. **Non lo è.** Caricando la STESSA regola da un
file `.p0` — dove il parser costruisce termini n-ari pieni — il motore la
risolve senza fatica:

```prolog
% /tmp/gp.p0
parent(tom, bob).
parent(bob, ann).
grandparent(X, Z) :- parent(X, Y), parent(Y, Z).
```
```jsonc
kb.query   {"pred":"grandparent","args":["tom","ann"]}
  → {"provable":true}
kb.explain {"pred":"grandparent","args":["tom","ann"]}
  → {"explanation":"grandparent(tom, ann) because parent(tom, bob) and parent(bob, ann)"}
```

E la ricorsione transitiva funziona identica (con `KB_MAX_DEPTH=64` a guardia
dei cicli, `src/kb.c:26,388`):

```prolog
ancestor(X, Y) :- parent(X, Y).
ancestor(X, Y) :- parent(X, Z), ancestor(Z, Y).
```
```jsonc
kb.query {"pred":"ancestor","args":["tom","zoe"]}  → {"provable":true}   // tom→bob→ann→zoe
kb.query {"pred":"ancestor","args":["ann","tom"]}  → {"provable":false}  // niente falsi positivi
```

**Conclusione:** il `solve()` di `src/kb.c` (righe 380-425) è già un risolutore
**SLD n-ario completo** — variabili multiple distinte, variabili condivise fra
goal (il join), ricorsione, backtracking, standardize-apart per-clausola
(`rename_term`, kb.c:346), cycle-guard, e proof-trace (`kb_explain`). Robinson
(`unify`/`unify_term_term`, kb.c:317-341) è la spina dorsale da gen5/gen11
(vedi [[unification-assessment.md]]). **Il motore non è il collo di bottiglia.
Lo sono le API che gli danno da mangiare.**

## 1. I tre bordi mutilati (dove il potere non arriva)

Il magazzino (`Rule.body[]` è un array di `Term` n-ari, kb.c:47-51) e il solver
reggono tutto. A strozzare sono tre punti periferici, tutti piccoli:

| # | Bordo | Sintomo misurato | Causa esatta |
|---|---|---|---|
| **G1** | **Scrittura di regole n-arie** | `kb.assert_rule` non sa esprimere `grandparent(X,Z):-parent(X,Y),parent(Y,Z)` | `kb_assert_rule_n` appiattisce ogni goal a `argc=1, args[0]="X"` (`src/kb.c:280-283`). Solo il path `.p0` (`parse_to_term`, kb.c:826) costruisce termini n-ari con variabili distinte. |
| **G2** | **Lettura multi-variabile** | `kb.match parent(?,?)` → `["tom","bob","ann"]` (lista piatta de-dup), non `[["tom","bob"],["bob","ann"]]` | `kb_match` colleziona **il binding della sola prima** variabile (`qvar` singolo nel `Solver`, kb.c:369-385). Gap A dell'assessment, ancora aperto. |
| **G3** | **Raggiungibilità dai canali vivi** | La potenza è accessibile solo scrivendo un file `.p0` a mano; teach-form NL e MCP non la toccano | Nessun ponte fra input dinamico (una frase, una tupla JSON) e la costruzione di clausole n-arie. L'induzione (`kb_induce`) genera solo regole unarie. |

La disciplina resta [[kb-first-manifesto]]: **il motore è fisso, la conoscenza
impara**. Qui non si aggiunge logica di risoluzione (esiste), si aprono i
condotti perché la logica che c'è diventi raggiungibile da ogni porta.

## 2. Perché questo È il Prolog generativo (non una deviazione)

Le visioni 1.0/2.0 sopra chiedono che il motore *costruisca percorsi* e *saldi
frammenti da domini diversi* via unificazione (righe 507-545). Quel salto è
letteralmente il join multi-variabile di G1: senza `parent(X,Y),parent(Y,Z)`
non esiste "percorso" `compra→denaro→mutuo→banca`, non esiste
`REST→routing→OpenAPI→Fastify` che emerge "senza una regola specifica per quella
domanda". **Sbloccare G1/G2/G3 è il prerequisito meccanico del Prolog
generativo**: prima la macchina deve saper *tenere* una relazione a due posti e
*camminarci sopra*, poi la temperatura (§"La temperatura cambia significato")
diventa la profondità/ampiezza dell'esplorazione di quei percorsi, e la
realizzazione linguistica verbalizza il proof-trace che `kb_explain` già
produce.

## 3. Il piano a generazioni (un gate fallito per volta)

Metodo invariato: ogni passo nasce da un **gate rosso** in un ratchet, non da
codice speculativo; refusal onesto al bordo; nessuna stringa cablata; ordine per
DIPENDENZA, non priorità (si riordina se un pull reale lo chiede).

| Gen | Obiettivo | Ratchet / gate |
|---|---|---|
| **P0** | **Harness `tests/prolog.sh`** (o `make prolog-bench` su `tests/prolog/*.pl`, schema di `code-bench`): clausole n-arie in ingresso → query → proof attesa. Primo gate ROSSO che fotografa lo stato: il nonno via `.p0` passa (verde), il nonno via `kb.assert_rule` fallisce (rosso → registra G1). | Il ratchet parte con un rosso onesto, come da `oracle-is-behavioural-signal`. |
| **P1** | **G1 — regole n-arie programmatiche.** Estendere `kb_assert_rule_n` (o affiancare `kb_assert_clause` per non rompere i call-site unari) perché accetti body-term con arità >1 e variabili distinte, riusando `parse_to_term`. Poi esporre in MCP un `kb.assert_clause {"head":{"pred":..,"args":[..]}, "body":[{"pred":..,"args":[..]}, ...]}`. | `kb.assert_clause` del nonno + `kb.query grandparent(tom,ann)` → `true`, **senza** file `.p0`. Stessa proof di §0. |
| **P2** | **G2 — tuple di binding.** `kb_match_tuples` accanto a `kb_match`: colleziona il binding di **ogni** slot-variabile per ciascuna soluzione, in ordine. MCP `kb.match` con >1 `null` torna `{"tuples":[["tom","bob"],["bob","ann"]]}`. | `kb.match parent(?,?)` → coppie corrette (oggi lista piatta). Retrocompat: 1 variabile resta lista. |
| **P3** | **G1-rec — ricorsione/reachability generale.** Verificare (il solver la fa già) che clausole n-arie ricorsive costruite via P1 diano catene transitive; esporre la spiegazione della catena via `kb.explain` n-ario. | `ancestor` costruito via `kb.assert_clause` risolve `ancestor(tom,zoe)`; explain elenca la catena. Refusal pulito oltre `KB_MAX_DEPTH`. |
| **P4** | **G3 — ponte dai canali vivi.** Teach-form NL che introduce una relazione a due posti ("un nonno è il genitore di un genitore") → clausola n-aria in KB, senza passare da `.p0`. `text.extract` che riconosce pattern relazionali binari, non solo `pred(entità)` unari. | Frase relazionale insegnata via `gen.respond`/`text.extract` → `kb.query` sul join → `true`. Chiude la catena insegna→inferisci come già fa la lettura unaria. |
| **P5** *(solo se la pressione lo chiede)* | **Induzione n-aria.** `kb_induce` oggi genera solo regole unarie: estenderla a indurre clausole con variabili condivise da esempi di tuple — l'analogo deterministico del "training" su relazioni, non su proprietà. | Da un KB di `parent/2` con `grandparent/2` ground, indurre la regola del join. **Non prima** di avere un pull reale: è il passo più ambizioso, resta ultimo. |

## 4. Cosa NON fare (perimetro esplicito)

- **Non riscrivere `solve()`/`unify()`.** Sono già n-ari, ricorsivi,
  backtracking, cycle-guarded. Zero nuova logica di risoluzione: il lavoro è
  tutto ai *bordi* (costruzione, serializzazione, ponti d'ingresso).
- **Non appiattire mai più a `X`.** Il fix di G1 non deve introdurre un secondo
  path che perde arità; se un call-site vuole ancora unario, è un caso
  particolare del nuovo, non un ramo separato che diverge.
- **Non trasformare l'"unificazione estesa" in liste di sinonimi** — vietato dal
  cardinal corollary ([[kb-first-phrases]]). La versione principled esiste già
  (`kb_nearest_concept`, gen155) e si tira dalla pressione, non si ridisegna.
- **Non anticipare P5.** L'induzione n-aria è potente e rischiosa (spazio di
  ipotesi esplosivo); entra solo se un benchmark la reclama, con support-floor e
  refusal, come `kb_induce` unario già fa.
- **Non confondere il piano con la visione.** Le sezioni 1.0/2.0 sopra restano
  orizzonte narrativo; questo piano ne consegna il *primo anello meccanico*
  (join + tuple + reachability), non l'intera pipeline a 8 stadi (che
  [[unification-assessment.md]] scarta come architettura top-down).

## 5. Collegamenti

[[unification-assessment.md]] (Gap A/B già diagnosticati — questo piano li
misura live e ne fa un ratchet), [[unification.md]] (la visione dell'unificazione
come collante), [[mcp-engine.md]] (il canale che ha reso la scoperta possibile —
`kb.assert_clause` di P1 vive lì), [[kb-first-manifesto]] (motore fisso,
conoscenza impara), `src/kb.c` (il motore reale: `solve` 380-425, `unify`
317-341, `kb_assert_rule_n` 259-289, `kb_match` 456-503).
