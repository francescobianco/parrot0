# Prima fogliata di grammatica e pragmatica inglese

19 settembre 2026. Consegna richiesta da F.: un unico popolamento ampio,
poi arresto prima degli esperimenti. Il riferimento iniziale è `dff074e2`.

Sono stati studiati il piano [lettura-della-prosa](plans/lettura-della-prosa.md),
il suo [percorso esecutivo](plans/lettura-della-prosa-esecuzione.md), i mantra,
i principi e la sequenza dei commit del 18–19 settembre. Il risultato è
conoscenza linguistica nella KB condivisa, caricata da `grammar.p0` attraverso
[`english-grammar.p0`](../kb/core/english-grammar.p0). Nessun cambiamento al C,
nessun profilo ridotto, nessun fatto copiato dai brani del banco.

> **19 settembre 2026, secondo giro — il nome della conoscenza.** F.: «i
> predicati `prose_*` scopizzano conoscenza universale», e poi, piu' preciso:
> «non importano le ridondanze di forma, quella che mi preoccupa e' lo scoping
> sui predicati che produce predicati non agganciabili dalla meta linguistica».
> Il pacchetto e' stato rifatto: i predicati portano il nome di CIO' CHE
> DESCRIVONO, il pacchetto e' `kb/core/english-grammar/`, 1.321 fatti duplicati
> sono spariti, e le distinzioni hanno ora una faccia che si puo' pronunciare
> ([§ La porta meta-linguistica](#la-porta-meta-linguistica)).

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
| [verbs.p0](../kb/core/english-grammar/verbs.p0) | `relation_verb` alimenta gli operatori IR e le cornici già esistenti; valenze e famiglie diventano viste sui nodi verbali |
| [morphology.p0](../kb/core/english-grammar/morphology.p0) | Paradigmi e radici alimentano `verb_form`, `verb_reading_form`, `verb_stem`, `participle_of`, `gerund_root`, `gerund_of`, forme finite e cornici di estrazione |
| [nominals.p0](../kb/core/english-grammar/nominals.p0) | Plurali e nomi di attività raggiungono i consumatori esistenti; tratti nominali e pronominali sono evidenze sugli stessi nodi IR |
| [function-words.p0](../kb/core/english-grammar/function-words.p0) | Classi funzionali e delimitazione nominale; operatori con span per modalità, quantità, polarità, tempo, spazio e interrogazione |
| [predication.p0](../kb/core/english-grammar/predication.p0) | Reggenze e relazioni aggettivali raggiungono lettori e domande esistenti; predicati composti conservano anche la loro lettura distinta |
| [constructions.p0](../kb/core/english-grammar/constructions.p0) | Indizi delle costruzioni, requisiti di attacco, ruoli mancanti e impegno semantico; non dichiara provato un albero sintattico dalla sola cue |
| [discourse.p0](../kb/core/english-grammar/discourse.p0) | Classi retoriche, marker documentali di osservazione/ipotesi/simulazione, abbreviazioni e letture pragmatiche |
| [reading.p0](../kb/core/english-grammar/reading.p0) | Morfologia produttiva, matcher di espressioni sulla IR, catene locali ausiliare-verbo, evidenza documentale, sonde e insegnamento |

Il matcher usa `input_node`/`input_node_next` per gli scope dell'input e
`document_unit_token` per le unità documentali già conservate. Un'espressione
composta deve allinearsi a token interi, nell'ordine e contigui negli offset;
non attraversa una virgola. Il vincolo attuale richiede esattamente uno spazio
tra token: whitespace diverso resta da verificare negli esperimenti.

Le viste pubbliche principali sono `grammatical_cue/4`, `token_morphology/4`,
`token_verb_valency/4`, `token_verb_family/4`,
`token_pronoun_reference/4`, `auxiliary_chain/4`,
`scope_requirement/4`, `construction_role_required/4`,
`commitment_at/4`, `pragmatic_candidate/4` e
`document_rhetorical_link/4`. Il primo argomento è `input(Scope)` oppure
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

Le etichette pronunciabili sono dati in `reading_name/2`. Una lezione
sull'espressione aggiorna la lettura anche dei nodi documentali conservati;
non richiede di sostituire il testo o ricompilare. Le forme sono implementate;
la loro verifica comportamentale resta da eseguire.

È preparato [english_grammar_growth.p0t](../tests/p0t/language/english_grammar_growth.p0t):
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
`scope_requirement` e `commitment_at` sono viste consultabili,
**non guardie già applicate a tutti i commit dei lettori preesistenti**.

`document_rhetorical_link` conserva possibili legami retorici con il loro span;
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

## La porta meta-linguistica

Il difetto vero della prima fogliata non era la ridondanza: era che il
lettore dell'appartenenza — «is X a Y?», «X is a Y» — fa **una** cosa sola,
unire le parole della classe in un atomo e interrogare quel predicato con
**arita' 1**. Una distinzione tenuta soltanto in una relazione a due o tre
argomenti non ha un nome che si possa pronunciare: la conoscenza c'e', e per
la meta-lingua non esiste. Un nome di missione (`prose_noun_feature`) e' lo
stesso difetto al quadrato, perche' nessuno dira' mai «prose noun feature».

Misurato prima del rifacimento, sulla KB `agi` completa:

| si chiede | prima | dopo |
|---|---|---|
| `is comprise a relation verb?` | Yes. | Yes. |
| `is however a contrastive connector?` | Yes. | Yes. |
| `is water a mass noun?` | I don't know about mass noun. | Yes. |
| `is comprise a transitive verb?` | (muto) | Yes. |
| `is comprise a verb of constitution?` | (muto) | Yes. |
| `what kind of verb is comprise?` | I don't know about comprise yet. | A verb of constitution, relation logic. |
| `what does however mark?` | I don't know about however yet. | «however» marks contrast discourse. |

`kb/core/english-grammar/naming.p0` non ricopia niente: ogni faccia e' una
**regola** su `verb_valency/2`, `verb_family/2`, `noun_feature/3`. Le 264
letture non ricevono 264 predicati — ricevono una **domanda** (`what does X
mark?`) che legge il nome gia' presente in `reading_name/2`. Il giro completo
e' verde: la lettura non si sa, si insegna parlando, si richiede parlando, si
ritira, e torna a non sapersi (`english_grammar_growth.p0t`).

La regola generale che ne esce, e che vale oltre questa consegna: **una
distinzione nuova va rappresentata nella relazione ricca e affacciata con il
nome che se ne dice.** Il nome della relazione dice la natura della
conoscenza; la faccia dice come la si nomina parlando; la missione che ha
scritto la riga resta nei commenti.

## La conoscenza detta una volta sola

Sono spariti 1.321 fatti che ripetevano conoscenza gia' presente:

| toltI | dove sono adesso |
|---:|---|
| 119 | `prose_noun_number` → `plural_of/2`, che li aveva tutti |
| 223 | `prose_valency_particle` → `verb_particle/2` (il terzo argomento era sempre la stessa costante) |
| 222 | `prose_adjective_complement` → `adjective_relation/1` |
| 88 | `prose_multiword_predicate` → `expression_reading/2` con la lettura `predicate_*` |
| 24 | `prose_auxiliary_form` → `auxiliary/1`, a cui mancavano solo `being` e `ought` |
| 367 | `irregular_verb_form`/`irregular_participle` riasseriti → **regole** che leggono le colonne di `verb_paradigm/4` (verificato: l'insieme delle forme derivate e' identico a quello dei fatti) |
| 278 | righe di catalogo che ora **derivano** dalle classi lessicali condivise (i connettivi retorici via `apply/2`, `modal_force/2`, i marcatori di osservazione/ipotesi/simulazione, `universal_quantifier/1`, `hedge_word/1`, `adjective_relation/1`) |

I ponti vanno dalle classi condivise verso le viste nuove: un membro
insegnato domani a `contrastive_connector` diventa un indizio con span al
turno dopo, senza una seconda lezione. `commitment_policy/2` non e' stata
rinominata ma **fusa** con quella di `context-scope.p0`: «un tipo di ambito
concede un certo grado di impegno» e' la stessa relazione, e `hypothesis` o
`reported` sono righe che le due parti condividevano gia'.

Corretto anche un difetto muto: i trigger di due parole erano atomi
(`can_you`), che nessun allineamento di token avrebbe mai raggiunto; ora sono
superfici (`"can you"`) e la sonda pragmatica li trova.

## Il costo misurato (aperto)

La fogliata **non era mai stata eseguita**. Alla prima esecuzione, sulla KB
`agi`, il controllo di salute del test engine non passava piu':

| stato | 6 turni banali | primo turno dopo l'avvio |
|---|---:|---:|
| senza il pacchetto | 0,60 s | < 1 s |
| pacchetto come consegnato | 8,5 s | 2,3 s |
| dopo il rifacimento | 2,4 s | 2,4 s |

Il collo di bottiglia risolto: `verb_finite_form/2` e' chiamato dentro i cicli
sulle particelle **con la radice legata e la forma libera**, e il ponte lo
risolveva attraverso la vista indicizzata per superficie, cioe' scorrendo
tutte le forme a ogni chiamata. Una vista gemella indicizzata per radice
(`verb_root_form/2`) porta i 6 turni da 11 s a 2,4 s.

Il costo che **resta** non e' della fogliata ma della sua stazza: le 844
dichiarazioni `relation_verb` in piu' portano la costruzione di
`extract_frame` da circa 0,2 s a **2,5 s** (misurata con `PARROT0_BOOT_TRACE=1`,
che ora stampa anche il costo di ogni vista), perche' le famiglie di cornici di
`grammar.p0` generano uno schema per ogni verbo di relazione — il rischio che
il commento di gen491 aveva gia' scritto. Ogni turno paga poi lo scorrimento di
quegli schemi (0,4 s contro 0,1 s). E ogni lezione che tocca un lemma verbale
invalida quella vista: il ritiro di un lemma costa 7-13 s.

**Decisione aperta per F.**: o si indicizzano le cornici per il verbo che le
regge (lavoro di motore), o si riduce cio' che la fogliata dichiara come verbo
di relazione (riduce cio' che parrot0 vede, contro il criterio di evoluzione).
Finche' resta aperta, `make test-engine` fallisce il controllo di salute e
`make soft-test` non parte.
