# THINKING_TODO — la coda del pensiero e dell'agente di codice

> Due piani, una coda sola, perché si incrociano: il **thinking**
> ([`docs/plans/thinking.md`](docs/plans/thinking.md)) è lo strato che
> rielabora un output con meta-prompt fino alla chiusura della pipeline; il
> **coding agent** ([`docs/plans/universal-code-comprehension.md`](docs/plans/universal-code-comprehension.md))
> è ciò su cui quel pensiero avrà più da pensare. Tenerli in due code separate
> avrebbe nascosto proprio i punti in cui si toccano.

## ⛔ Le due parole, da non confondere mai più

| | |
|---|---|
| **reasoning** | l'inferenza che parrot0 fa già: un turno entra, il solver risolve, esce una risposta. **Resta com'è.** |
| **thinking** | la rielaborazione dell'output con meta-prompt **fino alla chiusura della pipeline**: il risultato rientra, agganciato a un meta-prompt, e il giro si ripete finché una condizione dichiarata non lo chiude |

Non è pedanteria: hanno costi, garanzie e condizioni d'arresto diversi. Un
rientro costa un **turno intero**, non un passo di inferenza.

---

## Parte A — THINKING

### A0. ⛔ Il criterio di NON-PEGGIORAMENTO (prima voce, e non è negoziabile)

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

**Serve un criterio di non-peggioramento più forte di «non è un muro»**, e deve
essere conoscenza. Ipotesi da provare, in ordine: un passo che produce una
*domanda* invece di un'affermazione non chiude una pipeline; un passo che non
aggiunge nessun fatto (`reading_fact` invariati) non propaga; il risultato finale
deve dominare il primo su un criterio dichiarato, non essere semplicemente
l'ultimo.

### A1. L'esecutore c'è, ma esegue un solo schema alla volta

`brain_think` prende lo schema dal turno (`thinking_for/2`) o quello di default
(`thinking_default/1`). Manca: più schemi applicabili, la scelta fra loro, e il
meta-reasoning che la governa (E7 del piano).

### A2. Gli esperimenti del piano ancora non eseguiti

`docs/plans/thinking.md` §3 ne elenca otto con misura e falsificazione. Stato:

| | |
|---|---|
| E1 lo schema è conoscenza | ✅ `thinking_graph.p0t` |
| E2 il grafo non è una catena | ✅ (invertire un arco inverte il rango) |
| E3 l'arresto ha una ragione dichiarata | ⛔ **non provato**: `thinking_stop/2` esiste, nessuno lo legge |
| E4 la critica ritira davvero qualcosa | ⛔ **non provato**, ed è quello che conta di più |
| E5 quanto costa | ⛔ **non misurato**: un rientro è un turno intero, il costo va profilato con `/debug` |
| E6 l'operatore è una variabile | ⛔ |
| E7 meta-reasoning: scegliere lo schema | ⛔ |
| E8 anti-impostore (togliere un nodo alla volta) | ⛔ **da fare su ogni schema attivo** |

**E4 ed E8 sono i due che decidono se questo strato serve.** Se la critica non
ritira mai niente ed E8 è maggiore di zero, il thinking è teatro cognitivo e va
tolto, non migliorato.

### A3. Il costo, prima di accendere il thinking di default

Un rientro = un turno = ~250 ms a regime (gen491). Due rientri triplicano il
turno. Prima di alzare `thinking_max_steps/1` sopra 2 servono: la misura di E5, e
le viste materializzate sui predicati che il grafo rilegge (§L). **La regola di
F. vale qui più che altrove: un timeout di dieci secondi è già un sintomo.**

### A4. Il meta-prompt deve essere una capacità, non un'attività umana

Difetto già pagato: *«quali assunzioni non dimostrate ci sono in …»* è una frase
sensata per una persona e un muro per parrot0. Serve un **gate**: un
`thinking_prompt_text/2` che nel suo primo uso mura va segnalato come mal
formato, non lasciato lì a degradare i turni.

### A5. La UI, oltre al grigio

Fatto: meta-prompt + output, grigio, su `stderr`, mentre accade. Manca: un modo
per **ripercorrere** i pensieri di un turno passato (`/thinking` come `/debug`),
e la resa del pensiero nelle interfacce non-tty (MCP, `--test`), oggi assente.

---

## Parte B — CODING AGENT

### B0. Severità, ordinamento e tradeoff fra finding

Oggi `code_finding/3` produce un **insieme**, non una lista ordinata. Il §5 del
piano chiede severità come funzione di impatto/probabilità/scope/policy, le
alternative, e l'azione informativa che separa due ipotesi concorrenti. Nulla di
tutto questo esiste.

### B1. ⛔ La latenza (§5.2), e la regola di onestà che la governa

`speed of X` senza profilo dichiara che manca ed è giusto. Ma:
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

### C3. Il loop di specchio è più pericoloso sul codice

`reentry_brings/3` impedisce a un rientro di girare a vuoto sulla stessa
conoscenza. Sul codice il rischio è peggiore che altrove: parrot0 che critica la
propria lettura di un file **con la stessa lettura** produrrà sempre conferme.
Un rientro sul codice deve portare qualcosa di *esterno alla lettura*: un
oracolo, un test, una misura.

### C4. Il costo si somma

Un piano su strumenti (N letture) più un thinking (M rientri) costa N+M turni.
Le due code condividono lo stesso §L, e devono condividere lo stesso budget: non
due tetti indipendenti che si moltiplicano.

---

## Come si usa questa coda

- **Prima A0**, perché finché il thinking può peggiorare una risposta non va
  acceso di default.
- **Poi C1**, perché decide se il thinking serve davvero e costa poco provarlo.
- **Poi E5/A3**, perché senza la misura del costo ogni altra decisione è a occhio.
- Le voci di B avanzano in parallelo: non dipendono dal thinking.

Regola di chiusura, la stessa di sempre: **una voce si chiude quando un
cricchetto la tiene ferma e un'ablazione la fa cadere.** Un TODO chiuso senza
falsificazione è un TODO spostato.
