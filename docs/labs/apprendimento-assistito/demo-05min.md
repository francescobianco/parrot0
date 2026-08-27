# Demo di cinque minuti — che cosa si riesce a insegnare parlando

**Data:** 2026-08-27
**Binario:** `parrot0` a `4eac5ee` (dopo gli incrementi M3-inversione e M2)
**Teacher:** solo turni di chat, in un unico processo continuo
**Durata:** 47 turni — 3 secondi di macchina, circa cinque minuti di chi scrive

Questo non è un gate e non promuove nulla nella KB del repository. È la misura
di che cosa un interlocutore riesce a far apprendere in una sessione breve,
dopo gli strati chiusi oggi, e soprattutto di che cosa **continua a non
riuscire**.

## 1. Protocollo

```text
PARROT0_SESSION=<file temporaneo>
PARROT0_LANG=en
```

- nessun `!assert`, nessuna query interna, nessun MCP, nessuna modifica a C o
  `.p0` durante la sessione;
- un solo processo: ogni turno vede quello che i precedenti hanno insegnato;
- entità e verbi inventati, perché il mondo preesistente non risponda al posto
  della lezione;
- ogni lezione seguita da un transfer su nomi mai pronunciati nella lezione.

## 2. Esito per strato

| Strato | Lezione parlata | Prova held-out | Verdetto |
|---|---|---|---|
| M2 menzione | `the word notwithstanding is a concession marker` | fatto sulla parola, non sul marcatore | **verde** |
| M2 riflessivo | `"lemma" is a mention_marker` | `the lemma albeit is a…` funziona subito dopo | **verde** |
| lessico relazionale | `brinks is a relation verb` | `zilvan brinks torvo` → estratto | verde (già noto) |
| M5 domanda duale | nessuna: emerge dal verbo | `who brinks torvo?` **e** `what does zilvan brinks?` | **verde** |
| M3 costruzione | `x krells y means x brinks y` | `mara krells sento` su nomi nuovi | verde |
| M5 morfologia | `krell means brinks` | `what does mara krell?` → `Sento` | **verde, ma non automatico** |
| M3 inversione | `x plummets y means y brinks x` | `oberon plummets quintal` → `brinks(quintal, oberon)` | **verde** |
| M3 alfabeto | `a is a rule_variable` | `a wrenches b means a brinks b` | **verde** |
| M3 pivot | `learn "vale a dire" as a construction teaching pivot` | lezione con il pivot inventato | verde |
| regole e catene | due `if … then …` | `is pellon a kavos?` → `Yes`, `dorane` → `No` | verde (già noto) |
| join a tre variabili | regola con x/y/z | `who is the eldertutor of pim?` → `narik` | verde (già noto) |
| congiunzione | `use blen as a conjunction` | `soup requires broth blen salt` → 2 prerequisiti | verde (già noto) |
| prosa breve | `read: …` due frasi | `who brinks marnow?` → `Kessil` | verde |
| ablation | `forget that x krells y means x brinks y` | `zeda krells noor` torna rosso | verde |
| M10 procedura | `to convert vekras to zolins…` | muro onesto **solo con unità inventate** | **rosso** |

## 3. I passaggi che contano

### 3.1 La menzione estende il proprio vocabolario

```text
> the word notwithstanding is a concession marker
< Learned: concession_marker(notwithstanding).

> "lemma" is a mention_marker
< Learned: mention_marker(lemma).

> the lemma albeit is a concession marker
< Learned: concession_marker(albeit).
```

Il secondo turno insegna **come si menziona**, e il terzo usa subito quella
regola. È il ciclo riflessivo cercato — l'atto didattico che amplia il proprio
canale — applicato al metalinguaggio invece che alle risposte.

### 3.2 Una costruzione inversa mantiene i ruoli invertiti

```text
> x plummets y means y brinks x
< I learned a candidate construction: @O plummets @S maps to @S brinks @O.

> oberon plummets quintal
< Learned: brinks(quintal, oberon).

> who brinks oberon?
< Quintal.
```

I nomi degli slot li dà il lato già compreso. Il fatto prodotto è quello
invertito, e la domanda lo conferma dal verso opposto.

### 3.3 La forma base si apre con una frase

```text
> what does mara krell?
< I don't understand that yet.

> krell means brinks
< I learned a candidate construction: @S krell @O maps to @S brinks @O.

> what does mara krell?
< Sento.
```

La domanda con la forma flessa (`what does zilvan brinks?`) funzionava già. La
forma base no, e non si apre da sola: si apre perché il teacher la dice. È
*addressable*, non automatico — la distinzione che il piano chiede di non
confondere.

### 3.4 Il pivot didattico è vocabolario vivo

```text
> learn "vale a dire" as a construction teaching pivot
< Got it - I'll take "vale a dire" as a way to a construction teaching pivot now.

> x glimmers y vale a dire x brinks y
< I learned a candidate construction: @S glimmers @O maps to @S brinks @O.
```

Un separatore inventato, in un'altra lingua, insegnato e usato nello stesso
processo, senza ricompilare.

## 4. Che cosa NON ha funzionato

### 4.1 La procedura resta un misclaim — e la demo stava per nasconderlo

Nella sessione, con unità inventate, il turno didattico ha prodotto un muro
onesto:

```text
> to convert vekras to zolins multiply by 2 and add 30
< Hmm, I don't know about convert yet…

> convert 10 vekras to zolins
< I don't understand that yet.
```

Sarebbe stato facile leggerlo come un progresso. Non lo è: con le unità che la
KB conosce il difetto è intatto.

```text
> to convert celsius to fahrenheit multiply by 2 and add 30
< -16.6667.

> convert 10 celsius to fahrenheit
< -12.2222.
```

**M10 è rosso come nella prima sessione di profondità.** La frase che insegna
viene eseguita come se contenesse un input, e il secondo turno non usa la
procedura detta. Il muro della demo era un effetto delle unità sconosciute, non
una capacità nuova.

### 4.2 L'ablazione è corretta ma parziale

```text
> forget that x krells y means x brinks y
< The construction @S krells @O -> @S brinks @O is no longer active; I kept its lesson trace.

> zeda krells noor
< Hmm, I don't know about krells yet…

> what does mara krell?
< Sento.
```

La capacità ritratta sparisce. L'ultima risposta resta corretta per due ragioni
legittime — il fatto `brinks(mara, sento)` è storia, e `krell means brinks` è
una lezione diversa e ancora attiva — ma nessuna delle due è visibile a chi
guarda. Manca la genealogia di M14: parrot0 non sa dire *quale* lezione sostiene
ancora quella risposta.

### 4.3 Il muro non tipizza ancora il gap

Ogni fallimento della sessione ha prodotto la stessa forma di muro, e sempre
suggerendo il metalinguaggio già noto («say «X is a relation verb»»). Nessun
turno ha detto «mi manca la costruzione», «mi manca la forma base», «mi manca
la procedura». È M13, ed è il motivo per cui questa sessione ha avuto bisogno di
un teacher che sapeva già che cosa insegnare.

## 5. Bilancio

In cinque minuti si insegnano oggi: una classe applicata a una parola-funzione,
il modo stesso di menzionare, un verbo relazionale, quattro costruzioni (diretta,
inversa, con variabili nuove, con pivot inventato), la forma base di un verbo,
due regole unarie in catena, una regola relazionale a tre variabili, una
congiunzione, e due fatti da prosa. Tutto con transfer su nomi mai pronunciati,
e tutto ritrattabile parlando.

Non si insegnano: una procedura, il tipo del proprio gap, e la provenienza di
una capacità dopo un'ablazione parziale. La sessione lunga resta chiusa dal
cancello del §6 del piano, e questa demo indica i tre strati da aprire per
primi: **M10, M13, M14**.
