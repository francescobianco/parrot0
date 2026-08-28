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
