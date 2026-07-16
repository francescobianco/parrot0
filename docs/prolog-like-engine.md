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
SOLO se inizia con **`$`** (`$X`, `$nome` — named) o con **`_`** (anonima). Tutto
il resto è un **atomo costante**, MAIUSCOLE INCLUSE: `Madrid`, `socrates`, il
carattere `M` sono costanti. Un `_` singolo è una variabile **anonima fresca**
ogni volta (due `_` nella stessa clausola non si alias). Le stringhe fra `"..."`
sono un solo argomento: le virgole interne sono contenuto, non separatori
(`src/kb.c`).

> **gen284 — solo-`$` (fine del dual-accept).** La vecchia regola "maiuscola =
> variabile" (eredità del Prolog storico) è stata rimossa: `$` è un sigillo
> dedicato che i dati non usano mai, quindi il case è **pura presentazione**, mai
> un segnale di variabilità. Questo dissolve alla radice l'ambiguità `Madrid` e
> rende i caratteri singoli (`M`, `A`) costanti — prerequisito per le azioni-su-
> stringa come conoscenza (U4). Tutte le regole in `kb/` e i fixture di test usano
> `$`; l'hack di quotatura di U1 al bordo MCP ora quota solo `$`/`_`/spazi.

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
| **Termini composti / unificazione strutturale** | ✅ (gen283, U3) | `f(a…)` è una struttura; `unify` ci ricorre; una variabile lega una sotto-struttura. `nat(s(s(z)))`, liste `cons(H,T)`/`nil`. Niente occurs-check; termini oltre `KB_TERM_LEN` troncano (cap) |
| **Computazione ricorsiva come conoscenza** | ✅ (gen283, U3) | Peano `add(s($X),$Y,s($Z)):-add($X,$Y,$Z)` → `add(s(z),s(z),$R)` dà `$R=s(s(z))`; `length`/`reverse` su liste. Insegnabile via `.p0` E via `kb.assert_clause` |
| **Azioni-su-stringa come conoscenza** (builtin `chars/2`) | ✅ (gen285, U4) | `chars/2` è l'UNICO builtin: atomo↔lista-di-char bidirezionale (`chars(madrid,$L)`→`$L=cons(m,…,nil)` e viceversa). Così `capitalize_first($S,$R):-chars($S,cons($H,$T)),upper($H,$U),chars($R,cons($U,$T))` è una REGOLA (la mappa `upper/2` è una tabella di fatti), non C. Stringhe word-like; caratteri speciali un bordo |
| Cut / control | ❌ | nessun `!` |
| Aritmetica *nativa* (`is/2`) nelle clausole | ❌ | i numeri restano atomi; per calcolare si usa Peano/strutture (U3) o i moduli `brain/*` (wordmath) |

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
  (`argc=1, args[0]="X"`). Perciò una regola con join **non è asseribile** con
  `kb.assert_rule`, anche se il solver la eseguirebbe.
  `kb.assert_rule {"head":"grandparent","body":["parent","parent"]}`
  → `{"ok":true}` ma `kb.query grandparent(tom,ann)` → `false`.

- **`kb.assert_clause` colma il divario (gen311, verificato dal vivo gen335).**
  Una regola n-aria con JOIN **è** asseribile via MCP — questa parte di §5 non è
  più "roadmap futura". Head e body come oggetti `{"pred":…,"args":[…]}`, e la
  risoluzione ricorsiva SLD la esegue. Provato dal vivo:
  ```
  kb.assert_clause {"head":{"pred":"is_a","args":["$X","$Z"]},
                    "body":[{"pred":"is_a","args":["$X","$Y"]},
                            {"pred":"is_a","args":["$Y","$Z"]}]}
  # con is_a(dog,mammal), is_a(mammal,animal) nella KB:
  kb.query   is_a(dog,animal)  → {"provable":true}
  kb.explain is_a(dog,animal)  → "…because is_a(dog,mammal) and is_a(mammal,animal)"
  ```

> ⚠️ **Footgun silenzioso (`$` obbligatorio, MCP non avvisa).** In
> `kb.assert_clause` un argomento **senza `$` è un ATOMO letterale**, non una
> variabile (è la stessa regola `is_var` del §1, ma via MCP morde forte perché
> **non c'è errore visibile**). `args:["X","Z"]` asserisce
> `is_a('X','Z') :- …` — inutile — e restituisce comunque `{"ok":true}`. La query
> poi fallisce senza spiegazione. **Regola pratica:** ogni variabile in
> `kb.assert_clause`/`kb.assert` va scritta `$X`, `$Y`, `$Z`. Le MAIUSCOLE nude
> sono costanti (gen284), non variabili come nel Prolog ISO.
>
> ⚠️ **Chiavi dei tool MCP (facili da sbagliare).** `gen.respond` vuole `{"input":…}`
> (non `text`). `kb.query`/`kb.explain`/`kb.match` vogliono `{"pred":…,"args":[…]}`
> (non `goal:"is_a(dog,animal)"`). `null` in `args` = variabile di query (slot da
> legare in `kb.match`).
>
> ⚠️ **Provabile ≠ raggiungibile in linguaggio naturale.** Un fatto può essere
> `provable:true` via `kb.query`/`kb.explain` e comunque restare **irraggiungibile**
> da `gen.respond` in prosa (la superficie "is a dog an animal?" non instrada al
> goal `is_a`). È il divario di *routing/colla linguistica*
> ([docs/plans/universal-input.md](plans/universal-input.md),
> [docs/plans/the-linguistic-glue.md](plans/the-linguistic-glue.md)), non un buco
> del motore: la conoscenza c'è, manca la mappa superficie→goal.

Quindi, oggi: una regola n-aria si insegna da un file `.p0` **o** via
`kb.assert_clause` (con `$`-variabili); `kb.assert_rule` resta valido solo per
regole unarie. Le tuple di lettura restano la roadmap di
[docs/plans/generative-prolog.md](plans/generative-prolog.md) (§3). Il principio
non cambia: **il motore è fisso, si aprono i condotti** — nessuna logica di
risoluzione nuova.

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