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
