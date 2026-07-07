# NEXT — punto di ripartenza

> Scritto a gen279 (2026-07-07) alla fine di una sessione. Leggi questo file per
> primo: dice ESATTAMENTE cosa fare al prossimo giro, come una generazione sola.
> Contesto completo: `docs/plans/teach-comprehension-via-mcp.md` §5 (addendum) e
> §5.5 (sequenza upgrade U1..U5). U1 è già spedito (gen279, commit `270beec`).

## Prossimo passo: U1b — `$` come simbolo esplicito delle variabili

**Obiettivo (una generazione).** Introdurre `$` come marcatore esplicito di
variabile nel motore Prolog-like, indipendente da maiuscolo/minuscolo. Fine
ultimo: liberare le costanti maiuscole (`Madrid` = costante, la maiuscola torna
pura presentazione), uccidendo alla radice l'ambiguità che U1 oggi tampona con la
quotatura al bordo MCP.

**Perché.** Oggi `is_var` (`src/kb.c:95`) = "iniziale maiuscola o `_`". La
variabile "ruba" una proprietà che i dati reali hanno (la maiuscola) → da lì
l'ambiguità `Madrid`. `$` è un sigillo dedicato che i dati non usano mai →
rimuove l'overload. Concettualmente superiore sia a maiuscola-come-variabile sia
all'hack di quotatura di U1. Allineato alla direzione sostanza/presentazione.

## Il piano di migrazione (in questa singola generazione, atomico)

Va fatto TUTTO INSIEME: un KB mezzo-migrato si rompe in silenzio (dopo il flip,
una regola con `X` nudo tratterebbe `X` come costante e non unificherebbe più).

Scelta consigliata: **dual-accept prima, flip dopo, nello stesso commit** —
oppure fermarsi al dual-accept se il flip è troppo per un giro solo (vedi Nota).

1. **`is_var` accetta `$`** (`src/kb.c:95`): ritorna vero se `s[0]=='$'`
   (dual-accept: `|| isupper || '_'`; flip finale: SOLO `$`).
2. **Migrare i 4 siti C** che cablano `"X"` come variabile unaria di regola:
   `src/kb.c:252`, `:254`, `:278`, `:283` (in `kb_assert_rule`/`kb_assert_rule_n`)
   → usare `"$X"` invece di `"X"`. Controllare anche `rename_term` (`:346`) e i
   chiamanti di `is_var` (`resolve` `:298`, `unify` `:320`, `:353`) che restino
   coerenti.
3. **Migrare le regole `.p0`** (~150-200 righe, superficie piccola): ogni
   variabile maiuscola/`_` nelle regole diventa `$`-prefissata. File con regole
   (`grep -rl ':-' kb/`): 24 file, il grosso è `kb/profiles/agi.p0` (56 regole);
   gli altri 7-11 ciascuno. `_` anonimo → `$_` (o mantenere `_` come anonimo
   fresco: decidere e documentare). Fare con un sed scriptato + revisione.
4. **Parser**: verificare che `parse_term` (`src/kb.c:656`) e il tokenizer non
   spezzino `$`; se `$` non è già un char di atomo ammesso, ammetterlo.
5. **Bordo MCP**: una volta libere le maiuscole, l'hack di quotatura U1
   (`lit_encode`/`lit_decode` in `src/mcp.c`) diventa ridondante per le
   maiuscole — decidere se ritirarlo o tenerlo come cintura+bretelle per
   spazi/virgole (probabilmente tenerlo solo per spazi/virgola, togliere il ramo
   maiuscola). NON toccarlo finché il flip non è verde.

## Gate / rete di sicurezza (già esistono, usali)

- `tests/multigoal.sh` (grandparent, query-con-variabile via rule),
  `tests/grammar.sh`, `tests/knowledge.sh`, `tests/explain.sh`, `tests/anon.sh`
  (variabili anonime): TUTTI esercitano regole con variabili → devono restare
  verdi dopo la migrazione. Sono la rete che rende il flip sicuro.
- `tests/mcp-teach.sh` (gate di U1): deve restare verde.
- Nuovo gate suggerito: dopo il flip, `capital(spain, Madrid)` insegnato via MCP
  **senza** la quotatura di U1 deve rileggersi come `Madrid` (prova che `$`-only
  ha liberato le maiuscole). E una regola scritta con `$X` in un `.p0` deve
  risolvere (es. `mortal($X) :- man($X).`).

## Nota di prudenza (disciplina LOOP.md)

Se il flip completo (dual-accept → solo-`$`) è troppo per una generazione,
**spezzalo**: gen A = dual-accept + migrazione `.p0`/C a `$` (tutto ancora verde
perché la maiuscola resta accettata); gen B = flip a solo-`$` + nuovo gate +
ritiro del ramo-maiuscola in `lit_encode`. Ogni gen parte da un gate rosso, mai
codice speculativo. Dovere di pivot se la migrazione tradisce l'emergenza senza
beneficio misurabile.

## Stato al momento della scrittura

- gen279 spedito: U1 (fedeltà dei letterali via MCP), 4 doc nuovi/aggiornati,
  `tests/mcp-teach.sh`. Suite verde. Commit `270beec`, pushato su `main`.
- Memorie rilevanti: `parrot0-substance-presentation`, `parrot0-mcp-teach-quoting`,
  `parrot0-prolog-engine-nary` (in `~/.claude/.../memory/`).
