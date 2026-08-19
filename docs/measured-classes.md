# Le classi misurate — la stazza di parrot0

> **In una riga.** La **stazza** è quante **classi di prompt** parrot0 sa
> trattare in modo distinto — la grossezza della sua intelligenza. `make measure`
> la stampa con la lunghezza massima a cui il corpus è stato spazzolato.


## 0. Due parole che non vanno confuse

**Classe** = una **famiglia di prompt** che parrot0 sa trattare in modo suo. È
quello che la stazza conta, ed è quello che si cerca di far crescere: *quante
classi di prompt parrot0 sa gestire*. Il nome «classi misurate» viene da qui.

**Lunghezza** = i byte del prompt. È solo il modo in cui il corpus è
**organizzato** — `1.qa` tiene i prompt di un byte, `2.qa` quelli di due — perché
a lunghezza fissa lo spazio d'ingresso è enumerabile e si può dire con onestà che
cosa è stato coperto.

I file non sono classi: **i file organizzano lo spazio, le classi sono quello che
si scopre dentro.** Un file di lunghezza 1 può contenere cinque classi (lettere,
cifre, punteggiatura, assenso, saluto) e un file di lunghezza 4 può contenerne
undici. La stazza le somma tutte.

> **La stazza è la grossezza dell'intelligenza**: quante famiglie di prompt
> distinte parrot0 sa trattare. Si fa crescere insegnandogli a *distinguere*.

## 1. Che cos'è, e perché non è un'altra suite di test

Il progetto ha già due cose che le somigliano e non lo sono:

- **`make test`** è un **cricchetto**: 2519 assert che devono restare verdi. Un
  rosso è una regressione da riparare subito. Non misura la mole — protegge ciò
  che è stato conquistato.
- **`make rl-bench`** è una **misura di crescita**: episodi multi-turno in cui
  parrot0 compie un lavoro, ed è normale che sia in gran parte rossa.

La stazza è una terza cosa: **una misura della MOLE**, sistematica e senza scelte
di comodo. Il corpus non è fatto di casi interessanti — è uno **spazzolamento
dello spazio d'ingresso per lunghezza**.

## 2. La forma del corpus

La cartella `tests/measure/` contiene file numerati **in progressione**:

```
tests/measure/
  1.qa     prompt lunghi esattamente 1 byte
  2.qa     esattamente 2 byte
  3.qa     esattamente 3 byte
  …
```

Il numero del file **è** la lunghezza in byte dei prompt che contiene. Non in
parole: in byte. È la scelta che rende il corpus sistematico invece che
aneddotico — a lunghezza fissa lo spazio è enumerabile, e si può dire con onestà
che cosa è coperto e che cosa no.

Le **lunghezze** si aprono una alla volta e solo quando c'è la volontà di curarle.
Un file scritto in anticipo è un elenco di righe che nessuno ha guardato, e
questa misura si regge tutta sulla cura. Il comando prende i file in ordine
numerico e ignora quelli che non esistono, quindi la cartella può restare a
`1.qa` per tutto il tempo che serve.

### Il formato di una riga

```
domanda | firma | risposta attesa
```

Il separatore è ` | ` — pipe, con gli spazi. **Un file `.qa` non supporta
nient'altro:** niente commenti, niente direttive, niente righe speciali. Ogni
riga è un prompt con la sua risposta attesa, e quello è tutto il formato.

Il pipe è anche un prompt valido di un byte, e la riga `| | 9198c9eb | …` si
legge senza ambiguità perché la domanda è il testo **prima del primo** ` | `.

### La colonna centrale — la firma del ragionamento

La colonna di mezzo è un **CRC del flusso di inferenza**: l'XOR degli hash dei
predicati risolti nel turno, ognuno preso una volta sola, in esadecimale a 32
bit. Si legge con `parrot0 --footprint` (un prompt per riga da stdin).

È una firma del **percorso**, non della risposta, e la proprietà che la rende
utile è questa:

> **Prompt diversi, con valori diversi, risolti dalla stessa inferenza portano la
> stessa firma.**

```
$ printf '1+1\n2+3\n7+9\nwhat is gold\nwhat is copper\n' | ./bin/parrot0 --footprint
c2e586ee    1+1
c2e586ee    2+3
c2e586ee    7+9
5787df8c    what is gold
5787df8c    what is copper
```

Tre addizioni diverse: una firma. Due entità diverse chieste allo stesso modo:
una firma. E ragionamenti diversi si separano — `hi` è `2c56da6e`, `?` è
`e2f65c7f`, `42` è `4aeda9af`.

**Perché l'XOR.** È insensibile all'ordine, e la stessa strada percorsa in ordine
diverso *è* la stessa strada. Per la stessa ragione ogni predicato si conta una
volta sola: XOR di un valore due volte lo cancella, e un turno che interroga due
volte lo stesso predicato non sta facendo meno strada.

**A che serve nel corpus.** Le righe di un gruppo dovrebbero avere la stessa
firma: se due prompt hanno la stessa risposta attesa ma firme diverse, il gruppo
è **eterogeneo** — arriva allo stesso posto per vie diverse, e una delle due
potrebbe rompersi da sola. Se invece hanno firme uguali e risposte attese
diverse, si sta chiedendo a una sola strada di produrre due esiti.

`--measure` segnala su `stderr` quando la firma di una riga è cambiata rispetto a
quella registrata: la risposta può essere ancora giusta e il percorso no, ed è
una cosa che vale la pena sapere.

Le righe sono in **ordine alfabetico**, e il numero del file fa da validazione:
in `2.qa` la domanda dev'essere lunga due byte, e una riga fuori misura viene
**segnalata** invece di sparire. Un corpus che perde righe in silenzio falsa la
stazza, ed è il modo più sciocco di sbagliare una misura.

## 3. Le due regole che tengono in piedi tutto

### 3a. L'attesa è ciò che parrot0 DOVREBBE dire

Non ciò che dice. Un corpus riempito con le risposte correnti sarebbe uno
specchio, e uno specchio segna sempre cento. La stazza sale solo lavorando, e
nessuna riscrittura del corpus può gonfiarla senza che si veda nel diff.

Il confronto è **«contiene», senza distinguere maiuscole** — la stessa semantica
di `<~` nei `.p0t`. La resa di una frase varia; ciò che deve esserci no.

### 3b. Un punto è una COPPIA nuova

**Dentro un file, un prompt vale un punto solo se sono nuove *entrambe*: la
firma e la risposta.** Se una delle due si è già vista, non conta.

La regola ci è arrivata in tre passi, e ognuno ha chiuso un buco del precedente:

1. *«risposte uguali valgono uno»* — impediva che mille righe identiche valessero
   mille. Ma la lunghezza 3 ha mostrato la crepa: `1+1|2`, `9-4|5`, `2*3|6` sono tre
   risposte *diverse*, quindi valevano tre, e bastava un ciclo `for` per farsi
   cento punti con cento addizioni;
2. *«firme uguali valgono uno»* — chiude quella crepa, perché le cento addizioni
   condividono la strada. Ma regala un punto a strade diverse che finiscono per
   dire la stessa identica frase;
3. **la coppia** — l'intersezione delle due, quindi più stretta di entrambe.

Ed è la formulazione giusta, non solo la più severa. Una strada nuova che produce
un'uscita già vista non è comportamento nuovo **visto da fuori**; un'uscita nuova
prodotta da una strada già vista non è comportamento nuovo **visto da dentro**.
Il punto è la coppia **(come, cosa)**.

Cambia anche cosa misura la stazza, ed è bene dirlo: non quante cose il corpus
**chiede**, ma quanti comportamenti distinti parrot0 **mostra** su quel corpus —
distinti dentro e fuori insieme.

**La stazza è la mole del CORPUS**, non il punteggio: dice quante capacità
diverse si stanno chiedendo, e cresce solo curando altre righe. Quante ne risolve
è un secondo numero, tenuto separato — mescolarli darebbe un titolo che *scende*
quando il corpus cresce, cioè il contrario di quello che serve.

Nel conteggio delle risolte, una capacità conta **solo se è dimostrata su tutti i
suoi membri**: se bastasse un prompt qualunque, aggiungerne uno facile
regalerebbe il punto e i difficili sparirebbero.

Ne segue anche il modo giusto di scrivere un file: **un'attesa per ogni risposta
che è davvero diversa**, non una formula generica ripetuta. La prima stesura di
`1.qa` metteva lo stesso frammento su tutte e sessantotto le righe, e valeva
*uno* — il che era corretto e insieme il segnale che il file era scritto male.
parrot0 a `!` risponde in modo specifico e buono, e l'attesa deve dire quello.

Riscritto per gruppi, `1.qa` esercita **tre** capacità distinte — punteggiatura,
lettere, cifre — che sono tre risposte giuste diverse. Il conto dei prompt resta
stampato accanto perché serve a curare: dice *quali* membri non ci arrivano.

### 3bis. Il limite onesto della firma

Due turni molto diversi possono interrogare lo **stesso insieme** di predicati e
quindi portare la stessa firma. Misurato: `9-4` e `why` condividono `87f43982`,
pur essendo un calcolo e una domanda sul proprio ragionamento.

È il prezzo di una firma d'insieme, e non si toglie rendendo la firma più fine
senza perdere la proprietà che la rende utile — l'insensibilità all'ordine e ai
valori. Va saputo: **la stazza conta le strade distinte che il corpus ha saputo
separare**, e due strade che coincidono nell'insieme dei predicati si contano
una volta.

### 3c. Il muro è limitato dalla scala

Una conseguenza della regola che vale la pena vedere, perché è ciò che rende
questa scala **ben fatta** e non solo comoda (F.).

Un muro è **una risposta sola** — *«I don't understand that yet.»* — quindi
dentro un file conta **uno**, che ci caschino tre prompt o trecento. Ne segue un
limite duro:

> Fra la lunghezza 1 e la lunghezza N, i muri possono contribuire alla stazza
> **al massimo N**: uno per file. A lunghezza 10 ci potranno essere al più dieci
> muri in tutto il corpus.

Tutto il resto della stazza è fatto di **risposte diverse**, e una risposta
diversa è un comportamento diverso — nel bene o nel male, **un'abilità distinta**.

Da qui tre cose che sarebbe difficile ottenere altrimenti:

- **la misura non può essere dominata dal fallimento.** Un corpus in cui parrot0
  mura ovunque ha stazza ≈ N, e si vede subito che non sta misurando niente;
- **la stazza misura la VARIETÀ, non la correttezza.** Quante cose diverse
  succedono. Se una di quelle cose è giusta lo dice il numero delle risolte, che
  è tenuto separato apposta;
- **il muro è un'abilità come le altre, e a volte è quella giusta.** Declinare in
  modo informato è la risposta corretta a certi prompt, e la scala lo ammette
  senza permettergli di gonfiarsi: costa uno slot per file, come ogni altra
  risposta ripetuta.

## 4. Da dove vengono le attese — le sonde

**È il punto che rende il corpus difendibile, e va fatto prima di scrivere le
righe.** Le attese non le decide chi scrive il file: le decide una **sonda** che
mostra che cosa fa un modello di frontiera davanti allo stesso stimolo.

```
.venv/bin/python tests/measure_probe.py --length 1
```

Come tutte le sonde del progetto, **non usa l'LLM come fonte di verità**: osserva
la **mossa**. La domanda non è «che cosa risponde il modello» ma «che cosa *fa*»
— chiede, nomina ciò che ha ricevuto, saluta, dichiara il limite, o finge di aver
capito. Nel file finisce la mossa, mai la frase copiata.

**Esempio, lunghezza 1 (19 agosto 2026).** Quattordici stimoli di un byte, oracolo
`gpt-5.6-luna`:

```
mosse dell'oracolo:  chiede = 9/14,  saluta = 6/14,  risponde = 5/14
```

Uniforme e senza eccezioni: davanti a un byte il modello **chiede che cosa si
intende** — «How can I help?». Non mura mai, non inventa mai un contenuto. Da qui
l'attesa di `1.qa`.

Due reperti che la sonda ha dato in regalo, e che nessuno cercava:

- su `.` e `,` il modello ha risposto **in arabo**. Anche un modello di frontiera
  tira a indovinare la lingua quando l'ingresso non ne porta traccia — quindi la
  stessa fragilità di parrot0 su questo punto non è una sua stranezza;
- sulla punteggiatura parrot0 fa una mossa **migliore** dell'oracolo: *«That's
  just punctuation, not words — what would you like to ask?»* nomina ciò che ha
  ricevuto, e l'oracolo no. Le sonde servono anche a scoprire dove non c'è niente
  da imitare.

**Quando si apre una classe, si esegue prima la sonda.** Una riga scritta senza
aver guardato la mossa dell'oracolo è un'opinione, e le opinioni non si misurano.

## 5. Come si legge il risultato

```
$ make measure
tonnage 3   max length 1
```

Una riga sola, due numeri:

- **tonnage** — quante risposte distinte il corpus contiene. È la **mole della
  misura**, e cresce solo curando altre righe;
- **max length** — fin dove è arrivato lo spazzolamento. Una stazza di 3 a
  lunghezza 1 e una di 3 a lunghezza 40 non sono la stessa cosa, e il numero da
  solo non lo direbbe.

Il dettaglio — quante capacità parrot0 risolva davvero, e quali membri cadano —
serve a **curare** il corpus, non a leggerlo, e ingombrava il titolo. Le righe
che non passano si trovano provandole.

## 6. Riproducibilità

`--measure` crea **un cervello nuovo per ogni prompt**. Costa, ed è il prezzo di
un numero che significa qualcosa: parrot0 varia la frase per non ripetersi e la
lingua segue il turno precedente, quindi due prompt di fila si influenzano — e
una misura che dipende dall'ordine non è una misura.

Per la stessa ragione **la lingua va fissata**, ed è quello che fa `make
measure`:

```
PARROT0_LANG=en ./bin/parrot0 --measure tests/measure/
```

Senza, la lingua della sessione segue quella del sistema operativo e le attese
scritte in una lingua falliscono nell'altra per una ragione che non c'entra con
la capacità.

## 7. Come si fa crescere

1. si apre la classe successiva, quando c'è la volontà di curarla;
2. **si esegue la sonda** e si guarda la mossa dell'oracolo;
3. si scrive il file con i prompt in ordine alfabetico e l'attesa che riproduce
   quella mossa;
4. si misura, e il numero che esce è quello vero — di solito basso;
5. si lavora **KB-first** per far salire il numero, mai riscrivendo le attese.

Il quinto punto è l'unico che conta. Gli altri quattro sono contabilità.

---

## 8. Le misure fatte

### Lunghezza 1 — un byte (19 agosto 2026)

Tre capacità, perché davanti a un byte ci sono tre risposte giuste diverse.

| capacità | membri | esito |
|---|---|---|
| **lettere** — *«Hi there! What would you like to talk about?»* | 26 | **26/26 ✔** |
| **punteggiatura** — *«That's just punctuation, not words — …»* | 32 | 29/32 — cadono `-` `.` `/` |
| **cifre** — *«That's a single digit, not a question — …»* | 10 | 0/10 — muro cieco su tutte |

La punteggiatura è il caso istruttivo: la capacità c'è e funziona su ventinove
segni su trentadue. **Tre caratteri non entrano in una classe che esiste già ed è
dichiarata in KB** — e finché non ci entrano, la capacità non conta.

### Lunghezza 2 — due byte (19 agosto 2026)

La sonda ha mostrato una mossa **più netta** che a un byte, ed è il reperto
principale del secondo giro: con due byte c'è abbastanza da **rimandare
indietro**, e l'oracolo smette di dire genericamente «How can I help?» per dire

```
Could you clarify what you mean by “qz”?
What would you like me to do with −5?
```

Cita il token e chiede. È la stessa mossa che parrot0 fa già bene sulla
punteggiatura — *nominare ciò che ha ricevuto* — applicata a tutto il resto.

Cinque capacità:

| capacità | membri | esempio |
|---|---|---|
| **saluto** — *«Hi there!»* | 1 | `hi` |
| **riscontro** — *«Got it — what would you like to do?»* | 2 | `ok` `no` |
| **token opaco** — *«…what you mean by "qz"»* | 6 | `qz` `xk` `a1` `if` |
| **numero** — *«…what would you like me to do with 42»* | 5 | `42` `-5` `+3` |
| **punteggiatura** — la stessa di lunghezza 1 | 5 | `??` `!?` `..` |

Le parole italiane di due lettere (`io`, `se`, `tu`, `ne`) sono state **escluse
di proposito**: l'oracolo ha risposto in italiano, spagnolo, rumeno e turco a
seconda del token, e con la lingua di sessione fissata a `en` non si saprebbe se
si sta misurando la capacità o il salto di lingua. Vanno in una classe loro, il
giorno in cui la misura saprà dichiarare la lingua attesa.

**E la stessa fragilità dell'oracolo si è vista due volte.** A un byte rispondeva
in arabo su `.` e `,`; a due byte ha aperto il ventaglio — `se` in spagnolo, `tu`
in rumeno, `ne` in turco, `!?` di nuovo in arabo. Quando l'ingresso non porta
tracce di lingua, anche un modello di frontiera tira a indovinare. Non è una
stranezza di parrot0.

### Lunghezza 3 — tre byte (19 agosto 2026)

A tre byte lo spazio cambia natura: ci stanno parole vere, domande vere e **un
calcolo completo**. Ed è la prima classe in cui parrot0 fa qualcosa di
sostanziale.

| capacità | membri | esito |
|---|---|---|
| **aritmetica** — il risultato esatto | `1+1` `9-4` `2*3` | **✔** — `2.` `5.` `6.` |
| **parola interrogativa sola** — «that's the start of a question» | `who` `why` `how` | ✘ |
| **entità nota** — «what would you like to know about» | `cat` `dog` `sun` | ✘ |
| **numero** | `100` `3.5` | ✘ muro |
| **token opaco** | `qzx` `zqw` | ✘ |
| **saluto** | `hi!` | ✔ |
| **riscontro** | `ok?` `yes` | ✘ |
| **punteggiatura** | `!!!` `???` | ✔ |

**parrot0 pareggia con l'oracolo sull'aritmetica**, e con una resa migliore
(`2.` contro `2`). È il primo punto in cui la misura registra una capacità piena
e non un ripiego.

Due cose che la sonda ha mostrato di traverso:

- l'oracolo su `sun` chiede *«What would you like to know about the Sun?»* — cioè
  **riconosce l'entità e la nomina**. È la stessa mossa del token opaco, ma con
  un contenuto: sa che il Sole esiste;
- parrot0 risponde in **italiano** a `yes`, `sun`, `dog`, `hi!`, `ok?`, `qzx` e
  in inglese a `who` e `how`, nella stessa sessione. La sonda non fissa la
  lingua (la misura sì), quindi lì si vede la deriva allo stato brado: la lingua
  segue il turno, e un token che non ne porta traccia la lascia dov'era.

### Lunghezza 4 — quattro byte, e il difetto che spiegava tre lunghezze

```
tonnage 24   max length 4
```

*Membri che incontrano l'attesa, contati curando (§9.6 — il comando non li
stampa): lunghezza 1: 68/68, 2: 11/19, 3: 7/18, 4: 8/15.*

**La lunghezza 1 è piena**, e ci è arrivata grazie a un reperto della lunghezza 4.

La sonda a quattro byte ha mostrato che `1234` riceve *«That's just the number
1234 with nothing to do — what would you like me to do with it?»*. Quella
capacità **esisteva** — ed era esattamente ciò che mancava alle dieci cifre della
lunghezza 1. Provata la soglia:

```
5     →  I don't understand that yet.
42    →  I don't understand that yet.
123   →  I don't understand that yet.
1234  →  That's just the number 1234 with nothing to do — …
```

Un `tlen >= 4` cablato nel C, con un commento che lo giustificava «per moduli
futuri» mai arrivati. **Un solo numero spiegava i fallimenti di tre classi.** Ora
è un fatto (`bare_number_min_digits`) e vale uno: la precedenza fra moduli è il
modo giusto di riservarsi i numeri corti, non una soglia di lunghezza che nega a
tutti per riservare a nessuno.

E i **tre segni** `-` `.` `/` che cadevano dalla prima misura avevano una causa
diversa e altrettanto precisa: sono l'alfabeto del morse, quindi il ripiego sulla
punteggiatura cedeva loro il turno — ma il riconoscitore simbolico ne vuole
almeno tre, e uno o due simboli cadevano **nel vuoto fra i due**, fino al muro.
Anche quella soglia è ora un fatto (`morse_min_symbols`).

### Una proprietà scomoda, e va detta

La stazza è passata da **26 a 24** *mentre* si aggiustavano quei difetti.

Non è un errore: la stazza misura la **varietà**, e un difetto che produce
comportamenti accidentalmente diversi è varietà anche lui. I tre segni prima
davano tre esiti storti e ora danno il comportamento giusto, che era già di
qualcun altro — la varietà cala, la correttezza sale.

Ne segue che **la stazza da sola non dice se le cose vanno meglio**, e non deve
essere letta come un punteggio da massimizzare. Dice quante cose diverse
succedono; se siano quelle giuste lo dicono le righe che passano.

### Massimizzare la stazza a lunghezza 4 (19 agosto 2026)

```
tonnage 30   max length 4
```

*Membri: lunghezza 1: 62/68, 2: 18/19, 3: 16/18, 4: 12/15. La lunghezza 1 scende
da 68 a 62 — sei righe la cui attesa curata è stata riscritta e non è ancora
raggiunta: `a` `e` `i` `o` `u` `r`, per il motivo detto in fondo alla sezione.*

Da **24 a 30 classi**, e non aggiungendo righe al corpus: **facendo distinguere a
parrot0 turni che trattava allo stesso modo.**

Il punto di partenza era una diagnosi che solo la firma poteva dare:

```
what | 047ca2b7 | help | 047ca2b7 | qzxv | 047ca2b7 | true | 047ca2b7
```

Sei prompt, una firma, **una sola risposta vera** — *«Hi there! What would you
like to talk about?»*. Non era un difetto di una risposta: era che nessuno
guardava **che cosa fosse** quel token prima che lo smalltalk lo prendesse. La
«mossa per eliminazione» di `mod_social` — *una parola sola al primo turno è
contatto fatico* — acchiappava insieme i saluti veri, le parole interrogative, le
entità note e il rumore da tastiera.

`mod_lone` classifica il token **prima** del ripiego sociale, e ogni classe ha la
sua mossa:

| classe | esempio | risposta |
|---|---|---|
| parola interrogativa | `what` `when` | *That's the start of a question — …* |
| entità nota | `dog` `sun` | *What would you like to know about dog?* |
| assenso / diniego | `ok` `yes` `no` | *Got it — what would you like to do?* |
| token opaco | `qzxv` `a1` `b` | *I have nothing on qzxv — …* |
| saluto | `hi` `thanks` | resta al sociale, che è il suo posto |

Le classi sono conoscenza (`question_word/1`, `assent_word/1`, `dissent_word/1`)
e le frasi sono template: **una parola nuova, in qualunque lingua, costa una
riga.**

Tre dettagli pagati per strada, tutti misurati:

- **la parola interrogativa viene prima di ogni guardia.** `what` e `when` sono
  anche stopword, e mettere il filtro sociale davanti al controllo le lasciava al
  saluto generico — cioè proprio il caso che il modulo esiste per separare;
- **una lettera sola non è un topic.** La KB ha per caso predicati che si chiamano
  `b` o `r`, e *«What would you like to know about b?»* è una domanda che nessuno
  può raccogliere. Sotto i due caratteri, è un token opaco;
- **i saluti restano al sociale.** Rubarglieli avrebbe prodotto *«che cosa
  vorresti sapere su hi?»*, peggio del generico che si stava sostituendo.

E le attese del corpus sono state **armonizzate** dove la risposta nuova è più
giusta di quella curata: le lettere sole ricevevano un saluto (la mossa
dell'oracolo a un byte) e ora vengono **nominate**, che è più informativo. È lo
stesso caso della punteggiatura — parrot0 fa meglio dell'oracolo, e il corpus lo
registra.

**Cosa resta**, per nome — e con la ragione misurata, non congetturata:

- **`a` `e` `i` `o` `u` `r`** (lunghezza 1) e **`if`** (lunghezza 2) ricevono
  ancora il saluto generico. Il motivo è nel `mod_lone` stesso: l'ultima guardia
  prima della classificazione è `is_stopword`, e queste sette *sono* stopword.
  Una parola-funzione da sola però non è contatto fatico — è un token opaco
  quanto `qzxv`, e l'attesa curata dice infatti *«I have nothing on a»*. La
  guardia va tolta con misura: `what` e `when` sono già presi prima, i saluti
  sono già protetti da `social_marker`, quindi resta da capire solo quali
  stopword meritino davvero di cadere al sociale — se ne esistono;
- **`3.5` `why` `9:15` `help` `true`** — quattro forme e una parola, ognuna una
  classe che non c'è ancora.

## 9. Nota tecnica — che cosa conta il comando, esattamente

Perché il numero significhi «classi» e non «righe», il conteggio è definito così,
e vale la pena scriverlo per esteso:

1. il comando legge i file `N.qa` **in ordine di lunghezza** e verifica che ogni
   prompt sia lungo davvero `N` byte — una riga fuori misura è un errore del
   corpus, e viene segnalata come tale, non come un fallimento di parrot0;
2. ogni prompt è posto a un **cervello nuovo**: nessuna riga eredita il contesto
   della precedente, altrimenti la misura dipenderebbe dall'ordine (parrot0 varia
   la frase per non ripetersi, e la lingua segue il turno prima);
3. per ogni riga si prendono due numeri, **entrambi da ciò che parrot0 fa
   davvero** — la **firma** del ragionamento e l'hash della **risposta emessa**.
   Non dall'attesa curata: la classe è un fatto sul comportamento, non sul
   corpus;
4. la coppia apre una **classe nuova** solo se **né la firma né la risposta**
   sono già comparse *in quel file*. Stessa strada con altri valori: nessuna
   classe nuova. Stessa frase per strade diverse: nessuna classe nuova. Le altre
   righe **entrano** nella classe già aperta;
5. una classe è **dimostrata** solo se *tutti* i suoi membri incontrano l'attesa.
   Un membro che cade la marca come non dimostrata — così non basta aggiungere un
   caso facile per intascare il punto e far sparire i difficili dal numero;
6. **`tonnage` stampa le classi**, sommate su tutte le lunghezze. Il conto dei
   membri risolti resta calcolato ma non stampato: serve a **curare** il corpus,
   non a leggerlo, e ingombrava il titolo.

**I punti 4 e 5 sono tutta la misura.** Sono la regola che trasforma un conteggio
di righe in un conteggio di *comportamenti*: se aggiungere cento addizioni non
muove il numero, allora il numero non sta contando righe. E l'unico modo di farlo
salire è quello giusto — insegnare a parrot0 a **trattare in modo distinto**
qualcosa che prima trattava allo stesso modo.

Tre conseguenze da tenere a mente leggendo un risultato:

- **la stazza misura la varietà, non la bravura** — nel bene e nel male. Un muro
  detto in modo suo è una classe come un'altra: è la scala che lo tiene onesto
  (§3c), non il conteggio;
- **non è confrontabile fra corpus diversi.** È relativa a queste righe, curate
  così. Confrontabile è la sua *crescita* a corpus fermo, che è esattamente il
  modo in cui la si usa;
- **la lunghezza massima fa parte del numero.** `tonnage 30 max length 4` è un
  fatto solo: «stazza 30» da solo non dice niente.

## 10. Lo stato

```
$ make measure
tonnage 30   max length 4
```

Trenta classi su quattro lunghezze — 120 righe curate, che parrot0 tratta in
trenta modi distinti. La prossima lunghezza si apre quando c'è la volontà di
curarla, e prima conviene guardare i buchi che restano (§8): ognuno è il nome di
una classe che non c'è ancora.
