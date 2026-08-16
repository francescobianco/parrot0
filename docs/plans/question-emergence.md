# L'emersione delle domande

> **La domanda fondamentale.** Esiste un meccanismo — KB-based, e KB-first anche
> nel modo in cui è costruito — che faccia **emergere dalla KB stessa le domande
> a cui parrot0 non saprebbe rispondere ma a cui dovrebbe saper rispondere**?
>
> Documento aperto a gen382m. Serve a far ragionare in parallelo più agenti sullo
> stesso problema: qui c'è il contesto completo, il perché la strada ovvia non
> funziona, e i vincoli che una proposta deve rispettare per essere accettabile in
> questo progetto.

---

## 0. Il contesto minimo, per chi arriva adesso

parrot0 è un agente conversazionale in C puro con una base di conoscenza in un
dialetto Prolog (`kb/**/*.p0`). La regola fondativa del progetto non è "scrivi
codice che funziona", è **"non scrivere codice se la conoscenza può farlo"**
(`MANTRA.md`): il motore in C deve restare un adattatore fisso, e tutto ciò che
può crescere — vocabolario, classi, procedure, formati, risposte — vive nella KB
come fatti e clausole. Il test operativo di ogni proposta è:

> *parrot0 può impararne un nuovo membro domani, parlando, senza ricompilare?*

Chi propone una soluzione a questo documento deve passare quel test. Una
soluzione che elenca in C le domande da porre è, in questo progetto, una
non-soluzione — anche se funziona.

---

## 1. Il fatto che ha generato la domanda

Una conversazione reale, quattro turni:

```
you> come tichiami
    Non capisco ancora.
you> quante carte ci sono nel poker
    Non ho afferrato bene. Cosa vorresti sapere?
you> parlami del poker
    So già qualcosa su poker: a family of card games where players wager on
    hands whose strength may be shown or represented by betting.
you> e piu forte il full o il poker come punteggio
    Non capisco ancora.
```

Il terzo turno dimostra che parrot0 **ha** un esperto di poker: il file
`kb/experts/games/poker.p0` contiene la descrizione, le regole, i consigli, i
ranghi delle mani e persino una clausola `poker_hand_beats/2`. Eppure due domande
elementari su quel dominio vanno a muro.

## 2. Perché la strada ovvia non trova questi buchi

La reazione naturale è misurare: `tests/expertbench.py` (scritto in questa
sessione) legge dalla KB che cosa parrot0 **dichiara** di sapere — `expert/1`,
`category_surface/2`, `game_play/2`, `means/2` … — e per ogni dominio pone le
forme di domanda corrispondenti. È un buon strumento e ha già trovato difetti di
raggiungibilità.

**Ma sui domini di gioco dà 0% di muri.** Non perché parrot0 sia bravo: perché
quel bench può interrogare **solo lo spazio dichiarato**. Le domande che avevano
deluso — *quante carte ha il poker*, *è più forte il full o il poker* — non sono
irraggiungibili: sono **non dichiarate**. Il conteggio delle carte non esiste in
nessun fatto; il rango delle mani esisteva solo in un dialetto privato
(`poker_hand_rank/2`) che il motore generale di confronto non legge.

> Un bench derivato da ciò che la KB afferma **non può, per costruzione, trovare
> ciò che la KB tace.** È il complemento — lo spazio negativo — che serve.

## 3. Che cosa si chiede, detto con precisione

Non "elenca le domande possibili": sono infinite. Si chiede di far **emergere**,
dalla struttura della KB stessa, le domande che soddisfano *entrambe* queste
condizioni:

1. **parrot0 non saprebbe rispondere** — nessun fatto, nessuna regola, nessun
   frame le copre;
2. **dovrebbe saper rispondere** — c'è qualcosa nella KB che le rende *legittime
   e attese*: un fratello che quella cosa la sa, una forma di domanda dichiarata
   e mai soddisfatta, un'entità nominata e mai descritta.

La seconda condizione è tutto il problema. Senza di essa si ottiene rumore
infinito ("qual è il colore preferito del poker?"). Con essa si ottiene un
**debito di conoscenza misurabile**, e — questa è l'intuizione da cui il documento
nasce — *la stessa struttura che fa emergere la domanda dice anche come colmarla*.

## 4. Cinque sorgenti di spazio negativo, tutte calcolabili dalla KB

Ipotesi di lavoro, da valutare, estendere o smontare.

### 4a. Asimmetria fra fratelli

Se quindici esperti di gioco dichiarano `game_players/2` e il poker no, allora
*"quanti giocatori servono a poker"* è una domanda che parrot0 dovrebbe saper
reggere e non regge. La KB dice da sé che cosa sia un esperto di gioco
"completo": lo dice **per maggioranza**, non per definizione — nessuno deve
scrivere uno schema.

Generalizzato: per ogni insieme di entità che condividono un tipo
(`expert_domain(X, games)`, `is_a(X, mammal)`, `category_member(C, X)`), l'unione
dei predicati usati dai membri definisce il profilo atteso; ogni membro a cui ne
manca uno è un buco tipizzato.

### 4b. Frame dichiarati senza dati

`answer_frame(Cue, Pred)` dichiara una forma di domanda. Il prodotto
*frame × entità note* è l'insieme delle domande **poste in modo comprensibile**;
sottratte quelle per cui `Pred` ha fatti, restano le domande che parrot0 capisce
e non sa.

Questa sorgente ha una proprietà preziosa: il buco è già espresso nella forma
esatta del fatto che lo colmerebbe.

### 4c. Entità opache

Un termine che compare **solo come argomento** e mai come soggetto di un fatto:
`straight_flush` è membro di `poker_hand`, ma non ha `means/2`, non ha
descrizione, non ha magnitudine. parrot0 sa nominarlo e non sa dirne niente. Ogni
entità opaca è una domanda che qualunque interlocutore porrebbe.

### 4d. Regole morte e dialetti privati

Due varianti dello stesso difetto:

- una clausola il cui corpo non può essere soddisfatto (predicato del corpo senza
  fatti) — deduce zero, e nessuno se ne accorge;
- una relazione espressa in un **dialetto privato** invece che nella relazione
  generale che il motore legge. `poker_hand_rank(full_house, 7)` diceva
  esattamente ciò che `magnitude(hand_rank, full_house, 7)` dice, ma sotto un nome
  che solo quel file capiva: il motore generale di confronto — quello che risponde
  a *"quale fiume è più lungo"* — non poteva vederlo. **Non mancava una capacità:
  mancava che il dato fosse detto nella lingua comune** (mantra #3, *astrai fino
  al punto fisso*).

Questa quarta sorgente è la più insidiosa perché la conoscenza **c'è**, e nessuna
misura di copertura la segnala come mancante.

### 4e. La varianza della SUPERFICIE: resilienza linguistica

La prima riga della conversazione di §1 non e' un buco di conoscenza:

```
you> come tichiami
    Non capisco ancora.
```

"come ti chiami" scritto con uno spazio in meno. Un LLM lo capisce senza
accorgersene; parrot0 va a muro. E questa varianza **deve emergere dallo stesso
meccanismo**, perche' e' la stessa specie di debito: c'e' una domanda a cui
parrot0 *dovrebbe* saper rispondere — sa gia' rispondere alla sua forma pulita —
e non risponde.

E' la sorgente piu' feconda delle cinque, perche' e' calcolabile in modo
completamente meccanico e non richiede conoscenza nuova: **si prende una domanda
che oggi FUNZIONA e la si deforma.** Ogni deformazione che rompe la risposta e'
un buco di resilienza, e lo spazio delle deformazioni e' enumerabile:

- **segmentazione**: spazio mancante ("tichiami"), spazio di troppo ("po ker"),
  parole unite dalla fretta;
- **ortografia**: lettera scambiata, doppia mancata, accento assente ("piu",
  "perche", "e" per "è") — l'italiano scritto in fretta perde gli accenti sempre;
- **ordine**: "il full o il poker, quale e' piu' forte";
- **cortesia e riempitivi**: "scusa, sapresti dirmi …", "senti, ma …";
- **abbreviazioni e registro**: "qnt", "cmq", minuscole ovunque, nessuna
  punteggiatura;
- **codice misto**: una parola inglese dentro una frase italiana.

Il metro e' immediato e non richiede oracolo: **la risposta alla forma deformata
deve essere la stessa della forma pulita.** Non serve giudicare la verita' — si
confronta con sé stessi.

Due note che cambiano il modo di affrontarla, e sono il motivo per cui questa
sorgente sta in questo documento e non in un TODO qualunque:

1. **Il rimedio non e' un correttore ortografico in C.** Sarebbe vocabolario nel
   motore, e per giunta monolingue. La strada KB-first e' che le CLASSI DI
   DEFORMAZIONE siano conoscenza (`surface_variation(missing_space, …)`,
   `surface_variation(missing_accent, …)`) e che il motore le applichi come
   ipotesi di riparazione — parrot0 ha gia' `mod_robust` e una nozione di
   riparazione, ma non e' guidata da nulla di dichiarato.

2. **La riparazione ha bisogno del lessico che gia' esiste.** "tichiami" si
   risolve senza sapere nulla di ortografia: si prova a spezzarlo in due token
   che siano *entrambi parole note* (`ti` + `chiami`), e la KB il lessico ce
   l'ha. E' lo stesso principio con cui `np_closer/1` ha sbloccato l'estrazione
   dalla prosa — non una lista nuova, una lettura nuova di cio' che c'e'.

Questa sorgente, a differenza delle altre quattro, produce buchi **misurabili
oggi**: basta prendere le domande che il sistema gia' supera e deformarle. Un
prototipo di emersione che comincia da qui ha il vantaggio di poter mostrare
subito una lista vera, e di non dover attendere la parte piu' difficile
(l'asimmetria fra fratelli, §4a).

## 5. Il vincolo che rende il problema interessante

Il meccanismo di emersione deve essere **esso stesso KB-first**. Cioè:

- le sorgenti di §4 non devono essere cinque funzioni C, ma **regole nella KB**
  sopra una rappresentazione della KB stessa (parrot0 possiede già un modello di
  sé: `machinery/1`, `file_attribute/1`, `expert/1`, `capability/2`);
- il **tipo** di un buco e il suo **rimedio** devono essere fatti, non `switch`:

  ```prolog
  gap(missing_sibling_attribute, poker, game_players).
  gap(opaque_entity, straight_flush).
  gap(private_dialect, poker_hand_rank, magnitude).
  gap(surface_fragility, "come ti chiami", missing_space).

  gap_remedy(opaque_entity, dream).          % vai a leggere la pagina
  gap_remedy(missing_sibling_attribute, ask_user).
  gap_remedy(private_dialect, restate).      % ridillo nella relazione generale
  gap_remedy(surface_fragility, repair_hypothesis).
  ```

- un rimedio nuovo, o una sorgente nuova, deve costare **un fatto**.

## 6. Perché questo chiude un cerchio già aperto

parrot0 ha `--dream`: un'esplorazione ricorsiva che parte da un topic, ne legge la
prosa (corpus statico o Wikipedia) ed estrae fatti e regole, scendendo parola per
parola. Oggi il sogno esplora **ciò che incontra**. È già emerso, discutendone,
che servirebbe una guida in stile alfa-beta — una funzione di valutazione che
decida quali rami valga la pena approfondire.

**I buchi sono quella funzione di valutazione.** Un nodo vale la pena se colma un
buco dichiarato. Il sogno smetterebbe di esplorare a caso e comincerebbe a
*cercare ciò che gli manca* — e siccome ciò che impara viene instradato nell'albero
curato e committato, il ciclo si chiude: **emersione → sogno guidato → conoscenza
persistita → nuovi buchi di livello più fine**.

## 7. Che cosa renderebbe accettabile una proposta

- **Deriva le domande, non le elenca.** Nessuna lista di domande in C né in KB:
  devono nascere dalla struttura.
- **Nessun falso allarme.** Il primo prototipo di `expertbench.py` chiedeva "come
  si gioca a algebra" e dava 100% di muri: misurava sé stesso. Una proposta deve
  spiegare *perché* le domande che genera sono legittime.
- **Il buco è tipizzato e il rimedio è dichiarato**, così l'output è azionabile e
  non un elenco di lamentele.
- **Il meccanismo cresce come conoscenza**: una sorgente nuova = fatti.
- **Una prova che può fallire.** Questo progetto ha appena pagato caro il suo
  contrario (`KB_TODO.md`, sezione sul punto 1): una dimostrazione compatibile
  anche con la NON avvenuta implementazione non dimostra niente. Per ogni
  proposta: *quale osservazione sarebbe diversa se il meccanismo non ci fosse?*

## 8. La domanda di review

> Dato uno stato qualunque della KB, parrot0 sa **elencare da sé** le domande che
> un interlocutore ragionevole gli porrebbe e a cui non saprebbe rispondere — e
> sa dire, per ciascuna, **da dove prenderebbe la risposta**?
>
> E sa dire quali domande **sa già reggere solo se scritte bene**?

Oggi: no. Non ha nemmeno il vocabolario per nominare un proprio buco.

---

*File aperto. Le proposte possono essere aggiunte in coda come sezioni datate,
oppure discusse in `KB_TODO.md`. Chi propone: leggere prima `MANTRA.md` — non è
cerimoniale, è il criterio con cui la proposta verrà giudicata.*

## 9. Quaderno condiviso — gen382n, 13 agosto 2026

Questa sezione non dichiara il problema risolto. Fissa ciò che abbiamo osservato,
confronta architetture differenti e descrive il primo taglio verticale oggi nel
worktree. Le etichette hanno un significato preciso:

- **osservato**: riprodotto sul binario corrente;
- **proposta**: disegno ancora da falsificare;
- **prototipo**: codice presente nel worktree ma non ancora accettato;
- **aperto**: decisione per cui serve ancora evidenza.

### 9.1 Prima correzione: un muro non è l'unico fallimento

**Osservato.** L'ultima esecuzione esplorativa di `expertbench.py` ha trovato
cinque muri su sette per UNO e zero muri su sette per molti altri giochi. Ma
alcuni degli apparenti successi erano risposte semanticamente estranee alla
domanda: per esempio una comparazione fra `checker` e `doubling_cube` poteva
ricevere un elenco, non un confronto. Quindi «non è andato a muro» non equivale
a «ha risposto».

Il problema va separato in almeno quattro predicati:

```text
O(q)  la KB rende legittimo aspettarsi una risposta a q
K(q)  la risposta è derivabile dalla conoscenza, entro una ricerca completa
R(q)  la domanda raggiunge il predicato/consumer giusto
V(q)  l'output soddisfa il contratto semantico della domanda
```

Ne seguono difetti diversi, con rimedi diversi:

```text
knowledge gap       = O(q) ∧ ricerca_completa(q) ∧ ¬K(q)
reachability gap    = O(q) ∧ K(q) ∧ ¬R(q)
surface gap         = R(q) ∧ ¬R(muta(q))
wrong-answer gap    = R(q) ∧ ¬V(q)
```

La distinzione è sostanziale. UNO, per esempio, possiede conoscenza di gioco ma
la parola `uno` compare anche come stopword: è un candidato **reachability gap**,
non un knowledge gap. «Come ti chiami» funziona e «come tichiami» no: è un
**surface gap**. Una risposta qualsiasi alla comparazione non è un successo: è
un possibile **wrong-answer gap**.

`KbInferenceReport.budget_hit` introduce inoltre un terzo esito epistemico:

```text
proved | finite_failure | incomplete
```

`incomplete` non può mai essere trasformato in `gap`. Se il budget interrompe
la ricerca, parrot0 non ha dimostrato un'assenza: ha solo smesso di cercare.

Infine la domanda deve avere un contratto abbastanza preciso. «Quante carte ci
sono nel poker?» può significare carte nel mazzo, nella mano, sul board o in una
variante. Senza una dimensione dichiarata, promuoverla automaticamente a gap
produrrebbe proprio il rumore che vogliamo evitare.

### 9.2 Le architetture considerate

| Soluzione | Che cosa vede bene | Vantaggio | Difetto decisivo | Esito |
|---|---|---|---|---|
| Cinque scanner specializzati in C | pattern scelti a priori | semplice da avviare | sorgenti, soglie e vocabolario restano compilati | scartata |
| Sole regole `.p0` sul modello attuale | fatti meta già espliciti | KB-first puro | la KB oggi non può quantificare sui propri predicati e applicarne uno dato come valore | insufficiente da sola |
| Analizzatore offline dei file `.p0` | fatti, regole e sintassi completa | ottimo strumento di audit | vede testo su disco, non necessariamente stato runtime, derivabilità e comportamento conversazionale | supporto, non nucleo |
| Vista riflessiva virtuale + regole KB | struttura runtime e copertura derivata | decisioni e politiche restano insegnabili | richiede primitive generali molto piccole e limiti espliciti | scelta per i gap strutturali |
| Probe black-box/metamorfico | raggiungibilità, superficie e output reale | misura ciò che l'utente incontra | senza contratto semantico confonde una risposta sbagliata con un successo | scelta per i gap comportamentali |
| Ibrido riflessione + probe | O, K, R e V separati | falsificabile end-to-end | più stati da rappresentare e isolare | proposta raccomandata |

La conclusione corrente è che non esista un unico osservatore sufficiente. La
riflessione è adatta a chiedere «che cosa dovrebbe esserci?» e «è derivabile?»;
il probe è necessario per chiedere «una persona riesce a raggiungerlo?» e «la
risposta conserva il significato?». L'unione va fatta su record tipizzati, non
su stringhe di log.

### 9.3 Primo taglio verticale nel worktree

**Prototipo.** Il risolutore espone due sole operazioni cieche rispetto al
dominio e alla lingua:

```prolog
kb_fact(Predicate, ArgsList).   % vista dei fatti positivi diretti
apply(Predicate, ArgsList).     % meta-chiamata attraverso il normale risolutore
```

`kb_fact/2` rende osservabile la forma della KB; `apply/2` è indispensabile
perché «coperto» deve voler dire **derivabile da fatti o regole**, non «presente
fisicamente come fatto». Il C non conosce `poker`, `game_players`, la nozione di
fratello, una soglia o un tipo di gap.

Sopra queste operazioni, la KB dichiara oggi:

```prolog
gap_source(missing_sibling_attribute,
           sibling_majority_expected,
           binary_relation_coverage).
gap_policy(sibling_majority_expected, minimum_group_size, 3).
gap_remedy(missing_sibling_attribute, ask_user).
```

Le procedure `.p0` enumerano gli attributi binari realmente usati dai membri di
uno stesso `expert_domain/2`, richiedono almeno tre membri e una maggioranza
stretta, verificano la copertura via `apply/2` e producono:

```prolog
gap_record(Type, Entity, Facet, Remedy).
```

Questa è una **prova di architettura**, non ancora una soluzione alle cinque
sorgenti. Durante la prima verifica a stadi, riflessione, conteggio del dominio,
conteggio del supporto e controllo di copertura hanno funzionato; la composizione
finale della regola di maggioranza ha invece esposto un problema da isolare nel
passaggio aritmetico. Il dato è utile: il test ha già distinto il prototipo
funzionante da una semplice presenza di codice, esattamente come chiede §7.

La promessa «una sorgente nuova costa un fatto» va formulata con onestà: un
nuovo **uso di combinatori già generali** costa un fatto `gap_source/3`; una
nuova operazione riflessiva fondamentale non può essere inventata dai dati. Per
essere accettabile, ogni futura primitiva C deve essere indipendente da lingua,
dominio e tipo di gap, e deve sbloccare un'intera classe di regole insegnabili.

### 9.4 La prova minima che deve passare il taglio verticale

Il caso sintetico evita di cucire il test su poker:

```prolog
expert_domain(alpha, toy_domain).
expert_domain(beta,  toy_domain).
expert_domain(gamma, toy_domain).
toy_attribute(alpha, one).
toy_attribute(beta,  two).
```

Risultati attesi, tutti necessari:

1. emerge `gap_record(missing_sibling_attribute, gamma, toy_attribute,
   ask_user)`;
2. aggiungendo a runtime `toy_attribute(gamma, three)` il gap scompare;
3. ritraendo quel fatto il gap ricompare;
4. ritraendo `gap_source/3` il detector scompare e riasserendolo ritorna, senza
   rebuild;
5. con un solo supporto su tre non emerge nulla;
6. con una coorte di due membri non emerge nulla;
7. se `toy_attribute(gamma, Value)` è derivabile da una regola, non emerge un
   falso gap;
8. se la ricerca colpisce il budget, il risultato è `incomplete`, non `gap`.

I punti 2–4 sono il test di crescita e ablazione richiesto da `AGENTS.md`. I
punti 5–8 sono i falsificatori: senza di essi avremmo solo una demo positiva.

Restano due rischi specifici da chiudere prima di considerare il taglio pronto:

- il conteggio deve essere per membri distinti, non per numero di fatti: dieci
  valori dello stesso membro non costituiscono una maggioranza;
- il profilo non deve scambiare relazioni strutturali (`expert_domain`, tracce,
  registri e macchineria) per attributi del mondo. L'esclusione deve essere
  insegnabile tramite fatti meta, mai una blacklist C.

### 9.5 Disegno del ramo comportamentale

**Proposta.** I gap di superficie richiedono un esperimento metamorfico:

```text
seed KB-backed che oggi riesce
        ↓
operatore di variazione dichiarato nella KB
        ↓
esecuzione pulita e mutata in stati isolati equivalenti
        ↓
confronto di intent, entità, predicato raggiunto e risposta semantica
        ↓
surface_gap(seed, variation, evidence)
```

Il runner può essere meccanica fissa, ma il piano deve essere conoscenza:

```prolog
probe_seed(identity_name, "come ti chiami").
surface_variation(missing_space, split_known_tokens).
gap_remedy(surface_fragility, repair_hypothesis).
```

Le stringhe qui sono esempi di fatti insegnabili, non cue compilati. Quali
variazioni usare, su quali semi e con quale equivalenza sono decisioni KB. Il C
può applicare operazioni generali su caratteri/token e rieseguire una sessione,
ma non può sapere che `ti`, `chiami`, `piu` o `perche` hanno un significato
speciale.

Confrontare il testo grezzo delle due risposte è troppo fragile; controllare
solo «non-wall» è troppo debole. L'oracolo dovrebbe preferire, in quest'ordine:

1. stessa traccia semantica: intent, entità, relazione e slot;
2. stesso fatto/prova di risposta;
3. equivalenza dichiarata dal contratto del probe;
4. solo come segnale debole, classificazione wall/non-wall.

Ogni variante va eseguita da uno snapshot pulito: una prima domanda può
insegnare, cambiare il contesto, consumare casualità o influenzare la seconda.
Anche qui crescita e ablazione sono obbligatorie: asserire una nuova
`surface_variation` deve aggiungere casi generati; ritirarla deve rimuoverli.

### 9.6 Come trattare le altre sorgenti senza anticipare conclusioni

**Frame senza dati.** Il prodotto `answer_frame × entità` è legittimo solo se il
frame dichiara il proprio dominio o tipo di argomento. Senza questo contratto
chiederemmo «come si gioca ad algebra». Serve una relazione insegnabile simile a
`frame_applies_to(Predicate, Domain)`; non basta iterare tutte le entità.

**Entità opache.** Occorre distinguere un'entità da un valore, un'etichetta o un
pezzo di macchineria. Un candidato ragionevole è un termine che occupa un ruolo
dichiarato `entity`, appartiene a un tipo e non è soggetto di alcuna relazione
descrittiva applicabile. `kb_fact/2` permette di vedere le occorrenze nei fatti,
ma mancano ancora ruoli degli argomenti e una nozione KB-backed di «relazione
descrittiva». Prima questi metadati, poi il detector.

**Dialetti privati.** La somiglianza di forma fra due predicati non dimostra
equivalenza semantica. Un detector può proporre
`candidate_alias(Private, Canonical, Evidence)`, usando arità, tipi degli
argomenti, dimensione e tracce dei consumer; non deve asserire automaticamente
l'alias. `poker_hand_rank/2 → magnitude(hand_rank, …)` è un buon caso di test,
non una trasformazione da codificare.

**Regole morte.** La vista attuale espone fatti, non corpi di clausole. Servirà
eventualmente una vista virtuale `kb_rule(Head, Body)` o equivalente. Anche con
essa, «non ho trovato una soluzione» non significa «regola morta»: il giudizio è
lecito solo su semi tipizzati e finiti, con ricerca completata. Un budget
esaurito produce sempre `incomplete`.

### 9.7 Dal gap al rimedio e al sogno

Il detector non deve interrogare direttamente l'utente né avviare Wikipedia.
Produce evidenza; una politica separata sceglie il rimedio:

```prolog
gap_remedy(missing_sibling_attribute, ask_user).
gap_remedy(opaque_entity, dream).
gap_remedy(private_dialect, restate).
gap_remedy(surface_fragility, repair_hypothesis).
```

Per guidare `--dream` non basta contare gap. Serve un ordinamento KB-backed che
combini almeno:

```text
legittimità/supporto × valore del dominio × probabilità di colmare il gap
─────────────────────────────────────────────────────────────────────────
                         costo/rischio del rimedio
```

Il sogno deve ricevere un obiettivo tipizzato (`Entity`, `Facet`, `Dimension`),
non la stringa di una domanda. Dopo l'apprendimento, lo stesso detector deve
verificare che il gap sia davvero scomparso: questo rende il ciclo emersione →
rimedio → verifica osservabile e reversibile.

### 9.8 Decisioni aperte, in ordine di dipendenza

1. **Semantica della negazione.** Dove dichiariamo che un predicato/dominio è
   sufficientemente chiuso da consentire `finite_failure ⇒ uncovered`?
2. **Contratto della domanda.** Qual è la rappresentazione minima di intent,
   entità, relazione, dimensione e forma attesa della risposta?
3. **Riflessione sulle regole.** Basta esporre testa e corpo come termini o
   servono anche provenienza, file, confidenza e statistiche di esecuzione?
4. **Traccia semantica.** Quale oggetto stabile deve restituire la pipeline per
   confrontare due superfici senza dipendere dal wording?
5. **Ranking.** Come confrontiamo supporto statistico, importanza del dominio,
   costo del rimedio e rischio di falso positivo?
6. **Persistenza.** I gap sono viste sempre ricalcolate, osservazioni con
   evidenza, o entrambe le cose? Come invalidiamo un'evidenza diventata vecchia?

### 9.9 Piste consegnabili a nuovi collaboratori

Ogni pista ha un artefatto e un modo di smentirlo, così i lavori possono
procedere in parallelo senza convergere solo su opinioni.

- **Algebra e open world:** definire una semantica a tre valori per O/K/R/V e
  casi limite. Artefatto: tabella di verità + controesempi. Falsificatore: un
  caso `budget_hit` classificato come gap.
- **Riflessione del risolutore:** revisionare `kb_fact/2`, `apply/2` e disegnare
  `kb_rule`. Artefatto: API minima con limiti dichiarati. Falsificatore: una
  regola derivabile segnalata come conoscenza mancante.
- **Contratti semantici:** modellare applicabilità dei frame, dimensioni e forma
  della risposta. Artefatto: fatti `.p0` per almeno giochi e identità.
  Falsificatore: «come si gioca ad algebra» promosso a gap.
- **Probe di superficie:** costruire runner isolato e confronto di tracce.
  Artefatto: corpus generato da seed e variazioni KB. Falsificatore: aggiungere
  o ritirare una variazione non cambia i casi prodotti.
- **Dialetti privati:** produrre candidati con evidenza, senza auto-migrazione.
  Artefatto: ranking su coppie note e coppie-esca. Falsificatore: due relazioni
  omonime/iso-arie ma semanticamente diverse vengono fuse.
- **Valutazione:** sostituire la metrica wall-only con contratti e negative
  controls. Artefatto: report separato per knowledge, reachability, surface e
  wrong-answer gap. Falsificatore: una lista fuori tema conta come risposta.

La domanda da usare in review resta il mantra, applicato al detector stesso:

> Posso insegnare a runtime un nuovo dominio, una nuova politica, una nuova
> variazione o un nuovo rimedio e osservare che emergono — e poi scompaiono per
> ablazione — gap diversi, senza modificare né ricompilare il C?

### 9.10 Che cosa e' cambiato a gen382o — misurato, non progettato

**Osservato.** Il taglio verticale di §9.3 *non funzionava*, e il motivo non era
quello che il §9.3 supponeva («un problema nel passaggio aritmetico»).
L'aritmetica era giusta. Erano giusti anche il conteggio, la riflessione e la
copertura. La regola di maggioranza falliva per due difetti del MOTORE, entrambi
invisibili dall'esterno perche' si presentavano come «nessuna soluzione» — cioe'
esattamente come la risposta legittima del rilevatore.

**a. Cattura di variabile attraverso `findall/3`.** Il sotto-risolutore che
esegue una `findall` ereditava la sostituzione del chiamante ma *ripartiva da
zero il contatore di frame*. Le variabili di clausola sono rinominate
`$Nome_<frame>`: ripartire da zero significa riemettere nomi gia' LEGATI sopra,
quindi una clausola chiamata dentro la `findall` che usa per caso lo stesso nome
di variabile di un antenato ne ereditava silenziosamente il valore.
`observed_domain_attribute($Domain, $Facet, $Entity)` ha `$Entity` in testa, e
anche `sibling_majority_expected($Entity, $Facet)` ce l'ha: l'enumerazione dei
membri collassava sul singolo membro interrogato, il supporto diventava 0 e la
maggioranza non si formava mai.

Riproduzione minima, indipendente dal rilevatore:

```prolog
cap_inner($X, $Y) :- cap_edge($X, $Y).
cap_count($D, $N) :- findall($X, cap_inner($X, $D), $L), count_list($L, $N).
cap_outer($X, $N) :- cap_edge($X, $D), cap_count($D, $N).
```

Con tre `cap_edge(_, d1)`: `cap_count(d1, N)` dava 3 — giusto — e
`cap_outer(a1, N)` dava **1**. La stessa `findall`, due risposte, per il solo
fatto di essere chiamata da una clausola che usa il nome `$X`. Il contatore ora
attraversa il confine in entrambe le direzioni.

Questo difetto non riguardava solo il rilevatore: riguardava **ogni** procedura
insegnabile che piega una lista dentro una regola. Era un tributo silenzioso su
tutta la strada `teachable-procedures`.

**b. Il fondo della pista di sostituzione.** Una sostituzione e' una TRACCIA:
`count_list/2` lascia dietro di se' ogni legame intermedio, quindi piegare una
lista di N costa ~4N slot e due pieghe nella stessa congiunzione ~8N. Con
`KB_MAX_BIND` a 128 il tetto stava a una coorte di ~15, e i quindici esperti di
gioco ci stavano sopra ESATTAMENTE. Effetto: il rilevatore perdeva ogni attributo
con supporto PIENO e teneva solo quelli sparsi — cioe' produceva, dal proprio
punto di vista, «qui non ci si aspetta niente». Il tetto e' salito a 384 (il lato
variabile di un legame e' stato ristretto a 96 byte, che e' anche il motivo per
cui la traccia piu' lunga non costa piu' memoria in proporzione).

**c. `incomplete` non e' piu' assorbito in `false`.** Erano due, i modi di
tornare a mani vuote, e il motore ne dichiarava uno solo. Ora:

- `goal_provable` restituisce tre valori (provata / fallimento finito /
  incompleta) e la negazione per fallimento **declina** invece di riuscire quando
  la ricerca e' stata tagliata dal budget;
- la `findall` propaga il proprio budget e il proprio `budget_hit` al chiamante;
- l'esaurimento della pista di legami alza la stessa bandiera.

E' la richiesta di §9.1 resa esecutiva: un budget esaurito non puo' diventare un
`gap`. Prima poteva, e nessuno se ne sarebbe accorto.

**d. La vista riflessiva e' indicizzata.** `kb_fact/2` con il predicato gia'
legato — la forma comune quando un rilevatore ha scelto una faccetta e ne sta
contando il supporto — visita il censimento di quel predicato invece dell'intera
KB. Stessa semantica, `knowledge_gap(poker, ?)` da oltre due minuti a ~4 s.

Nessuna di queste quattro e' vocabolario: sono meccanica del risolutore, cieca a
lingua, dominio e tipo di lacuna.

### 9.11 Che cosa e' PROVATO, e che cosa no

`tests/p0t/meta/knowledge_gap.p0t` — 14 assert, dominio sintetico. Copre i punti
di §9.4 nell'ordine: la lacuna emerge (1); si chiude asserendo il fatto e
riapre ritirandolo (2, 3); **ritirando `gap_source/3` il rilevatore sparisce e
riasserendolo torna, senza ricompilare** (4); un supporto su tre non basta (5);
una coorte di due non basta (6); il supporto si conta per membri distinti, non
per fatti; **una copertura DERIVATA da una clausola non produce un falso gap**
(il punto 7 di §9.4).

Per l'ultimo e' servita una capacita' nuova del banco: `!clause <testo>` nel
test-engine, il gemello di `!assert` un piano piu' su. `!assert` sapeva scrivere
un fatto ground; una prova che parla di REGOLE non era esprimibile in `.p0t` e
finiva in uno script di shell sul motore MCP — la stessa asimmetria che gen345
aveva chiuso per i fatti. Ora passa dal parser `.p0` completo.

**Non provato:** il punto 8 di §9.4 (budget esaurito ⇒ `incomplete`, mai `gap`).
Il meccanismo c'e' (§9.10c) ma non ha un test che lo eserciti: serve un caso che
faccia sforare il budget in modo deterministico.

**Difetto aperto nel banco, non nel rilevatore.** Il file passa 14/14 su un
motore appena avviato e cade sul PRIMO blocco quando nella stessa esecuzione lo
precede un altro file — mentre i sette blocchi successivi, identici per forma,
passano. Qualcosa sopravvive al `!reset` fra un file e il successivo e nessuna
combinazione di `!set` (BASE, PROFILE, WORLD_FACTS, LANG) lo ha rimesso a posto;
un secondo `!reset` consecutivo viene saltato dalla logica «reset intelligente».
Per questo la riga nel `Makefile` e' commentata: mettercela significherebbe
scegliere fra una suite rossa e un'aspettativa indebolita fino a passare per il
motivo sbagliato. **E' il primo lavoro del prossimo giro** — finche' non e'
capito, ogni misura fatta dentro la suite e' sospetta, non solo questa.

### 9.12 Il primo risultato sulla KB vera: uno ZERO, e perche' e' informativo

Con il profilo `agi` (26 702 fatti) il rilevatore di asimmetria fra fratelli
trova **zero lacune** — su tutti e quindici gli esperti di gioco, e sui cinque di
programmazione e tre di matematica campionati.

Non e' un guasto. I quindici file di gioco dichiarano tutti e dieci gli stessi
attributi (`expert_description`, `means`, `game_players`, `game_goal`,
`game_setup`, `game_play`, `game_end`, `game_tip`, `category_surface`): supporto
15 su 15, popolazione 15, nessun membro in debito. Il profilo atteso e' saturo
perche' quei file sono stati scritti dallo stesso stampo.

Che lo zero sia una MISURA e non un silenzio e' verificato per differenza sulla
KB reale, non su un giocattolo:

```
knowledge_gap(poker, ?)                    -> []
retract game_tip(poker, "…")
knowledge_gap(poker, ?)                    -> [game_tip]
knowledge_gap_remedy(poker, ?)             -> [ask_user]
```

Questa e' la risposta empirica alla domanda di §2, ottenuta pero' **dall'interno**
invece che da un bench in Python: la sorgente 4a e' *strutturalmente incapace* di
trovare le lacune che hanno aperto questo documento. «Quante carte ha il poker»
non e' un attributo che i fratelli dichiarano e il poker no: non lo dichiara
nessuno. Un template uniforme rende la sorgente 4a muta esattamente dove la KB e'
piu' fragile — **e la sua saturazione e' essa stessa il segnale**: dice che il
prossimo lavoro non e' affinare 4a, ma aprire una sorgente che non dipenda dal
consenso fra fratelli.

### 9.14 CORREZIONE DI ROTTA (F., gen382o): il bersaglio e' il PONTE

> «Impedire che la conoscenza non emerga per colpa del gap di metaconoscenza
> dialogica.»

Il documento si era spostato sul contenuto: *quale fatto manca*. Il bersaglio e'
un altro e piu' stretto: **quale conoscenza c'e' e non riesce a uscire**, perche'
manca il pezzo di meta-conoscenza che collega una domanda alla relazione che la
risponde. Anche quel pezzo e' conoscenza — quindi anche lui ha uno spazio
negativo, e si misura con lo stesso strumento.

Le due misure di gen382o, messe una accanto all'altra, dicono esattamente questo:

```
rilevatore puntato sul MONDO   (coorti expert_domain)   -> 0 lacune su 15+8 esperti
rilevatore puntato sul PONTE   (dimensioni confrontabili) -> 1 lacuna su 7, al primo colpo
```

**Il caso, per intero.** `magnitude/3` ha 7 dimensioni. Sei hanno una parola che
le raggiunge (`magnitude_cue/3`); `population` no.

```prolog
mag_dim($D)     :- kb_fact(magnitude, cons($D, cons($I, cons($R, nil)))).
mag_door($D)    :- magnitude_cue($W, $D, $M).
mag_no_door($D) :- mag_dim($D), naf(mag_door($D)).      % -> [population]
```

La conoscenza c'e' e ordinata: `magnitude(population, china, 11)`,
`magnitude(population, india, 12)`. Il motore di confronto generale c'e' e
funziona. Manca solo la parola che li congiunge. Verifica per differenza, in
conversazione:

```
which river is longer, the nile or the amazon   -> Nile.          (length HA la porta)
e piu forte il full o il poker                  -> Full_house.    (hand_rank, porta aggiunta a mano a gen382l)
quale paese e' piu popoloso                     -> Non capisco ancora.   ← la lacuna, predetta
```

**E il muro non e' il danno peggiore.** Le stesse domande in inglese non vanno a
muro: vanno a una relazione VICINA che risponde a una domanda diversa.

```
which country has the largest population -> Russia.        (falso)
which country has the largest area       -> Russia.        (vero)
which has the bigger population, china or india -> 1400000000.   (un lookup, non un confronto)
```

`largest_in_category(country, russia)` non ha uno slot per la DIMENSIONE, quindi
ogni «largest <categoria>» ci arriva, qualunque cosa si stesse chiedendo. La
lacuna di meta-conoscenza non produce silenzio: produce **una risposta sbagliata
detta con sicurezza** — mantra #7 violato non da una regola sbagliata, ma da un
ponte assente che lascia vincere il ponte accanto.

**Che cosa cambia nel meccanismo: niente.** `gap_source(Type, Obligation,
Coverage)` e' gia' cieca a che cosa siano obbligo e copertura. Cambia solo dove
la si punta. E il pezzo che serviva era gia' scritto senza saperlo: il
`naf(machinery($Facet))` messo per tenere la macchineria FUORI dal profilo del
mondo e' esattamente l'interruttore che, invertito, porta il rilevatore DENTRO
lo strato dialogico.

**La forma generale.** Ogni registro di superficie apre una relazione:
`answer_frame(Cue, Pred)`, `aggregate_frame(Cue, Pred, …)`, `question_form(…)`,
`magnitude_cue(Word, Dim, Mode)`, `intent_cue(…)`. L'obbligo e' «questa
relazione (o questa chiave dentro la relazione) ha fatti, quindi qualcuno la
chiedera'»; la copertura e' «un registro la nomina». Un registro nuovo costa un
fatto:

```prolog
gap_consumer(magnitude_cue, magnitude, 2).   % apre magnitude(Dim, …) nominando Dim
gap_consumer(answer_frame,  any,       2).
```

**Il falso allarme da evitare, dichiarato prima di sbagliarlo.** `answer_frame`
non e' l'unico consumatore: molti moduli leggono una relazione direttamente dal
C. Contare «le relazioni senza `answer_frame`» darebbe decine di allarmi falsi —
lo stesso errore di §7 («come si gioca ad algebra»). Il conteggio ha senso solo
quando l'INSIEME dei registri e' esso stesso un fatto, e quando ogni consumer
in C dichiara la relazione che consuma. Fino ad allora si misura per registro,
come sopra: li' la copertura e' esatta e il risultato e' falsificabile parlando.

### 9.13 Da dove riprendere, in ordine

1. **Chiudere il difetto di isolamento del banco** (§9.11) e rimettere
   `knowledge_gap.p0t` in `make test`. Prima di questo, nessuna misura nuova.
2. **Rendere la coorte un fatto.** Oggi `expert_domain/2` e' scritto dentro
   `observed_domain_attribute`. Deve diventare `gap_cohort(Source, Relation)`,
   cosi' che (a) applicare la stessa sorgente alle classi `is_a` costi UN fatto,
   e (b) la relazione che DEFINISCE il gruppo smetta di comparire fra gli
   attributi dei suoi membri — l'esclusione strutturale chiesta da §9.4, per
   regola e non per lista.
3. **La sorgente «relazione senza porta», per registro** (§9.14). E' la seconda
   sorgente da fare, non l'entita' opaca: ha gia' un caso vero trovato
   (`population`), un rimedio che e' UN fatto (`magnitude_cue(populous,
   population, max)`) e una verifica che si fa parlando. L'entita' opaca resta
   dopo, quando ci saranno i ruoli degli argomenti di §9.6.
4. **La superficie (§4e).** Resta la sorgente piu' feconda e la sola che produce
   lacune misurabili senza conoscenza nuova. Non e' stata toccata. Sotto la
   lettura di §9.14 e' anche la piu' centrale: «come tichiami» e' il ponte
   raw->canonico che si rompe, non un fatto che manca.
4b. **La dimensione mancante in `largest_in_category/2`** (§9.14): una relazione
   senza slot per la dimensione fa sbagliare, non tacere. E' il primo rimedio da
   applicare perche' produce gia' oggi risposte false.
5. **Le domande, non le faccette.** Oggi il consumer risponde `game_tip`. La
   forma interrogativa e' gia' in KB: `answer_frame(Cue, Pred)` dice con quali
   parole si chiede quella relazione. `gap_question(Entity, Cue) :-
   knowledge_gap(Entity, Facet), answer_frame(Cue, Facet).` e' una clausola, e
   trasforma l'elenco di faccette nell'elenco di DOMANDE che §8 chiede — usando
   il registro che esiste, senza una lista nuova.

---

## 10. Punto della situazione dopo alcune elaborazioni

*Ragionamento generale, gen382o. Nasce da una correzione di F.: il bersaglio non
e' trovare lacune di conoscenza, e'* **impedire che la conoscenza non emerga per
colpa del gap di metaconoscenza dialogica**. *Questa sezione tira le somme al
livello di struttura, non di casi. I casi che l'hanno provocata stanno in §9.14.*

### 10.1 Che cosa cambia davvero la riformulazione

Ci sono due strati. **W**, la conoscenza del mondo: relazioni con fatti. **M**, la
meta-conoscenza dialogica: i legami che portano un enunciato dentro W e ne
riportano indietro una risposta ben formata. Un turno riesce se esiste un
*cammino* prompt -> M -> W -> M -> risposta.

La differenza fra i due non e' di contenuto, e' di **algebra**:

- **W cresce per addizione** — un fatto in piu', una risposta in piu';
- **M cresce per moltiplicazione** — un legame in piu' rende raggiungibile
  un'intera regione di W.

Ne segue immediatamente che ogni metrica di contenuto — numero di fatti,
copertura di dominio, wall-rate per entita' — e' **cieca a M per costruzione**,
perche' M non aggiunge contenuto: cambia soltanto quanto del contenuto esistente
sia attingibile. E' la ragione strutturale di §2: un bench derivato da cio' che la
KB *afferma* non puo' vedere M nemmeno in linea di principio.

### 10.2 Perche' una lacuna in M e' peggio di una in W

Una lacuna in W produce un muro, ed e' onesta. Una lacuna in M non produce
silenzio, e questo e' il punto generale:

> **Il dispatch e' competitivo.** Se il ponte giusto manca, il turno non va da
> nessuna parte — va al *migliore dei ponti rimasti*. L'assenza dentro un router
> competitivo non e' silenzio: e' **instradamento sbagliato**.

Quindi le lacune di M sono l'unica classe che fabbrica inganno *a partire da
conoscenza corretta*: la KB sa la cosa giusta e il sistema dice quella sbagliata,
con sicurezza. Rispetto alla dottrina anti-inganno di `PRINCIPLES.md` e al mantra
#7 questa e' la classe prioritaria — non per eleganza, per gravita'.

### 10.3 Perche' sono anche l'unica classe DECIDIBILE

Su W la domanda «manca il fatto F?» non ha risposta senza mondo chiuso o oracolo.
E' esattamente per questo che §4a ha dovuto ripiegare su un'euristica di
maggioranza, ed e' per questo che si satura: legge il consenso fra fratelli, e
fratelli scritti dallo stesso stampo non hanno consenso da rivelare.

Su M la domanda e' di un'altra specie. M e' un insieme di archi fra cose che
stanno **entrambe dentro il sistema**. «La relazione R e' raggiungibile da
qualche superficie dichiarata?» e' una domanda sul grafo che la KB ha di se'.
Niente oracolo, niente mondo aperto, niente soglie.

> Lo spazio negativo di M e' **calcolabile**; quello di W e' al massimo
> **stimabile**.

Che il rilevatore puntato sulle coorti desse zero e puntato sui ponti colpisse al
primo tentativo (§9.14) non e' fortuna: e' questa asimmetria.

### 10.4 Le tre forme dello spazio negativo di M, e sono esaustive

M e' bipartito: superfici <-> relazioni. Ogni fatto di M e' un arco. Quindi il
suo spazio negativo ha esattamente tre forme:

1. **Relazione senza porta** — R ha fatti, nessun arco la raggiunge. Conoscenza
   che non si puo' chiedere.
2. **Porta senza relazione** — un arco punta a R, R non ha fatti. Domanda capita,
   niente dietro.
3. **Porta che non discrimina** — piu' regioni di W condividono un arco, e l'arco
   non ha di che distinguerle.

La terza e' quella che nessuno enumera, perche' **entrambi gli estremi
esistono**: l'arco c'e', e' solo sotto-specificato. Ed e' anche l'unica delle tre
che produce risposte false invece di muri.

Ha inoltre un rapporto preciso col mantra #3. «Astrai fino al punto fisso» dice
normalmente di *fondere* relazioni viste attraverso verbi diversi; lo stesso
criterio letto al contrario dice che un arco il cui fan-in supera la propria
capacita' di discriminare non chiede una fusione, chiede **uno slot**. Stesso
test, direzione opposta.

### 10.5 La conseguenza operativa piu' forte: il ciclo si chiude solo su M

Il rimedio a una lacuna di M e' **sempre esprimibile come un singolo fatto di
M**, perche' la lacuna e' definita come arco mancante in una struttura bipartita
gia' dichiarata. Per W non e' vero: il rimedio a «quante carte ha il poker» e'
andare a saperlo.

```text
lacuna in W  ->  rimedio = ACQUISIZIONE   (esterna, costosa, fallibile)
lacuna in M  ->  rimedio = ENUNCIAZIONE   (interna, un fatto, verificabile subito)
```

Quindi il ciclo *emersione -> rimedio -> verifica* si chiude **senza uscire dal
sistema** solo sul lato M. §6 immaginava di chiuderlo col sogno, ma il sogno non
puo' chiuderlo su W senza una fonte. Su M si', e la verifica e' doppia:
rieseguire il rilevatore (la lacuna sparisce) e riprovare **parlando** (il turno
cambia). E' il ciclo autonomo che il progetto cerca — sta sull'altro strato
rispetto a dove lo stavamo cercando.

### 10.6 La richiesta strutturale al codice che ne discende

Se M e' dove sta la leva, allora ogni modulo C che consulta una relazione **senza
dichiarare quale relazione consuma** e' un buco nell'osservabilita' di M. Non
perche' sia sbagliato: perche' rende invisibile al rilevatore un arco che nel
sistema esiste. Il grafo che il rilevatore legge non e' il grafo su cui il sistema
gira, e ogni conteggio globale diventa rumore — lo stesso errore di §7.

E' una richiesta piu' tagliente del mantra «niente vocabolario nel C»:

> **Ogni consumer in C dichiara il proprio arco come fatto.**

Finche' non vale, «relazioni senza porta» non e' misurabile in generale: solo per
registro, un registro alla volta — dove pero' la copertura e' esatta e il
risultato e' falsificabile parlando.

### 10.7 Riclassificazione delle cinque sorgenti di §4

Erano ordinate per *dove sta il buco nella KB*. Vanno ordinate per **quale
freccia della pipeline e' rotta**:

| freccia | sorgente |
|---|---|
| grezzo -> canonico | §4e superficie |
| canonico -> relazione | relazione senza porta; porta che non discrimina |
| relazione -> dato | porta senza relazione; §4d dialetti privati |
| dato -> risposta | formato e realizzazione (il V(q) di §9.1) |

Da qui due semplificazioni vere:

- **§4d non e' un fenomeno a se'.** Un dialetto privato *e'* una relazione senza
  porta, vista dal lato del dato.
- **Delle cinque sorgenti, solo §4c (entita' opache) e' davvero una lacuna di
  W.** Le altre quattro parlavano gia' del ponte, e il documento non se n'era
  accorto.

Non sono cinque problemi: e' **un problema con quattro facce, piu' uno di natura
diversa**.

### 10.8 Che cosa renderebbe falsa questa tesi

Da mettere per iscritto adesso, prima di innamorarsene. Se M fosse davvero un
moltiplicatore, aggiungere **un** arco deve rendere risponibili **molte**
domande.

> Falsificatore: misurare, dopo l'aggiunta di un singolo fatto di M, quante
> domande prima murate ora passano. Se il numero e' circa 1, M non moltiplica —
> e' solo un'altra forma di contenuto, e §10 e' una descrizione elegante di
> niente.

E' anche la metrica giusta per ordinare i rimedi: il valore di un arco e' la
dimensione della regione di W che sblocca, non il fatto di esistere.

---

## 11. Il metodo: far uscire la conoscenza presente

*gen382p. §10 ha stabilito che il bersaglio e' il ponte. Questa sezione e' il
metodo, ed e' implementato: quattro clausole e tre fatti, **zero C**.*

### 11.1 Il metodo, in una riga

> Il rilevatore di §9 cerca conoscenza che MANCA. Questo cerca l'altra cosa, ed e'
> peggiore: **conoscenza che C'E' e non ha una porta da cui uscire.**

E la forma e' gia' quella di §9, perche' `gap_source(Type, Obligation, Coverage)`
e' cieca a che cosa siano obbligo e copertura. Cambia solo dove la si punta:

```prolog
% Le relazioni che parlano di un'entita' come SOGGETTO — la forma della KB
% letta com'e', nessun elenco. machinery/1 e' la frontiera, ed e' un fatto.
entity_facet($Entity, $Facet) :-
    kb_fact($Facet, cons($Entity, cons($Value, nil))),
    naf(machinery($Facet)).

% Una faccetta e' RAGGIUNGIBILE se un registro di superficie la nomina.
facet_reachable($Facet) :- answer_frame($Cue, $Facet).
facet_reachable($Facet) :- aggregate_frame($Cue, $Facet, $Ret, $Mode).
facet_reachable($Facet) :- question_form($Cue, $Facet).
facet_reachable($Facet) :- consumer_reads($Module, $Facet).

surface_reachable($Entity, $Facet) :- facet_reachable($Facet).
```

e la sorgente costa **esattamente cio' che era stato promesso a §5**, tre fatti:

```prolog
gap_source(bridge_gap, entity_facet, surface_reachable).
gap_remedy(bridge_gap, declare_surface).
```

Il rimedio non e' «vai a imparare»: e' **«dillo»**. Una lacuna di ponte si chiude
enunciando un fatto di cui il sistema possiede gia' tutti gli elementi — la
relazione la conosce, e la superficie gliel'ha appena data l'utente sbagliando la
domanda (§10.5).

### 11.2 La misura, e il moltiplicatore

Puntato sul poker, il rilevatore ha risposto subito:

```
entity_facet(poker, ?)        -> 10 relazioni
unreachable_facet(poker, ?)   -> [expert_domain, expert_description,
                                  game_end, category_surface]
gap_record(?, poker, ?, ?)    -> bridge_gap          (raccolto da solo)
```

`game_end` e' conoscenza del mondo vera, presente per **tutti e quindici** i
giochi, e non esisteva modo di chiederla. In `kb/core/intents.p0` erano state
scritte otto porte a mano — `game_play`, `game_goal`, `game_players`,
`game_setup`, `game_tip`, `game_component`, `game_summary` — e una era stata
dimenticata. **Nessuna misura di copertura poteva accorgersene, perche' il fatto
c'era.** E' esattamente il modo in cui sbagliano gli umani, ed e' quello che il
rilevatore trova da solo.

Verifica prima, parlando:

```
how do you play poker      -> Betting rounds let players check, bet, …   (ha la porta)
how does a game of poker end -> I don't understand that yet.             (non ce l'ha)
how does a game of chess end -> Hmm, that's a bit beyond me right now.
```

Poi **un solo fatto**, asserito a runtime, senza ricompilare:

```prolog
answer_frame("how does a game", game_end).
```

Risultato su quattordici giochi, tutti prima murati:

```
chess       -> The game ends by checkmate, resignation, draw agreement, …
checkers    -> A player wins when the opponent has no pieces or no legal move; …
go          -> After both players pass, dead stones are resolved and scored.
backgammon  -> The first player to bear off every checker wins, …
poker       -> A hand ends when one player remains or the showdown awards the pot.
blackjack   -> Compare every unbusted player hand with the dealer and settle …
bridge      -> Score the contract from tricks made or penalties, …
monopoly    -> The last player not bankrupt wins; …
scrabble    -> When no tiles remain and a player empties the rack, …
dominoes    -> A round ends when a player goes out or no player can move, …
sudoku      -> The puzzle is complete when every cell is filled …
mahjong     -> Score the winning hand or record an exhaustive draw, …
tic_tac_toe -> Stop at the first line of three matching marks …
risk        -> The first player to satisfy the domination, capital, or mission …
```

> **UN arco -> QUATTORDICI domande.** Il falsificatore dichiarato a §10.8 chiedeva
> che un fatto di M sbloccasse *molte* domande, non una: se ne sbloccasse una
> sola, M non moltiplicherebbe e §10 sarebbe una descrizione elegante di niente.
> Passa con largo margine, e nessuna di quelle risposte e' conoscenza nuova.

E il ciclo si chiude in modo osservabile: dopo il fatto,
`unreachable_facet(poker, ?)` non contiene piu' `game_end`. **Emersione ->
rimedio -> verifica**, doppia (strutturale e conversazionale), senza uscire dal
sistema.

### 11.3 Il reperto piu' interessante: una lacuna di meta-conoscenza SULLA meta-conoscenza

L'italiano non si e' sbloccato. Le stesse domande in italiano restavano al muro
anche dopo aver dichiarato `answer_frame("come finisce", game_end)`, e il motivo
non era la conoscenza:

```
come finisce una partita a poker  ->  "Mmh, non conosco ancora finish. …"
```

Il verbo era gia' stato normalizzato: la canonicalizzazione porta *finisce* a
*finish* e lascia cadere *come* come riempitivo interrogativo, **prima** che il
frame veda il turno. Quindi il cue va scritto nella forma **canonica**, non nella
superficie — e in inglese le due coincidono per caso, in italiano no.

Conseguenza generale, e va detta con precisione:

> Il punto di estensione KB-first e' **silenziosamente asimmetrico fra lingue**.
> Insegnare una porta funziona in inglese e fallisce in italiano, per una ragione
> che chi insegna non ha modo di vedere.

Il rimedio non e' aggiustare l'italiano a mano — quello e' il fix puntuale. E'
**rendere osservabile la forma canonica di un turno**, cosi' che una porta si
possa dichiarare contro cio' che il matcher effettivamente vede. E' il gemello
esatto della richiesta di §10.6: li' era «ogni consumer in C dichiara il proprio
arco», qui e' «ogni superficie e' ispezionabile nella forma in cui viene
confrontata». Senza le due, M esiste ma non e' osservabile, e un rilevatore che
legge un grafo diverso da quello su cui il sistema gira produce rumore.

### 11.4 Perche' questo e' il posto giusto per il muro

Oggi `not_understood()` in `src/brain/99-registry.c` contiene un **frasario
bilingue in C** (`v_en[]`, `v_it[]`) che duplica
`response_template(dont_understand, …)` gia' presente in KB — mantra #2 violato
nel punto esattamente piu' importante del sistema, quello in cui parrot0 ammette
di aver fallito.

E c'e' di peggio, ed e' un reperto: un registratore di lacune **esiste gia'**
(`pending_gap` / `pending_gap_question`), ma e' annidato nel ramo anti-ripetizione
— scatta solo quando la risposta *starebbe per ripetersi*, solo su una parola
ignota di almeno sei caratteri, e tiene al massimo UNA lacuna per volta. Cioe':
**il sensore delle lacune e' un effetto collaterale di una correzione di
naturalezza.** Alla prima occorrenza non registra niente, e su un topic NOTO non
registra mai.

Il muro e' il punto in cui il sistema sa di piu' e butta via tutto: sa quali
moduli hanno rinunciato, sa che l'entita' e' nota, sa — perche' il punteggiatore
di `universal-input` lo calcola — che forma ha la domanda. Misurato:

```
input.classify(intent_cue, "quante sono le carte del pocker")
   -> winner: arith_count_request, score 3, because intent_cue(…, keyword(quante))
```

Il classificatore **capisce** che e' una richiesta di conteggio. Il dispatch non
lo consulta. E la stessa domanda che invece FUNZIONA — «quanti giocatori servono
a poker» — viene classificata `ambiguous` dallo stesso punteggiatore, perche'
risponde per un'altra via (`answer_frame` -> `game_players`). Cioe':

> **Il punteggiatore KB-first di `universal-input` e la catena dei ~70 moduli
> `mod_*` sono due mondi paralleli e scollegati.** Uno raggiunge un verdetto che
> nessuno usa; l'altra risponde per confronto di sottostringhe; il muro finale
> scarta entrambi.

E' questa la ragione strutturale per cui i moduli `mod_*` vanno ripensati sopra
`universal-input` / `universal-comprehension`: non e' una questione di eleganza.
Un dispatch a **primo-che-rivendica**, con l'ordine cablato nel C e un ritorno
0/1 senza motivazione, non puo' produrre il record che serve all'autodiscovery —
per costruzione non sa dire *chi ha quasi risposto e perche' no*.

### 11.5 Da dove riprendere

1. **Il muro diventa un declino informato e un sensore.** Frasario fuori dal C;
   prima di murare, comporre il residuo del turno (entita' nota + verdetto del
   punteggiatore) ed emetterlo come fatto tipizzato. Da li' la conversazione
   diventa la sorgente di lacune piu' produttiva che abbiamo — la scansione
   strutturale sulle coorti dava zero (§9.12), il primo turno reale ne da' una.
2. **Osservabilita' di M** (§11.3 + §10.6): forma canonica ispezionabile, e ogni
   consumer in C che dichiara `consumer_reads(Module, Relation)` — il predicato
   e' gia' previsto in `facet_reachable/1`, oggi senza fatti.
3. **Dispatch per evidenza invece che per ordine.** Solo dopo (1) e (2), perche'
   senza osservabilita' non si puo' misurare se un dispatch nuovo migliora.
4. Poi le sorgenti rimaste di §4, con la superficie (§4e) per prima.

---

## 12. Correzione: M non e' bipartito — c'e' un terzo asse

*gen382q. Questa sezione nasce da un errore mio, corretto da F., e la correzione
vale piu' del fix che ne e' seguito.*

### 12.1 L'errore

Avevo scritto, chiudendo §11, che «quante carte ha il poker» non era curabile con
il metodo perche' il numero di carte **non e' nella KB** — quindi lacuna di W,
non di ponte. Era falso, e il dato c'era in **due** forme indipendenti:

```prolog
% kb/core/world-facts.p0 — sempre caricato, e con un consumer che FUNZIONA
quantity(deck, cards, 52).

% kb/experts/games/standard_cards.p0 — nel profilo agi, per estensione
playing_card(two_of_clubs, two, clubs, 2).   % … x52
```

E la prova che non era una lacuna di conoscenza si fa in un turno:

```
how many cards in a deck  ->  A deck has 52 cards.      ← il dato risponde
how many cards in poker   ->  I don't understand that yet.
```

Stesso dato, stessa relazione, stesso consumer, stessa lingua. Cambia solo il
**nome dell'entita' nella domanda**.

Vale la pena registrare *come* ho sbagliato, perche' e' lo stesso errore che il
documento denuncia: ho concluso «il dato non c'e'» da «il turno non risponde».
E' esattamente l'inferenza che §10.2 dichiara illecita — l'assenza di risposta
non e' assenza di conoscenza. Averla fatta mentre scrivevo il capitolo che la
vieta e' la misura di quanto sia naturale.

### 12.2 La correzione strutturale

§10.4 sosteneva che lo spazio negativo di M avesse **tre forme esaustive**,
perche' M e' bipartito: superfici <-> relazioni. La tesi era troppo stretta. M ha
un **terzo asse**, quello delle entita':

```text
asse 1   superficie <-> relazione     "con quali parole si chiede R"
asse 2   relazione  <-> dati          "R ha fatti"
asse 3   entita'    <-> entita'       "il nome nella domanda e il nome nel fatto
                                       sono la stessa cosa, o l'uno implica l'altro"
```

Le tre forme di §10.4 restano esaustive **per gli assi 1-2**. L'asse 3 e' un'altra
famiglia, e ci vivono almeno:

- **etichetta**: `scacchi` e `chess`, `mazzo` e `deck` — la stessa cosa sotto due
  nomi (`concept_label/4` esiste, e non copre questi);
- **implicazione**: `poker` non *e'* un `deck`, ma lo **richiede**, e una
  grandezza dell'attrezzo e' una grandezza dell'attivita';
- **granularita'**: `quantity("chess board", squares, 64)` sta sotto una chiave di
  due parole, e la domanda dice «chess».

E' un asse peggiore degli altri due, perche' e' l'unico in cui il ponte mancante
non e' visibile nemmeno enumerando i registri: entrambe le entita' esistono,
entrambe hanno fatti, la relazione ha una porta. Non manca niente di
enumerabile. Manca una **relazione fra due cose note**.

### 12.3 Il rimedio, e perche' non e' sui giochi

Nessuna relazione nuova: `requires/2` esisteva gia' in `world-facts.p0`, con
arieta' dichiarata in `meta.p0`, e dice esattamente questo (mantra #3 e #5).
L'arco e' un fatto per gioco; l'ereditarieta' e' **una regola sola**, e non parla
di giochi:

```prolog
quantity($Activity, $Part, $N) :-
    requires($Activity, $Thing),
    quantity($Thing, $Part, $N).

requires(poker, deck).   requires(blackjack, deck).   requires(bridge, deck).
```

```
how many cards in poker      ->  A poker has 52 cards.
how many cards in blackjack  ->  A blackjack has 52 cards.
how many cards in bridge     ->  A bridge has 52 cards.
```

La regola vale anche per `requires(car, fuel)`, `requires(computer, electricity)`
e ogni altra coppia gia' in KB: e' un moltiplicatore sull'asse 3 come
`answer_frame` lo era sull'asse 1.

**Difetto residuo, dichiarato invece che nascosto:** «A poker has 52 cards» ha
l'articolo sbagliato. La conoscenza e' giusta, la realizzazione no — e' il quarto
anello di §10.7 (dato -> risposta), che questo giro non tocca.

### 12.4 Che cosa cambia nel metodo

Il rilevatore di §11 **non avrebbe trovato** questa lacuna: `unreachable_facet/2`
guarda gli assi 1-2, e li' era tutto a posto. Serve un obbligo di specie diversa,
e la forma non e' ancora chiara — enumerare le coppie di entita' che *potrebbero*
essere collegate genera rumore illimitato, cioe' esattamente l'errore di §7.

L'unica sorgente che vede l'asse 3 senza inventare e' quella che F. indicava dal
principio: **la domanda vera**. Se un turno nomina un'entita' nota, chiede una
relazione che ha una porta, e non ottiene risposta, allora o il fatto manca per
quell'entita' — oppure manca l'arco verso l'entita' che il fatto ce l'ha. Sono due
ipotesi, entrambe enunciabili, ed entrambe verificabili con una domanda di
rimando all'utente.

Il che rafforza §11.5 punto 1 e ne cambia il rango: il muro che compone il residuo
del turno non e' *uno* dei prossimi passi. E' **l'unico sensore che copre tutti e
tre gli assi**, e va fatto per primo.
