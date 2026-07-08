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
- **gen287 — U5 (seconda regola-colla)** l'accordo dell'aggettivo femminile come
  MORFOLOGIA-regola COMPOSITIVA (non phrasebook). `fem(o,a)` + `agree_f/2` +
  `swap_last/2` in `grammar.p0`: scambia l'ULTIMO char via `chars/2`+U3 (il gemello
  simmetrico di `capitalize_first`). `piccolo`→`piccola`, `bianco`→`bianca`;
  `grande` (nessun `fem(e,_)`) resta invariante. `mod_translate` interroga `agree_f`
  invece di `agree_adj` (C, ora backstop). Filtrati come substrato. Gate
  `tests/adjagree.sh` (regressione + `kb.match agree_f` via MCP). Suite completa verde.
- **gen288 — U5 (terza regola-colla)** l'articolo definito FR ed ES come CONOSCENZA.
  `article_fr(m,le)/(f,la)` + `article_es(m,el)/(f,la)` in `grammar.p0` (forma
  per-lingua, come `tr_fr`/`gender_fr` — FR/ES dipendono solo dal genere). I path
  FR/ES di `mod_translate` interrogano le tabelle invece dei ternari C. `Le chat…`,
  `El gato.`, `La casa.` invariati; forme ispezionabili via MCP. Filtrati come
  substrato. Gate `tests/artfres.sh`. Suite completa verde.

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

## PROSSIMO (da fare): U5 ultima regola-colla — la morfologia verbale

**Stato:** tre regole-colla spedite — articolo IT (gen286), accordo aggettivo
(gen287), articolo FR/ES (gen288). Resta l'ULTIMA, la più complessa:

**Morfologia verbale** ("is sleeping"→verbo finito). Oggi il C dei path FR/IT
salta o riscrive perifrasi verbali in modo ad-hoc (vedi il ramo `is`+`sleeping`→
`continue` nel path FR, riga ~74). Migrare a `chars/2`+tabelle di coniugazione è
il pull più costoso e va valutato con cura: **serve un caso di traduzione reale
che lo TIRI** (una perifrasi che oggi il C non gestisce bene), non un refactor
speculativo. Candidata di gate: una coniugazione regolare come DATO (`conj/…`) +
una regola che compone la forma finita, con un caso `mod_translate` che dà lo
stesso output via KB. Se il beneficio non è misurabile o minaccia l'emergenza →
pivot / si dichiara U5 concluso alle tre regole isolate già fatte.

**Disciplina (invariata):** gate rosso→verde, regressione multilingue, dovere di
pivot. **Design:** `teach-comprehension-via-mcp.md` §5.5/§6, `generative-prolog.md`.

<details><summary>Piano U5 — terza regola (articolo FR/ES), storico, gen288</summary>

**Piano upfront (gen288).** Migrare la selezione dell'articolo definito FR
(`le`/`la`) ed ES (`el`/`la`), oggi ternari C nei path FR/ES di `mod_translate`
(righe ~73 e ~163-165 di `85-translate-synth-world.c`), a tabelle di fatti in
`grammar.p0`. Nel codice attuale FR ed ES dipendono SOLO dal genere (niente
elisione né indefinito, a differenza dell'IT), quindi la forma naturale è
`article_fr(Gender, Form)` / `article_es(Gender, Form)` — coerente col naming
per-lingua già in casa (`tr_fr`/`gender_fr`/`tr_es`/`gender_es`), non l'`article/4`
dell'IT (che ha def+vocale). Riusa il meccanismo di gen286 (kb_match + substrato).

```prolog
article_fr(m, le).  article_fr(f, la).
article_es(m, el).  article_es(f, la).
```

**Il pull:** due lingue in più con l'articolo come DATO ispezionabile/insegnabile,
a costo bassissimo (4 fatti, due kb_match), riusando l'infrastruttura gen286.

**Gate (`tests/artfres.sh`, rosso→verde):**
- **A (regressione, via chat, env default con world-facts):** FR
  `The cat is sleeping on the warm rug.`→`Le chat dort sur le tapis chaud.` (le);
  ES `the cat`→`El gato.` (el) e `the house`→`La casa.` (la) — ES copre già
  entrambi i generi. Stesso output, ora via le tabelle KB.
- **B (grammatica come DATO, via MCP):** `kb.match article_fr(m,?)`→`le`,
  `(f,?)`→`la`; `article_es(m,?)`→`el`, `(f,?)`→`la`. Rosso oggi (predicati inesistenti).

**Passi (atomici):**
1. `kb/core/grammar.p0`: 4 fatti `article_fr/2` + `article_es/2`.
2. `85-translate-synth-world.c` FR (~riga 73): `snprintf(piece, gender=='f'?"la":"le")`
   → `kb_match article_fr({gender},?)`; backstop C se assente. ES (~163-165): idem
   con `article_es`.
3. `is_internal_pred`/`is_struct_pred`: filtrare `article_fr`/`article_es` come
   substrato (come `article`).
4. `tests/artfres.sh` + `Makefile` (dopo `adjagree.sh`).
5. Verifica: gate verde + REGRESSIONE (`translate.chat` FR, `make test`).
6. Doc: `teach-comprehension-via-mcp.md` §5.5, `NEXT.md` Spedito. Commit
   `feat(engine): gen288 - U5 FR/ES article as knowledge`.

**Onestà/limiti:** FR/ES coprono solo articolo DEFINITO + genere (esattamente ciò
che il C fa oggi); niente elisione/indefinito FR/ES (non implementati nel C, non è
questa la migrazione). `el`/`le`/`la` sono forme diverse per lingua → tabelle
separate, non un'unica `article/N` (onesto: la grammatica differisce davvero).

</details>

<details><summary>Piano U5 — seconda regola (accordo aggettivo), storico, gen287</summary>

**Piano upfront (gen287).** Migrare `agree_adj` (C in `80-code.c` ~riga 1215:
femminile `-o`→`-a`, aggettivi invarianti intatti) a una REGOLA morfologica
COMPOSITIVA — non un phrasebook per-aggettivo, ma la trasformazione stessa come
conoscenza, riusando U3 (unificazione strutturale) + U4 (`chars/2`):

```prolog
fem(o, a).                                             % la mappa desinenza (dato)
agree_f($Adj,$Res) :- chars($Adj,$L), swap_last($L,$L2), chars($Res,$L2).
swap_last(cons($H, nil), cons($H2, nil)) :- fem($H, $H2).   % base: ultimo char
swap_last(cons($H, $T),  cons($H,  $T2)) :- swap_last($T, $T2).  % ricorsione
```

`agree_f(piccolo,$R)`→`piccola`; `bianco`→`bianca`; `grande`→∅ (nessun `fem(e,_)`,
la regola fallisce → l'aggettivo invariante resta intatto). **Provato dal vivo via
MCP (gate-first, gen287): funziona sul motore attuale**, nessun upgrade necessario.

**Il pull:** la morfologia d'accordo, oggi 2 righe di C cieche, diventa una regola
compositiva ISPEZIONABILE e insegnabile (`kb.match agree_f(piccolo,?)`), e la mappa
desinenza (`fem/2`) è una tabella estensibile — la realizzazione dichiarativa di
[[generative-prolog.md]]. È l'accordo-aggettivo simmetrico al `capitalize_first` di
U4 (swap del PRIMO char): qui lo swap dell'ULTIMO.

**Gate (`tests/adjagree.sh`, rosso→verde):**
- **A (regressione, via chat):** `the small house`→`la piccola casa`,
  `a white door`→`una bianca porta`. Stesso output, ora via la regola KB.
- **B (morfologia come DATO+REGOLA, via MCP):** `kb.match agree_f(piccolo,?)`→
  `piccola`; `agree_f(grande,?)`→∅ (invariante). Rosso oggi (predicato inesistente).

**Passi (atomici):**
1. `kb/core/grammar.p0`: aggiungere `fem/2` + `agree_f/2` + `swap_last/2`.
2. `src/brain/85-translate-synth-world.c` (~riga 299): sostituire
   `agree_adj(piece, clause_gender)` con: se `clause_gender=='f'`, `kb_match
   agree_f(piece,?)`; se c'è un match usa la forma, altrimenti lascia invariato
   (l'invariante). Il maschile non tocca nulla (come oggi).
3. `is_internal_pred`/`is_struct_pred`: filtrare `fem`/`agree_f`/`swap_last` come
   substrato-grammatica (come `article`), così non inquinano l'introspezione.
4. `tests/adjagree.sh` + `Makefile` (dopo `article.sh`).
5. Verifica: gate verde + REGRESSIONE (`translate.chat`/`.it`, `make test`).
6. Doc: `teach-comprehension-via-mcp.md` §5.5, `NEXT.md` Spedito. Commit
   `feat(engine): gen287 - U5 adjective agreement as morphology rule`.

**Onestà/limiti:** copre `-o`→`-a` (la regola C esistente, femm. sing.); plurali e
altre morfologie sono pull successivi. `agree_adj` C resta come backstop (se
`grammar.p0` manca). Ricorsione `swap_last` limitata da `KB_MAX_DEPTH` (parole finite).

</details>

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
