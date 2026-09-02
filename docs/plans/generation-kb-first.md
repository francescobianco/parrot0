# La generazione è indietro — analisi e migrazione KB-first

*Aperto il 2026-09-01, su indicazione di F.: «potrebbe essere che tutto il
generation sia indietro, va analizzato e va portato in KB-first».*

## 0. La misura, prima di ogni opinione

`src/brain/30-generation-reading.c`, 3331 righe:

| cosa | quanti |
|---|---|
| punti in cui `mod_gen` **rivendica** il turno (`return 1`) | **39** |
| test di **cue diretta** (`kb_cue_match`) | **77** |
| decisioni che passano da un **punteggio di evidenza** | **1** |

Trentanove modi di prendere la parola, uno solo dei quali **compete**.

## 1. La diagnosi

Il resto del sistema — e un ramo di `mod_gen` stesso, quello di
`creative_response` — sceglie facendo **competere ipotesi su evidenza
dichiarata** (`kb_hypothesis_best`): i candidati sono fatti, il vincitore è
unico, la prova è ispezionabile e chi perde lo fa per una ragione dicibile.

Gli altri trentotto rami rivendicano **posizionalmente**. La politica *è
l'ordine degli `if` nel C*. Questo significa tre cose, tutte gravi:

1. **Non ha nome.** Non esiste un fatto che dica «la continuazione narrativa ha
   la precedenza sulla lettura di un fatto». Esiste solo il fatto che una riga
   viene prima di un'altra.
2. **Non è interrogabile.** `who answered?` dice *quale* modulo ha risposto, mai
   *perché aveva diritto* di farlo.
3. **Non è correggibile parlando** — cioè viola il mantra #17 e il secondo
   corollario cardinale di `PRINCIPLES.md`.

Il sintomo che si vede da fuori è il **turno rubato**: una frase dichiarativa
riceve un frammento di narrativa.

## 2. Perché le cure precedenti erano sbagliate, e come somigliano

Questo difetto è stato curato due volte, e le due cure hanno la **stessa forma**:

- **gen337** — `imperative_opener/1` (execute, run, migrate, scan…), aggiunto
  *«so the narrative-continuation gate never claims a long command as a story»*.
- **gen491, primo tentativo (mio, ritirato lo stesso giorno)** — diciotto
  `move_requires(narrative_continuation, "storia"/"scrivi"/…)`.

Entrambe sono **liste di parole per sintomo**. La seconda è peggiore, perché
prendeva un principio generale — *parrot0 deve poter cambiare comportamento in
tutto* — e lo degradava nel caso particolare, aggiungendo un frasario nel gesto
stesso con cui si aggiungeva il mantra che lo vieta. F. l'ha fermata: *«il mio
era solo un esempio generico, non volevo che lo usassi come caso»*.

E nessuna delle due poteva funzionare, perché entrambe cercavano di riconoscere
**la richiesta** in un turno che non è una richiesta. «metto il libro sul
tavolo» non è un comando: è un'**asserzione**. Nessuna lista di verbi imperativi
la vedrà mai.

## 3. Il pezzo che manca davvero

> Nessuno legge **che cosa chi parla sta facendo** col turno.

Universal-input assegna ruoli di **contenuto** (`segment_role(goal, …)`,
`expected`, …) ma non ha mai assegnato una **forza illocutiva**: asserzione,
domanda, richiesta. Senza quella lettura, ogni facoltà deve indovinare da sola
se le si stia chiedendo qualcosa — e indovina con una lista.

La cura è una lettura **sola e condivisa**, più **un fatto per facoltà**:

```prolog
illocution_cue(directive, $Opener) :- imperative_opener($Opener).
faculty_force(narrative_continuation, directive).
```

`imperative_opener/1` smette di essere una lista d'esclusione al servizio di un
cancello e diventa **evidenza di una lettura generale**: lo stesso oggetto usato
da tutti, invece di una copia per sintomo. Migliorare la lettura migliora ogni
facoltà insieme — che è esattamente ciò che una lista per facoltà non può fare.

**Additivo per costruzione:** una facoltà senza `faculty_force/2` si comporta
come prima. Un cancello implicito su tutto avrebbe spento in silenzio condotte
che nessuno aveva esaminato, ed è il criterio di evoluzione sbagliato.

### Ciò che la classe conteneva davvero

`imperative_opener/1` aveva quindici membri, **tutti** verbi di comando
sull'infrastruttura (execute, run, migrate, scan, propose…): i verbi che avevano
causato il sintomo del gen337. Mancavano gli imperativi ordinari — *continue,
write, finish, tell, describe, explain* — cioè proprio quelli con cui si chiede
davvero di generare qualcosa.

> Una classe popolata dai sintomi non è una classe: è **l'elenco degli
> incidenti**.

È un test riutilizzabile su qualunque classe KB: si guardano i membri e ci si
chiede se descrivono la classe o la storia dei bug.

## 4. Stato

- ✅ Lo strato della forza esiste, condiviso, e la facoltà narrativa lo usa.
  Il dirottamento è chiuso e le richieste narrative reggono (verificato in
  differenziale: la suite `generation` è identica a prima del cancello).
- ✅ La condotta è **nominabile** (`faculty_surface/3`): chi parla può chiamare
  una facoltà con parole sue, requisito perché sia oggetto di discorso.

## 5. Il lavoro che resta, in ordine di leva

1. **La correzione parlata.** `faculty_surface/3` e
   `faculty_force_lesson/2` sono ora consumate da un unico motore in
   `00-lex.c`; la lezione asserisce `faculty_force/2` senza nominare la facoltà
   nel C. Il gate runtime è preparato in
   `tests/p0t/conversation/faculty_conduct_teach.p0t` e resta da eseguire.
2. **Le altre trentotto rivendicazioni.** Cinque artefatti `creative_text` sono
   ora candidati additivi del dispatcher comune; la metafora parametrica usa
   `creative_response_topic/3`. Le restanti vanno portate a competere come già
   fa `creative_response`: candidati dichiarati, `kb_hypothesis_best`, prova
   memorizzata. Le rese dei percorsi legacy toccati in questo giro passano
   inoltre da template KB (`creative_text_answer`, `riddle_answer_reply`,
   `poem4_answer` e simili), così il contenuto letto non torna a essere una
   frase compilata. Il ramo che già lo fa è il modello — non c'è niente da inventare.
3. **La forza oltre l'imperativo.** `directive` legge ora anche gli opener
   indiretti dichiarati in `request_opener/1`; restano da portare nella stessa
   vista le domande (che hanno già superfici sparse in `answer_frame`) e la
   copertura completa delle richieste cortesi/indirette.
   La forza direttiva è inoltre pubblicata dal frame universale come
   `turn_illocution/2`, così i consumer possono condividere la lettura del turno.
   **✅ Chiusa il 2026-09-02:** `turn_illocution/2` riusa la domanda strutturale
   (`turn_opens_question/1`) e la estende all'inversione di qualunque ausiliare
   dichiarato; le richieste cortesi si compongono come ausiliare × destinatario.
   Crescita e ritiro parlati nel gate `illocution_comprehension.p0t` (14 assert).
4. **`raccontami una storia` mura** (pre-esistente, non causato dal cancello):
   la forma con clitico enclitico non arriva alla classe. È la stessa famiglia
   di GD8.
   **✅ Chiusa il 2026-09-02:** nessuna cue `raccontami`; la forma nasce da
   host + gloss del verbo + gloss del clitico e alimenta canonicalizzazione,
   lingua e forza del turno. Gate `enclitic_imperative.it.p0t` (18 assert).

## 5bis. Fatto il 2026-09-02

1. **✅ La correzione parlata.** `faculty_conduct_teach.p0t` — 8 passed. Tre
   difetti veri: `kb_dequote` non idempotente, la resa italiana non scelta
   (mancava l'evidenza di lingua, ora `language_phrase/2`), e il gate stesso che
   dopo il `forget` asseriva `!query` invece di `!query!`.
2. **La cessione del turno ha un nome.** `faculty_yield(Facolta', Stadio,
   Classe)` + `faculty_yield_both/4`: 12 fatti KB al posto delle catene di `if`
   in `gen`, `role`, `wordquery`, `answer_frame`, `arith`. Lo **stadio** è parte
   della condotta — cedere all'apertura e cedere dopo la gara dei propri
   artefatti sono due condotte diverse.
3. **Due copie della stessa macchina unificate.** `concise_explain` e
   `sensory_phrase` erano lo stesso lettore scritto due volte, e le loro sei cue
   di misura esistevano due volte in KB sotto nomi opachi. Ora `word_count_cue/2`
   è una lettura sola e `sized_artifact/3` fa **competere** gli artefatti su
   quante cue soddisfano, invece di ordinarli.
4. **Le due rese vuote di §B-bis.** `riddle_answer_reply` riempita (+ forma
   italiana), `creative_text_answer` tolta con la cornice resa facoltativa al
   sito di chiamata.

**Il bilancio (mantra #18):** C +195/−104 di solo codice = **netto +91**. Le
righe sono tre lettori generici che si ammortizzano (il tredicesimo
`faculty_yield` costa 0 C), ma **la tesi non è verificata**: va verificata
continuando a migrare sugli stessi lettori finché il bilancio non gira.

## 6. La regola che questo lavoro lascia

> Quando una facoltà sbaglia a prendere il turno, la domanda giusta non è
> «quale cue le manca?» ma **«quale lettura del turno non è stata fatta?»**.
> La prima domanda produce una lista; la seconda produce una capacità.
