# Che cosa stiamo facendo, e come si accelera

*Aperto il 2026-09-06 su richiesta di F.: «lavora al livello metodologico,
lasciamo i numeri da parte — capiamo cosa stiamo facendo (accrescere la KB per
aumentare le abilita' di comprensione) e come accelerare questo processo».*

I numeri stanno in coda, al §7, come prova. Qui c'e' il metodo.

---

## 1. La scoperta che riorienta tutto: non stiamo accumulando, stiamo distinguendo

Guardando **tutte e otto** le abilita' acquisite nella sessione del 6 settembre,
nessuna e' stata «parrot0 non sapeva una cosa e adesso la sa». Tutte e otto sono
state **una distinzione che mancava**:

| il difetto | la distinzione che mancava |
|---|---|
| il thinking sovrascriveva la risposta | un passo *puo'* portare conoscenza ≠ l'*ha portata* |
| «yo» rivendicava «yoga» e «beyond» | contenere le *lettere* ≠ contenere la *parola* |
| il boot costava il doppio | una vista *e' dichiarata* ≠ *e' costruita* |
| «say» non apriva «says» | la direzione *a buon mercato* ≠ quella *cara* |
| «he eats nice food» non era un'affermazione | poter essere *soggetto* ≠ poter essere *chiave* |
| «l'arrocco negli scacchi» → definizione degli scacchi | *nominato* nel turno ≠ *chiesto* dal turno |
| «un altro modo di dire» confermava il falso | entrare in *quella classe* ≠ *leggersi uguale* |
| «la legge di ohm» → «la read di ohm» | conoscere la *legge* ≠ conoscere il suo *nome* |

Otto su otto. E in tutti e otto i casi la conoscenza **c'era gia'**: la legge di
Ohm era in KB con tutto il suo albero, il campo `Pre` era dichiarato dal giorno
in cui lo schema fu scritto, `view_depends` diceva gia' l'ordine giusto.

> **La comprensione non cresce aggiungendo righe. Cresce separando due cose che
> un predicato solo stava rispondendo insieme.**

Questo cambia tutto il resto: che cosa cercare, dove cercarlo, e come misurare
se una sessione e' andata bene.

### Dove si trovano le prossime distinzioni

Se la forma e' sempre «un predicato risponde a due domande», allora si possono
**cercare** invece che aspettare che si presentino. Il segnale e':

> **un predicato che ha sia un consumatore che LEGGE sia uno che SCRIVE.**

`subject_guard/1` era esattamente questo — chiedeva insieme «puo' essere il
soggetto di una predicazione?» (lettura) e «puo' essere la chiave di un fatto?»
(scrittura) — e la fusione teneva parrot0 cieco a ogni frase con un pronome.

Altri candidati con la stessa firma, da guardare: `stopword/1` (filtro di
ricerca e filtro di apprendimento), `machinery/1` (nascondere a «cosa sai di X»
e escludere da `/save`), `wall_marker/1` (riconoscere un muro e decidere se
propagarlo), `question_word/1` (aprire una domanda e non essere un soggetto).

---

## 1-bis. L'unita' di costo non e' il secondo: e' il ciclo diagnostico

*F., leggendo la prima versione di questo documento: «i tempi che hai misurato
sono irrilevanti rispetto ad esempio al tempo di alcune ore che abbiamo
impegnato per gestire il ciclo completo dell'arrocco negli scacchi».*

Ha ragione, e la correzione riorienta il documento. Tutte le misure meccaniche
del §7 — ricompilazioni, boot, suite — messe insieme fanno **minuti**. Il caso
dell'arrocco ne ha richieste **ore**, attraverso due sessioni.

Il tempo non se ne va nelle operazioni. Se ne va nei **cicli diagnostici**:

> ipotesi → modifica → misura → tenere o disfare.

Un ciclo costa pochi secondi di macchina e molto di tutto il resto: formularlo,
scriverlo, misurarlo, capire che era sbagliato, disfarlo, e ricostruire il
contesto mentale che si era spostato. **Ottimizzare i secondi e' inutile.
Ottimizzare il numero di cicli e' tutto.**

### Dove sono andati i cicli dell'arrocco

| # | ciclo | esito |
|---|---|---|
| 1 | riprodurre, vedere `semantic_lead`, ipotizzare «il topic si sceglie ovunque nel turno» | ✅ giusta |
| 2 | **insegnare** «arrocco» per distinguere ignoranza da difetto | ✅ informativo, ed e' costato niente |
| 3 | costruire il fuoco, verificarlo nella proiezione | ⚠️ `semantic_lead` tace, **ma risponde un'altra facolta'** |
| 4 | restringere il testo su cui la proiezione segna l'evidenza | ❌ rompe «what is water» → disfatto |
| 5 | restringere la scansione delle parole | ❌ peggiora l'arrocco → disfatto |
| 6 | filtrare i token candidati | ❌ peggiora ancora → disfatto |
| 7 | accettare il parziale, committare, scrivere l'handoff | — |
| 8 | *(F.)* pubblicare la lettura **una volta** e verificarla in otto punti | ✅ chiuso |
| 9 | aggiungere le sonde di `/debug` | ✅ |

**Tre cicli su nove buttati** — e non erano tre errori diversi: erano **tre
varianti della stessa forma sbagliata**, «restringere l'ingresso», tentate dopo
che il ciclo 3 aveva gia' detto tutto.

### I due segnali che erano li' e che non ho letto

**Segnale A — il difetto si e' spostato.** Alla fine del ciclo 3 il difetto non
era chiuso: era migrato a un'altra facolta'. Quello era il momento di fermarsi.
Un difetto che migra non e' un difetto di chi lo mostra: e' una **lettura del
turno che nessuno fa**, e la cura e' pubblicarla una volta, non inseguirla.

**Segnale B — ho rotto qualcosa di non correlato.** Al ciclo 4 «what is water»
e' diventato un elenco di fatti. L'ho trattato come un ostacolo da aggirare. Era
**informazione**: se una modifica rompe casi che non c'entrano, stai toccando
una risorsa **condivisa** — e allora il difetto vive li', ma la cura dev'essere
una *verifica*, non una *restrizione* (R3).

Due segnali, entrambi arrivati presto, entrambi letti come attrito invece che
come diagnosi. Sono costati cinque cicli.

### Le tre regole d'arresto che ne seguono

Non sono consigli, sono **soglie**. Servono proprio perche' nel momento in cui
scattano si e' convinti di essere vicini alla soluzione.

1. **Alla seconda facolta' che ripete lo stesso errore, si smette di curare
   facolta'.** Non «quando si nota uno schema»: alla seconda. Il conto e'
   oggettivo e va tenuto.
2. **Al secondo tentativo della stessa forma di cura, si cambia forma.** Cicli
   4-5-6 erano tutti «restringi l'ingresso». Il terzo non andava tentato.
3. **Se una modifica rompe un caso non correlato, si annulla e si sale di
   livello** — non si cerca la variante che non lo rompe. Quel danno collaterale
   sta dicendo a che livello vive il problema.

### Che cosa avrebbe accorciato il caso a due cicli

Il ciclo 2 e' il modello da imitare: **insegnare «arrocco» per separare
ignoranza da difetto** e' costato pochi secondi e ha eliminato meta' dello
spazio delle ipotesi. E' una domanda che si puo' porre quasi sempre:

> **Il sistema sbaglia perche' non sa, o perche' non riesce ad arrivarci?**
> Rendere noto cio' che manca e riprovare risponde in un turno.

Se dopo il ciclo 3 avessi applicato la regola d'arresto 1, il caso sarebbe stato
1-2-3-8: **quattro cicli invece di nove**, e in una sessione sola.

---

## 2. I colli di bottiglia, come ricette

### R1 — Il difetto non abita dove si presenta

Ogni reperto di questa sessione e' emerso lontano dalla sua causa. «what is your
designation» sembrava un difetto di `chitchat`; era una cue di due lettere che
combaciava dentro «your». Il rosso del thinking sembrava coreferenza; era la
risposta sovrascritta dal proprio secondo pensiero.

**La prima ipotesi e' quasi sempre sul posto dove sei fermo.**

> **Ricetta:** prima di formulare una sola ipotesi, guarda la *strada* del
> turno: `/debug` (`turn_module`, `turn_outcome`, `turn_focus`),
> `lang.canonical`. Se non lo dicono, aggiungi una sonda — che e' una riga di
> KB, non una `fprintf`.

### R2 — Se la cura sposta il difetto invece di chiuderlo, sei al livello sbagliato

Sull'arrocco: `semantic_lead` ha smesso di mentire, e ha mentito `answerframe`.
Tolta quella, ha mentito il legatore di sintagmi. Tre facolta', stesso errore.

Non e' fastidio: e' **il segnale diagnostico piu' affidabile che abbiamo.**
Quando due facolta' sbagliano allo stesso modo, il difetto non e' in nessuna
delle due — e' in una **lettura del turno che nessuno ha fatto**, e va
pubblicata una volta e consumata da tutte.

> **Ricetta:** al secondo consumatore che ripete l'errore, smetti di curare
> consumatori. Il difetto e' una lettura mancante, e va **pubblicata una volta**
> e consumata da tutti. E' la regola d'arresto 1 del §1-bis, e ignorarla e'
> costato da sola cinque cicli.

### R3 — Verificare l'uscita e' additivo, restringere l'ingresso non lo e'

Provato due volte oggi, e due volte annullato: restringere il testo su cui si
segna l'evidenza, o filtrare i candidati in gara, cambia **quale relazione
vince** e non solo quale soggetto — «what is water» e' diventato un elenco di
fatti. Verificare il vincitore *dopo* averlo trovato non ha rotto niente.

> **Ricetta:** davanti a un turno rubato, aggiungi una verifica a valle. Non
> togliere concorrenti a monte. Vale in generale, non solo qui: una guardia che
> riduce cio' che un pezzo *vede* ha ragione per costruzione, ed e' il criterio
> di evoluzione che questo progetto rifiuta.

### R4 — Le bugie sono invisibili dall'interno, i muri no

Un muro si trova esplorando. Una risposta fluente e sbagliata no — perche'
esplorando si chiede cio' che si sospetta, e nessuno sospetta cio' che sembra
funzionare. «cosa e' l'arrocco negli scacchi» rispondeva benissimo, e di
un'altra cosa.

I tre reperti incollati da F. hanno prodotto tre difetti veri, **uno dei quali
un misclaim**. Un'ora di esplorazione autonoma non l'avrebbe trovato.

> **Ricetta:** una sessione si apre su un transcript reale, e lo si legge
> cercando le risposte **fluenti**, non i muri. Un muro e' gia' una voce di
> to-do; una bugia e' un difetto nascosto. Se il transcript non c'e', il primo
> atto e' produrlo — dieci turni nel dominio del giorno, incollati grezzi.

### R5 — Le convinzioni del progetto scadono, e sono scritte come se non scadessero

`verb_suffix/1` e' rimasta a un membro solo per settimane, con accanto la
ragione misurata che l'aveva resa vera — e che un cambiamento successivo aveva
gia' invalidato. La lore «troppe variabili» era mia, era falsa, e ci sono
ricascato in un secondo file. Due handoff di questa stessa giornata portavano
diagnosi sbagliate, entrambe mie, scritte con la sicurezza di chi ha appena
visto il sintomo.

Il problema non e' che ci si sbagli. E' che **una frase scritta nel codice
diventa legge**, e nessuno la ridata.

> **Ricetta:** ogni sessione **ridata una convinzione ereditata**, scelta fra
> quelle che bloccano qualcosa. Costa pochi minuti e oggi ha sbloccato una
> classe intera. E ogni «misurato» che si scrive porta la generazione, cosi' chi
> lo legge sa quanto e' vecchio.

### R6 — Accoppiamento forte si intreccia, accoppiamento debole si accumula

Questa e' l'intuizione di F. sui test, generalizzata. F.: *«mi ero accorto che
l'impatto della crescita non rompeva i test in maniera sostanziale, quindi ho
deciso di fixare i test solo dopo alcune sessioni di apprendimento»*.

E' una decisione di **batching basata su una proprieta' osservata**: fra
crescita della KB e rottura dei test l'accoppiamento e' *debole*, quindi i due
processi possono girare a frequenze diverse. Verificato oggi: su sette file
controllati, la crescita di una giornata intera ha spostato **due** assert, e
uno dei due era il test ad avere ragione vecchia.

La regola generale che ne segue:

> **Intreccia cio' che e' fortemente accoppiato a quello che hai appena
> cambiato. Accumula tutto il resto.**

| accoppiamento | quando |
|---|---|
| **forte** — si fa subito | il differenziale sul file toccato; il replay dopo una lezione; il censimento dei `PARSE ERROR` dopo aver scritto `.p0` |
| **debole** — si accumula | riallineare le suite; potare i registri; rivedere le regole indotte; ripulire `savemap` |

Il costo di sbagliare verso: intrecciare cio' che e' debole spende tempo su
rumore; accumulare cio' che e' forte lascia entrare un difetto che poi si cerca
per un'ora.

### R7 — Una lezione che conferma il falso corrompe il maestro

`"come sei messo" is another way to say "come stai"` → **«Got it»** → e il turno
dopo murava. Per un sistema che impara parlando questo e' il guasto peggiore,
peggiore di un rifiuto: non sbaglia una risposta, sbaglia **il modello che il
maestro ha dello studente**. Da quel momento si insegna sopra una base falsa.

> **Ricetta:** dopo ogni lezione, il **replay e' il turno immediatamente
> successivo**. Non a fine sessione, non «poi verifico»: subito. E una lezione
> che non cambia niente deve dirlo.

---

## 2-bis. I tentativi falliti: che cosa se ne tiene, e in che forma

*F.: «i tentativi che non producono effetti positivi dovrebbero rimanere in KB
come un monito a non sbagliare — ma potrebbero in condizioni diverse essere vie
corrette per un comportamento alternativo. La cosa e' molto delicata».*

E' delicata, ed e' lo stesso principio di `keep-secondary-structures` applicato
non alle strutture ma ai **tentativi**: non si pota cio' che oggi perde, perche'
la selezione ha bisogno di alternative su cui lavorare.

### La prova, fatta contro me stesso

Al `gen505s` ho scartato una via — generare gli schemi dalla **radice** tramite
`verb_root/1` appoggiata a `verb_stem/2` — e l'ho annotata con due numeri:
boot **13,7 s** con la guardia `naf`, **15,2 s** senza, contro i 4,53 s della
via scelta. Sembrava una refutazione solida.

**Rimisurata il giorno dopo, sullo stesso albero:**

| | ieri | oggi |
|---|---|---|
| la via scartata | **15,2 s** | **7,44 s** |
| la stessa, con `verb_root` dichiarata vista materializzata | *non esisteva* | **5,66 s**, ma perde soluzioni |
| la via scelta | 4,53 s | 3,67 s |

Nella stessa sessione in cui l'avevo scartata avevo poi corretto l'ordine di
costruzione delle viste — e quella correzione ha **dimezzato** il costo della
via che avevo appena dichiarato troppo cara. Il verdetto tiene ancora, ma per
una ragione **diversa** da quella scritta: oggi non e' la rienumerazione, e' che
`verb_root` come vista materializzata perde soluzioni («luca watches a film»
smette di funzionare) — che e' lo stesso limite gia' sospettato per
`plural_noun/1`.

> Un tentativo scartato invecchia su **tre assi indipendenti**, e confonderli e'
> l'errore: il **verdetto**, la **ragione**, il **numero**. Qui il verdetto ha
> retto, la ragione e' cambiata e il numero si e' dimezzato in 24 ore.

### Le tre specie, che vanno tenute distinte

**(a) Refutazione di principio** — non scade. «Restringere l'ingresso cambia
quale *relazione* vince, non solo quale soggetto»: e' una conseguenza di come
funziona lo scorer, non della KB di oggi. Si puo' scrivere come regola.

**(b) Refutazione di circostanza** — scade, e in fretta. «Costa 15,2 s» dipende
dal motore di quel giorno. **Deve portare la condizione che la reggeva**, perche'
cio' che si ricontrolla poi e' la *condizione*, non l'esperimento.

**(c) Tentativo giusto ma prematuro** — il piu' prezioso e il piu' pericoloso da
archiviare come fallimento. `verb_root` e' esattamente questo: l'**idea** e'
migliore di quella in uso (genera solo forme corrette, niente «absorbss»), e
perde solo perche' il meccanismo delle viste non regge una vista che non si
enumera per intero. Il giorno che quel meccanismo cresce, quella diventa la via
giusta — e sarebbe un danno averla registrata come «sbagliata».

E c'e' il caso che F. solleva, che sta di traverso a tutti e tre: **una via
sbagliata per un comportamento puo' essere quella giusta per un altro.**
Restringere l'ingresso e' sbagliato per *rispondere*; potrebbe essere
esattamente giusto per *proporre*, o per una modalita' di interrogazione
ristretta. Registrarla come «vietata» chiuderebbe un uso legittimo mai esaminato.

### La forma: mai un divieto, sempre un fatto con la sua condizione

Un divieto e' un'affermazione **sul futuro** e diventa legge: e' esattamente il
meccanismo con cui `verb_suffix/1` e' rimasta ferma per settimane. Una misura e'
un'affermazione **su un momento**, e si puo' ridatare.

E parrot0 ha gia' il vocabolario giusto per dirlo — e' il template da cui questo
intero arco e' partito:

> *«Not proved is not the same as false.»*

Un tentativo fallito e' quasi sempre **non dimostrato**, non **falso**. Quindi
la forma da scrivere e':

```
tentativo   che cosa si e' provato
verdetto    scartato / tenuto              ← invecchia lentamente
ragione     perche'                        ← invecchia
condizione  che cosa era vero quel giorno  ← e' cio' che si ricontrolla
specie      principio | circostanza | prematuro
```

La **specie** dice quanto spesso rileggerlo:

- `principio` → quasi mai; e' una regola, sta fra le ricette del §2;
- `circostanza` → si ricontrolla **la condizione**, non l'esperimento, ed e'
  quasi sempre una domanda da un minuto;
- `prematuro` → si ricontrolla **se il bloccante e' caduto**. Questi vanno
  guardati per primi quando si tocca il meccanismo che li bloccava.

### Dove vanno: al sito, non in un cimitero

Un elenco centrale di errori passati non lo legge nessuno. Un commento **nel
punto di codice che sta per essere riscritto** lo legge chi sta per ripetere lo
sbaglio, ed e' gia' la pratica del progetto — il difetto misurato non e' che
manchino, e' che **non portano ne' la condizione ne' la data** (R5).

⚠ E **non nella KB di runtime**: la storia ingegneristica del progetto non e'
conoscenza del mondo ne' della lingua, e ogni fatto in `kb/` si carica al boot e
si scandisce a ogni turno. Sarebbe peso morto in ogni risposta. L'unica ragione
per portarcela sarebbe volere che parrot0 ragioni sulla **propria** storia di
progettazione — un'ambizione legittima e separata, da decidere apposta e non
come effetto collaterale di questa regola.

---

## 2-ter. La strategia: massimizzare e declinare *(F.)* — **mantra #22**

*F.: «quando troviamo un circuito che funziona, un percorso, una soluzione su cui
si sono investite ore, allora la dobbiamo **massimizzare** — riempirla di casi
completi, varianti, temi lunghi di ogni genere — e **declinare**: quell'abilita'
ne puo' portare un'altra simile, sovrapponibile, con terminazioni alternative e
interconnessa ad altre. Cosi', dato un investimento x di tempo, la crescita
della KB e' rilevante».*

E' la strategia giusta, e il motivo per cui funziona e' l'asimmetria che il
§1-bis misura: **il circuito e' la parte cara, i casi sono la parte gratis.**
Il `turn_focus` e' costato nove cicli su due sessioni; il decimo caso che ci si
mette dentro costa una riga. Chi si ferma al primo caso paga il prezzo pieno del
circuito e ne raccoglie un nono.

E si incastra con il §1 in modo che rende entrambi piu' precisi:

> **Massimizzare** = esaurire i **casi** di una distinzione.
> **Declinare** = trovare la **stessa forma** di distinzione altrove.

### La dimostrazione: quanto e' pieno il circuito che ho lasciato ieri

Costruito `turn_focus`, verificato su un caso, e passato oltre. Rimisurato:

| turno | esito |
|---|---|
| `…arrocco **in** chess` | ✅ muro onesto |
| `…arrocco **within** chess` | ✅ |
| `…arrocco **among** the openings` | ✅ |
| `…arrocco **under** the rules` | ✅ |
| `**tell me about** the arrocco in chess` | ✅ — regge anche una forma di domanda diversa |
| `…arrocco **for** chess` | ⛔ **mente ancora**: definizione degli scacchi |
| `…arrocco **of** chess` | ⛔ **mente ancora** |
| `…arrocco **during** a game` | ⚠️ «I can't show that» — una **terza** facolta' |
| `cosa e' l'arrocco **nel gioco degli** scacchi` | ⛔ muro (annidamento italiano) |

Cinque casi funzionano **gratis**, per una proprieta' che vale la pena notare:
`among` e `under` non sono in `domain_preposition/1` e passano lo stesso, perche'
la **verifica** rifiuta il topic anche quando il fuoco non e' stato tagliato.
E' il dividendo di «verificare a valle invece di filtrare a monte» (R3): degrada
con grazia invece di rompere.

E quattro no. **Averli lasciati li' e' esattamente l'errore che questa strategia
corregge**: il difetto successivo e' sempre piu' attraente del finire quello in
corso.

### Le due discipline che la tengono in piedi

Senza queste, massimizzare e declinare degenerano nei due difetti che il
progetto conosce gia' per nome.

**Massimizzare la CLASSE, non i casi.** Aggiungere membri e' giusto solo se
condividono un **criterio dichiarato** che uno straniero potrebbe estendere
domani. `domain_preposition/1` lo e'. Aggiungere una riga per sintomo e' come
sono nate le 73 cue corte, ed e' «l'elenco degli incidenti».

> Il test: *il membro nuovo entra in una classe, o e' un caso in piu' in una
> lista?* Se e' una lista, il circuito non era finito — mancava la classe.

**Una declinazione RIUSA la lettura, non la copia.** «Simile ma non esatta,
sovrapponibile» e' proprio il punto in cui un meccanismo si allunga fino a
mezzo-funzionare in cinque posti. Il segnale che si e' sbagliato: ci si trova a
scrivere un **secondo** meccanismo parallelo. Quella non e' una declinazione, e'
un duplicato (mantra #5), e divergera' al primo cambiamento di uno dei due.

> Il test: *sto aggiungendo un consumatore a una lettura che c'e', o sto
> scrivendo una seconda lettura?*

### Quando un circuito e' pieno

Serve una condizione d'arresto, altrimenti si massimizza per sempre. La sola
onesta e' comportamentale:

> **quando i casi nuovi smettono di cambiare il comportamento su un transcript
> tenuto da parte.**

Non «quando la classe sembra completa» — nessuno sa che aspetto ha una classe
completa dall'interno. `for` e `of` restano fuori non perche' la classe sia
piena, ma perche' non sono ambiti: `of` fa parte del soggetto («the capital of
France»), e chiuderli chiede una lettura diversa — *qual e' la testa del
sintagma*. Che e' una **declinazione**, non una massimizzazione: circuito nuovo,
prossima sessione.

### ⭐ E la conseguenza che vale piu' di tutte

**Massimizzare e' il momento in cui il lavoro smette di essere ingegneria e
diventa insegnamento.** Costruire il circuito richiede il motore, il C, i cicli
diagnostici. Riempirlo e' conoscenza: si fa **parlando**, ed e' il punto
dell'intero esperimento.

Cioe' questa strategia separa da sola i due modi che abbiamo mescolato per tutta
la sessione:

| | costruire | massimizzare / declinare |
|---|---|---|
| che cos'e' | ingegneria | addestramento |
| quanto costa | ore, cicli diagnostici | minuti, per voce |
| dove si scrive | `src/` + `kb/` | `kb/`, spesso solo parlando |
| quante volte per sessione | **una** | fino a saturazione |
| protocollo | questo documento | `LEARN_PROTOCOL.md` |

### Il ritmo di una sessione che ne segue

1. **un circuito per sessione, al massimo** — e se ne saltano fuori due, il
   secondo si scrive, non si insegue;
2. **poi si massimizza, per voce, fino a saturazione** — ed e' qui che la KB
   cresce davvero, con il costo per caso vicino a zero;
3. **poi si cerca una declinazione**, e la si controlla con il test del riuso:
   se riusa la lettura si fa adesso, se chiede un meccanismo nuovo **diventa il
   circuito della sessione dopo**.

Cosi' l'investimento non finisce mai in un caso solo, e la coda del lavoro e'
sempre gia' scritta.

---

## 3. Che cosa vuol dire «andata bene», qualitativamente

Se la crescita e' fatta di distinzioni e non di righe, il conteggio giusto non
e' quante clausole sono entrate. E':

- **quante distinzioni** sono state fatte, e se ognuna ha un nome interrogabile
  (`predication_subject/1`, `turn_focus/2`, `inflection_suffix/1`);
- **quante capacita' gia' presenti** sono diventate raggiungibili parlando;
- **quante bugie** sono diventate muri onesti — questo conta piu' di quante
  domande in piu' hanno risposta;
- **quante convinzioni ereditate** sono state ridatate;
- e, in negativo: **zero conferme false**.

Una sessione che aggiunge duecento fatti e nessuna distinzione ha ingrassato la
KB senza far crescere la comprensione. Una che ne aggiunge quattro e separa due
concetti fusi ha fatto il lavoro.

---

## 4. La procedura, sei passi

> Per una sessione **mista** — comprensione, metacomprensione, KB viva — cioe'
> quella che facciamo davvero. Per l'insegnamento puro resta
> `LEARN_PROTOCOL.md`, che questo documento non sostituisce.

**0. Aprire su un reperto.** Un transcript reale. Leggerlo cercando le risposte
fluenti, non i muri (R4).

**1. Fissare la baseline una volta.** Zero `PARSE ERROR`, il boot annotato, i
quattro-sei `.p0t` del dominio con i loro rossi **scritti**. Da qui in poi «e'
rosso» non e' un'informazione: lo e' «e' rosso *diversamente*».

**2. Guardare prima di ipotizzare.** Al massimo **una** ipotesi prima di aver
letto la strada del turno (R1). Se le sonde non bastano, se ne aggiunge una: una
riga di KB, non una `fprintf`.

**3. Chiedersi sempre per prima: «quale capacita' esiste gia' e non si
raggiunge?»** Otto su otto, questa sessione. E provare la cura **in KB**: se
funziona hai finito senza compilare; se non e' esprimibile, *quello* e' il
risultato — hai trovato il confine del motore senza pagarlo.

**4. Se serve il C: verificare a valle, non filtrare a monte** (R3). E tenere
le tre regole d'arresto del §1-bis, che scattano proprio quando ci si sente
vicini alla soluzione:
seconda facolta' che ripete l'errore → si sale di livello;
secondo tentativo della stessa forma di cura → si cambia forma;
danno collaterale su un caso non correlato → si annulla e si sale.

**5. Verifica differenziale sul solo dominio toccato.** Se un rosso compare,
`git stash` e rimisurare **prima** di accusare il proprio codice: due volte su
tre oggi il rosso era preesistente, e la terza volta era una scoperta. Il
riallineamento delle suite si accumula (R6).

**6. Chiudere il registro invece di farlo crescere.** Un solo handoff vivo per
file. Ogni «misurato» con la sua generazione. Ogni diagnosi con la sonda che la
falsifica — o marcata `da riverificare` (R5).

---

## 5. Le tre cose che, se cambiate, accelerano tutto il resto

In ordine di quanto tolgono **cicli**, non secondi.

1. **Le sonde di `/debug` come primo riflesso, non come ultimo.** Sono l'unico
   strumento che accorcia la diagnosi invece delle operazioni: costano una riga
   di KB e raccontano il caso intero. Le tre righe di `turn_focus` dicono in un
   colpo che cosa il turno chiedeva e chi si e' ritirato — cioe' esattamente
   cio' che al ciclo 3 dell'arrocco non sapevo e che mi e' costato cinque cicli.
   **Ogni difetto chiuso dovrebbe lasciare dietro di se' la sonda che lo
   avrebbe trovato.**
2. **Un gradino di verifica che stampi il *delta* su una baseline**, invece di
   un elenco di rossi da interpretare. Il differenziale a mano l'ho fatto sei
   volte in una sessione: e' l'unico modo onesto di dire «questo rosso e' mio»,
   e finche' costa un gesto manuale si tende a saltarlo — cioe' a confondere
   danno collaterale (che e' diagnosi, §1-bis) con rumore.
3. **Spezzare l'unita' di traduzione.** Vale poco in secondi — venti
   ricompilazioni fanno sei minuti — ma toglie l'unico incentivo strutturale a
   **ragionare invece di provare**. Quando un tentativo costa quanto un
   pensiero, si tentano le ipotesi invece di sceglierle: e' il modo in cui si
   fanno tre cicli della stessa forma sbagliata.

---

## 6. Se se ne ricorda una sola

> **Non stiamo insegnando cose a parrot0: stiamo separando cose che teneva
> insieme.** E il tempo non se ne va nelle operazioni, se ne va nei cicli
> diagnostici — quindi il guadagno non e' fare i cicli piu' in fretta, e'
> **farne di meno**: guarda prima di ipotizzare, fermati alla seconda facolta'
> che ripete l'errore, e tratta cio' che rompi come diagnosi, non come attrito.

---

## 7. Le misure meccaniche, e perche' contano poco

⚠ **Da leggere sapendo il §1-bis.** Tutto cio' che segue, messo insieme, fa
minuti; un solo caso mal diagnosticato ne fa ore. Questi numeri servono a due
cose sole, non a una terza:

1. a decidere **in che ordine** provare le cose — il ciclo KB non ha
   compilazione, quindi va per primo, e non per velocita' ma perche' un
   tentativo che costa zero si puo' fare *prima* di aver deciso se e' giusto;
2. a giustificare **una** modifica strutturale (spezzare l'unita' di
   traduzione), che vale non per i secondi che risparmia ma perche' toglie
   l'incentivo a ragionare al posto di provare.

Non servono a stimare la durata di una sessione. Quella la decide il numero di
cicli diagnostici, e i cicli li decide la qualita' della diagnosi.

### La sessione del 6 settembre

| | |
|---|---|
| commit | 12 |
| finestra attiva | ~4,2 h |
| righe di documentazione | **890** |
| righe di KB | **533** |
| righe di C | **527**, di cui **197 (42%) commento** |

Il codice vero scritto in una giornata piena e' **~277 righe**. Tutto il resto e'
conoscenza e spiegazione. Le 890 righe di documentazione **non sono spreco**:
sono il motivo per cui questa analisi si puo' fare — i commit contengono le
misure, le strade sbagliate e il perche'. Il difetto non e' scriverne troppa: e'
non **datarla** (R5) e non **potarla**.

### Il costo di un'operazione

| operazione | costo |
|---|---|
| ricompilare dopo **una riga di C** | **18,3 s** |
| ricompilare dopo una modifica **solo KB** | **0 s** — non serve |
| sonda CLI (boot + un turno) | 3,1 s |
| avvio del demone di test | 3,7 s |
| un `.p0t` mirato | 4,3 s |
| `make soft-test` | **32 s** (budget dichiarato: 15 s) |
| suite intera | minuti, fail-fast, 4 rossi preesistenti |

I 18,3 s hanno una causa strutturale: `src/brain.c` include tutti i tredici
`brain/*.c`, e l'unita' di traduzione e' di **53.605 righe**. Toccare
`70-social-pragma.c` (1.467 righe) ricompila anche le 17.706 di
`10-memory-knowledge.c`.

Rimessi nella scala giusta: **~20 ricompilazioni fanno 6 minuti**, contro le ore
del caso dell'arrocco. Il numero serve solo a dire *in che ordine* provare —
prima cio' che non compila — e a giustificare la separazione dell'unita' di
traduzione, che vale perche' toglie l'incentivo a ragionare invece di provare.

### Le convinzioni non datate

```
«misurat*» nei commenti di kb/ e src/     76
di cui con una generazione o una data      5
numeri di prestazione citati              43
```

E' la misura di R5. Il caso concreto: `verb_suffix/1` fermo a un membro con
accanto la ragione — «con -ed e -d il budget di un TURNO passava a 1,85 s» —
vera quando fu scritta, resa falsa dal `gen491` che ha spostato quel costo al
boot, e mai ridatata.

### L'accoppiamento crescita → test, verificato

`LEARN_TODO.md`: **7.355 righe**, +1.024 (**+16%**) in un giorno, **6 handoff
impilati** di cui uno vivo. E' la misura di cio' che va potato.

Sui test: su sette file controllati, la crescita di una giornata intera ha
spostato **due** assert, e in uno dei due era il test ad avere ragione vecchia.
E' la conferma quantitativa dell'intuizione di F. su cui poggia R6 —
l'accoppiamento fra crescita della KB e rottura dei test e' **debole**, quindi
i due processi possono girare a frequenze diverse.
