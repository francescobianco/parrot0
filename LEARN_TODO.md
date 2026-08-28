# LEARN_TODO — la coda dei temi da apprendere

Coda dei temi da far apprendere a parrot0 con il protocollo di
[`LEARN_PROTOCOL.md`](LEARN_PROTOCOL.md). Ogni voce è un'unità di lavoro: si
apre una sessione, si eseguono i gate, si committa e si pusha l'incremento.

**Non è una lista della spesa di fatti.** Un tema entra qui solo se chiude una
*classe* di fallimenti, e ogni voce dichiara la classe che chiude. Il criterio è
quello del §8 di [`docs/plans/apprendimento-assistito.md`](docs/plans/apprendimento-assistito.md):
*preferire il gap che, chiuso, libera più famiglie di frasi.*

## Da dove si pescano i temi

Oltre a questa coda, il serbatoio è [`docs/llmscores/`](docs/llmscores/): **531
prompt** estratti dalle 48 revisioni storiche di `LLMSCORE.md` e dalle sonde
tematiche, di cui **315 sono muri** — prompt su cui parrot0 ha preso 0 da un
giudice esterno — divisi in venti file per tema.

È materiale già filtrato dalla realtà: non prompt inventati da noi, ma
fallimenti veri davanti a un giudizio esterno. Quando una voce di questa coda è
chiusa e non si sa da dove ripartire, si apre il file del tema più promettente e
si prende una **famiglia** di muri, mai un prompt solo — chiudere l'istanza
senza chiudere la classe non conta come progresso.

I prompt marcati «già vinti» in quei file servono al verso opposto: se uno torna
a murare, è un tema da riapprendere e va in P4.

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

## Missione attiva 2026-08-29 — dalla lettura alla supercomprensione

Questa e' la coda operativa delle prossime sessioni. Nasce dal confronto fra
[`apprendimento-assistito.md`](docs/plans/apprendimento-assistito.md) e il
fronte K0-K11 di
[`frontier-kb-natural-dialogue.md`](docs/plans/frontier-kb-natural-dialogue.md):
parrot0 possiede fatti, scope, proof, situazioni e piani, ma non possiede ancora
un oggetto comune per **cio' che un documento sta facendo**. Una pagina
scientifica non e' una sequenza di fatti: propone una domanda, delimita uno
scope, adotta assunzioni, descrive un metodo, riporta osservazioni, argomenta una
conclusione, qualifica un limite e lascia problemi aperti.

La missione non autorizza una sessione lunga prima del cancello M0-M14. Ogni
voce viene consumata con una sonda breve del `LEARN_PROTOCOL`; se la lezione non
e' indirizzabile parlando, l'esito e' `diagnostic`, si promuove soltanto il
motore metacognitivo generale che manca e il protocollo riparte in un processo
nuovo. Le fonti devono essere primarie o istituzionali e il testo insegnato
viene parafrasato: una fonte non diventa un corpus copiato nella KB.

### Ordine di consumo

| # | Piano complesso | Classe liberata | Interazione didattica minima | Gate duro | Stato |
|---|---|---|---|---|---|
| **SC0** | **Baseline stratificata su prosa narrativa, espositiva e scientifica** | distingue muro, fatto estratto, misclaim, perdita di subordinata e perdita di struttura documentale | presentare tre brani veri brevi, poi chiedere tesi, supporto, sequenza e limite senza anticipare le risposte | transcript classificato frase per frase; zero conoscenza consolidata; mappa M0-M20 e SC1-SC16 | **IN CORSO** |
| **SC1** | **Unita' documentali e relazioni retoriche** | una frase o sezione puo' essere definizione, sfondo, contrasto, causa, metodo, risultato, limite o transizione | «qui la seconda frase contrasta la prima», «questa frase descrive il metodo, non il risultato» | una cue retorica nuova insegnata a voce cambia la segmentazione; transfer su tre domini; retract la rimuove | aperto |
| **SC2** | **Claim tipati, attribuzione e forza epistemica** | separa osservazione, dato, inferenza, ipotesi, assunzione, definizione, citazione e raccomandazione | «gli autori osservano X ma concludono Y», «Z e' un'ipotesi, non un risultato» | nessun claim perde fonte, scope o status; un fatto riportato non diventa automaticamente commitment di parrot0 | aperto |
| **SC3** | **Grafo argomentativo e dipendenze della conclusione** | estrae premesse, conclusioni, supporti, obiezioni, qualificatori e rebuttal da prosa articolata | «questa osservazione sostiene la conclusione solo insieme a quest'altra premessa» | domande `perche'`, `da cosa dipende`, `cosa la confuterebbe`; ablation di una premessa ritira solo le conclusioni dipendenti | aperto |
| **SC4** | **Ricostruzione del disegno scientifico** | riconosce domanda, popolazione/sistema, variabili, intervento, confronto, misura, controllo e confondenti | «il gruppo B e' il confronto; la temperatura e' mantenuta costante» | ricostruzione su esperimento, studio osservazionale e simulazione; non inventa controllo o causalita' | aperto |
| **SC5** | **Da metodo in prosa a procedura eseguibile e ispezionabile** | compila passi, input, output, precondizioni, invarianti, branch, criterio d'arresto, rischi e provenance | «per eseguire il metodo, prima calibra; ripeti finche'...; scarta se...» | la lezione non viene eseguita; piano su input nuovi; ogni numero ha ruolo e unita'; trace dei passi; retract parlato | aperto |
| **SC6** | **Modello causale dal testo** | distingue correlazione, meccanismo proposto, causa necessaria/sufficiente, mediatore e confondente | «X e Y covariano; il testo non dice ancora che X causa Y» | controfattuale e intervento coerenti; un'associazione non produce un arco causale senza evidenza dichiarata | aperto |
| **SC7** | **Modellizzazione nello spazio logico** | costruisce mondi/modelli compatibili, vincoli, invarianti, conseguenze, controesempi e residui | «cerca un caso in cui le premesse valgono e la conclusione no» | entailment tramite assenza di contromodello entro budget; se la ricerca e' incompleta lo dichiara; transfer fra logica, scienza e procedure | aperto |
| **SC8** | **Scope complesso, quantificazione e modalita'** | comprende `tutti`, `alcuni`, `nessuno`, `solo se`, `a meno che`, possibilita', necessita' e negazione annidata | «qui `solo se` introduce una condizione necessaria, non sufficiente» | coppie minime positive/negative; nessuna inversione per somiglianza superficiale; nuova forma modale assert/retract | aperto |
| **SC9** | **Coreferenza documentale e identita' attraverso paragrafi** | collega pronomi, ellissi, abbreviazioni, nomi alternativi, campioni e variabili senza fondere entita' diverse | «in questo paragrafo `esso` riprende il campione, mentre `questo risultato` riprende l'osservazione» | catene a 20+ frasi; ambiguita' conservata; correzione locale ri-deriva soltanto i legami dipendenti | aperto |
| **SC10** | **Equazioni, unita', tabelle e figure come proposizioni** | collega simboli, definizioni, dimensioni, assi, righe e didascalie al testo che li interpreta | «qui k e' una costante di velocita', non una misura osservata» | controllo dimensionale; domanda incrociata testo-tabella-equazione; simbolo nuovo insegnato senza nome interno | aperto |
| **SC11** | **Sintesi compressiva con copertura semantica** | produce un nucleo minimo da cui le proposizioni importanti restano derivabili | «riassumi conservando domanda, metodo, risultato, limite e nesso fra loro» | claim coverage, nessun claim nuovo, proof verso le unita' fonte, versione breve/lunga dalla stessa struttura | aperto |
| **SC12** | **Integrazione fra documenti e gestione del dissenso** | allinea concetti e risultati fra fonti senza cancellare scope, metodi e popolazioni diverse | «questi lavori sembrano divergere, ma usano misure e condizioni diverse» | accordo, conflitto reale e apparente distinti; ogni sintesi conserva fonte e contesto; nessuna media di confidence inventata | aperto |
| **SC13** | **Generazione di ipotesi e falsificatori** | propone spiegazioni candidate da residui, analogie strutturali e modelli concorrenti | «proponi due meccanismi compatibili e l'osservazione che li distinguerebbe» | ipotesi marcate, non promosse a fatti; previsione discriminante; controllo negativo; ablation dell'analogia toglie solo l'ipotesi | aperto |
| **SC14** | **Lettore metacognitivo attivo** | sceglie se rileggere, definire, risolvere un riferimento, cercare una premessa, chiedere o sospendere | «non ti manca il fatto: non sai a cosa si riferisce `questo`» | gap corretto su batteria eterogenea; domanda di chiarimento cambia davvero la decisione; nessun menu fisso di remediation | aperto |
| **SC15** | **Induzione metacostruttiva da esempi e controesempi** | induce una costruzione o un operatore senza che il teacher nomini lo schema interno | mostrare tre esempi veri e un contrasto, poi chiedere che cosa hanno in comune | candidato in quarantena; transfer 3/3; contrasto; descrizione naturale dei ruoli; retract; nessun predicato suggerito dal teacher | aperto |
| **SC16** | **Comprensione ricorsiva e critica di un intero articolo** | compone SC1-SC15: mappa, interroga, esegue procedure, critica limiti e aggiorna il modello | articolo open-access mai usato nel training, sezioni presentate progressivamente | almeno 70% held-out; tesi/metodo/risultati/limiti completi; domande avversariali; nessuna risposta oltre la proof; fresh-process recall 100% sui fatti promossi | aperto |

### Pacchetti di sessione

Le righe sopra non si affrontano in parallelo: ciascun pacchetto produce il
prerequisito del successivo.

1. **Pacchetto A — sensore (SC0, SC1, SC2).** Tre testi corti, una sola nuova
   relazione retorica per ciclo, classificazione completa dei misclaim. Uscita:
   Document IR minimo oppure referto del meta-gap che ne impedisce la nascita.
2. **Pacchetto B — ragione del testo (SC3, SC4, SC6).** Ricostruire un argomento
   e un disegno sperimentale; separare evidenza, inferenza e causalita'. Uscita:
   grafo interrogabile con provenance e ablation causale.
3. **Pacchetto C — fare e simulare (SC5, SC7, SC8, SC10).** Compilare una
   procedura, verificarla nello spazio dei modelli e legare simboli/unita'.
   Uscita: piano eseguibile che sa anche quando non e' giustificato.
4. **Pacchetto D — mantenere e comprimere (SC9, SC11, SC12).** Attraversare
   paragrafi e fonti, poi comprimere senza amputare tesi o limiti. Uscita:
   sintesi proposizionale reversibile verso le fonti.
5. **Pacchetto E — superare il teacher (SC13, SC14, SC15).** Usare i residui
   per scegliere la prossima domanda, indurre candidati e proporre
   falsificatori. Uscita: crescita guidata dall'informazione, non da un menu.
6. **Pacchetto F — integrazione (SC16).** Articolo held-out e replay ostile.
   Uscita: report del protocollo, non una dichiarazione impressionistica di
   comprensione.

### Metriche aggiuntive per questa missione

Le metriche del protocollo restano obbligatorie. Per la prosa complessa si
aggiungono:

- **Claim coverage:** claim corretti e collegati / claim rilevanti nella fonte;
- **Relation fidelity:** archi argomentativi corretti / archi estratti;
- **Source fidelity:** proposizioni con scope e provenance corretti /
  proposizioni affermate;
- **Procedure executability:** passi applicabili e tipati / passi estratti;
- **Countermodel yield:** conclusioni universali respinte da un controesempio
  valido / conclusioni universali false provate;
- **Compression with recovery:** claim recuperabili dalla sintesi / claim
  rilevanti prima della sintesi;
- **Diagnostic addressability:** gap per cui parrot0 formula la domanda
  didattica corretta / gap osservati;
- **Cross-genre transfer:** operatori che passano da prosa scientifica a
  normativa, tecnica e narrativa senza nuovo C.

Una risposta scorrevole non contribuisce a nessuna di queste metriche se il
grafo che la sostiene non e' interrogabile.

---

## P0 — Le cose che parrot0 non può sbagliare

Sono i temi che rendono parrot0 *non credibile* se falliscono, a prescindere da
quanto sia bravo altrove. Hanno la precedenza su tutto.

| # | Tema | Classe che chiude | Stato |
|---|---|---|---|
| **P0.0** | **Scegliere fra due alternative date nel turno, in base a un effetto** — con le alternative che sono *codice*, non numeri | «quale di questi due X fa Y, A o B?» — la forma con cui si chiede un confronto, in qualunque dominio | **APERTA, prioritaria** (F., 2026-08-28). Vedi l'analisi qui sotto |
| P0.1 | **Una risposta è nella lingua della domanda** — sempre, muri e messaggi di errore compresi | il muro inglese in chat italiana | **parziale**: marcatori e pareggio risolti (`012e034`); resta l'output misto — `reflexive_skeleton.it` produce «aldric is coraggioso» dentro una frase inglese. **Gen490**: la *domanda* italiana è ora insegnabile con una parola (vedi P1.12), la *risposta* no — 141 famiglie `response_template` su 854 hanno una forma italiana |
| P0.2 | **Hello world e i primi snippet**, in ogni lingua naturale e nei linguaggi principali | «non so scrivere il programma più semplice del mondo» | **fatto**: python, c, shell, javascript, sql (`012e034`, `3ecfceb`) — i tre in più sono costati *zero C*, che è la prova che il meccanismo è KB-first |
| P0.3 | **Le operazioni aritmetiche e le loro parole** | `what is 2 plus 2?` era rotto da gen443 | **fatto** (`1f9f3d9`) — la classe ora si deriva da `infix_operator` |
| P0.4 | **Un muro è un muro, non una battuta sociale** | quattro input reali ricevevano riempitivo smalltalk | **aperto** — è M0, il prerequisito di tutto il resto |
| P0.5 | **Una domanda non diventa mai un fatto** | `what causes X?`, `what requires X?`, `what is the same as X?`, `what has N X?` finivano in KB come fatti falsi | **fatto**: la regola è ora UNA (`p0_turn_opens_as_question`) applicata ai quattro rami di asserzione (`6c898e5`, questo giro). Da sorvegliare a ogni ramo nuovo |
| P0.6 | **Le unità di misura e le conversioni** | le conversioni sono la domanda più banale che un assistente riceve | **fatto** per il sistema SI (`d8020b3`): 8 fatti, replay 4/4. Restano le conversioni fra sistemi — piedi, libbre, fahrenheit — che chiedono un *calcolo*, non una tabella |
| P0.7 | **Date e tempo** | idem | **parziale**: dodici mesi su dodici, decade, secolo e anno bisestile (`d8020b3` + questo giro). Resta l'**aritmetica fra date** — quanti giorni fra due date, che giorno cade il — che chiede un calcolo, non una tabella |

### P0.0 — l'analisi, misurata al `gen459`

Il turno segnalato da F.:

```
you> quale di questi due codici aumenta la variabile i++ o i--
Hmm, I don't know about questi yet. Want me to learn about it? …
```

Non è un tema di dominio: è **una forma di domanda che parrot0 non sa ricevere**,
e il dominio (il C) è solo il campione. Misurato:

| turno | esito |
|---|---|
| `which is bigger 3 or 5` | **5.** — la forma funziona sui numeri |
| `which of these increases the variable i++ or i--` | muro |
| `what does i++ do` | muro |
| `does i++ increase i` | muro, e nomina «increase» |

Quindi la classe si spacca in tre pezzi, e vanno chiusi in quest'ordine perché
ognuno è prerequisito del successivo:

1. **Il confronto fra alternative date nel turno esiste già, ma solo per i
   numeri e solo per proprietà che sono relazioni note** (`bigger`). Qui la
   proprietà è un *effetto* («aumenta») e i termini non sono valori ma
   **frammenti di codice**. È la stessa forma — «quale fra A e B ha la proprietà
   P?» — con i tre slot riempiti diversamente.
2. **La semantica degli operatori manca del tutto.** `what does i++ do` è un
   muro: non c'è conoscenza di che cosa faccia un operatore, in nessun
   linguaggio. Va insegnata come conoscenza — `i++` aumenta di uno, `i--`
   diminuisce di uno — non cablata.
3. **Il muro italiano nomina la parola sbagliata.** Indica `questi`, che è un
   dimostrativo: una parola funzione, non l'argomento del turno. È esattamente
   il fallimento che `99-registry.c` ha già annotato al gen384 — il declino che
   finiva per nominare «facciamo», «chiamo», «allora» — e che lì fu contenuto
   spegnendo la nomina dove il vocabolario non è completo. In italiano la
   guardia non basta: `questi` **è** nel lessico, quindi passa il filtro e viene
   nominato. Un declino che indica un dimostrativo non aiuta chi insegna, lo
   svia.

**Perché è P0 e non P2.** Chiedere «quale dei due fa X» è una delle forme più
comuni che un assistente riceve, in qualunque dominio: due farmaci, due
algoritmi, due città. Chiuderla su `i++`/`i--` non vale niente; chiuderla come
*forma* libera tutte le famiglie che hanno quella struttura — ed è il criterio
del §8 di `apprendimento-assistito.md`, «preferire il gap che, chiuso, libera più
famiglie di frasi».

**Nota di metodo, obbligatoria per questa voce.** Il punto 2 tenta al fatto
scritto a mano in `kb/experts/programming/`. Non si fa: il vincolo #3 del
protocollo vale anche qui, e per giunta questa voce serve proprio a misurare se
la semantica di un operatore è **insegnabile parlando**. Se non lo è, il
risultato della sessione è quel gap tipizzato, non un file toccato — e va in
`docs/plans/apprendimento-assistito.md` §6.2b accanto agli altri.

## P1 — Il metalinguaggio: ciò che sblocca l'insegnabilità

Non sono domini: sono le *forme* con cui si insegnano tutti i domini. Chiuderne
una vale più di dieci sessioni di fatti. Riferimento: M0–M14 del piano.

| # | Tema | Classe che chiude | Stato |
|---|---|---|---|
| P1.1 | **Dire di che tipo è la propria lacuna** (M13) | il registro era *scritto e muto* | **fatto** (`837a044`): referto con frase, ancore, opachi e specie; filtrato sulla conversazione corrente, citando le parole dell'utente; la cue cresce a runtime. `tests/p0t/meta/gap_report.p0t` 13/13 |
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
| P1.12 | **Il gloss di una parola** (M15/M16, il punto fisso) | una domanda italiana non è una superficie nuova, è la *stessa* superficie sotto una traduzione: `tr/2` sta a monte di ogni modulo | **fatto, parziale** (`gen490`): «usa is the italian of used» apre la famiglia intera; transfer 2/3, ablation verde. Restano la morfologia del gloss (M5) e `tr/2` binario — il punto fisso è `translation(Lingua, …)`. Vedi [il referto](docs/labs/apprendimento-assistito/2026-08-29-gloss-e-forma-della-domanda.md) |
| P1.13 | **Insegnare COME SI CHIEDE senza nominare la relazione** (M15, residuo) | il gen457 chiedeva `… as a way to ask side_color`, cioè il nome interno del predicato — il vincolo zero violato | **fatto** (`gen490`): si àncora a una domanda che già funziona, relazione e verso si deducono dal modello; guardia conclusiva contro il modello inesistente |

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
| P3.1 | Snippet per linguaggio: shell, javascript, sql, oltre python e c di P0.2 | **fatto** (`3ecfceb`) |
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
