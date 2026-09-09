# parrot0: valutazione critica dell'architettura rispetto alla missione

9 settembre 2026. Lettura statica del progetto al commit `4360751`.
Oggetto: adeguatezza dell'implementazione alla missione di raggiungere capacità
di ragionamento e proprietà di linguaggio comparabili a un LLM. Il giudizio
riguarda rappresentazioni, procedure, controllo e possibilità di crescita;
non attribuisce punteggi di prestazione. Le osservazioni sul codice sono
distinte dalle conseguenze architetturali che ne ricavo.

Integrazione su richiesta di F.: le sezioni 10–14 affrontano la lentezza della
crescita utile, la metafora della «tela nera» e l'ipotesi che addestramento
continuo e colla linguistica possano portare alla comprensione universale.
Le sezioni 15–19 approfondiscono la distinzione fra trasferire conoscenza e
apprendere comportamento, l'argomento della macchina a stati, la funzione
dell'attenzione e la possibilità di acquisire le capacità del maestro.
La proposta di processo è un disegno architetturale: non è stata avviata una
campagna di addestramento né modificata la KB.

Da questa analisi deriva il
[quadro preliminare «Verso una KB viva»](../plans/quadro-preliminare-kb-viva.md):
un documento pre-operativo che raccoglie ipotesi e decisioni da chiarire,
senza trasformarle ancora in una roadmap di implementazione.

**Giudizio centrale.** parrot0 ha una base architetturale seria per un agente
simbolico che apprende: la conoscenza può essere eseguibile, il sistema può
interrogare parte del proprio funzionamento, e letture, contesti e procedure
possono diventare oggetti modificabili. Tuttavia, l'implementazione non forma
ancora un'organizzazione cognitiva unitaria. Coesistono un nucleo dichiarativo
generale e percorsi specializzati che conservano proprie decisioni di lettura,
selezione e risposta. La distanza dalla missione si concentra in questa
frattura: il progetto rende modificabili molti contenuti, ma non rende ancora
altrettanto generale il processo che comprende, combina e usa quei contenuti.

La direzione più promettente è già nel repository: una rappresentazione
condivisa, procedure come conoscenza, letture rivedibili, questioni persistenti
e generazione derivata dal contenuto. Perché questa direzione governi davvero
l'agente, i percorsi specializzati devono contribuire a quegli oggetti comuni.
La sola crescita della KB non produce automaticamente questo raccordo.
La crescita cumulativa richiede anche che il sistema apprenda come costruire
e riutilizzare quei raccordi. La coerenza dialogica può guidare questo
apprendimento, ma deve essere vincolata al contenuto e a fonti di correzione
che non dipendano soltanto dalla risposta che il sistema vuole sostenere.
Questo requisito non impone a parrot0 un'infallibilità che non chiediamo
agli LLM: la qualità delle premesse è distinta dalla capacità di ragionarci.
Il punto più forte dell'ipotesi di F. è che la conoscenza appresa possa
modificare anche il modo di interpretare e di imparare. Il codice ne offre
già forme locali; resta da generalizzarne il ciclo, non da attendere che
una quantità indeterminata di fatti trasformi spontaneamente l'architettura.

**1. La missione è plausibile come ricerca; il meccanismo di convergenza resta aperto.**

[PRINCIPLES.md](../../PRINCIPLES.md) propone di ricostruire attraverso il
comportamento una struttura funzionale di ragionamento, rendendola esplicita
in C. Il documento distingue correttamente equivalenza funzionale e identità
del meccanismo. Quella distinzione deve governare anche il progetto: il codice
prodotto da un LLM è una soluzione ingegneristica proposta dal modello; non
è, per questo fatto, un'estrazione dell'algoritmo che il modello usa internamente.

Non vedo nel determinismo o nel C un impedimento di principio alla missione.
Il linguaggio d'implementazione non stabilisce la generalità cognitiva. La
questione è quali rappresentazioni il sistema possa costruire e quali
trasformazioni possa apprendere su di esse. Un motore fisso può eseguire
programmi nuovi; può però anche restare un insieme di riconoscitori molto
estendibili, senza acquisire un metodo generale per costruire programmi nuovi.

La tensione interna più importante è fra conservare le strutture secondarie,
come chiede PRINCIPLES, e unificare la decisione, come chiedono i mantra più
recenti. La ridondanza è utile quando mantiene disponibili interpretazioni e
strategie alternative. Diventa un ostacolo quando ogni alternativa conserva
il potere di decidere autonomamente che cosa significhi il turno e quando
considerarlo risolto. Il progetto dovrebbe conservare le competenze e
ricondurre la loro selezione a un controllo comune.

**2. Gli strati esistono, ma i loro confini non sono ancora un contratto unico.**

| Strato | Implementazione presente | Questione architetturale |
|---|---|---|
| Substrato logico | Fatti, regole, unificazione, risoluzione, termini composti, riflessione | Può eseguire una rappresentazione formale; non decide da solo quale rappresentazione renda il significato del testo. |
| Comprensione | Segmenti, token, gerarchie, cue, frame, denotazioni, legame degli slot | La lettura comune convive con riletture locali e scelte anticipate della prima analisi compatibile. |
| Conoscenza e memoria | Fatti di dominio, contesti, claim attribuite, versioni delle letture, turni archiviati | L'identità semantica e le dipendenze devono attraversare tutte queste viste. |
| Controllo dialogico | Questioni aperte, politiche delle mosse, priorità, diritti dei moduli | Il controllo dichiarativo convive con precedenze ed esiti decisi nella catena C. |
| Ragionamento procedurale | Procedure ricorsive, situazioni, precondizioni, effetti, schemi di thinking | L'esecuzione di schemi esistenti è più sviluppata della loro costruzione a partire da uno scopo nuovo. |
| Linguaggio in uscita | Template, forme lessicali, composizione inferenziale, continuazioni locali | Non c'è ancora un percorso generale obbligato dal significato da esprimere alla frase realizzata. |
| Apprendimento | Lezioni di fatti e regole, procedure, candidati induttivi, revisione documentale | Ogni canale cresce entro ciò che il suo interprete sa già riconoscere e trasformare. |

Questa distinzione conta: usare la stessa KB permette l'interoperabilità, ma
non la realizza da sola. Due sottosistemi possono interrogare lo stesso
database e attribuire identità, scopi o condizioni di validità diversi allo
stesso contenuto.

**3. Il motore inferenziale è un fondamento valido, non una teoria completa del ragionamento.**

In [src/kb.c](../../src/kb.c) il nucleo è realmente generale: unificazione
strutturale, risoluzione con backtracking, regole ricorsive, primitive
aritmetiche, `call`/`apply` e riflessione su fatti e regole. In
[procedures.p0](../../kb/core/procedures.p0) somma di liste, filtri e relazioni
computazionali sono procedure sopra quei primitivi. Una procedura può quindi
diventare conoscenza eseguibile senza introdurre un operatore C per ogni
dominio. Questa è una delle scelte più coerenti con la missione.

Va però distinta l'inferenza su simboli dalla costruzione del problema da
risolvere. Il solver può concatenare regole su una relazione già formulata;
non ottiene dall'unificazione il significato di una frase ambigua, la scelta
delle premesse pertinenti o l'invenzione di una buona astrazione. Questi
compiti devono essere svolti dagli strati superiori. Chiamare il solver
«ragionamento» è corretto, purché non gli si attribuisca implicitamente tutto
il lavoro cognitivo che rende possibile una deduzione utile.

La separazione fra prova, fallimento finito e ricerca incompleta è una scelta
forte: `goal_provable` distingue l'interruzione, e la negazione per fallimento
non dovrebbe trasformarla in assenza. Anche la distinzione fra sostegno
positivo, negativo, conflitto e ignoto è concettualmente necessaria.

Esiste però una scorciatoia semantica importante in
[epistemic-status.p0](../../kb/core/epistemic-status.p0): la presenza di una
regola che definisce una classe, in assenza di suoi membri osservati, può
autorizzarne la chiusura del mondo. Avere una definizione non garantisce che
si conoscano completamente le sue premesse. Per esempio, da
`abilitato(X) :- formato(X)` e dall'assenza d'informazioni sulla formazione
di una persona non segue che quella persona non sia abilitata. È un
controesempio logico alla generalità della politica, non una prova eseguita.
Il consumer `closed_world_answer` è effettivamente consultato dalle risposte
polari in [10-memory-knowledge.c](../../src/brain/10-memory-knowledge.c).

Il problema è istruttivo: anche una politica interamente in KB può essere
epistemicamente troppo forte. KB-first garantisce una possibilità di
ispezione e modifica; la correttezza della semantica resta un obbligo distinto.

**4. La comprensione comune è il centro giusto, ma non governa ancora tutte le letture.**

[turn-frames.p0](../../kb/core/turn-frames.p0),
[input-structure.p0](../../kb/core/input-structure.p0) e
[dialogue-frames.p0](../../kb/core/dialogue-frames.p0) costruiscono un ponte
fra superficie e oggetti interrogabili: segmenti, ruoli, forza del turno,
relazione richiesta, entità e residui. Le viste di
[language-forms.p0](../../kb/core/language-forms.p0) e
[denotation.p0](../../kb/core/denotation.p0) riconoscono inoltre che una forma
linguistica denota qualcosa in una lingua, in un registro e in un dominio.
È un fondamento migliore della corrispondenza diretta parola-risposta.

La limitazione emerge seguendo i consumatori. `p0_frame_reading`, in
[10-memory-knowledge.c](../../src/brain/10-memory-knowledge.c), percorre gli
schemi e si ferma alla prima lettura dichiarativa accettata. Altri percorsi
ricevono ancora `canon` e `raw`, e interpretano nuovamente il turno. Il
proiettore dialogico di base considera completo un frame che possiede
relazione ed entità: è una porta utile per domande relazionali, ma la sua
completezza locale non equivale alla completezza semantica di una richiesta
con condizioni, quantificatori e più obblighi.

Il caso strutturalmente più chiaro è `compound_turn_lead` in
[99-registry.c](../../src/brain/99-registry.c). Il procedimento separa il
testo sulle forme dichiarate in `clause_boundary_cue`, richiama
`brain_respond` sulle clausole e concatena le risposte. Il riuso del lettore
è positivo. Tuttavia, nel procedimento il confine trovato serve al taglio;
la relazione fra le clausole non diventa l'oggetto che governa la
composizione. «Ma», «quindi» e «mentre» possono separare contenuti, ma
esprimono relazioni differenti fra quei contenuti.

Questa decomposizione offre accesso ai pezzi; non costituisce ancora una
semantica generale del tutto. Occorre conservare il legame fra premesse,
conclusioni richieste, eccezioni, tempi e ambiti di validità, facendo lavorare
le facoltà su quel legame. Lo stesso vale per l'italiano: una
canonicalizzazione verso forme condivise aiuta, ma non sostituisce una
composizione grammaticale capace di conservare ciò che l'ordine, la
morfologia e il contesto esprimono.

**5. L'arbitrato resta in parte una competizione fra risponditori.**

In [dialogue-policy.p0](../../kb/core/dialogue-policy.p0) lo stato epistemico
propone mosse e le priorità possono sconfiggerne altre. In
[issues.p0](../../kb/core/issues.p0) una domanda irrisolta diventa una
questione con tema, origine e obbligo. Sono due passaggi verso una gestione
del dialogo fondata su ciò che serve all'interlocutore.

Nel percorso generale di [99-registry.c](../../src/brain/99-registry.c),
tuttavia, il registro continua a offrire il turno ai moduli fino a quando
uno lo rivendica. Esistono cessioni dichiarative, una distinzione fra moduli
con titolo e ripieghi, controlli di formato e trattamenti successivi della
resa. Queste aggiunte governano meglio la catena, ma il contratto del modulo
rimane sostanzialmente «produco una risposta oppure declino».

Per la missione, il contratto più fertile sarebbe «offro una lettura, una
prova, un calcolo o un passo che soddisfa questa parte del bisogno». Il
decisore potrebbe così comporre contributi e confrontarli rispetto alla
richiesta intera. Nella catena attuale il primo successo locale può chiudere
la ricerca prima che altre competenze abbiano contribuito. Conservare molti
moduli non garantisce quindi che le loro capacità si sommino.

Anche l'identità delle questioni ha una conseguenza semantica. `issue_id`
deriva da genere e tema; la relazione richiesta è un attributo separato.
Due domande dello stesso genere sullo stesso tema possono avere scopi
diversi: «dove si trova?» e «quando è nato?» non sono la stessa questione.
La rappresentazione deve distinguere quegli obblighi prima che un controllo
generale possa sospenderli, riprenderli e soddisfarli separatamente.

**6. Le procedure sono insegnabili; la costruzione autonoma di procedure resta il salto.**

[assisted-learning.p0](../../kb/core/assisted-learning.p0) contiene una
catena ricorsiva di passi: una lezione può legare operazioni e operandi,
conservarli e applicarli a ingressi successivi. Il legatore delle regole
naturali riusa inoltre gli schemi relazionali della lettura comune. Questi
sono raccordi effettivi: una nuova conoscenza può essere usata come parte
di un'altra procedura.

La generalità va però attribuita al livello corretto. La catena di passi
dell'apprendimento assistito è aperta nella lunghezza e nei dati; la forma
della lezione esaminata è organizzata intorno a conversioni e operatori già
interpretabili. Il motore `kb_induce` cerca invece inclusioni fra estensioni
di predicati unari e deposita candidati, evitando di dichiararli
automaticamente veri. È una forma precisa di induzione, non un apprendimento
generale di strutture relazionali, programmi e strategie.

Esiste poi una distanza fra descrivere un metodo e possederlo come azione.
[document-method.p0](../../kb/core/document-method.p0) ricava passi,
precondizioni e arresti da claim documentali, e ne deriva uno stato `ready`.
Nei consumatori esaminati questa rappresentazione rende il metodo
interrogabile; non è un compilatore generale dal metodo descritto a un
programma eseguibile con parametri, effetti e responsabilità. La presenza
del predicato `method_executable` non basta a colmare quel confine.

[thinking.p0](../../kb/core/thinking.p0) e `brain_think` introducono schemi,
dipendenze, precondizioni, effetti e rientri attraverso meta-prompt. Il riuso
delle capacità tramite la stessa porta linguistica è interessante. Ha però
un costo architetturale: risultati già ottenuti vengono tradotti nuovamente
in testo e sottoposti a riconoscimento. La prosa può essere una superficie
di insegnamento e spiegazione; come interfaccia interna esclusiva può perdere
identità, dipendenze e precisione che un risultato strutturato conserverebbe.

Inoltre la promozione dell'output del thinking usa classificazione della
risposta, eventuali incrementi di fatti e una politica il cui esito di
default è `answer`. Questo riconosce alcune forme di arresto, ma non dimostra
che lo scopo sia più vicino o che una conclusione sia meglio sostenuta.
L'effetto raggiunto dovrebbe essere stabilito dal contenuto e dallo stato
del compito. Rielaborare una risposta è una meccanica di controllo;
costruire e correggere un ragionamento richiede anche un criterio semantico
di avanzamento.

**7. La memoria documentale è una delle parti più mature concettualmente.**

La separazione fra osservazione, interpretazione e impegno epistemico in
[document-structure.p0](../../kb/core/document-structure.p0) e
[document-claims.p0](../../kb/core/document-claims.p0) è particolarmente
coerente con la missione. Il testo osservato conserva fonte e posizione;
la lettura è versionata; una proposizione riportata può essere normalizzata
senza diventare una credenza sul mondo. Le dipendenze permettono di
identificare letture da rivedere quando cambia la conoscenza.

Questo supera un limite tipico del semplice accumulo di fatti: imparare
può cambiare il significato attribuito a ciò che era già stato letto. Anche
la ritenzione dei turni, in [discourse.p0](../../kb/core/discourse.p0), è
espressa attraverso ragioni quali una questione aperta o un riferimento,
con la recenza come una politica fra altre.

Il raccordo ancora necessario è rendere queste dipendenze obbligatorie per
le conclusioni, le sintesi e i piani che consumano il contenuto. Le viste
contestuali esistono, mentre varie domande continuano a interrogare
direttamente predicati di dominio. Non si può presumere che ogni consumer
erediti automaticamente attribuzione, supersessione e revisione. Una
memoria profonda, in questa architettura, richiede che ciò che viene
ricordato resti utilizzabile con le sue condizioni di validità.

**8. Le proprietà di linguaggio richiedono una generazione guidata dal significato.**

La generazione ha più meccanismi: template e slot, lessicalizzazione di
concetti, composizione di passaggi e continuazioni apprese. Il percorso
`next_word_ctx` in
[25-wordmath-reasoning.c](../../src/brain/25-wordmath-reasoning.c) combina
transizioni di bigrammi e trigrammi. È un modello locale di continuazione;
la sua procedura non porta un piano semantico completo del discorso da
realizzare. Aumentarne il corpus non aggiunge da solo quel piano.

Più vicina alla missione è [composition.p0](../../kb/core/composition.p0):
uno stadio entra nel testo se la sua tesi regge, e la relazione fra stadi
determina il connettivo. Ci sono già consumer effettivi, per esempio nella
spiegazione negativa e nella domanda su un candidato induttivo; il commento
iniziale che descrive il file come interamente inerte è superato dal codice.
Questo è un nucleo da valorizzare, perché lega la forma della risposta alla
struttura del ragionamento.

Resta da trasformarlo in una disciplina generale di produzione. Spostare
una frase da C a `response_template` la rende modificabile, ma non rende
automaticamente compositiva la competenza linguistica. Per padronanza del
linguaggio intendo poter decidere che cosa affermare, quale dettaglio
omettere, come collegare due proposizioni, quale referente riprendere e come
riformulare conservando contenuto, tono e vincoli. Sono decisioni sul
significato che devono precedere e guidare la realizzazione della frase.

**9. Il valore di KB-first dipende da che cosa rende apprendibile.**

I mantra hanno correttamente allargato KB-first dal vocabolario alle frasi,
alle condotte e alle congiunzioni che definiscono una forma. Questo
allargamento rivela il punto essenziale: il luogo in cui vive una regola
non esaurisce la generalità del suo apprendimento.

Distinguerei tre proprietà. Una conoscenza può essere modificabile come
dato; può essere acquisibile attraverso una spiegazione naturale; può
infine trasferirsi a contesti e compiti diversi. Sono tre gradini, non tre
nomi della stessa cosa. L'implementazione contiene casi del secondo e del
terzo, ma non fornisce ancora una chiusura generale fra questi livelli.

Il rischio progettuale è costruire un interprete sempre più potente il cui
programma continua a essere scritto principalmente dall'agente sviluppatore.
L'interprete cresce, ma il soggetto che compie l'astrazione rimane esterno.
Questo è compatibile con una fase dell'esperimento; non può essere il
meccanismo definitivo della sua convergenza. La domanda decisiva diventa:
quali trasformazioni oggi effettuate da chi sviluppa possono diventare
procedure che parrot0 comprende, applica e corregge?

**10. La «tela nera»: perché una crescita reale può restare poco utilizzabile.**

F. descrive un'esperienza precisa: ogni espansione appare interessante vista
da vicino, ma lascia un insieme sterminato di varianti fuori; si formano
sistemi autonomi, senza che l'insieme acquisisca una completezza operativa.
La sua ambizione è costruire un processo continuo che renda viva la KB e
permetta alla continuità del dialogo di produrre ragionamento utile.

Trovo nel repository segnali convergenti di questa difficoltà. Li considero
storia delle cause e delle scelte progettuali, senza assumere che ogni
difetto storico sia ancora presente:

- [Perché non cresceva](../labs/apprendimento-assistito/2026-08-31-perche-non-cresceva.md)
  descrive un fatto acquisito sotto un'identità che il percorso interrogativo
  non ricostruiva. Aggiungere forme di domanda non poteva rendere raggiungibile
  quel fatto. La crescita aveva ampliato un anello di una catena interrotta.
- [Il confine dell'addestrabilità](../labs/apprendimento-assistito/2026-08-28-confine-addestrabilita.md)
  osserva che lezioni su domini differenti attraversavano lo stesso ponte
  metalinguistico già noto. Cambiavano i contenuti, mentre il repertorio di
  trasformazioni che il sistema sapeva apprendere restava quasi lo stesso.
- [Procedura di crescita KB](../plans/procedura-crescita-kb.md) registra
  correzioni il cui difetto ricompariva in un'altra facoltà. L'interpretazione
  che ne ricavo è che una distinzione necessaria a molti consumatori veniva
  introdotta in uno solo. Il costo maggiore era ricostruire ripetutamente la
  stessa causa, non eseguire l'operazione materiale.
- [Dialogica](../plans/dialogica.md) riconosce che aggiungere memorie separate
  per offerte e disambiguazioni produceva nuovi stati da coordinare. Una
  soluzione locale aumentava anche gli obblighi d'integrazione successivi.
- Il mantra «massimizzare e declinare» e il lavoro sulla grammatica e sul
  linguaggio `.p0` in [apprendimento assistito](../plans/apprendimento-assistito.md)
  sono tentativi espliciti di cambiare questo andamento: sfruttare una
  distinzione oltre il primo caso e insegnare strumenti che migliorino lo
  stesso atto di imparare.

Questi segnali sostengono una diagnosi più specifica di «manca tanta
conoscenza». Una risposta utile dipende da una catena: leggere la forma,
identificare i referenti, legare i ruoli, mantenere lo scope, raggiungere la
conoscenza, inferire e realizzare. Migliorare un tratto può non rendere ancora
percorribile la catena. Inoltre, se una stessa distinzione viene rappresentata
diversamente nei vari tratti, ogni estensione deve pagare di nuovo il costo
dell'allineamento.

La metafora della pennellata descrive quindi due fenomeni distinti: copertura
ristretta delle forme e mancata circolazione delle capacità. Il primo chiede
astrazioni compositive; il secondo chiede identità e contratti condivisi.
Moltiplicare le lezioni senza distinguere questi problemi può ampliare il
deposito lasciando invariata la sua utilizzabilità.

C'è anche una rettifica alla prima parte di questo report: unificare gli
oggetti interni è necessario per il raccordo descritto, ma non basta a
rendere produttiva la crescita. Serve un processo che scelga che cosa
apprendere, riconosca le strutture riusabili e le trasformi in strumenti
per l'apprendimento successivo. Altrimenti si ottiene una tela meglio
organizzata, ancora dipinta principalmente a mano.

**11. La completezza utile riguarda i percorsi, non l'esaurimento del linguaggio.**

Non condivido l'idea che ogni espansione debba esaurire tutte le proprie
varianti prima di essere utile. Uno spazio di espressioni potenzialmente
infinito può essere governato da regole finite e ricorsive. Una procedura
per combinare proposizioni può trattare combinazioni mai enumerate; questo
non implica che una grammatica finita già disponibile descriva tutta la
lingua naturale, né che ogni frase sia decidibile entro risorse finite.

Il problema è che una classe nominalmente generale può nascondere ancora
una forma particolare. «Nuovo verbo» è un'estensione lessicale; il suo uso
sotto negazione, in una relativa, in una domanda o in un contesto ipotetico
richiede che quelle operazioni si compongano con la relazione appresa.
Se occorre insegnare separatamente ogni combinazione, la generalizzazione
ha spostato il confine senza eliminare il prodotto combinatorio.

Distinguerei quattro significati di completezza:

| Significato | Che cosa chiedere al progetto |
|---|---|
| Conoscere tutti i fatti e tutte le forme | Non è una condizione realistica di avvio o di conclusione. La conoscenza del mondo resta aperta. |
| Conservare tutto il contenuto pertinente di una lettura | È un obbligo locale: segnalare ciò che resta irrisolto invece di eliminarlo dal significato. |
| Chiudere il percorso di una capacità | Una conoscenza acquisita deve poter essere interrogata, usata, spiegata e corretta attraverso i consumer pertinenti, entro condizioni dichiarate. |
| Poter estendere il proprio repertorio | Un limite deve diventare un bisogno di apprendimento rappresentabile, oppure un limite del motore esplicitamente riconosciuto. |

Per «comprensione universale» adotterei quindi un'architettura comune e
estensibile per rappresentare contenuti e gestire residui, non la promessa
di capire subito qualunque input. La seconda interpretazione non fornisce
una condizione di completamento operativa. La prima permette di crescere
senza scambiare una lacuna locale per il fallimento dell'intero progetto.

L'unità di crescita dovrebbe essere una distinzione utilizzabile attraverso
una catena completa. «A precede B» può inizialmente essere un fatto; il
guadagno compositivo consiste nel poter usare la relazione con referenti
nuovi, interrogarne i ruoli, combinarla con altre precedenze e rivederne le
conseguenze dopo una correzione. Qui gli esempi illustrano la struttura
proposta, non dichiarano nuove capacità già implementate.

Neppure la varietà d'uso è una tela uniforme: conversazioni differenti
riutilizzano riferimenti, condizioni, esclusioni, quantità, dipendenze e
correzioni. È ragionevole concentrare l'apprendimento su queste strutture
ricorrenti. Rimane un'ipotesi progettuale che il repertorio acquisibile da
parrot0 possa diventare abbastanza ampio: la ricorsione consente il riuso,
ma non garantisce di avere scelto le astrazioni giuste.

**12. La colla linguistica può rendere produttivo il ragionamento; la sola coerenza non lo garantisce.**

La teoria in [the-linguistic-glue.md](../plans/the-linguistic-glue.md)
attribuisce alla colla la continuità fra memoria, inferenza, intenzioni e
correzioni. Il testo stesso distingue questa funzione dalla produzione
diretta di inferenze. La nuova formulazione di F. ne propone una conseguenza
più forte: una volta ottenuta la continuità, il ragionamento utile potrebbe
derivarne attraverso il dialogo.

Una parte di questa intuizione è convincente. Se una relazione introdotta
prima viene ripresa da un pronome, una precisazione ne restringe lo scope
e una domanda ne richiede una conseguenza, mantenere la continuità rende
possibile un'inferenza che i turni isolati non consentivano. La colla non
è soltanto eleganza della conversazione: seleziona e conserva gli oggetti
su cui ragionare. Può anche far emergere la domanda discriminante che
permette di acquisire una distinzione nuova.

Tuttavia, «coerenza» può indicare tre cose diverse. La continuità della
superficie mantiene tema e tono; la consistenza logica evita contraddizioni;
la continuità semantica mantiene referenti, contenuto, scopi e sostegni.
Le prime due possono appartenere a una spiegazione falsa. Due interlocutori
possono accordarsi su una premessa errata e derivarne conseguenze senza
contraddirsi. **Precisazione richiesta da F.: questo non confuta la loro
capacità inferenziale.** Date quelle premesse, la derivazione può essere
corretta, come in un mondo ipotetico o in un ragionamento condizionale.
La qualità della KB è un problema distinto, comune anche al maestro LLM.
L'obiezione rilevante alla sufficienza della coerenza è un'altra: non
contraddirsi non determina ancora quali relazioni costruire e quali
trasformazioni eseguire per rispondere. Anche una domanda reale può ammettere
più risposte coerenti con ciò che è stato detto: occorre allora acquisire
informazione ulteriore.

Se per colla si intende la terza forma, l'ipotesi diventa più forte, ma
incorpora già operazioni cognitive sostanziali: interpretare, risolvere
riferimenti, riconoscere equivalenze, distinguere ipotesi da fatti,
propagare correzioni. Spiegare quelle operazioni attraverso la sola parola
«coerenza» rinvierebbe il problema che vogliamo risolvere.

La versione dell'ipotesi che ritengo sostenibile è questa: **la continuità
semantica del dialogo può organizzare un apprendimento cumulativo e rendere
riusabili le inferenze, se le nuove conclusioni restano vincolate a
evidenze, condizioni di validità e correzioni effettive.** Non ne segue che
ogni conversazione coerente generi progresso. Ripetizione, conferme e
parafrasi possono lasciare identica la conoscenza disponibile.

Per ancoraggio esterno non intendo necessariamente un corpo o sensori.
Possono bastare, secondo il compito, una fonte attribuita, un'osservazione
dell'utente, un calcolo controllabile o l'esito di un'azione. Questi
sostegni possono essere fallibili, e vanno rappresentati come tali. Il punto
è che la validità di una conclusione non dipenda esclusivamente dal fatto
che parrot0 l'abbia già formulata o che un teacher l'abbia approvata.

**13. Un processo continuo è costruibile, ma deve apprendere anche gli strumenti con cui cresce.**

Distinguerei addestramento continuamente disponibile, addestramento
continuamente orchestrato da un teacher e apprendimento autonomo dei propri
metodi. Il primo può accogliere lezioni ogni volta che servono. Il secondo
mantiene un'agenda e svolge cicli anche senza una nuova richiesta di F.
Il terzo sceglie e migliora le procedure con cui interpreta le lezioni e
costruisce conoscenza. Si può realizzare il secondo prima del terzo, ma
non andrebbero chiamati la stessa capacità.

Il progetto possiede componenti da raccordare: questioni aperte, residui di
lettura, candidati di apprendimento, fonti, revisioni e procedure KB.
[LEARN_PROTOCOL.md](../../LEARN_PROTOCOL.md) contiene già gran parte della
disciplina didattica, ma il suo esecutore è un coding agent esterno.
In [autocorrezione](../autocorrezione.md) il passaggio dall'indovinare al
chiedere riconosce esplicitamente che un interlocutore può fornire la
distinzione mancante. Questa dipendenza è compatibile con l'apprendimento,
finché parrot0 interpreta la lezione e ne incorpora la regola; diventa
delega del ragionamento se il teacher continua a risolvere ogni nuovo caso.

Il ciclo automatico attuale non realizza ancora la proposta completa.
In [dream.c](../../src/dream.c), `agenda_from_gaps` ricava anche parole dai
turni con lacune di meccanismo; `dream_read_prose` passa le frasi al lettore
e si arresta se ne restano non comprese. `dream_run` interrompe quel giro
quando la lettura della pagina è incompleta. Queste scelte rendono visibile
il limite, ma non lo trasformano da sole in una lezione, in un rimedio
selezionato e nella ripresa del bisogno originario. Il sogno è una base di
acquisizione; eseguirlo indefinitamente non costituisce ancora addestramento
continuo della comprensione.

Propongo un unico episodio persistente di apprendimento, raccordato alle
questioni e ai candidati esistenti. L'episodio deve conservare il bisogno
originario, il contenuto già compreso, il residuo, le ipotesi alternative,
il sostegno richiesto, la revisione proposta e ciò che resta da riprendere.
La persistenza dell'agenda è distinta dalla persistenza dei fatti: molte
questioni dialogiche attuali sono stato effimero, mentre un ciclo che
attraversa sessioni deve ricordare anche il lavoro incompleto.

| Passaggio del ciclo proposto | Operazione cognitiva | Ciò che deve sopravvivere al passaggio |
|---|---|---|
| Scegliere un bisogno | Dare priorità a questioni utili e ostacoli condivisi fra più capacità, mantenendo anche esplorazione di nuovi usi | Scopo e ragione della scelta |
| Localizzare il residuo | Distinguere informazione mancante, riferimento ambiguo, costruzione non letta, procedura assente e errore del motore | Parte capita, parte irrisolta e dipendenza bloccante |
| Chiedere o leggere | Cercare informazione che separi le interpretazioni ancora possibili | Risposta attribuita e alternativa che permette di escludere |
| Costruire un candidato | Legare la lezione a una relazione, a una regola o a una procedura esistente; se necessario proporre un'astrazione | Parametri, condizioni di applicazione e origine |
| Riprendere il bisogno | Applicare il cambiamento al contenuto rimasto sospeso e propagarne le conseguenze | Risultato sostenuto oppure un nuovo residuo esplicito |
| Consolidare | Rendere la conoscenza disponibile ai consumer pertinenti; raccogliere passi ricorrenti in procedure riusabili | Dipendenze, limiti, possibilità di correzione |
| Proseguire | Riutilizzare quanto appreso in un altro bisogno e aggiornare l'agenda | Capacità acquisita e lavoro ancora aperto |

Il ritorno al bisogno e il controllo delle conseguenze sono parte del
processo di apprendimento: il ciclo deve distinguere una spiegazione che
cambia la comprensione da una semplice conferma verbale. Una domanda
discriminante è spesso più utile di una richiesta generica di altri
esempi. La scelta attiva dell'informazione da chiedere ha un fondamento
nella letteratura sull'active learning; l'adattamento a residui, regole e
dialoghi di parrot0 è una proposta di questo report, non una conseguenza
automatica di quella letteratura. [Settles, Active Learning Literature
Survey](https://burrsettles.com/pub/settles.activelearning.pdf).

L'agenda dovrebbe alternare due direzioni. Una segue le questioni reali di
F. e dei testi: assicura utilità e incontra ciò che il sistema non sa ancora
nominare. L'altra esercita l'uso incrociato degli operatori già posseduti:
negare una relazione, riprenderne gli argomenti, chiederne le conseguenze,
correggerne una premessa. Il solo inseguimento degli arresti ignora gli
errori sicuri; la sola esplorazione interna resta entro il mondo che il
sistema sa già costruire. Servono entrambe, con una scelta del prossimo
bisogno che resti modificabile in KB.

Il passaggio capace di cambiare la scala è la **costruzione di astrazioni
riutilizzabili a partire da procedure e letture già acquisite**. Quando
più soluzioni condividono una struttura, parrot0 dovrebbe poterla nominare,
parametrizzare e usare come componente della soluzione successiva. Anche
le condizioni che ne vietano l'applicazione fanno parte dell'astrazione.
Consolidare non significa cancellare eccezioni o prove: si conserva la
genealogia e si rende disponibile una procedura più generale.

Un precedente pertinente è DreamCoder: alterna ricerca di programmi e
apprendimento di nuove astrazioni simboliche riutilizzabili. Mostra una
forma concreta del ciclo «risolvi → costruisci strumenti → riusa», non la
possibilità dimostrata di ottenere comprensione universale con parrot0.
DreamCoder usa anche reti neurali per guidare la ricerca e lavora entro
linguaggi e compiti definiti; non è una prova della sufficienza di un
teacher dialogico o dell'approccio puramente simbolico. Il riferimento
serve a precisare il meccanismo di accumulazione che manca, non a
trasferirne i risultati al progetto. [Ellis et al.,
DreamCoder](https://arxiv.org/abs/2006.08381).

Un'implementazione graduale del processo dovrebbe partire da una famiglia
di bisogni reali con una catena già percorribile e frontiere dichiarate.
Il teacher offre lezioni e chiarimenti naturali; parrot0 conserva il
bisogno, incorpora il candidato e lo usa. Una costruzione non esprimibile
con i meccanismi presenti diventa un intervento di sviluppo distinto.
Il blocco di quel ramo non deve cancellare l'agenda né promuovere una
conoscenza incerta: altri episodi indipendenti e già ammissibili possono
continuare. Questi sono criteri per un futuro esecutore, non autorizzazioni
a ignorare i vincoli di promozione correnti.

**14. Le falle dell'ipotesi e le condizioni della possibile convergenza.**

La prima falla potenziale è la circolarità del prerequisito: aspettare la
comprensione universale per cominciare l'addestramento che dovrebbe
produrla. Serve un nucleo didattico limitato ma capace di estensione.
La comprensione successiva nasce dall'uso e dalla revisione di quel nucleo.
La prescrizione di chiudere molti strati prima del consolidamento massivo,
presente in apprendimento assistito, ha una ragione di affidabilità; letta
come divieto di cicli continui circoscritti renderebbe però mobile e forse
irraggiungibile la soglia di partenza. Il piano di integrazione ammette già
i piccoli cicli: è questa distinzione che renderei operativa.

La seconda è confondere l'espressività del linguaggio interno con quella
del learner. Una procedura può essere scrivibile in `.p0` senza essere
acquisibile da una spiegazione. Se la trasformazione necessaria è fuori
dal repertorio del learner, altre lezioni attraverso lo stesso canale non
la introducono per semplice ripetizione. Occorre un metodo per costruirla
da componenti già apprendibili, oppure riconoscere che serve un nuovo
meccanismo generale. Studiare grammatica e `.p0` diventa decisivo soltanto
quando quella conoscenza governa l'interpretazione e la costruzione di
procedure: saperne parlare non basta.

La terza è assumere che una generalizzazione abbia individuato una legge
perché rende coerenti gli esempi disponibili. Esempi finiti sono compatibili
con più regole, che possono divergere fuori da essi. Il ciclo ha bisogno
di alternative, condizioni di validità e occasioni di correzione, anche
quando il dialogo non presenta contraddizioni. Un teacher LLM è utile come
fonte di proposte, ma le sue conferme ripetute non sono sostegni indipendenti.

La quarta è che l'automazione selezioni soltanto ciò che sa già insegnare.
Un processo può essere instancabile e restare nello stesso piccolo
metalinguaggio. Rendere persistenti i residui fuori repertorio e lasciare
che bisogni esterni influenzino l'agenda evita di chiamare crescita della
comprensione il solo riempimento di schemi disponibili.

La quinta è aspettarsi che tutto ciò che si aggiunge resti compatibile
senza revisione. Nuovi fatti possono correggere vecchie credenze; nuove
forme possono creare ambiguità; astrazioni più ampie possono richiedere
distinzioni prima inutili. Il progresso cumulativo non implica verità
immutabili. Richiede che la revisione conservi fonti e motivazioni e che
l'accesso alla conoscenza resti sostenibile mentre il repertorio cresce.

La sesta è confondere auto-correzione con auto-conferma. Se lo stesso
meccanismo propone una lettura, formula la risposta e la giudica solo
attraverso indizi della propria prosa, il ciclo può stabilizzarsi su un
errore. Le premesse formali, le fonti e gli effetti delle azioni devono
poter contraddire il risultato, e il teacher deve poter correggere anche
il criterio con cui il sistema accetta una lezione.

La mia conclusione è quindi distinta per ambizione. Un processo continuo
assistito è costruibile mediante un'agenda persistente, episodi
riprendibili e i canali di apprendimento esistenti, completando i raccordi
descritti. Una crescita utile cumulativa è plausibile se il ciclo amplia
anche il repertorio delle trasformazioni e ne collega gli usi. La
convergenza a capacità comparabili a un LLM resta un'ipotesi aperta:
nessuno di questi meccanismi, né la loro sola presenza, la garantisce.

Per rendere l'ipotesi esigente senza tornare a un conteggio di pennellate,
chiederei questo al progetto: **una lezione appresa oggi diventa uno
strumento con cui parrot0 comprende e acquisisce la lezione di domani?**
Se la risposta resta «serve un nuovo raccordo scritto dal programmatore»,
il processo è continuo nel tempo ma non ancora cumulativo nella capacità
di imparare. Se invece la nuova distinzione entra nei consumatori comuni
e nella costruzione delle lezioni successive, si è identificato un
meccanismo concreto per allargare la zona utilizzabile della tela.

**15. L'addestramento può trasferire comportamento: «conoscenza» e «KB viva» non sono alternative.**

La domanda di F. richiede una risposta più netta: **in parrot0 una lezione
può già modificare ciò che il sistema fa, non soltanto ciò che sa dire.**
In [assisted-learning.p0](../../kb/core/assisted-learning.p0), una procedura
acquisita diventa una lista di passi; `procedure_apply_steps` la interpreta
ricorsivamente. Il contenuto appreso determina la sequenza delle operazioni.
Non è una descrizione conservata accanto a un algoritmo invariato nel suo
comportamento: è parte del programma effettivamente eseguito. Rimane il
limite del repertorio di operatori e delle forme di lezione acquisibili,
già discusso, ma il principio di conoscenza operativa è presente.

Una rappresentazione concettuale utile, non una nuova API, è questa:

```text
stato successivo = interprete(stato corrente, ingresso, KB)
KB successiva    = apprendimento(KB, lezione, contesto, correzione)
```

L'interprete può restare fisso mentre la sua condotta cambia profondamente:
dipende da quanto la KB governa le trasformazioni dello stato. Imparare
un dato cambia una risposta possibile; imparare una regola amplia le
conseguenze ricavabili; imparare una procedura cambia le operazioni
componibili; imparare una politica cambia quando scegliere quelle operazioni.
Se le procedure apprese partecipano anche alla seconda riga, l'apprendimento
può modificare il proprio metodo entro le possibilità dell'interprete.
Non serve, per questo, che il sistema riscriva il C o inventi istruzioni
macchina nuove.

Definirei dunque «viva» la **KB nel suo rapporto con l'esecutore**, non il
file isolato: una conoscenza entra nel lavoro in corso, modifica letture
e decisioni, produce conseguenze, riceve correzioni e partecipa a nuove
acquisizioni. Qui «viva» è una proprietà operativa, non un'affermazione su
coscienza o esperienza soggettiva. Esistono gradi di questa proprietà, non
una divisione assoluta fra archivio morto e mente completa.

Il requisito KB-first riguarda proprio questo confine. Spostare una parola
dal C a una tabella apre il vocabolario; rendere apprendibile la combinazione
di condizioni apre una parte della grammatica del comportamento. Il
commento gen489 in [00-lex.c](../../src/brain/00-lex.c) formula già questa
distinzione: aggiungere membri a ruoli fissi non equivale a imparare una
forma nuova se la congiunzione dei ruoli resta compilata. È un segnale
diretto che il progetto ha incontrato il problema posto da F.

La criticità, quindi, non è che una KB simbolica possa trasferire soltanto
fatti. È che la parte apprendibile della condotta resta disomogenea:
procedure, forme e politiche sono modificabili in alcuni percorsi; altrove
il loro uso resta delimitato da scelte locali dell'implementazione.
Un fatto appreso non apre da solo un passaggio che nessun consumer consulta.
Viceversa, una regola generale appresa e consultata nei passaggi condivisi
può cambiare molti comportamenti insieme: questa è una possibile uscita
dalla crescita per pennellate.

**16. L'esistenza degli LLM dimostra la possibilità computazionale, non la convergenza di qualsiasi learner.**

L'argomento di F. ha un nucleo corretto e importante. Un LLM implementato
su un calcolatore esegue operazioni fisiche e computabili. Per riprodurne
il comportamento osservabile non occorre postulare un ingrediente magico.
Con memoria, precisione e risorse fissate, il sistema può essere descritto
come una macchina a stati finiti, includendo stato del generatore
pseudocasuale e contesto. Se si ammettono memoria e durate estendibili,
la descrizione appropriata è più generalmente una macchina computazionale
con memoria. Questa è una conseguenza della sua implementazione digitale,
non una teoria completa della cognizione umana.

La distinzione necessaria riguarda tre affermazioni:

| Affermazione | Che cosa possiamo concludere |
|---|---|
| Esiste una macchina computazionale con le capacità osservate nel maestro | Sì: l'implementazione del maestro è già una costruzione di quel tipo |
| Esiste una ricostruzione simbolica compatta, trasparente ed economicamente praticabile delle stesse capacità | L'esistenza del maestro non basta a stabilirlo; una simulazione esatta potrebbe conservarne quasi tutto il costo e la complessità |
| Il learner attuale di parrot0 può raggiungere quella ricostruzione attraverso lezioni | Non segue dalle prime due: dipende da rappresentazioni, trasformazioni apprendibili, ricerca e correzione |

Una tabella immensa delle transizioni sarebbe una descrizione teorica, non
una soluzione alla missione. Il lavoro interessante è trovare una
fattorizzazione riutilizzabile: pochi meccanismi capaci di comporre molte
condotte. Neppure occorre riprodurre gli stati interni del maestro uno per
uno: l'obiettivo dichiarato è una capacità funzionale comparabile.

Il trasferimento di comportamento fra modelli non è soltanto un'idea
astratta: la distillazione studia come addestrare un modello a riprodurre
informazione predittiva di un altro modello o di un insieme di modelli.
Il lavoro di Hinton, Vinyals e Dean ne fornisce una realizzazione concreta.
Non dimostra però il trasferimento completo del ragionamento di un LLM
in una KB simbolica mediante conversazione. Lo cito come precedente
del trasferimento funzionale, non come garanzia per parrot0.
[Distilling the Knowledge in a Neural Network](https://arxiv.org/abs/1503.02531).

La lezione teorica per il progetto è incoraggiante ma precisa: non c'è
ragione di escludere la missione perché il substrato è simbolico o perché
il motore è deterministico. Occorre però spiegare **come l'aggiornamento
della KB raggiunga le trasformazioni che producono la capacità desiderata**.
La computabilità stabilisce che una macchina può farlo; non stabilisce
quale curriculum e quale algoritmo la faranno apprendere a questa macchina.

**17. L'attenzione può essere una parte sottovalutata: conta la costruzione contestuale delle relazioni.**

L'intuizione di F. sulla «relazione tra le parole» merita un posto centrale,
con una precisazione meccanica. Nel Transformer, l'attenzione calcola pesi
dipendenti da query e chiavi e combina i valori corrispondenti. Le proiezioni
apprese e le diverse teste consentono combinazioni differenti; negli strati
successivi si opera su rappresentazioni già trasformate dal contesto.
Non è soltanto una tabella di associazioni lessicali. L'architettura comprende
anche trasformazioni feed-forward, rappresentazioni dei token, informazione
posizionale e connessioni residuali: attribuire tutto il risultato alla sola
attenzione sarebbe incompleto. [Vaswani et al., Attention Is All You Need,
§3](https://arxiv.org/html/1706.03762v7).

La funzione che propongo di mettere a fuoco in parrot0 è questa: **il
contesto deve poter modificare la rappresentazione che viene elaborata,
oltre a scegliere quale elaboratore chiamare.** Se il sistema decide
presto «questa è una domanda di tipo X», passa un frammento a una facoltà
e riceve una risposta finita, buona parte delle altre relazioni può non
entrare più nella costruzione della risposta. Aumentare il numero di
associazioni disponibili non risolve necessariamente questa selezione
prematura.

Non sarebbe corretto dire che parrot0 non possiede nulla di pertinente.
`kb_hypothesis_best`, in [kb.c](../../src/kb.c), aggrega evidenze per
candidati; [dialogue-frames.p0](../../kb/core/dialogue-frames.p0) e
[issues.p0](../../kb/core/issues.p0) rappresentano ruoli e questioni;
le letture dei documenti possono essere rivedute. Esistono quindi selezione,
contesto e revisione. Da queste presenze non segue però un equivalente
funzionale dell'elaborazione contestuale distribuita: un punteggio di
riconoscimento è una componente, non l'intero processo di costruzione del
significato.

La lettura statica segnala una tensione concreta: `p0_frame_reading` sceglie
la prima lettura ammissibile nel proprio percorso, il registro delle
facoltà termina al primo risultato accettato, e il percorso dei turni
composti può concatenare risposte a clausole separate. Sono i punti già
descritti nelle sezioni 4–5. Il loro limite comune, rispetto all'ipotesi
di F., è la difficoltà di far rientrare le relazioni fra parti nella
revisione della lettura complessiva prima di rispondere.

Un esempio concettuale chiarisce la differenza. «Luca ha passato a Marco
il libro che aveva preso in biblioteca; ora vuole riaverlo». Non basta
attivare *Luca*, *Marco*, *libro* e *biblioteca*. Occorre distinguere
partecipanti ed eventi, collegare *lo* al libro, conservare l'eventuale
ambiguità del soggetto di *vuole*, e usare la domanda successiva per
stabilire quale distinzione serva. L'attenzione neurale non garantisce da
sola una lettura corretta di questo esempio; esso indica il lavoro
contestuale che un'alternativa simbolica deve saper organizzare.

La colla linguistica di F. può essere letta come un'ipotesi su questa
organizzazione a scala di discorso. Non è identica alla self-attention:
la colla richiesta deve mantenere anche scopi, attribuzioni, obblighi e
correzioni attraverso i turni. Il collegamento funzionale è plausibile:
rendere produttive relazioni dipendenti dal contesto, anziché trattare
ogni contributo come informazione isolata. Rimane una proposta del report,
non un'equivalenza dimostrata fra le due architetture.

**18. Il pezzo complementare all'attenzione è imparare quali collegamenti costruire.**

Un meccanismo capace di collegare tutto a tutto lascia aperta la domanda
decisiva: quali collegamenti sono utili qui, e come si impara a costruirli?
Nel Transformer l'addestramento ottimizza parametri anche nelle proiezioni
dell'attenzione e nelle trasformazioni successive; il segnale di errore
può quindi modificare le operazioni che costruiscono le rappresentazioni,
non soltanto inserire contenuti da ricordare. Questa osservazione deriva
dalla struttura e dall'addestramento del modello descritto da Vaswani et al.;
non implica che ogni aggiornamento produca un miglioramento cognitivo.
[Attention Is All You Need, §§3 e 5](https://arxiv.org/html/1706.03762v7).

Per parrot0 il problema corrispondente è l'**attribuzione della correzione
al passaggio responsabile**. Se il maestro dice «qui *lo* riprende il libro,
ma non sappiamo ancora chi lo rivuole», il learner deve poter cambiare una
regola di riferimento o una condizione di scelta. Registrare soltanto la
frase corretta lascia invariato il meccanismo che ha sbagliato. Registrare
una regola troppo ampia, invece, esporta l'errore su altri contesti.
Servono rappresentazioni dei passaggi e alternative modificabili; il
maestro può aiutare a distinguere quale passaggio correggere, senza che
ogni episodio richieda di ricostruire dall'esterno l'intero programma.

Propongo dunque di precisare l'analogo funzionale dell'attenzione attraverso
un ciclo sullo spazio di lavoro condiviso, non attraverso una nuova facoltà
chiamata «attenzione»:

1. La lettura rende disponibili frammenti, entità, eventi, ruoli e scopi,
   conservando ciò che è ancora ambiguo o non interpretato.
2. Regole di pertinenza apprendibili selezionano legami candidati in funzione
   della questione aperta, dello scope e delle dipendenze già note.
3. Le procedure applicabili aggiungono vincoli o conseguenze agli stessi
   oggetti; questi contributi possono rivedere le letture precedenti.
4. Il controllo prosegue finché resta un avanzamento pertinente, oppure
   espone un residuo e chiede informazione. Limiti di risorse e assenza di
   novità devono fermare il ciclo senza fingere di aver chiuso la questione.
5. Una correzione aggiorna il legame, la procedura o la politica responsabile,
   mantenendo condizioni e origine. La nuova conoscenza governa poi altri
   episodi attraverso i medesimi punti d'accesso.

Sono requisiti di progetto, non descrizione di un ciclo già completo.
Gli oggetti di partenza esistono in parte; la novità necessaria è che le
facoltà vi contribuiscano prima della scelta finale e che il learner possa
modificare i criteri di collegamento. Una ricerca simbolica indiscriminata
su tutta la KB potrebbe diventare impraticabile: selezione, indicizzazione,
riuso delle astrazioni e controllo del lavoro sono parti sostanziali della
proposta, non dettagli da rinviare alla fine.

Qui la «sola coerenza dialogica» acquista una formulazione più utile.
Se significa soltanto non contraddirsi, è un vincolo insufficiente: anche
non rispondere mai evita molte contraddizioni. Se significa soddisfare
progressivamente le richieste, conservare i referenti, applicare le
relazioni pertinenti e incorporare le correzioni, può essere un criterio
di apprendimento molto ricco. Ma occorre implementare le trasformazioni
e il modo di correggerle: la coerenza specifica una qualità desiderata del
processo, non fornisce da sola l'algoritmo che la produce.

Questo limite resta anche assumendo una KB vera. La questione non è più
«e se le premesse fossero false?», bensì «come sceglie e combina le premesse
pertinenti per questa richiesta?». Una derivazione corretta da premesse
ipotetiche vale come ragionamento; una risposta coerente ma estranea alla
domanda non diventa pertinente perché tutti i suoi fatti sono veri.

**19. Quando potrà comportarsi come il maestro? La soglia è funzionale, non una quantità di KB.**

È possibile che l'acquisizione di una struttura molto riusabile produca
un cambiamento ampio: insegnare un modo di legare riferimenti può rendere
utilizzabili molte conoscenze già presenti. La crescita osservabile può
allora sembrare discontinua, pur derivando da un cambiamento preciso del
meccanismo. Non è necessario immaginare che ogni capacità debba essere
insegnata caso per caso; questo è il lato promettente della tesi di F.

Esiste un precedente meccanicistico circoscritto: Olsson et al. descrivono
teste di attenzione che realizzano uno schema di continuazione
`[A][B] ... [A] → [B]` e ne studiano il rapporto con l'apprendimento nel
contesto. Riportano evidenza causale forte per piccoli modelli composti
di sola attenzione e correlazionale per modelli più grandi con MLP.
È un esempio di come un circuito riusabile possa sostenere una capacità;
non è una dimostrazione di una soglia universale di ragionamento, né di
una soglia futura per parrot0. [In-context Learning and Induction
Heads](https://arxiv.org/abs/2209.11895).

Va inoltre distinto l'adattamento durante un episodio dalla modifica
persistente del learner. Usare una regola fornita nel contesto e conservare
un metodo che permetterà di acquisire regole future sono due risultati
diversi. Per la KB viva richiesta da F. occorrono entrambi: plasticità
del lavoro corrente e consolidamento selettivo di ciò che diventa riusabile.

Non possiamo indicare un momento in cui, superato un certo numero di
lezioni, parrot0 diventerà simile al maestro. Possiamo però definire che
cosa dovrebbe essere accaduto architetturalmente: le lezioni devono
alimentare rappresentazioni composizionali; queste devono governare
lettura, inferenza e risposta; le procedure nuove devono essere richiamabili
in altri contesti; il sistema deve saper incorporare correzioni al proprio
modo di selezionarle. Infine, quelle capacità devono partecipare al processo
di apprendimento stesso. L'insieme rende plausibile un avvicinamento;
la sua sufficienza e praticabilità per una competenza ampia restano aperte.

«Comportarsi come il maestro» va inteso come acquisire la capacità di
affrontare famiglie di situazioni nuove, non riprodurne le frasi o richiamarlo
per ogni decisione difficile. Il maestro può continuare a insegnare:
l'autonomia cercata consiste nel fatto che, su ciò che ha appreso, sia
parrot0 a costruire e usare la soluzione. Non richiede indipendenza da
qualsiasi fonte esterna, né infallibilità, né conoscenza completa del mondo.

La formulazione conclusiva che accoglie meglio l'ipotesi di F. è dunque:
**un interprete computazionale con conoscenza eseguibile e collegamenti
contestuali apprendibili potrebbe acquisire dal dialogo anche metodi di
ragionamento, se le lezioni modificano le operazioni condivise con cui
comprende, agisce e apprende ancora.** L'esistenza degli LLM rende concreta
la possibilità di capacità computazionali di questo ordine. Non dimostra
che l'implementazione attuale di parrot0 possieda già un percorso sufficiente
per acquisirle.

La risposta alle due domande iniziali è quindi asimmetrica. Alla prima,
«l'addestramento trasferisce soltanto conoscenza descrittiva?», la risposta
è **no: può trasferire comportamento, e parrot0 ne implementa già una forma
locale**. Alla seconda, «ci sarà un momento in cui saprà comportarsi come
il maestro?», la risposta è **possibile come direzione di ricerca, non
garantito dalla crescita attuale della KB**. La funzione dell'attenzione
e l'apprendimento dei criteri che la governano sono candidati seri per
spiegare parte della distanza. Ridurre tutto alla mancanza di fatti o alla
loro verità perderebbe proprio il punto architetturale sollevato da F.

**Direzione di progetto conseguente.**

La priorità che ricavo dall'implementazione è rendere comune il ciclo
comprensione–deliberazione–apprendimento, riusando gli oggetti già presenti.
Il [piano di integrazione cognitiva](../plans/integrazione-cognitiva-operativa.md)
va in questa direzione; qui la considero un'esigenza della struttura del
programma, non una nuova lista di capacità da aggiungere.

1. Fare della proposizione contestuale, dei suoi referenti e dei suoi
   vincoli l'oggetto condiviso fra lettura, domanda, spiegazione, memoria e
   generazione. Una parafrasi deve poter cambiare superficie conservando
   quell'identità; una correzione deve cambiarne esplicitamente il contenuto.
2. Fare della richiesta un insieme di obblighi distinti e collegati. Le
   facoltà contribuiscono a soddisfarli; il loro successo locale non chiude
   da solo il turno. Il controllo sceglie in base a copertura e sostegni.
3. Unificare le procedure attorno a parametri, precondizioni, effetti,
   dipendenze e risultati strutturati. La lezione naturale e il metodo letto
   devono poter arrivare alla stessa rappresentazione eseguibile.
4. Estendere la revisione dalle letture ai loro utilizzatori. Una premessa
   superata deve rendere rivedibili la conclusione, la sintesi e il piano che
   vi si appoggiavano; attribuzione e ignoto devono sopravvivere ai passaggi.
5. Dare alla generazione un piano di contenuto comune. Template,
   morfologia e scelte stilistiche diventano strumenti per realizzare quel
   piano; la composizione inferenziale esistente ne offre già un inizio.
6. Rendere l'apprendimento un processo persistente che attraversa questi
   oggetti: seleziona bisogni, conserva residui, chiede chiarimenti,
   costruisce candidati, riprende il compito e consolida procedure riusabili.
   L'estensione del repertorio didattico deve essere uno dei suoi scopi,
   insieme all'acquisizione di conoscenza sul mondo.
7. Rendere apprendibili i criteri che costruiscono e selezionano i legami
   contestuali, collegando una correzione al passaggio responsabile.
   La priorità di una facoltà non sostituisce questa elaborazione comune:
   il contesto deve poter cambiare la lettura prima che la risposta la chiuda.

Il mio giudizio sul progetto è dunque favorevole al suo nucleo dichiarativo
e critico sull'organizzazione che lo circonda. parrot0 possiede strumenti
concreti per rendere il comportamento conoscenza; deve ancora fare in modo
che questi strumenti governino sistematicamente il comportamento intero.
Il passaggio decisivo verso la missione è che una nuova comprensione diventi
utilizzabile nello spiegare, nel ricordare, nel decidere e nell'agire,
attraverso gli stessi oggetti e le stesse procedure. L'integrazione richiesta
da F. aggiunge la condizione che rende questo investimento cumulativo:
quelle procedure devono contribuire anche alla costruzione della conoscenza
successiva. È sulla continuità fra comprendere, usare, correggere e imparare
ancora che investirei il lavoro architetturale.

L'ultima precisazione di F. sposta ulteriormente il centro: non basta che
la KB contenga conoscenza sul comportamento del maestro; deve contenere
conoscenza che diventi comportamento del discente. Questo passaggio è
possibile localmente nell'architettura attuale. Generalizzarlo attraverso
relazioni contestuali e procedure apprendibili è il problema di ricerca
che il report identifica, senza attribuirgli una soluzione già dimostrata.
