# L'autocorrezione — che cos'è, perché quasi mai si attiva, e cosa le serve

> **Che cos'è.** parrot0 incontra una frase che non sa leggere, **si accorge**
> di non saperla leggere, **propone** a se stesso un pezzo di macchineria che gli
> permetterebbe di leggerla, lo **prova** riponendosi la frase, e lo **tiene**
> solo se ora funziona. Nessuno gli dice cosa imparare.
>
> Isolato e verificato a gen412-413. Questo file esiste perché è la candidata più
> seria al ruolo di arma di crescita di parrot0, e perché oggi **su ottantotto
> fallimenti reali si attiva tre volte**. Le due cose vanno lette insieme.

---

## 0. LA TESI — l'autocorrezione non è un rimedio, è una forma di inferenza

La formulazione che segue è di F., e riordina tutto il resto del documento:

> **L'autocorrezione non è un processo postumo. È preliminare e operativo, e
> abbraccia l'inferenza stessa.**

Detta così sembra una sfumatura. Non lo è, ed è la differenza fra ciò che il
meccanismo è oggi e ciò che può essere.

### ⛔ Il modo di sbagliarla che si e' presentato davvero (F., gen435)

Fra i due modi qui sotto ne esiste un terzo, che non e' un'interpretazione ma un
**travisamento**, ed e' capitato: leggere «trovare le lacune» come *scansionare
la KB a freddo con un tool dedicato e colmarle*.

> **Una lacuna non e' un'assenza nella KB: e' un ARRESTO nell'inferenza.**

Le assenze sono illimitate, non hanno un rimedio determinato e non hanno un
oracolo; un arresto e' uno per turno, nomina il pezzo che manca e si verifica
riponendo il turno. Un tool che colma dall'esterno lascia per giunta la KB **nello
stato di prima**: la volta dopo serve ancora il tool — e quella non e'
autocorrezione, e' manutenzione. Il bersaglio e' una KB **autocorrettiva per
stato**, cioe' una configurazione in cui il turno che fallisce produce da se' la
propria riparazione.

Le tre domande per non ricascarci, da fare a ogni proposta: *gira dentro un turno
o accanto? quella lacuna esisterebbe se nessuno avesse parlato? produce un elenco
o una mossa?* Il perimetro per esteso sta in
[`plans/autocrescita.md`](plans/autocrescita.md) §0a.

### I due modi di intenderla

**Postumo** — come funziona adesso. Il turno fallisce → si emette un muro → si
registra una lacuna → *più tardi*, qualcuno tenta una riparazione. La correzione
è una **seconda passata su un fallimento**, e per farla bisogna prima aver
fallito in un modo riconoscibile. Da qui tutti i numeri di §3: su ottantotto
fallimenti reali, tre sono riconoscibili.

**Preliminare** — l'idea. Ogni atto di inferenza è già un **tentativo di
composizione che sa dire dove si è fermato**. Non c'è un momento in cui il turno
"fallisce" e un momento successivo in cui si ripara: c'è un solo movimento —
prova a comporre — e il punto in cui la composizione si arresta è
*contemporaneamente* la risposta («sono arrivato fin qui») e il bersaglio della
riparazione («mi manca questo, in questa posizione»).

### Perché oggi non può esserlo: l'inferenza è booleana

È qui che le due visioni si separano, e la causa è misurabile.

I lettori di parrot0 restituiscono `1` o `0`. `p0_rule_clause` legge una clausola
o non la legge; `nw == 3 && w[1] == "is"` combacia o non combacia. **Un fallimento
non porta nessuna informazione**: non dice che cosa mancava, né dove.

E siccome il fallimento è muto, chi deve ripararlo non ha niente in mano — e
tira a indovinare. Le due manifestazioni di questo, entrambe misurate:

- **il messaggio**: non sapendo che cosa dire, il ripiego nomina la prima parola
  di sei lettere o più su cui la KB non ha fatti. Su «If it rains then the ground
  is wet…» esce «ground» — la parola meno rilevante della frase (§3b);
- **la riparazione**: non sapendo che cosa proporre, il ciclo prova sottostringhe
  del turno. Funziona per le cue, perché una cue *è* una sottostringa; per tutto
  il resto no (§4c).

**Un'inferenza che riporta dove si è fermata rende la riparazione gratuita.** Non
serve indovinare il pezzo mancante se il lettore lo ha appena nominato. È questo
che intende «preliminare»: la correzione non viene dopo l'inferenza, **è una
proprietà di come l'inferenza è scritta**.

### Il framework

Da cui la definizione operativa, che è il vero contenuto di questo file:

> **L'autocorrezione è un metodo, non un modulo.** Dato un prompt che fallisce:
> lo si studia fino a trovare che cosa manca davvero; si stabilisce che quel
> qualcosa sia **conoscenza** e non codice; si scrive il meccanismo generico che
> lo consuma; e da quel momento il sistema chiude da solo tutta la classe.

Le tre proprietà che un meccanismo deve avere per entrare in questo framework:

1. **KB-first** — ciò che manca è un fatto, e domani se ne aggiunge un altro senza
   ricompilare. Se il rimedio è una riga di C, la classe non è stata chiusa: è
   stato chiuso un caso.
2. **Parziale e loquace** — il meccanismo deve poter riuscire *a metà* e dirlo.
   Un lettore che restituisce solo sì/no non partecipa all'autocorrezione,
   qualunque cosa sappia leggere.
3. **Il candidato si legge nel turno** — la riparazione dev'essere proponibile.
   Una cue lo è perché è una sottostringa; **una posizione mancante in uno schema
   lo è per la stessa ragione**. Un pattern con slot no, e questa è la diagonale
   di §4c.

Il resto del documento è la verifica di questa tesi contro i numeri: §1-§3 cosa
il meccanismo fa e quanto poco arriva, §3b-§4 perché, §5-§6 cosa gli manca, e
§9-§10 le due idee che lo renderebbero preliminare invece che postumo.

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

| # | come il turno finisce oggi | n | prompt rappresentativo | risposta | lacuna? |
|---|---|---:|---|---|---|
| **A** | **muro cieco** | **3** | *Compare two graphs structurally.* | «Non capisco ancora.» | **sì** |
| **B** | declino su parola opaca | **49** | *If it rains then the ground is wet. The ground is wet. Did it necessarily rain?* | «Hmm, I don't know about **ground** yet. Want me to learn about it?» | no |
| **C** | declino su schema assente | **5** | *Write a decision record with alternatives and rejected options.* | «I understood the request … but I don't have a verified schema for that artifact yet.» | no |
| **D** | template fuori bersaglio | **31** | *Explain a counterexample to every swan is white.* | «On a counterexample to every swan is white, **a causal account turns on the condition that starts**…» | no |
| **E1** | rifiuto corretto per limite reale | 7 | *What is the latest price of Bitcoin?* | «I have no live source for a market price: it changes while we speak…» | — |
| **E2** | risposta corretta | 4 | *What is the remainder of 29 divided by 5?* | «4.» | — |
| **E3** | risposta secca non motivata | 1 | *If all doctors are scientists and some scientists are artists, are all doctors artists?* | «No.» | no |
| | **totale** | **100** | | | |

> **88 fallimenti reali, e 85 sono invisibili all'autocorrezione.**
> Solo la classe A registra una lacuna. B, C, D ed E3 non lasciano niente.

Due precisazioni che il conto impone, ed entrambe correggono qualcosa.

**E1 non sono fallimenti.** Sette dei prompt che `parrot0-100-failures.md`
classifica FALLITO sono rifiuti corretti — nessuna fonte viva per un prezzo che
si muove, nessuna consulenza legale, il numero di emergenza davanti a un dolore
toracico. Il disclaimer di quel documento dice esattamente questo («non
considero fallimento il rifiuto prudente»), quindi il suo stesso indice va
corretto: i fallimenti sono **88**, non 96.

**D è la classe peggiore, e non lo sembra.** Trentuno turni producono un
paragrafo plausibile che non risponde alla domanda — «a causal account turns
on…» applicato a un controesempio logico. Chi legge in fretta vede un successo, e
il motore *crede* di aver risposto. Sono più numerosi dei muri di dieci volte e
non lasciano nessuna traccia.

Il numero da non fraintendere è proprio questo. Gli 85 prompt che non arrivano al
ciclo **non sono casi gestiti bene**: sono fallimenti che il meccanismo non ha
modo di vedere. L'autocorrezione non sta rifiutando di ripararli — non sa che
esistono.

E la distribuzione dice anche in che ordine conviene lavorare, perché le classi
non costano uguale: **B è la più numerosa e la più economica** (il turno ha già
individuato la parola che manca: diventa una lacuna con un fatto), mentre **D è
la più insidiosa** (bisogna prima saper dire che una risposta non è pertinente —
il substrato S3 di §6).

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

## 3b. ANATOMIA DI UN CASO DELLA CLASSE B — tre strati, non uno

F.: *«questa domanda deve pure essere risolvibile con l'autocorrezione»*. Il
rappresentante della classe B è stato smontato pezzo per pezzo, e sotto c'erano
**tre blocchi in fila**, nessuno dei quali è quello che il messaggio d'errore
nomina.

```
If it rains then the ground is wet. The ground is wet. Did it necessarily rain?
                                  →  «Hmm, I don't know about ground yet.»
```

**«ground» non c'entra niente.** È solo la prima parola che il motore non
riconosce, e viene nominata perché *nessuna* delle tre frasi era leggibile. Il
lessico è il ripiego, non la causa — ed è la ragione per cui i 49 prompt della
classe B sembrano tutti problemi di vocabolario e nessuno lo è.

| strato | cosa bloccava | stato |
|---|---|---|
| 1 | **il condizionale fra proposizioni.** `if someone is a doctor then they are a scientist` si legge da sempre (due appartenenze); `if it rains then the ground is wet` no — due proposizioni intere, la forma in cui la logica si dice quasi sempre | **rimosso** (gen413) |
| 2 | **l'articolo sul soggetto.** `ground is wet` entrava, `the ground is wet` no. Una parola in più, e le classi di articoli erano già dichiarate in KB | **rimosso** (gen413) |
| 3 | **tre frasi in un turno solo.** Il prompt è un periodo con tre atti: regola, fatto, domanda. Il lettore lavora sul turno intero | aperto |

Rimossi i primi due, la catena funziona — a patto di dare le tre frasi come tre
turni:

```
> if it rains then the ground is wet   Learned rule: holds(ground_is_wet) :- holds(it_rains).
> the ground is wet                    Learned: holds(ground_is_wet).
> is the ground wet                    Yes.
```

Lo stesso contenuto in un turno solo continua a fallire. **Sui 100 prompt il
guadagno misurato è quindi zero**, ed è un risultato onesto da tenere: due
blocchi reali rimossi non spostano il numero finché il terzo regge.

### Il dizionario che si scrive da solo

La proposta di F. era di mettere in KB un dizionario per autoalimentare la
correzione. La forma che ha preso è più stretta e più sicura: **il dizionario lo
costruisce il lettore di regole mentre legge**. Ogni atomo nato da un
condizionale viene registrato (`proposition_seen/1`), e da quel momento — e solo
da quel momento — la stessa proposizione detta da sola è asseribile.

È lo stesso cancello del gen133 («si legge in quel modo solo ciò di cui una
regola già parla»), spostato dalle classi alle proposizioni, e serve a impedire
l'effetto opposto: senza, *qualunque* frase diventerebbe una proposizione opaca e
il motore smetterebbe di capire quelle che capiva. Il costo di sbagliare qui è
stato misurato subito — la prima versione si mangiava i piani condizionali
(«…altrimenti…»), e la guardia è anch'essa conoscenza: un ramo alternativo vuol
dire piano, non implicazione.

### Che cosa dice questo all'autocorrezione

Lo strato 3 è il posto giusto per il ciclo, e la ragione è la forma del rimedio:
**dove finisce una frase è una regola generale, non un fatto sul mondo**, e un
candidato si può leggere nel turno stesso — esattamente come una cue. È il primo
caso in cui una lacuna della classe B ha una riparazione della forma che il ciclo
sa già proporre.

---

## 3c. I QUARANTANOVE NON SONO UNA CLASSE

La classe B si chiama «declino su parola opaca» perché è così che *finisce*, non
perché i prompt si assomiglino. Classificati per forma:

| forma | n | esempio |
|---|---:|---|
| altro | 20 | *Forget my name.* |
| domanda-wh | 14 | *What information is missing before comparing two cities?* |
| imperativo / compito | 7 | *Translate the dog runs into Spanish.* |
| numeri / tempo | 4 | *A train leaves at 14:30 and travels for 2h45. When does it arrive?* |
| logica | 4 | *If it rains then the ground is wet. …* |

**Cinque problemi diversi con la stessa etichetta appiccicata sopra.** L'unico
tratto comune è il messaggio, che è un artefatto del ripiego — e la parola
nominata non è mai l'argomento: `travels`, `ground`, `missing`, `matters`.

Ne segue una cosa scomoda e utile: **non esiste un rimedio unico per la classe
B**, e cercarlo è tempo perso. Esistono cinque letture mancanti, ognuna con il
suo meccanismo — che è esattamente ciò che il framework di §0 prescrive
(«processi KB-first dedicati»). Il lavoro fatto a gen413 ne ha chiusa una parte
di una: il condizionale proposizionale, quattro prompt su quarantanove.

---

## 4. LE TRE CECITÀ, per causa

### 4a — Il declino informato non è considerato un fallimento (classi B e C, 54 casi)

«I don't know about *compare* yet. Want me to learn about it?» è una risposta
onesta e, dal punto di vista del motore, **riuscita**: ha nominato l'anello
mancante e ha offerto un rimedio. È il *declino informato* di
`universal-comprehension.md`, ed è un progresso rispetto al muro cieco.

Ma è anche un turno in cui il lavoro dell'utente non è stato fatto, e il
meccanismo non ne conserva traccia. **La categoria «fallimento» vive nel C come
una biforcazione di rami**, non in KB come conoscenza.

*Cosa servirebbe:* che quali esiti contino come fallimento sia un **fatto**, non
un ramo. Con `unsatisfying_outcome(informed_decline).` in KB, quei 54 casi
entrerebbero nel registro senza toccare il motore — e chi non li vuole li toglie.
E' il passo piu' economico di tutti: il turno ha gia' individuato la parola che
manca, quindi la lacuna nasce gia' con la sua ancora.

### 4b — Una risposta non pertinente non è un fallimento visibile (classi D ed E3, 32 casi)

Il caso peggiore, e per costruzione il più silenzioso:

```
> the bird I saw was a kestrel     Learned: kestrel(bird_i_saw).
```

Ha letto *«bird I saw»* come entità e *«kestrel»* come classe — l'inverso della
verità, imparato in silenzio. Non c'è niente da riparare perché il sistema
**crede di aver avuto successo**.

È il MANTRA #9 («il wall-rate non vede le risposte sbagliate») che morde il ciclo
autonomo nel punto in cui fa più male: l'autocorrezione si nutre di muri, e una
risposta sbagliata non è un muro. Trentuno di questi sono la classe D — template causali o progettuali applicati
a domande di tutt'altro tipo, dieci volte piu' numerosi dei muri.

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

**S4b — gli schemi come sequenze di RUOLI.** *(oggi: i ruoli ci sono, gli schemi no)*
`np_opener`, `np_closer`, `generic_copula`, `preposition`, `logic_connector` sono
già fatti; manca la **composizione**, cioè uno schema che li metta in sequenza e
che, fallendo, dica quale posizione è vuota. È il substrato che rende l'inferenza
loquace (§0) e quindi la riparazione proponibile. Vedi §9.

**S4c — gli indizi di registro.** *(oggi: solo per il codice)*
Quali segni dicono «questo turno è di questo tipo» anche senza combaciare con uno
schema. Per il codice sono in C e funzionano; per la logica sono in KB e nessuno
li legge. Vedi §10.

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

**P0 — l'inferenza che riporta dove si è fermata (S4b, §9).**
Aggiunto dopo la riformulazione di F.: viene **prima di tutto il resto**, perché
è ciò che rende le altre fasi possibili invece che faticose. Con un lettore
composizionale, P1 e P2 quasi si scrivono da soli — la lacuna nasce già nominata
e il criterio di accettazione ha qualcosa da confrontare.
*Criterio:* sui 49 prompt della classe B, quanti passano da «parola opaca» a
«forma riconosciuta, pezzo mancante nominato».

**Perché quest'ordine.** P0 rende l'inferenza loquace, e senza quello ogni altra
fase deve indovinare. P1 dà al ciclo qualcosa da fare (oggi ha tre casi su
cento). P2 gli permette di farlo senza mentire. P3 gli impedisce di girare a
vuoto. P4 allarga ciò che può proporre — ed è l'unico che sposta il tetto, ma su
tre lacune all'anno non lo si potrebbe nemmeno misurare.

**Una nota sull'ordine, che vale come avvertimento.** Prima della riformulazione
questo elenco cominciava da P1 — «rendere il fallimento un fatto». Era l'ordine
giusto per un'autocorrezione *postuma*: prima raccogli i fallimenti, poi li
ripari. Visto come processo **preliminare**, il primo passo è un altro, e i
quattro che seguono cambiano di costo. È la differenza pratica che la tesi di §0
produce, ed è il motivo per cui vale la pena averla scritta.

---

## 9. GLI SCHEMI COMPOSIZIONALI — l'idea che rende l'inferenza loquace

*(F., gen413: «tutte queste categorie di prompt possono essere autoriparate con
processi KB-first dedicati — per esempio la costruzione compositiva degli schemi
delle proposizioni».)*

È la forma concreta della tesi di §0, e i pezzi esistono già: `np_opener`,
`np_closer`, `generic_copula`, `preposition`, `logic_connector` sono fatti in
`grammar.p0`. Il C li usa in sedici punti sparsi — **come filtri, mai per
costruire**.

La differenza è tutta qui:

```
oggi        nw == 3 && w[1] == "is"          →  combacia, o niente
composito   [soggetto][copula][predicato]    →  «soggetto e predicato trovati,
                                                 manca la copula in posizione 2»
```

Il primo, fallendo, non ha niente da dire: e infatti finisce a nominare `ground`.
Il secondo, fallendo, **sa quale pezzo manca e dove** — che è esattamente la
proprietà 2 del framework, e produce una lacuna proponibile (proprietà 3): una
posizione mancante in uno schema si legge nel turno come una sottostringa.

**Uno schema è una sequenza di RUOLI, non un conteggio di token.** È anche la
correzione della fragilità che ha bloccato la classe B per due strati su tre:
`ground is wet` entrava e `the ground is wet` no, perché `nw == 3` diventava
`nw == 4`. Uno schema di ruoli non ha quel problema — l'articolo è parte del
sintagma nominale, non una parola in più.

**Cosa costa, onestamente.** Non è come spostare un indice. È un lettore nuovo,
e va montato senza disturbare i sessantotto `mod_*` che leggono a conteggio di
token. Il posto sicuro è **come ultimo tentativo prima del ripiego**, dove oggi
c'è `not_understood` e non c'è niente da perdere.

**E non basta da solo per due delle cinque forme** (§3c): per «numeri/tempo» e
per gli imperativi il pezzo mancante non è sintattico ma una **procedura**. Lì lo
schema dirà correttamente *«ho riconosciuto la forma, non so eseguirla»* — che è
una lacuna di tipo diverso, riparabile insegnando la procedura invece del ponte,
e comunque incomparabilmente meglio di una parola a caso.

---

## 10. IL CLASSIFICATORE DI REGISTRO — un'asimmetria che costa quarantanove prompt

*(F., gen413: «siamo in grado di riconoscere il codice dentro la prosa e non
siamo in grado di classificare un prompt come problema proposizionale anche
quando non c'è il match con lo schema».)*

L'osservazione è esatta, e il codice la conferma. `looks_code`
(`src/brain/70-social-pragma.c:846`) riconosce il codice **per indizi
strutturali**, senza nessuno schema da far combaciare: `{`, `}`, `;`, `==`, un
`(` preceduto da un identificatore, una keyword seguita da `:`. Ha perfino una
finezza — un `(` con lo spazio davanti è una parentetica in prosa, non una
chiamata.

Ed è un **classificatore di registro**: dice *«questo è di quel tipo»* senza
dover capire il contenuto, e quando riconosce senza saper eseguire **dice quale
registro è**.

Per la logica non esiste niente del genere, benché gli indizi siano altrettanto
robusti — `if…then`, `necessarily`, `all…are`, `contradictory`, `contrapositive`
— e **siano già in KB** (`logic_connector/2`). Ma sono usati solo *dentro* il
lettore di regole, che o fa combaciare lo schema o restituisce 0.

È la stessa asimmetria del messaggio finale: il registro `codice`, riconosciuto e
non eseguibile, si annuncia; il registro `logica`, non riconosciuto affatto,
finisce nel ripiego che nomina una parola a caso.

Per l'autocorrezione il guadagno è diretto: un classificatore di registro
trasforma *«non conosco ground»* in *«questo è un problema di logica
proposizionale che non so ancora risolvere»* — cioè una lacuna **nominabile**,
quindi riparabile. Venti righe di indizi, e per la logica sono già dichiarati.

---

## 11. IL MESSAGGIO FINALE — dire che non si è capito

*(F., gen413: «quando qualcosa sfugge a tutti i moduli deve dare come risposta
"non sono riuscito a comprendere"».)*

Nominare una parola è una **diagnosi falsa spacciata per informazione**. Il
turno non ha fallito su `ground`: ha fallito interamente, e `ground` è solo il
primo token di sei lettere senza fatti in KB (§3b). Chi legge — utente o ciclo di
riparazione — viene mandato a inseguire il lessico mentre il problema è la forma.

Finché il classificatore di registro (§10) non esiste, la risposta onesta è
ammettere di non aver compreso. La nomina della parola va tenuta **solo** per il
caso in cui è vera: quando il turno è stato riconosciuto e l'unica cosa che manca
è un fatto su quel termine.

**Il freno è misurato:** quarantadue file `.p0t` asseriscono quella frase. Non è
una riga da cambiare, è un lavoro suo — ma va fatto, perché ogni giorno che resta
produce diagnosi false che finiscono nei documenti e nelle decisioni. Questo
stesso file ci è cascato: la classe B si chiamava «parola opaca» finché non l'ho
smontata.

---

## 12. LA DOMANDA DA TENERE APERTA

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

## 13. L'AUTOCORREZIONE FATTA IN DUE — chiedere invece di indovinare

*(F., gen430: «studiati tutti i prompt di `docs/issues/` e capisci come il
processo di insegnamento si possa automatizzare dal punto di vista dialogico».)*

### 13.1 Il cambio di ipotesi

Tutto quello che precede descrive un'autocorrezione **solitaria**: parrot0 deve
accorgersi di aver fallito, **inventare** il pezzo mancante, provarlo. Il passo
che la uccide è il secondo — sa proporre solo cue (C3), e quindi arriva a tre
casi su ottantotto.

Ma c'è un fatto che questo documento, scritto a gen413, non poteva usare: **dal
gen427-429 una forma si insegna parlando, e ha effetto nel turno successivo.** La
notazione di un letterale, un verbo di relazione, un marcatore di segmento, una
relazione nuova: tutti si installano dicendo una frase.

Il che cambia la domanda. Non più *«come faccio a indovinare il pezzo che mi
manca?»*, ma:

> **«come faccio a CHIEDERLO, in una forma che la risposta dell'altro installi da
> sola?»**

L'interlocutore è già nella stanza. Il pezzo mancante non va inventato — va
**nominato**, e la frase che lo insegna va **consegnata già scritta**.

### 13.2 Le quattro condizioni, riscritte

Le condizioni di §2 sono strettissime perché descrivono un'invenzione. Chiedere
ne ha altre, e sono molto più larghe:

| | solitaria (§2) | dialogica |
|---|---|---|
| **C1** | il turno deve fallire in modo **visibile** (solo il muro cieco) | basta **fermarsi**: anche un declino informato può chiedere. I 49 della classe B tornano dentro |
| **C2** | **nessuna** parola dev'essere opaca | una parola opaca **è la domanda migliore**: *«che cos'è "travels"?»* è esattamente ciò che si sa chiedere |
| **C3** | il ponte dev'essere una **cue** (una sottostringa, quindi indovinabile) | il ponte dev'essere **dicibile**: basta che esista una frase che lo installa |
| **C4** | verifica: *«la risposta è diversa dal muro?»* | verifica: **si ripone il turno**. Se ora si legge, il pezzo era quello — e non serve S3 per dirlo |

C3 è il cardine. La condizione «indovinabile» è una proprietà della *forma del
ponte*; la condizione «dicibile» è una proprietà della **KB**, e quindi si
allarga con una riga. È il passaggio da un limite di principio a un inventario.

### 13.3 Il seme, che gira (gen430)

Il muro non chiede più *«vuoi che lo impari?»*, che è una domanda retorica:
consegna la frase.

```
> zorak governs nivora
< Hmm, I don't know about governs yet. Want me to learn about it? Or teach me:
  if governs is something one thing does to another, say «governs is a relation
  verb»; if it is a kind of thing, say «something is a governs».
> governs is a relation verb
< Learned: relation_verb(governs).
> zorak governs nivora
< Learned: governs(zorak, nivora).
> who governs nivora?
< Zorak.
```

Quattro turni, e nessuno dei due sapeva in anticipo che cosa andasse insegnato.
Le forme offerte sono fatti (`word_teaching_offer/2`): una forma nuova si affaccia
da sola nel messaggio, e quante offrirne è un fatto anche quello.

Da notare, perché è il punto di tutta la sezione: **l'ultimo turno non era stato
insegnato.** Insegnando il verbo si è aperta insieme la porta di lettura e quella
di risposta (`answer_frame($V,$V) :- relation_verb($V)`), che è la metà che
manca sempre — il «buco del consumatore» del gen306.

### 13.4 I nove prompt di `docs/issues/`, riletti come richieste di insegnamento

La domanda operativa, per ognuno: **esiste una frase che l'interlocutore potrebbe
dire, e che chiuderebbe il caso?** Se sì, il caso è automatizzabile in dialogo —
e ciò che manca è solo la frase con cui chiederla.

| # | forma | la frase che lo chiuderebbe | oggi |
|---|---|---|---|
| **01** orario e durata | numeri/tempo | *«"leaves at" is a departure marker»*, *«"travels for" is a duration marker»* | **dicibile** — i marcatori si insegnano (gen429), manca chi li chieda |
| **02** contrapposizione | logica | *«the contrapositive of "if A then B" is "if not B then not A"»* | **non dicibile** — è una procedura su una forma, serve P4 (induzione del pattern) |
| **03** traduzione | superficie | *«"translate X into Y" means "how do you say X in Y"»* | **quasi** — una riscrittura di superficie è una relazione fra due forme, e nessuna riga la dichiara |
| **04** dimenticare | ritrattazione | *«"forget" is a retraction verb»* | **dicibile** — stessa classe di `relation_verb`, manca la riga |
| **05** confronto senza oggetti | muro cieco | *«a comparison needs two objects and a dimension»* | **non dicibile** — è uno **schema di ruoli**, S4b |
| **06** artefatto senza schema | schema assente | *«a decision record has a context, alternatives, a decision and rejected options»* | **non dicibile** — stesso schema di ruoli |
| **07** template fuori bersaglio | non pertinente | *«a counterexample question is answered by an instance, not by a causal account»* | **non dicibile** — è S3, la pertinenza |
| **08** sillogismo non motivato | risposta secca | *«answer a syllogism with its derivation»* | **non dicibile** — registro della risposta, S3 |
| **09** metadomanda sul metodo | domanda-wh | *«what is missing before X is the list of X's required roles»* | **non dicibile** — richiede prima 05/06 |

Letta così, la distribuzione dice una cosa netta e non ovvia:

> **Tre dei nove si chiudono con una frase che oggi si sa già dire; quattro
> chiedono tutti la stessa cosa mancante — uno SCHEMA DI RUOLI dicibile; due
> chiedono la pertinenza (S3).**

Cioè: il collo di bottiglia dell'insegnamento dialogico **non sono novanta forme
diverse**. Sono due, e sono esattamente le due che §6 aveva già isolato come
substrato (S4b e S3). La differenza è che ora hanno un criterio operativo che le
rende verificabili senza teoria:

> *Esiste una frase che un umano direbbe, che le installi?*

### 13.5 Che cosa manca perché il giro si chiuda da solo

Il seme del §13.3 chiede **quando si ferma su una parola**. Perché diventi il
processo generale servono, in quest'ordine:

1. **La domanda nasce dove il lettore si è fermato** (P0/S4b). Oggi la parola
   opaca è l'unico posto che il motore sa nominare; con un lettore composizionale
   la domanda diventa *«ho letto "compare due X", mi manca il ruolo Y»* — ed è la
   stessa cosa che serve ai quattro casi di schema.
2. **Uno schema di ruoli si dice** (S4b, lato superficie). *«un confronto ha due
   oggetti e una dimensione»* deve installare `schema_shape/2` +
   `role_required/1`. La forma esiste in KB dal gen412; manca la frase.
3. **La memoria di ciò che si è già chiesto** (S4). Chiedere due volte la stessa
   cosa è peggio che non chiedere: è la regola dei tre colpi applicata al
   dialogo, e senza di essa il muro diventa un questionario.
4. **La verifica è il ripasso del turno**, non «la risposta è cambiata». Costa
   poco (si ripone la frase) e non richiede S3 — che resta necessario per i due
   casi di pertinenza, non per gli altri sette.

### 13.6 Il rischio, e come si riconosce

Un sistema che chiede diventa fastidioso, e c'è un modo preciso di sbagliarlo:
**offrire forme che non c'entrano**. Il seme già lo mostra — a `governs` propone
anche *«something is a governs»*, che è la forma sbagliata per un verbo.

Il rimedio non è una guardia nel C: è che **l'offerta sia scelta dalla posizione
in cui il lettore si è fermato** (punto 1), invece che dal fatto che si è fermato.
Finché quella posizione non esiste, il menu è un elenco fisso, e va tenuto corto
di proposito (`teaching_offer_max`, oggi due).

> **La regola che riassume:** una richiesta di insegnamento è onesta se **nomina
> dove ci si è fermati** e **consegna la frase**. Se nomina solo la parola, è un
> muro con più parole.

---

## Riferimenti

- `docs/plans/question-emergence.md` — il piano che ha costruito il ciclo (gen406-411)
- `docs/plans/reinforcement-suite.md` — la batteria che lo misura; §«Il meta-problema» per la diagonale
- `docs/plans/parrot0-100-failures.md` — i cento prompt di §3
- `docs/plans/universal-comprehension.md` — il declino informato, di cui §4a è il conto
- `tests/rl/episodes/macchineria/riflessiva/` — gli episodi vivi: `ponte-che-generalizza`, `autocorrezione-sul-muro`
- `kb/core/meta.p0` — `bridge_shape/2`, `self_correct_on_wall/1` e il perché è spento
- `docs/plans/autocrescita.md` — **il piano che chiude il ciclo**: le sei
  sorgenti di lacuna, le cinque strategie in parallelo, il gate unico e il
  perimetro anti-impostore (la forma D non ha un generatore)
- `docs/plans/fix-patterns.md` — **lo studio delle riparazioni vere**: 959 righe
  di KB contate e classificate per forma, sette forme, tre meccanizzabili, e la
  scoperta che sette difetti su sette erano conoscenza dichiarata che non poteva
  funzionare
- `docs/plans/teach-comprehension-via-prompt.md` — le forme che oggi si insegnano
  parlando: è l'inventario da cui §13 pesca le frasi da consegnare
- `docs/issues/` — i nove prompt, riletti in §13.4 come richieste di insegnamento
