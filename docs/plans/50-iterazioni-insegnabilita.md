# 50 iterazioni di insegnabilità — handoff a 28/50

> **Stato: 28 giri chiusi, committati e pushati su `main` (`gen507/1` … `gen507/28`).
> Ne restano 22.** Ogni giro è un commit con il prompt che ha scoperto il limite,
> la cura, e le righe di verifica. Questo file è il contratto del lavoro, non un
> riassunto: chi riprende deve poterne fare il 29° senza chiedere niente.

---

## 0. Le premesse — da leggere prima di toccare qualsiasi cosa

Sono indicazioni date da F. **durante** la campagna, e hanno cambiato il metodo
in corsa. Valgono da qui in avanti, non dal prossimo file.

### 0.1 Che cosa si conquista

> *«quello che dobbiamo conquistare sono la crescita della abilità di apprendere
> di parrot0»* — F., 2026-09-09

Il bersaglio non è far passare un prompt. È che **la classe di prompt** a cui
quello appartiene diventi insegnabile. Un giro è valido se, dopo, parrot0 può
imparare un *membro nuovo* di quella classe **senza ricompilare**. È il test
operativo del mantra, applicato a ogni singola iterazione.

### 0.2 Il C è un punto di innesco, non il posto delle forme

> *«mi sembra che non usi la comprensione universale
> [`universal-comprehension.md`](universal-comprehension.md) … le soluzioni che
> hai fatto sono troppo basate sul C, ogni forma nuova era un blocco di C. Quello
> che mi aspetto è che anche l'istanza procedurale esposta dal C — cioè la sua
> presenza all'interno di un punto preciso di un flusso — sia pilotata da KB:
> quindi in questo momento stiamo mettendo nel C punti di innesco di interpreti
> locali basati sulla KB»* — F., 2026-09-10

**Questa è la correzione più importante del documento.** I giri **/1–/18** hanno
chiuso muri veri, ma quasi ognuno ha aggiunto *una funzione scritta a mano*: una
forma, un blocco C. Funziona e non si rimuove ([§0.5](#05-non-si-revert)), ma
**non è il metodo**.

Dal giro **/19** il metodo è: **una porta in C, le forme in KB.**
Vedi [§3](#3-linterprete-di-forme--il-motore-che-i-giri-devono-usare).

### 0.3 Comprensione universale

[`universal-comprehension.md`](universal-comprehension.md) è il piano di
riferimento. Tre cose da portarsi dietro:

- **Le tre specie di lacuna** (§10 di quel piano): variante di superficie /
  costruzione mancante / forma telegrafica. *Due su tre si chiudono senza mai
  vedere una chat.* La terza non si anticipa: **si rende insegnabile.**
- **`turn_pattern/3`** (mantra #19) aveva già sciolto la *congiunzione*: quali
  condizioni deve soddisfare un turno sono **fatti**, non `&&` nel C. Ma decide
  solo *che cosa vale* il turno — non estrae i pezzi. Il giro /19 ha aggiunto il
  pezzo mancante: la stessa idea portata dalla riconoscibilità alla **lettura**.
- **Si cerca il punto che tutte le vie attraversano**, non si enumerano i
  chiamanti. (La lezione delle tre volte, in quel piano.)

### 0.4 I test

Dalla politica di `CLAUDE.md` più l'indicazione esplicita di F. per questa
campagna:

> *«non ti preoccupare i test — valida la soluzione puntuale se risolve il prompt
> che ti eri prefissato»* · *«lascia che ti dica io come fare le verifiche»*

Operativamente:

- **La verifica di un giro è il prompt che ha scoperto il limite**, più i casi
  adiacenti che devono restare invariati. Si mandano a `./bin/parrot0` e si
  leggono le risposte **verbatim**. Niente altro.
- **La suite intera NON si lancia.** Non è allineata, è lunga, e va approvata da
  F. ([[dont-run-full-suite]]). Nessun `make test`, nessuna sessione di fix dei
  test dentro un giro.
- `make soft-test` si può usare dentro il ciclo di modifica (budget 15 s). Se
  sfora si tolgono casi, **mai si alza il budget**.
- **⚠ `tests/p0t/code/segment.p0t` si pianta** — verificato anche su `HEAD`
  prima di qualunque modifica di questa campagna. È un difetto precedente,
  segnalato e non toccato.
- Un confronto **prima/dopo** si fa con `git stash` + rebuild, e si fa **solo
  quando si sospetta una regressione** (è così che il giro /28 ha scoperto la
  propria). Non come rito.

### 0.5 Non si reverta

> *«non revertare nulla, migliora solo le prossime iterazioni; ci penso io a
> gestire il vecchio. Tu migliorati nel tuo processo e continua»* — F.

Vale anche per le strutture di parrot0: **si tengono le strutture secondarie**
([[keep-secondary-structures]]). Il dispatch è *first-match*: se una superficie
è già letta da qualcuno, **non si ruba il turno**. Si dichiara la lacuna nel
file e si sceglie un'altra superficie — è quello che hanno fatto i giri /23
(«a cosa serve X?») e /27 («X is transitive»).

### 0.6 Onestà

Un «Got it» su un fatto che non si tiene è **peggio di un muro**. Tre giri su
ventotto hanno chiuso esattamente questo: /4 (un fatto inventato da una lista),
/6 (un attributo accolto e non interrogabile), /16 (due valori contraddittori
tenuti entrambi, e una metà taciuta). Quando si trova una risposta *sicura e
sbagliata*, quella ha la precedenza su qualunque muro.

---

### 0.7 Dove finisce la catena di ciò che un giro apre

> *«preso una qualsiasi cosa che può essere insegnata a parrot0 si può
> individuare la superficie che la insegna e chiedersi quale superficie serve
> per insegnare quella abilità di insegnamento»* — F., 2026-09-11

Un giro apre una superficie. Da ora ogni giro dice anche **dove finisce la
catena** di quella superficie: in una primitiva del motore (radice), in una
lezione che sa estendere la propria forma (circolo), o in una riga `.p0`
scritta a mano che nessuna lezione produce. Il terzo caso non è un difetto del
giro, è l'informazione che serve per scegliere il prossimo: il buco più vicino
alla radice che chiude più catene insieme. Il metodo e la misura della
**vitalità** sono in [radici-insegnabilita.md](radici-insegnabilita.md).

## 1. Che cos'è un giro

Sette passi. Il ciclo dura pochi minuti quando la cura è in KB.

1. **Sondare.** Si mandano 6–10 prompt a `./bin/parrot0` in un colpo
   (`printf '%s\n' … | PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0`) e si
   legge quali cadono. Si sondano **famiglie**, non frasi: asserzione + domanda +
   negazione + variante d'ordine della stessa relazione.
2. **Scegliere il limite.** Si preferisce, in quest'ordine:
   una **risposta sbagliata** > un **muro su una forma frequente** > un muro su
   una forma esotica. Un limite che è *solo* una variante di superficie è il caso
   migliore: costa righe.
3. **Diagnosticare.** `/debug` sul turno — le sonde dicono la specie della
   lacuna (`turn_gap_kind`), il modulo che ha risposto, il fuoco, il registro. Poi
   `grep` per capire **se la conoscenza c'è già e manca il consumatore**: è stato
   il caso di /1 (`is_a_t/2` esisteva), /5 (`relation_noun/2` esisteva), /13 e /15
   (`answer_frame/2`, `extract_frame/2` esistevano). *Questo è il caso più comune
   di tutti.*
4. **Curare, in quest'ordine di preferenza.**
   1. righe di `turn_form*` / `*_cue` / `function_word` — **zero C**;
   2. una regola in un `.p0` che collega due conoscenze esistenti (/5 è così:
      quattro regole, zero C);
   3. una **primitiva generica** nel motore (un *piece*, un *atto*, una guardia
      letta dalla KB) — e allora la si documenta in [§4](#4-il-vocabolario-del-motore);
   4. una funzione dedicata — **solo se non c'è altro modo**, e dicendolo.
5. **Verificare.** Il prompt di partenza, più i vicini che devono restare
   invariati. Si riportano le risposte vere, non una parafrasi.
6. **Committare e pushare** — uno per giro. Il messaggio contiene il prompt
   *prima*, la cura, e le righe *dopo*. È il solo resoconto che sopravvive.
7. **Lasciare scritti i debiti.** Quello che non si è chiuso va nel messaggio o
   nel `.p0`, con le parole di chi l'ha trovato.

---

## 2. I 28 giri

| # | Il prompt che ha scoperto il limite | Che cosa mancava | Dove sta la cura |
|---|---|---|---|
| 1 | `is zelnik a thing?` (dopo «tool is a thing») | transitività dell'appartenenza | C generico |
| 2 | `how many tools are there?` | contare ciò che si sa (non solo ciò che è stato detto) | C + template |
| 3 | `list the tools` | chiedere un elenco | `enumerate_cue/1` |
| 4 | `zelnik, grum and pella are tools` | **fatto inventato**: «zelnik grum is a tool» | `expected_copula/3`, `plural_of/2` |
| 5 | `what colour is zelnik?` | due tabelle per la stessa domanda | **4 regole .p0, zero C** |
| 6 | `is zelnik red?` | attributo tenuto e non interrogabile | C + genere del valore |
| 7 | `what tools do you know?` | smalltalk rubava una domanda sulla KB | 3 righe `enumerate_cue` |
| 8 | `zelnik is bigger than grum` | comparativo come forma | `comparative_frame/2`, `relation_particle/1` |
| 9 | `tell me everything you know about zelnik` | formulazione senza porta | 4 `phrase_canon/2` |
| 10 | `give me an example of a tool` | un esempio è un elenco corto | `enumerate_limit/2` |
| 11–12 | `how many tools do you know?` · `is there a tool?` | elencare/contare/accertare | `enumerate_mode/2` |
| 13 | `where does zelnik live?` | relazione che porta un altro nome | C generico (+ particella dalla forma) |
| 14 | `does zelnik have a handle?` | `has`/`have`: due superfici, due esiti | KB + 3 difetti uso/menzione |
| 15 | `every tool has a handle` | ereditarietà per le relazioni | C generico |
| 16 | `actually zelnik is green` | **non lo si poteva correggere parlando** | `correction_cue/1` + orologio del turno |
| 17 | `actually …` | parola tolta due volte, e mai conservata | 1 riga |
| 18 | `why is zelnik useful?` | la ragione c'era, mancava la porta | `why_cue/1` |
| **19** | `smith does not owner zelnik` | **l'interprete di forme** | **`turn_form*` — la svolta** |
| 20 | `when was zelnik built?` | — | `bind`, atto `answer_relation` + righe |
| 21 | `where does zelnik come from?` | `come`→`how` senza guardia di lingua | righe + 1 riga C rimossa |
| 22 | `how much does zelnik weigh?` | — | **solo righe** |
| 23 | `what is zelnik used for?` | risposta sicura e fuori bersaglio | **solo righe** |
| 24–25 | `what can zelnik do?` | **fatto inventato da una domanda** | `turn_form_mood/2` |
| 26 | `which is bigger, zelnik or grum?` | scegliere fra due è un atto | atto `answer_choice` |
| 27 | `is zelnik bigger than pella?` | transitività — **insegnabile**, non cablata | `transitive_relation/1` + atto `assert_unary` |
| 28 | *(nessun prompt: richiesta di F.)* | 101 coppie di frasario nel motore | **le 101 righe in KB** |

Il gradiente è il contenuto di questa tabella: **i giri /20–/27 costano righe,
i giri /1–/18 costavano funzioni.**

---

## 3. L'interprete di forme — il motore che i giri devono usare

Una porta in C (`p0_turn_form_reader`, in `src/brain/10-memory-knowledge.c`) e
dietro tutte le forme che la KB dichiara. Il motore **non conosce nessuna forma,
nessuna parola, nessuna lingua**: enumera le forme dichiarate, fa combaciare i
pezzi in ordine, esegue l'atto che la KB ha attaccato alla forma.

```prolog
turn_form(Forma, Ordine, Pezzo).
turn_form_act(Forma, Atto).
turn_form_mood(Forma, question | statement).   % opzionale
turn_form_reply(Forma, NomeTemplate).          % opzionale
```

Esempio completo — **negare una relazione**, il primo muro chiuso senza C:

```prolog
turn_form(negate_relation, 1, slot(subject)).
turn_form(negate_relation, 2, class(negation_marker)).
turn_form(negate_relation, 3, relation(relation)).
turn_form(negate_relation, 4, rest(object)).
turn_form_mood(negate_relation, statement).
turn_form_act(negate_relation, assert_negative).
turn_form_reply(negate_relation, learned_negative_relation).
```

### Tre regole che costano tempo se non si sanno

1. **Una forma si dichiara su ciò che il LETTORE vede, non su ciò che
   l'interlocutore ha scritto.** «quanto pesa zelnik?» arriva canonicalizzato in
   «how much pesa zelnik?»: il quantificatore è parola funzione e viene tradotto,
   il verbo è contenuto e resta. (Giro /22.)
2. **Dichiarare sempre il `mood`.** Senza, una forma dichiarativa legge una
   domanda e parrot0 impara un fatto inventato. (Giro /24.)
3. **Quanti token valga un membro di `class(C)` è una proprietà del membro**, non
   del motore: `negation_marker("does not")` è *un* pezzo.

---

## 4. Il vocabolario del motore

Questo è ciò che si può usare **senza toccare il C**. Allungare questa tabella è
l'unica cosa che giustifica una modifica al motore — e va documentata qui.

### Pezzi

| pezzo | significato |
|---|---|
| `slot(N)` | un token, legato al nome `N` (salta un determinante in testa) |
| `rest(N)` | tutto il resto del turno |
| `class(C)` | qui stanno le parole di una classe KB `C/1`, anche di più token |
| `text("…")` | qui sta questa superficie, letterale |
| `relation(N)` | qui sta una relazione; risolta per tre strade (vedi sotto) |
| `bind(N, V)` | lega `N` a `V` **senza consumare token** — è come una forma dichiara la relazione che intende quando la frase non la nomina |

### Atti

| atto | slot richiesti | che cosa fa |
|---|---|---|
| `assert_relation` | `subject`, `relation`, `object` | posa il fatto |
| `assert_negative` | idem | posa il fatto negativo |
| `assert_unary` | `subject`, `class` | mette il soggetto in una classe |
| `answer_relation` | `subject`, `relation` | legge e rende i valori |
| `answer_choice` | `relation`, `first`, `second` | quale dei due sta nel primo posto; se nessuno, **non indovina** |

### Le tre strade per risolvere una superficie in relazione

In quest'ordine, ed è l'ordine giusto: `relation_verb/1` → `verb_stem/2` →
`answer_frame/2` e le teste di `extract_frame/2`. La terza è quella che i giri
/13, /14 e /19 hanno dovuto aggiungere. **Una superficie può avere più letture
dichiarate e prenderne la prima è un errore**: si provano tutte e risponde quella
che ha un fatto (giro /14).

### Guardie lette dalla KB

`transitive_relation/1` (si percorre la catena) · `correction_cue/1` (un secondo
valore sostituisce invece di aggiungere) · `canonicalization_exempt/1` +
`evidence_extent/2` (uso vs menzione) · `enumerate_mode/2` ·
`relation_particle/1` · `function_word/2,3`.

---

## 5. I 22 che restano

### Già trovati e non chiusi — si parte da qui

| prompt | che cosa manca |
|---|---|
| `which is the biggest tool?` | superlativo su una relazione d'ordine |
| `which tools are bigger than pella?` | **risposta sbagliata**: elenca tutti i tool, compreso pella |
| `sort the tools by size` | ordinare una classe per una relazione |
| `who owns zelnik?` | `owns` non si lega a `owner` quando la catena è a due salti |
| `zelnik is owned by smith` | il passivo di una relazione insegnata |
| `owns is another form of own` | **non si può insegnare una flessione** |
| `a cosa serve X?` · `X serve a Y` | collisione con due lettori esistenti (giro /23) |
| `X is transitive` | collisione con l'elenco delle transitive (giro /27) |
| `chi ha scritto amleto?` | cade anche su `HEAD` — precedente alla campagna |
| `Got it: THE zelnik is red` | articolo di troppo su un'entità nominata (giro /6) |
| «risoluzione superficie→relazione scritta in tre posti» | tre modi di fare la stessa domanda — è una potatura |

### Come trovarne altri

Il banco che ha prodotto i 28: si prende una **relazione nuova e inventata** (così
niente risponde per caso dalla conoscenza del mondo) e si chiude il giro completo
intorno a lei —

```
zelnik is a tool                  asserire una classe
a tool is a thing                 una classe dentro una classe
zelnik <rel> grum                 asserire una relazione
<rel> chains                      dichiararne una proprietà
does zelnik <rel> grum?           verificarla
who <rel> grum?                   interrogarla dall'altro verso
what does zelnik <rel>?           enumerarla
zelnik does not <rel> grum        negarla
every tool <rel> grum             l'universale
actually zelnik <rel> pella       correggerla
why does zelnik <rel> grum?       spiegarla
how many tools / is there a tool  aggregarla
```

Ogni casella vuota è un giro. Le caselle di **questa** griglia non sono finite:
mancano il tempo (`when`), il luogo (`where`), il modo (`how`), la quantità
(`how much`) su relazioni insegnate, e la stessa griglia **in italiano** — dove
il giro /22 ha mostrato che la forma va scritta su ciò che il lettore vede.

---

## 6. Debiti dichiarati in questa campagna

- `tests/p0t/code/segment.p0t` si pianta — **precedente**, verificato su `HEAD`.
- Le forme italiane di `purpose_*` e la superficie «X is transitive» non sono
  dichiarate: il turno è già di altri lettori, e non si ruba (§0.5).
- `p0_learn_attribute` decide la sostituzione su una sola relazione per volta:
  una proprietà a più valori legittimi (un colore *e* un materiale) è gestita,
  due valori della *stessa* no per costruzione. È voluto, e va detto.
- I giri /1–/18 restano come sono (§0.5). Chi li riprende, li riprende per
  **portarli nell'interprete di forme**, non per riscriverli.
