# Missione Multi-Hop Deep Memory

> **In una riga.** parrot0 risolve un compito di ragionamento la cui risposta non sta
> né nel prompt né in una sola pagina: scopre che cosa gli manca, lo legge dalla sua
> memoria profonda (Wikipedia), lo integra, e da lì capisce che cosa leggere dopo.
> Nessun LLM, nessun motore di ricerca esterno, nessun aiuto nel prompt: soltanto la
> KB viva, la lettura e l'inferenza. Ogni passo è ispezionabile e porta la sua fonte.
>
> **Il luogo in cui tutto questo accade è la IR.** Il problema, ogni pagina letta, ogni
> lacuna, ogni candidato e ogni conclusione sono nodi e frame della stessa
> rappresentazione intermedia (`kb/core/input-structure.p0`), in scope diversi. Un hop non
> è «una ricerca»: è **un ruolo mancante in un frame della IR che una lettura riempie**.

Indice: [0. Stato](#0-stato-misurato-14-settembre-2026) ·
[1. Premessa](#1-premessa-che-cosè-la-memoria-profonda) ·
[2. Missione](#2-la-missione) · [3. Regole anti-inganno](#3-regole-anti-inganno) ·
[4. La IR è il ciclo](#4-la-ir-è-il-ciclo) ·
[5. Architettura](#5-architettura-kb-first-che-cosa-cè-che-cosa-manca) ·
[6. Progressione](#6-progressione-dm-1--dm-10) · [7. Esperimenti](#7-esperimenti) ·
[8. Traccia](#8-che-cosa-si-registra-per-ogni-hop) · [9. Metriche](#9-metriche) ·
[10. La demo](#10-la-demo) · [11. Criterio finale](#11-criterio-finale) ·
[12. Registro](#12-registro-delle-sessioni)

---

## 0. Stato misurato (14 settembre 2026)

Misurato in `make chat` con politica «look things up yourself» e sulle pagine vere in
edizione locale (`tests/fixtures/wiki/`, lead di Wikipedia con revisione).

| Prova | Risposta di oggi | Diagnosi |
|---|---|---|
| Prompt a passi di §7.2 (Karamazov → … → religione) | «A word that starts with "t": tab. … a close reading turns on the premise…» | **misclaim**: il compito a passi arriva a facoltà che non lo capiscono |
| Prompt della demo §7.1 (Zarathustra → … → divinità) | «Thus was a mysterious Thus. Then one day…» | **misclaim**: il generatore di storie prende il turno |
| «who wrote the brothers karamazov?» | «fyodor dostoevsky.» | hop 0 nella KB: sa |
| «what is thus spoke zarathustra?» | legge la pagina e dice il lead | la lettura c'è |
| «who is the protagonist of thus spoke zarathustra?» | «I don't know about protagonist.» | la relazione non si estrae dal testo letto |
| «what did zoroaster found?» | ripete la definizione letta | idem: «founder of Zoroastrianism» è nel lead e non diventa `founded/2` |
| «which deity do zoroastrians worship?» | lacuna su «zoroastrians» | nome del gruppo non collegato al topic |
| «who wrote thus spoke zarathustra?» | lacuna su «zarathustra» | la lacuna nomina una parola, non il titolo |

**Dove sono scritti gli anelli** (lead veri, revisioni del 14 settembre):

| Anello | Nel lead? | Frase |
|---|---|---|
| The Brothers Karamazov → Dostoevsky | sì | «…final novel by Russian author Fyodor Dostoevsky» |
| Dostoevsky → Nietzsche | **no** | solo nel corpo di *Friedrich Nietzsche*, e nel verso opposto: «Nietzsche called Dostoevsky "the only psychologist from whom I have anything to learn"» |
| Nietzsche → Thus Spoke Zarathustra | no (nel lead del libro, verso opposto) | «…written by German philosopher Friedrich Nietzsche» |
| Thus Spoke Zarathustra → Zoroaster | sì | «The protagonist is nominally the historical Zarathustra, more commonly called Zoroaster» |
| Zoroaster → Zoroastrianism | sì, **con distrattori** | «…the spiritual founder of Zoroastrianism» — nello stesso lead: Ancient Iranian religion, Judaism, Christianity, Islam |
| Zoroastrianism → Ahura Mazda | sì | «…deity known as Ahura Mazda, who is hailed as the supreme being» |

Conclusione operativa: la catena di §7.1 è percorribile **con i soli lead** e ha una
biforcazione vera (quale religione); la catena di §7.2 richiede due capacità in più:
leggere oltre il lead e cercare all'indietro («chi cita Dostoevskij?»).

---

## 1. Premessa: che cos'è la memoria profonda

parrot0 dispone di una **memoria profonda**: esterna, persistente, non parametrica, il cui
corpus è Wikipedia. Non è un information retrieval messo accanto al sistema: fornisce la
conoscenza che manca **nel momento in cui serve per proseguire un ragionamento**
(`kb/core/network.p0`, [la-rete-come-memoria-profonda](la-rete-come-memoria-profonda.md)).

Il ciclo fondamentale:

**reason → knowledge gap → recall → evidence → integrate → reason**

Tre spazi, distinti:

| Spazio | Che cosa | Dove vive oggi |
|---|---|---|
| **KB** | ciò che parrot0 conosce operativamente | `kb/core`, `kb/facts`, `kb/wiki`, `kb/learning` |
| **Working memory** | ciò che tiene durante il problema corrente | **la IR**: lo scope del problema e uno scope per ogni pagina letta, con i legami fra scope (§4) |
| **Memoria profonda** | ciò che può leggere da Wikipedia quando serve | la pagina, finché non è letta; letta, diventa uno scope della IR. In KB resta ciò che si conserva: `topic_read/2`, `topic_definition/2` (`kb/wiki/deep-memory.p0`) |

E cinque statuti epistemici, che ogni informazione della catena deve portare:

| Statuto | Significato | Come si dice oggi |
|---|---|---|
| **known** | nella KB prima del problema | fatto della base curata |
| **given** | detto dal problema | premessa (`problem_premises`, non si impara) |
| **inferred** | ottenuto per inferenza | «The conclusion is mine» (`derivation_answer/3`) |
| **recalled** | letto dalla memoria profonda | «I read it on Wikipedia: the article …, revision …» (`source_answer/3`) |
| **unknown** | non ancora disponibile | lacuna nominata (`turn_gap_phrase/2`, `pending_gap`) |

---

## 2. La missione

Dimostrare che parrot0 risolve problemi la cui soluzione **non è disponibile né nel prompt
né attraverso un singolo accesso a Wikipedia**, ma emerge da una sequenza di accessi
intercalati da ragionamento.

Non vogliamo dimostrare che parrot0 sa fare dieci ricerche. Vogliamo dimostrare che:

> **parrot0 risolve autonomamente un problema che richiede una catena di N accessi alla
> memoria profonda, individuando progressivamente che cosa deve sapere, recuperando
> l'evidenza, integrandola nella propria rappresentazione e usandola per decidere il
> passo successivo.**

Il comportamento desiderato:

```
Problema → ragionamento → lacuna₁ → lettura₁ → evidenza₁ → aggiornamento della IR
        → ragionamento → lacuna₂ → lettura₂ → evidenza₂ → … → inferenza finale → risposta
```

Ogni risultato intermedio deve poter diventare parte della rappresentazione con cui si
formula il bisogno successivo: l'entità trovata al passo *k* è il soggetto della lettura
al passo *k+1*.

---

## 3. Regole anti-inganno

Valgono i MANTRA, e in più per questa missione:

1. **Il prompt non dice come.** Niente «use your Deep Memory», niente «this requires 5
   hops», niente titoli di pagina da aprire. Il numero di hop è una proprietà nascosta,
   osservata nella valutazione.
2. **Le pagine sono vere.** Le prove deterministiche usano i lead reali di Wikipedia in
   edizione locale (`tests/fixtures/wiki/<topic>.txt`), mai testo scritto per il test.
   La demo dal vivo usa la rete. Stesso motore, stessa KB: cambia solo il provider.
3. **Nessun anello nella KB prima del test.** Se un anello è già un fatto della base, quel
   hop non conta come lettura (si registra come *known*). Le prove partono dalla KB viva
   completa, mai amputata.
4. **Nessun lettore per il problema.** Una relazione che serve a un hop («founder of»,
   «named after», «supreme being») è conoscenza generale del lettore di prosa, valida per
   ogni pagina, e si insegna parlando. Un pattern che vale solo per Zoroastro vale zero.
5. **Una risposta giusta con un passo sbagliato è un fallimento.** Si valuta la catena,
   non la parola finale (HCR, §9).
6. **Nessun «Imparato» dal prompt.** Le frasi del problema sono premesse, non conoscenza
   (la lezione dell'[esperimento età e incontri](../sessions/2026-09-14-esperimento-ordine-eta.md)).

---

## 4. La IR è il ciclo

### 4.1 Perché la IR non è un dettaglio

La tesi di F. del 12 settembre ([lettura-della-prosa](lettura-della-prosa.md) §0) vale qui
più che altrove: **una prosa si incamera una volta, in una rappresentazione che non sa a
che cosa servirà, e chi la lavora ne è un consumatore, non un secondo lettore della
stringa.** Un problema multi-hop è fatto di prose — il prompt e N pagine — che devono
**parlarsi**: l'entità trovata in una pagina è il soggetto della domanda alla pagina dopo.
Se ogni hop avesse il suo lettore, la catena sarebbe una sequenza di stringhe passate di
mano; con la IR è **una sola rappresentazione che cresce**.

Due prove dal 14 settembre che senza la IR si sbaglia:

- la base della demo («Thus was a mysterious Thus. Then one day…») è la stessa malattia
  M4 di `lettura-della-prosa.md` («Quipu was a mysterious Quipu…»), la cui diagnosi era:
  *nessun lettore ha consultato l'IR*;
- l'[esperimento età e incontri](../sessions/2026-09-14-esperimento-ordine-eta.md) ha
  funzionato con un lettore per posizioni di token parallelo alla IR, e §8 del rapporto
  misura il prezzo: tre varianti minime (un verbo, una dimensione, tre nomi) falliscono.

Una prima stesura di questo piano ripeteva l'errore — predicati ad hoc per le frasi del
lead, per le menzioni e per la traccia. Questa sezione lo corregge: **ogni capacità di §5 è
un consumatore o un produttore della IR, e nient'altro.**

### 4.2 Il ciclo, detto in IR

| Momento del ciclo | Che cosa è nella IR | Pezzi che esistono già |
|---|---|---|
| **Problema** | lo scope `problem` con i suoi nodi: entità nominate, descrizioni («a historical religious figure»), anafore («that figure», «its followers») | `input_node/4`, `input_entity_node`, `input_class_node`, `input_node_unresolved` |
| **Lacuna** | un frame con un ruolo **mancante**: `question, binary(named_after), roles(subject(protagonist_of(tsz)), object(missing))` | `input_semantic_frame(…, object(missing))`, `input_frame_gap`, `input_gap_node` |
| **Query** | la lettura si sceglie dal frame: il ruolo **legato** nomina la pagina (il soggetto noto), il ruolo **mancante** dice che cosa cercare in essa | `turn_gap_phrase/2`, `acquisition_move/1` (network.p0) |
| **Lettura** | la pagina diventa uno **scope** della IR (`page(zoroaster)`), segmentata in frasi e nodi con la stessa gerarchia del turno | `input_structure_publish(…, "last_text", …)` usato oggi per le frasi del turno composto |
| **Evidenza** | un frame **assertivo** nello scope della pagina che ha la stessa relazione e un soggetto coreferente: `assertion, binary(founder_of), roles(subject(zoroaster), object(zoroastrianism))` | `input_semantic_frame(…, assertion, …)`, `input_binary_assertion` |
| **Integrazione** | il ruolo mancante del frame del problema si **lega** al nodo della pagina: un legame fra scope, con la provenienza del nodo (articolo, revisione, frase, range) | `input_node_range`, `fact_source/3` — manca il legame cross-scope |
| **Candidati** | più nodi dello scope pagina compatibili col ruolo: restano **alternative** del legame, non fatti | `input_frame_set` / `input_frame_unique`, ambiguità dicibile (universal-input, «Quale? …») |
| **Prossima lacuna** | l'anafora del problema («that figure») ora si risolve al nodo legato: il frame successivo ha il soggetto e diventa interrogabile | spazio del discorso (`discourse_referent/2`), coreferenza |
| **Risposta e traccia** | il cammino dei legami, dal problema all'ultimo scope, reso in parti | `turn_response_part/3`, `source_answer/3`, `derivation_answer/3` |

**Conseguenza**: i sette campi di §8 non sono un registro da scrivere accanto al
ragionamento. **Sono una vista della IR.** Se il ragionamento è avvenuto nella IR, la
traccia si interroga; se bisogna scriverla a mano, il ragionamento è avvenuto altrove.

### 4.3 Che cosa manca alla IR per reggere il ciclo

| # | Mancanza nella IR | Perché blocca | Direzione |
|---|---|---|---|
| IR1 | **Scope di pagina**: una pagina letta non è pubblicata nella IR, se ne tengono la prima frase e i fatti estratti | l'evidenza non ha nodi, range, frasi a cui legarsi | la lettura pubblica il lead (poi le sezioni) in `page(Topic)` con lo stesso `input_structure_publish` delle frasi del turno |
| IR2 | **Legami fra scope**: un nodo del problema e un nodo di una pagina che sono la stessa cosa | senza, l'integrazione è una copia di stringa e la provenienza si perde | `input_bound(ScopeA, NodeA, ScopeB, NodeB, Evidence)` (arietà da impacchettare) come vista, prodotta dalla coreferenza |
| IR3 | **Frame con descrizioni come ruoli**: «the protagonist of X», «a historical religious figure», «its followers» | la lacuna del problema non è un'entità nominata ma una descrizione | le descrizioni come termini (la lezione di `problem-texts.p0`: «chiunque Bruno abbia incontrato…» come referente), dentro la IR |
| IR4 | **Scope persistente per il problema**: la IR di oggi è di turno (`turn_scoped/2`) | un problema multi-hop dura più letture, e forse più turni | scope numerati per problema, senza `retract` a metà risoluzione (limite noto) |
| IR5 | **Frame assertivi dalla prosa vera**: le relazioni del lettore (`extract_frame/2`) non tornano come frame della IR con i nodi | «spiritual founder of Zoroastrianism» diventa (quando diventa) un fatto, non un frame con range | unificare `P0FrameReading` e `input_semantic_frame`: una lettura, due usi (commit in KB, o frame nello scope) |
| IR6 | **Alternative**: più legami candidati per un ruolo, con evidenza per ciascuno | il branching senza promuovere un candidato a fatto | `input_frame_set` esteso ai legami; esclusione per evidenza (come `order-determinacy.p0`, ma sui legami) |

### 4.4 Prima misura su IR1 (14 settembre, sera): l'ingresso non è il collo di bottiglia

**Che cosa c'è già.** La lettura dalla rete (`network_acquire_passage` →
`learn_from_prose`) è un **secondo lettore di prosa**: conosce due forme (enumerazione e
appartenenza a una classe), mentre la conversazione ne legge 136, e non entra nella
Document IR. Il codice lo dice da solo (nota gen505z, «il lettore non usa i frame della
conversazione», verdetto non chiuso). Il lettore di «read:» (`read_passage`) invece
incamera il testo nella **Document IR** (SC1: `document_unit/3`, `document_unit_token/3`,
fonte e impronta) e prova ogni frase col protocollo della IR (`input_assertion_bundle`,
`input_frame_commit`) e poi con i moduli della conversazione. Esiste anche
`reading_intent(bridge, …)`: **leggere per rispondere a una domanda**, con le frasi
pertinenti per prime.

**Prototipo misurato** (tolto dopo la misura): la pagina letta dalla rete passata anche
per `read_passage`, con la fonte Wikipedia come documento.

| Pagina (lead vero) | Frasi lette nella IR | Esito |
|---|---|---|
| Zoroaster | 0 su 3 | «…becoming the spiritual founder of Zoroastrianism» non produce nessun frame |
| Danube | 1 su 7 | l'unico frame è **sbagliato**: «it was once a frontier of the Roman Empire» → `membership(was), roles(subject(river), class(empire))`; «…into the Black Sea» non produce nulla; un modulo risponde a una frase con i confini della Germania |

**Conclusione.** Unificare l'ingresso nella IR è necessario (un lettore solo, MANTRA #24)
ma **non fa guadagnare nessun hop**: la IR non sa ancora comprendere la frase
enciclopedica — apposizioni, participiali («becoming the founder of»), relative,
coordinazioni lunghe, preposizioni di direzione («into the Black Sea»). È lo stesso
risultato di `lettura-della-prosa.md` §1.3 (tardigrado 0/7, quipu 0/5). Il collo di
bottiglia della missione è **IR5**.

**Conseguenza per la strategia.** Due strade, non esclusive:

1. **Comprensione completa** di ogni frase del lead nella IR (IR5 pieno): è il lavoro di
   `lettura-della-prosa.md`, lungo, e utile a tutto parrot0.
2. **Lettura guidata dal frame aperto** — la mossa che fa una persona che cerca: il frame
   lacuna del problema (relazione cercata, tipo del ruolo mancante) sceglie nello scope
   della pagina le frasi pertinenti (`reading_intent`) e dentro quelle i **nodi candidati**
   del tipo giusto; la relazione si verifica sulla frase (superficie di relazione
   insegnabile) e il tipo leggendo il candidato (M5). Non richiede di capire tutta la
   frase, solo il pezzo che risponde alla domanda aperta. È interamente nella IR:
   consumatore dello scope pagina, produttore di legami (IR2) e di alternative (IR6).

La strada 2 è quella che serve alla classe del banco, ed è generale se le superfici di
relazione e i tipi sono conoscenza insegnabile e non una tabella per il banco. La strada
1 resta la destinazione, e ogni frase che la strada 2 legge bene è un caso per essa.

### 4.5 La lettura guidata dal frame aperto — prima tappa (14 settembre, sera)

`kb/core/guided-reading.p0` (decisioni) e `guided_reading_lead` in
`src/brain/50-self-research-loop.c` (meccanica). Il ciclo:

1. **Quando**: la domanda chiede un tipo («which religion», «what nationality», «into which
   sea») **e** ha un ponte (relativa, anafora, più tipi, «named after»), con la politica
   «leggi da solo». «what is the capital of France?» resta alla KB.
2. **Da dove**: il primo nome del prompt che ha una pagina; oppure un fatto della KB che il
   prompt interroga con la sua superficie (`answer_frame/2`: «the capital of Hungary» →
   Budapest, detto come *known*).
3. **Che cosa cercare**: il tipo chiesto, nell'ordine; gli indizi sono le altre parole piene
   del prompt, meno quelle del nome di partenza.
4. **Un candidato è del tipo** se: la testa del nome è il tipo («Black **Sea**»,
   «Ancient Iranian **religion**»); la KB lo sa (`known_entity_type/2`); la prima frase
   della sua pagina lo dice («is an Abrahamic **religion**»); la frase lo nomina («deity
   **known as** Ahura Mazda»); o una forma di risposta dichiarata per il tipo
   (`answer_shape/2`: il modificatore della classe per la nazionalità, il sintagma con la
   testa per «law»).
5. **Fra candidati**: indizi nella frase, prove a favore vicine (`cue_evidence/2`:
   «associated» ← «founder»), prove contrarie (`opposition_evidence/1`: «challenged»). Gli
   sconfitti si dicono, con la loro frase.
6. **Se il tipo non c'è**: la frase più pertinente sceglie la pagina ponte, e si continua.
7. **La risposta** è composta in KB (`guided_part/2`): risposta, pagine lette con titolo e
   revisione, frase di ogni anello, candidati scartati, e — se manca qualcosa — il tipo non
   trovato e gli indizi che nessuna pagina letta nomina.

**Misura sul banco: 0/8 → 5/8**, E1 23/23, nessun furto di turno sulle domande normali,
test della memoria profonda e `prose_triage` verdi.

⚠ **Debito dichiarato (MANTRA #24).** Frasi, menzioni e legami sono strutture locali del
ciclo e fatti di scratch, non ancora scope e nodi della Document IR. I gemelli sono scritti
in testa a `guided-reading.p0`; la migrazione è IR1+IR2+IR6. E il riconoscimento dei nomi
propri per maiuscole è una produzione ortografica che vale per le lingue che le usano.

**Seconda tappa, stessa notte — 5/8 → 7/8.** Quattro mosse generali:
- **i compiti all'imperativo** («identify / determine / name / find») aprono un tipo come
  un interrogativo, e un compito a passi è già un ponte; il tipo è la **testa** del
  sintagma («the supreme **deity**»), non la prima parola piena;
- **i tipi generici** («figure», «person», «one») li soddisfa l'entità che la frase
  pertinente nomina come ponte;
- **ciò che la KB sa del nodo raggiunto**: se il tipo chiesto ha una superficie di domanda
  (`answer_frame`: «capital»), la KB risponde prima della pagina, e il passo si dice *known*
  («known before reading: capital of Tanzania = Dodoma»);
- **partenza per ricerca**: se nessun nome ha una pagina e la KB non dà un fatto, si cerca
  la frase introdotta dal ponte («where the highest mountain in Africa stands» → *Mount
  Kilimanjaro*), con la ricerca di Wikipedia o, in edizione locale, risultati veri
  registrati (`tests/fixtures/wiki/search.tsv`).

E due correzioni nate da un errore vero del banco: C1 rispondeva «Fyodor Dostoevsky» come
filosofo (il suo lead lo chiama davvero *philosopher*) — ma nel prompt «**this** author» è
lui: un candidato presentato con un nome di ruolo che il prompt usa per un referente già
dato non può rispondere a un altro ruolo (`guided_anaphor_noun/2`); e «Russian philosopher»
non è un nome — un nome esteso col tipo è candidato solo se ha una pagina.

**Reperti.** (1) Dopo un turno italiano la lingua resta appiccicosa e il testo inglese del
turno successivo esce **corrotto** dalla canonicalizzazione, prima della pubblicazione
(«identify the historical figure he **figure founded**»): non è della lettura guidata, ma
tocca ogni lettore; nel banco B6 sta in fondo finché non è curato. (2) «figure» è
`stopword/1` nel lessico (per «figure out»): i tipi generici valgono come candidati di tipo
anche così. (3) La guardia della lettura guidata dentro le regole della prosa portata deve
chiedere prima la politica (economica): un `naf` incompleto su un paragrafo lungo
spegneva la lettura della prosa (`prose_triage` 74/75, curato).

**Terza tappa — 7/8 → 8/8, l'italiano.** Tutto generale e insegnabile: il confronto di
tipi e indizi passa anche per la traduzione della KB (`tr/2`: «mare» ↔ «sea», «fiume» ↔
«river»); un nome del prompt si cerca nella sua forma inglese («Ungheria» → hungary); la
testa del sintagma dipende dalla lingua (`compound_head_side/2`); gli articoli elisi si
staccano dal nome («dell'Ungheria») e non sono indizi; «che» dopo l'inizio è una relativa;
la risposta si dice nella lingua di chi chiede se la KB conosce il nome («Black Sea» →
«Mar Nero»). Tre nomi insegnati parlando e salvati: `tr(hungary, ungheria)`,
`tr(black_sea, mar_nero)`, `tr(danube, danubio)`. In `make chat` con la rete la stessa
domanda legge l'**edizione italiana** («Ungheria», it edition): l'edizione segue la lingua.

**Reperti della terza tappa.** (4) Un filtro meccanico del motore chiedeva la morfologia
solo a parole con le stesse prime tre lettere, e la traduzione non passava mai: i fatti di
traduzione si controllano sempre. (5) Artefatto del file di prova: `!set PARROT0_LANG`
dopo `!reset` fa ricaricare il cervello al turno dopo e cancella il provider locale
asserito — il blocco B6 cambia lingua prima del reset.

**Che cosa ha insegnato il banco nella stessa sera** (ognuno è una correzione generale, non
una patch per un problema): una verifica «contiene» nel test passava su una frase citata e
non sulla risposta (le attese ora sono «tipo: valore»); una menzione scavalcava la virgola
(«Midtown Manhattan, New York City»); le parole del nome di partenza sembravano indizi; un
nome seguito dal suo tipo è un nome solo; una riga oltre 512 byte cadeva in silenzio.

Il test di generalità di questa sezione è quello di `lettura-della-prosa.md`: **una forma
nuova di hop deve costare un consumatore della IR, mai uno scanner nuovo.**

---

## 5. Architettura KB-first: che cosa c'è, che cosa manca

### 5.1 Che cosa c'è già (e va riusato, non riscritto)

| Capacità | Dove | Serve a |
|---|---|---|
| Decidere di leggere (ask/act), leggere, ricordare con indirizzo e revisione | `network.p0`, `50-self-research-loop.c`, `deep-memory.p0` | ogni hop |
| Non leggere mentre si legge (`reading_now`) | `network.p0` | evitare letture a cascata non pertinenti |
| La lacuna nomina il sintagma, non una parola | `turn_gap_phrase/2` (grammar.p0) | formulare la query giusta |
| Definizione letta come risposta; «where did you read that?» | `mod_knowledge`, `source-questions.p0` | statuto *recalled* |
| Catene e premesse con provenienza, attenuazione | `causal-questions.p0` | statuto *inferred*, traccia |
| Testo con compito: premesse non imparate, lettura a stadi, risposta in parti | `problem-texts.p0`, `turn_response_part/3` | tenere il problema nella working memory e rispondere in più righe |
| Conversa di una relazione con particella («result from» → `causes`) | `converse_particle_reading/3` | leggere relazioni nel verso giusto |
| Determinatezza (necessario / possibile / indeterminato) | `order-determinacy.p0` | tenere aperte le ipotesi nel branching |

### 5.2 Che cosa manca, in ordine di dipendenza

Ogni riga è un produttore o un consumatore della IR (§4.3 dice che cosa manca alla IR stessa).

| # | Capacità | Perché serve | Nella IR |
|---|---|---|---|
| M1 | **Il compito a catena come atto** | oggi il prompt va a storie e letture ravvicinate | una forza del turno letta dallo scope `problem`: frame con ruoli `missing` collegati da anafore (IR3), non cue del prompt |
| M2 | **La pagina letta come scope** | oggi si tiene la prima frase e i fatti estratti | **IR1**: il lead pubblicato in `page(Topic)`, frasi e nodi con range |
| M3 | **Menzioni come candidati** | è il ponte fra una pagina e la successiva | nodi entità dello scope pagina (`input_entity_node`), non una regola sulle stringhe |
| M4 | **Relazione dell'hop dalla frase** | la risposta di un hop | **IR5**: frame assertivi dello scope pagina con la relazione del frame lacuna; superfici di relazione insegnabili parlando (`extract_frame/2`, `converse_particle_reading/3`) |
| M5 | **Verifica di tipo leggendo il candidato** | scegliere fra candidati, e non accettare un nome solo perché compare | la descrizione del ruolo (IR3) contro il frame di classe dello scope pagina del candidato |
| M6 | **Branching** | il lead di Zoroaster nomina cinque religioni | **IR6**: legami alternativi con evidenza, esclusione per evidenza |
| M7 | **La traccia e la risposta** | «reconstruct every step» | **vista** sui legami fra scope (IR2), resa con `turn_response_part/3` — nessun registro scritto a parte |
| M8 | **Oltre il lead** | Dostoevsky → Nietzsche sta nel corpo | più scope per pagina (sezioni), stessa pubblicazione |
| M9 | **Ricerca all'indietro** | «a philosopher who treated this author as a precursor» | la query è il frame con il **soggetto** mancante; il provider di ricerca restituisce pagine candidate, che diventano scope da verificare (M5) |

⚠ Lezioni dall'esperimento del 14 settembre che valgono qui — sulla meccanica, **non** sul
lettore, che era proprio ciò da non ripetere: le letture composte in KB si fanno **a stadi** (limite dei 384 legami), nessun `retract` a metà risoluzione, i fatti
del problema si **numerano**, `findall/3` raccoglie solo variabili, liste ≤ 512 byte
(`C_TODO.md`, «Limiti silenziosi del risolutore»).

---

## 6. Progressione DM-1 → DM-10

Il numero è la profondità **minima** della catena di letture necessarie.

| Livello | Che cosa dimostra | Capacità richieste |
|---|---|---|
| **DM-1** | una lacuna, una lettura, una relazione estratta dal lead | IR1, IR5; M2, M4 |
| **DM-3** | catena lineare su lead, relazioni eterogenee, una biforcazione | IR1–IR6; M1–M7 |
| **DM-5** | un anello fuori dal lead e uno all'indietro | + M8, M9 |
| **DM-7** | domini diversi (letteratura, filosofia, religione, storia) e più biforcazioni | stabilità di M6 |
| **DM-10** | catena lunga con relazioni tutte diverse, traccia completa | tutto, a costo del turno controllato |

Gli hop devono rappresentare **relazioni semanticamente diverse**, non
`persona → città → regione → stato → capitale`. Per esempio:
opera → autore → influenza intellettuale → filosofo → opera → personaggio → figura storica →
dottrina → testo → concetto.

### Lineare e ramificata

- **Lineare** — ogni hop ha un solo successore: `A → B → C → D`. Misura la profondità.
- **Ramificata** — un hop produce candidati: `A → B → {C₁, C₂}`, e un'evidenza successiva
  ne elimina alcuni: `{C₁, C₂} → evidenza → C₂`. Misura la capacità di **tenere
  l'incertezza senza trasformare un candidato in un fatto**: è il punto in cui parrot0 si
  distingue da un sistema che indovina.

---

## 7. Esperimenti

### 7.0 Il banco della classe — prima della cura (MANTRA #24)

F., 14 settembre: *«non vorrei fare la demo e sentirmi dire funziona solo se il caso è
questo: dobbiamo guadagnare la generalità, cioè che altri problemi simili siano risolti —
e per simili intendo della classe "ragiono, cerco, risolvo, tutto in maniera continua e
guidata dalla mia intelligenza"».*

Quindi la missione non ha un caso: ha un **banco**, scritto prima di qualunque cura, e
una capacità conta solo quando passa il banco. E1 (la demo) è uno dei suoi membri.

**Che cosa rende due problemi «della stessa classe»** — e dunque che cosa il banco deve
variare, perché nessuno di questi assi sia imparato come caso:

| Asse | Varia fra | Che cosa smaschera |
|---|---|---|
| dominio | letteratura, religione, geografia, arte, chimica, biografia | un lettore di dominio |
| relazioni degli hop | named after, founder of, supreme being, stands on, flows into, held in, located in, nationality, formulated | una tabella di relazioni per il problema |
| statuto degli anelli | tutti letti; known e recalled alternati | una catena che ignora la KB, o una KB che ignora la lettura |
| partenza | un titolo; una descrizione («the highest mountain in Africa»); una relazione della KB («the capital of Hungary») | un solo modo di trovare il primo nodo |
| forma del prompt | una domanda; due domande coordinate; passi imperativi | cue sulla forma del prompt |
| lingua | inglese; italiano su pagine inglesi | frasari per lingua |
| distrattori | religioni nello stesso lead; omonimi (Black Forest / Black Sea); luogo sbagliato con la stessa entità (Saint-Rémy / MoMA); la risposta intermedia (Budapest) | l'estrazione del primo nome che combacia |
| controllo | un anello che nessun lead contiene | l'invenzione |

**Il banco** — `tests/p0t/crossing/dm_class_bench.p0t`, pagine vere in edizione locale,
revisioni in `tests/fixtures/wiki/SOURCES.tsv`:

| # | Problema | Catena (K = known nella KB, R = da leggere) | Distrattori | Oggi |
|---|---|---|---|---|
| B1 | protagonista di *Thus Spoke Zarathustra* → religione → divinità | TSZ —R→ Zoroaster —R→ Zoroastrianism —R→ Ahura Mazda | 4 religioni nello stesso lead | «Thus was a mysterious Thus…» (**misclaim**) |
| B2 | in quale mare sfocia il fiume della capitale d'Ungheria | Hungary —K→ Budapest —K→ Danube —R→ Black Sea | Black Forest, Danube Delta, Volga | «Budapest.» (**misclaim**: la risposta intermedia) |
| B3 | in quale paese è il museo che conserva *The Starry Night* | Starry Night —R→ MoMA —R→ New York City —R→ United States | Saint-Rémy (dove fu dipinto, K) | lacuna su «starry night» |
| B4 | nazionalità e legge dello scienziato del mendelevio | Mendelevium —R→ Dmitri Mendeleev —R→ Russian, periodic law | — | «I couldn't read…» |
| B5 | capitale del paese della montagna più alta d'Africa | descrizione —K→ Kilimanjaro —R→ Tanzania —K→ Dodoma | Everest (la più alta del mondo) | «Mount Everest is the highest mountain in the world.» (**misclaim**) |
| B6 | B2 in italiano | come B2, pagine inglesi | Foresta Nera | «Non so ancora tradurre «sfocia»» |
| B7 | B1 a passi imperativi | come B1 | — | «I couldn't read…» a ogni passo |
| C1 | controllo: Karamazov → «un filosofo che lo considerò precursore» | l'anello Dostoevsky → Nietzsche non è in nessun lead | la religione di E1 | «A word that starts with "t": tab.» (**misclaim**) |

Base: **0 problemi su 8**, e 4 risposte su 8 sono sbagliate con sicurezza.

**Dopo la prima tappa della lettura guidata** (14 settembre, sera; §4.5): **5 su 8** —
B1, B2, B3, B4 e il controllo C1 — con catena, letture con revisione, passo noto dichiarato
e biforcazione detta.

**Dopo la seconda tappa** (notte): **7 su 8** — anche B5 (ricerca della descrizione →
Kilimanjaro → Tanzania → capitale nota) e B7 (lo stesso problema di B1 a passi imperativi).

**Dopo la terza tappa** (notte): **8 su 8** — anche B6, la stessa classe in italiano su
pagine inglesi: «Risposta: Mar Nero.», con la catena detta in italiano. Il primo
guadagno misurabile, prima ancora di una risposta giusta, è **zero misclaim**: un problema
della classe non va mai a storie, parole, letture ravvicinate o al passo intermedio.

**Banco di controllo (held-out)** — `tests/p0t/crossing/dm_class_heldout.p0t`, scritto *dopo*
l'8/8 e senza toccare il ciclo, verificando solo dove sono scritti gli anelli:

| # | Problema | Esito, prima di qualunque cura |
|---|---|---|
| H1 | valuta del paese dove sta il Taj Mahal | ✓ «rupee» (Taj Mahal → India letto → valuta nota) |
| H2 | nazionalità del compositore della *Carmen* | ✗ «I don't know about nationality» — la catena sta in «the X of the Y of Z», senza parola ponte: la lettura non si attiva |
| H3 | lingua del paese la cui capitale è Lima | ✓ «Spanish» |
| H4 | catena montuosa della montagna più alta d'Europa | ✗ «Mount Everest is the highest mountain in the world.» — di nuovo niente ponte, e torna il misclaim |
| H5 | H1 in italiano («quale moneta») | ✗ legge le pagine ma il tipo tradotto non incontra la valuta nota |
| H6 | H2 a passi imperativi | ✓ «French» |
| C2 | controllo: il lead del *Cairo* non nomina il Nilo | ✓ non lo inventa |

**4 su 7.** L'8/8 del banco della classe era in parte adattamento: la regola del ponte (una
parola: relativa, anafora, possessivo) copre le forme del banco ma non la catena di
complementi («the nationality of the composer of the opera Carmen») né una descrizione che
chiede un tipo diverso da quello che nomina («which range contains the highest mountain»).
È esattamente ciò che il criterio 3 serviva a far vedere. La prossima cura si misura sui
**due** banchi insieme, e il banco di controllo resta tale solo finché non si guarda:
quando sarà verde, se ne scrive un altro.

**Cura, misurata sui due banchi insieme** (notte): banco della classe **8/8**, banco di
controllo **4/7 → 7/7**. Quattro mosse generali, nessuna scritta per un problema:
- **il ponte come descrizione**: dopo il tipo chiesto, un sintagma col determinante la cui
  testa non è il tipo, seguito da una preposizione («the **composer** of», «the highest
  **mountain** in»), descrive un'entità intermedia (`guided_description_at/3`); «what is the
  capital of France?» resta diretta;
- **la ricerca parte dalla descrizione** quando non c'è parola ponte;
- **l'entità di partenza può essere già del tipo chiesto** («the capital of Hungary» è una
  città);
- **il tipo tradotto** arriva alla risposta nota attraverso la sua forma inglese
  («moneta» → currency → `currency_of_country`).

E due correzioni del riconoscimento: un verbo di relazione noto chiude il sintagma del
tipo («which mountain range **contains**»); «trova» non è un verbo di compito («si trova» è
«è situato»). Trappola ritrovata: la congiunzione «politica ∧ tipo ∧ ponte» in una regola
sola esauriva i 384 legami — il motore chiede i tre fatti separatamente. Lessico insegnato
parlando e salvato: `tr(currency, moneta)`, `tr(rupee, rupia)`, `tr(indian_rupee,
rupia_indiana)`.

⚠ **Il banco di controllo ora è visto.** Per il criterio 3 il prossimo passo non è una cura
ma un **terzo banco** scritto senza guardare il codice — e la prova dal vivo con la rete,
dove le pagine candidate sono molte di più di quelle locali.

**Criteri di generalità** (valgono per ogni mossa della missione):

1. **Nessuna cura si misura su un problema**: si misura sul banco intero, e si riporta il
   numero (problemi risolti, hop giusti, misclaim) prima e dopo.
2. **Nessuna parola del banco entra nella KB come regola per il banco.** Le relazioni
   degli hop («stands on», «flows into», «named after») entrano come superfici di
   relazione generali, insegnate parlando o lette, e valgono per ogni pagina.
3. **Il banco cresce prima della cura, non dopo.** Quando una mossa diventa verde su tutto,
   si aggiungono problemi nuovi (altri domini, DM-4, due biforcazioni) *prima* della mossa
   successiva, e li si guarda fallire.
4. **Una variante nuova si scrive senza guardare il codice**: verificando solo dove sono
   scritti gli anelli nelle pagine vere.
5. **La demo usa un problema scelto dal pubblico fra quelli del banco**, e uno nuovo
   costruito sul momento con lo stesso metodo: così il «funziona solo su questo» non si
   può dire.

### 7.1 E1 — DM-3, la demo: il nome del profeta di Nietzsche  *(primo test, in corso)*

**Prompt** (unico, niente istruzioni sul come):

> The protagonist of Thus Spoke Zarathustra is named after a historical religious figure.
> Which religion is traditionally associated with that figure, and which deity do its
> followers exalt as the supreme being? Reconstruct every step.

**Catena attesa** (non data a parrot0):

| Hop | Lacuna | Lettura | Evidenza (frase vera) | Relazione | Risultato |
|---|---|---|---|---|---|
| 1 | di chi porta il nome il protagonista? | *Thus Spoke Zarathustra* | «The protagonist is nominally the historical Zarathustra, more commonly called Zoroaster» | named_after | Zoroaster (candidati: Zarathustra = Zoroaster, West) |
| 1b | è una figura religiosa? | *Zoroaster* | «…was an Iranian religious reformer…» | verifica di tipo | sì |
| 2 | quale religione è associata a lui? | *Zoroaster* | «…becoming the spiritual founder of Zoroastrianism» | founder_of | Zoroastrianism — **scartate**: Ancient Iranian religion (la sfidò), Judaism, Christianity, Islam (influenza «with much scholarly controversy») |
| 3 | quale divinità esaltano i suoi fedeli? | *Zoroastrianism* | «…its adherents exalt an uncreated … deity known as Ahura Mazda, who is hailed as the supreme being» | supreme_deity | **Ahura Mazda** |

**Risposta attesa**: Zoroastrianism e Ahura Mazda, con i tre articoli letti e le loro
revisioni, lo statuto di ogni passo (*given* il nome del libro, *recalled* le tre frasi,
*inferred* la scelta fra i candidati) e i candidati scartati con il perché.

**Perché lascia il segno**: tre pagine lette in sequenza, ciascuna scelta da ciò che la
precedente ha detto; una biforcazione vera risolta con l'evidenza e dichiarata; nessun
modello linguistico; ogni frase citata con la revisione. E si può chiedere a ogni passo
«where did you read that?».

**La stessa catena, nella IR** (ciò che la prova dovrà poter interrogare, non solo leggere
nella risposta):

```text
scope problem:  F1 = question named_after(subject: protagonist_of(tsz), object: missing)
                F2 = question associated_religion(subject: ↑F1.object, object: missing)
                F3 = question supreme_deity(subject: ↑F2.object, object: missing)
lettura 1 → scope page(thus_spoke_zarathustra):  A1 = assertion named_after(protagonist, zoroaster)
            legame F1.object ← A1.object   [evidenza: frase 2, rev. 1373299517]
lettura 2 → scope page(zoroaster):  A2 = assertion founder_of(zoroaster, zoroastrianism)
            candidati F2.object: {zoroastrianism ✓ founder_of, ancient_iranian_religion ✗ challenged,
                                  judaism|christianity|islam ✗ influence "with much scholarly controversy"}
lettura 3 → scope page(zoroastrianism):  A3 = assertion supreme_deity(zoroastrianism, ahura_mazda)
            legame F3.object ← A3.object
risposta = cammino F1 → A1 → F2 → A2 → F3 → A3
```

**Prova**: `tests/p0t/crossing/dm3_zarathustra_chain.p0t` — pagine vere in edizione
locale. **Oggi è rossa**, e deve esserlo: rende visibili M1–M7. Le verifiche sulla IR
(scope di pagina, legami, alternative) si aggiungono con IR1–IR6.

### 7.2 E2 — DM-5: da Karamazov alla religione

> Begin with the author of *The Brothers Karamazov*. Identify a philosopher who treated
> this author as an important precursor to his own thought. Determine which major
> philosophical work by that philosopher introduces the figure of a prophet who announces
> the death of God. Identify the historical religious figure from whom the prophet takes
> his name. Finally, determine the ancient religion traditionally associated with that
> figure. Return the religion and reconstruct every step that led you to it.

Catena: The Brothers Karamazov → Fyodor Dostoevsky *(known)* → Friedrich Nietzsche *(M8+M9:
corpo della voce, verso opposto)* → Thus Spoke Zarathustra *(verso opposto: il libro nomina
l'autore)* → Zoroaster → Zoroastrianism. Hop semanticamente diversi: work → author, author →
influenza intellettuale, philosopher → work, figura letteraria → figura storica, figura →
tradizione. La relazione Dostoevskij–Nietzsche non è un attributo: è evidenza testuale da
interpretare («the only psychologist from whom I have anything to learn»).

### 7.3 E3 e oltre — DM-7, DM-10

Da costruire con lo stesso metodo: prima si verificano **dove** sono scritti gli anelli
(tabella di §0), poi si scrive il prompt. Candidati di dominio diverso: storia della
scienza (strumento → inventore → città → università → allievo), musica (opera → libretto →
fonte letteraria → autore → movimento). Nessun prompt si scrive prima di aver letto le
pagine vere.

---

## 8. Che cosa si registra per ogni hop

Per ogni esecuzione si deve poter ricostruire — **come vista interrogabile della IR**, non
come registro scritto a parte (§4.2):

1. **Lacuna** — quale informazione parrot0 ritiene mancante.
2. **Query** — che cosa decide di leggere, e perché quella pagina.
3. **Evidenza** — la frase letta, con articolo, edizione e revisione.
4. **Aggiornamento della IR** — il frame assertivo trovato nello scope della pagina, il
   legame che riempie il ruolo mancante del frame del problema, e i legami alternativi
   rimasti aperti. È il campo **centrale**: gli altri sei sono letture di questo.
5. **Inferenza** — quale conclusione diventa possibile; quali candidati cadono e perché.
6. **Prossimo obiettivo** — perché quella conclusione genera il bisogno successivo.
7. **Risposta** — la conclusione e la catena di evidenze che la sostiene.

Una risposta corretta ottenuta per caso non completa la missione.

---

## 9. Metriche

| Metrica | Definizione | Che cosa punisce |
|---|---|---|
| **DMHR(n)** — Deep Memory Hop Rate | problemi risolti con almeno *n* hop necessari / problemi di profondità *n* | la profondità che non regge |
| **HCR** — Hop Correctness Rate | hop corretti e giustificati / hop eseguiti | la risposta giusta per la strada sbagliata |
| **ARR** — Autonomous Recall Rate | letture attivate da una lacuna reale / letture totali, senza suggerimenti nel prompt | leggere a tappeto, o leggere perché il prompt lo dice |
| **BRR** — Branch Resolution Rate | biforcazioni risolte con evidenza citata / biforcazioni incontrate | promuovere un candidato a fatto senza prova |
| **Costo** | tempo del turno e letture per problema | una demo che non si può fare dal vivo |

Il banco (`tests/comprehension-probe/`-like, da creare in `tests/dm/`) tiene un file per
livello con prompt, pagine locali, catena attesa e punteggio per hop.

---

## 10. La demo

**Obiettivo**: far vedere in pochi minuti che parrot0 **non sa la risposta, capisce che cosa
gli manca, lo va a leggere, e ragiona su ciò che ha letto**, dicendo da dove viene ogni
passo.

Copione (dal vivo, `make chat`, rete accesa):

1. «what is Ahura Mazda?» → non lo sa (e lo dice): la risposta non è nella KB.
2. Il prompt di E1 → la risposta con la catena, le tre letture, i candidati scartati.
3. «where did you read that?» → articolo e revisione dell'ultimo anello.
4. «why not Christianity?» → la frase che la cita e perché non è l'associazione chiesta.
5. Lo stesso prompt con un'altra opera e un altro personaggio (variante del banco) → per
   mostrare che non è un copione.

**Piano B senza rete**: la stessa sessione con `topic_provider(fixture, …)` sulle pagine
vere salvate con la revisione; lo si dice al pubblico.

---

## 11. Criterio finale

Il traguardo **DM-10** non è avere fatto dieci letture. È che parrot0, partendo soltanto
dal problema, produca da solo

**reason → discover missing knowledge → recall from Wikipedia → validate evidence → update
representation → infer → discover next missing knowledge → … → final solution**

per una catena di almeno **10 hop necessari**, con la provenienza di ogni informazione
tracciabile.

> **parrot0 can autonomously traverse a ten-hop chain of heterogeneous knowledge
> dependencies using Wikipedia as Deep Memory, while preserving an explicit and inspectable
> reasoning state between successive recalls.**

A quel punto la memoria profonda non è «Wikipedia accessibile a parrot0»: è parte del
ciclo cognitivo. **parrot0 non deve possedere tutta la conoscenza prima di ragionare: deve
scoprire che cosa gli manca mentre ragiona, recuperarlo e continuare.**

---

## 12. Registro delle sessioni

- **14 settembre 2026** — piano riordinato ed espanso; stato misurato (§0); pagine vere dei
  sei anelli salvate in `tests/fixtures/wiki/`; primo test E1 scritto e rosso
  (`tests/p0t/crossing/dm3_zarathustra_chain.p0t`). Prossima mossa: M1 + M2 (il compito a
  catena non va più a storie; il lead intero nella working memory), poi M4 sull'hop 2, che
  è il primo anello leggibile con una relazione già a portata del lettore di prosa.
- **14 settembre 2026, dopo** — osservazione di F.: la IR era citata pochissimo. Aveva
  ragione: il piano proponeva predicati ad hoc per frasi, menzioni e traccia, cioè un
  lettore parallelo. Aggiunta §4 (la IR è il ciclo: lacuna = ruolo mancante di un frame,
  lettura = scope di pagina, integrazione = legame fra scope, traccia = vista), le mancanze
  della IR IR1–IR6, e M1–M9 riscritte come produttori e consumatori della IR. **La prossima
  mossa cambia**: prima IR1 (la pagina letta pubblicata come scope) e IR5 (i frame
  assertivi della prosa dentro la IR), perché senza di esse M1–M7 si costruirebbero di
  nuovo fuori dalla IR.
- **14 settembre 2026, sera** — F.: la generalità prima della demo. MANTRA #24 (un lettore
  fuori dalla IR è un'esplorazione con scadenza; la crescita si misura sulle varianti).
  Scritto il **banco della classe** (§7.0): 7 problemi e un controllo, domini, relazioni,
  lingue, forme e distrattori diversi, pagine vere con revisione. Base: 0/8 e 4 misclaim.
  Prossima mossa: IR1 (la pagina letta come scope della IR), misurata sul banco intero.
- **14 settembre 2026, sera (2)** — IR1 misurato con un prototipo (§4.4): il lettore della
  rete è un secondo lettore di prosa fuori dalla Document IR; farlo passare per
  `read_passage` legge 0/3 frasi di *Zoroaster* e 1/7 del *Danube* (e quella sbagliata).
  Il collo di bottiglia è IR5, la comprensione della frase enciclopedica. Prossima mossa:
  **lettura guidata dal frame aperto** (§4.4, strada 2), costruita nella IR e misurata sul
  banco; in parallelo, unificare l'ingresso (un lettore solo) quando la strada 2 lo
  richiede.
- **14 settembre 2026, notte** — prima tappa della lettura guidata dal frame aperto (§4.5):
  banco 0/8 → **5/8** (B1 Zarathustra, B2 Danubio con passo noto, B3 MoMA → New York → Stati
  Uniti, B4 Mendeleev, C1 controllo onesto), E1 23/23. Prossime: B5 (descrizione come
  partenza), B6 (italiano), B7 (passi imperativi), poi allargare il banco prima della mossa
  successiva (§7.0, criterio 3).
- **14 settembre 2026, notte (2)** — seconda tappa (§4.5): imperativi e teste di sintagma,
  tipi generici, risposte note dal nodo raggiunto, partenza per ricerca; esclusione dei
  referenti anaforici. Banco **7/8** (manca B6, italiano), E1 23/23, `prose_triage` 75,
  `order_determinacy_problem` 28, memoria profonda verde. Reperto: lingua appiccicosa che
  corrompe il turno inglese dopo uno italiano.
- **14 settembre 2026, notte (3)** — terza tappa: la stessa classe in italiano (traduzioni
  della KB nel confronto, forma inglese dei nomi, testa per lingua, articoli elisi,
  risposta nella lingua di chi chiede). Banco **8/8**. Prossima mossa, per il criterio 3 di
  §7.0: **allargare il banco** con problemi nuovi (altri domini, DM-4, due biforcazioni,
  un'altra lingua) *prima* di toccare il ciclo, e guardarli fallire; poi la prova dal vivo
  con la rete.
- **14 settembre 2026, notte (4)** — banco di controllo scritto dopo l'8/8, senza toccare il
  ciclo: **4/7**. Falliscono la catena di complementi senza parola ponte (H2), la
  descrizione che chiede un altro tipo (H4, con misclaim) e il tipo tradotto verso una
  risposta nota (H5). Registrato prima di curare.
- **14 settembre 2026, notte (5)** — cura sui due banchi: descrizione come ponte, ricerca
  dalla descrizione, entità di partenza già del tipo, tipo tradotto verso la risposta nota.
  Banco della classe **8/8**, banco di controllo **7/7**; E1, memoria profonda,
  `prose_triage`, `order_determinacy_problem` verdi. Prossimo: terzo banco non visto e prova
  dal vivo con la rete.
