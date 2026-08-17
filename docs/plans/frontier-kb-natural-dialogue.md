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

### K11 — Modello situazionale, azioni e transizioni

Fra il frame del turno e il piano proposizionale manca ancora un oggetto: una
**situazione modificabile**. E' il livello necessario per problemi di
sopravvivenza, uso astuto delle risorse, triangolazione, rilettura del contesto e
ripianificazione. Non e' un catalogo di indovinelli e non e' una faculty
`mongolfiera`: e' una rappresentazione comune in cui domini diversi possono
fornire oggetti, stati, affordance, vincoli e leggi causali.

Fatti candidati:

```prolog
situation($Situation, $Context).
situation_goal($Situation, $Goal).
situation_entity($Situation, $Entity, $Role).
state_value($Situation, $Entity, $Property, $Value).
resource_available($Situation, $Resource, $Amount).
situation_constraint($Situation, $Constraint).
hazard($Situation, $Hazard, $Severity).

action_schema($Action, $Domain).
action_precondition($Action, $Condition).
action_uses($Action, $Resource, $Amount).
action_effect($Action, $Effect).
action_side_effect($Action, $Effect).
action_duration($Action, $Duration).
action_reversibility($Action, $Class).
action_risk($Action, $Risk).

candidate_world($Situation, $World).
world_assumption($World, $Assumption).
world_evidence($World, $Evidence, $Source).
transition($World, $Before, $Action, $After).
plan_step($Plan, $Index, $Action).
plan_support($Plan, $Proof).
plan_unresolved($Plan, $Question).
```

I nomi sono un lessico di progetto, non uno schema gia' congelato. Prima di
promuoverli va riusato quanto esiste in `procedures.p0`, nei contesti K4 e nei
piani K6. Il contratto semantico, invece, e' gia' vincolante:

1. una correzione crea una nuova versione locale del mondo e rende visibile che
   cosa invalida; non riscrive silenziosamente la storia;
2. un'azione e' candidabile soltanto se le sue precondizioni sono provate o
   dichiarate come assunzioni;
3. una risorsa non menzionata e non derivabile non puo' apparire nel piano;
4. ogni passo porta effetto, durata, consumo, rischio e provenance quando
   pertinenti;
5. la ricerca puo' essere una meccanica C fissa, ma schemi d'azione, causalita',
   priorita' e policy di rischio vivono nella KB;
6. l'output del livello e' un piano con proof, alternative e residui, mai testo
   naturale terminale.

In questo livello «astuzia» significa una composizione verificabile di operatori:
scoprire un'assunzione nascosta, riclassificare una risorsa, osservare una
proprieta' laterale, scegliere un'azione informativa, confrontare mondi
alternativi o cambiare prospettiva. Non significa recuperare una risposta
memorabile associata alle parole dell'enigma.

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
| situazioni, azioni e transizioni | core planning comune + payload di dominio | schemi e policy in KB; ricerca meccanica nel motore |
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
| piano situazionale | stato, goal, risorse, azioni, effetti e rischi |
| sopravvivenza | riduzione del danno senza risorse inventate |
| triangolazione | mondi coerenti, provenance e informazione discriminante |
| azione informativa | agire per rendere osservabile una variabile nascosta |
| bilancio dinamico | durate, flussi, soglie e stati intermedi |
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
- **plan validity:** ogni passo soddisfa le precondizioni nello stato in cui
  viene eseguito;
- **causal faithfulness:** gli effetti dichiarati sono sostenuti da una regola
  causale con provenance;
- **resource discipline:** nessun oggetto, capacita' o quantita' viene inventato;
- **replanning consistency:** una correzione ritira solo passi e conclusioni che
  dipendevano dall'informazione sostituita;
- **assumption exposure:** le assunzioni che cambiano la decisione sono nominate
  e separate dalle conseguenze provate;
- **counterfactual robustness:** il piano cambia nel modo atteso quando si
  ablano risorsa, affordance o premessa;
- **safety calibration:** rischio e danno non vengono ottimizzati come semplici
  costi se la policy KB li rende vincoli;
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
generazione, insieme a un consumer reale e al ratchet `.p0t`. Il punto di
livello zero, non la newline, chiude una clausola: piu' clausole sulla stessa
riga sono valide e protette da `meta/multiclause_cues.p0t`.

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

Il prompt reale «dove si trova la ruanda» ha mostrato un secondo confine. Il
fatto `continent_of(rwanda, africa)` esisteva, ma la forma locativa non apriva la
relazione spaziale comune e l'esonimo italiano non raggiungeva l'entita'. Il
taglio KB-first aggiunge `answer_frame("where is", located_in)`, la regola
`located_in(Country, Continent) :- continent_of(Country, Continent)` e
`tr(rwanda, ruanda)`. Il ratchet usa anche Parigi e un paese inventato, e abla
separatamente fatto, porta ed esonimo. Questo chiude il caso di ponte, non il
producer NL -> frame generale, che resta il gate onesto di gen393.

Il controllo immediatamente successivo, «dove si trova Milano», ha falsificato
la generalita' del taglio: Ruanda esercitava soltanto `continent_of/2` e Parigi
aveva gia' un fatto `located_in/2`. La KB conteneva invece Milano nel dialetto
separato `capital_of_region(lombardia, milano)`, incompatibile anche con il
canone `tr(milan, milano)`. Il ratchet e' stato quindi allargato alla classe
amministrativa: `administrative_capital/2` proietta capitali regionali, statali
e nazionali nella stessa vista `located_in/2`; la variante nazionale richiede
`continent_of(Country, _)` per non accettare il verso sbagliato delle vecchie
righe bidirezionali. Milano, Firenze, Sacramento, Nairobi e una regione
inventata rendono ora osservabile il trasferimento che mancava. Eseguendo il
ratchet con `PARROT0_TOOLS=1` e' emersa anche la collisione con
`intent_cue(piact_grep, "where is")`: la forma generica mandava le domande
inglesi al filesystem. Il cue e' stato ristretto a relazioni dichiaratamente di
codice (`where is defined`, `where is declared`, ...), sempre nella KB. Il test
ora copre lo stesso profilo tool-on usato da `make chat`.

Il controllo seguente ha trovato una risposta peggiore del muro:
«dove si trova Napoli» -> Campania, ma «dove si trova la Campania» -> Napoli.
La seconda risposta non era inferenza geografica: il consumer generico provava
prima `located_in(campania, ?)` e poi, senza distinguere i ruoli,
`located_in(?, campania)`. Il nuovo contratto
`answer_frame_input_arg(Cue, Predicato, 1|2)` rende il verso uno slot del frame;
il C esegue soltanto il binding indicato e conserva il comportamento storico
quando il metadato manca. Per la superficie `where is` la KB dichiara argomento
1. `region_of_country/2` aggiunge separatamente il contenimento amministrativo,
cosi' Campania risponde Italia. Il `.p0t` abla il metadato, osserva ricomparire
l'inversione su una regione inventata e lo riasserisce: e' una prova runtime del
confine KB-first, non un controllo cucito su Napoli.

Gli stimoli immediatamente successivi hanno raffinato lo stesso contratto.
«Quali sono i colori che identificano gli scacchi» mostrava che il topic
`chess` cancellava la faccetta richiesta e cadeva sull'enumerazione predefinita
dei pezzi. La KB conserva ora la faccetta come relazione `side_color/2` e il suo
slot nel frame; un dominio inventato prova che colori e superficie crescono e
si ablano a runtime. La regola per il producer e': il default di dominio vale
soltanto quando la domanda non nomina una faccetta.

`i=0; i++; quanto vale i` mostrava invece una separazione ancora piu'
importante: identificare correttamente `code(c)` non soddisfa la query che lo
segue. `segment_role(query, Cue)` e' la singola sorgente KB del confine e
dell'atto; la meccanica separa la traccia chiusa e ne valuta il binding con
l'interprete gia' esistente. Il ratchet aggiunge e ritrae una cue nuova senza
rebuild. Nel producer universale ogni query deve dunque generare
un'obbligazione che non puo' essere chiusa da una semplice classificazione del
registro.

L'audit delle `stipulation_cue` affiancate in `grammar.p0` ha infine escluso un
difetto del parser corrente: il loader gen335 separa sul punto di livello 0 e
carica entrambi i fatti. Oltre alla prova generica del loader, un `.p0t`
interroga ora i secondi membri reali delle righe e ne abla uno. Resta da
costruire il consumer stipulation -> contesto ipotetico; non va confuso con il
caricamento sintattico che e' gia' operativo.

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

**Stato di attuazione.** Il primo kernel KB-only e' presente in
`core/dialogue-policy.p0`: `dialogue_state/2` proietta proof e gap,
`move_policy/2` resta modificabile come conoscenza e `frame_move/2` compone i due
livelli. Il ratchet `conversation/dialogue_moves.p0t` cambia la stessa domanda
da `answer` a `decline`, poi da `decline` a `clarify`, usando soltanto
assert/retract della KB. Issue, obblighi, risoluzione della precedenza e il
consumer universale prima del first-match restano aperti: questo e' l'avvio di
gen394, non la sua definizione di done.

---

### gen395 — Contesti e scope restano visibili insieme

**Fallimento tirante.** Mondo e premessa vengono decisi da percorsi distinti;
parrot0 non puo' confrontarli o spiegare perche' ha scelto uno scope.

**Task 395.1 — contesti espliciti.**

```prolog
context(world, world).
context(penguin_story, hypothesis).
context(alice_quote, quotation).
context(alice_report, reported_belief).
context_default_parent(hypothesis, world).
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

**Stato di attuazione.** Il kernel KB-only e' presente in
`core/context-scope.p0`. Le credenze effettive locali escludono soltanto cio'
che `supersedes_in/3` corregge nello stesso contesto; la vista ereditata conserva
il parent e la vista visibile unisce entrambe. `proposition_signature/4` rende
derivabili incompatibilita' e `context_conflict/2`, mentre
`scope_kind_for_act/2` e `commitment_policy/2` restano dati modificabili. Il
ratchet `conversation/context_scope.p0t` prova mondo contro premessa, entrambe
le viste nella stessa risposta, fonte e confidenza, citazione e credenza
riportata non committenti, correzione locale e ablazione dello scope senza
cancellare `holds_in/2`. Non sono ancora collegati i producer NL dei mondi
locali, delle ipotesi e dell'entailment gia' esistenti: questo e' l'avvio di
gen395, non la definizione di done.

**Verifica corrente.** `make soft-test` chiude in 7 secondi sul budget 15 con
il percorso geografico in modalita' tool-on; `make test` passa 1870 asserzioni,
zero fallimenti. Il solo cambiamento C e' la meccanica generale che applica
`answer_frame_input_arg/3`; cue, predicato, verso e fatti geografici restano KB
e il ratchet ne prova crescita e ablazione.

---

### gen396 — Registro multidimensionale e risposta da proposizioni

**Fallimenti tiranti.** `preferred_register` e `style_temperature` sono
interruttori larghi; molti template mescolano claim, struttura e stile. Inoltre
«se Milano e' in Italia allora rispondi Paolo altrimenti Piero» non era
componibile, mentre `i=0; i++; quanto vale i` mostrava che classificare un
registro non soddisfa l'obbligo del turno. I due casi chiedono la stessa cosa:
un piano sopra la memoria di lavoro universale.

**Task 396.0 — materializzare il turno senza interpretarlo nel C.** Il
segmentatore conserva la cue vincente e un adattatore generico proietta:

```prolog
turn_span($Turn, $Index, $Role, $Payload).
turn_span_cue($Turn, $Index, $Evidence).
turn_span_surface($Turn, $Index, $Surface).
```

Ruolo, evidenza e consumer restano KB. Il primo esempio guida e'
`conditional-plans.p0`; il secondo deve essere un piano misto code+query. Non e'
done finche' una nuova cue di proposizione, connettore o azione non cambia il
piano con assert/retract a runtime.

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

Il piano non e' soltanto una lista di frasi. Deve rappresentare controllo ed
effetti senza privilegiare la prosa:

```prolog
plan_condition($Plan, $Proposition).
plan_branch($Plan, true, $Action, $Payload).
plan_branch($Plan, false, $Action, $Payload).
plan_obligation($Plan, state_after, value($Symbol, $Value)).
span_transition($Span, $Before, $After).
```

Le relazioni concrete (`located_in_t`, `surface_in_language`, assegnazione,
incremento) sono membri della KB. Il motore offre soltanto unificazione,
ordering, binding ed esecuzione delle primitive generali.

**Task 396.4 — ratchet `.p0t`.** Condizionali veri/falsi, ellissi, cue concorrenti
e crescita/ablazione runtime; code+query e code+expected+constraint devono
produrre piani completi senza che la risposta sul registro rubi il turno. Poi:
stessa proof resa comune/tecnica,
concisa/dettagliata e formale/informale; forma marcata capita ma non riflessa;
domanda esplicita sul termine dichiara lo status; nessun registro cambia i claim.
Runtime growth aggiunge una dimensione/form candidate, retract la rimuove.

**Definizione di done.** Il contenuto proposizionale resta identico sotto
registri diversi; il renderer non introduce fatti; «si dice X o Y?» si chiude
attraverso menzione + status + mossa, non con un consumer privato. Prosa, codice
e input misto attraversano le stesse relazioni di span, proposizione,
obbligazione, piano e proof.

**Stato di attuazione.** Task 396.0 e il primo taglio di 396.3 sono presenti:
gli span universali vengono reificati e il piano condizionale e' interamente KB.
Il ratchet `reasoning/conditional_plan.p0t` copre 28 asserzioni, incluse ellissi,
Italia/italiano e crescita/ablazione.

**396.1-396.2 eseguiti** in `kb/core/register.p0`. Il registro non e' piu' un
interruttore largo: `register_value(Registro, Dimensione, Valore)` colloca i
registri in uno spazio di dimensioni indipendenti, `context_preference/2` e' lo
stato, e la realizzazione sceglie la forma dentro `concept_label/4` — che porta
gia' sia la lingua sia il registro. La stessa proof si rende in tre forme senza
che cambi di una virgola cio' che afferma, e il ratchet lo prova togliendo il
fatto: nessuna preferenza puo' farlo ricomparire in nessuna forma.

La regola del gen390 e' ora generale e ha una clausola sua: accettare una forma
marcata non obbliga a rispecchiarla. Se il registro chiesto esiste per quel
concetto ma la sua forma e' marcata, parrot0 non la emette e non tace — usa la
forma compatibile non marcata dello stesso concetto, senza correggere nessuno.
La guardia sta su OGNI gradino della scala di realizzazione, non solo sul primo:
la prima versione lasciava che la localizzazione ordinaria emettesse comunque
`mangiare` mentre il registro si rifiutava, cioe' la policy era vera per meta'.

Il livello e' additivo per costruzione: senza una `context_preference` dichiarata
nessuna vista si attiva e il comportamento resta quello di prima. La porta
conversazionale esiste (`register_cue/3` + `turn_cue_registry`), quindi il
registro si cambia parlando e la mossa dovuta e' riconoscerlo; una preferenza
nuova sulla stessa dimensione SOSTITUISCE la vecchia, perche' due valori sullo
stesso asse renderebbero preferiti due registri opposti e la scelta tornerebbe a
essere l'ordine dei fatti.

Restano aperti il piano proposizionale del codice, la condivisione strutturata
delle proof, le dimensioni oltre technicality/formality (verbosity, directness,
politeness, hedging) e la COMPOSIZIONE — una richiesta di stile dentro una
domanda vera oggi non ruba il turno alla domanda, ma nemmeno la modifica. E'
debito dichiarato e ratchettato, non silenzio. gen396 e' avanzata, non chiusa.

**Task 396.-1 — il prerequisito del §2.1.5, pagato.** Il primo giro di gen396 ha
consegnato un `make soft-test` rosso in modo dipendente dall'ordine dei file, che
il §2.1 vincolo 5 obbligava a chiudere PRIMA di promuovere qualunque livello
dipendente da composizione di query. Non era ne' costo ne' isolamento del reload:
`parse_to_term/2` lasciava indefinita la polarita' `neg` del goal, e i tre
chiamanti che costruiscono un goal a RUNTIME (`findall/3` e i due `call/1`)
passano uno `Term` automatico mai azzerato. Con quel byte non nullo un'enumerazione
positiva veniva risolta come `naf(G)`, trovata non-ground e declinata per
floundering: **zero soluzioni presentate come insieme vuoto legittimo**.

E' esattamente la divergenza silenziosa che il vincolo 5 descrive, e la sua forma
peggiore: non un fallimento visibile, ma una risposta sbagliata travestita da
assenza onesta — il piano condizionale collassava sul muro. Dipendeva dalla storia
del processo (un turno a muro precedente bastava a cambiare lo stack) e non dalla
KB, per questo sembrava non deterministico. La correzione e' nel punto unico che
costruisce un goal; il loader delle regole azzerava gia' la `Rule`, quindi le
clausole dei file `.p0` non erano toccate. Ratchet in
`reasoning/sequential_view.p0t`: `findall` su `apply/2` e `call/1` costruiti a
runtime, ciascuno col controllo negativo, falsificati forzando `neg = 1`.

Questo chiude anche l'anomalia gen389 «`apply/2` non si comporta dentro
`findall/3`» e rende ridiscutibile — non chiusa — l'ipotesi dei `kb_match`
consecutivi a zero.

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

**Stato di attuazione (gen397, avvio — la RIPRESA).** `kb/core/discourse.p0`.
Il gen394 aveva dato a parrot0 la memoria di cio' che non ha saputo dire, ma
restava muta: bisognava gia' sapere quale parola nominare per interrogarla. Ora
e' parrot0 a dire che cosa manca, e l'elenco si aggiorna da solo perche' e' una
vista e non una lista tenuta a mano:

```text
what is still open   -> Nothing is pending: I have answered everything you asked.
where is zorbia      -> Hmm, I don't know about zorbia yet...
where is florbix     -> Hmm, I don't know about florbix yet...
what is still open   -> I still owe you an answer about zorbia, florbix.
zorbia is in europe  -> Learned: located_in(zorbia, europe).
what is still open   -> I still owe you an answer about florbix.
what did we settle   -> Since you asked, I have learned about zorbia.
```

Il ratchet `conversation/discourse_recall.p0t` (30 asserzioni) fa passare in
mezzo DODICI turni estranei — aritmetica, saluti, geografia, ringraziamenti —
perche' la prova non e' che parrot0 ricordi l'ultimo turno: e' che ricordi
attraverso turni che non c'entrano niente. E' la differenza fra un buffer e una
memoria.

Il taglio porta anche la piega che mancava al §K6: `list_text/3` realizza un
elenco di lunghezza VARIABILE con il separatore della lingua come dato, mentre
`answer_content/4` sapeva comporre solo un numero fisso di pezzi con ruoli noti.
Come il fold del piano proposizionale, non decide nulla su cosa dire.

**Secondo taglio: cio' che e' stato DETTO.** Un'issue ricorda le domande senza
risposta; `exchange/3` ricorda le risposte date. Non e' la stessa cosa di sapere
il fatto — il fatto sta nella KB e puo' cambiare, mentre uno scambio e' accaduto
e resta accaduto. Senza questa distinzione una conversazione non puo' riferirsi
a se stessa, ed e' `unit_contains/2` nella forma piu' piccola che serva:

```text
where is zorbia                        -> Europe.
!forget located_in(zorbia, europe)
where is zorbia                        -> (non piu' Europe)
what did you tell me about zorbia      -> Europe.
what is still open                     -> ... zorbia ...
```

La superficie «what did we talk about» NON e' stata presa: ha gia' un consumer
dal gen58 che elenca le parole di contenuto dei turni recenti. Fa una cosa
diversa, piu' larga e piu' grezza, e rubargliela sarebbe amputare una struttura
secondaria per far posto a una nuova. Le due memorie convivono: quella conta
cio' che e' stato NOMINATO, questa cio' che e' stato RISPOSTO.

**Terzo taglio: salienza e topic (397.2).** L'argomento non e' un campo tenuto
da qualcuno: e' l'entita' il cui scambio piu' recente non ha nulla dopo di se'.
Essendo una vista, una digressione che non risponde a niente — un conto, un
saluto — non sposta l'argomento, e non perche' qualcuno l'abbia esclusa: perche'
non ha lasciato uno scambio. «Prima di questo» e' la stessa vista tolta la cima,
cioe' il topic stack nella sua profondita' minima e senza una pila da mantenere.

```text
where is milan          -> Lombardy.
where is paris          -> France.
what are we on          -> We are on paris.
what were we on before  -> Before that we were on milan.
what is 2 + 2           -> 4.
what are we on          -> We are on paris.
```

Il turno ha ora un NUMERO (`turn_counter/1`, un contabile come gli altri):
senza ordine la conversazione e' un insieme e non una sequenza, e «prima»,
«poi» e «ancora prima» non esistono. Lo scambio e il suo posto nella sequenza
restano due fatti, perche' un termine del dialetto porta al massimo quattro
argomenti e un `assert` puo' percio' creare un fatto di arita' tre — il quinto
argomento non e' rappresentabile nel corpo di una regola. Separarli e' anche
piu' onesto: quando accadde e che cosa fu detto sono due domande.

Restano `discourse_unit/3` con i suoi confini, `relevant_to/2` e la sintesi con
provenance. I dialoghi da 50 e 100 turni non sono ancora provati: il ratchet ne
prova quarantanove asserzioni su una conversazione che cambia argomento tre
volte.

## 16. Stato di avanzamento

**Aggiornamento del 17 agosto 2026.** Tutte e sette le generazioni hanno ora
almeno un taglio verticale funzionante, e il piano e' intorno al **70%**. La
frase che riassume la sessione: il pilastro che mancava quattro volte — il
producer NL — esiste ora UNA volta sola (`kb/core/turn-frames.p0`), e 394, 395,
396, 397 e 398a hanno smesso di reinventarlo.

La catena universale, dall'alto in basso:

```text
turno naturale
  -> universal_turn_lead    span, token, cue — soltanto meccanica, nel C
  -> turn_bookkeeping/2     gli EFFETTI del turno, come contabili KB
  -> turn_plan_candidate/1  questo turno appartiene a un piano?
  -> turn_response/2        la risposta, e nient'altro
```

Le tre domande sono separate apposta: `turn_response/2` e' **pura**, perche' una
risposta non puo' dipendere da un effetto accaduto mentre la si cercava. Gli
impegni — stipulare, descrivere uno stato, cambiare registro, aprire un'issue,
registrare uno scambio — vivono tutti nella contabilita', ed e' cio' che permette
a un turno di fare DUE cose invece di scegliere quale meta' di se stesso essere.

Per lo stato per-generazione, le trappole del dialetto pagate e l'ordine di
ripresa, l'handoff operativo e' in testa a `KB_TODO.md`.

Il piano e' partito da **gen391**. Il testo che segue e' la fotografia
precedente, conservata perche' ogni generazione ne annota l'avanzamento nel
proprio paragrafo:

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
4. **PRODUCER CHIUSO (gen393, secondo taglio):** `kb/core/turn-frames.p0`
   materializza `frame_act/2`, `frame_slot/3` e `frame_source/3` dal turno
   reale, prima del first-match dispatch, per ogni superficie ammessa. Il turno
   arriva gia' spezzato dall'unico producer universale, che ora pubblica due
   meta' complementari: `turn_span_token/4` (le parole DETTE, citate) e
   `turn_cue/3` (le superfici DICHIARATE dalla KB che il turno contiene). La
   sola meccanica nuova nel C cerca stringhe dentro byte ed e' parametrizzata da
   `turn_cue_registry/2`: nessuna cue, lingua, relazione o dominio nel motore, e
   un registro nuovo e' un fatto. Il consumo e' quello del gen394: `frame_move`
   dalla policy, e si risponde soltanto se la mossa e' `answer`. Il ratchet
   `meta/turn_frame_producer.p0t` (16 asserzioni) interroga sempre il MODULO che
   ha risposto, perche' il testo da solo sarebbe passato anche senza produttore.
   Restano aperti: l'ammissione e' oggi la superficie con VERSO dichiarato
   (vedi sotto), e il declino non e' realizzato — resta fatto interrogabile e il
   turno prosegue verso il percorso storico;
5. **primo gap epistemico presente:** `missing_fact` nasce soltanto dalla NAF
   ground su `frame_has_answer/1`; il solver gia' distingue finite failure da
   incomplete e nel secondo caso non autorizza il gap. Restano da falsificare
   deterministicamente il caso di budget e da modellare ponte, operatore e
   realizzazione.

Avvio della gen394:

1. **stato dialogico derivato:** proof e `missing_fact` diventano classi di
   evidenza comuni, non decisioni private dei consumer;
2. **policy residente nella KB:** lo stesso stato puo' cambiare mossa con
   assert/retract di `move_policy/2`, senza rebuild;
3. **ratchet runtime presente:** `conversation/dialogue_moves.p0t` prova
   risposta, declino e chiarimento sulla stessa struttura di frame;
4. **ISSUE E OBBLIGHI PRESENTI (gen394, secondo taglio):** `kb/core/issues.p0`.
   Il posto dove registrarli non esisteva — il turno che fallisce esce dal frame
   senza che nessuno gli chieda niente, perche' il frame declina apposta per
   lasciare la parola al percorso storico. `turn_bookkeeping/1` e' una terza
   domanda che il motore pone a ogni turno e il cui risultato viene IGNORATO:
   registrare non e' rispondere, e le altre due domande restano pure.

   Lo stato dell'issue e' una VISTA e non un flag: si chiude quando la risposta
   diventa derivabile e si riapre se la conoscenza viene ritratta, quindi non
   puo' divergere da cio' che parrot0 sa. `answer_obligation/2` e' derivato per
   la stessa ragione — non puo' sopravvivere alla propria ragione d'essere.

   Resta aperta la PRECEDENZA fra mosse (`move_priority/3` e' dato ma nessun
   consumer lo risolve), e la condizione dell'issue e' oggi
   `naf(relation_mentions(...))` invece di «nessuna lettura regge»: la seconda
   chiede l'enumerazione esaustiva e la contabilita' gira a ogni turno. Il caso
   Napoli/Campania — relazione che conosce la parola ma non nel verso chiesto —
   resta percio' fuori, ed e' debito dichiarato.

Avvio della gen395:

1. **contesti reificati:** mondo, ipotesi, citazione e credenza riportata sono
   istanze tipizzate con parent, fatti, fonti, confidenza e commitment;
2. **viste concorrenti:** locale, ereditata e visibile non cancellano
   alternative; firme di proposizione compatibili derivano conflitti fra scope;
3. **crescita e ablazione:** il `.p0t` cambia la scelta di scope ritraendo una
   policy e prova che i fatti degli altri contesti restano disponibili;
4. **PRODUCER CHIUSO (gen395, secondo taglio):** `kb/core/stipulation.p0`
   proietta un turno che stipula in `context/2` + `holds_in/2`. La superficie e'
   un ruolo di span aperto da `stipulation_cue/1` — la stessa conoscenza che il
   percorso storico usava per decidere se rinominare i concetti — e la lettura
   riusa le procedure del piano condizionale (forma canonica, tipi degli
   argomenti), quindi non nasce un secondo parser: cambia l'ATTO, non la
   meccanica. L'impegno e' reso persistente da `assert/1`, che e' gia' una
   primitiva del solver: la KB rende durevoli le proprie conclusioni senza che un
   modulo C decida per lei quando farlo, e l'effetto accade dentro la
   realizzazione della mossa `acknowledge` — stipulare in silenzio sarebbe un
   cambiamento di stato che l'interlocutore non puo' vedere.

   La differenza col percorso storico e' il punto della generazione: quello
   applica le premesse sotto provenienza ipotetica, risponde e le ritira a fine
   turno — giusto per «se tutti i gatti sono pesci...», dove l'ipotesi serve una
   volta sola. Qui il mondo stipulato SOPRAVVIVE al turno, quindi si puo'
   stipulare e poi chiedere. Le due strade convivono; nessuna e' stata amputata.

   La definizione di done e' raggiunta e ratchettata in
   `conversation/stipulated_world.p0t` (13 asserzioni, IT e EN):

   ```text
   dove si trova milano                  -> Lombardia.
   supponiamo che milano sia in francia  -> D'accordo: lo tengo come ipotesi, [...]
   dove si trova milano                  -> Per quello che so, lombardia;
                                            per la tua ipotesi, francia.
   ```

   Due risposte diverse, messe in relazione, ciascuna con lo scope in cui vale:
   il mondo non viene corretto, e ritrarre la credenza ipotetica non deve
   ripristinare nulla — perche' non aveva cancellato nulla;
5. **producer degli altri scope aperto:** citazione, credenza riportata e
   premessa di task non hanno ancora la loro porta; e il mondo stipulato e' uno
   solo (`stipulated_world`), quindi due ipotesi concorrenti non sono ancora
   distinguibili;
5. **ratchet di precondizione:** faccetta esplicita, query sullo stato di un
   registro misto e clausole multiple per riga sono ora protette da `.p0t` con
   crescita/ablazione. `make soft-test` resta nel budget invariato (11s/15s),
   `make test` chiude 1894 asserzioni verdi e il loader passa 15/15 prove
   dedicate.

## 17. Nuovo fronte pianificato: ragionamento situazionale causale (gen398+)

Questa sezione pianifica una capacita' futura; **non dichiara implementato nulla**.
Nasce dal turno reale del 2026-08-17:

```text
you> cosa faresti se una mongolfiera sta cadendo per rallentare la caduta
Non capisco ancora.
```

Il muro non richiede una risposta sulla mongolfiera. Rivela che il universal
input non sa ancora trasformare una descrizione aperta in uno stato del mondo,
un obiettivo, un insieme di azioni applicabili e una previsione dei loro effetti.
Lo stesso vuoto impedisce problemi piu' complessi di sopravvivenza,
triangolazione, uso non ovvio delle proprieta', revisione del contesto e scelta
sotto vincoli.

### 17.1 Obiettivo osservabile

Da un normale turno `gen.respond`, senza comando o router di dominio, parrot0
deve poter:

1. estrarre entita', ruoli, stato, risorse, capacita', pericoli, goal e vincoli;
2. conservare piu' letture o mondi quando il testo non determina quale valga;
3. recuperare dalla KB schemi d'azione le cui precondizioni si unificano con lo
   stato corrente;
4. simulare effetti, consumi, durate e rischi, componendo un piano multi-step;
5. verificare invarianti, soglie e conflitti fra azioni concorrenti;
6. distinguere conclusioni dimostrate, ipotesi, testimonianze deboli e dati
   mancanti;
7. scegliere se agire, chiedere, osservare, qualificare, rifiutare una scorciatoia
   dannosa o dichiarare onestamente che nessun piano e' provato;
8. rileggere il piano dopo una correzione, nominando che cosa rimane valido e
   quale dipendenza e' stata spezzata;
9. trasformare proof e piano nel K6 proposizionale e solo dopo realizzarli nella
   lingua e nel registro appropriati.

Il test di generalizzazione resta quello del mantra: domani una nuova azione,
affordance, legge causale, fonte o policy deve diventare usabile con
assert/retract della KB, senza ricompilare il C.

### 17.2 Sonda OpenCode-GO e lezioni, non risposte da copiare

La batteria di design e' in `tests/situational_reasoning_probe.py`; il trascritto
del giro guida e' `tests/sym/situational-reasoning-20260817-013348.md`, modello
`gpt-5.6-luna` via endpoint OpenCode-GO. Come le sonde ambiguity/repair, confronta
la **mossa** e non promuove il modello esterno a fonte di verita'.

| scenario | parrot0 osservato | mossa del riferimento utile al progetto |
|---|---|---|
| mongolfiera nuda/vincolata | muro | propone azioni condizionate e le lega a effetti |
| risorse esplicitamente assenti | muro | dichiara il limite e non finge di ripristinare la portanza |
| correzione aria calda -> gas | muro/chiarimento vuoto | conserva passi invarianti e ritira quello causalmente invalido |
| separazione cesto/involucro | risposta meta fuori tema | rilegge struttura e applicabilita' delle azioni |
| proposta di espellere una persona | muro | tratta il danno come vincolo e cerca alternative |
| tre uscite e fonte non verificata | risposta meta fuori tema | enumera due mondi, separa prova e testimonianza, cerca un bit discriminante |
| barca con flussi e riparazione | storia fuori tema | simula il picco intermedio e dichiara assunzioni |
| tre interruttori, una visita | falso positivo codice | usa un'azione per produrre due osservabili: luce e calore |

La sonda espone anche i limiti del riferimento. Nel caso nudo assume il tipo piu'
comune di pallone; nei casi di emergenza puo' nominare procedure o risorse non
fornite dal testo. Quindi:

- dal modello si estraggono firme come `ripianifica`, `dichiara_assunzione`,
  `cerca_informazione` e `rifiuta_danno`;
- fatti fisici, procedure di sicurezza e policy normative richiedono fonti
  indipendenti prima di entrare nella KB;
- un transcript non diventa mai `response_template` e non e' un golden test;
- i ratchet locali misurano rappresentazioni, proof e mosse, non similarita' di
  stringa con l'oracolo.

Le sonde future devono aggiungere controlli avversariali: premessa impossibile,
risorsa irrilevante, azione con effetto ritardato, fonte contraddittoria,
correzione che cambia solo un attributo, piano senza soluzione e caso in cui la
domanda di chiarimento e' migliore di un piano fragile.

### 17.3 Rappresentazione minima comune

Il producer universale deve materializzare quattro strati collegati da ID e
provenance:

```text
FRAME DEL TURNO
  descrizione + domanda + modalita' + lingua
        |
        v
SITUAZIONE / MONDI CANDIDATI
  entita' + ruoli + proprieta' + relazioni + risorse + fonti + assunzioni
        |
        v
PIANO CAUSALE
  goal + azioni applicabili + transizioni + costi/rischi + residui
        |
        v
PIANO DI RISPOSTA
  conclusione + motivi + condizioni + alternative + avvertenze
```

Ogni fatto situazionale deve portare almeno:

- contesto/versione del mondo;
- fonte: testo utente, KB, derivazione, ipotesi o testimonianza;
- stato epistemico: provato, assunto, contestato, mancante;
- intervallo temporale o ordine, quando rilevante;
- entita' e faccetta a cui si applica.

Non basta una lista piatta di triple. `il bruciatore e' guasto` deve poter
invalidare soltanto azioni che richiedono un bruciatore funzionante; `non e' ad
aria calda` deve aprire un mondo con un diverso schema del mezzo; `il cesto si e'
separato` deve spezzare relazioni strutturali da cui dipendeva il controllo.

### 17.4 Espansione KB-first

Prima di creare file nuovi va fatto l'audit dei predicati riusabili. La
destinazione concettuale prevista e':

| classe | proprietario previsto | contenuto |
|---|---|---|
| frame situazionale | core semantico eager | ruoli e slot universali, nessun dominio |
| stato e transizioni | `kb/core/procedures.p0` o core planning | operatori generali di applicabilita' e successione |
| policy deliberative | core dialogico/presentation | quando agire, chiedere, qualificare o declinare |
| action schema | bundle di dominio lazy | precondizioni, effetti, consumi, durate, rischi |
| causalita' del mondo | expert/domain file | fatti verificati, unita' e provenance |
| lessico | gloss/grammar/intents | forme NL insegnabili che denotano ruoli e relazioni |
| realizzazione | responses/presentation | micro-frame per esporre piano, condizioni e residui |

Esempi di regole trasferibili da perseguire, non sintassi definitiva:

```prolog
applicable($Action, $World) :-
    action_schema($Action, $Domain),
    all_preconditions_hold($Action, $World),
    resources_sufficient($Action, $World).

invalidated_step($Step, $NewWorld) :-
    plan_step($Plan, $Index, $Action),
    naf(all_preconditions_hold($Action, $NewWorld)).

needs_clarification($Situation, $Question) :-
    decision_relevant_unknown($Situation, $Variable),
    discriminating_question($Variable, $Question).

prefer_information_action($A, $B, $Situation) :-
    expected_world_reduction($A, $Situation, $More),
    expected_world_reduction($B, $Situation, $Less),
    greater_than($More, $Less).
```

Le quantita' concrete, le parole italiane e inglesi, i tipi di pallone, il
calore della lampadina e le procedure nautiche non possono apparire nelle
regole core. Sono membri di relazioni aperte. Ogni nuovo cue naturale richiede
il suo test di crescita e retrazione.

### 17.5 Meccaniche ammesse nel motore

Il C puo' fornire solo primitive invarianti e parametrizzate:

- enumerazione di azioni e mondi dalla KB;
- unificazione e verifica delle precondizioni;
- applicazione non distruttiva di delta di stato;
- aritmetica di quantita', durate, flussi e soglie;
- ricerca bounded, ordinamento, dominance pruning e rilevamento di cicli;
- hashing/indici/cache su predicati, firme ground e dipendenze;
- budget e produzione di un residuo tipizzato quando la ricerca e' incompleta.

Non puo' decidere che parole come `cadere`, `zavorra`, `buttare`, `prima`,
`pericoloso`, `libera` o `calda` attivino quelle meccaniche. Non puo' contenere
una tabella di risposte agli enigmi, una lista di oggetti di emergenza o un ramo
`if balloon`. La policy che un danno umano e' un vincolo, non un costo ordinario,
deve essere ispezionabile e sostituibile nella KB senza essere ridotta a un
numero magico del planner.

### 17.6 Politica per casi incompleti e borderline

Il planner non deve produrre sempre un'azione. La mossa dipende dalla qualita'
della proof:

| stato | mossa |
|---|---|
| piano provato, rischio entro policy | proporre passi e motivi |
| piano valido solo sotto un'assunzione | proporlo condizionatamente e nominare l'assunzione |
| due mondi portano a piani incompatibili | chiedere il dato discriminante o offrire rami espliciti |
| azione informativa sicura riduce i mondi | preferire osservazione/test prima dell'azione irreversibile |
| nessuna leva disponibile | dichiarare il limite e passare a mitigazione/preparazione |
| conoscenza causale assente | non inventare; nominare il gap e chiedere/ricercare se autorizzato |
| scorciatoia viola una policy forte | rifiutarla e cercare piani ammissibili |
| budget esaurito | risposta incompleta tipizzata, mai certezza simulata |

La domanda di chiarimento e' utile soltanto se la risposta puo' cambiare la
decisione. Chiedere genericamente «cosa vuoi sapere?» dopo una correzione e' un
muro cortese, non ragionamento.

### 17.7 Ordine esecutivo gen398+

**gen398a — Situation IR dal universal input.** Un solo producer trasforma
descrizioni e domande in entita', stato, goal, risorse, vincoli e fonti. Primo
ratchet: stesso frame per parafrasi IT/EN; cue nuovo assert/retract; nessun
consumer speciale.

**Stato di attuazione (gen398a, primo taglio — META' DEL CONTRATTO).** Il
livello esiste in `kb/core/situation.p0` ed e' interamente KB: **zero righe di
C**. Non nasce un router: il turno arriva dall'unico producer universale
(`turn_span/4`, `turn_span_token/4`) e la risposta esce dall'unico contratto
`turn_plan_candidate/1` + `turn_response/2`, lo stesso di `conditional-plans.p0`
e `code-plans.p0` — se questo livello avesse preteso un percorso suo, il piano
universale non sarebbe universale.

Cosa e' collegato:

1. **il goal e' letto dal turno** — il ruolo di span nasce da `segment_role/2`,
   il verso del cambiamento e la proprieta' sono concetti raggiunti da
   `linguistic_form/4`, cioe' dalla sorgente unica `concept_label/4` di gen391.
   «how do i lower the level» e «come faccio ad abbassare il livello» producono
   lo stesso Situation IR e la stessa azione provata; cambia la realizzazione,
   non il contenuto proposizionale;
2. **lo stato e' una credenza di contesto**, non una seconda tabella:
   `situation_state/4` e' una vista di `context_visible_belief/2` (gen395), e le
   proposizioni sono reificate (`state_prop/4`) come `proposition_signature/4`
   gia' presupponeva. La ripianificazione di gen398e eredita quindi il
   versionamento invece di reinventarlo;
3. **applicabilita' come insieme**: si legge l'INSIEME delle precondizioni non
   provate, non la prima. `applicable/2` e `blocked_action/2` sono relazioni
   interrogabili dalla porta binaria universale, con verso dichiarato;
4. **effetto e conseguenza restano distinti**: ritrarre `causal_law/2` toglie il
   piano e lascia l'azione descritta e applicabile. Due relazioni, due ablazioni;
5. **trasferimento provato**: gli stessi operatori pianificano in un secondo
   dominio che non condivide una parola col primo. Mondo, azioni, leggi causali
   e forme entrano tutti da assert runtime — nel core non vive un solo fatto di
   dominio.

Cosa NON e' collegato, e non va dichiarato chiuso:

1. **la meta' `descrizione` del producer manca.** Il turno contribuisce oggi il
   GOAL; lo STATO deve essere gia' conoscenza. Trasformare una descrizione in
   prosa («la valvola e' chiusa e il livello sale») in `state_prop/4` +
   `holds_in/2` e' il gate onesto che chiude gen398a;
2. niente piani multi-passo, stati intermedi, risorse quantitative, mondi
   concorrenti, azioni informative, ripianificazione o policy di rischio: sono
   398c-398f e restano contratto;
3. lo stimolo guida della mongolfiera **mura ancora**, come deve: non esiste un
   dominio pallone e non c'e' motivo di fabbricarne uno per far passare il
   prompt che ha aperto il fronte;
4. due azioni applicabili producono per ora un template di ambiguita' invece di
   enumerare le alternative: e' un debito di realizzazione K6, non di inferenza.

**Il difetto trovato dal ratchet, e la sua forma.** Il primo taglio rispondeva
che un'azione era BLOCCATA in una situazione mai dichiarata. In un mondo
inesistente nessuna credenza vale, quindi ogni precondizione vi risulta
«mancante»: l'assenza di conoscenza si travestiva da conoscenza negativa — la
stessa specie di divergenza silenziosa del §2.1 vincolo 5, e la sua forma
peggiore. La correzione e' il gate `context($Situation, $Kind)` dentro
`applicable/2` e `blocked_action/2`, e il controllo negativo su una situazione
mai dichiarata e' ora nel ratchet.

**Verifica.** `tests/p0t/reasoning/situation_plan.p0t` chiude 25 asserzioni sul
budget ordinario di un secondo per turno, profilo AGI, senza `!timeout`.
`make test` passa 2052 prove, zero fallimenti. `make soft-test` resta
semanticamente verde e fuori budget per il carico ambientale gia' registrato
nell'handoff (20-22s contro 15s, misurato a 21s anche disattivando questo
livello): la guardia `turn_goal_span/1` prima della findall riporta il costo per
turno dentro il rumore, e il budget non e' stato alzato.

**gen398b — Schemi d'azione e applicabilita'.** Introdurre precondizioni, effetti,
consumi e azioni inapplicabili. Tre domini minimi: controllo della discesa,
contenimento di un flusso, uscita da un ambiente. Ablare una precondizione deve
ritirare il passo senza cambiare il motore.

**gen398c — Transizioni temporali e invarianti.** Comporre due o piu' passi,
calcolare stati intermedi e verificare soglie. Il caso barca e' membro guida;
stress con almeno dieci passi/azioni irrilevanti e stesso budget ordinario.

**Stato di attuazione (primo taglio: il PASSO ABILITANTE).** Un pianificatore
che sa fare un passo solo non fa piani: cerca risposte. Ora l'azione che
raggiunge il goal puo' non essere applicabile adesso e diventarlo dopo un'altra
che invece lo e':

```text
la valvola e' chiusa                 -> Annotato.
la valvola e' bloccata               -> Annotato.
come faccio ad abbassare il livello  -> Prima sbloccare la valvola,
                                        poi aprire la valvola.
```

La chiave e' `holds_after/3`: applicare un'azione non modifica la situazione, la
si GUARDA come sarebbe. E' la condizione perche' un piano si possa valutare
senza eseguirlo, e perche' valutarne uno non renda impossibile valutarne un
altro — un motore che dovesse asserire per provare non potrebbe mai confrontare
due alternative. Il delta e' minimo e dichiarato: vale l'effetto, e vale tutto
cio' che l'effetto non contraddice sullo STESSO ASSE (stessa entita', stessa
proprieta', valore diverso). Aprire una valvola non dice niente sul livello.

Il ratchet `reasoning/multi_step_plan.p0t` (14 asserzioni) prova anche il
negativo che conta: tolta l'azione che abilita, non compare un piano piu' corto
inventato — resta il residuo onesto. Un pianificatore che allunga il passo per
non tacere e' peggio di uno che tace.

**Secondo taglio: la sequenza.** `plan_state/3` non ha piu' profondita': regge
una sequenza di qualunque lunghezza. La lista e' in ordine INVERSO — l'ultima
azione in testa — perche' cosi' la ricorsione dice esattamente cio' che si vuole
dire: vale cio' che l'ultima azione produce, e vale cio' che sopravviveva prima e
l'ultima non contraddice. `plan_ok/2` verifica ogni passo NELLO STATO IN CUI
VIENE ESEGUITO, che e' la differenza fra un elenco di azioni possibili e un piano.

Ad avere un limite e' la GENERAZIONE delle catene, ed e' un limite dichiarato e
non un caso: cercare senza confini in uno spazio combinatorio non e' piu'
ragionare, e il §17.5 autorizza una ricerca bounded proprio per questo.
Allungarlo e' una riga; toglierlo no.

```text
la valvola e' bloccata, il pannello e' serrato
come faccio ad abbassare il livello -> Prima aprire il pannello, poi sbloccare
                                       la valvola, poi aprire la valvola.
il pannello e' dischiuso
come faccio ad abbassare il livello -> Prima sbloccare la valvola,
                                       poi aprire la valvola.
la valvola e' sbloccata
come faccio ad abbassare il livello -> Puoi aprire la valvola.
```

Il piano si ACCORCIA da solo man mano che il mondo migliora, e non perche'
qualcuno lo riscriva: e' la stessa derivazione su uno stato diverso. Un
pianificatore che tenesse il piano da parte continuerebbe a dirlo di tre passi.

Restano aperti gli stati intermedi quantitativi, le soglie e il bilancio
temporale del caso barca — cioe' il resto di 398c.

**gen398d — Mondi concorrenti e azioni informative.** Conservare alternative,
calcolare quale osservazione le distingue e separare proof da fonte debole. I
tre interruttori non passano se la soluzione e' una risposta terminale: una
nuova proprieta' osservabile insegnata a runtime deve generare una strategia
analoga in un dominio diverso.

**Stato di attuazione (primo taglio: l'osservazione che separa).** Fino a qui un
piano serviva a CAMBIARE il mondo; `kb/core/inquiry.p0` serve a saperne di piu'.
Quando lo stato non e' determinato, la mossa giusta non e' scegliere a caso fra
le alternative — e' fare la cosa che le distingue.

```text
(due mondi possibili, nessuna osservazione dichiarata)
come faccio a sapere quale   -> (muro: non si inventa un modo di guardare)
!assert observes(prova_sinistra, st(leva, feeds, sinistra))
come faccio a sapere quale   -> Puoi scoprirlo cosi': tirare la leva sinistra
                                e guardare.
```

La definizione e' la piu' piccola che sia ancora corretta, e non nomina nessun
dominio: un'azione informa quando la proposizione che rivela SEPARA i mondi —
vera in uno, non vera in un altro. Osservare qualcosa che vale in tutti i mondi
candidati non riduce niente, e il ratchet lo prova: un'osservazione vera in
entrambi non viene proposta. Se bastasse «e' un'osservazione» per suggerirla, il
livello sarebbe un generatore di fatica inutile.

I mondi candidati sono CONTESTI — `holds_in/2` del gen395 li regge gia', ed e' il
terzo consumatore della stessa reificazione dopo stipulazioni e situazioni.

Il criterio di non-barabilita' del piano e' soddisfatto: l'osservazione entra e
esce a runtime, e togliendola non resta una risposta memorizzata a fare da fondo.
Il trasferimento e' su un dominio in inglese senza una parola in comune col
primo. Restano aperte la separazione fra prova e fonte debole, e la preferenza
esplicita per l'azione informativa PRIMA di quella irreversibile.

**gen398e — Correzione e ripianificazione.** Versionare il mondo, tracciare le
dipendenze dei passi e spiegare valido/invalido dopo una rettifica. Le correzioni
tipo di pallone e separazione strutturale sono due classi diverse e devono
restare tali.

**Stato di attuazione (primo taglio: l'impegno che si rompe).** Un piano dato e'
un impegno preso: chi lo ha ricevuto ci conta. Quando il mondo cambia,
ricalcolarlo in silenzio non e' onesto — la differenza fra un pianificatore e un
interlocutore e' che il secondo dice CHE COSA e' cambiato.

```text
la valvola e' bloccata               -> Annotato.
come faccio ad abbassare il livello  -> Prima sbloccare la valvola,
                                        poi aprire la valvola.
la valvola e' guasta                 -> Annotato — cosi' pero' il piano cade:
                                        non posso piu' raggiungere: aprire la valvola.
```

Due distinzioni portano tutto il peso, e senza nessuna delle due il livello
diventerebbe un allarme continuo:

1. **rotto non e' «non piu' applicabile».** Quella e' la condizione ordinaria di
   ogni piano a piu' passi — bloccare la valvola rende il passo inapplicabile e
   il piano si allunga da solo, senza annunci. Rotto significa non piu'
   RAGGIUNGIBILE, ne' adesso ne' attraverso un passo che lo abiliti;
2. **compiuto non e' rotto.** Aprire la valvola rende `apri_valvola` non piu'
   applicabile — la sua precondizione era che fosse chiusa — ma non ha spezzato
   niente: ha fatto esattamente cio' che il piano diceva. Si guarda percio'
   l'EFFETTO, e se vale gia' non c'e' nulla da annunciare.

Si ricorda l'azione che raggiunge il goal, non la catena: la catena e' derivata
e si ridurra' da sola, mentre cio' che vale la pena confrontare e' se il traguardo
sia ancora raggiungibile.

Restano aperti il tracciamento per-passo delle dipendenze — «questo resta valido,
quest'altro no» — e la distinzione fra le classi di correzione (attributo contro
struttura) che il piano chiede di non confondere.

**gen398f — Policy di rischio e risposta calibrata.** Fare derivare dalla KB
ammissibilita', reversibilita', danno, richiesta di chiarimento e declino.
Provare crescita/retrazione della policy su scenari sintetici non pericolosi;
non usare il test etico come scusa per hardcodare la frase.

**Stato di attuazione (primo taglio: l'esclusione).** `kb/core/risk.p0`. Un
pianificatore che tratta il danno come un numero alto prima o poi lo paga: basta
che il guadagno sia piu' alto. Qui l'inammissibilita' non e' una penalita', e'
un'ESCLUSIONE — e resta conoscenza, quindi la si puo' leggere, discutere e
cambiare. Una policy che nessuno puo' ispezionare non e' un principio, e' un
comportamento.

```text
come faccio ad abbassare il livello  -> Puoi aprire lo scarico rapido.
!assert action_risk(scarico_rapido, human_harm)
come faccio ad abbassare il livello  -> Non lo faccio: il danno a una persona
                                        e' un vincolo, non un costo da soppesare.
(esiste un'alternativa ammissibile)  -> Puoi aprire lo scarico lento.
```

Tre proprieta' portano il livello, e il ratchet le prova tutte:

1. **fisico e normativo restano separati.** Un'azione vietata resta
   perfettamente applicabile — le precondizioni valgono, gli effetti sono quelli
   — e semplicemente non viene scelta. Il test lo verifica chiedendo l'azione
   applicabile: c'e' ancora. Confonderli farebbe dire «non posso» dove la verita'
   e' «non lo faccio», e sono due affermazioni diverse su di se';
2. **l'alternativa ammissibile viene presa e basta**, senza prediche: rifiutare
   quando c'e' una strada buona sarebbe raccontare un dilemma che non esiste;
3. **la regola e' KB.** Ritratta la policy, l'azione torna utilizzabile;
   rimessa, torna vietata. Una classe di rischio NUOVA insegnata a runtime si
   comporta allo stesso modo: il livello non conosce `human_harm` piu' delle
   altre, e la frase del rifiuto nomina la classe invece di essere una formula.

Restano aperte la reversibilita' come dimensione a se', la richiesta di
chiarimento quando il rischio dipende da un dato mancante, e la gradazione fra
vietato e da-confermare.

**gen398g — Realizzazione K6 e confronto frontier.** Esporre obiettivo, piano,
motivo, assunzioni, alternative e residui nella lingua del turno. Rieseguire la
sonda OpenCode solo per scoprire nuove mosse; promuovere i casi nel `.p0t` con
oracle strutturali locali.

**Stato di attuazione (primo taglio: le ASSUNZIONI).** Prima della resa serviva
una distinzione che mancava. La NAF confonde «so che quella proprieta' ha un
altro valore» e «di quella proprieta' non so niente»: per l'APPLICABILITA' e' lo
stesso — in entrambi i casi il passo non e' provato — ma per cio' che si DICE non
lo e' affatto. Nel primo caso il piano e' impossibile, nel secondo e' possibile
sotto un'assunzione, e chiamarli con lo stesso nome e' la differenza fra un
rifiuto e una domanda.

```text
(nulla si sa della condizione)  -> Puoi aprire la valvola, se la valvola funziona.
la valvola funziona             -> Puoi aprire la valvola.
la valvola e' guasta            -> Conosco un'azione adatta, ma in quella
                                   situazione una sua precondizione non vale.
```

E' la riga del §17.6 «piano valido solo sotto un'assunzione -> proporlo
condizionatamente e nominare l'assunzione», con l'accento su NOMINARE: senza un
modo di dire a parole cio' che si assume il piano condizionale non viene
proposto, perche' chiedere fiducia al buio e' peggio del residuo. Il difetto che
lo ha reso evidente vale la pena ricordarlo: un'assunzione non nominabile
sopprimeva il residuo senza produrre una proposta, e quando la frase partiva
usciva troncata — il fold si ferma sul pezzo che manca.

Restano aperti l'esposizione dei MOTIVI (perche' quel passo e non un altro),
delle alternative scartate e dei residui, e il confronto frontier vero e proprio.

Ogni sottogenerazione e' un taglio verticale: producer universale, inferenza,
piano proposizionale, resa minima, test IT/EN, crescita/retrazione e negativo
vicino. Nessuna sottogenerazione puo' limitarsi ad aggiungere schema morto.

### 17.8 Matrice TDD e controlli di generalizzazione

La batteria locale minima deve incrociare fenomeni e domini:

| fenomeno | guida | transfer 1 | transfer 2 | negativo |
|---|---|---|---|---|
| azione causale | pallone/zavorra | porta/chiave | circuito/interruttore | precondizione assente |
| bilancio temporale | barca/falla | batteria/consumo | serbatoio/flusso | soglia gia' superata |
| triangolazione | uscite | sensori | diagnosi guasto | fonte sola non verificata |
| azione informativa | lampadina/calore | contenitore/peso | rete/latenza | osservazione non discriminante |
| correzione | tipo pallone | unita' di misura | agente/ruolo | dettaglio irrilevante |
| policy | danno umano | azione irreversibile | rischio alto | alternativa sicura presente |

Per ogni operatore nuovo servono:

1. positivo nel membro guida;
2. almeno due domini held-out senza lessico condiviso;
3. negativo vicino che non deve attivarlo;
4. assert a runtime di un nuovo membro che abilita la condotta;
5. retract/ablazione che la rimuove;
6. prova che il C e i file core non contengono il lessico dei prompt;
7. stress 10x con azioni, fatti o mondi irrilevanti;
8. stessa inferenza entro il secondo sul profilo AGI.

Gli oracle `.p0t` controllano almeno: frame, lista delle precondizioni, piano
selezionato, picco/soglia, assunzioni, passi invalidati, fonte della conclusione,
mossa dialogica e assenza di risorse inventate. Il wording resta subordinato.

### 17.9 Performance: indici e cache senza cambiare la semantica

Il planner amplifica il rischio combinatorio. Prima di alzare qualsiasi budget:

- indice degli action schema per predicato di goal, tipo di entita' e firma delle
  precondizioni;
- hash ground per stato e risorse, riusando il percorso esatto del solver;
- memoizzazione per `(world_version, goal, policy_version)`, invalidata da
  assert/retract e correzioni pertinenti;
- cache delle transizioni pure per `(state_signature, action_schema)`;
- dominance pruning solo quando la KB prova che un piano non e' migliore su
  nessuna dimensione rilevante;
- lazy residency dei payload di dominio, lasciando eager catalogo e porte.

La cache non puo' scegliere una lettura, nascondere provenance o far sopravvivere
un piano a una retrazione. Ogni ottimizzazione richiede equivalenza con cache
off, invalidazione testata e curva di latenza al crescere di fatti irrilevanti.

### 17.10 Gate di riuscita e non-obiettivi

Il fronte e' riuscito quando:

1. lo stimolo guida non mura e produce un piano causalmente supportato o una
   richiesta discriminante, senza modulo mongolfiera;
2. gli stessi operatori risolvono almeno tre domini non correlati;
3. risorse e azioni nuove diventano utilizzabili e retraibili a runtime;
4. una correzione ritira precisamente i passi dipendenti e conserva gli altri;
5. il sistema distingue prova, testimonianza, assunzione e ignoranza;
6. i casi senza soluzione e quelli dannosi non ricevono piani inventati;
7. ogni proposizione della risposta risale a testo, KB o derivazione;
8. la latenza resta nel budget ordinario con il profilo AGI e sotto stress;
9. la sonda frontier migliora per firme di mossa senza diventare dipendenza di
   runtime o oracle di verita'.

Non sono obiettivi: coprire tutti gli enigmi noti, fingere competenza operativa
in emergenze reali, copiare procedure da un LLM, usare ricerca web a runtime per
saltare la KB, o produrre prosa lunga senza un piano ispezionabile. Il risultato
desiderato non e' che parrot0 «sembri furbo»: e' che possa mostrare quale stato
ha costruito, quale informazione gli manca, perche' un'azione e' applicabile e
come una nuova evidenza cambia la decisione.
