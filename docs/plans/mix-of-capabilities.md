# Mix of capabilities — dalle abilità ai comportamenti composti

*13 settembre 2026. Analisi e proposta sperimentale su richiesta di F. Il lavoro
richiesto è questo piano: censimento, matrice degli incroci e banco di prova;
non l'implementazione di un nuovo orchestratore.*

## 1. L'ipotesi da mettere alla prova

Una facoltà che riconosce una forma ed esegue un'azione può essere utile senza
mostrare integrazione cognitiva. L'ipotesi proposta da F. è cercare il salto
nel **mix di capacità**: ciò che una capacità produce cambia ciò che un'altra
può capire, decidere o fare. Esplorare sistematicamente le coppie dovrebbe
allargare lo spazio applicativo, scoprire difetti fra parti funzionanti e far
emergere capacità contingenti che nessun modulo dichiara singolarmente.

Questa ipotesi diventa sperimentabile se distinguiamo quattro cose:

1. **Compresenza:** ricorda il mio nome e somma due numeri. Le risposte sono
   indipendenti; averle nello stesso turno non dimostra un mix.
2. **Composizione utile:** usa un valore ricordato come operando di un calcolo.
   Il risultato dipende dalla memoria e dall'aritmetica.
3. **Integrazione adattiva:** correggo quel valore e cambiano il calcolo, la
   spiegazione e l'eventuale decisione che lo utilizzavano.
4. **Generalizzazione compositiva:** una lezione cambia una distinzione comune
   e migliora combinazioni e compiti tenuti fuori dall'addestramento, senza
   una nuova risposta o un ramo scritto apposta per ciascun caso.

Il quarto livello è il bersaglio più interessante. Due capacità combinate
**non costituiscono una dimostrazione di intelligenza per definizione**:
anche una pipeline fissa può passare un esempio. Possiamo però misurare
dipendenza, adattamento e trasferimento, rendendo l'ipotesi falsificabile.

Domanda guida: **che cosa può fare A grazie a B che, nelle medesime condizioni,
non sapeva fare usando soltanto la propria informazione?** Domanda successiva:
**quando B cambia, A se ne accorge senza essere riprogrammata?**

## 2. Ricognizione: che cosa ho studiato e che cosa ho misurato

### 2.1 Perimetro e fonti

Ho letto [MANTRA](../../MANTRA.md), [PRINCIPLES](../../PRINCIPLES.md), il
registro dei moduli, i motori di apprendimento, inferenza e sogno, le
principali relazioni della KB e una selezione dei test che esercitano questi
confini. Ho inoltre eseguito quattro conversazioni attraverso `make chat`.

**Aggiornamento del caricamento (attuato, non testato in questo giro):** non esiste una configurazione dei fatti
del mondo. Si sceglie il profilo e si esercita la sua KB completa. I setup
legacy dei test citati sono stati normalizzati; le loro vecchie misure restano
obsolete e la normalizzazione non certifica le attese.

Il ramo che poteva saltare la conoscenza condivisa è stato rimosso dal motore,
insieme ai riferimenti operativi nei setup. Base e lessico comuni si caricano
sempre; gli eventuali percorsi aggiuntivi li integrano. Un profilo assente o
vuoto usa `agi`; un profilo esplicito resta la scelta del chiamante. Anche
`!forget @base` è stato rimosso dal test engine: cancellare il soggetto da
dentro non è più valido che amputarlo all’avvio. La migrazione completa del
manifesto core nel grafo dei profili rimane descritta nel piano di caricamento.

**Su richiesta di F. non sono stati eseguiti test dopo questa modifica.**
I 26 turni riportati qui sotto appartengono alla ricognizione precedente;
non validano il nuovo caricamento. Le attese legacy restano da rivalutare da
parte di F., senza ripristinare alcuna configurazione di KB ridotta.

La ricognizione è iniziata su `699faa55`; durante il lavoro HEAD è avanzato a
`ee9ee9f5`. Tutti i transcript qui riportati dichiarano il binario
`gen510-riparazioni@ee9ee9f5`. I commenti di alcuni file riportano generazioni
successive: per riprodurre le osservazioni conta il commit, non il numero
in un commento. Questa è una fotografia di lavoro, non un audit immutabile
di ogni riga della codebase.

Nella fotografia censita:

| Elemento | Quantità | Che cosa significa |
|---|---:|---|
| Voci di `registry[]` in [99-registry.c](../../src/brain/99-registry.c) | 79 | Ingressi al dispatch, non 79 capacità cognitive indipendenti. |
| File `.p0` sotto `kb/` | 243 | Distribuzione della conoscenza, non una misura di comprensione. |
| File `.p0t` sotto `tests/p0t/` | 490 | Casi e contratti disponibili; la loro presenza non dichiara che oggi passino. |
| Fatti/regole annunciati al boot delle sonde | 54.451 / 3.548 | KB caricata con base reale, mondo e profilo `agi`. |
| Turni naturali osservati nelle quattro sonde | 26 | Campione esplorativo, senza valutazione globale della suite. |

Il censimento incrocia tre inventari che non coincidono:

- **Moduli effettivamente registrati**, più ingressi esterni al dispatch:
  `brain_think`, `dream_run`, lettura, salvataggio, solver, strumenti e API.
- **Conoscenza delle capacità:** [capabilities.p0](../../kb/core/capabilities.p0)
  contiene un ledger di maturità di 12 voci, anche `absent` e `seed`;
  [procedures.p0](../../kb/core/procedures.p0) contiene `module_capability`;
  [inference-capabilities.p0](../../kb/core/inference-capabilities.p0) espone
  invece portatori interrogabili attraverso `capability_registry`,
  `capability_provider` e `capability_call`. Sono tre contratti diversi.
- **Comportamenti:** i `.p0t`, i report e i dialoghi. Il ledger generato conserva
  descrizioni storiche; non sostituisce la verifica del consumer attuale.

### 2.2 Il censimento usa famiglie, non scatole anatomiche

Il catalogo seguente comprende **24 famiglie operative**, con sottoabilità
esplicite. La matrice enumera **tutte le 276 coppie fra queste famiglie**:
`24 × 23 / 2`. Non pretende che 24 sia il numero definitivo delle abilità di
parrot0. Una famiglia può essere scomposta quando le evidenze lo richiedono.

Le famiglie sono una lente sperimentale esterna, non una partizione disgiunta
delle clausole e non nuovi moduli da imporre al progetto. Memoria profonda,
per esempio, è già una capacità composta da scelta, acquisizione, lettura e
ritenzione. In una sua coppia bisogna specificare che cosa aggiunge l'altra
capacità rispetto a questo funzionamento ordinario. Altrimenti si conta due
volte la medesima operazione.

Gli stati delle evidenze sono:

- **I:** implementazione o regole individuate e ispezionate.
- **T:** test pertinente sulla KB completa, letto come contratto; non
  necessariamente rieseguito, né sufficiente a provare trasferimento.
- **O:** comportamento osservato in questa ricognizione, limitato al transcript.
- **H:** ipotesi applicativa o emergente da verificare.

Una riga `I/T` non equivale a una capacità generale certificata. **Ogni
esperimento con KB ermetica è obsoleto, escluso dall'evidenza e dai punteggi:
non misura parrot0.** Vale anche quando pretendeva di misurare soltanto una
meccanica. La KB è parte del soggetto, non una dipendenza da sostituire.
Nei test storici va letta la configurazione effettiva: perfino l'etichetta
`[mock hermetic]` può accompagnare una KB completa. Conta ciò che viene
caricato, non il nome della sezione. I rimandi a test obsoleti servono soltanto
a rintracciare un'intenzione storica; quel comportamento è da misurare da capo.

## 3. Catalogo delle capacità

| ID | Famiglia e sottoabilità censite | Appigli nell'implementazione e prove pertinenti | Evidenza e confine |
|---|---|---|---|
| **01** | **Lettura linguistica e denotazione.** Canonicalizzazione, lessico, morfologia, sinonimi, traduzione, segmentazione, sintagmi, ruoli, uso/menzione, domande e verso delle relazioni; forme simboliche e trasformazioni di stringhe. | [00-lex.c](../../src/brain/00-lex.c), [grammar.p0](../../kb/core/grammar.p0), [denotation.p0](../../kb/core/denotation.p0), [85-translate-synth-world.c](../../src/brain/85-translate-synth-world.c); [question_direction.p0t](../../tests/p0t/language/question_direction.p0t), [taught_lexicon.p0t — misure storiche obsolete](../../tests/p0t/language/taught_lexicon.p0t). | I/T/O. Domanda italiana sul sangue riuscita. Copertura per forme e lingue, non comprensione linguistica illimitata. |
| **02** | **Memoria di lavoro e discorso.** Referenti, ordine delle menzioni, coreferenza, possessivi, ellissi, soggetto eliso, argomento corrente, questioni e offerte pendenti, continuità fra turni. | [discourse.p0](../../kb/core/discourse.p0), [issues.p0](../../kb/core/issues.p0), [30-generation-reading.c](../../src/brain/30-generation-reading.c); [reference_binding.p0t](../../tests/p0t/conversation/reference_binding.p0t), [session_board.p0t](../../tests/p0t/conversation/session_board.p0t), [retention.p0t](../../tests/p0t/conversation/retention.p0t). | I/T. Sapere che cosa è stato nominato è distinto dal possedere il fatto sul mondo. Ritenzione e ripresa vanno provate con distrattori. |
| **03** | **Memoria personale e persistente.** Nome, valori, preferenze, possessi, relazioni personali; richiamo, instradamento del salvataggio e ripristino. | [10-memory-knowledge.c](../../src/brain/10-memory-knowledge.c), `brain_save_session`, `kb_save_routed`; [memory_natural.p0t — misure storiche obsolete](../../tests/p0t/knowledge/memory_natural.p0t), [persist.p0t — misure storiche obsolete](../../tests/p0t/save/persist.p0t). | I/O. Test storici citati obsoleti. Primo valore ricordato e usato; aggiornamento e dimenticanza falliti nella sonda. Persistenza fra processi non provata qui. |
| **04** | **Memoria profonda e ricerca esterna.** Rilevazione del bisogno, politica chiedi/agisci/mai, accesso a un provider, disambiguazione, lettura, indirizzo e revisione della fonte, riuso del topic acquisito. | [network.p0](../../kb/core/network.p0), `network_acquire` in [50-self-research-loop.c](../../src/brain/50-self-research-loop.c); [deep_memory.p0t](../../tests/p0t/knowledge/deep_memory.p0t), [disambiguation.p0t](../../tests/p0t/knowledge/disambiguation.p0t). | I/T. Il ciclo è presente. Il test con zorbium prova meccanica su un'edizione artificiale; non certifica conoscenza reale o affidabilità attuale di Wikipedia. Nessun fetch confermato nelle sonde. |
| **05** | **Comprensione della prosa e dei documenti.** Estrazione di proposizioni, enumerazioni e relazioni; continuità del soggetto nel passo; unità, claim, struttura retorica, argomentazione, metodo descritto e provenienza degli span. | [30-generation-reading.c](../../src/brain/30-generation-reading.c), [document-claims.p0](../../kb/core/document-claims.p0), [document-method.p0](../../kb/core/document-method.p0); [structural_reader_live.p0t](../../tests/p0t/language/structural_reader_live.p0t), [document_argument.p0t](../../tests/p0t/language/document_argument.p0t). | I/T/O. Lettura elementare reefs/colonies riuscita; non prova comprensione di un documento intero. «Ho estratto N fatti» non misura la sostanza conservata. |
| **06** | **Apprendimento impartito a runtime.** Fatti, definizioni, lessico, forme di domanda e lezione, costruzioni, regole con variabili, procedure, template e condotte insegnabili. | [assisted-learning.p0](../../kb/core/assisted-learning.p0), [language-forms.p0](../../kb/core/language-forms.p0), [10-memory-knowledge.c](../../src/brain/10-memory-knowledge.c); [taught_turn_form.p0t](../../tests/p0t/language/taught_turn_form.p0t), [higher_order_lesson.p0t](../../tests/p0t/language/higher_order_lesson.p0t). | I/T/O limitata alle lezioni osservate. Una ricevuta non prova acquisizione efficace. `!assert` prova una meccanica, non insegnamento naturale. |
| **07** | **Revisione, correzione e dimenticanza.** Supersessione, ritiro, invalidazione delle dipendenze, nuova interpretazione di un testo già letto, ritenzione selettiva. | [context-scope.p0](../../kb/core/context-scope.p0), [document-claims.p0](../../kb/core/document-claims.p0); [document_revision.p0t](../../tests/p0t/language/document_revision.p0t), [correction_supersedes.p0t](../../tests/p0t/conversation/correction_supersedes.p0t), [forget_move.p0t — misure storiche obsolete](../../tests/p0t/conversation/forget_move.p0t). | I/T/O negativa sulla memoria numerica. I test di supersessione dichiarano anche debito sulla conservazione del fatto storico: non assimilare ritrattazione, aggiornamento e revisione. |
| **08** | **Accesso relazionale alla conoscenza.** Ricerca di fatti, enumerazione, classificazione, interrogazione per faccette, scoperta del predicato che collega entità, modelli e leggi disponibili. | [inference-capabilities.p0](../../kb/core/inference-capabilities.p0), `holds`, `apply`, `mod_aggregate`, `mod_answer_frame`; [living_capabilities.p0t](../../tests/p0t/reasoning/living_capabilities.p0t), [faceted_enumeration.p0t](../../tests/p0t/knowledge/faceted_enumeration.p0t). | I/T/O su Ghana/cedi, Shakespeare/Hamlet, carbonato di calcio/ossigeno. Tre relazioni recuperate non provano scoperta di una nuova legge. |
| **09** | **Deduzione e spiegazione della prova.** Unificazione, congiunzioni, quantificazione nelle forme supportate, transitività, sillogismi, catene multipasso, spiegazione dei sostegni. | [kb.c](../../src/kb.c), [procedures.p0](../../kb/core/procedures.p0), [composition.p0](../../kb/core/composition.p0); [syllogism_universal.p0t — misure storiche obsolete](../../tests/p0t/reasoning/syllogism_universal.p0t), [proof_trace.p0t — misure storiche obsolete](../../tests/p0t/meta/proof_trace.p0t), [all_quadrants.p0t](../../tests/p0t/crossing/all_quadrants.p0t). | I/T/O su Terra/Mercurio, senza traccia verificata nelle sonde. Una risposta «No» richiede controllo epistemico: fallimento della prova e negazione non coincidono. |
| **10** | **Abduzione e diagnosi.** Ricerca di premesse che spieghino un esito, alternative, catene abduttive, eliminazione di ipotesi incompatibili. | [90-repair-robust-abduce.c](../../src/brain/90-repair-robust-abduce.c); riferimenti storici: [abduce_chain.p0t — misure storiche obsolete](../../tests/p0t/reasoning/abduce_chain.p0t), [branching_abduce.p0t — misure storiche obsolete](../../tests/p0t/reasoning/branching_abduce.p0t). | I; occorre una misura valida con KB completa. Generare una causa possibile non equivale ad accertarla; servono alternative, osservazioni e condizioni esplicite. |
| **11** | **Astrazione e trasferimento.** Due sottoabilità distinte: **11a induzione**, da esempi numerici o cooccorrenze a candidati di regola; **11b analogia**, trasferimento della struttura di una relazione. Include few-shot, successioni e archetipi supportati. | `kb_induce` in [kb.c](../../src/kb.c), [65-induce-verify-shell.c](../../src/brain/65-induce-verify-shell.c), `mod_analogy` in [40-meta-reflection.c](../../src/brain/40-meta-reflection.c); riferimenti storici: [induce.p0t — misure storiche obsolete](../../tests/p0t/reasoning/induce.p0t), [analogy.p0t — misure storiche obsolete](../../tests/p0t/reasoning/analogy.p0t). | I. Induzione e analogia non si certificano a vicenda. Il precedente esperimento ermetico di analogia è obsoleto e non vale come evidenza; occorre una nuova misura sulla KB completa. Restano riconoscitori legacy. |
| **12** | **Causalità, ipotesi e mondi contestuali.** Cause/effetti, condizioni abilitanti, conseguenze, controfattuali, «se non», credenze attribuite, stipulazioni e separazione fra descritto, ipotizzato e reale. | [situation.p0](../../kb/core/situation.p0), [stipulation.p0](../../kb/core/stipulation.p0), [context-scope.p0](../../kb/core/context-scope.p0); [counterfactual.p0t — misure storiche obsolete](../../tests/p0t/reasoning/counterfactual.p0t), [attributed_belief.p0t](../../tests/p0t/conversation/attributed_belief.p0t). | I/T. Una relazione causale dichiarata non conferisce causalità alle correlazioni indotte. Il mondo ipotetico deve restare locale. |
| **13** | **Calcolo e ragionamento quantitativo.** Aritmetica, algebra, equazioni, percentuali, unità, tassi, geometria e leggi, probabilità nei modelli supportati, conteggi, aggregazioni, problemi verbali e ruoli numerici. | [20-math.c](../../src/brain/20-math.c), [25-wordmath-reasoning.c](../../src/brain/25-wordmath-reasoning.c), [numeric-questions.p0](../../kb/core/numeric-questions.p0); [rate_total.p0t](../../tests/p0t/math/rate_total.p0t), [probability_inverse_growth.p0t — misure storiche obsolete](../../tests/p0t/reasoning/probability_inverse_growth.p0t). | I/T/O: 7+3 riuscito, operando obsoleto dopo correzione. Formula presente, binding degli slot e interpretazione del problema sono prove diverse. |
| **14** | **Tempo, spazio e ordinamento.** Durate, date, intervalli, prima/dopo, ordini transitivi, luoghi e contenimenti geografici, trasferimenti locativi e validità situazionale. | [time-questions.p0](../../kb/core/time-questions.p0), [procedures.p0](../../kb/core/procedures.p0); [time_date_complex.p0t — misure storiche obsolete](../../tests/p0t/math/time_date_complex.p0t), [geographic_location.p0t](../../tests/p0t/knowledge/geographic_location.p0t), [locative_transfer_frame.p0t](../../tests/p0t/conversation/locative_transfer_frame.p0t). | I/T. Contenimento geografico non significa navigazione fisica; ordinare eventi non significa disporre di una cronologia generale. |
| **15** | **Confronto, vincoli e scelta.** Somiglianze/differenze, grandezze, ammissibilità, priorità, risorse, rischio e criteri di valutazione. | `mod_compare`, [risk.p0](../../kb/core/risk.p0), [code-quality.p0](../../kb/core/code-quality.p0); [instance_under_constraint.p0t](../../tests/p0t/knowledge/instance_under_constraint.p0t), [risk_policy.p0t](../../tests/p0t/reasoning/risk_policy.p0t). | I/T. La scelta deve dipendere da criteri dichiarati; non attribuire ottimizzazione generale alla presenza di una graduatoria. |
| **16** | **Pianificazione e procedure.** Scomposizione della richiesta, più obblighi di risposta, prerequisiti, risorse, passi, rami condizionali e piani di azione. | [procedures.p0](../../kb/core/procedures.p0), [conditional-plans.p0](../../kb/core/conditional-plans.p0), [thinking.p0](../../kb/core/thinking.p0); [multi_step_plan.p0t](../../tests/p0t/reasoning/multi_step_plan.p0t), [plan_resources.p0t](../../tests/p0t/reasoning/plan_resources.p0t). | I/T. Piani KB esistenti, planner generale ancora da non presumere. Dire i passi, eseguirli e verificarne l'effetto sono capacità differenti. |
| **17** | **Strumenti, azione e osservazione.** Elencare/cercare/leggere/scrivere file, eseguire comandi, interpretare esiti, verificare con un oracolo, ricondurre l'uscita a conoscenza e obblighi. | [60-agent-tools.c](../../src/brain/60-agent-tools.c), [exec.c](../../src/exec.c), [capabilities.p0](../../kb/core/capabilities.p0); [tool_result_becomes_knowledge.p0t](../../tests/p0t/tools/tool_result_becomes_knowledge.p0t), [toolexec.p0t](../../tests/p0t/tools/toolexec.p0t). | I/T. Dipende dagli strumenti attivi e dai contratti disponibili. L'executor non è una competenza illimitata sul contenuto dei file. |
| **18** | **Comprensione e costruzione del codice.** Segmentazione, AST/IR, simboli, relazioni fra file, contratti di header, qualità, scelta/emissione di forme, patch, build e riparazione. | [code.c](../../src/code.c), [80-code.c](../../src/brain/80-code.c), [patch.c](../../src/patch.c), [code-ir.p0](../../kb/core/code-ir.p0); [universal_code_ir.p0t — misure storiche obsolete](../../tests/p0t/code/universal_code_ir.p0t), [build_repair_cycle.p0t](../../tests/p0t/code/build_repair_cycle.p0t). | I/T. Il ciclo su un piccolo progetto artificiale è una prova di meccanica con build reale. Non dimostra capacità generale su repository sconosciuti, transazioni multi-file o riparazioni arbitrarie. |
| **19** | **Realizzazione, sintesi e generazione.** Risposte da frame, parafrasi e spiegazioni, riassunto, enumerazioni rese in testo, descrizione, continuazioni apprese, narrativa/dialoghi e altre forme creative supportate. | [30-generation-reading.c](../../src/brain/30-generation-reading.c), [responses.p0](../../kb/core/responses.p0), [presentation.p0](../../kb/core/presentation.p0); [semantic_summary.p0t — misure storiche obsolete](../../tests/p0t/growth/semantic_summary.p0t), [register_realization.p0t](../../tests/p0t/generation/register_realization.p0t). | I/T/O sulla resa delle sonde. Generare testo plausibile non autorizza fatti nuovi; anche il numero di dettagli conservati deve essere verificato. |
| **20** | **Pragmatica, interlocutore e registro.** Atti sociali, intenzioni, ruolo, preferenze dell'utente, situazione comunicativa, livello di dettaglio, formato, lingua di risposta e pertinenza. | [70-social-pragma.c](../../src/brain/70-social-pragma.c), [user-situations.p0](../../kb/core/user-situations.p0), [register.p0](../../kb/core/register.p0); [user_model_stress.p0t — misure storiche obsolete](../../tests/p0t/conversation/user_model_stress.p0t), [turn_size_constraint.p0t](../../tests/p0t/conversation/turn_size_constraint.p0t). | I/T. Reazioni sociali non equivalgono a teoria della mente generale. Lo scambio «forget…» di §4 mostra una risposta personale fuori compito. |
| **21** | **Metacognizione, provenienza e limiti epistemici.** Self-model da stato reale, capacità/limiti, «come lo sai», sorgenti, prova, negazione guadagnata, conflitto, incertezza, gap, autoverifica e condotte interrogabili. | [epistemic-status.p0](../../kb/core/epistemic-status.p0), [own-methods.p0](../../kb/core/own-methods.p0), [40-meta-reflection.c](../../src/brain/40-meta-reflection.c); [earned_negation.p0t](../../tests/p0t/knowledge/earned_negation.p0t), [reflexive_selftest.p0t — misure storiche obsolete](../../tests/p0t/meta/reflexive_selftest.p0t). | I/T/O: autodescrizione prudente, ma derivata dal ledger, non una nuova verifica di tutte le capacità. Non implica coscienza o infallibilità del self-report. |
| **22** | **Riparazione e ripresa del compito.** Chiarimento di slot, ambiguità e input incompleto; arresti tipati, rimedi e compensazioni, correzione di un percorso, ripresentazione del bisogno originario. | [arrests.p0](../../kb/core/arrests.p0), [gap-kinds.p0](../../kb/core/gap-kinds.p0), [90-repair-robust-abduce.c](../../src/brain/90-repair-robust-abduce.c); [repair.p0t](../../tests/p0t/repair/repair.p0t), [self_compensation.p0t](../../tests/p0t/conversation/self_compensation.p0t). | I/T. Distinta da 07: qui si ripara l'attività bloccata, là la validità della conoscenza. Non ogni arresto ha un rimedio disponibile. |
| **23** | **Iniziativa e indagine.** Offerte motivate, approfondimento, domande nate da lacune, scelta di un'osservazione discriminante, gestione delle risposte e uscita dai loop conversazionali. | [initiative.p0](../../kb/core/initiative.p0), [inquiry.p0](../../kb/core/inquiry.p0), `mod_initiative`; [informative_action.p0t](../../tests/p0t/reasoning/informative_action.p0t), [investigation.p0t — misure storiche obsolete](../../tests/p0t/reasoning/investigation.p0t). | I/T. Il test delle osservazioni discriminanti dichiara mondi e osservabili artificiali: ottima prova di meccanica, non scoperta autonoma di un esperimento reale. |
| **24** | **Rielaborazione e crescita autonoma.** Due sottoabilità distinte: **24a thinking**, rientro di risultati tramite schemi, precondizioni e arresto; **24b dream**, attività di lettura/esplorazione guidata anche dalle lacune, con bilancio e persistenza. | `brain_think` in [99-registry.c](../../src/brain/99-registry.c), [thinking.p0](../../kb/core/thinking.p0), [dream.c](../../src/dream.c); [thinking_e1.p0t](../../tests/p0t/reasoning/thinking_e1.p0t), [autonomous_cycle.p0t](../../tests/p0t/meta/autonomous_cycle.p0t). | I/T. Thinking è distinto dal solver di 09; dream è distinto dall'accesso ordinario di 04. Il driver ispezionato imposta la rete durante il fetch e mantiene controlli C: non dichiarare già un'attività autonoma integralmente KB-first. |

### 3.1 Copertura del registro, senza perdere le facoltà minori

Questa mappa assegna una casa di censimento a tutte le **79 voci** del registro.
È una classificazione primaria, non un vincolo su chi possa contribuire a cosa.

| Famiglia | Nomi registrati |
|---|---|
| 01 | `mention`, `input`, `translate`, `spell`, `wordquery`, `namestart`, `symbolic` |
| 02 | `coref`, `discourse` |
| 03 | `memory`, `personal`, `family` |
| 04 | `learn` |
| 05 | `reader`, `claimq` |
| 06 | `lessonform`, `teachconstruction`, `taughtframe`, `teachrule`, `teachreply` |
| 07 | `forget` |
| 08 | `qa`, `world`, `knowledge`, `answerframe`, `aggregate` |
| 09 | `deepreason`, `conj`, `claim`, `same` |
| 10 | `abduce`, `robust` |
| 11 | `induce`, `fewshot`, `analogy`, `archetype`, `sequence` |
| 12 | `cause`, `counterfactual`, `whatifnot` |
| 13 | `arith`, `operator`, `algebra`, `wordproblem`, `quantity`, `count` |
| 14 | Sottooperazioni di `quantity`, `world`, `knowledge` e regole condivise; nessun ingresso esclusivo. |
| 15 | `compare`, `strategy` |
| 16 | `compose`, `plan`, `agent`, `toolplan` |
| 17 | `piact`, `tool`, `search`, `verify`, `shell`, `toolpolicy` |
| 18 | `rulespec`, `codeast`, `code` |
| 19 | `gen`, `synth`, `summary`, `reqgen`, `bench` |
| 20 | `role`, `pragma`, `social`, `chitchat`, `lone`, `smalltalk` |
| 21 | `meta`, `self`, `calibrate`, `gapreport` |
| 22 | `repair` |
| 23 | `initiative` |
| 24 | `loop`, più `brain_think` e `dream_run` fuori dal registro. |

Scacchi, altri giochi, medicina, programmazione, scienze, arti e discipline
del [profilo agi](../../kb/profiles/agi.p0) sono **domini nei quali esercitare
le capacità**, non moltiplicatori automatici del numero di facoltà. Conoscere
le regole di un gioco non dimostra saper scegliere una buona mossa. Lo stesso
vale per i nomi `skill` nei file dei profili: sono indizi da verificare.

MCP, server e API sono canali di accesso; materialized view, indicizzazione,
cache, limiti del solver e serializzazione sono meccaniche abilitanti. Vanno
controllati perché possono spezzare un mix — soprattutto con dati obsoleti —
ma non contati come ulteriori capacità cognitive. L'inventario mantiene quindi
visibili anche le abilità indirette, senza trasformare ogni funzione C in una
facoltà.

## 4. Interlocuzioni: un primo banco concreto

Configurazione: `make chat`, `PARROT0_BASE=kb/core/base.p0`,
`PARROT0_LANG=en`, profilo `agi` e strumenti/rete
attivati dal target. Directory runtime separate sotto `/tmp`; nessun `/save`.
`PARROT0_SESSION=` è stato impostato per convenzione, ma [env.c](../../src/env.c)
lo dichiara obsoleto: non è la garanzia di isolamento. Processi distinti e
assenza di salvataggio delimitano queste conversazioni. Nessun `PARSE ERROR`
negli stderr raccolti. Build riuscita; suite generale non eseguita per questa
modifica documentale.

### 4.1 Relazioni reali raggiungibili

```text
> what is the relation between ghana and cedi?
Between ghana and cedi I hold 1: currency.
> what is the relation between william shakespeare and hamlet?
Between william shakespeare and hamlet I hold 1: author.
> what holds between calcium carbonate and oxygen?
Between calcium carbonate and oxygen I hold 1: contains.
> di che colore è il sangue?
Rosso.
> is earth habitable?
Yes.
> is mercury habitable?
No.
```

Le prime tre risposte usano conoscenza già presente: non ho insegnato i
collegamenti. La quarta è un candidato concreto 01×08; le ultime due sono
sonde compatibili con il crossing già esistente, ma senza diagnosi della prova
il «No» non va promosso a dimostrazione di una negazione. Sono osservazioni
positive circoscritte, non ancora certificazioni causali del mix.

### 4.2 Il primo mix funziona; la correzione rompe il comportamento

```text
> remember my favorite number is 7
Got it: your favorite number is 7.
> what is my favorite number plus 3
10.
> my favorite number is 11
Got it: your favorite number is 11.
> what is my favorite number plus 3
10.
> forget my favorite number
I don't have real preferences, but if I had to choose, I'd pick seven.
> what is my favorite number plus 3
10.
```

03×13 ha una prima composizione utile. La variante corretta dovrebbe dare 14,
oppure dichiarare un conflitto reale e chiedere quale valore usare; confermare
11 e continuare silenziosamente con 7 è un fallimento.

Una seconda sessione ha inserito richiami diretti, per localizzare il difetto:

```text
> remember my favorite number is 7
Got it: your favorite number is 7.
> what is my favorite number
Your favorite number is 7.
> my favorite number is 11
Got it: your favorite number is 11.
> what is my favorite number
Your favorite number is 7.
> what is my favorite number plus 3
10.
> forget that my favorite number is 7
Got it: your favorite number is 7.
> what is my favorite number
Your favorite number is 7.
> what is my favorite number plus 3
10.
```

**Il problema è già visibile nel richiamo semplice.** Questa sonda non dimostra
un difetto esclusivo del passaggio memoria→aritmetica: prima va verificato
03×07, poi ripetuto 03×07×13. È proprio il vantaggio della matrice: distingue
un ponte rotto da un componente che consegna un dato errato. La lezione
personale inventata misura aggiornamento e binding; non è prova di
*connecting dots* fra conoscenze preesistenti.

### 4.3 Metaconoscenza del verso e limite della resa

```text
> read: Reefs are formed of colonies.
Learned 1 fact(s), skipped 0.
> what are reefs made of?
Colonies.
> what are colonies made of?
I don't know: nothing I hold says colonies made of of, and nothing says it isn't so. Tell me either way and I will hold it.
> "made of" is a whole-part relation
Learned: "made of" is a whole_part_relation.
> what are colonies made of?
I don't know: nothing I hold says colonies made of of, and nothing says it isn't so. Tell me either way and I will hold it.
```

La direzione inversa non produce «reefs», ma la resa «made of of» rimane
difettosa. Poiché la distinzione era già disponibile, questa lezione ridondante
**non prova nuova crescita a runtime**. La prova di acquisizione/ablazione
esiste in [prose_triage.p0t](../../tests/p0t/language/prose_triage.p0t), non è
stata rieseguita qui. L'esempio propone 01×05 e 06×21 come piste, conservando
il confine fra ciò che è stato osservato e ciò che è stato soltanto letto.

Nell'ultimo turno, `what can you do?` ha prodotto un'autodescrizione qualificata
da «limited inference», «small code fragments» e «still immature». È un uso
del ledger, utile per 21 ma insufficiente come censimento: non enumera tutte
le sottoabilità sopra e non esegue una prova di competenza al momento.

## 5. Come leggere e certificare una coppia

Una scheda non si intitola soltanto «memoria × ragionamento». Deve registrare:

| Campo | Contenuto necessario |
|---|---|
| Identità | Coppia di famiglie, sottoabilità esatte, revisione del catalogo. |
| Compito | Bisogno dell'interlocutore, prompt naturale, risultato utile verificabile. |
| Contributi | Che cosa deve produrre A; quale decisione di B dipende da quel prodotto. |
| Oggetto condiviso | Entità, proposizione, ruolo, quantità, fonte, questione, obbligo o risultato di strumento. |
| Direzione | A→B, B→A, ciclo A→B→A; eventuali prerequisiti comuni C, D… |
| Evidenza iniziale | Conoscenze già vive, fonte reale o lezione necessaria; distinguere i tre casi. |
| Controllo | Variante che modifica il contributo di A; variante che rende discriminante B; conservazione degli aspetti irrilevanti. |
| Esito | Corretto, errato, incompleto, chiarimento pertinente, limite dichiarato, timeout, non eseguito. |
| Diagnosi | Difetto di componente, collegamento, arbitrato, revisione, resa oppure conoscenza assente. |
| Crescita | Lezione naturale nuova, uso nello stesso binario, ritiro, ripristino; transfer tenuto da parte. |

**A×B è simmetrica come casella del catalogo, non come processo.** 04→09
significa dedurre usando ciò che si è letto; 09→04 significa usare la deduzione
per individuare quale premessa cercare. Sono due prove distinte della stessa
coppia. Aggiungere un ritorno, una revisione o un chiarimento crea altre forme
di prova, non altre coppie aritmetiche.

Il supporto linguistico ordinario non rende ogni caso una coppia con 01. Per
contare 01 serve una distinzione linguistica discriminante: lingua diversa da
quella del fatto, cambio di verso, sintagma composto, forma nuova insegnata.
Analogamente 04×05 richiede comprensione nuova del passo, oltre a ottenere
una definizione; 16×17 richiede che un'osservazione cambi il piano, oltre a
lanciare due strumenti nell'ordine già fissato.

### 5.1 Due binari di prova, compatibili con i mantra

**Comportamento sulla KB viva.** Base, mondo e profilo completo restano
caricati. Si parte da conoscenze reali già presenti, si domanda in lingua
naturale e si giudica il contenuto utile. Nessuna iniezione del risultato,
micro-mondo costruito per il verde o requisito sul nome del modulo vincente.
Sono il riferimento [crossing/README](../../tests/p0t/crossing/README.md) e
la disciplina di [connecting-dots](connecting-dots.md).

**Meccanica e crescita.** Sulla stessa KB completa si possono insegnare forme
nuove e ritirare precisamente la lezione o il supporto sotto prova. Queste
ablazioni locali verificano la causalità del comportamento e la fertilità
KB-first; non amputano il mondo e non certificano da sole conoscenza reale.
Le sonde con dati artificiali portano questa etichetta anche quando passano.

La matrice non propone quindi di spegnere intere facoltà o metà della KB per
costruire una baseline. Nei casi reali i controfattuali del compito e le
variazioni dei dati sono il controllo principale. Una traccia di prova è
diagnostica aggiuntiva: attraversare due moduli non è il criterio di successo.

### 5.2 Metriche che non premiano una vetrina

- **Copertura esplorativa:** coppie con almeno una scheda eseguita / 276.
- **Copertura verificata:** coppie con comportamento corretto, controlli
  discriminanti e limiti annotati / 276. Separare esplicitamente `H` e `O`.
- **Necessità dei contributi:** quote di casi nei quali cambiare A cambia
  correttamente B e cambiare B cambia correttamente l'esito. Segnalare se un
  percorso alternativo spiega comunque il risultato.
- **Trasferimento:** casi nuovi corretti prima/dopo una lezione generale;
  domini, superfici e ordine dei turni non usati per il fix.
- **Coerenza dopo revisione:** dipendenti aggiornati / dipendenti verificati;
  è la misura che il caso del numero preferito impone fin dall'inizio.
- **Qualità epistemica:** errori fattuali, inferenze indebite e false conferme
  separati da limiti onesti, chiarimenti e timeout.
- **Costo:** latenza, letture/azioni necessarie e lavoro sprecato. Un secondo
  giro che ripete soltanto il primo non conta come guadagno.

Si registra anche `non applicabile` con motivazione, ma non si restringe
silenziosamente il denominatore per alzare il punteggio. Una casella vuota è
un risultato conoscitivo: può indicare duplicazione della tassonomia, assenza
di contenuto reale oppure un collegamento ancora da costruire.

## 6. Matrice completa delle coppie

Le tabelle seguenti sono la **rappresentazione triangolare della matrice
24×24**: una riga per ogni coppia distinta, senza ripetere A×B come B×A.
Ogni cella propone un'applicazione e il vincolo che dovrebbe renderla utile.
**Tutte le celle sono ipotesi H**, anche quando esistono sonde iniziali nel §4:
nessuna di queste 276 righe è un risultato di test dichiarato verde.

Gli esempi sono formulazioni di obiettivi: la prima attività della relativa
scheda sarà trovare conoscenze reali sufficienti. Se mancano, il caso rimane
un gap e non si inventa un mondo per certificarne la riuscita.

<!-- MATRIX_START -->

### 6.1 Lingua × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 01×02 | Risolvere «il secondo» dopo due sintagmi composti, conservando ordine e identità. |
| 01×03 | Richiamare in italiano una preferenza insegnata in inglese senza duplicarla. |
| 01×04 | Disambiguare il titolo da cercare usando il senso richiesto, non la sola stringa. |
| 01×05 | Leggere e interrogare una relazione parte/tutto nei due versi senza invertirla. |
| 01×06 | Insegnare una costruzione nuova e usarla con entità mai incluse nella lezione. |
| 01×07 | Una correzione della lettura cambia i ruoli interpretati, conservando la superficie originale. |
| 01×08 | Raggiungere lo stesso fatto attraverso traduzione, esonimo e nome composto. |
| 01×09 | Distinguere «tutti», «alcuni» e negazione nell'applicazione della medesima regola. |
| 01×10 | Distinguere una causa ipotizzata da una causa asserita nella descrizione di un guasto. |
| 01×11 | Trasferire un'analogia relazionale attraverso parafrasi che conservano i ruoli. |
| 01×12 | Leggere «se» e «solo se» senza attribuire al mondo reale la conseguenza ipotetica. |
| 01×13 | Una nuova forma di tasso lega prezzo, quantità e totale correttamente. |
| 01×14 | Interpretare intervalli e precedenze con connettivi insegnabili, senza perdere gli estremi. |
| 01×15 | «Fra questi, escluso il primo» cambia l'insieme sul quale scegliere. |
| 01×16 | Una richiesta coordinata diventa più obblighi, tutti conservati nel piano. |
| 01×17 | Insegnare un verbo per uno strumento esistente e verificarne gli argomenti. |
| 01×18 | Separare istruzione, codice e commento, collegando i nomi alle dichiarazioni giuste. |
| 01×19 | Parafrasare una proposizione conservando negazione, verso, quantità e referente. |
| 01×20 | La stessa frase come citazione, domanda o ordine determina un atto appropriato. |
| 01×21 | Distinguere «non ho provato X» da «ho provato non-X» anche nella risposta. |
| 01×22 | Un chiarimento linguistico riempie lo slot mancante e riprende la domanda originale. |
| 01×23 | Formulare una domanda che nomini precisamente il sintagma rimasto ambiguo. |
| 01×24 | Il thinking rilegge una risposta senza scambiarne parole e notazione per nuovi fatti. |

### 6.2 Discorso × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 02×03 | Trasferire una preferenza pertinente dal discorso alla memoria, ritrovandola dopo distrattori. |
| 02×04 | Interpretare «approfondisci il secondo» come acquisizione sul referente giusto. |
| 02×05 | Rispondere sul soggetto di «esso» dentro un paragrafo, anche dopo una digressione. |
| 02×06 | «Quella parola significa…» insegna sul referente discusso, non sull'ultimo token. |
| 02×07 | Correggere «il primo» aggiorna soltanto il referente indicato e i suoi dipendenti. |
| 02×08 | «E la valuta dell'altro?» riusa il bisogno e cambia soltanto l'entità. |
| 02×09 | Collegare premesse distribuite su più turni senza incorporare una digressione come premessa. |
| 02×10 | Mantenere più ipotesi di guasto mentre l'utente aggiunge osservazioni ellittiche. |
| 02×11 | Completare «e questo è analogo a…?» recuperando la relazione, non solo il tema. |
| 02×12 | Riprendere un mondo ipotetico dopo aver parlato del mondo reale senza contaminarli. |
| 02×13 | «Aggiungi anche il secondo» usa la quantità del referente corretto. |
| 02×14 | Ricostruire una sequenza temporale da eventi narrati fuori ordine. |
| 02×15 | «Preferisco l'altro, purché…» aggiorna scelta e vincolo mantenendo le alternative. |
| 02×16 | Riprendere un piano interrotto dal prossimo obbligo ancora aperto. |
| 02×17 | «Leggi quello» agisce sul file offerto, non su una stringa dell'ultimo output. |
| 02×18 | «Questa funzione» dopo due file continua a denotare il simbolo discusso. |
| 02×19 | Riassumere gli impegni ancora validi della conversazione, distinguendoli dai superati. |
| 02×20 | Riconoscere che «sì» risponde a una specifica offerta, non a tutte le questioni. |
| 02×21 | Spiegare a quale domanda risponde una conclusione e quali assunzioni erano locali. |
| 02×22 | Dopo il chiarimento, chiudere il bisogno originario senza chiedere di ripeterlo. |
| 02×23 | Proporre il prossimo approfondimento sulla questione aperta pertinente, non sull'ultima parola. |
| 02×24 | La rielaborazione riprende una questione sospesa senza creare un nuovo interlocutore interno. |

### 6.3 Memoria personale/persistente × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 03×04 | Riutilizzare una lettura conservata in una sessione successiva, mantenendone l'indirizzo. |
| 03×05 | Estrarre da una nota dell'utente un dato ricordabile e ritrovarne il passaggio. |
| 03×06 | Una lezione salva una preferenza o regola che modifica il comportamento successivo. |
| 03×07 | Correggere o dimenticare un valore cambia il richiamo senza resuscitare la versione precedente. |
| 03×08 | Interrogare una relazione personale usando gli stessi ruoli delle relazioni del mondo. |
| 03×09 | Derivare una conseguenza da un fatto personale ricordato e da una regola già nota. |
| 03×10 | Recuperare precedenti pertinenti a un problema, senza elevarli automaticamente a diagnosi. |
| 03×11 | Trasferire uno schema fra episodi conservati e riconoscere un controesempio successivo. |
| 03×12 | Simulare una scelta personale modificando un'ipotesi, lasciando intatte le preferenze reali. |
| 03×13 | Calcolare usando un valore ricordato; cambiarlo deve cambiare l'operando e il risultato. |
| 03×14 | Ricordare un appuntamento distinguendo data dell'evento e momento in cui fu comunicato. |
| 03×15 | Filtrare alternative con un vincolo personale conservato e aggiornarle se cambia. |
| 03×16 | Preparare un piano con risorse e obiettivi personali già detti, esplicitando ciò che manca. |
| 03×17 | Recuperare un percorso ricordato per leggere un file, verificando che esista ancora. |
| 03×18 | Applicare una convenzione di progetto ricordata a una nuova funzione. |
| 03×19 | Preparare un riepilogo personale usando dati pertinenti, senza inventare dettagli autobiografici. |
| 03×20 | Adattare dettaglio e formato alle preferenze ricordate, lasciandole correggere dall'utente. |
| 03×21 | Rispondere «come lo sai?» distinguendo ciò che disse l'utente da ciò che fu inferito. |
| 03×22 | Riprendere un compito sospeso quando viene fornito il dato precedentemente mancante. |
| 03×23 | Fare una domanda utile su un obiettivo ricordato senza trasformarlo in consenso ad agire. |
| 03×24 | Conservare solo rielaborazioni con sostegni, senza promuovere ipotesi generate a ricordi reali. |

### 6.4 Memoria profonda × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 04×05 | Una proposizione secondaria del passo acquisito risponde a una domanda oltre la definizione. |
| 04×06 | Una costruzione appena insegnata rende leggibile una frase della fonte acquisita. |
| 04×07 | Una nuova revisione della fonte aggiorna le risposte dipendenti, preservando la provenienza storica. |
| 04×08 | Collegare un'entità acquisita a una relazione già viva nella KB. |
| 04×09 | La premessa cercata completa una deduzione non già scritta nella fonte. |
| 04×10 | Cercare un'osservazione che distingua ipotesi concorrenti, anziché accumulare definizioni. |
| 04×11 | Usare una relazione acquisita per un'analogia su un dominio già noto, dichiarandone i limiti. |
| 04×12 | Integrare una condizione causale letta con un modello esistente, senza inventare un nesso. |
| 04×13 | Estrarre una misura con unità e usarla in un calcolo con dati residenti. |
| 04×14 | Unire luogo o data acquisiti a una gerarchia geografica o temporale già conosciuta. |
| 04×15 | Acquisire il criterio mancante per scegliere fra alternative già identificate. |
| 04×16 | Cercare un prerequisito mancante e ripianificare in base a ciò che è stato trovato. |
| 04×17 | Consultare documentazione pertinente all'esito di uno strumento e verificarne l'applicazione locale. |
| 04×18 | Una specifica esterna acquisita seleziona un contratto applicabile al codice letto. |
| 04×19 | Sintetizzare quanto acquisito conservando distinzioni e lacune, oltre il primo periodo. |
| 04×20 | Scegliere profondità e proposta di lettura secondo il bisogno espresso dall'interlocutore. |
| 04×21 | Una risposta acquisita cita indirizzo/revisione e distingue testo letto da conclusione derivata. |
| 04×22 | La lettura elimina la lacuna specifica e riattiva il turno che si era fermato. |
| 04×23 | Una lacuna reale genera una ricerca mirata; una lacuna non pertinente non la genera. |
| 04×24 | Il dream visita fonti utili alle lacune e misura domande chiuse, non pagine scaricate. |

### 6.5 Prosa/documenti × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 05×06 | Una lezione rende leggibile una nuova costruzione in un documento reale. |
| 05×07 | Una lezione successiva rivede il documento già letto senza reincollarlo. |
| 05×08 | Rispondere incrociando un claim del testo con una relazione residente. |
| 05×09 | Ricavare una conseguenza fra due proposizioni del documento che nessuna frase esplicita. |
| 05×10 | Usare una descrizione di sintomi per generare ipotesi sostenute dal modello disponibile. |
| 05×11 | Trasferire la struttura di un procedimento letto a un secondo caso compatibile. |
| 05×12 | Separare nel testo conseguenza causale, ipotesi e semplice successione narrativa. |
| 05×13 | Risolvere un problema descritto in prosa conservando ruoli numerici e unità. |
| 05×14 | Estrarre una cronologia o un contenimento spaziale da frasi distribuite. |
| 05×15 | Confrontare due testi rispetto a criteri espliciti, segnalando dati mancanti. |
| 05×16 | Trasformare un metodo descritto in passi e prerequisiti senza dichiararli già eseguiti. |
| 05×17 | Un'istruzione letta porta a un'azione verificata; il suo esito torna al documento pertinente. |
| 05×18 | Il contratto in prosa di un header vincola la forma di codice prodotta. |
| 05×19 | Riassumere il testo conservando tesi, eccezioni e quantità determinanti. |
| 05×20 | Spiegare lo stesso passo a destinatari diversi senza mutarne il contenuto. |
| 05×21 | Distinguere quanto l'autore afferma da quanto parrot0 ritiene provato. |
| 05×22 | Nominare la proposizione non compresa e riprendere la lettura dopo il chiarimento. |
| 05×23 | Far emergere dal documento una domanda utile su un prerequisito non specificato. |
| 05×24 | Una rilettura guidata cambia la comprensione verificabile, non soltanto il riassunto. |

### 6.6 Apprendimento a runtime × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 06×07 | Insegnare, ritirare e reinsegnare una forma modifica coerentemente tutti i suoi consumatori. |
| 06×08 | Insegnare una nuova relazione la rende sia interrogabile sia scopribile fra due entità. |
| 06×09 | Una regola insegnata produce conseguenze su membri reali mai citati nella lezione. |
| 06×10 | Una condizione diagnostica insegnata elimina un'ipotesi, senza confermare indebitamente le altre. |
| 06×11 | Insegnare la relazione, poi trasferirla per analogia senza fornire il quarto termine. |
| 06×12 | Una condizione insegnata cambia un controfattuale lasciando immutato il mondo reale. |
| 06×13 | Una procedura quantitativa insegnata funziona su nuovi numeri e forme linguistiche. |
| 06×14 | Un nuovo connettivo temporale cambia l'interpretazione dell'intervallo senza ricompilazione. |
| 06×15 | Insegnare un criterio di preferenza cambia una scelta con alternative invariate. |
| 06×16 | Una precondizione insegnata inserisce il passo necessario in un piano esistente. |
| 06×17 | Un nuovo schema di lettura dell'output rende utilizzabile l'esito di uno strumento. |
| 06×18 | Insegnare un contratto riusabile guida una nuova emissione di codice verificabile. |
| 06×19 | Una nuova resa insegnata cambia le parole, conservando la proposizione sottostante. |
| 06×20 | Correggere una condotta dialogica vale dal turno dopo in più situazioni equivalenti. |
| 06×21 | Insegnare che una relazione è asimmetrica impedisce un'inversione indebita in più domande. |
| 06×22 | Il chiarimento di una forma sconosciuta sblocca il bisogno originario nello stesso dialogo. |
| 06×23 | Insegnare quando chiedere evita domande inutili e fa emergere quelle informative. |
| 06×24 | Insegnare un criterio di arresto cambia la rielaborazione senza alterare il solver. |

### 6.7 Revisione × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 07×08 | Una relazione corretta scompare dalle risposte correnti ma conserva la storia pertinente. |
| 07×09 | Ritirare una premessa invalida le conseguenze dipendenti lasciando valide le prove alternative. |
| 07×10 | Una nuova osservazione ritira una diagnosi candidata senza cancellare le altre possibili. |
| 07×11 | Un controesempio ritira una generalizzazione e impedisce il suo trasferimento successivo. |
| 07×12 | Una correzione locale cambia soltanto il mondo ipotetico cui appartiene. |
| 07×13 | Correggere quantità o unità aggiorna tutti i risultati che le impiegavano. |
| 07×14 | Una nuova posizione supera quella incompatibile ma conserva contenimenti annidati compatibili. |
| 07×15 | Correggere il criterio o un dato cambia la graduatoria e ne spiega il motivo. |
| 07×16 | Una precondizione divenuta falsa riapre il piano e invalida i passi dipendenti. |
| 07×17 | Un file modificato rende obsoleta l'osservazione precedente prima di una nuova azione. |
| 07×18 | Una dichiarazione modificata aggiorna IR, usi e obblighi di build senza riusare simboli obsoleti. |
| 07×19 | Correggere un fatto cambia il riassunto, senza lasciare entrambe le versioni come attuali. |
| 07×20 | Revocare una preferenza comunicativa cambia la risposta senza compromettere quelle ancora valide. |
| 07×21 | Spiegare perché una conclusione è cambiata citando la premessa superata. |
| 07×22 | Riconoscere che un rimedio non è più necessario dopo una correzione indipendente. |
| 07×23 | Ritirare una lacuna risolta elimina l'iniziativa che continuava a riproporla. |
| 07×24 | Rielaborare dopo una revisione evita di risalvare una conclusione ormai smentita. |

### 6.8 Accesso relazionale × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 08×09 | Una relazione scoperta alimenta una deduzione con regole già vive. |
| 08×10 | Enumerare tutte le cause note compatibili, evitando la prima corrispondenza arbitraria. |
| 08×11 | Scoprire la relazione di un'analogia e usarla su una coppia non suggerita. |
| 08×12 | Consultare la relazione nel contesto corretto, distinguendo credenza attribuita e fatto reale. |
| 08×13 | Enumerare oggetti con proprietà quantitative e aggregare solo quelli pertinenti. |
| 08×14 | Risalire una gerarchia geografica per rispondere al livello di luogo richiesto. |
| 08×15 | Estrarre candidati dalla KB e filtrarli con tutti i vincoli del turno. |
| 08×16 | Cercare una procedura che soddisfi il bisogno, non soltanto che ne condivida il nome. |
| 08×17 | Un'osservazione dello strumento diventa una relazione interrogabile nei turni successivi. |
| 08×18 | Scoprire una relazione fra simboli letti e usarla per localizzare un obbligo di modifica. |
| 08×19 | Costruire una spiegazione composta da relazioni reali, senza unire frammenti incompatibili. |
| 08×20 | Scegliere la faccetta pertinente al bisogno dell'utente fra più proprietà dello stesso oggetto. |
| 08×21 | Spiegare quale portatore ha fornito la relazione e distinguere ricerca incompleta da assenza. |
| 08×22 | Se manca il portatore, nominare il limite e riprendere quando diventa disponibile. |
| 08×23 | Un collegamento reale suggerisce una domanda pertinente a un obiettivo ancora aperto. |
| 08×24 | La rielaborazione usa un nuovo portatore appena acquisito senza un catalogo compilato. |

### 6.9 Deduzione × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 09×10 | Dedurre le conseguenze di ogni ipotesi diagnostica ed eliminare quelle contraddette. |
| 09×11 | Controllare su esempi esclusi dall'induzione una regola candidata, senza dichiararla universale. |
| 09×12 | Derivare conseguenze dentro un'ipotesi senza esportarle come fatti fuori dal suo contesto. |
| 09×13 | Una conseguenza logica seleziona formula e slot prima del calcolo. |
| 09×14 | Comporre precedenze o contenimenti senza invertire relazioni asimmetriche. |
| 09×15 | Dimostrare che un candidato soddisfa ogni vincolo, distinguendo quelli ancora non verificati. |
| 09×16 | Derivare quali prerequisiti mancano e costruire un piano che li soddisfi. |
| 09×17 | Usare un risultato osservato come premessa, distinguendolo dall'effetto soltanto previsto. |
| 09×18 | Derivare un obbligo sul codice da dichiarazioni, contratti e usi effettivi. |
| 09×19 | Rendere in prosa una prova preservando le premesse necessarie e le condizioni. |
| 09×20 | Adattare il dettaglio della prova al destinatario senza ometterne una condizione decisiva. |
| 09×21 | Riferire un limite di ricerca come inconclusivo, non come negazione dimostrata. |
| 09×22 | Una premessa chiarita riattiva esattamente la deduzione interrotta. |
| 09×23 | Dalla prova bloccata nasce la domanda sulla premessa che può realmente completarla. |
| 09×24 | Il thinking aggiunge una verifica indipendente alla prima deduzione, senza autocertificarla. |

### 6.10 Abduzione × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 10×11 | Proporre un'ipotesi per analogia e mantenerla candidata finché manca la verifica. |
| 10×12 | Valutare quali conseguenze osserveremmo se ciascuna causa fosse vera. |
| 10×13 | Confrontare previsioni numeriche delle ipotesi con una misura, entro il modello disponibile. |
| 10×14 | Escludere cause incompatibili con l'ordine temporale o la localizzazione osservata. |
| 10×15 | Ordinare indagini diagnostiche secondo discriminazione, costo e rischio dichiarati. |
| 10×16 | Costruire un piano diagnostico i cui passi dipendono dagli esiti intermedi. |
| 10×17 | Eseguire una verifica e usare l'output per discriminare le cause candidate. |
| 10×18 | Localizzare una causa plausibile di build fallita e verificarla sul codice pertinente. |
| 10×19 | Spiegare più cause possibili distinguendo evidenze, assunzioni e dati mancanti. |
| 10×20 | Chiedere un'osservazione comprensibile all'utente senza prescrivere una diagnosi non provata. |
| 10×21 | Distinguere spiegazione possibile, migliore ipotesi disponibile e causa accertata. |
| 10×22 | Riformulare un problema sottospecificato chiedendo il dato che separa le ipotesi. |
| 10×23 | Scegliere una domanda che elimini almeno un'ipotesi invece di raccogliere dettagli irrilevanti. |
| 10×24 | Rielaborare la diagnosi cercando una confutazione, senza rafforzare soltanto la prima ipotesi. |

### 6.11 Astrazione/trasferimento × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 11×12 | Trasferire una struttura causale soltanto se il dominio destinazione ne sostiene le condizioni. |
| 11×13 | Indurre una trasformazione da esempi e calcolarla su valori tenuti da parte. |
| 11×14 | Riconoscere uno schema di successione e verificarlo su una serie temporale diversa. |
| 11×15 | Usare un controesempio per restringere l'insieme delle generalizzazioni ammissibili. |
| 11×16 | Trasferire uno schema procedurale fra compiti con precondizioni corrispondenti. |
| 11×17 | Una regolarità suggerita dagli esiti degli strumenti viene verificata con un nuovo esperimento. |
| 11×18 | Riconoscere una struttura di codice riusabile mantenendo distinti i contratti non equivalenti. |
| 11×19 | Spiegare un concetto per analogia dichiarando precisamente dove il confronto smette di valere. |
| 11×20 | Scegliere un'analogia dal dominio noto all'utente mantenendo la relazione pertinente. |
| 11×21 | Presentare il supporto di una regola indotta e cercarne un controesempio reale. |
| 11×22 | Un caso fallito rivela il limite dello schema e riapre il bisogno di una distinzione. |
| 11×23 | Domandare un esempio capace di distinguere due regole candidate. |
| 11×24 | Il dream produce candidati valutati su compiti esclusi dall'esplorazione, senza autopromozione. |

### 6.12 Causalità/contesti × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 12×13 | Quantificare l'effetto di una variazione solo attraverso una legge e unità compatibili. |
| 12×14 | Distinguere «avvenuto prima» da «ha causato», conservando entrambi i tipi di relazione. |
| 12×15 | Confrontare conseguenze di scelte sotto le stesse assunzioni esplicite. |
| 12×16 | Pianificare un intervento utilizzando effetti e precondizioni, non un elenco di azioni. |
| 12×17 | Confrontare effetto previsto e osservato per correggere lo stato della situazione. |
| 12×18 | Valutare le conseguenze di una modifica ipotetica prima di applicarla al codice reale. |
| 12×19 | Narrare un mondo ipotetico coerente senza introdurne i fatti nella conoscenza reale. |
| 12×20 | Distinguere ciò che l'interlocutore crede da ciò che il sistema sostiene. |
| 12×21 | Esporre l'assunzione causale necessaria, evitando di mascherare correlazione come causa. |
| 12×22 | Chiarire in quale scenario vale una premessa prima di riprendere il compito. |
| 12×23 | Proporre un'osservazione diversa da un intervento e dire quale dubbio può risolvere. |
| 12×24 | Il thinking confronta scenari alternativi senza lasciare ipotesi residue nella risposta reale. |

### 6.13 Quantità × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 13×14 | Calcolare una durata attraverso un cambio di giorno mantenendo gli estremi temporali. |
| 13×15 | Verificare budget e soglie con quantità omogenee, senza confrontare unità incompatibili. |
| 13×16 | Dimensionare risorse e passi di un piano usando quantità effettivamente disponibili. |
| 13×17 | Un valore misurato da uno strumento alimenta il calcolo con unità e provenienza. |
| 13×18 | Verificare limiti numerici e overflow previsti dal contratto di una funzione. |
| 13×19 | Spiegare il calcolo conservando passaggi, unità e precisione giustificata. |
| 13×20 | Rispondere con il dettaglio quantitativo richiesto senza nascondere un'ipotesi necessaria. |
| 13×21 | Separare valore esatto, stima e quantità sconosciuta prima di riportare un risultato. |
| 13×22 | Chiedere l'operando mancante per ruolo e ricalcolare dopo la risposta. |
| 13×23 | Individuare quale misura aggiuntiva renderebbe determinato un problema numerico. |
| 13×24 | La rielaborazione verifica un risultato con una procedura indipendente e arresto dichiarato. |

### 6.14 Tempo/spazio × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 14×15 | Selezionare alternative che rispettino finestre temporali o contenimenti spaziali. |
| 14×16 | Ordinare passi secondo precedenze e disponibilità, segnalando vincoli incompatibili. |
| 14×17 | Riconoscere che un'osservazione precedente al cambiamento non descrive più lo stato corrente. |
| 14×18 | Distinguere versione del sorgente, ordine delle operazioni e validità del risultato di build. |
| 14×19 | Produrre una cronologia leggibile senza confondere ordine del racconto e ordine degli eventi. |
| 14×20 | Risolvere «qui», «domani» e «prima» rispetto al contesto dichiarato, chiedendolo se manca. |
| 14×21 | Dire a quale tempo o luogo vale un fatto, senza presentarlo come universale. |
| 14×22 | Chiarire un estremo di intervallo e riprendere il confronto interrotto. |
| 14×23 | Proporre una verifica di attualità quando il bisogno richiede uno stato recente. |
| 14×24 | Ordinare le riletture secondo dipendenze temporali effettive, evitando revisioni obsolete. |

### 6.15 Vincoli/scelta × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 15×16 | Costruire un piano ammissibile e spiegare perché un'alternativa viola un vincolo. |
| 15×17 | Scegliere lo strumento disponibile che soddisfa bisogno, costo e permessi effettivi. |
| 15×18 | Selezionare una forma di codice secondo contratto e priorità di qualità esplicite. |
| 15×19 | Generare una risposta che rispetti insieme contenuto, numero di elementi e formato. |
| 15×20 | Negoziare requisiti incompatibili senza eliminare silenziosamente quello meno comodo. |
| 15×21 | Esporre il criterio della scelta e dichiarare le alternative non valutabili. |
| 15×22 | Chiedere quale vincolo sia negoziabile quando il problema non ammette soluzione nota. |
| 15×23 | Formulare la domanda che può cambiare la scelta, evitando raccolta indiscriminata di preferenze. |
| 15×24 | Arrestare la rielaborazione quando il costo supera il miglioramento utile dichiarato. |

### 6.16 Pianificazione × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 16×17 | Un esito inatteso dello strumento cambia il prossimo passo del piano. |
| 16×18 | Decomporre un problema di codice in obblighi su simboli, contratti, artefatti e verifiche. |
| 16×19 | Una risposta articolata soddisfa tutti gli obblighi del piano, inclusi i residui irrisolti. |
| 16×20 | Concordare lo scopo operativo prima di eseguire passi suggeriti da una richiesta ambigua. |
| 16×21 | Distinguere «previsto», «tentato» e «verificato» nel resoconto dei passi. |
| 16×22 | Riparare una precondizione mancante e riprendere il piano conservando ciò che resta valido. |
| 16×23 | Inserire una domanda informativa prima di un passo che dipende dalla sua risposta. |
| 16×24 | Il thinking sceglie ulteriori verifiche in base agli obblighi rimasti, non a un numero fisso di giri. |

### 6.17 Strumenti × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 17×18 | Una riparazione proposta produce un artefatto che viene compilato ed eseguito davvero. |
| 17×19 | Riassumere l'output dello strumento senza dichiarare riuscite azioni fallite o non eseguite. |
| 17×20 | Interpretare una richiesta di istruzioni come spiegazione, distinguendola da una richiesta di esecuzione. |
| 17×21 | Motivare una conclusione con l'esito osservato e dichiarare gli strumenti non disponibili. |
| 17×22 | Un errore operativo nominato consente un rimedio pertinente invece della ripetizione cieca. |
| 17×23 | Eseguire o proporre una verifica che discrimina alternative ancora aperte. |
| 17×24 | Rielaborare usando una nuova osservazione esterna, senza rileggere soltanto la propria risposta. |

### 6.18 Codice × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 18×19 | Spiegare una funzione a partire da IR e contratto, senza una descrizione generica del linguaggio. |
| 18×20 | Adattare una review al compito richiesto: errore, leggibilità o prestazione effettivamente valutabile. |
| 18×21 | Distinguere proprietà lette staticamente, ipotesi e comportamento verificato in esecuzione. |
| 18×22 | Un simbolo ambiguo genera un chiarimento che riattiva la modifica sul file corretto. |
| 18×23 | Individuare e proporre il test che può confutare una riparazione candidata. |
| 18×24 | La rielaborazione riesamina il codice dopo il verdetto, evitando una seconda patch identica. |

### 6.19 Realizzazione × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 19×20 | Rendere la stessa conoscenza come spiegazione breve o istruzione, preservandone i vincoli. |
| 19×21 | Un riassunto conserva incertezza e fonte, senza rendere categorica un'ipotesi. |
| 19×22 | Correggere una risposta incompleta recupera l'obbligo omesso, non soltanto lo stile. |
| 19×23 | Concludere con una domanda fondata su un residuo reale, evitando offerte automatiche. |
| 19×24 | La rielaborazione migliora contenuto verificabile o formato senza aggiungere fatti non sostenuti. |

### 6.20 Pragmatica × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 20×21 | Spiegare un limite in termini utili all'utente senza fingere incomprensione della richiesta. |
| 20×22 | Riconoscere una correzione dell'utente e chiedere il chiarimento pertinente, senza cambiare tema. |
| 20×23 | Proporre un approfondimento quando serve allo scopo, rispettando un precedente rifiuto. |
| 20×24 | Il registro richiesto resta vincolo nei giri di thinking, senza dilatare una risposta breve. |

### 6.21 Metacognizione × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 21×22 | Distinguere lacuna di conoscenza, forma, accesso o risorsa e scegliere il rimedio coerente. |
| 21×23 | Una lacuna verificata genera una domanda utile; il semplice non sapere non basta. |
| 21×24 | Il bilancio della rielaborazione misura sostegni nuovi e questioni risolte, non autoconferme. |

### 6.22 Riparazione × altre capacità

| Coppia | Applicazione da provare |
|---|---|
| 22×23 | Dal fallimento emerge una domanda discriminante, e la risposta riprende il bisogno bloccato. |
| 22×24 | Il ciclo autonomo prova un rimedio, ripone il compito e trattiene solo un miglioramento verificato. |

### 6.23 Iniziativa × rielaborazione

| Coppia | Applicazione da provare |
|---|---|
| 23×24 | Il dream seleziona una lacuna pertinente dall'agenda e si arresta quando il bisogno è soddisfatto. |

<!-- MATRIX_END -->

### 6.24 Diagonale e granularità: non perdere gli incroci interni

La diagonale contiene composizioni fra sottoabilità della stessa famiglia:
non va chiamata «nessun incrocio». Esempi da mantenere nel catalogo:

| Famiglia | Composizione interna discriminante |
|---|---|
| 01 | Traduzione × binding dei ruoli: la stessa domanda mantiene il verso dopo il cambio di lingua. |
| 02 | Ordine delle menzioni × coreferenza: «il secondo» rimane lo stesso dopo una digressione. |
| 03 | Memorizzazione × persistenza × richiamo: il valore sopravvive a un nuovo processo. |
| 04 | Indirizzamento × disambiguazione: il provider restituisce il significato richiesto. |
| 05 | Struttura argomentativa × claim: una conclusione mantiene i sostegni descritti dall'autore. |
| 06 | Lezione di forma × lezione di condotta: la forma nuova riceve il trattamento insegnato. |
| 07 | Supersessione × invalidazione: correggere una premessa rende obsolete solo le letture dipendenti. |
| 08 | Scoperta relazionale × enumerazione: il predicato scoperto permette di cercare altri membri. |
| 09 | Unificazione × congiunzione: i goal condividono lo stesso legame, senza soluzioni incoerenti. |
| 10 | Generazione di ipotesi × esclusione: il referto conserva tutte e sole le candidate compatibili. |
| 11 | **Induzione × analogia:** trasferire una regola candidata richiede conservare il suo status provvisorio. |
| 12 | Causalità × contesto ipotetico: la stessa legge opera con assunzioni locali differenti. |
| 13 | Calcolo × conversione: unità compatibili prima di applicare la formula. |
| 14 | Ordinamento × durata: un intervallo rispetta l'ordine degli eventi e i cambi di giorno. |
| 15 | Filtraggio × priorità: classificare soltanto i candidati ammissibili. |
| 16 | Scomposizione × prerequisiti: ogni sottocompito viene svolto quando i suoi sostegni esistono. |
| 17 | Esecuzione × lettura dell'esito: distinguere processo terminato da obiettivo raggiunto. |
| 18 | Comprensione IR × emissione: la forma generata soddisfa il contratto effettivamente letto. |
| 19 | Sintesi × realizzazione: abbreviare senza perdere la condizione decisiva. |
| 20 | Modello utente × formato: personalizzare senza violare il formato richiesto nel turno. |
| 21 | Self-model × verifica: una capacità dichiarata cambia stato dopo una prova negativa pertinente. |
| 22 | Diagnosi dell'arresto × ripresa: il rimedio riprende quel bisogno, non uno simile. |
| 23 | Scelta della domanda × recezione della risposta: l'indagine cambia in funzione dell'informazione ottenuta. |
| 24 | **Thinking × dream:** una lacuna del riesame guida una lettura che torna a quel riesame. |

Questa diagonale non esaurisce ogni possibile sottoincrocio. Se si vuole una
matrice atomica, si devono prima censire e separare i contratti delle
sottoabilità: dividere 11 in induzione e analogia, per esempio, porta il
catalogo a 25 famiglie e le coppie a 300. Non basta duplicare le etichette.
La revisione conserva gli ID e la corrispondenza con le schede precedenti;
non si possono confrontare percentuali di copertura su tassonomie diverse.

## 7. Capacità indirette e contingenti che il mix potrebbe far emergere

Alcune possibilità non hanno bisogno di un nuovo «modulo intelligente»:
hanno bisogno che due o più parti condividano davvero un oggetto. Le seguenti
sono **ipotesi di capacità emergenti**, con appigli reali ma senza dichiarazione
di riuscita generale.

| Capacità candidata | Composizione minima da esplorare | Appiglio presente e distinzione ancora da provare |
|---|---|---|
| **Comprensione retroattiva** | 05×07, poi +06 | `document_revision.p0t` esercita revisione dopo lezione. Da provare che cambi anche risposte e sintesi su prosa reale tenuta da parte. |
| **Memoria che orienta decisioni** | 03×15, poi +07 | Fatti personali e criteri esistono; il difetto del valore obsoleto mostra perché correggere il richiamo deve precedere qualsiasi fiducia nella decisione. |
| **Scoperta di connessioni non richieste per nome** | 08×09, poi +23 | Il catalogo dei portatori è vivo; distinguere il recupero Ghana/cedi dalla derivazione di una connessione nuova utile a una questione. |
| **Ricerca guidata dalla prova mancante** | 04×09, poi +22 | Esistono acquisizione e inferenza multipasso. La premessa cercata deve essere quella necessaria, e la nuova risposta deve usarla. |
| **Indagine attiva** | 10×23, poi +17 | `informative_action.p0t` rappresenta osservazioni discriminanti. Il salto è riconoscerne una in una situazione reale e usarne l'esito. |
| **Trasferimento procedurale** | 11×16, poi +15 | Analogie e procedure sono presenti. Occorre trasferire ruoli e prerequisiti, non sostituire i nomi in una sequenza memorizzata. |
| **Autocorrezione con ragioni** | 07×21, poi +09 | Sostegni e supersessione rendono possibile spiegare perché è cambiata una conclusione, senza inventare a posteriori una giustificazione. |
| **Insegnabilità del modo di ragionare** | 06×24, poi +21 | Gli schemi di thinking sono conoscenza. Il criterio nuovo deve modificare il processo e il suo arresto, non soltanto la frase di risposta. |
| **Apprendimento orientato allo scopo** | 23×24, poi +04 | Agenda e lacune sono appigli. Misurare bisogni risolti e transfer; acquisire molti fatti senza usarli non è questa capacità. |
| **Continuità epistemica** | 02×21, poi +04 | Un'unica questione tiene insieme ciò che si sapeva, si è chiesto e si è letto. Deve restare chiaro da dove proviene ogni cambiamento. |
| **Creatività vincolata dalla conoscenza** | 15×19, poi +12 | Una narrazione può rispettare relazioni e vincoli di un mondo; non si assume un generatore generale dal solo template creativo. |
| **Agente di codice che chiude il compito** | 16×18, poi +17 | Il ciclo build→obbligo→forma→scrittura→build esiste in un caso costruito. Il salto è reggere contratti, distrattori ed errori nuovi su progetti reali. |
| **Autodescrizione operativa** | 21×24, poi +17 | Moduli reificati e autoverifiche sono appigli. «Posso farlo» deve dipendere dalle prove e dalle risorse disponibili, anche restringendosi. |
| **Riuso cognitivo fra rappresentazioni** | 05×18, poi +09 | Il contratto in prosa dell'header può selezionare una forma di codice. Il risultato deve dipendere dalla stessa proposizione nelle due viste. |

Il test più interessante di emergenza non è assegnare un nuovo nome: è
mostrare che **una distinzione insegnata per una coppia migliora un'altra
coppia non allenata**, conservando ciò che già funzionava. Se per ogni
risultato serve una nuova regola dedicata al prompt, si è ampliato il
repertorio, ma il trasferimento compositivo resta da dimostrare.

### 7.1 Dove cercare il ponte, prima di costruire un motore

I casi già incontrati suggeriscono oggetti condivisi precisi:

| Oggetto | Parti che devono accordarsi | Difetto che evita |
|---|---|---|
| Identità del referente e ruoli della relazione | Lettore, domanda, traduzione, risposta | Un nome composto entra nella KB ma non è più raggiungibile. |
| Proposizione, contesto e verso | Deduzione, controfattuale, revisione, spiegazione | Una relazione vera viene invertita, o una supposizione diventa reale. |
| Quantità, unità e validità corrente | Memoria, calcolo, confronto, piano | Si continua a calcolare con il valore vecchio dopo una conferma nuova. |
| Claim, superficie, fonte e revisione | Prosa, memoria profonda, sintesi, autoverifica | La nuova interpretazione si accumula accanto alla vecchia senza sostituirne l'uso. |
| Questione, bisogno e obbligo | Discorso, iniziativa, acquisizione, riparazione | Si risponde a un'altra domanda o si perde quella sospesa. |
| Azione, effetto previsto, osservazione | Planner, strumento, codice, metacognizione | «Ho finito» significa solo «ho lanciato il comando». |
| Specie della relazione e contratto del portatore | Apprendimento, accesso relazionale, inferenza | Due moduli corretti separatamente non possono accordarsi sulla stessa conoscenza. |

È l'applicazione diretta del mantra #23: prima di correggere uno dei due
pezzi, cercare la proprietà della conoscenza che nessuno dei due può ancora
consultare. I piani [the-magic-of-apply](the-magic-of-apply.md),
[universal-comprehension](universal-comprehension.md) e
[integrazione-cognitiva-operativa](integrazione-cognitiva-operativa.md)
contengono già molti di questi appigli.

## 8. Dal catalogo a un banco di prova eseguibile

### 8.1 Primo giro: pochi confini, controllati bene

Questo è l'ordine proposto per la prima campagna; sono lavori futuri, non test
aggiunti da questo documento. Ogni riga mantiene la KB completa.

| Priorità | Scheda | Primo compito e criterio di uscita |
|---|---|---|
| P0 | **03×07 — aggiornamento del ricordo** | Riprodurre il transcript 7→11 del §4. Richiamo coerente dopo correzione e dimenticanza; niente conferma di una lezione non applicata. Poi varianti linguistiche e altro attributo. |
| P0 | **03×13 — memoria come operando** | Prima 7+3, poi nuovo valore, altro operatore e distrattori. La risposta dipende dal valore corrente; se manca, si chiede o si dichiara il limite. |
| P0 | **01×08 — lingua e accesso** | Partire dal sangue e da altri fatti reali già raggiungibili; cambiare lingua e forma senza cambiare entità o faccetta. Negativi reali e sintagmi composti. |
| P1 | **08×09 — relazione e conseguenza** | Trovare una catena reale nella KB completa. Una domanda naturale deve richiedere la relazione recuperata e una regola; il mero nome della relazione non basta. |
| P1 | **05×07 — reinterpretazione** | Un passo reale contiene una forma non ancora letta. Una lezione naturale cambia la successiva risposta sul medesimo passo, senza reincollarlo; il ritiro riapre il limite. |
| P1 | **04×09 — acquisizione e deduzione** | Verificare prima che manchi una premessa reale; acquisire una fonte identificata e rispondere a una conseguenza che la fonte da sola non enuncia. Registrare revisione ed errori di accesso. |
| P1 | **16×17 — osservazione e ripianificazione** | Un compito locale con due possibili esiti: il primo strumento deve cambiare il passo successivo. Due azioni sempre identiche non superano il controllo. |
| P2 | **10×23 — domanda discriminante** | Cercare una situazione reale con almeno due spiegazioni sostenute e un osservabile noto. La domanda proposta deve distinguerle; se la KB non offre l'osservabile, registrare il gap. |
| P2 | **19×21 — sintesi con status** | Sintetizzare contenuto che comprende fatto, ipotesi e limite. La sintesi non deve trasformare in certezza ciò che il materiale lasciava aperto. |
| P2 | **21×24 — riesame con bilancio** | Confrontare risposta iniziale e rielaborata: quale sostegno nuovo è entrato, quale obbligo si è chiuso, perché il ciclo si è fermato. Nessun vantaggio se c'è solo parafrasi. |

P0 parte da un difetto osservato, non da una promessa astratta. P1 privilegia
oggetti condivisi che servono a molte coppie. P2 verifica se il sistema sa
orientare l'attività e valutare i suoi effetti. La priorità riguarda il
confine da studiare: non autorizza a correggere contemporaneamente dieci motori.

### 8.2 Famiglia di prove per ciascuna scheda

1. **Base reale:** conoscenza viva sufficiente, domanda utile, risposta
   semanticamente verificata. Se manca conoscenza, gap esplicito.
2. **Contributi discriminanti:** cambiare un dato pertinente di A modifica il
   risultato di B; modificare un distrattore non lo deve modificare.
3. **Variazioni linguistiche:** sinonimo, ordine inverso, sintagma composto,
   lingua diversa dove supportata; una forma nuova può essere insegnata.
4. **Continuità:** altri turni fra premessa e uso, ripresa ellittica,
   conservazione della questione. Un riavvio è una prova separata quando la
   capacità dichiara persistenza.
5. **Revisione:** correzione, ritiro e reinsegnamento; verificare conseguenze,
   risposta e assenza di ricomparsa della versione superata.
6. **Negativi:** premessa assente, relazione nel verso sbagliato, ambiguità,
   vincolo incompatibile, strumento non disponibile, osservazione inconclusiva.
7. **Trasferimento:** casi tenuti fuori dallo sviluppo; per un operatore
   generale, almeno tre domini con conoscenza reale adeguata. Mancanza di
   archi reali resta un gap, non un motivo per fabbricarli nel test.

Non ogni scheda deve superare subito tutte le prove. Il suo stato deve però
dire quali mancano. Un caso di crescita usa lezioni in lingua naturale;
la notazione `.p0`, MCP o `!assert` non può essere spacciata per insegnamento
parlato. Le ablazioni meccaniche locali possono accompagnarlo, sempre con la
KB completa, con uno scopo dichiarato e senza spegnere il mondo.

### 8.3 Registro dei risultati proposto

La matrice di questo documento resta una mappa di ipotesi. Le future schede
eseguibili possono vivere nel sistema `.p0t` già esistente, con un indice
esterno che riporti almeno:

```text
id: mix-03-13-001
catalogo: mix-v1 / famiglie 03 e 13
sottoabilita: richiamo di valore personale -> binding di operando
classe: meccanica su KB completa, insegnamento naturale
prerequisiti: memoria corrente; interpretazione del turno; ruolo numerico
commit / fingerprint KB / profilo / provider / risorse: ...
prompt e transcript: ...
esito_semantico: ...
controllo_dato_pertinente: ...
controllo_distrattore: ...
revisione: ...
trasferimento: ...
diagnosi: componente / ponte / arbitrato / revisione / resa / conoscenza
stato: proposta / eseguita / verificata / regressione / gap / obsoleta
```

È **uno schema documentale proposto**, non una nuova sintassi `.p0t` già
supportata. Non aggiungere un secondo test runner o un secondo database di
conoscenza soltanto per questa matrice. Le schede di *connecting dots* devono
continuare a giudicare risposte naturali e conoscenze preesistenti; quelle di
crescita possono controllare anche lo stato, ma non sostituire il comportamento
con la sola presenza di tuple.

I vecchi esperimenti ermetici non entrano nello stato `verificata`, né come
baseline né come controllo di meccanica. Si archivia il loro risultato come
**obsoleto**; la nuova evidenza parte dall'esecuzione con parrot0 intero.

### 8.4 Classificare il fallimento prima di fare un fix

| Sintomo | Prima verifica | Intervento pertinente |
|---|---|---|
| A dà già il dato sbagliato nella domanda semplice | Richiamo o lettura indipendente | Correggere la classe in A; non costruire un adattatore A→B che mascheri il problema. |
| A e B funzionano, ma il dato non passa | Identità, ruoli, scope, unità, sostegni | Rendere interrogabile l'oggetto comune e riusarlo. |
| Risponde una facoltà fuori compito | Maturità e titolo del modulo secondo mantra #21 | Correggere l'arbitrato con il criterio previsto, evitando cessioni puntuali a moduli immaturi. |
| Dopo una lezione resta il risultato vecchio | Dipendenze, viste, supersessione e consumatori | Invalidazione guidata dalla conoscenza, non reset totale come cura. |
| Stato corretto, risposta sbagliata | Realizzazione e piano della risposta | Correggere frame e template insegnabili, non aggiungere una frase C. |
| Il dato o la legge non ci sono | Domanda naturale, fonti e conoscenza disponibile | Acquisizione/apprendimento appropriato oppure gap dichiarato. |
| Il solver non conclude entro budget | Distinguere incompletezza e negazione | Profilare la KB completa; non ridurla per rendere il caso verde. |

L'errore del numero preferito cade al momento nella prima riga: il richiamo
semplice è già incoerente con la conferma. Non ho modificato il codice per
risolverlo durante questo censimento.

## 9. KB-first: il mix deve essere conoscenza, non un dispatcher di coppie

Il documento non propone `if (memory && math)` o un modulo `mix_memory_math`.
Un elenco compilato di 276 combinazioni riprodurrebbe il problema iniziale
su una scala più grande. Anche mettere 276 ricette dedicate in `.p0` può
essere una scorciatoia sterile: il criterio resta insegnabilità e riuso.

Prima si cercano i contratti che ci sono già:

- `capability_provider/call` per trovare un portatore con famiglia e arità;
- `turn_form` e le letture condivise per ruoli e bisogni del turno;
- `holds_in`, claim e dipendenze per validità, contesto e revisione;
- `thinking_step`, precondizioni/effetti e viste dei piani per l'attività;
- questioni e arresti per continuità, residui e rimedi;
- frame e `response_template` per una resa della conclusione correggibile.

Il portatore del catalogo inferenziale **non è ancora un orchestratore
universale**: registrare una relazione non dimostra poter comporre qualsiasi
facoltà, né possedere costi, effetti e criteri di scelta completi. Si promuove
un contratto comune solo quando un caso reale ne mostra il pezzo mancante.

La prova di una nuova integrazione deve rispondere a queste domande:

1. Può imparare domani una forma, relazione, precondizione o politica nuova,
   usarla nello stesso binario e perderne l'effetto quando viene ritirata?
2. I due consumatori leggono lo stesso oggetto, oppure due copie che possono
   divergere dopo una correzione?
3. La regola descrive una classe di problemi o nomina gli oggetti dell'esempio?
4. La scelta aumenta ciò che parrot0 può vedere e distinguere, oppure nasconde
   dati e percorsi concorrenti per far vincere quello desiderato?

Se servono nuove clausole, rispettare i limiti effettivi del dialetto:
`KB_MAX_ARGS=4`, `KB_MAX_BODY=8`, nessun binding affidato a `naf` con variabili
libere. Si nominano pezzi intermedi e si controllano gli errori di parse al
boot. Un ponte che non viene caricato non è una capacità debole: non esiste.

## 10. Il salto a tre o più capacità

Con 24 famiglie abbiamo:

| Livello | Combinazioni senza ordine | Uso sperimentale |
|---|---:|---|
| Coppie | 276 | Trovare contributi reciproci e oggetti condivisi. |
| Triple | 2.024 | Verificare coerenza, revisione e chiusura di cicli. |
| Quadruple | 10.626 | Compiti estesi, con costi e dipendenze espliciti. |

Le due direzioni lineari delle coppie danno 552 percorsi candidati, non 552
abilità. Per triple e oltre l'ordine, i rami e i ritorni moltiplicano le forme
di processo. Non ha senso scriverne a mano tutte le pipeline possibili.

### 10.1 Triple iniziali con una ragione precisa

| Tripla | Comportamento ulteriore rispetto alle coppie |
|---|---|
| **03×07×13 — memoria, revisione, calcolo** | Il valore corretto cambia il calcolo; è la prosecuzione diretta del difetto misurato. |
| **04×09×21 — acquisizione, deduzione, provenienza** | Una nuova premessa chiude una prova e la risposta distingue fonte e inferenza. |
| **05×06×07 — documento, lezione, revisione** | Una lezione cambia l'interpretazione del testo già letto, senza reinserirlo. |
| **02×04×22 — discorso, memoria profonda, ripresa** | Un «sì» pertinente permette di acquisire e rispondere alla domanda iniziale dopo un'interruzione. |
| **10×23×17 — diagnosi, indagine, osservazione** | Un'ipotesi guida una verifica, il cui risultato cambia le ipotesi o la domanda successiva. |
| **16×18×17 — piano, codice, strumenti** | Il contratto guida l'artefatto e il verdetto di build determina se proseguire o riparare. |
| **03×15×20 — memoria, scelta, pragmatica** | Le preferenze pertinenti guidano una proposta resa per quell'utente, senza inventarne altre. |
| **11×15×21 — trasferimento, vincoli, status** | Un'analogia o regola candidata si usa soltanto entro condizioni provate e limiti dichiarati. |
| **12×19×21 — ipotesi, generazione, status** | Un racconto o una spiegazione ipotetica resta coerente e non entra come fatto nel mondo reale. |
| **06×21×24 — lezione, metacognizione, thinking** | Una condotta insegnata cambia come il riesame valuta e conclude il lavoro. |

Una tripla non è promossa perché nel trace compaiono tre nomi. Bisogna
specificare un compito che renda necessario il terzo contributo: in
03×07×13, per esempio, prima della correzione basta 03×13; dopo la correzione
la revisione deve fare una differenza osservabile. Se un paio di capacità
risolve comunque il caso, la terza era decorativa o già contenuta nella
definizione operativa di una delle altre.

Le famiglie comuni di supporto si annotano come prerequisiti. Questo evita di
chiamare «coppia pura» un ciclo che comprende già lettura, inferenza e memoria,
o di vendere come nuova tripla il semplice esplicitare la loro presenza.

### 10.2 Selezione progressiva, senza chiudere lo spazio

Si parte da triple che uniscono coppie con un oggetto comune già individuato:
valore valido, claim, questione o osservazione. Una tripla con un paio ancora
rosso può essere una sonda utile, ma non va presentata come integrazione
riuscita aggirando quel rosso. Si tengono anche alcuni casi a tre capacità
mai usati nello sviluppo: una copertura di tutte le coppie non garantisce
l'assenza di interferenze a tre.

I compiti più estesi diventano **grafi di dipendenze**: per esempio, una
richiesta apre un bisogno; il bisogno guida una lettura; una lezione rivede il
claim; il nuovo claim cambia il piano; lo strumento verifica l'effetto; la
spiegazione chiude la questione citando i sostegni. Il grafo va scoperto e
insegnato attraverso i contratti del progetto, non imposto come nuova
sequenza universale.

## 11. Rapporto con i piani esistenti e condizioni di avanzamento

Questo piano aggiunge **il censimento operativo e lo spazio combinatorio
delle prove**. Si appoggia a:

- [connecting-dots](connecting-dots.md): incroci fra conoscenze reali e rigore
  nel distinguerli da collegamenti soltanto disegnati;
- [integrazione-cognitiva-operativa](integrazione-cognitiva-operativa.md):
  oggetti comuni, continuità delle questioni, sostegni e curriculum;
- [la-rete-come-memoria-profonda](la-rete-come-memoria-profonda.md): contratto
  dell'acquisizione come capacità cognitiva;
- [the-magic-of-apply](the-magic-of-apply.md): portatori dinamici e proprietà
  della conoscenza interrogabili;
- [thinking](thinking.md) e [dream](dream.md): rielaborazione ed esplorazione,
  da valutare come attività con effetti e arresti verificabili;
- [test-engine](test-engine.md): infrastruttura unica per i futuri contratti
  eseguibili; ogni esperimento deve mantenere la KB completa.

Le condizioni di avanzamento proposte sono:

1. **Censimento mantenuto:** ogni nuova facoltà o consumer trova posto nel
   catalogo e nelle sue prove; le capacità senza evidenza restano visibili.
2. **Prima campagna P0/P1:** transcript reali, diagnosi dei rossi e controlli
   sulla revisione. Nessun test ermetico recuperato come certificazione.
3. **Un collegamento generale per incremento:** costruire la distinzione
   mancante, poi massimizzare i casi e cercare un secondo consumatore secondo
   il mantra #22. Non aggiungere una soluzione separata per ogni cella.
4. **Trasferimento misurato:** almeno una lezione su un confine modifica
   correttamente un altro incrocio tenuto da parte, con regressioni controllate.
5. **Prime triple:** richiedono un effetto del terzo contributo e mantengono
   coerenza dopo correzione, interruzione e fallimento di una risorsa.

**Stato della ricognizione precedente alla rimozione dell’interruttore:** catalogo e matrice documentati;
26 turni esplorativi eseguiti sulla KB completa; un difetto riproducibile di
aggiornamento/richiamo personale annotato; nessuna delle 276 coppie certificata
dal solo elenco; nessuna nuova facoltà implementata. I setup della suite sono ora normalizzati
alla KB viva, senza esecuzione dei test su richiesta di F. Il prossimo
esperimento concreto è il confine 03×07, seguito da 03×07×13.

## 12. Registro delle schede eseguite (campagna P0/P1, dal 13 settembre 2026 sera)

Formato di §8.3, ridotto. Ogni scheda ha un `.p0t` sulla KB viva e un commit.

### mix-03-07-001 — aggiornamento del ricordo; mix-03-07-13-001 — il ricordo corretto come operando

- **catalogo:** mix-v1, famiglie 03 × 07 (e 13 per la tripla)
- **sottoabilità:** scrittura di un valore personale con chiave di più parole →
  richiamo → operando; ritiro del valore
- **classe:** meccanica su KB completa (il valore è dell'utente, non un fatto del mondo)
- **diagnosi (§8.4, prima riga):** A dava già il dato sbagliato nel richiamo
  semplice. Causa di **componente**, non di ponte: il lettore di «my <chiave di
  più parole> is N» faceva `kb_assert` diretto invece di passare dallo scrittore
  unico dello slot (`user_value_write`), quindi 11 si aggiungeva a 7 e il
  richiamo trovava il primo. «Due consumatori leggono la stessa cosa» (§9 domanda
  2) era vero; **due scrittori** no.
- **seconda diagnosi:** «forget that my favorite number is 11» ri-insegnava il
  valore: `mod_forget` ritirava solo gli slot dichiarati in `user_slot_cue`
  (il nome). Ora ritira ogni valore personale detto, leggendo la chiave dai fatti
  `user_value`.
- **esito:** 7 → 11 → «Your favorite number is 11» → «14» → «Done — I've let go of
  your favorite number» → il calcolo non lo usa più. Controllo distrattore: un
  secondo valore («my lucky number is 4») resta.
- **resta:** dopo la dimenticanza la resa è «I don't know what your favorite is
  called» (il lettore dei possessi con nome prende la domanda): limite di resa,
  non di stato.
- **test:** `tests/p0t/conversation/mix_memory_revision.p0t` (9); rossi di
  `forget_move`, `deep_memory`, `memory_recall.it` identici su HEAD.
- **stato:** verificata (meccanica), trasferimento a un altro attributo con chiave
  di più parole osservato («lucky number»).

### mix-01-08-001 — lingua × accesso sui fatti veri

- **catalogo:** mix-v1, famiglie 01 × 08 (e 06 per le lezioni lessicali)
- **compito:** la stessa entità e la stessa faccetta in inglese, grafia britannica,
  passato prossimo e italiano: `color_of(blood, red)`, `color_of(grass, green)`,
  l'autore di *Hamlet* e dell'*Odissea*.
- **misura iniziale:** «di che colore è il sangue?» → «Rosso.»; «what color is
  blood?» → «I am missing the value for blood» (**falso**: il valore c'era);
  «what colour is grass?» idem; «chi ha scritto amleto?» → «Non so ancora
  tradurre «scritto»».
- **diagnosi:** (1) **regressione di questa giornata** (gen514): il controllo del
  tipo di risposta («what *color* …» chiede un colore) accettava solo `color(red)`,
  non `category_member(color, red)`; la lettura vera veniva scartata. (2) La
  risposta prioritaria sulla lacuna parlava anche su un turno che una lettura
  l'aveva. (3) Mancavano due distinzioni linguistiche, non fatti del mondo.
- **cure:** `answer_fits_type/2` legge la categoria e gli alias (grammar.p0); la
  lacuna non si dichiara su un turno con lettura (turn-frames.p0); **lezioni
  parlate salvate**: «colour and color name the same thing», «x has written y means
  x wrote y», «the italian for written is scritto», «the italian for hamlet is
  amleto».
- **transfer:** «who has written the odyssey?» → «Homer.» senza lezione
  sull'Odissea; «what colour is the sky?» → «Blue.».
- **negativo:** «what color is zorblat?» non riceve un colore.
- **seguito (mix-01-08-002):** «what currency does ghana use?» scartava «cedi»
  perché nessun fatto dice che il cedi è una valuta. Conoscenza generale e vera: i
  valori di una relazione di valuta sono valute — `value_class(Rel, Classe)`
  (valuta, città capitale, lingua) letto da `answer_fits_type`. «what currency is
  used in japan?» → «Yen.», «which currency does brazil use?» → «Real and brazilian
  real.».
- **mix-01-08-003:** «what language does brazil speak?» → «c, python.» (misclaim
  preesistente, verificato su HEAD). Il nome «language» portava a `language_of`,
  che non ha fatti, e il turno cadeva nel lettore che enumera la classe unaria
  `language/1` degli esperti di programmazione. Chi riconosce l'entità guarda le
  menzioni, che sono una vista sui fatti. La prima cura (dare il nome della
  relazione al cassetto dei fatti) rispondeva giusto ma portava il turno da 0,3 a
  **37 secondi**, e l'ho committata con il test rosso per timeout: corretta nel
  commit seguente con la superficie della domanda e il suo argomento d'ingresso
  (`answer_frame_input_arg("what language does", language_of_country, 1)`), che
  il lettore induceva già. «Portuguese.» 1,1 s, «Japanese.» 1,3 s.
- **resta:** per un paese che la KB non conosce («what language does zorbia
  speak?») l'enumerazione della classe risponde ancora «c, python.» — arbitraggio
  del lettore delle classi, da curare a parte; «what is the language of ghana?» →
  «Nobody that I know of.»; «what color is a zebra?» → «I can't show that.».
- **test:** `tests/p0t/crossing/mix_language_access.p0t` (7);
  `question_direction` e `prose_triage` verdi.
- **stato:** verificata sulla KB viva.

### mix-08-09-001 — la proprietà di una classe arriva al membro, con il suo perché

- **catalogo:** mix-v1, famiglie 08 × 09 (e 06 per le proprietà insegnate)
- **compito:** «is a whale warm-blooded?», «does copper conduct electricity?»:
  nessun fatto lo dice del soggetto; lo dicono la sua classe (`is_a`, `is_a_t`) e
  una proprietà di classe (`has_prop`). La KB aveva già entrambe le metà e
  `inherits/2`: **la deduzione c'era, l'accesso no** («I don't understand that
  yet.»).
- **cura:** `kb/core/property-questions.p0`, sola lettura — superficie →
  proprietà (EN/IT), soggetto dalla tassonomia anche al plurale, resa della prova
  («Yes: a whale is a mammal, and every mammal is warm-blooded.»). Il **No** solo
  con la proprietà opposta tenuta da una classe del soggetto («is a whale
  cold-blooded?» → No, perché ogni mammifero è a sangue caldo); «do whales lay
  eggs?» non riceve né sì né no (nessuna classe lo dice, e i monotremi rendono
  falso «nessun mammifero depone uova»).
- **crescita con fatti veri, parlando:** «birds are typically warm-blooded»,
  «amphibians are typically cold-blooded», «fish are typically cold-blooded». La
  lezione salva plurale e superficie (`has_prop(birds, warm-blooded)`): il lettore
  li riconduce leggendo lo stesso fatto (`class_has_property/2`), senza copie.
- **transfer:** «is a toad cold-blooded?», «is a penguin cold-blooded?» → No,
  «is a salmon warm-blooded?» → No, su membri che nessuna lezione nominava.
- **test:** `tests/p0t/crossing/mix_property_inheritance.p0t` (7).
- **stato:** verificata sulla KB viva.

### mix-08-09-14-001 — il contenimento geografico per la strada che la KB conosce

- **catalogo:** mix-v1, 08 × 09 con la famiglia 14 (spazio)
- **misura:** «where is tokyo?» → «Japan.» ma «is tokyo in asia?», «is paris in
  europe?», «is paris in spain?» → «I don't understand that yet.»; i passi c'erano
  in tre cassetti (`located_in`, `capital_of_country`, `continent_of`).
- **cura:** `kb/core/place-questions.p0` — la catena a uno o due passi e la sua
  resa («Yes: Tokyo is in Japan, and Japan is in Asia.»). **Nessun No**: una città o
  un paese possono stare su due continenti e la KB non li tiene tutti; se la strada
  porta altrove si dice dove («Not as far as I know: Madrid is in Spain, and Spain
  is in Europe.», «Not as far as I know: Paris is in France.»).
- **test:** `tests/p0t/crossing/mix_place_containment.p0t` (4); rossi di
  `geographic_location.p0t` (4, tempi e un caso) identici su HEAD.
- **resta:** i nomi di più parole («is new york in the united states?»), e la
  frase non geografica («is the cat in the box?») resta ai lettori di prima.
- **stato:** verificata sulla KB viva.

### mix-13-08-001 — una quantità per esemplare, per il numero di esemplari chiesto

- **catalogo:** mix-v1, 13 × 08
- **misura:** «how many legs do three spiders have?» → «A spider has 8 legs.»
  (risposta che sembra giusta e ignora il numero chiesto); «how many days are in
  two weeks?» → «A week has 7 days.».
- **cura:** `numeric-questions.p0` §5, zero C: il numero del turno (cifre o parola),
  la cosa che lo segue anche al plurale, l'unità nominata, `quantity/3` e una
  moltiplicazione, con la resa del conto («24 legs: 3 spiders with 8 legs each.»).
- **transfer:** «how many hours are in three days?» → 72; «how many legs do 5 cats
  have?» → 20. La domanda singolare resta al lettore di prima.
- **test:** `tests/p0t/math/rate_total.p0t` 3 → 6; rossi di `quantity` e
  `wordproblem` identici a HEAD.
- **stato:** verificata sulla KB viva.

### mix-12-09-001 — la catena delle cause, nel verso detto

- **catalogo:** mix-v1, 12 × 09 (e 06 per le cause insegnate)
- **misura:** «what does rain cause?» → «Wet ground.» e «what causes slippery
  surfaces?» → «wet ground.», ma «does rain make surfaces slippery?» → «I don't know:
  nothing I hold says rain made surfaces slippery», con la catena in KB.
- **cura:** `kb/core/causal-questions.p0`, zero C: gli estremi nominati nel turno
  (atomi di più parole letti con `atom_words`, anche al plurale), la catena fino a
  tre passi **nel verso della frase**, resa per passi. Nessun No.
- **crescita con fatti veri, parlando:** «burning fossil fuels causes pollution»,
  «deforestation causes soil erosion». Transfer: «does burning fossil fuels cause
  rising seas?» → tre passi (combustibili → inquinamento → cambiamento climatico →
  innalzamento dei mari) su una catena che nessuna lezione nominava intera.
- **costo e diagnosi:** la prima stesura enumerava tutte le coppie di nodi prima
  della catena e finiva il budget sulla catena a tre passi; riordinata (causa →
  effetti raggiungibili → effetto nel turno), 2,2 s. Una seconda stesura aveva un
  predicato a 5 argomenti: il caricatore lo segnala, la sonda ora lo vede.
- **test:** `tests/p0t/crossing/mix_causal_chain.p0t` (3); `cause.p0t` verde,
  `cause.it.p0t` rosso identico su HEAD.
- **stato:** verificata sulla KB viva.

### mix-02-08-09-001 — la domanda ellittica eredita la proprietà appena chiesta

- **catalogo:** mix-v1, 02 × 08 × 09 (tripla: discorso, accesso, deduzione)
- **misura:** «is a whale warm-blooded?» → sì; «and a snake?» → «I don't understand
  that yet.».
- **cura:** nella colla di `discourse.p0`, che dichiara già
  `elliptical_reference_policy(inherit_previous)`: la domanda piena registra quale
  proprietà è stata chiesta (`property_asked/2`, di sessione, non salvato); un turno
  con apertura ellittica («and», «what about», «e invece») e senza proprietà
  propria la eredita. «and a snake?» → «No: a snake is a reptile, and every reptile
  is cold-blooded.»; «what about an eagle?» → sì.
- **diagnosi del percorso:** registrarla come `exchange/3` nella contabilità
  (prima della risposta) invalidava le viste che lo leggono; e mettere le due
  letture dietro un predicato comune faceva finire il budget alla domanda piena
  («is a toad cold-blooded?» → muro). Regole separate, costo invariato (0,2 s).
- **terzo contributo necessario (§10.1):** senza il discorso la seconda domanda non
  ha proprietà; senza la tassonomia non ha classe; senza la proprietà di classe non
  ha verità.
- **test:** `mix_property_inheritance.p0t` 7 → 10; `ellipsis_reference` verde,
  `continuation` rosso identico su HEAD.

### Stato della campagna (13 settembre 2026, 21:34–22:34) e da dove riprendere

**Schede verificate sulla KB viva:** 03×07 e 03×07×13 (memoria personale, revisione,
calcolo); 01×08 in tre gradini (lingua, grafia, tempo verbale; valuta; lingua di un
paese); 08×09 (proprietà di classe ereditate); 08×09×14 (contenimento geografico);
13×08 (quantità per esemplare × numero); 12×09 (catene causali); 02×08×09 (la prima
tripla: ellissi sulla proprietà). Otto file `.p0t` sulla KB viva, nessun esperimento
di coding (indicazione di F.: il coding si incrocia in sessioni future).

**Crescita della KB con fatti veri, detti parlando e salvati:** «colour and color name
the same thing», «x has written y means x wrote y», «the italian for written is
scritto», «the italian for hamlet is amleto», «birds are typically warm-blooded»,
«amphibians are typically cold-blooded», «fish are typically cold-blooded», «burning
fossil fuels causes pollution», «deforestation causes soil erosion». Più la
conoscenza generale scritta come lettura: `value_class/2`, `hazard`-like
`opposite_property/2`, i passi di contenimento.

**Il reperto trasversale.** Quasi ogni cella rossa **non era una capacità assente**:
la conoscenza e la regola c'erano (`inherits/2`, `causes/2` a catena, `continent_of`,
`quantity/3`), e mancava la **lettura che le porta alla domanda**. È la riga «A e B
funzionano, ma il dato non passa» di §8.4, e la cura è stata sempre una lettura in KB
(zero C, salvo lo scrittore unico di 03×07). Due volte la cura corretta nel
comportamento ha rotto il **costo** (37 s per un `relation_noun`, budget esaurito per
un predicato comune): il costo è parte del criterio d'uscita di ogni scheda.

**Costo (misurato alla fine):** `basics.p0t` [taxonomy] 1.13 → 1.20 s, [antonym] 1.24 → 1.33 s: i nuovi lettori sono famiglie di candidati provate a ogni turno. Prima della prossima scheda, profilare e far fallire ogni candidato su una cue del turno prima di enumerare (TEST_TODO).

**Da riprendere (P1/P2 di §8.1, senza coding):**
1. 05×07 reinterpretazione di un passo già letto dopo una lezione;
2. 04×09 acquisizione e deduzione (una premessa reale mancante, una fonte);
3. 10×23 domanda discriminante su una situazione reale con due spiegazioni;
4. 19×21 sintesi che non trasforma un'ipotesi in certezza;
5. i residui annotati: la lingua di un paese ignoto («c, python»), «what is the
   language of ghana?», i nomi di più parole nel contenimento, il seguito
   dell'ellissi per le cause. **«what is the language of japan?»** → «Japanese.» con la superficie «language of». **Non riuscito:** insegnare parlando «the language of ghana is english» (vero) → «I don't understand that yet.» (la lacuna è registrata): nessuna forma di lezione scrive `language_of_country`, e «what language does ghana speak?» → «c, python.» resta il misclaim del paese senza fatto. **Fatto alle 22:31 (mix-02-08-09-14-001):** il
   seguito ellittico per i luoghi, con la stessa colla — «is tokyo in asia?» → «and
   madrid?» → «Not as far as I know: Madrid is in Spain, and Spain is in Europe.»,
   «what about beijing?» → «Yes: Beijing is in China, and China is in Asia.».

## 13. Seconda campagna — casi pratici, non fatti da manuale (F., 13 settembre 2026, 22:34)

> F.: «per le prove non usare fatti stereotipati come capitali, regioni, geografia
> spicciola o fatti spiccioli delle materie di riferimento: usiamo contenuti più
> originali e utili per casi pratici».

### mix-12-09-08-15-001 — si possono mescolare questi due prodotti?

- **catalogo:** 12 (causalità) × 09 (deduzione) × 08 (accesso per classe) × 15
  (giudizio di sicurezza), con 06 per le lezioni.
- **fonte:** Washington State Department of Health, *Dangers of mixing bleach with
  cleaners* — candeggina + ammoniaca → cloramine; candeggina + un acido (aceto) →
  cloro gassoso; candeggina + alcol → cloroformio; effetti respiratori.
- **misura:** «is it safe to mix bleach and vinegar?» → «Who or what does «it» refer
  to?»; «mixing bleach and vinegar releases chlorine gas» non entrava (nessuna
  relazione a tre posti si poteva insegnare).
- **cure:** una forma di lezione a tre posti («mixing A and B releases C», anche «and
  an <classe>») in `messages.p0`; il lettore `kb/core/mixture-safety.p0`: miscela
  nota anche per la classe di un prodotto, prodotto ricondotto all'atomo dei fatti
  causali, danno noto → «No: …» con la strada; domanda aperta («what happens if I
  mix…») → «Careful: …»; senza danno noto si dice solo che cosa si libera; senza
  miscela nota la regola tace (mai «è sicuro»). Il lettore delle conseguenze
  sconosciute non dice più «non so che cosa succede» su una miscela nota.
- **lezioni parlate salvate:** le tre miscele, «chlorine gas causes breathing
  problems», «chloramine gas causes breathing problems», «chloroform causes
  dizziness», «vinegar is an acid».
- **quarantena:** «chloroform causes organ damage» letto come
  `causes(chloroform, organ)` — tolto; il danno detto con un sostantivo composto
  finale si perde (lettore da guardare).
- **esito:** «can i mix ammonia and bleach?» → «No: mixing bleach and ammonia
  releases chloramine gas, and chloramine gas causes breathing problems.»; «is it
  safe to mix bleach and vinegar?» → passa per «vinegar is an acid»; «what happens if
  i mix bleach and alcohol?» → «Careful: … chloroform causes dizziness.».
- **resta:** «is it safe to mix flour and water?» → «Who or what does «it» refer
  to?» (la riparazione del pronome, non una risposta); prodotti di più parole
  («hydrogen peroxide») e le miscele non insegnate.
- **test:** `tests/p0t/crossing/mix_household_safety.p0t` (5).

### mix-13-14-08-15-001 — quante ore fuori dal frigorifero

- **fonte:** USDA FSIS, *«Danger Zone» (40°F–140°F)*: mai più di 2 ore fuori dal
  frigorifero (1 ora sopra i 32 °C).
- **misura:** «I left cooked chicken out for 3 hours, is it safe?» → «Hmm, I don't
  know about cooked yet.».
- **cure:** una forma di lezione («X should not be left out for more than N hours»)
  e il lettore `kb/core/food-time.p0`: ore dette nel turno (cifre o parole), alimento
  e sua classe (anche detta al plurale nella lezione), confronto con il limite. Oltre:
  «No: …, so it is safer to throw it away.»; entro: «It is within the limit: … Put it
  in the fridge now.» — mai «è sicuro».
- **lezioni parlate salvate:** «perishables should not be left out for more than 2
  hours», «chicken / rice / milk / fish is a perishable».
- **transfer:** il pesce per quattro ore, che nessun esempio usava.
- **integrazione adattiva (mix-13-14-08-12-15-001):** la temperatura detta nel turno
  cambia la regola e il verdetto. Lezione parlata salvata: «above 32 degrees
  perishables should not be left out for more than 1 hour». «I left the chicken out
  for 2 hours» → entro il limite; «… for 2 hours at 35 degrees» → «No: at 35 degrees a
  perishable like chicken should not be left out for more than 1 hour; …».
- **resta:** una frase senza domanda («the milk was
  left on the counter for one hour») cade nel lettore delle affermazioni.
- **test:** `tests/p0t/crossing/mix_food_time.p0t` (4).

### mix-20-16-21-001 — un'ustione, e il rimedio sbagliato

- **fonti:** NHS, *Burns and scalds* (acqua fresca corrente 20 minuti, togliere
  gioielli non attaccati, pellicola appoggiata, niente creme/oli/burro, pronto
  soccorso se grande, profonda, sul viso, chimica o elettrica); NHS, *Burns and
  scalds – treatment* e Croce Rossa (niente ghiaccio né acqua ghiacciata).
- **misura:** «I burned my hand on the stove» → «Hmm, I don't know about burned
  yet.»; «should I put butter on a burn?» → «Hmm, I don't know about butter yet.».
- **cure:** la situazione `user_burn` con le sue mosse (user-situations.p0) e il
  piano insegnato parlando; una forma «never put X on a Y» (⚠ non «do not put…»:
  l'ausiliare in testa fa leggere una domanda) e `kb/core/remedy-questions.p0`, che
  dice il no e **la prima mossa dello stesso piano** al posto.
- **lezioni parlate salvate:** le quattro mosse del piano; «never put butter on a
  burn», «never put ice on a burn».
- **esito:** «I scalded myself with boiling water» e «mi sono bruciato un dito col
  ferro da stiro» ricevono il piano (in italiano); «can I put ice on a burn?» → «No:
  do not put ice on a burn. Instead: Hold the burn under cool running water for 20
  minutes, as soon as you can.».
- **quarantena:** `pending_gap_failed(burned)`.
- **test:** `tests/p0t/crossing/mix_first_aid_burn.p0t` (4).

### mix-13-15-001 — quanto consuma e quanto costa un apparecchio

- **misura:** «how much does it cost to run a 2000 watt heater for 3 hours at 0.25
  euros per kwh?» → **«1996.75.»**; «how many kwh does a 2000 watt heater use in 3
  hours?» → **«1997.»**: aritmetica sui numeri del turno presi alla rinfusa, un
  numero falso detto con sicurezza.
- **cura:** `numeric-questions.p0` §6, zero C: ogni numero ha il ruolo che la parola
  dopo dichiara (potenza in watt, durata in ore, prezzo in valuta per kWh) e un fatto
  vero regge la conversione (un kilowattora = 1000 watt per un'ora). La resa dice il
  conto intermedio.
- **esito:** «1.5 euros: 6 kWh at 0.25 per kWh.»; «6 kWh: 2000 watts for 3 hours.»;
  transfer «a 60 watt bulb left on for 10 hours» → 0,6 kWh; «a 1500 watt kettle for 2
  hours at 0.30 euros a kwh» → 0,9 euro.
- **test:** `tests/p0t/math/rate_total.p0t` 6 → 8; `arith` e `wordproblem` rossi
  identici a prima.

### mix-16-14-13-001 — quando partire, quando cominciare

- **misura:** «if the roast needs 90 minutes and dinner is at 8 pm, when should I
  start cooking?» → «Hmm, I don't know about minutes yet.»; «the meeting is at 9 am
  and the drive takes 1 hour 20 minutes, when should I leave?» → **«23:59»** (la
  durata prendeva anche il 9 dell'orario: 9 ore e 1 minuto).
- **cure (time-questions.p0, zero C):** l'orario con am/pm (mezzogiorno e
  mezzanotte come eccezioni); le domande di pianificazione a ritroso («when should I
  leave/start») come operatore «prima»; la durata letta **per posizione** — il
  numero seguito dalla sua unità — che il file stesso annunciava come «la forma che
  verrà», con la lettura per unità presenti come ripiego.
- **esito:** 18:30; 07:40; il treno delle 7:45 con 25 minuti a piedi → 07:20;
  «2 hours 45 minutes after 14:30» → 17:15 invariato.
- **test:** `rate_total.p0t` 9 → 12; `time_date_complex` verde (28).

### mix-10-23-001 — le foglie gialle: le cause, la domanda che le separa, la risposta

- **catalogo:** P2 di §8.1, 10 × 23 (con 12 e 02), su un caso pratico.
- **fonti:** University of Maryland Extension, *Overwatered indoor plants*; guide di
  cura (troppa acqua: foglie gialle e terreno fradicio; poca acqua: foglie gialle e
  terreno secco; poca luce).
- **misura:** «why are my plant's leaves turning yellow?» → «Hmm, I don't know about
  plant's yet.».
- **cure:** una forma di lezione «a sign of X is Y» (`sign_of/2`) e
  `kb/core/diagnosis.p0`: l'effetto nominato con più cause, le candidate, e la
  domanda sui segni che le distinguono (`findall` e i segni di due cause diverse);
  la domanda fatta registra l'effetto (di sessione) e la risposta con un segno
  («the soil is soggy», «the soil feels very dry») restringe a quella causa, con il
  perché. Nessuna causa si afferma senza il suo segno.
- **diagnosi del percorso:** «the soil is soggy» (copula, niente «?») cedeva come
  prosa portata: la forza del turno si materializza prima dei `turn_span_token`,
  quindi l'eccezione legge i token di superficie.
- **lezioni parlate salvate:** tre cause delle foglie gialle, due segni.
- **seguito:** la domanda nomina tutti i segni noti («do you see leggy stems, dry soil
  or soggy soil?»); lezione salvata «a sign of low light is leggy stems» (guide sulle
  piante con poca luce: steli lunghi e sottili, foglie pallide) con la causa di più
  parole ricondotta all'atomo; «the stems look leggy» → «That points to low light».
  **Quarantena:** «the stems are long and leggy» era stato letto come enumerazione
  chiusa (`condition_closed_world(stem)`, `stem(long)`, `stem(leggy)`) e salvato:
  tolto; il lettore delle enumerazioni prende una descrizione per un elenco.
- **resta:** «leggy stems is a sign» (accordo), e la frase descrittiva che il lettore
  delle enumerazioni cattura.
- **test:** `tests/p0t/crossing/mix_diagnosis_question.p0t` (4).

### mix-15-13-001 — quale confezione conviene

- **misura:** «which is cheaper: 750 g for 3 euros or 1 kg for 3.80 euros?» → «Hmm, I
  don't know about cheaper yet.».
- **cura:** `numeric-questions.p0` §7, zero C: le offerte «N <unità> for P» nel turno,
  le conversioni vere all'unità di base (1 kg = 1000 g, 1 l = 1000 ml), il prezzo
  per unità, il confronto; resa al chilo o al litro, al centesimo (l'aritmetica non
  arrotonda: la parte intera si ottiene con `mod`, che lavora sugli interi).
- **esito:** «1 kg for 3.80 is cheaper: 3.8 per kilo against 4 per kilo.»; «1.5
  liters for 5 is cheaper: 3.33 per liter against 4 per liter.»; «2 kg for 5 euros or
  500 g for 1.20» → la confezione piccola.
- **test:** `rate_total.p0t` 12 → 14.

### mix-08-16-14-001 — a che ora andare a letto

- **fonte:** NHS — la maggior parte degli adulti sani ha bisogno di circa 7-9 ore di
  sonno a notte.
- **misura:** «I wake up at 6:30, when should I go to bed?» → «I don't understand that
  yet.».
- **cure:** forma di lezione «X need N to M hours of sleep» e, in
  `time-questions.p0`, la finestra a ritroso dall'ora della sveglia con la durata
  presa dalla conoscenza: una **finestra**, perché la raccomandazione è un
  intervallo; se l'utente dice la durata («want 8 hours of sleep») vale la sua.
- **lezione parlata salvata:** «adults need 7 to 9 hours of sleep».
- **esito:** «Between 21:30 and 23:30: adults need 7 to 9 hours of sleep.»; «get up at
  5 am» → tra le 20:00 e le 22:00; con 8 ore dette → 23:00.
- **test:** `rate_total.p0t` 14 → 16; `time_date_complex` verde.

### mix-13-08-002 — le tazze della ricetta in grammi, per ingrediente

- **fonte:** King Arthur Baking, *Ingredient Weight Chart* — 1 tazza: farina 00/all-purpose
  120 g, zucchero semolato 198 g, fiocchi d'avena 113 g, latte 227 g.
- **misura:** «how many grams are 2 cups of flour?» → «I don't understand that yet.».
- **cure:** forma di lezione «a cup of X weighs N grams» e `numeric-questions.p0` §8:
  tazze (cifre, parole, «half a cup»), ingrediente, peso per tazza, moltiplicazione e
  il perché.
- **lezioni parlate salvate:** i quattro pesi per tazza.
- **esito:** «240 grams: a cup of flour weighs 120 grams.»; «half a cup of sugar» →
  99 g; «the recipe says 3 cups of oats» → 339 g; «2 cups of sand» → nessun numero
  inventato.
- **test:** `rate_total.p0t` 16 → 19.

### mix-08-13-15-001 — la carne ha raggiunto la temperatura sicura?

- **fonte:** USDA FSIS, *Safe Minimum Internal Temperature Chart* — pollame 165 °F
  (74 °C), carne macinata 160 °F (71 °C), tagli interi di manzo e maiale 145 °F (63 °C).
- **misura:** «I cooked the chicken to 68 degrees, is it safe?» → «Hmm, I don't know
  about cooked yet.».
- **cure:** forma di lezione «the safe internal temperature of X is N degrees» (nome
  anche di più parole) e, in `food-time.p0`, il confronto dei gradi del turno con la
  soglia dell'alimento: sotto → «Not yet…, 6 degrees short. Keep cooking and measure
  again in the thickest part.»; sopra → raggiunta, «as long as you measured it in the
  thickest part» (non «sicuro» in assoluto).
- **lezioni parlate salvate:** pollo 74, carne macinata 71, braciole di maiale 63.
- **composizione:** nello stesso file, «I left the chicken out for 3 hours» continua a
  rispondere con la regola del tempo: le due regole leggono lo stesso alimento.
- **test:** `mix_food_time.p0t` 7 → 9.

### mix-01-12-15-001 e mix-01-13-14-15-001 — gli stessi casi pratici in italiano

- **misura:** «posso mescolare candeggina e aceto?», «ho lasciato il pollo fuori per 3
  ore, è sicuro?» → «Non capisco ancora.».
- **cure:** lessico insegnato parlando e salvato («the italian for bleach is
  candeggina», vinegar/aceto, ammonia/ammoniaca, alcohol/alcol, acid/acido); cue
  italiane nei due lettori (anche le domande sì/no «posso mescolare», «è sicuro
  mescolare»); resa italiana delle due risposte con i nomi nella lingua quando la KB
  li sa.
- **esito:** «No: aceto è un acido, e mescolare candeggina e un acido libera chlorine
  gas, e chlorine gas causa breathing problems.»; «No: pollo è deperibile, e un
  alimento deperibile non deve stare fuori dal frigorifero più di 2 ore; 3 ore sono
  oltre, quindi è più sicuro buttarlo.».
- **seguito:** una forma sorella con lo span (`teach_tr_phrase`) insegna i nomi di più
  parole: «the italian for chlorine gas is cloro gassoso», «… breathing problems is
  problemi respiratori», «… chloramine gas is clorammine». Ora «No: aceto è un acido,
  e mescolare candeggina e un acido libera cloro gassoso, e cloro gassoso causa
  problemi respiratori.». **Quarantena:** «gas di clorammina» era diventato
  `gas_of_clorammina` (il «di» canonicalizzato dentro l'oggetto della lezione): tolto;
  la canonicalizzazione tocca anche il testo italiano che si sta insegnando.
  `translate.p0t` rosso identico su HEAD.
- **test:** `mix_household_safety.p0t` +1 blocco, `mix_food_time.p0t` +1 blocco.

### mix-20-16-21-002 — il sangue dal naso, e la testa all'indietro

- **fonte:** NHS, *Nosebleed* — seduti, piegati in avanti con la bocca aperta; stringere
  il naso sopra le narici 10-15 minuti respirando con la bocca; non piegare la testa
  indietro; aiuto urgente se dura più di 10-15 minuti, molto sangue, colpo alla testa,
  debolezza o capogiri.
- **misura:** «my nose is bleeding» → **«Got it: your nose is bleeding.»** (e salvava
  `called(nose, bleeding)`: il nome di una cosa).
- **cure:** due situazioni in `user-situations.p0` — il sangue dal naso, e la domanda con
  la testa all'indietro (distinta per una cue sua) — e i due piani insegnati parlando.
- **esito:** «my son has a nosebleed, what do I do?» → i passi; «should I tilt my head
  back for a nosebleed?» → «No: don't tilt your head back, the blood runs down your
  throat. Sit down and lean forward…»; «mi sanguina il naso» in un processo nuovo → i
  passi in italiano.
- **quarantena:** `called(nose, bleeding)`.
- **test:** `tests/p0t/crossing/mix_first_aid_nosebleed.p0t` (4).

### mix-13-15-12-001 — l'umidità di casa contro l'intervallo raccomandato

- **fonte:** US EPA, *A Brief Guide to Mold, Moisture and Your Home* — tenere l'umidità
  relativa interna sotto il 60%, idealmente fra il 30% e il 50%.
- **misura:** «the humidity in my bedroom is 70 percent, is that a problem?» → la
  situazione «guasto» («Let's look at it together…»), e la lezione successiva veniva
  letta come dettaglio di quel guasto e non entrava.
- **cure:** forma di lezione «X should stay between N and M percent» e
  `numeric-questions.p0` §9 (sopra/sotto/dentro, con i numeri); la situazione
  «guasto» cede a una misura contro un intervallo noto; **una lezione non è la
  risposta alla domanda aperta** (`lesson_surface_cue` in user-situations.p0); la
  frase senza «?» non cede come prosa.
- **lezione parlata salvata:** «indoor humidity should stay between 30 and 50 percent».
- **esito:** 70 → «That is high: … and 70 is above that.»; «my hygrometer says the
  humidity is 45 percent» → «That is fine…»; 25 → «That is low…».
- **quarantena:** `quantity(my_hygrometer_says_the_humidity, percent, 45)` (il lettore
  delle grandezze prendeva la frase), `pending_gap_failed(humidity)`.
- **resta:** la conseguenza (la muffa): «high humidity causes mould» non entra come
  lezione causale.
- **test:** `rate_total.p0t` 19 → 21.
