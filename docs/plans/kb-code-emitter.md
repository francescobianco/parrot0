# L'emettitore guidato dalla KB — A5, e perché è il pezzo che cambia il gioco

> **Aperto gen503.** Nasce da una constatazione, non da un'ambizione: guardando
> come sintetizzare un file per il banco di gara si è visto che
> `code_synth_from_shape` è una catena di `if (strcmp(shape, …))` con **il corpo
> della funzione scritto come stringa C dentro `src/code.c`**. Aggiungerci un
> caso significa mettere conoscenza nel C — mantra #1 — e, se quel caso è la
> risposta del banco, significa anche barare (`CHALLENGE_TODO.md` §6.5-bis).

## 1. Che cos'è oggi

```c
if (strcmp(shape, "nested_loop_compare_swap") == 0) {
    snprintf(out, out_sz,
        "void %s(int a[], int n) {"
        " for (int i = 0; i < n; i++)"
        " …"
```

Un solo caso, dopo quattro generazioni. `kb/experts/programming/algo_steps.p0`
lo dice di sé: *«ogni algoritmo nuovo costa una generazione di C o un fatto
scritto a mano, quindi la generazione n compra la capacità n»* — crescita
lineare, la forma che `LOOP.md` chiama impostora.

## 2. Il taglio: l'algoritmo non è la lingua

L'errore non è avere un template: è avere **un template per algoritmo**. Un
corpo di funzione è due conoscenze diverse sovrapposte:

```text
CHE COSA FA          scorri, confronta, scambia, accumula, restituisci
COME SI SCRIVE IN C  "for (int {0} = {1}; {0} < {2}; {0}++)"
```

Separarle è ciò che rende la crescita non lineare:

| aggiungo… | costo |
|---|---|
| un algoritmo nuovo con operazioni già note | **righe di KB, zero C** |
| un'operazione nuova | una riga per lingua |
| una lingua nuova (Python, Go) | una riga per operazione, **nessun algoritmo riscritto** |

Ed è la stessa forma già pagata due volte oggi: `local_tool/3` + `tool_argv/2`
per gli strumenti, `tool_result_cue/2` per la lettura di un'uscita. Il C fa la
meccanica, la KB dice quale.

## 3. Il vocabolario

Cinque predicati, tutti entro i 4 argomenti del motore.

```prolog
% ── LA FORMA: che cosa fa l'algoritmo, senza nominare nessuna lingua ──
code_shape_step(Shape, StepId, Parent, Op).   % l'albero delle operazioni
code_shape_order(Shape, StepId, N).           % l'ordine fra fratelli
code_shape_slot(StepId, Index, Value).        % gli argomenti dell'operazione

% ── LA LINGUA: come quell'operazione si scrive ──
code_shape_signature(Shape, Lang, Template).
lang_syntax(Lang, Op, Template).
```

`Parent` vale `root` al livello esterno. Nei template `{0}`, `{1}`, … sono gli
slot dell'operazione; `{name}` e `{cmp}` sono i legami che il chiamante passa
(il nome della funzione da produrre, il verso del confronto).

**La regola di composizione, e non ce ne sono altre:** un nodo con figli apre
un blocco, ci mette dentro i figli e lo chiude; un nodo foglia è solo il suo
template.

⚠ **Corretto dopo M5.** Qui c'era scritto «le graffe non sono conoscenza: sono
la meccanica», e la seconda lingua lo ha falsificato in mezz'ora: Python i
blocchi li segna col rientro. *Come* si apre e chiude un blocco è una proprietà
della lingua e sta in KB:

```prolog
lang_block(c, "{", "}").
lang_block(python, ":", "").
lang_layout(python, indented).
```

Meccanica resta solo l'annidamento: quale nodo sta dentro quale.

### Bubblesort, per intero

```prolog
code_shape_signature(nested_loop_compare_swap, c, "void {name}(int a[], int n)").

code_shape_step(nested_loop_compare_swap, nlcs_1, root,   loop_up).
code_shape_step(nested_loop_compare_swap, nlcs_2, nlcs_1, loop_up_bounded).
code_shape_step(nested_loop_compare_swap, nlcs_3, nlcs_2, if_compare).
code_shape_step(nested_loop_compare_swap, nlcs_4, nlcs_3, swap_via_temp).

code_shape_slot(nlcs_1, 0, "i").   code_shape_slot(nlcs_1, 1, "0").   code_shape_slot(nlcs_1, 2, "n").
code_shape_slot(nlcs_2, 0, "j").   code_shape_slot(nlcs_2, 1, "0").   code_shape_slot(nlcs_2, 2, "n - i").
code_shape_slot(nlcs_3, 0, "a[j]").code_shape_slot(nlcs_3, 1, "{cmp}").code_shape_slot(nlcs_3, 2, "a[j + 1]").
code_shape_slot(nlcs_4, 0, "a[j]").code_shape_slot(nlcs_4, 1, "a[j + 1]").code_shape_slot(nlcs_4, 2, "t").

lang_syntax(c, loop_up,         "for (int {0} = {1}; {0} < {2}; {0}++)").
lang_syntax(c, loop_up_bounded, "for (int {0} = {1}; {0} + 1 < {2}; {0}++)").
lang_syntax(c, if_compare,      "if ({0} {1} {2})").
lang_syntax(c, swap_via_temp,   "int {2} = {0}; {0} = {1}; {1} = {2};").
```

## 4. Che cosa NON è, e il confine

⛔ **Non è spostare il corpo di una funzione dal C alla KB.** Se `code_shape_step`
per `string_join` fosse la trascrizione riga per riga di `strjoin.c`, avremmo
solo cambiato il file in cui sta la risposta del banco. Il confine di §6.5-bis
vale identico: *questa forma serve anche a un compito che non è nel banco?*
`loop_up` sì; una `emit_the_body_match0` no.

⛔ **Non è un linguaggio di programmazione nuovo.** Gli slot sono testo, non
espressioni: nessun typechecker, nessuna valutazione. Chi vuole un'espressione
la scrive nello slot, e l'oracolo la giudica compilandola.

⭐ **E il giudizio resta esterno.** Un corpo emesso non è corretto perché
l'abbiamo composto: è corretto se `cc -Werror` lo accetta e l'oracolo lo esegue.
`code_check_sort` è già quell'oracolo per gli ordinamenti — verificato da
`tests/p0t/code/check_sort.p0t`.

## 5. L'ordine di lavoro

```text
M1 ✅ il renderer generico, e nessun corpo di funzione nel C
      prova: `nested_loop_compare_swap` viene ri-derivato dalla KB e produce un
      ordinamento che l'ORACOLO accetta; il template C letterale è cancellato.

M2 ✅ una forma NUOVA dichiarata solo in KB, zero righe di C
      prova: sum_over_array, con due operazioni nuove (una riga di lang_syntax
      ciascuna) e nessuna ricompilazione della logica.

M3 ✅ le operazioni che servono a un contratto: allocare, controllare un
      trabocco PRIMA di allocare, copiare, terminare.
      prova: `concat_two` — l'algoritmo da manuale, non il compito di nessun
      banco — emesso, COMPILATO con `-std=c11 -Wall -Wextra -Werror -O2` ed
      ESEGUITO. Sei operazioni nuove, sei righe di `lang_syntax`.

M4    la scelta della forma dal CONTRATTO letto (A2) invece che dal nome:
      `contract_clause/3` → quali operazioni servono. È qui che smette di
      essere un generatore e comincia a essere sintesi.

M5 ✅ una seconda lingua, per falsificare il taglio.
      prova: la STESSA forma `sum_over_array` — stesso albero, stessi slot, non
      toccati — resa in Python ed eseguita. Quattro righe di `lang_syntax`.

      ⭐ E la falsificazione ha fatto il suo mestiere: ha trovato una riga
      sbagliata. Le graffe erano scritte nel C e chiamate «meccanica» (§3). Non
      lo sono: un blocco si segna con due parentesi o con un rientro, e quale
      delle due è una proprietà della LINGUA. Ora sono `lang_block/3` e
      `lang_layout/2`, e il §3 di questo piano è corretto di conseguenza.
```

## 6. Come si riconosce che ha funzionato

Non dal numero di forme. Da **quanto costa la prossima**: se una forma nuova
richiede una riga di C, il pezzo è ancora dalla parte sbagliata, per quanti
`.p0` si siano aggiunti.
