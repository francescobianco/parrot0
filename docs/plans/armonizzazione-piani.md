# Armonizzare i piani — frontier, comprensione universale, colla linguistica

> **Seguito operativo (ricognizione su `e66792b3`, gen506h):**
> [parrot0 organico — integrazione e addestramento](integrazione-cognitiva-operativa.md)
> traduce questa armonizzazione in contratti della IR condivisa, inventario
> delle capacità, incrementi O0–O8, curriculum e prove di parità con un LLM.
> Distingue i risultati già presenti dai lavori ancora aperti, inclusa la
> ritenzione cognitiva richiesta dopo la critica alla finestra di sei turni.

*Aperto il 2026-09-07 su richiesta di F.: «studia questi piani perché forse
molte cose vanno armonizzate». Letti per intero `the-linguistic-glue.md` e
`universal-comprehension.md`, e di `frontier-kb-natural-dialogue.md` la testa,
la tesi, la scala K0–K11, le fasi, i TODO aperti e lo stato; letto anche
`continue-as-resumption.md` (gen502), che è già un tentativo di convergenza.*

*Il reperto che ha aperto la domanda è in fondo (§6): tre turni di `make chat`
che i tre piani coprono ciascuno per un pezzo, e nessuno per intero.*

---

## 1. La tesi: sono tre lenti sullo stesso oggetto, non tre piani

| piano | che cosa guarda | il suo oggetto centrale |
|---|---|---|
| **la colla linguistica** (gen215–387) | i cinque sintomi dell'assenza di continuità, misurati da `glue-bench` | *operazioni deterministiche su stato di sessione*: `coref_resolve`, `correction_peel`, `memref_resolve`, `continue_resolve`, `topic_continue_resolve` |
| **comprensione universale** (gen267–) | la forma è sempre leggibile; il dato è gated; tre specie di lacuna | *il frame del turno* (`intent_schema`, `extract_frame`, il declino informato, l'acquisizione come passo di piano) |
| **frontier** (gen391–) | la parità dialogica con un LLM: letture, mosse, scope, registro, realizzazione | *la scala K0–K11* e il producer universale (`turn-frames.p0`) |

La colla dice **come** si mantiene la continuità (meccanismi), la comprensione
universale dice **che cosa** si legge di un turno (il frame), frontier dice
**chi decide** che cosa fare del frame (la mossa). Sono i tre piani dell'unica
architettura che frontier §6 disegna — PERCEZIONE → DELIBERAZIONE →
RAGIONAMENTO → REALIZZAZIONE — e il difetto è che ciascun documento ha costruito
il proprio pezzo **senza passare per l'oggetto degli altri due**.

## 2. Le sette divergenze, misurate

### 2.1 La colla è condotta scritta nel C — e frontier K3 dice che la condotta è KB

I cinque meccanismi della colla sono funzioni pre-dispatch (`99-registry.c`)
che leggono il **grezzo** (`low`), sbucciano un marcatore, riscrivono il turno e
ri-dispatchano. Sono esattamente ciò che il mantra #17 chiama «condotta nel
posto sbagliato» e ciò che il mantra #21 chiama *posizione* travestita da
capacità: chi vince è chi corre prima. Frontier K3 (`appropriate_move/2`,
`dialogue_move/2`) è il livello che dovrebbe **decidere** quella condotta, e —
misurato oggi — **non ha nessun consumatore in tutta la KB**: `open_issue/2` e
`answer_obligation/2` vengono scritti e mai letti.

> **Armonizzazione:** i meccanismi della colla sono le *implementazioni di
> fallback* delle mosse che K3 deve scegliere. Ognuno va riespresso come
> `dialogue_move` + policy KB (`move_policy/2` esiste già), e il C tiene solo la
> **meccanica** della riscrittura. `continue-as-resumption.md` (D49) l'ha già
> proposto per la mossa `resume`: è il primo caso, non un caso a sé.

### 2.2 Due produttori del turno: il frame universale e le sbucciature sul grezzo

Dal gen393 esiste **un** producer universale (`universal_turn_lead` →
`turn_span`, `turn_cue`, `turn_illocution`, `turn_focus`). I meccanismi della
colla non lo leggono: `pragma_peel`, `correction_peel`, `memref_resolve`,
`continue_resolve` rileggono la stringa da capo. È la forma ricorrente che i
piani stessi nominano tre volte — D33, D35, D37, *«due percorsi che devono
accordarsi e non condividono l'oggetto su cui accordarsi»* — applicata alla
famiglia che avrebbe dovuto essere la prima a condividerlo.

Il costo si è visto ieri e oggi: «una domanda non insegna» e «l'apertura di
discorso non è la testa del sintagma» sono stati chiusi facendo leggere a chi
impara la lettura **pubblicata** invece di una privata; la colla ha ancora le
sue.

> **Armonizzazione:** ogni sbucciatura diventa un consumatore del frame: la
> apertura sbucciata è già `turn_span` con ruolo, il marcatore di correzione è
> una cue di registro, l'antecedente è `discourse_referent/2`. Un solo
> produttore, come chiede frontier §16.

### 2.3 Tre tassonomie della lacuna

| documento | le specie |
|---|---|
| comprensione universale §10 | variante di superficie · costruzione mancante · forma telegrafica |
| frontier §2.1 | `missing_fact` · `missing_entity_bridge` · `missing_surface` · `missing_operator` · `missing_realization` |
| frontier K8 | surface gap · semantic gap · operator gap · realization gap |

Sono la stessa domanda — *che cosa manca perché il turno attraversi il grafo* —
vista da tre altezze: la §10 guarda la **distanza dalla frase che funziona**
(un carattere, una parola, la struttura), frontier guarda **quale arco** del
ponte manca. Non si contraddicono, ma nessuna delle tre è quella che `/debug`
mostra (`gap_kind`, `turn_gap_kind`), e `information_need/4` (frontier,
2026-09-01) è una quarta.

> **Armonizzazione:** una sola classe KB `gap_kind/2` con i membri di frontier
> §2.1 come *specie*, la distanza di §10 come **test diagnostico** che le
> distingue (è un metodo, non una tassonomia), e `information_need/4` come
> l'oggetto che le porta. Le sonde di `/debug` la leggono; i piani la citano.

### 2.4 Il ciclo di acquisizione è descritto due volte, ed era rotto

`universal-comprehension` §7 descrive l'acquisizione come passo del planner
(`goal → precond know(X) → acquire`), con il fetch che **scrive `pages/<key>.md`**
(gen240). Il gen436 ha vietato di archiviare le pagine e ha reso il fetch «in
memoria»; frontier lo registra. Nessuno dei due ha aggiornato l'altro, e il
risultato misurato oggi è che **dal gen436 nessun fetch è mai arrivato al
lettore**: `wiki_fetch_bilingual` scartava la prosa, `acquire_knowledge` cercava
il corpus locale che non esiste più, «Looking up beer…» finiva sempre in «I
still don't know much about beer» con la rete viva. Chiuso oggi (`32c3660`):
la prosa scaricata passa dal lettore di `read: …`, come chiedeva il gen436.

> **Armonizzazione:** §7 va riscritto sul ciclo vero — offerta → conferma →
> fetch in memoria → `learn_from_prose` → ri-dispatch della domanda — e il
> «passo del planner» di §7 e il «piano proposizionale» di frontier K6 sono lo
> stesso oggetto: la precondizione `know(X)` è un'issue aperta di K3.

### 2.5 Blocchi duplicati per copia, che divergeranno

- «DUE OBIETTIVI NUOVI» (F., 2026-09-06) è **identico** in testa a
  `frontier-kb-natural-dialogue.md` e `apprendimento-assistito.md`;
- «Lo spazio del discorso» e «Il cassetto senza maniglia» sono **identici** in
  `universal-comprehension.md` e nei TODO di frontier;
- la Disciplina della colla dice ancora *«`make test` resta ermetico»* — il
  contrario della regola R1 di `TEST_TODO` §0.0 (la KB ermetica non esiste).

> **Armonizzazione:** ogni blocco vive in **un** file e gli altri puntano.
> Proprietari proposti: i due obiettivi → `apprendimento-assistito.md` (la
> missione); il cassetto e lo spazio del discorso → `universal-comprehension.md`
> (è la sua giunzione); la disciplina della colla → una riga che rimanda a
> `TEST_TODO` §0.0.

### 2.6 Gli stati dichiarati sono di date diverse, e la suite di oggi li contraddice

- la colla dichiara «11/11 crisp HELD, qualitative 0» (gen386): oggi `glue.p0t`
  è **25/7** — `means/2` asserito non viene reso da «tell me about zorb»,
  «what is it part of» chiede «What number should I use for it», il pro-drop
  italiano mura (TEST_TODO, regressioni aperte 4);
- frontier §16 dichiara «copertura funzionale completa, ~70%» (17 agosto);
  `dialogue_moves`, `context_scope`, `sequential_view` erano **a zero** fino a
  stamattina per la verifica del fuoco (gen505t), e K3 non ha consumatore;
- comprensione universale dichiara vivo il declino informato (gen267): oggi
  «why is the sky green?» è tornato al muro cieco (`blankwall` 38) e il
  declino ha perso la frase «you can teach me with…».

> **Armonizzazione:** ogni «✅» porta la generazione **e** il cricchetto che lo
> prova (R5 della procedura): «11/11 HELD (gen386, `glue-bench`; al gen505y
> `glue.p0t` 25/7)». Un piano che non si ridata è una convinzione che scade.

### 2.7 La grammatica per giudicare e la grammatica per leggere

I due obiettivi nuovi chiedono che la conoscenza che **giudica** («my name are
Francesco») sia la stessa che **legge**. Oggi `p0_grammar_judgement` (gen505i)
consulta `word_number`, `copula_number`, `agreement_error` — classi proprie —
mentre il lettore usa `clause_copula`, `plural_copula`, `verb_stem`. Sono già
**due grammatiche**, nate a un giorno di distanza dal monito. Ed è la facoltà
che stamattina rivendicava con un rifiuto anche il codice (`code.p0t` 8 rossi).

> **Armonizzazione:** `copula_number/2` dev'essere una vista di `clause_copula`
> + `plural_copula`, non una seconda lista; il giudizio deve poter dire *quale
> classe del lettore* è stata violata.

> **Aggiunto la sera del 7 settembre:** F. ha chiesto di dichiarare in un punto
> unico la facoltà di rete — `la-rete-come-memoria-profonda.md`. È il caso
> concreto in cui i sei piani devono convergere, e il suo passo 6 (riprendere la
> questione dopo la lettura) è il consumatore di K3 della voce 1 qui sotto.

## 3. L'ordine, per leva

1. **Il consumatore di K3** — la mossa `resume`/`continue` di D49, costruita dal
   caso più piccolo (§6.2 qui sotto: «più precisamente»). È l'unico gradino che
   fa convergere colla, comprensione e frontier senza scriverne un quarto, e
   rende falsificabile un livello che oggi non lo è.
2. **Le sbucciature della colla come consumatori del frame** (§2.2), una alla
   volta, con l'ablazione: la stessa mossa di ieri sui lettori che imparano.
3. **Una sola `gap_kind`** (§2.3), letta da `/debug`.
4. **Riscrivere §7 di comprensione universale sul ciclo vero** (§2.4), e
   ridatare gli stati (§2.6) con i numeri della suite.
5. **De-duplicare i blocchi** (§2.5).
6. **Una grammatica sola** (§2.7), quando si riprende il giudizio grammaticale.

## 4. Che cosa NON armonizzare

- Le **strutture secondarie** restano (principio evolutivo di F., gen240): la
  colla non si smonta, si **riespone** come mosse. Il dispatch first-match rende
  innocua la ridondanza finché il più generale vince quando si applica.
- Le **ipotesi D1–D48** di frontier §18 non vanno accorpate: sono ipotesi datate
  con evidenza; accorparle cancellerebbe la storia che le rende leggibili.
- I **numeri storici** non si correggono: si datano.

## 5. Il metodo che i tre piani condividono, e che va scritto una volta

Tutti e tre arrivano — con parole diverse — alla stessa regola, che
`procedura-crescita-kb.md` ha poi formalizzato:

> Un difetto che migra fra facoltà non è di nessuna delle due: è una **lettura
> del turno che nessuno fa**, e va pubblicata una volta e consumata da tutte.

La colla lo dice come «un meccanismo per generazione, tirato dal primo caso»;
la comprensione universale come «chi altro deve accordarsi con questa, e su che
cosa?»; frontier come «un livello senza consumatore non si può falsificare». È
la stessa disciplina, e basta che viva in `procedura-crescita-kb.md` con i tre
rimandi.

## 6. Il reperto di F. (2026-09-07), letto con i tre piani

```text
>>> sai dove si trova malta
Europe.
>>> piu precisamente
Non capisco ancora.
>>> vorrei che mi fai un riassuto sui problemi ticipi nella produzione della birra
Non sono sicuro di aver seguito. Puoi dirlo in un altro modo?
```

### 6.1 «sai dove si trova malta» → Europe — giusto ma grossolano

La KB tiene `continent_of(malta, europe)`. La pagina Wikipedia dice «an island
country located in the central Mediterranean Sea», e da oggi il fetch arriva al
lettore. Ma la frase di testa produce `located_in(republic,
central_mediterranean_sea)`: l'apposizione «officially the Republic of Malta»
si mangia il soggetto. È D29 (l'affermazione che copre meno di quanto legge) e
il gemello del circuito di oggi sul prefisso: **l'apposizione fra soggetto e
copula non è il soggetto**. Comprensione universale §4bis la nomina
(«apposition(NP)»), nessuno l'ha costruita. Finché non lo è, il fatto giusto
sta nel cassetto sbagliato.

### 6.2 «più precisamente» — la mossa che tutti e tre descrivono e nessuno ha

È una **precisazione della domanda precedente**: la stessa issue, con la
richiesta di un valore *più specifico*. La colla ha `continue_resolve` (la
precisazione che continua il **calcolo**) e `topic_continue_resolve` (il
soggetto eliso), entrambi con un connettore obbligatorio e un residuo con
contenuto — qui il residuo è vuoto, è solo la richiesta. Frontier K3 la chiama
`qualify`/`resume` su `open_issue`; comprensione universale la chiama «la
domanda di seguito», abilitata dal referente. Gli ingredienti ci sono tutti:
l'issue dell'ultimo turno (`issues.p0`), il referente (`discourse_referent`),
una regola di specificità (un valore V' tale che `located_in(V', europe)` è
più preciso di `europe`). Manca **il consumatore di K3** — è la voce 1
dell'ordine.

### 6.3 «un riassunto sui problemi tipici nella produzione della birra» — tre lacune impilate

1. **Superficie:** «riassuto», «ticipi» — specie 1 di comprensione universale
   §10 (riparazione ortografica come ipotesi dichiarata, gen385): oggi non
   scatta su queste due.
2. **Atto:** `summarize` nella KB significa *riassumi la conversazione*
   (`mod_summary`); «riassunto su X» è un atto diverso con uno slot topic. È la
   distinzione frame_act + slot di K1, mai fatta per questo verbo.
3. **Contenuto:** i «problemi tipici nella produzione» non stanno nella frase di
   testa che l'API `summary` restituisce; servono le **sezioni** della pagina
   (un secondo endpoint) e poi una **selezione per aspetto** e una **sintesi
   proposizionale con provenienza** — K7 `summary_contains`, D8 «la sintesi è
   compressione con recupero». È il gradino più lontano dei tre, ed è quello che
   F. intende con «pianificazione della lettura»: il piano è *leggi la pagina →
   trova le sezioni sull'aspetto → estrai → componi*, e ogni passo è una
   precondizione di K3/K6.

Risposta alla domanda di F.: **sì per 6.1 e 6.2 con ciò che esiste** (il fetch
riparato + l'apposizione + il consumatore di K3); **6.3 chiede tre cose nuove**,
di cui la prima (le sezioni) è meccanica e le altre due sono K1 e K7.

## 7. gen506g — il consumatore di K3 ha un nome: `docs/plans/dialogica.md`

La voce 1 dell'ordine (§3) e' ora un piano proprio: il tabellone unico
(`open_issue` per genere), le mosse come letture del frame sulla questione
massima (L2), e i cinque incrementi. Le tre lenti convergono li': frontier
K3 da' i predicati, comprensione universale da' la lettura del turno, la
colla da' le mosse gia' costruite (coref, correzione, ellissi) da rileggere
come `move_addresses`.
