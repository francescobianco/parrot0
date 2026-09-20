# Train the Learning Process — far crescere la capacità di essere addestrato

> **La missione non è insegnare qualcosa a parrot0. È far crescere le strutture
> con cui una frase detta lo modifica** — finché la prosa di un maestro non è
> più un suggerimento, ma **un atto che davvero addestra**.
>
> Si agisce su **quattro elementi insieme**: la **KB viva**, la **IR**, la
> **comprensione universale** e il **mondo allargato**. Un piano che ne tocca
> uno solo si ferma: è il reperto comune di tutti i tentativi precedenti,
> consolidati qui.

Aperto il 21 settembre 2026 su richiesta di F. Consolida i tentativi sparsi in
`docs/plans/` (§2) e definisce l'indicatore **learning-capability** (§4).

---

## HANDOFF — 21 settembre 2026, notte: SI RIPRENDE DA QUI

**learning-capability ≈ 60–65.** Dentro la banda 61–75, non oltre. Il percorso
della giornata, con le misure: **30 contato** dal censimento (§4.5-bis) → 50
(pavimento risalito, condotta insegnabile) → 60–65 (la lettura estensibile
parlando).

### Che cosa è vero adesso, e come si verifica in un minuto

```sh
# un GENERE di indizio grammaticale nuovo, insegnato parlando
printf '%s\n' 'the reading hedge is of kind modality' \
  'the expression forsooth marks hedge' 'forsooth the cat sleeps' '/debug' '/quit' | \
  PARROT0_SESSION= PARROT0_LANG=en PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0 | \
  grep debug_grammatical_cue
# → evidence(span(1, 1), modality, hedge)   accanto a quelle native
```

| capacità | dove | prova |
|---|---|---|
| un **genere di lettura** nuovo costa una lezione | `kb/core/taught-reading-kind.p0` | insegna, trasferisce a una seconda espressione, si ritira, compare nell'ispettore |
| la **condotta** si insegna e si ritira | `kb/core/conduct-lessons.p0` | la guardia di pertinenza, con trasferimento alla classe |
| «una forma ha concluso» è **interrogabile** | `turn_form_concluded/2` | distingue la lezione riuscita dal quasi |
| la cessione si decide **dopo** le forme | `mod_knowledge` | una condotta può guardare che cosa il turno *non* è riuscito a essere |
| una **regola KB può contribuire un nodo** alla IR | misurato, non ancora usato | `input_node(...) :- …` è visto dai consumatori |

### ⛔ L'esperimento del §6.5, fatto e FALLITO — leggere prima di ritentarlo

Il piano dice che le riscansioni sono il collo, e che finché durano la banda è
irraggiungibile. **La prima ipotesi era che bastasse un cambio solo**, e aveva
una base misurata: `split_words` è **una funzione sola**, quindi i suoi 349
chiamanti concordano già fra loro; il disaccordo è fra *quel* confine e quello
della IR. Cambiare il confine in un punto avrebbe fatto leggere a tutti lo
stesso testo.

**Provato.** `split_words` allineato al confine della IR (caratteri di parola,
più il decimale). **Misurato:**

| cancello | esito |
|---|---|
| `soft-test` | verde, un turno a 1,25 s |
| `facts`, `derivation`, `clause_content` | **tutti verdi** |
| `english_grammar_growth`, `taught_lexicon` | solo tempi, nessuna asserzione rotta |
| **piolo r300 della prosa** | **45/62 → 6/62** |

**Il banco dei `.p0t` non protegge il lettore di prosa.** Ogni suite era verde
e la comprensione era crollata dell'87%: le suite fanno turni corti, la prosa
no. **Revocato**, e r300 riverificato a 45/62.

**Che cosa se ne impara, e vale più del tentativo.** I consumatori delle
riscansioni **dipendono dalla semantica a spazi bianchi**: non si unificano
cambiando il confine, vanno tolti **un consumatore alla volta**, e ogni passo
va misurato **con il banco della prosa**, non con i `.p0t`. Il §6.5 resta il
lavoro che apre la banda, e ora si sa come non farlo.

### Da dove ripartire, in ordine

1. **Il §6.5, un consumatore alla volta.** Scegliere un `split_words` che
   *decide* qualcosa, sostituirlo con un consumatore della IR, e misurare
   r300 **prima e dopo**. Il gate è il banco della prosa; i `.p0t` non bastano.
2. **Il limite del nome multiparola** (`taught-reading-kind.p0`): un genere con
   un nome di due parole insegna ma non raggiunge la IR — l'atomo quotato non
   sopravvive dentro il termine della vista `expression_first_word`. Isolato,
   non curato.
3. **Il rilevatore del «quasi una lezione»** è collegato e funziona; restano i
   suoi due falsi negativi misurati (testo fisso parziale, sinonimi).
4. **18 dei 30 elementi del censimento non sono stati misurati**: la quota di
   catene che finiscono a mano resta una stima inferiore.

### Le due trappole pagate oggi, da non ripagare

- **`naf` con una variabile libera flounderà**, quindi la clausola non scatta
  mai e sembra che la regola non esista. Mi ha morso **due volte**. Le guardie
  si scrivono come facce unarie su termini legati.
- **Una forma di lezione che comincia con un jolly** non può avere una gemella
  di ritiro distinguibile: «forget that X is …» viene letta dalla forma di
  insegnamento con nome «forget that X». L'apertura dev'essere testo fisso.

---

## ⇨ TODO PRIORITARI — da qui si comincia

Cinque attività **puntuali** (T1–T5): ognuna ha un esito binario, un costo in minuti e
una prova che la falsifica. Nessuna è una scansione della KB o un'indagine a
largo spettro — quelle si fanno dopo, e il §6 dice in quale ordine.

| # | attività | elemento (§3) | livello (§4.2) | passo del §6 | costo |
|---|---|---|---|---|---|
| ~~**T1**~~ | ✅ **fatta il 21 settembre** — le tre forme italiane insegnano; trovato e chiuso un guasto del canale (policy incompleta accettata che rompeva il turno dopo) | comprensione universale | L2 | 1 | ~20 min |
| ~~**T2**~~ | ✅ **fatta il 21 settembre** — declino informato su una forma: tre stati, tre messaggi, ognuno dice che cosa scrivere dopo | comprensione universale | — (il pavimento) | 2 | ~1 h |
| ~~**T3**~~ | ✅ **fatta il 21 settembre** — la guardia di pertinenza si insegna, trasferisce alla classe e si ritira. **Prima lezione L4** | KB viva | **L4** | 3 | ~1–2 h |
| ~~**T4**~~ | ✅ **fatta il 21 settembre** — la lingua di uno scambio aperto è quella dell'ancora; un seguito corto non la sposta più | KB viva | L4 | 1 | ~45 min |
| ~~**T5**~~ | ⚠ **misurata, non curata il 21 settembre** — il costo è attribuito (il boot, 5,7 s) e una cura è stata provata e ritirata perché dannosa; vedi la scheda sotto | — (banco) | — | 0 | ~1 h |

> **I cinque TODO sono chiusi**, con T5 chiusa come *misura* e non come cura:
> la sua scheda qui sotto dice perché, e sposta la domanda dal banco al boot.
> Il §5 riporta dove l'ago si è mosso e che cosa tiene il numero sotto 50: una
> coppia `situazione × comportamento` sola, la precedenza fra facoltà ancora
> scritta, e il censimento del §4.5 mai eseguito.

---

### T1 — Provare le tre forme di lezione italiane mai verificate

[`LEARN_PROTOCOL.md`](../../LEARN_PROTOCOL.md) dichiara quattro forme italiane
*«non ancora verificate end-to-end»*. **Una** è stata provata il 20 settembre
(`quando dico X intendo Y`) e funziona, con ablazione. Le altre tre no:

```text
la mossa conversazionale partenza concreta riguarda un esempio concreto
per la mossa conversazionale partenza concreta di Mostrami un esempio.
quando guidi apprendimento inizia con partenza concreta
```

**Perché per prima.** È il §6.1 — *guarire il canale prima di allargarlo* — e ha
l'esito più netto che esista: ogni forma o insegna, o riceve un muro, o la
prende il lettore sbagliato. Nessuna interpretazione. E i difetti, se ci sono,
non vanno **cercati**: li ha già nominati il catalogo.

**Prova.** Tre righe di tabella: replay, trasferimento a un altro argomento,
ritiro, replay. Verde = un sospetto in meno sulla banda. Rosso = tre difetti
localizzati senza aver scansionato niente.

### T2 — Declino informato su **una** forma di lezione

Oggi una lezione non riconosciuta riceve `Non capisco ancora.`, e il maestro non
sa se ha sbagliato la frase, il nome della mossa, o se la forma non esiste. Si
prende **una** forma e le si fa dire che cosa le mancava:

> *«sembra una lezione sulla condotta, ma non riconosco «partenza concreta»
> come nome di mossa»*

**Perché.** È il canale che **parla all'indietro** (§3): senza, l'addestramento
è un imbuto. Il §6.2 lo vuole sul catalogo intero; qui si restringe a **una**
forma per vedere se il meccanismo regge prima di generalizzarlo.

**Prova.** Si sbaglia la lezione in tre modi diversi e si ricevono tre messaggi
diversi, ognuno dei quali dice che cosa scrivere al turno dopo.

**Rischio dichiarato.** Potrebbe non esistere un punto unico a cui appendere il
messaggio. Se è così **si dice**, invece di forzarlo: sarebbe un reperto sulla
comprensione universale, non un fallimento dell'attività.

### T3 — La guardia di pertinenza sul turno che fallisce

Non costruire F1. Rendere **dicibile** questo, e basta:

> *«Quando il tema di una domanda è un turno di questa conversazione, non
> rispondere con l'elenco delle cose che sai.»*

**Perché.** Il turno di partenza — `in quale lingua ti ho chiesto quale lingua
sai parlare` → **`c, python.`** — non deve diventare *«in italiano»*: deve
diventare un **muro onesto**. Sulla scala di F. è un salto più grande di una
risposta giusta presa dal frasario, perché toglie la specie peggiore, *fluente e
infondato*. Ed è **L4** — la banda dove parrot0 è più debole — al costo di un
caso solo. La forma di seme è quella che il `/debug` di parrot0 già nomina da sé.

**Prova.** Il turno cambia; il **ritiro** lo riporta a `c, python`; e una
domanda diversa della stessa famiglia riceve lo stesso trattamento **senza una
seconda lezione**.

### T4 — La lingua che salta a metà scambio

```text
>>> sono indeciso tra leggere e passeggiare
Su leggere e passeggiare: Prima di scegliere, nomina un criterio…
>>> per me conta riposare
You added: riposare. On leggere e passeggiare: Use this criterion…
```

Rosso misurato e **pre-esistente** (identico sulla baseline), su un asse
dichiarato: *continuità dopo un seguito*. È il rosso 1 dell'handoff di
[`the-rational-philosopher.md`](the-rational-philosopher.md), e **la leva
esiste già**: la lingua dell'ancora è registrata, quindi la resa può leggere
quella invece di indovinare su quattro parole ambigue.

**Prova.** Un seguito corto non sposta più la lingua; un cambio di lingua
**esplicito** continua a funzionare.

### T5 — Il costo di `soft-test`

Il banco sta a **12–14s su 15 di budget** e ha toccato 16 con un fallimento
durante la sessione del 20 settembre. Misurare per file contro un worktree di
baseline, trovare il costo vero, curarlo. Le leve candidate sono già elencate
nell'handoff di [`the-rational-philosopher.md`](the-rational-philosopher.md).

**Perché non è manutenzione.** Finché il banco fallisce a intermittenza, **ogni
misura successiva è contestabile** — comprese quelle che direbbero se le
attività T1–T4 hanno funzionato. È la meno creativa e la più abilitante.

**Prova.** Cinque run consecutive sotto i 12s, e il delta attribuito a una
modifica precisa invece che al rumore.

---

### T5 — il costo di `soft-test`: misurato e attribuito, non curato

**Misura, 21 settembre 2026.** Cinque esecuzioni consecutive: **12, 12, 12, 11,
11 s** su un budget di 15. Stabile, nessuna intermittenza osservata — ma la
metà del budget ha un nome.

| file | costo | quota |
|---|---|---|
| `tests/p0t/knowledge/facts.p0t` | **9,3 s** | **80%** |
| `tests/p0t/conversation/basics.p0t` | 1,8 s | 15% |
| `tests/p0t/health.p0t` | 0,47 s | 4% |

**Dentro `facts.p0t`, il costo è uno solo.** Nove turni valgono ~3,5 s; il
`!reset` in testa al file ne vale **5,8**, misurato isolandolo in un file che
contiene solo le direttive e nessun turno. Il `!reset` è già «smart» — salta se
la configurazione non è cambiata e nulla è stato insegnato — ma qui la
configurazione cambia, quindi ricarica.

**E la ricarica è un boot.** Un boot a freddo, a vuoto, costa **5,7 s** su
156.451 fatti e 4.950 regole. La memoria di progetto registrava **0,40 s**:
il boot è cresciuto di circa **quattordici volte** insieme alla KB. Non è il
banco a essere lento: è l'avvio, e il banco lo paga una volta per file che
ricarica.

**Una cura provata e misurata DANNOSA, quindi ritirata.** Poiché ciò che un
test insegna vive negli strati di runtime, sembrava che un reset potesse
togliere per **origine** (`KB_SESSION|INDUCED|HYPOTHETICAL|REFLECTIVE|DERIVED`)
invece di rileggere il disco. Misurato: **15,8 s e cinque rossi**. Il livello
riflessivo non è scarto di sessione — contiene il modello di sé — e le viste
ricostruite costano più di quanto la ricarica risparmi. Revocata.

**Che cosa resta, con la sua evidenza.** La prova che T5 chiede — *cinque run
sotto i 12s con il delta attribuito a una modifica precisa* — **non è
raggiunta**: le run stanno a 11–12 s e nessuna modifica le ha spostate. Ma la
domanda è cambiata: non «perché il banco è lento» bensì **«perché il boot costa
5,7 s»**, che è una domanda sul motore e vale per ogni cosa, non per il banco.
Le leve candidate, in ordine di resa attesa:

1. **Il boot stesso** — è il 100% del costo di ogni ricarica. Vedi la memoria
   «KB growth degrades the engine»: lookup O(n) e un pass di boot quadratico.
2. **Una ricarica che conservi il livello curato** invece di rileggerlo: è ciò
   che la cura ritirata cercava di fare dalla parte sbagliata. Va fatta
   preservando il riflessivo e le viste, non buttandoli.
3. **`facts.p0t` che non cambi la configurazione**: provato, **non sposta la
   misura** (11 s con e senza le righe `!set`). Annotato perché non venga
   ritentato.

### Il resto, dopo

| | |
|---|---|
| **L'ordine strategico** | §6 — che cosa alza il numero, e perché ogni riga è la condizione della successiva |
| **La misura vera** | §4.5, il censimento delle catene. **È un'indagine a largo spettro**, ed è per questo che non sta qui: ma finché non si esegue, il `30–35` del §5 resta una stima e va detto ogni volta che lo si cita |
| **F1 — il turno come contenuto con un atto** | non è un'attività d'apertura: è il gradino su cui poggiano quattro facoltà di [`the-rational-philosopher.md`](the-rational-philosopher.md). Costruirlo prima che il canale regga significa costruirlo e non riuscire a insegnargli niente |
| **La tripletta di accettazione** | §4.4 — astrofisica, clone di un LLM, interprete PGN. Sono il bersaglio del 100, non il lavoro di domani |

---

## 1. Che cosa è «train-the-learning-process», e che cosa non è

### 1.1 La distinzione che fonda tutto

| | |
|---|---|
| **Addestrare parrot0** | insegnargli i minerali, i nodi, la grammatica inglese, le leggi fisiche. Cresce ciò che **sa**. |
| **Addestrare il processo di apprendimento** | far sì che una lezione possa cambiare cose che oggi **nessuna lezione può cambiare**. Cresce ciò che **può diventare**. |

Il primo è lavoro di dominio e si misura in copertura. Il secondo è lavoro di
struttura e si misura in **altezza**: fin dove, nello stack di parrot0, arriva
una frase detta in lingua naturale.

Un esempio dalla sessione del 20 settembre 2026 chiarisce la differenza meglio
di ogni definizione. Al turno

```
>>> in quale lingua ti ho chiesto quale lingua sai parlare
c, python.
```

si può reagire in due modi. **Insegnare la risposta** — e allora si è aggiunto
un fatto, e la domanda successiva formulata in un altro modo tornerà a
sbagliare. Oppure **accorgersi che manca un oggetto**: per parrot0 un turno
passato non è una cosa di cui si possa parlare, quindi «ti ho chiesto…» non ha
nulla a cui ancorarsi. Finché quell'oggetto non esiste, *nessuna* lezione su
quella famiglia può attecchire — si potrà solo memorizzare frasi.

**Questo piano lavora sul secondo modo.** Il primo è già coperto da
[`LEARN_PROTOCOL.md`](../../LEARN_PROTOCOL.md).

### 1.2 Perché è la missione giusta

Perché è l'unica che **compone**. Ogni dominio insegnato con lo stato attuale
delle strutture costa quanto il precedente. Ogni struttura nuova rende più
economici **tutti** i domini futuri, compresi quelli che nessuno ha ancora
nominato. È la stessa ragione per cui
[`procedura-crescita-kb.md`](procedura-crescita-kb.md) §1 conclude che non
stiamo accumulando, stiamo **distinguendo**: le otto abilità di una giornata
non erano otto fatti mancanti, erano otto distinzioni mancanti.

### 1.3 La regola anti-inganno, ereditata e non negoziabile

Da [`MANTRA.md`](../../MANTRA.md) — *anti-barare per l'apprendimento via prompt*:

> «Parlando» significa lingua naturale, non Prolog/P0 o una API serializzata nel
> testo. Se il teacher deve conoscere nomi di predicati interni, arità, tuple,
> `!assert`, MCP o la forma di `kb.assert`, non ha insegnato: ha **scritto nella
> KB attraverso un altro trasporto**. Quel risultato vale zero.

Il controllo operativo: *un esperto del dominio che ignora lo schema interno
saprebbe formulare la lezione?* Se no, ci si ferma e si amplia la
meta-comprensione; **non si espone la rappresentazione**.

E il secondo controllo, che vale specificamente per questo piano: **una lezione
che non si può ritirare non è una lezione.** L'ablazione fa parte della prova,
sempre. Senza, non si sa se il comportamento nuovo viene dalla lezione o dal
caso — e per questo progetto vale zero allo stesso modo.

---

## 2. I tentativi precedenti — che cosa hanno trovato, e che cosa resta valido

Questi documenti non sono superati: sono **i pezzi** di questo piano. Qui si
dice che cosa ciascuno ha stabilito e che cosa va ripreso.

### 2.1 Il metodo — come si trova il prossimo buco

| documento | che cosa ha stabilito | stato |
|---|---|---|
| [`radici-insegnabilita.md`](radici-insegnabilita.md) | **La catena di insegnabilità.** Per ogni abilità, quale superficie la insegna; e per quella superficie, quale la insegna a sua volta. Una catena finisce in una **radice** (primitiva del motore: legittima), in un **circolo** (una lezione che estende la propria forma: il caso migliore), o in una **riga a mano** (un buco: il prossimo lavoro). *«Una KB viva è una KB in cui ogni catena finisce in una radice o in un circolo.»* | **il metodo di misura di questo piano** |
| [`50-iterazioni-insegnabilita.md`](50-iterazioni-insegnabilita.md) | Il bersaglio non è far passare un prompt, è rendere insegnabile **la classe** a cui appartiene. 28 giri su 50 chiusi. | campagna aperta, 22 giri |
| [`fenomenologia-dei-difetti.md`](fenomenologia-dei-difetti.md), [`fix-patterns.md`](fix-patterns.md) | Le specie ricorrenti di guasto e le forme di cura, fra cui la **guardia di pertinenza** (dire quando una facoltà NON deve prendere il turno) | vivo |

### 2.2 Il livello KB — che cosa può contenere una lezione

| documento | che cosa ha stabilito | stato |
|---|---|---|
| [`teach-comprehension-via-prompt.md`](teach-comprehension-via-prompt.md) | La tesi: *ciò che parrot0 sa fare con una forma deve poter cambiare per effetto di una FRASE*. E la contro-tesi: se ogni frase-che-insegna richiede un pezzo di C che la riconosca, il canale-dialogo è un'illusione. | tesi vigente |
| [`teach-comprehension-via-mcp.md`](teach-comprehension-via-mcp.md) | Il gemello per l'altro canale, e i muri misurati: computazione ricorsiva, generalizzazioni defeasible | muri aperti |
| [`teachable-procedures.md`](teachable-procedures.md) | La conoscenza non è solo fatti: sono **trasformazioni**. Un interprete di riscrittura generico invece di un consumer C per costrutto. | realizzato in parte |
| [`due-strutture-kb-viva.md`](due-strutture-kb-viva.md) | **Definizione come espressione** e **fatto a ruoli aperti**: le due strutture che permettono di insegnare *relazioni fra relazioni* | prima realizzazione, non certificata |
| [`abstraction-ceiling.md`](abstraction-ceiling.md) | Che cosa è esprimibile come conoscenza **senza** nuovi primitivi C, e che cosa sta oltre il soffitto | mappa del possibile |
| [`insegnamento-super-umano.md`](insegnamento-super-umano.md) | Insegnare la **condotta** e rendere la IR programmabile da prosa. *«Da ora in poi al posto della parola NO dì la parola CAVALLO.»* | proposta, U0→U1 |

### 2.3 La IR — che cosa parrot0 vede, prima di capirlo

| documento | che cosa ha stabilito | stato |
|---|---|---|
| [`ir-e-predicato-variabile.md`](ir-e-predicato-variabile.md) | La IR combinata con `apply/2` — il predicato variabile — perché la lettura evolva senza che ogni evoluzione costi un ramo nel motore | forma d'arrivo |
| [`universal-input.md`](universal-input.md) | Lo scheletro gerarchico: token, span, ruoli, range — il materiale su cui tutto il resto si appoggia | motore |
| [`lettura-della-prosa.md`](lettura-della-prosa.md) §1 | **Il reperto più duro dell'intero repo:** la IR *esiste* ed è buona (albero con range di byte, ruoli, ~50 viste), ma i file di KB che la consumano sono **7**, contro **217** riscansioni `split_words` nei tre lettori maggiori. Ogni `split_words` riapre la frase con la propria idea di dove finiscono le cose. | **il collo di bottiglia** |

### 2.4 La comprensione universale — la legge che rende una lezione raggiungibile

Non è la IR, ed è l'errore da non fare: la IR è **l'oggetto** che il motore
pubblica, la comprensione universale è il **regime** che vincola chi lo usa.
Vive in [`universal-comprehension.md`](universal-comprehension.md), ed è
l'estensione operativa del manifesto [`kb-first.md`](kb-first.md).

| che cosa stabilisce | perché è decisiva **qui** |
|---|---|
| **Nessun muro cieco su una frase ben formata.** parrot0 sa estrarre la struttura di qualsiasi frase; strutture, ruoli e schemi d'intento vivono **nella KB**, non nel C | una lezione **è un turno**. Se una lezione detta in un modo non previsto riceve *«Non capisco ancora»*, il canale di addestramento è morto **prima** di arrivare alla KB |
| **Comprendere la forma ≠ saper rispondere.** Il muro cieco si sostituisce con il **declino informato**, che dimostra di aver letto la domanda e nomina l'anello mancante | è il **ritorno di informazione al maestro**. Un maestro che riceve *«non so nulla di Zembla»* sa che cosa dire dopo; uno che riceve un muro cieco non impara nulla dalla propria lezione fallita |
| **La forma rivela l'intento; l'intento dice che cosa servirebbe sapere** | è ciò che permette a una lezione di valere per la **classe** invece che per la frase: senza, ogni superficie nuova è una voce di frasario |
| **Le tre specie di lacuna** (§10): variante di superficie, costruzione mancante, forma telegrafica — con il test diagnostico che le distingue in tre turni. *Due su tre si chiudono senza mai vedere una chat* | è il **triage** del lavoro di insegnabilità: dice quali buchi si generano dalla struttura e quali vanno resi insegnabili parlando |
| **Il ramo sociale** (gen240): capire la forma vale anche quando non serve un fatto ma una mossa conversazionale | l'addestramento è una conversazione: il maestro corregge, insiste, cambia esempio. Anche quei turni devono essere compresi |

> **La frase che la lega a questo piano:** *«comprensione universale non vuol
> dire aver previsto tutto: vuol dire che ciò che è generabile dalla struttura
> è già chiuso, e ciò che non lo è si chiude parlando»* — §10.

### 2.5 Il mondo allargato — di che cosa parla ciò che viene detto

Cinque oggetti, costruiti il 20 settembre 2026 per la prosa e già eseguibili
(la mappa delle porte è in
[`the-rational-philosopher.md`](the-rational-philosopher.md) §4):

| oggetto | porta | che cosa permette |
|---|---|---|
| **contenuto** | `kb_clause/4` | menzionare senza credere |
| **atto** | `kb_act/3` + `act_layer/2` | *chi* ha fatto entrare un contenuto e a quale titolo |
| **contesto** | `holds_in/2`, `context-scope.p0` | posizioni che convivono senza cancellarsi |
| **giudizio** | `epistemic-status.p0` | positivo, negativo, entrambi, nessuno, ricerca incompleta |
| **derivazione** | `kb_derivation/4`, `supported_from_premises/1` | da che cosa viene, e che cosa cade se l'assunto cade |

**Quello che manca non è l'astrazione: è che quasi nulla è ancora un oggetto di
quell'astrazione** — e che nessuna lezione può aggiungerne uno.

### 2.6 I bersagli — dove si vede se il processo funziona

| documento | ruolo in questo piano |
|---|---|
| [`mimic-llm.md`](mimic-llm.md) | il bersaglio «clone di un LLM»: un profilo, pesi che fanno emergere la risposta, condotta imitata |
| [`motorize-the-class.md`](motorize-the-class.md) | *motorizza la classe, poi nutri il motore* — perché una tabella non scala e un motore sì |
| [`learning-mesh.md`](learning-mesh.md) | catene di addestramento su una KB condivisa: maestro → parrot0 A → parrot0 B |
| [`quanto-manca.md`](quanto-manca.md) | il precedente di una misura onesta: muri, risposte buone, **turni rubati** |

---

## 3. I quattro elementi, e perché vanno mossi insieme

```text
             ┌──────────────────────────────────────────────────────┐
     dice →  │  MONDO ALLARGATO   di che cosa si può parlare         │ ← qui nascono
             │  contenuto · atto · contesto · giudizio · derivazione │   i generi di cosa
             ├──────────────────────────────────────────────────────┤
             │  IR                che cosa parrot0 vede in un turno  │ ← qui si decide
             │  nodi · ruoli · range · forza · lingua                │   se c'è appiglio
             ├──────────────────────────────────────────────────────┤
             │  KB VIVA           che cosa ne fa                     │ ← qui vive
             │  fatti · forme · procedure · condotta                 │   la lezione
             └──────────────────────────────────────────────────────┘
      ╔══════════════════════════════════════════════════════════════════╗
      ║  COMPRENSIONE UNIVERSALE — la legge che attraversa tutti e tre:   ║
      ║  nessun muro cieco su una frase ben formata; la forma rivela      ║
      ║  l'intento; ciò che manca si NOMINA invece di tacere.             ║
      ╚══════════════════════════════════════════════════════════════════╝
```

I primi tre sono **strati**: ognuno è un posto dove stanno delle cose. Il
quarto non è uno strato, è un **regime** — la disciplina che attraversa gli
altri tre e dice come devono comportarsi. Disegnarlo come un quarto piano
sarebbe comodo e falso.

**La regola di dipendenza, che è il cuore di questo piano:**

> Una lezione può cambiare solo ciò che la **KB viva** sa esprimere; la KB può
> esprimere solo ciò su cui la **IR** le dà un appiglio; la IR può dare un
> appiglio solo a ciò che il **mondo allargato** ammette come *genere di cosa*;
> e **nulla di tutto questo si mette in moto se la lezione non viene capita** —
> che è il mestiere della **comprensione universale**.

La quarta clausola non è un'aggiunta ornamentale. Le prime tre descrivono che
cosa una lezione *potrebbe* cambiare una volta arrivata. La quarta dice se
**arriva**. Ed è la sola che lavora in **entrambe le direzioni**: fa entrare la
lezione, e fa uscire il **declino informato** che dice al maestro che cosa
insegnare dopo. Un canale che non parla all'indietro non è un canale di
addestramento: è un imbuto.

Da cui i **quattro** modi tipici di fallire, che sono i quattro modi in cui i
piani precedenti si sono fermati:

| sintomo | elemento mancante | esempio reale |
|---|---|---|
| la lezione detta in un modo nuovo riceve *«Non capisco ancora»*, e il maestro non sa perché | **comprensione universale**: muro cieco invece di declino informato | le quattro forme di lezione **italiane** del circuito del dialogo erano dichiarate e *«non ancora verificate end-to-end»* (`LEARN_PROTOCOL.md`): sembravano insegnabili. Una è stata provata il 20 settembre e funziona; le altre tre restano non verificate, e nulla lo direbbe |
| la lezione entra, il comportamento non cambia | **KB viva**: il predicato che la lezione scrive non lo legge nessuno | `greeting(ahoy)`: vero in KB, invisibile al comportamento |
| la lezione sarebbe esprimibile ma non c'è su che cosa dirla | **IR**: il turno non pubblica il pezzo di cui la lezione parla | insegnare a trattare l'avverbiale di tempo, quando la lettura lo scarta |
| la lezione non è nemmeno formulabile | **mondo allargato**: manca il genere di oggetto | «in quale lingua ti ho chiesto…»: un turno passato non è una cosa |

**Corollario operativo per chi legge questo piano.** Prima di aprire il codice,
dire a quale dei quattro appartiene il buco — e il primo si controlla per
primo, perché è il più economico da verificare e il più facile da scambiare per
uno degli altri tre. Una cura all'elemento sbagliato produce un verde che non
compone: è il modo più efficiente di perdere una sessione, e questo repo ne ha
esempi committati.

---

## 4. L'indicatore: **learning-capability**, da 0 a 100

### 4.1 La definizione

> **learning-capability misura quanto di parrot0 può essere cambiato
> parlandogli**, in natura e in altezza — non quanto sa.
>
> **0** = parrot0 non impara nulla: nessuna frase detta modifica alcun suo
> comportamento, e ogni sua capacità è una riga scritta da qualcuno.
>
> **100** = parrot0 può imparare e **diventare qualsiasi cosa** lo si voglia far
> diventare: un esperto di astrofisica, un clone di un LLM, un interprete di
> file PGN scacchistici. Sono esempi: **100 vuol dire ogni cosa**, e senza mai
> ricompilare.

È la gemella della **scala di F.** per la comprensione
([`lettura-della-prosa.md`](lettura-della-prosa.md) §0-bis), e ne eredita il
carattere: **non misura quante lezioni passano, misura che cosa una lezione può
raggiungere.** Le due scale sono indipendenti. Un parrot0 che capisce benissimo
e non si lascia modificare sta alto sulla prima e a zero su questa.

**Ma non sono scorrelate in un punto solo, ed è la comprensione universale.**
Una lezione è un turno: se la frase che insegna non viene compresa, la lezione
non arriva. Per questo la comprensione universale è **il pavimento** di questa
scala — non un elemento fra gli altri tre, ma la condizione perché gli altri
tre siano misurabili. Se la banda di comprensione crolla, questa crolla con
lei; il contrario non vale.

### 4.2 Le due dimensioni che F. ha nominato

**Forza modificativa** — *che cosa* una lezione può cambiare:

| | livello | una lezione può cambiare… |
|---|---|---|
| **L0** | niente | nulla |
| **L1** | contenuto | un fatto del mondo |
| **L2** | superficie | una forma riconosciuta: un sinonimo, un indizio, una parafrasi |
| **L3** | procedura | una trasformazione, un piano, una mossa: il *come si fa* |
| **L4** | condotta | chi prende il turno, con quale precedenza, e **quando NON deve** |
| **L5** | struttura | un genere di cosa nuovo: un ruolo nella IR, uno strato del mondo allargato, **una forma di lezione** |

**Astrazione** — su *quale ordine* la lezione agisce:

| | ordine | la lezione vale per… |
|---|---|---|
| **A0** | istanza | questa risposta, questo turno |
| **A1** | classe | ogni membro della classe, anche futuro |
| **A2** | relazione fra relazioni | *«doubled x is x followed by x»*, poi *«grandparent is doubled parent»* |
| **A3** | la lezione stessa | una lezione che crea una **forma di lezione**: il circolo si chiude |

L5 e A3 sono il punto dove il sistema comincia a nutrirsi da sé. Sono anche i
due dove parrot0 oggi è più debole, e non è una coincidenza: sono gli unici due
che nessun lavoro di dominio produce come effetto collaterale.

### 4.3 Le bande, con le loro àncore

| banda | che cosa è vero a quella banda |
|---|---|
| **0** | nessuna frase cambia nulla |
| **1–15** | solo **fatti** (L1/A0). Ogni forma nuova costa C |
| **16–30** | **+ superfici** (L2): sinonimi, indizi, parafrasi di forme **esistenti**. Un *genere* di forma nuovo costa C. **Soglia di comprensione universale:** una lezione detta in un modo non previsto riceve un **declino informato**, non un muro cieco — sotto questa riga il maestro lavora alla cieca e nessuna banda superiore è raggiungibile |
| **31–45** | **+ procedure** (L3) e classi (A1): si insegna a *fare*, non solo a sapere. La condotta resta compilata |
| **46–60** | **+ condotta** (L4): precedenza, cessione e guardie di pertinenza si insegnano **e si ritirano** |
| **61–75** | **+ la lettura** (IR): un ruolo o un genere di nodo nuovo costa **una lezione**, non un ramo. Le riscansioni della stringa sono sparite, quindi due lettori dello stesso turno non possono più essere in disaccordo su che cosa c'è scritto |
| **76–90** | **+ il mondo allargato** è estensibile, e **A3**: una lezione crea una forma di lezione. Le catene finiscono in radici e circoli, non in righe a mano |
| **91–100** | **la tripletta di accettazione** (§4.4) passa per sola conversazione e curriculum, su un dominio, una condotta e una notazione **mai visti** |

### 4.4 La tripletta di accettazione per il 100

I tre esempi di F. non sono intercambiabili: **misurano tre assi diversi**, ed è
per questo che insieme definiscono il 100 meglio di qualsiasi formula.

| bersaglio | che cosa mette davvero alla prova | livello richiesto |
|---|---|---|
| **esperto di astrofisica** | ingestione su scala + forme di domanda: sapere molto e farsi interrogare in modi non previsti | L1+L2 su volume, A1 |
| **clone di un LLM** | **condotta**: registro, iniziativa, quando rispondere e quando tacere, che cosa non dire | L4, A1 — vedi [`mimic-llm.md`](mimic-llm.md) |
| **interprete di file PGN** | una **notazione** nuova (una grammatica), una **procedura** (applicare una mossa), un **oggetto** nuovo (lo stato della scacchiera) | L3+L5, A2 — tocca tutti e tre i livelli del §3 |

Il terzo è il più esigente e va tenuto come bersaglio di riferimento: un file
PGN non è prosa, non è un fatto e non è un dialogo. Se parrot0 può imparare a
leggerlo **parlandogli**, allora la IR non era una scansione dell'inglese e il
mondo allargato non era una lista di cinque parole.

### 4.5 Come si misura — il censimento delle catene

Non si stima: si conta. La procedura eredita da
[`radici-insegnabilita.md`](radici-insegnabilita.md) §3, che fissa il vantaggio
decisivo — **il punto di partenza è dato**, perché ogni abilità che parrot0 ha
già dimostra che una catena la sostiene.

1. **Campionare** N abilità reali da tre pozzi, non da un elenco scritto per
   l'occasione: le asserzioni dei `.p0t`; le forme del catalogo
   (`LEARN_PROTOCOL.md` §6-bis); **ogni ramo C che decide qualcosa**.
2. Per ciascuna, la domanda di risalita: *parrot0 potrebbe riapprenderla da una
   lezione di ordine superiore?* — e si classifica la fine della catena:
   **radice**, **circolo**, **riga a mano**.
3. Per ciascuna, segnare il livello **L** e l'ordine **A** più alti raggiunti.
4. Il referto è **un istogramma su L0–L5, più la quota di catene che finiscono
   in una riga a mano**. La banda si legge da lì.
5. Ogni abilità marcata insegnabile va **provata parlando**: replay, altro
   argomento, ritiro, replay. Un censimento che controlla solo che la forma
   esista non vede gli anelli rotti — il gen510 ne ha trovato uno che rompeva
   *tutte* le catene di un tipo, in silenzio.
6. **E si misura anche il pavimento.** Per ogni lezione del campione, dirla una
   seconda volta **in un modo non previsto** (altra formulazione, altra lingua,
   forma telegrafica) e registrare che cosa torna: la lezione capita, un
   **declino informato** che nomina ciò che manca, o un **muro cieco**. La quota
   di muri ciechi sulle lezioni è l'indicatore anticipato di tutto il resto:
   sale sempre prima che la banda scenda. Le tre specie di lacuna di
   [`universal-comprehension.md`](universal-comprehension.md) §10 dicono quale
   dei tre esiti ci si doveva aspettare.

## 4.5-bis. IL CENSIMENTO ESEGUITO — 21 settembre 2026

**Questa sezione sostituisce la stima del §5 con una misura, e la contraddice.**
Il numero contato è **più basso** di quello stimato, e la ragione è una sola:
la stima guardava che cosa parrot0 *può* imparare, il censimento guarda che
cosa arriva quando il maestro non conosce la superficie esatta.

### Il campione, fissato prima di misurare

Regola meccanica, nessuna scelta a mano: ogni pozzo ordinato in modo
deterministico, un elemento ogni ⌊N/10⌋ a partire dal primo. **N = 30**, dieci
per pozzo (4.676 asserzioni `.p0t`; 199 righe del catalogo §6-bis; 80 facoltà
`mod_*`). Il campione è riproducibile con lo script di estrazione; una riga
estratta (`B1`) è risultata l'intestazione della tabella — **artefatto del
campionamento, lasciato dentro** proprio perché prova che la scelta è stata
meccanica.

**Misurati parlando: 12 dei 30.** Gli altri 18 restano da classificare, e
questa sezione non finge di averli visti.

### Il pavimento — la misura che decide tutto

Per ogni lezione: detta nella forma prevista, poi in una forma **non prevista**.
Il §4.5 punto 6 attendeva tre esiti; la misura ne trova **un quarto, ed è il
più numeroso e il peggiore**.

| lezione, detta in modo non previsto | esito | specie |
|---|---|---|
| «zorbo **belongs to** the birds» | `Learned: zorbo belong birds.` | **fatto storto, in silenzio** |
| «zorbi is the **plural form of** zorbo» | `Learned: zorbi is a plural. Learned: zorbi form zorbo.` | **due fatti storti** |
| «glorp **takes an event as its subject**» | `Learned: glorp take event as its subject.` | **fatto storto** |
| «**the opposite of** zabby **is** zibby» (ordine invertito) | `I don't understand that yet.` | muro cieco |
| «grandparent **means** parent **then** parent» | «I found the teaching pivot, but I cannot align the same variables…» | **declino informato** ✓ |

| esito | quota sul campione |
|---|---|
| lezione capita | — (nessuna delle cinque varianti) |
| **declino informato** | **1 su 5 (20%)** |
| muro cieco | 1 su 5 (20%) |
| **fatto storto entrato in silenzio** | **3 su 5 (60%)** |

**La quarta specie non era prevista dal piano, e va aggiunta al §4.5.** Un muro
cieco lascia il maestro all'oscuro; un fatto storto gli dice **«Learned»**
mentre scrive in KB una cosa sbagliata. È peggio del muro per la stessa ragione
per cui, sulla scala della prosa, una risposta fluente e infondata è peggio di
un «non so»: il maestro crede di aver insegnato e non ha nessun segnale.

### Le forme che funzionano, e fin dove arrivano

| # | forma | esito nella forma prevista | L | A |
|---|---|---|---|---|
| B2 | `X is a member of <classe>` | ✅ `what is zorbo?` → «zorbo is a birds»; `is zorbo a birds?` → «Yes» | L1 | A1 |
| B4 | `the plural of X is Y` | ✅ accettata e ritenuta | L2 | A1 |
| B5 | `X is the opposite of Y` | ✅ replay: `what is the opposite of zabby?` → «Zibby» | L2 | A1 |
| B9 | `V is W followed by Z` | ✅ **catena intera**: insegnata la composizione, dati due fatti, `who is the grandparent of carl?` → «ann» | **L3** | **A2** |
| — | `X is a Y` (base) | ✅ lezione, replay, **ablazione** (`Forgotten: animal(bob)`), replay | L1 | A1 |
| T3 | `when the topic … do not answer with …` | ✅ lezione, effetto, **trasferimento alla classe**, ritiro | **L4** | A1 |

**B9 e T3 sono i due punti alti misurati**, e sono veri: una relazione fra
relazioni e una condotta, entrambe insegnate parlando, entrambe con effetto
verificato.

### Le rotture misurate

| # | che cosa | esito |
|---|---|---|
| A5 | `X is the capital of Y` — **ablazione** | 🔴 **tre formulazioni, nessuna ritira**: «forget that bezra is the capital of nivora» → «I didn't know that anyway» e il fatto **resta**; «forget that the capital of nivora is bezra» → muro cieco. La forma semplice `X is a Y` invece ritira: il buco è della forma **relazionale** |
| A7 | forma di domanda con variabile (`when i say what shade is X i mean what colour is X`) | 🔴 muro cieco |
| A1 | procedura detta in prosa (`to double a number multiply it by two`) | 🔴 muro cieco, poi «I looked up «double» but found nothing» |
| B3 | `V is an event subject verb` | 🔴 **letta come due fatti mutilati**: `Learned: glorp is an event. Learned: glorp subject verb.` |

§4.7 è esplicito: **se ritirando la lezione il comportamento resta, il punto
non si conta**. A5 quindi non si conta, e con lei ogni abilità relazionale
della stessa famiglia.

### L'istogramma, e la banda

Sui **12 misurati**, livello più alto raggiunto:

| | L0 | L1 | L2 | L3 | L4 | L5 |
|---|---|---|---|---|---|---|
| abilità | 4 (rotte) | 2 | 2 | 1 | 1 | 0 |

Catene che finiscono in una **riga a mano**: le quattro rotture più le facoltà
`mod_*` del pozzo C non ancora risalite. Sulla parte misurata, **un terzo delle
abilità campionate non arriva affatto quando il maestro cambia una parola**.

> **learning-capability contato ≈ 30**, non 45–50.

**Perché non di più, ed è il punto del censimento.** La forza modificativa
*arriva* a L4 e ad A2 — B9 e T3 lo provano, e non sono stime. Ma il §4.1 dice
che la comprensione universale **non è un elemento fra gli altri: è il
pavimento**, e che *«se la banda di comprensione crolla, questa crolla con
lei»*. Il pavimento misurato è sotto la sua soglia: **l'80% delle formulazioni
non previste non riceve un declino informato**, e il 60% scrive un fatto
sbagliato dicendo «Learned». La banda 16–30 chiede esattamente il contrario.

**Perché non di meno.** Due punti alti sono reali e ripetibili, l'ablazione
funziona sulla forma base, e il canale ha imparato a parlare all'indietro su
almeno una forma (T2). Non è un sistema che «non impara nulla».

**Che cosa muoverebbe il numero più di ogni altra cosa**, e adesso è contato e
non argomentato: **le tre superfici che scrivono un fatto storto invece di
declinare**. Non servono forme nuove — serve che una forma *quasi* riconosciuta
smetta di essere accettata a metà. È il §6.2, e la misura dice che vale più di
tutto il resto messo insieme.

### Correzione del censimento, e chiusura di due superfici su tre (21 settembre 2026, sera)

**La prima lettura era troppo dura, e va corretta.** Chiamare quei tre esiti
«fatti storti» era sbagliato: misurando meglio, `zorbo belongs to the birds`
scrive `belong(zorbo, birds)`, e alla domanda speculare **«what does zorbo
belong to?» risponde «Birds.»**. Il fatto è **corretto e raggiungibile** — ma
solo dalla strada del verbo che l'ha scritto. La specie vera non è la
corruzione: è la **strada rotta** (`broken-roads-not-gaps`), cioè una lezione
che atterra su una relazione diversa da quella che il maestro intendeva, **in
silenzio**.

**Il blocco vero, e dove stava.** La lezione di redirezione esiste già —
«"X" is another way to say "Y"», la parafrasi che scrive `phrase_canon/2` — e
davanti a questi casi dava un **declino informato**, che è la specie giusta:

> *I do not understand «is a member of» well enough to copy it: teach me with a
> phrasing I already handle.*

Ma rifiutava un bersaglio che parrot0 **gestisce davvero**: `zorbo is a member
of birds` è una forma del catalogo e funziona. La causa è una soglia: il
bersaglio è accettato solo se una famiglia di cue ne copre almeno il
`lesson_anchor_min_cover` per cento — **40** — e il controllo misura la
*superficie*, non «so leggere questa frase».

**Misurato, non stimato.** La soglia più alta che ammette il caso legittimo:

| soglia | «belongs to» → «is a member of» | «qzwx» → «nothing understands this» |
|---|---|---|
| 40 (prima) | ⛔ rifiutato | ⛔ rifiutato |
| 35 / 30 / 25 | ⛔ rifiutato | ⛔ rifiutato |
| 20 | ✅ accettato | ⛔ rifiutato |
| **10 (ora)** | ✅ accettato | ⛔ **rifiutato** |

Il buco che la soglia proteggeva — il commento in `src/brain/00-lex.c` cita
*«qzwx nothing understands this»* — **non si riapre**: a tenerlo chiuso sono le
altre condizioni, non il 40%. La soglia era già conoscenza
(`lesson_anchor_min_cover/1`, `kb/core/intents.p0`) e il commento diceva che si
può stringere o allentare senza ricompilare: è **una riga di KB**, con la
misura accanto.

**Esito sulle tre superfici del censimento:**

| superficie | prima | dopo la redirezione insegnata |
|---|---|---|
| «zorbo **belongs to** the birds» | `Learned: zorbo belong birds.` | ✅ `Held: zorbo is one of the birds — it inherits what they have.` e `what is zorbo?` → «zorbo is a birds» |
| «zorbi **is the plural form of** zorbo» | `Learned: zorbi is a plural. Learned: zorbi form zorbo.` | ✅ `Learned: plural(zorbi, zorbo).` e `what is the plural of zorbo?` → «zorbi» |
| «glorp **takes an event as its subject**» | `Learned: glorp take event as its subject.` | ⛔ la redirezione è accettata, ma il **bersaglio stesso** si legge male: `is an event subject verb` produce due fatti mutilati. È il difetto **B3** già registrato dal censimento, ed è un'altra cosa |

**Che cosa è cambiato davvero, in termini della scala.** Non tre superfici in
più — quelle il §4.6 le conta zero. È un **rifiuto diventato una porta**: il
maestro che sbaglia formulazione ora può *dirlo* e la lezione arriva, per
qualunque superficie, non per queste tre. La redirezione è insegnata parlando,
non scritta in KB da noi.

**Resta aperto, e nominato:** il bersaglio `V is an event subject verb` si
legge come due fatti mutilati. Finché una forma del catalogo si legge male,
nessuna redirezione verso di lei può funzionare — e questa è la specie che
tiene ancora il pavimento sotto la sua soglia.

### B3 chiusa, e il terzo esito del censimento con lei (21 settembre 2026, sera tardi)

`LEARN_PROTOCOL.md` dichiara la forma `V is an event subject verb` e la dà per
verificata. Misurata su un verbo **mai visto**, non insegnava:

```text
> glorp is an event subject verb
Learned: glorp is an event. Learned: glorp subject verb.      (9,7 s)
```

Due fatti mutilati. La causa: «event subject verb» è un nome di classe di
**tre parole** e il lettore generico lo spezza — con `help`, già presente in
KB, non si vedeva. **Una forma che vale solo per i membri già presenti non è
una forma: è un ricordo**, e il catalogo la contava come capacità.

Chiusa dichiarandola, con la sua ritrattazione (`kb/core/conduct-lessons.p0`):

| prova | esito |
|---|---|
| lezione su un verbo nuovo | ✅ `Held: the subject of «glorp» can be an action or a means.` |
| **trasferimento** a un secondo verbo mai visto | ✅ `event_subject_verb(zorblax)` |
| **ablazione mirata** | ✅ `glorp` sparisce, `zorblax` **resta** |
| tempo del turno | 9,7 s → 4,9 s (ancora lento: è il costo del turno, non della forma) |

Con questa, il **terzo** esito del censimento è affrontato: non più «fatto
storto», ma una forma dichiarata che ora regge un membro nuovo.

### Riletura dell'indicatore dopo le due chiusure

| | prima | dopo |
|---|---|---|
| formulazione non prevista che diverge in silenzio | il maestro non aveva strumento | **può redirigerla parlando**: la porta esiste e funziona su ogni superficie |
| forme del catalogo che valgono solo per membri noti | B3, contata come capacità | chiusa, con trasferimento e ablazione |

> **learning-capability ≈ 35–40**, contro i **30** contati dal censimento.

**Perché sale.** Il §4.6 conta *«un muro cieco su una lezione diventato
declino informato»*: qui un **rifiuto** è diventato una **porta che funziona**,
che è di più. E una catena che finiva in una riga a mano (B3) ora finisce in
una forma con ablazione.

**Perché non arriva a 50, e va detto.** La divergenza silenziosa è ora
*riparabile* parlando, non *prevenuta*: il maestro deve accorgersene. Finché
una lezione quasi riconosciuta viene accettata a metà senza dirlo, il pavimento
resta sotto la soglia della banda 16–30 per quella specie. E **18 dei 30
elementi campionati non sono ancora stati misurati**: la quota di «riga a mano»
resta una stima inferiore.

**Il prossimo passo, ora nominato e non generico:** che una lettura *parziale*
di una lezione non asserisca nulla e lo dica — la specie, non le superfici.
È il §6.2, ed è l'unica cosa che porta il pavimento sopra la sua soglia.

### Il passo verso il pavimento: rilevatore costruito, **misurato, non collegato**

Il §6.2 chiede che una lettura *parziale* di una lezione non asserisca nulla e
lo dica. Il segnale strutturale c'è: il turno contiene il **testo fisso** di
una forma dichiarata — la parte che non varia, quella che identifica la
lezione — e nessuna forma conclude. Le forme si dichiarano da sé, quindi il
rilevatore varrebbe anche per quelle che verranno.

`lesson_near_miss/2` è scritto (`kb/core/conduct-lessons.p0`) e misurato:

| turno | scatta? | giudizio |
|---|---|---|
| «zorbo **is a member of**» (forma incompleta) | ✅ sì | vero positivo |
| «socrates is a man» | ❌ no | corretto |
| «what is the capital of france» | ❌ no | corretto |
| «the cat sleeps on the mat» | ❌ no | corretto |
| «glorp is an event subject» (testo fisso **parziale**) | ❌ no | falso negativo: chiede tutte le parole del testo fisso |
| «zorbo **belongs to** the birds» | ❌ no | falso negativo: è un **sinonimo**, non una forma incompleta |
| «zorbo is a member of birds» (forma **completa**) | ⚠️ **sì** | **ecco il blocco** |

**Perché non è collegato.** L'ultima riga: il rilevatore scatta anche quando la
lezione è completa e ha funzionato. Per usarlo servirebbe sapere che **nessuna
forma ha concluso**, e quel fatto oggi non è interrogabile dalla KB. Collegarlo
così spegnerebbe l'apprendimento ordinario — il canale principale — per un
segnale che non distingue il successo dal quasi.

**E non copre la specie del censimento.** I tre casi erano **sinonimi**, non
lezioni digitate a metà: «belongs to» non contiene il testo fisso di nessuna
forma. Sono due specie diverse, e averle separate misurando vale più che
averle confuse in una cura.

**Il prossimo passo, ora preciso:** rendere interrogabile *«in questo turno una
forma ha concluso»*. È un fatto che il motore già conosce — decide su di esso —
e che la KB non può leggere. Con quello, il rilevatore diventa una porta in una
riga; senza, resta una sonda.

### «Una forma ha concluso» è ora interrogabile — e il collegamento è stato provato e ritirato

Il passo nominato dalla sezione precedente è fatto: `turn_form_concluded(N, Forma)`
è scritto dal motore dove una forma dichiarata **conclude davvero**, indicizzato
col contatore del turno come ogni altro fatto di turno. Verificato:

| turno | `lesson_concluded(yes)` |
|---|---|
| «glorp is an event subject verb» (lezione riuscita) | ✅ vero |
| «zorbo is a member of» (incompleta) | ✅ falso |

Era il fatto che mancava: prima il rilevatore non distingueva il successo dal
quasi, e per questo non si poteva collegare.

**Collegato, misurato, ritirato.** Con la guardia `naf(lesson_concluded(yes))`
la porta è stata costruita davvero:

| turno | esito col collegamento |
|---|---|
| «zorbo is a member of» | ✅ **declina** invece di asserire in silenzio |
| «socrates is a man» | ✅ impara, nessun falso positivo |
| «bob is an animal» | ✅ impara |
| «zorbo is a member of birds» (lezione **completa**) | ⛔ **declina** |

**Il cancello della cessione si decide PRIMA che le forme vengano tentate**,
quindi la guardia non può ancora vedere il successo. Una regressione su una
lezione che funziona non è accettabile: ritirato, con il commento accanto in
`kb/core/conduct-lessons.p0`.

**Che cosa manca davvero, ora nominato al livello giusto.** Non un altro fatto:
**che la cessione possa essere decisa dopo il tentativo delle forme dichiarate**.
È una questione di *ordine del turno*, non di conoscenza mancante — ed è la
stessa specie del §6.1: il canale, non il suo contenuto. Finché l'ordine è
questo, il rilevatore resta una sonda e il pavimento resta sotto la soglia.

**L'indicatore non si muove per questo giro: resta 35–40.** Un fatto abilitante
in più e una porta provata e ritirata non sono una banda nuova, e contarli
sarebbe esattamente ciò che il §4.6 vieta.

### L'ordine del turno cambiato, la porta collegata, e il pavimento che si muove

Il blocco nominato dal giro precedente — *«la cessione si decide prima che le
forme vengano tentate»* — è stato tolto. In `mod_knowledge` la chiamata alla
porta condivisa è ora **dopo** `p0_turn_form_views`: le forme dichiarate hanno
già avuto il turno e, se una ha concluso, la funzione è già tornata. Solo
allora una condotta può guardare un fatto vero: **che cosa questo turno non è
riuscito a essere**.

Con quell'ordine, la porta sul «quasi una lezione» si collega senza regressione:

| turno | prima | ora |
|---|---|---|
| «zorbo is a member of birds» (lezione **completa**) | ⛔ declinava | ✅ `Held: zorbo is one of the birds…` |
| «zorbo is a member of» (incompleta) | `Learned: zorbo is a member.` | ✅ **declina** |
| «socrates is a man», «bob is an animal» | imparano | ✅ imparano |
| «glorp is an event subject verb» | ✅ | ✅ |
| guardia di pertinenza (T3) | ✅ | ✅ invariata |

### Il pavimento, rimisurato sulle stesse cinque sonde del censimento

| sonda | censimento | ora |
|---|---|---|
| «zorbo is a member of» | fatto storto | ✅ **declina** |
| «zorbi is the plural form of zorbo» | due fatti mutilati | ✅ **lacuna onesta** |
| «the plural of» | — | ✅ lacuna onesta |
| «glorp is an event subject» (testo fisso **parziale**) | fatto storto | ⛔ resta |
| «zorbo belongs to the birds» (**sinonimo**) | fatto storto | ⛔ resta, **ma riparabile parlando** |

| esito | censimento | ora |
|---|---|---|
| declino informato / lacuna onesta | **1 su 5 (20%)** | **3 su 5 (60%)** |
| fatto storto in silenzio | **3 su 5 (60%)** | **2 su 5 (40%)**, entrambi riparabili con una lezione |

> ## learning-capability ≈ **50**
>
> Contato, con le misure di questa sezione.

**Che cosa lo sostiene, riga per riga del §4.3:**

- **banda 16–30, la soglia del pavimento:** *«una lezione detta in un modo non
  previsto riceve un declino informato, non un muro cieco»* — ora vale per la
  **maggioranza** delle sonde (60%), e i due residui non lasciano il maestro
  senza strumento: la redirezione *«"X" is another way to say "Y"»* funziona e
  chiude il caso parlando. Al censimento questa riga era **sotto** soglia, ed
  era la ragione del 30.
- **banda 31–45, procedure e classi:** `V is W followed by Z` insegna una
  relazione fra relazioni e la catena regge fino in fondo (**L3 × A2**);
  la forma di `event_subject_verb` insegna, trasferisce a un membro mai visto
  e si ritira in modo mirato (**A1** con ablazione).
- **banda 46–60, l'àncora:** *«precedenza, cessione e guardie di pertinenza si
  insegnano e si ritirano»* — la guardia di pertinenza si insegna in lingua
  naturale, ha effetto, **trasferisce alla classe** senza una seconda lezione,
  e il ritiro riporta il comportamento di prima. E da questo giro la cessione
  si decide **nel punto giusto del turno**, che è ciò che la rendeva
  inutilizzabile per le condotte che dipendono dall'esito delle forme.

**Perché 50 e non di più.** Le bande sopra chiedono cose che non ci sono e che
non ho finto: **61–75** vuole la IR consumata invece che riscansionata (217
`split_words` sono ancora lì); **76–90** vuole il mondo allargato estensibile e
**A3**, una lezione che crea una forma di lezione — il gradino S2, tuttora una
riga a mano. E **18 dei 30 elementi campionati non sono stati misurati**: la
quota di catene che finiscono a mano resta una stima inferiore, quindi 50 è il
**limite superiore difendibile** di questa misura, non il suo centro.

**Costo pagato, dichiarato:** `taught_lexicon.p0t` passa da 6 a 7 rossi. Tutti
e sette sono **tempi**, nessuna asserzione rotta; il settimo è un turno a
1,16 s su un budget di 1,00. `soft-test` resta verde a 12 s, `facts.p0t` e
`derivation.p0t` verdi.

### La lettura diventa estensibile parlando (21 settembre 2026, notte)

La banda **61–75** chiede due cose. La prima è fatta e misurata; la seconda no,
e lo dico prima dei dettagli.

#### Prima clausola: *«un ruolo o un genere di nodo nuovo costa una lezione, non un ramo»* ✅

«the expression X marks Y» insegnava una lettura nuova **solo dentro un genere
che esisteva già**: lo slot accetta soltanto nomi dichiarati, e un genere nuovo
era una riga a mano in `function-words.p0`. Ora il genere si insegna:

```text
> the reading hedge is of kind modality
Held: «hedge» is now a reading of the modality kind. Teach me an expression
with «the expression … marks hedge».
> the expression forsooth marks hedge
> forsooth the cat sleeps
```

e l'ispettore, sulla IR di quel turno, mostra la lettura nuova **accanto a
quelle native**:

```text
debug_grammatical_cue — evidence(span(1, 1), modality, hedge)
                        evidence(span(2, 2), determination, definite)
```

| prova | esito |
|---|---|
| il genere arriva a `grammatical_cue/4`, cioè alla IR che i consumatori leggono | ✅ |
| una **seconda** espressione nello stesso genere insegnato | ✅ senza una seconda lezione sul genere |
| **ablazione**: ritirato il genere, l'indizio sparisce dal turno | ✅ |
| visibile nell'ispettore come una lettura qualunque | ✅ |

**Due lezioni che si compongono**: la prima crea la categoria, la seconda la
riempie. È il circolo che la scala chiama **A3** — una lezione che apre lo
spazio di un'altra lezione — su un pezzo della **lettura**, non del contenuto.

**Limiti misurati, non nascosti:**

- il nome del genere dev'essere di **una parola**. Con un nome multiparola
  («epistemic hedge») le prime quattro maglie tengono e `expression_at` non
  trova più l'espressione: l'atomo quotato non sopravvive dentro il termine
  della vista materializzata `expression_first_word`. Misurato, isolato, non
  curato;
- una forma di lezione che **comincia con un jolly** non può avere una gemella
  di ritiro distinguibile — «forget that X is …» viene letta dalla forma di
  insegnamento con nome «forget that X». Misurato due volte oggi. L'apertura è
  ora testo fisso, come tutte le coppie che funzionano.

**Un reperto abilitante, registrato per chi continua:** una **regola KB può
contribuire un nodo alla IR**. `input_node(...) :- …` scritto in KB viene visto
dai consumatori e dalle viste derivate (`input_node_parent` lo trova). Non
serve un ramo C per un genere di nodo: serve la forma di lezione che scriva
quella regola. È il gradino successivo, ed è ora a portata.

#### Seconda clausola: *«le riscansioni della stringa sono sparite»* ⛔

**Non è vera, e il numero non la aiuta: sono 349** `split_words` in 14 file
(erano 217 quando il piano fu scritto: il conteggio è cresciuto). Quello che è
misurabile è la **conseguenza** che la clausola nomina — *«due lettori dello
stesso turno non possono più essere in disaccordo su che cosa c'è scritto»*:

```text
scripts/fenomeni.sh flussi  →  nessun disaccordo sul corpus
```

dopo l'unificazione delle tre regole di confine di parola. **È meno di quanto
la clausola chiede**: il rilevatore guarda due flussi su un corpus di prosa,
non tutti i lettori su tutti gli input.

> ## learning-capability ≈ **60–65**
>
> Dentro la banda 61–75, non oltre: la prima clausola è soddisfatta e
> verificata, la seconda no.

**Che cosa serve per il 70, ora nominato al livello giusto.** Non un'altra
forma di lezione: **togliere le riscansioni**. Finché 349 chiamate leggono la
stringa per conto proprio, non c'è un *dove* stabile su cui una lezione possa
dire qualcosa, e ogni guadagno sulla lettura resta locale al consumatore che lo
ha ricevuto. È il §6.5, ed è l'unico lavoro che sposta questa banda.

**Costo dichiarato:** `english_grammar_growth.p0t` ha un rosso nuovo — un turno
a **1,01 s** su un budget di 1,00, nessuna asserzione rotta. `soft-test` verde
a 12 s, `derivation.p0t` verde, `taught_lexicon.p0t` invariato ai suoi sette
tempi.

### Che cosa questo censimento non ha fatto

- **18 elementi su 30 non sono stati misurati parlando** (gran parte del pozzo A
  e tutto il pozzo C). La risalita delle facoltà `mod_*` — *questa decisione
  potrebbe essere riappresa da una lezione?* — è la metà più costosa e manca.
- La quota di «riga a mano» è quindi **una stima inferiore**, non un conteggio.
- Il campione è di 30 su migliaia: dice la specie dei guasti, non la loro
  frequenza esatta nella KB intera.

---

### 4.6 Le regole di lettura — che cosa muove l'ago

Stessa disciplina della scala della prosa, perché ha funzionato.

| muove l'ago | **non** lo muove, anche se il numero sale |
|---|---|
| una catena che passa da **riga a mano** a radice o circolo | una superficie in più per una forma che già esisteva |
| una lezione che raggiunge un livello **L più alto** di prima | una lezione in più allo stesso livello |
| un'ablazione che **toglie davvero** il comportamento insegnato | un acknowledgement (*«ho imparato»*) senza cambiamento misurato |
| un **trasferimento** a un membro mai visto della classe | il replay dello stesso turno della lezione |
| una riscansione della stringa **sostituita** da un consumatore della IR | un consumatore della IR aggiunto accanto a una riscansione che resta |
| un **muro cieco** su una lezione diventato **declino informato**: il maestro ora sa che cosa dire dopo | un muro cieco chiuso aggiungendo la superficie che l'ha prodotto — chiude quel turno, non la specie |
| una specie di lacuna **generata dalla struttura** invece che attesa in chat (`universal-comprehension.md` §10, specie 1 e 2) | una variante in più registrata a mano dopo averla vista fallire |
| un genere di cosa nuovo, di cui ora si può parlare | un predicato nuovo senza una forma che lo insegni — *un cassetto senza maniglia*, mantra #26 |

### 4.7 Anti-inganno specifico dell'indicatore

- **Il maestro non conosce lo schema.** Se la lezione nomina predicati, arità o
  tuple, quel punto non si conta (§1.3).
- **Nessuna ricompilazione**, mai, fra la lezione e la prova.
- **L'ablazione è obbligatoria.** Se ritirando la lezione il comportamento
  resta, il punto non si conta: non veniva da lì.
- **Il campione non si sceglie dopo.** Le abilità del censimento si fissano
  prima di misurarle, e il banco non si allarga per abbassare la quota di righe
  a mano.
- **Una lezione che vale per una frase sola è A0**, anche se sembra una regola.
  La prova è il membro mai visto.

---

## 5. Dove siamo oggi — stima con le sue ragioni, da contraddire con una misura

**learning-capability ≈ 60–65** a fine giornata del 21 settembre 2026 — **30 contato** dal censimento, poi risalito dalle due chiusure che seguono nel §4.5-bis — vedi il censimento
eseguito in **§4.5-bis**, che sostituisce e **contraddice** la stima di 45–50
scritta poche ore prima nella stessa giornata. La stima guardava che cosa
parrot0 *può* imparare; il censimento guarda che cosa **arriva** quando il
maestro non conosce la superficie esatta, e il pavimento è sotto la sua soglia.
La tabella qui sotto resta valida su *che cosa esiste*: è la sua raggiungibilità
che il numero contato corregge.

**Che cosa ha mosso l'ago, il 21 settembre** — tre TODO chiusi, e per ognuno
la regola del §4.6 che dice perché conta:

| | esito | perché muove l'ago |
|---|---|---|
| **T1** | ✅ le tre forme italiane insegnano davvero, con replay, trasferimento e ablazione; ognuna ha un effetto proprio | *un'ablazione che toglie davvero il comportamento insegnato* |
| **T1-bis** | 🔴→✅ trovato e chiuso un guasto del canale: la sola policy, senza bisogno né parole, veniva **accettata** e il turno dopo crollava dove prima funzionava | *toglie punti falsi* (§6.1): una lezione confermata che peggiora il comportamento è peggio di un muro |
| **T2** | ✅ declino informato su una forma: tre stati, tre messaggi, ognuno dice **che cosa scrivere dopo** | *un muro cieco diventato declino informato: il maestro ora sa che cosa dire* |
| **T3** | ✅ **la prima lezione L4 con ablazione e trasferimento** | *una lezione che raggiunge un livello L più alto di prima* |

**T3, per esteso, perché è il salto di banda.** Questo è ora dicibile, e
nessuna parte nomina predicati, arità o tuple:

```text
when the topic of a question is a turn of this conversation
do not answer with the list of things you know
```

| prova | prima | dopo la lezione | dopo il ritiro |
|---|---|---|---|
| `in which language did i ask you which language you speak` | `c, python.` | `I don't understand that yet.` | `c, python.` |
| `in which language did i ask you about python` | `c, python.` | muro onesto | — |
| `which language did i ask you about` | `c, python.` | muro onesto | — |

Le ultime due **non sono state insegnate**: vengono dalla stessa lezione. È
**L4 × A1** — condotta, valida per la classe — ed è esattamente l'àncora della
banda 46–60: *precedenza, cessione e guardie di pertinenza si insegnano e si
ritirano*.

E la specie tolta è quella peggiore sulla scala di F.: non una risposta giusta
in più, ma una **risposta fluente e infondata in meno**.

| livello | stato | evidenza |
|---|---|---|
| **L1** contenuto | solido | l'intero canale di crescita parlando |
| **L2** superficie | solido, con un **circolo** già chiuso | la lezione di parafrasi (`phrase_canon`) estende anche la propria superficie |
| **L3** procedura | **parziale** | procedure e piani insegnabili (gen507); le trasformazioni generiche restano incomplete |
| **L4** condotta | **dimostrato su una coppia** | `kb/core/conduct-lessons.p0`: `situazione × comportamento` con nomi pronunciabili, lezione, effetto, trasferimento alla classe, ritiro. Una coppia nuova costa **un nome**, non una riga di C. La precedenza fra facoltà resta però quasi tutta scritta |
| **L5** struttura | **quasi chiuso** | il gen511 ha aperto uno spiraglio; un genere di forma **nuovo** resta una riga a mano — gradino **S2** di `radici-insegnabilita.md` |
| **comprensione universale** | **il pavimento, meno incrinato** | il declino informato esiste ora anche su una **lezione**, non solo su una domanda; le tre forme italiane sono verificate. Resta la regola ordinaria altrove |
| **IR** | **il collo** | 7 file di KB consumano la IR contro **217** `split_words` |
| **mondo allargato** | esiste, non è estensibile | i cinque oggetti sono eseguibili; nessuna superficie ne aggiunge uno |

**Perché non meno di 45:** la condotta si insegna, ha effetto, trasferisce alla
classe e si ritira — l'àncora della banda 46–60 è dimostrata, e i tre guasti
del canale che tenevano il numero sotto 35 sono chiusi o localizzati.

**Perché non più di 50:** una coppia sola è dimostrata, e una coppia nuova è
ancora una riga di KB, non una lezione (*A3 non è toccato*); la precedenza fra
facoltà resta scritta; L5 e la IR sono dove erano. E **il censimento non è
stato eseguito**: finché non lo è, questo resta un numero argomentato, non
contato — va detto ogni volta che lo si cita.

**Limiti misurati e non nascosti, del circuito T3:**

- La forma **italiana** insegna e ha effetto, ma la conferma mostra i nomi
  interni in inglese, e la forma di **ritiro** italiana non aggancia — viene
  letta come un fatto. L'inglese è completo.
- Il rilevatore della situazione usa `words_in_turn`, che **non richiede
  contiguità**: una domanda che contenga quelle parole sparse verrebbe
  catturata. Si stringe quando il turno sarà un contenuto con un atto (**F1**
  di [`the-rational-philosopher.md`](the-rational-philosopher.md)).
- La cessione è **per facoltà**, non per template: la lezione nomina «l'elenco
  delle cose che sai» e la KB cede l'intera facoltà `knowledge` in quella
  situazione. Proporzionato finché la situazione è stretta, da raffinare quando
  una condotta dovrà spegnere una sola resa.

## 6. Il lavoro — che cosa alza il numero, in ordine

L'ordine non è un gusto: ogni riga è la condizione della successiva.

1. **Guarire il canale prima di allargarlo.** Una lezione deve arrivare al
   proprio lettore, avere effetto, essere l'ultima detta a contare, e sparire
   quando la si ritira. I tre guasti del §5 sono chiusi; il metodo che li ha
   trovati — provare la forma **nativa** prima di accusare la lezione — va
   applicato al catalogo intero. *Nessun punto nuovo: toglie punti falsi.*
2. **Il pavimento: nessuna lezione riceve un muro cieco.** È il §10 di
   [`universal-comprehension.md`](universal-comprehension.md) applicato al
   catalogo delle forme di lezione invece che alle domande. Per ogni forma:
   generare dalla struttura le varianti di superficie (specie 1) e le
   costruzioni simmetriche mancanti (specie 2) — *due specie su tre si chiudono
   senza mai vedere una chat* — e rendere la terza insegnabile parlando. Chi
   insegna deve ricevere, sempre, o la lezione capita o il nome di ciò che
   manca. **È il lavoro con il rapporto valore/costo più alto dell'intero
   elenco**, perché ogni banda superiore lo assume già fatto.
3. **L4 — la condotta diventa dicibile.** «Quando il tema di una domanda è un
   turno di questa conversazione, non rispondere con l'elenco delle cose che
   sai.» La **guardia di pertinenza** è la forma di seme che il `/debug` di
   parrot0 già nomina quando un turno gli riesce per il motore e fallisce per
   l'interlocutore. È il salto di banda più economico disponibile: **46–60**.
4. **L5/S2 — una lezione che crea una forma di lezione.** Il buco dichiarato da
   `radici-insegnabilita.md`, aperto a metà dal gen511. Chiuderlo trasforma il
   grafo: le catene cominciano a finire in circoli. Porta verso **76–90**.
5. **La IR consumata invece che riscansionata.** I 217 `split_words` sono il
   motivo per cui due lettori dello stesso turno sono in disaccordo su che cosa
   c'è scritto. Finché durano, la banda **61–75** è irraggiungibile per
   costruzione: non c'è un *dove* stabile su cui una lezione possa dire qualcosa.
6. **Il mondo allargato estensibile.** Un genere di cosa nuovo — *un turno
   passato è una cosa*, cioè **F1** di
   [`the-rational-philosopher.md`](the-rational-philosopher.md) — deve poter
   nascere da una lezione. È il gradino che rende plausibile la tripletta §4.4.
7. **Solo allora, la tripletta.** Tre curricula veri, tre profili, nessuna
   riga di C: astrofisica, condotta imitata, PGN.

---

## 7. Come si usa questo piano

**Chi apre una sessione su questo piano dichiara tre cose, prima di lavorare:**

1. **a quale dei quattro elementi** (§3) appartiene il buco che sta aprendo —
   e, prima di ogni altra cosa, di aver verificato che la lezione che intende
   usare **venga compresa**: un buco attribuito alla KB che era un muro cieco
   della comprensione è la diagnosi sbagliata più frequente;
2. **quale livello L e quale ordine A** la lezione dovrà raggiungere (§4.2);
3. **quale abilità esistente** userà come punto di partenza della catena — mai
   il vuoto, perché il punto di partenza è dato (§4.5).

**E chiude dichiarando:** dove ha mosso l'ago, con quale evidenza, e quali
catene sono passate da riga a mano a radice o circolo. Se il numero non si è
mosso, si dice — un giro che guarisce il canale senza alzare la banda è un
buon giro, e va raccontato per quello che è.

**Un giro non vale** se il comportamento nuovo non sopravvive al trasferimento,
se non sparisce all'ablazione, o se la lezione ha dovuto nominare lo schema
interno. In nessuno di quei casi si è addestrato il processo di apprendimento:
si è scritto nella KB con più passaggi.
