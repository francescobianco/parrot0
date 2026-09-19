# Catena di specie A: la IR legge, la KB risponde (19 settembre 2026)

`2026-09-19-catena-a-nome-nudo.patch` è la catena completa, **funzionante e non
applicata**. Si applica sopra `a1b63a53` con `git apply`. Due regole di quel
commit, il candidato sintagma e le forme flesse come operatore, sono state
**ritirate** nel commit successivo perché costavano già da sole: su r320
`input_frame_observe` passava da 1,7 a 11,1 s. Per riprovare la catena si
rimettono quelle due regole, poi si applica la patch, **dopo** la riscrittura
della lista delle entità descritta sotto.

## Che cosa fa (solo KB)

1. `input-structure.p0`: il **nome nudo** («Mercury», «beetles») diventa un candidato
   ruolo quando nessun sintagma candidato lo contiene (vince lo span massimo) e
   quando non appartiene a nessuna classe funzionale che la KB conosce già.
   Le condizioni con variabili stanno dietro ausiliari ground, perché `naf` va in flounder.
2. `input-structure.p0`: il nome del sintagma candidato si costruisce dai **token**
   (minuscoli) dentro lo span, non dalla superficie grezza («The zorvan beetle»).
3. `input-structure.p0`: lo span massimo vale anche per le entità note
   («beetle» dentro «the zorvan beetle» non è il soggetto).
4. `grammar.p0`: la **forma finita al presente** di un verbo di relazione (-s,
   -es, -ies) chiude il sintagma, mentre participi e gerundi no.
5. `text-structure.p0`: l'entità che apre il testo è la prima **per posizione**.

## Misurato

- `read: Beetles threaten forests.` → la IR impegna
  `semantic_binding(binary(threaten), beetles, object(forests))`. «what do beetles
  threaten?» → «forests.» da `turn_plan` (porta `turn_response`), non da `answerframe`.
  Lo stesso vale per «The zorvan beetle threatens the kelda forest.» → «kelda forest.»
  e per `read: Mercury is a metal.` (membership impegnata).
- Pioli: r300 48/62 (=), r320 20/68 (=), meta 2/2 dopo il punto 5.
- **Costo, motivo per cui non è applicata:** `input_frame_observe` sul paragrafo
  di r320 passa da ~1,7 s a **34,6 s**, il turno da ~10 s a 42 s e il piolo
  r320 da 86 s a 132 s, oltre il limite dei 2 minuti.

## Perché costa, e il passo successivo

`input_nearest_entity_before/after` chiude con `naf(input_entity_between(...))`,
che rienumera tutte le entità della clausola; con il nome nudo ogni token è
candidato, e il test di candidatura fa otto `naf` di classe. È il caso della
memoria del progetto: **calcolare una volta la lista delle entità della
clausola** (con posizione) e negare `member` su quella lista, invece di
rienumerare dentro un `naf`. Prima si applica quella riscrittura, poi la patch,
poi si rimisura: il piolo sotto i 2 minuti è la condizione.
