# Verso una KB viva — quadro preliminare di progetto

9 settembre 2026. Documento pre-operativo derivato dal
[report sull'architettura e la missione](../reports/2026-09-09-architettura-e-missione.md),
con le osservazioni e le ipotesi di F. sulla crescita della KB, sulla colla
linguistica e sull'apprendimento del comportamento del maestro.

**Stato: proposta di inquadramento, non piano esecutivo.** Questo documento
prepara le decisioni dalle quali potrà nascere un piano. Non assegna lavori,
non stabilisce una sequenza di implementazione, non autorizza addestramento
e non dichiara risolte le questioni che espone. I riferimenti al codice
derivano dalla lettura statica del report al commit `4360751`.

## 1. Perché un quadro preliminare

La difficoltà descritta da F. è la distanza fra crescita locale e crescita
del soggetto nel suo insieme. Ogni integrazione può aprire una capacità
interessante, ma il territorio utilizzabile resta discontinuo: una pennellata
in una tela ancora prevalentemente nera. Aggiungere conoscenza, completare
un percorso e migliorare la capacità di apprendere non sono necessariamente
lo stesso avanzamento.

Il problema da porre prima di un nuovo piano è quindi:

> Quale organizzazione permette a una lezione di diventare comportamento
> riutilizzabile e, successivamente, strumento per comprendere altre lezioni?

Un piano che partisse direttamente da «aggiungere attenzione» o «avviare
addestramento continuo» avrebbe già scelto una risposta senza esplicitare
il meccanismo atteso. Questo quadro serve a precisare quel meccanismo e
le alternative ancora aperte. Non sostituisce l'indagine sulle capacità
reali con una nuova terminologia.

La missione resta acquisire capacità di ragionamento e proprietà di
linguaggio comparabili a quelle del maestro LLM. «Comprensione universale»
indica qui l'ambizione di estendere e comporre la comprensione attraverso
domini e situazioni, non il possesso di ogni fatto né l'infallibilità.

## 2. Le distinzioni da conservare

Il report, soprattutto nelle sezioni 10–19, suggerisce di separare tre
livelli di affermazione.

**Base osservata nell'implementazione.** La KB può contenere conoscenza
eseguibile: procedure apprese determinano sequenze di operazioni. Esistono
anche forme linguistiche, politiche, questioni e letture rivedibili.
Questa capacità è locale e disomogenea; non equivale ancora a un metodo
generale per acquisire e integrare nuove condotte.

**Ipotesi di progetto.** Rappresentazioni condivise, relazioni contestuali
apprendibili e procedure riutilizzabili potrebbero trasformare l'accumulazione
in crescita cognitiva cumulativa. L'addestramento diventerebbe produttivo
anche perché modifica gli strumenti con cui il sistema apprende ancora.

**Questione aperta.** Non sappiamo se questi meccanismi, resi sufficientemente
generali e sostenibili, possano portare parrot0 a una competenza ampia
comparabile al maestro. L'esistenza degli LLM rende concreta la possibilità
computazionale di tali capacità; non dimostra che qualsiasi architettura
o qualsiasi processo didattico possa acquisirle.

Non si attende una quantità magica di fatti. Si cerca una struttura che
possa rendere molte conoscenze reciprocamente utilizzabili. La distinzione
è compatibile con la scommessa di [PRINCIPLES.md](../../PRINCIPLES.md),
senza trasformarla in una promessa di convergenza.

## 3. La tesi che questo documento propone di maturare

> Un interprete generale può sostenere una KB viva se la conoscenza appresa
> modifica le operazioni con cui il sistema comprende il contesto, sceglie,
> ragiona, si esprime, si corregge e acquisisce altra conoscenza.

«Viva» qualifica il rapporto fra KB ed esecutore: non il file da solo e
non una presunta esperienza soggettiva. La conoscenza è operativa quando
partecipa al lavoro, riceve correzioni e cambia le possibilità successive.

In questa prospettiva, trasferire conoscenza e trasferire comportamento
non sono alternative. Una procedura, una regola di riferimento o un criterio
di scelta sono conoscenza che può diventare condotta. Il confine decisivo
è fra una descrizione di ciò che il maestro fa e una rappresentazione che
parrot0 sa effettivamente usare per farlo.

La forma più esigente della tesi è riflessiva: il sistema deve poter usare
parte di ciò che apprende per interpretare e acquisire nuove lezioni.
Non richiede che riscriva il proprio C. Richiede che l'interprete renda
componibili e apprendibili trasformazioni abbastanza espressive, comprese
quelle che partecipano all'apprendimento.

Resta distinta la qualità delle premesse. Un'inferenza corretta su un mondo
ipotetico è ragionamento; la falsità di una premessa non confuta da sola
la capacità inferenziale. Anche assumendo una KB vera, però, occorre
spiegare come il sistema selezioni e combini le relazioni pertinenti alla
richiesta. È questo il problema architetturale qui in discussione.

## 4. Attenzione e colla linguistica: una direzione funzionale

L'ipotesi di F. è che si stia sottovalutando il ruolo delle relazioni fra
parole e contesto. Il report (§§17–18, con i riferimenti tecnici) distingue
l'attenzione del Transformer da un semplice inventario di associazioni:
il suo interesse per questa discussione è l'elaborazione di rappresentazioni
dipendenti dal contesto e l'apprendimento delle trasformazioni coinvolte.

La domanda per parrot0 non è ancora se introdurre un particolare algoritmo
di attenzione. È se il contesto possa cambiare la lettura sulla quale il
sistema lavora, oltre a selezionare una facoltà che risponde.

La [colla linguistica](the-linguistic-glue.md) dà un nome alla continuità
ricercata fra referenti, memoria, inferenza, intenzioni e correzioni.
La possibile relazione con l'attenzione è funzionale, non un'identità
architetturale: entrambe suggeriscono di rendere produttivi collegamenti
che dipendono dalla situazione. La continuità dialogica richiede inoltre
scopi e dipendenze che attraversano i turni.

Questa direzione lascia aperte due questioni inseparabili:

- Come rappresentare e rivedere i collegamenti mentre la comprensione
  è ancora in corso, senza fissare prematuramente un'unica lettura?
- Come una lezione o una correzione modifica il criterio che ha costruito
  quei collegamenti, invece di registrare soltanto la risposta corretta?

La seconda questione impedisce di attribuire tutto a un nuovo meccanismo
di selezione. Un sistema che può collegare molti oggetti ma non può
apprendere quali collegamenti servano lascia irrisolto il problema della
crescita. Analogamente, chiamare «coerenza» il risultato desiderato non
fornisce ancora le operazioni che lo producono.

La formulazione utile dell'ipotesi di F. è dunque una coerenza semantica
attiva: conservare referenti e scopi, soddisfare richieste, incorporare
distinzioni e propagare correzioni. La sua sufficienza come guida
all'apprendimento resta da argomentare; la sola assenza di contraddizioni
non determina quale passo compiere.

## 5. La continuità da spiegare prima di progettarla

Il rapporto fra le componenti candidate può essere rappresentato così:

```text
bisogno e contesto
        ↓
lettura rivedibile ↔ relazioni e procedure pertinenti
        ↓                         ↑
conseguenza, risposta o residuo    │
        ↓                         │
lezione e correzione → conoscenza riutilizzabile
                              ↓
                  comprensione di bisogni e lezioni successive
```

È uno schema di dipendenze, non una pipeline da implementare né una
successione di milestone. L'attenzione è rivolta a ciò che attraversa i
passaggi: identità dei referenti, condizioni, alternative, origine della
conoscenza, scopo ancora aperto. Il report individua già oggetti pertinenti;
non propone di sostituirli con un secondo sistema parallelo.

La chiusura cercata è locale e riapribile: comprendere abbastanza da usare
una conoscenza, conservarne il limite e poterla correggere. Non è la
completezza di un dominio infinito. Una struttura parametrica può servire
molti casi senza enumerarli, ma la sua utilità dipende anche dai percorsi
che sanno riconoscerla e usarla.

Questo consente di immaginare un apprendimento continuo senza attendere
prima la comprensione universale. Un nucleo limitato può accogliere lezioni
e costruire strumenti più ampi. Rimane da chiarire dove finisca questa
estensione attraverso insegnamento e dove manchi invece una meccanica
generale che richiede sviluppo.

## 6. Le decisioni che precedono un eventuale piano

Le domande seguenti non sono una coda di task. Identificano scelte ancora
da motivare prima di assegnare modifiche a file o moduli.

| Nodo | Decisione da chiarire | Rischio di una scelta prematura |
|---|---|---|
| Oggetto condiviso | Quali identità, ruoli, scope e alternative devono restare gli stessi fra lettura, inferenza, memoria e generazione? | Creare una nuova rappresentazione che duplica quelle esistenti |
| Elaborazione contestuale | Quali contributi possono cambiare una lettura prima della risposta, e come sono selezionati? | Rinominare il dispatch come attenzione senza cambiarne la funzione |
| Lezione eseguibile | Quali forme di regola, procedura e politica il learner può costruire e comporre parlando? | Confondere ciò che è scrivibile in `.p0` con ciò che è apprendibile |
| Correzione del metodo | Come attribuire una correzione alla lettura, al legame o alla scelta responsabile? | Conservare la risposta corretta e lasciare invariata la causa dell'errore |
| Continuità didattica | Che cosa conserva il bisogno incompleto e che cosa riattiva il suo apprendimento, anche fra sessioni? | Automatizzare l'acquisizione senza aumentare la capacità di comprendere |
| Consolidamento | Quando una soluzione diventa una procedura parametrica e come conserva condizioni, eccezioni e dipendenze? | Moltiplicare casi o promuovere generalizzazioni prive dei loro limiti |
| Sostenibilità | Come limitare il lavoro e riusare risultati senza perdere conoscenza pertinente? | Rendere la crescita possibile in teoria ma impraticabile nell'uso |

Non serve sciogliere ogni questione sull'intelligenza prima di un piano.
Serve invece che il perimetro scelto abbia una spiegazione del passaggio
da lezione a condotta e da condotta a ulteriore apprendimento. Una domanda
rimasta aperta deve restare visibile come ipotesi, non diventare una
dipendenza tacita dell'implementazione.

## 7. I vincoli che una futura proposta deve ereditare

[MANTRA.md](../../MANTRA.md) e [PRINCIPLES.md](../../PRINCIPLES.md)
restano i riferimenti normativi. Questo documento non li sostituisce.

La conoscenza linguistica e di condotta deve essere insegnabile in KB:
non soltanto parole e risposte, ma condizioni, combinazioni e criteri di
scelta. Le meccaniche generali possono restare nel motore; una politica
cognitiva non diventa meccanica soltanto perché oggi è compilata.

L'insegnamento naturale non deve presupporre che il maestro conosca
predicati o formati interni. Un esperto che spiega una distinzione insegna;
un agente che scrive direttamente la sua rappresentazione interna svolge
un intervento diverso. Questa differenza va preservata anche quando
l'orchestrazione è automatica.

La KB è parte del soggetto intero. L'integrazione deve aumentare ciò che
le capacità possono vedere e distinguere; non ottenere coerenza togliendo
loro informazione. Le strutture secondarie restano risorse: uno spazio
di lavoro comune non impone un algoritmo monolitico né autorizza a
eliminare facoltà funzionanti sulla sola base di questa analisi.

La correzione deve poter raggiungere le conseguenze dipendenti; l'incertezza
non va promossa a fatto per chiudere un ciclo. Separare verità delle
premesse e capacità inferenziale non sospende la disciplina anti-inganno.

L'autonomia del discente non esclude il maestro. Esclude che, dopo ogni
lezione, sia ancora il maestro a costruire dall'esterno la soluzione di
ogni nuovo caso. L'apprendimento continuo non è sinonimo di esecuzione
ininterrotta né autorizzazione a campagne massive.

## 8. Rapporto con i documenti già presenti

Il [report](../reports/2026-09-09-architettura-e-missione.md) conserva
l'argomentazione, i riscontri nel codice e i riferimenti scientifici.
Questo quadro ne seleziona le conseguenze progettuali, mantenendo distinta
la proposta dalla constatazione.

[The linguistic glue](the-linguistic-glue.md) descrive la continuità
ricercata. [Apprendimento assistito](apprendimento-assistito.md) tratta
la KB viva e il canale didattico. Il
[piano di integrazione cognitiva](integrazione-cognitiva-operativa.md)
contiene già oggetti comuni e percorsi operativi: questo quadro non li
annulla e non introduce una roadmap concorrente. Evidenzia il problema
trasversale da rendere esplicito quando quei percorsi vengono discussi:
quali strumenti acquisiti entrano anche nell'apprendimento successivo?

[Procedura di crescita KB](procedura-crescita-kb.md) disciplina il riuso
delle distinzioni e dei circuiti. Qui se ne approfondisce la motivazione:
la crescita utile dipende dalla capacità di riutilizzare e comporre ciò
che si è aperto, non dalla sola quantità di integrazioni.

## 9. Quando questo quadro potrà generare un piano

Un futuro piano avrà una base sufficientemente chiara quando potrà nominare
un passaggio cognitivo circoscritto, mostrarne la continuità con gli oggetti
esistenti e spiegare quale trasformazione diventerà apprendibile. Dovrà
distinguere la meccanica da costruire dalla conoscenza da insegnare e
indicare perché l'acquisizione sarà riutilizzabile oltre il caso iniziale.

Solo in quel documento successivo avranno posto ordine degli interventi,
responsabilità, dettagli implementativi e modalità di verifica. Qui non
si scelgono soglie, benchmark, programmi di test o calendari. Questa
sospensione dell'operatività non è attesa della completezza: è il tempo
necessario a dichiarare quale ipotesi un intervento intende rendere concreta.

Il discrimine da portare alla discussione è uno:

> Dopo questa acquisizione, parrot0 dispone soltanto di una risposta in più,
> oppure di uno strumento in più per comprendere, ragionare e imparare?

Non tutte le lezioni devono modificare il metodo: anche i fatti servono.
Ma un progetto di KB viva deve spiegare come possano avvenire entrambe
le crescite e come si sostengano. È questo il preludio al piano, non ancora
il piano.
