# C_TODO — che cosa deve ancora uscire dal C

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

| | inizio campagna | ora |
|---|---:|---:|
| punti della voce con chiave KB | 160 | **656** |
| `snprintf` di frase senza chiave | 297 | **11** |
| `put("…")` letterali | 77 | **7** |
| `cue(x, "letterale")` | 1393 | **1** |
| parole in `strcmp`/`strstr` | 1629 | **292** |
| predicati di dominio nominati nel C | 250 | **250** |
| template in `kb/core/messages.p0` | 49 | **524** |

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

### Percentuale operativa

Sul perimetro iniziale della **migrazione dei messaggi** (`11` `snprintf` senza
chiave + `7` `put` letterali, `18` siti complessivi), i lotti fin qui chiusi
hanno migrato `16/18` siti: **circa 89% completato, 11% residuo**. Questa è
solo la misura locale dei messaggi già censiti.

La misura onesta del lavoro complessivo censito è molto più bassa: oltre ai
`18` siti di messaggistica restano `4` superfici lessicali, `292` confronti di
parole, `250` predicati di dominio e le rinomine delle famiglie generate. Senza
pretendere che categorie diverse abbiano lo stesso peso, il rapporto grezzo è
circa `25/564`, quindi **circa 4% dell'inventario C-first censito** completato.
Le classi lessicali migrate nei lotti successivi coprono inoltre un gruppo
significativo dei confronti parola-per-parola, ma non sono ancora state
ricontate con una sonda completa. Il valore operativo prudente da ora è dunque
**circa 6% globale**, mentre
`89%` resta soltanto l'indicatore del sotto-percorso messaggi.

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
