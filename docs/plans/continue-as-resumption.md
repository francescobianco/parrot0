# `continue` — la mossa che riprende, e perché non è una parola

> **gen502, su richiesta di F.** dopo che il banco di gara ha misurato che
> parrot0 non ha nessuna forma di «vai avanti col lavoro» (CHALLENGE_TODO §S4).
> Questo documento non propone una feature nuova: sostiene che il `continue`
> **è già richiesto da tre punti diversi dei piani esistenti**, con tre nomi
> diversi, e che costruirlo è il modo più economico di farli convergere.

---

## 1. La scoperta: il `continue` è già scritto, tre volte

Nessuno dei tre lo chiama così, e nessuno dei tre è stato costruito.

| dove | come lo chiama | la frase che lo dice |
|---|---|---|
| `frontier-kb-natural-dialogue.md` **K3** | `issue_status(Issue, open)` + `answer_obligation(Issue, Agent)` | *«un modello della conversazione come sequenza di mosse che aprono e chiudono obblighi»* |
| stesso, **K11 / §17.3** | `plan_unresolved(Plan, Question)`, i «residui» | *«l'output del livello è un piano con proof, alternative e residui»* |
| `universal-code-comprehension.md` **§4** | `budget_exhausted` | *«il cammino esiste ma non è stato completato»* |

Quel terzo è la definizione letterale di uno stato riprendibile, ed era già
scritta in un elenco di stati del claim. **Le tre dicono la stessa cosa in tre
vocabolari**: *qualcosa è stato aperto, non è stato chiuso, ed è nominabile.*

`continue` è l'unica **mossa dialogica** che dice «chiudi il più vicino». È
quindi esattamente `appropriate_move/2` di K3 — un'astrazione che il piano
elenca fra le mosse mancanti e che nessuna generazione ha ancora aggiunto.

**Conseguenza sul metodo, e la ragione per cui vale la pena farlo adesso:** non
è un allargamento della roadmap, è il primo **consumatore** di tre livelli che
finora hanno solo prodotto. Un livello senza consumatore non è misurabile.

---

## 2. Che cosa esiste già davvero — misurato, non ricordato

Il pezzo difficile del `continue` non è riconoscerlo: è **avere a che cosa
tornare**. Buona parte c'è.

### ✅ `kb/core/issues.p0` (gen394) — K3 costruito a metà

```prolog
turn_bookkeeping($Turn, issue) :- turn_unanswered($Turn, $Word, $Relation),
                                  assert(open_issue, $Word, $Relation).
issue_status($Word, open)     :- open_issue($Word, $R), naf(issue_answered($Word)).
answer_obligation($Word, parrot0) :- issue_status($Word, open).
```

E porta già scritto il principio che tutto il resto deve rispettare:

> *«Non si chiude perché qualcuno la dichiara chiusa: si chiude quando la
> risposta diventa derivabile. Lo stato è perciò una VISTA sul mondo corrente e
> non un flag da mantenere — non può divergere da ciò che parrot0 sa, perché non
> è una seconda copia di quel sapere.»*

⛔ **Ma `open_issue` non ha consumatori.** Nessuna regola, in tutta la KB, lo
legge per *agire*. C'è un contabile e non c'è chi riprende. È il buco esatto in
cui il `continue` entra.

### ✅ `kb/core/discourse.p0` — il pattern da copiare, non da inventare

La salienza non è un campo tenuto da qualcuno:

```prolog
current_topic($E)  :- entity_last_turn($E,$T), naf(any_later_exchange($T)).
previous_topic($E) :- non_current_last($E,$T), naf(later_non_current($T)).
```

> *«E non perché qualcuno l'abbia esclusa: perché non ha lasciato uno scambio.»*

**Il `continue` deve avere questa forma**, non un puntatore di ripresa.

### ✅ `turn_bookkeeping/2` è un aggancio generico, esteso da regole KB

Verificato: sei file lo estendono (`issues`, `stipulation`, `register`,
`state-description`, `assisted-learning`, `discourse`), ciascuno con il proprio
`bookkeeper(Nome)`. **La contabilità del `continue` non costa una riga di C.**

### ✅ `kb/experts/codebase/actions.p0` (gen258/259) — il piano c'è già

`plan_goal/2`, `action_needs/2`, `action_yields/2`, `action_desc`,
`action_impl/2`, `plan_param/3`, e un chainer generico e muto che ne deriva il
postordine. **Il «passo successivo» non va memorizzato: è già derivabile.**

### ⛔ Che cosa manca, ed è una sola cosa

La conversazione reifica **ciò che l'utente ha detto** (`exchange/3`), mai **ciò
che parrot0 ha intrapreso**. Non esiste un fatto che dica *«al turno N mi è
stato chiesto G, e sono arrivato fino a K»*.

Al suo posto ci sono due campi nel C, ed è il posto sbagliato:

```c
src/brain.c:102   char last_module[32];
src/brain.c:223   char last_input_canon[256];
src/brain.c:224   char last_input_raw[256];
src/brain.c:225   int  has_last_input;
```

⚠ **`last_input_canon`, `last_input_raw` e `has_last_input` non sono letti da
nessuna parte** — dichiarati e mai usati. Vanno **cancellati**, non riusati: un
campo «ultimo input» nel C è precisamente la forma che `discourse.p0` rifiuta.

---

## 3. Il design — quattro strati, tutti fatti

### 3.1 Reificare l'impresa (zero C: è una clausola di `turn_bookkeeping`)

Come `exchange` è due fatti e non uno — perché un termine porta al massimo
quattro argomenti, e perché *quando accadde* e *che cosa fu* sono due domande:

```prolog
bookkeeper(undertaking).
turn_scratch(undertaking).
turn_scratch(undertaking_turn).
turn_scratch(undertaking_reached).
machinery(undertaking).        % come open_issue: la memoria di un lavoro non e' sapere sul mondo

turn_bookkeeping($Turn, undertaking) :-
    turn_goal($Turn, $Goal), turn_counter($N),
    assert(undertaking, $N, $Goal),
    assert(undertaking_turn, $N, $Goal).

% quel che l'impresa ha gia' ottenuto: un artefatto per volta, appena e' vero
turn_bookkeeping($Turn, undertaking) :-
    undertaking($Id, $Goal), action_yields($A, $Art), artifact_present($Art),
    assert(undertaking_reached, $Id, $Art).
```

`turn_goal/2` è il ponte con ciò che c'è: un goal nominato da `goal_cue/2`
(gen258), oppure — quando arriverà UC3 — il `subject`+`operation` del Task IR.
**Un turno che non intraprende niente non lascia niente**, esattamente come una
digressione non sposta il topic.

### 3.2 «Riprendibile» è una VISTA, non un puntatore

Ricalcata riga per riga su `current_topic`:

```prolog
unfinished($Id) :- undertaking($Id, $Goal), plan_goal($Goal, $Terminal),
                   naf(undertaking_reached($Id, $Terminal)).
later_unfinished($T) :- undertaking_turn($L, $Id), gt($L, $T), unfinished($Id).
resumable($Id) :- unfinished($Id), undertaking_turn($T, $Id), naf(later_unfinished($T)).
```

Un saluto in mezzo al lavoro non fa perdere il filo — e non perché qualcuno lo
abbia escluso: perché non ha lasciato un'impresa. **E un lavoro finito smette di
essere riprendibile da solo**, perché `unfinished` è una vista sul mondo
corrente e non può divergere da ciò che parrot0 sa.

### 3.3 Il passo successivo è derivato, mai memorizzato

```prolog
needs_met($Id, $A)  :- naf(unmet_need($Id, $A)).
unmet_need($Id, $A) :- action_needs($A, $Art), naf(undertaking_reached($Id, $Art)),
                       naf(plan_given($Art)).
next_action($Id, $A) :- resumable($Id), undertaking($Id, $Goal), plan_action($Goal, $A),
                        action_yields($A, $Art), naf(undertaking_reached($Id, $Art)),
                        needs_met($Id, $A).
```

`continue` = **cammina il piano dell'impresa riprendibile dalla prima azione il
cui artefatto non è ancora raggiunto**. È il walk di gen259, ripartito dallo
stato invece che da zero — e lo stato sono fatti, quindi ispezionabile,
salvabile, e soprattutto *contestabile*.

### 3.4 La mossa, che è il livello che K3 chiede e nessuno ha costruito

```prolog
appropriate_move($Ctx, resume)  :- resumable($Id).
appropriate_move($Ctx, resolve) :- issue_status($Word, open).   % la issue di gen394
```

Che `continue` scelga fra «riprendi un lavoro» e «chiudi una domanda rimasta
aperta» **non è un ramo nel C**: sono due regole di politica, e la loro
precedenza è a sua volta un fatto.

### 3.5 La superficie — la parte da poco, e teachable

```prolog
intent_phrase(resume_work, "continue").      intent_phrase(resume_work, "continua").
intent_phrase(resume_work, "go on").         intent_phrase(resume_work, "vai avanti").
intent_phrase(resume_work, "keep going").    intent_phrase(resume_work, "prosegui").
intent_phrase(resume_work, "carry on").      intent_phrase(resume_work, "e poi?").
learnable("continuare", resume_work, exact).
learnable("riprendere il lavoro", resume_work, exact).
```

Per gen214 una nuova forma diventa **dato a runtime**: `learn "dai" as
continuing` e non si ricompila. ⚠ E per la regola cardinale di
`kb-first-phrases`: **nessuna di queste stringhe deve finire in un array C.**

---

## 4. Perché vale già oggi, prima di B3

Il banco ha misurato che parrot0 non sa scrivere file (`THINKING_TODO` B3).
Si potrebbe concludere che il `continue` sia prematuro. **È il contrario**: è
proprio senza B3 che si vede se il design è onesto, perché tutte e tre le
risposte devono **nominare il buco** invece di fabbricare un passo.

| stato | risposta | perché è la risposta giusta |
|---|---|---|
| nessuna impresa aperta | *«Non ho un lavoro in sospeso da riprendere.»* | il template gemello — `i_don_t_have_a_previous_turn_to_recons` — **esiste già** in `messages.p0:52` |
| impresa aperta, `naf(action_impl($A,_))` | *«Riprendo `<goal>`: il prossimo passo è `<action_desc>`, ma non ho una primitiva che lo realizzi.»* | è lo stop onesto di gen259, ed è **la verità di oggi** |
| impresa arrivata al terminale | *«Quel lavoro è arrivato in fondo: `<terminal>`.»* | `unfinished` è una vista: si spegne da sola |

Confronto con oggi, misurato sotto il profilo della league:

```text
>>> continue     ->  «Hi there! What would you like to talk about?»
>>> go on        ->  «I don't understand that yet.»
>>> keep going   ->  «Thanks — I'm learning as we go.»
```

Un muro, un reset di cortesia, e un ringraziamento a vuoto. **Un gap nominato
batte tutti e tre** — ed è mantra #17: la facoltà che non sa onorare la
richiesta deve tacere o dire che cosa le manca, mai produrre.

---

## 5. Il legame col thinking, che è la ragione forte

`THINKING_TODO` A0 e `CHALLENGE_TODO` S2 dicono la stessa cosa: **il pensiero
peggiora la risposta**, e la guardia prende i muri ma non le degradazioni. Nel
transcript `gen500-stream-v3` il pensiero 2 ha trasformato la risposta in
*«What number should I use for "it"?»*.

L'ipotesi che questo documento propone: **il pensiero degrada perché non ha un
oggetto ben posto su cui pensare.** Pensa su una stringa e su una lista piatta
di letture, quindi «migliorare» può solo voler dire riscrivere la frase.

Un'impresa con un goal nominato, un piano derivato e un insieme `reached`
**è** quell'oggetto: un pensiero utile diventa una domanda con risposta
verificabile — *quale bisogno non soddisfatto posso ottenere per primo?* — e la
guardia diventa scrivibile, perché una degradazione è finalmente osservabile:
un passo di pensiero che **riduce** `next_action` o che propone un'azione con
un `unmet_need` è sbagliato per costruzione, non a giudizio.

È anche la forma che §17.3 chiede (`FRAME → SITUAZIONE → PIANO CAUSALE → PIANO
DI RISPOSTA`), nella sua istanza più piccola possibile che sia comunque vera.

---

## 6. Gate anti-impostor — nessuno è negoziabile

1. **La ripresa non nomina mai il task.** Il `continue_text` non porta
   informazione: se togliendo il goal dalla KB la risposta resta giusta, la
   conoscenza era nel posto sbagliato.
2. **Un dominio mai visto.** Insegnare un `plan_goal` nuovo *parlando*, aprirlo,
   interromperlo con una digressione, dire `continue`, e ottenere il passo
   giusto. Se serve una riga di C per il dominio nuovo, il design ha fallito.
3. **Ritrattabilità.** Ritirare `intent_phrase(resume_work, "go on")` deve far
   perdere *quella sola* forma — non la facoltà (è il gate UC3).
4. **Nessuna divergenza.** Aggiungere a mano il fatto terminale deve far
   sparire la ripresa **senza nessun retract**: se serve chiuderla
   esplicitamente, `unfinished` è diventato un flag e il design è tradito.
5. **Nessuna stringa nel C**, e i tre campi morti di `brain.c` cancellati nella
   stessa generazione in cui la vista li sostituisce.
6. **Il turno rubato.** `continue` non deve poter essere rivendicato dal
   generatore di storie (CHALLENGE_TODO S1): è la stessa classe, e questo è il
   caso più piccolo su cui provarla.

---

## 7. Ordine di lavoro

| | | |
|---|---|---|
| **R1** | `undertaking`/`undertaking_turn` + `resumable` come vista, sul solo dominio `plan_goal` esistente. Le tre risposte oneste. | KB pura |
| **R2** | La superficie EN+IT come `intent_phrase` + `learnable/3`. Cancellare i tre campi morti di `brain.c`. | KB + una rimozione dal C |
| **R3** | `next_action` derivato dal chainer, e il walk che riparte dallo stato (gen259). | KB |
| **R4** | `appropriate_move/2` con `resume` **e** `resolve`, e il primo consumatore vero di `open_issue` (gen394 chiude il suo cerchio dopo otto generazioni). | KB |
| **R5** | Il gate su un dominio insegnato parlando, e `PARROT0_THINKING=1` riacceso *solo* qui: la guardia di A0 su un oggetto ben posto. | KB + misura |

---

## 8. Cose da non fare

- ⛔ **Un campo «ultimo task» nel C.** È già stato provato, si chiama
  `last_input_raw`, ed è morto senza essere mai stato letto.
- ⛔ **Un `continue` che rilegge l'ultimo input e lo ri-sottopone.** Sembra
  funzionare e non riprende niente: rifà. La differenza si vede sul secondo
  `continue`, che è dove serve.
- ⛔ **Chiudere le imprese con un retract.** Uno stato che va spento a mano
  diverge da ciò che parrot0 sa; `issues.p0` lo dice già ed è l'errore che quel
  file ha evitato.
- ⛔ **Un modulo `mod_continue`.** Non è una facoltà nuova: è una mossa (K3) su
  strutture che esistono. Se serve un modulo, quasi sempre manca una classe
  (D31) — qui la classe è `resumable/1`.
