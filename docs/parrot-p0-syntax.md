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
(`turn_form`) · 10 Le risposte: template, `answer_content`, stadi · 11 Cue, classi e condotta
· 12 Il tabellone e i contabili · 13 Pratiche di buona scrittura · 14 Diagnosi
rapida · 15 Come si verifica · 16 Contenuti, contesti e prove (contratto
progettato, non nuova sintassi eseguibile).

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
| `member($X, $L)`, `list_len($L, $N)`, `append($A, $B, $AB)` | **non** builtin: regole in `kb/core/procedures.p0` (`append/3` dal 26 settembre 2026) | `naf(member($R, $Seen))` è il modo standard di tagliare i cicli; `append` si chiama con la prima lista legata (due `findall` e poi `append`, così l'ordine dei pezzi lo decide chi scrive) |
| `concat_atoms($A, $B, $C)` | concatena testi | il pezzo di resa più usato; 16 goal per corpo si esauriscono presto |
| `atom_words($A, $Ws)` | atomo ↔ lista di parole, spezza su `_` **e spazi**, bidirezionale | serve a leggere «at most 1» come parole |
| `map_words($Testo, $Pred, $Uscita)` | riscrive ogni parola intera del testo secondo le righe di `$Pred/2` (nucleo confrontato senza maiuscole, punteggiatura conservata, sostituto letterale) | la condotta sull'espressione (`reply-conduct.p0`): `reply_conduct($In, $Out)` è chiesta da `turn_done` al livello esterno di ogni turno |
| `words_of($A, $T)` | presentazione di un atomo composto come parole | |
| `upcase_first($A, $B)` | iniziale maiuscola | |
| `chars($A, $L)` | atomo ↔ lista di caratteri | |
| `apply($Op, cons($A, cons($B, nil)))` | applica un confronto o un'operazione nominata da un atomo | `apply(le, …)`, `apply(gt, …)`, `apply(dif, …)`: il verso di un confronto diventa un **dato** |
| `kb_fact/2`, `kb_rule/2`, `kb_rule_body/2` | introspezione: fatti, regole, nomi dei predicati del corpo | `kb_rule_body` dà solo nomi, non argomenti; **`kb_fact($P, …)` con il predicato libero scandisce TUTTA la KB** anche con gli argomenti legati (misurato il 26 settembre: 136 M fatti visitati, 3.5 s per turno). Si lega prima il predicato da una classe dichiarata (`description_relation($P), kb_fact($P, …)`) |
| `kb_clause/4`, `kb_clause_arg/4` | la clausola INTERA come dato (M1, 20 settembre 2026): `kb_clause(Id, Testa, 0, N)` la clausola con N premesse, `kb_clause(Id, Testa, I, Premessa)` la I-esima; `kb_clause_arg(Id, Dove, Cammino, Nodo)` la stessa per nodi e archi. Forma canonica taggata a ogni livello — `var(N)` variabile, `atom(A)` costante, `app(F, cons(…, nil))` applicazione — così un dato scritto `var(0)` è `app(var, cons(atom(0), nil))` e non si confonde con la variabile. Fatti negativi `not(E)`, `naf(G)` conservato. Identità `content(Pred, impronta)` | l'impronta copre la struttura intera, non un testo che un buffer può tagliare; con l'Id o la testa legata costa un bucket; un pezzo che non entra in un termine vale `overflow(Pred)`, resta ritrovabile legandone testa e Id, e si legge per archi; facce nominabili in `kb/core/clause-content.p0` |
| `kb_act/3` | gli ATTI di un contenuto (M2): `kb_act(Id, Testa, Bit)`, un atto per livello di provenienza che lo ha fatto entrare. Lo stesso contenuto entrato per due vie ha due atti, e ritirarne uno lascia vivo l'altro | il motore dà il BIT, il NOME del livello è un fatto KB (`act_layer/2`): un livello si nomina senza ricompilare |
| `kb_derivation/4` | la PROVA prodotta dalla ricerca che decide (M2): `kb_derivation(D, Goal, 0, N)` una derivazione con N dipendenze, `kb_derivation(D, Goal, I, Dip)` la I-esima. Dipendenze congiunte (AND), derivazioni alternative sul backtracking (OR). Tre specie: `content(P, impronta)` una clausola usata, `absent(G)` una negazione per fallimento, `aggregate(G)` un findall | con D libera e Goal legato non si apre una seconda ricerca; `aggregate_incomplete(G)` se l'enumerazione è stata tagliata, `incomplete(N)` se i passi hanno superato la pila; `derivation_<n>` vale nella sessione e non si salva; facce in `kb/core/derivation.p0` |
| `kb_turn_act/4` | che cosa ha scritto QUESTO turno (L3 §14.6, 25 settembre 2026): `kb_turn_act(P, Args, Pol, K)`, i fatti (`pos`) e le negazioni esplicite (`neg`) che un atto di sessione ha scritto nel turno corrente; `K` = `new` se il contenuto è entrato adesso, `again` se era già noto e solo riaffermato | `Args` è una lista `cons(…)` come in `kb_fact/2`; scandisce tutta la tabella (economico, ma si mette dopo i cancelli); il timbro non si salva |
| `read_support/2` + `fact_withheld/1` | lo STRATO sopra un fatto letto (§14.6, §19): se esiste `read_support(fact(P, A1, A2, Pol), Via)`, il motore chiede `fact_withheld(fact(…))` e, se vale, il fatto è **invisibile a ogni lettore** (risolutore, `kb_match`, `kb_query`, negazioni) senza essere cancellato. Che cosa sia un sostegno in forza lo dice la KB (`kb/core/contact.p0`) | protocollo di nomi fissi, non builtin; paga solo chi ha sostegni registrati; dentro `fact_withheld` lo strato è spento (niente ricorsione); le viste congelate che contengono il fatto non si rifanno da sole |
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
- **Dentro le regole una stringa quotata unifica solo con una stringa quotata
  identica o con una variabile: NON con l'atomo nudo.** `time_preposition("at")`
  non combacia con il token `at` del frame (`span_atom` dà atomi nudi), mentre
  `day_word(tomorrow)` sì (misurato in E3). Le parole che si confrontano con i
  token del turno si scrivono nude; le stringhe quotate servono alle superfici
  lette dal C (`kb_cue_match`, `named(...)`, `atom_words`).
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
| ⚠ fatti pubblicati dal C a ogni turno (`turn_*`, `clock_time`, `calendar_day`, `turn_pattern_match`) | vanno dichiarati **sia** `machinery` **sia** `turn_scratch`: altrimenti entrano nel giornale dei fatti recenti e un turno con «never» (modulo `negation`) li nega uno per uno e ruba la lezione (misurato: «transfer is never below 0» → «Learned: …» invece di «Held: …») |
| `provenance_predicate(P).` | `P` porta provenienza |
| strati `KB_BASE / SESSION / INDUCED / REFLECTIVE / HYPOTHETICAL / DERIVED` | origine di ogni clausola; `!forget @session` nei test butta uno strato |
| `clock_time(H, M)`, `calendar_day(Offset, "YYYY-MM-DD", weekday)` | l'orologio pubblicato dal C a ogni turno (offset −1…+7); `origo.p0` ne fa oggi/domani/ora |
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
| `span(Nome)` | le parole fino alla prossima ancora `text` (o fino alla fine) | conservate come testo, **canonicalizzate**; `turn_form_slot_form(F, Nome, atom)` le unisce con `_` (`singular` singolarizza); `turn_form_slot_form(F, Nome, mention)` le tiene **come dette**, grafia compresa (copiate a parola intera dal turno grezzo) |
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

## 10. Le risposte: tre livelli, dal segnaposto allo stadio

Ciò che parrot0 DICE è conoscenza quanto ciò che legge (MANTRA #16): nessuna
frase vive nel C. Esistono **tre livelli** di resa, e la scelta fra loro è
parte del disegno (`docs/plans/inferenza-compositiva.md`,
`docs/plans/messages-are-knowledge.md`).

### 10.1 Il template a segnaposto: `response_template`

```prolog
response_template(decision_answer, "For {subject}: {result}").
response_template(decision_answer, it, "Per {subject}: {result}").
response_template(decision_datum_noted, "Noted: {result}").
```

- `/2` è la forma di default, `/3` con la lingua (`it`, `en`, …) è scelta da
  `current_language/1`; il C chiama `kb_term_say(b, chiave, slots…)` /
  `kb_response_slots` e non contiene la frase.
- Gli slot `{nome}` prendono i valori degli slot della forma o del sito C;
  `{result}` è l'argomento `free` dell'atto di una `turn_form`.
- **Un template `"{text}"` vuoto di lingua non è una resa** (MANTRA #18 b): la
  frase deve vivere nel template o in fatti di parole (`decision_bound_words(en,
  le, "at most ")`) composti con `concat_atoms`.
- Un template più lungo di `KB_TERM_LEN` (512) **non carica**, in silenzio a
  meno del `PARSE ERROR`.
- **Il limite del segnaposto:** la forma esiste prima del contenuto. Se un
  pezzo manca resta un buco (o una bugia: `undetermined_cycle` diceva «le
  regole si rimandano» anche quando a fermarsi era il budget), e due template
  monolitici per due stati sono la stessa risposta scritta due volte. Un buco
  è lecito solo se nomina un argomento della tesi che quella frase dimostra.

### 10.2 La risposta a pezzi: `answer_content/4` → `answer_text/2`

```prolog
answer_content(setting_ack($Place, $Language), 0, opener, $Piece) :- setting_opener($Language, $Piece).
answer_content(setting_ack($Place, $Language), 1, place,  $Piece) :- value_in_language($Language, $Place, $Piece).
answer_content(setting_ack($Place, $Language), 2, closer, $Piece) :- setting_closer($Language, $Piece).
turn_response($T, $Text) :- setting_unique($T, $Place), current_language($L), answer_text(setting_ack($Place, $L), $Text).
```

Il fold è in `procedures.p0` (`answer_tail/3` concatena i pezzi `0, 1, 2, …`
finché esistono); `list_text/3` (`discourse.p0`) rende un elenco con il
separatore della lingua; `state_copula/2`, `list_separator/2`,
`sentence_terminator/2`, `linguistic_form/4` sono le convenzioni vive. Ventitré
famiglie lo usano (`situation.p0`, `place-questions.p0`, `honest-limits.p0`,
`code-plans.p0`, `state-description.p0`, …). **I suoi quattro limiti**, che
sono l'intera delta verso il livello successivo: l'ordine è un intero scritto
a mano; l'**arietà** del termine (`explain($Action, $Effect, $Change, $Lang)`)
decide *prima della prova* quanti pezzi ci saranno — un template scritto in
Prolog; se **un** pezzo non si prova, `answer_text` fallisce e il turno cade in
silenzio al percorso storico; un pezzo è una stringa, non può essere un'altra
risposta condizionale.

### 10.3 Lo stadio: la risposta come albero di inferenza (`composition.p0`)

> *Un segnaposto è un buco che aspetta un valore; uno stadio è un'inferenza
> che, se riesce, avvolge dentro di sé ciò che ha trovato — e se non riesce,
> non esiste.* I connettivi («: », «, and», «, so») sono le **cicatrici** delle
> relazioni retoriche che l'appiattimento in template ha cancellato.

Il motore è in KB, sei clausole senza una parola di lingua:

```prolog
stage_holds($S) :- stage_claim($S, $C), call($C).          % la tesi dello stadio, dimostrata
stage_holds($S) :- naf(stage_has_claim($S)).               % senza tesi: incondizionato (il nucleo)
composed($S, $L, $T) :- naf(stage_is_wrapper($S)), stage_holds($S), stage_text($S, $L, $T).            % (1) il nucleo
composed($S, $L, $T) :- stage_wraps($S, $In), composed($In, $L, $X), stage_holds($S), stage_around($S, $L, $X, $T). % (2) lo strato che regge
composed($S, $L, $T) :- stage_wraps($S, $In), naf(stage_holds($S)), composed($In, $L, $T).                          % (3) quello che non regge SPARISCE
```

Il vocabolario di uno stadio, tutto fatti — e quindi insegnabile uno strato
alla volta:

| fatto | ruolo |
|---|---|
| `stage_wraps(S, Interno)` | quale composizione avvolge; il nucleo non avvolge nessuno |
| `stage_claim(S, Goal)` | la **tesi** che deve reggere (una vista: `turn_goal_unanchored(current_turn)`), mai un fatto messo lì per far comparire la frase |
| `stage_relation(S, frames\|elaborates\|concludes\|qualifies\|offers\|embeds)` | la relazione retorica con l'interno; il connettivo è la sua resa per lingua (`relation_connective/3`), non punteggiatura nello strato |
| `stage_side(S, before\|after)` | avvolge prima (cornice) o dopo (elaborazione, conseguenza) |
| `stage_text(S, L, Testo)` | le parole dello strato, per lingua; possono essere a loro volta composte (`class_phrase/3`, `list_text/3`) |

Ogni stadio riceve **una sola cosa** dall'interno, il testo già composto, e si
dimostra il resto da sé: ogni strato è indipendentemente vero o falso e
interrogabile («perché hai detto quella frase?» ha una prova per strato). Un
buco (`{klass}`) è lecito solo se nomina un argomento della tesi provata da
quello stadio. Stato del primo taglio (gen505): il motore e il caso di studio
`undetermined_cycle` (sei stadi) esistono e si interrogano con `composed/3`;
**non prendono ancora la parola** (nessun `turn_response` lì), perché i
sensori riflessivi che le tesi consultano (`turn_goal/3`, `inference_cycle/2`,
`inference_incomplete/2`) non sono ancora depositati dal C: è il gate C1 del
piano. Per esercitarla in un `.p0t` si mettono quei fatti a mano.

**Vincoli misurati che valgono per tutti e tre i livelli:** `concat_atoms`
in overflow (512) **fallisce**, non tronca — una risposta troppo lunga sparisce
intera e il turno cade al percorso storico (il piano chiede di farne un fatto,
`composition_truncated/2`); `naf` con una variabile libera nel goal negato non
lega mai (`naf(stage_wraps($S, $Any))` è sempre falso: si nomina un ausiliario
`stage_is_wrapper/1`); una cipolla ciclica finisce nella guardia di profondità.
`turn_response/2` è chiesto **a ogni turno** con un solo `kb_match`: ogni
famiglia in più costa budget, e si misura (`PARROT0_TE_SLOW`), non si stima.

**Quale livello scegliere.** Una frase fissa con un valore: template. Una
frase i cui pezzi esistono tutti sempre nello stesso ordine: `answer_content`.
Una risposta il cui **numero di parti dipende da ciò che si è dimostrato**
(un ostacolo c'è o non c'è, il ciclo ha membri o no, un'offerta è possibile o
no): stadi. La direzione del progetto è la terza: un template lungo con due
o più tesi dentro va letto come un albero appiattito e sfogliato.

Le frasi composte in KB e raccolte in liste **non contengono virgole** (§4,
`findall`): «The transfer must be at most 1 to keep ready at most 11.»

## 11. Cue, classi e condotta

| predicato | ruolo |
|---|---|
| `intent_cue(Classe, "superficie")` | cue **substring** sul turno canonico (`kb_cue_match`); attenzione a «eat» ⊂ «f-eat-hers» (MANTRA #8): per le discriminanti, parola intera |
| `intent_phrase(Classe, "frase")` | frase intera |
| `phrase_canon("mi chiamo", "my name is")` | locuzione canonicalizzata prima della lettura (`lexicon.p0`) |
| `spelling_of(Errata, Giusta)` | refusi insegnati parlando (`spelling.p0`) |
| classi `*_lex*`, `*_cue*`, `*_chain*` con nome seriale | **debito**: sono `strcmp` con un altro indirizzo; una classe prende il nome del suo **ruolo** (MANTRA #19 a) |
| `turn_pattern(Forma, cue\|not_cue\|word\|text\|not_text\|number, Arg)` + `turn_pattern_intent(Forma, Intento)` | la **congiunzione** come regola KB (MANTRA #19 b); valutata dentro `kb_cue_match` |
| `turn_pattern_force(Forma, Forza)` + `turn_pattern_match(Turno, Forma)` | la **forza** insegnata parlando (`illocution.p0`): il C valuta le forme con una forza a inizio turno sul turno canonicalizzato e pubblica il match; `turn_declared_act` lo legge. Lezioni: «a turn that contains "X" [but not "Y"] is a question», «forget that …», «un turno che contiene "X" è una domanda» |
| `turn_declared_act(Turn, Atto)` | atto dichiarato del turno dalla IR (`turn-frames.p0`): `production_request`, `own_procedure_request`, `lesson_turn`, `prose_carried`, … |
| `turn_illocution(Turn, question\|expressive\|prose_carried\|…)` | forza del turno; **asserita dal C a inizio turno** enumerando `turn_declared_act`, quindi congelata: una regola che vuole cambiarla (es. escludere `prose_carried`) deve leggere `turn_surface_token`, che esiste allora, non `turn_span_token`, che arriva con la segmentazione; `naf(turn_illocution($T, question))` è la guardia standard per «non è una domanda» |
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
7-bis. **Una vista data a `answer_frame/2` viene chiamata anche con l'argomento
   LIBERO** (il consumatore in C prova la relazione in più modi). Il `.p0` non
   ha un test di variabile legata, quindi il primo goal lega l'argomento a ciò
   che il turno nomina (`belief_named/1`, `function_subject/2` in
   `function-questions.p0`). Senza quel goal, la vista enumera ogni entità della
   KB (26 settembre: 25 s e timeout).
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
15. **Un nome nuovo si cerca prima con `grep`** (MANTRA #5): `role_name/1` e
    `role/3` sono la macchina delle relazioni (`procedures.p0`), `tok`,
    `relation`, `about` idem; un predicato omonimo esteso da un altro file non
    dà errore, dà un comportamento nuovo a chi lo enumerava (misurato: una
    lezione di parafrasi appesa oltre 60 s per `decision_formula` di base
    letta come ruolo di relazione). Prefisso di famiglia sempre
    (`decision_role_name`, `ev_tok`).
16. **Nomi:** `decision_*` per il circuito, `_in` per la variante con
    accumulatore/lista, `_words`/`_text`/`_render` per la resa,
    `_lesson`/`_forget`/`_question` per le forme.
17. **Misure con la data.** «L'abbiamo misurato» vale solo con quando e dove.
18. **Il commit dice il bilancio**: quante righe C, quante KB (#18 a).

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
| una sonda `? pred a b` con tutti gli argomenti legati dà `total: 0` | conta i BINDING, non le prove: con zero variabili libere è sempre 0; lascia un `_` |
| una lettura vede i token nel contabile ma non nella forza del turno | usa `turn_surface_token` (esiste a inizio turno), non `turn_span_token` |
| una lezione che prima diceva «Held: …» ora dice «Learned: …» e `who answered?` dice `negation` | fatti nuovi non dichiarati `machinery`/`turn_scratch` nel giornale recente |
| un turno resta appeso oltre il budget dopo l'aggiunta di FATTI di base | un predicato omonimo di un altro file (`role_name`, `role/3`) ora enumerabile: rinomina con prefisso di famiglia; bisezione a varianti del file |
| un test rosso solo per `turn took 1.2s (timeout 1.00s)` | costo del turno base (`TEST_TODO.md`), non del cambiamento; non si alza il budget |
| «Held: for K, N is «…»» ma `apply K to …` declina o dà un risultato parziale | un passo che il motore non sa (§17.1): `rule for` non valida, l'esecuzione salta in silenzio; `/debug` mostra i `turn_plan_step` eseguiti |
| un'istruzione «start at N. if it is even … repeat until …» risponde «I couldn't read …» e impara un fatto strano | il turno è stato spezzato in frasi prima del registro (§17.2); una frase sola, o niente `. ` fra le clausole |
| una lezione «if … then …» dice «Learned rule» ma la polare non cambia | premesse diventate `holds(atomo)` (§17.4): variabili fuori da `rule_variable`, o premessa «the R of x is h» / confronto |
| un fatto appreso contiene un'entità dei turni precedenti al posto di «it» | `coref_resolve` post-dispatch ha riscritto il pronome e rilanciato il registro (§17.2) |

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

## 16. Contenuti, contesti e prove — contratto progettato (20 settembre 2026)

**Stato: progetto, non implementazione.** Il piano autoritativo, la baseline
e l'ordine M0–M5 sono nell'[HANDOFF della lettura della
prosa](plans/lettura-della-prosa.md#handoff--20-settembre-2026-contenuti-atti-e-giudizi-sostenuti).
Questa sezione impedisce di confondere il modello desiderato con ciò che il
loader e il solver fanno oggi. Non aggiungere direttive o builtin operativi
copiando nomi da questo progetto.

### 16.1 Quello che la sintassi corrente esprime davvero

```prolog
% Esempi di rappresentazione, NON lezioni da promuovere nella KB viva.
context(studio_alfa, hypothesis).
holds_in(studio_alfa, proposition(frame(colore, roles(oggetto, bianco)))).
```

I termini composti sono dati unificabili. `holds_in/2` e le viste di
`context-scope.p0` permettono di conservarli e interrogarli. **Inserire quel
termine non esegue `colore(oggetto, bianco)`**, né realizza automaticamente un
ragionamento contestuale. `context_parent/2` oggi alimenta la vista delle
credenze ereditate: non va assunto come semantica universale dell'importazione
di assiomi.

Una regola nativa `p($X) :- q($X).` viene eseguita dal solver. Il testo di
quella regola conservato in una stringa è invece testo. Non sono intercambiabili.
`kb_rule/2` espone le teste e `kb_rule_body/2` i nomi dei predicati del corpo.
**Dal 20 settembre 2026 (M1)** `kb_clause/4` espone la clausola integra — testa,
premesse ordinate, polarità (`not(E)`, `naf(G)`) e legami — in forma canonica
taggata a ogni livello (`var`/`atom`/`app`), con identità `content(Pred,
impronta)` calcolata sulla struttura intera. `kb_clause_arg/4` la legge per nodi
e archi quando un pezzo non entra in un termine. Vedi la tabella dei builtin in
§6 e `kb/core/clause-content.p0`. Il corpo è dato per archi (una premessa per
riga) perché una lista unica non reggerebbe i corpi lunghi.
**Variabile del solver ≠ variabile menzionata.** `$X` anche dentro un termine
composto resta una variabile attiva. Un fatto che la contiene è una unit clause
standardizzata a parte a ogni uso, non una citazione di una variabile chiamata
X. In M1 il nodo è `var(N)` dentro una forma taggata a ogni livello
(`var`/`atom`/`app`), con la clausola come binder (chiusura universale): un dato
ground, che si unifica strutturalmente e non si istanzia da solo, e che non si
confonde con un termine ordinario scritto `var(0)`. L'istanza di esecuzione
resta quella che il risolutore produce rinominando la clausola a ogni
applicazione. Quantificatori annidati, astrazioni e citazioni sono legami che
questo frammento **non** ha ancora.

### 16.2 Il contratto della rappresentazione da costruire

La seguente è **notazione di progetto**, non grammatica `.p0`:

```text
Espressione  = operatore + argomenti ordinati + legami delle variabili
Atto        = identità + espressione + contesto + forza + fonte/agente
Giudizio    = contesto + espressione sostenuta
Derivazione = giudizio concluso + regola + istanziazione + dipendenze
```

- Un contenuto ha identità indipendente dalle sue occorrenze. Due fonti
  possono affermarlo e una sola ritrattarlo; deduplicare il contenuto non
  autorizza a deduplicare gli atti.
- Applicazione, congiunzione, implicazione, quantificazione e negazione
  devono conservare la propria struttura. I quantificatori legano occorrenze
  di variabili; i nomi degli individui non diventano testimoni per convenzione.
- La stessa espressione può essere citata, assunta, negata o provata. La
  citazione di P non implica P, né implica automaticamente che chi cita
  creda P. La buona formazione di P non prova la sua verità o soddisfacibilità.
- Gli assunti sono riferimenti. Un teorema conserva le dipendenze rimaste
  dopo lo scarico delle assunzioni locali: chiudere lo scope non trasforma la
  conclusione locale in fatto globale.
- Le dipendenze di UNA derivazione sono congiunte; derivazioni distinte
  dello stesso giudizio sono alternative. Non sostituire questo grafo con
  una lista unica di nomi di predicati.
- `naf` non è negazione esplicita. Il suo eventuale certificato deve portare
  ambito e completezza della ricerca; una guardia non autorizza una negazione.

Identità e archi consentono relazioni entro arità 4 e termini entro 512 byte.
L'eventuale serializzazione deve segnalare un limite, mai troncare una formula
o un certificato facendolo passare per intero. Le identità di clausola non
possono essere indici mobili del vettore dei fatti. Va dichiarato se durano
una sessione oppure sopravvivono al salvataggio e alla rilettura.

### 16.3 Cosa NON certifica una prova oggi

Misurato con `bash tests/probes/kb_abstraction_probe.sh` sul profilo completo:

- `kb_prove_support` restituisce il sostegno della prima spiegazione; non
  certifica che non esistano prove alternative ammissibili.
- Le regole diretta e inversa con gli stessi predicati collassano nella
  medesima riga testuale; il limite interno è di 24 sostegni.
- `machinery/1` può togliere sostegni dalla rappresentazione. Una prova con
  elenco vuoto non è per questo indipendente dalle conoscenze del mondo.
- Ripetere un fatto sotto un'altra origine non crea una seconda occorrenza
  interrogabile: il fatto già noto viene deduplicato.

Perciò né il giornale né le righe di supporto correnti sono il formato da
estendere a teoremi e citazioni. Servono come diagnostica della baseline.
La nuova prova dovrà essere prodotta dalla ricerca che decide, conservare
gli effetti del backtracking e distinguere completamento da interruzione.

M1 (clausola integra e legami) è stata riaperta dalla revisione del 20 settembre
2026 per tre difetti dell'astrazione — variabile rappresentata e termine
ordinario collassati, identità calcolata su un testo troncato, riga con testa
in overflow non più ritrovabile — ed è ora corretta e verificata:
`kb_clause/4` e `kb_clause_arg/4`, `kb/core/clause-content.p0`,
`tests/p0t/reasoning/clause_content.p0t` (52 assert, i tre controesempi
inclusi). **M2 è conclusa:** `kb_derivation/4` e `kb_act/3`,
`kb/core/derivation.p0`, `tests/p0t/reasoning/derivation.p0t` (50 assert) —
la prova esce dalla ricerca che decide, con AND fra i passi e OR fra le
derivazioni, e un contenuto può avere più atti. Il caso Zelvo resta aperto:
`kb_prove_support` e `supported_by_premises` non sono stati toccati. Seguono
M3 (ammissibilità KB) e M4 (producer IR e migrazione di un lettore). Ogni
costrutto diventato eseguibile va spostato dalla descrizione progettata alla
sezione operativa pertinente, con un test e un esempio realmente verificati.

## 17. Le procedure — tre motori che non si parlano (letto nel C il 26 settembre 2026)

Nato dalla sessione live sulle procedure per nome
([sessions/live/2026-09-26-procedure.log](sessions/live/2026-09-26-procedure.log)):
«conosci Collatz? no → te lo spiego → dimmi il valore» **non si può fare oggi**,
e il motivo è che le procedure vivono in tre meccanismi separati, nessuno dei
quali chiama l'altro. Chi scrive KB per le procedure deve sapere in quale dei
tre sta scrivendo.

### 17.1 La procedura nominata su testo — `proc_step/3`

| fatto / forma | dove | che cosa |
|---|---|---|
| `proc_step(Nome, N, "passo")` | `procedures.p0` (`machinery`), dichiarato in `messages.p0` | il passo N della procedura Nome, **come testo** |
| `char_class(Classe, c)` | `messages.p0` (`vowel`, `consonant`, `digit`) | le classi di caratteri: l'**unica** parte del toolkit che è KB |
| `teach_proc` = `text("rule for") slot(key) text("is") rest(text)` → `assert_ordered` | `messages.p0` | accoda il passo al primo indice libero (1..64) |
| `run_proc` / `run_proc_it` = `apply K to …` / `applica K a …` → `run_procedure` | idem | esegue i passi 1..32 in ordine, profondità di chiamata 6 |
| `list_procs` (`what procedures do you know`) → `list_keys`; `forget_proc` → `retract_ordered`; `fix_step` (`step N of K is now …`), `drop_step` (`remove step N of K`) → `op(retract_all, proc_step, [key, order, free])` (+ `assert`) | idem | manutenzione |
| `teach_charclass` = `the K letters are …` → `assert_many` in `char_class` | idem | una classe nuova, parlando |

**Gli operatori e le condizioni sono compilati** (`p0_apply_op`,
`p0_cond_holds` in `10-memory-knowledge.c`), `strcmp` sulla prima parola del
passo: `keep C` · `drop C` · `reverse` · `count` · `upper` · `lower` · `first [N]`
· `last [N]` · `sort` · `unique` · `split on c` · `join with c` · `replace a with
b` · `apply P` · `if <cond> then <passo>` · `repeat <passo> until stable|<cond>`
(tetto 64 giri); condizioni `empty` · `any` · `has C` · `length N` · `shorter N`
· `longer N` · `starts T` · `is T`. **Nessuna aritmetica**: gli unici numeri sono
lunghezze e conteggi di `first`/`last`. È la lista di parole nel C che il mantra
#2 vieta, ed è nel `C_TODO`: un operatore nuovo oggi è una ricompilazione.

Trappole misurate:

- **`rule for` non valida.** Qualunque `rest(text)` diventa un passo («Held: for
  collatz, 1 is «start at the number»»). All'esecuzione un passo che il motore
  non sa viene **saltato in silenzio** (`return 0` → `continue`): se un altro
  passo è girato la procedura risponde con un risultato parziale, se nessuno è
  girato la forma declina e il turno cade altrove («I don't know about collatz
  yet»). Chi insegna non viene avvisato in nessuno dei due casi.
- **Le classi si confrontano byte per byte**, in minuscolo: una vocale accentata
  (`à`, due byte) o un membro di due caratteri (`sh`) non combaciano mai, anche
  se stanno in `char_class`.
- **`why?` dopo `apply`** dice solo «N applied step(s) of K gave V»: la traccia
  passo per passo va in `turn_plan_step` ed è visibile solo con `/debug`.
- Il lettore delle forme rinuncia sopra i **300 caratteri** o le **48 parole**.
- Le forme di procedura non hanno `turn_form_priority(_, early)`: le legge
  `mod_knowledge`, tardi nel registro, **dopo** `teachrule`, `arith`, `agent`.
  Un turno che somiglia a un'altra cosa viene preso prima da un'altra facoltà.
- Solo `apply`/`applica` ha la forma italiana: `rule for`, `step … is now`,
  `remove step`, `forget the procedure` sono solo inglesi; e «forget the
  procedure» si sovrappone alla cue `procedure_forget_cue("forget the procedure
  from")` di `assisted-learning.p0`.

### 17.2 Il ciclo numerico a rami — `mod_agent`, anonimo

`60-agent-tools.c` (gen116/117) esegue a parole un ciclo con rami di parità: il
vocabolario è KB — `agent_branch_step(Superficie, Op, Fattore)` (`double ×2`,
`triple ×3`, `halve /2`, `add`/`plus`/`subtract`/`minus`/`multiply`/`times`/
`divide` + numero, e gli italiani) e `agent_parity_marker(even|odd, Superficie)`
in `procedures.p0` — ma la procedura **non ha nome, non si salva e non si
richiama**: la funzione non contiene alcun `kb_assert`; l'unico effetto è
`store_proof`. Il prompt che funziona nei banchi (`tests/p0t/agent/agent_branch.p0t`):

```text
> start at 27. if it is even, halve it. if it is odd, triple it and add 1. repeat until it reaches 1.
< Reached 1 after 111 steps…
```

Condizioni della superficie: meno di **256 caratteri**; una cue di partenza
(`start`, `begin`, `parti`, `inizia`, `comincia` — `intents.p0`); una frontiera
trovata con `strstr`: `until`, `finch`, `fino a`; i due marcatori di parità
(substring). Il valore iniziale è il primo numero prima del primo marcatore, il
bersaglio il primo numero dopo `until`; arresto sull'uguaglianza, tetto 10⁶.

**Trappola del turno spezzato.** Prima del registro, `compound_turn_lead`
spezza un turno con `sentence_boundary_cue` (`". "`, `"! "`, `"; "`) e senza
`?` e manda **ogni frase da sola** in `brain_respond`; il turno intero torna al
registro (dove sta `mod_agent`) **solo se nessuna frase è stata letta**. Nel
profilo `base` dei test tutte murano e il ciclo parte; nel profilo `agi` una
frase viene letta e il ciclo non gira mai: «I couldn't read «start at 6» … Learned:
repeat until shout reach 1». I prompt a una frase di `agent.p0t` («start at 3
and double until you reach 50») aggirano il problema. Qui «shout» non è una
lacuna pendente: è `coref_resolve` (post-dispatch) che sostituisce il primo
`entity_pronoun` («it») con l'ultima `entity_mentioned` e **rilancia il registro**
sulla frase riscritta, che un lettore salva come fatto senza chiedersi se ha
senso; al turno dopo lo stesso meccanismo produce «repeat until repeat until…».

### 17.3 Le procedure numeriche scritte a mano — senza consumatori

`factorial/2`, `fib/2` (solo i casi base), `gcd/3`, `lcm/3`, `is_prime/1`,
`power/3` stanno in `procedures.p0` (§ MATH PROCEDURES) ma **nessuno le
chiama**: «What is the factorial of 6?» → 720 lo calcola `20-math.c`
(`find_token "factorial"/"fattoriale"`, n ≤ 20), i primi `arith_is_prime`.
L'unica consumata è `choose/3` (`hypergeom_term`). La via KB verso i numeri è
`numeric_cue(Frase, Op)` → `apply_numeric(Op, Lista, R)` (`numeric-questions.p0`:
greatest, least, ascending, median, remainder, gcd, lcm) e `infix_operator/2` +
`apply_operator(Op, A, B, R)` (`gcd_op`, `avg_op`, plus/minus/times/divide) —
nessuna delle quali ha una `turn_form`: una procedura numerica nuova **non si
insegna parlando**. La terza via, `procedure_teach_cue("to convert")` in
`assisted-learning.p0` (`learning_candidate(procedure(Da, A), …)` → `apply_operator`),
è lineare — niente cicli, niente rami, nome legato alla coppia di unità — e il
suo protocollo di risposta `turn_learning_response/2` non ha consumatore nel C.
«Do you know the factorial?» non ha forma (`knowledge_head` copre solo «do you
know about / anything about»): finisce nello smalltalk.

### 17.4 Il lettore delle regole «if … then» — che cosa può essere una premessa

`mod_teach_rule` (`10-memory-knowledge.c`): `w[0]` ∈ `rule_antecedent_marker`,
un `rule_consequent_marker` dopo; l'antecedente si spezza sulle congiunzioni.
Ogni clausola tenta tre letture, in ordine:

1. **tipizzata** — `V is the R of V2` (arità 2) o `V is a C` (arità 1, la classe è
   l'ultima parola); il soggetto deve essere un termine di regola;
2. **frame** — almeno due `rule_variable`/`rule_anaphor` e un frame a due slot:
   «x contains y» → `contains($V1, $V2)`;
3. **atomo opaco** (attivo per `propositional_conditionals(on)`): ≤ 8 parole
   alfabetiche, articoli tolti → `holds(slug)`.

Le variabili sono **solo** `someone/anyone/something/qualcuno/chiunque/qualcosa/
x/y/z` (`grammar.p0`): `h`, `n` sono costanti. Perciò «if the minimum height of x
is h and n is greater than h then x is required at n» diventa
`holds(x_is_required_at_n) :- holds(minimum_height_of_x_is_h), holds(n_is_greater_than_h)`
e viene annunciata «Learned rule»: il controllo di sicurezza guarda solo le
variabili `$`, e una regola di soli atomi passa. Limiti duri: arità ≤ 2 per
goal, nessuna negazione, **nessun confronto, `is/2` o aritmetica**, ≤ 8 variabili.
Una soglia («serve la protezione a 3 metri?») non è esprimibile come regola detta.

### 17.4-bis Stato dopo la notte del 26 settembre — l'interprete è KB

Scritto dopo lo studio qui sopra, nella stessa notte: `p0_cond_holds`,
`p0_apply_op`, `p0_run_proc` **non esistono più**. L'interprete sta in
`procedures.p0` («LE PROCEDURE NOMINATE»): `proc_run(Nome, In, Out)` →
`proc_from/4` sui passi in ordine → `step_term(Testo, Termine)` →
`run_step(Nome, Termine, V, V1)`. I termini di un passo: `op(Op, Args)` (testo),
`num(Op, N)` (numerico, via `agent_branch_step/3` + `apply_operator/4`),
`if(C, S)`, `ifelse(C, S, E)`, `seq(A, B)` («and»), `repeat(S, C)`; le
condizioni: `parity(even|odd)`, `compare(eq|lt|gt, N)`, `text(empty|any)`,
`has(C)`, `length(Cmp, N)`, `starts(T)`, `is(T)`, `stable`. Un passo nuovo è una
clausola; una parola nuova per un passo è una riga (`proc_operator/2` o
`agent_branch_step/3`); `step_surface/1` è l'elenco che parrot0 dice a chi
insegna un passo illeggibile (rifiutato alla lezione: `step_readable/1`).

Tre primitive del solver sotto: `chars/2`, `is/2` e la nuova **`iterate(Passo,
Arresto, In, Out)`** — rifà `Passo/2` finché `Arresto/1` regge, a profondità
costante e a carico del budget; il ciclo in KB usa `loop_step/2` e `loop_done/1`
su un valore `loop(Nome, Passo, Cond, V)`. Il **tetto di profondità** (64) è ora
una specie propria del registro dei paradossi, `paradox_event(proof, depth, Pred,
seen(Turno, Goal))`, con `KbInferenceReport.depth_hit` e lo stadio
`depth_reached` in `composition.p0`.

Trappole nuove, misurate: (1) un numero **fra virgolette** non è un numero per
`is/2` ed `eq/2` (`eq(mod("6", 2), 0)` fallisce): `proc_value/2` spoglia la
parola sola all'ingresso; (2) `atom_words/2` spezza anche sugli underscore, quindi
«apply collatz_step» arriva come due parole e il nome si ricompone con
`atom_words($Q, $Rest)`; (3) `assert` dentro una prova lanciata da `kb_match`
(KB a sola lettura) fallisce: la traccia è best-effort; (4) **il demone dei test
tiene il binario vecchio**: dopo un rebuild si rilancia `make test-engine`, o
ogni prova sul C è un falso. Aperto: chi rivendica il turno prima delle forme
(«rule for … repeat …» va allo smalltalk), e il budget di `iterate` che non
raggiunge il report — handoff in [plans/live-teaching.md](plans/live-teaching.md).

### 17.5 Che cosa manca per «te lo spiego e mi dici il valore»

Il ponte è uno: dare al ciclo numerico un **nome** (`rule for collatz is …` con
passi che riusano `agent_branch_step` e `agent_parity_marker`, così il
vocabolario resta KB) e far sì che `run_procedure` sappia eseguire un passo
numerico — cioè un interprete generico che consulta `apply_operator/4` invece
della `switch` di `strcmp`. La rotta è in
[plans/teachable-procedures.md](plans/teachable-procedures.md) §2.3 (lo strato I,
«l'unico C che cresce»), che però oggi non nomina nessuno dei tre motori qui
sopra: la sezione va riallineata prima di costruire.

## 18. `/debug` e la profondità della traccia (26 settembre 2026)

Richiesto da F.: `/debug` da solo è la **guida**; ogni azione ha un verbo;
accendere il profilo è esplicito; le tracce di diagnosi **non si tolgono**, si
spengono per profondità.

| comando | che cosa fa |
|---|---|
| `/debug` · `/debug help` | la guida, con lo stato (profilo, profondità) in testa |
| `/debug on` · `/debug off` | il profilo per turno: tempi, passi, strada, e le **visite ai fatti per goal ← regola** |
| `/debug turn` | l'ispettore dell'ultimo turno: nota, anatomia, sonde KB |
| `/debug trace [parola]` | la traccia del turno fino alla profondità corrente; le righe più profonde si contano |
| `/debug depth N` | la soglia (default `debug_trace_depth(1)` in `debug.p0`) |
| `/debug pred NOME[/ARITÀ]` | regole e fatti di un predicato, con i primi valori |
| `/debug dump` | lo stato completo dell'ultimo turno su file |

In un `.p0t` gli stessi verbi con `!debug` (`!debug` da solo resta l'ispettore
con il profilo acceso).

**La guida e la profondità sono KB** (`kb/core/debug.p0`): `debug_help_line/2`
(un comando nuovo è una riga), `debug_trace_depth/1`, e `trace_stage_depth(Specie,
N)` che porta una specie intera di righe a un altro livello senza ricompilare
(`read.named` a 3, `iterate` a 4).

**Chi scrive C** usa `p0_trace(b, specie, …)` per il filo del turno (livello 1)
e `p0_trace_at(b, livello, specie, …)` per il dettaglio: 2 = il perché di un
cancello e il costo di un contabile; 3 = i passi interni di un lettore; 4+ = i
giri di un ciclo. Una traccia messa per capire un problema **resta**, al suo
livello: la prossima volta serve di nuovo. Il motore della KB scrive con
`kb_trace(kb, specie, …)` (livello 1, spostabile con `trace_stage_depth`).

**Lavorato: il turno da 7,9 s.** Il profilo diceva `turn_after_reply 6,7 s, 274
passi`. Con le nuove righe: la traccia a livello 2 ha nominato il contabile
(`contact_near_way: 6703 ms, 9 100 608 visite`) e le visite per goal ← regola il
goal (`relation_verb`). La cura era un cancello di conoscenza, non un'ottimizzazione:
il contatto insegna solo con un'affermazione, e il turno era una domanda. Il
turno è sceso a 1,2 s. Il costo base che resta, `relation_verb ← verb_finite_form`
con circa 2 000 cammini dell'intera classe per turno, è il prossimo bersaglio.
