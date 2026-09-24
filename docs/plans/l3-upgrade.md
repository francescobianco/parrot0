# L3 — insegnare a parrot0 per contatto, senza schemi di lezione

**Piano di indirizzo, 24 settembre 2026. Stato: proposta, nessuna implementazione.**
Nasce da una conversazione fra F. e l'agente alla fine del lotto di iterazioni
di riferimento `2026-09-24` ([train-the-learning-process.md](train-the-learning-process.md),
RI-019…RI-023). Prosegue [l2-upgrade.md](l2-upgrade.md), di cui prende il limite
come punto di partenza, e riprende il criterio di evoluzione del
[MANTRA](../../MANTRA.md) (#18, #24, #26) portandolo un piano più giù. Si
appoggia sui quattro elementi del piano di training: la KB viva, la
[IR](universal-input.md), il [mondo allargato](the-rational-philosopher.md) e la
[comprensione universale](universal-comprehension.md), e su
[kb-first.md](kb-first.md). Il §6-bis dice che cosa diventa ciascuno sotto L3.

> **In una frase.** Finora ogni capacità di parrot0 è diventata insegnabile
> grazie a uno **schema di lezione**: una frase di un registro speciale
> («X is a relation», «X is a contraction of Y», «end the previous noun phrase
> before X»). Lo schema è il limite di L2. Una persona impara la stessa cosa
> **per contatto**, cioè dall'uso ordinario della lingua, dalla correzione e
> dall'apposizione, senza che nessuno passi a un registro diverso e senza
> conoscere i suoi processi interni. L3 è la capacità di parrot0 di adattarsi
> così. Il principio che la regge lo chiamiamo, provvisoriamente,
> **adatto-linguistico**.

> ⛔ **PRIMA DI LAVORARE A L3, LEGGERE IL §1-bis.** Il rischio principale di
> questo piano è che L3 diventi **una collezione di forme di ordine superiore**:
> schemi dall'aria più naturale che lasciano dietro di sé un **residuo
> metalinguistico**, che poi si ignora o si riduce. Non è vietato arrivare a una
> versione superiore di L2. È obbligatorio **non partire da lì**: chi lavora a
> questo piano deve prima tenere L3 indirizzato come qualcosa di **diverso** da
> L2. Costruire L3 banalmente, come un lettore di schemi solo un po' più
> naturali, è il **primo passo falso**. Prima si esplorano le possibilità
> evolutive; solo se non si trova nulla si ripiega, e il ripiego si dichiara.

---

## 0. Premesse

Tutto quello che segue presuppone cinque cose già stabilite nel progetto.

1. **La KB viva è il soggetto** (MANTRA, in testa). Crescere vuol dire far
   crescere la KB, e ogni misura vale solo sulla KB completa.
2. **Insegnare parlando vuol dire lingua naturale, non uno schema serializzato**
   (anti-barare del MANTRA). Se il maestro deve conoscere predicati, arità o
   `!assert`, non ha insegnato.
3. **Spostare il C non è portarlo in KB** (mantra #18). Un commit KB-first deve
   mostrare il C che si accorcia, e un template vuoto non è una resa.
4. **Una soluzione vale di più se amplia ciò che si può insegnare** (mantra #26).
   E il mantra stesso avverte: *«evitare un teach-handler C per ciascuna nuova
   capacità»*.
5. **L1 e L2** ([l2-upgrade.md](l2-upgrade.md)). L1: si insegna una proprietà di
   una **classe** (una parola, una relazione). L2: si insegna che cosa è vero di
   **questa occorrenza** (dove finisce questo sintagma, a chi rimanda questo
   pronome).

Questo documento aggiunge una premessa che le cinque non contengono:

6. **Che una conoscenza sia in KB non basta perché sia insegnabile.** La prova
   non è *dove sta* ma *quanto dista da una frase che la cambi*, e la frase deve
   essere quella di una persona che insegna a un'altra persona.

---

## 1. Da dove nasce: il problema degli schemi

### 1.1 Il lotto del 24 settembre, visto dall'alto

Cinque iterazioni chiuse, cinque capacità nuove, tutte certificate con prima,
lezioni, trasferimenti, contrasto, ablazione, salvataggio e processo nuovo.
Queste sono le lezioni usate:

```text
flash point is a relation                                   (RI-019)
the boiling point of x is y means x boils at y              (RI-020)
tell me the x means what is the x?                          (RI-021)
"i would like to know" is another way to say "tell me"      (RI-021)
a turn that contains "i would like to know" is a question   (RI-021)
"i'd like" is a contraction of "i would like"               (RI-022)
LED is short for light-emitting diode                       (RI-023)
```

Sono tutte in lingua naturale e superano l'anti-barare. Però sono tutte in un
**dialetto**: il registro «ti dico come leggere». Nessuno parla così a una
persona. È lo stesso registro in cui vivono le superfici di L2:

```text
end the previous noun phrase before opens
use that boundary for every verb
```

E ogni iterazione che apre una capacità nuova apre **uno schema nuovo**: RI-022
ha dovuto scrivere `teach_contraction_en` e `forget_contraction_en`, RI-023
`forget_abbrev`. È il sintomo che il §0.5-bis del piano di training aveva già
nominato («le forme e gli atti si moltiplicano, uno per superficie»), solo
spostato dal C alla KB.

### 1.2 Perché lo schema è un limite, e non soltanto una scomodità

Uno schema di lezione fa tre cose che l'insegnamento umano non fa:

- **separa le fasi**: prima «ti dico come leggere» (metalinguistico), poi «uso la
  lingua» (linguistico). Chi insegna deve sapere che esiste la prima fase e
  come si pronuncia;
- **chiude l'insieme delle cose insegnabili**: si insegna solo ciò per cui
  esiste uno schema. Una capacità nuova chiede uno schema nuovo, cioè lavoro di
  ingegneria, cioè qualcuno che conosce l'interno;
- **asserisce in un colpo solo**: la lezione diventa un fatto tenuto, senza che
  l'uso la metta alla prova.

Il limite di L2 non è la granularità (classe o occorrenza): è che **anche
l'occorrenza si corregge con uno schema**.

---

## 1-bis. ⛔ Il primo passo falso: L3 come L2 più naturale (F., 24 settembre 2026)

### Il rischio, detto per intero

La strada più corta verso L3 è anche quella sbagliata: prendere gli strumenti
del contatto (apposizione, «so», «I mean», «No, …») e scrivere per ciascuno una
**forma di ordine superiore** che li riconosce e ne ricava un fatto. Il
transcript sembrerebbe umano, perché il maestro dice «An LED, a light-emitting
diode, emits light» e non più «LED is short for light-emitting diode». Eppure
non sarebbe cambiato niente di ciò che conta:

- ogni strumento del contatto avrebbe **il suo schema**, e uno strumento nuovo
  («that is,», le parentesi, «also known as») chiederebbe uno schema nuovo;
- la decisione su **che cosa significa** uno strumento («un'apposizione glossa il
  nome che la precede») starebbe scritta nello schema, fuori dalla portata del
  contatto;
- il registro speciale sarebbe solo più ben nascosto: prima il maestro doveva
  conoscere la forma della lezione, adesso deve usare, senza saperlo, uno degli
  strumenti che qualcuno ha previsto.

Questo è il **residuo metalinguistico**: la parte del «come si legge una
lezione» che resta compilata, in C o in una forma KB, dentro un meccanismo che
dall'esterno sembra contatto. È pericoloso perché **si ignora** (il transcript è
naturale, quindi nessuno lo cerca) oppure **si riduce** (diventa più piccolo a
ogni iterazione e si conclude che prima o poi sparirà, mentre è solo il fondo
che nessuno ha provato a spostare).

### Un esempio, perché si riconosca quando capita

La cura sbagliata dell'esperimento del §7 sulla sigla sarebbe questa, e
funzionerebbe:

```prolog
% ⛔ il passo falso: uno schema, con una faccia più naturale
turn_form(appositive_gloss, 1, slot(short)).
turn_form(appositive_gloss, 2, text(",")).
turn_form(appositive_gloss, 3, span(long)).
turn_form(appositive_gloss, 4, text(",")).
turn_form_act(appositive_gloss, "op(assert, entity_alias, [short, long])").
```

«An LED, a light-emitting diode, emits light.» → `entity_alias(led, …)`. Il §7
passa. Ma: «an LED (light-emitting diode)» vuole un altro schema; «an LED —
that is, a light-emitting diode —» un terzo; «Paris, the capital, is large»
scriverebbe che «paris» *sta per* «the capital»; e nessuno può dire a parrot0
«no, qui le virgole racchiudono un inciso». È L2 con le virgole al posto di «is
short for».

### Che cosa è permesso e che cosa è imposto

- **Non è vietato** che L3, alla fine, abbia anche forme di ordine superiore. Se
  l'esplorazione non trova niente di meglio, una versione superiore di L2 è un
  risultato legittimo e va costruita bene.
- **È imposto l'ordine.** Chi lavora a questo progetto deve *in prima battuta*
  tenere L3 indirizzato come qualcosa di diverso da L2. Si esplorano prima le
  possibilità evolutive; il ripiego sugli schemi arriva solo dopo, e si dichiara
  come tale nella scheda dell'iterazione, con il residuo metalinguistico che
  lascia, elencato.
- **Il ripiego non si chiama L3.** Un'iterazione che passa il §7 con uno schema
  nuovo si registra come **L2+** (schema naturale), non come L3.

### Le possibilità evolutive da esplorare prima

Sono direzioni, non soluzioni. Hanno in comune una cosa: **nessuna associa uno
strumento del contatto a un significato scritto in anticipo.**

1. **Il ponte si induce dall'istanza, non dalla superficie.** «Water boils at 100
   °C, so its boiling point is 100 °C» mette accanto due letture dello stesso
   valore sullo stesso soggetto. Il ponte fra `boils_at` e «boiling point» si
   può *indurre* dalla coincidenza verificata («le due relazioni danno lo stesso
   valore sullo stesso oggetto»), qualunque connettivo le leghi. parrot0 ha già
   motori di induzione (`mod_induce`, il few-shot di gen104, l'archetipo
   relazionale) che non sono mai stati puntati sui turni del maestro.
2. **La coerenza al posto della regola.** Un'apposizione non «significa» glossa:
   la lettura propone che i due sintagmi siano lo stesso referente, e l'ipotesi
   regge se l'uso successivo non la contraddice (mondo allargato: contenuto,
   giudizio). «Paris, the capital» e «an LED, a light-emitting diode» producono
   la stessa ipotesi di coreferenza; sono i contatti successivi, non uno schema, a
   dire che nel secondo caso è una sigla.
3. **La riparazione come revisione, non come comando.** «No, I mean…» non è una
   forma: è un turno che contraddice una lettura appena pubblicata. Se ogni
   lettura lascia la sua ricevuta (L2, §7.1), la riparazione è ciò che resta
   dopo il confronto fra la ricevuta e il turno nuovo, senza che nessuno scriva
   quale parola la introduca.
4. **Gli strumenti del contatto si imparano per contatto.** Se il sapere su
   «come si legge un inciso» è conoscenza ordinaria, anche la prima volta che
   parrot0 incontra un nuovo strumento («that is,») è un contatto: l'ipotesi è
   che faccia ciò che fa l'apposizione, perché compare nella stessa posizione con
   la stessa coreferenza. È la domanda del §10.4 presa sul serio: gli schemi
   come *risultato* di molti contatti, non come ingresso.

### Come si riconosce il passo falso nel diff

Prima di dichiarare L3 un'iterazione si chiede:

- il diff aggiunge una `turn_form` o un lettore dedicato **per uno strumento del
  contatto**? Allora è L2+;
- uno **strumento nuovo**, mai visto (le parentesi, «that is,»), funziona senza
  toccare niente? Se serve una riga per lui, la capacità è uno schema;
- il **significato** dello strumento si può correggere parlando («no, qui è un
  inciso»)? Se no, c'è un residuo metalinguistico: lo si **elenca**, non lo si
  tace.

---

## 2. Il finto spostamento: KB muta

### 2.1 Il fenomeno

Discutendo la discesa dei «primitivi» dal C alla KB è emerso un rischio: se il C
diventa un interprete di fatti KB che codificano esattamente il C di prima
(«esegui lo `strcmp` che la KB ti dice»), il bilancio del mantra #18 migliora e
nessuno può insegnare niente di nuovo. Portato fino in fondo è l'**effetto
piattaforma interna**: un secondo motore scritto in `.p0`, perfettamente
collocato e altrettanto chiuso.

Le due prove che il progetto usa oggi, **collocazione** (`.p0` e non `.c`) e
**bilancio** (il C si accorcia), non lo vedono. Guardano entrambe *dove* sta la
conoscenza. Nessuna guarda la **distanza fra la conoscenza e una frase che la
cambi**.

### 2.2 Il caso concreto: le regole aggiunte il 24 settembre

Ogni regola scritta nel lotto supera le prove di collocazione. Contro la prova
parlata:

| regola aggiunta | che cosa decide | una frase la cambia? |
|---|---|---|
| `relation_named_by/2` (RI-020) | che «boils at» nomini `boils_at` | ❌ |
| `surface_has_content_word/1` (RI-023) | che «is a» non nomini una relazione | ❌ |
| `construction_reading/2`, verso inverso (RI-020) | che la copula si legga nei due versi | ❌ |
| `dialogue_excluded/1` sulla forza (RI-021) | che una domanda dichiarata non apra un'attività | ❌ (la *forza* si insegna, il collegamento no) |
| `faculty_yields_after_forms/1` (RI-023) | quando `knowledge` decide di cedere | ❌ |
| `alias_needs_capitals/1` (RI-023) | che «led» minuscolo non sia la sigla | ❌ e qui fa male: in chat si scrive «led» intendendo la sigla, e nessuno può dirglielo |
| `teach_contraction_en`, `forget_abbrev` | nuovi schemi di lezione | ✅ ma solo perché *sono* schemi |

Le lezioni erano vere, il comportamento è cresciuto, i trasferimenti hanno
retto. Ma **ogni iterazione ha lasciato dietro di sé una fascia di KB che solo
un ingegnere può cambiare**. È la stessa cosa che il mantra #19a ha trovato
nelle 1245 classi `*_lex*` a un membro, un piano più su.

### 2.3 Motore contro decisione

Una regola di macchinario può essere due cose diverse:

- un **motore**: dice *come* si combinano conoscenze che si insegnano. Può
  restare macchinario;
- una **decisione**: dice *quale* conoscenza vale. Compilarla in una regola è lo
  stesso difetto di una lista di parole nel C (mantra #2), un piano più su.

Esempio. `alias_needs_capitals/1` oggi è una decisione («le sigle che sono
anche forme verbali chiedono la maiuscola»). Fatta bene sarebbe:

- un motore minimo: *se la KB dice che una sigla vale solo in maiuscolo, la
  canonizzazione guarda la maiuscola*;
- una classe insegnabile: «LED counts only in capitals» / «forget that…»;
- al più un default derivato dalle forme verbali, ritirabile parlando.

### 2.4 La prova, resa operativa

Per ogni predicato che il C interroga, o che una regola di macchinario usa come
condizione:

1. esiste una frase che lo **scrive**?
2. esiste una frase che lo **toglie**?
3. il suo nome è **pronunciabile** da chi insegna?
4. un membro nuovo cambia il comportamento **dal turno dopo**?

Se la (1) fallisce, il pezzo è **KB muta**. La frazione di KB raggiungibile
parlando si può calcolare (nomi dei predicati interrogati dal C e condizioni
delle regole `machinery`, incrociati con i bersagli degli atti di forma e dei
lettori che imparano). È un indicatore più onesto del bilancio del C e del
«learning-capability 60–65», che non è calibrato.

---

## 3. Il principio adatto-linguistico

### 3.1 Enunciato (F., 24 settembre 2026)

> L'addestrabilità da prosa naturale e umana è l'antidoto. Se riusciamo a
> insegnare come si insegna a un umano, senza conoscere i processi di inferenza
> interni, vuol dire che il grado di costituzione dei processi permette
> insegnabilità e trasferimento. Il protocollo del linguaggio naturale porta con
> sé un elemento che non abbiamo ancora isolato e che garantisce
> l'insegnabilità: lo chiamiamo **adatto-linguistico**. È un concetto che fonde
> il metalinguistico e il linguistico: non c'è una distinzione in fasi, c'è un
> **adattamento per contatto cognitivo**.

### 3.2 Che cosa vuol dire, in pratica

Quando si insegna a una persona non si passa a un registro speciale. Si usa la
lingua, e la lezione è già dentro l'uso: la correzione, l'esempio,
l'apposizione, il «cioè», il contrasto. Chi impara non entra in una fase
distinta dalla comprensione: **capire e adattarsi sono lo stesso atto**. Il
metalinguistico non è un livello sopra il linguistico: viaggia dentro l'uso.

La conseguenza, scritta come criterio:

> **Una capacità è costituita bene quando può crescere per contatto con la
> lingua ordinaria.** Non basta che esista una frase speciale che la modifica.

### 3.3 Le lezioni di oggi, dette come le direbbe una persona

| schema di lezione (oggi) | come lo direbbe una persona |
|---|---|
| `"i'd like" is a contraction of "i would like"` | «I'd like to know — I mean, I would like to know — the melting point of tin.» |
| `LED is short for light-emitting diode` | «An LED, a light-emitting diode, emits light.» |
| `the boiling point of x is y means x boils at y` | «Water boils at 100 degrees Celsius, so its boiling point is 100 degrees Celsius.» |
| `a turn that contains "i would like to know" is a question` | «No, I was asking you.», detto dopo la risposta sbagliata |
| `end the previous noun phrase before opens` (L2) | «No — the *valve* opens, not the relief valve opens.» |

Nella colonna di destra la lezione è **incastonata nell'uso**, e l'uso la
**verifica** nello stesso momento: «so its boiling point is 100» afferma il
ponte e ne mostra un'istanza vera che chi impara può controllare su ciò che sa.

### 3.4 Perché è l'antidoto alla KB muta

Le regole mute del §2.2 sono mute perché stanno in uno **strato separato** dalla
conoscenza che si apprende: sapere *come leggere* sta nel macchinario, sapere
*che cosa è vero* sta nei fatti. Se il sapere su come leggere fosse conoscenza
ordinaria, appresa dallo stesso contatto e nello stesso modo del sapere sul
mondo, lo strato muto non avrebbe dove formarsi. La fusione di metalinguistico
e linguistico è anche la fusione dei due strati.

---

## 4. Candidati per l'elemento adatto-linguistico

Non sappiamo ancora che cosa sia. Questi sono i tratti che la lingua ordinaria
porta con sé e gli schemi di lezione no. Sono ipotesi da isolare con
l'esperimento del §7, non componenti da costruire.

1. **Ridondanza verificante.** La stessa informazione è insieme detta e usata
   («so its boiling point is 100»). Chi impara controlla l'ipotesi nell'atto
   stesso di riceverla.
2. **Terreno comune.** Si insegna agganciandosi a ciò che l'altro sa già («like a
   transformer, but…»). La lezione è un *delta* rispetto alla conoscenza
   dell'altro, non un'assegnazione assoluta.
3. **Riparazione.** «No, I mean…», «I was asking», l'autocorrezione. Il segnale
   di addestramento è la *reazione* al proprio errore, e arriva quando serve.
4. **Ipotesi rivedibile.** Per contatto non si «tiene» subito: si accumula
   evidenza da più contatti e ci si può ricredere. Gli schemi asseriscono in un
   colpo solo.
5. **Nessun cambio di modo.** Nessun turno è marcato come lezione; ogni turno
   *può* esserlo, e lo diventa se la lettura produce una corrispondenza nuova
   fra il noto e il nuovo.

Un indizio che almeno uno di questi c'è già, a pezzi, nel motore: la riparazione
(`correction_peel`, «no, the cat is not grey»), l'apposizione come lettura di
classe («metals such as copper, tin and lead», gen405), la prova a secco che
verifica una lettura prima di crederci (RI-018). Sono circuiti separati, ognuno
con il suo schema. L'elemento, se c'è, è ciò che li renderebbe **un circuito
solo**.

---

## 5. Due scale, da non confondere

**Livelli di ciò che si insegna** (continuazione di l2-upgrade.md):

| livello | che cosa si insegna | come |
|---|---|---|
| L1 | una proprietà di una **classe** | con uno schema |
| L2 | ciò che è vero di **questa occorrenza** | con uno schema |
| **L3** | l'una e l'altra | **per contatto**, senza schema |

**Gradi di costituzione di un pezzo di conoscenza** (la prova del §2.4):

| grado | dove sta | chi lo può cambiare |
|---|---|---|
| G0 | compilato nel C | chi ricompila |
| G1 | in KB ma **muto** | chi scrive `.p0` |
| G2 | in KB, con uno schema di lezione | chi conosce lo schema |
| G3 | in KB, raggiunto dall'uso ordinario | **chiunque parli** |

L3 è il livello in cui ciò che si insegna arriva a G3. Il lotto del 24
settembre è G2 nelle lezioni e G1 nei supporti.

**La prova del G3.** Il transcript dell'addestramento deve essere
indistinguibile da una conversazione in cui si insegna a una persona. Se una
persona che lo legge lo trova strano («x V y means x W y»), quella è una lezione
G2.

---

## 6. La discesa dei primitivi, riletta

Nella stessa conversazione era emersa una seconda tesi: non esiste un fondo
dato di «primitivi del motore» da dichiarare in anticipo. **Il fondo è ciò che
resta quando un'iterazione non riesce più a spostare niente, e si scopre
scendendo.** Anche citare, tenere, togliere e chiedere devono poter diventare
conoscenza.

Misurato il 24 settembre:

- **citare** è già a metà. In KB ci sono i delimitatori (`mention_delimiter/2`),
  le aperture di menzione (`segment_role(mention, …)`), ventotto slot `mention`.
  Nel C restano ~150 confronti con caratteri di virgolette in quattordici file.
  Non sono tutti uguali: una parte sono le virgolette **della lingua `.p0`**, cioè
  la KB che descrive sé stessa; l'altra parte sono quelle **della
  conversazione**, cioè conoscenza;
- **gli atti** sono ancora vocabolario compilato: le forme dichiarano
  `op(assert, …)`, ma il nome lo confronta il C, e gli atti sono sette (`assert`,
  `assert_neg`, `retract`, `retract_all`, `forget_each`, `match`, `count`). Il
  solver però sa già fare `assert`/`retract` dentro una regola: un atto può
  diventare una regola KB che compone i builtin.

Una scala previsionale, da smentire scendendo:

| gradino | che cosa diventa conoscenza | che cosa emerge sotto |
|---|---|---|
| 1 | l'appartenenza a una classe | gli atti che le forme nominano |
| 2 | gli atti, come regole sui builtin | il testo di una forma, interpretato dal C |
| 3 | il matcher delle forme | il tokenizzatore |
| 4 | tokenizzazione e menzione della conversazione | le virgolette della lingua `.p0` |
| 5 | ? | unificazione, risoluzione, memoria dei termini |

**Il principio del §3 cambia il criterio di questa discesa.** Un pezzo non è
sceso quando sta in `.p0` (G1). È sceso quando ha una lezione, un ritiro e un
trasferimento (G2), e il traguardo è che ci arrivi l'uso ordinario (G3).
Senza questo criterio ogni gradino produce una fascia di macchinario muto, e in
fondo alla discesa ci sarebbe un secondo motore scritto in Prolog: il finto
spostamento, fatto con molta cura.

### 6.1 Esempio: l'appartenenza a una classe, rifondata

Oggi «A transformer is a device.» la legge il **percorso rigido a quattro
parole** nel C (trace: `gate: plain «transformer is a device» deferred to the
interactive class intake`). Tre varianti della stessa idea danno tre esiti:

| detto | esito |
|---|---|
| `Transformers are devices.` | ✅ ma diventa una regola (`device(X) :- transformer(X)`), per un'altra via |
| `A buck converter is an electrical device.` | ⚠ impara `electrical_device`; «is it a device?» → I don't know |
| `A step-down transformer is a kind of transformer.` | ❌ muro |

Rifondarla per schemi (G2) vorrebbe tre lezioni:

```text
A thing can belong to a kind. If it does, what is true of every member of the kind is true of it.
"X is a Y" tells you that X belongs to the kind Y.
When a sentence tells you that a relation holds, keep it; turned into a question
it asks whether it holds; after "forget that" it takes it back.
```

Rifondarla per contatto (G3) vorrebbe invece che l'appartenenza si imparasse
da frasi come queste, senza nessuna delle tre:

```text
A buck converter is a device — an electrical one — that steps down the voltage.
Like any transformer, a step-down transformer is a device.
No, a guinea pig is not a pig.
```

La terza riga mostra che per contatto si impara anche l'**eccezione**, che gli
schemi di oggi non sanno ricevere: è il «muro di Horn» sulle generalizzazioni
rivedibili annotato nelle memorie del progetto.

---

## 6-bis. L3 nei quattro elementi, nella KB-first e nei mantra

Il piano di training ([train-the-learning-process.md](train-the-learning-process.md)
§3) mette l'addestrabilità su **quattro elementi**. Tre sono **strati**: la KB
viva (che cosa ne fa), la IR (che cosa vede in un turno), il mondo allargato (di
che cosa si può parlare). Il quarto è un **regime**: la comprensione universale
(nessun muro cieco, e ciò che manca si nomina). La regola di dipendenza del
piano è:

> una lezione può cambiare solo ciò che la KB viva sa esprimere; la KB può
> esprimere solo ciò su cui la IR le dà un appiglio; la IR può dare un appiglio
> solo a ciò che il mondo allargato ammette come genere di cosa; e nulla si
> mette in moto se la lezione non viene capita.

Con gli schemi quella regola si poteva aggirare: lo schema **è** l'appiglio, e
porta con sé la propria lettura, il proprio atto e il proprio genere di cosa.
Senza schemi non si aggira più. **L3 è il punto in cui i quattro elementi
smettono di essere quattro piani paralleli e diventano una catena sola**, ed è
per questo che sono strategici: ognuno risponde a un rischio preciso del
contatto.

### La comprensione universale: il contatto *è* la comprensione

Con gli schemi c'erano due canali: i turni che si capiscono e i turni che
insegnano. Per contatto il canale è uno: **ogni turno capito è una lezione
possibile**. Il regime cambia ruolo e diventa il canale di addestramento stesso.

- **Nessun muro cieco** diventa la condizione di esistenza di L3: un turno
  murato è un contatto perso. «I'd like to know — I mean, I would like to know —»
  deve essere *letto* (autocorrezione, poi ripresa) prima che se ne possa
  imparare qualcosa.
- **Il declino informato** diventa la metà di ritorno del contatto. Una persona
  impara anche perché l'altra le mostra che cosa ha capito («ah, quindi il LED
  è un diodo?»). È la §7.2 di l2-upgrade.md, «parrot0 dice che cosa ha capito,
  in lingua», generalizzata: senza, la riparazione del maestro non ha niente da
  riparare.
- **Le tre specie di lacuna** (universal-comprehension.md §10) diventano il
  triage del contatto: una variante di superficie si chiude dalla struttura, una
  costruzione mancante si chiude dal contatto, e una forma telegrafica si chiede.

### La IR: l'unico posto a cui un'ipotesi si può attaccare

Gli strumenti del contatto sono **pezzi di struttura**: l'apposizione («an LED, a
light-emitting diode, …»), il connettivo di conseguenza («…, so its boiling point
is …»), il marcatore di riparazione («I mean», «No, …»), il contrasto («like a
transformer, but …»). Se ciascuno viene letto da un lettore privato, ogni
strumento diventa uno schema travestito e si torna a G1 (mantra #24: un lettore
fuori dalla IR è un'esplorazione con scadenza).

Quindi, per L3:

- ogni strumento del contatto è un **nodo o un ruolo della IR**, dichiarato in KB
  come i ruoli che esistono già (`segment_role/2`, `turn_form_slot_form/3`);
- l'ipotesi che nasce dal contatto si attacca **ai nodi** (lo span apposto, i due
  lati del «so»), non alla stringa. È la condizione perché la riparazione possa
  dire *questo* (L2);
- un limite misurato il 24 settembre diventa bloccante: la IR si costruisce
  **prima** della canonizzazione, quindi «I'd like to learn» e «I would like to
  learn» producono due IR diverse (RI-022). Per contatto la forma contratta e
  quella piena devono essere lo stesso oggetto.

### Il mondo allargato: ciò che rende il contatto sicuro

Il rischio più serio del §8 è indovinare. La risposta c'è già, costruita il 20
settembre per la prosa: i cinque oggetti del mondo allargato
(`the-rational-philosopher.md` §4, `LEARN_PROTOCOL.md` §1-bis).

| oggetto | che cosa fa per L3 |
|---|---|
| **contenuto** (`kb_clause/4`) | l'ipotesi nata dal contatto si può *menzionare* senza crederla |
| **atto** (`kb_act/3`, `act_layer/2`) | «appreso per contatto» è un atto con la sua provenienza, distinto da «affermato dal maestro» |
| **contesto** (`holds_in/2`) | l'ipotesi vale dove è nata («in questa conversazione, LED sta per…») finché l'uso non la allarga |
| **giudizio** (`epistemic-status.p0`) | *ipotesi → confermata → contraddetta*: il ciclo di vita del contatto è un cambio di giudizio, non una scrittura |
| **derivazione** (`kb_derivation/4`) | se la riparazione ritira l'ipotesi, cade anche ciò che ne era derivato, e il resto no |

In questi termini la regola del §8 («ipotesi prima, fatto dopo la conferma») è
un cambio di **giudizio** su un **contenuto** entrato con un **atto** di
contatto. Non serve un meccanismo nuovo: serve che il contatto scriva in quegli
oggetti invece che nei fatti. Anche l'eccezione («No, a guinea pig is not a
pig.») diventa rappresentabile come giudizio negativo su un'istanza che una
generalizzazione copriva: il muro di Horn si aggira nel mondo allargato, non
nel solver.

E il mantra #25 dice dove sta tutto questo: in uno **strato** della mente unica
(`KB_HYPOTHETICAL` e le provenienze in lettura), mai in un secondo cervello.

### La KB viva, e che cosa diventa KB-first

Il manifesto [kb-first.md](kb-first.md) e il mantra #2 hanno misurato finora la
**collocazione**: una parola, una cue, un verbo stanno in KB e non nel C. La
scala G0–G3 del §5 non contraddice il manifesto: lo completa con la dimensione
che gli mancava, la **raggiungibilità**.

> **KB-first, in forma L3:** una conoscenza è KB-first quando sta in KB **e** la
> si raggiunge parlando. In KB e irraggiungibile è G1: è KB-first per il
> linguaggio in cui è scritta, non per chi insegna.

E c'è una conseguenza ricorsiva, che è il cuore del finto spostamento: **anche
le regole con cui parrot0 impara dal contatto devono essere KB raggiungibile**.
«Un'apposizione glossa il nome che la precede» è un fatto sulla lingua
inglese: deve stare in KB come fatto, deve poter essere corretto («no, here the
commas enclose an aside») e, idealmente, deve potersi imparare a sua volta per
contatto. Dove questa ricorsione si ferma è il fondo del §6.

### I mantra, riletti da L3

| mantra | che cosa diventa sotto L3 |
|---|---|
| **#2** niente liste nel C · **#16** ciò che dice · **#17** la condotta · **#19** le congiunzioni | tutti e quattro passano la stessa prova in più: *la cosa spostata in KB è raggiungibile parlando?* (§2.4) |
| **#7** mai una risposta sbagliata | il contatto produce ipotesi, giudicate nel mondo allargato; nessuna risposta si appoggia su un'ipotesi non confermata senza dirlo |
| **#18** spostare non è portare | si estende: *portare in KB non è rendere insegnabile*. Il bilancio del C resta necessario, non è più sufficiente |
| **#20** la KB crescerà | per contatto crescono anche le ipotesi aperte: il costo si misura per turno, le guardie si chiedono al momento dell'uso (lezione pagata in RI-023: +270 ms) |
| **#22** massimizzare e declinare | il contatto è il modo in cui i casi arrivano gratis: il maestro non deve più tradurre ogni caso in uno schema |
| **#23** la dimensione che manca alla KB | i ponti del contatto («so its boiling point is…») sono conoscenza sulla conoscenza: specie e verso della relazione, non regole di lettura |
| **#24** un lettore fuori dalla IR è un'esplorazione | ogni strumento del contatto è un ruolo della IR, o è debito con scadenza |
| **#25** una mente sola | le ipotesi del contatto sono uno strato, non un sandbox |
| **#26** vale di più ciò che amplia l'insegnabile | L3 ne è la forma più forte: non amplia *che cosa* si insegna, amplia *chi* può insegnare, cioè chiunque parli |

Un candidato a mantra, da discutere prima di scriverlo in MANTRA.md:

> **Una lezione che una persona non darebbe a una persona è uno schema, e uno
> schema è debito.** Si ammette come ripiego dichiarato, con la sua versione per
> contatto provata prima e registrata anche quando fallisce.

---

## 7. L'esperimento che isola l'elemento

Piccolo e falsificabile. Nessuna modifica al codice: solo misura.

**Protocollo.** Rifare tre lezioni del lotto del 24 settembre **solo per
contatto**, con le frasi della colonna destra del §3.3, in sessioni pulite sulla
KB completa, senza nessuno schema di lezione:

```text
An LED, a light-emitting diode, emits light.
  → What does LED stand for?
  → What does a light-emitting diode emit?

Water boils at 100 degrees Celsius, so its boiling point is 100 degrees Celsius.
  → What is the boiling point of ethanol?      (il trasferimento è l'adattamento)

I'd like to know — I mean, I would like to know — the melting point of tin.
  → I'd like to know the working load of an M10 eye bolt.
```

**Lettura dei risultati.** Ogni fallimento si traccia con il trace unico e si
attribuisce **due volte**:

- all'**elemento** che si è fermato, nell'ordine della regola di dipendenza del
  §6-bis: la comprensione universale (il turno è stato letto o murato?), la IR
  (lo strumento del contatto è un nodo, o l'ha preso un lettore privato?), il
  mondo allargato (l'ipotesi aveva un posto come contenuto con atto e giudizio?),
  la KB viva (qualcuno consuma ciò che è stato scritto?);
- al **candidato** del §4 che mancava (o a un sesto, da nominare).
L'elemento è ciò che, aggiunto al motore **come capacità generale e non come
schema**, fa passare le tre insieme.

**Criterio di successo.** Il transcript delle tre sessioni supera la prova del
G3: una persona che lo legge non riconosce che si stava addestrando una macchina.

**Il §7 si può superare col passo falso.** Uno schema per l'apposizione, uno
per il «so», uno per «I mean» farebbero passare le tre lezioni (§1-bis). Perciò
un esito verde conta come L3 solo se:

- nessuna forma o lettore nuovo è stato scritto per uno strumento del contatto;
- un quarto strumento **mai previsto**, provato dopo le tre lezioni, funziona
  allo stesso modo:
  ```text
  A PWM (that is, pulse-width modulation) signal switches very fast.
    → What does PWM stand for?
  ```
- il residuo metalinguistico del meccanismo usato è elencato nella scheda.

Altrimenti l'esito si registra come L2+, e l'esplorazione del §1-bis continua.

**Contrasto obbligatorio.** Per contatto si impara anche il falso: «An LED, a
kind of lamp, emits light» non deve far tenere un'identità falsa senza
evidenza. Ogni ipotesi nata dal contatto ha provenienza, si verifica
nell'uso successivo e si ritira con la riparazione.

---

## 8. Rischi

1. **Indovinare.** Adattarsi per contatto vuol dire anche sbagliare. Una persona
   sbaglia e si ricrede; parrot0 non deve mai pagare l'adattamento con una
   risposta falsa (mantra #7). Il contatto produce **ipotesi**, non fatti; un
   fatto nasce quando l'uso le conferma.
2. **Costo.** Più letture per turno, più ipotesi aperte. Il mantra #20 vale: si
   misura col profilo per turno di `/debug`, mai col cronometro del soft-test.
   Nel lotto del 24 settembre una sola `naf` nel posto sbagliato costava 270 ms
   per turno.
3. **Il nome che non spiega.** «Adatto-linguistico» è un nome per ciò che non
   abbiamo ancora isolato. Il rischio è usarlo come spiegazione invece che come
   domanda. Finché l'esperimento del §7 non lo attribuisce a un meccanismo, è
   un'ipotesi.
4. **Gli schemi non spariscono di colpo.** L3 non abolisce L1 e L2: li rende
   *derivati*. Uno schema diventa una generalizzazione che parrot0 ricava da
   molti contatti, non una porta di ingresso scritta a mano. Nel frattempo gli
   schemi restano, e servono a costruire il corpus dei contatti.
5. **Il fondo.** La discesa del §6 troverà qualcosa che non scende. Va trovato,
   non dichiarato.

---

## 9. Che cosa cambia nel metodo, da subito

Anche prima di qualunque implementazione:

- **l'ordine del §1-bis**: prima le possibilità evolutive, poi, solo se non si
  trova nulla, gli schemi naturali, registrati come L2+ con il loro residuo
  metalinguistico elencato. Mai il contrario;
- **nel ciclo delle iterazioni di riferimento** (R2 del piano di training): per
  ogni lezione si scrive anche la **versione per contatto**, e la si prova per
  prima. Se fallisce, il fallimento si registra; lo schema si usa solo dopo,
  come ripiego dichiarato;
- **per ogni regola di macchinario nuova**: nella scheda si risponde alla prova
  del §2.4. Se è una *decisione* e non un *motore*, si scrive la sua maniglia
  parlata nella stessa iterazione, o la si registra come debito G1;
- **nel resoconto del lotto**: accanto a `W/L/C/P/O/X`, il grado G0–G3 di ogni
  supporto aggiunto;
- **nella diagnosi**: prima di aprire il codice si dice a quale dei quattro
  elementi appartiene il buco (§6-bis), come chiede già il piano di training.
  Sotto L3 la domanda ha un secondo tempo: *la cura rende quell'elemento
  raggiungibile per contatto, o aggiunge uno schema?*

---

## 10. Domande aperte

1. L'elemento adatto-linguistico è **uno**, o sono i cinque candidati del §4 che
   lavorano insieme?
2. La riparazione («no, I mean…») è il caso più semplice del contatto, o il più
   difficile? È quello che più assomiglia a L2 (occorrenza), senza schema.
3. Quanta KB è oggi G1? Il calcolo del §2.4 dà il primo numero vero.
4. Se uno schema può essere derivato da molti contatti, quanti contatti servono,
   e chi decide che sono abbastanza?

---

## 11. Parere dell'agente, richiesto da F.

F. chiede: *«secondo me questa volta ci siamo, perché stiamo superando il
problema degli schemi di apprendimento, che era il limite di L2. Che ne pensi?»*

Penso che **la domanda sia quella giusta**, e che lo sia per una ragione
misurata, non estetica. Il lotto del 24 settembre è andato bene secondo ogni
criterio del piano, eppure ha lasciato sette schemi nuovi e sei regole mute.
L'iterazione di riferimento, per come è definita oggi, **produce** schemi: non
è un difetto dell'esecuzione, è la forma del metodo. Un metodo che, quando
funziona, genera il proprio limite va cambiato al livello del criterio, ed è
quello che il principio del §3 fa.

Sul «ci siamo» sono più cauto, per tre motivi:

- **Abbiamo la domanda, non ancora la risposta.** L'elemento non è isolato. Il
  §7 serve a sapere se è uno, se sono cinque, o se c'è un sesto che non vediamo.
- **L3 non si costruisce contro L2, ci si arriva attraverso L2.** Le ricevute di
  lettura, la rilettura nello stesso turno e la portata dichiarata di
  l2-upgrade.md sono esattamente ciò che serve perché «No — the valve opens» abbia
  un *questo* a cui attaccarsi. Il contatto senza ricevute sarebbe indovinare.
- **Il rischio più serio è la risposta falsa.** Un sistema che si adatta per
  contatto e non ha un'etica della prova diventa convincente e sbagliato. Il
  principio va accoppiato, fin dal primo esperimento, alla regola «ipotesi prima,
  fatto dopo la conferma».

Una cosa mi fa pensare che la direzione sia davvero strategica e non soltanto
elegante: **L3 non chiede pezzi nuovi, chiede che quelli esistenti si
accordino.** La comprensione universale, la IR, il mondo allargato e la KB viva
sono stati costruiti in piani diversi, e finora ogni schema di lezione li
scavalcava portandosi dietro la sua lettura, il suo atto e il suo genere di
cosa. Senza schemi devono parlarsi. È la forma ricorrente dei difetti di questo
repository (D33/D35/D37: «due percorsi che devono accordarsi e non condividono
l'oggetto su cui accordarsi») presa come obiettivo invece che come incidente.

Se il §7 riesce anche solo su una delle tre lezioni, cioè se parrot0 impara una
sigla da un'apposizione senza nessuno schema e la usa su un caso nuovo, allora
sì, credo che sia il passaggio che cambia la natura del progetto: da un sistema
a cui si insegna con un manuale d'uso a uno a cui si insegna parlando.
