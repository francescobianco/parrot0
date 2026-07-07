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
- **gen281 — U2** `kb.assert_clause` (regole n-arie via MCP). `tests/assertclause.sh`
  verde; `use-mcp-engine.md` + `teach-comprehension-via-mcp.md` §5.5 allineate.

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
