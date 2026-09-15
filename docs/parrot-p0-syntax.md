# La sintassi `.p0` di parrot0 — riferimento esaustivo e pratiche di scrittura

> **Stato:** scritto il 15 settembre 2026 da lettura del sorgente (`src/kb.c`,
> `src/kb.h`, `src/brain/*.c`), dalle prove dal vivo di questa e delle sessioni
> precedenti (`docs/plans/interlocutore-di-frontiera.md` §8.2, §9.12, §9.13) e
> dai documenti esistenti. È il riferimento **pratico**: che cosa si può scrivere
> in un `.p0`, che cosa il motore fa davvero con quello che si scrive, e le
> trappole che costano ore perché si mascherano da «nessuna soluzione».
> Il contratto d'inferenza in dettaglio (unificazione, SLD, provenienza, MCP)
> sta in [`prolog-like-engine.md`](prolog-like-engine.md); il caricamento e i
> profili in [`kb-loading-and-profiles.md`](kb-loading-and-profiles.md); la
> sintassi dei test `.p0t` in [`plans/test-engine.md`](plans/test-engine.md) §2.
> Le regole di *che cosa* mettere in KB e che cosa no sono in
> [`../MANTRA.md`](../MANTRA.md) e [`../PRINCIPLES.md`](../PRINCIPLES.md):
> questo file dice *come* scriverlo.

**Indice.** 1 Clausole · 2 Direttive · 3 Termini, liste, numeri, stringhe ·
4 Builtin del solver · 5 Guardie del solver · 6 Virgolette e corrispondenza ·
7 Viste materializzate · 8 Provenienza e stato · 9 Lo strato di superficie
(`turn_form`) · 10 Le risposte (`response_template`) · 11 Cue, classi e condotta
· 12 Il tabellone e i contabili · 13 Pratiche di buona scrittura · 14 Diagnosi
rapida · 15 Come si verifica.

---

## 1. Clausole

```prolog
% commento: dal % a fine riga, anche in coda a una clausola (fuori dalle virgolette)
parent(tom, bob).                                   % fatto ground
grandparent($X, $Z) :- parent($X, $Y), parent($Y, $Z).   % regola definita (Horn)
stipulation_cue("suppose that"). stipulation_cue("suppose").   % due clausole sulla stessa riga
```

- Una clausola termina con **un punto di livello 0** (fuori da parentesi e
  virgolette). La newline non ha valore sintattico: più clausole per riga vanno
  bene, e una clausola può continuare sulla riga dopo.
- Una clausola che non si parsa è **scartata con un errore rumoroso** su
  stderr: `kb_load: PARSE ERROR in <file>: … dropped …`. Nel test engine finisce
  in `obj/test-engine.log`; con `--mcp-engine` nello stderr del processo
  (`scripts/p0t-echo.py` lo scrive in `trace-<file>-<ora>.log`). **Il primo
  gesto quando una vista «non dà niente» è `grep "PARSE ERROR"` in quel log.**
- **Variabili:** solo ciò che comincia con `$` (`$X`, `$Seen`) o con `_`
  (anonima, fresca a ogni occorrenza). Le maiuscole nude sono **atomi**:
  `Madrid`, `M`, `Wrong` sono costanti.
- **Arità massima 4** (`KB_MAX_ARGS`, `src/kb.h`). Una regola o un fatto con
  cinque argomenti è scartato al boot con `PARSE ERROR`. Il quinto argomento
  viaggia in un termine: `goal($Op, $B, $Seen)`, `bound($Op, $B)`,
  `limit($K, $V)`.
- **Corpo massimo 16 goal** (`KB_MAX_BODY`). Una resa lunga si spezza in due
  predicati (`decision_residual_head` + `decision_residual_tail` in
  `kb/core/decisions.p0` è l'esempio lavorato).
- **Lunghezza di un termine: `KB_TERM_LEN` = 512** byte per predicato o
  argomento. Un `response_template` più lungo non carica (v. §10).
- Un fatto con variabili è una unit clause: le sue variabili sono
  standardizzate a parte a ogni invocazione.
- Non esiste il cut (`!`). L'ordine delle clausole conta per l'ordine delle
  soluzioni, non per l'esclusione: se due clausole possono valere insieme,
  valgono insieme (v. §10 su `op(match)`).

## 2. Direttive

```prolog
:- include(relative/path.p0).      % risolto dalla directory del file corrente
:- file_attribute(machinery).      % per ogni predicato introdotto dal file: machinery(Pred)
```

- `include/1` carica il file; oggi **non** è idempotente per file (una regola
  raggiunta da due catene può entrare due volte); i fatti identici sono
  deduplicati da `kb_assert`.
- `file_attribute/1` propaga un attributo **ai predicati dei fatti e delle
  teste di regola** introdotti dal file (e, comportamento corrente, anche agli
  inclusi). `machinery` è il valore più usato: quel predicato non è conoscenza
  del mondo, non si mostra all'interlocutore e non conta nei muri informati.
  In un file misto si scrive il fatto puntuale `machinery(nome_predicato).`
- La forma nuda `file_attribute(X).` senza `:-` è deprecata.
- `lazy_load/1` e `file_layer/1` sono **progettate, non implementate**: non
  vanno scritte nei `.p0` operativi.
- Un **profilo** (`kb/profiles/agi.p0`) è un manifesto di `include`: il
  profilo non specificato è `agi`; la KB viva è sempre completa (MANTRA: niente
  KB ermetica).

## 3. Termini, liste, numeri, stringhe

| forma | esempio | note |
|---|---|---|
| atomo | `train`, `le`, `parrot0` | minuscole per convenzione; le maiuscole sono atomi, non variabili |
| stringa | `"at most"`, `"Held: {role} is never below {value}."` | UN argomento; le virgole interne sono contenuto; v. §6 per la corrispondenza |
| numero | `7`, `0.5`, `-1` | `is/2` stampa con `%g`: `sub(11, 10)` → `1`, `add(10, 0.5)` → `10.5` |
| termine composto | `bound(le, 11)`, `status($R, bound($Op, $B), $S)` | unificazione strutturale; una variabile lega una sotto-struttura |
| lista | `cons(a, cons(b, nil))`, `nil` | non c'è la sintassi `[a, b]` nei corpi delle regole (solo dentro le stringhe `op(...)`, §9) |
| atomo composto | `important_meeting` | gli underscore uniscono parole: `atom_words/2` e `words_of/2` li rileggono come parole |

Un `dif($A, $B)` fra atomi funziona; `ne/2` è solo numerico.

## 4. Builtin del solver

Riconosciuti in `src/kb.c` (`solve`); non esiste il predicato «utente» con
quel nome, quindi non si ridefiniscono.

| builtin | semantica | trappole |
|---|---|---|
| `is($R, Expr)` | valuta `add/sub/mul/div/mod` annidabili; lega `$R` | `div` per zero fallisce; il risultato è testo `%g` |
| `lt le gt ge eq ne` /2 | confronti **numerici** | entrambi gli argomenti devono valutare a numero, altrimenti falliscono |
| `dif($A, $B)` | disuguaglianza fra termini (differita) | con variabili non ground dentro un termine composto può non decidere: legare prima |
| `naf($G)` | negazione per fallimento | **solo goal ground** (altrimenti flounder = fallisce); **declina sotto qualunque guardia** del solver (profondità, passi, resolvent): mai avvolgere una vista ricorsiva pesante |
| `not($G)` | come `naf` (alias) | e `not(P)` come **fatto** è negazione esplicita che blocca `P` |
| `call($G)` | meta-chiamata | |
| `findall($T, $G, $L)` | raccoglie in `$L` le soluzioni di `$T` — **è un INSIEME** (dedup) | (1) il template è **solo una variabile**: `findall(cand($M,$R), …)` dà `nil` in silenzio → costruisci il termine nella testa di un ausiliario; (2) il terzo argomento deve essere **una variabile libera**: `findall($T, G, cons($T, $Ts))` non riesce mai; (3) le soluzioni vengono **ristampate e rilette**: una **virgola** dentro un testo raccolto spezza la lista (`cons(a, b, nil)` a tre argomenti) e il consumatore fallisce in silenzio |
| `findall_bag/3` | come `findall` ma conserva i duplicati | stesse trappole |
| `assert(Pred, A1, …)` / `retract(Pred, A1, …)` | scrive/toglie un fatto; il predicato è il **primo argomento** (fino a 4 argomenti dopo) | `assert` dentro una prova che sta rispondendo è tollerato (idempotente); **`retract` dentro una prova che sta rispondendo può uccidere il processo** (lo stato cambia mentre il solver lo enumera): i retract si fanno nei contabili (§12) |
| `member($X, $L)`, `list_len($L, $N)` | **non** builtin: regole in `kb/core/procedures.p0` | `naf(member($R, $Seen))` è il modo standard di tagliare i cicli |
| `concat_atoms($A, $B, $C)` | concatena testi | il pezzo di resa più usato; 16 goal per corpo si esauriscono presto |
| `atom_words($A, $Ws)` | atomo ↔ lista di parole, spezza su `_` **e spazi**, bidirezionale | serve a leggere «at most 1» come parole |
| `words_of($A, $T)` | presentazione di un atomo composto come parole | |
| `upcase_first($A, $B)` | iniziale maiuscola | |
| `chars($A, $L)` | atomo ↔ lista di caratteri | |
| `apply($Op, cons($A, cons($B, nil)))` | applica un confronto o un'operazione nominata da un atomo | `apply(le, …)`, `apply(gt, …)`, `apply(dif, …)`: il verso di un confronto diventa un **dato** |
| `kb_fact/2`, `kb_rule/2`, `kb_rule_body/2` | introspezione: fatti, regole, nomi dei predicati del corpo | `kb_rule_body` dà solo nomi, non argomenti |
| `present_term/2` | resa di un termine secondo le `present_rule` | |
| `prob/2`, `ranges_over/3` | probabilità KB-backed, intervalli temporali | usi rari |

## 5. Guardie del solver

`KB_MAX_DEPTH` = 64 di profondità, `KB_MAX_GOALS` sul resolvent, un budget di
passi, lo stack. Quando una guardia scatta la prova **fallisce** senza dirlo.
Due conseguenze pratiche:

1. **Le selezioni si calcolano una volta in una lista** e dominanza, resa e
   negazioni lavorano sulla lista con `member/2` (`decision_verdict/3`,
   `decision_statuses/2`): con le viste ricorsive ogni `naf` rifaceva l'intera
   ricerca, una guardia scattava dentro la negazione e il turno cadeva al muro
   generico. Misurato: la barca con due formule e quattro dati ignoti da 22 s a
   1,7 s.
2. **Un `naf` avvolge solo goal ground e leggeri**: `naf(decision_has_kind($Fs,
   broken))` sì; `naf(decision_statuses(...))` no. Se serve negare un insieme,
   si raccoglie con `findall` fuori dal `naf` e si nega `member`.

Il profilo per predicato (`/debug`) dice **quale** predicato costa: si
profila, non si indovina (MANTRA #20).

## 6. Virgolette e corrispondenza (letto nel C il 15 settembre 2026)

- Un argomento fra virgolette è un argomento come gli altri per l'unificazione
  **dentro le regole**: `decision_comparison("at most", le)` combacia con
  `decision_comparison($Name, $Op)`.
- **Ma un argomento LEGATO passato dal C a `kb_match`/`kb_query` non combacia
  con un fatto scritto tra virgolette**: `kb_query(slot_value_stop, "and")` non
  trova `slot_value_stop("and").`, né con la parola nuda né con le virgolette.
  (Verificato via MCP: `? self_reference i` → 0 soluzioni, mentre il C legge
  `self_reference("i")` benissimo.) I lettori C che funzionano **enumerano con
  pattern `NULL` e confrontano con `kb_dequote`** (`personal_selfref_word`,
  `p0_listed_word`, `p0_say_empty`, i lettori di `social_pattern`).
- `kb_cue_match(b, Classe, testo)` legge le `intent_cue(Classe, "…")` quotate
  con il proprio lettore: le cue **vanno** quotate.
- **Convenzione da tenere:** stringhe di superficie (cue, pattern, parole di
  resa, nomi detti) **tra virgolette**; atomi interni (nomi di ruoli, mosse,
  generi, chiavi di template) **nudi**. Un file che mescola le due forme per
  lo stesso predicato (com'era `social_pattern`) va armonizzato **insieme al
  suo lettore**, altrimenti metà delle righe smette di combaciare.
- Nel turno canonicalizzato gli apostrofi diventano spazi: la cue si scrive
  `"you re wrong"`, `"what s wrong"`.
- Gli `span` delle forme (§9) arrivano **canonicalizzati**: in italiano «al
  massimo» arriva come «to the massimo». Una forma italiana diretta con «al»
  non combacia mai: l'italiano entra per **parafrasi insegnata** («vale al
  massimo z means it is at most z»), che vale subito e si ritratta.

## 7. Viste materializzate

```prolog
materialized_view(scenario_claim, 2).          % il predicato e la sua arità
view_depends(scenario_claim, turn_surface_token).
view_depends(scenario_claim, relation_surface).
```

Le soluzioni del predicato si enumerano una volta al boot (`kb_views_warm`) e
si congelano come fatti `KB_DERIVED`; la vista si invalida quando cambia una
conoscenza da cui dichiara di dipendere (`view_depends/2`), quindi un verbo
insegnato adesso è visibile nello stesso turno. **Quali** viste congelare è
conoscenza; il meccanismo è uno. La chiave della cache dev'essere stretta e
O(1) (MANTRA #20 d). Togliere la dichiarazione fa tornare la derivazione: una
vista è un acceleratore, mai parte del significato.

## 8. Provenienza e stato

| dichiarazione | effetto |
|---|---|
| `machinery(P).` / `:- file_attribute(machinery).` | `P` non è conoscenza del mondo: non si mostra, non conta nei muri, non si impara come lemma |
| `turn_scratch(P).` | i fatti di `P` sono stato del dialogo: **non sopravvivono a `/save`** |
| `provenance_predicate(P).` | `P` porta provenienza |
| strati `KB_BASE / SESSION / INDUCED / REFLECTIVE / HYPOTHETICAL / DERIVED` | origine di ogni clausola; `!forget @session` nei test butta uno strato |
| `turn_counter($N)`, `current_turn`, `turn_reply/2`, `turn_input/2`, `turn_entity/2`, `turn_topic/2` | il turno corrente e i precedenti nella finestra: la conversazione **è** fatti KB (`discourse.p0`) |
| `situation_state(Sit, Ent, Prop, Val)` con `state_commit/3` | lo stato descritto; un valore nuovo **supersede** il vecchio senza distruggerlo (`supersedes_in`) — anche un termine come `bound(le, 1)` è un valore |

## 9. Lo strato di superficie: `turn_form`

Una **forma di turno** è una sequenza numerata di pezzi. È il lettore generico
della IR: quando una forma combacia, il suo atto scrive o interroga la KB e il
suo template risponde. Non esiste parser privato: una forma nuova è un gruppo
di fatti e vale dal turno dopo.

```prolog
turn_form(decision_requirement_lesson, 1, text("for")).
turn_form(decision_requirement_lesson, 2, span(subject)).
turn_form(decision_requirement_lesson, 3, text("the")).
turn_form(decision_requirement_lesson, 4, slot(role)).
turn_form(decision_requirement_lesson, 5, text("must be")).
turn_form(decision_requirement_lesson, 6, named(decision_comparison, comparison)).
turn_form(decision_requirement_lesson, 7, slot(value)).
turn_form_act(decision_requirement_lesson, "op(assert, decision_requirement, [subject, role, comparison, value])").
turn_form_priority(decision_requirement_lesson, early).
turn_form_view(decision_requirement_lesson, said).
turn_form_reply(decision_requirement_lesson, decision_requirement_learned).
turn_form_slot_form(decision_requirement_lesson, subject, atom).
```

**I pezzi** (`src/brain/10-memory-knowledge.c`, lettore di `turn_form`):

| pezzo | legge | note |
|---|---|---|
| `text("…")` | un'ancora letterale, anche di più parole | in minuscolo, canonica |
| `slot(Nome)` | **un** token | `turn_form_slot_class(F, Nome, Pred)`: il valore deve soddisfare `Pred/1` (es. `decision_numeric`) |
| `span(Nome)` | le parole fino alla prossima ancora `text` (o fino alla fine) | conservate come testo, **canonicalizzate**; `turn_form_slot_form(F, Nome, atom)` le unisce con `_` (`singular` singolarizza) |
| `rest(Nome)` | tutto il resto del turno | |
| `named(Pred, Nome)` | le **parole con cui si chiama** qualcosa: cerca una superficie fra i primi argomenti di `Pred/2` e mette nello slot il **nome interno** (secondo argomento) | è il pezzo che rende insegnabile un nome tecnico: un sinonimo è una riga (`decision_move_name("confirming the check", confirm)`) |
| `class(Pred)` | un token membro di una classe `Pred/1` | |
| `relation(Nome)` | una relazione nominata nel turno | |
| `bind(Nome, valore)` | lega uno slot a una costante senza leggere | es. `bind(relation, ability_of)` |
| `expr`, `construct` | espressioni e costruzioni (forme avanzate) | vedi le forme esistenti prima di usarli |

**Gli atti** (`turn_form_act`): la stringa `"op(Verbo, Pred, [slot, …])"` con
`Verbo` ∈ `match | assert | retract | retract_all`; gli slot si passano
**nell'ordine** come argomenti di `Pred`; `free` è un argomento libero il cui
valore diventa `{result}` nel template. `op(match, …)` **concatena tutte le
righe con «, » e rifiuta i duplicati**: una vista di risposta deve produrre
esattamente una riga (guardie mutuamente esclusive). Altri atti nominati:
`assert_relation`, `answer_relation`, `answer_polar`, `assert_unary`,
`assert_ordered`, `run_procedure`, `reread(…)`.

**Le altre dichiarazioni:** `turn_form_priority(F, early)` (prima delle
facoltà di contenuto), `turn_form_view(F, said)` (legge la superficie detta),
`turn_form_mood(F, statement|question)`, `turn_form_reply(F, Chiave)` (§10),
`turn_form_empty_reply(F, Chiave)` (che dire se l'atto non dà righe),
`turn_form_empty_opens(F, …)`, e la **cessione**
`turn_form_yield(F, Rel, Pos, Slot)`: la forma non legge un turno che contiene
una superficie di `Rel/2` (l'argomento in `Pos`) che copre anche il valore di
`Slot`. `Rel` **deve essere binario** (il lettore enumera coppie): `second_person_deictic("you", parrot0)`.

**La parafrasi insegnata:** «x is at least z for y means for y the x must be
at least z» e «forget that … means …» sono forme generiche: una lezione in
un'altra lingua o in un'altra superficie non si promuove in `.p0`, si insegna
(MANTRA, gerarchia di crescita). Nei banchi si insegna e si ritratta.

## 10. Le risposte: `response_template`

```prolog
response_template(decision_answer, "For {subject}: {result}").
response_template(decision_answer, it, "Per {subject}: {result}").
response_template(decision_datum_noted, "Noted: {result}").
```

- `/2` è la forma di default, `/3` con la lingua (`it`, `en`, …) è scelta da
  `current_language/1`; il C chiama `kb_term_say(b, chiave, slots…)` e non
  contiene la frase (MANTRA #16).
- Gli slot `{nome}` prendono i valori degli slot della forma o del sito C;
  `{result}` è l'argomento `free` dell'atto.
- **Un template `"{text}"` vuoto di lingua non è una resa** (MANTRA #18 b): la
  frase deve vivere nel template o in fatti di parole (`decision_bound_words(en,
  le, "at most ")`) composti con `concat_atoms`.
- Un template più lungo di `KB_TERM_LEN` non carica.
- Le frasi composte in KB e raccolte in liste **non contengono virgole** (§4,
  `findall`): «The transfer must be at most 1 to keep ready at most 11.»

## 11. Cue, classi e condotta

| predicato | ruolo |
|---|---|
| `intent_cue(Classe, "superficie")` | cue **substring** sul turno canonico (`kb_cue_match`); attenzione a «eat» ⊂ «f-eat-hers» (MANTRA #8): per le discriminanti, parola intera |
| `intent_phrase(Classe, "frase")` | frase intera |
| `phrase_canon("mi chiamo", "my name is")` | locuzione canonicalizzata prima della lettura (`lexicon.p0`) |
| `spelling_of(Errata, Giusta)` | refusi insegnati parlando (`spelling.p0`) |
| classi `*_lex*`, `*_cue*`, `*_chain*` con nome seriale | **debito**: sono `strcmp` con un altro indirizzo; una classe prende il nome del suo **ruolo** (MANTRA #19 a) |
| `turn_pattern(Forma, cue\|not_cue\|word\|text, Arg)` + `turn_pattern_intent(Forma, Intento)` | la **congiunzione** come regola KB (MANTRA #19 b) |
| `turn_declared_act(Turn, Atto)` | atto dichiarato del turno dalla IR (`turn-frames.p0`): `production_request`, `own_procedure_request`, `lesson_turn`, `prose_carried`, … |
| `turn_illocution(Turn, question\|expressive\|…)` | forza del turno; `naf(turn_illocution($T, question))` è la guardia standard per «non è una domanda» |
| `faculty_yield(Facoltà, open\|late, Classe)` | la facoltà tace davanti a una classe di cue; `open` prima della gara, `late` dopo |
| `faculty_yield_force(Facoltà, open, Atto)` | idem, su un `turn_declared_act` |
| `faculty_yield_both(Facoltà, open, C1, C2)` | tace solo se entrambe le classi sono presenti |
| `compound_guard(Facoltà, Classe)` | nel turno composto |
| `move_policy(Situazione, Mossa)` | politica di mossa (`dialogue-policy.p0`) |
| `pragma_act(Atto)` + `intent_cue(Atto, …)` + `response_template(Atto, …)` | atti pragmatici serviti da `chitchat` — **soggetti alla politica di mossa**: per una risposta che deve valere sempre, una `turn_form` early è più sicura |
| `social_marker(Tipo, parola)`, `social_pattern(Tipo, "frase")` | registro sociale (`social.p0`) |
| `slot_evidence(Slot, "cue")`, `slot_eager(Slot)`, `slot_value_stop("and")` | memoria personale (`personal.p0`): il valore si ferma a una congiunzione |

Le facoltà C che hanno il gancio di cessione (`p0_faculty_yields`):
`analysis_family`, `analysis_last_resort`, `answer_frame`, `arith`, `gen`,
`process_steps`, `role`, `turn_plan`, `wordquery`, `family`, `discourse`,
`smalltalk`. Una facoltà senza gancio si ripara con **una** riga in C che lo
apre, mai con una condizione cablata (MANTRA #17).

## 12. Il tabellone e i contabili

```prolog
open_issue(datum_train_transfer, datum).   issue_topic(I, train).
issue_relation(I, transfer).               issue_turn(I, 12).
issue_state(I, open|resolved) :- …         % UNA VISTA, mai un campo
max_qud(I) :- issue_open(I, K), naf(issue_superseded_by_later(I)).
```

- Le questioni (`issues.p0`) sono fatti `turn_scratch`; il loro **stato è una
  vista**, così una questione si risolve quando la conoscenza la rende risolta.
- **I contabili:** `bookkeeper(Nome).` + `turn_bookkeeping(current_turn, Nome)
  :- …` girano **a inizio turno, prima della risposta**, ciascuno **fino alla
  prima soluzione** (per fare N cose si raccoglie una lista e si scorre). È il
  posto lecito per `retract`: `datum_reasked/2` in `decisions.p0` §5 e
  `inquiry_reset_turn` in `issues.p0` sono gli esempi.
- Una prova che vuole «ricordare» qualcosa per il turno dopo **asserisce un
  fatto scratch** e lascia che il contabile lo consumi.

## 13. Pratiche di buona scrittura

1. **Prima il banco, poi la cura.** Un `.p0t` con i contrasti che devono
   distinguersi, letto a HEAD con `scripts/p0t-echo.py` (mantra #9: il runner
   non vede una risposta sbagliata che contiene la parola attesa). Poi la KB.
2. **Il file comincia con il perché.** Un commento in testa: che cosa entra,
   qual era il sintomo misurato, con la data. Le sezioni `% ── N. TITOLO ──`.
3. **Una classe per ruolo, con un nome pronunciabile** da chi insegna:
   `acquisition_verb`, non `..._lex2975`. Il test: «parrot0 può impararne un
   nuovo membro domani, parlando?».
4. **Nessuna parola, frase o condotta nel C**: le parole in KB (#2), le frasi
   in `response_template` (#16), il *se* e il *chi* in `faculty_yield`,
   `turn_declared_act`, `move_policy` (#17). Nel C solo primitive generali e
   porte di una riga.
5. **Un termine, non un predicato nuovo, per ogni distinzione**: `exact(V)` →
   `limit(exact, V)` quando arriva il secondo genere; il genere è un dato in una
   tabella (`decision_bound_confirms/3`), non un ramo.
6. **Le liste costose si calcolano una volta**; negazioni e rese lavorano su
   `member`. Mai `naf` su una vista ricorsiva.
7. **`findall` con template variabile, risultato libero, testi senza virgole.**
8. **Corpi ≤ 16 goal**: le rese si spezzano in `..._head` / `..._tail`.
9. **Mai `retract` in una prova che risponde**; `assert` idempotente sì;
   i retract nei contabili.
10. **`op(match)` vuole UNA riga**: guardie mutuamente esclusive
    (`decision_empty($Bs)` vs `naf(decision_empty($Bs))`).
11. **Virgolette:** superfici quotate, atomi nudi, lettore coerente (§6).
12. **L'italiano entra per parafrasi**, non per forma promossa (§6, §9).
13. **Le viste, non i campi**: `issue_state`, `decision_status`, `role/3`
    sono derivate; ritrarre la lezione cambia subito il comportamento
    (ablazione nel banco).
14. **Ogni numero ha un ruolo** prima di fare aritmetica (#12); il muro onesto
    batte la risposta sbagliata (#7) e batte il repertorio di genere.
15. **Nomi:** `decision_*` per il circuito, `_in` per la variante con
    accumulatore/lista, `_words`/`_text`/`_render` per la resa,
    `_lesson`/`_forget`/`_question` per le forme.
16. **Misure con la data.** «L'abbiamo misurato» vale solo con quando e dove.
17. **Il commit dice il bilancio**: quante righe C, quante KB (#18 a).

## 14. Diagnosi rapida

| sintomo | prima causa da controllare |
|---|---|
| una vista «non dà niente» ma i pezzi funzionano da soli | `PARSE ERROR` nel log (arità 5, corpo 17); poi `findall` (template composto, risultato legato, virgole); poi `naf` sotto guardia |
| il turno cade al muro generico dopo aver quasi risposto | una guardia dentro un `naf`; calcola la lista una volta |
| `op(match)` risponde «a, b» con due frasi | due clausole valgono insieme: guardie esclusive |
| il processo MCP muore senza stderr | `retract` dentro la prova |
| un fatto quotato «non esiste» per il C | `kb_query`/`kb_match` con argomento legato: enumerare e dequotare |
| una forma italiana non combacia mai | lo span arriva canonicalizzato: parafrasi insegnata |
| «Because because …» / simboli interni nella risposta | la prova esposta cruda: presentazione, non ramo |
| un modulo risponde a sproposito | «who answered?» dice quale; la cura è `faculty_yield`/`turn_declared_act`, o il gancio se manca |
| il nome dell'utente è tutta la frase | `slot_value_stop` e lettore che dequota |
| un test rosso solo per `turn took 1.2s (timeout 1.00s)` | costo del turno base (`TEST_TODO.md`), non del cambiamento; non si alza il budget |

## 15. Come si verifica

```sh
make test-engine                                   # demone + build
./bin/parrot0 --test tests/p0t/reasoning/taught_decision.p0t     # certifica (assert-only)
python3 scripts/p0t-echo.py FILE.p0t | grep -v '^  \['             # LEGGE le risposte (righe > ) e interroga (righe ? pred a _)
grep 'PARSE ERROR' trace-*.log obj/test-engine.log                 # il primo sospetto
P0_FORM_TRACE=1 python3 scripts/p0t-echo.py FILE.p0t               # quale forma ha letto il turno
> who answered?                                                    # in chat: quale modulo ha risposto
make soft-test                                                     # avanzamento, budget 15 s
```

La suite intera si lancia solo con l'approvazione di F. (`CLAUDE.md`).
