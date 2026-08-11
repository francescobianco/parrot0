# Una sola KB

*Non deve esistere, per definizione, una distinzione fra «KB nuda» e «KB
caricata». La KB è parte di parrot0: un parrot0 costruito senza la conoscenza di
parrot0 non è lo stesso soggetto con meno dati, è un altro soggetto. Questo piano
dice dove quella distinzione è nata, cosa è già stato chiuso (gen371) e qual è il
divario di design che resta.*

> **Stato (2026-08-12):** il sintomo è chiuso — nessun brain di scratch è più
> privo della macchineria di parrot0, e le liste di parole che facevano da rete
> di sicurezza nel C sono state cancellate. **La causa è ancora aperta:** §3.

---

## 0. Da dove viene la distinzione

Non da una decisione, ma da un'esigenza legittima risolta con lo strumento
sbagliato. Alcuni ragionatori devono valutare **premesse ipotetiche** senza che
sporchino la conoscenza vera:

```
"if all cats are mammals and Tom is a cat, is Tom a mammal?"
```

Le premesse valgono per questo turno soltanto, e la domanda va decisa **solo** da
esse: se la KB del mondo intervenisse, `"rex is a dog. all cats are animals. is
rex an animal?"` risponderebbe `Yes` — la risposta comoda e sbagliata, perché
parrot0 *sa* che i cani sono animali ma le premesse non lo implicano. La risposta
corretta è `No`, ed è closed-world sulle premesse.

L'isolamento è quindi **giusto**. Il modo in cui era ottenuto no: si costruiva un
secondo `Brain` sopra un `kb_create()` nudo. Isolare *ricominciando da zero*
funziona per i fatti del mondo, ma butta via anche tutto il resto — grammatica,
classi lessicali chiuse, conoscenza di instradamento. Il sandbox non sapeva
distinguere un articolo da un sostantivo.

## 1. Il costo, misurato

Il danno non era teorico: **bloccava la migrazione KB-first**. Ogni classe
lessicale portata dal C alla KB (`universal_quantifier/1`, `definite_article/1`,
`relation_preposition/1`, `asks_slot/2`) doveva conservare nel C una lista di
parole di riserva, perché nel sandbox la lookup non avrebbe trovato niente:

```c
static const char *const d[] = { "all", "every", "any" };   /* la rete di sicurezza */
```

Cioè: si toglieva l'inglese dal motore e lo si rimetteva subito accanto. La
migrazione non poteva mai chiudersi davvero.

## 2. Cosa è stato chiuso (gen371)

I sei siti che costruivano un sandbox a mano passano ora da un'unica costola,
`brain_scratch_init(scratch, parent)`. Il sandbox possiede ancora una KB **vuota
per i fatti** — l'isolamento closed-world è intatto — ma porta un collegamento
NON possessivo alla KB del genitore, consultato **solo** per la macchineria di
parrot0 (`brain_substrate_query`). I fatti del mondo non hanno alcun percorso per
entrare: il collegamento è raggiungibile da una sola funzione, il lookup delle
classi lessicali.

Risultato concreto: **tutte le liste di riserva nel C sono state cancellate.** Le
classi vivono solo in KB, e restano insegnabili a runtime dentro il sandbox come
fuori (`tests/p0t/language/taught_lexicon.p0t`). Verificato che l'isolamento
regge: `rex is a dog. all cats are animals. is rex an animal?` risponde ancora
`No`.

## 3. Il divario di design che RESTA

`brain_scratch_init` è un **ponte sul divario, non la sua eliminazione**. La
distinzione «KB nuda / KB caricata» esiste ancora: c'è ancora un secondo oggetto
KB, e ancora due modi di interrogare. La domanda giusta è perché serva un secondo
contenitore per ottenere una vista ristretta.

**La causa esatta.** parrot0 ha già le PROVENIENZE — `KB_BASE`, `KB_SESSION`,
`KB_INDUCED`, `KB_REFLECTIVE`, una maschera di bit su ogni fatto (`src/kb.h:30`).
Ma sono usate **solo per la persistenza**:

```c
int kb_save(const KB *kb, const char *path, int origin_mask);   /* scope: SÌ */
int kb_query(const KB *kb, const char *pred, ...);              /* scope: NO  */
```

`kb_save` sa restringersi a uno strato. `kb_query` no: cerca sempre in tutto.
**Il concetto di strato esiste per scrivere e non per leggere** — ed è
esattamente per questo che, per ottenere una vista ristretta, l'unica strada è
costruire un contenitore che contenga solo quello strato.

## 4. La chiusura

Portare le provenienze anche in lettura. Un'ipotesi diventa uno **strato**, non
una seconda KB:

```
KB_HYPOTHETICAL      una nuova provenienza per le premesse del turno
kb_query_scoped(kb, mask, ...)   interroga solo gli strati richiesti
kb_retract_origin(kb, mask)      butta via lo strato a fine turno
```

Il ragionamento sulle premesse diventa allora: asserisci in `KB_HYPOTHETICAL`,
interroga con `KB_HYPOTHETICAL | machinery`, ritratta lo strato. Stessa semantica
closed-world di oggi, **una sola KB**, nessun secondo `Brain`, nessun substrato da
collegare — e `brain_scratch_init` sparisce invece di essere mantenuto.

Ne discende anche il resto: `!forget` del test-engine e l'amputazione via
`PARROT0_WORLD_FACTS=0` sono due modi diversi di dire «restringi la vista», e con
gli strati interrogabili diventano la stessa cosa detta bene.

## 5. Perché non è stato fatto ora

Onestà sul rischio: `kb_query` è chiamata da tutto il motore. Aggiungere lo scope
significa decidere, per ogni sito, quale maschera è quella giusta — e un default
sbagliato non dà un errore di compilazione, dà una risposta diversa. Serve la
suite intera come rete, e `make test` è **rosso da prima** per uno snapshot
stantio (`tests/p0t/meta/introspect.p0t`: si aspetta 195 fatti, la KB ne ha 310).

**Prerequisito operativo:** rimettere verde `make test`. Finché l'unica rete è
`soft-test` — nove file scelti per il flusso rapido, non per la copertura — una
modifica di questa ampiezza non è verificabile, e va rimandata.

## 6. Verdetto

La distinzione non doveva esistere, e la parte che faceva danno è chiusa: nessun
pezzo di parrot0 ragiona più senza la macchineria di parrot0, e il C non tiene
più copie di riserva del lessico. Ciò che resta è un residuo strutturale con una
causa precisa e una cura precisa — le provenienze sono metà implementate, vivono
in scrittura e non in lettura. Finché è così, «KB nuda» resterà una cosa che si
può costruire, e qualcuno la costruirà.
