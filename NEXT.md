# NEXT — punto di ripartenza

> Aggiornato a gen280 (2026-07-07). Leggi questo file per primo: dice ESATTAMENTE
> cosa fare al prossimo giro, come una generazione sola. Contesto completo:
> `docs/plans/teach-comprehension-via-mcp.md` §5.5 (sequenza upgrade U1..U5) e
> `docs/prolog-like-engine.md` §1 (convenzione variabili).

## Fatto finora

- **gen279 — U1** (fedeltà dei letterali via MCP): `kb.assert` quota i valori che
  l'engine fraintenderebbe come variabili; `kb.match` li ripulisce in uscita.
  Gate `tests/mcp-teach.sh`. Commit `270beec`.
- **gen280 — U1b gen A** (`$` come variabile esplicita, DUAL-ACCEPT): `is_var`
  (`src/kb.c:101`) ora accetta `$` oltre a maiuscola/`_`. Migrati a `$` i 4 siti
  C che costruiscono la variabile unaria (`kb_assert_rule`/`_n`) e le 6 regole
  `.p0` shippate (`kb/core/base.p0`, `kb/experts/linguistics/grammar.p0`). Gate
  `tests/dollarvar.sh` (regola `$` unaria + join multi-variabile). Suite verde;
  i fixture di test legacy restano maiuscoli e passano grazie al dual-accept.

## Prossimo passo: U1b gen B — il FLIP a `$`-only

**Obiettivo (una generazione).** Rimuovere il ramo `isupper` da `is_var`, così la
variabile è marcata SOLO da `$` (e `_` per l'anonima). Effetto: le costanti
maiuscole si liberano — `Madrid` diventa una costante a tutti gli effetti, la
maiuscola torna pura presentazione. È il payoff dell'intera direzione
sostanza/presentazione.

### Piano (atomico, in una generazione)

1. **Gate rosso** — nuovo test (o estendi `tests/dollarvar.sh`): dopo il flip,
   `capital(spain, Madrid)` insegnato via MCP **senza** affidarsi alla quotatura
   di U1 deve rileggersi come `Madrid` E una query con valore sbagliato deve dare
   `false`. Oggi (dual-accept) `Madrid` è ancora variabile → il test è rosso →
   registra il gap.
2. **Migrare i fixture di test legacy che usano variabili maiuscole** a `$`.
   Cercali: `grep -rlnE '\)[[:space:]]*:-' tests/ kb/ | xargs grep -lE '[A-Z]'`.
   Candidati noti: il fixture di `tests/multigoal.sh` (grandparent con X/Y/Z),
   e qualunque `.p0` sotto `tests/`. Verifica anche `tests/anon.sh` (usa `_`, che
   RESTA valido — non toccare `_`). Questo è il passo cruciale: dopo il flip una
   regola con `X` nudo tratterebbe `X` come costante e smetterebbe di unificare.
3. **Flip** `is_var` (`src/kb.c:101`): da
   `s[0]=='$' || isupper(...) || s[0]=='_'` a `s[0]=='$' || s[0]=='_'`.
4. **Semplificare l'hack U1** in `src/mcp.c` (`lit_needs_quote`): togliere il ramo
   `isupper` (le maiuscole ora sono costanti sicure); tenere la quotatura solo per
   spazi/virgola (che restano ambigui per il parser `.p0`). NON prima che il flip
   sia verde.
5. **Verifica**: TUTTA la suite verde. Le reti: `multigoal/grammar/knowledge/
   explain/anon/dollarvar/mcp-teach`. Se una regola smette di unificare, è un
   fixture non migrato al passo 2 → torna indietro, non forzare.

### Rischio / pivot

Il rischio è un fixture con variabile maiuscola dimenticato: si manifesta come un
test rosso su una regola, non come crash. La rete di test lo becca subito. Dovere
di pivot se emergono variabili maiuscole generate dinamicamente in punti non
previsti (ricontrolla `kb_induce` e i moduli `src/brain/*` che costruiscono
termini).

## Dopo gen B (orizzonte, non ora)

Sequenza in `docs/plans/teach-comprehension-via-mcp.md` §5.5 + §6 (design per
superare i limiti D.1/D.2):
- **U2** `kb.assert_clause` n-ario via MCP (P1 di `generative-prolog.md`) —
  **prerequisito** di U3 e U6 (serve per mandare regole ricche dal vivo).
- **U6** negazione-per-fallimento (`naf(G)` nel corpo) → **D.2**: default con
  eccezioni come conoscenza (`flies($X):-bird($X),naf(abnormal($X))`). Piccolo,
  alto valore, indipendente da U3. Attenzione: floundering (solo goal ground) +
  stratificazione. Design in §6.2.
- **U3** unificazione STRUTTURALE su termini composti → **D.1**: computazione
  ricorsiva come conoscenza (Peano, `length`/`reverse`) + strings-as-knowledge.
  L'upgrade grosso; `unify` ricorsivo + `rename_term` su var annidate. Design §6.1.
- **U4** (de)serializzazione stringa⟷struttura (sopra U3).
- **U5** migrazione del Secchio B (accordo/casing/morfologia) a `present/2`.

Ordine consigliato per superare D: **U2 → U6 → U3**. La defeasibilità con
priorità/probabilità resta deliberatamente FUORI (altro motore).

## Memorie rilevanti (in `~/.claude/.../memory/`)

`parrot0-substance-presentation`, `parrot0-mcp-teach-quoting`,
`parrot0-prolog-engine-nary`.
