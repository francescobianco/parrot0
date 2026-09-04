# Connecting the dots — *split and cross*, o come si moltiplicano gli archi

> **gen502, su indicazione di F.** Un metodo per costruire la conoscenza che
> risponde alla **connessione fra le isole**, invece di aggiungere isole. Non una
> matrice forgiata a monte: un procedimento **progressivo**, che a ogni
> iterazione raddoppia la granularità e produce nuovi archi — e che porta con sé
> **un indice**, così l'espansione è certificata invece che dichiarata.
>
> Deve essere possibile incrociare **scacchi e informatica**, **matematica e
> chimica**, **i pronomi personali inglesi e gli animali**. Tutto con tutto.

---

## 1. L'origine — un caso vero, di oggi

Il 2026-09-04 il banco di gara diceva che parrot0 non sa fare un compito di
coding: 5/100 su difficoltà 1, 5/100 su difficoltà 4, cartella intatta. Ho
passato una giornata a inseguire chi rubava il turno — dispatch, finestre di
buffer, meccanica di strumenti. Ogni difetto trovato era reale, e **nessuno era
la ragione.**

Poi, cercando altro:

```text
> make a plan to fix the repository
  My derived plan for code task: 1) inspect the workspace and collect grounded
  repository evidence [cost 1] [needs …]
```

In `kb/experts/codebase/actions.p0` **c'era già la spina dorsale di un coding
agent** — `inspect_workspace → localize_change → edit_candidate →
verify_candidate`, con rischio, costo e ordine derivato. Era lì da prima che
cominciassi.

E dall'altra parte, misurato lo stesso giorno: parrot0 sa **elencare** (`run
ls`), **leggere in struttura** (`read main.c`), **rispondere con le prove**
(span, digest, provenienza) ed **eseguire il build leggendone il verdetto**
(`run make` → *«No rule to make target 'strjoin.c'»*).

⛔ **Le due isole non si toccavano.** Il piano non sapeva che gli strumenti
esistevano (`action_impl` = 0 su tutte e quattro le azioni); gli strumenti non
sapevano che esisteva un piano. Nessuna delle due parti era rotta, mancava
**l'arco** — e un arco mancante non somiglia a un guasto: somiglia a due cose che
funzionano.

> **La conoscenza c'era. Il coding non se n'era accorto.**

Questo documento generalizza quel caso in un metodo.

---

## 2. La tesi

Una KB cresce per **isole**: un esperto di algoritmi, uno di chimica, la
morfologia, il discorso, il dominio dei piani, l'IR del codice. Ogni isola,
presa da sola, è verificabile e cresce bene. **Il valore però non sta nelle
isole: sta negli archi fra loro** — perché un'inferenza interessante quasi
sempre attraversa un confine.

`THINKING_TODO` H11 lo dice già come **Leva 1** — *«ponti prima di nuove
isole»* — e misura la cosa giusta (`cold tasks unlocked / new curated clauses`
deve crescere più di uno-a-uno). Ma **non dà un metodo**: dice che i ponti
valgono di più, non *quali* costruire, in che ordine, né come sapere quanto
manca.

⭐ **`split and cross` è quel metodo**, e la sua proprietà principale è che
produce **un numero**: a ogni iterazione si sa quanti archi sono possibili,
quanti sono certificati, e quindi quanto della propria conoscenza parrot0 sta
davvero *usando insieme*.

---

## 3. Il metodo

### 3.0 Passo zero — la conoscenza è una

Si parte da un insieme unico: tutto ciò che parrot0 sa. Nessun arco, nessun
indice. È lo stato di riferimento.

### 3.1 Passo 1 — la bipartizione, e il primo cross

Si divide la conoscenza in **due supergruppi**, con due vincoli non negoziabili:

1. **completi**: la loro unione è tutta la conoscenza — niente resta fuori;
2. **disgiunti**: nessuna clausola sta in entrambi.

Poi si costruisce **almeno un cross**: un'inferenza che, per produrre il suo
risultato, **deve passare per entrambe le regioni**. Non due fatti che si
citano: una derivazione che *cade* se una delle due sparisce (§4).

```text
   ┌───────────── A ─────────────┐   ┌───────────── B ─────────────┐
   │                             │ ✕ │                             │
   └─────────────────────────────┘   └─────────────────────────────┘
   regioni: 2      cross possibili: 1      nuovi a questo passo: 1
```

### 3.2 Passo 2 — si divide **uno** dei due

`A` si divide in `A1` e `A2`. I cross da costruire sono quelli dei due nuovi
contro l'altro grosso — `A1 ✕ B`, `A2 ✕ B` — più il fratello `A1 ✕ A2`.

```text
   ┌──── A1 ────┐┌──── A2 ────┐   ┌───────────── B ─────────────┐
   regioni: 3      cross possibili: 3      nuovi a questo passo: 2
```

### 3.3 Passo 3 — tocca all'altro

`B` si divide in `B1` e `B2`, e si incrociano contro `A1` e `A2` e fra loro.

```text
   ┌── A1 ──┐┌── A2 ──┐   ┌── B1 ──┐┌── B2 ──┐
   regioni: 4      cross possibili: 6      nuovi a questo passo: 3
```

### 3.4 E così via — **alternando**, con il registro

Ogni iterazione sceglie una regione e la divide a metà. **L'alternanza non è
estetica**: dividere sempre lo stesso lato produrrebbe un albero sbilanciato in
cui una metà della conoscenza resta un blocco opaco, e i suoi archi non si
raffinano mai.

Il **registro della suddivisione** è parte del piano, non un sottoprodotto: dice
in ogni momento qual è l'albero delle regioni, chi è stato diviso e quando,
quali cross esistono e quali sono deliberatamente non costruiti.

---

## 4. Che cos'è un **cross**, in forma certificabile

Questo è il punto in cui il metodo o diventa rigoroso o diventa fuffa.

> **Definizione.** Esiste un cross fra le regioni `R` e `S` quando esiste una
> domanda la cui derivazione **tocca clausole di entrambe**, e che **non regge**
> se una delle due regioni viene ablata.

Due condizioni, e servono entrambe:

| | condizione | perché |
|---|---|---|
| **C1** | l'impronta della derivazione contiene predicati di `R` **e** di `S` | senza questo il cross è una coincidenza lessicale |
| **C2** | ablando `R` (o `S`) la risposta **cade o degrada** | senza questo una delle due regioni era decorativa |

⭐ **E il meccanismo per verificarlo esiste già.** `kb_footprint_reset()` azzera
a ogni turno, `kb_footprint_width()` dice quanti predicati distinti sono stati
toccati e `kb_footprint_pred(i)` li nomina, uno per volta. **C1 è misurabile
oggi**, senza scrivere niente: basta chiedere a quale regione appartiene ogni
predicato dell'impronta. E C2 è l'ablazione, che è già la regola di chiusura di
ogni voce di questo repository — *«una voce si chiude quando un cricchetto la
tiene ferma e un'ablazione la fa cadere»*.

⛔ **Che cosa NON è un cross:**

- un fatto che nomina parole di due regioni (`related(chess, computer_science)`)
  — è un'asserzione sulla connessione, non una connessione;
- una regola che sta in `R` e cita un predicato di `S` senza che nessuna domanda
  la percorra — è un arco disegnato, non attraversato;
- un cross che regge anche togliendo una delle due regioni — allora l'inferenza
  passava da un'altra parte, e il cross è un'illusione di provenienza.

---

## 5. La misura, e perché è la parte che serve davvero

Sia **n** il numero di regioni foglia (il **grado di frammentazione**).

| | | |
|---|---|---|
| **cross possibili** | tutte le coppie | `n(n−1)/2` |
| **nuovi a questo passo** | dividendo una regione si arriva a `n` | `n−1` |
| **cross certificati** | quelli che passano C1 **e** C2 | misurati |
| ⭐ **indice** | | **certificati / possibili** |

```text
   n      possibili      nuovi
   2              1          1
   3              3          2
   4              6          3
   5             10          4
   8             28          7
  16            120         15
  32            496         31
 110          5 995        109
```

### 5.1 ⚠ A scala i cross non si potranno costruire tutti — ed è il punto

Con 110 regioni servirebbero **5 995** cross. Non si faranno mai tutti, e non è
un difetto del metodo: **è la ragione per cui serve l'indice.** Un numero che si
può solo far salire è una vanità; questo si può far **scendere**, e questa è la
proprietà che lo rende onesto:

> **Dividere una regione senza costruire i cross nuovi ABBASSA l'indice.**

Cioè: raffinare la conoscenza senza connetterla è registrato come un
peggioramento. È esattamente l'incentivo giusto, ed è l'opposto di quello che una
KB che cresce per isole produce da sola.

### 5.2 Le tre letture dell'indice

| lettura | domanda a cui risponde |
|---|---|
| **globale** | quanto della mia conoscenza so usare insieme? |
| **per regione** | quale isola è più sola? (poche coppie certificate sulle sue) |
| **per coppia** | questo confine è attraversabile, o l'ho solo disegnato? |

La seconda è quella operativa: **l'isola più sola è il prossimo lavoro**, e la
sceglie il numero invece dell'intuizione.

---

## 6. Gli incroci che devono essere possibili

F. li ha chiesti per nome, e sono il collaudo del metodo: se questi non passano,
il metodo è una tassonomia e non una capacità.

### 6.1 Scacchi ✕ Informatica

> *«Il giro del cavallo su una scacchiera 8×8 è un problema difficile?»*

Il cross passa da `chess` (il cavallo, la scacchiera, la mossa a L) a
`computer_science` (cammino hamiltoniano, classe di complessità). Nessuna delle
due isole risponde da sola: la prima non conosce la complessità, la seconda non
sa che cos'è un cavallo. **Ablando l'una o l'altra, la risposta cade.**

### 6.2 Matematica ✕ Chimica

> *«Bilancia questa reazione.»*

Il cross passa da `chemistry` (conservazione degli atomi, formule) a `algebra`
(sistema lineare a coefficienti interi). La chimica pone il vincolo, la
matematica lo risolve. È il caso più istruttivo perché **l'arco è una
traduzione**: una reazione *è* un sistema lineare, e riconoscerlo è il cross.

### 6.3 Pronomi personali inglesi ✕ Animali

> *«Devo chiamare il cane "it" o "he"?»*

Il cross passa da `grammar` (i pronomi di terza persona, il genere) a `animals`
(animatezza, se l'animale è un compagno con un nome). **È il più piccolo dei
tre**, e per questo il migliore per cominciare: due isole che nessuno avrebbe
mai pensato di collegare, e una domanda che chiunque pone.

### 6.4 ⭐ E il caso di casa — Piani ✕ Strumenti

Quello del §1, che è il motivo per cui questo documento esiste. La regione dei
piani conosce `code_task`; la regione degli strumenti conosce `list`, `read`,
`run`, `write`. **Il cross è `action_impl/2`**: il fatto che lega un passo del
piano alla primitiva che lo realizza. Ce ne sono 9 su 28 azioni, e **zero sulle
quattro di `code_task`.**

Questo è un cross **misurato mancante**, non ipotizzato — ed è la prova che il
metodo trova cose vere: applicato ieri, avrebbe indicato quel confine come il
più povero del sistema, e avrebbe risparmiato una giornata di caccia al dispatch.

---

## 7. La prima bipartizione di parrot0

Il passo 1 chiede due supergruppi completi e disgiunti. La proposta, che va
discussa prima di essere eseguita:

```text
A — PRESENTAZIONE      come si dice, si riconosce, si dispone una risposta
                       forme, intenti, morfologia, discorso, registro, messaggi
B — SOSTANZA           che cosa è vero, e come si deriva
                       domini, esperti, IR del codice, piani, procedure
```

⚠ Non è una scelta neutra, ed è deliberata: è la stessa linea che F. ha già
tracciato come direzione (`substance ⟂ presentation`), quindi la bipartizione
**mette alla prova una tesi che già esiste** invece di inventarne una nuova. Se
la frontiera è quella giusta, il primo cross sarà facile da trovare e difficile
da ablare; se non lo è, si vedrà subito — ed è informazione.

Il primo cross candidato: **una risposta la cui *forma* dipende da un fatto di
dominio** (per esempio: dire un numero con la sua unità solo quando il dominio
dichiara che l'unità è obbligatoria). Tocca A e B, e cade se si toglie uno dei
due.

---

## 8. Il registro, come conoscenza

Il registro non è un allegato del documento: è **conoscenza in KB**, perché
altrimenti l'indice non è interrogabile e il metodo torna a essere una promessa.

```prolog
kb_region(Regione, Genitore).        % l'albero delle suddivisioni
region_holds(Regione, Predicato).    % quali predicati vivono in una regione
region_split(Genitore, Figlio1, Figlio2, Iterazione).
cross(RegioneA, RegioneB).           % dichiarato: questo confine ci interessa
cross_status(RegioneA, RegioneB, certified | attempted | deferred | refuted).
cross_witness(RegioneA, RegioneB, "la domanda che lo attraversa").
cross_ablation(RegioneA, RegioneB, "che cosa cade togliendo A").
```

`cross_status(_, _, deferred)` è **obbligatorio e non è una scusa**: a scala i
cross non si fanno tutti, e un confine deliberatamente non costruito va
*dichiarato*, non dimenticato. La differenza fra un piano onesto e un backlog è
tutta lì.

---

## 9. Gate anti-impostore

1. **Un cross senza ablazione non è un cross.** C1 senza C2 è una coincidenza.
2. **Nessuna matrice a monte.** Le regioni si dividono una per iterazione, e i
   cross si costruiscono dopo la divisione. Generare `n(n−1)/2` righe vuote e
   chiamarle archi è il camuffamento del mantra #18(b) — *un template vuoto non è
   una resa*.
3. **L'indice deve poter scendere.** Se una divisione non abbassa l'indice
   finché i cross nuovi non esistono, il numero è truccato.
4. **Le regioni sono complete e disgiunte a ogni passo.** Una clausola che non
   sta in nessuna regione è un buco che l'indice non vede — e quindi mente.
5. **Un cross si prova su una domanda che nessuno ha usato per costruirlo.**
   Altrimenti si è insegnato quel caso, non l'arco.
6. **Il metodo si applica a se stesso.** La regione «piani» e la regione
   «strumenti» sono il primo banco: se `split and cross` non avesse trovato
   `action_impl` mancante, non serve.

---

## 10. Ordine di lavoro

| | | gate |
|---|---|---|
| **S0** | il registro come KB (`kb_region`, `cross`, `cross_status`) e l'indice calcolato dall'impronta | l'indice si stampa e vale **0** |
| **S1** | bipartizione presentazione/sostanza + il primo cross certificato | 1 cross su 1 possibile, e l'ablazione lo fa cadere |
| **S2** | dividere la sostanza; i due cross nuovi | indice su 3 possibili, e **scende** prima di risalire |
| **S3** | dividere la presentazione; i tre cross nuovi | indice su 6 |
| **S4** | i tre incroci nominati da F. (scacchi✕informatica, matematica✕chimica, pronomi✕animali) come **prova di trasferibilità**, non come membri | ognuno passa C1+C2 su una domanda mai usata per costruirlo |
| **S5** | il caso di casa: `action_impl` per le quattro azioni di `code_task` | il piano si cammina fino in fondo su match0 |

---

## 11. Stato di partenza, misurato

| | |
|---|---|
| file di conoscenza | **110** (58 in `kb/core`, 52 in `kb/experts`) |
| ponti a predicato variabile vivi (`representation_bridge/4`) | **10** |
| cross possibili se ogni file fosse una regione | **5 995** |
| indice, con quella lettura grezza | **≈ 0,17 %** |

⚠ Il numero va letto per quello che è: `representation_bridge` non è l'unica
forma di arco, e «un file = una regione» non è la partizione che il metodo
prescrive. **È un fondo scala, non una diagnosi** — serve a dire che il margine
è quasi tutto, non a giudicare quello che c'è.

---

## 12. Che cosa questo metodo non è

- **Non è un'ontologia.** Non impone una tassonomia del sapere: la partizione è
  strumentale e si rifà a ogni iterazione.
- **Non è un grafo di similarità.** Due regioni possono essere lontanissime e
  avere un cross fortissimo — pronomi e animali.
- **Non è completo, e lo dichiara.** A scala i cross si scelgono; il metodo
  serve a scegliere *sapendo che cosa si sta rinunciando a fare*.
- **Non sostituisce la crescita delle isole.** Dice solo che, a parità di
  sforzo, un arco vale più di un'isola — e ora c'è un numero che lo dimostra
  invece di una convinzione.
