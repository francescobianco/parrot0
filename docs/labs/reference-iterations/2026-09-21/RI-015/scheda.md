# RI-015 — REPERTO DIAGNOSTICO (non una iterazione completa)

**Stato:** `diagnostic`. La causa è isolata e il sito nominato; la cura **non**
è stata scritta. Per il §0.1 un tentativo così non incrementa il contatore.

## Il fatto

La lezione canonica di `LEARN_PROTOCOL` §B — la prima riga del catalogo, quella
con cui si apre un verbo di relazione — **non funziona nella forma nuda**:

```text
> thicken is a relation verb
Hmm, I don't know about relation verb yet. Want me to learn about it?

> "thicken" is a relation verb
Learned: thicken is a relation_verb.

> the word thicken is a relation verb
Learned: thicken is a relation_verb.
```

E dopo la forma citata la prosa tecnica si legge:

```text
> Anodising thickens the oxide layer.
Learned: anodising thicken oxide layer.
```

Quindi **la conoscenza che serve è una sola parola di differenza**: le
virgolette. Il maestro che segue il protocollo alla lettera, e scrive la forma
nuda documentata, sbatte in un muro proprio sul verbo **nuovo** — cioè
esattamente quando la lezione serve.

## Non è una regressione di questo lotto

Misurato ricompilando l'albero a `b0554b7f~4` (prima di RI-011): «threaten is a
relation verb» muraba già. Il successo osservato nel lotto precedente era su uno
stato di KB diverso.

## Il sito, nominato

`/debug` sul turno che fallisce:

```text
turn_class_read   niente
pending_gap       relation_verb
debug_turn_entity relation_verb  thicken
acquisition_policy ask
```

La IR lega entrambe le parole; nessun lettore di lezione arriva. La traccia di
lettura mostra che la forma citata passa da `mod_mention`
(`p0_parse_mention_membership`) e la forma nuda no: il parser richiede o le
virgolette o un marcatore di menzione («the word …»), e altrimenti **esce**
(`if (!p0_mention_marker(b, w[i])) return 0;`).

## La cura proposta, e perché non l'ho scritta

La direzione giusta — sotto il criterio del §0.5-bis — **non** è una forma
nuova: è che il parser accetti la forma nuda **quando la classe nominata è una
classe di PAROLE**. E quali classi lo siano è derivabile, non una lista: la
superficie di una classe di parole finisce con una testa metalinguistica
(`metalinguistic_head/1` — verb, noun, marker, word, name…), che la KB tiene già.
Coprirebbe in un colpo `relation verb`, `condition marker`, `verb particle`,
`english verb lemma`.

Non l'ho scritta perché tocca un parser condiviso con il ritiro e con la
domanda («is the word unless a condition marker?»), e una modifica lì va
certificata con il suo contrasto: una frase come «A pump is a device.» **non**
deve diventare una menzione. È il primo lavoro del prossimo lotto.

## Perché conta più di una iterazione qualunque

Se la lezione che apre un verbo non funziona nella forma documentata, ogni
dominio tecnico nuovo comincia con un muro, e il maestro non ha modo di sapere
che gli bastava virgolettare. È un difetto di **addestrabilità del protocollo
stesso**, non di una superficie.
