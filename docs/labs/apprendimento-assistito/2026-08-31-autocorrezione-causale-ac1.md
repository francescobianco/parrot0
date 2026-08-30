# AC0–AC1 — autocorrezione causale del token, non del campione

**Data:** 2026-08-31

**Stato:** `meta-capability-only`

**Prompt motivante:** `quanot fa 2 +3`

**Vincolo:** KB-first, nessuna mappa di typo, nessun fatto di dominio scritto a mano

## Parametri

| parametro | valore |
|---|---|
| dominio | robustezza di superficie nelle richieste aritmetiche |
| obiettivo | chiudere il prompt motivante attraverso una famiglia revocabile e trasferibile |
| budget | un verticale causale, poi checkpoint |
| fonti | comportamento del motore e piani versionati di autocorrezione/autocrescita; nessuna fonte fattuale esterna necessaria |
| target world facts | 0: e' sviluppo di meta-capacita', non training fattuale |
| target capabilities | 1: segmentazione contestuale di operatore unito all'operando destro |
| stop condition | mapping lessicale del typo, rewrite silenzioso, collisione su segni/unita', assenza di grow/retract |

Non e' stato eseguito `/save`: nessun fatto vero sul mondo e' stato insegnato in
questa fase. La policy aggiunta manualmente e' infrastruttura generale e viene
contata come meta-capacita', non come apprendimento naturale.

## Studio preliminare

Sono stati letti e confrontati:

- `docs/autocorrezione.md`;
- `docs/plans/fix-patterns.md`;
- `docs/plans/autocrescita.md`, `autocrescita-v2.md`, `autocrescita-v3.md`;
- le sezioni di superficie, gap e registro di `question-emergence.md`;
- `tests/p0t/language/spell_repair.p0t`, `meta/self_repair.p0t`,
  `meta/correction.p0t`, `knowledge/gap_dialogue.p0t`;
- `kb/core/gap-kinds.p0`, `kb/core/meta.p0`, `src/brain/20-math.c` e il
  dispatch del turno.

Il materiale converge su quattro invarianti: il gap nasce nella traccia; una
riparazione e' un'ipotesi; replay e pertinenza precedono la promozione; il nuovo
membro deve crescere e ritrarsi a runtime.

## Baseline e falsificazione dell'ipotesi intuitiva

Nel profilo AGI italiano, da processi puliti:

```text
quanot fa 2 +3    -> Non capisco ancora.
quanto fa 2 +3    -> Non capisco ancora.
quanto fa 2 + 3?  -> 5.
quanot fa 2 + 3?  -> 5.
```

Il refuso non e' necessario al fallimento. `parse_num("+3")` lo accetta come
numero positivo; l'espansore compatto preservava quindi un token che nel
contesto di un operando sinistro completo deve poter essere letto come `+`,
`3`. La causa e' una segmentazione contestuale, non ortografia o intent.

## Implementazione generale

La KB contiene la licenza:

```prolog
token_variation(joined_infix_rhs, "split_operator_prefix").
```

Il C implementa una sola meccanica cieca, selezionata interrogando la policy.
L'inventario dei simboli non e' piu' enumerato nel nuovo scanner: ogni
carattere candidato passa da `infix_operator/2` e `operator_symbol/2`. Il
contesto richiede un operando sinistro gia' leggibile e un operando destro
valido; per questo `+39` dopo `chiama` non viene catturato e `-3` dopo un
operatore separato resta un numero unario.

Ogni uso pubblica:

```prolog
turn_surface_repair(Class, split_operator_prefix, Original, Normalized).
```

Il record scade al turno successivo; l'utterance originale resta separatamente
nel log. La normalizzazione e' quindi osservabile e revocabile, non una
riscrittura muta.

## Matrice di trasferimento

Il dossier `tests/p0t/math/arith_surface_repair.it.p0t` passa **27 proprieta'**:

- prompt motivante senza punto interrogativo;
- refuso invariato con espressione esplicitamente segmentata;
- forget/reteach della policy senza rebuild;
- `+`, `-`, `*`, `/` uniti al secondo operando;
- valori diversi e cue `fa`, `calcola`, `calculate`, `compute`;
- segno unario iniziale e dopo un operatore;
- negativo telefonico `chiama +39`;
- nuova superficie `x` insegnata come operatore di moltiplicazione a runtime,
  usata su `6 x7` e poi ritratta.

La matrice mostra trasferimento per classe, non soltanto variazioni del prompt
docente. La conclusione resta locale alla firma `operando operatore+operando`.

## Ablation e metriche

```text
CausalPrecision       = 1/1
FamilyTransfer        = 12/12 celle positive preregistrate
CollisionRate         = 0/3 confini espliciti
RuntimeGrowth         = pass
RuntimeRetract        = pass
CriticalTokenPreserve = pass per numeri e segni coperti
FalseRewriteRate      = 0 nel dossier
```

Togliere `token_variation/2` riapre sia `+3` sia `/5`; riasserirla ripristina il
prompt motivante. Togliere soltanto `infix_operator("x", times)` spegne `x7`
senza spegnere la famiglia. Sono due livelli di causalita' distinti: licenza
dell'operazione e appartenenza del membro.

## Verifica software

```text
make build                                      -> clean
arith_surface_repair.it.p0t                     -> 27 passed
arith_nl.it.p0t                                 -> 16 passed
arith_flex.it.p0t                               -> 5 passed
arith.p0t                                       -> 8 passed
spell_repair.p0t                                -> 8 passed
make soft-test                                  -> 55 passed, 1 failure storico
```

Il rosso di `frontier_chat_audit.it.p0t` linea 97 e' lo stesso gia' osservato
prima di AC1: l'aspettativa cerca la frase corta `I don't know about
designation`, mentre il renderer attuale produce la variante piu' ricca `I
don't know much about your designation yet. Want me to look it up?`. Nessun
codice o test di quella famiglia e' stato modificato.

`arith_flex.p0t` inglese mostra analogamente un'aspettativa storica del gap
`silver` diversa dal renderer AGI corrente; le undici proprieta' aritmetiche
precedenti passano. Non e' stato riallineato per rendere verde il checkpoint.

## Strategia CADRE e limite

Il metodo generalizzato e' CADRE: *Causal Ablation, Declarative Repair,
Exogenous transfer*. Prima di promuovere una correzione, costruisce un reticolo
di perturbazioni, ne trova il sottoinsieme causalmente minimo, conserva proof e
originale, trasferisce su assi indipendenti ed esegue ablation.

AC1 non risolve ancora typo lessicali, slang, omissioni grammaticali o intenti
rumorosi. Queste classi entrano in AC2 tramite un corpus stratificato di almeno
240 prompt; il prossimo ciclo dialogico GD1 usa almeno 360 turni multi-registro.
Il successo aritmetico e' il primo ratchet del metodo, non una dichiarazione di
autocorrezione universale.

## Conteggi LEARN_PROTOCOL

```text
Nuovi fatti veri del mondo salvati in KB (W): 0
Fatti linguistici appresi parlando (L): 0
Meta-policy/infrastruttura generale (C): 1 famiglia
Provenienza/genealogia runtime (P): 1 nuovo record di turno
Clausole invalide (X): 0
Clausole dichiarate da /save (S): non applicabile, /save non eseguito
Stato: meta-capability-only
```
