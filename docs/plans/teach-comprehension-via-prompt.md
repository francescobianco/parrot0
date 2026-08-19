# Insegnare la comprensione a parrot0 PARLANDOGLI — quadro e piano

> **Stato:** scritto a gen428 (2026-08-19), con misura viva: ogni riga di
> «distanza» qui sotto ha un turno reale allegato, non una stima. Gemello di
> [[teach-comprehension-via-mcp]] — **stesso quadro, altro canale**: là si
> insegna scrivendo nella KB con `kb.*`, qui si insegna **dicendolo**.
> **Ruolo:** capire che cosa cambia quando il canale non è un'API ma una
> conversazione, e perché è proprio quel cambio a decidere se il progetto resta
> KB-first o scivola nel frasario.

## 0. Il concetto (F., 2026-08-19)

> «Tutto deve essere insegnabile, anche la formattazione dell'ora con i due
> punti; e da adesso in poi al posto del simbolo percentuale PERC per le
> percentuali — e lui, usando i modi ipotetici, farà tutto.»

Riformulato come tesi verificabile: **ciò che parrot0 sa fare con una forma —
riconoscerla, nominarla, tradurla — deve poter cambiare per effetto di una FRASE
detta in conversazione, non solo di un file modificato o di una chiamata MCP.**

E la contro-tesi, che è la ragione per cui questo documento esiste: se ogni
frase-che-insegna richiede un pezzo di C che la riconosca, il canale-dialogo
diventa **la via più veloce per riempire il progetto di frasario**. Un canale che
si allarga a colpi di modulo non è insegnabile: è cablato con più passaggi.

### Perché il canale cambia il problema

Via MCP il fatto si scrive: `kb.assert {"pred":"notation_of", …}` va a segno per
costruzione. Parlando, prima di scrivere bisogna **essere capiti**, e questo
aggiunge un requisito che l'API non ha:

| | via MCP | parlando |
|---|---|---|
| chi decide la forma del fatto | chi chiama | **chi ascolta** |
| che cosa serve perché funzioni | il predicato esista | la **frase** sia una forma riconosciuta |
| dove si nasconde il debito | nella tipizzazione dei tool | **nell'inventario delle superfici** |
| come si rompe male | un atomo corrotto | un fatto **falso** annunciato come appreso |

Il debito quindi si sposta, non sparisce: **la domanda non è più «il motore sa
rappresentarlo?» ma «la frase che lo dice è una forma dichiarata?»**. È una
domanda migliore, perché ha una risposta KB-first: una forma è un fatto.

## 1. Quello che si insegna GIÀ parlando (misurato a gen428)

Ogni riga è un turno reale, con la risposta che parrot0 dà oggi.

| Che cosa si insegna | Come glielo si dice | Risposta |
|---|---|---|
| un fatto di classe | `rex is a dog` | `Learned: dog(rex).` |
| un **universale** (una regola!) | `whales are mammals` | `Learned rule: mammal(X) :- whale(X).` |
| una relazione binaria, verso «fatto» | `bezra is the capital of nivora` | `Learned: capital_of(bezra, nivora).` |
| una relazione binaria, verso **«valore»** | `the capital of nivora is bezra` | `Learned: capital_of(nivora, bezra).` |
| un alias | `nivora is known as the pearl` | `Learned: also_known_as(nivora, pearl).` |
| un fatto al passato | `zorak was born in nivora` | `Learned: born_in(zorak, nivora).` |
| una **risposta da dare** | `when i say bonjour answer hello there` → `bonjour` | `hello there` |
| una **NOTAZIONE** | `the notation of percent is perc` → `50perc` | `50perc is a percentage — 50 out of a hundred.` |
| la notazione dell'**ora** | `the notation of time is h` → `14h30` | `14h30 is a time of day` |

Le ultime tre righe sono il gen428 e sono la risposta alla richiesta di F.: la
formattazione dell'ora e il simbolo di percentuale **sono conoscenza**, si
cambiano parlando, e nessuna riga di C sa che esistano.

**Come, esattamente** — perché il «come» è la parte riutilizzabile:

1. il simbolo di una forma non sta dentro la regola della forma, sta in un fatto
   suo: `notation_of(percent, "%")`, `notation_of(time, ":")`;
2. la frase che dice un valore è un `extract_frame/2`, che il motore di
   estrazione (gen382) già sa applicare — e non serve una riga per relazione,
   perché una **regola** costruisce il pattern da una dichiarazione compatta:

```prolog
extract_frame($Pat, $Pred) :-
    relation_noun($Pred, $Noun),
    concat_atoms("the ", $Noun, $A),
    concat_atoms($A, " of @S is @O", $Pat).

relation_noun(capital_of, "capital").
relation_noun(notation_of, "notation").      % ← la notazione è una relazione come le altre
```

3. quindi **insegnare a parrot0 una relazione nuova costa un fatto**:
   `relation_noun(mayor_of, "mayor")` e da quel momento *«the mayor of nivora is
   zorak»* è un turno che capisce.

Nessun modulo nuovo, nessuna riga di C: il gen428 ha aggiunto **zero** moduli.

## 2. La distanza reale, per natura del gap

### P-A — l'inventario delle superfici
*(chiudibile, e ogni chiusura costa un fatto)*

Il gap più frequente non è una facoltà mancante: è una **formulazione non
dichiarata**. Misurato due volte oggi:

```
> the mayor of nivora is zorak        → Hmm, I don't know about nivora yet.
                                        (manca relation_noun(mayor_of, "mayor"))
> when i say bonjour answer hello     → non capito, prima del gen428
                                        (mancava intent_cue(teach_reply, "when i say"))
```

In entrambi i casi la facoltà c'era **tutta**, e mancava la riga che dice come si
chiama. È il secchio buono: si chiude una riga per volta, e chi la chiude non
tocca il motore.

**Trappola, e va detta:** questo è anche il secchio in cui il frasario entra
travestito. La differenza fra una *forma* e una *frase* è che la forma ha **slot**
e generalizza (`"the @R of @S is @O"` vale per ogni relazione dichiarata),
mentre una frase copre solo sé stessa. Regola operativa: **se la riga nuova non
serve nessun turno che non sia quello che l'ha motivata, è frasario.**

### P-B — quello che a voce non si sa ancora dire
*(la vera differenza col canale MCP)*

Via MCP `kb.assert_clause` scrive una clausola definita qualunque, con join a più
variabili. Parlando si insegna oggi **una sola forma di regola** — l'universale
plurale (`whales are mammals`). Tutto il resto della logica — condizioni
multiple, congiunzioni, eccezioni — non ha una superficie:

```
> a shark is a fish that swims fast   → I don't understand that yet.
```

Non è un muro di principio (la KB lo rappresenta benissimo, l'MCP lo scrive): è
un **buco di superficie con una struttura dietro**, e va affrontato come tale —
prima le forme condizionali che il corpus reclama, non tutte insieme.

### P-C — le procedure, e il misclaim che ci sta sotto
*(il secchio pericoloso)*

Insegnare un **procedimento** parlando oggi non solo non funziona: produce la
cosa peggiore, una risposta che finge.

```
> to convert celsius to fahrenheit multiply by 2 and add 30   →   -16.6667.
```

parrot0 ha **eseguito** la frase che voleva insegnargli una procedura. Non ha
capito e non l'ha detto: ha calcolato qualcosa. È il mantra #7 violato dal canale
stesso — e il rischio è strutturale, perché una frase che insegna un calcolo
*somiglia* a una frase che chiede un calcolo. Vedi [[teachable-procedures]] per
la rappresentazione; qui serve prima **la guardia**: una frase con un cue di
insegnamento non deve poter essere eseguita come richiesta.

### P-D — la forma della classe, e un rifiuto onesto
*(il secchio che funziona già bene)*

```
> ahoy is a greeting            → Learned: greeting(ahoy).      ← ma non cambia niente
> greetings are social markers  → I know social_marker as a 2-part fact, not a class,
                                  so I can't put something in it that way.
```

La seconda riga è parrot0 che **rifiuta onestamente** invece di scrivere un fatto
storto: sa che `social_marker/2` non è una classe unaria. La prima invece passa e
non serve a nulla — `greeting(ahoy)` non è la relazione che il sociale legge.
Gap: manca il **ponte fra il nome comune di una classe e il predicato che la
implementa**, che è lo stesso ponte del gen428 per i generi
(`genre_name(percent, "percentage")`), da generalizzare.

### P-E — i valori che sono NUMERI
*(scoperto misurando, non previsto)*

```
> the capital of nivora is bezra   → Learned: capital_of(nivora, bezra).
> the capital of nivora is 42      → Hmm, I don't know about capital yet.
> the population of nivora is large  → Learned: population_of(nivora, large).
> the population of nivora is 40000  → (non impara)
```

La stessa forma, con un numero al posto della parola, **non estrae**. È esattamente
la metà dei valori che uno vorrebbe insegnare (popolazioni, anni, prezzi, quote),
e finora nessuno se n'era accorto perché nessun test lo chiedeva.

## 3. I tre reperti del gen428

Vale la pena isolarli, perché sono **specie ricorrenti**, non incidenti:

**1. Conoscenza morta.** Due `extract_frame` dichiarati (`"@S was born in @O"`,
`"@S was founded in @O"`) non potevano combaciare **mai**: la normalizzazione
della copula riscriveva «was» in «is» prima che i frame la vedessero. Stessa
specie della riga `currency_char("£")` del gen427, che c'era e non poteva
funzionare perché il confronto era su un carattere solo e «£» ne occupa due.
> **Un fatto che non combacia non si lamenta.** L'unico modo di accorgersene è
> provare la conoscenza dichiarata, non leggerla.

**2. Collisione fra ruoli di una stessa parola.** «say» era insieme parte della
formulazione (*when i **say** X*) e separatore (*… **say** Y*), quindi il taglio
cadeva dentro la formulazione e la situazione spariva. La riparazione non è stata
una guardia sul caso: **il separatore si cerca dopo la formulazione**, e l'intera
classe di collisioni sparisce.

**3. Il cablato che precede il dichiarato.** L'estrattore di creazione leggeva
*«percent is written as pct»* come una paternità e ne faceva
`created_by(percent_is, as_pct, wrote)` — un fatto **falso**, scritto in KB e
annunciato come appreso. Il commento del file dichiarava dal gen382 che i frame
KB corrono per primi; non era vero. Ora lo è.

## 4. Piano per gate (uno per generazione, gate rosso → passo)

| Gate | Obiettivo | Ratchet |
|---|---|---|
| **P0** ✅ | La notazione si stipula parlando; una relazione nuova costa un fatto | `literal_forms.p0t`, 45 assert, dentro `make test` |
| **P0b** ✅ | **Estrattori di prosa, porte di risposta e marcatori di segmento** si insegnano parlando | `governs is a relation verb` → estrae *e* risponde; `howbeit is a condition marker` → `segment_role(condition, keyword(howbeit))` |
| **P1** ✅ | **P-E**: un valore numerico si impara come uno testuale | `the population of nivora is 40000` → `Learned: population_of(nivora, 40000).` |
| **P2** | **P-C**: una frase che insegna non può essere eseguita | la frase del celsius riceve un rifiuto onesto, non `-16.6667` |
| **P3** | **P-D**: il ponte nome-comune → predicato, generalizzato dai generi alle classi | `ahoy is a greeting` cambia davvero come parrot0 tratta «ahoy» |
| **P4** | **P-B**: la prima forma condizionale insegnabile a voce | `a shark is a fish that swims fast` → due fatti, o rifiuto onesto |
| **P5** | **modi ipotetici** (la parte della richiesta di F. non ancora affrontata): una stipulazione vale *dentro un contesto*, e si può revocare | `from now on …` apre un `context/2`; `forget the notation …` lo chiude |
| **P6** | Il canale si misura: quante forme-che-insegnano parrot0 riconosce | una scala come le classi misurate, ma sugli **atti di insegnamento** |

## 4bis. Il gen429: estrattori, porte di risposta e segmenti — parlando

F.: *«anche i segmenti di input, gli answer frame e gli estrattori di prosa devono
essere insegnabili, col prompting classico interattivo».* Fatto, e di nuovo con
zero moduli e zero righe di C per l'insegnamento.

**Un verbo transitivo È già un pattern.** «@S governs @O» non è una forma da
scrivere: è la forma che *ogni* verbo ha. Quindi non si insegna il pattern — si
insegna che una parola è un verbo di relazione, e il pattern lo costruisce una
regola:

```
> zorak governs nivora        →  (non impara niente)
> governs is a relation verb  →  Learned: relation_verb(governs).
> zorak governs nivora        →  Learned: governs(zorak, nivora).
> who governs nivora?         →  Zorak.
```

La quarta riga è la metà che di solito manca. Un fatto imparato e non
interrogabile è un fatto **morto** — è il «buco del consumatore» del gen306, e si
chiude con una riga:

```prolog
answer_frame($Verb, $Verb) :- relation_verb($Verb).
```

Un verbo di relazione apre **per costruzione** anche la sua porta di risposta:
detto e chiesto restano lo stesso atto di apprendimento.

**I marcatori di segmento** sono classi come le altre, e il ponte è una regola
sola che *costruisce il nome del predicato* dal nome del ruolo:

```prolog
taught_marker($Role, $Cue) :-
    markable_role($Role), concat_atoms($Role, "_marker", $Pred),
    kb_fact($Pred, cons($Cue, nil)).
segment_role($Role, keyword($Cue)) :- taught_marker($Role, $Cue).
```

```
> howbeit is a condition marker  →  Learned: condition_marker(howbeit).
                                    → segment_role(condition, keyword(howbeit))
```

Un marcatore nuovo per un ruolo che esiste costa **una frase**; un ruolo nuovo
costa una riga (`markable_role/1`). Mai una riga di C.

**Il limite onesto, misurato:** `unless is a condition marker` **non** funziona,
perché «unless» è già una parola-funzione nota e il percorso che insegna le
classi la scarta. Insegnare qualcosa *a proposito di una parola che il sistema
già usa* richiede lo strato uso/menzione (K0 del piano di frontiera): è il gate
P3bis, e non va aggirato con un'eccezione.

## 5. Cosa NON fare

- **Non un modulo per formulazione.** Il gen428 ha aggiunto zero moduli e ha
  chiuso tre gap: è il metro. Se una richiesta sembra chiedere un modulo, quasi
  sempre chiede un `extract_frame` o un `intent_cue`.
- **Non frasi intere in KB.** Una riga nuova deve servire turni che nessuno ha
  ancora scritto, altrimenti è frasario ([[kb-first-phrases]]).
- **Non far eseguire ciò che insegna.** Finché P2 non è chiuso, ogni forma nuova
  di insegnamento va provata anche nel suo caso *ostile*: la stessa frase letta
  come richiesta.
- **Non fidarsi della conoscenza dichiarata.** Ogni riga aggiunta va provata
  viva. Due frame morti sono sopravvissuti a quarantasei generazioni.

## 6. Collegamenti

- [[teach-comprehension-via-mcp]] — lo stesso quadro dal canale API; qui sono
  cambiati il canale e, con lui, il punto in cui si accumula il debito
- [[teachable-procedures]] — la rappresentazione che serve a P-C
- [[frontier-kb-natural-dialogue]] — K0 (forma ≠ senso) è la base di
  `notation_of/2`; P5 (modi ipotetici) è K4
- [[kb-first-manifesto]], [[no-word-lists-in-c]] — la disciplina
- `docs/measured-classes.md` — la misura che ha tirato fuori tutto questo
