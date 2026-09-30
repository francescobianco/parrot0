# Handoff L4 — anatomia della KB, patch aperta e challenge locale

**30 settembre 2026. Ripartire da questo documento.** Il piano e' stato
approfondito, due baseline con LFM locale sono complete, una patch sperimentale
e' nel working tree. **Il miglioramento NON e' ancora dimostrato:** non sono
stati eseguiti replay dopo la patch ne' una nuova conversazione dopo la patch.
Build e caricamento della KB riusciti non cambiano questo stato.

## 1. Mandato da conservare

F. ha chiesto, nell'ordine:

1. Lavorare [l4-upgrade.md](l4-upgrade.md) dalle sue premesse, entrare
   nell'anatomia della KB e capire come la sua crescita possa espandere la
   comprensione. Usare [llm-challenge.md](llm-challenge.md) per leggere le
   lacune. Non ridurre il troubleshooting a un problema di metodo e non
   perdersi in dettagli o test inutili.
2. «concretizza le tue intuizioni dimostrando che la challenge con llm locale
   viene migliorata».
3. Ora: un handoff dettagliato che permetta a un altro agente di continuare.

**La consegna 2 resta aperta.** Questo documento trasferisce il lavoro, non
ne certifica il successo. L'implementazione e le misure pertinenti sono gia'
autorizzate dalla richiesta di concretizzare: non ripartire chiedendo una
conferma generica. La vecchia prescrizione di allineamento nel piano va letta
insieme a questa successiva autorizzazione. Nessun commit e' stato eseguito.

Leggi prima [MANTRA.md](../../MANTRA.md), [PRINCIPLES.md](../../PRINCIPLES.md),
[LEARN_PROTOCOL.md](../../LEARN_PROTOCOL.md), quindi la sezione **Indirizzo
attuale — anatomia e crescita della comprensione** del piano L4. Gli handoff
piu' vecchi nei due piani sono storia: non ricominciare dai loro elenchi come
se fossero lo stato corrente.

## 2. Stato materiale: che cosa preservare

Checkout di partenza: `7ffe7631`. Il working tree iniziale era pulito.

| file | stato e significato |
|---|---|
| [l4-upgrade.md](l4-upgrade.md) | revisione sostanziale gia' svolta: anatomia, residui, crescita condizionata, ordine di lavoro; preservarla anche se si abbandona la patch |
| [turn-frames.p0](../../kb/core/turn-frames.p0) | patch sperimentale: scelta del lettore composto, confini condivisi, ricevute per clausola e gap parziale |
| [99-registry.c](../../src/brain/99-registry.c) | patch sperimentale: ingresso anticipato del composto, punteggiatura e offset conservati, pubblicazione delle ricevute |
| [baseline standard](../sessions/talk/2026-09-30-l4-growth-base.log) | 12 scambi reali prima della patch |
| [baseline trasferimento](../sessions/talk/2026-09-30-l4-growth-transfer-base.log) | 12 scambi reali prima della patch, apertura sul parco |

I log erano untracked alla preparazione del handoff: includerli nell'eventuale
consegna/commit. Il primo log contiene tre righe iniziali di un avvio fallito
per rete sandbox (`avvio`, `pronto`, `fine`), poi la sessione completa riuscita.
Contare i 12 `M>`/`P<`, non le intestazioni. Nessun punteggio nuovo e' stato
ancora assegnato sistematicamente: quelli dei giri storici non sono i punteggi
di queste baseline.

Una fotografia **completa** di prima della patch si trova in
`/tmp/parrot0-l4-growth-before/`: `kb/`, `bin/parrot0`, `world.p0`. Non e' una
KB ridotta. Serve per confrontare processi prima/dopo sullo stesso ingresso;
non introduce un secondo cervello nel motore. E' temporanea: se si cambia
macchina o si pulisce `/tmp`, conservarla prima o ricostruire esattamente la
versione iniziale. Non copiarci sopra la KB modificata.

SHA256 del binario prima:
`03b922fbe550aabbcaf4e306b7c822f3f77812a71f58c3c894098bbc3ac3c69c`.
Binario compilato al handoff:
`cbce933630a50f4614f5bc974818371e16025a7b89e14c8ea0cd9c6b4b2698c8`.
Gli hash identificano artefatti, non dimostrano una capacita'.

## 3. L'intuizione centrale, con i suoi limiti

**Il problema non e' solo sapere piu' cose: e' collegare l'evidenza osservata,
la lettura scelta e l'azione autorizzata da quella lettura.** Oggi questi
passaggi esistono, ma sono discontinui. Una nuova conoscenza puo' migliorare
un lettore senza raggiungerne un altro; un esito `answered` puo' nascondere
una domanda saltata o un fatto inventato da un taglio di stringa.

La crescita rapida richiede un circuito: un'occorrenza sostiene un ruolo;
un consumatore usa quel ruolo; il residuo nomina esattamente il legame
mancante; una lezione modifica quel punto condiviso; si rilegge e si
revisionano soltanto gli usi che ne dipendono. Un registro di errori piu'
lungo non produce da solo questo circuito.

Distinguere sempre tre avanzamenti:

- **Visibilita':** il sistema espone quale clausola o ruolo e' rimasto aperto.
- **Uso:** la risposta o l'apprendimento cambiano grazie alla lettura.
- **Fertilita':** una nuova lezione cambia altri casi senza ricompilare, e
  ritirarla toglie precisamente quel cambiamento.

La patch aperta tenta soprattutto il primo e parte del secondo. Non ha
ancora dimostrato il terzo. Non chiamare «comprensione universale» uno
splitter piu' accurato o un nuovo fatto diagnostico.

## 4. Anatomia gia' esplorata: non ripetere l'indagine da zero

| punto reale | insight e conseguenza per chi continua |
|---|---|
| `src/code.c`, `input_structure`; `kb/core/input.p0` | il produttore usa aperture/chiusure/interruzioni e registra `reading_choice`; un `np_candidate` non e' una prova compositiva di testa, modificatori e attacchi. La copula e' gia' fra le evidenze di confine: non aggiungere di nuovo quel fix |
| `kb/core/input-structure.p0`, `bare_noun_candidate`, `bare_run` | il nome nudo nasce molto per esclusione. Parole sconosciute adiacenti possono diventare una falsa entita'. Questo spiega perche' imparare il ruolo di un verbo puo' riparare anche uno slot nominale |
| `word_is_verb_form` e `english-grammar/reading.p0:token_morphology` | la vista dei verbi di relazione, il POS e la morfologia generale non coincidono. Unire tutte le liste in un veto globale romperebbe gli usi nominali di parole anche verbali: serve una decisione sull'occorrenza |
| `construction_role_required/4`, `scope_requirement/4`, `commitment_at/4` | strutture gia' presenti. Nelle ricerche svolte non e' emerso un consumatore semantico generale; `scope_requirement` arriva al debug. Non trattare la presenza del predicato come prova che guidi gli apprendimenti |
| `input_binary_assertion` → `input_semantic_frame` → `input_assertion_store` | la prima relazione porta ID dei nodi; le proiezioni portano valori e scrivono `semantic_proposition`/`semantic_binding` e fonte. Manca una ricevuta uniforme della lettura selezionata usabile da tutti i consumatori |
| `kb/core/reading-choices.p0` | correzione locale, estensione a parola/classe e rilettura esistono. `reading_closer/1` e `reading_continuer/1` proiettano ancora sulla parola: per due usi diversi nello stesso testo serve la condizione locale |
| `language-lessons.p0`, `grammar.p0` | `lesson_form_target`, `lesson_condition_opens`, `inverts_when_via`, `condition_holds(all(...),...)` sono un circuito fertile da studiare. E' specializzato nell'inversione: non e' gia' un interprete di ogni lezione su ruolo/ambito |
| `document-claims.p0`, `contact.p0`, `derivation.p0` | letture correnti, dipendenze e sostegni sono materiale da riusare. `kb_derivation` spiega prove riuscite; non fabbrica automaticamente il residuo di una lettura semanticamente sbagliata |
| `gap-kinds.p0`, `arrests.p0`, `issues.p0` | molte lacune sono derivate da esito/topic/modulo. `wrong_suspect` copre una famiglia di template e propone una guardia. Il tabellone delle questioni esiste: non crearne un altro per i composti |

L'invariante del piano vale per i valori **estratti** dal testo: costituente
e ruolo sostenuto. Un valore calcolato o inferito deve invece avere una
derivazione collegata al ruolo richiesto; non inventargli un NP nella frase.

## 5. Che cosa hanno mostrato le conversazioni nuove

Leggere i log integrali prima di giudicare. Questi sono punti diagnostici,
non un punteggio selezionato sui soli turni convenienti.

| sessione/turno | osservazione | lacuna suggerita, da distinguere |
|---|---|---|
| standard 3 | «Well, I can explain it in a way that makes sense. ... What part of this topic fascinates you most?» → «Glad that lands. What would you like to do next?» | una cue sociale prende il posto della domanda effettiva; non e' un problema di conoscenza del tema |
| standard 5 | «... I'll share facts or stories. What sounds interesting?» → `Learned: i'll share facts` piu' muri | impegno/proposizione non allineati; leggere tutte le clausole non basta a evitare un falso apprendimento |
| standard 7–8 | richiesta di chiarimento → dialogo artificiale sui cavalieri; seguito → `Socrates is mortal` | un esempio o una meta-attivita' preconfezionata occupano il ruolo della risposta |
| standard 9 | entita' `core here let's break lets` | frammenti verbali/interrogativi entrano in una falsa entita'; qui resta necessario il lavoro sui ruoli/NP |
| standard 10 | domanda su curiosita' preceduta da «Let's keep it simple» → solo conferma di registro | una sottoparte servita viene trattata come turno risolto |
| trasferimento 3 | `Let me explain! Pronouns...` resta una sola clausola non letta; conferma di esempio senza chiara base | confini divergenti e stato di insegnamento/questione da ricostruire |
| trasferimento 7–9 | `Learned: i'd say youre`, `Learned: oh twist you're making me think. oh.`, `Learned: just share sentence` | modalita', imperativo e incisi non autorizzano automaticamente fatti |
| trasferimento 10 | «Let's clarify...» → racconto con protagonista `Let's` | un lettore decide su una superficie senza l'allineamento al ruolo |

La sezione F del piano proponeva di partire dal circuito costituente → slot
memoria. Durante la concretizzazione il primo bersaglio e' stato spostato
**a monte**, sul turno composto: una domanda puo' sparire prima che un
consumatore semantico abbia la possibilita' di rispondervi. Questo e' un
tentativo circoscritto, non una sostituzione dell'obiettivo sui costituenti.
Se non produce un beneficio reale, non insistere per difendere la patch.

Sul turno standard 3, una sessione diagnostica ha osservato:

- forze `directive`, `question`, `compound_inquiry` gia' pubblicate;
- `input_segment` pari a un solo span: **gli span non sono le frasi**;
- cessioni per `answer_frame` e `knowledge`, ma risposta sociale finale;
- nessun tentativo del lettore composto visibile nel trace esaminato.

In KB esistevano gia' cessioni `chitchat`/`smalltalk` su `compound_inquiry`.
**La causa completa del bypass non e' stata localizzata.** Non aggiungere
alla cieca un'altra riga uguale: controllare i percorsi anticipati, lo scope
di `p0_turn_is` e quando viene interrogata la forza. Il trace profondo ha
prodotto migliaia di righe, molte enumerazioni `world stays part_of`:
partire da trace superficiale e filtrare dispatch/cessioni/`read.compound`.

Tentativo naturale gia' fatto: `Read each sentence separately before answering
the question.` La risposta era `tools_disabled` per una richiesta `read`
interpretata come accesso al filesystem. **Non e' una lezione riuscita.**
Non e' stata salvata. E' evidenza di una porta di meta-comprensione mancante,
non licenza di presentare un `!assert` come insegnamento parlato.

## 6. Patch aperta: intenzione, meccanica, punti da contestare

In `turn-frames.p0`:

- `clause_boundary_cue` eredita `sentence_boundary_cue`, inclusa `! `.
- `clause_reading/2` associa forza e classe di confini;
  `turn_clause_boundary/2` e' interrogato dal C e dalla cessione di `turn_plan`.
- `class_surface(sentence_boundary_cue, sentence boundary)` espone il nome
  della classe, **ma la sua insegnabilita' naturale non e' stata provata**.
- `turn_clause_result/3` e `turn_clause_source/3` registrano risultato locale
  e range; `turn_clause_unread` alimenta `turn_gap_kind(..., partial)`;
  `debug_clause_result` espone la ricevuta.

In `99-registry.c`:

- il percorso composto anticipato accetta la decisione KB anche per le
  domande composte, non solo per le dichiarative;
- il taglio conserva il segno terminale delle frasi e gli offset nel testo
  originario; rimuove invece i connettivi usati da confine;
- ogni clausola viene ancora riletta con `brain_respond`, come prima;
- al ripristino del turno esterno si pubblicano `result(Module, Status)` e
  `range(Start, Len)`, associati ai nodi di clausola pubblicati in `last_text`;
- le ricevute di `current_turn` vengono pulite al turno successivo;
  clausole non processate sono inizialmente `unread`.

**Correzione effettuata durante il handoff:** la prima versione aveva
`faculty_yield_when(..., turn_requires_clause_reading)` con aiutante di
arita' 1. Il consumer in `src/brain/00-lex.c` chiama il predicato passato con
**due** argomenti `(current_turn, libero)`: la regola non avrebbe mai ceduto,
senza errore di parsing. Ora la cessione usa direttamente
`faculty_yield_when(turn_plan, open, turn_clause_boundary)`, arita' 2.
Non reintrodurre l'alias di arita' 1. Questo e' un esempio concreto della
necessita' di leggere il contratto del consumer, non solo la regola KB.

Rischi aperti da risolvere quando si riprende:

1. **`answered` non significa vero.** La ricevuta usa il criterio esistente
   `reply_is_wall`; un falso `Learned` puo' restare `answered`. Non dichiarare
   risolto il rilevamento dei misclaim.
2. **Spezzare puo' perdere legami.** Condizioni, citazioni, anafore e relazioni
   premessa/domanda devono conservare il contesto. Le esclusioni esistenti
   `turn_is_problem_text`/guided inquiry non sono una prova che ogni caso sia
   protetto. Una cessione troppo larga del piano puo' peggiorare i composti.
3. **Identita' della prova.** Ricevute nello scope del turno e nodi in
   `last_text`: verificare vita e identita' dopo archiviazione o testo nuovo.
   Non chiamarle gia' sostegno semantico persistente. I range includono
   talvolta whitespace che il nodo rifilato non include.
4. **Ripristino annidato.** Vengono ripristinate forze e `outer_turn_structure`;
   verificare che non restino frame/stati della sola ultima clausola. Il
   ramo `if (!read)` restituisce il turno intero a un fallback: potrebbe
   nascondere di nuovo il residuo o lasciare effetti collaterali.
5. **Residui troppo larghi.** Una clausola di soli spazi puo' essere marcata
   `unread`. La capienza delle risposte puo' interrompere prima della fine:
   la ricevuta iniziale lo espone, non risolve da sola la risposta.
6. **Confini ancora incompleti.** `? ` non e' oggi in `sentence_boundary_cue`.
   Non aggiungere liste in C. Valutare una lezione e il suo effetto su
   domande multiple, citazioni e abbreviazioni; non cambiare la classe solo
   per migliorare questo log.
7. **Commenti C da aggiornare.** Il commento sulla sorgente `input` parla
   ancora di un NUL sostituito senza spostare byte; ora il buffer e'
   ricostruito con offset espliciti. Il commento del dispatch anticipato
   parla ancora solo di dichiarative. Non usarli come descrizione esatta
   del nuovo codice.

Il C e' cresciuto in questo esperimento. Non e' una migrazione completata
dei lettori sulla IR, ne' la realizzazione del circuito di apprendimento
descritto nel piano. Valutare il guadagno prima di ampliare il patchset.

## 7. Ripresa pratica: fare una cosa utile per volta

**Primo passo:** leggere `git diff`, poi riprodurre il turno standard 3 con
il contesto dei turni 1–2. Il caso isolato e il caso contestualizzato possono
differire. Guardare se la domanda arriva a un lettore e che cosa diventa la
clausola `makes sense`. Se non cambia, localizzare il bypass prima di scrivere
altre regole. Se migliora, verificare subito uno dei falsi `Learned`: non
confondere la visibilita' delle clausole con la liceita' di cio' che imparano.

Compilazione esplicita:

```sh
make build -j2
```

**Non usare `make` nudo:** in questo checkout il target predefinito avvia
`cefr-bench`. E' successo durante questo lavoro; il bench e' stato interrotto
e i suoi risultati non sono evidenza di questa patch.

Controllo minimo del caricamento:

```sh
printf '/quit\n' | PARROT0_SESSION= PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0 > /tmp/parrot0-l4-boot.out 2> /tmp/parrot0-l4-boot.err
rg -n 'PARSE ERROR|bad rule' /tmp/parrot0-l4-boot.err
```

Al handoff: `make build -j2` completato con exit 0; boot completo con exit 0,
nessun `PARSE ERROR`/`bad rule` nei due flussi. Non sono prove comportamentali.
`make soft-test` **non ancora eseguito**: farlo una volta quando la modifica
al motore e' stabile, come richiede la challenge. Il caso esistente pertinente
e' [compound_inquiry.p0t](../../tests/p0t/conversation/compound_inquiry.p0t).
Leggerne le attese prima di usarlo: non certificare come comprensione una
vecchia attesa che premia un frammento mal letto. Niente campagne di suite
per procrastinare la conversazione reale.

Attenzione al dialetto: nel checkout attuale `src/kb.h` dichiara
**KB_MAX_ARGS=4 e KB_MAX_BODY=16**. Il limite 8 riportato in AGENTS e' storico.
L'arita' sbagliata puo' dare zero soluzioni; `naf` con variabile libera non
lega; `findall` ha vincoli su template e risultato; le variabili citate nel
testo di turno possono perdere maiuscole. Consultare
[parrot-p0-syntax.md](../parrot-p0-syntax.md) prima di attribuire un vuoto al
ragionamento del motore.

## 8. Dimostrazione prima/dopo: evitare confronti ingannevoli

Il server osservato e' LM Studio su `http://localhost:1234`, modello
`liquid/lfm2.5-1.2b`. La verifica `/v1/models` ha risposto e le due conversazioni
sono riuscite. Non serve riavviare il servizio solo per abitudine.

```sh
curl -sS --max-time 5 http://localhost:1234/v1/models
.venv/bin/python scripts/live-talk.py --turns 12 --temperature 0 --log docs/sessions/talk/2026-09-30-l4-growth-after.log
.venv/bin/python scripts/live-talk.py --turns 12 --temperature 0 --opener "Hey there! I just got back from a walk in the park. Do you like being outside?" --log docs/sessions/talk/2026-09-30-l4-growth-transfer-after.log
```

Questi sono comandi **da eseguire**, non misure gia' ottenute. Se la data o
la revisione cambia, scegliere nomi nuovi: lo script **accoda**. Mantiene il
profilo agi completo e `PARROT0_SESSION` vuoto; vuoto non disabilita la KB.
L'accesso Python a localhost era bloccato dalla sandbox: e' stato autorizzato
il prefisso `.venv/bin/python scripts/live-talk.py`. Se l'ambiente richiede
nuovamente escalation, usare il meccanismo previsto, senza aggirarlo.

**Serve anche un replay congelato degli ingressi.** Una nuova conversazione
libera non e' lo stesso esperimento: appena cambia una risposta, LFM cambia
il seguito. Il seed 7 e temperatura 0 non garantiscono un ramo identico;
la documentazione storica segnala che questo modello puo' ignorarli.

Procedura di replay ancora da realizzare, usando gli strumenti gia' presenti:

1. Estrarre in ordine i testi dopo `M> ` da ciascuna baseline; devono essere
   12. Non estrarre anche le risposte e non riscrivere i prompt.
2. Avviare un processo nuovo per ogni conversazione, inviare i 12 ingressi
   preservando il contesto, salvare ogni risposta e il tempo effettivo.
3. Fare lo stesso sul prima completo e sul dopo completo. Niente `/save`,
   niente lezioni supplementari presenti soltanto in uno dei due rami.
4. Il helper `Parrot` in `scripts/coherence-bench.py` puo' evitare di
   reinventare la gestione del prompt. Importarlo con `importlib`; **impostare
   sia `module.REPO` sia `Parrot(root, turn_timeout)` alla radice scelta**:
   il costruttore prende il binario da `REPO`, non da `root`. Ometterlo
   confronterebbe nuova meccanica e vecchia KB, non il vero prima.
5. Impostare esplicitamente `PARROT0_SESSION=""` nell'ambiente del helper e
   `PARROT0_LANG=en`; il helper non azzera la sessione da solo. Registrare
   timeout/uscite e chiudere il processo anche in caso di errore.
6. Poi eseguire le due conversazioni libere locali per vedere il trasferimento.

Giudicare tutti i turni con la rubrica della challenge: A risposte adeguate
(muro onesto che specifica il limite = 0,5), M misclaim, S soste, R ripetizioni;
`(A − M) / turni`, piu' mediana dei tempi di parrot0 separata da quella LFM.
Una clausola corretta non riscatta automaticamente un turno che inventa un
fatto o perde la domanda. Non premiare un aumento di testo come comprensione.
Documentare il criterio per le risposte miste e applicarlo uguale prima/dopo.

Il modello piccolo produce stimoli, **non e' una fonte di verita'** e le sue
lodi non sono valutazioni. Un falso `Learned` e' un misclaim anche se LFM
prosegue compiaciuto. Ridurre un misclaim a un muro pertinente e' un progresso
misurabile, ma non prova di avere acquisito la conoscenza che manca.

## 9. Dalla prima riparazione alla crescita della KB

Una volta stabilito il risultato del composto, tornare al nodo piu' fertile
del piano: **un consumatore reale deve smettere di estrarre un valore dalla
coda della stringa e usare il ruolo sostenuto di un costituente**. Memoria
rimane un buon primo circuito perche' consente domanda successiva, revisione
e ritiro della lezione. Non migrare contemporaneamente decine di lettori.

Per ogni passo successivo rispondere concretamente a queste domande:

- Quale evidenza manca: confine, testa, ruolo, ambito, referente, impegno o
  uso della lettura? Il residuo deve puntare all'occorrenza, non solo al topic.
- La KB sa gia' descrivere quell'evidenza ma il consumer la ignora? Collegare
  producer e consumer prima di duplicare la conoscenza.
- Quale lezione minima puo' cambiare la decisione per una classe di casi?
  Cercare condizioni locali: la stessa parola puo' richiedere letture diverse.
- Come ritirare la lezione e rivedere i fatti che dipendevano da essa? La
  revisione riguarda anche conclusioni prima assenti, non solo prove riuscite.
- Come chiude il residuo la questione gia' aperta in `issues.p0`, evitando
  chiarimenti ripetuti sul medesimo referente?

Prima provare lingua naturale per contatto L3, poi schema L2 dichiarato;
per lezioni sulla lingua/comprensione usare i circuiti L4 realmente
disponibili. Se la lezione non entra, descrivere precisamente la porta
mancante e costruirla KB-first. **Nomi di predicati nel prompt, tuple e
`!assert` non sono insegnamento naturale.** Un fatto iniziale scritto nel
file KB non prova che l'utente sappia insegnarne domani un membro.

La prova di fertilita' deve essere piccola ma reale: forma nuova insegnata,
uso su un ingresso diverso, ritiro/ablazione mirata e perdita di quel solo
effetto sulla KB viva completa. Per connecting dots usare conoscenza reale
preesistente: un micromondo costruito apposta dimostra al massimo meccanica.

## 10. Invito operativo da passare al prossimo agente

> Continua dal working tree, non riscrivere il piano da zero. Leggi questo
> handoff e il diff. L'obiettivo ancora aperto e' dimostrare un miglioramento
> reale nella challenge con LFM locale, costruendo strutture che rendano
> fertile la KB. Le baseline esistono, la patch e' sperimentale e compilata,
> il dopo non esiste. Prima controlla il percorso del turno standard 3 e i
> rischi di scope/condizioni; poi decidi se questa patch merita di proseguire.
> Non fermarti al debug o al piano: produci il confronto con ingressi
> congelati e le conversazioni libere, giudicale interamente e dichiara
> l'esito anche se negativo. Non inserire parole naturali nel C, non amputare
> la KB, non fingere una lezione parlata tramite assert. Conserva le
> strutture secondarie funzionanti. Se il composto migliora, collega almeno
> un uso concreto della lettura alla crescita insegnabile, senza spacciare
> la segmentazione per comprensione semantica. Aggiorna questo handoff e i
> due piani con l'evidenza nuova, i limiti rimasti e il prossimo residuo utile.

Il lavoro e' concluso solo quando si puo' indicare **quale comportamento e'
migliorato, per quale struttura condivisa, con quali lezioni future
utilizzabili e con quali limiti misurati**. Fino ad allora conservare la
distinzione fra intuizione, implementazione ed evidenza.
