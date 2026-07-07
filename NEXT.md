# NEXT — punto di ripartenza + piano upfront del lavoro in corso

> Aggiornato a gen281 (2026-07-07). Questo file è il **piano upfront**: se stacco a
> metà, ogni modifica non committata deve essere riconducibile a quanto scritto
> qui. Contesto: `docs/plans/teach-comprehension-via-mcp.md` §5.5 (sequenza
> U1..U6) e §6 (design per superare D.1/D.2). `docs/prolog-like-engine.md` §1-2.

## Spedito

- **gen279 — U1** fedeltà letterali via MCP (quotatura al bordo). `270beec`.
- **gen280 — U1b gen A** `$` variabile esplicita (dual-accept). `aa67d76`.
- **gen280 — fix** quota anche i `$`-letterali; doc allineate. `8d9951b`.
- **gen280 — docs** Secchio D (limiti misurati) + §6 design per superarli.
  `f779d6c`, `087c637`.
- **gen281 — U2** `kb.assert_clause` (regole n-arie via MCP). `0bfebf8`.
  `tests/assertclause.sh` verde; doc allineate.
- **gen282 — U6** negazione-per-fallimento `naf(G)` → **D.2 superato** (default con
  eccezioni). `Term.neg`, NAF in `solve()`/`prove_seq_ex()` con floundering-guard,
  `"neg":true` via `kb.assert_clause`, `kb_save` round-trippa `naf`. Gate
  `tests/naf.sh` (via `.p0` e via MCP). Suite verde. `e7e64b3`.
- **gen283 — U3** unificazione STRUTTURALE su termini composti → **D.1 superato**
  (computazione ricorsiva: Peano, liste). `split_compound`+`unify` ricorsivo,
  `rename_arg` su var annidate, `deep_resolve` per l'output, `parse_term` depth-aware,
  `looks_compound` al bordo MCP. Gate `tests/compound.sh` (via `.p0` e via MCP).
  Suite verde. `e8cb976`.
- **gen284 — U1b gen B** flip `is_var` a **solo-`$`** (fine dual-accept, richiesta F.).
  Maiuscola = costante; `kb_match` "Q"→"$Q"; 5 fixture migrati. Gate `dollarvar.sh`.
  Suite completa verde. `dd427e5`.
- **gen285 — U4** azioni-su-stringa COME CONOSCENZA. Builtin `chars/2` (atomo↔lista-
  di-char, l'unico pezzo fisso cieco all'operazione); `capitalize_first` è una REGOLA
  + tabella `upper/2`. `cap_first(madrid,$R)`→`$R=Madrid`. Gate `tests/strknow.sh`
  (`.p0` + MCP). Suite completa verde.
- **gen286 — U5 (prima regola-colla)** l'accordo dell'articolo IT come CONOSCENZA.
  Tabella `article(Def, Gender, Vowel, Form)` in `kb/core/grammar.p0` (nuova casa
  del Secchio B); `mod_translate` la interroga invece del cascade di ternari C.
  Substrato fisso invariato (determinante/genere/vocale); migrata solo la SELEZIONE
  della forma (incl. elisione `l'`/`un'`, che vive SOLO nella tabella). `article`
  filtrato come substrato in `is_internal_pred`/`is_struct_pred`. Gate
  `tests/article.sh` (regressione traduzione + `kb.match article` via MCP: grammatica
  ispezionabile). Suite completa verde.

---

## (chiuso) U6 — negazione-per-fallimento (`naf`) → D.2 superato, gen282

Spedito. Sezione tenuta come record del design; il prossimo lavoro è U3 (CODA).

<details><summary>Piano U6 (storico)</summary>

**Perché.** Insegnare un DEFAULT con eccezioni come conoscenza: "gli uccelli
volano, tranne i non-volatori". Oggi non è esprimibile (clausole di Horn definite,
teste positive). Con un goal `naf(G)` nel corpo — "G non derivabile" — diventa:
```prolog
flies($X)    :- bird($X), naf(abnormal($X)).
abnormal($X) :- flightless($X).
```
→ `flies(eagle)` true, `flies(penguin)` false. Design in
`teach-comprehension-via-mcp.md` §6.2.

**Gate (rosso→verde).** `tests/naf.sh`: carica il default sopra (via `.p0` e via
MCP `kb.assert_clause` con un goal negato), verifica `flies(eagle)` true e
`flies(penguin)` false. Oggi la seconda è true (naf ignorato) → rosso.

**Idea chiave (evita la dipendenza da U3).** NON serve parsare `naf(G)` come
termine composto: si **striscia il wrapper `naf(...)` al parse/assert time** e si
marca il goal interno (una normale predicazione `abnormal($X)`) con un flag
`neg`. Il solver, quando incontra un goal con `neg`, fa la NAF.

**Passi (atomici):**

1. **`src/kb.c` — `Term`**: aggiungere `int neg;` (default 0 via il `memset` di
   `term_make`; facts/teste restano 0). `rename_term` copia `dst->neg = src->neg`
   così il flag viaggia nel resolvent.
2. **`src/kb.c` — parser `.p0`** (rule branch, ~riga 815): per ogni goal-string,
   se ha forma `naf(<inner>)` (rispettando le parentesi), parsare `<inner>` come
   Term e settare `neg=1`. `split_goals` già rispetta la profondità di parentesi.
3. **`src/kb.c` — `solve()`** (~riga 380): se il goal corrente ha `neg`:
   - risolvi i suoi argomenti sotto la Subst corrente;
   - **floundering guard**: se NON è ground → il ramo fallisce (declino onesto,
     niente NAF su variabili libere);
   - provalo con un solve annidato (`Solver` fresco, `qvar=NULL`); se **fallisce**
     → naf riesce, continua col goal successivo; se riesce → naf fallisce.
   - la guardia `KB_MAX_DEPTH` (già presente) copre ricorsione/cicli.
4. **`src/kb.c` — `prove_seq_ex()`** (~riga 509, il prover della spiegazione):
   rispecchiare la stessa logica `neg` così `kb_explain` resta onesto sui default.
5. **`src/kb.h` + `src/kb.c`** — `KbGoal` gains `int neg;`; `kb_assert_clause`
   copia `r.body[i].neg = body[i].neg`. (le init in mcp.c zero-inited restano ok.)
6. **`src/mcp.c`** — nell'handler `kb.assert_clause`, leggere `"neg":true` di un
   body goal e settare `body_goals[nb].neg`. Documentare nello schema.
7. **`tests/naf.sh`** — il gate (via `.p0` e via MCP); aggiungerlo al `Makefile`
   dopo `assertclause.sh`.
8. **Verifica**: gate verde + regressione (`explain.sh knowledge.sh multigoal.sh
   persist.sh` + `make test`). **persist.sh** tocca la negazione ground esistente
   (`neg` list / `kb_is_negated`) — deve restare verde: `naf` è ORTOGONALE ai
   fatti negativi ground, non li tocca.
9. **Doc**: `prolog-like-engine.md` §2 (naf nel contratto), `use-mcp-engine.md`
   (campo `neg`), `teach-comprehension-via-mcp.md` §6.2 (U6 ✅). Commit
   `feat(engine): gen282 - U6 naf`.

**Onestà/limiti (nel commit e nei doc):** `naf` copre "P salvo eccezione" con NAF
stratificata a goal ground; NON copre priorità fra default né probabilità (resta
fuori, altro motore). Stratificazione non verificata staticamente: ci si affida a
ground-guard + depth-guard.

</details>

---

## (chiuso) U3 — unificazione STRUTTURALE → D.1 superato, gen283

Spedito. Sezione tenuta come record del design; il prossimo lavoro è la CODA
(U1b gen B, oppure U4/U5 sopra U3).

<details><summary>Piano U3 (storico)</summary>

## (storico) U3 — unificazione STRUTTURALE (termini composti) → supera D.1

**Perché.** Insegnare una COMPUTAZIONE ricorsiva come conoscenza (Peano, liste,
`length`/`reverse`), oggi impossibile perché `s(z)` è un atomo piatto. Design in
`teach-comprehension-via-mcp.md` §6.1.

**Idea intelligente (minima).** NON un nuovo datatype: lo storage a stringa già
contiene `s(z)`; si insegna a `unify` a **vederci la struttura**. Un arg di forma
`f(a…)` è composto; `unify` ci ricorre; una variabile si lega a una sotto-struttura
intera (stringa). Riusa `parse_term` come splitter.

**Gate (rosso→verde).** `tests/compound.sh`: via `.p0` E via MCP `kb.assert_clause`
- Peano: `add(z,$Y,$Y). add(s($X),$Y,s($Z)):-add($X,$Y,$Z).` →
  `kb.match add(s(z),s(z),?)` → `["s(s(z))"]`; `nat(s($N)):-nat($N)` →
  `nat(s(s(z)))` true.
- Liste: `len(nil,z). len(cons($H,$T),s($N)):-len($T,$N).` →
  `len(cons(a,cons(b,nil)),?)` → `["s(s(z))"]`.
Oggi tutto falso → rosso.

**Passi (atomici, in `src/kb.c` salvo dove indicato):**

1. **`parse_term` comma-split depth-aware** (~riga 721): splittare gli argomenti
   sulle virgole SOLO a profondità-parentesi 0, così `cons(a, b)` è UN argomento,
   non due. (Oggi splitta su ogni virgola → rompe i composti n-ari.) Le virgolette
   sono già gestite. Serve anche il forward-decl di `parse_term` prima di `unify`.
2. **`unify` strutturale** (~riga 325): dopo i casi var, se entrambi non-var,
   splittare con `parse_term`; se ENTRAMBI composti con stesso funtore/arità →
   `unify` ricorsivo sugli argomenti; se uno solo composto → fail; se entrambi
   atomi → `strcmp`. Nessun occurs-check (come Prolog standard); la ricorsione è
   limitata dalla dimensione del termine.
3. **`rename_arg` ricorsivo** (dentro/accanto a `rename_term`, ~riga 352):
   rinominare le variabili ANNIDATE (`s($N)`→`s($N_frame)`), non solo l'arg-se-var
   top-level. Ricorsione via `parse_term`. Senza, lo standardize-apart si rompe.
4. **`deep_resolve`** (nuovo, accanto a `resolve`): sostituzione RICORSIVA delle
   variabili dentro i composti, per rendere il binding di output (`$R`→`s(s(z))`).
   Cap di profondità (`KB_MAX_DEPTH`) contro cicli (X↦s(X)).
   - usarlo in `solve()` alla raccolta soluzione (~riga 389-392) al posto di
     `resolve` per `kb_match`;
   - usarlo in `render_goal` (~riga 543) per una spiegazione onesta.
5. **`src/mcp.c` — `build_clause_args`**: un arg che "sembra composto"
   (`funtore(...)`, helper `looks_compound`) va passato as-is, NON `lit_encode`
   (altrimenti una lista `cons(a,b)` con virgola verrebbe quotata → atomo).
   Ordine: `$`-var → as-is; composto → as-is; altrimenti `lit_encode`.
6. **`tests/compound.sh`** — il gate; aggiungere al `Makefile` dopo `naf.sh`.
7. **Verifica**: gate verde + regressione COMPLETA (`make test`). Rischi maggiori:
   `parse_term` depth-aware potrebbe cambiare il parsing di fatti esistenti con
   parentesi negli argomenti (raro; controllare `run.sh`, `knowledge.sh`,
   `world-facts`). `rename_arg`/`deep_resolve` con buffer `KB_TERM_LEN`: termini
   profondi possono troncare → cap onesto, non corruzione.
8. **Doc**: `prolog-like-engine.md` §2 (composti ✅, aritmetica-via-Peano),
   `teach-comprehension` §6.1 (U3 ✅) + sintesi D.1, `use-mcp-engine.md`. Commit
   `feat(engine): gen283 - U3 structural unification`.

**Onestà/limiti:** niente occurs-check; termini oltre `KB_TERM_LEN` troncano
(cap, non crash); `naf` su goal con argomenti composti non-ground resta un bordo
(gen A: caso raro, si dichiara). Se il passo 1/2 destabilizza il parsing esistente
senza beneficio, si spezza U3 (prima solo `.p0`, poi MCP) o si valuta un `Term`
ricorsivo vero.

</details>

---

## (chiuso) U1b gen B — flip `is_var` a solo-`$`, gen284

Spedito: `is_var` = `$`/`_` soltanto; `kb_match` "Q"→"$Q"; `lit_needs_quote` senza
`isupper`; 5 fixture migrati a `$`. Gate `dollarvar.sh` GATE E (maiuscola =
costante). Suite completa verde. Il prossimo è **U4** (CODA).

<details><summary>Piano flip (storico)</summary>

## (storico) U1b gen B — flip `is_var` a solo-`$` (fine del dual-accept)

**Perché.** F. (2026-07-07): "non teniamo il dual concept, migriamo tutto a `$`".
La variabile è marcata SOLO da `$` (named) o `_` (anonima); la MAIUSCOLA torna un
atomo costante. Libera nomi propri e — cruciale per U4 — i caratteri singoli
(`M`, `A`) come costanti. Fine dell'ambiguità `Madrid`.

**Gate.** `tests/dollarvar.sh` esteso: `p($X):-q($X)` + `q(a)` → `p(a)` true; MA
`r(X):-q(X)` (maiuscola nuda) → `r(a)` **false** (X ora è costante). E
`capital(spain, Madrid)` via MCP → `kb.match` torna `Madrid` come costante.

**Passi (atomici, tutto insieme — un KB mezzo-migrato si rompe in silenzio):**
1. `src/kb.c` `is_var` (riga 104): togliere `isupper` → `s[0]=='$' || s[0]=='_'`.
2. `src/kb.c` `kb_match` (righe ~656-665): var interne `"Q"`→`"$Q"`, `"Q%zu"`→
   `"$Q%zu"`, `qvar "Q"`→`"$Q"` (altrimenti kb_match si rompe).
3. `src/mcp.c` `lit_needs_quote`: togliere il ramo `isupper` (le maiuscole ora
   sono costanti sicure; restano `$`/`_`/`looks_compound`/spazi-virgola).
4. Migrare i FIXTURE di test che insegnano regole con var maiuscole → `$`:
   `tests/restore.sh`, `tests/anon.sh`, `tests/multigoal.sh`, `tests/howknow.sh`,
   `tests/explain.sh` (X→$X, Y→$Y, Z→$Z nelle regole; `_` resta anonima).
   NB: i messaggi di display "Learned rule: %s(X)" nei moduli brain restano "X"
   (testo umano, non parsato) → i `.chat` non cambiano.
5. Verifica: gate + REGRESSIONE COMPLETA (`make test`). Le reti (multigoal/grammar/
   knowledge/explain/anon/persist/restore/howknow) sono la garanzia.
6. Doc: `prolog-like-engine.md` §1 (`is_var` solo-`$`), `teach-comprehension`,
   `use-mcp-engine`. Commit `refactor(engine): gen284 - $-only variables`.

**Rischio:** un fixture o un sito interno dimenticato → test rosso su una regola
(non crash). La rete lo becca. Audit fatto: nessuna regola `.p0` shippata usa
maiuscole nude; unico sito interno = `kb_match` "Q".

</details>

---

## (chiuso) U4 — azioni-su-stringa come conoscenza, gen285

Spedito: builtin `chars/2` (`atom_to_charlist`/`charlist_to_atom` + dispatch in
`solve()`), `capitalize_first` come regola. Gate `tests/strknow.sh`. Suite verde.
Il prossimo è **U5** (CODA) — l'ultimo della sequenza.

<details><summary>Piano U4 (storico)</summary>

## (storico) U4 — azioni-su-stringa COME CONOSCENZA (builtin `chars/2`)

**Perché.** F.: "anche l'eseguire capitalize_first deve essere conoscenza". La
`capitalize_first` diventa una REGOLA su una lista di caratteri; la mappa
maiuscole una TABELLA di fatti; l'unico pezzo fisso è la (de)serializzazione
stringa⟷lista-di-char, cieca all'operazione (§5.3/§6.1). Post-flip i char singoli
(`m`, `M`) sono costanti → rappresentazione pulita.

```prolog
upper(m, M).  upper(a, A).  ...                     % mappa = conoscenza (tabella)
cap_first($S, $R) :- chars($S, cons($H, $T)), upper($H, $U), chars($R, cons($U, $T)).
```
`chars/2` è il builtin bidirezionale: atomo↔lista-di-char (`chars(madrid, $L)` →
`$L=cons(m,cons(a,…,nil))`; e viceversa compone). `cap_first(madrid,$R)` → `$R=Madrid`.

**Gate.** `tests/strknow.sh`: via `.p0` E via MCP — insegna `upper/2` + `cap_first`,
query `cap_first(madrid, ?)` → `["Madrid"]`. Oggi `chars` non esiste → rosso.

**Passi (`src/kb.c` salvo dove indicato):**
1. Helper `atom_to_charlist(atom→"cons(m, …, nil)")` e `charlist_to_atom` (inverso),
   riusano `split_compound`. Char alfanumerici nudi; speciali quotati (limite:
   niente `"` interno). Buffer generoso + cap sulla lunghezza.
2. Builtin `chars/2` in `solve()` (dopo il caso `naf`, come goal speciale
   `pred=="chars", argc==2`): se arg0 bound → decomponi e `unify` con arg1; elif
   arg1 bound (cons-list) → componi e `unify` con arg0; else flounder→fail.
3. (opz.) rispecchiare in `prove_seq_ex` per l'explain; altrimenti dichiarare il
   limite (explain di regole con `chars` incompleto).
4. `tests/strknow.sh` + `Makefile` dopo `compound.sh`.
5. Verifica: gate + `make test`. Rischio: `chars` builtin intercetta un predicato
   utente chiamato `chars/2`? Improbabile; è un nome riservato del motore (come
   `naf`). Documentarlo.
6. Doc: `prolog-like-engine.md` (builtin `chars/2` + naf nel contratto),
   `teach-comprehension` §6.1/§5.3 (U4 ✅). Commit `feat(engine): gen285 - U4 chars`.

**Onestà/limiti:** `chars` gestisce stringhe word-like (alfanumeriche) pulite;
caratteri speciali/lunghezze estreme sono un bordo. È il primo BUILTIN (predicato
valutabile) del motore — giustificato da §5.3 come la (de)serializzazione
irriducibile e cieca-all'operazione, NON una primitiva d'azione.

</details>

---

## PROSSIMO (da fare): U5 continua — la prossima regola-colla del Secchio B

**Stato:** la PRIMA regola-colla è spedita (gen286: l'articolo IT come `article/4`).
U5 è una SEQUENZA — si migra una regola-colla per volta, gate-first, con dovere di
pivot. La casa dichiarativa esiste (`kb/core/grammar.p0`); le prossime candidate,
tutte in `src/brain/85-translate-synth-world.c`, in ordine di isolamento:

1. **Accordo dell'aggettivo** (`agree_adj`, riga ~1215 in `80-code.c`): oggi C
   (`-o`→`-a` per il femminile). Migra a una tabella/regola (es.
   `adj_agree(Gender, EndingIn, EndingOut)` o morfologia via `chars/2`+tabella).
   Gate: `the small house`→`la piccola casa` passa per la regola KB. Candidata più
   isolata dopo l'articolo.
2. **Articolo/genere FR ed ES** (`le/la`, `un/une`; `el/la`): stessa forma
   `article/4` per lingua (`article_fr`/`article_es` o una chiave lingua),
   estende la tabella senza nuovo C.
3. **Morfologia verbale** ("is sleeping"→verbo finito): più complessa, richiede
   `chars/2`+tabelle; ultima.

**Disciplina (invariata):** ogni regola col suo gate rosso→verde (un caso di
`mod_translate` che oggi passa per C dà lo stesso output via KB), regressione
multilingue (`translate.chat`/`.it`), pivot se tradisce l'emergenza senza
beneficio. **Design:** `teach-comprehension-via-mcp.md` §5.5/§6, `generative-prolog.md`.

<details><summary>Piano U5 — prima regola (articolo IT), storico, gen286</summary>

**Piano upfront (gen286).** U5 è una MIGRAZIONE del Secchio B (grammatica-colla),
non un upgrade di motore, UNA regola per volta, gate-first, con dovere di pivot
(`primitives-first-pivot-duty`). La regola PIÙ ISOLATA è la selezione dell'articolo
italiano — `il/la/un/una/l'/un'` — oggi un cascade di ternari C in
`src/brain/85-translate-synth-world.c` (righe 268-280). Migra a una TABELLA di
fatti `article(Def, Gender, Vowel, Form)` interrogata dal motore. Il substrato
FISSO resta per design (§5.2, confine irriducibile): tokenizzazione, riconoscimento
del determinante (`is_en_det`), lookup del genere (`gender/2`), rilevazione della
vocale iniziale. Migra SOLO la SELEZIONE della forma → la grammatica diventa dato
ispezionabile e correggibile via MCP.

**Il pull (non è refactor speculativo).** F. ha COMMITTATO U5 in §5.5. Il beneficio
misurabile: la forma dell'articolo, oggi invisibile (sepolta in C), diventa
CONOSCENZA interrogabile (`kb.match article(...)`) — è la realizzazione dichiarativa
di [[generative-prolog.md]] ("lingua = ultimo passaggio"). Gate B è genuinamente
rosso oggi.

**Gate (`tests/article.sh`, rosso→verde):**
- **A (regressione, via chat):** `the dog runs`→`il cane corre`, `the small house`→
  `la piccola casa`, `a white door`→`una bianca porta`, `the man reads a book`→
  `l'uomo legge un libro` (elisione). Stesso output, ora dalla tabella KB.
- **B (grammatica come DATO, via MCP):** `kb.match article(def,m,no,?)`→`il`;
  `kb.match article(def,f,yes,?)`→`l'`. Oggi vuoto → ROSSO (verificato gen286).

**Passi (atomici):**
1. `kb/core/grammar.p0` (NUOVO — la casa dichiarativa del Secchio B che si popolerà
   una regola per volta): 8 fatti `article/4` (def/indef × m/f × vowel no/yes),
   `l'`/`un'` come atomi bare (il parser `.p0` li accetta; il join C riga 306 già
   omette lo spazio dopo un `'`).
2. `src/brain/99-registry.c` (`brain_create`, dopo `gloss.p0`): `kb_load(…grammar.p0)`.
3. `src/brain/85-translate-synth-world.c` (268-280): rimpiazzare il cascade con
   `kb_match(b->kb,"article",{def,gen,vow},4,…)`. Backstop minimo (il/la/un/una,
   niente elisione) se la tabella manca, commentato come rete di sicurezza (pattern
   `kb_response`): la tabella è la FONTE DI VERITÀ.
4. `tests/article.sh` + `Makefile` (dopo `strknow.sh`).
5. Verifica: gate verde + REGRESSIONE (`translate.chat`/`translate.it.chat` in
   `run.sh`, `make test` completo).
6. Doc: `teach-comprehension-via-mcp.md` §5.5 (U5 prima regola ✅),
   `prolog-like-engine.md`, `use-mcp-engine.md`. Commit
   `feat(engine): gen286 - U5 article agreement as knowledge`.

**Onestà/limiti:** migrata SOLO la selezione dell'articolo IT; genere/vocale/
determinante restano substrato C (irriducibile, §5.2). Il backstop C duplica le 4
forme base per robustezza — l'elisione (`l'`/`un'`) vive SOLO nella tabella (davvero
migrata). Le prossime regole-colla (accordo aggettivo `agree_adj`, morfologia
verbale, FR/ES) sono i pull successivi, una per generazione, ognuna col suo gate.

**Design di riferimento:** `teach-comprehension-via-mcp.md` §5.5 (U5), §6, §5.2-5.3.
`generative-prolog.md`. Fuori scommessa (NON senza pull reale): defeasibilità con
priorità/probabilità.

</details>

## Memorie rilevanti (in `~/.claude/.../memory/`)

`parrot0-mcp-cannot-teach` (D.1/D.2 + come superarli), `parrot0-substance-presentation`
(U-sequence), `parrot0-prolog-engine-nary`, `parrot0-mcp-teach-quoting`.
