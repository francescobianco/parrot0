# LLM challenge — un modello piccolo conduce, parrot0 risponde, si ripara, si rigioca

**Piano, 30 settembre 2026.** Nasce da una richiesta di F. dopo la prima
chiacchierata fra parrot0 e un modello locale:

> *«fai un file che ha il compito di eseguire delle sessioni live come quella
> fatta con un modello piccolo perfetto, lfm; analizzare la sessione, fixare i
> problemi riportati, tu come giudice, e rieseguire fino al miglioramento di
> parrot0. Ovviamente le lezioni di miglioramento a parrot0 vanno fatte con L4,
> o per contatto L3, o L2 ecc… comanda sempre il LEARN_PROTOCOL.md.»*

> **In una frase.** Un LLM piccolo e fluente (LFM2.5-1.2B, locale, LM Studio)
> conduce una conversazione che nessuno ha scritto; l'agente fa da **giudice**
> di ogni risposta di parrot0, sceglie il difetto che vale di piu', lo cura
> **insegnandolo parlando** quando e' conoscenza e **aprendo una strada in KB**
> quando manca il meccanismo, poi **rigioca la stessa conversazione** e una
> conversazione nuova. Il giro e' riuscito solo se il replay migliora e la
> conversazione nuova non peggiora.

---

## ▶ HANDOFF — 30 settembre 2026, notte (ripartire da qui)

**Stato.** Giro 1 chiuso e committato (`af0346fa`): tre cure di classe
(`choice_cue` unica, la porta degli incisi che legge le cessioni KB con la
forza marcata `exclamation`, le varianti tipografiche dell'apostrofo).
Replay +0,21 (baseline −0,04); conversazione nuova −0,21 (stato, non
guadagno). `make soft-test` verde; `cause.p0t` 8/8, `cause.it.p0t` 4/4
(allineato: test obsoleto), `user_situations.p0t` 56/56.

**Per riprendere:**

1. `~/.lmstudio/bin/lms server start` (lo script carica il modello da solo).
2. Replay del giro 1, per partire dal punto fermo:
   `scripts/live-talk.sh start 12 liquid/lfm2.5-1.2b en 0` e confronto con
   [2026-09-30-giro1-replay.log](../sessions/talk/2026-09-30-giro1-replay.log).
3. Trasferimento con la stessa apertura del giro 1:
   `LIVE_TALK_OPENER="Hey there! I just got back from a walk in the park. Do
   you like being outside?" scripts/live-talk.sh start 12`, confronto con
   [2026-09-30-giro1-transfer.log](../sessions/talk/2026-09-30-giro1-transfer.log).
4. **Giro 2, primo bersaglio:** il chiarimento su «first» che si ripete (5
   turni su 12 del trasferimento). Diagnosi da fare con `/debug turn` sul turno
   ripetuto: chi chiede (il piano di turno che risolve gli ordinali, gen513) e
   perche' la domanda aperta non si chiude quando l'altro risponde. La cura
   attesa e' KB: un chiarimento aperto e' un fatto della conversazione
   (`open_issue`/tabellone), la risposta lo chiude, e la stessa domanda non si
   rifa' sullo stesso referente.
5. Poi, nell'ordine di §«Giro 1 → Ordine del giro 2».

**Trappole note.** LM Studio ignora seme e temperatura con LFM2.5 (la
conversazione e' sempre la stessa: usare l'apertura). `live-talk.sh stop`
archivia con l'ora nel nome: rinominare in `AAAA-MM-GG-giroN-*.log`. Con Qwen3-8B
caricato insieme a LFM la GPU (Intel Arc, Vulkan) si divide e i tempi del
modello salgono: tenere caricato un modello solo. `intel_gpu_top` vuole `sudo`
(`perf_event_paranoid` = 4); senza, la frequenza si legge in
`/sys/class/drm/card1/gt_act_freq_mhz`.

## 0. Chi comanda

**[LEARN_PROTOCOL.md](../../LEARN_PROTOCOL.md) comanda.** Questo piano non ne
sostituisce nessuna regola; ne aggiunge soltanto la sorgente degli stimoli (un
LLM al posto dell'insegnante che esplora) e il giudizio del giro. In
particolare:

- **Lingua naturale, mai API travestita** (§1.1): le lezioni si dicono a parrot0
  in una sessione `live-teach`, come le direbbe una persona.
- **Conoscenza vera** (disclaimer): i contenuti portati dall'LLM non sono fonti.
  Se parrot0 deve *sapere* qualcosa che l'LLM ha nominato, il fatto si verifica
  prima (§4) e si insegna solo se vero e utile.
- **L'ordine dei meccanismi**: prima **L3 per contatto**, poi **L2 per schema**
  (dichiarandolo: «qui uso uno schema, perche' …»), poi **L4** quando la
  lezione riguarda il mondo di chi parla, di parrot0 o il trasferimento fra i
  due ([l4-upgrade.md](l4-upgrade.md)).
- **Quando la strada non c'e'** (la lezione non puo' entrare perche' manca il
  lettore, la cessione, il meccanismo), la sessione si ferma (live-teaching §3
  regola 6) e il lavoro diventa un passo di KB/motore con la domanda zero del
  [MANTRA](../../MANTRA.md): *parrot0 potra' impararne un membro nuovo domani,
  senza ricompilare?* Il C e' l'ultima risorsa, ed e' adattatore.
- **Classe, non istanza.** Il giudice nomina la SPECIE del difetto
  (memoria: *fenomenologia dei difetti*); una riparazione che fa passare solo
  la battuta dell'LLM e' un frasario, e non conta.
- **Test**: niente suite. `make soft-test` una volta se il motore cambia.

## 1. Il dispositivo

| pezzo | che cosa |
|---|---|
| [`scripts/live-talk.sh`](../../scripts/live-talk.sh) | `start [TURNI] [MODELLO] [LINGUA] [T]`, `watch` (per F.), `stop` (archivia in `docs/sessions/talk/`) |
| [`scripts/live-talk.py`](../../scripts/live-talk.py) | il conduttore: l'LLM parla (`M>`), parrot0 risponde (`P<` con il tempo del turno); oltre 30 s il turno e' `BLOCCATO` e la conversazione finisce |
| LM Studio | `lms server start`, `lms load liquid/lfm2.5-1.2b` — endpoint OpenAI su `localhost:1234`, motore llama.cpp Vulkan sulla GPU Intel Arc |
| [`scripts/live-teach.sh`](../../scripts/live-teach.sh) | la sessione in cui l'agente **insegna** la cura (il canale 1) |

**Riproducibilita'.** Con `T=0` (e il seme fisso) l'LLM dice le stesse battute
finche' parrot0 risponde uguale: il **replay** di un giro e' la stessa
conversazione, e il primo turno in cui diverge e' il punto in cui la cura ha
agito. La **conversazione nuova** (trasferimento) si fa con `T=0.7` o con
un'altra apertura.

**Perche' LFM.** E' il conduttore giusto *perche'* e' piccolo: fluido,
grammaticale, generico, senza agenda. Non e' un giudice (sbaglia i compiti di
grammatica; misurato il 30 settembre: «Me and him» → «My friend and him») e non
e' un oracolo di correttezza: e' **pressione conversazionale reale**. Il
giudice e' l'agente.

## 2. Il giro

1. **Gioca.** `scripts/live-talk.sh start 12 liquid/lfm2.5-1.2b en 0`.
2. **Giudica** ogni `P<` con le etichette del LEARN_PROTOCOL §6.1
   (`KNOWN_CORRECT`, `WALL`, `WRONG`, `IRRELEVANT`, `PARTIAL`, `AMBIGUOUS`) piu'
   le specie conversazionali, e scrivi la tabella del giro (§4).
3. **Scegli** il difetto che vale di piu': un `WRONG`/`IRRELEVANT` prima di un
   `WALL` (un misclaim e' peggio di un muro), una classe prima di un caso.
4. **Diagnostica** con `/debug turn` e `/debug trace` sul turno riproposto
   (non grep, non C).
5. **Cura**, nell'ordine: lezione L3 → L2 → L4 in `live-teach`; se la lezione
   non puo' entrare, strada in KB (cessione, forma, classe); motore solo se il
   meccanismo manca, con il bilancio C nel commit.
6. **Rigioca** la stessa conversazione (`T=0`) e una nuova (`T=0.7`).
7. **Misura e registra** il punteggio del giro (§3) e committa.

Il giro si ripete finche' il punteggio sale. Un giro che non migliora non si
nasconde: si scrive perche'.

## 3. Il punteggio del giro

Per ogni conversazione, sulle risposte di parrot0:

- **A** = adeguate (la risposta e' pertinente, vera, nel registro giusto; un
  muro onesto che dice che cosa manca vale mezzo punto);
- **M** = misclaim (`WRONG`, `IRRELEVANT`, racconto al posto della risposta):
  **ognuno pesa −1**;
- **S** = soste (tetto di tempo, `BLOCCATO`), **R** = ripetizioni della stessa
  risposta in una conversazione.

`punteggio = (A − M) / turni`. Il giro migliora se sale sul replay **e** non
scende sulla conversazione nuova. Il tempo mediano del turno si riporta accanto
(memoria: *slow is a bug*).

## 4. I giri

### Giro 0 — la prima chiacchierata (30 settembre 2026, 00:55, T=0.7, 6 scambi)

Transcript nella risposta all'utente di quella notte (prima del dispositivo di
replay). Specie viste: tetto di tempo su una domanda aperta («What part of this
topic fascinates you most?», 10 s); ripetizione della stessa frase fatta
(«Both can be the right choice…»); il turno composto rifiutato («Tell me the
statements one per turn»); in un'altra corsa il **racconto** al posto della
risposta («That's was a mysterious That's…» su «What's the main goal here?»).

### Giro 1 — 30 settembre 2026, notte (T=0, 12 scambi, LFM2.5-1.2B)

**Baseline** (transcript: [docs/sessions/talk/2026-09-30-giro1-base.log](../sessions/talk/2026-09-30-giro1-base.log)).

| # | l'LLM | parrot0 | giudizio |
|---|---|---|---|
| 1 | Hello! How can I help you today? | I'm a chatbot… you can ask me questions | adeguata |
| 2 | …What would you like to know? | That sounds nice… nobody has taught me | muro onesto (½) |
| 3 | …What part of this topic fascinates you most? | tetto di tempo (10,1 s) | **S** |
| 4 | Oh, that happens sometimes! …understand better. Are you curious about X or Y? | «Both can be the right choice…» | **M** (consiglio su una scelta: la domanda era su di lui) |
| 5 | …explore ideas or dive into details? | «That turn joins several statements…» | muro onesto (½) |
| 6 | Let's break it down: 1. … 4. … | «what you mean by «first»» | muro onesto (½) |
| 7 | …the initial steps or the overall process? | «Both can be the right choice…» | **M** (stessa frase fatta, **R**) |
| 8 | That’s a great way… What’s the main goal here? | un racconto su «That’s» | **M** |
| 9 | …the core of what you're trying to explore? | tetto di tempo (10,0 s) | **S** |
| 10 | …what’s one thing you're curious about? | nobody has taught me + non letto | muro onesto (½) |
| 11 | That’s a good start! Maybe you’re thinking… | «Learned: that’s a good start maybe you’re thinking.» | **M** |
| 12 | …What’s the next step you'd like to take? | «That turn joins several statements…» | muro onesto (½) |

A = 3,5 · M = 4 · S = 2 · **punteggio −0,04** · mediana del turno ~4,5 s.

**Le specie scelte, e le cure** (classi, non battute):

1. **Due classi con lo stesso nome** (turno 4). `choice_cue/1` era, in
   `user-situations.p0`, l'elenco delle superfici che chiedono un consiglio
   («should i», «dovrei»); in `scales.p0` due parole sciolte, `should` e
   `better`. «understand **better** … X **or** Y» diventava una richiesta di
   scelta. Cura KB: una classe sola (scales.p0 legge le superfici con
   `turn_cue_form`), e la domanda comparativa entra come superficie («which is
   better», «quale è meglio»).
2. **Una seconda porta di dispatch che non legge l'arbitrato.** Il prefisso del
   turno 4 da solo, «Oh, that happens sometimes!», riceveva **«Learned:
   causes(that, sometimes!).»**: `adjunct_peel` toglie l'inciso («oh,») e offre
   il resto a tutte le facolta' chiamandole una per una, senza le cessioni della
   KB. Cura: la porta chiede `p0_faculty_yields` come il dispatch principale
   (99-registry.c, adattatore), e la KB dice chi cede: un turno con «!» dichiara
   la forza marcata `exclamation` (gemella del «?»), e chi impara le cause la
   cede. ⚠ Il primo tentativo cedeva la forza `expressive`, che e' definita per
   residuo: ci cadeva «rain causes flood» (`cause.p0t` 5/8) — ritirato.
3. **La stessa lettera scritta in un altro modo** (turni 8, 11). Un LLM scrive
   «That’s» con l'apostrofo tipografico, e lo stesso turno con «'» si leggeva
   diversamente. Cura: `typographic_variant/2` in lexicon.p0 (’ ‘ ʼ → '), una
   porta all'ingresso del turno che riscrive le varianti che la KB elenca.

**Replay** (T=0; diverge dal turno 4, cioe' dove la cura ha agito):
A = 5,5 · M = 3 · S = 1 (+ un turno di 9 s) · **punteggio +0,21**.

**Rimasti, per il giro 2** (in ordine di valore):

- **«I see!» letto alla lettera**: «No, I can't see: I have no body.» (M). E'
  un idioma: si insegna parlando (L2 «when i say I see i mean …»), non si cabla.
- **Il turno che segue un'offerta viene preso come un «si'»**: dopo «Want me to
  learn about it?», «I'm tired.» riceve «I looked up «great way»…» (M, anche
  nel turno 12 del replay).
- **«Are you curious about …?» dopo un'esclamazione** → «You haven't told me
  that yet.» (la facolta' `personal` legge «you're trying to learn» come un dato
  dell'utente).
- **Tetti di tempo** su domande aperte sulle preferenze di parrot0 («What part
  of this topic fascinates you most?»): 10 s e nessuna risposta. E' il turno
  lento da curare per primo (memoria: *slow is a bug*).
- **Frasi di circostanza senza mossa sociale** («Oh, that happens
  sometimes!», «Wow, that sounds amazing!», «I'm tired.»): muri onesti ma fuori
  registro. Materia da lezione di condotta (LEARN_PROTOCOL, aperture e mosse).
- **Il racconto sui turni con «That’s»** e l'`I didn't keep that:
  «let's_try_a_quick_experiment_i'll»` che mostra atomi interni.
- **Punteggiatura dentro i fatti**: «Learned: causes(smoking, cancer.).».

**Trasferimento** (conversazione nuova, apertura «Hey there! I just got back
from a walk in the park. Do you like being outside?»; transcript
[2026-09-30-giro1-transfer.log](../sessions/talk/2026-09-30-giro1-transfer.log)).
Non c'e' una baseline di questa conversazione prima delle cure, quindi misura
lo stato, non il guadagno:

| # | parrot0 | giudizio |
|---|---|---|
| 1 | «No, I can't walk: I have no body.» (a «Do you like being outside?») | **M** |
| 2 | nobody has taught me | ½ |
| 3 | la definizione di machine learning (a «What's something you're learning right now?») | **M** |
| 4 | «Yes, I can learn.» (a «Do you find AI interesting…?») | **M** |
| 5, 8, 9, 10, 12 | «I am not sure what you mean by «first».» | ½ la prima volta, poi **R** ×4 |
| 6 | l'eco storpiata della battuta + una procedura d'indagine + una ricerca di «detection» | **M** |
| 7, 11 | tetto di tempo | **S** |

A = 1,5 · M = 4 · S = 2 · R = 4 · **punteggio −0,21**.

**La specie che pesa di piu' nel giro 2: il chiarimento su «first» che non si
chiude mai.** Cinque turni su dodici. Il piano di turno risolve gli ordinali e
chiede che cosa si intende per «first» ogni volta che la parola compare
(«What would you like to explore **first**?», «the **first** step»), anche
quando l'interlocutore ha appena risposto alla domanda: la domanda non ricorda
di essere stata fatta. E' la stessa famiglia del gen513 («un ordinale dentro la
prosa non e' la domanda del turno», turn-frames.p0) e della memoria
«smalltalk che non guarda la storia»: un chiarimento aperto deve sapere di
essere aperto, e la risposta dell'altro lo chiude.

**Il dispositivo, imparato stanotte.** LM Studio ignora seme e temperatura con
questo modello: con qualunque `T` e seme la conversazione e' la stessa. Il
replay e' quindi garantito; il trasferimento si fa cambiando l'apertura
(`LIVE_TALK_OPENER`).

**Ordine del giro 2:** (1) il chiarimento su «first» che si ripete; (2) il
turno dopo un'offerta preso come «si'»; (3) «I see» / «I just got back from a
walk» letti come domande sul corpo di parrot0 («I have no body»); (4) i tetti di
tempo sulle domande aperte sulle preferenze; (5) le mosse sociali per le frasi di
circostanza, insegnate parlando.
