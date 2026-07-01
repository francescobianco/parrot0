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
