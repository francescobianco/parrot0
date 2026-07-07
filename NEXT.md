# NEXT — punto di ripartenza + piano upfront del lavoro in corso

> Aggiornato a gen280 (2026-07-07). Questo file è il **piano upfront**: se stacco a
> metà, ogni modifica non committata deve essere riconducibile a quanto scritto
> qui. Contesto: `docs/plans/teach-comprehension-via-mcp.md` §5.5 (sequenza
> U1..U6) e §6 (design per superare D.1/D.2). `docs/prolog-like-engine.md` §1-2.

## Spedito

- **gen279 — U1** fedeltà letterali via MCP (quotatura al bordo). `270beec`.
- **gen280 — U1b gen A** `$` variabile esplicita (dual-accept). `aa67d76`.
- **gen280 — fix** quota anche i `$`-letterali; doc allineate. `8d9951b`.
- **gen280 — docs** Secchio D (limiti misurati) + §6 design per superarli.
  `f779d6c`, `087c637`.

---

## LAVORO IN CORSO: U2 — `kb.assert_clause` (regole n-arie via MCP)

**Perché ora.** È il prerequisito per insegnare DAL VIVO qualunque regola ricca:
join a più variabili (il nonno), e più avanti i goal `naf(...)` di U6 e le clausole
a termini composti di U3. Oggi `kb.assert_rule` appiattisce il corpo a unario
(`head(X):-b0(X),…`), quindi le relazioni non passano da MCP (Secchio A.2).

**Gate (rosso→verde).** `tests/assertclause.sh`: insegna via MCP
`grandparent($X,$Z) :- parent($X,$Y), parent($Y,$Z)` con `kb.assert_clause`, poi
`kb.query grandparent(tom,ann)` → true, `grandparent(tom,bob)` → false — **senza
file `.p0`**. Oggi il tool non esiste → rosso.

**Passi (atomici, ordine):**

1. **`src/kb.h`** — nuovo tipo pubblico + firma:
   ```c
   /* A goal/term: predicate + args (atoms or $-variables). */
   typedef struct { const char *pred; const char *const *args; size_t argc; } KbGoal;
   /* Assert a definite clause  head :- body[0], …, body[nbody-1]  with FULL n-ary
    * head and goals (distinct/shared $-variables). Idempotent. 1 on success. */
   int kb_assert_clause(KB *kb, const KbGoal *head,
                        const KbGoal *body, size_t nbody);
   ```
2. **`src/kb.c`** — implementare `kb_assert_clause` vicino a `kb_assert_rule_n`
   (~riga 292): costruisci un `Rule r` con `term_make(&r.head, head->pred,
   head->args, head->argc)` e per ogni goal `term_make(&r.body[i], …)`; `r.nbody`,
   `r.origin = kb->origin`; check idempotente leggero (come `kb_assert_rule_n`);
   `kb_add_rule(kb, &r)`. `term_make`/`kb_add_rule` sono statici lì → per questo
   la funzione VA in kb.c. Validare `nbody` in 1..KB_MAX_BODY e argc≤KB_MAX_ARGS.
3. **`src/mcp.c`** — nuovo tool `kb.assert_clause`:
   - schema: `{"head":{"pred":P,"args":[...]}, "body":[{"pred":P,"args":[...]}, …]}`.
   - helper `build_clause_args`: come `build_args` MA un arg che inizia con `$` è
     una VARIABILE → passa as-is (NON lit_encode); ogni altro letterale →
     `lit_encode` (quota se serve). Numeri/bool → stringify.
   - `kb_set_origin(kb, KB_SESSION)` prima di asserire (provenienza, come gli altri
     write). Costruisci KbGoal per head e body, chiama `kb_assert_clause`.
   - registra il tool nella tool-table (nome+desc+schema) e nel dispatch strcmp.
   - Attenzione: NIENTE serializzazione a stringa del clause (evita injection via
     args); si costruisce strutturalmente coi KbGoal.
4. **`tests/assertclause.sh`** — il gate sopra; aggiungerlo al target `test` del
   `Makefile` dopo `dollarvar.sh`.
5. **Verifica**: gate verde + regressione (`run.sh mcp.sh mcp-teach.sh dollarvar.sh
   multigoal.sh knowledge.sh grammar.sh explain.sh` almeno; poi `make test`).
6. **Doc**: `use-mcp-engine.md` (nuovo tool nella tabella + Limiti noti: A.2 chiuso),
   `teach-comprehension-via-mcp.md` §5.5 (U2 ✅). Commit `feat(mcp): gen281 - U2`.

**Rischi:** injection via args (mitigato: costruzione strutturale, non stringa) e
`$`-var vs letterale in `build_clause_args` (regola: `$`-prefix = variabile). Se
un arg è un termine composto `f(a)` — NON supportato fino a U3 — resta un atomo
piatto: accettabile ora, U3 lo renderà struttura.

---

## CODA (dopo U2, ordine consigliato per superare D.1/D.2)

Design completo in `teach-comprehension-via-mcp.md` §6.

- **U1b gen B** — flip `is_var` a solo-`$` (libera le maiuscole; ritira l'hack di
  quotatura per maiuscole). Migrare prima i fixture di test legacy a `$`. Indipendente
  da U2; si può fare quando si vuole. Dettaglio pieno: era la vecchia sezione di
  questo file (vedi git history di NEXT.md, commit `aa67d76`+).
- **U6** — negazione-per-fallimento `naf(G)` nel corpo → **D.2** (default con
  eccezioni). Piccolo. `solve()` prova G annidato; se fallisce, `naf` riesce. Solo
  goal GROUND (floundering guard) + stratificazione. Design §6.2.
- **U3** — unificazione STRUTTURALE su termini composti → **D.1** (computazione:
  Peano, liste, stringhe). Il grande: `term_split` + `unify` ricorsivo +
  `rename_term` su var annidate. Design §6.1.
- **U4** (de)serializzazione stringa⟷struttura; **U5** migrazione Secchio B a
  `present/2`. Sopra U3.

Fuori scommessa (NON fare senza pull reale): defeasibilità con priorità/probabilità.

## Memorie rilevanti (in `~/.claude/.../memory/`)

`parrot0-mcp-cannot-teach` (D.1/D.2 + come superarli), `parrot0-substance-presentation`
(U-sequence), `parrot0-prolog-engine-nary`, `parrot0-mcp-teach-quoting`.
