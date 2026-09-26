# L4 — la coerenza dell'apprendimento con la comprensione

**Piano di indirizzo e progettazione operativa, 26 settembre 2026. Stato: aperto,
niente implementato.** Nasce dalla «domanda delle domande» di F. alla fine del
giro di grammatica:

> *«il meccanismo di apprendimento di parrot0 è consistente con la sua crescita,
> cioè ciò che impara dalla grammatica si innesta nel set di regole che parrot0
> usa per operare la comprensione?»*

La risposta misurata è stata **no, non in generale**: l'apprendimento è coerente
quando innesta un MEMBRO in una regola che esiste (un plurale, un lemma, una
contrazione, un verbo per contatto); non lo è quando la lezione riguarda la
REGOLA stessa, né quando il membro entra senza le condizioni della regola. Poi F.:

> *«ma nominarle abbiamo visto con L3 che non è un requisito essenziale; il
> derivato della mia domanda deve dare il via a l4-upgrade.md, cioè la coerenza
> dell'apprendimento a tutto campo dentro i meccanismi di comprensione»*

Prosegue [l3-upgrade.md](l3-upgrade.md) (il contatto come canale, §12 «imparare
una lettura attraverso le sue conseguenze») e [l2-upgrade.md](l2-upgrade.md) (gli
schemi, che restano il livello di sotto). Si appoggia sul contesto come rete di
percorsi di [frontier-kb-natural-dialogue.md §19](frontier-kb-natural-dialogue.md),
sul [MANTRA](../../MANTRA.md) (#2, #17, #18, #19, #20) e sulle tre sessioni del 26
settembre, i cui fallimenti sono il banco di partenza
([train-the-learning-process.md](train-the-learning-process.md), sezione
PRIORITARIO).

> **In una frase.** L1–L3 hanno reso possibile *imparare* (per fatto, per schema,
> per contatto). L4 rende l'apprendimento **coerente con ciò che parrot0 fa per
> capire**: ogni cosa imparata — fatto, lettura, regola, procedura — deve entrare
> nella **stessa rete di inferenza** che la comprensione percorre, nel punto in
> cui cambia qualcosa e con le condizioni sotto cui vale; e chi impara trova quel
> punto **facendo girare la propria comprensione sulle conseguenze della
> lezione**, non attraverso un nome.

> **Niente nomi obbligatori (F.).** Nominare una regola può essere un *risultato*
> di L4, mai un *requisito*. L3 ha mostrato che «hails from» diventa un verbo di
> `born_in` senza che nessuno nomini niente: il punto d'innesto si trova per
> contatto con ciò che la comprensione già fa. L4 generalizza quel gesto a tutte
> le lezioni.

---

## HANDOFF — da dove si comincia

Primo passo: **§6, L4-0 — il banco di coerenza**, perché senza la misura ogni
passo successivo si giudica a occhio. Poi L4-1 (riconoscere la sovrapposizione)
sulle otto regole grammaticali che parrot0 applica già e che la lezione non
riconosce (G2, G3, G4, G19, G21, G22, G23, G1-parziale). Il banco è già scritto
in forma di prova nelle tre sessioni del 26 settembre: vanno promossi da
scratchpad a strumento. La contraddizione (§5-bis, L4-8) corre in parallelo:
il suo primo passo, depositare la specie nel registro dei paradossi, è piccolo e
non dipende dal banco.

---

## 1. Che cosa si è misurato: le rotture di coerenza

Tre sessioni live del 26 settembre (debug PHP, meccanica di precisione,
grammatica inglese), tutte con conoscenza vera. Il dettaglio è in
[train-the-learning-process.md](train-the-learning-process.md); qui la
classificazione che fonda il piano.

| # | rottura | caso misurato | che cosa dice sulla coerenza |
|---|---|---|---|
| R1 | **la lezione su una regola diventa un fatto sul mondo** | «To make a question with other verbs, put do, does or did before the subject…» → `Learned: put do does.`; «Short adjectives make the comparative with er…» → `short adjectives make comparative`, che poi risponde a «What needs grease?» | ciò che doveva modificare la comprensione finisce nel deposito dei fatti, dove la comprensione non lo usa come regola e dove contamina altre domande |
| R2 | **la sovrapposizione non si vede** | otto regole grammaticali già operanti (do-support, modali, negazione, passivo, who sul soggetto, aggettivo attributivo, ordine SVO): nessuna lezione risponde «già lo faccio» | chi impara non si confronta con ciò che sa fare: non conferma, non estende, duplica o sporca |
| R3 | **il membro entra senza le condizioni della regola** | dopo «so it gauges thickness» ogni «gauge» è *measures* anche in «gauge blocks», «bore gauge»; «lead screw» diventa *causes* | l'innesto avviene per identità della parola, non nel punto della rete (ruolo del nodo, sintagma, forma di nascita) in cui vale |
| R4 | **la regola che la comprensione usa sta nel C** | l'articolo a/an scelto per LETTERA (`p0_indef_article` passa una lettera alla KB: «an universal quantifier»); guardie di sintagma e cancello dei concetti in parte compilati | la lezione giusta («universal starts with a consonant sound») non ha dove innestarsi |
| R5 | **due depositi della stessa cosa** | «print_r prints a readable view…»: la KB tiene `readable_view`, il deposito semantico parallelo `readable`, e la domanda risponde dal secondo | la stessa lezione produce due verità, e quale si usa dipende dal lettore |
| R6 | **l'innesto non sopravvive o non si raggiunge** | «Is a lathe a machine tool?» risponde in sessione e non dopo `/save` e riavvio; i fatti salvati sparsi per predicato in file estranei (una fresa in `physiology.p0`) | l'apprendimento vale nel processo che l'ha fatto, non nella KB che ne resta |
| R7 | **la procedura imparata non entra nelle altre vie** | «rule for X is multiply by 25.4» si esegue con «apply X to 2», ma non dentro un'altra procedura (il passo `apply` saltato), non all'inverso («What number gives 50.8…?» dirottata), non in lingua («How many millimeters are 3 inches?») | una procedura imparata è un'isola: la comprensione non la attraversa |
| R8 | **una lezione capita male resta** | «rule for rpm takes 2 inputs» → `Noted: The trip takes 2 hours.`; il controesempio «…but the cutting tool is not its product» → `not(rotate(spindle, cutting_tool))` | ciò che non si è capito non deve innestarsi: oggi diventa un fatto, a volte falso |

## 2. La tesi

La coerenza non è una proprietà di un modulo: è una proprietà del **rapporto** fra
ciò che si impara e ciò che si usa per capire. Si definisce così:

> **Una lezione è coerente quando il suo effetto (a) è prodotto dalla stessa rete
> di inferenza che la comprensione percorre, (b) cambia soltanto ciò che la
> lezione giustifica, (c) vale dove la lezione vale e non altrove, (d) resta dopo
> il riavvio, e (e) quando la comprensione ne era già capace, lo riconosce invece
> di duplicarlo.**

Da qui segue il principio operativo di L4, il seguito diretto del circuito di L3
§12.3:

> **Il punto d'innesto si trova per conseguenza.** Una lezione produce delle
> conseguenze osservabili (un esempio, una domanda che dovrebbe cambiare
> risposta, una lettura che dovrebbe cambiare). Chi impara fa girare la propria
> comprensione su quelle conseguenze **prima** di scrivere: se la comprensione le
> produce già, la lezione è una conferma; se le produce in parte, il punto dove il
> percorso si interrompe è il punto d'innesto; se non le produce e non c'è un
> punto riconoscibile, la lezione resta un'ipotesi e si chiede un esempio.

Nessun nome è richiesto: il punto d'innesto è **un percorso** della rete (una
derivazione, una lettura della IR, un passo di procedura), trovato dalla
comprensione stessa. È il «contesto a rete» di F. applicato all'apprendimento:
imparare non è aggiungere un valore a un insieme, è modificare un percorso in un
punto e con le condizioni del punto.

## 3. Le sette proprietà di coerenza (C1–C7)

| | proprietà | si verifica con | rompe se |
|---|---|---|---|
| **C1** | **un solo substrato** — ciò che si impara sta dove la comprensione legge | la stessa lezione è usata da un lettore diverso da quello che l'ha scritta | esistono due depositi (R5) o la lezione finisce in un deposito che nessun lettore consulta come regola (R1) |
| **C2** | **innesto per conseguenza** — la lezione trova il suo punto facendo girare la comprensione | il punto d'innesto è il percorso che la lezione cambia; si mostra con la traccia (`/debug trace`) e con `kb_derivation/4` | la lezione si scrive senza aver provato le sue conseguenze (R1, R8) |
| **C3** | **sovrapposizione riconosciuta** — se la comprensione già lo fa, la lezione conferma | la risposta alla lezione dice «già lo faccio» e mostra l'esempio che lo prova | duplicazione, fatto spazzatura, silenzio (R2) |
| **C4** | **condizioni ereditate** — il membro imparato porta le condizioni del percorso in cui è entrato | la stessa parola in un altro ruolo non viene letta con la lezione | innesto per identità (R3) |
| **C5** | **raggiungibilità** — ogni decisione della comprensione è conoscenza che una lezione può toccare | la lezione cambia la decisione, senza ricompilare | la decisione è nel C (R4) |
| **C6** | **persistenza e confine** — l'effetto resta dopo il riavvio e non tocca ciò che la lezione non giustifica | stessa risposta dopo `/save` e riavvio; il contrasto resta com'era | l'effetto svanisce (R6) o contamina (R1, R3, R8) |
| **C7** | **la contraddizione è uno stato, non un guasto** — quando ciò che si impara urta ciò che la comprensione sostiene, l'urto entra nel registro dei paradossi e diventa uno stato dialettico che parrot0 maneggia nel discorso e nelle inferenze (§5-bis) | la contraddizione si chiede («what contradicts what?»), si dice con le due parti e la ragione, e ha un esito dichiarato | la lezione sovrascrive in silenzio, o viene scartata in silenzio, o le due verità convivono senza che nessuno lo sappia (R5, R8) |

La procedura (R7) non è una settima proprietà: è C1+C2 applicate a ciò che si
esegue. Una procedura imparata è coerente se la comprensione la attraversa da
tutte le parti da cui attraversa le procedure che già sa: dentro un'altra
procedura, all'inverso quando l'inverso è conoscenza, in lingua quando la domanda
lo chiede.

## 4. Che cosa L4 non è

- **Non è L3 con i nomi.** Nessuna lezione deve nominare una regola perché
  l'innesto avvenga; il nome, se c'è, è un'altra faccia dello stesso percorso.
- **Non è un lettore di regole grammaticali descritte.** Una forma che legga «to
  make a question, put the verb before the subject» e scriva una clausola sarebbe
  L2 in un registro nuovo (il primo passo falso di L3 §1-bis). La descrizione
  interessa L4 per le **conseguenze** che produce: un esempio, una domanda che
  dovrebbe riuscire.
- **Non è un sistema di test.** Il banco di coerenza misura parrot0 sulla KB viva,
  con conoscenza vera; non si inventano entità, non si amputa la KB, non si alzano
  i tempi.
- **Non è un secondo deposito per ciò che si impara.** Ciò che si impara entra
  dove la comprensione legge; se oggi ci sono due depositi, L4 li riduce a uno.

## 5. Il circuito di L4, passo per passo

Riprende i sette passi di L3 §12.3 e ne aggiunge due, prima di scrivere e dopo
avere scritto.

1. **Osservare la lezione come turno ordinario.** Nessun registro speciale: la
   lezione è un'affermazione, una correzione, un esempio, una descrizione.
2. **Ricavarne le conseguenze.** Che cosa dovrebbe cambiare se la lezione fosse
   vera? Un'affermazione ha per conseguenza le domande che dovrebbero rispondere;
   una correzione, la lettura che dovrebbe cambiare; un esempio, se stesso; una
   descrizione di regola, gli esempi che la descrizione genera — e se parrot0 non
   sa generarne, **chiede un esempio** («For example?»), in lingua ordinaria.
3. **Far girare la comprensione sulle conseguenze, PRIMA di scrivere.** È il
   passo che oggi manca e da cui dipendono R1, R2, R8. Tre esiti:
   - **già prodotte** → sovrapposizione: si conferma («I already read it that way:
     …» con l'esempio), si registra l'episodio come sostegno, niente di nuovo si
     scrive (C3);
   - **prodotte in parte** → il percorso si interrompe in un punto: quello è il
     punto d'innesto, e la lezione diventa un delta su quel punto (C2);
   - **non prodotte, nessun punto** → la lezione resta un'ipotesi inerte con la
     sua richiesta di esempio; **niente entra fra i fatti** (R8).
4. **Proporre il delta minimo nel punto trovato** (L3 §12.3 passo 4): un membro in
   una classe che il percorso consulta, una condizione in più, una clausola in più
   del predicato che si è interrotto, un passo di procedura.
5. **Portare con il delta le condizioni del punto** (C4): il ruolo del nodo, la
   forma di nascita, il contesto in cui la lezione è nata (`holds_in`), come in
   [frontier §19](frontier-kb-natural-dialogue.md).
6. **Rifare girare le conseguenze e un contrasto.** Il delta è accettato solo se
   le conseguenze ora riescono e il contrasto (un caso vicino dove la lezione non
   deve valere) resta com'era.
7. **Usare, chiedere o sospendere** (L3 §12.3 passo 6).
8. **Consolidare dove la comprensione legge** (C1, C6): lo stesso deposito, lo
   stesso file accanto ai suoi simili, riletto dopo il riavvio.
9. **Revisionare nel tempo** (L3 §19): uno strato che ritira, non una cancellazione.

## 5-bis. La contraddizione entra nel registro dei paradossi (F., 26 settembre 2026)

> *«nel caso di contraddizione il predicato di contraddizione deve essere messo
> in campo come per i paradossi, per i loop, per i cap di soglia: anche per le
> contraddizioni ci sarà un processo che rende quello stato, ingestibile con la
> coerenza logica, uno stato dialettico gestibile.»*

**Il precedente.** Il motore ha già un registro unico dei paradossi,
`paradox_event(Livello, Specie, Dove, Dettaglio)` (debug.p0, §25.3 di L3). Il
ciclo tagliato, il budget, il tetto di profondità, il cortocircuito e il ciclo di
una vista sono **specie** di quel registro. La KB le legge
(`inference_incomplete/2`, `inference_cycle/2` in composition.p0), e parrot0 ne
parla invece di fermarsi. Il tetto di profondità ci è entrato il 26 settembre con
la stessa richiesta di F.: *«quando li raggiunge fa inferenza con essi e te ne
parla»*. La contraddizione è la specie che manca.

**Perché è la stessa famiglia.** Un ciclo è una definizione che per chiudersi
consulta se stessa. Un tetto è una ricerca che non finisce nel suo bilancio. Una
contraddizione è una conclusione che, con i suoi soli mezzi, la logica non può
tenere: sia P che non-P, o due membri di classi incompatibili, o una regola e il
suo controesempio. In tutti i casi la logica classica ha due sole uscite
sbagliate: fermarsi, oppure derivare qualsiasi cosa (*ex falso*). Il registro dà
la terza uscita, la stessa per tutte: **lo stato si deposita, diventa
conoscenza, e la conoscenza si maneggia.**

**Che cosa c'è già, sparso.** Oggi i pezzi esistono ma non si parlano:

| dove | che cosa rileva | che cosa manca |
|---|---|---|
| `contradiction/1` (procedures.p0, L12) | un'entità in due classi `incompatible/2` | nessuno lo consulta durante l'apprendimento |
| `incompatible_propositions/2`, `contradicts_across/4` (context-scope.p0) | una proposizione e la sua negazione in due contesti | vale fra contesti, non fra la lezione e la KB |
| `episode_contradicted`, `precedent_contradicted` (episodes.p0) | un episodio smentito | nessun esito dialettico |
| `precondition_contradicted` (situation.p0) | una precondizione smentita | idem |
| `own_method(contradiction)` e `own_method(non_contradiction)` (own-methods.p0) | parrot0 **dice** già il metodo giusto: non sovrascrivere, tenere le due viste in contesti separati; due affermazioni vere che sembrano contraddirsi riguardano sensi, tempi o rispetti diversi, e nominare quale è tutto il lavoro | il metodo è **detto**, non **eseguito** |

L4 chiede di chiudere quest'ultima distanza. Il metodo che parrot0 descrive
diventa il processo che fa.

**Il processo, dall'urto allo stato dialettico.**

1. **Rilevare.** Al passo 3 del circuito (§5), quando le conseguenze della
   lezione si fanno girare, una conseguenza può urtare ciò che la comprensione
   sostiene. Le regole che rilevano sono le relazioni di incompatibilità già in
   KB, più le nuove che si insegnano. Nessuna lista nel C. Il motore dà solo la
   primitiva che tiene insieme le due derivazioni.
2. **Registrare.** L'urto diventa un fatto nel registro:
   `paradox_event(belief, contradiction, Dove, pair(Tesi, Antitesi))`. `Dove` è
   il punto della rete in cui le due derivazioni si incontrano, e ciascuna parte
   porta la sua derivazione (`kb_derivation/4`) e la sua fonte (lezione,
   contatto, KB di base, turno). `/debug` lo mostra con la sonda 44, come le altre
   specie.
3. **Tenere, non scegliere in silenzio.** Nessuna delle due parti viene
   cancellata. Nessuna vince per ordine di arrivo. Lo stato vale finché non ha un
   esito (L3 §19: il ritiro è uno strato, mai una `retract`).
4. **Cercare la distinzione.** Il metodo già detto da `own_method(non_contradiction)`
   diventa una ricerca. Le due parti differiscono per **senso** (due letture della
   stessa parola), **tempo** (`holds_in`, il qualificatore di tempo), **rispetto**
   o **contesto** (il contesto a rete di frontier §19, `context_effective_belief`),
   o **portata** (una regola generale e il suo caso particolare, come l'articolo
   per lettera e «universal»)? Ogni dimensione è una relazione in KB, e se ne può
   insegnare una nuova.
5. **Dare un esito dichiarato**, uno fra cinque:
   - **distinzione trovata:** le due parti convivono, ciascuna con la sua
     condizione; è l'esito più forte, perché la condizione diventa conoscenza;
   - **eccezione:** la parte particolare restringe la generale (è anche lo
     schema di R4: la lezione giusta sull'articolo è un'eccezione alla regola per
     lettera, finché l'esempio non mostra che la regola va riscritta per suono);
   - **revisione:** una parte si ritira con uno strato (L3 §19), con la ragione e
     la fonte;
   - **domanda:** manca ciò che decide; parrot0 lo chiede in una sola domanda
     che nomina le due parti («You told me X, but I hold Y because Z. Which holds
     here, or in what sense are both true?»);
   - **sospensione:** lo stato resta aperto e dichiarato; le risposte che ne
     dipendono lo dicono, invece di fingere una certezza.
6. **Parlarne e inferire con esso.** Lo stato dialettico è conoscenza come le
   altre. Si interroga («Is there a contradiction in what you know about X?»,
   «Why do you doubt X?»). Entra nelle risposte che toccano le due parti. E può
   essere a sua volta premessa: «due fonti si contraddicono su X» è un fatto da
   cui si ragiona sulla fiducia nelle fonti.

**Che cosa non è.** Non è una guardia che rifiuta la lezione. Non è un halt. Non
è un contatore nel C, e non è una lista di coppie incompatibili nel C. È la
consapevolezza di un paradosso dentro l'inferenza, come per
la guardia anti-isteresi: il motore offre la primitiva e il registro, la KB
decide che cosa è incompatibile, quali distinzioni cercare, e come dirlo.

**Dove tocca le rotture del §1.** R5 (due depositi, due verità) è una
contraddizione che oggi nessuno vede: con C7 diventa uno stato dichiarato. R8 (il
controesempio letto come negazione di un fatto vero) è una contraddizione
creata dall'apprendimento: il registro la rende visibile prima che entri. R4 (la
regola per lettera contro la lezione per suono) è il caso di scuola
dell'eccezione. R3 (la contaminazione) è una contraddizione di lettura: «gauge»
verbo contro «gauge» nome nello stesso nodo, e il pareggio vero diventa una
domanda.

## 6. Le fasi

Ogni fase è tirata da fallimenti misurati del 26 settembre; nessuna scrive C che
contenga lingua (mantra #2, #16, #17). Il C resta IR, unificazione, enumerazione,
derivazione e le primitive che servono a eseguire.

### L4-0 — Il banco di coerenza

**Tirato da:** tutto il §1. **Che cosa:** lo strumento che per ogni lezione misura
le cinque cose della definizione del §2, sulla KB viva, dentro una sessione di
[live-teaching](live-teaching.md):

| misura | come |
|---|---|
| **prima** | la conseguenza della lezione, chiesta prima di dirla (sovrapposizione?) |
| **dopo** | la stessa conseguenza con una frase nuova della stessa regola |
| **trasferimento** | un caso tenuto fuori, mai nominato nella lezione |
| **contrasto** | un caso vicino dove la lezione NON deve cambiare niente |
| **riavvio** | la conseguenza dopo `/save` e un processo nuovo |

Esce un **coefficiente di coerenza** per lezione e per forma, accanto a quello
«imparate / con problemi» già usato. Il banco nasce dai tre piloti della sessione
del 26 settembre (sessione, procedure, differenziale di grammatica), promossi da
scratchpad a `scripts/` con le batterie vere: G1–G25, le 100 della meccanica,
P1–P14. **Gate:** il banco riproduce i numeri del 26 settembre sullo stesso
binario (131/100; 0/25 regole lette; 8 sovrapposizioni non riconosciute).

### L4-1 — Riconoscere la sovrapposizione (C3)

**Tirato da:** R2 (G2, G3, G4, G19, G21, G22, G23), R1 (i fatti spazzatura che
quelle lezioni producono). **Che cosa:** il passo 3 del circuito per le lezioni
che descrivono qualcosa che la comprensione fa già. La descrizione genera un
esempio (o lo chiede); l'esempio si fa leggere; se la lettura riesce, la risposta
è una conferma con l'esempio e l'episodio diventa un sostegno. **Gate:** sulle
otto regole, la lezione risponde confermando e mostrando l'esempio; nessun fatto
nuovo nella KB (`kb_turn_act` vuoto per i predicati del mondo); nessuna risposta
estranea contaminata («What needs grease?» non risponde più «short adjectives»).

### L4-2 — Il punto d'innesto per conseguenza (C2)

**Tirato da:** R1, G10–G14, G24–G25 (regole che mancano), R7 (procedure). **Che
cosa:** quando la conseguenza fallisce, trovare **dove** il percorso si
interrompe: la derivazione parziale (`kb_derivation/4`), la lettura della IR, il
passo di procedura che non si applica. Il delta minimo si propone lì. Per le
regole descritte senza esempio, chiedere l'esempio; con l'esempio, il punto
d'innesto è il primo passo della lettura che non lo regge (un plurale in -ies che
nessuna regola genera, un comparativo senza clausola). **Gate:** almeno tre regole
grammaticali mancanti (una morfologica, una sintattica, una di accordo) diventano
operative dopo una lezione in lingua ordinaria con un esempio, trasferiscono su un
caso tenuto fuori e lasciano intatto il contrasto; nessuna riga di C che nomini
una parola o una regola.

### L4-3 — Le condizioni viaggiano con il membro (C4)

**Tirato da:** R3. **Che cosa:** la lettura per occorrenza proposta in
[frontier §19](frontier-kb-natural-dialogue.md): un'ipotesi (del contatto, di una
lezione di classe) si applica a un nodo solo se il percorso di ruolo che l'ha fatta
nascere regge per quel nodo; altrimenti le letture restano concorrenti, con il
candidato «nessun cambiamento». **Gate:** quello del §19 — «gauges» verbo dove ha
la forma di nascita, nome in «gauge blocks», «bore gauge», «lead screw»; un
pareggio vero diventa una domanda.

### L4-4 — Un solo substrato, persistente (C1, C6)

**Tirato da:** R5, R6. **Che cosa:** (a) il deposito semantico parallelo
(`semantic_binding`, `semantic_proposition`) non può divergere dai fatti: è una
vista dei fatti o ne è la sorgente, non un secondo archivio; (b) `/save` mette ciò
che si impara accanto ai suoi simili per materia (la regola di F., «per
similarità, non per provenienza»), e ciò che è stato salvato si raggiunge dopo il
riavvio per gli stessi percorsi. **Gate:** una batteria di lezioni della sessione
meccanica risponde uguale prima e dopo il riavvio; nessun fatto finisce in un
file di un'altra materia.

### L4-5 — Le decisioni della comprensione diventano raggiungibili (C5)

**Tirato da:** R4. **Che cosa:** l'inventario delle decisioni della comprensione
ancora compilate che una lezione ordinaria dovrebbe poter cambiare, ordinate per
danno misurato: l'articolo per lettera (G9), le guardie di confine del sintagma e
del soggetto, il cancello dei concetti, il riconoscimento delle forme di lezione.
Ognuna passa in KB solo quando una lezione la reclama (mantra #18: il C deve
accorciarsi). **Gate:** «universal starts with a consonant sound», detto con un
esempio, cambia «an universal» in «a universal» e non tocca «an umbrella».

### L4-6 — La procedura imparata è attraversabile (C1+C2 sulle procedure)

**Tirato da:** R7 (33 verifiche indirette e 38 inverse fallite nella sessione
meccanica). **Che cosa:** una procedura imparata entra nelle vie da cui la
comprensione attraversa le procedure: (a) dentro un'altra procedura (il passo
`apply` che oggi si salta); (b) all'inverso, quando l'inverso dei suoi passi è
conoscenza («divide is the inverse of multiply» come fatto, da cui la procedura
inversa si deriva, non si scrive); (c) in lingua («How many millimeters are 3
inches?» raggiunge la procedura o la misura detta). **Gate:** sulle procedure
della sessione meccanica, le tre verifiche passano senza procedure nuove scritte a
mano.

### L4-7 — Ciò che non si è capito non si innesta (C2, C6)

**Tirato da:** R8, R1. **Che cosa:** il passo 3 del circuito come cancello: una
lezione le cui conseguenze non si possono formare non diventa un fatto, resta
un'ipotesi con una domanda. Vale per le lezioni in forma di regola, per i
controesempi letti come negazioni di un fatto vero, per le frasi dirottate da un
altro lettore. **Gate:** sulle 25 lezioni di grammatica e sulle 100 della
meccanica, nessun fatto spazzatura entra; il numero di «lezioni non lette» si
trasforma in domande di esempio, non in fatti.

### L4-8 — La contraddizione come specie del registro (C7)

**Tirato da:** la richiesta di F. del 26 settembre, e da R3, R4, R5, R8 letti
come contraddizioni. **Che cosa:** il processo del §5-bis. Il motore deposita
`paradox_event(belief, contradiction, …)` quando due derivazioni incompatibili si
incontrano, e aggiunge solo quella primitiva. La KB rileva l'incompatibilità con
le relazioni che ha già, cerca la distinzione e sceglie l'esito, poi dice lo
stato. I pezzi sparsi (`contradiction/1`, `contradicts_across/4`,
`episode_contradicted`, `precondition_contradicted`) diventano facce dello
stesso registro, come è successo al ciclo, al budget e al tetto. Non si
cancellano: restano come strutture secondarie finché la selezione non decide.
**Gate:**
- una lezione che urta un fatto di base («A whale is a fish») produce lo stato
  con le due parti e la loro fonte, non sovrascrive e non tace;
- una lezione che si distingue per tempo o per senso («The capital of Germany
  was Bonn») trova la distinzione, e le due parti rispondono ciascuna nella sua
  condizione;
- «an universal» contro la lezione per suono dà un'eccezione dichiarata, e poi
  la revisione quando l'esempio lo mostra;
- la contraddizione si chiede in lingua e si vede con `/debug`;
- nessuna parola e nessuna coppia incompatibile nel C.

## 7. Il criterio d'esito

L4 è chiuso quando, sul banco L4-0 e sulle tre batterie del 26 settembre:

1. **ogni lezione ha un esito dichiarato** fra conferma, innesto, ipotesi aperta
   o rifiuto motivato — nessuna diventa un fatto che la comprensione non usa;
2. **il coefficiente di coerenza** (lezioni che passano prima-dopo-trasferimento-
   contrasto-riavvio sul totale delle lezioni che cambiano qualcosa) è riportato
   per forma, e cresce di giro in giro;
3. **le sovrapposizioni sono riconosciute**: le regole che parrot0 già applica
   vengono confermate, non duplicate;
4. **nessun membro imparato si applica fuori dal suo percorso** (contaminazione
   zero sulle batterie di contrasto);
5. **ogni contraddizione incontrata ha uno stato dichiarato** nel registro dei
   paradossi e un esito fra i cinque del §5-bis — nessuna sovrascrittura
   silenziosa, nessuna verità doppia invisibile;
6. **il C si è accorciato** per ogni decisione della comprensione resa
   raggiungibile (mantra #18).

## 8. Rischi

- **Il passo falso di L3, un piano più su.** Costruire L4 come un catalogo di
  forme che «leggono regole descritte». Antidoto: la descrizione conta solo per
  le conseguenze che genera; il punto d'innesto lo trova la comprensione.
- **Costo.** Far girare la comprensione sulle conseguenze prima di scrivere
  moltiplica i turni interni. Il bilancio di tempo resta quello dei mantra (mai
  alzare un budget): le conseguenze si provano con le stesse viste e gli stessi
  percorsi del turno, e il costo si misura con `/debug on` (tempo proprio,
  chiamate, visite).
- **Falsa conferma.** Una conseguenza già prodotta per una ragione diversa da
  quella della lezione sembra una sovrapposizione. Antidoto: la conferma porta la
  derivazione, e un contrasto che la lezione distinguerebbe.
- **Chiedere troppo.** Una domanda di esempio per ogni lezione è un interlocutore
  pesante. Antidoto: si chiede solo quando le conseguenze non si possono formare,
  e la domanda è una sola.

## 9. Domande aperte

1. Come si ricava una conseguenza da una **descrizione** senza un lettore di
   descrizioni? Ipotesi: dagli esempi che la descrizione stessa contiene o che la
   sessione ha appena mostrato, e altrimenti chiedendone uno.
2. Il punto d'innesto è sempre unico? Una lezione può toccare due percorsi (il
   plurale in lettura e in generazione): va innestata in entrambi o in una
   sorgente comune da cui entrambi derivano?
3. ~~Che cosa fa L4 quando la lezione contraddice la comprensione corrente?~~
   Risposta di F. (26 settembre): la contraddizione entra nel registro dei
   paradossi come specie, e un processo la porta a uno stato dialettico
   gestibile (§5-bis, L4-8). Resta aperto: **quale fonte pesa di più** quando
   nessuna distinzione si trova e l'utente non risponde? La fiducia nelle fonti
   deve essere conoscenza (chi l'ha detto, quante volte, con quale esito), non un
   ordine fisso.
4. Come si misura la coerenza di una lezione che non ha conseguenze osservabili
   subito (una regola che serve solo in frasi future)?

## 10. Da dove si parte, in concreto

Le batterie esistono già e sono vere:

- G1–G25 della grammatica ([train-the-learning-process.md](train-the-learning-process.md),
  PRIORITARIO §1): le otto sovrapposizioni sono il primo banco di L4-1;
- le 100 della meccanica, con le 71 verifiche indirette e inverse fallite delle
  procedure: il banco di L4-6;
- la contaminazione «gauge» / «lead»: il banco di L4-3;
- «print_r» e «Is a lathe a machine tool?» dopo il riavvio: il banco di L4-4;
- «an universal»: il banco di L4-5.

Si comincia da L4-0, poi L4-1 sulle otto sovrapposizioni, perché è il caso più
semplice del passo 3 — la comprensione produce già la conseguenza — e perché
chiude subito la sorgente dei fatti spazzatura.
