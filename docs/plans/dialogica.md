# La dialogica di parrot0 — una teoria della conversazione, KB-first

> **gen506g, 9 settembre 2026, su richiesta di F.:** «siamo molto indietro da
> una vera conversazione naturale… più che andare sulla soluzione esplicita
> cerca di costruire una teoria della dialogica efficace… tale per cui
> risolvere questi problemi sia in un quadro di KB-first e di comprensione
> universale». Questo documento è quella teoria. Non aggiunge un quarto piano:
> è il consumatore che frontier **K3** chiede dal gen502 e che
> `armonizzazione-piani.md` §3 mette al primo posto, scritto abbastanza
> preciso da spiegare le chat imbarazzanti PRIMA di toccare il C.

## 0. Il reperto

```
>>> parlami dei plc
Su plc non so ancora molto. Vuoi che cerchi?
>>> dai
«plc» può voler dire più cose: - Future plc … - Programmable logic controller … Quale intendi?
>>> parlami di programmable ...
Su programmable non so ancora molto. Vuoi che cerchi?          ← (1)
>>> intendevo Programmable logic controller
Vediamo cosa trovo su programmable logic controller... Il controllore logico programmabile è …
>>> cosa sono i processi industriali
«programmable» può voler dire più cose: … Quale intendi?        ← (2)
```

(1) Sotto la domanda «Quale intendi?» il turno «parlami di programmable…» è
stato letto **da zero** — come una richiesta nuova su una parola nuova — e ha
aperto una SECONDA domanda. Era una risposta parziale alla prima.
(2) Tre turni dopo, la domanda vecchia («programmable»?) era ancora aperta e
ha catturato una domanda nuova dell'utente, che non c'entrava.

Nessuno dei due è un bug di una parola. Sono violazioni di leggi della
conversazione che parrot0 non possiede: non ha un modello di **che cosa è
aperto, in che ordine, e come un turno vi si legge contro**. Ogni riparazione
puntuale (un'altra lista di parole, un altro `pending_*`) aggiunge un
tabellone parallelo e rende il prossimo reperto più probabile. Lo dico contro
di me: gen506d (`pending_disambiguation`) e gen506f (`pending_gap` che resta
aperta, `offer_resolution`) sono esattamente questo — giusti nella lettura
(il «sì» come regola sul frame), sbagliati nel posto (due stati separati
invece di UNO).

## 1. La tesi

**La conversazione è un tabellone condiviso** (Ginzburg: *dialogue
gameboard*; frontier K3 lo chiama «stato dialogico»). Il tabellone ha quattro
parti, tutte fatti KB:

| parte | che cosa | dove sta oggi |
|---|---|---|
| **FATTI** condivisi | ciò che è stato affermato e accettato, con provenienza | la KB di sessione, `fact_source`, `exchange` (discourse.p0) |
| **QUD** — le questioni in discussione | le domande aperte, **ordinate**: la più recente è la *max-QUD* | `open_issue/2` (issues.p0, senza consumatore); `pending_gap`, `pending_disambiguation` (gen506d/f, paralleli) |
| **ULTIMA MOSSA** | che cosa ha appena fatto chi ha parlato per ultimo | `turn_module`, `turn_outcome`, `last_acquisition`, `last_reply` |
| **OBBLIGHI** | chi deve la prossima mossa e su che cosa | `answer_obligation/2` (issues.p0), mai letto |

**Ogni turno è una mossa, e si legge prima sul tabellone, poi da zero.** Il
frame universale del turno (`turn_illocution`, `turn_cue`, `input_node_atom`,
i referenti) è la stessa lettura di sempre; la novità è la domanda che si
pone per prima: *questo turno indirizza la max-QUD?* — e come. Solo se la
risposta è «no, è altro» il turno passa alle facoltà come turno nuovo. È
comprensione universale applicata alla conversazione: non un rilevatore di
«sì», ma la lettura del turno **relativa a una domanda che parrot0 stesso ha
posto**.

### 1bis. Il prompt e' la sessione (F., gen506h)

«Quando si fa inferenza su un prompt in realta' si sta facendo inferenza
sull'intera sessione del dialogo… il prompt e' sempre l'intera sessione con
aggiunto in coda il prompt corrente.» E' la forma operativa del tabellone, e
va presa alla lettera **come struttura, non come testo**: il frame di ogni
turno (nodi, cue, letture, risposta, tema, entita') resta in KB sotto lo
scope `turn_N` dentro una finestra dichiarata (`session_window/1`), e
`current_turn` e' il turno in corso. Una regola quantifica su due turni come
su uno. Nessuno stato del dialogo vive piu' solo nel C: `last_reply`,
`last_entity`, `last_topic`, `last_input` sono `turn_reply/2`,
`turn_entity/2`, `turn_topic/2`, `turn_input/2` del turno che li ha prodotti
(i campi C restano come cache da ritirare). Quali predicati siano «di turno»
e' `turn_scoped/2` (discourse.p0 §6). Fatto in gen506h; i primi consumatori:
la scelta letta sul frame con la risposta parziale che restringe (L3) e la
domanda nuova che non viene catturata (L4).

> ⛔ **Critica di F. alla soluzione (gen506h, `session_window(6)`):** «la
> finestra fissa e' solo un difetto delle abilita' cognitive: il contesto va
> ottimizzato per abilita' cognitive, non per logica cablata». Un contatore
> decide per numero di turni una cosa che dipende dal contenuto: una
> questione aperta al turno 3 e' ancora aperta al turno 12, ma il turno che
> l'ha posta e' caduto al 10 — il tabellone dice «aperto», la memoria dice
> «dimenticato», e nessun fatto registra la caduta. La forma giusta e' la
> ritenzione come REGOLA KB sul contenuto: un turno si tiene finche' qualcosa
> lo cita (una questione aperta nata li', un referente vivo, una mossa che vi
> rimanda) e cade quando nessuno lo cita piu'; il costo, se va limitato, e' un
> vincolo secondario e dichiarato. Il numero e' un ponte da togliere, non un
> parametro da regolare.

**Le finestre non sono un vincolo di progetto.** I 256 byte di `norm`/`canon`
nel dispatch, i 4096 della prosa, `KB_TERM_LEN` per un fatto: sono debito.
L'ambizione (F.) e' incollare un file Python di mille righe e dire
«trasformalo in C»; la lettura universale deve reggere un turno di 100 KB
come uno di dieci parole, perche' il frame e' la lettura e il testo e' solo
la sua sorgente. Ogni finestra va tolta quando la si incontra (C_TODO), mai
alzata di poco.

## 2. Le leggi

Tutte in KB. Il C tiene il tabellone (asserisce, ritira, ordina) e chiede.

**L1 — Un tabellone solo.** Esiste un solo predicato per «è aperto»:
`open_issue(Issue, Kind)` con `issue_status(Issue, open|resolved|superseded)`
e il contenuto per genere (`issue_topic`, `issue_option(Issue, N, Titolo)`,
`issue_question(Issue, Testo)`). I generi oggi: `gap_offer` («vuoi che
cerchi X?»), `choice` («quale intendi?»), `question` (una domanda dell'utente
rimasta senza risposta, gen394). `pending_gap` e `pending_disambiguation`
diventano VISTE su questo, poi spariscono.

**L2 — Il turno si legge prima come mossa sulla max-QUD.**
`move_addresses($T, $Issue, $How)` è una regola KB per genere, sul frame:
- `accept` / `refuse` — cue di assenso/dissenso (gen506f `offer_resolution`
  è già questa regola per `gap_offer`);
- `answer` — il turno porta il valore che la domanda chiedeva;
- `partial` — il turno porta una parte del valore (una parola di un'opzione);
- `clarify` — il turno chiede qualcosa SULLA domanda («cosa hai trovato?»,
  «quali opzioni?», «perché?»);
- `supersede` — il turno è una domanda nuova (forza `question` nel frame) che
  non indirizza l'issue;
- `unrelated` — nessuna delle precedenti.

**L3 — Una risposta parziale restringe, non riapre.** Sotto `choice`, un
turno che nomina parte di un'opzione riduce le opzioni a quelle compatibili:
se ne resta una, l'issue è risolta con quella; se ne restano più d'una, si
richiede con la lista ristretta. Non si apre MAI una seconda issue sullo
stesso tema finché una è aperta (reperto 1).

**L4 — Una domanda nuova supera la max-QUD.** Se il frame legge il turno come
domanda e nessuna regola L2 lo lega all'issue, l'issue vecchia resta aperta
ma **non è più max-QUD**: il turno si risponde (o mura) per sé (reperto 2).
L'issue vecchia si riprende con una mossa di ripresa («torniamo ai plc»,
«continua» — `continue-as-resumption.md`, D49) o si chiude per politica
(`issue_expiry(Kind, N turni)`, un fatto).

**L5 — Il turno non rivendicato si legge con la QUD.** Se nessuna facoltà
serve il turno: sotto `gap_offer` la politica dice `accept`
(`offer_unclaimed_turn(accept)`, insegnabile: «non cercare se non dico di
sì»); sotto `choice` si ripete la domanda con le opzioni; senza QUD, il muro
informato. La politica è un fatto per genere, non un ramo.

**L6 — L'ultima mossa è un fatto, e si può chiedere.** «cosa hai trovato»,
«perché così», «cosa è rimasto in sospeso» sono mosse `clarify` sul
tabellone (discourse.p0 §1, `mod_strategy`, `last_acquisition`): la stessa
lettura, non intenti a parte.

**L7 — Ciò che parrot0 dice apre obblighi; ciò che l'utente afferma va
accolto.** Una domanda posta da parrot0 crea `answer_obligation(Issue, user)`
(già in issues.p0); un'affermazione dell'utente è `proposed` finché una
mossa la accetta o la corregge (`grounding_status`, K3). È il gradino dopo:
serve perché «no, intendevo…» sia una correzione della mossa precedente e non
una frase nuova.

## 3. I reperti, letti con le leggi

| turno | che cosa ha fatto parrot0 | legge | lettura giusta |
|---|---|---|---|
| «dai» sotto «Vuoi che cerchi?» | accettato (gen506f) | L2 accept | ok: `assent_word(dai)` è un fatto |
| «parlami di programmable…» sotto «Quale intendi?» | letto da zero: nuova `gap_offer` su «programmable» | **L3, L1** | `partial`: le opzioni che contengono «programmable» sono due (logic controller, …) → richiedere con quelle; se una, risolvere |
| «intendevo Programmable logic controller» | risolto per parole del titolo | L2 answer | ok; «intendevo» è anche una cue di correzione (L7) |
| «cosa sono i processi industriali» | catturato dall'issue vecchia su «programmable» | **L4** | `supersede`: è una domanda nuova; si risponde/mura su quella; «programmable» resta aperta non-max |
| (chat precedente) «una panoramica generale» sotto l'offerta | muro | L5 | accept per politica (fatto gen506f) |
| (chat precedente) «cosa hai trovato» | «Non capisco» | L6 | clarify sull'ultima mossa (fatto gen506d) |

Le due cose fatte (gen506d/f) sono corrette **come letture** e vanno solo
spostate sul tabellone unico (L1). Le due mancanti (L3, L4) non si possono
fare bene con due tabelloni: è il motivo per cui l'ordine sotto parte da L1.

## 4. Che cosa esiste, misurato

- `kb/core/issues.p0` (gen394): `open_issue/2`, `issue_status/2`,
  `answer_obligation/2` — costruiti, **mai consumati** (K3 «ha un contabile
  e non ha chi riprende»).
- `kb/core/discourse.p0`: `turn_bookkeeping/2` (l'aggancio per far crescere
  il tabellone da regole), salienza, ellissi, la ripresa («cosa è rimasto in
  sospeso»).
- gen506d/f: `pending_gap` (+question), `pending_disambiguation` +
  `disambiguation_option/3`, `offer_resolution/2`, `offer_unclaimed_turn/1`,
  `last_acquisition/3`, `acquire_and_report`, `pending_offer_fallthrough` —
  funzionano (`offer_context.p0t` 21/21, `disambiguation.p0t` 19/19) e sono
  il debito L1.
- Il frame universale del turno: forze, cue, nodi, referenti — è la lettura
  su cui L2 scrive le sue regole; niente da inventare.

## 5. Il piano, a incrementi (ciascuno: una chat nel banco piccolo, un `.p0t`, `make soft-test`)

| # | incremento | legge | prova |
|---|---|---|---|
| I1 | **Un tabellone.** `open_issue(Issue, gap_offer|choice)` + contenuto; `pending_gap`/`pending_disambiguation` diventano viste derivate, poi si tolgono. `offer_resolution` diventa `move_addresses(_, _, accept|refuse)` per `gap_offer`; la scelta per parole/ordinale diventa `move_addresses(_, _, answer)` per `choice`. **Zero comportamento nuovo**: le stesse chat, gli stessi test. | L1, L2 | offer_context, disambiguation invariati |
| I2 | **Parziale.** `move_addresses(_, Choice, partial)`: le opzioni compatibili con le parole del turno; una → risolta; più → richiesta ristretta. | L3 | reperto 1 |
| I3 | **Superamento.** Una domanda nuova non è catturata dall'issue vecchia; l'issue resta, non-max; la ripresa esplicita («torniamo a…», «continua») la rende max di nuovo (D49). `issue_expiry` come fatto. | L4 | reperto 2; «torniamo ai plc» |
| I4 | **Il consumatore K3.** `appropriate_move(Context, answer|clarify|qualify|repair)`: «più precisamente» (armonizzazione §6.2) come `qualify` sull'issue risolta dell'ultimo turno. | L2 | §6.2 |
| I5 | **Grounding.** Le affermazioni dell'utente `proposed` → `accepted`; «no, intendevo…» come `repair` della propria ultima mossa. | L7 | correzioni |

Ogni incremento tocca il C solo per il tabellone (assert/retract/ordine) e
per chiedere alla KB; le letture, le politiche e le parole sono KB.

## 6. Il metodo, in una riga per regola

- Davanti a una chat imbarazzante: **prima la legge violata, poi il
  consumatore**; mai «la parola che mancava».
- Un turno sotto una domanda di parrot0 non si capisce da zero: si legge sul
  tabellone con il frame universale.
- Le politiche («se non dico no, vai», «scadi dopo tre turni») sono fatti
  insegnabili, non rami.
- La prova è uno **script di dialogo** (3–6 turni) nel banco piccolo, non un
  turno isolato: `tests/comprehension-probe/smoke.txt` accetta anche script.
- Due stati per la stessa cosa sono un bug anche quando funzionano.

## 7. Che cosa NON è

Non è un automa a stati compilato («stato: attesa conferma»), non sono
intenti («intent: confirm»), non sono rilevatori di frasi. Le mosse sono
LETTURE del frame relative a una domanda, e la domanda è un fatto che
parrot0 ha messo lui sul tabellone: per questo può spiegarle («perché mi hai
chiesto quale?») e può farsele insegnare.
