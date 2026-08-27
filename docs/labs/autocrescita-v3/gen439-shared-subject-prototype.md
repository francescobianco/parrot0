# Gen439 — prototipo conservato: coordinazione di predicati con soggetto condiviso

**Stato:** incompleto, non caricato dal runtime
**Data:** 27 agosto 2026

Questo reperto conserva il tentativo successivo al verticale verde sulla
coordinazione di oggetti. Non va promosso in `kb/core/input-structure.p0` finche'
non supera il proprio testimone, ma non va perduto: alza l'astrazione dalla
distribuzione di un singolo ruolo alla continuita' di un ruolo fra eventi.

## Classe cercata

```text
tree absorbs water and produces oxygen
mill grinds grain and produces flour
university funds research and produces graduates
```

La seconda predicazione non dichiara il soggetto. La lettura corretta eredita
il `subject` del primo frame:

```text
binary(absorb_relation, subject(tree), object(water))
binary(produce_relation, subject(tree), object(oxygen))
```

La lettura locale ingenua produce invece il falso:

```text
binary(produce_relation, subject(water), object(oxygen))
```

Il problema non e' un verbo o un dominio. E' un operatore compositivo che
mantiene un ruolo attraverso due frame coordinati.

## Relazioni sperimentate

Il tentativo introduceva queste viste, tutte KB-first:

```prolog
input_elided_subject_operator(
    $Scope, $Language, operator($Relation, $RId), cue($CId)) :-
    input_coordination_node($Scope, $Language, $CId, $Surface),
    input_operator_node($Scope, $Language, operator(binary, $Relation), $RId),
    input_node_before($Scope, $CId, $RId),
    naf(input_entity_between($Scope, $Language, interval($CId, $RId))),
    input_operator_node($Scope, $Language,
                        operator(binary, $Prior), $PriorId),
    input_node_before($Scope, $PriorId, $CId).

input_binary_predicate_coordination(
    $Scope, $Language,
    first(relation($FirstRelation, $FirstRId),
          roles(entity($SId, $Subject), entity($FirstOId, $FirstObject))),
    second(relation($SecondRelation, $SecondRId),
           roles(entity($SId, $Subject), entity($SecondOId, $SecondObject)))) :-
    input_binary_assertion(
        $Scope, $Language, relation($FirstRelation, $FirstRId),
        roles(entity($SId, $Subject), entity($FirstOId, $FirstObject))),
    input_elided_subject_operator(
        $Scope, $Language, operator($SecondRelation, $SecondRId), cue($CId)),
    input_nearest_entity_after($Scope, $Language, $SecondRId,
                               entity($SecondOId, $SecondObject)),
    input_node_before($Scope, $FirstOId, $CId),
    input_node_before($Scope, $CId, $SecondRId),
    dif($FirstRId, $SecondRId).
```

`input_elided_subject_operator/4` risultava dimostrabile sul primo campione. La
regola composta `input_binary_predicate_coordination/4` no: la ricostruzione
annidata di due frame, forme, lingua e prossimita' superava il limite pratico
di profondita' del solver. Tentare di nascondere il frame locale scorretto con
NAF dentro il producer rompeva anche la coordinazione di oggetti gia' verde.

Firma osservata prima del rollback del prototipo:

```text
gen439-coordination.p0t: 38 passed, 5 failed
input_elided_subject_operator/4: verde
input_binary_predicate_coordination/4: rosso
seconda proposition con soggetto condiviso: assente
domanda sul secondo predicato: muro
```

## Conclusione architetturale conservata

Non serve un'altra regola sempre piu' profonda. Serve materializzare una volta
i frame locali come osservazioni opache e poco profonde, nello stesso modo in
cui `input_frame_record/3` e il bundle di commit evitano di rieseguire tutta la
catena semantica.

Il prossimo prototipo deve quindi introdurre una porta generale, non binaria:

```text
input_composition_atom(Scope, Observation)
    -> materializzazione meccanica di tutte le osservazioni locali
input_composition_record(Scope, Observation)
    -> composizione KB fra frame gia' osservati
```

Il C puo' enumerare e ripubblicare il termine `Observation` senza aprirlo, come
fa gia' per altri protocolli generici. La KB resta l'unico luogo che decide che
un'osservazione e' un frame binario, che il soggetto e' eliso e che il primo
`subject` deve essere condiviso col secondo evento.

## Testimone da riattivare

Il futuro dossier deve ripristinare almeno queste proprieta':

1. le tre frasi sopra producono due proposizioni ciascuna;
2. non viene mai committata la lettura `water produces oxygen`;
3. le domande sul primo e secondo predicato recuperano i rispettivi oggetti;
4. togliere la congiunzione spegne soltanto la composizione condivisa;
5. insegnare una congiunzione nuova parlando riattiva la stessa classe;
6. ordine inverso dei fatti e tre domini non cambiano il grafo canonico.

Questo reperto e' deliberatamente fuori dal runtime: conserva l'ipotesi e la
falsificazione senza spacciare un prototipo rosso per facolta' acquisita.
