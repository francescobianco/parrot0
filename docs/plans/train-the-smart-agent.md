# Addestrare l'agente situazionale — train the smart agent

> **⛔ NOVITA' — 28 settembre 2026 (F.): i PONTI FRA PREDICATI e l'asse
> strutturale.** Quando lo stesso concetto sta in due cassetti (`capital_of` e
> `capital_of_country`, argomenti invertiti o arita' diverse) **non si unifica
> riscrivendo la KB e non si scrive una regola per coppia**: si dichiara un
> ponte insegnabile e ritirabile — `predicate_same_of(A, B)`,
> `predicate_reverse_of(A, B)`, `predicate_args_of(A, B, Posti)` — consumato
> in un punto solo, il solver, cosi' che ogni lettore lo veda. Contratto in
> [parrot-p0-syntax.md §19](../parrot-p0-syntax.md); procedura, registro dei
> sintomi strutturali e **test di rigidita'** (ogni cambio di struttura deve
> poter essere contraddetto parlando, dal turno dopo) in
> [train-the-learning-process.md, «▶ ASSE STRUTTURALE»](train-the-learning-process.md).
> Prima di curare un «Learned, poi non so», chiedersi se e' un ponte che manca.

**Piano, 27 settembre 2026.** Nasce da un giro live di F. e dell'agente
(transcript: [2026-09-27-situazionale-differenziale.log](../sessions/live/2026-09-27-situazionale-differenziale.log), §7 qui sotto; il giro di scelta in [2026-09-27-situazionale-scelta.log](../sessions/live/2026-09-27-situazionale-scelta.log), §7-bis). Richiesta di F.:

> *«vedere se parrot0 è in grado di risolvere problemi multi-turno di
> ragionamento che non siano solamente matematici ma anche situazionali […]
> immagina che io devo trovare un problema, a un certo punto non so più cosa
> fare e ho l'idea di spegnere sezione per sezione delle cose e così isolo
> quella corrotta. Questo piano chiaramente deve essere spiegato come
> opportunità, non come piano con condizioni d'ingresso precise: la scoperta
> del piano come una valida soluzione deve essere l'inferenza, quella che a
> livello umano potremmo chiamare intuizione.»*

> *«senza fare banchi nuovi, descrivendo esperimenti di intelligenza
> situazionale come questi […] facendo uso del live-train, quindi non dobbiamo
> reinventare questo framework […] colmando tutte le lacune di comprensione.
> Non voglio banchi: tutto deve essere nel piano; gli esperimenti si conducono
> con il coding agent come stiamo facendo adesso.»*

> **Due scopi, sempre insieme (F., 27 settembre 2026).** Il piano **mostra** la
> crescita dell'intelligenza situazionale di parrot0, ed è **l'occasione per far
> crescere la KB**: ogni sessione è addestramento, non solo verifica. La
> conoscenza vera dei campi attraversati (leggi causali, stati, azioni e
> effetti), i principi delle famiglie di mosse e le forme di comprensione che
> mancavano restano in KB per il futuro. Una lacuna che si può colmare con
> L4, L3 o L2 **si colma durante la sessione**; un insegnamento che fallisce
> va in testa a [train-the-learning-process.md](train-the-learning-process.md)
> (§2-bis).

> **In una frase.** Una mossa intelligente in una situazione (isolare spegnendo
> una parte alla volta, tornare all'ultimo stato che funzionava, confrontare con
> un gemello sano) non è un piano che scatta quando le condizioni d'ingresso
> combaciano: è una **conseguenza** che parrot0 inferisce dal modello della
> situazione. Una mossa che ha esiti diversi a seconda di quale ipotesi è vera
> *informa*; proporla quando si è bloccati è ciò che chiamiamo intuizione. Il
> piano addestra questa inferenza parlando, colmando una per una le lacune di
> comprensione che la sessione rivela, su scenari veri e variati.

---

## 0. Che cosa questo piano è, e che cosa non è

- **È un curriculum di sessioni live** con il dispositivo esistente,
  [live-teaching.md](live-teaching.md) e `scripts/live-teach.sh`
  (`start`, `say`, `think`, `steer`, `watch`, `stop`). Nessun framework nuovo.
- **Non è un banco.** Niente `.p0t` nuovi, niente punteggi automatici, niente
  giudice. Il giudizio è di F., sul transcript, con la scala del §3. Un `.p0t`
  si scrive solo *dopo*, per fissare una capacità già mostrata dal vivo (regola
  di live-teaching §7), e non fa parte di questo piano.
- **È addestramento, non solo verifica.** Ogni scenario lascia KB: le leggi
  vere del campo («an unplugged appliance cannot leak current»), gli stati e le
  azioni con i loro effetti, i principi generali delle famiglie (§5), le forme
  di comprensione insegnate per chiudere una lacuna (§2). Si salva ciò che ha
  retto al trasferimento e al contrasto (§4, fase 9); il resto resta nel
  transcript come diagnosi.
- **Non è un catalogo di risposte.** Nessuno scenario del §5 va «insegnato»
  dicendo a parrot0 la mossa da fare in quel caso. Si insegna ciò che gli
  manca per *capire* la situazione, e i principi generali da cui la mossa
  segue; la mossa deve nascere da lì, e poi trasferirsi a uno scenario gemello
  mai nominato.
- **Vale tutto il [MANTRA](../../MANTRA.md)**: solo lingua naturale, conoscenza
  vera, niente entità inventate, niente nomi di predicati nelle frasi. Quando
  la mossa giusta è il motore, la sessione si ferma (live-teaching §3, regola 6)
  e il lavoro di motore diventa un passo separato, annotato qui.

## 0-bis. ⛔ Le premesse: senza di loro il piano è sterile

*F., 27 settembre 2026: «requisiti fondamentali per far crescere queste
abilità sono quelli di riferirsi alla "comprensione universale", alla "IR", al
concetto di "mondo allargato" e a tutti i lavori derivati da
frontier-kb-natural-dialogue.md e da l4-upgrade.md: senza queste premesse e
indirizzi di lavoro il piano è sterile e rischia di costruire male le
soluzioni.»*

L'agente situazionale **non è una facoltà nuova**: è ciò che diventano i
meccanismi già costruiti quando li si porta su una situazione aperta. Ogni
lacuna, ogni lezione e ogni cura di questo piano si colloca in uno di questi
quadri, e si costruisce **dentro** di essi. Una cura che li scavalca (un
lettore dedicato a una scena, una regola per uno scenario, una tabella di
parole con il verso scritto a mano) costruisce male anche quando lo scenario
diventa verde.

| premessa | che cosa garantisce | che cosa impone a questo piano |
|---|---|---|
| **La IR** — [universal-input.md](universal-input.md) | l'input è **uno**: ogni turno diventa nodi (token, sintagmi, quantità, entità) che tutti i lettori consultano, con le categorie in KB | opzioni, candidati, quantità con unità, coordinazioni e anafore collettive («one of them») sono **nodi della IR**, non campi di un parser di scena. G9, G10, G13, G16 si curano nelle categorie della IR |
| **La comprensione universale** — [universal-comprehension.md](universal-comprehension.md) | nessuna frase ben formata merita un muro cieco: la struttura si estrae sempre, e un muro dice **che cosa** manca | «Both can be the right choice: it depends…» e «That sounds nice» su una scena di dati sono **mimica**, peggio di un muro che nomina l'oggetto mancante (G2, G14). Ogni risposta sotto il livello 3 deve almeno dire che cosa non ha capito |
| **Il mondo allargato** — [the-rational-philosopher.md](the-rational-philosopher.md) | ciò che parrot0 sente è un contenuto con un atto, una fonte e un giudizio; ipotesi e credenze sono distinte; parrot0 conversa nello spazio logico dell'interlocutore e prende iniziativa motivata | il racconto di un guasto è una **situazione riportata** dall'interlocutore, non una lezione sul mondo (G1); «is it true that…» è una domanda, mai un impegno (G7); un'ipotesi («if I unplug the kettle…») si ragiona senza diventare un fatto (G11); proporre una mossa a chi è bloccato è **iniziativa motivata** (G2, G12) |
| **frontier-kb-natural-dialogue** — [frontier-kb-natural-dialogue.md](frontier-kb-natural-dialogue.md) | la scala delle astrazioni K0–K11 e lo schema `FRAME → SITUAZIONE → PIANO CAUSALE → PIANO DI RISPOSTA` (§17.3) | il modello del §1 è **K11** (situazione modificabile: stati, affordance, vincoli, leggi causali), non un modello nuovo. Il problema aperto è una **questione aperta** di K3 (con obblighi e mosse). Le ipotesi sono **contesti** di K4 (`context-scope.p0`: credenze concorrenti visibili insieme). Le famiglie di mosse del §5 sono **operatori trasferibili** di K9 («operatori, non altre risposte»). Le letture concorrenti di una scena sono K2. La scala di giudizio del §3 è parente del reticolo dei livelli nominabili (D19). Le «cose da non fare» del §12 valgono qui |
| **L4** — [l4-upgrade.md](l4-upgrade.md) | ciò che si impara entra nella **stessa rete** che la comprensione percorre, riconosciuto per conseguenze, con le condizioni sotto cui vale (C1–C7) | ogni lezione del piano (una legge causale, un polo di scala, un principio di famiglia) deve essere **usata dagli stessi lettori** che la comprensione usa (C1), riconoscersi se già operante (C3), portare le sue condizioni (C4), restare dopo il riavvio (C6). Distinguere capire, credere, usare e confermare (§2.5: G7). La direzione minore/maggiore dal polo di una scala (§9) segue il precedente della **condizione insegnata** (`inverts_when/2`, incrementi 5–7). I costi si leggono con [kb-growth-dynamics.md](../kb-growth-dynamics.md) |

**Le lacune del §2 nei loro quadri** (la colonna «canale» del §2 dice come si
insegna; questa dice **dove** la cura deve vivere):

| quadro | lacune |
|---|---|
| IR (nodi, categorie, coordinazione, quantità) | G3, G9, G10, G13, G16, G17 |
| comprensione universale (muro che nomina, niente mimica) | G2, G14 |
| mondo allargato (atto, impegno, ipotesi, iniziativa) | G1, G7, G8, G11, G12 |
| K2/K3/K4/K9/K11 (letture, questione aperta, contesti, operatori, situazione) | G1, G2, G11, G12, G14 |
| L4 (regole che operano, condizioni, coerenza) | G4, G5, G6, G15 |

**Regola operativa.** Prima di curare una lacuna, il `think` della sessione ne
**nomina il quadro** e il meccanismo esistente su cui la cura si appoggia
(una categoria della IR, una forma di `situation.p0`, un contesto di
`context-scope.p0`, una regola che L4 raggiunge). Se non se ne trova nessuno,
la sessione si ferma e la domanda va al piano del quadro (frontier, L4,
universal-input), non a questo: qui si addestra e si verifica, **non si
inventano architetture parallele**.

## 1. Il modello: la mossa come conseguenza, non come piano con ingresso

Un agente situazionale tiene, anche senza nominarli, cinque oggetti. Sono
quelli di **K11** di [frontier-kb-natural-dialogue.md](frontier-kb-natural-dialogue.md)
(situazione modificabile) e di `situation.p0` / `context-scope.p0`: questo
piano li usa, non li ridefinisce (§0-bis).

| oggetto | domanda che lo riempie | esempio (differenziale che scatta) |
|---|---|---|
| **stato** | com'è il mondo adesso? | il differenziale scatta; tre apparecchi collegati |
| **obiettivo** | che cosa si vuole? | trovare l'apparecchio che disperde |
| **ipotesi** | che cosa potrebbe spiegarlo? | il bollitore, il frigo o la lavatrice |
| **leggi** | che cosa causa che cosa? | un apparecchio che disperde fa scattare il differenziale; uno scollegato non disperde |
| **azioni** | che cosa posso cambiare, e con quale effetto? | scollegare un apparecchio; ricollegarlo |

**L'intuizione, in termini di inferenza:** fra le azioni possibili, quella il
cui esito **previsto differisce fra le ipotesi aperte** porta informazione. Se
scollego il bollitore e il differenziale smette di scattare, l'ipotesi
«bollitore» sopravvive e le altre cadono; se continua, cade il bollitore.
Nessuno ha scritto «quando il differenziale scatta, scollega uno alla volta»:
la mossa si **deriva** confrontando le previsioni delle leggi sotto ciascuna
ipotesi. È la stessa inferenza che, in un altro campo, propone di fare
`git bisect`, di togliere metà delle luci dell'albero, di spegnere metà dei
router. Da qui tre conseguenze per l'addestramento:

1. **Le mosse sono opportunità derivate, non regole con condizioni.** Una mossa
   si propone quando l'inferenza la trova *utile qui*, e parrot0 deve saper
   dire perché («se ho ragione su X, facendo Y vedremo Z»). Un piano con
   ingresso («se l'utente dice "trips", rispondi "unplug"») è esattamente ciò
   che questo piano vieta.
2. **Il principio generale si insegna una volta, in un campo, e deve valere
   negli altri.** «Un'azione che ha esiti diversi secondo l'ipotesi vera ti
   dice quale è vera» è conoscenza; la sua applicazione al differenziale no.
3. **La prova è il trasferimento e il contrasto.** Lo stesso principio deve
   proporre la mossa giusta in uno scenario gemello mai nominato, e **non**
   proporla dove è sbagliata (guasti che non si ripetono, due guasti insieme,
   una prova pericolosa, §5 colonna «contrasto»).

Il repertorio del §5 è organizzato per **famiglie di mosse** (isolare,
tornare indietro, confrontare, ridurre, invertire…): ogni famiglia è un
principio generale, gli scenari sono i suoi campi di prova.

## 2. Le lacune di comprensione, come le ha rivelate il primo giro

Il primo giro (§7) non è arrivato a chiedersi se l'isolamento nasce per
inferenza: si è fermato prima, su lacune di **comprensione**. Sono il primo
lavoro del piano, perché ogni scenario del §5 le attraversa. Ogni lacuna ha un
identificativo, la strada osservata, il meccanismo da cui passa la cura e il
canale d'insegnamento da tentare per primo.

| # | lacuna | strada osservata (27 set) | canale / meccanismo |
|---|---|---|---|
| **G1** | **un resoconto di guasto diventa un fatto sul mondo** | «The RCD in my house keeps tripping, and I can't work out which appliance…» → `Learned: … keep tripping. Learned: can't work out.` | L4-7 (l'impegno segue la comprensione): un racconto in prima persona di un problema è una **situazione aperta**, non una lezione. Parente di R1 di [l4-upgrade.md](l4-upgrade.md) |
| **G2** | **il blocco della persona non si riconosce** | «I have already checked the obvious things and I don't know what to try next.» → «I don't know about obvious things» | lettura dello stato dell'interlocutore ([initiative.md](initiative.md), `trouble_cue`): «non so cosa provare» è uno stato con una mossa sociale (proporre), e riprende il problema del turno prima |
| **G3** | **l'obiettivo si prende dalla subordinata** | «How do I find out which appliance is making the RCD trip?» → «how to make trip»; «…which appliance leaks current?» → «how to make current» | `situation.p0`, il ruolo `goal`: l'obiettivo è il verbo retto da «how do I» (*find out*), non l'ultimo verbo. Confine di sintagma: L2 («end the goal before which»), poi L4 sulla regola del ruolo |
| **G4** | **la causa detta con «when» non si legge** | «An RCD trips when an appliance leaks current to earth.» → muro | forma nuova L2 (`<frase con x, y> means <frase con x, y>`): «x trips when y means if y then x trips» |
| **G5** | **le regole entrano proposizionali, o con il conseguente opaco** | «If an appliance leaks…, then the RCD trips» → `holds(rcd_trips) :- holds(appliance_leaks…)`; con variabili, `holds(x_trips_…) :- leaks($V1,$V2)` | il lettore `if … then` lega le variabili nell'antecedente ma non nel conseguente non copulare. Prima L2 sulle forme; se non basta, è motore (lettore delle regole) |
| **G6** | **le domande sulle proposizioni non consultano le regole** | «Does the RCD trip?», «Why does the RCD trip?» → «I don't know about residual» (con `leaks(washing_machine, current)` e la regola in KB) | lettore polare/why sulle proposizioni `holds/1`; la sigla espansa rompe l'entità. Probabile motore (C_TODO) |
| **G7** | **una domanda diventa un fatto** (grave) | «Is it true that the RCD trips?» → `Learned: holds(residual_current_device_trips).` | L4-7: «is it true that» è una domanda di verità, mai un atto d'impegno. Da trattare **prima di ogni altra** lacuna: un fatto falso pesa più di un muro (mantra #7) |
| **G8** | **un fatto detto non si ritira parlando** | «forget that the RCD trips» → «I don't know about forget» | la forma `forget that <proposizione>` per le proposizioni; parente di «Forget is a layer» (L3 §19) |
| **G9** | **«uno di loro» e l'insieme dei candidati** | «The kettle, the fridge and the washing machine are plugged in, and one of them leaks current.» → `fridge is a plugged, located_in(fridge, and_one_of_them_leaks_current)` | coordinazione e anafora collettiva («one of them»): l'insieme dei candidati e il vincolo «esattamente uno» sono l'oggetto **ipotesi** del §1 |
| **G10** | **uno stato («plugged in», «switched off») letto come classe** | «are plugged in» → `is a plugged` | aggettivo di stato vs nome di classe; `verb particle` («plug in», «switch off») come verbi di azione con effetto |
| **G11** | **l'azione ipotetica non si simula** | «If I unplug the kettle and the RCD stops tripping, what does that tell me?» → «I don't know about unplug» | il ragionamento in avanti sotto un'ipotesi (contesti di `context-scope.p0`, `holds_in/2` di `situation.p0`): *che cosa sarebbe vero se*… |
| **G13** | **la scena con più opzioni si fonde in un solo viaggio** | «The bus leaves at 6:10 pm and takes 40 minutes. The train leaves at 6:25 pm and takes 20 minutes.» → «That sounds nice…» + `The trip takes 0.666667 hours`, `…0.333333 hours`: partenze perse, bus e treno fusi | `event-time.p0` modella UN viaggio in prima persona (`journey_subject(trip)`); `departure_verb(leave)` solo alla radice. Servono opzioni come soggetti distinti, ciascuno con partenza, durata e arrivo (la legge arrivo = partenza + durata c'è già) |
| **G14** | **l'obiettivo detto non entra nella scelta** | «I want to get home as soon as possible. Should I take the bus or the train?» → «Both can be the right choice: it depends on…» (anche con le durate lette) | manca l'oggetto **scelta fra opzioni per un obiettivo**: `decisions.p0` verifica requisiti (soglie, formule), non sceglie la migliore |
| **G15** | **la direzione (minore/maggiore) è una casistica per parola** | «Which is faster…?», «Which has the shortest travel time?» → muro | oggi il verso sta in `comparative_word(younger, age, lt)`, una riga per forma e per lingua, e solo per l'età. Vedi §9: un fatto per polo di scala, il resto derivato |
| **G16** | **il confronto fra quantità con unità non c'è** | «Is 20 less than 40?» → Yes; «Is 20 minutes less than 40 minutes?» → muro, anche dopo «minute measures time» (lezione accettata senza effetto) | il confronto numerico non usa `measured_value/1`; motore o KB del lettore dei confronti |
| **G17** | **il valore di una relazione non è un operando** | «Is the travel time of the train less than the travel time of the bus?» → «I don't know about travel time» (con i due fatti in KB) | la composizione «the R of X» dentro un confronto |
| **G12** | **nessuna mossa informativa** | nessuna proposta, in nessun turno | la facoltà del §1: confrontare le previsioni fra ipotesi. Oggetto nuovo in KB (vedi §4) |

### 2-bis. Colmare durante il piano, e dove vanno gli errori

**Regola (F.).** Durante una sessione, ogni lacuna di comprensione che si può
colmare **insegnando** si colma subito, nel canale più alto che la regge:

1. **L4** se la lacuna è una regola che la comprensione già usa e che la
   lezione deve raggiungere, confermare, estendere o condizionare
   ([l4-upgrade.md](l4-upgrade.md): riconoscimento per conseguenze, analogia,
   condizione insegnata);
2. **L3** se basta il contatto: la parola, la relazione o la cornice nuova si
   allineano a ciò che parrot0 legge nello stesso turno
   ([l3-upgrade.md](l3-upgrade.md));
3. **L2** se serve uno schema dichiarato: una forma nuova, un confine, una
   regola con variabili ([l2-upgrade.md](l2-upgrade.md)), scritto nel `think`
   come ripiego.

La lezione si verifica subito (la frase che mancava, un caso vicino, un
contrasto) e **resta**: è conoscenza per il futuro, non un'impalcatura della
sessione.

**Quando l'insegnamento fallisce** (la lezione non entra, entra storta, scrive
un fatto falso, non si ritira, non trasferisce), l'errore va **in testa a
[train-the-learning-process.md](train-the-learning-process.md)**, nella
sezione PRIORITARIO, con la frase verbatim, la risposta, la diagnosi e lo stato
(🔴/🟡/✅), prefisso **SA** (situational agent). È lì che si curano le
capacità di addestramento; qui resta il rimando alla riga SA. La lacuna del §2
che dipende da quell'errore resta aperta finché l'errore è 🔴.

**Ordine di lavoro:** G7 e G1 prima (sono falsi, non muri), poi G2–G4 e G9–G10
(la situazione deve essere capita), poi G5–G6 e G11 (le leggi devono operare),
poi G12. Una lacuna si chiude quando la sessione mostra la lettura giusta **e**
un trasferimento su un altro scenario del §5 che la attraversa.

## 3. Come si giudica la strada (per F.)

Niente punteggio automatico. F. legge il transcript e colloca la strada di
parrot0 su questa scala; l'insegnante la annota nel `think` di chiusura di ogni
scenario. È la risposta alla domanda di F.: *quando siamo lontani da un agente
intelligente*.

| livello | che cosa mostra la strada | esempio (differenziale) |
|---|---|---|
| 0 | **legge male**: fatti spazzatura, domanda presa per lezione, obiettivo sbagliato | `Learned: can't work out.` |
| 1 | **capisce la scena**: stato, obiettivo, candidati letti come tali | «So one of three appliances leaks, and you want to know which.» |
| 2 | **riconosce il blocco** e resta sul problema invece di cambiare tema | «You've ruled out the obvious. Let's think about what we can observe.» |
| 3 | **tiene le ipotesi**: dice che non può ancora distinguere, e perché | «Any of the three could be leaking: nothing so far separates them.» |
| 4 | **propone una mossa informativa**, come opportunità | «You could unplug one appliance at a time: when the tripping stops, the last one you unplugged is the leaking one.» |
| 5 | **spiega perché informa**, dalle leggi | «An unplugged appliance can't leak, so if the RCD stops tripping, the leak was in that one.» |
| 6 | **si adatta all'esito** detto al turno dopo, e restringe | «It still trips with the kettle unplugged: it's the fridge or the washing machine. Try the washing machine next.» |
| 7 | **migliora la mossa** (dimezzare invece di uno alla volta, quando i candidati sono molti) o ne vede il limite | «With twenty sockets, switch off half the circuits at the board first.» / «If it trips only sometimes, one test per appliance may not be enough.» |
| 8 | **trasferisce**: la stessa inferenza in uno scenario gemello mai nominato, e la **trattiene** nel contrasto | propone di togliere metà delle estensioni del browser; *non* propone di staccare uno alla volta i respiratori di un reparto |

Primo giro (27 set): **livello 0**, con isole di livello 1 dopo le lezioni
(la sigla, i verbi di relazione, la regola).

## 4. Il protocollo di un esperimento

Ogni esperimento è **una sessione live** (o un blocco di una sessione) su uno
scenario del §5. L'insegnante è il coding agent; F. guarda con `watch` e
indirizza con `steer` o in chat. Le fasi, ognuna aperta da un `think`:

1. **Scena.** La situazione come la racconterebbe una persona, in più turni se
   serve (§1: stato, obiettivo, candidati), senza parole chiave di comodo.
2. **Blocco.** La persona dice di non sapere cosa fare, con parole sue. Nessun
   suggerimento della mossa.
3. **Attesa dell'intuizione.** Si guarda che cosa propone parrot0. Si annota il
   livello del §3.
4. **Diagnosi della strada.** Per ogni turno sotto il livello atteso: quale
   **quadro** del §0-bis (IR, comprensione universale, mondo allargato, K2–K11,
   L4) e quale lacuna del §2 (o una nuova, da aggiungere alla tabella con la strada
   osservata), letta con `/debug`, «why did you answer that way?», varianti della
   frase. Il ragionamento nomina il meccanismo (IR, comprensione, L2, L3, L4).
5. **Colmare, in ordine di canale.** Insegnare ciò che manca per **capire**: per
   contatto (L3) con una frase che direbbe una persona; se non basta, schema
   (L2) dichiarato nel `think`; se la lacuna è una regola che la comprensione
   già usa, L4 (riconoscerla, estenderla). **Mai** la mossa dello scenario. I
   principi generali (§5, prima riga di ogni famiglia) si insegnano **una volta
   sola in tutto il piano**, nel primo scenario della famiglia, e la sessione lo
   annota qui (§6).
6. **Ri-porre** la scena e il blocco, e rileggere il livello.
7. **Trasferire** a uno scenario gemello della stessa famiglia, mai nominato
   nelle lezioni.
8. **Contrastare** con lo scenario di contrasto della famiglia: la mossa non
   deve comparire, o deve comparire con il suo limite.
9. **Salvare e chiudere.** Lo scopo è anche far crescere la KB: la
   conoscenza che ha retto (leggi, stati, azioni, principi, forme) si salva.
   `stop` con `/save` solo se la sessione non ha lasciato fatti
   spazzatura non ritirabili (primo giro: sì, quindi chiuso **senza** `/save`,
   live-teaching §8.3); altrimenti la conoscenza buona si ridice in una
   sessione pulita e si salva lì. Il resoconto va nel §6.

**Quando serve il motore:** la sessione si ferma, la lacuna passa allo stato
«motore» nel §2 con la diagnosi, e il lavoro diventa un passo separato
(KB-first, mantra), poi si riapre lo stesso scenario.

**La facoltà che manca (G12), come la si prepara.** Non è un modulo: è
l'operatore di K9 applicato alla situazione di K11 con le ipotesi come
contesti di K4 (§0-bis), cioè un oggetto in KB sopra `situation.p0` e
`context-scope.p0` (stato come credenza
di un contesto, `holds_in/2`) e sopra le leggi insegnate. In forma di
contratto, da scrivere quando G1–G11 lo rendono raggiungibile:
`hypothesis_open(Situazione, H)`, `predicts(H, Azione, Osservazione)` (dalle
leggi, sotto il contesto dell'ipotesi), `informative(Azione)` se esistono due
ipotesi aperte con previsioni diverse per la stessa azione; la proposta è un
`answer_plan` che dice azione, previsioni ed esito atteso. Il suo costo va
misurato con [kb-growth-dynamics.md](../kb-growth-dynamics.md) alla mano (le
ipotesi sono contesti: una vista per ipotesi moltiplica, S7).

## 5. Il repertorio degli scenari

Ogni famiglia ha: il **principio** (la conoscenza generale da cui la mossa
segue, da insegnare una volta), gli **scenari** (campi veri, variati: casa,
cucina, auto, salute, lavoro, codice, viaggi, relazioni, natura), la **lacuna
probabile** oltre a quelle del §2, e il **contrasto** (dove la mossa è
sbagliata o va limitata). Le frasi fra virgolette sono aperture possibili della
scena, non copioni.

### F1 — Isolare per esclusione (una parte alla volta, o a metà)

**Principio.** Se il guasto sta in una parte sola, togliere una parte e vedere
se il guasto resta dice se era lì; togliere metà delle parti dimezza i
sospetti a ogni prova.

| scenario | apertura | lacuna probabile |
|---|---|---|
| F1.1 differenziale che scatta | «The RCD keeps tripping and I can't tell which appliance causes it.» | G1–G12 (primo giro) |
| F1.2 luci dell'albero spente | «Half of my Christmas lights went dark and I can't see which bulb is broken.» | lampadine in serie: un guasto spegne il ramo |
| F1.3 browser lento dopo le estensioni | «My browser became very slow and I have fifteen extensions installed.» | «disable» come azione reversibile |
| F1.4 regressione nel codice | «The tests passed last month and fail now, and there are two hundred commits in between.» | ordine temporale dei commit: dimezzare nel tempo (git bisect) |
| F1.5 rumore in macchina | «There is a rattle in my car and I can't tell where it comes from.» | isolare per posizione e condizione (velocità, buche) |
| F1.6 allergia alimentare | «I get a rash after dinner but I don't know which food causes it.» | dieta di esclusione: togliere un alimento per volta, **sotto consiglio medico** |
| F1.7 rete di casa lenta | «The Wi-Fi is slow in the evening and there are ten devices connected.» | un dispositivo che satura; staccarli a gruppi |
| F1.8 foglio di calcolo con il totale sbagliato | «The yearly total in my spreadsheet is wrong and I can't find the bad cell.» | subtotali per mese: dove il subtotale diverge |
| F1.9 perdita d'acqua in bagno | «There is water on the bathroom floor every morning.» | asciugare e osservare da dove riappare |
| F1.10 odore in cucina | «The kitchen smells bad and I have cleaned everything I can see.» | togliere/controllare contenitori uno per volta |

**Contrasto.** Un guasto **intermittente** (una prova per parte non basta:
servono prove ripetute), **due guasti insieme** (togliere uno non basta),
**parti che non si possono togliere senza pericolo** (un apparecchio medico,
l'impianto frenante), **parti dipendenti** (spegnere il router spegne tutti).
La mossa deve comparire con il suo limite o non comparire.

### F2 — Tornare all'ultimo stato che funzionava

**Principio.** Se prima funzionava e ora no, qualcosa è cambiato in mezzo:
annullare i cambiamenti riporta al funzionamento, e annullarli uno alla volta
dice quale era.

| scenario | apertura | lacuna probabile |
|---|---|---|
| F2.1 stampante dopo un aggiornamento | «My printer stopped working after the update last night.» | «after» come relazione temporale causale candidata |
| F2.2 ricetta che non viene più | «My bread always rose, but this week it stayed flat.» | che cosa è cambiato: lievito nuovo, farina, temperatura |
| F2.3 pianta che appassisce | «My basil was fine until I moved it to the balcony.» | spostamento come cambiamento con effetti (luce, vento) |
| F2.4 mal di testa nuovo | «I've had headaches since I started my new job.» | cambiamenti concomitanti (schermo, sonno, caffè) |
| F2.5 codice dopo una modifica | «The program crashed right after I changed the config file.» | ripristinare la copia precedente |
| F2.6 auto che consuma di più | «My car uses more fuel since I changed the tyres.» | pressione, misura degli pneumatici |

**Contrasto.** Il guasto **non** è legato a un cambiamento (usura, età:
tornare indietro non aiuta); tornare indietro è **irreversibile o costoso**
(una ristrutturazione); il cambiamento era una **correzione di sicurezza** da
non annullare.

### F3 — Confrontare con un gemello che funziona

**Principio.** Se due cose uguali si comportano diversamente, la causa sta in
ciò che le distingue.

| scenario | apertura | lacuna probabile |
|---|---|---|
| F3.1 un solo termosifone freddo | «All radiators are warm except the one in the bedroom.» | aria nel radiatore, valvola: che cosa ha di diverso |
| F3.2 un computer lento su due uguali | «Two identical laptops, and only mine is slow.» | differenze di configurazione |
| F3.3 una torta riuscita e una no | «I baked the same cake twice and only the first was good.» | che cosa è cambiato fra le due volte |
| F3.4 una pianta su due | «I have two tomato plants and only one has yellow leaves.» | posizione, acqua, vaso |
| F3.5 un collega che riceve le email e uno no | «My colleague gets the newsletter and I don't.» | filtri, indirizzi, iscrizione |

**Contrasto.** I due non sono davvero uguali in ciò che conta (modelli
diversi); la differenza è **casuale** (una sola torta andata male può essere
sfortuna); il gemello «sano» ha lo stesso problema nascosto.

### F4 — Controllare la catena delle precondizioni, dalla più semplice

**Principio.** Un effetto richiede tutte le sue condizioni; quando manca,
conviene controllare prima quelle più probabili e più economiche da verificare.

| scenario | apertura | lacuna probabile |
|---|---|---|
| F4.1 televisore che non si accende | «The TV won't turn on.» | spina, presa, telecomando, batterie: ordine per costo |
| F4.2 auto che non parte | «My car won't start this morning.» | batteria (luci deboli?), carburante, motorino: la domanda che separa |
| F4.3 lievito che non fa effetto | «The dough didn't rise at all.» | lievito scaduto, acqua troppo calda, sale sul lievito |
| F4.4 email che non parte | «My emails stay in the outbox.» | connessione, allegato troppo grande, password |
| F4.5 bambino che non dorme | «My toddler won't fall asleep tonight.» | fame, caldo, pannolino, paura: le cause comuni prima |
| F4.6 lavatrice che non scarica | «The washing machine won't drain.» | filtro, tubo piegato, pompa |

**Contrasto.** Il segnale di **pericolo** salta la catena (odore di gas, fumo,
un sintomo grave: prima la sicurezza o il soccorso, §F10); la causa rara è
**nota** (la spia già dice che cos'è).

### F5 — Cambiare una sola variabile alla volta

**Principio.** Se cambio due cose insieme e il risultato cambia, non so quale
delle due è stata; per imparare da una prova, ne cambio una sola.

| scenario | apertura | lacuna probabile |
|---|---|---|
| F5.1 caffè amaro | «My espresso tastes bitter and I changed the beans and the grind.» | due variabili cambiate insieme |
| F5.2 allenamento e dolore | «My knee hurts since I started running more and with new shoes.» | confondimento |
| F5.3 annuncio che non vende | «I changed the price and the photos and still no buyers.» | |
| F5.4 dormire meglio | «I want to sleep better; should I stop coffee, screens and late dinners all at once?» | trade-off: tutte insieme migliora prima ma non insegna |

**Contrasto.** Quando **conta il risultato e non il sapere** (una festa
stasera: cambia tutto ciò che aiuta), o quando le variabili **interagiscono**
(una senza l'altra non ha effetto).

### F6 — Riprodurre prima di aggiustare

**Principio.** Un guasto che non so far comparire non so se l'ho aggiustato:
prima trovare quando compare.

| scenario | apertura | lacuna probabile |
|---|---|---|
| F6.1 rumore del frigorifero | «The fridge makes a strange noise, but never when the technician is here.» | registrare, annotare le ore |
| F6.2 bug che capita a volte | «The app crashes sometimes, I can't say when.» | le condizioni del crash |
| F6.3 sintomo intermittente | «I sometimes feel dizzy.» | diario dei sintomi, **medico** |

**Contrasto.** Riprodurre è **pericoloso** (un corto circuito, un malore): non si
provoca, si osserva o si chiama chi può.

### F7 — Ridurre il problema a un caso più piccolo

**Principio.** Un problema grande che fallisce si capisce su una sua versione
piccola che fallisce nello stesso modo.

| scenario | apertura | lacuna probabile |
|---|---|---|
| F7.1 documento che non si stampa | «A 200-page document won't print.» | stampare una pagina, poi metà |
| F7.2 ricetta per 40 persone | «I need to cook for 40 and I've never made this dish.» | provarla prima per 4 |
| F7.3 query lenta | «This database query takes minutes.» | le sue parti una per volta |
| F7.4 trasloco | «I have to move house next week and I don't know where to start.» | una stanza, una scatola |

**Contrasto.** Il problema **nasce dalla scala** (per 40 persone il forno non
basta: la versione piccola non lo mostra).

### F8 — Lavorare all'indietro dall'obiettivo

**Principio.** Se so dove devo arrivare e quando, posso ricavare ogni passo
precedente e il momento in cui iniziare.

| scenario | apertura | lacuna probabile |
|---|---|---|
| F8.1 cena alle otto | «Dinner must be on the table at eight and the roast takes two hours.» | durate, sottrazione nel tempo (event-time.p0) |
| F8.2 treno delle sette | «My train leaves at seven and the station is forty minutes away.» | margine, «on time» (già in KB: `on_time_link`) |
| F8.3 esame fra un mese | «My exam is in four weeks and there are twelve chapters.» | distribuire il lavoro |
| F8.4 invito a un matrimonio | «The wedding is in June and I need a suit, travel and a gift.» | dipendenze fra compiti |

**Contrasto.** L'obiettivo **non è fisso** (una data spostabile: la mossa
giusta è chiedere se lo è), o un passo **non dipende dal tempo** (un
documento che arriva quando arriva).

### F9 — Trovare il collo di bottiglia

**Principio.** In una catena, il risultato va alla velocità del passo più
lento: migliorare gli altri non serve.

| scenario | apertura | lacuna probabile |
|---|---|---|
| F9.1 cucina del ristorante | «Orders are late although we hired another cook.» | il passo lento è altrove (il forno, la cassa) |
| F9.2 imbottigliamento in casa | «Making jam takes all day, even with two people chopping.» | la pentola è una sola |
| F9.3 fila alla posta | «There are three counters open but the queue doesn't move.» | un solo sportello fa quell'operazione |
| F9.4 computer lento | «I added memory but my computer is still slow.» | il disco, la rete |

**Contrasto.** Più passi **si alternano** come collo di bottiglia (ogni
miglioramento ne sposta un altro); il vincolo è una **regola**, non una risorsa.

### F10 — Prima la sicurezza, poi la diagnosi

**Principio.** Quando c'è un rischio per le persone, la prima mossa è
togliere il rischio o chiamare aiuto; capire viene dopo.

| scenario | apertura | lacuna probabile |
|---|---|---|
| F10.1 odore di gas | «I smell gas in the kitchen.» | non accendere niente, aprire, uscire, chiamare |
| F10.2 filo che fa scintille | «The plug sparked when I pulled it.» | staccare il generale prima di toccare |
| F10.3 persona che non risponde | «My neighbour collapsed and isn't responding.» | chiamare il soccorso, poi le istruzioni |
| F10.4 pentola con olio in fiamme | «The oil in the pan caught fire.» | mai acqua, coperchio |

**Contrasto.** È il contrasto **di tutte le altre famiglie**: in presenza di
un rischio, isolare, riprodurre o sperimentare sono mosse sbagliate. Parrot0
deve riconoscere il rischio anche quando lo scenario parte come F1 o F4.

### F11 — Ripercorrere i propri passi

**Principio.** Una cosa smarrita è quasi sempre lungo il percorso fatto dopo
l'ultima volta che la si aveva; ripercorrerlo all'indietro restringe la ricerca.

| scenario | apertura | lacuna probabile |
|---|---|---|
| F11.1 chiavi perse | «I can't find my keys and I had them when I got home.» | «l'ultima volta che» come ancora temporale |
| F11.2 file scomparso | «I saved the report yesterday and now I can't find it.» | percorso delle azioni, cartella recente |
| F11.3 portafoglio in viaggio | «I lost my wallet somewhere between the hotel and the museum.» | tappe, chi chiamare |

**Contrasto.** L'oggetto può essere stato **preso da altri** (furto: la mossa è
denunciare/bloccare prima di cercare); il percorso non è ricostruibile.

### F12 — Misurare invece di indovinare

**Principio.** Quando due spiegazioni competono, una misura che le distingue
vale più di un ragionamento in più.

| scenario | apertura | lacuna probabile |
|---|---|---|
| F12.1 bolletta alta | «My electricity bill doubled.» | leggere il contatore con tutto spento: dispersione o consumo |
| F12.2 febbre del bambino | «My son feels hot.» | il termometro, non la mano |
| F12.3 perdita del radiatore dell'auto | «The coolant level keeps dropping.» | segnare il livello, controllare sotto l'auto la mattina |
| F12.4 pane crudo dentro | «My bread is raw in the middle.» | temperatura reale del forno (termometro da forno) |

**Contrasto.** La misura **costa più** del problema, o **non distingue** le
ipotesi rimaste (misurare qualcosa che le due prevedono uguale).

### F13 — Chiedere a chi sa, o cambiare prospettiva

**Principio.** Quando la conoscenza che manca ce l'ha qualcuno, la mossa
migliore è chiedere a lui; quando si è bloccati, riformulare il problema dal
punto di vista di un altro.

| scenario | apertura | lacuna probabile |
|---|---|---|
| F13.1 sintomi persistenti | «I've had a cough for three weeks.» | quando la mossa è il medico |
| F13.2 codice di un collega | «I can't understand why this function exists.» | chiedere all'autore, la storia del file |
| F13.3 conflitto con un vicino | «My neighbour is angry about the noise and I don't know why, I'm quiet.» | il suo punto di vista: orari, pareti sottili |
| F13.4 cliente insoddisfatto | «The client rejected the design twice.» | chiedere che cosa vuole ottenere, non che cosa non gli piace |

**Contrasto.** La persona che sa **non è raggiungibile** o non è affidabile;
chiedere è la mossa di **tutte** le situazioni (un agente che rimanda sempre al
medico non è intelligente, è evasivo: mantra «misclaims worse than walls»).

### F14 — Invertire la domanda

**Principio.** Se «come ottengo X?» non trova strade, «che cosa impedisce X?» o
«come otterrei il contrario?» spesso le trova.

| scenario | apertura | lacuna probabile |
|---|---|---|
| F14.1 squadra poco produttiva | «How do I make my team more productive?» | che cosa le fa perdere tempo |
| F14.2 casa sempre in disordine | «How do I keep my flat tidy?» | come si produce il disordine (dove si accumula) |
| F14.3 risparmio | «I never manage to save money.» | dove vanno i soldi |

**Contrasto.** L'inverso **non è informativo** (togliere gli ostacoli non basta
se manca la risorsa).

### F15 — Scegliere per rischio e costo, non solo per probabilità

**Principio.** Fra due controlli, conviene prima quello che costa poco o che
esclude un rischio grave, anche se è meno probabile.

| scenario | apertura | lacuna probabile |
|---|---|---|
| F15.1 dolore al petto | «I have a pain in my chest, probably a muscle.» | escludere prima il grave (**soccorso**) |
| F15.2 freni che fischiano | «The brakes squeal, probably just dust.» | il costo di sbagliarsi |
| F15.3 backup | «Should I test the backup or finish the feature first?» | il danno se il backup non funziona |

**Contrasto.** Il rischio grave è **escluso da un fatto noto** (appena
controllato); fissarsi sul caso raro paralizza.

### F16 — Situazioni fra persone

**Principio.** Fra persone, la situazione comprende ciò che l'altro sa,
vuole e sente; la mossa utile spesso è rendere esplicito ciò che è implicito.

| scenario | apertura | lacuna probabile |
|---|---|---|
| F16.1 due coinquilini e le pulizie | «My flatmate and I keep arguing about cleaning.» | un accordo esplicito (turni) invece di chi ha ragione |
| F16.2 amico offeso | «My friend stopped answering my messages after the party.» | che cosa è successo alla festa, chiedere direttamente |
| F16.3 dividere una torta fra due bambini | «Two kids always fight over who got the bigger slice.» | «uno taglia, l'altro sceglie» come conseguenza dell'equità |
| F16.4 riunione senza decisioni | «Our meetings never end with a decision.» | chi decide, e quando |

**Contrasto.** Rendere esplicito **ferisce** (una situazione che chiede tatto);
la persona **non vuole** una soluzione ma essere ascoltata (initiative.md:
registro sociale).

### F17 — Pianificare con risorse limitate

**Principio.** Quando le risorse non bastano per tutto, si ordina per valore e
si rinuncia a qualcosa esplicitamente.

| scenario | apertura | lacuna probabile |
|---|---|---|
| F17.1 un solo forno per quattro piatti | «I have one oven and four dishes that all need baking.» | temperature comuni, ordine, piatti che aspettano |
| F17.2 valigia troppo piena | «My suitcase is over the weight limit.» | che cosa pesa e serve meno |
| F17.3 tre scadenze nella stessa settimana | «Three deadlines fall in the same week.» | spostarne una chiedendo prima |
| F17.4 budget di un viaggio | «The trip costs more than we can spend.» | le voci grandi prima |

**Contrasto.** La risorsa **si può aumentare** a basso costo (prendere un
secondo forno in prestito): rinunciare è la mossa sbagliata.

### F18 — Imprevisto in viaggio: il piano B dalla situazione

**Principio.** Quando un passo del piano salta, si riparte dall'obiettivo e
dallo stato attuale, non dal piano vecchio.

| scenario | apertura | lacuna probabile |
|---|---|---|
| F18.1 treno cancellato | «My train to Milan was cancelled and I have a meeting at two.» | alternative (autobus, auto), avvisare |
| F18.2 volo perso per la coincidenza | «I missed my connecting flight.» | a chi rivolgersi, diritti |
| F18.3 strada chiusa | «The road to the village is closed for a landslide.» | percorso alternativo, tempi |

**Contrasto.** L'obiettivo **si può spostare** più facilmente del percorso
(rimandare la riunione online).

### F19 — Leggere i segnali deboli nel tempo

**Principio.** Un cambiamento lento si vede solo confrontando nel tempo:
annotare e confrontare mostra ciò che il singolo giorno nasconde.

| scenario | apertura | lacuna probabile |
|---|---|---|
| F19.1 peso della pianta d'appartamento | «My plant is slowly losing leaves.» | diario delle innaffiature |
| F19.2 batteria del telefono | «My phone battery lasts less and less.» | ore, app, età della batteria |
| F19.3 spese di casa | «Every month we end up short.» | registro delle spese |

**Contrasto.** Il cambiamento è **improvviso** (F2 invece di F19).

### F20 — Riconoscere di non poter risolvere, e dirlo bene

**Principio.** Un agente intelligente riconosce quando la situazione è fuori
dalla sua portata o richiede un professionista, e lo dice con la mossa utile
(chi chiamare, che cosa dire), non con un muro.

| scenario | apertura | lacuna probabile |
|---|---|---|
| F20.1 crepa nel muro portante | «A crack appeared in the wall and it's getting longer.» | strutturista, misurare la crepa nel frattempo |
| F20.2 questione legale | «My landlord wants to keep the whole deposit.» | a chi rivolgersi, che documenti tenere |
| F20.3 sintomo neurologico | «Half of my face feels numb.» | **emergenza**: F10 prevale |

**Contrasto.** La situazione **è** alla sua portata e rimandare è evasione
(F13 contrasto).

### F21 — Scegliere per un obiettivo: la direzione viene dalle condizioni

**Principio.** La migliore fra le opzioni, per un obiettivo, è quella che
nessun'altra batte sulla grandezza che l'obiettivo nomina; **da che parte**
batte (più o meno) lo dice il significato della parola dell'obiettivo, cioè
quale polo della scala nomina («soon», «early», «fast»: poco tempo; «cheap»:
poco prezzo; «spacious»: tanto spazio). Nessuna riga «sooner → minimo»: un fatto
per polo, e comparativo, superlativo, «as … as possible» e scelta ne derivano
(§9).

| scenario | apertura | lacuna probabile |
|---|---|---|
| F21.1 bus o treno per tornare prima | «The bus leaves at 6:10 pm and takes 40 minutes; the train leaves at 6:25 pm and takes 20. I want to get home as soon as possible.» | G13–G17 (giro del 27 set, §7-bis): l'arrivo si **calcola** (partenza + durata), e prima non è «il più veloce» |
| F21.2 il volo più economico | «Three flights: 120, 95 and 140 euros. I want to spend as little as possible.» | polo basso del prezzo |
| F21.3 l'appartamento più grande entro il budget | «Two flats in my budget: 60 and 75 square metres.» | polo alto, con un vincolo (requisito di `decisions.p0`) |
| F21.4 la strada più sicura, non la più corta | «The motorway is shorter but it's snowing; the valley road is longer.» | due grandezze in conflitto: l'obiettivo sceglie quale conta |
| F21.5 il supermercato più vicino che è aperto | «The nearest shop closes at 8, it's 7:50 and it's 15 minutes away.» | un'opzione esclusa da un vincolo prima del confronto |
| F21.6 arrivare **puntuale**, non **presto** | «The meeting is at 9 and I don't want to wait outside.» | il polo non è un estremo: il migliore è il più vicino a un valore |

**Contrasto.** L'obiettivo nomina un **valore giusto** e non un estremo
(F21.6: il più presto non è il migliore); l'opzione migliore sulla grandezza
**viola un vincolo** (il treno più veloce è pieno); due grandezze e
l'obiettivo non dice quale pesa di più (la mossa è chiedere).

### Scenari composti (multi-famiglia, per i livelli 6–8)

| scenario | famiglie | perché è difficile |
|---|---|---|
| C1 il differenziale che scatta **solo la sera** | F1 + F6 + F19 | intermittenza: l'esclusione vuole prove ripetute nel momento giusto |
| C2 il pane piatto **dopo il trasloco** | F2 + F3 + F12 | cambiamenti multipli: altitudine, acqua, forno nuovo |
| C3 il sito lento **dopo il deploy, solo per alcuni utenti** | F2 + F3 + F1 | gemelli (utenti sani), cambiamento, esclusione per regione |
| C4 odore di bruciato e scatto del differenziale | F10 + F1 | la sicurezza deve prevalere sull'esclusione |
| C5 treno cancellato con un bambino piccolo e poco budget | F18 + F17 + F16 | vincoli sociali e di risorse sul piano B |
| C6 esame fra una settimana e mal di testa da giorni | F8 + F2 + F13 | pianificare e riconoscere quando chiedere al medico |

Il repertorio si **espande a ogni sessione**: uno scenario nuovo nasce da ciò che
F. propone o da ciò che una sessione rivela; si aggiunge alla sua famiglia con
apertura, lacuna probabile e, se serve, un contrasto nuovo.

## 6. Registro delle sessioni

Una riga per sessione, compilata alla chiusura: data, scenari, livello
raggiunto (§3) prima e dopo, lacune aperte e chiuse (§2), principi insegnati
(§5), conoscenza salvata, transcript.

| data | scenari | livello | lacune | principi insegnati | salvato | transcript |
|---|---|---|---|---|---|---|
| 27 set 2026, 18:37 | sessione A: solo conoscenza generale e vera (la legge del differenziale, l'azione, le relazioni, la somma, fast/soon/cheap/quiet) | — | — | quelli di F1 e F21 | **sì**, commit `c8216831` | [2026-09-27-1837.log](../sessions/live/2026-09-27-1837.log) |
| 27 set 2026, 19:13 | sessione A2: le costruzioni «leaves at» e «takes … minutes» | — | — | due costruzioni L2 | **sì**, commit `f00fa617` | [2026-09-27-1913.log](../sessions/live/2026-09-27-1913.log) |
| 27 set 2026, 19:15 | sessione C, processo nuovo, KB salvata, **frasi naturali** («The bus leaves at 6:10 pm.»): F21.1 e F1.1 fino in fondo | F21.1 risolto; F1.1 **6** (mossa, esito, restringimento, colpevole); incoerenza della conclusione curata (iter. 14) | residui: «Why?» ellittico; coordinazione dei predicati | — | no | [2026-09-27-sessione-C-dimostrazione.log](../sessions/live/2026-09-27-sessione-C-dimostrazione.log) |
| 27 set 2026, 18:40 | sessione B, processo nuovo con la KB salvata, **nessuna lezione**: F1.1, F21.1, contrasto tram/taxi | F1.1 **4–5** al primo turno di blocco; F21.1 risolto; il contrasto separa «prima» (il tram, arrivo 6:40) da «più veloce» (il taxi, 15 minuti) | nuova: il «why» su un effetto osservato con causa ignota | — | no (dati di situazione) | [2026-09-27-sessione-B-dimostrazione.log](../sessions/live/2026-09-27-sessione-B-dimostrazione.log) |
| 27 set 2026 | F21.1 (bus o treno, scelta) — SA8–SA10 | 0 → 1 sui dati, 0 sulla scelta | rivelate G13–G17 | nessuno (fermo sui mattoni del §9) | **no** (i dati sono della situazione dell'utente, non del mondo; «bus get me home» spazzatura) | [2026-09-27-situazionale-scelta.log](../sessions/live/2026-09-27-situazionale-scelta.log), chiusa senza `/save` |
| 27 set 2026 | F1.1 (differenziale) — errori di addestramento in train-the-learning-process.md, SA1–SA7 | 0 → 0 (isole di 1) | rivelate G1–G12; chiuse nessuna. Entrate: la sigla RCD, `leaks`/`trips` come verbi di relazione, «if x leaks y then the RCD trips», il fatto della lavatrice | nessuno (il giro si è fermato alla comprensione) | **no** (fatti spazzatura non ritirabili: «can't work out», «fridge is a plugged», una domanda scritta come fatto) | [2026-09-27-situazionale-differenziale.log](../sessions/live/2026-09-27-situazionale-differenziale.log), chiusa senza `/save` |

## 7. Il primo giro, strada per strada (27 settembre 2026)

Campo: impianto di casa. Nessun suggerimento della mossa. Le risposte sono
verbatim.

| # | l'insegnante dice | parrot0 risponde | strada | lacuna |
|---|---|---|---|---|
| 1 | The residual current device in my house keeps tripping, and I can't work out which appliance is causing it. | Learned: residual current device in my house keep tripping. Learned: can't work out. | racconto → due fatti | G1 |
| 2 | I have already checked the obvious things and I don't know what to try next. | Hmm, I don't know about obvious things yet. Want me to learn about it? | blocco → nome ignoto | G2 |
| 3 | How do I find out which appliance is making the residual current device trip? | I understood you're asking how to make trip, but I don't have the steps for it yet. | obiettivo dalla subordinata | G3 |
| 4 | Why does a residual current device trip? | Hmm, I don't know about residual yet. | nome di più parole non unito | G6 |
| 5 | What is a residual current device? | (la definizione enciclopedica, corretta) | la prosa c'è, la causa no | — |
| 6 | A residual current device trips when an appliance leaks current to earth. | I looked up «residual» but found nothing… | «when» causale non letto | G4 |
| 7 | If an appliance leaks current to earth, then the RCD trips. | Learned rule: holds(rcd_trips) :- holds(appliance_leaks_current_to_earth). | regola proposizionale | G5 |
| 8 | If I unplug the kettle and the RCD stops tripping, what does that tell me? | Hmm, I don't know about unplug yet. | nessuna simulazione | G11 |
| 9 | The kettle, the fridge and the washing machine are plugged in, and one of them leaks current. | Learned: fridge is a plugged, located_in(fridge, and_one_of_them_leaks_current)… | candidati e stato letti male | G9, G10 |
| 10 | *(F.: insegnagli ciò che non sa, con L2/L3/L4)* | | | |
| 11 | RCD is short for residual current device. | Held: «rcd» is short for «residual current device». | ✅ la sigla | — |
| 12 | leaks is a relation verb / trips is a relation verb | Learned … | ✅ | — |
| 13 | if x leaks y then x trips the RCD | Learned rule: holds(x_trips_residual_current_device) :- leaks($V1, $V2). | variabili solo nell'antecedente | G5 |
| 14 | forget if x leaks y then x trips the RCD / if x leaks y then the RCD trips | Understood… / Learned rule: holds(residual_current_device_trips) :- leaks($V1, $V2). | ✅ la regola giusta | — |
| 15 | The washing machine leaks current. / Does the RCD trip? | Learned… / Hmm, I don't know about residual yet. | la domanda non consulta la regola | G6 |
| 16 | Is it true that the RCD trips? | Learned: holds(residual_current_device_trips). | **una domanda scritta come fatto** | G7 |
| 17 | forget that the RCD trips | Hmm, I don't know about forget yet. | il ritiro non esiste | G8 |
| 18 | How can I find out which appliance leaks current? | I understood you're asking how to make current… | G3 di nuovo | G3, G12 |

**Distanza dall'agente intelligente (giudizio dell'insegnante, da confermare da
F.):** la domanda «l'isolamento nasce per inferenza?» non è ancora
ponibile. Mancano prima la lettura di un problema aperto (G1, G2, G9), la
lettura delle leggi con le loro variabili (G4, G5) e l'uso delle leggi nelle
domande (G6, G11). Due difetti sono più gravi di un muro e vanno prima di
tutto: una domanda che diventa un fatto (G7) e un fatto che non si ritira
parlando (G8).

## 7-bis. Il giro di scelta (27 settembre 2026, sessione pulita)

Scopo di F.: *«scegliere quello che ti fa andare a casa prima sembrerebbe
operazionale, ma secondo me derivare l'azione da compiere, cioè minore o
maggiore, viene dalle condizioni, non da casistiche. Facciamo la prova: ci
serve per capire se effettivamente questo piano potrà scalare o ci
illudiamo.»* Scenario F21.1. Risposte verbatim.

| # | l'insegnante dice | parrot0 risponde | lacuna |
|---|---|---|---|
| 1 | I'm at the office. The bus leaves at 6:10 pm and takes 40 minutes. The train leaves at 6:25 pm and takes 20 minutes. | That sounds nice -- tell me more about it. Noted: The trip takes 0.666667 hours. Noted: The trip takes 0.333333 hours. | G13 |
| 2 | I want to get home as soon as possible. Should I take the bus or the train? | Both can be the right choice: it depends on what you need it for, how much time or money you want to put in, and what you already know. … | G14 |
| 3 | When does the train leave? / How long does the bus take? | I don't understand that yet. / I'm not sure I followed. … | G13 |
| 4 | The bus gets me home at 6:50 pm and the train gets me home at 6:45 pm. | Learned: bus get me home. | G13 (SA8) |
| 5 | I want to get home as early as possible. Which one should I take? | On get home as early as possible: Let us start with one observable result. … I couldn't read «Which one should I take?». | G14 |
| 6 | travel time is a relation / The travel time of the bus is 40 minutes. / … of the train is 20 minutes. / What is the travel time of the train? | Learned … / 20 minutes. | ✅ la lettura |
| 7 | I want to get home as soon as possible. Should I take the bus or the train? | Both can be the right choice: … | G14 |
| 8 | Which is faster, the bus or the train? / Which has the shortest travel time? | Hmm, I don't know about faster yet. / … about shortest travel time yet. | G15 |
| 9 | Is 20 less than 40? / minute measures time / Is 20 minutes less than 40 minutes? | Yes. / Learned. / Hmm, I don't know about minutes yet. | G16 (SA9) |
| 10 | Is the travel time of the train less than the travel time of the bus? | Hmm, I don't know about travel time yet. | G17 |

Livello (§3): **0 → 1** sui dati (letti e interrogabili dopo la forma giusta),
**0** sulla scelta.

## 9. Scala o ci illudiamo? (la risposta del giro di scelta)

**Oggi, parlando, la domanda non si può ancora provare**: prima della
direzione mancano tre mattoni (G16 il confronto fra quantità con unità, G17 il
valore di una relazione come operando, G14 l'oggetto scelta). Ma il giro ha
mostrato **dove sta il rischio d'illusione**, ed è già nella KB:
`comparative_word(younger, age, lt)`, `comparative_word(older, age, gt)`… una
riga per parola, per forma e per lingua, con il verso scritto a mano. Se il
piano crescesse così («sooner → minimo», «cheaper → minimo», «bigger →
massimo», «as soon as possible → minimo»), ogni scenario vorrebbe le sue righe
e l'intelligenza sarebbe un elenco: **non scalerebbe**.

**Scala se la conoscenza sta nei poli delle scale, non nelle parole.** Un
fatto per concetto, insegnabile parlando in una frase che direbbe una persona
(«Fast means a short travel time.», «Cheap means a low price.», «Early means a
small time.»), dove *short/long, low/high, small/big* sono il piccolo insieme
chiuso delle parole di grandezza, con il loro verso (è l'unico elenco, ed è
lessico generale, non di dominio). Da quel fatto si **derivano**, con la
morfologia già in KB (`verb_form/3` fa lo stesso per i verbi):

- il comparativo («faster», «sooner», «earlier», «cheaper»): quale delle due ha
  la grandezza più vicina al polo;
- il superlativo («the fastest», «the shortest»): quella che nessuna batte;
- «as … as possible»: l'obiettivo di andare verso il polo;
- la **scelta**: fra le opzioni che rispettano i vincoli (`decisions.p0`), la
  migliore verso il polo che l'obiettivo nomina; e, se l'obiettivo nomina un
  valore giusto e non un polo («puntuale»), la più vicina al valore (F21.6).

La prova che scala è quella del §1: **una lezione nuova, un aggettivo mai
visto** (per esempio «Quiet means a low noise level.»), e la scelta fra due
opzioni rumorose deve riuscire senza altre righe. Se per farla riuscire serve
una riga in più di quel fatto, ci stiamo illudendo, e il registro lo deve dire.

**Fatto il 27 settembre (iterazione 4, §10): la prova di scalabilità è
passata.** Con i livelli di rumore di tre elettrodomestici e **un aggettivo mai
visto**, prima della lezione «Which is quieter, …?» non ha risposta; dopo la
sola lezione «Quiet means a low noise level.» reggono insieme «Which is
quieter, the fridge or the dishwasher?» → «The fridge: its noise level is 40
decibels, against 45 decibels for the dishwasher.», «Is the dishwasher quieter
than the fan?» → «No, the fan: …», «Which is the quietest?» → la ventola,
«Which has the highest noise level?» → la lavastoviglie. Nessuna riga oltre
quel fatto. Resta aperta la **scelta per un obiettivo** detto in un altro turno
(«I want to get home as soon as possible. Should I take the bus or the
train?», G14) e l'arrivo **calcolato** (partenza + durata, G13).

**I tre mattoni, in ordine** (storico: G16 e G17 chiusi nell'iterazione 4), sono lavoro di motore/KB fuori dalla sessione
(live-teaching §3, regola 6): G16 → G17 → la rappresentazione dei poli con la
derivazione dei comparativi (sostituendo `comparative_word/3` con i poli, per
l'età compresa) → G14. Poi si riapre F21.1 e si prova con un aggettivo nuovo.

## 10. Registro delle iterazioni — sessione di 5 ore (27 settembre 2026, 15:30–20:30)

Obiettivo di F. (`/goal`): una KB che dopo la sessione risolva problemi di
logica situazionale come quelli del §5; le lacune si lavorano quando
emergono come difetti di una prova di soluzione, generalizzate dove si può;
ogni incremento valido si committa e si pusha subito.

| iter. | difetto (prova) | generalizzazione | cura | quadro (§0-bis) |
|---|---|---|---|---|
| 1 | G7/SA1: «Is it true that the kettle trips?» dopo una regola che nomina la proposizione → `Learned: holds(kettle_trips)` | qualunque domanda su una proposizione vista in una regola veniva asserita, non solo «is it true that» | il ramo proposizionale di `knowledge` legge `turn_declared_act(current_turn, question)` e risponde; A/B su `higher_order_lesson`, `taught_rules`, `scenario_inference`, `conditional_plan`: rossi identici a HEAD | mondo allargato (atto vs impegno), L4 §2.5 |
| 2 | G6: «Does the RCD trip?» → «I don't know about residual» con `holds(residual_current_device_trips) :- leaks($V1,$V2)` e `leaks(washing_machine, current)` in KB | il ramo polare proposizionale reggeva solo l'ausiliare davanti a un soggetto di una parola (+ articolo): niente do-support, niente soggetti di più parole (una sigla espansa ne ha tre) | si prova ogni fine del soggetto; con il portatore del tempo il verbo riprende la forma finita dalla composizione dei paradigmi (`polar_do_finite/3`, grammar.p0, accanto a `polar_fronted`). «Does the RCD trip?» → «Not necessarily.» / «Yes.» dopo il fatto; «Is the ground wet?» regge. A/B come sopra: identico; `conditional_plan` riga 92 («Scartato: located_in(se_milan, …)», piano condizionale italiano) è un rosso di contenuto preesistente, visibile solo quando la riga 91 sta sotto 1 s | IR (soggetto come sintagma), L4 (la stessa composizione dei paradigmi del ramo `supported`) |
| 3 | «Why does the RCD trip?» → «Yes.»; «Why is the ground wet?» → «Yes.»; «How do you know?» → «I haven't answered a knowledge-based question yet» | (a) un «why» senza prova restituiva la risposta interna, cioè la risposta a **un'altra domanda**: valeva per ogni «why» (anche «Why is the sky blue?» → «Yes.», che nascondeva la spiegazione vera già in KB); (b) il ramo proposizionale non lasciava la sua prova | (a) una risposta polare nuda (`bare_polar_reply/1`, KB) non è una ragione: la via del «why» si ritira; (b) la ragione la compone la KB dalla **stessa derivazione** che ha deciso il «Yes» (`proposition_because/2`, `clause_fact_said/2` in derivation.p0, forma canonica di `kb_clause/4`) e il C la conserva. «Why does the RCD trip?» → «Because washing machine leaks current.»; «Why is the ground wet?» (dopo «it rains») → «Because it rains.»; «Why is the sky blue?» → la spiegazione enciclopedica. A/B su 12 file con domande «why»: identico | mondo allargato (la ragione come sostegno), L4 §2.4 (la regola discutibile), derivation.p0 |
| 4 | F21.1: con le durate lette, «Which is faster, the bus or the train?» → muro; «Is 20 minutes less than 40 minutes?» → muro; la direzione era una casistica per parola (`comparative_word/3`) e il confronto un `mod_compare` C di cinque parole | il verso di un confronto viene dal **polo** di una grandezza; il resto si deriva. G15, G16, G17 insieme | nuovo [kb/core/scales.p0](../../kb/core/scales.p0) (incluso da `decisions.p0`): unità di tempo con la base comune; valore di una quantità (stessa unità non convertibile: si confronta con se stessa); valore di una relazione come operando (`operand_amount/4` via `relation_noun/2`); poli (`magnitude_adjective/2`, i `comparative_less/more` già in lexicon.p0); **una** tabella di verso (`pole_direction/2`); comparativo e superlativo derivati dalla morfologia; scelta fra due con la ragione, verdetto, superlativo su tutto l'insieme; la lezione parlata «<aggettivo> means a <grandezza> <relazione>». Le forme di confronto **cedono** quando non trovano niente (A/B su 7 domande di confronto già note e su `compare`, `numeric_questions`, `magnitude_compare`: identico) | L4 (il precedente della condizione insegnata), IR (il sintagma detto → l'entità: `said_entity/2` con `np_opener/1`), comprensione universale |
| 5 | **F21.1 risolto per intero**: obiettivo detto («I want to get home as soon as possible.»), orari di partenza e durate, «Should I take the bus or the train?» | quattro pezzi generali: (a) l'ora del giorno come scala (`clock`, secondi dalla mezzanotte, am/pm); (b) una **relazione calcolata** insegnata a parole («The arrival time is the departure time plus the travel time.»), con la scala della somma (ora + durata = ora); (c) i poli del tempo (`early`/`late`) e la lezione con «an» («Soon means an early arrival time.»); (d) l'**obiettivo** della conversazione (`pursued_quality/1`) e la scelta che lo usa, con la cessione di `turn_plan` dichiarata in KB (`faculty_yield_when(turn_plan, open, …)`, predicati binari: la porta chiede `Pred(current_turn, Testimone)`) | «Should I take the bus or the train?» → «The train: its arrival time is 6:45 pm, against 6:50 pm for the bus.»; lo stesso per «Which is sooner, …?». Due trappole del motore pagate e scritte nel file: `measure_pred` non deve passare da `relation_noun` con il predicato legato (budget esaurito), e la scelta non deve **ricalcolare** i valori (il risolutore a continuazioni supera la profondità 64 e fallisce in silenzio: la coppia ordinata porta i valori). A/B: domande di confronto note identiche; `planning/` identico a senza `scales.p0` | K11 (situazione: stati, leggi), K3 (l'obiettivo come stato del dialogo), L4 (condotta come KB), IR |
| 6 | G9 (F1.1): «The kettle, the fridge and the washing machine are appliances.» → `Learned: fridge is an appliances. Learned: washing machine is an appliances.` (il primo elemento sparisce, la classe resta al plurale) | due difetti generali di ogni elenco con la virgola: (a) lo sbucciatore degli incisi (`adjunct_peel`, gen513) prendeva il primo elemento per un inciso; (b) l'accordo prendeva la classe con il punto finale («appliances.») e non la singolarizzava | (a) `residue_continues_list/1` (grammar.p0): se il residuo porta una congiunzione prima del primo verbo, continua un elenco e il turno resta intero (la scansione si ferma anche sulle forme verbali: «composting requires … leaves, grass, and food scraps» si sbuccia ancora); (b) la classe senza punteggiatura di bordo. → «Learned: kettle is an appliance. Learned: fridge is an appliance. Learned: washing machine is an appliance.». Tracce di diagnosi `coord` a profondità 3. A/B: `prose_compost` identico, `kb_conjunction` verde | IR (elenco e inciso come nodi, non posizioni) |
| 7 | **G12, la mossa informativa (F1.1)**: con la legge «if x leaks y then the RCD trips», i tre elettrodomestici e nessun colpevole noto, «Which appliance leaks current?» → prima un **falso** («kettle, fridge, washing machine.»: un lettore elencava la classe ignorando la relazione), poi nessuna mossa | l'operatore di isolamento (K9) in KB: si propone quando la KB trova **per inferenza** una legge d'effetto letta dalla clausola (`holds(E) :- P($X, …)` via `kb_clause/4`), almeno due candidati detti nella conversazione (atto di sessione), un'azione insegnata a parole che toglie P a un membro della classe, e nessun colpevole noto. Nessun nome di dominio nel meccanismo | nuovo [kb/core/isolation.p0](../../kb/core/isolation.p0): la lezione «Unplugging an appliance stops it from leaking current.»; dopo, «Which appliance leaks current?» / «How can I find out which appliance leaks current?» → «I can't tell yet which appliance leaks current: it could be the kettle, the fridge or the washing machine. You could try unplugging them one at a time. Unplugging an appliance stops it from leaking current, so when the residual current device stops tripping, the last one you changed is the one.»; con il fatto «The washing machine leaks current.» → «The washing machine: you told me so.»; senza l'azione, la risposta onesta al posto del falso. **Trasferimento** a un campo mai nominato (i plugin di una build: «Disabling a plugin stops it from corrupting files.» → «…You could try disabling them one at a time… so when the build stops failing…»), **contrasto** con un solo candidato (nessuna mossa). A/B su 6 file con domande «which»: identico. Livello §3: **4–5** | K9 (operatore), K11 (situazione), K3 (la questione aperta), mondo allargato (il candidato non è un colpevole finché nessuno lo dice) |
| 8 | G1+G2 (F1.1): la conversazione come la direbbe una persona: «The RCD keeps tripping.» → un fatto su «keep»; «I can't work out which appliance is causing it.» → nessuna mossa; «I don't know what to try next.» → il modello da coach di `turn_plan` | (a) un effetto è **osservato** se detto nella conversazione, anche con l'aspetto continuativo (una regola per ogni verbo: «keeps tripping» ⇔ «trips», con `verb_form/3`); (b) il **blocco** con un effetto osservato fa nascere la mossa; (c) se manca l'azione, parrot0 **chiede la conoscenza** che gli servirebbe invece di un muro; (d) `turn_plan` cede in quei turni (KB) | `observed_effect/1`, `isolation_for_effect/2`, `isolation_any/1` e le forme di blocco in isolation.p0. La conversazione intera: «The RCD keeps tripping.» / «I can't work out which appliance is causing it.» → «I can't tell yet which appliance is causing it: it could be the kettle, the fridge or the washing machine. To find out, I need to know what stops an appliance from leaking: can you tell me?» / «Unplugging an appliance stops it from leaking current.» / «I don't know what to try next.» → la mossa con il perché. A/B `user_situations`, `plan`, `planact`: identico | mondo allargato (il racconto è una situazione riportata, il blocco è uno stato della persona), K3 (la domanda di conoscenza come mossa), comprensione universale (dire che cosa manca) |
| 9 | F21.2 (trasferimento: il volo più economico, euro non convertibili, «Cheap means a low price.»): scelta con obiettivo e superlativo reggono; il verdetto con un soggetto di più parole («Is the night flight cheaper than the morning flight?») fallisce in silenzio | due cause: (a) uno span seguito da uno slot non ha confine per il lettore delle forme; (b) **il tetto di profondità del risolutore limitava la lunghezza della prova, non la sua profondità** (a continuazioni, i goal fratelli ereditavano `depth + 1`): ogni composizione KB-first lunga falliva in silenzio | (a) la coda della domanda la legge la KB (`comparative_verdict_clause/2`: la parola prima di «than»); (b) **motore**: ogni goal di un corpo porta la profondità del suo nodo (campo `depth` di `Term`), 64 torna a essere la profondità dell'albero. «Is the night flight cheaper than the morning flight?» → «No, the morning flight: its price is 120 euros, against 140 euros for the night flight.». A/B della correzione su 24 file campionati in tutte le cartelle dei test: identico. Specie S12 in [kb-growth-dynamics.md](../kb-growth-dynamics.md) | IR (la coda letta dalla KB), motore (un limite misura ciò che dice di misurare) |
| 10 | SA7/G5: «if x leaks current then the RCD trips» → `holds(…) :- holds(x_leaks_current)` (un atomo opaco: la legge non si lega ai fatti); il ripiego «if x leaks y then …» era troppo largo («The pipe leaks water.» faceva scattare il differenziale) | una legge ha di solito **una** variabile e una costante; il legatore dei frame del lettore delle regole pretendeva due variabili | basta una variabile: l'altro argomento resta una costante scritta come le entità (`p0_rule_constant`). → `holds(residual_current_device_trips) :- leaks($V1, current)`; «The pipe leaks water.» → «Does the RCD trip?» → «Not necessarily.»; la conversazione F1.1 regge con la legge stretta. A/B su 5 file che insegnano regole: identico (le due righe di `conditional_plan` sono il rosso di tempo/contenuto preesistente) | L4 (la regola che la comprensione usa, con le sue condizioni: C4) |
| 11 | livello 6 della scala (adattarsi all'esito): dopo la mossa, «I unplugged the kettle.» / «It still trips.» non avevano lettura | l'azione fatta si riconosce dalla lezione d'azione (il passato dal gerundio: «unplugging» → «unplugged», nessun elenco); l'esito restringe i candidati (chi è stato tolto e l'effetto continua, non è lui) e propone il prossimo; «It stopped …» indica l'ultimo tolto, con l'ipotesi dichiarata del guasto singolo | isolation.p0 §9. Con la KB salvata: «Then it isn't the kettle: it could still be the fridge or the washing machine. Try unplugging the fridge next.» → «…that leaves the washing machine, so it must be the one that leaks current (if only one appliance is at fault).» → «Then the washing machine is the one that leaks current: once it was out of the way, the effect stopped.». Lo stesso sul gemello dei plugin («I disabled the linter.» / «It still fails.»). Livello §3: **6** | K11 (la situazione cambia con l'azione), K3 (la questione si restringe), mondo allargato (l'ipotesi del guasto singolo detta, non presunta in silenzio) |
| 12 | sessione B: «Why does the RCD trip?» su un effetto osservato con causa ignota → «I don't know about residual»; e dopo il riavvio «Does the RCD trip?» non si leggeva | (a) un effetto **riportato** vale: l'osservazione continuativa diventa un fatto (contabile del turno); (b) il suo perché è **abduttivo**: la condizione della legge detta con «something», o il colpevole se noto; (c) `proposition_seen/1` nasceva solo leggendo la regola nella sessione: dopo il riavvio la regola salvata non la registrava (C6) | `effect_reason/2` e `proposition_seen/1` derivata dalle regole (`kb_rule(holds, …)`, isolation.p0 §10–11); `proposition_because/2` preferisce la ragione della legge (derivation.p0). «Why does the RCD trip?» → «Because something leaks current, and I don't know yet which one.» → dopo il fatto «Because the washing machine leaks current.». Due trappole di costo pagate: `kb_clause/4` in un predicato interrogato a ogni turno (64 ms a chiamata, `soft-test` rosso) e una regola `holds($E) :- …` a testa variabile, che rendeva «vista» ogni proposizione. A/B su regole, situazioni, pianificazione: identico (il contenuto di `conditional_plan` sotto 1 s è il rosso preesistente) | mondo allargato (l'abduzione come ragione dichiarata), L4 C6 (persistenza) |
| 13 | G13: la scena detta con i verbi («The bus leaves at 6:10 pm.», «The bus takes 40 minutes.») | (a) le costruzioni si insegnano **parlando** (L2): «x leaves at y means the departure time of x is y», «x takes y minutes means the travel time of x is y minutes»; (b) ma la durata la rubava il lettore del viaggio in prima persona (`event-time.p0`), che scattava su qualunque soggetto e lo chiamava «trip»: la causa di SA10 | la durata è del viaggio solo se il soggetto del verbo è il viaggio (`journey_word/1`: trip, journey, it, viaggio, l'impersonale «ci»). «The bus takes 40 minutes.» → «Learned: the travel time of bus is 40 minutes.»; «The trip takes 3 hours.» resta al viaggio. A/B sui file del viaggio e delle durate: identico | IR (il soggetto del verbo, non la posizione), gerarchia di crescita #1 (le costruzioni insegnate a parole) |
| 14 | sessione C (dimostrazione in frasi naturali sulla KB salvata): dopo «It stopped tripping.» → «Then the fridge is the one…», ma «Why does the RCD trip?» → «…I don't know yet which one» | la conclusione di una prova non entrava nella rete (C1): va registrata come **giudizio**, distinto dai fatti riportati, con la sua fonte e la sua ipotesi | `isolation_concluded/3` (dopo la risposta, dal turno letto da «It stopped …»); la ragione dell'effetto lo usa: «Because the fridge leaks current, as your tests showed (if only one is at fault).» | L4 C1 (coerenza), mondo allargato (giudizio vs fatto, con la fonte) |
| 15 | le **formulazioni originali** della prova: «How do I find out which appliance is making the residual current device trip?» (G3 del primo giro) → prima «how to make trip»; «Which should I take, the bus or the train?», «Is it better to take …?» → il modello «Both can be the right choice» | (a) l'effetto può essere **nominato nella domanda** stessa, non solo osservato prima: dalle parole si ricava la proposizione (la radice riportata alla terza persona) e si controlla su una legge; (b) la cessione di `turn_plan` dipende dalle parole della scelta e della causa (KB), non dalla posizione | `effect_from_said/2`, `isolation_for_named_effect/3` e le forme «how do I find out which … is making/causing …» (isolation.p0 §12); varianti della scelta e `choice_cue/1` (scales.p0). Tutte e quattro danno la mossa o la scelta giusta. A/B su situazioni e pianificazione: identico | IR (l'effetto come nodo della domanda), mantra #17 (condotta come KB) |
| 16 | F21.1, livello 7 (spiegare la scelta): dopo «Should I take the bus or the train?» → «The train…», il «Why?» ellittico → «I don't understand that yet.» | (a) una domanda nuda si risolve sul **turno precedente**: la scelta fatta è uno stato del dialogo (K3), non un'eco; (b) il lettore delle forme scartava ogni turno di una parola | `last_choice/2` scritta **dopo** la risposta da un contabile KB (`after_reply_bookkeeper(last_choice_note)`, dalle fasce lette che il lettore ora pubblica: `turn_form_slot/3`); `choice_why/1` ricompone la ragione dall'obiettivo e dal confronto; il lettore legge un monosillabo solo se la KB dichiara che quella parola apre una forma (`turn_form_word_alone/1`, niente giro delle forme a ogni «ok»). «Why?» → «Because you want it as soon as possible. The train: its arrival time is 6:45 pm, against 6:50 pm for the bus.»; «Why?» senza scelta: come prima. `soft-test` verde in 3 s | K3 (la questione chiusa resta interrogabile), IR (l'ellissi si lega al nodo dell'ultima scelta) |
| 17 | SA9, riprovato a fine sessione: «Is 20 minutes less than 40 minutes?» → «I don't know about minutes» (il verdetto leggeva solo entità con un valore memorizzato) | il confronto fra due **quantità dette** è lo stesso confronto fra entità, senza il passo del valore memorizzato | un secondo ramo di `comparative_verdict_clause/2` (scales.p0): entrambe le quantità nella stessa scala (`quantity_amount/3`), il verso dal polo della parola (`nearer_pole/3`). «Is 20 minutes less than 40 minutes?» → «Yes.»; «Is 1 hour more than 40 minutes?» → «Yes.»; «Is 40 minutes less than 40 minutes?» → «No.»; «Is 50 decibels more than 45 watts?» → non confrontabile. A/B su `compare`, `magnitude_compare`, `numeric_questions`, `taught_reply_word`: identico | comprensione universale (una scala, due modi di arrivarci) |

## 8. Da dove si riparte

**Stato a fine della sessione di 5 ore (27 settembre 2026, 20:30).** I due
problemi presentati si risolvono **dalla KB salvata, in frasi naturali**:

- **F1.1 (il differenziale)**: livello 6 della scala §3, cioè la mossa di
  isolamento per inferenza (§10 iter. 7–8, 15), l'adattamento all'esito
  (iter. 11), la conclusione registrata come giudizio (iter. 14) e il perché
  abduttivo (iter. 12). Trasferito ai plugin di una build (F1.3), contrastato
  con un solo candidato.
- **F21.1 (bus o treno)**: livello 7, cioè la scelta per un obiettivo con una
  relazione calcolata insegnata a parole (iter. 4–5), le costruzioni della
  scena insegnate a parole (iter. 13) e la scelta spiegata con il «Why?»
  (iter. 16). Trasferito ai voli (F21.2: euro, «cheap»), contrastato con tram
  (più presto) contro taxi (più veloce).

Residui, in ordine di resa:

0. **Addestramento prima della verifica.** Ogni sessione colma con L4/L3/L2
   ciò che si può colmare (§2-bis) e salva solo le **lezioni**, mai i dati di
   situazione (l'errore di A → A2 è in
   [train-the-learning-process.md](train-the-learning-process.md), §SA).
1. **G13-bis / SA8**: la coordinazione dopo una costruzione insegnata si
   legge a metà, sia con il soggetto condiviso («The bus leaves at 6:10 pm
   and takes 40 minutes.» → lo slot @S prende «bus leaves at 6:10 pm and»)
   sia con due soggetti («The bus gets me home at 6:50 pm and the train gets
   me home at 6:45 pm.», dopo la costruzione «x gets me home at y means the
   arrival time of x is y» → solo il bus). **Diagnosi (fine sessione)**:
   `p0_frame_reading` (10-memory-knowledge.c, `frame bind` nella traccia) lega
   lo schema fino a «and» e sa quanto ha coperto (`consumed` < `total`, già
   esposto come `covered(N), of(M)` alla policy KB
   `normalization_extent_policy/4`); il resto non viene riletto. La cura:
   quando il resto comincia con una `conjunction/1`, rileggerlo come clausola,
   con il soggetto della prima se il resto comincia con un verbo (lo stesso
   gesto di `predicate_coordination_split` in 99-registry.c, che copre solo
   «V1 and V2 O» nelle relative). La decisione di rileggere è KB (una policy
   sull'estensione), il C taglia.
2. **I candidati come classe salvata**: `situation_member` si appoggia agli
   atti della sessione; «The kettle is an appliance» è vero e va salvato come
   conoscenza di classe, separato dalla scena.
3. **F21.6 (la puntualità)**: un obiettivo con una soglia («I must be there by
   7») è un vincolo, non un polo; va letto come condizione che scarta, prima
   della scelta.
4. **La doppia lettura** di «X means a low Y» (proposizione `mean` accanto
   alla lezione): **non si riproduce** nel test engine a fine sessione
   («Heavy means a high weight.» → nessun `mean/2`, nessun `holds/1`); da
   ricontrollare solo se ricompare in una sessione live. Osservazione nuova:
   dopo `!reset` la lingua appiccicosa del demone è `it` (`read.lang
   sticky=it selected=it`), e una prima lezione inglese riceve la risposta in
   italiano («Tengo: «pesante» vuol dire weight alto.»): la lezione entra, la
   lingua della replica no. Una frase inglese intera dovrebbe bastare a
   cambiare lingua.
5. **Le famiglie non ancora provate** (F2–F20): la prossima con resa alta è
   **F5** (cambiare una variabile alla volta), che riusa l'operatore di
   isolamento con l'azione «cambiare» al posto di «togliere». Poi F4 (catena di
   precondizioni: una legge con più condizioni, provate dalla più semplice).
6. La replica sociale a «That sounds nice» dopo una scelta.

Le sessioni seguono il §4; ogni sessione aggiorna §2, §6 e, se nasce uno
scenario nuovo, §5.
