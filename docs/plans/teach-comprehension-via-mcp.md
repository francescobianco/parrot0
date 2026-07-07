# Insegnare la comprensione a parrot0 via MCP (kb-first) — quadro e piano

> **Stato:** scritto a gen278 (2026-07-07), con misura empirica dal vivo via
> `parrot0 --mcp-engine`. Ogni claim di "distanza" ha un output allegato, non è
> stimato. Companion di [[mcp-engine.md]] (il canale) e [[generative-prolog.md]]
> (il motore); estende la disciplina [[kb-first-manifesto]] al caso più ambizioso:
> non travasare *fatti di dominio*, ma insegnare a parrot0 la sua stessa
> **macchina di comprensione**.
> **Ruolo:** dare una collocazione onesta a un'intuizione di F. e misurare quanto
> è già vera, quanto è a un adattatore di distanza, e quanto è muro di principio.

## 0. Il concetto (F., 2026-07-07)

> parrot0 dovrebbe imparare **forme grammaticali, termini ed espressioni via
> MCP**. Grazie al principio kb-first dovrebbe essere possibile dargli strumenti
> di comprensione grammaticale, modi di interpretare i testi, significati dei
> termini, strutture di risposta e modelli di interpretazione del codice, dei
> testi, dei flussi codificati — ma anche conoscenza generale che si riconduce
> ai processi elaborativi. Tutto via MCP e sempre seguendo il principio KB-first.

Riformulato come tesi verificabile: **la macchina di comprensione di parrot0 è
(o dovrebbe essere) dato KB, non codice — perciò insegnabile a runtime via i tool
MCP `kb.*`, senza ricompilare.** Questo piano misura quanto di quella tesi è già
realtà e traccia la strada per il resto.

## 1. La buona notizia: gran parte È GIÀ dato KB (inventario con riferimenti)

Il progetto ha già migrato verso il KB, generazione dopo generazione, proprio le
sette categorie che F. elenca. Non è una direzione nuova: è quella in corso. Ogni
riga qui è **insegnabile via MCP oggi** (con l'asterisco del §2):

| Categoria di F. | Predicato KB | File | Origine | # fatti base |
|---|---|---|---|---|
| Forme grammaticali (classe chiusa) | `stopword/1`, `conjunction/1` | `kb/core/lexicon.p0` | gen193 | ~357 |
| Significati dei termini (traduzione/glossa) | `tr/2`, `gender/2` | `kb/core/gloss.p0` | gen126 | ~39 |
| Espressioni / idiomi (multi-parola) | `intent_phrase/2`, `intent_cue/2` | `kb/core/intents.p0` | gen211 | ~438 |
| Modi di interpretare i testi (trigger di routing) | `intent_cue/2` per chain-site | `src/brain/*.cues.p0` | Track 5, gen271+ | crescente |
| Strutture di risposta | `response_template/2` | `kb/core/responses.p0` | gen211 (lato output) | ~63 |
| Marcatori sociali / pragmatica | `social_marker/2` | `kb/core/social.p0` | gen225 | ~125 |
| Modelli d'interpretazione del codice | `algo_shape/2`, `codebase_lookup/2`, `lookup_call/2` | `kb/experts/programming/*`, `kb/experts/codebase/actions.p0` | Track 5 | crescente |
| Conoscenza generale (legata ai processi) | `world-facts` + self-model `module/1`, `i_am/1` | `kb/core/world-facts.p0`, `40-meta`/`99-registry` | gen195+ | ~1175 |

**Misura viva (contenuti minuscoli — funziona oggi):**
```jsonc
kb.assert {"pred":"conjunction","args":["plus"]}   → {"ok":true,"stored":true}
kb.query  {"pred":"conjunction","args":["plus"]}    → {"provable":true}     // forma grammaticale nuova, viva
kb.assert {"pred":"tr","args":["tree","albero"]}    → {"ok":true,"stored":true}
kb.query  {"pred":"tr","args":["tree","albero"]}     → {"provable":true}     // significato nuovo, vivo
```
Il principio kb-first è già il *modo* con cui parrot0 comprende: la legge
"engine fixed, knowledge learns" (`PRINCIPLES.md`, [[kb-first-phrases]]) rende
ogni classe di superficie una tabella di fatti che il kernel C interroga
(`kb_intent_match`, `kb_cue_match`, `kb_response`, `stopword`/`conjunction`
lookup). L'intuizione di F. **è la spina dorsale del progetto**, non una deviazione.

## 2. La distanza reale (misurata, tre secchi)

Dove la tesi NON è ancora vera. Onestamente separati per *natura* del gap.

> **Stato gen280:** il **Secchio A.1** (letterali corrotti in variabili) è
> **RISOLTO** — U1 (gen279, quotatura al bordo) + U1b (gen280, `$` esplicito):
> nomi propri, `$`-valori e template ora round-trippano (`tests/mcp-teach.sh`).
> Resta **A.2** (regole n-arie via MCP → U2, prossimo) e i Secchi **B**/**C**. La
> dimostrazione originale di A.1 qui sotto è tenuta come record del *perché*.

### Secchio A — l'API di scrittura MCP è troppo sottile per le forme strutturate
*(gap tecnico, piccolo, chiudibile — è dove sta la maggior parte della "distanza")*

MCP `kb.assert` passa le stringhe **grezze**, senza applicare la disciplina
virgolette/atomo che i file `.p0` applicano. Due conseguenze **dimostrate dal vivo**:

1. **Valori con iniziale maiuscola o `_` diventano VARIABILI** (`is_var`,
   `src/kb.c:95` — maiuscola/underscore = variabile). Nei `.p0` il contenuto è
   protetto da `"..."`; MCP non ha questo passo:
   ```jsonc
   kb.assert {"pred":"capital","args":["france","parigi"]}  ; kb.match {"pred":"capital","args":["france",null]}
     → {"bindings":["parigi"]}          // minuscolo: OK
   kb.assert {"pred":"capital","args":["spain","Madrid"]}   ; kb.match {"pred":"capital","args":["spain",null]}
     → {"bindings":[]}                   // "Madrid" preso per VARIABILE: il fatto è capital(spain, <var>)
   kb.query  {"pred":"capital","args":["spain","Madrid"]}
     → {"provable":true}                 // FALSO POSITIVO: nella query "Madrid" è pure variabile, unifica con tutto
   ```
   **Impatto diretto sulla visione di F.:** nomi propri (Paris, Madrid), template
   in sentence-case (`response_template(greet_name, "Welcome, {name}!")`),
   acronimi (SQL, API, HTTP), sigle — tutto ciò che inizia maiuscolo — **oggi non
   è insegnabile fedelmente via MCP**. E questa è gran parte della conoscenza
   grammaticale/lessicale/di-risposta reale.
2. **Le regole n-arie (join a più variabili) non sono asseribili** via
   `kb.assert_rule` (appiattisce a unario) — vedi [[parrot0-prolog-engine-nary]] e
   [[generative-prolog.md]] §"Piano operativo". Perciò "modi di interpretare i
   flussi" che richiedono relazioni a due posti non passano dal canale MCP.

È lo stesso pattern della scoperta sul motore: **MCP espone un sottoinsieme; il
KB è più capace del suo adattatore di scrittura.** Il fix è ai bordi, non nel
motore.

### Secchio B — la "colla grammaticale" è C per disegno
*(muro di principio, si scavalca solo caso-per-caso sotto pressione)*

Parte della comprensione NON è un insieme di forme, ma una **procedura
strutturale**, e vive in C perché è il *motore fisso*, non la *conoscenza*:

- accordo dell'articolo con il genere ed elisione (`il/la → l'`, `un/una → un'`),
  morfologia ("is sleeping" → verbo finito) — `src/brain/85-translate-synth-world.c`
  ("C supplies only grammar glue", righe 31, 243-305);
- riconoscimento articolo, tokenizzazione, normalizzazione — `src/brain/00-lex.c`
  (`is_article`, `split_words`);
- la risoluzione/unificazione stessa (`solve`, `unify`, `src/kb.c`).

Questi **non sono insegnabili come fatti**: un fatto descrive *cosa*, non *come
computare*. Insegnare "concorda l'articolo col genere" via MCP significherebbe
insegnare un *algoritmo*, che la rappresentazione KB (fatti + regole Horn) non
esprime come dato senza un meta-linguaggio interpretato. **Onestà:** alcune di
queste regole POTREBBERO migrare a forma dichiarativa (es. accordo come
`article(Gender, VowelInitial, Form)` interrogato da un kernel generico) — ma
solo se un benchmark lo reclama, con la disciplina Track 5 (una regola per
generazione, tirata dal gate), mai come "insegna un algoritmo via MCP".

### Secchio C — i processi elaborativi (corpi dei moduli) sono C
*(fuori perimetro MCP: è la via LOOP.md, non un tool)*

Track 5 migra a KB i **trigger** dei moduli (`intent_cue` chains), ma ciò che un
modulo *fa* dopo aver matchato — l'elaborazione vera — è codice
(`src/brain/*.c`). Insegnare un processo elaborativo genuinamente NUOVO via MCP =
scrivere nuovo C da remoto, che [[mcp-engine.md]] §7.3/§9 vietano esplicitamente:
la via per potenziare le *capacità* resta il loop di auto-evoluzione (`LOOP.md`),
non un tool MCP. MCP insegna **conoscenza** (forme, significati, relazioni), non
**facoltà**.

### Secchio D — ciò che il LINGUAGGIO KB non può ESPRIMERE (misurato gen280)
*(caccia esplicita a "qualcosa che via MCP non si può insegnare", dopo che U1/U1b
hanno chiuso A.1)*

Con i letterali e le variabili a posto, il limite si è spostato dai *bordi* al
**potere espressivo del linguaggio delle clausole** stesso. Due reperti, provati
dal vivo:

**D.1 — Non puoi insegnare a CALCOLARE, solo a RICORDARE** *(profondo ma sulla
roadmap: lo sblocca U3)*. MCP insegna fatti e regole di Horn su atomi, ma **non
una funzione ricorsiva su dati strutturati**, perché il motore non ha **termini
composti** (`prolog-like-engine.md` §2: "Liste/strutture ❌"). Misurato:
```jsonc
// una TABELLA di fatti funziona ma non generalizza:
kb.assert sum(2,1,3);  kb.match sum(2,1,?) → {"bindings":["3"]}
                       kb.match sum(2,2,?) → {"bindings":[]}         // mai insegnata → niente
// PEANO via .p0 (nat(z). nat(s($N)):-nat($N).) NON scatta:
kb.query nat(z)     → {"provable":true}
kb.query nat(s(z))  → {"provable":false}   // "s(z)" è un ATOMO PIATTO, non s(_) che unifica con s($N)
kb.match add(s(z),s(z),?) → {"bindings":[]}
```
`sum(2,1,3)` è memorizzabile; l'addizione (o `length`, `reverse`) **no**. È la
distinzione memorizzazione⁄computazione. Lo sblocca **U3** (termini-lista/composti,
§5.5): U3 non serve solo alle stringhe — è la porta per insegnare *computazione*.

**D.2 — Non puoi insegnare una GENERALIZZAZIONE DEFEASIBLE** *(il caso comune è
superabile con NAF, vedi §6.2/U6; la defeasibilità con priorità resta fuori)*.
"Gli uccelli volano, tranne i pinguini" come regola generale oggi non è
esprimibile: le clausole di Horn **definite** hanno solo teste positive. Misurato
(`flies($X):-bird($X). not(flies($X)):-flightless($X).` + `flightless(penguin)`):
```jsonc
kb.query flies(eagle)   → {"provable":true}
kb.query flies(penguin) → {"provable":true}   // la regola a testa negata NON prevale
```
L'eccezione andrebbe **enumerata** caso per caso (e nemmeno quello passa da MCP:
non c'è un tool di negazione, solo `kb.assert` positivo). Default, priorità,
non-monotonìa (`most`, `usually`, `unless`) sono un **altro tipo di logica**
(defeasible/ASP), non un upgrade di plumbing: NON è in U1..U5 e non va aggiunto
di slancio. È il bordo onesto: MCP+Horn insegna *ciò che è*, non *ciò che di norma
è salvo eccezioni*.

### Sintesi della distanza

| Ciò che F. vuole insegnare | Stato | Secchio |
|---|---|---|
| Termini, glosse, traduzioni (minuscoli) | ✅ già oggi | — |
| Idiomi/espressioni multi-parola (minuscoli) | ✅ già oggi | — |
| Forme di classe chiusa (stopword/conjunction) | ✅ già oggi | — |
| Fatti generali di dominio | ✅ già oggi | — |
| Nomi propri, template, acronimi (maiuscoli) | ✅ risolto (U1/U1b, gen279/280) | ~~A~~ |
| Relazioni a più posti / regole di join | ❌ non asseribili via MCP | **A** (→ U2) |
| Accordo, morfologia, articoli | ⚠️ C by design | **B** |
| Nuovi processi di interpretazione | ⛔ = nuovo C | **C** |
| Computazione ricorsiva (addizione, liste) | ❌ niente termini composti | **D.1** (→ U3, §6.1) |
| Default con eccezioni (`unless`) | ✅ risolto (U6/`naf`, gen282) | ~~D.2~~ (§6.2) |
| Defeasibilità con priorità / probabilità | ⛔ muro logico, fuori roadmap | **D.2** (resta fuori) |

**Dopo U1/U1b il Secchio A.1 è chiuso.** La distanza residua è: A.2 (regole n-arie
via MCP → U2), B/C (colla e facoltà in C), e i nuovi **D.1** (computazione → U3,
§6.1) e **D.2** (default con eccezioni → NAF/U6, §6.2; la defeasibilità con priorità
resta fuori). La visione di F. resta corretta e i due limiti D sono **superabili in
modo intelligente** (§6) senza cambiare motore: ciò che è *conoscenza* si insegna;
la *computazione* si insegna con l'unificazione strutturale; i *default con
eccezioni* con la negazione-per-fallimento; solo la non-monotonìa con priorità
resta un altro motore.

## 3. Il piano operativo (una generazione per gate)

Focalizzato sul Secchio A (chiudibile, onesto) più le migrazioni B caso-per-caso.
Metodo invariato: gate rosso → passo; refusal onesto al bordo; nessuna stringa
cablata; ordine per dipendenza.

| Gen | Obiettivo | Ratchet / gate |
|---|---|---|
| **C0** | **Harness `tests/mcp-teach.sh`**: insegna via MCP una forma di ogni categoria del §1 e verifica che il comportamento cambi (non solo che il fatto sia stoccato). Primo gate ROSSO: `capital(spain, Madrid)` insegnato via MCP deve rileggersi come `Madrid`, oggi fallisce → registra A.1. | Rosso onesto, come [[parrot0-prolog-engine-nary]] P0. |
| **C1** | **A.1 — scrittura fedele degli atomi.** MCP `kb.assert`/`assert_rule`/`response_template` applica la stessa disciplina del `.p0`: un argomento che inizia maiuscolo/`_`, o contiene spazi/punteggiatura, va **quotato** prima dello store (o si aggiunge un flag `literal`), così torna una costante e non una variabile. Simmetrico allo strip-virgolette già in `kb_intent_match`. **Nota:** il §5 (addendum) propone un fix più profondo — variabili esplicite + sostanza⟂presentazione — che rende C1 non necessario; C1 resta il ripiego minimale se l'addendum non parte. | `capital(spain, Madrid)` insegnato via MCP → `kb.match` restituisce `Madrid`; `kb.query` con valore sbagliato → `false` (niente più falso positivo). |
| **C2** | **A.2 — `kb.assert_clause` n-ario.** Il passo P1 di [[generative-prolog.md]]: regole con join a più variabili asseribili via MCP. Sblocca "interpretare flussi/relazioni" come conoscenza. | Il nonno via `kb.assert_clause` → `kb.query grandparent(tom,ann)` true, senza file `.p0`. |
| **C3** | **A.3 — tool ad-alto-livello per le forme di comprensione.** Wrapper MCP tipizzati sopra `kb.assert`, uno per categoria, che validano la forma e nascondono la quotatura: `teach.term {en,it}`, `teach.idiom {intent,phrase}`, `teach.template {intent,text}`, `teach.stopword {word}`, `teach.gloss {term,meaning}`. Non nuovo motore: adattatori che rendono la scrittura *sicura per costruzione*. | Ogni `teach.*` insegna una forma che poi **cambia una risposta** di `gen.respond` (es. `teach.idiom` di "how do you call me" → `gen.respond` la riconosce come richiesta del nome). |
| **C4** | **B (solo sotto pressione) — migrazione di UNA regola-colla a dichiarativa.** Scegliere la più isolata (es. accordo articolo→genere) e provarla come fatto interrogato da un kernel generico, tirata da un gate di traduzione. Se tradisce l'emergenza senza beneficio misurabile, pivot (non forzare). | Un caso di `mod_translate` che oggi passa per la colla C passa invece per una regola KB, stesso output — o si dichiara onestamente che resta C. |
| **C5** | **Documentazione + onestà del bordo.** Aggiornare [[prolog-like-engine.md]] e [[use-mcp-engine.md]] con la disciplina di quotatura; documentare i tre secchi come contratto pubblico ("cosa MCP può e non può insegnare"). | Un utente/agente legge e sa esattamente dove sta il muro. |

## 4. Cosa NON fare (perimetro)

- **Non insegnare algoritmi via MCP.** Il Secchio B/C non si "aggira" con un tool
  che scrive C o un meta-linguaggio procedurale: la via per nuove *facoltà* è
  `LOOP.md`. MCP insegna conoscenza, non motore ([[mcp-engine.md]] §9).
- **Non trasformare le forme in un phrasebook.** Ogni `teach.*` asserisce una
  forma *generativa* (una glossa che si ricompone, un template con slot), mai una
  frase-intera cablata — cardinal corollary [[kb-first-phrases]].
- **Non migrare la colla grammaticale in massa.** Il Secchio B si tocca una
  regola per volta, sotto pressione di benchmark, con dovere di pivot — non come
  refactor speculativo.
- **Non fidarsi dello store grezzo.** Il fix A.1 non è cosmetico: senza, ogni
  nome proprio insegnato via MCP è silenziosamente corrotto. È il primo gate.

## 5. Addendum (F., 2026-07-07): variabili esplicite + presentazione inferita

Due upgrade proposti da F. che **dissolvono il Secchio A.1 alla radice** invece
di rattopparlo con la quotatura del §3-C1, e iniziano a migrare il Secchio B.
Registrati qui come direzione da tirare con un gate, non ancora gen-planned.

> **F.:** «possiamo mettere una sintassi per la definizione delle variabili
> facendo un piccolo upgrade alla nostra macchina Prolog-like. Inoltre mi
> piacerebbe che la trasformazione di un dato alla presentazione sia essa stessa
> espressa attraverso la conoscenza: astrarre la **sostanza** dalla sua
> **presentazione**, e la presentazione sarà inferita, dove esisteranno dei
> **manipolatori addestrabili** — ad esempio *mostrare un testo con la prima
> maiuscola* sarà una conoscenza.»

### 5.1 Variabili esplicite (piccolo upgrade al parser, non al solver)

Oggi la variabile è **implicita** (`is_var`, `src/kb.c:95`: iniziale maiuscola o
`_`). È la causa diretta del bug §2-A.1: `Madrid` *sembra* una variabile.
Rendere la variabile **esplicita** (sigillo, es. `?X` / `$X`) capovolge il
default: il contenuto è sicuro-per-costruzione, la variabile è marcata.

- **Additivo, non distruttivo:** centinaia di regole `.p0` usano `X`/`Y` nudi. Il
  sigillo COESISTE con la convenzione attuale; sul canale MCP (dati da agente)
  diventa la forma canonica. Tocca `parse_term`/`is_var`, non `solve`.
- **Alternativa MCP-locale (ancora più piccola):** nell'argomento JSON, una
  variabile è un oggetto `{"var":"X"}` e una stringa nuda è SEMPRE un letterale.
  Zero cambi al motore, letterali sicuri di default al bordo. Da scegliere in
  implementazione: sigillo nel linguaggio clausole vs marcatura al confine MCP.

### 5.2 Sostanza ⟂ presentazione: manipolatori addestrabili

Separare ciò che una cosa È da come viene MOSTRATA. La sostanza è canonica
(minuscola, atomica); la presentazione è **inferita** componendo manipolatori.

```
Sostanza (KB)          capital(spain, madrid).
Tipo                   proper_noun(madrid).
Presentazione (KB)     present(proper_noun, capitalize_first).   % ← conoscenza addestrabile
Realizzazione          madrid --capitalize_first--> "Madrid"
```

**Non basta rendere conoscenza la SCELTA del manipolatore — anche l'AZIONE deve
esserlo** (correzione di F., 2026-07-07: «anche l'eseguire capitalize_first deve
essere conoscenza; niente libreria C fissa di primitive di formattazione»). Una
prima versione di questa sezione lasciava le primitive (`capitalize_first`,
`article_it`…) come libreria C fissa. F. la rifiuta, giustamente: quella sarebbe
ancora colla cablata, solo spostata. La posizione corretta è §5.3.

### 5.3 L'azione su stringa COME conoscenza (regole + tabelle, non primitive C)

Il modo per rendere conoscenza *anche l'esecuzione*: smettere di trattare la
stringa come blob opaco e trattarla come **struttura su cui il motore unifica**.
Allora ogni azione è una **regola Horn** e ogni mappa una **tabella di fatti** —
entrambe KB, entrambe insegnabili via MCP.

```prolog
% (1) LA MAPPA = conoscenza (tabella di fatti, addestrabile: insegni il casing di
%     un nuovo alfabeto via MCP, zero C)
upper(a, 'A').  upper(b, 'B').  ...  upper(m, 'M').  ...

% (2) L'AZIONE = conoscenza (regola sulla struttura testa/coda della stringa)
capitalize_first([H|T], [U|T]) :- upper(H, U).

% (3) LA SCELTA = conoscenza (quale azione per quale tipo)
present(proper_noun, capitalize_first).
```

`capitalize_first` **non è più C**: è una clausola. `madrid` → `[m,a,d,r,i,d]` →
la regola mappa la testa via `upper/2` → `[M,a,d,r,i,d]` → `Madrid`.

**Il confine irriducibile (onestà, non c'è "zero C"):** il motore di unificazione
stesso è C. La mossa giusta è **spingere il substrato fisso al livello più
generale e cieco-all'operazione possibile**, così che *nessuna azione specifica*
sia mai C. Resta fisso solo:
1. **unificazione + risoluzione** — già esiste (`solve`, `unify`, `src/kb.c`);
2. **(de)serializzazione stringa⟷struttura** — `"madrid"`⟷`[m,a,d,r,i,d]`: UNA
   primitiva generale, agnostica all'operazione (non sa cos'è "capitalize", sa
   solo spacchettare/impacchettare caratteri).

Con questi due, `capitalize_first`, `pluralize`, accordo articolo, elisione —
**tutto** diventa regole+fatti KB. Il motore guadagna *struttura*, non *operazioni*.
È lo stesso pattern di `algo_shape/2` portato al limite, e la realizzazione
dichiarativa dello stadio "lingua = ultimo passaggio" di [[generative-prolog.md]].

**Costo onesto:** serve un upgrade reale al motore — **termini composti / liste
con unificazione testa-coda** (`[H|T]`), oggi assenti (gli argomenti sono atomi
piatti — vedi [[prolog-like-engine.md]] §2 "Liste/strutture ❌"). È lo STESSO
upgrade che [[generative-prolog.md]] vuole per il ragionamento strutturale: non è
speso solo per le stringhe. È il più grosso dei tre upgrade di questo addendum
(variabili esplicite < `present/2` < termini-lista), ma è l'unico che realizza
*davvero* «l'azione è conoscenza».

**Alternativa senza liste (peggiore, citata per onestà):** codificare i caratteri
come fatti `char(w,0,m)` + tabella `next(0,1)` per l'indice — evita il nuovo
tipo-termine ma reintroduce aritmetica-come-fatti goffa e non ha il potere
compositivo dell'unificazione testa-coda. Le liste sono più pulite.

### 5.4 Perché insieme risolvono più di quanto costano

- **A.1 sparisce:** non si memorizza mai `Madrid` come sostanza → niente atomo
  corrotto in variabile. Si memorizza `madrid` + `present(proper_noun, capitalize_first)`.
- **Secchio B inizia a migrare:** accordo articolo/genere, casing, morfologia —
  oggi colla C in `src/brain/85-translate-synth-world.c` — diventano catene di
  `present/2` dichiarative, una regola per volta, sotto pressione di un gate.
- **Coerente con la visione:** è esattamente il "piano del discorso →
  realizzazione linguistica" di [[generative-prolog.md]], reso dato e allenabile.

### 5.5 Sequenza degli upgrade al motore (COMMITTED, F. 2026-07-07)

> **F.:** «dobbiamo necessariamente fare gli upgrade al motore che esso
> necessita, perché l'arricchimento è molto elevato.»

Decisione presa: gli upgrade al motore NON sono più "direzione da pressare" ma
**lavoro committato**. Il payoff (strings-as-knowledge + ragionamento
strutturale + realizzazione linguistica allenabile) giustifica il costo. Restano
però disciplinati: **gate-first, uno per generazione**, ordinati per dipendenza e
per costo crescente. Ogni upgrade è al MOTORE (fisso, generale), mai
un'operazione specifica.

Stato (gen281): **U1 ✅ spedito** in due passi — gen279 quota i letterali al bordo
MCP (fedeltà: `Madrid`/`$HOME`/template round-trippano) e gen280 introduce `$`
come marcatore di variabile esplicito (dual-accept; il flip a `$`-only è gen B,
vedi `NEXT.md`). **U2 ✅ spedito** in gen281 — `kb.assert_clause` (src/kb.h,
src/kb.c, src/mcp.c) permette di insegnare regole n-arie con join a più variabili
via MCP (`tests/assertclause.sh`). **U3 è il prossimo.**

| Ord | Upgrade al motore | Sblocca | Costo | Gate d'ingresso |
|---|---|---|---|---|
| **U1** ✅ | **Variabili esplicite** (`$` come sigillo, gen280) + **fedeltà dei letterali** (quotatura al bordo, gen279); additivo a `is_var`/`parse_term`/`mcp.c` | A.1: letterali sicuri; nomi propri/`$`-valori/template insegnabili | basso | `capital(spain, Madrid)` via MCP → `kb.match` torna `Madrid` (`tests/mcp-teach.sh`, `tests/dollarvar.sh`) |
| **U2** ✅ | **Costruzione clausole n-arie via MCP** (`kb.assert_clause`; gen281) | relazioni/join come conoscenza insegnabile | basso | nonno via MCP → `grandparent(tom,ann)` true senza `.p0` (`tests/assertclause.sh`) |
| **U3** | **Termini composti / liste con unificazione testa-coda** (`[H|T]`) — *l'upgrade grosso, "arricchimento molto elevato"* | strings-as-knowledge, ragionamento strutturale, il Prolog generativo | alto | una regola `capitalize_first([H|T],[U|T]):-upper(H,U).` risolve su `[m,a,d,r,i,d]` |
| **U4** | **(de)serializzazione stringa⟷struttura** — UNA primitiva generale, cieca all'operazione | l'azione su stringa diventa interamente regole+fatti KB | medio | `present(proper_noun,capitalize_first)` insegnato via MCP rende `madrid`→`Madrid` in `gen.respond`, sostanza resta `madrid` |
| **U5** | **Migrazione del Secchio B**, una regola-colla per volta (accordo, elisione, morfologia) da C a `present/2` | `85-translate-synth-world.c` si assottiglia; grammatica allenabile | per-item | un caso di `mod_translate` passa per regole KB, stesso output |

U1/U2 sono piccoli e sbloccano subito; U3 è il cuore (e va speso una sola volta,
serve anche a [[generative-prolog.md]]); U4/U5 vivono sopra U3. Ordine per
dipendenza: U4 richiede U3; U5 richiede U3+U4. Nessun codice prima del gate rosso
corrispondente; se U3 minaccia terminazione/emergenza senza beneficio misurabile,
si chiama il pivot (dovere di `primitives-first-pivot-duty`).

## 6. Superare D.1 e D.2 in modo intelligente (design, gen280)

I due limiti del Secchio D non richiedono un motore diverso: richiedono di rendere
il **solver** un po' più intelligente, restando dentro "engine fisso e generale,
la conoscenza impara". Ancoraggio al codice (letto a gen280): `Term.args` è piatto
(`char args[][KB_TERM_LEN]`, `src/kb.c:34`) → nessuna struttura; la negazione sono
solo fatti ground nella lista `neg` (`kb_is_negated`, `src/kb.c:209`), controllata
in cima a `kb_query` (`:449`) → nessuna negazione nel corpo delle regole.

### 6.1 D.1 → unificazione STRUTTURALE su termini composti (il "vedere struttura")

**Idea intelligente (minima, generale):** non serve un nuovo tipo di dato. Lo
storage a stringa già CONTIENE `s(z)`; si insegna all'UNIFICATORE a *vederci
dentro* una struttura. Un argomento che ha forma `f(a1,…,an)` è un termine
composto; `unify` ci ricorre; una variabile (`$X`) si lega a una sotto-struttura
intera (una stringa). Riusa la `Subst` string-based già esistente — il motore
guadagna una **lente**, non un datatype.

Modifiche (limitate, tutte in `src/kb.c`):
1. `term_split(str, &functor, args, &argc)` — spacchetta `f(…)` rispettando
   parentesi annidate e virgolette (generalizza `parse_term`, `:656`).
2. `unify` diventa **ricorsivo**: risolvi a,b; se uno è var → lega (a una
   sotto-struttura intera); se entrambi composti con stesso funtore/arità →
   unifica gli argomenti a coppie; se entrambi atomi → `strcmp`.
3. `rename_term` (`:346`) rinomina le variabili **annidate** dentro le stringhe
   composte (`s($N)`→`s($N_7)`), non solo l'arg-se-variabile top-level.
4. Terminazione: la guardia `KB_MAX_DEPTH` (`:26`) già copre la ricorsione.

**Payoff:** Peano (`add(z,$Y,$Y). add(s($X),$Y,s($Z)):-add($X,$Y,$Z).`), liste come
`cons(H,T)`/`nil` (sugar `[H|T]`), quindi `length`/`reverse`/`append` E le
stringhe-come-conoscenza (U3/U4). La **computazione diventa conoscenza
insegnabile**, non C. È **U3** di §5.5, ora con spec affilata.
*Gate:* `add(s(z),s(z),$R)` → `$R=s(s(z))`; `length(cons(a,cons(b,nil)),$N)` → `$N=s(s(z))`.
*Alternativa onesta:* se il parse-on-unify diventa troppo, si passa a un `Term`
ricorsivo vero (Opzione A) — più costo, stessa semantica.

### 6.2 D.2 → negazione-per-fallimento (NAF) nel corpo: i default come conoscenza  ✅ SPEDITO (U6, gen282)

> **Fatto (gen282).** `naf(G)` nel corpo (o `"neg":true` su un goal di
> `kb.assert_clause`) è implementato: `Term.neg`, strisciamento del wrapper al
> parse/assert, NAF in `solve()` e `prove_seq_ex()` con floundering-guard
> (solo goal ground). Gate `tests/naf.sh` (via `.p0` e via MCP). `kb_save`
> round-trippa il wrapper. La defeasibilità con priorità/probabilità resta fuori.

**Il "muro" è in realtà una porta ben precisa.** Non serve ASP/defeasible completo
per il caso reale ("gli uccelli volano *salvo eccezioni*"): basta la
**negation-as-failure stratificata**, l'estensione standard e modesta con cui il
Prolog vero fa i default. Un goal di corpo `naf(G)` riesce se `G` NON è derivabile:

```prolog
flies($X)    :- bird($X), naf(abnormal($X)).
abnormal($X) :- flightless($X).
bird(eagle).  bird(penguin).  flightless(penguin).
```
→ `flies(eagle)` true, `flies(penguin)` false. **Generalizzazione defeasible come
conoscenza insegnabile**, senza enumerare le eccezioni.

Modifiche (limitate):
1. In `solve()` (`:380`), un goal di corpo `naf(G)` (funtore riservato,
   auto-documentante; distinto dal fatto negativo `not/1` top-level) si prova con
   un `solve` annidato: se FALLISCE, `naf` riesce (senza nuovi binding); se
   riesce, `naf` fallisce.
2. **Stratificazione + guardia floundering:** applica `naf` solo a goal **ground**
   (dopo che le var sono legate); se `G` non è ground, **declina onestamente**
   (niente NAF su variabili libere — è il bordo corretto, non un bug). Depth-guard
   già presente.

**Onestà su cosa NON copre** (e resta fuori, deliberatamente): priorità fra
default in conflitto ("il più specifico vince"), credenza graduata/probabilistica,
non-monotonìa piena. `naf` copre il caso dimostrato (P salvo eccezione) con una
tecnica nota e limitata — non è un motore ASP. È un nuovo upgrade piccolo,
chiamiamolo **U6**.

### 6.3 Catena di dipendenze verso MCP (perché entrambi passino DAL VIVO)

Per insegnarli via MCP (non solo `.p0`) serve prima poter mandare regole ricche:
**U2** (`kb.assert_clause`, corpi n-ari via MCP) è **prerequisito** sia di U3 (le
clausole Peano/lista) sia di U6 (il goal `naf` nel corpo). Ordine consigliato:

1. **U2** — `kb.assert_clause` (già in roadmap; sblocca qualunque regola via MCP).
2. **U6** — NAF nel corpo → D.2 (piccolo, alto valore, indipendente da U3).
3. **U3** — unificazione strutturale → D.1 (il grande, spinge anche stringhe/liste).

Ogni passo gate-first; U6 e U3 sono estensioni non-monotone/strutturali reali →
attenzione a terminazione, floundering, stratificazione, con dovere di pivot.

## 7. Collegamenti

[[mcp-engine.md]] (il canale e i suoi limiti), [[generative-prolog.md]] (il motore
n-ario e il suo sblocco — C2 vive lì), [[prolog-like-engine.md]] (il contratto del
protocollo, dove `is_var` e la quotatura sono documentati), [[unification.md]] /
[[unification-assessment.md]] (l'unificazione come collante fra domini), `PRINCIPLES.md`
(la legge "engine fixed, knowledge learns" che tutto questo rispetta),
`src/brain/*.cues.p0` (Track 5: la migrazione dei trigger di comprensione già in
corso — il precedente vivo più vicino).
