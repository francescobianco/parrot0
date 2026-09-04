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

- **Prima C1**, ora che A0 è chiusa: critica reale dei finding e ritiro misurato.
- **Poi E8 completo** sullo schema attivo, nodo per nodo.
- **Poi E5/A3**, perché senza la misura del costo ogni altra decisione è a occhio.
- Le voci di B avanzano in parallelo: non dipendono dal thinking.

Regola di chiusura, la stessa di sempre: **una voce si chiude quando un
cricchetto la tiene ferma e un'ablazione la fa cadere.** Un TODO chiuso senza
falsificazione è un TODO spostato.
