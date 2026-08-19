# Le classi misurate — la stazza di parrot0

> **In una riga.** La **stazza** è quante **risposte distinte** il corpus curato
> contiene — cioè quante capacità diverse si stanno chiedendo a parrot0. `make
> measure` la stampa insieme a quante ne risolve, e alla lunghezza massima a cui
> la misura è stata fatta.

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

Le classi si aprono **una alla volta e solo quando c'è la volontà di curarle**.
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

### 3b. I doppioni non si contano

**Dentro un file, risposte attese uguali valgono uno.** La stazza non conta i
prompt: conta le **risposte distinte**. Senza questa regola bastava aggiungere
mille righe con la stessa attesa per farla salire di mille.

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

### 3bis. Il caso che la regola non copre — le famiglie parametriche

L'ha trovato la classe 3, ed è giusto scriverlo invece di lasciarlo scoprire a
qualcun altro.

La regola dei doppioni protegge dal ripetere **la stessa** risposta. Non protegge
dall'enumerare una **famiglia parametrica**: `1+1 | 2`, `9-4 | 5`, `2*3 | 6` sono
tre risposte diverse, quindi valgono tre — e nulla impedisce di aggiungerne cento
e farsi cento punti con un ciclo `for`.

Non c'è una regola meccanica che lo impedisca, perché quelle risposte *sono*
davvero diverse. C'è invece una disciplina, e va rispettata scrivendo:

> **Di una famiglia parametrica si mettono i membri che coprono le FORME, non i
> valori.** L'aritmetica a tre byte ha tre righe — una per operatore — perché ci
> sono tre operatori, non perché tre sia un bel numero. Una quarta riga con
> `4+5` non aggiunge nessuna capacità: aggiunge un valore.

Il criterio pratico: **se una riga nuova può fallire per una ragione che nessuna
riga esistente copre, va messa. Altrimenti no.**

### 3c. Il muro è limitato dalla scala

Una conseguenza della regola dei doppioni che vale la pena vedere, perché è ciò
che rende questa scala **ben fatta** e non solo comoda (F.).

Un muro è **una risposta sola** — *«I don't understand that yet.»* — quindi
dentro un file conta **uno**, che ci caschino tre prompt o trecento. Ne segue un
limite duro:

> Fra la classe 1 e la classe N, i muri possono contribuire alla stazza **al
> massimo N**: uno per classe. A lunghezza 10 ci potranno essere al più dieci
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
  senza permettergli di gonfiarsi: costa uno slot per classe, come ogni altra
  risposta ripetuta.

## 4. Da dove vengono le attese — le sonde

**È il punto che rende il corpus difendibile, e va fatto prima di scrivere le
righe.** Le attese non le decide chi scrive il file: le decide una **sonda** che
mostra che cosa fa un modello di frontiera davanti allo stesso stimolo.

```
.venv/bin/python tests/measure_probe.py --class 1
```

Come tutte le sonde del progetto, **non usa l'LLM come fonte di verità**: osserva
la **mossa**. La domanda non è «che cosa risponde il modello» ma «che cosa *fa*»
— chiede, nomina ciò che ha ricevuto, saluta, dichiara il limite, o finge di aver
capito. Nel file finisce la mossa, mai la frase copiata.

**Esempio, classe 1 (19 agosto 2026).** Quattordici stimoli di un byte, oracolo
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

### Classe 1 — un byte (19 agosto 2026)

Tre capacità, perché davanti a un byte ci sono tre risposte giuste diverse.

| capacità | membri | esito |
|---|---|---|
| **lettere** — *«Hi there! What would you like to talk about?»* | 26 | **26/26 ✔** |
| **punteggiatura** — *«That's just punctuation, not words — …»* | 32 | 29/32 — cadono `-` `.` `/` |
| **cifre** — *«That's a single digit, not a question — …»* | 10 | 0/10 — muro cieco su tutte |

La punteggiatura è il caso istruttivo: la capacità c'è e funziona su ventinove
segni su trentadue. **Tre caratteri non entrano in una classe che esiste già ed è
dichiarata in KB** — e finché non ci entrano, la capacità non conta.

### Classe 2 — due byte (19 agosto 2026)

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
| **punteggiatura** — la stessa di classe 1 | 5 | `??` `!?` `..` |

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

### Classe 3 — tre byte (19 agosto 2026)

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

### Lo stato### Lo stato

```
$ make measure
tonnage 8   max length 2
```
