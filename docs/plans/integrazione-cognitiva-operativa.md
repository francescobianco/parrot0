# parrot0 organico — piano operativo di integrazione e addestramento

*2026-09-08. Richiesta di F.: rendere la memoria profonda un'abilità cognitiva,
risolvere i problemi comportamentali attraverso la comprensione universale,
organizzare e addestrare tutte le capacità verso l'equivalenza funzionale con
un LLM. Ricognizione statica sul checkpoint `e66792b3` (`gen506h`); alcuni
handoff del repository sono datati 9 settembre. Questa stesura non esegue
training né nuove misure comportamentali: distingue il codice ispezionato dai
risultati storici e dalle prove ancora da fare.*

> **Stato (gen506i–j, 9 settembre 2026):** il primo ciclo O0→O1 e' eseguito —
> il tabellone delle questioni e' uno (I1 di dialogica) e l'orologio della
> conversazione e' uno; il circuito 2 (O3, la ritenzione come regola sul
> contenuto) e' fatto. Report in **§11**; il runbook operativo dei prossimi
> circuiti (`move_addresses`, supersede/resume, la lettura che chiude la
> questione) in **§12**.

Il risultato da costruire è **un interlocutore che comprende una richiesta,
mantiene il suo scopo, combina le capacità disponibili e modifica il proprio
comportamento attraverso ciò che impara**. La memoria profonda è la prima
catena completa sulla quale rendere osservabile questa organizzazione.

Questo piano rende eseguibili le tappe V0–V7 di
[apprendimento assistito §0](apprendimento-assistito.md#0-missione-principale-la-kb-viva).
Riprende [armonizzazione-piani](armonizzazione-piani.md), stabilendo oggetti
condivisi, dipendenze, incrementi, curriculum e condizioni di uscita. Non
assegna nuove percentuali di completamento ai lavori precedenti.

## 1. Che cosa significa «organico», e come si riconosce

Una capacità partecipa all'organizzazione cognitiva quando:

1. **Comprende attraverso la lettura comune.** Consuma referenti, ruoli,
   scope, vincoli e alternative del turno; rende visibili le parti irrisolte.
2. **Contribuisce alla questione.** Sa quale bisogno soddisfa, quali
   precondizioni richiede e quali risultati offre alle altre capacità.
3. **Può essere insegnata e corretta.** Una spiegazione naturale modifica
   forme, regole, procedure, criteri di scelta o realizzazione dal turno dopo.
4. **Trasferisce.** La conoscenza appresa diventa utilizzabile in un altro
   compito e, per un operatore generale, in almeno tre domini.
5. **Ricorda con sostegni.** Conserva origine e condizioni di validità;
   correggere una premessa cambia le conclusioni che ne dipendono.
6. **Sa proseguire.** Un chiarimento, una lettura o un risultato di strumento
   riattiva il bisogno originario senza obbligare l'utente a ripetere tutto.

Le sei condizioni vanno provate insieme su conversazioni. Una nuova cue può
aprire una superficie; un nuovo elenco di cue non dimostra da solo nessuna
delle altre cinque condizioni.

La tesi di lavoro è che **IR universale e apprendimento continuo si rafforzino
a vicenda**: la rappresentazione condivisa rende applicabile una lezione in
più luoghi; l'apprendimento amplia le distinzioni che la rappresentazione sa
esprimere. È un'ipotesi sperimentale, coerente con
[PRINCIPLES](../../PRINCIPLES.md), da valutare attraverso trasferimento,
composizione e comportamento su casi nuovi. La sola adozione di una IR non
dimostra comprensione universale né garantisce la convergenza a un LLM.

Per «equivalente a un LLM» adottiamo il bersaglio di
[frontier §9](frontier-kb-natural-dialogue.md#9-confronto-empirico-con-un-llm-di-frontiera):
equivalenza funzionale sul perimetro misurato, nella comprensione, nelle mosse,
nei risultati e nella continuità. Il perimetro deve poi allargarsi. La forma
interna del sistema può essere diversa; una demo o una suite finita non
autorizza una dichiarazione di equivalenza universale.

## 2. Punto di partenza: che cosa riusare e che cosa resta aperto

Le righe «presente» della tabella sono constatazioni statiche al checkpoint
indicato. I test citati sono reperti da riutilizzare, non risultati rieseguiti
da questo piano.

| Giunzione | Evidenza corrente | Conseguenza operativa |
|---|---|---|
| Frame → scelta | [turn-frames.p0](../../kb/core/turn-frames.p0), [dialogue-policy.p0](../../kb/core/dialogue-policy.p0): `frame_decision` consuma le priorità delle mosse. | Collegare i percorsi che ancora rileggono il grezzo; non ricostruire un altro decisore. |
| Sessione → frame passati | [discourse.p0](../../kb/core/discourse.p0): `turn_scoped`, `turn_reply`, `turn_input`, `turn_entity`; `session_archive_turn` in [99-registry.c](../../src/brain/99-registry.c). | Il soggetto su cui inferire può già comprendere più turni. Restano cache `last_*`, limiti di enumerazione e cancellazione con `session_window(6)`. |
| Questioni → obblighi | ✅ gen506i: [issues.p0](../../kb/core/issues.p0) e' il tabellone unico, `open_issue(Issue, Kind)` con identita' `Kind_Topic`, tre generi, stato/obbligo/massima come viste; `pending_gap` e `pending_disambiguation` sono viste di transizione. (Prima: `open_issue(Word, Relation)` + due `pending_*` paralleli.) | Restano i lettori C delle viste (C_TODO) e le mosse I2–I4 (§12). |
| Risposte parziali → scelta | [network.p0](../../kb/core/network.p0): `option_hit_word`, `offer_resolution`, `unclaimed_turn_captured`; [session_board.p0t](../../tests/p0t/conversation/session_board.p0t). | L3/L4 di dialogica hanno già un primo consumer. Il conteggio delle parole resta una prova limitata: negazione, esclusione e riferimento vanno letti semanticamente. |
| Lacuna → rete | `network_available` deriva da `policy(network, on)`; esistono `acquisition_policy/1`, `gap_remedy_action`, `action_schema`, provider ed edizioni. Il boot proietta ancora l'ambiente in policy. | L'incremento «stato interrogabile» è già iniziato. Il lavoro è legare il rimedio al bisogno concreto e la decisione al contesto, oltre l'attuale `acquisition_move/1` globale. |
| Rete → lettura → memoria | [50-self-research-loop.c](../../src/brain/50-self-research-loop.c): `network_acquire` chiama `learn_from_prose` e registra `topic_read` con indirizzo e revisione. | Il collegamento fetch/lettore esiste. `topic_definition` resta testo ricavato col taglio C al primo `.`, `!`, `?`; una lettura registrata non certifica la comprensione del passo. |
| Lezione → regola/procedura | [assisted-learning.p0](../../kb/core/assisted-learning.p0), binder in [10-memory-knowledge.c](../../src/brain/10-memory-knowledge.c). | Estendere i canali naturali esistenti. La promozione basata sul conteggio di almeno tre replay va sostituita con verifiche indipendenti del contenuto e delle condizioni. |
| Lezione → revisione | [document-claims.p0](../../kb/core/document-claims.p0), lavori SC40/SC41 documentati in apprendimento assistito. | Riutilizzare le viste versionate; provare la propagazione a risposte, sintesi, piani e persistenza, oltre i tagli già coperti. |
| Capacità → arbitrato | Il registro statico di `99-registry.c` contiene **78 voci**; esiste `module_claim_right`. Acquisizione e sogno hanno anche ingressi esterni al registro. | Il censimento deve includere moduli, rami interni, pre/post-dispatch, comandi e driver. Il nome di un modulo non esaurisce una capacità. |
| Misura → giudizio | [LEDGER](../../tests/comprehension-probe/LEDGER.md) distingue `answered`, muri e problemi di trasporto, e riporta anche risposte errate fra gli `answered`. | Riusare la sonda riparata, leggere i transcript e aggiungere giudizi semantici. Il contatore `answered` non può diventare il punteggio di parità. |

Tre correzioni all'ordine dei piani precedenti:

- I1 di [dialogica](dialogica.md) resta il primo lavoro strutturale; I2/I3
  vanno integrate e generalizzate a partire da ciò che gen506h ha già fatto.
- La comprensione del passo precede la dichiarazione di memoria profonda
  riuscita. L'incremento 8 del piano di rete è una dipendenza del ciclo, anche
  se nella lista storica compare per ultimo.
- La ritenzione dipende dalle questioni e dai sostegni. Aumentare sei turni a
  un numero maggiore rinvia lo stesso difetto; non soddisfa §1bis di dialogica.

## 3. La IR universale: il contratto fra tutte le capacità

### 3.1 Un grafo comune, con viste specializzate

L'IR è la rappresentazione intermedia del contenuto e dell'attività di
parrot0. Si ottiene raccordando gli oggetti esistenti. Turn IR, Document IR,
Task IR e Code IR rimangono viste con le distinzioni necessarie al loro uso;
devono condividere identità, riferimenti, scope, sostegni e bisogni.

| Oggetto | Contratto minimo | Prima sede da riusare |
|---|---|---|
| Osservazione | Identità, sorgente, lingua, posizione, versione, autore e accessibilità dell'originale. | `input.p0`, `input-structure.p0`, `document-claims.p0` |
| Lettura candidata | Atto, ruoli nominati, alternative, evidenze, copertura e residui; versione della conoscenza usata. | `turn-frames.p0`, matcher e scorer condivisi |
| Referente | Identità distinta dalla stringa; menzioni, proprietà, determinazione e contesto. | `input.p0`, `discourse.p0` |
| Proposizione | Relazione e ruoli, polarità, quantificazione, tempo, modalità, scope e attribuzione. | `claims.p0`, `context-scope.p0`, `document-claims.p0` |
| Questione/impresa | Origine, interlocutore, obiettivo, vincoli, sottoquestioni, impegni e criteri di soddisfazione. | `issues.p0`, `discourse.p0`, piani esistenti |
| Sostegno | Premesse congiunte, prove alternative, regole e fonti versionate, dipendenze correnti. | `document-claims.p0`, provenienza e solver |
| Azione | Precondizioni, argomenti tipati, esiti osservabili, effetto atteso, costo, stato e destinatario del risultato. | `procedures.p0`, `network.p0`, piani e strumenti |
| Lezione | Spiegazione naturale, candidata, campo di validità, verifiche, esito, versione e retract. | `assisted-learning.p0` |
| Risposta | Proposizioni, sostegni, richieste coperte, relazioni retoriche, lingua, registro e residui. | `composition.p0`, `responses.p0`, frontier K6 |

Questa tabella descrive requisiti, non nove schemi nuovi da aggiungere
integralmente. Prima di introdurre un campo si identifica il produttore
esistente, chi lo consuma e quale dialogo ne prova la necessità.

**Universale significa estensibile e condiviso.** Un elenco compilato di atti,
ruoli o costruzioni renderebbe la IR un frasario strutturale. Una lezione deve
poter aggiungere una relazione, una costruzione o una combinazione di
condizioni sopra le meccaniche disponibili. L'inglese canonicalizzato resta
una vista linguistica: non sostituisce identità, contenuto e superficie
originale, né deve alterare citazioni o codice.

### 3.2 Il ciclo che ogni incremento deve attraversare

```text
nuovo turno / passo letto / osservazione di un'azione
                       ↓
letture candidate ↔ referenti, questioni, vincoli e memoria pertinente
                       ↓
mossa e sotto-obiettivi derivati dalla KB
                       ↓
prova disponibile oppure bisogno tipato
       ↓                         ↓
piano di risposta        rimedio candidato: chiedere / leggere / apprendere / agire
       ↓                         ↓
realizzazione            osservazione con sostegni → revisione → stesso bisogno
       ↓
impegni assunti, richieste soddisfatte, residui ancora interrogabili
```

Il disegno rappresenta dipendenze, non una sequenza C obbligatoria di sei
stazioni. Una capacità può contribuire a più bisogni; una mossa può sia
rispondere a una domanda sia aprirne un'altra. La KB determina quali passi
servono nello stato corrente, il motore esegue le operazioni generali.

### 3.3 Invarianti da rispettare dal primo incremento

- **Leggere, chiedere e ridire conservano l'oggetto.** La domanda inversa lega
  gli stessi ruoli; la parafrasi conserva scope e quantità. Rileggere una
  propria risposta non crea una seconda fonte indipendente.
- **La conoscenza resta visibile.** Selettività e contesti guidano l'accesso;
  non nascondono fatti o letture concorrenti per far vincere una risposta.
- **Ogni richiesta ha un esito.** Risposta, chiarimento, azione, rifiuto
  motivato o residuo esplicito. La prima clausola riuscita non chiude il turno.
- **L'assenza di prova ha una causa distinta.** Informazione assente,
  negazione provata, conflitto, ambiguità e ricerca incompleta non collassano
  nello stesso esito. Un timeout non dimostra una negazione.
- **La spiegazione segue la prova.** «Ho imparato», «ho letto», «ho finito» e
  «mi manca» devono corrispondere a stati osservabili e correttamente nominati.
- **Una lezione può cambiare la decisione.** Anche priorità, guardie,
  condizioni composte, politica di memoria e forme di risposta sono KB.
  La review di maturità resta verificata sull'implementazione, come nel
  [mantra #21](../../MANTRA.md).

## 4. Dialogica e memoria: decisioni che evitano un altro circuito meccanico

### 4.1 L'identità della questione precede il suo stato

La questione ha un identificatore stabile e un'origine. «Dove si trova Malta?»
e «Quale lingua si parla a Malta?» condividono il tema ma hanno obiettivi
diversi. «Più precisamente» può modificare il criterio di soddisfazione della
prima; una disambiguazione è una sottoquestione che serve a legarne il referente.

La migrazione di `open_issue` deve quindi:

1. censire produttori e consumer di `(Word, Relation)`, incluse le domande
   introspettive già esistenti;
2. introdurre un'identità di questione con kind, origine e goal separati,
   riusando i predicati compatibili;
3. rendere `pending_gap` e `pending_disambiguation` viste di quello stato;
   durante la transizione mantenere **un solo punto autorevole di scrittura**;
4. migrare i consumer, verificare le chat e solo allora ritirare gli archivi
   duplicati. Vietato caricare due significati diversi sotto `open_issue/2`.

Si distinguono **risposta derivabile**, **risposta comunicata** e **richiesta
soddisfatta**. Il fatto trovato può chiudere il bisogno di conoscenza; finché
parrot0 non risponde rimane un obbligo comunicativo. Una risposta può restare
insufficiente rispetto a precisione, formato o seconda richiesta.

Gli stati correnti si derivano da goal, prove, mosse ed eventi osservati.
Sospensione e abbandono sono eventi motivati: una digressione sospende, una
rinuncia esplicita può abbandonare. Uno stato booleano indipendente dalle sue
ragioni ricreerebbe la divergenza fra tabellone e mondo.

### 4.2 Il contesto genera letture, non cattura automaticamente il turno

La max-QUD è un candidato privilegiato dalla pertinenza; la lettura comune
deve considerare anche altre questioni indirizzate esplicitamente. Le mosse
`accept`, `partial`, `repair`, `clarify`, `resume`, `qualify` e `supersede`
si deducono dal rapporto fra lettura e questione.

«Quello per l'automazione» può scegliere un PLC senza ripeterne il titolo;
«non quello per l'automazione» deve escluderlo; «che differenza c'è?» confronta
le opzioni e non ne sceglie una. Una frase può risolvere una scelta e contenere
una nuova richiesta. Contare parole o prendere sempre l'ultima domanda non
esaurisce questa classe.

La politica legacy `offer_unclaimed_turn(accept)` resta registrata come tale.
La sua generalizzazione richiede pertinenza e contesto: il fallimento di una
facoltà, da solo, non è evidenza semantica di assenso. Il criterio è
insegnabile, per esempio «quando rispondo a un'altra domanda, lascia in sospeso
la tua proposta». Il consenso vale per l'azione a cui si riferisce.

### 4.3 Memoria di lavoro, memoria conservata, memoria profonda

La memoria ha tre modalità di accesso allo stesso patrimonio con sostegni:

| Modalità | Contenuto | Criterio di gestione |
|---|---|---|
| Attiva | Letture, questioni, referenti e prove utili al passo corrente. | Pertinenza e bisogni inferiti; costo osservato. |
| Conservata | Conoscenza consolidata, impegni, lezioni, eventi e sintesi strutturate. | Validità e richiamabilità; persistenza attraverso il router previsto. |
| Profonda esterna | Prosa di Wikipedia indirizzabile per lingua, titolo, revisione e sezione. | Bisogno informativo, politica di acquisizione e disponibilità osservata. |

La ritenzione parte da **radici semanticamente vive**: imprese aperte,
impegni, vincoli ancora validi, referenti pertinenti e richieste esplicite di
memoria. Mantiene la chiusura delle dipendenze necessarie a interpretarli.
Non basta «qualcuno cita questo turno»: due turni che si citano reciprocamente
non devono diventare immortali senza una radice viva.

La riduzione della memoria attiva produce sintesi con proposizioni, ruoli,
vincoli e collegamenti ai sostegni. Cambia l'accessibilità, non la verità.
Se il limite di risorse impedisce una rilettura, il bisogno resta nominato e
la parte non verificata non alimenta una risposta certa. La politica che
sceglie cosa attivare, sintetizzare e richiamare appartiene alla KB.

Si rispetta la regola della rete: **nessun archivio di pagine Wikipedia**.
L'originale remoto si recupera dalla revisione indirizzata; finché quella
revisione non è nuovamente disponibile, la rilettura è sospesa. Un aggiornamento
di pagina è una nuova osservazione, non la riscrittura della fonte precedente.
Testi incollati e file locali seguono il loro contratto di conservazione;
non si promette la rilettura di un originale perduto. Il dump runtime non
diventa un archivio ricaricato al boot
([sessione e provenienza](../session-and-provenance.md)).

## 5. Organizzare tutte le abilità senza moltiplicare le ontologie

L'inventario è derivato dal registro reale, dai rami interni e dagli ingressi
esterni. Le famiglie sotto organizzano lavoro e curriculum; non costituiscono
un nuovo dispatch. Uno stesso consumer può servire più famiglie.

| Famiglia | Agganci attuali, esempi | Competenza da addestrare e composizione richiesta |
|---|---|---|
| Comprensione e lingua | `input`, `mention`, `lone`, `spell`, `wordquery`, `translate`, frame | Ruoli, costruzioni, morfologia, rumore, menzione, scope; una lezione cambia lettura, domanda e risposta in IT/EN. |
| Dialogo e pragmatica | `discourse`, `pragma`, `coref`, `repair`, `initiative` | Scelta, chiarimento, correzione, ripresa, implicito; mantiene lo scopo attraverso lettura e azione. |
| Conoscenza e memoria personale | `knowledge`, `world`, `memory`, `personal`, `family`, `answerframe`, `qa` | Fatti contestuali, referenti distinti, domande dirette/inverse e revisione; riuso in spiegazione e pianificazione. |
| Lettura, sintesi e memoria profonda | `reader`, `claimq`, `summary`, `synth`, `learn`, acquisizione | Apposizioni, anafore, argomentazione, selezione per aspetto e fonti; il testo appreso risolve una questione. |
| Ragionamento | `deepreason`, `cause`, `abduce`, `induce`, `compare`, `same`, `analogy`, `counterfactual`, `whatifnot` | Deduzione, causalità, ipotesi, confronto e trasferimento con condizioni; conclusioni nuove sulla KB reale. |
| Quantità, matematica e procedure | `count`, `quantity`, `aggregate`, `sequence`, `operator`, `arith`, `algebra`, `wordproblem` | Ruoli dei numeri, unità, vincoli e procedure; usa dati letti in prosa e spiega il calcolo. |
| Pianificazione e strumenti | `plan`, `toolplan`, `piact`, `agent`, `search`, `tool`, `shell`, `toolpolicy` | Sotto-obiettivi, precondizioni, osservazioni, residui; un nuovo contratto sopra primitive disponibili è insegnabile. |
| Codice e linguaggi formali | `codeast`, `code`, `symbolic`, `rulespec`, Code IR | Comprendere, spiegare, tradurre, verificare e correggere artefatti; stessa impresa e stessi sostegni della prosa. |
| Produzione e relazione sociale | `gen`, `compose`, `reqgen`, `role`, `social`, `chitchat`, `smalltalk` | Registro, spiegazione, sintesi e creatività coerente; segue interlocutore, vincoli e scope della finzione. |
| Apprendimento e metacognizione | `teachconstruction`, `taughtframe`, `teachrule`, `teachreply`, `forget`, `fewshot`, `meta`, `self`, `strategy`, `gapreport`, `verify`, `calibrate`, `robust`, `loop`, `claim` | Lezioni, controesempi, candidati, fiducia calibrata, genealogia; spiegare il cambiamento effettivo e ritrattarlo. |
| Giochi e specializzazioni interne | Rami nei moduli, `archetype`, `namestart`, `conj`, `bench` e driver `dream` | Censimento per operazione effettiva; regole e strategie diventano casi di trasferimento degli oggetti comuni. |

La scheda per ogni capacità contiene:

```text
nome dell'abilità e comportamento osservabile; ingressi e consumer effettivi
oggetti IR letti/prodotti; altri consumer che riusano il risultato
review L1/L2/L3, diritto al turno e debito compilato
lezione naturale disponibile; forma di correzione e retract
fonti, contesti, precondizioni, risultato, lacuna e realizzatore
evidenza: presente / osservata / insegnabile / integrata e persistente
transcript e checkpoint; famiglia di verifica; primo arresto; prossimo incremento
```

Ogni stato porta evidenza, data e versione; `non misurata` è uno stato
ammesso. La copertura si calcola sul totale censito, includendo capacità in
fallback e percorsi fuori registro. Il registro dell'automodello riusa queste
evidenze per rispondere a «che cosa sai fare?». I nomi tecnici rimangono nel
report diagnostico; le lezioni e le risposte ordinarie usano lingua naturale.

## 6. Sequenza esecutiva e condizioni di uscita

Gli identificatori O0–O8 individuano pacchetti di lavoro di questo piano e
rimandano ai V/K/M esistenti. Non sono nuove generazioni né nuove facoltà.
Ogni pacchetto si scompone in sessioni: **un circuito generale per sessione,
poi insegnamento e massimizzazione della classe** (mantra #22).

| Pacchetto | Dipendenza | Lavoro e sedi principali | Uscita osservabile |
|---|---|---|---|
| **O0 — Inventario e baseline** | Nessuna | Schede §5; registro, handoff, sonda, `crossing/AREE.md`. V0. | Ogni percorso ha una collocazione; cinque dialoghi §8 hanno transcript e primo arresto. Stato statico, misura e ipotesi sono separati. |
| **O1 — Una questione, un'origine, una lettura contestuale** | O0 | Migrazione §4.1 in `issues.p0`, `discourse.p0`, `network.p0`, `99-registry.c`; I1–I5 di dialogica, K3, V1/V5. | Scelta parziale, negata e corretta; domanda nuova, digressione e ripresa sullo stesso obiettivo; due richieste sullo stesso topic restano distinguibili. Nessun doppio archivio autorevole. |
| **O2 — Lettura condivisa e completa** | O0; si integra con O1 | `input*`, `turn-frames`, binder, lettore e realizzazione; `C_TODO` sulle finestre. K0–K2/K4, V1–V3. | Apposizione e anafora conservano il referente; domanda, lezione e menzione restano distinte; il vincolo in coda a input lunghi influenza la risposta. La forma insegnata serve prosa e dialogo. |
| **O3 — Ritenzione e richiamo cognitivi** | O1 e identità/sostegni di O2 | Radici e dipendenze §4.3, memoria K7 e ripresa D49; archiviazione del turno e consumer `last_*`. V2/V5/V6. | Questione ripresa dopo 20/50/100 turni, vincoli e antecedenti corretti; compressione, correzione e richiamo verificabili; nessuna scadenza cieca a sei turni. |
| **O4 — Memoria profonda come piano inferito** | Primo taglio O1–O3 | `arrests`, `gap-kinds`, `network`, `initiative`, procedure e adapter di acquisizione. K3/K7/K11, V5. | Bisogno → fonte pertinente → lettura con sostegni → risposta alla stessa questione; ask/act/never, rifiuto, disambiguazione, errore esterno e ripresa sono governati da KB. |
| **O5 — Apprendimento generale e revisione completa** | Porte naturali aperte da O1–O4 | M0–M20, `assisted-learning`, `document-claims`, archi, persistenza e materializzazioni. V3/V4/V6. | Nuova costruzione, regola condizionata, procedura e condotta insegnate; retract/reteach modifica passato e futuro, conserva sostegni indipendenti e regge il processo nuovo. |
| **O6 — Tutte le capacità diventano consumer** | Contratti O1/O2; O5 per la chiusura | Migrazione famiglia per famiglia del §5; K5/K6/K8/K9/K11, V4/V7. | Ogni scheda ha una catena integrata; gli operatori generali trasferiscono a tre domini; output composto da proposizioni e vincoli, con stile insegnabile. |
| **O7 — Curriculum autonomo e sogno** | O4/O5; gate M0–M20 verificato | `dream.c`, arresti, opportunità e medesimo ciclo di acquisizione. V6. | La KB sceglie bisogno e rimedio, verifica il guadagno, evita cicli improduttivi e conserva il residuo. Il sogno migliora anche domande successive non usate per guidarlo. |
| **O8 — Parità empirica e allargamento** | Misure continue da O0; accettazione dopo O6/O7 | Batterie frontier e protocollo §9. V7. | Criteri preregistrati soddisfatti per ogni famiglia, con limiti, costi e controesempi pubblicati. Le nuove famiglie riaprono il perimetro. |

L'ordine esprime dipendenze, non autorizza a sospendere l'insegnamento fino a
O5 o il confronto fino a O8. Ogni incremento deve già essere insegnabile,
corretto e misurato. O5 completa la generalità e la propagazione; quarantena,
validità e assenza di false conferme sono precondizioni dal primo giorno.
O6 procede su ciascun consumer appena il contratto necessario è disponibile.

### 6.1 Primo ciclo concreto: O0 e O1

1. Registrare commit, profilo AGI, KB completa, binario, policy e condizioni
   della misura. Rileggere i risultati correnti delle sonde prima di
   diagnosticare problemi di trasporto già corretti.
2. Riprodurre i dialoghi PLC e Malta del §8; aggiungere una seconda domanda
   sullo stesso topic e una digressione. Annotare cosa manca prima di intervenire.
3. Censire tutti gli accessi ai tre stati di questione; definire ID, origine,
   contenuto e criterio di soddisfazione senza sovraccaricare `open_issue/2`.
4. Migrare il primo tipo di questione e i suoi consumer tramite viste;
   verificare che letture e risultati già validi restino disponibili.
5. Insegnare una condotta pertinente, provarne effetto e retract; estendere
   a scelta, conferma e correzione attraverso la medesima relazione contestuale.
6. Consegnare scheda aggiornata, transcript, diff e prossimo arresto. Se la
   ritenzione blocca la ripresa, O3 è il prossimo circuito; non una nuova
   eccezione per «torniamo a».

**Prodotti del ciclo:** baseline datata nel ledger esistente; inventario delle
capacità e prima mappa produttore/consumer in `docs/reports/`; migrazione
circoscritta; una lezione causale e una famiglia di dialoghi. I file di report
si creano quando contengono evidenza, non come tabelle vuote da dichiarare finite.

### 6.2 Requisiti particolari di O2/O3

La copertura dell'input deve essere semantica: conservare tutto il testo in
memoria non basta se il binder vede soltanto il prefisso. Provare lo stesso
vincolo all'inizio, nel mezzo e in coda, oltre 256 byte, oltre 4 KB e fino al
traguardo di 100 KB dichiarato in dialogica. Rilevare separatamente lettura,
inferenza, realizzazione e memoria utilizzata.

Riutilizzare allocazioni per lunghezza e span sull'originale; inventariare
anche i tetti di enumerazione, non solo i buffer. La grammatica del periodo e
le eccezioni di segmentazione restano insegnabili. Il C offre scansione,
binding e gestione della memoria; la KB stabilisce cosa significhino i confini.

Le rappresentazioni eseguibili rispettano `KB_MAX_ARGS=4` e `KB_MAX_BODY=8`:
oggetti identificati e relazioni normalizzate consentono più proprietà senza
inventare goal a cinque argomenti. La lunghezza del testo è un problema
diverso dall'arità. `naf` richiede variabili legate; i controlli esistenziali
si esprimono attraverso helper legati. Ogni eventuale modifica `.p0` deve
caricare senza `PARSE ERROR`, prima di valutarne la semantica.

### 6.3 Requisiti particolari di O4: la rete deve chiudere un bisogno

La mossa di acquisizione riceve la **questione concreta**: relazione o aspetto
mancante, referente, precisione richiesta, fonte già consultata e lingua.
Il piano inferisce un indirizzo candidato e l'azione che può ottenere la
prova mancante. Una pagina già letta sul topic non dimostra che siano stati
letti anche l'aspetto o la sezione richiesti.

Si riusano `policy(network, ...)`, provider, edizioni e rimedi presenti.
La politica d'uso e la disponibilità osservata sono distinte: autorizzare la
rete non prova che il provider sia raggiungibile. L'adapter restituisce
esiti tipati; successo del trasporto, contenuto disponibile, proposizioni
comprese e questione risolta sono quattro risultati diversi.

Il piano minimo lavora sulla definizione e sulla ripresa; il successivo
seleziona sezioni per aspetto e combina proposizioni con sostegni. Il lettore
pubblica comprensioni e residui nell'IR comune. `topic_definition` diventa una
vista della proposizione definitoria riconosciuta, anziché il prefisso prima
del primo punto. La fonte di ciascuna proposizione porta revisione e posizione.

L'esito riattiva la questione tramite le sue dipendenze. La presenza di un
bisogno ancora aperto guida il passo successivo; il replay del testo
dell'ultima domanda resta un adattatore transitorio. Un'azione ha identità ed
esito per non essere ripetuta a ogni rivalutazione dello stesso piano.
Errori e indisponibilità lasciano un residuo; ordine dei tentativi e criteri
d'arresto sono policy KB. Il provider cognitivo resta Wikipedia secondo il
[piano di rete](la-rete-come-memoria-profonda.md).

### 6.4 Requisiti particolari di O5/O6: apprendere e trasferire

Una nuova generalizzazione viene rappresentata come candidata con condizioni
e alternative. Si verifica con casi indipendenti, controesempi e vincoli;
il numero di replay non può promuoverla da solo. Se due ipotesi restano
compatibili, parrot0 chiede un caso che le distingua.

Retract e correzione invalidano i sostegni congiunti colpiti e propagano
l'effetto a viste, sintesi e piani. Una conclusione sostenuta indipendentemente
rimane utilizzabile. Gli eventi storici restano consultabili; la loro
conclusione precedente non passa per verità corrente. Anche una nuova forma
può far rileggere un testo precedente: servono dipendenze dalle opportunità di
lettura, oltre a quelle dalle sole prove che avevano vinto.

Ogni consumer riceve oggetti IR e restituisce contributi verificabili. Il
realizzatore combina contenuto, relazioni retoriche e registro senza aggiungere
fatti. Una nuova resa si insegna e si ritrae; `{text}` con una frase intera
costruita in C non chiude la migrazione. Le strutture secondarie restano
disponibili secondo la review di maturità, finché la prevalenza del percorso
condiviso non è dimostrata sul comportamento.

### 6.5 Requisiti particolari di O7: il sogno usa il medesimo ciclo

Un bisogno di apprendimento nasce da un arresto, da un residuo di lettura o da
una possibilità di connessione sostenuta. La KB sceglie il prossimo bisogno
in base a utilità per questioni vive, trasferimento atteso, incertezza e costo
osservato. Queste preferenze devono essere insegnabili e verificabili.

La ricorsione «parola per parola» conserva anche sintagmi, concetti, riferimenti
e contesto: un termine ignoto genera un candidato di approfondimento, non un
fetch obbligatorio. Cicli fra topic, fonti già esaurite e assenza di nuovo
supporto danno evidenza di mancato progresso. Budget e arresto rimangono
dichiarati e riprendibili. `dream.c` esegue azioni inferite e pubblica gli esiti;
la politica di frontiera esce dal driver.

L'apprendimento è **continuamente disponibile**, anche prima del teacher
massivo. I giri autonomi lunghi partono solo dopo il gate M0–M20 di
apprendimento assistito §6, con revisione e persistenza verificate. Il gate
non impedisce i piccoli cicli didattici che servono a raggiungerlo.

## 7. Curriculum operativo: costruire una distinzione, poi addestrarla

Le tre attività hanno consegne diverse:

| Attività | Unità di lavoro | Risultato da dichiarare |
|---|---|---|
| Diagnosi | Dialogo fallito e variante discriminante. | Primo arresto causale e capacità condivisa che manca. |
| Sviluppo | Una meccanica generale o un consumer mancante, con conoscenza KB. | Nuova porta naturale, ablazione e riuso. I dati sintetici restano prove di sviluppo. |
| Addestramento | Una lezione vera e persistibile attraverso lingua naturale. | Conoscenza verificata, trasferita, salvata e riletta in processo nuovo secondo [LEARN_PROTOCOL](../../LEARN_PROTOCOL.md). |

Il curriculum segue dipendenze e risultati, con questa prima progressione:

| Livello didattico | Lezione naturale esemplificativa, da adattare a fonti e baseline | Verifica che distingue comprensione da memorizzazione |
|---|---|---|
| Forme e ruoli | «In questa costruzione il luogo viene prima della cosa ospitata.» | Nuove frasi e domande inverse; una diversa disposizione dei ruoli cambia la lettura. |
| Periodo e riferimento | «Il nome ufficiale tra virgole rinomina il paese; la frase continua a parlare dello stesso paese.» | Altre apposizioni, pronomi e sintagmi; due paesi nello stesso passo rimangono distinti. |
| Dialogo | «Se chiedo più precisione, mantieni l'argomento e cerca un'informazione più specifica.» | Luogo, data e quantità; una domanda nuova resta nuova, un approfondimento eredita i vincoli. |
| Regole con condizioni | «In questo contesto, se si verificano entrambe queste condizioni, segue questa conseguenza; fuori dal contesto no.» | Antecedenti e ruoli ricavati dagli esempi; controesempio fuori dominio; una sola premessa non basta. |
| Procedure | Spiegazione reale di un calcolo o di una trasformazione, con unità e condizioni. | Input nuovi, precondizione assente, composizione con dati letti e spiegazione del risultato. |
| Lettura e sintesi | «Per rispondere sui problemi di un processo, seleziona i passaggi che li descrivono e conserva cause e condizioni.» | Altro processo e altra struttura documentale; negazioni, attribuzioni e residui sopravvivono alla sintesi. |
| Condotta e stile | «Quando ti chiedo una spiegazione per un principiante, introduci i termini necessari prima di usarli.» | Argomenti diversi, stesso contenuto fondato; correzione dello stile senza cambiare la tesi. |
| Metaconoscenza | «Spiega quale informazione cambierebbe la scelta fra le due interpretazioni.» | Il chiarimento discriminante modifica davvero la decisione; una risposta irrilevante non la chiude. |

Queste frasi sono esempi di intenzione didattica, **non superfici da inserire a
mano nel frasario**. Si parte da fatti reali e forme già comprese; se manca la
capacità di capire la lezione, si registra il meta-gap e si torna allo sviluppo
del meccanismo generale. Il teacher non espone nomi di predicati, tuple o API.

I due obiettivi prioritari già indicati da F. in apprendimento assistito
diventano curricula trasversali: **grammatica inglese**, usando la stessa
conoscenza per leggere, giudicare, correggere e produrre una frase;
**linguaggio `.p0`**, usando le strutture reali del motore per spiegare una
regola, formularla e verificarne gli effetti. Nel secondo caso `.p0` è
l'oggetto studiato o l'artefatto prodotto: la lezione continua ad arrivare
in lingua naturale. Questi curricula devono migliorare anche la comprensione
e la correggibilità di parrot0, oltre la risposta a esercizi di grammatica.

Ogni lezione applica il ciclo già fissato da apprendimento assistito:
baseline → spiegazione → candidata → replay → Transfer@3 → due parafrasi →
contrasto → composizione → retract/reteach → retention → persistenza.
Una capacità generale richiede `Transfer@3=3/3`; la porta alla retrazione deve
essere naturale. L'assert/retract diagnostico prova una dipendenza dalla KB,
non l'addestrabilità parlata.

Per ciascuna classe si variano ruoli, ordine, lingua, negazione, quantificazione,
scope, lunghezza, riferimenti e presenza di altre richieste. Si prosegue finché
l'aggiunta di esempi smette di migliorare un campione di sviluppo tenuto a parte;
il campione di valutazione finale rimane nascosto. Poi si cerca una
declinazione che riusi la stessa lettura. Se richiede un secondo meccanismo,
diventa il circuito della sessione successiva.

La scelta del lavoro successivo dà precedenza a false affermazioni e false
conferme; poi a oggetti persi fra più consumer; poi a forme e operatori con
trasferimento maggiore. Le competenze sociali, creative e di registro entrano
nel curriculum insieme alle altre: anche un risultato corretto può rispondere
male all'intenzione dell'interlocutore.

## 8. Cinque dialoghi guida, poi famiglie indipendenti

Sono scenari di accettazione da misurare, non trascritti di riuscite attuali.
Le risposte si valutano per contenuto e condotta, senza imporre la frase esatta.
Ogni scenario guida una classe; dopo una modifica diventa regressione e deve
essere affiancato da casi indipendenti.

### D1 — Una scelta dentro una ricerca

```text
Utente: Parlami dei PLC.
parrot0: [se l'ambiguità blocca, espone le interpretazioni sostenute]
Utente: Quello usato nell'automazione industriale.
parrot0: [lega il referente, legge se serve, risponde sul controllore]
Utente: Che cosa intendi per processo industriale?
parrot0: [spiega il termine riferito alla risposta precedente]
Utente: Torniamo ai PLC: spiegamelo per un principiante.
```

Prova O1/O2/O4 e registro. Varianti: risposta parziale, negazione di
un'opzione, scelta ordinale, domanda sulle differenze, correzione e cambio di
tema. Il gate fallisce se nasce una seconda ricerca su una parola isolata o
se una vecchia scelta cattura una richiesta indipendente.

### D2 — La stessa domanda diventa più precisa

```text
Utente: Sai dove si trova Malta?
Utente: Più precisamente.
Utente: Da dove ricavi questa informazione?
Utente: E la sua capitale?
[digressione; estensioni a 20/50/100 turni]
Utente: Riprendi la prima domanda e riassumi quello che abbiamo stabilito.
```

Prova identità della questione, specificità, fonte, anafora e richiamo. La
rete è necessaria soltanto se manca la prova richiesta: non si deve scaricare
per rispettare una sceneggiatura. Nel test del bisogno remoto si sceglie una
lacuna reale del checkpoint; se la KB già sa rispondere, il caso diventa
evidenza di richiamo e si sceglie un altro bisogno per provare l'acquisizione.

### D3 — Leggere per un aspetto e soddisfare più vincoli

```text
Utente: Vorrei un riassunto dei problemi tipici nella produzione della birra,
        con le cause e le possibili conseguenze. Spiegalo in italiano semplice.
Utente: Distingui quello che dice la fonte dalle tue deduzioni.
Utente: Approfondisci il secondo problema e poi aggiorna il riassunto.
```

Prova selezione delle sezioni, proposizioni causali, piano multi-goal,
provenienza, referente ordinale e revisione della sintesi. La fonte deve
contenere davvero il materiale; un titolo o una definizione non bastano.
Variare processo, formulazione, refusi e ordine dei vincoli. Una lettura
parziale rimane esplicita e non viene contata come sintesi completa.

### D4 — Una lezione modifica passato e futuro

Scegliere un passo reale già osservato con una costruzione non compresa.
Insegnarne naturalmente ruoli e condizioni, chiedere una nuova lettura senza
reinserire il passo, usare la costruzione su tre casi reali nuovi e in un'altra
capacità. Ritrarre la lezione, verificare invalidazione e sostegni indipendenti,
insegnarla nuovamente e controllare il risultato dopo salvataggio e riavvio.

Prova O5. Se l'originale remoto deve essere recuperato, si usa la stessa
revisione. La lezione che modifica soltanto i turni futuri lascia aperto il
gate di revisione. La conferma senza acquisizione o con acquisizione falsa
invalida il ciclo.

### D5 — Un'impresa attraversa codice, prosa, strumenti e digressione

```text
Utente: [file Python lungo] Trasformalo in C mantenendo il comportamento.
        Spiega le scelte e verifica il risultato sui casi concordati.
Utente: Prima chiarisci come tratti questo caso limite.
Utente: [corregge un requisito]
[digressione]
Utente: Continua da ciò che manca.
```

Prova lettura completa, Task/Code IR, obblighi, effetti osservati e ripresa.
La verifica riguarda equivalenza sul perimetro di casi dichiarato; non si
afferma una prova generale di equivalenza fra programmi. Il codice generato
e i risultati degli strumenti alimentano lo stesso piano. Un fallimento di
build è un'osservazione; la correzione del requisito invalida solo i passi
dipendenti. Nessuna affermazione di esecuzione senza risultato osservato.

## 9. Misurare il progresso e la parità

### 9.1 Il cruscotto minimo, per famiglia

| Misura | Definizione operativa | Criterio |
|---|---|---|
| Comprensione | Richieste, vincoli, ruoli e scope correttamente rappresentati / quelli richiesti. | Nessuna omissione decisiva nascosta nei casi promossi. |
| Utilità | Dialoghi conclusi con risultato corretto, fondato e pertinente / dialoghi valutabili. | Misura primaria; risposta parziale e chiarimento hanno categorie proprie. |
| Dialogica | Scelte, correzioni, cambi di tema e riprese corretti; chiarimenti necessari/superflui. | Il contesto aiuta senza catturare turni estranei. |
| Fertilità | Lezioni riuscite / tentate; transfer, contrasto, composizione, retract e persistenza. | Conteggi separati per fatti, forme, regole, procedure e condotte. |
| Integrazione | Capacità con consumer comuni e catena completa / capacità censite; domini serviti per operatore. | Nessuna famiglia omessa; almeno tre domini per un operatore generale. |
| Memoria | Bisogni ripresi, vincoli mantenuti e sostegni recuperati dopo distanza, sintesi e riavvio. | Nessuna conclusione stale; nessuna amnesia silenziosa su un impegno vivo. |
| Rete/sogno | Bisogni chiusi grazie a nuova conoscenza / bisogni tentati; utilità su domande successive. | Fetch, pagine e fatti estratti sono costi o volumi, non successi cognitivi. |
| Affidabilità | False affermazioni, false conferme, leakage di scope, falsi ponti e risultati inventati. | Target zero sul campione di accettazione, senza compensazione con altre metriche. |
| Prestazioni | Boot, lettura, inferenza, revisione, realizzazione e IO separati; p50/p95/max, memoria. | Inferenza ordinaria entro 1 s sulla KB completa; costo degli atti lunghi dichiarato. |

Il confronto misura sia comprensione sia utilità: evitare errori rifiutando
sempre o chiedendo sempre chiarimenti non raggiunge il bersaglio. La fedeltà
di un renderer al suo stesso parser non è un oracolo sufficiente: servono
verifiche indipendenti sulle proposizioni decisive.

### 9.2 Protocollo di confronto con il riferimento

Prima della prova si fissano modello/versione del riferimento, configurazione,
contesto, strumenti, fonti, lingua, budget e rubriche. Il modello viene scelto
al momento dell'esperimento; questo piano non presume quale sia oggi il
migliore. Si confrontano sessioni complete sul medesimo compito, con risorse
esterne comparabili e differenze dichiarate.

Servono tre condizioni separate: conoscenza già disponibile; apprendimento
da lezioni date a entrambi; accesso alle fonti consentite. Per la prima si
registra l'asimmetria inevitabile delle conoscenze pregresse. Per la seconda
si distinguono prestazione nel contesto e persistenza dopo riavvio, indicando
quale memoria sia disponibile al riferimento. Il teacher esterno rimane
fuori dall'inferenza di parrot0 durante la valutazione.

Almeno il 70% delle verifiche resta fuori dalle lezioni, come previsto da
apprendimento assistito. Il campione finale comprende nuove costruzioni,
combinazioni, documenti e domini; non soltanto sostituzioni di nomi. La
valutazione include italiano e inglese, dialoghi lunghi, testo/codice misto,
negativi vicini e tutte le famiglie censite. I giudici, quando possibile,
non conoscono quale sistema abbia prodotto la risposta; fatti e calcoli
usano fonti o verificatori indipendenti dal riferimento.

**Proposta iniziale da preregistrare:** margine di non inferiorità di 5 punti
percentuali sul successo del compito per famiglia, con limite inferiore
dell'intervallo di confidenza al 95% della differenza parrot0−riferimento
superiore a −5 punti. Per dichiarare equivalenza statistica, l'intero
intervallo deve rientrare nel margine simmetrico prestabilito; una prestazione
superiore si riporta come tale. I margini di naturalezza e utilità su scale
ordinali si definiscono separatamente. Queste soglie sono una proposta di
lavoro, non risultati né una legge della comprensione.

Si stima prima la numerosità necessaria, usando il dialogo come unità e
tenendo conto dei confronti multipli fra famiglie; i turni dello stesso
dialogo non sono campioni indipendenti. Un campione insufficiente produce
«inconclusivo». La somiglianza testuale e l'accordo con un errore del LLM
non danno credito. Se fallisce una famiglia si conserva il dettaglio e si
riapre il relativo collo cognitivo.

### 9.3 Regime delle verifiche e sostenibilità

Le misure usano sempre la KB reale completa. Un provider locale di sviluppo
rende ripetibile il trasporto; per provare la lettura del mondo reale si
usano estratti autentici con provenienza. Le fixture sintetiche dimostrano
meccaniche e runtime-growth, non conoscenza reale né connecting dots.

Si seguono i regimi già distinti nel repository: apprendimento KB-only con
verifiche dialogiche del LEARN_PROTOCOL, senza suite; sviluppo del motore con
verifiche puntuali pertinenti e il gate `make soft-test` previsto, entro 15 s.
La suite completa non è parte del giro ordinario e segue la politica di
esecuzione esplicita del Makefile. Questa modifica documentale non richiede
una suite del motore.

I test di regressione restano `.p0t`. Un consumer verde con un timeout legacy
più alto non ha superato il contratto di 1 s. Si profila la KB completa,
misurando lookup, derivazioni ripetute e invalidazione delle viste. Indici,
memoizzazione e `materialized_view` accelerano senza cambiare il significato
([mantra #20](../../MANTRA.md)); la selezione della revisione deve mantenere
recall completo rispetto all'audit di riferimento prima di ottimizzare il costo.

## 10. Chiusura di un incremento e manutenzione dei piani

Un incremento è chiuso quando il suo report consente di verificare:

1. quale legge del comportamento mancava e quale oggetto era perso;
2. cosa è stato riusato e quale meccanica generale è stata eventualmente aggiunta;
3. la lezione naturale, i casi indipendenti e la perdita/ripresa dopo retract;
4. almeno un altro consumer che usa la stessa distinzione;
5. sostegni, revisione, persistenza e limiti effettivamente provati;
6. risposta completa o residui espliciti, prestazioni e rossi rimasti;
7. conoscenza rimossa dal C in una migrazione dichiarata KB-first, con bilancio
   del diff e spiegazione delle eventuali nuove primitive generali.

La promozione ha tre livelli: **capacità organica locale**, **integrazione
fra capacità**, **parità sul perimetro valutato**. Il primo richiede la catena
causale; il secondo la copertura delle schede e il trasferimento; il terzo
anche il confronto indipendente. Nessuno implica automaticamente il successivo.

| Documento | Responsabilità che conserva |
|---|---|
| [MANTRA](../../MANTRA.md), [PRINCIPLES](../../PRINCIPLES.md) | Vincoli e tesi dell'esperimento. |
| [Apprendimento assistito](apprendimento-assistito.md) | Missione della KB viva, M0–M20, V0–V7 e contratto epistemico. |
| [Frontier](frontier-kb-natural-dialogue.md) | Scala K, ipotesi e confronto comportamentale. |
| [Comprensione universale](universal-comprehension.md), [input](universal-input.md) | Lettura condivisa, forme, ruoli, referenti e lacune. |
| [Dialogica](dialogica.md) | Questioni, mosse, obblighi e continuità. |
| [Memoria profonda](la-rete-come-memoria-profonda.md) | Acquisizione cognitiva, fonti e sogno. |
| [Armonizzazione](armonizzazione-piani.md) | Diagnosi delle divergenze e loro storia. |
| **Questo piano** | Dipendenze esecutive, inventario, curriculum e prove d'integrazione. |
| [Procedura di crescita](procedura-crescita-kb.md), [LEARN_PROTOCOL](../../LEARN_PROTOCOL.md) | Metodo delle sessioni di sviluppo e insegnamento. |
| [LEARN_TODO](../../LEARN_TODO.md), [C_TODO](../../C_TODO.md), ledger e report | Stato corrente, misure e prossimo arresto. |

Le chiusure future aggiornano il documento proprietario e il report con
checkpoint e prova; negli altri si aggiunge un rimando. Le evidenze storiche
restano datate. Il prossimo lavoro è **O0 → O1: misurare la continuità e dare
alle questioni un'identità comune**, mantenendo leggibili tutti i risultati
già disponibili. Da quel punto la memoria profonda può servire un bisogno
che la conversazione conserva, verifica e sa riprendere.

## 11. Il primo ciclo, eseguito — O1 al gen506i (9 settembre 2026)

E' il report che il §10 chiede, per il primo incremento. Misure sul binario
di `gen506i`, KB completa, profilo `agi`, provider locale
`tests/fixtures/wiki` dove serve una lettura.

### 11.1 Il censimento dei tre stati di questione (§6.1, passo 3), misurato

| stato (prima) | scrittori C | lettori C | lettori KB |
|---|---|---|---|
| `open_issue(Word, Rel)` (gen394) | nessuno: `turn_bookkeeping(issue)` in KB | nessuno | `issue_status`, `answer_obligation`, `recall_kind_word` |
| `pending_gap/1` + `pending_gap_question/1` (+ `pending_gap_failed/1`) | 3 siti (`mod_learn` in 50-self-research-loop.c; il declino informato e l'offerta della definizione in 99-registry.c) + la fotografia di `compound_turn_lead` | il pre-dispatch dell'offerta, `pending_offer_fallthrough`, due guardie `already_gap`, `dream.c`, `main.c` | `offer_resolution` (network.p0 §10), `debug_probe(40)` |
| `pending_disambiguation/1` + `disambiguation_option/3` | 1 sito (`network_acquire`, 50-self-research-loop.c) | il pre-dispatch della scelta (con il restringimento), il fallthrough, `disambiguation_render`, i due lookup dell'opzione scelta | `option_hit_word` (network.p0 §11) |

Quattro scrittori e una fotografia per due stati che avrebbero dovuto essere
uno; tre chiusure globali (`kb_retract_pred`) che spegnevano OGNI offerta per
aprirne una; «la pending» presa come prima riga asserita.

### 11.2 Che cosa e' cambiato

- **`kb/core/issues.p0` riscritto come tabellone:** `open_issue(Issue,
  Kind)`, `issue_topic`, `issue_turn` (numero), `issue_question`,
  `issue_option`, `issue_relation`; identita' `Kind_Topic` (`issue_id/3` in
  KB, `board_issue_id` in C: lo stesso nome); tre generi; `issue_state`,
  `issue_owed_by`, `issue_open`, `max_qud/1`, `max_qud_of_kind/2`,
  `max_qud_topic/2`; le viste per tema che i frame di risposta e la ripresa
  gia' consumavano (`issue_status`, `answer_obligation`, `issue_answer`); le
  quattro viste di transizione. Tutto `machinery` e `turn_scratch`.
- **C:** `board_open`, `board_option`, `board_close`, `board_close_kind`
  (50-self-research-loop.c) — apre e chiude per identita', origine
  `KB_REFLECTIVE`. Dieci siti di scrittura ridotti a una chiamata; i lettori
  del «pending» corrente (pre-dispatch dell'offerta e della scelta,
  fallthrough) leggono `max_qud_topic(Kind, T)` e la `issue_question` di
  quella questione, non la prima riga.
- **Un orologio solo:** il motore pubblica `turn_counter(N)` all'ingresso
  del turno; `bookkeeper(clock)` e i fatti di boot sono tolti da
  discourse.p0.
- **Guardia per tema** anche in `mod_learn`, come gen384 aveva fatto nel
  sito gemello.
- **Cricchetto:** `tests/p0t/conversation/dialogue_board.p0t` (55 assert):
  offerta, bivio e domanda dell'utente come tre generi dello stesso
  tabellone; due generi insieme con la massima piu' recente e la vecchia che
  resta; «si'» all'offerta piu' recente; l'orologio giusto dal turno 1;
  l'ablazione della questione che spegne la vista e la scelta.

### 11.3 Due distinzioni trovate, non previste

Nella forma di `procedura-crescita-kb.md` §1 — *un predicato che rispondeva
a due domande insieme*:

| predicato | le due domande fuse | la separazione |
|---|---|---|
| `turn_counter` | «che turno e'?» e «quando scatta l'orologio?» — un contabile KB che scattava a meta' turno, accanto al contatore C che nomina gli scope | il motore osserva e pubblica il numero; la KB lo legge |
| `pending_gap` | «c'e' un'offerta aperta?» e «qual e' l'offerta che questo turno indirizza?» — la prima riga asserita valeva per entrambe | `issue_open` per la prima, `max_qud_of_kind` per la seconda |

### 11.4 Bilancio e prove

| | righe (al netto dei commenti) |
|---|---|
| C | +100 / −66 = **netto +34** (i dieci siti −66; i `board_*` e l'orologio +100) |
| KB | +46 / −12 = netto +34 |

Il C non si e' accorciato in totale (mantra #18a): la meccanica di aprire e
chiudere per identita' e' entrata una volta, e i siti si sono accorciati.
Cio' che e' uscito dal C come **conoscenza**: quali stati del dialogo esistono
e come si chiamano; «la pending» come *prima asserita* → la *massima* per
regola; l'orologio doppio. Prove: dialogue_board 55/55; invariati
offer_context 21, disambiguation 19, session_board 24, open_issues 16,
deep_memory 44, deep_memory.it 15, compound_inquiry 31, gap_kinds 9, savemap
10, move_precedence 9, self_compensation 9; `make soft-test` 7 s.
discourse_recall (2 rossi: smalltalk ruba «what did you tell me about
milan») e issue1 (3-4 rossi: divisione per zero in italiano, «verified
schema») erano rossi anche sul binario di HEAD, verificato in un worktree con
un secondo socket.

### 11.5 Residui, misurati

1. Un dichiarativo che nomina il tema dell'offerta e' letto come assenso
   («zorbia is in europe» sotto «vuoi che cerchi zorbia?» → una lettura
   parte): `input_node_atom` in `offer_resolution` non guarda la forza del
   turno. Primo caso del circuito 3 (§12.2).
2. `session_window(6)` e' ancora un numero (§12.1).
3. Fuori dal tabellone: `option_word`, `pending_gap_failed`, la fotografia
   del `compound_turn_lead`, i lettori delle viste, `b->last_*` (C_TODO).

## 12. Runbook dei prossimi circuiti — uno per sessione, poi si massimizza

Ogni circuito: baseline (`dialogue_board.p0t` + banco piccolo), costruzione,
cricchetto con ablazione, massimizzazione per voce, una declinazione, handoff
(mantra #22, `procedura-crescita-kb.md`). Le forme sotto sono la proposta
esecutiva: si cambiano se il primo dialogo le smentisce, non prima.

### 12.1 Circuito 2 — O3: la ritenzione e' una regola sul contenuto

> ✅ **Fatto al gen506j** nella forma sotto, con una aggiunta: il citatore
> generale `retention_cite(Pred, Pos)` — un predicato che porta il numero del
> turno in un argomento tiene il turno finche' esiste — cosi' una ragione
> nuova e' un fatto, non una regola. Cricchetto
> `tests/p0t/conversation/retention.p0t` (26/26) con le tre ablazioni.
> L'orologio si pubblica PRIMA dell'archivio. Residuo: il referente vivo.

**Legge violata:** dialogica §1bis — un contatore decide per numero di turni
cio' che dipende dal contenuto. **Oggetto perso:** «perche' questo turno e'
ancora in memoria».

```prolog
% discourse.p0 §6 — il motore pubblica che cosa ha archiviato; la KB dice che cosa tenere
machinery(turn_archived).                  % turn_archived(turn_N, N): scritto dal motore quando archivia
retention_reason(open_issue).              % ha aperto una questione ancora sul tabellone
retention_reason(last_move).               % l'ultima mossa e' sempre viva
retention_reason(recency).                 % la recenza: un costo dichiarato, non la regola
turn_retained($T) :- retention_reason(open_issue), turn_archived($T, $K), open_issue($I, $Kind), issue_turn($I, $K).
turn_retained($T) :- retention_reason(last_move), previous_turn($T).
turn_retained($T) :- retention_reason(recency), turn_archived($T, $K), turn_counter($N), session_window($W), is($D, sub($N, $K)), le($D, $W).
turn_expired($T)  :- turn_archived($T, $K), naf(turn_retained($T)).
```

**C (`session_archive_turn`):** dopo aver spostato `current_turn` sotto
`turn_N`, asserisce `turn_archived(turn_N, N)`; poi enumera `turn_expired`
(`kb_match_all`) e ritira ogni scope trovato — gli stessi `turn_scoped/2` di
oggi — e la sua riga `turn_archived`. Il calcolo `done − window` sparisce.
`session_window(6)` resta come UNA ragione, dichiarata: `!forget
retention_reason(recency)` lascia la sola ritenzione per contenuto, ed e'
l'ablazione. **Prove:** session_board «la finestra» invariato (a…h non sono
citati da nulla); nuovo blocco: un bivio aperto al turno 2, dieci turni di
aritmetica, `turn_input(turn_2, …)` ancora presente e «il primo» ancora una
scelta; con `!forget open_issue(choice_plc, choice)` il turno 2 cade al giro
dopo. **Costo:** una query per turno vivo per turno; si misura con il banco
piccolo prima di ottimizzare (mantra #20b). **Declinazione:** un referente
vivo cita il turno che l'ha introdotto — `retention_reason(referent)` con
`turn_entity(turn_N, E)` quando la coreferenza lo legge.

### 12.2 Circuito 3 — I2/I4: `move_addresses/3`, un nome per la lettura del turno sulla questione

**Legge:** L2. **Oggetto perso:** la mossa come lettura del frame relativa
alla questione — oggi tre letture con tre nomi (`offer_resolution`, l'ordinale
in C, `option_hit`).

```prolog
move_addresses($T, $I, refuse)  :- open_issue($I, gap_offer), turn_cue($T, dissent_word, $W).
move_addresses($T, $I, accept)  :- open_issue($I, gap_offer), turn_cue($T, assent_word, $W), naf(move_addresses($T, $I, refuse)).
move_addresses($T, $I, accept)  :- open_issue($I, gap_offer), issue_topic($I, $Topic), input_node_atom($T, $Id, $Topic),
                                   naf(turn_illocution($T, assertion)), naf(move_addresses($T, $I, refuse)).
move_addresses($T, $I, answer)  :- open_issue($I, choice), issue_topic($I, $Topic), option_chosen($Topic, $N).      % una colpita, o l'ordinale
move_addresses($T, $I, partial) :- open_issue($I, choice), issue_topic($I, $Topic), option_hit($Topic, $N), naf(option_chosen($Topic, $M)).
move_addresses($T, $I, supersede) :- turn_illocution($T, question), naf(move_addresses($T, $I, accept)), naf(move_addresses($T, $I, answer)).
```

**C:** il pre-dispatch chiede `max_qud_of_kind(K, I)` e poi
`move_addresses(current_turn, I, How)`, ed esegue per `How`: accept →
acquisisci `issue_question`; refuse → chiudi e prendi atto; answer → chiudi
e leggi l'opzione; partial → ritira le `issue_option` non colpite e richiedi;
supersede/unrelated → non toccare. Le tre letture di oggi diventano clausole
di questa regola; l'ordinale (`ordinal_choice`) diventa una cue del frame.
Chiude il residuo del dichiarativo (§11.5.1). **Prove:** offer_context,
disambiguation, session_board, dialogue_board invariati + «zorbia is in
europe» sotto l'offerta che NON legge e impara.

### 12.3 Circuito 4 — I3/D49: superare e riprendere

`issue_raised(I, N)` ogni volta che la questione e' posta o ripresa (la
prima coincide con `issue_turn`); `max_qud` sull'ultima `issue_raised`.
«torniamo a X» / «continua» (`continue-as-resumption.md`) → `resume`: la
questione con `issue_topic(I, X)` riceve `issue_raised(I, N)` corrente e
torna massima. `issue_expiry(Kind, N)` come fatto; una questione scaduta e'
`issue_state(I, superseded)`, non ritirata — cosi' «cosa e' rimasto in
sospeso» la nomina ancora. **Prova:** il reperto 2 di dialogica §0 per
intero, «torniamo ai plc» compreso.

### 12.4 Circuito 5 — O4: la lettura chiude la questione

Dopo `topic_read`, il C non ridispatcha un testo salvato: chiede
`resume(I)` = riprova la `issue_question` come turno sotto la conoscenza
nuova (e' gia' cio' che `acquire_and_report` fa, senza il nome), e la
risposta cita `topic_read` (la fonte). Sono G1 e G2 del piano di rete; il
«piu' precisamente» (`precision_request_cue`) diventa `qualify` sulla stessa
questione risolta.

### 12.5 Poi si massimizza, parlando

Ogni circuito lascia una classe da riempire per voce: le parole di assenso
e dissenso, le forme della ripresa, le ragioni di ritenzione, i generi di
questione. E' il lavoro del `LEARN_PROTOCOL.md`, e vale dal turno dopo.
