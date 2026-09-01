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

> **Aggiornamento 2026-09-01 — ipotesi di metacomprensione differenziale.** La
> comprensione non è binaria. Un turno può avere relazione, entità, lingua e
> goal corretti e tuttavia non poter concludere perché manca un solo oggetto.
> Questo residuo è ora reificato come
> `information_need(Turno, Specie, Mancante, Goal)`: valore del mondo e
> antecedente discorsivo sono due membri della stessa classe, con rimedi tipati.
> La risposta prioritaria non è un elenco C: policy, superfici, lingua e resa
> restano KB e sono ritrattabili. **Ipotesi frontier:** uno strato di
> supercomprensione emerge quando il sistema sa proiettare ogni lettura nello
> spazio logico dei goal, calcolare la differenza minima che ne impedisce la
> soddisfazione, nominare quella differenza e scegliere una trasformazione che
> la chiuda. Il replay verifica la trasformazione; l'ablazione ne misura la
> causalità. Non è introspezione verbale: è un oggetto di prova.

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

## ⛔ TODO APERTI DEL PIANO — non chiuderlo senza questi

Il piano `docs/plans/frontier-kb-natural-dialogue.md` ha copertura FUNZIONALE
completa: tutte e sette le generazioni del §15 e tutte e sette le
sotto-generazioni del §17 hanno tagli verticali ratchettati. Quello che manca non
e' piu' capacita' mancante, ed e' scritto qui perche' non diventi invisibile.

> ## 0. ✅ SC40-A/B CHIUSE; ⛔ SC41: DIRE TUTTO CIO' DA CUI DIPENDE UNA LETTURA
>
> **Aggiunto il 2026-08-30 su richiesta esplicita di F., e messo per primo
> perche' e' la piu' importante delle voci di questa lista.**
>
> **Stato al checkpoint SC40-B.** Il difetto descritto sotto e' chiuso per le
> claim documentali e per la prima famiglia di dipendenza: la lettura e'
> versionata; add/retract/reteach producono un delta semantico; un indice KB
> trova sia letture riuscite sia gap candidati. Su 100 claim una lezione ne ha
> visitata e cambiata una sola; un verbo non presente produce `0/0` e un turno
> `knowledge` senza delta non produce alcun pass. `StaleLeak=0`,
> `Transfer@3=3/3`, fan-out controllato `1/100`. Report e ipotesi in
> §§18.38–18.43.
>
> Il nuovo confine e' piu' preciso: **una lettura dice oggi soltanto una parte
> di cio' che l'ha resa possibile**. `relation_verb(P)` e' esatta, ma uno schema
> curato resta `frame_predicate(P)`; morfologia, marker, modalita', ellissi,
> ordine dei ruoli e policy di copertura non sono tutti archi di dipendenza.
> L'indice puo' essere perfetto rispetto a un grafo incompleto e perdere
> comunque revisioni semanticamente necessarie.
>
> **SC41 e' quindi prioritaria:** materializzare l'identita' completa delle
> coordinate consultate da una lettura e provarle con ablation una alla volta.
> Non aggiungere controlli C per `passive`, `modal` o parole specifiche: ogni
> nuova famiglia deve comparire nella vista aperta
> `revision_dependency_member/2`, mentre il motore continua a confrontare
> termini opachi.
>
> ```prolog
> last_revision_pass(relation_verb(solvates), scope(selective),
>                    outcome(visited(1), changed(1))).
> ```
>
> Il receipt sopra chiude SC40-B, ma non dimostra `DependencyCompleteness`.
> Lo stress da 100 claim e' permanente in
> `tests/p0t/language/document_revision_scale.p0t`; il report causale e'
> `docs/labs/apprendimento-assistito/2026-08-30-supercomprensione-sc40b.md`.
>
> **Regola di metodo che resta vincolante:** quando un test ha
> bisogno di un dato fresco per passare, chiedersi **prima** se il sistema abbia
> bisogno di dimenticare o di **rivedere**. Aggirare un limite e descriverlo
> sono compatibili; aggirarlo e chiamarlo intenzionale no.

> ## ✅ TRAGUARDO — LO SPAZIO DEL DISCORSO (2026-08-31)
>
> **Da conservare: è la prima volta che parrot0 ricorda *che cosa* è stato
> nominato e *in che ordine*, e che un'espressione può riferirsi a quella
> memoria invece che a una parola.**
>
> ```text
> > Il libro rosso è sul tavolo.      ->  located_in(book_red, tavolo)
> > Il quaderno blu è sulla mensola.  ->  located_in(quaderno_blue, mensola)
> > Dov'è il primo?                   ->  tavolo        (prima: muro)
> > Dov'è il secondo?                 ->  mensola       (prima: muro)
> ```
>
> ### Che cos'è, esattamente
>
> Poco, di proposito: `discourse_referent(Ordine, Chiave)`. Una cosa nominata e
> la sua posizione nel discorso. Testa e proprietà si ricavano dalla chiave (G2);
> determinante, span e superficie originale non ci sono ancora (G5).
>
> Ma è la prima **memoria del dialogo che non è una lista di frasi**: è una lista
> di *cose*. Fino a ieri parrot0 conservava turni; ora conserva referenti.
>
> ### A che cosa serve, e che cosa abilita
>
> Non è una funzione in più: è il posto a cui si attaccano cose che prima non
> avevano appiglio.
>
> | abilita | perché prima non si poteva |
> |---|---|
> | **coreferenza** — «quello», «l'altro», «quello di prima» | non esisteva l'oggetto a cui riferirsi: F03 era la famiglia peggiore del corpus, 24 muri su 30 |
> | **ellissi** — «E il secondo?» senza ripetere il verbo | il turno ellittico non ha entità da nominare: deve prenderla dal discorso |
> | **correzione** — «no, quello rosso l'ho spostato» | correggere richiede di individuare *che cosa* si corregge, non solo che si corregge |
> | **ambiguità dicibile** — «Quale? …» invece di un muro | serve saper elencare i candidati, cioè averli |
> | **il soggetto eliso** (SC5) e **l'apposizione** | entrambi recuperano un ruolo da qualcosa già introdotto |
> | **la domanda di seguito** — «e dove si trova adesso?» | «adesso» presuppone una cosa di cui si stava parlando |
>
> Ed è la precondizione dichiarata di **GD4** (riferimento cross-turn) e di
> **D37/G4-G5**: il referente con proprietà, e il referente che sa ridirsi.
>
> ### Come è stato costruito — le tre cose che hanno deciso l'esito
>
> Vale più del risultato, perché sono riusabili:
>
> 1. **Il punto di strozzatura condiviso.** Le vie che imparano un fatto sono
>    più d'una — lo schema dichiarato, la copula binaria, il locativo — e la
>    prima versione agganciava i referenti a *una*. Misurato: il locativo
>    italiano non registrava niente, e metà del dialogo restava senza memoria.
>    L'osservazione sta ora in `p0_learn_source`, che **tutte** attraversano
>    perché registrare la provenienza è ciò che ogni via fa comunque. *Un
>    referente è esattamente questo: una cosa nominata, e quando.* Cercare il
>    punto che tutti attraversano invece di enumerare i chiamanti è la stessa
>    mossa della fase pura di SC2-B.
> 2. **Quale posizione introduca un referente è una politica, non una scelta del
>    C.** Registrando *ogni* argomento, «il secondo» diventava il **tavolo**
>    invece del quaderno: in «Il libro rosso è sul tavolo» sono nominati due
>    oggetti, ma quello di cui si parla è il primo. `referent_arg_position/1` lo
>    dichiara, e una relazione con un'altra geometria costa una riga.
> 3. **La superficie da dichiarare è quella che sopravvive al percorso.** «primo»
>    arriva al matcher come «prime» — la canonicalizzazione lo traduce — e la
>    forma col determinante non combacia più. È la **terza volta** che questa
>    lezione si presenta (dopo le cue di SC2-B e le locuzioni di SC2-D): finché
>    la canonicalizzazione non conserva anche l'originale (G5), una classe di
>    superfici deve tenere *entrambe* le forme.
>
> ### Il limite onesto
>
> È la **prima forma**, non la forma finale. Non c'è ancora il determinante (che
> distingue introdurre da riprendere), non c'è lo span, non c'è la superficie
> originale — quindi parrot0 sa che il libro rosso è stato nominato per primo e
> non sa ancora ridirlo «il libro rosso». E due referenti con la stessa testa si
> distinguono per proprietà (G2) ma non hanno ancora identità propria.


> ## ⛔⛔ IL CASSETTO SENZA MANIGLIA — il problema che teneva ferma la comprensione universale
>
> **Scoperto e misurato il 2026-08-31. Scritto in tutti i piani perché è la
> giunzione da cui dipendono `universal-input`, `universal-comprehension` e il
> fronte SC/GD insieme.**
>
> ### Il problema, in cinque righe di transcript
>
> ```text
> > Il libro rosso è sul tavolo.   ->  Learned: located_in(book_red, tavolo).
> > dove si trova il libro rosso   ->  muro
> > dove si trova book_red         ->  Tavolo.      ← solo col nome INTERNO
> > Il gatto è sul tetto.
> > dove si trova il gatto         ->  Tetto.       ← una parola sola: funziona
> ```
>
> Il **lettore** lega un *sintagma*: unisce i token fino al confine di sintagma e
> produce una chiave sola (`book_red`). La **domanda** provava un token alla
> volta — `located_in(il,?)`, `located_in(libro,?)`, `located_in(rosso,?)` — e
> non provava **mai** la frase intera. Il fatto c'era e non era raggiungibile.
>
> > **parrot0 imparava sotto un nome che non sapeva più pronunciare**, e ogni
> > entità di più di una parola finiva in un cassetto senza maniglia.
>
> ### Perché era il collo di tutto
>
> Due giri di insegnamento massiccio — 276 forme reali entrate parlando,
> verificate in processo nuovo — avevano mosso **+11 turni su 360**. Non perché
> il metodo fosse debole: perché un turno riesce solo se tengono **insieme**
> superficie, forma della domanda, nome dell'entità, riferimento, fatto e
> realizzazione, e il nome dell'entità cedeva sempre. I referenti multi-parola
> («il libro rosso», «il quaderno blu», «il treno notturno») sono la norma del
> parlato, non un caso limite — ed è anche il motivo per cui la coreferenza era
> la famiglia peggiore del corpus: non aveva **niente a cui attaccarsi**.
>
> `book_red` non è un nome scomodo: è un nome che ha **perso informazione**.
> Testa fusa col modificatore, determinante buttato, ordine invertito, lingua
> cambiata a metà — e ogni perdita chiude una porta diversa (chiedere «quale
> libro?», distinguere «un libro» da «il libro», risolvere «il primo»,
> ripronunciarlo come è stato detto).
>
> ### Il piano di soluzione — la giunzione in cinque gradini
>
> L'invariante che li governa tutti:
>
> > **ciò che si impara da una frase dev'essere interrogabile con la stessa
> > frase, e ridicibile come è stato detto.**
>
> | # | gradino | stato |
> |---|---|---|
> | **G1** | **La domanda prova i sintagmi che il lettore ha costruito.** Non un secondo indice: le *stesse tre cose* del lettore — confine di sintagma (`np_closer/1`, conoscenza), caduta del determinante, la stessa `p0_join`. Additivo: i passaggi per token restano. | ✅ **FATTO** 2026-08-31 |
> | **G2** | **Testa e proprietà.** «il libro rosso» → testa `libro` + proprietà `rosso`, così «il libro» combacia e «di che colore è il libro» risponde. Dov'è la testa è **conoscenza** (`noun_phrase_head_position(Language, first \| last)`), non una regola nel C. | aperto |
> | **G3** | **Il referente.** Un'entità introdotta occupa una **posizione nel discorso**: parrot0 ricorda che cosa è stato nominato e in che ordine, e «il primo»/«il secondo» ci si attaccano. Quale posizione di un fatto introduca un referente è una **politica** (`referent_arg_position/1`), non una scelta del C. | ✅ **FATTO** 2026-08-31 (prima forma: ordine + chiave; determinante e span restano G5) |
> | **G4** | **La coreferenza si attacca al referente.** «il primo», «quello», «l'altro» diventano `referent_same/3` — una **relazione**, non una fusione. | aperto |
> | **G5** | **Il referente sa ridirsi.** `referent_surface/3`: rispondere «il tavolo» come è stato detto, non `tavolo`. | aperto |
>
> ### Come si misura che funziona
>
> **Non con una percentuale sul totale: con una famiglia che chiude.** Il gate è
> che il dialogo `gd1_011` — cinque turni, due oggetti, un riferimento — passi
> **da capo a fondo**. Una catena che regge vale più di dieci punti sparsi,
> perché dieci punti sparsi non provano che nessuna catena regga.
>
> ### La forma ricorrente, che è la lezione vera
>
> È la **terza volta** che compare lo stesso difetto sotto un vestito diverso:
> D33 (un'interpretazione congelata perché la KB non può richiamare la lettura),
> D35 (una chiave costruita da un percorso e non dall'altro), D37 (una struttura
> costruita da un percorso e ignorata dall'altro).
>
> > **Due percorsi che devono accordarsi, e non condividono l'oggetto su cui
> > accordarsi.**
>
> Prima di aggiungere una capacità, la domanda da farsi è: *chi altro deve
> accordarsi con questa, e su che cosa?*
>
> E la stessa mossa, un piano più su, è **l'unificazione fra zone della KB**
> (D38, §18.43): far parlare aritmetica e sociale, prosa e geografia, non è
> ingegneria di dettaglio — è la condizione perché emergano abilità che nessuno
> ha progettato. Differenziarsi non basta: le parti differenziate devono potersi
> parlare.
>
> Dettaglio completo: `docs/plans/frontier-kb-natural-dialogue.md` §18.40 (D35),
> §18.42 (D37) · referto
> `docs/labs/apprendimento-assistito/2026-08-31-perche-non-cresceva.md` · coda
> `LEARN_TODO.md` GD9.


> ## 0b. ⛔ E LE DUE FAMIGLIE DI DIFETTO CHE LO ACCOMPAGNANO
>
> Sono emerse dalla stessa serie e vanno migliorate insieme, perche' hanno la
> stessa radice — il motore decide qualcosa che nessuno puo' vedere:
>
> - **D29, §18.33 — l'affermazione che copre meno di quanto legge.** Tre
>   occorrenze misurate in due giorni (modale inghiottito nel soggetto, premessa
>   sparita dall'argomento, complemento pendente perso). Nessuna somigliava a un
>   errore: sono risposte **ben formate costruite su meno di quanto il sistema
>   aveva davanti**. Il mantra #7 copre la risposta falsa; questa e' la classe
>   accanto, e sopravvive a ogni controllo che chieda soltanto se sia vera.
>   Voce **SC35**.
> - **D30, §18.34 — un default implicito e' una decisione che nessuno puo'
>   vedere.** Cinque chiusi il 2026-08-30 con la stessa ricetta (`cue_scope`,
>   `move_policy`, `support_mode`, `specific_participle`,
>   `normalization_extent_policy`): in tutti e cinque il motore stava gia'
>   decidendo, non lo diceva, e la decisione non era ne' insegnabile ne'
>   ritrattabile. Voce **SC36**, che chiede il censimento.

1. **§9 — il confronto empirico non e' stato fatto.** Le batterie del §9.2
   (parafrasi, ambiguita', ponte fra entita', uso/menzione, registro,
   pragmatica, correzione, piano situazionale, sopravvivenza, triangolazione,
   azione informativa, bilancio dinamico, multi-goal, scope, conversazione
   lunga, cross-domain, negativo vicino) esistono come ratchet SPARSI, non come
   batteria unica con le metriche del §9.3. Finche' non esiste, la percentuale
   di completamento e' una stima nostra e non una misura.
2. **La latenza (§10).** `reflexive_audit.p0t` porta un `!timeout 3` messo come
   CEROTTO: quel turno costava 0,18s al gen382 e oggi ne costa 0,9. E' un
   fattore cinque riguadagnato mentre la KB cresceva, e il §10 dice di
   classificarlo come meccanica del solver. `findall/3` senza uscita anticipata
   e' il nodo noto.
3. **Due descrizioni nello stesso turno** non compongono: enumerarle
   esaustivamente intreccia effetti e ricerca, e il solver ribacktracka dentro
   gli assert. Si chiude separando gli effetti dalla ricerca.
4. **Rifiniture dichiarate dei livelli situazionali:** alternative scartate e
   residui nella resa (398g), dipendenze PER-PASSO invece del solo traguardo
   (398e), separazione fra prova e fonte debole (398d), reversibilita' come
   dimensione a se' (398f).
5. **Dimensioni di registro oltre tecnicita' e formalita'** (396.1): concisione,
   direttezza, cortesia, hedging.
6. **`docs/plans/parrot0-100-failures.md`** — il censimento e' chiuso solo in
   parte. Chiuse le famiglie numeriche (confronto, ordinamento, resto, mediana)
   e il contesto dichiarato; restano logica formale (contrapposizione,
   affermazione del conseguente), meta-domande sul sapere, salienza in un log,
   trasferimento di pattern, e la stipulazione su un SIMBOLO
   («immagina che 2 vale 3 quanto fa 2+2»), che chiede una sostituzione dentro
   l'aritmetica e non solo un contesto ipotetico.

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

La batteria di design e' in `tests/probes/situational_reasoning_probe.py`; il trascritto
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

**Stato di attuazione.** Precondizioni, effetti e applicabilita' sono arrivati
con gen398a; i CONSUMI sono ora in `situation.p0` (`action_uses/3`,
`resource_available/3`), e portano una regola di onesta' tutta loro:

```text
come faccio ad abbassare il livello -> Puoi svuotare a mano.
!assert action_uses(svuota, secchio, 1)
come faccio ad abbassare il livello -> Non posso: mi manca un secchio.
```

L'ignoto NON e' un'assunzione, qui. Di una proprieta' si puo' dire «se la valvola
funziona» ed e' una proposta legittima; di una risorsa mai nominata non si puo'
dire «se hai tre secchi», perche' quella frase suggerisce che i secchi ci siano.
Il silenzio sul mondo si assume, quello su cio' che si possiede no — ed e'
esattamente la riga del piano «una risorsa non menzionata e non derivabile non
puo' apparire nel piano». Dichiarare di averne zero e' invece un'informazione, e
porta allo stesso esito della penuria: la quantita' viene CONFRONTATA.

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

**Secondo taglio: il MOTIVO.** Un piano che non sa dirsi non e' ispezionabile, e
il §17.10 chiede che ogni proposizione della risposta risalga a testo, KB o
derivazione. Il motivo e' letteralmente la catena che lo ha prodotto:

```text
come faccio ad abbassare il livello -> Puoi aprire la valvola.
perche' questo piano                -> Perche' aprire la valvola rende vero:
                                       la valvola aperta, e da li' il deflusso
                                       abbassa il livello.
```

Non una spiegazione scritta a parte — la stessa derivazione, detta. Tolto il nome
di un anello la spiegazione non esce a meta': o si dice tutta la catena o non si
dice, perche' una frase troncata sembrerebbe una risposta. E tolta la legge
causale non resta una spiegazione memorizzata a fare da fondo: e' l'ablazione che
distingue una derivazione da una frase.

La superficie «perche'» nuda NON e' stata presa: ha gia' un consumer dal
proof-trace e fa una cosa diversa. Le due convivono.

Restano aperte le alternative scartate, i residui e il confronto frontier vero e
proprio.

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

## 18. Ipotesi frontier: supercomprensione documentale e metacostruttiva

Questa sezione estende K0-K11 senza dichiarare implementata una nuova facolta'.
Lo stimolo e' il confine successivo al ragionamento situazionale: parrot0 puo'
rappresentare uno stato e pianificare dentro di esso, ma un testo complesso non
gli consegna lo stato gia' pronto. Lo costruisce progressivamente, mentre
distingue la voce dell'autore dai dati, il metodo dai risultati, una spiegazione
da un'osservazione e una conclusione dalle condizioni che la limitano.

Chiamo **supercomprensione** una proprieta' operativa, non uno slogan:

> comprendere un testo significa costruire il piu' piccolo modello
> interrogabile che ne conserva claim, ruoli, dipendenze, scope, fonti,
> procedure e alternative; metacomprenderlo significa sapere quali parti del
> modello sono provate, quali ipotetiche, quali mancanti e quale operazione di
> lettura ridurrebbe davvero l'incertezza.

La parola *super* non autorizza output piu' lunghi. Al contrario, il segnale e'
la capacita' di rispondere a molte domande nuove da una rappresentazione piu'
compatta del testo, di produrre un controesempio, di eseguire una procedura
estratta e di spiegare quale frase sostiene ogni passo.

### 18.1 Ipotesi D1 — il documento e' un programma epistemico

Un documento non e' soltanto un contenitore di proposizioni. Esegue mosse sullo
stato epistemico del lettore: introduce un problema, definisce termini,
stabilisce assunzioni, descrive un metodo, presenta evidenze, licenzia una
conclusione, la qualifica e apre un residuo. L'ipotesi e' che queste mosse
possano vivere nello stesso modello K3 delle mosse dialogiche:

```prolog
document_unit($Document, $Unit, $Order).
unit_act($Unit, $Act).
unit_content($Unit, $Proposition).
unit_source_span($Unit, $Span).
unit_scope($Unit, $Context).
unit_addresses($Unit, $Issue).
unit_opens($Unit, $Issue).
```

`Act` non deve diventare un elenco chiuso nel C. Definizione, osservazione,
metodo, risultato, obiezione e limite sono membri insegnabili di una relazione
aperta. La meccanica puo' ordinare unita' e collegare span; la KB decide che cosa
una forma discorsiva segnala.

**Predizione falsificabile.** Insegnare a voce un nuovo marcatore di limite deve
cambiare la lettura di tre testi non correlati e la sua retrazione deve togliere
soltanto quella lettura, non i claim delle frasi.

### 18.2 Ipotesi D2 — il claim e' distinto dalla frase e dalla sua forza

La stessa proposizione puo' essere osservata, ipotizzata, negata, attribuita,
assunta per il calcolo o citata come posizione altrui. K4 offre gia' i contesti;
manca il ponte sistematico fra unita' documentale, proposizione e status:

```prolog
unit_claim($Unit, $Claim).
claim_proposition($Claim, $Proposition).
claim_status($Claim, observed).
claim_status($Claim, inferred).
claim_status($Claim, hypothesized).
claim_attributed_to($Claim, $Agent).
claim_supported_by($Claim, $Evidence).
claim_qualified_by($Claim, $Qualifier).
claim_source($Claim, $Source).
```

Non si duplica `holds_in/2`: il claim e' l'atto epistemico compiuto su una
proposizione; il contesto dice dove la proposizione vale. `confidence` non viene
inventata come percentuale: nasce dalla classe di supporto, dalla provenienza e
dai conflitti effettivi.

**Predizione falsificabile.** «I dati mostrano X; gli autori ipotizzano Y» deve
permettere di rispondere sì a «e' stato osservato X?» e no a «e' stato osservato
Y?», pur conservando Y come ipotesi interrogabile.

### 18.3 Ipotesi D3 — l'argomento e' un grafo, non un riassunto

Il proof engine dimostra conseguenze da clausole gia' normalizzate. La prosa
scientifica consegna invece relazioni fra claim che devono essere ricostruite:
supporto congiunto, alternativa, eccezione, qualifica e confutazione.

```prolog
argument_node($Argument, $Claim).
argument_edge($From, $To, supports).
argument_edge($From, $To, attacks).
argument_edge($From, $To, qualifies).
support_set($Support, $Conclusion).
support_member($Support, $Claim).
argument_residue($Argument, $Gap).
```

`support_set` e' necessario: due osservazioni possono sostenere una conclusione
solo insieme, mentre ciascuna isolata e' insufficiente. Convertire ogni arco in
una regola binaria produrrebbe conclusioni troppo forti.

**Predizione falsificabile.** Ablare un membro di un supporto congiunto deve
ritirare la conclusione; ablare un dettaglio retorico non portante deve
lasciarla. La domanda «che cosa la confuterebbe?» deve produrre una condizione
logica, non una frase pessimista.

### 18.4 Ipotesi D4 — leggere scienza richiede una Experimental IR

Una procedura descrive come agire; un disegno scientifico descrive anche perche'
un risultato conta come evidenza. Il Situation IR di K11 va riusato ma non basta:

```prolog
study_question($Study, $Question).
study_system($Study, $System).
study_variable($Study, $Variable, $Role).
study_group($Study, $Group, $Role).
study_operation($Study, $Step).
study_measure($Study, $Measure).
study_control($Study, $Control).
study_result($Study, $Claim).
study_limitation($Study, $Limitation).
```

I ruoli `intervention`, `outcome`, `control`, `confounder`, `parameter` e
`measurement` sono conoscenza insegnabile. Il motore non deduce causalita' dal
solo ordine temporale e non inventa un controllo per completare lo schema.

**Predizione falsificabile.** La stessa struttura deve ricostruire un esperimento
controllato, uno studio osservazionale e una simulazione, conservando la
differenza fra i tre invece di forzarli nello stesso template.

### 18.5 Ipotesi D5 — una procedura appresa e' una proof obligation

M10 e K11 chiedono piani eseguibili. La prosa tecnica aggiunge dettagli che una
lista di passi perde: tipi, unita', precondizioni, invarianti, ripetizione,
branch, criterio di arresto, failure mode e validazione.

```prolog
procedure_input($Procedure, $Slot, $Type).
procedure_output($Procedure, $Slot, $Type).
procedure_step($Procedure, $Index, $Action).
step_requires($Procedure, $Index, $Condition).
step_establishes($Procedure, $Index, $Condition).
step_invariant($Procedure, $Index, $Invariant).
step_branch($Procedure, $Index, $Condition, $Target).
procedure_stop($Procedure, $Criterion).
procedure_validation($Procedure, $Check).
```

Compilare non significa eseguire durante la lezione. Prima il candidato entra
in quarantena; poi type, dimensioni e precondizioni vengono verificati; soltanto
dopo puo' essere applicato a valori held-out.

**Predizione falsificabile.** Una procedura di conversione e una di laboratorio
devono condividere scheduling, binding e trace pur avendo azioni di dominio
diverse. Un numero senza ruolo o un'unita' incompatibile deve produrre un
residuo, mai un risultato.

### 18.6 Ipotesi D6 — la comprensione forte vive nello spazio dei modelli

Una proof sola risponde «questa conclusione segue?». La comprensione profonda
richiede anche: quali mondi soddisfano il testo, quale vincolo li elimina, quale
proposizione e' invariante e quale controesempio separa necessario da possibile.
K4 e i mondi concorrenti di 398d forniscono il substrato.

```prolog
model_candidate($Problem, $Model).
model_satisfies($Model, $Constraint).
model_violates($Model, $Constraint).
model_entails($ModelSet, $Proposition).
countermodel($Claim, $Model).
model_invariant($ModelSet, $Proposition).
discriminating_observation($Models, $Observation).
```

La ricerca bounded puo' vivere nel C, ma vincoli, operatori, domini e criteri di
equivalenza vivono nella KB. Se il budget non basta, l'esito e' `incomplete`,
non «nessun controesempio».

**Predizione falsificabile.** Lo stesso operatore trova un contromodello per una
tesi logica, un caso limite per una procedura e un mondo alternativo per
un'ipotesi scientifica senza condividere lessico di dominio.

### 18.7 Ipotesi D7 — la causalita' e' una disciplina di archi

La prosa usa «causa» in modi eterogenei. Vanno tenuti distinti almeno:
associazione osservata, ordine temporale, meccanismo proposto, intervento,
condizione necessaria, condizione sufficiente, mediatore e confondente.

```prolog
causal_claim($Cause, $Effect, $Kind).
causal_support($Claim, $Evidence, $Design).
causal_path($Cause, $Effect, $Path).
causal_alternative($Claim, $Alternative).
intervention_prediction($Action, $Outcome).
```

Le leggi `causal_law/2` di K11 restano effetti del mondo; `causal_claim` e'
cio' che un documento sostiene su quegli effetti. Confonderli farebbe diventare
vera una spiegazione soltanto perche' qualcuno l'ha scritta.

**Predizione falsificabile.** Da una correlazione il sistema puo' conservare
ipotesi causali candidate, ma non deve rispondere a un controfattuale come se
una fosse provata. Aggiungere un disegno d'intervento verificato puo' cambiare
lo status senza cambiare la frase originaria.

### 18.8 Ipotesi D8 — la sintesi e' compressione con recupero

Un riassunto e' valido se conserva una base sufficiente a recuperare i claim
decisivi e le loro qualifiche. Non e' valido per somiglianza lessicale.

```prolog
summary_unit($Summary, $Proposition).
summary_covers($Summary, $Claim).
summary_omits($Summary, $Claim, $Reason).
summary_source($Summary, $Unit).
summary_budget($Summary, $Constraint).
recoverable_from($Claim, $Summary).
```

La stessa rappresentazione genera una frase, cinque punti o una spiegazione
estesa variando il budget K5, senza cambiare la teoria del documento. Un limite
non puo' essere eliminato se qualifica direttamente il claim principale.

**Predizione falsificabile.** Dopo avere ritratto il testo originale dalla
memoria attiva, le domande decisive restano risolvibili dalla sintesi con link
alla provenance; le domande sui dettagli dichiaratamente omessi no.

### 18.9 Ipotesi D9 — la metacomprensione e' controllo attivo della lettura

M13 tipizza il gap. Il passo ulteriore e' scegliere una mossa di lettura che
abbia fan-out e information gain: risolvere un pronome, chiedere una definizione,
rileggere una tabella, cercare una premessa, confrontare due scope o sospendere
la conclusione.

```prolog
reading_goal($Session, $Goal).
reading_gap($Session, $Gap, $Kind).
reading_action($Action, $Kind).
action_expected_resolution($Action, $Gap, $Gain).
reading_policy($State, $Action).
reading_checkpoint($Session, $Model).
```

Non si introduce un menu fisso nel C. Le azioni disponibili, la loro
applicabilita' e la policy sono KB; il motore ordina candidati e applica budget.
Una domanda e' utile soltanto se le risposte possibili cambiano una lettura o
una conclusione.

**Predizione falsificabile.** Su gap lessicale, coreferenziale, causale e
procedurale il sistema sceglie quattro rimedi diversi; insegnare un nuovo rimedio
lo rende selezionabile al turno dopo e retract lo elimina.

### 18.10 Ipotesi D10 — l'induzione metacostruttiva crea candidati, non verita'

M3 sa ricevere alcune equivalenze esplicite. La forma piu' forte mostra esempi e
controesempi naturali e lascia a parrot0 il compito di ipotizzare la costruzione,
i ruoli e il dominio di validita'.

```prolog
construction_example($Lesson, $Surface, $Reading, positive).
construction_example($Lesson, $Surface, $Reading, negative).
induced_construction($Lesson, $Candidate).
candidate_scope($Candidate, $Scope).
candidate_support($Candidate, $Example).
candidate_counterexample($Candidate, $Example).
candidate_status($Candidate, quarantine).
```

La meccanica puo' cercare allineamenti e sostituzioni; non puo' decidere che una
parola particolare sia un quantificatore, una concessiva o un marcatore di
metodo. Una singola ipotesi coerente non e' ancora conoscenza: servono transfer,
contrasto e ablation del protocollo.

**Predizione falsificabile.** Tre esempi veri e un controesempio devono produrre
una costruzione che trasferisce a un dominio non mostrato; cambiando il
controesempio deve restringersi lo scope senza cambiare il C.

### 18.11 Ipotesi D11 — piu' documenti formano un dibattito, non un merge

Due articoli possono usare parole diverse per lo stesso concetto, la stessa
parola per misure diverse o risultati opposti sotto popolazioni diverse. La
fusione deve quindi passare per forma/senso K0, contesti K4 e grafi D3-D4.

```prolog
cross_document_alignment($ClaimA, $ClaimB, $Relation).
comparison_basis($Alignment, $Dimension).
apparent_conflict($ClaimA, $ClaimB, $Reason).
substantive_conflict($ClaimA, $ClaimB, $Proposition).
evidence_scope($Claim, $Population).
```

`Relation` puo' essere `agrees`, `extends`, `narrows`, `uses_different_measure`,
`apparent_conflict` o `contradicts`. Non si calcola una confidence globale
facendo la media di etichette incomparabili.

**Predizione falsificabile.** Il sistema distingue un conflitto reale da uno
dovuto a misura, popolazione o scope, e puo' indicare quale nuova osservazione
renderebbe confrontabili i lavori.

### 18.12 Ipotesi D12 — la supercomprensione e' ricorsiva ma finita

Il lettore puo' trattare il proprio modello del documento come un altro oggetto:
chiedere quali claim non hanno supporto, quali procedure hanno slot non legati,
quali termini restano ambigui e quali limiti qualificano la tesi. Questa e' la
chiusura riflessiva di `PRINCIPLES.md`, ma deve rispettare budget e arresto.

```prolog
model_audit($Model, $Finding).
unsupported_claim($Model, $Claim).
unbound_procedure_slot($Model, $Slot).
unresolved_reference($Model, $Reference).
unpropagated_qualification($Model, $Claim).
audit_priority($Finding, $Fanout).
```

Il controllo non ricomincia indefinitamente. Termina quando non resta un gap che
cambi il goal di lettura, quando il budget e' esaurito o quando serve autorita'
esterna. L'arresto conserva il residuo tipizzato.

**Predizione falsificabile.** Inserire intenzionalmente una premessa mancante,
un pronome ambiguo e un'unita' incoerente deve produrre tre finding distinti e
ordinati per impatto sulla tesi, non tre muri uguali.

### 18.13 Architettura comune proposta: Document IR

Le dodici ipotesi convergono su una sola estensione della catena universale:

```text
span e cue K0
  -> frame/letture K1-K2
  -> unita' documentali e atti D1
  -> claim + scope + provenance D2
  -> grafi argomentativi / sperimentali / procedurali D3-D5
  -> modelli, causalita' e alternative D6-D7
  -> goal di lettura e controllo metacognitivo D9-D12
  -> proof e answer plan K6
  -> realizzazione per lingua, registro e budget
```

Il Document IR non e' un nuovo router. E' una vista strutturata dello stesso
turno e della stessa KB. Un consumer che ri-parsa il testo per riconoscere
`however`, `therefore`, `methods` o `we found` viola il piano anche se il suo
output e' perfetto: quelle superfici devono essere membri KB della relazione
discorsiva pertinente.

### 18.14 Ordine sperimentale

L'ordine e' vincolato dalle dipendenze, non dall'effetto dimostrativo:

1. **D1-D2:** unita', atti, claim, status e provenance; senza questi qualunque
   estrazione successiva puo' consolidare una frase sotto la voce sbagliata;
2. **D3:** argomento e supporti congiunti; rende misurabile che cosa una
   conclusione perde;
3. **D4-D5:** disegno scientifico e procedure; separa conoscere da saper fare;
4. **D6-D7:** modelli e causalita'; autorizza controesempi e controfattuali;
5. **D8:** sintesi soltanto dopo che esiste qualcosa di strutturato da
   comprimere;
6. **D9-D10:** controllo attivo e induzione in quarantena;
7. **D11-D12:** integrazione fra fonti e audit ricorsivo;
8. **ratchet integrato:** articolo held-out, lingua diversa, domande avversariali
   e fresh-process recall dei soli fatti promossi.

Ogni incremento segue `LEARN_PROTOCOL.md`: fonte vera, baseline prima della
lezione, replay, `Transfer@3`, parafrasi, contrasto, composizione, ablation,
provenance, `/save`, diff semantico e processo nuovo. Se un testo produce anche
un solo claim falso attivo, non viene salvato.

### 18.15 Gate di supercomprensione

Il fronte e' riuscito soltanto quando, su documenti mai usati per costruirlo:

1. ogni claim importante conserva frase fonte, autore, scope e status;
2. tesi, metodo, risultato e limite sono distinti e interrogabili;
3. i supporti congiunti non vengono indeboliti in regole indipendenti;
4. una procedura estratta trasferisce a input nuovi e rende visibili i propri
   pre-requisiti;
5. correlazione, meccanismo e causalita' provata non collassano;
6. un controesempio valido cambia il modello e le sole conclusioni dipendenti;
7. il riassunto non introduce claim e conserva i qualificatori portanti;
8. due fonti discordanti restano due viste attribuite finche' una regola
   giustifica il confronto;
9. parrot0 sa nominare il gap di lettura e scegliere una domanda che potrebbe
   risolverlo;
10. una costruzione discorsiva, un ruolo scientifico o una policy di lettura
    nuova entra e si ritrae a runtime;
11. il C non contiene vocabolario di genere, dominio o lingua;
12. latenza e ricerca incompleta rispettano il contratto del §10.

Il non-obiettivo e' produrre una recensione che *sembra* competente. Il target
e' piu' severo: un modello logico del documento dal quale recensione, risposta,
procedura, controesempio e domanda di chiarimento siano viste diverse della
stessa comprensione.

### 18.16 Evidenza SC0 — prima del Document IR serve un envelope epistemico

Il ciclo SC0 del 2026-08-29 ha falsificato una premessa implicita di D1: non
basta che esista un lettore. Il documento deve arrivargli come **un solo atto
epistemico protetto**. Sul medesimo testo Apollo 13, «Leggi questo breve testo:»
veniva preso dal generatore e trasformato in una storia inventata; insegnare
soltanto che `leggi` e' un imperativo eliminava la storia ma lasciava un muro.
La superficie non possedeva ancora un ruolo instradabile.

L'incremento ha introdotto quattro decisioni aperte e separabili:

```prolog
% la superficie nuova eredita il ruolo da un modello gia' funzionante
segment_role(prose_source, "leggi questo breve testo:").

% il ruolo possiede il payload, anche se dentro compare lessico di altri registri
segment_extent(prose_source, whole).

% il consumer dichiarato riceve la prima offerta
faculty_dispatch(reader, eager).

% il suo conteggio di acquisiti/saltati e' un esito, non un muro da coprire
module_result_policy(reader, terminal).
```

Il primo fatto non viene scritto dal teacher: nasce dalla lezione naturale
«impara “Leggi questo breve testo:” come un altro modo per introdurre
“leggi:”». Il motore risolve il modello con lo stesso segmentatore universale,
copia il ruolo soltanto se esiste un consumer, e conserva tutto in sessione.
Le altre tre relazioni sono politiche generali: un ruolo, una facolta' o un
modulo nuovi possono diventarne membri senza ricompilare.

Il caso avversario decisivo e' stato *measurement error*. Prima della policy di
extent, `error` forniva evidenza al registro dei diagnostici di compilazione e
il testo scientifico perdeva il lettore. Non era un problema della parola
`error`: era l'assenza di una regola su chi possiede lo span. Dopo la policy, la
stessa forma insegnata trasferisce attraverso il conflitto di registro.

**Risultato misurato.** Crescita/replay/ablazione/riapprendimento e arbitraggio
sono coperti da 21 assert nel ratchet dedicato. In chat, la forma nuova ha
instradato tre testi finali su tre senza narrativa. Ma Apollo, subduzione e un
contrasto sulla forza causale hanno prodotto insieme `0` fatti e `9` frasi
saltate. D1-D12 non sono quindi convalidate: SC0 ha costruito il prerequisito
che permette finalmente di misurarle senza che un altro modulo nasconda il
fallimento.

Da questa evidenza segue una nuova ipotesi, **D0**:

> ogni documento deve entrare in un envelope epistemico insegnabile che lega
> superficie, extent, facolta' e politica d'esito; soltanto dentro quell'envelope
> le unita' D1 possono essere segmentate senza contaminazione da registri o
> intenti concorrenti.

**Predizione falsificabile.** Una nuova formula introduttiva insegnata da un
modello deve instradare tre generi diversi, sopravvivere a lessico che altrove
segnala codice/log e smettere immediatamente dopo l'ablazione. A parita' di D0,
gli errori residui devono comparire come unita' saltate o gap documentali, mai
come narrativa o output di un registro concorrente.

### 18.17 Evidenza SC1 — il documento deve sopravvivere al proprio parser

Il ciclo SC1 del 2026-08-29 ha provato la parte minima di D1 e ha ristretto
altre cinque ipotesi. La baseline su Apollo 13 attraversava correttamente il
reader costruito da D0, ma terminava con `0` fatti, `2` frasi saltate e nessun
oggetto interrogabile. La lezione naturale
`"albeit" is a contrastive connector` entrava nella classe linguistica; il
replay restava identico. La forma era quindi insegnabile, ma nessun consumer
poteva applicarla dopo che `current_prose` era stato cancellato.

L'incremento introduce una proiezione documentale di sessione:

```prolog
document_unit($Document, $Unit, $Order).
document_unit_source($Document, $Unit, $Surface).
document_unit_token($Unit, $Token, token($Surface, range($Begin, $End))).

rhetorical_marker_class(contrastive_connector, contrast,
                         previous_to_current).
rhetorical_edge($Document, $Left, $Right, contrast).
```

Il C assegna soltanto handle monotone, ordine e invoca
`document_unit_observe/4` prima del clear. La KB copia token e range dalla
struttura universale e deriva l'arco applicando la classe aperta con `apply/2`.
Nessuna superficie contrastiva compare nel consumer. Insegnare
`nevertheless` ha cambiato immediatamente il replay e tre documenti trasferiti;
ritrarlo ha eliminato gli archi gia' derivati senza eliminare unita', span o il
fatto indipendente `metal(mercury)`. Reteach e fresh-process recall hanno
ricostruito la stessa vista.

Da questa evidenza emergono sei affinamenti falsificabili.

**D1a — copy-on-observe precede qualunque interpretazione distruttiva.** Una
vista transiente e' sufficiente per riconoscere una frase, non per comprendere
un documento. Il producer deve pubblicare unita', ordine e geometria prima che
un parser specializzato ripulisca il proprio workspace. La copia non decide
ancora se l'unita' sia metodo, risultato o limite: conserva evidenza riusabile.

**Predizione.** Due consumer diversi devono poter derivare viste indipendenti
dalle stesse unita' senza ri-tokenizzare la superficie e senza dipendere
dall'ordine in cui vengono eseguiti.

**D1b — la retorica si fattorizza in classe, relazione e direzione.** Un marker
non e' l'arco. `contrastive_connector` e' una classe insegnabile;
`rhetorical_marker_class/3` ne dichiara lettura e orientamento;
`rhetorical_edge/4` applica quella politica a un contesto strutturale. Questa
fattorizzazione permette a nuove superfici di trasferire senza nuovo C e a una
stessa classe di ricevere in futuro politiche diverse per scope o genere.

**Predizione.** Insegnare una nuova superficie alla classe esistente deve
creare gli stessi archi; cambiare a runtime soltanto la mappa di direzione deve
invertire source/target senza cambiare token o unita'.

**D1c — le relazioni documentali iniziali devono essere viste derivate.** Se
l'arco fosse materializzato al momento della lettura, l'ablation della cue
lascerebbe conoscenza fossile. Poiche' e' derivato, retract cambia la risposta
sui documenti gia' osservati e conserva lo strato evidenziale piu' debole.
Questa e' la forma documentale dell'invalidazione causale del mantra 15.

**Predizione.** Ritrarre una cue, uno status o una regola deve eliminare
esattamente le viste che ne dipendono; reinserirla deve farle riapparire senza
rileggere il testo, finche' le unita' fonte restano in sessione.

**D1d — apprendimento e correzione devono condividere la lettura.** Due sonde
abortite hanno trasformato «forget that X is a contrastive connector» in
`contrastive_connector(forget)`: la lezione e il retract attraversavano parser
asimmetrici. La correzione non e' una utility successiva all'apprendimento; e'
il suo inverso semantico e deve usare la stessa analisi pura di membership.

**Predizione.** Per ogni forma naturale che asserisce una relazione learnable,
la stessa proposizione sotto un atto di retract deve risolvere predicato e
argomenti identici. Le forme che entrano ma non possono uscire bloccano la
promozione anche quando replay e transfer sono verdi.

**D1e — identita' di sessione e identita' epistemica non coincidono.**
`document_N` risolve l'ordine locale ma colliderebbe dopo un riavvio. Per questo
le sessioni documentali non sono state salvate: persisterle avrebbe simulato
provenance. D2 deve ancorare un documento a fonte/versione/fingerprint e un
claim ai suoi span; il contatore resta al massimo una handle effimera.

**Predizione.** Lo stesso documento riletto puo' essere riconciliato con la
stessa identita' epistemica, mentre due versioni o due fonti con testo simile
restano distinte. Un processo nuovo non puo' fondere documenti soltanto perche'
entrambi ricominciano dal numero uno.

**D2a — claim, attribuzione e commitment sono assi ortogonali.** SC1 ottiene
`Relation fidelity=4/4` su replay e transfer ma soltanto `Claim coverage=1/8`.
L'arco corretto non implica che il contenuto sia stato capito. SC2 deve quindi
modellare separatamente proposizione, chi la presenta, con quale status e se
parrot0 la assume:

```prolog
document_claim($Document, $Claim, $Proposition).
claim_source_span($Claim, $Unit, $Range).
claim_attributed_to($Claim, $Agent).
claim_status($Claim, hypothesis).
claim_status($Claim, observation).
claim_commitment($Claim, reported).
```

Un testo «gli autori ipotizzano X; i dati mostrano Y» deve conservare X come
ipotesi attribuita e Y come osservazione riportata. Nessuna delle due diventa
automaticamente una credenza non qualificata di parrot0. Le cue di status sono
classi KB insegnabili e retraibili; il C non riconosce `hypothesize`, `show`,
`authors` o `data`.

**Predizione.** Alla domanda se X sia stato osservato la risposta e' negativa,
ma X resta recuperabile come ipotesi degli autori. Ritrarre la cue di ipotesi
rimuove lo status derivato, non la frase, la proposition o l'attribuzione. La
stessa distinzione deve trasferire a esperimento, studio osservazionale e
simulazione.

**Risultato misurato.** `Transfer@3=3/3`, contrasto negativo `1/1`, ablation e
reteach verdi, `FreshProcessRecall=2/2`; il ratchet persistente contiene 33
assert. Il save pulito promuove soltanto la lezione linguistica e le sue tracce:
`W=0, L=1, C=0, P=1, O=3, X=0, S=5`. Questi numeri chiudono **SC1-A**, non D1
intera: cue multi-parola, archi intra-periodo/a distanza, `unit_act`, identita'
persistibile e claim tipati restano gate espliciti di SC1-B/SC2.

### 18.18 Evidenza SC2 — un claim compreso comincia da cio' che non si deve credere

Il ciclo SC2-A del 2026-08-29 ha chiuso il primo strato operativo di D2a e ha
falsificato una seconda scorciatoia: riconoscere correttamente lo status di una
frase non significa ancora averne normalizzato la proposizione.

La baseline su prosa scientifica breve mostrava due perdite indipendenti. Una
frase Salmonella di sedici token spariva dal Document IR perche' la copia SC1
attraversava una lista ricorsiva oltre il limite pratico del solver. Le domande
«che cosa ipotizzano gli autori?», «e' stato osservato?» e «che cosa mostra la
simulazione?» non raggiungevano alcun claim. Inoltre la lezione quotata
multi-parola veniva compressa o il suo retract produceva
`hypothesis_report_marker(forget)`.

L'incremento conserva oggi questo envelope:

```prolog
document_source($Document, $URI).
document_fingerprint($Document, $Fingerprint).
document_claim($Document, $Claim, proposition(surface($Text))).
claim_source_span($Claim, $Unit, $Range).
claim_attributed_to($Claim, $Agent).
claim_status($Claim, $Status).              % vista viva
claim_context($Claim, context($Claim)).     % vista viva
claim_commitment($Claim, attributed_only).  % vista viva
```

Il documento source-addressed usa hash separati della coordinata e del
contenuto. Il C produce hash, handle, ordine e range; il riconoscimento del
marker attraversa lo scorer universale. La KB dichiara la semantica della
classe:

```prolog
claim_marker_class(
    hypothesis_report_marker,
    reading(hypothesized, reported_belief),
    attribution(document_authors),
    extent(remainder)).
```

Un nuovo membro multi-parola entra parlando ed e' consumato con `apply/2`.
Status, contesto e commitment dipendono dalla membership viva della cue; fonte,
span, superficie e attribuzione restano osservazioni piu' deboli. Ritrarre una
cue elimina quindi la lettura epistemica anche da una claim gia' osservata, ma
non cancella l'evidenza da cui una lettura futura potra' essere ricostruita.

Da questa evidenza seguono sei ipotesi piu' strette.

**D1f — un limite di ricorsione non puo' diventare un limite linguistico.** Una
lista ricorsiva e' una rappresentazione possibile dei token, non il contratto
del documento. Il producer enumera ora i node ID e lascia alla KB la selezione
dei token: la lunghezza della frase non cambia la sua osservabilita' fino ai
limiti espliciti del buffer.

**Predizione.** A parita' di buffer, aggiungere token a una frase deve aumentare
linearmente le osservazioni e non far sparire l'intera unita'. Una futura
struttura annidata deve poter essere copiata per node senza introdurre un nuovo
predicato C per ciascun tipo linguistico.

**D2b — la proposition di superficie e' quarantena, non comprensione.** Il
remainder sotto `proposition(surface(Text))` permette provenance, retrieval e
correzione senza inventare semantica. Non permette equivalenza di parafrasi,
composizione logica o risposta su argomenti e relazioni.

**Predizione.** Una metrica onesta puo' avere `SurfaceClaimCoverage=100%` e
`NormalizedClaimCoverage=0%`. SC2-B e' chiusa soltanto quando la stessa pipeline
semantica usata dalle asserzioni normali produce frame interrogabili con proof
verso la superficie, e fallisce apertamente sui remainder ambigui.

**D2c — lo status deve essere una vista viva sopra evidenza durevole.** Una cue
e' conoscenza correggibile. Materializzare `hypothesized` durante la lettura
creerebbe un fossile dopo retract; materializzare che quella cue apparve in
quello span conserva invece il dato necessario a reinterpretare.

**Predizione.** Ablation di una cue elimina status/context/commitment e lascia
claim/source/span; reteach li riattiva senza rilettura. Ablation di una sola
parafrasi non deve spegnere claim osservate con un altro membro della classe.

**D2d — identita' epistemica = coordinata di fonte + versione del contenuto.**
SC2 usa URI e fingerprint dei byte come primo identificatore riproducibile. E'
piu' forte di `document_N`, ma non e' ancora canonicalizzazione: redirect, DOI,
frammenti, copie equivalenti e revisioni editoriali possono richiedere
coordinate diverse o un arco di equivalenza provato.

**Predizione.** Stessa coordinata e stessi byte danno lo stesso documento fra
processi; una modifica del contenuto cambia ID; due URI equivalenti non vengono
fusi senza una relazione KB/provenance esplicita. Un hash non conferisce
autenticita' o autorevolezza.

**D2e — la metalingua multi-parola richiede reversibilita' esatta.** Una
locuzione quotata e' una superficie atomica per l'atto didattico anche quando il
tokenizer la divide. Articoli e complementatori dentro le virgolette non sono
plumbing da eliminare: sono parte dell'evidenza che lo scorer deve ritrovare.

**Predizione.** Learn, query e retract devono risolvere lo stesso termine
quotato byte per byte; una nuova locuzione con determinante deve entrare e
uscire senza generare un fatto sul verbo `forget`. Escape e virgolette annidate
restano un gate separato.

**D2f — «riportato» non e' una probabilita' ne' un fatto del mondo.** Ipotesi,
osservazione descritta e risultato simulato sono oggi tre status sotto il
contesto `reported_belief`. Il sistema conserva la differenza, ma non assegna
confidence numerica e non promuove automaticamente nessuno dei tre in `world`.

**Predizione.** Una domanda osservativa non deve essere soddisfatta da una
ipotesi o da una simulazione. L'eventuale adozione di un claim nel mondo deve
richiedere una policy separata, fontata e retraibile; non puo' emergere dal solo
verbo riportativo.

**Risultato misurato.** Quattro locuzioni sono state insegnate e salvate;
`Transfer@3=3/3`, parafrasi `1/1`, contrasto `1/1`, ablation/reteach verdi,
status fidelity `8/8`, source/span fidelity `8/8` e
`FreshProcessRecall=5/5`. Il save e' `W=0, L=4, C=0, P=4, O=12, X=0, S=20`.
Contemporaneamente `NormalizedClaimCoverage=0/8` e
`NaturalClaimQuestionCoverage=0/3`: questi due zeri sono il gate autoritativo
di **SC2-B**, non debito cosmetico.

SC2-B deve riusare la pipeline semantica dichiarativa comune sul remainder,
conservare in parallelo la superficie e costruire domande naturali KB-first su
attribuzione e status. Solo allora SC3 potra' collegare premise e conclusioni
senza ragionare direttamente su stringhe.

### 18.19 Evidenza SC2-B — la fase pura, e i due zeri che sono diventati numeri

Il ciclo SC2-B del 2026-08-29
([report](../labs/apprendimento-assistito/2026-08-29-supercomprensione-sc2b.md))
ha chiuso i due zeri autoritativi lasciati da SC2-A e ha falsificato una terza
scorciatoia, piu' profonda delle prime due.

**Che cosa era davvero bloccato.** Non mancava un parser. La pipeline semantica
dichiarativa esisteva gia' — `extract_frame/2`, gli slot, i confini di sintagma
— ma *analizzare* una clausola e *commettere* il fatto che ne esce erano lo
stesso corpo di funzione. Nessuno poteva chiedere al motore «come leggeresti
questa frase?» senza che la frase diventasse conoscenza. Percio' il remainder di
una claim riportata si poteva conservare come byte e mai normalizzare: l'unico
modo di normalizzarlo era crederci.

**L'incremento.** La fase pura ha ora un'identita':

```text
ANALIZZARE una clausola dichiarativa  ->  candidato semantico   (fase pura)
COMMETTERE quel candidato nel mondo   ->  conoscenza di parrot0 (fase impura)
```

Il consumer storico usa la stessa funzione, quindi non esistono due legatori di
schemi. Il lettore la invoca sul remainder e consegna alla KB un candidato con
**origine** e **copertura**; la decisione non e' del parser:

```prolog
normalization_origin(reported, quarantine).
normalization_origin(asserted, world).
claim_proposition($Claim, proposition(frame($Predicate, $Roles))).
holds_in(context($Claim), proposition(frame($Predicate, $Roles))).  % vista viva
```

Misurato: nove proposizioni normalizzate su quattro fonti reali, e
`/debug shortened` mostra **nessuna clausola**. Il frame vive dentro il contesto
della claim ed e' invisibile al mondo; «what did DART shorten?» resta un muro,
che e' la risposta corretta.

**La terza scorciatoia falsificata: una lettura parziale non e' una lettura.**
`«DART slowed the orbital period of Dimorphos»` si legava a
`slowed(dart, orbital_period)`. Lo schema combacia, e perde **di chi** sia il
periodo. Nella chat quella perdita esisteva da sempre senza che nessuno la
vedesse; su una claim riportata e' peggio, perche' la verifica di status
confronta *frame con frame* e due proposizioni su due oggetti diversi
collasserebbero nella stessa. Un `Yes` cosi' e' peggio di un muro.

La correzione non e' una condizione cablata. La fase pura riporta quanto della
frase ha consumato, e la soglia e' una policy:

```prolog
normalization_extent_policy($Origin, covered($N), of($M), normalized) :-
    normalization_origin($Origin, quarantine), eq($N, $M).
normalization_extent_policy($Origin, covered($N), of($M), partial) :-
    normalization_origin($Origin, quarantine), lt($N, $M).
```

Coordinazione e complemento pendente diventano quindi `gap(partial_reading)`
tipati, con la superficie intera e nessuna risposta inventata.

**La domanda non e' una seconda lezione.** `claim_question_evidence/2` deriva
`«what did the investigators predict?»` dalla locuzione insegnata togliendone il
complementatore; `claim_status_question_evidence/2` deriva `«observed that»`
dallo status dichiarato nella politica di classe. Chi insegna un marker nuovo
apre insieme la sua porta interrogativa, senza dire una seconda frase.

**Risultato misurato.** `Transfer@3=3/3` su NASA/GMD/WHO/PRL,
`ContrastPrecision=3/3`, ablation e reteach `1/1`, `Retention=pass`,
`FreshProcessRecall=7/7`, `QuestionAnswerCoverage` da `0/3` a `3/3`,
`NormalizedClaimCoverage=9/9` a copertura piena e `0/2` a copertura parziale —
**rifiutate per politica** — con `WorldCommitLeak=0`. Save:
`W=0, L=5, C=0, P=5, O=15, X=0, S=25`, `B1-B0=25`, `R1=R0`.

**Che cosa questo ciclo insegna al piano.** Le tre osservazioni che seguono non
riguardano i documenti: riguardano il motore, e sono la sorgente delle ipotesi
D13-D20.

1. L'accoppiamento fra lettura e credenza non era un difetto del lettore
   documentale: era ovunque. Ogni facolta' che legge ha sempre creduto per il
   fatto stesso di aver capito.
2. Una lettura che combacia parzialmente e' indistinguibile, dall'esterno, da
   una lettura completa — a meno che la copertura non esca dalla funzione. Il
   residuo non letto e' l'informazione piu' preziosa che il sistema buttava via.
3. La porta interrogativa di una conoscenza e' derivabile dalla conoscenza
   stessa. Dove non lo e', quel «dove» e' misurabile e nominabile.

### 18.20 Ipotesi D13-D20: purezza, copertura, spazio logico e curriculum

D1-D12 descrivono che cosa un documento *contiene*. Le otto ipotesi seguenti
descrivono come un lettore puo' contenerlo **senza mentire a se' stesso**, e
nascono da fallimenti misurati in SC0-SC2B, non da un'analogia con gli LLM.

#### 18.20.1 Ipotesi D13 — l'origine e' un parametro di ogni inferenza

SC2-B ha reso pura *una* lettura. L'ipotesi e' che la separazione sia una legge
generale: ogni inferenza di parrot0 dovrebbe poter girare sotto un'origine
dichiarata, e l'origine — non l'inferenza — decide il commitment.

```prolog
inference_origin($Origin, $Commitment).      % world | quarantine | simulated
inference_under($Origin, $Goal, $Result).
origin_visible_from($Inner, $Outer).
origin_collapse($Inner, $Outer, $Policy).    % quando un risultato risale
```

Le origini candidate sono gia' tutte presenti nel sistema come casi speciali
sparsi: la claim riportata (SC2-B), il discorso citato, l'ipotesi controfattuale
(«supponiamo che…»), la simulazione di un piano, il sogno, la sonda di
autocorrezione e la lettura di codice non fidato. Oggi ognuna ha o non ha la
propria quarantena; nessuna la condivide.

**Predizione falsificabile.** Lo stesso operatore puro deve servire almeno tre
di queste origini senza aggiungere un parser, e ritrarre un'origine deve
eliminare esattamente i risultati che dipendevano da essa, non di piu' e non di
meno. Se una sola origine richiede un ramo di C dedicato, l'ipotesi e' falsa.

**Non-obiettivo.** Non e' un sistema di livelli di confidenza. Un'origine non
dice quanto un fatto sia probabile: dice **chi lo sostiene e dove vale**.

#### 18.20.2 Ipotesi D14 — la comprensione si misura in copertura, e il residuo e' un oggetto

Una lettura che consuma metà della frase e ne restituisce un frame pulito e' la
forma piu' pericolosa di incomprensione, perche' e' indistinguibile dal
successo. L'ipotesi e' che ogni lettura debba dichiarare la propria copertura e
**reificare il residuo non letto**, invece di lasciarlo cadere.

```prolog
reading_extent($Reading, covered($N), of($M)).
reading_residue($Reading, span($Start, $Length), $Surface).
residue_kind($Residue, $Kind).       % coordination | negation | modality | apposition | reference
residue_blocks($Residue, $Consumer).
coverage_policy($Origin, $Threshold, $Verdict).
```

La conseguenza forte non e' l'onesta': e' che **il residuo diventa un'agenda**.
Se ogni frase non compresa lascia un oggetto tipato invece di un silenzio, la
somma dei residui e' un censimento di che cosa parrot0 non sa ancora leggere,
ordinabile per frequenza e per quanti consumer sblocca.

**Predizione falsificabile.** Su un corpus reale, i residui devono raggrupparsi
in poche classi ricorrenti (coordinazione, modalita', negazione, riferimento) e
insegnare **una** di quelle classi deve abbassare la frequenza dell'intera
classe, non del solo esempio. Se i residui restano una polvere di casi unici,
l'ipotesi e' falsa e la lettura non e' compositiva.

#### 18.20.3 Ipotesi D15 — ogni conoscenza apre per costruzione la propria porta interrogativa

Il «buco del consumatore» (gen306) e' stato chiuso tre volte a tre livelli
diversi: il verbo di relazione, il frame ternario, e ora la claim riportata.
Tre volte la stessa forma: cio' che si puo' dire deve poter essere chiesto, e la
forma della domanda si **deriva** dalla forma dell'asserzione.

```prolog
assertion_form($Relation, $Pattern).
interrogative_transform($Language, $Pattern, $QuestionPattern).
interrogative_closure($Relation, open | missing($Reason)).
```

L'ipotesi e' che la derivazione sia un'operazione di lingua — spostamento del
gap, caduta del complementatore, inversione dell'ausiliare — e quindi conoscenza
KB, non un consumer per classe.

**Predizione falsificabile.** Una metrica `InterrogativeClosure` sul totale
delle relazioni insegnabili deve poter salire aggiungendo **trasformazioni**, non
consumer. Se chiudere una nuova classe richiede sempre un modulo, la derivazione
non esiste e la chiusura e' un aneddoto ripetuto.

#### 18.20.4 Ipotesi D16 — comprendere e' collocare in una regione, non tradurre in un punto

D6 introduce modelli e contromodelli. L'ipotesi D16 e' piu' forte e piu'
scomoda: un testo non denota **un** modello, denota la **regione** dei modelli
compatibili con esso, e la precisione della comprensione e' l'ampiezza di quella
regione. Un testo vago non e' un testo mal letto: e' un testo che vincola poco.

```prolog
reading_region($Text, $ConstraintSet).
region_refines($RegionA, $RegionB).
region_incomparable($RegionA, $RegionB).
constraint_narrows($Constraint, $Region, $Narrower).
question_gain($Question, $Region, $ExpectedNarrowing).
region_empty($Region, $Contradiction).
```

Da qui discende una definizione **operativa** di buona domanda, che oggi manca:
la domanda migliore non e' quella che nomina il gap piu' grande, e' quella che
restringe di piu' la regione. E una definizione operativa di contraddizione: la
regione vuota, cioe' nessun modello soddisfa insieme i vincoli del testo.

**Predizione falsificabile.** Dati due chiarimenti possibili su uno stesso
paragrafo ambiguo, il sistema deve scegliere quello con `question_gain`
maggiore e la scelta deve coincidere con quella di un lettore esperto in una
maggioranza di casi misurati. Se la scelta e' indistinguibile dal caso, la
nozione di regione non sta facendo lavoro.

**Rapporto con la latenza.** La regione non va enumerata. Si rappresenta per
vincoli e si confronta per raffinamento; l'enumerazione resta bounded e il suo
esito insufficiente e' `incomplete`, mai «nessun modello».

#### 18.20.5 Ipotesi D17 — il testo ha un mittente, e il mittente e' modellabile

D1-D12 modellano il documento. Ma la prosa complessa e' scritta da un agente con
un obiettivo: convincere, cautelarsi, anticipare un'obiezione, concedere il
minimo necessario. Molte strutture che sembrano rumore retorico sono **mosse**.

```prolog
author_goal($Document, $Goal).
concession($Document, $Claim, $ConcededTo).
hedge_purpose($Span, $Purpose).          % scope limitato | incertezza | cortesia | difesa
anticipated_objection($Document, $Objection, $Response).
emphasis_asymmetry($Document, $Claim, $Support).
```

**Predizione falsificabile.** Su un articolo held-out il sistema deve poter
rispondere a «che cosa vuole farmi credere questo testo, e che cosa concede?»
senza produrre un riassunto, e la sua risposta deve cambiare se si ritrae la
classe di una singola locuzione concessiva. Se cambia solo la lunghezza
dell'output, il modello del mittente non esiste.

**Guardia anti-inganno.** Modellare l'intenzione dell'autore non autorizza a
dichiararla provata. `author_goal` e' una lettura attribuita come qualunque
altra claim, sotto la disciplina D2 e D13.

#### 18.20.6 Ipotesi D18 — una procedura e' un riferimento risolvibile, non un blocco chiuso

D5 tratta la procedura come proof obligation. In letteratura scientifica reale
un metodo e' quasi sempre **composizione per riferimento**: «seguendo il
protocollo di [12], con la modifica seguente». Una procedura che finge di essere
completa e' un misclaim sull'eseguibilita'.

```prolog
procedure_step($Procedure, $Index, $Step).
procedure_reference($Step, $Source).
reference_resolution($Source, resolved($Procedure) | unavailable($Reason)).
step_modification($Step, $Base, $Delta).
procedure_executable($Procedure, yes | blocked($Step)).
```

**Predizione falsificabile.** Su un metodo che delega, il sistema deve dire
**quale passo** delega e a quale fonte, e deve dichiararsi `blocked` invece di
eseguire. Fornendo la fonte mancante, lo stesso metodo deve diventare
eseguibile senza reinsegnare i passi gia' noti.

#### 18.20.7 Ipotesi D19 — la comprensione e' un reticolo monotono con livelli nominabili

SC2-B ha dovuto tenere separate quattro metriche (superficie, normalizzata,
parziale rifiutata, precisione di status) perche' confonderle avrebbe scambiato
un buon envelope per comprensione. L'ipotesi e' che quella separazione sia
strutturale e vada resa un **tipo**: ogni livello e' nominabile, i livelli sono
ordinati, e ogni consumer dichiara il livello minimo che gli serve.

```prolog
comprehension_level($Level, $Rank).      % observed < surface < normalized < grounded < modelled
reading_level($Reading, $Level).
consumer_requires($Consumer, $Level).
level_sufficient($Reading, $Consumer).
level_downgrade($Reading, $Level, $Reason).
```

Un consumer che pretende `normalized` deve declinare su `surface`, e la
declinazione deve nominare il livello mancante. La monotonia e' il vincolo che
rende il reticolo utile: ritrarre conoscenza puo' abbassare un livello, mai
alzarlo, e abbassare un livello deve spegnere esattamente i consumer che lo
pretendevano.

**Predizione falsificabile.** Introducendo un consumer nuovo, deve bastare
dichiarare il suo livello minimo perche' rifiuti automaticamente gli input
insufficienti. Se serve una guardia scritta a mano per ogni consumer, il
reticolo e' decorativo.

#### 18.20.8 Ipotesi D20 — un gap ricorrente e' una richiesta di lezione

Chiusura del cerchio con D9, D10, D14 e con il protocollo `needhelp`. Se i
residui sono oggetti tipati (D14) e i livelli sono nominabili (D19), allora
parrot0 puo' fare l'unica cosa che oggi fa soltanto il teacher: **accorgersi di
che cosa gli manca e chiedere la lezione giusta, in lingua naturale**.

```prolog
gap_frequency($Kind, $Count, $Window).
gap_fanout($Kind, $BlockedConsumers).
lesson_request($Kind, $NaturalFormulation, $ExampleSpan).
lesson_accepted($Request, $TaughtForm).
lesson_effect($Request, $BeforeCoverage, $AfterCoverage).
```

Il punto non e' che parrot0 chieda aiuto: e' che formuli **la lezione**, con un
esempio reale preso dal proprio residuo, cosi' che il teacher debba solo
confermarla o correggerla. E che l'effetto della lezione sia misurato sulla
copertura, non sul singolo prompt.

**Predizione falsificabile.** Dopo la lettura di un corpus reale, la lezione che
parrot0 chiede per prima deve essere quella che, insegnata, produce
l'incremento di copertura maggiore fra quelle candidate. Se l'ordine delle sue
richieste e' scorrelato dal guadagno misurato, non sta metacomprendendo: sta
elencando errori.

**Guardia.** Una lezione richiesta non e' una lezione ricevuta. `lesson_request`
non autorizza nessuna auto-promozione: resta una domanda finche' un teacher non
risponde parlando, sotto `LEARN_PROTOCOL.md`.

### 18.21 Ordine sperimentale esteso e gate

Le otto ipotesi non hanno pari dipendenza. L'ordine e' obbligato:

1. **D13 e D14 per primi.** Sono gia' meta' costruiti da SC2-B e sono
   precondizione di tutto il resto: senza origine esplicita non si puo' leggere
   senza credere, e senza residuo tipato non c'e' niente da contare.
2. **D19 subito dopo.** Nominare i livelli e' cio' che impedisce a D16-D18 di
   dichiarare comprensione quando hanno soltanto superficie.
3. **D15.** La chiusura interrogativa e' l'unico modo di *misurare* se un
   livello nuovo e' davvero utilizzabile e non soltanto memorizzato.
4. **D16.** La regione e il guadagno di una domanda; poggia su D6 e su D19.
5. **D17 e D18** in parallelo: mittente e procedure per riferimento sono
   indipendenti fra loro e dipendono da D2/D5 gia' esistenti.
6. **D20 per ultimo**, perche' e' l'unica che chiude su se stessa: richiede
   residui (D14), livelli (D19) e regione (D16) per poter ordinare le proprie
   richieste per guadagno.

Gate aggiuntivi rispetto al §18.15, tutti su documenti mai usati per costruirli:

13. nessuna lettura produce conoscenza del mondo se la sua origine non lo
    autorizza, e ritrarre l'origine spegne esattamente cio' che ne dipendeva;
14. ogni frase non compresa lascia un residuo tipato e localizzabile, e i
    residui si raggruppano in classi che una lezione sola riduce;
15. una relazione insegnabile e' interrogabile per derivazione, non per modulo;
16. fra due chiarimenti il sistema sceglie quello che restringe di piu', e sa
    dire perche';
17. tesi e concessioni dell'autore sono distinte dalle proposizioni del testo;
18. una procedura che delega si dichiara bloccata e nomina il passo;
19. nessuna risposta e' costruita a un livello di comprensione piu' alto di
    quello effettivamente raggiunto;
20. la prima lezione richiesta da parrot0 e' quella con il guadagno di copertura
    maggiore, e il guadagno viene misurato dopo.

Il non-obiettivo resta quello del §18.15, rafforzato: non si cerca un lettore
che *sembri* capire di piu'. Si cerca un lettore che sappia **dove finisce** cio'
che ha capito, e che sappia chiedere la frase esatta che lo farebbe finire piu'
in la'.

### 18.22 Evidenza SC2-C/SC2-D — due muri caduti, due nuovi confini nominati

I cicli SC2-C e SC2-D del 2026-08-29
([SC2-C](../labs/apprendimento-assistito/2026-08-29-supercomprensione-sc2c.md),
[SC2-D](../labs/apprendimento-assistito/2026-08-29-supercomprensione-sc2d.md))
hanno chiuso l'equivalenza attivo/passivo, il residuo tipato, la prova
pronunciata e la modalita'. Tre risultati meritano di stare nel piano perche'
cambiano il modello, non soltanto il punteggio.

**Il misclaim che non somiglia a un errore.** Prima di SC2-D, «a kinetic
impactor CAN shorten an asteroid orbit» produceva
`shorten(kinetic_impactor_can, asteroid_orbit)`. Il modale non faceva fallire la
lettura: veniva inghiottito nel sintagma del soggetto, e la claim usciva come se
il testo affermasse che la cosa succede, attribuita a un ente inesistente.
Questo e' peggio di un muro e peggio di un gap, e SC2-C lo aveva classificato
come residuo soltanto perche' nessun verbo di quella famiglia era insegnato.
**Un residuo tipato puo' nascondere un misclaim latente**: appena la KB cresce,
la stessa frase smette di fermarsi e comincia a mentire. Corollario operativo:
ogni classe di residuo va riesaminata quando la KB che la circonda cresce.

**Il passivo non era un problema di superficie.** Non si e' potuto leggere
finche' il legatore di schemi buttava via la lettera dopo `@`, cioe' finche'
l'ordine degli argomenti era l'ordine delle parole. Lo stesso bug faceva
produrre silenziosamente fatti scambiati alla costruzione spedita
`construction_frame("@O is home to @S", …)`. Una capacita' che sembrava
linguistica era una coordinata mancante nella rappresentazione.

**La derivazione ha un costo, e il costo ha cambiato il confine.** Derivare la
radice verbale (`improved` -> `improve`) e' la mossa KB-first giusta e ha portato
un turno di inferenza da meno di un secondo a 1,85 s, sopra il budget. Non per il
numero di schemi: per il costo di **ricostruirli** a ogni turno. La forma nuda di
un verbo al passato e' quindi tornata a essere una lezione. E' la prima volta in
questa serie che un limite di performance decide dove passa il confine fra
derivare e insegnare, e non e' un dettaglio di implementazione: e' l'ipotesi D21.

### 18.23 Ipotesi D21 — dove una conoscenza viene calcolata e' conoscenza

Il piano ha sempre trattato «derivare» come strettamente migliore di
«memorizzare»: una regola che genera cento schemi vale piu' di cento fatti,
perche' il centounesimo arriva gratis. SC2-D ha misurato il rovescio. Una regola
che ricostruisce le proprie conclusioni **dentro un'enumerazione calda** paga il
costo a ogni turno, e oltre una soglia la capacita' esiste ma non e'
utilizzabile — che, per un sistema conversazionale, e' indistinguibile dal non
averla.

L'ipotesi e' che il punto di calcolo debba diventare una coordinata dichiarata,
non una conseguenza accidentale di come una regola e' stata scritta:

```prolog
derivation_policy($Relation, recomputed | materialized_on_learn | materialized_on_boot).
derivation_cost($Relation, $Observed).
derivation_trigger($Relation, $Event).      % quale atto rimaterializza
materialized_from($Fact, $Rule, $Source).   % genealogia, per poter ritrattare
stale_derivation($Relation, $Reason).
```

Il vincolo che rende l'ipotesi non banale: **materializzare non deve rompere la
ritrattabilita'**. Un fatto materializzato porta la genealogia della regola che
lo ha prodotto, quindi ritrarre la regola o la sua premessa lo elimina — se no si
e' comprata latenza vendendo la proprieta' piu' importante del sistema.

**Predizione falsificabile.** Spostare una relazione da `recomputed` a
`materialized_on_learn` deve (a) riportare il turno dentro il budget, (b) non
cambiare nessuna risposta, (c) conservare l'ablation: ritrarre la premessa deve
spegnere esattamente le stesse conclusioni di prima. Se una delle tre non vale,
la materializzazione non e' una politica ma un'ottimizzazione, e va rifiutata.

**Non-obiettivo.** Non e' una cache. Una cache e' invisibile alla conoscenza e si
invalida da sola; qui il punto e' che *dove* una conclusione vive sia una
decisione ispezionabile e correggibile come tutte le altre.

### 18.24 Ipotesi D22 — una collisione di dispatch e' un fatto osservabile

> ⚠️ **Premessa corretta il 2026-08-29, vedi §18.27.** La prima stesura diceva
> che tre lezioni erano state intercettate da moduli precedenti. Era falso, e lo
> era perche' il furto era stato **dedotto dall'esito** invece che chiesto:
> `who answered?` funziona da sempre, e risponde. Dei cinque casi raccolti, uno
> solo era un furto vero (`claim` -> `missing_referent`); tre erano gap
> uso/menzione con il rimedio gia' proposto da parrot0 e funzionante, e uno era
> una perdita dentro la citazione. L'ipotesi resta valida ma cambia oggetto: il
> **vincitore** e' gia' nominabile, cio' che manca e' **perche'** ha vinto e chi
> e' stato scavalcato.

Il caso reale:

```text
was it hypothesized that a causal CLAIM must survive confounding?
  -> «I don't have the claim you mean»
who answered?
  -> «The 'missing_referent' module answered your last question.»
```

Dall'esterno la prima riga e' indistinguibile da «non lo so». Dall'interno e'
un'altra cosa: **qualcuno ha risposto al posto di chi doveva** — e la seconda
riga lo dice gia', ma solo se qualcuno pensa a chiederlo. Il dispatch a primo-match
e' deliberato e va conservato (`PRINCIPLES.md`, corollario sulle strutture
secondarie), ma oggi e' l'unico strato del sistema che non lascia traccia.

```prolog
turn_claimed_by($Turn, $Module, $Evidence).
turn_declined_by($Turn, $Module, $Reason).
dispatch_collision($Turn, $Winner, $Contender, $Overlap).
collision_frequency($Winner, $Contender, $Count).
collision_remedy($Collision, $Guard).
```

L'ipotesi e' che la collisione sia **tipizzabile come un residuo** (D14): non un
bug da inseguire uno alla volta, ma una classe da contare e ordinare. Il mantra
#14 chiede gia' che ogni collisione diventi una guardia insegnabile; qui si
aggiunge il gradino che manca — per scrivere la guardia bisogna prima poter
**vedere** la collisione, e oggi la si scopre solo per caso, isolandola a mano.

**Predizione falsificabile.** Registrando chi ha preso ogni turno, le collisioni
osservate su un corpus reale devono concentrarsi su poche coppie di moduli e su
poche parole comuni. Se sono distribuite uniformemente, non c'e' una classe da
motorizzare e l'ipotesi e' falsa. E il segnale forte: parrot0 deve poter
rispondere «quel turno l'ha preso un altro modulo per via di questa parola»
invece di «non lo so», che e' l'unica risposta che oggi sa dare.

**Guardia.** Osservare il dispatch non autorizza a riordinarlo automaticamente.
Il riordino resta una decisione umana o una guardia insegnata; la traccia serve a
renderla informata, non a sostituirla.

### 18.25 Ipotesi D23 — un supporto congiunto non e' due supporti

SC3 ha chiuso l'arco di supporto singolo e ha lasciato in piedi, misurato, il
limite che il §18.15 gate 3 chiama per nome: *«i supporti congiunti non vengono
indeboliti in regole indipendenti»*. Oggi due premesse per una conclusione
producono due archi indipendenti, e questo **rafforza** la conclusione invece di
condizionarla: togliendone una, l'altra continua a sostenerla da sola.

E' il duale esatto della disciplina che SC3 ha guadagnato. SC3 garantisce che
togliere una premessa spenga cio' che ne dipendeva; D23 chiede che togliere
**una parte** di una premessa congiunta spenga la conclusione **intera**.

```prolog
support_set($Conclusion, $SetId).
support_member($SetId, $Premise).
support_mode($SetId, joint | independent).
joint_support_live($SetId) :- naf(support_member_dead($SetId)).
support_member_dead($SetId) :- support_member($SetId, $P), naf(claim_status($P, $S)).
conclusion_support_state($Conclusion, live | weakened($Missing) | lost).
```

La distinzione fra `joint` e `independent` non e' deducibile dalla superficie in
generale — «A. B. Therefore C.» puo' essere entrambe — ed e' quindi
un'ambiguita' da conservare, non da risolvere scegliendo. La lettura giusta:
finche' il modo non e' determinato, la conclusione ha **due stati candidati**, e
il sistema deve saper dire quale osservazione o quale lezione lo deciderebbe.

**Predizione falsificabile.** Su un argomento a due premesse dichiarato
congiunto, ritrarre una qualunque delle due deve portare la conclusione a
`lost`; sullo stesso argomento dichiarato indipendente, deve portarla a
`weakened` nominando la premessa caduta. Se i due modi producono lo stesso
comportamento, la distinzione non sta facendo lavoro e l'ipotesi e' falsa.

**Perche' e' urgente.** Un lettore che rafforza per errore e' peggio di uno che
non collega: la conclusione sopravvive a meta' delle sue ragioni e nessuno se ne
accorge. E' la stessa famiglia del misclaim modale di SC2-D — non un muro, non
un gap, ma un'affermazione piu' forte della sua fonte.

### 18.26 Ipotesi D24 — la citazione e' esente dalla lingua che la contiene

Quattro volte in questa serie una lezione e' fallita perche' una **parola
comune** dentro il turno apparteneva a qualcun altro: `claim`, `shall`, `known`,
`ground`. E una quinta volta per un motivo diverso e piu' profondo: la menzione
quotata `"what is that based on"` e' entrata nella KB come
`claim_support_question("what based on")` — la canonicalizzazione della domanda
ha tolto parole **dentro le virgolette**.

Le due famiglie sembrano una sola («qualcosa mangia il turno») e non lo sono. La
prima e' dispatch (D22). La seconda e' un principio linguistico che il sistema
dichiara e non applica fino in fondo:

> cio' che e' **menzionato** non partecipa alla lingua che lo menziona.

`canonicalization_exempt(mention)` esiste gia' in `kb/core/input.p0` dal lavoro
sull'input universale. Non raggiunge il percorso della lezione quotata, e il
risultato e' un muro sull'addestrabilita': **non si puo' insegnare una forma
interrogativa che contenga una copula**, perche' la copula viene consumata dal
riconoscitore di domande prima che la lezione la veda.

```prolog
mention_span($Turn, range($Start, $Length)).
exempt_from($Transform, $Span).
transform_applied($Turn, $Transform, $Span).
mention_fidelity($Taught, $Original, exact | lossy($Missing)).
```

**Predizione falsificabile.** Insegnando dieci locuzioni quotate che contengono
copule, articoli, dimostrativi e ausiliari, tutte e dieci devono entrare nella
KB **byte per byte** come pronunciate, e il retract deve trovarle con la stessa
superficie. Se anche una sola perde una parola, l'esenzione non e' un principio
del sistema ma una guardia locale, e ogni percorso nuovo dovra' riscoprirla.

**Corollario di metodo.** `mention_fidelity/3` non serve solo a passare il test:
serve perche' oggi la perdita e' **silenziosa**. Il turno dice `Learned:` e
mostra la forma mutila, e chi insegna deve accorgersene leggendo. Una lezione che
non e' entrata come e' stata detta deve essere un errore dichiarato, non una
riga da confrontare a occhio.

### 18.27 Il primo frutto dell'autocorrezione, e la correzione di un'ipotesi

> **Nota di F., 2026-08-29.** I turni rubati che continuano a emergere durante
> l'addestramento dicono che il motore di comprensione deve crescere in **due
> direzioni**: la **puntualita'** e il **contesto**. E il punto di partenza non
> e' vuoto: bisogna guardare dove la KB gia' distingue contesti, registri e
> terminologia — il registro scacchistico FSI contro quello informale e' il caso
> di scuola.

Questa sezione fa quell'analisi. Ma prima deve correggere un errore, perche' la
correzione **e' il risultato**.

#### La scoperta: il muro non era un muro

Nella serie SC2-C/SC2-D/SC3 tre lezioni lessicali sono state riportate come
«muro, gap conservato»:

```text
shall is a necessity marker        -> muro
known is an irregular participle   -> muro
because is a ground connector      -> muro
```

Non erano muri. Riproposte **menzionando** la parola invece di usarla, passano
tutte e tre al primo colpo:

```text
the word shall is a necessity marker   -> Learned: necessity_marker(shall).
"known" is an irregular participle     -> Learned: irregular_participle(known).
the word because is a ground connector -> Learned: ground_connector(because).
```

E il punto che conta: **la forma che funziona e' esattamente quella che parrot0
proponeva dentro il muro**.

```text
> shall is a necessity marker
parrot0> Hmm, I don't know about necessity yet. Want me to learn about it? Or
         teach me: if it is a kind of thing, say «something is a necessity»;
         if necessity is a word you want to talk ABOUT, say «the word
         necessity is a …».
```

Parrot0 aveva **tipizzato correttamente il proprio arresto** (una parola di cui
si sta parlando, non una che si sta usando), aveva **proposto il rimedio
giusto**, e il rimedio **funziona**. Il fallimento e' stato del teacher, che ha
letto quella frase come un rifiuto invece che come una proposta e ha archiviato
il caso come gap.

Questo e' il primo frutto misurabile del lavoro su **autocorrezione e
tipizzazione degli arresti** (`kb/core/arrests.p0`, `kb/core/gap-kinds.p0`, e la
mossa che `kb/learning/taught-lexicon.p0` documenta dal gen429): il sistema non
si e' limitato a fallire, ha nominato la specie del proprio arresto e ha detto
come ripararlo. Vale la pena scriverlo per esteso perche' e' il comportamento
che il §6 del piano di apprendimento assistito chiede come condizione di
missione — *«davanti a un fallimento nuovo, parrot0 sa indicare il tipo di
lacuna, ricevere una spiegazione, trasformarla in una capacita' generale»* — e
qui e' successo senza che nessuno lo stesse cercando.

Corollario di metodo, sgradevole e necessario: **un rimedio proposto e non
provato e' un gap che non esiste**. La checklist del `LEARN_PROTOCOL` §6.3
chiede di classificare il gap; da qui in avanti deve chiedere anche di
**eseguire il rimedio che parrot0 propone** prima di dichiararlo tale.

#### La tabella corretta dei «turni rubati»

Chiedendo a parrot0 `who answered?` dopo ciascun caso — cosa che sa fare da
sempre (`intent_cue(…, "who answered")`, `turn_module/2`) e che nessuno aveva
chiesto:

| turno | chi ha risposto | che cosa era davvero |
|---|---|---|
| «…a causal **claim** must survive confounding?» | `missing_referent` | **furto vero**: un modulo aggancia una parola che nel registro scientifico e' terminologia |
| «"what is that based on" is a …» | `mention` — quello **giusto** | perdita **dentro** la citazione: la canonicalizzazione consuma parole fra virgolette |
| «**shall** is a necessity marker» | nessuno, fallback | uso/menzione, con rimedio proposto e funzionante |
| «**known** is an irregular participle» | nessuno, fallback | idem |
| «**because** is a ground connector» | nessuno, fallback | idem |

Quindi **D22 va corretto**: la sua premessa («tre lezioni sono state
intercettate da moduli precedenti») era sbagliata, e lo era perche' avevo
dedotto il furto dall'esito invece di chiederlo. Il vincitore del dispatch e'
gia' nominabile. Cio' che manca a D22 non e' *chi* ha risposto: e' **perche'** —
quale superficie gli ha dato titolo — e se qualcuno di piu' adatto sia stato
scavalcato. L'ipotesi resta, con la premessa riscritta e il campione ridotto a
**un** caso reale su cinque.

I tre falsi allarmi non sono pero' rumore: sono tutti la stessa cosa, ed e' la
cosa che segue.

#### Un solo principio sotto tre meccanismi

```text
claim        -> un MODULO usa la parola come termine del proprio registro
shall/known  -> il PARSER usa la parola per il suo ruolo grammaticale
"what is …"  -> la CANONICALIZZAZIONE usa le parole dentro le virgolette
```

Tre strati diversi, un solo principio violato:

> cio' che e' **menzionato** non partecipa alla lingua che lo menziona.

Il sistema conosce gia' questo principio — `canonicalization_exempt(mention)` e'
in `kb/core/input.p0`, `segment_role(mention, …)` esiste, `mod_mention` corre
prima del lettore universale proprio per questo — ma lo applica **localmente**,
una guardia per percorso. Ogni percorso nuovo dovra' riscoprirlo. Vedi D25.

#### Che cosa la KB gia' distingue (l'inventario chiesto da F.)

L'analisi mostra un'asimmetria netta: **il registro governa cio' che parrot0
DICE, non cio' che CAPISCE.**

*Lato uscita — maturo:*

| meccanismo | dove | che cosa distingue |
|---|---|---|
| `concept_label(Concept, Lang, Register, Form)` | `kb/core/language-forms.p0` | lo stesso concetto ha nomi diversi per registro |
| `register_value(R, Dimension, Value)` | `kb/core/register.p0` | il registro e' un **punto in uno spazio di dimensioni** (`technicality`, `formality`), non un'etichetta |
| `register_cue/3`, `register_request/3`, `register_commit/2` | `kb/core/register.p0` | l'interlocutore puo' **chiedere** un registro, e la richiesta si ritratta |
| `denotation_register/4`, `form_denotes/4` | `kb/core/denotation.p0` | una forma denota un concetto **solo dentro un dominio** |
| `concept_in_domain/2`, `domain_category/2` | `kb/core/denotation.p0` | la terminologia appartiene a domini |

Il caso di scuola di F. e' letteralmente nella KB:

```prolog
register_value(fsi, technicality, high).   register_value(common, technicality, low).
concept_label(queen, it, fsi, donna).      % kb/experts/games/chess.p0
concept_label(queen, it, common, regina).
```

parrot0 **sa** che «donna» e «regina» sono la stessa cosa in due registri, e sa
che `fsi` e' il registro tecnico. Questa e' esattamente la struttura che
servirebbe per non farsi rubare `claim`.

*Lato ingresso — presente ma di un'altra specie:*

| meccanismo | dove | che cosa distingue |
|---|---|---|
| `register_evidence/2` + scorer universale | `kb/core/input.p0` | il registro di **formato**: C, Python, JSON, diff, log, prosa, quoted |
| `segment_role/2` + `faculty_for/2` | `kb/core/input.p0` | il **ruolo** di uno span sceglie la facolta' che lo consuma |
| `canonicalization_exempt/1` | `kb/core/input.p0` | un ruolo puo' essere **esente** da una trasformazione |
| `turn_register/2`, `turn_module/2` | asseriti per turno | quale registro e quale facolta' hanno vinto |
| `turn_arrest(…, wrong_suspect, arrest(guard($Module), …))` | `kb/core/arrests.p0` | esiste gia' la specie «ha risposto il modulo sbagliato» |

**Il buco, detto in una riga.** `register_evidence` decide se un input e' *codice
o prosa*; `concept_label` decide se dire *donna o regina*. Nessuno decide **in
quale registro terminologico stia la prosa in arrivo** — e quindi nessuno puo'
dire che in un articolo di metodologia scientifica `claim` e' un sostantivo
tecnico e non l'invito ad aprire una verifica aritmetica. Le due meta' non si
incontrano mai, e i due assi che F. nomina sono esattamente i due modi di farle
incontrare: il **contesto** (D26) e la **puntualita'** (D27).

### 18.28 Ipotesi D25 — uso e menzione sono un invariante, non una guardia locale

Il principio e' gia' scritto in tre punti del sistema e applicato in nessuno
come invariante. L'ipotesi e' che debba diventare una proprieta' dell'**atto**,
non del percorso: uno span dichiarato menzione e' esente da **ogni**
trasformazione che riscriva le sue parole, e la fedelta' e' verificabile.

```prolog
mention_span($Turn, range($Start, $Length)).
exempt_from($Transform, $Role).
transform_applied($Turn, $Transform, range($Start, $Length)).
mention_fidelity($Taught, $Spoken, exact | lossy($Missing)).
lesson_repair_offered($Gap, $Form).      % il rimedio che il muro propone
lesson_repair_confirmed($Gap, $Form).    % il rimedio che ha funzionato
```

Le ultime due righe sono il pezzo che questa sezione ha reso obbligatorio: se
parrot0 propone un rimedio, il sistema deve poter registrare **se qualcuno lo ha
provato e com'e' andato**. Un rimedio proposto, funzionante e mai eseguito e'
conoscenza che il progetto sta buttando via a ogni sessione.

**Predizione falsificabile.** Dieci locuzioni quotate contenenti copule,
ausiliari, articoli e dimostrativi entrano nella KB **byte per byte** e si
ritrattano con la stessa superficie; e ogni lezione lessicale che fallisce nella
forma d'uso riesce nella forma di menzione proposta dal muro. Se anche una sola
perde una parola in silenzio, l'esenzione e' una guardia e non un invariante.

**Corollario misurabile.** `mention_fidelity/3` non serve a passare un test:
serve perche' oggi la perdita e' **muta**. Il turno dice `Learned:` e mostra la
forma mutila, e chi insegna se ne accorge solo rileggendo. Una lezione che non e'
entrata come e' stata detta deve essere un errore dichiarato.

### 18.29 Ipotesi D26 — il registro terminologico decide chi ha titolo (asse: contesto)

`register_evidence/2` sceglie gia' un registro di formato con lo scorer
universale, e `faculty_for/2` lega gia' un ruolo a una facolta'. L'ipotesi e'
che la stessa coppia di meccanismi, con vocabolario diverso, risolva la classe
dei furti: un turno o uno span ha un **registro terminologico**, e una facolta'
dichiara quali registri serve.

```prolog
terminology_register($Register, $Domain).
register_term($Register, $Surface, $Concept).      % «claim» in metodologia
turn_terminology($Turn, $Register, $Evidence).
faculty_serves($Faculty, $Register).
faculty_declines($Faculty, $Turn, foreign_register($Register)).
```

Il vocabolario non va inventato: `concept_in_domain/2` e `denotation_register/4`
lo producono gia' dai domini caricati, e `concept_label/4` porta il registro. Un
esperto che carica il proprio profilo carica **con esso** la propria terminologia,
esattamente come gli scacchi caricano `fsi`.

**Predizione falsificabile.** Dichiarando che un modulo serve il registro
`arithmetic_verification` e che la prosa scientifica e' nel registro
`research_methodology`, la frase «a causal claim must survive confounding» deve
smettere di essere presa da quel modulo **senza toccarne il codice e senza
toccare l'ordine del registry**. Se serve spostare il modulo nella lista, il
registro non sta decidendo e l'ipotesi e' falsa.

**Guardia.** Un registro non e' un permesso di rispondere: e' un titolo a
provarci. La precedenza a primo-match resta, e una facolta' senza registro
dichiarato continua a vedere tutto — altrimenti il livello diventa un filtro
distruttivo invece che additivo (`PRINCIPLES.md`, corollario sulle strutture
secondarie).

### 18.30 Ipotesi D27 — la puntualita' e' la cue dentro il ruolo (asse: precisione)

Il mantra #8 avverte da tempo che `cue()` e' substring e che «eat» sta dentro
«f-EAT-hers». La serie SC2/SC3 mostra la stessa specie un piano piu' su: una cue
combacia con una **parola vera**, ma con una parola che sta in un altro ruolo
del turno. «claim» dentro il soggetto di una proposizione riportata non e' una
richiesta di verificare una claim; «that» dentro le virgolette non e' il
complementatore della frase che le contiene.

L'input universale ha gia' i ruoli (`input_node_role/3`, `segment_role/2`,
`turn_span/4`). Cio' che manca e' che il **matching delle cue li rispetti**.

```prolog
cue_scope($Relation, whole_turn | role($Role) | outside_role($Role)).
cue_match_in($Turn, $Relation, $Cue, $Role, range($Start, $Length)).
cue_blocked($Turn, $Relation, $Cue, $Role).
```

**Predizione falsificabile.** Dichiarando `cue_scope($R, outside_role(mention))`
per le relazioni di intento, nessuna cue deve piu' accendersi dentro una
citazione, e il numero di lezioni quotate che falliscono deve andare a zero
senza che nessuna cue venga riscritta. Se per farlo bisogna modificare le cue,
il problema non era lo scope.

**Rapporto con D26.** Le due ipotesi sono ortogonali e si compongono: D26 dice
*quali* facolta' hanno titolo su questo turno, D27 dice *dove* dentro il turno
una cue puo' guardare. Una sola delle due lascia meta' della classe aperta —
ed e' esattamente la ragione per cui F. le ha nominate insieme.

### 18.31 Insegnare una cue parlando: dove funziona, e dove la strada dei moduli finisce

> **Nota di F., 2026-08-29.** «Tramite interlocuzione dovrebbe essere possibile
> alterare i cue dei vari moduli in modo naturale — *"quando ti dico helyla, ti
> sto salutando"* — e questo dovrebbe agire sul modulo sociale. Al tempo stesso:
> **lavorare sui moduli non deve essere il come parrot0 migliora la sua
> comprensione.** La strada principale e' quella di universal-input e universal
> comprehension, cioe' questo documento.»

Le due meta' della nota non sono in tensione: sono la stessa misura vista da due
lati, e misurarla ha prodotto tre risultati, uno buono e due gravi.

#### 1. L'atto didattico esiste gia' e funziona

```text
> helyla friend
< I have nothing on helyla …
> learn "helyla friend" as a casual opener
< Got it - I'll take «helyla friend» as a way to a casual opener now.
> helyla friend
< Hey! I'm here. Ask me something, or tell me about your day?
> who answered?
< The 'chitchat' module answered your last question.
```

Nessuna ricompilazione, effetto dal turno successivo. Il registro `learnable/3`
(`kb/core/intents.p0`) dichiara **oltre cento** etichette naturali di questo
tipo, e il teacher deve conoscere l'etichetta, non il predicato: «a casual
opener», «a sign of tiredness», «another way to ask», «un altro modo per
introdurre». Questo pezzo del desiderio di F. e' gia' vero.

#### 2. Ma un token solo non ottiene mai titolo

```text
> learn "helyla" as a casual opener
< Got it - I'll take «helyla» as a way to a casual opener now.
!query intent_cue(casual, "helyla")        -> VERO, la cue e' viva
> helyla
< I have nothing on helyla — could you say what you mean by it?
> who answered?
< The 'lone' module answered your last question.
```

La lezione e' entrata, la cue e' viva, e non conta niente: `mod_lone` classifica
un turno di un token solo **prima** del sociale, per una ragione strutturale
documentata nel registry al gen426. Nessuno ha sbagliato — e proprio per questo
il caso e' istruttivo.

La tentazione, qui, e' spostare `lone` piu' in basso. E' la strada dei moduli, e
non e' una strada: la parola successiva incontrerebbe il modulo successivo, e si
pagherebbe la stessa tassa un'altra volta. La cura giusta e' che il **ruolo del
turno** decida chi ha titolo (D26) e che una cue guardi **dentro il proprio
ruolo** (D27) — cioe' esattamente universal-input.

#### 3. E la cue insegnata non e' ritrattabile: si mangia la frase che la ritratta

Questo e' il risultato che rende la nota di F. non un'opinione di stile ma un
vincolo di rilascio. `AGENTS.md` chiede, come condizione di conformita' KB-first:

> asserting a new cue must change recognition without rebuilding, and
> **retracting/ablating the cue must remove that recognition**.

Per questa classe la seconda meta' **non e' esprimibile**:

```text
> forget "helyla friend" as a casual opener
< Hey! I'm here. …            ← la cue si e' accesa dentro il proprio retract
> stop treating "helyla friend" as a casual opener
< Hey! I'm here. …            ← idem
> forget that the phrase "helyla friend" is a casual opener
< Learned: casual_opener(forget).   ← un fatto FALSO sul verbo stesso
```

Ogni frase che **nomina** la locuzione la **contiene**, e una cue `substring` non
distingue le due cose. L'unica frase che sfugge alla cue finisce per insegnare
che «forget» e' un attacco informale — la stessa specie di
`hypothesis_report_marker(forget)` che SC2-A aveva gia' dovuto guardare a mano su
un altro percorso, riaperta qui perche' quella guardia e' **locale a quel
percorso** invece di essere un invariante.

Tre percorsi, tre volte lo stesso principio violato, ed e' quello del §18.27:
**cio' che e' menzionato non partecipa alla lingua che lo menziona.** La retract
non e' un caso d'angolo del vocabolario: e' il posto dove la sua assenza rende
il sistema non conforme al proprio contratto.

#### La scala della cue, per moduli sempre piu' complessi

F. chiede di allargare l'esempio ai moduli complessi. Allargandolo si vede che
«cue» significa cose diverse man mano che si sale, e che il progetto le ha gia'
rese insegnabili quasi tutte:

| gradino | che cos'e' la «cue» | relazione | insegnabile oggi |
|---|---|---|---|
| 1 | superficie -> **intento** | `intent_cue/2`, `intent_phrase/2` | si' (gen211/213) |
| 2 | superficie -> **relazione** | `answer_frame/2`, `relation_verb/1` | si' (gen429) |
| 3 | superficie -> **ruolo di span** | `segment_role/2` | si' |
| 4 | superficie -> **costruzione con ruoli** | `construction_frame/3` | si' (A1) |
| 5 | superficie -> **classe con politica** (status, attribuzione, extent) | `hypothesis_report_marker/1` + `claim_marker_class/4` | si' (SC2-A) |
| 6 | superficie -> **operatore con forza** | `necessity_marker/1` + `modal_force/2` | si' (SC2-D) |
| 7 | superficie -> **relazione fra unita' con direzione** | `consequence_connector/1` + `argument_relation/2` | si' (SC3) |
| 8 | superficie -> **passo, precondizione, criterio d'arresto** | — | **no** (SC5) |
| 9 | superficie -> **contratto** (quale oracolo, quale permesso) | `faculty_for/2` esiste, i membri non si insegnano | **no** |

La riga che conta non e' quali gradini mancano: e' **perche' i sette che
funzionano funzionano**. In tutti e sette, cio' che si insegna non e' «un
comportamento a un modulo»: e' **un membro di una classe dichiarata che un
motore generico gia' consuma**. La classe porta la propria politica in KB
(`claim_marker_class/4`, `modal_force/2`, `argument_relation/2`) e il consumatore
non nomina nessuna superficie.

Dove questo e' vero, insegnare parlando funziona e l'ordine dei moduli non
serve. Dove non e' vero — gradini 8 e 9 — si e' tentati di far imparare al
modulo le proprie cue, e li' comincia il frasario.

#### Il test che separa le due strade

Prima di aprire un'attivita' su un modulo, una domanda sola:

> **la classe esiste, e un motore generico la legge?**
>
> Se si', la lezione entra parlando e non c'e' niente da fare al modulo.
> Se no, il lavoro non e' insegnare al modulo: e' **creare la classe** e dare al
> lettore universale un consumatore che la legga.

Il caso `helyla` a un token e' la controprova: la classe esiste, la lezione e'
entrata, e il modulo non e' il problema — il problema e' che il **titolo** a
rispondere non passa da nessuna classe.

### 18.32 Ipotesi D28 — insegnare una cue e' insegnare un membro, mai un comportamento

Le due meta' della nota di F. si compongono in un'ipotesi verificabile:

> ogni atto didattico su una superficie deve poter essere espresso come
> **`classe(membro)`**, con la politica della classe in KB e un consumatore
> generico che la legge. Un atto che richiede di toccare un modulo — il suo
> codice, o la sua posizione nel registry — e' la prova che manca una classe.

```prolog
teachable_class($Class, $Policy, $Consumer).
class_consumer_generic($Class).                  % nessuna superficie nel C
taught_member_effective($Class, $Member, $Turn).  % ha davvero cambiato il turno
taught_member_retractable($Class, $Member).       % e si e' potuto togliere
module_order_independent($Class).                 % senza toccare il registry
```

**Predizione falsificabile.** Per ognuno dei sette gradini gia' aperti, un
membro nuovo insegnato a voce deve (a) cambiare il turno successivo, (b) essere
ritrattabile a voce, (c) senza spostare nulla nel registry. Oggi il gradino 1
fallisce (b) — e (a) per i turni di un token solo. Se chiudere quei due casi
richiede una guardia in `mod_lone` o in `mod_forget`, l'ipotesi e' falsa e la
strada dei moduli e' inevitabile; se si chiudono con ruolo e scope
(D26/D27), e' confermata.

**Non-obiettivo.** L'ipotesi non dice che i moduli siano un male: la precedenza
a primo-match e la ridondanza restano un valore (`PRINCIPLES.md`). Dice che i
moduli sono **esecutori**, non il luogo dove la comprensione cresce. La
comprensione cresce dove crescono le classi e i loro consumatori generici —
cioe' lungo la catena di questo documento: span, ruolo, frame, evidenza,
registro, claim, argomento.

#### Esito del primo test di D28 (2026-08-30)

Il gradino 1 falliva su due contatori: una cue insegnata non era **ritrattabile**
(SC32, non conformita' rispetto ad `AGENTS.md`) e una cue di un token solo non
otteneva **titolo**. Il primo e' chiuso, e la chiusura e' la prova che
l'ipotesi chiedeva:

- **niente guardia in un modulo, niente riordino del registry.** Sono bastate una
  politica di scope (`cue_scope/2` + `mention_delimiter/2`: una cue non guarda
  dentro una menzione) e il **consumatore mancante** dell'altro verso
  (`try_forget_form`, che legge lo stesso `learnable/3` di `try_teach_form`);
- il livello e' additivo: una relazione senza `cue_scope` si comporta come prima;
- transfer su una seconda classe (`mood_tired`) mai usata per progettare la
  correzione;
- e il retract funziona **anche** per la cue che non ottiene titolo — togliere
  una lezione non dipende dal fatto che qualcuno l'abbia ascoltata.

Un dettaglio vale come conferma indipendente del mantra #8: prima che
`try_forget_form` corresse **davanti** a `try_teach_form`, la frase
`unlearn "X" as a casual opener` **re-insegnava** la cue, perche' «unlearn»
contiene «learn». L'ordine fra due handler generici non e' la stessa cosa che
l'ordine fra moduli: e' il verso di lettura di un unico registro.

Resta aperto il secondo contatore — il **titolo** — ed e' il caso di prova di
D26/D27: la cue e' viva, la lezione e' entrata, e chi risponde non passa da
nessuna classe. Finche' quello non e' chiuso, il gradino 1 e' conforme a meta'.

### 18.33 Ipotesi D29 — il difetto pericoloso è l'affermazione che copre meno di quanto legge

Tre volte in due giorni, su tre strati diversi, lo stesso difetto — e nessuna
delle tre volte somigliava a un errore:

| dove | che cosa usciva | che cosa c'era davvero |
|---|---|---|
| SC2-D, modalita' | `shorten(kinetic_impactor_can, asteroid_orbit)` | «a kinetic impactor **can** shorten…» — un'affermazione, da una possibilita' |
| SC27, argomento | «poggia su B» | «A. B. Therefore C.» — meta' del supporto, dichiarata intera |
| SC2-B, copertura | `slowed(dart, orbital_period)` | «…the orbital period **of Dimorphos**» — di chi fosse il periodo, perso |

Nessuno di questi e' un muro e nessuno e' un gap. Sono **risposte ben formate
costruite su meno di quanto il sistema aveva davanti**, ed e' esattamente la
forma che nessun controllo di superficie puo' vedere: l'uscita e' corretta nella
grammatica, plausibile nel contenuto, e piu' forte della propria fonte.

L'ipotesi generalizza D14 da una dimensione a tutte:

> ogni lettura deve portare con se' **che cosa ha consumato e che cosa c'era**,
> e la copertura non riguarda solo i token: riguarda operatori, ruoli, premesse,
> unita' e qualunque struttura che il livello sopra assuma completa.

```prolog
reading_covers($Reading, $Kind, consumed($N), available($M)).
%   $Kind ∈ token | operator | role | premise | unit | qualifier
coverage_complete($Reading, $Kind).
coverage_shortfall($Reading, $Kind, $Missing).
answer_strength($Answer, supported_by($Reading)).
overclaim($Answer, $Kind) :-
    answer_strength($Answer, supported_by($R)), coverage_shortfall($R, $Kind, $M).
```

Il punto operativo non e' impedire le letture parziali — SC27 mostra che a volte
la lettura parziale e' la migliore disponibile. E' che **una risposta non possa
essere piu' forte della copertura che la sostiene**: `overclaim/2` deve essere
interrogabile prima che la frase esca, non scoperto da un teacher che rilegge.

**Predizione falsificabile.** Iniettando in un corpus reale tre difetti costruiti
— un operatore inghiottito, una premessa fuori dal blocco adiacente, un
complemento pendente — il sistema deve produrre tre `coverage_shortfall`
distinti e **nessuna** risposta che li ignori. Se una delle tre esce come
affermazione piena, la copertura non sta proteggendo, sta solo contando.

**Rapporto con il mantra #7.** «Uccidi il muro, mai con una risposta sbagliata»
copre il caso in cui la risposta e' falsa. Questa e' la classe accanto: la
risposta e' *vera su meno*, e sopravvive a ogni controllo che chieda soltanto se
sia vera.

### 18.34 Ipotesi D30 — un default implicito è una decisione che nessuno può vedere

Le correzioni del 2026-08-30 hanno tutte la stessa forma, e vale la pena
nominarla perche' e' diventata una ricetta:

| difetto | default implicito che lo causava | politica dichiarata che lo ha chiuso |
|---|---|---|
| la cue si accende nel proprio retract | «una cue guarda tutto il turno» | `cue_scope/2` |
| un token solo e' contatto fatico | «chi arriva prima rivendica» | `move_policy(lone_bare_token, claim)` |
| due premesse rafforzano | «se il modo non e' noto, e' indipendente» | `support_mode` + `undetermined_mode` |
| il passivo generico batte una costruzione | «vince chi viene enumerato prima» | `specific_participle/1` |
| una lettura parziale vale | «se combacia, basta» | `normalization_extent_policy/4` |

In tutti e cinque i casi il motore stava gia' **decidendo**. Non lo diceva, e la
decisione non era ne' insegnabile ne' ritrattabile — cioe' era conoscenza di
dominio nel C, sotto un nome che non sembrava conoscenza: un default.

```prolog
engine_default($Site, $Decision, declared | implicit).
default_policy($Site, $Decision).
default_overridden($Site, $By).
```

**Predizione falsificabile.** Un censimento dei punti in cui il motore sceglie
senza consultare la KB deve produrre una lista **finita e corta** — decine, non
migliaia — e ognuno deve poter diventare una riga di politica senza cambiare il
comportamento di partenza. Se la lista e' lunga quanto il codice, i default non
sono una classe ma la struttura stessa del programma, e l'ipotesi e' falsa.

**Corollario operativo, gia' valido.** Quando una correzione richiede di
*spostare* qualcosa — un modulo nel registry, una clausola prima di un'altra —
quasi sempre esiste la stessa correzione scritta come politica, ed e' migliore
per tre ragioni misurate oggi: non cancella la strada vecchia
(`PRINCIPLES.md`), la rende ritrattabile nello stesso turno, e non rimanda il
problema alla parola successiva.

### 18.35 Ipotesi D31 — una facoltà nuova è quasi sempre una classe nuova

SC5 sembrava una facolta': *leggere un metodo* — passi, precondizioni, criteri
d'arresto, eseguibilita'. La coda del piano la teneva accanto a SC3 e SC4 come
un fronte suo, e il gradino 8 della scala della cue (§18.31) la dava per
**chiusa a nessuno**.

E' costata **tre righe di `claim_marker_class/4`** e una vista.

Il motivo e' che un passo di metodo non e' un oggetto nuovo: «We then heat the
mixture» dice che gli autori **descrivono** di aver scaldato, esattamente come
«the data show that …» dice che i dati mostrano. Cambia lo status — `described`
invece di `observed` — e cambia il contesto, `reported_method` invece di
`reported_belief`. Tutto il resto era gia' li': scorer universale, span,
remainder, normalizzazione in quarantena con la disciplina di copertura,
provenienza, prova pronunciata, retract, reteach.

L'ipotesi generalizza:

> quando un fronte sembra richiedere una facolta' nuova, la domanda prima e':
> **di quale classe gia' consumata e' un membro?** Una facolta' vera e' quella
> che sopravvive a questa domanda.

```prolog
faculty_reduces_to($Faculty, $Class, $Policy).
faculty_irreducible($Faculty, $Reason).
class_policy_axis($Class, $Axis, $Value).   % status, contesto, attribuzione, extent
```

I fronti gia' ridotti in questa serie, tutti su un solo asse della politica:

| fronte | classe consumata | asse che cambia |
|---|---|---|
| claim riportata (SC2-A) | — | fondazione |
| modalita' (SC2-D) | la stessa | operatore staccato prima |
| argomento (SC3) | la stessa + arco retorico | vista, non estrattore |
| metodo (SC5) | la stessa | status e contesto |

**Predizione falsificabile.** Preso il prossimo fronte della coda — disegno
sperimentale (SC4), causalita' (SC6) — deve essere possibile aprirlo con righe di
politica su una classe esistente e una vista, **senza** un nuovo produttore in C.
Se anche uno solo richiede un estrattore proprio, l'ipotesi ha un confine e va
scritto **dove** sta: quale asse della politica non basta piu'.

**Corollario di coda.** `LEARN_TODO.md` elenca fronti come se fossero
indipendenti. Se D31 regge, molte voci sono la stessa voce sotto politiche
diverse, e la coda va riletta chiedendosi per ciascuna di quale classe sia un
membro — un lavoro di riduzione, non di costruzione.

### 18.36 Ipotesi D32 — il testo omette ciò che il contesto già porta

«We then heat the mixture» ha per remainder «heat the mixture»: comincia dal
verbo, e nessuno schema soggetto-verbo-oggetto puo' legarlo. Per due giorni
questa sarebbe stata la risposta giusta — un residuo tipato, onesto e inutile.

Ma il soggetto non e' **assente**: e' **eliso**, e parrot0 lo aveva gia'. Sta
nell'attribuzione della classe, `attribution(document_authors)`, scritta al
momento in cui la claim e' stata osservata. La lettura si e' potuta completare
non indovinando, ma **leggendo la propria struttura**.

E' una mossa diversa da tutto cio' che precede in questa serie, e merita un nome:

> una lettura incompleta puo' essere completata **da coordinate che il sistema
> ha gia' costruito**, e questo non e' inferenza sul mondo: e' ricostruzione di
> cio' che il testo ha omesso perche' era ridondante.

```prolog
elided_role($Class, $Role, $Source).      % da dove si recupera
recovered_role($Reading, $Role, $Value, from($Coordinate)).
recovery_licensed($Class, $Role, $Source).
unlicensed_recovery($Reading, $Role).      % deve restare vuoto
```

Il confine e' tutto: **recuperare non e' inventare.** Il ruolo si puo' riempire
solo da una coordinata gia' osservata e ancora viva, e il riempimento deve
portare la propria provenienza — altrimenti si e' costruito il misclaim di D29
al contrario, aggiungendo invece che togliendo.

**Predizione falsificabile.** Ritrattare la coordinata da cui un ruolo e' stato
recuperato deve far **sparire il recupero**, non lasciarlo materializzato: se
l'attribuzione cade, il passo torna non legato. E `unlicensed_recovery/2` deve
restare vuoto su un corpus reale: nessun ruolo riempito da una coordinata che la
politica non autorizza.

**Estensioni prevedibili, tutte nella stessa forma.** L'ellissi non e' rara nella
prosa complessa: l'oggetto sottinteso di un passo successivo («then cool it»), il
soggetto di una relativa, l'unita' di misura ereditata dalla frase precedente,
l'agente di un passivo senza `by`. Ognuna e' una riga di `elided_role/3` **se** la
coordinata da cui recuperare esiste gia'. Dove non esiste, il residuo tipato
resta la risposta giusta — ed e' il modo di distinguere le due cose senza
scivolare nell'invenzione.

### 18.37 Ipotesi D33 — parrot0 non sa rileggere alla luce di ciò che ha appena imparato

> **Domanda di F., 2026-08-30.** «Quando dici *le due porte negative falliscono
> perché la claim è un'osservazione materializzata, uso passaggi nuovi*, di fatto
> trovi un limite e lo aggiri. Ma quel limite è nella natura di un motore
> conversazionale universale, o è una stupidità di parrot0? Vorrei che diventasse
> intelligente: aggirare va bene, ma è interessante capire come superarlo.»

La domanda merita una risposta misurata, non un'opinione. E la misura dice che
il limite è **reale**, non è nella natura del dialogo, e non è nemmeno una
stupidità: è una **coordinata messa nello strato sbagliato**, con una
conseguenza cognitiva molto più grande dell'inconveniente che l'ha rivelata.

#### Che cosa è giusto, e va difeso

SC2-A ha diviso due cose e la divisione è corretta:

- **osservazione**, irreversibile: *quel testo conteneva quella cue in
  quello span*. È successo, e nessuna lezione futura può farlo non succedere.
- **interpretazione**, viva: *quella cue significa `hypothesized`*. Dipende da
  conoscenza correggibile, quindi è una vista, e ritrattare la cue la spegne.

Questa è la ragione per cui l'ablation di SC2/SC3 funziona. Non si tocca.

#### Che cosa è nel posto sbagliato

`claim_proposition` — il **frame normalizzato** — sta dalla parte delle
osservazioni. Ma non è un'osservazione: è il *risultato di un'interpretazione*
che usa conoscenza ritrattabile (`relation_verb(heat)`). Sta con le cose
irreversibili una cosa che dipende da ciò che parrot0 sa oggi.

Perché ci è finito? Non per una scelta semantica: perché **la lettura vive nel C
e la KB non può richiamarla**. Una vista si ri-deriva a ogni interrogazione; un
frame no, perché ri-derivarlo vorrebbe dire rieseguire la fase pura dentro il
solver. Quindi è stato congelato. È un vincolo **meccanico**, travestito da
scelta di modello.

#### La conseguenza, misurata, ed è peggio dell'aggiramento

```text
> read: … We then warm the tube.          (warm sconosciuto)
!query claim_normalization_gap(…, no_reading)      ✓
> warm is a relation verb
> read: … We then warm the tube.          (stesso testo, stessa claim)
!query claim_normalized(…, reported)               ✓
!query claim_normalization_gap(…, no_reading)      ✓   ← ANCORA VERO
```

Rileggere **non rivede: accumula**. Dopo la seconda lettura parrot0 tiene
contemporaneamente due letture incompatibili dello stesso span, e niente dice
quale sia quella corrente. Il ratchet SC5-A passa **anche grazie a questo**, ed è
il motivo per cui la domanda di F. vale più della risposta che stavo per dare.

#### Il limite interessante, detto per quello che è

L'inconveniente di test è la buccia. Sotto c'è questo:

> **parrot0 non può rileggere ciò che ha già letto alla luce di ciò che ha
> appena imparato.** Le sue letture passate restano congelate al livello di
> comprensione che aveva quel giorno.

Un lettore umano fa il contrario di continuo: impara un termine e un articolo
letto la settimana prima **cambia significato**. Non lo rilegge con gli occhi di
prima. Questa è, senza retorica, una differenza di intelligenza — e non richiede
niente di magico per essere colmata, perché tutti i pezzi ci sono già.

#### La strada: la lettura dichiara da che cosa dipende

```prolog
reading_depends_on($Claim, $Knowledge).      % relation_verb(heat), cue viva, policy
knowledge_retracted($Knowledge, $When).
reading_stale($Claim, $Reason).
reading_current($Claim, $Frame) :- claim_proposition($Claim, $Frame),
                                   naf(reading_stale($Claim, $R)).
revision_pass($Document, $Rereadable).       % che cosa vale la pena rileggere
revision_effect($Claim, before($Old), after($New)).
```

Tre proprietà, e nessuna è un'invenzione:

1. **niente si cancella.** Una lettura superata diventa `stale`, non sparisce:
   `revision_effect/3` conserva prima e dopo, che è la genealogia che il
   progetto già pretende (`PRINCIPLES.md`, strutture secondarie).
2. **la dipendenza è già scritta.** `fact_source/3` e la provenienza registrano
   da dove viene un fatto; qui serve la stessa cosa per una *lettura*, ed è la
   coordinata che oggi manca.
3. **la revisione è un atto, non un effetto collaterale.** Rileggere costa: la
   politica di *quando* farlo è conoscenza (D21 — dove una conoscenza viene
   calcolata è conoscenza), non un automatismo nascosto.

**Predizione falsificabile.** Insegnato un verbo dopo aver letto un testo che lo
conteneva, una domanda sul contenuto di quel testo deve poter essere risposta
**senza che nessuno rilegga a mano**, e la lettura vecchia deve risultare
`stale` invece che coesistere con la nuova. Ritrattando di nuovo il verbo, la
lettura deve tornare al gap, non a un terzo stato. Se per ottenerlo serve
cancellare l'osservazione, la separazione osservazione/interpretazione era
sbagliata e va rifatta — non aggirata.

**Che cosa questo NON è.** Non è memoria a lungo termine, non è un secondo
lettore, e non è ri-addestramento. È la chiusura riflessiva di ciò che il
progetto fa già: se un'interpretazione è viva, deve esserlo **anche quando è
costosa da ri-derivare**, e il costo si dichiara invece di essere pagato con il
congelamento.

**Nota di metodo, sgradevole.** Il limite era davanti a me tre volte — SC2-C,
SC5, e qui — e le prime due l'ho aggirato usando passaggi nuovi, annotando
l'aggiramento come se fosse una proprietà del sistema. Aggirare un limite e
descriverlo sono compatibili; aggirarlo e **chiamarlo intenzionale** no. La
riga corretta, da qui in avanti: quando un test ha bisogno di un dato fresco per
passare, chiedersi prima se il sistema abbia bisogno di dimenticare o di
**rivedere**.

### 18.38 Risultato D34 — comprendere e' mantenere una vista versionata

SC40-A falsifica la parte debole di D33 e conferma quella forte. Non serviva un
secondo lettore e non serviva cancellare il passato. Servivano tre identita'
che prima erano fuse:

```prolog
claim_reading_record(Claim, Reading, Signature).   % storia
claim_current_reading(Claim, Reading).             % vista corrente
reading_depends_on(Reading, Knowledge).            % licenza
```

La superficie e lo span non cambiano. La firma passa invece da
`gap(no_reading)` a `normalized(frame(...))`; il record precedente resta
`stale` e `revision_effect/3` conserva l'arco. Ritrarre la lezione produce un
nuovo gap corrente, non riattiva in silenzio il primo record; il reteach produce
una nuova versione normalizzata. La genealogia e' quindi una sequenza di atti,
non un insieme di risultati incompatibili.

**Ipotesi D34.** Una rappresentazione e' comprensione soltanto se distingue
almeno osservazione, versione interpretativa, dipendenze e puntatore corrente.
Senza una delle quattro, o il passato viene cancellato o il presente resta
ambiguo.

**Predizione falsificabile.** Applicata a modalita', coreferenza, argomenti,
procedure e sintesi, la stessa decomposizione deve consentire retract locale e
ri-derivazione senza un nuovo parser. Se una di queste famiglie richiede una
semantica di versione diversa, D34 non e' un principio generale ma soltanto il
modello delle claim.

La risposta naturale pre-lezione aggiunge un corollario: il sistema riconosce
l'atto `described verification` ma dichiara che la proposition non e' ancora
normalizzabile. La metacomprensione non e' un altro riassunto del testo: e'
sapere **quale coordinata** della lettura manca.

### 18.39 Ipotesi D35 — la frontiera di revisione e' un taglio minimo nel grafo

SC40-A paga un full scan dichiarato dopo i moduli `knowledge`, `mention`,
`forget` e `teachconstruction`. E' semanticamente corretto e computazionalmente
grossolano: una domanda ordinaria servita da `knowledge` puo' visitare claim che
non dipendono da nulla di cambiato.

**Ipotesi D35.** La revisione necessaria dopo un atto didattico e' il taglio
minimo dei nodi raggiungibili dall'insieme di conoscenze mutate. Non e' «tutto
il documento» e non e' «l'ultimo modulo»: e' la chiusura transitiva inversa di
`depends_on`.

```text
knowledge_event(retract, relation_verb(warm))
        -> readings indexed by relation_verb(warm)
        -> method nodes depending on those readings
        -> answers/summaries depending on those method nodes
```

Un gap `unresolved` pone il problema interessante: non dipende ancora dalla
lezione che lo risolvera'. Serve una dipendenza candidata, ricavata dai token e
dalle famiglie di schema provate, oppure un fallback più largo dichiarato. Un
indice che vede soltanto le letture gia' riuscite ha precisione alta e recall
basso — ed e' quindi cognitivamente sbagliato anche se veloce.

**Predizione falsificabile.** Su 100 claim con dieci predicati ignoti,
insegnarne uno deve produrre lo stesso snapshot corrente del full scan visitando
soltanto le candidate che contengono quel predicato e i loro dipendenti. Le due
misure obbligatorie sono `RevisionRecall=1` contro il full scan e
`RevisionPrecision=changed/visited`; ottimizzare la seconda sacrificando la
prima falsifica il taglio.

### 18.40 Ipotesi D36 — il guadagno didattico e' soprattutto retroattivo

Il protocollo misura il transfer in avanti: insegno oggi, provo tre frasi
nuove. SC40 apre una dimensione più potente: quante letture **gia' possedute**
diventano migliori grazie alla lezione?

**Ipotesi D36.** A parita' di correttezza e costo, la prossima lezione migliore
massimizza il guadagno retroattivo pesato:

```text
retroactive_gain(Lesson) =
    sum(importanza(Reading) * livello_dopo-prima)
    - costo_revisione
    - rischio_overclaim
```

Questo non e' popularity. Una forma rara che sblocca la premessa critica di
dieci procedure puo' valere piu' di una forma frequente che cambia soltanto la
superficie. Provenienza e consumer dipendenti forniscono i pesi senza inventare
una confidence numerica.

**Predizione falsificabile.** Fra due lezioni candidate tratte dai residui,
quella con `retroactive_gain` maggiore deve produrre dopo il pass un aumento
maggiore di claim normalizzate o consumer riabilitati su un corpus held-out.
Se la correlazione resta nulla, D36 non ordina il curriculum meglio della sola
frequenza e va scartata.

### 18.41 Ipotesi D37 — metacomprendere e' prevedere l'effetto di una lezione

Sapere che cosa manca e' M13. Un livello ulteriore e' sapere **che cosa
cambierebbe** se quella lacuna venisse colmata, senza promuovere ancora la
lezione.

**Ipotesi D37.** La metacomprensione operativa e' un controfattuale sul grafo
delle dipendenze: dato un candidato in quarantena, parrot0 deve enumerare le
letture che diventerebbero correnti, quelle che resterebbero bloccate e i
consumer che si riaprirebbero. La previsione precede la mutazione ed e'
confrontata con `revision_effect` dopo la lezione.

**Predizione falsificabile.** Su tre candidati — uno utile, uno irrilevante e
uno che produrrebbe una lettura parziale — `predicted_revision_effect` deve
coincidere con l'effetto reale dopo promozione/rollback. Una previsione che
elenca tutto non conta: servono precisione e recall, e il candidato parziale
deve restare gap per la policy di copertura.

Questo e' il ponte naturale SC24 -> teacher attivo: parrot0 non chiede soltanto
«insegnami che cos'e' warm», ma «se mi insegni il ruolo di warm, rivedro' questi
passi; questo altro metodo restera' bloccato per il criterio d'arresto».

### 18.42 Ipotesi D38/D39 — spazio logico e propagazione della comprensione

Una singola lettura corrente non esaurisce il significato di prosa complessa.
Ambiguita', scope e assunzioni denotano una regione di modelli compatibili.
Versionare il frame suggerisce come versionare quella regione.

**Ipotesi D38.** Una lezione corretta restringe o ristruttura la regione dei
modelli compatibili; un retract la riallarga. `normalized` e' quindi un punto
nel reticolo dei livelli, mentre `modelled` conserva più alternative e i
vincoli che le escludono. La regione vuota e' contraddizione; una ricerca senza
controesempio entro budget e' `incomplete`, non entailment certo.

**Ipotesi D39.** Claim, argomento, metodo, modello e sintesi formano un unico
DAG di prodotti interpretativi. Una revisione locale deve propagarsi soltanto
ai discendenti: cambiare il verbo di un passo aggiorna la readiness del metodo
e le risposte che la consumano, ma non rilegge una premessa indipendente né
cancella la sua fonte.

**Predizioni falsificabili.** (1) una correzione di scope elimina alcuni
modelli ma conserva quelli ancora compatibili; (2) il retract ripristina la
regione precedente come versione, non ricostruendola da zero; (3) una lezione
su una claim cambia esattamente i nodi downstream elencati dalla dependency
closure; (4) una sintesi non revisionata mentre una sua claim e' stale viene
marcata `stale_summary` e non pronunciata come corrente.

Queste ipotesi definiscono «supercomprensione» senza invocare una facolta'
magica: più livelli della stessa struttura diventano osservabili, correggibili,
controfattuali e componibili nello spazio dei modelli.

### 18.43 Risultato D35 e ipotesi D40/D41 — evento estensionale e grafo delle opportunita'

SC40-B ha chiuso il primo taglio selettivo senza aggiungere un mutation hook
linguistico. Il motore fotografa prima/dopo una vista dichiarata dalla KB:

```prolog
revision_dependency_member(document_claim, relation_verb($Predicate)) :-
    relation_verb($Predicate).
```

La differenza fra i due insiemi e' l'evento. Il modulo vincitore resta soltanto
un gate: una domanda servita da `knowledge` che non cambia la vista non rilegge
niente. Aggiunta e rimozione di `relation_verb(solvates)` producono invece lo
stesso termine mutato e percorrono lo stesso indice in versi opposti.

Il ratchet piccolo misura `visited(1), changed(1)` su tre claim, `0/0` per un
verbo assente e nessun nuovo receipt per un fatto tassonomico ordinario. Lo
stress naturale permanente porta il denominatore a 100: una sola claim contiene
il membro insegnato, una sola viene visitata, la domanda risponde con fonte e il
retract riapre soltanto quel gap. D35 e' quindi confermata per la famiglia
esatta `relation_verb/1`; non ancora per tutte le coordinate della lettura.

**Ipotesi D40 — un evento semantico e' una differenza estensionale di una vista,
non il nome di un'operazione.** `assert`, lezione naturale, induzione o restore
possono cambiare lo stesso significato attraverso percorsi diversi; se il
consumer ascolta l'API chiamata, quattro percorsi producono quattro semantiche.
Se ascolta la vista prima/dopo, tutti producono lo stesso termine cambiato. Il
contrario vale per operazioni rumorose ma estensionalmente idempotenti: non sono
eventi per quel consumer.

**Predizioni falsificabili D40.** (1) insegnare lo stesso membro attraverso due
facolta' diverse produce lo stesso fronte; (2) reinsegnare un membro gia' vivo
non crea una revisione; (3) un restore che cambia dieci membri produce lo stesso
insieme di receipt di dieci cambi naturali, eventualmente in una sola
transazione; (4) aggiungere una nuova famiglia alla vista e alle regole candidate
non richiede modificare il diff C. Se serve riconoscere il nome della facolta'
per distinguere due significati uguali, D40 e' falsa o la vista e' incompleta.

Il secondo risultato e' piu' profondo. Una lettura riuscita puo' dichiarare
`reading_depends_on(Reading, relation_verb(solvates))`; un gap precedente alla
lezione non puo' dipendere da un fatto che non conosce ancora. Un indice basato
soltanto sulle giustificazioni attuali non avrebbe mai trovato la claim da
sbloccare. SC40-B ha dovuto aggiungere una seconda relazione, derivata dai token
osservati, per modellare **che cosa potrebbe diventare leggibile**.

**Ipotesi D41 — la supercomprensione richiede due grafi duali:** il grafo di
supporto dice da che cosa dipendono le interpretazioni correnti; il grafo delle
opportunita' dice quali interpretazioni mancanti potrebbero diventare possibili
se una conoscenza candidata fosse aggiunta. Il primo governa retract, proof e
staleness; il secondo governa apprendimento attivo, revisione dei gap e
`retroactive_gain`. Confonderli rende invisibile o il passato riuscito o il
futuro apprendibile.

```text
support graph:      knowledge -> reading corrente -> consumer autorizzati
opportunity graph:  candidate knowledge -> gap/residui -> letture possibili
```

**Predizioni falsificabili D41.** (1) un candidato presente in cento residui ma
in nessuna lettura riuscita ha guadagno retroattivo non nullo; (2) ritrarre una
licenza percorre il grafo di supporto senza inventare opportunita' estranee;
(3) una lettura parziale compare nell'opportunity graph ma resta bloccata dalla
policy di copertura; (4) la previsione SC37 di una lezione coincide con gli
effetti reali soltanto se unisce i due grafi. Se un unico `depends_on` copre i
quattro casi senza perdere la distinzione fra prova e possibilita', D41 puo'
essere semplificata; fino a quella dimostrazione la dualita' e' il modello piu'
falsificabile.

Il prossimo confine SC41 segue direttamente: enumerare **tutte** le coordinate
consultate da una lettura e stabilire quali siano supporto, quali scelta fra
candidate e quali opportunita'. Solo dopo ha senso propagare il taglio verso
argomenti, procedure, regioni di modelli e sintesi (SC42).

### 18.44 Risultato SC41-A — la prova fa parte dell'identita' della comprensione

SC41-A chiude il primo denominatore locale: una lettura passiva conserva ora
predicato, schema selezionato, morfologia, ausiliare, marker d'agente, ordine
dei ruoli e policy di copertura. Il C conserva il pattern opaco prodotto da
`extract_frame/2`; e' la KB a convertirlo nelle coordinate
`frame_reading_dependency/2`. Nessuna parola del passivo entra nel motore.

Una sola lezione naturale, `bound is an irregular participle`, ha revisionato
tre claim scientifiche gia' osservate con `visited(3), changed(3)`. Il retract
ha ripristinato tre gap e il reteach ha ricostruito tre versioni normalizzate,
sempre senza replay. `DependencyCompleteness(passive_core)=7/7` e' enumerabile
da `reading_dependency_requirement/2` e `reading_dependency_coverage/2`; non e'
un commento che conta soltanto cio' che l'implementazione ricorda.

Il caso piu' informativo conserva invece la stessa proposizione. Una
costruzione insegnata continua a produrre `glorphs(mira,kora)` quando la licenza
`relation_verb(glorphs)` viene ritratta; la nuova licenza e'
`frame_predicate(glorphs)`. La firma semantica e' identica, ma nasce comunque
una versione successiva e la vecchia resta stale.

**Ipotesi D42 — una comprensione corrente ha identita' almeno
`(interpretazione, giustificazione)`.** Due letture con lo stesso frame e prove
diverse non sono la stessa lettura. Collassarle impedisce di rispondere a «perche'
lo leggi cosi'?», rende l'ablation non locale e nasconde sostituzioni fragili
con supporti piu' forti.

**Predizioni falsificabili D42.** (1) sostituire una prova mantenendo il frame
produce un successore genealogico; (2) ritrarre la prova vecchia non spegne il
frame sostenuto dalla nuova; (3) una risposta «come hai interpretato questo
passaggio?» deve poter nominare la prova corrente, non una prova storica; (4)
due proof set equivalenti per derivazione possono collassare soltanto se la loro
equivalenza e' dimostrata in KB.

### 18.45 Ipotesi D43 — licenza, selezione e opportunita' sono tre archi diversi

SC40 distingueva supporto riuscito e opportunita' futura. SC41 mostra che il
supporto stesso ha almeno due specie:

```text
license(K)    senza K la lettura non esiste
selection(K)  K fa vincere/ordina una lettura fra alternative
opportunity(K, Gap)  aggiungere K potrebbe aprire un residuo
```

Ritrarre una licenza tende ad aprire un gap; ritrarre una selezione tende a
riaprire una regione di alternative o a cambiare l'ordine dei ruoli; aggiungere
un'opportunita' non prova ancora alcuna lettura. Rappresentare tutti e tre come
un `depends_on` non tipato conserva forse la reachability, ma perde la politica
di revisione e la spiegazione del cambiamento.

**Ipotesi D43.** La metacomprensione minima e' un ipergrafo tipato: non domanda
soltanto «da che cosa dipendo?», ma «questa conoscenza mi autorizza, mi sceglie
o mi renderebbe possibile?». Il tipo dell'arco predice l'esito del retract prima
di eseguirlo.

**Predizioni falsificabili D43.** Su una matrice di ablation, una licenza
ritratta produce un gap, una selezione ritratta produce ambiguita' o un frame
alternativo, un'opportunita' ritratta cambia il curriculum ma non una lettura
corrente. Se i tre esiti non correlano con il tipo, la tassonomia non aggiunge
potere predittivo e va semplificata.

### 18.46 Ipotesi D44 — gli eventi devono stare alla radice causale minima

`past_participle(slowed)` e' una vista derivata da `relation_verb(slowed)` e dal
suffisso; fotografare entrambe produrrebbe due eventi e due revisioni per una
sola lezione. `past_participle(bound)`, invece, nasce dalla radice autonoma e
insegnabile `irregular_participle(bound)`. SC41-A fotografa quest'ultima e
conserva entrambe nella proof della lettura.

**Ipotesi D44.** Il membro di evento ottimale e' la radice causale minima che il
teacher puo' aggiungere o ritrarre; le viste derivate appartengono alla proof,
non necessariamente al journal degli eventi. Questo e' analogo a un database:
il delta si registra sulla base relation, mentre la materialized view viene
invalidata per dipendenza.

**Predizioni falsificabili D44.** (1) una lezione regolare produce un solo
evento pur aggiornando piu' viste; (2) una lezione irregolare produce un evento
distinto; (3) replay idempotente non produce eventi; (4) il set di letture
finali coincide con uno snapshot che fotografi tutte le viste, ma con meno pass.
Se manca anche una sola revisione, la radice scelta era troppo stretta.

### 18.47 Ipotesi D45 — la completezza e' locale, stratificata e avversariale

Il valore `7/7` di SC41-A e' vero soltanto per `passive_core`. Marker
epistemico, modalita', ellissi, coreferenza, confini di sintagma e precedenza
fra schemi restano consultazioni del lettore non ancora incluse nello stesso
denominatore. Chiamare il lettore «completo» sommando soltanto le coordinate
implementate sarebbe Goodhart sul grafo delle dipendenze.

**Ipotesi D45.** `DependencyCompleteness` deve essere un vettore per strato e
costruzione, con un denominatore derivato dal trace delle consultazioni:

```text
DC(frame/passive), DC(modality), DC(epistemic_marker),
DC(ellipsis), DC(coref), DC(coverage), DC(consumer_propagation)
```

La metacomprensione nasce quando parrot0 sa dire quali celle sono incomplete e
quale esperimento le falsificherebbe. La supercomprensione non e' quindi un
numero grande, ma la capacita' di mantenere interpretazioni proof-carrying,
alternative logiche e confini espliciti della propria prova.

**Predizioni falsificabili D45.** Un corpus avversariale che incrocia passivo,
modale ed ellissi deve far scendere soltanto le celle mancanti; aggiungere la
genealogia modale deve alzare `DC(modality)` senza alterare `DC(frame/passive)`.
Una metrica scalare che resta 1 mentre un'ablation non viene propagata e'
falsificata per costruzione.

### 18.48 Ipotesi D46 — capire un input rumoroso richiede causalita' controfattuale

Il caso `quanot fa 2 +3` contiene due anomalie visibili, ma soltanto una e'
causale. Correggere `quanot` non chiude `quanto fa 2 +3`; separare `+3` chiude
anche `quanot fa 2 + 3`. La distanza ortografica avrebbe quindi ordinato prima
la riparazione sbagliata, perche' misura somiglianza della superficie e non
necessita' rispetto al goal.

**Ipotesi D46.** Una metacomprensione robusta rappresenta la riparazione come
un insieme causale minimo di trasformazioni rispetto agli obblighi del turno.
Ogni trasformazione candidata viene ablatata mantenendo fissi goal, contesto e
resto della superficie. Si promuove soltanto cio' la cui sottrazione riapre
l'arresto.

**Predizioni falsificabili D46.** (1) su input con due errori, uno non causale
non compare nella proof finale; (2) correggere il solo errore non causale non
cambia il primo arresto; (3) l'insieme promosso e' invariante all'ordine in cui
si provano i candidati; (4) aggiungere trasformazioni non necessarie peggiora
precisione senza aumentare chiusure. Se la distanza superficiale predice sempre
lo stesso insieme del replay controfattuale su un corpus stratificato, la
procedura causale puo' essere semplificata; il primo testimone la distingue gia'.

### 18.49 Ipotesi D47 — la robustezza e' un intorno semantico stratificato

Cambiare una cifra, un operatore, una cue o la lingua non sono variazioni della
stessa natura. Una lista piatta di parafrasi puo' produrre cento verdi quasi
identici e crollare al primo cambio di coordinata. Il verticale della
segmentazione usa invece una matrice: quattro operatori, valori nuovi, cue
EN/IT, segni unari, negativi e una superficie operatore appresa a runtime.

**Ipotesi D47.** Il raggio di supercomprensione di una lettura e' un vettore per
coordinate di variazione, non il conteggio dei prompt verdi. La capacita' e'
chiusa soltanto entro il prodotto cartesiano preregistrato delle coordinate su
cui trasferisce e resta selettiva.

```text
RobustnessVector = (surface, tokenization, intent, language,
                    value, operator, discourse_context, negative_scope)
```

**Predizioni falsificabili D47.** (1) una micro-media alta puo' coesistere con
una coordinata interamente rossa; (2) insegnare un membro KB nuovo alza la
coordinata `operator` senza patch C; (3) l'ablazione della licenza abbassa le
sole celle che la usano; (4) una nuova firma logica non viene nascosta nella
media della famiglia precedente. Se una metrica scalare preserva tutte queste
distinzioni e predice le ablazioni, il vettore e' ridondante.

### 18.50 Ipotesi D48 — la normalizzazione deve essere proof-carrying e bifocale

Una riscrittura silenziosa perde due oggetti: che cosa l'utente ha davvero
detto e perche' il sistema ha consumato un'altra forma. Il primo serve per
correggere una diagnosi; il secondo per replay, ablation e spiegazione. Nel
verticale AC1 l'utterance resta originale e `turn_surface_repair/4` conserva
classe, operazione, token osservato e sequenza consumata.

**Ipotesi D48.** Ogni comprensione indiretta mantiene simultaneamente il piano
fenomenico (byte/span originali) e il piano operativo (struttura normalizzata),
uniti da una proof revocabile. Non si sostituisce il primo con il secondo.

**Predizioni falsificabili D48.** (1) ritrarre la licenza rende non valida la
normalizzazione senza alterare lo storico dell'utterance; (2) due
normalizzazioni concorrenti restano confrontabili; (3) una domanda «che cosa
hai corretto?» puo' indicare span e base KB; (4) il replay pulito e quello
riparato producono semantica canonica identica ma genealogy diversa. Se una
riscrittura muta permette le stesse quattro operazioni senza stato nascosto,
D48 e' falsa.

### 18.38 Ipotesi D34 — una forma ha una famiglia di varianti, e la variante non è una lezione

Il giro GD2 ha insegnato **200 forme colloquiali reali** parlando, tutte
accettate, 193 persistite, verificate in processo nuovo. Poi ha misurato il
muro che nessuna quantità di forme supera:

```text
you're a legend   (insegnata)   ->  battuta riconosciuta
you are a legend                ->  «Alright — I am a legend now.»
perché non funziona             ->  «Annotato: lo tengo come stato attuale.»
cmq ciao                        ->  muro
comunque ciao                   ->  «Arrivederci!»
```

Tre fatti, in ordine di gravità crescente:

1. **Una variante ortografica azzera la lezione.** Apostrofo (`you're`/`you
   are`), accento (`perche`/`perché`), abbreviazione di chat
   (`cmq`/`comunque`, `xké`/`perché`, `nn`/`non`, `qnd`/`quando`), elisione,
   maiuscole. Ogni variante costa una lezione propria: un lessico di 200 forme
   ne vorrebbe 600 e ne mancherebbe comunque. **È un frasario travestito da
   corpus**, e il mantra #2 lo vieta esattamente come vieta la lista nel C.
2. **Il fallback non è un muro: è un misclaim.** A un complimento parrot0 ha
   risposto affermando qualcosa **su di sé** — «I am a legend now». È D29 in un
   posto nuovo: la risposta più forte della propria fonte, prodotta non da una
   lettura parziale ma da una lettura *sbagliata* che nessuna guardia intercetta
   perché la frase è ben formata.
3. **La classe non esiste.** `grep` su `kb/core` trova solo `lexeme(abbrev)`,
   che è un'altra cosa. Non c'è nessun posto dove dire che due superfici sono la
   stessa forma.

L'ipotesi:

> una forma linguistica non è una stringa: è una **famiglia di superfici** che
> denotano la stessa cosa. Insegnare la forma deve insegnare la famiglia, e il
> riconoscimento deve avvenire sulla forma canonica — mai su ciascuna variante
> ripetuta a mano.

```prolog
form_variant($Canonical, $Variant, $Kind).
%   $Kind ∈ apostrophe | accent | chat_abbrev | elision | spacing | case
variant_rule($Language, $Kind, $Pattern, $Replacement).
canonical_surface($Surface, $Canonical).
variant_licensed($Language, $Kind).      % una lingua ammette certe famiglie
ambiguous_variant($Surface, $A, $B).     % «e» -> «è»? si conserva, non si sceglie
```

Due confini che rendono l'ipotesi non banale:

- **la canonicalizzazione della variante non è traduzione.** Deve avvenire
  *prima* del matching delle cue e *dopo* la protezione della menzione (D25):
  una parola dentro le virgolette è citata, e la sua ortografia è il contenuto.
  Le due politiche si compongono, e sbagliare l'ordine rompe l'insegnabilità
  che SC32 ha appena guadagnato.
- **una variante può essere ambigua.** In italiano `e`/`è`, `da`/`dà`, `si`/`sì`
  sono coppie vere: risolverle per default introdurrebbe l'errore che la
  classe esiste per togliere. L'ambiguità si conserva e si dichiara, come
  `undetermined_mode` in SC27.

**Predizione falsificabile.** Insegnata **una** forma canonica, tutte le sue
varianti dichiarate devono funzionare **senza una seconda lezione**; ritrattare
la forma deve spegnerle tutte insieme; e una variante ambigua non deve mai
essere risolta in silenzio. Se per coprire le varianti bisogna insegnarle una
per una, la famiglia non esiste e l'ipotesi è falsa.

**Perché è il moltiplicatore.** GD2 ha misurato che un lessico fa esattamente
ciò che un lessico può fare: `+29%` relativo sulla famiglia che indirizza, e
niente altrove. Le varianti non aggiungono una famiglia: **moltiplicano tutte
quelle che già esistono**, comprese le classi di SC2-SC5 — un marker epistemico
scritto con un accento diverso oggi non è lo stesso marker.

### 18.39 Che cosa GD1 ha localizzato, e perché conta più del delta

La misura di GD1 vale al di là del giro che l'ha prodotta: dice **dove sono i
muri**, e non sono sparsi.

| famiglia | muri (su 30) | dove si chiude |
|---|---:|---|
| F03 context/coref | 24 | riferimento cross-turn — la colla del dialogo lungo |
| F10 complex prose | 21 | SC2-SC5, già in corso |
| F05 clarification | 20 | la mossa di chiarimento, K3 |
| F09 procedure/rollback | 20 | SC5-B, input/output tipati e annullamento |
| F11 scientific literature | 20 | SC2/SC4 |
| F12 mixed code/math | 20 | composizione fra registri |

**F03 è la voce singola più alta di tutto il corpus** e non ha un fronte aperto
nella coda: il riferimento che attraversa i turni — «quello», «l'altro»,
«quello di prima» — è ciò che rende un dialogo un dialogo invece che una
sequenza di domande. Vale una voce propria, ed è **GD4**.

Nota di metodo: il delta di GD2 (+3% sul totale) sarebbe stato leggibile come
fallimento da chi guardasse solo il totale. Guardando per famiglia, il lessico
ha fatto il suo lavoro dove poteva e zero dove non doveva — e *zero dove non
doveva* è il risultato che conferma che non ha barato.

### 18.40 Ipotesi D35 — ciò che si impara da una frase deve essere interrogabile con la stessa frase

Misurato il 2026-08-31, e spiega due giri di insegnamento che non crescevano:

```text
> Il libro rosso è sul tavolo.   ->  Learned: located_in(book_red, tavolo).
> dove si trova il libro rosso   ->  muro
> dove si trova book_red         ->  Tavolo.     ← solo col nome INTERNO
> Il gatto è sul tetto.
> dove si trova il gatto         ->  Tetto.      ← una parola sola: funziona
```

Un'entità di **una** parola fa il giro completo. Una di **più** parole no: la
lettura canonicalizza «il libro rosso» in `book_red` — tradotto *e* riordinato —
e il percorso della domanda non applica la stessa trasformazione. L'unico modo
di recuperare quel fatto è pronunciare un nome che nessun essere umano
digiterebbe.

> **parrot0 impara sotto un nome che non sa più pronunciare.**

L'invariante che manca non è una capacità nuova: è una **simmetria**.

```prolog
entity_key($Surface, $Language, $Key).      % la stessa funzione per i due versi
entity_surface($Key, $Language, $Surface).  % e il ritorno, per poterlo dire
key_roundtrip($Surface, $Language).         % deve valere per costruzione
unnameable_fact($Fact, $Reason).            % il censimento di ciò che è murato
```

**Perché è la stessa forma di D33.** Là un'interpretazione era congelata fra le
osservazioni perché la KB non poteva richiamare la lettura; qui una chiave è
costruita da un percorso e non dall'altro. In entrambi i casi il difetto non è
in ciò che parrot0 *sa*, ma nel fatto che **due percorsi che devono accordarsi
non condividono la funzione che li accorda** — la stessa cura della fase pura di
SC2-B, applicata alla chiave invece che al frame.

**Predizione falsificabile.** Per ogni frase dichiarativa che produce un fatto,
la domanda formata con **le stesse parole** deve raggiungerlo — in entrambe le
lingue, e a qualunque numero di parole del sintagma. Se serve conoscere il nome
interno, la simmetria non c'è. E `unnameable_fact/2` su un corpus reale deve
tendere a zero: un fatto che nessuna frase può nominare è conoscenza murata, e
contarla è il modo per sapere quanta ne stiamo producendo.

**Portata, misurata.** Il corpus GD1 è pieno di referenti multi-parola — «il
libro rosso», «il quaderno blu», «il treno notturno»: sono la norma del parlato,
non un caso limite. Finché la simmetria manca, ogni fatto appreso da prosa
italiana su un nome composto è scritto in un cassetto senza maniglia, e nessuna
quantità di lessico o di forme interrogative lo apre.

### 18.41 Ipotesi D36 — un aggregato nasconde una congiunzione

Due giri di insegnamento, 276 forme reali entrate parlando e verificate in
processo nuovo, hanno mosso **+11 turni su 360**. La spiegazione non è che il
metodo sia debole: è che il corpus misura **congiunzioni** e i giri riparavano
**congiunti**.

Un turno del corpus riesce solo se tengono insieme, tutte insieme: la superficie
riconosciuta, la forma della domanda nella lingua giusta, il nome dell'entità
che fa il giro, il riferimento risolto, il fatto presente e la realizzazione
disponibile. Riparare uno solo di questi muove ~zero, perché il turno continua a
fallire su un altro — e il tasso di muro aggregato **non lo dice**: `66% di muri`
suggerisce «serve più conoscenza», mentre la traccia di un dialogo solo ha detto
«servono quattro cose diverse, e una è un bug di simmetria».

```prolog
turn_requires($Turn, $Link).             % superficie, chiave, riferimento, fatto…
link_holds($Turn, $Link).
turn_blocked_by($Turn, $Link).           % il PRIMO anello che cede
chain_closed($Family, $Turn).
shortest_chain_to_close($Family, $Links).
```

**Predizione falsificabile.** Registrando per ogni turno quale anello cede per
primo, la distribuzione su un corpus reale deve essere **concentrata** su pochi
anelli, non uniforme. Se è uniforme non esiste una catena corta da chiudere, e
la strategia giusta torna a essere quantitativa. La misura del 2026-08-31 dice
che è concentrata: quattro anelli su un dialogo di cinque turni, e uno dei
quattro (la chiave dell'entità) compare in tutti i turni che nominano una cosa.

**Corollario di metodo, ora vincolante.**

1. **Traccia una catena intera prima di insegnare.** Un aggregato non dice mai
   dove si rompe; un dialogo eseguito turno per turno lo dice in trenta secondi.
2. **Ripara la catena più corta che chiude una famiglia**, non il difetto più
   evidente. Il criterio non è «quanti muri tocca» ma «quanti anelli restano».
3. **Insegnare viene per ultimo.** Il lessico moltiplica una catena che chiude;
   su una catena rotta è rumore misurabile a +3%.
4. **Il gate è una famiglia che chiude**, non un punto percentuale sul totale:
   un dialogo che passa da capo a fondo prova che la catena regge, dieci punti
   sparsi non provano niente.

### 18.42 Ipotesi D37 — un'entità è un referente con proprietà, non un atomo fuso

> **Domanda di F., 2026-08-31.** «Questo cassetto senza maniglia è legato a
> predicati senza costruzione logica e metadati: il famoso `book_red` non
> significa nulla. Sarebbe interessante pensare a `book` **con le sue
> proprietà** in uno spazio dialogo. E per slittare le frasi in pezzi annidati
> caratterizzanti si dovrebbe costruire un IR per la frase
> ([[universal-input]]), e tutto questo dovrebbe concorrere alla comprensione
> universale ([[universal-comprehension]]). Ha senso o mi illudo?»

Ha senso, e la parte scomoda della risposta è un'altra: **è già scritto nei
piani, e la struttura è già costruita a metà.** Il difetto non è che manchi la
IR — è che chi produce i fatti non la usa.

#### Che cosa i piani già dicono

`universal-input.md` §4bis descrive la catena esatta che F. nomina:

```text
flusso -> segmento/register span -> token span
       -> sintagmi (NP, VP, PP, clause)
       -> relazioni fra sintagmi (subject, predicate, object, modifier)
       -> frame o intent schema della KB
```

e aggiunge, testualmente, che **«`extract_frame/2` consuma la stessa
struttura»**. `universal-comprehension.md` §4bis porta l'esempio che è la
risposta alla domanda:

```text
"The okapi, a giraffid mammal, lives in Congo"
  -> NP(subject) + apposition(NP) + VP + PP
  -> is_a(okapi, giraffid_mammal)
     located_in(okapi, congo)
```

Testa e proprietà **separate**, due fatti invece di un atomo fuso
`okapi_giraffid_mammal`. È esattamente «book con le sue proprietà».

#### Che cosa esiste davvero, misurato oggi

Per «Il libro rosso è sul tavolo.» parrot0 costruisce **già** la gerarchia:

```text
input_node(current_turn, 0, node(clause, clause, root), range(...))   ✓
input_node(current_turn, _, node(phrase, _, _), range(...))           ✓
input_node_atom(current_turn, 1..5) = il · libro · rosso · sul · tavolo ✓
```

Clausola, sintagmi, token, con gli offset. Il punto (a) di
`universal-comprehension` §5 — «manca la proiezione gerarchica dell'InputSpan in
token e sintagmi» — **non manca più**: è arrivato col gen438 e quel paragrafo non
lo sa.

#### Che cosa succede invece, nello stesso turno

```text
Learned: located_in(book_red, tavolo).
```

`extract_frame/2` **non guarda la gerarchia**. Prende i token, li canonicalizza,
li unisce con `p0_join` e produce una stringa. Da «il libro rosso» esce
`book_red`: testa e modificatore fusi, determinante buttato, ordine invertito,
lingua cambiata a metà.

Due percorsi paralleli, e vive quello sbagliato:

| percorso | stato | esito |
|---|---|---|
| gerarchia `input_node` → `input_semantic_frame` | **costruito, dormiente** | ruoli, span, nesting conservati |
| `extract_frame` → `p0_join` | **vivo** | `book_red`, un atomo senza struttura |

#### Perché `book_red` è peggio di un nome brutto

Non è un nome scomodo: è un nome **che ha perso informazione**, e ogni perdita
chiude una porta diversa.

| ciò che è andato perso | ciò che non si può più fare |
|---|---|
| testa (`libro`) distinta dal modificatore (`rosso`) | chiedere «quale libro?»; far combaciare «il libro» con «il libro rosso» |
| il determinante | distinguere «un libro» da «il libro» — cioè introdurre vs riprendere |
| la posizione nel discorso | risolvere «il primo», «quello», «l'altro» |
| la lingua della superficie | ripronunciarlo come è stato detto |

Ed è la stessa perdita che rende F03 la famiglia peggiore del corpus (24 muri su
30): la coreferenza non ha niente a cui attaccarsi, perché **non esiste un
referente** — esiste solo un atomo fuso.

#### L'ipotesi

> Leggere una frase deve creare **referenti di discorso**, non chiavi. Un
> referente ha una testa, delle proprietà, un determinante, una menzione con
> span, e vive nello spazio del dialogo finché il dialogo dura. Il fatto lega
> referenti, non stringhe.

```prolog
referent($Scope, $Id, head($Concept)).
referent_property($Id, $Property, $Span).
referent_determiner($Id, definite | indefinite).
referent_mention($Id, $Turn, range($Start, $Length)).
referent_same($IdA, $IdB, $Evidence).      % coreferenza: una RELAZIONE, non una fusione
fact_binds($Fact, $Role, $Id).             % located_in lega referenti
referent_surface($Id, $Language, $Surface). % e sa ridirsi come è stato detto
```

Il correttivo che aggiungerei alla formulazione di F.: il problema non sono i
**metadati mancanti sul predicato**. Aggiungere metadati a `book_red` non
servirebbe, perché il danno è già fatto a monte. Serve che `book_red` **non
venga mai creato**: al suo posto un referente con testa `libro` e proprietà
`rosso`, e un fatto che lega quel referente al referente `tavolo`.

**Predizione falsificabile.** Dopo «Il libro rosso è sul tavolo»:
«dove si trova il libro rosso» e «dove si trova il libro» devono **entrambe**
rispondere; «di che colore è il libro» deve rispondere dalla proprietà; «Il
libro è grande» deve attaccarsi allo stesso referente invece di crearne un
secondo; e «dov'è il primo», dopo un secondo oggetto, deve risolversi o
dichiararsi ambiguo. Se per ottenerlo bisogna enumerare le frasi, il referente
non c'è.

**Rapporto con D35.** D35 chiedeva la simmetria fra imparare e chiedere. Il
referente è **il modo giusto** di ottenerla: due percorsi si accordano non
perché condividono una funzione di normalizzazione, ma perché parlano dello
stesso oggetto. La simmetria diventa una conseguenza, non una toppa.

**Perché non è un rifacimento.** La gerarchia c'è, gli span ci sono, la KB ha
già `semantic_entity`, `phrase_form`, `denotation` e `input_semantic_frame`. Ciò
che manca è **una giunzione**: far sì che il produttore di fatti legga la
struttura invece di ricostruirla piatta. È lo stesso movimento di SC2-B — una
fase pura condivisa al posto di due percorsi che si ricostruiscono a vicenda — e
lo stesso di D33 e D35. Tre volte la stessa forma: **due percorsi che devono
accordarsi e non condividono l'oggetto su cui accordarsi.**

### 18.43 Ipotesi D38 — connettere zone della KB è unificazione, e l'unificazione è la condizione dell'emergenza

> **Considerazione di F., 2026-08-31.** «La pratica di connettere zone della KB
> attraverso **archi di ordine superiore** è una sorta di **unificazione
> dell'intelligenza**. Può sembrare banale, ma quando — solo a titolo di esempio
> — le abilità aritmetiche si combineranno e si connetteranno con le abilità
> sociali, questo potrebbe portare all'emersione di abilità che **non abbiamo
> progettato per design nella KB**, ma che emergono dalla complessità.»

Non è banale, e la ragione per cui non lo è si è vista oggi tre volte di fila.

#### Perché non è banale: la stessa forma, a due scale

Le tre diagnosi del 2026-08-30/31 hanno tutte lo stesso scheletro:

| | difetto | cura |
|---|---|---|
| D33 | un'interpretazione congelata perché la KB non può richiamare la lettura | far condividere ai due percorsi **la lettura** |
| D35 | una chiave costruita da un percorso e non dall'altro | far condividere ai due percorsi **la chiave** |
| D37 | una struttura costruita da un percorso e ignorata dall'altro | far condividere ai due percorsi **il sintagma** |

Ogni volta il difetto era *due percorsi che devono accordarsi e non condividono
l'oggetto su cui accordarsi*, e ogni volta la cura è stata **creare l'oggetto
condiviso**. F. sta descrivendo la stessa mossa un piano più su: non fra due
percorsi dello stesso meccanismo, ma fra due **zone di competenza**.

E se la mossa funziona alla scala del meccanismo — misurato tre volte in due
giorni — l'ipotesi che funzioni alla scala del dominio non è un'analogia
gratuita: è la stessa legge, applicata dove non l'abbiamo ancora applicata.

#### Che cosa dice davvero `PRINCIPLES.md`

> «Il substrato è quasi uniforme, eppure si formano **circuiti funzionalmente
> specializzati**. Uniforme il substrato, articolata la funzione. […] La
> struttura è la condizione dell'emergenza, non il suo nemico.»

Quel corollario è sempre stato letto come *permesso a differenziarsi* — lascia
che il cervello si articoli. F. ne nomina la metà mancante: **differenziarsi non
basta, le parti differenziate devono potersi parlare.** Un organismo fatto di
organi che non comunicano non è un organismo, è una collezione. Oggi la KB di
parrot0 è ricca di zone — aritmetica, sociale, geografia, scacchi, chimica,
prosa scientifica — e povera di **archi fra zone**.

#### L'ipotesi, in forma verificabile

> Le capacità di parrot0 non stanno soltanto nei suoi predicati: stanno anche
> negli **archi fra zone** che nessuno ha progettato come capacità. Aggiungere un
> arco fra due zone deve produrre comportamenti che **nessuna delle due zone
> possiede da sola**, e quei comportamenti devono essere **dimostrabili su un
> compito**, mai dichiarati.

```prolog
kb_zone($Zone, $Predicates).                    % il censimento delle zone
zone_bridge($ZoneA, $ZoneB, $Relation, $Why).   % l'arco, con la sua ragione
composed_capability($Capability, from($A, $B), $Proof).
unplanned_capability($Capability).              % nessuno l'ha scritta come tale
bridge_yield($Bridge, $TasksUnlocked).          % quanto ha reso
false_composition($Capability, $Counterexample). % e quanto ha rotto
```

#### La prova che F. propone, presa alla lettera

Aritmetica × sociale è un ottimo primo arco perché nessuna delle due zone
risolve da sola compiti che un umano trova ovvi:

```text
«siamo in quattro e il conto è 86 euro, quanto ciascuno?»
   -> serve il numero di partecipanti dalla zona SOCIALE (chi è «noi»)
      e la divisione dalla zona ARITMETICA

«ho salutato tre persone stamattina, e poi altre due»
   -> serve l'atto sociale come EVENTO CONTABILE
```

Nessuno ha progettato «dividere un conto» come capacità. Se emerge dall'arco fra
due zone che esistevano già, l'ipotesi ha una prova; se per farla funzionare
bisogna scrivere una regola che nomina i conti, l'arco non stava facendo lavoro
e l'ipotesi è falsa in quel punto.

#### Le due guardie, che non sono negoziabili

1. **Comporre può produrre il falso.** Due zone corrette possono generare
   un'inferenza sbagliata proprio perché nessuna delle due la controlla. Un arco
   deve quindi portare la propria **provenienza** e ogni composizione dev'essere
   **refutabile**: `false_composition/2` non è un campo decorativo, è il prezzo
   d'ingresso. Vale qui il mantra #7 — una risposta composta e sbagliata è
   peggio di due zone che tacciono.
2. **L'emergenza si misura, non si annuncia.** `PRINCIPLES.md` rifiuta il
   sistema che recita «sono cosciente» da un `printf`; rifiuta allo stesso modo
   il piano che dichiara «sono emerse capacità nuove». Una capacità non
   progettata esiste quando **risolve un compito che nessuno le ha insegnato**,
   con proof, e sopravvive all'ablazione dell'arco: togliere l'arco deve
   toglierla.

#### Predizione falsificabile

Censite le zone della KB e aggiunti **N archi** fra coppie che oggi non si
parlano, il numero di compiti risolti deve crescere **più che linearmente** in N
— perché ogni arco nuovo compone anche con quelli esistenti. Se cresce
linearmente, gli archi sono scorciatoie una per compito e non c'è unificazione;
se non cresce, le zone erano già connesse o l'arco è decorativo.

#### E la conseguenza sul metodo, che vale da subito

Prima di aggiungere una capacità, la domanda non è soltanto *«è generalizzabile
KB-first?»* (mantra #1). È anche:

> **chi altro deve accordarsi con questa, e su che cosa?**

Tre difetti su tre, in questa serie, sarebbero stati evitati facendosela prima.

---

## §18.44 — D39: un fatto sbagliato non si ripara aggiungendone uno giusto

*Registrato il 2026-09-01, chiudendo GD12.*

Il cassetto senza maniglia (§18.30) aveva finora una sola forma nota: parrot0
impara sotto un nome che poi non sa più pronunciare. GD12 ne ha mostrata una
**seconda, peggiore**, perché non lascia traccia di sé.

«il mio libro è sul tavolo» produceva `possession_name(libro, tavolo)`: il ramo
del possesso prendeva l'**ultima parola** della frase e la registrava come
*nome*, buttando via la preposizione. Cioè parrot0 imparava, con piena
convinzione e senza alcun segnale d'incertezza, che **il libro si chiama
«tavolo»**.

La differenza con tutti i casi precedenti è cruciale:

| | fatto **mancante** | fatto **sbagliato** |
|---|---|---|
| sintomo | muro, «non capisco ancora» | risposta sicura e falsa |
| tentazione | insegnare la forma mancante | insegnare *anche* la forma giusta |
| esito della tentazione | funziona | **due fatti in competizione**, e quello falso resta |
| cura vera | aggiungere | **togliere la rivendicazione** |

Un turno che risponde *bene* dopo la cura non dimostra che la cura è giusta: se
il fatto falso è ancora nella KB, si è solo costruito un secondo percorso che
per ora vince. La domanda da farsi non è «adesso risponde?» ma **«che cosa ha
davvero imparato?»** — e si guarda il fatto, non la risposta.

**La regola operativa, generalizzata oltre GD12.** Quando un ramo del C
costruisce un argomento con un'euristica posizionale (*l'ultima parola*, *la
prima parola*, *la parola dopo il verbo*), quel ramo **deve poter declinare**.
L'euristica è una scommessa; una scommessa senza possibilità di passare è una
rivendicazione. E la condizione per declinare è **conoscenza**, non una lista
nel C: qui è bastato chiedere alla KB se una preposizione di luogo apre il resto
della frase, e il ramo si è tolto di mezzo da solo lasciando lavorare la lettura
locativa che già esisteva.

### Il corollario sui ratchet

Il ratchet che GD11 aveva scritto **asseriva la resa sbagliata**: fissava
«Ricevuto: il tuo libro è tavolo» come comportamento atteso. Un test che
congela un difetto lo **difende** — e lo difende con l'autorità di un verde.

Quando la cura è giusta, il ratchet si aggiorna e si scrive **perché**; non si
annacqua la cura per non toccare il test. Il ratchet aggiornato prova ora la
classe che GD11 voleva davvero fissare (l'ack del possesso segue la lingua del
turno) **sulla forma che resta un possesso**, e aggiunge il contrasto: la cura
non ha travolto il caso legittimo.

### L'ambiguità trovata di rimbalzo

`ordinal_reference(it, "secondo", 2)`, introdotto in G3, catturava la
**preposizione** in «secondo Marco la terra è piatta». Due letture legittime
della stessa stringa, e la KB ne dichiarava una sola come se fosse l'unica.
Curata richiedendo il determinante per gli ordinali italiani ambigui, con
l'avviso sul posto: *alcune forme ordinali sono ambigue, e l'ambiguità non si
risolve qui.* È la lezione della sonda all'oracolo (§18.38) applicata a noi
stessi: **il posto dove si dichiara una forma non è il posto dove si sceglie fra
le sue letture.**

---

## §18.45 — D40: la struttura che esiste, ed è a digiuno

*Registrato il 2026-09-01, chiudendo G4/GD4 (l'ellissi e il riferimento).*

Fin qui il cassetto senza maniglia aveva due forme: il **fatto mancante**, che
fa muro, e il **fatto sbagliato** (D39), che risponde sicuro e falso. G4 ne ha
mostrata una terza, ed è la più difficile da vedere perché **non ha sintomi
propri**: la struttura giusta esiste, è documentata, è già consumata — e
nessuno la riempie.

`exchange/3` è in `kb/core/discourse.p0` dal gen58, con un commento che dice
esattamente che cos'è: *ciò che parrot0 ha DETTO*, distinto da ciò che sa. Tre
consumatori la leggono già — `told_about/2`, `covered_entity/1`, e la superficie
«su cosa mi hai risposto». Ma la popolavano soltanto le **letture dichiarative**:
una DOMANDA a cui si era risposto non lasciava alcuna traccia.

Il risultato è un inganno di secondo ordine. Tre capacità scritte e verdi in
lettura erano **morte per fame**, e nessun test le coglieva perché ciascuna
falliva restituendo il vuoto — che è indistinguibile da «non c'era niente da
dire». E l'ellissi sembrava richiedere un'infrastruttura da costruire, mentre
mancava solo il pasto.

**La regola:** prima di aggiungere una memoria del discorso, chiedersi se ce n'è
già una che nessuno nutre. Un secondo registro parallelo sarebbe stato più
rapido da scrivere e avrebbe creato due memorie destinate a divergere — cioè
avrebbe riprodotto il difetto ricorrente invece di curarlo.

### La quarta comparsa della stessa forma (D33, D35, D37 → D40)

*Due percorsi che devono accordarsi non condividono l'oggetto su cui accordarsi.*

Le due ellissi sono **duali**, e nessuna delle due lo sapeva:

| | soggetto | relazione |
|---|---|---|
| forma | «e quanti giocatori» | «e il secondo?» |
| che cosa manca | il soggetto | la relazione |
| che cosa c'è | la relazione | il referente |
| meccanismo | `topic_continue_resolve` (gen387) | eredità (G4) |

gen387 aveva già la guardia giusta — *«il residuo non deve già nominare
un'entità»* — ma la verificava solo per le entità **nominate**. «Il secondo» non
nomina: **riferisce**. Il residuo passava la guardia, il topic saliente veniva
appeso in coda, e nasceva `the second mensola`: un turno che poi qualcuno
rivendicava all'indietro, rispondendo con un OGGETTO a una domanda su un LUOGO.

La cura non è una guardia nuova accanto a quella esistente: è quella guardia con
l'estensione che le mancava, perché **riferire è un modo di nominare**. E
l'oggetto condiviso è `referring_surface/1` — una nozione sola, consultata da
entrambi i duali, dove prima ce n'erano zero e stavano per diventare due.

### Il vincolo GD4, tenuto

*Su turni multipli il riferimento si risolve o si dichiara ambiguo, mai si
sceglie in silenzio.* Un dimostrativo **nudo** non restringe nulla, e rispondere
comunque sarebbe inventare che cosa si intendeva:

```text
> E quello blu?   → Mensola.                                   (individua)
> E quello?       → Ce n'è più di uno — book red: table;
                    book blue: mensola. Quale intendi?         (dichiara)
```

Perché la seconda riga funzioni, un sintagma di **un token solo** deve poter
arrivare al risolutore: di norma non aggiunge nulla, ma il pro-forma nudo è
esattamente il caso in cui l'assenza di proprietà è l'informazione. Scartarlo
per economia lo faceva cadere in un muro, cioè trasformava un'ambiguità
dichiarabile in un fallimento muto.

### Il dimostrativo è vincolato dal discorso, non dalla KB

«quello blu» si cerca **fra i referenti già introdotti**, non fra tutte le
entità. Non è prudenza: è il significato della parola — «quello» vuol dire
*quello di cui stiamo parlando*. E per costruzione evita lo scan globale che era
già stato scartato altrove perché rendeva ogni turno lineare nella KB.
