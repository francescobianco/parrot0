# Apprendimento assistito

> ## ⛔ Vincolo zero: la chat non è una API travestita
>
> Una lezione è valida soltanto se il teacher la esprime in **lingua naturale**
> e parrot0 deve ricostruirne forma, ruoli e conseguenze. È vietato far passare
> dalla chat Prolog/P0, nomi di predicati interni, arità, tuple, `!assert`, MCP o
> equivalenti testuali di `kb.assert`. Cambiare il trasporto da API a prompt non
> è comprensione.
>
> **Test non negoziabile:** un teacher competente nel dominio ma ignaro dello
> schema interno della KB saprebbe formulare la lezione? Se deve conoscere
> `relation_noun/2`, `answer_frame/2`, `topic_action_surface/3`, parentesi o
> nomi simili, l'esperimento è invalido. Il risultato vale zero anche quando il
> comportamento successivo è corretto.
>
> Quando la lingua naturale non basta, non si abbassa il livello esponendo la
> rappresentazione: ci si ferma sul confine osservato e si amplia la
> meta-comprensione. Qualunque run che violi questo vincolo resta soltanto una
> diagnosi del gap e non può essere riportata come apprendimento riuscito.

**Stato:** piano attivo, 2026-08-27  
**Missione:** fare della conversazione il canale primario con cui parrot0 amplia
non soltanto i fatti, ma le forme con cui comprende, compone, interroga e
ragiona.  
**Prima evidenza:**
[`depth-session-01.md`](../labs/apprendimento-assistito/depth-session-01.md).  
**Cancello:** §6 — nessuna sessione lunga prima che gli strati di
meta-comprensione M0–M14 siano chiusi.  
**Ritmo:** §11 — ogni cosa appresa è un piccolo incremento, e si committa e
pusha anche se parziale.

## 1. Il pensiero da rendere vero

La KB è davvero viva quando, davanti a una frase che non sa comprendere, si può
dire a parrot0:

> Vedi, questa forma significa quest'altra cosa; qui il primo nome è chi agisce,
> il secondo è ciò su cui agisce.

e parrot0 non si limita a memorizzare quelle parole. Deve:

1. proporre una nuova lettura candidata;
2. ripetere con parole proprie che cosa ha capito;
3. rileggere la frase che prima falliva;
4. riuscire su frasi nuove con la stessa struttura;
5. distinguere un controesempio;
6. conservare la lezione con provenienza;
7. poterla correggere o dimenticare parlando.

Questa è la forma operativa di “bene, ho capito”. Un riconoscimento verbale, un
fatto aggiunto o la risposta esatta all'esempio insegnato non bastano.

La scommessa del progetto resta plausibile: il C può essere l'esecutore minimo
di meccaniche stabili e una KB viva può contenere lessico, costruzioni, ruoli,
regole, procedure e criteri di scelta. Ma una KB grande non è automaticamente
una mente generale. La capacità decisiva è **acquisire nuove astrazioni e farle
comporre tramite il dialogo**.

## 2. Che cosa ha stabilito l'esperimento iniziale

Il primo esperimento teacher→parrot0 ha usato soltanto turni di chat normali,
in processi isolati, senza `!assert`, senza MCP, senza modifiche a C o `.p0` e
senza salvare la sessione nella KB del repository.

Il risultato non è né “funziona” né “non funziona”. La frontiera è precisa:

| Livello insegnato parlando | Esito attuale |
|---|---|
| membro di una classe grammaticale | forte: uso immediato, transfer e retract |
| verbo relazionale nuovo | forte nel frame soggetto-verbo-oggetto già noto |
| regola unaria e catena inferenziale | forte |
| regola relazionale con tre variabili | forte nel registro `if … then …` noto |
| congiunzione nuova | forte: cambia il parsing al turno successivo |
| nuova frase per un intento noto | forte |
| nuovo verbo con cui insegnare | forte e riflessivo |
| nuova risposta per un intento noto | forte |
| prosa lunga | parziale e dipendente dall'instradamento e dalle forme |
| parafrasi interrogative | fragile |
| “X significa Y” come equivalenza operativa | assente |
| nuova costruzione con ruoli | assente come atto didattico generale |
| procedura descritta in prosa | assente; oggi può produrre un misclaim |
| ragionamento situazionale sugli scacchi | assente nei casi provati |

La scoperta centrale è questa:

> parrot0 sa imparare quando la lezione è già espressa in un metalinguaggio che
> conosce; non sa ancora apprendere liberamente un nuovo pezzo di metalinguaggio.

Dire `glorphs is a relation verb` funziona perché “relation verb” è già un ponte
fertile. Dire `glints means glorphs` non rende `glints` utilizzabile come
`glorphs`. Dire una nuova procedura di conversione non la compila: la frase
viene scambiata per una richiesta di calcolo e produce numeri errati.

Questa frontiera impedisce oggi di promettere che ogni fallimento sia
indirizzabile semplicemente parlando. È esattamente il motore che questo piano
deve costruire.

## 3. Comprensione, non frasario

Una lezione supera il gate di comprensione soltanto se passa tutti questi
controlli:

- **replay:** la frase originale prima rossa ora viene letta correttamente;
- **transfer:** almeno tre esempi mai pronunciati dal teacher usano la stessa
  struttura;
- **parafrasi:** cambia la superficie, resta il significato;
- **controesempio:** una frase quasi uguale ma semanticamente diversa non viene
  assorbita dalla regola;
- **composizione:** la nuova abilità funziona insieme ad almeno una vecchia;
- **ablation:** dimenticando la lezione, il comportamento acquisito scompare;
- **retention:** dopo altri turni la capacità resta disponibile;
- **provenienza:** parrot0 sa quale lezione sostiene quella lettura;
- **onestà:** nessun “ho capito” prima che replay e transfer siano verdi.

L'esatto prompt→risposta può essere utile come memoria episodica, ma vale zero
come comprensione universale finché non produce transfer. Nell'esperimento la
risposta insegnata per `c1→h6` non si è trasferita a `c1→g5`: è frasario, non una
regola degli scacchi.

## 4. L'unità di crescita: una transazione didattica

Ogni episodio di apprendimento assistito segue un solo ciclo osservabile:

1. **Fallimento conservato.** Input, risposta, modulo vincitore, letture
   concorrenti e stato della KB vengono registrati; nulla viene sovrascritto.
2. **Tipizzazione del gap.** Il sistema distingue almeno lessico, denotazione,
   costruzione, legame dei ruoli, composizione, coreferenza, scope, regola,
   procedura, risposta e conflitto fra intenti.
3. **Domanda utile.** Parrot0 non chiede genericamente “vuoi insegnarmelo?”: dice
   quale informazione gli manca e in quale forma sa riceverla.
4. **Spiegazione del teacher.** La lezione arriva come lingua naturale normale,
   eventualmente con esempi e controesempi; non come `.p0` travestito.
5. **Candidato in quarantena.** La nuova conoscenza non entra subito nella KB
   consolidata. Vive in un contesto temporaneo e conserva la spiegazione fonte.
6. **Rilettura.** Il turno originale viene riprocessato con il candidato.
7. **Prove nascoste.** Il teacher genera esempi di transfer, parafrasi e casi
   negativi che non erano contenuti nella spiegazione.
8. **Confronto.** Parrot0 espone lettura precedente, lettura nuova e motivo della
   scelta. Un pareggio resta ambiguità, non diventa certezza arbitraria.
9. **Promozione o conservazione del fallimento.** Solo una lezione verificata
   entra nella KB viva. Se fallisce, candidato, trace e ipotesi restano nel lab.
10. **Retract parlato.** Il teacher deve poter revocare la generalizzazione e
    osservare che la capacità collegata scompare.

La transazione deve essere atomica: una lezione non può lasciare mezza grammatica
attiva dopo avere fallito i controlli.

## 5. La scala del metalinguaggio insegnabile

Il bootstrap non si risolve con un dizionario enorme. Ogni lezione nuova deve
potersi ancorare a qualcosa che parrot0 sa già distinguere. La scala da aprire è:

1. **membri:** “`movo` è un quantificatore universale”;
2. **denotazioni:** “`glints` qui ha lo stesso senso relazionale di `glorphs`”;
3. **ruoli:** “prima di `glints` c'è l'agente, dopo c'è l'oggetto”;
4. **costruzioni:** “`X glints Y` esprime `glorphs(X,Y)`”;
5. **composizione:** coordinazione, modificatori, relative e apposizioni;
6. **scope e riferimento:** negazione, condizionali, quantificatori, pronomi;
7. **regole:** antecedenti multipli, eccezioni, causalità e vincoli;
8. **procedure:** una descrizione diventa un piano eseguibile con slot tipati;
9. **atti dialogici:** cosa una formulazione chiede di fare, ricordare o
   correggere;
10. **criteri di scelta:** come preferire una lettura senza nascondere le altre.

Oggi i gradini 1 e alcune isole dei gradini 4, 7 e 9 sono vivi. Il ponte generale
fra 2–6 manca. È il prossimo nucleo, non un altro lotto di risposte di dominio.
Il §6 traduce questa scala nell'inventario operativo di ciò che manca, e ne fa la
precondizione di qualunque sessione lunga.

## 6. Il cancello duro: nessuna sessione lunga prima del metalinguaggio

> ⛔ **PRIORITÀ, indicata da F. il 2026-08-28.** Fra tutti gli strati di questo
> capitolo, **M15 — le forme della domanda — va sbloccato per primo e il prima
> possibile** (§6.2b). Non è una preferenza di ordine: è che M15 è l'unico che
> rende *invisibili* i risultati di tutti gli altri. Si possono insegnare fatti
> nuovi tutto il giorno, ma se non si può insegnare **come si chiedono**, quei
> fatti non rispondono a nessuno — e il giro di addestramento sembra fallito
> quando invece era riuscito.
>
> La misura che lo dice: `270` formulazioni interrogative scritte a mano coprono
> `136` relazioni, quasi tutte in inglese. `which colors are used in chess`
> risponde; `which color is used in chess` no. Oggi la differenza si colma
> **editando un file**, e questo contraddice direttamente il criterio del
> progetto: *l'addestramento via prompt è lo standard e non va mai bypassato.*

**Regola.** Le dieci ore del §7 — e a maggior ragione qualunque teacher che giri
come processo continuo — non si avviano finché gli strati di meta-comprensione
elencati in questo capitolo non sono chiusi. Non è un ordine dei lavori
preferibile: è una conseguenza di che cosa una sessione lunga può misurare. Prima
del cancello, dieci ore non misurano l'apprendimento; misurano la copertura delle
forme che il motore già supportava, moltiplicata per la pazienza del teacher nel
tradurre tutto in quelle forme.

Le tre ragioni per cui il cancello è duro:

1. **La durata non crea astrazione.** Se un fallimento non è indirizzabile
   parlando, resta non indirizzabile alla decima ora come alla prima. Aggiungere
   turni aggiunge fatti, non forme.
2. **Ciò che non è insegnabile viene sostituito da frasario.** Il teacher, per
   non fermarsi, ripiega sulla risposta esatta: è il caso `c1→h6` che non
   trasferisce a `c1→g5`. Dieci ore così producono una KB grande e una
   comprensione ferma, e la crescita del numero di fatti nasconde esattamente
   questo.
3. **Senza tipizzazione del gap e quarantena, l'errore si consolida.** Una
   sessione lunga scrive. `organism(metabolism)` e la risposta `4` sull'arrocco
   mostrano che oggi il sistema non distingue da sé un'estrazione falsa da una
   vera, né un muro da una risposta irrilevante. Il costo di accorgersene cresce
   con la durata della sessione.

### 6.1 Che cosa vuol dire «addestrabile a voce su tutto»

L'obiettivo non è un elenco di argomenti: è la chiusura di cinque modi di
fallire. Per ciascuno deve esistere un atto didattico pronunciabile in chat
normale che produca una capacità generale e verificabile.

| Il fallimento | Che cosa il teacher deve poter dire | Strati |
|---|---|---|
| **non sa** un fatto | l'asserzione, in prosa normale | chiuso (A0) |
| **non conosce** una forma | «questa forma significa quest'altra», «qui il primo nome è chi agisce» | M2–M6 |
| **non comprende** una struttura | «qui questo si riferisce a quello», «questo vale solo se…», «X perché Y» | M7–M9 |
| **non sa fare** qualcosa | «per fare X, prima Y poi Z», «quando dico così ti sto chiedendo di…» | M10, M11 |
| **non sa di non sapere** | «quella risposta non c'entrava», «qui non stai scegliendo, stai indovinando» | M0, M12, M13 |

L'ultima riga è quella che manca di più ed è quella che rende sicure tutte le
altre: finché parrot0 non sa dire *che tipo* di lacuna ha, la diagnosi resta a
carico del teacher — e un teacher automatico, che è il punto d'arrivo del §8, non
può che sbagliarla su scala.

### 6.2 Inventario degli strati mancanti

Ogni voce dichiara che cosa manca, l'atto didattico che deve funzionare e il gate
che ne autorizza la chiusura. Nessun gate si supera con gli esempi usati per
implementarlo.

**M0 — Onestà del turno: muro, risposta, falso.**  
*Manca:* la distinzione fra «non so», «so e rispondo» e «ho prodotto qualcosa che
non è ancorato a nulla». Evidenze: `4` per l'arrocco, `-16.6667` per la
conversione, la descrizione generale degli scacchi data alla domanda sul cavallo,
la prosa didattica trasformata in racconto.  
*Atto didattico:* «quella risposta non c'entrava con la domanda» deve produrre un
fatto sul turno — tipo di errore, modulo vincitore, letture concorrenti — non una
scusa.  
*Gate:* nessuna risposta priva di ancoraggio; ogni risposta sa esibire su
richiesta la conoscenza che l'ha prodotta; false-understanding rate a zero su una
batteria nascosta. **È il prerequisito di tutti gli altri: senza M0 ogni metrica
successiva è inaffidabile.**

**M1 — Un solo atto di lettura, deciso da conoscenza.**  
*Manca:* la priorità fra consumer è oggi l'ordine dei moduli. Lo stesso testo
finisce al lettore o al generatore di racconto a seconda del contenuto, e persino
`read:` viene rubato nel profilo integrato.  
*Atto didattico:* «questo è un testo da leggere, non da raccontare» — e la
preferenza deve valere per la *classe* del testo, non per quel testo.  
*Gate:* route stability pari a 1 sulla batteria di prosa, indipendentemente da
contenuto e profilo; ciò che non viene estratto è riportato come frase saltata
con il tipo di gap, mai sostituito da narrativa.

**M2 — Uso e menzione.** *(aperto — l'atto esiste, il gate non è chiuso)*  
*Fatto:* la menzione è un atto con due superfici, le virgolette e un marcatore
dichiarato (`mention_marker/1`). Il motore non nomina marcatori, classi,
articoli né copule. L'atto estende il proprio vocabolario: `"lemma" is a
mention_marker` abilita `the lemma albeit is a concession marker`, e l'ablazione
del marcatore lo richiude.  
*Manca:* la forma interrogativa (`is unless a condition marker?`); e una classe
dichiarata su una parola-funzione resta spesso conoscenza morta per i consumer,
che è il problema `greeting(ahoy)` e appartiene a M12.  
*Gate:* insegnare una proprietà di una parola-funzione attiva, usarla nello stesso
processo, ritrarla e vedere la capacità sparire.

**M3 — Denotazione ed equivalenza operativa.** *(A1: aperto — l'arita' e' caduta
il 2026-08-28, la catena e l'induzione no)*  
*Fatto:* `construction_frame/3` con la vista `extract_frame/2`, pivot insegnabile,
retract parlato.  
*Fatto anche:* l'**inversione dei ruoli** — i nomi degli slot li dà il lato già
compreso, quindi «`X glints Y` significa `Y glorphs X`» si conserva come
`construction_frame("@O glints @S", "@S glorphs @O", glorphs)`. E l'alfabeto
delle variabili è conoscenza: `a is a rule_variable` abilita nuovi slot nella
stessa sessione.  
*Fatto (2026-08-28):* l'**arita' diversa da due** e' caduta, e con lei l'idea che
il legame degli slot fosse binario per natura. Il pattern dichiara quanti slot
ha e il fatto nasce con quella arita'; il verbo ternario e la parola che collega
il terzo ruolo sono conoscenza (`ternary_relation_verb/1`, `link_word/1`), quindi
una relazione a tre ruoli nuova costa due frasi dette in chat. E la vista
interrogativa non e' piu' limitata al caso SVO: un interrogativo in uno slot
trasforma il pattern in una domanda a QUALUNQUE arita', su ciascuno dei ruoli.
Transfer su tre domini reali, ablation verde, zero regressioni.
Vedi [`2026-08-28-confine-addestrabilita.md`](../labs/apprendimento-assistito/2026-08-28-confine-addestrabilita.md).

*Manca ancora:* catena di costruzioni verificata; induzione dai soli esempi
concreti.

*E una scoperta che cambia la mappa:* M3 e M8 erano **lo stesso muro**, e la
sonda lo ha mostrato facendolo nominare a parrot0 in due domini diversi con la
stessa frase — «I cannot align exactly two shared variables on both sides». Una
sola condizione in `p0_explicit_pattern` (`*nvars != 2 || seen[0] != 1 ||
seen[1] != 1`) bloccava il conteggio dei ruoli per M3 e l'unicita' per M8. La
meta' di M3 e' caduta; la meta' di M8 — il bersaglio congiuntivo — no.  
*Atto didattico:* «`X glints Y con Z` significa …».  
*Gate:* arità ≠ 2 e catena a due passi, ciascuna con transfer held-out e
ablation.

**M4 — Ruoli nominabili.**  
*Manca:* il teacher può allineare due slot, ma non *nominare* i ruoli, che è
esattamente la frase con cui il §1 apre questo piano: «qui il primo nome è chi
agisce, il secondo è ciò su cui agisce». Non può insegnare un ordine diverso da
quello della lingua corrente, né ruoli oltre soggetto e oggetto (strumento,
luogo, tempo, beneficiario).  
*Atto didattico:* la frase del §1, letteralmente, senza slot espliciti.  
*Gate:* una costruzione a tre ruoli insegnata a voce e interrogabile su ciascun
ruolo; un ordine non canonico insegnato senza toccare il motore.

**M5 — Morfologia e dualità delle domande.** *(A2 — parzialmente aperto)*  
*Fatto:* la dualità c'è già per la forma flessa — `who brinks X?` e `what does Y
brinks?` rispondono entrambe dallo stesso verbo insegnato. E la forma base è
*addressable*: `krell means brinks` la apre in una frase.  
*Manca:* la forma base non emerge da sola dalla lezione, e resta il divario
singolare/plurale (`organism(algae)` risponde a una formulazione e non alla sua
variante).  
*Atto didattico:* una sola lezione su un verbo relazionale deve aprire
dichiarativa, domanda su ogni slot e varianti flesse; le irregolarità si insegnano
come eccezioni, non come seconda abilità.  
*Gate:* da una lezione, quattro superfici held-out verdi; una flessione
irregolare insegnata separatamente e composta con la prima.

**M6 — Composizione.**  
*Fatto:* la coordinazione dell'oggetto (gen439) è composizione KB, e produce un
bundle di proposizioni invece di un'ambiguità.  
*Manca:* relative, apposizioni, modificatori, coordinazione sugli altri ruoli e
sulle relazioni.  
*Atto didattico:* «`A, che è un B, R C` dice due cose».  
*Gate:* una frase produce più proposizioni corrette, ciascuna con la sua
provenienza, su forme mai pronunciate dal teacher.

**M7 — Scope e riferimento.**  
*Manca:* negazione, condizionali, quantificatori, pronomi, ellissi, e il
riferimento che attraversa i turni.  
*Atto didattico:* «qui "esso" è la luna», «questo vale solo se…», «nessun X è Y».  
*Gate:* paragrafi mai visti con pronomi ed ellissi risolti correttamente; una
negazione insegnata non deve invertire la lettura di una frase che le somiglia
soltanto in superficie.

**M8 — Ricongiungimento delle rappresentazioni.**  
*Nota (2026-08-28):* condivide la CAUSA con M3 — la stessa condizione in
`p0_explicit_pattern`, e la stessa frase quando parrot0 nomina il muro. Metà è
caduta con l'arità; il bersaglio congiuntivo no. Riprodotto su conoscenza vera:
«basalt is an igneous rock» memorizza `igneous_rock(basalt)` e «is basalt
igneous?» va a muro, cioè `occupied_square(d2)` alla lettera.  
*Manca:* `occupied_square(d2)` non soddisfa `occupied(X), square(X)`. Due
traduzioni ragionevoli della stessa nozione restano conoscenza viva e isolata, e
una sessione lunga moltiplica il debito invece di ridurlo.  
*Atto didattico:* «essere un `occupied_square` vuol dire essere `occupied` ed
essere `square`» deve costruire il ponte nei due versi.  
*Gate:* una domanda posta nella forma composta trova la risposta memorizzata in
quella scomposta e viceversa; l'ablation toglie il ponte e non i fatti.

**M9 — Struttura del discorso in prosa.**  
*Manca:* entrano enumerazioni di membri; restano fuori causalità, finalità,
processo e condizione. E l'estrazione produce atomi falsi — `organism(metabolism)`
da «fuel their metabolism».  
*Evidenza 2026-08-29, SC1-A:* il testo possiede ora unita' ordinate con span e
un primo arco di contrasto derivato da una classe insegnabile a voce;
`Transfer@3=3/3`, retract e reteach sono causali. Il gate M9 resta aperto:
SC2-A porta la coverage delle claim **di superficie** a `8/8`, con fonte,
attribuzione e status vivi, ma la normalizzazione proposizionale resta `0/8` e
le domande naturali sul contenuto `0/3`. Nessun atto di metodo/risultato/limite
e nessuna causalita' sono ancora rappresentati.
*Atto didattico:* «in un testo, "X perché Y" dice che Y causa X» — la lezione
riguarda la forma del discorso, non il tema.  
*Gate:* precisione dell'estrazione misurata, non solo la quantità: «Learned N
facts» non è una frase ammessa finché la precisione non è nota; e domande causali
sul testo, non solo di appartenenza.

**M10 — Procedure insegnabili.** *(A4)*  
*Manca:* menzionare una procedura non è distinguibile dall'eseguirla; niente slot
tipati; niente transfer su numeri nuovi.  
*Atto didattico:* «per convertire A in B, moltiplica per k e aggiungi h» non deve
produrre un numero.  
*Gate:* la frase didattica non calcola; il piano risultante lega i numeri a ruoli
e si applica a input nuovi; l'esecuzione sa esibire i passi.

**M11 — Atti dialogici.**  
*Manca:* che cosa una formulazione *chiede* di fare — rispondere, ricordare,
correggere, chiedere chiarimento, rifiutare — è oggi cablato nella scelta dei
moduli.  
*Atto didattico:* «quando dico "…" ti sto chiedendo di correggere ciò che hai
appena detto».  
*Gate:* un atto nuovo insegnato cambia il comportamento su formulazioni non
insegnate, e si ritrae.

**M12 — Criteri di scelta fra letture.**  
*Manca:* quando due letture competono, la scelta la fa l'ordine dei moduli. Il
teacher non può insegnare una preferenza, e un pareggio non è visibile.  
*Atto didattico:* «in un testo così, preferisci questa lettura» — senza cancellare
l'altra, coerentemente con la regola di non potare le strutture secondarie.  
*Gate:* la preferenza è un fatto ritrattabile; un pareggio resta ambiguità
dichiarata e mai certezza arbitraria.

**M13 — Tipizzazione del gap e domanda utile.**  
*Manca:* la classificazione del fallimento (§4, punto 2) e la domanda che ne
consegue (§4, punto 3). Oggi il muro sa suggerire un metalinguaggio preesistente
— «dì «X è un relation verb»» — e non sa dire «mi manca la costruzione», «mi manca
il referente di questo pronome», «mi manca la procedura», «ho due letture pari».  
*Atto didattico:* nessuno: qui è parrot0 che deve parlare per primo.  
*Gate:* su una batteria di fallimenti eterogenei il tipo di gap dichiarato è
corretto, e per ogni tipo dichiarato esiste almeno un atto didattico che lo
chiude. **Senza M13 il teacher automatico del §8 non è avviabile**, perché
tenterebbe la lezione sbagliata sul gap sbagliato.

**M14 — Quarantena, promozione e genealogia.** *(meccanismo trasversale)*  
*Manca:* il candidato in contesto temporaneo, le prove nascoste generate dopo la
lezione, la promozione solo se verde, il rollback atomico, e la genealogia degli
stati `active`/`superseded`/`failed`/`partial`.  
*Gate:* una lezione che fallisce le prove non lascia mezza grammatica attiva; ogni
capacità sa dire da quale lezione discende; il retract di quella lezione la fa
sparire e non tocca il resto.  
*Nota:* i fatti che servono a questo strato — `fact_source/3`, `reading_fact/2`,
`utterance/3`, i registri di gap — sono esattamente quelli che oggi finiscono
nella ricaduta di `/save` e che verrebbe facile scambiare per rumore. M14 non si
costruisce sopra a un filtro che li butta.

### 6.2b Ciò che oggi NON si può insegnare parlando — misurato al gen456

Le voci sopra sono strati progettati. Questa sezione è diversa: è ciò che è
**emerso provando**, in una sessione di giri `LEARN_PROTOCOL` fatta apposta per
far fallire l'addestramento via prompt su frasi articolate. Ogni riga ha una
misura, non un'impressione.

Va letta con il criterio di F.: *l'addestramento via prompt è lo standard e non
andrà mai bypassato.* Quindi ogni voce qui è un punto in cui, oggi, per insegnare
qualcosa **bisogna aprire un file** — e finché è così quella capacità è fuori dal
protocollo.

#### M15 — Le forme della DOMANDA non sono insegnabili. **È il blocco più urgente.**

*Manca:* `answer_frame/2` — la relazione fra una superficie interrogativa e la
relazione che deve interrogare — non è raggiungibile da nessun atto didattico.
`learnable/3` ha quattro modi (`exact`, `substring`, `fill`, e la maniglia
generica `cue_for`/`reply_for` del M11) e **nessuno arriva a `answer_frame`**.

*Misura:* `270` righe `answer_frame` per `136` relazioni distinte — due
formulazioni a testa in media, quasi tutte inglesi. La conseguenza si tocca con
mano:

```
which colors are used in chess   ->  White and black.      (c'è la riga)
which color  is  used in chess   ->  I don't understand.   (manca la riga)
quale colore ha legami con gli scacchi -> Non capisco ancora.
```

La conoscenza c'è — `side_color(chess, white)` è nella KB — ma è raggiungibile
solo da quattro frasi inglesi al plurale. **Una variante singolare/plurale
richiede di editare un file.** Il tentativo di insegnarla parlando fallisce:

```
teach "quali colori" as which colors   ->  Hmm, I don't know about colors yet.
```

*Perché è il più urgente:* è il collo di bottiglia di ogni altro giro. Si possono
insegnare fatti nuovi tutto il giorno, ma se non si può insegnare **come si
chiedono**, restano invisibili — ed è esattamente la forma di conoscenza morta
che il M11 nomina («un fatto vero in KB, invisibile al comportamento»). Colpisce
per intero le lingue diverse dall'inglese.

*Atto didattico che deve funzionare:* nominare la relazione, come già si fa per
le famiglie al M11 — «*learn "quali colori" as a way to ask side_color*» — con la
stessa guardia: la relazione deve già esistere, altrimenti la lezione scrive in
un cassetto che nessuno apre. La forma è già disegnata: un modo `frame_for`
accanto a `cue_for`/`reply_for`, che eredita `answer_frame_input_arg` da una
formulazione già esistente della stessa relazione.

*Gate:* una relazione qualunque fra le 136 diventa interrogabile in una lingua
nuova senza toccare un file, e la prova si fa su una relazione che chi implementa
non ha usato per implementare.

#### M16 — La voce di parrot0 è insegnabile solo al 16,5%

*Manca:* le forme italiane. `kb_response_slots` preferisce già
`response_template/3` per la lingua del turno e ricade sulla `/2` inglese — il
meccanismo è a posto. È la copertura a non esserci.

*Misura:* `141` famiglie su `854` hanno una forma italiana. In una sessione
italiana **più di quattro famiglie su cinque rispondono in inglese**, ed è ciò
che F. ha visto:

```
you> come stai
Sto bene, grazie. Come posso aiutarti?
you> genera un template html
I understood the request — produce «template html» — but I don't have …
```

*Nota di metodo, perché è il modo sbagliato più tentante:* la correzione **non**
è un ramo `it ? "…" : "…"` nel C. Al gen450 ne è stato tolto uno; riaggiungerlo
per andare più veloce farebbe regredire l'esperimento anche a test verdi.

#### M17 — Un periodo articolato perde le sue subordinate

*Misura:* baseline su quattro frasi vere, quattro modi diversi di fallire.

| frase | esito | stato |
|---|---|---|
| `water boils …, but it boils lower … because …` | **racconto inventato** | chiuso al gen454 |
| `although mercury is a metal, it is liquid …` | muro, perdeva `metal(mercury)` | chiuso al gen455 |
| `copper is a metal that conducts electricity` | muro, perde `metal(copper)` | **aperto** |
| `if a substance is a noble gas then …` | `holds(it_does_not_react_easily) :- …` | **aperto** |

*Resta aperto:* la **relativa** (`… that conducts …`) non viene scomposta, e con
essa si perde anche la classe che stava in chiaro nella principale. E la
condizionale su classi produce atomi opachi congelati: la regola è vera come
stringa e inapplicabile a `helium`, cioè non è una regola.

*Nota:* la concessiva italiana si scompone (il meccanismo del gen455 è neutro
rispetto alla lingua) ma nessuna delle due metà viene poi capita, perché il
congiuntivo «sia» non ha lettore.

#### M18 — Una lezione può andare a segno e non vedersi

*Era:* il ramo generico di `try_teach_form` componeva la conferma e non la
scriveva mai in `out`. Chi insegnava riceveva la risposta del turno *precedente*:

```
you> ciao
Ciao!
you> learn "sono a pezzi" as a cue for mood_tired
Ciao!                                    <- la lezione era andata a segno
```

*Perché va tenuto scritto anche se è chiuso (gen456):* è il difetto peggiore per
questo piano in particolare. Un protocollo che verifica l'apprendimento
**leggendo le risposte** non può funzionare se l'atto didattico è muto: ogni
lezione sembra fallita, e chi insegna corregge una cosa che non era rotta. La
stessa forma — un messaggio composto e mai emesso — è stata trovata due volte in
due punti diversi (`mod_role` al gen452, qui al gen456), quindi vale come classe
di difetto da cercare, non come incidente.

#### M19 — Moduli che rubano il turno a una lezione

*Misura:* tre casi trovati provando, tutti della stessa forma — un modulo
risponde plausibilmente a un turno che non ha capito, e la lezione è persa.

- `spanish is a romance language` → «*I can translate most of it, but I don't
  know the Spanish for «romance»*». La parola *spanish* fa vincere il traduttore
  su un'asserzione di classe. **Aperto.**
- `what is your designation` → «*I don't have any of my own — I'm parrot0, an
  AI*». Chiuso al gen453 con `self_attribute_request`.
- qualunque frase `X, but it … because …` → un racconto. Chiuso al gen454.

*Gate:* un turno che asserisce un fatto non può essere vinto da un modulo che non
lo asserisce. Finché non c'è, ogni giro di addestramento paga un pedaggio
casuale che dipende dalle parole scelte.

#### M20 — parrot0 mostra la propria forma interna

`what is white` risponde:

```
side_color(chess, white); chess_pawn_direction(white, north); white is a light_color.
```

Non è sbagliato ed è persino informativo, ma è la KB nuda con il punto e virgola.
Per questo piano conta perché **chi insegna non deve dover leggere predicati**:
il vincolo n.1 del `LEARN_PROTOCOL` dice che un esperto del dominio che ignora lo
schema della KB deve poter formulare la lezione — e deve poterne leggere la
verifica. **Aperto.**

### 6.3 Quando uno strato si può dichiarare chiuso

I nove controlli del §3 valgono per la singola lezione. Uno **strato** è chiuso
solo quando quei controlli passano su una lezione che chi ha implementato lo
strato non aveva mai visto: forma nuova, dominio nuovo, e lingua diversa dove ha
senso. Finché il gate è verde soltanto sugli esempi che hanno guidato
l'implementazione, si sta misurando l'implementazione e non la capacità.

Vale anche il verso opposto, e non è negoziabile: uno strato non si chiude
cablando in C il vocabolario che gli serve. La domanda del progetto — «parrot0 può
impararne un nuovo membro domani, senza ricompilare?» — è parte del gate, non un
commento a margine.

### 6.4 Che cosa resta lecito prima del cancello

Il cancello vieta il **consolidamento** su sessione lunga, non la misura. Restano
autorizzati, e sono anzi il lavoro corrente:

- sonde brevi e limitate nel tempo, per fare emergere il gap successivo;
- sessioni di profondità come
  [`depth-session-01.md`](../labs/apprendimento-assistito/depth-session-01.md),
  che raccolgono fallimenti senza promuovere nulla nella KB del repository;
- la coda dei residui, ordinata per livello di astrazione e non per fastidio;
- uno strato alla volta, con il suo test e la sua evidenza.

Non sono autorizzati prima della chiusura: un teacher che gira a lungo scrivendo
nella KB, l'estrazione massiva da dizionari o corpora, e qualunque dichiarazione
di avanzamento basata sul numero di fatti appresi.

## 7. Esperimento di dieci ore

Questo capitolo descrive una sessione che si esegue **dopo** il cancello del §6.
Prima di quel punto il curriculum resta una batteria di misura da eseguire a
sonde brevi, senza consolidare nulla nella KB del repository.

Le dieci ore non sono dieci “generazioni” e non hanno un traguardo cosmetico.
Sono una singola sessione longitudinale: stesso processo, stessa KB di sessione,
curriculum progressivo, checkpoint e prove nascoste.

| Ora | Lavoro del teacher | Gate dell'ora |
|---:|---|---|
| 0 | baseline stratificata: prosa, scacchi, istruzioni, anafora, domande | mappa dei fallimenti e nessuna risposta falsa ignorata |
| 1 | lessico, denotazioni, sinonimi e morfologia | uso/menzione, varianti flesse e retract |
| 2 | domande e slot inversi | stessa relazione interrogabile in entrambi i versi e in parafrasi |
| 3 | asserzione↔domanda e relazioni | verbi nuovi trasferiscono a entità e tempi nuovi |
| 4 | coordinazione, relative, apposizioni | una costruzione insegnata produce più proposizioni corrette |
| 5 | pronomi, ellissi, quantificatori, negazione e scope | riferimenti corretti su paragrafi mai visti |
| 6 | regole, eccezioni, causalità e spiegazioni | conclusione più proof trace, incluso controllo negativo |
| 7 | procedure insegnate | la frase didattica non viene eseguita; il piano trasferisce a numeri nuovi |
| 8 | acquisizione da prosa reale | fatti, relazioni e regole con provenance; domande sul testo |
| 9 | dominio integrato, iniziando dagli scacchi | posizioni nuove risolte dalle regole, non da risposte memorizzate |
| 10 | replay ostile, ablation e consolidamento | checkpoint riproducibile e genealogia delle capacità |

Almeno il 70% delle verifiche deve essere held-out: numeri, nomi, ordine delle
frasi e formulazioni non usati dal teacher. Ogni ora conserva anche le lezioni
fallite; sono l'inventario dei motori ancora mancanti.

### Che cosa produrrebbero davvero dieci ore oggi

Con i meccanismi attuali, dieci ore potrebbero accumulare molti fatti, regole
nel registro già noto, nuovi membri lessicali, intenti e risposte. Sarebbe una KB
più capace e utile. Non diventerebbe però equivalente a un LLM, perché i
fallimenti fuori dal metalinguaggio noto resterebbero non compilabili; il teacher
sarebbe costretto a tradurre tutto nelle poche forme già supportate o a creare
frasario.

La sessione lunga diventa l'acceleratore decisivo **dopo** che una spiegazione di
costruzione, denotazione e procedura può creare un candidato verificabile. Prima
di quel gate, aumentarne la durata misura soprattutto la copertura delle forme
esistenti.

## 8. Il teacher come processo lungo

Il teacher non deve attendere che qualcuno scelga dieci prompt a mano. Il lavoro
continuo autorizzato da questo piano è:

- campionare prosa e dialoghi ammessi dalle fonti del progetto;
- presentare un testo a parrot0 e porre domande di comprensione;
- classificare ogni risposta in corretta, muro, irrilevante o falsa;
- tentare una lezione parlata, mai una patch nascosta;
- generare transfer e controesempi;
- consolidare soltanto ciò che supera i gate;
- mettere i residui in una coda ordinata per livello di astrazione;
- preferire il gap che, chiuso, libera più famiglie di frasi;
- fermare una pista che accumula risposte esatte senza transfer.

Un grande dizionario può fornire esempi, definizioni, flessioni e contesti. Non
deve diventare il surrogato della comprensione. L'estrazione massiva comincia
solo quando un campione di forme supera replay, transfer e controllo di qualità;
altrimenti moltiplica errori come `organism(metabolism)` osservato nella prima
sessione.

## 9. Metriche che sostituiscono il conteggio delle generazioni

- **Lesson yield:** lezioni consolidate / lezioni tentate.
- **Transfer@N:** successo su N esempi nuovi per ogni lezione.
- **Paraphrase invariance:** quota di formulazioni equivalenti con stessa
  lettura.
- **Contrast precision:** quota di quasi-esempi correttamente esclusi.
- **Ablation fidelity:** quota di capacità che scompare ritraendo solo la sua
  causa.
- **Retention:** capacità ancora attiva dopo 100, 1.000 e 10.000 turni.
- **Addressability:** fallimenti per cui il teacher può formulare una lezione che
  parrot0 sa almeno rappresentare come candidato.
- **False-understanding rate:** “appreso/capito” senza prova di uso; obiettivo 0.
- **Route stability:** stesso testo acquisito indipendentemente dal modulo che
  tenta di reclamarlo.
- **Inference depth:** conclusioni nuove ottenute per composizione, con proof
  trace, non presenti come fatti.
- **Compression:** quante famiglie held-out copre una sola lezione; è la misura
  diretta dell'astrazione.

Nessun numero di generazione chiude la missione. Un milestone passa soltanto se
una batteria nascosta cresce in capacità e non in frasi memorizzate.

## 10. Tracce che non si cancellano

Ogni episodio conserva:

- testo originale e risposta originale;
- ipotesi semantiche provate, anche quelle perdenti;
- spiegazione del teacher;
- candidato KB e sua provenienza;
- replay, transfer, parafrasi, controesempi e ablation;
- motivo della promozione, del rollback o del rinvio;
- dipendenze fra la lezione e le capacità nate dopo.

Le strutture secondarie non vengono eliminate quando una strada migliore
emerge. Si marcano come `superseded`, `failed` o `partial` e restano consultabili.
Un tentativo incompleto che alza il livello di astrazione è materiale di ricerca,
non spazzatura.

## 11. Ogni incremento è committabile

**Regola.** La conoscenza e le regole che parrot0 apprende durante una sessione
si committano e si pushano **sempre**, anche parziali, anche incomplete. Non
esiste il contributo compatto: non è ammesso tenere l'apprendimento in staging
finché la sessione non è completa.

Il principio è quello dei piccoli incrementi. **Ogni cosa appresa è un piccolo
incremento**, e in qualunque momento è legittimo committare e pushare un piccolo
avanzamento della KB. Non serve che chiuda uno strato, una milestone o un'ora di
curriculum. Non serve nemmeno che sia elegante.

Questo **non** significa commit compulsivo. Non c'è un obbligo di frequenza e non
c'è un merito nel numero di commit. Significa soltanto togliere il permesso
implicito che si dava all'attesa: nessun avanzamento resta fuori dal repository
perché «è ancora poco», «lo committo quando ho finito il resto» o «da solo non si
capisce». Un incremento parziale nel repository vale più di un incremento
completo che vive soltanto nella sessione di chi lo ha prodotto.

Le ragioni sono le stesse del §10:

- una lezione non committata è una traccia persa, e le tracce non si cancellano;
- un incremento visibile può essere contraddetto, superato o ritratto da
  chiunque; uno invisibile no;
- la genealogia delle capacità si costruisce dai passi, non dal risultato;
- un lotto grande nasconde quale lezione ha prodotto quale capacità, che è
  esattamente ciò che il gate di provenienza del §3 chiede di sapere.

Conseguenze operative:

- ciò che è parziale si dichiara parziale nel messaggio di commit, non si
  trattiene fino a diventare completo;
- una lezione che ha fallito i gate si committa come `failed` o `partial` con la
  sua evidenza, coerentemente con il §10: è inventario, non spazzatura;
- una suite rossa preesistente non è motivo per trattenere un incremento
  indipendente; si dichiara il rosso, non lo si usa come lucchetto;
- il messaggio di commit dice che cosa parrot0 ha imparato e che cosa non ha
  ancora imparato, non soltanto quali file sono cambiati.

E una precisazione che vale quanto la regola: **committare tutto non autorizza a
filtrare il resto.** Una sessione salva anche fatti che non parlano del mondo —
il registro della conversazione, la provenienza delle letture, i gap aperti, i
contatori. La tentazione è chiamarli rumore e toglierli automaticamente al
salvataggio; è vietato, perché ciò che oggi sembra rumore può essere conoscenza
di ordine superiore, e un filtro la distrugge prima che qualcuno capisca a che
cosa serviva. La questione è aperta e sta in
[`session-and-provenance.md`](../session-and-provenance.md#6-il-rumore-di-sessione--questione-aperta-da-non-chiudere-con-un-filtro):
finché è aperta, ciò che sembra fuori posto si annota, non si scarta.

## 12. Milestone

### A0 — Misura iniziale (completato)

- sessione isolata teacher→parrot0;
- successo forte su classi lessicali, relazioni, regole e intenti;
- fallimenti riproducibili su equivalenza operativa, procedure, prosa integrata,
  parafrasi e scacchi situazionali.

### A1 — “Questo significa questo”

Una lezione parlata deve poter creare una trasformazione semantica candidata da
una forma con slot a un frame noto. Gate minimo:

> `X glints Y` significa `X glorphs Y`

seguito da replay, tre coppie held-out, una parafrasi, un controesempio e retract.
La regola deve conservare ruoli e provenienza; non può essere un alias di stringa
globale.

### A2 — Domande duali e morfologia

Da un verbo relazionale insegnato devono emergere forma dichiarativa, domanda sul
soggetto, domanda sull'oggetto e varianti flesse. `studies`/`study` non possono
diventare due abilità scollegate.

### A3 — Un solo atto di lettura

Qualunque input dichiarativo multi-frase deve essere offerto allo stesso lettore
prima dei generatori di racconto o analisi. Se non estrae nulla, deve riportare
le frasi saltate e i gap, non produrre narrativa. `read:` non può essere rubato
da un altro intento nel profilo integrato.

*Stato 2026-08-29:* il routing e' chiuso da D0/SC0; SC1-A conserva inoltre le
unita' oltre il workspace transiente e deriva un primo arco retorico retraibile.
SC2-A aggiunge identita' source-addressed, claim di superficie, span,
attribuzione e status/context/commitment derivati da cue multi-parola
insegnabili. A3 non equivale ancora a comprensione proposizionale: il prossimo
confine e' normalizzare il contenuto con la pipeline semantica comune e renderlo
interrogabile naturalmente senza promuovere il riportato a fatto del mondo.

### A4 — Procedure insegnabili senza misclaim

Una frase che insegna una procedura deve entrare in quarantena, mai essere
eseguita come domanda. Il piano risultante deve legare i numeri a ruoli e
trasferire a input nuovi.

### A5 — Scacchi come dominio di integrazione

Non una tabella di domande sugli scacchi: rappresentazione di pezzi, mosse,
occupazione, traiettorie, attacco, stato del re ed eccezioni. Il gate usa
posizioni generate dopo le lezioni e richiede spiegazione della legalità.

### A6 — Sessione di dieci ore

Si esegue il curriculum completo solo dopo la chiusura degli strati M0–M14 del
§6, di cui A1–A5 sono le milestone visibili. Le milestone non bastano da sole: un
gate di milestone verde sugli esempi che l'hanno guidata non chiude lo strato
corrispondente (§6.3). Il risultato è un checkpoint della KB con genealogia, non
una promozione opaca di tutto ciò che è stato detto.

## 13. Criterio di missione

Non si può essere sicuri in anticipo di eguagliare un LLM. Si può però evitare di
auto-convincersi con traguardi minimi: la missione resta falsificabile e ogni
milestone deve aumentare l'insieme delle strutture che parrot0 può apprendere
domani parlando, senza ricompilare.

Il segnale che stiamo davvero convergendo non sarà “conosce più risposte”, ma:

> davanti a un fallimento nuovo, parrot0 sa indicare il tipo di lacuna, ricevere
> una spiegazione, trasformarla in una capacità generale, provarla su casi nuovi
> e conservarla senza che il teacher tocchi il motore.

Finché questo ciclo non vale per costruzioni e procedure, la comprensione
universale è la missione aperta, non un risultato già dichiarabile.
