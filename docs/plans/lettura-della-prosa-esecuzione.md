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

### Il ciclo di misura è di nuovo praticabile — piolo r320, 19 settembre 10:21

`scripts/prose-rung.sh r320` (lettura del brano + 68 domande con «who
answered?») gira in **87 s**, contro i 10–20 minuti in background dei giorni
precedenti. Referto `docs/labs/prose-ladder/referti/r320-2026-09-19-1021.txt`:
**merito 19/68**, meta 2/2, struttura 5/5, cancello non superato (105 parole
contro più di 319 richieste). È la previsione registrata dopo il ritiro della
guardia sul gerundio, ora misurata. Diff con `-2318`:
- **+4**: «Gathering a mix» (×2), «providing nutrients», «plant nutrients»,
  cioè le risposte che la guardia ritirata faceva perdere;
- **−1**: «what aids the decomposition process?», che prima dava «shredding the
  plant matter» (una delle tre, via il passivo dalla radice) e ora mura.
  È il bersaglio di E2b;
- da rivedere a mano: `knowledge` risponde «how much of the waste in landfill…»
  con un elenco sul rame (**possibile bugia**); «what is added to the plant
  matter» riceve la definizione di «matter».
Le ottimizzazioni di E0/E0b non hanno perso risposte rispetto allo stato atteso.

### E2b e i furti del piolo — 19 settembre, tarda mattinata

- ⚠ **Correzione di F. sul passo qui sotto (19 settembre, 10:43).** Una
  risposta di `answerframe` è **specie B**, cioè un frasario: uno schema di
  superficie (`extract_frame`, «@O is aided by @S») che scrive un fatto
  ritrovato per cue. La direzione voluta è la **specie A**: la IR del testo
  (`input_semantic_frame`, ruoli e span) letta da regole KB che compongono la
  risposta. Il passo qui sotto è quindi un guadagno di punteggio, **non** di
  comprensione, e contraddice la regola scritta in E2 («una riscrittura in
  stringa seguita da nuovo parsing non chiude la migrazione»). Il passo giusto
  per E2b è un frame `passive` in `input-structure.p0`, con i ruoli
  agente/mezzo sugli span osservati, e `event_subject_verb/1` come conoscenza
  che decide il ruolo del complemento con «by». Lo schema di stringa aggiunto
  qui sotto si tiene o si ritira secondo la decisione di F.; non è la base su
  cui costruire E2c.
- **E2, la relativa (13:47–13:57): provata e annullata, con la causa trovata a monte.**
  Una regola `input_relative_proposition/3` (dopo una punteggiatura, un
  `relative_opener/1` seguito dal verbo; l'antecedente è l'entità prima del relativo;
  gli oggetti sono quelli di `coord_object/4`) forma la proposizione. Ma su «Coral
  belongs to the phylum Cnidaria, which includes sea anemones and jellyfish.» la IR
  **non ha nessuna entità** per «the phylum Cnidaria»: né il sintagma, né «phylum»,
  né «cnidaria». C'è solo «coral». L'antecedente quindi non si trova, e comunque
  non coinciderebbe con la domanda del banco («the **animal** phylum Cnidaria» nel
  testo, «the phylum cnidaria» nella domanda: identità del nome per testa e
  modificatori, E4b). r300 invariato; la frase singola in chat dà un muro anche
  senza la regola (preesistente). **Prossima prova:** `!query input_node(current_turn,
  I, node(phrase, np_candidate, P), R)` e le superfici su quella frase, per vedere
  dove si chiude o manca il sintagma candidato.
- ✅ **E3 in specie A, 13:39–13:45: il complemento preposizionale come ruolo.**
  Dopo l'oggetto, una preposizione e un'entità raggiungibile si registrano accanto
  alla proposizione impegnata: `semantic_complement(binding(R, S, O), Prep, X)`.
  Non sono un fatto nuovo sul mondo. «what does S V O Prep?» è un frame di domanda
  che chiede il complemento. Visti nello stesso giro:
  - due regole scartate in silenzio per arità 5 (`PARSE ERROR` nel log del demone);
  - le domande con l'oggetto già detto non sono `qsr`;
  - la domanda sul soggetto non vale con un soggetto già detto;
  - un turno con forza di domanda non ha asserzioni binarie da leggere.
  Sul testo vero: r320 **20 → 21/68**, «what does compost reduce dependency on?» →
  «Commercial chemical fertilizers.», letta da participiale + complemento. r300
  48/62 con cancello, r340 7/65; nessuna falsa risposta A.
- ✅ **E2c in specie A, 13:25–13:38: la coordinazione degli oggetti.** Misurato
  prima di scegliere: gli elenchi «, and» sono la costruzione più frequente dei
  pioli (14 in r320, contro 0–1 relative e 1–2 complementi preposizionali).
  - «Compost supplies nitrogen, phosphorus and potassium» impegna tre
    proposizioni; «what does compost supply?» → «Nitrogen, phosphorus and
    potassium.» (dalla lettura, con `input_frame_reading_many/4`), e «what supplies
    phosphorus?» → «Compost.». La lettura distributiva è il default; una relazione
    collettiva si dichiara `collective_relation/1`;
  - l'osservazione registra anche un insieme di frame che differiscono **solo
    nell'oggetto** (è un elenco, non un'ambiguità), con l'insieme calcolato una volta;
  - un elenco vero richiede una **congiunzione** e finisce con l'entità subito dopo
    di essa, senza attraversare preposizioni; la vicinanza dell'oggetto non
    attraversa una preposizione («form some OF Earth's…» dava «Earth»).
  Pioli invariati: r300 48/62 con cancello, r320 20/68, r340 7/65; nessuna falsa
  risposta A. Visti e corretti nello stesso giro: gli elenchi troppo larghi
  («Gardens, landscaping…» per «what does compost improve?») e «Earth.».
  **Limite onesto:** l'elenco che continua dopo una preposizione («deliver
  ecosystem services **for** tourism, fisheries…») resta parziale. Serve il
  complemento preposizionale come ruolo, che è la capacità successiva.
- ✅ **Specie A, 13:00–13:08: la participiale e il passivo nella IR.**
  - «Reefs form ecosystems, displacing the sponge reefs» impegna anche
    `displace(reefs, sponge_reefs)`: la participiale ha il soggetto della principale
    (il contrasto del piano). r340 +1 dalla lettura;
  - il **passivo** è un ordine della IR (`frame_pattern(en, binary, assertion,
    passive)`): «The forest is threatened by beetles» impegna
    `threaten(beetles, forest)`, cioè la stessa relazione dell'attivo, e
    «what threatens the forest?» e «what is the forest threatened by?» leggono lo
    stesso legame. Un agente al gerundio è un mezzo, e lì la IR tace. È il
    passo di E2b in specie A che sostituisce lo schema di stringa del mattino,
    ancora presente in attesa della decisione di F.;
  - la domanda passiva legge gap e ausiliare dalla **cue** del turno («what is» è
    `segment_role`, non nodo IR); il participio in coda a un sintagma non entra
    nel nome dell'entità; un participio dopo un ausiliare passivo non è il verbo
    di un attivo (prima leggeva `threaten(forest, beetles)`).
  Pioli invariati e veloci: r300 48/62 con cancello (51 s), r320 20/68 (69 s),
  r340 7/65 (78 s). Risposte giuste dalla lettura: r300 2, r320 2, r340 3.
  **Aperto:** quando la IR impegna una frase che nessuno schema di stringa legge,
  al turno risponde un altro modulo, e la risposta non dice che la frase è stata
  letta («Hmm, I don't know about forest yet»). La lettura A deve avere la sua
  resa del turno, come «Learned: …».
- **Stato a fine giro (12:50).** Risposte giuste lette dalla IR invece che dal
  frasario: r300 1, r320 2, r340 2 («Ancient times.» dopo `tokens_contiguous/3`).
  Nessuna falsa risposta della strada A nei referti. Pioli: r300 48/62 con
  cancello (72 s), r320 20/68 (107 s), r340 6/65 (103 s). I pioli lunghi sono
  vicini al limite dei 2 minuti: il costo sta nelle domande dopo la lettura, e
  una parte nota è `analysis_family` (specie C). In parallelo, la capacità
  «domanda d'abilità su di sé» (`self-ability.p0`: lingue e ogni abilità, in
  italiano e in inglese) è nata dalla segnalazione di F. «sai parlare italiano»
  e ha il suo test.
- ✅ **Specie A, 12:15–12:35: radice preferita, sequenze senza articolo,
  commit dal record.**
  - una forma flessa dichiarata anche verbo a sé si legge come la radice («Compost
    supplies nutrients» ora si impegna);
  - `qro` vale solo senza un soggetto detto fra gap e verbo;
  - una **sequenza** di parole piene senza articolo è un'entità («hard carbonate
    exoskeletons», «coral polyps», «ancient times»), e le sue parole non sono
    più «parole non lette»;
  - il nome esclude anche `preposition/1` e `trailing_subordinator/1`: «since.» su
    r300 era una falsa risposta;
  - il commit della prosa del turno parte dal frame già registrato
    (`input_recorded_bundle/2`), senza ricalcolare: -6 s sul paragrafo.
  Pioli: r300 48/62 con cancello, r320 20/68, r340 **6/65** (+1 dalla lettura:
  «ancient times rooted.», span di una parola troppo lungo). Risposte giuste lette dalla
  IR: r300 1, r320 2, r340 1. **Costo:** `input_frame_observe` sul paragrafo di r320
  passa da 2,1 a 5,5 s (le sequenze si rivalutano a ogni domanda di vicinanza);
  i pioli durano r320 100 s e r340 108 s, contro 67 e 74. Sono sotto il limite, ma
  la prossima cosa da fare è una lista delle sequenze calcolata una volta per
  clausola, come per la vicinanza. **Fatto alle 12:38:** le entità della clausola
  si chiedono una volta per pubblicazione e si registrano (`input_entity_cached/3`),
  e `input_frame_observe` scende da 5,5 a 2,6 s. I pioli però restano r320 95 s e
  r340 108 s, mentre i turni brevi costano come prima (A/B su 14 turni): il
  tempo sta nelle **domande dopo un paragrafo letto**, con molti più fatti di
  lettura in KB. **Profilato alle 12:43** (r340 letto, poi due domande con `/debug`):
  «what is charcoal made of?» costa 292 ms; «where is wood carbonized in modern
  methods?» 698 ms, di cui **377 ms in `extract_frame`** con 40 chiamate e 40 passi.
  **Rettifica (12:47):** la vista **non** si ricostruisce. Una misura temporanea in
  `kb_views_changed` non registra nessuna invalidazione di `extract_frame`, né
  durante la lettura né durante le domande. I 377 ms sono **40 enumerazioni intere**
  della vista (un passo e ~9 ms ciascuna), fatte da `analysis_family`, la facoltà
  che ha risposto a quella domanda (specie C, già nel registro dei furti).
  Non è un costo introdotto oggi; la cura è quella del registro: migrare o togliere
  `analysis_family`, non accelerarla.
  Cosmetico aperto: la risposta A non mette la maiuscola iniziale («zilvan.»).
- ✅ **Specie A, 11:30–11:50: la prosa del turno, il soggetto, e tre false
  risposte chiuse.** Anche la prosa detta nel turno si impegna dalla IR
  (`db2e713a`). Nuove domande sul **soggetto** («what/who threatens forests?»,
  ordine `qro`). Tre false risposte della strada A viste sui pioli e chiuse in KB:
  - «hard.»: una sequenza senza articolo spezzata in nomi singoli; ora un nome
    nudo vale solo **isolato**;
  - «yet.»: una congiunzione presa per un nome; ora vale la classe dichiarata
    `function_word_class/1`, e i pioli sono anche più veloci;
  - «abundance.»: la vicinanza saltava parole piene non lette; ora fra operatore
    ed entità ci possono stare **solo parole funzionali**;
  - «reefs.»: un gap dopo una preposizione («in what water») letto come ruolo;
    ora `gap_after_preposition/2` lo esclude.
  Pioli invariati e più veloci: r300 48/62 (54 s), r320 20/68 (67 s), r340 5/65
  (74 s). Una risposta del banco viene dalla lettura (r320, «what does composting
  offer?»). `ir_reading_answer.p0t`: 10 verdi.
  **Aperto (11:55):** «Compost supplies nutrients.» non si impegna dalla IR,
  e «what does compost supply?» risponde ancora `answerframe`. La IR vede **due**
  asserzioni `binary(supply)` (misurato con `count_list` su
  `input_assertion_set(current_turn, S)`), quindi nessuna è unica. Non dipende
  dal doppio «compost» (entità nota + nome nudo: provato e annullato).
  **Causa trovata (11:58):** la KB ha sia `relation_verb(supply)` sia
  `relation_verb(supplies)`, una vecchia lezione. La IR legge due operatori,
  `supplies(compost, nutrients)` e `supply(compost, nutrients)`, e nessuna
  asserzione è unica. Provata e **annullata**: preferire la radice
  (`linguistic_form(W, W) :- relation_verb(W), naf(word_has_other_verb_root(W))`).
  «what does compost supply?» e «what supplies nutrients?» rispondevano dalla
  strada A, ma su r300 si passava da 48 a 47/62 e il cancello cadeva, perché la
  strada A rispondeva anche dove non deve. **Prima di allargare**, la strada A deve
  imparare a tacere su due classi misurate:
  1. **domanda qualificata**: «what **phylum** does coral belong to?» → «class
     anthozoa.». Il qualificatore dopo il gap (`question_qualifier/2`, già
     insegnabile) va letto anche qui, come fa la porta di `answerframe`;
  2. **coda dopo il verbo**: «what do coral polyps form **colonies …**?» →
     «reefs.». Una parola piena non letta dopo l'operatore deve impedire la
     risposta, come già fa `unread_word_between/3` per la vicinanza.
  Poi si riprova la preferenza per la radice (+3 risposte dalla lettura su r300).
  **Regola ricavata:** ogni volta che la strada A risponde dove prima c'era un
  muro, il referto la mostra come «non muro, non giusta». Va guardata subito,
  perché la strada A risponde **prima** del frasario, e una sua falsa risposta
  vale più di un muro.
- ✅ **Specie A, la catena ENTRATA (19 settembre, 11:20).** Dopo la lingua
  memorizzata per lo scope (`f4706f2a`) la catena v2 di `docs/labs/specie-a/`
  costa **2,1 s** di `input_frame_observe` sul paragrafo di r320 (erano 34,6 s
  nella prima versione), con il turno a 11,9 s. Pioli invariati e sotto i
  2 minuti: r300 48/62 (72 s), r320 20/68 (88 s), r340 5/65 (98 s). 14 turni
  identici. `read:` di una frase nuova si impegna dalla IR, e la domanda risponde
  da `turn_plan` (porta `turn_response`), non da `answerframe`: test
  `tests/p0t/language/ir_reading_answer.p0t`, 7 verdi. **Limiti:** vale per
  `read:`, perché la prosa incollata nel turno non si impegna ancora dalla IR;
  e solo per le domande binarie sull'oggetto (`qsr`). Il banco non cambia,
  perché le sue risposte vengono ancora dal frasario, che legge prima. Il
  prossimo passo di specie A è impegnare dalla IR anche la prosa del turno, e
  misurare quante risposte del banco passano da `answerframe` a `turn_plan`.
- **Specie A, la catena misurata (19 settembre, 10:45–10:57).** Su una frase nuova
  («The zorvan beetle threatens the kelda forest.») la strada A si fermava in tre punti:
  1. **ruoli**: i frame legavano solo entità note. Aggiunto: un `np_candidate`
     della IR è un candidato entità (`np_candidate_entity/2`, `input-structure.p0`);
  2. **operatore**: la IR conosceva solo la forma nuda del verbo. Aggiunto:
     `linguistic_form(Form, Root, en, common) :- verb_reading_form(Root, Form)`
     (`grammar.p0`);
  3. **risposta**: `input_frame_reading_unique/4` non aveva consumatori. Aggiunto
     `ir_reading_answer/2` sulla porta `turn_response/2`.
  ⚠ **Rettifica delle 11:08.** Due dei tre anelli, il candidato sintagma e le
  forme flesse come operatore, **non** erano inerti: su r320 portavano
  `input_frame_observe` da 1,7 a 11,1 s, perché i frame ora venivano valutati
  davvero e l'enumerazione sotto `naf` di `input_nearest_entity_*` esplodeva.
  Il controllo su r300 (74 s) non lo vedeva. Sono stati ritirati; resta solo
  `ir_reading_answer/2`, che non costa niente senza un legame. La catena
  completa, compresi il nome nudo e la chiusura alle forme finite, risponde
  davvero dalla strada A: «what do beetles threaten?» → «forests.» via `turn_plan`.
  È salvata con le misure in `docs/labs/specie-a/`, e prima di applicarla va
  riscritta la vicinanza fra entità (lista calcolata una volta, `member`).
  Il testo qui sotto descrive lo stato prima della rettifica.
  Con i tre anelli la IR osserva il frame della frase nuova
  (`input_semantic_frame` e `input_frame_record` presenti). La catena si ferma
  però al **commit**: la prosa detta nel turno non viene mai impegnata (il commit
  esiste solo in `extract_clause`, cioè per `read:`), e su `read:` il bundle
  risulta assente («no bundle» in `P0_READ_TRACE`). Causa non trovata; in più
  `!query input_entity_node(current_prose, en, _, kelda_forest)` ha dato esiti
  diversi in due file identici. Le tre regole sono **inerti sul banco**
  (r300 48/62 identico, stessi moduli; 14 turni identici): sono fondamenta,
  non un guadagno. Prossimo passo: chiedere `input_assertion_set(current_prose, S)`
  e `input_assertion_unique` per vedere perché il bundle non si forma, poi
  impegnare anche la prosa del turno con la stessa coppia di domande
  (`input_assertion_bundle` → `input_frame_commit`) usata da `extract_clause`.
  Contrasto: la risposta A deve comparire con `who answered?` diverso da `answerframe`.
  **Aggiornamento 10:53.** Dopo `read:` l'insieme `input_assertion_set(current_prose, S)`
  ha **0** elementi (misurato con `count_list`), mentre in un altro file identico
  `input_semantic_frame(current_prose, assertion, binary(threaten), _)` era riuscito.
  Due ipotesi da falsificare, in quest'ordine:
  (a) **guardia del solver**: `input_nearest_entity_before/after` chiude con
  `naf(input_entity_between(…))`, che ora enumera anche i sintagmi candidati, e
  `naf` declina sotto qualunque guardia (vedi `docs/parrot-p0-syntax.md`, riga
  su `naf`). Prova: stessa query con un solo sintagma nuovo e con l'entità
  nota al posto del secondo; se l'esito diventa stabile, la cura è
  calcolare una volta la lista delle entità della clausola e negare `member`;
  (b) **`findall` con risultato parzialmente legato**: `input_assertion_unique`
  chiama `input_assertion_set(S, cons(A, nil))`, e la memoria del progetto dice
  che `findall` fallisce se il risultato arriva legato in parte. Prova: una
  frase con entità tutte note, che il percorso IR impegnava già (i test
  `document_*`), e `!query input_assertion_unique(current_prose, _)`.
  **Misurato alle 10:57.** Il commit della IR su `read:` **non produce bundle nemmeno
  con la KB d'inizio sessione** (`636ce2a2`, A/B per file), neanche per «read:
  Mercury is a metal.». Con la KB di oggi il nodo operatore (`membership`) e il
  nodo classe (`metal`) ci sono, ma **«Mercury» non è un'entità**: non è
  `semantic_entity`, e senza articolo non diventa `np_candidate`. Quindi non nasce
  nessun frame, l'insieme delle asserzioni è vuoto, e ogni fatto letto da
  `read:` viene oggi dagli schemi di stringa (specie B). Il prossimo anello
  di E2a è riconoscere come candidato ruolo anche il **nome nudo** (un token
  pieno fuori da ogni classe funzionale e non forma di verbo), sempre
  come conoscenza KB sulla IR e non come elenco. Solo dopo ha senso
  provare l'ipotesi (b).
- **E2b, primo passo (tenuto).** Il passivo dalla radice torna, ma solo per
  `event_subject_verb/1`: sono i verbi il cui soggetto può essere un'azione o un
  mezzo, seme `aid`, insegnabili con «V is an event subject verb» e
  ritirabili. r320: 19 → 20/68 («what aids…» → «shredding the plant matter.»,
  vero ma **parziale**: gli altri due mezzi, «adding water» e «ensuring proper
  aeration…», sono ancora fuori dallo slot, perché la coordinazione è E2c).
  I verbi agentivi (`prepare`) restano esclusi: lì «by + gerundio» è il mezzo,
  e serve il lettore che separa mezzo e agente.
- **Furto chiuso.** «how much of the waste in landfills do compostable materials
  make up?» riceveva la procedura del generatore, scelta per la sola parola
  «materials». Aggiunto `topic_noise(process_topic, materials)`. Resta
  la classe: «how much … make up» viene letto come richiesta di procedimento
  («how to make make»). La guardia `not_cue(count_question)` è stata provata
  e scartata perché toglieva la ricetta a «how many eggs … carbonara».
- **Classe di furto diagnosticata, non chiusa: «what is + proposizione» →
  glossa di un'entità vicina.** Tre istanze: r320 «what is added to the plant
  matter?» → glossa di «matter»; r340 «what is the temperature of
  carbonization a factor for?» → glossa di «temperature»; r340 «what is
  charcoal made of carbon by?» → il ciclo del carbonio. La risposta è la glossa
  `wiki_concept/3` (`kb/facts/foundations.p0`) restituita da `mod_answer_frame`
  (`who answered?` → answerframe). Non viene da `read_about`, che risponde «I
  have no definition of it, but I read…»: la prima attribuzione nel
  registro era sbagliata. Forma comune: dopo «what is» non c'è un sintagma
  nominale solo ma una proposizione (participio, «a factor for», «made of … by»).
  La cornice definitoria vale per «what is <SN>?». Prossimo passo: trovare
  in `mod_answer_frame` il punto in cui la glossa dell'entità risponde, e far
  decidere alla KB (fuoco del turno o condotta di cessione) che una domanda
  con la coda proposizionale non è una domanda di definizione. Contrasto:
  «what is temperature?» deve ancora dare la glossa.

### Stato di E1 — 19 settembre 2026 (diagnosi fatta, nessuna modifica tenuta)

Testo: le prime tre frasi di r320 (compost), due sessioni separate con la
stessa KB (`agi`, rete spenta), una come prosa incollata e una con `read:`.
Fatti letti dal dump di sessione (`PARROT0_SESSION_DUMP`).

| Oggetto | Prosa incollata | `read:` (oggi) | `read:` con lo splitter che cede al lettore (provato, annullato) |
|---|---|---|---|
| documento | nessuno | **2 documenti** da 1 unità | 1 documento, 3 unità |
| unità della frase 1 | — | spezzata in due frammenti riscritti (`document_1_unit_0` «Compost is a mixture of ingredients», `document_2_unit_0` «Compost used as plant fertilizer…»), range relativi al frammento | giusta, `range(0, 128)` |
| unità della frase 2 («It is commonly prepared…») | — | nessuna | **sbagliata**: testo della frase 1 troncato, `range(0, 105)` |
| unità della frase 3 | — | nessuna (letta fuori dal documento) | giusta, `range(232, 123)` |
| fatti | `mixture(compost)`, `used_as(compost, plant_fertilizer)`, `rich_in(resulting_mixture, plant_nutrients)`, `includes(beneficial_organisms, bacteria)` | gli stessi | `mixture(compost)`, **`used_as(compost_of_ingredients, …)`**; niente `rich_in` |
| «what is compost used as?» / «…rich in?» | Plant fertilizer. / plant nutrients. | uguali | **muri** |

**Prima divergenza, riproducibile.** `read:` con più frasi è
`compound_statement` (`turn-frames.p0`), quindi `compound_turn_lead` lo spezza
e lo riscrive **prima** di `mod_reader`. Ogni frammento che conserva la cue
diventa un documento a sé, e il resto del testo esce dal documento.
L'identità documentale del passo si perde nello splitter.

**Perché la cura ovvia è stata annullata.** Una guardia KB,
`naf(turn_reads_source(T))` con `reader_source_cue/1` derivata da
`segment_role` + `faculty_for(_, reader)`, restituisce il documento unico. Però
fa emergere due difetti del lettore di `read_passage`, e fa perdere due risposte vere:
1. **Span della clausola riscritta** (`extract_clause`, `30-generation-reading.c`):
   la posizione è `c - source_base`. Quando `reader_focus_rewrite` riscrive
   «It» → «Compost», la clausola sta in un buffer separato, lo start cade a 0 e
   l'IR pubblica i primi *len* byte del passo come sorgente dell'unità.
   L'unità vuole lo span **originale**, l'estrattore legge la clausola
   **riscritta**: sono due cose, e oggi una sola pubblicazione le confonde.
2. **Il lettore di `read_passage` legge peggio dello splitter composto**: il
   modificatore «used as…» diventa `compost_of_ingredients`, e «The resulting
   mixture is rich in…» non produce `rich_in`. Lo splitter passa ogni frase al
   lead completo del turno; `read_passage` la passa al solo `extract_clause`.

**Prossima azione E1 (una sola).** Far passare le unità del documento dallo
stesso percorso di lettura della prosa incollata, invece del contrario:
- oppure `compound_turn_lead`, quando la cue di sorgente c'è, registra i
  frammenti come unità di **un** documento (con gli span dell'originale);
- oppure `read_passage` consegna ogni frase al lead del turno.
La scelta va misurata con le stesse quattro domande e i quattro fatti di sopra.
Contrasto obbligatorio: nessuna risposta persa rispetto alla prosa incollata.
Il difetto 1 (span della clausola riscritta) appartiene a E4a: «It» →
antecedente va rappresentato come menzione + candidato, non come stringa
riscritta.

**Prossima azione già determinata:** E1, l'unica modifica descritta in «Stato
di E1» (unità di un solo documento lungo il percorso della prosa incollata),
misurata sulle tre frasi del compost. In parallelo, quando serve: E0b, il costo
di `input_frame_observe`, cioè quale clausola di `input_semantic_frame` spende
i ~12.000 goal per frase. Se il
profilo della revisione nuova cambia, aggiornare la priorità con quella
misura invece di difendere questa diagnosi.
