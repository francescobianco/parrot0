# L2 — insegnare a parrot0 che cosa è vero di QUESTA occorrenza

**Bozza, 21 settembre 2026.** Nasce da una sessione di iterazioni di riferimento
([train-the-learning-process.md](train-the-learning-process.md)) e da
un'obiezione di F. che ne ha rovesciato il senso. Si appoggia al metodo delle
[radici dell'insegnabilità](radici-insegnabilita.md), di cui è l'applicazione a
una classe di catene che quel documento non aveva ancora censito.

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
reading_choice(Turno, Span, Specie, Valore, Perché)
```

- **Specie**: confine, antecedente, ruolo, forza, senso — le famiglie di
  decisione per occorrenza, dichiarate in KB e non nel motore;
- **Perché**: il fatto KB che l'ha causata (`np_closer(opens)`), così la
  ragione è **nominabile** e quindi contestabile.

⚠ **Costo.** Una ricevuta per decisione per turno non si può pagare su tutto.
Vincolo di progetto: le ricevute vivono **un turno solo**, nello strato
riflessivo (`KB_REFLECTIVE`, mai persistito — la lezione di RI-004), perché la
correzione arriva subito dopo. Se serve tenerle oltre, è una scelta esplicita,
non il comportamento normale.

### 7.2 parrot0 dice che cosa ha capito, in lingua

Non `/debug`: quello è per chi sviluppa. Una riga detta al maestro:

```text
Ho letto «a relief valve opens» come una cosa sola, perché «opens» chiude un
sintagma; e «its» come il tornio dinamometrico del turno prima.
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
Tolgo «opens» dai chiusori di sintagma quando segue un nome?
  · solo in questa frase
  · ogni volta che «opens» segue un nome
  · ogni volta che un verbo segue un nome
```

Il maestro sceglie. È questo il punto in cui una correzione diventa una
**lezione**: la generalizzazione è proposta da chi ha sbagliato e approvata da
chi insegna, invece di essere indovinata da uno dei due.

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
| **0 — la ricevuta** | `reading_choice/5` per **una sola** specie: il confine di sintagma | `/debug` mostra la ricevuta con la sua ragione; costo del turno invariato dentro il budget di `soft-test` |
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
