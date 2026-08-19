# Le analisi dei prompt che falliscono

Un file per **prompt rappresentativo** dei cento di
[`../plans/parrot0-100-failures.md`](../plans/parrot0-100-failures.md). Non uno
per prompt: uno per **forma di fallimento**, perché i cento non sono cento
problemi — sono cinque o sei problemi con la stessa etichetta appiccicata sopra
dal ripiego (vedi [`../autocorrezione.md`](../autocorrezione.md) §3c).

Ogni file ha la stessa struttura, e la parte che conta è la terza:

1. **Il prompt e l'esito** — che cosa risponde oggi, letterale;
2. **La traccia** — il prompt smontato in pezzi, ognuno provato da solo. È qui
   che si scopre quasi sempre che i mattoni ci sono e manca la fila;
3. **Perché non viene processato** — le cause, numerate, in ordine di catena.
   Nessuna è «non conosce la parola X»: quella è la conseguenza;
4. **Cosa manca** — e se è conoscenza o motore;
5. **Dove sta l'autocorrezione** — se il ciclo lo vede, e se ha un rimedio.

## La regola che questi file servono a non dimenticare

Il messaggio d'errore **non nomina mai la causa**. `not_understood` è il fondo
della catena e, non sapendo che dire, nomina la prima parola di sei lettere su
cui la KB non ha fatti. «ground», «missing», «matters», «travels» non sono il
problema: sono il sintomo che il turno non è stato letto da nessuno.

## Indice

| file | forma | prompt |
|---|---|---|
| [01-orario-e-durata](01-orario-e-durata.md) | numeri/tempo | *A train leaves at 14:30 and travels for 2h45m…* |
| [02-contrapposizione](02-contrapposizione.md) | logica | *What is the contrapositive of if it rains then the ground is wet?* |
| [03-traduzione](03-traduzione.md) | superficie | *Translate the dog runs into Spanish.* |
| [04-dimenticare](04-dimenticare.md) | ritrattazione | *Forget my name.* |
| [05-confronto-senza-oggetti](05-confronto-senza-oggetti.md) | muro cieco | *Compare two graphs structurally.* |
| [06-artefatto-senza-schema](06-artefatto-senza-schema.md) | schema assente | *Write a decision record with alternatives…* |
| [07-template-fuori-bersaglio](07-template-fuori-bersaglio.md) | risposta non pertinente | *Explain a counterexample to every swan is white.* |
| [08-sillogismo-non-motivato](08-sillogismo-non-motivato.md) | risposta secca | *If all doctors are scientists and some scientists are artists…* |
| [09-metadomanda-sul-metodo](09-metadomanda-sul-metodo.md) | domanda-wh | *What information is missing before comparing two cities?* |

## Che cosa dicono i nove, letti insieme

Non erano scritti per convergere, e convergono.

**Quattro su nove chiedono lo stesso rimedio: dichiarare che cosa deve esserci.**
Gli slot vuoti di un confronto ([05](05-confronto-senza-oggetti.md)), le sezioni
di un artefatto ([06](06-artefatto-senza-schema.md)), i requisiti di un compito
([09](09-metadomanda-sul-metodo.md)), i ruoli dei numeri in un problema di orario
([01](01-orario-e-durata.md)). È la stessa forma di soluzione — una lista di
posizioni obbligatorie — e nessuno dei quattro la può ottenere oggi, perché il
ciclo di autocorrezione sa proporre **solo cue**.

**Tre su nove non sono nemmeno visibili.** [04](04-dimenticare.md) risponde «Nice
to meet you» a una richiesta di dimenticare; [07](07-template-fuori-bersaglio.md)
produce sei righe che non nominano mai un cigno; [08](08-sillogismo-non-motivato.md)
dà la risposta giusta senza averla derivata. Tutti e tre passano per turni
riusciti. Sono la classe `wrong_answer` che §9.1 di
[`../plans/question-emergence.md`](../plans/question-emergence.md) aveva nominato
nel 2026 e che nessuna generazione ha implementato.

**Uno su nove è già risolvibile con una riga.** [03](03-traduzione.md): la
capacità c'è (`how do you say dog in spanish` → «Perro»), manca la riscrittura di
superficie che ci porti l'imperativo. È anche l'unico in cui il ciclo avrebbe un
oracolo interno — provare una riscrittura e vedere se il muro cade.

**In nessuno dei nove la causa è la parola nominata dal messaggio d'errore.**
`travels`, `contrapositive`, `missing`, `forget`, `compare`: cinque sintomi, zero
cause. Se questi file servono a una cosa sola, è a rendere quel fatto difficile da
dimenticare.

## Riletti come richieste di insegnamento (gen430)

`../autocorrezione.md` §13.4 rilegge questi nove con una domanda sola: **esiste
una frase che l'interlocutore potrebbe dire, e che chiuderebbe il caso?** Se sì,
il caso è automatizzabile in dialogo, e ciò che manca è solo chi la chieda.

Il conto che ne esce non è quello che ci si aspetta: **tre si chiudono con una
frase che parrot0 sa già capire** (01, 04, e quasi 03), **quattro chiedono tutti
la stessa cosa** — uno schema di ruoli dicibile (05, 06, 09, e 01 per metà) — e
**due chiedono la pertinenza** (07, 08). Il collo di bottiglia non sono nove
problemi: sono due.
