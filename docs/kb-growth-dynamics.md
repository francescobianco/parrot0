# La dinamica della KB che cresce — viste, cicli, modi, costo

*Aperto il 27 settembre 2026, su richiesta di F., alla fine degli incrementi
4–7 di L4 ([l4-upgrade.md](plans/l4-upgrade.md)):*

> *«questo troubleshooting lungo deve essere anche capitalizzato come teoria
> sullo studio della crescita della KB: essa diventerà complessa e
> interdipendente, quindi poter fare tesoro di queste esperienze e renderle
> lezioni di analisi, crescita e miglioramento della KB è importante»*

[procedura-crescita-kb.md](plans/procedura-crescita-kb.md) è il metodo di
lavoro: come si sceglie, si verifica e si accelera. Questo documento tratta
un'altra cosa: **come si comporta la KB mentre cresce**, cioè quali forze
compaiono quando le regole si appoggiano l'una sull'altra. Ogni specie qui
sotto ha un caso misurato, il segnale con cui si riconosce in `/debug on` e la
cura che ha funzionato. Chi aggiunge una specie nuova aggiunge una sezione, con
la misura e la data.

---

## 1. Il modello: una rete di viste, e due tipi di turno

Le regole che la comprensione chiede a ogni turno si **congelano** in viste
(`materialized_view/2`): la vista si calcola una volta e poi si legge come una
tabella. Una vista si **invalida** quando cambia qualcosa da cui dipende
(`view_depends/2`, più le premesse delle sue regole), e si ricostruisce alla
prima domanda.

Ne seguono due tipi di turno, con costi di ordine diverso:

| turno | che cosa succede | costo tipico |
|---|---|---|
| **ordinario** | le viste sono vive e si leggono come tabelle | 0,3–1 s |
| **di ricostruzione**, il turno *dopo* una lezione che tocca ciò da cui le viste dipendono | le viste sporche si ricalcolano; mentre una vista è in ricostruzione le sue regole si valutano dal vivo | da 3 s a 30 s, o un blocco |

**La regola di confine** (`view_close` in [kb.c](../src/kb.c)): *solo una vista
viva fa da confine*. Mentre si ricostruisce, una vista non ferma più la
ricerca, e le regole che la attraversano scendono fino in fondo. Una lezione
che sporca **più viste insieme** toglie i confini proprio dove servono. Da qui
la prima legge della crescita:

> **Il costo della crescita non si paga dove si scrive la conoscenza, ma nel
> turno dopo, quando le viste si ricostruiscono.** Si misura quel turno.

## 2. Lo strumento: leggere `/debug on`

| segnale nel profilo | che cosa vuol dire |
|---|---|
| `chiamate N regole di X` con X una vista | X si sta valutando dalle regole: non è viva (sporca o in ricostruzione) |
| `visite V  C cammini  goal A <- B` | B chiede A C volte e scorre V fatti: V/C alto = scansione senza indice o con la chiave sbagliata |
| `proprio T ms … goal A` | tempo speso in A stesso, non nei figli |
| un predicato che compare come figlio di sé stesso attraverso altri (`derived_like <- turn_form <- … <- derived_question`) | ciclo fra viste |
| molti passi in poche chiamate | la chiamata ricostruisce qualcosa |
| pochi passi e molto tempo | costruzione di indici o di viste, non ricerca |

Si misura con `!debug on` nel `.p0t`, subito prima del turno sospetto e
dentro la **sequenza vera**: lo stesso turno da solo può costare un terzo,
perché le forme accumulate dalle lezioni precedenti moltiplicano la
ricostruzione (§3, S7).

## 3. Le specie

### S1 — Il nome composto che nessun indice serve

**Caso** (incremento 4): le forme derivate si chiamano con la loro derivazione
(`inverted(D, W)`, `supported(D, W)`). L'indice sul primo argomento valeva solo
per gli atomi, quindi ogni domanda su una forma derivata scorreva tutti i
fatti `turn_form`: **4,3 milioni di visite** in un turno di lezione.
**Segnale:** visite/cammini ≈ numero dei fatti del predicato.
**Cura:** nel motore, se nessun fatto ha un composto in quella posizione, un
goal composto ground ha la fetta vuota (`pred_bucket_a0_compound`).
**Legge:** nominare per derivazione è giusto (il nome si legge, e una forma
scritta a mano non unifica con le teste derivate), ma chiede che il motore
serva quei nomi.

### S2 — La scrittura dentro la prova che sporca una vista

**Caso** (incremento 5): il contabile della lezione faceva `assert` della
condizione nuova *prima* che si calcolasse la risposta. `derived_question`
diventava sporca a metà turno, e ogni domanda successiva la riderivava dal
vivo: **35 s**, 42 000 cammini.
**Segnale:** una vista valutata dalle regole in un turno che non ha avuto
lezioni *prima*.
**Cura:** scrivere dopo la risposta (`after_reply_bookkeeper`), e calcolare la
risposta dalla lezione, non dal fatto scritto.
**Legge:** chi scrive un fatto da cui dipende una vista enumerata a ogni turno
lo scrive dopo la risposta.

### S3 — La portata non dichiarata, spostata di posto

**Caso** (incremento 7): un `apply` spostato da `operator_like_form` in un
aiutante (`operator_member`) ha reso il turno dopo un'analogia **bloccato oltre
120 s**. `apply` e `kb_fact` nel corpo **spengono la clausola** durante il
congelamento, a meno che il predicato che li contiene non dichiari la propria
portata (`view_apply_resolved/1`). La dichiarazione c'era per il vecchio
contenitore, non per il nuovo.
**Segnale:** blocco, o una vista che non si congela più dopo un refactoring
«innocuo».
**Cura:** la dichiarazione segue il costrutto, non il nome del predicato di
prima.
**Legge:** spostare una meta-chiamata cambia il modo in cui la rete si congela.
Il refactoring in KB non è neutro.

### S4 — Il ciclo spurio fra viste

**Caso** (incremento 7): `operator_form` («quali forme dichiarative hanno una
classe di operatori al secondo posto?») chiedeva `turn_form`, e così entrava
in tutte le regole delle forme derivate. `derived_question` dipendeva da
`derived_like`, e `derived_like` da `derived_question`. Nella ricostruzione
nessuna delle due poteva fare da confine: **7,7 milioni di visite, 20 s**. Il
ciclo era spurio: nessuna forma derivata ha una classe in quel posto.
**Segnale:** una vista che compare fra i propri discendenti nel profilo; il
registro dei paradossi (`debug_paradox`) mostra `cycle` sulla vista.
**Cura:** leggere i **fatti scritti** (`kb_fact`) con la portata dichiarata. Il
motore ora ammette `kb_fact` fra le meta-chiamate risolte
(`view_apply_resolved`). Da 20,7 a 7,0 s.
**Legge:** quando una regola chiede un predicato che ha sia fatti scritti sia
regole derivate, chiedersi *quale dei due le serve*. La semantica non ha il
ciclo; lo introduce una domanda più larga del necessario.

### S5 — L'ordine dei goal ha un modo

**Caso** (incremento 7): gli spostamenti di posizione delle forme derivate
sono diventati una grammatica in KB (`position_step/2`, `position_beyond/2`)
invece di `lt`/`is`, che valgono in un verso solo. Messa **in testa** al
corpo, taglia i rami impossibili quando la posizione arriva legata (turno di
ricostruzione: da 5,0 a 3,9 s), ma moltiplica per 12 l'enumerazione quando la
forma è legata e la posizione libera, che è il caso di ogni turno (la lezione
ripetuta passava da 1,6 a 5,3 s). Messa **in coda**, vince il caso frequente.
**Segnale:** una modifica che accelera un turno e ne rallenta un altro.
**Cura:** misurare i due modi; scegliere per il caso frequente; scrivere il
modo nel commento.
**Legge:** una regola non ha un costo, ne ha uno per modo di chiamata (quali
argomenti arrivano legati). Il modo è parte della conoscenza della regola.

### S6 — La relazione fissa ricorsiva

**Caso**: `position_beyond/2` ricorsiva, chiamata 13 700 volte in un turno,
scendeva ogni volta lungo la catena: **3 s**. Congelata come vista binaria:
trascurabile.
**Legge:** una relazione che non cambia e si chiede spesso si congela.

### S7 — La regola che moltiplica le forme

**Caso** (incremento 7): un'analogia estesa al paradigma («dares» come
«needs» ⇒ «dared» come «needed», «dare» come «need») crea tre forme sorelle
invece di una. Ogni forma derivata è un nodo in più in ogni ricostruzione
annidata: il turno passava da 5 a **27 s**. **Non adottata**: l'analogia resta
per superficie, e il passato si insegna con la sua lezione («"dared" behaves
like "needed"»).
**Legge:** crescere per derivazione moltiplica le forme, e il costo delle
ricostruzioni cresce col prodotto, non con la somma. Prima di una regola che
genera forme, i confini (S4) devono tenere.

### S8 — La scansione ripetuta per coppia

**Caso**: la clausola dell'analogia scorreva tutte le forme dichiarative per
ogni coppia prima di sapere se i due termini erano verbi.
**Cura:** i filtri economici e selettivi prima (sono verbi? la vecchia parola
occupa il posto dell'operatore?), le scansioni dopo e una volta sola.

### S9 — La vista che costa al boot

**Caso**: congelare `operator_member` come vista binaria ha portato il boot
oltre i 15 s concessi al demone di test: congelare al boot significa derivare
tutto quello da cui dipende, forme derivate comprese. Il boot è già 12–14 s
(`!reset` costa lo stesso): il margine è piccolo e comune a tutti.
**Legge:** una vista nuova si paga anche al boot. Si misura `make test-engine`
dopo averla aggiunta.

### S10 — Costruire invece di smontare

**Caso** (incremento 4): per riconoscere «verbs» come plurale di «verb»,
`language_term_form` attaccava la desinenza a *ogni* termine con liste di
caratteri: 1,6 s dentro `app` in un turno. Staccare la desinenza dalla parola
detta con `concat_atoms` in modo divisione costa quasi niente.
**Legge:** dal dato verso la conoscenza, non dalla conoscenza verso il dato,
quando il dato è uno e la conoscenza è tanta.

### S11 — L'ordine di ricostruzione deciso dalle righe del file, e il grafo falso

**Caso** (27 settembre, sera): nel turno dopo la seconda analogia
`derived_question` si ricostruiva **tre volte**: la prima (2 223 ms) con
`derived_like` ancora spenta, poi invalidata da `derived_like` (262 ms), poi
invalidata da `supported_carry` (226 ms). Il motore ricostruisce le viste
sporche in ordine di dipendenza (`kb_views_refresh`, `kb_view_ensure`), ma
legge solo le dipendenze **dichiarate dalla vista stessa**. Quelle vere erano
dichiarate su `question_inversion`, che non è una vista, e così decideva
l'ordine delle righe del file. E il grafo calcolato era falso: la derivazione
chiedeva `turn_form`, quindi conteneva le regole di tutte le forme derivate
(cicli e invalidazioni mai percorsi a runtime).
**Segnale:** nella traccia (`!debug depth 3` + `!debug turn`, righe `view`) la
stessa vista compare più volte, con `invalidata da` fra una e l'altra, e righe
`ciclo:`.
**Cura:** (1) la derivazione legge le forme SCRITTE (`written_piece`,
`written_act`, `written_statement` su `kb_fact`, portata dichiarata) e le
sorelle per analogia da una sola definizione (`like_piece/4`): il grafo
diventa un DAG vero; (2) `view_depends(derived_question, derived_like)`: l'ordine
è conoscenza; (3) `kb_fact` con il primo elemento della lista legato usa la
fetta dell'indice invece del predicato intero (0,7 ms a chiamata prima).
Risultato: `derived_like` → `derived_question` (una volta, 195 ms) →
`supported_carry`; il turno da 7,0 a **3,7 s** (era 20,7 s prima di S4).
**Legge:** l'ordine di ricostruzione è parte della conoscenza della vista, e un
grafo delle dipendenze onesto vale più di un motore che indovina l'ordine. Chi
dichiara una dipendenza la dichiara sulla VISTA, non su un predicato interno.

### S12 — Il tetto di profondità che limitava la lunghezza della prova

**Caso** (27 settembre, sessione di cinque ore, train-the-smart-agent §10):
tre composizioni KB-first corte, ciascuna vera pezzo per pezzo, fallivano
**in silenzio** quando si mettevano in fila: la scelta con una relazione
calcolata (`ordered_pair` seguito da `choice_reason`), il verdetto con un
soggetto di più parole (`comparative_verdict_clause`), la ragione dopo la
coppia ordinata. Ogni sonda isolata reggeva; la catena no.
**Causa:** il risolutore procede a continuazioni. Il corpo di una regola e il
resto della congiunzione del genitore stanno nella stessa lista, e la
profondità passata a quella lista (`depth + 1`) valeva anche per i goal
**fratelli** che seguono. `KB_MAX_DEPTH` (64) limitava così la **lunghezza** di
una prova, circa 64 espansioni in tutto, non la profondità dell'albero che il
suo commento promette.
**Segnale:** ogni pezzo regge da solo e la congiunzione no; una sonda con un
goal in più fa fallire ciò che prima riusciva; nessun muro, nessun budget
esaurito nel profilo.
**Cura** ([kb.c](../src/kb.c), campo `depth` di `Term`): ogni goal di un corpo
porta la profondità del nodo che l'ha posto (quella del goal espanso + 1); i
goal della continuazione conservano la propria; `solve` usa quella del goal
quando c'è. 64 torna a essere la profondità dell'albero. Due false piste
pagate prima di trovarla: il «ciclo tagliato globalmente» (il marcatore
`__end_inference_scope` chiude già correttamente l'ambito degli antenati) e
il ricalcolo (portare i valori calcolati nella coppia ordinata aiutava solo
perché accorciava la prova).
**Legge:** in una KB che cresce per composizione, la lunghezza delle prove
cresce con lei. Un tetto che conta la lunghezza fa fallire proprio le
composizioni più corrette, e in silenzio. Un limite si misura su ciò che dice
di misurare.

## 4. Il metodo che ha funzionato, e l'errore da non ripetere

1. **Misura, poi ipotesi.** Ogni volta che ho indovinato la causa senza profilo
   (la dipendenza di vista, il bisogno al passato) ho perso un giro.
2. **Isolare togliendo una clausola**, non con stash o branch. Salvare il file
   intero nello scratchpad e ripristinarlo.
3. **Tenere il manifesto dei file scambiati.** In una bisezione ho rimesso il
   `language-lessons.p0` di HEAD e ripristinato solo `grammar.p0`: la misura
   successiva sembrava una regressione del comportamento, ed era il file
   sbagliato. Prima di interpretare una sonda dopo una bisezione:
   `git diff --stat` contro ciò che ci si aspetta.
4. **Il turno giusto è quello nella sequenza vera**, con le lezioni precedenti
   (S7: 7 s nel contesto contro 4 s da solo).
5. **Le sonde `!query` mentono sulle forme**: una costante `"travel time"`
   in un `!query` perde le virgolette, dentro una regola no; `$T = ok` non
   esiste (niente `=/2`). Una sonda di diagnosi si scrive come **regola** nel
   file, con la stessa forma del flusso vero, e si toglie dopo.
6. **A/B contro HEAD** per ogni rosso di tempo prima di attribuirselo (il
   turno «capital of france» di `basics.p0t` sta a 1,0 s con e senza le
   modifiche di oggi: non era mio).

## 5. La teoria, in cinque proposizioni

1. **La crescita si paga in ricostruzione.** Il costo di una lezione si vede
   nel turno dopo, e cresce con il numero di viste che invalida insieme.
2. **Una vista è confine solo se viva.** Le lezioni che toccano insieme lessico
   (verbi) e regole (analogie) tolgono i confini proprio nel turno che ne ha
   bisogno.
3. **Le domande larghe creano cicli che la semantica non ha.** Chiedere «tutto
   `turn_form`» quando servono i fatti scritti lega viste che non dipendono
   davvero l'una dall'altra (S4). Quale parte di un predicato serve (fatti o
   regole, scritto o derivato) è conoscenza da dichiarare.
4. **Ogni regola ha un costo per modo.** L'ordine dei goal decide quale modo è
   economico. Il modo frequente si misura e si scrive (S5).
5. **Derivare moltiplica.** I nomi-derivazione, le sorelle per analogia e le
   domande derivate sono crescita vera, e ognuna è un nodo in più in ogni
   ricostruzione (S7). La KB viva resta sostenibile se i confini tengono.

## 6. Questioni aperte (motore)

- **L'ordine di ricostruzione dal grafo calcolato.** Oggi l'ordine segue le
  dipendenze dichiarate dalla vista (S11). Seguire anche il grafo calcolato
  (`v->deps`) lo renderebbe automatico, ma solo dove il grafo è onesto: molte
  viste hanno ancora cicli spuri (`cortocircuito`, `ibrida` nella traccia), e
  farle attendere le lascerebbe spente a inizio turno. Prima i grafi, poi il
  motore.
- **Il costo di un verbo nuovo.** Ogni lezione che tocca i verbi ricostruisce
  `extract_frame` (~3 s): è il pavimento di tutti i turni di ricostruzione
  misurati qui.
- **Il boot** a 12–14 s contro un limite di 15 s (S9).
- **Un'analogia per paradigma** (S7) tornerà quando i primi due punti saranno
  risolti.
