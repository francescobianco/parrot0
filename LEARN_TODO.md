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
| **SC0** | **Baseline stratificata su prosa narrativa, espositiva e scientifica** | distingue muro, fatto estratto, misclaim, perdita di subordinata e perdita di struttura documentale | presentare tre brani veri brevi, poi chiedere tesi, supporto, sequenza e limite senza anticipare le risposte | transcript classificato frase per frase; zero conoscenza consolidata; mappa M0-M20 e SC1-SC16 | **CHIUSA come diagnosi** — 2026-08-29: falso racconto e falsa autodiagnosi isolati; porta di lettura resa insegnabile; route Transfer@3 = 3/3; estrazione complessa = 0/9. Report: [`2026-08-29-supercomprensione-sc0.md`](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc0.md) |
| **SC1** | **Unita' documentali e relazioni retoriche** | una frase o sezione puo' essere definizione, sfondo, contrasto, causa, metodo, risultato, limite o transizione | «qui la seconda frase contrasta la prima», «questa frase descrive il metodo, non il risultato» | una cue retorica nuova insegnata a voce cambia la segmentazione; transfer su tre domini; retract la rimuove | **SC1-A CHIUSA; SC1-B aperta** — 2026-08-29: unita' ordinate e span stabili di sessione; cue naturale persistita; `Transfer@3=3/3`; ablation/reteach causali; restano ID fonte, atti, cue multi-parola e archi intra-periodo. [Report SC1](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc1.md) |
| **SC2** | **Claim tipati, attribuzione e forza epistemica** | separa osservazione, dato, inferenza, ipotesi, assunzione, definizione, citazione e raccomandazione | «gli autori osservano X ma concludono Y», «Z e' un'ipotesi, non un risultato» | nessun claim perde fonte, scope o status; un fatto riportato non diventa automaticamente commitment di parrot0 | **SC2-A e SC2-B CHIUSE; SC2-C aperta** — 2026-08-29. SC2-A: documenti source-addressed, claim di superficie con span, attribuzione, status/context/commitment derivati ([report](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc2.md)). SC2-B: normalizzazione come **fase pura e contestuale**, `NormalizedClaimCoverage 0/8 -> 9/9`, `QuestionAnswerCoverage 0/3 -> 3/3`, `WorldCommitLeak=0`, letture parziali rifiutate per policy ([report](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc2b.md)). Resta l'equivalenza fra frame, la modalita' e la prova pronunciata |
| **SC3** | **Grafo argomentativo e dipendenze della conclusione** | estrae premesse, conclusioni, supporti, obiezioni, qualificatori e rebuttal da prosa articolata | «questa osservazione sostiene la conclusione solo insieme a quest'altra premessa» | domande `perche'`, `da cosa dipende`, `cosa la confuterebbe`; ablation di una premessa ritira solo le conclusioni dipendenti | **SC3-A CHIUSA; SC3-B aperta** — 2026-08-29: arco di supporto fra claim (nessun terzo estrattore), supporto come vista VIVA, `unsupported_conclusion/1` dichiarata invece che taciuta, connettivo avversativo != argomento, due ablation che spengono due cose diverse, forma interrogativa che cresce parlando. Restano **supporti congiunti** (gate 3 del §18.15, vedi D23), obiezioni/qualificatori/rebuttal, «cosa la confuterebbe» e la direzione `ground`. [Report SC3](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc3.md) |
| **SC27** | **Un supporto congiunto non e' due supporti** | D23 | «A e B insieme sostengono C» non deve diventare due archi indipendenti; togliere una parte di una premessa congiunta spegne la conclusione intera | «therefore e' un marcatore di supporto congiunto» | congiunto: ritrarre una qualunque premessa porta la conclusione a `lost`; senza modo dichiarato l'ambiguita' si conserva e si dice | **CHIUSA — 2026-08-30.** Il difetto era peggiore del previsto: con tre unita' la prima premessa **spariva in silenzio** (l'arco retorico e' fra unita' adiacenti) e parrot0 rispondeva «poggia su B» con la stessa sicurezza. Ora tutte le premesse sono visibili e la risposta le nomina tutte; il modo si insegna e, se non e' dichiarato, lo stato e' `undetermined_mode` invece di una scelta. Limite dichiarato: su un documento lungo la lettura sovra-raccoglie, che e' l'errore giusto da fare qui |
| **SC28** | **La citazione e' esente dalla lingua che la contiene** | D24 | cio' che e' menzionato non partecipa alla lingua che lo menziona; una lezione quotata entra nella KB byte per byte | «"what is that based on" e' una forma con cui ti si chiede su cosa poggia» | dieci locuzioni quotate con copule, articoli, dimostrativi e ausiliari entrano esatte e si ritrattano con la stessa superficie; una perdita e' un errore dichiarato, non una riga da confrontare a occhio | **aperto.** Misurato: `"what is that based on"` entra come `claim_support_question("what based on")`. `canonicalization_exempt(mention)` esiste in `kb/core/input.p0` e non raggiunge questo percorso |
| **SC4** | **Ricostruzione del disegno scientifico** | riconosce domanda, popolazione/sistema, variabili, intervento, confronto, misura, controllo e confondenti | «il gruppo B e' il confronto; la temperatura e' mantenuta costante» | ricostruzione su esperimento, studio osservazionale e simulazione; non inventa controllo o causalita' | aperto |
| **SC5** | **Da metodo in prosa a procedura eseguibile e ispezionabile** | compila passi, input, output, precondizioni, invarianti, branch, criterio d'arresto, rischi e provenance | «"we then" e' un marcatore di passo»; «"before this" e' un marcatore di precondizione» | la lezione non viene eseguita; ogni passo ha provenienza; una procedura con un passo non legato si dichiara bloccata | **SC5-A CHIUSA; SC5-B aperta** — 2026-08-30: un passo di metodo e' una claim riportata con status `described` e contesto `reported_method`, quindi **nessun secondo estrattore** — il metodo e' una vista sulle claim, ordinata dall'ordine delle unita'. Soggetto eliso preso dall'agente attribuito (`elided_subject/2` + `agent_surface/2`). D29 applicata al saper fare: un passo non legato blocca la procedura e resta visibile. Restano input/output tipati, invarianti, branch, rischi e l'esecuzione. [Ratchet](tests/p0t/language/document_method.p0t) |
| **SC37** | **Una cue di ripetizione indipendente dal criterio d'arresto** | D29 | «questo si ripete» e «finche' X» sono due letture diverse: oggi la stessa cue porta entrambe, quindi ritrarre l'arresto toglie anche il ciclo e la porta `blocked(no_stop_criterion)` non e' misurabile | «"we repeat" e' un marcatore di ripetizione» | ritrarre il criterio d'arresto lascia vivo il ciclo e porta la procedura a `blocked(no_stop_criterion)` | **aperto.** La clausola esiste gia' in `document-method.p0` come seme dichiarato e non provato |
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

### Checkpoint lasciato da SC0

SC0 ha separato due problemi che prima apparivano come uno solo:

1. **accesso al lettore:** chiuso per la classe degli introduttori. Una forma
   nuova si insegna ancorandola a una forma funzionante, senza nomi interni; il
   ruolo puo' possedere l'intero payload, la KB sceglie la facolta' eager e il
   suo esito terminale non viene riclassificato come muro;
2. **comprensione del contenuto:** ancora rossa. Sui tre testi finali Apollo,
   subduzione e forza epistemica il lettore ha instradato 3/3, ma ha riportato
   `0` fatti e `9` frasi saltate. Non esistono ancora unita' documentali, claim
   tipati o archi retorici interrogabili.

Per questo la prossima voce e' **SC1**, non un altro sinonimo di `read:`. Il
primo gate dovra' insegnare a voce una relazione fra due unita' (almeno
contrasto o metodo/risultato), conservarne gli span e ritirarla senza perdere
il contenuto proposizionale.

### Handoff operativo dettagliato: riprendere da SC1

Questa sezione e' il punto di ripresa autoritativo per un agente che non ha
assistito alla sessione SC0. Non reinterpretare il risultato a partire dalla
sola risposta finale di parrot0: leggere prima il
[report SC0](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc0.md),
poi questa sezione, poi il codice indicato sotto.

#### Confine esatto gia' raggiunto

- Il commit `a722689` contiene il primo checkpoint: la lezione naturale
  `role_for`, le sue forme KB-backed, i template di risposta e il primo
  deferral verso la faculty dichiarata. E' gia' su `origin/main`.
- Il commit immediatamente successivo, quello che contiene questo handoff,
  chiude SC0 con l'**envelope epistemico insegnabile**: estensione dell'input,
  dispatch della faculty e politica del risultato sono fatti KB, mentre C fa
  soltanto binding, ordinamento e arbitraggio.
- Il test mirato persistente
  `tests/p0t/language/taught_segment_role.p0t` chiude **21/21** assert nello
  stesso processo: baseline, lezione, replay, transfer, collisione con
  `measurement error`, ablation e reteach.
- Il solo `make soft-test` permesso dal protocollo e' gia' stato consumato in
  questo ciclo. Il risultato e' **55 passati, 1 fallito**, nel test preesistente
  `frontier_chat_audit.it.p0t` alla riga 97: atteso
  `I don't know about designation`, ottenuto
  `I don't know much about your designation yet. Want me to look it up?`.
  Non attribuire questo rosso a SC0 e non rieseguire il soft-test per
  "provare ancora": nel prossimo ciclo, dopo una modifica engine, spetta una
  sola nuova esecuzione.
- Il dato che conta e' doppio: **Route Transfer@3 = 3/3**, ma **Claim coverage =
  0/9**. Apollo, subduzione e il contrasto associazione/causalita' raggiungono
  il lettore; il lettore non costruisce ancora claim complessi interrogabili.

Non investire il prossimo ciclo in altri sinonimi di `leggi:`. La porta e'
dimostrata e retraibile. La frontiera e' ora dentro il documento.

#### Mappa minima dell'implementazione da non rompere

1. `src/brain/00-lex.c`, `try_teach_form`, implementa il modo generico
   `role_for`. Il teacher insegna una forma nuova ancorandola a una forma viva,
   per esempio `impara "Leggi questo breve testo:" come un altro modo per
   introdurre "leggi:"`. Il codice non conosce quella frase: risolve il ruolo
   dell'anchor attraverso l'evidenza `input_segment` e verifica una
   `faculty_for` realmente disponibile.
2. `kb/core/intents.p0` contiene le etichette naturali learnable di `role_for`;
   `kb/core/messages.p0` contiene le conferme. Aggiungere una lingua o una
   parafrasi significa crescere queste famiglie nella KB e provarne
   assert/retract a runtime, non aggiungere confronti lessicali in C.
3. `src/code.c`, all'inizio di `input_segment`, applica la proprieta' aperta
   `segment_extent(Role, whole)`: se un ruolo insegnato possiede il payload,
   viene pubblicato un unico span prima che i registri interni competano. E'
   il fix generale che impedisce alla parola interna `error` di trasformare un
   brano scientifico in un log del compilatore. In caso di evidenza pari deve
   emergere ambiguita', non una preferenza nascosta.
4. `kb/core/input.p0` dichiara attualmente
   `segment_extent(prose_source, whole)`, `faculty_dispatch(reader, eager)` e
   `module_result_policy(reader, terminal)`. Da questi fatti deriva il
   precedence meccanico. Sono policy aperte: una nuova faculty deve poter
   entrare aggiungendo fatti e test, senza una nuova branch nominativa.
5. `src/brain/99-registry.c` fa il join generico
   `input span -> faculty_for/2 -> faculty_dispatch/2 -> registry C`, offre
   prima l'unica faculty eager e, se ve ne sono piu' di una, lascia
   l'arbitraggio al percorso ordinario. La policy `module_result_policy`
   impedisce che il risultato onesto del reader (`0 learned, N skipped`) sia
   scambiato per resa e sovrascritto da una risposta generativa.
6. Il reader vero e' in `src/brain/30-generation-reading.c`:
   `mod_reader -> read_passage -> extract_clause`. `extract_clause` pubblica
   la struttura sotto `current_prose`, prova `input_assertion_bundle` e
   `input_frame_commit`, poi i percorsi legacy. `read_passage` conserva una
   proposizione originale soltanto se qualcosa e' stato estratto.
7. `current_prose` e' oggi **transiente per clausola**: viene ripulito prima
   della clausola successiva. Questo e' probabilmente il primo ostacolo reale
   a SC1, perche' un arco retorico fra due clausole non puo' appoggiarsi a nodi
   gia' cancellati. Non risolverlo con una lista di connettivi in C. Serve una
   pubblicazione documentale stabile di sessione, con identita', ordine, span
   fonte e receipt di commit, prima del clear.
8. Esiste gia' una scomposizione concessiva in `src/brain/99-registry.c`, prima
   del registry: separa forme come `although ..., ...` consultando
   `subordinator_stance/2` in `kb/core/grammar.p0`. E' utile e non va rimossa,
   ma oggi conserva al massimo lo stance dei due lati: non crea una relazione
   retorica persistente, non prova la provenance e non sostituisce Document
   IR.
9. Esistono gia' `contrastive_connector/1` in `kb/core/lexicon.p0` e
   `subordinator_stance/2` in `kb/core/grammar.p0`. Prima di implementare un
   nuovo protocollo, provare se la lezione generica di classe accetta gia'
   `"albeit" is a contrastive connector` e se retract ne elimina davvero
   l'effetto. Non assumere che una riga statica equivalga a teachability.
10. `input_structure_publish` fornisce gia' token, span e phrase node. Riutilizzare
    quella geometria: duplicare un secondo tokenizer o ricostruire offset da
    stringhe rendera' impossibile la provenance precisa.

#### Prossimo esperimento minimo: SC1, non ancora SC2

Obiettivo stretto: due unita' di un documento sopravvivono all'analisi della
singola clausola; una relazione retorica insegnata a voce le collega; ablation
della sola cue o regola rimuove l'arco senza cancellare le proposizioni.

Usare almeno tre generi veri e separare sempre **baseline**, **lezione**,
**replay**, **transfer** e **ablation** nello stesso processo:

1. una concessiva fattuale breve, per esempio la struttura «Although X, Y»,
   con entrambi i lati veri e verificabili;
2. il caso Apollo 13 gia' documentato dalla NASA: la missione non alluno', ma
   fu considerata riuscita per il rientro salvo dell'equipaggio;
3. un passaggio scientifico in cui un'associazione osservata non autorizza una
   conclusione causale, preso da una fonte istituzionale o open-access mai
   memorizzata prima.

Prima della lezione chiedere esplicitamente: quali claim afferma ciascuna
unita', che relazione c'e' fra le due, quale span sostiene ogni risposta e se
entrambi i claim sono stati committed. La risposta corretta non basta: devono
esistere nodi interrogabili e proof verso gli span.

La prima lezione da tentare e' una lezione naturale di classe gia' disponibile,
per esempio `"albeit" is a contrastive connector`. Se la classe viene
assertita ma non cambia la relazione documentale, il gap non e' lessicale: e'
il consumer. Solo allora introdurre il piu' piccolo producer/view KB capace di
trasformare struttura e classe in una relazione. Una correzione come «qui la
seconda unita' qualifica la prima» deve restare un obiettivo successivo, non un
pretesto per esporre al teacher nomi di predicati interni.

I nomi `document_unit`, `unit_act`, `unit_source_span` e `rhetorical_edge` sono
ipotesi di lavoro, non uno schema gia' approvato. Prima di aggiungerli cercare
relazioni equivalenti con `rg`; poi scegliere la rappresentazione minima che
permetta queste query:

- quali unita' compongono il documento e in quale ordine;
- quale atto/ruolo ha ogni unita';
- quale arco collega source e target;
- quali byte o token della fonte giustificano nodo e arco;
- da quale regola/cue deriva l'arco;
- che cosa resta dopo retract della cue.

#### Sequenza di lavoro consigliata a un agente successivo

1. Rileggere `MANTRA.md`, `PRINCIPLES.md`, `LEARN_PROTOCOL.md`, il report SC0 e
   il paragrafo 18 di `docs/plans/frontier-kb-natural-dialogue.md`.
2. Eseguire una sessione diagnostica senza `/save`. Catturare transcript e
   delta; se una correzione crea un fatto falso, chiudere con `X` e annotarlo.
3. Provare la teachability gia' esistente di `contrastive_connector` e
   `subordinator_stance`; non aggiungere C finche' non e' chiaro se manca il
   sensore, il consumer, la memoria documentale o la query.
4. Progettare il piu' piccolo strato Document IR. Se serve C, limitarlo a
   meccanica fissa: numerazione delle unita', copia degli span, ordine,
   transazione e receipt. Ruoli, nomi di cue, direzione e semantica degli archi
   devono venire dalla KB.
5. Scrivere prima il test di crescita nello stesso processo:
   baseline fallisce onestamente; lezione; replay; tre transfer; contrasto
   negativo; retract; replay negativo; reteach. Verificare separatamente che
   retract dell'arco non distrugga i claim contenuto.
6. Aggiungere query di audit via MCP o linguaggio naturale per ruolo, faculty,
   unita', span, arco, sorgente della regola e confidence. Senza audit, non
   dichiarare chiuso il gate.
7. Misurare claim coverage, relation fidelity e source fidelity, non la
   scorrevolezza della risposta. Ogni claim deve essere vero rispetto alla
   fonte scelta; nessun fatto del test va promosso in core/profile per far
   diventare verde il caso.
8. Eseguire `make build`, il test mirato persistente, `git diff --check` e una
   sola volta `make soft-test` dopo la modifica engine. Aggiornare report e
   questa sezione prima di commit e push.

#### Trappole gia' osservate

- Senza envelope, un brano lungo puo' finire nella generazione narrativa:
  Apollo e' diventato un racconto inventato. Non e' "quasi comprensione".
- La parola `error` dentro `measurement error` competeva col registro dei log.
  Conservare questo caso avversariale in ogni refactor di dispatch.
- Il sommario del reader con zero fatti veniva interpretato come resa e
  sovrascritto. La policy terminale e' parte del contratto, non cosmetica.
- Una correzione naturale mal compresa ha prodotto un falso `created_by(...)`.
  Se il delta contiene falsita', usare `X`, non `/save`.
- Diversi buffer canonici sono ancora limitati (in particolare percorsi da
  circa 256 byte): un testo lungo puo' troncare senza dimostrare un limite
  concettuale. Testare dapprima brani corti, poi stressare la lunghezza come
  asse separato.
- Il conteggio del reader e' oggi per frasi/clausole e nel probe PCR ha
  riportato `2 skipped` per un passaggio percepito come tre frasi. Non usare il
  contatore come sostituto di unita' documentali esplicite.
- Output grammaticalmente fluente o nella lingua giusta non prova estrazione.
  Chiedere claim, arco, span e proof.
- L'ordine dei moduli C non e' semantica. Per nuove facolta' usare fatti come
  `faculty_dispatch`, con tie osservabile, non spostare silenziosamente righe
  del registry.
- Sono vietati `cue(...)`, `strstr(...)` o `strcmp(...)` su nuovi letterali di
  lingua naturale in `src/brain`. Ogni superficie va nella KB e deve avere un
  test assert/retract senza rebuild.
- Non trasformare esempi scientifici, nomi propri o risposte attese in world
  facts del profilo. Le fonti vere restano input/test; la KB deve contenere la
  competenza trasferibile.

#### Comandi e disciplina di verifica

Per evitare che un test crei un processo nuovo a ogni assert, mantenere un
engine unico in una PTY con socket non condiviso:

```sh
make build
./bin/parrot0 --test-engine --sock /tmp/parrot0-sc1-<id>.sock
./bin/parrot0 --test tests/p0t/language/<nuovo-test>.p0t \
  --sock /tmp/parrot0-sc1-<id>.sock
git diff --check
make soft-test
```

Terminare l'engine della propria PTY con `Ctrl-C`. Non uccidere processi di
test non creati dalla sessione: nel workspace possono operare altri agenti.
`make soft-test` compare qui come passo del **prossimo ciclo engine**: SC0 lo ha
gia' eseguito una volta e non va ripetuto durante la sua chiusura.

Il commit SC1 dovra' essere autonomo e descrivere: ipotesi provata, lezione
naturale, fatto KB cresciuto, consumer che lo usa, test di ablation, metriche,
fonti, limite residuo e prossimo gate. Se una di queste voci manca, lasciare il
gate aperto nel TODO.

### Checkpoint lasciato da SC1 — punto di ripresa autoritativo

SC1-A e' chiusa. Leggere il
[report completo](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc1.md)
prima di modificare il lettore. Il risultato non e' «comprensione dei claim»:
e' il primo strato causale di Document IR sul quale SC2 puo' finalmente
costruire status epistemici senza ricominciare dal testo grezzo.

#### Che cosa e' stato dimostrato

- `document_unit_observe/4` copia superficie, token, range e ordine dalla
  gerarchia transiente `current_prose` **prima** che venga ripulita. Due frasi
  successive restano quindi due oggetti interrogabili nella stessa sessione.
- Il C genera soltanto l'identita' monotona `document_N`, l'ordine delle unita'
  e attiva il producer KB. La semantica vive in
  `kb/core/document-structure.p0`: classe del marker, relazione e direzione
  sono aperte e `rhetorical_edge/4` e' una vista derivata.
- La lezione naturale `"nevertheless" is a contrastive connector.` crea
  `contrastive_connector(nevertheless)`. Il consumer la usa subito senza
  rebuild: Apollo replay e tre transfer indipendenti producono due unita' e un
  arco di contrasto (`Transfer@3=3/3`). `therefore` non produce quell'arco.
- Il retract naturale elimina il membro della classe e rende non dimostrabile
  l'arco anche sui documenti gia' osservati, ma conserva unita', span e il fatto
  estratto `metal(mercury)`. Reteach e retention sono verdi.
- La stessa grammatica pura di menzione e' ora condivisa da learn/query/retract.
  Questo chiude il bug diagnostico che trasformava «forget that X is a
  contrastive connector» nel fatto falso `contrastive_connector(forget)`.
- La sola lezione linguistica e' stata promossa in una sessione pulita:
  `W=0, L=1, C=0, P=1, O=3, X=0, S=5`. Un processo nuovo parte da
  `36312 facts, 2418 rules`, ricorda la classe e ricostruisce l'arco senza
  ripetere la lezione (`FreshProcessRecall=2/2`).

Il ratchet permanente e' `tests/p0t/language/document_rhetoric.p0t`: 33 assert
nello stesso processo coprono baseline, crescita, replay, tre domini,
punteggiatura, composizione, contrasto, ablation, conservazione del contenuto,
reteach e retention. Sono verdi anche `mention.p0t` (24),
`taught_segment_role.p0t` (21) e `retract.p0t` (17 su engine ermetico).
`make build` e' verde. Il solo `make soft-test` del ciclo e' gia' stato
consumato: 55 verdi e il rosso preesistente di `frontier_chat_audit.it.p0t`
riga 97 sulla formulazione di `designation`. Non rilanciarlo per SC1.

#### Confini da non mascherare

1. `document_N` e `document_N_unit_M` sono locali al processo. Le sessioni che
   contenevano documenti non sono state salvate: fra due processi gli ID
   colliderebbero e falsificherebbero la genealogia. Non promuovere queste
   clausole finche' l'identita' non e' ancorata a fonte, versione e span.
2. L'arco attuale collega soltanto unita' adiacenti quando il primo token della
   seconda e' membro della classe dichiarata. Non copre cue multi-parola,
   marker interni al periodo, strutture annidate o archi a distanza.
3. Il sistema mappa la retorica ma non identifica ancora l'atto dell'unita':
   definizione, metodo, risultato, limite e raccomandazione restano indistinti.
4. `Claim coverage=1/8` sul nucleo replay+transfer. Un sommario con «Mapped»
   misura soltanto unita' e archi: non autorizza a dire che gli otto claim sono
   stati compresi.
5. Due sessioni di sviluppo abortite produssero ciascuna
   `contrastive_connector(forget)`. Sono `X=1` diagnostici, mai salvati. Il run
   promosso ha `X=0`; conservare entrambi i dati evita di nascondere il percorso
   causale che ha imposto il parser condiviso.
6. `reading_fact(...)` e' stato classificato come `O` e promosso dal normale
   `/save`. Questo comportamento e' un residuo M14 da riesaminare, non una
   clausola da filtrare retroattivamente durante SC2.

#### Mappa dei file e invarianti

- `src/brain/30-generation-reading.c`: apre il documento, numera le unita',
  chiama `document_unit_observe/4` prima del clear e rende un sommario
  KB-templated quando esiste almeno un arco. Non aggiungere qui parole come
  `however`, `method`, `result` o `hypothesize`.
- `kb/core/document-structure.p0`: producer e viste del Document IR. La classe
  e' invocata con `apply/2`; la regola per il token iniziale passa attraverso
  `document_unit_token_precedes/2` perche' la NAF con variabile libera non e'
  affidabile nel solver corrente.
- `src/brain/10-memory-knowledge.c`: parser puro per membership esplicita e
  parser della forma unaria multi-parola usato dal retract anticipato. Copule,
  determinanti e classi vengono interrogati nella KB. Ogni futura forma di
  correzione deve restare simmetrica alla sua forma di apprendimento.
- `kb/core/responses.p0`: il sommario documentale e la conferma di retract sono
  template KB. Per la lingua di default serve la forma `response_template/2`;
  soltanto `response_template(..., en, ...)` non viene scelta dal renderer
  default.
- `src/brain.c`: `document_seq` e' meccanica di sessione, non identita'
  persistente. Sostituirla, non attribuirle significato.

#### Prossimo esperimento minimo: SC2

SC2 deve separare tre cose che oggi coincidono accidentalmente: **frase
osservata**, **claim attribuito** e **commitment di parrot0**. Prima di
implementare, cercare con `rg` strutture riusabili (`holds_in`, provenance,
claim reificati, source/version, stance) e verificare i vincoli di K4: non creare
un secondo sistema di contesti se quello esistente puo' ospitare il documento.

L'esperimento didattico minimo usa un brano vero con due proposizioni della
forma parafrasata «gli autori ipotizzano X; i dati mostrano Y». Baseline e
lezione devono rendere verificabili separatamente:

- la frase fonte e il suo span;
- il contenuto proposizionale X o Y;
- l'attribuzione agli autori/documento;
- lo status `hypothesis` contro `observation`;
- l'assenza di commitment autonomo di parrot0 a X.

Poi servono `Transfer@3` su esperimento, studio osservazionale e simulazione,
un contrasto in cui «suggerisce» non equivale a «dimostra», composizione con
una relazione gia' nota, ablation della sola cue/status e replay negativo. Il
retract deve togliere la classificazione epistemica derivata, **non** la frase
fonte e non la proposizione attribuita. Una domanda equivalente a «il documento
ha osservato X?» deve rispondere no quando X e' soltanto ipotizzata, mentre
l'ipotesi deve restare interrogabile come ipotesi.

Prima viene l'identita' persistibile: source URI canonica o altro riferimento
fontato, versione/fingerprint e span devono determinare il documento e i claim.
Il contatore puo' restare una handle effimera, ma non deve entrare nel save come
identita' globale. Se il protocollo naturale non offre ancora un envelope per
insegnare fonte/versione, registrare quel gap e chiudere il ciclo come
diagnostico invece di inventare un URI in C.

Il test KB-first e' obbligatorio: una nuova cue di status o attribuzione deve
entrare parlando, cambiare l'analisi nello stesso processo e smettere dopo
retract, senza rebuild. Il C puo' costruire identita', copiare span, fare
binding/ordine e applicare transazioni; non puo' riconoscere letteralmente
`shows`, `hypothesize`, `authors`, `data` o le loro traduzioni. Status,
direzione, ruoli e template appartengono alla KB.

#### Strategia di crescita oltre SC2

Per aiutare agenti meno esperti, mantenere questa disciplina ad ogni SC:

1. aggiungere una sola nuova distinzione semantica per ciclo e provarne la
   causalita' con ablation;
2. conservare sempre la rappresentazione piu' debole (span/frase) quando quella
   piu' forte (claim/status/arco) viene ritirata;
3. derivare viste e risposte, non duplicare verita' materializzate difficili da
   invalidare;
4. promuovere in KB soltanto competenza linguistica/meta appresa e fatti veri
   fontati, mai i documenti diagnostici con handle locali;
5. misurare coverage e precision separatamente: aumentare gli archi con claim
   coverage rosso non e' supercomprensione;
6. portare ogni nuovo predicato attraverso proof/provenance e fresh process
   prima di comporlo con SC3-SC16;
7. dopo ogni modifica engine eseguire un solo `make soft-test`, annotare il
   primo rosso e non cambiare l'attesa per farlo sparire;
8. chiudere ogni checkpoint con report, aggiornamento di questo handoff, diff
   semantico, commit e push. Se un ID, uno status o un arco non ha una
   spiegazione di retract, non e' ancora pronto per essere salvato.

### Checkpoint lasciato da SC2-A — storico, superato da SC2-B

SC2-A e' chiusa nel
[report completo](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc2.md).
Il prossimo agente deve trattare questo checkpoint come una piattaforma, non
come una promessa di comprensione semantica: parrot0 sa conservare **chi ha
riportato che cosa, con quale status, dove e sotto quale fonte**, ma il «che
cosa» e' ancora `proposition(surface("..."))`.

#### Risultato acquisito e misurato

- Il reader riconosce una coordinata meccanica `scheme://...`, la separa dalla
  prosa e costruisce `document_<hash-source>_<hash-content>`. `document_source`
  e `document_fingerprint` rendono lo stesso input riconciliabile fra processi.
  Il contatore `document_N` resta solo il fallback per testo senza fonte.
- Le frasi lunghe non attraversano piu' la lista ricorsiva SC1. Il C enumera i
  node ID gia' pubblicati; `document_unit_node_observe/3` decide in KB quali
  siano token. La baseline Salmonella conserva il token 16 (`death`) e la
  seconda unita'. Non reintrodurre una ricorsione bounded per comodita'.
- Le menzioni quotate multi-parola sono conservate letteralmente e attraversano
  learn/query/retract con la stessa analisi. Il determinante dentro le
  virgolette e' contenuto, non una stopword. Questo e' il motivo della guardia
  in `mod_forget`; rimuoverla ricrea `hypothesis_report_marker(forget)`.
- `claim_marker_class/4` fattorizza classe aperta, status, context kind,
  attribuzione ed extent. `claim_status_evidence/2` passa attraverso `apply/2`:
  membri nuovi insegnati a voce vengono usati subito senza rebuild.
- Il C usa `kb_hypothesis_best` e `kb_evidence_matches` per ottenere classe e
  span. Copia marker e remainder e chiama `document_claim_observe/4`; non
  contiene superfici come `shows`, `hypothesize`, `authors` o `data` e non
  decide `observed`, `simulated` o `hypothesized`.
- La KB materializza `document_claim`, `unit_claim`, osservazione della cue,
  attribuzione e source record. Status, context, `holds_in(context(...), ...)`
  e commitment sono **derivati dalla cue viva**. Il retract elimina queste
  viste anche da documenti vecchi e conserva superficie, fonte, span e
  attribuzione; il reteach le fa riapparire senza rilettura.
- Le claim sono `attributed_only` dentro `reported_belief`. Nessuna viene
  promossa automaticamente a `holds_in(world, ...)`.
- Il sommario del reader conta unita', claim e status vivi, ma la sua
  verbalizzazione e' in `response_template(reader_claim_summary, ...)`, non in
  una stringa naturale C.
- Le lezioni promosse sono quattro membri linguistici; il save e' esattamente
  `W=0, L=4, C=0, P=4, O=12, X=0, S=20`. `B0=36345/2433`,
  `B1=36365/2433`, dunque `B1-B0=S=20`. `kb/learning/learned.p0` e' vuoto
  rispetto a questo ciclo: le classi hanno una casa semantica.
- Processo fresco: quattro membership `Yes` e rilettura DART con lo stesso
  document ID, due claim e due status. `FreshProcessRecall=5/5`.
- Metriche: LessonYield `4/4`, Replay pass, `Transfer@3=3/3`, parafrasi `1/1`,
  contrasto `1/1`, ablation `1/1`, reteach `1/1`, status fidelity `8/8` e
  source/span fidelity `8/8`. La normalizzazione semantica resta **`0/8`** e
  le tre domande naturali restano **`0/3`**.

Il ratchet e' `tests/p0t/language/document_claims.p0t`: 50 assert con tre cue
held-out (`the investigators predict that`, `the measurements indicate that`,
`the model outputs show that`). Non sostituirle con le forme gia' persistite:
la baseline diventerebbe verde per memoria e non misurerebbe piu' crescita
runtime. Restano verdi `document_rhetoric.p0t` 33/33, `mention.p0t` 24/24,
`retract.p0t` 17/17 e `taught_segment_role.p0t` 21/21. `retract.p0t` usa `<^`
sulla testa della regola perche' l'induzione puo' aggiungere una coda lecita e
variabile quando la KB cresce.

Il solo `make soft-test` di SC2 e' gia' consumato: 55 verdi, un rosso
preesistente in `frontier_chat_audit.it.p0t` riga 97 (`designation`). Non
rilanciarlo come parte di SC2 e non cambiare quell'attesa dentro questo
checkpoint.

#### Mappa dei file per chi riprende

- `kb/core/document-structure.p0`: unita', token non ricorsivi, source e
  fingerprint. Fonte e fingerprint sono coordinate osservate; non attribuire
  loro autenticita', canonicalita' o autorevolezza.
- `kb/core/document-claims.p0`: unica casa di classi/status/attribution e viste
  epistemiche. Le quattro righe apprese sono mischiate ai seed dal router di
  persistenza: non duplicarle altrove e non spostarle in `learned.p0`.
- `src/brain/30-generation-reading.c`: meccanica URI/hash/span, invocazione
  dello scorer universale e pubblicazione. `document_claim_from_clause` crea al
  massimo una claim per unita' e assume `extent(remainder)`: sono limiti da
  generalizzare attraverso policy KB, non condizioni linguistiche da cablare.
- `src/brain/10-memory-knowledge.c`: `p0_quoted_words` e la simmetria della
  menzione. Il quoting non gestisce ancora virgolette annidate/escape complessi;
  non far dipendere SC2-B da quel caso senza un ratchet dedicato.
- `kb/core/responses.p0`: sommario claim in inglese/italiano.
- `tests/p0t/language/document_claims.p0t`: identita', fingerprint, status
  positivo/negativo, commitment, context, source span, ablation e reteach.
- `kb/machinery/fact-provenance.p0` e `kb/machinery/transcripts.p0`: diff
  naturale di `/save`. Non riordinarlo o «ripulirlo» manualmente.

#### Limiti precisi da non coprire con una risposta elegante

1. La proposition e' una stringa di superficie. Parrot0 non sa ancora che
   `kinetic impact can alter an asteroid orbit` e' una relazione modale fra
   entita' e evento, ne' che una parafrasi esprime lo stesso contenuto.
2. `What did the authors hypothesize?`, `Was X observed?` e `What did the
   simulations show?` non consumano ancora Document IR: finiscono a muro o in
   diagnostici nominali opachi.
3. Esiste una sola claim per unita'; il marker migliore/anteriore vince e il
   contenuto e' tutto il remainder. Coordinazione, negazione, citazioni
   annidate, apposizioni e piu' status nella stessa frase restano aperti.
4. L'attribuzione e' materializzata dalla classe osservata; lo status e' vivo.
   Se in futuro la stessa cue puo' avere attribuzioni diverse per sintassi, va
   resa anch'essa una vista causale, non duplicata in un secondo sistema.
5. L'estrattore URI conserva una coordinata per forma meccanica. Non fa
   canonicalizzazione, non legge DOI, data/versione, frammenti o redirect e non
   prova autenticita'. Un punto finale attaccato all'URI non e' oggi rimosso.
6. I documenti source-addressed non sono stati salvati. Prima di farlo serve
   una policy esplicita di versione, deduplicazione, volume e invalidazione.
7. FNV-1a e' un fingerprint deterministico, non una garanzia crittografica.
8. Le risposte del sommario dicono quanti status sono vivi, non dimostrano che
   il contenuto sia semanticamente compreso.

#### Prossimo esperimento minimo: SC2-B

Obiettivo: sostituire `proposition(surface(Text))` con una coppia evidenziale
che conservi la superficie e, quando giustificato, punti a un frame semantico
normalizzato prodotto dalla **pipeline di input comune**.

Ordine consigliato:

1. cercare con `rg` gli oggetti gia' esistenti (`holds_in`, `semantic_entity`,
   `semantic_class`, frame dichiarativi, provenance, belief/attribution) e
   scegliere un solo asse canonico. Non creare `document_semantic_*` se lo
   stesso frame e' gia' usato dalle asserzioni normali;
2. estrarre la trasformazione «clausola dichiarativa -> candidato semantico» in
   una funzione pura riusabile dal reader e dal normale learner. Il reader deve
   invocarla sul remainder con un origin/context che vieta il commit nel mondo;
3. conservare sempre `proposition(surface(...))`; aggiungere una relazione
   separata verso il frame normalizzato soltanto quando l'analisi e'
   unambigua. Il fallimento della normalizzazione non deve cancellare la claim;
4. costruire query naturali come intenti/frame KB-first. Insegnare una nuova
   forma interrogativa ancorandola a un modello che gia' funziona; non chiedere
   al teacher il nome di `claim_status` o `document_claim`;
5. prima chiudere tre domande: recupero per attribuzione («che cosa
   ipotizzano?»), verifica di status («X e' osservato?»), recupero per tipo di
   evidenza («che cosa mostra la simulazione?»). Ogni risposta deve avere proof
   a claim, unita', range, document source e cue viva;
6. ratchet baseline -> lezione -> replay -> tre domini -> parafrasi -> contrasto
   -> composizione con `rhetorical_edge` -> ablation -> reteach -> retention ->
   fresh process. Usare predicati e lessico held-out rispetto alle lezioni;
7. aggiungere casi negativi in cui una ipotesi non soddisfa una domanda
   osservativa, una simulazione non soddisfa `observed`, e la rimozione di una
   cue non cancella il frame proposizionale attribuito;
8. misurare separatamente `SurfaceClaimCoverage`, `NormalizedClaimCoverage`,
   `StatusPrecision`, `QuestionAnswerCoverage`, `ProofCompleteness` e
   `FalseUnderstandingRate`. Non chiudere SC2-B con il solo conteggio `Mapped`;
9. non salvare documenti durante le sonde. Se il run finale e' pulito, salvare
   soltanto le nuove competenze linguistiche/meta e poi fare diff semantico e
   fresh process secondo `LEARN_PROTOCOL.md`;
10. dopo la prima modifica engine eseguire una sola volta `make soft-test` per
    il nuovo ciclo, annotare il primo rosso e continuare con test mirati.

#### Strategia per manipolare la crescita futura della KB

- Far crescere **classi aperte** prima delle superfici: un nuovo status, extent,
  tipo di fonte o atto documentale deve avere una relazione di policy; le
  parole diventano membri insegnabili di quella relazione.
- Mantenere un lattice di evidenza monotono: byte/range -> unita' -> claim di
  superficie -> frame candidato -> status/contesto -> eventuale commitment.
  Retrarre un livello forte non deve distruggere i livelli piu' deboli.
- Rendere derivate tutte le viste che dipendono da conoscenza correggibile.
  Materializzare soltanto osservazioni irreversibili della sessione (che testo,
  quale span, quale cue fu vista), non la loro interpretazione corrente.
- Usare `apply/2` e lo scorer universale come punti di estensione. Se una nuova
  forma richiede `strcmp`, `strstr` o una cue letterale in `src/brain`, fermarsi:
  manca una relazione KB o un consumer meccanico abbastanza generale.
- Separare sempre persistenza di competenza e persistenza del corpus. Lezioni e
  policy possono essere promosse; documenti, claim e trace richiedono una
  decisione esplicita su versione, licenza, volume, invalidazione e provenienza.
- Tenere test held-out rispetto alla KB corrente. Dopo ogni `/save`, cercare le
  cue dei ratchet: se una baseline le conosce gia', sostituirle con membri nuovi
  e verificare learn/retract nello stesso processo.
- Non aumentare la metrica per inferenza nominale. Una stringa sotto
  `proposition(surface(...))` vale come copertura evidenziale, non come
  comprensione semantica; un frame senza proof vale come candidato, non come
  conoscenza.
- Ogni nuovo status deve avere almeno un near-miss: `suggests` contro
  `demonstrates`, `estimated` contro `measured`, `simulated` contro `observed`.
  La precisione negativa viene prima della copertura aggressiva.
- Quando si introducono scope e coreference, farli puntare agli stessi node/range
  di Document IR. Non ritokenizzare il testo in un modulo parallelo: due mappe
  di span divergono e rendono impossibile la genealogia.
- Conservare nei report anche gli `X` delle sessioni abortite. Il run promosso
  puo' avere `X=0`, ma cancellare la diagnosi impedisce agli agenti successivi
  di capire quali simmetrie e guardie siano causali.


### Checkpoint lasciato da SC2-B — punto di ripresa autoritativo

SC2-B e' chiusa nel
[report completo](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc2b.md)
e nell'evidenza §18.19 di
[`frontier-kb-natural-dialogue.md`](docs/plans/frontier-kb-natural-dialogue.md).
Chi riprende deve trattare questo checkpoint come una **piattaforma sul motore**,
non come una vittoria sul contenuto: parrot0 sa normalizzare il contenuto di una
claim riportata senza crederci, e sa dire quando non ci riesce; non sa ancora
riconoscere che due frasi diverse dicono la stessa cosa.

#### Risultato acquisito e misurato

- La **fase pura** esiste ed e' una sola. `P0FrameReading` / `p0_frame_bind` /
  `p0_frame_reading` in `src/brain/10-memory-knowledge.c` legano uno schema
  dichiarato al flusso di token e restituiscono predicato, slot, slot
  interrogativo e **copertura**, senza assert, senza risposta, senza traccia. Il
  consumer storico `p0_try_extract_frames_only` usa la stessa funzione: non
  esistono due legatori di schemi. Non reintrodurne uno «per comodita' del
  lettore».
- Il lettore invoca la fase pura sul remainder e consegna alla KB un candidato
  con **origine** e **copertura**. Il C non nomina verbi, status o fonti.
- `normalization_origin/2` decide il commitment: `reported -> quarantine`,
  `asserted -> world`. Il frame di una claim riportata vive in
  `holds_in(context($Claim), ...)` e **mai** in `holds_in(world, ...)`.
  Misurato: `/debug shortened` -> nessuna clausola dopo quattro letture reali.
- `normalization_extent_policy/4` rifiuta una lettura che non copre la frase.
  Coordinazione e complemento pendente diventano `gap(partial_reading)` con la
  superficie intera. **Questa non e' una limitazione da rimuovere**: e' la
  ragione per cui i frame accettati possono essere confrontati.
- La forma interrogativa e' **derivata**, non insegnata:
  `claim_question_evidence/2` toglie il complementatore dalla locuzione appresa,
  `claim_status_question_evidence/2` costruisce `«observed that»` dallo status
  dichiarato in `claim_marker_class/4`. Insegnare un marker apre la sua domanda.
- `mod_claim_question` (registrato come `claimq`, dopo gli atti didattici e
  prima di `qa`) risponde a due forme: recupero per classe e verifica di status.
  Il cancello richiede claim osservate **e** una evidenza da cue viva.
- Ogni parola pronunciata viene da `response_template`:
  `claim_content_answer`, `claim_content_answer_unsourced`,
  `claim_status_confirmed`, `claim_status_other`,
  `reader_claim_summary_normalized`.
- Save: `W=0, L=5, C=0, P=5, O=15, X=0, S=25`; `B0=36406/2454`,
  `B1=36431/2454`, `B1-B0=S`. Le cinque lezioni sono verbi di relazione veri
  (`shortens`, `shortened`, `improved`, `detected`, `eradicated`) instradati in
  `kb/learning/taught-lexicon.p0`.
- Metriche: `Transfer@3=3/3` (NASA, GMD, WHO, PRL), `ContrastPrecision=3/3`,
  `Paraphrase=1/1`, `Composition=1/1`, `AblationFidelity=1/1`, reteach `1/1`,
  `Retention=pass`, `FreshProcessRecall=7/7`, `WorldCommitLeak=0`,
  `FalseUnderstandingRate=0`.

Il ratchet e' `tests/p0t/language/document_claims.p0t`: **124 assert** (50 di
SC2-A + 74 di SC2-B), con locuzioni e verbi **held-out** rispetto alla KB
persistita (`the investigators predict that`, `the measurements indicate that`,
`the model outputs show that`; `slowed`, `deflected`, `raised`, `weakened`,
`increased`). Non sostituirli con i membri salvati: la baseline diventerebbe
verde per memoria e smetterebbe di misurare crescita a runtime.

Rossi noti e **non** introdotti da SC2-B, verificati identici su `HEAD`
precedente: `assisted_construction.p0t` 60/6 (inversione dei ruoli di una
costruzione insegnata) e il rosso di `frontier_chat_audit.it.p0t` riga 97
(`designation`) nell'unico `make soft-test` del ciclo (55 verdi, 1 rosso).

#### Mappa dei file per chi riprende

- `src/brain/10-memory-knowledge.c`: `P0FrameReading` e la fase pura. Il campo
  `consumed`/`total` e' il cuore di D14: chi lo ignora reintroduce la perdita
  silenziosa.
- `src/brain/30-generation-reading.c`: invocazione della fase pura sul
  remainder, `mod_claim_question`, sommario del lettore.
- `src/brain/99-registry.c`: posizione di `claimq`. Spostarlo prima degli atti
  didattici fa si' che una lezione o un retract che *contengono* la locuzione
  vengano interpretati come domande.
- `kb/core/document-claims.p0`: origine, policy di copertura, gap tipati, viste
  (`claim_frame`, `claim_proof`, `claim_surface`, `claim_document_source`) ed
  evidenza interrogativa derivata.
- `kb/core/responses.p0`: le quattro frasi di risposta e il sommario normalizzato.
- `tests/p0t/language/document_claims.p0t`: il ratchet.

#### Limiti precisi da non coprire con una risposta elegante

1. Il confronto fra frame e' **strutturale**. `DART shortened the period` e
   `the period was shortened by DART` restano due frame diversi.
2. Solo `binary(Relation)` a due ruoli. Misura, classe, relazione ternaria,
   negazione e modalita' (`can`, `may`) producono un gap, non un frame.
3. La domanda di contenuto e' limitata al **documento corrente**: con piu'
   documenti letti non disambigua la fonte e sceglie l'ultimo.
4. La verifica di status pretende il complementatore esplicito
   (`observed that`); `«did anyone observe X?»` non e' ancora una parafrasi.
5. `claim_proof/2` si interroga ma non si **pronuncia**: «come lo sai?» non ha
   ancora una porta naturale.
6. Restano aperti i limiti SC2-A: una claim per unita', `extent(remainder)`,
   nessun salvataggio dei documenti, nessuna canonicalizzazione degli URI.

---

## Seconda coda 2026-08-29 — dalla supercomprensione alla metacomprensione

Le voci SC0-SC16 restano valide e descrivono che cosa un documento *contiene*.
Questa seconda coda nasce da tre osservazioni misurate in SC2-B che non
riguardano i documenti ma **il motore che li legge**, ed e' l'operazionalizzazione
delle ipotesi **D13-D20** di
[`frontier-kb-natural-dialogue.md` §18.20](docs/plans/frontier-kb-natural-dialogue.md).

Le tre osservazioni:

1. l'accoppiamento fra *leggere* e *credere* non era un difetto del lettore
   documentale: era ovunque, e SC2-B lo ha sciolto in un solo punto;
2. il residuo non letto e' l'informazione piu' preziosa che il sistema buttava
   via, perche' una lettura parziale e' indistinguibile da una completa;
3. la porta interrogativa di una conoscenza e' **derivabile** dalla conoscenza
   stessa, e dove non lo e' quel «dove» e' misurabile.

Ordine obbligato dalle dipendenze, non dall'effetto: **SC2-C, SC17, SC18** sono
precondizioni di tutto il resto.

| # | Piano complesso | Ipotesi | Classe liberata | Interazione didattica minima | Gate duro | Stato |
|---|---|---|---|---|---|---|
| **SC2-C** | **Equivalenza fra frame, modalita' e prova pronunciata** | D2, D19 | due superfici diverse che dicono la stessa cosa convergono o dichiarano perche' no; `can`/`may` diventano operatori di status invece di far fallire la lettura; «come lo sai?» risponde in lingua naturale | «"X was shortened by Y" dice la stessa cosa di "Y shortened X"»; «"può" indica una possibilità, non un risultato» | parafrasi attiva e passiva sullo stesso frame; una claim modale non soddisfa una domanda osservativa; la prova nomina documento, unita', span e cue | **CHIUSA per 2 gate su 3** — 2026-08-29: attivo/passivo convergono su un frame (passivo derivato dal participio, morfologia in KB, `frame_role_order/2` chiude anche il rosso storico dell'inversione dei ruoli); la prova si pronuncia nel registro di «come lo sai?»; il **modale resta tipizzato e non compreso** — `modal_operator` e' un residuo, non uno status. [Report SC2-C](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc2c.md) |
| **SC2-D** | **La modalita' e' un operatore dello status, non un residuo** | D2, D13 | `can`, `may`, `might`, `must` modulano la forza di una claim invece di farne fallire la lettura | «"può accorciare" dice che e' possibile, non che e' successo» | una claim modale non soddisfa una domanda osservativa; ritrarre il modale cambia lo status e non la proposizione; transfer su tre modali mai usati per la lezione | **CHIUSA** — 2026-08-29: l'operatore si stacca prima della fase pura e resta detto; `claim_asserted_status/2` esclude le claim modali; classe aperta a voce («ought is a necessity marker» vale dal turno dopo); transfer 3/3 su may/must/ought; ablation 1/1. Il misclaim `shorten(kinetic_impactor_can, …)` non e' piu' producibile. [Report SC2-D](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc2d.md) |
| **SC25** | **Dove una conoscenza viene calcolata e' conoscenza** | D21 | una relazione dichiara se le sue conclusioni sono ricalcolate a ogni turno o materializzate all'atto dell'insegnamento, senza perdere la ritrattabilita' | nessuna: e' uno strato di motore, ma deve **restare** insegnabile cio' che oggi lo e' | spostare una relazione a `materialized_on_learn` riporta il turno nel budget, non cambia nessuna risposta e conserva l'ablation; se una delle tre non vale, e' un'ottimizzazione e va rifiutata | **aperto — prossimo.** Caso concreto gia' misurato: la radice verbale per `-ed`/`-d` porta un turno da <1s a 1,85s, quindi oggi e' una lezione invece che una derivazione (`kb/core/grammar.p0`, `verb_suffix/1`) |
| **SC26** | **Una collisione di dispatch e' un fatto osservabile** | D22, mantra #14 | non *chi* ha preso il turno — quello parrot0 lo dice gia' — ma **perche'**: quale superficie gli ha dato titolo, e chi di piu' adatto e' stato scavalcato | «quel turno l'ha preso un altro modulo per via di questa parola» | le collisioni su corpus reale si concentrano su poche coppie di moduli e poche parole comuni | **aperto, campione corretto.** `who answered?` funziona da sempre: dei cinque casi raccolti uno solo era un furto (`claim` -> `missing_referent`). Vedi §18.27 |
| **SC29** | **Uso e menzione sono un invariante, non una guardia locale** | D25 | cio' che e' menzionato non partecipa alla lingua che lo menziona, in **ogni** percorso: dispatch, parser, canonicalizzazione | «"shall" e' un marcatore di necessita'» deve valere quanto «shall e' un marcatore di necessita'» quando la parola non ha ruolo | dieci locuzioni quotate con copule, ausiliari, articoli e dimostrativi entrano byte per byte e si ritrattano uguali; ogni lezione che fallisce nella forma d'uso riesce in quella di menzione; una perdita e' un errore **dichiarato**, non muta | **aperto — prossimo.** Tre gap della serie SC2/SC3 erano questo, e il rimedio parrot0 lo proponeva gia' da se' |
| **SC34** | **`mod_lone` rivendica casi speciali, non ogni turno di una parola** | D26, D28 | la rivendicazione GENERICA su un token ignoto e' sospesa e dichiarata (`move_policy(lone_bare_token, claim)`); i casi fondati restano | nessuna: e' uno strato di motore, ma non deve togliere niente a cio' che si insegna | un token insegnato ottiene titolo; un token noto alla KB resta di `mod_lone` (gen491, «milano»); un token ignoto raggiunge un muro che PROPONE il rimedio; asserire il fatto riporta il comportamento storico nello stesso turno | **CHIUSA — 2026-08-30** (F.: «dovrebbe rivendicare alcuni casi speciali, non tutti i turni con una sola parola»). Niente e' stato cancellato: la scommessa e' dichiarata, come il gen491 aveva gia' fatto per quella fonotattica. Il rientro pieno passa da SC30/SC31 |
| **SC40** | **Rileggere alla luce di cio' che si e' appena imparato** | D33 | una lettura dichiara da che cosa dipende; quando quella conoscenza cambia la lettura diventa `stale` e si ri-deriva, invece di restare congelata accanto alla nuova | «adesso che sai cosa vuol dire, rileggi quel passaggio» | insegnato un verbo dopo la lettura, una domanda sul contenuto risponde **senza rileggere a mano**; la lettura vecchia e' `stale`, non coesistente; ritrattando il verbo si torna al gap, non a un terzo stato | **aperto — PRIORITARIO.** Misurato: rileggere **accumula** invece di rivedere, e la claim resta contemporaneamente `gap(no_reading)` e `normalized(reported)`. Domanda di F.: e' un limite reale, non la natura del dialogo — `claim_proposition` e' un'interpretazione messa fra le osservazioni perche' la KB non puo' richiamare la lettura. Vedi §18.37 |
| **SC38** | **Riduzione della coda: di quale classe e' membro ogni fronte?** | D31 | molte voci della coda sono la stessa voce sotto politiche diverse; aprirle costa righe di politica e una vista, non un produttore in C | nessuna | i due fronti successivi (SC4 disegno sperimentale, SC6 causalita') si aprono senza un nuovo estrattore; se anche uno lo richiede, si scrive **quale asse della politica** non basta piu' | **aperto — prossimo.** SC5 sembrava una facolta' ed e' costata tre righe di `claim_marker_class/4` piu' una vista |
| **SC39** | **L'ellissi si recupera dalla struttura, non si inventa** | D32 | oggetto sottinteso, soggetto di relativa, unita' ereditata, agente di un passivo senza `by`: ognuno e' una riga di `elided_role/3` **se** la coordinata da cui recuperare esiste gia' | «in questo passo l'oggetto e' quello del passo prima» | ritrattare la coordinata fa sparire il recupero, non lo lascia materializzato; `unlicensed_recovery/2` resta vuoto su corpus reale; dove la coordinata non esiste, il residuo tipato resta la risposta | **aperto.** Primo caso chiuso in SC5-A: il soggetto eliso di un passo viene dall'attribuzione della classe |
| **SC35** | **Nessuna risposta piu' forte della copertura che la sostiene** | D29 | operatore, ruolo, premessa, unita' e qualificatore hanno una copertura come i token; `overclaim/2` e' interrogabile **prima** che la frase esca | nessuna: e' uno strato di motore, ma il teacher deve poter chiedere «che cosa hai lasciato fuori?» | tre difetti iniettati in un corpus reale — operatore inghiottito, premessa fuori blocco, complemento pendente — producono tre `coverage_shortfall` distinti e nessuna risposta che li ignori | **aperto — prossimo.** Tre occorrenze gia' misurate in due giorni (SC2-B, SC2-D, SC27), nessuna delle quali somigliava a un errore |
| **SC36** | **Censimento dei default impliciti del motore** | D30 | ogni punto in cui il motore sceglie senza consultare la KB diventa una politica dichiarata, a comportamento invariato | nessuna | la lista e' finita e corta; ogni voce diventa una riga di politica senza cambiare il comportamento di partenza; se e' lunga quanto il codice, D30 e' falsa | **aperto.** Cinque chiusi il 2026-08-30 con la stessa ricetta: `cue_scope`, `move_policy`, `support_mode`, `specific_participle`, `normalization_extent_policy` |
| **SC30** | **Il registro terminologico decide chi ha titolo** | D26 — asse **contesto** | un turno ha un registro terminologico, e una facolta' dichiara quali registri serve; il vocabolario arriva dai domini gia' caricati | nessuna lezione nuova: `concept_label/4`, `concept_in_domain/2` e `denotation_register/4` lo portano gia' | «a causal claim must survive confounding» smette di essere presa da un modulo di verifica **senza toccarne il codice e senza toccare l'ordine del registry**; se serve spostare il modulo, il registro non sta decidendo | **aperto.** La KB distingue gia' `fsi` da `common` sugli scacchi: la struttura c'e', governa l'uscita e non l'ingresso |
| **SC32** | **Una cue insegnata deve poter essere ritratta parlando** | D28, D25 | il contratto di conformita' di `AGENTS.md` — «retracting the cue must remove that recognition» — vale anche per le cue `substring` insegnate a voce | «forget "helyla friend" as a casual opener» deve togliere il riconoscimento | ogni frase che nomina la locuzione non la attiva; nessun tentativo di retract produce un fatto sul verbo di retract; se serve una guardia in `mod_forget` o in `mod_lone`, D28 e' falsa | **CHIUSA — 2026-08-30, e D28 CONFERMATA.** Due interventi, nessuno tocca un modulo: `cue_scope/2` + `mention_delimiter/2` (una cue non guarda dentro una menzione, additivo) e `try_forget_form` (lo stesso registro `learnable/3` letto nell'altro verso, prima di `try_teach_form` perche' «unlearn» contiene «learn»). Transfer su una seconda classe; il retract vale anche per la cue che non ottiene titolo. [Report](docs/labs/apprendimento-assistito/2026-08-30-sc32-retract-simmetrico.md) |
| **SC33** | **La scala della cue insegnabile: chiudere i gradini 8 e 9** | D28 | superficie -> passo/precondizione/criterio d'arresto (SC5), e superficie -> contratto di facolta' (`faculty_for/2` esiste, i membri non si insegnano) | «"prima di iniziare" introduce una precondizione»; «il verificatore C serve le richieste di compilazione» | il membro nuovo cambia il turno successivo, si ritratta a voce, e non richiede di toccare il registry | **aperto.** Sette gradini su nove sono gia' aperti: la ragione per cui funzionano e' che si insegna un MEMBRO di una classe con politica in KB, mai un comportamento a un modulo |
| **SC31** | **La puntualita' e' la cue dentro il ruolo** | D27 — asse **precisione** | una cue vale nel ruolo a cui appartiene, non in qualunque punto del turno; e' il mantra #8 un piano piu' su | nessuna: e' uno strato di motore | dichiarando `cue_scope(R, outside_role(mention))` nessuna cue si accende dentro una citazione, e le lezioni quotate che falliscono vanno a zero **senza riscrivere nessuna cue** | **aperto.** Ortogonale a SC30 e da comporre con essa: SC30 dice quali facolta' hanno titolo, SC31 dove possono guardare |
| **SC17** | **L'origine come parametro di ogni inferenza** | D13 | discorso citato, ipotesi controfattuale, simulazione di piano e lettura non fidata condividono la quarantena della claim riportata | «supponiamo per un momento che...», «lui sostiene che...», e poi chiedere che cosa parrot0 crede davvero | la stessa fase pura serve almeno tre origini senza un nuovo parser; ritrarre un'origine spegne **esattamente** cio' che ne dipendeva | aperto |
| **SC18** | **Il residuo non letto diventa un oggetto tipato** | D14 | ogni frase non compresa lascia span, superficie e classe del residuo, invece di un silenzio | «in questa frase la parte che non hai letto e' la coordinazione»; poi chiedere a parrot0 di elencare i propri residui | su un corpus reale i residui si raggruppano in poche classi; insegnare una classe abbassa la frequenza dell'**intera** classe | aperto |
| **SC19** | **Livelli di comprensione nominabili e monotoni** | D19 | `observed < surface < normalized < grounded < modelled`; ogni consumer dichiara il minimo che gli serve e declina nominando cio' che manca | «non chiederti se sai la frase: chiediti a che livello l'hai capita» | un consumer nuovo rifiuta input insufficienti dichiarando **solo** il proprio livello minimo; ritrarre conoscenza abbassa il livello e spegne i consumer giusti | aperto |
| **SC20** | **Chiusura interrogativa per derivazione** | D15 | la domanda si costruisce dall'asserzione con trasformazioni di lingua (caduta del complementatore, spostamento del gap, inversione dell'ausiliare) | «se sai dire "A precede B", devi saper rispondere a "che cosa precede B?"» | `InterrogativeClosure` sale aggiungendo **trasformazioni**, non consumer; una relazione insegnata oggi e' interrogabile domani senza nuovo C | aperto |
| **SC21** | **La regione, e il guadagno di una domanda** | D16, D6 | un testo denota la regione dei modelli compatibili; la precisione e' l'ampiezza della regione; la contraddizione e' la regione vuota | mostrare un paragrafo ambiguo e chiedere quale chiarimento restringerebbe di piu' | fra due chiarimenti sceglie quello a `question_gain` maggiore e sa dire perche'; ricerca incompleta -> `incomplete`, mai «nessun modello» | aperto |
| **SC22** | **Il mittente del testo e' modellabile** | D17 | obiettivo dell'autore, concessioni, funzione delle attenuazioni, obiezioni anticipate | «qui l'autore concede un punto per difendere la tesi principale» | risponde a «che cosa vuole farmi credere, e che cosa concede?» senza riassumere; ritrarre **una** locuzione concessiva cambia la risposta | aperto |
| **SC23** | **Una procedura e' un riferimento risolvibile** | D18, D5 | un metodo che delega a un'altra fonte si dichiara bloccato e nomina il passo, invece di fingersi completo | «segui il protocollo di quell'altro lavoro, con questa sola modifica» | dice **quale** passo delega e a quale fonte; fornendo la fonte diventa eseguibile senza reinsegnare i passi noti | aperto |
| **SC24** | **Il gap ricorrente diventa una richiesta di lezione** | D20, D9, D10 | parrot0 formula in lingua naturale la lezione che gli manca, con un esempio preso dal proprio residuo, ordinata per guadagno di copertura | leggere un corpus, poi chiedere «che cosa dovrei insegnarti per primo?» | la prima richiesta e' quella che, insegnata, produce il guadagno di copertura maggiore fra le candidate; il guadagno viene **misurato dopo**; `lesson_request` non auto-promuove niente | aperto |

### Pacchetti della seconda coda

1. **Pacchetto G — onesta' strutturale (SC2-C, SC17, SC18).** Sciogliere
   l'accoppiamento lettura/credenza ovunque e reificare il residuo. Uscita: un
   motore che sa dire dove finisce cio' che ha capito.
2. **Pacchetto H — tipi della comprensione (SC19, SC20).** Livelli nominabili e
   porta interrogativa derivata. Uscita: nessuna risposta costruita a un livello
   piu' alto di quello raggiunto.
3. **Pacchetto I — spazio logico (SC21).** Regione, raffinamento, guadagno di
   una domanda, regione vuota. Uscita: una definizione operativa di «buona
   domanda» che non dipende dal giudizio di chi guarda.
4. **Pacchetto J — testo come atto di un agente (SC22, SC23).** Mittente e
   procedure per riferimento. Uscita: lettura di letteratura scientifica che
   distingue cio' che l'articolo *fa* da cio' che *dice*.
5. **Pacchetto K — curriculum autogenerato (SC24).** Uscita: la coda di questo
   stesso file smette di essere scritta soltanto da noi.

### Metriche della seconda coda

Si aggiungono a quelle gia' obbligatorie:

- **WorldCommitLeak:** fatti entrati nel mondo da un'origine che non lo
  autorizzava. Deve restare `0`; qualunque valore diverso invalida il ciclo.
- **Coverage:** token consumati da una lettura / token della frase, per classe
  di residuo.
- **ResidueClassConcentration:** frazione dei residui coperta dalle prime cinque
  classi. Se resta bassa, la lettura non e' compositiva (falsifica D14).
- **InterrogativeClosure:** relazioni insegnabili interrogabili per derivazione
  / relazioni insegnabili totali.
- **LevelDiscipline:** risposte costruite a un livello non superiore a quello
  raggiunto / risposte costruite. Deve essere `1`.
- **QuestionGainAccuracy:** chiarimenti scelti che coincidono con quelli di un
  lettore esperto / chiarimenti proposti.
- **LessonRequestOrdering:** correlazione fra l'ordine delle richieste di
  lezione e il guadagno di copertura misurato dopo l'insegnamento.

### Regole di consumo di questa coda

1. Una voce per sessione, sonda breve, `LEARN_PROTOCOL.md` per intero.
2. Nessun documento viene salvato finche' la policy di versione, licenza,
   volume e invalidazione non e' esplicita. Si salvano competenze, non corpora.
3. Ogni ratchet nuovo usa membri **held-out** rispetto alla KB persistita, e
   dopo ogni `/save` si verifica che lo siano ancora.
4. Un rosso preesistente si annota e non si tocca dentro il ciclo che lo trova.
5. Se una voce richiede C, il rimedio deve trasferire ad almeno un secondo
   dominio reale non usato per progettarlo (`LEARN_PROTOCOL.md` §7.2.7).
6. Un `X > 0` ferma il salvataggio. Le sessioni abortite si riportano lo stesso:
   cancellare la diagnosi impedisce al prossimo agente di capire quali guardie
   siano causali.

---

## ⛔ HANDOFF 2026-08-30 — punto di ripresa autoritativo

Chi riprende legge **questa sezione per prima**, poi
`docs/plans/frontier-kb-natural-dialogue.md` §0 dei TODO aperti.

### Da dove ripartire, in ordine

1. **SC40 — le letture sono congelate.** È la voce prioritaria e non è una
   voce come le altre: è l'unica in coda che, chiusa, renderebbe parrot0 *più
   intelligente* invece che più capace su una classe in più. Misura riproducibile
   in §18.37; il difetto in una riga: **rileggere accumula invece di rivedere**,
   e la stessa claim resta insieme `gap(no_reading)` e `normalized(reported)`.
2. **SC35/SC36** — le due famiglie con la stessa radice (D29 l'affermazione che
   copre meno di quanto legge; D30 il default implicito).
3. **SC38** — la riduzione della coda: se D31 regge, molte voci sono la stessa
   voce sotto politiche diverse. Provarlo su SC4 e SC6 **prima** di aprirle.

### Che cosa è stato chiuso in questa sessione

| voce | esito |
|---|---|
| SC2-B | normalizzazione come fase **pura e contestuale**; `WorldCommitLeak=0` |
| SC2-C | attivo/passivo su un frame, residuo tipato, prova pronunciata |
| SC2-D | la modalità è un operatore, non una parola inghiottita |
| SC3-A | grafo argomentativo; ritrarre una premessa ritira solo i dipendenti |
| SC27 | due premesse non sono due argomenti (una **spariva in silenzio**) |
| SC32 | una cue insegnata si ritratta parlando — era una **non conformità** |
| SC30/SC34 | un token insegnato ottiene titolo; `mod_lone` cede per policy |
| SC5-A | il metodo in prosa è una **vista sulle claim**, non un secondo estrattore |

Ipotesi nuove nel piano: **D21–D33**. Ratchet nuovi:
`document_argument.p0t` (75), `document_method.p0t` (26),
`use_mention_lesson.p0t` (19), `taught_cue_ladder.p0t` (23);
`document_claims.p0t` è passato da 50 a 182 assert.

### Regole di metodo nate qui, e sono vincolanti

1. **Un rimedio proposto e non provato è un gap che non esiste.** Il muro di
   parrot0 spesso *nomina la specie del proprio arresto e propone la
   riparazione*; tre «muri» di questa serie erano riparabili al primo colpo con
   la forma che lui stesso suggeriva. Prima di classificare un gap, **eseguire
   il rimedio proposto**. (§18.27 — è il primo frutto dell'autocorrezione.)
2. **Chiedere `who answered?` invece di dedurre il colpevole.** `turn_module/2`
   c'è da sempre; dedurre il furto dall'esito ha prodotto un'ipotesi falsa.
3. **Quando un test ha bisogno di un dato fresco per passare**, chiedersi prima
   se il sistema debba *dimenticare* o **rivedere**. Aggirare un limite e
   descriverlo sono compatibili; aggirarlo e chiamarlo intenzionale no.
4. **Quando una correzione chiede di *spostare* qualcosa** (un modulo nel
   registry, una clausola prima di un'altra), quasi sempre esiste la stessa
   correzione scritta come **politica dichiarata** — e non rimanda il problema
   alla parola successiva.

### Rossi noti, tutti verificati preesistenti

`repair.p0t` (l'oracolo non compila in questo ambiente), `check_sort`,
`forget_move`, `greet` 7/1, `games`, `faceted_enumeration`,
`foundational_concepts`, `gap_dialogue`, `name_is_knowledge`,
`reactions_are_knowledge`, `gap_is_a_fact`, `gap_anchor`,
`frontier_chat_audit.it` riga 97, `assisted_construction` 65/1 (attesa stantia
sulle virgolette di `teach_form_ack` — il resto del file è stato **riportato in
verde** chiudendo il bug dell'ordine dei ruoli).

### Debiti aperti che non vanno persi

- `blocked(no_stop_criterion)` in `document-method.p0` è un **seme dichiarato e
  non provato**: serve una cue di ripetizione indipendente dall'arresto (SC37).
- Le modalità `fill` e `define` di `learnable/3` non hanno retract simmetrico.
- `mod_lone` cede per policy; il rientro pieno passa da SC30/SC31 (il **ruolo**
  del turno decide chi ha titolo, non l'ordine di arrivo).
- Il confronto fra frame è **strutturale**: due parafrasi restano due frame.

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
