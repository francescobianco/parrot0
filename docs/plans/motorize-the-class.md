# Motorize the Class — missione LLMSCORE-max

> **Principio unico:** *motorizza la classe, poi nutri il motore su scala — non
> memorizzare istanze.* Ogni classe che parrot0 vince oggi è un MOTORE su una KB
> ampia (rime → `wordquery` + 35k `lexeme`; aritmetica → `wordproblem`). Ogni
> classe che perde è un LOOKUP su una tabella sparsa (`explanation`=4,
> `because`=25, `qa_reply`=6). Il fix è convertire ogni classe fragile da
> *tabella* a *motore + ingestione*, non aggiungere righe alla tabella.

Coerente con lo steer di F.: **meno moduli, più KB**. Un motore causale è UN
motore per l'intera classe "perché" — l'opposto di N moduli-qa. I motori sono
C (motori, non frasi); la conoscenza cresce nella KB per processo.

## Missione aggiornata — massimizzare il punteggio atteso, non il run visto

LLMSCORE non misura una checklist fissa: campiona 10 richieste da una coda lunga
di comportamenti LLM-like. Dopo gen350 il probe interno può dire **70/70 answered**
e il run ufficiale può ancora cadere a **4/10**. Questo non contraddice il piano:
dimostra che il vecchio metro primario, il wall-rate, misurava solo la prima
frontiera. La missione ora è più stretta:

> **Massimizzare il valore atteso di LLMSCORE sotto alta varianza, riducendo sia
> muri sia risposte sbagliate, tramite crescita KB-first di classi componibili.**

La domanda guida non è più soltanto "parrot0 sa ingaggiare questa classe?" ma:

1. **completezza multi-intento:** se il prompt chiede A **e** B, la risposta deve
   coprire entrambe le slot, non fermarsi al primo frame che matcha;
2. **fedeltà di formato:** se il prompt chiede due righe, tre modi, una lista, una
   frase semplice, il formato è parte del task, non decorazione;
3. **calcolo semanticamente tipato:** numeri e operazioni non bastano; servono gli
   slot giusti (`price`, `change`, `width`, `length`, `digit_count`);
4. **fatti bundle/compositi:** le domande factuali spesso combinano capitale +
   motivo storico, anno + entità correlate, opera + autore + contesto;
5. **anti-collisione di routing:** il primo modulo che risponde può ancora essere
   quello sbagliato. Il routing deve imparare a declinare quando un cue più
   specifico o compositivo è presente.

Quindi il piano non mira a inseguire il prossimo `LLMSCORE.md`. Usa ogni failure
come campione da una distribuzione latente e chiede: *quale famiglia di prompt ha
appena esposto? quale rappresentazione KB la copre a pioggia? quale test runtime
dimostra che la superficie è insegnabile senza rebuild?*

## Diagnosi gen351 — perché 0% wall non basta

Ultimo campione osservato: **4/10**. I failure non sono "mancano sette risposte":
sono segnali di classi deboli.

| Failure | Sintomo | Classe latente da motorizzare |
|---|---|---|
| Poema due-linee | contenuto buono, formato sbagliato | post-shaper di formato come KB task constraint |
| Rettangolo perimetro/lunghezza doppia | numero arbitrario, slot mancanti | algebra schema-backed: variabili, equazioni, output dimensionale |
| Canberra compromesso | prima risposta factual vince e tronca il "why" | domanda composta: answer plan con più subgoal |
| Apples + change | calcola prezzo ma non resto | chain aritmetica multi-step con ledger di quantità |
| WWII + atomic cities | risponde all'anno ma non alle entità correlate | factual bundle/event frame |
| Digit 7 tra 1 e 100 | aritmetica generica sbaglia count combinatorio | enumeratore/count procedure per digit/range |

Pattern comune: parrot0 oggi eccelle nei **single-goal frames**. LLMSCORE campiona
sempre più spesso **compound goals**: task + formato, fatto + spiegazione, calcolo
seguito da secondo calcolo, evento + attributi multipli. La prossima frontiera è quindi una
KB che non memorizza solo fatti atomici, ma anche **piani di risposta**: frame che
dicono quali subgoal devono essere soddisfatti prima di parlare.

## Mantra operativi — da eseguire PRIMA di ogni passo

Regole imposte a noi stessi per non ricadere nel fix puntuale. Prima di scrivere
*qualsiasi* riga, si passa questa checklist. Nate dagli errori reali di gen348-349.

1. **"È generalizzabile KB-first?"** — la domanda zero. Non fixare l'istanza,
   motorizza la CLASSE. Se la risposta è "sì ma è più lavoro", si fa il lavoro.
2. **Niente liste di parole nel C.** Trigger, cue, sinonimi, unità, verbi: sono
   fatti KB enumerabili (`causal_process_verb/1`, `verb_syn/2`, `time_unit/1`).
   Test: *"parrot0 può impararne un nuovo membro domani senza ricompilare?"*
   **Estensione procedure:** se il fix introduce una trasformazione o un calcolo
   di classe, non fermarti a mettere cue/template in KB: cerca prima una procedura
   insegnabile in `kb/core/procedures.p0` sopra i primitivi (`is/2`, confronti,
   termini composti). Il C deve restare adattatore NL→goal o primitiva generale,
   non consumer della procedura di dominio.
3. **Astrai fino al punto fisso.** Non moltiplicare *predicati* per una relazione
   vista attraverso verbi diversi: `wrote`/`painted`/`composed` = UNA relazione
   `created_by(Creator, Work, Verb)`, il verbo è un campo. Chiedi: *"relazione
   NUOVA o STESSA relazione sotto un'etichetta diversa?"*
4. **Cerca il motore esistente prima di scriverne uno nuovo.** (Avevo duplicato
   `transitive_comparison` senza accorgermene.) `grep` prima di scrivere.
5. **Non estendere per analogia col codice esistente.** Se c'è già `wrote/2`,
   NON aggiungere `painted/2` per riflesso: ri-derivare dallo scheletro, o
   propaghi il debito di disaggregazione.
6. **Uccidi il muro, MAI con una risposta sbagliata.** Un errore factuale è
   peggio di un muro (dottrina no-deception). Nel dubbio, declina.
7. **Attenzione ai cue substring.** `cue()` è substring: "eat" ⊂ "f-EAT-hers".
   Per i cue discriminanti, match a PAROLA INTERA.
8. **Il wall-rate non vede le risposte sbagliate.** Quando tocchi una classe,
   ispeziona a mano anche le risposte marcate "ok".
9. **Nessuna risposta prima di aver soddisfatto il piano.** Se il prompt contiene
   più richieste coordinate, il modulo deve costruire un `answer_plan` o declinare.
   Vietato rispondere al primo subgoal e ignorare il resto.
10. **Il formato è un vincolo semantico.** "two-line", "three ways", "one
    sentence", "simple terms", "as a list" vive in KB come `format_constraint/2`
    o relazione equivalente. Il post-shaper deve provare il formato.
11. **Ogni numero deve avere un ruolo.** Prima di fare aritmetica, lega i numeri a
    slot (`total`, `unit_price`, `paid`, `width`, `length`, `range_low`). Se non
    sai il ruolo, non calcolare.
12. **Preferisci event frames ai fatti sparsi.** Una domanda su WWII non è solo
    `ended_in(world_war_ii, 1945)`: è un evento con anno, luogo, attori, cause,
    conseguenze e oggetti correlati. La KB deve crescere per frame interrogabili.
13. **Ogni collisione diventa una guardia teachable.** Se un frame generico vince
    su uno specifico, non riordinare a mano soltanto: aggiungi un cue/registro KB
    che fa declinare il generico davanti alla classe compositiva.
14. **Un failure LLMSCORE vale come seed di fuzzing.** Dopo il fix, genera varianti
    della classe: sinonimi, ordine invertito, numeri diversi, multiword entities,
    formato diverso. Il test non deve coprire il prompt, ma il fascio.

## Evoluzione KB richiesta per LLMSCORE-max

La KB di parrot0 deve passare da "grande dizionario di fatti interrogabili" a
"substrato di piani, procedure e frame compositivi". Le nuove famiglie di fatti
da far crescere sistematicamente:

| Famiglia KB | Scopo | Esempio |
|---|---|---|
| `format_constraint/2` + `format_realizer/2` | rendere verificabile il formato richiesto | `format_constraint(two_line, "two-line")` |
| `answer_plan/3` | mappare un task composto a subgoal obbligatori | `answer_plan(capital_and_reason, [capital, reason])` |
| `quantity_role/3` | assegnare ruolo ai numeri prima del calcolo | `quantity_role(change_problem, paid, "$20")` |
| procedure in `procedures.p0` | portare calcoli combinatori fuori dal C | `digit_count_between(D,Lo,Hi,N)` |
| `event_frame/1` e `event_attr/3` | interrogare eventi come pacchetti coerenti | `event_attr(wwii, atomic_bomb_cities, ...)` |
| `relation_registry/*` | dichiarare quali relazioni un motore può usare | `analogy_relation(used_for)` |
| `compound_guard/2` | far declinare frame generici se c'è un task più ricco | `compound_guard(answerframe, border_intersection)` |

Questa crescita è "a pioggia" solo se ogni fatto aumenta il prodotto cartesiano:
un nuovo `format_constraint` vale per poesia, story, explain, advice; una nuova
procedura di range vale per digit-count, letter-count, prime-count; un event frame
vale per anno, luoghi, cause e domande composte.

## Nuovo ciclo operativo LLMSCORE

1. **Leggi il failure come distribuzione.** Non chiedere "come rispondo a questa
   frase?", ma "quale famiglia di frasi avrebbe lo stesso errore?".
2. **Classifica l'errore:** wall, fatto mancante, procedura mancante, formato,
   routing collision, compound incompleto, risposta falsa.
3. **Disegna il frame KB:** cue, slot, subgoal obbligatori, template, procedure.
4. **Collega il C solo come adattatore:** parse NL→goal, ordering, slot binding,
   primitive generali. Nessuna parola naturale nuova nel C.
5. **Espandi a pioggia:** aggiungi fatti/procedure/registri che coprono varianti
   future della classe, non solo l'istanza.
6. **Ablazione runtime:** assert/retract del cue o registro che prova la crescita
   senza rebuild.
7. **Fuzz focused:** 5-20 varianti della classe, inclusi ordini e sinonimi.
8. **Ricontrollo manuale delle "ok":** wall-rate verde non basta.

## Perché i fix puntuali perdono

Il giudice campiona 10 domande da una distribuzione illimitata. I run osservati
condividono ~0 domande. Una coppia `qa_cue/qa_reply` copre misura-zero →
valore atteso ~0 sul run successivo. Ottimizziamo il **punteggio atteso sulla
distribuzione**, non il prossimo campione di 10. Dopo gen351 questo vale anche
per i **fix di classe troppo stretti**: una batteria interna può diventare verde
mentre il giudice campiona fuori dal bordo. Ogni fix deve allargare il bordo.

## Tassonomia delle classi e stato (baseline)

| Classe | Meccanismo | Copertura | Stato |
|---|---|---|---|
| Lessicale (rime/anagrammi) | motore `wordquery` + `lexeme` | qualsiasi parola | ✅ robusto |
| Aritmetica / word-problem | motore `wordproblem` | qualsiasi calcolo | ✅ robusto |
| Deduzione formale (sillogismi) | motore no-overlap (A+E) | forme A/E | 🟡 parziale |
| Generazione creativa | composer + fatti-tema | solo temi con fatti | 🟡 semi |
| Causale ("perché/come") | lookup `explanation/because` | ~30 istanze | ❌ fragile |
| Enciclopedico (fatti) | fatti a mano | capitali 175, resto sottile | ❌ fragile |
| Riddle laterali | lookup `qa_reply` | 6 istanze | ❌ fragile |
| Pragmatica / persona | insieme di fatti persona | comuni | 🟢 ok |
| Formato/post-shaping | vincoli parziali | alcune forme | ❌ fragile |
| Compound question | primo frame che vince | A o B, raramente A+B | ❌ fragile |
| Algebra schema | arithmetic generica | calcoli scalari | ❌ fragile |
| Event frames | fatti isolati | anno/capitale singoli | ❌ fragile |

**Insight:** vinciamo dove abbiamo un motore su KB ampia; perdiamo dove abbiamo
un lookup su tabella sparsa.

## Leva combinatoria (da `generative-leverage.md`)

Un motore non ha bisogno di *migliaia* di fatti: ha bisogno di **fatti atomici
componibili** il cui prodotto cartesiano copre la classe.

- **La forma dell'output è già nel prompt** → il motore fa *selezione vincolata*,
  non recupero. Risolve un predicato parzialmente istanziato (es.
  `haiku(ocean, $O, $M, $C)`) e sceglie una tupla valida.
- **Prodotto, non somma:** 3 open × 3 mid × 3 close = 27 haiku; ~17 atomi
  narrativi = 486 storie. Aggiungere una variante è UN fatto; aggiungere uno
  slot è un fatto + un arco.
- **Garanzie per costruzione:** grammatica (linee pre-composte), coerenza
  (atomi etichettati per ruolo/tema), novità (l'assemblaggio esatto non è mai
  pre-scritto).
- **Corollario per la Fase 1 causale:** una spiegazione si *compone* da atomi
  relazionali (`cause` + `mechanism` + `purpose` + catena `is_a`), non da una
  stringa monolitica per topic → stessa leva prodotto, stessa economia.

Regola pratica: ogni classe generativa = UN motore + fatti atomici componibili.
L'anti-pattern è un modulo (o una `qa_reply`) per istanza.

## Le tre leve

- **(a) Motori generativi** che assemblano la risposta da relazioni strutturate
  (query sul grafo KB), così *qualsiasi* istanza della classe è tentabile.
- **(b) Ingestione per processo** (pipeline `kb/learning/` già esistente): la
  breadth viene dall'ingestione da Wikipedia statica, non dal digitare fatti.
- **(c) Uccidere il muro.** Ogni "I don't understand that yet" è uno 0 garantito
  *e* un tell squalificante. Tentativo plausibile in-dominio > muro — **senza
  inventare fatti falsi**; si declina solo in modo LLM-plausibile e onesto.

## Le fasi (ordinate per valore atteso)

### Fase 0 — Misura la CLASSE, non il campione *(prerequisito)*
Batteria di sonde per-classe (`tests/llmscore-probes/`), held-out, **mai
fixate singolarmente**. Metro oggettivo e senza-API: **wall-rate per classe**
(rileva le frasi-muro note). Un secondo metro opzionale con giudice-LLM per la
qualità. Deliverable: `make llmscore-probe` → tabella copertura per classe.

Frasi-muro rilevate: `I don't understand`, `I don't know about … yet`,
`Want me to learn`, `not sure`, `no idea`, `haven't learned`.

### Fase 1 — Motore causale "perché/come" *(EV più alto)*
Parser generico `why does X <verb>?` / `how does X work?` → query su
`cause/causes/mechanism/purpose/enables` + catene `is_a/has_part` → compone una
frase causale. Poi estrazione bulk cause→effetto dalle wiki-pages
(`docs/plans/extract-knowledge-from-prose.md`). Da ~30 a migliaia, per processo.

### Fase 2 — Ampiezza enciclopedica per ingestione
Autolearn puntato su classi di entità ad alta frequenza: superlativi
("il più alto/lungo"), autori↔opere, invenzioni↔date, capitali (già). Recall
factuale scala col processo.

### Fase 3 — Completezza deduttiva *(parziale)*
Quadrato delle opposizioni completo (all/some/no × aff/neg) + catene 2-3 passi.
Investimento limitato, alta certezza: copre l'intera classe puzzle-logici.
- [x] **Forma-E con istanza** ("No A are B. Z is a B. Is/Can Z a A? → No"):
      un pattern copre fish/whale, reptiles/snake, cats/Rex. deduction 50%→20%.
- [ ] Barbara con predicato ("All A are B. All B need W. Do A need W? → Yes").
- [ ] Darii ("Some A are B. All B are C. Are some A C? → Yes").
- [x] **RISPOSTE SBAGLIATE corrette**: "birds have feathers → Yes" (il cue
      substring `eat` scattava su "f-EAT-hers" → match a parola intera + Barbara
      con istanza); "taller-than → C/A" (esteso `transitive_comparison` alla
      forma superlativa "who is the shortest/tallest?", estremo per polarità
      dello stem comparativo vs superlativo).

> **Limite del metro (Fase 0):** il wall-rate conta i muri, NON le risposte
> sbagliate. Una risposta errata è *peggio* di un muro (dottrina no-deception)
> ma non appare nel wall-rate. Serve un secondo metro (giudice o oracolo per-
> classe) che marchi le risposte errate. Fino ad allora, ispezionare le
> risposte "ok" a mano quando si tocca una classe.

### Fase 4 — Layer di framing/formato (universal-input)
Verbi di cornice (describe/explain/write/tell) e vincoli (in one sentence /
in N words / as a list) trattati come **modificatori** instradati sul contenuto,
mai come blocchi. Post-shaper generico impone il formato.

### Fase 5 — Fallback anti-muro
Ultimo stadio generativo che ingaggia la domanda in-dominio invece di murare.

### Fase 6 — Answer planner per domande composte *(nuova priorità alta)*
Costruire un planner leggero che riconosce cue coordinati e produce subgoal
obbligatori. Esempi:

- `capital(X)` + `why_created_as_compromise(X)` → risposta con capitale **e**
  motivazione;
- `event_end_year(world_war_ii)` + `event_attr(world_war_ii, atomic_bomb_cities, Cities)`;
- `price_for_quantity` + `change_from_payment`.

Il planner non deve inventare: se un subgoal manca, deve dire quale parte sa e
quale manca, non fingere completezza. La forma della risposta sta in
`response_template` o in un composer di subgoal.

### Fase 7 — Format contract layer *(nuova priorità alta)*
Separare contenuto e rendering. Il motore produce una struttura (`poem_line/2`,
`continuation/3`, `explanation/1`), il post-shaper applica il vincolo:

- due righe = output con newline reale e due unità;
- tre modi = tre elementi distinti;
- one sentence = un solo periodo;
- simple terms = lessico breve, niente prova lunga;
- exactly N words = verificatore già esistente come procedura di formato.

Il contratto deve essere testabile: non basta contenuto corretto.

### Fase 8 — Algebra e word-problem schemas
Portare in KB gli schemi ricorrenti, non solo le operazioni:

- rettangolo: `perimeter_rectangle(L,W,P)`, `twice(L,W)`, solve `L=2W`;
- unit price + total quantity + change;
- mixture/ratio/rate multi-step;
- digit/range counting.

Il C lega quantità e ruoli; `procedures.p0` risolve. Ogni schema deve avere prove
con numeri diversi e varianti lessicali insegnabili.

### Fase 9 — Event-frame ingestion
L'enciclopedico LLMSCORE non chiede solo fatti singoli. Serve ingestione verso
frame:

```
event_frame(world_war_ii).
event_attr(world_war_ii, end_year, 1945).
event_attr(world_war_ii, atomic_bomb_cities, cons(hiroshima, cons(nagasaki, nil))).
event_attr(canberra, created_as_compromise_between, cons(sydney, cons(melbourne, nil))).
event_attr(canberra, compromise_reason, "Sydney and Melbourne both wanted the capital, so a planned inland capital was chosen between them").
```

La pipeline deve preferire fatti collegati dallo stesso evento, perché il giudice
campiona domande con più slot correlati.

### Fase 10 — Oracolo qualità e anti-wrong
Il wall-rate resta utile ma subordinato. Servono due metriche nuove:

- **completion-rate:** percentuale di subgoal obbligatori soddisfatti;
- **wrong-rate:** risposte factualmente o semanticamente errate.

Finché non c'è giudice automatico, ogni failure LLMSCORE aggiunge un oracle
deterministico per la classe (`has`, `lacks`, formato, numeri, subgoal).

## Criteri di misura (definizione di "fatto")

- **Wall-rate per classe** ↓ (metro primario Fase 0, senza API).
- **Copertura per classe %** (attempt plausibile) tracciata run-su-run.
- **Completion-rate per compound question** ↑: tutti i subgoal richiesti presenti.
- **Format-pass rate** ↑: il rendering soddisfa il vincolo esplicito.
- **Wrong-rate** ↓: risposte sbagliate contano più dei muri.
- LLMSCORE ufficiale come controllo esterno, NON come metro di sviluppo
  (non-deterministico, campione piccolo).

## Ordine di esecuzione raccomandato

Fase 6 + Fase 7 prima: gli ultimi run mostrano che il collo di bottiglia non è
più solo "muro", ma incompletezza e formato. Fase 8 subito dopo per tagliare le
risposte numeriche sbagliate. Fase 9 in parallelo come crescita KB a pioggia.
Fase 10 deve accompagnare ogni modifica.

## Bug noti (rimandati) — da riprendere

Trovati durante gen349 e consapevolmente NON risolti ora. Ognuno viola almeno un
mantra; annotati per riprenderli senza riscoprirli.

- **B1 — il decomposer spezza unità semantiche su " and ".** "Romeo and Juliet"
  → "romeo" + "juliet"; e i word-problem multi-frase vengono divisi. Aggravante:
  `mod_wordproblem` legge il turno intero da `input`, non il sub-clause → rispose
  due volte ("15 dollars. 15 dollars."); tamponato con un dedup, ma la radice è
  che il decomposer divide ciò che è UNA unità. Serve una guardia: non decomporre
  quando il turno è un problema/entità singola (numeri+cue matematico, o titolo).
- **B2 — RISOLTO gen350: `answer_frame`/`created_by` fallivano sui titoli multi-parola sotto
  "the".** "Mona Lisa", "Starry Night" murano (Guernica, David — parola singola —
  funzionano). Un blocco precedente in `mod_knowledge` intercetta "the <X multi>";
  inoltre il motore `created_by` è piazzato tardi. Serve: estrazione del titolo
  multi-parola + anticipare il motore. Ora `created_by` normalizza le forme del
  verbo via `creation_verb_form/2` e salta i determinanti con `p0_lead_det()`.
- **B3 — `kb_match` arity-1 completamente LEGATO non riporta l'hit.** Una query
  `time_unit(hour)` con arg legato torna 0 anche su match; ho dovuto ENUMERARE e
  confrontare. Quirk del motore KB: verificare se è un bug generale di `kb_match`.
- **B4 — RISOLTO gen350: "make a word from the letters r,e,a,d" va a `mod_compose`** invece di
  `mod_wordquery` (precedenza di routing: "make a word" → code synth). Non è un
  muro (risposta non-muro), ma è una non-risposta. Il routing lessicale copre la
  probe e l'anagramma esplicito `anagram_of/2` vince sull'enumeratore.
- **B5 — `cue()` è substring, ovunque.** Fixato eat/feathers, ma il pattern può
  ripresentarsi su altri cue discriminanti. Valutare un `cue_word()` a parola
  intera come default per i cue discriminanti.
- **B6 — sillogismo affermativo con pronome mangia il testo.** "Every square is a
  rectangle. This shape is a square. Is it a rectangle?" → "Yes -- Is is a
  Rectangle" (giusto ma sgrammaticato: "it" diventa "Is").
- **B7 — RISOLTO gen350: le parole-cue della dieta (eat/eats/diet/food) erano una lista in
  C** (le ho rese whole-word ma restano cablate) → viola mantra #2, portarle in KB.
  Ora sono `diet_cue/1`, con test assert/retract runtime.
- **B8 — il wall-rate non conta le risposte sbagliate** (vedi Fase 0). Serve un
  secondo oracolo per-classe che marchi gli errori.
- **B9 — RISOLTO gen350: estrazione CREATION solo attiva.** "The Odyssey was written by Homer"
  (passivo) non viene estratto (declina, non sbaglia). Aggiungere il frame
  passivo "O was <verb>ed by S" → `created_by(S, O, verb)`. Ora il passivo usa
  `creation_verb_form/2` + `creation_passive_agent_marker/1`.
- **B10 — messaggio "Learned: mammal.(platypus)"** ha un punto spurio nel nome
  predicato (cosmetico, pre-esistente all'estrazione copula).
- **B11 — formato poesia ignorato.** "two-line rhyming poem" genera una sola riga.
  Serve Format contract layer: newline reale + conteggio righe.
- **B12 — wordproblem rettangolo collassa a scalare.** "perimeter 24, length twice
  width" risponde `48`. Serve schema algebraico con ruoli e output dimensionale.
- **B13 — domanda composta capital+why tronca al primo fatto.** `answerframe`
  risponde Canberra e ignora la spiegazione storica. Serve answer planner e guardia
  contro first-frame incomplete.
- **B14 — prezzo+change tronca al primo calcolo.** Serve ledger quantità con
  subgoal obbligatori: prezzo totale e resto.
- **B15 — event bundle WWII incompleto.** L'anno è noto, le città atomiche no o non
  vengono aggregate. Serve `event_frame/event_attr`.
- **B16 — digit count fra range usa aritmetica sbagliata.** Serve procedura
  combinatoria/enumerativa `digit_count_between/4`, non calcolo generico.

## Stato

- [x] Fase 0: batteria sonde + `make llmscore-probe` + baseline wall-rate.
      **Baseline (2026-07-22): 31% wall overall (48/70 risposte).**
      Per classe: deduction 50%, causal 40%, factual 40%, arithmetic 30%,
      creative/lexical/pragma 20%. Conferma le priorità.
- [x] Fase 1: motore causale + ingestione cause→effetto.
      **Diagnosi:** il motore ESISTE (`10-memory-knowledge.c:4302`, key =
      content-word joined → `because/2`/`explanation/2`). I wall causali sono:
      (a) "how does X form/work?" non instrada al motore (solo "why");
      (b) key brittle: `rainbow_form` ≠ `rainbow_appears` (verb-synonymy);
      (c) fatti mancanti (hiccups, tides, stars_twinkle, onions).
      **Cautela:** match a sovrapposizione lasca dà risposte SBAGLIATE
      (moon→`moon_glows` per "tides") — peggio di un muro. Il fix va fatto
      con canonicalizzazione verbo-sinonimi + guardia `make llmscore-probe`,
      NON con overlap ingenuo. Prossimo passo: (1) routing "how does X"
      sicuro; (2) ingestione cause→effetto subject-keyed da `kb/learning/pages`.
- [x] **gen350:** routing "how does X form/cause/work" e causal lookup robusto
      coprono la batteria causale: 10/10 answered, 0% wall. Le forme restano KB:
      `causal_process_verb/1` e `verb_syn/2`; i fatti mancanti sono `because/2`.
- [x] Fase 2: ingestione enciclopedica mirata. **AVVIATA (gen349):** estrazione
      CREATION transitiva KB-first in `extract_class_statement` — la prosa
      "S <creation-verb> O" → `created_by(S,O,Verb)`, verbi da `creation_verb/1`.
      Testato end-to-end: "Claude Monet painted Water Lilies" → fatto estratto →
      "Who painted Water Lilies?" → "Claude monet". La base factual cresce per
      PROCESSO, non a mano. **gen350:** aggiunto passivo `O was <form> by S`
      tramite `creation_verb_form/2` e marker KB; aggiunto `created_by` per
      `romeo_and_juliet`; "Who painted the Mona Lisa?" e "Who wrote Romeo and
      Juliet?" rispondono via relazione astratta.
- [x] Fase 3: quadrato opposizioni completo sulla batteria: deduction 10/10,
      0% wall.
- [x] Fase 4: layer framing/formato sulla batteria: `concise_explain/3` ora copre
      anche richieste vincolate da word-count come "exactly three words"; advice
      usa `intent_cue(activity_request, ...)` + `activity_topic/activity_step`.
- [x] Fase 5: fallback anti-muro sulla batteria: creative/pragmatic gaps chiusi
      con motori KB (`metaphor_line/metaphor_topic`, `story_scene`,
      `continuation_template`, `activity_step`) e test runtime-growth
      `tests/motorize_class.sh`.

**Risultato gen350:** `make llmscore-probe` = **0/70 wall, 70/70 answered**.
Per classe: arithmetic/causal/creative/deduction/factual/lexical/pragma = 0%.

**Aggiornamento gen351:** LLMSCORE ufficiale successivo = **4/10** nonostante il
probe interno verde. Diagnosi: il piano ha vinto la prima frontiera (muri) ma non
ancora la seconda (completezza, formato, wrong-rate). Da qui la missione diventa
LLMSCORE-max sotto alta varianza: answer planner, format contract, algebra schema,
event frames e oracolo qualità.
