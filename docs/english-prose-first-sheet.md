# Prima fogliata di grammatica e pragmatica inglese

19 settembre 2026. Consegna richiesta da F.: un unico popolamento ampio,
poi arresto prima degli esperimenti. Il riferimento iniziale è `dff074e2`.

Sono stati studiati il piano [lettura-della-prosa](plans/lettura-della-prosa.md),
il suo [percorso esecutivo](plans/lettura-della-prosa-esecuzione.md), i mantra,
i principi e la sequenza dei commit del 18–19 settembre. Il risultato è
conoscenza linguistica nella KB condivisa, caricata da `grammar.p0` attraverso
[`english-prose.p0`](../kb/core/english-prose.p0). Nessun cambiamento al C,
nessun profilo ridotto, nessun fatto copiato dai brani del banco.

## Dimensione e contenuto

**10.222 fatti espliciti e 67 regole**, oltre alle direttive di caricamento e
agli attributi `machinery` prodotti dal caricatore. Sono conteggi del nuovo
pacchetto, non del guadagno di comprensione. Le viste generano altre forme.

| Conoscenza | Quantità nel pacchetto |
|---|---:|
| Lemmi verbali per la morfologia | 1.266 |
| Dichiarazioni aggiuntive `relation_verb` | 844 |
| Appartenenze dei verbi a famiglie semantiche | 1.606 |
| Valenze verbali | 1.387 |
| Paradigmi irregolari, comprese varianti | 190 |
| Eccezioni aggiuntive plurale → singolare | 107 |
| Associazioni espressione → lettura | 1.688 |
| Distinzioni grammaticali e pragmatiche nominate | 264 |
| Reggenze aggiuntive `verb_particle` | 203 |
| Relazioni aggettivali aggiuntive | 217 |
| Famiglie di costruzioni | 60 |
| Requisiti dei ruoli delle costruzioni | 193 |

Un'espressione può avere più letture: i 1.688 fatti non sono necessariamente
1.688 superfici diverse. Lo stesso vale per verbi con più valenze, varianti
ortografiche e famiglie semantiche sovrapposte.

Il lessico verbale attraversa costituzione, causalità, cambiamento, fisica,
misura, informazione, biologia, chimica, geografia, storia, società,
cognizione, comunicazione, creazione, calcolo, percezione, possesso e spazio.
Include usi transitivi, intransitivi, ditransitivi, copulari, predicativi,
controllo del soggetto e dell'oggetto, raising e complementi frasali.

La grammatica comprende determinanti, pronomi e riflessivi, numero e nomi
di massa, quantificazione e distributività, comparativi e superlativi,
grado, reggenze, phrasal verbs, tempo, durata, frequenza, aspetto, ausiliari,
attivo e passivo, subordinate finite e non finite, relative e relative
ridotte, subordinate senza relativo espresso, interrogative indirette,
causali, temporali, condizionali e controfattuali, concessive, finali,
consecutive, coordinazioni e correlazioni, ellissi, gapping, apposizioni,
costruzioni assolute, esistenziali, cleft, extraposition e inversione.

La pragmatica comprende aggiunta, contrasto, conseguenza, motivazione,
esemplificazione, riformulazione, analogia, eccezione, concessione,
sequenza, sfondo, cambio di argomento, qualificazione, fonte, attribuzione,
osservazione, simulazione, ipotesi, incertezza, disaccordo, presupposizione,
implicature cancellabili, focus, richieste indirette e uso metalinguistico.

## Collegamenti effettivi

| File | Dove entra la conoscenza |
|---|---|
| [verbs.p0](../kb/core/english-prose/verbs.p0) | `relation_verb` alimenta gli operatori IR e le cornici già esistenti; valenze e famiglie diventano viste sui nodi verbali |
| [morphology.p0](../kb/core/english-prose/morphology.p0) | Paradigmi e radici alimentano `prose_verb_form`, `verb_reading_form`, `verb_stem`, `participle_of`, `gerund_root`, `gerund_of`, forme finite e cornici di estrazione |
| [nominals.p0](../kb/core/english-prose/nominals.p0) | Plurali e nomi di attività raggiungono i consumatori esistenti; tratti nominali e pronominali sono evidenze sugli stessi nodi IR |
| [function-words.p0](../kb/core/english-prose/function-words.p0) | Classi funzionali e delimitazione nominale; operatori con span per modalità, quantità, polarità, tempo, spazio e interrogazione |
| [predication.p0](../kb/core/english-prose/predication.p0) | Reggenze e relazioni aggettivali raggiungono lettori e domande esistenti; predicati composti conservano anche la loro lettura distinta |
| [constructions.p0](../kb/core/english-prose/constructions.p0) | Indizi delle costruzioni, requisiti di attacco, ruoli mancanti e impegno semantico; non dichiara provato un albero sintattico dalla sola cue |
| [discourse.p0](../kb/core/english-prose/discourse.p0) | Classi retoriche, marker documentali di osservazione/ipotesi/simulazione, abbreviazioni e letture pragmatiche |
| [reading.p0](../kb/core/english-prose/reading.p0) | Morfologia produttiva, matcher di espressioni sulla IR, catene locali ausiliare-verbo, evidenza documentale, sonde e insegnamento |

Il matcher usa `input_node`/`input_node_next` per gli scope dell'input e
`document_unit_token` per le unità documentali già conservate. Un'espressione
composta deve allinearsi a token interi, nell'ordine e contigui negli offset;
non attraversa una virgola. Il vincolo attuale richiede esattamente uno spazio
tra token: whitespace diverso resta da verificare negli esperimenti.

Le viste pubbliche principali sono `prose_evidence/4`, `prose_morphology/4`,
`prose_predicate_valency/4`, `prose_predicate_family/4`,
`prose_reference_evidence/4`, `prose_local_chain/4`,
`prose_scope_requirement/4`, `prose_required_role/4`,
`prose_local_policy/4`, `prose_pragmatic_candidate/4` e
`prose_document_link/4`. Il primo argomento è `input(Scope)` oppure
`document(Unit)`; il legame documentale usa invece il Document ID esistente.
Gli span contengono gli ID originali, non una copia privata del testo.

Le sonde 30–34 dell'ispettore mostrano indizi, morfologia, catene verbali,
operatori che richiedono scope e inferenze pragmatiche candidate. Le viste
materializzate sono **binarie**, con analisi composte nel secondo argomento:
il motore corrente materializza solo arità 1 e 2. Dipendenze dichiarate e
grafo delle regole ne governano l'invalidazione.

## Crescita a runtime

Resta il canale esistente «X is a relation verb». Per le nuove viste:

```text
the expression perchance marks epistemic possibility
forget that the expression perchance marks epistemic possibility

zorbify is an english verb lemma
forget that zorbify is an english verb lemma
```

Le etichette pronunciabili sono dati in `prose_reading_name/2`. Una lezione
sull'espressione aggiorna la lettura anche dei nodi documentali conservati;
non richiede di sostituire il testo o ricompilare. Le forme sono implementate;
la loro verifica comportamentale resta da eseguire.

È preparato [english_prose_growth.p0t](../tests/p0t/language/english_prose_growth.p0t):
insegnamento e ritiro della stessa espressione sullo stesso nodo, frase composta
e punteggiatura, crescita della morfologia, distinzione passato/participio,
ablazione di una regola di catena verbale, letture modali concorrenti.
I nodi artificiali servono esclusivamente a contratti meccanici sulla KB
`agi` completa. Non sono una prova di comprensione, né fatti di Wikipedia.
**Il file non è stato eseguito**, come richiesto per questa fogliata.

## Che cosa non è ancora certificato

La presenza delle 60 famiglie non significa che il motore sappia già
risolverne ogni attacco. Le nuove viste espongono indizi e requisiti;
coreferenza, scope incassato, ellissi e composizione documentale completa
richiedono ancora consumatori che soddisfino quei requisiti. In particolare
`prose_scope_requirement` e `prose_local_policy` sono viste consultabili,
**non guardie già applicate a tutti i commit dei lettori preesistenti**.

`prose_document_link` conserva possibili legami retorici con il loro span;
non li promuove automaticamente a inferenze causali. Il lettore esistente
delle singole parole continua a consumare `rhetorical_marker_class`.

La morfologia tipata distingue passato e participio, ma le regole legacy di
`participle_root`, `participle_of` e i suffissi generici continuano a esistere.
La loro eventuale sovragenerazione non è stata dichiarata risolta. Analogamente,
una reggenza aggiunta a `verb_particle` incontra ancora i limiti di identità
della relazione del lettore storico: le particelle non sono tutte sinonime.

Le molte nuove radici possono cambiare l'ambiguità nome/verbo e i costi delle
viste e del turno. Non sono stati rimisurati punteggi o latenze della prosa,
non sono stati corretti regressioni o oracoli sulla base delle risposte.
Nessuna dichiarazione di comprensione universale è sostenuta dal solo volume.

## Verifiche di consegna

Controllo statico su tutti i nuovi `.p0`: arità entro 4, corpi entro 8 goal
(massimo effettivo 5), parentesi e stringhe bilanciate. Il boot del profilo
`agi` completo con sessione vuota termina normalmente, stderr senza errori
di parsing. Non è stata lanciata alcuna suite, alcun piolo o esperimento di
comprensione. Nessuna modifica a `src/`.

## Riferimenti linguistici

Il repertorio è una prima redazione linguistica, non un corpus scaricato.
Per la distinzione tra forma finita e non finita e tra tipi di modalità:
[Cambridge, finite and non-finite verbs](https://dictionary.cambridge.org/grammar/british-grammar/finite-and-non-finite-)
e [modality: meanings and uses](https://dictionary.cambridge.org/grammar/british-grammar/modality-meanings-and-uses),
consultati tramite gli estratti indicizzati; il recupero delle pagine complete
ha restituito 403. Per il ruolo dei segnali discorsivi nella connessione
delle idee e nella gestione della comunicazione:
[British Council, Discourse markers](https://www.teachingenglish.org.uk/professional-development/teachers/teaching-knowledge-database/d-h/discourse-markers).
Queste fonti orientano la tassonomia; non certificano ogni voce del popolamento.
