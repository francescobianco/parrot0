# 100 Nuovi Prompt Difficili Per Parrot0

**Data del corpus:** 2026-08-20
**Motivo:** il corpus precedente e' stato chiuso dalle generazioni successive e
non misura piu' la frontiera. Questi cento prompt sono nuovi e vanno eseguiti
con un cervello nuovo per turno, salvo dove il prompt dichiara esplicitamente
una sequenza.

## Criterio di misura

La colonna **successo minimo** non e' una risposta modello completa: indica la
mossa che deve essere osservabile per considerare il compito pertinente. Una
risposta generica, un muro, una richiesta di apprendimento o un fatto laterale
non conta. Per i casi con dati insufficienti, il successo consiste nel nominare
il dato mancante senza inventarlo. Per i casi composizionali, devono essere
rispettati tutti i vincoli espliciti.

Il corpus precedente resta nella storia Git. Questo file e' un nuovo set di
discovery: un caso va promosso a test solo dopo aver chiuso la classe con
varianti, crescita KB-first e ablazione.

## Strategia di autoapprendimento prevista

Questi prompt non dovevano essere risolti aggiungendo cento handler. Il piano di
`docs/plans/autocrescita.md` prevede una KB fertile che riconosce il proprio
spazio negativo e apre il ciclo seguente per ogni fallimento:

```text
lacuna -> domanda tipata -> candidato -> prova -> ablazione -> gate -> promozione
```

La lacuna deve diventare un oggetto osservabile, non restare il testo del muro:

```text
gap(Kind, Subject, Relation, Position)
```

Il ciclo operativo e' questo:

1. **Segnale:** il turno fallisce, risponde fuori bersaglio, raggiunge un fatto
   senza consumarlo, oppure l'audit trova conoscenza mai attivata.
2. **Domanda tipata:** il sistema distingue `knowledge`, `reach`, `surface` e
   `wrong`, invece di chiamare ogni problema “non conosco X”. Lo spazio
   negativo puo' anche produrre domande a freddo, senza un nuovo turno utente.
3. **Candidato:** si propone solo una riga di forma A (cue/superficie), B
   (fatto o concetto) o C (classe). Una forma D, cioe' nuova prosa o un nuovo
   `response_template` inventato per chiudere il caso, non ha generatore.
4. **Prova:** il candidato viene asserito nella sessione e si ripete lo stesso
   turno. La risposta deve diventare pertinente, non soltanto diversa dal muro.
5. **Ablazione:** si toglie il candidato e si ripete ancora. Se il prompt passa
   comunque, la diagnosi era sbagliata e la riga viene scartata.
6. **Gate:** `make test` resta verde e il saldo su `make hundred` e `make measure`
   non diminuisce. La provenienza deve essere registrata.
7. **Promozione:** la riga entra prima in quarantena come `KB_INDUCED`, con
   `learned_from(Row, Source)`, poi diventa ufficiale solo dopo N giri puliti;
   deve restare revocabile in blocco.

### Le cinque strategie applicate a questo corpus

| Strategia | Lacuna che vede | Candidato e fonte | Dove si applica qui |
|---|---|---|---|
| **S1 fatto** | entita' opaca, frame senza valore, asimmetria fra fratelli | forma B da corpus/wiki con provenienza citabile | concetti, eventi e relazioni dei prompt 11-20, 31-40, 42-50 |
| **S2 superficie** | facolta' esistente ma nessuna cue o forma espressiva la raggiunge | forma A raccolta dalla ricorrenza nella prosa o nel turno | traduzione strutturale, anafora, tempo, formati e trasformazioni: 11-40, 61-80, 91-100 |
| **S3 classe** | token usato da un motore ma assente dalla classe KB | forma C dal token, con ablazione doppia | quantificatori, marcatori temporali, operatori, formati e lessico inventato: 1-20, 31-50, 61-80 |
| **S4 audit** | regola morta, dialetto privato o predicato mai attivato | nessuna riga automatica: domanda su di se' o segnalazione al revisore | tutti i casi che richiedono una nuova inferenza, procedura, tool o modulo |
| **S5 dialogo** | lacuna di superficie o classe emersa nella conversazione | forma A/C insegnata dall'utente, effetto al turno seguente | prompt 41, 43, 45, 46, 48 e varianti multi-turno |

S1-S4 devono correre in parallelo sullo stesso banco: non si sceglie a priori
la strategia “piu' probabile”. Si misura quale sorgente produce crescita reale per
riga e quale cade in una tasca. S5 e' crescita assistita, non la prova di
autonomia; serve come controllo.

### Strategia assegnata alle dieci famiglie

La riga **strategia primaria** indica da quale ciclo deve partire la diagnosi;
le strategie secondarie sono necessarie quando il prompt richiede composizione.
Questa assegnazione copre esattamente tutti i cento indici del corpus.

| Prompt | Strategia primaria | Strategie secondarie | Cosa avrebbe dovuto apprendere |
|---:|---|---|---|
| 1-10 | S4 audit | S2, S3 | forme logiche e classi di quantificatori; una nuova regola di inferenza e' procedura E e non va promossa automaticamente |
| 11-20 | S1 fatto | S2, S4 | frame di eventi, relazioni temporali e forme causali; la provenienza non autorizza conclusioni causali inventate |
| 21-30 | S2 superficie | S3, S5 | forme di coreferenza, anafora, interruzione e atto indiretto; la colla deve conservare il contesto attivo |
| 31-40 | S1 fatto | S2, S3, S4 | concetti, alias, relazioni e forme estrattive dei mondi nuovi; il gate deve impedire nomi inghiottiti dalla prosa |
| 41-50 | S5 dialogo | S3, S4 | fatti con scope, supersessione, provenienza e lessico insegnato; assert e retract devono cambiare il comportamento senza rebuild |
| 51-60 | S4 audit | S2, S5 | vincoli e prerequisiti come procedura dichiarabile; una nuova strategia di pianificazione va proposta al revisore, non inventata in C |
| 61-70 | S4 audit | S2, S3 | procedure numeriche, operatori e notazione; i numeri vanno legati a slot e le regole nuove passano da verifica, non da una risposta plausibile |
| 71-80 | S2 superficie | S3, S4 | schemi e marcatori di forma per CSV, JSON, YAML, SQL, EBNF e liste; solo schemi gia' verificati possono essere usati |
| 81-90 | S4 audit | S2, S5 | lacune di diagnosi, sicurezza e tool; il sistema deve segnalare il modulo o dato mancante, mai fabbricare un comando o una causa |
| 91-100 | S2 superficie | S4, S5 | trasformazioni e vincoli formali gia' rappresentati; la generazione libera e' forma D e non puo' essere autoappresa come fatto |

### Cosa puo' e non puo' chiudere da solo

Il piano promette autocrescita della conoscenza, non la magia di trasformare una
lacuna procedurale in una facolta'. In particolare:

- S1 puo' recuperare da una fonte un concetto, un fatto o un frame mancante;
- S2 puo' raccogliere una superficie ricorrente come `extract_frame` o
  `intent_cue`, poi verificarla con ablazione;
- S3 puo' imparare che una forma appartiene a una classe, con prova prima/dopo;
- S4 puo' scoprire che una regola e' morta o che un predicato non e' mai stato
  toccato, ma deve riportare il difetto e non modificare il motore;
- S5 puo' chiedere o ricevere dall'utente una forma insegnabile e usarla subito;
- una nuova procedura di logica, aritmetica, planning, analisi del codice o
  accesso a tool e' una proposta di forma E: richiede revisione, implementazione
  generale e poi lo stesso gate;
- una risposta creativa nuova, un disclaimer nuovo o un template scritto per un
  solo prompt e' forma D: il ciclo non lo deve generare.

Per questo i prompt 1-10, 51-70 e 81-90 sono anche sonde di confine: una buona
autocrescita deve derivare la lacuna e fermarsi onestamente quando manca il
motore, invece di fingersi capace. Il risultato corretto del ciclo puo' essere
quindi una domanda tipata, una proposta in quarantena o una segnalazione al
revisore; non ogni riga deve diventare una risposta.

### Criterio di massa critica

La misura finale non e' il numero di righe aggiunte. Per ogni giro si registrano
le lacune chiuse e le nuove lacune autochiudibili, cioe' quelle per cui esiste un
generatore di candidato. La fertilita' e':

```text
R = nuove_lacune_autochiudibili / lacune_chiuse
```

La KB e' fertile solo quando la media mobile di `R` resta almeno 1 e il saldo di
`make hundred` e `make measure` resta non negativo. K, P e S devono valere
insieme: una riga con provenienza sopravvive all'ablazione (K), lo stesso prompt
riceve una risposta pertinente (P), e il punteggio esterno resta almeno 80% su
piu' esecuzioni (S). Un aumento di R senza aumento dei banchi e' una fuga in una
tasca, non apprendimento riuscito.

### Roadmap di apprendimento

La campagna sui cento deve seguire le fasi del piano, non saltare direttamente
alla promozione della prima riga che produce una risposta:

| Fase | Passo della strategia | Applicazione al corpus | Cricchetto |
|---|---|---|---|
| **F0** | tipizzare la lacuna | ogni fallimento riceve `Kind`, soggetto, relazione e posizione stabili | due esecuzioni assegnano lo stesso `Kind` allo stesso prompt |
| **F1** | audit a freddo | aggregare le impronte su questi cento, `measure` e `make test` | l'audit trova almeno un predicato mai attivato o una regola morta senza che venga indicato a mano |
| **F2** | S1 su entita' opache | recuperare concetti/frame mancanti da una fonte citabile | almeno un candidato B con provenienza chiude un prompt |
| **F3** | ablazione come cancello | verificare che ogni riga candidata sia necessaria | almeno una proposta viene scartata perche' inutile |
| **F4** | S2 sulla superficie | raccogliere forme ricorrenti dalla prosa e dai turni | una forma A chiude un caso che prima falliva e sopravvive all'ablazione |
| **F5** | strategie in parallelo | confrontare S1-S4 sullo stesso corpus e separare i saldi | si misura quale sorgente porta piu' crescita per riga |
| **F6** | promozione | quarantena `KB_INDUCED` con fonte, turno e revoca | una riga non scritta a mano entra in `KB` senza regressioni |
| **F7** | fertilita' | misurare le nuove lacune autochiudibili aperte dai cento | la curva di `R` puo' essere tracciata e testata contro la soglia 1 |

F0-F1 sono percezione, F2-F4 sono azione controllata, F5-F6 sono confronto e
conservazione, F7 e' la misura della massa critica. Se un prompt richiede una
procedura E, la roadmap lo deve lasciare come proposta al revisore invece di
fabbricare una riga D: anche questo e' un esito positivo dell'autocrescita,
perche' rende visibile il confine tra conoscenza apprendibile e motore mancante.

## Logica, controfattuali e quantificatori

| # | Prompt | Successo minimo |
|---:|---|---|
| 1 | If no poets are engineers and some engineers are swimmers, can any poet be a swimmer? | distinguere cio' che segue da cio' che resta possibile |
| 2 | If every amber object is warm, and this object is not warm, what may you conclude about it? | contraposizione solo con la premessa universale corretta |
| 3 | If the key opens the box only when the lamp is green, the lamp is green, and the box is open, did the key open it? | non confondere condizione necessaria con causa osservata |
| 4 | Assume, counterfactually, that all ravens are blue. What follows about a black raven? | applicare l'ipotesi al caso senza usare la KB del mondo |
| 5 | The rule says: if a card is marked A, it has an even number. Which cards must you inspect to test the rule: A, B, 2, 3? | selezionare A e 3 e motivare entrambe le scelte |
| 6 | Either the sensor failed or the battery is empty, but not both. The sensor did not fail. What follows? | disgiunzione esclusiva con eliminazione |
| 7 | Every monk admires some painter. Does that imply that one painter is admired by every monk? | distinguere quantificatori con ordine diverso |
| 8 | All pilots who are calm are trusted. Some trusted pilots are not calm. Are all trusted pilots calm? | negare l'inversione indebita dell'implicazione |
| 9 | If the alarm is on, the door is locked; if the door is locked, the guard is present. The guard is absent. What can be inferred? | catena contrapposta fino alla prima premessa |
| 10 | Is “This sentence is false” a contradiction, an undecidable sentence, or both? State the semantic caveat. | non dare un si/no piatto a un paradosso |

## Causa, tempo ed eventi

| # | Prompt | Successo minimo |
|---:|---|---|
| 11 | The plant wilted after the heater was moved, but nobody measured soil moisture. Which causal claims are justified? | separare successione, ipotesi e prova |
| 12 | Event A starts before B, B overlaps C, and C ends before A ends. Can A be said to precede C? | ragionare sugli intervalli senza trasformarli in punti |
| 13 | Mira promised to call after lunch, called before lunch, and later apologized. What is the temporal inconsistency? | identificare l'evento che viola il vincolo |
| 14 | A repair causes a reboot, and a reboot causes data loss only if the disk is full. The disk was not full. Did the repair cause data loss? | propagare una causa condizionata senza affermare il risultato |
| 15 | Before the bridge opened, the road was closed; while it opened, traffic stopped; afterward, traffic resumed. Give the state sequence in order. | ricostruire stati e transizioni |
| 16 | “Nora stopped sending alerts” and “Nora did not send alerts.” What presupposition differs? | distinguere cambiamento di stato da semplice negazione |
| 17 | A medication was changed on Monday and symptoms improved on Tuesday, but treatment also began Tuesday. What confounder prevents attribution? | citare il trattamento concorrente |
| 18 | The backup completed at 09:10, the deletion began at 09:05, and the restore was requested at 09:20. Which operations overlap? | ordinare eventi con orari e intervalli |
| 19 | If the ferry is late, the meeting starts late. The meeting started late before anyone checked the ferry. Is the ferry's lateness established? | distinguere effetto osservato da causa stabilita |
| 20 | Describe the state changes in: “The door was open, Sara closed it, then Leo reopened it.” | estrarre due transizioni e il loro ordine |

## Ambiguita', riferimenti e dialogo

| # | Prompt | Successo minimo |
|---:|---|---|
| 21 | Alex told Jordan that the package was damaged, but they signed for it anyway. Who does “they” refer to, and what remains ambiguous? | non inventare un antecedente univoco |
| 22 | The red key is beside the blue key. Move it into the drawer. Which object is ambiguous? | segnalare l'ambiguita' invece di scegliere per posizione |
| 23 | “I saw the astronomer with the telescope.” Give the two parses and explain their difference. | distinguere strumento da accompagnatore |
| 24 | First say: “The archive contains a map.” Then ask: “Does it contain a map?” What does “it” resolve to? | risolvere la coreferenza sul contesto corrente |
| 25 | We were comparing trains. Interrupting: “My sister bought a bicycle.” Then: “Which one is faster?” What must be clarified? | non saltare automaticamente al topic precedente |
| 26 | Sam asked Pat to email Lee after the meeting. Who is the intended sender? | dichiarare che la frase non lo determina |
| 27 | “Every old book is rare” followed by “This one is not.” Is “this one” a book, and what inference is licensed? | usare il referente dichiarato e rilevare la tensione |
| 28 | In “The manager approved the plan because she trusted the analyst”, what does “she” most naturally refer to, and why is it not certain? | fornire preferenza più caveat |
| 29 | Track the referent of “that decision” across: “The team rejected the merger. The board discussed that decision.” | collegare un'anafora nominale a un evento |
| 30 | Answer only after identifying whether “Can you open the file?” is a capability question or a request. | distinguere domanda letterale e atto indiretto |

## Prosa, estrazione e mondi nuovi

| # | Prompt | Successo minimo |
|---:|---|---|
| 31 | From “Leto, a nocturnal glider, nests beneath basalt arches and avoids the copper moth,” extract the entity, class, habitat, and behavior. | quattro slot distinti, senza inglobare la relativa |
| 32 | Read: “Unlike copper, which conducts heat, glass is a poor conductor.” What contrast and fact are stated? | estrarre il contrasto e il fatto su entrambi i materiali |
| 33 | In “The Vela is called a dusk-bird in the northern records,” record the alias without treating “dusk-bird” as a second species. | alias separato dall'identita' |
| 34 | “Some pilots are healers; no healer is nocturnal.” Can a nocturnal pilot be one of those healers? | combinare esistenziale e divieto di classe |
| 35 | Learn this miniature world: “A tor is a vessel. Every vessel has a keel. Nemi is a tor.” What can you ask about Nemi's keel? | derivare la proprieta' su due passaggi |
| 36 | The sentence “May is a month, but in this paragraph may expresses permission.” Which senses must be kept apart? | non fondere omonimo lessicale e uso modale |
| 37 | “The city is north of the lake, and the station is there.” What relation should “there” inherit? | ereditare il luogo senza creare un'entita' fittizia |
| 38 | Infer the relation shared by: “Kiro greets Luma”, “Luma greets Vesh”, “Vesh greets Kiro.” | riconoscere una relazione ciclica senza imporre transitivita' |
| 39 | In “Only after the seal broke did the archive become accessible,” what event is the condition and what event is the result? | invertire correttamente l'ordine superficiale |
| 40 | Given “A red tal is not a blue tal” and “Rin is a red tal,” is Rin blue? | applicare la negazione alla stessa categoria composta |

## Memoria, apprendimento e provenienza

| # | Prompt | Successo minimo |
|---:|---|---|
| 41 | Remember that my dog is called Miso. Later I say: “Forget the name of my dog.” What fact must be retracted? | ritrarre il fatto mirato, non tutta la memoria |
| 42 | I tell you “the badge is silver,” then “the badge is wooden.” How should you answer whether it is silver? | esporre conflitto e provenienza delle due viste |
| 43 | Teach the invented word “nava” as a color, then ask “What color is the flag?” when no flag fact exists. What should happen? | usare il lessico appreso ma non inventare il fatto |
| 44 | A fact was inferred from two rules, one of which is later retracted. Is the fact still supported? | ricalcolare la dipendenza, non lasciare una derivazione fantasma |
| 45 | Store “I prefer tea” for this session only, then ask in a new session what I prefer. | rispettare lo scope di sessione |
| 46 | Learn “glim” as a conjunction, use it in a rule, then ablate that lexical fact. What behavior must change? | mostrare crescita e perdita runtime della stessa forma |
| 47 | “All norks are quiet. Pila is a nork.” Which part of the answer is a fact and which is an inference? | separare provenienza esplicita e derivata |
| 48 | Two users teach different meanings for “sora.” How can the system avoid contaminating their sessions? | legare la forma al contesto utente |
| 49 | Retract the rule “every vale is blue” while retaining the explicit fact “Nara is blue.” What remains true? | distinguere regola ritratta da fatto indipendente |
| 50 | Explain why a conclusion is uncertain when one premise is missing, rather than saying the conclusion is false. | distinguere sconosciuto da falso con lacuna nominata |

## Piani, vincoli e decisioni

| # | Prompt | Successo minimo |
|---:|---|---|
| 51 | Plan a three-step move: the fragile vase must travel first, the truck arrives at noon, and the elevator is unavailable. | piano ordinato che soddisfa tutti i vincoli |
| 52 | Choose between two routes: A is shorter but flooded; B is longer, accessible, and safe. I need accessibility above speed. | applicare la priorita' dichiarata |
| 53 | Start a plan to publish a report, then add “the figures are not verified.” What step must block publication? | inserire una precondizione nel piano esistente |
| 54 | The user changes “minimize cost” to “minimize risk” halfway through a plan. Which decisions must be revisited? | propagare il cambio di obiettivo |
| 55 | Make a plan that uses a printer, but the printer is broken and no replacement exists. | rilevare impossibilita' e proporre solo alternativa compatibile |
| 56 | Schedule A before B, C after A, and B before C. Is the schedule consistent? | verificare il grafo dei vincoli |
| 57 | You have one room, two meetings, and no overlap allowed. Give a schedule or state why none exists. | assegnare risorse e dichiarare eventuale impossibilita' |
| 58 | Rank these actions by reversibility before deleting data: inspect, export, delete, restore-test. | ordinare per rischio/reversibilita' |
| 59 | The cheapest supplier cannot meet the deadline, while the second-cheapest can. Which criterion decides the choice? | trattare la deadline come vincolo duro |
| 60 | Continue the plan only if its next step does not require a tool that is unavailable. | controllare prerequisito operativo prima di agire |

## Numeri, strutture e trasformazioni

| # | Prompt | Successo minimo |
|---:|---|---|
| 61 | A recipe for 6 uses 450 g of flour. How much flour is needed for 14, preserving the ratio? | 1050 g con proporzione esplicita |
| 62 | A price of 80 is reduced by 25% and then increased by 25%. Is it back to 80? | 75, non confondere percentuali successive |
| 63 | A tank is 3/5 full. After adding 12 liters it is 4/5 full. What is its capacity? | 60 liters con equazione di differenza |
| 64 | The sequence is 2, 6, 12, 20, 30. Give the next term and one rule, while noting whether it is unique. | 42 e riconoscimento della non unicita' |
| 65 | If “7” is stipulated to denote 10, evaluate 7 + 2 under ordinary addition and under the stipulated notation. | separare simbolo, valore e operazione |
| 66 | A rectangle has perimeter 30 and integer side lengths. List all unordered possibilities. | coppie (1,14) through (7,8) |
| 67 | Two sets are A={1,2,3,4} and B={3,4,5}. Give intersection, union, and A\B. | tre operazioni senza scambiarne i ruoli |
| 68 | A graph has edges A-B, B-C, and A-C. Is it a tree? Explain using the cycle. | no, ciclo A-B-C-A |
| 69 | Convert 2 hours 35 minutes into minutes, then subtract 48 minutes. | 107 minuti |
| 70 | A bag has 3 red and 2 blue tokens. Without replacement, what is the probability of drawing two red tokens? | 3/10 con modello senza reinserimento |

## Artefatti e forme verificabili

| # | Prompt | Successo minimo |
|---:|---|---|
| 71 | Return exactly two lines of CSV with header `name,count` and one row for `miso,3`. | CSV valido, due righe e nessun testo extra |
| 72 | Produce JSON for a user with `name`, `roles` as an array, and `active` as a boolean. | virgolette, array e booleano validi |
| 73 | Write a YAML document for two services, each with a port and healthcheck. | struttura annidata coerente |
| 74 | Give a SQL query that returns the latest order per customer, including ties. | soluzione con ranking e tie esplicito |
| 75 | Write a regex that accepts `user@example.com` but rejects `user@` and `@example.com`. | pattern con controllo minimo di local e dominio |
| 76 | Return a markdown table with columns risk, likelihood, impact, and mitigation for two risks. | quattro colonne, due righe dati, intestazione corretta |
| 77 | Define an EBNF grammar for identifiers made of a letter followed by zero or more letters or digits. | produzione ricorsiva o ripetitiva formalmente valida |
| 78 | Give a JSON Patch operation that replaces `/profile/name` with `Ada`. | operazione `replace` con path e value corretti |
| 79 | Write a two-case truth table for `P XOR Q`, including column headers. | quattro combinazioni e risultati corretti |
| 80 | Return a numbered list of exactly three non-overlapping acceptance criteria for a login form. | tre elementi distinti e numerati, nessun preambolo |

## Codice, diagnosi e strumenti

| # | Prompt | Successo minimo |
|---:|---|---|
| 81 | This Python function mutates its default list. Explain the bug and show the smallest behavior-preserving fix: `def add(x, xs=[]): xs.append(x); return xs`. | causa, fix con `None`, comportamento preservato |
| 82 | A test passes alone but fails after another test changes an environment variable. What class of defect is suggested? | contaminazione di stato/test order dependence |
| 83 | Review this transaction: read balance, wait, write balance. Two requests can withdraw concurrently. Name the race and one repair. | lost update e sincronizzazione/atomic update |
| 84 | A parser accepts `12` but rejects `-12` and `12.5`. What grammar capability is missing? | segno e parte frazionaria, non un generico “parser issue” |
| 85 | A cache returns stale data only after a write succeeds. Which invalidation edge should be inspected first? | relazione write-success/cache invalidation |
| 86 | Given an error `connection refused` after DNS resolves, which layer is implicated first? | trasporto/servizio in ascolto, non DNS |
| 87 | Propose a minimal property-based test for a sort function that catches mutation of its input. | copia input, sort, confronto input originale |
| 88 | A command is destructive and the current directory is unknown. What must be checked before execution? | directory/target, preview e conferma o backup |
| 89 | The requested live weather source is unavailable, but a cached value is from yesterday. What answer is honest? | distinguere dato stale da dato corrente e dichiarare limite |
| 90 | Show how to investigate a failing build when the compiler output is truncated, without guessing the root cause. | recuperare output completo e isolare il primo errore |

## Generazione, trasformazione e limiti epistemici

| # | Prompt | Successo minimo |
|---:|---|---|
| 91 | Rewrite “The committee rejected the proposal” in passive voice, preserving tense and agent. | “The proposal was rejected by the committee” |
| 92 | Summarize this claim in exactly 12 words: “A late backup is safer than no backup, but it cannot restore changes made after it.” | dodici parole, stesso contrasto temporale |
| 93 | Give two counterexamples to “more data always improves a decision,” from different domains. | due domini e controesempi effettivi |
| 94 | Invent a four-line poem with an AABB rhyme scheme about a broken compass. | quattro versi e schema AABB verificabile |
| 95 | Explain the difference between correlation and causation using one example, then state what evidence would change your view. | esempio, distinzione e prova aggiornante |
| 96 | Create a fictional law, derive one consequence, and label both as fictional rather than factual. | mondo inventato separato dalla conoscenza reale |
| 97 | Give medical triage guidance for sudden severe chest pain while avoiding a diagnosis. | urgenza, invio a emergenza, nessuna diagnosi |
| 98 | Assess whether “this contract is enforceable” can be answered from the title alone. | rifiutare il giudizio e indicare giurisdizione/testo mancanti |
| 99 | Explain a poem whose text is not included, but do not fabricate lines or historical context. | richiesta del testo e limite dichiarato |
| 100 | Defend a scientific theory that has no observations, then separate speculation from evidence. | rifiutare la difesa fattuale e distinguere ipotesi/evidenza |

## Aggregazione delle classi

| Classe | Prompt |
|---|---:|
| Logica, controfattuali e quantificatori | 10 |
| Causa, tempo ed eventi | 10 |
| Ambiguita', riferimenti e dialogo | 10 |
| Prosa, estrazione e mondi nuovi | 10 |
| Memoria, apprendimento e provenienza | 10 |
| Piani, vincoli e decisioni | 10 |
| Numeri, strutture e trasformazioni | 10 |
| Artefatti e forme verificabili | 10 |
| Codice, diagnosi e strumenti | 10 |
| Generazione, trasformazione e limiti epistemici | 10 |
| **Totale** | **100** |

## Uso previsto e cricchetto

La prima esecuzione deve registrare risposta letterale, tempo, modulo
rivendicante, `gap(Kind, Subject, Relation, Position)` e strategia che ha
prodotto la diagnosi. Non si deve correggere un prompt alla volta: ogni
risposta interessante va trasformata in una sonda di classe, con sinonimi,
ordine diverso, entita' inventate, numeri diversi e almeno una prova di
ablazione.

Per ogni candidato si conserva questa traccia minima:

```text
prompt -> gap tipato -> strategy -> candidate/source -> prova -> ablation -> gate
```

I prompt 41, 46 e 48 sono cricchetti espliciti di S5/S3: l'apprendimento e la
separazione della conoscenza devono funzionare a runtime, senza ricompilazione,
e devono perdersi quando il fatto viene ritratto. I prompt 31-40 verificano che
S1 non apprenda soltanto fatti isolati ma anche forme e relazioni interrogabili.
I prompt 71-80 verificano che S2 non promuova un artefatto solo perche' il testo
sembra plausibile: lo schema deve essere verificato. I prompt 1-10, 51-70 e
81-90 verificano invece che S4 sappia dichiarare una lacuna procedurale senza
farla passare per conoscenza.

Il documento e' pronto per una prima campagna di misura, non per essere copiato
automaticamente nella suite TDD: la promozione richiede una classe chiusa,
provenienza, ablazione, non-regressione e saldo positivo o neutro sui banchi.
