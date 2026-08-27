# A1 — progettazione del ponte “questo significa questo”

**Data:** 2026-08-27  
**Stato:** decisione prima dell'implementazione; alternative perdenti conservate.

## Problema misurato

Nel primo esperimento:

```text
> glints means glorphs
< muro

> mira glints kora
< muro
```

Il motore sa già usare `glorphs` dopo `glorphs is a relation verb`. Manca
quindi il ponte che dice che una nuova costruzione proietta sullo stesso frame,
non l'esecutore della relazione.

## Decisione

La rappresentazione candidata è:

```prolog
construction_frame(SourcePattern, TargetPattern, Predicate).
```

Esempio prodotto dalla conversazione:

```prolog
construction_frame("@S glints @O", "@S glorphs @O", glorphs).
```

`TargetPattern` deve essere già risolvibile tramite `extract_frame/2`. Questo è
il punto di bootstrap: una spiegazione può introdurre una forma nuova soltanto
se almeno il lato che la spiega è già compreso. Non si pretende di fondare due
simboli ignoti l'uno sull'altro.

La KB espone poi la vista:

```prolog
extract_frame($Source, $Predicate) :-
    construction_frame($Source, $Target, $Predicate).
```

Il lettore esistente applica così la costruzione senza sapere che sia stata
insegnata, da quale lingua provenga o quale relazione esprima.

Una seconda relazione conserva l'eventuale superficie interrogativa:

```prolog
construction_answer_cue(SourcePattern, Cue, Predicate).
```

Non è la comprensione stessa: evita che il fatto appreso resti morto nel caso
SVO semplice. Le forme interrogative più generali restano il gate A2.

## Confine C/KB

Il C può:

- trovare un pivot dichiarato in KB;
- separare due span;
- riconoscere variabili interrogando `rule_variable/1`;
- allineare il primo slot con `@S` e il secondo con `@O`;
- verificare che il target abbia un solo `extract_frame/2`;
- asserire o ritrarre il candidato nella sessione.

Il C non può:

- conoscere i letterali `means`, `significa`, `glints` o `glorphs`;
- decidere quale relazione esprima una parola;
- contenere una lista di costruzioni;
- generare una risposta inglese cablata;
- accettare un target che non sa già leggere;
- fingere di avere compreso un mapping di slot incompatibile.

Il pivot è un `intent_cue(teach_construction, Surface)` ed è esso stesso
insegnabile tramite `learnable/3`. La prova richiesta sarà:

```text
learn "denotes" as a construction teaching pivot
```

seguita da una lezione con `denotes`, e poi dall'ablazione del solo fatto-cue.

## Alternative considerate e non scelte

### 1. `relation_form(glints, glorphs)` soltanto

Era realizzabile interamente in KB e avrebbe chiuso il prompt singolo. Non
conserva però la costruzione né i ruoli: non può distinguere una forma diretta da
una forma inversa e confonde sinonimia lessicale con equivalenza strutturale.
Resta una vista secondaria eventualmente derivabile, non la rappresentazione
canonica.

### 2. `means(glints, glorphs)` → alias automatico

È troppo largo. `means/2` contiene definizioni, descrizioni e riassunti; assumere
che ogni suo oggetto sia il nome di un predicato trasformerebbe conoscenza
definitoria in grammatica eseguibile. Un target esplicitamente riconosciuto come
frame è una guardia necessaria.

### 3. Riscrivere la frase e rilanciare tutto il dispatcher

È potente ma introduce ricorsione, priorità e possibili loop fra costruzioni. Per
A1 non serve: il motore `extract_frame/2` è già il consumer condiviso da chat e
prosa. Il rilancio generale resta una struttura secondaria possibile quando una
costruzione dovrà mappare su atti non assertivi.

### 4. Insegnare una risposta esatta

È già possibile e il primo esperimento ne ha misurato il non-transfer. Non
produce slot, proposizioni o inferenza; resta memoria episodica e non conta per
A1.

### 5. Regola ricorsiva fra due `extract_frame/2`

```prolog
extract_frame($Source, $Pred) :-
    construction_equivalence($Source, $Target),
    extract_frame($Target, $Pred).
```

È elegante e consentirebbe catene, ma mette una ricorsione nello stesso
predicato enumerato a ogni frase. Conservare il predicato risolto al momento
della lezione evita cicli e rende visibile che cosa il teacher abbia davvero
ancorato. Le catene possono comunque essere insegnate perché un frame appreso è
già un target riconoscibile per la lezione successiva.

## Gate A1

1. target noto, sorgente ignota;
2. lezione `X source Y means X target Y` in chat;
3. replay sulla frase rossa;
4. transfer con almeno tre coppie nuove;
5. forma multiword nuova;
6. controesempio con un letterale diverso, che non deve essere assorbito;
7. lettura da prosa attraverso lo stesso frame;
8. domanda sul soggetto per il caso SVO semplice;
9. retract parlato della costruzione;
10. dopo retract, un esempio nuovo deve tornare rosso;
11. pivot inventato insegnato e poi ablato senza rebuild;
12. target sconosciuto o mapping incompatibile: rifiuto informato, mai
    “ho capito”.

## Residui dichiarati prima di iniziare

- la quarantena del candidato e la promozione automatica dopo prove hidden non
  esistono ancora;
- A1 allinea due slot espliciti; induzione da soli esempi concreti resta aperta;
- inversione dei ruoli e costruzioni con arità diversa devono essere rifiutate;
- le domande inverse e la morfologia restano A2;
- il retract conserva la provenienza della lezione ma non ha ancora una
  genealogia temporale completa degli stati attivo/disattivo.
