# Verso una KB da interlocutore naturale

> **Stato:** analisi e piano, 16 agosto 2026. Nessuna implementazione accompagna
> questo documento.
>
> **Missione:** far crescere parrot0 fino a renderlo confrontabile empiricamente
> con un LLM di frontiera nella comprensione e nella conduzione del dialogo,
> mantenendo la sua natura deterministica, ispezionabile e KB-first.
>
> **Domanda zero:** una nuova forma, un nuovo registro, una nuova mossa dialogica
> o un nuovo dominio possono entrare domani come conoscenza, senza ricompilare?

## 1. Tesi del piano

La naturalezza di un LLM non deriva soltanto dalla quantita' di fatti che
possiede. Deriva soprattutto dalla capacita' di:

1. conservare piu' letture dello stesso turno;
2. scegliere una lettura usando il contesto, senza cancellare le alternative;
3. capire quale **mossa dialogica** sia opportuna prima di formulare una frase;
4. distinguere cio' che e' detto, presupposto, ipotizzato, citato o corretto;
5. adattare registro, dettaglio e tono alla situazione;
6. costruire la risposta da proposizioni provate, non recuperare una risposta
   gia' pronta;
7. mantenere questi oggetti attraverso i turni.

La KB attuale contiene molti ingredienti di questa struttura, ma sono collegati
in modo diseguale. La traiettoria corretta non e' aggiungere altre frasi a
`intent_cue` e `response_template`: e' introdurre i livelli intermedi che
permettano agli stessi fatti e alle stesse regole di partecipare a molte piu'
interpretazioni e mosse.

La sintesi e':

```text
oggi:     superficie -> faculty -> dato -> frase

bersaglio:
          superficie
              -> letture candidate
              -> frame semantico
              -> contesto e mossa dialogica
              -> goal/proof
              -> piano proposizionale
              -> realizzazione per lingua e registro
```

Il valore non sta nel numero di scatole, ma nel fatto che ogni passaggio diventa
conoscenza interrogabile, correggibile e riusabile.

## 2. Evidenza: cosa dicono gli ultimi commit

Le generazioni 383-390 non sono una serie di feature indipendenti. Mostrano una
direzione comune.

| generazione | cambiamento osservabile | astrazione emersa |
|---|---|---|
| gen383 | locuzioni, canonizzazione e lingua d'ingresso diventano fatti | la superficie e' conoscenza |
| gen384 | il muro nomina il residuo e registra la lacuna | il fallimento e' un oggetto informativo |
| gen385 | la riparazione ortografica resta un'ipotesi dichiarata | una lettura incerta non va trasformata in verita' |
| gen386 | un vincolo posto prima plasma la risposta dopo | la pragmatica e' stato KB persistente |
| gen387 | una domanda con soggetto eliso recupera il topic | la continuita' e' una relazione fra turni |
| gen388 | topic, lingua della risposta e rotazione vengono separati | contenuto, lingua e scelta della forma sono assi diversi |
| gen389 | il conteggio espone piu' letture e la loro chiave | rispondere significa anche dichiarare l'interpretazione |
| gen390 | il registro cambia estensione ed etichetta | il significato e' indicizzato al contesto d'uso |

Il principio unificante e' questo:

> parrot0 diventa piu' naturale quando aumenta cio' che puo' rappresentare e
> decidere, non quando un ramo gli impedisce di vedere le alternative.

Questo coincide con `one-kb.md`: il comportamento vicino a un LLM nasce dal
tenere insieme mondo, ipotesi, registri e letture, poi scegliere esplicitamente.

### 2.1 I cinque vincoli ereditati da `question-emergence.md` §§12-14

La parte finale di `question-emergence.md` e' la premessa piu' importante di
questo piano. Ne discendono cinque vincoli che non possono essere degradati a
semplici esempi.

**1. Il ponte ha tre assi.** Non basta collegare superficie e relazione:

```text
superficie <-> relazione     con quali forme si domanda una faccetta
relazione  <-> dati          la faccetta possiede fatti interrogabili
entita'    <-> entita'       l'entita' domandata raggiunge quella che ha il dato
```

Il caso `poker -> deck -> 52 cards` dimostra che una risposta assente non prova
che il dato manchi. Superficie, relazione, consumer e fatto possono esistere
tutti, mentre manca soltanto un arco fra due entita' note. Qualunque audit della
KB che osservi solo predicati e frame e' quindi incompleto.

**2. Il turno reale e' il sensore universale.** L'asse fra entita' non si puo'
scoprire enumerando tutte le coppie possibili senza produrre rumore. Lo si vede
quando una domanda nomina un'entita' nota, raggiunge una relazione nota e non
ottiene un valore. Il residuo del turno deve generare ipotesi distinte:
`missing_fact`, `missing_entity_bridge`, `missing_surface`, `missing_operator`
o `missing_realization`. Il muro informato di gen384 e' il primo seme di questo
sensore, non il punto d'arrivo.

**3. Dall'oracolo si copia la mossa, mai il contenuto.** Nel caso `pamino`, due
modelli divergono sulla parola ma convergono sulla condotta: ipotizzare,
enumerare se ci sono piu' vicini, non riparare cio' che e' corretto, non
inventare senza evidenza e dichiarare la riparazione. Il livello trasferibile e'
`dialogue_move`; la proposta lessicale deve essere validata dalla KB di
parrot0. Questo e' il criterio anche per ogni futuro confronto con un modello di
frontiera.

**4. Il registro e' portato dall'uso.** `vincere un pedone` attiva il senso
tecnico anche senza la parola "tecnico"; la copula nuda no. Il registro non e'
quindi soltanto una preferenza di sessione o una proprieta' della parola: puo'
essere evidenza portata dal predicato, dall'atto, dal dominio e dalla relazione
fra i partecipanti. Inoltre lo status appartiene alla forma: `mangiare` puo'
essere marcato anche se il registro `common` non lo e' in generale.

**5. La correttezza dell'inferenza e' un prerequisito.** `apply/2` dentro
`findall/3` e i `kb_match` derivati consecutivi hanno mostrato divergenze
silenziose. Una KB piu' astratta moltiplicherebbe questi cammini. Prima di
promuovere un livello che dipende da composizione di query, l'inferenza interna
e quella osservata dall'esterno devono restituire gli stessi risultati, nello
stesso ordine semantico e sotto il budget ordinario. Un workaround nel consumer
non chiude il difetto.

Da questi vincoli segue una correzione importante al linguaggio del piano: il
bersaglio non e' soltanto una pipeline superficie-relazione-risposta. E' un
**grafo di mediazione** fra forme, relazioni, entita', contesti e mosse, osservato
a partire dai turni che non riescono ad attraversarlo.

### 2.2 I quattro residui di §14.7 come taglio verticale

I quattro punti rimasti aperti alla fine di `question-emergence.md` devono
restare il primo banco di prova concettuale. Non vanno chiusi con quattro
consumer locali: insieme attraversano tutta l'architettura mancante.

| residuo | cosa prova davvero | livelli coinvolti |
|---|---|---|
| usare un termine marcato in ingresso ma non in uscita | comprensione e realizzazione hanno politiche di registro diverse | forma/senso, contesto, registro, realizzazione |
| «si dice X o Y?» | X e Y sono menzionati, non usati; la risposta parla dello status delle forme | uso/menzione, frame, mossa, answer plan |
| contare i `pezzi minori` | una categoria annidata eredita membri e quantita' senza un contatore privato | entita', tassonomia, quantificazione, proof |
| `cavallo` animale o pezzo | la denotazione dipende dal dominio e il contesto deve conservare l'alternativa | forma/senso, letture, dominio, register evidence |

La chiusura corretta deve mostrare che la stessa struttura funziona altrove:

- una forma marcata in un dominio non scacchistico;
- una domanda di terminologia in una seconda lingua;
- una sottocategoria quantitativa fuori dai giochi;
- un omonimo con due sensi in domini indipendenti.

Se il comportamento nuovo non trasferisce a questi controlli, e' una soluzione
degli scacchi e non un'evoluzione della KB.

## 3. Stato reale della KB

L'inventario testuale corrente conta 152 file `.p0`, circa 62.883 righe-fatto e
1.457 righe-regola. Nel core sono circa 44.049 fatti e 293 regole. Il rapporto
non e' una metrica di qualita', ma rende visibile la forma attuale: la KB cresce
molto piu' per enumerazione che per trasformazioni riusabili.

Le famiglie piu' grandi del core confermano il quadro:

| relazione | righe circa | lettura |
|---|---:|---|
| `lexeme/1` | 35.551 | lessico inglese molto esteso |
| `intent_cue/2` | 1.091 | superficie collegata direttamente a intenti/faculty |
| `tr/2` | 657 | canonizzazione/traduzione, spesso molti-a-uno |
| `category_member/2` | 475 | tassonomie ed estensioni |
| `response_template/*` | 337 | molte realizzazioni, alcune sono risposte complete |
| `stopword/1` | 297 | grammatica dichiarativa |
| `analysis_act_cue/2` | 216 | atti analitici raggiunti dalla superficie |
| `answer_frame/2` | 179 | ponti generali fra domanda e relazione |
| `machinery/1` | 168 | frontiera riflessiva fra substrato e mondo |
| `answer_plan/4` | 75 | primi piani retorici composizionali |
| `register_evidence/2` | 54 | percezione dell'input per evidenza |

Questa KB possiede gia' sette strati importanti:

1. **forma linguistica:** `lexeme`, classi grammaticali, `intent_phrase`,
   `intent_cue`, `tr`, `phrase_canon`;
2. **percezione tipizzata:** `register_evidence`, `segment_role`,
   `faculty_for`;
3. **mondo e concetti:** fatti di dominio, tassonomie, relazioni, quantita';
4. **procedure:** clausole in `procedures.p0`, operatori di confronto,
   spiegazione, processo e conteggio;
5. **ponti interrogativi:** `answer_frame`, `aggregate_frame`, `question_form`;
6. **presentazione:** `answer_plan`, `atom_frame`, `task_response_frame`,
   `response_template`, `concept_label`;
7. **stato riflessivo e dialogico:** `utterance`, `entity_mentioned`,
   `active_constraint`, topic, gap, provenance e auto-modello.

Questi strati dimostrano che non serve ripartire da zero. Serve farli convergere
su una rappresentazione comune del turno e della risposta.

### 3.1 Il debito piu' rivelatore

`intent_cue/2` ha 194 intenti distinti; 49 portano ancora nomi come
`50_self_research_loop_chain63` o `65_induce_verify_shell_chain500`. Sono fatti
KB-first quanto alla collocazione, ma non ancora quanto all'astrazione: la KB
sta nominando la riga del consumer C, non il significato della mossa.

All'altro estremo, una parte di `response_template` contiene risposte complete
e specifiche. Anche qui il testo e' nella KB, ma il riuso e' basso: recuperare
una prosa terminale non equivale a costruire una risposta.

Il problema centrale non e' quindi soltanto **dove** vive la conoscenza, ma a
quale livello di generalita' viene rappresentata.

## 4. Diagnosi: il livello mancante e' la decisione linguistica

Oggi una parte consistente del percorso resta:

```text
cue di superficie -> nome di una faculty -> risposta locale
```

La KB descrive bene molti ingressi e molte uscite, ma descrive meno bene cio'
che sta in mezzo:

- quale proposizione il turno esprime;
- quali alternative interpretative erano possibili;
- quale contesto le favorisce;
- quale questione e' aperta nella conversazione;
- quale risposta e' dovuta adesso;
- quali proposizioni vanno dette e in quale relazione retorica;
- quale forma e' adatta a lingua, dominio, interlocutore e registro.

Un LLM di frontiera sembra naturale soprattutto in questo spazio intermedio.
Per avvicinarsi empiricamente, parrot0 deve trasformarlo da comportamento
implicito dei consumer a oggetti della KB.

## 5. Scala delle astrazioni da elevare

La scala seguente non e' una lista di moduli da costruire in blocco. Ogni
livello deve essere tirato da un fallimento reale e introdotto in modo additivo.
L'ordine esprime dipendenze logiche.

### K0 — Forme, sensi e menzione

**Stato attuale (gen392).** `concept_label(Concept, Lang, Register, Name)` resta
la sorgente unica delle forme. Le viste di gen391 la rendono bidirezionale;
gen392 separa la denotazione dal dominio e impedisce alla canonicalizzazione di
collassare uno span che la KB classifica come menzione. La materializzazione di
un frame per-turno resta invece responsabilita' di gen393.

**Astrazione necessaria.** La forma linguistica deve essere un oggetto della KB,
distinto dal concetto che denota. Uso e menzione devono essere due ruoli dello
stesso span, non due parser.

Il dialetto eseguibile ha arita' massima 4: la tupla concettuale a cinque assi
non va troncata e non giustifica un allargamento del C. Viene normalizzata in
viste ortogonali con una sola sorgente lessicale:

```prolog
linguistic_form(Form, Concept, Language, Register) :- concept_label(Concept, Language, Register, Form).
concept_in_domain(Concept, Domain) :- domain_category(Domain, Category), category_member(Category, Concept).
form_denotes(Form, Language, Domain, Concept) :- linguistic_form(Form, Concept, Language, Register), concept_in_domain(Concept, Domain).
denotation_register(Form, Domain, Concept, Register) :- linguistic_form(Form, Concept, Language, Register), concept_in_domain(Concept, Domain).
candidate_denotation(Form, Concept) :- form_denotes(Form, Language, Domain, Concept).
```

Il confine uso/menzione riusa il modello universale dell'input; non nasce un
secondo vocabolario di span:

```prolog
code_register(quoted, quotation).
register_role(quoted, mention).
segment_role(mention, "the word").
canonicalization_exempt(mention).
```

La proprieta' indispensabile resta la separazione fra forma, senso, concetto e
contesto. Una nuova cue di menzione deve diventare efficace per assert/retract,
senza ricompilare; il ratchet gen392 lo prova con una superficie inventata.

**Capacita' sbloccate:** domande sulle parole, sinonimia contestuale,
terminologia preferita, omonimia, citazioni, correzioni lessicali e
disambiguazione per dominio.

### K0b — Ponti fra entita' e proiezione delle faccette

**Stato attuale.** `requires(poker, deck)` e la regola che propaga
`quantity(deck, cards, 52)` verso il poker sono il modello corretto: un fatto di
dominio piu' una regola generale. `concept_label` non deve essere esteso fino a
far sembrare identiche entita' che hanno soltanto una relazione.

**Astrazione necessaria.** La lettura del turno deve distinguere:

- stessa entita' sotto forme diverse: risolta da forma/senso;
- entita' dipendente da un'altra: `requires`, `uses`, `contains`, `part_of`;
- entita' a granularita' diversa: intero, componente, categoria, esemplare;
- nessun ponte provato: ipotesi di lacuna, non ereditarieta' inventata.

Schema diagnostico candidato:

```prolog
question_entity(Turn, Role, Entity).
fact_bearing_entity(Relation, Entity).
entity_bridge_candidate(Turn, Asked, Bearing, Bridge).
entity_bridge_evidence(Turn, Bridge, Evidence).
missing_entity_bridge(Turn, Asked, Relation).
```

Le regole di proiezione devono rispettare la semantica della faccetta. Una
quantita' dell'attrezzo puo' qualificare un'attivita' che lo richiede; non segue
che ogni proprieta' dell'attrezzo appartenga all'attivita'. Non va introdotta
un'ereditarieta' universale per comodita'. Ogni classe di proiezione deve
dichiarare direzione, relazione ponte e faccetta ammessa.

**Capacita' sbloccate:** domande che attraversano attivita'-strumento,
insieme-componente, evento-partecipante e concetto-esemplare senza duplicare i
fatti sull'entita' nominata nel prompt.

### K1 — Frame semantico del turno

**Stato attuale.** Le span di `universal-input` hanno registri e ruoli, mentre
la Task IR di `learning-mesh` descrive operazione, deliverable, argomenti,
premesse e vincoli. Le due idee non sono ancora la rappresentazione ordinaria
di ogni turno.

**Astrazione necessaria.** Ogni turno compreso produce un frame semantico
indipendente dalla faculty che lo consumera'.

```prolog
utterance_frame(Turn, Frame).
frame_act(Frame, Act).
frame_slot(Frame, Role, Value).
frame_constraint(Frame, Constraint).
frame_presupposition(Frame, Proposition).
frame_source(Frame, Role, Span).
frame_language(Frame, Language).
```

Esempi di `Act` generali: `ask`, `assert`, `request`, `correct`, `clarify`,
`compare`, `explain`, `stipulate`, `mention`, `acknowledge`. `count`, `chess` o
`birthday` non sono atti: sono operatori o domini collegati agli slot.

Una parafrasi nuova deve produrre lo stesso frame; una parola nuova deve poter
entrare in un ruolo gia' noto; una faculty deve consumare il frame, non
riconoscere di nuovo la frase.

### K2 — Letture candidate ed evidenza

**Stato attuale.** `register_evidence` e lo scorer universale sanno conservare
ipotesi e dichiarare parita'. Gen389 ha introdotto letture multiple per un caso
di conteggio, ma non esiste ancora un oggetto universale "lettura del turno".

**Astrazione necessaria.** Applicare lo stesso modello a senso lessicale,
intento, scope, riferimento, quantificazione e pragmatica.

```prolog
candidate_reading(Turn, Reading).
reading_frame(Reading, Frame).
reading_evidence(Reading, Evidence).
reading_against(Reading, Evidence).
reading_relation(Reading, Other, compatible).
reading_relation(Reading, Other, alternative).
reading_status(Reading, selected).
reading_status(Reading, retained).
```

La decisione non deve ridursi sempre a un punteggio unico. Servono almeno tre
esiti:

- una lettura domina: rispondi e, se utile, dichiara la chiave;
- due letture sono vicine ma compatibili: rispondi alla probabile e aggiungi
  l'alternativa rilevante;
- le letture portano ad azioni incompatibili o rischiose: chiedi chiarimento.

Questa e' la generalizzazione della mossa di gen389, non un altro contatore.

### K3 — Stato dialogico: questione aperta, obblighi e mosse

**Stato attuale.** La colla conserva entita', topic, vincoli, risultati e
correzioni, ma soprattutto come fatti separati. Manca un modello della
conversazione come sequenza di mosse che aprono e chiudono obblighi.

**Astrazione necessaria.** Rappresentare almeno:

```prolog
dialogue_move(Turn, Move).
move_content(Move, Proposition).
move_addresses(Move, Issue).
open_issue(Issue, Context).
issue_status(Issue, open).
issue_status(Issue, resolved).
answer_obligation(Issue, Agent).
grounding_status(Proposition, proposed).
grounding_status(Proposition, accepted).
grounding_status(Proposition, corrected).
```

Regole di politica dialogica, anch'esse KB:

```prolog
appropriate_move(Context, answer) :- answerable_issue(Context, Issue).
appropriate_move(Context, clarify) :- unresolved_blocking_ambiguity(Context, Issue).
appropriate_move(Context, qualify) :- answerable_with_competing_reading(Context, Issue).
appropriate_move(Context, repair) :- corrected_commitment(Context, Proposition).
appropriate_move(Context, acknowledge) :- new_shared_constraint(Context, Constraint).
```

Queste regole spiegano perche' una risposta naturale a volte e' un numero, a
volte un numero con chiave, a volte una correzione discreta e a volte una
domanda. La naturalezza va cercata prima nella mossa, poi nella frase.

### K4 — Contesti, scope e prospettive

**Stato attuale.** La KB ha provenienze e fatti ipotetici, ma `one-kb.md`
mostra che mondo e premesse sono ancora spesso consumati da percorsi separati.
Il registro e' in parte globale. Fonte, parlante, tempo e validita' non formano
ancora un modello uniforme.

**Astrazione necessaria.** Un fatto puo' valere in un contesto senza sparire
dagli altri contesti.

```prolog
context(Context, Kind).
context_parent(Context, Parent).
context_speaker(Context, Speaker).
context_domain(Context, Domain).
context_language(Context, Language).
context_time(Context, Time).
holds_in(Context, Proposition).
source_of(Proposition, Source).
confidence_of(Proposition, Confidence).
supersedes(New, Old, Context).
contradicts_in(P, Q, Context).
```

`Kind` puo' distinguere `world`, `conversation`, `hypothesis`, `quotation`,
`reported_belief`, `fiction` e `task_premise`. La meccanica puo' interrogare
scope diversi; la KB decide quale scope e' pertinente e se vadano confrontati.

**Capacita' sbloccate:** what-if onesto, conflitti fra premessa e mondo,
discorso riportato, correzioni locali, opinioni attribuite, finzione e
conoscenza temporalmente qualificata.

### K5 — Registro come vettore contestuale

**Stato attuale.** `concept_label/4`, `part_excluded/3`, `register_trigger/2`,
`label_status/2`, `style_temperature/1` e `active_constraint/1` sono semi
importanti ma non ancora una teoria comune. Un registro non e' soltanto
`common` o `fsi`: comprende formalita', tecnicita', concisione, cortesia,
direttezza, hedging e densita' esplicativa.

**Astrazione necessaria.** Separare dimensioni indipendenti e comporle nel
contesto.

```prolog
register_value(Register, Dimension, Value).
context_preference(Context, Dimension, Value).
audience_property(Context, Property, Value).
register_fallback(Register, Parent).
register_compatible(Context, Register).
realization_feature(Form, Dimension, Value).
```

Dimensioni iniziali, da introdurre soltanto quando tirate da casi reali:

- `formality`;
- `technicality`;
- `verbosity`;
- `directness`;
- `politeness`;
- `hedging`;
- `terminology`;
- `language`.

Il registro di comprensione e quello di risposta non devono coincidere. La
regola osservata in gen390 e' generale: accettare una forma marcata non obbliga
a rispecchiarla; la risposta puo' usare la forma compatibile non marcata senza
mettersi a correggere l'utente.

Lo stile resta subordinato a verita' e calibrazione. Nessun profilo puo' rendere
assertiva una proposizione non provata.

### K6 — Piano proposizionale e realizzazione

**Stato attuale.** `answer_plan`, `atom_frame` e `task_response_frame` mostrano
la direzione giusta. Molti `response_template`, pero', contengono ancora intere
risposte e quindi mescolano contenuto, mossa e forma.

**Astrazione necessaria.** Prima costruire cio' che va detto; poi ordinare le
proposizioni; infine scegliere come dirle.

```prolog
answer_content(Answer, Proposition, Role).
answer_support(Answer, Proposition, Proof).
answer_relation(P, Q, elaborates).
answer_relation(P, Q, contrasts).
answer_relation(P, Q, qualifies).
answer_relation(P, Q, justifies).
answer_order(Answer, Proposition, Position).
realization_candidate(Proposition, Context, Form).
```

Ruoli iniziali:

- `direct_answer`;
- `interpretation_key`;
- `support`;
- `alternative_reading`;
- `assumption`;
- `qualification`;
- `limitation`;
- `next_step`.

Una risposta come quella sul numero dei pezzi diventa cosi' un piano:

```text
direct_answer(32)
interpretation_key(total)
alternative_reading(per_side, 16)
support(composition(...))
```

Il renderer non decide che cosa sia vero e non introduce nuove proposizioni.
I template diventano micro-frame riusabili per relazioni retoriche e accordo,
non contenitori di risposte benchmark-specifiche.

### K7 — Memoria discorsiva strutturata

**Stato attuale.** `utterance/3`, `entity_mentioned`, topic e fatti personali
permettono continuita' reale. Per dialoghi lunghi, una cronologia di testi e
ultimi-elementi non basta.

**Astrazione necessaria.** La memoria deve conservare eventi dialogici e
proposizioni, non soltanto stringhe o un singolo valore saliente.

```prolog
discourse_unit(Unit, TurnRange).
unit_move(Unit, Move).
unit_about(Unit, Entity).
unit_contains(Unit, Proposition).
unit_goal(Unit, Goal).
salient_in(Entity, Context, Weight).
relevant_to(Unit, Issue).
summary_contains(Summary, Proposition).
summary_supersedes(Summary, Unit).
```

La sintesi non deve essere una frase opaca che rimpiazza la storia. Deve essere
un insieme piu' piccolo di proposizioni collegate alla provenienza originale.
Il forgetting deve abbassare salienza e accessibilita', non trasformare in falso
cio' che non e' piu' in primo piano.

### K8 — Meta-conoscenza dei ponti e auto-evoluzione

**Stato attuale.** `entity_facet`, `facet_reachable`, `gap_source` e
`gap_remedy` hanno dimostrato che una singola porta puo' liberare molte regioni
della KB. `consumer_reads/2` e' previsto ma non popolato in modo generale.

**Astrazione necessaria.** La KB deve poter distinguere almeno quattro tipi di
lacuna:

```text
surface gap:       il frame esiste, manca una forma che lo raggiunga
semantic gap:      la forma e' capita, manca il concetto/relazione
operator gap:      i fatti esistono, manca una trasformazione riusabile
realization gap:   la proof esiste, manca una forma adatta per esporla
```

Fatti candidati:

```prolog
consumer_reads(Faculty, Relation).
consumer_produces(Faculty, Representation).
relation_surface(Relation, Frame).
relation_renderer(Relation, Plan).
gap_kind(Gap, Kind).
gap_evidence(Gap, Evidence).
gap_remedy_kind(Kind, Remedy).
```

Questo livello serve a scegliere **che cosa insegnare**, non a inventare la
risposta. La priorita' di un arco e' il suo fan-out: quante domande e quanti
domini rende raggiungibili.

### K9 — Operatori trasferibili, non altre risposte

La KB ha molti fatti e pochi operatori rispetto alla sua scala. Il salto verso
un comportamento da modello di frontiera richiede aumentare la densita' di
regole che producono conclusioni nuove.

Famiglie prioritarie:

- selezione di una lettura sotto contesto;
- confronto fra letture e scope;
- rilevanza rispetto alla questione aperta;
- sufficienza dell'informazione;
- revisione di credenze dopo una correzione;
- spiegazione contrastiva: perche' X e non Y;
- scelta sotto obiettivi, vincoli e trade-off;
- decomposizione di richieste multiple;
- costruzione e verifica di procedure;
- pianificazione retorica da proof e stato dialogico.

Una regola conta come avanzamento solo se:

1. produce una conclusione non gia' presente come fatto terminale;
2. funziona in almeno tre domini non correlati;
3. la sua ablazione rompe piu' domini;
4. un fatto locale rotto danneggia soltanto il proprio dominio;
5. non contiene lessico o entita' del prompt campionato.

## 6. Un'unica architettura concettuale

I livelli precedenti possono essere letti come quattro piani, non come nove
sistemi indipendenti:

```text
PERCEZIONE
  forme -> span -> sensi -> letture -> frame

DELIBERAZIONE DIALOGICA
  frame + contesto + issue -> mossa -> goal

RAGIONAMENTO
  goal + fatti + scope + operatori -> proof/proposizioni

REALIZZAZIONE
  proposizioni + relazione retorica + registro -> testo
```

La KB attraversa tutti e quattro. Il C, quando servira', potra' soltanto fornire
meccaniche invarianti: span, unificazione, enumerazione, ordinamento, binding,
cache e limiti. Questo piano non autorizza nessun consumer linguistico nuovo in
C.

## 7. Piano ordinato di espansione

### Fase 0 — Audit semantico della KB

Obiettivo: sapere quali relazioni descrivono mondo, superficie, procedura,
stato, proof e presentazione.

Azioni future:

1. censire predicati, arita', file proprietario, consumer e renderer;
2. classificare i 194 intenti per atto, operatore, dominio o nome tecnico;
3. individuare i 49 intenti con nome di chain C e assegnare loro un significato
   stabile, senza rimuovere subito le strutture secondarie;
4. classificare i template in micro-frame, frame composizionali e risposte
   terminali;
5. misurare relazioni con fatti ma senza porta, porta ma senza dati, proof senza
   renderer e renderer senza proof;
6. classificare i fallimenti reali sui tre assi del ponte, includendo i casi in
   cui il dato e' raggiungibile soltanto da un'altra entita'.

Uscita: una mappa del grafo, non una modifica comportamentale.

### Fase 1 — Forma/senso/contesto

Obiettivo: chiudere insieme i casi `cavallo`, terminologia marcata e
uso/menzione.

Ordine:

1. rappresentare denotazioni dipendenti dal dominio;
2. derivare le viste compatibili con `concept_label` e `tr`;
3. rappresentare lo span menzionato senza canonizzarlo come uso;
4. scegliere la forma d'uscita da lingua, dominio, registro e status;
5. mantenere esplicite le ambiguita' non risolte.

In parallelo, il residuo tipizzato del turno deve distinguere una denotazione
mancante da un ponte mancante fra entita'. Senza questa distinzione il sistema
continuerebbe a scambiare "non ho risposto" per "non so".

Questa fase non e' chiusa se risolve soltanto gli scacchi. Deve trasferire ad
almeno due omonimie di domini diversi e a una nuova lingua/forma insegnata a
runtime.

### Fase 2 — Frame e letture universali

Obiettivo: far produrre a ogni turno compreso un frame e una o piu' letture,
prima del dispatch.

Ordine:

1. partire da `ask`, `assert`, `request`, `correct`, `stipulate`;
2. portare nel frame argomenti, vincoli, quantificatori e fonte span;
3. riusare lo scorer di evidenza per le letture senza imporre sempre un winner;
4. rendere il residuo tipizzato del muro una lettura incompleta dello stesso
   schema;
5. far dichiarare alla KB quale faculty consuma quale frame.

Esito atteso: una parafrasi cambia superficie ma non frame; un'ambiguita'
cambia l'insieme delle letture, non il codice raggiunto.

### Fase 3 — Politica delle mosse dialogiche

Obiettivo: decidere `answer`, `clarify`, `qualify`, `repair`, `acknowledge`,
`decline` o `continue` dai fatti sul dialogo.

Ordine:

1. introdurre issue aperte e obblighi di risposta;
2. collegare correzioni e chiarimenti all'issue che modificano;
3. generalizzare la mossa gen389 a qualunque insieme di letture;
4. far sopravvivere atto e issue allo stripping delle aperture;
5. registrare esito e motivazione della scelta.

Le prime mosse da formalizzare non vanno inventate: sono quelle invarianti gia'
isolate in `question-emergence.md`:

- `propose_hypothesis` senza applicarla in silenzio;
- `enumerate_alternatives` quando piu' candidati restano vivi;
- `answer_with_interpretation_key` quando una lettura domina;
- `add_nearby_reading` quando l'alternativa e' utile ma non bloccante;
- `show_decomposition` quando rende verificabile la risposta;
- `clarify` soltanto quando scegliere scaricherebbe un rischio reale;
- `leave_unchanged` quando il controllo negativo mostra che non c'e' nulla da
  riparare.

Questa e' la fase con il maggiore impatto percepito sulla naturalezza: evita
risposte localmente corrette ma dialogicamente sbagliate.

### Fase 4 — Scope e credenze concorrenti

Obiettivo: tenere insieme mondo, premessa, ipotesi, citazione e credenza
attribuita, poi scegliere o confrontare gli scope.

Ordine:

1. rendere i contesti oggetti KB;
2. associare proposizioni, fonti e confidenza ai contesti;
3. derivare conflitti senza cancellare una delle viste;
4. distinguere stipulazione da asserzione;
5. produrre risposte che etichettano le viste quando entrambe sono rilevanti.

### Fase 5 — Registro, interlocutore e stile

Obiettivo: passare da interruttori globali a vincoli contestuali componibili.

Ordine:

1. formalita' e tecnicita';
2. concisione e densita' esplicativa;
3. cortesia, direttezza e hedging;
4. fallback fra registri;
5. profili come preferenze sulle forme, mai sulle verita'.

La stessa proof deve poter essere resa in registri diversi senza cambiare il
contenuto proposizionale.

### Fase 6 — Risposta da proposizioni

Obiettivo: ridurre la dipendenza da risposte terminali e aumentare la
composizione.

Ordine:

1. rappresentare ruoli e relazioni fra proposizioni;
2. derivare un answer plan dalla mossa e dalla proof;
3. riusare micro-frame per collegare, qualificare e contrastare;
4. scegliere forme compatibili col registro;
5. verificare che il renderer non aggiunga claim.

Le risposte terminali esistenti restano strutture secondarie. Non vanno rimosse
finche' una via composizionale non dimostra maggiore copertura e fedelta'.

### Fase 7 — Memoria lunga

Obiettivo: mantenere 20, 50 e 100 turni senza ridurre la conversazione a ultimi
campi o riassunti opachi.

Ordine:

1. unita' discorsive e topic stack;
2. legame fra issue, goal, entita' e proposizioni;
3. salienza e rilevanza dichiarative;
4. sintesi proposizionale con provenance;
5. forgetting come accessibilita' decrescente, non cancellazione semantica;
6. riapertura di topic e goal precedenti.

### Fase 8 — Crescita degli operatori

Obiettivo: aumentare la capacita' di produrre nuove conclusioni e nuove
risposte da conoscenza gia' presente.

Ogni giro parte da un fallimento held-out, estrae la Task IR, cerca un operatore
esistente e aggiunge una regola soltanto se trasferisce fra domini. Il target
non e' un numero di regole: e' il fan-out medio di ciascuna regola nuova.

La prima classe di operatori su cui investire e' la proiezione controllata lungo
ponti fra entita': e' l'asse che una scansione statica non puo' enumerare e che
massimizza il riuso di dati gia' presenti.

## 8. Fatti e regole: dove dovranno vivere

La posizione su disco e' una decisione sul comportamento, non sulla sola
navigabilita'. Il file e' l'unita' condivisa da quattro meccanismi:

```text
file_attribute  -> provenienza dei predicati fisicamente dichiarati
include         -> dipendenza idempotente nel grafo della KB
lazy_load       -> unita' di catalogazione e residenza
profilo         -> manifesto dei file eager e dei contesti lazy disponibili
```

I file vanno quindi tagliati per coesione semantica e ciclo di vita. Colla,
porte e indici di contesto restano eager; un insieme di fatti e regole che si
attiva insieme costituisce un payload lazy. Un aggregatore contiene include e
non si appropria della provenienza dei figli. Due profili possono raggiungere
lo stesso file da rami diversi senza duplicarlo: l'identita' progettata e' il
path canonico nella hashmap dell'istanza KB.

| conoscenza | proprietario futuro | nota |
|---|---|---|
| forme, sensi, denotazioni | `lexicon/gloss` o un file semantico comune | non nel dominio del consumer |
| grammatica e ruoli di superficie | `grammar/intents/input` | classi aperte, insegnabili |
| frame e letture | core semantico comune | indipendenti dalle faculty |
| atti, issue e mosse | core dialogico comune | stato di sessione come fatti |
| contesti e scope | core meta/procedure | una KB, viste concorrenti |
| registro e stile | presentation + profili | subordinate alla proof |
| operatori | `procedures.p0` o reasoning comune | almeno tre domini |
| fatti del mondo | expert/domain file | nessuna frase-risposta |
| answer plan e micro-frame | presentation/responses | nessun claim nuovo |
| auto-osservazione e gap | meta/procedures | misura ponti e copertura |
| catalogo di residenza | profilo/core eager + intestazioni expert | i predicati-porta restano visibili; il payload puo' essere lazy |

Prima di aggiungere un predicato nuovo vanno poste quattro domande:

1. e' una relazione nuova o un'etichetta diversa di una relazione esistente?
2. quale consumer generico la legge gia'?
3. quale conclusione nuova permette di derivare?
4. il nuovo membro puo' essere insegnato e ritratto a runtime?

Se non esiste un consumer, il fatto proposto e' soltanto schema morto. Va
documentato come tale e non contato come capacita'.

Per gli archi fra entita' vale un vincolo ulteriore: alias, dipendenza e
mereologia non sono sinonimi. Va riusata la relazione semanticamente corretta
(`requires`, `part_of`, `contains`, ecc.); un generico `related_to` non abilita
alcuna inferenza affidabile.

## 9. Confronto empirico con un LLM di frontiera

Il confronto non deve premiare la somiglianza letterale. Deve confrontare la
struttura della condotta dialogica.

### 9.1 Unita' di confronto

Per ogni scenario si annotano:

```text
lettura scelta
letture alternative conservate
mossa dialogica
proposizioni affermate
scope e assunzioni dichiarate
supporto/proof
registro adottato
continuita' con i turni precedenti
calibrazione del limite
```

Due risposte con parole diverse sono equivalenti se compiono la stessa mossa,
rispettano gli stessi impegni e non divergono nelle proposizioni decisive.

### 9.2 Batterie necessarie

Tutte le future prove di regressione del progetto devono restare nel framework
`.p0t`; eventuali trascritti di modelli esterni servono soltanto alla scoperta
delle mosse.

| batteria | fenomeno |
|---|---|
| parafrasi | stessa semantica con superficie diversa |
| ambiguita' | scegliere, qualificare o chiarire in modo appropriato |
| ponte fra entita' | raggiungere un dato senza duplicarlo sull'entita' chiesta |
| uso/menzione | parlare di una parola senza usarla come concetto |
| registro | capire una forma e rispondere in un'altra compatibile |
| pragmatica | richieste indirette, implicature e presupposizioni |
| correzione | modificare commitment e ri-derivare |
| multi-goal | soddisfare tutte le richieste coordinate |
| scope | mondo, ipotesi, citazione e opinione attribuita |
| conversazione lunga | riferimenti, topic, vincoli e goal a 20/50/100 turni |
| cross-domain | stessa regola su almeno tre mondi |
| negativo vicino | non applicare la lettura dove manca la prova |

### 9.3 Metriche

- **move agreement:** accordo sulla mossa dialogica rispetto al riferimento;
- **semantic coverage:** quota di richieste e vincoli rappresentati nel frame;
- **context carry:** fatti, issue e vincoli correttamente mantenuti;
- **calibration:** risposte certe, qualificate e declini coerenti con la proof;
- **register fit:** compatibilita' di lingua, registro e livello di dettaglio;
- **paraphrase invariance:** stabilita' del frame sotto variazione di superficie;
- **transfer fan-out:** domini nuovi sbloccati da una regola;
- **novel conclusion rate:** proposizioni derivate e non memorizzate;
- **prompt leakage:** lessico specifico del benchmark nelle regole comuni;
- **bridge leverage:** domande rese raggiungibili da un singolo arco;
- **three-axis gap accuracy:** corretta distinzione fra superficie, dato e
  ponte fra entita';
- **latency by KB size:** costo dello stesso turno al crescere della KB.

La metrica principale non puo' essere il wall-rate: una risposta fluente ma
semanticamente incompleta e' peggiore di un declino preciso.

## 10. Contratto di performance dell'inferenza

La KB deve poter crescere senza rendere l'inferenza inutilizzabile. Il budget
ordinario di un turno `.p0t` resta **1 secondo anche col profilo AGI**.

Regole operative per i giri futuri:

1. nessun `!timeout` per coprire una normale query KB;
2. nessun aumento del budget di `soft-test`;
3. misurare la stessa inferenza su core e AGI;
4. se il costo cresce con fatti non pertinenti, classificare il difetto come
   meccanica del solver/indicizzazione, non come limite naturale della KB;
5. separare tempo di reload/boot dal tempo del turno;
6. conservare una sonda AGI nel ratchet quando il difetto e' corretto;
7. un operatore che supera il budget non viene promosso, anche se corretto.

Le eccezioni possono esistere soltanto per atti intrinsecamente esterni, come
compilare un artefatto, e devono nominare quel costo. L'inferenza ordinaria non
e' un'eccezione.

### 10.1 Residenza contestuale: `:- lazy_load(...)`

La crescita della KB richiede di separare **conoscenza disponibile** da
**conoscenza gia' residente**. La feature proposta e' la direttiva:

```prolog
:- lazy_load(chess_context).
:- lazy_load(any(chess_context, legal_move, opening)).
:- lazy_load(all(context_games, topic_chess)).
:- lazy_load(all(context_games, any(topic_chess, legal_move))).
```

Al bootstrap il loader registra file e condizione, poi non materializza il
corpo successivo. Il file viene caricato una sola volta quando la frontiera SLD
contiene il predicato semplice, almeno un membro di `any`, oppure tutti i membri
di `all`. `any` e `all` sono termini componibili, non parole del prompt. Il
contratto completo e' in
[`docs/prolog-like-engine.md` §1.2](../prolog-like-engine.md#12-lazy_load1-specifica-della-feature-di-residenza).

Questa separazione aggiunge un livello K10 alla scala: **il grafo sa quali
contesti puo' rendere residenti senza fingere che siano gia' aperti**. La spina
eager contiene colla, frame, indice dei contesti e predicati-porta; i file expert
contengono il payload lazy. Se la colla necessaria ad aprire un contesto viene
messa dentro quel contesto, la KB crea un bootstrap impossibile.

`include/1` e `lazy_load/1` condividono una registry per path canonico. Caricare
lo stesso file attraverso piu' profili, bundle o rami di un diamante non duplica
ne' fatti ne' regole. La registry distingue caricamento in corso, provider lazy
catalogato, materializzazione e file caricato; un ciclo viene fermato e
diagnosticato. Oggi questa idempotenza non esiste ancora a livello di file: la
deduplica dei fatti non impedisce al loader di aggiungere due volte una regola.

Il profilo e' percio' una caratterizzazione del comportamento. Senza barriere,
i suoi include scelgono cosa entra in memoria al boot; con le barriere scelgono
sia il nucleo residente sia l'insieme dei contesti catalogati che potranno
diventare residenti in seguito.

L'obiettivo non e' aggiungere il profilo dopo un boot deciso dal C. Il profilo
diventa l'**unico entrypoint curato**: `brain_create` alloca la KB, una sola
`kb_load(Profile)` percorre il grafo eager/lazy e soltanto dopo il processo
proietta i fatti runtime. La migrazione dai caricamenti differenziali correnti e'
specificata in [`docs/kb-loading-and-profiles.md`](../kb-loading-and-profiles.md).

Questo vale anche per i caricamenti parziali oggi sparsi nei consumer:
`lexeme.p0`, `actions.p0`, `compose.p0` e `algo_steps.p0` non devono restare
mini-entrypoint C con un proprio flag. Il profilo li rende provider disponibili;
il consumer formula un goal e il resolver applica la barriera logica. Il gate
finale non cerca soltanto un boot piu' corto: pretende che nessun modulo conosca
il path di un file curato.

Il caricamento non puo' dipendere da token o nomi di dominio cablati nel C. E'
il solver che osserva predicati logici gia' prodotti dalla KB. `all(A,B)` vale
soltanto quando A e B sono nella stessa risoluzione: non accumula coincidenze fra
turni non collegati. Se il topic deve persistere, lo stato dialogico esplicito
deve rimettere il relativo goal nella nuova derivazione.

La lazy load non chiude il problema di latenza dell'inferenza. Per ogni sonda si
misurano separatamente:

1. boot con il solo catalogo;
2. primo accesso cold, che comprende I/O e parsing;
3. accesso warm, con contesto gia' residente;
4. equivalenza della risposta e della proof rispetto al caricamento eager.

Il punto 3 resta sotto il secondo senza `!timeout`. Se la query warm rallenta al
crescere di clausole non pertinenti, il difetto e' ancora nel solver o nei suoi
indici; nascondere quelle clausole dietro una barriera non lo corregge.

## 11. Metodo TDD futuro

Questo documento non introduce test. Quando iniziera' l'implementazione, ogni
incremento seguira' questo ciclo:

1. un `.p0t` held-out mostra il fallimento;
2. un controllo negativo delimita la classe;
3. si aggiungono fatti o regole generali;
4. assert runtime di un membro nuovo cambia il comportamento;
5. retract dello stesso membro rimuove il comportamento;
6. ablazione della regola rompe piu' domini;
7. il caso AGI resta sotto 1 secondo senza override;
8. `make soft-test` accompagna gli scambi intermedi;
9. la suite completa e' il gate finale.

Golden response e runtime growth provano cose diverse e servono entrambe. Il
primo prova il comportamento; il secondo prova che l'architettura e' KB-first.

## 12. Cose da non fare

- non aggiungere un modulo C per ogni fenomeno linguistico;
- non trasformare catene di `cue` in fatti con nomi di righe C e chiamarlo
  punto d'arrivo;
- non aggiungere altre risposte complete per coprire prompt campionati;
- non usare il topic come sostituto dell'atto o del frame;
- non scegliere una lettura cancellando strutturalmente le altre;
- non usare un registro globale quando il contesto puo' cambiarlo nel turno;
- non comprimere cento turni in una stringa non interrogabile;
- non far introdurre al renderer proposizioni che la proof non contiene;
- non popolare schemi che nessun consumer legge;
- non alzare timeout per rendere verde un'inferenza lenta;
- non misurare naturalezza come sola somiglianza di wording;
- non confondere stile da LLM con ragionamento o conoscenza importati da un LLM.

## 13. Priorita' razionale

L'ordine consigliato e':

1. **audit dei ponti e dei predicati** — evita di costruire sopra relazioni
   morte o duplicate e copre tutti e tre gli assi di `question-emergence`;
2. **forma/senso/contesto** — risolve l'asimmetria fondamentale fra input e
   `concept_label`;
3. **frame + letture universali** — crea il luogo comune in cui le faculty
   possono cooperare;
4. **mosse dialogiche** — massimizza subito la naturalezza percepita;
5. **scope concorrenti** — avvicina il comportamento che tiene insieme mondo e
   ipotesi;
6. **piano proposizionale** — sostituisce progressivamente le risposte
   terminali con composizione provata;
7. **registro multidimensionale** — rende adattiva la stessa sostanza;
8. **memoria strutturata** — porta la coerenza da pochi turni a 100;
9. **operatori trasferibili** — aumenta la profondita' cognitiva senza
   atomizzare nuovi benchmark.

Il primo incremento futuro non dovrebbe essere una nuova risposta. Dovrebbe
unire due prove minime:

1. un turno fallito produce un residuo che distingue correttamente
   `missing_fact` da `missing_entity_bridge` sui tre assi;
2. la stessa superficie genera piu' letture esplicite e il contesto ne seleziona
   una, lasciando la seconda interrogabile.

Insieme, queste prove collegano il sensore di `question-emergence` alla
decisione: parrot0 non si limita a sapere che ha fallito, ma rappresenta **dove
nel grafo** ha fallito e quale mossa sia appropriata.

## 14. Criterio di riuscita della missione

parrot0 sara' empiricamente paragonabile a un LLM di frontiera sul dialogo
quando, su scenari held-out:

1. produce frame equivalenti sotto parafrasi e lingue diverse;
2. conserva e gestisce letture alternative come il riferimento;
3. sceglie la stessa classe di mossa dialogica senza copiare la frase;
4. mantiene issue, commitment, riferimenti, vincoli e goal nei dialoghi lunghi;
5. separa mondo, ipotesi, citazioni e credenze attribuite;
6. adatta il registro senza alterare la verita';
7. costruisce le risposte da proof e piani proposizionali;
8. apprende forme, sensi, registri e mosse a runtime;
9. trasferisce ogni nuova regola a domini non visti;
10. mantiene latenza ordinaria sotto il contratto mentre la KB cresce.

Questo non dimostrera' che parrot0 e' un LLM, ne' che possiede la stessa
struttura interna. Dimostrera' qualcosa di piu' preciso e falsificabile: che una
KB esplicita, con un motore generale, riproduce una quota crescente delle
decisioni linguistiche che rendono naturale un modello di frontiera.

## 15. Piano esecutivo: sette generazioni consecutive

Questa sezione trasforma la scala precedente in sette incrementi consecutivi.
La numerazione prosegue idealmente da gen390; `VERSION` non viene usato come
autorita' semantica perche' nel repository e' rimasto indietro rispetto ai
commit. Ogni generazione ha un solo centro, ma attraversa l'intero percorso
necessario a renderlo osservabile in conversazione.

Le clausole mostrate sono **esempi guida della conoscenza da introdurre**, non
licenza a popolare subito la KB. Entrano nei file `.p0` soltanto nella propria
generazione, insieme a un consumer reale e al ratchet `.p0t`. Tutte le clausole
eseguibili dovranno essere scritte su una riga, come richiede il parser attuale.

### Quadro delle sette generazioni

| gen | centro | astrazione guadagnata | ratchet principale |
|---|---|---|---|
| 391 | forme linguistiche come oggetti | concetto, forma, lingua, registro e status diventano interrogabili | `language/forms_as_objects.p0t` |
| 392 | denotazione contestuale e ponti fra entita' | il turno raggiunge sensi e dati attraverso dominio e relazioni provate | `language/contextual_denotation.p0t` |
| 393 | frame, letture e residuo sui tre assi | ogni turno produce interpretazioni e una diagnosi strutturata del gap | `meta/three_axis_gap.p0t` |
| 394 | politica delle mosse dialogiche | la KB decide se rispondere, qualificare, proporre o chiarire | `conversation/dialogue_moves.p0t` |
| 395 | contesti e scope concorrenti | mondo, ipotesi, citazione e credenza restano visibili insieme | `reasoning/context_scopes.p0t` |
| 396 | registro multidimensionale e answer plan | una proof riceve piano e forma adatti al contesto | `generation/register_answer_plan.p0t` |
| 397 | memoria discorsiva e confronto frontier | mosse, issue, proof e registro restano coerenti in dialoghi lunghi | `conversation/frontier_dialogue.p0t` |

### Filo trasversale delle sette generazioni: residenza della KB

`lazy_load/1` attraversa le sette generazioni senza sostituirne il centro. Viene
introdotta per incrementi verificabili, dopo avere congelato l'equivalenza eager
come oracolo:

| gen | task di residenza | evidenza richiesta |
|---|---|---|
| 391 | specificare barriera, registry canonica idempotente, stati file, goal trigger ed equivalenza eager; censire boot e caricamenti parziali C | documento Prolog; inventario `lexeme/actions/compose/algo_steps`; nessun `.p0` operativo usa ancora la direttiva |
| 392 | auditare il manifesto core e dichiarare le frontiere dei provider oggi aperti dai consumer, mantenendo tutto eager | ordine e layer sono congelati; `file_layer/1` emerge come prerequisito; nessuna barriera operativa anticipa la registry |
| 393 | implementare registry canonica, provenienza fisica e `file_layer/1` | diamante e path equivalenti non duplicano fatti o regole; layer incompatibili, cicli e fallimenti sono espliciti |
| 394 | introdurre un entrypoint di profilo sperimentale e confrontarlo col boot storico | una sola radice curata produce fatti, regole, layer, proof e ordine equivalenti |
| 395 | rendere conversational e AGI profili completi | il profilo caratterizza tutto il soggetto; i vecchi assi di amputazione entrano in deprecazione |
| 396 | attivare `lazy_load` semplice, `any(...)`, `all(...)` e formule annidate; migrare i quattro carichi parziali | equivalenza eager/lazy, catalogato distinto da residente, consumer privi di path e flag, misure cold/warm |
| 397 | applicare la barriera agli expert voluminosi e rimuovere il boot nominale | unico entrypoint globale, memoria ridotta ed equivalenza warm entro il budget ordinario |

Tre vincoli impediscono che questo filo diventi una scorciatoia. Primo: la
gen391 non puo' usare la futura barriera per rendere verde il proprio gate AGI;
il costo del solver va capito sulla KB eager. Secondo: la materializzazione lazy
non precede la registry canonica, altrimenti eager e lazy avrebbero identita' e
provenienze concorrenti. Terzo: un expert viene convertito soltanto quando
esiste gia' una porta semantica eager che lo rende raggiungibile. Un file lazy
che nessun goal puo' aprire e' conoscenza morta, non ottimizzazione.

---

### gen391 — Le forme linguistiche sono oggetti della KB

**Fallimento tirante.** `concept_label/4` funziona dal concetto alla forma, ma
la KB non possiede una vista ordinaria dalla forma al concetto e allo status.
«Si dice X o Y?» resta irraggiungibile, e la canonicalizzazione puo' consumare
la distinzione prima che il turno parli delle parole.

**Task 391.1 — vista bidirezionale senza duplicare il lessico.**

Aggiunte guida in un file omogeneo `kb/core/language-forms.p0`, caricato da
`procedures.p0`:

```prolog
:- file_attribute(machinery).

linguistic_form($Form, $Concept, $Language, $Register) :- concept_label($Concept, $Language, $Register, $Form).
form_mark($Form, $Status) :- label_status($Form, $Status).
alternative_form($Form, $Alternative, $Language) :- linguistic_form($Form, $Concept, $Language, $Register), linguistic_form($Alternative, $Concept, $Language, $OtherRegister), dif($Form, $Alternative).
marked_form($Form) :- form_mark($Form, $Status).
preferred_form($Marked, $Preferred, $Language) :- alternative_form($Marked, $Preferred, $Language), marked_form($Marked), naf(marked_form($Preferred)).
alternative_label($Form, $Alternative) :- alternative_form($Form, $Alternative, $Language).
preferred_label($Marked, $Preferred) :- preferred_form($Marked, $Preferred, $Language).
```

Queste sono viste derivate: un nuovo `concept_label` entra automaticamente.
Non deve apparire una seconda tabella `form_concept` mantenuta a mano. Il file
dedicato rende corretto `file_attribute/1`: applicarlo a tutto
`procedures.p0` contaminerebbe i predicati di `meta.p0`, che contiene anche
conoscenza del mondo; ripetere sette `machinery/1` perderebbe invece il vantaggio
della provenienza per file.

**Task 391.2 — dati guida in un dominio reale.**

Aggiunte guida in `kb/experts/games/chess.p0`:

```prolog
concept_label(capture, en, common, eat).
label_status(eat, informal).
```

Le righe completano il gemello inglese del caso italiano gia' presente. Il
dominio non crea la regola; fornisce soltanto un membro reale della classe.

**Task 391.3 — porte conversazionali generali.**

Aggiunte guida in `kb/core/intents.p0`:

```prolog
answer_frame("preferred term", preferred_label).
answer_frame("alternative term", alternative_label).
answer_frame("term status", form_mark).
```

Le superfici sono fatti e devono essere insegnabili/ritrattabili. Il consumer e'
quello binario universale gia' esistente; nessun recognizer terminologico
dedicato e' ammesso.

**Task 391.4 — ratchet `.p0t`.**

`tests/p0t/language/forms_as_objects.p0t` deve coprire:

1. forma marcata -> alternativa non marcata;
2. due forme non marcate -> nessuna preferenza inventata;
3. concetto inventato a runtime -> eredita la regola;
4. retract dello status -> la preferenza sparisce;
5. nuova superficie `answer_frame` -> abilita la stessa relazione;
6. retract della superficie -> la disabilita;
7. controllo AGI sul fatto reale inglese, sotto 1 secondo e senza `!timeout`.

**Task 391.5 — gate del solver.** Se il caso ermetico passa e quello AGI supera
un secondo, la generazione resta aperta. Va isolato il costo generico della
query derivata; non si riduce il profilo e non si alza il timeout. Qualunque
eventuale modifica al motore deve essere priva di lessico e migliorare almeno
un'altra relazione derivata.

**Task 391.6 — baseline per la residenza.** Congelare un piccolo expert eager e
le sue query/proof come oracolo della futura `lazy_load/1`. In questa generazione
si documentano sintassi e invarianti ma non si converte ancora alcun file: la
barriera non deve nascondere il difetto eventualmente emerso dal Task 391.5.

La baseline comprende anche un diamante di include. Lo stesso file raggiunto da
due bundle deve contribuire una sola volta, incluse le regole; due path relativi
equivalenti devono avere la stessa chiave canonica. Un controllo separato
verifica che `file_attribute/1` appartenga al file fisico e non erediti
l'attributo dell'aggregatore che per primo lo ha incluso.

**Definizione di done.** Le forme sono interrogabili e crescono a runtime; il
caso AGI resta nel budget; il caso italiano naturale «si dice mangiare o
catturare?» puo' restare aperto fino al ruolo `mention` di gen392, ma il piano
deve dichiararlo e non simulare di averlo chiuso.

---

### gen392 — Denotazione contestuale e ponti fra entita'

**Fallimenti tiranti.** `cavallo` viene ridotto all'animale; `poker` non
raggiunge sempre i dati di `deck`; uso e menzione condividono oggi la stessa
canonicalizzazione.

**Task 392.1 — forma, dominio e concetto.**

Aggiunte guida nel nucleo semantico `denotation.p0`. `concept_label/4` resta
l'unica sorgente; dominio e registro sono viste normalizzate per rispettare
l'arita' massima 4 del dialetto senza perdere assi:

```prolog
domain_category(chess, chess_piece).
domain_category(zoology, animal).
concept_label(knight, it, common, cavallo).
concept_label(horse, it, common, cavallo).
concept_in_domain($Concept, $Domain) :- domain_category($Domain, $Category), category_member($Category, $Concept).
form_denotes($Form, $Language, $Domain, $Concept) :- linguistic_form($Form, $Concept, $Language, $Register), concept_in_domain($Concept, $Domain).
denotation_register($Form, $Domain, $Concept, $Register) :- linguistic_form($Form, $Concept, $Language, $Register), concept_in_domain($Concept, $Domain).
candidate_denotation($Form, $Concept) :- form_denotes($Form, $Language, $Domain, $Concept).
```

Non si mantengono due fonti di verita': asserire una nuova `concept_label/4` e
la membership del suo concetto rende subito derivabile la denotazione.

**Task 392.2 — uso e menzione come ruoli di span.**

```prolog
delim_pair(quotation, ', ').
code_register(quoted, quotation).
register_evidence(quoted, balanced(quotation)).
register_role(quoted, mention).
segment_role(mention, "si dice").
segment_role(mention, "the word").
canonicalization_exempt(mention).
```

`quoted` e' un registro strutturale, non un delimitatore cablato; una cue
discorsiva e una coppia bilanciata convergono sullo stesso ruolo. Il motore
applica l'operazione generica dichiarata da `canonicalization_exempt/1`, senza
conoscere il nome `mention`, la cue o la lingua. `mentioned_form/3` viene
rimandato a gen393, quando esisteranno identita' di turno e frame materializzati.

**Task 392.3 — terzo asse: entita' verso entita'.**

Riutilizzare fatti reali:

```prolog
requires(poker, deck).
requires(blackjack, deck).
requires(bridge, deck).
quantity($Activity, $Part, $N) :- requires($Activity, $Thing), quantity($Thing, $Part, $N).
```

E aggiungere meta-conoscenza soltanto se un consumer la usa:

```prolog
facet_projection(quantity, requires, required_to_requirer).
```

Non e' consentito un generico `related_to`: identita', dipendenza e parte-tutto
producono inferenze diverse.

**Task 392.4 — ratchet `.p0t`.**

1. `cavallo` in contesto scacchi -> `knight`;
2. `cavallo` in contesto zoologico -> `horse`;
3. contesto assente -> entrambe le letture restano candidate;
4. una parola citata non viene usata come concetto;
5. un'attivita' eredita una quantita' soltanto attraverso `requires`;
6. retract del ponte toglie soltanto l'ereditarieta';
7. un ponte di relazione sbagliata non propaga il dato;
8. due omonimi e un ponte fuori dai giochi provano il trasferimento.

**Definizione di done.** Il dominio sceglie senza distruggere l'alternativa,
la menzione preserva la forma e il terzo asse riusa dati senza duplicarli.

**Stato di attuazione.** 392.1-392.4 sono presenti. Il consumer
resta il proiettore universale guidato da `answer_frame/2`,
`answer_projection/2`, `projection_gate/2` e `projection_source/3`; nessun
dominio e nessuna superficie sono stati aggiunti al C. Il ratchet
`language/contextual_denotation.p0t` copre le otto condizioni, piu' crescita e
ablazione del confine di menzione con una cue inventata. `make soft-test` passa
in 6 secondi sul budget invariato di 15 e `make test` chiude 1800 asserzioni,
zero fallimenti. L'audit del diff non trova cue, lingue, domini o risposte
naturali nel C. Resta aperto soltanto il manifesto core eager del filo di
residenza; non autorizza ad anticipare la registry o `lazy_load/1`.

---

### gen393 — Frame, letture e sensore sui tre assi

**Fallimento tirante.** Il muro sa qualcosa del turno ma non produce ancora un
record uniforme che distingua superficie, dato, ponte, operatore e
realizzazione.

**Task 393.1 — frame minimo comune.**

```prolog
frame_candidate($Frame) :- frame_act($Frame, $Act).
frame_slot($Frame, relation, $Relation).
frame_slot($Frame, entity, $Entity).
frame_slot($Frame, constraint, $Constraint).
frame_source($Frame, $Role, $Span).
frame_complete($Frame) :- frame_candidate($Frame), frame_has_relation($Frame), frame_has_entity($Frame).
frame_answer($Frame, $Value) :- frame_slot($Frame, relation, $Relation), frame_slot($Frame, entity, $Entity), apply($Relation, cons($Entity, cons($Value, nil))).
```

Atto, slot e source sono forme di schema; i fatti concreti devono essere
materializzati dal turno, non curati per prompt. `frame_answer/2` usa il normale
solver: una regola derivata vale quanto un fatto e nessun consumer possiede un
secondo percorso di inferenza.

**Task 393.2 — letture concorrenti.**

```prolog
candidate_reading($Frame, $Reading) :- reading_frame($Reading, $Frame), reading_evidence($Reading, $Evidence).
selected_reading($Frame, $Reading) :- candidate_reading($Frame, $Reading), reading_status($Reading, selected).
retained_reading($Frame, $Reading) :- candidate_reading($Frame, $Reading), reading_status($Reading, retained).
```

**Task 393.3 — diagnosi dei gap.**

```prolog
gap_kind($Turn, missing_surface) :- known_relation_for_turn($Turn), naf(reachable_surface_for_turn($Turn)).
gap_kind($Turn, missing_fact) :- complete_frame($Turn, $Frame), known_entity_in_frame($Frame), naf(value_for_frame($Frame)).
gap_kind($Turn, missing_entity_bridge) :- value_on_related_entity($Turn, $Other), naf(proven_bridge_for_turn($Turn, $Other)).
gap_kind($Turn, missing_operator) :- facts_cover_frame($Turn), naf(operator_covers_frame($Turn)).
gap_kind($Turn, missing_realization) :- proof_for_turn($Turn, $Proof), naf(renderer_for_proof($Proof)).
```

**Task 393.4 — ratchet `.p0t`.** Un caso inventato per ognuno dei cinque gap;
assert del solo arco mancante chiude soltanto quel gap; controlli sui tre assi;
nessuna diagnosi da sola autorizza una risposta.

**Definizione di done.** Il muro e il successo descrivono lo stesso tipo di
frame; una lacuna e' localizzata nel grafo e porta una proof diagnostica.

**Stato di attuazione.** Il primo taglio KB-only e' in
`core/dialogue-frames.p0`: completezza, residuo di slot, proof positiva e
letture concorrenti sono viste dello stesso frame. Il ratchet
`meta/three_axis_gap.p0t` materializza un frame inventato, toglie e ripristina
uno slot, conserva due letture, cambia selected/retained e abla una superficie
`answer_frame/2` a runtime. `make soft-test` resta a 6 secondi sul budget 15;
`make test` chiude 1821 asserzioni, zero fallimenti. Il file e' deliberatamente
incompleto nel proprio confine end-to-end:

1. il produttore NL -> frame non esiste ancora, quindi la generazione non e'
   end-to-end e non va promossa.

`missing_fact` e' invece ora derivato in modo sicuro. Il solver possiede gia' i
tre esiti `proved | finite_failure | incomplete`: la NAF riesce soltanto sul
fallimento finito e declina quando il budget viene esaurito. La regola nega la
vista ground `frame_has_answer(Frame)`, non `frame_answer(Frame, Value)` con una
variabile libera. Il `.p0t` prova che assert del solo fatto chiude il gap e che
retract lo riapre. Resta aperto il falsificatore deterministico che porti davvero
la ricerca a `incomplete`, oltre ai gap di ponte, operatore e realizzazione.

Il primo rosso del ratchet ha inoltre fissato una regola del dialetto: la NAF
deve ricevere un goal ground. `frame_residue/2` passa quindi dalle viste positive
`frame_has_relation/1` e `frame_has_entity/1`, invece di lasciare una variabile
esistenziale libera dentro `naf(frame_slot(...))`.

---

### gen394 — La KB sceglie la mossa dialogica

**Fallimento tirante.** I consumer decidono localmente se rispondere o cedere;
la mossa osservata nell'oracolo non e' ancora un oggetto comune.

**Task 394.1 — issue e obblighi.**

```prolog
open_issue($Issue, $Context) :- dialogue_move($Turn, ask), move_opens($Turn, $Issue), context_of_turn($Turn, $Context).
answer_obligation($Issue, parrot0) :- open_issue($Issue, $Context).
issue_resolved($Issue) :- dialogue_move($Turn, answer), move_addresses($Turn, $Issue).
```

**Task 394.2 — mosse invarianti da `question-emergence`.**

```prolog
move_policy(single_dominant_reading, answer_with_interpretation_key).
move_policy(near_compatible_readings, add_nearby_reading).
move_policy(multiple_lexical_candidates, enumerate_alternatives).
move_policy(unverified_repair, propose_hypothesis).
move_policy(blocking_incompatible_readings, clarify).
move_policy(valid_surface, leave_unchanged).
move_policy(proven_answer_with_components, show_decomposition).
```

**Task 394.3 — precedenza semantica.**

```prolog
move_priority(answer_with_interpretation_key, clarify, preferred_when_nonblocking).
move_priority(propose_hypothesis, silent_repair, always).
move_priority(decline, unsupported_answer, always).
```

I simboli di confronto sono dati di politica; la meccanica di ordinamento resta
generale.

**Task 394.4 — ratchet `.p0t`.** Coprire `pamino`, conteggio ambiguo, domanda
non ambigua, riparazione senza candidati e alternativa bloccante. Aggiungere a
runtime una nuova policy deve cambiare la mossa senza cambiare il contenuto del
dominio; retract la ripristina.

**Definizione di done.** La stessa classe di evidenza produce la stessa mossa in
domini diversi; nessuna risposta del modello di riferimento e' memorizzata.

---

### gen395 — Contesti e scope restano visibili insieme

**Fallimento tirante.** Mondo e premessa vengono decisi da percorsi distinti;
parrot0 non puo' confrontarli o spiegare perche' ha scelto uno scope.

**Task 395.1 — contesti espliciti.**

```prolog
context(world, world).
context($Context, hypothesis).
context($Context, quotation).
context($Context, reported_belief).
context_parent($Context, world).
holds_in($Context, $Proposition).
```

**Task 395.2 — fonti, conflitti e scelta.**

```prolog
source_of($Proposition, $Source).
confidence_of($Proposition, $Confidence).
contradicts_across($P, $Q, $C1, $C2) :- holds_in($C1, $P), holds_in($C2, $Q), incompatible_propositions($P, $Q).
scope_relevant($Turn, task_premise) :- frame_act_for_turn($Turn, entailment_query).
scope_relevant($Turn, world) :- frame_act_for_turn($Turn, factual_query).
```

**Task 395.3 — stipulazione e asserzione.** Le cue restano fatti esistenti o
nuovi di grammatica; l'esito diventa `context_kind`, non una rinominazione
opaca.

**Task 395.4 — ratchet `.p0t`.** Pinguini fra premessa e mondo; citazione che non
diventa commitment del parlante; opinione attribuita; correzione locale; query
che chiede entrambe le viste. Ablare lo scope non deve cancellare i fatti degli
altri contesti.

**Definizione di done.** parrot0 puo' dare e mettere in relazione due risposte
diverse senza confonderle e senza perdere provenance.

---

### gen396 — Registro multidimensionale e risposta da proposizioni

**Fallimento tirante.** `preferred_register` e `style_temperature` sono
interruttori larghi; molti template mescolano claim, struttura e stile.

**Task 396.1 — dimensioni del registro.**

```prolog
register_value(common, technicality, low).
register_value(chess_technical, technicality, high).
register_value(concise, verbosity, low).
register_value(formal, formality, high).
context_preference($Context, $Dimension, $Value).
register_evidence(chess_technical, predicate(win_material)).
register_evidence(common, predicate(copula)).
```

Le ultime due righe rendono esplicita la scoperta di §14.3: il predicato d'uso
porta registro.

**Task 396.2 — status della forma, non del registro.**

```prolog
form_status(mangiare, it, chess, informal).
form_status(catturare, it, chess, standard).
realization_policy(marked_input, prefer_unmarked_output).
realization_policy(explicit_terminology_question, state_form_status).
```

**Task 396.3 — answer plan proposizionale.**

```prolog
answer_content($Answer, $Proposition, direct_answer).
answer_content($Answer, $Proposition, interpretation_key).
answer_content($Answer, $Proposition, alternative_reading).
answer_content($Answer, $Proposition, support).
answer_relation($P, $Q, qualifies).
answer_relation($P, $Q, justifies).
```

**Task 396.4 — ratchet `.p0t`.** Stessa proof resa comune/tecnica,
concisa/dettagliata e formale/informale; forma marcata capita ma non riflessa;
domanda esplicita sul termine dichiara lo status; nessun registro cambia i claim.
Runtime growth aggiunge una dimensione/form candidate, retract la rimuove.

**Definizione di done.** Il contenuto proposizionale resta identico sotto
registri diversi; il renderer non introduce fatti; «si dice X o Y?» si chiude
attraverso menzione + status + mossa, non con un consumer privato.

---

### gen397 — Memoria discorsiva e ratchet frontier

**Fallimento tirante.** Le faculty conservano molti ultimi-valori, ma manca una
storia strutturata di issue, mosse, proof e goal che regga cambi di topic e
ritorni dopo decine di turni.

**Task 397.1 — unita' discorsive.**

```prolog
discourse_unit($Unit, $FromTurn, $ToTurn).
unit_move($Unit, $Move).
unit_issue($Unit, $Issue).
unit_contains($Unit, $Proposition).
unit_goal($Unit, $Goal).
unit_about($Unit, $Entity).
```

**Task 397.2 — salienza, rilevanza e sintesi strutturata.**

```prolog
salient_in($Entity, $Context, $Weight).
relevant_to($Unit, $Issue).
summary_contains($Summary, $Proposition).
summary_source($Summary, $Unit).
topic_stack($Context, $Topic, $Depth).
```

Il summary non e' prosa opaca. Una proposizione sintetizzata conserva il link
all'unita' da cui proviene.

**Task 397.3 — operatori di continuita'.**

```prolog
resume_issue($Context, $Issue) :- open_issue($Issue, $Earlier), relevant_context($Earlier, $Context).
retain_constraint($Context, $Constraint) :- active_constraint($Constraint), naf(constraint_retracted($Constraint)).
prefer_recent_relevant($A, $B, $Issue) :- relevant_to($A, $Issue), naf(relevant_to($B, $Issue)).
```

**Task 397.4 — ratchet `.p0t` frontier.** Dialoghi da 20, 50 e 100 turni
composti da scenari brevi riusabili: cambio e ripresa di topic, riferimento a
entita' non ultima ma rilevante, correzione tardiva, vincolo persistente,
ipotesi chiusa, domanda terminologica e multi-goal. Il confronto usa le firme
di mossa e le proposizioni, non il wording.

**Task 397.5 — audit finale.** Misurare move agreement, context carry,
paraphrase invariance, register fit, novel conclusion rate, transfer fan-out,
three-axis gap accuracy e latenza AGI. Ogni zero deve essere classificato prima
di aprire la generazione successiva.

**Definizione di done.** I sette livelli compongono senza consumer privati; una
nuova forma, un nuovo senso, un nuovo ponte, una nuova policy o un nuovo registro
cambiano il dialogo a runtime; la suite ordinaria resta entro i budget.

## 16. Stato di avanzamento

Il piano e' partito da **gen391**. Le gen394-397 restano contratti, non
capacita' rivendicate; gen393 e' il fronte semantico corrente.

Stato della gen391:

1. **391.1-391.4 eseguiti:** le viste sono in `language-forms.p0`, il dominio
   scacchi fornisce il membro guida inglese, le porte restano `answer_frame/2` e
   il ratchet `.p0t` prova positivo, negativo, crescita e retrazione;
2. **391.5 eseguito:** la sonda ha separato boot, query Prolog e dispatch. La
   query derivata diretta e' nell'ordine dei millisecondi; il costo residuo era
   la scansione di tutte le teste di regola e il tentativo di frame generici
   prima dell'evidenza piu' specifica. Il censimento indicizza ora anche le
   teste e le superfici sovrapposte seguono una precedenza meccanica per
   specificita'. `make soft-test` resta nel budget ordinario senza timeout e la
   suite completa chiude 1782 asserzioni senza fallimenti;
3. **391.6 specificato, oracolo operativo ancora aperto:** la semantica di
   include idempotente, provenienza fisica, registry e lazy load e' documentata,
   ma nessuna direttiva lazy e' ancora dichiarata come implementata. La vecchia
   formulazione lo rendeva impropriamente un blocco dell'intero filo linguistico:
   e' invece il gate del filo di **residenza**, da chiudere prima della registry
   e di qualunque barriera operativa;
4. **filo dei profili elevato:** il target non e' piu' un profilo aggiunto al
   boot C. Ogni soggetto nasce da un solo entrypoint curato, che e' il suo
   profilo; core, bundle eager e provider lazy sono raggiungibili soltanto dal
   suo grafo di include.

La parte linguistica di gen391 e' stata promossa nel commit `be3cedf`; il debito
391.6 resta visibile e non autorizza ad anticipare `lazy_load/1`.

Stato corrente della gen392:

1. **392.1 eseguito:** `denotation.p0` deriva concetto e dominio dalla sorgente
   unica `concept_label/4` e dalle categorie esistenti, con viste di arita' 4;
2. **392.2 eseguito:** registri citati e cue discorsive convergono sul ruolo di
   span; `canonicalization_exempt/1` rende il confine estensibile dalla KB;
3. **392.3 riusato:** la proiezione `quantity/3` lungo il solo arco
   `requires/2` era gia' la regola generale corretta; non e' stato inventato un
   secondo ponte ne' un `related_to` opaco;
4. **392.4 eseguito:** il ratchet `.p0t` prova omonimia per dominio, alternative
   conservate, citazione, ponte tipato fuori dai giochi, retract e crescita del
   ruolo di menzione a runtime;
5. **filo di residenza preparato, non attivato:** gen392 deve produrre il
   manifesto core eager. L'audit ha mostrato che `include/1` da solo perderebbe
   il layer reflective di `capabilities.p0`; `file_layer/1` e' ora specificato
   come prerequisito distinto da `file_attribute/1`. La barriera lazy resta
   soltanto documentata finche' layer, entrypoint e registry non ne rendono la
   semantica univoca;
6. **verifica corrente:** `make soft-test` e' verde in 6 secondi sul budget 15;
   `make test` passa 1800 asserzioni senza fallimenti; il diff C contiene una
   sola operazione strutturale parametrizzata da `canonicalization_exempt/1`.

La gen392 puo' essere promossa soltanto dopo che:

1. denotazione e registro restano viste di `concept_label/4`, non vocabolari
   duplicati;
2. dominio presente seleziona una lettura e dominio assente conserva le
   alternative;
3. uso e menzione cambiano per fatti asseribili e retraibili a runtime;
4. soltanto un arco relazionale provato proietta una faccetta;
5. `make soft-test` resta nel budget invariato e `make test` e' verde;
6. il diff non contiene cue, lingue, domini o risposte naturali nel C;
7. il manifesto core eager del filo di profilo e' almeno definito senza
   dichiarare implementate idempotenza o lazy load.

Avvio della gen393:

1. **kernel logico presente:** frame, slot, source, completezza, residuo, proof
   positiva e letture sono oggetti della KB;
2. **crescita/ablazione presente:** il `.p0t` cambia frame e letture con
   assert/retract senza rebuild;
3. **producer aperto:** nessun consumer C locale viene ancora promosso a
   produttore universale; il prossimo taglio deve materializzare lo stesso frame
   prima del first-match dispatch;
4. **primo gap epistemico presente:** `missing_fact` nasce soltanto dalla NAF
   ground su `frame_has_answer/1`; il solver gia' distingue finite failure da
   incomplete e nel secondo caso non autorizza il gap. Restano da falsificare
   deterministicamente il caso di budget e da modellare ponte, operatore e
   realizzazione.
