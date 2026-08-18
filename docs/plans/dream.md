# Il sogno — leggere come atto, e le lacune che dovrebbero colmarsi da sole

**gen405.** Questo documento nasce da due osservazioni di F. che non sono
miglioramenti del sogno ma una sua riformulazione, e da una terza domanda che le
tiene insieme: *cosa ci impedisce di essere in una situazione in cui le lacune si
autocolmano?*

---

## 1. La prosa da sola non è un atto — e c'è la misura

L'osservazione di F.: quando a un modello passi della prosa **e basta**, lui non
la apprende. La enuncia, la commenta, la confronta. Se la stessa prosa arriva
dentro «acquisisci queste informazioni», la tratta come conoscenza da
trattenere.

`tests/dream_intent_probe.py` la mette alla prova: stessa prosa — la pagina vera
di `photosynthesis` dal corpus — tre cornici, e si guarda la **reazione
immediata**, prima di chiedere qualunque cosa.

| cornice | cosa fa il modello |
|---|---|
| **nuda** — solo la prosa | «How would you like me to help with this passage—summarize, simplify, paraphrase, or fact-check it?» |
| **acquisizione** — «acquire the following information» | «Acquired. Key points: …» |
| **mirata** — «…so that you can answer: *does photosynthesis release oxygen?*» | «Yes. In oxygenic photosynthesis, plants, algae and cyanobacteria split water using light energy, releasing oxygen as a byproduct.» |

Tre atti diversi sullo stesso testo. Il primo **non impara**: chiede quale atto
compiere. Il terzo non riassume nemmeno — va dritto a chiudere la lacuna: **la
lettura è subordinata alla domanda.**

La conseguenza per parrot0 è secca, e va detta senza attenuanti: oggi `--dream`
chiede sempre la stessa cosa — `read the page on X` — a qualunque pagina, per
qualunque motivo. La lettura è uguale a sé stessa. **È la cornice nuda, senza
saperlo.**

## 2. Il sogno è una sidechain, e non dovrebbe esserlo

Oggi `dream.c` è un driver che decide da fuori: quali pagine leggere, in che
ordine, quando fermarsi, cosa contare come progresso. parrot0 esegue letture che
gli vengono passate. Il file dichiara di essere «uno strumento di osservazione,
non un test» — ed è onesto — ma resta che **imparare è una modalità del
programma, non un'attività dell'agente**.

Quello che il gen405 ha già spostato dalla parte giusta:

- **l'agenda viene dalle sue lacune.** `--dream` senza topic prende i semi da
  `pending_gap` (la parola ignota *è* un topic) e da `machinery_gap` (le parole
  di contenuto del turno che non ha saputo servire);
- **la verifica non è un giudizio.** Dopo ogni nodo produttivo si ri-pone il
  turno che murava: se ora risponde, `turn_done` ritira la lacuna da solo;
- **cio' che non ha saputo leggere lo sa dire.** Una frase che l'estrattore non
  legge diventa un `machinery_gap`, e il sogno la nomina.

Quello che è ancora fuori posto:

- **l'intenzione non è dichiarata.** La sonda dice che è l'intenzione a decidere
  cosa si trattiene, e parrot0 non ne ha nessuna: ha un solo modo di leggere.
  Dovrebbe esserci `reading_intent(Kind, …)` come fatto, e la scelta
  dell'intenzione dovrebbe essere sua — *leggo per sapere di più* è un atto
  diverso da *leggo per chiudere questo ponte*, e non devono rendere lo stesso;
- **la politica del sogno è C.** Profondità, ordine della frontiera, criterio di
  arresto: sono decisioni, e le decisioni di parrot0 stanno in KB. `dream.c`
  deve restare l'adattatore che porta le pagine e conta il budget.

## 3. La domanda vera: perché le lacune non si colmano da sole

> *«vedo che stai processando la conoscenza della prosa e le lacune come una tua
> assegnazione, ma l'idea dovrebbe essere che la tua assegnazione sia il processo
> di apprendimento e le lacune si dovrebbero autocolmare»* — F.

È esatto, ed è la critica giusta al lavoro di oggi: ogni lacuna trovata l'ho
diagnosticata io e l'ho chiusa io. Il registro delle lacune è, per adesso, una
lista di cose da fare **per me**.

Cosa lo impedisce, verificato nel codice e non dedotto:

### Barriera A — parrot0 non sa di che cosa è fatta la propria macchineria

Ha `numeric_cue`, `answer_frame`, `enumeration_cue`, `slot_evidence`,
`chitchat_reaction`. Non ha **nessun fatto che dica che quelle sono le forme con
cui un turno diventa rispondibile**. Perciò, davanti a `machinery_gap("what is
the total of 4, 5 and 6")`, non può proporre *«il fatto che manca è una
numeric_cue»*: quella conoscenza esiste solo nei commenti in prosa e nella testa
di chi scrive.

È la barriera più vicina, ed è la solita mossa del progetto: un registro
`bridge_shape(Genere, Predicato, Arietà)` — macchineria che descrive la
macchineria. Senza, lo spazio di ricerca di un rimedio è l'intera KB.

### Barriera B — la lacuna non ha un'ancora

`machinery_gap_record()` scrive il turno **e nient'altro**. Non registra quale
facoltà è andata più vicino, quali parole ha capito, cosa mancava. La traccia
esiste già (`b->trace_declined`, i moduli che hanno declinato) e non viene
scritta nel fatto.

Senza ancora, anche con la barriera A risolta, non c'è modo di restringere
l'ipotesi: si sa che *qualcosa* manca, non *dove*.

### Barriera C — nessuno propone e prova

Ed è quella già quasi costruita, il che rende le prime due ancora più decisive.
Esiste tutto:

| pezzo | dove |
|---|---|
| asserire in prova, senza impegno | `KB_HYPOTHETICAL` |
| ritirare tutto ciò che era in prova | `kb_retract_origin()` |
| ri-porre il turno e vedere se risponde | `retry_open_walls()` (gen405) |
| verificare che non si sia rotto altro | la suite a cricchetto |
| indurre una regola da esempi | `mod_induce` |

Il ciclo *proponi → asserisci in ipotetico → ri-poni il turno → tieni se
risponde, ritira altrimenti* si scriverebbe con quello che c'è. Manca **cosa**
proporre, ed è esattamente A più B.

### La barriera che non si supera: il C

Delle tre lacune chiuse oggi, due erano fatti — una `numeric_cue`, una
`enumeration_cue` — e parrot0 avrebbe potuto scriverle. La terza, la forma
plurale senza articolo, ha richiesto di modificare l'estrattore in C: **quella
non è autocolmabile per costruzione**, e nessun progresso sulle tre barriere la
renderà tale.

È la stessa linea di `question-emergence.md` §10 fra W e M, vista da vicino: non
passa fra *conoscenza del mondo* e *macchineria*, passa fra **macchineria in KB**
e **macchineria compilata**. Ogni forma di prosa che resta nel C è una lacuna che
parrot0 non potrà mai chiudere da solo — ed è l'argomento più forte, e più
concreto, per continuare a spostare il parser dentro la KB.

## 4. Ordine di lavoro

Il piano completo, in sette generazioni con i criteri di riuscita, sta in
[`question-emergence.md`](question-emergence.md), che a gen405 è diventato il
progetto dell'autocorrezione. Qui resta l'ordine in breve, perché è il sogno a
eseguirlo:

1. **L'ancora nella lacuna** (barriera B). `machinery_gap(Turno, Ancora,
   PiuVicino)`: cosa aveva capito, e chi è arrivato più vicino. È poco codice,
   usa una traccia che già esiste, e senza di essa le altre due non hanno
   ingresso.
2. **Il registro delle forme-ponte** (barriera A). `bridge_shape/3`: quali
   predicati rendono un turno rispondibile. È la conoscenza che oggi sta solo
   nei commenti.
3. **Il ciclo proponi-e-prova** (barriera C), con i pezzi già esistenti. La
   prima proposta da tentare è la più facile da generare e da verificare: una
   **cue** — quale sottostringa comune alle frasi non lette, promossa a
   `enumeration_cue`, farebbe entrare più fatti senza romperne nessuno.
4. **L'intenzione come fatto** (§2). `reading_intent`, scelta da parrot0 fra
   *sapere di più* e *chiudere questo ponte*, e passata all'estrattore. La sonda
   dice che cambia cosa si trattiene; finché non è dichiarata, il sogno legge in
   cornice nuda.
5. **La politica del sogno in KB**, e `dream.c` ridotto ad adattatore.

## 5. Come si misura se sta funzionando

Non «quanti fatti in KB»: quello misura quanto parrot0 **sa**. Le due misure che
dicono se ha **capito** di più, entrambe già stampate dal sogno:

- **ponti trovati** — turni che murarono e ora rispondono;
- **frasi non lette** — prosa che ha avuto sotto gli occhi e non ha saputo
  leggere.

Un sogno che estrae mille fatti e non chiude nessun ponte non è un successo
piccolo: è un dato, e dice che stava leggendo la cosa sbagliata. Tenere separate
le due misure è ciò che impedisce di scambiare l'accumulo per la comprensione.

E il criterio che riassume tutto: **il numero delle lacune aperte deve scendere
senza che nessuno le chiuda a mano.** Finché scende solo quando ci lavoro io, il
processo di apprendimento è la mia assegnazione, non la sua.
