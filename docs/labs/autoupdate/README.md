# Laboratorio `autoupdate` — sei dissezioni

> Appunti del 20 agosto 2026 (gen436). Sei prompt che parrot0 non chiude,
> sezionati con lo stesso metodo: **si verifica il fallimento, si smonta
> l'inferenza passo per passo, si scrive che cosa sarebbe servito** — e nessuna
> riga di codice. Servono a `docs/plans/autocrescita.md`: sono i casi su cui il
> ciclo deve accendersi, e la misura di quanto manca davvero.

| # | prompt | dove si ferma | `\|S\|` |
|---|---|---|---|
| [a1](a1.md) | *what is an okapi* | offre di cercare, e la catena passa dal disco | 3 righe + 1 collegamento |
| [a2](a2.md) | *how much does an elephant weigh* | **manca la domanda**, non il valore | 1 riga |
| [a3](a3.md) | *raccontami la fotosintesi* | il fatto c'è, in un'altra lingua | **1 regola** |
| [a4](a4.md) | *which is longer, the nile or the po* | **cinque passi su sei verdi**: mancano due numeri | 2 righe + 1 forma |
| [a5](a5.md) | *when was the eiffel tower built* | l'entità è nota e il sintagma non ci arriva | 2 righe |
| [a6](a6.md) | *in which country is the okapi found* | manca tutto: caso di **controllo negativo** | 2 righe |

## Che cosa dicono insieme

**1. La prosa vera non si legge, e questo è il collo di bottiglia.** In a1 («An
okapi is a giraffid artiodactyl mammal native to…») e in a4 («The Po is 652
kilometres long») l'estrattore restituisce **zero fatti** da frasi che un umano
legge senza sforzo. Anche con fetch, indirizzo e ripresa perfetti, quei turni non
finirebbero. **È una forma mancante, non un parser mancante**: la stessa specie
che il gen382 raccoglieva misurando il corpus.

**2. Le lacune hanno un ordine, e la domanda viene prima del valore.** a2, a5,
a6: finché non esiste la forma della domanda, il valore non è *mancante* — è
*irrilevante*. Un ciclo che vada a prendere valori senza avere la domanda riempie
la KB di roba che nessun turno sa chiedere: la deriva di §0a.

**3. L'indirizzo va risolto prima in casa.** a3 (traduzione) e a5 (composizione)
sono la stessa mossa: *«la cosa che mi chiedi, ce l'ho già sotto un altro
nome?»*. Costano una regola l'una e **evitano di scaricare ciò che si possiede**.

**4. Il declino migliore è già un indirizzo.** a4: *«I recognize a comparison on
length, but I don't have magnitudes for Nile and Po»* contiene, scritto,
**che cosa** estrarre e **dove**. Non serve inventare l'indirizzo: serve leggere
quel messaggio come una richiesta.

**5. Un caso su sei sa fermarsi bene, e va difeso.** a6 non deve partire: se
partisse leggerebbe una pagina intera senza sapere che cosa cercarci — cioè
`--dream` sotto mentite spoglie.

## Il candidato per gen437-438

**a4.** È il turno con il set minimo più piccolo e la prova più netta: prima non
finisce, dopo finisce, e la riga che l'ha chiuso si può togliere per vederlo
spegnersi — cioè esattamente la forma di risultato che il leitmotiv chiede.
