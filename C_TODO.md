# C_TODO — che cosa deve ancora uscire dal C

> ## ⛔ HANDOFF — 2026-08-28, `gen459`. Leggere prima di riprendere.
>
> ### U1. Il troncamento silenzioso è una CLASSE, non un incidente
>
> Al `gen459` un difetto è costato dieci turni di congetture. La causa:
>
> ```c
> char pats[64][KB_TERM_LEN];
> size_t np = kb_match(b->kb, "extract_frame", anyq, 2, pats, 64);
> ```
>
> `extract_frame/2` non è un elenco: è fatti **più regole** che generano uno
> schema per ogni verbo di relazione insegnato. Misurati con `/debug
> extract_frame`: **124 schemi contro un tetto di 64.** Sessanta invisibili, e
> *quali* dipendeva dall'ordine di enumerazione — per questo, aggiungendo una
> regola, funzionava una frase **oppure** l'altra ma mai entrambe.
>
> **Non è un limite di memoria: è un limite a quanto parrot0 può imparare prima
> di cominciare a dimenticare senza dirlo.** Ed è la terza volta che il progetto
> lo incontra (gen376 `declined[64]`, gen382e il tetto di 128 predicati, ora
> questo). Censimento al `gen459`:
>
> | | siti |
> |---|---:|
> | `kb_match` con tetto **fisso** | **514** |
> | di cui tetto 64 / 32 / 128 (liste vere) | 34 / 28 / 7 |
> | `kb_match_all` (dimensionato sui dati) | **20** |
>
> I tetti a `1` (290 siti) sono legittimi — si chiede un binding solo. Gli altri
> sono tutti candidati allo stesso guasto.
>
> ### U2. La guardia proposta da F., ed è la direzione giusta
>
> F.: *«non si possono mettere delle guardie su questi valori che mettono subito
> in allerta durante l'inferenza, pilotando l'inferenza nel riconoscere che essa
> stessa è in uno stato di risposta corrotta, come per le deduzioni di
> incoerenza che già abbiamo implementato?»*
>
> Sì, ed è più della correzione puntuale che ho fatto. La forma giusta non è
> alzare i tetti — è **rendere la saturazione un fatto su cui l'inferenza può
> ragionare**, esattamente come `machinery_gap` rende il muro un fatto e come
> `kb_is_conflicted` rende l'incoerenza una risposta invece di un silenzio.
>
> Disegno da valutare:
>
> - quando un `kb_match` rende esattamente `max` binding, asserire
>   `saturated_read(Pred, Arity, Cap)` in KB_REFLECTIVE;
> - una risposta prodotta in un turno che ha un `saturated_read` **non è
>   attendibile**: è la stessa figura di `undetermined_cycle` — non «No.», ma
>   «non l'ho chiuso»;
> - la soglia e il comportamento sono fatti, non costanti: chi vuole il taglio
>   muto lo può avere, ma deve dirlo.
>
> Il motore oggi **non può accorgersi** di aver risposto su una vista amputata.
> Finché è così, ogni risposta che dipende da un elenco è vera *forse*, e nessuna
> misura lo distingue.
>
> ### U3. `/debug` è cresciuto, e la regola di crescita è scritta
>
> `/debug <predicato>` ora dice, per arità: fatti ground, regole per quella
> testa, binding effettivamente resi — ed è così che U1 è diventato visibile in
> un comando invece che in dieci esperimenti a mano. Dice anche `SATURO` quando
> è la *sua* ispezione a toccare il tetto, perché un debugger che tronca in
> silenzio riprodurrebbe il difetto che esiste per trovare.
>
> **La regola, da rispettare:** se per capire qualcosa si è dovuto fare un
> esperimento a mano, quell'esperimento appartiene a `/debug`.
>
> ### U4. Dove siamo con l'addestrabilità via prompt
>
> Vedi `docs/plans/apprendimento-assistito.md` §6.2b, aggiornato al `gen456` e
> ancora valido. In breve, chiuso in questa sessione: la forma della domanda
> (M15, `gen457` — una `question_shape` con slot copre tutte e 136 le relazioni),
> la concessiva (`gen455`), la relativa (`gen458`), il muro lungo scambiato per
> racconto (`gen454`), la lezione muta (`gen456`), la copula mangiata dal
> soggetto (`gen459`).
>
> **Resta aperto e va attaccato per primo:** la canonicalizzazione italiana non è
> stabile — «quali colori si usano negli scacchi» diventa *"which color is usano
> in the chess"*, un ibrido, e la stessa frase si canonicalizza diversamente a
> seconda di dove sta. Finché è così le `question_shape` italiane non possono
> agganciare, e M15 vale solo in inglese.
>
> ### U5. Metodo di verifica — correzione di un mio errore
>
> Non eseguire i `.p0t` in un ciclo sequenziale contro un solo demone: è lento
> (5-15 minuti) **e produce fallimenti falsi**, perché lo stato passa da un file
> all'altro. `tests/tools/run.sh` dà a ogni caso il proprio processo e gira a
> `nproc` vie. In questa sessione il mio ciclo riportava decine di suite rosse;
> la suite vera ne ha **una** (`check_sort.p0t`, debito MCP noto).
>
> Istruzione operativa di F.: usare `timeout 120 make soft-test` durante il
> lavoro, non `make test`.

### U2 avanzamento — osservazione a costo minimo

`kb_match` ora registra una saturazione soltanto come metadata nel `KB`; la
scrittura riflessiva di `saturated_read/3` avviene una sola volta al confine del
turno (`kb_saturation_commit`). La politica KB attuale guarda `extract_frame/2`
a cap `64` e usa `undetermined_cycle`; i cap piccoli restano sonde legittime.
Il driver C prova commit e ablazione. Resta da decidere se conservare più
saturazioni indipendenti nello stesso turno.

### Decisione di architettura — eliminare gli OR semantici dal C

La modifica locale iniziata nella sessione interrotta ha sostituito una lista C
con questo controllo:

```c
if (kb_query(b->kb, "question_word", q, 1) ||
    kb_query(b->kb, "auxiliary", q, 1) ||
    kb_query(b->kb, "stopword", q, 1) ||
    kb_query(b->kb, "open_quantifier", q, 1)) return 1;
```

È un miglioramento lessicale, ma non è ancora il punto fisso KB-first: la
contingenza «almeno una di queste condizioni vale» è ancora decisa dal C. Non
va risolta aggiungendo una nuova catena di `kb_query` né introducendo un array
di predicati nel motore.

La trasformazione da implementare è:

```prolog
% nome semantico del test, non elenco nascosto nel C
subject_guard($T) :- question_word($T).
subject_guard($T) :- auxiliary($T).
subject_guard($T) :- stopword($T).
subject_guard($T) :- open_quantifier($T).
subject_guard($T) :- social_marker($Kind, $T).
```

`social_marker/2` conserva il proprio tipo ma la proiezione verso il secondo
argomento avviene nella clausola KB; non serve inventare
`social_marker_value/1` nel C. Il C deve poi contenere soltanto una domanda al
predicato stabile:

```c
return kb_query(b->kb, "subject_guard", q, 1);
```

L'obiettivo è che il consumer C faccia **una sola query booleana** a una
relazione, per esempio `subject_guard/1`, mentre l'OR sia la semantica già
espressa dal motore KB tramite più clausole dello stesso capo.

La generalizzazione da conservare è quindi:

```prolog
contingent($Test, $Value) :- evidence($Test, $Value).
contingent($Test, $Value) :- alternative_evidence($Test, $Value).
```

con un binding KB che istanzi `$Test` a `subject_guard` e con il consumer che
chiede `contingent(subject_guard, $T)`. Il nome `subject_guard` è solo il
primo caso d'uso; la capacità da costruire è la valutazione «una qualunque
delle prove dichiarate per questo test» senza modificare il C quando compare
una nuova classe di prova.

Gate obbligatori prima di considerarla chiusa:

1. aggiungere a runtime una nuova clausola/prova del test e mostrare che un
   soggetto nuovo viene bloccato senza ricompilare;
2. ritirare quella clausola e mostrare che il blocco scompare;
3. verificare che il consumer non contenga più `||` fra predicati KB distinti;
4. provare almeno un secondo consumer, perché una soluzione chiamata
   `subject_guard` ma non riusabile sarebbe soltanto una nuova eccezione.

Questa è una trasformazione progettata. Prima di scrivere altro motore va
verificato se il supporto attuale a più clausole e ai termini composti è
sufficiente; in caso contrario si aggiunge una primitiva generale in
`kb/core/procedures.p0`, non un altro OR in `src/brain`.

### Gen466 — prima realizzazione della contingenza

Il primo passo è stato applicato: `subject_guard/1` ora raccoglie in KB le
cinque alternative che prima erano una catena di query C, e `p0_bad_subject()`
fa una sola query. È stata completata anche `location_preposition/1`, che il
consumer locativo aveva già iniziato a interrogare ma che non aveva ancora
ricevuto i fatti. Restano da aggiungere i ratchet runtime prima del commit:
uno per l'aggiunta/ritiro di una clausola `subject_guard/1`, uno per la classe
locativa e uno per la crescita/ablazione della cue `role_open`.

### Gen467 — operazioni dell’act-loop come relazione ternaria

Il parser delle branche dell’act-loop non enumera più in C `double`, `triple`,
`halve`, né i verbi di addizione/sottrazione/moltiplicazione/divisione. La KB
ora espone `agent_branch_step(Surface, Operator, Factor)`; il C interroga la
relazione e conserva soltanto il binding del token, la gestione dell’operatore
pendente e l’aritmetica. Il ratchet deve provare una forma italiana aggiunta a
runtime e la sua ablazione, oltre a una variante inglese già presente.

> Compagno di `KB_TODO.md`, che elenca i residui della conoscenza. Questo
> elenca i residui del **motore**: tutto ciò che oggi vive in `src/brain/*.c` e
> che, per i mantra #2 e #16, dovrebbe essere conoscenza.
>
> Aperto durante la campagna gen442-443. Le misure sono sonde a espressioni
> regolari su 40.918 righe di moduli: ordini di grandezza, non un censimento.

## Stato al 2026-08-27

### Round ibrido 1 — 2026-08-28

Chiuso un target per ciascun fronte operativo:

- **Voce:** la risposta inglese sulla famiglia di parrot0 usa ora il template KB
  `self_family_no_family`, senza `snprintf` umanizzato nel C.
- **Lessico:** il coordinatore `and`/`e` non è più deciso da una lista C; il
  parser interroga `conjunction/1`, con crescita e ablazione runtime verificate.
- **Dominio:** l’assert locativo passa dal binding KB `domain_relation(location,
  located_in)` attraverso `domain_assert()`; `geographic_location.p0t` verifica il
  percorso.
- **Famiglie:** lo slug `got_it_i_ll_treat_x_as_a_conjunction_now_lik` è stato
  rinominato semanticamente in `conjunction_taught`.

Verifica del round: `make build`, `kb_conjunction.p0t` 7/7,
`family.p0t` 9/9, `geographic_location.p0t` 26/26. Il residuo misurato è ora
più basso di una voce, una classe lessicale, una relazione e una famiglia.

### Round ibrido 2 — 2026-08-28

Secondo avanzamento trasversale:

- **Voce:** la risposta `Found {name}` usa ora il template KB `found_file`.
- **Lessico:** il parser dei goal interroga `goal_filler/1` invece di una lista
  locale in C; il `Brain *` è stato propagato ai chiamanti.
- **Dominio:** `wrote/2` è risolto dal ruolo KB `authored_work` tramite
  `domain_match()`.
- **Famiglie:** la famiglia generata per l’acknowledgment di insegnamento è ora
  `teach_form_ack`.

Verifica del round: `make build`, `kb_first_round2.p0t` 8/8 e `family.p0t` 9/9.
Restano separati i tre fallimenti preesistenti di `planact.p0t`.

### Round ibrido 3 — 2026-08-28

Terzo giro completato:

- **Voce:** la lista di capacità mancanti usa ora il template KB con slot
  `items`, senza cornice umanizzata nel C.
- **Lessico:** la lista di parole funzionali poetiche è stata spostata nella
  classe KB, con test di aggiunta e ritiro runtime.
- **Dominio:** il colore è risolto tramite `surface_color -> color_of` e
  l’adapter generico di dominio.
- **Famiglie:** la risposta delle transizioni di sequenza usa la famiglia
  semantica `sequence_transitions_learned`.

Verifica del round: `make -B build`, test focalizzato `9/9`, nessun warning.

### Round ibrido 4 — 2026-08-28

Quarto giro completato:

- **Voce:** l’output dell’haiku usa ora `response_template` invece di una
  composizione umanizzata nel C.
- **Lessico:** i marcatori della forma generativa sono nella classe KB
  `generation_form_marker/1`, con crescita e ablazione runtime.
- **Dominio:** la relazione causale passa da `domain_relation(causal, causes)`.
- **Famiglie:** la famiglia dell’haiku è stata rinominata semanticamente in
  `haiku_theme_unavailable`.

Verifica del round: build senza warning e `kb_first_round4.p0t` 8/8. `make test`
resta bloccato soltanto dalle cinque aspettative di `check_sort.p0t` già note.

### Round ibrido 5 — 2026-08-28

Quinto giro completato:

- **Voce:** l’acknowledgment dell’apprendimento di una classe usa la famiglia
  KB `learned_facts`.
- **Lessico:** i marcatori dei nomi di funzione usano `function_name_marker/1`,
  con prova runtime di crescita e ablazione.
- **Dominio:** la parentela usa `domain_relation(kinship, family_relation)`.
- **Famiglie:** `got_it_i_ll_remember_that` è stata rinominata in
  `personal_acknowledged`.

Verifica del round: `make build` senza warning, `kb_first_round5.p0t` 9/9,
round precedenti e test di famiglia tutti verdi. Il blocco globale resta il
disallineamento già noto delle cinque aspettative MCP di `check_sort.p0t`.

### Round ibrido 6 — 2026-08-28

Sesto giro completato su quattro target ulteriori:

- **Voce:** una risposta di generazione è stata portata a una famiglia
  `response_template` KB.
- **Lessico:** una classe di marcatori del lettore è ora interrogata dalla KB,
  con aggiunta e ritiro runtime verificati.
- **Dominio:** un consumer di riparazione usa ora un binding
  `domain_relation/2` invece del predicato concreto nel percorso C.
- **Famiglie:** una famiglia residua è stata rinominata con una chiave
  semantica stabile.

Verifica: `make build` senza warning, `kb_first_round6.p0t` 8/8 e tutti i round
precedenti verdi. Restano i cinque fallimenti MCP già noti in `check_sort.p0t`.

### Round ibrido 7 — 2026-08-28

Settimo giro completato:

- **Voce:** una conferma familiare usa la famiglia KB `family_acknowledged`.
- **Lessico:** la preposizione di topic usa `topic_preposition/1`, con crescita
  e ablazione runtime.
- **Dominio:** il contenuto passa dal binding `content -> content_kind` e
  dall’adapter generico.
- **Famiglie:** `suddenly_x` è stata rinominata in `sudden_continuation`.

Verifica: `make build`, `kb_first_round7.p0t` 8/8, `family.p0t` 9/9 e
`social_opener.p0t` verde. Restano i cinque fallimenti MCP già noti in
`check_sort.p0t`.

### Round ibrido 8 — 2026-08-28

Ottavo giro completato:

- **Voce:** l'enunciato della regola indotta usa ora il template KB
  `induced_rule_statement` con lo slot `rule`, al posto dello `snprintf`
  umanizzato in `65-induce-verify-shell.c`.
- **Lessico:** i TIPI di pattern sociale non sono piu' una lista C in
  `is_exact_social_pattern()`: sono enumerati dalla KB via
  `social_pattern_type/1`, quindi un tipo nuovo insegnato a runtime diventa
  visibile senza ricompilare. Chiude il `TODO(kb-first)` lasciato nel file.
- **Dominio:** il tratto di ruolo passa dal binding
  `domain_relation(role_trait, trait)` e dagli adapter generici.
- **Famiglie:** lo slug `those_examples_don_t_all_follow_one_rule_i_c` e' stato
  rinominato in `induction_rule_unavailable`.

Verifica: `make build` senza warning e `kb_first_round8.p0t` 9/9, con i round
2-7 tutti verdi. Restano i cinque fallimenti MCP gia' noti in `check_sort.p0t`.

### Round ibrido 9 — 2026-08-28

Nono giro completato, e questa volta la migrazione ha **scoperto un bug**:

- **Voce:** le tre risposte d'identita' in ruolo (`mod_role`) usano ora
  `role_identity_named_profession`, `role_identity_named`, `role_identity_kind`.
  Componevano `msg` con `snprintf` e **non lo scrivevano mai in `out`**: la
  risposta a «who are you?» in ruolo restituiva il buffer del turno precedente.
  La conversione a `kb_term_say` + `put` chiude sia il testo umanizzato sia il
  difetto. Ratchet: `kb_first_round9.p0t`, caso
  `voice_role_identity_actually_answers`.
- **Lessico:** i titoli (`queen`, `pharaoh`, `regina`, `imperatore`, …) erano
  una lista C con un `TODO(kb-first)` esplicito. Ora sono la classe
  `role_title/1` in `kb/core/lexicon.p0`, con crescita e ablazione runtime; un
  titolo insegnato a runtime e' riconosciuto senza ricompilare.
- **Dominio:** chiusi i quattro siti residui che nominavano ancora un predicato
  gia' dotato di binding — le tre `kb_match(… "continuation_template" …)` di
  `30-generation-reading.c` passano da `domain_match(b, "narrative_completion", …)`,
  e l'ultima `kb_query(… "trait" …)` di `10-memory-knowledge.c` da
  `domain_query(b, "role_trait", …)`.
- **Famiglie:** `okay_i_m_myself_again_i_am_parrot0` rinominata in
  `role_cleared`.

Verifica: `make build` senza warning, `kb_first_round9.p0t` 11/11, round 2-8
verdi. Le 112 suite di `meta/`, `conversation/` e `generation/` danno esito
**identico** a quello del commit di round 8 (confronto riga per riga): i rossi
sono il debito preesistente della sezione 6, non una regressione di questo
giro.

### Round ibrido 10 — 2026-08-28

Decimo giro completato. Il tema del giro e' **togliere dal C anche la scelta
della lingua**, non solo il testo:

- **Voce:** le tre conferme di presa di ruolo (`role_set_named`,
  `role_set_kind`, `role_set_plain`) erano `snprintf(msg, …, it ? "Va bene …" :
  "Alright …")`. Ora sono template KB con la forma inglese in
  `response_template/2` e quella italiana in `response_template/3`: il ramo
  `int it = lex_class_member(…)` sparisce dal C e insegnare una terza lingua e'
  una riga di KB, non una ricompilazione.
- **Lessico:** chiusa la **terza copia** dei sequenziatori
  (`99-registry.c`): il peeling del sequenziatore iniziale interroga
  `sequencer/1` in `kb/core/lexicon.p0` — la stessa classe che legge il resto
  del motore — invece della lista duplicata. Restano le due copie di
  `60-agent-tools.c`, gia' annotate con `TODO(kb-first)`.
- **Dominio:** `world_superlative/3` passa dal binding
  `domain_relation(world_extreme, world_superlative)`; le tre query dirette del
  modulo memoria non nominano piu' il predicato concreto.
- **Famiglie:** `that_s_just_punctuation_not_words_what_would` rinominata in
  `punctuation_only` — una delle tre rinomine indicate per nome nella sezione 5.

Verifica: `make build` senza warning, `kb_first_round10.p0t` 12/12, round 2-9
verdi, e la suite `knowledge/` (34 file) con esito identico al commit di round
nove.

### Round ibrido 11 — 2026-08-28 — `gen447-kb-first-lexical-unification`

Undicesimo giro. Il bersaglio e' la nota che il C stesso si era scritto addosso:
*«tre verita' divergenti sulla stessa nozione»*.

- **Lessico (il grosso del giro):** i sequenziatori forti erano scritti **tre
  volte** nel C con contenuti diversi. Il round 10 ha chiuso la copia di
  `99-registry.c`; questo chiude le altre due in `60-agent-tools.c`. La classe
  KB e' `strong_sequencer/1` (frasi anche multi-parola, in
  `kb/core/lexicon.p0`), letta da un unico helper `load_strong_sequencers()`.
  Nel C restano solo le **meccaniche**: gli spazi di confine e il separatore
  `";"`, che e' punteggiatura, non vocabolario. Migrata nello stesso giro la
  lista dei riempitivi di clausola, ora `clause_filler/1`.
- **Voce:** `"I composed \`%s\` but %s."` diventa `composed_with_caveat`.
- **Dominio:** `invented_object/5` passa da
  `domain_relation(invention, invented_object)` — prima volta che l'adapter
  N-ario viene usato su un predicato di arita' 5.
- **Famiglie:** `learned_those_steps_describe_the_x_structure` rinominata in
  `steps_shape_learned`.

Verifica: `make build` senza warning, `kb_first_round11.p0t` 11/11, round 2-10
verdi, e le suite `agent/`, `code/`, `planning/` — quelle che passano da
`compose_plan`, il codice riscritto — con esito **identico** al commit di round
dieci.

### Round ibrido 12 — 2026-08-28 — `gen450-the-wall-is-knowledge`

Dodicesimo giro. Bersaglio: **la voce numero uno della sezione 7.2** — le frasi
del muro, cio' che parrot0 dice piu' spesso e l'unica cosa che non si poteva
insegnare.

- **Voce e lessico insieme:** `not_understood()` teneva due array C, uno per
  lingua, con la rotazione anti-ripetizione scritta a mano accanto. Ma la
  rotazione fra forme intercambiabili `kb_response_slots` **la fa gia' da
  sola** — sceglie fra le righe della stessa famiglia con un contatore per
  chiave — e la scelta della lingua pure. Il C ne teneva una seconda copia
  divergente. Ora sono `wall_classic` (forma canonica) e `wall_generic` (forme
  intercambiabili), ciascuna con `/2` e `/3`; nel C resta solo la meccanica
  «non ripetere l'ultima risposta».
- **Dominio:** tre binding nuovi — `mereology -> part_of`, `idiom ->
  idiom_meaning`, `pair_scale -> pair_magnitude`; nove query dirette in meno.
- **Famiglie:** quattro rinomine — `entailment_not_understood`,
  `action_knowledge_incomplete`, `evidence_conflicting`, `world_slot_busy`.

**La migrazione ha allargato il repertorio, non solo spostato il testo.** Il
vecchio indice era `v[b->fallbacks % 4]`; misurato su otto muri di fila,
percorreva **solo due delle quattro varianti** — «Mmh, questo per ora va un po'
oltre le mie capacita'» non usciva mai. La rotazione di `kb_response_slots` le
visita tutte e quattro. E' l'unica aspettativa cambiata (`apology.p0t`), ed e'
aggiornata con la ragione scritta accanto, non allineata in silenzio.

Verifica: `make build` senza warning, `kb_first_round12.p0t` 14/14, round 2-11
e `reply_language.p0t` verdi, e le 112 suite di `conversation/`, `meta/`,
`knowledge/` identiche a `gen449` a parte quell'unica riga.

### Dove siamo — misura al `gen450`

| sonda | apertura | ora | migrato |
|---|---:|---:|---:|
| letterali-parola in array C | 435 | 243 | 44% |
| parole confrontate inline in `str*()` | 351 | 211 | 40% |
| `snprintf` che compone una frase | 94 | 70 | 26% |
| `put("…")` letterali | 25 | 10 | 60% |
| `TODO(kb-first)` aperti nel C | 32 | 18 | 44% |
| **predicati di dominio senza binding** | **250** | **209** | **16%** |

Totale: `1155` siti all'apertura, `743` ora — **35,7% migrato, 64,3% residuo.**

Fra il round 11 e il round 12 il totale si e' mosso di mezzo punto, e vale la
pena dire perche' invece di far finta che sia lento per caso: **il denominatore
e' dominato dal dominio.** I 209 predicati senza binding sono il 28%
dell'inventario di partenza e il 75% di cio' che resta da fare in ogni altra
categoria messa insieme.

Al ritmo di questi giri — tre o quattro binding di dominio per round — i 209
predicati sono **una cinquantina di round**. Non e' una stima pessimista: e' il
costo reale del fatto che ogni predicato richiede di trovare la relazione piu'
generale che lo comprende (mantra #3), e che quella e' l'unica categoria che
per costruzione non si chiude a macchina.

Le altre quattro categorie, sommate, sono `534` siti di cui `380` gia' fatti:
**71% completo**. Se si guarda solo a «togliere il vocabolario e la voce dal
C», il lavoro e' in dirittura. Se si guarda alla tesi del progetto — che il
motore non sia un'enciclopedia con un parser davanti — siamo al 16%.

### Processo: ogni giro cambia la generazione

> Nota di F., 2026-08-28. I round 8-10 sono stati committati **senza cambiare
> `VERSION`**, quindi `make chat` continuava ad annunciarsi
> `gen396-universal-answer-plan` mentre il motore era gia' molto diverso.
> Sbagliato: la generazione e' l'etichetta con cui si legge un comportamento in
> chat, e se non si muove non si sa piu' che cosa si sta parlando.

**Da qui in avanti, il ciclo di ogni giro e':**

1. la modifica, con `make build` a ogni lotto piccolo;
2. il ratchet `.p0t` del giro, verde, piu' i round precedenti;
3. il confronto con il commit precedente sulle suite toccate (non «i rossi
   c'erano gia'», ma il `diff` riga per riga);
4. **`VERSION` aggiornato a `genNNN-nome-parlante`** — il nome dice che cosa e'
   cambiato, non «round N»;
5. `git commit` e `git push`.

Il numero di generazione riparte dal massimo citato nei documenti (`gen446` al
2026-08-28), non dal valore fermo che c'era in `VERSION`.

| | inizio campagna | ora |
|---|---:|---:|
| punti della voce con chiave KB (`kb_term_say`) | 561 | **658** |
| `put("…")` letterali | 25 | **10** |
| parole confrontate inline in `str*()` | 351 | **211** |
| letterali-parola in array C | 435 | **243** |
| `TODO(kb-first)` aperti | 32 | **19** |
| predicati di dominio con binding | 0 | **38** |
| chiamate che passano dall'adapter di dominio | 0 | **138** |
| template in `kb/core/messages.p0` | 49 | **601** |

I valori «inizio campagna» sono rimisurati sul commit `42082576` con le sonde
di oggi, non ripresi dalle stime originali: la tabella precedente mescolava
regex diverse e non era confrontabile con se stessa.

La regola che governa tutto, ed è più netta del mantra #16
(vedi `docs/plans/messages-are-knowledge.md`):

> **Nel C non deve esistere nessun testo umanizzato.** Il default, quando la KB
> non è addestrata, è la forma funzionale del messaggio — `famiglia(valore)` —
> non una frase più povera. `kb_term_say/6` lo fa già: se in chat compare un
> termine, è una famiglia che nessuno ha ancora insegnato, e si vede quale.

---

## 1. Gli undici `snprintf` rimasti — e nessuno è un messaggio

Vanno chiusi **decidendo che cosa sono**, non estraendoli a macchina.

### 1a. Frammenti di composizione (5)

Costruiscono una frase pezzo per pezzo, quindi la famiglia giusta è la **frase
intera**, che va disegnata.

- `10-memory-knowledge.c:1058` — `"Learned rule: %s("`
- `10-memory-knowledge.c:12319` — `"Learned rule: %s(X) :- "`
- `20-math.c:2431` — `"Near-rhymes for \"%s\": "`
- `60-agent-tools.c:566` — `` "Matches for `%s`" ``
- `10-memory-knowledge.c:579,628` — `"%s your name is %s"`, `"%s current topic is %s"`
  (il primo `%s` è un prefisso variabile: «I remember: » / «You told me: »)

**Come chiuderli:** una famiglia per la frase completa, con gli slot che oggi
sono pezzi separati — `learned_rule(head, body)`, `near_rhymes(word, list)`.
Serve guardare il ciclo che li compone, non solo la riga.

**Lotto successivo chiuso il 2026-08-27:** le due composizioni di regole
apprese in `10-memory-knowledge.c:1058-1071` e `12323-12335` ora costruiscono
solo il valore dello slot `rule` e delegano la frase completa a
`response_template(learned_rule_text, ...)`. Il testo della regola resta dato
calcolato dal motore; la cornice linguistica è nella KB.

Nello stesso lotto, `20-math.c:2431-2437` usa ora
`response_template(near_rhymes_for_x, ...)`; il motore calcola soltanto la lista
e la passa allo slot `list`.

Il lotto corrente porta inoltre in KB la label di ricerca di
`60-agent-tools.c:566` e le due cornici di continuazione narrativa di
`30-generation-reading.c:1362-1364` (`matches_for_x`, `suddenly_x`,
`continuation_x`).

Il lotto include anche `30-generation-reading.c:1579` (`weight_updated`) e i
due arresti diagnostici del piano in `25-wordmath-reasoning.c:784,798`, ora
resi con `plan_stopped_missing_action` e `plan_stopped_via_action`.

La composizione della memoria personale in `10-memory-knowledge.c:576-636` ora
usa template KB per intestazione, nome, possesso, preferenza, umore, topic e
vincolo; il C conserva soltanto l'assemblaggio dei frammenti e i valori di
sessione.

Questo chiude anche il residuo `10-memory-knowledge.c:579,628` della lista
iniziale di composizioni.

### Percentuale operativa — quanto manca davvero

Le cifre della campagna erano prodotte da sonde diverse fra loro e non
riproducibili. Questa sezione usa **una sola batteria di sonde**, applicata sia
al commit di apertura di questo file (`42082576`) sia allo stato corrente, in
modo che i due numeri siano confrontabili. Comandi in
`docs/plans/messages-are-knowledge.md`.

| sonda (stessa regex ai due estremi) | apertura | ora | migrato |
|---|---:|---:|---:|
| letterali-parola dentro array C (`static const char *const`) | 435 | 243 | 44% |
| parole confrontate inline in `strcmp`/`strstr`/… | 351 | 211 | 40% |
| `snprintf` che compone una frase | 94 | 70 | 26% |
| `put("…")` letterali | 25 | 10 | 60% |
| `TODO(kb-first)` aperti nel C | 32 | 19 | 41% |
| predicati di dominio con binding `domain_relation/2` | 0 / 250 | 38 / 250 | 15% |

Sommando i siti censiti — `435 + 351 + 94 + 25 + 250 = 1155` all'apertura,
`243 + 211 + 70 + 10 + 212 = 746` ora — il rapporto grezzo e':

> **circa 35% migrato, quindi ~65% dei siti censiti manca ancora.**

Il conto e' *grezzo* nel senso preciso del termine: tratta un letterale di
lista e un predicato di dominio come un sito ciascuno, e non lo sono. Se si
guarda per categoria il quadro e' molto piu' netto:

- **la voce e' quasi finita** — `put` letterali al 60%, e i due terzi degli
  `snprintf` rimasti sono composizione di dati (`"%s is called by "`), non
  frasi;
- **il lessico e' oltre meta'** — 44%, e ogni giro ne chiude una classe con
  prova di crescita e ablazione a runtime;
- **il dominio e' il collo di bottiglia, al 15%** — ed e' anche la voce che la
  sezione 4 definisce «la piu' grave rispetto alla tesi del progetto». Finche'
  resta li', il numero complessivo non salira' molto: 212 predicati di dominio
  sono da soli il 28% dell'inventario residuo, e sono l'unica categoria che
  **non si chiude a macchina**, perche' ogni caso vuole che si trovi la
  relazione piu' generale che lo comprende.

La stima onesta, quindi: **~65% da fare**, ma il 65% non e' omogeneo — e' per
lo piu' il dominio, e il dominio si paga a mano.

### 1b. Dati travestiti da testo (4)

Non sono la voce di parrot0: sono **superfici di concetti**. La loro casa non è
`messages.p0`, è il lessico.

- `10-memory-knowledge.c:11491-11492` — `"the United States"`, `"the United Kingdom"`
  (dovrebbero essere `concept_label/4` o equivalente: la forma con articolo di
  un nome di paese)
- `30-generation-reading.c:1022` — `"quiet street"` (materiale di scena)

**Come chiuderli:** una riga di lessico e un `kb_match`, non un
`response_template`. Metterli fra i template li archivierebbe nel posto
sbagliato solo per far scendere un contatore.

Le due etichette dei paesi sono state migrate a `concept_label/4` in
`kb/core/world-facts.p0`; il ramo C ora le risolve tramite
`concept_label_lookup()`.

La superficie di scena `quiet street` è ora `story_default(place, ...)` in
`kb/templates/story_atoms.p0`; il lettore la interroga invece di
hardcodarla.

La classe `world`, `story`, `scenario`, `puzzle` è stata estratta dai cinque
chiamanti di `is_world_noun()` in `85-translate-synth-world.c` e registrata
come `world_scope_noun/1` in `kb/core/lexicon.p0`.

Il lotto lessicale successivo ha portato in KB unità di ricetta/lunghezza,
determinanti inglesi/italiani, pronomi di entità, marcatori di negazione,
stopword personali, connettivi contrastivi e predicati di atteggiamento sociale.
I relativi helper ora ricevono `Brain *` e interrogano fatti modificabili.

Gli opener interrogativi di `85-translate-synth-world.c` ora usano le classi KB
`auxiliary/1` e `question_word/1`; il duplicato di parole nel C è stato rimosso.

La migrazione degli operatori aritmetici ha inoltre sostituito le liste in
`arith_op_char()`/`is_arith_op()` con `infix_operator/2`, includendo le forme
verbali inglesi e italiane e il divisore. Il C conserva solo la mappatura del
nome canonico alla primitiva numerica.

Primo gruppo di predicati di dominio: le query geografiche sui confini non
nominano più direttamente `borders` nel C. Usano il binding KB
`domain_relation(neighbor, borders)` e l'adapter generico
`domain_query()` / `domain_match()`; la relazione concreta è
quindi sostituibile dalla KB.

Per relazioni N-arie non si aggiunge un helper C per ogni arità: l'adapter unico
riceve `role`, un vettore di argomenti e `argc`, risolve
`domain_relation(role, predicate)` e inoltra la query a `kb_query` o `kb_match`.
Le relazioni binarie già migrate usano ora questo percorso generale.

La stessa astrazione ora copre le query di capitale: `capital_of_country` è
risolta dal binding `domain_relation(capital, capital_of_country)` invece che
da sette chiamate dirette nel modulo memoria.

Il lotto `gen462` ha inoltre spostato in KB le estensioni e i nomi dei linguaggi
usati dal comando di elenco file (`file_extension/1`, `file_kind_word/2`); il
parser conserva solo la meccanica del glob esplicito.

Il lotto `gen462` ha anche portato `has_arith_cue()` alla classe KB
`arithmetic_word/1`, lasciando nel C soltanto il riconoscimento meccanico dei
simboli operatori.

Il lotto `gen464` ha migrato le superfici di richiesta shell e sorpresa/
supposizione a `intent_cue/2` e le cornici dei risultati dei tool a template KB.
La saturazione resta registrata come osservazione, ma la guardia non è attiva
per default finché il motore non distingue una vista consultata per decidere
da una vista preparatoria.

Il percorso N-ario copre ora anche `magnitude/3` e `planet_superlative/3`:
quattordici query del modulo memoria risolvono il predicato tramite
`domain_relation/2`, lasciando nel C solo il ruolo semantico della richiesta.

Migrata anche la relazione trasversale `category_member/2`: i consumer usano il
ruolo `membership` e l'adapter N-ario, mentre il binding concreto resta nella
KB. Questo gruppo copre tutte le query dirette censite nei moduli memoria,
aritmetica, lettura e riparazione.

Secondo gruppo di dominio migrato tramite `domain_relation/2`: differenze,
suoni, assenza di confine terrestre, colori, opposti e punti di riferimento.
Le query dirette corrispondenti non nominano più i predicati concreti nel C.

Terzo gruppo: le firme e le risposte degli indovinelli (`riddle_sig/2` e
`riddle_answer/2`) usano anch'esse ruoli KB, senza adapter dedicati.

Quarto gruppo misto: distanza, tempo d'incontro, quantità, costi proporzionali,
resto, conteggio parti, causalità e continuazioni sono ora risolti tramite
ruoli `domain_relation/2`; sono state rimosse altre query dirette dal modulo
wordmath.

Il dominio `quantity/3`, `causes/2`, `same/2` e `cont/3` usa ora anche
`domain_assert()`/`domain_retract()`;
le conferme “Learned” sono state spostate nei template KB insieme alle query.

Quinto gruppo geografico e semantico: `river_of/2`, `ocean_borders/2`,
`ocean_west_of/2` e `kind_is/2` usano ora ruoli KB e l'adapter N-ario.

Completata la copertura delle query residue di `ocean_borders/2` e della
relazione didattica `capital/2`, entrambe instradate da ruoli distinti per non
confondere il predicato enciclopedico con quello usato dai test teachable.

Lotto output: `entailment_status()` usa template KB anche per `Unknown`,
`Neutral`, `Conflicted`, `Contradicted`, `Contradiction` e le spiegazioni
entailment; i label del benchmark restano separati dal linguaggio naturale.

Migrati inoltre output singoli da `20-math.c`, `40-meta-reflection.c` e
`80-code.c`: conferma di `requires`, spiegazione `Because ...` e compilazione
riuscita. Le cornici sono ora template KB con gli slot dei valori calcolati.

Questo giro aggiunge la diagnosi `code_not_valid` e l'identità dinamica
`self_identity`, entrambe senza testo di risposta hardcoded nel C.

Migrati anche i messaggi di apprendimento/retract di fatti unari e binari in
`10-memory-knowledge.c`; predicato e argomenti restano slot, la cornice è KB.

Estesa la stessa migrazione ai frame di estrazione: apprendimento binario,
fatto unario già noto e rifiuto di un fatto non composto da concetti.

Le due risposte causali residue `Because ...` ora riusano il template
`because_proof`.

Migrata anche la risposta dinamica della relazione `count_of/2` tramite
`count_of_answer`.

La query `count_of/2` ora usa inoltre il ruolo `count` dell'adapter dominio,
chiudendo il passaggio diretto rimasto nel modulo memoria.

Il parser loose dei goal in `90-repair-robust-abduce.c` legge ora i filler da
`goal_filler/1`, rimuovendo un'altra lista di parole dalla logica C.

Migrati anche hedge e quantificatori aperti di `70-social-pragma.c` in
`hedge_word/1` e `open_quantifier/1`.

Migrata anche la relazione `is_a/2` nei consumer di memoria e ricerca prosa,
tramite il ruolo `isa` e gli adapter generici di query/assert.

La composizione delle proprietà aggettivali non inserisce più la cornice
`Learned:` nel C; la lista calcolata viene resa da `learned_properties`.

Migrata anche la cornice della spiegazione multi-step in `howknow_reply()` con
`reasoning_steps`.

Migrata la risposta del call graph in `80-code.c`: l'elenco dei callees è ora
uno slot di `code_calls_answer`, non una frase composta dal C.

Migrata anche la cornice `Induced:` del riepilogo delle regole dedotte in
`10-memory-knowledge.c`; il C costruisce solo l'elenco delle regole.

Sono state migrate anche le cinque varianti di autoriflessione su ruolo e
tratti (`self_role_*`, `self_trait_*`).

Le unità di ricetta e di lunghezza di `25-wordmath-reasoning.c` ora usano le
classi KB `recipe_unit/1` e `length_unit/1`; i relativi helper non contengono
più liste di parole nel C.

Le liste di articoli di `80-code.c` sono state sostituite da
`english_determiner/1` e `italian_determiner/1`, interrogate dal traduttore.

### 1c. Diagnostica di piano (2)

- `25-wordmath-reasoning.c:784,798` — `"%s stopped at step %zu %s because no
  action_impl fact named…"`, `"%s stopped at step %zu %s via %s: %s."`

Sono messaggi veri e convertibili; hanno quattro-cinque slot e meritano nomi di
slot scelti a mano.

## 2. I `put("…")` letterali residui

**Lotto chiuso il 2026-08-27.** I messaggi umanizzati residui affrontati sono stati portati a
`kb_term_say()`: `induce`, `translate`, apertura mondo, i due rifiuti di
`robust`, l'errore di entailment e la conferma del goal. Sono stati riusati i
template già presenti quando esistevano; l'unico nuovo template è
`goal_noted`.

Le `put("…")` che restano nel C sono valori di protocollo o di benchmark, non
voce rivolta all'interlocutore: `Yes.`/`No.` nei probe booleani e le etichette
`entailment`/`contradiction`/`neutral` dell'output SuperGLUE. Non vanno convertite
in `response_template`, perché cambierebbero il contratto della sonda.

## 3. Le 292 parole ancora confrontate nel C

È il residuo del mantra #2, e la parte più grande che resta. Concentrate in:

| file | conta |
|---|---:|
| `25-wordmath-reasoning.c` | 60 |
| `10-memory-knowledge.c` | 43 |
| `20-math.c` | 43 |
| `60-agent-tools.c` | 30 |
| `90-repair-robust-abduce.c` | 29 |
| `70-social-pragma.c` | 15 |

Quasi tutte stanno in **funzioni ausiliarie che non ricevono un `Brain`**
(`is_entity_pronoun`, `wp_length_unit`, `arith_op_char`, `eval_operand`,
`parse_branch_ops`, `parse_goal_loose`, …). Circa 35 funzioni.

**Come chiuderle:** aggiungere `Brain *b` come primo parametro e aggiornare i
chiamanti, poi far girare la conversione. **Non in blocco**: un tentativo
automatico su tutte e 35 ha rotto la build in dieci punti, perché il threading
va a cascata e alcuni chiamanti lavorano su un sub-brain (`run_composition` usa
`sub`, non `b`). Poche per volta, con il compilatore a ogni passo.

Non tutti i letterali sono uguali: `strcmp(tok, "@S")` confronta un simbolo del
protocollo e va benissimo. Il numero vero è più basso di 292, ma resta nelle
centinaia.

## 4. I 250 predicati di dominio nominati nel C

**La voce più grave rispetto alla tesi del progetto**, e la sola che non si può
affrontare a macchina.

Dei 465 nomi di predicato scritti nei moduli, 215 sono di **protocollo** —
`input_frame_commit`, `turn_teaching_offer`, `turn_gap_middle`: il contratto
aperto fra motore e conoscenza, documentato e legittimo. Gli altri **250 sono
di dominio**: `borders`, `capital_of_country`, `planet_superlative`,
`magnitude`, `category_member`, `riddle_sig`.

Finché il motore nomina `borders`, non è un adattatore: è un'enciclopedia con
un parser davanti. Ogni caso richiede di trovare (o costruire) la relazione più
generale che lo comprende — mantra #3, astrai fino al punto fisso — e non si
chiude con una sostituzione.

## 5. I nomi di famiglia generati per slug

Le famiglie create dalla campagna hanno nomi derivati dal testo inglese:
`got_it_i_ll_treat_x_as_a_conjunction_now_lik`. Andava bene quando la chiave era
invisibile; **ora la chiave è la voce di riserva**, quindi è ciò che
l'interlocutore legge quando la KB tace.

Vanno rinominate le famiglie che si vedono spesso — `conjunction_taught`,
`name_recall`, `punctuation_only` — lasciando lo slug alle altre. Una rinomina
tocca due punti: la riga in `kb/core/messages.p0` e la chiave nel sito C.

## 6. Il debito lasciato dalla campagna

- **Test rossi.** La campagna ha cambiato la forma di molti messaggi (le
  virgolette dritte sono diventate « »), e i commit di conoscenza appresa hanno
  aggiunto template che entrano nella rotazione: `teachverb`, `retract`,
  `glue`, `reflexive_*`, `prosefact.it` e altri si aspettano formulazioni
  precedenti. La bisezione è già fatta e sta nel messaggio di `b81b52b`.
- **`messages.p0` è cresciuto per accumulo**, non per disegno: 524 template in
  un file solo, in ordine di conversione. Va diviso per famiglia quando
  diventerà scomodo — non prima (mantra sulle strutture secondarie).
- **`cue(norm, "?")`** in `30-generation-reading.c:82` resta, ed è corretto: la
  punteggiatura è una meccanica, non vocabolario.
- **Verifica del lotto 2026-08-27.** `make build` e `git diff --check` passano.
  `make soft-test` raggiunge la suite ma resta rosso su 31 aspettative nelle
  sonde di dialogo italiano/inglese. Il fallimento non è stato attribuito a
  questo lotto e va trattato come debito separato, non mascherato aggiornando
  i template appena migrati.

## 7. TODO aperti che non sono migrazione

Voci chieste esplicitamente e non ancora fatte. Non sono debito della campagna
KB-first: sono lavoro del motore, e vanno tenute separate per non confonderle
con i contatori della sezione «Percentuale operativa».

### 7.1 History dei messaggi nella chat interattiva (frecce su/giu')

**Chiesto da F. il 2026-08-28.** Oggi `make chat` legge la riga con una lettura
di linea nuda: premere freccia-su non richiama il messaggio precedente, scrive
la sequenza di escape. Serve il comportamento standard di un terminale Unix —
freccia su/giu' per navigare la history dei messaggi inviati nella sessione.

Cosa va deciso prima di scrivere il codice:

- **Dove sta la history.** In sessione e basta, o persistita fra sessioni (e in
  quel caso vicino a `PARROT0_SESSION`, non in un file nuovo scelto a caso).
- **Se e' conoscenza.** Probabilmente no: e' meccanica del terminale, come la
  punteggiatura della sezione 6 — la history *dei turni* invece esiste gia' nel
  Brain, e vale la pena guardare se la si puo' riusare come sorgente invece di
  tenerne una seconda copia nel loop di lettura. Questa e' la domanda zero del
  progetto applicata qui: due liste della stessa nozione sono il sintomo che
  abbiamo gia' pagato tre volte con i sequenziatori.
- **Se dipendere da `readline`.** Aggiunge una dipendenza esterna e con essa
  history, editing e completamento gratis; l'alternativa e' la modalita' raw a
  mano (~150 righe: `termios`, il parsing di `ESC [ A` / `ESC [ B`, il
  ridisegno della riga). La build non linka `readline` oggi, e va verificato
  che non rompa i target headless (`--test-engine`, `--bench-engine`) dove
  l'input non e' un TTY: la modalita' history va attiva **solo** se
  `isatty(STDIN_FILENO)`.

Punto d'ingresso gia' individuato: `src/main.c:1022`, la `fgets(line, sizeof
line, stdin)` del ciclo interattivo. La guardia `int interactive =
isatty(STDIN_FILENO)` **esiste gia'** dodici righe sopra (`src/main.c:1010`),
quindi la modalita' raw si aggancia li' senza inventare una condizione nuova.

---

### 7.2 La risposta esce in inglese in una sessione italiana

**Segnalato da F. il 2026-08-28**, da questa sessione:

```
you> come stai
Sto bene, grazie. Come posso aiutarti?
you> genera un template html
I understood the request — produce «template html» — but I don't have a
verified schema for that artifact yet; …
```

**Diagnosi: non e' un bug del motore, e' una lacuna di conoscenza** — che e'
esattamente la forma che questo progetto vuole. `kb_response_slots` preferisce
gia' `response_template/3` per la lingua del turno e ricade sulla `/2`; e a quel
turno `current_language(it)` **e' vero** (verificato con una sonda `.p0t`). La
risposta e' uscita in inglese solo perche' quella famiglia non aveva la riga
italiana.

Chiuso per questa famiglia (`gen448`), ma la misura dice che il caso non e'
isolato:

| | |
|---|---:|
| famiglie con forma `/2` (default inglese) | 829 |
| famiglie con forma `/3` italiana | 141 |
| **copertura italiana** | **16,5%** |

Cioe': in una sessione italiana, **piu' di quattro famiglie su cinque
rispondono in inglese.** Non e' un difetto di codice da nessuna parte — e' 692
righe di KB che non sono state scritte.

**La correzione da NON fare:** un ramo `it ? "…" : "…"` nel C. Il round 10 ne
ha appena tolto uno; riaggiungerlo per andare piu' veloce farebbe regredire
l'esperimento anche a test verdi.

Ordine di lavoro suggerito, per valore:

1. **Le frasi del muro.** `not_understood()` in `99-registry.c:1547` tiene
   ancora **entrambe le lingue dentro il C**, con un `TODO(kb-first)` gia'
   scritto. E' la cosa che parrot0 dice piu' spesso ed e' l'unica che non si
   puo' insegnare: va per prima, come famiglie con `/2` e `/3`.
2. **I declini.** Sono cio' che un utente italiano incontra di piu', perche' si
   incontrano proprio quando la KB non sa rispondere.
3. **Il resto**, con una sonda di copertura come ratchet, cosi' che la
   percentuale non possa scendere.

### 7.3 L'errore di battitura non viene riparato

Dalla stessa sessione, ed e' il punto piu' interessante dei tre:

```
you> genere un template html      ->  Non capisco ancora.
you> genera un template html      ->  (capito)
```

Un carattere. `intent_cue(make_verb, "genera")` e' un confronto di superficie
esatto, quindi `genere` non aggancia niente e il turno cade al muro.

**Il confine giusto.** La distanza di edit e' *meccanica*: e' una metrica fra
stringhe e non puo' essere un fatto, quindi puo' stare nel C senza violare il
mantra #2 — allo stesso titolo della punteggiatura. Ma **tutto cio' che decide
che farsene deve essere conoscenza**:

- quali classi sono candidate alla riparazione (`intent_cue`, `code_action`, …)
  — un fatto, non un elenco nel C;
- quante correzioni sono ammesse — un fatto (`repair_max_edits(1)`), non una
  costante;
- quali token sono **protetti** — un `lexeme/1` noto, un `proper_name/1`, un
  numero non si "correggono" mai.

**La guardia che rende il caso difficile:** `genere` *e' una parola italiana
vera* (il genere letterario, il genere grammaticale). Oggi non e' in
`lexeme/1`, quindi una riparazione che scatta su «token sconosciuto» prima o
poi correggera' una parola vera che la KB non conosce ancora — e lo fara' in
silenzio.

Da cui la regola di disegno, che e' la regola generale del progetto applicata
qui: **parrot0 deve dire che cosa ha assunto, non sostituire di nascosto.**

```
you> genere un template html
parrot0> Non conosco «genere». Intendevi «genera»? (a un carattere da un
         verbo che so eseguire)
```

Una sostituzione silenziosa *riduce* cio' che l'interlocutore vede per avere
ragione per costruzione: e' il lato sbagliato della regola che riassume tutte
le altre. Una domanda di conferma *aumenta* cio' che entrambi vedono — e in
piu' la risposta e' insegnabile, perche' un «si'» diventa un fatto.

Casa naturale: `mod_repair` in `90-repair-robust-abduce.c`, che e' gia' nella
catena di dispatch **prima** dei moduli che rispondono, non dopo il muro.

### 7.4 «genera un template html» — che cosa manca davvero

Il declino del terzo turno **e' corretto e va difeso**: parrot0 sintetizza solo
cio' che un oracolo puo' controllare, e lo dice elencando cosa sa fare. E' la
regola anti-inganno di `PRINCIPLES.md`, non un limite da aggirare.

Quindi la domanda giusta non e' «aggiungiamo la generazione di HTML», e':
**qual e' l'oracolo di un template HTML?** E ce n'e' uno ovvio — la buona
formazione strutturale: `<!doctype>` presente, tag bilanciati, annidamento
corretto. E' controllabile a macchina esattamente come
`code_check_print_program()` controlla che un programma stampi davvero la sua
stringa.

Il seam esiste gia' ed e' KB-first: `60-agent-tools.c:1975` interroga
`program_shape/2` — uno schema **dichiarato dalla KB** — sintetizza il
candidato e lo **dispone eseguendolo**. Un template HTML e' la stessa figura
con un oracolo diverso:

- `document_shape(html_page, …)` in `kb/experts/`, con gli slot del documento;
- un oracolo strutturale accanto a `code_check_print_program()`;
- nessuna stringa HTML nel C, e nemmeno **una** stringa HTML unica nella KB:
  sarebbe un phrasebook di documenti. Lo schema deve avere slot, altrimenti
  parrot0 non ha imparato a generare, ha imparato a citare.

Il test operativo del progetto, applicato qui: *parrot0 puo' imparare un
formato di documento nuovo domani, senza ricompilare?* Se per l'HTML si scrive
un ramo dedicato nel C, la risposta e' no, e il punto era proprio quello.

---

---

## Le guardie imparate sbagliando

Chi riprende questa campagna eviti di ripetere questi quattro errori, tutti
commessi e corretti durante gen442-443:

1. **Un frammento non è un messaggio.** Se il testo ha parentesi o virgolette
   spaiate, o finisce con un separatore, sta componendo qualcosa: la famiglia
   giusta è la frase intera.
2. **`snprintf` come espressione va lasciato stare.** `off = (size_t)snprintf(…)`
   usa il valore di ritorno; sostituirlo lascia un'assegnazione senza destra.
3. **Le dichiarazioni issate non stanno nel corpo nudo di un `if`.** Servono le
   graffe, tranne quando il prefisso dichiara una variabile usata dopo — e in
   quel caso, se è anche un corpo nudo, si lascia stare.
4. **Il rilevamento della guardia KB dev'essere stretto.** Guardare indietro
   senza fermarsi al confine di istruzione fa saltare i siti *vicini* a una
   conversione precedente, che non c'entrano nulla.

E una regola di metodo che ha retto per tutta la campagna: **convertire a lotti
piccoli, con `make build` e una prova a mano dopo ognuno, e committare ogni
lotto.** Un lotto grande che rompe la build costa più di dieci lotti piccoli.
### Gen467 — vocabolari di procedura e di modulo come dati KB

Il parser dell’act-loop legge `agent_branch_step(Surface, Operator, Factor)`;
le famiglie operative (raddoppio/triplicazione, divisione, somma/sottrazione e
moltiplicazione) sono clausole insegnabili, non enum C. Nello stesso lotto le
descrizioni delle capability e i tipi phatic sono relazioni KB interrogate dal
consumer. Il test `kb_first_round14.p0t` verifica query, assert e forget del
vocabolario nuovo e il comportamento EN/IT. Il tentativo di portare anche i
profili del self-composition in KB è stato scartato perché cambia il contratto
del golden test: resta una migrazione successiva, non dati inutilizzati.

### Gen468 — valutazione composta delegata alla procedura KB

`apply_arith_op` non contiene più la tabella C di `+ - * /`: risolve la
superficie con `infix_operator/2` e valuta tramite `apply_operator/4`. Il C
mantiene soltanto conversione numerica, controllo `ok` e gestione del risultato.
Le regressioni `algebra2.p0t`, `algebra2.it.p0t` e `arith_flex.p0t` proteggono il
percorso composto bilingue.

### Gen472 — lessico numerico condiviso nella KB

`number_word/2` sostituisce le liste duplicate in `word_to_int`, nel conteggio
categoriale e nel word-query. C conserva parsing delle cifre e composizione
aritmetica; le parole numeriche sono dati insegnabili e il ratchet verifica
assert/query/forget con `septendecim`.

### Gen469 — inverse algebriche come relazione KB

Le quattro inversioni del solver (`left/right × +,-,*,/`) sono ora dati in
`algebra_inverse/4` e `algebra_inverse_order/3`; il C interroga la procedura e
mantiene solo l’ordine degli argomenti, il calcolo e il rendering strutturale.

### Gen470 — tokenizzazione algebrica senza enum C

`algebra_op` delega ora a `infix_operator/2` invece di enumerare superfici
inglesi/italiane nel C. Il tokenizer resta meccanico (separatori e binding),
mentre il significato dell’operatore è KB-teachable; le regressioni algebriche
EN/IT e `arith_flex` restano verdi.

### Gen473 — knowledge heads come dati runtime

Le superfici `strong` e `weak` del percorso research sono state spostate da
`strong_heads[]`/`weak_heads[]` alla relazione `knowledge_head/2`. Il C conserva
solo precedenza, estrazione del topic e binding; il ratchet `kb_first_round15`
verifica assert/query/forget di una nuova superficie. Il golden research
storico ha un mismatch di risposta indipendente dal rollback, registrato
separatamente e non mascherato.

### Gen474 — vocabolari symbolic e code in KB

`solfege_note/1` e `code_keyword(Language, Word)` sostituiscono le tre liste C
di note, keyword Python e keyword C. Il riconoscimento resta scansione e
struttura; le parole che nominano il registro sono teachable. `kb_first_round16`
verifica crescita e ablazione runtime.

### Gen471 — topic e constraint come user_value di sessione

`current_topic` e `user_constraint` non vengono più scritti o letti dai campi
C: il consumer usa `user_value_write/read`, con la stessa semantica di slot
sovrascrivibile e supersedibile. La selezione del topic resta meccanica; nome,
valore e stato della sessione sono conoscenza interrogabile dalla KB.

### Gen475 — pattern di sintassi C come conoscenza KB

I prefissi `return `, `int ` e `char *`, usati dal verificatore dei frammenti C,
sono ora fatti `code_pattern/3` nella KB. Il C conserva soltanto la scansione
del testo, l'avanzamento sul prefisso trovato e i controlli strutturali (`=` ,
virgolette, cifre e `;`); non decide più quali parole nominano quei costrutti.
`kb_first_round16.p0t` verifica anche assert/query/forget di un pattern inventato
a runtime. Il test `codeintent.p0t` mantiene un mismatch di wording preesistente
(`wrong with this the sky...`), da sistemare in un lotto dedicato.

### Gen476 — classi lessicali del pianificatore interrogate come KB

`plan_request_fn` non confronta più direttamente le parole di chiamata e i
connettori (`call/calls/chiamata/chiamate`, `to/a/di`): usa
`function_call_word/1` e `function_link_word/1`. Nello stesso lotto le guardie
`goal_filler/1` e `function_name_marker/1` sono passate da un helper di classe a
query KB dirette. Il C conserva solo la finestra ordinata di tre token e la
validazione meccanica del nome; `kb_first_round16.p0t` dimostra crescita e
ablazione runtime di una nuova parola di chiamata.
La regressione `planact.p0t` continua a mostrare tre differenze storiche di
wording/slot del piano; il lotto non le nasconde né le corregge incidentalmente.

### Gen477 — guardie grammaticali dell’entailment nella KB

Le parole `is`, `the`, `of` e la coppia di copule dell’universale non sono più
confrontate direttamente nei due lettori di `10-memory-knowledge.c`: il C
interroga rispettivamente `entailment_copula/1`, `entailment_article/1`,
`entailment_relation_preposition/1` e `universal_copula/1`. Restano meccanici la
forma della frase, l’estrazione degli argomenti e il binding del predicato. Il
ratchet mostra anche assert/query/forget di una copula nuova.
Le regressioni `entail.p0t` e `syllogism_universal.p0t` mantengono il loro
contratto storico; il primo riconosce le forme ma conserva nove differenze di
template già presenti, mentre il secondo passa 1/1.

### Gen478 — frasi di apertura dell’induzione come dati KB

Le undici forme inglesi e italiane che aprono una richiesta di induzione sono
state raccolte in `induce_query_phrase(Surface, Kind)`. Il consumer enumera le
superfici dalla KB, sceglie la corrispondenza più precoce (e, a parità, più
lunga) e conserva solo il tipo operativo `continue/next/rule`; offset, parsing
degli esempi e calcolo restano meccanici. Il ratchet verifica anche una nuova
forma aggiunta e ritirata senza ricompilare.

### Gen479 — pattern strutturali del verificatore codice nella KB

I marker `void `, `[]`, `int`, `for(`, `while(` e `print(` non sono più
letterali nel riconoscitore C: sono fatti `code_pattern/3`. L’helper interroga
la KB e il consumer conserva solo scansione, estrazione dell’identificatore e
controlli di struttura. Le sei superfici sono coperte dal ratchet di
`kb_first_round16.p0t` e dalle regressioni del modulo code.
`check_sort.p0t` conserva cinque differenze di sorgente MCP già note, pur
riportando i verdetti attesi; non sono state mascherate in questo lotto.

### Gen482 — marker di parità del ciclo agente nella KB

Le superfici `even`, `odd`, `pari`, `dispari` sono ora fatti
`agent_parity_marker/2`. Il ramo agente interroga la relazione e conserva solo
la delimitazione delle clausole; la parità del numero resta calcolo meccanico.
Il ratchet verifica crescita e ablazione di una superficie nuova.

### Gen480 — le quattro operazioni composte delegate alla KB

Lo switch locale di `apply_op_char` non calcola più `+`, `-`, `*` e `/` nel C:
ogni operatore passa a `apply_operator/4` attraverso `apply_arith_op`, già
usato dal percorso semplice. Il C conserva soltanto il simbolo, il binding
degli operandi e il controllo `ok`; la divisione per zero resta una condizione
del predicato KB. Le regressioni aritmetiche bilingui proteggono il percorso.
`arith_flex.p0t` mantiene un solo mismatch storico nel fallback testuale di
`gold + silver`; i percorsi numerici e algebrici del lotto restano verdi.

### Gen481 — superfici delle azioni tool nella KB

Le superfici `run `, `compile`, `learn `, `impara ` e i delimitatori sorgente
` from`/` da ` sono ora fatti `tool_surface(Kind, Surface)`. Il C enumera e
seleziona la superficie più precoce/più lunga, poi conserva solo gli offset e
il parsing del comando o dei passi. L’aggiunta di una forma nuova non richiede
ricompilazione.
