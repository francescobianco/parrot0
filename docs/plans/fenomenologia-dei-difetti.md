# Fenomenologia dei difetti — dalle riparazioni una alla volta alle specie

> **Richiesta di F., 20 settembre 2026.** «Rendi questo caso del calcolo del
> quadrato di 2.7 una sorta di caso studio per poter creare una fenomenologia
> generica dei problemi e poterli risolvere con un approccio all-in-one-shot al
> posto di one-on-time-when-it-appear.»

Il costo di riparare un difetto quando appare non è la riparazione. È che le
altre istanze della **stessa specie** restano vive e invisibili, e si
ripresentano una per volta per mesi — ognuna sembrando un caso nuovo.

Questo documento fa il contrario: nomina la specie, le dà un **rilevatore**, e
lo passa su tutto. Le specie stanno anche in KB
([`kb/core/fenomenologia.p0`](../../kb/core/fenomenologia.p0)), perché una
specie nominata è una domanda che parrot0 può porsi da solo. Lo strumento è
[`scripts/fenomeni.sh`](../../scripts/fenomeni.sh).

---

## Il caso studio: «2.7 al quadrato»

**Il sintomo.** Alla frase di prosa

```
The reef covers 2.7 million square kilometres.
```

parrot0 rispondeva **`7.29.`** — cioè 2,7². Non un muro, non un «non ho
capito»: un numero, detto con sicurezza, a una frase che non faceva domande.

**La catena, per intero.**

1. La facoltà aritmetica leggeva `square` come l'operazione «al quadrato», non
   come il modificatore di `kilometres`.
2. Il turno era un'**asserzione** (`turn_illocution` diceva `assertion`), e
   l'aritmetica ha risposto lo stesso.
3. Nessuno le aveva mai chiesto *su quali forze ha diritto di parlare*.

**Quello che il caso NON era.** Non era un difetto dell'aritmetica: il suo
conto era giusto. Non era un buco di lessico: `square` e `kilometres` erano
entrambi noti. Non era nemmeno legato ai decimali — «The reef covers **2**
million square kilometres.» rispondeva `4`, e lo faceva da prima. I decimali lo
hanno solo reso **visibile**, perché fino a quel giorno la IR spezzava `2.7` in
`2` e `7` e il conto non partiva.

**Perché era rimasto invisibile.** Il banco di prosa misura le **domande**, e
questa era un'asserzione: il furto avveniva nel passo di lettura, dove nessuno
guardava la risposta. Un difetto che nessuno strumento interroga non è raro:
è muto.

**La riparazione, e la sua forma.** Il cancello esisteva già ed era condiviso:
`faculty_force(Facoltà, Forza)` dichiara su quali forze una facoltà può
parlare, e la forza è **una lettura sola** che migliora per tutte insieme. Una
riga di C generica in `mod_arith` (`p0_move_allowed`), due fatti in KB:

```prolog
faculty_force(arith, question).
faculty_force(arith, directive).
```

Additivo per costruzione: una facoltà senza quei fatti si comporta come prima,
e la condotta si corregge parlando. **Il punto non è la patch: è che la stessa
domanda vale per le altre novanta facoltà**, e nessuno l'aveva mai posta a
tutte insieme.

---

## Le quattro specie, con la loro firma e il loro rilevatore

| specie | firma | rilevatore | stato |
|---|---|---|---|
| **furto di turno** | una facoltà risponde a un turno la cui FORZA non le appartiene | passare un corpus di asserzioni di prosa e raggruppare per facoltà che risponde | `fenomeni.sh furti` |
| **due strade che divergono** | lo stesso oggetto letto per due strade dà due risultati | confrontare i flussi paralleli sullo stesso turno e dichiarare ogni disaccordo | `fenomeni.sh flussi` + sonda `/debug` |
| **distinzione collassata** | due oggetti diversi che la rappresentazione rende uguali | contare gli oggetti distinti e le loro identità: se le identità sono meno, collassano | da scrivere |
| **strumento muto** | uno strumento di misura che fallisce e sembra dire «la conoscenza non c'è» | confrontare il risultato con il tetto dello strumento: uguale al tetto = troncato | da scrivere |

### Perché sono quattro e non una lista di bug

Ognuna è nata da istanze reali **misurate**, non immaginate, e per ognuna la
domanda che la trova è diversa:

- **furto di turno** — il caso 2.7; e prima ancora ogni «furto» del registro
  `docs/labs/prose-ladder/furti.tsv` (406 righe): la colonna del modulo c'era
  già, mancava la domanda «questo modulo aveva diritto?».
- **due strade che divergono** — il motore aveva **tre** regole di confine di
  parola. `turn_span_token` sapeva dal gen399 che «un punto fra due cifre
  appartiene al numero»; `turn_publish_words` e il tokenizzatore della IR in
  `src/code.c` non lo sapevano. Stesso testo, tre segmentazioni. Nella stessa
  sessione: la via rapida del fatto ground esatto saltava la registrazione del
  passo, così `mortal(plato)` aveva una prova diversa a seconda di come era
  stata trovata; e una derivazione conservava il goal in forma canonica ma lo
  richiedeva in forma ordinaria.
- **distinzione collassata** — la revisione prioritaria di M1: `$X` e il
  termine ground `var(0)` erano lo stesso contenuto; due alberi diversi
  diventavano la stessa impronta perché il testo veniva troncato prima
  dell'hash.
- **strumento muto** — `/debug` enumerava al massimo 32 sonde e ce n'erano 37:
  le ultime sparivano in silenzio, e si credeva di aver guardato. La lettura
  di una clausola per identità enumerava 155.000 righe e sforava il budget,
  restituendo zero: un errore dello strumento travestito da assenza. Il demone
  dei test resta in piedi dopo `make build` e dà rossi che sembrano del codice
  nuovo.

### La regola che le genera tutte

Le prime due specie sono le due domande della revisione prioritaria di F.,
girate verso l'esterno:

> **Quali due oggetti diversi la rappresentazione potrebbe rendere uguali?
> E quale stesso oggetto due percorsi potrebbero leggere diversamente?**

Le altre due sono le stesse domande poste agli **strumenti** invece che alla
conoscenza: uno strumento è un oggetto come gli altri, e quando mente lo fa
nello stesso modo — collassando una distinzione, o divergendo da se stesso.

---

## Come si usa

```sh
scripts/fenomeni.sh flussi    # dove due flussi di token divergono
scripts/fenomeni.sh furti     # quale facolta' risponde a un'ASSERZIONE
scripts/fenomeni.sh           # tutte e due
```

Il corpus è `tests/fixtures/prose/ladder/*.txt`: prosa vera, esterna alla KB,
i testi più densi per primi. `P0_FEN_TEXTS` e `P0_FEN_SENT` regolano la taglia.

**Il rilevatore dichiara, non giudica.** Un disaccordo fra flussi può essere
voluto (i flussi hanno scopi diversi); una facoltà che risponde a
un'asserzione può averne diritto (`knowledge` registra, `input` legge). Le
righe si leggono a mano, come i furti del banco. Un rilevatore che giudicasse
da solo sarebbe la quinta specie: uno strumento che decide invece di mostrare.

## La prima passata, misurata

`P0_FEN_TEXTS=5 P0_FEN_SENT=8 scripts/fenomeni.sh furti` — 40 asserzioni di
prosa vera, una sessione per testo:

| facoltà che ha risposto | turni | giudizio |
|---|---|---|
| `knowledge` | 21 | di diritto: registra ciò che ha letto |
| `fallback` | 10 | di diritto: è il muro onesto |
| `negation` | 2 | **da guardare** |
| `compound` | 2 | **da guardare** |
| `acquisition` | 2 | **da guardare** |
| `toolpolicy` | 1 | **da guardare** |
| `gen` | 1 | **da guardare** |
| `analysis_last_resort` | 1 | **da guardare** |

**Sette turni su quaranta** presi da una facoltà che non legge e non registra.
Non sono sette bug dichiarati: sono sette righe da leggere a mano, ognuna con
la sua frase. Ma sono state trovate **in una passata**, non aspettando che un
banco diventasse rosso — ed è tutto il punto del documento.

Il rilevatore dei flussi, sulla stessa prosa, non trova disaccordi: dopo la
riparazione delle tre regole di confine i flussi concordano. Un rilevatore che
tace dopo una cura è un risultato, non un fallimento — purché si sia visto
parlare prima (`Estimates range from US$30-375 billion.` → `at(4, span(30-375),
ir(30))`).

## Che cosa manca

- Leggere le sette righe qui sopra e chiudere **in un colpo** quelle che sono
  furti, con i fatti `faculty_force/2`. Non una facoltà alla volta: la domanda
  «su quali forze ha diritto?» va posta a tutte e novanta.
- I due rilevatori mancanti (distinzione collassata, strumento muto). Il primo
  è scrivibile subito sulle identità di `kb_clause/4`: contare le clausole
  distinte e le identità distinte, e dichiarare ogni collisione.
- Una specie nuova si aggiunge in tre posti: una riga in
  `kb/core/fenomenologia.p0`, un ramo in `scripts/fenomeni.sh`, una riga nella
  tabella delle specie. Se costa più di così, la specie non è stata capita.
---

## Perché l'ispettore non aveva trovato il difetto (F., 20 settembre 2026)

> «Come mai le tecniche di dump della IR e tracking della KB con `/debug` non
> ci permettono di trovare il problema? Riusciamo a fare tesoro di quali
> elementi per il debug sono emersi qui?»

**La risposta, in una riga: l'ispettore mostra uno STATO, e i difetti che
cacciamo sono DIFFERENZE.** `/debug` e `/debug dump` rispondono benissimo a
«che cosa c'è adesso in questo turno»; nessuno dei due risponde a «che cosa è
cambiato», «chi ha perso la gara» e «perché qui e non là». Da qui tre cecità
precise, tutte misurate in questa sessione.

### 1. L'ispettore guarda un turno; il banco misura una sessione

Una domanda del banco non è un turno isolato: arriva **dopo** che sedici frasi
sono state lette, e lo stato che la fa sbagliare è quello. Chi rifaceva la
domanda da sola ispezionava un altro turno e non riproduceva niente — è il
motivo per cui la diagnosi era ferma.

**Il ponte, ora esiste:** [`scripts/prose-why.sh`](../../scripts/prose-why.sh)

```sh
scripts/prose-why.sh r300 "what are shallow coral reefs sometimes called?"
```

Stessa prosa, stessa domanda, ispettore addosso. **Ha trovato il difetto in una
passata:** la lettura aveva registrato
`complement(binding(call, shallow_coral_reefs, rainforests), of, sea)` e la
risposta diceva «Rainforests.». Il complemento era **conoscenza già acquisita**
e la resa lo buttava — una risposta più povera della lettura che la sostiene.
Sulla scala di F. questa è la specie giusta da chiudere: non aggiunge un
frasario, compone ciò che il testo ha già detto.

### 2. L'ispettore nomina il vincitore, non la gara

`turn_module` dice chi ha risposto; `turn_said_by` dice con quale modello di
frase. Nessuno dei due dice **quali altri candidati c'erano e perché hanno
perso**. Per una troncatura — «rainforests of the sea» → «rainforests»,
«water conditions» → «water» — è esattamente il fatto mancante: la risposta
lunga non è stata rifiutata, non è mai stata costruita. `turn_focus_rejected`
copre un solo motivo di scarto (fuori fuoco) e taceva su tutti gli altri.

### 3. L'ispettore non si confronta con se stesso

Il caso «2.7» non si vedeva guardando la IR: la IR era **coerente con sé
stessa**, semplicemente diceva `2` e `7`. Si è visto solo mettendo due flussi
uno accanto all'altro. Da qui la sonda `debug_token_streams`, che non ispeziona
una rappresentazione ma **dichiara dove due rappresentazioni che devono
concordare non concordano**.

## La regola che ne esce

> **Una sonda nuova che mostra uno stato vale poco; una sonda che mostra una
> DIFFERENZA trova i difetti da sola.**

Le sonde differenziali da scrivere, in ordine di resa:

| sonda | differenza che mostra | stato |
|---|---|---|
| `debug_token_streams` | due flussi di token sullo stesso turno | ✅ fatta |
| `prose-why.sh` | il turno del banco contro il turno isolato | ✅ fatto |
| la gara | il vincitore contro i candidati scartati, con il motivo | da scrivere |
| la lettura contro la resa | ciò che è stato letto contro ciò che è stato detto | da scrivere — è quella che ha trovato il complemento buttato, ma a mano |
| due letture dello stesso testo | la IR di ieri contro quella di oggi | da scrivere |

## Quello che gli strumenti hanno anche rivelato di sé

- **`/debug` enumerava 32 sonde e ce n'erano 37.** Le ultime sparivano in
  silenzio: si credeva di aver guardato. Tetto a 96. È la specie «strumento
  muto», trovata mentre se ne aggiungeva una.
- **Due sonde nuove, due righe di KB** (`debug_ir_token`, `debug_ir_node`):
  senza di esse la diagnosi del caso 2.7 non era possibile. Il costo di vedere
  una cosa nuova deve restare una riga.
- **Il dump distingue i fatti dai DERIVATI** (`[derivato]`), ed è così che si è
  capito che `input_node_atom` non è memorizzato ma calcolato. Un dump che
  avesse detto «nessun fatto» avrebbe mentito.
- **Il demone dei test resta in piedi dopo `make build`**: `--test` interroga
  quello vecchio e dà rossi che sembrano del codice nuovo. Costa una diagnosi
  sbagliata a chi non lo sa; `make test-engine` dopo ogni build.

## Il debito misurato, non attribuito

`r300` è **45/62**, contro il 48/62 del referto del 19 settembre sera. Le
cinque risposte perse sono tutte **troncature di sintagma**. Tre sospetti sono
stati **esclusi con una misura ciascuno**, non per ragionamento:

| sospetto | prova | esito |
|---|---|---|
| unione delle origini (`kb_assert`) | disabilitata, banco rilanciato | 45/62 — **non è lei** |
| le tre KB nuove (clause-content, derivation, fenomenologia) | includes esclusi, banco rilanciato | 45/62 — **non sono loro** |
| il `Solver` cresciuto di 4 KB sulla pila C (tetto gen514) | pila dei passi spostata sullo heap | 45/62 — **non è lui** |

Lo spostamento sullo heap resta comunque: un `Solver` finisce sulla pila a ogni
negazione, e quella pila è il terzo cancello della ricerca. **Il −3 resta non
attribuito**, e la prossima sessione ha ora `prose-why.sh` per guardarlo turno
per turno invece che banco per banco.

---

## Il coefficiente, e perché il numero non sale: misurato, non argomentato

La scala di F. non conta le domande che passano: misura **da che cosa vengono
le risposte**. Questa sessione ha trovato, con il ponte
[`prose-why.sh`](../../scripts/prose-why.sh), il fatto che spiega il tetto.

**L'esperimento.** Domanda del banco r300: *«what are shallow coral reefs
sometimes called?»*. La risposta è «Rainforests.», e il testo dice
«rainforests of the sea».

L'ispettore, sul turno **nello stato in cui sbaglia**, mostra due cose insieme:

```text
debug_complement   complement(binding(call, shallow_coral_reefs, rainforests), of, sea)
modulo             answerframe
```

Cioè: **la lettura ha letto il complemento, e chi risponde non è la lettura.**

**La prova che non è un dettaglio di resa.** Ho scritto la composizione mancante
come regola KB su `ir_reading_answer` — la faccia che rende una lettura in
risposta — con la guardia resa ground perché `naf` declina sui goal non ground.
La regola è corretta e **non cambia niente**: la risposta continua a essere
«Rainforests.», perché `answerframe` prende il turno prima e non guarda la IR.
La regola è stata **ritirata**: una regola che non sposta la misura non resta
in albero.

**Perché questo è IL fatto del coefficiente.** Il piano lo aveva già scritto il
18 settembre — *«il 49/50 del piolo 300 viene tutto da `answerframe` sopra
fatti estratti da schemi e ritrovati per cue: un frasario ben fornito, non una
lettura»* — ma come giudizio. Ora è una misura riproducibile in un comando, su
una domanda nominata, con la conoscenza mancante visibile accanto alla risposta
povera che la ignora.

**Ne segue l'ordine del lavoro, e non è «insegnare parole».** Finché
`answerframe` vince, ogni riga di lessico in più alza il conteggio e lascia il
coefficiente dov'è. Quello che lo alza è spostare il turno sulla lettura:

1. `answerframe` deve **cedere** quando la IR del turno ha una lettura completa
   per quella domanda. La condotta esiste già ed è KB: `faculty_yield/3` — la
   stessa porta con cui l'aritmetica ha smesso di rispondere alle asserzioni.
2. Solo allora la composizione del complemento (scritta e ritirata qui) sposta
   una risposta, e si potrà misurarla.
3. La prova che il passo è avvenuto **non** è il numero del banco: è che la
   riga risponda con `modulo` di lettura invece che `answerframe`. Il banco
   mostra già la colonna del modulo: il coefficiente si può leggere **da lì**,
   contando quante risposte giuste vengono da una lettura e quante da una cue.

### Il numero, misurato per la prima volta

[`scripts/coefficiente.sh`](../../scripts/coefficiente.sh) conta, sui referti
del banco, quante risposte **giuste** vengono da un circuito di lettura e
quante dal frasario:

| piolo | lettura | frasario | coefficiente |
|---|---|---|---|
| r300 | 10 | 42 | **19%** |
| r340 | 11 | 4 | 73% |

**r300 dà 19%, e la stima di F. era «20».** La misura e il giudizio coincidono:
è la prova che la colonna del modulo è la scala, e che da oggi il coefficiente
si legge invece di stimarlo. (r340 ha un rapporto alto perché il frasario lì
quasi non aggancia: su 65 domande ne passano 15. Il coefficiente va letto
accanto al conteggio, non al posto suo.)

La lista dei moduli «di lettura» sta nello script e non in KB **apposta**: è un
giudizio sul progetto, non conoscenza di parrot0, e va discussa a mano quando
un modulo cambia natura.

**Una misura per il coefficiente, non più un giudizio.** È la cosa che questa
sessione lascia di più utile sulla scala di F.: r300 dà 45 risposte nel merito,
e la colonna dei moduli dice da dove vengono. Contarle è un `awk`, e diventa il
numero che sostituisce la stima «12–15».

### Il tetto, misurato: non è chi risponde, è quanto viene letto

Tre tentativi di alzare il coefficiente, tre misure, tutte nulle. Vale la pena
riportarli perché la conclusione vale più di un guadagno:

| tentativo | ipotesi | misura |
|---|---|---|
| composizione del complemento su `ir_reading_answer` | la resa butta ciò che la lettura ha letto | r300 invariato: `answerframe` prende il turno prima — **regola ritirata** |
| `faculty_yield_when(answer_frame, …, ir_reading_answer)` | il frasario ruba il turno a una lettura disponibile | 45/62, 19% — **identico** |
| renderer della classe (`ir_membership_answer`) | una lettura di `membership` esisteva senza voce | 45/62, 19% — **identico**, ma la voce ora c'è e si vede rispondere |

**Perché nessuna sposta il numero.** Contate le frasi del piolo r300, una per
una, con l'ispettore addosso:

```text
debug_frame_record — niente  →  12 frasi su 16
```

**Tre quarti del testo non vengono letti in nessuna struttura.** Il frasario
non sta rubando il turno a una lettura disponibile: per quelle domande la
lettura *non esiste*. Far cedere `answerframe` prima che la copertura salga
toglierebbe risposte senza darne — ed è esattamente quello che la misura dice.

**Ne segue l'ordine vero del lavoro, e non è quello che sembrava.** Il passo
che alza il coefficiente non è la condotta (chi risponde) né il lessico (quante
parole si conoscono): è la **copertura della lettura** — quante frasi di prosa
reale diventano frame. Da 4 su 16 in su, ogni frase letta in più è una domanda
che può essere risposta da una lettura invece che da una cue.

Restano in albero, perché chiudono buchi reali e si sono viste funzionare:
`faculty_yield_when/3` (la condotta può ora dipendere da una lettura e non solo
da una cue di superficie — la porta è pronta per quando la copertura sale) e
`ir_membership_answer/2` (una lettura che esisteva senza voce). La politica che
le userebbe è stata scritta, misurata e **tolta**: una regola che non sposta la
misura non resta in albero, e il commento in `kb/core/intents.p0` dice perché.
