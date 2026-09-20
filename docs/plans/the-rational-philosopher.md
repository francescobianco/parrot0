# The Rational Philosopher — conversare nello spazio logico dell'interlocutore

## Primo giro — dal dialogo interrogativo all'iniziativa motivata (storico)

> ⚠ **Questo era l'handoff; ora non lo è più.** L'handoff vigente è la sezione
> successiva, che ne **rettifica il §D.1 e il §D.4**. Questa resta per il
> contesto e per le parti non toccate (§B, §C, §E, §F, §G).

**Leggere questa sezione dopo l'handoff vigente.** La richiesta
attuale di F. è più ampia: trasformare il mondo allargato in un framework
operativo per i contesti umani, comprese aperture senza domanda, desideri,
incertezze, vissuti, creazione, collaborazione e continuazioni. Quando manca
una direzione, parrot0 deve poter proporre un seguito pertinente di propria
iniziativa, lasciando correggibile la propria interpretazione.

F. chiede ora un passaggio di consegne utilizzabile anche da un coding agent
meno potente. **Non ripartire dall'implementazione di F1 alla cieca:** prima
riprodurre il circuito e i difetti riportati qui. La nuova consegna è un
**prototipo KB-only funzionante su un sottoinsieme, non il completamento del
piano né comprensione universale certificata**. Le modifiche sono nel working
tree; nessun commit o push è stato eseguito per questo lavoro.

## ⇨ HANDOFF VIGENTE — secondo giro: i confini misurati (20 settembre 2026)

**Questa è la sezione da leggere per prima.** Rettifica il §D.1 e il §D.4 del
primo giro: non riaprirli senza averla letta.

### In una pagina

Consegna: il piano, più questo dialogo reale del profilo `agi`:

```
>>> come stai
Sto bene, grazie. Come posso aiutarti?
>>> facciamo filosofia insieme
Non capisco ancora.
```

I due difetti che il §D.1 chiamava «le due lesioni concrete» **non erano quello
che il §D.1 diceva**, e il turno di F. era una terza cosa ancora. Sei reperti
(S1-S6), tutti misurati; quattro chiusi, uno chiuso a metà, sei rossi lasciati
aperti e dichiarati.

| | stato |
|---|---|
| `facciamo filosofia insieme` → proposta, `perché`, `continua`, vincolo, rifiuto | ✅ chiuso (S3) |
| `i am keen to` / `i am studying drawing` → il circuito, non lo smalltalk | ✅ chiuso (S1) |
| resa con `and` non più mutilata, e niente fatto falso in KB | ✅ chiuso (S2) |
| una seconda lezione sulla stessa mossa **sostituisce** | ✅ chiuso (S5) |
| `dialogue_emitted` non registra una proposta senza parole | ◐ metà (S4) |
| precedenza a chi ha un piano insegnato | ✅ chiuso (S6) |
| F1 (il turno come contenuto con un atto) | ✗ non iniziato |

**Stato del lavoro:** tutto nel working tree, **nessun commit e nessun push**.
Nove file toccati: quattro KB, tre C, due documenti.

| file | che cosa |
|---|---|
| `kb/core/turn-frames.p0` | `turn_opens_act/1` — chi apre un atto non porta prosa (S1) |
| `kb/core/dialogue-initiative.p0` | `dialogue_opening_at/3`, `dialogue_payload_trim`, `dialogue_reply_text/3`, `dialogue_direct_plan/1` (S1, S3, S4, S6) |
| `kb/core/dialogue-initiative/forms.p0` | atto `together`, 16 aperture IT/EN (S3) |
| `kb/core/dialogue-initiative/teaching.p0` | ritira-prima-di-asserire su resa e bisogno (S5) |
| `src/brain/99-registry.c` | `decompose_undo` — una decomposizione abbandonata non lascia fatti (S2) |
| `src/kb.c`, `src/kb.h` | `kb_journal_start_scoped` — il giornale per origine |
| `src/brain/10-memory-knowledge.c` | due tracce `P0_FORM_TRACE` (nessuna conoscenza) |
| `docs/plans/the-rational-philosopher.md`, `LEARN_PROTOCOL.md` | questo referto |

### Come ripartire, in quest'ordine

```sh
make build && make test-engine          # ⛔ sempre entrambi prima di misurare
printf '%s\n' 'come stai' 'facciamo filosofia insieme' 'continua' \
  'ho solo dieci minuti' 'no grazie' 'continua' '/quit' | \
  PARROT0_SESSION= PARROT0_LANG=it PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0
python3 scripts/p0t-echo.py tests/probes/rational_philosopher.it.p0t
python3 scripts/p0t-echo.py tests/probes/rational_philosopher.en.p0t
make soft-test                          # 12-14s su 15 di budget: vedi il rischio sotto
```

### ⚠ Tre trappole che hanno fatto perdere tempo in questo giro

1. **`/save` scrive nell'albero KB tracciato.** Non produce un file di sessione:
   *routa le clausole* dentro `kb/core/**.p0` e `kb/machinery/transcripts.p0`,
   e `PARROT0_SESSION=` non lo devia. È il modo più rapido di vedere che cosa
   una lezione ha **davvero** asserito (`/save` + `git diff`) — ma si ripulisce
   **file per nome**. Un `git checkout -- kb/` si porta via anche le proprie
   modifiche: è successo, e sono state riscritte da zero.
2. **Mai `git stash` dentro un job in background** mentre si continua a
   modificare: il `pop` va in conflitto e lascia l'albero mezzo-baseline. Letto
   come regressione fantasma, ha prodotto due diagnosi sbagliate di fila. Per
   confrontare con la baseline si usa un **worktree**
   (`git worktree add --detach /tmp/p0base HEAD`), che ha il suo socket.
3. **Un `make test-engine` concorrente uccide il demone di un altro banco.**
   I `.p0t` in corso riportano `test-send: cannot reach engine` e «0 passed»,
   che si legge come rosso. Un banco alla volta.

### S1. `i am keen to` non era un difetto della lezione

La stessa risposta — «That sounds nice -- tell me more about it.» — arriva a
un'apertura **nativa** della stessa forma:

| turno | prima | dopo |
|---|---|---|
| `i am studying drawing` (nativa) | smalltalk | proposta sul tema `drawing` |
| `i am keen to draw` (dopo la lezione) | smalltalk | proposta sul tema `draw` |
| `sono indeciso tra leggere e passeggiare` | «Non so ancora tradurre «indeciso»» | proposta sulla scelta |

La catena, letta con `/debug`: la copula fa dichiarare `prose_carried`; il
piano di turno **cede la prosa** (`faculty_yield_force(turn_plan, open,
prose_carried)`); il turno arriva intero allo smalltalk di ultima istanza.
`keen` non c'entrava, e nemmeno la lezione: era un **confine di arbitrato**.

**Cura:** `turn_opens_act/1` in `turn-frames.p0`, accanto alle eccezioni U2 ed
E3 che esistevano già — *chi dichiara di aprire un atto non porta un paragrafo
da leggere*. Il gancio non nomina nessun atto; lo riempie
`dialogue-initiative.p0` con `dialogue_opening_at/3` (l'atto senza pagare la
costruzione della coda). **Un'apertura insegnata parlando eredita l'eccezione
senza una riga in più.**

**La lezione generale, da aggiungere al §F:** quando una forma insegnata non
cambia niente, provare la stessa forma **nativa** prima di cercare il difetto
nella lezione. Se fallisce anche quella, il difetto non è nell'insegnabilità.

### S2. La resa tagliata su `and` non era «and»

`decompose_and_dispatch` taglia su « and » quando la seconda metà apre con un
verbo di richiesta, manda la **prima metà al registro per davvero** — la forma
di lezione la legge e asserisce la resa **troncata** — poi la seconda metà non
viene rivendicata, la decomposizione **si annulla** (`return 0`) e il turno
intero viene dispatchato di nuovo, stavolta con la resa giusta.

```
[form] op assert dialogue_move_text/3 <… careful_beginning en alpha>
[form] op assert dialogue_move_text/3 <… careful_beginning en "alpha and explain what is unclear">
```

La risposta è corretta e in KB restano **due** lezioni, di cui una falsa — ed è
quella che poi parla. Non è un difetto di `and`: **una lettura abbandonata
aveva comunque scritto.** Vale per qualunque modulo con un effetto — un fatto
personale, una policy, una lezione — dietro ogni split che non regge.

**Cura (meccanica, non conoscenza):** una decomposizione è un'**ipotesi** sul
turno; finché non è accettata i suoi effetti non sono del mondo. Il giornale
della KB dice che cosa è entrato di nuovo e l'annullamento lo ritira.

⚠ **Primo tentativo sbagliato, da non rifare:** giornale non filtrato. Registrava
le migliaia di pubblicazioni riflessive e di scratch di ogni turno —
`soft-test` a **16s, fuori budget** — e l'annullamento avrebbe ritirato anche
fatti che non erano suoi. `kb_journal_start_scoped(KB_SESSION | KB_BASE)`:
si registra solo ciò che la lettura ha **insegnato**, e il giornale torna di
poche righe invece di migliaia.

### S3. «facciamo filosofia insieme» era una famiglia mancante

Non un sinonimo di un atto già coperto: `first_step` chiede di prendere
l'iniziativa e non nomina niente, `curiosity` dichiara un interesse di chi
parla. Qui l'interlocutore **nomina un tema e propone di lavorarci in due**.
Nuovo atto `together` (16 aperture IT/EN), `dialogue_task(together, inquiry)`:
la prima mossa è `specify_uncertainty`, che è esattamente il §3 di questo piano
— *quando manca ciò da cui la risposta dipende, si nomina la dipendenza*.

E un avverbio dell'atto non è il tema: `dialogue_payload_trim` tiene «insieme»
fuori dal complemento («Su filosofia», non «Su filosofia insieme»). Verificato
con ablazione: `quando dico affrontiamo intendo proporre di occuparsene
insieme` cambia condotta, il ritiro la toglie.

### S4. §D.4 — si registra ciò che ha delle parole

Il contabile registrava la **candidatura**, non la resa: una mossa senza parole
nella lingua del turno lasciava `dialogue_emitted` dietro di sé, e «continua»
proseguiva da un passo mai pronunciato. Resa e registrazione sono ora **un
oggetto solo** (`dialogue_reply_text/3`): chi parla e chi registra leggono la
stessa cosa, quindi non possono divergere.

**Non chiude tutto il §D.4.** Tolta la falsa memoria da *resa mancante*, resta
quella da *turno perso*: che il circuito abbia davvero parlato lo sa l'arbitrato,
e il contabile corre prima della risposta. L'osservabile comune (`turn_done`)
è il passo successivo.

### S5. Dirlo due volte è correggersi, non aggiungere

Reperto della sonda `rational_philosopher.en.p0t`: insegnata prima «say Show one
example.» e poi «say Show one example and explain what is unclear.», la mossa
continuava a dire la **prima**. Due rese vive per la stessa mossa nella stessa
lingua, e parlava quella entrata prima: **la lezione più recente non aveva
effetto e nulla lo diceva**. Una resa e una ragione sono a valore unico, quindi
la forma di lezione ora ritira prima di asserire (due `turn_form_act` in
sequenza, tutto in KB). Per un'**apertura** il conflitto resta invece visibile
(`dialogue_ambiguous`): lì due letture dello stesso turno sono un'ambiguità
vera, non una correzione.

### S6. Aprire un confine sposta il turno a chi lo prendeva prima

Conseguenza immediata di S1, misurata su `user_situations.p0t` (49/7 rossi):
`I have a problem with my bike.` è un'apertura `problem` **vera**, e appena il
piano di turno smette di cedere la prosa il circuito se la prende — ma per
quella situazione qualcuno ha già **imparato** che cosa fare. I quattro passi
per attività sono un bootstrap, non una risposta migliore di una lezione
ricevuta: `dialogue_excluded` ora cede a chi ha il piano (56/56).

⚠ **La vista giusta è `direct_situation/2`, non `turn_has_situation_plan/1`.**
La seconda è vera anche quando il turno non nomina nessun guaio — le basta una
questione di ricerca aperta — e con un'issue aperta **ogni seguito** le
apparterrebbe: «facciamo filosofia insieme» al secondo turno tornava al muro.
La precedenza si merita nominando la situazione, non essendo il turno dopo.

**Vale come regola per chi apre il prossimo confine:** ogni `naf(...)` tolto da
una forza restituisce turni a una facoltà, e alcuni di quei turni avevano già
una risposta migliore. Si misura *prima* quali file cambiano, non dopo.

### Misure di questo giro

| prova | esito |
|---|---|
| `make soft-test` | **verde 5 volte su 5, 12-14s** su 15 di budget (baseline 11-12s) — vedi il rischio dichiarato sotto |
| `tests/probes/rational_philosopher.en.p0t` | i **due controesempi** del §D.1 sono verdi |
| `tests/probes/rational_philosopher.it.p0t` | l'intero circuito IT: proposta, `perché`, `continua`, vincolo, rifiuto, non-ripresa |
| `tests/p0t/meta/decompose.p0t` | 6 passed |
| `tests/p0t/conversation/move_precedence.p0t` | 9 passed |
| `tests/p0t/conversation/user_situations.p0t` | 56 passed (49/7 prima di S6) |
| `dialogue_moves` / `smalltalk` / `turn_thefts` | 1 rosso ciascuno — **identici sulla baseline**, non di questo giro |
| `continuation.p0t` (0/5), `compound_inquiry.p0t` (30/1) | rossi **identici sulla baseline** |
| suite intera | **non eseguita** (serve l'approvazione di F.) |

### Rossi misurati e NON toccati — non rimuoverli per far quadrare il referto

1. **La lingua salta a metà scambio.** `per me conta riposare` dentro uno
   scambio italiano risponde «You added: riposare. On leggere e passeggiare:
   Use this criterion…». È il rilevatore di lingua sul turno corto, non il
   circuito — **pre-esistente**, identico sulla baseline. Ma colpisce l'asse
   «continuità dopo un seguito»: una proposta che cambia lingua a metà l'ha già
   persa. La leva: la lingua dell'**ancora** è già registrata
   (`dialogue_language/2`), quindi la resa potrebbe leggerla invece di
   `current_language/1`. Non fatto qui: è un quinto problema strutturale e il
   §D dice uno per volta.
2. **Il circuito ruba un turno che aveva una risposta onesta.**
   `turn_thefts.p0t`: «what do you think about artificial intelligence» riceve
   la proposta di `reflect` invece del «non ho opinioni mie». Rosso **già
   prima** di questo giro. È esattamente la riga «il problema si sposta a
   un'altra facoltà» del §F: si classifica con il mantra #21 e si migra la
   specie, non si mette una cue di cessione per il caso.
3. **Una citazione non è ancora esclusa.** `"vorrei imparare a dipingere"` fra
   virgolette viene letta come intenzione di chi parla: `dialogue_quotation_cue`
   non scatta sul virgolettato semplice.
4. **Il discorso riportato produce un fatto spazzatura.** `maria dice che vorrei
   imparare a dipingere` → «Learned: maria dice say vorrei imparare.»
5. **Preposizione articolata.** «Su **il** tempo», «Su **la** filosofia»:
   difetto di presentazione dell'ancora italiana, non di lettura.
6. **F1 non è costruito.** I turni 2 e 3 dell'esempio del §1 rispondono come
   prima: «Non capisco ancora.» e «c, python.». Il §8 resta valido.

### ⚠ Rischio dichiarato: il budget di `soft-test`

`soft-test` sta a **12-14s** contro un budget di **15s** (baseline 11-12s).
Verde 5 volte su 5 a macchina scarica, ma durante questo giro ha toccato
**16s e fallito** mentre altri banchi giravano. Il margine è sottile e la
politica è chiara: *il budget non si alza, si cura la lentezza o si tolgono
casi*.

**Non è stato possibile attribuire il delta a una modifica sola:** togliendo
`naf(turn_opens_act($Turn))` dalle due regole di `prose_carried` la misura
resta 12-15s, cioè dentro la dispersione della macchina (±3s fra run
identiche). Le leve da provare, in ordine di sospetto:

1. `dialogue_reply_text/3` è derivata **due volte** per turno — una dal
   contabile (S4), una da `turn_priority_response`. Un osservabile pubblicato
   una volta sola la pagherebbe una volta.
2. Le 16 aperture nuove entrano nella ricerca di sottostringa che
   `turn_publish_cues` fa su **ogni** turno, su quattro viste del testo.
3. `dialogue_payload_last/2` aggiunge un `naf` per parola nella ricorsione
   della coda (solo sui turni che aprono un atto).

**Prima di ottimizzare, misurare per file** con `/usr/bin/time` sui tre `.p0t`
del soft-test e su un worktree di baseline: senza quel confronto si ottimizza
il rumore.

### Da dove riprendere

Non dal §D.1 (chiuso) né dal §D.4 (chiuso a metà, il residuo è scritto sopra).
Tre candidati, in ordine di rapporto valore/rischio:

1. **Il rischio del budget qui sopra**, se la prossima sessione deve toccare
   ancora questo circuito: lavorare sopra un banco che fallisce a intermittenza
   rende ogni misura successiva discutibile. È il lavoro meno creativo e il più
   abilitante.
2. **Il rosso 1** (la lingua che salta a metà scambio): piccolo, misurato, su
   un asse dichiarato («continuità dopo un seguito»), e la leva esiste già —
   la lingua dell'ancora è registrata in `dialogue_language/2`.
3. **Il §D.3** — la semantica degli aggiornamenti, dove `correction` non
   ricostruisce ancora il tema. È il prossimo problema strutturale vero.

Il **§D.2** (sonde → regressioni con attese semantiche) resta aperto e diventa
ogni giro più caro: il driver `p0t-echo.py` **non verifica attese**, quindi i
verdi di questo referto si rileggono a mano e nessuno se ne accorge se
regrediscono. Chiunque prenda il punto 1 dovrebbe valutare di chiudere prima
questo, perché rende verificabile tutto il resto.

**E per chi arriva da un modello meno potente:** i sei reperti sotto sono
scritti come *catene causali misurate*, non come conclusioni. Il valore non è
nella patch ma nel metodo che l'ha trovata — le righe nuove della tabella §F
(«le domande da porsi quando ci si blocca») sono il precipitato riusabile di
questo giro. Leggerle prima di aprire il codice.

---

## ⤶ Riprende il PRIMO GIRO (storico) — §A-§G

> Da qui in poi è il referto del primo giro. Vale ancora per intero **tranne
> il §D.1 e il §D.4**, rettificati dall'handoff sopra. Le righe rettificate
> portano un rimando in linea.

### A. Che cosa è stato costruito e dove

| file | responsabilità |
|---|---|
| `kb/core/dialogue-initiative.p0` | lettura dei token della IR condivisa; candidati; confronto delle letture; atti citati; contesti di lavoro; proposta, continuazione, aggiornamento, rifiuto e spiegazione |
| `kb/core/dialogue-initiative/forms.p0` | **279 aperture, 21 atti**; forma intera o con complemento; nomi pronunciabili degli atti; interpretazione dei seguiti |
| `kb/core/dialogue-initiative/moves.p0` | **10 itinerari, 40 mosse** con bisogno dichiarato e resa EN/IT; il metodo è proposto, non spacciato per un risultato ottenuto |
| `kb/core/dialogue-initiative/teaching.p0` | 16 forme di lezione/ritiro: apertura → atto; nome della mossa → bisogno; mossa → parole; attività → prima mossa |
| `kb/core/procedures.p0` | include del circuito dopo `discourse.p0` |
| `tests/probes/rational_philosopher.it.p0t` | transcript diagnostico: aperture, continuità, vincoli, motivazione, arresto, controlli e caso storico F1 |
| `tests/probes/rational_philosopher.en.p0t` | transcript diagnostico: crescita per lezione, trasferimento, ritiro, policy e controesempi |
| `MANTRA.md`, #26 | una soluzione acquista valore se amplia ciò che si può insegnare; replay e ritrattazione restano obbligatori |
| `LEARN_PROTOCOL.md`, aggiornamento sul dialogo | forme nuove con grado di verifica e limiti, non promessa indiscriminata di apprendimento |

Sono dati di metodo e un nuovo consumatore KB delle strutture comuni: **zero
modifiche a `src/`**. La quantità di aperture misura il vocabolario, non il
grado di comprensione. Non promuovere queste cifre a punteggio cognitivo.

### B. Misure e limiti: che cosa sappiamo davvero

Baseline fresca, profilo `agi`, sessione vuota ma **KB completa**:
«vorrei imparare a dipingere», «non so da dove cominciare», «mi interessa il
tempo» ricevevano muri. Dopo il primo circuito:

| prova | risultato osservato |
|---|---|
| `vorrei imparare a dipingere` | proposta di partire da un esempio e dal livello già posseduto; tema conservato come `a dipingere` |
| `continua` | passa all'esercizio minimo sullo stesso tema |
| `no grazie` → `continua` | interrompe la proposta; il seguito non la riattiva di nascosto e chiede un'ancora |
| `i want to learn to paint` | proposta di un esempio concreto, dopo la correzione del confronto delle letture descritta sotto |
| insegnare una mossa e usarla come prima mossa dell'apprendimento | il turno successivo usa la nuova resa; `why` nomina il bisogno insegnato; ritirando la policy torna la proposta di base |
| insegnare `let us study` come apertura di apprendimento | il replay `let us study pottery` entra in `turn_plan`; dopo il ritiro questa lettura sparisce |
| dopo il ritiro di `let us study` | il percorso legacy può dire `Learned: let study pottery.`: la perdita della forma è provata, **l'innocuità del fallback no** |
| insegnare `i am keen to` con lo stesso atto | lettura, `dialogue_seen`, candidatura e `turn_priority_response` sono corrette; quest'ultima restituisce `On draw: Let us use one concrete example…`, ma il turno reale è risposto da `smalltalk`: **difetto fra decisione e arbitrato**, non una forma ancora da riconoscere |
| insegnare una resa con `and` | `Show one example and explain what is unclear` viene resa come `show one example`: **perdita della seconda parte da diagnosticare** |
| `make soft-test` dopo le aggiunte | **verde in 11 s**, budget 15 s; suite completa non eseguita |

Il ciclo italiano è stato osservato prima della modifica che separa `update`
da `advance`: il difetto scoperto era che «ho solo dieci minuti» avanzava
come se il lavoro fosse già stato svolto. La separazione e la memoria del
dettaglio ora esistono nel codice KB; **il replay finale dell'intero circuito
è una verifica da completare**, non una misura implicita nella modifica.
Le sonde versionate raccolgono anche i casi ancora rossi. Non sono golden e
il driver `p0t-echo.py` non verifica automaticamente attese.

### C. I comandi di ripartenza, in quest'ordine

```sh
git status --short
git diff -- MANTRA.md LEARN_PROTOCOL.md kb/core/procedures.p0
LANGX=it python3 scripts/p0t-echo.py tests/probes/rational_philosopher.it.p0t
python3 scripts/p0t-echo.py tests/probes/rational_philosopher.en.p0t
rg 'PARSE ERROR' logs/p0t-echo/trace-rational_philosopher*
```

I file nuovi non compaiono nel normale `git diff`: aprirli direttamente.
Il driver avvia un motore fresco con l'intero profilo e conserva transcript e
stderr in `logs/p0t-echo/`. Leggere le risposte intere. Eseguire una sonda alla
volta: non misurare latenza mentre un altro banco lavora. **Non rilanciare
subito la suite:** `soft-test` è già verde; ripeterlo dopo cambiamenti che lo
giustificano. Non alzare budget e non togliere conoscenza.

### D. Ordine di lavoro obbligatorio: un problema strutturale alla volta

1. **Rendere affidabile il giro insegnamento → lettura → scelta → resa.**
   ✅ **CHIUSO nel secondo giro — vedi S1 e S2.** La diagnosi qui sotto era
   sbagliata in entrambi i casi: `i am keen to` non era un difetto della
   lezione (falliva anche la forma nativa) e la resa tagliata non era «and».
   Il testo resta come esempio di *diagnosi plausibile e falsa*.
   Partire dalle due lesioni concrete: `i am keen to` e la resa tagliata su
   `and`. Confrontare il valore appena insegnato, `dialogue_raw`,
   `dialogue_seen`, `dialogue_candidate`, `turn_priority_response` e
   `turn_module`. Annotare il primo confine dove il dato o la decisione cambia.
   Non aggiungere un'eccezione per `keen` o una frase senza congiunzioni.
2. **Trasformare le sonde in regressioni con attese semantiche.** Per ogni
   nuova forma: prima, lezione naturale, stesso turno, altro argomento,
   ritiro, stesso turno. Per una resa: conservare entrambe le parti. Per una
   policy: mostrare la mossa selezionata e il bisogno, non solo l'acknowledgement.
   Tenere separato il difetto legacy dopo ablazione dall'effetto della lezione.
3. **Correggere la semantica degli aggiornamenti.** Un vincolo restringe una
   proposta, un criterio cambia il confronto, un'osservazione porta evidenza,
   una correzione può cambiare il referente: non sono quattro modi di dire
   «avanti». Oggi `dialogue_detail` conserva il dato e `dialogue_update_move`
   sceglie una mossa distinta, ma il vincolo non governa ancora tutti i passi
   successivi e `correction` non ricostruisce il tema. Chiudere questo prima
   di aggiungere altre aperture.
4. **Collegare la memoria al tabellone unico.**
   ◐ **META' CHIUSO nel secondo giro — vedi S4.** La falsa memoria da *resa
   mancante* è tolta; resta quella da *turno perso*, e il collegamento a
   `open_issue`/`issue_turn` non è iniziato. Il caso `i am keen to draw`
   citato qui sotto non è più riproducibile: quel turno ora è servito dal
   circuito. `dialogue_scope` e
   `dialogue_emitted` sono un primo registro del circuito; non inventare un
   secondo `pending_*`. Farne contenuti/atti riferiti da `open_issue`,
   `issue_turn`, `issue_topic` e viste di stato. Aprire la questione per una
   proposta effettivamente emessa, non soltanto candidata. Oggi il contabile
   registra la candidatura prima della resa: una resa fallita può lasciare
   una falsa memoria di proposta. **Misurato proprio su `i am keen to draw`:**
   `dialogue_emitted(2, …)` esiste pur avendo parlato `smalltalk`. Non è un
   rischio solo teorico: riparare la registrazione dell'emissione insieme al
   confine di arbitrato. Questo è debito esplicito.
5. **Sostituire l'avanzamento numerico con progresso verificato.** I quattro
   passi per attività sono bootstrap di procedure insegnabili. `continua`
   autorizza a spiegare il passo successivo; **non prova** che il precedente
   sia stato eseguito. Un esito deve venire da un atto o da un'osservazione.
   Collegare `dialogue_move_need` a prerequisiti, dati disponibili e residui:
   prima riusare `frame_residue`, `dialogue-policy`, `situation` e `inquiry`.
6. **Solo sul circuito affidabile, espandere la composizione.** Due intenzioni
   nello stesso turno, interruzione e ripresa, contesti annidati, esempi citati,
   dissenso e correzione della policy. Ogni espansione deve aprire subito una
   lezione pronunciabile e aggiornare il catalogo del protocollo.

**Gate di consegna:** un turno non interrogativo produce un passo motivato;
il seguito conserva l'ancora; un dato nuovo cambia il passo pertinente; un
rifiuto lo ferma; la motivazione rimanda all'atto e al bisogno effettivi;
la lezione cambia condotta e il ritiro ne toglie l'effetto. Dichiarare i casi
non letti e quelli non eseguibili. Non dire «qualunque prompt è supportato».

### E. Insight verificati e leve per il prossimo passaggio

**1. La difficoltà non era solo leggere una domanda: mancava l'oggetto
«proposta che attende un seguito».** Una risposta e una proposta producono
obblighi conversazionali diversi. Il passo proposto deve avere identità,
ancora, scopo, stato e modalità di ripresa. Altrimenti «sì» e «continua» sono
token isolati e l'iniziativa diventa un nuovo frasario.

**2. Separare la lettura dal confronto delle letture ha sbloccato l'inglese.**
La prima versione faceva `naf(dialogue_ambiguous(...))` sopra regole che
rileggevano ricorsivamente tutti i prefissi e i payload. `dialogue_best`
trovava la lettura, ma `dialogue_reading` falliva: la negazione non riusciva a
concludere entro i limiti del solver. Dopo la pubblicazione di
`dialogue_raw(N, Act, payload(Text, End))`, il confronto delle alternative è
finito e poco profondo; `i want to learn to paint` risponde. **Lezione generale:
non negare una nuova esecuzione di un parser per dire che una sua alternativa
manca. Pubblicare le evidenze una volta, poi ragionare sulle evidenze.**

**3. Il ponte più fertile è «nome pronunciabile → ruolo → procedura».**
La lezione non deve nominare `dialogue_move_need`. Dice «the conversational
move careful beginning addresses a concrete example». Una seconda lezione
ne insegna le parole, una terza quando usarla. Questo rende correggibile la
condotta invece di accumulare risposte. Un nuovo predicato senza una forma
di insegnamento è un cassetto senza maniglia; mantra #26.

**4. Una proposta razionale non è necessariamente una conclusione dedotta.**
La prova di un fatto e la giustificazione di un'azione sono oggetti diversi.
«Propongo di confrontare due esempi perché manca il criterio» richiede una
ragione operativa, non la prova che una delle opzioni sia vera. La distinzione
permette iniziativa onesta senza fingere di avere opinioni o dati mancanti.
Oggi `why` espone il bisogno della mossa: non è ancora un certificato della
correttezza di tutte le premesse o dell'utilità futura della proposta.

**5. Il tema non è la verità del tema.** Il complemento di «supponiamo che…»
o «mi interessa…» viene conservato come contenuto attribuito. Non chiamare
`assert(P)` sul contenuto per poterne discutere. La stessa distinzione deve
reggere desideri, citazioni, vissuti, piani, finzioni e negazioni.

**6. Massimizzare gli ingressi prima di stabilizzare gli effetti è pericoloso.**
279 aperture moltiplicano l'uso delle stesse poche procedure, ma moltiplicano
anche un errore nella gestione di un vincolo o di un rifiuto. Ora investire
nelle distinzioni e nella continuità, non in un'altra lista di sinonimi.

### F. Domande da porsi quando ci si blocca

| sintomo | prima domanda | esperimento discriminante |
|---|---|---|
| dice «imparato» ma nulla cambia | la lezione è entrata? chi legge quel predicato? | leggere il fatto; replay identico; osservare lettura, candidatura e vincitore |
| la vista positiva funziona, la sua negazione no | è assenza o ricerca incompleta? | interrogare separatamente gli ausiliari; pubblicare i candidati prima del confronto; non aumentare il budget |
| una frase più lunga rompe il caso breve | quale informazione si perde, e a quale confine? | confrontare payload, clausole della lezione, resa registrata e testo emesso |
| il problema si sposta a un'altra facoltà | la comprensione è condivisa o locale? | leggere `turn_module`; classificare il concorrente con mantra #21; migrare la specie, non mettere una cue di cessione per il caso |
| la replica parla ancora del vecchio tema | la correzione cambia contenuto, atto o contesto? | due temi reali; correzione esplicita; `continua`; verificare quale identità è rimasta attiva |
| un «sì» scatena qualcosa di inatteso | a quale proposta e a quale forza si è legato? | interporre un turno estraneo, un rifiuto o una proposta concorrente; mai trattare l'assenso come autorizzazione globale |
| aggiungere conoscenza peggiora le risposte | viene scelta una lettura o solo la prima? | inserire una lettura concorrente e invertire l'ordine; il conflitto deve restare interrogabile |
| una forma INSEGNATA non cambia niente | la stessa forma **nativa** funziona? | provarla; se fallisce anche quella il difetto non e' nell'insegnabilita' ma in chi prende il turno (S1) |
| la risposta e' giusta e la KB e' sbagliata | qualche lettura ha scritto ed e' stata **abbandonata**? | `P0_FORM_TRACE=1` e leggere gli `op assert`: due righe per la stessa relazione sono uno split che non ha retto (S2) |
| la seconda lezione non ha effetto | la prima e' ancora viva? | `/save` e contare i fatti: una relazione a valore unico deve **sostituire**, non accumulare (S5) |
| apro un confine e altri file diventano rossi | quali turni ho restituito, e a chi li stavo togliendo? | misurare i file che cambiano PRIMA; cedere a chi ha una lezione piu' specifica, con la vista piu' stretta che regge (S6) |
| sembra tutto corretto ma è generico | quale dato dell'utente cambia davvero la decisione? | mantenere le parole quasi uguali e cambiare vincolo, criterio o commitment; la mossa deve cambiare per quella ragione |

### G. Ipotesi da sperimentare, non capacità già consegnate

- **Una mossa come procedura con prerequisiti ed effetti.** Rappresentare
  `mossa → bisogno → dato richiesto → effetto atteso`, usando procedure e
  inferenza comuni. L'assenza di un ruolo produce il residuo; il residuo
  propone una domanda o una prova discriminante. Confrontare con la sequenza
  numerica attuale su input dove il dato è già presente: deve evitare domande
  ridondanti. Non aggiungere un planner C per la conversazione.
- **Un atto pragmatico può avere più letture senza decidere subito.** «Sono
  stanco» può informare, cercare ascolto o chiedere aiuto. Conservare candidate
  con evidenza e commitment; preferire un passo reversibile che non richieda
  di sceglierne una arbitrariamente. Prova: ritiro/correzione dell'intenzione
  nel turno dopo, senza perdita del vissuto già detto.
- **Il progresso è un cambiamento dello stato epistemico o pratico.**
  Distinguere dato acquisito, opzione eliminata, ipotesi riformulata, azione
  tentata ed esito osservato. Un contatore di turni non misura nessuna di
  queste cose. Prova: ripetere «continua» non deve certificare lavoro eseguito.
- **Contesti come scopi e assunti, non etichette di dominio.** Apprendere,
  negoziare e creare possono coesistere sullo stesso contenuto. Le dieci
  attività bootstrap non sono dieci menti. Prova: un racconto usato per
  esplorare una tesi etica, con la finzione mantenuta distinta dagli assunti
  personali e dai fatti del mondo.
- **La ragione della proposta deve sopravvivere alla resa.** Un oggetto di
  mossa scelto dalla KB dovrebbe essere lo stesso che il contabile registra
  come emesso. Cercare prima un osservabile comune già presente in
  `turn_done`; un eventuale gancio C deve pubblicare l'esito, senza sapere
  quali attività, parole o mosse esistano.

## Il framework da costruire: ogni contesto umano, senza fingere onniscienza

La generalità richiesta è del **contratto**, non di una lista esaustiva di
argomenti. Il soggetto resta una KB unica. Per ogni turno distinguere:

```text
espressione letta → letture candidate → atto attribuito → contesto e commitment
                  → obiettivo / bisogno / vincoli / evidenze
                  → mosse ammissibili → proposta motivata → uptake → revisione
```

Questi collegamenti non cancellano gli oggetti a sinistra. La lingua originale,
la fonte, le alternative scartate e gli assunti restano consultabili. Riutilizzare
IR, `context-scope`, contenuti e derivazioni del mondo allargato. L'introspezione
di una clausola già asserita non è, da sola, la capacità di introdurre una
citazione senza crederla; `kb_act` per livello non equivale a un'identità per
ogni fonte e ogni atto conversazionale. Verificare questi ponti prima di
prometterli, anche se una API porta il nome giusto.

**Famiglie da coprire con la stessa struttura:** domanda chiusa e aperta;
affermazione; desiderio; progetto incompleto; richiesta indiretta; dubbio;
vissuto; contrasto di valori; disaccordo; vincolo; preferenza; osservazione;
esempio; citazione; ipotesi; controesempio; correzione; rifiuto; ripresa;
creazione e valutazione di un artefatto; resoconto di un'azione; richiesta di
iniziativa. Un turno può contenere più famiglie. Un nuovo membro deve costare
conoscenza e una lezione, non un recognizer C.

**Confini della prima versione:** la lettura è ancorata all'inizio del turno;
citazioni e più frasi vengono escluse, non comprese; l'uptake richiede la
proposta del turno immediatamente precedente; mancano ripresa dopo digressione,
vincoli compositivi, revisione piena del tema e collegamento operativo al
tabellone. I bisogni sono dichiarati dalle procedure: non sono ancora ricavati
da una ricerca generale delle premesse mancanti. Questi limiti costituiscono
il banco successivo, non scelte da congelare come architettura definitiva.

**Misura su quattro assi, senza media che nasconda un danno:** pertinenza
dell'ancora; fedeltà a contenuto e commitment; utilità e rivedibilità della
mossa; continuità dopo un seguito. Aggiungere costo e crescita per lezione.
Una mossa pertinente ma infondata non è razionale; una domanda di ritorno
sempre uguale non è iniziativa; una risposta corretta senza un seguito
interpretabile non chiude questa capacità.

---

## Piano iniziale e baseline storica — da leggere con le rettifiche sopra

> **Richiesta di F., 20 settembre 2026.** «Sempre usando la KB, la IR e la
> concezione del mondo allargato espressa in
> [`lettura-della-prosa.md`](lettura-della-prosa.md), costruiamo un piano
> dedicato all'abilità di gestire conversazioni che spaziano nello **spazio
> logico dell'interlocutore** ma che rimangono **razionali**. L'esempio è
> troppo piccolo: concentrarsi sul **carattere della domanda intermedia** fatta
> al secondo turno, e sul concetto di spazio logico. Devono essere domande
> aperte, anche generiche, anche di carattere filosofico — tipo *cosa pensi
> della vita*.»

---

## 1. L'esempio, esteso e misurato

Riprodotto il 20 settembre 2026, profilo `agi`, `PARROT0_LANG=it`. I primi tre
turni sono quelli di F.; gli ultimi due li ho aggiunti per allargare il campione
al registro filosofico che il piano deve reggere.

| # | turno | risposta di oggi | specie del guasto |
|---|---|---|---|
| 1 | «sai parlare italiano» | «Sì, parlo italiano.» | ✅ nessuno |
| 2 | «quali parole conosci che possono sorprendermi» | «Non capisco ancora.» | **muro su una domanda che andava aperta** |
| 3 | «in quale lingua ti ho chiesto quale lingua sai parlare» | «c, python.» | **ancora persa + senso sbagliato, detto con sicurezza** |
| 4 | «cosa pensi della vita?» | «Non so ancora tradurre «pensi».» | **apertura filosofica letta come lacuna lessicale** |
| 5 | «secondo te il tempo esiste davvero?» | «Non ne ho di miei — sono parrot0, un'IA — ma raccontami dei tuoi.» | **posizione nello spazio logico letta come possesso di opinioni** |

**Non sono cinque bug: sono cinque modi di uscire dallo spazio.** Il turno 3 è
il peggiore di tutti sulla scala di F., perché non è un muro ma una risposta
confidente e sbagliata: «lingua» è stata collassata su *linguaggio di
programmazione* e il referente — *un turno di questa conversazione* — è sparito.
L'ispettore lo conferma: `turn_illocution` legge `question`, e
`debug_frame_record`, `turn_focus`, `max_qud` e `topic_read` sono tutti
**niente**. La domanda è riconosciuta come domanda e non è letta come nulla.

---

## 2. Che cos'è lo spazio logico, operativamente

Non è una metafora, ed è già quasi tutto rappresentabile con il mondo allargato
di [`lettura-della-prosa.md`](lettura-della-prosa.md) §0 — **contenuto, atto,
contesto, giudizio, derivazione**.

> **Lo spazio logico dell'interlocutore è l'insieme dei contenuti che possono
> essere coerentemente intrattenuti, dato ciò che lo scambio ha stabilito:**
> ciò che l'altro ha asserito, ciò che ha supposto, ciò che ha presupposto, e
> ciò che ne segue.

Una mossa sta **nello spazio** se il suo contenuto è collegato ad almeno un
**atto** dello scambio. È **razionale** se il suo **giudizio** porta con sé la
**derivazione** e gli assunti su cui poggia. Le due proprietà sono indipendenti,
e i quattro quadranti sono esattamente i modi di sbagliare:

| | nello spazio | fuori dallo spazio |
|---|---|---|
| **razionale** | ✅ la mossa da costruire | il muro corretto ma irrilevante — turno 2 |
| **non razionale** | fluente e insostenibile: il rischio LLM | «c, python» — turno 3, il caso peggiore |

**Perché questo piano non è «fare filosofia».** Una risposta bella su *cosa sia
la vita* non vale niente se non si sa da che cosa viene: sarebbe il frasario
della prosa, misurato al 19% in `lettura-della-prosa.md`, trapiantato nel
registro filosofico. Il criterio resta quello di F.: **da che cosa viene la
risposta**, non quanto suona bene.

---

## 3. La domanda intermedia, e perché è l'unità dell'abilità

F. indica il secondo turno. Ha ragione, e vale la pena dire esattamente perché.

> «quali parole conosci che possono sorprendermi»

Questa domanda ha tre proprietà, e **tutte e tre** la distinguono da una
domanda di conoscenza:

1. **È aperta:** non esiste *la* risposta giusta, esiste una risposta
   giustificabile.
2. **È indicizzata all'altro:** «sorprendente» non è una proprietà delle
   parole, è una relazione fra una parola e **chi ascolta**. La risposta
   dipende da un modello dell'interlocutore che nello scambio **non è stato
   stabilito**.
3. **È intermedia:** non chiude un argomento, lo **apre**. Il suo valore non è
   nella risposta ma nel movimento che provoca.

Ne segue il comportamento corretto, ed è **né il muro né l'elenco**:

> **Quando la risposta dipende da qualcosa che lo scambio non ha stabilito, la
> mossa razionale è nominare quella dipendenza** — e poi, a scelta dichiarata,
> chiedere («che cosa sai già?») oppure assumere esplicitamente («assumo che
> tu non conosca il lessico tecnico: allora…»).

Nominare la dipendenza **è** una mossa nello spazio logico: restringe lo spazio
invece di saltarne fuori. Un muro non lo restringe; un elenco confidente lo
abbandona. Questa è l'abilità che il piano deve costruire, e il resto sono
conseguenze.

---

## 4. Che cosa esiste già, e non va rifatto

Il mondo allargato è stato costruito il 20 settembre 2026 per la prosa. È la
stessa astrazione che serve qui, e **è eseguibile**:

| porta | che cosa dà a questo piano |
|---|---|
| `kb_clause/4`, `kb_clause_arg/4` | un contenuto è un dato: si può **menzionare senza crederlo** — la condizione minima per intrattenere una posizione |
| `kb_act/3` + `act_layer/2` | **chi** ha fatto entrare un contenuto e a quale titolo: detto, supposto, derivato. È l'ancora di ogni mossa |
| `kb_derivation/4` | la prova con le sue dipendenze (AND), le alternative (OR), e `absent(G)` — «lo dico perché non trovo il contrario» è una mossa filosofica legittima **se dichiarata** |
| `supported_from_premises/1` | ciò che segue **dalle sue premesse** e non dal mondo: la differenza fra discutere una tesi e ripetere un fatto |
| `holds_in/2`, `context-scope.p0` | posizioni che convivono senza cancellarsi |
| `epistemic-status.p0` | i cinque stati: positivo, negativo, entrambi, nessuno, ricerca incompleta |
| `turn_illocution`, `faculty_force/2`, `faculty_yield_when/3` | la condotta è conoscenza: chi ha diritto di parlare, su quale forza, e quando cede a una lettura |
| `discourse.p0` (`previous_turn`, `turn_counter`, `turn_retained`) | il filo della conversazione, già parzialmente tenuto |
| `scripts/prose-why.sh`, `/debug` | l'ispettore **nello stato in cui sbaglia** — vedi la regola: una sonda che mostra una differenza trova i difetti da sola |

**Quello che manca non è l'astrazione: è che la conversazione stessa non è
ancora un oggetto di quell'astrazione.** Un turno dell'interlocutore non è un
contenuto con un atto. Per questo il turno 3 non trova la sua ancora.

---

## 5. Le facoltà da costruire, in ordine

Una per volta, ciascuna con la sua condizione di riuscita **misurabile**.

### F1 — Il turno è un contenuto, e il dire è un atto

Ogni turno dello scambio diventa un contenuto con la sua identità e il suo
atto: *chi* l'ha detto, *quando*, *in quale lingua*, *con quale forza*. La
lingua del turno è già osservata (`turn_language_observed/2`); manca che il
turno sia **nominabile** come oggetto e che un riferimento anaforico lo
raggiunga («ti ho chiesto…», «quando hai detto…», «la domanda di prima»).

**Riuscita:** il turno 3 dell'esempio risponde **«in italiano»**. E la prova
che non è un caso: «in che lingua ti ho risposto?» e «che cosa ti ho chiesto
per primo?» rispondono dallo stesso circuito, senza una riga per ciascuna.

**Non barare:** nessuna cue «in quale lingua». La domanda deve leggersi come
riferimento a un atto dello scambio, e la risposta venire da quell'atto.

### F2 — La dipendenza non stabilita si nomina

Quando la risposta dipende da un contenuto che lo scambio non ha stabilito, la
facoltà produce una **mossa** invece di un muro: nomina la dipendenza, e poi
chiede o assume **dichiarandolo**.

**Riuscita:** il turno 2 produce qualcosa come *«dipende da che cosa conosci
già: dimmelo, oppure assumo che… e allora…»*. Con la stessa regola, e senza
scriverne un'altra, «qual è il libro più bello?» e «che musica mi consigli?»
producono la stessa forma di mossa.

**Non barare:** la dipendenza deve essere **calcolata** dalla derivazione
mancante (`kb_derivation` che non trova una prova ammissibile e sa dire di che
cosa avrebbe avuto bisogno), non scelta da una tabella domanda→dipendenza.

### F3 — Una posizione si intrattiene senza asserirla

«cosa pensi della vita?» apre un **contesto**, non chiede un fatto. La risposta
è una posizione che parrot0 **intrattiene**: `holds_in` con un atto della forza
giusta, e la derivazione disponibile.

**Riuscita:** tre domande di controllo funzionano sulla stessa posizione —
«perché lo dici?» restituisce la derivazione; «lo credi davvero?» distingue
l'intrattenuto dall'asserito; «e se invece…?» apre un secondo contesto senza
cancellare il primo.

**Non barare:** vietato l'aforisma preconfezionato per argomento. Il banco
conta il **modulo** che risponde, come `scripts/coefficiente.sh`: una posizione
che viene dal frasario vale zero anche se è vera e bella.

### F4 — Gli assunti cadono insieme a ciò che reggono

Ritirare un assunto fa cadere le posizioni che lo richiedono e **lascia vive**
quelle che hanno un'altra via. È già la meccanica di M2/M3 della prosa: qui
diventa condotta di conversazione.

**Riuscita:** l'ablazione. Stabilito un assunto, derivata una posizione,
ritirato l'assunto: la posizione cade e lo **dice**; una posizione con doppio
sostegno resta e sa dire con quale.

### F5 — Il registro delle mosse

Una conversazione razionale non è un'interrogazione. Le mosse — **domanda di
ritorno, distinzione, controesempio, concessione, riformulazione** — sono
conoscenza con le loro condizioni d'uso, non rami nel C. Una mossa nuova deve
costare **una riga di KB**.

**Riuscita:** si insegna una mossa nuova parlando, si vede usarla nel turno
successivo, si ritira e sparisce. È il criterio del mantra #2 applicato al
discorso.

---

## 6. Il banco, e come si misura

Stesso disegno della scala di prosa, perché ha funzionato: **una colonna che
dice da dove viene la mossa**, e un numero che si legge invece di stimarlo.

Per ogni turno si registrano tre cose:

| colonna | domanda |
|---|---|
| **ancora** | a quale atto dello scambio si attacca questa mossa? (vuoto = fuori dallo spazio) |
| **sostegno** | quale derivazione la regge? (vuoto = non razionale) |
| **mossa** | risposta, domanda di ritorno, assunto dichiarato, distinzione, muro |

e il punteggio è **la frazione di turni che hanno ancora e sostegno**. Un muro
onesto non è un errore: è una mossa con ancora e senza sostegno, e va contato
per quello che è. Una risposta fluente senza sostegno è il caso peggiore e va
contata come tale — **peggio del muro**, come sulla scala di F.

**Il corpus:** aperture filosofiche vere e generiche — *cosa pensi della vita*,
*il tempo esiste*, *che cos'è giusto*, *si può sapere qualcosa con certezza* —
più le metaconversazionali dell'esempio. Devono essere **aperte**: una domanda
con una risposta sola misurerebbe la conoscenza, non questa abilità.

**Anti-inganno, dal piano della prosa:** il banco è fisso, non si allarga per
abbassare il tasso; le entità non si inventano per far passare un turno; e se
una mossa giusta viene dal frasario, il referto lo dice nella colonna del
modulo e quel turno **non** conta come compreso.

---

## 7. Le trappole prevedibili, scritte prima di caderci

- **Il frasario filosofico.** Trenta aforismi indicizzati per argomento
  passerebbero il banco e non sarebbero l'abilità. È lo stesso guasto misurato
  al 19% sulla prosa, e si riconosce con la stessa colonna.
- **La domanda di ritorno come scappatoia.** Rispondere sempre con una domanda
  sembra socratico ed è un muro travestito. La domanda di ritorno è legittima
  **solo** quando nomina la dipendenza che la rende necessaria.
- **Il relativismo di comodo.** «Dipende dai punti di vista» è fuori dallo
  spazio quanto «c, python»: non si attacca a nessun atto.
- **Confondere intrattenere con credere.** Una posizione detta e non creduta
  deve restare distinguibile a ogni turno successivo, altrimenti la
  conversazione accumula fatti falsi — ed è precisamente il difetto che il
  mondo allargato è stato costruito per togliere.
- **Il turno come stringa.** Se il turno non diventa un contenuto con un atto,
  ogni riferimento all'eschange sarà una cue, e ce ne vorrà una nuova per ogni
  formulazione.

---

## 8. Da dove si comincia

**F1, e non un'altra.** È la più piccola, è misurabile in un turno solo, ed è la
condizione delle altre quattro: finché un turno non è un contenuto con un atto,
non c'è nulla a cui ancorare una mossa, e «nello spazio logico» resta una frase.

Il primo comando da eseguire è la riproduzione dell'esempio, perché il piano si
apre con una misura e non con un'intenzione:

```sh
printf '%s\n' 'sai parlare italiano' \
  'quali parole conosci che possono sorprendermi' \
  'in quale lingua ti ho chiesto quale lingua sai parlare' \
  'cosa pensi della vita?' \
  'secondo te il tempo esiste davvero?' '/quit' | \
  PARROT0_SESSION= PARROT0_LANG=it PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0
```

Le cinque righe della tabella §1 sono il punto di partenza. La prima che deve
cambiare è la terza, e deve diventare **«in italiano»**.
