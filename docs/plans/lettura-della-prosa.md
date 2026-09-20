# Lettura della prosa — il miglioramento continuo della comprensione

> **19 settembre 2026 — prima fogliata massiva richiesta da F.** Integrato
> [`english-grammar.p0`](../../kb/core/english-grammar.p0): 10.222 fatti e 67 regole
> di lessico, morfologia, grammatica e pragmatica inglese, zero modifiche C.
> [Inventario, consumatori e limiti](../english-grammar-first-sheet.md).
> Verificati sintassi e caricamento della KB completa. **Esperimenti rinviati
> su richiesta di F.; nessun nuovo punteggio di comprensione.** I cicli
> sperimentali descritti sotto non sono stati avviati in questa consegna.
>
> **Secondo giro, stesso giorno.** F.: i predicati `prose_*` scopizzavano
> conoscenza universale, e soprattutto producevano «predicati non agganciabili
> dalla meta linguistica». Il pacchetto e' `kb/core/english-grammar/`: nomi per
> natura, 1.321 fatti duplicati tolti, e una faccia pronunciabile per ogni
> distinzione (`naming.p0`) — «is water a mass noun?» risponde, e una lettura
> insegnata parlando si puo' richiedere parlando. Resta aperto il costo: le 844
> dichiarazioni `relation_verb` portano `extract_frame` a 2,5 s di
> costruzione e il turno da 0,1 a 0,4 s.

## HANDOFF — 20 settembre 2026, notte: L'IPOTETICO SENZA UN SECONDO CERVELLO

> **La premessa, nelle parole di F.** *Esiste un'evoluzione dell'astrazione
> della KB che produca come effetto una gestione NATURALE di rappresentazioni
> come questa — casi ipotetici, premesse, mondo concreto — e che implichi la
> missione di ridurre enormemente la parte «C» di parrot0?*
>
> **La risposta che questo giro ha trovato: sì, e sono due mosse.** La prima è
> contestualizzare le proposizioni (`holds_in(Contesto, Proposizione)`, K4 di
> [frontier-kb-natural-dialogue](frontier-kb-natural-dialogue.md)): un fatto
> vale in un contesto **senza sparire dagli altri**. La seconda — quella che
> mancava, ed è la chiave — è rendere primitiva la **provenienza della PROVA**:
> una risposta porta con sé *da che cosa è sostenuta*. Allora «le premesse lo
> implicano» e «lo so dal mondo» non sono due motori né due viste: sono due
> **letture dello stesso sostegno**, e la differenza la dice una regola di KB.
>
> **E la domanda di ripartenza, nella sua forma piena (F.).** L'astrazione
> nuova — della KB *e* della IR — deve valere **per generalità**, non solo per
> l'ipotetico: deve ospitare anche ciò che **il mondo non prevede**. Una
> generalizzazione («tutti i corvi sono neri» detta come regola, non come
> fatto), un'astrazione (una classe che nessuno ha ancora popolato), una
> premessa, un **teorema** (qualcosa che vale *dati* certi assunti), un'idea
> temporanea («mettiamo per un attimo che…»), una finzione, una citazione, una
> credenza altrui. Oggi ognuna di queste, quando serve, si ottiene con un
> **trucco locale** — uno strato, un sandbox, un flag, un lettore dedicato — e
> quel trucco è esattamente la parte di C che non si riesce a togliere.
> Il criterio di riuscita è perciò: *una proposizione deve poter valere in un
> contesto qualunque, dichiarato dalla KB, e una risposta deve poter dire in
> quale contesto vale e da che cosa è sostenuta* — senza che il motore sappia
> nulla di «ipotesi», «teorema» o «finzione». Se quei nomi restano in C, siamo
> ancora al trucco.
>
> È questo che riduce il C. Oggi esistono quattro lettori speciali — sillogismo
> in un turno (101 righe), sillogismo multi-frase (82), entailment (76+27),
> catena transitiva, supposizione — e ciascuno esiste **solo per ricostruire un
> mondo ristretto** in cui la domanda abbia la risposta giusta. Con contesti e
> sostegno restano: la IR che dà le clausole, un risolutore, una regola di
> verdetto. I lettori diventano righe di conoscenza.

**Che cosa è stato fatto stanotte.**

1. **Mantra #25 — non si pensa in un secondo cervello.** F.: «ogni astrazione è
   dentro il cervello, non in un cervello secondario; questo crea handicap di
   crescita». La critica era già scritta in [`one-kb.md`](one-kb.md) §3/§6
   («`brain_scratch_init` non va perfezionato: va fatto sparire») ed è tornata
   viva in `MANTRA.md` con il danno misurato: ogni classe portata in KB doveva
   lasciare nel C una lista di riserva, perché nel sandbox la lookup non
   trovava niente.

2. **I quattro sandbox sono spariti**, `brain_scratch_init` compreso. Le
   premesse entrano nello strato `KB_HYPOTHETICAL` della mente unica.

3. **Il passaggio che conta, e l'errore che l'ha preceduto.** Per tenere la
   semantica closed-world avevo aggiunto `kb_read_scope`: un flag in C che
   *nasconde* il mondo. F. l'ha respinto — «deve essere un'astrazione che
   permette la convivenza fra KB e IR, non una differenziazione operativa
   derivata da un flag in C» — ed è stato sostituito dal **sostegno**:
   - `kb_prove_support/6` dimostra e dice **con che cosa** (fatti e regole
     usati, senza la macchineria, che `machinery/1` dichiara);
   - il **giornale** delle asserzioni registra che cosa le premesse hanno
     *detto*, anche quando ripetono ciò che il mondo già sa (riga `=`);
   - il verdetto nasce dal confronto: ogni sostegno viene dalle premesse →
     implicato; qualcuno viene dal mondo → non implicato **da queste premesse**.

   Niente è nascosto: mondo e premesse convivono nella stessa mente, e la
   differenza è **conoscenza in più**, non una vista in meno.

4. **E si fa guardare** (`/debug dump`): `turn_premise_read` dice quale premessa
   è stata presa o rifiutata, `turn_answer_support` su che cosa poggia la
   risposta e se quel sostegno stava fra le premesse.

**Misurato.**

| caso | prima | ora |
|---|---|---|
| `io is a glorp. every glorp is a dax. is io a dax?` | Yes (mondo ristretto) | **Yes** (sostegno: tutto dalle premesse) |
| `rex is a dog. all cats are animals. is rex an animal?` | No (il mondo era nascosto) | **No** (sostegno: `dax`… viene dal mondo) |
| `socrates is a man. all men are mortal. is socrates mortal?` | Yes | **Yes** |
| `taught_lexicon.p0t` | 29/11 | 29/11 (pari) |
| `soft-test` | verde | **verde, 11 s** |
| `entail.p0t` | 13/1 | 10/4 — 3 solo tempo, 1 rosso preesistente |

**Aperto, in ordine di leva.**

1. **I turni ipotetici costano ~2 s**: pensare nella mente piena costa più che
   in una mente vuota. È il prezzo onesto del mantra #25, e si paga una volta
   sola se le premesse smettono di essere riasserite (vedi punto 3).
2. **Il verdetto è ancora in C** (`from_premises ? "Yes." : "No."`). Deve
   diventare una regola: `entailment_verdict(SostegnoKind, Verdetto)` più i
   template, e il C consegna solo i sostegni.
3. **`holds_in` non è ancora la strada delle premesse**: oggi si scrive un
   fatto nello strato e si confronta col giornale. Il passo successivo è che la
   premessa sia *una proposizione che vale in un contesto*, e che il contesto
   sia nominato dalla IR (lo span `condition` lo nomina già).
4. **`kb_read_scope` resta nel motore, inutilizzato dai lettori**: o diventa la
   meccanica di `holds_in` (lettura per contesto dichiarato dalla KB) o si
   toglie. Non deve restare un flag comodo.
5. `entail.p0t:30` — la forma «explain premise: …» non arriva al lettore
   (rosso anteriore a questo giro).

## HANDOFF — 19 settembre 2026, sera (si riprende da qui)

**Che cosa e' successo.** La prima fogliata di grammatica inglese era stata
consegnata senza essere mai eseguita, con i predicati battezzati dalla missione
(`prose_*`). F. ha posto il difetto vero: «lo scoping sui predicati produce
predicati non agganciabili dalla meta linguistica». Il giro ha fatto tre cose.

**1. I nomi.** Il pacchetto e' `kb/core/english-grammar/`; ogni predicato porta
il nome di cio' che descrive. 1.321 fatti duplicati tolti (`plural_of`,
`verb_particle`, `adjective_relation`, `auxiliary`, le forme irregolari ora
derivate da `verb_paradigm/4`, 278 righe di catalogo derivate dalle classi
condivise). `commitment_policy` FUSA con quella di `context-scope.p0`.

**2. La porta meta-linguistica** (`kb/core/english-grammar/naming.p0`). Il
lettore dell'appartenenza interroga il predicato che porta il **nome
pronunciato**, con **arita' 1**: una distinzione tenuta solo in una relazione a
due o tre argomenti esiste e non si puo' nominare. Le facce sono regole sulle
relazioni ricche, non copie. Misurato: «is water a mass noun?» da «I don't know
about mass noun» a «Yes.»; «what kind of verb is comprise?» risponde; «what does
X mark?» risponde, e il giro insegna→chiedi→ritira→non so e' verde.
**La regola da portarsi dietro:** una distinzione nuova va rappresentata nella
relazione ricca *e* affacciata col nome che se ne dice.

**3. La velocita', che era il blocco.** `make test-engine` non passava piu' il
controllo di salute. Quattro cause, tutte misurate (`/debug`, e il nuovo
`PARROT0_BOOT_TRACE=1` che ora stampa il costo di ogni vista, chi la invalida e
chi viene rifiutata):

| | prima | dopo |
|---|---:|---:|
| 6 turni banali | 8,5 s | **1,18 s** (senza il pacchetto: 0,60 s) |
| turno a regime | 1,4 s | **0,20 s** |
| «is however a contrastive connector?» | 5,45 s | **0,28 s** |
| primo turno aritmetico | 1,70 s | **0,19 s** |
| controllo di salute | ROSSO | **verde** |

- `verb_finite_form` risolveva per superficie dentro i cicli sulle particelle →
  vista gemella per radice (`verb_root_form`).
- tre processi di `answer_frame` non erano congelati → `materialized_view` per
  `verb_particle_surface`, `adjective_relation_pred`, `passive_participle_frame`.
- `expression_first_word` era **rifiutata** (raggiungeva `apply`) e quindi
  invalidata da ogni asserzione → `view_apply_resolved(expression_reading)`.
- in C: il pool lessicale e la lista delle cornici si scaldano all'avvio, non
  nel primo turno; `kb_match` ora si fida di una vista congelata (le regole di
  quel predicato non si riespandono: il solver lo faceva gia'); tre lettori
  chiedono le cornici alla lista del cervello invece di rienumerare la KB.

**Aperto, in ordine.**
1. **I turni di LEZIONE costano 2–4 s**: insegnare o ritirare un lemma verbale
   invalida `verb_form_analysis` e con essa `extract_frame` (2,3 s di
   ricostruzione). `english_grammar_growth.p0t` e' **verde nel contenuto, 31
   asserzioni**, e ha 12 turni oltre il budget solo per questo. Il passo giusto
   e' l'aggiornamento INCREMENTALE di una vista (aggiungere le cornici del verbo
   insegnato invece di rifare le 15.000), non alzare i budget.
2. **L'avvio ora costa ~5 s** (`views_warm` 3,6 s + `frame_cache` 1,5 s). E'
   infrastruttura e non un turno, ma cresce con la KB: stessa leva del punto 1.
3. **`conversation/basics.p0t:23`** e' rosso («what is the opposite of hot?»
   letta come asserzione): misurato ANTERIORE, non causato ne' dalla fogliata
   ne' dal motore nuovo. Tracciato in `TEST_TODO.md` §5.3. E' l'unico rosso di
   `soft-test`.
4. **Le letture non ancora affacciate**: 60 famiglie di costruzioni e i ruoli
   (`construction_role`, `scope_requirement`, `pragmatic_candidate`) non hanno
   ancora ne' faccia ne' domanda. Il modello c'e' — si copia da `naming.p0`.
5. Gli **esperimenti di comprensione** della fogliata restano da fare: nessun
   punteggio di prosa e' stato rimisurato in questo giro.

## La pratica dell'ottimizzazione — ottimizzare e' trovare la causa piu' in fretta

F., 20 settembre 2026: «non posso accettare che il debug di "scoprire chi
stampa Held:" non sia banalmente gestito da un `/debug` che ti certifica i
passaggi». E' il criterio giusto, ed e' piu' largo di quanto sembri: in questo
giro **ogni guadagno di velocita' e' venuto da uno strumento che ha accorciato
la distanza fra il sintomo e la causa**, mai da un'intuizione.

**Il conto di questo giro.** Tre ipotesi le ho fatte a naso e la misura le ha
smontate in pochi minuti: l'indice del participio (nessun effetto), il
censimento dei predicati (0,8 ms, non era lui), la ricostruzione delle viste
dentro il turno aritmetico (non c'era). Le cause vere le ha dette uno
strumento, ogni volta:

| domanda | prima | strumento | dopo |
|---|---|---|---|
| quale predicato costa il turno? | si leggeva il C | `/debug` (profilo per predicato) | `answer_frame 364 ms, 27 chiamate` |
| quale vista costa l'avvio? | si sommavano i tempi a occhio | `PARROT0_BOOT_TRACE=1` | `extract_frame 2,4 s` |
| chi invalida quella vista? | si deduceva dalle dipendenze | stesso tracciato | `invalidata da verb_lemma` |
| perche' una vista non si congela mai? | invisibile | stesso tracciato | `rifiutata: il grafo non si chiude` |
| da che cosa dipende? | si leggeva la KB a mano | `PARROT0_VIEW_DEPS=<vista>` | l'arco di troppo, in una riga |
| chi ha detto questa frase? | ~20 letture del sorgente | `/debug`, sonda 35 | `template(learned_opposite) form(teach_opposite)` |

L'ultima riga e' la piu' istruttiva. Il rosso di `basics.p0t` («what is the
opposite of hot» senza «?» risponde «Held: …») e' rimasto aperto per giorni
perche' *nessuno sapeva chi rispondeva*. Con la sonda, la causa si legge in un
turno: e' la forma di lezione `teach_opposite`, che rivendica la frase perche'
la lettura «domanda» del turno non e' ancora pubblicata quando lei decide. Il
tempo speso a costruire la sonda e' meno di quello speso a cercare a mano UNA
volta — e la prossima volta e' gratis.

**La regola operativa.** Quando per rispondere a una domanda sul turno bisogna
leggere il C, quella domanda diventa una sonda: il motore sa gia' la risposta
mentre agisce, e deve depositarla (`turn_said_by/2` e' esattamente questo —
il C aveva gia' `b->turn_frame` dal gen363 e non lo diceva a nessuno).
Vale l'inverso come criterio di maturita': **un progetto e' maturo quando il
debug non e' esplorazione ma interrogazione.** Oggi non lo siamo ancora: le
tracce vivono in variabili d'ambiente diverse (`PARROT0_BOOT_TRACE`,
`PARROT0_VIEW_DEPS`, `P0_FORM_TRACE`, `P0_READ_TRACE`, `PARROT0_TE_SLOW`) e
`/debug` ne raccoglie solo una parte.

**La seconda strategia: il DUMP, non la sonda.** F., stesso giorno: «un
`/debug dump_ir=on` accelererebbe di molto la scoperta di comportamenti non
allineati — un tracciato ricco da cui estrarre incongruenze, invece di muoversi
punto per punto come un debug umano». E' un punto sul METODO, e riguarda chi
legge: una sonda per volta e' il modo in cui indaga una persona, che di dati ne
regge pochi; un agente regge una finestra grande e trova le incongruenze per
INCROCIO — ma solo se qualcuno gliela riempie.

Da qui `/debug dump` (e `turn_reading_predicate/1` in `kb/core/debug.p0`, che
dice quali predicati sono «la lettura del turno»): un blocco solo con gli span
e i loro ruoli, i nodi della IR, le cue, la forza, chi ha parlato. Nel motore
e' servita una primitiva che mancava — `kb_dump_pred`, i fatti come RIGHE
INTERE: `kb_match` raccoglie una colonna, che e' la forma giusta per
interrogare e quella sbagliata per guardare.

Il primo dump ha gia' pagato, e non sulla domanda che stavo indagando: su «is
bob a man?» mostra `input_node_next` popolato e `input_node_atom` /
`input_node_range` **vuoti**, cioe' una IR pubblicata a meta'. Nessuna sonda
mirata l'avrebbe detto, perche' nessuno avrebbe pensato a chiederlo: si vede
solo mettendo le colonne una accanto all'altra.

**Il debito misurato di questa sonda:** `turn_said_by` copre le risposte che
passano dal rendering dei template; «hello» e «what is the capital of france»
non lasciano ancora provenienza. Chiuderlo significa far passare di li' anche
le risposte composte — e' il prossimo pezzo della stessa pratica.

> **Piano vivo.** Non si chiude: si cricchetta. La misura è
> [`scripts/prose-probe.sh`](../../scripts/prose-probe.sh) (`make prose-probe`),
> e ogni giro deve farne scendere l'ultimo numero.
>
> **Aperto il 2026-09-12 (gen513)** su indicazione di F., dopo il primo referto
> del banco: **0 risposte su 7** e **0 su 5** su due prose vere ed esterne alla KB.
>
> **Sta sotto:** [`kb-first.md`](kb-first.md) (la bussola),
> [`universal-comprehension.md`](universal-comprehension.md) (il contratto di
> comprensione), [`universal-input.md`](universal-input.md) (la prosa è un
> registro fra altri), [`extract-knowledge-from-prose.md`](extract-knowledge-from-prose.md)
> (i frame di estrazione come fatti).
> **Porta avanti:** [`the-linguistic-glue.md`](the-linguistic-glue.md),
> [`inferenza-compositiva.md`](inferenza-compositiva.md).

---

## Ripresa operativa — comprensione generale (18 settembre 2026)

**La richiesta corrente è comprendere la prosa in generale.** Chiudere altri
schemi del piolo 300 non basta a dimostrarlo. Il lavoro supera una singola
iterazione: il [percorso esecutivo](lettura-della-prosa-esecuzione.md) divide
l'obiettivo in consegne con dipendenze, file da consultare, prove positive e
negative, condizioni di arresto e un modello di passaggio al prossimo agente.
Si legge prima dell'handoff storico qui sotto; le sue istruzioni operative
risolvono i conflitti fra i diversi aggiornamenti di questo piano.

**Verificato in questa ripresa:** build riuscita; profilo `agi` completo,
58.401 fatti e 4.552 regole al boot. La frase del compost «The decomposition
process is aided by shredding…» e la domanda «What aids the decomposition
process?» producono due muri. Senza profiler: **3.181,8 ms** e **1.622,6 ms**,
una misura per turno, non un benchmark. Nel profilo separato `phrase_canon`
è chiamato **3.052 volte**. [Sonda, log e limiti della diagnosi](../labs/prose-ladder/2026-09-18-ripresa-generale/README.md).
Non sono stati cambiati motore o KB e non è stato rimisurato il piolo intero:
nessun nuovo punteggio di comprensione è certificato.

**Prima consegna: E0, costo del turno**, poi **E1, accordo fra i percorsi di
lettura**. La prosa incollata passa anche da `compound_turn_lead`; `read:`
passa da `extract_clause`. Esistono già `document_unit`, claim attribuite e
revisioni delle letture: prima di crearne copie, verificare quali viste i due
ingressi condividono davvero. I nomi e i limiti sono nell'inventario di E1.

**Correzioni operative:** niente banco lungo all'apertura, niente bisezione
con stash/checkout, niente test concorrenti alla misura di latenza. Un timeout
è un difetto da localizzare, non automaticamente un costo preesistente. Le
vecchie mosse M1, M7, M8 e M13 vanno lette con le rettifiche sotto.

## ⛔ HANDOFF storico — 18 settembre 2026, notte

### ⛔ DIAGNOSI DELL'OPERATO — 18 settembre 2026, notte (F.: «non siamo stati efficienti»). OBBLIGATORIA, si legge prima di tutto

F.: *«se una cosa è lenta non la paghiamo: ci fermiamo. Le cose lunghe sono
bug. Se avessi studiato il progetto avresti scoperto che `!reset` ha uno stato
di misurazione dei cambiamenti … ho criticato più volte l'uso dei timeout come
cerotto e ora vedo test con timeout da 60 secondi … invece di vederti mandare
la prosa e capire come addestrare, vedo che esegui script di test».*

**Verificato sui fatti del progetto, non a memoria:**

| tesi | verifica | esito |
|---|---|---|
| `!reset` è smart | `src/testeng.c` §115–206, `docs/plans/test-engine.md` §3: salta se config invariata **e** nulla è stato imparato | vero. Ma ogni caso di lettura della prosa *impara*, quindi ogni reset ricarica: **28 reset in `prose_triage.p0t`** |
| il reset è il costo | misurato: un boot intero della KB viva costa **0,40 s** (`printf /quit \| parrot0`) → 28 reset ≈ 11 s su ~150 s del file | **falso**: il costo sono i TURNI, non i reset |
| dove va il turno | `/debug` sulla frase delle stime: **1398 ms** (754 nel solver, 644 fuori, 17 ricostruzioni d'indice); `phrase_canon` **295 ms per 2499 chiamate** su una frase di 30 parole, `input_frame_observe` 155 ms in 1 chiamata, `turn_bookkeeping` 113 ms; la domanda **813 ms** (527 **fuori** dal solver) | il turno costa 1,4 s perché una tabella di locuzioni viene riletta ~2500 volte per frase (mantra #20a: *che cosa rilegge la classe e quante volte*) e perché il C fuori dal solver pesa più del solver stesso: **è un bug, non un costo base** |
| F. ha criticato i timeout come cerotto | `da-parola-a-stato.md` §tabella («alzare i !timeout o togliere casi per far passare un gate: i timeout misurano il motore, non il test»), `frontier-kb-natural-dialogue.md` §CEROTTO (2341) e 1282/1462, `TEST_TODO.md` 93/302/457, `insegnamento-super-umano.md` 355, `universal-code-comprehension.md` 1165, `MANTRA.md` 396, `CLAUDE.md` 21 | vero, **nove volte scritto**. E `prose_triage.p0t` porta 12 `!timeout 60`, 7 `!timeout 90`, 9 `!timeout 30`: sei dal 13 settembre, **quattro aggiunti stanotte da me** copiando il pattern del file invece di chiedermi perché un turno di lettura dovesse poter durare un minuto |

**Il conto del tempo di stanotte (≈ 3 h di parete):** 3 rilanci del banco (r300 ×3,
r340 ×2, r320 ×1: ~40 min di macchina, in background ma con il soft-test che vi
girava sopra e segnava 1,75 s), `prose_triage.p0t` **sette volte** (~2,5 min
l'una ≈ 18 min), `taught_question_qualifier` ×4, `basics` ×3, soft-test ×2, una
bisezione con checkout di `kb/` e `src/` (6 min), venti minuti a inseguire un
rosso apparso aggiungendo tredici verbi (la stessa frase sondata da sola
rispondeva bene), e uno `stash` sbagliato che ha perso i tredici verbi dal
working tree. **Tempo passato con la prosa davanti, a insegnare: meno di
venti minuti.** KB vera lasciata: 82 + 32 + 4 righe, più 3 di forma; tredici
verbi da riscrivere.

**Le regole che ne escono, obbligatorie da qui in avanti:**

1. **Lento = bug = ci si ferma.** Un turno sopra 1 s, un file di test sopra il
   minuto, un banco sopra i 10 minuti non si *pagano* e non si *aspettano*: si
   apre `/debug`, si scrive nel registro la riga dominante (predicato, ms,
   chiamate) e si decide se curarlo adesso o scriverlo come circuito. Il
   profilo di stanotte è il primo: `phrase_canon` 2499 chiamate per frase.
2. **Nessun `!timeout` sopra 1 s in un test nuovo.** Se un caso ne ha bisogno,
   il commit porta la riga di `/debug` che dice perché, e il caso va nel
   registro dei turni lenti, non nel banco. I 28 budget lunghi di
   `prose_triage.p0t` sono debito da smontare, non pattern da copiare.
3. **Prima di usare uno strumento del progetto, leggerne il contratto**
   (`test-engine.md` §3 per `!reset`, `Makefile` per `soft-test`, `debug.p0`
   per `/debug`): un'ora di questa notte è stata pagata per non averlo fatto.
4. **La prosa si lavora in chat, non con i `.p0t`.** Il ciclo è: una sessione
   aperta, la prosa dentro, la domanda, la lezione parlando, la domanda di
   nuovo; il `.p0` e il commit. Il `.p0t` si scrive **una volta** a fine giro
   come contrasto e si lancia **una volta**; il banco del piolo si lancia una
   volta a fine sessione. `make soft-test` è la sola verifica dentro il ciclo.
5. **Niente stash, niente cambio di branch, niente bisezione con HEAD** (F.,
   stessa notte: *«switchare branch, fare stash non si fa: è un costo che non
   possiamo pagare; tu non devi verificare regressioni con HEAD, non ti serve;
   devi procedere facendo crescere la KB; ci sarà una fase post-training,
   stabilita da me, di fix delle regressioni»*). Un rosso che compare mentre
   la KB cresce si **scrive** in `TEST_TODO.md` con la frase che lo mostra e si
   va avanti: non è la sessione di crescita a doverlo chiudere.
6. **La suite non si lancia mai.** Al massimo **qualche test puntuale** per
   capire un effetto, e uno solo per volta. `make soft-test` compreso solo
   quando serve una risposta in 15 s. Il banco del piolo: una volta a fine
   sessione, in background, come conferma.
7. **Ogni cosa che dura più di 2 minuti è un problema** (F.): sta
   rallentando la crescita. Il lavoro dell'agente è **inferenza** — leggere
   il muro, capire la lezione, modificare il file, committare, pushare — non
   attesa. Se un comando supera i 2 minuti, si interrompe e si scrive perché
   (`/debug`, una riga) invece di aspettarlo. L'unità di misura della sessione
   è: righe di KB vera per ora, e commit utili per ora.
8. **Per rendere il giro fluido (indicazioni mie, da contraddire se non
   fanno risparmiare):** una sola sessione di chat aperta per tutta la
   sessione, con la prosa dentro, così la lezione si verifica sul testo vero
   senza reboot; il `.p0` ufficiale si scrive **subito** dopo il «Learned/Held»
   (la sessione di chat non persiste: `PARROT0_SESSION=`); il messaggio di
   commit nomina il muro, la lezione e la riga; si tengono a portata i tre
   comandi che costano meno di un secondo — la sonda per frase, `who
   answered?`, `/debug` — e nessun altro; il referto del piolo si legge una
   volta all'inizio (i muri sono la coda di lavoro) e una volta alla fine.

### Dove siamo, in una tabella (KB viva, banco esteso, colonna dei moduli)

| piolo | merito | meta | struttura | cancello | chi risponde | furti residui |
|---|---|---|---|---|---|---|
| r300 (299 parole) | **48/62** (referto `-2300`; era 50/62) | 2/2 | 5/5 | ✅ 301 > 299 | `answerframe` 48 | **0** furti; **−2** («what threatens coral reefs?», «…under threat from?») **pre-esistenti** al secondo turno: bisezione per file → compaiono con i 13 verbi ripristinati in `75da5e50` (`relation_verb(threaten)`), non con le regole della notte; in `TEST_TODO.md` |
| r320 (319) | **19/68** atteso dopo il ritiro (referto `-2318` con la guardia: 16/68, +1 «what aids the decomposition process?» −3 gerundi; da rimisurare) | 2/2 | 5/5 | ⛔ 105/319 | | `answerframe`×4 («soil fertility», «plant nutrients», definizione di *matter*), `knowledge`×2 (procedura del rame, «compost is a mixture»); il «why» è ora un muro onesto |
| r340 (338) | **5/65** | 2/2 | 5/5 | ⛔ 25/338 | `answerframe` 4, `knowledge` 1 | `knowledge`×4 (lettura), `answerframe`×2 (definizioni), `analysis_family`×1 (seconda passata) |
| i100 italiano (108) | **0/23** | 2/2 | 5/5 | ⛔ 0/108 | | lessico insegnato (37 `tr/2`); blocco: nome canonico lettura↔domanda |

Referti: `docs/labs/prose-ladder/referti/r300-2026-09-18-2300.txt` e
`r320-2026-09-18-2318.txt` (misura prima del ritiro; `session-2026-09-18-2320.log`
è il log di sessione, non un nuovo referto), `r3*-2026-09-18-2102.txt` (con la porta
del qualificatore), `…-2044.txt` (la baseline della stessa notte); i furti in
`docs/labs/prose-ladder/furti.tsv` (quattro righe nuove dal secondo turno). I referti storici della giornata stanno in
`docs/labs/apprendimento-assistito/2026-09-18-regressione-e-banco/`.

**Come si cresce, da qui in avanti: §4-sexies** (giri di cinque minuti,
muro → lezione parlando → `.p0` → commit; la regola sul C). ~~Primo giro della
prossima sessione: il canale #1 che non passa per metà dei verbi~~ ✅ fatto
(notte, secondo turno, §6). ~~Primo giro della prossima sessione: la flessione
del verbo nel ponte della domanda~~ ✅ giro 2 (§6). Prossimo: i muri del
referto r320/r340 in ordine (handoff, residui 2 e 5).

**Strumento storico, non avvio automatico: `make prose-session`** (o `scripts/prose-session.sh
r300 r320`): stampa questa tabella, l'ultimo giro del §6, gli ultimi referti e
furti, ricompila, riavvia il demone e rimisura i pioli in background. Questo
comportamento confligge con i limiti di durata fissati nell'ultima diagnosi:
per la ripresa corrente usare E0 del percorso esecutivo, senza avviare i pioli.

### Il giudizio, non il numero

Il 50/62 del piolo 300 è quasi tutto **una facoltà sola** (`answerframe`, specie
B: relazioni e forme in KB, pretesa su cue, zero usi del frame) che risponde su
fatti estratti da **schemi** (`extract_frame/2`), non da una lettura della IR.
Sul piolo nuovo lo stesso apparato fa 5/65 e in italiano 0/23. Le uniche
risposte che sono un atto cognitivo nel senso pieno — leggono la IR e compongono
— sono meta e struttura (`text-structure.p0`, 100% su ogni piolo) e i
consumatori KB come `event-time.p0`: piccoli, ma sono la forma che scala. Il
piano dei furti (`turn-arbitration.md` §1-bis.1-ter) ha reso i muri più onesti,
non la lettura più profonda.

### Comandi di ripresa (5 minuti, non 40)

Comandi storici; **non eseguire questo blocco come sequenza di apertura**.
La sonda breve e il profilo di E0 sostituiscono il rilancio dei pioli. La
verifica finale completa si fa solo quando rientra nei limiti di durata;
un'esecuzione interrotta non certifica il piolo.

```sh
make build && make test-engine
scripts/prose-rung.sh r300 r340 it:i100        # in parallelo: referto datato, diff col precedente, furti nel registro
scripts/prose-bench-coverage.py tests/fixtures/prose/ladder/r3*.txt   # il banco copre tutte le frasi?
python3 tests/tools/module_review.py           # il cricchetto delle review (17/82, 8 retrocessi)
P0_READ_TRACE=1 <sonda per frase> + «who answered?»   # la diagnosi, prima di ogni cura
```

### I residui, dal più fertile (uno per sessione, banco prima della cura)

1. ~~**`answerframe` sulla domanda letta intera**: il qualificatore~~ ✅ chiuso
   la notte del 18 (`question_qualifier/2`, vedi §6). **Resta la seconda metà
   della stessa voce**: la definizione della parola nota dentro una domanda più
   lunga («what is the temperature OF CARBONIZATION?» → la definizione di
   *temperature*; «what is charcoal made of carbon…» → il ciclo del carbonio).
   È lo stesso gesto — il resto della domanda («of carbonization») non è
   coperto dal valore — ma con «of» il qualificatore sta spesso *dentro* la cue
   («the capital OF france»): serve la testa del sintagma, non un'apertura.
   **E il prezzo della porta, da pagare per primo**: la lettura scarta
   l'avverbiale di tempo («estimated at 5 million IN 2010» → `estimated_at(…,
   5_million)`, niente tempo), quindi «…estimated at in 2010?» che prima
   rispondeva per fortuna ora mura. Il circuito: l'avverbiale di tempo e la
   parentetica con cifre («(a 2020 estimate)», oggi tolta da
   `strip_annotation_parentheticals`) diventano un qualificatore del FATTO
   letto, e la porta lo prova su quello invece che sulla superficie del valore.
   Con quello, «in 2020» sul piolo 300 risponde «US$2.7 trillion».
2. **I quattro difetti di lettura di `knowledge`** (registro dei furti): la
   frase sbagliata a parità di costruzione («in regions like»: Central Europe vs
   South America), la definizione al posto della relazione, «scores applications
   on diverse», la procedura del rame. Mantra #23: che cosa manca alla KB per
   distinguerli.
3. **Il canonicalizzatore italiano**: la locuzione più lunga vince sulla parola
   (`carbone_vegetale` vs `coal_vegetale`), «dal» non è un luogo, «è» come
   confine di sintagma nella IR. Poi si rimisura i100 (le 37 traduzioni sono
   già in `gloss.p0`).
4. **`analysis_last_resort` attraverso la porta sottile**, e il costo della prima
   passata (+0,5 s sui prompt d'analisi: `analysis_planner_growth.p0t` 23 timeout
   su budget 1 s, non alzato): si profila, non si indovina.
5. **Le forme nuove del 340**: relative ridotte lunghe, «involves + gerundio»,
   «led to», «aimed to maintain», participi con agente.

### ⛔ LA MISSIONE SECONDARIA: efficientare il processo DELL'AGENTE — stato onesto

F. (18 settembre, notte): «non ho visto maturare nulla in questo senso», e poi:
«non lo scripting: i processi logici che tu come coding agent metti in campo;
come scopri le cose, come parrot0 potrebbe riorganizzarsi o darti evidenze, le
mosse standard che hanno dato evidenza». **Il soggetto è l'agente.** Quello che
è maturato in quel senso sta in §4-quater.4–7: quattordici mosse con l'evidenza
che le ha promosse, la scala di diagnosi che le ordina (il C si apre solo
all'ultimo gradino), sei cose che parrot0 deve arrivare a dirmi da solo, e gli
anti-pattern con il loro costo. La prossima sessione le usa e le contraddice:
una mossa che non fa risparmiare esce. Quanto segue sugli strumenti è un
sottoprodotto, e resta scritto perché costa poco tenerlo.


**Che cosa esiste** (§4-quater): la tabella delle evidenze di processo (11 righe
misurate), le regole anti-malizia, `prose-diff.py` (che cosa cambia fra due
referti), `prose-bench-coverage.py` (ogni frase ha una domanda), la colonna dei
moduli e i furti per modulo nel banco (`P0_PROBE_WHO=1`), la raggiungibilità del
cancello stampata dal banco, i pioli in parallelo, i contatori del banco che non
ripetono il difetto del lettore, e da stanotte **`scripts/prose-rung.sh`**: un
piolo, un comando — banco con i moduli, referto datato in
`docs/labs/prose-ladder/referti/`, diff col referto precedente dello stesso
piolo, furti accodati a `docs/labs/prose-ladder/furti.tsv`, più pioli in
parallelo.

**Che cosa NON è maturato, e perché si vede**: il ciclo della sessione (§4-quater.3,
punto 1) è ancora una **lista scritta**, non un comando; i referti di oggi sono
stati copiati a mano in `docs/labs/…/2026-09-18-…/`; il registro dei furti in
`turn-arbitration.md` è una tabella scritta a mano, mentre `furti.tsv` è appena
nato e vuoto; la calibrazione a freddo si ripaga a ogni piolo (una sessione
intera) anche se la KB non è cambiata; le domande in italiano e le review le
scrive una persona (per scelta, anti-malizia 2 — ma il tempo va misurato); e
nessuna evidenza di processo si aggiunge da sola alla tabella. Il tempo
misurato di questa sessione: ~40 min per arrivare alla prima misura, ~10–20 min
per piolo con la colonna dei moduli, ~8 min per certificare una riga di KB.

**I prossimi tre gradini, misurabili** (uno per sessione, come i circuiti):

1. ~~**`scripts/prose-session.sh`**~~ ✅ **fatto la notte del 18**
   (`make prose-session`, `RUNGS="r300 r320"`): stato dal piano (tabella
   dell'handoff, ultimo giro del §6, ultimi referti e furti), `make build`,
   `make test-engine`, rimisura dei pioli in background con il log in
   `docs/labs/prose-ladder/referti/session-<data>.log`. Misurato a secco
   (`P0_SESSION_NOBENCH=1`): ~40 s; la prima misura arriva quando finisce il
   piolo, ~10 min dopo, e intanto si sonda. Gate raggiunto: 5 min di parete
   per essere in condizione di lavorare, contro i 40 del 18 mattina.
2. **La calibrazione a freddo con memoria**: le risposte a freddo di un piolo
   si conservano con la firma della KB (hash di `kb/` + del binario); si
   rifanno solo se la firma cambia. Gate: *un piolo con KB invariata costa una
   sessione, non due.*
3. **Il registro dei furti come sorgente unica**: la tabella di
   `turn-arbitration.md` si genera da `furti.tsv` + `module-review.p0` (modulo →
   specie → stato), invece di scriverla a mano. Gate: *ogni furto misurato ha
   una riga senza che nessuno la scriva.*

Il criterio per dire che la missione secondaria è matura resta quello del
§4-quater: **le sessioni successive costano meno di questa a parità di lavoro
cognitivo**, e lo dice la tabella delle evidenze, non un giudizio.

---

## 0-bis. LA MISURA E LA MISSIONE: da 12–15 verso 100 (F., 18 settembre 2026, notte)

**La scala di F.** 0 = *legge ma non capisce: le risposte non si possono
considerare un atto cognitivo*. 100 = *sarebbe in grado di rispondere a
qualsiasi domanda, cognitivamente*. Non misura quante domande passano: misura
**da che cosa** vengono le risposte.

**Dove sta parrot0 il 18 settembre 2026: 12–15.** Le ragioni, perché la
prossima sessione possa contraddirle con una misura:

- *non meno*, perché esiste un circuito che è un atto cognitivo pieno: la IR
  del testo letta da regole KB che compongono la risposta — meta e struttura
  (100% su ogni piolo, anche in italiano), `event-time.p0`, `decisions.p0`.
  Piccolo, ma è la forma che scala;
- *non più*, perché il 49/50 del piolo 300 viene tutto da `answerframe` sopra
  fatti estratti da schemi (`extract_frame/2`) e ritrovati per cue: un frasario
  ben fornito, non una lettura. La prova è il trasferimento — 5/65 sul piolo
  nuovo, 1/12 sulle domande nuove del 300, 0/23 in italiano anche col lessico —
  e le risposte confidenti e sbagliate (qualificatore ignorato, parola nota
  che vince sulla domanda), che un sistema a 30 non darebbe.

**Report della notte del 18 settembre (sessione di due ore): 12–15 → 13–16.**
Che cosa è cambiato, sulla scala di F. (*da che cosa vengono le risposte*):

| evidenza | verso | peso |
|---|---|---|
| tre risposte confidenti e sbagliate del piolo 300 (+ il «why» del 320) sono muri onesti **che nominano ciò che manca** («I don't know about «in 2020» here: what I read is that …»), chiusi per **tre classi** (tempo, porzione, causa) e non per superficie | ↑ | è la voce «una bugia in meno per specie» della tabella sotto; un sistema che risponde a un «why» con l'oggetto del verbo sta più in basso di uno che dice di non sapere il perché |
| la classe si insegna parlando e si ritratta (`taught_question_qualifier.p0t`) | ↑ | conoscenza, non codice: cresce senza ricompilare |
| `answerframe` legge il **resto della domanda** (specie B → un passo verso A): quattro punti di emissione, una pretesa | ↑, piccolo | la pretesa resta sulla cue e la prova è sulla superficie del valore, non sul frame della IR |
| nessuna risposta in più viene da una lettura; il 50/62, 19/68, 5/65 sono identici | = | il numero non sale, ed è giusto così: la cura era sulla bugia, non sulla lettura |
| la lettura scarta l'avverbiale di tempo, e la porta lo rende visibile (una variante che rispondeva per fortuna ora mura) | = (onesto) | non è una perdita di comprensione: è una fortuna in meno; ma dice dove sta il prossimo circuito |

**Perché +1 e non di più**: il criterio della scala è «da che cosa viene la
risposta», e stanotte nessuna risposta nuova viene dalla IR. **Perché +1 e non
zero**: la scala punisce le risposte confidenti e sbagliate («che un sistema a
30 non darebbe»), e la classe più numerosa di quelle sul piolo 300 è chiusa
per specie e insegnabile. Che cosa lo porterebbe a 18–20: il tempo come
qualificatore del **fatto letto** (allora «in 2020» risponde «US$2.7
trillion» da lettura, e la stessa porta lo prova sul fatto, non sulla
superficie), e la pretesa di `answerframe` su `turn_declared_act(question)`.

**Report della notte del 18 settembre, secondo turno (22:11–23:40): 13–16 → 14–16** (il passivo dalla radice è stato ritirato a fine sessione: la riga della morfologia vale per -ies e per la domanda, non per la lettura del passivo).
Quattro giri, quattro commit (`2fa93830`, `65d2f593`, `363fc74a`, `79eb1927`),
**zero righe di C di lingua** (il C committato: la chiave di una cache che vede
il turno, una nota di turno per `/debug`, un ramo «lo so già» che rende un
template KB). Sulla scala di F. (*da che cosa vengono le risposte*):

| evidenza | verso | peso |
|---|---|---|
| **il canale della crescita parlando dice il vero**: «X is a relation verb» per una parola già in classe murava (9/13 dei verbi dei muri) e ora conferma («I already know that…»); una lezione ripetuta o una seconda classe passano; la decisione è una condizione in più su una regola gen513, non un ramo C | ↑ | non è una risposta in più: è il canale su cui ogni risposta futura arriva per lezione, che prima perdeva metà delle lezioni in silenzio |
| **la morfologia come conoscenza**: -y/-ies e il participio regolare nei due versi (lettura, domanda, passivo dalla radice), validi per ogni verbo **insegnato parlando** («cats zorblies mice» → «what do cats zorbly?»; «mice are zorblied by cats» → «what are mice zorblied by?») | ↑ | risposte che vengono dalla **forma** letta e dalla relazione, non da una cue: «what does compost supply?» era un muro del referto 320 |
| **un fatto falso evitato per specie**: «by + gerundio» è un mezzo, non un agente; il gerundio di un verbo di relazione è un confine (`gerund_of` trova il consumatore che il giro 1 aveva lasciato aperto) | ↑ | mantra #7: il fatto falso da un testo vero è il furto più grave, e questo lo chiude per classe |
| **quattro stemmi falsi tolti** dalla KB (`supplie`, `applie`, `carrie`, `occupie`), residui di una vista salvata | ↑ piccolo | meno conoscenza sbagliata a riposo |
| le risposte in più sui pioli: vedi la tabella dell'handoff (referto `-2300`) | = / ↑ piccolo | i muri restanti del 320 (parentetica-definizione, avverbio nel passivo) non sono di morfologia |
| **tre furti nuovi registrati** (`furti.tsv`): `knowledge` risponde a un'AFFERMAZIONE con «Grass is green…», `semantic_lead` su «green waste», la parentetica letta come «10 prerequisiti»; e `arith` risponde «0.025.» alla frase 8 del 300 | = (onesto) | nominati, non curati: il registro cresce, la bugia no |
| **il turno di lettura costa ~3 s da prima** (30 parole dopo un saluto; `input_frame_observe` 1,3–1,6 s), e ogni turno base 1,0–1,2 s: tutti i `.p0t` a budget 1 s sono rossi per costo | = | non muove l'ago ma fissa il prossimo circuito del motore; la diagnosi in testa (1398 ms) era su un'altra frase |

**Perché +1 e non di più**: nessuna facoltà è passata da B ad A, e le risposte
in più vengono da forme (morfologia) sopra la stessa cornice. **Perché +1 e
non zero**: la scala punisce la bugia, e stanotte ne sono chiuse due per specie
(la lezione che «passa» senza passare; l'agente-gerundio), entrambe insegnabili
e ritrattabili; e la crescita per lezione ora vale per la morfologia intera di
ogni verbo, non per la forma detta. Che cosa lo porterebbe a 18–20 resta
quello scritto sopra (il tempo come qualificatore del fatto letto; la pretesa
di `answerframe` sul turno) più il circuito del motore: l'osservatore dei
frame che indicizza gli schemi per parola d'ancora invece di provarli tutti.

**La missione primaria è massimizzare questa misura.** Ogni sessione dichiara
nel §6 dove ha mosso l'ago e con quale evidenza; la regola di lettura è:

| muove l'ago | non lo muove (anche se il numero sale) |
|---|---|
| una risposta in più che viene da una **lettura** (IR, frame, forza del turno) e vale sul piolo **non** toccato | una risposta in più da uno schema o da una cue, misurata solo sul testo che l'ha fatta nascere |
| una bugia in meno per **specie** (registro dei furti) | una cessione in più per un modulo senza titolo |
| una facoltà passata da B ad A (pretende sulla lettura) | un modulo in più che «copre» un turno |
| un piolo nuovo che parte più alto senza cure | un piolo vecchio che sale con cure locali |

---

## 0. La tesi (F., 12 settembre 2026)

> «Il concetto generale è che dovrebbe essere usata la **comprensione universale**
> e il **modello IR** per incamerare la prosa e poi poterla lavorare in tutte le
> infinite forme che una prosa può rappresentare: codice, documenti, teorie
> matematiche, ecc. Sono infinite le cose che una prosa può rappresentare, e
> anche in maniera **mixata** una stessa prosa può spaziare. Quindi il concetto è
> che **la IR deve essere generale**.»

Detto come contratto operativo:

> **Una prosa si incamera UNA VOLTA, in una rappresentazione intermedia che non
> sa a che cosa servirà. Chi la lavora — il lettore di fatti, il ponte verso il
> codice, il costruttore di documenti, il solutore matematico, il pianificatore —
> è un CONSUMATORE di quella rappresentazione, non un secondo lettore della
> stringa.**

E il corollario che rende la tesi falsificabile:

> **Una forma nuova che la prosa può rappresentare deve costare un CONSUMATORE
> dell'IR, mai uno scanner nuovo della superficie.** Se per leggere le teorie
> matematiche bisogna riscrivere un tokenizzatore, la IR non era generale.

Perché è la mossa giusta e non una preferenza estetica: le forme che una prosa
può rappresentare sono **infinite e miste** — lo stesso paragrafo di un manuale
porta una definizione, una formula, un vincolo di sicurezza e un passo di
procedura. Un lettore per forma moltiplica per il numero delle forme; un'IR
sola, con N consumatori, somma.

---

## 1. Dove siamo davvero (misurato il 12 settembre, non ipotizzato)

### 1.1 L'IR **esiste già**, ed è meglio di come viene usata

`kb/core/input-structure.p0` (gen438) dichiara una gerarchia osservata
dell'input:

```prolog
input_node(Scope, Id, node(Level, Kind, Parent), range(Start, Len))
input_node_surface(Scope, Id, "Surface")
input_node_role(Scope, Id, Role)
```

con sopra una cinquantina di viste interrogabili — entità, classi, unità,
operatori, coordinazioni, lacune, numeri, ordine fra nodi, frame semantici,
asserzioni binarie, provenienza di clausola. Non è un abbozzo: è un albero con
range di byte e ruoli, che conserva la catena *superficie + lingua → concetto*
prima che la canonicalizzazione perda la forma.

### 1.2 Il problema non è che manca: è che **quasi nessuno la consuma**

| | |
|---|---|
| file di KB che consumano l'IR | **7** |
| `split_words(...)` — riscansioni della stringa grezza nei tre lettori maggiori | **217** (`10-memory-knowledge.c` 130, `25-wordmath-reasoning.c` 54, `30-generation-reading.c` 33) |

Ogni `split_words` è un lettore che **riapre la frase da capo** con la propria
idea di dove finiscono le cose. Non è duplicazione innocua: è il motivo per cui
due lettori dello stesso turno sono in disaccordo su che cosa c'è scritto, e
perché il primo che afferra il turno vince.

### 1.3 Il referto del banco, il 12 settembre

`make prose-probe` su due lead veri di Wikipedia, esterni alla KB
(`tests/fixtures/prose/`, fonti in `SOURCES.md`), con domande la cui risposta è
**scritta nel testo**:

```text
tardigrade   0 risposte su 7
quipu        0 risposte su 5
```

Il numero zero non è l'informazione utile. L'informazione utile è il **passo 1**
del banco — ogni frase da sola, e non «ha risposto?» ma «**che cosa ne ha
capito?**».

---

## 2. Le quattro malattie, e perché sono la stessa

| # | la frase (prosa vera) | che cosa ne fa oggi |
|---|---|---|
| **M1** | «Tardigrades, **also known as** water bears or moss piglets, **are a phylum of eight-legged segmented micro-animals**.» | `Learned: tardigrades also known as "water bears or moss piglets are a phylum of eight-legged segmented micro-animals"` |
| **M2** | «They were **first** described by the German zoologist … in 1773» | «I am not sure what you mean by «first».» |
| **M3** | «…named them Tardigrada, **which means** "slow walkers".» | «I found the teaching pivot, but I cannot align the same variables…» |
| **M4** | «Quipu, also spelled khipu, are record-keeping devices…» | «**Quipu was a mysterious Quipu. Then one day, quipu discovered what it meant to be seen…**» |

Sono quattro sintomi e una malattia sola: **nessuno di questi lettori ha
consultato l'IR.**

- **M1 — l'apposizione non ha una fine.** Chi legge «also known as» prende tutto
  il resto della riga come valore. L'IR sa dove finisce l'apposizione: le due
  virgole sono un nodo con il suo `range`. Il lettore non gliel'ha chiesto.
  **È la specie peggiore**, perché un fatto storto entra in KB e non si lamenta —
  un fatto invece di quattro, e nessun segnale.
- **M2 — una parola prende il turno.** Un ordinale dentro la frase («first»)
  dirotta il turno sul chiarimento, e una frase attributiva al passato non viene
  letta mai. L'IR sa che «first» è dentro un avverbiale di una passiva, non è la
  domanda del turno.
- **M3 — una relativa letta come lezione.** «which means» è il perno di una
  lezione sulle parole **quando è il turno a essere una lezione**; dentro una
  frase dichiarativa è una relativa. Chi l'ha presa non ha guardato in che nodo
  si trovava.
- **M4 — chi PRODUCE prosa risponde a chi la PORTA.** Il generatore di racconti
  si prende un turno che portava un testo da leggere. Non è un difetto di
  lettura: è una **condotta di cessione mancante**, ed è il più economico da
  chiudere perché è KB pura.

> **La regola che ne esce, e che vale per ogni giro futuro:** quando un lettore
> sbaglia un confine — dove finisce un valore, dove comincia una clausola, a che
> cosa si riferisce un pronome — **la cura non è aggiustare quel lettore: è
> fargli chiedere all'IR il confine che l'IR conosce già.** Aggiustare il lettore
> è il modo in cui si arriva a 217 scanner.

---

## 3. La forma d'arrivo

```text
                       ┌──────────────────────────────┐
   prosa  ──────────▶  │  UNA lettura, UNA IR         │
   (detta, letta,      │  input_node/4 + ruoli + range│
    scaricata,         └──────────────┬───────────────┘
    da un file)                       │
                    ┌─────────────────┼─────────────────┬──────────────┐
                    ▼                 ▼                 ▼              ▼
              fatti e classi      modelli/codice     documenti      piani, prove,
              (mod_knowledge)     (model-bridge)     (document-*)   matematica, …
```

Tre proprietà che la rendono *generale* invece che *ampia*:

1. **L'IR non sa a che cosa servirà.** Registra struttura — nodi, ruoli, range,
   lingua — non intenzioni. Nel momento in cui l'IR contiene un nodo `formula`
   perché qualcuno vuole generare codice, ha smesso di essere generale.
2. **I consumatori si dichiarano, non si compilano.** Un consumatore nuovo è un
   insieme di regole KB sopra le viste dell'IR, come
   `kb/core/model-lesson.p0` è un consumatore per i modelli. Il gen513 lo ha già
   dimostrato: quel lettore è **tutto in KB, zero righe di C**, agganciato a
   `bookkeeper/1`.
3. **Le forme miste non sono un caso speciale.** Se la stessa frase porta una
   definizione e una formula, due consumatori la leggono e ne ricavano due cose
   diverse. È possibile solo se nessuno dei due «consuma» il turno impedendo
   all'altro di vederlo — cioè solo se leggere non è dispatchare.

> **Il confine (mantra #7, PRINCIPLES anti-inganno).** Una comprensione parziale
> si DICHIARA. Un'IR che riempie i buchi per essere completa produce fatti
> plausibili e non verificabili, che è il danno peggiore di un muro. Il banco
> misura le risposte giuste, mai «quanto sembra aver capito».

---

## 4. Il metodo: come si cricchetta

1. **Il banco prima della cura.** Si fissano attese e contrasti prima della
   modifica; nella ripresa si usa l'ultimo referto e una sonda breve, non si
   rilancia automaticamente `make prose-probe` (rettifica della diagnosi del
   18 settembre). Il banco completo conferma a fine giro se il costo lo permette, e
   il testo su cui si lavora va aggiunto a `tests/fixtures/prose/` **con la
   fonte**. Prosa vera ed esterna alla KB: una risposta giusta su prosa che
   parrot0 già conteneva non prova niente.
2. **Il passo 1 vale più del passo 2.** «Che cosa ne ha capito» dice la specie
   del limite; «ha risposto» dice solo che c'è.
3. **Una malattia per giro**, con la prova di chiusura scritta prima.
4. **Ogni cura si chiede: è un consumatore dell'IR o è uno scanner nuovo?** Se è
   uno scanner, si è appena aggiunto il 218°.
5. **Un fatto storto conta come un fallimento più grave di un muro**, e va
   contato a parte: M1 è peggio di M2.
6. **Niente suite.** Il banco è una sonda e si lancia a mano (politica dei test
   di F.).

---

## 4-bis. E QUESTO PIANO E' IL VEICOLO PER PORTARE IL C IN KB (F., 12 settembre)

> F.: «ricordiamo che questo piano deve essere anche un'occasione per migrare
> codice C in equivalente addestrabile su KB con il principio KB-first».

Non è un requisito aggiunto: è **la stessa cosa detta dall'altro lato**. I 217
`split_words` non sono soltanto la causa delle quattro malattie — sono anche
**l'inventario del C da portare via**. Ogni lettore che smette di riscandire la
stringa e comincia a consultare l'IR è, nello stesso atto, una riga di motore in
meno e una regola insegnabile in più.

Il gen513 lo ha già dimostrato due volte, e le due prove vanno tenute come
modello di misura:

| cosa | com'era | com'è |
|---|---|---|
| il lettore dei modelli (`model-lesson.p0`) | non esisteva; sarebbe stato un ramo in C | **un lettore intero in KB, zero righe di C** — copula, foglie, operatori infissi e prefissi, fold, risposta — agganciato a `bookkeeper/1`, che era già una porta |
| il ponte modello→codice | 4 righe in C**…**in KB che sapevano il nome `law_formula` | `model_carrier/1` + `apply/2`: non conosce nessun predicato |

### La disciplina, per ogni giro

1. **Prima si cerca la porta che esiste già.** `bookkeeper/1`, `turn_response/2`,
   `turn_plan_candidate/1`, `faculty_yield*`, `answer_frame/2`,
   `turn_pattern/3` sono porte KB già aperte: il C le interroga e non sa che cosa
   ci passi dentro. Una cura che entra da lì costa **zero** righe di motore.
   (Mantra #5: grep prima.)
2. **Se la porta non c'è, se ne apre UNA, sottile.** Una primitiva, non un ramo:
   deve dire *come* si fa una cosa, mai *quali parole*. `kb_rule_body/2` e
   `kb_journal_refused/4` di questo mese sono della taglia giusta.
3. **Il test del mantra, ogni volta:** *«parrot0 può impararne un nuovo membro
   domani, senza ricompilare?»* Se la risposta è no, la conoscenza è nel posto
   sbagliato — anche se il banco è diventato verde.
4. **Il debito si misura, non si racconta.** Ogni giro riporta qui sotto due
   numeri: quanti `split_words` restano nei tre lettori maggiori, e quante righe
   di C sono uscite. Senza il conteggio, «abbiamo migrato» è un'opinione.
5. **Additivo, mai sostitutivo** ([[keep-secondary-structures]]): il lettore
   vecchio resta come struttura secondaria finché il nuovo non si dimostra
   prevalente. Si toglie quando dà fastidio, non in campagna.

### Le prede grosse, già identificate

- **`split_words` nei tre lettori** — 130 / 54 / 33. È la misura di partenza.
- **Le catene compilate di `&&`** — `TODO(kb-first, gen489)`: 213 istruzioni con
  due o più `kb_cue_match` in congiunzione, fino a quindici congiunti. La forma
  d'arrivo esiste già (`turn_pattern/3` + `turn_pattern_intent/2`, motore in
  `00-lex.c`, esempio in `taught_turn_form.p0t`): ogni malattia curata qui è
  un'occasione per spostarne una.
- **I `mod_*` obsoleti** ([[mod-star-legacy]]): pre-segmentazione, e sono
  proprio i lettori che riscandiscono. Si aggiornano quando danno fastidio — e
  una malattia di questo piano È il fastidio.
- **Le 30 voci di [`kb-first-audit.md`](kb-first-audit.md)**: quando una tocca la
  lettura, si chiude qui.

---

## 4-ter. I VINCOLI DI VALIDAZIONE DEL BANCO (F., 12–18 settembre 2026)

Sono le regole con cui un piolo della scala si dice **compreso**. Stavano
sparse fra gli handoff di `LEARN_TODO.md` e la testa di `scripts/prose-probe.sh`;
qui sono il contratto, e il banco le stampa tutte.

1. **Tre colonne, mai un totale unico.** *Merito* (ciò che il testo dice),
   *meta* (di che cosa parla), *struttura* (quante frasi, come comincia e come
   finisce). Un 14/20 non dice se il lettore ha capito il testo o se ha solo
   saputo contarne le frasi; le attese di meta e struttura le calcola uno script
   indipendente, non parrot0.

2. **⛔ Il cancello (F., 12 settembre).** *«Ogni iterazione dovrà rispondere con
   successo a un numero di domande tale che il conteggio delle parole con cui
   sono scritte le domande a cui ha successo sia più lungo del numero di parole
   del testo stesso; man mano che cresce il testo crescono le domande a cui,
   avendo successo, parrot0 risponde.»* Cioè: **Σ parole delle domande nel
   merito risolte > parole del testo.** È dura e onesta per costruzione: non
   premia un banco corto, non premia una domanda facile ripetuta, e cresce per
   forza col testo — un testo di 500 parole esige più comprensione di uno di
   300, non la stessa percentuale.

3. **⛔ Il banco deve POTER passare il cancello (F., 18 settembre).** Un banco
   le cui domande nel merito, *tutte* risolte, non arrivano alle parole del
   testo non misura la comprensione: misura la propria taglia. Quindi **Σ
   parole di TUTTE le domande nel merito > parole del testo, con margine**: la
   soglia è `P0_BENCH_MARGIN` (default **1,25**), il banco la stampa prima dei
   conti e dichiara `BANCO INSUFFICIENTE` o `BANCO STRETTO` quando non regge.
   Senza margine il cancello coincide con l'obiettivo del 100% (il piolo 300 a
   50 domande, 310 parole, passava solo a 49/50) invece di essere un gradino
   verso di esso.

   | piolo | testo | domande merito | Σ parole | margine 1,25 |
   |---|---|---|---|---|
   | r300 | 299 | 50 → **62** | 310 → **417** | 374 ✓ |
   | r320 | 319 | 13 → **68** | 68 → **435** | 399 ✓ |
   | r340 | 338 | 13 → **65** | 72 → **450** | 423 ✓ |
   | r356 | 356 | 13 → **68** | 74 → **510** | 445 ✓ |
   | r374 | 374 | 13 → **72** | 63 → **488** | 468 ✓ |
   | r395 | 395 | 13 → **72** | 67 → **517** | 494 ✓ |
   | r420 | 427 | 13 → **80** | 69 → **571** | 534 ✓ |
   | r440 | 440 | 13 → **80** | 84 → **584** | 550 ✓ |
   | r464 | 464 | 13 → **85** | 71 → **601** | 580 ✓ |
   | r485 | 485 | 13 → **95** | 71 → **666** | 607 ✓ |
   | r497 | 497 | 13 → **87** | 72 → **651** | 622 ✓ |
   | r010 … r250, r362, r508, r621 | 21–621 | 1–4 | 4–23 | ⛔ banchi diagnostici, senza cancello |

   (Stato al 18 settembre 2026. I pioli sotto le 300 parole sono nati come
   scala «una prosa per piolo, tre domande» prima del cancello: restano
   diagnostici finché non vengono estesi.)

4. **Il banco è FISSO, e il successo tende al 100%.** *«parrot0 deve saper
   rispondere a ogni domanda rispondibile sulla prosa; la stima non implica
   crescere il set di domande ma migliorare la comprensione»* (F., 12
   settembre). **Allargare il banco per abbassare il tasso richiesto è barare;
   allargare un banco che non poteva passare il cancello non lo è**: il tasso
   si calcola su tutte le domande, e le nuove sono altrettante da rispondere.
   Le domande vecchie restano identiche e in testa al file, così i referti
   restano confrontabili sul sottoinsieme storico.

5. **La calibrazione a freddo.** Le stesse domande, in una sessione pulita,
   *senza* la prosa: ciò che riceve risposta lì non è lettura ed esce dal conto
   (`già`). Il banco resta onesto per costruzione anche quando la KB cresce di
   sotto — è successo due volte (`made_of(reefs, colonies)`,
   `located_in(satellite, orbit)` depositati da un `/save`).

6. **Un muro non è mai una risposta**, nemmeno quando contiene la parola
   attesa; e **una risposta falsa conta più di un muro** e si dichiara a parte
   (regola 5 del §4). La revisione a mano delle ✓ è parte della misura
   (mantra #9): il runner non vede una risposta sbagliata che contiene la
   parola attesa.

7. **Prosa vera, esterna alla KB, con la fonte** (`ladder/SOURCES.md`), intera e
   coerente, mai troncata. Nessun fatto del brano si persiste con `/save`
   durante un giro: W=0, L=0.

---

## 4-quater. IL PROCESSO SI RIPETE: LE EVIDENZE DELLE SESSIONI E L'EFFICIENTAMENTO (F., 18 settembre 2026)

> F.: «efficientare questo processo perché lo ripeteremo molte volte con prose
> sempre differenti e anche in italiano; annotare in una sezione dedicata le
> evidenze derivate dalle varie sessioni e grazie alle evidenze gestire
> l'efficientamento. Attenzione: l'efficientamento non deve essere malizioso,
> cioè viziando la prosa di elementi facili da gestire.»

> ⛔ **Che cosa vuol dire «processo», detto da F. la notte del 18 settembre
> dopo che questa sezione era stata scritta nel senso sbagliato:** *«non lo
> scripting: i processi logici che tu come coding agent metti in campo per
> traguardare. Ottimizzare come scopri le cose, come parrot0 potrebbe
> riorganizzarsi o darti evidenze che ti permettono di arrivare prima a un
> risultato, e avere delle mosse standard che hanno dato evidenza.»* Il
> soggetto della missione secondaria è **l'agente**, non il banco: i comandi
> (`prose-rung.sh`, `prose-diff.py`) sono sottoprodotti utili, non la missione.

Questa sezione è quindi il **registro delle mosse dell'agente** con la loro
evidenza (§4-quater.4), la scala di diagnosi che le ordina (§4-quater.5), ciò che
parrot0 deve arrivare a darmi da solo perché io muova meno (§4-quater.6), e gli
anti-pattern pagati (§4-quater.7). La tabella delle evidenze (§4-quater.2) resta
la sorgente: una mossa entra fra le standard solo con una riga lì. Le strategie
senza evidenza non entrano.

### 4-quater.1 Le regole anti-malizia (valgono prima di ogni efficientamento)

Un processo più veloce che misura meno è una regressione travestita. Quindi:

1. **La prosa è cieca.** Si prende il lead di Wikipedia **com'è**, verbatim
   (l'API `rest_v1/page/summary` per l'italiano, la pagina per l'inglese, fonte
   in `SOURCES.md`), su un argomento **scelto prima** di sapere che cosa parrot0
   legge di quel testo. Non si ritocca una frase, non si scarta un testo perché
   «troppo difficile», non si sceglie il piolo successivo fra quelli che
   sembrano andare bene. Se un testo viene scartato, si scrive perché
   (`SOURCES.md`) e vale solo la ragione «non è un lead intero e coerente».
2. **Le domande si scrivono prima delle risposte.** Il `.q` di un piolo nuovo
   si compila dal testo, mai dal referto: chi scrive le domande non ha ancora
   visto che cosa parrot0 risponde. Le domande coprono **tutte le frasi**
   (`scripts/prose-bench-coverage.py`: nessuna frase a zero domande, nessuna
   risposta attesa assente dal testo) e mescolano le forme — definizione,
   relazione, qualificatore, causa, quantità, elenco, coreferenza — senza
   guardare quali forme parrot0 sa già.
3. **Il banco è fisso, tende al 100%, e si estende solo per poter passare il
   cancello** (§4-ter). Mai si toglie una domanda perché non passa.
4. **Nessun fatto della prosa entra in KB durante un giro** (W=0): la
   calibrazione a freddo lo scoprirebbe e lo toglierebbe dal conto, ma il
   punto è non doverlo fare. **La crescita della KB che una sessione lascia è
   conoscenza vera del mondo e della lingua** — classi lessicali reali
   (`adverbial_particle(together)`, `rank_noun(phylum)`), regole sulle forme
   (`ev_origin_bearer/2`), relazioni del mondo con la fonte — mai un fatto
   inventato per un test, mai un fatto del brano per far passare il piolo
   (mantra: «un test non può inventare la conoscenza che dichiara di scoprire»).
5. **Le risposte ✓ si rivedono a mano** (mantra #9) e le «non muro, non
   giuste» del referto sono le prime da leggere: `prose-diff.py` le elenca.
6. **Ogni misura è confrontata con la precedente sullo stesso banco**
   (`prose-diff.py PRIMA DOPO`): guadagnate, perse, cambiate. Un numero senza
   il diff non è una misura.

### 4-quater.2 Le evidenze di processo, per sessione

| data | sessione | evidenza sul processo (misurata) | costo / effetto |
|---|---|---|---|
| 12–13 set | scala 300→500, piolo 300 17→49 | un piolo intero (57 domande, 2 sessioni) costa **~10 min** in `P0_PROBE_STEP2=1`; il passo 1 (una sessione per frase) costa un boot per frase, oltre il quarto d'ora | il passo 2 è la misura di routine; il passo 1 solo per diagnosticare UNA frase |
| 13 set | piolo 320 | il banco a 13 domande (68 parole) non poteva passare il cancello delle 319 | nasce la regola 3 del §4-ter |
| 18 set | ripresa dopo 3 giorni | **il registro del piano era fermo a 17/50 mentre lo stato era 49/50**: 40 minuti persi a ricostruire lo stato da `LEARN_TODO.md`, i referti e `git log -S` | regola: ogni giro chiude aggiornando il §6 del piano; gli handoff altrove rimandano qui |
| 18 set | ripresa dopo 3 giorni | **un piolo certificato regredisce in silenzio** quando altre missioni aggiungono lettori (E3: 49→45, quattro domande, un `from`) | regola: **prima misura, poi cura**; il `.p0t` del piolo (`prose_triage.p0t`) porta il contrasto di ogni furto chiuso, così il cricchetto lo vede prima del banco |
| 18 set | diagnosi della regressione | la sonda **frase sola + domanda + «who answered?» + `P0_READ_TRACE=1`** ha trovato il ladro in una sessione (2 min); il banco intero ne avrebbe impiegate 10 senza dirlo | la sonda per frase è il primo passo di ogni diagnosi; il banco intero è la verifica |
| 18 set | banco esteso | scrivere 60 domande con la risposta nel testo costa **~5 min per piolo** a mano; 11 pioli in un'ora; il controllo di copertura è automatico | le domande le scrive chi legge il testo, non un generatore: un generatore sceglierebbe le forme facili (anti-malizia 2) |
| 18 set | r340 prima misura | **il trasferimento delle classi a un testo nuovo è debole** (4/65) e 6 risposte su 52 sono **furti di turno** di facoltà che non leggono la prosa (saggio causale, definizioni del mondo, «Ask me whether…») | sui pioli nuovi la prima cura è sempre la condotta (mantra #21), non la lettura; il banco stampa le «non muro, non giuste» a parte |
| 18 set | r300, r320, r340 in parallelo | tre banchi insieme su 12 core non si rallentano (nessun timeout: il banco è chat, non `.p0t`) | i pioli si misurano **in parallelo**, uno per processo; i `.p0t` no (budget per turno) |
| 18 set | primo piolo italiano (i100, Carbone vegetale, 108 parole) | **merito 0/23, meta 2/2, struttura 5/5**: 15 muri su 23 sono «Non so ancora tradurre «X»» — la prosa italiana passa per l'interlingua (`tr/2`) e il lessico del testo mancava (carbone, legna, combustibile, carbonaia, mummia, ricoperte, …); il muro stesso dice la forma della lezione | il canale #1 funziona: **37 traduzioni vere insegnate parlando** («the italian for coal is carbone»), 37/37 «Held», `/save` le instrada da solo in `kb/core/gloss.p0`; costo 3 min |
| 18 set | certificare una riga di KB (`phrase_boundary` sulla copula) | i `.p0t` di guardia (5 file) costano ~8 min **e un turno appeso a HEAD** (`bridge_gap.p0t`, «knowledge gap zorb», 60 s) uccide il demone e lascia i file dopo senza esito; `name_is_knowledge` rosso per 0,02 s sopra il budget di 1 s (costo base del turno, `TEST_TODO`) | prima di attribuire un rosso alla modifica si rilancia con la KB di HEAD (`git stash push kb/…` → test → `stash pop`): 3 min, e distingue il pre-esistente dal proprio; un file appeso va **per ultimo** nella catena, o da solo |
| 18 set | il costo della crescita, misurato | 37 fatti `tr/2` + il transcript salvato: `prosepage.it.p0t` «leggi la pagina su Xyzzy» passa da ~1,0 s a **1,09 s** (budget 1 s) — +0,1 s su un turno italiano al limite; `register_realization` (24/29) e il timeout da 2 s di `prosepage.it` sono **identici a HEAD** (bisezione: KB di HEAD, poi solo `input.p0`) | la crescita del lessico ha un prezzo per turno (mantra #20): si misura, non si nega; i rossi al confine del budget si classificano con la bisezione, mai «a occhio» |
| 18 set | Newton chiuso: «quante frasi?» sul 340 | parrot0 dice **16**, il banco attendeva 17: il contatore del banco spezzava dopo «e.g.» come faceva parrot0 prima della cura. **Il banco aveva lo stesso difetto del lettore**, e lo avrebbe premiato | i contatori del banco (`prose-probe.sh` passo 1, `prose-bench-coverage.py`) condividono ora la stessa conoscenza della KB (`sentence_boundary_exception/1`), sull'ultima PAROLA e non sul suffisso (il primo tentativo fondeva «carbonization.» per via di «n.»); attese di r340 (16) e r356 (15) corrette. Due muri onesti in più riconosciuti («I have no rule», «I understood «…»») |
| 18 set, notte | ripresa con `prose-rung.sh` e la sonda | dalla ripresa alla diagnosi nominata (`who answered?` + `P0_READ_TRACE`) **25 min**, contro i 40 del mattino solo per lo stato; il banco di baseline è girato in background mentre si sondava | M1 + M2 + M13 insieme; e da stanotte `make prose-session` fa i primi due passi da solo |
| 18 set, notte | la porta del qualificatore | `answerframe` emette un valore da **quattro** punti (forma, ordinale, sintagma, token): il primo tentativo ne copriva tre e il «why» passava dal quarto; una funzione sola (`p0_qualifier_gate`) e −6 righe di C | prima di aprire una porta in una facoltà, **contare i suoi punti di emissione** (`grep slot_answer`): la porta va nella strozzatura, non nel primo sito trovato |
| 18 set, notte | banchi in parallelo mentre si scrive | i banchi caricano la KB dal disco **all'inizio di ogni sessione** (calibrazione a freddo, poi lettura): una modifica di `kb/` durante il giro entra a metà referto; il soft-test con tre banchi in corso segna 1,75 s dove a macchina scarica segna 1,13 s | si scrive C mentre il banco gira (non entra finché non si ricompila); la KB si tocca fra un giro e l'altro; i `.p0t` si lanciano a macchina scarica (M8) |
| 18 set, notte | un rosso di soft-test dopo il commit | `[antonym]` «Held: the opposite of what is hot»: bisezione con KB e binario di `HEAD~1` (checkout dei soli `kb/` e `src/`, 3 min) → pre-esistente | M7 vale anche dopo il commit: `git checkout HEAD~1 -- kb/` è la stessa mossa dello stash |
| 18 set | i100 dopo il lessico | ancora **0/23**, ma i muri cambiano specie: da «non so tradurre» a «non capisco»; la sonda per frase mostra la lettura (`coal vegetale …`, `located_in(coal_vegetale, …)`): **il blocco è l'accordo sul nome canonico fra lettura e domanda** (locuzione vs parola singola in `tr/2`), non la lingua | il lessico si insegna in minuti e non basta: il prossimo circuito italiano è nel canonicalizzatore (locuzione più lunga prima); la copula come confine di sintagma vale in inglese (misurato) e non ancora in italiano («è» non arriva alla KB) |

| 18 set, notte, 2° turno | il canale #1 «non passa per metà dei verbi» | non era una cue substring (ipotesi del turno prima): le parole che non passano sono **esattamente** quelle già in classe — tre sonde, poi il C solo all'ultimo gradino (`np_closer ← relation_verb`, gen513) | prima di ipotizzare, **partizionare i casi per una proprietà della KB** («è già noto?»): una `grep` sui fatti spiega più di una lettura del C |
| 18 set, notte, 2° turno | «Learned: zorblax is a relation verb» senza tenere nulla | un mio refactoring aveva perso `kb_assert`: successo apparente, nessuna traccia lo mostrava; trovato con **gdb** su un binario `-g -O0` compilato nello scratch (`gcc src/*.c`, 6 s), breakpoint su `kb_assert` con arg «zorblax» → **zero assert** | dopo un «Learned» sospetto: `is X a Y?` subito; se dice no, gdb sul punto di scrittura, non il codice a occhio |
| 18 set, notte, 2° turno | il prezzo di una regola misurato con un banco in background | +1,2 s attribuiti alla regola; A/B a macchina scarica: **3,54 / 3,51 / 3,51 s** (con, senza, KB di inizio sessione) — il prezzo era zero e il turno costa 3 s da prima; il referto del banco intanto contaminato (la KB cambia sotto) e cancellato | una misura di costo vale solo **stesso turno, stessa macchina scarica, stesso strumento** (`/debug` triplica); il banco si lancia **dopo** il commit, mai durante un giro |
| 18 set, notte, 2° turno | tracce temporanee su stderr | F.: «valuta sempre se le tracce temporanee possono diventare parte di /debug»: la traccia `[ecs]` è diventata `turn_class_read` + `debug_probe(23)`; un ordine doppio (15 era di `network.p0`) fa sparire la sonda **in silenzio** | ogni traccia che serve due volte è una sonda: un fatto di turno + una riga in `debug.p0`; `grep -rho '^debug_probe([0-9]*' kb/ \| sort \| uniq -d` prima di scegliere il numero |
| 18 set, notte, 2° turno | `pkill -f <pattern>` per fermare un banco | ha ucciso **la mia shell** due volte (il pattern era nella riga di comando che conteneva anche il rilancio) | fermare per PID (`pgrep -f 'pat[t]ern'`), e mai nello stesso comando che rilancia |

### 4-quater.3 Le strategie in campo, con l'evidenza che le regge

1. **Il ciclo minimo di una sessione, in ordine e con i tempi** (tutte le
   evidenze sopra): (a) `git log -3`, il §6 di questo piano, l'ultimo referto in
   `docs/labs/…` → 5 min; (b) `make build && make test-engine` (binario fresco:
   `parrot0-stale-binary-trap`) → 1 min; (c) **rimisura** dell'ultimo piolo
   certificato in background (`P0_PROBE_STEP2=1`) → 10 min in parallelo con
   (d); (d) il piolo nuovo: prosa cieca, `.q` prima delle risposte, copertura
   verde, banco in background → 15 min; (e) `prose-diff.py` sui due referti;
   le «non muro, non giuste» a mano → 5 min; (f) UNA malattia per giro, sonda
   per frase con `who answered?`, contrasto nel `.p0t` prima della cura, cura
   KB-first, `.p0t` puntuali → il tempo vero della sessione; (g) rimisura, diff,
   §6 + §4-quater, commit e push. **Totale fisso: ~40 min; il resto è cura.**
2. **In parallelo, mai in serie.** I banchi dei pioli sono processi
   indipendenti: tre o quattro insieme costano quanto uno. Il tempo di parete
   di una rimisura completa (300→497, 11 pioli) scende da ~2 h a ~30 min.
3. **La sonda per frase prima del banco.** Ogni frase sospetta si prova da sola
   con la sua domanda e «who answered?» — 2 minuti, e dice CHI ha risposto.
   Il banco intero verifica; non diagnostica.
4. **Il cricchetto della prosa è il `.p0t`, non il banco.** Ogni furto chiuso
   e ogni forma guadagnata lascia un contrasto in `tests/p0t/language/prose_triage.p0t`
   (o nel `.p0t` del piolo): 80 assert in 3 minuti contro 10 minuti di banco, e
   `make test` lo vede quando un'altra missione lo rompe.
5. **Le domande a mano, la copertura a macchina.** Non si genera il banco:
   si controlla (`prose-bench-coverage.py`). È l'unico modo di estendere il
   banco senza scegliere le forme facili.
6. **L'italiano è un piolo, non un ramo**: stessa scala (`ladder-it/`), stesso
   banco (`P0LANG=it`), stessi vincoli; i muri italiani sono marcatori nel banco,
   le cue di meta/struttura sono già in KB (`text_question_cue`, «quante
   frasi», «di che cosa parla»). Un difetto italiano è una riga di KB (una
   parafrasi insegnata, una cue), come per ogni altra lingua (mantra #2).
7. **Lo stato vive qui.** Il §6 è il registro, il §4-quater le evidenze di
   processo; `LEARN_TODO.md` porta solo un rimando. Chi riprende legge questo
   file e l'ultimo referto: 5 minuti, non 40.

### 4-quater.4 Le mosse standard dell'agente, con l'evidenza che le ha promosse

Ogni mossa ha la forma: *quando* → *che cosa faccio* → *che cosa ho risparmiato
o evitato, misurato in questa sessione*. Una mossa senza la terza colonna è
un'opinione e non sta qui.

| # | quando | la mossa | evidenza (18 settembre) |
|---|---|---|---|
| M1 | riprendo dopo giorni | **misuro prima di toccare**: referto salvato + sonda breve corrente; piolo completo alla conferma, entro i limiti di durata | il 49→45 storico prova il bisogno di misurare, non autorizza 10 minuti di banco all'apertura |
| M2 | una risposta è sbagliata ma non è un muro | **chiedo «who answered?» prima di leggere codice** | il ladro (`event-time.p0`, `semantic_lead`, `analysis_family`) nominato in un turno; la specie decide la cura senza aprire il C |
| M3 | il difetto è di lettura | **sonda della frase sola + `P0_READ_TRACE=1` + `P0_FRAME_TRACE=1`**, poi la domanda | «Noted: The built is from stony» visto in 2 min contro 10 di banco intero |
| M4 | la frase sola non riproduce | **traccia sul testo intero**: il difetto è del paragrafo (splitter, coreferenza, offerte pendenti) | Newton: la frase sola era pulita, lo split su «e.g.» compariva solo nel paragrafo |
| M5 | il turno viene deciso senza lettura | **classifico la specie (A/B/C) prima di scegliere il rimedio**: A una riga di conoscenza, B la pretesa sulla lettura, C nessun titolo | `ev_origin_bearer/2` (A) contro retrocessione di `robust` (C); mai una `faculty_yield` per C |
| M6 | due pezzi giusti danno insieme un errore | **cerco la dimensione che alla KB manca per descrivere se stessa** (mantra #23), non il pezzo colpevole | «che cosa può avere un'origine»; «un punto d'abbreviazione non chiude la frase»; «un ramo di domanda chiede se il turno è una domanda» |
| M7 | un `.p0t` va rosso dopo una modifica | **registro prompt, risposta e stato corrente**; niente stash, checkout o bisezione con HEAD, come impone l'ultima diagnosi | la vecchia mossa è ritirata; non attribuisco la causa senza una misura |
| M8 | un rosso è «turn took 1,0x s (timeout 1,00 s)» | **profilo il turno**, tengo il budget e distinguo osservazione da ipotesi sulla causa | il caso storico `name_is_knowledge` non prova che ogni nuovo timeout sia preesistente |
| M9 | devo scegliere il piolo/il testo | **prosa cieca e verbatim, domande scritte prima delle risposte, copertura di tutte le frasi** | 697 domande scritte senza guardare il referto: anti-malizia misurabile (`prose-bench-coverage.py`) |
| M10 | il banco premia qualcosa che sembra giusto | **controllo il contatore del banco contro la conoscenza del lettore** | il banco spezzava su «e.g.» come il lettore: avrebbe premiato il taglio |
| M11 | cambio una condotta (titolo, cessione) | **misuro i turni che il vecchio commento diceva «protetti»** e i pioli che non ho toccato | Aurakai (gen) previsto e trovato; r300/r340 invariati con 8 retrocessi |
| M12 | un'ora è passata su un caso | **scrivo il caso nel registro e cambio circuito**: un circuito per sessione (mantra #22) | il canonicalizzatore italiano scritto come prossimo circuito, non inseguito |
| M13 | ho più misure da fare | **una misura di prestazione alla volta**, senza crescita della KB durante il referto; interrompo oltre i limiti | rettifica dopo la contaminazione dei tempi riportata nell'handoff; il parallelismo storico non certifica la latenza |
| M14 | chiudo un giro | **committo e spingo con il bilancio (C, KB, banco) nel messaggio** | 9 commit in una sessione: ogni ripresa ha un punto certo |
| M15 | apro una porta in una facoltà | **conto prima i suoi punti di emissione** (`grep` della resa: `slot_answer`) e metto la porta in UNA funzione che tutti attraversano | notte del 18: quattro emissioni in `answerframe`, il primo tentativo ne copriva tre e il «why» usciva dal quarto |
| M17 | un muro divide le parole in «passa / non passa» | **partiziono per una proprietà della KB** (`grep` dei fatti: già noto? in quale classe?) prima di leggere il C | canale #1: i 9 che non passavano erano i 9 già `relation_verb` |
| M18 | un «Learned» che poi non si ritrova | **gdb sul punto di scrittura** (binario `-g` nello scratch, breakpoint condizionale su `kb_assert`) invece di rileggere il codice | l'assert perso dal refactoring, trovato in un giro |
| M19 | misuro il prezzo di una regola | **A/B stesso turno, macchina scarica, stesso strumento, e la KB di inizio sessione come terzo punto** | 3,54 / 3,51 / 3,51 s: la regola non costava, il turno costava già |
| M20 | sto per scrivere un `if` sulla lingua nel C | **cerco la regola KB esistente da condizionare** (`np_closer`, `subject_guard`, `turn_declared_act`) prima di crearne una nuova o una funzione | F. ha respinto due tentativi (ramo, predicato parallelo); la forma giusta era una condizione in più su una regola gen513 |
| M16 | ho chiuso una bugia con un muro | **provo le varianti che prima rispondevano giuste per fortuna** (stessa forma, valore che porta / non porta il qualificatore) e dichiaro il prezzo | «in 2010» sulla popolazione dell'isola: giusta per fortuna prima, muro ora; il verso previsto da P2, misurato invece che temuto |

### 4-quater.5 La scala di diagnosi (l'ordine delle mosse, dal più economico)

```text
referto già salvato + sonda breve corrente         nessun banco in background
  └─ «non muro, non giusta» → M2 «who answered?»    1 turno
       ├─ specie C  → titolo (review)               M5, nessuna lettura di codice
       ├─ specie B  → la pretesa sulla lettura      M5, un file KB
       └─ specie A / lettura → M3 sonda + traccia   2 min
             └─ non riproduce → M4 testo intero     3 min
                  └─ la dimensione mancante         M6, una riga KB
                       └─ contrasto fissato prima; .p0t a fine giro; M7 se rosso
```

Il codice C si apre **solo** all'ultimo gradino, e solo per una porta sottile
(come `p0_stage_demoted`, `compound_boundary_is_abbreviation`): tutte le mosse
sopra si fanno parlando con parrot0 e leggendo la KB.

### 4-quater.6 Che cosa parrot0 deve arrivare a darmi da solo (così muovo meno)

Sono crescite di parrot0, KB-first, che accorciano *il mio* processo: ogni
riga è un'evidenza che oggi ricavo a mano e che parrot0 potrebbe **dire**.

1. **La ricevuta di lettura per clausola**: dopo un testo, «da questa frase ho
   tenuto X, da quella niente». Oggi la ricavo da `P0_READ_TRACE` (M3/M4); come
   fatto KB (`clause_reading(Clause, Fact)`) sarebbe interrogabile e mi
   direbbe subito la frase persa e il fatto storto.
2. **«Perché non hai risposto tu?»** per ogni facoltà, con il titolo: «non ho
   titolo: sono fallback», «la mia pretesa era sulla parola, non sulla
   lettura». Il tabellone delle review lo permette già in parte (mantra #21).
3. **I furti come fatti**: quando una risposta arriva in seconda passata, o da
   un modulo senza review, parrot0 lo dichiara nel turno (oggi lo vedo solo con
   la colonna del banco). Il registro `furti.tsv` diventerebbe una vista KB.
4. **Il residuo della lettura**: «di questo turno ho reso conto di questi span,
   non di questi» (D14, copertura): è la misura che separa una risposta da
   lettura da una risposta da cue, cioè la scala del §0-bis fatta fatto.
5. **La calibrazione a freddo detta**: «a questa domanda avrei risposto anche
   senza il testo»; oggi costa una sessione per piolo.
6. **L'auto-riorganizzazione per titolo**: una facoltà che ha risposto in
   seconda passata per tre pioli di fila si propone da sola per la review, o
   per la migrazione a A. È il passo che rende il piano dei furti autonomo.

### 4-quater.7 Anti-pattern pagati (con il costo)

| anti-pattern | costo pagato | la mossa che lo evita |
|---|---|---|
| fidarsi del registro del piano invece dell'ultimo handoff/referto | 40 min a ricostruire lo stato (17/50 vs 49/50) | M1, e il §6 aggiornato a ogni giro |
| curare un furto come incidente (una cessione) | il turno dopo ruba altrove (whack-a-mole misurato tre volte in `turn-arbitration.md`) | M5 |
| leggere il C prima di chiedere «who answered?» | ore; il nome del ladro costa un turno | M2 |
| provare la frase sola e fermarsi | Newton non si riproduceva | M4 |
| modificare uno script mentre un banco lo esegue | un banco vuoto e 20 min persi (permessi) | sostituzione atomica, e il banco non si tocca in corsa |
| intendere «processo» come scripting | mezza sessione sugli strumenti invece che sulle mosse | questa sezione |
| mettere in C la meccanica della lingua (menzione vs uso, confini) | due tentativi respinti da F., «sto valutando di interrompere la sessione» | M20, §4-sexies.2 |
| misurare con un banco in background, o con `/debug` contro un tempo nudo | una regola giusta ritirata per un prezzo inesistente; un referto buttato | M19 |
| un refactoring del proprio C senza una sonda che lo veda | «Learned» senza fatto: il caso peggiore del mantra #7, creato da me | M18; `is X a Y?` dopo ogni «Learned» |

---

## 4-sexies. IL CICLO VIRTUOSO A GIRI DI CINQUE MINUTI, E LA REGOLA SUL C (F., 18 settembre 2026, notte)

> F.: *«la crescita della KB in questa ora di lavoro è stata pochissima … fare
> test con lo spauracchio delle regressioni non è servito a nulla, c'è una certa
> resilienza della KB … la KB deve crescere come effetto dei test e delle prove
> con cose reali»*; e poi: *«quello che mi aspetto da questo processo ripetuto
> più e più volte è che sia un meccanismo di crescita virtuosa: la prosa che
> non viene compresa diventa crescita e la prosa che viene compresa diventa
> conferma … al tendere la crescita della KB dovrebbe essere di un commit e push
> utile ogni 5 minuti: in un round di 5 minuti si capitalizza una nuova
> estrazione ed espansione di abilità linguistiche, invece 40 minuti per
> scoprire che non sapeva leggere le espressioni temporali»*; e sul C: *«ho
> visto la nascita di nuove funzioni C grandi, siamo sicuri che non potevano
> essere sostituite dalla KB stessa? teniamo i mantra e il kb-first»*.

**Questo paragrafo è il punto di partenza di ogni sessione da qui in avanti.**
Le sezioni sopra dicono *come si misura*; questa dice *come si cresce*, ed è
la parte che si ripete.

### 4-sexies.1 Il ciclo, in un giro di cinque minuti

```text
muro o bugia di un piolo  (referto: «Hmm, I don't know about X yet», «·» non muro non giusta)
  └─ 1. la lezione che il muro stesso propone, PARLANDO           1 turno   (canale #1)
       ├─ «Learned/Held» → 2. verifica sulla frase vera della prosa   1 sonda
       │      └─ 3. la riga in `.p0` ufficiale (con data e provenienza), commit, push
       └─ non passa → 3'. la riga in `.p0` LO STESSO, con la nota «la lezione non passa»
              e la parola nel banco del canale (§4-sexies.3): il canale si ripara dopo,
              la conoscenza entra adesso
prosa compresa → conferma: si rilancia il piolo in background, una volta, e si legge il diff
```

Regole del giro:
- **una lezione per giro, vera**: una classe lessicale (`relation_verb`,
  `tr/2`, `rank_noun`, `adverbial_particle`…), una regola di forma
  (`gerund_of/2`), una relazione del mondo con la fonte — mai un fatto del
  brano (§4-quater.1, regola 4);
- **il commit è il giro**: ogni commit porta una riga di KB in più e dice
  quale muro l'ha prodotta; il messaggio è il registro, il §6 riassume;
- **una misura di baseline per sessione basta**; il banco non si rilancia per
  paura di regressioni (misurato la notte del 18: tre rilanci, zero
  regressioni, un'ora di parete): si rilancia come *conferma* dopo una serie
  di giri, in background, mentre si fa il giro successivo;
- **il tempo di scoperta si conta**: se un difetto costa più di un giro a
  capirlo, si scrive nel registro con la sonda che lo mostra e si passa al
  muro successivo (mantra #22: il difetto successivo è sempre più attraente).

### 4-sexies.2 La regola sul C, prima di scrivere una funzione

Una funzione C nuova sopra le venti righe si scrive **prima come regola KB**
sulle viste del turno che già esistono (`turn_word/3`, `turn_surface_at/3`,
`turn_surface_token`, `atom_words`, `member`, `apply/2`, `concat_atoms`), e
solo se la regola non può reggere (misurato: budget, guardia, primitiva
mancante) si apre una porta sottile che *chiede il verdetto* alla KB. Il caso
lavorato della notte del 18: il lettore del qualificatore era nato come
funzione di novanta righe (`p0_question_qualifier`), F. ha chiesto se non
potesse essere la KB stessa, e lo era: `turn_qualifier/3` +
`qualifier_verdict/4` in `grammar.p0`, e il C è sceso a venti righe che
chiedono `qualifier_verdict(Cue, Valore, carries|lacks, Detto)`. Bilancio
del porting: C −107/+31, KB +32. **La domanda da farsi, prima di ogni
`static int`:** *le viste del turno bastano a scrivere questa decisione come
regola?* Se sì, la funzione C è un debito dal primo minuto.

**E dopo ogni commit e push (F., stessa notte):** si rilegge il C appena
committato (`git show --stat HEAD -- src/` e il diff) e ci si chiede, funzione
per funzione, se può essere rifattorizzato KB-first. **Se sì, si fa subito**:
il refactoring è il commit successivo, non una voce di TODO. La sequenza
misurata della notte del 18 è la forma del rito: commit `f6284d47` (porta +
lettore in C, 162 righe) → domanda di F. → commit `a1de019e` (lettore in KB,
C −107). Da qui in avanti la domanda si fa da soli, prima che la faccia F.

### 4-sexies.3 Il registro dei giri della notte del 18 (il primo uso)

| giro | muro | lezione | esito | KB |
|---|---|---|---|---|
| 1 | i100: «Non so ancora tradurre «controllava»» (+3) | `the italian for controlled is controllava` … | 4/4 «Held», promosse in `gloss.p0` | +4 |
| 1 | r300: «what did coral reefs displace…» → «don't know about displace» | `displace is a relation verb` | Learned; ma «…, displacing the …» non si legge: il lettore delle aperture lega al sintagma prima della virgola, un gerundio d'azione parla del soggetto della principale → `gerund_of/2` scritto come forma, il ponte al lettore è un circuito | +3 |
| 2 | r300/r320/r340: 34 parole nominate dai muri | 12 verbi insegnati parlando in un turno ciascuno | **6 su 12 la lezione non passa** («threaten», «endanger», «supply», «remove», «contain», «manage» → fallback o «non ho capito»; «maintain»/«measure» già noti danno un muro invece di «lo so già»); i 13 verbi promossi in `taught-lexicon.p0` con la nota | +13 |
| 2 | «Compost supplies nutrients.» letto, «what does compost supply?» → muro | — | la lettura deposita `supplies`, la domanda chiede `supply`: la flessione vale in un verso solo (`inflection_suffix/1` nella lettura, non nel ponte della domanda) | circuito |
| 5 (secondo turno) | r320 frase 2: «It is commonly prepared by …» / «Compost is prepared by farmers» non si legge | `participle_of/2` + schema passivo della RADICE; guardia: il gerundio è `np_closer` (via `gerund_of`); `decompose`, `recycle` insegnati | il passivo legge per ogni radice; il fatto falso dal gerundio non si scrive; frase 2 resta muro onesto | +22 |
| 4 (secondo turno) | «Compost supplies nutrients.» letto, «what does compost supply?» → muro | y dopo consonante → ies come regola di forma nei due versi (`y_to_ies/2`, `verb_stem`, `extract_frame`); 4 stemmi falsi tolti | «Nutrients.»; un verbo insegnato si legge flesso e si chiede nudo | +13 −4 |
| 3 (secondo turno) | «threaten is a relation verb» → muro per le parole GIÀ verbo (9/13) | la regola gen513 `np_closer ← relation_verb` con la condizione «salvo che il turno parli di una parola» (`turn_mentions_word/1`, 14 teste metalinguistiche); `known_facts` per la lezione ripetuta | 6/6 passano, il verbo insegnato legge; due tentativi in C respinti da F. (vedi §6) | +36 |

~~**Il difetto più fertile emerso**~~ ✅ riparato nel secondo turno della
notte (§6): non era una cue substring, era `np_closer` che vedeva il verbo
noto come confine. **Il prossimo**: la flessione nel ponte della domanda
(giro 2, riga 4: «supplies» letto, «supply» chiesto → muro).
Il testo originale: il muro propone una lezione («say «X is a
relation verb»») che per metà delle parole **non passa** — il canale #1, su
cui poggia tutta la gerarchia di crescita, ha un tasso di successo da
misurare, non da presumere. Il banco per ripararlo sono le sette parole
sopra; l'ipotesi (mantra #8) è una cue substring che ruba il turno («eat» in
threat-eat-en, «anger» in end-anger) e un ramo che tratta la parola già nota
come sconosciuta. È il primo giro della prossima sessione: si ripara una
volta e la crescita per lezione raddoppia.

---

## 4-quinquies. LO SVINCOLO: UN FURTO DI TURNO NON SI SISTEMA, SI REGISTRA E SI CHIUDE PER SPECIE (F., 18 settembre 2026)

> F.: *«il furto è il segnale di un modulo che non sta usando la comprensione
> universale … quando un modulo ruba un turno è un problema cognitivo … voglio
> che questi incidenti siano gestiti con un piano che li risolva una volta per
> tutte, con l'idea di portare tutto in KB … il chitchat è un frasario in KB,
> non un comportamento cognitivo di cui la KB è pregna.»*

Il piano intero sta in [`turn-arbitration.md` §1-bis.1-ter](turn-arbitration.md):
la tesi (un turno rubato è un turno a cui parrot0 ha risposto **senza averlo
letto**), la misura (**74 facoltà su 80 non toccano mai il frame**; 5 recensite
su 80), le tre specie — **A** consumatore KB della IR, **B** frasario in KB con
la pretesa nel C, **C** lettore privato — con il rimedio di ciascuna, e il
registro dei furti. Qui sta ciò che **cambia in ogni giro di questa scala**:

1. **Il banco nomina il ladro.** Con `P0_PROBE_WHO=1` ogni domanda è seguita da
   «who answered?»; il referto porta la colonna **modulo** e il conto dei furti
   per modulo («non muro, non giusta»). Una risposta sbagliata senza il nome di
   chi l'ha data è un incidente; con il nome è una riga del registro.
2. **Ogni furto va nel registro di `turn-arbitration.md`, con la specie.** Mai
   una `faculty_yield` per una facoltà di specie C (mantra #21: insegnare a chi
   non ha titolo lascia il ladro libero altrove). Per la specie B la cura è
   spostare la pretesa sulla lettura (`turn_declared_act`), per la A è una riga
   di conoscenza (come `ev_origin_bearer/2`).
3. **Un piolo è pulito** quando la colonna modulo non contiene nessuna risposta
   «non muro, non giusta» di specie C. Il numero ✓ viene dopo: un 49/50 con
   una bugia dentro vale meno di un 40/50 pulito.
4. **La lettura stessa è nel registro.** Il 18 settembre la prosa del carbone ha
   lasciato in KB `part_of(other, newtons_law_of_universal_gravitation)` da una
   parentetica: un fatto falso da un testo vero è il furto più grave di tutti,
   perché nessuna domanda lo rivela. Prima di ogni cura di lettura, il passo 1
   del banco (una frase per volta) si rilegge cercando i «Learned» che non
   stanno nel testo.

Misurato il 18 settembre sui pioli 320 e 340: i ladri di specie C sono
`analysis_family` (saggi causali), `semantic_lead` (definizioni del mondo per
parola nota), `robust`; i difetti di specie A/B sono di `knowledge` e
`answerframe` (la frase sbagliata a parità di costruzione, il «why» ignorato).
La prima sessione di questo svincolo recensisce le facoltà che portano la scala
(`answerframe`, `knowledge`, `wordquery`, `count`, `quantity`, `coref`,
`discourse`, `reader`) e retrocede a `fallback` le tre ladre di specie C.

---

## 5. I giri, in ordine, con la prova di chiusura

| giro | malattia | cura prevista | prova |
|---|---|---|---|
| ~~G1~~ ✅ | **M4** cessione | condotta KB: una facoltà che PRODUCE prosa cede un turno che PORTA prosa (`faculty_yield_force`) | «Quipu, also spelled khipu, …» non riceve più un racconto |
| ~~G2~~ ✅ | **M1** apposizione | il valore chiede all'IR dove finisce il suo nodo (`np_closer`, le virgole come confine) | la frase 1 lascia in KB i fatti giusti, non uno storto |
| ~~G3~~ ✅ | **M2** ordinale | «first» dentro un avverbiale non è la domanda del turno | «when did coral reefs first appear?» → «485 million years ago» (piolo 300, 13 settembre) |
| ~~G4~~ ✅ | **M3** relativa | «which/who» aprono una relativa sul nodo precedente, non una lezione (`relative_opener/1`, antecedente dall'IR) | «what does the phylum cnidaria include?» → «sea anemones» (piolo 300) |
| **G5** | consumo | portare UN lettore grosso a consumare l'IR invece di `split_words` | il conteggio 217 scende, e il banco non peggiora |
| ~~G6~~ ✅ | **M5** preposizione orfana | «What are X also known **as**?» — l'oggetto e' in testa, la preposizione resta in coda | «what are shallow coral reefs sometimes called?» → «rainforests of the sea» (piolo 300) |
| **G7** | furto di turno | un lettore nuovo di un'altra missione si prende una frase di prosa e dice un fatto storto (18 settembre: E3 e «from») | ogni piolo certificato si rimisura dopo ogni missione che tocca la lettura; il `.p0t` del piolo porta il contrasto |

**Ogni riga porta anche il suo conto KB-first**: quale porta KB è stata usata (o
aperta), quanti `split_words` restano, quante righe di C sono uscite.

Dopo G1-G4 il referto del tardigrade deve passare da 0/7 a qualcosa; il numero
esatto non si promette, si misura.

---

## 6. Registro dei giri

### 18 settembre 2026, notte (secondo turno, giro 3) — il passivo dalla radice, la guardia del gerundio, e la misura che corregge la diagnosi

**Il muro** (r320, riga 2: «It is commonly prepared by decomposing plant and
food waste…», domande 52–54): isolato in chat fino alla forma minima —
«Compost is prepared by farmers.» **non si legge**, mentre «Farmers commonly
prepare compost.» sì. Causa (KB, `passive_frame_coordinate`): il passivo si
genera solo se il **participio stesso** è un `relation_verb` («used»), non
dalla radice («prepare» → «prepared»).

**La cura è morfologia, come -ies** (`grammar.p0`, zero C): `participle_of/2`
(-e → -d, consonante+y → -ied, altrimenti -ed, più `irregular_verb_form`) e lo
schema passivo della radice, che scrive il fatto **sotto la radice**:
«Compost is prepared by farmers» → `prepare(farmers, compost)` → «who
prepares compost?» → «Farmers.»; vale per un verbo insegnato parlando («mice
are zorblied by cats» → «what do cats zorbly?» → «Mice.»).

**Il prezzo, misurato due volte, e la prima volta male**: il primo A/B era
**contaminato** dal banco r320 che girava in background (avviato come
conferma, poi ucciso e il referto cancellato perché la KB cambiava sotto di
lui) e dal profilo `/debug`, che triplica il turno; letto così, +1,2 s e ho
ritirato la regola. Rifatto a macchina scarica, stesso turno («hello» + frase
di 30 parole): **3,54 s con la regola, 3,51 s senza, 3,51 s con la KB di
inizio sessione** (`75da5e50`). Quindi: gli schemi in più non costano, e **il
turno di lettura di 30 parole costa ~3 s da prima di stanotte**, con
`input_frame_observe` a 1,3–1,6 s e `phrase_canon` 2499 chiamate — non i
1398 ms della diagnosi in testa (misurati su un'altra frase, la prima di una
sessione, che parte in lingua `it`). Boot 0,62 → 0,70 s per `participle_of`.

**Il prezzo vero era un fatto falso**: sulla frase del piolo la regola
scriveva `prepare(decomposing_plant, compost_of_ingredients)` — «by +
gerundio» è un mezzo, non un agente (mantra #7). La guardia è conoscenza: il
gerundio di un verbo di relazione è un confine di sintagma (`np_closer($G) :-
gerund_of($Root, $G)`, il ponte che il giro 1 aveva lasciato senza
consumatore), quindi un agente che comincia con un gerundio non è un
concetto e il fatto non si scrive; i verbi della frase («decompose»,
«recycle») insegnati in `taught-lexicon.p0`. Contrasto: `!query!
prepare(decomposing_plant, compost)` in `morphology_ies.p0t` (26 casi: 18
verdi, 8 rossi **solo** per costo > 1 s).

**⛔ RITIRATO a fine sessione, misurato sul referto r320 `-2318` (16/68, +1 −3)**:
la guardia «gerundio = confine» faceva perdere tre risposte vere («Gathering a
mix», «providing nutrients», «the resulting mixture…») perché il lettore al
confine **scarta** l'intero riempitivo invece di tagliare; bisezione per file
in chat: KB di inizio sessione le dava, KB della notte no. Ritirati la guardia
e lo schema passivo dalla radice (le righe restano commentate in `grammar.p0`
con la misura); restano `participle_of/2` e il ponte della domanda al passivo.
Sonda dopo il ritiro: «what does composting require?» → «Gathering a mix.»,
«what do the benefits of compost include?» → «providing nutrients.». Il rimedio
vero è un circuito del lettore: **al confine si taglia, non si scarta**. E la
lezione delle attese (F.): il muro «I don't know about threatens» mentre tiene
`threaten(...)` deve dire i fatti tenuti sotto la relazione chiesta — primo giro
della prossima ripresa, condotta KB.

**Non chiuso**: la frase 2 del piolo resta un muro onesto («commonly» nel
passivo: la lettura dell'avverbio vale per l'attivo); «how is compost
prepared?» / «what is compost prepared by?» chiedono il passivo e la cornice
non ha il ponte participio → radice sul lato della domanda. **Bilancio**: KB
+22 regole/fatti, C 0. **Ago**: una classe di lettura in più (passivo per
ogni radice, anche insegnata) e un fatto falso evitato per specie: ↑ piccolo.

### 18 settembre 2026, notte (secondo turno, giro 2) — la morfologia -y/-ies come regola di forma: «what does compost supply?» risponde

**Il muro** (§4-sexies.3, giro 2 riga 4): «Compost supplies nutrients.» letto,
«what does compost supply?» → muro. **Diagnosi in una sonda** (`P0_READ_TRACE`:
`[aframe] cue=supply pred=supply`, mai `supplies`; `/debug supplies` → il fatto
sta lì): il ponte della domanda (`turn_question_verb` ← `verb_stem/2`) toglie
la «s» e da «supplies» ottiene «supplie». E in `taught-lexicon.p0` stavano
**quattro stemmi falsi salvati** da una vista materializzata
(`verb_stem(supplies, supplie)`, applies, carries, occupies): tolti.

**La cura è conoscenza della lingua, non C** (regole in `grammar.p0`, zero
righe di C): una «y» dopo consonante diventa «ies» — `y_to_ies/2` con
`ends_with_vowel/1` sopra `vowel_letter/1` che già c'era — nei due versi:
`verb_reading_form` (radice → forma), `verb_stem` (forma → radice), e lo
schema `extract_frame` per la forma flessa, così un verbo **insegnato
parlando** si legge flesso e si chiede nudo («zorbly is a relation verb» →
«cats zorblies mice» → `zorbly(cats, mice)` → «what do cats zorbly?» →
«Mice.»). Tutto `concat_atoms`, mai `chars`; **boot invariato** (0,61 s con e
senza le regole; lo 0,39 s della diagnosi era stato misurato a macchina
diversa). Trovato per strada: la vista `extract_frame` legge
`inflection_suffix` direttamente, non `verb_reading_form` — una forma nuova
va detta due volte (un debito di duplicazione, annotato qui, non curato).

**Osservato e non curato**: la prima frase di una sessione in inglese
(«Compost supplies nutrients.») viene letta con lingua `it` (sticky iniziale)
e risponde «Imparato: supplies(compost, nutrients).»; dal secondo turno no.

**Misure**: `morphology_ies.p0t` 2 blocchi (regole 7/7; lettura+domanda
verde nel contenuto, rossa per costo sui turni > 1 s come tutto il resto).
**Bilancio**: KB +13 righe di regola, −4 fatti falsi; C 0. **Ago**: una
risposta in più **da lettura** sul piolo 320 («what does compost supply?»
era un muro del referto) e la morfologia intera per ogni verbo insegnato: ↑.

### 18 settembre 2026, notte (secondo turno) — il canale #1 riparato: la lezione ripetuta, la regola che vede il turno, e due errori miei

**Il muro** (§4-sexies.3, giro 2): «threaten is a relation verb», «endanger …»,
«supply …», «nourish …», «maintain …» cadevano nel muro «I don't know about
relation verb» mentre «jeopardize …», «imperil …» passavano. **Diagnosi in
tre sonde** (M2, M3, poi il C solo all'ultimo gradino): le parole che non
passano sono esattamente quelle **già** `relation_verb` in KB (nove su
tredici dei verbi nominati dai muri dei pioli). Causa: la regola gen513
`np_closer($V) :- relation_verb($V)` fa del verbo noto un confine di
sintagma, il soggetto di «threaten is …» resta vuoto, il lettore «X is a Y»
declina e il turno cade nel muro. Il canale su cui poggia la crescita
parlando perdeva **ogni lezione ripetuta e ogni seconda classe di una parola
già nota**.

**La cura, e la strada per arrivarci (da leggere: è il registro di F.)**.
Tre tentativi in un giro, i primi due sbagliati:

1. un ramo C (`meta_tail`) che salta i confini quando la classe è
   metalinguistica — F.: *«gestire nel C il caso in cui il soggetto sia una
   parola menzionata mi fa incazzare molto»*;
2. un predicato parallelo `subject_boundary/1` con una **seconda cache C
   fotocopiata** da `p0_np_closer` — F.: *«stiamo scherzando???»*, e poi: *«è
   di fatto pragmatica, conoscenza della meccanica della lingua: se la fai
   diventare C stai rompendo il paradigma KB-first, sei troppo poco sensibile,
   sto valutando di interrompere la sessione»*;
3. **la forma giusta: la stessa regola con una condizione in più**, e niente
   di nuovo nel C:

   ```prolog
   np_closer($V) :- relation_verb($V), naf(turn_mentions_word(current_turn)).
   turn_mentions_word($T) :- turn_word($T, $I, $C), clause_copula($C),
                             turn_word($T, $J, $H), gt($J, $I), metalinguistic_head($H).
   ```
   con quattordici teste metalinguistiche in più (`verb`, `noun`, `marker`,
   `particle`, `preposition`… accanto a `name`, `word`). Nel C solo la chiave
   della cache di `p0_np_closer`, che ora si rifà anche a turno nuovo (una
   riga). Il C non sa perché un verbo smetta di essere confine.

**La lezione ripetuta si conferma** invece di murare: `known_facts` («I
already know that threaten is a relation verb.», `So già che …`), un ramo
di otto righe nello stesso lettore. **Il lettore si lascia guardare**:
`turn_class_read` + `debug_probe(23, …)` — la traccia temporanea `[ecs]`
su stderr è diventata una sonda di `/debug` (F.: *«valuta sempre se le
tracce temporanee possono diventare parte della funzione /debug»*).
Trovato per strada: `debug_probe(15)` e `(16)` sono già di `network.p0`
(un ordine doppio fa sparire la sonda in silenzio: `kb_match(…, 1) != 1`).

**Il secondo errore mio, e come l'ha trovato gdb**: rifattorizzando il ramo
«Learned» ho perso la chiamata a `kb_assert`: «zorblax is a relation verb»
diceva *Learned* e non teneva nulla — «is zorblax a relation verb?» → «nothing
I hold says…». Un successo apparente, il caso che il mantra #7 teme più di
tutti; nessuna traccia lo mostrava, l'ha mostrato un breakpoint su
`kb_assert` con arg «zorblax» (binario `-g -O0` a parte, nello scratch, senza
toccare `bin/`). Ripristinato; il contrasto sta in `taught_lexicon.p0t`
`[taught_repeat_lesson]`.

**Misure**: le sei lezioni dei muri passano (threaten, nourish, endanger,
supply, maintain → «I already know»; jeopardize → «Learned») e il verbo
insegnato **legge** («coral reefs zorblax fish» → Learned, «what do coral
reefs zorblax?» → «fish»). Il turno di prosa (30 parole) resta a 1,5 s
parete con il boot: la rienumerazione di `np_closer` per turno non compare
fra i primi otto predicati del profilo (< 10 ms). **Lento = bug, misurato**:
`/debug` su «is a tiger a mammal» → 1168 ms turno, 385 nel solver, **783
fuori**, 19 ricostruzioni d'indice, 3,2 M fatti visitati; è il profilo della
diagnosi in testa (pre-esistente), e rende rosso per costo ogni caso a
budget 1 s (`taught_lexicon.p0t`: 38 rossi, **tutti** timeout 1,0–1,2 s;
il blocco nuovo passa nel contenuto). Non si alza il budget: la riga è in
`TEST_TODO.md`. **Furto nuovo**: la frase 8 del piolo 300 (AFFERMAZIONE,
«…less than 0.1 percent… at least 25 percent…») riceve «0.025.» da
`arith` — specie C, in `furti.tsv`.

**Bilancio dichiarato**: KB +36 (14 teste, 2 regole, 2 template, sonda +
machinery), C: `p0_np_closer` +3, nota di lettura +12, ramo «già noto» +8,
un helper che toglie una duplicazione −10. Test +30 (un blocco, budget 1 s).

**Che cosa muove l'ago (§0-bis)**: nessuna risposta in più da lettura (=);
ma il canale #1 — la crescita parlando — passava per metà delle parole e ora
passa, e dice il vero quando sa già: è «una bugia in meno per specie» sul
lato dell'insegnamento, non del piolo. Piccolo ↑ nel report finale.

### 18 settembre 2026, notte — il qualificatore della domanda: tre bugie del piolo 300 chiuse per classe, e la ripresa in un comando

**Misurato prima di toccare** (M1, `prose-rung.sh r300 r340`, referti `-2044`):
r300 50/62 con `answerframe`×3, r340 5/65 — la KB della sera, nessuna
regressione. **La diagnosi in tre sonde** (M2, M3): «what was the economic
value of coral reefs estimated at in 2020?» → `[aframe] cue=estimated at` →
«anywhere from us$30–375 billion 1997», `who answered?` → `answerframe`; la
lettura lascia `estimated_at(annual_global_economic_value_of_coral_reefs,
anywhere_from_us$30–375_billion_1997)` — **un valore solo**, con il 1997 della
parentetica incollato e le altre due stime perdute; «about half of» idem
(`occupy` → la percentuale); «why does composting offer…» → l'oggetto di
`offer` dal passaggio per token. Tre superfici, una malattia: la cornice
rivendica sulla cue e non legge **il resto della domanda**.

**La dimensione che mancava alla KB (mantra #23)**: *una domanda può
restringere il valore che chiede* — a un tempo, a una porzione, a una causa.
Ora è conoscenza: `question_qualifier(Classe, Apertura)` (time: in, since,
during, before, after, nel, dal; portion: half of, a third of, a quarter of,
twice; cause: why, perche) e `qualifier_shape(Classe, number|word|none)`
(che cosa il qualificatore porta: il numero che segue, la parola
dell'apertura, niente). Il consumatore è **una** porta in C
(`p0_question_qualifier` + `p0_qualifier_gate`, 10-memory-knowledge.c): cerca
le aperture nel turno *fuori dalla cue*, prende il contenuto che la forma
dichiara, lo prova sui valori letti; chi lo porta risponde, se nessuno lo
porta la cornice dice che cosa ha letto e che cosa le manca
(`answer_frame_unqualified`: «I don't know about «in 2020» here: what I read
is that economic value of coral reefs estimated at anywhere from us$30–375
billion 1997.»). Nessuna parola nel C. **Si insegna parlando e si ritratta**
(`taught_question_qualifier.p0t` 7/7): «a question that says "towards" asks
about a time» vale dal turno dopo, «forget that …» lo toglie; la classe si
nomina con le parole di chi insegna (`qualifier_class_name/2`).

**Misure con la porta** (referti `-2102`, tre pioli in parallelo):

| piolo | merito | furti prima → dopo | perse |
|---|---|---|---|
| r300 | 50/62 (=) | `answerframe`×3 → **0** | 0 |
| r320 | 19/68 (=) | «why» → muro onesto; restano `answerframe`×4, `knowledge`×2 (ora nominati) | 0 |
| r340 | 5/65 (=) | invariati (`knowledge`×4, `answerframe`×2, `analysis_family`×1) | 0 |

`prose_triage.p0t` 95/95 (+4 contrasti, con l'ablazione `!forget
question_qualifier(time, in)` che fa tornare la bugia). Soft-test:
`basics.p0t` [taxonomy] è il costo base (1,13 s a macchina scarica, 1,75 s con
tre banchi in corso); [antonym] «Held: the opposite of what is hot» è
**pre-esistente** (bisezione: stesso esito con KB e binario di `3faf9140`),
annotato in `TEST_TODO.md`.

**Bilancio, dichiarato**: C +162/−1, KB +82, test +56. Non è una migrazione
(mantra #18a): è una porta nuova. Il C è un consumatore di due classi KB e
una funzione di resa; i quattro punti in cui `answerframe` emette un valore
(forma, ordinale, sintagma, token) passano da una sola funzione — ed è la
prima misura utile per la migrazione B → A della cornice: **quattro
emissioni, una pretesa**.

**Il prezzo, misurato con le varianti (mantra #24)**: «The population of the
island was estimated at 5 million in 2010.» → `estimated_at(island,
5_million)`, **la lettura scarta l'avverbiale di tempo**; quindi «…estimated
at in 2010?» che prima rispondeva «5 million» *per fortuna* (avrebbe risposto
uguale «in 1990») ora mura per entrambe. È il verso previsto da
`the-magic-of-apply.md` §7.6 P2 («quante risposte giuste per fortuna
diventano muri: misurare»): sui tre pioli **zero** perse, sulla variante una.
Il rimedio è il circuito successivo, sul lato della lettura: il tempo (e la
parentetica con cifre, oggi tolta da `strip_annotation_parentheticals`) come
qualificatore del fatto letto.

**Che cosa muove l'ago (§0-bis)**: una bugia in meno **per specie** (tre
superfici, tre classi, insegnabili); `answerframe` un passo da B verso A (legge
il resto della domanda, non ancora il frame). Non muove: nessuna risposta in
più da lettura. Il conto sta nel report di §0-bis.

### 18 settembre 2026, sera — il fatto falso di Newton e le review delle facoltà della scala

**Newton, diagnosticato con la traccia sul testo intero, non sulla frase.** La
frase sola non lo produceva; il paragrafo sì: lo splitter del turno composto
tagliava «…may contain many other additives, e.g. coal.» al punto di «e.g.»
(`sentence_boundary_cue(". ")`), la clausola monca senza copula finiva nel
ramo mereologico di domanda di `knowledge` (cue di contenimento «contain»),
che risolveva «other» per descrizione — la legge di Newton «attract each other»
— e rispondeva «other is part of newtons_law…» a una frase che non chiedeva
niente. Non un fatto in KB: una risposta falsa a una clausola dichiarativa,
composta nel «Learned:» del turno. Due cure, entrambe generali: le
abbreviazioni con il punto interno sono conoscenza
(`sentence_boundary_exception/1`, en e it) e lo splitter la consulta prima di
tagliare; il ramo di domanda chiede alla lettura se il turno **è** una domanda
(`p0_turn_is(question)`) — il gesto di specie B → A che il piano dei furti
prescrive. Ora la frase resta intera («2 sentences»), la clausola dà un muro
onesto, «what is the heart part of?» risponde ancora. Contrasto in
`prose_triage.p0t` (84/84). C: +25 righe (una funzione di confine e una
guardia), KB +17 abbreviazioni vere.

**Le review (mantra #21, cricchetto verde: 14 su 79).** Le otto facoltà della
scala recensite in testa al modulo e in `module-review.p0` con i numeri
misurati; `coref` (zero letture dalla KB) e `robust` (il ladro di «depend on»)
retrocesse a `fallback`. Dettaglio e specie in `turn-arbitration.md`
§1-bis.1-ter. I ladri pre-registro (`semantic_lead`, `analysis_family`) restano:
non sono facoltà del registro e una review non li tocca — prossimo lavoro di
motore. **Misure dopo le retrocessioni, con la colonna dei moduli** (`P0_PROBE_WHO=1`,
referti `r300-dopo-review-50-di-62.txt`, `r340-dopo-newton.txt`,
`r340-dopo-review.txt`):

| piolo | merito | meta | struttura | cancello | chi risponde (✓) | furti per modulo |
|---|---|---|---|---|---|---|
| r300 | **50/62** (invariato) | 2/2 | 5/5 | ✅ 312 > 299 | `answerframe` 48, `knowledge` 1, `turn_plan` 7 (canale KB) | `answerframe`×3 (qualificatore ignorato: «in 2020», «in 2014», «about half of») |
| r340 dopo Newton | 5/65 | 2/2 | 4/5 → 5/5 con l'attesa corretta | ⛔ 25/338 | `answerframe` 4, `knowledge` 1 | `knowledge`×4, `semantic_lead`×2, `analysis_family`×2 |
| r340 dopo le review | **5/65** (invariato) | 2/2 | 5/5 (attesa corretta) | ⛔ 25/338 | `answerframe` 4, `knowledge` 1 | `knowledge`×4 (lettura), `semantic_lead`×2, `analysis_family`×2 (pre-registro); `robust` non ruba più |

Retrocedere `coref` e `robust` non ha tolto niente alla scala: **il titolo si
può togliere senza perdere risposte**, che è la prova del rimedio giusto per la
specie C (mantra #21: un modulo immaturo si retrocede, non si governa). E la
colonna dei moduli dice una cosa che il numero nascondeva: sul piolo 300
**tutte le risposte nel merito vengono da una facoltà sola**, `answerframe`,
di specie B — relazioni e forme in KB, pretesa su cue, zero usi del frame. È
la facoltà da portare per prima sulla lettura (`turn_declared_act(question)` +
la relazione letta): finché non lo è, il 50/62 è un frasario ben fornito, non
una condotta di cui la KB è pregna. Sul 340 i ladri restavano `semantic_lead` e
`analysis_family`, che nessuna review toccava perché stanno nel dispatcher
prima delle due passate. **La porta sottile è aperta** (stessa sera):
`p0_stage_demoted` legge il titolo dello stadio, e uno stadio retrocesso riprova
solo nella seconda passata (`turn-arbitration.md` §1-bis.1-ter). La porta ha
mostrato subito il passo successivo: i furti degli stadi non spariscono, **si
spostano** a chi ha titolo (`answerframe` dà la definizione di *temperature*
dove prima la dava `semantic_lead`; `gen` prendeva la domanda di progettazione
e gli è stato insegnato a cedere, `design_request`), e ogni prompt d'analisi
paga ora la prima passata intera. Misura del 340 con la porta in vigore
(`r340-dopo-porta.txt`): **5/65, meta 2/2, struttura 5/5** — nessuna risposta
persa; furti per modulo `knowledge`×4 (lettura), `answerframe`×2 (le
definizioni di *temperature* e del ciclo del carbonio, ereditate da
`semantic_lead`), `analysis_family`×1 («where is wood carbonized in modern
methods?»: parla in seconda passata perché nessuna facoltà con titolo ha
detto niente — è il limite del diritto `fallback`: la specie C dice l'ultima
parola invece di un muro, finché non viene migrata o tolta). `semantic_lead`:
zero. Sul piolo 300 tutto invariato.

### 18 settembre 2026 — la regressione di tre giorni, il banco che può passare il cancello

**Misurato prima di toccare qualcosa.** Il piolo 300, certificato **49/50 con
cancello passato** la notte del 13 (vedi sotto), ritorna oggi a **45/50**,
cancello riaperto (274 < 299), sulla stessa KB viva più tre giorni di altre
missioni (mix di capacità, interlocutore di frontiera E1–E4, insegnamento
super-umano: +833 righe in `grammar.p0`, +978 in `10-memory-knowledge.c`,
+1005 in `99-registry.c`). Le quattro domande perse:

```text
Most coral reefs are built from stony corals, whose polyps cluster in groups.
  → «Noted: The built is from stony.»           (13 settembre: Learned … stony corals)
Coral reefs are under threat from excess nutrients (nitrogen and phosphorus), …
  → «Noted: The threat is from excess.»         (13 settembre: Learned … threaten)
```

Non un muro: **un fatto storto detto con sicurezza** — la specie peggiore
(§4, regola 5). Il ladro è la lettura E3 dell'interlocutore di frontiera
(`kb/core/event-time.p0`): «X from Y = l'origine di X» scattava su qualunque
«from» di qualunque turno, e il turno di prosa diventava un evento descritto
con il suo `turn_response`. È la forma del mantra #21: un modulo **maturo**
(tutto in KB, addestrabile) che ruba per una lettura troppo larga — quindi
**si insegna**, non si retrocede.

**La cura, KB pura (0 righe di C).** Alla lettura mancava la dimensione *che
cosa può avere un'origine* (mantra #23): un evento con orario, il viaggio, o
la controparte letta nello stesso turno («a client from Milan»).
`ev_origin_bearer/2`, tre righe; un portatore nuovo è una riga. Contrasto nel
banco `tests/p0t/language/prose_triage.p0t` (le due frasi: mai «Noted», e le
quattro domande rispondono); E3 intatto (`frontier_transcript.p0t` 21/21,
`origo.p0t` 20/20; `prose_triage.p0t` 80/80).

**Il banco, esteso perché possa passare il cancello** (§4-ter, regola 3):
r300 50→62 domande nel merito (310→417 parole), r320 13→68 (68→435), r340
13→65 (72→450), r356 13→68 (74→510), r374 13→72 (63→488), r395 13→72
(67→517), r420 13→80 (69→571), r440 13→80 (84→584), r464 13→85 (71→601),
r485 13→95 (71→666), r497 13→87 (72→651): **+697 domande nel merito**, tutte
con la risposta scritta nel testo. Le domande storiche restano identiche in
testa ai file. Il banco
stampa da oggi la propria raggiungibilità (`P0_BENCH_MARGIN`, 1,25).

**Misure dopo la cura** (referti in
`docs/labs/apprendimento-assistito/2026-09-18-regressione-e-banco/`):

| piolo | storiche | nuove | merito | meta | struttura | cancello |
|---|---|---|---|---|---|---|
| r300 | **49/50** (come il 13) | 1/12 | **50/62** | 2/2 | 5/5 | ✅ **312 > 299** |
| r320 | **12/13** (come il 13) | 7/55 | **19/68** | 2/2 | 5/5 | ⛔ 105/319 |
| r340 | 3/13 (era 1/13 il 12) | 1/52 | **4/65** | 2/2 | 5/5 | ⛔ 19/338 |

Le domande storiche tornano dove erano: **la regressione è chiusa** e il
cancello di r300 è di nuovo passato. Le domande nuove dicono il resto, ed è
la ragione per cui il banco andava esteso: sul piolo 300 ne passa una su
dodici, sul 320 sette su cinquantacinque. Le forme che le tengono aperte, in
ordine di frequenza sui due pioli:

1. **Il qualificatore ignorato** — due risposte confidenti e sbagliate, quindi
   prima di tutto: «what was the economic value of coral reefs estimated at
   **in 2020**?» → «anywhere from US$30–375 billion» (la cifra del 1997);
   «what area do coral reefs occupy **about half of**?» → «less than 0.1 percent
   of the world's ocean area». La relazione è giusta, il vincolo della domanda
   non viene provato.
2. **Il turno rubato da un'altra facoltà**, due misclaim: «how much of the
   waste in landfills…?» → un elenco di salvataggio di rame e magneti (una
   procedura); «what is added to the plant matter…?» → la definizione di
   *matter* dal mondo. Mantra #21: prima classificare se il ladro è maturo.
3. **La domanda «what kind of / in what kind of»** sull'aggettivo o sul
   modificatore («in what kind of water», «what kind of process», «what kind
   of reclamation», «in what kind of farming»): manca il lessico degli
   aggettivi come valore, già l'unica aperta del 13.
4. **Il sostantivo composto della domanda che non trova la chiave** («brown
   waste», «brown materials», «compost rich», «turned regularly»): la domanda
   costruisce un nome di più parole che la lettura non ha lasciato.
5. **La parentetica come definizione** («green waste (nitrogen-rich
   materials such as …)», «brown waste (woody materials …)»): la parentesi
   dopo un nome dice che cos'è, e oggi si legge solo come esempi.
6. **«What did X displace / lead to / aim to maintain»**, «at the dawn of
   which period», «where do … exist on smaller scales»: verbi letti ma non
   interrogati da quella forma, e avverbiali di tempo dentro un inciso.

**Il primo piolo italiano — i100, «Carbone vegetale» (lead di it.wikipedia,
verbatim dall'API, 108 parole, 4 frasi; `tests/fixtures/prose/ladder-it/`).**
Prima misura: **merito 0/23, meta 2/2, struttura 5/5** — la struttura e il tema
valgono già in italiano (le cue di `text-structure.p0` c'erano), il merito no.
Non per la lettura: per il **lessico**. Quindici muri su ventitré dicono «Non
so ancora tradurre «carbone»», «…«ricoperte»», «…«mummia»»: la prosa italiana
entra per l'interlingua (`tr/2`, gen506e) e le parole del carbone non c'erano.
Il muro stesso nomina la lezione («the italian for … is carbone»), che è la
prova che il canale #1 della gerarchia di crescita è aperto: **37 traduzioni
vere** insegnate parlando in tre minuti, tutte «Held», `/save` le ha
instradate da solo in `kb/core/gloss.p0` — conoscenza vera della lingua, non
un fatto del brano (regola 4 del §4-quater.1). Due debolezze da tenere:
«di che cosa parla?» risponde con la prima frase intera (il sintagma italiano
non si chiude sulla copula «è»); «da che cosa è prodotto?» → «carbone vegetale
is classed as combustibile prodotto» (definizione al posto della relazione, e
la resa in inglese dentro un turno italiano).

**Rimisura dopo il lessico (`i100-dopo-lessico.txt`): ancora 0/23 — ma i muri
sono cambiati di specie, e questo è il dato.** «Non so ancora tradurre» resta
su quattro parole (controllava, otteneva, aveva, Similaun); tutte le altre
domande ora si traducono e cadono su «Non capisco ancora» o «Su carbone
vegetale non so ancora molto». La prosa viene letta — la sonda dice
`Imparato: coal vegetale è un fuel product, located_in(coal_vegetale,
process_of_carbonization_of_the_firewood)` — e il difetto è a monte di ogni
domanda: **la lettura e la domanda non si accordano sul nome canonico**
(mantra #23, la forma di D33/D35/D37). «carbone vegetale» nel testo diventa
`coal_vegetale` (la traduzione a parola singola `tr(coal, carbone)` vince
sulla locuzione `tr(charcoal, carbone_vegetale)`), mentre la domanda «che
cos'è il carbone vegetale?» arriva come «what is charcoal?»; e «prodotto dal
processo» lascia un `located_in` — una bugia in KB, la specie peggiore. **È il
circuito della prossima sessione italiana**: la locuzione più lunga vince sulla
parola nel canonicalizzatore (una regola generale, non un vocabolario), e «dal»
non è un luogo. Non si cura qui: un circuito per sessione (mantra #22).

Un difetto laterale con la sua misura: «di che cosa parla il testo?» rispondeva
la prima frase intera perché il sintagma non si chiudeva sulla copula «è». Una
copula finita non sta mai dentro un sintagma nominale: `phrase_boundary(np,
breaker, $W) :- clause_copula($W)` (`input.p0`, una riga sulla classe che
c'era). **Misurato:** in inglese chiude («The reefs are red. They are old.» →
«The text is about The reefs.»), in italiano no («La torba è scura.» → la frase
intera): la parola «è» non arriva alla KB come `è` dal costruttore della IR
(accento perso o riscritto prima del confronto). Anche questo va alla sessione
italiana, con `P0_READ_TRACE` sulla IR.

**Il piolo 340 (Charcoal), prima misura con queste classi: 4/65.** Il
trasferimento che r320 aveva mostrato (4/13 a freddo) qui quasi non c'è: le
frasi del carbone sono relative ridotte lunghe («a lightweight black residue
made of carbon that is produced by strongly heating wood (or other …) in
minimal oxygen to remove …»), participi con agente, «involves + gerundio»,
«led to», «aimed to maintain», e **sei risposte confidenti e sbagliate** su
52 domande nuove, tutte furti di turno di facoltà che non leggono la prosa:
il generatore di saggi causali («On charcoal used as in chemical, a causal
account turns on…»), la definizione del mondo di *temperature* e del *carbon
cycle*, «Ask me whether something holds first», e «in which regions did
charcoal production contribute to deforestation?» → «america, africa» (la
frase sbagliata: quella è la produzione illegale; il testo dice Central
Europe). È la classe 2 dell'elenco, e sul piolo nuovo è la più numerosa:
**prima di ogni cura di lettura, i furti** (mantra #21) — un muro onesto vale,
una bugia no. Referto: `r340-prima-misura.txt`.


Conto KB-first: C **0 righe**; KB +14 (event-time.p0), banco +1 caso.
`split_words` invariato: 132/54/33 = **219**.

### 13 settembre 2026, notte — piolo 300 da 17/50 a **49/50 = 98%, cancello 301 > 299**; piolo 320 da 4 a 12/13

> ⚠ Questo registro era rimasto fermo a 17/50: i gradini della notte del 13
> stanno negli handoff di `LEARN_TODO.md` («HANDOFF 2026-09-13 (gen514,
> notte)», cinque voci) e nei referti di
> `docs/labs/apprendimento-assistito/2026-09-13-direzione-della-domanda/`
> (`bench-27 … bench-49-cancello.txt`) e `…/2026-09-13-piolo-320/`. Qui il
> riassunto, perché il piano vivo deve dire dove si è.

| gradino | merito | cancello | che cosa ha aperto la strada |
|---|---|---|---|
| triage delle 33 aperte | 17 → 19 | 107 | due fortunate e quattro bugie chiuse |
| attenuazione, avverbi del verbo, relative possessive e con `that`, coordinati, domande locative | 19 → 27 | 156 | `attenuating_quantifier`, `verb_adverb`, `bare_relative_opener`, `relative_clause_verb` |
| gli esempi di «including», costruzioni a ruoli invertiti interrogabili | 27 → 30 | 184 | `participial_opener/2`, `frame_role_order/2` |
| «what is X?» dice la classe o ciò che ha letto, e solo su X | 30 → 35 | 209 | la lettura citata («I have no definition of it, but I read: …») |
| costruzione con copula chiesta senza copula; participio anteposto | 35 → 37 | — | `fronted_participle/1` |
| perfetto con particella, subordinatore in coda, predicati aggettivali | 37 → 40 | — | `perfect_auxiliary`, `trailing_subordinator`, `subordinator_modifier`, `adjective_relation` |
| la parentetica misurata; «since when» | 40 → 41 | 248 | `parenthetical_relation/1`, `particle_question_word/2` |
| il tipo chiesto decide fra due descrizioni; alternanza di voce; scopo dell'agente; parentetiche di annotazione; nomi d'attributo partitivi; valuta; complemento con particella dopo l'oggetto; ranghi tassonomici e catena; composti agentivi | 41 → **49** | **301 ✅** | `active_agent_surface`, `purpose_by_agency`, `attribute_noun`, `value_relation`, `particle_surface_for`, `rank_noun`, `membership_chain` |

Revisione a mano delle 49: nessuna risposta falsa; deboli «Warm.» (elenco di
aggettivi troncato) e «coral reefs first.» (resa). **L'unica aperta:** «in what
kind of water do reefs grow best?» — manca un lessico degli aggettivi per
tenere «warm, shallow, clear, sunny, and agitated water» come un solo valore.

**Piolo 320 (Compost), subito dopo:** al primo passaggio, senza toccare niente,
4/13 — il trasferimento c'è. Poi 4 → 7 → 11 → **12/13**, ogni gradino
riverificato su r300 (sempre 49/50). Cure: `includes` canonico, «such as» come
esempi, «because» anche senza virgola e «since» solo con, un turno inglese non
traduce le sue parole (`content_translation_source/1`), soggetti coordinati
distribuiti, `verb_particle(break, up)` e `(use, as)`, la ridotta dopo un
predicato nominale, nomi relazionali con articolo, `np_opener(those/these)`.
Aperta: «what can compost be used for?» (il modale con «be» e `use for`).
Il banco a 13 domande (68 parole) non poteva passare il cancello delle 319:
è il reperto da cui nasce la regola 3 del §4-ter.

Trappole pagate (valgono per il prossimo): `naf` su goal non ground (due
volte), `snprintf` su se stesso, un «of» partitivo preso per particella
(bugia chiusa prima del commit).

### 13 settembre 2026 — partitivo e verifica delle definizioni: 17/50

Baseline confermata 16/50, risultato **17/50**, cancello **97/299**, meta 2/2
e struttura 5/5. `some of ... ecosystems` resta intero; una domanda con
preposizione in coda non riceve piu' una semplice definizione. Il partitivo
riusa i due consumatori esistenti; la verifica definitoria consuma i token IR.
Sonda runtime: **31/31**. Soft-test ancora rosso per timeout di `basics.p0t`.
C +33/-45 (in buona parte commenti), KB +26, `split_words` invariato a
132/54/33 = **219**. Nessuna risposta del banco salvata nella KB.
[Report, limiti e prossimo circuito](../labs/apprendimento-assistito/2026-09-13-prosa-partitivi.md).


### Giro 0 — 12 settembre 2026 (gen513): il banco e la diagnosi

Creato `scripts/prose-probe.sh` + `make prose-probe`; due prose esterne con le
fonti. Referto di partenza **0/7** e **0/5**, quattro malattie isolate e
attribuite. Nessuna cura in questo giro: prima la misura.

Debito di partenza: **`split_words` 130 / 54 / 33 = 217** nei tre lettori
maggiori. Righe di C uscite: 0.

### ✅ LA SCALA OLTRE LE 500 PAROLE (12 settembre, iterazioni 12-18)

F.: «ti fermi quando dimostri che parrot0 e' in grado di comprendere una prosa
lunga almeno 500 parole», con prose sempre diverse e un commit per iterazione.

| piolo | testo | parole | esito |
|---|---|---|---|
| r300 | barriera corallina | 299 | 3/4 |
| r356 | magnete | 356 | **2/2** |
| r497 | acciaio | 497 | **2/2** |
| r508 | satellite | 508 | 0/2 |
| **r621** | **foresta** | **621** | **2/2** |

```text
r621 — foresta, 621 parole (lead di https://en.wikipedia.org/wiki/Forest)
  a freddo, prima di leggere:  «I don't know much about forest yet»
  dopo aver letto il testo:
    What is a forest?   ->  forest is an ecosystem.                 (frase 1)
    What are forests?   ->  forests is a largest terrestrial ecosystems.  (frase 5)
  2 domande su 2.
```

> ⚠ Onesto su che cosa prova: le due risposte vengono da due frasi DIVERSE del
> testo, ma sono tutte e due DEFINIZIONALI. Le domande non definizionali su
> questo testo («dove si formano le foreste?», «che cosa succede quando…») non
> rispondono ancora: quelle forme non hanno un lettore. La comprensione
> dimostrata a 621 parole e' quella della classe, non ancora quella piena.

**Quattro cure di SCALA**, tutte invisibili su una frase e fatali su un
paragrafo — ed e' la specie che questo piano esiste per trovare:

1. **Il turno era lungo 255 byte e la prosa no** (`P0_TURN_MAX`, 4096): a 255
   byte il paragrafo arrivava mozzato e nessuna facolta' riconosceva piu' niente.
2. **«Dovunque stia» vale per una frase, non per un paragrafo**: un interrogativo
   seguito dalla copola dentro una subordinata («…explains how it works») faceva
   leggere il TURNO INTERO come domanda, e il testo non veniva piu' diviso.
3. **«Nessuno lo precede» non regge su un testo lungo**: il primo nodo si
   chiedeva per negazione, e su centinaia di nodi l'enumerazione dentro il `naf`
   non arriva in fondo. Ora si chiede direttamente (`input_node_first/2`).
4. **La forza del turno si ri-derivava a ogni domanda**: la stessa domanda dava
   due risposte diverse a due momenti dello stesso turno — `compound_statement`
   valeva subito dopo la pubblicazione e non valeva piu' trenta righe dopo. Ora
   si materializza una volta, sul turno appena pubblicato.

**Il blocco che resta, diagnosticato con precisione.** Una superficie insegnata
che legge un MODIFICATORE («@S characterized by @O») si prende la frase
principale: «A forest is an ecosystem characterized by a dense community of
trees» diventa `has(forest_is_an_ecosystem, …)`. La guardia del verbo finito la
RIFIUTA — giustamente — ma dopo il rifiuto il turno viene perso invece di
tornare al lettore di classe, e la definizione sparisce. Quindi oggi si puo'
avere la definizione **o** la relazione del modificatore, non tutte e due.
E' il primo lavoro del prossimo giro, e vale per ogni frase d'enciclopedia.

Aperto anche: il **soggetto coordinato** («A satellite **or an artificial
satellite** is an object») blocca la lettura di classe — r508 e' a 0/2 per
questo.

### La SCALA (F., 12 settembre): 10 → 150 parole, una prosa per piolo

`scripts/prose-ladder.sh` · `tests/fixtures/prose/ladder/` — quindici prose vere
ed esterne alla KB (lead di Wikipedia, fonti in `ladder/SOURCES.md`), una per
piolo, **intere e coerenti, mai troncate**: leggere un frammento e' un problema
diverso e si affronta dopo questo traguardo.

> ## ✅ TRAGUARDO — 12 settembre 2026: **153 parole, 4 domande su 4**
>
> ```text
> tests/fixtures/prose/ladder/r150.txt — meridiana, 153 parole
> (lead di https://en.wikipedia.org/wiki/Sundial, esterno alla KB)
>
>   What is a sundial?                        ->  sundial is a horological device.
>   What does a sundial consist of?           ->  Flat plate.
>   What does the gnomon cast?                ->  Broad shadow.
>   What does the shadow of the style show?   ->  time.
> ```
>
> Le quattro risposte vengono da **quattro frasi diverse** del testo, non dal
> solo incipit; il banco rifiuta i muri che contengono la parola attesa, quindi
> il 4/4 non e' regalato; la KB e' quella viva e intera; la prosa e' quella che
> Wikipedia pubblica, senza troncamenti.
>
> **Che cosa e' servito, in undici iterazioni** — e nessuna di queste era
> «conoscenza mancante»: erano tutte strade rotte, e quattro erano difetti
> introdotti dal lavoro dei giorni prima.
>
> | # | la cura |
> |---|---|
> | 1 | «What is **a** X?» non rispondeva nemmeno per `dog` (disattivata di proposito per la lettura di appartenenza, che pero' non aveva niente da elencare) |
> | 2 | la relativa ridotta e' una **seconda proposizione** sullo stesso soggetto |
> | 3 | la definizione **con la coda** («a device used to weave cloth») veniva delegata a valle, dove la coda non si legge |
> | 4 | il **passivo** insegnato come inverso dell'attivo |
> | 5 | il **passato** di un verbo di relazione chiede la stessa cosa (`past/2` era un fatto senza consumatori) |
> | 6 | ⚠ un sintagma nominale **non contiene un verbo finito** — chiuso un FATTO FALSO |
> | 7 | le superfici di **domanda**, e il classificatore che non e' la classe |
> | 8 | il tetto del nome di una classe (3 → 4 parole), e la prosa lunga senza copula |
> | 9 | l'avverbiale che apre la frase non e' il soggetto |
> | 10 | ⭐ il **soggetto con un sintagma dentro** — «the shadow of the style» |
> | 11 | ⭐ l'**inciso** che apre la frase non e' la frase (`adjunct_peel`) |
>
> E il banco stesso e' stato corretto due volte: rifiutava i muri solo dopo
> l'iterazione 5, e prima regalava un ✓ a una risposta che non c'era.

> ### ⚠ E QUELLO CHE IL TRAGUARDO **NON** DICE
>
> Il 4/4 e' su QUEL testo. Misurato subito dopo, sugli altri pioli alti:
> r080 2/4 · r090 2/4 · r110 0/4 · r120 1/3 · r130 0/3 · r140 0/3 · **r150 4/4**.
>
> E la differenza non e' la difficolta' delle frasi: e' il **paragrafo**.
>
> ```text
> «A mangrove is a shrub or tree that grows mainly in coastal saline water.»
>   letta DA SOLA          ->  Learned: mangrove is a shrub.   ✓
>   dentro il paragrafo    ->  «I don't know much about mangrove yet»
> ```
>
> La stessa frase si legge o si perde a seconda di quanto testo le sta intorno.
> Il sospetto ha gia' un nome e un TODO scritto nel motore — `canon[256]` tronca
> la prosa lunga prima del dispatch (99-registry.c, `prose_learn_lead`) — e ora
> ha anche una misura: e' il **prossimo blocco**, e vale per ogni testo lungo.
>
> Detto in modo che non si possa fraintendere: parrot0 sa comprendere UNA prosa
> di 153 parole, non ancora QUALUNQUE prosa di 150 parole.

Stato misurato dopo otto iterazioni (12 settembre, PRIMA delle iterazioni 9-11):

| piolo | testo | parole | esito |
|---|---|---|---|
| 10 | incudine | 24 | 1/3 |
| 20 | cairn | 21 | 1/3 |
| 30 | quipu | 31 | 1/3 |
| 40 | kelp | 35 | 0/3 |
| 50 | telaio | 51 | 1/3 |
| 60 | mulino a vento | 57 | 0/3 |
| 70 | pomice | 54 | 1/3 |
| 80 | savana | 82 | **2/4** |
| 90 | abaco | 89 | **2/4** |
| 100 | anfora | 100 | 0/3 |
| 110 | ossidiana | 109 | 0/4 |
| 120 | clavicembalo | 126 | 1/3 |
| 130 | basalto | 128 | 0/3 |
| 140 | mangrovia | 139 | 0/3 |
| **150** | **meridiana** | **153** | **2/4** |

Il piolo 150 risponde alla definizione **e** a una relazione presa da una frase
in mezzo al testo:

```text
What is a sundial?           ->  sundial is a horological device.
What does the gnomon cast?   ->  Broad shadow.
```

**Non e' ancora il traguardo** — comprendere una prosa di 150 parole vuol dire
rispondere a tutto cio' che dice, non a meta'. Ma e' la prima volta che una
prosa vera di quella lunghezza risponde su piu' di un fronte.

### I blocchi che restano, in ordine di quante domande sbloccano

1. **Il soggetto con un sintagma dentro.** «The shadow of the style shows the
   time» non si legge affatto: il soggetto ha un «of» in mezzo, e il lettore lo
   taglia. E' la forma piu' comune di soggetto della prosa d'enciclopedia.
2. **L'acquisizione si prende i turni di prosa.** «In the narrowest sense of the
   word, it consists of…» riceve «I looked up «narrowest» but found nothing»:
   la cessione `faculty_yield_force(learn, open, prose_carried)` e' dichiarata
   ma quel percorso non passa dal registro, quindi non la consulta.
3. **La domanda a oggetto in testa** (M5): «What else is an abacus called?».
4. **La coreferenza dentro un paragrafo**: «It is an igneous rock» dopo
   «Obsidian is…» — il pronome non arriva al soggetto della frase precedente.
5. **Il conteggio** («How many savanna forms exist?») e le relazioni dentro una
   subordinata («until largely replaced by…»).

### Giro 1 — 12 settembre 2026 (gen513): M4 e M1 chiuse

**G1 · M4 — la cessione (KB pura, zero righe di C).** Il turno di prosa non
dichiarava **nessuna forza**: `turn_declared_act($T, assertion)` pretende un
frame dichiarativo completo, e una frase vera di enciclopedia — apposizione,
coordinazione, participio — non lo lega. Cosi' il turno usciva dalla lettura
senza etichetta e la prima facolta' che sapeva dire qualcosa lo prendeva.
Aggiunta la lettura piu' DEBOLE che basta: `turn_declared_act($T, prose_carried)`
— c'e' una copula, non c'e' il punto interrogativo, non si apre con una
richiesta. Non pretende di aver capito la frase: dichiara che non e' un ordine.
Con `faculty_yield_force(gen, open, prose_carried)`.

> «Quipu, also spelled khipu, are record-keeping devices…» non riceve piu' un
> racconto inventato. Il generatore di storie e' invariato (differenziale).

**G2 · M1 — l'apposizione (la peggiore: un fatto storto, in silenzio).** La
regola che chiude uno slot su una virgola **c'era gia'** (gen505y) e non poteva
scattare: quando il legatore dei frame riceve i token, **le virgole sono gia'
state tolte in place** da un chiamante piu' a monte. Una guardia giusta e cieca —
la stessa specie delle radici morte del gen512. Il turno originale invece non e'
stato toccato: ora la virgola si chiede a `active_turn_norm`, camminando sul
turno in ordine insieme ai token.

> «Tardigrades, also known as water bears or moss piglets, are …» →
> `Learned: tardigrades also known as "water bears or moss piglets".`
> E il fatto e' RAGGIUNGIBILE: «What is another name for tardigrades?» →
> **«water bears or moss piglets.»**

**Conto KB-first, onesto.** G1: **0 righe di C**, +35 di KB — porta gia' aperta
(`faculty_yield_force`). G2: **+20 righe di C** in un punto solo, e nessuna
uscita. Non e' una migrazione: e' un ponte. La forma giusta e' che il confine
venga dall'IR come vista KB, ed e' il giro **G5** — finche' il legatore dei
frame e' in C, la sua virgola resta in C. **Debito invariato: 217.**
Sonda nuova: `P0_FRAME_TRACE=1` mostra pattern, token, virgole e slot legati —
e' quella che ha trovato che la guardia era cieca.

**Aperta in questo giro — M5, la preposizione orfana.** «What are tardigrades
also known **as**?» → *«nothing I hold says tardigrades also known as **as**»*:
l'oggetto e' in testa (e' il pronome interrogativo) e la preposizione resta
sospesa in coda, dove il lettore la prende per oggetto. Il fatto c'e' e un'altra
superficie lo trova: e' un difetto di forma della DOMANDA, non della lettura.
Da mettere in coda ai giri.

### Giro 2 — 12 settembre 2026 (gen513): rispondere *sul* testo, non solo *dal* testo

F. ha alzato il livello di verifica: «ogni iterazione deve rispondere a 20
domande mixate tra nel merito del testo e meta domande tipo di cosa parla e
anche domande di struttura come è composto il testo».

Il banco ora conta **tre colonne**, perché sono tre strade diverse e un totale
unico le mescolerebbe (13/20 non dice se il lettore ha capito il testo o se ha
soltanto saputo contarne le frasi):

| specie | che cosa chiede | da dove viene la risposta |
|---|---|---|
| **merito** | ciò che il testo dice | lettura + fatti in KB |
| **meta** | di che cosa parla | il testo come oggetto |
| **struttura** | quante frasi, come comincia | la IR del testo trattenuto |

Le risposte attese di meta e struttura **non vengono da parrot0**: le calcola
uno script indipendente dal testo. Se parrot0 dicesse un altro numero, il banco
lo deve dire — ed è successo due volte, vedi sotto.

#### La correzione di rotta: **usare la IR, non affiancarla**

La prima stesura faceva asserire al lettore composto in C una *seconda*
struttura del testo (`text_sentence/2`, `text_sentence_count/1`,
`text_word_count/1`, `text_topic/1`). F.: **«non stai lavorando usando la IR
universale»**. Era vero, ed era il difetto peggiore: conoscenza nuova nel
motore, parallela a quella che l'IR già aveva. Quei fatti sono spariti.

Per arrivarci l'IR andava riparata in tre punti, e sono **tre difetti veri**:

1. **Gli id erano della chiamata, non dello scope.** Ogni
   `input_structure_publish` ripartiva da `0`, e un turno di più frasi pubblica
   una volta per frase nello stesso scope: `input_node(current_turn, 0, …)`
   aveva una soluzione per frase, e **ogni join per id era un prodotto
   cartesiano fra frasi diverse**. L'IR universale era illeggibile proprio
   sulla prosa lunga.
2. **Il testo non restava.** «Di che cosa parlava?» arriva *dopo* la prosa, e a
   quel punto `current_turn` descrive la domanda: «il testo ha 1 frase e 7
   parole» — la domanda stessa. Ora la IR di un testo letto resta in
   `last_text`; che cosa *meriti* di restare lo dice la KB (`turn_is_text/1`).
3. **Le frasi non finivano nella IR.** Il lettore composto aveva già in mano la
   segmentazione vera e la teneva per sé; `turn_publish` pubblica gli *span*,
   che non sono le frasi. «Da quante frasi è composto?» diceva **1**.

#### I tre tetti muti che mangiavano la prosa vera

- **`MAX_CLAUSES = 8`.** Al nono confine il ciclo usciva e **il resto del turno
  non lo leggeva nessuno**: nessun muro, nessuna traccia. Su 500 parole (~25
  frasi) due terzi del testo sparivano, e la scala misurava la comprensione di
  un terzo di testo credendo di misurarla tutta. Ora è `turn_max_clauses/1`.
- **`span_atom/2` è `chars/2` andata e ritorno**: la stringa passa per una
  lista di caratteri dentro un termine da 512 byte, e oltre ~50 caratteri
  fallisce **in silenzio**. Era il motivo per cui «come comincia?» rispondeva
  sulla prosa corta e murava su quella vera.
- **`list_len/2` è ricorsiva** e il motore si ferma a `KB_MAX_DEPTH` (64):
  contare 300 parole non arrivava in fondo.

#### Due difetti della capacità nuova, trovati dai pioli

- **Il tema murava su metà dei testi veri** (piolo 320). Si leggeva dal
  sintagma che apre il testo, e un sintagma lo delimita un determinante: «A
  coral reef is…» sì, «Compost is a mixture…» no. Terzo strato: la parola con
  cui il testo comincia, che in un lead è il definiendum.
- **«Quante parole?» rispondeva 311 su 299** (piolo 320). Il flusso di token
  spezza dove la lingua non spezza («0.1%», e la normalizzazione stacca «20%»
  in «20 %») perché quel flusso serve a *leggere*. Chi chiede quante parole ci
  sono intende le parole: ora si pubblica un flusso di **parole**, sul turno
  come l'ha scritto l'interlocutore, e il separatore è un fatto
  (`word_separator/1`).

#### ✅ Il difetto che teneva ferma la scala da tre giri (piolo 340)

> «Charcoal is a lightweight black residue made of carbon.» → **«Steel.»**
> — e la definizione non entrava affatto.

Non era un errore di lettura, era un errore di **turno**: lo schema
«@O is made of @S» combacia, lo slot @O prende «charcoal is a lightweight black
residue», la guardia lo rifiuta giustamente (verbo finito, mantra #7) — ma a
quel punto il turno era già di chi **risponde alle domande**.
`turn_declared_act(assertion)` non aiuta: nasce da `turn_reading`, e questa
frase una lettura non ce l'ha. L'evidenza che non è una domanda è *precedente*
a qualunque lettura:

```prolog
turn_declared_act($T, unasked) :- turn_prose_copula($T),
    naf(turn_has_question_mark($T)), naf(turn_opens_question($T)),
    naf(turn_opens_request($T)).
faculty_yield_force(answer_frame, open, unasked).
```

Adesso la stessa frase lascia **due** fatti invece di zero: definizione *e*
modificatore convivono. Era il conflitto che aveva fatto ritirare la lezione
«characterized by» al giro scorso.

#### La crescita laterale della KB (F.: «un obbiettivo che dobbiamo sempre avere»)

**+138 verbi di relazione**, insegnati parlando e salvati con `/save`, e
**15 particelle** (`relation_particle/2`) — appartiene *a*, consiste *di*,
deriva *da*, cresce *in*: una delle forme più comuni della prosa
d'enciclopedia, e non ne esisteva il lettore. Una riga per verbo, non il
prodotto cartesiano con tutte le preposizioni: gli schemi si scorrono tutti a
ogni turno (gen459).

#### ⛔ I blocchi che restano, riordinati per quante domande sbloccano

1. **La relazione dentro una subordinata.** «…because composting reduces
   methane emissions due to…», «…called charcoal burning, often by forming a
   charcoal kiln, the heat is supplied by…». È il caso dominante sui pioli
   340 e oltre: dodici domande su tredici.
2. **Il passivo.** «Most coral reefs **are built from** stony corals» →
   risposta confidente e **sbagliata** («Colonies.», presa dalla frase prima).
   Peggio di un muro.
3. **Il qualificatore della domanda ignorato.** «what **phylum** does coral
   belong to?» → «Class anthozoa.» Giusto il verbo, sbagliato il valore.
4. **Il soggetto coordinato.** «Aerobic bacteria **and** fungi manage…» — lo
   stesso difetto che tiene r508 a zero.
5. **Il participio in testa** («Sometimes called rainforests of the sea, …»)
   e **con agente** («held together **by** calcium carbonate»).
6. **L'anafora fra frasi** («**They** occupy less than 0.1%…»).
7. **Le forme di domanda non definitorie**: «where do X grow best?», «when
   did X first appear?», «how much of Y…?».
8. **La particella di due parole**: «break **down into**» —
   `relation_particle/2` ne regge una sola.

Le due **risposte confidenti e sbagliate** (2 e 3) vanno prima di tutto il
resto: un muro si conta, una bugia no.

#### La scala 300 → 500, misurata per intero (12 settembre 2026)

| piolo | testo | parole | merito | meta | struttura | tot |
|---|---|---|---|---|---|---|
| 300 | Coral reef | 299 | 6/13 | 2/2 | 5/5 | **13/20** |
| 320 | Compost | 319 | 3/13 | 2/2 | 5/5 | **10/20** |
| 340 | Charcoal | 338 | 1/13 | 2/2 | 5/5 | **8/20** |
| 360 | Magnet | 356 | 2/13 | 2/2 | 5/5 | **9/20** |
| 380 | Sugar | 374 | 3/13 | 2/2 | 5/5 | **10/20** |
| 400 | Concrete | 395 | 1/13 | 2/2 | 5/5 | **8/20** |
| 420 | Erosion | 427 | 1/13 | 2/2 | 5/5 | **8/20** |
| 440 | Violin | 440 | 1/13 | 2/2 | 5/5 | **8/20** |
| 460 | Clock | 464 | 1/13 | 2/2 | 5/5 | **8/20** |
| 480 | Thunderstorm | 485 | 2/13 | 2/2 | 5/5 | **9/20** |

**Le due colonne nuove sono chiuse su tutta la scala: meta 2/2 e struttura 5/5,
da 300 a 500 parole.** parrot0 sa dire di che cosa parla un testo che non
conosce, da quante frasi e quante parole è fatto, come comincia e come finisce
— e lo sa leggendo la IR, non un riassunto che qualcuno gli ha messo accanto.

**E la lunghezza non è più la variabile.** Il merito non scende salendo la
scala: 300 fa 6/13 e 440 ne fa 1/13, ma non perché il testo sia più lungo —
perché è scritto in un modo che il lettore non attraversa. I tre tetti muti del
giro 2 (`MAX_CLAUSES`, `span_atom`, `list_len`) erano la lunghezza, e sono
chiusi. Quello che resta è la **forma**: subordinate incassate, apposizioni,
soggetti coordinati, participi. È una notizia buona travestita da numero basso:
si sa che cosa misurare adesso.

⚠ Nessun piolo ha segnalato domande «già rispondibili a freddo»: dopo la
rimozione di `made_of(reefs, colonies)` non resta contaminazione, e il passo di
calibrazione lo dirà da solo se ne rientrasse.
