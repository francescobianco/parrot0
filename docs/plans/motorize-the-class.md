# Motorize the Class — strategia per massimizzare LLMSCORE

> **Principio unico:** *motorizza la classe, poi nutri il motore su scala — non
> memorizzare istanze.* Ogni classe che parrot0 vince oggi è un MOTORE su una KB
> ampia (rime → `wordquery` + 35k `lexeme`; aritmetica → `wordproblem`). Ogni
> classe che perde è un LOOKUP su una tabella sparsa (`explanation`=4,
> `because`=25, `qa_reply`=6). Il fix è convertire ogni classe fragile da
> *tabella* a *motore + ingestione*, non aggiungere righe alla tabella.

Coerente con lo steer di F.: **meno moduli, più KB**. Un motore causale è UN
motore per l'intera classe "perché" — l'opposto di N moduli-qa. I motori sono
C (motori, non frasi); la conoscenza cresce nella KB per processo.

## Mantra operativi — da eseguire PRIMA di ogni passo

Regole imposte a noi stessi per non ricadere nel fix puntuale. Prima di scrivere
*qualsiasi* riga, si passa questa checklist. Nate dagli errori reali di gen348-349.

1. **"È generalizzabile KB-first?"** — la domanda zero. Non fixare l'istanza,
   motorizza la CLASSE. Se la risposta è "sì ma è più lavoro", si fa il lavoro.
2. **Niente liste di parole nel C.** Trigger, cue, sinonimi, unità, verbi: sono
   fatti KB enumerabili (`causal_process_verb/1`, `verb_syn/2`, `time_unit/1`).
   Test: *"parrot0 può impararne un nuovo membro domani senza ricompilare?"*
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

## Perché i fix puntuali perdono

Il giudice campiona 10 domande da una distribuzione illimitata. I run osservati
condividono ~0 domande. Una coppia `qa_cue/qa_reply` copre misura-zero →
valore atteso ~0 sul run successivo. Ottimizziamo il **punteggio atteso sulla
distribuzione**, non il prossimo campione di 10.

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

## Criteri di misura (definizione di "fatto")

- **Wall-rate per classe** ↓ (metro primario Fase 0, senza API).
- **Copertura per classe %** (attempt plausibile) tracciata run-su-run.
- LLMSCORE ufficiale come controllo esterno, NON come metro di sviluppo
  (non-deterministico, campione piccolo).

## Ordine di esecuzione raccomandato

Fase 0 + Fase 1 insieme (senza la batteria non sappiamo se Fase 1 muove la
classe o l'istanza). Fasi 3-4 come quick-win in parallelo. Fase 2 e 5 dopo.

## Bug noti (rimandati) — da riprendere

Trovati durante gen349 e consapevolmente NON risolti ora. Ognuno viola almeno un
mantra; annotati per riprenderli senza riscoprirli.

- **B1 — il decomposer spezza unità semantiche su " and ".** "Romeo and Juliet"
  → "romeo" + "juliet"; e i word-problem multi-frase vengono divisi. Aggravante:
  `mod_wordproblem` legge il turno intero da `input`, non il sub-clause → rispose
  due volte ("15 dollars. 15 dollars."); tamponato con un dedup, ma la radice è
  che il decomposer divide ciò che è UNA unità. Serve una guardia: non decomporre
  quando il turno è un problema/entità singola (numeri+cue matematico, o titolo).
- **B2 — `answer_frame`/`created_by` falliscono sui titoli multi-parola sotto
  "the".** "Mona Lisa", "Starry Night" murano (Guernica, David — parola singola —
  funzionano). Un blocco precedente in `mod_knowledge` intercetta "the <X multi>";
  inoltre il motore `created_by` è piazzato tardi. Serve: estrazione del titolo
  multi-parola + anticipare il motore.
- **B3 — `kb_match` arity-1 completamente LEGATO non riporta l'hit.** Una query
  `time_unit(hour)` con arg legato torna 0 anche su match; ho dovuto ENUMERARE e
  confrontare. Quirk del motore KB: verificare se è un bug generale di `kb_match`.
- **B4 — "make a word from the letters r,e,a,d" va a `mod_compose`** invece di
  `mod_wordquery` (precedenza di routing: "make a word" → code synth). Non è un
  muro (risposta non-muro), ma è una non-risposta.
- **B5 — `cue()` è substring, ovunque.** Fixato eat/feathers, ma il pattern può
  ripresentarsi su altri cue discriminanti. Valutare un `cue_word()` a parola
  intera come default per i cue discriminanti.
- **B6 — sillogismo affermativo con pronome mangia il testo.** "Every square is a
  rectangle. This shape is a square. Is it a rectangle?" → "Yes -- Is is a
  Rectangle" (giusto ma sgrammaticato: "it" diventa "Is").
- **B7 — le parole-cue della dieta (eat/eats/diet/food) sono ancora una lista in
  C** (le ho rese whole-word ma restano cablate) → viola mantra #2, portarle in KB.
- **B8 — il wall-rate non conta le risposte sbagliate** (vedi Fase 0). Serve un
  secondo oracolo per-classe che marchi gli errori.
- **B9 — estrazione CREATION solo attiva.** "The Odyssey was written by Homer"
  (passivo) non viene estratto (declina, non sbaglia). Aggiungere il frame
  passivo "O was <verb>ed by S" → `created_by(S, O, verb)`.
- **B10 — messaggio "Learned: mammal.(platypus)"** ha un punto spurio nel nome
  predicato (cosmetico, pre-esistente all'estrazione copula).

## Stato

- [x] Fase 0: batteria sonde + `make llmscore-probe` + baseline wall-rate.
      **Baseline (2026-07-22): 31% wall overall (48/70 risposte).**
      Per classe: deduction 50%, causal 40%, factual 40%, arithmetic 30%,
      creative/lexical/pragma 20%. Conferma le priorità.
- [ ] Fase 1: motore causale + ingestione cause→effetto.
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
- [~] Fase 2: ingestione enciclopedica mirata. **AVVIATA (gen349):** estrazione
      CREATION transitiva KB-first in `extract_class_statement` — la prosa
      "S <creation-verb> O" → `created_by(S,O,Verb)`, verbi da `creation_verb/1`.
      Testato end-to-end: "Claude Monet painted Water Lilies" → fatto estratto →
      "Who painted Water Lilies?" → "Claude monet". La base factual cresce per
      PROCESSO, non a mano. Prossimo: passivo (B9), altre relazioni (borders,
      composed…) via una tabella generica `extract_frame(Verb, Pred)`, poi far
      girare l'estrazione sulle `kb/learning/pages/`.
- [ ] Fase 3: quadrato opposizioni completo
- [ ] Fase 4: layer framing/formato
- [ ] Fase 5: fallback anti-muro
