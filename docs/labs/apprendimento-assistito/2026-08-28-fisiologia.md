# 2026-08-28 — Fisiologia umana: il dominio scelto dai muri

Sessione condotta secondo [`LEARN_PROTOCOL.md`](../../../LEARN_PROTOCOL.md).

**Stato finale: `trained`** — otto fatti veri, `X = 0`, e un bug di misclaim
trovato *interrogando parrot0 in chat*, non da un test.

## 1. Come è stato scelto il dominio

Non a tavolino. Ho interrogato parrot0 con `make chat` su quattro aree —
medicina, diritto, economia, musica — e raccolto dove muriva.

| prompt | esito |
|---|---|
| what does the liver do? | «I don't have a module by that name» |
| what is insulin for? | muro |
| which organ produces bile? | muro |
| what is a contract? | muro |
| what makes a contract void? | il lettore ruba la parola *contract* |
| what is inflation? | **sa rispondere** |
| **what causes inflation?** | **`Learned: causes(what, inflation).`** |
| what is a fugue? | muro |
| how many strings does a violin have? | **sa rispondere** |

La fisiologia era il grappolo di muri più denso, ed è diventata il dominio.
Baseline dedicata: **sei domande di fisiologia, sei muri**, nessuna risposta
falsa.

## 2. Il bug trovato per primo, che valeva più del dominio

```
> what causes inflation?
parrot0: Learned: causes(what, inflation).
```

Una **domanda scritta in KB come fatto falso**, con l'interrogativo promosso a
entità. `mod_cause` asseriva su qualunque turno di tre parole con il verbo
causale in mezzo, e «what causes X?» ha esattamente quella forma. È il caso
peggiore del mantra #7 — peggiore di un muro, perché persiste oltre il turno.

Doppiamente sbagliato: `causes(fire, smoke)` è in `world-facts.p0` da sempre, e
«what causes smoke?» invece di rispondere *fire* imparava `causes(what, smoke)`.

Riparato in due metà — la guardia (se il turno apre con un interrogativo sta
chiedendo, non affermando: la stessa che l'estrattore di classi aveva già) e la
risposta, che semplicemente **non esisteva**: la forma era rappresentata solo
come asserzione. Commit `6c898e5`.

```
> what causes smoke?    → fire.
> what causes tsunami?  → earthquake.
```

## 3. Fonti e conoscenza acquisita — `W = 8`

Guyton & Hall, *Textbook of Medical Physiology*; Gray's Anatomy.

| fatto | |
|---|---|
| il fegato produce bile | `produces(liver, bile)` |
| il pancreas secerne insulina | `secretes(pancreas, insulin)` |
| i reni filtrano il sangue | `filter(kidneys, blood)` |
| il cuore pompa il sangue | `pumps(heart, blood)` |
| l'emoglobina trasporta ossigeno | `carries(hemoglobin, oxygen)` |
| lo stomaco digerisce le proteine | `digests(stomach, protein)` |
| la tiroide produce tiroxina | `produces(thyroid, thyroxine)` |
| la ghiandola salivare secerne amilasi | `secretes(salivary_gland, amylase)` |

`L = 6` verbi relazionali aperti parlando.

**Il metodo che ha evitato un secondo avvelenamento**: prima di usare un verbo,
`grep` su `src/` e `kb/` per vedere se è già posseduto. `exchanges` era già in
`lexicon.p0` ed è stato scartato senza pronunciarlo; gli altri sei erano puliti.
È lo stesso accorgimento della sessione ML — e resta il sintomo che **manca a
parrot0 un controllo di collisione** quando impara un verbo.

## 4. Gate

| Gate | Esito |
|---|---|
| **replay** | **verde sul prompt che ha aperto la sessione**: «which organ produces bile?» → *Liver.* |
| transfer | verde: tiroide/tiroxina, ghiandola salivare/amilasi — mai pronunciati nella spiegazione |
| contrasto | verde: «the appendix produces what?» mura invece di inventare |
| retention | verde dopo molti turni diversi |
| FreshProcessRecall | **6/6 = 100%** |
| mantra | verde: un membro nuovo imparato da un **processo nuovo** senza rispiegare la forma |

## 5. Gap tipati

**F1 — M5, la forma flessa e quella base sono due abilità scollegate *in
ingresso*.** La frase vera è «the kidneys filter blood», al plurale. Insegnare
`filters` non la fa leggere:

```
> filters is a relation verb
> the kidneys filter blood
parrot0: Hmm, I don't know about filter yet.
```

È `studies`/`study` del piano, §6.2 M5. Ma il lato **risposta** il ponte ce l'ha:
dopo aver insegnato `filter`, «the kidneys **filters** what?» risponde lo stesso,
via `lemma_candidate/2`. Il divario è quindi in ingresso, non in uscita — una
precisazione che il piano non fa e che restringe il lavoro da fare.

**F2 — un'asserzione con oggetto già noto viene letta come domanda.**

```
> the spleen filters blood
parrot0: Kidneys.
```

`filter(kidneys, blood)` esisteva, e la frase nuova — che è un'**asserzione** su
un soggetto nuovo — è stata risolta come query sull'oggetto. Il fatto sulla milza
non è stato imparato. Stessa famiglia dell'over-claim di `answerframe` nel report
elettronica: si risponde prima di aver deciso se il turno chiede o afferma.

**F3 — resa incoerente della risposta.** «the salivary gland secretes what?»
risponde *amylase.* in minuscolo, dove gli altri slot rispondono capitalizzati
(*Bile.*, *Insulin.*). Cosmetico, ma è una frase rivolta all'interlocutore.

## 6. Una sessione abbandonata, di nuovo

Il primo processo conteneva `causes(what, inflation)` e `causes(what, deflation)`
— prodotti dalle sonde **prima** che il bug del §2 fosse riparato, nello stesso
processo lungo. Con `X = 2` il §8 vieta il salvataggio e vieta di contare sulla
pulizia manuale successiva.

Il processo è stato chiuso e le lezioni ridate da zero su un processo nuovo che
girava il binario **corretto** — il che ha anche verificato la riparazione
end-to-end. `git status` dopo l'abbandono: pulito.

## 7. `/save` e sparpagliamento

```
parrot0: routed 65 clause(s)      (sessione)
parrot0: routed 19 clause(s)      (processo nuovo, amilasi)
```

`X = 0`. Nessuno scratch di turno nell'albero curato.

Sette predicati erano una specie nuova senza parenti e sono finiti nella
ricaduta; hanno avuto la casa che mancava — `kb/core/facts/physiology.p0`, la
quarta aperta con lo stesso criterio di `units.p0`,
`scientific-discourse.p0` e `machine-learning.p0`. L'amilasi, insegnata dopo,
è stata instradata **direttamente lì** senza intervento.

**`kb/learning/learned.p0` è a zero clausole.**

## 8. Metriche

```text
LessonYield            = 8/9    (`filters` scartato per il divario di flessione)
Contrast               = 1/1
Retention              = pass
FreshProcessRecall     = 6/6 = 100%
FalseUnderstandingRate = 0      (l'unico fatto falso non e' stato salvato, ed e' stato riparato nel motore)
WorldKnowledgeGain     = 8
Clausole invalide (X)  = 0
```
