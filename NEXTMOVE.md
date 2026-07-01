# NEXTMOVE - piano per un parrot0 sostanzialmente comparabile a un LLM

## Tesi operativa

L'obiettivo non e imitare la superficie di un LLM. L'obiettivo e costruire un
sistema che, su una quota sostanziale di compiti comuni da LLM, produca risposte
competenti usando solo C, KB, strumenti deterministici e test.

La misura non deve essere "sembra fluido", ma:

- capisce la forma della richiesta;
- sa dire cosa sa, cosa non sa e perche;
- usa memoria e contesto;
- risolve problemi piccoli con proof trace;
- acquisisce dati senza outsourcing cognitivo;
- usa strumenti locali quando servono;
- non finge capacita non supportate.

Il collo di bottiglia attuale non e aggiungere conoscenza o stile. E trasformare
il linguaggio naturale in strutture interrogabili abbastanza generiche da
alimentare KB, memoria, ragionamento e strumenti.

## Audit di realismo

Il piano e realistico se viene letto come convergenza funzionale su classi di
task misurabili, non come promessa di parita open-domain in poche iterazioni.
parrot0 puo diventare concretamente paragonabile a un LLM su segmenti dove:

- la richiesta viene trasformata in una struttura interrogabile;
- la conoscenza necessaria e in KB o acquisibile come dato dichiarativo;
- il risultato puo essere verificato da test, proof trace, shell, compilatore o
  oracle locale;
- il sistema ammette il gap quando manca un fatto, una regola o uno strumento.

Non e realistico aspettarsi equivalenza generale con LLM moderni senza un
substrato di parsing, retrieval, memoria e composizione molto piu ampio. La
metrica corretta quindi non e "parrot0 batte un LLM", ma "parrot0 copre in modo
onesto e verificabile una quota crescente di compiti da LLM".

Nota operativa: `LLMSCORE.md` e uno snapshot del 2026-06-28, non lo stato live.
Le generazioni successive hanno gia chiuso vari item di quel 4/10; va usato come
traccia dei fallimenti, non come score corrente senza rerun.

## Audit dei principi

Il piano non rompe i principi se Fase A resta declino informato e non diventa un
fallback che simula comprensione. I rischi sono tre:

- phrasebook: aggiungere frasi in C invece di schemi e lessico in KB;
- overclaim: rispondere con tono fluente quando manca supporto;
- bench gaming: inseguire prompt singoli invece di categorie held-out.

La disciplina pratica e: forme linguistiche e dominio come KB quando possibile,
C come motore fisso, test held-out, e risposte calibrate. Un `intent_schema`
serve solo se alimenta parser, memoria, KB e strumenti; se resta documentazione
inerte non cambia la capacita.

## Decisione implementativa immediata

Il prossimo taglio utile e piccolo e coerente con Fase A+B:

- correggere le contrazioni apostrofate comuni (`what's`) nella canonicalization;
- generalizzare `process_step/3` con topic KB (`process_topic/2`) invece di
  branch C su singoli task;
- aggiungere raccomandazioni pratiche come `activity_topic/2` +
  `activity_step/3`, con declino informato quando manca la situazione;
- ratchettare i casi in `llmscore_world` perche dipendono dal mondo curato.

## Salto gen245 - frame semantici condivisi

Per essere piu ambiziosi senza rompere i principi, il taglio successivo non deve
essere "aggiungere piu prompt". Deve introdurre una spina comune:

> richiesta -> frame -> slot -> solver/render -> proof o gap

Il primo passo implementato e deliberatamente compatibile con il registry
attuale: i frame entrano nei moduli esistenti invece di aggiungere un orchestrator
centrale ancora prematuro. I casi coperti sono scelti per diversita:

- aritmetica con trasformazione di unita;
- moto relativo;
- lookup inverso di una relazione KB;
- generazione vincolata da conteggio parole;
- continuazione narrativa per scena;
- superlativo di mondo curato.

Il criterio per i prossimi passi: ogni item nuovo deve aggiungere un frame, una
relazione KB o un solver riusabile. Se aggiunge solo una risposta, non e un
salto.

## Salto gen250 - contrasto e magnitudini KB-backed

Il passo successivo ha chiuso una categoria ampia invece di un prompt: confronti
e distinzioni. La forma e:

> richiesta comparativa -> cue/due slot -> relazione KB -> risposta o gap

Due relazioni sono ora il nucleo:

- `difference_between(X, Y, Gloss)` per "what is the difference between X and Y";
- `magnitude_cue(Cue, Dim, Direction)` + `magnitude(Dim, Item, Rank)` per
  "which is faster A or B" e "is A bigger than B".

Questo resta fedele al piano perche il C riconosce il frame e le slot, mentre
lessico, dominio e valori vivono in KB. Quando il contrasto manca, la risposta
nomina il gap invece di fingere o cadere nel muro cieco.

## Salto gen251 - hardening da LLMSCORE live

Il nuovo `make llmscore` del 2026-07-01 ha spostato il campione su sei buchi
concreti: Everest, recipe scaling, poesia a quattro versi, treni verso
l'incontro, countdown con sostituzione, antonimo di `ephemeral`.

Il giro corretto non e stato aggiungere sei frasi. Le correzioni sono frame o
facts riusabili:

- `world_superlative(Property, Domain, Answer)` per primati stabili;
- scaling di ricette come triple `quantita/unita/ingrediente`;
- filtri di conteggio che sostituiscono invece di saltare;
- distanza tra citta come KB e moto relativo come solver;
- quatrain topic in KB;
- antonimi come relazione `opposite/2`.

Il run ha anche esposto un flake reale: la durata conversazionale partiva da
`brain_create` e usava secondi interi. Ora parte dal primo turno e usa
`timespec_get`, perche "da quanto parliamo" deve misurare la conversazione, non
il tempo di startup.

## Salto gen252 - protocollo line-based e second-frame pass

Il rerun post-fix di `make llmscore` ha mostrato un fallimento diverso: una
risposta multi-linea veniva letta dall'interviewer come piu risposte separate.
Questo non era un problema di conoscenza ma di protocollo conversazionale:
quando il canale e line-based, alcuni prompt devono produrre una risposta
monoriga.

Il secondo pass ha quindi aggiunto:

- `activity_summary/2` per preferenze situate monoriga, lasciando i piani
  multi-step ai prompt che chiedono davvero un piano;
- un frame di arrivo a destinazione per due treni, separato dal frame
  "si incontrano";
- simultaneita in cucina (`same time/same pot`) come vincolo sul tempo;
- EN->FR minimo come lessico KB + grammatica in C;
- routing delle riddles con numeri prima dell'aritmetica;
- cue di completamento per frasi tronche su `bottle/bottl`.

La lezione operativa: migliorare LLMSCORE non significa solo sapere piu cose.
Significa anche rispettare il contratto I/O dell'intervistatore senza perdere
onesta o struttura.

## Salto gen253 - allargamento sul campione successivo

Il successivo `make llmscore` e tornato a 4/10 ma ha mostrato gap piu locali:
problemi di scaffali, ordinamento di lettere, spiegazioni cielo/tramonto,
richieste di tre continuazioni, couplet con due temi.

Le correzioni restano strutturali:

- `with N each` e `complete shelves worth` sono slot del solver contenitore;
- reverse alphabetical order e una trasformazione di stringa;
- cielo blu + tramonto rosso compone due cause KB;
- `continuation_template/2` deve avere cardinalita sufficiente quando il prompt
  chiede piu alternative;
- i temi poetici composti vanno risolti scegliendo una surface che copra i
  vincoli salienti, non il primo token comodo.

Resta aperto un frame utile: puzzle algebrici vincolati che possono essere
inconsistenti. La risposta corretta non deve essere un numero inventato, ma
"nessuna soluzione" con prova del vincolo impossibile.

Snapshot finale del turno: `make llmscore` resta 4/10 su un nuovo campione.
Prossimi frame ad alto rendimento:

- geometria elementare (`circumference = 2*pi*r`) con un valore approssimato;
- continuation topic `horizon/traveler/light` con cardinalita multipla;
- vocabolario composto (`opposite(generous, selfish)`, truth-teller -> honest);
- vincoli numerici a scelta (`10 < n < 20`, `not prime`);
- idiom intent senza richiedere la parola "idiom";
- puzzle troncati/incompleti: declino calibrato invece di un numero casuale.

## Salto gen254 - dai prompt alle categorie (steer di F.)

F. ha corretto la rotta a meta giro: l'intervista di LLMSCORE cambia ogni run,
quindi inseguire i sei prompt dell'ultimo campione non basta. Il giro gen254 ha
chiuso i buchi come CATEGORIE, con regole di motore che generalizzano:

- geometria elementare come famiglia (cerchio: 2*pi*r e pi*r^2; rettangolo:
  lato mancante da perimetro e area), con derivazione nel testo di risposta;
- sistemi lineari a due incognite come famiglia (eta con doppio rapporto;
  teste+zampe con zampe per specie da KB `quantity/3` - un fatto mancante,
  `quantity(rabbit, legs, 4)`, teneva spento un solver gia esistente);
- passo RELATIVO nel walker sequenziale ("twice what I currently have" applica
  il moltiplicatore al totale corrente, in guadagno o rimozione);
- vincoli numerici enumerati (bound + predicati primo/pari/divisibile) con
  "nessuna soluzione" provata quando il set e vuoto;
- binding morfologico dei concetti (moonlight->moon, raindrops->rain): regola
  del composto inglese nel motore, niente alias per parola;
- word_for/2 e role_holder/2 come relazioni substring-matched (stesso schema di
  idiom_meaning): vocabolario per definizione e "who was the first X";
- frammento poetico nudo riconosciuto per FORMA (corto, senza function words,
  senza "?") e continuato dalla scena KB;
- storia su richiesta dal medesimo substrato scene/continuation, con
  story_scene/1 a marcare le scene con clausole autonome e declino informato
  che nomina alternative reali;
- "what are the three X" instradato nel counted-pick esistente;
- interviste line-based: risposte monoriga anche per le liste numerate.

Due bug di precisione trovati dal giro: il possessivo che leggeva un token
spurio quando "my" mancava, e il composto "-_what" che diventava VARIABILE
Prolog e matchava ogni categoria ("give me 2 - what" elencava colori). Il
secondo e un pattern da audit: ogni query costruita da testo utente deve
scartare token vuoti prima di comporre chiavi.

Il rerun dopo il giro: 4/10 -> 6/10 (campione nuovo), con i quattro zeri del
6/10 chiusi in giornata come frame. Prossime categorie ricorrenti osservate:

- fonologia/omofoni ("desert/dessert" letti ad alta voce): serve una relazione
  pronuncia in KB;
- il residuo di compose che lascia trapelare la clarification di repair su
  "why do you enjoy it" dopo una prima clausola gia risposta;
- percentuali e frazioni nei word problems;
- date e calendario ("what day comes three days after Saturday?").

## Roadmap

### Fase A - Eliminare il muro cieco

Priorita assoluta: nessuna frase ben formata deve cadere in `I don't understand
that yet.` senza analisi. Anche quando manca il dato, parrot0 deve rispondere
con un declino informato:

> stai chiedendo X su Y, ma non ho il fatto/regola Z.

Questa fase alza subito `LLMSCORE`, `chat-bench` e qualita percepita senza
tradire i principi. Il target e riconoscere la forma della richiesta e nominare
il gap, non fingere di sapere.

### Fase B - Costruire grammatica come KB

Introdurre progressivamente:

- `pos/2`;
- `intent_schema/2`;
- ruoli sintattici;
- schemi interrogativi, dichiarativi e imperativi come dati KB.

Il C resta motore fisso; il sapere linguistico cresce come conoscenza. Questo e
il passaggio piu importante: finche il parser resta fatto di branch
specializzati, la crescita resta lineare.

### Fase C - Composizione conversazionale

Chiudere multi-intento, coordinazione, negazione, possessivi, coreference,
memoria personale e memoria discorsiva.

Il target non e aggiungere template, ma far cooperare moduli diversi nello
stesso turno. Esempio:

> ciao, ricordati che mi chiamo F e poi dimmi cosa sai fare

La risposta corretta richiede saluto, memoria e self-model nello stesso giro,
senza perdere sub-intenti e senza rispondere alla prima cue riconosciuta come se
fosse tutto il turno.

### Fase D - Ragionamento locale verificabile

Spingere su:

- aritmetica e word problems;
- regole temporanee;
- sillogismi con nonce words;
- eccezioni;
- contraddizioni;
- proof trace;
- calibrazione.

Un LLM sostanziale non e solo fluente: mantiene uno stato di credenza. parrot0
deve distinguere `true`, `false`, `unknown`, `conflict`, `hypothetical` e
`inferred`, e deve poter spiegare quale fatto o regola sostiene la risposta.

### Fase E - Knowledge acquisition come piano

Quando manca un dato, il sistema deve generare un goal:

```text
need(know(topic)) -> acquire_knowledge(topic) -> assert wiki_concept -> answer
```

Questo va tenuto rigidamente come acquisizione di dati, non intelligenza
esternalizzata. Wikipedia o corpus statici sono ammessi come fonte dichiarativa;
un LLM a runtime no.

### Fase F - Agency locale

Consolidare il ciclo:

```text
perceive -> plan -> act -> observe -> revise
```

Prima su compiti piccoli: shell, file locali, mini verifiche, parsing codice.
Poi su coding task reali. L'agency deve essere grounded: ogni affermazione su
file, test o output deve derivare da un comando eseguito o da un fatto KB.

### Fase G - Code mastery come prova dura

Il codice e il benchmark piu onesto perche compila o non compila. Il percorso
corretto resta AST-as-KB:

```text
sorgente -> fatti strutturali -> regole di analisi -> patch -> test
```

Non template di codice, ma trasformazioni verificabili. SWE-bench resta un
north star, da usare come mappa dei fallimenti e poi come oracle quando il ciclo
patch/test sara sufficientemente grounded.

## Metriche di uscita

- `LLMSCORE`: da 4/10 a 7/10 senza fingere identita LLM.
- `chat-bench`: wall rate sotto il 5%.
- `long-chat-bench`: continuita sopra 85%.
- `compose-bench`: casi nuovi con almeno 3 moduli cooperanti.
- `basic-chat-bench`: crescita per categorie, non per prompt singoli.
- `swe-bench`: prima localizzazione corretta, poi patch verificate.

## Ordine dei prossimi lavori

1. Promuovere come task corrente il declino informato generalizzato.
2. Aggiungere un mini `intent_schema` KB-backed per domande attributive:
   capitale di X, definizione di X, processo di X, migliore modo per X.
3. Correggere arithmetic/wordproblem: `LLMSCORE` mostra errori banali e costosi.
4. Aggiungere parsing locale per regole del tipo:
   `all X are Y; A is X; is A Y?`.
5. Solo dopo riaprire con decisione il fronte coding/SWE-bench.

## Criterio di fedelta alla missione

Ogni avanzamento deve rispettare questi vincoli:

- niente LLM a runtime;
- niente phrasebook dove serve struttura;
- conoscenza e forme linguistiche come KB quando possibile;
- test held-out e, dove utile, ratchet EN/IT;
- proof o trace quando la risposta e inferenziale;
- declino informato quando manca un anello;
- stile subordinato a verita e calibrazione.

La strada concreta verso un parrot0 sostanzialmente comparabile a un LLM e
questa: trasformare linguaggio naturale, memoria, conoscenza e strumenti in un
unico substrato interrogabile e verificabile.
