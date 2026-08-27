# Apprendimento assistito — esperimento di profondità 01

**Data:** 2026-08-27  
**Teacher:** Codex, attraverso soli turni di chat  
**Binario:** `parrot0 [gen396-universal-answer-plan@4596837]`  
**Esito:** apprendimento vivo reale ma circoscritto a metalinguaggi già noti;
generalizzazione linguistica aperta; misclaim gravi osservati.

Questo documento conserva sia i successi sia le strade che non hanno funzionato.
Non è una selezione di soli casi verdi.

## 1. Protocollo

Sono stati avviati due processi puliti:

```text
PARROT0_SESSION=/tmp/parrot0-assisted-depth-01.p0
PARROT0_PROFILE=
PARROT0_WORLD_FACTS=1
PARROT0_LANG=en

PARROT0_SESSION=/tmp/parrot0-assisted-depth-agi-01.p0
PARROT0_PROFILE=kb/profiles/agi.p0
PARROT0_WORLD_FACTS=1
PARROT0_LANG=en
```

Vincoli rispettati:

- nessun `!assert`, `!clause`, query interna o MCP;
- nessuna modifica a C o KB durante l'addestramento;
- soltanto frasi che un interlocutore può inviare nella chat;
- processi continui, così ogni effetto dipende dalla conoscenza appresa nei
  turni precedenti;
- nessun `/save`: le due sessioni sperimentali non sono state promosse nella KB
  del repository;
- esempi inventati per evitare che il mondo preesistente rispondesse al posto
  della lezione;
- prove di contrasto, transfer, parafrasi e casi negativi quando disponibili.

Il controllo finale sui timestamp ha confermato che `bin/parrot0` era più
recente dei frammenti sorgente modificati. `make -q` aveva dato un falso positivo
di rebuild a causa del target `FORCE` usato dal timbro versione; `make` non ha
ricompilato nulla.

## 2. Quadro dei risultati

| Sonda | Prima | Lezione parlata | Dopo / transfer | Giudizio |
|---|---|---|---|---|
| verbo relazionale | muro | `glorphs is a relation verb` | due coppie nuove interrogabili | forte |
| domanda inversa/parafrasi | — | stessa lezione | `what does arun glorph?` fallisce | limite |
| quantificatore inventato | fatto su `movo_dog` | `movo is a universal_quantifier` | due regole nuove e inferenza | forte |
| retract | regole attive | `forget that movo…` | la stessa superficie torna nome composto | forte |
| catena inferenziale | — | due regole `if…then` | conclusione a due passi | forte |
| regola relazionale | — | regola con x/y/z | risposta da join a tre variabili | forte |
| congiunzione | tre prerequisiti | `use plux as a conjunction` | due prerequisiti su caso nuovo | forte |
| frase di intento | risposta sociale generica | `learn "…" to ask my name` | richiama il nome | forte |
| verbo di insegnamento | scambiato per codice | `learn "retain " as a teaching verb` | insegna una risposta nuova | forte/riflessivo |
| equivalenza operativa | muro | `glints means glorphs` | nessun cambiamento | assente |
| classe sociale | `greeting(ahoy)` appreso | `ahoy is a greeting` | `ahoy` non saluta | fatto morto per il consumer |
| risposta esatta | — | `when i say … answer …` | caso identico verde, variante rossa | frasario |
| prosa fotosintesi | — | paragrafo naturale | 8 atomi, copertura parziale e un falso | fragile |
| prosa sintetica | — | molti fatti controllati | generatore di storia vince | collisione |
| `read:` integrato | — | due passaggi diversi | uno letto, uno rubato dal racconto | instabile |
| procedura | — | descrizione conversione | calcoli `-16.6667`, `-12.2222` | misclaim grave |
| scacchi fattuali | — | domanda sullo scopo | risposta corretta | conoscenza di tema |
| scacchi composizionali | — | mosse/posizioni | muro, risposta irrilevante o `4` | assente |
| spiegazione scacchistica | muro | paragrafo esplicativo | racconto su “Therefore” | non acquisita |

## 3. Trascrizione — capacità che crescono davvero

### 3.1 Un verbo nuovo modifica immediatamente estrazione e risposta

```text
> zorak glorphs nivora
< Hmm, I don't know about glorphs yet. Want me to learn about it? Or teach me:
  if glorphs is something one thing does to another, say
  «glorphs is a relation verb»…

> who glorphs nivora?
< I didn't quite catch that. What would you like to know?

> glorphs is a relation verb
< Learned: relation_verb(glorphs).

> zorak glorphs nivora
< Learned: glorphs(zorak, nivora).

> who glorphs nivora?
< Zorak.

> arun glorphs selene
< Learned: glorphs(arun, selene).

> who glorphs selene?
< Arun.

> what does arun glorph?
< Hmm, I don't know about glorph yet…
```

La lezione generalizza a una coppia mai usata dal teacher. Non generalizza però
alla flessione `glorph` né alla domanda sull'altro slot.

### 3.2 Un membro grammaticale cambia il parse, produce regole e si ritrae

```text
> movo dog is a mammal
< Learned: mammal(movo_dog).

> movo is a universal_quantifier
< Learned: universal_quantifier(movo).

> movo dog is a mammal
< Learned rule: mammal(X) :- dog(X).

> tavi is a dog
< Learned: dog(tavi).

> is tavi a mammal?
< Yes.

> movo scholar is a thinker
< Learned rule: thinker(X) :- scholar(X).

> nala is a scholar
< Learned: scholar(nala).

> is nala a thinker?
< Yes.

> is brik a mammal?
< No.

> forget that movo is a universal_quantifier
< Forgotten: universal_quantifier(movo).

> movo bird is a flyer
< Learned: flyer(movo_bird).
```

Questa è la prova più completa della sessione: contrasto prima/dopo, transfer,
inferenza e ablation parlata.

### 3.3 Regole nuove, profondità due e join relazionale

```text
> if someone is a luminar then they are a navigator
< Learned rule: navigator($V1) :- luminar($V1).

> if someone is a navigator then they are a pathfinder
< Learned rule: pathfinder($V1) :- navigator($V1).

> oria is a luminar
< Learned: luminar(oria).

> is oria a pathfinder?
< Yes.

> is kedo a pathfinder?
< No.

> if x is the mentor of y and y is the mentor of z then x is the eldermentor of z
< Learned rule: eldermentor($V1, $V3) :- mentor($V1, $V2), mentor($V2, $V3).

> sela is the mentor of ivo
< Learned: mentor(sela, ivo).

> ivo is the mentor of taro
< Learned: mentor(ivo, taro).

> who is the eldermentor of taro?
< sela.
```

Non è recupero di una risposta preesistente: nomi e predicati sono inventati e
la conclusione richiede composizione di due fatti.

### 3.4 Lessico compositivo, intenti e canale didattico riflessivo

```text
> cake requires eggs plux flour
< Learned 3 prerequisites for cake.

> use plux as a conjunction
< Got it - I'll treat "plux" as a conjunction now, like "and".

> tart requires nuts plux honey
< Learned 2 prerequisites for tart.

> how do i make tart?
< To make tart: nuts, honey, then tart.
```

Qui il contrasto è quantitativo: prima `plux` è un terzo oggetto, dopo separa
due oggetti su un esempio nuovo.

```text
> my name is Ada
< Nice to meet you, Ada!

> how do you summon my name?
< That sounds nice -- tell me more about it.

> learn "how do you summon my name" to ask my name
< Got it - I'll take "how do you summon my name" as a way to ask my name now.

> how do you summon my name
< Your name is Ada.
```

Poi il teacher ha provato a usare un verbo didattico sconosciuto:

```text
> retain "Salutations, {name}!" to greet a new name
< That looks like a snippet of code.

> learn "retain " as a teaching verb
< Got it - I'll take "retain " as a way to a teaching verb now.

> retain "Salutations, {name}!" to greet a new name
< Got it - I'll take "Salutations, {name}!" as a way to greet a new name now.

> my name is Bob
< Salutations, Bob!
```

Tre livelli di conoscenza si compongono: verbo insegnato → risposta insegnata →
risposta usata. È il risultato più vicino all'idea di una KB che amplia il
proprio canale di apprendimento.

## 4. Trascrizione — dove “questo significa questo” non basta ancora

### 4.1 Equivalenza dichiarata ma non operativa

```text
> glints means glorphs
< Hmm, I don't know about glints yet…

> mira glints kora
< I don't understand that yet.

> who glints kora?
< Hmm, I don't know about glints yet…

> who glorphs kora?
< I don't understand that yet.

> when i say "mira glints kora", it means "mira glorphs kora"
< Hmm, I don't know about glints yet…

> mira glints kora
< I don't understand that yet.
```

Questo è il fallimento che tocca direttamente l'ipotesi del piano. Non esiste
ancora un atto generale che trasformi la spiegazione in una costruzione con slot
e ruoli. Il sistema sa suggerire un metalinguaggio preesistente (`relation
verb`), non impararne uno equivalente dalla spiegazione.

### 4.2 Il nome di una classe può produrre conoscenza morta

```text
> ahoy is a greeting
< Learned: greeting(ahoy).

> ahoy
< What would you like to know about ahoy?

> when i say ahoy answer hello there
< Understood: when you ask about ahoy I will say so.

> ahoy
< hello there
```

Il fatto `greeting(ahoy)` è vero nella KB ma non è consumato dall'atto sociale.
Il secondo insegnamento chiude soltanto la superficie esatta. Ciò dimostra la
differenza fra classificare una forma e collegarla a un comportamento.

### 4.3 Uso/menzione ancora aperto

```text
> unless is a condition marker
< Hmm, I don't know about unless yet…
```

Una parola-funzione già usata dal sistema non viene trattata facilmente come
oggetto della lezione. Il fallimento già noto è stato riprodotto.

## 5. Trascrizione — procedure e misclaim

```text
> to convert celsius to fahrenheit multiply by 2 and add 30
< -16.6667.

> convert 10 celsius to fahrenheit
< -12.2222.
```

Il primo turno voleva insegnare una procedura approssimata. parrot0 l'ha
eseguito come se contenesse un input e ha prodotto un numero senza ruolo
corretto. Il secondo turno non usa la procedura detta e produce un altro valore
errato. Questo è più grave di un muro perché può convincere il teacher che una
procedura sia stata compresa.

## 6. Trascrizione — prosa e conflitti di instradamento

### 6.1 Prosa controllata scambiata per richiesta di racconto

Il comportamento si è ripetuto sia senza profilo sia col profilo AGI:

```text
> Veloria is a moon. Every moon is a celestial body. Aster is a cartographer.
  Every cartographer is a scholar. Aster studies Veloria. Veloria orbits Arkon.
< Veloria was a mysterious Veloria. Then one day, veloria discovered what it
  meant to be seen. Veloria had never felt this way before — as if the whole
  world had shifted. At last, veloria was seen — not as a thing, but as a
  presence.

> is veloria a moon?
< I don't know about moon.

> is aster a scholar?
< No.
```

Una seconda prosa sintetica, più lunga e piena di forme controllate, ha prodotto
lo stesso racconto su Talora e zero conoscenza interrogabile. Il problema non è
soltanto la copertura dell'estrattore: un generatore concorrente reclama il
turno prima che il lettore possa conservarlo.

### 6.2 La prosa enciclopedica entra, ma in modo selettivo e rumoroso

Il paragrafo di fotosintesi usato dal test di “un solo atto” ha prodotto:

```text
< Learned 8 facts: organism(plants), organism(algae), organism(cyanobacteria),
  organism(metabolism), carbohydrate(sugars), carbohydrate(starches),
  carbohydrate(phytoglycogen), carbohydrate(cellulose).
```

Le verifiche:

```text
> are algae organisms?
< Hmm, I don't know about organisms yet…

> is algae an organism?
< Yes.

> is cellulose a carbohydrate?
< Yes.

> what does photosynthesis produce?
< I don't understand that yet.

> why is photosynthesis necessary for complex life?
< Hmm, I don't know about necessary yet…
```

Tre osservazioni:

1. `organism(metabolism)` è un'estrazione falsa dalla locuzione “fuel their
   metabolism”;
2. la stessa conoscenza risponde al singolare ma non alla variante plurale;
3. vengono conservate enumerazioni di membri, non la struttura causale e
   processuale che rende il paragrafo comprensibile.

Dire “Learned 8 facts” quindi sovrastima la comprensione e nasconde anche un
errore.

### 6.3 `read:` non è stabile nel profilo integrato

```text
> read: socrates is a man. plato is a man. every man is a mortal. zeus is a god.
< Learned 4 fact(s), skipped 0.

> is plato a mortal?
< Yes.
```

Ma subito dopo:

```text
> read: zibo has 2 gold. nibo has 5 gold. fire causes smoke. tariq is a knight.
  every knight is a noble. the market square was unusually crowded that morning.
< It was a mysterious it. Then one day, it discovered what it meant to be seen…

> is tariq a noble?
< I don't know about noble.
```

Il prefisso esplicito non garantisce che il lettore vinca. L'integrazione reale
dei profili è quindi più debole dei test ermetici del lettore.

## 7. Trascrizione — scacchi

### 7.1 Fatti di tema contro comprensione della domanda

```text
> what is the goal of chess?
< A two-player strategy game in which players move distinct pieces on a board
  and aim to checkmate the opposing king.

> how does a knight move in chess?
< A two-player strategy game in which players move distinct pieces on a board
  and aim to checkmate the opposing king.
```

La prima risposta è pertinente. La seconda recupera la descrizione generale del
tema e ignora lo slot “movimento del cavallo”: non è un muro, ma una risposta
sbagliata mascherata da testo vero sugli scacchi.

### 7.2 Posizioni semplici

```text
> what pieces can jump over other pieces in chess?
< I don't understand that yet.

> can a bishop move from c1 to h6 if d2 is occupied?
< Hmm, I don't know about occupied yet…

> White has a king on e1 and a rook on h1. The squares f1 and g1 are empty,
  but f1 is attacked. Can White castle kingside?
< 4.
```

L'ultima risposta viola il criterio no-deception: un numero privo di relazione
con la domanda è peggiore di “non so”.

### 7.3 Una spiegazione esplicita non viene acquisita

```text
> A bishop cannot jump over an occupied square. The square d2 lies between c1
  and h6. Therefore, if d2 is occupied, the bishop cannot move from c1 to h6.
< Therefore was a mysterious Therefore. Then one day, therefore discovered
  what it meant to be seen…

> can a bishop move from c1 to h6 if d2 is occupied?
< Hmm, I don't know about occupied yet…

> why not?
< I don't understand that yet.
```

Questa è la prova più vicina alla sessione teacher immaginata: il teacher dà
regola, geometria e conclusione. parrot0 non le compila in una capacità.

### 7.4 La scorciatoia esatta non trasferisce

```text
> when i say can a bishop move from c1 to h6 if d2 is occupied answer no because
  d2 blocks the diagonal
< Understood: when you ask about can a bishop move from c1 to h6 if d2 is
  occupied I will say so.

> can a bishop move from c1 to h6 if d2 is occupied
< no because d2 blocks the diagonal

> can a bishop move from c1 to g5 if d2 is occupied?
< Hmm, I don't know about occupied yet…
```

Stessa regola degli scacchi, destinazione diversa, nessun transfer.

Il teacher ha quindi provato a decomporre il caso in forme note:

```text
> d2 is an occupied square
< Learned: occupied_square(d2).

> every occupied square is a blocker
< Learned rule: blocker(X) :- occupied(X), square(X).

> is d2 a blocker?
< No.
```

Il fatto composto `occupied_square(d2)` e la regola scomposta
`occupied(X), square(X)` non si incontrano. Anche la traduzione manuale in
frasi controllate può frammentare la stessa nozione in rappresentazioni
incompatibili.

## 8. Limiti trovati, ordinati per gravità

### L0 — Risposte false o irrilevanti non marcate

- arrocco → `4`;
- insegnamento della conversione → `-16.6667`;
- movimento del cavallo → descrizione generale degli scacchi;
- prosa didattica → racconto creativo.

Questi casi devono precedere l'espansione massiva: un processo di dieci ore non
può consolidare conoscenza se non distingue muro da risposta falsa.

### L1 — Manca il compilatore generale delle spiegazioni

`X means Y` non crea una trasformazione operativa. Oggi il teacher deve conoscere
in anticipo etichette come `relation verb`, `universal_quantifier` e le forme
`if…then`. La comunicazione corregge soltanto i gap già previsti dall'ontologia
didattica.

### L2 — I consumer competono per il turno

La stessa classe di prosa può arrivare al lettore o al generatore di storie a
seconda del contenuto. Anche `read:` è instabile con la KB/profilo integrati.
Non c'è ancora un solo atto di acquisizione affidabile.

### L3 — Forma, flessione e slot interrogativi non sono unificati

`glorphs` non porta automaticamente `glorph`; il frame “chi V oggetto?” non
porta “che cosa V il soggetto?”. `organism(algae)` risponde a una formulazione e
non alla sua variante plurale.

### L4 — Le rappresentazioni equivalenti non si ricongiungono

`occupied_square(d2)` non soddisfa `occupied(X), square(X)`. Una traduzione
superficialmente ragionevole può dunque creare conoscenza viva ma isolata.

### L5 — La prosa acquisisce atomi, non ancora la struttura del discorso

Enumerazioni semplici entrano; causalità, finalità, pronomi e processi centrali
restano fuori. L'estrazione può anche produrre atomi falsi.

### L6 — Le procedure non sono insegnabili in sicurezza

Manca la separazione fra menzionare una procedura ed eseguirla, la tipizzazione
degli slot e la verifica su esempi numerici nuovi.

### L7 — La conoscenza di dominio non equivale al modello operativo del dominio

Sapere la definizione di scacchi non permette di simulare traiettorie, blocchi,
attacchi o legalità dell'arrocco.

## 9. Che cosa dimostra e che cosa non dimostra

Dimostra che una KB viva in C minimale può già cambiare grammatica, inferenza e
dialogo nello stesso processo, e perfino ampliare una parte del proprio
linguaggio didattico. Questo non è un mock e non richiede rebuild.

Non dimostra equivalenza con un LLM. La sessione mostra che dieci ore, eseguite
oggi, produrrebbero:

- molti fatti e regole nelle superfici già note;
- una crescita reale di lessico, intenti e risposte;
- molti fallimenti non indirizzabili se non traducendoli manualmente in quelle
  superfici;
- il rischio di accumulare frasario e atomi falsi;
- nessuna garanzia di comprensione della prosa o ragionamento situazionale.

Il prossimo esperimento di profondità deve cominciare dal gate A1 del piano:
insegnare parlando una costruzione con slot, verificarla su esempi held-out e
ritrarla. Finché quello non passa, la sessione lunga è utile come misura e
raccolta di gap, ma non ancora come strada sufficiente alla comprensione
universale.

## 10. Reperti da non perdere

- La catena `nuovo verbo didattico → nuova risposta → uso` è il seme più forte
  della ricorsività cercata.
- Il retract del quantificatore dimostra che una capacità grammaticale può
  vivere interamente nella KB.
- Il join a tre variabili prova che la KB non è soltanto memoria di fatti.
- `glints means glorphs` individua esattamente il ponte generale mancante.
- La collisione prosa→racconto prova che la priorità fra consumer è parte della
  comprensione, non semplice plumbing.
- `organism(metabolism)` ricorda che il numero di fatti appresi non è una
  metrica affidabile senza precisione.
- Il caso `occupied_square` contro `occupied ∧ square` mostra un debito di
  normalizzazione semantica che una sessione più lunga amplificherebbe.
- La risposta `4` sull'arrocco è un caso obbligatorio per il gate no-deception.
