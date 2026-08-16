# Knowledge Base TODO

## Fronte attivo: gen395, contesti e scope concorrenti

Il piano operativo e' in
[`docs/plans/frontier-kb-natural-dialogue.md`](docs/plans/frontier-kb-natural-dialogue.md),
costruito a partire soprattutto dalla parte finale di
[`docs/plans/question-emergence.md`](docs/plans/question-emergence.md). Le sette
generazioni consecutive elevano forme linguistiche, denotazione contestuale,
letture e gap, policy dialogica, scope, registro/answer plan e memoria
discorsiva. Le clausole future nel piano sono esempi guida, non capacita'
gia' rivendicate.

La parte linguistica di gen391 e' promossa nel commit `be3cedf`: viste
bidirezionali delle forme, status e alternative crescono per assert/retract;
le superfici conversazionali sono fatti `answer_frame/2`. Il gate AGI e'
tornato sotto il secondo senza timeout grazie a meccanismi generali del solver
e della colla, senza lessico nel C. L'oracolo eager/diamante di 391.6 resta un
debito del filo di residenza, non un motivo per fermare l'evoluzione semantica;
blocca invece qualunque dichiarazione operativa di `lazy_load/1`.

Il taglio verticale gen392 e' ora presente: `denotation.p0` deriva le letture da
`concept_label/4`, `domain_category/2` e categorie esistenti; il dominio
seleziona senza cancellare le alternative. Uso e menzione convergono sui ruoli
del modello universale dell'input, e `canonicalization_exempt/1` e' il solo
protocollo nuovo consumato dal C: il motore copia span, mentre cue, ruolo,
lingua, registro e forme restano fatti. La proiezione fra entita' riusa il solo
arco tipato `requires/2`; non e' stato aggiunto un `related_to` generico.

Il ratchet `tests/p0t/language/contextual_denotation.p0t` prova omonimia per
dominio, alternative senza dominio, citazione, ponte fuori dai giochi,
relazione sbagliata, retract e crescita/ablazione di una cue di menzione
inventata. `make soft-test` e' verde in 6 secondi sul budget invariato di 15;
`make test` chiude 1800 asserzioni, zero fallimenti.

L'audit del manifesto eager ha trovato un asse che la sola `include/1` non puo'
esprimere: `capabilities.p0` entra oggi come reflective, mentre gli altri file
core entrano come base. Un manifesto unico lo renderebbe persistibile anche se
le risposte restassero verdi. E' stata quindi specificata `file_layer/1`, locale
al file e distinta da `file_attribute/1`. Finche' layer e registry canonica non
esistono, non viene creato un manifesto `.p0` morto o semanticamente falso e
`lazy_load/1` resta non operativa.

Il primo taglio gen393 e' in `kb/core/dialogue-frames.p0`. Atto, slot, source,
letture e status sono fatti; completezza, residuo strutturale e risposta
derivabile sono regole. `frame_answer/2` usa `apply/2`, quindi attraversa il
solver comune. `tests/p0t/meta/three_axis_gap.p0t` prova assert/retract di slot,
proof positiva, due letture concorrenti e ablazione della superficie; `make
soft-test` resta verde in 6 secondi e `make test` passa 1821 asserzioni senza
fallimenti. Questo non e' ancora il producer NL -> frame: il test materializza
il frame esplicitamente e il TODO resta aperto.

Il primo gap epistemico e' ora presente senza nuova primitiva C:
`frame_gap(Frame, missing_fact)` nega la vista ground
`frame_has_answer(Frame)`. Il solver possiede gia' i tre esiti richiesti da
`question-emergence.md` — proved, finite failure, incomplete — e la NAF declina
quando il budget rende la ricerca incompleta. Il ratchet chiude il gap asserendo
il solo fatto e lo riapre ritraendolo. Restano aperti il falsificatore che
esaurisca deterministicamente il budget e le diagnosi di ponte, operatore e
realizzazione.

Una conversazione reale ha poi falsificato la raggiungibilita' del frame:
`continent_of(rwanda, africa)` era gia' residente, ma «dove si trova la ruanda»
andava a muro. Non era `missing_fact`: mancavano il ponte semantico
`continent_of -> located_in`, la superficie `where is -> located_in` e
l'esonimo `ruanda -> rwanda`. I tre pezzi sono ora conoscenza KB. Il ratchet
`knowledge/geographic_location.p0t` copre Ruanda, Parigi e un paese inventato;
assert/retract del fatto, della superficie e della traduzione cambiano la
risposta senza rebuild. Il producer NL -> frame generale resta aperto: il caso
passa ancora dal consumer binario esistente e non autorizza a dichiarare gen393
end-to-end.

Il prompt successivo «dove si trova Milano» ha falsificato anche quel ratchet:
Ruanda copriva soltanto `continent_of/2` e Parigi possedeva gia' un fatto
`located_in/2`, quindi nessuno dei due obbligava le relazioni amministrative a
comporre. Milano era presente come `capital_of_region(lombardia, milano)`, ma il
predicato non raggiungeva la vista spaziale e gli atomi locali contraddicevano
la canonicalizzazione `milano -> milan`. I capoluoghi regionali sono ora
canonici e `administrative_capital/2` unifica capitali di regione, stato e
paese; per il vecchio `capital_of_country/2` bidirezionale la guardia
`continent_of/2` impedisce di invertire paese e capitale. Il ratchet comprende
Milano, Firenze, Sacramento, Nairobi e una regione inventata asserita e ritratta
a runtime. Con `PARROT0_TOOLS=1` il rosso ha inoltre scoperto che la cue
`intent_cue(piact_grep, "where is")` rubava ogni domanda inglese e cercava nel
filesystem: e' stata sostituita da forme esplicitamente legate al codice, come
`where is defined`. Il `.p0t` gira ora con strumenti attivi, quindi questo
conflitto di routing non puo' piu' restare nascosto dal mock. Questo e' il
livello di trasferimento che il primo test non aveva.

Il controllo «Napoli -> Campania; Campania -> Napoli» ha poi scoperto un
`wrong-answer gap` nel consumer generale: `answer_frame/2` provava sempre
`predicato(entita', ?)` e poi `predicato(?, entita')`, trattando ogni relazione
binaria come interrogabile in entrambe le direzioni. Il protocollo opzionale
`answer_frame_input_arg(Superficie, Predicato, 1|2)` rende ora il binding uno
slot della conoscenza; il C applica soltanto la meccanica numerica e non conosce
ne' `where is` ne' `located_in`. `region_of_country/2` separa inoltre la
collocazione della regione dal rapporto col capoluogo. Il ratchet prova che
Campania risponde Italia, che una regione inventata senza contenimento non
risponde col proprio capoluogo e che ablare/reinserire il metadato riproduce e
chiude l'errore senza rebuild.

Due stimoli successivi hanno aggiunto altri due `wrong-answer gap`. «Quali sono
i colori che identificano gli scacchi» perdeva la faccetta esplicita `color`
perche' il dominio `chess` selezionava prima la categoria predefinita
`chess_piece`; ora il frame KB `side_color/2` conserva faccetta e verso, e un
gioco inventato prova crescita, retrazione e ablazione della superficie a
runtime. `i=0; i++; quanto vale i` veniva invece soltanto classificato come
codice: il riconoscimento del registro consumava una domanda ancora aperta. La
KB dichiara ora `segment_role(query, Cue)` come unica sorgente viva del confine
e dell'atto; il motore separa lo span chiuso e riusa la propria meccanica di
statement per il binding finale. Il `.p0t` aggiunge e ritrae una cue inventata
senza rebuild.

Durante questo taglio una vista Horn
`segment_role(query, Cue) :- intent_cue(code_state_query, Cue)` ha fatto salire
`make soft-test` da 10 a 24 secondi pur lasciando verdi le asserzioni. Il budget
non e' stato alzato: la causa e' la rivalutazione ripetuta della relazione
derivata da parte di `kb_evidence_matches` quando enumera tutte le ipotesi. La
sorgente canonica e' stata resa direttamente `segment_role/2`, evitando anche
la duplicazione semantica, ma il costo delle relazioni di evidenza derivate
resta un debito del motore da isolare e correggere, non un limite accettato.

Il dubbio sulle coppie `stipulation_cue` in `grammar.p0` non era invece un bug
del loader corrente. Da gen335 il punto di livello 0, non la newline, chiude una
clausola. `tests/multigoal.sh` verifica il caso generale e il nuovo
`meta/multiclause_cues.p0t` interroga i secondi predicati reali di tre righe,
poi ne ritrae e riasserisce uno. Se una stipulation cue resta inerte, il prossimo
luogo da indagare e' il collegamento cue -> contesto -> mossa.

Verifica di questo taglio: `make soft-test` verde in 11 secondi sul budget
invariato di 15; `make test` chiude 1894 asserzioni senza fallimenti;
`tests/multigoal.sh` chiude 15 prove del loader, inclusa la clausola multipla
sulla stessa riga.

La gen394 e' avviata con `kb/core/dialogue-policy.p0`. `dialogue_state/2`
trasforma proof e `missing_fact` in evidenza; `move_policy/2` sceglie la mossa e
`frame_move/2` le compone. Il `.p0t` `conversation/dialogue_moves.p0t` prova che
lo stesso frame passa da `answer` a `decline`, e da `decline` a `clarify`
cambiando soltanto la policy a runtime. Non e' ancora il router dialogico: issue,
obblighi, precedenza effettiva e consumo prima del first-match restano aperti.
`make soft-test` e' verde in 7 secondi sul budget 15; `make test` chiude 1870
asserzioni, zero fallimenti.

La gen395 e' avviata in `kb/core/context-scope.p0`. `context/2`, `holds_in/2`,
provenienza, confidenza e policy di commitment mantengono mondo, ipotesi,
citazione e credenza riportata come oggetti distinti. Le viste locale, ereditata
e visibile restano concorrenti; `proposition_signature/4` permette di derivare
un conflitto fra contesti e `supersedes_in/3` applica una correzione soltanto
nello scope che la dichiara. `scope_kind_for_act/2` e' policy retraibile. Il
ratchet `conversation/context_scope.p0t` materializza fatti inventati a runtime,
abla lo scope senza cancellarne i fatti e conserva entrambe le risposte sul
caso dei pinguini. Restano aperti il producer NL -> contesto e la proiezione dei
vecchi mondi locali, ipotesi e premesse implementati da consumer distinti: la
gen395 non e' ancora end-to-end.

Il filo di caricamento ha un proprio contratto in
[`docs/kb-loading-and-profiles.md`](docs/kb-loading-and-profiles.md): il profilo
deve diventare l'**unico entrypoint curato della KB**. Il C alloca e applica il
protocollo, ma non enumera file core o domini. Il grafo del profilo decide cosa
e' residente al boot e quali contesti lazy sono catalogati; include idempotente,
`file_attribute/1` e confini fisici dei file rendono l'organizzazione su disco
parte della vita semantica della KB. Questo filo e' ancora una specifica: non va
usato per nascondere costi di inferenza eager.

L'inventario comprende anche il falso lazy loading gia' presente nei consumer:
`lexeme.p0`, `actions.p0`, `compose.p0` e `algo_steps.p0` vengono aperti da
moduli C con flag dedicati. Nel target non diventano eccezioni del nuovo boot:
sono provider raggiunti dal grafo del profilo e attivati dai predicati nella
frontiera SLD. Il gate finale e' semplice: fuori dal loader nessun modulo deve
conoscere un path `.p0` o mantenere un flag specifico "loaded".

L'audit del routing di `/save` ha separato il router dal file
`kb/savemap.tsv`. Il TSV non viene mai letto: `kb_save_routed()` riscansiona i
`.p0`, costruisce `SmRow[]` in memoria e riscrive il file soltanto per
ispezione. E' quindi un artefatto derivato ignorato da Git e generato localmente,
non una cache operativa. La sua rimozione e' proposta nel filo di residenza; il
router dovra' usare la provenienza fisica della registry con due indici:
`(predicato, primo_argomento) -> file` e `predicato -> file` soltanto quando la
casa e' univoca. L'attuale fallback "ultimo file con lo stesso predicato" e'
dipendente dall'ordine di scansione e non va trasferito tale e quale in hashmap.

## Popular wisdom and proverbs

Add popular wisdom from multiple cultures as atomized, inferable knowledge,
not as a phrasebook or a flat collection of quotations.

The KB should separate the surface form of a proverb from the principle it
expresses. Candidate relations include:

```prolog
proverb_surface(Proverb, Language, Text).
proverb_origin(Proverb, Culture).
proverb_expresses(Proverb, Principle).
principle_condition(Principle, Condition).
principle_tendency(Principle, Consequence).
principle_recommends(Principle, Action).
principle_warns_against(Principle, Action).
principle_tradeoff(Principle, Benefit, Cost).
principle_applies_to(Principle, Domain).
principle_tension(Principle, OtherPrinciple).
```

Rules should reason over principles, conditions, consequences, advice, and
tensions independently of the original wording. Equivalent sayings from
different languages or cultures should therefore converge on a shared
principle instead of duplicating their meaning as prose.

Before considering this complete:

- cover several cultural traditions without presenting one as universal;
- preserve provenance and language for every surface form;
- model context, exceptions, and conflicting principles where applicable;
- add conversation intents and response frames in the KB, not as C literals;
- add `.p0t` coverage for inference and conversational use;
- prove runtime growth: teaching or retracting a surface form or principle must
  change recognition and inference without rebuilding Parrot0.

The review question remains: can Parrot0 learn a new proverb, map it to an
existing or newly taught principle, and use it in conversation without a C
change?

## Strati di nome: registro, simbolo, e chi li consuma

gen382b ha introdotto `concept_label(Concept, Lang, Register, Name)` — il nome di
un concetto in una lingua e in un REGISTRO, che non e' la traduzione della sua
parola (`knight` e' "cavaliere", ma il pezzo e' il cavallo). Il registro e' una
dimensione, non un'eccezione: `preferred_register(fsi)` fa rispondere "donna"
invece di "regina", con ricaduta automatica sull'uso comune per gli altri pezzi.

Restano aperte tre cose:

- **`concept_symbol/4` non ha ancora un consumatore.** I simboli della notazione
  (D per la donna FSI, N per il knight algebrico) sono in KB ma nessuna domanda
  li raggiunge: manca la superficie ("qual e' il simbolo della donna?") e il
  frame che la mappa. Vale la pena farlo come classe, non per gli scacchi.
- **Il registro e' globale.** `preferred_register/1` e' una preferenza di
  sessione; la forma giusta e' probabilmente per-dominio
  (`preferred_register(chess, fsi)`), cosi' registri diversi convivono.
- **`dimmi i pezzi degli scacchi` va ancora a muro.** Non e' una riga mancante:
  `dimmi` e' un `intent_starter` e viene TOLTO prima del dispatch, quindi con lui
  sparisce l'unica prova che il turno fosse una richiesta e resta un sintagma
  nudo. Il fix vero e' che l'atto sopravviva allo stripping, non un frame piu'
  largo. Asserito come comportamento corrente in games.p0t.

La domanda di review: puo' parrot0 imparare il nome di un pezzo in una nuova
lingua, o in un nuovo registro, senza toccare il C?  (Oggi si': e' un fatto.)

## Lo strato linguistico per insegnare REGOLE CON VARIABILI

Oggi si puo' insegnare parlando una sola forma di regola: `every X is a Y` ->
`y(V) :- x(V)`. Una variabile implicita, due predicati unari. Tutto il resto —
le regole che legano PIU' entita' — resta fuori dalla conversazione, e quindi
parrot0 non puo' essere addestrato a ragionare, solo a classificare.

Verificato: nessuna di queste funziona oggi.

    if someone is a barista then they can make coffee
    if X is a barista then X is a person
    chi e' un barista sa fare il caffe'
    if a person is the parent of a parent then he is a grandparent

**L'osservazione che da' la forma alla soluzione:** in lingua naturale le
variabili non si scrivono con le lettere, si portano con i PRONOMI INDEFINITI
("someone", "anyone", "chiunque", "chi") e si riprendono con l'ANAFORA ("they",
"them", "lui", "esso"). Sono due classi chiuse, quindi conoscenza — esattamente
come np_opener/1 e np_closer/1:

```prolog
% Le parole che INTRODUCONO una variabile.
rule_variable(someone).  rule_variable(anyone).  rule_variable(something).
rule_variable(qualcuno). rule_variable(chiunque). rule_variable(chi).

% Le parole che RIPRENDONO la stessa variabile (anafora).
rule_anaphor(they). rule_anaphor(them). rule_anaphor(he). rule_anaphor(she).
rule_anaphor(lui).  rule_anaphor(lei).  rule_anaphor(esso).

% Antecedente e conseguente.
rule_antecedent_marker(if).   rule_antecedent_marker(se).
rule_consequent_marker(then). rule_consequent_marker(allora).
```

Il motore: taglia il turno sui marcatori, dà a ciascuna clausola lo STESSO
parser di comprensione gia' in uso, e sostituisce ogni `rule_variable` con una
variabile fresca; ogni `rule_anaphor` si lega all'ultima variabile introdotta.
Il risultato e' una clausola per kb_assert_clause, che gia' esiste.

Due variabili distinte servono per le regole relazionali ("se qualcuno e' il
genitore di qualcuno, ..."): la seconda occorrenza di un `rule_variable` nella
STESSA clausola introduce una variabile NUOVA, l'anafora invece riprende. E' la
distinzione che la lingua fa gia', e per cui esistono due classi di parole
invece di una.

Nota di collegamento: "se ... allora ..." e' oggi trattato come CORNICE da
sbucciare (gen378). Qui va letto come REGOLA. La differenza e' se l'antecedente
contiene una variabile: senza, e' una cornice; con, e' una quantificazione. Il
gen378 e questo sono lo stesso sito visto dai due lati.

La domanda di review: puo' parrot0 imparare una regola relazionale a due
variabili, parlando, senza ricompilare?  (Oggi no. E' il prossimo salto.)

### Il gemello: apprendere da PROSA le regole con variabili

Stesso problema, lingua diversa — e la differenza decide il progetto. Nella
conversazione le variabili si portano con i pronomi indefiniti ("se QUALCUNO
e'..."); nella prosa enciclopedica quasi mai. La prosa usa il **generico plurale
piu' una relativa restrittiva**, ed e' li' che sta la variabile:

    mammals THAT live in water are aquatic
    rivers THAT flow into the sea form deltas
    a number is prime IF it is divisible only by itself and one
    WHEN water is heated to 100 degrees IT boils

Misurato oggi, e ogni riga sbaglia in modo diverso:

| prosa | risposta attuale | lettura |
|---|---|---|
| mammals that live in water are aquatic | "Amphibian, fish and human." | INTERROGAZIONE, non regola |
| a number is prime if it is divisible... | "No, 1 is not a prime number." | domanda su un numero |
| rivers that flow into the sea form deltas | muro | — |
| when water is heated to 100 degrees it boils | "Who or what does 'it' refer to?" | **anafora riconosciuta** |

Tre cose da qui.

**1. Il risolutore di anafora esiste gia'.** L'ultima riga lo dimostra: parrot0
sa che "it" richiede un referente e lo CHIEDE. Per una regola il referente non e'
un'entita' — e' una VARIABILE. Non serve una macchina nuova: serve che quella
esistente possa legarsi a una variabile invece che a un'entita'.

**2. `that` ha due ruoli, e oggi ne conosce uno solo.** In np_closer/1 `that`
CHIUDE un sintagma (gen382, ed e' cio' che ha sbloccato l'estrazione da prosa
vera). In una relativa restrittiva APRE una clausola che vincola la stessa
variabile del soggetto. Stessa parola, due funzioni, e la discriminante e' cosa
segue: un verbo finito apre una clausola, un nome chiude il sintagma. La KB deve
poter dire entrambe le cose della stessa parola — non e' un conflitto da
risolvere scegliendo, e' una proprieta' da rappresentare.

**3. La lettura restrittiva perde contro quella interrogativa.** "mammals that
live in water" viene enumerato invece che quantificato. Non basta aggiungere una
forma: serve decidere QUANDO una relativa e' un filtro di query e quando e'
l'antecedente di una regola. Il segnale c'e' ed e' sintattico — una regola ha un
CONSEGUENTE ("...are aquatic"), una query no — quindi e' decidibile senza
indovinare.

La forma comune con lo strato conversazionale: in entrambi i casi si tratta di
riconoscere che DUE punti del turno parlano della STESSA cosa non nominata, e di
darle un nome fresco. Pronome indefinito + anafora nel dialogo, relativa +
soggetto nella prosa. Un solo meccanismo, due vocabolari — che e' la forma di
ogni migrazione riuscita di questa sessione.

## Gli scratch brain rimasti (gen382i, parziale)

Il meccanismo che toglie l'amputazione esiste ed e' in uso su un sito:

- `brain_fresh_token(b, base, out, n)` conia un token e VERIFICA che la KB non lo
  menzioni da nessuna parte (`kb_mentions_term`: predicato, argomento, testa o
  corpo di regola);
- `p0_rename_content` rinomina i termini di CONTENUTO — chiedendo alle classi
  chiuse gia' dichiarate quali parole siano funzionali, quindi una lingua nuova
  entra come fatto;
- le premesse vivono sulla KB vera con provenienza `KB_HYPOTHETICAL` e se ne
  vanno col turno (`kb_retract_origin`, gen373).

Migrato: `one_turn_syllogism`. Restano quattro siti che costruiscono ancora un
Brain con KB vuota:

    10-memory-knowledge.c:~1557  entailment_reply
    10-memory-knowledge.c:~1790  multi_sentence_syllogism
    10-memory-knowledge.c:~1877  transitive_comparison
    10-memory-knowledge.c:~8591  l'ipotesi di mod_knowledge

Sono la stessa migrazione, e conviene farli uno per volta con la loro prova: il
sito migrato ha gia' mostrato che il cambio ha effetti non ovvi (il suffisso del
token fresco non puo' contenere "_", perche' gli atomi composti si scrivono cosi'
e il cancello dei concetti conterebbe le parole sbagliate).

La domanda di review: resta un solo posto in cui parrot0 ragiona senza la propria
conoscenza?  (Oggi quattro. Erano cinque.)

## Isolare o far convivere: cosa dice un ragionatore vero (sonda gen382j)

`tests/coexistence_probe.py` (strumento di progetto, mai runtime) estende la
sonda di gen371 — che aveva gia' stabilito che **l'LLM non isola, DECIDE** — sulla
domanda che il lavoro di gen382i ha aperto: quando una premessa contraddice il
mondo, il "gatto" dell'ipotesi e' un ALTRO concetto (rinominazione) o lo STESSO
con una credenza sospesa (convivenza con provenienza)?

**1. Sono due letture, entrambe legittime, e il discriminante e' la PREMESSA.**
Interrogato con cura, il modello non sceglie: distingue. In logica del primo
ordine "gatto" denota lo stesso insieme e la premessa aggiunge una relazione —
concetto unico, conflitto visibile. Come *stipulazione* (mondo possibile alla
Kripke) "gatto" e' un concetto nuovo con la stessa etichetta lessicale — ed e'
esattamente la rinominazione. La deduzione, dice, "opera sulla struttura formale,
non sul contenuto intuitivo del termine": funziona in entrambi i casi.

Quindi la scelta non e' una preferenza architetturale: e' una **proprieta' della
premessa**. "Supponi che tutti i gatti siano pesci" stipula; "tutti i gatti sono
pesci" asserisce, e va in conflitto. parrot0 oggi non distingue le due cose.

**2. Cosa si PERDE rinominando** (la parte che riguarda direttamente gen382i):

| capacita' | perche' la rinominazione non ce l'ha |
|---|---|
| rilevare la contraddizione | i simboli sono ortogonali: non c'e' conflitto da trovare |
| spiegare il conflitto | non puo' dire "so gatto=mammifero ma l'ipotesi dice pesce": sono termini diversi |
| revisione delle credenze | "cosa cambia se scarto X?" — X non e' mai entrato |
| abduzione esplicita | "cosa deve essere falso perche' l'ipotesi regga?" |

Sintesi del modello: **la rinominazione e' un motore di conseguenza logica pura,
la sospensione e' un motore di what-if.**

**3. La tensione dentro parrot0, che questa sonda rende visibile.** gen382i ha
scelto la rinominazione — e la rinominazione e' precisamente l'architettura che
NON PUO' accorgersi del conflitto. Ma gen375 (`class_conflict`) ha costruito
l'abilita' opposta su un'altra superficie: detto "un cane e' un pesce", parrot0
accetta E NOMINA la tensione con quello che sa. Le due filosofie oggi convivono
nel sistema senza essersi mai incontrate, e quella scelta a gen382i chiude la
porta a quella di gen375 sul percorso ipotetico.

**4. La persistenza e' gia' giusta.** Alla domanda "finita la supposizione, Tom
cosa e'?" il modello tiene i due livelli in una sola risposta: pesce
nell'ipotesi, mammifero nel mondo. E' cio' che fa `KB_HYPOTHETICAL` con la
retrazione a fine turno — su questo parrot0 e il ragionatore vero coincidono.

**La direzione che ne esce**, e va decisa prima di migrare gli altri quattro siti
scratch: la rinominazione non e' sbagliata, e' *incompleta*. Serve che la premessa
dichiari (o che il motore inferisca) se STIPULA o ASSERISCE, e che il caso
asseritivo usi la provenienza ipotetica SENZA rinominare — cosi' il conflitto
resta visibile e `class_conflict` puo' parlare anche li'.

La domanda di review: parrot0 sa dire "l'ipotesi che mi hai dato contraddice
quello che so, e te lo dico prima di ragionarci"?  (Oggi no, e con la sola
rinominazione non potra' mai.)

## PUNTO 1 (stipula/asserisce): fatto a meta', e una CORREZIONE da fare prima

### La correzione, che viene prima di tutto

gen382i dice di aver migrato `one_turn_syllogism` dalla KB amputata alla
rinominazione. **Quel sito non viene mai raggiunto.** Verificato con una stampa
sull'ingresso della funzione: per "if all bloops are razzies and zorb is a bloop,
is zorb a razzie" — e per ogni variante provata — `mod_knowledge` risponde prima
di arrivare alla riga che lo chiama (10-memory-knowledge.c:8405).

La dimostrazione che avevo portato ("is tom a cat" -> "I don't know about cat")
NON provava quello che dicevo: la prova e' compatibile con qualunque percorso che
non lasci residui, e il sandbox amputato non ne lascia per costruzione. Ho
scambiato una condizione necessaria per una sufficiente.

Il codice scritto e' corretto e non fa danni (i test restano verdi perche'
asseriscono il comportamento osservabile, che non e' cambiato), ma **la
migrazione non e' avvenuta**: e' codice non esercitato. Il commit di gen382i va
letto con questa nota accanto.

**Prima mossa, prima di qualunque altra cosa:** trovare CHI risponde davvero a
"if <premesse>, <domanda>". Il modo giusto e' la traccia che parrot0 ha gia'
("why did you answer that way?" dice `knowledge`, quindi il sito e' dentro
mod_knowledge, prima della riga 8405), non altre stampe di debug. Poi:
o si migra QUEL sito, o si rimuove `one_turn_syllogism` se e' morto — un motore
che non viene mai chiamato e' peggio di un motore che manca, perche' sembra
esserci.

### Che cosa del punto 1 c'e' davvero

Tutto tranne l'aggancio, e ognuno di questi pezzi e' verificabile da solo:

- `stipulation_cue/1` (grammar.p0, EN+IT) — quali parole STIPULANO invece di
  asserire. E' la conoscenza che mancava, ed e' il discriminante che la sonda
  gen382j ha isolato: non una preferenza architetturale, una proprieta' della
  frase.
- `response_template(premise_conflict, …)` (EN+IT) — le parole per dire la
  tensione senza rifiutare la premessa.
- `premise_conflict_note()` — il gemello RIUSABILE di `note_class_conflict`
  (gen375), che era legato alle asserzioni di classe. Stessa conoscenza (is_a/2 +
  incompatible/2), seconda superficie. E' il punto 4 della lista, fatto per meta'
  e in anticipo.
- il ramo stipulativo/assertivo in `one_turn_syllogism` — scritto, non
  esercitato.
- `kb_retract_origin` ora ritratta anche le REGOLE (gen382k). Questa non e' una
  rifinitura: senza, una premessa universale non rinominata lascia una regola
  VIVA nella KB, cioe' conoscenza falsa che continua a dedurre. Era una perdita
  reale, mascherata dal fatto che finora le premesse morivano col sandbox.

### Il piano, in ordine

1. **Trovare il sito vero** (sopra). Finche' non e' fatto, ogni migrazione
   successiva rischia di essere di nuovo codice non esercitato.
2. **Agganciare il punto 1 li'**: stipulativo -> rinomina; assertivo -> niente
   rinominazione, provenienza ipotetica, e `premise_conflict_note` prima della
   risposta.
3. **Chiudere il punto 4**: un solo rilevatore di conflitto, due consumatori
   (l'asserzione di classe di gen375 e la premessa). Oggi sono due funzioni che
   leggono gli stessi fatti.
4. Solo dopo, **i punti 2 e 3** (le ipotesi come oggetti con identita', e lo
   scope del risolutore). Sono una generazione a se': e' il momento in cui la
   provenienza smette di essere un'etichetta e diventa un contesto di credenza.
5. **Non migrare gli altri siti scratch** finche' 1-3 non sono chiusi. Migrarli
   ora significa scrivere quattro volte l'architettura che non puo' accorgersi
   del conflitto, e poi disfarla.

### Come non ripetere l'errore

La lezione non e' "fai piu' attenzione". E' che **una prova deve poter fallire**:
"non restano residui" e' vera anche col sandbox amputato, quindi non distingue le
due architetture e non prova niente. La prova giusta per una migrazione a
rinominazione e' che i token freschi COMPAIANO — per esempio chiedendo a parrot0
la traccia della derivazione, dove i simboli rinominati sono visibili — oppure un
comportamento possibile SOLO con la KB viva, come il conflitto dichiarato del
punto 1. Da qui in avanti, per ogni sito migrato: quale osservazione sarebbe
DIVERSA se la migrazione non fosse avvenuta?

## Un turno che ha rallentato di ~1s, e non so ancora perche'

`rulespec` (il caso che sintetizza e COMPILA un programma C) stava sotto i 6s a
HEAD e sta fra 6.7 e 6.9 dopo le righe di conoscenza aggiunte a
`kb/experts/games/poker.p0` (magnitude/3, magnitude_cue/3, concept_label/4).
Verificato per differenza: con `git stash` il test passa, senza fallisce.

Il budget e' stato portato a 8 con la ragione scritta accanto — NON e' un cerotto
(quella lezione e' costata 23 override tolti su 24 all'inizio di questa serie): e'
un budget che dice la verita' finche' la causa non e' capita.

L'ipotesi da verificare per prima: la scansione delle evidenze — `format_constraint`
e `magnitude_cue` vengono enumerate a ogni turno, e ogni riga aggiunta le allunga.
Se e' cosi', il difetto e' strutturale e non del poker: **la KB cresce e un turno
rallenta**, cioe' esattamente il fallimento che gen382 aveva chiuso per kb_query.
Strumento: `PARROT0_TE_SLOW=0.5 make test`, e il profilo (-pg) su quel solo turno.


---

# gen383-390 — lo strato dialogico: che cosa resta aperto

*Scritto a fine sessione, 16 agosto 2026. Sette generazioni consecutive sullo
strato dialogico (`docs/plans/the-linguistic-glue.md`) piu' due sull'ambiguita' e
il registro (`docs/plans/question-emergence.md` §14). `make test` 1769 verdi,
`glue-bench` 11/11 crisp con zero righe qualitative. Qui c'e' solo cio' che NON e'
chiuso, in ordine di quanto blocca il resto.*

## 1. ⛔ PRIMO: tre `kb_match` consecutivi che restituiscono 0, e non so perche'

Dentro un modulo, in una sessione avanzata, tre `kb_match` di seguito su regole
DERIVATE restituivano `0` in modo non deterministico — mentre le stesse regole
interrogate da fuori (MCP `kb.match`) davano i numeri giusti nello stesso
processo. Riproduzione osservata: `count_readings_answer` in
`src/brain/25-wordmath-reasoning.c` con `collection_kinds` / `collection_per_side`
/ `collection_total`; il primo (o i primi due) tornavano 0 e l'ultimo il valore
giusto. Invertendo l'ordine, si spostava: falliva sempre il PRIMO.

**Non l'ho isolato.** Il consumer e' stato riscritto per non dipenderne (i numeri
vengono dall'enumerazione che serve comunque per la scomposizione), quindi il
sintomo non e' piu' visibile — **ma il difetto e' ancora li'**, e chiunque componga
piu' query derivate dentro un modulo puo' incontrarlo, in silenzio e con numeri
sbagliati invece che con un errore.

Ipotesi non verificate, in ordine di sospetto:
- `pred_bucket()` restituisce `b.idx`, un puntatore DENTRO il censimento;
  `pred_stats_rebuild()` fa `free(...idx)` su ogni slot. Un rebuild innescato da
  una chiamata annidata mentre un `PredBucket` esterno e' vivo lascia un puntatore
  penzolante — indici di fatto casuali, quindi risultati vuoti o sbagliati.
- oppure `pred_stat_slot(kb, pred, 0)` che non trova la voce e fa apparire il
  predicato come sconosciuto (`bk.live && bk.n == 0` -> "return 0").

Strumento: un test che ripete N `kb_match` su una regola derivata dentro un
modulo, con `P0DBG` a stampare il conteggio; e un `assert`/`retract` in mezzo per
forzare il rebuild.

## 2. L'isolamento fra file nel test-engine

`tests/p0t/meta/knowledge_gap.p0t` passa 14/14 su un motore appena avviato e cade
sul PRIMO blocco quando nella stessa esecuzione lo precede un altro file — i sette
blocchi successivi, identici per forma, passano. Nessuna combinazione di `!set`
(BASE, PROFILE, WORLD_FACTS, LANG) lo rimette a posto, e un secondo `!reset`
consecutivo viene saltato dalla logica «reset intelligente».

Per questo la riga nel `Makefile` e' commentata. Finche' non e' capito, **ogni
misura fatta dentro la suite e' sospetta**, non solo questa.

## 3. L'ultimo anello: dato -> risposta

Le CORNICI sono localizzate (`response_template` /3) e i VALORI si rendono con
`tr/2` (gen388), quindi «quante sono le carte del poker» -> «poker ha 52 carte».
Ma la PROSA memorizzata resta inglese:

```
come finisce una partita a poker -> A hand ends when one player remains …
```

Non e' traducibile senza contenuto italiano: e' un limite onesto, non un bug. La
strada e' conoscenza (descrizioni italiane), non motore.

## 4. Il vocabolario italiano e' vuoto

`kb/core/lexeme.p0` ha 35 551 voci inglesi e **zero** italiane (misurato: panino,
cammino, ambiente, perche, pesce, mazzo, carte, giocatori — tutte assenti). Due
conseguenze gia' in produzione:

- il muro non puo' nominare una parola ignota in italiano (`lexicon_language(en)`
  lo dichiara, e il declino resta generico li');
- la riparazione ortografica (gen385) in italiano non ha vicini da proporre —
  «pamino» propone «amino» e non «panino», perche' *panino* non e' un lessema.

Aggiungere `lexicon_language(it).` il giorno in cui il lessico esiste accende
entrambi senza toccare il C.

## 5. Il sensore nomina parole che non sono argomenti

Il declino informato (gen384) nomina la prima parola senza fatti, e a volte non e'
un topic: «is five more than apples?» -> nomina *apples*; «…bigger…» -> *bigger*.
Onesto, a volte goffo. Il discriminante giusto non e' lessicale ma **posizionale**
— quale token e' l'argomento del turno — e arriva col residuo tipizzato di
`question-emergence.md` §11.5.

**Vicolo cieco gia' battuto, non ripeterlo:** stringere il criterio su `lexeme/1`
e' preciso in inglese, ma rende invisibile *cheese* in «parliamo di formaggio» e
caricare il lessico al muro triplica la KB e fa sforare i timeout.

## 6. Il registro: quel che manca dopo gen390

- `label_status(mangiare, informal)` e' implementato — la comprensione accetta il
  termine marcato, la realizzazione lo evita — ma **nessun turno lo esercita
  end-to-end**: mancano le MOSSE degli scacchi, non il meccanismo.
- **«Si dice X o Y»**: parrot0 ha lo statuto in KB e potrebbe rispondere quale dei
  due e' il termine curato («soprattutto catturare; mangiare e' informale ma
  d'uso corrente»). Consumer piccolo e generale, non fatto.
- **`pezzi minori`** e' dichiarato come categoria (alfiere, cavallo) ma non ha un
  conteggio suo: e' il TERZO livello, sotto il tecnico.
- **`cavallo` -> «horse»**: il pezzo degli scacchi viene letto come l'animale. E'
  `concept_label` al contrario — la stessa parola denota due concetti in due
  domini — e non e' toccato. Probabilmente serve un `concept_in_domain/3`.

## 7. Il contatore di rotazione, e altri accoppiamenti

Chiuso a gen388 per `response_template` (una rotazione per famiglia), ma restano
altri due usi di `b->response_pick` globale: `30-generation-reading.c:1287` e
`20-math.c:1890`. Stessa specie di accoppiamento — due famiglie che non si parlano
si influenzano — e stessa cura.

## 8. `apply/2` non si comporta dentro `findall/3`

Osservato a gen389: `findall($P, apply(pred, cons(...)), $L)` raccoglie zero,
mentre lo stesso `apply` come goal diretto di una clausola funziona. Non isolato.
La via giusta in quel caso era comunque astrarre la relazione invece di
meta-chiamarla (mantra #3), ma il limite resta e va scritto o riparato: chi legge
`kb_fact/2` e `apply/2` in `question-emergence.md` §9.3 non ha modo di saperlo.

---

# Le lezioni di metodo, perche' non si ripetano

Non sono TODO: sono cose che sono costate tempo e che vanno sapute prima.

1. **I cue si dichiarano nella forma CANONICA, non nella superficie.** In
   inglese le due coincidono per caso, in italiano no. Due cue in albero erano
   scritti contro la forma CORROTTA — `answer_frame("quali am i", …)` e
   `intent_cue(…, "quali")` — cioe' la KB compensava un bug del motore, e
   sistemare il motore li ha uccisi. Non e' una regressione della correzione: e'
   debito che la correzione rende visibile. **Strumento: `lang.canonical`** (MCP),
   che esiste da gen383 apposta. Vale la pena passare in rassegna gli altri cue
   con quello strumento.

2. **Il topic non e' l'antecedente.** Registrare come antecedente ogni entita'
   nominata ha fatto smettere a parrot0 di chiedere «a chi si riferisce "it"?» —
   sceglieva in silenzio. Un pronome vuole un REFERENTE introdotto; un topic e'
   solo cio' di cui si parla. Due campi, due nozioni (`last_topic` vs
   `last_entity`).

3. **Una locuzione si dichiara SOLO dove il per-parole sbaglia**, mai dove e'
   soltanto goffo: lo strato delle locuzioni sta sopra quello delle parole e
   vince, quindi una locuzione superflua non e' neutra, CANCELLA una struttura.
   Tre casi misurati in `kb/core/lexicon.p0`.

4. **L'oracolo e' segnale comportamentale, mai autorita' sul contenuto.** Fra
   modelli varia il contenuto (kimi-k2.6 diceva «pannino» per «pamino»; gpt-5.6
   dice «panino»); resta invariante la MOSSA. Si copia l'invariante — ed e' il
   punto in cui il KB-first SUPERA l'LLM: un'ipotesi validata contro `lexeme/1`
   non puo' proporre una parola che non esiste.

5. **Il muro e' il punto in cui il sistema sa di piu'.** Ci arrivava sapendo
   quale parola non conosceva, e taceva. Peggio: il registratore di lacune
   esisteva gia' ma era annidato nel ramo ANTI-RIPETIZIONE — il sensore era un
   effetto collaterale di una correzione di naturalezza.

6. **Una prova deve poter fallire.** Vale anche per le sonde: il controllo
   negativo («non disambiguare cio' che non e' ambiguo», «non riparare cio' che
   non e' rotto») ha trovato due difetti veri che i casi positivi non vedevano.
