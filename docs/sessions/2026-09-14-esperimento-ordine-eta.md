# Esperimento — quanto costa insegnare a parrot0 un problema di età e incontri

**Data:** 14 settembre 2026, 10:18–11:17 (59 minuti di orologio, misurati dai
timestamp della sessione). **Richiesta di F.:** misurare il tempo e l'impegno di
trasformazione e crescita necessari perché parrot0 risponda al meglio a questo
prompt, e lasciare tutto committato.

> Anna è più giovane di chiunque abbia incontrato Bruno prima di martedì, ma non di
> chi Bruno incontrò martedì. Carlo incontrò Anna mercoledì e Bruno giovedì. Nessuno
> incontrato da Bruno dopo Carlo è più anziano di Anna.
> Senza assumere informazioni non esplicitamente implicate dal testo, stabilisci chi è
> necessariamente più giovane di chi e indica anche quali confronti di età restano
> indeterminati.

## 1. La soluzione di riferimento (a mano, prima di toccare il codice)

Siano P = chi Bruno ha incontrato prima di martedì, T = chi ha incontrato martedì,
D = chi ha incontrato dopo l'incontro con Carlo (giovedì).

- Frase 1: Anna < ogni membro di P (strettamente più giovane); e Anna **non** è più
  giovane di chi sta in T, cioè ogni membro di T ≤ Anna (stessa età o più giovane).
- Frase 2: incontri datati (Carlo–Anna mercoledì, Carlo–Bruno giovedì). Non mettono
  nessun nome in P, T o D, e non escludono altri incontri.
- Frase 3: ogni membro di D ≤ Anna.

Necessario: Anna < P; T < P e D < P (per transitività attraverso Anna, stretta perché
un anello è stretto); T ≤ Anna e D ≤ Anna (l'età può coincidere); **Anna non è in P**
(sarebbe più giovane di sé); nessuno sta insieme in P e in T, né in P e in D.
Indeterminato: Anna–Bruno, Anna–Carlo, Bruno–Carlo; Bruno e Carlo rispetto a ciascun
gruppo; T rispetto a D. Nessun gruppo è detto non vuoto: ogni frase su un gruppo vale
per i suoi membri, se ne ha.

## 2. La base: che cosa faceva parrot0 alle 10:19

In `make chat`, il prompt come scritto (con l'a capo, quindi due turni):

```
>>> Leggo «prima» come «primo». Imparato: anna is more giovane than chiunque abbia
    incontrato bruno prime of tuesday, ma not of. Non capisco ancora.
>>> Non so ancora tradurre «senza». … Imparato: senza assumere informazioni not
    esplicitamente implicate from the testo, stabilisci chi confronti of age restano
    indeterminati.
```

Due fatti falsi scritti in KB e annunciati come appresi, nessuna risposta. Il punto di
partenza reale era: transitività dei comparativi in inglese («a is taller than b …»),
nessuna conversa «più anziano», nessun incontro, nessun quantificatore con relativa,
nessuna nozione di «necessario/indeterminato».

## 3. Dopo: che cosa risponde alle 11:17

Stesso prompt, stessi due turni in `make chat`:

```
>>> Ho letto il testo come premesse di un problema, non come fatti da imparare. Che cosa
    devo stabilire? Per esempio chi è necessariamente più giovane di chi, e quali
    confronti restano indeterminati.
>>> Dal testo segue necessariamente (una frase su un gruppo vale per i suoi membri, se ne
    ha: il testo non dice che esistano):
- Anna è più giovane di chiunque Bruno abbia incontrato prima di martedì.
- Chiunque Bruno abbia incontrato martedì è più giovane di chiunque Bruno abbia incontrato prima di martedì.
- Chiunque Bruno abbia incontrato dopo Carlo è più giovane di chiunque Bruno abbia incontrato prima di martedì.
- Chiunque Bruno abbia incontrato martedì non supera in età Anna (l'età può coincidere).
- Chiunque Bruno abbia incontrato dopo Carlo non supera in età Anna (l'età può coincidere).
- Anna non può essere fra chiunque Bruno abbia incontrato prima di martedì.
- Nessuno è insieme fra chiunque Bruno abbia incontrato martedì e fra chiunque Bruno abbia incontrato prima di martedì.
- Nessuno è insieme fra chiunque Bruno abbia incontrato dopo Carlo e fra chiunque Bruno abbia incontrato prima di martedì.
Restano indeterminati:
- Anna e Bruno.
- Anna e Carlo.
- Bruno e Carlo.
- Bruno e chiunque Bruno abbia incontrato martedì.   (… e gli altri due gruppi)
- Carlo e chiunque Bruno abbia incontrato martedì.   (… e gli altri due gruppi)
- Chiunque Bruno abbia incontrato martedì e chiunque Bruno abbia incontrato dopo Carlo.
Letture: i giorni sono della stessa settimana; incontrarsi è reciproco (chi ha incontrato
qualcuno è stato incontrato da lui); negare un confronto stretto lascia possibile la stessa età.
«Dopo Carlo» è dopo l'incontro di Bruno con Carlo (giovedì).
```

Ogni punto della soluzione di riferimento c'è; nulla è stato imparato; le tre letture
che il ragionamento usa sono dichiarate. Lo stesso vale col prompt in un turno solo.
Tempo del turno: ~1,3 s.

**Non è un frasario** (verificato nel cricchetto): con altri nomi, altri giorni e il
comparativo nel verso opposto («Marta è più anziana di chiunque abbia incontrato Luca
prima di venerdì …») la risposta è quella giusta per quel problema; lo specchio inglese
(«Anna is younger than anyone who met Bruno before Tuesday, but not than whoever Bruno
met on Tuesday …») risponde in inglese; un secondo problema nella stessa sessione
sostituisce il primo.

## 4. Cronologia misurata

| Ora | Durata | Che cosa |
|---|---|---|
| 10:18–10:19 | 1′ | base misurata in chat |
| 10:19–10:23 | 4′ | esplorazione: comparativi, canone italiano, cue del turno |
| 10:23–10:26 | 3′ | disegno; **ragionatore generale** `order-determinacy.p0` (10 verifiche verdi) |
| 10:26 | — | trovata e tolta una clausola illeggibile salvata in learned.p0 |
| 10:28–10:34 | 6′ | **bug chiesto da F. durante l'esperimento, contato qui**: `attenuated_reading` si salvava con la frase nuda, una virgola la spezzava al boot (vedi §6) |
| 10:34–10:44 | 10′ | il testo con compito non si spezza né si impara; protocollo delle risposte in parti; lettore delle premesse |
| 10:44–10:56 | 12′ | limiti del motore trovati uno per uno (§6): arietà 4, corpo 16, legami 384, modello di `findall`, legame da 512 byte |
| 10:56 | — | **prima risposta completa** (turno unico) |
| 10:56–11:04 | 8′ | resa neutra e note derivate; variante con altri nomi e verso; inglese; cricchetto (21) |
| 11:04–11:14 | 10′ | il prompt reale in `make chat` è **due turni**: premesse senza compito; crash con `retract` a metà risoluzione → problemi numerati; legami esauriti → contabili a stadi; cricchetto (28) |
| 11:14–11:17 | 3′ | costo del turno base (+0,2 s) curato con una guardia sulle cue |

Senza il bug delle virgolette: ~53 minuti. Circa un terzo del tempo è andato ai limiti
silenziosi del motore, non alla conoscenza.

## 5. L'impegno: che cosa è cresciuto

**KB (la parte che conta):**
- `kb/core/order-determinacy.p0` (71 righe) — facoltà **generale**: dati vincoli stretti
  (`lt`) e larghi (`le`) su individui e gruppi, che cosa è necessariamente sotto, che cosa
  non è sopra, quali coppie restano indeterminate, quali appartenenze sono escluse, quali
  gruppi sono disgiunti. Non sa niente di età né di incontri.
- `kb/core/problem-texts.p0` (556 righe) — riconoscere un testo con compito o solo le
  premesse; il lessico del problema per lingua (comparativi con verso sulla scala,
  quantificatori di persona, forme di «incontrare», marcatori di tempo, ordine della
  settimana); i gruppi descritti («chiunque abbia incontrato B prima di D», «chi B incontrò
  D», «incontrato da B dopo X», e gli equivalenti inglesi); i tre schemi di vincolo
  (affermato, contrastato «ma non di», «nessuno … è più X di»); gli incontri coordinati;
  la risposta in parti, in italiano e in inglese.
- `kb/core/turn-frames.p0` (4 righe) — un testo di problema non è `compound_statement`,
  `compound_inquiry` né `prose_carried`.

Un comparativo nuovo («più alto», «taller») è una riga di `comparative_word/3`; una forma
nuova di gruppo è una regola `group_at/4`; una lingua è lessico e le righe di risposta.

**C (adattatori, nessun vocabolario):**
- `src/brain/99-registry.c` +42 righe — `turn_response_part(Turn, Ordine, Testo)`: una
  risposta più lunga di un atomo (512 byte) si dà in parti ordinate che il motore mette in
  fila senza sapere che cosa dicano.
- `src/brain/10-memory-knowledge.c` — `p0_quote_text`: una frase come argomento di un fatto
  va tra virgolette (il bug di §6).

**Prove:** `tests/p0t/reasoning/order_determinacy_problem.p0t` (28 verifiche: il prompt in
un turno e in due, la variante, l'inglese, un secondo problema, «nessun Imparato»).
`make soft-test`: gli stessi due rossi di tempo di HEAD; `prose_triage` 75,
`facts_split_three` uguale a HEAD.

## 6. I limiti del motore trovati (vanno in C_TODO)

Ognuno ha fatto fallire **in silenzio** una regola corretta:

1. **`KB_MAX_BIND` 384** — una lettura composta di cinque pezzi, dove ogni pezzo è una
   regola sui token, esaurisce i legami del ramo e l'ultimo goal fallisce senza dirlo.
   Cura in KB: lettura a stadi, pezzi come fatti; contabili separati (ognuno ha la sua
   sostituzione).
2. **`retract` a metà risoluzione** compatta i fatti che un goal antenato sta scorrendo:
   core dump. Cura in KB: niente ritrattazioni, i problemi portano un numero e vale il più
   recente.
3. **`findall/3` raccoglie solo una variabile**, non un modello composto
   (`findall(e($S,$X,$Y), …)` torna vuoto). Cura: un aiutante che lega il termine intero.
4. **Una lista legata tiene 512 byte**: il `findall` di tutti i pezzi di un testo fallisce.
   Cura: finestre di 15 token.
5. **`KB_MAX_ARGS` 4** vale anche per `assert(pred, …)`: a cinque argomenti la clausola è
   scartata al caricamento con un PARSE ERROR che solo il boot mostra.
6. **Una frase salvata senza virgolette** si spezza alle virgole al boot seguente (bug
   chiesto da F.): `attenuated_reading` ora si asserisce tra virgolette, 12 righe migrate;
   ne è sparito anche il giro `concat_atoms($S, "", $K)` della scheda mix-12-04.
7. Per chi misura: in un `.p0t`, le regole `!assert` con testo variabile nelle parti non
   producono, e `!query` dopo un turno che ha risposto non è affidabile: le sonde vanno
   messe nel file KB o in un contabile che asserisce un segno.

## 7. Che cosa manca per «al meglio», e quanto costerebbe

- **Resa**: raggruppare («Bruno e Carlo restano indeterminati rispetto a ciascuno dei tre
  gruppi») invece di elencare; «fra chiunque» è pesante (meglio «fra le persone che»).
  ~30′, solo KB.
- **Appartenenze note**: se il testo mettesse un nome in un gruppo (Bruno incontrò Carlo
  lunedì), i vincoli del gruppo dovrebbero passare al nome. La facoltà ha già il posto
  (`order_edge` sui membri); manca la regola che deriva l'appartenenza dagli incontri datati
  e dall'ordine dei giorni. ~30′.
- **Dire perché** Carlo non è in nessun gruppo (l'incontro di giovedì non ce lo mette, ma
  altri incontri non sono esclusi). ~15′.
- **Copertura linguistica**: altri quantificatori («tutti quelli che», «qualcuno che»),
  altri tempi («fra martedì e giovedì»), altre dimensioni. Righe di lessico e regole
  `group_at/4`; ogni forma ~10′.
- **Motore**: i limiti 1–5 di §6 andrebbero resi visibili (un fallimento per esaurimento
  dovrebbe dirsi incompleto, non falso) — lavoro C, stimato mezza giornata, fuori da questo
  esperimento.

Stima per una versione «al meglio» della stessa famiglia di problemi: altre 1,5–2 ore di
lavoro KB, più il lavoro sul motore.

## 8. Va bene che funzioni: che cosa ci ha fatto guadagnare davvero

La domanda giusta non è «il prompt ha la risposta giusta?», ma quella del MANTRA:
*parrot0 può impararne un nuovo membro domani, senza ricompilare?* Messa alla prova
subito dopo l'esperimento (14 settembre, 11:30), con tre varianti minime, tutte fallite:

| Variante | Che cosa cambia | Risposta |
|---|---|---|
| «chiunque abbia **conosciuto** Bruno prima di martedì …» | il verbo del gruppo | «Imparato: chi è (necessariamente + giovane).» |
| «Anna è più **alta** di chiunque …; chi è più **basso** di chi» | la dimensione | «Imparato: chi è (necessariamente + basso).» |
| «Anna è più giovane di Bruno e Bruno è più giovane di Carlo. Stabilisci …» | niente gruppi, tre nomi | «Imparato: chi è (necessariamente + giovane).» |

La terza è la più istruttiva: il caso **più semplice** della famiglia non è letto,
perché il lettore conosce solo i vincoli verso i gruppi. E in tutte e tre, quando la
lettura non trova vincoli, il turno con il compito ricade sul lettore di prima e
**scrive un fatto falso**. La protezione vale solo quando la lettura riesce: è il
difetto da curare per primo.

Il guadagno quindi va separato in due parti.

### 8.1 Guadagno generale — resta anche se il prompt cambia

1. **Una facoltà nuova: necessario / indeterminato.** Finora parrot0 rispondeva sì, no
   o «non so». `order-determinacy.p0` distingue ciò che i vincoli *impongono*, ciò che
   *lasciano possibile* (la stessa età) e ciò che *non decidono*, e ricava esclusioni
   per assurdo («Anna non può essere fra …: sarebbe più giovane di sé»). Non sa niente di
   età né di incontri: vale per altezze, prezzi, orari di arrivo, classifiche, versioni
   di un pacchetto. È la forma operativa del principio già scritto nei test di
   transitività: *non dimostrato non è falso*. Prima esisteva come onestà nel dire «non
   so»; ora è una risposta costruita.
2. **Una modalità epistemica: le premesse non sono conoscenza.** Ogni dichiarativa
   veniva imparata, e un problema diventava fatti falsi nella KB. Adesso esiste un ambito
   (`problem_task`, `problem_premises`) in cui le frasi valgono *dentro il problema*, si
   numerano e non entrano nel `/save`. È lo stesso ambito che serve ai problemi di
   matematica a parole, agli indovinelli, a «supponi che …», ai controfattuali. Il pezzo
   c'è; oggi lo apre soltanto il riconoscimento di questa famiglia (vedi 8.2).
3. **Un protocollo di risposta: `turn_response_part/3`.** Una risposta composta da
   molte frasi, ordinata e scritta in KB, per qualunque facoltà. Il tetto dei 512 byte
   di `turn_priority_response` spingeva ogni facoltà a risposte da una riga.
4. **Un metodo per i lettori in KB, e la mappa dei loro limiti.** La lettura a stadi
   (pezzi → fatti numerati → composizione, un contabile per stadio) e i cinque limiti
   silenziosi del risolutore (§6, ora in `C_TODO.md`). Probabilmente spiegano altri
   fallimenti muti già visti: una regola corretta che «non combacia» senza motivo può
   aver esaurito i legami.
5. **Un bug di persistenza curato** (`attenuated_reading` senza virgolette): vale per
   ogni lettura salvata, non per questo problema.
6. **Una descrizione come termine.** «Chiunque Bruno abbia incontrato prima di martedì»
   è un referente di prima classe, senza individui inventati: è la risposta concreta al
   difetto del gen506c (`several_pilots` trattato come un nome).

### 8.2 Guadagno locale — è debito, anche se oggi è verde

1. **Un secondo lettore.** `problem-texts.p0` (556 righe) è una grammatica per posizioni
   di token parallela al lettore di prosa (`extract_frame`, la IR universale). Il
   progetto la chiama per nome: *due letture della stessa cosa* (D33/D35). Per questo
   «conobbe» non funziona anche se `conoscere` potrebbe essere un verbo di relazione
   insegnato: il gruppo nasce da `meeting_finite/1`, non da una relazione qualunque.
2. **Lessico duplicato.** I comparativi stanno in `comparative_word/3` e non nella
   conoscenza con cui la transitività inglese già lavora; l'ordine dei giorni è stato
   aggiunto qui invece che nel lessico del tempo. «più alto» manca per questo motivo.
3. **Il riconoscimento è per cue.** «stabilisci chi», «restano indeterminati»: sono un
   frasario del compito, non una lettura dell'atto richiesto.
4. **Non si insegna parlando.** Una forma nuova di gruppo è una regola `group_at/4`
   scritta nel file: un esperto del dominio non potrebbe dirla a voce. Per il MANTRA vale
   zero come apprendimento, anche se è KB e non C.
5. **Non incontra la KB viva.** Le premesse restano isolate: un problema che richiede
   anche conoscenza vera («chi è nato prima di Garibaldi …») non può unire le due cose.

### 8.3 Il saldo, e la prossima mossa che lo rende positivo

L'esperimento ha prodotto tre capacità generali — facoltà di determinatezza, ambito
delle premesse, risposte in parti — e un metodo. Ma le ha agganciate a un lettore
stretto: **oggi la generalità sta dietro il lettore, non davanti.** Il costo misurato
(59′) è per un terzo motore, per un terzo lettore e per un terzo le cose che restano.

Le mosse, in ordine di guadagno:

1. **Chiudere il misclaim**: un turno con un compito non impara mai, anche quando la
   lettura non trova vincoli. Deve dire che cosa non ha saputo leggere. Solo KB.
2. **Portare i vincoli dal lettore universale**: «X è più ADJ di Y» come `extract_frame`
   con il comparativo come relazione ordinata (la transitività inglese esiste già), e i
   gruppi come slot di un frame di relazione qualunque («chiunque abbia R Bruno T»). Così
   ogni verbo e ogni comparativo insegnato a voce entra da solo, e il caso con tre nomi
   funziona gratis. Le 556 righe dovrebbero scendere a una frazione.
3. **Aprire l'ambito delle premesse ad altri atti** («supponi che», problemi a parole) e
   dare alla facoltà di determinatezza altri consumatori (classifiche, orari).
4. **Un banco di varianti**, non un caso: le tre righe della tabella sopra, più l'inglese
   e un problema con conoscenza vera. È questo, non il prompt originale, che misura se
   l'esperimento ha fatto crescere parrot0.

## 9. Abbiamo perso il treno della IR? (analisi a posteriori, stessa sera)

La domanda di F., dopo la revisione del piano multi-hop: se lì la IR era sottovalutata,
qui è andata allo stesso modo? **Sì, e in modo più netto.** Ma la misura dice anche perché
era facile sbagliare, e quale pezzo dell'esperimento resta buono.

### 9.1 Misurato

**Quanto `problem-texts.p0` usa la IR** (conteggio delle chiamate nel file):

| Strato | Chiamate |
|---|---|
| token del turno (`turn_word/3`) | 38 |
| cue del turno (`turn_cue/3`) | 6 |
| strato strutturale della IR (`input_node`, `input_entity_node`, `input_semantic_frame`, `input_binary_assertion`, `input_coordination_node`, …) | **0** |
| lettore di prosa (`extract_frame/2`), spazio del discorso (`discourse_referent/2`) | **0** |

**Che cosa la IR pubblicava per le tre premesse**, dentro il turno (sonda con contabili in
un file KB, poi tolta):

| Strato della IR | Pubblicato |
|---|---|
| token, nodi (`input_node/4`), lingua dello scope, coordinazioni | sì |
| entità nominate («Anna», «Bruno») | **no** |
| frame semantici, asserzioni binarie | **no** |
| classi, lacune | **no** |

### 9.2 Che cosa vuol dire

Due fatti insieme, e servono tutti e due:

1. **Il lettore ha scavalcato la IR.** Ha preso i token, lo strato più basso, e sopra ha
   costruito una **IR sua**: `problem_piece/3` sono nodi (nome, copula, comparativo, gruppo,
   contrasto), `group_at/4` sono descrizioni, `problem_edge/4` sono frame, i problemi numerati
   sono uno scope persistente. Ogni pezzo ha un gemello nella IR universale o nel piano della
   IR:

   | Costruito in `problem-texts.p0` | Il suo posto nella IR |
   |---|---|
   | `problem_name_at/3` («una parola che non è del lessico del problema») | `input_entity_node/4` |
   | `problem_piece(N, K, pv(I, V))` | `input_node/4` con ruolo |
   | `group_at/4` («chiunque abbia incontrato B prima di D») | una **descrizione come nodo** (IR3 del piano multi-hop) |
   | `problem_edge/4`, `meeting_clause_at/4` | `input_semantic_frame(Scope, assertion, order(lt), roles(…))`, `…binary(met)…` (IR5) |
   | `contrast_negated_link_at/3` («ma non di») | coordinazione con negazione e ellissi sul frame precedente (le coordinazioni la IR le pubblica già) |
   | `problem_opened/1` e i fatti numerati | uno scope che dura quanto il problema (IR4) |
   | l'ordine dei nomi per prima comparsa | `input_node_before/…`, l'ordine dei nodi |

   Questa è la definizione esatta del difetto che `lettura-della-prosa.md` misura: un
   secondo lettore della stringa, con la sua idea di dove finiscono le cose. **Ed è la
   causa delle tre varianti fallite di §8**: «conobbe» non è un `meeting_finite`, «più alta»
   non è un `comparative_word`, «Anna è più giovane di Bruno» non ha un gruppo. Un
   consumatore della IR avrebbe ricevuto un frame di relazione qualunque, una relazione
   d'ordine qualunque, un'entità qualunque.

2. **Ma la IR non offriva quello strato.** Per queste frasi italiane la IR non vede le
   entità e non costruisce frame: un consumatore onesto, il 14 settembre, avrebbe trovato
   nodi e coordinazioni e nient'altro. Quindi la scelta non era fra «usare la IR» e «un
   lettore privato»: era fra **far crescere la IR** (entità, descrizioni, frame d'ordine e
   di relazione) e **aggirarla**. Abbiamo aggirato, perché era più veloce e il test diventava
   verde — esattamente la mossa che il MANTRA chiama regressione anche quando funziona.

### 9.3 Che cosa resta buono

Non tutto l'esperimento è dalla parte sbagliata della IR:

- **`order-determinacy.p0` è già un consumatore corretto.** Lavora su `order_edge/4`, non
  sa da dove vengono gli archi: se domani gli archi arrivano da frame della IR, non cambia
  una riga. È la parte da tenere così com'è.
- **La risposta in parti (`turn_response_part/3`)** è neutra rispetto alla IR.
- **Le premesse come ambito non imparato** (`problem_task`, `problem_premises`) sono una
  forza del turno: il loro posto naturale è la IR (`turn_illocution`), e lì stanno già.
- **La mappa dei limiti del risolutore** vale per chiunque costruirà la IR in KB.
- **La lettura a stadi** (pezzi → fatti → composizione) è la forma giusta: sbagliata è la
  sua sede. Gli stadi dovrebbero essere gli strati della IR, non una struttura parallela.

### 9.4 Il saldo, rivisto

§8.3 diceva che «la generalità sta dietro il lettore, non davanti». Ora si può dirlo con
più precisione: **la generalità sta dietro una IR privata.** Il costo nascosto non sono le
556 righe: è che ogni capacità costruita sopra `problem_piece` è invisibile al lettore di
prosa, alla memoria profonda e al piano multi-hop, e ogni loro progresso è invisibile a lei.

E il collegamento con la missione multi-hop è diretto: le mancanze della IR che servono lì
(IR3 descrizioni come nodi, IR4 scope di problema, IR5 frame dalla prosa) sono **le stesse**
che questo esperimento ha ricostruito fuori. Ci sono quindi due lavori separati da non
fare, e uno comune da fare una volta:

1. **Far crescere la IR** con: entità nominate in italiano e in inglese; descrizioni come
   nodi («chiunque abbia R X T», «the protagonist of X», «a historical religious figure»);
   frame d'ordine dai comparativi (`order(lt|le)`, con il verso e la negazione) e frame di
   relazione dai verbi insegnati (riusando `extract_frame/2`, non duplicandolo); uno scope
   di problema che dura più turni.
2. **Riscrivere `problem-texts.p0` come consumatore**: gli archi di `order-determinacy.p0`
   dai frame d'ordine della IR, i gruppi dalle descrizioni, gli incontri dai frame di
   relazione. Il criterio di riuscita è §8: le tre varianti fallite diventano verdi **senza
   una riga di lessico nuova nel file**, e il file si riduce a regole di consumo.
3. **Il banco**: le tre varianti, il prompt originale in uno e due turni, l'inglese, e un
   caso con tre soli nomi — lo stesso banco resta valido prima e dopo, ed è la misura del
   passaggio.
