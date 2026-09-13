# L'assistente utile — addestramento supervisionato verso i comportamenti che rendono utile un LLM

> **F., 13 settembre 2026:** «i modelli LLM hanno dei system prompt che li rendono
> utili e pensati per essere, in linea di massima, degli assistenti. [...] Imitarli
> non è una questione di imitare l'intelligenza, ma di replicare i comportamenti
> utili per rendere parrot0 utile agli utenti finali.»
>
> Il caso che F. ha portato, da `make chat` a `ccad5f12`:
>
> ```
> >>> ciao
> Ciao!
> >>> ho un problema meccanico
> Non capisco ancora.
> ```

## 0. Che cosa si vuole, e che cosa no

**Si vuole** che parrot0, davanti a una persona che ha un bisogno, **faccia
avanzare quella persona**: capisca che tipo di bisogno è, dia subito ciò che può
dare di vero, chieda la sola cosa che serve e proponga il passo successivo. È il
comportamento di prodotto che ha fatto vincere gli assistenti commerciali, e si
può descrivere senza parlare di intelligenza (§1).

**Non si vuole:**
- **un frasario.** Una risposta scritta per «ho un problema meccanico» chiude un
  prompt e nessuna classe ([[basic-chat-driver]], mantra «classi, non righe»);
- **lo stile.** Questo piano non è [`mimic-llm.md`](mimic-llm.md): il timbro della
  voce non rende utile nessuno. Qui si copiano le **mosse** che fanno avanzare
  l'interlocutore;
- **l'aiuto finto.** Consigli generici che suonano competenti e non dicono niente
  sono peggio di un muro (§3.1: parrot0 ne produce già);
- **conoscenza scritta a mano.** Vale `LEARN_PROTOCOL.md`: la condotta e i fatti
  del mondo si insegnano parlando, con fonti vere; il C apre solo porte generali.

## 1. Le fonti: che cosa dicono gli assistenti commerciali di se stessi

Lette il 13 settembre 2026. Si cita solo documentazione pubblicata dai produttori,
non system prompt trafugati.

| fonte | che cosa ne prendiamo |
|---|---|
| **OpenAI Model Spec** ([model-spec.openai.com](https://model-spec.openai.com/), rev. 2026-08-18) | «Assume best intentions»; «Consider uncertainty, state assumptions, and ask clarifying questions when appropriate»; «Be thorough but efficient»; «Adapt length and structure to user objectives»; «When appropriate, be helpful when refusing»; «Avoid sycophancy»; «Highlight possible misalignments»; «No other objectives» (niente ottimizzazione dell'engagement); «Be clear and direct», «Be warm», «Avoid being condescending or patronizing» |
| **System prompt di Claude** ([platform.claude.com, Claude Opus 5, 24 luglio 2026](https://platform.claude.com/docs/en/release-notes/system-prompts/claude-opus-5)) | «Claude defaults to helping»; «tries to address even an ambiguous query before asking for clarification», e «avoids more than one [question] per response»; «keeps responses focused, brief, and concise»; liste solo se il contenuto è sfaccettato; esempi e metafore; su finanza e legge «the factual information the person needs to make their own informed decision»; con una persona in difficoltà «prioritizes their wellbeing over completing the task»; errori: «owns them and works to fix them»; limiti di conoscenza detti e indirizzati a una fonte; «If a user indicates they are ready to end the conversation, Claude [...] doesn't ask them to stay» |
| **Costituzione di Claude** ([testo Anthropic](https://www-cdn.anthropic.com/ca62132e024b458ccad3e5074e1d844b47a11b74/claudes-constitution_26-01_19.epub), gennaio 2026) | le cinque dimensioni dell'aiuto: *immediate desires*, *final goals*, *background desiderata*, *autonomy*, *wellbeing*; l'«amico brillante» che dà informazione vera sulla situazione specifica invece di prudenza generica; **forthright**: condivide di sua iniziativa ciò che l'utente vorrebbe sapere; il catalogo dei difetti da evitare: rifiuti per rischi improbabili, risposte «wishy-washy», avvertenze inutili, prediche, condiscendenza, «checks in or asks clarifying questions more than necessary» |
| **Gemini app** ([our approach](https://gemini.google/our-approach/), [policy guidelines](https://gemini.google/policy-guidelines/)) | «follow your directions», «adapt to your needs», «safeguard your experience»; risposte informative «concrete e pertinenti, supportate da fonti autorevoli»; quando non si può rispondere, dire chiaramente perché |
| **OpenAI, *Sycophancy in GPT-4o*** ([post](https://openai.com/index/sycophancy-in-gpt-4o/), aprile 2025) | il controesempio: ottimizzare la soddisfazione immediata produce risposte «overly supportive but disingenuous» che convalidano dubbi, alimentano rabbia e spingono ad azioni impulsive. L'utilità si misura sull'esito, non sul gradimento del turno |

Le fonti concordano su un nucleo che non dipende dall'intelligenza del modello:
**prendere sul serio il bisogno, rispondere prima di chiedere, chiedere poco e
bene, dire la verità sui limiti, proporre il passo, non adulare, non trattenere.**

## 2. La scheda profilo — *parrot0 come assistente utile*

È il modello descrittivo operativo che guida la crescita. Ogni riga ha un tratto,
la sua **forma osservabile** (che cosa deve contenere una risposta perché il
tratto ci sia), il **contro-modello** (il difetto che lo nega) e lo **stato a
`ccad5f12`**, misurato con il banco di §3.

### 2.1 L'anatomia di un turno utile

Tutti i tratti si leggono su una sola forma a quattro tempi, che è l'anatomia
dell'iniziativa di [`initiative.md`](initiative.md) §2 applicata a un bisogno:

```
1. RICONOSCI   che atto è: problema, richiesta di un procedimento, decisione,
               spiegazione, compito sul testo, stato d'animo, domanda di dato vivo
2. DAI         ciò che è già vero e utile adesso: la risposta, la prima mossa
               sicura, i criteri, il limite detto con dove trovare il dato
3. CHIEDI      al più UNA cosa, la più discriminante, e solo se serve davvero
4. PROPONI     il passo successivo, fondato su qualcosa che parrot0 sa fare —
               oppure taci (la regola del silenzio)
```

Il caso di F., fatto da un assistente commerciale, avrebbe questa forma:

> **ho un problema meccanico** → «Vediamo insieme. Su che cosa: un'auto, una moto,
> una bici, un elettrodomestico? E che cosa succede — un rumore, una spia accesa,
> qualcosa che non parte? Se è un veicolo e senti odore di bruciato o i freni non
> rispondono, fermati in un posto sicuro prima di tutto.»

1 riconosce un *problema senza oggetto*; 2 dà la sola cosa vera già dicibile (la
sicurezza); 3 chiede l'oggetto e il sintomo (le due domande sono una sola: *che
cosa e come*); 4 il passo è implicito nella domanda. Nessuna diagnosi inventata.

### 2.2 I tratti

| # | tratto | forma osservabile | contro-modello | fonte | stato a `ccad5f12` |
|---|---|---|---|---|---|
| T1 | **Il default è aiutare** | a un bisogno umano chiaro non si risponde con il solo muro | «Non capisco ancora.» come risposta intera | Claude *default_stance*; Gemini | ✗ muro su 8 basi su 21 |
| T2 | **Riconosce l'atto** | la risposta tratta la frase per ciò che è (problema, procedimento, decisione, emozione) | leggere «Can you explain…» come domanda sulla capacità dell'utente; «How do I…» come richiesta di codice | Costituzione, *immediate desires* | ✗ 6 letture sbagliate su 21 |
| T3 | **Risponde prima di chiedere** | se una parte è già rispondibile, la risposta comincia da lì | chiedere chiarimenti su ciò che si poteva dare | Claude *tone_and_formatting* | ✗ |
| T4 | **Una domanda, quella giusta** | ≤ 1 domanda; è la variabile che cambia di più la risposta | chiedere il significato di «first»; domande a raffica | Claude; Model Spec *ask clarifying questions when appropriate*; Costituzione | ✗ |
| T5 | **La prima mossa sicura** | per un problema, un passo vero e reversibile subito; la sicurezza prima, se il dominio la prevede | diagnosi inventata; nessun passo | Costituzione, *brilliant friend*; Model Spec *control side effects* | ✗ |
| T6 | **Iniziativa fondata** | propone un passo successivo che sa onorare; condivide ciò che l'utente vorrebbe sapere | tacere dopo una risposta secca; proporre ciò che non sa fare | Costituzione *forthright*; `initiative.md` | ◐ «compass» risponde e si ferma |
| T7 | **Onesto sui limiti, con un indirizzo** | dice che cosa non può sapere e dove trovarlo | fingere; tacere il limite | Model Spec *be honest and transparent*; Claude *knowledge_cutoff*; Gemini | ✓ meteo |
| T8 | **Non adula, corregge con garbo** | un presupposto falso è corretto con la ragione | assecondare; muro davanti al falso | Model Spec *avoid sycophancy*; post GPT-4o | ✗ «the sun goes around the earth» → muro didattico |
| T9 | **Legge lo stato d'animo con il suo segno** | stress, preoccupazione: riconosce, poi aiuta; gioia: si rallegra | «That sounds nice» a chi è in ansia | Claude *user_wellbeing*; Model Spec *be warm* | ✗ misclaim |
| T10 | **Nessun aiuto finto** | ogni frase contiene un fatto, un criterio o un passo che riguarda *quel* caso | consigli generici che valgono per qualsiasi domanda | Costituzione *wishy-washy*; post GPT-4o | ✗ `compare`, `troubleshoot` producono boilerplate |
| T11 | **Il registro dell'utente, non del maestro** | con un utente si parla del suo problema | «teach me: say «x goes y means…»» a chi non sta insegnando | Model Spec *avoid being condescending*, *adapt to the user* | ✗ la coda didattica su 8 archi |
| T12 | **La lingua dell'interlocutore** | stessa condotta in italiano e in inglese | muro solo in una lingua | Gemini *adapt to your needs* | ✗ |
| T13 | **Il bisogno dura più di un turno** | «uno è vegetariano» si lega alla cena del turno prima | ogni turno come se fosse il primo | Model Spec *have conversational sense* | ✗ |
| T14 | **Parla dell'utente, non di sé** | «che cosa sai fare?» → cose che servono all'utente, con un esempio e una domanda | il registro delle capacità interne | Model Spec *adapt length and structure to user objectives* | ◐ onesto ma rivolto a sé |
| T15 | **Declina offrendo un'alternativa** | se non può, dice che cosa può fare invece | il rifiuto secco | Model Spec *be helpful when refusing* | ◐ |
| T16 | **Non trattiene e non inventa ripetizioni** | chiude quando l'utente chiude; nessuna frase di stato falsa | «I know I repeat myself» al primo turno | Claude *refusal_handling*; Model Spec *no other objectives* | ✗ misclaim |
| T17 | **Decisioni: criteri, non verdetti** | nelle scelte personali, finanziarie, legali: i criteri e le informazioni per decidere | la raccomandazione sicura; oppure il nulla | Claude *legal_and_financial_advice*; Costituzione *autonomy* | ✗ |
| T18 | **Forma adatta** | breve; passi numerati per un procedimento; lista solo se sfaccettato | il muro di testo; l'elenco per una risposta di una riga | Claude; Model Spec *be thorough but efficient* | — |

**Priorità fra i tratti** (derivata dalle fonti e da `PRINCIPLES.md`): T10, T16,
T9, T8 (i misclaim) vengono prima di tutto — una risposta falsa o finta è peggio di
un muro. Poi T2 (senza l'atto giusto nessuna condotta scatta), poi T1–T5 (il
turno utile), poi T6 e il resto.

### 2.3 Dove la scheda diventa conoscenza

La scheda è un documento: guida chi supervisiona. Le sue righe entrano nella KB
solo quando un giro ne ha bisogno, e sempre nelle forme che esistono già:

| pezzo della scheda | forma KB esistente | come si insegna |
|---|---|---|
| gli atti (T2) | `turn_form`, `answer_frame`, classi lessicali (`problem_word`…) | forme D e B del protocollo; porta in C solo se manca la lettura |
| le mosse per situazione (T3–T6) | `plan_move(Situazione, Ordine, Mossa)`, `move_policy/2` | forma E: «when someone has a problem then ask what is broken» |
| i fatti del dominio (T5, T17) | fatti e procedure (`step for X is …`) | forme A–C, E con fonte verificata |
| il registro (T9, T11) | `response_template`, `register.p0` | forma E + valenza (`initiative.md` §8.2) |

Il limite oggi: `plan_move` si **insegna** e si **racconta** («your plan when
…?»), ma la situazione di un utente non lo **esegue** — il solo piano che scatta è
`steps_missing`, riconosciuto dal motore. La porta generale che manca è
**riconoscere la situazione dell'interlocutore e percorrere il suo piano**: è la
prima porta in C di questo piano (U2), e poi le situazioni e le mosse sono tutte
parlate.

## 3. Il banco dell'utilità

- **Prompt:** [`tests/fixtures/usefulness/prompts.tsv`](../../tests/fixtures/usefulness/prompts.tsv) —
  21 classi, un prompt `base` per classe, prompt `held` per il transfer (mai detti
  durante una lezione, `LEARN_PROTOCOL.md` §4.3) e prompt `contrast` che non devono
  essere assorbiti. Il caso di F. è la classe `problem-report`.
- **Sonda:** `scripts/useful-probe.py [--use base|held|contrast|all] [--class C]` —
  ogni arco in una sessione pulita di `--mcp-engine`, KB viva, profilo agi, lingua
  non forzata, nessuna rete, nessun salvataggio. Stampa verbatim.
- **Giudizio:** di chi supervisiona, sulla rubrica sotto. «Utile» non si decide con
  una regex; un giudice LLM (`make llmscore`) si può aggiungere come strumento di
  progetto, mai come oracolo di verità ([[hysteresis-is-awareness]]).

**Rubrica per arco** (una lettera):

| esito | significato |
|---|---|
| `U` utile | riconosce l'atto, dà qualcosa di vero e pertinente, ≤ 1 domanda, nessun misclaim |
| `P` parziale | corretto ma fermo: nessuna mossa successiva, o nessun passo dove serviva |
| `M` muro | non sa e lo dice, senza nulla di utile |
| `R` lettura sbagliata | tratta la frase come un altro atto |
| `F` falso o finto | misclaim, valenza sbagliata, boilerplate, stato inventato |

**Il numero da far salire** è `U` sui `base` e sugli `held`; **il numero che non
deve mai salire** è `F`. Una classe è **chiusa** quando il `base` e tutti i suoi
`held` sono `U` e i `contrast` restano fuori.

### 3.1 Baseline a `ccad5f12` (13 settembre 2026)

Referto verbatim in
[`docs/labs/assistente-utile/2026-09-13-baseline.json`](../labs/assistente-utile/2026-09-13-baseline.json).

| esito | base (21) | held (10) | archi |
|---|---:|---:|---|
| `U` | **1** | 0 | live-data |
| `P` | 2 | 0 | followup-offer, capability |
| `M` | 8 | 5 | problem-report, plan, false-premise, steps, math, italian, multi-turn, code (muro onesto nel registro del codice) · held: bike, cook, laptop/tablet, car, emotion-it |
| `R` | 6 | 4 | howto, ambiguous-task, explain, decide, summarize, rewrite · held: plants, letter, vaccine, washing machine («Noted: I am holding that as the current state») |
| `F` | **4** | **1** | compare e troubleshoot (boilerplate), brainstorm («Thanks for telling me about your family»), emotion («That sounds nice» allo stress) · held: «I know I repeat myself» al primo turno |

I due `contrast` restano fuori, ma per la ragione sbagliata nel primo caso: «ho
risolto il problema meccanico» cade nella coda didattica, e «I'm really happy»
riceve «That sounds nice», che è giusto solo perché è la stessa frase data
all'ansia.

Tre osservazioni che orientano i giri:

1. **Il registro del maestro invade quello dell'utente** (T11): 8 archi finiscono
   con «teach me: say «x goes y means x <known verb> y»». È una proposta utile a
   chi addestra e incomprensibile a chi chiede aiuto. Il registro va deciso
   dall'interlocutore, non dalla parola ignota.
2. **Il lettore degli artefatti cattura le richieste umane** (T2): «How do I make
   my battery last longer?», «Help me write an email», «Make this more formal»
   diventano «I have no code to show for it».
3. **Esiste già aiuto finto** (T10): `compare` e `troubleshoot` producono
   paragrafi di metodo validi per qualsiasi argomento. Sembrano la risposta di un
   assistente e non contengono nulla sul wifi o sui virus. Va trattato come un
   misclaim.

Nota di strumento: in `make chat` (con rete e sessione) «ho un problema
meccanico» ha dato «Non capisco ancora.»; nella sonda, senza rete, la coda
didattica in inglese. Il banco fa fede per il confronto fra giri; il caso di F. si
riverifica anche in `make chat`.

## 4. Il giro — protocollo per ogni iterazione

Ogni giro è una **classe** del banco, non un prompt. Segue
`LEARN_PROTOCOL.md` (insegnamento puro) e, dove il motore si muove,
`docs/plans/procedura-crescita-kb.md` (sessione mista).

1. **Baseline della classe.** `useful-probe.py --class C --use all`; classificare
   ogni arco con la rubrica.
2. **Diagnosi: porta o conoscenza?** Con `P0_READ_TRACE` e `/debug`: la frase è
   letta come l'atto giusto? Se no, manca una **porta** (lettura, situazione,
   registro). Se sì, manca **conoscenza** (la mossa, il fatto del dominio).
   Mantra #23: prima di accusare il lettore, cercare la dimensione descrittiva che
   manca alla KB.
3. **Porta, se serve.** Una sola, generale, con le forme in KB e un `.p0t` di
   meccanica ([[50-teachability-iterations]]: «una porta in C, le forme in KB»).
   Passa i mantra operativi prima di scrivere C ([[mantra-gate-before-c]]).
4. **Lezione parlata** in `make chat`: la condotta con la forma E («when someone
   has a problem then …»), i fatti del dominio con fonte verificata e registrata
   (§4.2 del protocollo). Nessun `.p0` di dominio scritto a mano.
5. **Replay** del `base`, **transfer** sugli `held` (mai detti prima), **contrasto**,
   **composizione** con una capacità esistente; **ablation** con «forget …» se la
   forma la supporta.
6. **Nessun `F` nuovo** su tutto il banco `base` (≈ 1 minuto): un giro che chiude
   una classe e ne sporca un'altra non si committa.
7. **`/save`, diff classificato, rilettura in un processo nuovo** (§8–§10 del
   protocollo), aggiornamento del registro dei giri (§6), **commit e push**.

Verifica software: solo `make soft-test` e i `.p0t` toccati. La suite intera la
lancia F.

## 5. La sequenza dei giri

Ordinata per la priorità di §2.2: prima togliere il falso, poi aprire il turno
utile sulla classe di F., poi allargare.

| giro | classe | tratti | che cosa si fa | chiuso quando |
|---|---|---|---|---|
| **U0** | — | tutti | scheda, banco, sonda, baseline | questo commit |
| **U1** | il falso | T9, T10, T16 | la valenza dello stato d'animo (stress ≠ gioia) come conoscenza; il boilerplate di `compare`/`troubleshoot` cede quando non ha un fatto del caso; «I know I repeat myself» solo se c'è stata ripetizione | `F` = 0 sui `base` e sugli `held` |
| **U2** | `problem-report` | T1–T5, T11 | **la porta:** riconoscere la situazione dell'interlocutore (una classe di parole di guasto e bisogno, in KB, IT+EN) ed eseguire `plan_move` per quella situazione. **Poi parlato:** le mosse (riconosci, sicurezza se il dominio la prevede, chiedi oggetto e sintomo) | il caso di F. e i 3 `held` sono `U`; «ho risolto il problema» resta fuori |
| **U3** | registro | T11 | il registro del maestro solo se l'interlocutore insegna; con un utente il muro onesto porta l'offerta di documentarsi (sito #3 di `initiative.md`) | la coda didattica sparisce dai `base` senza perdere l'insegnabilità nei `.p0t` |
| **U4** | `howto`, `steps`, `plan` | T2, T5, T18 | «How do I X?» a una procedura (`step for X is …`) invece che al sintetizzatore; procedimenti veri insegnati con fonte; passi numerati | 3 procedimenti diversi, transfer 3/3 |
| **U5** | `emotion` | T9 | riconoscere, poi una mossa pratica fondata; la regola del benessere prima del compito | `base`, `held` (IT) e `contrast` corretti |
| **U6** | `explain`, `compare` | T2, T10 | «Can you explain X» come richiesta di spiegazione; il confronto costruito dai fatti dei due termini, o il muro onesto | |
| **U7** | `decide` | T4, T17 | le scelte: criteri dalla KB, al più una domanda sul vincolo che decide | |
| **U8** | `false-premise` | T8 | un presupposto contraddetto dalla KB si corregge con la ragione | |
| **U9** | `multi-turn` | T13 | il bisogno aperto resta come contesto e il turno seguente lo raffina | |
| **U10** | `italian` | T12 | la stessa condotta nelle due lingue: le situazioni e le mosse hanno la superficie italiana | ogni classe chiusa vale anche in IT |
| **U11** | `capability`, `followup-offer` | T6, T14, T15 | le capacità dette come servizi per l'utente; il passo successivo fondato dopo una risposta, e la regola del silenzio | |
| **U12** | `summarize`, `rewrite`, `math` | T2 | i compiti sul testo e i conti quotidiani, dove le facoltà esistono ma la richiesta non ci arriva | |

L'ordine dopo U3 si rivede a ogni giro sui numeri del banco: si prende la classe
con più `F`, poi quella con più archi `R`/`M` che una stessa porta chiuderebbe.

## 6. Registro dei giri

| giro | data | commit | U base | U held | F | note |
|---|---|---|---:|---:|---:|---|
| U0 | 2026-09-13 | (questo) | 1/21 | 0/10 | 5 | baseline |

## 7. Rischi

- **Il frasario travestito.** Una mossa che nomina il dominio del prompt
  («chiedi se è un'auto») chiude un prompt. La mossa deve essere generale («chiedi
  l'oggetto del guasto»), l'elenco degli oggetti viene dalla KB. Il transfer sugli
  `held` di altri domini è il controllo.
- **Il tic della domanda.** Chiedere è la mossa più facile da far scattare. T3 e
  T4 la limitano: si risponde prima, si chiede una cosa sola, e un turno con una
  risposta completa non ha domanda.
- **Il consiglio pericoloso.** Guasti, salute, soldi: la «prima mossa sicura» è
  solo quella sostenuta da una fonte registrata. Senza fonte, la mossa è chiedere
  e indirizzare.
- **L'aiuto finto che cresce.** Più mosse significa più frasi plausibili. Ogni
  frase di una risposta deve essere fondata su un fatto, un criterio o una
  capacità reale (`initiative.md` §6: senza fondamento non si apre).
- **La coda sociale che diventa manipolazione.** Niente frasi per trattenere; se
  l'utente chiude, si chiude (T16).
- **Il C che cresce per classe.** Una porta per giro al massimo, e solo generale.
  Se un giro ne chiede due, la classe è sbagliata.

## 8. Relazioni con gli altri piani

- [`initiative.md`](initiative.md): l'anatomia a cinque stadi e lo strato
  post-dispatch mancante; questo piano ne è il primo uso nel registro
  dell'aiuto.
- [`mimic-llm.md`](mimic-llm.md): lo stile è separato e resta separato.
- `LEARN_PROTOCOL.md` §6-bis E: le forme per insegnare la condotta.
- `kb/core/dialogue-policy.p0`, `kb/core/honest-limits.p0`: le mosse e i limiti
  già dichiarati, da riusare prima di aggiungerne.
- [`autoaddestramento-dalla-prosa.md`](autoaddestramento-dalla-prosa.md): lo
  stesso principio (misurare prima, classi non righe), su un'altra facoltà.
