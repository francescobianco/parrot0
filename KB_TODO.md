# Knowledge Base TODO

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
