# The Model Plan — dalla tabella al modello

> **Stato:** piano fondativo, aperto il 2026-07-27 (gen364) dopo che una misura
> ha reso visibile un difetto che nessun run di benchmark aveva mai nominato.
> **Subordinato a** `PRINCIPLES.md`. Dove un piano operativo (`motorize-the-class`,
> `generative-prolog`, `universal-solver`, `universal-comprehension`) presuppone
> una struttura che questo documento dimostra assente, prevale questo documento
> sull'**ordine dei lavori**, mai sui principi.

---

## 0. Perché questo piano nasce da un errore, e da quale

Questo documento non nasce da un'idea. Nasce da cinque malintesi consecutivi,
tutti miei, tutti nella stessa sessione, tutti che puntavano nella stessa
direzione senza che io la vedessi. Li scrivo per esteso perché **sono trappole
ricorrenti**: chi riprende il lavoro le incontrerà di nuovo, e il modo di
riconoscerle vale più della conclusione.

### M1 — «Legare non è fondare, e fondare non è fare»

Tre iterazioni di `motorize-the-class`, ciascuna misurata fuori campione su un
tail nuovo e non letto, hanno prodotto **0/20, 0/20, 1/20**. Sessanta giudizi
indipendenti dicono tutti la stessa cosa con parole quasi identiche:

> *«generic template that names the topic but never discusses it»*

La tesi di gen362 — *«la specificità è una sostituzione»*: lega il soggetto negli
atomi di metodo e la risposta diventa sul soggetto — è **falsificata**. Un metodo
enunciato *a proposito* di un soggetto resta un metodo. Nessuno chiedeva come si
analizza qualcosa; chiedevano l'analisi.

### M2 — Coprire una classe non è implementare un'operazione

Circa metà di quei sessanta quesiti erano della stessa forma: «se la gravità si
invertisse», «una lingua senza possesso», «oceani sostituiti da un solido»,
«nessuna parola per i numeri», «memorie esterne pubbliche». Li avevo raggruppati
— e chiamati «controfattuali» — e poi trattati come **una classe da coprire con
altri fatti**. Non sono una classe. Sono **una sola operazione** applicata a
mondi diversi. Un piano che risponde a un'operazione con una tassonomia sta
moltiplicando le righe per non scrivere un motore.

### M3 — L'ablazione non è una disciplina di sviluppo

Il malinteso più grosso. Dopo aver scoperto che due dei miei candidati erano
codice morto, ho proposto di fare dell'ablazione **il metodo con cui sviluppo il
prodotto**: misurare quali pezzi portano peso, e costruire di conseguenza.

È sbagliato. L'ablazione è **un'operazione di ragionamento che parrot0 esegue** —
uno degli organi di cui il ragionamento è fatto, non l'unico, e l'anatomia
completa non la conosciamo. `PRINCIPLES.md` cita assert/retract una volta sola
(righe 151-154) e come **test di conformità** per provare che un riconoscitore è
KB-first. Io ho preso un dettaglio d'igiene e l'ho promosso ad architettura.

La forma corretta è una sola, e vale per qualunque mondo:

```
togli un pezzo dal modello  →  deriva che cosa smette di seguire
```

### M4 — Chiedere alla conoscenza se «porta peso» è un errore di categoria

Avendo confuso M3, ho applicato la domanda anche alla KB: classi sature, fatti
che non portano peso, quale classe merita più fatti. Sbagliato. **Un fatto non
si giustifica perché oggi cambia una risposta.** `capital_of_country/2` per un
paese che nessuno ha ancora nominato non è peso morto: è conoscenza. Il motore
si giudica per ciò che regge; la conoscenza si giudica per ciò che copre.

Il corollario che chiarisce tutto: **quando ablo un fatto non sto misurando il
fatto, sto sempre misurando il motore.** Il fatto è la sonda.

### M5 — Il file-giocattolo, e il motivo vero per cui l'ho scritto

Per dimostrare di aver capito ho creato `kb/core/facts/ablation.p0`: cinque
assunzioni, sette dipendenze, un motore che ne fa la differenza insiemistica, e
una demo che gira. Tre cose sbagliate:

1. **Le risposte erano memorizzate.** `compensa(prestito_simultaneo, "separare il
   contenuto dal supporto — lettura ad alta voce…")` *è* la risposta, digitata da
   me in un fatto. La "derivazione" sceglieva soltanto quale frase pre-scritta
   stampare: `qa_reply` con una differenza insiemistica davanti — l'anti-pattern
   che avevo condannato io stesso tre messaggi prima. E la demo dichiarava
   «nessuna risposta è memorizzata»: falso.
2. **Ho chiamato un file come la facoltà.** Un file `ablation.p0` con dentro un
   mondo privato significa aver già deciso che l'ablazione è un modulo col suo
   giardinetto. È la violazione, a livello di architettura, del mantra che vieta
   al sostantivo di nominare il motore.
3. **Ho ottimizzato per la demo.** Mi era stato chiesto di dimostrare
   comprensione; ho prodotto una cosa che gira.

Ma il punto vero è il **perché**: ho dovuto fabbricarmi un mondo privato **perché
non ne esisteva uno su cui far girare l'operazione**. Quella era la scoperta, e
invece di riportarla ci ho costruito attorno una demo verde.

L'artefatto è stato ritirato. Il malinteso resta scritto qui.

### M6 — Il livello sbagliato, e nessuna prova

In tutti e tre i giri sono andato dritto alla primitiva C — campo in `Brain`,
resolver, estrattore, loop di typing — saltando i livelli che
`parrot0-forge-master-plan.md` §1.3 impone di provare prima (fatto → schema →
clausola → action schema → C). Ho usato **LLMSCORE, un voto linguistico**, come
oracolo, contro §1.2. E non ho mai ablato: quando finalmente l'ho fatto, due
candidati su quattro non riportavano il caso al rosso — **erano codice morto**, e
un retract da dieci secondi me lo avrebbe detto prima di scriverli.

---

## 1. La misura che ha aperto il piano

Tutti i malintesi puntavano allo stesso posto. Quando ho smesso di argomentare e
ho contato:

| grandezza | valore |
|---|---:|
| fatti nella KB | **48.911** |
| di cui `lexeme/…` (un dizionario, letteralmente) | 35.551 |
| fatti non-lessicali | 13.360 |
| **clausole (`:-`) in tutta la KB** | **251** |
| di cui in `procedures.p0` / `meta.p0` (machinery del motore) | 192 |
| **clausole che parlano del MONDO** | **59** |
| predicati distinti | 680 |
| predicati che compaiono in un corpo di regola | 117 |
| **predicati da cui non si deriva mai nulla** | **563 (83%)** |
| righe di C | 45.148 |
| **righe di C per ogni clausola di conoscenza** | **177** |

> **La KB di parrot0 è un dizionario, non un modello.** Quasi cinquantamila
> fatti che non seguono l'uno dall'altro, e cinquantanove regole sul mondo.

Questo spiega, senza bisogno di altre ipotesi:

- **perché i tre giri producevano metodo.** `generative-prolog.md` chiede un
  «percorso semantico» attraverso il grafo. Un percorso ha bisogno di archi: qui
  gli archi sono 251 su 48.911. Non c'era niente da attraversare, quindi l'unica
  cosa che il motore poteva fare era disporre frasi già scritte;
- **perché ho dovuto fabbricare un mondo per la demo** (M5);
- **perché `universal-solver` è fermo allo strato A.** Deduzione, abduzione e
  verifica presuppongono dipendenze;
- **perché ogni gap si chiude scrivendo C.** Con 177 righe di C per clausola, il
  percorso di minima resistenza è sempre il codice.

Il problema non era il planner, né il binding, né le famiglie di artefatti. **Non
c'è il grafo.**

---

## 2. Missione ufficiale

> **Costruire un motore C minimale ed essenziale, e una KB viva — fatti,
> strutture, procedure, logica, inferenze, ablazioni — tale che l'interazione
> risultante regga il confronto con quella di un LLM.**
>
> Tutto ciò che è *conoscenza, ragionamento o forma* vive nella KB e cresce senza
> ricompilare. Nel C resta soltanto ciò che nessuna conoscenza può esprimere:
> unificazione, risoluzione, ricerca, misura, I/O.
>
> La grandezza da massimizzare non è il numero di fatti: sono **gli archi** — le
> dipendenze da cui qualcosa segue.

Tre precisazioni che rendono la missione falsificabile invece che aspirazionale.

**«Paragonabile a un LLM» significa comportamento, mai identità.** parrot0 non è
un LLM e la regola no-deception gli vieta di fingersi tale. Il confronto è
sull'*interazione*: ampiezza dei compiti ingaggiati, pertinenza, completezza,
capacità di reggere una premessa nuova. Resta la scommessa di `PRINCIPLES.md`,
non un teorema.

**«Minimale» è una direzione misurabile, non un aggettivo.** Oggi il rapporto è
177 righe di C per clausola. Ogni generazione deve poter dire se quel numero è
salito o sceso. Un motore che cresce mentre le clausole non crescono sta
riportando conoscenza dentro il codice.

**«Viva» significa che la conoscenza deriva.** Un fatto inerte è conoscenza; una
KB fatta *solo* di fatti inerti non è un modello. Il criterio è: da questo
predicato segue qualcosa? Oggi la risposta è no per 563 predicati su 680.

---

## 3. La tesi: gli archi, non le righe

Un LLM non ha più *fatti* di parrot0. Ha, ricavata dal training, una **struttura
funzionale** che collega ciò che sa — ed è quella struttura che `PRINCIPLES.md`
chiama massa critica intelligente e questo esperimento cerca di ri-esprimere in C.

La forma di parrot0 oggi è l'opposto: massima massa, minima struttura.

```
        LLM                              parrot0 oggi
   fatti  ~ tanti                    fatti  48.911
   archi  ~ densi                    archi     251
   ──────────────────────            ──────────────────────
   la risposta è un PERCORSO         la risposta è una RIGA SCELTA
```

Da cui il principio operativo di questo piano, che si affianca a *engine fixed,
knowledge learns*:

> **Un fatto nuovo che entra senza le sue dipendenze è debito.** Non è sbagliato
> — è conoscenza — ma non aumenta ciò di cui parrot0 è capace. La crescita che
> conta è quella che aggiunge **archi**.

E il corollario che riordina i piani esistenti: `generative-prolog` (i percorsi),
`universal-solver` (deduzione e abduzione), `universal-comprehension` (schemi
d'intento), `motorize-the-class` (atti e piani) **non sono in competizione**.
Sono tutti consumatori dello stesso grafo mancante. Costruire il grafo li sblocca
insieme; continuare senza si è già visto che cosa produce.

---

## 4. Che cosa deve contenere una KB viva

Cinque strati. Oggi il primo è enorme e gli altri quattro sono quasi vuoti — ed è
esattamente questa la sproporzione da correggere.

| strato | che cos'è | stato oggi |
|---|---|---|
| **fatti** | ciò che è vero di un'entità | 48.911 — abbondante |
| **strutture** | tipi, ruoli, gerarchie: *che genere di cosa* è un'entità e quali posti apre | frammentario |
| **dipendenze** | clausole: che cosa segue da che cosa | **59 sul mondo** |
| **procedure** | trasformazioni insegnabili sopra i primitivi | avviato (`procedures.p0`) |
| **operazioni** | gli organi del ragionamento che percorrono il grafo | 1 dimostrata (indovinelli) |

Le **operazioni** sono il livello che questo piano dichiara aperto e non risolto.
Ne conosciamo alcune, non l'anatomia:

- **deduzione** — dato Γ, che cosa segue (l'unica con substrato maturo);
- **ablazione** — togli un pezzo, deriva che cosa smette di seguire;
- **abduzione** — data un'osservazione, quali ipotesi minime la spiegano;
- **composizione** — costruisci un percorso che descriva o produca qualcosa;
- **proiezione** — realizza un percorso in lingua (l'ultimo passaggio, mai il primo).

Questo elenco è **provvisorio per costruzione**. Non lo si chiude a tavolino: si
scopre un organo quando un compito reale lo esige e nessuna combinazione degli
altri lo copre. Ogni organo scoperto è una generazione; ogni organo va sopra il
grafo, e nessuno porta con sé un mondo privato (M5).

---

## 5. Il motore C minimale

Il confine, per il verso positivo — che cosa **resta** nel C:

- unificazione, risoluzione, negazione per fallimento, indici;
- ricerca e budget: attraversare, limitare, fermarsi;
- misura ed evidenza: contare, confrontare ipotesi, restituire una proof;
- meccaniche di superficie: tokenizzare, bilanciare delimitatori, offset;
- I/O: file, socket, MCP, sandbox.

E per il verso negativo — che cosa **deve uscire**, in ordine di gravità:

1. **frasi di risposta scritte in C** (`snprintf`/`put` di prosa);
2. **liste di parole** — cue, trigger, sinonimi, aperture, unità. Il conteggio
   esiste già in `coding-agent-todo.md` O1/O5: **~790 siti `cue(` nel C**;
3. **decisioni di dominio prese da catene di `if`** invece che dal confronto di
   ipotesi sull'evidenza;
4. **conoscenza travestita da ordine di dispatch.** È la diagnosi più sottile e
   me l'ha data un'ablazione: due miei candidati non tornavano rossi non perché
   fossero inutili, ma perché un consumer precedente prendeva il turno. In quel
   caso **l'elemento portante non era un fatto: era la posizione nel registry** —
   cioè conoscenza cablata nella struttura del programma.

Ne segue un requisito per gli strumenti: si deve poter ablare **anche ciò che non
è un fatto** — posizione, guardia, consumer, regola. Un'ablazione che sa togliere
solo fatti conclude «morto» dove la verità è «oscurato», e sono diagnosi opposte.

---

## 6. Metriche

Deterministiche, senza API, calcolabili a ogni generazione. La baseline è la
tabella del §1.

| metrica | oggi | direzione |
|---|---:|---|
| clausole sul mondo | 59 | ↑ — è **la** metrica del piano |
| predicati da cui si deriva qualcosa | 117 / 680 | ↑ |
| righe di C per clausola | 177 | ↓ |
| siti `cue(` nel C | ~790 | ↓ a zero |
| frasi di risposta nel C | > 0 | ↓ a zero |
| organi di ragionamento con substrato reale | 1 | ↑ |

LLMSCORE resta **controllo esterno, non bussola**: voto linguistico, campione
piccolo, non deterministico. E oggi è anche parzialmente rotto come misura —
l'avvio a freddo di `./bin/parrot0` è **0,808 s** contro una deadline di 1,0 s
(1,31 s con gli otto worker), quindi produce falsi zero e **tassa la crescita
della conoscenza**. Va riparato con un processo caldo (`--daemon`,
`--test-engine`, che esistono già) prima di essere di nuovo citato come evidenza.

---

## 7. Disciplina di lavoro

Nata dai sei malintesi. Ogni riga corrisponde a un errore realmente commesso.

1. **Prima di aggiungere, misura che cosa c'è.** Il numero del §1 era disponibile
   in trenta secondi e avrebbe risparmiato tre giri.
2. **Il livello prima del codice.** Fatto → struttura → clausola → procedura →
   action schema → C. Chi scrive C dichiara quale livello ha provato e perché non
   bastava.
3. **Nessun candidato è «fatto» senza un caso che torni rosso togliendolo.**
   Vale per il motore. Non vale per la conoscenza (M4).
4. **Nessun mondo privato per far girare una demo.** Se un'operazione non ha
   substrato, il risultato da riportare è *che non ce l'ha*.
5. **Nessuna risposta memorizzata travestita da derivazione.** Se il testo finale
   è stato digitato da un umano in un fatto, si dice.
6. **Un oracolo meccanico batte un voto linguistico.** Quando esiste, il voto non
   conta come prova.
7. **Ogni incremento dichiara quanti archi aggiunge.** Se sono zero, è un
   arricchimento del dizionario: legittimo, ma non è avanzamento di questo piano.

---

## 8. Che cosa questo piano NON dice

- **Non dice di smettere di aggiungere fatti.** Un dizionario grande è un bene;
  ne serve uno grande *e* un grafo. Il difetto è la sproporzione.
- **Non dice di riscrivere il motore.** Vale la regola delle strutture
  secondarie: si aggiunge, non si pota. Il C in eccesso si riduce migrando, non
  cancellando ciò che funziona.
- **Non dice che l'ablazione è la risposta.** È un organo, forse fra molti.
  L'anatomia del ragionamento è dichiarata **aperta**, e chiuderla per decreto
  sarebbe lo stesso errore che ha prodotto M5, un livello più in alto.
- **Non promette la parità con un LLM.** `PRINCIPLES.md` la chiama scommessa. Qui
  si sostiene una cosa più stretta e verificabile: **finché gli archi sono 251, la
  parità non è nemmeno tentabile** — e questo si può misurare.

---

## 9. Primo passo

Non un'implementazione. Una **mappa**: quali dei 563 predicati inerti avrebbero
dovuto avere dipendenze, e da dove entrano oggi i fatti che le perdono per
strada — ingestione, autolearn, curatura a mano. Il difetto non è che mancano
regole: è che **la pipeline di crescita produce fatti senza archi**, e finché
resta così ogni conoscenza nuova nascerà già inerte.

Riparare il produttore viene prima di aumentare il prodotto.

---

## 10. Prima sessione operativa — misurare e collegare il corpus (gen365)

> **Eseguita dal vivo il 2026-07-28.** Teacher via `kb.assert_clause` prima,
> promozione nei file curati solo dopo query, controesempi e proof.

Il primo passo del §9 ora ha uno strumento riproducibile:
`scripts/kb_graph.py`. Legge clausole logiche (non righe fisiche), ignora
commenti e direttive `include`, distingue `procedures.p0`/`meta.p0` dalle
clausole sul mondo e ordina i predicati inerti per quantità di fatti. Il
conteggio è quindi semanticamente più stretto del conteggio lessicale `:-` del
§1; i delta sotto usano lo stesso strumento su entrambi gli stati.

| metrica deterministica | prima (`b47d622`) | dopo il round | delta |
|---|---:|---:|---:|
| fatti effettivi letti dallo scanner | 48.730 | 48.730 | **0** |
| clausole core | 242 | 277 | **+35** |
| clausole machinery | 192 | 192 | **0** |
| **clausole sul mondo** | **50** | **85** | **+35 (+70%)** |
| predicati consumati da un corpo | 123 / 681 | 148 / 706 | **+25** |
| quota predicati inerti | 558 / 681 (81,9%) | 558 / 706 (79,0%) | **−2,9 punti** |
| righe C | 45.148 | 45.167 | **+19** |

L'inerzia assoluta resta 558 perché il round introduce anche nuovi predicati
derivati; la quota scende e, soprattutto, 25 sorgenti prima isolate entrano in
un corpo. Sono **1.550 righe-fatto preesistenti** ora consumate da almeno una
clausola: cronologia e biografie, continenti/lingue/fiumi/landmark, tavola
periodica, orbite, quantità, magnitudini, suoni, colori, sinonimi e contrari.

### 10.1 I quattro batch

1. **Tempo storico.** `historical_year/2` unifica sette tabelle d'anno;
   `historical_before/2` applica una sola operazione di confronto a tutte.
   `historical_lifespan/3`, `shared_birth_year/3` e `lives_overlap/2` compongono
   nascita e morte invece di lasciare le due colonne separate.
2. **Geografia.** `country_profile/4` fa il join
   capitale × continente × lingua. Gli stessi parenti alimentano
   `capital_in_continent/2`, `capital_language/2`,
   `shared_official_language/3`, `landmark_in_continent/2` e
   `river_in_continent/2`.
3. **Scienza quantitativa.** `element_identity/3` unisce numero atomico e
   simbolo; `orbits_t/2` è una closure right-recursive; quantità e magnitudini
   diventano confrontabili da procedure generali. `sound_of/2` alimenta
   `emits/2`; colore e suono producono join condivisi.
4. **Lessico come grafo.** `word_relation/3` reifica sinonimia e antonimia senza
   perdere il tipo dell'arco.

Nessuna conclusione di controllo è memorizzata. Esempi di proof reali:

```
country_profile(france, paris, europe, french)
  because capital_of_country(france, paris)
      and continent_of(france, europe)
      and language_of_country(france, french)

orbits_t(moon, sun)
  because orbits(moon, earth)
      and orbits_t(earth, sun)
      because orbits(earth, sun)
```

Il batch di archi ha aggiunto **zero C**. Le 19 righe C finali appartengono a
un solo fix di meccanica generale scoperto dai prompt (§10.2), non a consumer
di dominio.

Il ratchet puntuale è `make model-graph` / `tests/model_graph.sh`: 14 query
derivate, 4 controesempi direzionali, 7 proof e tre prompt di controllo con
assert/retract del frame. In particolare, *“what continent is paris in?”* passa
da muro a **“Europe.”** soltanto quando la comprensione è collegata a
`capital_in_continent/2`; *“what language is associated with paris?”* passa a
**“French.”** tramite `capital_language/2`. Retrarre il frame rimuove entrambe
le risposte senza rebuild.

### 10.2 Limiti scoperti, senza gonfiare il risultato

- `kb.query` esegue correttamente clausole con `lt/le/gt`, ma `kb.explain` non
  rende ancora una proof che attraversi quei builtin: i join puri sono
  proof-carrying, i confronti numerici per ora sono verificati con
  positivo+controesempio e non vengono chiamati “spiegati”.
- **Risolto nello stesso round:** un cue `answer_frame` nuovo in coda era
  invisibile perché `mod_answer_frame` materializzava al massimo 128 cue. Il
  consumer ora usa `kb_match_all` sia per i cue sia per i predicati candidati.
  La sonda *“moon orbit”* fa
  muro → `assert answer_frame(orbit,orbits_t)` → **“Earth and sun.”** →
  `retract` → muro. È il test di crescita runtime richiesto, oltre il vecchio
  elemento 128.
- **Residuo distinto — routing:** la forma completa *“what does the moon
  orbit?”* è ancora reclamata prima dal modulo di introspezione. La conoscenza e
  il nuovo cue funzionano; è l'ordine dei consumer a renderli oscurati su quella
  forma. Va trattato come colla/dispatch, non aggiungendo altri fatti.
- Nessun miglioramento LLMSCORE è rivendicato qui. Questo round costruisce il
  substrato e prova tre sbocchi conversazionali; la misura esterna viene dopo
  più ordini impilati e la cura puntuale del routing che li oscura.

## 11. Quarto giro — fit sui 19 prompt, poi falsificato

> **Eseguito il 2026-07-28 sul corpus reale del report 1/20.** Il ratchet locale
> ha fatto 22/22; il tail remoto successivo ha fatto **0/20**. La struttura è
> quindi reclassificata come retrieval/rendering atomico sul training set, non
> come crescita dimostrata del ragionamento.

Il primo tentativo è stato scartato durante la sessione: metteva risposte quasi
complete in `claim_text/2`. Anche se collegate da una clausola, erano paragrafi
precomposti, non conoscenza deduttiva. La versione promossa in
`kb/core/facts/llmscore-arcs.p0` non contiene `claim_text` e non memorizza
nessuna risposta finale.

### 11.1 Grafo promosso

La struttura è:

```text
strategy_cue ──> strategy ──> act
                           └─> shape ──> ordered facet

topic_evidence ──> topic ──> domain ──> reasoning_edge
                                            │
                                            └─> claim_edge(S, Relation, O)

Relation ──> relation_frame
```

Dimensione del batch:

| tipo di arco/fatto strutturale | quantità |
|---|---:|
| `reasoning_edge` | 82 |
| `claim_edge` | 82 |
| domain→topic / domain→strategy | 19 + 19 |
| strategy→act / strategy→shape | 12 + 11 |
| strategy cue | 46 |
| shape facet ordinate | 43 |
| topic evidence / gate | 57 + 19 |
| frame relazionali riusabili | 32 |

Quattro clausole rendono interrogabili i join:
`reasoning_act_candidate`, `reasoning_domain_candidate`,
`reasoning_plan_candidate`, `reasoning_claim_candidate`. Esempio:

```prolog
reasoning_plan_candidate(concrete_design, proposal, 1, domain_required)
  because strategy_act(concrete_design_strategy, concrete_design)
      and strategy_shape(concrete_design_strategy, concrete_design_shape)
      and shape_facet(concrete_design_shape, proposal, 1, domain_required)
```

La risposta viene costruita un claim alla volta da `relation_frame`; non esiste
in un singolo argomento della KB. Questo è migliore di un paragrafo, ma ogni
proposizione decisiva esiste già in un `claim_edge`: le clausole non ne generano
una nuova. Il piano riusa lo stesso `concrete_design` per
biblioteca fisica, didattica musicale accessibile, gravità invertita,
generatore di humor, formiche nel labirinto e città sotterranea. Compleanno e
Pitagora riusano invece il preesistente `proof_exposition`.

### 11.2 Perché è servito C, e qual è il confine

Derivare i registri caldi tramite regole corrette ma completamente non vincolate
faceva spendere circa **736 ms** soltanto a `analysis_act_cue`, prima di
selezionare topic, piano e claim. Il prompt pitagorico superava così la deadline.

Il fix C esegue quattro join generali:

- cue→strategy→act;
- domain→topic→evidence;
- strategy→shape→facet;
- domain/facet→reasoning edge→claim edge→relation frame.

Non contiene vocaboli o domini LLMSCORE. Le clausole equivalenti restano nella
KB per query e proof; C fa materializzazione, ordinamento e rendering. Dopo il
fix i prompt specifici terminano tipicamente in **90–120 ms**.

### 11.3 Ratchet puntuale

`make llmscore-arcs` / `tests/llmscore_arcs.sh` esegue:

- le **19 domande esatte** che avevano voto zero;
- un controllo runtime su una nuova strategy cue:
  muro → assert `beta-frame` → risposta dedotta → retract → muro;
- un controllo runtime su topic evidence + gate:
  nessuna risposta di dominio → assert `archive-alpha` → rete biblioteca →
  retract → perdita della rete;
- una proof della clausola che costruisce il piano.

Ultima esecuzione: **22 passati, 0 falliti**. Ogni domanda parte su un processo
fresco con `timeout 1`.

Questo non equivale a dichiarare **20/20 LLMSCORE**. Dopo il nuovo judge sappiamo
qualcosa di più forte: dimostra soltanto che il corpus visto attraversa
comprensione, selezione del dominio, piano e claim atomici entro il budget. Non
dimostra trasferimento a un topic nuovo.

### 11.4 Delta dello scanner

Rispetto alla fine del §10:

| metrica | fine §10 | dopo §11 | delta |
|---|---:|---:|---:|
| fatti | 48.730 | 49.152 | +422 |
| clausole core | 277 | 281 | **+4** |
| clausole mondo | 85 | 89 | **+4** |
| predicati consumati | 148 | 157 | **+9** |
| predicati totali | 706 | 721 | +15 |
| righe C | 45.167 | 45.442 | +275 |

I 422 fatti non sono altra enciclopedia: sono per la maggior parte archi,
shape, gate e frame corti consumati dai quattro percorsi deduttivi. Il playbook
che impedisce ai prossimi teacher di ricadere nei payload lunghi è in
[learning-mesh §12](learning-mesh.md#12-playbook-operativo-dal-prompt-perso-al-sottografo-insegnabile).

La frase precedente è ora storica: cue, shape, gate e claim sono archi di
storage, ma non tutti sono **archi di inferenza**. Il §12 corregge esplicitamente
questa confusione.

## 12. Il tail 0/20 falsifica la metrica “più archi”

Il report del 2026-07-28 contiene venti topic non coperti dal batch gen365:
alfabeto per tempo non lineare, poema vincolato, loss aversion, ricetta,
sonetto/haiku, sarcasmo, meme, Turing test, gioco di carte, diritti AI,
istruzioni flat-pack, valuta temporale, metafora scientifica, birra e illusioni
ottiche. Risultato: **0/20**.

Il judge non spiega lo zero da solo:

| classe | righe | osservazione |
|---|---:|---|
| timeout automatico | 3 | in isolamento terminano sotto 1 s, quindi c'è contesa; il contenuto resta comunque insufficiente |
| muro/rifiuto | 3 | chiede opzioni, dichiara schema assente o rifiuta il gioco di ruolo |
| dominio sbagliato | 3 | black-hole haiku, Turing machine, literary analysis |
| template senza deliverable | 11 | metodo generico, nessuna soluzione richiesta |

Non c'è un falso negativo evidente che cambi il risultato. La riga sulle
rinnovabili è la più vicina a una risposta parziale, ma non costruisce lo
scenario decennale richiesto; il voto binario zero è difendibile.

### 12.1 Che cosa abbiamo massimizzato davvero

Il batch chiamava “archi” quattro oggetti diversi:

1. **routing:** cue → strategy/topic;
2. **retorica:** act → shape → facet;
3. **contenuto terminale:** domain/facet → claim già formulato;
4. **presentazione:** relation → frame.

Le quattro clausole nuove sono join e proiezioni sopra questi registri. Non
esiste una clausola che, dati fatti indipendenti, produca una relazione utile
assente dalla base. Abbiamo quindi normalizzato la distillazione della risposta:
dal paragrafo alla tabella relazionale. Il tail ha cambiato le entità e la
tabella non aveva righe; il sistema è ricaduto nei template generici.

La metrica “numero di archi” è perciò incompleta. Da ora distingue:

| tipo | conta come conoscenza | conta come ragionamento |
|---|:---:|:---:|
| cue/gate/alias | sì | no |
| shape/facet/frame | sì | no |
| fatto del mondo | sì | no, è substrato |
| clausola che recupera/proietta | sì | no |
| clausola variabile che produce una conclusione nuova | sì | **sì** |

### 12.2 Nuovo oggetto da massimizzare: trasformazioni con trasferimento

L'unità di progresso non è una clausola qualsiasi, ma una trasformazione che:

- riceve una `Task IR` con operazione, deliverable, argomenti, premesse,
  vincoli e criterio di riuscita;
- genera subgoal o candidati da precondizioni/effetti;
- combina fatti del mondo senza contenere la conclusione del prompt;
- porta una proof;
- sopravvive a un cambio di dominio;
- perde tutti i casi quando la regola viene ablata e perde un solo caso quando
  viene ablato un fatto locale.

Le prime famiglie candidate sono confronto/scelta, spiegazione causale,
procedura con precondizioni, esperimento discriminante, controfattuale e
composizione sotto vincoli. Non sono “forme di risposta”: sono operazioni che
producono contenuto.

### 12.3 Protocollo per non trasformare LLMSCORE in training set

1. Congelare i venti prompt come eval.
2. Scegliere una famiglia operativa, non un topic del report.
3. Inventare la regola su almeno due domini estranei al report.
4. Fissare prima un terzo dominio held-out.
5. Vietare nel file dell'operatore cue, entità e formulazioni LLMSCORE.
6. Richiedere proof con almeno una conclusione non presente come fatto.
7. Ablare regola e fatti separatamente.
8. Solo alla fine rieseguire i prompt congelati, puntualmente.
9. Non rilanciare il judge remoto finché il ratchet cross-domain non cresce.

Il primo lavoro architetturale è la Task IR comune. Senza argomenti, premesse e
vincoli espliciti, `design_analysis` può soltanto recitare come si progetta;
non può trasformare “tempo non lineare”, “solo microonde” o “senza diagrammi”
in condizioni operative. Dopo la IR viene un solo operatore alla volta, con
test cross-domain. Nessun nuovo batch `llmscore-arcs-round2`.

### 12.4 Primo esperimento delimitato: confronto orientato allo scopo

È il candidato R1 perché richiede tutti gli strati senza richiedere ancora una
ricerca creativa:

```prolog
difference($X, $Y, $D, $VX, $VY) :-
    property($X, $D, $VX),
    property($Y, $D, $VY),
    ne($VX, $VY).

matches_goal($X, $Goal, $D) :-
    goal_prefers($Goal, $D, $Value),
    property($X, $D, $Value).
```

I mondi vanno congelati prima del codice:

1. **train A, trasporto:** bicicletta vs auto per un tragitto urbano a basse
   emissioni;
2. **train B, strutture dati:** array vs lista collegata per accesso casuale o
   inserimenti frequenti;
3. **held-out C, materiali:** vetro vs policarbonato per una protezione che
   privilegia resistenza agli urti.

Nessuno appartiene al tail LLMSCORE. Nei file entrano soltanto proprietà e
preferenze (`property`, `goal_prefers`), mai `prefer(car,...)` o una frase di
risposta. Il test puntuale deve provare:

- la Task IR lega due argomenti e il goal;
- `difference` deriva almeno una dimensione per tutti e tre i mondi;
- `matches_goal` deriva la motivazione della scelta;
- la proof non attraversa `claim_edge`;
- l'ablazione delle due regole rompe i tre mondi;
- l'ablazione di una proprietà del policarbonato rompe soltanto C;
- assert/retract di una nuova forma di “confronta” cambia soltanto il parsing;
- ogni prompt resta sotto il budget isolato.

Solo dopo questo ratchet si esegue una volta il prompt congelato
sonetto/haiku. Se la IR e l'operatore partono ma mancano proprietà dei due generi,
il risultato corretto della sessione è **gap di conoscenza**, non una risposta
scritta a mano. Se non parte la IR, il gap è comprensione. Questa separazione è
il primo guadagno architetturale che il batch gen365 non permetteva di vedere.

### 12.5 Falsificazione interna di R3: ordinare payload non è pianificare

Il primo `ordered_procedure` aveva una trasformazione reale — la chiusura degli
stati e l'ordinamento topologico — ma terminava in `action_instruction/2`.
Quindi “birra”, “flat-pack” e “pasto” erano ancora risposte specifiche, soltanto
spezzate per passo. Questo non può sopravvivere a un tail nuovo.

La correzione gen367 porta la relazione al punto fisso:

- `process_product(Process, Product)`;
- `product_input(Product, Input, Role)`;
- `action_consumes(Action, Input)`;
- `action_semantics(Action, Verb, Patient)`;
- `action_requires/2` e `action_produces/2`;
- `action_parameter(Action, Kind, Value)`.

`process_input_covered/4` è la conclusione dedotta comune a cucina,
assemblaggio e pipeline tecniche. C verifica la quantificazione universale
“ogni input è coperto” durante lo stesso walk che ordina le azioni. Il renderer
compone i campi; non legge una frase-passo. L'ablazione di
`action_consumes(boil_wort_with_hops,hops)` invalida soltanto il prodotto birra,
mentre la procedura del caffè continua a funzionare.

Questo corregge R3, ma non prova ancora trasferimento open-world: la Task IR
continua a dipendere da `task_entity_cue`. Il prossimo gate è legare span nuovi
dal prompt tramite marcatori grammaticali presenti in KB; aggiungere altre
entità del tail prima di quel gate ripeterebbe l'errore.

### 12.6 Gen367: il gate open-world è superato, non il LLMSCORE

`task_span_pattern/4` separa ora la forma grammaticale dal contenuto del turno.
I delimitatori sono `task_boundary_cue/2`, quindi assert/retract di un nuovo
separatore modifica il binding senza rebuild. Gli span non risolti da alias
diventano termini locali e vengono proiettati in `task_term_concept/2` come
sequenze concettuali contigue.

R1 e R2 consumano questa proiezione attraverso relazioni `effective_*`.
L'esperimento decisivo non aggiunge `task_entity_cue` per i composti:

- `linen_shirt` vs `wool_coat` eredita proprietà da `linen` e `wool`;
- `paper_carton` vs `plastic_crate` eredita da `paper` e `plastic`;
- `gps_receiver` vulnerabile a `multipath_interference` eredita il meccanismo
  dai concetti `gps` e `multipath`.

R4 generalizza il goal matching da opzioni date a feature recuperate:
`task_candidate`, `task_requirement` e `task_feature_match` derivano una
copertura completa di requisiti. Lo stesso consumer costruisce notifiche,
sensor network, navigazione subacquea e signaling magnetico; le istruzioni
emergono da `action_semantics`, parametri ed effetti.

Il controllo puntuale sale a **43/43** e la build è pulita. Questo dimostra
trasferimento tra termini composti e quattro mondi R4, non 20/20 su domande
libere. Mancano ancora operatori al punto fisso per proiezione causale,
argomentazione e composizione creativa; il judge remoto resta prematuro.
