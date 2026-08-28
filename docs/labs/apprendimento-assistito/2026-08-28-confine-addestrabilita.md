# 2026-08-28 — Il confine dell'addestrabilità: mappa e prima espansione

Giro condotto contro [`docs/plans/apprendimento-assistito.md`](../../plans/apprendimento-assistito.md),
con l'obiettivo di **far emergere il confine e spostarlo**, non di aggiungere
fatti.

**Esito: un limite mappato su otto strati, uno spostato (M3), due bug del motore
trovati e corretti.**

## 0. Perché i tre giri precedenti non misuravano il confine

Le sessioni di elettronica, letteratura scientifica e ML/NLP hanno prodotto 24
fatti veri — e sono rimaste tutte **dentro l'unico ponte che parrot0 già
conosce**, «X is a relation verb». È esattamente il caso che il §2 del piano
descrive:

> parrot0 sa imparare quando la lezione è già espressa in un metalinguaggio che
> conosce; non sa ancora apprendere liberamente un nuovo pezzo di
> metalinguaggio.

Misurare quel ponte misura la copertura delle forme esistenti moltiplicata per
la pazienza del teacher. Questo giro parte da lì.

## 1. Metodo

Batteria stratificata su M0–M13, con conoscenza **reale e verificabile**
(petrologia: basalto, granito, talco, ossidiana; fonti standard di petrografia e
la scala di Mohs), su un processo pulito. Nessun nome inventato, nessuna
fixture. `B0` = 35266 fatti.

## 2. La mappa del confine

| Strato | Sonda | Esito |
|---|---|---|
| **M0** onestà del turno | «why is granite coarse-grained?» | **falso**: risponde spiegando *la pioggia* |
| **M0** | «granite is coarse-grained because magma cools slowly» | smalltalk: «Nice. Tell me what made it good» |
| **M0** | «how do you identify a mineral?» | smalltalk: «Oh? What's it like?» |
| **M13** tipizzazione del gap | «what kind of gap do you have there?» | smalltalk: «That sounds nice — tell me more» |
| **M13** | «what is missing for you to answer that?» | deflessione personale |
| **M8** ricongiungimento | «being an igneous rock means being igneous and being a rock» | muro **nominato**: «I cannot align exactly two shared variables» |
| **M8** | `igneous_rock(basalt)` poi «is basalt igneous?» | muro — è il caso `occupied_square(d2)` del piano, alla lettera |
| **M4** ruoli nominabili | «in that sentence the first name is what forms…» | non parsato |
| **M7** negazione | «no mineral is softer than talc» | muro |
| **M7** pronome cross-turno | «it forms when lava cools very fast» | smalltalk |
| **M9** causalità in prosa | «X because Y» | assorbito da smalltalk |
| **M10** procedura | «to identify a mineral, first test its hardness…» | muro |
| **M3** arità ≠ 2 | «a metaphor maps a source domain onto a target domain» | muro **nominato** |

Due risultati strutturali, oltre alle singole caselle.

### 2a. M0 è violato in modo sistematico, e non da un muro

Quattro input reali e ben formati ricevono **riempitivo sociale** invece di un
muro, e «why is granite coarse-grained?» produce deterministicamente una
spiegazione della **pioggia**. Non è una risposta sbagliata su un dettaglio: è
una risposta non ancorata a niente, data con sicurezza. Il piano lo dice già —
«è il prerequisito di tutti gli altri: senza M0 ogni metrica successiva è
inaffidabile» — e la sonda lo conferma su conoscenza vera.

### 2b. M3 e M8 sono lo stesso muro

Il piano li elenca come strati distinti. La sonda mostra che **una sola riga di
C li bloccava entrambi**: in `p0_explicit_pattern`, `*nvars != 2 || seen[0] != 1
|| seen[1] != 1`. M3 (tre ruoli) sfora il conteggio; M8 (bersaglio congiuntivo)
sfora l'unicità. Stesso messaggio, stessa causa.

E c'era una seconda metà dello stesso limite, non nominata dal piano: il
**legame degli slot** in lettura era binario end-to-end — `subj`/`obj`, e un
fatto sempre a due argomenti.

## 3. L'espansione: M3 aperto

La scelta è caduta su M3 perché è il gap che, chiuso, libera più famiglie di
frasi (§8 del piano): quasi ogni strumento logico articolato ha tre ruoli.

**Che cosa NON è servito.** L'atto didattico costa **zero C**: la strada
generica «X is a Y» produce già `ternary_relation_verb(converts)` da sola.
Verificato prima di scrivere una riga.

**Che cosa è servito.** Il legame degli slot conta ora quanti slot dichiara il
pattern, e il fatto nasce con quell'arità. La lettera dopo `@` smette di essere
un interruttore fra due variabili e diventa il nome di uno slot.

**Che cosa resta conoscenza** — il punto:

```
> converts is a ternary relation verb     → ternary_relation_verb(converts)
> the word into is a link word            → link_word(into)
> an enzyme converts a substrate into a product
→  Learned: converts(enzyme, substrate, product).
```

Il verbo e la parola che collega il terzo ruolo sono fatti; la regola che ne
costruisce il pattern sta in `kb/core/grammar.p0`. **Una relazione ternaria
nuova domani costa due frasi dette in chat.**

### 3a. Il buco del consumatore, riaperto e chiuso

Un fatto a tre ruoli imparato e non interrogabile sarebbe un fatto morto — è
gen306 un'arità più in là. Il meccanismo generale: **un interrogativo in uno
slot trasforma il pattern in una domanda**, a qualunque arità.

```
> an enzyme converts a substrate into what?   → product.
> an enzyme converts what into a product?     → substrate.
> what converts a substrate into a product?   → enzyme.
```

La lettura delle asserzioni resta chiusa ai turni interrogativi, com'è giusto:
la porta per le domande è separata e non asserisce mai.

### 3b. I gate

| Gate | Esito |
|---|---|
| replay | verde |
| transfer, dominio 2 | `translates(compiler, source_code, machine_code)` — verde |
| transfer, dominio 3 + parola di collegamento nuova | `focuses(lens, light, screen)` con «onto» — verde |
| ablation | verde: ritrattando la classe la capacità sparisce |
| onestà del messaggio | corretta — vedi §4b |
| regressione | **zero** verdi diventati rossi su 329 file |

Tre domini reali (biochimica, compilatori, ottica), due parole di collegamento,
**nessuna ricompilazione**.

## 4. Due bug del motore trovati strada facendo

### 4a. L'aritmetica era rotta, e nascondeva tutto il resto

`make soft-test` e `make test` si fermano al primo fallimento, e il primo era
`health.p0t:20` — «what is 2 plus 2?» → «I don't understand that yet.», da
gen443. **Finché restava rosso, nessuna modifica al motore era verificabile**:
la suite non arrivava mai agli altri 2400 assert.

Due metà dello stesso passaggio KB-first lasciate a metà: `arith_op_char`
interrogava `infix_operator/2` con il token nudo mentre le superfici in KB sono
citate; e `arithmetic_word/1`, nato copiando l'elenco che stava nel C, aveva
perso proprio le parole degli operatori.

La seconda metà non si ripara ricopiando: due elenchi paralleli divergono una
seconda volta. Si **deriva** — `arithmetic_word($W) :- infix_operator($W, $Op)`
— e un operatore insegnato domani è ammesso in un'espressione lo stesso giorno.

### 4b. Il motore mentiva su ciò che aveva appena imparato

Il primo fatto ternario entrava corretto in KB e veniva annunciato **mutilo**:
`converts(enzyme, product)`, due argomenti su tre, perché il template ne aveva
due. Un misclaim su ciò che si è appena appreso, cioè il caso peggiore del
mantra #7. La frase per tre ruoli ora sta in KB.

## 5. Che cosa NON si è mosso, e va detto

- **M8 resta al muro.** «Being an igneous rock means being igneous and being a
  rock» dà lo stesso messaggio di prima, ma per l'altra metà della causa: il
  bersaglio congiuntivo vive nell'allineatore delle lezioni di costruzione, non
  nel legame degli slot in lettura. Metà del muro comune è caduta, metà no.
- **M0 e M13 sono intatti** e sono il debito più grave. Il registro delle lacune
  esiste ed è scritto a ogni turno — `gap_kind`, `gap_opaque`, `gap_anchor` —
  ma **non è dicibile**: la conoscenza c'è e non ha un consumer. È il gap con
  il rapporto valore/rischio migliore per il prossimo giro, ed è la
  precondizione dichiarata del teacher automatico (§8).
- **Le collisioni di verbo** restano non rilevate: `projects` è stato rubato da
  una risposta di chitchat («Finally start the project I keep putting off»),
  come `needs`→`requires` nella sessione ML. Il motore non sa ancora rifiutare
  un verbo il cui predicato è già posseduto.
- **L'albero è pesantemente rosso da prima**: 362 assert falliti su 329 file,
  123 file rossi già a `origin/main`. Questo giro ne toglie 3 e non ne aggiunge.

## 6. File toccati

- `src/brain/10-memory-knowledge.c` — legame a N slot, interrogativo in uno
  slot, porta di sola interrogazione, messaggio a tre argomenti
- `src/brain/20-math.c` — la chiave di lettura degli operatori
- `kb/core/grammar.p0` — `ternary_relation_verb/1`, `link_word/1`, la regola che
  costruisce il pattern
- `kb/core/lexicon.p0` — `arithmetic_word` derivata da `infix_operator`
- `kb/core/messages.p0` — `learned_ternary_fact`, `slot_answer`
