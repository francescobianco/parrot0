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

## Evoluzione KB richiesta per LLMSCORE-max

La KB di parrot0 deve passare da "grande dizionario di fatti interrogabili" a

---

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

## Dove sta il resto

- `PRINCIPLES.md` — il *perché* dell'esperimento (la regola anti-inganno).
- `AGENTS.md` — le regole operative per chi modifica parrot0.
- `docs/plans/motorize-the-class.md` — la fonte originale di questi mantra, con
  il contesto dei fallimenti reali che li hanno generati.
- `docs/plans/one-kb.md` — la KB è parte di parrot0, non un volume montato.
- `docs/sessions/` — i resoconti di sessione: cosa è stato scoperto e perché.
