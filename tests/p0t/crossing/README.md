# Crossing — prove comportamentali sulla KB viva

Questa cartella non descrive l'anatomia della KB e non certifica percorsi di
inferenza. I casi usano la KB completa, conoscenze reali e prompt naturali. Il
gate osserva soltanto ciò che parrot0 risponde: niente micro-mondi inventati,
`!query`, tracking o conoscenza iniettata dal test.

## Secondo livello del taglio logico

Il primo taglio è sintattico: fatti senza corpo (`F`) e regole con corpo (`R`).
Entrambi i lati vengono divisi ancora, sempre sulla clausola e non sul tema:

```text
F = Fs ∪ Fr
Fs = fatti con arità zero o uno       (singolari)
Fr = fatti con arità almeno due       (relazionali)

R = Ru ∪ Rm
Ru = regole con una premessa          (unipremessa)
Rm = regole con almeno due premesse   (multipremessa)
```

I due sottotagli sono esaustivi e disgiunti nella fotografia studiata. Sono una
lente teorica sostituibile, non quattro regioni residenti nella KB.

## Incrocio dei quattro frammenti

Prima dell'incrocio completo, due fotografie ternarie mantengono intero un lato
mentre dividono l'altro:

```text
facts_split_three.p0t   Fs | Fr | R
                        Fs <-> R, Fr <-> R, Fs <-> Fr attraverso R

rules_split_three.p0t   F | Ru | Rm
                        F <-> Ru, F <-> Rm, Ru <-> Rm attraverso F
```

In ciascun file le tre righe sono tre cross distinti, non tre stadi che mutano
la KB. Tutte le clausole sono già nella KB viva quando il test comincia.

`all_quadrants.p0t` verifica una sola catena reale:

```text
Fr  planet_order(earth, 3)
     └─ Ru  planet(X) :- planet_order(X, Order)

Fs  supports_life(earth)
     └─ Rm  habitable(X) :- planet(X), supports_life(X)
```

Le clausole vivono in `kb/facts/science-nature.p0`. La proiezione `planet/1`
vale per l'intera tabella degli otto pianeti e `habitable/1` per qualunque mondo
che soddisfi entrambe le premesse: non sono regole dedicate alla Terra o al
test. La prova positiva sulla Terra è accompagnata dal controllo negativo reale
su Mercurio, per impedire che il solo essere pianeta basti alla conclusione.

## Terzo livello: le aree logiche

Il taglio `Fs | Fr | Ru | Rm` è sintattico: divide le clausole per forma. Il
livello successivo divide per **che cosa la clausola parla** — mondo, lingua,
condotta — e dentro il mondo per area di realtà, usando il dominio che la KB
dichiara già in `wiki_concept/3`.

La divisione, il criterio con cui regge, e lo spoglio di che cosa la conoscenza
residente permette davvero di attraversare stanno in [`AREE.md`](AREE.md).

```text
areas_homonym.p0t          M-astronomia × M-chimica     (mercury)
areas_world_language.p0t   M × L                        (fatto inglese, domanda italiana)
```

Lo spoglio ha anche misurato il suo contrario: delle dodici clausole binarie che
collegano due aree dichiarate, **nessuna risponde a una domanda naturale**. Sono
archi disegnati e mai attraversati, e `AREE.md` li elenca per nome invece di
lasciarli fra le cose che si suppongono funzionanti.
