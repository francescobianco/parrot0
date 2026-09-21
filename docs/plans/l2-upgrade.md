# L2 — insegnare a parrot0 che cosa è vero di QUESTA occorrenza

**Piano in revisione e prima implementazione, 21 settembre 2026.** Nasce da una sessione di iterazioni di riferimento
([train-the-learning-process.md](train-the-learning-process.md)) e da
un'obiezione di F. che ne ha rovesciato il senso. Si appoggia al metodo delle
[radici dell'insegnabilità](radici-insegnabilita.md), di cui è l'applicazione a
una classe di catene che quel documento non aveva ancora censito.

**Stato operativo:** i §§1–6 conservano la diagnosi storica, non una nuova
misurazione del codice corrente. Contratto eseguibile, lavoro in corso,
prove e guida di continuazione sono nel §13. La sola presenza della patch non
certifica L2: servono correzione, trasferimento controllato e ritrattazione.

**Vincolo aggiunto da F.: rendimento della lezione.** Una lezione deve servire
molti casi, potenzialmente una classe aperta. Il bersaglio non è collezionare
correzioni per frase o per parola: è massimizzare il trasferimento corretto
per lezione, conservando contesto, eccezioni e controllo sulla portata.

> **In una frase.** Tutto ciò che oggi si insegna a parrot0 è una proprietà di
> una **classe** — una parola, una relazione. Niente è una proprietà di
> un'**occorrenza**. Ma il taglio di un sintagma, il legame di un pronome,
> l'assegnazione di un ruolo sono decisioni per occorrenza: perciò ogni errore
> che vive lì è **fuori** da ciò che una lezione può raggiungere, e «vedi,
> questo non è così, è così» non ha un *questo* a cui attaccarsi.

---

## 1. Il fatto che ha aperto il discorso

Una frase qualunque di manualistica, misurata su `61c49758`, profilo `agi`:

```text
> A torque wrench is a tool.
Learned: torque wrench is a tool.

> A relief valve opens above its set pressure.
I didn't keep that: «relief_valve_opens_above_torque_wrench» and «pressure»
don't read as things I can hold a fact about.
```

Tre difetti in una riga, di specie diverse:

1. il verbo finisce dentro il nome — `relief_valve_opens`;
2. «its» viene legato all'entità del **turno precedente**, di un altro dominio;
3. il risultato di quel legame entra nell'**atomo**, invece di restare un
   legame: l'errore si solidifica in un nome che non esiste.

E il rifiuto finale è quasi una beffa: il cancello dice «non leggo queste come
cose su cui tenere un fatto» — ha ragione, ma su una cosa che ha inventato lui.

## 2. Che cosa succede se provi a correggerlo parlando

Il passo 5 del metodo delle radici («verificare che ogni anello raggiunga il suo
lettore, parlando»). Tre tentativi, tre esiti, tutti peggiori del muro:

| detto | risposta | che cosa è successo davvero |
|---|---|---|
| `no, the subject is a relief valve` | «Hmm, I don't know about relief valve yet.» | la correzione **non è capita** |
| `"opens" is the verb` | **«Learned: opens is a verb.»** | la lezione **è accolta** e non cambia niente: `verb/1` non ha **nessun consumatore** (verificato: nessuna regola KB, nessun lettore C). Una classe morta, e al maestro viene detto che ha imparato |
| `its refers to the relief valve` | **«Learned: torque wrench refer relief valve.»** | la frase che corregge il pronome **subisce lo stesso errore che sta correggendo**, e il risultato entra in KB come fatto del mondo |

Il secondo caso è il più grave dei tre, ed è una specie nuova di anello rotto,
da aggiungere al §6 di [radici-insegnabilità](radici-insegnabilita.md):

> **la lezione raggiunge un lettore, viene accolta, e finisce in una classe che
> nessuno legge.** Non è «il turno catturato da un altro lettore»: è peggio,
> perché il maestro riceve una conferma e non ha modo di accorgersi di niente.

## 3. Che cosa registra parrot0 di quella lettura

`/debug` sul turno sbagliato:

```text
debug_np_candidate     a relief valve opens          ← il taglio sbagliato si VEDE
debug_turn_entity      relief_valve_opens  pressure  ← l'entità inventata si VEDE
debug_frame_record     niente                        ← la lettura usata NON è registrata
machinery_gap          niente
turn_gap_kind          niente
turn_gap_remedy        niente
turn_said_by           template(rejected_binary_fact)
```

Ha rifiutato un fatto, l'ha detto, e **non ha lasciato traccia del perché**. Per
il motore quel turno è «gestito». Le sonde mostrano il sintomo; una sonda non è
una cosa che si possa contraddire.

## 4. La catena, e dove finisce

Metodo del §5 di radici-insegnabilità.

**Abilità A — «in questa frase il nome finisce prima di *opens*».**

| Gradino | Che cosa c'è | Fine |
|---|---|---|
| A | il taglio, deciso leggendo `np_closer/1` | — |
| S1: aggiungere un chiusore | «X is a relation verb» → `relation_verb(X)` → `np_closer(X)` | superficie presente |
| S1′: **negare** il chiusore *qui* | nessuna. E negarlo in generale sarebbe **falso**: *opens* è un verbo | **riga a mano** |

**Abilità B — «in questa frase *its* si riferisce alla valvola».**

| Gradino | Che cosa c'è | Fine |
|---|---|---|
| B | il legame del pronome | — |
| S1 | nessuna superficie: il tentativo viene letto come un fatto del mondo | **riga a mano** |

Due catene di **lunghezza zero**: il caso §8.2 di radici-insegnabilità,
«abilità con un ramo C dedicato». Vitalità 0.

## 5. I due livelli

| | **L1 — oggi** | **L2 — da aprire** |
|---|---|---|
| Di che cosa parla una lezione | di una **classe**: una parola, una relazione, una forma | di un'**occorrenza**: questo span, in questo turno |
| Esempi che funzionano | «the hertz measures frequency», «flow into is a relation verb», «flow chains», «A pump is a device» | — |
| Esempi che servirebbero | — | «no, qui *opens* è il verbo», «*its* qui è la valvola», «il soggetto è *a relief valve*» |
| Come si corregge un errore | cambiando ciò che vale **sempre** | dicendo che cosa vale **qui**, e lasciando che parrot0 proponga fin dove estenderlo |
| Stato | esiste, ed è cresciuto molto | **non esiste**: manca l'oggetto, non la superficie |

**L2 non è «correggere l'output».** Correggere una frase alla volta non insegna
niente: domani «a check valve opens under back pressure» sbaglia uguale. E una
correzione che generalizza da sola è peggio: il maestro perde il controllo di
ciò che ha appena cambiato. L2 è il **ciclo** fra le due cose — si dice che cosa
è vero qui, e si decide insieme fin dove vale.

## 6. Il precedente che dice che si può fare

Non è un'idea nuova in questo progetto: è già successo, un piano più in basso.

I **fatti** sono correggibili parlando — «forget that the Po flows into the
Black Sea» toglie il fatto, la sua provenienza, le registrazioni del conflitto,
e la riga dal file (RI-006). Funziona per una ragione sola: **ogni fatto appreso
porta con sé la frase che l'ha prodotto** (`fact_source/3`, `reading_fact/2`).
La ritrattazione aveva qualcosa da indicare.

Lo strato della **comprensione** non ha nulla di equivalente. Le decisioni di
lettura leggono la KB, ma non lasciano ricevuta: nessun nome, nessuna ragione,
nessun turno a cui appartenere.

> **La tesi di questo documento: la lettura ha bisogno del suo `fact_source`.**

## 7. Che cosa deve esistere perché «no, è così» funzioni

Quattro cose, in ordine. La prima abilita tutte le altre.

### 7.1 Ogni decisione di lettura lascia la sua ricevuta

Una decisione per occorrenza diventa un oggetto pubblicato:

```text
reading_choice(Turno, Nodo, Specie, choice(Valore, Evidenza))
```

- **Specie**: confine, antecedente, ruolo, forza, senso — le famiglie di
  decisione per occorrenza, dichiarate in KB e non nel motore;
- **Evidenza**: il goal KB effettivamente usato, così la
  ragione è **nominabile** e quindi contestabile.

La vecchia bozza usava arità 5: non caricava. Il limite reale in `src/kb.h`
è arità 4; il corpo è ora 16 goal (alcune istruzioni storiche dicono ancora 8).
Un goal usato non equivale alla sua intera prova: non inventare una catena di
ragioni che il sensore non ha osservato.

⚠ **Costo.** Una ricevuta per decisione per turno non si può pagare su tutto.
Vincolo di progetto: strato riflessivo (`KB_REFLECTIVE`, mai persistito — la
lezione di RI-004), con durata dichiarata in KB. **Non cancellarle all'ingresso
del turno seguente:** proprio quel turno deve correggerle. Il primo incremento
riusa `turn_scoped` e la ritenzione del discorso già esistente; una finestra
dedicata più breve va implementata e misurata, non dichiarata già presente.

### 7.2 parrot0 dice che cosa ha capito, in lingua

Non `/debug`: quello è per chi sviluppa. Una riga detta al maestro:

```text
Ho letto «a relief valve opens» come una cosa sola; la lettura non si è fermata
prima di «opens». Ho legato «its» all'entità del turno precedente.
```

Oggi quella riga non esiste, ed è il motivo per cui l'errore te l'ho dovuto
mostrare io con una sonda. **Senza questa frase non c'è niente da negare.**

⚠ Non va detta sempre: è il rumore che ucciderebbe la conversazione. Il
criterio naturale è *quando la lettura non produce niente* (il turno rifiutato,
il muro) o *quando il maestro la chiede*.

### 7.3 La negazione atterra sulla conoscenza, con la portata dichiarata

«no, qui *opens* è il verbo» non deve scrivere una toppa sulla frase: deve
arrivare alla decisione, e parrot0 deve **proporre** che cosa cambierebbe:

```text
Faccio terminare il sintagma prima di «opens»?
  · solo in questa frase
  · ogni volta che «opens» segue un nome
  · ogni volta che un verbo segue un nome
```

Il maestro sceglie. È questo il punto in cui una correzione diventa una
**lezione**: la generalizzazione è proposta da chi ha sbagliato e approvata da
chi insegna, invece di essere indovinata da uno dei due.

**Correzione della bozza:** togliere un chiusore avrebbe allungato il sintagma,
cioè l'opposto della cura proposta. Inoltre decidere il confine non dimostra
che si sappiano rappresentare il processo di apertura, la soglia o il
possessivo. Queste acquisizioni richiedono verifiche distinte.

⚠ Vale il divieto del §7 di radici-insegnabilità: la proposta non deve nominare
`np_closer` né `turn_form`. Deve dire *chiusore di sintagma* con le parole di
chi insegna, o non è una superficie.

### 7.4 La stessa frase viene riletta nello stesso turno

```text
Adesso leggo: soggetto «a relief valve», verbo «opens», complemento «above its
set pressure». Tengo: opens(relief_valve) sopra la pressione di taratura?
```

È il momento in cui si vede se ha capito — e l'unico modo perché il maestro
possa accorgersi che la correzione ha preso la piega sbagliata **prima** che
diventi conoscenza.

## 8. Come si misura

| Misura | Definizione | Oggi |
|---|---|---|
| **Correggibilità conversazionale** | errori di lettura osservati che il maestro può correggere **parlando**, senza toccare il sorgente / errori osservati | **0 su 3** (§2) |
| **Vitalità L2** | catene di decisione per occorrenza che finiscono in radice o circolo / catene censite | **0 su 2** |
| **Silenzi** | lezioni accolte che non cambiano nessun comportamento | almeno 1 misurato (`verb/1`) |
| Test del mantra | «parrot0 può impararne un membro nuovo domani, senza ricompilare?» applicato alle **decisioni di lettura** | no |

La misura che interessa è la prima. Una suite verde con correggibilità zero dice
che parrot0 legge molte frasi e non può essere corretto su nessuna.

## 9. Stadi

Ognuno si chiude con il **segno d'uso reale** del §5.7 di radici-insegnabilità:
una lezione vera, su conoscenza vera, salvata e riverificata in un processo
nuovo.

| Stadio | Che cosa apre | Come si sa che è chiuso |
|---|---|---|
| **0 — la ricevuta** | `reading_choice/4` per **una sola** specie: il confine di sintagma | query mostra la ricevuta con la sua ragione; durata, non persistenza e costo verificati sulla KB viva |
| **1 — dirla** | parrot0 dice in lingua che cosa ha letto, quando la lettura non produce niente | sulla frase della valvola, la riga si legge senza `/debug` |
| **2 — negarla** | «no, qui X è il verbo», con la proposta di portata e la rilettura nello stesso turno | la frase della valvola entra dopo la correzione, e «a check valve opens under back pressure» entra **senza** una seconda correzione se la portata scelta era quella |
| **3 — le altre specie** | antecedente e ruolo, con la stessa meccanica | «its refers to the relief valve» smette di essere letta come un fatto del mondo |
| **4 — durata** | la correzione si salva, si ritira («forget that …»), e sopravvive al processo nuovo | il ciclo di RI-006 applicato a una correzione di lettura |

**Lo stadio 0 è il buco ad alta leva** (§7 di radici-insegnabilità: si apre il
gradino che chiude una **classe** di catene). Chiuderlo serve al confine, alla
coreferenza, al ruolo, alla forza del turno e al senso di una parola insieme.

## 10. Trappole, scritte prima di caderci

- **Non è un editor di grammatica.** Se il maestro deve dire dove tagliare in
  ogni frase, abbiamo spostato il lavoro, non l'abbiamo tolto. La correzione ha
  valore solo se la portata la generalizza.
- **Non è una console di debug in lingua.** Se la riga del §7.2 diventa un dump,
  nessuno la leggerà e la correzione non arriverà mai.
- **Non esporre lo schema** (divieto anti-inganno di `MANTRA.md`).
- **Non aprire un anello per un'istanza** (§7 di radici-insegnabilità): la prima
  correzione che «funziona» su una frase sola è il segnale che stiamo rifacendo
  l'errore del §11.
- **Attenzione alla correzione che si auto-conferma**: «its refers to the relief
  valve» oggi viene mis-letta *con lo stesso difetto*. Una correzione che passa
  dal circuito che sta correggendo non è una prova.

## 11. Perché questo documento esiste: l'errore da cui nasce

Nei due lotti di iterazioni di riferimento ho «curato» quattro decisioni per
occorrenza — il confine dell'entità (RI-011), quale parola è il verbo (RI-012),
che cosa conta come valore (RI-013), che cosa è una cosa (RI-014). Ogni volta
ho scritto **una forma di turno più un atto in C**, e ogni volta la frase
passava.

Per il §2 di radici-insegnabilità quella forma **non è una radice**: è una riga
a mano, cioè **un buco nuovo**. Ho scambiato per cure quelle che il metodo
classifica come debito, e ho fatto crescere l'insegnabilità del motore —
ricompilando — invece che quella di parrot0.

F., vedendolo: *«la missione è stata tradita dall'insegnare nuove forme di
apprendibilità; mi sembra che gli stai verificando la grammatica»*. Il caso
operativo è nel §0.5-bis di
[train-the-learning-process.md](train-the-learning-process.md), con il test da
applicare **prima** di scrivere una cura.

L'unica di quelle iterazioni che regge senza riserve è RI-013 **dopo** la
correzione di rotta: lì la cosa che si insegna — «the hertz measures frequency»
— è una proprietà di una **classe**, e infatti la lezione funziona, si ritira e
si salva. È la prova che L1 è sano; il problema non è L1, è che a L1 stavamo
chiedendo un lavoro che è di L2.

## 12. La domanda aperta, per chi legge

Lo stadio 2 chiede a parrot0 di **proporre una generalizzazione** di una
correzione. Ma proporre una generalizzazione è, a sua volta, un'abilità: e la
domanda delle radici si ripete — *con quale superficie si insegna a parrot0 a
proporre meglio?* Se la risposta è «con una lista di proposte scritte a mano»,
abbiamo spostato il buco di un gradino invece di chiuderlo.

La risposta che mi sembra giusta, e che va discussa prima di scrivere: le
portate possibili non sono una lista, sono i **livelli di astrazione che la
ricevuta già contiene** — questa occorrenza, questa parola, questa classe di
parole, questa forma. La proposta si genera dalla ricevuta, e cresce quando
cresce ciò che la ricevuta sa dire.

## 13. Contratto di implementazione e passaggio di consegne

### 13.1 Ordine di lavoro KB-first

F. ha giustamente chiesto perché la prima patch partisse dal C. L'ordine
corretto è: **oggetti KB → regole che li consumano → lezione pronunciabile →
prova del comportamento → sola meccanica mancante**. Questa sessione aveva
descritto il riuso della IR, ma non aveva ancora scritto il contratto prima
della patch: errore di metodo da non ripetere.

L'oggetto non è un nuovo parser: è la decisione sul nodo che esiste già.
`src/code.c:input_structure` produce la IR, `input_structure_publish` la
pubblica; `input_node_range`, `np_phrase_words`, `input_token_in_phrase` e
`input_entity_node` ne sono consumatori. `session_archive_turn` conserva i
nodi del turno precedente. Prima di inventare uno stato nuovo, cercare qui.

| Oggetto | Contratto | Durata |
|---|---|---|
| `reading_choice(Scope, Id, Kind, choice(Value, Evidence))` | ricevuta immutabile della decisione del producer; `Id` è il nodo IR, non un indice privato | riflessiva, stessa ritenzione dello scope |
| `reading_revision(Scope, Id, revision(Value, Evidence))` | correzione distinta dall'osservazione originaria; non è un fatto del mondo | riflessiva |
| `input_node_range/4` | vista effettiva: revisione se presente, altrimenti osservazione | derivata |
| `reading_boundary_lesson(Word, contrast(Old, New))` | prima portata generale, approvata esplicitamente; conserva un contrasto leggibile dopo la scadenza dello scope | sessione, persistibile |
| `reading_boundary_class(Class, example(Word, Contrast))` | estensione approvata a una classe aperta già insegnabile, consumata attraverso `apply/2` | sessione, persistibilità da verificare |
| `phrase_boundary_stop(Kind, Word, Evidence)` | la disgiunzione dei criteri che arrestano il producer e il goal usato | conoscenza KB |

I nomi del primo incremento dicono deliberatamente «boundary»: **non**
affermare di aver insegnato un verbo, un ruolo o il significato di una frase.
La famiglia generale è la ricevuta/revisione; le politiche della prima specie
stanno in `kb/core/reading-choices.p0`. Specie ulteriori devono riusare questi
oggetti, senza nuovi registri C per ogni abilità.

### 13.2 Primo incremento verticale e limiti

Lezione prevista e da verificare sulla KB completa:

```text
A relief valve opens above its set pressure.
end the previous noun phrase before opens
use that boundary for opens in future sentences
A safety valve opens under pressure.
forget the boundary for opens
```

La prima correzione raggiunge un'unica occorrenza del turno precedente e
rilegge i token attraverso **lo stesso** consumatore dei sintagmi. Solo
l'approvazione successiva cambia il criterio dei futuri input. Una parola
ripetuta o più candidati compatibili richiedono disambiguazione: mai scegliere
il primo. La rilettura strutturale non ridispaccia la frase come asserzione e
non acquisisce il fatto della valvola: quel replay completo è ancora aperto.

Scelte meccaniche da mantenere piccole:

- Il producer registra l'evidenza quando decide; ricostruirla dopo una lezione
  confonderebbe la KB di adesso con la KB che ha causato l'errore.
- Il matcher di `turn_form` applica gli atti già esistenti. La durata degli
  effetti di una query è dichiarata da `turn_form_effect_origin(Form,
  reflective)`, non dedotta dal nome del predicato della correzione.
- La superficie osservata del nodo resta originale; la vista dell'intervallo
  e i consumatori dei token leggono la revisione. Chi vuole la superficie
  **corretta** deve renderla dai token effettivi, non spacciare la vecchia
  `input_node_surface` per una nuova lettura.

Debiti espliciti: la IR può essere ripubblicata durante ridispatch e turni
annidati; non tutti i consumatori leggono `input_node_range` anziché il range
grezzo; il lettore storico `p0_np_closer` ha ancora una potatura per parola
compilata (RI-012) che non coincide con il producer della IR. Non affermare
che una revisione della IR abbia già corretto questi percorsi.

### 13.3 Scalabilità delle lezioni: criterio di scelta, non ottimizzazione finale

Per ciascuna lezione tenere quattro insiemi separati: esempio corretto,
trasferimenti mai insegnati, controlli che non devono cambiare, casi ambigui.
Variare entità, parola, dominio, ordine, ruolo e lingua; cambiare soltanto
il nome dell'entità prova una portata per parola, **non** una portata per classe.

| Portata | Stato desiderato | Prova necessaria |
|---|---|---|
| occorrenza | modifica locale con ricevuta | un'altra occorrenza identica nello stesso turno non cambia |
| parola | primo gradino, mai traguardo di scalabilità | nuovi soggetti e complementi; altre parole non cambiano |
| classe | proprietà interrogabili della parola e ruoli già noti | parole diverse; membro insegnato domani, poi ritirato |
| contesto/forma | classe più vincoli di posizione, ruolo e forza | omonimo nominale non cambia; varianti della costruzione sì |
| politica di proposta | ordina le portate per copertura ed eccezioni | preferenza insegnabile e ritrattabile, senza ramo C nuovo |

La proposta deve enumerare astrazioni **giustificate**: non attribuire una
classe ignota alla parola per ottenere la portata più ampia. Una correzione
non basta a provare universalità. Fra portate compatibili, proporre quella
che copre più casi verificati con meno eccezioni; il maestro ne autorizza
l'estensione. Se la KB non sa ancora distinguere l'uso nominale da quello
verbale, mostrarlo come limite e chiedere quel contrasto utile.

Metriche da riportare senza gonfiarle:

- rendimento: trasferimenti corretti nuovi / lezioni ricevute;
- ampiezza: numero di parole, strutture e domini distinti trasferiti;
- interferenza: controlli peggiorati / controlli provati;
- costo: fatti persistenti per lezione, tempo del turno e della rilettura;
- fertilità: nuova superficie e nuovo membro della classe utilizzabili e
  ritirabili a runtime; nessuna ricompilazione tra le prove.

Non ottimizzare il rapporto nascondendo casi rossi, unendo lezioni diverse
nel denominatore o chiamando «trasferimento» la sola presenza di un fatto KB.

### 13.4 Checklist di continuazione, nell'ordine

1. Verificare il caricamento `.p0` e leggere stderr. Un'arità errata sembra
   assenza di conoscenza. Conservare arità ≤4 anche negli `assert`: il nome
   del predicato occupa un argomento del builtin, quindi asserire da una
   regola un fatto a quattro argomenti non entra nello stesso limite.
2. Eseguire il banco L2 sulla KB `agi` completa. Distinguere prove meccaniche
   con entità inventate da letture reali. Query interne verificano il
   collegamento al consumatore; le lezioni devono restare lingua naturale.
3. Chiudere durata, ritiro locale, ambiguità e nessuna falsa conferma prima
   di ampliare le specie. Un fallimento non deve lasciare mezza revisione.
4. Generalizzare con proprietà/contesti KB e provare parole diverse; vietato
   introdurre un handler C per ogni portata. Conservare un contrasto negativo
   nominale (es. `check valve`, `direct current`).
5. Per il replay completo separare proposta di lettura e commit dei fatti.
   Riutilizzare lo stesso Brain e la KB viva. Non chiamare ricorsivamente
   `brain_respond` sperando che i suoi effetti siano una transazione.
6. Collegare la coreferenza agli stessi nodi e ricevute. Non riscrivere il
   pronome dentro l'atomo prima di poter contestare l'antecedente.
7. Persistenza in processo nuovo: salvare solo le lezioni approvate, poi
   dimostrarne trasferimento e ritiro. Le ricevute non devono riapparire al
   boot. Non cambiare il profilo e non usare una KB vuota per renderlo verde.
8. Aggiornare `LEARN_PROTOCOL.md` soltanto con superfici provate, effetto e
   limite. Registrare sotto comandi, esiti e lavoro rimasto.

### 13.5 Registro delle verifiche

Base della sessione: `94b69234`, working tree inizialmente pulito.
Build della patch C: `make -j2 build`, riuscita, senza nuovi warning.

| Prova | Esito osservato |
|---|---|
| Primo banco `l2_reading_choices.p0t`, un test | **11/11**: ricevuta, correzione locale, consumo IR, portata per parola, trasferimento, ritiro |
| Conversazione reale con `opens is a verb`, `vents is a verb`, correzione e `use that boundary for every verb` | promozione accolta; dopo `A safety valve vents under pressure.`, la domanda sul sintagma risponde **safety valve** |
| Prima versione del banco esteso, categoria `release verb` | **25 pass, 11 fail**: le lezioni L1 non creavano `release_verb/1`, quindi nessuna prova valida di quella categoria; non curare L2 aggirando quel prerequisito |
| Ultima versione del banco esteso, categoria `verb`, sotto gdb | **SIGSEGV in `solve_frame`**, riprodotto. Il client stampa erroneamente `ok … 0 passed`: esito invalido, non verde |
| Caricamento nelle sessioni CLI osservate | nessun `PARSE ERROR` riportato |
| `make soft-test` dopo la patch | **non eseguito**, resta requisito prima di considerare chiuso lo stadio 0 |
| Persistenza e processo nuovo | **non verificati** |

Il banco esteso è stato corretto per usare la categoria `verb`, che il
transcript ha realmente insegnato. Il suo ultimo esito è riportato nel §13.7.
Queste sono misure di crescita strutturale, non prove di comprensione fisica
della valvola. Le membership della classe sono lezioni prerequisite e vanno
contate separatamente: non attribuire alla sola lezione L2 ciò che si è dovuto
insegnare prima.

### 13.6 Incidenti e insight che evitano di ripartire al buio

1. **Due tagli diversi erano nascosti sotto lo stesso nome.** Il producer IR
   è in `src/code.c`; `p0_np_closer` in `10-memory-knowledge.c` è un altro
   percorso. Cambiare il primo cambia `input_entity_node`, ma una risposta
   legacy può ancora dire «I don't know about safety valve opens». Non
   riscrivere l'output per nasconderlo: collegare il consumatore alla IR.
2. **Il contesto può peggiorare il fatto acquisito.** Dopo aver insegnato
   `opens is a verb` e `vents is a verb`, il turno della valvola ha risposto
   `Learned: relief valve opens above vents set pressure.`. È il difetto di
   coreferenza originario, ancora vivo. La revisione locale del sintagma non
   ritira quel fatto già scritto. Il replay transazionale e la dipendenza
   fatto→lettura sono necessari prima di certificare comprensione L2.
3. **La prima superficie era rubata.** `in that sentence the noun phrase ends
   before opens` riceveva «I don't have the sentence you mean…», prima della
   nuova forma. La forma attuale provata è `end the previous noun phrase
   before opens`. La vecchia formulazione resta un gap, non un sinonimo
   supportato. Non aggiungere un'altra lista di cue nel C per coprirla.
4. **Ricompilare non aggiorna il demone già vivo.** Le prime query non
   vedevano `reading_choice` perché il vecchio test-engine aveva ricaricato
   la KB nuova ma eseguiva il producer vecchio. Usare un processo nuovo dopo
   ogni modifica C. Modifiche KB sono rilette dal `!reset` del banco.
5. **`0 passed` non significa successo.** Un'esecuzione sul demone separato
   è terminata con exit 139; il client ha comunque stampato `ok … 0 passed`.
   Non contare quell'esito. Una ripetizione sotto gdb ha fornito i 25/11
   sopra; la successiva, con categoria `verb`, ha riprodotto **SIGSEGV in
   `solve_frame`**. **Causa non isolata:** è un difetto aperto, prioritario
   rispetto a ogni estensione. Vedi traccia sotto.
6. **La sandbox può impedire il socket.** Un altro avvio, distinto dal crash,
   è fallito con `bind: Operation not permitted`. Non diagnosticare la KB:
   avviare demone e client con l'escalation prevista dall'ambiente.
7. **Mutazioni nel solver:** la revisione è l'ultimo goal dopo validazione e
   rendering del risultato. Questo riduce gli effetti parziali, ma non offre
   una transazione generale né prova assenza di invalidazione dei puntatori.
   Non aggiungere sequenze di scritture speculative nelle regole.
8. **Il linguaggio del test conta.** `<` confronta tutta la risposta; `<~`
   controlla un contenuto. I due fallimenti iniziali su risposte più lunghe
   erano aspettative sbagliate, non un fallimento della correzione. I test
   semantici devono verificare anche la lettura, non soltanto il template.

### 13.7 HANDOFF — ripartire qui

**Richiesta dell'utente:** continuare il piano L2 con forte astrazione KB-first,
massimizzando i casi coperti per lezione. L'utente ha chiesto esplicitamente
questo passaggio di consegne per esaurimento dei token. Fermarsi al handoff
è quindi intenzionale; non presentare il lavoro come completamento di L2.

**Ultimo stato: WIP NON STABILE.** La revisione del piano e il handoff sono
completi; la patch cognitiva richiede diagnosi del crash e verifiche prima
di un commit di capacità. Il banco corrente non ha un risultato verde.

**Working tree, nessun commit creato:**

| File | Contenuto della modifica |
|---|---|
| `src/code.h`, `src/code.c` | ricevuta sul nodo NP costruito, evidenza effettiva del confine, pubblicazione e pulizia nello stesso scope IR |
| `kb/core/input.p0` | disgiunzione dei criteri di arresto in `phrase_boundary_stop/3`, prima compilata |
| `kb/core/input-structure.p0` | proiezione del range effettivo; i consumatori dei sintagmi leggono la revisione |
| `src/brain/10-memory-knowledge.c` | dichiarazione generica di origine riflessiva degli effetti per `op(match/count)`; origine precedente ripristinata subito dopo la query |
| `kb/core/reading-choices.p0` **nuovo** | targeting univoco, revisione, replay dei token, ispezione, portate parola/classe, proposte di classi da membership note, ritiro locale e delle portate, forme NL e risposte |
| `kb/core/procedures.p0` | include del nuovo file |
| `tests/p0t/language/l2_reading_choices.p0t` **nuovo** | quattro casi sulla KB completa: parola, classe aperta, undo locale, rifiuto di ambiguità/assenza |
| `LEARN_PROTOCOL.md` | superfici del primo incremento e limiti |
| questo piano | revisione, contratti, rischi e sequenza di continuazione |

**Prossima sessione, evitare deviazioni:**

1. Leggere MANTRA, PRINCIPLES, questo §13 e il diff. Eseguire il banco
   mirato con un demone fresco; accertare un numero positivo di asserzioni e
   nessuna caduta del processo. Il test non è ancora inserito in `Makefile`.
2. Diagnosticare il SIGSEGV già riprodotto. Poi eseguire `make soft-test`
   senza aumentarne il budget. Non lanciare l'intera suite per inerzia.
3. Chiudere le prove mancanti del primo incremento: ablazione di una **forma
   di riconoscimento**, membro di classe ritirato, lezione per classe ritirata,
   durata oltre la finestra, assenza di ricevute nel salvataggio e ripartenza
   con la sola lezione approvata. Non fare un altro incremento C prima.
4. Aggiungere il contrasto che serve alla generalizzazione **contestuale**:
   l'attuale portata per classe termina prima di ogni uso del membro, anche
   nominale. Non è ancora sicura per classi poliseme. Il template lo dichiara;
   la soluzione deve rappresentare ruolo/contesto nella KB, non un'eccezione C
   per `check` o `direct`. Una lezione contestuale deve coprire molte parole.
5. Integrare revisione e lettori dei fatti: rilettura nello stesso scope,
   dipendenze dei fatti dalla versione della lettura, ritiro/sostituzione dei
   derivati, solo poi commit. Non creare un secondo Brain e non simulare un
   rollback cancellando la KB del profilo.
6. Il criterio di fine è il ciclo intero del §8 e il rendimento del §13.3,
   non il numero di nuove forme/righe o una demo del sintagma giusto.

**Comandi pratici:**

```sh
make -j2 build
# Processo in primo piano, terminale/tool session dedicata; KB agi completa.
./bin/parrot0 --test-engine --sock /tmp/parrot0-l2-test.sock
# Altro terminale/tool call, dopo il boot:
./bin/parrot0 --sock /tmp/parrot0-l2-test.sock --test tests/p0t/language/l2_reading_choices.p0t
./bin/parrot0 --sock /tmp/parrot0-l2-test.sock --test-report
make soft-test
```

Per il crash è stato usato `gdb -q -batch -ex run -ex 'bt 18' --args
./bin/parrot0 --test-engine --sock /tmp/parrot0-l2-debug.sock`. I socket in
`/tmp` sono trasporto del test, **non KB ridotte**. Per la persistenza,
attenzione: `kb.save` instrada le lezioni nei file della KB viva, non soltanto
nel path richiesto; leggere `brain_save_session`/`kb_save_routed` prima di
usarlo in una prova e pulire soltanto le lezioni specifiche introdotte.

**Traccia finale del crash** (binario `-O2`, senza simboli di linea):

```text
Program received signal SIGSEGV, Segmentation fault.
#0  solve_frame
#1  solve
#2  solve_frame
#3  solve
#4  solve_frame
#5  solve
#6  solve_frame
#7  solve
#8  solve_frame
#9  solve
… alternanza fino al frame #17 catturato
```

Non basta per distinguere ricorsione/stack da puntatori invalidati durante
una scrittura KB. Ripartire con simboli di debug e banco diviso nei quattro
test, conservando il profilo completo; registrare il turno preciso prima
di cambiare il solver. Non introdurre una seconda KB per evitare il crash.
Il processo gdb è terminato dopo la traccia; non occorre continuare quel
processo. Il controllo `git diff --check` è passato. Nessun salvataggio di
lezioni, commit o push è stato eseguito.
