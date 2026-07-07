# Il motore Prolog-like di parrot0 (protocollo e contratto)

> **Stato:** scritto a gen278 (2026-07-07), da lettura del sorgente
> (`src/kb.c`) e da prove dal vivo via `parrot0 --mcp-engine`. Documenta cosa il
> motore **fa davvero**, non cosa un Prolog "dovrebbe" fare. È il riferimento
> sotto [docs/use-mcp-engine.md](use-mcp-engine.md) (il canale MCP) e
> [docs/plans/generative-prolog.md](plans/generative-prolog.md) (dove va).

parrot0 ha un motore d'inferenza **Prolog-like** al centro (`src/kb.c`, da
gen5/gen11): fatti, regole definite (Horn), unificazione di Robinson, risoluzione
**SLD** con backtracking e standardize-apart. Non è un Prolog completo (niente
cut, niente aritmetica dentro le clausole, niente liste come termini), ma il
nucleo di risoluzione è **n-ario e ricorsivo** — più potente di quanto alcune API
sappiano esprimere (vedi §5).

## 1. La sintassi delle clausole (`.p0`)

I file `.p0` sono il formato canonico. Una clausola per riga; `%` inizia un
commento; la riga termina con `.` opzionale.

```prolog
% FATTO — termine ground, arità qualsiasi
man(socrates).
parent(tom, bob).
math_op(addition, "combining two numbers").   % stringa fra virgolette = UN argomento

% REGOLA DEFINITA — head :- goal0, goal1, ...   ($ marca le variabili, gen280)
mortal($X)          :- man($X).                          % unaria, una variabile
grandparent($X, $Z) :- parent($X, $Y), parent($Y, $Z).   % n-aria, JOIN su $Y
ancestor($X, $Y)    :- parent($X, $Y).                    % ricorsione: caso base
ancestor($X, $Y)    :- parent($X, $Z), ancestor($Z, $Y).  %             caso ricorsivo

% NEGAZIONE esplicita (closed-world locale)
not(likes(alice, snakes)).
```

**Variabile vs atomo** (`is_var`, `src/kb.c:101`): un argomento è una **variabile**
se inizia con **`$`** (`$X`, `$nome` — marcatore esplicito, gen280/U1b), oppure —
convenzione legacy — con **maiuscola** o **`_`** (`X`, `Y`, `_`, `_tmp`). Tutto il
resto è un **atomo** costante (`socrates`, `tom`, `addition`). Un `_` singolo è una
variabile **anonima fresca** ogni volta (due `_` nella stessa clausola non si
alias). Le stringhe fra `"..."` sono un solo argomento: le virgole interne sono
contenuto, non separatori (`src/kb.c:673`).

> **In corso (U1b, NEXT.md):** `$` è un sigillo dedicato che i dati non usano mai;
> è la forma canonica raccomandata. Oggi vale il **dual-accept** (`$` *e*
> maiuscola/`_` sono variabili). Un flip futuro a **solo-`$`** libererà le
> costanti maiuscole (`Madrid` = costante, la maiuscola torna pura presentazione)
> e renderà ridondante l'hack di quotatura di U1 al bordo MCP. Le regole shippate
> in `kb/` usano già `$`; i fixture di test legacy restano maiuscoli e passano
> grazie al dual-accept.

Direttiva di inclusione: `:- include(relative/path.p0).` carica un altro file
(`src/kb.c:767`).

## 2. Il contratto d'inferenza (cosa garantisce il solver)

Il cuore è `solve()` (`src/kb.c:380-425`), invocato da `kb_query`. Contratto
misurato:

| Capacità | Supportata? | Dettaglio |
|---|---|---|
| Fatti ground ad arità arbitraria | ✅ | `parent(tom, bob)`, `persona(x, y, ingegnere)` |
| Regole unarie a una variabile | ✅ | `mortal(X) :- man(X)` |
| **Regole n-arie, variabili distinte** | ✅ | `grandparent(X,Z) :- parent(X,Y), parent(Y,Z)` |
| **Join su variabile condivisa** | ✅ | la `Y` sopra lega i due goal |
| **Ricorsione** | ✅ | `ancestor` transitivo; guardia `KB_MAX_DEPTH=64` (`src/kb.c:26,388`) sui cicli |
| Backtracking + standardize-apart | ✅ | `rename_term` per-clausola (`src/kb.c:346`); frame unico per invocazione |
| Proof-trace | ✅ | `kb_explain` → `"grandparent(tom,ann) because parent(tom,bob) and parent(bob,ann)"` |
| Negazione esplicita (fatti) | ✅ | `not(P)` fatto negativo ground blocca `P` (`kb_is_negated`) |
| **Negazione-per-fallimento nel corpo** | ✅ (gen282, U6) | `head :- …, naf(G)` — `naf(G)` riesce se `G` non è derivabile. Solo goal **ground** (floundering: declina); default con eccezioni (`flies($X):-bird($X),naf(abnormal($X))`). NON copre priorità fra default/probabilità |
| Cut / control | ❌ | nessun `!` |
| Aritmetica nelle clausole | ❌ | i numeri sono atomi; il calcolo vive nei moduli `brain/*` (wordmath) |
| Liste/strutture come termini | ❌ | gli argomenti sono atomi/variabili piatti |

**Prova viva** (via `--mcp-engine`, con le clausole del §1 in un `.p0`):

```jsonc
kb.query   {"pred":"grandparent","args":["tom","ann"]}  → {"provable":true}
kb.explain {"pred":"grandparent","args":["tom","ann"]}
   → {"explanation":"grandparent(tom, ann) because parent(tom, bob) and parent(bob, ann)"}
kb.query   {"pred":"ancestor","args":["tom","zoe"]}     → {"provable":true}   // tom→bob→ann→zoe
kb.query   {"pred":"ancestor","args":["ann","tom"]}     → {"provable":false}  // niente falsi positivi
```

## 3. Interrogazione con variabili (`kb_match`)

`kb_match` prende un pattern in cui `null` (MCP) / una variabile marca un buco e
raccoglie i valori che lo riempiono:

```jsonc
kb.match {"pred":"parent","args":["tom", null]}  → {"bindings":["bob"]}
```

**Limite attuale (Gap A):** `kb_match` colleziona il binding della **sola prima**
variabile e restituisce una lista **piatta e de-duplicata**. Con più buchi non
dà le tuple:

```jsonc
kb.match {"pred":"parent","args":[null, null]}
   → {"bindings":["tom","bob","ann"]}          // NON [["tom","bob"],["bob","ann"]]
```

Il join fra gli slot va perso. Le tuple sono il passo **P2** del piano
([docs/plans/generative-prolog.md](plans/generative-prolog.md)).

## 4. Origine e persistenza (provenienza)

Ogni clausola porta un tag di **origine** (`Rule.origin`/`Fact.origin`,
`src/kb.c:65`): `KB_BASE` (substrato curato dai file `.p0`), `KB_SESSION` (scritto
a runtime — es. tutto ciò che arriva da MCP), `KB_INDUCED` (generato da
`kb_induce`). `kb_save(kb, path, mask)` serializza selettivamente per origine: il
layer di sessione si persiste **separato** dal substrato curato, mai sopra di
esso. È la disciplina di sicurezza ripresa da MCP (`use-mcp-engine.md` §Sicurezza)
e dal manifesto [kb-first].

## 5. Il divario API ↔ motore (importante)

Il motore (§2) è più potente delle sue **porte di costruzione**. Oggi:

- Il path **`.p0`** (`kb_load` → `parse_to_term`, `src/kb.c:826`) costruisce
  termini n-ari pieni: **tutta** la potenza del §2 è raggiungibile da qui.
- Il path **programmatico** `kb_assert_rule_n` (`src/kb.c:259-289`), e quindi il
  tool MCP `kb.assert_rule`, **appiattisce** ogni goal del body a unario
  (`argc=1, args[0]="X"`). Perciò una regola con join **non è asseribile** così,
  anche se il solver la eseguirebbe. `kb.assert_rule {"head":"grandparent","body":["parent","parent"]}`
  → `{"ok":true}` ma `kb.query grandparent(tom,ann)` → `false`.

Quindi, oggi: per insegnare una regola n-aria usa un file `.p0`; per una regola
unaria qualsiasi porta va bene. Colmare il divario programmatico (`kb.assert_clause`)
e le tuple di lettura è la roadmap di
[docs/plans/generative-prolog.md](plans/generative-prolog.md) (§3, passi P1-P2).
Il principio non cambia: **il motore è fisso, si aprono i condotti** — nessuna
logica di risoluzione nuova.

## 6. Mappa dei simboli C (per chi tocca il motore)

| Simbolo | File:riga | Ruolo |
|---|---|---|
| `unify` / `unify_term_term` / `unify_term_fact` | `src/kb.c:317-341` | unificazione di Robinson |
| `resolve` + `Subst`/`Bind` | `src/kb.c:297` | catena di binding |
| `rename_term` | `src/kb.c:346` | standardize-apart per-clausola |
| `solve` | `src/kb.c:380-425` | risoluzione SLD n-aria + backtracking |
| `kb_query` / `kb_match` / `kb_explain` | `src/kb.c` | prova booleana / binding / prova+traccia |
| `kb_assert` / `kb_assert_rule_n` | `src/kb.c` | scrittura fatti / regole (unario, §5) |
| `kb_load` / `parse_term` | `src/kb.c:735,656` | parser `.p0` (n-ario pieno) |
| `kb_induce` | `src/kb.c` | induzione di regole (unarie) dai fatti |
| `KB_MAX_DEPTH` / `KB_MAX_GOALS` / `KB_MAX_BODY` | `src/kb.c:25-26` | ceiling di ricorsione / resolvent / corpo |

## 7. Collegamenti

[docs/use-mcp-engine.md](use-mcp-engine.md) (il protocollo come tool MCP),
[docs/plans/generative-prolog.md](plans/generative-prolog.md) (dove il motore va:
Prolog generativo + sblocco dei bordi),
[docs/plans/unification.md](plans/unification.md) e
[docs/plans/unification-assessment.md](plans/unification-assessment.md)
(l'unificazione come collante, e l'assessment che ha diagnosticato i due gap).