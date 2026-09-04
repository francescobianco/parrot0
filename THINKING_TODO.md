# THINKING_TODO — la coda del pensiero e dell'agente di codice

> Due piani, una coda sola, perché si incrociano: il **thinking**
> ([`docs/plans/thinking.md`](docs/plans/thinking.md)) è lo strato che
> rielabora un output con meta-prompt fino alla chiusura della pipeline; il
> **coding agent** ([`docs/plans/universal-code-comprehension.md`](docs/plans/universal-code-comprehension.md))
> è ciò su cui quel pensiero avrà più da pensare. Tenerli in due code separate
> avrebbe nascosto proprio i punti in cui si toccano.

> **Checkpoint gen499.** A0, E3, la prima forma di E7, il contratto
> anti-specchio e la diagnostica dei meta-prompt sono ora eseguiti. Questa coda
> è stata riallineata al codice reale e al riesame di
> `docs/plans/universal-code-comprehension.md`: la latenza resta un esempio, non
> la destinazione della IR.

---

# ⛔ PARTE 0 — LA PRIORITÀ: far maneggiare a parrot0 un compito di coding

> **gen502.** Aggiunta in testa su richiesta di F. dopo la prima gara valida del
> banco. **Non sostituisce niente**: le Parti A/B/C e l'handoff H0-H15 restano
> integri e validi sotto. Questa parte dice soltanto *da dove si comincia*, e
> perché quell'ordine è diverso da quello che sembrava.

## 0.1 I risultati misurati, e sono cattivi

`tests/challenge`, run `gen502-m0b`, **match0 — difficoltà 1**: manca un file,
il seed non compila, il contratto è in un header di venti righe.

| | parrot0 | freebuff / deepseek-v4-flash |
|---|---:|---:|
| punteggio | **5**/100 | **100**/100 |
| durata | 6,6 s | 52,1 s |
| albero modificato | **no** — identico al seed | sì |
| primo check fallito | `artifact` (non ha creato niente) | nessuno |

I 5 punti sono `seed_integrity`: **i punti che si prendono non toccando nulla.**

E **match1 — difficoltà 4** (run `gen502-m1`), lo stesso giorno, dà lo stesso
risultato con un margine ancora più netto:

| | parrot0 | freebuff |
|---|---:|---:|
| punteggio | **5**/100 | **100**/100 |
| durata | 6,6 s | 604 s (budget esaurito, non fallimento) |
| albero modificato | **no** | sì — build intera, dodici check su dodici |

⭐ **Il punto che conta di questa seconda riga:** il compito difficile non è
stato *un po'* più difficile per parrot0. È stato **identico**: 6,6 secondi,
5 punti, cartella intatta, in entrambi. La difficoltà del task non ha cambiato
niente — perché parrot0 non è mai arrivato al task. Se la differenza fra
difficoltà 1 e difficoltà 4 non si vede nella misura, **la misura non sta
guardando la capacità di coding**: sta guardando un turno che non è mai partito.
Ed è la ragione per cui la Parte 0.2 dice quello che dice.

E i tre difetti, in ordine di gravità crescente:

**F1 — il turno è stato rubato dal generatore di storie.** Profilo `agi.p0`,
37952 fatti, 2720 regole, strumenti accesi:

```text
>>> The C11 project in code/ does not build: the Makefile and main.c both
    expect a backend file named exactly strjoin.c…

1) Makefile was a mysterious Makefile. Then one day, makefile discovered what
   it meant to be seen. Makefile had never felt this way before…
```

⚠ E rispetto a `gen500-stream-v3` è **peggiorato**: lì il protagonista era `it`,
un segnaposto; qui il generatore ha estratto un token **vero** della codebase e
lo ha personificato. Il turno rubato somiglia di più a comprensione ed è di
meno.

**F2 — un'istruzione è stata appresa come un fatto.** Nella stessa risposta:

```text
2) Learned: do not merely describe patch.
```

È l'ultima frase del **testo del compito** — un vincolo dato all'esecutore —
asserita come conoscenza. Un imperativo non è una proposizione sul mondo. F1
produce una risposta sbagliata; **F2 modifica la KB**. Nel banco il danno è
contenuto (`PARROT0_LEARN_KB` in `unset_env`), fuori da lì quella rete non c'è.

**F3 — nessuna scrittura, e un turno solo.** B3 (non sa scrivere file) e
l'assenza di ripresa (D49) chiudono qualsiasi possibilità di finire.

## 0.2 ⛔ La diagnosi corregge l'ipotesi «manca la massa critica»

L'ipotesi naturale — *le potenzialità ci sono, manca la massa di conoscenza che
renda la KB viva e fertile* — è **falsificata dai numeri di questa corsa**, e
vale la pena dirlo perché cambia che cosa si fa per primo.

La massa non è piccola: **37952 fatti, 2720 regole**, e il profilo `agi.p0` è
quello grosso. Il problema non è quanta conoscenza c'è: è che **il turno non
arriva mai dove quella conoscenza vive**. Una facoltà narrativa ha rivendicato il
turno prima che qualunque cosa sul codice avesse voce.

> **Aggiungere massa a una facoltà che non riceve il turno non aggiunge niente.**
> È la forma più costosa di lavoro inutile: cresce il numero di fatti, cresce il
> costo di ogni inferenza, e la misura non si muove di un punto.

Non è un argomento contro la crescita della KB — è un argomento sull'**ordine**.
Prima si apre il canale, poi lo si riempie; e una volta aperto, la stessa massa
che oggi non serve a niente diventa disponibile in blocco. È esattamente la
Leva 1 di H11 («ponti prima di nuove isole»), applicata al ponte più a monte di
tutti: quello fra *un turno che porta un compito* e *le facoltà che sanno
guardare il codice*.

**La misura che manca, ed è corta:** su un turno di coding, quante delle 2720
regole vengono anche solo interrogate? Se la risposta è «una manciata, e
nessuna della Code IR», il collo di bottiglia è dimostrato e la priorità è
questa. È il primo cricchetto da scrivere, prima di qualsiasi lezione.

## 0.3 I due bloccanti — nessuna lezione regge se restano aperti

Vanno chiusi **prima** della scala, perché altrimenti addestrare peggiora:
ogni lezione entra in una KB che si mangia gli imperativi e in un dispatch che
non consegna il turno.

| | | |
|---|---|---|
| **P0** | Un turno che porta un **compito su una codebase** non è disponibile alla narrazione. | `faculty_yield` — conoscenza, zero C. ⚠ La classe **non** può essere «il turno non nomina entità della codebase»: F1 le nomina. La classe è la **richiesta**, non il materiale. |
| **P0b** | La **modalità imperativa** è una classe riconosciuta *prima* dell'estrazione dei fatti. | Un ordine dato all'esecutore non è una proposizione da acquisire. Voce già aperta in `LEARN_TODO.md`. |

Gate di P0: lo stesso prompt di match0 non produce narrazione, e **il rifiuto è
osservabile** — non «non capisco», ma un turno che dice che cosa ha
riconosciuto. Gate di P0b: dopo il turno, `do not merely describe patch` **non**
è nella KB, e un'ablazione della classe imperativa lo rifà entrare.

## 0.4 La scala — start small, grow fast

Il principio: **ogni gradino è una generalizzazione della stessa relazione**, non
una facoltà nuova. Così la crescita si vede nella misura invece che nel numero
di file `.p0`, ed è la Leva 2 di H11 (schemi ad alto fan-out) presa alla lettera.

Il caso piccolo **ma vero** è già sul banco: `match0/seed` non compila perché il
`Makefile` nomina `strjoin.c` e quel file non esiste. Sono **due osservazioni e
un join** — un'inferenza vera, non un frasario.

### T1 — «che cosa è rotto»: il riferimento che non atterra

```prolog
% una sola relazione, e nessuna parola di dominio
missing_referenced_file($Snapshot, $Missing, $Referrer) :-
    file_reference($Referrer, $Missing),
    naf(snapshot_file($Snapshot, $Missing)).
```

Risposta attesa: *«il Makefile nomina `strjoin.c`, che non esiste nello
snapshot.»* Piccolo, vero, e già oggi alla portata degli strumenti read-only
(gen495 legge i file).

⭐ **Il gate che rende T1 un gradino e non un caso:** deve valere anche quando il
riferimento viene da un `#include "x.h"` invece che da un `Makefile`. Se serve
toccare il codice per il secondo, T1 era un parser di Makefile travestito.
`file_reference/2` è il punto di fan-out: ogni nuova forma di riferimento è un
produttore in più, mai una regola in più.

### T2 — «che cosa va prodotto»: il riferimento mancante diventa un obbligo

```prolog
must_produce($Task, $File) :- missing_referenced_file($Snap, $File, $Ref), task_scope($Task, $Snap).
```

È il primo subject di una Task IR che **non esiste ancora** — il caso che §4 di
`universal-code-comprehension.md` non ha mai istanziato — ed è anche la prima
`undertaking` di D49 (`continue-as-resumption.md`). Tre piani si toccano qui, e
si toccano su un file da venti righe.

### T3 — «che cosa deve soddisfare»: il contratto del file mancante

```prolog
contract_for($File, $Header) :- file_reference($File, $Header), header_declares($Header, $Fn).
```

Da *«manca `strjoin.c`»* a *«manca `strjoin.c`, che deve definire `str_join`
come dichiarato in `strjoin.h`»*. Qui la risposta smette di essere una
constatazione e diventa una **specifica derivata**, ed è il primo punto in cui
il thinking ha qualcosa di ben posto su cui pensare (D50).

### T4 — produrlo: B3, e KB-first

Scrivere un file è uno strumento dichiarato (`local_tool/3` + `tool_argv/2`,
gen494), non un ramo nel C, e passa dal gate del workspace. **T4 è l'unico
gradino che costa capacità nuova**, e arriva quarto di proposito: prima si
dimostra che parrot0 sa dire *che cosa* va scritto e *perché*.

### T5 — verificarlo: il verdetto rosso è una fonte esterna

`make` è l'oracolo. Chiude il ciclo `osserva → decidi → scrivi → verifica` di B3
e soddisfa il contratto anti-specchio di C3: la fonte della critica è esterna
alla lettura criticata.

## 0.5 ⭐ La prova che il ragionamento SCALA — e costa una corsa

Questo è il gate che F. chiede, ed è **falsificabile**:

> Le regole T1-T3 vengono addestrate **soltanto su match0**. Poi si esegue
> **match1**, che nessuno ha usato per insegnare, e che ha la stessa forma —
> `quicksort.h` dichiara `quicksort_records`, il `Makefile` lo lega, il file
> manca. **T1-T3 devono accendersi su match1 senza una riga in più.**

Se si accendono, il ragionamento è **costruttivo e scala**, e lo dice un numero
(match1 passa da 0 a un punteggio parziale nominato) invece di un'impressione.
Se non si accendono, quello che abbiamo insegnato era match0, non una relazione
— e va buttato, non allargato.

⛔ **Anti-impostore, non negoziabile:** match1 non si guarda mentre si insegna.
Il curriculum si costruisce su match0 e su un fixture sintetico; match1 è
**cieco** fino alla misura. È la Leva 3 di H11 usata come si deve — i
fallimenti differenziali ordinano il curriculum, non lo scrivono.

## 0.6 Ordine, e come si innesta su H12

H12 resta la sequenza giusta **una volta aperto il canale**. Questa parte si
infila prima dei suoi punti 4-5 e li rende eseguibili:

```text
P0   il turno non viene rubato            ← senza questo, tutto il resto è invisibile
P0b  l'imperativo non viene appreso       ← senza questo, addestrare sporca
T1   il riferimento che non atterra       ← il caso piccolo ma vero
     ⤷ gate di fan-out: Makefile E #include, senza toccare il C
T2   l'obbligo di produrre                ← Task IR con subject inesistente + D49
T3   il contratto derivato                ← la specifica, non la constatazione
     ⤷ ⭐ MISURA: match1 a freddo. Qui si vede se scala.
T4   scrivere (B3)                        ← l'unica capacità nuova
T5   verificare con make                  ← chiude il ciclo, fonte esterna
     ⤷ poi H12 punti 4-12, che ora hanno un canale in cui passare
```

**Regola di chiusura, la stessa di sempre:** un gradino si chiude quando un
cricchetto lo tiene fermo e un'ablazione lo fa cadere. E qui se ne aggiunge una
propria di questa parte: **un gradino che non si accende su match1 a freddo non
è un gradino** — è un caso imparato a memoria, e va tolto invece che ampliato.

---

## ⛔ Le due parole, da non confondere mai più

| | |
|---|---|
| **reasoning** | l'inferenza che parrot0 fa già: un turno entra, il solver risolve, esce una risposta. **Resta com'è.** |
| **thinking** | la rielaborazione dell'output con meta-prompt **fino alla chiusura della pipeline**: il risultato rientra, agganciato a un meta-prompt, e il giro si ripete finché una condizione dichiarata non lo chiude |

Non è pedanteria: hanno costi, garanzie e condizioni d'arresto diversi. Un
rientro costa un **turno intero**, non un passo di inferenza.

---

## Parte A — THINKING

### A0. ✅ Il criterio di NON-PEGGIORAMENTO

Oggi la guardia impedisce che un **muro** diventi la risposta finale
(`wall_marker/1`). Non basta, ed è misurato:

```text
> what is photosynthesis
  · pensiero 1 ─ read: …            ↳ Learned 0 fact(s), skipped 2.
  · pensiero 2 ─ what do you know about it
                                     ↳ What number should I use for «it»?
```

Quella non è un muro dichiarato, quindi **è diventata la risposta finale**:
pensare di più ha peggiorato il turno. È il difetto peggiore possibile per uno
strato di deliberazione, perché è invisibile a chi guarda solo il risultato.

Ora `thinking_outcome_evidence/2` classifica l'esito con lo scorer universale e
`thinking_outcome_policy/2` decide se può propagare. Una clarification resta
visibile ma non sostituisce la risposta; un passo `fact_delta` propaga soltanto
se il predicato dichiarato ha realmente acquisito fatti. Sul caso sopra la
risposta iniziale sopravvive. Ablando a runtime la policy `hold`, la domanda
errata torna a propagare: il cricchetto prova quindi la causalità della guardia,
non soltanto una risposta golden.

**⭐ gen502 — la guardia è chiusa, la CAUSA no.** A0 impedisce che il passo
peggiorativo propaghi; non dice perché esista. Ipotesi **D50**
(`frontier-kb-natural-dialogue.md` §18.48): il pensiero degrada perché pensa su
una *stringa* e su una lista piatta di letture, e con quell'oggetto «pensare
meglio» può solo voler dire riformulare la frase. Il test che la separa:
riaccendere il thinking **solo** sul turno di ripresa di D49
(`docs/plans/continue-as-resumption.md`), dove l'oggetto del pensiero è
un'impresa con piano derivato e insieme `reached`. Se lì la guardia non deve
mai intervenire, la causa era il referente.

### A1. ✅ Scelta fra più schemi applicabili; resta la composizione

`brain_think` prende gli schemi applicabili da `thinking_for/2`, li ordina con
`thinking_scheme_priority/2` e pubblica `thinking_selected_scheme/1`. Cambiare
la priorità a runtime cambia lo schema scelto senza rebuild (E7). Resta aperta
la composizione di più schemi nello stesso turno: oggi se ne sceglie uno.

### A2. Gli esperimenti del piano ancora non eseguiti

`docs/plans/thinking.md` §3 ne elenca otto con misura e falsificazione. Stato:

| | |
|---|---|
| E1 lo schema è conoscenza | ✅ `thinking_graph.p0t` |
| E2 il grafo non è una catena | ✅ (invertire un arco inverte il rango) |
| E3 l'arresto ha una ragione dichiarata | ✅ l'esecutore legge l'effetto, interroga `thinking_stop/2` e pubblica `thinking_stop_reached/2` |
| E4 la critica ritira davvero qualcosa | ⛔ **non provato**, ed è quello che conta di più |
| E5 quanto costa | ⛔ **non misurato**: un rientro è un turno intero, il costo va profilato con `/debug` |
| E6 l'operatore è una variabile | 🟡 `reenter(Prompt)` varia dalla KB; restano operatori non-rientro |
| E7 meta-thinking: scegliere lo schema | ✅ priorità KB, scelta osservabile e ablation runtime |
| E8 anti-impostore (togliere un nodo alla volta) | 🟡 A0/policy e feedback hanno ablation; manca l'ablazione completa di `second_thought` |

**E4 ed E8 sono i due che decidono se questo strato serve.** Se la critica non
ritira mai niente ed E8 è maggiore di zero, il thinking è teatro cognitivo e va
tolto, non migliorato.

### A3. Il costo, prima di accendere il thinking di default

Un rientro = un turno = ~250 ms a regime (gen491). Due rientri triplicano il
turno. Prima di alzare `thinking_max_steps/1` sopra 2 servono: la misura di E5, e
le viste materializzate sui predicati che il grafo rilegge (§L). **La regola di
F. vale qui più che altrove: un timeout di dieci secondi è già un sintomo.**

### A4. ✅ Il meta-prompt improduttivo diventa un fatto diagnostico

`thinking_prompt_issue/3` registra per nodo `wall`, `gap`, `no_delta`,
`clarification` o `unclassified`. Il passo resta visibile ma non propaga; la
diagnosi è interrogabile e può guidare la sostituzione del prompt nella KB.

### A5. 🟡 La UI, oltre al grigio

Fatto: meta-prompt + output, grigio, su `stderr`, mentre accade; `--test` ora
esegue lo stesso `brain_think` quando la policy è attiva e stop/issue sono fatti
interrogabili. Manca ancora `/thinking` come trace completo e la resa MCP.

---

## Parte B — CODING AGENT

### B0. 🟡 Severità e ordinamento chiusi; tradeoff aperto

`quality_severity/3`, `finding_priority/3` e `finding_precedes/3` sono policy KB
con riordinamento e ablation runtime. Task IR e obbligo di evidenza sono vivi.
Restano tradeoff e l'azione informativa che separa ipotesi concorrenti.

### B1. La latenza è un provider di evidenza, non una corsia privilegiata

`speed of X` senza profilo dichiara che manca ed è giusto. Ma questo è un caso
di repertorio utile a falsificare l'onestà della IR, non la roadmap. Oggi
`perf_evidence/3` ha **zero fatti** e nessun profiler ci si collega. Finché è
così, parrot0 può solo nominare *candidati costosi*. **«Questa parte è lenta»
senza evidenza dinamica deve restare impossibile**: è la regola più forte del
piano e non va indebolita per far sembrare l'agente più capace.

### B2. La rilettura come guadagno, non come invalidazione

Oggi rileggere una sorgente **invalida** lo snapshot vecchio. Il §7 leva 7 chiede
l'altra metà: una lezione deve far **rileggere meglio** ciò che era già stato
letto. È lo stesso difetto che `LEARN_TODO` §0 segnala per la prosa dal gen-433.

### B3. Il piano su strumenti è uno solo

`analyze_sources` con due passi. Mancano: un piano che si **componga** da un
obiettivo invece che da una superficie dichiarata, la scelta fra piani
alternativi, e il ciclo `osservazione → verifica → riparazione` (T03 di
`coding-agent-todo.md`). La macchina (`action_schema/2`) esiste e non è
collegata.

**⭐ gen502 — e manca il pezzo prima di tutti: la ripresa.** Un piano che si
compone da un obiettivo non serve a niente se il turno dopo non sa di averlo
aperto. Oggi non lo sa: parrot0 non reifica ciò che ha *intrapreso*, solo ciò
che l'utente ha detto (`exchange/3`). D49
(`docs/plans/continue-as-resumption.md`) aggiunge `undertaking`/`resumable` come
vista sul chainer di gen258/259 — **zero C**, perché `turn_bookkeeping/2` è già
un aggancio esteso da regole KB. È anche il motivo per cui il banco di gara
finisce 0-0: senza ripresa un REPL ha un turno solo e basta.

### B4. Il residuo KB-first del layer strumenti

Confronti di parole letterali in `60-agent-tools.c`: **18 → 7**. Restano
l'ordine dei rami, la politica di scelta fra template `tool_argv/2`, e sette
confronti. È O1 di `coding-agent-todo.md`, quasi chiusa.

---

## Parte C — DOVE I DUE PIANI SI TOCCANO

Sono le voci che valgono doppio, e per questo la coda è una sola.

### C1. ⭐ Il thinking sui finding di qualità (E4, in concreto)

Lo schema `finding → critica → revisione` applicato a `code_finding/3` è
**l'esperimento più informativo di entrambi i piani**: i finding hanno già
evidenza (`criterion_finding`), controevidenza (`criterion_waiver`) e
attribuzione (`ir_domain_claim_basis`), quindi la critica ha di che lavorare — e
la misura è netta: *findings ritirati / findings prodotti*.

Se `wide_fanout` su un dispatcher non viene ritirato da nessuna critica, il
thinking non serve a questo dominio e va detto.

### C2. Il rientro come passo di piano

`plan_step` e `thinking_step` condividono già la firma (R1). Il passo mancante:
un piano su strumenti che includa un **rientro** — «leggi i sorgenti, poi pensa
a quello che hai letto» — che è esattamente ciò che un coding agent fa.

**⭐ gen502 — il rientro presuppone la ripresa, ed è la stessa struttura.**
«Leggi i sorgenti, poi pensa a quello che hai letto» è un piano interrotto e
riavviato dallo stato: esattamente `next_action/2` di D49
(`docs/plans/continue-as-resumption.md`). C2 e il `continue` non sono due
lavori — sono lo stesso, visto una volta da dentro il piano e una volta dal
turno dell'utente. Farne uno solo, e cominciare dal `continue` perché è
osservabile da fuori.

### C3. ✅ Il loop di specchio è una guardia eseguibile

`reentry_brings/3` non basta più da solo: `thinking_feedback/4` deve dichiarare
`fact_delta`, `query_result` oppure `gap_with_action`; l'ultimo richiede una
`thinking_feedback_action/3`. Ablando delta o azione il nodo diventa
inammissibile. Sul codice la fonte deve restare esterna alla lettura criticata:
un'altra vista IR, un oracolo, un test, una misura o una conoscenza di dominio.

### C4. Il costo si somma

Un piano su strumenti (N letture) più un thinking (M rientri) costa N+M turni.
Le due code condividono lo stesso §L, e devono condividere lo stesso budget: non
due tetti indipendenti che si moltiplicano.

---

## Come si usa questa coda

- ⛔ **Prima di tutto la Parte 0** (gen502): finché il turno di coding viene
  rubato e un imperativo viene appreso come fatto, ogni voce qui sotto si misura
  su un canale chiuso. Le voci sotto **non cambiano** — cambia quando si aprono.
- **Prima C1**, ora che A0 è chiusa: critica reale dei finding e ritiro misurato.
- **Poi E8 completo** sullo schema attivo, nodo per nodo.
- **Poi E5/A3**, perché senza la misura del costo ogni altra decisione è a occhio.
- Le voci di B avanzano in parallelo: non dipendono dal thinking.

Regola di chiusura, la stessa di sempre: **una voce si chiude quando un
cricchetto la tiene ferma e un'ablazione la fa cadere.** Un TODO chiuso senza
falsificazione è un TODO spostato.

---

# Handoff operativo — dalla IR viva al coding agent competitivo

> Questo handoff è la continuazione eseguibile di
> `docs/plans/universal-code-comprehension.md`, non un piano alternativo. È
> aggiornato dopo il checkpoint gen499 e dopo la costruzione del laboratorio
> `tests/challenge/`. Chi riprende deve partire da qui, verificare lo stato reale
> del repository e portare avanti **un incremento causale per volta**.

## H0. Verità di partenza e decisioni ormai non negoziabili

La direzione generale è corretta e non va riscritta a ogni generazione:

1. il codice è un input da osservare nella KB, non testo su cui applicare un
   repertorio di pattern;
2. una domanda apre una Task IR e obblighi di evidenza, non sceglie un handler;
3. i giudizi qualitativi derivano da criteri, prove, controprove e policy;
4. i ponti a predicato variabile permettono alla conoscenza acquisita in una
   rappresentazione di finanziare ragionamenti in un'altra;
5. il thinking è produttivo solo quando cambia rappresentazione, evidenza,
   prospettiva od obiettivo;
6. una codebase reale è multi-file. Nessun limite single-file può essere
   incorporato nel planner, nell'editore, nella gara o nella definizione di
   successo;
7. parrot0 è esso stesso il coding agent nel proprio terminale. Il server
   OpenAI-compatible e l'integrazione `pi` sono adapter secondari e attualmente
   lossy; non sono il runtime canonico e non devono misurare la sua capacità.

L'ultimo punto corregge un errore storico importante. Montare parrot0 dentro
`pi` non lo rende più agente: oggi appiattisce il contesto e non espone il ciclo
di tool call. La gara e il dogfood devono quindi lanciare direttamente
`bin/parrot0` con cwd nella codebase, `PARROT0_TOOLS=1` e, quando l'esperimento
lo richiede, `PARROT0_THINKING=1`. L'adapter server può restare vivo per
compatibilità, ma si depreca progressivamente ogni assunzione che lo presenti
come via primaria.

### Ciò che è già patrimonio e non va ricostruito

- snapshot, source unit, nodi, nomi, span, edge e provenance della Code IR;
- identità source-qualified per definizioni e chiamate;
- `ir_denotation`, `representation_bridge` e attraversamento con `apply/2`;
- parti di identificatore, scope, containment, reference e ordine osservato;
- criterion/evidence/counterevidence, severità e precedenza dei finding;
- Task IR iniziale per review qualitative;
- grafo di thinking, scelta fra schemi, stop dichiarato, feedback contract,
  diagnostica dei prompt e cricchetto anti-peggioramento;
- `PatchArtifact`, `P0Obs`, kernel agentico e repair come organi secondari già
  disponibili, anche quando non sono ancora collegati al ciclo universale.

La regola di lavoro è **collegare prima di ampliare**. Aggiungere il ventesimo
scanner o il centesimo prompt prima di collegare questi organi produce volume,
non comprensione.

## H1. Definizione operativa della north star

La north star non è «parrot0 dà sempre una risposta». È:

```text
task naturale su una codebase fredda
  -> snapshot revisionato dell'intero repository
  -> Task IR con subject, scope, vincoli e deliverable
  -> ipotesi concorrenti
  -> piano minimo di evidenza
  -> osservazioni da sorgente/documenti/build/test/runtime/storia
  -> ponti verso conoscenza strutturale e di dominio
  -> claim qualificati con basis e controevidenza
  -> eventuale modifica multi-file candidata
  -> oracle, critica, repair o replan
  -> risposta e artefatto verificati
```

“Universale” descrive l'apertura di ogni freccia: domani un nuovo linguaggio,
criterio, strumento, API, forma interrogativa o dominio deve poter entrare come
conoscenza/mapping/observation provider, senza un nuovo caso lessicale in C.

### Metriche che contano

Non usare il solo wall-rate e non usare una sola accuracy aggregata. Registrare:

| metrica | che cosa rivela |
|---|---|
| task freddi risolti | utilità end-to-end su codebase non nominate |
| famiglie di domanda per relazione IR | riuso, contro pattern recognition |
| claim con basis completa | grounding epistemico |
| gap con azione informativa | qualità del fallimento |
| bridge fan-out e composizioni valide | moltiplicatore di conoscenza |
| finding ritirati dalla critica | valore reale del thinking |
| retry che cambiano stato | fertilità del loop |
| edit multi-file verificati | agency concreta |
| regressioni da ablation | causalità della conoscenza |
| costo ingest/query/reentry/tool | sostenibilità del processo |

Un punteggio gara è un segnale esterno. Diventa apprendimento soltanto quando un
check fallito viene attribuito a una transizione mancante, riprodotto fuori dal
match e chiuso con una facoltà generale.

## H2. Laboratorio competitivo: come usarlo senza allenarsi sul test

`tests/challenge/` separa tre oggetti:

```text
tasks/matchN/                         specifica immutabile
  match.json                          difficoltà, ordine, artefatto-ancora
  task.md                             incarico identico
  seed/                               snapshot iniziale multi-file
  judge.py + probe                    oracolo non mostrato nel prompt

<league>/matchN/runs/<run-id>/        osservazione storica
  prompt.md
  parrot0/{raw.log,transcript.txt,code/}
  freebuff/{raw.log,transcript.txt,code/}
  result.json
  analysis.md

<league>/scoreboard.md                ultimo risultato + storico
```

Durante il run, entrambi lavorano in successione nello **stesso pathname
neutrale** terminante in `code/`. Il controller ricostruisce il seed prima del
secondo agente e verifica il digest. Nome dell'agente, output dell'avversario e
judge non entrano nel prompt o nel cwd. L'intero albero finale viene archiviato:
`artifact` nel manifest è solo un'ancora, non un limite a un file.

I primi due match hanno questo scopo:

1. **match1, codebase C:** capire una pipeline esistente, introdurre un backend
   generico worst-case-safe, integrarlo in API/Makefile/CLI, rispettare
   contratto, genericità, overflow e input avversi;
2. **match2, codebase Python:** seguire un incidente attraverso codec, locking,
   API, filesystem e CLI; chiudere durability, recovery, corruption,
   compaction atomica e writer concorrenti.

Non sono tutorial a soluzione nota dal prompt. Sono codebase seed che già
funzionano sul cammino facile e falliscono su proprietà che richiedono analisi
da sviluppatore.

### Protocollo dopo ogni gara

Non modificare subito parrot0 guardando il nome del check. Eseguire in ordine:

1. confrontare i due snapshot e i transcript, distinguendo mancata comprensione,
   mancata azione, azione errata e verifica insufficiente;
2. classificare ogni divergenza nella tassonomia seguente;
3. scegliere **il primo confine causale** in cui parrot0 perde informazione;
4. costruire un caso reale diverso dal match che richiede la stessa facoltà;
5. provare prima una lezione naturale secondo `LEARN_PROTOCOL.md`;
6. se la lezione non è rappresentabile, ampliare il motore astratto minimo;
7. chiudere replay, transfer, contrasto, composizione e ablation;
8. rieseguire il caso diverso; soltanto dopo, in un nuovo run-id, il match.

Tassonomia minima del gap:

| classe | evidenza nel transcript/artefatto | leva probabile |
|---|---|---|
| `task_binding` | perde vincolo, scope o deliverable | Task IR e role binding |
| `repository_sensing` | non trova file/build graph/simbolo | source index e provider |
| `cross_file_identity` | confonde definizione, declaration, call | symbol/scope/reference IR |
| `semantic_model` | legge nodi ma non comportamento | CFG, def-use, effect summary |
| `domain_bridge` | possiede il fatto ma non lo applica al codice | denotation + bridge + basis |
| `planning` | sa cosa manca ma non ordina azioni | action schema e precondizioni |
| `tool_execution` | piano corretto senza osservazione reale | adapter/tool contract |
| `artifact_construction` | propone testo, non patch coerente | PatchArtifact multi-file |
| `oracle_use` | dichiara successo senza build/test | evidence obligation |
| `repair` | si ferma al primo rosso | Verdict -> critique -> replan |
| `epistemic_control` | confonde ipotesi e prova | claim state/policy |
| `thinking_stall` | secondo giro ripete il primo | feedback delta e schema selection |

La differenza “FreeBuff passa, parrot0 fallisce” rende il gap discriminante; non
dimostra da sola la causa. “Falliscono entrambi” non è automaticamente un buon
target: può segnalare un contratto ambiguo, un judge errato o una difficoltà
inutile. `analysis.md` deve conservare questa distinzione.

### Come far crescere le gare

Ogni nuovo match deve aggiungere una dimensione, non soltanto righe:

1. codebase piccola multi-file, una modifica con build e casi avversi;
2. requisito distribuito fra codice, README e test;
3. simboli omonimi, indirection e più linguaggi nello stesso repository;
4. bug che richiede osservazione runtime, non deduzione statica;
5. alternative architetturali con tradeoff e policy locali;
6. patch multi-file con migrazione/backward compatibility;
7. fallimento iniziale dell'oracolo e repair obbligatorio;
8. issue incompleta in cui l'azione giusta è acquisire evidenza o chiedere una
   scelta, non editare;
9. codebase abbastanza grande da tirare indici, invalidazione e budget;
10. dogfood su parrot0 senza fatti privilegiati.

Alternare l'ordine degli agenti. Non cambiare prompt fra loro. Non dare hint sui
file per compensare parrot0: un gap di localizzazione deve restare misurabile.

## H3. Obiettivo intermedio 1 — Repository IR completo e revisionato

### Risultato richiesto

Dato il root di una codebase, parrot0 deve indicizzare almeno artifact, unit,
linguaggio, build target, definizione, dichiarazione, reference, call e test,
con snapshot e provenance. Due simboli omonimi non collidono e un edit invalida
solo la closure dipendente.

### Strategia

1. fare di directory walk, parser C/Python, manifest reader e build observer
   semplici provider dello stesso contratto;
2. pubblicare observation facts in overlay revisionato, mai nella KB curata;
3. derivare le viste legacy, mantenendole secondarie;
4. introdurre lineage solo quando l'identità fra snapshot è dimostrabile;
5. registrare unresolved/ambiguous invece di scegliere per nome;
6. collegare README, header, test e diagnostica allo stesso symbol id.

### Primo vertical slice

Usare una codebase reale piccola con due `init`, un header, due caller e un test.
Porre: dove è definito ciascun `init`, chi lo usa, quale test lo esercita, quale
snapshot sostiene la risposta. Non aggiungere quattro handler: le quattro
risposte devono derivare dagli stessi archi.

### Gate causale

- modifica una call e rileggi: l'arco vecchio sparisce;
- aggiungi un mapping frontend a runtime: nasce una relazione astratta;
- abla il mapping: scompare la vista, non il nodo osservato;
- nessun source-derived fact finisce in `/save`;
- una query con simboli ambigui restituisce alternative con proof.

## H4. Obiettivo intermedio 2 — Semantica del programma come reticolo di viste

### Risultato richiesto

Parrot0 deve poter spiegare controllo, dati, effetti e dipendenze attraverso più
file. Non serve subito una semantica completa del linguaggio; serve un contratto
estensibile in cui ogni nuovo costrutto aggiunge osservazioni componibili.

### Strategie ordinate

1. basic block e control edge, mantenendo espliciti branch/merge/exit;
2. definition/use/read/write con scope risolto;
3. call-site -> callee candidate -> resolved/unresolved/ambiguous;
4. effect facts (`pure`, I/O, allocation, lock, process, unknown) con basis;
5. summary interprocedurali condizionali, mai assoluti davanti a unknown call;
6. propagazione di vincoli e taint come consumer della stessa def-use;
7. mapping test/diagnostica/profile da span a node e symbol.

Il primo target non deve essere “capire quicksort”. Deve essere una domanda
generale come «quale scrittura può raggiungere questo ritorno?» che vale su
quicksort, journal, parser e qualunque codebase futura.

### Gate

Una singola modifica all'IR deve sbloccare almeno due famiglie non equivalenti,
per esempio spiegazione di un valore e blast radius. Ablando una regola
semantica deve cadere soltanto la parte della spiegazione che la usa. Un call
esterno sconosciuto rende il summary `supported/unknown`, non inventa purezza.

## H5. Obiettivo intermedio 3 — Dalla lingua naturale a Task IR universale

### Risultato richiesto

Domande disparate — descrivere, localizzare, confrontare, valutare, predire,
diagnosticare, modificare — devono produrre combinazioni di ruoli, non `qtype`
sempre nuovi.

### Strategia

Espandere gradualmente:

```text
operation + subject + scope + dimension + constraints + deliverable
          + evidence_requirement + budget + non_goal
```

Le superfici vivono in `intent_phrase`/`intent_cue` o relazioni più specifiche;
gli identificatori sconosciuti si legano dall'indice della codebase. Coordinare
più richieste in un `answer_plan`; non rispondere al primo sotto-goal.

### Training via prompt

Per ogni nuova forma seguire `LEARN_PROTOCOL.md`: baseline naturale, lezione in
lingua ordinaria, replay, Transfer@3, due parafrasi, quasi-esempio, composizione,
retract/reteach, save e fresh process. Se il docente deve dire
`task_operation_cue/2`, il metalinguaggio non è pronto.

### Gate

Insegnare una nuova forma di domanda e usarla su un simbolo mai menzionato.
Ablarla deve perdere solo la superficie, non operation, IR o facts. Una domanda
ambigua chiede o presenta alternative; non seleziona silenziosamente.

## H6. Obiettivo intermedio 4 — Claim qualitativi aperti e confutabili

### Risultato richiesto

Rispondere a «va migliorato?», «è sicuro?», «è corretto?», «è chiaro?», «che
impatto avrebbe?» senza un catalogo chiuso di smell e senza privilegiare la
performance.

### Forma generale

```text
criterion
  -> applicability(subject, policy, context)
  -> evidence obligations
  -> evidence + counterevidence
  -> alternatives + tradeoff
  -> qualified finding
  -> priority relative to project goals
```

Il nome della misura resta una variabile eseguita con `apply/2`. Le soglie e la
priorità sono policy scoped. Build, test, compiler, sanitizer, coverage,
profiler, documenti e storia sono provider equivalenti del contratto di
evidenza: nessuno diventa una corsia privilegiata.

### Prossimo incremento preciso: C1/E4

Applicare davvero `finding -> critique -> revision` a un finding prodotto dalla
Code IR. La critica deve cercare una controevidenza indipendente e poter
ritirare o ridurre il claim. Misurare:

- finding iniziali;
- finding confermati;
- finding declassati;
- finding ritirati;
- basis nuova che ha causato ogni transizione.

Se nessun finding cambia mai, lo schema non aggiunge conoscenza e va rimosso o
ridisegnato. Non renderlo verde con una critica che ripete il finding.

## H7. Obiettivo intermedio 5 — Active evidence e piano di strumenti

### Risultato richiesto

Quando le prove non decidono, parrot0 sceglie l'azione minima che separa le
ipotesi: leggere un file, risolvere un binding, costruire, eseguire un test,
profilare un workload, ispezionare storia o chiedere una policy.

### Strategia

Collegare Task/Goal/Constraint già esistenti a `action_schema`:

```text
hypotheses + missing_evidence
  -> candidate actions
  -> expected discrimination / cost / risk
  -> selected action
  -> P0Obs
  -> claim update
```

La scelta è KB policy. Il C esegue meccaniche autorizzate e produce observation;
non decide che `pytest` prova correttezza o che un profiler è necessario perché
ha letto la parola “lento”.

### Gate

- due ipotesi reali e due azioni possibili;
- cambiare costo/priorità a runtime cambia l'azione;
- l'osservazione rientra come fact interrogabile;
- la stessa Task IR riparte dal gap, senza ricominciare dall'input grezzo;
- un'azione senza potere discriminante non viene ripetuta;
- budget comune per read/tool/reentry, non tetti moltiplicativi.

## H8. Obiettivo intermedio 6 — Thinking proliferante, non loop infinito

### Risultato richiesto

Ogni iterazione deve poter scoprire qualcosa di nuovo perché cambia il materiale
su cui inferisce. “Più thinking” senza più rappresentazioni o evidenza raggiunge
inevitabilmente il muro.

### Schema minimo per il codice

```text
candidate interpretation
  -> structural counterview
  -> domain/policy bridge view
  -> evidence gap
  -> action or KB query
  -> revised claim set
  -> oracle/critique
  -> stop(reason)
```

Gli operatori non devono essere soltanto `reenter(Prompt)`. Introdurre operatori
KB descrivibili per query, tool, confronto di proof, retract/declassamento e
sintesi. La composizione di più schemi sostituisce gradualmente la scelta di un
solo schema quando i loro output sono compatibili e il budget lo consente.

### Anti-stallo

Un nodo è ammissibile soltanto se dichiara uno fra:

- nuova observation;
- nuova query su una vista diversa;
- controevidenza;
- nuova ipotesi con azione discriminante;
- modifica di stato del claim;
- chiusura motivata.

Il delta deve essere osservato dopo il passo, non soltanto promesso dalla KB.
Due passi consecutivi con la stessa basis e lo stesso claim set fermano lo
schema con `no_progress`. Il risultato precedente sopravvive se il nuovo esito
è una clarification, un muro o un output meno grounded.

### Gate E8

Ablare ogni nodo dello schema su task reali. Il nodo vale se almeno un esito
grounded peggiora in sua assenza; un nodo la cui ablazione non cambia niente è
teatro cognitivo. Misurare anche E5: tempo, query, facts letti, tool e rientri per
nodo.

## H9. Obiettivo intermedio 7 — Modifica multi-file e repair come un solo ciclo

### Risultato richiesto

Parrot0 deve operare su codebase sfidanti alla pari con l'avversario: creare,
editare, rinominare e cancellare più file in una patch coerente, preservare lo
snapshot iniziale, validare pre/postcondizioni, costruire, testare e riparare.

### Strategia

1. Task IR produce un set di change obligations, non una stringa di patch;
2. localizzazione lega ogni obligation a symbol/span con confidenza;
3. le trasformazioni diventano operazioni di un solo `PatchArtifact`;
4. precondition digest impedisce edit su contenuto stale;
5. l'artefatto viene applicato a un candidate tree completo;
6. build/test producono `P0Obs` collegati a constraint;
7. verdict rosso genera gap e controesempio;
8. critique/replan prepara una nuova versione, senza mutare il workspace buono;
9. commit finale soltanto su policy e oracle verdi.

Il limite storico di alcune primitive di generazione single-file è una
struttura secondaria, non un vincolo. Può restare come fallback per compiti
piccoli, ma il planner universale non deve mai ridurre un task multi-file per
farlo entrare lì.

### Gate

Una codebase fredda richiede almeno tre file modificati, un file nuovo, un
fallimento iniziale della build e un repair. Il judge valuta lo snapshot intero.
Ablando la policy di una singola operazione, l'intero commit viene negato senza
lasciare una patch parziale.

## H10. Obiettivo intermedio 8 — Rilettura dopo apprendimento e dopo edit

### Risultato richiesto

Una nuova lezione semantica deve far rileggere meglio lo stesso snapshot; un
edit deve sostituire soltanto le osservazioni dipendenti. È il passaggio dalla
KB che accumula alla KB che cresce.

### Strategia

- dependency key per ogni derived claim: snapshot, mapping, bridge, criterion,
  policy e observation usati;
- invalidazione selettiva quando una dipendenza cambia;
- overlay nuovo costruito e validato prima dello swap;
- lineage fra simboli solo se non ambiguo;
- cache materializzata eliminabile, mai fonte di verità;
- query replayata dalla Task IR originale.

### Gate

Insegnare naturalmente la semantica di un costrutto/API reale, rileggere una
codebase già indicizzata e ottenere nuovi claim senza cambiare byte. Retract
della lezione rimuove quei claim. Dopo un edit, nessun finding del vecchio hash
può essere presentato come attuale.

## H11. Le leve migliori per accelerare la crescita della KB

### Leva 1 — Ponti prima di nuove isole

Prima di aggiungere fatti, cercare quali zone già esistenti non comunicano. Un
ponte corretto può rendere disponibili centinaia di facts di dominio a tutti i
symbol denotati. Misurare `cold tasks unlocked / new curated clauses`: deve
crescere più di uno-a-uno. Ogni bridge conserva basis ed è ablabile.

### Leva 2 — Schemi ad alto fan-out

Priorità a conoscenza che serve più famiglie: scope/reference finanzia locate,
impact e rename; def-use finanzia explanation, security e correctness; effect
summary finanzia concurrency, performance e safety. Rifiutare incrementi che
servono soltanto il wording del match appena fallito.

### Leva 3 — Fallimenti differenziali come active curriculum

La gara ordina i gap per valore informativo:

```text
parrot0 fallisce + avversario passa + judge chiaro
  > entrambi falliscono
  > entrambi passano
  > differenza soltanto stilistica
```

Fra gap discriminanti scegliere quello che interrompe più catene a valle, non
quello più facile da rendere verde.

### Leva 4 — Imparare via prompt prima di promuovere `.p0`

Il percorso obbligatorio resta:

```text
lezione naturale -> replay -> transfer -> contrasto -> composizione
-> ablation/reteach -> save -> fresh process -> piccolo commit/push
```

Una sessione con zero facts veri è diagnostica. Se il prompt non può insegnare
la classe, non aggiungere a mano membri: promuovere soltanto il metameccanismo
che rende la prossima lezione apprendibile.

### Leva 5 — Conservare negative knowledge e controesempi

Un bridge falso o una regola troppo larga costa più di un gap. Registrare
false-composition, scope, eccezioni e candidate perdenti. La critica ha bisogno
di controevidenza reale; senza, il thinking conferma sempre se stesso.

### Leva 6 — Replay incrementale, non riscan globale

Dependency tracking e materialized views permettono più cicli nello stesso
budget. Profilare facts/query/reentry prima di alzare timeout. Ottimizzare il
predicato dominante preservando l'identico proof, non tagliando ciò che vede.

### Leva 7 — Separare acquisizione, capacità e prova

Tenere contatori distinti:

- facts veri acquisiti;
- forme linguistiche apprese;
- procedure/bridge/criteri nuovi;
- task freddi sbloccati;
- ablation che dimostrano causalità;
- risultati gara.

Una KB grande senza transfer non è crescita; un match vinto senza sapere quale
facoltà lo ha reso possibile non è apprendimento.

## H12. Sequenza esatta consigliata per chi riprende

Non aprire dieci cantieri insieme. Questa è la coda concreta:

1. **C1/E4:** critica reale di un finding, con conferma/declassamento/ritiro e
   basis osservabile;
2. **E8:** ablation nodo per nodo dello schema appena usato;
3. **E5:** misura corta del costo dello stesso schema;
4. **Repository IR:** build graph, test link, definition/reference qualificate
   su codebase multi-file;
5. **Task IR:** un task di modifica e una domanda qualitativa passano dallo
   stesso subject/scope/constraint binding;
6. **Active evidence:** una missing evidence seleziona un tool read-only e
   riprende la Task IR;
7. **CFG + def-use minimo:** solo le relazioni tirate dal caso reale, ma
   generalizzate e riusate da due domande;
8. **PatchArtifact multi-file:** collegare obligation -> candidate tree ->
   build/test P0Obs;
9. **Repair:** un verdict rosso cambia patch o piano, non soltanto testo;
10. **Training naturale:** insegnare un criterio/API/policy reale e far
    rileggere la codebase;
11. **Challenge baseline:** eseguire match1 con run-id esplicito, analizzare e
    aprire il gap ad alto fan-out;
12. **Challenge crescente:** match2 solo dopo aver chiuso o classificato il
    primo limite, senza allenarsi sul probe.

Per ogni punto: un ratchet focal, una ablation causale, al massimo il gate breve
consentito; niente sessioni di test lunghe come sostituto della comprensione.

## H13. Checklist di handoff per ogni checkpoint

Prima di lasciare il lavoro al prossimo agente, scrivere qui o nel commit:

- commit/versione e worktree preesistente preservato;
- capacità generale cercata;
- task reale che l'ha tirata;
- baseline e controesempio;
- rappresentazioni attraversate;
- facts/bridge/criteria/policy appresi o modificati;
- proof e provenance;
- replay/transfer/contrast/composition/ablation;
- cosa è migliorato fuori dal caso trainato;
- costo osservato;
- gap residuo e prima azione informativa;
- eventuale match/run e link a transcript/snapshot;
- ragione per cui nessuna conoscenza linguistica è finita nel C.

## H14. Stop condition architetturali

Fermarsi prima di committare se:

- il fix nomina nel C parole, API, smell, linguaggi o forme del match;
- il nuovo criterio non può essere insegnato o ritratto parlando;
- la codebase viene ridotta a un file per adattarla a una primitiva esistente;
- il judge guarda una frase della risposta invece dell'artefatto/osservazione;
- un claim qualitativo non cita snapshot e basis;
- il thinking propaga un output meno grounded;
- un loop ripete basis/claim set senza `no_progress`;
- una patch tocca il workspace prima di essere un artifact verificabile;
- un run dell'avversario può vedere l'output del primo;
- si ritocca il task dopo aver visto un risultato senza versionare il cambio;
- una vittoria viene descritta come comprensione senza transfer e ablation;
- l'integrazione `pi` viene usata per definire o limitare il coding agent nativo.

## H15. Definizione di arrivo del programma di lavoro

Il piano è traguardato quando parrot0, dal proprio terminale e su più codebase
fredde, riesce ripetutamente a:

1. costruire una IR revisionata dell'intero repository;
2. collegare codice, documenti, build, test, runtime, policy e dominio;
3. capire domande qualitative e task di modifica non presenti nel training;
4. dichiarare ipotesi, prove, controprove, incertezza e prossima azione;
5. attraversare ponti cross-rappresentazione conservando la basis;
6. acquisire evidenza con strumenti e riprendere lo stesso ragionamento;
7. produrre e riparare PatchArtifact multi-file;
8. lasciare decidere a oracle reali prima di dichiarare successo;
9. imparare via prompt nuove semantiche, criteri e policy con Transfer@3;
10. rileggere dopo lezione o edit senza conclusioni stale;
11. usare il thinking per cambiare davvero claim/evidenza, con costo sostenibile;
12. competere in match crescenti senza hint differenziali o adattatori che lo
    mutilano.

Il segnale decisivo non sarà un 100/100 isolato. Sarà vedere che una singola
lezione o un singolo ponte chiude più gap in codebase e domini diversi, che la
sua ablazione li riapre selettivamente e che il match successivo fallisce più
avanti nella catena. Quello è il moltiplicatore che rende la KB viva.
