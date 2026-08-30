# GD1/GD2 — apertura generica al dialogo: la misura, e che cosa muove davvero

Data: 2026-08-31 · Protocollo: [`LEARN_PROTOCOL.md`](../../../LEARN_PROTOCOL.md)

> **Richiesta di F.** «Nei giri fatti si sono fatte cose molto valide ma nella
> *profondità di ragionamento*. Vorrei dei giri di apertura generica al dialogo:
> insegnargli tante cose, varietà, slang, ma **in maniera massiccia** — non
> campioni vari, un corpus di varietà elevata.»

## 1. La misura che mancava (GD1)

Prima di insegnare, misurare. `scripts/dialogue_corpus_probe.py` porta un corpus
di **360 turni in 60 dialoghi persistenti**, 12 famiglie, italiano e inglese,
attraverso processi parrot0 reali — uno per dialogo, nessun `/save`, nessuna
finzione di verità semantica: conta muri, mosse attese, richieste di
chiarimento, ripetizioni.

| famiglia | turni | muri | move_match |
|---|---:|---:|---:|
| F01 phatic | 30 | 16 | 14 |
| F02 slang/register | 30 | 19 | 11 |
| F03 context/coref | 30 | **24** | 6 |
| F04 correction | 30 | 19 | 11 |
| F05 clarification | 30 | 22 | 7 |
| F06 factual/relational | 30 | 17 | 12 |
| F07 explanation levels | 30 | 19 | 9 |
| F08 reasoning multistep | 30 | 18 | 11 |
| F09 procedure/rollback | 30 | 20 | 9 |
| F10 complex prose | 30 | 22 | 8 |
| F11 scientific literature | 30 | 20 | 9 |
| F12 mixed code/math | 30 | 20 | 10 |
| **TOTALE** | **360** | **236 (66%)** | **117 (32%)** |

F. aveva ragione: **due terzi dei turni di conversazione ordinaria finiscono a
muro**, e persino il phatic — la cosa più facile — ne perde metà.

## 2. Prima di insegnare in massa: dove arriva l'insegnamento

Insegnare una forma che nessun consumatore legge scrive in un cassetto che
nessuno apre — il vecchio odore di `greeting(ahoy)`: vero in KB, invisibile nel
comportamento. `scripts/teach_ladder_audit.py` misura, per etichetta di
`learnable/3`, se un membro insegnato **cambia davvero il turno dopo**: è la
coordinata `taught_member_effective` di D28.

```text
TaughtMemberEffective = 13/16
```

I tre «non cambiati» non sono fallimenti: `yo`, `fair enough` e `nice one`
erano **già** gestiti prima della lezione, quindi non cambiare è corretto. Dove
c'era un buco, la lezione lo ha chiuso 13 volte su 13. Via libera.

## 3. Il giro massiccio (GD2)

`docs/labs/apprendimento-assistito/2026-08-31-gd2-variety-lexicon.tsv`: **200
forme colloquiali reali**, italiano e inglese, otto classi — attacchi informali,
stanchezza, noia, buonumore, frustrazione, accordo, incoraggiamento, battuta.

Ogni forma entra **parlando**, con l'atto che una persona compie:

```text
learn "sono uno straccio" as a sign of tiredness
learn "spot on" as a way of agreeing
```

`scripts/teach_lexicon.py` non asserisce niente da un canale laterale: se una
forma non entra così, non entra (`LEARN_PROTOCOL` §1.1).

```text
forms=200  taken=200  not_taken=0
routed 193 clause(s)      # 7 erano già note
```

Casa: `kb/core/reactions.p0`, trovata dal router senza intervento.

**Processo nuovo, senza reinsegnare nulla:**

```text
> weilà              -> Ciao! Di cosa ti va di parlare?
> sono uno straccio  -> Sembra un momento un po' fiacco. Teniamola leggera…
> spot on            -> Sono contento che regga. Cosa facciamo adesso?
```

E il cue **sopravvive dentro una frase lunga**, che era il dubbio vero:

```text
> Bella, tutto a posto?              -> Ehilà! Sono qui. …
> Ci sta, ma domani ho poco tempo.   -> Sono contento che regga. …
```

## 4. Il delta, e come va letto

| | muri | move_match |
|---|---:|---:|
| prima | 236 | 117 |
| dopo | **226** | **128** |

Sul totale è **+3%**. Detto così sembra poco, e sarebbe la lettura sbagliata.
Dove il lessico *poteva* agire ha agito:

| famiglia | match prima → dopo |
|---|---|
| F01 phatic | 14 → **18** (+29% relativo) |
| F05 clarification | 7 → 9 |
| F06 factual | 12 → 13 |
| F08 reasoning | 11 → 13 |
| F02 slang/register | 11 → 12 |

**Dieci famiglie su dodici non sono sociali**, e un lessico colloquiale non
doveva toccarle — se le avesse toccate sarebbe stato un allarme, non un
successo. Il risultato onesto è: *il lessico ha fatto esattamente ciò che un
lessico può fare, e niente di più.*

E la misura ha fatto una cosa che vale più del delta: **ha localizzato i 226
muri rimasti**. Non sono sparsi — vivono in F03 coref (24), F10 prosa (21),
F05/F09/F11/F12 (20 ciascuna). Sono i fronti della catena SC, non del lessico.

## 5. Il moltiplicatore che manca, misurato

Insegnare in massa si scontra con un muro che nessuna quantità di forme supera:

| turno | esito |
|---|---|
| `you're a legend` (insegnata) | ✓ battuta riconosciuta |
| `you are a legend` | ✗ **«Alright — I am a legend now.»** |
| `perché non funziona` | ✗ «Annotato: lo tengo come stato attuale.» |
| `cmq ciao` | ✗ muro |
| `comunque ciao` | ✓ «Arrivederci!» |

Tre cose, e la seconda è la più grave:

1. **Una variante ortografica azzera la lezione.** Apostrofo, accento,
   abbreviazione di chat: ogni variante costa una lezione propria, quindi un
   lessico di 200 forme ne vorrebbe 600 e ne mancherebbe comunque. È un
   frasario travestito da corpus.
2. **Il fallback non è un muro, è un misclaim.** A un complimento parrot0 ha
   risposto affermando qualcosa **su di sé** («I am a legend now»): la specie di
   D29, la risposta più forte della propria fonte, in un posto nuovo.
3. Non esiste **nessuna classe** di variante: `grep` su `kb/core` trova solo
   `lexeme(abbrev)`, che è un'altra cosa.

Da qui l'ipotesi **D34** e la voce **GD3**: una forma ha una famiglia di
varianti, e la variante non è una lezione nuova.

## 6. Conteggio

| Categoria | Conteggio |
|---|---:|
| `W` fatti del mondo | 0 |
| `L` fatti linguistici | **193** (`intent_cue/2`, otto classi, it+en) |
| `C` costruzioni | 0 |
| `X` invalide | **0** |

Le forme sono uso colloquiale corrente, comune e non regionale, del tipo
registrato dai dizionari dell'uso; nessun neologismo effimero, nessuna forma
inventata, niente nonce words. Sono conoscenza linguistica vera e destinata a
restare.

## 7. Limiti dichiarati

1. `che noia mortale` riceve il template della **stanchezza** invece che della
   noia: le due famiglie condividono una resa e la distinzione insegnata non si
   vede in uscita.
2. `Ci sta, ma domani ho poco tempo.` risponde all'accordo e **ignora la seconda
   metà**. È il mantra #10 sul turno composto, e D29 di nuovo.
3. Il probe è euristico: `move_match` guarda superfici, non semantica. Serve a
   localizzare, non a certificare.
4. Il corpus GD1 non è held-out rispetto a chi lo ha scritto. Il prossimo giro
   dovrebbe misurare su turni raccolti, non redatti.
