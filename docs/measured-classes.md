# Le classi misurate — la stazza di parrot0

> **In una riga.** `parrot0 --measure tests/measure/` conta quanti prompt curati
> parrot0 risolve e stampa la somma. *«Oggi parrot0 è a classe 103»* vuol dire
> che di quelli ne risolve centotré.

## 1. Che cos'è, e perché non è un'altra suite di test

Il progetto ha già due cose che sembrano questa e non lo sono:

- **`make test`** è un **cricchetto**: 2519 assert che devono restare verdi. Un
  rosso è una regressione da riparare subito. Non misura la mole, protegge ciò
  che è stato conquistato.
- **`make rl-bench`** è una **misura di crescita**: episodi multi-turno in cui
  parrot0 compie un lavoro, ed è normale che sia in gran parte rossa.

La stazza è una terza cosa: **una misura della MOLE**, sistematica e senza
scelte di comodo. Il corpus non è fatto di casi interessanti — è uno
**spazzolamento dello spazio d'ingresso per lunghezza**.

## 2. La forma del corpus

Una cartella (`tests/measure/`) con file `N.qa`, dove **N è la lunghezza in byte** dei
prompt che contiene:

```
tests/measure/
  1.qa     prompt lunghi esattamente 1 byte
  2.qa     esattamente 2 byte
  3.qa     …
```

Dentro ogni file, una riga per prompt, **in ordine alfabetico**:

```
domanda | risposta attesa
```

Il separatore è ` | ` — pipe, con gli spazi. **Un file `.qa` non supporta
nient'altro:** niente commenti, niente direttive, niente righe speciali. Ogni
riga è un prompt con la sua risposta attesa, e questo è tutto il formato.

Il pipe è anche un prompt valido di un byte, e la riga `| | what would you like`
si legge senza ambiguità perché il separatore è il **primo** ` | ` della riga.
**Il numero del file è la validazione:** in `N.qa` la domanda dev'essere lunga
esattamente N byte, e una riga fuori misura viene segnalata invece di sparire. Un
corpus che perde righe in silenzio falsa la stazza verso il basso, ed è il modo
più sciocco di sbagliare una misura — trovato al primo giro contando 65 righe su
66, quando il formato aveva ancora i commenti.

**La lunghezza è in byte, non in parole.** È la scelta che rende il corpus
sistematico invece che aneddotico: a lunghezza fissa lo spazio è enumerabile, e
si può dire con onestà che cosa è stato coperto e che cosa no.

## 3. La regola che tiene in piedi tutto

> **La risposta attesa è quello che parrot0 DOVREBBE dire, non quello che dice.**

Un corpus riempito con le risposte correnti sarebbe uno specchio, e uno specchio
segna sempre cento. La stazza sale solo lavorando, e nessuna riscrittura del
corpus può gonfiarla senza che si veda nel diff.

Il confronto è **«contiene», senza distinguere maiuscole** — la stessa semantica
di `<~` nei `.p0t`. La resa di una frase varia; ciò che deve esserci no.

## 4. Da dove vengono le risposte attese — le sonde

**È il punto che rende il corpus difendibile, e va fatto prima di scrivere le
righe.** Le attese non le decide chi scrive il file: le decide una **sonda** che
mostra che cosa fa un modello di frontiera davanti allo stesso stimolo.

```
.venv/bin/python tests/measure_probe.py --class 1
```

Come tutte le sonde del progetto, **non usa l'LLM come fonte di verità**: osserva
la **mossa**. La domanda non è «che cosa risponde il modello» ma «che cosa *fa*»
— chiede, nomina ciò che ha ricevuto, saluta, dichiara il limite, o finge di aver
capito. Nel `N.qa` finisce la mossa, mai la frase copiata.

**Esempio, classe 1 (misurato il 19 agosto 2026).** Quattordici stimoli di un
byte, oracolo `gpt-5.6-luna`:

```
mosse dell'oracolo:  chiede = 9/14,  saluta = 6/14,  risponde = 5/14
```

Uniforme e senza eccezioni interessanti: davanti a un byte il modello **chiede
che cosa si intende** — «How can I help?». Non mura mai, non inventa mai un
contenuto. Da qui l'attesa di `1.qa`: *parrot0 deve chiedere*.

Due reperti che la sonda ha dato in regalo, e che nessuno cercava:

- su `.` e `,` il modello ha risposto **in arabo**. Anche un modello di frontiera
  tira a indovinare la lingua quando l'ingresso non ne porta nessuna traccia —
  quindi la stessa fragilità di parrot0 su questo punto non è una sua stranezza;
- parrot0 sulla punteggiatura fa una mossa **migliore** dell'oracolo: *«That's
  just punctuation, not words — what would you like to ask?»* nomina ciò che ha
  ricevuto, e l'oracolo no. Le sonde servono anche a questo: a scoprire dove non
  c'è niente da imitare.

**Quando si aggiunge una classe, si esegue prima la sonda.** Una riga di `N.qa`
scritta senza aver guardato la mossa dell'oracolo è un'opinione, e le opinioni
non si misurano.

## 5. Come si legge il risultato

```
$ make measure
classe   1    55/68
            non risolti: [-] [.] [/] [0] [1] [2] [3] [4] [5] [6] [7] [8] [9]
----------------------------------------------------------------------
STAZZA 55   (su 68 prompt curati)
```

Tre numeri, tre significati diversi:

- **la stazza** — quanti prompt curati risolve. È il titolo;
- **il totale curato** — quanti ne esistono. Cresce quando si aggiungono classi,
  e da solo non è un merito;
- **i non risolti, per nome** — il lavoro che resta, già elencato.

Una classe **piena** (`ok == n`) è segnata come tale: vuol dire che quella
lunghezza è stata coperta, e che il prossimo lavoro sta altrove.

## 6. Riproducibilità

`--measure` crea **un cervello nuovo per ogni prompt**. Costa, ed è il prezzo di
un numero che significa qualcosa: parrot0 varia la frase per non ripetersi e la
lingua segue il turno precedente, quindi due prompt di fila si influenzano — e
una misura che dipende dall'ordine non è una misura.

Per la stessa ragione **la lingua va fissata**:

```
PARROT0_LANG=en ./bin/parrot0 --measure tests/measure/
```

Senza, la lingua della sessione segue quella del sistema operativo, e le attese
scritte in una lingua falliscono nell'altra per una ragione che non c'entra con
la capacità.

## 7. Come si fa crescere

**Una classe alla volta, e solo quando serve.** `N` è generico — dopo la 1 viene
la 2, poi la 3 — ma le classi **non si pre-generano**: se ne apre una quando c'è
la volontà di curarla, perché un file scritto in anticipo è un elenco di righe
che nessuno ha guardato, e la misura si regge sulla cura. Il comando le prende
in ordine numerico e ignora quelle che non esistono, quindi la cartella può
restare a `1.qa` per tutto il tempo che serve.

Quando si apre la successiva:

1. si sceglie la classe N successiva;
2. **si esegue la sonda** (`tests/measure_probe.py --class N`) e si guarda la
   mossa dell'oracolo;
3. si scrive `N.qa` con i prompt in ordine alfabetico e l'attesa che riproduce
   quella mossa;
4. si misura, e il numero che esce è quello vero — di solito basso;
5. si lavora **KB-first** per far salire il numero, mai riscrivendo le attese.

Il quinto punto è l'unico che conta. Gli altri quattro sono contabilità.

---

## 8. La prima misura — 19 agosto 2026

```
STAZZA 55   (su 68 prompt curati)
```

**Classe 1 — 55/68.** I non risolti sono tredici e si dividono in due gruppi soli,
il che è già una diagnosi:

| gruppo | prompt | che cosa risponde | perché è sbagliato |
|---|---|---|---|
| **le cifre** | `0`–`9` | «I don't understand that yet.» | dieci muri ciechi. Una cifra non è incomprensibile: è una cifra, e la mossa giusta è chiedere che cosa se ne voglia fare |
| **tre segni** | `-` `.` `/` | «I don't understand that yet.» | e gli **altri ventisette** segni ricevono invece *«That's just punctuation, not words — what would you like to ask?»*. La capacità c'è: tre membri della classe non ci arrivano |

Il secondo gruppo è il più istruttivo. Non manca niente — manca che tre caratteri
entrino in una classe che esiste già ed è dichiarata in KB. È il caso in cui la
stazza sale di tre punti con tre righe di conoscenza, e vale la pena dirlo perché
è esattamente l'effetto che questa misura deve produrre: **non «parrot0 è
scarso», ma «ecco i tre punti dove non arriva, per nome».**

Le cifre sono un lavoro diverso e più interessante: un numero nudo *è* un
contenuto, e la risposta giusta non è la stessa della punteggiatura.
