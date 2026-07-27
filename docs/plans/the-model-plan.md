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
