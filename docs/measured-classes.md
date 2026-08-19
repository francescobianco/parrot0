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
domanda | risposta attesa
```

Il separatore è ` | ` — pipe, con gli spazi. **Un file `.qa` non supporta
nient'altro:** niente commenti, niente direttive, niente righe speciali. Ogni
riga è un prompt con la sua risposta attesa, e quello è tutto il formato.

Il pipe è anche un prompt valido di un byte, e la riga `| | what would you like`
si legge senza ambiguità perché il separatore è il **primo** ` | ` della riga.

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
classe   1   stazza 3     risolte 1/3     (55/68 prompt)
            non risolti: [-] [.] [/] [0] [1] [2] [3] [4] [5] [6] [7] [8] [9]
----------------------------------------------------------------------
STAZZA 3   —   risolte 1/3   (lunghezza massima misurata: 1)
```

Quattro numeri, quattro significati diversi:

- **la stazza** — quante risposte distinte il corpus contiene. È la **mole della
  misura**, e cresce solo curando altre righe;
- **le risolte** — quante di quelle capacità parrot0 dimostra per intero;
- **il conto dei prompt** — a che punto è la copertura *dentro* una capacità. Non
  entra nei primi due, ma è quello che si guarda mentre si lavora;
- **la lunghezza massima misurata** — fin dove è arrivato lo spazzolamento. Una
  stazza di 3 a lunghezza 1 e una stazza di 3 a lunghezza 40 non sono la stessa
  cosa, e il numero da solo non lo direbbe.

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

## 8. La prima misura — 19 agosto 2026

```
STAZZA 3   —   risolte 1/3   (lunghezza massima misurata: 1)
```

**Stazza 3 a lunghezza massima 1.** Il corpus chiede tre capacità, perché davanti
a un byte ci sono tre risposte giuste diverse; parrot0 ne dimostra una per
intero.

| capacità | membri | esito |
|---|---|---|
| **lettere** — *«Hi there! What would you like to talk about?»* | 26 | **26/26 ✔** |
| **punteggiatura** — *«That's just punctuation, not words — what would you like to ask?»* | 32 | 29/32 — cadono `-` `.` `/` |
| **cifre** — *«That's a single digit, not a question — what would you like to ask?»* | 10 | 0/10 — muro cieco su tutte |

Il gruppo della punteggiatura è il più istruttivo. Non manca niente: la capacità
c'è e funziona su ventinove segni su trentadue. **Tre caratteri non entrano in
una classe che esiste già ed è dichiarata in KB** — e finché non ci entrano, la
capacità non conta.

Le cifre sono un lavoro diverso: ricevono *«I don't understand that yet.»*, dieci
muri ciechi. Una cifra non è incomprensibile — è una cifra, e l'attesa scritta
nel file è una risposta che oggi **non esiste**, che è precisamente il suo
mestiere.

È l'effetto che questa misura deve produrre: **non «parrot0 è scarso», ma «ecco
i tredici punti dove non arriva, per nome»** — e il numero non si muove finché
non ci arriva su tutti.
