# La batteria di rinforzo — lavori multi-turno con esito verificabile

> **L'obiettivo.** Costruire il corpus che dice a parrot0 se sta migliorando:
> una batteria di **episodi**, cioè dialoghi multi-turno in cui gli viene chiesto
> di **compiere un lavoro**, e in cui l'esito è **verificabile da una macchina**,
> non da un'opinione. Non si addestra nessuna rete: il rinforzo è un ciclo —
> lavoro → verifica → lacuna → proposta → conoscenza — e la batteria ne è la
> funzione di ricompensa.
>
> Aperto a gen412, subito dopo che gen410-411 hanno chiuso il ciclo di
> autoriparazione ([`question-emergence.md`](question-emergence.md)). Quel ciclo
> ora cammina; quello che gli manca è **contro cosa** camminare.

---

## LA TESI

Il rinforzo, qui, non tocca nessun parametro. Il segnale non aggiorna dei pesi:
**produce conoscenza**, e la conoscenza è ispezionabile, diffabile, revocabile.

Il ciclo esiste già, pezzo per pezzo, ed è stato costruito senza sapere che
sarebbe servito a questo:

| pezzo | cosa fa | dove |
|---|---|---|
| il muro registrato | un turno che non ha risposto diventa `machinery_gap` con la sua ancora | gen406 |
| la proposta | parrot0 propone un ponte, lo assume in ipotetico, **ripone il turno** e lo tiene solo se ora risponde | gen410 |
| la persistenza | lacune e ponti sopravvivono al processo (`gaps.p0`, `bridges.p0`) | gen411 |
| il comando di run | `--dream` sceglie il rimedio e stampa il bilancio | gen411 |
| l'instradamento | ciò che si impara si deposita accanto ai suoi simili, non in un deposito indistinto | gen411 |

Manca **una sola cosa**, ed è questo piano: un corpus di lavori la cui riuscita
si misura. Senza, «migliorare» è un'impressione; il bilancio del sogno sa dire
*quante lacune ha chiuso*, ma non *se parrot0 sa fare qualcosa che ieri non
sapeva fare*.

**L'ancoraggio è in `PRINCIPLES.md`**, ed è letterale:

> *the harder and broader the tasks we demand, the less room a mimic has to
> cheat.*

La batteria è quella frase resa operativa. Non è una suite di test in più: è
**la superficie su cui l'imitatore non ha più spazio**.

---

## LA SPERANZA — e perché non è «sapere di più»

La ragione per cui questo piano vale la pena non è far crescere la KB. La KB
cresce già, e crescere è la cosa facile.

**La speranza è che la batteria affini il ragionamento**, e che parrot0 diventi
intelligente **nel senso pratico del termine**: non che sappia molte cose, non
che risponda bene alle domande, ma che **sappia cosa fare** quando la situazione
non è quella che gli è stata insegnata.

Detto in cosa si vede, perché altrimenti è una frase e basta:

- **decide l'ordine degli atti** invece di seguire l'ordine in cui gliel'hanno
  detto;
- **si accorge che manca un dato** e lo chiede, invece di riempire il buco;
- **si accorge che il lavoro, com'è formulato, non si può fare**, e lo dice —
  che è diverso dal non riuscirci;
- **porta una procedura imparata in un dominio dentro un altro** dove nessuno
  gliel'ha mostrata;
- **si ferma quando ha finito**, e non continua a produrre perché può.

Nessuna di queste cinque si misura con una domanda. Tutte e cinque si misurano
con un **lavoro**, ed è per questo che l'unità della batteria è un lavoro e non
un prompt: una domanda misura cosa sai, un lavoro misura cosa fai quando la
situazione non combacia.

**La confusione onesta da tenere a bada.** Crescere e ragionare meglio sono due
cose diverse, e la batteria le può confondere: un episodio si può chiudere
imparando *quel* fatto, e il conteggio sale lo stesso. Ma la matrice ha già il
numero che le separa, ed è il **trasferimento**:

- se chiudere un episodio ne chiude altri in domini lontani, è cresciuta una
  **facoltà** — e questa è la speranza;
- se chiude solo sé stesso, è cresciuta l'**enciclopedia** — legittimo, utile,
  ma un'altra cosa.

Lo stesso numero serve due volte e va letto in due direzioni opposte: trasferimento
**alto** dice che è migliorato il ragionamento; trasferimento **basso** in una
cella che intanto tiene bene dice che lì c'è un verticale da costruire. Sono i
due esiti buoni di questo piano, e per una volta sono distinguibili guardando.

Resta una scommessa, nel registro di `PRINCIPLES.md`: che una pressione fatta di
lavori larghi e verificabili produca ragionamento e non solo accumulo. Non lo
decretiamo. Costruiamo la superficie, e guardiamo.

---

## COSA NON È — le tre confusioni da evitare

1. **Non è `make test`.** La suite è un **cricchetto**: 2477 assert che devono
   restare verdi, e un rosso è una regressione da riparare subito. La batteria è
   una **misura**: è normale e sano che sia in gran parte rossa, e il numero che
   conta è *quanto rosso è diventato verde da solo*. Confonderle produce l'unico
   esito davvero dannoso — togliere episodi difficili per far tornare il conto.
2. **Non è `llmscore` / `rulescore` / `sym-bench`.** Quelli sono strumenti di
   **scoperta**: un LLM come oracolo comportamentale, per capire quale mossa fa
   un modello di frontiera. Servono a trovare la classe da motorizzare, non a
   dare un punteggio; un giudizio di un LLM non è un esito verificabile e non può
   essere una ricompensa. Restano dove sono, con quel ruolo.
3. **Non è addestramento.** Nessun gradiente, nessun peso, nessuna epoca. La
   "politica" di parrot0 è la sua KB più il suo motore; l'unico aggiornamento
   ammesso è **conoscenza scritta in un file `.p0`**, con provenienza e possibilità
   di revoca. Se per far salire il punteggio serve una lista di parole nel C, il
   punteggio è salito nel modo sbagliato (MANTRA #2).

---

## L'UNITÀ: L'EPISODIO

Un episodio è **un lavoro, non una domanda**. La differenza è che un lavoro ha
uno **stato finale**, e uno stato finale si può guardare.

Anatomia:

| campo | cosa dice |
|---|---|
| **id** | `famiglia/nome`, stabile per sempre — è la coordinata su cui si misura il progresso fra due generazioni |
| **premessa** | lo stato iniziale dichiarato: quale KB, quale profilo, quale lingua, quali file nel mondo. Nessuno stato implicito |
| **il lavoro** | una frase in italiano che dice cosa un umano si aspetta sia fatto alla fine |
| **turni** | il dialogo, con gli interventi dell'interlocutore. Multi-turno per costruzione: il lavoro non deve stare in un prompt solo |
| **verifiche** | una o più, di classe dichiarata (sotto). Tutte devono passare |
| **perché è difficile** | una riga, obbligatoria. Un episodio di cui non si sa dire cosa mette alla prova non misura niente |
| **origine** | da dove viene: un fallimento reale, una sonda, un piano, una segnalazione |

**Il multi-turno non è decorativo.** È la parte del lavoro in cui si misura ciò
che un prompt singolo non tocca: che parrot0 tenga il filo, che chieda quando
manca un dato invece di inventarlo, che non ricominci da capo, che l'ordine degli
atti sia il suo e non quello suggerito dalla domanda.

---

## LA TASSONOMIA DEI VERIFICATORI

Cinque classi, e nessun episodio è ammesso senza almeno una. La regola è che
**la verifica non deve poter essere soddisfatta parlando**.

**V1 — stato della conoscenza.** Alla fine del dialogo la KB contiene (o non
contiene) certi fatti. È la verifica più forte per gli episodi di comprensione:
non «hai risposto bene», ma «hai **capito**, e la prova è che adesso lo sai».
Strumento: `!query` / `!query!`, che il motore di test ha già.

**V2 — uscita.** Il turno risponde esattamente, o contiene una forma richiesta.
È la più debole e la più facile da falsificare imparando la frase: si usa **solo
in coppia** con un'altra classe, mai da sola.

**V3 — artefatto.** Alla fine esiste un file, e quel file supera una prova
indipendente: compila, passa i suoi test, produce l'output atteso. È il
verificatore che non si può addolcire — c'è già in casa (`code_eval`, l'oracolo
di `swe-bench`, `--test`), ed è la spina dorsale delle famiglie di lavoro sul
codice.

**V4 — invariante di onestà.** Certe verifiche stabiliscono che parrot0 **non**
deve fare qualcosa: non affermare un fatto non derivabile, dichiarare il limite,
chiedere il dato mancante. Vale MANTRA #7 — *uccidi il muro, MAI con una
risposta sbagliata* — e MANTRA #9 — *il wall-rate non vede le risposte
sbagliate*. **Un episodio che fallisce V4 non vale zero: vale meno di un
episodio non tentato.** È l'unica asimmetria del punteggio, ed è deliberata.

**V5 — progresso.** L'episodio ha lasciato una `machinery_gap` al giro
precedente; adesso quella lacuna è chiusa, e **resta chiusa al giro dopo**. È il
verificatore che chiude l'anello con gen411: misura il ciclo autonomo, non la
risposta.

---

## LA STRUTTURA DEL DATASET

Il principio è di non costruire un secondo motore. Il runner degli episodi
**è già scritto**: il motore di test (`docs/plans/test-engine.md`) fa multi-turno,
multi-riga, `!set`, `!reset`, `!query`, e si ferma al primo controesempio. Un
episodio è **un `.p0t`**: stessa estensione, stessa sintassi, stesso runner.

La separazione fra cricchetto e misura la fa **la cartella**, non il nome: la
suite non raccoglie per wildcard, enumera i file uno per uno nel `Makefile`
(verificato), quindi `tests/rl/` non entra in `make test` a meno che qualcuno non
ce lo scriva. Il vincolo resta però quello, e va detto forte: i `.p0t` di
`tests/p0t/` devono restare verdi, quelli di `tests/rl/` sono una misura e
possono essere rossi. Il giorno in cui i due insiemi si mescolano, la batteria
smette di misurare e comincia a essere potata.

```
tests/rl/
  manifest.tsv                 il catalogo: una riga per episodio —
                               id, famiglia, macro-area, dominio, gradino,
                               vettore, verificatori, origine, stato
  episodes/
    <famiglia>/<macro-area>/<nome>.p0t
    comprensione/naturale/fotosintesi-implicita.p0t
    codice/pratica/riscala-ricetta.p0t
    ...
  variants/<id>/               il fascio: metà pubblico, metà tenuto fuori
  fixtures/<id>/               il mondo iniziale: file che l'episodio può leggere
  artifacts/<id>/              ciò che l'episodio ha prodotto (mai versionato)
  ledger/                      lo storico per id: chi passava a quale generazione
```

L'albero è **famiglia prima, macro-area poi**, e non il contrario: la famiglia è
ciò che si legge scrivendo un episodio («sto scrivendo un lavoro di codice»),
mentre il confronto fra domini è un'operazione sul manifest, non sulle cartelle.

Il **manifest è un TSV** perché la matrice si legge da lì. «Quanti episodi di
gradino 3 nella famiglia codice passano oggi», o «qual è la resa della macro-area
pratica a novità 3», devono essere righe di shell, non letture di file.

Il **ledger** è la parte che rende la batteria una misura invece di una
fotografia: per ogni id, a quale generazione è passato per la prima volta, e se
è mai tornato indietro. Un episodio che passa e poi regredisce è un'informazione
più preziosa di uno che non è mai passato.

Forma di un episodio, nella sintassi che il motore già legge:

```
# id:        comprensione/naturale/enumerazione-implicita
# famiglia:  comprensione
# dominio:   naturale / biologia
# gradino:   2
# vettore:   profondità 1  distanza 2  ambiguità 0  composizione 1
#            rumore 0  apertura 1  novità 3
# lavoro:    estrarre i membri di un insieme nominato di sfuggita, e saperli
#            poi elencare quando gli vengono chiesti
# difficile: l'insieme non è mai introdotto come elenco; la sua esistenza si
#            deduce da come viene usato due turni dopo
# origine:   fallimento reale, sessione gen382o
# verifica:  V1 + V4

[premessa]
!set PARROT0_PROFILE=kb/profiles/agi.p0
!reset

[turni]
> ...
< ...

[verifiche]
!query ...
```

Le entità degli episodi sono **nuove**, mai prese dalla KB curata: la memoria di
progetto è esplicita su questo — non si prova la comprensione amputando il
contesto, la si prova su cose che parrot0 non può già sapere.

---

## LE FAMIGLIE — la batteria

Ogni famiglia è un tipo di lavoro, non un argomento. L'elenco iniziale nasce da
dove parrot0 oggi lavora davvero:

| famiglia | il lavoro | verifica |
|---|---|---|
| **comprensione** | leggere prosa e restare capaci di rispondere su ciò che ne è entrato | V1 + V4 |
| **conversazione** | tenere il filo su molti turni: coreferenza, ripresa, correzione di un dato già dato | V2 + V1 |
| **procedure** | eseguire una procedura **insegnata nel dialogo**, non compilata | V1 + V2 |
| **codice** | localizzare, modificare, far passare una prova indipendente | V3 |
| **ricerca** | trovare in fonti locali il dato che manca, e dichiarare quando non c'è | V1 + V4 |
| **strumenti** | condurre un lavoro con gli strumenti locali (leggere, cercare, elencare) in un giro solo | V3 + V4 |
| **investigazione** | condurre lui il discorso: porre le domande che mancano prima di concludere | V4 + V1 |
| **macchineria** | accorgersi di un proprio limite, proporre il rimedio, verificarlo | V5 |

L'ultima famiglia è quella che rende la batteria un ciclo di rinforzo invece di
una pagella: gli episodi che falliscono nelle altre sette **generano** gli
episodi dell'ottava.

---

## I GRADINI

Cinque, e servono a non confondere «non ci arriva» con «non ci è arrivato oggi».

- **0 — il caso nudo.** Un turno, tutto detto, nessun distrattore. Se fallisce
  qui, manca una facoltà, non una sfumatura.
- **1 — multi-turno pulito.** Il lavoro sta in tre o quattro turni collaborativi.
- **2 — con distrattori.** Nel dialogo c'è materiale che non serve, e un dato
  che viene **corretto** a metà strada.
- **3 — incompleto.** Manca un dato necessario. La riuscita **è** chiederlo: qui
  V4 è il verificatore principale, e rispondere comunque è il fallimento peggiore.
- **4 — avversario.** L'interlocutore insiste per una risposta che non è
  derivabile, o propone una premessa falsa con sicurezza.

Un episodio nasce sempre a un gradino dichiarato, e **lo stesso lavoro va scritto
a più gradini**: è la sola forma di confronto che dice dove si rompe.

---

## IL SECONDO ASSE: I DOMINI — a 360 gradi, per scoprire dove puntare

La famiglia dice **che tipo di lavoro** è. Il dominio dice **di che cosa parla**.
Sono indipendenti, e tenerli separati è il punto: senza il secondo asse la
batteria misura le facoltà ma non sa dire *dove* parrot0 potrebbe diventare
davvero forte.

**Perché a tappeto e non mirato.** Oggi non sappiamo su quale verticale valga la
pena costruire un super-esperto, e sceglierlo adesso significherebbe sceglierlo
per intuizione — cioè per i domini che *noi* troviamo interessanti, o peggio per
quelli in cui la KB è già cresciuta e quindi sembrerà brava per costruzione. Si
copre tutto lo scibile con episodi pochi e sparsi, si guarda dove il segnale è
inaspettatamente buono, **e solo allora** si scava. La copertura larga non è
generosità: è l'unico modo di non decidere prima di sapere.

### Le nove macro-aree

Nove, con i loro domini. Le prime otto sono lo scibile; la nona è particolare e
va tenuta separata perché non è sapere del mondo.

| macro-area | domini |
|---|---|
| **formale** | matematica, logica, statistica e probabilità, informatica teorica |
| **naturale** | fisica, chimica, biologia, scienze della terra, astronomia |
| **vita e salute** | medicina, farmacologia, nutrizione, psicologia, neuroscienze |
| **tecnica** | programmazione, ingegneria, elettronica, reti e sicurezza, materiali |
| **umanistica** | storia, geografia, filosofia, linguistica, letteratura |
| **espressiva** | arti visive, musica, design, narrativa, cinema |
| **sociale** | diritto, economia, istituzioni, antropologia, educazione |
| **pratica** | cucina, sport e giochi, viaggi e trasporti, finanza personale, casa e burocrazia |
| **riflessiva** | parrot0 su sé stesso: la propria KB, il proprio codice, i propri limiti |

La macro-area **pratica** non è il fondo del barile: è quella dove i lavori hanno
esiti verificabili più naturalmente di ogni altra — una ricetta si scala, un
viaggio si somma, un interesse si calcola, una scadenza si data. È un candidato
verticale serio, e sarebbe stato il primo a essere escluso da una scelta a
intuito.

La macro-area **riflessiva** resta fuori dai confronti fra domini: parrot0 ha su
di sé un accesso che sugli altri domini non ha, e mescolarla falserebbe la
classifica. Si misura, ma in una colonna a parte.

### La matrice

9 macro-aree × 8 famiglie = **72 celle**. La cella è l'unità di analisi, non
l'episodio: un episodio da solo non dice niente, una cella con un segnale
ripetuto sì.

Non tutte le celle sono ovvie, e le meno ovvie sono spesso le più informative —
*codice × cucina* significa «scrivi il programma che riscala una ricetta per
dodici persone», ed è un lavoro vero con una verifica dura (V3). Una cella che
resta vuota perché nessuno riesce a immaginarci un lavoro verificabile **è un
risultato**, e va annotata come tale invece che lasciata bianca.

### Il budget, per non annegare

La tentazione è riempire la matrice. Sarebbe il modo più rapido di produrre un
dataset grande e muto.

- **Primo passaggio:** 1 episodio per cella, al gradino 1. **72 episodi.** Basta
  a dire dove c'è segnale e dove no.
- **Secondo passaggio:** solo nelle celle con segnale, si sale di gradino — 2 e
  3 — e si scrive il fascio di varianti. Tre o quattro celle, non venti.
- **Terzo passaggio:** nella cella candidata a verticale, si scende in profondità
  con i domini specifici della macro-area invece che con la macro-area intera.

Un episodio si scrive per essere **piccolo**: tre o quattro turni, una fixture
minima, una verifica non aggirabile. Un episodio lungo e bello è un episodio che
nessuno riscriverà quando il formato cambierà.

---

## LA COMPLESSITÀ NON È UNA SOLA COSA

Il gradino è comodo ma è un numero solo, e un numero solo non sa dire *perché*
un episodio è difficile. Ogni episodio porta perciò anche un **vettore di
complessità**: sette dimensioni, ognuna da 0 a 3, dichiarate in intestazione.

| dimensione | cosa fa crescere | 0 | 3 |
|---|---|---|---|
| **profondità** | quanti salti inferenziali separano i dati dalla conclusione | dato esplicito | catena di quattro o più |
| **distanza** | quanti turni passano fra quando un dato è dato e quando serve | stesso turno | otto turni prima |
| **ambiguità** | quante letture ammette ciò che è stato detto | una | più letture plausibili, da disambiguare chiedendo |
| **composizione** | quante richieste coordinate stanno nello stesso turno (MANTRA #10) | una | tre, con vincoli fra loro |
| **rumore** | distrattori, dati corretti a metà strada, contraddizioni | nessuno | correzione + contraddizione esplicita |
| **apertura** | quanto la risposta è vincolata | una forma sola | il formato è parte del lavoro (MANTRA #11) |
| **novità** | quanto le entità sono estranee a ciò che parrot0 già sa | entità note | tutto nuovo, coniato per l'episodio |

Il gradino resta, ed è **il massimo** del vettore, non la somma: un episodio è
difficile quanto la sua dimensione peggiore.

**A cosa serve davvero.** Quando una cella va male, il vettore dice se va male
*per il dominio* o *per una dimensione*. Se tutti gli episodi falliti hanno
`distanza 3` a prescindere dalla macro-area, il problema non è lo scibile: è la
memoria di lavoro, e va risolta una volta per tutte invece che dominio per
dominio. È la differenza fra ottantadue riparazioni e una.

La dimensione **novità** ha un ruolo speciale ed è il correttivo contro il
vantaggio ingiusto: i domini in cui la KB è già ricca — matematica,
programmazione — sembreranno bravi per costruzione. Ogni cella va perciò coperta
**anche** a `novità 3`, con entità coniate apposta. Il confronto onesto fra
domini si legge lì, non sulla media.

---

## COME SI RICONOSCE IL VERTICALE

Il verticale non è «il dominio col punteggio più alto». Quello è il dominio dove
la KB è già cresciuta di più, e sceglierlo sarebbe girare in tondo. Sono quattro
indicatori, e vanno letti insieme:

1. **Resa strutturale** — quanti episodi della cella si chiudono con **una regola
   generica** invece che con *n* fatti. Un dominio dove una regola ne chiude sei è
   un dominio con struttura; uno dove servono sei fatti per sei episodi è un
   elenco travestito. È il primo indicatore e il più importante: dice se lì
   parrot0 può *ragionare* o soltanto *sapere*.
2. **Pendenza** — come cade il tasso di chiusura salendo di gradino. Un dominio
   che tiene al gradino 3 vale più di uno che parte più in alto e crolla dopo il 1.
3. **Trasferimento basso** — se chiudere gli episodi del dominio X chiude anche
   quelli di Y e Z, non abbiamo trovato un verticale: abbiamo trovato una facoltà
   generale, e va motorizzata al centro. Un verticale si riconosce dal fatto che
   la sua crescita **non** si propaga: quello è il segno che lì c'è conoscenza
   specifica che vale la pena accumulare.
   *Attenzione a non leggerlo come un giudizio*: trasferimento alto è l'esito
   **migliore** dei due — è il ragionamento che è cresciuto (vedi «La speranza»).
   Qui serve solo a rispondere a un'altra domanda, cioè dove conviene scavare.
4. **Tenuta a novità 3** — la resa sul dominio con entità coniate. Separa
   «conosce» da «sa fare».

Il candidato è la cella con **resa strutturale alta, pendenza dolce, trasferimento
basso e tenuta a novità**. Se nessuna cella soddisfa tutti e quattro, la risposta
onesta è che il verticale non c'è ancora e la batteria va allargata invece che
approfondita — ed è un esito legittimo del primo passaggio, non un fallimento.

---

## LE REGOLE ANTI-IMBROGLIO

Sono vincoli, non buone intenzioni: la batteria è la funzione di ricompensa, e
una funzione di ricompensa si ottimizza — anche per le vie sbagliate.

1. **Il fascio, non il caso** (MANTRA #15). Ogni episodio ha varianti generate
   sulla stessa struttura: sinonimi, ordine invertito, numeri diversi, entità
   multi-parola, lingua diversa. **Metà delle varianti resta fuori dal catalogo
   pubblico** e si tira fuori solo per misurare. Un episodio che passa e le cui
   varianti no non è passato: è stato memorizzato.
2. **Nessuna chiusura letterale.** Se un episodio si chiude aggiungendo un cue o
   una parola nel C, non conta come progresso — va rifiutato in revisione anche
   se il verde è vero (MANTRA #1, #2).
3. **Il verificatore non guarda le parole quando può guardare lo stato.** Se
   esiste una V1 o una V3 possibile, la V2 da sola è vietata.
4. **Zero risposte false è un blocco, non una metrica.** Un giro con una V4
   fallita non produce nessun punteggio: si guarda quella e basta.
5. **Gli episodi non si tolgono.** Un episodio troppo difficile si annota come
   tale, non sparisce. Ciò che si toglie dal catalogo smette di misurare.

---

## LA PROMOZIONE — quando un episodio si chiude a buon mercato

Prima o poi succede, e succederà spesso: un episodio si chiude **imparando un
fatto solo**. Nessuna regola, nessuna facoltà — è bastato sapere quella cosa.

Non è un imbroglio ed è anzi il funzionamento normale del ciclo. Ma se resta
così, la batteria si degrada da sola: ogni fatto imparato lascia dietro un
episodio che d'ora in poi passa sempre, e col tempo la misura diventa un
inventario di cose già risolte.

**La regola (F.): il fatto resta, l'episodio si rimaneggia.**

1. **Il fatto imparato NON si toglie.** È conoscenza guadagnata, sta nella KB
   accanto ai suoi simili e ci rimane. Toglierlo per far tornare rosso
   l'episodio sarebbe l'amputazione che questo progetto rifiuta ovunque: si
   misura su cose nuove, non spegnendo quelle vecchie.
2. **L'episodio chiuso cambia mestiere.** Migra da `tests/rl/` a `tests/p0t/`:
   ha smesso di essere una misura ed è diventato un **cricchetto**, che è
   esattamente il posto di ciò che è stato conquistato e non deve regredire. La
   batteria non lo perde — lo promuove.
3. **Nasce il successore, deliberatamente più difficile**, con lo stesso id più
   un suffisso di generazione e il campo `genitore` nel manifest. Il vincolo di
   qualità è uno solo, e va rispettato o il successore non vale niente:

   > il fatto appena imparato dev'essere **necessario ma non sufficiente**.

   Se il successore si chiude senza usarlo, non è un successore: è un altro
   episodio facile. Se si chiude con quello solo, la scalata non è avvenuta.

**Su quale dimensione si sale.** Non a caso: si sale su quella che la chiusura a
buon mercato **non ha toccato**.

| se ha chiuso... | si sale su | e diventa |
|---|---|---|
| sapendo *quel* fatto | **novità** | stesso lavoro, entità coniate — il fatto non si applica più, serve la regola |
| con un salto solo | **profondità** | il fatto diventa una premessa, non la risposta |
| a richiesta singola | **composizione** | il fatto è uno di tre vincoli che devono valere insieme |
| in un turno | **distanza** | il fatto serve otto turni dopo essere stato dato |

La prima riga è la più forte e va provata per prima: **stessa forma, entità
inventate**. È la scalata che distingue in un colpo solo l'aver imparato una cosa
dall'aver imparato a fare una cosa.

**Quando ci si ferma.** Se dopo tre promozioni consecutive il successore continua
a chiudersi con un fatto solo, la scalata si interrompe e si annota il risultato:
in quella cella parrot0 sta costruendo un'**enciclopedia**, non una facoltà. Non è
un fallimento — è un dato, e alimenta direttamente la lettura del verticale (una
cella che accumula bene e non trasferisce è precisamente un candidato).

**Il costo è sostenibile perché è mirato.** Si rimaneggiano solo gli episodi
chiusi a buon mercato, che sono un sottoinsieme dei chiusi, che sono un
sottoinsieme di tutti. Il dataset cresce **dove parrot0 è cresciuto**, e in
nessun altro punto: è questo che lo tiene sempre appena oltre la sua portata.

---

## COME SI PUNTEGGIA, E I RISULTATI ATTESI

Cinque numeri, tutti stampabili in fondo a un giro:

| misura | cosa dice | dove deve andare |
|---|---|---|
| **episodi chiusi** | quanti lavori sono stati portati a termine | sale |
| **chiusi da solo** | ... di cui senza che nessuno abbia scritto un fatto o toccato il C | sale, ed è **la misura vera** |
| **regressioni** | episodi che passavano e non passano più | zero |
| **V4 fallite** | risposte false, premesse false accettate | zero, sempre |
| **costo** | turni e passi di inferenza per episodio chiuso | non deve esplodere |

Ma il totale è il numero meno interessante. Quello che serve a decidere è la
**matrice stampata**: le cinque misure per cella, più le due letture trasversali
che il vettore rende possibili —

- **per dimensione**: la resa a `distanza 3`, a `rumore 3`, a `novità 3`, ignorando
  famiglia e dominio. Dice se c'è **una** cosa da riparare invece di ottanta.
- **per macro-area a novità 3**: la classifica onesta fra domini, quella che non
  premia i domini dove la KB è già cresciuta.

**Il risultato atteso non è un punteggio alto.** All'inizio la batteria deve
essere in gran parte rossa: se al primo giro passa più della metà, gli episodi
sono troppo facili e la batteria non misura niente. Le attese, per fase:

- **F0** — il catalogo esiste, gli episodi girano, e il numero di partenza è
  noto. Nessuno si aspetta che sia buono; si aspetta che sia **vero**.
- **F1** — gli episodi falliti producono lacune registrate invece di silenzio.
  È il ponte con gen406: un fallimento che non lascia traccia non è utilizzabile.
- **F2** — il primo episodio chiuso **da solo**: il ciclo del sogno propone un
  ponte, la batteria lo conferma, il ponte sopravvive al processo. È il criterio
  che vale per tutto il piano.
- **F3** — la chiusura autonoma diventa ripetibile su una famiglia intera, e il
  rapporto «chiusi da solo / chiusi» comincia a essere una serie storica.
- **F4** — la batteria cresce da sé: gli episodi falliti in una famiglia
  generano episodi della famiglia *macchineria*, e il ledger mostra il ciclo.

---

## IL PIANO IN FASI

Ogni fase ha un criterio che è **una misura che cambia**, mai «funziona».

### F0a — Il formato, scoperto usandolo

Tre episodi scritti a mano — uno di comprensione, uno di codice, uno di onestà —
girati col motore di test com'è. Serve a scoprire cosa manca al formato
**usandolo**, non progettandolo. Da lì si fissano intestazione, manifest e ledger.

**Criterio:** i tre episodi girano senza modifiche al motore, oppure è chiaro e
scritto quale sola estensione serve.

### F0b — La prima passata a tappeto

Un episodio per cella, gradino 1: **72 episodi**, nove macro-aree per otto
famiglie. È molto lavoro di scrittura e poco lavoro di codice, ed è
deliberatamente così — la parte cara di questo piano è il corpus, non il runner.

Le celle si scrivono in ordine di **facilità di verifica**, non di interesse:
prima quelle dove esiste una V1 o una V3 ovvia, per ultime quelle dove si finisce
a dipendere da V2. Una cella per cui non si riesce a immaginare una verifica non
aggirabile si annota come vuota, con la ragione.

**Criterio:** la matrice 9×8 è stampata con la sua linea di partenza per cella,
registrata nel ledger con la generazione. Nessuno si aspetta che sia buona; si
aspetta che sia **vera**.

### F0c — Il correttivo di novità

Ogni cella con segnale viene raddoppiata a `novità 3`, con entità coniate.

**Criterio:** esiste una seconda classifica fra macro-aree, quella a novità
piena, e si può confrontare con la prima. La distanza fra le due è la misura di
quanto la batteria stava premiando la KB invece della facoltà.

### F1 — Il fallimento diventa materiale

Un episodio fallito deve lasciare una lacuna con la sua ancora — cosa nominava,
cosa se n'era capito, quale facoltà è arrivata più vicino. Oggi succede per i
muri del dialogo (gen406); qui deve succedere per **i lavori**, che possono
fallire pur avendo risposto a ogni turno.

**Criterio:** il numero di lacune aperte dopo un giro di batteria è > 0 e ogni
lacuna nomina il suo episodio.

### F2 — Il primo lavoro chiuso senza mano umana

Il sogno lavora sulle lacune lasciate dalla batteria, propone, verifica
riponendo l'episodio intero — non il turno singolo — e tiene solo ciò che chiude
l'episodio senza romperne altri.

**Criterio:** almeno un episodio passa da rosso a verde **senza che nessuno
scriva un fatto**, e resta verde dopo un riavvio.

### F3 — Il gradino e il fascio

Le varianti nascoste entrano in uso. Si misura la distanza fra «passa
l'episodio» e «passa il fascio», che è la misura di quanto la chiusura è
strutturale.

**Criterio:** per la prima famiglia, il tasso sul fascio nascosto è entro pochi
punti da quello sul catalogo.

### F4 — La batteria che cresce

Due sorgenti di crescita, e sono opposte: gli episodi **falliti** generano
episodi di macchineria, gli episodi **chiusi a buon mercato** generano i loro
successori più difficili. Il ledger diventa la storia del progetto misurata
invece che raccontata.

**Criterio:** un episodio nato da un fallimento della batteria stessa viene
chiuso dal ciclo autonomo, e almeno un successore da promozione è più difficile
del genitore in modo verificabile (il genitore passa, il successore no).

### F5 — La scelta del verticale

Si leggono i quattro indicatori sulla matrice — resa strutturale, pendenza,
trasferimento, tenuta a novità — e si nomina **una** cella. Poi si scende nei
domini specifici di quella macro-area invece che nella macro-area intera, e la
batteria smette di essere larga e diventa profonda **su un punto solo**.

**Criterio:** la scelta del verticale è **derivata dalla matrice e scritta con i
numeri che l'hanno prodotta**, non argomentata. E se i numeri non indicano
niente, si dice quello — allargare è un esito legittimo, sceglierlo lo stesso no.

---

## COSA FALSIFICA IL PIANO

- Se per far salire «chiusi da solo» serve regolarmente toccare il C, allora la
  conoscenza non è il posto giusto per quella classe di lavori, e va detto invece
  di aggirarlo.
- Se il tasso sul fascio nascosto resta molto sotto quello sul catalogo anche
  dopo F3, la batteria sta misurando memorizzazione: il formato degli episodi è
  sbagliato, non il motore.
- Se le V4 non arrivano mai a zero, il problema non è la crescita ma la
  disciplina di risposta, e ha precedenza su tutto il resto.
- Se il costo per episodio chiuso cresce più in fretta degli episodi chiusi, il
  ciclo sta comprando risultati con il tempo, e la misura giusta diventa quella.

---

## DA DOVE SI COMINCIA

Nessun codice, per ora. L'ordine è:

1. **Scrivere tre episodi a mano** — uno di comprensione, uno di codice, uno di
   onestà — al gradino 1 e in tre macro-aree diverse, e girarli con il motore di
   test com'è. Serve a scoprire cosa manca al formato **usandolo**, non
   progettandolo.
2. **Decidere il formato definitivo** dell'intestazione, del manifest e del
   vettore sulla base di quei tre.
3. **Riempire la matrice 9×8** al gradino 1, una cella alla volta, in ordine di
   facilità di verifica.
4. **Registrare la linea di partenza** per cella nel ledger, con la generazione.

Il resto del piano non si tocca finché quei tre episodi non girano.

**La tentazione da evitare, per tutta la durata di F0:** guardare i primi
risultati e cominciare a scavare dove sembrano promettenti. È esattamente il
modo di scegliere il verticale per intuito, con l'aggravante di sembrare
motivato dai dati. La matrice si riempie **tutta** prima di guardarla.
