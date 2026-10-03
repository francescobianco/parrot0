# Piano sintetico — rendere parrot0 davvero comparabile a un LLM

> **Data:** 3 ottobre 2026 · stato dell'albero `gen510-riparazioni@8991be44`.
> **Natura:** questo **non** è un piano nuovo. È la riduzione di 93 documenti in
> `docs/plans/`, dei 27 mantra, delle 4 cartelle della letteratura radice e di una
> sessione `make chat` tenuta lo stesso giorno, a **sette interventi in un ordine**.
> Se un intervento qui non trova la sua fonte, è inventato; se una fonte non
> trova il suo intervento qui, è un caso non massimizzato — cioè lavoro perso.
>
> **Perché ora.** Tre fatti, non tre opinioni.
> 1. Una sessione `make chat` reale mostra che il difetto non è la conoscenza:
>    «perché il cielo è blu» → Rayleigh, corretto; «what is the capital of
>    France?» → «Paris.»; **«and of Italy?» → «Non capisco ancora»**;
>    «se Roma ha 3M e Milano 5M?» → **«milano, napoli, torino, Rome.»**;
>    «tell me a story» → **«It was a mysterious it.»** e la seconda storia è
>    **identica byte per byte**; «spiegamelo passo passo» → muro; «one more story,
>    please» → «I don't know about please yet».
> 2. La lettura statica del codice dice **perché**: il protagonista della storia è
>    scelto per forma, non per comprensione (`src/brain/30-generation-reading.c:1344`),
>    e la guardia che lo decide è una catena `||` di **quindici classi lex private
>    con un solo membro ciascuna** (`:1337-1343`, `*_lex990…_995_3`) — cioè
>    l'antipattern del mantra #19(a) verificato sul campo, in un modulo che
>    contiene già l'ammissione «questo generatore sa riempire uno schema, non
>    raccontare» (`:1358`).
> 3. La misura non è in grado di certificare nessuna di queste affermazioni:
>    quattro target `make` sono morti, il gate `core` passa su **zero** casi,
>    `docs/capabilities/manifest.json` è di ~190 generazioni fa, e l'unico
>    confronto vero con un LLM è `tests/challenge/` (parrot0 **10** vs FreeBuff
>    **200**, 2026-09-04).
>
> **Vincoli che questo piano non può violare** (i 24 capoversi rossi di
> `parrot0-forge-master-plan.md:2013-2040`, i 27 mantra, `PRINCIPLES.md`).
> I tre che lo vincolano di più, qui ricordati: **nessun LLM nel runtime**;
> **la voce è conoscenza** (#16); **una soluzione vale di più se amplia ciò che si
> può insegnare** (#26).

---

## 1. Che cosa significa «comparabile» — una scala, non un voto

I tre bersagli dichiarati nella letteratura sono diversi e vanno tenuti separati,
altrimenti ogni piano ottiene un numero decente sull'uno e deludente sugli altri
(`mimic-llm.md:26-31`, `parrot0-forge-master-plan.md:38-41`,
`interlocutore-di-frontiera.md:33-42`):

| | bersaglio | chi lo misura | stato |
|---|---|---|---|
| **B1** | *non* diventare un LLM in miniatura | — | non-goal, fissato |
| **B2** | parità su **esiti verificabili** (coding) | VTC = 1 | 0/5 engaged, freebuff 200:10 |
| **B3** | interlocutore naturale, giudizi **graduati e appresi** che restino conoscenza ispezionabile | nessuno strumento unico | non dichiarabile oggi |

Questo piano lavora su **B3**, che è l'unico dei due bersagli *verificabili con
l'unica tecnologia disponibile in locale* (un modello piccolo che conduce, giudica
e riprova: `llm-challenge.md`), e prepara B2 accettando che B2 ha bisogno del
ciclo `goal→action→observe→verify→repair` che oggi ha **una** istanza dimostrata
(l'adattatore di sort, `coding-agent-guided-rounds-dossier.md:346-350`).

### La scala, con gli strumenti che esistono

| livello | che cosa chiede a parrot0 | strumento oggi | oggi |
|---|---|---|---|
| **L0** | *rende conto di tutto il turno?* | nessuno — `reading_residue/3` ha **0 fatti** | — |
| **L1** | *sa cosa non sa?* | `llmscore-probe`, 70 prompt | 63% buona · **10% sbagliata · 27% estranea** (`quanto-manca.md:13-16`) |
| **L2** | *regge 20 scambi?* | `long-chat-bench`, `longtalk-bench` | 3–5 coerenti (`long-conversation.md:12`); 2/15 col partner |
| **L3** | *la risposta porta la prova?* | `inferenza_compositiva`, `reasoning_operators` | `composed/3` su **2%** dei template lunghi |
| **L4** | *risponde onestamente quando manca?* | `arrests.p0`, `compensates/2` | **1** riga: `repair_surface` |

**La definizione operativa di «comparabile» che useremo d'ora in poi:**
> su un insieme dichiarato di compiti, parrot0 e un LLM ottengono un punteggio
> **entro una banda** (non zero-v-one) e, dove il risultato è verificabile, una
> traccia. Fuori dall'insieme dichiarato, parrot0 dichiara il gap.

È l'unica definizione che non premia i template («non dichiarare risolto un limite
sulla base di un template riuscito», `breakthrough-limits.md:1587-1588`).

---

## 2. Le sei cause, non i novanta sintomi

Ogni causa è un difetto **di struttura** con un solo oggetto mancante. Il test
che le identifica è il mantra #23: *due pezzi che fanno bene il loro lavoro
producono insieme un errore → il difetto è l'oggetto su cui dovrebbero
accordarsi e che nessuno dei due può consultare perché non esiste.*

### R1 — Il turno intero non è mai letto prima del dispatch

L'evidenza viva è la più economica di tutta la letteratura: una domanda che
parrot0 sa fare, seguita dalla sua ripresa con `and of Italy?`, è un muro. Non
manca la conoscenza (Roma e capitale sono in KB): manca **l'oggetto «che cosa era
aperto e a cosa si riferisce questo turno»**.

Evidenza statica: `reading_extent/3` esiste **solo per le frasi di un documento**
(`kb/core/document-claims.p0:615`), `reading_residue/3` non esiste in nessun
file; `appropriate_move/2` non esiste; `move_addresses` è in un solo file.
`turn-arbitration.md:531` dichiara **S4 (copertura) ⛔ assente**, e il mantra #21
misura **74 facoltà su 80 che non toccano mai il frame**.

> Un modulo che risponde senza aver letto il turno non è un bug di priorità: è
> un turno a cui si è risposto **senza averlo letto** (`MANTRA.md:510-518`).

### R2 — La generazione riempie schemi invece di comporre

«It was a mysterious **it**» è uno slot non legato, non un errore di lessico.
`composed/3` esiste (`kb/core/composition.p0`) e funziona su una famiglia sola:
23 famiglie su 1179 template, mentre **176 template lunghi non hanno nessun
segnaposto** (`inferenza-compositiva.md:19-40`). La regola del piano è già
scritta e non è stata eseguita: *uno stadio che fallisce scompare invece di
mentire* (`:140-179`).

La prova che il caso non è un caso: `story_default` esiste come fallback per
**cinque** slot (`30-generation-reading.c:1351-1414`) e il commento del file dice
che un `story_default` al suo posto sarebbe **peggiore**
(`quanto-manca.md:200-205`). Il generatore è già stato giudicato dal suo
autore e continua a girare.

### R3 — Non esiste un oggetto unico di «stato del dialogo»

Il tabellone c'è: `open_issue` in 12 file, `max_qud` in 8
(`kb/core/issues.p0`). Non ha **consumatori**: le leggi L3 («una risposta
parziale stringe, non riapre») e L4 («una nuova domanda sostituisce, non cancella»)
di `dialogica.md:146-157` sono le due che il live chat dimostra mancanti, e
`D49` (`unfinished/resumable/next_action`) ha **zero occorrenze in `kb/`** pur
essendo referenziato da tre piani. Un registro di issue senza chi le riprende
è un contabile senza ufficio (`frontier-kb-natural-dialogue.md:521-529`).

### R4 — La condotta è ancora in C

| misura statica oggi | valore |
|---|---|
| `TODO(kb-first)` in `src/` | **221** |
| siti `kb_cue_match` | **830** |
| `strcmp`/`strstr` in `src/brain/` | **791** |
| `intent_cue` in KB | 3459 |
| `response_template` in KB | 1908 |
| `turn_form` in KB | 1466 |
| classi lex private a **un solo membro** in un solo modulo | **15** |
| moduli con testata `MODULE REVIEW` | **9** su ~80 |

Il caso delle quindici classi è la prova che **la coda non scende da sola**: sono
nate dalla stessa catena, nello stesso file, e nessun intervento previous le ha
toccate. Non è un problema di volume, è un problema di *chi attraversa la coda*.

### R5 — L'acquisizione non chiude il turno

`kb/core/arrests.p0:428-432` dichiara `⛔ AZIONE NON ANCORA ESEGUIBILE` per le due
azioni che servono (`read_source`, `ask_user`); esiste solo
`compensates(repair_surface, surface_variant)` (`:440`). `address/3`, il passo che
`autocrescita.md:456` dichiara «**è il piano**», non è mai stato costruito. Il
risultato è il sintomo M10 di `interlocutore-di-frontiera.md:319`: l'offerta di
cercare una cosa che non sa viene persa, e il turno muore lì.

### R6 — La KB non sa di che specie sono le sue relazioni

Il caso del mantra #23, nato due giorni fa: «Reefs are formed of colonies» è
corretto, «what are colonies made of?» rispondeva «Reefs» — la KB non sapeva
che quella relazione è *whole-part*. Lo stesso difetto spiega «Roma 3M vs Milano
5M → lista di città»: non manca il confronto, manca **la relazione d'ordine col
suo verso** (mantra #12: ogni numero deve avere un ruolo prima di fare
aritmetica). `relation_kind` è nelle intenzioni di `the-magic-of-apply.md` Parte
VII e **⛔ non verificato**; `kb/core/bridges.p0` ha già il consumatore giusto
(`solve_bridges` in `src/kb.c:3712`) — manca solo la dichiarazione.

---

## 3. I sette interventi

Ogni intervento dichiara: **difetto che chiude · cosa · dove · gate di crescita
a runtime · ablation**. Il gate è un test che **insegna qualcosa parlando** e poi
lo ritira: senza ablazione non è un test, è un golden (mantra #2, #16, #24 #3).

### I0 — La yardstick (prima di tutto, e non è un trucco)

**Chiude:** l'impossibilità di certificare qualunque affermazione di parità.

1. **Riparare o cancellare i target morti.** `bench`, `bench-mmlu`, `bench-bbh`,
   `bench-superglue-local` leggono `tests/cases/*.chat`, che è stato cancellato a
   gen346: oggi stampano `score: 0.00%` e sono nel manifest. `model-graph` e
   `reasoning-operators` chiamano script cancellati il 2026-08-28. Il gate `core`
   dichiara `tests/tools/run.sh`, che oggi gira **zero** casi ed esce 0: **una
   porta che passa quando non c'è niente** è peggio di una porta rossa.
   *Cosa:* ognuno dei due esiti è accettabile, l'attuale no. Preferibilmente il
   primo, e in ogni caso `benchmarks.json` non deve più mentire.
2. **`manifest_audit.py` deve vedere la bugia che esiste per nascere.** Oggi
   verifica esistenza, exit-contract e dipendenze: **non** verifica che lo
   `script` della riga sia ciò che il `target` esegue. è il buco che ha lasciato
   passare il drift `core`.
3. **Un solo numero, in un solo file, rigenerato da zero.**
   `make parity` = envelope dichiarato + tre contugi LLM + VTC, con
   `commit` e `brain_version` stampati dentro. Oggi il numero di parità esiste in
   tre posti (`LLMSCORE.md` 0/20 del 28 luglio, `RULESCORE.md` 0/25, lo
   scoreboard `tests/challenge/` 10:200) e nessuno aggiornato.
4. **`llmscore-probe` cresce di una colonna e di una coda.** Il terzo secchio
   («estranea») è già il dato più informativo (`quanto-manca.md:19-21`: *una
   risposta estranea è peggiore di un muro*) ed è ancora misurato a mano. E
   sotto il 70% di risposte corrette sta una coda di **varianti di classe**
   generate da quel prompt (mantra #15): oggi il banco copre il prompt, non il fascio.
5. **Una riga di latenza.** `soft-test` è già il gate de facto a 15 s e la soglia
   `!timeout 1` è già rossa; nessuno stampa il numero.

**Costo:** una generazione. **Perché prima:** senza questo, ogni numero di questo
piano è una convinzione che scade — che è la maledizione che
`armonizzazione-piani.md:146-148` scrive: *un piano che non si ridata è una
convinzione che scade*.

### I1 — Il residuo diventa un oggetto (copertura al posto di primo-match)

**Chiude:** R1.

**Cosa:** `reading_extent(Turn,Node,From,To)` e `reading_residue(Turn,Node,Residue)`
portati dal livello documento (`document-claims.p0:615`) al **frame del turno** in
scope `turn_N`, con `residue_kind/2` (segmentazione, coreferenza, costruzione,
enumerazione, ignoto). Poi S4 di `turn-arbitration.md:422-449`: fra due
rivendicazioni legittime vince **quella che rende conto di più parte del turno** —
misurabile oggi con `input_segment`/`segment_role`/`faculty_for` già esistenti.
`/debug coverage` stampa extent, residuo, e i residui diventano **agenda
frequenza-ordinata** di ciò che parrot0 non sa ancora leggere.

**Dove:** `kb/core/turn-frames.p0`, `kb/core/input-structure.p0`, un solo posto di
dispatch. Il meccanismo non è nuovo: `claim_reading_extent/1` esiste già e nessuno
lo consuma.

**Gate:** un turno con due richieste → o la risposta rende conto di entrambe, o
nomi quella che ha lasciato. Ablazione: togliere `reading_extent` → la scelta
torna first-match e il benchmark torna al primo-match.

### I2 — Un solo oggetto lettore↔produttore (chiude il *cassetto senza maniglia*)

**Chiude:** il giunto citato da tutti e chiuso da nessuno
(`universal-comprehension.md:389-396`): `extract_frame/2` **ricostruisce una
stringa piatta** e produce `book_red` (testa e modificatore fusi, determinante
perso, ordine invertito). Esistono due percorsi e vive il distruttivo.

**Cosa:** `reading_choice(Turn,Node,choice(Value,Evidence))` — che L2 ha già e che
sta in 605 righe di `kb/core/reading-choices.p0` — diventa **l'unico oggetto su cui
il lettore e il produttore di fatti si accordano**. I lettori a stringa si
ritirano **classe per classe**, con la tabella dei gemelli scritta prima
(mantra #24 #2: un lettore privato è un'esplorazione con scadenza, e finché non
è migrata «non conta come capacità in nessun resoconto»).

**Gate:** il banco delle 13 frasi di `l4-growth-handoff.md` (1 frame IR sbagliato →
10 corretti) più ablation. **Costo:** è il lavoro più invisibile e più
moltiplicatore: tutto il resto lo rende possibile.

### I3 — Una sola coppia comporre/rispondere (chiude «It was a mysterious it»)

**Chiude:** R2. **Fase 3, non fase 1** — vedi §5.

**Cosa:**
- **La storia.** Il protagonista non si sceglie più per forma. Si sceglie dalla
  comprensione del turno (`turn_focus`, `max_qud`, `topic`), oppure si declina
  informando di che cosa non è stato capito. **`story_default` ritirato per lo
  slot del soggetto**: un fallback che promuove un token a personaggio è un
  misclaim travestito da generatore, e il file stesso lo dice.
- **Le 15 classi private** spariscono con il ramo che le usa (ora: la catena
  `||` deve diventare `turn_pattern/3`, mantra #19(b)).
- **La composizione** sale sulle famiglie lunghe: `stage_has_claim/2`,
  `composed/3`, `composition_truncated/2` (la saturazione come fatto, non come
  silenzio), e l'arbitrazione fra stadi per evidenza dichiarata, non per ordine di
  clausola.

**Gate:** (a) due richieste di storia diverse **non** producono la stessa frase;
(b) nessuno slot prende un valore da una lista di fallback;
(c) la seconda storia è **diversa dalla prima** — oggi è identica byte per byte,
e questa è la prova più economica che il generatore è un template;
(d) ablation: togliere `story_arc` → la storia perde l'arco e si vede che l'arco
era conoscenza, non testo.

### I4 — Un solo oggetto di stato del dialogo (chiude «and of Italy?»)

**Chiude:** R3.

**Cosa, in quest'ordine:**
1. **D49 — la mossa `continue`.** `unfinished/1`, `resumable/1`, `next_action`
   come viste su `open_issue` e `budget_exhausted`, più
   `appropriate_move(Context, answer|clarify|qualify|repair|acknowledge|resume)`.
   Non è una parola, non è una facoltà: è la mossa che chiude l'ultima
   incompiuta. È **l'unico gradino che fa convergere colla, comprensione e frontier
   senza scriverne un quarto** (`armonizzazione-piani.md:168-180`) ed è
   falsificabile per la prima volta (`continue-as-resumption.md:106-175`).
2. **`move_policy/2` e `claim_guard/2` diventano la politica dichiarata** che il
   dispatch consulta. *«Perché non hai risposto tu?»* deve poter diventare una
   risposta alla stessa maniera in cui `module_review/4` lo è già diventato
   (`kb/core/module-review.p0:64-101`).
3. **Le due leggi mancanti, L3 e L4** (`dialogica.md:146-157`), più la retention
   **per citazione** (`retention_cite/2`) invece che per numero di turno: un
   counter è sbagliato per costruzione, perché un issue aperto al turno 3 è ancora
   aperto al 12 mentre il turno che l'ha aperto è già caduto.
4. **`constraint_limit/3` applicato in un solo punto.** `brain_respond` ha undici
   uscite: un vincolo che vale per alcune non è un vincolo
   (`the-linguistic-glue.md:312-341`). Oggi «tieni la risposta breve» viene
   **riconosciuto, confermato e non applicato** — il caso peggiore, perché ha
   promesso.

**Gate:** (a) «capital of France?» → «Paris.» → «and of Italy?» → «Rome.»;
(b) «spiegamelo passo passo» → una risposta **a stadi**, non un muro; (c) «one more
story, **please**» → la cortesia non è un'entità sconosciuta; (d) ablation: chiudere
l'issue senza risposta → la domanda non si ripete.

**Priorità dichiarata:** è l'intervento che l'utente sente per primo, ed è l'unico
che produce una capacità che l'LLM non ha per costruzione (possono essere
riscritti in parallelo).

### I5 — La condotta in KB (titolo al turno, non posizione nell'array)

**Chiude:** R4. **Nodo costante, non una fase finale.**

1. Applicare il default già scritto: **senza lettura del frame e senza review,
   una facoltà è `fallback`** (`MANTRA.md:524-526`). I 9 moduli con testata
   `MODULE REVIEW` su ~80 diventano la coda, e la coda si chiude **demotando** i
   maturati `transitional` — non insegnando loro niente (mantra #21: a un modulo
   immaturo una `faculty_yield` è la risposta sbagliata anche quando funziona).
2. **La coda dei 221 `TODO(kb-first)` ha un solo criterio di ordinamento:** il
   danno. Prima i duplicati (`quando due sorgenti divergono, vince quella che
   nessuno può correggere`), poi i predicati di dominio compilati in C (250), poi
   le superfici, e infine i ~3000 letterali di parola **per classe, non per
   riga** (`kb-first-audit.md:44-48`, `messages-are-knowledge.md:178-193`).
3. **Regola operativa di avanzamento:** quando si tocca un ramo che porta un
   `TODO(kb-first)`, lo si chiude lì. È il momento più economico in cui costerà
   mai.

**Gate (ed è il test del mantra #17):** *«il generatore di poesie non deve
rispondere se non c'è anche X»* → vale dal turno dopo. Va nel `.p0t` come
**crescita** con retract, non come golden.

**Avvertenza esplicita:** I5 e I1/I2 vanno **di pari passo**, I5 non dopo. Se la
copertura sceglie il lettore e due moduli hanno pretese unite, l'arbitrato peggiora:
la copertura senza legittimità è un arbitro che sceglie il migliore dei due
ladri.

### I6 — L'acquisizione chiude il turno

**Chiude:** R5.

**Cosa:** `compensates(read_source, knowledge)` e
`compensates(ask_user, reference)` — le due righe dichiarate non eseguibili — più
`address(Term, Source, Predicate)` con la sua prova di minimalità (le cinque
strategie A1–A5 si confrontano sul **solo** numero: quanti arresti diventano
indirizzi, e quanti di quelli fanno finire il turno, `autocrescita.md:497-528`).
Niente di più: la fonte non si archivia, la prosa arriva in memoria e passa
dallo stesso lettore del testo incollato.

**Gate:** un fatto che manca → fetch → **il turno finisce**. Più la verifica di
non-regressione: dopo il giro, il turno che finisce non ha lasciato traccia della
sorgente oltre il fatto, e il replay del turno senza rete **declassa con onestà**.

**Perché è fase 3:** un indirizzo ha senso solo se prima esiste il **residuo** (I1).
Costruire `address/3` prima di I1 significa indirizzare la lacuna sbagliata — che è
esattamente il `machinery_gap` che `question-emergence.md:59-73` ha già descritto
come «una parola a caso».

---

## 4. L'ordine, e perché questo ordine e non un altro

```
Fase 0   I0   la yardstick                                    ~1 generazione
Fase 1   I1   il residuo come oggetto ─┐ insieme, perché la
         I2   lettore/produttore unico ┘ copertura non ha significato senza I2
Fase 2   I4   stato del dialogo (continue, L3/L4, vincoli)
         I6   l'acquisizione chiude il turno
Fase 3   I3   composizione e generazione
         I5   nodo costante: titolo al turno, coda dei 221 TODO
```

**Le quattro ragioni di questo ordine, e sono le uniche che contano:**

1. **I0 prima, perché il resto si misura.** Quattro target morti e un gate che
   passa su zero casi significano che oggi il progetto non può distinguere un
   miglioramento da un artefatto. Costruire I1–I6 sopra un metro rotto è il modo
   più economico di produrre convinzioni che scadono.
2. **I1 prima di I3, e questa è la scelta che il corpus non fa.** Il piano di
   generazione (`generative.md`, `generative-prolog.md`) è il più affascinante e il
   meno pronto: generare **composizioni corrette** prima che il turno sia letto
   produce risposte composte sbagliate, che sono peggio di risposte banali perché
   sono convincenti. È la no-deception del mantra #7 applicata all'ordine del
   lavoro.
3. **I4 prima di I5, perché il furto è un difetto cognitivo.** Applicare il
   criterio di legittimità (I5) sopra una copertura che non esiste (I1) dà un
   arbitro che sceglie il migliore di due ladri. Viceversa, la copertura (I1) senza
   legittimità non sceglie niente.
4. **I6 dopo I1** per la ragione dell'ancora: `address/3` senza residuo indirizza la
   lacuna sbagliata.

**Il ritmo, che è il mantra #22 e non una mia invenzione:** un circuito per sessione
— se ne saltano fuori due, il secondo **si scrive**; poi si massimizza la classe
del circuito (casi, non prompt); poi si cerca una declinazione e si passa al test
del riuso (*sto aggiungendo un consumatore a una lettura che c'è, o sto scrivendo
una seconda lettura?*). **Il circuito è la parte cara, i casi sono la parte gratis**
(`MANTRA.md:539-542`).

---

## 5. I criteri di uscita, e da dove si misurano

> I numeri sotto sono **da documenti e da ispezione statica**, non rimesi oggi:
> la sessione `make soft-test` di oggi non ha fatto partire il demone
> (`obj/test-engine.log`) e `make measure` non è stato completato. Le date sono
> indicate; `armonizzazione-piani.md:146-148` pretende esattamente questo.

| livello | strumento | oggi (e quando) | bersaglio | forma della verifica |
|---|---|---|---|---|
| L0 | `reading_residue` | 0 fatti (oggi) | residuo presente per ogni turno lungo | ablation per lettura |
| L1a | `llmscore-probe` | 0 muri / 70, 31% era il baseline (2026-09-02) | **non è il bersaglio**: è già saturo | — |
| L1b | idem, terzo secchio | **27% estranee**, 10% sbagliate | estranee **≤ 8%**, e **mai più alte delle sbagliate** | colonna dichiarata, non manuale |
| L1c | `llmscore-probe` varianti | assente | ogni classe con coda di varianti (mantra #15) | generatore deterministico |
| L2a | `long-chat-bench` | 3–5 scambi coerenti (misura del 2026-08) | **20 scambi sostenuti** | giudizio + transcript |
| L2b | vincoli | riconosciuti e non applicati | applicati in **un** punto | ablation del vincolo |
| L3 | composizione | 2% dei template lunghi | ogni famiglia lunga con ≥ 1 stadio che sparisce quando non regge | ablation per stadio |
| L4 | `compensates/2` | 1 riga | 3 righe, e il ciclo acquisisce→finisce | ablation della rete |
| — | **parità** | `tests/challenge` 10 : 200 (2026-09-04) | un **numero solo**, in un file solo, con l'impronta del commit dentro | `make parity` |

**La soglia che conta più di tutte le altre**, perché è l'unica che non si può barare:
> un incremento vale se **insegnando una forma nuova a parlando** il comportamento
> cambia dal turno dopo, e **ritraendola** torna indietro. Un golden che resta
> verde con la lezione ritrattata è un falso.

---

## 6. Cosa non fare — le otto tentazioni che questo piano crea

1. **Non generare con un modello esterno.** Sarebbe la soluzione più economica e
   falsificherebbe il perimetro dell'esperimento
   (`breakthrough-limits.md:741-743`). L'LLM può condurre, giudicare e proporre
   (`llm-challenge.md`); **non può essere l'oracolo finale e non promuove nulla**.
2. **Non coprire i sintomi con un vocabolario di frasi in C.** «`please` come
   stopword», «`and` come congiunzione di ripresa», «`spiegamelo` come cue di
   didattica» — tre righe, tre prompt verdi, **zero leverage**
   (`parrot0-forge-master-plan.md:1473-1479`). Ognuna è un caso della classe che
   I1/I4 chiudono; ognuna è un inchiodo nel `src/`.
3. **Non cominciare da I3.** È il pezzo più bello e il meno pronto (vedi §4.2).
4. **Non alzare i soffitti per ipotesi.** `KB_MAX_ARGS = 4`, `KB_MAX_BODY = 16`
   (`src/kb.h:20,26` — **correzione necessaria: `AGENTS.md` e `abstraction-ceiling.md`
   dicono ancora 8**), `KB_TERM_LEN = 512`. Un soffitto si alza quando una campagna
   reale lo ha colpito e una volta reso osservabile come errore
   (`parrot0-forge-master-plan.md:556-559`).
5. **Non misurare su KB amputata.** Nessuna misura di questo piano vale se il
   soggetto è stato ridotto: la KB viva è il soggetto (`PRINCIPLES.md:109-117`).
6. **Non insegnare la condotta a un modulo immaturo** — si retrocede. Confondere
   le due classi è la causa del whack-a-mole, ed è un errore già commesso
   (`MANTRA.md:474-479`).
7. **Non chiamare "migrazione" un template vuoto.** `response_template(k, "{text}")`
   è un `printf` in costume: *se cancello questa riga, cambia ciò che parrot0 dice,
   o solo se lo dice?* (mantra #18b).
8. **Non scrivere un altro piano.** Questo è un ordine, non una coda. La coda
   esiste già e sta in `LEARN_TODO.md` e nei 221 `TODO(kb-first)`.

---

## 7. I residui che questo piano **non** chiude, dichiarati

`interlocutore-di-frontiera.md:441-521` lascia tre residui che nessuno dei sette
interventi tocca. Si dichiarano, non si mascherano:

- **R2 — giudizi graduati e appresi su spazi troppo grandi per essere enumerati**
  (plausibilità in KB, non un campo `confidence`). È il residuo che rende
  strutturalmente diversa la conoscenza da un peso. *«Se no, parrot0 resterà un
  ottimo interlocutore sul verificabile e rigido sul resto»*
  (`interlocutore-di-frontiera.md:555-558`).
- **R5 — generazione aperta.** I sette interventi rendono la composizione **corretta**;
  non rendono il testo **buono**. È la distanza fra "risponde bene" e "è degno di
  essere letto", e non è un difetto della KB: è il punto in cui l'assenza di un
  generatore statistico si fa sentire.
- **R6 — la coda della conoscenza tacita.** Il problema storico di Cyc
  (`interlocutore-di-frontiera.md:480-493`): da solo può tenere la distanza.

E una constatazione che va detta perché è la più importante di tutte:

> La **forma** dell'intervento, non la sua quantità, è ciò che tiene la distanza.
> Un parrot0 con 173 229 fatti e 6 607 regule che risponde «milano, napoli,
> torino, Rome.» a una domanda di confronto non è *indietro di dieci anni*: è
> **esattamente sullo stesso piano** di un sistema che ha capito la domanda e non
> ha la relazione. Il primo difetto non è la mancanza di conoscenza. È che la
> comprensione non è un oggetto che esiste prima della risposta.

---

## 8. Provenienza — dove sta ogni intervento

| I | fonte primaria | cofonti |
|---|---|---|
| I0 | `parrot0-forge-master-plan.md:244-263, 318-340` · `tests/bench/benchmarks.json` · `docs/capabilities/manifest.json` | `parrot0-100-failures.md`, `quanto-manca.md:119-131` |
| I1 | `frontier-kb-natural-dialogue.md:3986-4010` (D14) | `turn-arbitration.md:422-449` (S4), `MANTRA.md:510-526`, `l3-upgrade.md:51-59` |
| I2 | `universal-comprehension.md:389-396` | `l4-upgrade.md:64-90` (costituente), `MANTRA.md:412-426`, 655-706 (#24) |
| I3 | `inferenza-compositiva.md:140-179` | `generative.md:360-441`, `generative-leverage.md:15-47`, `quanto-manca.md:187-205`, `30-generation-reading.c:1332-1359` |
| I4 | `frontier-kb-natural-dialogue.md:6449-6520` (D49/D50) | `dialogica.md:43-175`, `continue-as-resumption.md:106-175`, `the-linguistic-glue.md:312-372`, `initiative.md:106-133`, `assistente-utile.md:104-165`, `MANTRA.md:510-526` |
| I5 | `MANTRA.md:163-189` (#17), `:190-237` (#18), `:238-320` (#19), `:450-527` (#21) | `turn-arbitration.md:98-215, 246-339`, `kb-first-audit.md:44-115`, `messages-are-knowledge.md:106-193`, `kb-first-c-gold-standard.md:157-163` |
| I6 | `autocrescita.md:428-465` | `autocrescita-v3.md:30-43`, `la-rete-come-memoria-profonda.md:301-333`, `dream.md:148-170`, `arrests.p0:428-440` |
| R6 | `MANTRA.md:607-653` (#23) | `the-magic-of-apply.md` Parte VII, `kb/core/bridges.p0`, `question-emergence.md:563-668` |

**Una nota sullo stato dei documenti, che è essa stessa un risultato di questo
piano.** Le 93 `docs/plans/` contengono stati dichiarati **non ridatati** — la stessa
tecnica dichiarata «11/11 crisp HELD» valeva 25/7 (`armonizzazione-piani.md:133-148`);
`docs/capabilities/manifest.json` è di ~190 generazioni fa; `LLMSCORE.md` è del 28
luglio e segna 0/20 su una scala che è cambiata due volte. **Nessun ✅ senza
generazione e senza il ratchet che lo prova.** Il primo atto di I0 è anche questo:
ridatare, o cancellare.

---

*Se una riga di questo piano non può essere chiusa da una lezione detta a
parlando, non è un intervento: è una spesa.*