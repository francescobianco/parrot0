# 2026-09-05 — Minerali di ferro

Sessione condotta secondo [`LEARN_PROTOCOL.md`](../../../LEARN_PROTOCOL.md).
L'handoff linguistico-inferenziale completo è in
[`LEARN_TODO.md`](../../../LEARN_TODO.md), sezione iniziale «addestrare la KB
viva».

**Stato finale: `trained` per quattro fatti ground.** Nessuna nuova capacità
generale è rivendicata. Due sessioni precedenti sono state scartate senza
salvare e restano parte essenziale del risultato diagnostico.

## 1. Parametri

| Parametro | Valore |
|---|---|
| `DOMINIO` | mineralogia: ematite, magnetite e minerale di ferro |
| `OBIETTIVO` | aggiungere almeno tre fatti veri e interrogabili su minerali di ferro |
| `BUDGET` | 15 minuti iniziali, estesi solo per scartare le sessioni non promuovibili e rifare il lotto pulito |
| `TARGET_WORLD_FACTS` | 3 |
| `TARGET_CAPABILITIES` | 0 |
| `STOP_CONDITION` | primo misclaim non spiegato; nessun salvataggio della sessione contaminata |

Preflight Git pulito e nessuna delle classificazioni candidata già presente.
Boot senza errori di parsing. `B0=38266`, `R0=2732`.

## 2. Fonti e proposizioni candidate

- [USGS — Iron Ore Statistics and Information](https://www.usgs.gov/centers/national-minerals-information-center/iron-ore-statistics-and-information):
  il minerale di ferro consiste quasi sempre di ossidi di ferro; magnetite ed
  ematite ne sono le forme primarie.
- [USGS — Hematite](https://pubs.usgs.gov/sir/2017/5118/elements/Hematite/Hmtt_txt.html):
  l'ematite è un minerale ossido di ferro e viene estratta come minerale di
  ferro insieme alla magnetite.
- [USGS — Quartz](https://pubs.usgs.gov/sir/2017/5118/elements/Quartz/Qtz_txt.html):
  il quarzo è SiO2; fonte del contrasto negativo.

Candidate: le quattro classificazioni promosse e «iron ore is a mineral».

## 3. Baseline

Raccolta nel processo promosso prima di insegnare il primo membro:

```text
is hematite an iron oxide mineral?
  I don't know about iron_oxide_mineral.
is magnetite an iron oxide mineral?
  I don't know about iron_oxide_mineral.
is hematite an iron ore mineral?
  I don't know about iron_ore_mineral.
is magnetite an iron ore mineral?
  I don't know about iron_ore_mineral.
```

La variante «is hematite a type of iron oxide mineral?» ha nominato
`type_of_iron_oxide_mineral`, diverso dalla classe della copula semplice. Non è
stata promossa né contata come parafrasi riuscita.

## 4. Sessione scartata 1 — falsa negazione per assenza

```text
hematite is an iron oxide mineral
  Learned: hematite is an iron oxide mineral.
is hematite an iron oxide mineral?
  Yes.
is magnetite an iron oxide mineral?
  No.
```

L'ultima risposta è falsa. Dopo la prima estensione positiva della classe, il
modulo `knowledge` ha interpretato l'assenza di magnetite come negazione. È una
violazione del contratto epistemico: una classe non è completa solo perché se ne
conosce un membro. La sessione è stata chiusa senza `/save`.

## 5. Sessione scartata 2 — fatto multi-parola irraggiungibile

Dopo avere raccolto tutte le baseline prima delle lezioni, i quattro fatti
minerari avevano replay corretto. Una quinta lezione ha prodotto:

```text
iron ore is a mineral
  Learned: iron ore is a mineral.
is iron ore a mineral?
  I don't understand that yet.
what is iron ore?
  I don't know much about iron ore yet. Want me to look it up?
```

Il dump conteneva la rappresentazione corretta del fatto, ma domanda,
descrizione e ritrattazione naturale non raggiungevano lo stesso referente
multi-parola. Anche questa sessione è stata chiusa senza `/save`: il fatto vero
ma non usabile non è stato contato come lezione riuscita.

## 6. Sessione promossa

Lezioni naturali:

```text
hematite is an iron oxide mineral
magnetite is an iron oxide mineral
hematite is an iron ore mineral
magnetite is an iron ore mineral
```

Ogni lezione ha ricevuto `Learned: ...` e la domanda sì/no corrispondente ha
risposto `Yes.`. Dopo turni diversi, due fatti ricontrollati hanno ancora
risposto `Yes.` (`Retention=2/2`).

Il contrasto «is quartz an iron oxide mineral?» ha risposto correttamente
`No.`. Il risultato è fattualmente corretto ma non assolve la logica generale
di negazione, già falsificata su magnetite.

Inventario pre-save: quattro fatti ground veri, quattro origini della lezione,
quattro letture, due referenti, tracce conversazionali e nessuna clausola falsa
o ambigua. Output esatto:

```text
parrot0: routed 119 clause(s) into the KB tree
```

## 7. Diff semantico

| Categoria | Conteggio | Clausole |
|---|---:|---|
| fatti veri del mondo `W` | 4 | due `iron_oxide_mineral`, due `iron_ore_mineral` |
| fatti linguistici `L` | 77 | `verb_stem` materializzati collateralmente |
| costruzioni/regole/procedure `C` | 0 | nessuna |
| provenienza `P` | 8 | quattro `fact_source`, quattro `reading_fact` |
| altre clausole `O` | 30 | 28 `utterance`, due `discourse_referent` |
| invalide `X` | 0 | nessuna |

Nuovi fatti veri del mondo salvati in KB: **4**

Nuove clausole totali salvate e classificate: **119**

Clausole dichiarate da `/save`: **119**

Clausole invalide: **0**

Il rapporto fra quattro fatti e 119 clausole espone un fan-out di persistenza:
le 77 forme verbali non rappresentano 77 lezioni minerarie. I nuovi predicati
sono inoltre caduti nel fallback `kb/learning/learned.p0`, che non è una casa
semantica definitiva.

## 8. Processo nuovo

Boot senza errori di parsing: `B1=38308`, `R1=2732`.

```text
is hematite an iron oxide mineral?  → Yes.
is magnetite an iron oxide mineral? → Yes.
is hematite an iron ore mineral?    → Yes.
is magnetite an iron ore mineral?   → Yes.
```

`FreshProcessRecall=4/4=100%`.

La richiesta «how do you know that hematite is an iron oxide mineral?» non ha
raggiunto la provenienza: ha generato una spiegazione causale generica su
condizioni, processi ed effetti. È un gap distinto fra giustificazione
epistemica e causa; non invalida il richiamo dei fatti, ma impedisce di
dichiarare viva la loro spiegazione.

## 9. Metriche

```text
LessonYield            = 4/5 fatti candidati
Transfer@3             = n/a; nessuna capacità generale promossa
Paraphrase             = n/a; variante “type of” non promossa
ContrastPrecision      = 1/1, con logica di negazione generale ancora rossa
Composition            = n/a
AblationFidelity       = n/a
Retention              = 2/2
FreshProcessRecall     = 4/4 = 100%
FalseUnderstandingRate = 0/4 nella sessione promossa;
                         1 overclaim osservato sul candidato multi-parola scartato
WorldKnowledgeGain     = 4
TotalPersistedClauses  = 119
```

## 10. Gap e ripresa

1. Assenza da una classe conosciuta trattata come `No` invece di `Unknown`.
2. Referente multi-parola costruito dalla lezione ma non condiviso da domanda,
   descrizione e retract.
3. «A type of» non converge ancora sulla classe della copula semplice.
4. «What did you understand?» non espone la delta della lezione.
5. «How do you know?» viene confuso con una richiesta causale e ignora la
   provenienza già persistita.
6. Il salvataggio materializza 77 stem collaterali e lascia i fatti di dominio
   nel fallback.
7. Gli identificatori di turno locali possono collidere fra sessioni e perdere
   eventi transcript identici.

L'ordine dettagliato di continuazione è nella sezione iniziale di
[`LEARN_TODO.md`](../../../LEARN_TODO.md). Il prossimo incremento deve continuare
la crescita fattuale in piccoli lotti sicuri oppure chiudere causalmente il
primo dei gap sopra e riaprire subito la lezione naturale che lo ha esposto.

## 11. File e pubblicazione

File KB modificati:

- `kb/learning/learned.p0`
- `kb/machinery/fact-provenance.p0`
- `kb/machinery/transcripts.p0`

Il commit e il push sono eseguiti insieme a questo report e all'handoff; il
relativo identificatore è quello che contiene il presente file.
