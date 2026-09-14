# Profondità del ragionamento simbolico — il banco D×W

> Piano vivo. Il testo originale di F. (obiettivo, catene, join, distrattori, rami morti,
> metriche) è tutto qui, riordinato in §3–§5; le sezioni nuove dicono **come** si misura
> senza aprire un ramo di ragionamento a parte (§2), che cosa si è misurato (§0, §7) e che
> cosa ne segue (§8).

## Indice

- §0 Stato misurato
- §1 Obiettivo
- §2 Il vincolo: comprensione universale e IR, non un ramo nuovo
- §3 Regole anti-inganno
- §4 La struttura dei problemi (profondità, larghezza, join, distrattori, rami morti)
- §5 Metriche
- §6 I limiti del motore, detti come ipotesi
- §7 Esperimenti
- §8 Che cosa ne segue: le mosse, in ordine
- §9 Registro

---

## 0. Stato misurato (14 settembre 2026)

Prima campagna: 47 problemi, un problema per cella, KB intera, superficie S1 (§7.1).
Risultati in `tests/symbolic-depth/results-2026-09-14.jsonl` (44 righe; D1–D3 della prima
prova, seme 14092026, sono nel registro).

| Che cosa | Esito |
|---|---|
| **Il ragionatore** | catene lineari risolte **fino a D62** (aperta fino a D63); da D64 «Nobody that I know of» / «I don't know» — il muro è `KB_MAX_DEPTH` |
| **Larghezza con distrattori** (`lin`) | tutto risolto fino a **D10-W10** (172 turni), nessun falso |
| **Join** | tutto risolto fino a D10-W3 |
| **Spazio di ricerca esponenziale** (`dag`, W^(D-1) cammini) | risolto fino a D16-W3 (14 milioni di cammini) **ma** da D14-W3 la ricerca finisce il budget (500.000 passi) e **non lo dice** |
| **Misclaim** | 0 su «Yes» falsi; **1 lista incompleta detta come intera** (D14-W3 con 3 radici: «viklogon.», due risposte vere perse) |
| **Le superfici** | **1 su 12** arriva al ragionamento (§7.1): notazione formale, clausole P0, testo unico, «whenever», condizione posposta, polare italiana non si leggono |
| **Stati intermedi** | non dicibili: «why?» dopo un «Yes» derivato → «I don't have a proof to share» |
| **RE** | non osservabile: la dimostrazione è sotto il rumore del turno (§5.3), salvo quando satura il budget |

**In una riga**: il solver è profondo e largo più di quanto il piano temesse; parrot0 no,
perché **la classe entra da una frase sola, e quando la ricerca si ferma parrot0 non lo sa
dire**. Le prime mosse (§8) non sono di ragionamento: sono l'onestà del taglio, la
dimostrazione come oggetto, e l'ingresso universale.

---

## 1. Obiettivo

Misurare **quanto in profondità parrot0 ragiona** su problemi simbolici puri, togliendo il
più possibile ciò che non è ragionamento:

- conoscenza del mondo (nessun fatto della KB può rispondere al posto della catena);
- ambiguità lessicale (relazioni ed entità sono parole senza senso);
- Wikipedia e la memoria profonda.

La domanda non è se parrot0 comprende un testo difficile, ma se **costruisce e mantiene una
catena inferenziale profonda** fino alla risposta, e se la **trova** dentro uno spazio
simbolico più grande della catena.

Due dimensioni, controllate separatamente:

- **depth (D)**: il numero minimo di inferenze necessarie per arrivare alla soluzione;
- **width (W)**: il numero di alternative, rami, fatti e percorsi plausibili da esplorare.

Il traguardo è **D10-W10**: dieci inferenze minime, più percorsi plausibili, rami morti,
fatti irrilevanti, join intermedi, nessuna indicazione del percorso. Lì non si verifica più
la propagazione logica, ma

```text
SEARCH + REASONING + MEMORY OF INTERMEDIATE STATES + GOAL-DIRECTED CONTROL
```

---

## 2. Il vincolo: comprensione universale e IR, non un ramo nuovo

F., 14 settembre: *«anche questi dovranno essere appoggiati dalla comprensione universale e
dalla IR generale; anche se non è un problema di grammatica poco cambia: il motore di
comprensione universale deve saper gestire logica simbolica, testo, codice in maniera
universale. Non si è legittimati ad aprire un nuovo ramo di ragionamento, ma a estendere
l'esistente e verificarlo con questa classe di problemi.»*

Che cosa vuol dire, in pratica:

1. **Il problema è un testo.** `a(A,B)`, «tavo is the dax of brin», «se x è il dax di y…»
   e `fep(X,Z) :- dax(X,Y), wug(Y,Z).` sono **superfici** della stessa cosa. Entrano dallo
   stesso ingresso del turno e devono diventare la stessa rappresentazione: clausole e
   fatti che il solver già usa. Un lettore di notazione simbolica che scrivesse clausole
   per conto suo sarebbe il lettore per posizioni di token dell'esperimento età e incontri
   (MANTRA #24): funziona sul banco e non insegna niente al resto di parrot0.
2. **Il ragionatore c'è già e resta uno solo.** Il risolutore della KB fa join e ricorsione
   (`kb.c`); le regole insegnate parlando sono clausole come quelle dei file `.p0`. Il banco
   **misura quel solver attraverso la conversazione**, non un motore di ricerca scritto per
   il banco.
3. **Il banco separa i livelli**, perché un fallimento dice cose diverse a seconda di dove
   avviene:
   - **lettura**: la lezione diventa una clausola? (esito `READ` se no);
   - **ragionamento**: la clausola giusta è derivata alla profondità chiesta?
   - **controllo**: con W alto la trova senza perdersi, e senza dire il falso?
   - **resa**: la risposta, e **gli stati intermedi dicibili** («perché?»).
4. **Un'estensione si giustifica sulla classe.** Se la notazione formale non si legge, la
   cura è far leggere alla comprensione universale la notazione come legge la prosa e il
   codice; la prova è che le **altre superfici** della stessa classe (§7.1) passano senza
   toccare altro.

**Debito dichiarato (MANTRA #24).** Il lettore di lezioni condizionali di oggi
(`mod_teach_rule`, `src/brain/10-memory-knowledge.c`) taglia le parole della frase ai
marcatori dichiarati in KB e non passa per la IR del turno (`input_semantic_frame`). È la
strada che il banco usa perché è quella che c'è; la sua migrazione nella IR è una mossa di
§8 e si misura con lo stesso banco.

---

## 3. Regole anti-inganno

1. **Parole senza senso, verificate.** Relazioni ed entità sono generate da sillabe e
   scartate se compaiono in qualunque `.p0` della KB (`gen.py`, `kb_words`), o se finiscono
   come una flessione (`-s`, `-ed`, `-er`…), o se sono una lettera-variabile (`x`, `y`, `z`)
   o una forma SMS (`r` → `are`, misurato in §7.1).
2. **KB intera.** Profilo `agi`, nessuna amputazione: il banco misura parrot0 com'è.
3. **Banco prima della cura, seme fisso.** I problemi si generano con un seme dichiarato e si
   misurano prima di toccare il motore; una cura si prova su semi **nuovi** e su superfici
   diverse (MANTRA #24).
4. **Niente dentro al banco.** Nessun predicato, nessuna parola, nessun ramo del C che
   conosca `lin`, `join`, «dax» o la forma delle domande del banco.
5. **Un «Yes» falso è peggio di dieci muri** (MANTRA #7): la polare falsa è sempre nel
   problema, e un `MISCLAIM` vale come fallimento della cella intera.
6. **Le lezioni del banco non si salvano** (`PARROT0_SESSION=` vuoto): sono input della
   misura, non conoscenza.

---

## 4. La struttura dei problemi

Il testo originale usava una notazione simbolica. Qui resta come **forma astratta**;
come la si dice a parrot0 è §7.1 (le superfici). In `gen.py` la famiglia `lin` produce le
catene di §4.1–4.3 con i distrattori di §4.6–4.7, la famiglia `join` il caso di §4.5, la famiglia
`dag` uno spazio di ricerca esponenziale con una sola risposta (§7.2); le famiglie di
§4.8–4.9 sono da scrivere (§8).

### 4.1 Caso minimale: inferenza transitiva

```text
A > B
B > C
C > D
? A > D        → TRUE
```

Misura pochissimo, perché il sistema applica sempre la stessa relazione. È utile solo come
**baseline**. In parrot0 esiste già, in due forme: la transitività del comparativo
(`tests/p0t/reasoning/transitivity.p0t`) e `order_path` fino a 3 salti
(`kb/core/order-determinacy.p0`).

### 4.2 Catena inferenziale esplicita

Ogni inferenza produce un fatto nuovo, necessario per il passo successivo:

```text
FACTS:  p(A,B)  q(B,C)  r(C,D)
RULES:  p(X,Y) & q(Y,Z) -> s(X,Z)
        s(X,Y) & r(Y,Z) -> t(X,Z)
QUERY:  t(A,D)
```

```text
p(A,B) + q(B,C)  →  s(A,C)
s(A,C) + r(C,D)  →  t(A,D)
```

Il risultato intermedio `s(A,C)` non è fra i fatti iniziali: va inferito e poi riusato.
È già un vero multi-hop.

### 4.3 Profondità arbitraria

```text
FACTS:  a(A,B) b(B,C) c(C,D) d(D,E) e(E,F)
RULES:  a(X,Y) & b(Y,Z)    -> ab(X,Z)
        ab(X,Y) & c(Y,Z)   -> abc(X,Z)
        abc(X,Y) & d(Y,Z)  -> abcd(X,Z)
        abcd(X,Y) & e(Y,Z) -> final(X,Z)
QUERY:  final(A,F)
```

Genera catene di profondità D1, D3, D5, D10, D20, D50. **Convenzione del banco**: D è il
numero di applicazioni di regola (D1 = una regola, due fatti). Una catena lineare misura
quasi solo la **propagazione**: non misura ancora la capacità di **cercare** un percorso.

### 4.4 Soluzione nascosta

Il benchmark interessante ha molti fatti, molte regole, percorsi irrilevanti, rami morti,
più inferenze locali possibili e **una sola catena utile**. Il sistema non deve eseguire una
catena: la deve **trovare**.

```text
follow a reasoning path   ≠   discover the reasoning path
```

### 4.5 Parallelismo e join

```text
FACTS:  a(A,B) b(B,C) c(C,D)       x(A,E) y(E,F) z(F,G)
RULES:  a(X,Y) & b(Y,Z) -> p(X,Z)   p(X,Y) & c(Y,Z) -> q(X,Z)
        x(X,Y) & y(Y,Z) -> r(X,Z)   r(X,Y) & z(Y,Z) -> s(X,Z)
        q(X,Y) & s(X,Z) -> final(Y,Z)
QUERY:  final(D,G)
```

```text
A─a→B─b→C─c→D
│
└─x→E─y→F─z→G
```

Primo ramo `p(A,C)`, `q(A,D)`; secondo ramo `r(A,F)`, `s(A,G)`; join
`q(A,D) + s(A,G) → final(D,G)`. Tre capacità: **branch, reason independently, join**. I
risultati intermedi di rami distinti vanno tenuti insieme.

### 4.6 Distrattori

Fatti e regole che sembrano utili e non portano alla soluzione:

```text
a(A,B) b(B,C) c(C,D)     a(A,X) b(X,Y) c(Y,Z)     m(A,Q) n(Q,R)
```

Una sola catena soddisfa la query. Si osserva se parrot0 esplora indiscriminatamente, pota,
riconosce i percorsi promettenti, evita i cicli e le inferenze irrilevanti.

In `gen.py` (`lin`, W>1), per ogni posizione della catena: **predecessori morti** (entrano
nella catena con la relazione giusta, ma nessuno li raggiunge), **successori morti** (escono
dalla catena e proseguono 1–3 passi sulle relazioni giuste, poi si fermano) e **regole
valide ma inutili** sulle stesse relazioni.

### 4.7 Dead ends

```text
A → B → C → D
      \
       → X → Y → Z
```

`X → Y → Z` è logicamente corretto e non porta alla query: distingue **valid reasoning** da
**goal-directed reasoning**.

### 4.8 Branching e pruning (da scrivere)

```text
A ├── B ├── C          Solo una combinazione porta alla soluzione:
  │     └── D          generate hypotheses → evaluate them →
  ├── E ├── F          discard some paths → preserve others
  │     └── G
  └── H ├── I
        └── J
```

### 4.9 Join multipli (da scrivere)

```text
branch A ─────┐
              ├─ join1 ───┐                A ──→ B ──→ C
branch B ─────┘           ├─ final          \         /
branch C ─────────────────┘                  → D → E
                                                 |
                                                 → F → G
```

La query vuole risultati da più sottoproblemi: non si risolve seguendo una catena sola.

### 4.10 Progressione

| Tappa | Struttura | Famiglia |
|---|---|---|
| D1 | inferenza semplice | `lin` W1 |
| D3 | catena lineare | `lin` W1 |
| D5 | catena + distrattori | `lin` W3 |
| D7 | biforcazione + join | `join` |
| D10 | più rami + join intermedi | `join` W3, poi §4.9 |
| D15 | percorsi morti | `lin` W10 |
| D20 | soluzione profonda nascosta | §4.8 + §4.9 |

Non si aumenta il numero di regole: si aumenta **in modo controllato la struttura**.

### 4.11 Proprietà da osservare

Inference Depth · Search Width · Branching · Join Handling · Intermediate Fact Retention ·
Dead-End Detection · Pruning · Cycle Avoidance · Goal Direction · Rule Selection.

---

## 5. Metriche

### 5.1 Esito di un problema

Ogni problema ha tre domande: la **aperta** («who is the R of N»), la **polare vera** e la
**polare falsa** (un nome che arriva vicino alla fine senza la catena).

| Esito | Quando |
|---|---|
| `READ` | almeno una lezione non è tornata «Learned…»: misura l'ingresso, non il ragionamento |
| `SOLVED` | aperta col nome giusto e nessun nome sbagliato, polare vera «Yes», polare falsa non «Yes» |
| `UNSOLVED` | nessun falso, ma almeno una risposta manca |
| `MISCLAIM` | un nome sbagliato nell'aperta o un «Yes» alla polare falsa |

### 5.2 SR e MSD

- **SR(D,W)**: percentuale di problemi `SOLVED` nella cella. Esempio `SR(D10,W5)`.
- **MSD@90**: la profondità massima con SR ≥ 90%, a W fissato.

### 5.3 RE, e che cosa si può osservare oggi

```text
RE = minimum required inference steps / executed inference steps
```

Se il percorso ottimale richiede 10 inferenze e ne vengono eseguite 100, RE = 0.10; con 12,
RE = 0.83.

**Onestà sulla misura.** Il solver conta i passi di risoluzione (`Solver.steps`,
`kb_inference_report`) e `/debug` li riporta, ma **per turno intero**: ~42.000 passi per una
polare, ~75.000 per un'aperta, quasi tutti della macchina del turno (illocuzione, cue,
frame). La dimostrazione è una frazione non isolata. Oggi si può riportare solo il **costo
marginale** (passi al variare di D a parità di forma); una RE vera richiede che la
dimostrazione sia **un oggetto osservabile** — lo stesso requisito della resa degli stati
intermedi (§8, mossa 3).

---

## 6. I limiti del motore, detti come ipotesi

| Limite (`src/kb.c`, `src/kb.h`) | Valore | Che cosa predice sul banco |
|---|---|---|
| `KB_MAX_DEPTH` | 64 | la ricorsione si ferma: una catena lineare dovrebbe rompersi fra D30 e D64, a seconda di quanti livelli aggiungono le regole della domanda |
| `KB_MAX_BIND` | 384 | i legami si accumulano per ramo: circa 3 per applicazione di regola |
| `KB_MAX_GOALS` | 64 | la risolvente cresce di un goal per livello |
| `KB_MAX_STEPS` | 500.000 | W alto moltiplica i rami: il budget, non la profondità, dovrebbe rompere prima D10-W10 |
| controllo dei cicli (`anc`) | goal ground ripetuti | un ciclo nelle regole è tagliato, non esplode |
| `KB_MAX_BODY` / `KB_MAX_ARGS` | 16 / 4 | nessun effetto: le regole del banco hanno 2 goal binari |

**Previsione scritta prima di D40** (lettura di `solve_frame`, 14 settembre): la regola del
banco è `c(k+1)(X,Z) :- c(k)(X,Y), a(k+1)(Y,Z)`, quindi a ogni livello la risolvente tiene il
secondo goal del corpo come continuazione, più il segnaposto `__end_inference_scope` quando
il goal espanso è **ground**. Per la polare (`c(D)(n0, fine)` è ground) la risolvente cresce
di 2 per livello e `KB_MAX_GOALS` = 64 dovrebbe tagliare **intorno a D31**; per l'aperta (il
primo argomento è una variabile, nessun segnaposto) di 1 per livello, **intorno a D62**, cioè
lo stesso punto di `KB_MAX_DEPTH`. Il taglio è marcato (`budget_hit`), quindi l'esito atteso
è un «I don't know», non un «No».

Un limite che si raggiunge **deve dirsi** (`budget_hit` e `loops_cut` sono contati apposta):
un «I don't know» dopo un budget esaurito è un muro onesto a metà. Il banco verifica anche
questo.

---

## 7. Esperimenti

Strumenti: `tests/symbolic-depth/gen.py` (generatore, seme fisso) e
`tests/symbolic-depth/run.py` (demone `.p0t`, giudizio per esito, `--steps` per i passi di
`/debug`).

```sh
make build && make test-engine
python3 tests/symbolic-depth/gen.py --depths 1,3,5 --widths 1,3 --seed 1 > /tmp/p.jsonl
python3 tests/symbolic-depth/run.py --verbose /tmp/p.jsonl
```

### 7.1 Le superfici della stessa classe (sonde, 14 settembre)

La stessa catena D1–D2 detta in modi diversi. È l'asse della **generalità**: la classe
non è «la frase del banco», è «un problema simbolico, in qualunque superficie».

| # | Superficie | Che cosa fa parrot0 oggi | Livello che si rompe |
|---|---|---|---|
| S1 | lezione condizionale inglese, un turno per frase: «if x is the dax of y and y is the wug of z then x is the fep of z», «tavo is the dax of brin», «who is the fep of celo» | `Learned rule: fep($V1, $V3) :- dax($V1, $V2), wug($V2, $V3).`, fatti, «tavo.», «Yes.», e sulla polare falsa «I don't know: nothing I hold says…» | nessuno: **è la superficie della griglia §7.2** |
| S1b | S1 con i fatti **prima** della regola | uguale | nessuno |
| S2 | S1 in italiano («se x è il dax di y…», «chi è il fep di celo») | regola e fatti letti, l'aperta risponde; la polare «tavo è il fep di celo?» → «Non capisco ancora»; senza «?» diventa un fatto asserito | lettura della polare |
| S3 | «**whenever** x is the dax of y and y is the wug of z, x is the fep of z» | `Held: whenever x is dax of y…` — tenuta come frase, non come regola; poi «I don't know about fep» | lettura: il marcatore di condizione non è in `logic_connector` |
| S4 | condizione posposta: «x is the fep of z **if** x is the dax of y…» | tenuta come frase, **e i due fatti dopo diventano anch'essi `Held:`** invece di `Learned:` | lettura, con contagio del contesto |
| S5 | fatto rovesciato «the dax of brin is tavo», genitivo «celo's wug is brin» | «I don't understand that yet», «I don't know about celo's»; poi «Nobody that I know of» | lettura |
| S6 | **un solo testo** con regola, fatti e domanda separati da punti | legge la prima frase (la regola) e **tace sul resto**: nessuna risposta, nessun gap detto | lettura del testo composto, muro silenzioso |
| S7 | notazione del piano: `FACTS: p(a,b) … RULES: p(X,Y) & q(Y,Z) -> s(X,Z) … QUERY: t(a,d)` | «That looks like a snippet of code.» | lettura: la notazione non entra nella IR |
| S8 | clausole P0/Prolog in chat: `dax(tavo, brin).`, `fep(X, Z) :- dax(X, Y), wug(Y, Z).`, `fep(tavo, celo)?` | «That looks like a snippet of code» a ogni riga — **parrot0 non legge in conversazione la lingua della sua stessa KB** | lettura |
| S9 | frecce proposizionali: «A -> B. B -> C. C -> D. A. Prove D.» | «I don't understand that yet.» (4,5 s) | lettura |
| S10 | proposizioni: «p implies q. q implies r. r implies s. p is true. Is s true?» | `Held: if a p b then a q b -- one way only` ×3, poi «I couldn't read «p is true»», e `r` letto come **are** (forma SMS) | lettura: implicazione fra proposizioni letta come fra relazioni |
| S11 | «if p then q», «p», «is r true?» | «That looks like a logic problem, and I cannot solve it yet.» | lettura (il registro è riconosciuto, il contenuto no) |
| S12 | «why?», «how do you know?», «why is tavo the fep of celo?» dopo un «Yes» derivato | «That's the start of a question», «I haven't answered a knowledge-based question yet, so I don't have a proof to share», e al terzo **«Yes.»** (il «why» è ignorato) | **resa degli stati intermedi**: la dimostrazione non è un oggetto che si possa dire |

**Lettura.** Il ragionatore regge (§7.2), ma **una sola superficie su dodici** arriva al
ragionamento. La classe «problema simbolico» oggi esiste in una frase inglese. È la stessa
diagnosi della missione multi-hop (§4.4 là): il collo di bottiglia non è il solver, è
l'ingresso nella IR. E la S12 dice che la memoria degli stati intermedi, che il piano mette
fra le quattro capacità del traguardo, **c'è nel solver e non esiste per parrot0**: non la
può né mostrare né interrogare.

### 7.2 La griglia D×W sulla superficie che si legge

Un problema per cella (quindi SR è 0 o 100% e **MSD@90 non è ancora calcolabile**: servono
più semi per cella, §8 mossa 5). Tempi di parete dal demone, ~1,1 s per turno.

**Catena lineare, W1** (`lin`, semi 14092026, 1, 4, 6)

| D | 1 | 2 | 3 | 5 | 7 | 10 | 15 | 20 | 30 | 40 | 50 | 60 | 61 | 62 | 63 | 64 | 70 | 100 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| esito | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ½ | ✘ | ✘ | ✘ |

D63: l'aperta risponde, la polare vera dice «I don't know: nothing I hold says…». D64 e
oltre: «Nobody that I know of» e «I don't know». Nessun falso, **ma nessuna delle due
risposte dice che la ricerca è stata tagliata**: dice che non c'è niente, ed è falso.

**Catena con distrattori** (`lin`, seme 2): D3-W3, D3-W10, D5-W3, D5-W10, D10-W3,
**D10-W10** tutti ✔, polari false oneste.

**Join** (`join`, seme 3): D3, D5, D7, D10 × W1, W3 tutti ✔.

**Attenzione a che cosa misura W in `lin` e `join`.** Il solver ragiona all'indietro dalla
domanda: le regole inutili hanno teste che la domanda non chiede e non vengono mai toccate,
e i predecessori morti falliscono al primo livello. Per un ragionatore goal-directed quei
distrattori sono quasi gratis — che è il comportamento giusto, ma non mette sotto sforzo il
controllo. Da qui la famiglia `dag`.

**Spazio esponenziale** (`dag`: strati di W nodi collegati tutti con tutti, una radice)

| Cella | Cammini | Esito | Passi del turno, aperta / polare falsa (base ~75k / ~48k) |
|---|---|---|---|
| D4-W2 … D10-W2 | 8 … 512 | ✔ | invariati |
| D8-W3 | 2.187 | ✔ | 79k / 52k |
| D10-W3 | 19.683 | ✔ | 105k / 78k |
| D12-W3 | 177.147 | ✔ | 341k / 314k |
| D14-W3 | 1.594.323 | ✔ | **575k / 548k: il predicato della domanda usa esattamente 500.000 passi** |
| D12-W4 | 4.194.304 | ✔ | 577k / 548k, budget esaurito |
| D16-W3 | 14.348.907 | ✔ | 575k / 548k, budget esaurito |

Oltre D12-W3 la risposta giusta arriva **prima** del taglio, quindi il banco con una
radice la dà per buona; la polare falsa esaurisce il budget e dice «nothing I hold says…,
and nothing says it isn't so» — lo stesso testo di una ricerca completa.

### 7.3 La prova del taglio: una lista incompleta detta come intera

Stesso `dag` con **tre radici** (tre risposte vere), seme 8:

| Cella | Risposta all'aperta | Esito |
|---|---|---|
| D10-W3 (sotto budget) | «drokufe, viboma, kibrugip.» | ✔ |
| D14-W3 (sopra budget) | «**viklogon.**» | **PARTIAL**: due risposte vere perse, nessun avviso |

È un misclaim di completezza, il più insidioso: la risposta è vera, la sua forma («chi è…?»
→ un nome) dice che è tutta. Il solver il taglio lo conta (`budget_hit`) e la polare di
classe lo legge già (`polar_class_answer` → `undetermined_cycle`); le risposte relazionali
no. `run.py` ora dà l'esito `PARTIAL`.

### 7.4 I passi, e perché RE aspetta

`tests/symbolic-depth/steps-2026-09-14.txt`. Con `/debug` i passi del turno sono ~75.000
per l'aperta e ~42.000 per la polare vera **a qualunque D da 1 a 40** (differenze sotto
l'1%): la dimostrazione lineare costa pochi passi per inferenza e sparisce nel rumore della
macchina del turno. Diventa visibile solo nei `dag`, dove la riga del predicato chiesto la
isola (`500000 passi 1 call trubrot`). Una RE onesta vuole la dimostrazione come oggetto
(§8 mossa 2), non una sottrazione fra turni.

### 7.5 Previsioni e smentite

| Previsione (§6) | Esito |
|---|---|
| `KB_MAX_GOALS` taglia la polare intorno a D31 | **smentita**: D62 regge |
| `KB_MAX_DEPTH` taglia intorno a D62 | **confermata**: D62 ✔, D63 ½, D64 ✘ |
| il budget rompe prima D10-W10 | **smentita** per `lin` (goal-directed, §7.2); **confermata** per `dag` da D14-W3 |
| un taglio si dice | **smentita** per le risposte relazionali (§7.2, §7.3) |

---

## 8. Che cosa ne segue: le mosse, in ordine

Nessuna di queste è un ramo nuovo: ognuna fa crescere un pezzo che c'è, e si prova sul
banco **con semi nuovi e con superfici diverse** da quelle su cui è stata scritta.

1. **Dire il taglio (zero misclaim prima di tutto).** Le risposte relazionali, aperta e
   polare, leggano `kb_inference_report` come fa già la polare di classe: una ricerca
   tagliata dice che si è fermata e perché (profondità, budget), e un elenco tagliato non
   si presenta come intero. Le parole sono template in KB (`undetermined_cycle` c'è già).
   Prova: `lin` D64–D100 e `dag` D14-W3 a tre radici passano da UNSOLVED/PARTIAL a un muro
   dicibile; nessuna cella oggi ✔ cambia.
2. **La dimostrazione come oggetto della IR.** La derivazione del solver (quali clausole,
   quali legami intermedi) diventa un fatto di turno interrogabile, e «why?», «how do you
   know?», «perché?» la rendono con `derivation_answer/3` e `turn_response_part/3`. È la
   «memoria degli stati intermedi» del traguardo resa **di parrot0** e non solo del solver;
   dà anche la RE vera (§5.3). Stessa necessità della traccia multi-hop
   ([multi-hop-deep-memory](multi-hop-deep-memory.md) §8: la traccia è una vista della IR).
3. **L'ingresso universale.** Le superfici S2–S11 di §7.1 entrano nella stessa IR, e la
   lezione condizionale smette di essere un lettore di parole a parte (debito §2):
   - marcatori di condizione mancanti («whenever», condizione posposta): conoscenza in
     `logic_connector`, insegnabile parlando;
   - il testo unico (S6): il lettore del turno composto (gen506c) esiste, e il problema
     simbolico è un testo composto;
   - la notazione formale e le clausole P0 (S7, S8): la sintassi della KB ha già un parser;
     leggerla in conversazione vuol dire darle un posto nella IR del turno (un enunciato
     con regola o fatto), non un secondo lettore;
   - proposizioni e frecce (S9–S11): l'implicazione fra proposizioni è un'altra arietà
     della stessa regola.
   Prova: un asse `--surface` in `gen.py`, **scritto prima della cura**, con la stessa
   griglia su ogni superficie.
4. **Il controllo della ricerca.** Il muro a D63 e il budget a D14-W3 non si spostano
   alzando i limiti: il primo chiede che una catena lunga non consumi un livello di
   ricorsione del C per inferenza, il secondo che un sotto-goal già risolto non si risolva
   di nuovo (tabling). Il solver è motore e resta nel C; la politica (quali predicati
   tabulare, quanto spendere) è conoscenza. Prova: `dag` D16-W3 a tre radici completo
   **sotto** il budget attuale.
5. **Il banco che manca.** Più semi per cella (SR vero e MSD@90); le famiglie di §4.8
   (albero con pruning), §4.9 (join multipli) e i cicli; la stessa griglia in italiano.

---

---

## 9. Registro

| Data | Che cosa | Esito |
|---|---|---|
| 14 set 2026 | piano riordinato; vincolo di F. sulla comprensione universale (§2); `gen.py` e `run.py` | — |
| 14 set 2026 | sonde di superficie (§7.1) | 1 superficie su 12 arriva al ragionamento |
| 14 set 2026 | prima griglia: un «misclaim» a D3-W3 era **del banco** (un distrattore in posizione 0 era una seconda soluzione vera); da allora le attese le calcola l'oracolo di `gen.py` per concatenazione in avanti | griglia rifatta |
| 14 set 2026 | `lin` D1–D100, `lin` W3/W10, `join`, `dag` fino a D16-W3, `dag` a tre radici (§7.2–7.3) | lineare fino a D62; D10-W10 ✔; budget da D14-W3 non detto; 1 PARTIAL |
