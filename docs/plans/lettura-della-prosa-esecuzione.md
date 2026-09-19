# Comprensione della prosa — percorso esecutivo

Parte operativa di [lettura-della-prosa.md](lettura-della-prosa.md), aggiornata
il 18 settembre 2026 sulla revisione `636ce2a2`. Prima di modificare il progetto
leggere `MANTRA.md` e `PRINCIPLES.md`. Questo documento specifica lavoro ancora
da fare: i criteri di uscita non sono capacità già certificate.

## 1. Obiettivo e prove ammissibili

Comprendere un testo significa conservare ciò che afferma, chi lo afferma e
a quali condizioni; collegarne le parti; rispondere a domande nuove con una
giustificazione riconducibile al testo; distinguere informazione assente,
ambigua, negata, ipotizzata e derivata. Contare frasi o estrarre la prima
relazione non soddisfa questo contratto.

Il progetto deve poter imparare una **forma** parlando, applicarla al testo
già osservato e perderne gli effetti quando la lezione viene ritirata. Non
insegnare la risposta alla domanda usata per misurare comprensione.

Tre livelli di evidenza, da non sommare:

| Livello | Prova | Che cosa autorizza a dichiarare |
|---|---|---|
| Meccanica | Contrasti controllati, anche inventati, insegnamento e ablazione | Il meccanismo cresce a runtime |
| Lettura | Testo reale con fonte, domande fissate prima, confronto prima/dopo lettura | Le informazioni indicate sono state comprese in quel testo |
| Trasferimento | Testi reali tenuti da parte, forme e domini differenti, nessuna lezione specifica sui loro contenuti | La classe si generalizza nel perimetro misurato |

Nessun insieme finito prova «qualunque prosa». Riportare lingue, generi,
costruzioni e limiti misurati; evitare un punteggio soggettivo unico verso 100.
Il cancello storico delle parole resta necessario per certificare un piolo,
ma non sostituisce il trasferimento né assolve una risposta falsa.

## 2. Stato accertato e inventario da riusare

La [sonda di questa ripresa](../labs/prose-ladder/2026-09-18-ripresa-generale/README.md)
conferma un muro sulla frase del compost e un costo superiore a 1 s. Non
rimisura r300/r320/r340 e non certifica i test documentali elencati qui:
l'inventario è una verifica del codice e dei contratti dei test.

| Oggetto | Implementazione esistente | Limite da verificare prima di estendere |
|---|---|---|
| Nodi, superficie, ruoli, intervalli | `kb/core/input-structure.p0`: `input_node/4`, `input_node_surface/3`, `input_node_role/3` | Identità e offset devono restare coerenti fra clausola e documento |
| Lettura semantica e commit | Stesso file: `input_assertion_bundle/2`, `input_frame_commit/3`, `semantic_binding/3`, `proposition_source/4` | Un bundle unico e una coordinazione binaria non equivalgono a tutte le proposizioni di un periodo |
| Prosa incollata | `src/brain/99-registry.c`: `prose_learn_lead`, `compound_turn_lead` | Lo splitter e i ritorni ricorsivi non sono automaticamente il percorso documentale |
| `read:` e acquisizione | `src/brain/30-generation-reading.c`: `extract_clause` | Pubblica `current_prose` e osserva unità; resta un fallback ai lettori privati |
| Testo trattenuto | `kb/core/text-structure.p0`, scope `last_text` | Meta/struttura non dimostrano che ogni proposizione sia stata acquisita |
| Documento e ordine | `kb/core/document-structure.p0`: `document_unit/3`, `document_unit_source/3`, `rhetorical_edge/4` | Riusare gli ID; non creare un secondo registro delle frasi |
| Attribuzione e revisione | `kb/core/document-claims.p0`: `claim_status/2`, `claim_frame/4`, `claim_normalization_gap/2`, `reading_depends_on/2`, `reading_stale/2` | Verificare collegamento alla prosa incollata e a tutte le dipendenze introdotte |
| Contesti e credenze | `kb/core/context-scope.p0`, `claims.p0`, `epistemic-status.p0` | Una frase riportata non autorizza un fatto globale |
| Discorso, argomenti, procedure | `kb/core/discourse.p0`, `document-argument.p0`, `document-method.p0` | Un consumatore nuovo deve ricevere la lettura comune, non ricostruirla |
| Domande qualificate | `kb/core/grammar.p0`: `question_qualifier/2`; `src/brain/10-memory-knowledge.c` | La guardia sulla superficie del valore non sostituisce un qualificatore del fatto |

Sonde esistenti da **leggere**, poi selezionare solo il contrasto pertinente:
`tests/p0t/language/{prose_compost,prose_relation_scope,taught_question_qualifier,document_rhetoric,document_claims,document_argument,document_revision,document_revision_scale}.p0t`.
Molti hanno timeout lunghi: sono debito, non modelli da copiare. Non lanciarli
tutti per «vedere come va».

Il sorgente corrente dichiara `KB_MAX_ARGS=4`, `KB_MAX_BODY=16` in `src/kb.h`;
la tabella storica di AGENTS riporta ancora 8 per il corpo. Per questo lavoro
preferire aiutanti con corpi entro 8 goal e non dipendere dall'ampliamento.
Ricontrollare l'header prima di implementare; non confondere arità e numero
di variabili. `naf` non lega variabili libere. Leggere anche il contratto di
`findall` in `docs/parrot-p0-syntax.md` prima di raccogliere superfici o bundle.

## 3. Ordine di esecuzione

```text
E0 costo della sonda → E1 identità e percorsi → E2 proposizioni composte
                                            ├→ E3 qualificatori e contesti
                                            └→ E4 riferimenti fra frasi
E2 + E3 + E4 → E5 composizione delle risposte → E6 trasferimento
```

Ogni consegna è un traguardo, non necessariamente una sessione. Le sottoparti
si eseguono in ordine, **un solo circuito per sessione**. La revisione dopo
una lezione e l'ablazione si provano già in ciascuna consegna: non rimandarle
a un ultimo grande rifacimento. Se E0 blocca, si consegnano profilo e prossima
ipotesi falsificabile; non si certificano gli stadi successivi.

### E0 — Rendere praticabile il ciclo di apprendimento

**Ingresso:** la frase completa e la domanda nel referto allegato. **Prima
azione:** riprodurre una volta senza trace/debug, separando boot e turni.
Il profilo separato corrente vede 3.052 chiamate a `phrase_canon`, 34 a
`turn_bookkeeping`, 12 ricostruzioni d'indice; `turn_teaching_offer` è il
predicato più costoso. Non è ancora dimostrato quale sia la causa primaria.

1. Localizzare produttori e chiamanti di **uno** di questi costi con `rg`;
   scegliere quello su cui una singola ipotesi può eliminare lavoro ripetuto.
2. Prima di proporre una cache, consultare `materialized_view/2` e
   `view_depends/2`. Non congelare una relazione dipendente dal turno come se
   fosse lessico immutabile; distinguere le chiavi di conoscenza e osservazione.
3. Se si cambia una cache, insegnare e ritirare una locuzione nella stessa
   sessione: la risposta deve cambiare immediatamente nei due versi. Confrontare
   anche l'esito senza accelerazione, senza togliere la KB.
4. Confrontare prima/dopo sullo stesso input e stato iniziale, senza altri
   banchi in esecuzione. Misurare separatamente il costo dell'invalidazione.

**Uscita:** sonda e domanda sotto 1 s, senza perdita semantica, con crescita
e ablazione ancora funzionanti. Se c'è solo un miglioramento parziale,
registrarlo come tale: non alzare il budget, non aprire un secondo circuito.
Nessuna diminuzione della KB, nessuna lista di locuzioni compilata.

### E1 — Un documento riconoscibile da tutti i percorsi

**Ingresso:** inventario sopra; `document_rhetoric.p0t` e
`document_revision.p0t` come contratti da riusare.

1. In sessioni separate con la stessa KB, presentare lo stesso testo breve
   reale come prosa incollata e con `read:`. Ripetere con una frase sola e
   con il brano intero: la prima è una sonda, il secondo misura la lettura.
2. Confrontare unità, superfici, offset, lingua, frame, fonti, gap e risposte.
   Non pretendere lo stesso ID letterale: verificare che ogni oggetto abbia
   una corrispondenza stabile e che i join restino nello stesso documento.
3. Scrivere la tabella «oggetto nel percorso A → oggetto comune → percorso B».
   Il primo disaccordo riproducibile è **l'unica** modifica della sessione.
4. Usare `document_unit` e gli osservatori esistenti; se manca un ponte,
   aggiungere un adattatore meccanico che consegna la stessa osservazione
   alle regole KB. Non instradare ogni asserzione ordinaria alla lettura di
   documenti: la distinzione di atto deve restare insegnabile.

**Uscita:** stessa informazione interrogabile nei due ingressi, nessuna
clausola perduta, fonte e range verificati contro il testo originale; un
secondo documento e una domanda interposta non mescolano le fonti. Una lezione
e la sua ablazione aggiornano il testo trattenuto senza reinviarlo.
Ambiguità conservata come gap, nessun nuovo fatto inventato.

### E2 — Comporre proposizioni senza confondere gli argomenti

**Caso iniziale:** la frase del compost del referto. La risposta completa
deve conservare triturazione, aggiunta d'acqua e aerazione tramite rivoltamento;
«shredding the plant matter» da solo è parziale. Non promuovere il gerundio
automaticamente ad agente del passivo.

**E2a, una sessione:** delimitare principale e un modificatore dalla IR;
entrambi restano interrogabili. Prove: relativa ridotta, apposizione,
modificatore fra soggetto e verbo, frase semplice di controllo.

**E2b, sessione successiva:** associare argomenti e ruoli al predicato locale.
Prove: passivo con agente, passivo con mezzo espresso da un'azione, gerundio
che è oggetto legittimo. Il caso «gathering a mix» già perso nel giro storico
è il contrasto obbligatorio. Non riattivare in blocco le regole ritirate
`np_closer ← gerund_of` e passivo dalla radice.

**E2c:** coordinazioni tramite gli oggetti E2a/b, mantenendo le distinzioni
fra elenco distributivo, azione collettiva e alternative. Non assumere che
ogni «and» o «or» autorizzi due fatti indipendenti.

**File di partenza:** `input-structure.p0`, `grammar.p0`, `turn-frames.p0` e
i due produttori di E1. Preferire regole sugli span e sui ruoli osservati;
una riscrittura in stringa seguita da nuovo parsing non chiude la migrazione.

**Uscita di ciascuna sottoparte:** più proposizioni compatibili convivono;
letture incompatibili restano alternative, mai fatti cumulati; i vecchi
contrasti del compost non perdono informazione. La forma nuova si insegna e
si ritira parlando; ogni fatto conserva lo span che lo giustifica.

### E3 — Conservare a quali condizioni una proposizione vale

**E3a, priorità iniziale:** le due stime del valore delle barriere coralline
in r300 e la domanda «in 2020». Seguire `strip_annotation_parentheticals` e
il punto in cui si proietta il valore: non basta impedire la cancellazione,
bisogna legare ciascuna data **alla propria stima**.

Riusare identità della proposizione e contesto; non attaccare la data soltanto
al soggetto o a un generico fatto binario. Conservare valore, unità e fonte.
Un fatto qualificato non diventa automaticamente una verità senza condizioni.

**Contrasti prima della cura:** due valori con due date; ordine invertito;
data non presente; domanda senza data; stesso valore in due fonti. Le varianti
inventate provano solo la meccanica, il brano reale prova la lettura.

**E3b e seguenti, una distinzione per sessione:** negazione e relativo ambito;
ipotesi/condizionale contro asserzione; quantità e quantificatori; attribuzione
e conflitto fra fonti. Riutilizzare `claim_status`, `claim_commitment` e
`context-scope.p0`. Una negazione non si rappresenta soltanto con l'assenza
di un fatto; «non tutti» e «nessuno» non sono equivalenti.

**Uscita:** la domanda consulta il qualificatore della proposizione. La data
presente risponde col valore pertinente, quella assente produce un limite
esplicito; il conflitto resta visibile. Un connettore o qualificatore nuovo
appreso a runtime modifica la lettura ed è reversibile senza perdere le fonti.

### E4 — Collegare i riferimenti attraverso il testo

**E4a:** «It» e «The resulting mixture» in r320: confrontare soggetto
esplicito e ripresa anaforica. Prima cercare i referenti già rappresentati in
`discourse.p0`, senza creare una seconda pila dell'ultima entità.

Conservare menzione e candidato antecedente come oggetti distinti. Se più
candidati soddisfano i vincoli KB, mantenere l'ambiguità: la semplice vicinanza
non prova la coreferenza. Vincoli di compatibilità e preferenze devono essere
insegnabili, non una lista di pronomi nel C.

**E4b:** sintagmi annidati e nomi italiani: locuzione intera, testa, modificatori
e concetto canonico devono essere condivisi da lettura e domanda. Usare i100
senza riscriverlo in inglese e senza duplicare il lettore per lingua.

**Contrasti:** antecedente unico; due candidati; cambio di argomento; entità
vicina incompatibile; possesso diverso; singolare/plurale; domanda italiana
su testo inglese e viceversa. Le forme non ancora leggibili restano gap.

**Uscita:** risposta con referente corretto e menzione di supporto; nessun
legame arbitrario nei casi ambigui. Una regola appresa rivede il collegamento
già osservato, e l'ablazione rimuove solo le conclusioni che dipendono da essa.

### E5 — Rispondere componendo ciò che è stato letto

**Prerequisiti:** identità E1, composizione E2 e le distinzioni E3/E4 richieste
dal caso. Consultare `document-argument.p0`, `document-method.p0` e le porte
`turn_response/2`, `turn_plan_candidate/1`, prima di creare un consumer.

Ordine delle sottoconsegne, una per sessione:

1. Risposta a una relazione che attraversa due frasi, con entrambi i supporti.
2. «Perché» e «come»: distinguere causa, evidenza, scopo e mezzo. Un arco
   retorico non rende da solo valida un'inferenza causale.
3. Confronto o domanda coordinata: costruire tutti i sottogoal, dichiarare
   quelli mancanti e non rispondere soltanto al primo.
4. Riassunto: selezionare proposizioni del documento, mantenendo negazioni,
   attribuzioni e condizioni; ogni punto deve avere supporto ricostruibile.

**Uscita:** due consumatori riusano la medesima lettura; nessun accesso al
grezzo per ricostruire gli argomenti. Distinguere «il testo dice», «ne segue
con questa regola» e «la KB sa già» anche quando i contenuti coincidono.
Togliere una premessa o ritirare la regola deve far cessare l'inferenza,
senza cancellare le osservazioni. Test reali per *connecting dots*: non
fabbricare un micro-mondo con già dentro la conclusione attesa.

### E6 — Misurare il trasferimento e scegliere il prossimo limite

Prima di implementare una classe scegliere un testo di sviluppo e testi
tenuti da parte con fonti e attese congelate. Alla conferma usare almeno tre
testi reali di domini diversi, includendo italiano ed inglese. Se uno viene
usato per diagnosticare o insegnare, dichiararlo passato allo sviluppo:
conservarne il risultato e scegliere un nuovo testo per una futura conferma.

Estendere progressivamente i generi: descrizione enciclopedica, narrazione,
istruzione/procedura, argomentazione, testo misto con formule o codice.
Questa è una matrice di copertura da riempire, non cinque parser da costruire.

Per ogni testo riportare separatamente:

- corrette, parziali, muri onesti, false e già rispondibili a freddo;
- merito, meta, struttura e cancello storico, senza cambiare il denominatore;
- proposizioni/domande coperte e quelle ancora senza una lettura;
- prove di fonte, crescita/ablazione e revisione del testo trattenuto;
- durata di lettura e domanda, con profiler spento.

Il matcher per sottostringa del banco è un aiuto al triage. Rivedere le ✓
contro ruoli, verso, quantità, condizioni e completezza. Non modificare
un'attesa dopo aver visto l'output per renderlo corretto; un errore dell'oracolo
si corregge con fonte e motivazione, mantenendo traccia della misura vecchia.
Il conteggio strutturale va controllato sul testo: condividere le stesse
regole di confine del lettore non costituisce un oracolo indipendente.

**Uscita:** relazione di copertura, nessuna nuova risposta falsa nei contrasti,
trasferimento misurato senza insegnare fatti dei brani. Se il banco supera
i limiti di durata, consegnare «non certificato» e il profilo; una sonda breve
verde non autorizza a dichiarare compreso il piolo. Nessuno stadio chiude
definitivamente la comprensione generale: il primo nuovo controesempio apre
la prossima classe, conservando i risultati e i limiti precedenti.

## 4. Procedura per l'agente che riprende

1. Leggere mantra, principi, questa roadmap e il referto della consegna scelta.
   `git status --short`; non cambiare branch e non usare stash.
2. Scrivere prima della modifica: sintomo, attesa semantica, contrasto negativo,
   ipotesi unica, vista IR coinvolta, possibile controprova. Non basta «manca
   il passivo»: indicare il primo oggetto che perde il ruolo o il supporto.
3. `make build`, poi chat con profilo completo, rete spenta e sessione non
   persistita. Una sessione per il ciclo; processi nuovi solo per confronti
   che richiedono lo stesso stato iniziale. Mai `/save` sui fatti del brano.
4. Provare la lezione in lingua naturale, usando esempi di insegnamento già
   documentati nei test. Se servono nomi di predicati o tuple, il canale manca:
   specificare il ponte necessario, non chiamare apprendimento un `!assert`.
5. Se il turno supera 1 s: una profilazione, registrazione del costo e scelta
   esplicita di lavorare quel circuito. Oltre 2 minuti interrompere il comando
   e conservarne lo stato; nessun timeout lungo può certificare un test nuovo.
6. Implementare una sola ipotesi. Se è una lezione, persistere solo la
   conoscenza generale verificata, nella sua sede, senza i fatti del brano.
   Se serve C, spiegare quale operazione meccanica mancava e quale parte della
   decisione resta insegnabile; vietato un nuovo scanner di lingua.
7. Verificare una volta il contrasto mirato finale, con lezione/ablazione e
   trasferimento pertinente. Non lanciare la suite; `make soft-test` solo
   quando risponde a un dubbio concreto e dopo averne letto il contratto.
8. Consegna: diff rivedibile, transcript, misura, limite residuo e prima azione
   successiva. Non dichiarare «migrazione» senza il bilancio C/KB e il percorso
   realmente dismesso; non dichiarare «generale» senza trasferimento.

Scheda obbligatoria di consegna (compilare, non lasciare formule vaghe):

```text
Consegna/sottoparte:
Revisione e modifiche presenti; profilo e configurazione:
Testo reale + fonte + frase/span; oppure prova meccanica dichiarata:
Domanda; attesa; risposta prima; risposta dopo:
Prima divergenza osservata (ingresso → IR → commit → query → resa):
Ipotesi testata; prova che avrebbe potuto smentirla:
Lezione naturale; ablazione; effetto sul testo già trattenuto:
Contrasto negativo; trasferimento; capacità precedenti preservate:
Tempo senza profiler; profilo separato se lento:
Bilancio C/KB; test eseguito una volta; verifiche non eseguite:
Stato: chiuso / parziale / bloccato, con evidenza:
Prossima azione concreta; file/simbolo; risultato discriminante atteso:
```

### Stato di E0 — 19 settembre 2026 (chiuso sulla sonda; E0b aperto sul brano intero)

Stessa frase del compost, macchina scarica, profiler spento, misure singole
(la variabilità fra esecuzioni è di circa 200 ms):

| Passo | Turno di lettura | Domanda | Che cosa è cambiato |
|---|---:|---:|---|
| sonda iniziale (`636ce2a2`) | 3.181,8 ms | 1.622,6 ms | — |
| `phrase_canon` una volta per canonicalizzazione (`d7dd25c0`) | 2.650,6 ms | 1.542,5 ms | 3.052 chiamate → fuori dalla testa del profilo |
| `turn_teaching_offer` come domanda di esistenza | 2.075–2.265 ms | 1.530,1 ms | 540 ms in una chiamata → fuori dal profilo |
| indice del motore sul 1° e 2° argomento (`pred_bucket_a0`, `src/kb.c`) | 1.729 ms (con /debug) | 1.402 ms (con /debug) | fatti visitati 9,3 M → 3,0 M, passi identici (150.323) |
| `np_closer` sul turno: condizione ground prima del verbo (`grammar.p0`) | 1.664 ms (con /debug) | 1.193 ms (con /debug) | `np_closer` 210 → <5 ms; nella domanda 237 → 5 ms |
| `lemma_candidate/2` con `concat_atoms` inverso invece di `chars`+`append_list` (`morphology.p0`) | 1.599–1.638 ms | 1.383 ms | ~34.000 goal `append_list` in meno nei due contabili |
| hash del fatto calcolato una volta in `fact_make` (`src/kb.c`) | **1.117 ms** | **872 ms** | `fact_index_rebuild` ri-hashava ~59.000 fatti a ogni retract (8,6 ms × ~50 per turno) |
| `kb_match`: niente giro fatto per fatto a caccia di variabili senza fatti non-ground; indice nel percorso veloce | 1.010 ms | 797 ms | 9,6 M `term_contains_var` in meno |
| censimento: `nonground` e hash del predicato calcolati in `fact_make` | 886 ms | 632 ms | `pred_stats_rebuild` era il 20% del turno |
| `kb_hypothesis_best` sui secchi del censimento | 828 ms | 592 ms | 1,2 s su 21.000 chiamate in un paragrafo |
| retract incrementale (`kb_compact`: indice e censimento riscritti solo per la coda spostata) | **511 ms** | **479 ms** | ~800 ricostruzioni complete per paragrafo |

**Uscita di E0 sulla sonda: raggiunta.** Frase e domanda sotto 1 s, risposte
identiche in ogni A/B (14 turni vari e brano r320 + 3 domande), insegnamento e
ritiro in sessione verificati: una costruzione insegnata cessa col ritiro e
una nuova vale subito, una glossa insegnata entra in `tr_phrase_surface`
(`tests/p0t/language/tr_phrase_view.p0t`). `make soft-test` 3,6 s; resta solo
[antonym] «Held», preesistente.

**E0b, aperto — il brano intero.** Il paragrafo di r320 incollato in un turno
(2.185 byte, 31 frasi) costava **19,4 s**; ora **11,7 s**, dopo i passi di
sopra più `tr_phrase_surface` (vista con `view_apply_resolved`) e
`verb_reading_form` come vista. Resta il costo in ordine: `input_frame_observe`
~1,7 s (31 chiamate; il tempo sta dentro un `findall` su `input_semantic_frame`,
~12.000 goal a frase, ~6 µs per goal); `answer_frame` ~1,5 s (599 enumerazioni
intere dal C); `phrase_canon` ~0,7 s (3.885 enumerazioni); `phrase_boundary`
~0,7 s; contabili ~0,6 s; fuori dal solver ~4,7 s.
Tentativi misurati e scartati: vista su tutta `phrase_canon` e cache C con
chiave sul timbro (−3%, rumore); vista su `answer_frame` (peggiora: 11,7 →
12,4 s, si ricostruisce più spesso di quanto risparmi); vista `turn_word_form`
scaldata prima dei contabili (peggiora); riordino di `turn_effect` (193 → 700 ms).

**Nota sugli strumenti.** «fatti visitati» in `/debug` somma il secchio anche
quando il goal ground salta la scansione (hash esatto): è un limite superiore,
non il lavoro fatto. Il profilo C si legge con gprof su un binario separato
(`-pg -fno-inline`: con l'inlining i chiamanti risultano sbagliati).

**Profilo C (gprof, 19 settembre).** Fuori dal solver restavano ~900 ms per
turno. Il 60% del tempo profilato era `fact_hash` + `fact_index_rebuild`: ogni
`kb_retract*` ricostruiva l'hash esatto dei fatti e ri-hashava le stringhe di
tutti. Con l'hash conservato nel fatto le risposte sono identiche (A/B su 14
turni) e ogni turno scende del 30–45%. `make soft-test`: 4,6 s; [taxonomy], che
prima andava in timeout a 1,41 s, ora passa; resta solo l'antonimo «Held»,
preesistente.

Esiti invariati: la frase e la domanda danno ancora i due muri (E2 non è
cominciata). Le offerte di forma sono intatte: il primo muro di «zilvan brinks
torvo» porta ancora costruzione e relation verb, nell'ordine giusto. Il rosso
di contenuto in `teaching_offer_shape.p0t` è registrato in `TEST_TODO.md`.

**Il costo seguente, misurato contabile per contabile** con una misura temporanea
nel ciclo di `turn_bookkeeping` (tolta dopo l'uso): due soli contabili pesano,
`diagnosis` ~193 ms e `read_topic_named` ~179 ms; gli altri 32 insieme stanno
sotto i 10 ms. Entrambi fanno generate-and-test su tutta la KB contro il turno
tramite `words_in_turn/2`, la cui seconda clausola prova `lemma_candidate/2`
(scomposizione in caratteri con `append_list`) su ogni token del turno, e
questo **per ogni candidato**. `diagnosis` si attiva perché «turning» è una
`diagnosis_cue`.

- **Tentativo scartato:** riordinare `turn_effect/2` (parole dell'effetto prima
  della seconda causa) porta `diagnosis` da 193 a 700 ms; è stato annullato.
- **Ipotesi successiva, falsificabile:** calcolare le forme del turno (parola e
  lemmi) **una volta per turno**. Il meccanismo c'è già: `materialized_view`
  con `view_depends` su `turn_span_token`, come `scenario_claim`. Però una vista
  interrogata dentro una prova (`frame_depth > 0`) non si materializza: viene
  ri-derivata dalle regole. Serve quindi un punto meccanico nel C, dopo la
  pubblicazione dei token e prima dei contabili, che scaldi le viste marcate
  come sporche. Quali viste scaldare resta deciso dalla KB. Da misurare: quanto
  costa ricostruire per turno le viste già dipendenti dal turno.
  Il risultato discriminante atteso: `diagnosis` + `read_topic_named` sotto
  50 ms, senza cambiare le risposte.

**19 settembre, mattina — l'indice sugli argomenti.** Il contatore temporaneo
delle visite per predicato del goal ha mostrato che il costo dei contabili era
di **scansione**, non di passi. I goal con il primo o il secondo argomento
legato ma non interamente ground (`intent_cue(bound, _)` 2,9 M visite,
`tr(_, bound)` da `canonical_value/2`, `relation_verb`, `singular`) non
passavano dall'hash dei fatti ground e scorrevano il secchio intero. Nel
motore ora c'è un indice pigro (hash dell'argomento → posizioni in ordine di
inserimento; ordine SLD e prima soluzione invariati). Si sospende per i
predicati con una variabile nuda in quella posizione, si spegne con
`PARROT0_NO_ARG_INDEX=1`, e non contiene vocabolario.
A/B sullo stesso binario e sulla stessa KB, 14 turni vari (insegnamento di
relation verb, passivo, «mar nero», compost): **risposte identiche**, turni
più veloci dell'8–12%. I rossi di `basics.p0t` e `facts.p0t` sono identici a
indice spento, quindi non vengono dall'indice.

Tentativo scartato nella stessa mattina: una vista `turn_word_form/2` scaldata
prima dei contabili (`kb_views_warm`). Ricostruiva anche le altre viste
sporche (`view_pair` 30 chiamate) e i contabili non miglioravano, perché il loro
costo non era nei passi.

`np_closer` è chiuso: la clausola `relation_verb($V), naf(turn_mentions_word(…))`
rivalutava la condizione ground per ognuno dei ~290 verbi a ogni enumerazione.
Il tentativo di materializzarla non si attiva, perché la chiusura delle
dipendenze passa per `tr/2`, la cui regola in `gloss.p0` usa `apply`, e
`kb_view_dependencies` rinuncia. Resta `input_frame_observe` (~80–150 ms in una
chiamata). Fuori dal solver restano ~860 ms,
con 12 ricostruzioni d'indice.

**Prossima azione già determinata:** E1 (stesso testo come prosa incollata e
con `read:`), usando la frase del compost come sonda e il brano r320 come
lettura. In parallelo, quando serve: E0b, il costo di `input_frame_observe` —
quale clausola di `input_semantic_frame` spende i ~12.000 goal per frase. Se il
profilo della revisione nuova cambia, aggiornare la priorità con quella
misura invece di difendere questa diagnosi.
