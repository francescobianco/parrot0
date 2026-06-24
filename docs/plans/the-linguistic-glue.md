
**Colla linguistica** è il nome che diamo a quell'insieme di meccanismi che non costituiscono direttamente il ragionamento, la memoria o l'inferenza, ma che permettono a tutte queste componenti di apparire come un tutto coerente.

La colla linguistica è ciò che mantiene la continuità semantica tra le parti di una conversazione. È il tessuto connettivo che collega concetti, intenzioni, riferimenti impliciti, correzioni, ambiguità e cambi di prospettiva, evitando che l'interazione si presenti come una successione di blocchi isolati.

Quando un LLM comprende che una frase è una precisazione di qualcosa detto dieci messaggi prima, quando riconosce che una critica modifica il significato di una risposta precedente, quando ricostruisce il senso di una richiesta formulata in modo incompleto, sta operando attraverso questa colla linguistica.

La colla linguistica non coincide con il ragionamento.

Il ragionamento produce inferenze.

La memoria conserva informazioni.

La pianificazione organizza azioni.

La colla linguistica, invece, rende possibile il passaggio fluido tra questi elementi, mantenendo la continuità dell'intero sistema conversazionale.

Si potrebbe dire che:

* il ragionamento costruisce ponti logici;
* la memoria conserva il territorio;
* la pianificazione sceglie la direzione;
* la colla linguistica mantiene unite le sponde.

Senza di essa, ogni risposta apparirebbe corretta localmente ma scollegata dal resto della conversazione.

---

Una definizione più astratta potrebbe essere:

> La colla linguistica è il campo di coerenza che emerge dall'uso del linguaggio e che permette a strutture cognitive differenti — inferenza, memoria, pianificazione, interpretazione e correzione — di operare come se appartenessero a un unico processo continuo.

La caratteristica fondamentale della colla linguistica è che diventa visibile soprattutto quando manca.

Quando la colla è presente, la conversazione scorre naturalmente e la sua esistenza passa inosservata.

Quando la colla è assente, emergono sintomi caratteristici:

* risposte corrette ma fuori contesto;
* perdita dei riferimenti impliciti;
* interpretazioni eccessivamente letterali;
* incapacità di integrare correzioni successive;
* sensazione di dialogare con più sistemi indipendenti anziché con un unico interlocutore.

In questa visione, il ragionamento non è ciò che tiene insieme la conversazione.

Il ragionamento è uno degli oggetti che la colla linguistica tiene insieme.

La colla linguistica è quindi una metastruttura: non produce necessariamente nuove conclusioni, ma permette alle conclusioni, alle memorie e alle intenzioni di appartenere alla stessa storia.

---

## Critica e piano d'azione (Claude, 2026-06-24)

> Letto accanto a [PRINCIPLES.md](../../PRINCIPLES.md) (emergenza-su-design, struttura
> funzionale all'emergenza, anti-impostore, KB-first) e al metodo discovery-harness di
> [CODE-MASTERY.md](../CODE-MASTERY.md) §7-8 (non si hardcoda la roadmap: ogni faculty si
> *tira* da un fallimento osservato).

### È un passo avanti? Sì — come LENTE e come MISURA, non come nuovo primitivo.

Il documento è una **fenomenologia**, non un meccanismo: dice *cosa* fa la colla, non
*come* costruirla in modo deterministico. Il suo valore per parrot0 è duplice e reale:

1. **Diagnosi centrata.** I cinque sintomi dell'assenza di colla — risposta corretta ma
   fuori contesto, riferimenti impliciti persi, letteralità eccessiva, correzioni non
   integrate, "più sistemi indipendenti invece di un interlocutore" — sono *esattamente*
   il modo di fallire di un registry first-match-wins di ~50 moduli. parrot0 è per
   costruzione a rischio di essere "una successione di blocchi isolati"; l'incubo
   dell'essay è il suo default.
2. **Riconcettualizzazione utile.** "Il ragionamento è uno degli oggetti che la colla
   tiene insieme" e "la colla è visibile soprattutto quando manca" sono affermazioni
   *operative*: si trovano i buchi dai sintomi. Questo è già il metodo del progetto.

### Il rischio da non ignorare (anti-impostore)

"Campo di coerenza che **emerge** dall'uso del linguaggio" è vicino alla *zuppa
metabolica* che PRINCIPLES rifiuta. Preso alla lettera autorizzerebbe una coerenza
finta-fluente: il "perfetto impostore". Quindi il vincolo non negoziabile:

> La colla NON è un modulo magico. È un insieme di **operazioni deterministiche e
> ispezionabili su stato di sessione reale** — risolvere *questo* pronome a
> *quell'*antecedente, integrare *questa* correzione ri-derivando *quel* goal — non un
> generatore di continuità plausibile. La struttura è la *condizione* della coerenza, non
> il suo sostituto (PRINCIPLES §"struttura funzionale all'emergenza").

### Verità di base: la colla esiste già, sparsa e senza nome

Non è capacità nuova; è un **nome + criterio** per faculty che parrot0 ha già costruito a
pezzi (da verificare nel codice prima di toccarle — vedi nota gotcha):

- `mod_coref` — coreferenza (pronomi → entità recenti); stato: `entities`, `last_entity`.
- `mod_discourse` — memoria di discorso e topic (`topics`, `current_topic`).
- `mod_pragma` + `pragma_peel` (gen142) — peeling di aperture/marker e ri-dispatch del residuo.
- repair-loop (gen141) — `pending_repair`/`pending_slot`: chiarisce e RIPRENDE l'intento.
- auto-correzione (gen103) — ri-deriva `last_goal` quando la KB cambia ("allora X non è più…").
- counterfactual/trace (gen105/128) — `last_input`, `trace_*`: "perché hai risposto così".

La colla, per parrot0, **è già questa famiglia**. Manca: un nome di prima classe, una
misura, e il colmare i buchi in modo tirato-dai-fallimenti.

### Piano d'azione (piccolo, reversibile, oracle-gated)

**G0 — Reificare la colla nell'automodello.** Aggiungere un fatto KB
`glue_faculty(coref|discourse|pragma|repair|correction, "ruolo")` derivato dal registry
reale (come già si fa con `module/1`), così "quali parti ti tengono coerente?" risponde
dalla struttura, non da una storia. Zero rischio, alto valore di chiarezza. *Pull:* nessuno
ancora — è il preludio che rende misurabile il resto.

**G1 — `make glue-bench`: i 5 sintomi diventano metriche.** Uno strumento di *scoperta*
(modellato su swe-bench/superglue: non bara mai) con dialoghi multi-turno held-out, una
categoria per sintomo:
   1. *fuori-contesto* — una risposta corretta che ignora il vincolo posto 3 turni prima;
   2. *riferimento implicito* — "e il suo colore?" senza ripetere il soggetto;
   3. *letteralità* — una precisazione che modifica una richiesta precedente;
   4. *correzione* — "no, intendevo Y" che deve ri-derivare la conclusione;
   5. *un solo interlocutore* — catena che attraversa coref+memoria+aritmetica.
   Ogni caso è EN+IT (ratchet bilingue). Il bench *riporta dove la colla manca* — la
   mappa-dei-fallimenti che tira G2. Non fallisce la build (degrade mode), come swe-bench.

**G2 — tirare UN meccanismo connettivo per generazione, dal primo fallimento di G1.**
Esempi plausibili (ma li sceglie il bench, non noi a priori): estendere `mod_coref` a un
antecedente a due turni; far sopravvivere `user_constraint` al di là del turno in cui è
posto e *applicarlo* alla risposta; integrare una correzione "no, intendevo Y" come
ri-asserzione + ri-derivazione del `last_goal`. Ognuno: minimo, deterministico, EN+IT,
verificato, con `make test` verde e gated da stato reale ispezionabile.

**G3 — chiudere il cerchio con la misura.** Il punteggio di `glue-bench` sale solo quando
un meccanismo reale colma un sintomo reale: guadagnato, non simulato. Il numero non è
"naturalezza" a sensazione ma "quanti casi-sintomo held-out ora reggono".

### Disciplina (vale per ogni passo)

- Niente "modulo colla". La coerenza è operazioni su stato di sessione reale, queryable.
- Ogni faculty si tira da un caso `glue-bench` che fallisce (non si pre-progetta).
- KB-first: vincoli/correzioni/riferimenti vivono come stato ispezionabile (entità,
  topic, goal, world), non in una scatola nera.
- Bilingue EN+IT su ogni caso; `make test` resta ermetico.
- Gotcha: prima di estendere `mod_coref`/`mod_discourse`/`mod_pragma`, **verificare nel
  codice** cosa fanno davvero oggi (i moduli vedono la superficie canonicalizzata; vedi
  le note in PRINCIPLES e nei gotcha di brain.c) — non fidarsi di questa lista a memoria.

### Stato — G0 + G1 fatti (gen215)

- **G0 ✅** `kb/core/glue.p0` porta `glue_role/2` (coref, discourse, pragma, repair,
  counterfactual); `brain_create` reifica `glue_faculty(Module, Role)` SOLO per i moduli
  che esistono davvero nel registry (drift-safe, come `module/1`). Escluso dai conteggi
  fatti. La famiglia-colla è ora ispezionabile nella KB, derivata dalla struttura reale.
- **G1 ✅** `make glue-bench` (`tests/gluebench.sh`): dialoghi multi-turno held-out EN+IT,
  i 5 sintomi come metriche, degrade mode (non fa fallire la build, non è in `make test`).
- **Mappa-dei-fallimenti (verità di base, misurata — non assunta):**
  ```
  implicit-reference   0/3 carried   ("what is it part of" / "his name" / IT non risolvono)
  correction           0/2 carried   (dopo "no, X non è Y" la conclusione resta "Yes")
  out-of-context       qualitativo   ("keep it short" non accorcia la risposta dopo)
  over-literal         qualitativo   (una precisazione "and times 3" non continua l'op.)
  one-interlocutor     qualitativo   (catena memoria→aritmetica non regge)
  ```
  **0/5 crisp carried.** Esattamente la tesi dell'essay resa visibile: la colla manca, e
  ora sappiamo *dove*, con un caso ripetibile per ognuno.

### G2 — primo pull fatto (gen216): la coreferenza porta il riferimento

`coref_resolve` (99-registry.c): se la query non viene capita così com'è, un pronome
entità ("it"/"its") viene risolto all'entità recente (`last_entity`) e il turno riscritto
viene ri-dispatchato. Glue come **operazione deterministica su stato reale**, non un campo
emergente (PRINCIPLES anti-impostore). Conservativo: gira solo come *fallback* e reclama
solo se un modulo risponde davvero al turno risolto — non dirotta mai un turno che già
funziona. Ratchet ermetico in `make test`: `tests/cases/coref_resolve.chat`.

Effetto su `glue-bench`: `implicit-reference` da **0/3 → 1/3** (`ref-heart-en` ora HELD).

**Restano GAP (prossimi pull, dalla mappa):**
- `ref-dog-en/it` — bloccati a monte: "what is my dog called" non recupera nemmeno senza
  pronome (asimmetria store/recall del possesso "my dog is Rex"). Pull: sistemare il
  recall del possesso, poi la coref ci si appoggia.
- IT *pro-drop* — l'italiano omette il soggetto ("come si chiama", "è un cane?"): non c'è
  token "it" da risolvere. Meccanismo distinto (anafora a soggetto nullo), pull dedicato.
- `correction` 0/2 — "no, X non è Y" non viene nemmeno parsato come negazione; poi va
  ri-derivato il goal (gen103 esiste per la ri-derivazione, manca il parse della correzione).

Un meccanismo per generazione; la mappa detta l'ordine, non noi.

---

## Punto di ripresa (resume) — prossimi passi ordinati

> **Stato a gen216.** G0 (reify), G1 (`make glue-bench`), G2 (coref EN) fatti, committati e
> pushati su `main`. `make test` 203/0, benches verdi. Per rivedere la mappa in qualunque
> momento: **`make glue-bench`** (degrade mode, non rompe la build).
>
> Disciplina invariata: UN meccanismo per generazione, tirato dal primo caso che fallisce;
> deterministico e ispezionabile su stato di sessione reale (mai coerenza finta); EN+IT;
> ogni gap chiuso passa da GAP→HELD in `glue-bench` E guadagna un ratchet `.chat` in
> `make test`. **Prima di toccare un modulo, leggerlo nel codice** (i moduli vedono la
> superficie canonicalizzata — vedi gotcha in PRINCIPLES e in brain.c).

Ordine consigliato dei prossimi pull (dalla mappa, dal più sbloccante):

1. **Recall del possesso (sblocca `ref-dog-en`).** Bug a monte, indipendente dalla glue:
   `my dog is Rex` salva il possesso ma `what is my dog called` risponde "I don't know…".
   Allineare store/recall in `mod_memory` (10-memory-knowledge.c, intorno a `call_me`/
   `mi_chiamo`/`possessions[]` e `find_possession_name`). Poi `coref_resolve` per i
   possessivi ("his/her") ci si appoggia. Caso bench: `ref-dog-en`.

2. **`correction` 0/2.** Due sotto-passi: (a) parsare "no, X non è Y" / "no, X non è un Y"
   come negazione → `kb_assert_neg`/override (oggi cade in not-understood); (b) ri-derivare:
   gen103 (`last_goal`) già annuncia il cambio quando la KB cambia — verificare che scatti
   dopo la correzione. Casi bench: `corr-en`, `corr-it`.

3. **IT pro-drop (sblocca `ref-dog-it`, completa la coref bilingue).** L'italiano omette il
   soggetto ("come si chiama", "è un cane?"): nessun token "it". Serve un meccanismo di
   anafora a soggetto nullo (se il turno è una domanda senza soggetto esplicito e c'è
   `last_entity`, provare a iniettare l'entità e ri-dispatchare). Estende `coref_resolve`.

4. **Sintomi qualitativi → metriche crisp.** Rendere verificabili `out-of-context`
   (applicare davvero `user_constraint` "keep it short" alla risposta dopo),
   `over-literal` (una precisazione "and times 3" continua l'operazione precedente),
   `one-interlocutor` (catena memoria→aritmetica). Per ognuno: prima un predicato crisp in
   `gluebench.sh`, poi il meccanismo.

Quando un pull chiude un gap: aggiornare qui lo stato e la riga corrispondente della
mappa, bump versione, commit+push.
