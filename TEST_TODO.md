# TEST_TODO — le decisioni aperte della migrazione a `.p0t`

> **⛔ Politica dei test (F., 8 settembre 2026), vale sopra tutto il resto:**
> la suite intera va approvata da F. prima di lanciarla; mentre si lavora
> solo test puntuali e contingenti e `make soft-test` con il budget come
> limite imposto (si supera -> si uccide e si ripensa: lentezza da curare o
> casi da togliere, mai budget da alzare). Rapidita' di progresso > copertura.
> Le sessioni di fix dei test le pianifica F.; durante lo sviluppo di
> abilita' cognitive, la crescita della KB o l'apprendimento (KB viva) i test
> non si fanno. Stessa nota nel Makefile (target `test`) e in CLAUDE.md.

# 🔧 AGGIORNAMENTO gen510, secondo giro (2026-09-11 sera) — leggere prima dell'handoff sotto

Il metodo di questo giro: lotti di test **sotto i 5 minuti**, niente processi
lunghi, e ogni rosso classificato con una **bisezione** sul worktree del commit
`ec7d4b5` (prima della sessione): `git worktree add <dir> ec7d4b5`, `make build`
lì dentro, demone con `rm -f obj/test-engine.sock` prima dell'avvio, e fermato
per numero di processo (`pgrep -x parrot0` + `/proc/PID/cwd`: un `pgrep -f`
che contiene la stringa cercata uccide il comando stesso).

**Chiusi in questo giro**

| Rosso | Causa | Cura |
|---|---|---|
| `multigoal.p0t` «who is the grandparent of ann?» → «Kim.» | `answer_frame($V,$V) :- relation_verb($V)` + la crescita gen509 (`relation_verb(grandparent)`): la cornice generica provava l'entita' nei due versi | `answer_frame_defers/1` (grammar.p0): una relazione DEFINITA si interroga col lettore «R of»; il C la cede |
| `multigoal.p0t` «Learned: parent(tom, bob).» | resa piu' naturale | attesa aggiornata: «Learned: tom is the parent of bob.» (`say_frame_preferred/2`) |
| `multigoal.p0t` «is tom the grandparent of bob?» → «No.» | «No.» senza licenza | attesa aggiornata alla risposta onesta della scala del verdetto |
| `taught_lexicon.p0t:128` | resa piu' naturale | attesa aggiornata |
| `question_does_not_teach.p0t`, `deep_memory.p0t` (turni a 1,13 s) | `construction_frame/3` rienumerato 1194 volte per turno da `p0_frame_is_taught` | l'insieme dei pattern insegnati si raccoglie una volta per chiamata: 707k → 32k passi di solver |

⚠ **Una vista materializzata su `construction_frame/3` NON va dichiarata**:
provata, non accelerava niente (le chiamate avvengono dentro una risoluzione) e
rompeva la lettura di «tom is the parent of bob». Tolta.

**Preesistenti (rossi identici su `ec7d4b5`), da sistemare**: `accentless_copula`
(4), `coref` (1), `compose_coref.it` (1), `prefix_before_assertion` (9),
`string_transform` (1), `taught_cue_ladder` (8: «forget that … is a casual
opener» non letto), `lexicon_it` (1), `literal_forms` (4: «was born in» → «dates
from», e un turno di 9-12 s su «zorak vurbles nivora»), `assisted_construction_ternary`
(turni di 6-9 s: da profilare), `taught_lexicon` 153/157 (il sandbox).

**Terzo e quarto giro (2026-09-11 notte)** — chiusi: NA8 (Guerra fredda), NA9
(il piano raccontato: `module_result_policy(lessonform, terminal)`),
`taught_lexicon:228` (un turno non rivendicato accetta un'offerta solo se la
riguarda), le conseguenze (report §7-ter), e il **limite dello slot dopo
l'articolo** nel lettore delle forme — preesistente, trovato con la traccia
`[form] matched …`. Test aggiornato con motivo: `higher_order_lesson.p0t`
(«encloses»). Da bisecare: `one_act_of_learning` 4/6 (riferimento 5/5; turni di
prosa lunghi fuori tempo). Aperto: una frase di situazione insegnata dentro una
lezione inglese si canonicalizza in inglese («non hai i passi» → «not hai i
passi»), mentre la domanda italiana la canonicalizza in italiano («not hai the
passi»): serve canonicalizzare il frammento nella propria lingua.

**Aperti di questo giro**: la forma canonica del turno si vede ora con
`P0_READ_TRACE=1` (`[canon] «…» -> «…»`), e le cornici di domanda con `[aframe]`
e `[qshape]`. Le domande di conseguenza e il piano di traduzione del muro sono il
prossimo lavoro (report gen509 §7-bis).

# 🌙 HANDOFF — gen508 → gen510, 2026-09-11. RIPRENDERE DA QUI.

> Prevale sui due handoff qui sotto per lo **stato del codice**; il metodo del
> gen506b (turno appeso = rosso con nome, budget duro che non si alza) resta.

## 1. Lo stato in cinque righe

- **gen508** (`faf665f`): le due strutture di `docs/plans/due-strutture-kb-viva.md`
  — la definizione come espressione (`relation_def/2`, `eval_rel/3`) e il fatto
  a ruoli aperti (`occurrence/2`, `role/3`). Stato in quel documento, §0.
- **gen509** (`755e7e5`): addestramento soltanto via `make chat`, 19 fatti veri
  salvati (quattro battaglie, il bronzo) e il report dei limiti
  `docs/reports/2026-09-11-interlocutore-gen509.md`.
- **gen510** (questo commit): le riparazioni dei limiti del report. Il §7 del
  report dice limite per limite che cosa e' riparato, verificato o aperto.
- **Test: NON rifatti dopo le riparazioni.** Girato solo `make soft-test` (§3).
  Un giro mirato su `language/` e `knowledge/` e' stato fatto A META' e con un
  binario intermedio (§4). La suite intera la decide F.

## 2. Che cosa e' cambiato, e che cosa puo' spostare le attese

**C — `src/brain/10-memory-knowledge.c`** (tutto commentato `gen508`–`gen510`):

| # | Modifica | Rischio per i test |
|---|---|---|
| C1 | `p0_turn_form_reader`: il modo del turno consulta `turn_illocution` oltre al «?» | una forma DICHIARATIVA ora salta se il turno e' letto come domanda senza «?»: se un'affermazione viene letta come domanda, una lezione che prima entrava puo' non entrare |
| C2 | slot `expr(N)` / `construct(N)` (gen508); un nome nudo e' UNA parola (gen509) | le superfici #5–#8, #16, #18 leggono espressioni: un operando di piu' parole che non comincia con una costruzione ora fa fallire la forma |
| C3 | `p0_bad_subject`: salta il controllo per parola se l'atomo e' `known_referent/1` | soggetti con «the»/«of» interni, se noti, ora passano |
| C4 | estrattore delle classi: nessun `located_in` se la classe ha una `metalinguistic_head/1` | frasi di prosa «X is a … name/word/term … in Y» non producono piu' il luogo |
| C5 | lettore «X is the R of Y» (~riga 19990): verso dichiarato nell'asserzione a sei parole; domande con soggetto/oggetto di piu' parole; `who/what is the R of X?` ricade su `holds/3`; `is X the R of Y?` usa `holds/3` e poi la scala condivisa | **«No.» diventa «I don't know: nothing I hold says …»** quando non c'e' licenza; «Nobody that I know of» puo' diventare una risposta vera o un «I don't know about R» |
| C6 | `p0_relation_verdict`: la scala del verdetto estratta da `p0_polar_relation` | refactor a comportamento invariato per «does X V Y?»; da verificare |
| C7 | `p0_run_op_named`: `turn_form_empty_reply/2` — una forma di domanda che trova zero righe dice la sua resa invece di cedere | oggi solo `ask_about` la dichiara |

**KB**

| File | Modifica |
|---|---|
| `kb/core/procedures.p0` | gen508: famiglie G come viste di `relation_def/2`, `eval_rel/3`, `norm_expr/2`, `*_form/1`, `same_relation/2`, ruoli e proiezioni; gen510: ponte `holds(R,X,Y)` ↔ `R_of` nel verso dichiarato, `self_pair_allowed/3` + `irreflexive_relation/1`, `about/3` sui ruoli, aiutanti `machinery/1` |
| `kb/core/messages.p0` | superfici #21–#32 (gen508); `ask_abbrev`/`ask_abbrev_expanded` («what does X stand for?»); `teach_irreflexive`/`unteach_irreflexive` («V never holds of itself»); `ask_about` con `rest(subject)` e `about_nothing` |
| `kb/core/grammar.p0` | cornici `@S is the R of @O` da `family_relation/1` e `relation_noun/2`; `known_referent/1`; `metalinguistic_head/1`; `turn_gap_middle` non propone stopword |
| `kb/core/network.p0` | `turn_short_reply/1` + `offer_reply_max_words(3)`: un'offerta si accetta nominandone il tema solo con una risposta breve; stessa condizione su `option_hit_word` |
| `kb/core/intents.p0` | `lexical_relation_request` + `compound_guard(semantic_summary, …)`; `own_plan_question` + guardie — **NON hanno effetto**, vedi §5 |
| `kb/core/lexicon.p0` | marcatori d'inglese per il lessico delle lezioni; «are» nella classe della domanda «R of»; `has_part(W,P) :- part_of(P,W)` |
| `kb/core/social.p0` | `sibling_of` con `dif`: nessuno e' fratello di se' stesso |
| `kb/machinery/question-frames.p0` | `answer_frame("made of", has_part)` |
| `kb/core/world-facts.p0`, `kb/learning/*`, `kb/machinery/*` | la crescita salvata del gen509 |

## 3. Che cosa e' stato verificato

- `make soft-test` dopo tutte le modifiche: vedi il commit (ultimo risultato
  registrato qui sotto, §3a). Prima delle ultime modifiche era verde in 12 s.
- `basics.p0t` [antonym] era ROSSO dal gen507/71 («what is the opposite of
  hot», senza «?», letta come lezione): chiuso da C1.
- Due chat di verifica (non salvate, nessun `.p0t`), gli esiti per limite nel
  §7 del report. Riassunto: NL1, NL4 (lezione), NL6, NA1, NA3, NA5, NA6, NA7,
  NA10, NA11 verdi in chat; NA2, NA8, NA9 ancora aperti.

### 3a. Ultimo soft-test

`make soft-test` dopo TUTTE le modifiche C e KB del gen510: **verde in 13 s**
(budget 15 s) — `health` 2/2, `basics` 5/5, `facts` 9/9.

## 4. Il giro mirato interrotto — che cosa si sa

Lanciato con il binario che aveva C1 e C2 ma NON C3–C7 (e KB cambiata a meta'
giro: `!reset` rilegge i file, quindi i file successivi hanno visto KB piu'
nuova). Si e' fermato su `language/document_claims.p0t`: il client e' stato
ucciso dal `timeout 180` che avevo messo io e il demone e' morto scrivendo su
un socket chiuso (log vuoto). **Non e' un turno appeso registrato**; ma quel
file durava 133 s il 2026-09-08 e va rimisurato (le clausole nuove di
`holds/3`, le cornici derivate e `known_referent` a ogni soggetto costano).

| File | Esito ora | Riferimento 2026-09-08 | Da fare |
|---|---|---|---|
| `language/accentless_copula.p0t` | 5 ok / 4 FAIL | fuori da `make test` | bisezione contro `ec7d4b5` |
| `language/assisted_construction.p0t` | 54 / 12 | LEARN_TODO: 65/1 | **probabile regressione**: bisezione (C1? C2?) |
| `language/assisted_construction_ternary.p0t` | 27 / 6 | LEARN_TODO: 33 ok | **probabile regressione**, come sopra |
| `language/compose_coref.it.p0t` | 3 / 1 | ok | candidata regressione |
| `language/compose_coref.p0t` | 4 / 2 | 4 / 2 | invariato |
| `language/coref.p0t` | 7 / 1 | ok | candidata regressione |
| i primi file di `language/` fino a `document_claims` esclusi | ok | | |
| tutto il resto di `language/` e tutto `knowledge/` | **non eseguito** | | rifare |

## 5. Le attese da rivedere e i difetti aperti, in ordine di leva

1. **`reasoning/multigoal.p0t:22-23`** attende «No.» per «is tom the grandparent
   of bob?» in un contesto amputato (`PARROT0_BASE=`). Con C5 la risposta
   diventa «I don't know: nothing I hold says …»: quel «No.» era senza licenza
   (mantra #7). Decidere: cambiare l'attesa, oppure dare una LICENZA di mondo
   chiuso per le relazioni — il gemello relazionale di `closed_world_answer/2`
   (`epistemic-status.p0`). Qualunque altro test che attenda «No.» da «is X the
   R of Y?» su un fatto non dimostrabile ha la stessa causa.
2. **NA2 aperto**: «is elizabeth ii the grandparent of william?» → ancora
   «No.», mentre «who is the grandparent of william?» → «elizabeth ii.». La
   domanda NON arriva al lettore «R of» riparato: la prende prima un altro
   lettore. Sonda: `make chat`, poi `/debug` e la domanda; il tracciato dice
   quale facolta' la rivendica (candidati dall'agente di ieri:
   `polar_class_answer` ~riga 1207, `p0_polar_reply` ~7441).
3. **NA8 aperto**: «what is the opposite of cold?» → la Guerra fredda.
   `compound_guard(semantic_summary, lexical_relation_request)` non ha effetto:
   verificare come `answer_consumer_guarded` (≈10694–10735) legge le classi di
   guardia, e se `semantic_alias(cold, cold_war)` (`kb/facts/encyclopedia.p0`)
   vada ristretto.
4. **NA9 aperto**: «your plan when you don't have the steps?» → il saggio di
   progettazione. `faculty_yield(analysis_family, open, own_plan_question)` e
   `compound_guard(analysis_plan, own_plan_question)` non hanno effetto:
   capire quale livello di `structured_analysis_lead` risponde (0 o 1) e che
   cosa legge.
5. **NA (photon)**: la definizione ora si SALVA («Held: photon —
   elementary_particle_of_light»), ma «what is a photon?» non la usa e parte
   la ricerca. `means/2` va consultato prima dell'offerta.
6. **Resa del verdetto** per i predicati da nome di relazione: «is napoleon the
   winner of waterloo?» → «nothing I hold says waterloo winner of napoleon»
   (argomenti nel verso del predicato). Onesto ma goffo: passare a
   `p0_relation_verdict` anche la superficie e l'ordine detto.
7. **Rischi di C1 e della soglia di `offer_reply_max_words(3)`**: una risposta
   lunga a un'offerta che ne nomina il tema non fa piu' partire la ricerca.
   E' voluto, ma va guardato nei test di `network`/`gap_dialogue`.
8. **Non verificati**: NL5 (`sibling_of` con `dif` e «never holds of
   itself»), NL8 (`metalinguistic_head`), la ritrattazione delle definizioni
   parametriche.
9. **Aperti, non toccati**: NL2 («the battle of waterloo» — «of» dentro un
   nome; proposta in `docs/reports/…gen509.md` e dall'agente: `np_denotes/3`),
   NL7 (le situazioni di un piano sono chiuse: una sola, `steps_missing`),
   NA4 («did wellington defeat napoleon?» con la relazione `defeated`: forme
   verbali del nome di relazione).

## 6. Come si riprende

```sh
make test-engine
# il giro mirato, file per file, con il tempo di ciascuno (nessun timeout corto:
# un client ucciso a meta' turno fa morire il demone)
for f in tests/p0t/language/*.p0t tests/p0t/knowledge/*.p0t tests/p0t/reasoning/multigoal.p0t; do
  t0=$(date +%s); r=$(./bin/parrot0 --test "$f" 2>&1 | tail -n 1)
  echo "$f | $(( $(date +%s) - t0 ))s | $r"
done
```

Confrontare con `docs/reports/suite-run.txt` (2026-09-08, `0b860d96`). Per
bisecare un rosso senza fermare il demone: `make build BIN=obj/p0next` e il
demone su `obj/next.sock` (vedi l'handoff gen506b qui sotto). I rossi nuovi si
guardano col «got» prima di toccare l'attesa.

# 🏁 HANDOFF — il cane da guardia, 2026-09-08 (`gen506b`)

> Prevale sul gen505y qui sotto per il METODO; le classi di attese invecchiate
> e le regressioni chiuse restano valide.

- **Un turno che non finisce ora e' un rosso con nome**, non una suite muta:
  `src/testeng.c` (`te_turn`) arma `alarm(PARROT0_TE_HARD)` (default 60s, mai
  sotto 2× il `!timeout`) intorno al turno; se scatta il demone risponde
  `FAIL [sezione] riga N — turn HUNG …` con il testo del turno, e si ferma.
  `scripts/suite-run.sh` lo riavvia e annota `# engine stopped on <file>`.
- Quattro turni sopra i 60s trovati sulla suite intera: «prova a ripararti»
  (`self_repair` 26, `autonomous_cycle` 38), le iniziali (`initials` 12),
  «knowledge gap zorb» (`bridge_gap` 31); piu' `arith_guard`, dopo il quale il
  demone e' morto senza turno appeso (crash da riprodurre). Dettagli e numeri
  in `LEARN_TODO.md` §1 del gen506b. **Il budget duro non si alza per farli
  passare**: o si cura il motore o il test non e' un turno.
- Quattro file nati il 7 settembre erano FUORI da `make test` (nessuno li
  lanciava, due erano gia' rossi): `deep_memory`, `question_does_not_teach`,
  `prefix_before_assertion`, `mention` sono ora nel Makefile dopo
  `literal_forms`. Regola: **un `.p0t` nuovo entra nel `make test` nello
  stesso commit in cui nasce.**
- `make soft-test` e' tornato a tre file (F.: e' la porzione piccola, si
  tolgono casi, non si alza il budget).
- gen506c: `conversation/compound_inquiry.p0t` (31 assert) e' il cricchetto
  del turno composto, del testimone e degli involucri; nel `make test`. Per
  misurare un binario candidato mentre la suite gira su `bin/parrot0`:
  `make build BIN=obj/p0next`, demone su `obj/next.sock` (⚠ un socket nello
  scratchpad supera i 108 byte di `sun_path` e `bind` fallisce), e
  `PARROT0_BIN=obj/p0next tests/comprehension-probe/probe.py all`.
- Report: `docs/reports/suite-run.txt` e' la corsa sul binario di HEAD; la
  precedente (binario 5a4b160, 236 ok / 122 FAIL / 4 fermate) e' descritta
  in LEARN_TODO. `analogy.p0t` era rosso da giorni per un'ECO (buffer non
  scritto), non per un'attesa invecchiata: guardare il «got» prima di
  riscrivere l'atteso.

# 📁 HANDOFF — la suite riallineata, 2026-09-07 (`gen505y`)

> **Un solo handoff vivo per file.** Questo prevale su quelli sotto; il blocco
> «⚠ DA SISTEMARE» del gen505q e' chiuso qui (i due gialli erano lo stesso
> caso: risolti in `chitchat.p0t` e `reactions_are_knowledge.p0t`).

## Il metodo, in cinque righe

1. **`scripts/suite-run.sh`** (promosso dallo scratchpad, H.1a): tutta la
   suite, nell'ordine di `make test`, su un demone DEDICATO, senza fail-fast,
   una riga per file. Il demone di `make test-engine` resta libero per
   indagare. I report stanno in `docs/reports/gen505y-suite-*.txt`.
2. **Il differenziale con il gen491** (`docs/reports/gen491-suite-order-run.txt`)
   dice quali file erano verdi allora: e' cio' che separa *«test invecchiato»*
   (R2, si cambia l'attesa) da *«motore regredito»* (si cura il motore, o si
   scrive qui). Vale per i primi 59 file soltanto: gli altri 294 non avevano
   mai avuto una misura.
3. ⛔ **Il demone di test tiene il C con cui e' partito.** `!reset` ricarica la
   KB dal disco, il binario no: il demone del 4 settembre ha fatto sembrare
   morte tre cure giuste dell'handoff gen505w. Sintomo: chat e `--test` non
   concordano sullo stesso turno. `make test-engine` dopo ogni ricompilazione.
4. **Un rosso si guarda prima di cambiarlo** (LEARN_TODO §5): tredici classi di
   attese invecchiate e otto regressioni del motore, sotto, ciascuna con la
   causa.
5. **`scripts/p0t-live-header.py`** converte l'intestazione ermetica alla KB
   intera (R1) di ogni file che si tocca.

## I numeri

| corsa | binario | ok | FAIL | su |
|---|---|---|---|---|
| 1 — `gen505y-suite-run-1-old-binary.txt` | `4ed3fa8`-1 giorno (demone stale), test e KB di allora | 152 | 206 | 358 |
| 2 — `gen505y-suite-run-2.txt` | dopo i primi tre fix C (fuoco, polare, entailment); test e KB letti a ogni file, quindi correnti | 233 | 125 | 358 |

| 3 — `gen505y-suite-run-3.txt` | `a5c833b` (giudizio grammaticale, copula nel soggetto, testimone, «?», guardia per parola) | 235 | 123 | 358 |

⚠ La corsa 3 manca ancora dei fix del pomeriggio (`0c54d24`…`19f044e`: il
prefisso davanti all'affermazione, i coprenti, il glossario italiano, verify
e induce — agent/agent_induce/agent_verify e i tre .it passano da 0 a verdi).
Chi riprende rilancia la corsa sul binario di HEAD e aggiorna la riga.

⚠ La corsa 2 manca degli ultimi fix C (giudizio grammaticale sul codice, il
soggetto con la copula, il testimone del sillogismo, il «?» come cue, la guardia
del soggetto per parola). **La prima cosa da fare e' una corsa 3 sul binario
di HEAD** — `make build && scripts/suite-run.sh docs/reports/gen505y-suite-run-3.txt`
— e aggiornare questa tabella. (Avviata a fine sessione: se il file porta la
riga `# done`, i suoi conteggi sono i numeri di HEAD.)

## Le classi di attese invecchiate, e la convenzione scelta per ciascuna

| classe | file (esempi) | come si asserisce ora |
|---|---|---|
| «Learned: pred(arg).» → prosa «arg is a pred.» (gen491) | 43 file, riscrittura meccanica delle sole righe `<` | la prosa. ⚠ Non ovunque: `alsoclass`, `cause` (binari) restano `pred(arg)`; si guarda, non si generalizza |
| virgolette «» nei template | fewshot, teachverb, translate, reqgen, cue_learn, intent_* | «» |
| rotazione anti-ripetizione (gen55): la seconda voce della famiglia da' la seconda variante | chitchat «rough day», name_is_knowledge, memory, greet_name | si asserisce la famiglia o il nome (`<~ , Bob!`), non la variante |
| il muro ruota fra varianti | parrot, lexicon_it, comprehension.it, polar_meta.it | `<! Imparato` + `!query turn_outcome(current_turn, blind_wall)` |
| dottrina «non dimostrato non e' falso» (gen505x) | facts, facts.it, priority, decompose, taught_lexicon | `<~ not proved is not the same as false` |
| la risposta segue la lingua del turno | taught_rules, memref_arith, glue, gen_read.it, mention | l'italiano |
| il campione e' invecchiato perche' parrot0 ha imparato (R2) | greet «C», chitchat «sky», abduce socrate, blankwall Socrate/Parigi/mare, motorize_class «written», polar_meta.it «ci sei», pragma.it | entita' inventate, o la risposta vera come prova di crescita |
| conteggi e elenchi che crescono con la KB | introspect, self (registro), strategy (dispatch) | la forma o l'apertura, non la fotografia |
| il declino informato e' piu' corto | blankwall, investigation_access, arith_flex | `<~ I don't know about <relazione>` (nomina il termine; ha perso «you can teach me with…») |
| un'induzione si propone, non si asserisce (gen505d) | emerge, induce | la domanda «does that always hold?» |
| gli atomi si presentano con gli spazi | taught_lexicon, teaching_arity | `puppo dog is a mammal` |
| le pagine curate non esistono piu' (gen436, F.) | prosepage, prosepage.it, prose_forms, one_act | il passaggio INLINE `read: …` |
| il piano porta le dipendenze «[needs …]» | planact | l'apertura |

R1 applicata (ermetico → KB intera) a: motorize_class, chitchat, aggregate,
cause, abduce, blankwall, prosepage, prosepage.it. **Con la KB intera
`aggregate` (rosso dal gen491) e `cause` sono passati da soli**: il contesto
amputato era la meta' della diagnosi. Restano **~260** file ermetici; la regola
e' sempre «ogni file che si tocca».

## Le regressioni del motore chiuse in questa sessione

| che cosa | dove | effetto |
|---|---|---|
| la verifica del fuoco rifiutava il PORTATORE dell'attributo («the frame move for X») | `p0_answer_subject_in_focus` | dialogue_moves, sequential_view, context_scope: 32 assert |
| il lettore polare leggeva «how do you know…» come know(you, …) | `p0_polar_relation` + subject_guard | howknow 4/4, syllogism |
| i verdetti di entailment detti dal sandbox senza template | `entailment_status` | entail 14/14 |
| il giudizio grammaticale rivendicava con un rifiuto anche il codice | `p0_grammar_judgement` si ritira sui segmenti `code` | code 14/14, repair 52, codeintent |
| «come faccio AD abbassare» contro il confine di parola (gen505q) | `segment_role(goal, "come faccio ad")` (KB) | sei suite di pianificazione italiana, 107 assert |
| «\n» nei template copiato letterale | `kb_fill_slots` | reqgen 8/8 |
| «my dog is called Rex» → «called called Rex» | il valore comincia dopo il marcatore | entities, mixed |
| `created_by` scambiava autore e opera; la versione italiana usava slot inesistenti | messages.p0, responses.p0 | motorize_class |
| «zorb is a small invented device» → created_by(zorb_is_a_small, device, invented) | il soggetto non contiene una copula | glue |
| il testimone «someone is a fpser» restava in KB | `universal_witness_retract` | syllogism |
| il «?» non era una lettura pubblicata; le quantita' scrivevano da una domanda | `question_mark_cue` (KB) + `mod_quantity` consulta la forza | wordproblem_multi «-2.» → «9.» |
| soggetti di piu' parole con dentro un pronome/copula | `p0_bad_subject` per parola | «more five word test here», «mi dispiace ho sbagliato» → muri |

## ⛔ Le regressioni del motore APERTE, in ordine di leva

Ogni voce ha la sonda con cui si riproduce; nessuna e' stata «chiusa» cambiando
l'attesa. Dove il file resta rosso, il rosso e' la misura.

1. **Il glossario italiano** (LEARN_TODO punto 3, e ora ha sette file dietro).
   `rederive.it`/`compose.it`: «ogni uomo e' mortale» → `mortale(X) :- man(X)`,
   `amico(X) :- bear(X), happy(X)` — predicati mezzo tradotti. `agent*.it`
   («raddoppia», «continua» non glossati), `run_execute.it` («esegui» senza
   contratto), `compose_social.it` (risposta inglese a turno italiano),
   `polar_meta.it` «mi senti?».
2. **Turni rubati dal registro affettivo/fatico** (mantra #21): «how do you
   play poker» → smalltalk (games); «lol are you a bot» → chitchat
   (social_reaction); «racecar» → saluto (symbolic); «hi, i'm vera» →
   chitchat (compose_coref); «cos'e' un numero perfetto» → l'ASSENSO
   («perfetto») (research.it); «what did you tell me about milan» →
   smalltalk perche' `discourse` non rivendica (discourse_recall).
3. **Chi risponde a domande meta/analitiche**: «why might X be Y» risponde
   sull'ULTIMO obiettivo (abduce 57); «how would you solve it» → compose (self
   49); «what is the difference between a cause and an enabling condition» →
   lettore polare (meta_reasoning); «are you parrot0?» → «I only read text»
   (polar_meta, mantra #7); «rome is to italy as berlin is to what?» →
   **«paris.»** (analogy 35, mantra #7: un misclaim secco).
4. **L'offerta di ricerca al posto della conoscenza che c'e'**: `means/2`
   asserito e «tell me about zorb» → offerta (glue 26); il concetto insegnato
   chiesto al plurale (inflected_lookup 48) o in italiano
   (foundational_concepts 60-67); «colors that identify zorvian»
   (faceted_enumeration).
5. **Fatti falsi ancora scritti**: «pretend you are a dog named rex» →
   `dog(pretend_you)` (role 18: «you» non e' in subject_guard, lezione gen489
   su entity_pronoun — serve una classe «deittico» distinta); «my aunt lives
   in Paris» → habitat «aunt live in paris» (family 25: il possessivo va al
   registro familiare, mantra #14); «ponder make widget» (issue1 35).
6. **La lettura della prosa**: «organisms, such as most plants, algae and
   cyanobacteria» produce organism/1 solo LETTA (`read:`), non DETTA
   (one_act 27); «carbohydrates like sugars, …» non produce carbohydrate/1
   (prose_forms, one_act); la prosa illeggibile non lascia piu' una
   `machinery_gap` (prose_forms 65-72); il resoconto del lettore inline non e'
   localizzato (prosepage.it, asserito come corrente).
7. **La conferma di un'induzione non e' agganciata**: «does that always hold?»
   → «yes» → «Got it — what would you like to do?», la regola non entra
   (induce 32/35, rossi apposta).
8. **Tempi** (H.1c: si profila, non si alza): `autonomous_cycle` 38 e `self_repair` («prova a ripararti») → **108 s** e il ponte non nasce; `initials` 12 → **82 s**;
   `literal_forms` 246 → 7,7 s; `reader` 21 → 1,3 s; `syllogism` 36 → 2,1 s;
   `contextual_denotation` 30 → 1,6 s; il turno di 30 parole →
   `input_frame_observe` 2,7 s (LEARN_TODO).
9. **Il resto, gia' diagnosticato**: orchain (nome della funzione letto come
   «calls»); codeast (dicitura «I read X into structure» e `unsafe_path` sulle
   directory: da decidere se politica o difetto); codeintent 29 (il lettore di
   clausole rivendica una firma C); run_execute (contratti); repair (l'oracolo
   riporta `build_failed`: ambiente o difetto, mai deciso dal gen491);
   agent_induce/agent_verify (l'induzione di regole numeriche mura);
   register_realization (il registro insegnato e' ignorato: «Shiny» per
   «Glimmer»); wordproblem_multi 27/36/45 (tre calcoli sbagliati);
   universal-input 220 (`ambiguous_input` perso); meta_question 32 («where did
   i say the meeting is» → «where is the meeting is»); count_readings 49-51
   (il registro tecnico); self_repair, autonomous_cycle (non esaminati);
   taught_lexicon 153 (il sandbox non legge «puppo cat is a mammal»);
   explain_more 21 («The arith module can .»); teachverb 28 e intent_reply 21
   (la risposta insegnata «ehila luca»/«Welcome, Bob!» non viene usata);
   conditional_plan 62/74 (il ramo risponde anche senza la cue);
   same 23/36 («no» minuscolo via `bench_dispatch`); blankwall 38 (il declino
   informato sul «why» e' perso); forget_move (LEARN_TODO P4.3);
   faceted_enumeration, foundational_concepts, gap_dialogue, games (rossi gia'
   al gen491).

## Pulizia

Le tredici suite shell gia' convertite erano gia' cancellate; il target
`legacy-test` le chiamava ancora ed e' stato ripulito. Alla radice di `tests/`
non resta nessuno script.

---

# 📁 HANDOFF archiviato — `cefr-bench`, 2026-09-06 (`gen505q`)

> Nuovo banco, chiuso e pubblicato. **Niente in sospeso**: albero pulito, tutto
> su `origin/main`, nessun servizio avviato che resti acceso.

## Che cos'e', in tre righe

`make cefr-bench` misura **che cosa parrot0 sa fare, per livello CEFR**, su
frasi inglesi annotate da professionisti dell'insegnamento. Il risultato e' una
**curva per banda**, non un voto — e la curva e' anche il curriculum: si insegna
in ordine di livello e si guarda dove si muove.

```bash
make cefr-bench                                  # inglese, 25 frasi per banda
make cefr-bench CEFR_ARGS="--lang both"          # + specchio italiano
make cefr-bench CEFR_ARGS="--per-band 40 --split test,dev,train"
make cefr-fetch-score                            # porzione NC, non versionata
```

## ⛔ Prima di toccare i dati: non sono nostri

**CEFR-SP** — Yuki Arase, Satoru Uchida, Tomoyuki Kajiwara, **EMNLP 2022**
(<https://aclanthology.org/2022.emnlp-main.416>). Attribuzione, citazione e
licenze per porzione: **`tests/cefr/ATTRIBUTION.md`**, da leggere prima di usare
o ridistribuire.

| porzione | licenza | nel repo |
|---|---|---|
| Wiki-Auto, 7.453 frasi | CC BY-SA 3.0 | ✅ `tests/cefr/data/en/`, immutata |
| SCoRE, 2.551 | CC BY-NC-SA 4.0 | ⛔ fuori — `make cefr-fetch-score` |
| Newsela-Auto | licenza Newsela | ⛔ non distribuita dagli autori |

**Perche' SCoRE e' fuori:** la clausola NonCommercial si trasmette a chi riceve
il repository, e chi usasse parrot0 in un contesto commerciale dovrebbe
rimuoverla. Imporlo in silenzio a chi clona non sarebbe corretto, e il bench
funziona senza. ⚠ Se qualcuno decide diversamente, e' una scelta di F., non una
svista da correggere.

Le «17k frasi» del paper includono Newsela: **senza, le disponibili sono 10.004.**

## Che cosa misura — e la trappola da non ripetere

⛔ **Non** misura «quanto bene parrot0 parla inglese». CEFR-SP annota la
**difficolta'** di una frase, non la correttezza: ricavarne un voto di competenza
e' un errore di categoria, e il bench lo dice a schermo apposta.

✅ Misura, stratificate per banda:

- **lettura** — la frase produce una risposta o un muro;
- **giudizio** — decisa da una regola / rifiuto onesto / muro / **fuori tema**.

⚠ **La colonna «lettura» e' ottimista**: conta come letta ogni risposta che non
sia un muro, quindi include le fuori tema. Va letta accanto alla colonna «fuori
tema», che e' il debito vero — *una risposta che non risponde e' peggio di un
muro.*

## La misura che ha cambiato il disegno

**I due annotatori concordano nel 41,4% dei casi.** Il livello di una frase e'
quindi una **banda** `[min(A,B), max(A,B)]`, non un punto: il bench stratifica
sulla banda, e le frasi su cui gli esperti non concordano non contano come
fallimento di nessuno. Chi riprende non trasformi la banda in un punto per avere
numeri piu' belli.

## Stato misurato al `gen505q` (12 frasi per banda)

```text
banda  lette   decise  rifiutate  fuori tema
A1      17%      1         8          3
A2      33%      5         7          0
B1      50%      7         5          0
B2      33%      7         5          0
C1      75%      7         5          0
C2      83%      7         5          0
```

⚠ **La curva sale col livello, ed e' un reperto sul CORPUS, non sul motore:** le
frasi A1 di Wiki-Auto sono spesso frammenti di titolo («2001 Heisei Ultraman Side
Stories»), non frasi. Chi riprende non lo legga come «parrot0 capisce meglio le
frasi difficili».

## Lo specchio italiano — e la nota che F. ha chiesto di scrivere

48 frasi tradotte in sessione, campionate in modo deterministico
(`tests/cefr/data/it/`, con `PROVENANCE.md`).

> **Il punteggio italiano NON e' veritiero, ed e' comunque utile.** Le frasi sono
> tradotte e le etichette **ereditate**: la banda dice «in inglese era di livello
> X», non «in italiano e' di livello X» — **il livello CEFR non sopravvive alla
> traduzione**. Serve come **indicatore di scostamento** a parita' di frase:
> quanto cade la capacita' cambiando lingua. Il valore di **riferimento**, l'unico
> citabile come misura, resta quello **inglese**.

Presentare quelle etichette come annotazione italiana sarebbe scorretto due
volte: verso chi legge il bench, e verso gli autori di CEFR-SP, a cui
attribuirebbe un lavoro che non hanno fatto.

**Risultato italiano al `gen505q`: 0 decise, 30 muri, 18 fuori tema.** Non e' un
crollo del giudizio: la cue italiana **non arriva** — e' la canonicalizzazione
ibrida gia' registrata in `C_TODO` §U4. Il bench la misura, non la causa.

## Che cosa ha gia' prodotto

- **un difetto vero nelle prime dodici frasi**: «There are four games in the
  series» dichiarata sbagliata perche' «there» sembrava singolare. E' il
  **soggetto esistenziale**, dove il verbo concorda con cio' che segue. Curato
  come conoscenza (`expletive_subject/1`), non togliendo la regola. E' il ciclo
  per cui il banco esiste: **bench → lacuna → conoscenza → bench**;
- **due difetti del bench stesso**, corretti: il classificatore conosceva solo le
  rese inglesi — il giro italiano riportava 48 «fuori tema» che erano 48 **muri**,
  cioe' accusava il motore della colpa sbagliata — e un muro non e' una risposta
  fuori tema, ora sono classi distinte.

## Da dove riprendere

1. **la cue italiana** (§U4): sbloccarla fa passare la colonna italiana da
   «misura la canonicalizzazione» a «misura il giudizio»;
2. **le regole grammaticali**: oggi ne esiste **una** (accordo soggetto-verbo). I
   «rifiuti onesti» del bench sono la lista della spesa — ogni regola nuova li
   converte in «decise», e la curva si muove;
3. **la colonna lettura**: separare «risposta pertinente» da «risposta
   qualunque» richiede la lettura di pertinenza, che e' la stessa che manca ai
   turni rubati (`turn-arbitration.md`, S4 copertura);
4. **allargare lo specchio italiano** solo con provenienza dichiarata riga per
   riga: 48 frasi tradotte e dichiarate valgono piu' di 7.453 tradotte a macchina
   e non verificate.

---


Coda delle cose che **non decido da solo** e di quelle che restano da fare nella
migrazione delle suite shell verso il test-engine
([`docs/plans/test-engine.md`](docs/plans/test-engine.md)).

Stato: **113/113 file sistemati (100%)** alla radice di `tests/`.

---

# ⛔ HANDOFF — 2026-09-03, gen491. LEGGERE PRIMA DI RIPRENDERE.

> Sessione interrotta perché F. doveva andare. Qui c'è dove siamo arrivati, che
> cosa è già verde, e **l'ordine esatto** in cui continuare.

## H.1 — Il metodo, prima dei numeri (non ripetere i miei due errori)

**(a) Non si misura un rosso girando i file isolati.** `make test` li manda
**in ordine, su UN SOLO demone**, e alcuni dipendono dallo stato dei precedenti.
Il mio primo sweep, alfabetico e isolato, dava il 46% di rossi: accusava il
motore di difetti che non ha. Lo strumento giusto è in
`scripts/`-style ma vive ancora in scratchpad — **va promosso**: legge l'ordine
dal target `test:` del Makefile e non si ferma al primo rosso (`make test` è
fail-fast, quindi mostra un rosso e nasconde gli altri).

**(b) Ogni rosso va confrontato col commit di partenza.** Un worktree
(`git worktree add <dir> <sha>`) sullo stesso file è l'unico modo onesto per
separare *«l'ho rotto io»* da *«era già rosso»*. Su 17 rossi: **3 miei, 14
preesistenti, di cui 5 migliorati** durante la sessione. Le due corse sono in
`docs/reports/gen491-suite-order-run.txt` e `gen491-red-at-base.txt`.

**(c) ⛔ Non assecondare i tempi lunghi con timeout più grandi.** È la
correzione che F. ha dato a metà sessione: *«non mi piace che lavori con questi
timeout che ti chiami da solo; un timeout di 10 secondi è già un sintomo, anche
il modo come indaghiamo»*. Un turno lento **si profila** (`/debug`), non si
aspetta. Il `!timeout` è ammesso **solo quando la causa è nota e nominata** e la
misura è scritta accanto — F. l'ha confermato per il caso della vista
invalidata.

## H.2 — Che cosa è già chiuso

| | |
|---|---|
| `health.p0t` | la suite **non partiva più** (60% di rossi sul primo turno): firma della vista che contava i fatti congelati di un'altra vista. 891 ms → **131 ms** |
| `literal_forms.p0t` | 49/0 → 38/11 → **49/0**: la vista troncava a 256 schemi su 359, in silenzio |
| `contractions.p0t` | **8/0**, e primo file convertito da ermetico a KB piena (R1) |
| `TEST_TODO §0.0` + `docs/plans/test-engine.md` | le due regole R1/R2 di F., scritte dove si vengono a cercare |

## H.3 — ⛔ L'ORDINE IN CUI CONTINUARE

**1. Finire la corsa completa della suite.** La mia si è fermata al file 59
(`meta/self_repair.p0t`) perché quel file contiene un turno da **135 secondi**
(`prova a ripararti`: fino a 240 replay completi del turno, ~340 ms l'uno). Non
è un blocco e **non è una regressione** — è così anche al commit di partenza, e
i miei `timeout 90` lo troncavano facendolo *sembrare* un blocco, con 281
«cannot reach engine» a valle. Quindi: **i 17 rossi noti sono su 59 file, non su
353. I restanti 294 non sono mai stati misurati.** È la prima cosa da fare.

**2. I tre rossi rimasti fra quelli noti**, in ordine di chiarezza:
   - **`mcp/aggregate.p0t`** (4/0 → 2/2, **regressione mia, non diagnosticata**):
     «chi ha vinto di più» risponde `M1.` invece di `spain`. Il file usa
     `PARROT0_PROFILE=` vuota e `WORLD_FACTS=0`: **va prima convertito (R1)**,
     perché metà della diagnosi potrebbe essere il contesto amputato.
   - **`conversation/forget_move.p0t`** (3/3, preesistente): il messaggio di
     `forget` è *formattato e mai emesso* — `answerframe` ruba il turno. È
     `LEARN_TODO` P4.3, ed è mantra #17 puro: si chiude con un `faculty_yield`,
     non con una riga di C.
   - **`conversation/greet.p0t`** (7/1, preesistente ma è **R2 da manuale**):
     `tell me about C` asseriva ignoranza, e il corpus del gen490 ha insegnato
     il carbonio. Va spostato il confine, non rimessa l'ignoranza. ⚠ Nella
     risposta c'è anche un difetto vero da annotare: *«c is a chess_file»*
     appende trivia di scacchi a una definizione di chimica — collisione di
     classe che il mantra #14 vuole chiusa con una guardia teachable.

**3. Gli 11 preesistenti già identificati** (dettaglio in
`docs/reports/gen491-red-at-base.txt`): `motorize_class` (2, forma del messaggio
migrata in KB — probabile R2), `repair` (9, l'oracolo riporta `build_failed`:
verificare se è ambiente o difetto), `check_sort` (5, `expected source: turn, got
source: mcp` — è la migrazione `!expect` del §0.1 di questo file, non un difetto
del motore), `games`, `faceted_enumeration`, `foundational_concepts`,
`gap_dialogue`, `class_conflict`, `name_is_knowledge`,
`reactions_are_knowledge`, `gap_is_a_fact`, `gap_anchor`.

**4. La ricostruzione INCREMENTALE della vista materializzata.** Oggi insegnare
un verbo di relazione invalida `extract_frame` e la ricostruisce **intera**:
1406 ms contro 224 di regime, 715.931 passi. È l'unico `!timeout` che ho dovuto
dichiarare, ed è il candidato numero uno del §L.

**5. La conversione R1, 288 file.** Non si fa in campagna: **si converte ogni
file che si tocca**. Chi arriva a zero cancella il paragrafo in §0.0.

---

## ⛔ 0.0 LE DUE REGOLE CHE NON SI DISCUTONO (F., 2026-09-03)

Vengono prima di ogni altra voce di questo file, e prima di scrivere o
correggere qualunque `.p0t`.

### R1. Il contesto ermetico NON ESISTE PIÙ

> F.: *«non esiste più e non accettiamo uso di contesto ermetico: la KB è parte
> del progetto e non può essere spenta durante i test né frazionata».*

Sono vietati, in ogni file nuovo e in ogni file che si tocca:

```
[mock hermetic]
!set PARROT0_BASE=          ← una KB di sole regole, senza conoscenza
!set PARROT0_WORLD_FACTS=0  ← il mondo spento
```

**Il perché, e non è una preferenza di stile.** La KB non è un volume montato
sotto parrot0: *è* parrot0. Un test che la spegne non misura parrot0 con meno
rumore — misura **un altro sistema**, che non esiste e a cui nessuno parlerà mai.
E il costo si è già visto due volte: `frontier_chat_audit.it` misurava una KB
amputata e dava 31 rossi su 56 **per costruzione** (`830bc59`), e il gen459 ha
perso dieci turni su un difetto che in KB piena non c'era.

**Si testa sempre con la KB al massimo.** Se serve forzare una non-conoscenza,
si spegne *quella cosa lì* con gli strumenti che il framework già offre —
`!forget`, e la si rimette dopo — oppure si usa un'entità nuova che nessuno può
conoscere (`zorbles`, `puppo`, `nivora`). Mai spegnere il mondo per far tacere
una frase.

**Debito misurato al 2026-09-03: 288 file su 441 usano `[mock hermetic]`** (268
con `PARROT0_BASE=` vuota, 291 con `WORLD_FACTS=0`), cioè il 65% della suite. È
troppo per una sessione: la regola è **si converte ogni file che si tocca**, e
nessun file nuovo lo usa. Chi finisce la coda cancella questo paragrafo.

### R2. Un'ignoranza resa falsa dalla KB si chiude cambiando il TEST

> F.: *«le ignoranze rese false vanno colmate cambiando il test, man mano che la
> KB cresce, essendo essa stessa parte del progetto. I test perdono di
> significato e vanno ripensati per individuare un confine nuovo di
> testabilità».*

Un test che asserisce *«di questo parrot0 non sa niente»* ha una scadenza: il
giorno in cui glielo si insegna, quel rosso **non è una regressione, è una
crescita**. Esempio reale di oggi — `greet.p0t`:

```
> tell me about C
< I don't understand that yet.        ← vero fino al gen489
                                      ← falso dal gen490: C è il carbonio
```

La riparazione **non è** rimettere parrot0 nell'ignoranza: è chiedersi *qual è
adesso il confine della testabilità*, e spostare il test lì — con un'entità che
nessuno può conoscere, o forzando l'oblio di quella specifica cosa. L'intento del
caso («ciò che non è riconosciuto riceve un non-capisco onesto») si conserva; il
campione con cui lo si prova cambia, perché il campione è invecchiato.

⚠ **Questo NON autorizza a cambiare un'attesa per far passare un rosso.** Resta
la regola del §5 di `LEARN_TODO.md`: *prima si capisce chi ha torto fra il test e
il codice*. R2 vale solo quando ciò che il test asseriva **è diventato falso
perché parrot0 ha imparato** — e in quel caso il commit deve dirlo.

### R3. Un cricchetto che INSEGNA deve ritirare ciò che insegna

> Scoperta al gen492, e il costo era già stato pagato senza accorgersene.

`universal_code_ir.p0t` provava il canale #1 della Gerarchia di Crescita:
*prima* la parafrasi non funziona, si insegna, *dopo* funziona. Ma non ritirava
la forma insegnata, e il file conteneva anche un `kb.save`. Risultato: il
save-map ha instradato la frase inventata dal test —
`answer_frame("observation how from", code_definition_evidence)` — **dentro la
KB curata** (`kb/core/discourse.p0`), dove è stata committata.

Da quel momento il «prima» del caso era falso **per sempre**, e il file passava
solo su un albero che non l'aveva mai eseguito. Il rosso sembrava una
regressione del motore: era il test che aveva sporcato il progetto.

**La regola:** un ratchet che insegna **si chiude ritirando** (`!forget` di
tutti i fatti che ha creato), e nessun ratchet chiama `kb.save` su una KB in cui
ha appena insegnato qualcosa. Il residuo trovato è stato rimosso da
`discourse.p0`; il file ora è **idempotente** — passa 61/61 anche alla seconda
corsa sullo stesso demone, che è il vero criterio.

**Come si riconosce il sintomo:** un `.p0t` che passa la prima volta e fallisce
la seconda sullo stesso demone *sta insegnando senza ritirare*. È diverso da un
timeout, e diverso da una regressione: si controlla eseguendolo due volte prima
di accusare il motore.

---

## 0. HANDOFF — da leggere per primo

### 0.1 `!expect` — engine implementato, conversione suite in corso

`!mcp` ed `!exec` mettevano il loro risultato **dove va la risposta di parrot0**, e
si verificano con `<~` / `<!`. **È sbagliato**, e l'ha detto F.: `<` è
l'asserzione su *ciò che parrot0 ha risposto in un turno di conversazione*, e
l'uscita di una primitiva di test non è parrot0 che parla. Confonderle fa passare
per parola dell'agente quella che è uscita dello strumento.

**La forma decisa (F.): `!expect <sorgente> <testo>`.**

```
!mcp kb.query {"pred":"dog","args":["rex"]}
!expect mcp "provable":true

!exec make build
!expect exec exit 0
```

Due proprietà la rendono molto più di un `<~` rinominato, e vanno implementate
entrambe o non serve a niente:

1. **La sorgente è una GUARDIA, non decorazione.** Se l'ultima primitiva è stata
   `!exec` e il test scrive `!expect mcp`, il caso deve **fallire**. È un errore
   del test — la stessa classe di errore che questa migrazione ha prodotto più
   volte: asserire la cosa giusta sull'output sbagliato. Senza questo controllo
   `!expect` è solo `<~` con una parola in più.
2. **`<` deve RIFIUTARSI di asserire dopo una primitiva.** Se l'ultimo output non
   viene da un turno, `<` deve dire «questo non è parrot0 che parla». Altrimenti
   la forma vecchia continua a funzionare in silenzio e il debito resta soltanto
   scoraggiato, mai chiuso.

Con entrambe, la coppia diventa una **tipizzazione**: `<` è il canale
conversazionale, `!expect <sorgente>` quello degli strumenti, e mescolarli è un
errore *rilevato* invece che una svista.

Sono implementate le tre varianti che `<` ha già: contiene (`!expect`), non contiene
(`!expect!`), uguale esatto — e la sorgente da riconoscere è almeno `mcp` ed
`exec`.

Restano da aggiornare i file già convertiti che usano `<~` dopo `!mcp`:

`p0t/mcp/*.p0t` (6), `p0t/engine/naf.p0t`,
`p0t/engine/dif.p0t`, `p0t/engine/dollarvar.p0t`, `p0t/lang/*.p0t` (3),
`p0t/code/check_sort.p0t`, `p0t/input/universal-input.p0t`,
`p0t/tools/toolexec.p0t` (solo la parte `!mcp`), `p0t/save/savemap.p0t`.

Va fatto **prima** di convertire altro, o il debito cresce con ogni file nuovo.

### 0.2 Come si riprende

1. `bash <(echo 'ls -1 tests/*.sh tests/*.py 2>/dev/null | wc -l')` dice quanti
   file restano alla radice (erano 113 all'inizio).
2. Si sceglie un file dalla §2, si legge la sua forma di asserzione, si converte
   **a mano**, e si verifica con il criterio del §3: *stesso risultato dello
   script, verde o rosso che sia*.
3. Si cancella lo script, si toglie la sua riga dal `Makefile`, si aggiunge il
   `.p0t` a `make test` **solo se è verde**.

### 0.3 Che cosa è già stato fatto

- **`tests/probes/`** (12), **`tests/bench/`** (22), **`tests/tools/`** (12),
  **`tests/cdriver/`** (2) — spostamenti, non conversioni.
- **31 suite convertite** in `tests/p0t/{engine,mcp,expert,growth,lang,code,input,tools,proof,repair,save,oracle}/`.
- **Primitive nuove nel test-engine**: `!mcp`, `!sandbox`, `!symlink`, `!exec`,
  `!fileexists`/`!filemissing`, `!direxists`/`!dirmissing`,
  `!filehas`/`!filelacks`, `!fileclean`.
- **Riparazioni al motore trovate convertendo**: `mcp_tool_invoke` non
  raggiungeva `input.segment`; la risposta del test-engine era di 4 KB e
  troncava i payload; `!sandbox` rompeva il caricamento della KB (percorsi
  relativi); `PARROT0_ORACLE` era letto con `getenv` invece che con `p0env`,
  quindi il test-engine non poteva accenderlo.

### 0.4 La regola che F. ha posto, e che vale su tutto

**Mai più test con una KB vuota o vergine.** La KB è parte di ciò che si testa;
le variabili che la ri-basano per farla apparire vuota perdono di significato.
Nei test nuovi si usano **entità inventate** sulla KB reale, non l'amputazione.

Le conversioni fatte prima di questa regola ricopiano l'amputazione dagli script
(`!set PARROT0_BASE=`, `PARROT0_PROFILE=`, `PARROT0_WORLD_FACTS=0`) e **vanno
ripassate**. `p0t/oracle/posix.p0t` è il primo fatto con il criterio giusto — e
convertendolo è saltato fuori che `kb/experts/programming/bash.p0` è un
**duplicato orfano** di `shell.p0`, incluso da nessuno: il test passava solo
perché lo montava a mano.

---

## 1. Decisioni che aspettano una risposta

### 1.1 Esporre l'esecutore su MCP? — *bloccante per 2 file*

`tests/cdriver/exec_kernel.sh` e `tests/cdriver/exec_dirfd.sh` restano script
perché provano `p0_exec` e `p0_exec_at` **direttamente**: timeout tipizzato,
uccisione del gruppo di processi senza orfani, `spawn_failed`, cwd fuori dal
workspace, e per il secondo l'ancoraggio a un descrittore di directory.

Per convertirli servirebbe uno strumento MCP che esegue argv arbitrari — e per
`p0_exec_at` che accetta anche un **fd di directory**. È una decisione di
sicurezza: darebbe quel permesso a *qualunque* client MCP, non solo ai test.

Il precedente opposto è già preso: `code.check_sort` è esposto perché è un
**giudice** su un sorgente — dispone un candidato, non esegue ciò che gli si
chiede.

- **se sì** → due strumenti nuovi, i due file diventano `.p0t`, `tests/cdriver/`
  sparisce;
- **se no** → restano dove sono, e la categoria è definitiva.

### 1.2 `learnbuild`: allineato; debito residuo documentato

Lo **stesso identico prompt** riceve due risposte diverse:

| | risposta a `write a vunder function` |
|---|---|
| binario | «I can only synthesize and VERIFY the sum, product, or difference of two integers so far — I will not emit code I cannot check.» |
| test-engine | «I understood the request … but I don't have a verified schema for that artifact yet; I only synthesize what an oracle can check (a sort from a learned shape, arithmetic composition, a count-to-threshold game).» |

Riprodotto su un demone appena avviato, stesso env, senza sandbox. Sono due
declini entrambi onesti, ma **non sono la stessa frase**: qualcosa fa vincere un
modulo diverso sotto `--test-engine`.

Il disallineamento era nel boot del test-engine: il demone non partiva con il
profilo AGI e quindi il test-engine selezionava un modulo diverso dal processo
legacy. Ora il demone usa gli stessi default (`PARROT0_TOOLS=1`, sessione vuota,
profilo `kb/profiles/agi.p0`) e `learnbuild.p0t` passa insieme al test legacy.

Debito futuro: il test-engine deve poter esprimere nonce generati e verifiche di
induzione/build senza dipendere da fixture statiche; per ora il `.p0t` conserva
il caso held-out con `vunder` e il controllo meccanico del profilo.

### 1.3 `autolearn` legge file che non esistono più

`tests/tools/autolearn.py` e `scripts/learn.py` leggono `kb/learning/sources.tsv`,
`state.json`, `index.json` e scrivono in `logs/` — tutti rimossi quando
`kb/learning/` è stato ridotto ai soli `.p0`. Il target `make autolearn` passa
ledger e skip-list, anch'essi rimossi.

Quei due flussi sono **rotti**. Vanno rifatti o ritirati; il contenuto è
recuperabile da `git log --all`.

### 1.4 Le suite convertite ma ROSSE non entrano in `make test`

`make test` è fail-fast: una suite rossa fermerebbe tutte le successive. Queste
sono convertite fedelmente (stesso risultato dello script) ma restano fuori:

| file | stato |
|---|---|
| `p0t/expert/grammar.p0t` | 13 passed, 1 failed |
| `p0t/expert/knowledge.p0t` | 9 passed, 13 failed |
| `p0t/expert/profiles.p0t` | 9 passed, 4 failed |
| `p0t/expert/synth.p0t` | 6 passed, 1 failed |
| `p0t/expert/skills.p0t` | 5 passed, 1 failed |
| `p0t/lang/article.p0t` | 4 passed, 3 failed |
| `p0t/lang/adjagree.p0t` | 3 passed, 2 failed |
| `p0t/lang/vmorph.p0t` | 4 passed, 1 failed |
| `p0t/tools/toolexec.p0t` | 21 passed, 1 failed |

Serve una casa: un target `make test-red` che li esegue senza fermare la build,
oppure la scelta di chiuderne il debito prima di cablarli. Molti di questi
fallimenti sono **M0** — vedi `LEARN_TODO.md` P0.4.

### 1.5 `savemap` chiede due primitive nuove per un solo test

`tests/savemap.sh` prova il contratto centrale del save-map — *un fatto appreso
finisce accanto ai suoi simili* — e per farlo costruisce un albero `.p0` finto,
lo carica come `PARROT0_BASE`/`PARROT0_KB_ROOT`, salva, e poi **verifica in quale
file** ogni fatto è atterrato.

In `.p0t` mancano due cose, entrambe di filesystem:

- **scrivere un file di fixture** (l'albero finto, con i suoi `include`);
- **asserire il contenuto di un file** — qualcosa come `!filehas PATH pattern`.

Non le aggiungo da solo: sono due primitive di DSL per una suite sola, e la
seconda apre la porta a test che guardano il disco invece del comportamento.
L'alternativa — far scrivere il `/save` nell'albero VERO durante i test — è
peggio.

Il contratto però è importante, ed è quello che ho esercitato a mano tutta la
sessione con lo sparpagliamento della ricaduta.

### 1.6 I prompt di `basic-chat` restano in un file di piano

`tests/bench/basicchat.sh` misura la copertura leggendo i prompt da
`docs/plans/basic-chat.md`. Il TODO nella sua testa dice di portare in `.p0t` i
prompt rappresentabili e lasciare lì solo l'adapter di misura.

È un lavoro a sé, e si somma bene alla collezione `docs/llmscores/`.

---

## 2. Da migrare, senza decisioni aperte

Non restano file `.sh` alla radice di `tests/`. I runner shell sotto
`tests/cdriver/` sono invece l'adapter generico per driver C diretti e non fanno
parte della migrazione conversazionale.

### 2.1 Conversione automatica tentata e SCARTATA (storico)

Generate e buttate perché non riproducevano l'originale — vanno rifatte a mano:

Le conversioni automatiche storiche furono scartate perché perdevano asserzioni;
le suite interessate sono state poi convertite manualmente.

### 2.2 Ibride: conversazione + driver C

I casi ibridi mantengono la parte conversazionale in `.p0t` e i controlli API
diretti in `tests/cdriver/integration/`.

### 2.3 Grosse, da leggere prima di toccarle

Il ciclo candidato → oracolo → policy → commit è coperto dai driver C dedicati.

### 2.4 Il resto

Le restanti voci storiche sono state assorbite nelle suite `.p0t` o nei driver
API dedicati.

---

## 3. Il metodo, che vale per ogni voce qui

1. **L'equivalenza, non il verde.** Il criterio non è «il `.p0t` passa» ma «il
   `.p0t` dà lo stesso risultato dello script, verde o rosso che sia». Metà di
   queste suite era già rossa, e un file che diventa verde convertendolo ha
   perso delle asserzioni.
2. **Non si perdono i casi che falliscono.** Due volte la mia conversione ha
   omesso proprio quelli, risultando verde. Rimetterli fa tornare il file rosso,
   ed è giusto: *un test che lascia fuori il caso che fallisce è un test che
   mente*.
3. **Le negazioni si guardano.** `! lN id | grep -q 'x'` è un'asserzione di
   **assenza**: tradotta come presenza, il test è verde e sbagliato.
4. **Gli id si ripetono fra blocchi.** Ogni blocco ha il suo `l1`/`l2`; cercare
   «id 2» su tutto il file raccoglie asserzioni di blocchi diversi.
5. **Le asserzioni guardano spesso il formato di TRASPORTO.** Il payload viaggia
   come stringa JSON dentro l'envelope JSON-RPC, con le virgolette protette;
   `!mcp` restituisce il payload grezzo. I backslash vanno tolti.
6. **Lo script cancella, non archivia.** Uno script che resta accanto alla sua
   conversione è un doppione che fa crescere male la suite.

---

## §I. `!cwd` — lavorare in un'altra directory, e il danno che ha evitato

**Aggiunto gen503**, su richiesta di F.: *«implementa un'utility per i test !pwd
che sposta il percorso su una cartella di riferimento»* → *«forse è meglio
chiamarlo !cwd»*.

```text
!cwd DIR       lavora in DIR (relativa alla directory di partenza)
!cwd off       torna a casa   (e si torna comunque a fine file)
```

### Perché serviva

Alcuni casi **sono** una proprietà della directory — «il progetto non compila»,
«questo albero contiene X» — e il demone del test-engine vive nella radice del
repository, dove `make` riesce. Senza `!cwd` quei casi finivano fuori dalla
suite, in script a parte. `tests/p0t/code/repair_broken_build.p0t` è il primo che
rientra.

### ⛔ E il danno reale che ha evitato

`!exec` gira nel processo del demone, cioè **nella radice del repository**. Un
`.p0t` che scriveva e poi cancellava `Makefile` credendo di essere in una
sandbox **ha cancellato il Makefile di questo repository** (ripristinato da git).
Chi scrive un test che tocca il filesystem usa `!cwd`, o percorsi assoluti in una
temporanea propria — mai nomi nudi.

### Due difetti trovati mentre lo si costruiva

1. **`te_apply_config` riportava il demone a casa e non tornava.** Ricarica la KB
   da percorsi relativi alla radice, quindi esce dalla directory di lavoro: lo
   faceva già per la sandbox e non per `!cwd`. Il turno dopo un `!set` lavorava
   nel posto sbagliato.
2. ⭐ **La radice del workspace era quella del boot, per sempre.** `p0_root_init`
   la fissa alla cwd d'avvio — giusto per un agente che gira una volta, sbagliato
   per un demone che si sposta. Gli strumenti continuavano a lavorare sotto un
   descrittore vecchio, **a volte di una directory ormai cancellata**, e il
   sintomo era il più ingannevole possibile: `run make` che risponde «no makefile
   found» stando in una directory che il Makefile ce l'ha. Ora `p0_root_rebind()`
   la rilega, e `!cwd` la chiama.

⚠ Il secondo difetto **non riguarda solo i test**: chiunque sposti il processo
dopo l'avvio lo incontrava, e spiega anche perché `!sandbox` sembrava non fare
niente sugli strumenti.
