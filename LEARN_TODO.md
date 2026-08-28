# LEARN_TODO — la coda dei temi da apprendere

Coda dei temi da far apprendere a parrot0 con il protocollo di
[`LEARN_PROTOCOL.md`](LEARN_PROTOCOL.md). Ogni voce è un'unità di lavoro: si
apre una sessione, si eseguono i gate, si committa e si pusha l'incremento.

**Non è una lista della spesa di fatti.** Un tema entra qui solo se chiude una
*classe* di fallimenti, e ogni voce dichiara la classe che chiude. Il criterio è
quello del §8 di [`docs/plans/apprendimento-assistito.md`](docs/plans/apprendimento-assistito.md):
*preferire il gap che, chiuso, libera più famiglie di frasi.*

## Come si usa

1. Si prende la voce più alta non ancora fatta nella sezione applicabile.
2. Si esegue `LEARN_PROTOCOL.md` per intero — baseline, lezione, replay,
   transfer, contrasto, composizione, ablation, quarantena, `/save`, verifica in
   processo nuovo, report, commit, push.
3. Se la lezione **non è insegnabile parlando**, ci si ferma: è un risultato
   diagnostico, si tipizza il gap e si registra qui. Non si scrive il fatto a
   mano.
4. Se un tema è già stato fatto ma il comportamento è regredito, si **riapprende**
   — la voce torna in coda con la nota del perché.

Le tre regole che non si negoziano, dal protocollo: conoscenza **vera** e
fontata; nessuna API travestita; `X = 0` prima di `/save`.

---

## P0 — Le cose che parrot0 non può sbagliare

Sono i temi che rendono parrot0 *non credibile* se falliscono, a prescindere da
quanto sia bravo altrove. Hanno la precedenza su tutto.

| # | Tema | Classe che chiude | Stato |
|---|---|---|---|
| P0.1 | **Una risposta è nella lingua della domanda** — sempre, muri e messaggi di errore compresi | il muro inglese in chat italiana | **parziale**: marcatori e pareggio risolti (`012e034`); resta l'output misto — `reflexive_skeleton.it` produce «aldric is coraggioso» dentro una frase inglese |
| P0.2 | **Hello world e i primi snippet**, in ogni lingua naturale e nei linguaggi principali | «non so scrivere il programma più semplice del mondo» | **fatto** per python e c (`012e034`) — estendere a shell, javascript, sql |
| P0.3 | **Le operazioni aritmetiche e le loro parole** | `what is 2 plus 2?` era rotto da gen443 | **fatto** (`1f9f3d9`) — la classe ora si deriva da `infix_operator` |
| P0.4 | **Un muro è un muro, non una battuta sociale** | quattro input reali ricevevano riempitivo smalltalk | **aperto** — è M0, il prerequisito di tutto il resto |
| P0.5 | **Una domanda non diventa mai un fatto** | `what causes inflation?` → `causes(what, inflation)` | **fatto per il causale** (`6c898e5`) — serve la guardia *generale*, non una per modulo |
| P0.6 | **Le unità di misura e le conversioni** — metri/piedi, celsius/fahrenheit, kg/libbre | le conversioni sono la domanda più banale che un assistente riceve | **da fare** |
| P0.7 | **Date e tempo** — che giorno è, quanti giorni fra due date, giorni del mese | idem | **da fare** |

## P1 — Il metalinguaggio: ciò che sblocca l'insegnabilità

Non sono domini: sono le *forme* con cui si insegnano tutti i domini. Chiuderne
una vale più di dieci sessioni di fatti. Riferimento: M0–M14 del piano.

| # | Tema | Classe che chiude | Stato |
|---|---|---|---|
| P1.1 | **Dire di che tipo è la propria lacuna** (M13) | oggi il registro delle lacune è *scritto e muto*: `gap_kind`, `gap_opaque`, `gap_anchor` esistono a ogni turno e non c'è modo di chiederli | **aperto — la voce con il miglior rapporto valore/rischio**: i dati ci sono già, manca il consumer |
| P1.2 | **Ricongiungere le rappresentazioni** (M8) | `igneous_rock(basalt)` non soddisfa `igneous(X), rock(X)` — ogni classe composta è oggi un'isola | **aperto**: metà della causa comune con M3 è caduta, il bersaglio congiuntivo no |
| P1.3 | **Nominare i ruoli** (M4) | «qui il primo nome è chi agisce» — la frase con cui il piano si apre, e che parrot0 non sa ricevere | **aperto** |
| P1.4 | **La forma flessa e quella base sono lo stesso verbo** (M5) | `filter`/`filters`, `studies`/`study` sono due abilità scollegate **in ingresso** (in uscita il ponte c'è, `lemma_candidate/2`) | **aperto**, misurato nella sessione fisiologia |
| P1.5 | **Rifiutare un verbo che collide** | insegnare `needs` produceva un fatto falso perché `requires` è del modulo `plan`; oggi lo deve verificare il teacher con `grep` | **aperto** |
| P1.6 | **Ritrattare parlando un fatto binario** | `forget` copre la classe unaria e le costruzioni, non una relazione: un errore binario è irreversibile, e ha già costretto a buttare due sessioni | **aperto** |
| P1.7 | **Negazione, condizionale, quantificatore** (M7) | «nessun minerale è più tenero del talco» va a muro | **aperto** |
| P1.8 | **Il pronome che attraversa i turni** (M7) | «esso», «it», l'ellissi | **aperto** |
| P1.9 | **Causa, finalità, processo e condizione nella prosa** (M9) | «X perché Y» viene assorbito dallo smalltalk | **aperto** |
| P1.10 | **Procedure insegnate che non si eseguono** (M10) | «per identificare un minerale, prima…» va a muro | **aperto** |
| P1.11 | **Catena di costruzioni e induzione dagli esempi** (M3, resto) | l'arità è caduta, la catena no | **aperto** |

## P2 — I domini che allargano il mondo

Conoscenza vera, fontata, che serve a comporre. L'ordine è per quanto ciascuno
si compone con quello che c'è già.

| # | Tema | Perché | Stato |
|---|---|---|---|
| P2.1 | Geografia fisica e politica — capitali, fiumi, confini, fusi orari | è il dominio con più composizione possibile per fatto | da fare |
| P2.2 | Cronologia storica — eventi con anno, luogo, attori, cause | il mantra #13 chiede *event frame* interrogabili, non fatti sparsi | da fare |
| P2.3 | Chimica — elementi, gruppi, reazioni, stati | classi composte ovunque: ottimo banco per M8 | da fare |
| P2.4 | Biologia — tassonomia, organi, processi | la fisiologia è iniziata (`86d2126`), la tassonomia no | parziale |
| P2.5 | Matematica — definizioni, proprietà, controesempi | «un controesempio confuta un'affermazione universale» c'è; le proprietà no | parziale |
| P2.6 | Informatica — strutture dati, complessità, protocolli | serve alle abilità di coding di P3 | da fare |
| P2.7 | Diritto di base — contratto, obbligazione, nullità | muro totale nella sonda; dominio ad alta struttura | da fare |
| P2.8 | Economia — inflazione, tassi, offerta e domanda | metà già c'è, le relazioni causali no | parziale |
| P2.9 | Musica — forme, strumenti, notazione | muro totale nella sonda | da fare |
| P2.10 | Cucina e misure pratiche | è ciò per cui un assistente viene usato davvero | da fare |

## P3 — Le abilità da agente

Non conoscenza del mondo ma cose che parrot0 deve *saper fare*.

| # | Tema | Stato |
|---|---|---|
| P3.1 | Snippet per linguaggio: shell, javascript, sql, oltre python e c di P0.2 | da fare |
| P3.2 | Spiegare un frammento di codice riga per riga | parziale |
| P3.3 | Leggere un errore di compilazione e dire cosa manca | da fare |
| P3.4 | Formati richiesti — elenco, tabella, una frase, tre modi (mantra #11) | parziale |
| P3.5 | Riassumere conservando le proposizioni, non le parole | parziale |
| P3.6 | Tradurre conservando i ruoli, non parola per parola | parziale |
| P3.7 | Dire *quanto* è sicuro e perché, senza inventare una percentuale | da fare |

## P4 — Da riapprendere

Temi già passati che sono regrediti, o che erano stati chiusi su esempi troppo
vicini all'implementazione (§6.3: un gate verde sugli esempi che l'hanno guidato
non chiude lo strato).

| # | Tema | Perché torna in coda |
|---|---|---|
| P4.1 | L'annuncio di una regola appresa | il passaggio delle frasi alla KB aveva perso la testa: «every cat is a pet» rispondeva «cat(X).» (`210dd01`) |
| P4.2 | Le parole degli operatori aritmetici | perse nel passaggio dell'elenco dal C alla KB (`1f9f3d9`) |
| P4.3 | Il messaggio di `forget` | formattato e mai emesso; e comunque coperto da `answerframe` che rivendica il turno |
| P4.4 | La conversazione lunga in italiano | `frontier_chat_audit.it` misurava una KB amputata: 31 casi su 56 rossi per costruzione (`830bc59`) |
| P4.5 | Ogni tema che una migrazione KB-first tocca | le tre regressioni sopra hanno la stessa forma — la lista passa alla conoscenza e la *chiave di lettura* resta indietro. Dopo ogni migrazione, si riapprende il tema che tocca |

---

## Nota di metodo, dalle sessioni fatte

Cinque cose imparate sul campo, che valgono per ogni voce di questa coda:

1. **Il dominio si sceglie interrogando, non a tavolino.** La sessione di
   fisiologia è nata da sei muri consecutivi trovati parlando.
2. **Si verifica il verbo prima di usarlo.** `grep` su `src/` e `kb/`: un verbo
   già posseduto dal motore produce un fatto falso, non un muro (P1.5).
3. **Una sessione con anche un solo fatto falso si butta.** È già successo due
   volte, ed è meno costoso di quanto sembri: le lezioni si ridanno in minuti.
4. **La ricaduta va svuotata a ogni sessione.** Se `kb/learning/learned.p0`
   cresce, manca una casa: si crea la categoria, non si ordina il file.
5. **Un test rosso non si aggiusta cambiando l'attesa.** Delle cinquanta
   asserzioni che chiedevano «Learned rule: …» avevano ragione loro. Prima si
   capisce *chi* ha torto fra il test e il codice.
