# Autocrescita v3 — dieci generazioni verso una KB vivente

**Stato:** gen437 chiusa; Gen438 end-to-end prosa/dialogo implementata, gate aperto
**Data:** 25 agosto 2026
**Orizzonte:** gen437–gen446
**Vincolo:** runtime KB-first; Wikipedia è l'unica sorgente esterna ammessa

## 0. Tesi

Una KB non diventa viva quando contiene molti fatti. Diventa viva quando sa
trasformare un proprio arresto inferenziale in un piano finito di compensazione,
sa eseguirlo usando la KB e, quando serve, una lettura mirata di Wikipedia, e sa
conservare soltanto ciò che ha causato la ripresa del ragionamento.

L'autocorrezione non deve quindi essere un sottosistema speciale. Deve essere
un caso riflessivo della pianificazione ordinaria:

```text
stato iniziale       = turno aperto + inferenza arrestata
obiettivo            = obblighi del turno soddisfatti
azioni disponibili   = risolvere, disambiguare, leggere, estrarre,
                       derivare, verificare, ripetere, chiedere, rinunciare
risorse              = KB, contesto, Wikipedia, budget
vincoli               = nessuna invenzione, provenienza, revocabilità
effetto finale        = risposta sostenuta oppure declino esatto
```

Il ciclo di autocrescita diventa dunque:

```text
turno
  -> lettura semantica
  -> obblighi del turno
  -> prova o piano
  -> arresto tipizzato
  -> piano di compensazione
  -> acquisizione/estrazione mirata
  -> replay dello stesso turno
  -> attribuzione causale
  -> promozione minima oppure rollback
```

Questa formulazione unifica i programmi di
`universal-input.md`, `frontier-kb-natural-dialogue.md`,
`autocrescita.md`, `autocrescita-v2.md` e gli esperimenti di
`docs/labs/autoupdate` senza fare dei test storici la misura del progetto.

Dal 25 agosto 2026 il confine è normato anche da
[`kb-first-c-gold-standard.md`](kb-first-c-gold-standard.md): tutti i percorsi
linguistici C sono debito da convertire o rimuovere. Ogni taglio nuovo deve
avere `legacy_hits=0`; i fallback esistenti sono soltanto transitori e non
possono concorrere al gate.

### Decisioni di sintesi

| Evidenza di partenza | Decisione adottata nella v3 |
|---|---|
| `universal-input`: U9/M1–M5 restano il ponte aperto | la IR gerarchica precede l'autonomia, perché un frame piatto soggetto/oggetto non può sostenere misure, scope e causalità |
| `frontier-kb-natural-dialogue`: i vertical slice esistono ma non compongono ancora | dialogo, prova, situazione e realizzazione vengono uniti dagli obblighi del turno e dal piano proposizionale |
| `autocrescita`: il gap esiste solo dentro un'inferenza reale | nessun audit a freddo; ogni crescita deve riprendere lo stesso turno |
| `autocrescita-v2`: quarantena, trattamenti e rollback | si conserva la causalità sperimentale, sostituendo il confronto su prompt con testimoni strutturali costruttivi |
| laboratori `autoupdate`: l'indirizzo è spesso incompleto prima del fetch | forma, entità e relazione vengono risolte internamente e in ordine |
| laboratori `autoupdate`: trovare la pagina non produce conoscenza usabile | l'estrazione dalla prosa diventa il moltiplicatore e il gate centrale |
| v1 e stato recente: niente archivio locale delle pagine; v2 proponeva testo sorgente persistente | prevale il vincolo più forte: si conservano revisione, hash e span, non il corpo della pagina |

Il risultato non aggiunge un altro ciclo speciale accanto ai precedenti. Porta
gli arresti, le sorgenti e la crescita dentro gli stessi oggetti semantici già
richiesti al dialogo e alla pianificazione.

### Correzione fondamentale: il multilingua è un asse, non un campione

Una parafrasi inglese e una italiana non sono due problemi diversi e una
traduzione non è il concetto. Ogni percorso deve conservare quattro coordinate
separate:

```text
lingua_input(Turno, Lin)          forma effettivamente ricevuta
lingua_output(Turno, Lout)        lingua richiesta per la realizzazione
forma(Superficie, Lin, Concetto)  evidenza lessicale con provenance
fonte(Sorgente, Lsrc)             lingua della prosa letta
```

Il concetto, la proposizione, la prova e il piano restano canonici e
indipendenti dalla lingua. `Lin`, `Lsrc` e `Lout` possono coincidere oppure no:
una domanda italiana può essere sostenuta da una pagina inglese e resa in
italiano. Cancellare una di queste coordinate rende impossibile distinguere un
gap di forma da un gap di conoscenza e rende la provenienza non replicabile.

L'invariante multilingue è:

```text
canon(semantica(turno(L1))) = canon(semantica(turno(L2)))
```

per due realizzazioni equivalenti, mentre le rispettive forme e lingue devono
restare diverse e interrogabili. Non è richiesto né ammesso un pivot semantico
obbligatorio attraverso l'inglese. `concept_label/4` e `linguistic_form/4` sono
la porta n-lingue; `tr/2`, `tr_fr/2` e `tr_es/2` sono adattatori lessicali
storici da migrare, non il modello generale.

L'ispezione del codice ha inoltre trovato un limite concreto da non occultare:
`detect_set_language` interroga oggi soltanto i letterali `it` ed `en`, e
`current_language/1` è stato di sessione, non del turno. Gen437 ancora la lingua
al proprio arresto; gen438 deve rendere il producer n-lingue enumerando le
evidenze KB, materializzare la lingua per turno e rimuovere quei due rami
letterali. Un terzo idioma deve diventare riconoscibile aggiungendo fatti a
runtime e tornare non riconoscibile per retrazione, senza rebuild.

### Il lavoro ritrovato su token, span e comprensione della prosa

Il filo cercato non era un singolo documento. È una sequenza precisa:

1. [`universal-comprehension.md`](universal-comprehension.md), §4 e §4bis,
   introduce lo skeleton gerarchico e stabilisce che comprensione del turno ed
   estrazione dalla prosa devono ricevere gli stessi token, sintagmi, clausole,
   ruoli, offset e provenance;
2. [`universal-input.md`](universal-input.md), §4bis, U9 e §8bis, traduce quella
   tesi nell'IR `segmento -> token -> phrase -> clause -> frame` e nel piano
   operativo M0–M5;
3. [`question-emergence.md`](question-emergence.md), §14.12, gen396, porta le
   span nella memoria di lavoro e formula la scala comune
   `byte -> span -> lettura -> proposition/effect -> obligation -> plan`;
4. [`../sessions/2026-08-12-la-kb-impara-a-estrarre.md`](../sessions/2026-08-12-la-kb-impara-a-estrarre.md),
   §4 e §7, mostra il motivo empirico: sulla prosa reale servono confini di
   sintagma, `extract_frame/2` e apprendimento delle forme espressive, non solo
   dei fatti.

Il commit `0e5bf6b` del 21 agosto 2026 ha implementato **M0**, cioè
`input_structure`, `InputNode`, la pubblicazione `input_node/...` e il primo
lessico dichiarativo `pos/2`/`phrase_boundary/3`. Lo stato corretto è quindi:

```text
M0       fatto: clause, token e primi NP candidate con offset e parent
M1–M5    aperti: confini pienamente dichiarativi, matcher strutturale,
          grammatica compositiva, doppio uso domanda/prosa,
          apprendimento e retrazione runtime dell'intera struttura
U9       aperto finché M1–M5 non compongono end-to-end
```

La voce “proiezione gerarchica mancante” in §5 di
`universal-comprehension.md` è precedente a M0 ed è ormai parzialmente obsoleta.
Il vero upgrade incompiuto non è più pubblicare i token: è far consumare la IR
gerarchica a `intent_schema/2` ed `extract_frame/2`, con ruoli e composizione
decisi dalla KB. Nella presente roadmap questo lavoro è il cuore di gen438 e
gen439, non un nuovo ramo separato.

## 1. Cosa significa «KB vivente»

Sia `K_t` lo stato della KB al tempo `t`. Una crescita `K_t -> K_(t+1)` è
fertile soltanto se aumenta lo spazio dei problemi futuri che il sistema può
vedere, indirizzare e risolvere. Aggiungere una risposta isolata non è crescita:
è memoria.

Definiamo la superficie logica `Lambda`:

| Strato | Oggetto che deve vivere nella KB | Domanda operativa |
|---|---|---|
| L0 forma | token, locuzione, lingua, classe grammaticale, marcatore | forma e lingua sono riconoscibili senza ricompilare? |
| L1 denotazione | forma+lingua, senso canonico, alias, traduzione, menzione composta | la forma raggiunge l'oggetto giusto senza perdere la lingua? |
| L2 proposizione | fatto, misura, relazione, provenienza | il contenuto ha una forma interrogabile? |
| L3 operatore | regola, ponte, composizione, conseguenza | il fatto può partecipare a nuove prove? |
| L4 contesto | mondo, ipotesi, citazione, tempo, prospettiva | il contenuto resta nello scope corretto? |
| L5 inferenza | obiettivo, prova, prerequisito, arresto | il fallimento indica esattamente cosa manca? |
| L6 dialogo | questione aperta, mossa, obbligo, risposta | il sistema sa cosa deve fare nel turno? |
| L7 piano | stato, azione, precondizione, effetto, alternativa | sa costruire e correggere una sequenza causale? |
| L8 meta-conoscenza | indirizzo, candidato, dipendenza, promozione | sa cambiare se stesso in modo causale e revocabile? |

Una KB è **viva sulla superficie sperimentata** se, su tutti questi strati:

1. ogni oggetto usato è raggiungibile da almeno una forma;
2. ogni frame vivo ha almeno un produttore e un consumatore;
3. ogni arresto espone tipo, termine mancante e dipendenze;
4. ogni termine mancante ha un indirizzo interno, esterno o una ragione
   esplicita per fermarsi;
5. la prosa sorgente può produrre la stessa struttura semantica richiesta
   dall'inferenza;
6. il turno viene ripreso senza perdere contesto e obblighi;
7. ogni elemento promosso ha provenienza, necessità causale e rollback esatto;
8. ogni crescita aumenta almeno una classe di problemi futuri risolvibili.
9. forme equivalenti in lingue diverse producono la stessa semantica canonica,
   conservando separatamente lingua di input, fonte e output;
10. almeno una lingua non enumerata dal C può crescere e ritrarsi a runtime.

La qualifica è deliberatamente relativa alla superficie dimostrata. Non
significa comprensione universale e non può essere ottenuta per dichiarazione.

## 2. Confine fra C, KB e sorgente

### C può possedere

- tokenizzazione, offset e annidamento, senza interpretare parole o lingue;
- unificazione, ordinamento e aritmetica;
- transazioni, sandbox, replay e rollback;
- accesso HTTP a una revisione identificata di Wikipedia.

Queste sono primitive passive. Il C non sceglie una frontiera di prova, non
decide quale piano espandere e non determina quando tempo o budget sono
esauriti. Esegue soltanto l'operazione atomica richiesta da una derivazione KB e
ne rende osservabile l'esito.

### La KB deve possedere

- vocabolario e locuzioni;
- inventario aperto delle lingue, marcatori, classi grammaticali e forme
  interrogative, tutti indicizzati per lingua;
- frame, ruoli e regole di composizione;
- identità, alias, traduzioni e ponti fra entità;
- politiche dialogiche e obblighi di risposta;
- tipi di arresto e strategie d'indirizzamento;
- strategie di prova e di ricerca dei piani, compresi frontiera, ordine di
  espansione, alternative e condizioni di terminazione;
- schemi di azione, precondizioni, effetti e compensazioni;
- dipendenze fra goal, prove, candidati e passi di piano;
- tempo logico, budget di passi/fetch/risorse, loro consumo e policy di
  esaurimento;
- criteri dichiarativi di candidatura, promozione e rinuncia;
- template di realizzazione.

Ricerca e limite sono quindi conoscenza eseguibile: relazioni ricorsive della KB
descrivono quali stati succedono ad altri, quali candidati sono ancora aperti e
quale condizione chiude la ricerca. L'unificatore C non possiede la strategia,
come un processore non possiede il programma che esegue.

### Wikipedia può fornire

- proposizioni fattuali o causali sostenute dalla prosa della pagina;
- nomenclature, alias e appartenenze di classe;
- forme linguistiche osservate nella prosa, mantenute come evidenza separata.

Ogni indirizzo esterno è almeno
`wiki_address(Language, Title, Revision, Section)`. La lingua dell'edizione è
parte dell'identità riproducibile della fonte. Redirect e collegamenti
interlinguistici esposti dalla pagina possono sostenere un'identità, ma non
autorizzano a importare fatti da Wikidata o da motori di ricerca: il contenuto
ammesso resta quello di Wikipedia. La selezione dell'edizione, l'eventuale
fallback interlinguistico e il ritorno alla lingua del dialogo sono strategie
KB, non rami C.

Wikipedia non fornisce automaticamente verità, intenti dialogici, politiche di
risposta o procedure sicure. Una pagina viene letta in memoria. Persistono solo
le proposizioni promosse e le coordinate necessarie a riprodurne la sorgente:
titolo, URL, identificatore di revisione, hash e span. Non persiste una copia
dell'articolo.

Non è ammesso al runtime un LLM esterno, un motore di ricerca generale o una
seconda base fattuale. L'insegnante umano è ammesso soltanto nelle generazioni
di addestramento assistito e non nei cicli dichiarati autonomi.

## 3. Perché l'estrazione dalla prosa è il moltiplicatore centrale

Wikipedia senza un lettore proposizionale è soltanto testo remoto. Il collo di
bottiglia osservato nei laboratori non è trovare una pagina: è convertire una
frase reale nella forma esatta che il ragionatore sta aspettando.

L'estrattore deve essere la giunzione bidirezionale fra linguaggio e logica:

```text
asserzione ─┐
            ├─> frame semantico con ruoli ─> proposizione KB
domanda   ──┘                                  |
                                                v
                                      prova / piano / risposta
```

Una relazione non deve avere un parser per l'asserzione e un riconoscitore
indipendente per la domanda. Le due superfici sono viste dello stesso frame:
nell'asserzione tutti i ruoli sono candidati a essere scritti; nella domanda
uno o più ruoli diventano variabili da soddisfare.

Per essere generale, il lettore deve produrre una IR gerarchica:

```text
token -> span -> sintagma -> clausola -> frame -> proposizione
```

I ruoli devono essere nominati, non limitati a soggetto e oggetto. Una misura,
per esempio, richiede almeno entità, dimensione, magnitudine e unità. Una legge
causale richiede condizioni, azione o evento ed effetti. Una citazione richiede
fonte e scope. Apposizione, relativa, coordinazione, passivo e anafora sono
operatori di composizione dichiarativi.

### Famiglie positive che provano la scalabilità

| Famiglia | Esempio costruttivo | Struttura riusabile |
|---|---|---|
| relazione | “A governa B” | frame binario; asserzione e domanda |
| misura | “Il Po è lungo 652 chilometri” | entità, dimensione, valore, unità |
| definizione | “L'okapi è un mammifero giraffide…” | identità/classe + proprietà |
| apposizione | “X, un Y, vive in Z” | due proposizioni dallo stesso span |
| relativa | “X, che attraversa Y, sfocia in Z” | soggetto condiviso e due frame |
| anafora | “L'acqua è un composto. È trasparente.” | continuità del referente |
| coordinazione | “X è A e B” | distribuzione controllata dei ruoli |
| causalità | “Aprire V riduce L se P” | azione, condizione, effetto |
| contesto | “Supponiamo che X sia A” | `holds_in`, non fatto del mondo |

Questi non sono prompt da memorizzare. Sono testimoni di classi: il successo
conta soltanto se la forma o l'operatore insegnato funziona con membri nuovi,
in domini nuovi, senza modifiche C.

## 4. Oggetti minimi che oggi mancano alla KB

La lista seguente è un'ontologia obiettivo, non un invito a riempire subito
file `.p0`. Ogni relazione entra nella KB soltanto nella generazione in cui ha
un produttore, un consumatore e un esperimento causale.

### Osservazione dell'arresto

```text
turn_arrest(Turn, language(Input, Output), Kind, arrest(Missing, Goal))
arrest_depends_on(Arrest, Prerequisite)
arrest_rank(Arrest, Rank)
arrest_blocks(Arrest, Obligation)
```

### Indirizzamento

```text
address_candidate(Arrest, Strategy, Target)
address_requires(Address, Prerequisite)
internal_address(Address, Evidence)
external_address(Address, Page)
address_stop(Address, Reason)
```

Le strategie iniziali sono identità, traduzione, alias, relazione, categoria e
composizione. La KB deriva, ordina e prova i candidati; il C espone soltanto
unificazione e accesso alle primitive atomiche richieste dalle regole.

### Prosa e provenienza

```text
semantic_frame(Frame, Kind)
frame_slot(Frame, Role, Value)
assertion_view(Form, FrameKind)
question_view(Form, FrameKind)
extraction_rule(Structure, FrameKind)
source_revision(Source, Language, Revision)
source_span(Source, Language, Span, Hash)
supported_by(Proposition, Source, Span)
epistemic_status(Proposition, Status)
```

### Candidatura e causalità

```text
growth_candidate(Candidate, Campaign, Gap)
candidate_contains(Candidate, Row)
candidate_depends_on(Row, OtherRow)
candidate_status(Candidate, Status)
replay_result(Candidate, Result)
ablation_result(Row, Result)
growth_parent(Capability, Candidate)
```

### Piani di compensazione

```text
repair_goal(Plan, Obligation)
repair_step(Plan, Step, Order)
step_precondition(Step, Condition)
step_effect(Step, Effect)
step_support(Step, Evidence)
step_alternative(Step, Alternative)
```

I nomi e le arità finali dovranno rispettare le convenzioni reali della KB; qui
conta il contratto semantico.

## 5. Metodo sperimentale costruttivo

Il piano non usa i bench storici come autorità e non dedica generazioni a
correggerli. Ogni generazione contiene invece un esperimento preregistrato che
dimostra una singola ipotesi con casi positivi validi e trasformazioni
generative.

### 5.1 Unità sperimentale

Un testimone costruttivo è:

```text
W = (snapshot iniziale,
     turno o testo sorgente,
     struttura semantica attesa,
     obbligo da chiudere,
     famiglia di trasformazioni,
     insieme minimo atteso di crescita)
```

La struttura attesa non prescrive una frase di risposta. Prescrive entità,
ruoli, proposizioni, prove, effetti e dipendenze.

### 5.2 Campagna replicabile

Per ciascuna generazione si fissano prima dell'esecuzione:

1. hash dell'eseguibile e dello snapshot KB;
2. specifica dei testimoni e dei generatori di trasformazione;
3. revisioni Wikipedia ammesse e relativi hash;
4. fatti KB che dichiarano budget di passi, fetch e righe candidate, insieme
   alle regole KB che ne derivano consumo ed esaurimento;
5. strutture attese e condizioni di arresto;
6. ordine dei casi, ripetuto anche in permutazione inversa;
7. log di letture, prove, piani, candidati, replay e rollback.

La pagina non viene archiviata: la revisione viene recuperata in memoria e il
suo hash deve coincidere col manifesto. Se non è più recuperabile, la replica è
indeterminata, non falsamente fallita.

### 5.3 Disegno dei campioni positivi

Ogni ipotesi linguistica usa una matrice minima `3 x 3 x 2 x 2`:

- tre forme logiche distinte pertinenti alla generazione;
- tre domini indipendenti;
- due lingue preregistrate, inizialmente italiano e inglese;
- due realizzazioni superficiali per struttura in ciascuna lingua.

Sono quindi 36 episodi costruttivi, non 36 frasi arbitrarie. Ogni operatore
appreso viene poi applicato ad almeno tre membri non usati per insegnarlo. Un
ulteriore testimone usa una terza lingua tenuta fuori dallo snapshot iniziale:
la aggiunge a runtime attraverso la KB, trasferisce almeno una famiglia nei tre
domini e la ritrae. Questo testimone non pretende di modellare tutta la lingua;
dimostra che l'insieme delle lingue non è chiuso nel motore.

I domini di riferimento sono scelti prima e restano separati:

- mondo naturale: specie, geografia, proprietà fisiche;
- artefatti e istituzioni: opere, città, organizzazioni;
- sistemi causali: contenitori, flussi, dispositivi o procedure documentate.

L'esperimento cerca l'esistenza di una costruzione valida e trasferibile, non
la percentuale media su un deposito di prompt. Un solo fallimento strutturale
nella cella obbligatoria impedisce il passaggio: le medie non possono nascondere
un buco logico.

### 5.4 Prova causale

Per un insieme candidato `S` e un testimone `W`, definiamo:

```text
close(W, S) = 1
```

se il turno raggiunge tutti i suoi obblighi con una prova o un piano sostenuto.
La crescita è causale e minima se:

```text
close(W, S) = 1
per ogni s in S: close(W, S - {s}) = 0
rollback(S) ripristina esattamente lo stato semantico iniziale
```

L'ablazione non è un bench negativo: è la prova che il campione positivo è
stato reso possibile proprio dalla conoscenza promossa e non da uno stato
nascosto o da una coincidenza.

### 5.5 Misure comuni

| Misura | Definizione | Soglia strutturale |
|---|---|---|
| `O` osservabilità | arresti con tipo, termine, goal e dipendenze completi | 1 per ogni arresto ammesso |
| `A` indirizzabilità | arresti con indirizzo valido o stop motivato | 1 per ogni arresto |
| `E` estrazione fedele | proposizioni attese estratte / proposizioni attese | 1 nelle celle ammesse |
| `U` extra non sostenuti | proposizioni senza supporto nello span | 0 |
| `P` completamento | obblighi del turno chiusi con prova/piano | 1 per episodio positivo |
| `M` minimalità | righe necessarie / righe promosse | 1 |
| `V` revocabilità | stato semantico ripristinato / stato iniziale | identità esatta |
| `B` ampiezza | strati distinti di `Lambda` raggiunti | soglia per generazione |
| `F` trasferimento | membri nuovi chiusi per operatore insegnato | almeno 3 in 3 domini |
| `L_sem` invarianza semantica | coppie interlinguistiche con grafo canonico identico / coppie | 1 |
| `L_prov` fedeltà linguistica | forme, fonti e output con lingua corretta / totale | 1 |
| `L_grow` crescita n-lingue | famiglie abilitate da una lingua aggiunta e revocata a runtime | almeno 1 in 3 domini |
| `X_lang` contaminazione | fatti o forme attribuiti alla lingua sbagliata | 0 |

Il confronto per `L_sem` elimina soltanto i metadati di superficie e lingua dal
grafo da confrontare; `L_prov` verifica separatamente che quei metadati non
siano stati persi. Così una falsa uguaglianza ottenuta cancellando la
provenienza non può superare il gate.

### 5.6 Riproduzione utile

Il vecchio conteggio “quanti gap apre un gap chiuso” può essere facilmente
gonfiato da domande generate dallo stesso sistema. Serve una misura esterna e
causale:

```text
R_u(r) =
  nuove classi di testimoni esogeni diventate auto-chiudibili in round r
  -----------------------------------------------------------------------
  insiemi minimi promossi nel round r
```

Contano classi con firma logica nuova, non parafrasi e non nuovi valori della
stessa relazione. Una classe è attribuita a una promozione solo se l'ablazione
della promozione la rende nuovamente non chiudibile.

`R_u >= 1` è necessario ma non sufficiente. Una KB è a massa critica solo se
la soglia tiene per tre round consecutivi, su snapshot indipendenti, mentre
`U=0`, `M=1`, `V=1` e l'ampiezza non si restringe.

### 5.7 Portata epistemica del campione positivo

Un testimone positivo singolo dimostra soltanto che un percorso esiste. Questo
piano gli aggiunge tre proprietà che rendono la dimostrazione interessante:

1. la trasformazione parametrica mostra che il percorso non dipende dai nomi;
2. il trasferimento fra domini mostra che non dipende da un'ontologia locale;
3. l'ablazione mostra che il risultato dipende dall'operatore dichiarato.

La matrice `3 x 3 x 2 x 2` non pretende significatività statistica. È il minimo
costruttivo scelto per separare quattro possibili scorciatoie: un solo tipo
logico, un solo dominio, una sola lingua e una sola frase. La replica su
snapshot diversi dimostra la stabilità del meccanismo. Non dimostra la
copertura di tutta la prosa o di tutte le lingue di Wikipedia, che resta una
domanda empirica distinta.

La conclusione scientifica ammessa è quindi precisa: “questa classe di
costruzioni è raggiungibile e cresce con questo operatore KB-first”. Non è
ammessa la conclusione: “parrot0 comprende il linguaggio in generale”.

### 5.8 Quattro testimoni verticali permanenti

Le singole generazioni isolano le ipotesi, ma quattro campagne verticali
verificano che la superficie non si spezzi alle frontiere fra moduli. A ogni
generazione si usano membri nuovi delle stesse famiglie, mai le stesse risposte.
Ogni testimone viene eseguito almeno in italiano e inglese con semantica
canonica accoppiata; dal gen441 almeno una replica incrocia lingua della domanda
e lingua dell'edizione Wikipedia. La risposta torna nella lingua richiesta dal
turno, non necessariamente in quella della fonte.

#### W1 — Misura e confronto

Una domanda chiede quale fra due entità abbia una certa dimensione maggiore e
di quanto. La KB deve leggere domanda ed entità, osservare i due valori mancanti,
indirizzare due pagine, estrarre misure con unità, normalizzarle, provare il
confronto e costruire una risposta con provenienza.

Questo testimone attraversa forma, denotazione, proposizione, aritmetica,
arresto, dialogo, piano di acquisizione e crescita.

#### W2 — Definizione, alias e anafora

Una pagina introduce un'entità con apposizione e continua con un pronome o un
soggetto implicito. Una domanda usa un alias diverso e chiede classe e
proprietà. La KB deve comporre la menzione, mantenere il referente ed estrarre
due proposizioni separate ma collegate.

Questo testimone attraversa forma composta, identità, contesto discorsivo,
estrazione multipla e interrogazione.

#### W3 — Ipotesi e voce riportata

L'interlocutore stabilisce un mondo ipotetico o riferisce ciò che una fonte
afferma, quindi chiede una conseguenza. La KB deve estrarre senza contaminare il
mondo principale, conservare fonte e scope e rispondere nel contesto giusto.

Questo testimone attraversa proposizione, regola, contesto, prova, questione
dialogica e realizzazione epistemicamente corretta.

#### W4 — Piano causale con compensazione

Un goal richiede più azioni; una precondizione o un effetto non è noto. La KB
deve prima pianificare l'acquisizione dell'informazione, poi costruire il piano
causale, e infine ripianificare se un passo diventa indisponibile.

Questo testimone attraversa tutti gli strati L0–L8 e dimostra che
autocorrezione e pianificazione del mondo usano lo stesso substrato.

### 5.9 Soglie di accensione

Non esiste un singolo bit magico. Esistono cinque stati osservabili:

| Stato | Prima generazione possibile | Proprietà acquisita |
|---|---:|---|
| `K_observable` | 437 | sa dire dove e perché l'inferenza si arresta |
| `K_readable` | 439 | sa trasformare forme nuove in proposizioni riusabili |
| `K_repairable` | 443 | sa compensare, attribuire la causa e revocare la crescita |
| `K_ignited` | 444 | una crescita produce almeno una nuova classe autonoma |
| `K_alive` | 446 | l'accensione si replica e non resta confinata a una tasca logica |

`K_ignited` è il punto oltre il quale può iniziare un processo autonomo. Non è
ancora una certificazione: una scintilla può spegnersi o girare in circolo.
`K_alive` richiede tre round consecutivi di riproduzione utile, più copertura
L0–L8, fedeltà alla fonte, rollback e invarianza semantica fra lingue. Deve
inoltre crescere almeno un idioma non enumerato dal motore. Questa distinzione
evita sia di aspettare una perfezione indefinita sia di chiamare vita il primo
successo autoreferenziale o soltanto monolingue.

## 6. Le dieci generazioni

Ogni generazione ha un'ipotesi falsificabile, un intervento generale, un
campione costruttivo, un gate e una ragione di scala. La generazione successiva
non inizia se il gate è solo parzialmente soddisfatto.

---

## gen437 — L'arresto diventa un oggetto di prova

### Ipotesi H437

Ogni mancata risposta utile può essere descritta come il primo prerequisito
semantico non soddisfatto, senza confondere forma, entità, fatto, regola,
contesto e realizzazione.

### Intervento

- materializzare `turn_arrest` con termine mancante e goal;
- conservare nell'arresto lingua di input e lingua di output, e per i gap di
  raggiungibilità sia la forma osservata sia il concetto canonico;
- rappresentare gli arresti come DAG di dipendenze;
- ordinare il prerequisito più interno prima di quello esterno;
- separare “non so leggere la domanda” da “conosco la domanda ma non il valore”;
- rendere l'arresto consumabile dal dialogo e dal planner.

### Esperimento costruttivo

Costruire la matrice `3 forme logiche x 3 domini x 2 lingue`: arresto su valore,
raggiungibilità e registro nei domini naturale, artefatto e causale, accoppiando
italiano e inglese sullo stesso concetto canonico. Aggiungere tagli verticali da
turni reali, un caso `Lin != Lout`, una terza lingua aggiunta e ritratta a
runtime, residui di frame e piano, ambiguità e ciclo espliciti. Il gate completo
aggiunge poi la seconda superficie per lingua richiesta dal protocollo §5.3.

### Misura e gate

- `O=1` per tutti gli episodi;
- il DAG deve essere aciclico e il primo arresto deve essere stabile nelle due
  permutazioni dell'esperimento;
- nessun accesso esterno quando manca un prerequisito di forma o denotazione;
- rimuovendo il nodo dichiarato mancante si riproduce lo stesso arresto;
- `L_sem=1`, `L_prov=1`, `X_lang=0` e almeno un testimone di `L_grow`;
- `B >= 6`, coprendo L0, L1, L2, L3, L5 e L6.

### Falsificazione

H437 è falsa se lo stesso fallimento riceve tipi incompatibili, se il termine
mancante non è riutilizzabile come variabile di ricerca o se un arresto esterno
nasconde un prerequisito interno.

### Perché scala

L'oggetto non codifica “peso”, “fiume” o “okapi”: codifica il posto vuoto in
una prova. Tutti i domini successivi useranno lo stesso DAG.

### Avanzamento registrato il 25 agosto 2026

Il nucleo è implementato in `kb/core/arrests.p0`, incluso da
`kb/core/procedures.p0`, ed è accompagnato da
`docs/labs/autocrescita-v3/gen437-arrests.p0t` e dalla matrice completa nei
file `gen437-matrix-*`. Da processo pulito i tre dossier producono
**64 + 54 + 36 verifiche strutturali riuscite**. Sono esperimenti costruttivi
isolati, non aggiunte ai bench storici.

Oggetti ora presenti:

```text
turn_arrest(Turn, language(Lin,Lout), Kind, arrest(Missing,Goal))
arrest_depends_on/2       arrest_blocks/2
arrest_rank/2             arrest_cycle/1
arrest_dag/1              turn_first_arrest/4
turn_compensation_obligation/3
```

Risultati causali conservati:

1. i sensori runtime `turn_outcome`, `turn_topic`, `turn_module`,
   `turn_register` e `pending_gap` erano scambiati per fatti sul mondo; la loro
   dichiarazione `machinery` impedisce che osservare un gap produca
   circolarmente conoscenza sul suo topic;
2. `KB_MAX_ARGS=4` ha falsificato il primo contratto a cinque argomenti. La
   coppia `arrest(Missing,Goal)` conserva l'informazione entro il limite senza
   aumentare il motore;
3. residuo di frame e gap derivato potevano generare due arresti concorrenti.
   Il sensore di esito, quando esiste, è la diagnosi già prodotta dal turno; i
   residui di frame sono il fallback strutturale quando tale diagnosi manca;
4. mettere la chiusura ricorsiva `arrest_cycle/1` sotto NAF non costituisce una
   prova positiva di aciclicità quando la ricerca può esaurire il budget. Per il
   grafo gen437 a tre livelli, `arrest_dag/1` prova esplicitamente distinzione
   dei nodi e assenza dei tre archi inversi; `arrest_cycle/1` resta l'osservatore
   generale. Il planner esteso dovrà portare un certificato topologico KB;
5. un'etichetta `concept_label(lumion,eo,common,lumio)` aggiunta a runtime
   trasforma lo stesso arresto da `blind` a `reach`, conserva `eo`, forma
   `lumio` e concetto `lumion`; la retrazione ripristina `blind`;
6. ricerca della prova/piano, dipendenze, rango e blocco sono regole KB. Il C
   non contiene una politica di selezione o contabilizzazione introdotta da
   questa generazione.

Stato del gate: **H437 chiusa**. La matrice completa contiene 3 forme logiche,
3 domini, 2 lingue e 2 superfici; il dossier inverso cambia l'ordine dei fatti
senza cambiare il primo arresto e il reload pulito replica il risultato. Un
arresto strutturale unico ma ciclico resta osservabile in `turn_first_arrest/4`
e non diventa azionabile: `turn_arrest_complete/1` richiede separatamente il
certificato locale `arrest_dag/1`. Questa distinzione evita di confondere
unicità e validità del grafo.

---

## gen438 — IR gerarchica e dualità asserzione/domanda

### Ipotesi H438

Una rappresentazione gerarchica di span, sintagmi, clausole, frame e ruoli può
servire sia alla lettura della prosa sia alla comprensione delle domande.

### Intervento

- completare la migrazione M1–M5/U9 dell'input universale;
- sostituire l'enumerazione C `it/en` con conteggio meccanico di candidati
  linguistici pubblicati dalla KB e materializzare `turn_language` per turno;
- indicizzare cue, forme sintattiche e regole di composizione per lingua, senza
  confondere la lingua con il registro;
- introdurre ruoli nominati e composizione dichiarativa;
- legare `assertion_view` e `question_view` allo stesso tipo di frame;
- risolvere locuzioni ed entità multiword prima del binding dei ruoli;
- rendere insegnabili e revocabili a runtime forme e viste.

### Esperimento costruttivo

Insegnare un solo membro per ciascuna classe: relazione binaria, misura e
appartenenza di classe. Usare le altre celle della matrice `3 x 3 x 2 x 2` per
membri e superfici non insegnati. Ogni asserzione deve poter alimentare la
corrispondente domanda e ogni domanda deve indicare quale ruolo manca. Aggiungere
poi i marcatori e le forme minime di una terza lingua a runtime e ripetere i tre
domini senza cambiare il C.

### Misura e gate

- stessa firma di frame per asserzione e domanda in tutte le celle;
- binding esatto di tutti i ruoli, inclusi dimensione e unità;
- `F >= 3` per ogni operatore insegnato e trasferimento in tre domini;
- aggiunta e retrazione runtime di una forma cambiano soltanto la famiglia
  corrispondente;
- nessun codice o letterale di lingua nel C, inclusi `it`, `en` e il nuovo
  membro; il C enumera soltanto risultati KB;
- `L_sem=1`, `L_prov=1`, `L_grow>=1`, `X_lang=0`;
- `B >= 5`, coprendo L0–L3 e L5.

### Falsificazione

H438 è falsa se serve un recognizer C per una nuova superficie, se domanda e
asserzione producono strutture incompatibili o se una locuzione composta viene
risolta solo grazie al suo valore lessicale specifico.

### Perché scala

Si aggiungono membri e schemi nella KB; il motore continua a unificare strutture
senza conoscere le parole.

### Avanzamento registrato il 25 agosto 2026

Il checkpoint Gen438 corregge prima di tutto due falsi presupposti di M0:

1. `input_node` veniva asserito con arità 7, ma `KB_MAX_ARGS=4`; la chiamata
   falliva e in KB restavano soltanto i sidecar di superficie. Il contratto ora
   è `input_node(Scope,Id,node(Level,Kind,Parent),range(Start,Length))` e tutte le
   coordinate sono proiettabili da regole;
2. la pulizia della struttura cancellava indistintamente turno e prosa. La
   primitiva meccanica `kb_retract_match` scade ora un solo `Scope`, così una
   lettura interna non distrugge l'IR del turno che l'ha richiesta.

Il detector non contiene più l'enumerazione `it/en`: enumera tutti i binding di
`language_marker/2` e pubblica supporto, posizione e conteggio. Le regole in
`language-observation.p0` decidono massimo unico, pareggio esplicito e sticky
fallback. Anche lingua predefinita e mapping del prefisso locale sono fatti KB.

Il primo matcher compositivo usa nodi, forme linguistiche e vincoli d'ordine
per produrre la stessa firma nei due modi:

```text
input_semantic_frame(Scope, assertion|question, Operator, Roles)
```

Sono presenti tre famiglie: relazione binaria, misura con valore e unità, e
appartenenza di classe. La domanda conserva lo stesso operatore e pone
`missing` soltanto nel ruolo aperto; `input_frame_gap/3` rende quel ruolo
consumabile dall'arresto.

Evidenza costruttiva da processo pulito:

```text
gen438-observation.p0t  => 41 passed
gen438-frames.p0t       => 29 passed
```

La prima prova aggiunge `eo` a runtime, distingue ablazione del marcatore da
ablazione della denotazione, lascia un pareggio senza tiebreak e fa comparire e
scomparire un sintagma tramite `phrase_boundary/3`. La seconda copre
`3 operatori x 2 modi x 2 lingue` e ripete le sei celle in Esperanto aggiunto
interamente a runtime; togliere una sola forma verbale elimina soltanto la
famiglia binaria interessata.

Il secondo taglio porta ora la dualità fino al consumer reale. Il lettore inline
non riconosce più `read:`/`leggi:` nel C: il trigger è `segment_role/2` e la
facoltà è `faculty_for/2`. La prosa usa un proprio scope linguistico, produce lo
stesso `input_semantic_frame/4`, commette una `semantic_proposition/1`, conserva
`proposition_source/4` e pubblica l'indice `semantic_binding/3`. Una domanda in
un'altra lingua completa il ruolo `missing` contro quell'indice e la risposta
viene realizzata da forme e template KB.

`input_frame_record/3` materializza una sola volta la lettura ammessa al confine
di osservazione. Non è una cache linguistica C: è un record KB derivato dalla IR
e scaduto con lo scope. La proiezione poco profonda è necessaria perché il limite
di profondità del solver non trasformi la ricostruzione ripetuta della stessa
proposizione in una falsa assenza. Analogamente, `proposition_source_record/3`
conserva la provenienza entro `KB_MAX_ARGS=4` e la espone come vista pubblica a
quattro coordinate.

Nuova evidenza da processo pulito:

```text
gen438-prose-dialogue.p0t => 57 passed
```

Il dossier prova fonte inglese interrogata in italiano, fonte italiana
interrogata in inglese, una fonte Esperanto abilitata a runtime, ablazione e
ripristino del trigger del lettore, ablazione e ripristino della forma verbale,
ambiguità non committata e provenance esatta di lingua, superficie e span.
Il percorso positivo ritorna prima dei vecchi estrattori: per questo il suo
`legacy_hits` è zero. Il fallback lessicale C sottostante resta debito esplicito
da rimuovere, non una parte del risultato.

L'estensione del 27 agosto aggiunge due clausole nella stessa fonte. Lo splitter
resta una primitiva di confine, ripristina il buffer dopo ogni clausola e la KB
riceve per la seconda proposizione lo span globale `range(21,21)`, invece di un
nuovo `range(0,21)`. Questo è il primo testimone Gen439 della catena
`fonte intera -> clausole -> proposizioni multiple`: la provenienza non è più
soltanto corretta per una frase isolata.

Il taglio Gen439 aggiunge inoltre le locuzioni come nodi `phrase` della stessa
IR. Una relazione `phrase_form/4` insegnata a runtime (`"new york" ->
new_yorkion`) viene risolta senza modifiche al C, ordinata per gli span e
composta nella proposizione della prosa; la stessa locuzione viene poi
riconosciuta nella domanda e consumata dal frame di risposta. Anche il confine
verbale (`pos(produce,verb)`) è insegnato nella KB. Rimuovendo la forma, la
prosa non produce più quella proposizione. L'esperimento costruttivo
`gen439-phrases.p0t` chiude con **14 proprietà passate**. Questo campione
conferma il percorso prosa→locuzione→IR→domanda; restano da generalizzare
coordinazioni e locuzioni annidate.

Stato del gate: **H438 resta aperta** e H439 è in avanzamento. Questi risultati dimostrano il produttore
n-lingue e il primo verticale prosa→proposizione→domanda→risposta, non M1–M5
completo. Mancano
ancora locuzioni multiword risolte come nodi, ruoli annidati, scope e
coordinazioni, trasferimento di ogni operatore su tre domini e rimozione dei
rami linguistici legacy equivalenti. Il censimento conservativo iniziale trova
2040 siti sospetti fra `src/brain` e `src/code.c`; la prima migrazione porta il
checkpoint a 2039. Sono ordinati per classi e gate nel gold standard, non
trattati come fix locali. Chiamare chiusa la
generazione su
questi campioni sarebbe precisamente l'errore che il gate deve impedire.

---

## gen439 — La prosa diventa una fabbrica di operatori proposizionali

### Ipotesi H439

Un piccolo insieme di operatori compositivi insegnabili può trasformare prosa
reale in proposizioni interrogabili, con produttività maggiore della crescita
per fatti isolati.

### Intervento

- estrazione da definizioni, apposizioni, relative, coordinazioni e misure;
- continuità del soggetto e anafora locale;
- frame causali con condizioni ed effetti;
- provenienza per singola proposizione, non per pagina intera;
- distinzione tra contenuto della fonte e regola linguistica osservata.

### Esperimento costruttivo

Usare almeno cinque forme logiche della tabella al §3 e tre domini. Insegnare
una costruzione su una frase; applicarla poi a tre frasi reali non usate per
l'insegnamento. Le domande sono generate dagli slot semantici, non da template
testuali fissi. Ripetere la costruzione in italiano e inglese distinguendo:
operatori semantici condivisi, realizzazioni linguistiche indicizzate e
provenienza dello span. Una proposizione estratta in una lingua deve essere
interrogabile dall'altra attraverso il concetto canonico, senza tradurre lo
scope o inventare un ponte.

### Misura e gate

- `E=1` e `U=0` in ogni cella ammessa;
- ogni proposizione ha revisione, span e hash;
- una forma insegnata aumenta il numero di proposizioni utilizzabili su testo
  successivo, non solo sulla frase docente;
- l'ablazione dell'operatore elimina tutte e sole le estrazioni dipendenti;
- `L_sem=1`, `L_prov=1`, `X_lang=0` sulle coppie di prosa;
- `F >= 3` e `B >= 6`, coprendo L0–L5.

### Falsificazione

H439 è falsa se il lettore restituisce testo ma non la forma richiesta dal
goal, se una proposizione non è sostenuta dal suo span o se ogni nuovo verbo
richiede codice.

### Perché scala

Un fatto aggiunge un punto; un operatore di lettura aggiunge una famiglia di
punti a tutte le pagine future. È il primo vero moltiplicatore della crescita.

---

## gen440 — Indirizzamento interno prima della sorgente esterna

### Ipotesi H440

La maggioranza degli arresti apparentemente fattuali può essere resa precisa
risolvendo prima forma, identità, alias, traduzione, classe e composizione nella
KB locale.

### Intervento

- materializzare indirizzi e relativi prerequisiti;
- applicare in ordine identità, traduzione, alias, relazione, categoria e
  composizione;
- separare pagina candidata, relazione richiesta e forma di risposta attesa;
- rendere l'indirizzo una quadrupla di lingua-edizione, titolo, revisione e
  sezione, mantenendo distinta la lingua di output;
- ammettere `address_stop` quando l'indirizzo non è determinato.

### Esperimento costruttivo

Creare catene in cui la stessa entità è espressa con alias, traduzione e
locuzione multiword; poi chiedere una relazione presente e una acquisibile. Il
caso positivo di stop contiene una domanda leggibile ma priva della relazione
che determinerebbe cosa cercare: il comportamento corretto è una richiesta di
chiarimento, non un fetch. Almeno una coppia deve risolversi nella stessa
edizione della domanda e una deve usare un indirizzo interlinguistico esplicito
già sostenuto dalla KB.

### Misura e gate

- `A=1` per ogni arresto;
- tutti gli indirizzi interni risolvibili vengono consumati prima del fetch;
- ogni indirizzo esterno contiene entità, relazione e shape del valore;
- ogni indirizzo esterno contiene la lingua della pagina e non sovrascrive
  `Lin` o `Lout`;
- zero fetch con uno dei tre campi irrisolti;
- un nuovo alias insegnato a runtime abilita i membri della sua famiglia e la
  retrazione li disabilita;
- `B >= 6`, coprendo L0, L1, L2, L5, L6 e L8.

### Falsificazione

H440 è falsa se il sistema cerca una pagina per compensare una domanda non
compresa o se la scelta della pagina dipende da un nome codificato nel C.

### Perché scala

L'indirizzo è un ponte fra ruoli semantici, non una tabella di URL. La stessa
strategia vale per qualsiasi entità raggiungibile.

---

### Evidenza Gen440

Il profilo `gen440-addressing.p0` e l'esperimento `gen440-addressing.p0t`
dimostrano **15 proprietà passate**. Un alias insegnato a runtime viene
risolto in identità, relazione e shape; la KB costruisce l'indirizzo esterno
con lingua della pagina, edizione, titolo, revisione e sezione. Un prerequisito
mancante produce `address_stop/3` e non abilita alcun fetch. La retrazione e il
ripristino dell'alias cambiano l'indirizzamento senza ricompilare.

### Evidenza Gen441

Il profilo `gen441-wikipedia-memory.p0` e il relativo esperimento dimostrano
**7 proprietà passate**: uno span con lingua e coordinate di revisione sostiene
solo la claim richiesta (`height/measure`), mentre il contenuto irrilevante e i
frame non supportati non vengono riprodotti. Il protocollo
`source_candidate/4 → source_supported/4 → source_replay/4` conserva coordinate
e proposizione, non il corpo della pagina. È il gate KB per collegare in
seguito il fetch reale senza promuovere dati prima del replay.

## gen441 — Wikipedia diventa una sorgente mirata in memoria

### Ipotesi H441

Un indirizzo completo consente di leggere una revisione Wikipedia e ottenere
il minimo contenuto necessario a riprendere il turno, senza importare la pagina
come deposito opaco.

### Intervento

- fetch in memoria della revisione identificata;
- preservare `Lsrc` su pagina, span e regola di estrazione;
- selezione degli span candidati in base al frame richiesto;
- estrazione in quarantena con status `source_supported`;
- replay dello stesso turno contro il candidato;
- persistenza delle sole coordinate della fonte e delle proposizioni promosse.

### Esperimento costruttivo

Usare cinque famiglie fattuali: classe, relazione, misura, luogo e tempo, su tre
domini. Ogni pagina deve contenere informazione ulteriore irrilevante, così da
dimostrare selezione e minimalità senza costruire casi negativi artificiali.
Eseguire almeno una catena `Lin=it, Lsrc=en, Lout=it` e la simmetrica, con la
stessa proposizione canonica finale.

### Misura e gate

- in ogni episodio il frame richiesto viene ottenuto e il turno ripreso;
- `U=0`: nessuna proposizione estranea viene promossa;
- 100% delle proposizioni conservate ha coordinate riproducibili;
- `L_prov=1` per fonte e span, `L_sem=1` fra le due catene e `X_lang=0`;
- zero corpi pagina o cache testuali persistenti;
- un fetch non può cambiare la KB stabile prima del replay positivo;
- `P=1`, `E=1`, `M=1`, `B >= 6`.

### Falsificazione

H441 è falsa se il successo dipende dal titolo anziché dalla prosa, se il testo
intero viene trattenuto o se il turno non consuma la proposizione estratta.

### Perché scala

Il costo permanente è proporzionale alle proposizioni causalmente utili, non
alla dimensione di Wikipedia.

---

## gen442 — Il piano di compensazione entra nel dialogo

### Ipotesi H442

Le azioni di riparazione possono essere pianificate dalla stessa conoscenza
causale KB che pianifica azioni sul mondo, mantenendo tutti gli obblighi del
turno. Anche strategia di ricerca, dipendenze e limite del piano restano fatti e
regole della KB.

### Intervento

- rappresentare il turno come insieme di obblighi;
- introdurre schemi KB di `resolve`, `clarify`, `acquire`, `read`, `extract`,
  `derive`, `replay` e `decline`;
- associare a ogni passo precondizioni, effetti, costo, supporto e alternativa;
- mantenere `Lin`, `Lsrc` e `Lout` come vincoli del piano, mentre azioni ed
  effetti semantici restano canonici;
- impedire che il primo modulo che produce testo chiuda prematuramente il turno;
- realizzare dalla KB sia il risultato sia l'eventuale mossa di chiarimento.

### Esperimento costruttivo

Preparare episodi con un obbligo singolo, due obblighi indipendenti e due
obblighi dipendenti. Distribuire i difetti fra forma, ponte, fatto, operatore e
realizzazione. Almeno un piano deve usare Wikipedia; almeno uno deve risolversi
internamente; almeno uno deve scegliere un chiarimento informativo.

### Misura e gate

- ogni obbligo compare nel piano o in uno stop motivato;
- ogni passo ha precondizioni ed effetti verificabili;
- l'effetto dell'ultimo passo implica la chiusura del goal;
- se un passo non è disponibile viene scelta un'alternativa valida o il declino;
- nessun fatto, risorsa o capacità viene inventato per completare il piano;
- piani equivalenti da input italiano e inglese hanno gli stessi passi
  canonici (`L_sem=1`) e realizzazione nella lingua richiesta (`L_prov=1`);
- `P=1`, `U=0`, `B >= 7`, coprendo L2–L8.

### Falsificazione

H442 è falsa se la riparazione dipende da una sequenza cablata nel C, perde un
obbligo del turno o produce una risposta senza una catena di supporto.

### Perché scala

L'autocorrezione diventa un dominio di azioni dichiarative. Nuove strategie di
compensazione si insegnano come schemi, senza modificare l'esecutore.

---

### Evidenza Gen442

Il profilo `gen442-compensation.p0` e l'esperimento associato passano **9
proprietà**. Un obbligo interno e uno con `read_source → replay` producono
passi canonici con precondizioni, effetti, costi, dipendenze e supporto
Wikipedia; un obbligo senza ricetta produce `compensation_stop(no_steps)`. Una
ricetta aggiunta e poi ritratta a runtime apre e chiude il piano senza
ricompilare. Questo è il primo consumer dichiarativo degli arresti; la verifica
dell'effetto terminale su ogni passo resta il lavoro di completamento H442.

## gen443 — Promozione minima, quarantena e rollback causale

### Ipotesi H443

Il sistema può distinguere ciò che ha causato la ripresa da ciò che era solo
presente nella fonte o nello stato transitorio.

### Intervento

- raggruppare candidati per campagna e dipendenze;
- eseguire replay con il bundle, poi ablazione di ogni riga;
- promuovere il sottoinsieme minimo chiuso rispetto alle dipendenze;
- associare status epistemico e genealogia a ogni proposizione;
- includere lingua e coordinate dello span nella genealogia, senza farne parte
  del contenuto canonico della proposizione;
- rendere atomico il rollback di una campagna.

### Esperimento costruttivo

Ogni episodio presenta una pagina da cui sono estraibili almeno tre
proposizioni vere, ma soltanto una catena minima chiude il turno. Un secondo
episodio riusa una proposizione già stabile, così da distinguere dipendenza
nuova e preesistente.

### Misura e gate

- `M=1` in ogni campagna;
- rimuovere qualunque riga del bundle minimo rompe la chiusura prevista;
- rimuovere una riga non minima non cambia il risultato e quindi ne impedisce
  la promozione;
- `V=1`: rollback ripristina identità semantica, indici e conteggi;
- nessuna promozione eredita automaticamente l'autorità dell'intera pagina;
- `B >= 5`, includendo L2, L3, L5, L7 e L8.

### Falsificazione

H443 è falsa se una riga inutile resta stabile, se la provenienza è soltanto a
livello pagina o se il rollback lascia effetti osservabili.

### Perché scala

La KB cresce con complessità legata alle prove utili, mentre la quarantena
impedisce che il rumore della sorgente diventi debito permanente.

---

### Evidenza Gen443

Il profilo `gen443-quarantine.p0` dimostra **8 proprietà passate**: di tre
claim candidate solo quella richiesta e verificata dal replay viene promossa;
le altre restano in quarantena. Togliere una claim irrilevante non altera la
promozione, mentre togliere il replay causale la interrompe e rende osservabile
il rollback. La genealogia mantiene la campagna e lo span senza contaminare il
contenuto canonico.

### Evidenza Gen444 — audit conversazionale

Il primo smoke test da utente (`gen444-chat-smoke.p0t`) ha trovato un muro
reale: `make chat` non rispondeva a «cosa sai degli scacchi?» nel profilo base.
La correzione ha portato il concetto e il gloss italiano nella KB di base e ha
spostato le teste articolate su `knowledge_head/2`. Il percorso ora passa con
**3 proprietà**: risposta italiana, crescita di una nuova testa inglese e
ablazione della testa. Questo è un gate di usabilità, distinto dai campioni
strutturali; l'audit completo su chat e LLMSCORE resta obbligatorio prima della
chiusura del piano.

L'audit ha poi isolato un secondo muro reale: «che cosa possiamo fare insieme?».
Non è stato aggiunto un ramo C: la superficie è stata registrata come atto
pragmatico `collaboration_offer` in `kb/core/pragmatics.p0`, con cue EN/IT e
template bilingue. Il dialogo ora propone domanda, memoria e piano nella lingua
del turno; la stessa forma resta insegnabile e revocabile come gli altri atti.

Lo stesso criterio ha chiuso il muro sulle richieste d'opinione: l'atto
`opinion_request` in `kb/core/pragmatics.p0` risponde in EN/IT dichiarando il
limite epistemico e offrendo confronto di ragioni e compromessi. La risposta è
quindi una mossa dialogica riutilizzabile, non una preferenza simulata.

Per il passaggio verso Gen445 è stato aggiunto anche `next_step_request`: le
forme italiane e inglesi di richiesta del prossimo passo chiedono esplicitamente
obiettivo e vincoli prima di derivare una sequenza verificabile. Nel dialogo
italiano e inglese il percorso è ora attivo: il consumer interroga il cue KB
prima del più ampio router di raccomandazione, mantenendo il lessico fuori dal
C.

Il secondo passaggio ha eseguito `tests/basicchat.sh` sulla KB reale: la
copertura dei concetti matematici è salita da 65% a 88% e quella causale da 50%
a 75% dopo l'aggiunta del nucleo semantico. Restano muri osservabili su
sequenze, calendario, geografia e simboli; sono lacune registrate, non
considerate risolte per assenza di crash. Questo audit impedisce di confondere
la chiusura dei campioni Gen con una conversazione di frontiera già completa.

Per rendere l'audit riproducibile senza dipendere dal vecchio harness POSIX,
sono stati portati nel test-engine i casi utente ad alto valore in
`tests/p0t/conversation/frontier_chat_audit.it.p0t`: i quattro ingressi italiani,
la replica inglese, il nucleo di conoscenza e una campagna teach/use/retract.
Il registro `tests/p0t/meta/legacy-shell-migration.p0t` fissa inoltre il
criterio: ogni prova comportamentale deve avere un corpus `.p0t`; gli `.sh`
rimasti sono soltanto adapter di processo, report o compatibilità e non sono
il gold standard. La migrazione completa dell'inventario shell è una attività
meccanica separata, non un motivo per chiamare verde una risposta che resta un
muro.

## gen444 — Dall'addestramento assistito alla coltivazione autonoma

### Decisione di lavoro registrata

La priorità resta il completamento del percorso KB-first e della conversazione
continua in `make chat`. I test `.p0t` servono come esperimenti costruttivi e
tracce replicabili; non si investe tempo nel riallineare meccanicamente i
vecchi `.sh`. Gli script non immediatamente convertibili vengono inventariati
e discussi prima di qualunque rimozione, per non perdere strumenti di processo
o prove non ancora sostituite.

### Ipotesi H444

Un insegnamento umano di forme, classi e operatori generali può produrre più
chiusure autonome future di quante righe richieda, mentre l'insegnamento di soli
valori non può farlo sistematicamente.

### Intervento

- usare il dialogo per insegnare membri di classi grammaticali, frame e ponti;
- fare seguire immediatamente insegnamento, replay, trasferimento e retrazione;
- ammettere una modifica `.p0` manuale soltanto quando promuove un meccanismo
  generale non esprimibile dal dialogo corrente;
- richiedere che tale meccanismo renda subito insegnabile una classe a runtime;
- iniziare il conteggio genealogico delle capacità sbloccate.

### Esperimento costruttivo

Tre campagne indipendenti:

1. **forma:** si insegna un nuovo membro linguistico e il sistema usa membri
   successivi della stessa classe, incluso un idioma assente dallo snapshot;
2. **ponte:** si insegna un alias/traduzione e il sistema raggiunge fatti e
   domande già presenti;
3. **operatore:** si insegna una costruzione di prosa e il sistema acquisisce da
   Wikipedia membri nuovi in tre domini.

In ciascuna campagna, dopo il singolo atto docente, almeno tre episodi sono
autonomi e non preannunciati alla KB.

### Misura e gate

- `F >= 3` per ciascuna campagna e in tre domini complessivi;
- ogni capacità figlia ha una genealogia e fallisce dopo ablazione del genitore;
- nessuna modifica C o core KB fra insegnamento ed episodi autonomi;
- `L_grow>=1`, `L_prov=1` e `X_lang=0` nella campagna di forma;
- le classi coprono almeno forma, denotazione, proposizione e operatore;
- primo round con `R_u >= 1`, oltre a `U=0`, `M=1`, `V=1`;
- `B >= 7`.

### Falsificazione

H444 è falsa se l'insegnamento risolve soltanto il membro mostrato, se il
trasferimento è una parafrasi della stessa firma o se serve intervento umano
durante la fase dichiarata autonoma.

### Perché scala

La crescita passa da `un insegnamento -> un fatto` a
`un insegnamento -> una classe -> molti membri acquisibili`.

---

## gen445 — Pianificazione causale e compensativa soltanto dalla KB

### Stato corrente verificato

Il substrato generale è già esercitato da `tests/p0t/reasoning/multi_step_plan.p0t`:
stato, precondizioni, effetti, legge causale, catene da uno a tre passi,
accorciamento dopo un cambiamento del mondo e replica inglese su un dominio
disgiunto. Questo chiude il meccanismo di simulazione, ma non ancora il gate
Gen445: mancano la provenienza Wikipedia incrociata, l'azione informativa e la
compensazione completa con alternativa/replanning. Questi restano i prossimi
oggetti di lavoro; non vengono conteggiati come capacità già acquisite.

È stato aggiunto il campione isolato
`docs/labs/autocrescita-v3/gen445-planning.p0t` con profilo
`gen445-planning.p0`: una claim è indirizzata a una revisione Wikipedia,
verificata tramite provenienza e replay, diventa il supporto dell’azione
informativa e abilita il passo causale successivo. La sottrazione del secondo
passo riapre il residuo `missing_dependency(1,2)`. È una prova positiva del
ponte fonte→piano e del replanning, ma non chiude ancora il gate completo:
serve integrare lo stesso percorso con un turno reale e con almeno una
alternativa valida.

Il campione è stato esteso con `compensation_alternative/4` e con la vista
`compensation_alternative_valid/3`: quando il passo `open_gate` viene ritirato,
la stessa obbligazione può selezionare il ramo `use_manual_release`, purché il
ramo abbia un proprio passo dichiarato. L’alternativa è quindi una relazione
KB verificabile e revocabile, non un fallback implicito nel C.

### Ipotesi H445

Gli stessi frame estratti e gli stessi operatori KB possono sostenere piani
multi-step sul mondo e piani che acquisiscono informazione quando il mondo o il
proprio stato epistemico non sono sufficienti.

### Intervento

- completare situazione, stato, goal, risorsa, azione, transizione e costo;
- rappresentare leggi causali con precondizioni ed effetti;
- separare prova, testimonianza della fonte, assunzione e ignoranza;
- aggiungere azioni informative e punti di replanning;
- costruire alternative, dipendenze fra passi, reversibilità e residui;
- riusare gli stessi oggetti nel piano di risposta e nella realizzazione.

### Esperimento costruttivo

Tre famiglie non correlate:

1. sistema fisico semplice con livello, flusso e valvole;
2. itinerario o sequenza istituzionale con prerequisiti documentati;
3. procedura naturale o tecnica la cui causalità sia esplicitamente sostenuta
   da una revisione Wikipedia ammessa.

Per ogni famiglia: piano diretto, piano con informazione mancante, piano con un
passo reso indisponibile e compensazione. Le modifiche sono trasformazioni di
stato, non prompt golden. Ogni piano è richiesto in due lingue; almeno un piano
usa una fonte Wikipedia in una lingua diversa da quella del dialogo.

### Misura e gate

- ogni passo ha supporto KB o sorgente e nessuna risorsa è inventata;
- la simulazione degli effetti raggiunge il goal;
- le dipendenze formano un DAG e ogni passo non iniziale è abilitato dai
  precedenti;
- una correzione di fatto ritira soltanto i passi dipendenti e innesca replanning;
- l'azione informativa è scelta solo quando il suo effetto epistemico abilita
  un passo causale;
- esiste almeno un'alternativa valida o un residuo esplicito quando il piano
  completo non è possibile;
- `L_sem=1`, `L_prov=1` e `X_lang=0` sui piani accoppiati;
- secondo round con `R_u >= 1`; `U=0`, `M=1`, `V=1`, `B >= 8`.

### Falsificazione

H445 è falsa se un piano plausibile verbalmente non è simulabile, se una
correzione obbliga a ricostruire tutto senza dipendenze o se la fonte viene
trattata come procedura quando sostiene soltanto un fatto descrittivo.

### Perché scala

Il planner KB non contiene privilegi per valvole, itinerari o istituzioni.
Deriva la ricerca da stati, azioni, dipendenze e budget descritti nella KB;
nuovi domini e nuove strategie entrano come conoscenza e non come codice.

---

## gen446 — Stato vivente: dialogo, crescita e piani nello stesso ciclo

### Primo campione di stato condiviso

`docs/labs/autocrescita-v3/gen446-state-cycle.p0t` materializza il primo
round: una mossa dialogica, una claim promossa dopo replay e un piano
compensativo completo sono osservabili nello stesso snapshot KB tramite
`live_cycle/4`. L'ablazione del replay revoca la promozione e spegne il ciclo;
non viene mantenuta una copia implicita nel C. È il primo accoppiamento reale
fra i tre assi della gen446, ma non è ancora il gate finale dei quindici round.
Lo stesso campione dichiara inoltre `live_cycle_language/4`: input e output
italiani con claim sostenuta da una fonte inglese (`Lin != Lsrc`), così la
provenienza linguistica resta parte dello stato e non una scelta del renderer.
Il profilo contiene ora anche un secondo snapshot con firma diversa e traiettoria
inglese→francese→italiano; la regola è la stessa e non riceve un ramo dedicato.

Il primo collegamento a un turno reale è ora verificato manualmente con
«fai un piano per imparare gli scacchi» e «make a plan to learn chess»: il
planner carica il dominio dichiarato in `kb/experts/codebase/actions.p0` e
deriva tre passi (`learn_rules → study_openings → practice_games`) da
precondizioni e artefatti, senza una risposta speciale sugli scacchi. La
semantica è quindi viva nel dialogo. Il renderer generico cerca ora
`action_desc/3` nella lingua d’uscita e ricade su `action_desc/2`; il dominio
degli scacchi fornisce le tre descrizioni italiane e la relazione
`action_source/2`; la risposta annota `[wikipedia]` sui passi sostenuti dalla
fonte, senza un ramo C dedicato.
Anche l'intestazione del piano è ora `response_template(plan_header,...)`,
così il renderer non introduce testo naturale fuori dalla KB.

### Verifica d'uso reale

Una sequenza pulita di cinque turni indipendenti in italiano produce,
nell'ordine: preferenza onesta sugli scacchi, definizione dalla KB, opinione
epistemicamente limitata, proposta di collaborazione e piano a tre passi con
provenienza Wikipedia. Non è una prova di copertura generale, ma dimostra che
il percorso prioritario dell'utente non cade più nel muro né cambia registro
senza motivo.

### Ipotesi H446

Dopo le nove generazioni precedenti esiste uno stato KB dal quale campagne
esterne successive producono crescita utile autosostenuta senza intervento sul
motore o sul core.

### Intervento

- integrare memoria discorsiva, questioni aperte, contesti e registro;
- mantenere più obblighi e più turni senza perdere provenienza o scope;
- far competere risposta diretta, chiarimento, acquisizione, piano e declino
  come mosse KB-backed;
- misurare la riproduzione utile su campagne esogene;
- congelare motore e seed KB durante tutta la dimostrazione finale.

### Esperimento costruttivo

Preparare tre snapshot seed indipendenti. Per ciascuno eseguire cinque round.
Ogni round introduce una campagna nuova scelta prima dell'avvio e composta da:

- un fenomeno di forma o denotazione;
- una proposizione acquisibile da Wikipedia;
- una conseguenza inferenziale;
- un obbligo dialogico;
- una situazione che richiede un piano o una compensazione.

Le campagne devono complessivamente attraversare tutti gli strati L0–L8 e i
tre domini, due lingue di dialogo e almeno due edizioni linguistiche di
Wikipedia. Nessun caso può essere una semplice sostituzione di nomi di un caso
precedente: la firma logica deve cambiare.

### Gate finale

La KB è dichiarata viva sulla superficie sperimentata soltanto se la congiunzione
seguente è vera:

1. tutti i 15 round terminano con risposta/piano sostenuto o stop esatto;
2. ogni arresto soddisfa `O=1` e `A=1`;
3. ogni proposizione promossa soddisfa `E=1` e `U=0`;
4. ogni campagna soddisfa `M=1` e `V=1`;
5. ogni piano soddisfa precondizioni, dipendenze, effetti e provenienza;
6. tutti gli strati L0–L8 hanno almeno una crescita causale dimostrata;
7. `R_u >= 1` in ciascuno degli ultimi tre round di ogni snapshot;
8. nessun round richiede modifica C, modifica core KB o fonte diversa da
   Wikipedia;
9. l'ordine inverso dei casi produce lo stesso stato semantico finale;
10. ogni nuova forma o schema decisivo passa la prova teach/use/retract senza
    ricompilazione.
11. `L_sem=1`, `L_prov=1`, `X_lang=0` in ogni round e almeno una nuova lingua
    soddisfa `L_grow>=1` su ciascuno dei tre snapshot;
12. almeno una catena per snapshot ha `Lin != Lsrc` e torna correttamente in
    `Lout`, senza sorgenti esterne a Wikipedia.

Non si usa una media per compensare un requisito violato. I dodici punti sono
necessari insieme.

### Falsificazione

H446 è falsa se la riproduzione utile dipende da domande generate dal sistema,
se il trasferimento resta in una tasca logica, se compare conoscenza senza
genealogia o se uno snapshot non replica gli altri.

### Perché scala

La dimostrazione non misura quante domande già note vengono risposte. Misura se
un seed finito produce nuove capacità causali su firme logiche e domini che non
erano presenti nel suo addestramento.

## 7. Matrice di copertura della superficie logica

`●` indica lo strato direttamente oggetto dell'ipotesi; `○` uno strato che deve
essere attraversato per chiudere l'esperimento.

| Gen | L0 | L1 | L2 | L3 | L4 | L5 | L6 | L7 | L8 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 437 arresti | ● | ● | ● | ● |  | ● | ● |  | ○ |
| 438 IR duale | ● | ● | ● | ● | ○ | ● | ○ |  |  |
| 439 prosa | ● | ○ | ● | ● | ○ | ● | ○ |  | ○ |
| 440 indirizzi | ● | ● | ● | ○ |  | ● | ● |  | ● |
| 441 Wikipedia | ○ | ● | ● | ○ |  | ● | ○ |  | ● |
| 442 compensazione | ○ | ○ | ● | ● | ○ | ● | ● | ● | ● |
| 443 causalità crescita |  | ○ | ● | ● | ○ | ● | ○ | ● | ● |
| 444 coltivazione | ● | ● | ● | ● | ○ | ● | ● | ○ | ● |
| 445 piani |  | ○ | ● | ● | ● | ● | ○ | ● | ● |
| 446 stato vivente | ● | ● | ● | ● | ● | ● | ● | ● | ● |

La matrice impedisce di dichiarare autonomia perché è cresciuta una sola zona,
per esempio i fatti enciclopedici, lasciando immobili forme, dialogo o piani.

## 8. Protocollo di entrata nella generazione successiva

Per ogni generazione `g` si produce un dossier immutabile:

```text
generation-g/
  hypothesis.md
  witness-spec.p0
  wikipedia-manifest.p0
  language-manifest.p0
  initial-state.hash
  execution-trace.p0
  candidate-graph.p0
  ablation-trace.p0
  final-state.hash
  conclusion.md
```

Il dossier può avere un'altra rappresentazione concreta, ma deve contenere gli
stessi oggetti. Il gate viene valutato così:

1. **preregistrazione:** ipotesi, celle e soglie esistono prima del run;
2. **costruzione:** tutti i testimoni positivi producono la struttura attesa;
3. **trasferimento:** i membri non insegnati usano lo stesso operatore;
4. **invarianza linguistica:** le coppie producono lo stesso grafo canonico e
   conservano forma, lingua di input, fonte e output;
5. **causalità:** bundle e ablazioni dimostrano necessità;
6. **revocabilità:** rollback ripristina lo snapshot;
7. **replica:** un secondo run da snapshot pulito produce lo stesso grafo
   semantico, indipendentemente dalle frasi realizzate;
8. **decisione:** soltanto la congiunzione dei gate autorizza `g+1`.

Il confronto avviene su grafi semantici canonici, non sulla stringa finale. Il
linguaggio naturale può variare; entità, ruoli, scope, prove e dipendenze no.
Un secondo confronto sulla provenance verifica però che la canonicalizzazione
non abbia cancellato `Lin`, `Lsrc`, `Lout` o la forma osservata.

## 9. Ordine di colmatura della KB

La KB non va colmata aggiungendo ora valori sugli esempi noti. Va resa fertile
nel seguente ordine:

1. ontologia di arresti, dipendenze e contesto linguistico del turno;
2. producer n-lingue, IR gerarchica, ruoli e dualità domanda/asserzione;
3. operatori compositivi di prosa;
4. identità, alias, traduzione e indirizzi;
5. provenienza per proposizione e status epistemico;
6. obblighi del turno e mosse dialogiche;
7. schemi di compensazione;
8. quarantena, genealogia, minimalità e rollback;
9. schemi causali, azioni informative e replanning;
10. criteri di promozione e riproduzione utile.

Ogni voce entra con il primo testimone che la usa. Nessuna ontologia morta,
nessun fatto ornamentale e nessun frame privo di consumatore.

## 10. Cose che il piano esclude

- correggere o ottimizzare i bench storici come obiettivo;
- dichiarare successo perché cresce il numero di fatti;
- precaricare risposte per Po, okapi, Torre Eiffel o altri esempi;
- liste linguistiche o nomi di relazioni nel C;
- enumerazione chiusa di codici lingua nel C o cancellazione della lingua
  durante la canonicalizzazione;
- cache persistenti di pagine Wikipedia;
- generazione autonoma di testo di risposta non sostenuto;
- scansioni a freddo che inventano gap fuori da un turno reale;
- auto-modifica del motore C;
- promozione di procedure da prosa meramente descrittiva;
- uso di un punteggio medio per nascondere un'intera classe non raggiunta.

## 11. Risultato atteso

Alla gen446 parrot0 non deve “sapere Wikipedia”. Deve sapere fare qualcosa di
più importante:

1. capire quale struttura un interlocutore sta chiedendo;
2. riconoscere il primo punto esatto in cui la propria conoscenza si arresta;
3. costruire dalla KB un piano di compensazione;
4. leggere soltanto la prosa necessaria dalla sola fonte ammessa;
5. trasformarla in proposizioni e operatori riusabili;
6. riprendere lo stesso dialogo e produrre una risposta o un piano sostenuto;
7. dimostrare quale crescita ha causato il risultato;
8. trasferire quella crescita a casi nuovi e revocarla integralmente.

Tutto questo deve valere con semantica canonica condivisa fra lingue, lingua di
input e di fonte preservate e realizzazione nella lingua richiesta. Una KB viva
che vive soltanto in inglese o che tratta l'italiano come preprocessing non ha
raggiunto l'obiettivo.

## 12. Checkpoint operativo per la prossima sessione

Baseline d'ingresso: commit `f730ab8`; ultimo checkpoint pubblicato prima di
questo taglio: `52e97a6`. Il lavoro chiude gen437 e porta Gen438 al primo
verticale prosa/dialogo senza modificare né usare come obiettivo la suite storica.

### Artefatti prodotti

- `kb/core/arrests.p0`: contratto multilingue dell'arresto, DAG locale,
  dipendenze, rango, blocco, ambiguità e obbligo di compensazione;
- `kb/core/gap-kinds.p0`: sensori riflessivi correttamente esclusi dai fatti di
  mondo, specie `plan` e raggiungibilità via `linguistic_form/4`;
- `kb/core/procedures.p0`: inclusione del nuovo strato;
- `docs/labs/autocrescita-v3/gen437-arrests.p0t`: matrice costruttiva e prove
  runtime di crescita/retrazione linguistica;
- `docs/labs/autocrescita-v3/gen437-matrix*.p0*`: seconda superficie, ordine
  inverso e replica pulita del gate gen437;
- `kb/core/language-observation.p0`: evidenze linguistiche n-lingue e policy KB;
- `kb/core/input-structure.p0`: proiezioni gerarchiche, forma-concetto e primo
  matcher condiviso fra asserzione e domanda;
- `docs/labs/autocrescita-v3/gen438-*.p0*`: osservazione, crescita/ablazione e
  matrice dei tre operatori;
- `docs/labs/autocrescita-v3/gen438-prose-dialogue.p0t`: commit da prosa,
  consumo interlinguistico, provenance, crescita/ablazione e ambiguità;
- `docs/plans/kb-first-c-gold-standard.md`: norma, censimento, classi di
  migrazione e gate `legacy_hits=0`;
- questo documento: roadmap, misure, gate e debiti aggiornati.

### Evidenza eseguita

```text
make build
./bin/parrot0 --test docs/labs/autocrescita-v3/gen437-arrests.p0t
./bin/parrot0 --test docs/labs/autocrescita-v3/gen437-matrix-forward.p0t
./bin/parrot0 --test docs/labs/autocrescita-v3/gen437-matrix-reverse.p0t
./bin/parrot0 --test docs/labs/autocrescita-v3/gen438-observation.p0t
./bin/parrot0 --test docs/labs/autocrescita-v3/gen438-frames.p0t
./bin/parrot0 --test docs/labs/autocrescita-v3/gen438-prose-dialogue.p0t
=> ok — 64 + 54 + 36 + 41 + 29 + 57 passed
```

Il primo comando dimostra soltanto che il motore corrente costruisce; il
secondo è l'evidenza pertinente a H437. Nessun bench storico è stato corretto,
usato come obiettivo o incluso nel gate.

### Debiti espliciti, in ordine

1. mantenere il censimento del gold standard come ratchet: nessun nuovo literal
   linguistico C e `legacy_hits=0` su ogni verticale promosso;
2. risolvere locuzioni e phrase/clause node dichiarativi senza ridurre M1–M5 ai
   tre ordini già dimostrati;
3. estendere il mantenimento degli span globali a locuzioni,
   clausole coordinate, ruoli annidati e scope, preservando fonte e span;
4. comporre la prosa già portata su `input_semantic_frame/4` con locuzioni,
   ruoli annidati, coordinazione e scope, mantenendo una sola IR;
5. migrare cue e forme sintattiche a evidenza indicizzata per lingua: attribuire
   a posteriori la lingua globale a una cue non prova in quale lingua la cue sia
   stata appresa;
6. trasferire ciascuno dei tre operatori su tre domini e due superfici, poi
   provare l'ablazione dell'ordine oltre a quella lessicale;
7. rimuovere il fallback di estrazione C famiglia per famiglia appena il relativo
   producer e consumer KB superano il gate gold;
8. prima di estendere il planner oltre tre livelli, introdurre un certificato
   topologico KB generale. `arrest_cycle/1` osserva cicli arbitrari, mentre
   l'attuale `arrest_dag/1` prova positivamente il solo contratto gen437;
9. soltanto dopo questi gate iniziare indirizzi Wikipedia: cercare prima
   renderebbe un difetto linguistico un falso gap fattuale.

Il prossimo cambiamento corretto è completare M1–M5 sul lato compositivo:
locuzioni e strutture di clausola devono produrre la stessa IR appena portata
end-to-end, poi sostituire e rimuovere per famiglia i vecchi estrattori.

Il salto di frontiera non è da KB piccola a KB grande. È da archivio passivo a
sistema epistemico riflessivo: una KB che osserva i propri limiti, pianifica la
compensazione e trasforma ogni successo locale in una capacità generale
verificabile.
