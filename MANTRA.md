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
   **Estensione procedure:** se il fix introduce una trasformazione o un calcolo
   di classe, non fermarti a mettere cue/template in KB: cerca prima una procedura
   insegnabile in `kb/core/procedures.p0` sopra i primitivi (`is/2`, confronti,
   termini composti). Il C deve restare adattatore NL→goal o primitiva generale,
   non consumer della procedura di dominio.
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

## Evoluzione KB richiesta per LLMSCORE-max

La KB di parrot0 deve passare da "grande dizionario di fatti interrogabili" a

---

## Come verificare in fretta

`make soft-test` è la verifica di **avanzamento** — non la suite. Poche decine di
casi, budget 15 secondi: se sfora, si tolgono casi, non si alza il budget. La
suite intera è `make test`.

## Dove sta il resto

- `PRINCIPLES.md` — il *perché* dell'esperimento (la regola anti-inganno).
- `AGENTS.md` — le regole operative per chi modifica parrot0.
- `docs/plans/motorize-the-class.md` — la fonte originale di questi mantra, con
  il contesto dei fallimenti reali che li hanno generati.
- `docs/plans/one-kb.md` — la KB è parte di parrot0, non un volume montato.
- `docs/sessions/` — i resoconti di sessione: cosa è stato scoperto e perché.
