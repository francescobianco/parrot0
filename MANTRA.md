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

## Evoluzione KB richiesta per LLMSCORE-max

La KB di parrot0 deve passare da "grande dizionario di fatti interrogabili" a

---

## Come verificare in fretta

`make soft-test` è la verifica di **avanzamento** — non la suite. Poche decine di
casi, budget 15 secondi: se sfora, si tolgono casi, non si alza il budget. La
suite intera è `make test`.

## ⛔ TODO aperti, se stai cercando da dove ripartire

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
