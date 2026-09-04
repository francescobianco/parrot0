# Comprensione universale del codice

> **Piano operativo, 2026-09-04.** Questo documento porta la tesi di
> `docs/CODE-MASTERY.md` dentro la coda viva di `LEARN_TODO.md`: data una
> codebase, parrot0 deve poter costruire una conoscenza interrogabile del
> repository e rispondere a domande strutturali, causali e qualitative senza
> ridurle a un catalogo di prompt o di bug pattern.
>
> La domanda zero resta quella di `MANTRA.md`: **parrot0 può imparare domani una
> nuova forma, una nuova regola semantica o un nuovo criterio di valutazione,
> parlando e senza ricompilare?** Se no, non è ancora comprensione universale.
>
> Questo piano è il **come** della sola comprensione del codice. Il perché resta
> `CODE-MASTERY.md`; azione, patch e repair restano nei piani del coding agent.
> `coding-agent-todo.md` T11/T12 e `parrot0-forge-master-plan.md` W4 confluiscono
> qui per la parte di lettura, IR, localizzazione ed evidenza.

---

## 0. Verdetto dell'audit: gli organi esistono, ma non condividono ancora l'oggetto

La direzione giusta è già scritta in più punti del repository:

- `CODE-MASTERY.md` stabilisce che codice e lingua devono abitare lo stesso
  substrato e che l'AST è KB;
- `universal-input.md` ha già una IR gerarchica di span e il principio «un solo
  input»;
- `input-structure.p0` mostra come osservazioni meccaniche diventino letture,
  proposizioni e proiezioni interrogabili senza un secondo parser;
- `materialized_view/2` ha mostrato come congelare derivazioni costose senza
  trasformare una cache in significato;
- `p0_compensate` ha già il ciclo arresto → azione → replay → attribuzione;
- il kernel agentico, `PatchArtifact` e `P0Obs` esistono già, anche se sono oltre
  il perimetro read-only di questo piano.

Ma l'oggetto condiviso manca ancora. Oggi:

| capacità dichiarata | realtà osservata | limite strutturale |
|---|---|---|
| codice come KB | `code_ingest` emette `code_function/1` e `code_calls/2` | nessun file, span, scope, symbol id, source hash o revisione |
| C e Python sullo stesso schema | i due frontend emettono gli stessi due fatti | keyword, scope e molte semantiche restano due scanner C distinti |
| domande sul codice | `mod_codeast` distingue eval/functions/calls con rami e catene compilate | una nuova specie di domanda richiede C |
| spiegazione | riconosce `printf`, `return`, `for` e `while` | elenco di tratti, non derivazione di comportamento |
| localizzazione | il filesystem viene riscansionato per ogni richiesta | non esiste un indice di repository vivo e revisionato |
| review/repair | quattro scanner per quattro smell SWE scelti | il finding non nasce da una IR generale né da criteri insegnabili |
| benchmark | i 25 file di `tests/code/*.code` sono tutti `#expect: pass` | il discovery harness non contiene più un gap e non misura domande qualitative |
| capability truth | `code_reading = transfer` | il wall dichiara correttamente «IR, scope, CFG/dataflow» |

Due dettagli rendono il limite particolarmente netto:

1. `code_ingest` ritratta i vecchi archi per **nome nudo della funzione**. Due
   file con una funzione omonima non sono due entità: una lettura può cancellare
   o fondere l'altra.
2. `code_keyword/2` esiste già in KB, ma `src/code.c` mantiene ancora liste
   private di keyword C e Python. La conoscenza è stata rappresentata, ma il
   consumer non la esegue: è il moltiplicatore 3 di `LEARN_TODO.md` §−1.

Inoltre `codebench.sh` svuota intenzionalmente `PARROT0_BASE` e
`PARROT0_SESSION`: i suoi pass certificano le meccaniche legacy in isolamento,
non la crescita KB-first. In `80-code.c` restano nove TODO espliciti sulla forma
booleana compilata delle richieste; il nuovo ratchet deve quindi osservare sia il
proof semantico sia assert/retract/reteach nel profilo reale.

Il risultato non è da buttare: lexer, evaluator, compiler oracle, scanner e
repair sono strutture secondarie vive. Vanno trasformati in **produttori di
osservazioni della stessa IR**, non rimossi e non promossi a teoria universale.

---

## 1. Che cosa significa «comprensione universale del codice»

Non significa conoscere in anticipo ogni linguaggio, libreria, bug o domanda.
Significa che l'insieme di ciò che parrot0 può imparare e interrogare è aperto.

Una codebase è compresa quando parrot0 può:

1. **vedere** file, documenti, simboli, scope, nodi e relazioni con identità e
   provenance stabili;
2. **ricostruire** struttura, controllo, dati, effetti e dipendenze come fatti e
   regole della KB;
3. **legare una domanda nuova** a un goal di analisi senza un ramo C per la sua
   frase;
4. **decidere fra letture e prove**, conservando alternative, limiti e conflitti;
5. **formare un giudizio qualitativo** da criteri, evidenza e controevidenza,
   distinguendo misurato, derivato e ipotizzato;
6. **acquisire l'evidenza mancante** con un'azione read-only dichiarata, poi
   riprendere la stessa domanda;
7. **imparare parlando** una forma, un costrutto, una regola semantica, un
   criterio o una policy di progetto, e usarli dal turno successivo;
8. **rileggere** la codebase dopo una modifica o una lezione, sostituendo la
   vecchia interpretazione invece di accumularne una seconda.

«Universale» descrive dunque il **meccanismo di crescita**, non una promessa di
onniscienza. Davanti a informazione insufficiente la risposta corretta è un gap
preciso: «posso sospettarlo staticamente, ma per dire quale parte è lenta mi
serve un profilo», non un finding inventato e non un muro cieco.

### 1.1 La IR è un confine attraversabile, non il nuovo silo del codice

La base teorica del piano include D38/GD10 di `LEARN_TODO.md`: **le capacità
non stanno soltanto nelle zone della KB, ma negli archi di ordine superiore che
permettono a una zona di usare l'altra**. Il moltiplicatore cercato non è avere
più fatti sul codice; è rendere applicabile a un bisogno del codice conoscenza
acquisita in un'altra rappresentazione e in un altro dominio.

Esempi di composizione attesi, non casi da cablare:

- una API descritta nella documentazione come bloccante diventa evidenza su un
  nodo `call` osservato nel sorgente;
- una regola di sicurezza acquisita come conoscenza di dominio diventa un
  obbligo sui flussi di dati che raggiungono una sink;
- una policy appresa da `AGENTS.md` si lega al medesimo symbol id osservato dal
  frontend e restringe le trasformazioni ammissibili;
- una proprietà algoritmica appresa in forma astratta si combina con loop,
  call path e profilo senza una capacità speciale “analizza questo algoritmo”.

Il solver possiede già il meccanismo decisivo: `apply/2` invoca il normale
risolutore quando il nome del predicato è un **dato**. La IR deve usarlo come
confine ordinario, non relegarlo a consumer speciali. Il contratto generale è:

```prolog
ir_denotation(Representation, Item, Role, Entity).
representation_bridge(Representation, Role, Predicate, Mode).
ir_domain_claim(Representation, Item, Proposition).
ir_domain_claim_basis(Representation, Item, Proposition, Bridge).
```

Il frontend pubblica la denotazione debole che può provare. La KB decide quale
ponte abilita una relazione esterna; `apply/2` la percorre; il claim conserva il
predicato e il ponte che lo hanno prodotto. Un nuovo dominio diventa quindi
raggiungibile con un arco, non con un nuovo handler del codice.

Non è un join indiscriminato. Valgono le due guardie di D38:

1. ogni composizione porta basis/provenance ed è ritrattabile con il proprio
   arco;
2. `false_composition` e controesempi devono poter confutare il passaggio: due
   zone corrette possono produrre una conclusione falsa se il ponte è falso.

La prova di “KB viva” sarà causale e combinatoria: aggiunti `N` archi, i compiti
freddi sbloccati devono crescere più che linearmente perché gli archi nuovi si
compongono con quelli già presenti. Se crescono uno-a-uno, abbiamo rinominato un
frasario. Ablare un arco deve eliminare soltanto i claim che lo citano nella
basis e lasciare intatte osservazioni e conoscenza delle due zone.

### 1.2 Le famiglie di domanda, non un frasario

Le superfici sono aperte; le operazioni semantiche sono componibili:

| operazione | esempio di bisogno, non di stringa da matchare |
|---|---|
| descrivere | responsabilità, input, output, effetti, limiti di un simbolo |
| localizzare | dove è definito/usato/provato/profilato un comportamento |
| spiegare | perché un valore o una decisione emerge da quel cammino |
| confrontare | differenze di comportamento, costo, rischio o struttura |
| valutare | correttezza, manutenibilità, performance, sicurezza, chiarezza |
| predire | blast radius e conseguenze di una modifica ipotetica |
| diagnosticare | quale contratto è rotto e quale evidenza lo mostra |
| chiedere evidenza | che cosa manca per rendere una conclusione affidabile |

Una nuova espressione di una di queste operazioni è lessico/grammatica KB. Una
nuova dimensione, come consumo energetico o compatibilità ABI, è un criterio KB.
Il C non deve acquisire né la frase né il giudizio.

---

## 2. Tre piani di conoscenza, con cicli di vita diversi

La distinzione di `act-as-subagent.md` va resa parte dell'architettura, perché
salvare tutto nella stessa casa renderebbe falsa la KB al primo edit.

### 2.1 Conoscenza della codebase: osservata, revisionata, ricostruibile

Comprende nodi, simboli, chiamate, import, test, profili, diagnostica e storia del
repository corrente. È vera **rispetto a uno snapshot** e porta sempre:

- repository e source unit;
- hash dei byte o commit/tree id;
- frontend e versione;
- span;
- modo epistemico (`parsed`, `derived`, `observed`, `declared`, `hypothesized`).

Non si promuove nella KB curata con `/save`. Può essere materializzata in una
cache content-addressed, ma la cache è eliminabile: la sorgente resta l'autorità.

### 2.2 Conoscenza strutturale: generale e persistibile

Comprende:

- vocabolario astratto del codice;
- mapping frontend → IR;
- semantica di costrutti e API;
- regole di analisi;
- criteri qualitativi e obblighi di evidenza;
- action schema per acquisire osservazioni;
- forme linguistiche con cui insegnare e interrogare questi oggetti.

È la conoscenza che deve crescere soprattutto via prompt e che, dopo i gate del
`LEARN_PROTOCOL.md`, può essere salvata e versionata.

### 2.3 Policy di progetto: normativa e scoped

«In questa codebase la latenza vale più della memoria», «non cambiare l'ABI»,
`AGENTS.md` e `MANTRA.md` non sono proprietà universali del codice. Sono policy
del repository, con provenienza documentale e scope. Devono poter essere dette,
corrette e ritratte senza trasformarle in regole globali.

Questa distinzione risolve la domanda qualitativa: *“andrebbe migliorato?”* non
ha una verità assoluta. Ha una risposta rispetto a criteri generali, priorità
locali, evidenza disponibile e costi del cambiamento.

---

## 3. La IR bersaglio: una sola memoria di lavoro per documenti e codice

Non serve forzare ogni informazione dentro un unico predicato. Serve che ogni
consumer interroghi lo stesso **grafo identificato e revisionato** e che nessun
consumer riapra la stringa grezza per ricapire ciò che il producer ha già visto.

Il dialetto KB accetta al massimo quattro argomenti; la forma deve quindi usare
termini composti, come già fanno `input_node/4` e `semantic_proposition/1`.

```prolog
source_snapshot(Repo, Snapshot, source(Hash, Kind), provenance(Origin, Time)).
source_unit(Snapshot, Unit, artifact(Path, Language), content(Hash, Bytes)).

code_node(Snapshot, Node, node(Kind, Parent, Scope), span(Unit, Start, Length)).
code_name(Snapshot, Node, Role, "surface").
code_edge(Snapshot, From, EdgeKind, To).
code_property(Snapshot, Entity, Property, Value).

analysis_claim(Snapshot, Claim, state(Status, Confidence), Subject).
claim_evidence(Claim, Evidence, Role).
evidence_origin(Evidence, Mode, Source, Span).
```

I nomi sono illustrativi; il contratto è ciò che conta:

- **`source_*`** dice a quale realtà appartiene il fatto;
- **`code_node`** conserva gerarchia e coordinate;
- **`code_edge`** esprime relazioni aperte senza un predicato nuovo per ogni
  costrutto;
- **`code_property`** porta attributi estensibili;
- **claim/evidence** separa l'osservazione dal giudizio.

Le viste ergonomiche (`defines`, `calls`, `reads`, `writes`, `dominates`,
`depends_on`, `covered_by`) si derivano da questa IR. I legacy
`code_function/1` e `code_calls/2` restano viste compatibili finché hanno
consumer, coerentemente con la conservazione delle strutture secondarie. Non
sono però un vincolo di progetto: si deprecano progressivamente quando perdono
l'identità necessaria o costringono il nuovo percorso a nomi globali.

Ogni IR deve inoltre offrire una **denotazione**, non soltanto campi propri. Per
il codice, `code_name(Snapshot, Node, Role, Name)` proietta inizialmente in
`ir_denotation(code, item(Snapshot, Node), Role, Name)`. Gli archi
`representation_bridge/4` possono poi raggiungere via `apply/2` predicati
unari o binari di altre zone. È questa interfaccia — item, ruolo, entità,
predicate variabile e basis — l'oggetto condiviso fra IR del codice, documenti,
policy, profili e conoscenza di dominio.

### 3.1 Identità, span e rilettura

Un nome non è un'identità. L'id minimo deve incorporare:

- source unit content-addressed;
- cammino strutturale nel frontend;
- enclosing symbol;
- kind e ordinalità fra sibling equivalenti.

Riga e colonna sono coordinate, non identità. Un edit produce un nuovo snapshot;
un mapping di lineage può dichiarare che due nodi attraverso le revisioni sono
lo stesso simbolo, ma non deve fingere stabilità quando il match è ambiguo.

La rilettura segue una transazione al confine, non modifica fatti durante una
risoluzione:

```text
nuovi byte -> frontend -> overlay di osservazioni -> validazione
           -> swap dello snapshot -> invalidazione della closure dipendente
```

Una nuova lezione semantica invalida le viste di analisi che ne dipendono, non i
nodi sorgente. Un edit di un file invalida quel file e la closure inversa
dichiarata, non tutta la KB. È SC40 applicato al codice: **imparare deve poter far
rileggere ciò che era già stato letto**.

### 3.2 Linguaggi come delta

Il frontend concreto può essere interno o un parser deterministico esterno. Non
è intelligenza esternalizzata: è un sensore formale. Ma il suo output non è
ancora significato.

La KB dichiara:

- quale frontend osserva quale linguaggio;
- come un kind del frontend mappa su un kind astratto;
- quali keyword, delimitatori e regole di scope appartengono al linguaggio;
- quali semantiche sono condivise e quali sono override.

Il C legge byte, invoca/ospita il parser, valida record, assegna id e pubblica
fatti. Non decide che `CallExpr` significhi chiamata, che `def` apra una funzione
o che `await` sospenda: queste sono relazioni della KB. I fatti `code_keyword/2`
già esistenti diventano finalmente consumati invece di duplicare le liste in C.

### 3.3 Una codebase non è soltanto sorgente

Nella stessa memoria entrano, con mode epistemici diversi:

- sorgenti e header;
- README, documentazione, commenti e docstring come **claim dichiarati**, non
  comportamento provato;
- manifest e build graph;
- test come esempi/contratti osservabili;
- output di compilatore, test, sanitizer, coverage e profiler;
- issue e requisiti come Goal/Constraint;
- storia Git come evidenza temporale.

Il lettore documentale e il lettore di codice devono potersi agganciare allo
stesso symbol id. Così «questa funzione deve restare compatibile» da `AGENTS.md`
può diventare una policy sullo stesso oggetto che il call graph descrive.

---

## 4. Dalla domanda a un piano di evidenza

La domanda non deve selezionare un handler. Deve produrre una Task IR, estendendo
quella già usata dal reasoning:

```text
operation   = describe | locate | explain | compare | assess | predict
scope       = repository | file | symbol | span | revision
subject     = entità legata all'indice dei simboli
dimension   = correctness | performance | maintainability | ...
constraints = lingua, formato, budget, non-goal, policy locale
evidence    = ciò che il claim richiede per essere pronunciabile
```

Le forme linguistiche, le dimensioni e i legami di ruolo vivono nella KB. Un
identificatore mai visto entra come entità locale dallo span della domanda,
esattamente come `learning-mesh.md` richiede per gli argomenti nuovi: non serve
una cue per ogni nome di funzione.

Il piano di risposta è:

```text
domanda -> Task IR -> claim candidati -> obblighi di evidenza
        -> query/azione read-only -> prove e controprove
        -> decisione epistemica -> answer plan -> resa KB
```

Stati minimi del claim:

- `observed`: misurato da un oracle o da runtime;
- `proved`: derivato completamente da osservazioni e regole attive;
- `supported`: evidenza forte ma incompleta;
- `suspected`: ipotesi utile che nomina la verifica mancante;
- `refuted`: controevidenza sufficiente;
- `both`: prove in conflitto;
- `unknown`: nessun cammino adeguato;
- `budget_exhausted`: il cammino esiste ma non è stato completato.

Il renderer riceve claim già chiusi e le loro qualificazioni. Non può trasformare
un sospetto in una certezza con una frase più elegante.

---

## 5. Come si forma un giudizio qualitativo senza una libreria infinita di smell

Uno smell è una conclusione, non un sensore. La forma generale è:

```text
criterio applicabile
  + evidenza strutturale/dinamica
  + controevidenza
  + policy e soglia del progetto
  + costo/rischio del cambiamento
  = finding qualificato e ordinabile
```

La KB deve poter dichiarare almeno:

- `quality_dimension` e relazioni fra dimensioni;
- precondizioni di applicabilità del criterio;
- osservazioni che lo supportano o lo smentiscono;
- evidenza minima richiesta per il livello di certezza;
- severità e priorità come funzioni di impatto, probabilità, scope e policy;
- alternative e tradeoff;
- azione informativa che separa due ipotesi concorrenti.

Il C può contare, ordinare, attraversare grafi, calcolare dominanza, intervalli,
frequenze e costi. Non può decidere che una soglia sia “troppo”, che una funzione
“andrebbe refactorata” o che un pattern sia un bug.

### 5.1 «Questo codice andrebbe migliorato?»

Il sistema:

1. lega il soggetto e lo scope;
2. legge le policy del progetto e le dimensioni richieste;
3. enumera i criteri applicabili, non tutti quelli esistenti;
4. raccoglie evidenza e controevidenza;
5. produce un insieme ordinato di finding, ciascuno con prova, confidenza,
   tradeoff e verifica successiva;
6. se nessun criterio domina, lo dice e chiede quale qualità privilegiare.

La risposta non è un “sì” generico e non è una checklist fissa. È un argomento
ricostruibile dalla KB.

### 5.2 «Quale parte è lenta?»

La regola di onestà è più forte: dalla sola sintassi si può trovare un **candidato
costoso**, non una parte lenta.

```text
nessun profilo  -> sospetti statici + richiesta dell'evidenza discriminante
profilo presente -> frame/campione -> span -> node -> symbol -> call path
                  -> ranking inclusive/exclusive con contesto dell'input
```

Gli output dei profiler sono registri osservabili, come `cc` e `pytest` in
`universal-input.md`. Un formato nuovo si aggiunge tramite evidenza/mapping KB;
il consumer produce osservazioni comuni. La decisione su quale profiler usare è
un action schema e una policy, non un `if` nel modulo code.

Una conclusione di performance porta almeno workload, comando/configurazione,
timestamp, unità, sample count e source hash. Senza queste coordinate è una
descrizione, non una misura.

### 5.3 Gli scanner esistenti

`code_symmetry_fix`, `code_find_discarded_result`,
`code_find_cond_asymmetry` e `code_find_case_folding` restano attivi come sensori
secondari. La migrazione corretta è:

1. emettono osservazioni/finding candidati nella IR comune;
2. la KB dichiara criterio, precondizioni, repair rule e obbligo di oracle;
3. un'ablazione della regola KB lascia il sensore muto sul piano decisionale;
4. quando scope/CFG/def-use rendono derivabile lo stesso finding, lo scanner
   resta fallback finché l'evidenza non autorizza il consolidamento.

Non si aggiunge lo scanner numero cinque come via primaria.

---

## 6. L'apprendimento via prompt: estensione del `LEARN_PROTOCOL`

Il protocollo esistente resta vincolante, ma il dominio codice richiede di non
confondere milioni di nodi derivati con crescita della conoscenza.

### 6.1 Che cosa si deve poter insegnare parlando

Un esperto che ignora i predicati interni deve poter dire, per esempio:

- «In questo progetto, il codice eseguito a ogni frame è sensibile alle
  allocazioni.» — policy scoped;
- «Una chiamata pura che restituisce un valore non ha effetto se il risultato
  viene ignorato.» — regola semantica/criterio;
- «Nel formato di profilo X, la colonna self time è il tempo speso direttamente
  nella funzione.» — mapping di evidenza;
- «In questo linguaggio, `defer` esegue l'azione quando si esce dallo scope.» —
  semantica di un costrutto;
- «Con “dove si concentra il tempo?” ti sto chiedendo di localizzare il collo di
  bottiglia.» — nuova forma interrogativa.

Nessuna lezione può nominare `code_edge`, arità, tuple o comandi `!assert`.

### 6.2 Transazione didattica per una regola di comprensione del codice

Per ogni lezione:

1. **Fonte e verità.** Usare specifica ufficiale, documentazione primaria o
   comportamento verificato. Registrare versione e scope.
2. **Baseline naturale.** Porre la domanda su codice reale senza suggerire la
   risposta e classificare `correct/wrong/irrelevant/partial/unknown`.
3. **Lezione naturale.** Spiegare costrutto, ruoli, condizione e limite come a un
   collega, non come allo schema KB.
4. **Self-explanation.** Chiedere che cosa ha capito, a quali elementi del
   sorgente lega i ruoli e quale prova richiederebbe.
5. **Replay.** Riproporre la domanda originale sullo stesso snapshot.
6. **Transfer@3.** Tre casi reali held-out: almeno due repository e, quando la
   regola è astratta, due linguaggi/frontend.
7. **Parafrasi e contrasto.** Due domande equivalenti e un caso simile in cui il
   criterio non deve attivarsi.
8. **Composizione.** Usare la nuova regola con call graph, test, policy o profilo
   già noto.
9. **Ablation parlata.** Ritrarre/correggere la lezione: il finding dipendente
   scompare, i nodi sorgente restano.
10. **Reteach e rilettura.** Ripristinare la verità e far ricalcolare la vista
    sullo stesso snapshot senza riavvio.
11. **Fresh process.** La conoscenza strutturale promossa torna; la cache della
    codebase può essere ricostruita dai byte e deve dare lo stesso proof.

### 6.3 Che cosa si salva e che cosa si conta

- I nodi AST, le chiamate e i sample di profilo **non sono `W`** del
  `LEARN_PROTOCOL`: sono osservazioni revisionate della codebase.
- Una semantica vera di linguaggio/API è conoscenza del mondo e può contare come
  `W` se fontata e persistita.
- Una nuova regola/criterio/costruzione conta in `C`; forme e mapping linguistici
  contano in `L`; provenance e genealogy in `P`.
- Se la sessione produce solo il motore che rende insegnabile una classe, lo
  stato è `meta-capability-only`, non `trained`.
- La policy specifica del repository resta scoped e non diventa verità globale.

Il gate causale resta assert/retract/reteach. Un golden fisso o un AST più grande
non prova apprendimento.

---

## 7. Le leve per accelerare davvero la crescita della KB

La misura non è il numero di facts. È quante nuove decisioni corrette diventano
possibili senza C e quante restano valide su casi freddi.

```text
comprehension_yield = nuovi archi domanda->prova->decisione su held-out
                      / (teacher turn + engine LOC pesate + costo di replay)

rule_fanout         = claim/finding distinti resi possibili
                      / regole strutturali promosse

reread_gain         = nuove letture corrette di sorgenti già viste
                      / lezioni strutturali ricevute
```

Le leve, in ordine di rendimento atteso:

1. **Un solo punto di ingestione, molte viste.** Oggi locate, callers, smell ed
   explain riscansionano. Ingerire una volta e interrogare molte volte elimina
   diagnosi duplicate e rende ogni nuova regola immediatamente moltiplicativa.
2. **Identità + revisione prima del volume.** Senza invalidazione, più codice
   significa più fatti stale. Non si avvia ingestione massiva prima di poter
   sostituire una lettura.
3. **Archi fra rappresentazioni prima di adapter per dominio.** Ogni producer
   pubblica `ir_denotation`; ogni nuova zona dichiara un
   `representation_bridge`; il consumer attraversa il predicato variabile con
   `apply/2`. Misurare `bridge_yield`, riuso in catena e
   `false_composition`, non il numero grezzo di mapping.
4. **Collegare i motori già scritti.** `code_keyword`, input IR, evidence scorer,
   materialized views, compensation e Task IR esistono. Il primo lavoro è farli
   consumare, non progettare il quinto sottosistema.
5. **Completare una relazione quando entra.** Definizione, riferimento, call,
   read/write, control edge ed effect vanno coperti come classi intere su C e
   Python, non come membri scoperti dai prompt.
6. **Curriculum per fan-out.** Scegliere la prossima regola in base al numero di
   domande, linguaggi e consumer che sblocca. Scope/binding precede smell; def-use
   precede “variabile non inizializzata”; evidence obligation precede profiler X.
7. **Una lezione, rilettura di tutto lo snapshot.** Il guadagno migliore non è il
   nuovo fatto insegnato, ma quante vecchie sorgenti diventano comprensibili.
8. **Gap tipizzato → azione informativa.** Riutilizzare `p0_compensate`: mancano
   source, symbol binding, call target, profile, test result o project policy;
   ogni gap sceglie da KB l'azione minima che lo separa dalle alternative.
9. **Indici e viste come condizione di crescita.** Il codice moltiplicherà i
   fatti. Prima della scala servono l'indice per termine già aperto in
   `LEARN_TODO.md` §L e viste materializzate con dipendenze strette per call
   closure, symbol index e CFG. Si profila prima di ogni ottimizzazione.
10. **Cold code, non fixture didattiche, per la promozione.** I piccoli fixture
   restano test meccanici; il training usa sorgente reale e fontato. Il corpus
   SWE già statico offre Python reale, parrot0 offre C reale.
11. **Chiusura al secondo avvistamento.** Stessa regola della comprensione
    naturale: un secondo consumer che riapre bytes o ricostruisce una relazione
    rende il difetto una priorità, non una nota.

Metriche operative aggiuntive:

- percentuale di domande risolte dalla IR senza riaprire la sorgente;
- query P50/P95 per numero di nodi e facts;
- invalidation precision (facts invalidati / facts davvero dipendenti);
- prompt promotion ratio (regole acquisite parlando / regole aggiunte a mano);
- bridge yield (nuovi compiti freddi / nuovi archi) e crescita rispetto alla
  baseline lineare;
- claim cross-rappresentazione privi di `ir_domain_claim_basis` = 0;
- `false_composition` esercitati, non una tabella vuota usata come alibi;
- `Transfer@3`, ablation fidelity e fresh-snapshot replay;
- claim senza provenance = 0;
- “lento” dichiarato senza evidenza dinamica = 0;
- nuovi scanner di smell nel C = 0;
- C/KB balance: il C nuovo deve essere meccanica riusabile e la crescita
  decisionale deve stare nelle regole.

### 7-bis. ⭐ LE LEVE MISURATE (gen489-492) — e come riordinano la lista qui sopra

L'ordine del §7 è per **rendimento atteso**. Questi quattro giorni ne hanno
misurate quattro davvero, e il risultato corregge due punti dell'ordine.

| leva | effetto misurato |
|---|---|
| **collegare motori già scritti** | il lettore strutturale aveva 5 predicati con **zero fatti**; `representation_bridge` ne aveva **zero**. Collegarli: resa di lettura 0,13 → 0,26 fatti/frase, e il ponte cross-dominio da inesistente a vivo |
| **congelare le derivazioni costose** | turno 1058 → **259 ms** (4×), `extract_frame` da 803 ms a 1,9 ms |
| **allargare il corpus** | 232 → 331 `wiki_concept`, 9 → 21 domini; resa 0,26 → **0,37** fatti/frase |
| **completare le classi** | 647 classi-seriale → 74 reali; «is» da 79 classi a 1 |

**⛔ La prima correzione, ed è la più importante: una leva bloccata dal costo non
è una leva finché il costo non è pagato.** Il corpus (leva #10) esisteva ed era
inutilizzabile: aggiungere 50 verbi di relazione alzava la resa di lettura ma
mandava in timeout metà della suite. Solo dopo le viste materializzate la stessa
lista è passata **senza toccare un gate**. Quindi **il #9 (indici e viste) non è
nono: è una precondizione**, e va letto prima di ogni altro punto.

Il corollario operativo, che vale oltre questo piano: *quando una classe di
conoscenza non si può allargare, il difetto non è quasi mai nella classe — si
guarda che cosa la RILEGGE, e quante volte.*

**⛔ La seconda correzione: il #4 («collegare i motori già scritti») è il primo,
non il quarto.** Tre volte in quattro giorni lo stesso pattern — un organo
costruito, documentato, testato in laboratorio, e **mai collegato in
produzione**: `frame_pattern`/`semantic_entity` a zero fatti (gen490),
`representation_bridge` a zero fatti (gen492), il piano dichiarativo
dell'autocorrezione senza esecutore (gen442→gen491). In tutti e tre i casi il
guadagno è arrivato da una manciata di fatti, non da codice nuovo.

**Il sintomo da cercare, ed è cheap:** un predicato citato dalle regole e con
zero fatti in tutta la KB. Nessuna misura di rendimento lo troverebbe, perché
**un organo spento non compare in nessun profilo**: non è lento, non è rotto,
non sbaglia — semplicemente non c'è.

La sonda è `scripts/dead-organs.py`, e al gen492 ne trova **94**. ⚠ Il numero
grezzo sovrastima e va letto sapendolo: molti sono asseriti a runtime (lo stato
del turno, i referenti del discorso, le osservazioni del codice) e la loro
assenza a riposo è corretta. La sonda non dà un verdetto, dà **una lista da
triaggiare**, e il triage è una domanda sola: *«questo chi lo scrive, e
quando?»*. Se la risposta è «nessuno», l'organo è spento — ed è lì che sta il
guadagno più economico che questo progetto abbia misurato.

---

## 8. Roadmap a incrementi verticali

Ogni incremento deve lasciare una capacità interrogabile. Non si costruisce una
grande AST “in attesa” dei consumer: sarebbe di nuovo un piano scritto e non
eseguito.

### UC0 — North star corta e trace semantica

Preparare un piccolo pacchetto freddo di domande su sorgente reale, senza
risposte-template:

- struttura e responsabilità;
- dipendenza e blast radius;
- migliorabilità con motivazione;
- collo di bottiglia con e senza profilo;
- ambiguità e informazione mancante.

Il trace registra Task IR, snapshot, claim, evidence, consumer e tempo. Il gate
non è la frase esatta: è che la risposta citi un proof valido o un gap corretto.
Il pacchetto deve restare puntuale; niente suite lunga.

### UC1 — La sorgente diventa un documento revisionato

Introdurre snapshot/source unit/hash/span/id e pubblicare, per un file C e uno
Python reali, le osservazioni che i frontend sanno già produrre: definizioni e
chiamate. Derivare le viste legacy.

**Gate:** due funzioni omonime in file diversi restano distinte; una rilettura
dopo modifica sostituisce gli archi vecchi; “da dove lo sai?” arriva a file,
hash e span. Nessun fatto source-derived entra in `/save`.

### UC2 — Un solo frontend contract, linguaggi per delta

Spostare keyword e mapping dei kind nelle relazioni KB già predisposte; fare dei
frontend produttori dello stesso record validato. Commenti/stringhe restano
meccanica di lexing, il significato dei costrutti no.

**Gate KB-first:** aggiungere e ritrarre a runtime un mapping di costrutto cambia
la vista astratta senza rebuild. C e Python devono attivare lo stesso consumer
su un costrutto condiviso; un delta resta locale al linguaggio.

### UC3 — Dalla domanda al goal, senza `wants_*`

Far passare le richieste sul codice attraverso `input_structure` + Task IR. Il
subject viene legato dall'indice; operation, dimension, scope e formato sono
ruoli KB. I rami `wants_eval/wants_funcs/wants_calls/qtype` restano adapter
secondari, non la via primaria.

**Gate KB-first:** insegnare parlando una nuova forma di domanda strutturale,
usarla su un simbolo mai menzionato, ritrarla e osservare la perdita della sola
forma. Una domanda con due subject compatibili chiede, non sceglie.

### UC4 — Simboli, scope, binding, reference e indici

Costruire definition/reference/import/include/call su identità qualificate;
risolvere scope e shadowing; mantenere unresolved e ambiguous come stati. Il
filesystem scan diventa producer dell'indice, non strategia di risposta.

**Capacità:** “chi usa X?”, “quale X?”, “dove è visibile?”, “se cambia questa
firma chi va riesaminato?”.

**Gate:** repository con omonimi e decoy; top-k con proof, nessun match per nome
nudo. Nuove relazioni frontend si aggiungono via mapping KB.

### UC5 — CFG, def-use, effetti e summary interprocedurali

Pubblicare basic block/control edge, read/write/definition/use, return/throw e
side-effect evidence. Derivare summary di funzione conservando condizioni e
unknown call. Usare l'evaluator esistente come oracle/sensore, non come memoria
separata.

**Capacità:** spiegare cammini, valori, mutazioni, dipendenze di dati e limiti
della spiegazione.

**Gate:** la risposta “che cosa fa?” è un answer plan di claim derivati; ablare
una regola semantica rimuove la sola parte dipendente della spiegazione.

### UC6 — Motore di criteri e finding qualitativi

Introdurre criterion/applicability/evidence/counterevidence/severity/tradeoff e
far migrare il primo scanner esistente come sensore. Il primo vertical slice
consigliato è “risultato puro ignorato”, perché oggi la conoscenza del metodo
puro è una lista in C e il caso ha un oracle chiaro.

**Gate via prompt:** insegnare naturalmente la regola generale, ottenere il
finding su casi reali held-out, escludere la chiamata mutante, ritrarre la
lezione e perdere il finding senza perdere la IR.

### UC7 — Evidenza dinamica e performance

Normalizzare build/test/coverage/profile in observation + span/symbol; dichiarare
in KB gli obblighi dei claim e le azioni read-only. Separare costo statico da
tempo misurato.

**Gate:** la stessa domanda “dove è lento?” prima del profilo produce un gap
informato, dopo il profilo produce un ranking grounded, dopo un edit invalida il
ranking stale. Workload diverso può produrre un vincitore diverso senza
contraddizione.

### UC8 — Metalinguaggio completo per insegnare analisi del codice

Generalizzare il teaching oltre cue e fatti binari: costrutto → semantica,
osservazione → significato, criterio → evidenza, policy → scope. Il teacher parla
solo lingua naturale e parrot0 deve ridire ruoli, limiti e fonte.

**Gate:** tre lezioni di specie diversa (semantica, criterio, policy), ciascuna
con replay, Transfer@3, contrasto, composizione, ablation, reteach e fresh
process secondo §6.

### UC9 — Rilettura, autocorrezione e active evidence

Collegare i gap del code reasoner a `p0_compensate`: rileggi source, risolvi
binding, acquisisci diagnostica, esegui profilo autorizzato, chiedi policy. Dopo
una lezione o un nuovo dato, rigiocare la stessa domanda e attribuire il
miglioramento alla dipendenza cambiata.

**Gate:** una capacità riconquistata non deve essere riconquistata al turno
seguente; il colpo successivo usa la vista promossa. Un'azione che non separa le
ipotesi non viene ripetuta.

### UC10 — Repository intero: documenti, test e storia

Legare claim dei documenti, contratti dei test, diagnostics e commit ai simboli.
Le frasi di progetto diventano policy scoped; i test diventano evidenza, non
verità universale; la storia risponde al “perché” solo quando il commit lo
sostiene.

**Gate:** una domanda qualitativa usa almeno tre registri indipendenti senza
confonderli: sorgente, policy/documento e osservazione di test/profilo.

### UC11 — Scala e dogfood

Prima si profila su repository crescenti; poi indice per termine, symbol index e
viste materializzate chiudono i predicati dominanti con dependency key strette.
parrot0 legge se stesso come codebase ordinaria, senza facts privilegiati su
`src/brain`.

**Gate:** risposte qualitative su parrot0 e repository Python freddi, proof
riproducibili, query entro budget, invalidazione locale e nessun nuovo ramo
domain-specific nel C.

---

## 8-bis. ✅ FATTO al gen492 — il ponte è VIVO, e il moltiplicatore si vede

> Stato aggiornato dopo l'esecuzione dei punti 1-5 e 7 del §9.

Il meccanismo del §3 esisteva ma era **inerte**: `representation_bridge/4` aveva
**zero fatti**, quindi nessuna query attraversava nessun confine. E la causa non
era il meccanismo — era che il lato codice offriva soltanto **nomi interi**, e un
nome intero non incontra mai la conoscenza di dominio: nessuna KB conterrà mai
`data_structure(hash_table_insert, …)`.

**Il pezzo che mancava: i pezzi.** `code_name_part/4` pubblica all'ingest il
nome intero, i singoli pezzi e — la parte che fa il lavoro — le **coppie
adiacenti**, perché il concetto vero è quasi sempre di due parole: da
«hash table insert» il concetto è `hash_table`, e nessun pezzo singolo lo
raggiunge. Il taglio è meccanica; **quali caratteri separino è conoscenza**
(`identifier_separator/1`), quindi kebab-case o un namespace col punto sono una
riga, non una ricompilazione. Costo pagato all'ingest, mai per turno.

**I quattro ponti vivi**, e nessuno dei quattro è stato scritto pensando al
codice:

| ponte | che cosa attraversa | dove è stata imparata |
|---|---|---|
| `data_structure` | «hash_table è O(1) in lookup» | esperto di algoritmi |
| `algorithm_family` | «quicksort è ordinamento» | esperto di algoritmi |
| `complexity_class` | le classi di costo | esperto di algoritmi |
| `concept_domain` | i 331 `wiki_concept` | **leggendo Wikipedia** (gen490) |

Prova end-to-end, con la KB al massimo:

```text
> what functions does this define: int hash_table_insert(void) { quicksort(); … }
> domain knowledge about hash_table_insert
    Key-value mapping -- O(1) average lookup/insert/delete.
```

Quella frase non è mai stata scritta per parlare di codice. **È il
moltiplicatore:** la stessa conoscenza serve due rappresentazioni senza essere
duplicata, e la lettura di prosa del gen490 ora finanzia anche l'analisi del
codice.

**Ed è opt-in e attribuibile.** `ir_domain_claim_basis/3` conserva il ponte che
ha autorizzato la pretesa: ogni claim si può contestare e ritirare. L'ablazione
nel cricchetto toglie il ponte e mostra che cade **la pretesa e nient'altro** —
l'osservazione del codice resta, il fatto di dominio resta. Senza ponti la IR non
fa nessuna pretesa cross-dominio, che è il comportamento giusto e non un difetto.

Ratchet: `tests/p0t/code/representation_bridge.p0t` (17 assert, KB al massimo per
R1 — un contesto ermetico renderebbe questo file verde per costruzione e insieme
privo di senso, perché con la KB spenta non c'è nessun confine da attraversare).

**Resta del §9:** il punto 6 (rilettura con arco cambiato) è già coperto dal
ratchet UC1; il punto 8 (misura con `/debug`) non è stato fatto.

---

## 8-ter. ✅ FATTO al gen492 — UC2: il giudizio qualitativo, senza libreria di smell

Il §5 chiede un finding qualificato e ordinabile, non un catalogo. La forma
implementata è un fatto solo:

```prolog
quality_criterion(Nome, Dimensione, threshold(Misura, Verso, Soglia)).
```

dove **`Misura` è il NOME di una relazione binaria sulla IR**, non una funzione
cablata: `apply/2` — lo stesso confine a predicato variabile che regge il ponte
fra rappresentazioni — la attraversa. Ne segue la proprietà che conta:
**il motore del giudizio è una regola sola e non nomina nessun criterio.**

Le tre proprietà che distinguono un motore da un catalogo, tutte nel ratchet
`tests/p0t/code/code_quality_criteria.p0t` (16 assert):

1. **La soglia è conoscenza.** Alzata sopra il misurato, il giudizio cade — e
   non perché il codice sia cambiato, ma perché è cambiata la politica del
   progetto. Il C può contare e confrontare; non può decidere che 8 sia troppo.
2. **Un criterio nuovo è una relazione più un fatto.** Il ratchet ne dichiara
   uno *a runtime* (`short_body` + `tiny_body`) e il motore lo usa senza sapere
   che esiste. Zero C, zero rami nuovi.
3. **Un giudizio che non si può smentire non è conoscenza.** `criterion_waiver`
   dichiara che cosa neutralizza un criterio: l'osservazione resta, il finding
   non si forma. Un criterio incontestabile è un verdetto, e un verdetto non si
   discute — quindi non è conoscenza.

Prova end-to-end:

```text
> what functions does this define: int wide(void) { a(); b(); … i(); return 1; }
> improvement note for wide
    Wide fanout — code definition fanout = 9.
```

⚠ **Due limiti trovati costruendolo, entrambi scritti accanto al codice:**
l'arità del motore è **4**, quindi la soglia sta in un termine composto
(`threshold/3`), che è comunque la forma più leggibile; e `naf/1` rifiuta un
goal non ground — giusto, ma vuol dire che la guardia della controevidenza non
può portarsi dietro la variabile della ragione, che si chiede a parte.

⚠ **E un limite di superficie che vale come regola generale:** le parole della
domanda devono essere **parole che parrot0 conosce**, altrimenti il turno viene
preso dal percorso che offre di imparare il termine ignoto e la domanda non
arriva mai al consumer. Non è un frasario: è vocabolario, ed è una riga di
`lexeme/1`.

**Non ancora fatto del §5:** severità e ordinamento fra finding, il tradeoff,
l'azione informativa che separa due ipotesi, e tutto il §5.2 (la latenza, che
senza profilo può produrre solo *candidati costosi* — mai «questa parte è
lenta»).

---

## 8-quater. ✅ FATTO al gen493 — la demo, e i due difetti che ha scoperto

`make demo` + `tests/fixtures/demo-code-review/` (README guidato, ratchet
`tests/p0t/code/demo_code_review.p0t` a 13 assert, idempotente).

Costruire la demo ha trovato **due difetti che nessun test aveva visto**, e
sono entrambi della specie «l'organo c'è ma non è collegato»:

1. **⛔ parrot0 girava solo dalla radice del repository.** Tutti i percorsi della
   KB sono relativi, quindi lanciandolo nella cartella di un progetto da
   analizzare non caricava niente — e il primo turno rispondeva `wall_classic`,
   cioè la **chiave** di un template invece della sua frase. Due difetti in uno,
   e il secondo è peggiore: un marcatore interno uscito come risposta.
   *Un agente che gira solo dalla propria sorgente non è un agente.* Ora la
   radice si risolve da `PARROT0_ROOT`, dalla cwd o risalendo dal binario —
   senza `chdir`, perché la cartella di lavoro dell'utente è dove stanno i file
   di cui vuole parlare.

2. **⛔ Leggere un sorgente non rendeva i suoi nomi nominabili.** Ogni domanda su
   un simbolo appena letto cadeva nel declino della parola ignota: parrot0 aveva
   messo `hash_table_get` nella propria KB e un turno dopo diceva di non
   conoscerlo. Chiuso con una regola — `lexeme($N) :- code_name($S, $Nd,
   definition, $N)` — così il vocabolario **nasce con l'osservazione e sparisce
   con lo snapshot**, senza che nessuno debba ricordarsi di ritirarlo.

E una terza cosa, che è un miglioramento e non un difetto: **«ho guardato e non
ho trovato niente» non è «non so cosa sia»**. Un'assenza di finding dichiarata è
informazione; il muro della parola ignota al suo posto era falso due volte.

### UC2b — dalla sola sintassi non si dice «lento»

Implementata la regola di onestà del §5.2: `speed of X` senza profilo **dichiara
che manca** e offre ciò che la struttura può dare; `cost candidate for X` lo dà,
e non usa mai la parola «lento». `perf_evidence/3` non ha fatti, e non è una
dimenticanza: è il punto in cui un profiler si collega.

---

## 9. Primo incremento esatto da eseguire

Il prossimo lavoro non è un altro smell e non è un corpus massivo. È **UC1, in
verticale**, usando soltanto ciò che i frontend già sanno vedere.

1. Definire il contratto minimo snapshot/source unit/node/edge/provenance in un
   file machinery dedicato.
2. Far pubblicare a `code_ingest` e `code_ingest_py` definizioni e chiamate con
   file, hash, span e scope; nessuna nuova semantica.
3. Proiettare i nomi in `ir_denotation` e attraversare almeno un
   `representation_bridge` con `apply/2`, conservandone la basis; ablation
   dell'arco elimina il claim ma non l'osservazione né il fatto di dominio.
4. Derivare `code_function` e `code_calls` come compatibilità, evitando il
   namespace globale per nome nudo.
5. Far rispondere quattro domande dalla nuova IR: definizioni, chiamate, luogo e
   provenance.
6. Rileggere una revisione con un arco cambiato e provare che il vecchio arco non
   è più dimostrabile.
7. Insegnare via prompt una parafrasi nuova della domanda di provenance;
   retract/reteach devono cambiare la lettura senza rebuild.
8. Misurare facts prodotti, tempo ingest/query e predicati più costosi con
   `/debug`; non ottimizzare ancora a intuito.

Questo incremento è infrastruttura fertile: se non apre la rilettura e la forma
insegnabile, non si dichiara “comprensione del codice”. Il gate deve essere un
solo `.p0t` focalizzato più, se si modifica il motore, un solo `make soft-test`.
La suite lunga resta fuori da questa sessione per direttiva di F.

---

## 10. Stop condition e anti-impostor

Fermarsi e correggere l'architettura se accade uno di questi eventi:

- una domanda nuova richiede un nuovo `if`/`qtype` nel C;
- un criterio qualitativo o una soglia compare come letterale nel motore;
- una risposta dice “lento” senza un profilo riferito a workload e snapshot;
- un consumer riapre il sorgente per ricostruire una relazione già pubblicata;
- una nuova lingua duplica gli analizzatori invece di emettere la IR comune;
- una rilettura accumula facts incompatibili dello snapshot precedente;
- una lezione richiede nomi di predicato o non ha ablation;
- i nodi derivati vengono contati come crescita della conoscenza;
- si aggiunge uno scanner per rendere verde un caso SWE;
- il C cresce più della KB decisionale senza introdurre una meccanica generale;
- si alza un timeout invece di profilare il predicato dominante;
- il piano produce dati che nessun consumer usa nello stesso incremento.

---

## 11. Definizione di arrivo

Il livello nuovo è raggiunto quando, su una codebase fredda e senza hint di
file/funzione, parrot0 può:

1. costruire e aggiornare la propria IR revisionata;
2. interpretare una domanda qualitativa dalla Task IR;
3. esporre almeno due ipotesi quando l'evidenza non decide;
4. acquisire l'osservazione minima che le separa;
5. produrre un giudizio con proof, controevidenza, confidenza e limiti;
6. rispondere “da dove lo sai?” fino ai byte, al documento o all'oracle;
7. imparare via prompt un nuovo criterio o costrutto e trasferirlo su tre casi
   reali held-out;
8. perderlo con ablation e riacquisirlo con reteach;
9. rileggere una revisione senza conservare conclusioni stale;
10. fare tutto questo senza aggiungere un recognizer o uno smell specifico in C.

La frase guida è:

> **Il sorgente non viene riconosciuto: viene osservato. La domanda non sceglie
> un handler: apre un obbligo di evidenza. Il giudizio non è una risposta
> memorizzata: è una conclusione della KB che sa indicare perché vale, rispetto a
> quale snapshot e che cosa potrebbe smentirla.**
