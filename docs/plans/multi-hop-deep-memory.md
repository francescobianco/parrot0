# Parrot0 — Deep Memory Multi-Hop Mission

## Premessa

Parrot0 dispone di una **Deep Memory**, una memoria esterna, persistente e non parametrica il cui corpus è esclusivamente Wikipedia.

La Deep Memory non deve essere considerata semplicemente un sistema di information retrieval. Il suo ruolo è quello di fornire a Parrot0 conoscenza che non è disponibile nella sua KB o nel contesto corrente quando questa conoscenza diventa necessaria per proseguire un ragionamento.

Il ciclo fondamentale è quindi:

**reason → knowledge gap → recall → evidence → integrate → reason**

Parrot0 ragiona con ciò che possiede. Quando incontra una lacuna epistemica che impedisce di proseguire, può interrogare la Deep Memory, integrare l'informazione recuperata nella propria rappresentazione del problema e continuare il ragionamento.

La distinzione concettuale è:

**KB** → ciò che Parrot0 conosce operativamente
**Working Memory** → ciò che sta mantenendo durante il problema corrente
**Deep Memory** → ciò che può recuperare da Wikipedia quando necessario

Idealmente questo consente anche di distinguere epistemicamente informazioni:

**known** → presenti nella KB
**given** → fornite dal problema
**inferred** → ottenute mediante inferenza
**recalled** → recuperate dalla Deep Memory
**unknown** → non ancora disponibili

---

# Missione: Multi-Hop Deep Memory Reasoning

L'obiettivo dell'esperimento è dimostrare che Parrot0 può risolvere problemi la cui soluzione **non è direttamente disponibile né nel prompt né attraverso un singolo accesso a Wikipedia**, ma emerge attraverso una sequenza di accessi alla Deep Memory intercalati da operazioni di ragionamento.

Non vogliamo dimostrare semplicemente:

> Parrot0 sa effettuare dieci ricerche su Wikipedia.

Vogliamo dimostrare qualcosa di più forte:

> **Parrot0 è capace di risolvere autonomamente un problema che richiede una catena di N accessi alla memoria profonda, individuando progressivamente ciò che deve conoscere, recuperando l'evidenza necessaria, integrandola nella propria rappresentazione e utilizzandola per determinare il successivo passo del ragionamento.**

Il prompt, quindi, **non deve contenere istruzioni come "use your Deep Memory"**.

Parrot0 deve decidere autonomamente se e quando accedervi.

Allo stesso modo, il prompt non deve dichiarare:

> "This problem requires 5 hops."

Il numero degli hop è una proprietà nascosta del problema e viene osservato durante la valutazione.

Il comportamento desiderato è:

**Problem**
↓
**Reasoning**
↓
**Knowledge gap₁**
↓
**Deep Memory access₁**
↓
**Evidence₁**
↓
**IR / Working Memory update**
↓
**Reasoning**
↓
**Knowledge gap₂**
↓
**Deep Memory access₂**
↓
**Evidence₂**
↓
**...**
↓
**Final inference**
↓
**Answer**

Ogni risultato intermedio deve quindi poter diventare parte della rappresentazione utilizzata per formulare il bisogno epistemico successivo.

---

# Progressione sperimentale

La missione può essere affrontata progressivamente:

**DM-3 → DM-5 → DM-7 → DM-10**

dove il numero indica la profondità minima della catena di recupero necessaria alla soluzione.

Un problema DM-10 non dovrebbe essere semplicemente una catena artificiale come:

**persona → città → regione → stato → capitale → ...**

Gli hop dovrebbero possibilmente rappresentare **relazioni semanticamente differenti**.

Per esempio:

**opera → autore → influenza intellettuale → filosofo → opera → personaggio → figura storica → dottrina → testo → concetto → risposta**

La difficoltà consiste così nel mantenimento e nella trasformazione della rappresentazione attraverso domini e relazioni differenti.

---

# Linear e Branching Deep Memory

Possiamo distinguere due classi di esperimenti.

### Linear Deep Memory

Ogni hop determina sostanzialmente un unico successore:

**A → B → C → D → E**

Questi problemi sono ideali per misurare inizialmente la profondità raggiungibile.

### Branching Deep Memory

Un accesso può invece produrre più candidati:

**A → B → {C₁, C₂}**

Parrot0 deve mantenere più ipotesi nella Working Memory e utilizzare informazioni successive per eliminarne alcune:

**{C₁, C₂} → evidence → C₂**

Questa seconda categoria verifica qualcosa di più profondo della semplice navigazione: la capacità di preservare l'incertezza senza trasformare prematuramente un candidato in un fatto.

---

# Esperimento iniziale

Un primo problema può attraversare letteratura, filosofia, storia delle religioni e storia delle idee.

## Prompt

> Begin with the author of *The Brothers Karamazov*. Identify a philosopher who treated this author as an important precursor to his own thought. Determine which major philosophical work by that philosopher introduces the figure of a prophet who announces the death of God. Identify the historical religious figure from whom the prophet takes his name. Finally, determine the ancient religion traditionally associated with that figure. Return the religion and reconstruct every step that led you to it.

## Catena attesa

La catena generale è:

**The Brothers Karamazov**
↓
**Fyodor Dostoevsky**
↓
**Friedrich Nietzsche**
↓
**Thus Spoke Zarathustra**
↓
**Zarathustra / Zoroaster**
↓
**Zoroastrianism**

Ma questa catena **non viene fornita a Parrot0**.

Parrot riceve esclusivamente il prompt.

Il sistema deve scoprire progressivamente le relazioni necessarie.

Inoltre gli hop non sono semanticamente equivalenti:

**work → author**

**author → intellectual influence**

**philosopher → philosophical work**

**literary figure/name → historical figure**

**historical figure → religious tradition**

In particolare, la relazione fra Dostoevsky e Nietzsche non è necessariamente rappresentabile come un semplice attributo strutturato. Può essere necessario recuperare evidenza testuale, interpretarla e trasformarla in una relazione utilizzabile dal reasoning successivo.

Questo rende l'esperimento interessante anche per la costruzione della IR.

---

# Cosa registrare

Per ogni esecuzione dovremmo poter ricostruire almeno:

**1. Knowledge gap**

Quale informazione Parrot ritiene mancante.

**2. Deep Memory query**

Che cosa decide autonomamente di cercare.

**3. Retrieved evidence**

Quale informazione viene recuperata da Wikipedia.

**4. IR update**

Come quell'informazione viene rappresentata internamente.

**5. Inference**

Quale nuova conclusione diventa possibile.

**6. Next target**

Perché quella conclusione genera il successivo bisogno di conoscenza.

**7. Final answer**

La risposta conclusiva e la catena di evidenze che la sostiene.

In questo modo una risposta corretta ottenuta accidentalmente non equivale al completamento della missione.

---

# Metriche

Una prima metrica può essere il **Deep Memory Hop Rate**:

**DMHR(n) = problemi risolti correttamente con almeno n hop necessari / problemi totali di profondità n**

Possiamo quindi parlare di:

**DMHR-3**
**DMHR-5**
**DMHR-7**
**DMHR-10**

Una seconda misura può essere la **Hop Correctness Rate**:

**HCR = hop corretti e giustificati / hop complessivamente eseguiti**

Questa metrica penalizza sistemi che arrivano alla risposta corretta attraverso passaggi epistemicamente errati.

Ma una terza misura potrebbe essere ancora più importante per Parrot0:

**Autonomous Recall Rate (ARR)**

ovvero la percentuale di accessi alla Deep Memory che vengono attivati correttamente dal sistema in risposta a una reale lacuna epistemica, senza che il prompt suggerisca cosa cercare.

---

# Criterio finale della missione

Il traguardo **DM-10** non consiste semplicemente nell'avere effettuato dieci retrieval.

La missione è completata quando Parrot0 riesce, partendo esclusivamente dal problema, a produrre autonomamente una sequenza del tipo:

**reason**
→ **discover missing knowledge**
→ **recall from Wikipedia**
→ **validate evidence**
→ **update representation**
→ **infer**
→ **discover next missing knowledge**
→ ...
→ **final solution**

per una catena di almeno **10 hop necessari**, mantenendo tracciabile la provenienza epistemica di ogni informazione.

Il risultato interessante da dimostrare sarebbe quindi:

> **Parrot0 can autonomously traverse a ten-hop chain of heterogeneous knowledge dependencies using Wikipedia as Deep Memory, while preserving an explicit and inspectable reasoning state between successive recalls.**

A quel punto la Deep Memory non sarebbe più semplicemente "Wikipedia accessibile a Parrot0".

Diventerebbe parte del **ciclo cognitivo del sistema**:

**Parrot0 non deve possedere tutta la conoscenza necessaria prima di iniziare a ragionare. Deve essere capace di scoprire quale conoscenza gli manca mentre ragiona, recuperarla e continuare.**
