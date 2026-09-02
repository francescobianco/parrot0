# Quanto manca — valutazione misurata, 2026-09-02

*Domanda di F.: «quanto manca affinché parrot0 sia addestrabile da un LLM a
rispondere bene alla maggioranza delle domande, e quanto manca per finire la
metacomprensione».*

Misurato prima di opinare, sulla batteria held-out di `tests/llmscore-probes/`
(70 domande, 7 classi, nessuna vista in addestramento).

## 1. Il numero

```text
muri              0 / 70        0%      (baseline della campagna: 31%)
risposte buone   44 / 70       63%
risposte sbagliate 7 / 70      10%      forma giusta, contenuto sbagliato
turni rubati     19 / 70       27%      risposta sicura e ESTRANEA alla domanda
```

**Il metro del muro è saturo e non discrimina più.** Il difetto dominante non è
più «non so»: è **«rispondo con sicurezza una cosa che non c'entra»** — 27%
contro il 10% di errori veri.

Per classe (buone / 10):

```text
deduction   9    factual   9    causal   7    lexical   7
arithmetic  5    pragma    4    creative  3
```

## 2. Chi ruba i turni — il registro, con il modulo per ciascuno

Chiesto a parrot0 stesso (`who answered?`) sulle dieci risposte peggiori:

| domanda | modulo | che cosa fa |
|---|---|---|
| *Describe a sunrise in one vivid sentence* | **analysis_family** | sei frasi di metodologia progettuale |
| *Give me a piece of advice for someone starting a new job* | **analysis_family** | idem |
| *Explain what you are without pretending to be human* | **analysis_family** | boilerplate causale |
| *Tell me a short story about a lonely robot…* | **gen** | «Tell was a mysterious Tell» — prende il **verbo imperativo come protagonista** |
| *Give me three words that rhyme with "night"* | **gen** | «Scattering needs sunlight» — un artefatto `concise_explain` su una domanda lessicale |
| *What can you spell with the letters c, a, t?* | **spell** | «l-e-t-t-e-r-s» — scandisce una parola **della domanda** |
| *What would you do if you won the lottery?* | **chitchat** | un saluto |
| *Summarize the idea of friendship in one sentence* | — | «We talked about oranges, give, away and many» — **sanguinamento** da turni precedenti |
| *Half of a number is 14* | **arith** | 7 invece di 28: non inverte l'operazione |
| *I buy 4 packs of 6 pencils* | **wordproblem** | 10 invece di 24: somma invece di moltiplicare |

**Cinque su dieci vengono da due moduli** (`analysis_family` 3, `gen` 2), e tutti
e cinque hanno la stessa forma: **una facoltà rivendica un turno che non sa
onorare e, invece di tacere, emette qualcosa.**

## 3. La risposta alla prima domanda

> *Quanto manca perché un LLM possa addestrarlo a rispondere bene?*

**Il 63% è già lì, e il 27% che manca NON è insegnabile con dei fatti.** Un LLM
che insegnasse conoscenza a parrot0 non chiuderebbe nessuno dei dieci casi sopra:
non manca sapere, manca **condotta**. Nessun fatto nuovo impedisce ad
`analysis_family` di rispondere a «descrivi un'alba in una frase».

Ma la condotta **è diventata conoscenza in questo ciclo** — `faculty_force/2`,
`faculty_yield/3`, `faculty_yield_force/3`, il residuo come vincolo, il gap
dichiarato. Quindi la risposta esatta è:

> Manca **applicare lo strato di condotta alle facoltà del registro §2**. Sono
> cinque moduli, non cinquanta, e ciascuno costa righe di KB più un punto di
> lettura. Fatto quello, un LLM può correggere il resto **parlando**, perché la
> condotta sarà dicibile.

Il 10% di errori veri (aritmetica, `this shape`) è un'altra cosa e si chiude a
parte: sono letture sbagliate, non turni rubati.

## 4. La risposta alla seconda domanda — la metacomprensione

> *Quanto manca per finire la metacomprensione?*

La metacomprensione è **sapere quando non si sa**, e il §1 la misura
esattamente: 19 turni su 70 in cui parrot0 **non sapeva di non sapere**. Non ha
prodotto un muro — ha prodotto un template.

Il pezzo che manca ha un nome preciso, ed è **uno solo**:

> **⛔ Una risposta che viola un vincolo esplicito del turno non è una risposta.**

«Describe a sunrise **in one vivid sentence**» riceve sei frasi. «Answer **in
exactly three words**» è già gestito da `word_count_cue/2` perché qualcuno ci ha
lavorato; «in one sentence» no, perché nessuno ci ha ancora lavorato — e questo
è il punto: **il vincolo di forma è letto caso per caso invece che una volta**.

È lo stesso difetto di `namestart` («tell me a country in asia» → andorra) a un
livello più alto: là si buttava via un vincolo di *contenuto*, qui uno di
*forma*. La cura ha la stessa forma:

```text
il turno dichiara una misura  ->  la risposta che la viola non viene emessa
                              ->  la facoltà declina e il turno prosegue
                              ->  se nessuno la rispetta, si dichiara il gap
```

E deve stare **dopo il dispatch**, non dentro ogni facoltà — altrimenti è di
nuovo un elenco di incidenti. È lo «strato post-dispatch» che
`docs/plans/initiative.md` segnala mancante da tre cicli.

## 5. L'ordine di lavoro che ne esce

1. **Il vincolo di forma come lettura condivisa, post-dispatch.** Chiude da solo
   una parte del 27% e dà la metacomprensione la sua unica misura oggettiva:
   *parrot0 emette qualcosa che il turno aveva escluso?*
2. **`analysis_family` dichiara la sua condotta** (3/10 del registro). È il
   singolo intervento con il rapporto più alto.
3. **`gen` non prende il verbo imperativo come contenuto.** «Tell me a story
   about X» → il protagonista è X, non «Tell». È la stessa lettura della forza
   del turno, applicata dentro la facoltà invece che al suo ingresso.
4. **Le due letture aritmetiche sbagliate** («metà di un numero è 14», «4
   confezioni da 6») — inversione e moltiplicazione.
5. **`this shape`** — il dimostrativo non risolve nel sillogismo.
6. **`fammi una domanda`** — l'iniziativa, che è l'unica capacità *assente* del
   lotto (le altre nove ci sono e sbagliano).

## 6. La regola di misura, perché il prossimo non ricominci da zero

**Non usare più il tasso di muro come metrica di avanzamento: è a zero.** La
misura che discrimina ora è a tre bucket, e il terzo è quello che conta:

```text
buona | sbagliata | ESTRANEA
```

Una risposta estranea è **peggiore di un muro** e va contata a parte, perché un
muro si vede e una risposta sicura no — è la stessa ragione per cui
`tell me a country in asia -> andorra` era più grave di un «non lo so».


---

# Aggiornamento — primo giro sui ladri di turno (2026-09-02)

*F.: «questi turni rubati sono un problema ma anche l'occasione di ripensare chi
ruba il turno, fondarli sul principio KB-first e poi addestrare via prompt a
essere più preciso attraverso indicazioni precise».*

## Che cosa è cambiato sulla batteria

Tre risposte su settanta, tutte sui casi peggiori del §2:

| turno | prima | ora |
|---|---|---|
| *Describe a sunrise in one vivid sentence* | sei frasi di metodologia | **una descrizione dell'alba, in una frase** |
| *Tell me a short story about a lonely robot…* | «Tell was a mysterious Tell» | gap che **nomina ciò che ha letto**: «a lonely robot who finds a friend» |
| *Continue: "The letter arrived…"* | «Continue was a mysterious Continue» | «It was a mysterious it» — meno peggio, ancora sbagliato |

## Le cure, e perché sono di classe e non di caso

1. **Un vincolo sulla risposta non fa parte del soggetto.** L'estrattore di
   `analysis_family` prendeva tutto ciò che segue la cue, misura della risposta
   inclusa: «a sunrise **in one vivid sentence**» diventava il tema, e il piano
   parlava di *progettare una frase vivida*. `answer_shape_cue/1` è **derivata**
   da ogni misura dichiarata — nessun membro da aggiornare, e una misura nuova si
   porta dietro anche questa lettura.
2. **La soglia di una facoltà è conoscenza.** `analysis_family` rivendicava ogni
   turno più lungo di **40 caratteri**, e il quaranta era nel C. Ora è
   `faculty_min_turn_length/2`: una soglia cablata è una condotta che nessuno può
   correggere parlando — il mantra #17 in forma numerica.
3. **La parola con cui si chiede non è ciò che si chiede.** Il protagonista si
   raccoglieva per **maiuscola**, e la maiuscola di inizio frase è ortografia.
   Ora un'apertura di richiesta dichiarata (`imperative_opener/1`,
   `request_opener/1`) e una parola che `subject_guard/1` esclude non possono
   diventare personaggi.
4. **Il tema si legge, non si indovina.** `creative_topic_tail` — la lettura
   strutturale del tema, già usata per dialoghi e metafore — non era applicata
   alle storie. Ora la struttura viene **prima** dell'ortografia.

## ⛔ Due esperimenti fatti e RITIRATI — scritti perché nessuno li rifaccia

- **Togliere il lead di `analysis_family`.** Sembrava giusto: una famiglia
  metodologica non è più specifica di nessun modulo, quindi la sua precedenza non
  dovrebbe essere posizionale. Misurato: **due turni peggiorati**. Senza il lead,
  *«Explain what you are…»* → «Alright — I am Without now» (un modulo di ruolo!) e
  *«How would you design a timekeeping system…»* → l'artefatto sulla specie
  aliena Aurakai. La famiglia stava **proteggendo** quei turni da rivendicazioni
  peggiori. **Togliere un guardiano non è togliere un difetto.**
- **La guardia `subject_guard/1` sulla testa del soggetto d'analisi.** Non
  toccava il turno che doveva curare e ne rompeva un altro: *«advice for someone
  starting a new job»* → «From what I know, a good plan is:» **e nient'altro** —
  perché «someone» è un quantificatore aperto, cattivo soggetto per un *frame* e
  tema legittimo per un'*analisi*. Un boilerplate si legge, una promessa vuota no.

## ⛔ Il giudizio di F. sul generatore di storie, e perché va accolto

> *«questo modulo per quello che vedo è una monnezza: se sceglie il protagonista
> per forma con un'espressione senza applicare la comprensione universale, vuol
> dire che è molto indietro rispetto ai mantra.»*

È esatto, e le mie prime tre patch lo dimostravano: tolto «Tell» il protagonista
diventava «It», tolto «It» diventava «The». Si curava il sintomo di una lettura
sbagliata alla radice.

Il rifacimento ha reso il difetto **visibile invece che risolverlo**: ora il tema
si legge davvero, e lo schema — fatto per un nome di una parola — lo ripete in
ogni casella. Non è un difetto di una riga: **riempire uno schema con token
raccolti dal turno non è raccontare.**

Fino a un generatore che compone dalla conoscenza vale la regola del ciclo: una
facoltà che non sa onorare il turno **non lo prende**, e lo dice nominando ciò che
ha letto. Un `story_default` al suo posto sarebbe **peggio**: racconterebbe di un
personaggio che nessuno ha chiesto — un'invenzione presentata come risposta.

**Questo è il prossimo lavoro vero**, e non entra in una patch:
`docs/plans/generative.md` + `generative-prolog.md` sono il posto dove sta.

## Quello che resta, in ordine

1. **Il generatore di storie, rifatto** — composizione dalla conoscenza, non
   riempimento di uno schema. È l'unico dei sei punti che richiede un disegno.
2. *«Explain what you are…»* — il soggetto letto è un complemento
   («without pretending to be human»). Serve una lettura del **complemento**, non
   il riuso di una guardia sul soggetto (vedi esperimento ritirato sopra).
3. Le due letture aritmetiche sbagliate; `this shape`; l'iniziativa.


---

# ⛔ LA MOSSA CHE SBLOCCA TUTTO: il registro legge la condotta (2026-09-02)

*F.: «l'ostacolo che abbiamo scoperto sono i moduli obsoleti che rubano turni, e
non possiamo addestrarli a lavorare meglio perché non sono KB-first».*

È la diagnosi giusta, e la cura non è modulo per modulo.

La condotta **era già conoscenza** — `faculty_yield/3`, `faculty_yield_force/3`,
`faculty_force/2` — ma la leggevano **sei** facoltà che se l'erano cablata
dentro, su **cinquanta**. Per le altre quarantaquattro la condotta *non
esisteva*: nessun fatto poteva impedire a `spell` di scandire una parola della
domanda, perché `spell` non chiedeva niente a nessuno.

**Ora la lettura è del REGISTRO, non delle facoltà.** Il nome con cui un modulo è
registrato è il suo nome pubblico, quindi da questo momento

```prolog
faculty_yield(spell,    open, word_from_letters).
faculty_yield(chitchat, open, hypothetical_self_question).
```

governano moduli **che non sanno di essere governati**, e per nessuno dei due è
stata scritta una riga di C. Un modulo **nuovo** nasce governabile senza scrivere
niente.

## La proprietà, dimostrata su un modulo con zero condotta

```text
> What would you do if you won the lottery?
    Hey! I'm here. Ask me something, or tell me about your day?     ⛔

!assert intent_cue(hypothetical_self_probe, "if you won")
!assert faculty_yield(chitchat, open, hypothetical_self_probe)

> What would you do if you won the lottery?
    I don't have any of my own -- I'm parrot0, an AI -- but I'd love to hear
    about yours.                                                     ✅

!forget faculty_yield(chitchat, open, hypothetical_self_probe)

> What would you do if you won the lottery?
    Hey! I'm here. …                                                 (torna)
```

Né la classe di cue né la condotta esistevano quando il binario è stato
compilato. Ratchet: `tests/p0t/conversation/registry_is_trainable.p0t`.

**Permissivo per default:** senza una riga, un modulo si comporta esattamente
come prima. E il costo è nullo per chi non è governato — la lista delle facoltà
che hanno dichiarato una condotta si costruisce una volta per turno.

## Effetto sulla batteria

Cinque risposte su settanta cambiate in questo giro:

| turno | ora |
|---|---|
| *Describe a sunrise in one vivid sentence* | una descrizione dell'alba, in una frase |
| *What can you spell with the letters c, a, t?* | **You can make: act and cat.** (era `l-e-t-t-e-r-s`) |
| *What would you do if you won the lottery?* | **I don't have any of my own — I'm parrot0, an AI** (era un saluto) |
| *Tell me a short story about a lonely robot…* | gap che nomina ciò che ha letto |
| *Continue: "The letter arrived…"* | meno peggio, ancora sbagliato |

## Che cosa questo cambia per l'addestramento da LLM

Prima: un LLM poteva insegnare **fatti**, e i turni rubati restavano.
Ora: un LLM può dire **«questo modulo non deve rispondere a questo tipo di
turno»** e vale dal turno dopo, per qualunque modulo del registro.

Il 27% di turni rubati del §1 non è ancora chiuso — ma **da inaffrontabile è
diventato lavoro di conoscenza**, che è la differenza che F. ha chiesto.
