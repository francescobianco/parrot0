# Live teaching — l'agente insegna a parrot0 dal vivo, F. guarda e indirizza

**Piano, 25 settembre 2026.** Nasce da una richiesta di F. alla fine della
sessione §28–§30 di [l3-upgrade.md](l3-upgrade.md):

> *«non capisco come mai non possiamo fare delle sessioni di addestramento
> esclusive e dinamiche con parrot aperto come demone con tmux e tu che ci parli
> e lo addestri; invece lo tratti sempre come una codebase: dopo ogni prompt fai
> girare test o fai verifiche. […] deve essere una cosa live: non devi fare
> test, fare girare engine separati; si tratta proprio di un processo vivo che io
> osservo, e vedo te che parli con parrot0 e cerchi di farlo crescere.»*
>
> *«il processo di addestramento live deve essere un processo vivo e controllato
> da te, che io posso osservare in una console separata; quando vedo qualcosa
> che non mi convince posso dirti in live cosa indirizzare. È qualcosa che tu
> piloti, ma con la tua intelligenza.»*

> **In una frase.** Una sessione di live teaching è **una conversazione**, non un
> ciclo di sviluppo. Un solo processo parrot0, con la KB viva completa, resta
> aperto per tutta la sessione. L'agente è l'insegnante: legge ogni risposta,
> ragiona, sceglie la mossa successiva e la dice in lingua naturale. F. vede
> tutto dal vivo (prompt, risposte, ragionamento) in una console separata e può
> indirizzare in qualunque momento. La crescita si misura parlando con parrot0,
> non con un banco.


---

## ⭐ HANDOFF — 26 settembre 2026, notte (ripresa di domani)

**Dove siamo.** Tre sessioni live oggi: fisiologia (chiusa in
[sessions/2026-09-26-live-fisiologia.md](../sessions/2026-09-26-live-fisiologia.md)),
cantieri ([sessions/2026-09-26-live-cantieri.md](../sessions/2026-09-26-live-cantieri.md),
committata con la KB curata), procedure per nome
([sessions/2026-09-26-live-procedure.md](../sessions/2026-09-26-live-procedure.md),
chiusa senza `/save`). La terza ha rivelato che **l'interprete delle procedure
insegnate era una `switch` di `strcmp` nel C** — F.: *«grave rottura del
principio KB-first, dobbiamo sistemarle»* — e la notte è finita in un passo di
motore, non in una sessione. Lo studio completo è in
[parrot-p0-syntax.md §17](../parrot-p0-syntax.md).

**Che cosa è entrato (commit di stanotte, `make soft-test` verde).**

- **L'interprete è KB** (`kb/core/procedures.p0`, sezione «LE PROCEDURE NOMINATE»):
  `proc_run/3`, `step_term/2` (parole → termine), `run_step/4`, `cond_holds/2`,
  i mattoni sui caratteri (`keep_chars`, `rev_list`, `upper_lower/2`, `char_rank/2`,
  `replace_chars`, `sort_words`…). Operatori e condizioni sono clausole; i passi
  numerici (`halve`, `triple`, `add 1`, `divide by 2`) riusano `agent_branch_step/3`
  e `apply_operator/4`; le condizioni `even`/`odd` riusano `agent_parity_marker/2`;
  `reaches N` / `below N` / `above N` confrontano. Nuovo il ramo a due vie
  `if <c> then <s> else <s>`.
- **Il C si è accorciato**: tolte 259 righe (`p0_cond_holds`, `p0_apply_op`,
  `p0_run_proc`) da `10-memory-knowledge.c`; l'atto `run_procedure` chiede
  `proc_run` con `kb_match`; `assert_ordered` **rifiuta alla lezione** un passo
  che `step_readable/1` non legge (template `procedure_step_unknown`, con le
  parole ammesse da `step_surface/1`). Bilancio C: −282 / +215.
- **Una primitiva nuova nel solver**: `iterate(Passo, Arresto, In, Out)` (`kb.c`,
  accanto a `findall`): rifà `Passo/2` finché `Arresto/1` regge, a profondità
  costante, consumando il budget. Serve perché un ciclo come ricorsione KB
  sfonda `KB_MAX_DEPTH` (64: misurato, `power(2,60)` passa, `power(2,70)` no).
- **Il tetto di profondità è una specie sua** (F.: *«come i paradossi: quando li
  raggiunge fa inferenza con essi e te ne parla»*): `Solver.depth_hit` +
  `depth_pred`, `KbInferenceReport.depth_hit`, `paradox_event(proof, depth, Pred,
  seen(...))`; in `composition.p0` lo stadio `depth_reached` e
  `inference_incomplete(current_turn, depth)`; i template `procedure_stopped_depth`
  / `procedure_stopped_budget`.

**Verificato dal vivo** (scratch `proc.p0t`, profilo base): `apply devowel to
parrot` → prrt; `apply squash to baaad` → bad (ciclo `until stable` via
`iterate`); `rule for vowels is keep vowel` → `apply vowels to parrot` → ao;
`rule for collatz_step is if even then halve else triple and add 1` →
`apply collatz_step to 6` → 3, `to 3` → 10; `rule for bogus is frobnicate the
value` → rifiutato con l'elenco dei passi ammessi.

**Aperto — da qui si riparte (in ordine).**

1. **Lo smalltalk ruba «rule for collatz is repeat apply collatz_step until it
   reaches 1»** («I know I repeat myself…»): la lezione non entra, quindi il
   ciclo nominato non è mai stato eseguito dal vivo. `turn_form_priority(teach_proc,
   early)` NON basta (provato e tolto): chi rivendica sta prima delle forme
   early. Diagnosi con `/debug trace path` sul turno; la cura è una cessione KB
   (`faculty_yield` / `turn_declared_act`), mantra #17.
2. **`apply howmany to parrot` dà 6 invece di 2** con `howmany` = «apply vowels»,
   «count»: la traccia mostrava che la lezione «rule for howmany is apply
   vowels» a volte non veniva salvata (stesso genere del punto 1: un altro
   lettore prende il turno). Verificare con `!query proc_step(howmany, 1, …)`
   subito dopo la lezione.
3. **Il budget esaurito dentro `iterate` non arriva al report**: `rule for
   forever is repeat add 1 until below 0` gira ~83 000 giri, si ferma per
   budget, ma `kb_inference_report` dopo `kb_match(proc_run…)` dice
   `budget_hit=0` e parrot0 cade allo smalltalk invece di dire
   `procedure_stopped_budget`. Guardare `kb_match` → `kb_note_inference` e il
   `Solver` che `iterate` riceve. La parte «depth» è scritta ma **non ancora
   provata** dal vivo.
4. **La traccia dei passi** (`turn_plan_step`) è scritta dalla KB con `assert`,
   ma `kb_match` risolve con `kb_mut == NULL`: l'`assert` fallisce (reso
   best-effort in `proc_trace`), quindi «how do you know?» conta 0 passi. Serve
   un ingresso con KB scrivibile, o la traccia come valore di ritorno.
5. **Ancora conoscenza compilata, stessa famiglia** (`C_TODO.md`, voce del 26
   settembre): il ciclo anonimo `mod_agent` (`60-agent-tools.c`) deve diventare
   un consumatore di `proc_run` (una procedura senza nome); «What is the
   factorial of 6?» lo calcola `20-math.c` mentre `factorial/2` in KB non ha
   consumatori; il lettore delle regole `if … then` (solo triple, niente
   confronti); nessuna forma «do you know <procedura>?».
6. **Poi la sessione**: riaprire con `scripts/live-teach.sh start` e rifare il
   protocollo di F. — «Do you know the Collatz sequence?» → no → le tre lezioni
   → «apply collatz to 6» → 1, «to 27» → 1 — e chiedere «how do you know?».
   Il test scratch è in `tests/p0t/procedures/taught_numeric.p0t` (da scrivere
   a partire dal `proc.p0t` di stanotte, dopo che 1–3 sono chiusi).

Regressione nota, non di stanotte: «what is the capital of france» oscilla
intorno a 1,1–1,2 s (`np_closer` ~500 ms, `turn_verb_before/1` del commit
RI-012); stanotte il soft-test è passato, ma va curata.

---

## 1. Perché serve, e che cosa correggeva

Fino a oggi l'addestramento è passato quasi sempre dai `.p0t`: un demone di test
nuovo a ogni giro, un file di asserzioni, il verde e il rosso. È lo strumento
giusto per **fissare** una capacità, ma sbagliato per **farla crescere**:

- ogni `.p0t` ricomincia da una KB appena caricata: non c'è una storia, e quindi
  niente di ciò che L3 promette (contatto, uso, correzione, ritiro nel tempo);
- il giro test → lettura → modifica tratta parrot0 come una codebase: la mossa
  successiva la decide un diff, non ciò che parrot0 ha appena detto;
- F. vede solo il resoconto finale, mai il percorso; non può correggere una
  scelta dell'insegnante nel momento in cui la vede;
- la sessione §28–§30 ha speso più tempo a far girare motori paralleli, a
  indovinare dove fallisse una prova e ad aspettare banchi che a insegnare (§28.9).

Il live teaching è il canale 1 della gerarchia di crescita del
[MANTRA](../../MANTRA.md): **insegnamento diretto via prompt**, condotto come lo
condurrebbe una persona.

## 2. Il dispositivo

`scripts/live-teach.sh` (solo shell, nessun cambiamento al motore):

| comando | chi | che cosa fa |
|---|---|---|
| `start [titolo]` | insegnante | apre la sessione tmux `parrot0-live`: parrot0 con la KB `agi` completa legge le righe dell'insegnante e risponde riga per riga; il transcript e il socket per chi guarda |
| `say "frase"` | insegnante | manda una riga a parrot0, **aspetta la risposta** (il ritorno al prompt) e la restituisce all'insegnante, insieme alle note di F. arrivate nel frattempo |
| `think "nota"` | insegnante | scrive nel transcript il ragionamento che porta alla mossa successiva |
| `steer "nota"` | **F.** | un indirizzo dal vivo: entra nel transcript come `[F.]` e l'insegnante lo riceve con la risposta successiva |
| `watch` | **F.** | segue il transcript dal vivo (`tail -f`) |
| `stop` | insegnante | `/save`, archivia il transcript in `docs/sessions/live/`, chiude tmux |

**Per guardare**, in una console separata, uno a scelta:

```sh
scripts/live-teach.sh watch                      # il transcript, dal vivo
socat - UNIX-CONNECT:var/live/watch.sock         # lo stesso dal socket (anche nc -U …)
tmux attach -r -t parrot0-live                   # le finestre della sessione, in sola lettura
```

**Il transcript** (`var/live/transcript.log`, archiviato alla chiusura) mescola in
ordine le voci, ognuna con il suo prefisso (formato chiesto da F.): `> ` il prompt
dell'insegnante, `< ` la risposta di parrot0, `! ` il ragionamento che guida il
prompt successivo, `F: ` un indirizzo di F., `# ` la sessione e il sistema:

```text
! provo che il canale funzioni: una domanda di cui conosco la risposta
F: prova: questa nota viene da F.
> Does water contain hydrogen?
< Yes.
```

Provato il 25 settembre: domanda, risposta, ragionamento e nota di F. escono
dal socket nell'ordine giusto; il boot della KB completa richiede ~20 s, poi
parrot0 risponde dal vivo.

**Due canali per indirizzare.** F. può scrivere all'agente nella chat (arriva a
metà turno) oppure con `steer` dalla propria console. In entrambi i casi la nota
cambia **la mossa successiva**: l'insegnante la cita nel ragionamento prima del
prompt che la segue, così nel transcript si vede che è stata ascoltata.

## 3. Le regole della sessione

1. **Un solo processo, dall'inizio alla fine.** Niente `make test-engine`, niente
   `.p0t`, niente rebuild, niente chat parallele per «provare prima». Se una
   cosa va verificata, la si chiede a parrot0 nella sessione.
2. **Solo lingua naturale** (MANTRA, anti-inganno): niente `!assert`, niente MCP,
   niente nomi di predicati nelle frasi dell'insegnante. Un esperto del dominio
   che ignora lo schema interno deve poter formulare ogni lezione.
3. **Conoscenza vera, in campi utili.** Niente entità inventate, niente mondi
   finti, niente esempi da demo. I campi si scelgono dove la KB ha già fatti
   veri e il sapere serve a qualcuno: cucina e sicurezza alimentare, impianti
   elettrici, fisiologia, materiali, geografia, unità e misure, la codebase
   stessa (§6).
4. **Il ragionamento è parte del prodotto.** Prima di ogni mossa che non sia
   ovvia, una riga di `think`: che cosa ha rivelato la risposta, quale ipotesi
   la spiega, che cosa proverà la mossa successiva. F. deve poter giudicare la
   scelta, non solo l'esito.
5. **L'introspezione di parrot0 è ammessa, anzi è lo strumento.** `/debug`,
   `/debug trace <parola>` e le domande «why did you answer that way?» /
   «perché non lo sai più?» sono turni della conversazione: mostrano la lettura
   (IR), le cessioni del turno, i contatti, le ipotesi. È lì che l'insegnante
   guarda quando una risposta è sbagliata, non nel C.
6. **Quando serve il motore, la sessione si ferma.** Se la mossa giusta è
   cambiare C (una primitiva mancante, un crash, una lentezza), l'insegnante lo
   dichiara nel transcript, chiude con `stop` e il lavoro di motore diventa un
   passo separato con i suoi controlli. Nessuna patch a sessione aperta.
7. **Il tempo di una risposta è un dato.** Un turno oltre i 10–15 s si annota
   nel transcript (`think`) come difetto; oltre `LIVE_TEACH_WAIT` (180 s) lo
   script lo registra come mancata risposta. Non si aspetta in silenzio.
8. **Quello che la sessione lascia è KB.** Alla chiusura `/save` scrive ciò che
   parrot0 ha imparato nella ricaduta di salvataggio (`kb/learning/learned.p0`,
   la stessa della CLI). Le lezioni riuscite si **committano** con il transcript
   come provenienza (memoria: *autolearn knowledge is official*, *learning
   sessions use true knowledge*); quelle non riuscite restano nel transcript
   come diagnosi.

## 4. I meccanismi su cui la sessione lavora

L'insegnante deve sapere quale meccanismo sta chiamando in causa a ogni mossa, e
dirlo nel ragionamento. Sono i quattro elementi del piano di training più i due
livelli d'insegnamento.

### 4.1 La IR — [universal-input.md](universal-input.md)

L'input è **uno**: ogni turno diventa nodi (token, sintagmi, quantità, entità)
che tutti i lettori consultano, e le categorie che li decidono sono KB. In
sessione la IR si guarda con `/debug` (sonde `debug_turn_entity`,
`debug_quantity`, `debug_np_candidate`, `debug_ir_node`) e si **cambia parlando**
dove il confine è conoscenza:

- `"-" is a word joiner`: il trattino tiene insieme una parola («step-down»);
- `"-" is a number sign`: il segno appartiene al numero («-39»), §28.8;
- `"'s" is a clitic`: il genitivo si stacca dalla parola («France's»), §29;
- le lezioni di confine di L2 («end the previous noun phrase before X»).

Quando una risposta sbagliata nasce da una lettura sbagliata (un'entità
incollata, `acetone_boils`; un segno perso), la mossa giusta è sulla IR, non sul
fatto.

### 4.2 La comprensione universale — [universal-comprehension.md](universal-comprehension.md)

Nessuna frase ben formata merita un muro cieco: la struttura si estrae sempre,
e un muro dice **che cosa** manca. In sessione la si verifica leggendo **come**
parrot0 non capisce: «I don't know about X» (manca un nome), «I cannot settle
that» (manca una prova), «I found the teaching pivot, but…» (la lezione è letta
ma non ancorata). Ogni muro è una diagnosi per la mossa successiva. Un muro
**falso** («no fact I hold decides…» su un fatto presente, §30) è la prima cosa
da segnalare nel ragionamento: pesa più di un «non so».

### 4.3 Il mondo allargato — [the-rational-philosopher.md](the-rational-philosopher.md)

Ciò che parrot0 sente non è subito un fatto: è un contenuto con un atto, una
fonte, un giudizio. L'ipotesi di contatto, il sostegno di una lettura
(`read_support`), lo strato che sospende senza cancellare (§19 e §14.6 di L3), le
letture concorrenti che chiedono un esempio: tutto questo è il mondo allargato
al lavoro. In sessione conta per due ragioni. L'insegnante può **contraddire**
(un controesempio ordinario ritira un'ipotesi) e **chiedere conto** («why don't
you know that anymore?»). E parrot0 può prendere l'iniziativa, cioè proporre un
seguito o chiedere, quando una risposta resta aperta.

### 4.4 L2 — [l2-upgrade.md](l2-upgrade.md)

Le lezioni con uno **schema** («X means Y», «what is x's y means what is the y of
x», «end the previous noun phrase before X», «"X" is a contraction of "Y"»),
comprese le **forme con variabili**, che insegnano una classe con una lezione
sola (il genitivo del §29 vale per ogni nome di relazione). L2 è il **ripiego
dichiarato** della sessione: lo si usa quando il contatto non basta, e si scrive
nel ragionamento «qui uso uno schema, perché …». L2 e L3 convivono (§18 di L3).

### 4.5 L3 — [l3-upgrade.md](l3-upgrade.md)

L'insegnamento **per contatto**, senza registro speciale: ciò che parrot0 non
legge si allinea con ciò che legge nello stesso turno. È il primo meccanismo
della sessione, e la sessione è il posto dove L3 ha senso, perché L3 vive nel
tempo (episodi, conferme, controesempi, ritiri). Oggi parrot0 impara per
contatto:

- un **nome** o un **verbo** di relazione, anche di più parole («his Geburtsort is
  Ulm», «so he hails from Ulm», «its boiling point is …»), §21–§27;
- uno **strumento** del contatto («in other words») da due contatti su relazioni
  diverse, §26;
- un **modo di riferirsi** (il ruolo ripreso per nome) da due contatti, §28 H1;
- la **categoria** di una parola che entra nella lettura (il verbo appreso chiude
  il sintagma), §28 H2, e la **cornice** della relazione nominata, §28.7.

Ogni conclusione si ritira con un controesempio ordinario e sa dire perché è
caduta (§19.3). La sessione deve **provare anche questo**: un'ipotesi sbagliata
nata da una coincidenza vera, poi il controesempio, poi il caso tenuto fuori.

## 5. Com'è fatta una sessione

Una sessione ha un **campo** e una **domanda di partenza**: «che cosa, in questo
campo, parrot0 dovrebbe saper fare e non sa?». Non ha un copione.

1. **Esplorare.** Domande naturali che una persona del campo farebbe davvero. Il
   ragionamento annota che cosa risponde bene, che cosa mura e come, che cosa
   sbaglia.
2. **Scegliere il muro** che, superato, vale di più: una classe e non un caso, un
   errore prima di un'ignoranza.
3. **Diagnosticare parlando.** `/debug`, «why did you answer that way?», una
   variante della domanda. Il ragionamento nomina il meccanismo (§4).
4. **Insegnare.** Prima per contatto (L3), con una frase che direbbe una
   persona; se non basta, lo si dichiara e si passa a L2.
5. **Trasferire.** Un caso tenuto fuori, mai nominato nella lezione: se risponde
   solo l'esempio, non ha imparato.
6. **Contrastare.** Un caso vicino dove la lezione **non** deve valere.
7. **Correggere.** Se è nata una lettura sbagliata, un controesempio ordinario;
   poi di nuovo il caso tenuto fuori.
8. **Chiudere**: `stop`, poi il transcript letto a freddo e il resoconto (§7).

F. può interrompere in ogni punto con `steer` o in chat. L'insegnante riprende
dal punto indicato e lo dice.

## 6. Campi per le prime sessioni

Scelti perché la KB ha già fatti veri e il sapere serve (proposta, da
confermare con F.):

| campo | fatti già in KB | domanda di partenza |
|---|---|---|
| cucina e sicurezza alimentare | `safe_internal_temp`, `max_hours_out`, `perishable`, `cup_grams` (kb/facts/food-kitchen.p0) | «How long can cooked rice stay out?», «Is chicken safe at 70 degrees?», le grammature per tazza |
| impianti elettrici e sollevamento | `rated_current_of`, `working_load_of`, `operating_voltage_of`, `step_down` (kb/facts/engineering.p0) | la corrente di un fusibile, il carico di un golfare, a che cosa serve un trasformatore |
| fisiologia | `produces`, `secretes`, `pumps`, `filter`, `sleep_need_hours` (kb/facts/physiology.p0) | «Does the liver produce bile?» (oggi risponde, §30), che cosa filtra il rene, quante ore di sonno |
| scienza dei materiali e fasi | `boils_at`, `freezes_at`, `made_of`, `contains` | i punti di ebollizione e congelamento con il segno (§28.8), di che cosa è fatto un materiale |
| la codebase di parrot0 | `docs/`, `kb/`, i propri piani | parrot0 che impara a spiegare i propri meccanismi (IR, L3) a chi lo usa |

## 7. Che cosa resta di una sessione

- il **transcript** archiviato (`docs/sessions/live/AAAA-MM-GG-HHMM.log`), con
  ragionamento e indirizzi di F.;
- la **KB salvata** da `/save`, committata se le lezioni hanno retto al
  trasferimento e al contrasto;
- un **resoconto breve** in `docs/sessions/`: il campo; le classi aperte; i muri
  trovati e il meccanismo che li spiega; ciò che ha richiesto uno schema (L2) e
  perché; ciò che resta per il motore;
- le voci per gli **strumenti di introspezione** (§28.9 di L3) quando
  l'insegnante non ha potuto vedere perché parrot0 ha risposto così.

Un `.p0t` si scrive **dopo**, a sessione chiusa, solo per fissare una capacità
che la sessione ha già mostrato e che non deve regredire.

## 8. Limiti del dispositivo, oggi

1. Le righe di parrot0 nel transcript non hanno orario. Le scrive `say`, che lascia fuori i dump di `/debug` (una riga `#` al loro posto) e, con il profilo acceso, le righe rientrate e le `[debug]`: chi guarda non li legge (F., 26 settembre).
2. `say` riconosce la fine della risposta dal ritorno al prompt («>>> »). Una
   risposta che non torna al prompt blocca `say` fino a `LIVE_TEACH_WAIT`.
3. `/save` scrive nell'albero curato (`kb/learning/learned.p0`). Una sessione
   esplorativa che non vuole lasciare tracce si chiude senza `stop` (`tmux
   kill-session -t parrot0-live`), e lo si dice nel transcript.
4. Il binario è quello compilato all'avvio: una modifica del motore richiede
   una sessione nuova (regola 6).
5. Una sessione alla volta: nome tmux e socket sono fissi.
