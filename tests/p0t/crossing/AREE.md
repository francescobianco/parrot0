# Le aree logiche della KB, e dove si attraversano

Questo file è il lavoro analitico che precede i test: **come si divide la KB in
aree logiche**, con quale criterio, e quali attraversamenti la conoscenza già
residente permette di provare. Non introduce conoscenza e non descrive un
percorso di inferenza: nomina i confini, e i `.p0t` accanto li attraversano con
domande naturali.

## Il criterio della divisione

Una divisione vale solo se regge quattro condizioni insieme:

1. **completa** — ogni clausola della KB sta in un'area;
2. **disgiunta** — in una sola;
3. **decidibile su una clausola isolata** — non serve sapere dove sta il file;
4. **intensione positiva bilaterale** — nessun lato è «tutto il resto».

La quarta è quella che si perde per prima. «Astronomia vs non-astronomia» passa
completezza e disgiunzione ed è barare: il lato negativo non ha struttura
interna e non si può dividere ancora.

## Livello 1 — che cosa denota il soggetto della clausola

Il taglio non è sul tema e non è sul file: è su **che cosa nomina il primo
argomento**. La domanda si decide guardando la clausola da sola.

```text
M  Mondo      il soggetto è una cosa del mondo
              planet_order(mercury, 1)   atomic_number(mercury, 80)
              color_of(blood, red)       sound_of(dog, woof)

L  Lingua     il soggetto è una forma linguistica
              tr(sangue, blood)          relation_verb(orbits)
              indefinite_article(a)      question_word(what)

C  Condotta   il soggetto è un atto del turno
              intent_cue(...)            local_tool(...)
              thinking_step(...)         answer_frame(...)
```

Tutti e tre i lati hanno un'intensione positiva: *cosa*, *forma*, *atto*. Un
elemento chimico è M anche se il file si chiama `world-facts.p0`; `tr/2` è L
anche quando traduce il nome di un pianeta. La prova è: *questa riga parla di
com'è fatto il mondo, di come si dice, o di cosa faccio del turno?*

## Livello 2 — dentro M, per area di realtà

Il secondo livello **non lo scegliamo noi**: la KB lo dichiara già, nel secondo
argomento di `wiki_concept/3`. Contate sulla fotografia corrente, le aree
dichiarate sono 50, e le più popolate sono physics, mathematics,
computer_science, biology, chemistry, computing, history, philosophy, economics,
medicine. Le aree in coda hanno un solo concetto: sono aree vere, ma con un
abitante solo non reggono ancora un attraversamento.

## Che cosa la KB permette di attraversare, oggi

Lo spoglio delle clausole binarie i cui **due** argomenti hanno un dominio
dichiarato dà 41 coppie: 29 restano dentro una sola area, **12 attraversano due
aree dichiarate**.

```text
grows_with      heat(physics)        × fire(chemistry)
constituent_of  water(chemistry)     × blood(medicine)
provides        ocean(geography)     × water(chemistry)
provides        river(geography)     × water(chemistry)
lacks           desert(geography)    × water(chemistry)
requires        bread(food)          × water(chemistry)
semantic_alias  climate(geography)   × climate_change(earth_science)
semantic_alias  learning(psychology) × machine_learning(computer_science)
semantic_alias  carbon(chemistry)    × carbon_cycle(earth_science)
compound_word   moon(astronomy)      × light(physics)
process_topic   photosynthesis(biology) × water(chemistry)
process_topic   photosynthesis(biology) × carbon(chemistry)
```

⚠ **Nessuna di queste dodici risponde a una domanda naturale.** Provate nella
forma diretta — *«does a river provide water?»*, *«is water a constituent of
blood?»*, *«does a desert lack water?»* — parrot0 non capisce la domanda o
risponde sul nome del predicato. L'arco fra le due aree è **disegnato, non
attraversato**: la clausola c'è, la strada che ci arriva no. È il caso che il
mantra chiama peso morto, e qui è misurato invece che sospettato.

Un test costruito su quelle righe proverebbe che la clausola esiste, non che le
due aree si incontrano. Per questo i casi qui accanto usano gli attraversamenti
che una domanda raggiunge davvero.

## I tre attraversamenti provati

```text
all_quadrants.p0t          M-astronomia × M-biologia
                           planet_order(earth,3) + supports_life(earth)

areas_homonym.p0t          M-astronomia × M-chimica
                           un solo nome, mercury, che vive in due aree

areas_world_language.p0t   M × L
                           il fatto è in inglese, la domanda è in italiano
```

### Perché l'omonimo è un attraversamento e non una coincidenza

`mercury` è **l'unico nome della KB che vive in due aree del mondo**: sta nella
tavola dei pianeti (`planet_order/2`, `rocky_world/1`, `orbits/2`) e nella
tavola degli elementi (`atomic_number/2`, `chemical_symbol/2`). Non è ambiguità
lessicale da risolvere: è la stessa stringa che denota due cose in due aree, e a
scegliere l'area è **la domanda**. Il controllo negativo è `earth`, che sta solo
in astronomia: chiederne il simbolo chimico non deve produrre un simbolo.

### Perché M × L è un attraversamento e non l'impalcatura del turno

Ogni turno usa la lingua, e contare questo come cross gonfierebbe qualunque
misura. Qui la clausola linguistica è **necessaria e nominata**: il colore del
sangue è in KB come `color_of(blood, red)`, in inglese; la domanda è in italiano
e non contiene la parola `blood`. Senza l'esonimo il fatto non si raggiunge;
senza il fatto l'esonimo non risponde. Le due aree servono entrambe.
