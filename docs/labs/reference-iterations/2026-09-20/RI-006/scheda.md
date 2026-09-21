# RI-006 — si disdice con le parole con cui si è insegnato, e il ritiro arriva su disco

**ID, famiglia e contesto umano.** Famiglia: *ritrattazione* — la capacità di
**togliere**, che è l'altra metà di quella di aggiungere. Contesto umano: il
maestro sbaglia, se ne accorge, e vuole riprendersi ciò che ha detto. È la
situazione più ordinaria dell'insegnare, e non era possibile.

Il limite viene da RI-004, che lo aveva misurato e dichiarato.

**Commit/stato iniziale.** `a209504f`, profilo `agi`, lingua `en`.

**Stimolo congelato.** È un ciclo di due processi, non un turno:

```text
processo 1:  Turin is in France.            (un errore plausibile)
processo 1:  /save
processo 2:  forget that Turin is in France.
processo 2:  /save
shell:       grep -rn "turin, france" kb/
processo 3:  Where is Turin?
```

**Criterio.** Dopo il ritiro e il salvataggio, il falso non esiste più **in
nessun file**, e un processo nuovo non lo tiene. Il vero che lo circonda resta.

**Risposta iniziale e limite osservato** (`r1-step1.json`, `r1-step2.json`):

| turno | risposta |
|---|---|
| Turin is in France. | «Learned: turin is located in france.» |
| `/save` | `routed 15` — e `located_in(turin, france)` **entra** in `kb/core/world-facts.p0` |
| **forget that Turin is in France.** | **«Hmm, I don't know about forget yet.»** |
| Where is Turin? | «Italy, france and piedmont.» |
| `/save` | e la KB **cresce attorno al falso**: `holds_in(conversation, …)` e due `supersedes_in(…)` |

Due difetti in uno: la frase del ritiro non è capita, e il salvataggio
aggiunge credenze su una cosa falsa. Il gate di `LEARN_PROTOCOL` §8 — «X = 0»,
nessuna clausola falsa attiva — **non era raggiungibile parlando**.

## R4 — la cura, in due metà

**1. Dirlo.** `forget_fact` esisteva ma vuole UN verbo di relazione al posto
giusto; qui la superficie è la copula più la preposizione («is in»), che scrive
`located_in/2`. La strada c'era e non la leggeva nessuno: **ogni fatto letto
porta la frase che l'ha prodotto** (`reading_fact/2`, `fact_source/3` — la
provenienza del §6.8, finora scritta e mai consumata). La forma nuova
`forget_said` non rianalizza la frase: cerca **quale fatto quella frase ha
scritto**. Una superficie che si impara domani si disdice lo stesso giorno,
perché è la lezione stessa a lasciare la traccia.

Con il fatto se ne va la sua **scia**: quali predicati siano la scia è
conoscenza (`retraction_companion/1` — `fact_source`, `reading_fact`,
`holds_in`, `supersedes_in`), non una lista nel motore. `fact_mention/2` **non**
è nella lista, ed è una scelta: dice che un'entità è comparsa in una frase, e
quella frase è stata detta davvero. Il registro di ciò che si è detto non si
riscrive; si riscrive ciò che si crede.

**2. Portarlo su disco.** `kb_save_routed` sapeva solo aggiungere. Ora ogni
clausola ritirata lascia una **lapide** — `forgotten/1`, un fatto, non uno
stato nascosto — e il salvataggio la legge per togliere la riga dal file, con
la casa che dice il save-map e `sm_delete` come gemello di `sm_insert`. La
lapide è **riflessiva**: serve a questa sessione e non finisce nei file curati.

Due difetti trovati mentre si misurava, e corretti:

- il tetto di `kb_dump_pred` a 64 righe nascondeva **esattamente** il caso che
  interessa (`fact_source/3` ne ha centinaia, e le nuove stanno in fondo);
- l'estrazione della chiave per il save-map si fermava alla prima virgola, e
  con un argomento annidato (`fact_source(located_in(turin, france), …)`) dava
  la chiave `located_in(turin` — nessuna casa, riga non tolta.

## R5 — certificazione (`r5-certification.json`)

Con conoscenza vera e verificabile (`en.wikipedia.org/wiki/Po_(river)`: «ending
at a delta projecting into the Adriatic Sea»), in una sessione sola:

| turno | esito |
|---|---|
| `The Po flows into the Adriatic Sea.` + `The Adriatic Sea is a sea.` | appresi |
| `The Po flows into the Black Sea.` (falso) | appreso |
| `Which sea does the Po flow into?` | «adriatic sea.» |
| **`forget that the Po flows into the Black Sea.`** | **«Forgotten: the po flows into the black sea.»** |
| `Which sea does the Po flow into?` | «adriatic sea.» |
| `/save` | `routed 27` |

**Ablazione e selettività** — la prova che il ritiro toglie *solo* ciò che
nomina: nel ciclo Torino, dopo il ritiro `Where is Turin?` rende «Italy and
piedmont» (non più «france»), e nel processo nuovo `Is Turin in Italy?` rende
**«Yes: Turin is in Italy.»** mentre `Is Turin in France?` rende «Not as far as
I know». Il vicino di casa del falso non è stato toccato.

**Su disco**: dopo il ritiro, `grep -rn "turin, france" kb/core kb/machinery
kb/learning` rende **zero**. `flow(po, black_sea)` non è in nessun file.

## R6 — conteggio e processo nuovo

| categoria | n | clausole |
|---|---:|---|
| `W` fatti veri | **2** | `flow(po, adriatic_sea)`, `sea(adriatic_sea)` |
| `P` provenienza | 4 | `fact_source` ×2, `reading_fact` ×2 |
| `O` | resto | `utterance`, `fact_mention` |
| `X` | **0** | il falso insegnato è stato tolto, ed è il punto dell'iterazione |

**Processo nuovo** (`r6-fresh-process.json`): `Which sea does the Po flow
into?` → «adriatic sea.»; `Does the Po flow into the Black Sea?` → onesto «non
so»; e i replay di RI-003 («Naples.»), RI-005 («150 minutes.») e del ciclo
Torino. `make soft-test` verde in 12 s.

`persist.p0t` resta 31/6, `savemap.p0t` 10/10, `restore.p0t` 14/14,
`facts.p0t` e `basics.p0t` verdi — gli stessi numeri misurati in RI-004.

## Limiti residui

1. `Is the Adriatic Sea a sea?` resta un muro: è il limite dell'**accesso
   multiparola** già dichiarato in RI-001, non una regressione.
2. Il ritiro toglie ciò che la frase ha scritto; una conseguenza **dedotta** da
   quel fatto e salvata altrove non è inseguita.
3. `persist.p0t` ha una riga rossa proprio su questa specie, ma su un'altra via
   di salvataggio (`kb_save` con maschera d'origine) che questa cura non tocca.

**Stato dell'iterazione:** completa.
**Classificazione del training:** `trained` (W = 2, X = 0) — e per la prima
volta il gate «X = 0» è raggiungibile **parlando**, che è ciò che lo rende un
gate e non un auspicio.
