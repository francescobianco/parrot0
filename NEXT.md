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
  `tests/naf.sh` (via `.p0` e via MCP). Suite verde.

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

## CODA (dopo U6) — il prossimo è U3

Design completo in `teach-comprehension-via-mcp.md` §6.

- **U3** — unificazione STRUTTURALE su termini composti → **D.1** (computazione:
  Peano, liste, stringhe). Il grande: `term_split` + `unify` ricorsivo +
  `rename_term` su var annidate. Design §6.1.
- **U1b gen B** — flip `is_var` a solo-`$` (libera le maiuscole). Indipendente;
  quando si vuole. Migrare prima i fixture di test legacy a `$`.
- **U4** (de)serializzazione stringa⟷struttura; **U5** migrazione Secchio B a
  `present/2`. Sopra U3.

Fuori scommessa (NON fare senza pull reale): defeasibilità con priorità/probabilità.

## Memorie rilevanti (in `~/.claude/.../memory/`)

`parrot0-mcp-cannot-teach` (D.1/D.2 + come superarli), `parrot0-substance-presentation`
(U-sequence), `parrot0-prolog-engine-nary`, `parrot0-mcp-teach-quoting`.
