# Chi ha diritto al turno — le discipline dell'arbitrato, in ordine

> **gen502.** Nasce da una domanda di F. — *«ci sono delle discipline costituite
> per gestire i turni rubati, fai una ricerca e metti in ordine le strategie»* —
> e da una giornata di misure sul banco di gara. La ricerca conferma che il
> problema ha una letteratura di cinquant'anni; le misure dicono **dove si trova
> parrot0 su quella scala** e perché il gradino successivo non è quello che
> stavamo salendo.

---

## 1. Il fatto che ha aperto la domanda

Prompt di `match0` (difficoltà 1, 864 byte). Rispondeva `compose` con una storia
sul `Makefile` più un fatto appreso da un imperativo. Ho aggiunto la cessione
`faculty_yield_both(compose, open, codebase_referent, source_change_directive)`.
Poi ho rimisurato, e ho continuato a rimisurare:

| dopo aver governato | prende il turno | risposta |
|---|---|---|
| — | `compose` | *«1) Makefile was a mysterious Makefile… 2) Learned: do not merely describe patch.»* |
| `compose` | `gen` | *«Makefile was a mysterious Makefile…»* (la stessa storia, senza numeri) |
| `compose`, `gen` | **`codeast`** | *«I read that as code, but I am not sure which function you mean.»* |

Il terzo è **un buon turno**: nomina ciò che ha riconosciuto e il proprio buco.
Ma sono serviti tre giri per un prompt, e sugli altri due match del banco la
catena riparte da capo: `match1` → `gen`, `match2` → `role` (*«Alright — I am
Working now»*: ha letto «You are working…» come l'ordine di impersonare).

⚠ La KB lo aveva già scritto, otto generazioni fa, in `kb/core/intents.p0`:

> *«ed è la prova che governare una facoltà sola sposta il turno rubato invece
> di chiuderlo.»*

Allora fu una nota a margine. Oggi è una **misura ripetuta su tre input
indipendenti**, ed è la ragione per cui vale la pena guardare la letteratura
invece di aggiungere la quarta riga di cessione.

### E tre difetti strutturali trovati per strada, che non sono incidenti

| | trovato | effetto |
|---|---|---|
| **A** | L'indice «chi è governato» si costruiva dal solo `faculty_yield/3` | `faculty_yield_both/4` e `faculty_yield_force/3` erano **morte in silenzio** per ogni modulo dispacciato dal registro |
| **B** | La cessione congiunta leggeva solo `norm`, che il dispatch tronca a `canon[256]` | Misurato spostando la cue nello stesso testo: cede a colonna 217, **non cede a colonna 262**. I prompt del banco sono 864, 1485 e 1839 byte |
| **C** | `kb_cue_match_plain` copia il testo in un `masked[512]` | Terza finestra, terza cecità, indipendente dalle altre due |

**Non sono tre bug: sono lo stesso bug tre volte.** Sono ciò che succede quando
una politica di arbitrato è espressa come *molti piccoli test di sottostringa su
finestre di buffer* invece che come **un confronto fra pretese**. Ogni nuova
finestra è una nuova occasione di cecità, e nessuna di esse è visibile finché
qualcuno non misura proprio quel caso.

---

## 1-bis. ⛔ IL GRADINO CHE VIENE PRIMA DI TUTTA LA SCALA — la legittimità

> F., 2026-09-04: *«i moduli obsoleti che rubano turni non sono legittimati a
> farlo. Un modulo può continuare a prendere turni se è addestrabile, se è
> KB-first, se è basato sulla comprensione universale. Non esistono moduli che
> rubano turni per stato del codice.»*
>
> E, alla prima stesura di questa sezione: *«non la copertura. Il criterio è la
> **review della sua implementazione**, che deve stare in testa al modulo in un
> commento. Un modulo pieno di TODO KB-first è un modulo che non ha diritto di
> rubare turni, tranne dove risulta un fallback per gli altri. Dobbiamo premiare
> la competenza e la maturità dell'implementazione.»*

**Questa sezione riscrive la §3 e va letta prima della scala.** Le strategie
S0-S6 rispondono tutte a *«chi vince fra i pretendenti?»*, e danno per scontato
che i pretendenti abbiano titolo. **Non ce l'hanno.** Una facoltà che rivendica
un turno perché è arrivata prima nell'array — perché esiste da più tempo, perché
nessuno l'ha ancora governata — non esercita una **capacità**: esercita una
**posizione**, cioè lo stato del codice travestito da condotta.

### ⭐ 1-bis.1 I due fallimenti sono classi diverse, e hanno rimedi diversi

Questa è la parte che vale più di tutto il resto del documento.

| chi vince a torto | che cos'è | il rimedio |
|---|---|---|
| un modulo **immaturo** (vecchio, pieno di TODO KB-first, vocabolario nel C) | ⛔ **un errore di architettura** | **retrocederlo**: perde il diritto di pretendere, resta come ultima risorsa. **Non si insegna niente** |
| un modulo **maturo** (addestrabile, KB-first, fondato sul frame) | 🟡 **un comportamento da correggere** | **insegnarglielo**: una cue, una regola, una cessione. È esattamente ciò per cui `faculty_yield` esiste |

⛔ **E qui sta l'errore che ho commesso stamattina, che vale la pena scrivere
perché è la spiegazione del whack-a-mole:** ho applicato il *rimedio 2* a un
problema di *classe 1*. Ho scritto `faculty_yield_both` per `compose` e per
`gen` — cioè ho *insegnato una condotta* a moduli che non avevano titolo a
pretendere. Insegnare a un modulo immaturo di tacere su una classe non lo rende
maturo: lo lascia libero su tutte le altre, e il turno successivo lo ruba di
nuovo altrove. **Ogni riga di cessione scritta per un modulo immaturo è un
sintomo curato al posto della causa.**

⚠ Ne segue una regola operativa immediata, prima ancora dell'infrastruttura:
**prima di scrivere una `faculty_yield`, chiedersi in quale classe cade il
furto.** Se il modulo è immaturo, la cessione è la risposta sbagliata anche
quando funziona.

### 1-bis.2 Il criterio: una REVIEW dichiarata in testa al modulo

Non una metrica dedotta di nascosto: **una review scritta, in un commento in
testa al modulo**, che dichiara lo stato dell'implementazione e da cui discende
il diritto di pretendere.

```c
/* MODULE REVIEW — mod_gen                                    rivista: gen502
 *   maturita'      legacy | transitional | kb_first
 *   diritto        primary | fallback | none
 *   L1 addestrabile   si ritira una sua forma di riconoscimento -> la pretesa sparisce?
 *   L2 kb_first       quanti confronti di parole letterali restano nel C
 *   L3 universale     pretende sul FRAME del turno, o su una sottostringa del grezzo?
 *   todo_kb_first     quanti TODO(kb-first) aperti
 *   perche'           una riga: che cosa manca perche' salga di grado
 */
```

⭐ **E la review non può mentire, perché ogni campo ha un controllo meccanico
accanto** (`todo_kb_first` si conta, i literali si contano, L1 si prova
ritirando una cue a runtime). Una testata che dichiara `kb_first` con dieci
literali e due TODO aperti **fallisce un cricchetto**: è una review, non
un'autocertificazione.

### 1-bis.3 Dove vive, perché il commento non basta da solo

Il commento è per gli umani; l'arbitrato ha bisogno di **fatti**. La convenzione
esiste già in questo repository — `src/brain/00-lex.c.cues.p0` è la proiezione
in KB di ciò che sta nel C. Stessa strada:

```text
src/brain/30-generation-reading.c            la review, in testa, leggibile
src/brain/30-generation-reading.c.review.p0  la sua proiezione, generata
   module_review(gen, maturity, transitional).
   module_review(gen, claim_right, fallback).
   module_evidence(gen, literals, 5).
   module_evidence(gen, todo_kb_first, 0).
```

Così la review sta **in testa al modulo** come chiede F., e resta **conoscenza**
come chiede il mantra #17: interrogabile (*«perché non hai risposto tu?»* →
*«non ho titolo: sono `transitional`»*), e correggibile — ma correggibile solo
**migliorando il modulo**, che è il punto.

### 1-bis.4 L'evidenza misurata oggi, e correla

Conteggio grezzo sui quattro file in causa (confronti di parole letterali nel C
contro letture dalla KB — è un *proxy*, non ancora la review):

| modulo | file | literali | letture KB | rapporto | ha risposto |
|---|---|---:|---:|---:|---|
| `codeast` | `80-code.c` | **1** | 61 | **61:1** | ⭐ *«I read that as code, but I am not sure which function you mean.»* |
| `gen` | `30-generation-reading.c` | 5 | 218 | 44:1 | la storia sul `Makefile` |
| `role` | `40-meta-reflection.c` | 16 | 189 | 12:1 | *«Alright — I am Working now.»* |
| `compose` | `60-agent-tools.c` | 10 | 83 | **8:1** | la composizione che ha spezzato la specifica |

**Il modulo che ha dato l'unica risposta onesta è quello con meno vocabolario
compilato**, e il primo ladro è quello col rapporto peggiore. Un campione di
quattro non è una legge — ma è esattamente il verso che la tesi di F. prevede, e
rende il criterio misurabile invece che programmatico.

### ⭐ 1-bis.5 ESEGUITO il 2026-09-04 — e l'ablazione conferma le due classi

Costruito: dispatch a **due passate** nel registro (chi ha titolo, poi i
retrocessi), `kb/core/module-review.p0` con quattro review scritte, le testate
in cima ai quattro moduli, e il cricchetto `tests/tools/module_review.py`.

| | prima | dopo |
|---|---|---|
| match0 | `compose` — storia sul Makefile + imperativo appreso | **`codeast`** — *«I read that as code…»* |
| match2 | `role` — *«Alright — I am Working now»* | **`codeast`** — la stessa risposta onesta |
| match1 | `gen` — la storia | `gen` — ⚠ debito aperto |

⭐ **match2 si è spostato con la SOLA retrocessione di `role`, zero righe di
cessione scritte per lui.** È il rimedio della classe giusta: togliere il
titolo vale su *tutte* le classi, comprese quelle che nessuno ha incontrato.

**E l'ablazione che separa i due rimedi, misurata:**

| cessione scritta per | classe del modulo | ritirandola |
|---|---|---|
| `compose` | **immaturo** → si retrocede | **niente cambia** — era peso morto, la retrocessione la copriva già |
| `gen` | **maturo** → si insegna | match0 torna subito a essere rubato: è **portante**, e resta |

Non è una conferma comoda: la riga scritta per il modulo immaturo è stata
**ritirata dal repository**, e quella per il modulo maturo è rimasta. La
distinzione di F. regge una prova che poteva farla cadere.

### 1-bis.6 Il cricchetto, e perché può solo falsificare

`tests/tools/module_review.py` (in `make test`) rifiuta: una review in KB senza
testata nel C e viceversa; `kb_first` dichiarato con parole di dominio compilate
o `TODO(kb-first)` aperti; un `module_evidence` che non combacia con la misura
di oggi; una facoltà recensita che non è nel registro.

⚠ **Al primo giro ha falsificato una review corretta**, ed è servito: contava
`strcmp(b->last_module, "gen")` come vocabolario compilato, mentre è una
facoltà che riconosce se stessa — meccanismo. È stata corretta **la misura**,
non la review. Una metrica che accusa il codice giusto è peggio di nessuna
metrica.

⛔ **E il limite, dichiarato:** contando *per modulo* (non per file),
`mod_compose` — il primo ladro — ha **zero** parole compilate e **zero** TODO,
esattamente come `mod_codeast` che ha risposto bene. **Nessuna metrica distingue
un modulo maturo da uno immaturo.** Per questo la maturità resta un giudizio
scritto e datato, e i numeri possono solo provare che quel giudizio mente. È
anche il motivo per cui **4 moduli su 78 sono recensiti**: le altre 74 facoltà
restano senza riga finché qualcuno non le guarda davvero, e competono come
prima — permissivo per default, come ogni condotta di questo progetto.

### 1-bis.7 Il debito aperto, misurato

`gen` non cede su `match1` (1485 byte) benché entrambe le cue siano nel testo
(`codebase`@47, `replace its`@174). Due finestre già corrette — `canon[256]` nel
dispatch e `masked[512]` in `kb_cue_match_plain` — non bastano a spiegarlo.
Finché resta, `gen` è maturo ma **non ancora governabile su prosa lunga**, ed è
la prossima cosa da chiudere: è anche l'unico dei tre prompt del banco che il
lavoro di oggi non ha spostato.

### 1-bis.8 Che cosa resta della copertura

La copertura (S4) **non è più la prova di legittimità** — quello era il mio
errore. Resta però il criterio giusto per il gradino dopo: **fra pretendenti che
hanno già titolo**, vince chi rende conto di più turno. Le due cose stanno in
serie e non vanno confuse:

```text
LEGITTIMITA' (§1-bis)   chi ha DIRITTO di pretendere   ← la review dell'implementazione
        ↓
ARBITRATO (S4)          chi VINCE fra chi ha diritto   ← la copertura del turno
```

---

## 2. La scala delle discipline, dalla più debole alla più forte

Le prime tre vengono dai **sistemi a produzioni** (OPS5, Soar); la quarta e la
quinta dalla **subsumption architecture** e dai **sistemi a blackboard**
(Hearsay-II); le ultime due dal **dispatch multiplo** (CLOS) e dal
**ragionamento defeasible**.

### S0 — Ordine fisso *(dove parrot0 sta ancora, per la maggior parte)*

Il primo modulo del `registry[]` che accetta, vince. È il livello zero: **la
politica non ha nome**, esiste solo il fatto che una riga viene prima di
un'altra. Non è interrogabile e non è correggibile parlando — il difetto che il
mantra #17 vieta, e che `kb/core/intents.p0` già descrive con queste parole.

### S1 — Inibizione esplicita *(dove parrot0 è arrivato, ed è il gradino che sta finendo)*

`faculty_yield(Facoltà, Stadio, Classe)`: una facoltà dichiara quando **deve
tacere**. È esattamente l'**inibizione** della subsumption architecture di
Brooks — un livello superiore azzera il segnale di uno inferiore.

**È un progresso vero** rispetto a S0: la politica ha un nome, si interroga, si
corregge parlando, e un modulo nuovo nasce governabile senza scrivere C.

⛔ **Ma non scala, e oggi sappiamo perché in modo preciso.** L'inibizione è una
struttura **a coppie** su un insieme che cresce: N facoltà × M classi. Ogni
coppia va scritta a mano, dopo che il turno è già stato rubato almeno una volta.
La tabella si riempie di *sintomi*, non di *principio* — ed è la definizione di
whack-a-mole misurata al §1.

### S2 — Refrazione — *cheap, e parrot0 ne ha già un pezzo*

Da OPS5: **la stessa istanziazione non si esegue due volte.** Qui: una facoltà
già scavalcata in questo turno non torna a rivendicarlo. `mod_compose` ha già la
sua forma (`no re-entry`), ma come `continue` in un `for`, non come politica
nominata. Costa poco e va reso esplicito, però **non risolve niente da solo**:
impedisce i cicli, non le pretese sbagliate.

### S3 — Specificità — *il primo gradino che cambia la forma del problema*

Da OPS5, e la stessa idea sta nel dispatch multiplo di CLOS e nel defeasible
reasoning di Poole: **fra due pretese entrambe soddisfatte vince quella con le
condizioni più specifiche** — tipicamente quella con più precondizioni — così le
eccezioni battono i default senza che nessuno scriva l'eccezione a mano.

Il salto concettuale: si smette di dire *«A non deve parlare quando c'è B»*
(N×M fatti) e si comincia a dire *«ogni facoltà dichiara quanto pretende»*
(N fatti). **La tabella delle coppie sparisce.**

### S4 — ⭐ Copertura — *la specificità nella forma che serve QUI*

Per un turno lungo la misura naturale di specificità non è «quante
precondizioni», è **quanta parte del turno la pretesa rende conto**.

⭐ **E parrot0 ce l'ha già scritta**, in `frontier-kb-natural-dialogue.md`, come
ipotesi **D14**: *«la comprensione si misura in copertura, e il residuo è un
oggetto»*. Non è mai stata usata per l'arbitrato — ed è precisamente l'arbitrato
che avrebbe chiuso tutti e tre i furti in una volta:

```text
match1: 1485 byte, ~240 parole di specifica tecnica
  gen pretende il turno rendendo conto di ~una parola («it»)
  una facoltà di codice ne renderebbe conto di decine
```

Nessuna cessione da scrivere: `gen` **perde**, perché copre quasi niente. E la
stessa regola vale per `match2`, per `match0`, e per il prossimo prompt che
nessuno ha ancora visto.

⭐ **La parte migliore: il materiale c'è.** `input_segment` produce già span
tipati, `segment_role` li classifica, `faculty_for/2` nomina i consumatori.
La copertura è **calcolabile da ciò che esiste**: quanti span il vincitore
dichiara di consumare, sul totale. Non serve una rappresentazione nuova.

E porta un secondo regalo che l'inibizione non può dare: **il residuo diventa un
oggetto**. «Ho reso conto di questo, non di quello» è insieme una risposta più
onesta, un obbligo di evidenza (UC §4) e un'impresa da riprendere (D49).

### S5 — Offerta con evidenza dichiarata *(blackboard / contract net)*

In Hearsay-II le knowledge source non si inibiscono a vicenda: **contribuiscono
al blackboard e uno scheduler decide opportunisticamente** chi attivare, sulla
base di ciò che è stato scritto finora. La generalizzazione moderna è il contract
net: si annuncia il compito, si ricevono offerte, vince la migliore.

Per parrot0 significa: una facoltà non *risponde* — **offre** una pretesa con la
propria evidenza dichiarata, e l'arbitro sceglie. Il guadagno è che la scelta
diventa **ispezionabile e criticabile**: si può chiedere *perché tu e non lui*, e
la risposta è un confronto fra evidenze, non l'ordine di un array. È anche la
condizione perché il thinking abbia qualcosa su cui pensare (D50).

### S6 — L'ambiguità è un esito, non una scelta silenziosa

Da CLOS: se due metodi sono applicabili e nessuno è più specifico, **è un
errore**, non un tiro a sorte. parrot0 lo fa già in un punto solo — `eager_ambiguous`
rinuncia alla via veloce invece di indovinare — e quella riga è il modello da
generalizzare. Una pretesa contesa deve poter diventare **una domanda**, che è
anche la mossa `clarify` di K3.

---

## 3. La raccomandazione, e perché salta un gradino

⛔ **Non aggiungere la quarta riga di `faculty_yield`.** Ne servirebbero una per
ogni coppia (facoltà, classe) e ognuna arriva *dopo* un furto già avvenuto.

⭐ **Saltare da S1 a S4 (copertura), con la legittimità del §1-bis come
condizione d'ingresso** e S1 come rete di sicurezza sui casi residui. Le
ragioni, in ordine di forza:

0. **Ma prima viene il titolo, non l'efficienza** (§1-bis): S4 arbitra fra
   pretendenti *legittimi*. Applicarla prima della review lascerebbe competere
   moduli immaturi, e un modulo immaturo che vince «per copertura» è ancora un
   errore di architettura — solo con un punteggio accanto.

1. **È una regola, non una tabella.** N fatti invece di N×M, e la crescita è
   monotona: una facoltà nuova non obbliga a modificare le altre.
2. **Chiude i tre difetti strutturali del §1 alla radice.** Una pretesa
   confrontata sugli span non ha finestre di buffer da sbagliare: non c'è
   nessun `canon[256]` né `masked[512]` nel confronto fra due coperture.
3. **Il materiale esiste già** (`input_segment`, `segment_role`, `faculty_for`).
4. **È già una nostra ipotesi** (D14), mai messa alla prova: costruirla qui la
   falsifica su un caso reale invece di lasciarla scritta.
5. **Il residuo diventa un oggetto**, e quell'oggetto è la stessa cosa che
   servono D49 (l'impresa da riprendere) e UC §4 (l'obbligo di evidenza).

S2 (refrazione) si prende quasi gratis lungo la strada. S5 e S6 vengono dopo, e
**non si costruiscono prima di S4**: un'offerta con evidenza senza una misura di
copertura è un'asta senza valuta.

### Il gate, falsificabile

> Con l'arbitrato per copertura e **zero righe di `faculty_yield` aggiunte**, i
> prompt di `match0`, `match1` e `match2` devono andare tutti e tre a una
> facoltà di codice. Poi si toglie l'arbitrato per copertura e devono tornare
> tutti e tre a rubare. Se serve anche una sola cessione scritta a mano per uno
> dei tre, S4 non ha sostituito S1 — ci si è appoggiato, ed è un altro sintomo.
>
> ⭐ **E il gate di legittimità, che è il più severo:** le due righe di
> `faculty_yield_both` scritte oggi per `compose` e `gen` **vanno ritirate**.
> Se il comportamento non cambia, la copertura ha reso superflua l'inibizione ed
> è la prova che il titolo funziona. Se cambia, quelle righe stavano ancora
> reggendo il risultato e S4 non è finita.

⚠ **Anti-impostore:** la copertura non si misura sulle cue che abbiamo scritto
oggi. Se un modulo «copre» un turno perché contiene le parole che gli abbiamo
insegnato ieri, abbiamo rifatto S1 con un altro nome.

---

## 4. Dove sta ognuna, oggi

| | strategia | stato in parrot0 |
|---|---|---|
| S0 | ordine di registro | ✅ è il default, e va superato |
| S1 | inibizione (`faculty_yield`) | ✅ esiste e funziona — ⛔ non scala, misurato |
| S2 | refrazione | 🟡 esiste come `continue` in un `for`, senza nome |
| S3 | specificità | ⛔ assente |
| S4 | **copertura** | ⛔ assente — ma D14 la nomina e il materiale c'è |
| **L** | **legittimità: la review in testa al modulo** (§1-bis) | ⛔ assente — **è il gradino zero, prima di S4**; mantra #21 |
| S5 | offerta con evidenza | ⛔ assente (Hearsay-II) |
| S6 | ambiguità come esito | 🟡 solo su `eager_ambiguous` |

---

## 5. Fonti

- [Conflict resolution strategy](https://en.wikipedia.org/wiki/Conflict_resolution_strategy) — refrazione, recency, specificità
- [OPS5 Reference](https://www.cs.gordon.edu/local/courses/cs323/OPS5/ops5.html) — la forma canonica delle tre
- [Notes: Production Systems, Trinity College](https://www.cs.trincoll.edu/~ram/cpsc352/notes/productions.html)
- [The Hearsay-II Speech-Understanding System](https://websites.nku.edu/~foxr/CSC425/hearsay2.pdf) — knowledge source, blackboard, controllo opportunistico
- [A Retrospective View of the Hearsay-II Architecture](https://www.researchgate.net/publication/220815581_A_Retrospective_View_of_the_Hearsay-II_Architecture)
- [Subsumption architecture](https://handwiki.org/wiki/Subsumption_architecture) — inibizione e soppressione come arbitrato
