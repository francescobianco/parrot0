

dobbiamo introdurre un meccanismo di imitare gli llm iniziamo con deepseek-v4-flash

l'imitazione non e altro che un profilo da caricare

in questo caso questo kb/profiles/llm/deepseek-v4-flash.p0

dobbiamo introdurre al livello di KB un concetto di pesatura che faccia emergere la risposta dalla kb sulla base di come fa quel llm
in pratica le risposte sono presenti ma il tuning di conoscenza consiste nel memorizzare che quella particolare risposa a punteggio piu alto


il sistema avra una suite di generazione simicle a quella chiamata simchat che in pratica usando opencode va interrogare il modello con una domanda sonda analizza la risposta del modella e configura parrot0 a comportarsi allo stesso modo aggiungendo il pezzo nella kb generale e i pesi di selezione nel profilo

---

# Disàmina (analisi, non implementazione)

> Scritta in coda alla proposta, che resta intatta. Niente codice: solo cosa
> vorrei implementare, vantaggi, svantaggi, e la forma che resterebbe coerente
> coi principi (`PRINCIPLES.md`). Leggere anche [[parrot0-experiment]],
> [[dynamic-knowledge-direction]], [[oracle-is-behavioural-signal]].

## Di cosa si tratta davvero

> **Precisazione dell'autore (vincolante per questa lettura).** L'obiettivo NON è
> copiare conoscenza, né procedure di ragionamento: non è una distillazione per
> carpire abilità cognitive. È **puro stiling comportamentale** — si imita *come*
> un soggetto si comporta e si esprime, non *cosa* sa o *come* pensa. I profili
> futuri potranno essere persone (Magnus Carlsen, Garibaldi), non solo LLM.

Sotto questa precisazione la proposta è un **layer di STILE/PERSONA**: un
LLM-maestro (o, domani, un personaggio) viene sondato per estrarne *tratti di
superficie* — registro, tono, verbosità, formattazione, tic ricorrenti — e un
**profilo di pesi** (`kb/profiles/llm/…`) inclina la *realizzazione* della
risposta di parrot0 verso quei tratti. Conoscenza e ragionamento restano di
parrot0; cambia solo la voce. Due meccanismi distinti vivono nella proposta e
conviene separarli:

1. **Un layer di SCORING/ranking sulla scelta della FORMA della risposta** (il
   "concetto di pesatura"). Oggi la selezione delle *frasi* è in gran parte
   *first-match-wins* + rotazione anti-ripetizione (`response_template`); non
   esiste un meccanismo che ordini per punteggio fra realizzazioni alternative.
   Questo è un mattone **realmente mancante e utile a prescindere dagli LLM**.
2. **Un profilo di STILE estratto da un soggetto esterno** (opencode → deepseek,
   o un corpus di un personaggio). Qui il punto critico non è più "importo sapere"
   ma **tenere il confine fra stile e contenuto**, e non lasciare che la voce
   scavalchi le regole di onestà.

## Cosa vorrei implementare (il nucleo salvabile)

- **Scoring di selezione come dato KB.** Un predicato di peso — p.es.
  `answer_weight(Intent, Candidate, Score)` o un più generale `score/2` — e un
  selettore fisso che, fra i candidati che già unificano, sceglie per punteggio
  invece che per ordine. Si aggancia naturalmente allo scoring già esistente di
  `kb_nearest_concept` (gen155, "first brick of a similarity space"): è la
  prosecuzione della direzione *unificazione estesa / similarità*, tirata dalla
  pressione di un bench dove più risposte plausibili competono e la giusta deve
  vincere. **Questo lo implementerei.**
- **Profilo come PERSONA/POLITICA, non come oracolo di verità.** Un profilo che
  re-pesa *come* parrot0 sceglie e *come* verbalizza (registro, verbosità,
  stile) — coerente coi profili già presenti (`kb/profiles/agi.p0`) e con
  `mod_role`/impersonate. Un profilo cambia il COMPORTAMENTO di selezione, non
  ciò che è vero.
- **Il maestro come ORACOLO DI SCOPERTA, non come fonte di contenuto.** Usare la
  suite tipo `chatsim`/`simchat` esattamente come già fa il discovery-harness:
  per misurare *dove* parrot0 diverge e *quale capacità* costruire dopo
  ([[oracle-is-behavioural-signal]]: l'oracolo misura COME reagisce il modello,
  non la correttezza). Poi quella capacità si costruisce in C/KB per struttura —
  **senza mai cachare la risposta letterale del maestro come risposta di
  parrot0.**

## Temperatura e sonde minime

Due raffinamenti che rafforzano la lettura "puro stile":

- **Il profilo porta una temperatura.** Non la temperature di sampling di un LLM
  in senso letterale, ma una *manopola di variabilità/azzardo* sulla selezione:
  quanto il selettore pesato esplora le forme alternative invece di scegliere
  sempre la moda. Un profilo "Carlsen" tende a bassa (deciso, conciso,
  prevedibile); un profilo "creativo/giocoso" ad alta (più dispersione, più
  guizzi). Si innesta in modo naturale sul layer di scoring: i pesi diventano una
  **distribuzione** e la temperatura ne regola la nitidezza (softmax-like, t→0
  argmax, t alto quasi-uniforme), riusando anche l'istinto anti-ripetizione di
  gen55. È un parametro di stile, non di verità.

- **Le sonde sono minime e criptiche — ed è un pregio.** Prompt-sonda brevi e
  semplici (una singola lettera, un numero, una parola strana) sono **esattamente**
  la filosofia del `sym-bench` già in casa ([[symbolic-discovery-harness]],
  [[oracle-is-behavioural-signal]]): stimoli aperti che misurano *come* un
  soggetto reagisce, non se "risolve". Sono perfetti per catalogare lo STILE:
  davanti a `?`, a `7`, a `qwerty`, un soggetto chiede chiarimenti, un altro
  scherza, un altro rifiuta secco — puro comportamento, **quasi nessun contenuto
  da rubare**. Vantaggio doppio: (a) minimizzano per costruzione il rischio di
  catturare conoscenza o ragionamento — non c'è quasi nulla da *sapere* in "z" —
  e così tengono pulito il confine stile/contenuto (il punto 1 degli svantaggi
  quasi si dissolve); (b) sono economici e riusabili. La suite tipo `simchat`
  manderebbe questi stimoli, classificherebbe la reazione in *tratti*
  (chiede/scherza/rifiuta/elenca · conciso/prolisso · formale/informale) e
  scriverebbe quei tratti come pesi nel profilo.

## Vantaggi

- **Colma un buco architetturale reale**: la mancanza di un ranking esplicito
  delle risposte. È utile in sé, indipendente dal "mimare un LLM".
- **Target misurabile e netto**: imitare un modello *specifico* dà al discovery
  harness un oracolo concreto ("HOW it reacts"), più nitido di un vago "sii
  intelligente".
- **Compatibile col meccanismo dei profili** già esistente: caricare un profilo
  è un gesto che il sistema conosce.
- **Runtime puro preservabile**: se la distillazione cattura tutto in file
  statici, a runtime parrot0 resta senza rete — come la via wiki-learning
  ([[dynamic-knowledge-direction]]).
- **La persona trasparente è onesta**: parrot0 che adotta uno stile dichiarando
  di essere parrot0 sotto la maschera è già il pattern di `mod_role` (il
  truth-probe perfora il travestimento).

## Svantaggi e cautele (sotto la lettura "puro stiling comportamentale")

Con la precisazione — si copia STILE e COMPORTAMENTO, non conoscenza né procedure
di ragionamento — **quasi tutte le obiezioni da knowledge-distillation cadono**:
non si tocca la scommessa fondativa (il ragionamento resta di parrot0), non si
ereditano i *fatti* o gli errori cognitivi del maestro, non c'è un problema di
provenienza del sapere. Restano cautele più lievi e circoscritte, tutte sul
versante della *forma*:

1. **Il confine stile/contenuto è la vera linea da presidiare.** Sondando un
   soggetto è facile catturare per sbaglio anche il *contenuto* o lo *schema di
   ragionamento*, non solo la voce. La disciplina dev'essere esplicita: si
   estraggono solo *feature di superficie* (lunghezza, registro, formalità,
   hedging, formattazione, marcatori di persona), mai la proposizione né il passo
   inferenziale. È un confine sottile da mantenere quando l'estrazione è
   automatica — il punto su cui la suite va progettata con più cura.
2. **Lo stile non deve scavalcare onestà e calibrazione.** Una persona "sicura e
   tagliente" (es. Carlsen) applicata sopra può far *suonare* parrot0 più
   autorevole di quanto la sua base giustifichi: la voce cambia, **la verità e la
   calibrazione no**. Il layer di stile resta subordinato alle regole
   anti-impostore — parrot0 in persona dice comunque "non lo so", e il truth-probe
   di `mod_role` continua a perforare la maschera ("davvero, chi sei?" → parrot0).
   Una persona storica (Garibaldi) aggiunge un'avvertenza: il *registro* d'epoca
   è stile lecito, ma non deve diventare un alibi per asserire come vere le *idee*
   o i *fatti* di quel personaggio — quello sarebbe scivolare nel contenuto.
3. **Stile come PARAMETRI, non come phrasebook di persona.** La forma principled è
   un set di *parametri* (verbosità, formalità, hedging on/off, emoji, lunghezza
   frase, tic ricorrenti) applicato da uno *styler* sul contenuto già prodotto da
   parrot0 — non una lookup delle frasi letterali del personaggio. Se degenera nel
   memorizzare le battute del maestro indicizzate per sonda, è di nuovo il
   phrasebook che i principi rifiutano: ridiventerebbe surrogato di contenuto
   mascherato da stile.
4. **Outsourcing residuo, ma di stile e solo in tuning.** Interrogare deepseek
   resta un outsourcing; però di una cosa **molto meno load-bearing** (il tono,
   non il pensiero), e la cattura *statica* lo confina fuori dal runtime (parrot0
   resta senza rete, come la via wiki-learning, [[dynamic-knowledge-direction]]).
   Tensione minore col principio "niente intelligenza esternalizzata", non più la
   tensione di fondo.
5. **Misurare la fedeltà stilistica è fuzzy.** "Comportarsi allo stesso modo"
   richiede un giudice di *match di stile* (registro, lunghezza, tono), non di
   correttezza: la suite tipo `chatsim` darebbe un segnale utile ma soft. Nota
   metodologica, non bloccante.

## Raccomandazione

Con la precisazione "puro stile", la proposta è **legittima e desiderabile**;
basta tenerne tre pezzi riusabili e un solo paletto:

- **SÌ** al layer di scoring/ranking sulle FORME (`score`/`answer_weight`) come
  **distribuzione pesata** con la **temperatura** a regolarne la nitidezza — utile
  a prescindere dagli LLM, prosecuzione coerente della similarità (`kb_nearest_concept`).
- **SÌ** ai **profili di stile/persona** (LLM oggi; persone domani — Carlsen,
  Garibaldi) come **set di parametri** applicati da uno styler sul contenuto di
  parrot0, trasparenti: il truth-probe di `mod_role` perfora sempre la maschera.
- **SÌ** alle **sonde minime/criptiche** in stile `sym-bench` come metodo per
  catalogare il comportamento — riusano il discovery-harness già in casa e tengono
  il contenuto fuori dall'inquadratura.
- **Unico paletto fermo: il confine stile/contenuto.** Si estraggono *tratti di
  superficie*, non frasi o risposte; cattura **statica** (runtime senza rete); e
  lo stile **non scavalca onestà e calibrazione** — parrot0 in persona dice
  comunque "non lo so", e una voce sicura non rende vera la sostanza.

In una riga: **con la precisazione, mimic-llm non è il sentiero dell'impostore ma
un legittimo layer di STILE** — pesatura+temperatura sulla forma, profili-persona
come parametri, sonde minime per catalogare il comportamento. L'unica disciplina
è non lasciare che lo "stile" diventi un cavallo di Troia per il contenuto o per
una voce più sicura della sostanza.

---

## Stato implementazione — primo giro (gen226)

Spedito il **lato parrot0**, offline (nessuna rete/opencode), puro stile:

- **Temperatura sulla forma operativa.** `kb_response` (in `src/brain/00-lex.c`)
  onora un fatto `style_temperature(N)` portato da un profilo: `0` → argmax
  (sempre la forma canonica, persona decisa/concisa); assente o `!=0` → rotazione
  anti-ripetizione gen55. Influenza solo *come* si dice, mai *cosa*.
- **Primo profilo di stile**: `kb/profiles/llm/deepseek-v4-flash.p0` —
  `style_temperature(0)` + `style_trait/2` (tratti di superficie, dichiarati
  stile, non conoscenza). Caricabile con `PARROT0_PROFILE=…`.
- **Harness `make mimic-bench`** (`tests/mimic.sh`): (A) cataloga la reazione a
  sonde minime/criptiche (lettera, numero, parola strana, punteggiatura) in stile
  sym-bench; (B) mostra la temperatura che rende deterministica la scelta della
  forma. Non fa gate (strumento di scoperta). `make test` resta verde.

Da fare (giri successivi, dentro il paletto stile/contenuto): styler a
*parametri* su verbosità/registro/hedging; suite simchat che SCRIVE i pesi del
profilo sondando un soggetto (cattura statica, dichiarazione di provenienza);
profili-persona (Carlsen, Garibaldi).
