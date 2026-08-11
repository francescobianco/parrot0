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

## 4b. Uno strato `KB_MACHINERY`: provato e SCARTATO (gen374)

Tentativo scritto e buttato via lo stesso giorno, registrato qui perché la
ragione del rifiuto vale più del codice.

**L'idea.** Dopo che `!forget @base` si era portato via anche la grammatica
(§ test `kb_layers.p0t`), la mossa ovvia sembrava dare alla macchineria una
provenienza sua: un quinto strato, popolato al boot da una passata che sposta ogni
clausola il cui predicato è dichiarato `machinery/1`. Funzionava: tolto lo strato
base, parrot0 continuava ad analizzare le frasi.

**Perché è stato scartato.** Due ragioni, e la seconda è quella grossa.

1. **Ridondante.** `machinery(Pred)` *già* dice quell'informazione, in modo
   dichiarativo, e il filtro dell'auto-modello già la usa così. Lo strato prendeva
   lo stesso dato e lo ri-codificava come partizione in C, **congelata al boot**.
   Mantra #3 (astrai fino al punto fisso) e #5 (il motore esiste già).

2. **Andava nella direzione sbagliata.** Rendeva il sandbox *più comodo da
   abitare*. Ma il bersaglio (§5b) è farlo **sparire**: un'infrastruttura che rende
   l'amputazione tollerabile la consolida, perché toglie il dolore che serviva a
   eliminarla. Il muro trovato in `kb_layers.p0t` era un segnale, e quello strato
   lo zittiva invece di seguirlo.

**La distinzione da tenere ferma**, perché finora era confusa:

| | cosa fa | verdetto |
|---|---|---|
| **lettura con scope** (`kb_query_origin`) | vedere lo stesso fatto a livelli diversi e confrontarli | **serve la decisione** — è il contrario dell'isolare |
| **partizione da distribuire** (`KB_MACHINERY`) | pre-ordinare i fatti per consegnarne un sottoinsieme | **è isolamento con un nome migliore** |

**Cosa è stato tenuto al suo posto** — la stessa capacità, ma KB-first e viva:
`machinery/1` è insegnabile **a runtime**, come ogni altra classe.

```
> what do you know?              I know 13 fact(s) across 2 predicate(s).
> progenitor is a machinery      Learned: machinery(progenitor).
> what do you know?              I know 7 fact(s) across 1 predicate(s).
> !forget machinery(progenitor)
> what do you know?              I know 13 fact(s) across 2 predicate(s).
```

Blindato in `tests/p0t/language/taught_lexicon.p0t`. È strettamente più potente
dello strato: parrot0 può correggere il **proprio auto-modello** mentre gira,
senza ricompilare e senza che nessuno tocchi il C — cioè può apprendere e
migliorarsi da solo, che è il requisito da cui non si scende (F.).

## 4c. Il criterio: l'esperimento con l'LLM guida l'evoluzione (F.)

Regola di indirizzo, non un dettaglio di questo piano. Ogni proposta di
architettura si misura contro §5b: *l'LLM tiene entrambi i livelli e sceglie*.

Una proposta è nella direzione giusta se **aumenta ciò che parrot0 vede e la sua
capacità di decidere** fra le viste. È nella direzione sbagliata se **riduce ciò
che un suo pezzo vede** per ottenere la risposta giusta per costruzione — anche
quando è più semplice, anche quando i test diventano verdi. `KB_MACHINERY` è
caduto esattamente su questo test.

## 5. Perché non è stato fatto ora

Onestà sul rischio: `kb_query` è chiamata da tutto il motore. Aggiungere lo scope
significa decidere, per ogni sito, quale maschera è quella giusta — e un default
sbagliato non dà un errore di compilazione, dà una risposta diversa. Serve la
suite intera come rete, e `make test` è **rosso da prima** per uno snapshot
stantio (`tests/p0t/meta/introspect.p0t`: si aspetta 195 fatti, la KB ne ha 310).

**Prerequisito operativo:** rimettere verde `make test`. Finché l'unica rete è
`soft-test` — nove file scelti per il flusso rapido, non per la copertura — una
modifica di questa ampiezza non è verificabile, e va rimandata.

## 5b. La prova su un LLM reale — e perché cambia la conclusione (F.)

L'obiezione di F.: un LLM non ha alcun sandbox, eppure su quell'item non sbaglia.
Tiene **entrambi i livelli** — cosa implicano le premesse e cosa è vero nel mondo
— e **decide** quale gli stia venendo chiesto. Non è isolamento: è una decisione
adottata dalla conoscenza. Verificato, non supposto: `tests/premise_frame_probe.py`
(stesso endpoint di `llmscore`, `minimax-m2.5`, temperature 0).

Senza alcuna cornice, sull'item identico, il modello sceglie da sé la lettura
per entailment e la motiva nominando l'anello mancante:

> *"From these you cannot logically conclude that 'Rex is an animal'. […] The only
> valid syllogism you can form […] All cats are animals. Rex is a cat →"*

Chiedendo la lettura del mondo dà l'altra (*"a dog is a member of the kingdom
Animalia"*), e chiedendo entrambe le separa **nominando la loro relazione**:

> *"(b) […] This is a background fact that goes beyond the limited set of premises
> given."*

Il caso decisivo è quello in cui la premessa è **falsa nel mondo** — `"all birds
can fly. penguins are birds. can penguins fly?"`. Nessuno gliel'ha chiesto, e il
modello solleva da solo l'ambiguità:

> *"The answer depends on whether we treat the statement 'all birds can fly' as a
> true premise or as a simple assertion […] In that strict logical sense: yes.
> In the real world, however, the premise is false […] no."*

**Lo stesso ventaglio su parrot0 oggi:**

| Domanda | parrot0 | LLM |
|---|---|---|
| `is a dog an animal?` | `Yes.` | Yes |
| `rex is a dog. all cats are animals. is rex an animal?` | `No.` | No, con la spiegazione |
| entrambe le letture, etichettate | *impossibile* | le dà e le mette in relazione |
| `all birds can fly. penguins are birds…` | **muro** | rileva la premessa falsa |

parrot0 **sa già fare i due livelli** — `Yes` sul mondo, `No` sulle premesse. Ma
li fa con **due macchine diverse**, e la scelta fra loro non è una decisione: è la
FORMA sintattica dell'input che instrada silenziosamente all'una o all'altra. Non
c'è nessun punto in cui si decida quale cornice sia in gioco, quindi non si può
né dichiararla, né tenerle insieme, né accorgersi che una premessa contraddice il
mondo. La terza e la quarta riga della tabella non sono funzioni mancanti: sono
**inaccessibili per costruzione**, perché il sandbox è amputato apposta.

**Cosa ne segue per il piano.** Il §4 resta la strada giusta ma cambia di segno:
gli strati interrogabili non servono a *ricostruire l'isolamento in modo pulito*,
servono a **rendere la cornice una scelta**. Con `kb_query_scoped` parrot0 può
interrogare `KB_HYPOTHETICAL` da solo, o il mondo da solo, o entrambi e
confrontarli — e allora "quale cornice sta chiedendo questa domanda?" diventa una
domanda a cui si risponde **dalla conoscenza**, ispezionabile come ogni altra
(`why did you answer that way?`). È questo il senso pieno di KB-first qui: non
impedire strutturalmente un livello, ma **saper decidere** fra i livelli.

E il caso pinguini smette di essere un muro per diventare la capacità più
interessante del lotto: notare che una premessa data è falsa nel mondo si può fare
solo se si vedono entrambi gli strati.

## 6. Verdetto

La distinzione non doveva esistere, e la parte che faceva danno è chiusa: nessun
pezzo di parrot0 ragiona più senza la macchineria di parrot0, e il C non tiene
più copie di riserva del lessico. Ciò che resta è un residuo strutturale con una
causa precisa e una cura precisa — le provenienze sono metà implementate, vivono
in scrittura e non in lettura. Finché è così, «KB nuda» resterà una cosa che si
può costruire, e qualcuno la costruirà.

Ma la misura giusta non è più «isolare bene» (§5b). Un LLM su quello stesso item
non isola niente: vede tutto e **sceglie**. L'obiettivo quindi non è un sandbox
più pulito — è togliere di mezzo il sandbox e mettere al suo posto una decisione
presa dalla conoscenza. `brain_scratch_init` non va perfezionato: va fatto
sparire, e con lui l'ultimo posto in cui parrot0 pensa da menomato.
