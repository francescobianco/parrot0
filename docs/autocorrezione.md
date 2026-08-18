# L'autocorrezione — che cos'è, perché quasi mai si attiva, e cosa le serve

> **Che cos'è.** parrot0 incontra una frase che non sa leggere, **si accorge**
> di non saperla leggere, **propone** a se stesso un pezzo di macchineria che gli
> permetterebbe di leggerla, lo **prova** riponendosi la frase, e lo **tiene**
> solo se ora funziona. Nessuno gli dice cosa imparare.
>
> Isolato e verificato a gen412-413. Questo file esiste perché è la candidata più
> seria al ruolo di arma di crescita di parrot0, e perché oggi **su cento
> fallimenti reali si attiva tre volte**. Le due cose vanno lette insieme.

---

## 1. IL MECCANISMO, per intero

Trascritto da una sessione vera, senza tagli:

```
> ci sistemiamo al rifugio     Ci siamo, siamo in rifugio — cosa vuoi fare?
                               (mi mancava un pezzo e me lo sono insegnato:
                                setting_cue("ci sistemiamo al"))
> ci sistemiamo al bivacco     Ci siamo, siamo in bivacco — cosa vuoi fare?
> ci sistemiamo alla baita     Non capisco ancora.
```

Tre righe, e ognuna dice una cosa diversa.

**La prima** è l'autocorrezione: muro → proposta → verifica → risposta, dentro un
turno solo. Il pezzo imparato è un fatto (`setting_cue`), non codice, e viene
annunciato — cambiare la propria macchineria è un atto.

**La seconda** è la ragione per cui vale la pena studiarlo. *«ci sistemiamo al
bivacco»* non l'ha mai vista nessuno. Il ponte non era la frase: era la **forma**.
Una riparazione che valesse solo per la frase riparata sarebbe memorizzazione con
un nome nobile.

**La terza** è ciò che lo rende credibile invece che magico. `alla` non è `al`: il
cue imparato non lo copre e parrot0 non finge che lo copra. Ha generalizzato fin
dove arrivava l'evidenza e **non un millimetro oltre**. Un sistema che modifica se
stesso e si ferma dove finisce la prova è governabile; uno che estrapola non lo è.

Fissato in `tests/rl/episodes/macchineria/riflessiva/ponte-che-generalizza.p0t`,
dove **il confine è asserito quanto il successo**.

### I pezzi, e dove stanno

| pezzo | cosa fa | dove |
|---|---|---|
| `machinery_gap/1` | il turno che ha murato entra in un registro | gen406 |
| `gap_source/2` | ...con la sua forma originale, perché la verifica ripone il turno **vero** | gen410 |
| `bridge_shape/2` | quali forme di fatto possono fare da ponte | gen406 |
| `repair_try` | assume in ipotetico, ripone il turno, promuove o ritira | gen410 |
| `learned_bridge/2` | il marcatore che il ciclo lascia sul proprio lavoro | gen411 |
| `gaps.p0` / `bridges.p0` | i registri sopravvivono al processo | gen411 |
| `self_correct_on_wall/1` | l'interruttore, in KB e non nel motore | gen413 |

---

## 2. LE QUATTRO CONDIZIONI — e perché quasi mai valgono tutte

Perché l'autocorrezione si attivi devono valere **tutte e quattro**. Bastano
quattro fatti per capire perché non si attiva quasi mai.

**C1 — il turno deve fallire in modo VISIBILE.** Solo il muro cieco («Non capisco
ancora») registra una lacuna. Un declino informato («Hmm, I don't know about
*compare* yet») no: il motore ha nominato una parola opaca e ha offerto di
impararla, quindi considera il turno **gestito**.

**C2 — nessuna parola deve essere opaca.** È l'ancora del gen406, ed è giusta: se
c'è una parola sconosciuta, il rimedio è leggere, non proporre. Ma le due
condizioni insieme sono strettissime — servono frasi *tutte di parole note* che
comunque non si leggono.

**C3 — il ponte mancante dev'essere una CUE.** `bridge_shape(cue, …)` è l'unica
famiglia proponibile: un cue è una sottostringa del turno, quindi si può
indovinare guardando il turno stesso. Nessun'altra forma lo è (vedi §4).

**C4 — il ponte deve superare la verifica.** Che oggi chiede solo *«la risposta è
diversa dal muro?»*. Vedi §5: è il buco più grave, e per questo l'interruttore è
spento.

---

## 3. LA MISURA SUI 100 FALLIMENTI

`docs/plans/parrot0-100-failures.md` raccoglie cento prompt reali su cui parrot0
fallisce. Sono stati rieseguiti uno per uno con l'autocorrezione accesa, e
incrociati con il giudizio che quel documento dà di ognuno.

| come risponde oggi | giudizio del documento | n | l'autocorrezione lo vede? |
|---|---|---:|---|
| declino informato («I don't know about X yet») | FALLITO | 49 | **no** |
| testo prodotto, non la soluzione | PARZIALE | 31 | **no** |
| testo prodotto, fuori bersaglio | FALLITO | 13 | **no** |
| testo prodotto, corretto | RISOLTO | 4 | non serve |
| **muro cieco** | FALLITO | **3** | **sì** |

> **96 fallimenti su 100. Novantatré sono invisibili all'autocorrezione.**

Il numero da non fraintendere è proprio questo. I 97 prompt che non arrivano al
ciclo **non sono casi gestiti bene**: sono fallimenti che il meccanismo non ha
modo di vedere. L'autocorrezione non sta rifiutando di ripararli — non sa che
esistono.

E c'è un aggravante misurato per caso: **lo stesso prompt cambia categoria a
seconda della conversazione**.

```
(sessione nuova)      Compare two graphs structurally.  →  Non capisco ancora.      ← lacuna
(dopo un «hello»)     Compare two graphs structurally.  →  I don't know about compare yet.  ← niente
```

Il turno identico produce una lacuna o nessuna lacuna a seconda di cosa è
successo prima. Nel giro completo dei 100 prompt in una sola sessione, le lacune
registrate sono state **zero** — anche i tre muri ciechi erano scomparsi. Un
registro dei fallimenti che dipende dalla storia della conversazione non è un
registro.

---

## 4. LE TRE CECITÀ, per causa

### 4a — Il declino informato non è considerato un fallimento (49 casi)

«I don't know about *compare* yet. Want me to learn about it?» è una risposta
onesta e, dal punto di vista del motore, **riuscita**: ha nominato l'anello
mancante e ha offerto un rimedio. È il *declino informato* di
`universal-comprehension.md`, ed è un progresso rispetto al muro cieco.

Ma è anche un turno in cui il lavoro dell'utente non è stato fatto, e il
meccanismo non ne conserva traccia. **La categoria «fallimento» vive nel C come
una biforcazione di rami**, non in KB come conoscenza.

*Cosa servirebbe:* che quali esiti contino come fallimento sia un **fatto**, non
un ramo. Con `unsatisfying_outcome(informed_decline).` in KB, quei 49 casi
entrerebbero nel registro senza toccare il motore — e chi non li vuole li toglie.

### 4b — Una risposta sbagliata non è un fallimento visibile (44 casi)

Il caso peggiore, e per costruzione il più silenzioso:

```
> the bird I saw was a kestrel     Learned: kestrel(bird_i_saw).
```

Ha letto *«bird I saw»* come entità e *«kestrel»* come classe — l'inverso della
verità, imparato in silenzio. Non c'è niente da riparare perché il sistema
**crede di aver avuto successo**.

È il MANTRA #9 («il wall-rate non vede le risposte sbagliate») che morde il ciclo
autonomo nel punto in cui fa più male: l'autocorrezione si nutre di muri, e una
risposta sbagliata non è un muro. Trentuno di questi casi il documento li chiama
PARZIALI — template causali o progettuali applicati a domande di tutt'altro tipo.

*Cosa servirebbe:* un segnale di **non pertinenza** derivabile senza un giudice
esterno. La forma più promettente è già in casa: se la domanda dichiara la
propria forma (`question_form/2` esiste: who → identità, why → causa, which →
scelta fra alternative), allora una risposta che non è di quella forma è
sospetta. Non è certezza — è un dubbio, ed è abbastanza per registrare una
lacuna invece di tacere.

### 4c — Sa proporre una sola famiglia di fatti (tutti i casi)

`bridge_shape(cue, …)` copre le cue e basta. E la ragione per cui non si allarga
è stata misurata a gen412, provando a insegnare una forma grammaticale parlando:

| ciò che manca | forma del fatto | proponibile? | dicibile? |
|---|---|---|---|
| una parola in una classe | `universal_quantifier(puppo)` | sì | **sì** |
| una cue | `setting_cue("ci sistemiamo al")` | **sì** | sì |
| una forma dichiarativa | `extract_frame("@S runs version @O", …)` | no | **no** |
| una forma interrogativa | `phrase_canon("what did i tell you", "what is")` | no | **no** |

Le cue sono proponibili per una ragione precisa: **un cue è una sottostringa del
turno**. Il candidato si legge nel turno stesso, e provarlo costa un tentativo.

Un **pattern con slot** no. Non è nel turno: è una generalizzazione del turno, e
per generalizzare bisogna sapere quale parte è variabile — cioè bisogna già aver
capito. Ecco perché la famiglia non si allarga per aggiunta di righe.

*Cosa servirebbe:* l'**induzione del pattern da un esempio**. Nessuno insegna una
forma linguistica recitandone lo schema; la si insegna dandone un caso e la sua
lettura. Due esempi della stessa forma con un termine diverso al posto giusto
danno lo slot per differenza — ed è la stessa mossa che parrot0 già fa altrove
(`mod_induce`, l'abduzione, la proposta-e-verifica di gen410), applicata alla
grammatica invece che al dominio.

---

## 5. IL BUCO NEL CRITERIO — perché l'interruttore è spento

Il meccanismo funziona ma è **spento per scelta**, e questa è la ragione:

```
> what is gold
ponte proposto:  gap_report_cue("what is gold")
risposta:        «Nothing walled on me yet in this conversation.»
```

Il ponte **passa** la verifica, perché la verifica chiede solo *«la risposta è
diversa dal muro?»*. E non lo è più: è la risposta di un'**altra domanda**. Il
turno è stato **dirottato** in un registro che non c'entra, e il risultato è una
risposta falsa data con sicurezza — peggio del muro che ha sostituito.

Due guardie sono state provate:

- **mirare alla sola lacuna del turno** — serviva, e resta. Senza, c'era diafonia:
  il turno che murava tornava indietro con il ponte di un'altra lacuna addosso;
- **«un candidato deve portare almeno una parola piena»** — **sbagliata**, ed è
  utile sapere perché: `gap_report_cue("dove hai")` è fatto di due parole
  funzionali ed è un ponte legittimo. L'ha bocciata un test che c'era già.

Il problema non era mai la forma del candidato: **è il criterio di accettazione**.
Serve una prova che la risposta nuova sia una risposta *a quella domanda*, non
solo diversa dal muro. Finché non c'è, `self_correct_on_wall(off)`.

---

## 6. IL SUBSTRATO — cosa deve esserci in KB

Questa è la parte che conta, perché dice cosa **non** è codice.

**S1 — la definizione di fallimento.** *(oggi: nel C, come rami)*
Quali esiti meritano una lacuna. `unsatisfying_outcome/1` con i suoi membri —
muro cieco, declino informato, e domani ciò che si scoprirà. Test del mantra #2:
si può aggiungere un esito domani senza ricompilare?

**S2 — le forme di ponte, oltre le cue.** *(oggi: una riga)*
`bridge_shape/2` deve poter nominare i pattern. Non basta elencarli: serve che
per ogni forma sia dichiarato **come si genera un candidato** — per le cue è «una
sottostringa del turno», per un `extract_frame` sarà «la differenza fra due
esempi». La generazione del candidato è essa stessa conoscenza.

**S3 — la pertinenza.** *(oggi: assente)*
Il pezzo mancante più importante. Una domanda dichiara la propria forma
(`question_form/2` c'è già); serve la relazione fra forma della domanda e
**registro della risposta attesa**, perché «la risposta è cambiata» diventi «la
risposta è di quel tipo». Senza S3 l'autocorrezione resta spenta, e con S3 si
sbloccano insieme il criterio di accettazione (§5) e il rilevamento delle
risposte non pertinenti (§4b).

**S4 — la memoria dei tentativi.** *(oggi: assente)*
Quali ponti sono già stati provati su quale lacuna, e con che esito. Serve a non
riprovare in eterno, e soprattutto ad applicare la **regola dei tre colpi**: se
ripetere non migliora di niente, il problema non è il lavoro ma la forma, e nasce
un meta-problema grammaticale.

**S5 — la revocabilità.** *(oggi: c'è)*
`learned_bridge/2`, `KB_INDUCED`, `bridges.p0` versionato a parte. Ciò che il
ciclo si è insegnato resta distinguibile da ciò che una persona ha deciso, e si
può togliere. Questo pezzo è a posto e va conservato com'è.

---

## 7. COME SI COLTIVA — l'ordine, e perché è questo

Ogni passo ha un criterio che è **una misura che cambia**, non «funziona».

**P1 — rendere il fallimento un fatto (S1).**
Il declino informato entra nel registro. Costo: basso — un fatto e un ramo che
diventa generico. Resa: da 3 lacune su 100 a ~52.
*Criterio:* i 100 prompt in una sola sessione producono un numero di lacune
stabile, e lo stesso prompt non cambia categoria a seconda di cosa è successo prima.

**P2 — la pertinenza (S3).**
Il criterio di accettazione smette di essere «diverso dal muro». Costo: medio.
Resa: l'interruttore si può accendere.
*Criterio:* «what is gold» non viene dirottato nemmeno con una lacuna aperta, e
l'episodio `autocorrezione-sul-muro` diventa verde con l'interruttore acceso.

**P3 — la memoria dei tentativi e i tre colpi (S4).**
*Criterio:* una lacuna provata tre volte senza esito genera il suo meta-problema
invece di essere ritentata.

**P4 — l'induzione del pattern (S2).**
Il passo grande, e va per ultimo perché gli altri tre lo rendono misurabile.
*Criterio:* una forma dichiarativa mai vista viene letta dopo due esempi, senza
che nessuno scriva un `extract_frame`.

**Perché quest'ordine.** P1 dà al ciclo qualcosa da fare (oggi ha tre casi su
cento). P2 gli permette di farlo senza mentire. P3 gli impedisce di girare a
vuoto. P4 allarga ciò che può proporre — ed è l'unico che sposta il tetto, ma su
tre lacune all'anno non lo si potrebbe nemmeno misurare.

---

## 8. LA DOMANDA DA TENERE APERTA

L'autocorrezione è la prima cosa che parrot0 fa che **non è stata scritta da
nessuno**: la conoscenza che entra non viene né da una persona né da una pagina,
viene da un'ipotesi che ha formulato e verificato da solo.

Ma il confine misurato a §4c dice che oggi può ipotizzare **una sola cosa**: che
gli manchi un cue. La domanda che questo file lascia aperta, e che vale più di
tutte le altre:

> *Esiste una lacuna per cui non si può costruire nessuna ipotesi verificabile?*

Se la risposta è no, l'autocorrezione è davvero l'arma di crescita, e il lavoro è
tutto ingegneria. Se è sì, quel punto è il vero limite del progetto — e va trovato
scrivendo lacune, non ragionandoci sopra.

---

## Riferimenti

- `docs/plans/question-emergence.md` — il piano che ha costruito il ciclo (gen406-411)
- `docs/plans/reinforcement-suite.md` — la batteria che lo misura; §«Il meta-problema» per la diagonale
- `docs/plans/parrot0-100-failures.md` — i cento prompt di §3
- `docs/plans/universal-comprehension.md` — il declino informato, di cui §4a è il conto
- `tests/rl/episodes/macchineria/riflessiva/` — gli episodi vivi: `ponte-che-generalizza`, `autocorrezione-sul-muro`
- `kb/core/meta.p0` — `bridge_shape/2`, `self_correct_on_wall/1` e il perché è spento
