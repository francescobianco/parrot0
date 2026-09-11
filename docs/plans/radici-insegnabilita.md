# Le radici dell'insegnabilità

11 settembre 2026. Concetto posto da F. durante il gen510, sviluppato qui come
metodo di lavoro. Si affianca a [LEARN_PROTOCOL.md](../../LEARN_PROTOCOL.md)
(§«Le radici dell'insegnabilità»), a
[50-iterazioni-insegnabilita.md](50-iterazioni-insegnabilita.md) e al
[quadro preliminare della KB viva](quadro-preliminare-kb-viva.md) §5.7.

> *«preso una qualsiasi cosa che può essere insegnata a parrot0 si può
> individuare la superficie che la insegna e chiedersi quale superficie serve
> per insegnare quella abilità di insegnamento, e così facendo a ricorsione
> verso il prerequisito di insegnabilità necessario per quello corrente si
> costruisce una catena di superfici di insegnabilità […] noi abbiamo un
> vantaggio: possiamo fissare il punto di inizio»* — F., 2026-09-11

## 1. La domanda che si ripete

Ogni cosa che parrot0 sa fare ha, oppure non ha, una **superficie** che la
insegna: una frase in lingua naturale che, detta, produce quella capacità.

Per ogni superficie si può fare la stessa domanda un gradino sopra: **con quale
superficie si insegna a parrot0 a riconoscere e usare questa superficie?** La
risposta è a sua volta una superficie, oppure un buco. E la domanda si ripete.

```text
abilità A
  ← superficie S1 che insegna A                 («treat as a question any turn that contains X»)
    ← superficie S2 che insegna S1               (quale lezione crea una forma di lezione?)
      ← superficie S3 che insegna S2
        ← …
```

La sequenza è una **catena di insegnabilità**. Il suo interesse sta in come
finisce.

## 2. Come può finire una catena

| Fine | Che cosa significa | Stato |
|---|---|---|
| **Radice** | si arriva a una primitiva del motore: tokenizzare, legare uno slot, eseguire un'operazione sulla KB, `apply/2`, l'unificazione | legittima: il C *deve* esistere, e la catena può fermarsi solo qui |
| **Circolo** | la catena torna su una superficie già attraversata: una lezione che sa estendere anche la propria forma | il caso migliore: la classe si nutre da sé |
| **Riga a mano** | si arriva a una clausola `.p0` o a un ramo C che nessuna superficie produce | un **buco**: è il prossimo lavoro, non una fine |

Una KB viva è una KB in cui ogni catena finisce in una radice o in un circolo.
Una catena che finisce in una riga a mano dice esattamente dove parrot0 è
ancora un programma scritto da qualcuno invece di un sistema che impara.

Le catene non sono lineari. Una superficie può dipendere da più prerequisiti
(una forma di lezione chiede il riconoscimento del proprio modo, degli slot e
del proprio atto), e più abilità possono condividere lo stesso prerequisito.
Il risultato è un **grafo di insegnabilità**, con radici, circoli chiusi e
cammini articolati.

## 3. Il vantaggio: il punto di partenza è fissato

Non si parte dal vuoto a indovinare quali siano le superfici elementari.
**Ogni abilità che parrot0 ha già è un punto di partenza**, perché dimostra che
la capacità esiste e quindi che una catena, scritta a mano o no, la sostiene.

Da qui il ruolo nuovo della suite e dei piani:

- **ogni asserzione dei `.p0t`** è un'abilità esistente;
- **ogni forma del catalogo** (LEARN_PROTOCOL §6-bis) è una superficie esistente;
- **ogni ramo C che decide qualcosa** è un'abilità che oggi nessuna lezione
  potrebbe ricreare.

La domanda per ciascuna non è «funziona?», che la suite già misura, ma
**«parrot0 potrebbe riapprenderla da una lezione di ordine superiore?»**. Se la
risposta è no, la catena finisce in una riga a mano, e si risale.

È il senso di *rendersi vivo*: riapprendere quello che sa fare. Una capacità
che parrot0 ha soltanto perché qualcuno l'ha scritta è presente ma non viva.

## 4. Due esempi lavorati nel gen510

### 4.1 Riconoscere una domanda

| Gradino | Superficie o meccanismo | Fine |
|---|---|---|
| A: «questo turno è una domanda» | `turn_declared_act(T, question)` (turn-frames.p0) | — |
| S1: insegnare un indizio di domanda | «treat as a question any turn that contains X» → `taught_question_cue/1` | superficie presente |
| S2: insegnare la forma stessa di S1 | le righe `turn_form(teach_question_cue, …)` in messages.p0 | **riga a mano** |
| S2′: insegnare un altro modo di dire S1 | «"X" is another way to say "Y"» scrive `phrase_canon/2`, e la frase nuova si legge come S1 | **circolo**: la lezione di parafrasi estende anche la propria superficie |
| S3: insegnare un pezzo di forma nuovo (`span`, `rest`, …) | nessuna superficie | **radice**: la meccanica degli slot è del motore |

Lettura: il riconoscimento delle domande è ora insegnabile (S1). Una forma di
lezione **nuova**, con una struttura che nessuna forma esistente ha, resta una
riga a mano (S2). Una **variante** di una forma che esiste è già insegnabile
parlando (S2′). Il prossimo buco da aprire è S2: una superficie che dichiara
una forma di lezione dicendone i pezzi in lingua naturale.

La stessa analisi ha mostrato un difetto concreto: la lezione S1 detta come
«if a turn contains X then it is a question» viene presa dal lettore delle
regole «if … then». Una catena di insegnabilità vale solo se ogni suo anello
**raggiunge il proprio lettore**; la cattura del turno è un anello rotto.

### 4.2 Una relazione definita due volte

| Gradino | Superficie | Fine |
|---|---|---|
| A: «grandparent» come genitore del genitore | `relation_twice(grandparent, parent)` | — |
| S1: «V is W twice» | superficie gen507/86 | superficie presente, scritta a mano |
| S1′: la stessa capacità da una lezione di ordine superiore | «doubled x is x followed by x», poi «grandparent is doubled parent» | **ri-apprendibile**: la costruzione parametrica del gen508 ricrea S1 senza la sua riga |

Qui la catena mostra un'abilità che **si può già riapprendere**: la superficie
«twice» è diventata una comodità, non una necessità. Il gen508 l'ha resa
derivabile.

### 4.3 Un anello rotto per un intero gradino

Il gen510 ha trovato un difetto che rompeva **tutte** le catene di un tipo: nel
lettore delle forme, il limite di uno `slot` si calcolava prima di saltare
l'articolo iniziale. Ogni forma che apre con uno slot falliva in silenzio
davanti a un soggetto con l'articolo — «a cliff is a kind of height», «the box
contains the ring». Le forme esistevano, le lezioni sembravano insegnabili, e
non lo erano. Un censimento che controlla solo che la forma ci sia non lo vede;
lo vede il passo 5 del metodo, **parlando**. La traccia `P0_READ_TRACE=1`
(`[form] matched …`) dice ora quale forma combacia con un turno.

### 4.4 Il gradino S2 aperto (gen511): una lezione che crea una forma di lezione

Il buco ad alta leva del §4.1. Prima: «S means T» esisteva solo per i fatti
(le costruzioni), e con un bersaglio che è una lezione declinava — «I cannot
anchor that lesson yet … for ?». Ora, se una forma esistente legge T, la
lezione crea una forma nuova **nella stessa rappresentazione** di quelle
scritte a mano (`turn_form/3`): i pezzi letterali di S diventano `text`, ogni
variabile (`rule_variable/1`) `span` o `rest`, e l'atto è una sola primitiva
nuova, `reread(T)` — ridirsi T con i buchi riempiti, come turno annidato nella
lingua del discorso. Si disfa con «forget that S means T».

| Gradino | Superficie | Fine |
|---|---|---|
| A: «questo turno è una domanda» | `turn_declared_act(T, question)` | — |
| S1: «treat as a question any turn that contains X» | forma scritta a mano | superficie |
| S2: **«x counts as a question means treat as a question any turn that contains x»** | crea `taught_form_N` con atto `reread(…)` | **aperto** |
| S3: «x is interrogative means x counts as a question» | una forma che punta a una forma INSEGNATA | **circolo** |
| la primitiva `reread` | rileggere una frase come turno | **radice** del motore |

Prova: `tests/p0t/language/taught_lesson_form.p0t` (prima/dopo, circolo,
un'altra famiglia, ablazione, R3; idempotente).

Due anelli rotti trovati e chiusi facendolo, entrambi del tipo del §5 passo 5:
la lezione del circolo **finiva con la superficie della forma appena
insegnata** e quella la leggeva per prima (una lezione sulle forme ora precede
l'uso di una forma); e il declino nominava «?» invece del bersaglio.

**Il censimento, parlando** (34 superfici del catalogo §6-bis, ognuna
bersaglio di una lezione «x qq y means …»):

| Esito | N | Superfici |
|---|---|---|
| **raggiunte**: la catena diventa un circolo | 24 | `x is not a y`, `kind of`, `are typically`, otto proprietà delle relazioni, plurale, passato, `the italian for`, sigla, contrario, stessa cosa, definizione, `step for`, l'indizio di domanda, quattro superfici di ordine superiore |
| già circolo per costruzione di fatti | 1 | `x is a member of y` |
| **la catena finisce nel C** | 9 | `x is a y`, `every x is y`, `every x has y`, `no x is a y`, `x is a relation verb`, `correction: x is y`, `forget that x`; e `when x then y`, `your plan when x?` |

Le ultime due sono un **artefatto della verifica**: le forme dei piani hanno un
pezzo `named(situazione)` che una variabile non soddisfa. Le altre sette sono
lette da moduli compilati, non da forme: sono l'elenco preciso delle abilità
di lezione ancora **congelate nel C** — e la famiglia delle classi è la più
usata di tutte.

Vitalità misurata su questo campione: **25/34 catene finiscono in un circolo**
(prima di S2: 1/34).

**Secondo giro: «so leggerlo?» invece di «quale forma lo legge?».** Portare in
una forma ognuno dei lettori compilati sarebbe stata una seconda lettura
(mantra #5). La domanda generale ha una risposta generale: **provarci**. Se
nessuna forma legge il bersaglio, parrot0 lo legge in un processo figlio
(`p0_try_reading`: fork, copia in scrittura, un byte di esito, `_exit`), con
parole nuove al posto delle variabili. È una seconda primitiva-radice, senza
vocabolario, e serve a qualunque futura «lettura ipotetica».

Esito: **30/34**. La famiglia delle classi (`x is a y`, `every x is y`,
`every x has y`, `no x is a y`) e `x is a relation verb` sono ora bersagli.
Le quattro rimaste hanno la stessa forma: **presuppongono un referente** —
`correction: x is y` un fatto da correggere, `forget that x` qualcosa da
dimenticare, `when x then y` e `your plan when x?` una situazione nota. Con
parole nuove non c'è niente su cui agire e la prova mura: è un limite della
verifica con parole fresche, non del circuito. Chiuderlo chiede di provare la
lettura in un contesto che contenga il referente (un «supponiamo che …» della
prova), cioè il prossimo gradino.

Costo: ~1,4 s per una lezione che non ha una forma lettrice, soltanto quando si
insegna.

### 4.5 Un buco vale per ciò che la forma dichiara in quel posto (gen512)

Il punto 1 della coda del gen511: le quattro lezioni che **presuppongono un
referente**. La diagnosi, fatta leggendo le forme invece di aggiungere una
«preparazione» per ciascuna: tre delle quattro sono lette da **forme**, e la
verifica statica falliva per due ragioni meccaniche, non di contenuto.

- Un pezzo `named(situation_cue, …)` non accettava una variabile: il buco non è
  una situazione nota, e con parole nuove non lo diventa mai. Ma il buco **non
  è una parola**: è il posto dove chi userà la forma metterà la sua, e la forma
  stessa dice che cosa ci può andare. Quindi, in verifica, una variabile copre
  un pezzo `named`.
- «forget that x»: la `x` finale deve coprire soggetto, relazione e oggetto.
  L'ultimo buco copre il resto della forma, come l'ultima variabile della
  sorgente ne prende il resto.

Una radice nuova del motore, senza vocabolario: la **modalità buchi** di
`p0_form_match` (`p0_form_holes`), accesa solo mentre si verifica un bersaglio.

**Tre corrispondenze spurie, trovate guardando anche gli «ok»** (mantra #9 — il
circuito rispondeva bene, per il motivo sbagliato). Ognuna è una regola del
buco, e insieme dicono che cosa un buco è:

| Il bersaglio «correction: x is y» combaciava con | perché | la regola |
|---|---|---|
| `negate_relation` | `x` copriva `class(negation_marker)` | in mezzo, un buco sta solo per un **referente** (`named`), mai per una parola della grammatica (`class`, `relation`) |
| `teach_rel_chain` | `y` finale copriva anche `text("followed by")` | l'ultimo buco copre il resto, ma **non un'ancora**: un `text` che la forma esige deve stare nel bersaglio |
| `teach_def_use` | «correction: x» letto come la *definizione* di una costruzione | nel bersaglio una variabile è un buco, non la variabile di una definizione |

Chiuse le tre, «correction: x is y» **declina di nuovo, onestamente**: la legge
un modulo compilato che vuole un valore noto (anche con il referente preparato,
«qzxa is qzxc» mura). È la sola catena delle 34 che finisce ancora nel C, e la
sua via d'uscita è il punto 5 della coda: portato il lettore degli attributi in
una forma con un pezzo `named` sul valore, la modalità buchi lo coprirà senza
altro lavoro.

| | gen511 | gen512 |
|---|---|---|
| catene che finiscono in un circolo, sulle 34 del catalogo | 30/34 | **33/34** |

**Due anelli rotti a valle, trovati usando le forme sul serio.**

1. L'ultima variabile della sorgente diventava `rest(x)`, e `rest` salta
   l'articolo iniziale (gen510, per i soggetti delle forme a mano): «what about
   the recipe is missing» si ridiceva «your plan when recipe is missing», e la
   situazione nota non combaciava più. Ora resta uno `span`, come tutte le
   altre: ciò che verrà ridetto si conserva come il lettore lo vedrà.
2. `reread` prendeva il turno anche quando la frase ridetta murava: una
   sorgente larga avrebbe restituito il muro di una frase che l'interlocutore
   non ha detto. Ora la rilettura rivendica il turno solo se tiene, con lo
   stesso criterio della prova nel figlio (`reply_is_wall`).

**Il segno d'uso reale** (il passo 7 del §5, nato in questo giro): due lezioni
vere insegnate in `make chat`, usate su conoscenza reale, salvate e
riverificate in un processo nuovo — «what is your plan when x means your plan
when x?» (il piano vero di parrot0, chiesto anche con «you have no steps», mai
usato per insegnare) e «stop believing that x means forget that x» (su «the
heart pumps blood», poi reinsegnato). La lezione sui piani con un atto
`assert` (**upon x do y**) è provata ma **non salvata**: l'unica mossa fuori dal
piano, `decline_steps`, ne sta fuori per scelta (messages.p0), e le altre
sarebbero duplicati.

**Secondo giro: due anelli rotti che sembravano un furto.** Al primo giro avevo
scritto che l'analisi di progettazione rubava le lezioni che nominano «plan».
Era sbagliato: «who answered?» diceva `analysis_last_resort`. La lezione
«how do you act when x means your plan when x?» **imparava**, e l'annuncio
veniva coperto a valle dal saggio, perché il dispatcher tratta come resa ogni
risposta che non ripete il soggetto del turno, e un annuncio di lezione non lo
ripete mai. Lo stesso per la ritrattazione, che in più perdeva la chiave:
scavalcava come stopword le parole iniziali («what», «how», «do») che erano la
sorgente stessa della forma. È un anello rotto del tipo del §5 passo 5 in una
variante nuova: **l'anello raggiunge il suo lettore, e la sua RISPOSTA non
raggiunge chi insegna.** Una catena di insegnabilità in cui parrot0 impara
senza dirlo è rotta quanto una che non impara.

**Che cosa resta fuori, e perché:** la forma italiana («smetti di credere che x significa …») si salva mezza tradotta
(«smetti of credere what x») e l'uso lo prende un altro lettore — il muro
italiano del gen510.

## 5. Il metodo

Per ogni abilità scelta, dalla suite o dai piani:

1. **Nominare l'abilità** in una frase, senza nomi interni.
2. **Trovare la superficie** che la insegna oggi. Se non esiste, la catena
   finisce subito in una riga a mano: è un buco di primo gradino.
3. **Risalire**: per la superficie trovata, chiedere quale lezione la crea o la
   estende. Registrare ogni gradino.
4. **Classificare la fine**: radice, circolo o riga a mano.
5. **Verificare che ogni anello raggiunga il suo lettore**, parlando, con
   `make chat`. Un anello che viene catturato da un altro lettore è rotto
   anche se la forma esiste.
6. **Scegliere il lavoro**: il buco più vicino alla radice che chiude più
   catene insieme.
7. **Lasciare il segno d'uso reale** (F., 2026-09-11, requisito di ogni giro):
   un anello aperto si insegna in `make chat` con una lezione vera, si usa su
   conoscenza reale, si salva, si legge il diff della KB, si committa e si
   pusha. Le parole inventate provano la meccanica e non si salvano; se un uso
   reale non c'è ancora, lo si scrive invece di inventarlo. Dettaglio in
   `LEARN_PROTOCOL.md` §6, «Ogni superficie scoperta lascia un segno d'uso
   reale».

Il gradino S2 dell'esempio 4.1 chiude, se aperto, le catene di tutte le forme
di lezione: è un buco ad alta leva.

## 6. Che cosa si misura

| Misura | Definizione |
|---|---|
| **Copertura di primo gradino** | abilità con almeno una superficie / abilità censite |
| **Vitalità** | catene che finiscono in radice o circolo / catene censite |
| **Profondità di radice** | gradini dall'abilità alla sua fine |
| **Anelli rotti** | anelli con la forma presente ma il turno catturato da un altro lettore |

La vitalità è il numero che interessa. Una suite verde con vitalità bassa dice
che parrot0 fa molte cose e ne potrebbe reimparare poche.

## 7. Cose da non fare

- **Non contare una superficie scritta a mano come radice.** La radice è una
  primitiva del motore, non una clausola `.p0` che nessuna lezione produce.
- **Non chiudere un circolo esponendo lo schema.** Una lezione che chiede a chi
  insegna di nominare `turn_form` o `taught_question_cue` non è una superficie:
  vale il divieto anti-inganno di MANTRA.md.
- **Non trasformare il censimento in test.** Le catene si studiano parlando e
  si registrano in questo documento o nel report di sessione; un `.p0t` nuovo
  vale solo quando un anello viene aperto davvero.
- **Non aprire un anello per un'istanza.** Il gradino da aprire è quello che
  chiude una classe di catene.

## 8. Da dove partire

1. Il censimento delle forme del catalogo (LEARN_PROTOCOL §6-bis): per ognuna,
   il gradino S2. La maggior parte finirà nella stessa riga a mano, la forma di
   lezione, ed è il segnale che S2 è il buco da aprire per primo.
2. Le abilità della suite con un ramo C dedicato: sono quelle con catena di
   lunghezza zero.
3. Le abilità che il gen508 rende già derivabili, come «twice»: sono la prova
   che l'ordine superiore riduce le radici, e vanno registrate come tali.
