# 09 — «Che cosa manca per rispondere?»

> **Prompt (#11 dei cento).**
> `What information is missing before comparing the populations of two cities?`
>
> **Esito:** «Hmm, I don't know about **missing** yet.»

## Il problema

Il turno non chiede un dato sul mondo: chiede **quali dati servirebbero**. È una
domanda sul metodo, e la risposta giusta è un elenco di slot — le due città, la
fonte, l'anno del censimento, la definizione di «popolazione» (area urbana o
comune amministrativo).

`missing` è naturalmente la parola nominata: è la prima di sei lettere senza
fatti in KB. Non c'entra niente.

## Perché non viene processato

1. **La domanda è di secondo livello.** Non «qual è la popolazione» ma «che cosa
   serve per confrontarla». È lo stesso salto della classe *meta* già incontrata:
   parrot0 risponde **sul mondo** e non **sulle condizioni per rispondere**.
2. **Il frame del confronto non dichiara i propri requisiti.** Anche dove il
   confronto funziona, gli slot che gli servirebbero non sono scritti da nessuna
   parte — quindi non sono nemmeno *elencabili*. Non si può rispondere «mi manca
   X» se X non è dichiarato.
3. **Manca la mossa «rispondi con i tuoi requisiti».** Un frame sa fallire; non
   sa raccontarsi.

## Cosa manca — e la buona notizia

Il pezzo che serve **esiste già, in un altro punto del sistema**: gen416 ha
costruito esattamente questo per la grammatica. Uno schema è una sequenza di
ruoli, e quando non si completa **dice quale ruolo manca**.

Qui serve la stessa cosa un piano più su: uno schema di **compito** invece che di
frase.

```prolog
task_schema(compare_populations, "citta_a citta_b fonte anno definizione").
```

E la mossa: davanti a una domanda che chiede i requisiti, si elencano gli slot
del task invece di provare a eseguirlo. Non serve nessun motore nuovo — serve
riusare il meccanismo di gen416 su un altro tipo di schema.

## Dove sta l'autocorrezione

Lo vede (`gap_kind = reachability`) ma non ha un rimedio: proporrebbe una cue,
e una cue non elenca requisiti.

Ed è il quarto dei nove casi che chiede **lo stesso rimedio** — dichiarare che
cosa deve esserci — dopo [05](05-confronto-senza-oggetti.md),
[06](06-artefatto-senza-schema.md) e in parte [01](01-orario-e-durata.md). Quattro
su nove, con la stessa forma di soluzione e nessuna via per arrivarci da soli.

Se una sola cosa va costruita dopo questi nove file, è quella: **un rimedio che
proponga uno slot mancante**, non una cue.

```prolog
remedy_for(incomplete_schema, slot).   % ← la riga che manca a quattro casi su nove
```
