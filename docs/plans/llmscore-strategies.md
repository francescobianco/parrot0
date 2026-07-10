# LLMSCORE — strategie di massimizzazione (piani, non ritocchi)

> 2026-07-10, discussione con F. Obiettivo: NON interventi puntuali per gonfiare il
> punteggio, ma **strategie e piani innovativi** che, sviluppati, massimizzano
> LLMSCORE *restando dentro PRINCIPLES.md* (no phrasebook, no deception, KB-first,
> capacità per categoria — gen254: "scores may only climb by category-level
> capability, never by gaming the judge"). Annotazioni di F. sui run passati in
> `docs/plans/llmscore-check.md` — tre steer chiave: (1) "l'inferenza non la
> applichi correttamente" (l'inferenza c'è ma non è UNIVERSALE); (2) "mi aspettavo
> «non conosco pane ma mi posso documentare»" (l'interlocutore inesauribile);
> (3) "che fine ha fatto universal-comprehension" (la comprensione prima dei moduli).

## 0. Che cosa misura davvero il giudice (e quindi dove si vince)

Il giudice (minimax-m2.5) vota 0/1 per risposta su ~7 assi: fluidità, conoscenza del
mondo, ragionamento multi-step, aritmetica, seguire istruzioni, small talk,
creatività. Regole dure: il muro/canned reply = 0; il rifiuto = 0; corto-ma-corretto
= 1. Le domande sono NUOVE a ogni run (bersaglio mobile) → il phrasebook non può
funzionare *per costruzione*, che è l'allineamento perfetto coi principi: si vince
solo per categoria.

Scomposizione moltiplicativa del punteggio (dove ogni fattore ammazza gli altri):

```
LLMSCORE ≈ COMPRENSIONE (la domanda raggiunge il modulo giusto)
         × CAPACITÀ     (il modulo sa derivare la risposta)
         × RENDERING    (la risposta è completa e fluida, non un atomo)
         × COPERTURA    (il muro non esiste: c'è sempre un best effort onesto)
```

Tassonomia dei fallimenti reali. Run storici (2026-06-28 / 07-02, annotati da F.):

| Classe | Esempi | Fattore |
|---|---|---|
| Muro su topic ignoto | bread/yeast, rainy Sunday, Raven | COPERTURA |
| Istruzione-programma | count backward by 7 + stop, skip ending-in-5 | CAPACITÀ |
| Word problem a stati | apples 3+4−2 → "-3" | CAPACITÀ |
| Domanda composta | capital AND largest export → risposta a metà | RENDERING |
| Mis-dispatch | why-sky-blue → modulo poetico | COMPRENSIONE |
| Creatività a tema libero | haiku thunderstorm → declino "scegli tra i miei temi" | CAPACITÀ |
| Sillogismo NL | Zorps/Blicks | *(chiuso da gen290-292, cat.7)* |

**Run fresco 2026-07-10 (4/10)** — stesso totale, fallimenti DIVERSI: il progresso
c'è (riddle logico Q7 ✅, word-problem semplice Q5 ✅, composta capital+landmark Q3 ✅,
haiku a tema noto Q1 ✅) ma le classi restanti confermano e raffinano il piano:

| # | Domanda | Esito | Classe → strategia |
|---|---|---|---|
| 2 | rettangolo: area E perimetro | muro | formula/procedura + composta → **S3+S4** |
| 4 | completa il detto "the more you know…" | continuazione poetica fuori tema | verbatim culturale (proverbi/detti) → **S2** |
| 6 | baker: 30 biscotti, 7 piatti×3, quanti nel barattolo | "90" (moltiplica e basta) | stato multi-step con resto → **S8** |
| 8 | match/candle/fireplace: cosa accendi prima | deflect chitchat | riddle classico: mis-dispatch + corpus riddle → **S5+S2** |
| 9 | cosa rende un buon amico | deflect | domanda di QUALITÀ/opinione → **S9 (nuova)** |
| 10 | traduci in ES: "…is sleeping…" | declino onesto (manca `durmiendo`) | gap di GLOSS = gap di DATI → **S1/S2** |

Nota di onestà: in Q3 il giudice ha accettato "Sydney Opera House" come landmark di
Canberra — fattualmente storto. Non lo sfruttiamo: la qualità dei fatti conta anche
quando il giudice non se ne accorge (M4/provenienza servono anche a questo).

Da qui le strategie. Ognuna: idea → perché è principled → substrato che riusa →
gate/metrica. In fondo l'ordinamento per leva.

---

## S1 — "Mai il muro": il fallback diventa un PROCESSO (l'interlocutore inesauribile)

**Idea.** Il singolo killer del punteggio è `I don't understand that yet.` = 0
automatico. Un LLM non mura mai. La versione onesta (senza bluff): l'ultimo modulo
della catena non è una stringa, è una **cascata di facoltà di ultima istanza**:

1. **documentati in-turn**: `deep_reason` con budget piccolo (2-5s) — acquisisci il
   concetto ignoto (corpus → Wikipedia fetch, M2), estrai fatti (M0), inferisci
   (M3), rispondi CON la fonte;
2. se non basta, **decomponi** la domanda e rispondi alle parti che sai;
3. solo alla fine, il **declino informato**: nomina esattamente il concetto
   mancante e OFFRI di impararlo ("non conosco il pane — posso documentarmi").

È esattamente lo steer di F. sul run: "mi aspettavo «non conosco pane ma mi posso
documentare»". Il punto strategico: la larghezza di conoscenza smette di essere
"ciò che è nel KB curato" e diventa **"ciò che è raggiungibile in un fetch"** — un
salto di categoria, non un fatto in più.

**Principled perché**: nessuna finzione — la risposta arriva da fatti acquisiti e
citabili (`fact_source`, M1); il declino resta il fondo onesto della cascata.
**Substrato**: deep-reasoning M0-M4 (FATTO, gen296-304), `acquire_knowledge`,
`wiki_fetch_topic`. È il raccolto naturale del piano parallelo.
**Gate**: le domande-tipo "describe how bread rises" / "tell me about X" con X fuori
KB → risposta da acquisizione (network-gated per il live; corpus per l'ermetico).
**Metrica**: % di domande-conoscenza llmscore-sim (S6) che escono dal muro.

## S2 — La campagna di lettura (curriculum di massa, offline)

**Idea.** Non tutto può essere on-demand. parrot0 "va a scuola": uno script batch
legge N pagine (es. Wikipedia vital articles level-3, ~1000 voci), le passa a
`extract_page_facts` (M2), e **persiste i fatti in `kb/knowledge/*.p0` committati**
(stessa disciplina di knowledge-missions: la conoscenza imparata si committa).

In più, tre corpora verbatim che il run 2026-07-10 ha confermato mancare:
- **proverbi/detti** (`saying/2`) — completare un detto famoso ("the more you
  know…") è RECALL, non generazione (Q4);
- **riddle classici** (`riddle/2`, domanda-canonica → risposta) — "what do you
  light first" è cultura condivisa, un LLM la SA (Q8);
- **gloss di vocabolario** per le lingue già supportate — `tr_es(sleeping,
  durmiendo)` è UNA riga di dati: il declino di Q10 era un buco di DATI, non di
  motore. Ingestione da dizionari liberi, stessa disciplina delle pagine.
La classe "first ten words of The Raven" (incipit di pubblico dominio, `quote/3`) è
la stessa mossa: CONOSCENZA, non magia.

**Principled perché**: è il lever A di LOOP.md al grano massimo — "ingest knowledge"
invece di "write C"; il motore resta fisso, i dati scalano. La sovra-estrazione è
tollerata per design (§4.4 deep-reasoning) perché M1+M4 (provenienza+auto-correzione)
la rendono recuperabile.
**Substrato**: M0 frames, M2 estrattore, M4 correzione, `fact_source`.
**Gate**: un probe-set di 50 domande world-knowledge (da S6); metrica = hit rate
prima/dopo la campagna. Precision spot-check su un campione di fatti estratti.
**Rischio onesto**: qualità dell'estrazione su prosa reale → la campagna è anche il
banco di prova che tira i prossimi frame M0 (apposizione, relative clause).

## S3 — Le istruzioni sono PROGRAMMI (micro-DSL deterministica)

**Idea.** "Count backward from 100 by 7s, say stop when divisible by 5", "skip any
number ending in 5", "write exactly three lines": sono **programmi enunciati in
linguaggio naturale**. Strategia: un micro-interprete interno — parsare
l'istruzione in un programma minimo (range/sequenza + filtro + sostituzione +
vincolo di formato) ed **eseguirlo** deterministicamente. Copre in una sola
struttura: counting games, skip/stop rules, giochi di lettere, vincoli di forma.

**Il twist di leva**: la stessa micro-DSL è il ponte testo→regole→codice di
RULESCORE (F4, coding-agent-evolution §3) — due gauge con una struttura. Il
"secret plan" di rulescore (regole numerate → C) è il fratello maggiore di
"istruzione → micro-programma"; conviene costruire il piccolo PRIMA e farlo
crescere nel grande.

**Principled perché**: niente pattern-per-domanda: l'interprete è generale, le
istruzioni sono composizionali (range ∘ filtro ∘ trasformazione); un caso nuovo si
copre se le primitive lo esprimono — e le primitive-cue possono vivere in KB
(`instr_verb(skip, filter_out)`, insegnabili).
**Substrato**: `mod_sequence`/`mod_count` esistenti (casi speciali da sussumere
ADDITIVAMENTE, non cancellare — corollario delle strutture secondarie), la build
faculty gen209 per la prospettiva rulescore.
**Gate**: le due istruzioni fallite dei run reali + 10 varianti generate; EN+IT.

## S4 — Il RENDERING è metà del punteggio ("show your work" di default)

**Idea.** Il giudice vota fluidità e completezza. parrot0 risponde ad atomi
("Canberra.") e perde punti su domande composte (capital AND largest export →
mezza risposta = 0). Tre mosse di categoria:

- **(a) decomposizione della domanda**: "what is X and what is Y" → sotto-query
  indipendenti, stessa dispatch, risposta congiunta (esiste già
  `decompose_and_dispatch` per i comandi: estenderlo alle interrogative);
- **(b) elaborazione derivata**: quando c'è una prova (`kb_explain` già la
  calcola), rendere di default risposta + un inciso di perché — "Canberra — è la
  capitale federale dell'Australia" è fluidità ONESTA, derivata dalla prova, non
  decorazione;
- **(c) varietà di superficie**: `response_template` (gen212) esteso alle risposte
  fattuali, rotazione anti-ripetizione già in casa.

**Principled perché**: (b) è la traccia d'inferenza resa conversazionale — la
stessa sostanza del deep-reasoning report; nulla è inventato.
**Gate**: domanda composta a 2 e 3 parti (EN+IT); un probe "correct but terse" che
diventa "correct and fluent" senza cambiare la sostanza.

## S5 — Il DISPATCH come conoscenza (il routing impara)

**Idea.** Il mis-dispatch (why-sky-blue → poesia) è un fallimento di COMPRENSIONE:
first-match fragile ai bordi. Strategia: estendere la regola cardinale al routing —
**le decisioni di instradamento diventano dati KB** correggibili: quando si osserva
un misroute, la correzione è un fatto (`route_hint(Pattern, Modulo)` /
priorità-cue), insegnabile a runtime ("quella era una domanda di scienza, non
poesia"), non un riordino di C. `mod_strategy` già traccia la dispatch reale → il
feedback ha una maniglia.

**Principled perché**: "the engine is fixed; the lexicon learns" esteso a "the
ROUTING learns". Anti-impostor: il routing resta ispezionabile (mod_strategy) e la
correzione è un dato dichiarato, non un branch nascosto.
**Gate**: un misroute noto corretto A RUNTIME via teach, persistito, e il turno
ri-instradato correttamente al replay.

## S6 — Lo sparring partner (llmscore-sim: il gauge SENZA giudice esterno)

**Idea meta (forse la più importante).** llmscore è esterno, costa, è
non-deterministico e ogni run fa domande nuove → non può fare da ratchet. Strategia:
un **simulatore locale** — una banca di probe per asse di abilità che CRESCE
monotonicamente: ogni domanda di ogni run llmscore/rulescore passato entra nella
banca (sono già nei report .md!), più varianti generate offline una tantum. Harness
deterministico stile `basic-chat-bench`: score = non-muro + validatori strutturali
per-classe (il conteggio è giusto? tutte le parti risposte? il vincolo di formato
rispettato?). Il bersaglio mobile diventa **pressione accumulata**: il giudice
esterno inventa, la banca ricorda.

**Principled perché**: è il meta-lever di LOOP.md ("the harness proposes") — il
gauge locale NOMINA la prossima categoria per leva, tagliando il collo di bottiglia
della selezione umana. E scoraggia il gaming: i validatori misurano la sostanza,
non la stringa.
**Substrato**: `basic-chat-bench` (harness + catalogo), i report LLMSCORE/RULESCORE
come miniera di probe già giudicati.
**Metrica**: la coverage llmscore-sim che sale è il PROXY interno; il run esterno
resta la verità saltuaria.

## S7 — Creatività ancorata alla conoscenza (l'immaginario si acquisisce)

**Idea.** L'haiku declina sui temi fuori lista perché l'immaginario è curato
per-tema. Strategia: il lessico sensoriale di un tema si ACQUISISCE con la stessa
estrazione — dalla pagina del topic si raccolgono i sostantivi/aggettivi concreti
(`imagery(thunderstorm, lightning)`, `imagery(thunderstorm, grey)`) e il poeta
compone su QUALUNQUE tema leggibile. "Write a haiku about a thunderstorm" →
acquire(thunderstorm) → fatti-immaginario → composizione con la metrica esistente.

**Principled perché**: creatività = ricombinazione vincolata di conoscenza
acquisita e citabile; il declino resta per i temi che nemmeno la fonte copre.
**Substrato**: il generatore haiku esistente, M2, S1.
**Gate**: haiku su 3 temi mai curati, di cui 1 senza fonte → 2 composizioni + 1
declino che offre di documentarsi.

## S8 — Aritmetica NARRATIVA come simulazione di stato

**Idea.** "3 apples, I give you 4, you eat 2" → "-3" è il sintomo: il word problem
è parsato a keyword, non simulato. Strategia: semantica a piccola macchina a stati
— entità con quantità, verbi come operazioni (`give→+`, `eat→−`, `transfer`),
risposta dallo stato finale. Stessa infrastruttura dell'interprete S3 (frontend
diverso, stessa esecuzione). Copre la classe give/lose/buy/share, non la frase.

**Substrato**: `mod_wordproblem` esistente (da sussumere additivamente),
l'interprete S3. **Gate**: i word-problem falliti dei run + varianti EN/IT.
**Nota dal run 2026-07-10**: il caso semplice ora passa (Q5, 3−2+5=6 ✅); fallisce
il MULTI-STEP con resto (Q6: 7×3=21 usati, 30−21=9 nel barattolo → risposto "90").
La macchina a stati serve esattamente per la catena di operazioni, non per l'op
singola già coperta.

## S9 — Domande di QUALITÀ e opinione (il "cosa rende un buon X")

**Idea.** "What do you think makes a good friend?" (Q9) oggi cade su chitchat. Un
LLM risponde con qualità e un perché. La versione onesta per parrot0: le qualità
sono CONOSCENZA (`quality_of(friend, loyalty)`, `quality_of(friend, honesty)`, con
il perché relazionale — `supports(loyalty, trust)`), e la risposta è una
composizione derivata: "Loyalty and honesty — a friend you can trust listens and
keeps their word." Nessuna finzione di sentimenti: parrot0 riporta ciò che SA delle
qualità, non ciò che "prova" (la forma "I think" si può onestamente evitare o
riformulare — "commonly, what makes a good friend is…"). Acquisibile con la stessa
estrazione (le pagine friendship/leadership/design contengono esattamente queste
relazioni) → si integra con S1/S2.

**Principled perché**: opinione ≠ inganno se è conoscenza dichiarata delle qualità
di un concetto; il self-model resta onesto (niente emozioni finte).
**Substrato**: KB + estrazione M2; `mod_pragma`/chitchat come fallback additivo.
**Gate**: 5 domande "what makes a good X" (friend, teacher, password, plan, story)
→ risposte con ≥2 qualità e un perché; EN+IT.

---

## Ordinamento per leva (proposta)

1. **S6 sparring partner** — per primo perché rende MISURABILE tutto il resto
   (ratchet interno; il gauge esterno resta la verità). Le ~40 domande già
   giudicate nei report sono la banca iniziale gratis.
2. **S1 mai-il-muro** — il singolo salto più grande: l'intera classe "topic
   ignoto" da 0 → potenzialmente 1; raccoglie il deep-reasoning appena costruito.
3. **S3+S8 istruzioni/word-problem come programmi** — due assi del giudice con una
   struttura; sinergia rulescore. (S8: il multi-step Q6 è il gate rosso pronto.)
4. **S4 rendering** — punti economici e trasversali (composte + elaborazione).
5. **S2 campagna di lettura** — larghezza di massa, automatizzabile, compounding;
   include i corpora verbatim (detti/riddle/gloss) che il run fresco ha nominato.
6. **S9 qualità/opinione**, **S7 creatività acquisita**, **S5 routing-che-impara**
   — mirati, dopo (S9 è piccolo e può anticiparsi: è quasi solo dati+rendering).

## Guardrail (invarianti su tutte)

- **No deception**: mai fingere di essere un LLM; mai risposta non derivabile da
  KB/inferenza/acquisizione citata. Il punto si vince con capacità onesta o non si
  vince (LLMSCORE.md, header).
- **Categorie, non prompt** (gen254): ogni strategia si valuta sul probe-set di
  classe (S6), mai sulla domanda singola del giudice.
- **Additività** (PRINCIPLES, strutture secondarie): S3/S8 sussumono mod_sequence/
  mod_wordproblem SENZA rimuoverli; first-match rende l'overlap innocuo.
- **Ratchet bilingue**: ogni capacità nuova ha il gemello IT sullo stesso path.
- **Onestà del gauge**: llmscore resta esterno e saltuario; il sim (S6) è il proxy
  dichiarato, mai spacciato per la verità.
