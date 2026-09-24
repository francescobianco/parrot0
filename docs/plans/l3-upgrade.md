# L3 — insegnare a parrot0 per contatto, senza schemi di lezione

**Piano di indirizzo e progettazione operativa, 24 settembre 2026.
Stato: analisi in corso; L3 non implementato.**
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

## Handoff vivo — leggere prima di riprendere

**Mandato:** trasformare l'ipotesi L3 in un piano di meccanismi implementabili,
verificato contro il repository. Questa sessione modifica il piano; non dichiara
implementato il circuito e non aggiunge schemi di contatto al motore.

**Checkpoint 1 — impostazione.** Letti MANTRA, PRINCIPLES e il piano completo.
La distinzione da mantenere è fra strutture già presenti, garanzie effettivamente
implementate e meccanismi ancora da costruire. In particolare il §6-bis attribuisce
alla derivazione il ritiro delle conseguenze: va verificato nel codice, non
assunto dal nome del predicato.

**Ipotesi di lavoro:** L3 apprende una modifica rivedibile della lettura
confrontando episodi, alternative e conseguenze. Il segnale utile è una
discrepanza verificabile; la presenza di virgole, «so» o «I mean» da sola non
autorizza alcun significato. Occorre distinguere comprensione dell'occorrenza,
generalizzazione e promozione, evitando che un'ipotesi si confermi da sola.

**Checkpoint 2 — audit del codice.** `kb_induce` propone inclusioni fra
predicati unari e scarta `machinery`; `mod_induce` apprende trasformazioni
numeriche, `mod_fewshot` e `mod_archetype` richiedono esempi segmentati. Non
sono già un induttore di letture dei turni. `kb_clause` riflette clausole
presenti, `kb_act` distingue bit di provenienza, `kb_derivation` espone prove
con identità di sessione: nessuno dei tre da solo offre un archivio di
ipotesi inerti con sostegni persistenti. L2 ritira i fatti della frase tramite
`reading_stale_clause`; non è ancora una manutenzione generale delle dipendenze.

**Prossimo passo:** completare l'audit della prova senza effetti e del percorso
IR, misurare il §7 sulla KB completa se il binario è utilizzabile, poi scrivere
il contratto operativo (§12 e seguenti). Prima unità candidata: indurre un
allineamento fra due letture parziali, conservare alternative e verificarne
una conseguenza indipendente. Non partire da un riconoscitore di apposizioni.

**Checkpoint 3 — due ostacoli aggiuntivi.** La prova di RI-018 è una lettura
interrogativa (`query_only`), non una transazione generale. Le funzioni
`p0_try_reading` e `p0_dry_read_journal` usano invece `fork()`: conservano la KB
iniziale, ma il lavoro della copia non è uno strato interrogabile nella mente
unica. Non sono l'esecutore da moltiplicare per L3 (mantra #25). Inoltre LED,
la sua espansione e il fatto che emette luce sono già salvati nella KB viva:
un verde su LED nel §7 non misura nuovo apprendimento. Serve il controllo
prima/dopo, con ablazione mirata della lezione o un caso realmente non appreso.

**Checkpoint 5 — I0 chiuso (§17).** Baseline misurata sulla KB completa con
ritiro mirato in memoria. Il contatto oggi **non arriva** al lettore: tre furti
(il ramo compilato gen241 risponde a una dichiarazione; la forma `year_stated`
batte il frame `born_in` corretto e scrive «einstein dates from ulm so his
birthplace is ulm»; il punto e virgola va al rilevatore di codice). Residui IR:
nessun nodo fra le proposizioni attorno a «so», nessuna quantità per «100
degrees Celsius», nessun sintagma «its boiling point». Relazione di controllo
fissata prima della cura: `born_in` ↔ «birthplace». **Prossimo passo:** il
gradino prima di I1 (§17.4), cioè togliere i tre furti retrocedendo i lettori
immaturi, e dare voce al ramo muto nel trace.

**Checkpoint 4 — contratto concettuale e sonda.** Scritti §12 (circuito,
ambiguità, bootstrap) e §13 (audit con simboli del codice); corrette le garanzie
premature del §6-bis. Nel binario disponibile tutte e tre le domande di
trasferimento note rispondono già prima del contatto. PWM resta ignoto anche
dopo la frase con parentesi: il trace mostra una lettura del frammento
«pulse-width modulation) signal switches very fast», non un ponte con PWM.
Dettagli riproducibili da raccogliere al §17. Ora: rappresentazione delle
ipotesi, prova nella mente unica, criteri di consolidamento e incrementi con
criterio di arresto. Non è stato modificato il motore né salvata la sessione.

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
- uno **strumento nuovo**, mai visto (le parentesi, «that is,»), si apprende senza
  una patch dedicata? Una costruzione generata dal contatto è un risultato
  ammissibile; una riga aggiunta dall'ingegnere per quel marcatore è L2+.
  Il primo incontro può richiedere chiarimento: novità non significa evidenza
  sufficiente (§12.4);
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
  quella piena devono poter essere allineate alla stessa lettura, mantenendo
  entrambe le superfici e il collegamento fra i loro nodi. Non si cancella
  l'originale per rendere identiche due IR (§14.1).

### Il mondo allargato: ciò che rende il contatto sicuro

Il rischio più serio del §8 è indovinare. La risposta c'è già, costruita il 20
settembre per la prosa: i cinque oggetti del mondo allargato
(`the-rational-philosopher.md` §4, `LEARN_PROTOCOL.md` §1-bis).

| oggetto | che cosa fa per L3 |
|---|---|
| **contenuto** (`kb_clause/4`, da estendere ai candidati inerti) | l'ipotesi nata dal contatto si deve poter *menzionare* senza crederla |
| **atto** (`kb_act/3`, `act_layer/2`, da integrare con episodi distinti) | «appreso per contatto» deve avere la sua provenienza, distinta da «affermato dal maestro» |
| **contesto** (`holds_in/2`) | l'ipotesi vale dove è nata («in questa conversazione, LED sta per…») finché l'uso non la allarga |
| **giudizio** (`epistemic-status.p0`, da integrare) | distinguere sostegno, negazione, conflitto e ricerca incompleta; aggiungere il ciclo delle ipotesi |
| **derivazione** (`kb_derivation/4`, da integrare con manutenzione dei sostegni) | ottenere il ritiro delle conseguenze che hanno perso tutti i sostegni, conservando le altre |

In questi termini la regola del §8 («ipotesi prima, fatto dopo la conferma») è
un cambio di **giudizio** su un **contenuto** entrato con un **atto** di
contatto. Questi oggetti danno il vocabolario del progetto, **non garantiscono
già il ciclo**: l'audit del §13 distingue ciò che c'è dai meccanismi mancanti.
Anche l'eccezione («No, a guinea pig is not a
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

**Revisione dopo la sonda del §17:** LED, il ponte «boiling point» e la
contrazione sono già nella KB salvata. Le domande sotto sono quindi anche
controlli di capacità preesistenti. Prima di attribuire un verde al contatto,
misurare la risposta iniziale e la dipendenza dalla lezione; per l'acquisizione
seguire il setup mirato del §15.1. Non usare una KB ridotta.

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
- un quarto strumento **non usato nell'acquisizione**, provato dopo le tre
  lezioni, entra nello stesso circuito senza una patch dedicata. La diagnosi
  distingue struttura non vista ed espressione non ancora imparata; può servire
  ulteriore contatto quando il significato non è determinato:
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
elegante: **L3 chiede che i pezzi esistenti si accordino.** L'audit successivo
(§13) corregge la prima ipotesi: servono anche meccanismi nuovi per allineare,
provare e mantenere i sostegni; non basta collegare porte già complete.
La comprensione universale, la IR, il mondo allargato e la KB viva
sono stati costruiti in piani diversi, e finora ogni schema di lezione li
scavalcava portandosi dietro la sua lettura, il suo atto e il suo genere di
cosa. Senza schemi devono parlarsi. È la forma ricorrente dei difetti di questo
repository (D33/D35/D37: «due percorsi che devono accordarsi e non condividono
l'oggetto su cui accordarsi») presa come obiettivo invece che come incidente.

Se il §7 riesce anche solo su una delle tre lezioni, cioè se parrot0 impara una
sigla da un'apposizione senza nessuno schema e la usa su un caso nuovo, allora
sì, credo che sia il passaggio che cambia la natura del progetto: da un sistema
a cui si insegna con un manuale d'uso a uno a cui si insegna parlando.

---

## 12. Ipotesi operativa: imparare una lettura attraverso le sue conseguenze

**Progettazione del 24 settembre, successiva ai §§0–11. Non implementata.**
Le sezioni seguenti restringono l'ipotesi a un circuito costruibile e
falsificabile. Non pretendono di identificare il meccanismo interno di un LLM.

### 12.1 L'oggetto che L3 impara

L3 propone una **modifica della lettura**, con portata e giustificazioni:
«in questo contesto questo pezzo del discorso occupa questo ruolo». Ne osserva
le conseguenze su altri usi e conserva la modifica soltanto nella portata
sostenuta dall'evidenza. Una costruzione o uno schema può esserne il risultato;
il maestro non deve fornirlo e il programmatore non deve scriverlo per il caso.

L'unità minima non è dunque una coppia di stringhe sinonime. È la relazione fra:

- un **episodio osservato**: testo originale, interlocutore, contesto, nodi IR;
- le **letture concorrenti**, comprese quella corrente e «non determinato»;
- un **delta**: quali legami, ruoli o condizioni cambierebbero;
- una **conseguenza discriminante**: che cosa dovrebbe risultare diverso se
  quel delta fosse giusto;
- l'**evidenza indipendente** che sostiene o contraddice quella conseguenza.

Questo rende concreto il principio adatto-linguistico: il contatto fornisce
vincoli su una lettura, non un comando di scrittura. L3 è il circuito che
trasforma quei vincoli in un adattamento controllato. La sua ipotesi centrale
è che allineamento, revisione e trasferimento possano condividere quel circuito.

### 12.2 Il salto che non si può ottenere gratuitamente

Una coincidenza non determina il suo significato. Due relazioni con lo stesso
valore su un oggetto possono essere diverse; due sintagmi riferiti alla stessa
cosa possono avere sensi diversi. In particolare:

| contatto | autorizza a proporre | non dimostra da solo |
|---|---|---|
| «An LED, a light-emitting diode, …» | coreferenza locale, appartenenza a classe, espansione lessicale come alternative | che ogni apposizione sia una sigla o che i due termini siano sinonimi globali |
| «Paris, the capital, …» | un referente con una descrizione contestuale | `Paris = capital` nel lessico universale |
| due frasi su acqua e 100 °C | un possibile allineamento di soggetto, valore e relazione | equivalenza generale delle relazioni, condizioni di pressione comprese |
| «No, I was asking you» | revisione dell'atto attribuito al turno precedente | che ogni turno con quelle parole sia sempre interrogativo |

**Conferma dell'istanza, induzione della regola e autorizzazione a usarla sono
tre giudizi distinti.** Ripetere la stessa frase dieci volte non produce dieci
prove indipendenti. Un numero finito di esempi non rende una generalizzazione
una verità deduttiva: resta rivedibile, con fonte e portata.

### 12.3 Il circuito, con ingressi e uscite

1. **Osservare senza perdere.** Pubblicare nella IR ciò che si riconosce e ciò
   che resta irrisolto. Archiviare le scelte effettive prima di correggerle.
2. **Trovare un disaccordo o una ridondanza.** Confrontare un ruolo irrisolto
   con una lettura nota, oppure una lettura pubblicata con una correzione.
   Una forma sconosciuta può attivare ricerca senza che il sistema abbia già
   capito che si tratta di una lezione.
3. **Allineare ancore.** Cercare legami fra nodi usando referenti, ruoli,
   quantità con unità, contesti e posizioni. La sola somiglianza delle stringhe
   è un'evidenza debole. Conservare i possibili allineamenti concorrenti.
4. **Proporre il delta minimo.** Riempire un ruolo, rivedere un legame, oppure
   astrarre una corrispondenza già sostenuta. La proposta è un contenuto inerte.
5. **Confrontare le conseguenze.** Valutare la lettura corrente e le alternative
   sulla stessa KB completa. Distinguere conferma, smentita, assenza di dati e
   ricerca interrotta. Una risposta non vuota non è una prova di correttezza.
6. **Usare, chiedere o sospendere.** Se resta una distinzione rilevante,
   cercare un'osservazione che separi le alternative o chiedere in lingua
   ordinaria. Se non c'è evidenza sufficiente, tenere aperto il candidato.
7. **Consolidare e revisionare.** Rendere disponibile la lettura nella portata
   guadagnata, registrare da che cosa dipende, invalidarla quando quei sostegni
   cambiano. L'uso successivo torna al passo 1.

Non occorre una discrepanza con la verità già nota per imparare: anche una
lettura parziale con due ancore e un ruolo mancante è un problema. Viceversa,
essere d'accordo con la KB non dimostra di aver capito il maestro: potrebbe
stare correggendo proprio un fatto della KB o descrivendo un altro contesto.
Le alternative devono includere errore di lettura, nuova informazione e cambio
di contesto; il sapere preesistente non ha un veto assoluto.

### 12.4 Bootstrap e residuo dichiarato

Non si parte da zero: la KB viva possiede già lingua, relazioni e procedure.
Si parte da una **zona capita** che dà vincoli alla zona non capita. Se mancano
entrambe le ancore non si inventa una lettura; si registra il limite o si chiede.

Il nucleo meccanico candidato è piccolo: enumerare nodi e legami, unificare,
preservare identità ripetute, sostituire costanti con variabili, cercare
alternative entro risorse finite, registrare dipendenze. Che cosa conti come
ancora, quali trasformazioni siano pertinenti, quale portata sia autorizzata e
come porre la domanda sono conoscenza KB, da rendere raggiungibile per contatto.

**Non promettiamo un apprendimento senza presupposti.** Il primo incremento
avrà grammatica ereditata e politiche iniziali: le si elenca. La prova ricorsiva
è che una correzione d'uso possa cambiare almeno una politica o condizione
appresa con lo stesso circuito. Finché questo non avviene abbiamo un primo
apprendimento per contatto, non la chiusura completa di L3.

Un marcatore nuovo non deve necessariamente essere compreso al primo incontro.
Deve poter acquistare un ruolo da contatti sufficienti senza un nuovo handler.
Se la sua interpretazione è ambigua, chiedere è un esito corretto. Ignorare
«not», un inciso o un vincolo per ottenere una lettura comoda non vale.

## 13. Audit: che cosa parrot0 implementa già e che cosa manca

Audit statico sul commit `2a8ac69e`, 24 settembre 2026. I riferimenti nominano
file e simboli per restare cercabili quando cambiano le righe. «Presente» qui
significa riscontrato nel codice; le verifiche runtime di questa sessione sono
separate al §17.

| componente | appiglio verificato | limite per L3 |
|---|---|---|
| IR comune | `src/brain/99-registry.c`: `turn_publish_tokens`, `input_structure_publish`; `kb/core/input-structure.p0` | servono alternative e residui collegati a nodi stabili fra lettura originale e canonizzata; pubblicare token non equivale a leggere l'inciso |
| ricevute e rilettura L2 | `kb/core/reading-choices.p0`: `reading_choice/4`, `reading_revision/3`, `reading_stale_clause/1`; atto `reread` in `10-memory-knowledge.c` | la revisione entra da schemi; non tutte le decisioni hanno ricevuta; parte delle ricevute del binder può appartenere a candidati poi scartati |
| contenuto strutturato | `kb/core/clause-content.p0`; `src/kb.c`: `clause_scan`, `kb_clause_arg` | riflette clausole già presenti. Non archivia da solo una regola candidata senza attivarla |
| provenienza | `kb_act/3`, `act_layer/2` | distingue gli strati; due osservazioni nello stesso strato richiedono identità di episodio ulteriori |
| contesti | `kb/core/context-scope.p0`: `holds_in`, `context_visible_belief`, `supersedes_in` | sono proposizioni reificate con consumatori espliciti; non rendono automaticamente contestuale ogni lookup del motore |
| prove | `kb/core/derivation.p0`; `src/kb.c`: `derivation_door` | dipendenze AND e prove alternative OR presenti, anche `absent` e `aggregate`; gli ID sono in un anello di sessione. Non è un archivio persistente né un ritiro automatico transitivo |
| giudizi | `kb/core/epistemic-status.p0` | distingue sostegno, negazione, ignoto, conflitto e incompleto nelle risposte polari; non implementa il ciclo proposta/prova/promozione di una lettura |
| induzione di classi | `src/kb.c`: `kb_induce` | enumera predicati unari, esclude `machinery`, deposita `induced_candidate`; non induce corrispondenze fra grafi IR. Il commento in `src/kb.h` che parla di regole subito asserite è arretrato rispetto al corpo |
| induzione e analogia su esempi | `65-induce-verify-shell.c`: `mod_induce`; `40-meta-reflection.c`: `mod_fewshot`, `mod_archetype` | numeri o esempi con frecce/segmenti; il few-shot non conserva il risultato. Riutilizzabili alcune meccaniche di allineamento, non il percorso come L3 già pronto |
| apprendimento da esito | `kb/core/episodes.p0`: `episode_note`, `episode_verified`, `episode_contradicted` | precedente utile: conserva candidati e confronta esito/aspettativa; legato ai verdetti e alle loro forme di esito, non a ogni lettura |
| domanda discriminante | `kb/core/inquiry.p0`: `observation_splits`, `discriminating_action` | idea riusabile, ma non collegata a ipotesi linguistiche; una credenza assente non deve diventare una smentita in un mondo aperto |
| lettura preliminare | `10-memory-knowledge.c`: `p0_frame_reading`, `p0_try_extract_frames_only(query_only)` | RI-018 pubblica leggibilità interrogativa; non è un esecutore universale privo di effetti |
| prova con giornale | stesso file: `p0_try_reading`, `p0_dry_read_journal`; `kb_journal_*` | usa `fork` e `brain_respond`; il giornale registra asserzioni, non ogni effetto e il suo inverso. Non moltiplicare questo isolamento per L3 |
| punto di consumo già vivo | `kb/core/grammar.p0`: `construction_frame` → `construction_reading` → `extract_frame` | può consumare un ponte appreso; oggi lo produce una lezione esplicita. Le varianti copulari e le guardie sono conoscenza ereditata, non appresa da L3 |

**Conseguenza architetturale:** non aggiungere un `mod_l3` che rivendica frasi
con virgole. Il circuito deve osservare e migliorare la lettura comune; i
consumatori ordinari devono vedere la lettura migliorata, con il suo sostegno.

**Limiti del dialetto verificati:** `src/kb.h` dichiara attualmente
`KB_MAX_ARGS = 4`, `KB_MAX_BODY = 16`, `KB_TERM_LEN = 512`. L'8 riportato in
AGENTS è storico. Spezzare gli oggetti per identità e archi; non serializzare
interi episodi in un solo termine. Overflow e ricerca troncata devono essere
stati espliciti, mai assenza di evidenza.

## 14. Contratto minimo del circuito

### 14.1 Gli oggetti, prima dei nomi nuovi

Le firme qui sotto sono **proposte di rappresentazione**, non API esistenti né
istruzioni per il maestro. Riutilizzare contenuti, contesti ed episodi del §13;
prima di aggiungere una tabella, verificare se manca solo una loro proprietà.

| oggetto | informazione indispensabile | appiglio / estensione proposta |
|---|---|---|
| osservazione | identità distinta, autore/fonte, turno, contesto, span originale | estendere l'episodio con nodi della IR; non usare il solo bit `KB_SESSION` come identità |
| lettura | versione, scelte effettive, alternative, residui, dipendenze | `reading_choice` e archivio IR; ogni candidato ha identità propria, non sovrascrive l'ultima ricevuta |
| ipotesi | delta strutturato, bersaglio, portata, lettura di origine | possibile `contact_hypothesis(H, Episode, Delta, Scope)`, con Delta come ID se composto |
| allineamento | quali nodi corrispondono e per quali evidenze | possibile `contact_alignment(H, LeftNode, RightNode, Evidence)`; i nodi includono episodio e versione |
| verifica | previsione, esito, osservazione, dipendenze della prova | possibile `contact_check(H, Observation, Outcome, Proof)`; la prova va copiata come contenuto durevole |
| sostegno | relazione tra conclusione, prova e singolo atto osservato | AND dentro una prova, OR fra prove; deve distinguere due fonti della stessa clausola |
| decisione | uso autorizzato, contesto, politica applicata, motivo | possibile `contact_use(H, Context, Status, Reason)`; separare stato di lavoro e giudizio epistemico |

Un candidato sta come **dato su cui ragionare**, per esempio contenuto di un
contesto d'ipotesi. Non si asserisce `entity_alias` o `construction_frame` per
poterlo ispezionare. `KB_HYPOTHETICAL` da solo non basta: un'origine di scrittura
non rende automaticamente innocui i consumer che interrogano senza contesto.

Gli stati di lavoro sono proposto, in verifica, utilizzabile, sospeso, ritirato.
Gli esiti delle prove restano sostenuto, smentito, ignoto, conflittuale,
incompleto. **Ignoto e incompleto non sono smentite.** Una regola ritirata può
restare nella memoria degli episodi senza continuare a generare risposte.

### 14.2 Come nasce una proposta senza un lettore per ciascun contatto

Il primo generatore cerca **allineamenti ancorati**. Riceve due porzioni di IR
e le loro letture parziali, non una stringa da cercare con `strstr`.

1. Indicizza referenti e valori già legati, conservando ruolo, unità, tempo,
   polarità e contesto. Una quantità incastonata in una frase descrittiva non
   equivale automaticamente al testo intero del valore KB.
2. Recupera episodi pertinenti e prova corrispondenze compatibili fra nodi.
   Una stessa entità ripetuta deve mantenere la stessa corrispondenza; soggetto
   e oggetto non sono permutabili senza evidenza.
3. Dove una lettura nota e una parziale condividono ancore, propone il legame
   mancante. Include l'alternativa «coincidenza / informazioni distinte».
   Se manca la lettura nota può proporre solo ipotesi più deboli, oppure fermarsi.
4. Fra episodi risolti cerca una **struttura comune**: sostituisce i valori
   variabili preservando i legami ripetuti e i vincoli di ruolo. Questo è il
   lavoro di generalizzazione; non basta rimpiazzare due parole con `@S/@O`.
5. Registra la parte astratta e quella ancora contingente. Un solo episodio
   autorizza un candidato locale; l'ampliamento della portata richiede una
   verifica distinta.

Il catalogo delle trasformazioni deve essere interrogabile in KB. Il primo
insieme comprende legare un ruolo, riallineare un referente, astrarre un valore
e aggiungere una condizione già esprimibile; sono operazioni su strutture.
Le condizioni che le attivano sono regole KB. Una nuova combinazione o guardia
non deve richiedere un ramo C; una primitiva strutturale davvero mancante è
invece lavoro del motore, da motivare con un caso che le altre non esprimono.

La punteggiatura e i connettivi possono aiutare a proporre vicinanza e segmenti,
secondo conoscenza linguistica della KB. **Non assegnano direttamente il tipo
di adattamento.** Un nuovo separatore si apprende dagli allineamenti che
ricorrono nei contatti; la relativa costruzione è un risultato con provenienza.
Se il lettore non conserva i due lati, si estende la IR: non si aggiunge qui un
parser privato che li ricostruisce.

### 14.3 Provare nella stessa mente

La prima implementazione deve avere un perimetro limitato: **provare letture
strutturali e interrogazioni prive di effetti**, non rieseguire liberamente
`brain_respond` per ogni ipotesi. Il contesto di prova vede la KB completa più
il delta candidato pertinente; le altre ipotesi rimangono visibili come dati,
senza diventare premesse della prova corrente.

Servono tre proprietà verificabili:

- il lettore riceve il contesto esplicitamente e pubblica alternative in quel
  contesto; la proposta non entra nelle lookup globali;
- la verifica produce lettura, risposta prevista, dipendenze e completezza,
  senza asserire fatti del mondo né cambiare turno, focus, agenda o disco;
- una scrittura tentata durante la verifica restituisce «effetto non ammesso
  nella prova», non fallimento logico e non un effetto da sperare di annullare.

`p0_frame_reading` è un punto di ingresso da adattare, non già una garanzia:
si ferma alla prima lettura valida e il binder può scrivere ricevute. Occorre
separare enumerazione di candidati, scelta e deposito. Le cache devono includere
il contesto del delta o derivare senza materializzare globalmente la prova.

Il giornale delle asserzioni attuale è utile come osservabilità, **non come
rollback**: non annulla ritrattazioni, stato C, cache o effetti esterni. Se un
incremento futuro richiede effetti simulati, servirà un diario completo e
annullabile nella stessa mente. Non è prerequisito del primo incremento, che
deve restare nella lettura senza effetti. Nessun nuovo `Brain`, `kb_create`
vuoto o `fork` per pensare le alternative.

### 14.4 Quando una prova vale

Una verifica è indipendente da H se la sua osservazione e la sua interpretazione
non dipendono da H, dai suoi discendenti o da una risposta prodotta usando H.
La prova deve portare questa dipendenza, non un flag assegnato a intuito.
Può provenire dalla parte già capita dello stesso contatto o da un uso
successivo; provenire da un turno diverso, da solo, non basta.

Esempio: H legge «boiling point» come `boils_at`. Rispondere correttamente
usando H mostra che il consumer funziona; **non conferma H**. Una seconda
descrizione capita per una via indipendente e incompatibile con le alternative
può sostenerla. Un atteso scritto nel test verifica l'agente, ma non è evidenza
disponibile a parrot0 finché il dialogo non gliela dà.

La decisione deve rendere visibile un vettore di evidenze: ruoli spiegati,
residui, sostegni indipendenti, contraddizioni motivate, portata proposta e costo
del delta. Il candidato «nessun cambiamento» partecipa sempre. Semplicità e
copertura servono a ordinare la ricerca, non certificano la verità. Nessuna
somma di punti positivi cancella in silenzio un controesempio pertinente.

Per il primo banco, il **cancello conservativo proposto** per ampliare la
portata richiede: un episodio allineato, una verifica su un episodio diverso
con lettura indipendente, esito discriminante rispetto ai concorrenti noti,
assenza di conflitti irrisolti e replay dei contrasti pertinente. È una politica
iniziale da misurare, non «due esempi dimostrano una legge». Se le alternative
restano indistinguibili si chiede o si sospende. Numeri e priorità stanno in KB;
finché non sono correggibili per contatto sono residuo G1/G2 dichiarato.

La resa dipende dal giudizio: una lettura locale sostenuta può essere usata
come tale; una generalizzazione incerta non produce un'affermazione assoluta.
Per le domande usare template/composizione KB con alternative comprensibili,
ad esempio «Qui parli della stessa cosa o di due cose diverse?». Non chiedere
al maestro quale predicato o operatore attivare. Il feedback si lega alla
questione aperta e ai suoi nodi, non alla presenza isolata di «yes» o «no».

### 14.5 Consolidare significa collegare a un consumer

Un candidato accettato deve produrre un effetto nel percorso ordinario.
Per il primo ponte il consumer è `construction_reading` / `extract_frame`,
con il binding di soggetto e valore già in uso. La proposta può essere resa
nel formato che quel percorso comprende, conservando il collegamento a H.
Questa compilazione è una **cache di conoscenza appresa**, non la prova che
il sistema abbia appreso: il test deve ricostruirla dai contatti.

La portata deve restare nel consumer. Non materializzare un
`construction_frame` globale per una conclusione valida soltanto nel contesto C.
Il primo adattatore deve interrogare una vista di costruzioni ammesse nel
contesto del turno; il ramo già esistente continua a fornire le costruzioni
apprese esplicitamente. Guardie, criteri di uso e dipendenze sono KB.

Ritrattare H deve togliere la costruzione da quella vista, invalidare le cache
interessate e impedire nuove deduzioni che la usano. I fatti già ricavati
richiedono inoltre manutenzione dei sostegni (§14.6). Salvare una stringa o
rispondere «imparato» senza questa catena non conta come L3.

### 14.6 Ritiro e persistenza fanno parte del significato

Per una conclusione C mantenere le prove P1…Pn, ciascuna con le dipendenze
D1…Dm. Una prova vale se tutte le sue dipendenze sono valide; C resta
utilizzabile finché esiste almeno una prova ammissibile o un atto diretto
ancora valido. Togliere una fonte non deve cancellare l'altra, anche se
entrambe entrarono in `KB_SESSION`.

I cambiamenti comprendono ritiro di episodi, correzione di lettura, nuova
negazione, mutamento di contesto e cambiamento di regole. Una prova con
`absent(G)` scade anche quando G **viene aggiunto**; una con `aggregate(G)`
scade se cambia l'insieme pertinente. Un ciclo H1→H2→H1 senza un sostegno
esterno non sostiene nessuno dei due. Sono requisiti del manutentore, non
proprietà già garantite da `kb_derivation`.

Partire dalle conclusioni prodotte dal circuito L3 e dalle loro dipendenze
note, dichiarando il perimetro. Non promettere retroattivamente una provenance
completa per tutta la KB storica. Una prova incompleta blocca la promozione.
Per i fatti base o sostenuti da altre fonti, sospendere il sostegno L3 e
conservare il resto; non usare `retract` indiscriminato sul contenuto comune.

Persistono episodi necessari, delte, contesti, decisioni e sostegni con ID
stabili; non i numeri `derivation_<n>` né puntatori a cache di turno. Al riavvio
si ricostruiscono le viste operative e si rivalidano le dipendenze. Un candidato
aperto può essere salvato come aperto, mai ricaricato come fatto confermato.
Il routing di `/save` va provato su una copia completa di lavoro dedicata:
oggi può scrivere nell'albero curato, il solo `PARROT0_SESSION` non lo isola.

### 14.7 Il costo della ricerca

Non enumerare ogni coppia di fatti della KB a ogni turno. Attivare il lavoro
sui residui e sulle osservazioni cambiate; recuperare episodi tramite gli
indici delle ancore e aggiornare solo le dipendenze toccate. Questa selezione
riduce il lavoro, non spegne il sapere del profilo: una ricerca ulteriore può
ancora consultare tutto il mondo pertinente.

Misurare candidati generati/provati/scartati, ragioni degli scarti, passi del
solver, costo per turno e memoria degli episodi. Limiti di memoria, profondità
e tempo devono produrre `incomplete` e una ricerca riprendibile: il primo
candidato incontrato non diventa vincitore perché è finito il budget.
Non fissare ora latenze inventate; confrontare mediana e coda dei turni con il
binario di base, sotto la stessa KB completa. Le soglie operative vanno scelte
dopo quel profilo e registrate con la politica che le usa.

## 15. Primo esperimento verticale: un ponte fra letture

**Scelta:** iniziare dal ponte di relazione (§7, acqua/ebollizione). Costringe
a mostrare un cambiamento del lettore che trasferisce ad altri soggetti. La
sigla può invece sembrare riuscita con una semplice memorizzazione; la
riparazione dell'atto richiede già l'identità stabile della scelta precedente.
Si tengono entrambe come estensioni dello stesso circuito, senza tre handler.

### 15.1 Preparazione che evita un falso verde

La KB possiede già il ponte `construction_frame` per «boiling point», quello
per «freezing point» e fatti ottenuti usandoli. Per misurare crescita sul primo
si ritira **in memoria** quella precisa lezione e se ne controllano gli effetti
derivati pertinenti; non si cancella grammatica, fatti del mondo o base.
Un test meccanico può farlo tramite l'API del banco, dichiarandolo come setup,
mai contandolo come insegnamento naturale. Il test comportamentale finale deve
includere anche una relazione non precedentemente insegnata, scelta dopo il
censimento della KB e fissata prima dell'implementazione.

Controllare separatamente: la domanda non usa più il ponte ritirato; i fatti
del mondo rimangono accessibili dalla loro lettura nota; nessun sinonimo o
costruzione duplicata fornisce già il ponte bersaglio. Se risponde comunque,
tracciare la via: è conoscenza preesistente, non crescita misurata. Non eliminare
altre conoscenze solo per ottenere il rosso desiderato.

### 15.2 Che cosa deve accadere nel caso lavorato

Contatto iniziale del §7, poi un secondo episodio su un soggetto diverso e un
uso riservato per il trasferimento. Il transcript esatto si congela **prima**
della cura; la sequenza sotto è un contratto progettato, non un esito misurato.

| passo | ingresso / operazione | risultato richiesto |
|---|---|---|
| 0 | domanda sul punto di ebollizione, ponte bersaglio assente | baseline e via reale registrate; nessun apprendimento implicito dalla risposta attesa del test |
| 1 | «Water boils at 100 degrees Celsius, so its boiling point is 100 degrees Celsius.» | ricevute di entrambe le porzioni, coreferenza di `its` con sostegno o ambiguità dichiarata; nessuna trasformazione automatica del «so» in equivalenza |
| 2 | allineamento delle ancore | candidato che associa soggetto e valore della costruzione nominale ai ruoli di una relazione nota; alternativa coincidenza ancora visibile |
| 3 | prova locale | il candidato spiega quel contatto; resta un'ipotesi locale, non un'equivalenza universale |
| 4 | secondo episodio su acetone, con descrizione verbale e nominale ordinaria | verifica del legame su soggetto/valore diversi e confronto dei concorrenti; stessi valori ripetuti non bastano |
| 5 | «What is the boiling point of ethanol?» tenuta fuori dagli esempi | il nuovo lettore deve raggiungere il fatto già nella KB. La correttezza misura trasferimento, non viene usata dal sistema come conferma se manca feedback indipendente |
| 6 | stessa relazione in prosa nuova e domanda successiva | la costruzione deve leggere e far ritrovare un fatto, non funzionare solo nel modulo che risponde |
| 7 | ritiro della conoscenza acquisita, poi stessi usi | cade la via appresa e restano i fatti con sostegni indipendenti |

**Ostacolo da non nascondere:** oggi il valore di `boils_at(water, …)` è una
frase che comprende temperatura, conversione e condizione; quello di ethanol
comprende un confronto. Non è già una quantità tipata. L'allineatore non può
unificare «100 degrees Celsius» con tutto quel testo come se fossero uguali.
Prima si verifica quali nodi quantitativi e contestuali la IR produca davvero.
Se mancano, questa è la prima lacuna da rendere visibile e il trasferimento
resta rosso; niente estrattore privato di numeri per far passare il banco.

Anche il secondo episodio può lasciare candidati indistinguibili. In quel
caso il passo 5 richiede una lettura qualificata o una domanda discriminante;
non si abbassa la soglia per ottenere una risposta assoluta. Va registrato
quale informazione ulteriore serve e se arriva in lingua ordinaria.

### 15.3 Come si prova che non è uno schema nuovo

Ripetere l'esperimento con i due enunciati in turni separati, con il loro
ordine invertito e con un connettivo non usato nell'acquisizione. Il contesto
deve fornire le ancore equivalenti; non basta cambiare punteggiatura se così
si rende la frase semanticamente diversa. Nessuna riga specifica deve dire
«con questo marcatore asserisci questo ponte».

Il passo più forte viene dopo: stesso generatore per una relazione diversa,
poi per una corrispondenza fra espressioni. Ogni nuova famiglia deve dichiarare
quali primitive/consumer riusa e quale rappresentazione eventualmente manca.
Se occorre un nuovo schema di ingresso è L2+; se occorre estendere la IR si
registra quella capacità, senza attribuire il rosso all'apprendimento già fatto.

Il test ricorsivo usa un errore della generalizzazione: dopo contatti che hanno
suggerito una coreferenza, un uso con inciso e una correzione ordinaria devono
restringere la condizione appresa e cambiare un terzo caso. Correggere solo
quell'istanza è L2; cambiare la condizione tramite il contatto è il traguardo L3.

## 16. Incrementi e banco di falsificazione

### 16.1 Ordine di implementazione

Un circuito per incremento. Ogni riga richiede il proprio diff, traccia e
risultato nel piano; non si dichiara L3 completato perché un'infrastruttura passa.

| incremento | lavoro concreto e siti | condizione di uscita |
|---|---|---|
| **I0 — baseline** | congelare transcript e controlli del §15; profilo completo; trace di `reading_choice`, nodi, `construction_reading`, fonti | distinguere casi già saputi, residui IR e lacune del learner; primo rosso riproducibile senza impoverire la KB |
| **I1 — osservazione** | `input-structure.p0`, `reading-choices.p0`, pubblicazione in `99-registry.c`: identità episodio/versione, originale↔canonico, candidati e residui | rileggere non riscrive il passato; due occorrenze della stessa parola e due candidati restano distinguibili |
| **I2 — proposta e prova locale** | estendere lettura/binding in `10-memory-knowledge.c` e politiche KB; generalizzatore strutturale solo dove manca | da contatto ordinario nasce il delta; si confronta alla lettura corrente nella stessa mente, senza effetti né attivazione globale |
| **I3 — uso rivedibile** | verifica indipendente, domanda discriminante, consumer contestuale di `construction_reading`, sostegni per gli effetti L3 | passa un trasferimento utile, un contrasto e il ritiro; la semplice duplicazione dell'esempio non promuove |
| **I4 — durata** | estendere oggetti di contenuto/derivazione e routing del salvataggio | processo nuovo conserva lo stato esatto; ritiro dopo riavvio non resuscita la lettura, una seconda fonte conserva il suo fatto |
| **I5 — crescita del modo di imparare** | nuova relazione, nuovo strumento di contatto e correzione della condizione appresa | nessun nuovo teach-handler o schema di contatto; uso della condizione corretta su un caso tenuto fuori dal dialogo |

I1–I2 da soli sono infrastruttura. I3 dà il primo circuito di contatto con
portata dichiarata; I4 lo rende durevole; I5 misura se si supera la chiusura
degli schemi e non soltanto il loro registro linguistico. Un blocco si annota
nel punto preciso, con input e struttura mancante, senza costruire in parallelo
un secondo lettore.

### 16.2 Il banco da scrivere prima della cura

| prova | che cosa falsifica |
|---|---|
| **prima → contatto → trasferimento** | la risposta era già nella KB oppure si è memorizzato solo l'esempio |
| **ordine inverso, enunciati separati, altra lingua già leggibile** | la proposta dipende dalla posizione o da un marcatore previsto |
| **stesso valore, relazione diversa** | coincidenza scambiata per equivalenza |
| **Paris/the capital; LED/a kind of lamp; citazione altrui** | coreferenza, classe, alias e contenuto citato collassati nello stesso fatto |
| **stesso interlocutore ripete lo stesso contenuto** | conteggio delle ripetizioni scambiato per evidenza indipendente |
| **predizione derivata da H usata per confermare H** | auto-conferma e cicli di sostegno |
| **correzione senza «No» / «No» senza correzione** | comando di superficie spacciato per revisione della lettura |
| **controesempio, poi terzo uso** | correzione locale senza revisione della generalizzazione |
| **ritiro di H con due fonti di C** | cancellazione di conoscenza indipendente o conseguenza orfana lasciata attiva |
| **aggiunta che invalida `absent` / modifica di un aggregato** | manutenzione che reagisce solo alle cancellazioni |
| **limite di ricerca, overflow, effetto tentato durante la prova** | incompleto spacciato per falso, o ipotesi scartata che lascia effetti |
| **salva → processo nuovo → ritiro → processo nuovo** | stato epistemico perso, ID effimeri persistiti o ipotesi risuscitata |
| **togli il contatto, togli solo il candidato, riattiva il consumer** | l'effetto attribuito a L3 proviene invece da un handler o da una regola seminata a mano |

Per ogni nuova forma riconosciuta: acquisizione a runtime e ablazione mirata,
senza ricompilare. Per la prova di comprensione: conoscenze reali preesistenti,
prompt naturale, risposta semanticamente utile. Un caso inventato può isolare
meccaniche di identità o ritiro, ma non certifica connecting dots (§MANTRA).

Il banco interno può interrogare delte e sostegni per diagnosticare; il successo
comportamentale si valuta anche dalla risposta. Non basta imporre che venga
eseguito il percorso interno desiderato. Le nuove verifiche vanno nel banco
dedicato L3, senza gonfiare `make soft-test` né alzarne il budget.

Regressioni pertinenti già disponibili: `l2_reading_choices.p0t`,
`reasoning/clause_content.p0t`, `reasoning/derivation.p0t`,
`reasoning/taught_episode.p0t`, `conversation/context_scope.p0t`,
`engine/materialized_view.p0t` sotto `tests/p0t/`; aggiungere i banchi delle
costruzioni effettivamente toccate. Eseguire quelli pertinenti al diff, poi
`make soft-test`; questo piano documentale non richiede l'intera suite C.

### 16.3 Misure e condizioni che fanno cambiare ipotesi

Registrare separatamente: casi già noti prima, nuovi adattamenti riusciti,
trasferimenti, false generalizzazioni, sospensioni corrette, domande necessarie,
ritiri corretti e costo. Un unico «learning score» nasconde i fallimenti.
Per ogni successo elencare il residuo: grammatica iniziale, spazio delle
trasformazioni, politica di accettazione e superficie del feedback ancora G1/G2.

Il progetto del circuito è smentito o da rivedere se:

- cresce un riconoscitore specifico per ogni strumento di contatto;
- le alternative non si possono formulare senza avere già inserito il ponte
  che si pretende di apprendere;
- i vincoli restano indistinguibili e si sceglie comunque per frequenza o ordine;
- il costo richiede di cancellare conoscenza dal profilo;
- una politica appresa non può essere corretta dallo stesso ciclo;
- un verde scompare appena si tolgono conoscenze o attesi seminati apposta dal test.

In questi casi si conserva la diagnosi e si aggiorna il piano: non si rinomina
il risultato L3. L2+ rimane un ripiego dichiarabile, con costo e residuo espliciti.

## 17. I0 — la baseline, misurata (24 settembre 2026, sera)

**Stato: I0 chiuso. Nessuna modifica al motore o alla KB.** Binario e KB di
`783f59dd`, profilo `agi` completo, `PARROT0_SESSION` vuoto. Transcript e trace
in `docs/labs/l3/I0/` (`stato.txt`, `*-dialogo.txt`, `*-trace.txt`).
Setup dichiarato (§15.1): ritiro **in memoria** della sola lezione «the boiling
point of x is y means x boils at y», con la sua forma parlata; nessun'altra
conoscenza toccata.

### 17.1 Che cosa succede, passo per passo (§15.2)

| passo | ingresso | risposta | che cosa dice il trace |
|---|---|---|---|
| 0 | «What is the boiling point of water?» (ponte ritirato) | la definizione dell'acqua | `read.project … semantic_topic_cue(water) … speaks`: la proiezione risponde di X invece di «R of X» (la lacuna di RI-020, riemersa) |
| 0 | «What is the boiling point of ethanol?» | «I don't know» | onesto |
| controllo | «What is the freezing point of mercury?» | −39 °C | l'altra costruzione salvata resta: l'ablazione è mirata |
| 1 | «Water boils at 100 degrees Celsius, so its boiling point is 100 degrees Celsius.» | **«It boils at 100 degrees Celsius (212 °F) at sea level…»** | nessuna lettura. Il ramo compilato gen241 (`10-memory-knowledge.c`, catena `…chain11449`, template `it_boils_at_x`, solo acqua) risponde a una **dichiarazione** come se fosse una domanda. Il trace dice solo «knowledge answers»: sito muto |
| 1, dopo | ancora acqua / etanolo / acetone | definizione / «I don't know» / «I don't know» | il contatto non ha lasciato niente |
| 4 | «Acetone boils at 56 degrees Celsius; its boiling point is 56 degrees Celsius.» | **«That looks like a snippet of code.»** | il punto e virgola fa rivendicare il turno al rilevatore di codice |
| 6 | `read: Liquid nitrogen boils at minus 196 degrees Celsius. Its boiling point is far below room temperature.` | «Learned 0 fact(s), skipped 1» | la frase con «its» non si legge; «At what temperature does nitrogen boil?» → «I don't understand» (l'unico lettore di `boils_at` conosce solo l'acqua) |

### 17.2 La relazione di controllo, fissata prima della cura

Dal censimento (§15.1): **`born_in` ↔ «birthplace»**. È vera, è già in KB
(`born_in(einstein, ulm)`, `marie_curie`→`warsaw`, `napoleon`→`ajaccio`,
`galileo_galilei`→`pisa`, `christopher_columbus`→`genoa`) e il verbo risponde
(«Where was Marie Curie born?» → warsaw). Il nome non è mai stato collegato
(«What is the birthplace of Marie Curie?» → «I don't know about birthplace»:
onesto). Scartati per ora `discoverer`/`painter`: sono nomi di **agente**, una
forma di relazione diversa dal nome di proprietà del §15.

Il contatto analogo oggi **scrive il falso**:

```text
> Einstein was born in Ulm, so his birthplace is Ulm.
Held: einstein dates from ulm so his birthplace is ulm.
> Napoleon was born in Ajaccio; Ajaccio is his birthplace.
Held: napoleon dates from ajaccio. I couldn't read «Ajaccio is his birthplace.».
```

Trace (`controllo-trace.txt`): il lettore dei frame legge **giusto**
(`frame bind «@S was born in @O» slots=[einstein][ulm]`, `extract_frame(…, born_in)`),
ma vince la forma di lezione `year_stated` (`rel=year_of
obj=ulm_so_his_birthplace_is_ulm`): uno schema L1 per «X was born in ANNO» che
non verifica che l'oggetto sia un anno e attraversa il confine della
proposizione. Il D33 in una riga: due letture, una giusta e tipata, una
sbagliata, e vince la seconda.

### 17.3 Classificazione dei reperti (uscita di I0)

| # | reperto | specie | incremento che lo incontra |
|---|---|---|---|
| B1 | ponte «boiling point», ethanol, freezing, LED, contrazione: già in KB | **già saputo** — un verde su questi non misura contatto | setup §15.1, sempre |
| R1 | la IR riconosce «so» come `discourse, consequence` e «its» come possessivo, ma **nessun nodo lega le due proposizioni** e il confine non ferma gli slot | **residuo IR** | I1 |
| R2 | «100 degrees Celsius» non è una quantità: `measured_value/1` vuole numero+unità di due parole e «degree Celsius» non è un'unità in `measures/2`; le entità sono `water_boils`, `degrees_celsius`, il numero sparisce | **residuo IR / KB** | I1 (prima di I2: senza ancora quantitativa l'allineamento del §14.2 non parte) |
| R3 | «its boiling point» → candidato `its boiling`; nessun frame legge «its R is V» | **residuo IR** | I1 |
| R4 | i valori `boils_at` storici sono frasi, non quantità (previsto al §15.2) | **dato KB** | I2 (confronto fra quantità, non fra testi) |
| T1 | il ramo gen241 risponde a una dichiarazione | **furto di turno** (mantra #21) da un ramo `TODO(kb-first, gen489)` | prima di I1: il contatto non arriva al lettore |
| T2 | `year_stated` vince sul frame `born_in` e scrive il falso | **furto di lettura** (D33), X>0 se salvato | prima di I1 |
| T3 | il punto e virgola → rilevatore di codice | **furto di turno** | prima di I1 |
| L1 | nessun meccanismo propone il ponte dal contatto | **lacuna del learner**: è il lavoro di I2 | I2 |
| D1 | la risposta di T1 non ha riga nel trace | **trace muto** | subito |

### 17.4 Che cosa ne segue per l'ordine di lavoro

Il §16.1 mette I1 (osservazione) dopo I0. La baseline aggiunge un gradino
**prima**: finché T1–T3 rubano il contatto, nessuna osservazione arriva alla IR
e nessun candidato può nascere. Non sono lavoro di L3, ma sono la sua
precondizione, e vanno curati nella forma del mantra #21: **retrocedere il
lettore immaturo**, non insegnargli una cessione.

- **T1**: il ramo gen241 è una catena compilata, immatura per definizione. La
  cura è retrocederlo a ultima risorsa per le sole *domande* (la sua forma di
  pertinenza), o sostituirlo con un consumatore KB di `boils_at`/`freezes_at`.
- **T2**: la forma `year_stated` deve chiedere che l'oggetto sia un anno
  (classe KB esistente?) e fermarsi al confine della proposizione. È anche un
  caso per il §2.3: la condizione «è un anno» è una **decisione** da rendere
  raggiungibile, non da compilare nella forma.
- **T3**: da tracciare prima di decidere (quale cue del rilevatore di codice).
- **D1**: dare voce al ramo gen241 nel trace, nella stessa passata.

Criterio: dopo il gradino, i due contatti (acqua, Einstein) devono arrivare alla
IR **senza** essere letti in modo sbagliato e senza scrivere niente: l'esito
atteso è un'osservazione non capita o parzialmente capita, cioè il punto di
partenza di I1. Un contatto che «impara» già a questo gradino sarebbe sospetto.

