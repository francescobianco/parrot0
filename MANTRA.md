# I MANTRA — da passare PRIMA di scrivere qualsiasi riga

> Se stai leggendo questo file perché un agente te l'ha messo davanti: è
> voluto. In parrot0 la regola non è "scrivi codice che funziona", è **"non
> scrivere codice se la conoscenza può farlo"**. La differenza non è stilistica:
> è la tesi che il progetto sta cercando di dimostrare. Codice che funziona ma
> mette conoscenza nel C fa *regredire* l'esperimento anche quando i test
> diventano verdi.

**La domanda zero, prima di ogni modifica:** *questa cosa è generalizzabile
KB-first?* Se la risposta è "sì ma è più lavoro", si fa il lavoro.

**Il test operativo del mantra #2:** *"parrot0 può impararne un nuovo membro
domani, senza ricompilare?"* Se la risposta è no, quella conoscenza è nel posto
sbagliato. La forma più esigente della prova è dirglielo **parlando** e vedere
che cambia comportamento subito — vedi `tests/p0t/language/taught_lexicon.p0t`.

**⛔ Anti-barare per l'apprendimento via prompt:** “parlando” significa lingua
naturale, non Prolog/P0 o una API serializzata nel testo. Se il teacher deve
conoscere nomi di predicati interni, arità, tuple, `!assert`, MCP o la forma di
`kb.assert`, non ha insegnato: ha scritto nella KB attraverso un altro
trasporto. Quel risultato vale zero. Il controllo è: *un esperto del dominio
che ignora lo schema interno saprebbe formulare la lezione?* Se no, ci si ferma
e si amplia la meta-comprensione; non si espone la rappresentazione.

**Il criterio di evoluzione (F., dall'esperimento con l'LLM):** una proposta è
nella direzione giusta se **aumenta ciò che parrot0 vede e la sua capacità di
decidere** fra le viste; è nella direzione sbagliata se **riduce ciò che un suo
pezzo vede** per ottenere la risposta giusta per costruzione — anche quando è più
semplice, anche quando i test diventano verdi. Vedi `docs/plans/one-kb.md` §4c.

**⛔ La KB viva è il soggetto: non esistono KB ermetica o fatti del mondo
accesi/spenti.** parrot0 è il motore con la KB completa scelta attraverso il
profilo; la crescita di parrot0 è la crescita della sua KB. Non supportiamo
interruttori che ne sopprimano la conoscenza, né test con base o lessico
sostituiti da un vuoto. Ogni misura ottenuta su una KB amputata è **obsoleta**,
anche se pretendeva di dimostrare soltanto una meccanica: non misura parrot0.
Il profilo non specificato usa `agi`; valori vuoti non disattivano la KB.
`!reset` può pulire lo stato conversazionale, mai spegnere la conoscenza.
Le ablazioni mirate di una lezione servono a verificarne l'effetto sulla KB
viva; cancellare l'intera base non è un'ablazione valida. Le attese dei vecchi
test vanno rivalidate semanticamente, mai rese verdi riducendo il soggetto.
Vedi `TEST_TODO.md` §0.0 e `docs/plans/test-engine.md` §2b.

**⛔ Un test non può inventare la conoscenza che dichiara di scoprire.** Entità,
fatti e regole inventati nel test possono dimostrare una meccanica generale o
la crescita a runtime; **non** dimostrano che la KB viva possieda conoscenze
connesse, che attraversi rappresentazioni reali o che sappia unire punti già
appresi. Per verificare *connecting dots*, comprensione o trasferimento fra
domini, il test deve partire da conoscenze preesistenti nella KB completa, porre
un prompt naturale e verificare un fatto utile espresso dalla risposta. Non si
certifica il percorso interno, non si inietta il risultato atteso, non si crea
un micro-mondo su misura. Se non esistono ancora archi reali capaci di produrre
la risposta, il test deve restare rosso e rendere visibile il gap: farlo verde
con nomi fittizi significa trattare la KB come un mock e vale zero come prova di
comprensione.

---

Regole imposte a noi stessi per non ricadere nel fix puntuale. Prima di scrivere
*qualsiasi* riga, si passa questa checklist. Nate dagli errori reali di gen348-349.

1. **"È generalizzabile KB-first?"** — la domanda zero. Non fixare l'istanza,
   motorizza la CLASSE. Se la risposta è "sì ma è più lavoro", si fa il lavoro.
2. **Niente liste di parole nel C.** Trigger, cue, sinonimi, unità, verbi: sono
   fatti KB enumerabili (`causal_process_verb/1`, `verb_syn/2`, `time_unit/1`).
   Test: *"parrot0 può impararne un nuovo membro domani senza ricompilare?"*
   **E vale nei due sensi:** questo elenco nomina solo ciò che parrot0 LEGGE, e
   per anni ha lasciato fuori ciò che DICE. Le frasi di risposta stanno in KB
   esattamente come le cue — vedi il mantra #16.
   **Estensione procedure:** se il fix introduce una trasformazione o un calcolo
   di classe, non fermarti a mettere cue/template in KB: cerca prima una procedura
   insegnabile in `kb/core/procedures.p0` sopra i primitivi (`is/2`, confronti,
   termini composti). Il C deve restare adattatore NL→goal o primitiva generale,
   non consumer della procedura di dominio.

## Gerarchia di Crescita

La crescita della KB segue questa gerarchia obbligatoria, dal canale più fertile
al più invasivo:

1. **Insegnamento diretto via prompt.** Prima si prova a dire a parrot0 la nuova
   forma e si verifica che la riconosca immediatamente; il test deve includere
   anche retract/ablazione. Se una forma non è insegnabile parlando, la KB non
   ha ancora raggiunto la soglia minima di fertilità per quella classe.
2. **Lettura della prosa guidata da prompt.** La forma insegnata deve funzionare
   sul testo reale, frase per frase, conservando i fatti estratti e la provenance;
   una pagina non è acquisita se il lettore perde la sostanza di una frase.
3. **Autocorrezione, discovery e remediation.** Gli arresti osservati durante
   inferenza e lettura diventano gap tipati; il sistema scopre il rimedio minimo,
   lo prova riponendo lo stesso turno e conserva soltanto ciò che lo fa
   proseguire senza errore.
4. **Promozione manuale in `.p0`, ultima spiaggia.** È ammessa soltanto
   quando manca il motore astratto che rende possibile ai tre canali
   precedenti di crescere, mai per aggiungere un fatto o chiudere un prompt
   singolo. La riga deve essere la forma più generale possibile e deve aprire
   subito crescita via prompt, prosa o autocorrezione.

La domanda di controllo è: **se scrivo questa riga in `.p0`, sto promuovendo un
motore fertile o sto sostituendo un insegnamento che parrot0 dovrebbe poter
ricevere parlando?** Nel secondo caso la modifica è una regressione, anche se il
prompt diventa verde.

Il criterio di soglia minima è operativo: una classe è pronta quando la forma
può essere insegnata a runtime, la prosa può usarla per produrre fatti e
l'autocorrezione può riattivarla dopo un arresto. Una
KB piena di righe manuali ma incapace di questo ciclo è grande, non fertile.

3. **Astrai fino al punto fisso.** Non moltiplicare *predicati* per una relazione
   vista attraverso verbi diversi: `wrote`/`painted`/`composed` = UNA relazione
   `created_by(Creator, Work, Verb)`, il verbo è un campo. Chiedi: *"relazione
   NUOVA o STESSA relazione sotto un'etichetta diversa?"*
4. **Il sostantivo del prompt è sospetto.** `rectangle`, `apple`, `robot`,
   `Canberra`, `WWII` possono nominare fatti, cue-class, entity/frame in KB; non
   possono nominare un motore se sopra di loro esiste una struttura più generale.
   Prima di scrivere un nome nuovo, costruisci la scala: oggetto → categoria →
   relazione → vincolo → procedura. Il motore prende il nome dalla procedura più
   alta che conserva il comportamento, non dall'oggetto campionato.
5. **Cerca il motore esistente prima di scriverne uno nuovo.** (Avevo duplicato
   `transitive_comparison` senza accorgermene.) `grep` prima di scrivere.
6. **Non estendere per analogia col codice esistente.** Se c'è già `wrote/2`,
   NON aggiungere `painted/2` per riflesso: ri-derivare dallo scheletro, o
   propaghi il debito di disaggregazione.
7. **Uccidi il muro, MAI con una risposta sbagliata.** Un errore factuale è
   peggio di un muro (dottrina no-deception). Nel dubbio, declina.
8. **Attenzione ai cue substring.** `cue()` è substring: "eat" ⊂ "f-EAT-hers".
   Per i cue discriminanti, match a PAROLA INTERA.
9. **Il wall-rate non vede le risposte sbagliate.** Quando tocchi una classe,
   ispeziona a mano anche le risposte marcate "ok".
10. **Nessuna risposta prima di aver soddisfatto il piano.** Se il prompt contiene
   più richieste coordinate, il modulo deve costruire un `answer_plan` o declinare.
   Vietato rispondere al primo subgoal e ignorare il resto.
11. **Il formato è un vincolo semantico.** "two-line", "three ways", "one
    sentence", "simple terms", "as a list" vive in KB come `format_constraint/2`
    o relazione equivalente. Il post-shaper deve provare il formato.
12. **Ogni numero deve avere un ruolo.** Prima di fare aritmetica, lega i numeri a
    slot (`total`, `unit_price`, `paid`, `width`, `length`, `range_low`). Se non
    sai il ruolo, non calcolare.
13. **Preferisci event frames ai fatti sparsi.** Una domanda su WWII non è solo
    `ended_in(world_war_ii, 1945)`: è un evento con anno, luogo, attori, cause,
    conseguenze e oggetti correlati. La KB deve crescere per frame interrogabili.
14. **Ogni collisione diventa una guardia teachable.** Se un frame generico vince
    su uno specifico, non riordinare a mano soltanto: aggiungi un cue/registro KB
    che fa declinare il generico davanti alla classe compositiva.
15. **Un failure LLMSCORE vale come seed di fuzzing.** Dopo il fix, genera varianti
    della classe: sinonimi, ordine invertito, numeri diversi, multiword entities,
    formato diverso. Il test non deve coprire il prompt, ma il fascio.
16. **⚠️ Ciò che parrot0 DICE è conoscenza quanto ciò che legge.** Nessun testo
    rivolto all'interlocutore si scrive nel C — nemmeno un errore, un rifiuto o
    un messaggio meccanico. Ogni frase ha una famiglia `response_template`, e il
    test è: *«posso insegnargli a dirlo diversamente, parlando, e vale dal turno
    dopo?»* Un `snprintf(msg, …, "Got it - I'll treat …")` non lo passa.
    **Attenzione — questo mantra è nato perché i quattordici precedenti hanno
    lasciato passare 374 messaggi compilati:** il #2 elenca solo categorie
    d'ingresso (trigger, cue, sinonimi, unità, verbi) e nessuna d'uscita; il suo
    test («impararne un nuovo membro domani?») sembra non applicarsi a una frase,
    perché una frase non ha membri, quindi passa in silenzio invece di fallire; e
    `kb_say(b, chiave, "default", …)` **sembra** conforme perché la chiave c'è,
    mentre il letterale accanto è ciò che viene detto davvero. Fra il #2
    (l'ingresso) e il #11 (il formato richiesto) c'era un buco, ed è lì che sono
    passati. L'audit e la direzione stanno in
    `docs/plans/messages-are-knowledge.md`, i residui aperti in `C_TODO.md`.
17. **⛔ parrot0 deve poter imparare e cambiare comportamento in TUTTO ciò che
    fa — e questo include la decisione di CHI prende il turno.** È il corollario
    diretto di «tutto è KB»: se una condotta non è conoscenza, non è
    correggibile parlando, e allora non è conoscenza. Non ci sono eccezioni
    privilegiate — né il dispatch, né le guardie, né le priorità fra moduli.

    **Il test, nella forma che F. ha dato il 2026-09-01:** un dirottamento si
    deve poter riparare *dicendolo*, con una frase del tipo «il generatore di
    poesie non deve rispondere se non c'è anche X». Non è la forma a contare —
    è che la condotta sia **oggetto di discorso**. Se per correggere un modulo
    che ruba il turno bisogna ricompilare, quella condotta sta nel posto
    sbagliato esattamente come ci stava una lista di parole.

    **Attenzione — questo mantra è nato perché i sedici precedenti non lo
    coprivano, e per la stessa ragione strutturale del #16.** Il #2 nomina
    l'INGRESSO (trigger, cue, sinonimi); il #16 nomina l'USCITA (le frasi). In
    mezzo c'è la CONDOTTA — quando un modulo rivendica, con quale priorità, sotto
    quali guardie — e nessuno dei due la vede: una guardia non ha «membri» da
    imparare domani e non è «testo rivolto all'interlocutore», quindi passa in
    silenzio attraverso entrambi i test invece di fallirli. È lo stesso buco del
    #16, un piano più su.

    **Il criterio operativo:** ogni `if` nel C che decide *se* un modulo prende
    il turno deve poggiare su un fatto interrogabile (`move_policy/2`,
    `claim_guard/…`), mai su una condizione cablata. E la prova non è che il
    fatto esista: è che **parlando** si possa cambiarlo e che valga dal turno
    dopo. Un turno rubato è un bug di conoscenza, non di codice.
18. **⛔ SPOSTARE IL C NON È PORTARLO IN KB — e un template vuoto non è una
    resa.** Due camuffamenti che passano tutti i controlli precedenti perché
    *sembrano* conformi. Entrambi sono stati commessi il 2026-09-01 e stanno qui
    come esempio lavorato, non come teoria.

    **(a) Il test del bilancio.** Un commit che dichiara una migrazione KB-first
    deve mostrare il **C che si accorcia**. Se il C cresce più della KB, la
    migrazione non è avvenuta — qualunque cosa dica il messaggio. Il caso reale:

    | | |
    |---|---|
    | C | +398 / −75 = **netto +323** |
    | KB | +168 / −0 = netto +168 |
    | `30-generation-reading.c` | +155 / −62 = **il file è cresciuto di 93 righe** |

    Il messaggio diceva *«le cinque classi statiche … escono da C per entrare nel
    frasario KB»*. Erano uscite tre funzioni da `30-generation-reading.c` per
    entrare in `00-lex.c`: **spostamento fra file, non migrazione**. La misura di
    KB-first non è *in quale file* sta la logica — è *«parrot0 può impararne un
    nuovo membro domani senza ricompilare?»*. Cambiare l'indirizzo di una
    funzione non cambia la risposta a quella domanda.

    **(b) Il test del template vuoto.** Una resa il cui corpo è **solo un
    segnaposto** non porta conoscenza:

    ```prolog
    response_template(creative_text_answer, "{text}").
    response_template(riddle_answer_reply,  "{text}").
    ```

    Il C costruisce la frase intera, la passa a `kb_response_slots` e la riceve
    indietro identica: un `printf("%s")` in costume, un giro attraverso la KB che
    non cambia nulla. Il sito di chiamata **sembra** consultare la conoscenza e
    supera il grep del mantra #16, mentre ciò che viene detto è deciso altrove.

    **La prova, in una domanda:** *se cancello questa riga, cambia ciò che
    parrot0 DICE, o solo se lo dice?* Se cambia solo il «se», la frase vive
    ancora nel C e il template è un alibi.

    È la stessa trappola che il #16 già descrive per `kb_say(b, chiave,
    "default", …)` — la chiave c'è, quindi sembra a posto — **di un grado
    peggiore**, perché qui perfino la riga di KB è vuota di lingua.

    **Perché serviva un mantra in più:** il #2 guarda le liste di parole, il #16
    i letterali rivolti all'utente, il #17 le condotte. Nessuno dei tre guarda il
    **bilancio** di una modifica né il **contenuto** di una resa, e un
    rifacimento può quindi peggiorare la separazione superando tutti e tre.

19. **⛔ UNA CATENA DI `&&` O DI `||` NEL C E' CONOSCENZA COMPILATA — e i
    diciotto mantra precedenti la lasciavano passare tutti.** È la segnalazione
    di F. del 2026-09-03, e ha due gradini.

    **(a) Il gruppo di `||` sullo stesso argomento è SEMPRE una classe.**

    ```c
    lex_class_member(b, "..._lex2975",   t) ||   /* buys     */
    lex_class_member(b, "..._lex2975_2", t) ||   /* buy      */
    lex_class_member(b, "..._lex2975_3", t) ||   /* bought   */
    lex_class_member(b, "..._lex2976",   t) || …  /* gains, gain, gets, … */
    ```

    Undici classi private da **un membro ciascuna** per dire «verbo di
    acquisizione». Ognuna passava il grep del mantra #2 — la parola *sta* in KB —
    mentre la risposta alla domanda vera, *«parrot0 può impararne un nuovo membro
    domani?»*, restava **no**: il nome è un seriale legato al file C e alla riga,
    nessuno può pronunciarlo in una lezione, e la stessa parola `is` viveva in
    **79 classi diverse**, quindi insegnare un sinonimo della copula avrebbe
    voluto dire trovarne 79. Misura al gen489: **1245 classi `*_lex*`, tutte con
    esattamente un membro.** Non erano classi: erano `strcmp` con un altro
    indirizzo — il camuffamento del mantra #18(a), applicato mille volte.

    Il danno non è solo dottrinale. `clause_copula/1` esiste in `grammar.p0` con
    nove membri, «è» «sono» «era» «erano» compresi; il ramo della correzione
    negativa ne usava una copia privata che conosce solo `is`, quindi **non
    leggeva l'italiano pur avendo la KB per farlo**. Mantra #5 e #6 insieme.

    **La regola:** un gruppo di `||` sullo stesso argomento prende il nome del suo
    **ruolo** (`acquisition_verb`, `comparative_more`, `time_meridiem`), e quel
    nome deve essere **pronunciabile da chi insegna**. Un nome generato dal
    compilatore non è un nome.

    **(b) ⛔ Il gradino vero: la CONGIUNZIONE stessa deve essere una regola KB.**
    Con le classi al posto giusto si insegna un *membro* di un ruolo che esiste;
    **non si insegna una FORMA nuova**, perché quali condizioni, quante, in che
    ordine e con quale polarità restano compilate. Parole di F.:

    > *«se ha questa catena di `&&`, a runtime volessi aggiungere un nuovo
    > elemento tramite addestramento tu non puoi farlo, perché è la catena di
    > `&&` che deve diventare essa stessa una regola nella KB»*

    Finché la catena è nel C, **l'insieme delle forme che parrot0 può riconoscere
    è chiuso, e nessuna lezione lo apre.** Misura al gen489: 213 istruzioni con
    due o più `kb_cue_match` in `&&`, fino a **quindici congiunti in una sola**.

    La forma di arrivo, e il motore generico che la valuta:

    ```prolog
    turn_pattern(Forma, cue,     Classe).   % il turno porta una cue di Classe
    turn_pattern(Forma, not_cue, Classe).   % e non ne porta una di Classe
    turn_pattern(Forma, word,    Classe).   % un token e' membro di Classe
    turn_pattern(Forma, text,    "…").      % il turno contiene questa superficie
    turn_pattern_intent(Forma, Intento).    % che cosa vale il turno se tengono tutte
    ```

    La congiunzione è **l'insieme dei fatti che condividono il nome della forma**:
    il motore non sa quante siano né quali, le chiede. Perciò una forma nuova —
    con quante condizioni si vuole — è un gruppo di asserzioni a runtime e vale
    dal turno dopo. Il valutatore è `p0_turn_pattern_holds` in
    `src/brain/00-lex.c`, agganciato **dentro `kb_cue_match`**, cioè nella
    strozzatura da cui passano tutti i 1052 siti che chiedono «questo turno è di
    questa classe?». La prova è `tests/p0t/language/taught_turn_form.p0t`, con
    l'ablazione che toglie *una* delle due condizioni e fa tornare il difetto.

    **Come si lavora finché la migrazione non è finita.** Ogni catena ancora
    compilata porta sopra di sé un `TODO(kb-first, gen489)` che spiega il
    problema: sono **205** al gen489, e sono una **coda di lavoro**, non un
    archivio (moltiplicatore 1 di `LEARN_TODO.md` §−1). Quando tocchi un ramo che
    ne porta uno, lo chiudi lì: è il momento più economico in cui verrà mai a
    costare.

    **Due lezioni pagate durante la migrazione, e valgono per la prossima:**
    - **Mai collassare due seriali diversi che stanno nella STESSA condizione.**
      Se il sito li distingue, distingue i *membri* e non i ruoli: la prima
      versione produceva `!topic_preposition && topic_preposition` (sempre falso)
      e riscriveva «era» ed «erano» entrambi in `is`.
    - **Allargare una classe è allargare una RIVENDICAZIONE.** `entity_pronoun`
      ha ricevuto «i», «we», «you» e metà della chat italiana ha cominciato a
      rispondere *«What number should I use for «you»?»*: le due classi
      rispondono a domande diverse — `entity_pronoun` a *«questo pronome ha
      bisogno di un antecedente?»*, e un deittico non ne ha mai. **Chi vede di
      più deve anche distinguere di più**, ed è il corollario del #17.

20. **⛔ LA KB CRESCERÀ SEMPRE: IL CARICO NON È UN INCIDENTE, È UNA CONDIZIONE.**
    F., 2026-09-03: *«la nostra KB deve crescere e crescerà, quindi i problemi di
    carico li avremo sempre»*. Ne segue una regola che vale prima di ogni
    aggiunta di conoscenza, e una diagnosi che vale prima di ogni ottimizzazione.

    **(a) Quando una classe di conoscenza non si può allargare, il difetto non è
    quasi mai nella classe.** Aggiungere cinquanta verbi a `relation_verb/1`
    mandava in timeout metà della suite, e la conclusione ovvia — *«sono troppi
    verbi»* — era sbagliata. Il costo non era la lista: era che
    `extract_frame/2`, che da ogni verbo genera due schemi, veniva **riderivato
    442 volte per turno**. Congelato quel processo, la stessa lista passa senza
    toccare un gate. **Prima si guarda che cosa RILEGGE la classe, e quante
    volte.**

    **(b) Si profila, non si indovina.** `/debug` accende un profilo per
    predicato che esisteva da generazioni e non era mai stato puntato lì. Un
    solo predicato era l'**83%** del turno (890 ms su 1075); tutto il resto stava
    sotto i 25 ms. Nessuna intuizione ci sarebbe arrivata, e due delle tre
    ottimizzazioni tentate a intuito hanno *peggiorato* il tempo.

    **(c) Il meccanismo: `materialized_view/2`.** Le soluzioni di un predicato
    dichiarato si enumerano una volta e si congelano come fatti `KB_DERIVED`.
    La cache è la tabella dei fatti, la kv-con-hash è l'indice che c'era già,
    l'early match è il bucket del censimento — **nessuna struttura nuova**. E
    *quali* processi congelare è conoscenza, non una lista nel C.

    **(d) Tre trappole pagate, e sono la parte riusabile:**
    - **Dentro una risoluzione la conoscenza non si tocca.** Materializzare
      asserisce, quindi rialloca la tabella dei fatti mentre i chiamanti sopra
      hanno in mano `const Fact *`: due core dump prima di separare il
      *controllo* (dentro il solver) dalla *costruzione* (agli ingressi).
    - **La chiave della cache dev'essere stretta.** Con `kb_revision` — che
      conta ogni fatto, comprese le tracce che ogni turno asserisce — la vista
      si ricostruiva più volte per turno e il tempo **saliva** da 1058 a 1725 ms.
      *Una cache invalidata da ciò che non la riguarda è peggio di nessuna
      cache.*
    - **E la chiave dev'essere O(1).** Calcolarla contando i fatti a mano metteva
      730 ms *fuori* dal solver: la cache costava più della derivazione che
      evitava. È lo stesso errore del §L — *«un'exit condition che costa quanto
      ciò che evita non è un'ottimizzazione»* — un piano più su.

    **(e) «Una volta» deve cadere dove non c'è un budget.** Il costo del
    congelamento si paga al boot (`kb_views_warm`), non nel primo turno che
    chiede lo schema — lì sono due secondi e un timeout.

    **(f) E la cache non deve poter mentire.** La scadenza non è un tempo: è la
    conoscenza da cui la vista dipende (`view_depends/2`). Un verbo insegnato
    adesso invalida la vista e il suo schema è leggibile **nello stesso turno**.
    Il cricchetto `tests/p0t/engine/materialized_view.p0t` lo prova nei due
    versi, e prova anche che togliendo la dichiarazione il motore torna a
    derivare: **una vista è un acceleratore dichiarato, mai una parte del
    significato.**

## Evoluzione KB richiesta per LLMSCORE-max

La KB di parrot0 deve passare da "grande dizionario di fatti interrogabili" a

---

## La sintassi `.p0`, esaustiva

[`docs/parrot-p0-syntax.md`](docs/parrot-p0-syntax.md) documenta **tutta** la
sintassi che questi mantra presuppongono: clausole e direttive, i builtin con
le loro trappole (`findall` è un insieme, vuole un template variabile e un
risultato libero, e una virgola in un testo raccolto spezza la lista; `naf`
declina sotto guardia; arità 4, corpo 16), le virgolette e la corrispondenza
nel C, le `turn_form` con i loro pezzi e atti, i `response_template`, le
cessioni (`faculty_yield`, `turn_declared_act`), il tabellone e i contabili, e
le pratiche di buona scrittura. Prima di scrivere una riga di KB si legge quello;
prima di scrivere una riga di C si passano i mantra qui sopra.

## Come verificare in fretta

`make soft-test` è la verifica di **avanzamento** — non la suite. Poche decine di
casi, budget 15 secondi: se sfora, si tolgono casi, non si alza il budget. La
suite intera è `make test`.

## ⛔ TODO aperti, se stai cercando da dove ripartire

> ✅ **TRAGUARDO — LO SPAZIO DEL DISCORSO (2026-08-31).** parrot0 ricorda ora
> *che cosa* è stato nominato e *in che ordine*, e «il primo»/«il secondo» ci si
> attaccano. È il posto a cui si agganciano coreferenza, ellissi, correzione,
> ambiguità dicibile e soggetto eliso — tutte cose che prima non avevano
> appiglio. **Le tre lezioni riusabili di come è stato costruito:** cerca il
> punto di strozzatura che tutte le vie attraversano invece di enumerare i
> chiamanti; quale posizione introduca un referente è una *politica*, non una
> scelta del C; e la superficie da dichiarare è **quella che sopravvive al
> percorso** (terza volta che si ripresenta). Dettaglio in testa a
> `docs/plans/universal-comprehension.md`.

> ⛔⛔ **IL CASSETTO SENZA MANIGLIA (2026-08-31).** «Il libro rosso è sul tavolo»
> produceva `located_in(book_red, tavolo)`, e «dove si trova il libro rosso» era
> un muro: il lettore lega un SINTAGMA, la domanda provava un token alla volta.
> parrot0 imparava sotto un nome che non sapeva più pronunciare, e ogni entità di
> più di una parola finiva in un cassetto senza maniglia — ed è il motivo per cui
> due giri di insegnamento massiccio avevano mosso +11 turni su 360. Il gradino
> G1 è chiuso (la domanda prova i sintagmi del lettore); restano G2-G5 —
> testa/proprietà, referente, coreferenza, ridirsi. Il piano completo è in testa
> a `docs/plans/universal-comprehension.md`, `universal-input.md`,
> `apprendimento-assistito.md` e `LEARN_TODO.md`.
>
> **La forma ricorrente, ed è la lezione:** tre volte lo stesso difetto sotto
> vestiti diversi (D33, D35, D37) — *due percorsi che devono accordarsi e non
> condividono l'oggetto su cui accordarsi.* Prima di aggiungere una capacità:
> **chi altro deve accordarsi con questa, e su che cosa?**

> ⛔⛔ **PRIMA DI TUTTO, dal 2026-08-30: le letture di parrot0 sono CONGELATE.**
> Rileggere un testo dopo aver imparato una parola nuova **accumula** una seconda
> lettura invece di rivedere la prima, e le due restano vive insieme. Il limite
> vero non e' il test che ne soffre: e' che **parrot0 non puo' rileggere cio' che
> ha gia' letto alla luce di cio' che ha appena imparato**. E' il punto in cui
> migliorare lo renderebbe piu' *intelligente*, non solo piu' capace. Vedi
> `docs/plans/frontier-kb-natural-dialogue.md` §0 dei TODO aperti e §18.37 (D33),
> voce `LEARN_TODO.md` **SC40**.
>
> **Regola che ne discende:** quando un test ha bisogno di un dato fresco per
> passare, chiediti **prima** se il sistema abbia bisogno di dimenticare o di
> **rivedere**. Aggirare un limite e descriverlo sono compatibili; aggirarlo e
> chiamarlo intenzionale no.

`KB_TODO.md` in testa porta l'elenco dei residui del piano
`docs/plans/frontier-kb-natural-dialogue.md` — copertura funzionale completa, ma
il §9 (confronto empirico), la latenza del §10 e il censimento di
`docs/plans/parrot0-100-failures.md` restano aperti. Il metodo per chiuderli e'
quello delle sonde: `tests/*_probe.py` scopre la MOSSA di un modello di
frontiera, la KB la riproduce come regola, il `.p0t` la tiene ferma. Chiudere un
prompt senza chiudere la sua classe non conta come progresso.

21. **⛔ RIVENDICARE UN TURNO È UN TITOLO, NON UN DIRITTO ACQUISITO — e il
    titolo è la MATURITÀ DELL'IMPLEMENTAZIONE, dichiarata in testa al modulo.**

    F., 2026-09-04: *«i moduli obsoleti che rubano turni non sono legittimati a
    farlo. Un modulo può continuare a prendere turni se è addestrabile, se è
    KB-first, se è basato sulla comprensione universale. Non esistono moduli che
    rubano turni per stato del codice.»* E, sul criterio: *«è la review della sua
    implementazione, che deve stare in testa al modulo in un commento. Un modulo
    pieno di TODO KB-first non ha diritto di rubare turni, tranne dove risulta un
    fallback per gli altri. Dobbiamo premiare la competenza e la maturità.»*

    Il #17 dice che la condotta di dispatch dev'essere **correggibile parlando**.
    Questo è un piano più su e diverso: dice **chi ha titolo a parlare**. Una
    facoltà che vince perché è arrivata prima nell'array — perché esiste da più
    tempo, perché nessuno l'ha ancora governata — non esercita una **capacità**:
    esercita una **posizione**, cioè lo stato del codice travestito da condotta.

    ### ⭐ I due fallimenti sono classi diverse, e hanno rimedi diversi

    | chi vince a torto | che cos'è | il rimedio |
    |---|---|---|
    | un modulo **immaturo** — vecchio, pieno di `TODO(kb-first)`, vocabolario compilato | ⛔ **un errore di architettura** | **retrocederlo** a ultima risorsa. **Non gli si insegna niente** |
    | un modulo **maturo** — addestrabile, KB-first, fondato sul frame | 🟡 **un comportamento da correggere** | **insegnarglielo**: una cue, una regola, una cessione |

    ⛔ **Confondere le due classi è la causa del whack-a-mole**, ed è un errore
    che è stato commesso il giorno stesso in cui questo mantra è nato: scrivere
    una `faculty_yield` per un modulo immaturo significa *insegnare una condotta
    a chi non aveva titolo a pretendere*. Non lo rende maturo — lo lascia libero
    su tutte le altre classi, e il turno dopo lo ruba altrove.

    **Prima di scrivere una cessione, chiedersi in quale classe cade il furto.**
    Se il modulo è immaturo, la cessione è la risposta sbagliata anche quando
    funziona.

    ### La review, e perché non può mentire

    In testa al modulo, in un commento: maturità (`legacy` / `transitional` /
    `kb_first`), diritto (`primary` / `fallback` / `none`), e le tre prove —
    **L1** addestrabile (si ritira una sua forma di riconoscimento e la pretesa
    deve sparire), **L2** KB-first (quanti confronti di parole letterali restano
    nel C), **L3** universale (pretende sul *frame* del turno o su una
    sottostringa del grezzo). Ogni campo ha un controllo meccanico accanto:
    una testata che dichiara `kb_first` con dieci literali e due TODO aperti
    **fallisce un cricchetto**. È una review, non un'autocertificazione.

    Il commento è per gli umani; l'arbitrato ha bisogno di fatti, quindi la
    review si proietta in KB come già fa `src/brain/00-lex.c.cues.p0`. Così sta
    in testa al modulo **e** resta interrogabile: *«perché non hai risposto
    tu?»* → *«non ho titolo: sono transitional»*. Correggibile, sì — ma solo
    **migliorando il modulo**, che è esattamente il punto.

    ### L'evidenza che il criterio è misurabile

    Sui quattro moduli che si sono contesi un prompt di coding il 2026-09-04
    (literali nel C contro letture dalla KB): `codeast` **1:61** ha dato l'unica
    risposta onesta — *«I read that as code, but I am not sure which function you
    mean»* — mentre `compose` **10:83**, il rapporto peggiore, è stato il primo
    ladro. **Il modulo con meno vocabolario compilato è quello che ha risposto
    bene.** Quattro casi non sono una legge, ma sono il verso previsto.

    ⛔ **F., 2026-09-18 — il furto è un difetto COGNITIVO, non un incidente.**
    *«Il furto è il segnale di un modulo che non sta usando la comprensione
    universale … quando un modulo ruba un turno è un problema cognitivo …
    voglio che questi incidenti siano gestiti con un piano che li risolva una
    volta per tutte, con l'idea di portare tutto in KB. Il chitchat è un
    frasario in KB, non un comportamento cognitivo di cui la KB è pregna.»*
    Un turno rubato è un turno a cui parrot0 ha risposto **senza averlo
    letto**; le parole in KB (#2, #16) non bastano se la **decisione** di
    parlare è un confronto di sottostringhe e non una lettura della IR.
    Misurato quel giorno: **74 facoltà su 80 non toccano mai il frame**, 5
    recensite su 80. Un furto non si sistema per andare avanti: si registra
    con il nome del modulo (`P0_PROBE_WHO=1` nel banco della prosa) e si
    chiude per **specie** — A consumatore KB della IR, B frasario in KB con
    la pretesa nel C, C lettore privato — con il rimedio di ciascuna in
    `turn-arbitration.md` §1-bis.1-ter. Il default cambia verso: senza
    lettura del frame e senza review, una facoltà è `fallback`.

    Ordine delle discipline e piano: `docs/plans/turn-arbitration.md`.

22. **⛔ MASSIMIZZARE E DECLINARE — un circuito che funziona non si lascia a un
    caso solo.**

    F., 2026-09-06: *«quando troviamo un circuito che funziona, un percorso, una
    soluzione su cui si sono investite ore, allora la dobbiamo massimizzare —
    riempirla di casi completi, varianti, temi lunghi di ogni genere — e
    declinare: quell'abilità ne può portare un'altra simile, sovrapponibile, con
    terminazioni alternative e interconnessa ad altre. Così, dato un
    investimento x di tempo, la crescita della KB è rilevante.»*

    Funziona per un'asimmetria misurata: **il circuito è la parte cara, i casi
    sono la parte gratis.** `turn_focus` è costato nove cicli diagnostici su due
    sessioni; il decimo caso che ci si mette dentro costa una riga. Chi si ferma
    al primo caso paga il prezzo pieno e ne raccoglie un nono.

    > **Massimizzare** = esaurire i **casi** di una distinzione.
    > **Flettere/declinare** = trovare la **stessa forma** di distinzione altrove.

    ### ⛔ Il difetto successivo è sempre più attraente di quello in corso

    È la tentazione che questo mantra esiste per battere, e non è teorica:
    misurata sul giorno stesso in cui è nato. `turn_focus` costruito, verificato
    su **un** turno, e sessione chiusa. Rimisurato subito dopo, il circuito era
    a metà: `in`/`within`/`among`/`under` e perfino una forma di domanda diversa
    passavano già; `for` e `of` **mentivano ancora**, l'annidamento italiano
    murava, e una terza facoltà rispondeva altro.

    Cinque casi erano lì gratis e non li avevo raccolti.

    ### Le due discipline, senza cui degenera nei difetti che già conosciamo

    | | il test | se fallisce |
    |---|---|---|
    | **massimizzare la CLASSE, non i casi** | *il membro nuovo entra in una classe che uno straniero potrebbe estendere domani, o è una riga in più in una lista?* | è una lista → il circuito non era finito: **mancava la classe**. È così che sono nate le 73 cue corte |
    | **una declinazione RIUSA la lettura, non la copia** | *sto aggiungendo un consumatore a una lettura che c'è, o sto scrivendo una seconda lettura?* | seconda lettura → non è una declinazione, è un **duplicato** (#5), e divergerà al primo cambiamento |

    ### Quando un circuito è pieno

    Non «quando la classe sembra completa»: dall'interno nessuno sa che aspetto
    abbia una classe completa. La sola condizione d'arresto onesta è
    comportamentale — **quando i casi nuovi smettono di cambiare il
    comportamento su un transcript tenuto da parte.**

    E ciò che resta fuori non è sempre incompletezza: `of` sta fuori da
    `domain_preposition` perché **fa parte del soggetto** («the capital of
    France»). Chiuderlo chiede di leggere la *testa* del sintagma — cioè è una
    **declinazione**, non una massimizzazione: circuito nuovo, sessione dopo.

    ### ⭐ Perché questo mantra separa due mestieri

    **Massimizzare è il momento in cui il lavoro smette di essere ingegneria e
    diventa insegnamento.** Costruire il circuito richiede il motore, il C, i
    cicli diagnostici. Riempirlo è conoscenza: si fa **parlando**, ed è il punto
    dell'intero esperimento.

    | | costruire | massimizzare / flettere |
    |---|---|---|
    | che cos'è | ingegneria | addestramento |
    | costo | ore, cicli diagnostici | minuti, per voce |
    | dove | `src/` + `kb/` | `kb/`, spesso solo parlando |
    | per sessione | **una volta** | fino a saturazione |
    | protocollo | `docs/plans/procedura-crescita-kb.md` | `LEARN_PROTOCOL.md` |

    ### Il ritmo che ne segue

    1. **un circuito per sessione, al massimo** — se ne saltano fuori due, il
       secondo si **scrive**, non si insegue;
    2. **poi si massimizza, per voce, fino a saturazione** — è qui che la KB
       cresce davvero, con costo per caso vicino a zero;
    3. **poi si cerca una declinazione** e la si passa al test del riuso: se
       riusa la lettura si fa adesso; se chiede un meccanismo nuovo **diventa il
       circuito della sessione dopo**.

    Così l'investimento non finisce mai in un caso solo, e la coda del lavoro è
    sempre già scritta.

    Il metodo completo, con le misure: `docs/plans/procedura-crescita-kb.md`.

23. **⛔ FATTO GIUSTO, DOMANDA GIUSTA, RISPOSTA SBAGLIATA: PRIMA DI INCOLPARE IL
    LETTORE O LA DOMANDA, CERCA LA DIMENSIONE CHE ALLA KB MANCA PER DESCRIVERE
    SE STESSA.**

    F., 2026-09-13, notte, dopo il caso: *«il problema sembrava la IR o la
    domanda, e invece aumentare la granularità descrittiva della KB ha dato una
    leva risolutiva»*.

    **Il caso.** «Reefs are formed of colonies» → `made_of(reefs, colonies)`,
    corretto. «what are colonies made of?» → **«Reefs.»**. Le due diagnosi
    ovvie erano entrambe plausibili: la IR non porta metadati sui ruoli; la
    domanda non discrimina il verso. La prima era falsa (il fatto aveva un verso
    e non aveva perso niente). La seconda era vera ma portava a una regola per
    ogni forma di domanda, e ogni forma dimenticata sarebbe tornata bugia. La
    leva è stata una terza cosa: la KB **non sapeva di che specie fosse la
    relazione**. Una riga detta in chat — `"made of" is a whole-part relation`
    — ha chiuso la bugia per ogni domanda su quella superficie, e ritrattandola
    torna (`tests/p0t/language/prose_triage.p0t`).

    **La regola.** Quando due pezzi che fanno bene il proprio lavoro producono
    insieme un errore, il difetto non è quasi mai in uno dei due: è
    **l'oggetto su cui dovrebbero accordarsi, e che nessuno dei due può
    consultare perché non esiste**. È la forma ricorrente di D33/D35/D37
    (`LEARN_TODO.md`, «due percorsi che devono accordarsi e non condividono
    l'oggetto su cui accordarsi»), e questo mantra ne dice dove cercarlo:
    spesso è **conoscenza sulla conoscenza** — la specie di una relazione, il
    suo verso, la sua simmetria, il contesto o la quantità rispetto a cui vale.

    **Il test, prima di scrivere codice:** *«esiste una proprietà della
    relazione (o del fatto) che, se fosse dichiarata, renderebbe ovvia la
    risposta giusta a entrambi i pezzi?»* Se sì, quella proprietà va in KB come
    classe **insegnabile con le parole di chi insegna**, e i due pezzi la
    consultano. Una cura in uno dei due pezzi, al posto suo, è il whack-a-mole:
    si ripresenta alla forma successiva.

    **Perché è un mantra e non una lezione.** Si applica prima di ogni modifica,
    nomina un errore di *diagnosi* (non di codice) che i ventidue precedenti
    lasciano passare — il #2 guarda le parole d'ingresso, il #16 quelle
    d'uscita, il #17 la condotta, il #19 le congiunzioni, ma nessuno chiede
    *che cosa la KB sa di sé* — e aumenta ciò che parrot0 vede invece di
    ridurre ciò che un pezzo vede (il criterio di evoluzione in testa a questo
    file). ⚠ **Stato: nato da un esperimento solo.** Va confermato al prossimo
    caso della stessa forma; se il prossimo si cura meglio nel lettore o nella
    domanda, questo testo va corretto, non difeso.

    Il seguito (specie delle relazioni, dinamicità, relativizzazione) sta in
    `docs/plans/the-magic-of-apply.md`, Parte VII.

24. **⛔ UN LETTORE FUORI DALLA IR È UN'ESPLORAZIONE CON SCADENZA, NON CRESCITA
    — e la crescita si misura sulle varianti, mai sul caso.**

    F., 2026-09-14, dopo l'esperimento età e incontri e il piano multi-hop:
    *«cosa perdiamo se facciamo cose non fatte con la IR? è davvero un vantaggio
    o è solo una mia fissazione architetturale?»*

    **Il caso.** Un problema di logica (Anna, Bruno, Carlo, confronti d'età su
    gruppi descritti da incontri) risolto in un'ora con un lettore per posizioni
    di token: 0 predicati strutturali della IR, 38 letture dei token, una IR
    privata (pezzi, descrizioni, frame, scope numerati) accanto a quella
    universale. Il prompt: verde. Tre varianti minime — un altro verbo
    («conobbe»), un'altra dimensione («più alta»), il caso più semplice con tre
    nomi — tutte rosse, e con un fatto falso imparato al posto della risposta.
    `docs/sessions/2026-09-14-esperimento-ordine-eta.md` §8–9.

    **Che cosa si perde fuori dalla IR** (misurato, non teorico):
    *la generalizzazione* — un lettore privato riconosce le forme che conosce,
    un consumatore riceve «una relazione», «un ordine», «un'entità»;
    *l'insegnamento a voce* — una forma nuova diventa una regola nel file, non
    una lezione;
    *la crescita composta* — due missioni che hanno bisogno degli stessi pezzi li
    costruiscono due volte, e i progressi dell'una sono invisibili all'altra;
    *la coerenza* — lettori diversi leggono lo stesso turno in modi diversi e
    vince chi afferra per primo («Thus was a mysterious Thus»);
    *la traccia* — con la IR la ricostruzione è una vista, senza è un registro che
    può divergere dal ragionamento;
    *la possibilità di mescolare* premesse, fatti del mondo e letture.

    **Che cosa costa la IR** (va detto, o la regola diventa dogma): tempo subito,
    peso sul turno, il rischio di un minimo comune denominatore se la si progetta
    prima di sapere che cosa serve, un punto unico di rottura. Per questo la
    regola non vieta il lettore privato: lo **nomina**.

    **La regola.**
    1. Un lettore o una struttura fuori dalla IR è ammesso **solo come
       esplorazione dichiarata**: nel commento del file e nel resoconto si scrive
       che è debito, e perché si è scelto di aggirare invece di far crescere la IR.
    2. L'esplorazione **scade** con due consegne: la tabella dei gemelli (ogni
       pezzo privato → il suo posto nella IR) e la migrazione, nella stessa
       missione o nella successiva. Finché non è migrata, non conta come capacità
       di parrot0 in nessun resoconto e in nessuna demo.
    3. **Una capacità esiste quando passa un banco di varianti della sua classe**,
       scritto *prima* della cura: un altro verbo, un'altra dimensione, un'altra
       lingua, la forma più semplice, un caso con i distrattori. Il caso da cui si
       è partiti non è una prova: è il primo membro del banco.
    4. In una demo si può mostrare un'esplorazione solo dicendolo.

    **Il test, prima di scrivere il lettore:** *«la struttura che sto per
    costruire ha già un nome nella IR, o dovrebbe averlo?»* Se sì, si fa crescere
    la IR e si scrive un consumatore. Se si sceglie di aggirare, si scrivono
    subito la riga di debito e la sua scadenza.

25. **⛔ NON SI PENSA IN UN SECONDO CERVELLO: OGNI ASTRAZIONE STA DENTRO LA
    MENTE UNICA, COME STRATO.**

    F., 20 settembre 2026, trovando un sandbox ancora vivo:
    *«abbiamo più volte sperimentato che l'utilizzo di istanze di brain al fine
    di operare in isolamento è un errore forte — il nostro cervello non lavora
    così: ogni astrazione è dentro il cervello, non in un cervello secondario.
    Questo crea degli handicap di crescita del progetto.»*

    La critica non è nuova ed è già scritta per esteso in
    [`docs/plans/one-kb.md`](docs/plans/one-kb.md) §3 e §6: *«`brain_scratch_init`
    non va perfezionato: va fatto sparire, e con lui l'ultimo posto in cui
    parrot0 pensa da menomato»*. Qui torna viva perché un residuo la stava
    ancora aggirando.

    **L'esigenza è legittima, lo strumento no.** Le premesse di un ipotetico
    («if all cats are mammals and Tom is a cat, is Tom a mammal?») devono valere
    per quel turno e non sporcare la conoscenza: giusto, ed è closed-world sulle
    premesse. Ma isolare *ricominciando da zero* — un secondo `Brain` sopra un
    `kb_create()` nudo — butta via anche grammatica, classi lessicali e
    instradamento: il sandbox non distingueva un articolo da un sostantivo.

    **L'handicap di crescita, misurato (one-kb.md §1).** Ogni classe portata dal
    C alla KB doveva lasciare nel C una lista di parole di riserva, perché nel
    cervello secondario la lookup non trovava niente: si toglieva l'inglese dal
    motore e lo si rimetteva accanto. La migrazione KB-first non poteva chiudersi.

    **La forma giusta è lo STRATO.** Le provenienze esistono già
    (`KB_BASE`, `KB_SESSION`, `KB_INDUCED`, `KB_REFLECTIVE`, `KB_HYPOTHETICAL`) e
    oggi valgono in scrittura e non in lettura: `kb_save` sa restringersi a uno
    strato, `kb_query` no. La cura è portare le provenienze anche in lettura —
    asserisci in `KB_HYPOTHETICAL`, interroga con quello strato più la
    macchineria, ritira lo strato a fine turno — e `brain_scratch_init` sparisce
    invece di essere mantenuto. Il percorso migliore già fa così
    (`one_turn_syllogism` applica le premesse sul cervello vero in strato
    ipotetico); i `brain_scratch_init` rimasti sono il debito da chiudere.

    **E la misura non è «isolare meglio» (one-kb.md §5b).** Un LLM sullo stesso
    item non isola niente: vede tutto e **sceglie**. L'obiettivo non è un
    sandbox più pulito, è una decisione presa dalla conoscenza.

    **Il test, prima di creare un contenitore:** *«mi serve una VISTA ristretta o
    un SOGGETTO diverso?»* È sempre la prima: e una vista si ottiene con uno
    strato, mai con un altro cervello.

26. **⛔ UNA SOLUZIONE VALE DI PIÙ SE AMPLIA CIÒ CHE SI PUÒ INSEGNARE.**

    F., 20 settembre 2026, durante il lavoro sul filosofo razionale: espandere
    lo strato di insegnabilità avvalora un'ipotesi e una soluzione. Il valore
    non è soltanto il caso risolto: è la nuova distinzione che un interlocutore
    potrà insegnare domani, senza conoscere lo schema interno e senza cambiare
    il motore.

    **Il test prima della proposta:** *«dopo questa modifica, quale lezione
    nuova potrà ricevere parrot0, e quale comportamento potrà cambiare grazie
    a quella lezione?»* Scrivere la lezione in lingua naturale, il trasferimento
    a un caso diverso e la ritrattazione che deve toglierne l'effetto. Se non
    si riesce a farlo, non chiamare la soluzione «più generale» soltanto perché
    ha più parametri, più righe di KB o nomi più astratti.

    **Non basta rendere insegnabile una parola.** Chiedersi se la stessa porta
    permetta di insegnare un ruolo, una condizione d'uso, una mossa, il motivo
    per sceglierla e la sua resa. Una soluzione fertile apre composizioni fra
    queste distinzioni: la nuova superficie arriva a una condotta già
    interpretabile; la nuova condotta riusa lettura, contesto e inferenza.
    Evitare un teach-handler C per ciascuna nuova capacità.

    **È un criterio di selezione, non una prova automatica di verità.**
    L'insegnabilità avvalora l'ipotesi solo se le lezioni cambiano davvero il
    comportamento e conservano correttezza, pertinenza e provenienza. Una
    risposta falsa diventata configurabile resta falsa; una conferma di
    apprendimento senza replay non è una capacità.

    Quando entra una nuova forma di insegnamento, aggiornare nello stesso
    lavoro `LEARN_PROTOCOL.md` con sintassi pronunciabile, esempio verificato,
    effetto, ritrattazione e limiti. Distinguere sempre forme operative e
    contratti ancora progettati. La documentazione è la maniglia con cui il
    prossimo interlocutore può usare la crescita.

## Dove sta il resto

- `PRINCIPLES.md` — il *perché* dell'esperimento (la regola anti-inganno).
- `AGENTS.md` — le regole operative per chi modifica parrot0.
- `docs/plans/motorize-the-class.md` — la fonte originale di questi mantra, con
  il contesto dei fallimenti reali che li hanno generati.
- `docs/plans/one-kb.md` — la KB è parte di parrot0, non un volume montato.
- `docs/sessions/` — i resoconti di sessione: cosa è stato scoperto e perché.
