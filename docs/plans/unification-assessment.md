# Unification: assessment e piano (companion a `unification.md`)

> Questo documento NON sostituisce `unification.md` (la visione). Lo mette in
> tensione coi principi del progetto e ne estrae il sottoinsieme implementabile
> in modo coerente, da tirare con la pressione di un benchmark — non da disegnare
> dall'alto. Leggere prima `PRINCIPLES.md`.

## Punto di partenza che il doc-visione trascura

L'unification **esiste già** ed è la spina dorsale, non una direzione futura.
In `src/kb.c`, da gen5/gen11:

- `unify()`, `unify_term_fact()`, `unify_term_term()` — algoritmo di Robinson;
- `Subst`/`Bind` + `resolve()` — risoluzione della catena di binding;
- **SLD resolution con backtracking** + standardize-apart per frame di regola;
- `kb_match()` — query con variabili (`NULL` = variabile);
- `kb_induce()` — induzione di regole da fatti (analogo deterministico del training).

Conseguenza: il valore della discussione non è *"introdurre"* l'unification, ma
decidere **cosa farne** restando dentro il metodo.

## Cosa NON facciamo (e perché)

1. **La pipeline a 8 stadi** (`Testo→…→Generazione`) è architettura top-down.
   Contraddice *"we do not impose the parts top-down... we let structure appear
   when a task demands it"*. Resta come orizzonte narrativo, non come piano.
2. **La separazione netta comprensione/generazione** (NL come mero I/O,
   ragionamento su un'interlingua logica) contraddice il wager: l'LLM ragiona in
   **uno** spazio di rappresentazione (cfr. memoria `code-mastery-direction`). È
   la scommessa simbolico-classica (GOFAI), diversa da quella del progetto.
3. **"Unificazione estesa" come liste di sinonimi/equivalenze** = phrasebook,
   vietato dal cardinal corollary. La versione principled esiste già
   (`kb_nearest_concept` gen155, `part_of` emergente gen157/158) e si continua
   tirandola dalla pressione, non re-disegnandola come "motore cognitivo".

## Cosa SÌ facciamo — i due gap reali dell'engine

Entrambi emergono dagli esempi del doc-visione e sono onesti, KB-first, piccoli.

### Gap A — Query strutturale multi-variabile
Oggi `kb_match` restituisce **solo il binding della prima** variabile. L'esempio
`persona(X, Y, ingegnere)` (riga 183 del doc) richiede di restituire **tuple**.

### Gap B — Regole n-arie / inferenza relazionale in avanti
Oggi: backward-chaining SLD solo su regole **unarie a variabile singola**; per
arità >1 lookup ground diretto. L'esempio del mutuo
(`azione(compra, richiede, denaro)` → catena compra→denaro→mutuo→banca, righe
138-150) **non è derivabile** dall'engine attuale: serve unificazione su termini
n-ari condivisi tra premesse e una camminata sulle conseguenze.

## Il piano (discovery-harness, una generazione per volta)

Metodo invariato: ogni passo nasce da un **gate fallito** in un benchmark, non da
codice speculativo; refusal onesto al bordo; nessuna stringa hardcoded.

- **Passo 0 — Harness.** Aggiungere `make unify-bench` su `tests/unify/*.unify`
  (stesso schema di `code-bench`): stimolo strutturale → query → binding attesi.
  Primo gate: `persona(X, Y, ingegnere)` su un KB di poche persone deve elencare
  *tutte* le tuple (oggi fallisce → registra il gap A).

- **Passo 1 — Gap A, tuple di binding.** Estendere `kb_match` (o una
  `kb_match_tuples` accanto, per non rompere i call-site) perché collezioni il
  binding di **ogni** slot-variabile per ciascun fatto unificante, in ordine di
  inserimento. Verbalizzazione grounded dal risultato. Nessun nuovo campo C:
  resta tutto sul KB.

- **Passo 2 — Gap B.1, unificazione n-aria nelle regole.** Permettere corpi di
  regola con termini ad arità >1 e variabili condivise tra goal
  (`needs(A,C) :- action(A,requires,C)`). L'infrastruttura `unify_term_term` +
  standardize-apart c'è già: il lavoro è nel parser di regole (`kb_load`/
  `kb_assert_rule_n`, oggi unari) e nel motore di risoluzione per arità >1.
  Gate: derivare `needs(compra, denaro)`.

- **Passo 3 — Gap B.2, catena/forward reachability.** Risolvere la query
  transitiva dell'esempio mutuo (compra→denaro→mutuo→banca) come backward-chaining
  su regole n-arie ricorsive (`enables(A,C) :- action(A,requires,C)`;
  `enables(A,C) :- action(A,via,B), enables(B,C)`), con ceiling di profondità e
  refusal su ciclo — **niente** forward-chaining hardcoded. Gate: spiegare la
  catena via `kb_explain` esteso ai termini n-ari.

- **Passo 4 (solo se la pressione lo chiede) — unificazione "estesa".** *Non*
  prima, e *non* come liste di sinonimi. Si aggancia ai bricks esistenti
  (`kb_nearest_concept`, `part_of`): un goal che non unifica esattamente può
  ritentare contro il concetto più vicino *derivato dalla struttura del KB*, con
  soglia/margine già presenti in gen155. Pull-from-pressure, non design.

## Criteri di stop / pivot

Vale `primitives-first-pivot-duty`: se a un passo l'estensione strutturale inizia
a *tradire* l'emergenza senza beneficio osservabile sul bench, si chiama il pivot
a domain-pull invece di forzare. L'harness misura il comportamento, non solo la
correttezza (cfr. `oracle-is-behavioural-signal`).