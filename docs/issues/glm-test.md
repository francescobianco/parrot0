# Report di test critico — parrot0 valutato in chat interattiva

> ## ✅ ESITO DELLA LAVORAZIONE — 7 settembre 2026 (`gen505x`)
>
> Sette commit, da `69cee26` a `453828e`. **Undici finding chiusi, tre parziali,
> undici aperti con diagnosi.** Metodo: `docs/plans/procedura-crescita-kb.md` —
> guardare prima di ipotizzare, provare in KB prima di compilare, differenziale
> con lo stash prima di accusare il proprio codice.
>
> ### Chiusi
>
> | § | esito |
> |---|---|
> | **3.1** + **4.1** | erano **lo stesso difetto**, e la diagnosi del report era sbagliata: `habitat(penguin, ice)` **e' in KB**. «Ice.» non era confabulato — era vero e dato a una domanda polare. Perimetro reale piu' largo: *ogni* polare rispondeva con un valore («Is Paris the capital of France?» → «France.»). `polar_opener/1` + `p0_polar_reply`: se una coppia di token soddisfa la relazione → «Yes.»; «No.» solo dove `closed_world_answer/2` lo autorizza |
> | **3.2** | la negazione teneva il complemento: chiuso di rimbalzo dando a `habitat` le sue forme |
> | **3.4** | tre cause in fila: il valore non aveva una fine; il decompositore rifiutava un pronome; una sotto-frase non portava il proprio grezzo. Nome **e** citta' ora memorizzati e richiamabili, con la maiuscola |
> | **3.6** | nove `||` sul turno intero → `p0_is_confirmation` posta due volte, sul turno e sulla prima parola. «yes, learn about it» accetta |
> | **4.3** | **non era un difetto**: «Every dog is a mammal. Rex is a dog. Is Rex a mammal?» → «Yes.». A non passare era la coda del prompt del report |
> | **4.4** | non «i percorsi leggono solo l'inglese»: locuzioni. Undici `phrase_canon`, **zero righe di C** |
> | **4.6** | clausola relativa e doppio output: chiusi di rimbalzo |
> | **6.3** | `assent_word` e `dissent_word` condividevano la risposta: chi negava riceveva una conferma |
> | **3.5** | *(gen506)* non «due letture che non si parlano»: `kb_define_entity` legge benissimo i fatti unari. Era la facolta' di **acquisizione** a prendersi il turno per cercare fuori cio' che parrot0 aveva gia' dentro — mantra #21, dove non c'e' lacuna non c'e' titolo. `what is wombat` / `what is the wombat` / `define wombat` / `tell me about wombat` rispondono dal fatto insegnato (resta «what is **a** wombat», forma a se') |
> | **6.2** | *(gen506)* l'articolo era scritto dentro il template: «say «something is a eiffel»». `{article}` legge la stessa politica che il resto del motore usa per «a»/«an» — sbagliare la grammatica **nel messaggio con cui si chiede di essere insegnati** insegna male |
>
> | **5.4** | *(gen506)* causa trovata, cura no. La lingua della risposta si decide con `lex_class_member(b, "entity_pronoun", lang)` — la classe dei **pronomi**: e' vera esattamente quando il codice e' «it», perche' il codice italiano coincide col pronome inglese «it». Funziona per coincidenza, e **nessuna lingua nuova** (fr, de, es) potra' mai ricevere la propria resa. Scheda al sito in `10-memory-knowledge.c`, specie `principio` — non scade |
>
> ### Parziali
>
> | § | fatto / resta |
> |---|---|
> | **3.3** | la porta d'**ingresso** e' aperta: «Penguins live in Antarctica» ora si **impara** (era «Ice.»). La porta d'**uscita** no: `answer_frame("live in", habitat)` spegne le domande che gia' funzionano, perche' `can_live_in` occupa la stessa superficie |
> | **6.6** | il punto come separatore funziona sui casi positivi e **fabbrica un fatto falso** su quelli negati: annullato, scheda al sito |
> | **5.4** | migliorato in italiano; «hi» → «Ciao!» resta |
>
> ### Aperti, con la causa scritta
>
> §3.5 (la stessa entita' sotto forme diverse), §4.2 (aritmetica), §4.5
> (multi-obiettivo), §4.7 (pianificazione), §5.1–5.3, §6.1 (maiuscole), §6.2
> (grammatica dei template), §6.5, §6.7 (fetch).
>
> ### Tre schede di tentativi annullati — da non ripercorrere
>
> Al loro sito, nella forma **verdetto / ragione / condizione / specie**:
> `faculty_force(answerframe, question)` (la forza non e' leggibile quando il
> registro gira), `answer_frame("live in", habitat)` (collisione di superficie),
> il punto come separatore (la polarita' non sopravvive al taglio).
>
> ### Sul metodo del report
>
> Le riproduzioni deterministiche hanno funzionato: **ogni** finding e' stato
> riprodotto al primo tentativo. Due diagnosi del report erano sbagliate (§3.1
> confabulazione, §3.3 soggetti plurali) e in entrambi i casi **la cura giusta
> era diversa da quella suggerita** — il valore del report non e' diminuito da
> questo: e' aumentato, perche' le sonde erano abbastanza precise da falsificare
> la propria spiegazione.

---

> ## ✅ ESITO DELLA LAVORAZIONE — 12 settembre 2026 (`gen512`, undicesimo e dodicesimo giro)
>
> Commit `8a9b41d3` e `2a6626d6`. **Nove voci chiuse, verificate a PROMPT con le
> sonde del §D — nessuna suite** (politica dei test di F.).
>
> | § | esito |
> |---|---|
> | **3.2 + 3.3** | **il finding #1 e' chiuso.** Sonda B parola per parola: «No, penguins do not live in the Arctic. They live in Antarctica.» → *«Learned: penguins do not live in the Arctic. Learned: penguins live in antarctica.»*; «Do penguins live in Antarctica?» → **«Yes.»**; «Do penguins live in the Arctic?» → **«No.»**. E «The sky is not green.» → «Is the sky green?» → **«No.»**, «Is the sky blue?» → **«Yes.»** |
> | **5.3** | «can birds fly?» → «Yes.»; «can fish fly?» dopo «fish cannot fly» → «No.»; ignoto → declinazione onesta. Non era il C: due forme in KB con lo STESSO nome (`ability_asked`) fondevano pezzi e atti |
> | **D1** (polare sull'insegnato) | «A wombat is a marsupial.» → «Is a wombat a marsupial?» → **«Yes.»**. Il punto che chiude la frase restava attaccato all'ultima parola: la classe imparata era «marsupial.» |
> | **D1** (il «No.» non guadagnato) | «Is a wombat an animal?» ora declina. Una definizione e' chiusa solo quanto cio' su cui poggia: `animal` poggia su `is_a_t`, che si nutre di `is_a`, che riceve un fatto alla volta |
> | **D3** | «Imparato: **libro rosso** si trova in tavolo» — la chiave non parla piu' all'utente |
> | **D4** | «What is the wombat?» → «wombat is a marsupial.» — un punto solo |
> | (handoff) | «penguins live in antarctica. they eat fish.» non risponde piu' «A bear.» |
>
> **La lezione del giro, per chi legge il documento:** quasi nessuna di queste
> voci era conoscenza mancante. `habitat(penguins, arctic)` era GIA' in KB, e
> «do penguins live in **the** arctic?» non lo trovava perche' cercava
> `habitat(penguins, the arctic)`. Sono **strade rotte, non lacune** — ed e'
> per questo che non si chiudono parlando: insegnare passa dalla stessa strada.
>
> Restano aperte, in ordine: **4.2** (numeri con ruoli — diagnosi a meta': «Tom
> has 12 apples» si impara e «How many apples does Tom have?» non lo trova),
> **4.7** (il piano irraggiungibile da «how do I make X?» e «first» senza
> maniglia), **5.2**, **4.5**, **3.5** e **D2** (servono la rete), **6.1**.
> Diagnosi e ordine di attacco in `LEARN_TODO.md`, handoff del 12 settembre.

# ANALISI CRITICA — nuova intervista dell'11 settembre 2026 (`gen512`), `make chat` soltanto

> Autore: Buffy (GLM, agente Freebuff). Stesso metodo del report: sonde via pipe
> su stdin, sessioni indipendenti, `make chat` soltanto (tools on, fetch on),
> ~55 turni in 11 sessioni. Albero: `gen512 (ottavo giro, interrotto)@59be1ad0`,
> albero pulito; il binario e' stampato `[gen510-riparazioni@8995295c]` per quasi
> tutte le sonde e `[…@59be1ad0]` per l'ultima. Differenziale con `git stash`
> eseguito sui due casi anomali di oggi (§4.2, polar su fatto insegnato): si
> comportano **uguale** sull'albero pulito, quindi non sono artefatti del lavoro
> in corso. Non riprovate in questa passata: frame WWII (§2.4), ledger derivato
> (§2.8), richieste MCP/tools.

## A. Verdetto per sezione — che cosa regge e che cosa no

### Chiusi dopo il report, confermati al vivo oggi

| § | prova d'oggi |
|---|---|
| **3.1** | «Ice.» e' **sparito**. «Do penguins live in the Arctic?» → declinazione onesta: *«I don't know: nothing I hold says penguins habitat the arctic, and nothing says it isn't so. Tell me either way and I will hold it.»* — esattamente la cura chiesta alla voce 2 dell'ordine d'attacco. Nessuna risposta sbagliata presentata come fatto in tutta la rivista |
| **4.1** | «Is Rome the capital of Italy?» → **«Yes.»** (non piu' «Italy.»). Il canale polare e' booleano |
| **4.3** | «Every dog is a mammal. Rex is a dog. Is Rex a mammal?» → **«Yes.»** Il quantificatore non devia piu' il turno |
| **4.6** | «A wombat is a marsupial that lives in Australia» → *«Learned: wombat is a marsupial. Learned: wombat live in australia.»* — **due fatti, un solo ack**; «Where does the wombat live?» → «australia.» |
| **6.3** | «no» nudo ora **declina**: *«All right, I won't look it up.»* (chi negava riceveva conferma) |
| **3.6** | «ok learn about it» e «yes» portano **entrambi** al fetch (quokka, capybara: appresi nello stesso turno) |
| **3.4** | «My name is Marco and I live in Turin» → nome **e** citta' memorizzati: «what is my name?» → «Your name is Marco.»; «where do I live?» → «Turin.» |
| **6.2** | il messaggio d'offerta dice oggi «say «something is **an** eiffel»»» — l'articolo legge la politica comune (gen506, verificato a runtime) |
| **4.4** (parte) | «Ciao, mi chiamo Giulia» → «Piacere di conoscerti, Giulia!» + richiamo ok; «Il libro rosso e sul tavolo» → «Dove si trova il libro rosso?» → «tavolo.»; «Come si prepara un caffè?» → piano in 4 passi. Il conversazionale italiano esiste |
| **5.1** | il controfattuale ora dà **contenuto dal mondo**: «What would happen if the sun stopped shining?» → *«From what I've learned: sun causes plant_growth, and plant_growth causes oxygen.»* — niente piu' ponte d'oro di metodo; resta parziale (una catena, non la conseguenza chiesta) |

### Ancora validi — riprodotti oggi, quasi sempre nella stessa forma

| § | prova d'oggi | nota |
|---|---|---|
| **3.2 / 3.3** | «No, penguins do not live in the Arctic. They live in Antarctica.» → *«Held: penguins does not habitat in_the_arctic_they_live_in_antarctica.»* | La mangled e' **cambiata di forma** (niente piu' «penguins do not live»), ma il «fatto» resta non retrazione e non affermazione usabile. E «X is not Y» semplice («The sky is not green») non atterra: «I don't understand that yet.» **Il canale di correzione resta il difetto #1: si impara, non si ritira.** |
| **3.5** | «What is the Eiffel Tower?» → «eiffel tower is about 330 meters.»; subito dopo «How tall is the Eiffel Tower?» → muro + offerta di insegnare **eiffel** | La conoscenza fresca e' invisibile alla forma nuova, identico al report. Il learn dello stesso turno non collima con l'entita' che le domande successive cercano |
| **4.2** | «I have 12 apples and I eat 5 apples. How many apples do I have?» → *«I couldn't read «I have 12 apples and I eat 5 apples». A i has 12 apples.»* | Muro come allora, ma la lettura e' ora **evidenziata** (diagnosi a regalo: «A i» — il soggetto viene letto come articolo) |
| **4.5** | «What is your name and where do you live and what can you do?» → risposta che non aggancia nessun subgoal in modo pertinente (e il ledger, evocabile da solo, non arriva) | Aperto |
| **4.7** | «I want to make pasta carbonara. What do I need to do first?» → «I am not sure what you mean by «first».» | Aperto: la facoltà di piano resta irraggiungibile da questo shape |
| **5.2** | «Tom baked a cake and gave half to Anna. Who has less cake?» → «I don't understand that yet.» | Aperto |
| **5.3** | «Can fish fly?» → «I didn't quite catch that.» | Muro onesto ma nessun «No» inferito da assenza |
| **5.4** | «hi» → «**Ciao!**» in sessione inglese; e ora «Come mi chiamo?» risponde «**Your name is Giulia.**» in sessione italiana | La famiglia esiste ed e' instabile in **entrambe** le direzioni — la scheda 5.4 (decide `entity_pronoun` per coincidenza) resta valida e non scade |
| **6.1** | «eiffel tower is about 330 meters.» — minuscole in output | Aperto; v. nuovo D3 sotto, stessa radice |
| **6.7** | eiffel: fetch instradato ma la resa non aggancia l'entita'; quokka/capybara: fetch+estrazione ok | Affidabilita' non uniforme, confermata |

## B. Nuovi finding di oggi (non nel report)

- **D1 🟠 Polar su un fatto appena insegnato mura.** «A wombat is a marsupial.» → «Is a wombat a marsupial?» → *«I don't know about marsupial.»* — mentre la stessa catena con nomi propri («Rex…») passa (§4.3). Il gen506 aveva chiuso §3.5 sul ramo *what-is* («what is the wombat» oggi risponde davvero il fatto insegnato — verificato); il ramo **sì/no sull'insegnato** con articolo indeterminativo e' ancora senza maniglia. E' la stessa malattia di §3.5 vista da un'altra forma di domanda: articolo+comune insegnato non diventa l'entita' che il percettore polare cerca.
- **D2 ⚪ Il menu di disambiguazione non consuma la scelta.** Multi-hop su «eiffel» apre un menu («Which one do you mean?»); rispondere «Eiffel Tower» al turno dopo produce una nuova offerta di insegnamento, non la ripresa. Nuova superficie d'offerta, stessa famiglia frammentata di ripresa di §3.6: **il ciclo si riapre su ogni nuova superficie che lo produce.**
- **D3 ⚪ Il token canonico trapela nell'ack italiano.** «Il libro rosso e sul tavolo.» → *«Imparato: book_red si trova in tavolo.»* — la canonicalizzazione parla all'utente. Stessa radice di §6.1: la superficie dell'entita' non e' ricostruita alla stampa, e l'output e' conoscenza (mantra #16).
- **D4 ⚪ Doppio punto sulla resa del fatto insegnato.** «What is the wombat?» → *«wombat is a marsupial..»* — il template concatena il punto del fatto al punto della resa.

## C. Giudizio aggiornato sul report — che cosa resta valido come documento

1. **La tesi centrale regge, con perimetro spostato.** «Oggi parrot0 risponde; non ancora impara dallo scambio» era corretto al gen501 e resta corretto al gen512 — ma il difetto #1 non e' piu' «il canale di correzione non esiste»: e' **presente in ingresso e rotto in uscita**. Si impara (anche da soggetti plurali e da clausole relative, che il report dava perse); non si ritira. §9 voce 1 (retrazione parlando) resta il primo lavoro per danno.
2. **Le tre malattie del §7 reggono tutte e tre**, con pesi cambiati:
   - *le facoltà sono isole* — il vivo piu' forte di oggi: la stessa entita' insegnata e' prodotta da un lettore e non trovata da un altro (§3.5, D1); il menu D2 e' un'isola nuova nata **dopo** il report.
   - *il canale di correzione non esiste* — mezzo chiuso: ingresso sì (§3.3 porta d'ingresso aperta, confermata), uscita no (§3.2 di oggi).
   - *rispondere vs rispondere bene* — **molto migliorato**: zero confabulazioni in ~55 turni; i muri sono onesti e la maggior parte porta la porta aperta. Il rischio residuo si e' spostato dal contenuto alla **forma** (lingua sbagliata, token canonico, doppio punto): sono difetti di resa, non di verita'.
3. **Le diagnosi del report non sono state smentite da questa rivista.** Le due che il gen505x aveva corretto (§3.1 confabulazione, §3.3 soggetti plurali) sono oggi superate **anche nei fatti**: la prima cura (declinazione onesta) e' in produzione, la seconda (penguins si impara) e' in produzione. Il metodo delle sonde si e' confermato: riproduzioni deterministiche, un differenziale con stash per escludere l'albero di lavoro, diagnosi che il codice puo' falsificare.
4. **L'ordine d'attacco del §9, aggiornato al gen512:** fatte (da marcare) le voci **2** (guardia anti-confabulazione — «Ice.» non risponde piu'), **4** (famiglia di accettazione — salvo D2: ogni nuova superficie d'offerta riapre il ciclo), **5** (frattura dell'input composto), **6** (italiano conversazionale di base). Restano in testa, in quest'ordine: **1** (retrazione parlando — §3.2/3.3 di oggi), **3** (maniglia unica dell'entita' attraverso le forme — §3.5 + D1), **7** (numeri/comparativi con ruoli — §4.2/5.2). La voce 8 (pulizia superficie) ha oggi tre esempi in piu': D2, D3, D4.
5. **Che cosa il report non aveva e oggi servirebbe:** una sezione sulle **risposte di ripresa** (menu, offerte, selezioni) come classe a sé — e' la famiglia che genera D2 e ha generato §3.6, e cresce piu' in fretta delle altre: ogni nuova superficie interattiva aggiunge un ramo di ripresa non condiviso. Il test comportamentale da cricchetare: *menu → scelta → la scelta deve portare al fatto, non a una nuova offerta.*

## D. Riproduzioni rapide di oggi (da appendere a quelle del report)

```sh
# A. §3.1 chiuso: declinazione onesta al posto di «Ice.»
printf 'Do penguins live in the Arctic?\nIs Rome the capital of Italy?\n/quit\n' | \
  make -s chat

# B. §3.2/3.3 aperto: la negazione produce un fatto non usabile
printf 'No, penguins do not live in the Arctic. They live in Antarctica.\nDo penguins live in Antarctica?\n/quit\n' | \
  make -s chat

# C. D1: polar su fatto appena insegnato con articolo indeterminativo
printf 'A wombat is a marsupial.\nIs a wombat a marsupial?\n/quit\n' | \
  make -s chat

# D. D2: la selezione del menu non viene consumata
printf 'What is the capital of the country where the Eiffel Tower is located?\nEiffel Tower\n/quit\n' | \
  make -s chat

# E. §3.5: conoscenza fresca invisibile alla forma nuova (invariato dal gen501)
printf 'What is the Eiffel Tower?\nHow tall is the Eiffel Tower?\n/quit\n' | \
  make -s chat

# F. 5.4 in entrambe le direzioni
printf 'hi\n/quit\n' | make -s chat
printf 'Ciao, mi chiamo Giulia\nCome mi chiamo?\n/quit\n' | PARROT0_LANG=it make -s chat
```

— fine analisi critica —


> Autore: Buffy (GLM, agente Freebuff) — valutazione comportamentale su ca. **45 sonde**
> in **10 sessioni** indipendenti, due passate (base + acquisizione).
> Data: 2026-09-06. Binario: `bin/parrot0` `[gen501-challenge-pilot@7fdad87]`,
> **39567 fatti, 2863 regole (7.24%)**.

---

## 0. Metodo e riproducibilità

Tutte le sonde sono state condotte via pipe su stdin (nessun TTY), una sessione per
blocco tematico, così che ogni blocco riproduce esattamente ciò che si vede qui.

| modalità | env | note |
|---|---|---|
| conversazionale | `PARROT0_SESSION= PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0` | tools off, network off |
| conversazionale IT | come sopra + `PARROT0_LANG=it` | tools off, network off |
| acquisizione | come base + `PARROT0_WIKI_FETCH=1` | la configurazione reale di `make chat` |

Cioè: **niente API esterne di intelligenza, solo la KB locale + (in modalità
acquisizione) il fetch Wikipedia certificato.** È esattamente il banco di prova che
il progetto dichiara in `make chat`.

Ogni finding ha un comando di riproduzione. I fatti sono riportati **verbatim**;
le diagnosi interne sono esplicitamente marcate come *comportamentali* (da
confermare nel C) quando non ho letto il codice.

Gravità usata:

| livello | significato |
|---|---|
| 🔴 **CRITICO** | risposta sbagliata presentata come fatto, o incapacità strutturale di essere corretto: sonde che fanno regredire la tesi no-deception e KB-first |
| 🟠 **ALTO** | competenza conversazionale nucleare che fallisce su input naturale |
| 🟡 **MEDIO** | ampiezza/qualità del ragionamento, asperità di pipeline |
| ⚪ **BASSO** | superficie, coerenza linguistica, affidabilità del fetch |

---

## 1. Giudizio sintetico

**parrot0 è un motore di inferenza reale con un guscio linguistico fragile, non un
interlocutore intelligente.** Nel senso stretto è intelligente: deriva risposte che
non gli sono state dette (sillogismo: "Yes"), impara un fatto nuovo da una frase e
lo usa nel turno dopo, acquisisce conoscenza da sé via fetch e risponde **nello
stesso turno** ("Platypus belongs to the category mammal"). Nel senso ampio non lo
è ancora: si rompe su frasi composte, **non può essere corretto parlando**, perde
i complementi nella negazione, e le sue facoltà vivono su isole che non si
comunicano fra loro.

La distinzione decisiva, in una frase: **oggi parrot0 risponde; non ancora impara
dallo scambio.** E lo scambio è la tesi del progetto.

Punteggio per competenza (misurato, non stimato):

| competenza | esito | evidenza chiave |
|---|---|---|
| memoria a fattò singolo | ✅ | "My name is Marco" → "Your name is Marco." |
| memoria su input composto | 🔴 | "My name is Marco and I live in Turin" → "Got it: your name is Turin." |
| sillogismo semplice | ✅ | sparrow → "Yes." |
| inferenza quantificata a catena | 🔴 | "Every dog is a mammal…" → offerta di insegnare «every» |
| aritmetica da word problem | 🔴 | "I have 12 apples and I eat 5" → "I don't understand that yet." |
| confronto comparativo | 🔴 | "Tom gave half to Anna. Who has less cake?" → muro |
| controfattuale | 🟡 | template di metodo senza contenuti sul mondo |
| QA fattuale diretta | ✅ | Paris, Hamlet, WWII (frame ricco), Canberra (in italiano) |
| QA sì/no | 🔴 | "Is Rome the capital of Italy?" → "Italy." |
| insegnamento diretto (is-a) | ✅/🟡 | wombat imparato e propagato; la clausola relativa è caduta |
| insegnamento (locazione, singolare) | ✅ | red book → "table." |
| insegnamento (locazione, plurale) | 🔴 | "Penguins live in Antarctica" → non memorizzato |
| correzione / retrazione | 🔴🔴 | il difetto più grave del report, §3.1–3.3 |
| acquisizione autonoma (fetch) | ✅/🟡 | platypus ed Eiffel ok nello stesso turno; wombat fallito |
| riutilizzo della conoscenza fresca | 🔴 | "How tall is the Eiffel Tower?" subito dopo averlo appreso → declina |
| ciclo offerta→accettazione | 🔴 | "yes, learn about it" → "I don't understand that yet." |
| italiano conversazionale | 🟠 | "mi chiamo Giulia" → "Non capisco ancora."; Canberra però ok |
| richieste multi-obiettivo | 🟠 | risponde a un sotto-obiettivo, ignora gli altri (mantra #10) |
| onestà delle declinazioni | ✅ | la maggior parte dei muri è onesta (unica eccezione: "Ice.") |

---

## 2. Che cosa FUNZIONA — con evidenza

Per onestà del report, e perché è il capitale su cui costruire.

1. **Inferenza verificabile.** "All birds are animals. A sparrow is a bird. Is a
   sparrow an animal?" → **"Yes."** Sillogismo fresco, non lookup.
2. **Propagazione dell'insegnato all'inferenza.** Insegnato "A wombat is a
   marsupial", poi "Is a wombat a marsupial?" → **"Yes."** Un colpo solo, senza
   ricompilare: la tesi KB-first si vede lavorare.
3. **Acquisizione autonoma reale.** Con `PARROT0_WIKI_FETCH=1`:
   - "What is a platypus?" → **"Platypus belongs to the category mammal."** —
     fetch, parsing, risposta **nello stesso turno**, senza insegnamento.
   - "What is the Eiffel Tower?" → **"eiffel tower is about 330 meters."** — idem.
4. **Frame di evento, non fatti sparsi.** "When did World War II end?" →
   *"World War II ended in 1945; Japan's surrender was signed on Sept 2, 1945,
   aboard the USS Missouri in Tokyo Bay."* — esattamente la crescita per frame
   chiesta dal mantra #13.
5. **Memoria a fattò singolo**, nome e città, richiamo corretto dopo turni.
6. **Locazione insegnabile e richiamabile** per il modulo "X is on Y":
   "The red book is on the table" → "Learned: red_book is located in table." →
   "Where is the red book?" → **"table."** (sintagma multi-parola legato bene.)
7. **Declinazioni oneste** nella stragrande maggioranza dei muri: "I don't know
   about eiffel yet. Want me to learn about it?" — il muro con la porta aperta.
8. **Ledger di capacità derivato dallo stato reale**, non da un elenco scritto:
   "what can you do?" enumera le facoltà registrate (disciplina anti-impostor
   rispettata qui).
9. **Riproducibilità deterministica dei difetti.** "Ice." esce ogni volta che
   penguins+Arctic (o penguins in generale) compare. È un regalo per la suite di
   regressioni: i difetti sono cricchettabili.

---

## 3. 🔴 CRITICI — i difetti che fanno regredire l'esperimento

### 3.1 🔴 Confabulazione presentata come fatto: "Ice."

```
>>> Do penguins live in the Arctic?
Ice.
```

- La KB **non contiene** penguins (lo prova il fatto che "Where do penguins live?"
  riceve *"I don't know about penguins yet"*), eppure la domanda riceve una
  risposta fattuale secca: un'associazione lessicale (penguin→ghiaccio) presentata
  come conoscenza.
- **Viola direttamente il mantra #7** («uccidi il muro MAI con una risposta
  sbagliata — un errore factuale è peggio di un muro»). Su questo prompt, il
  sistema ha fatto esattamente la cosa che la dottrina vieta.
- È deterministico e facilmente cricchettabile: **prima regressione da scrivere**.
- Da indagare nel C: quale facoltà emette "Ice." e perché il suo titolo a
  rivendicare il turno (mantra #21) non è stato contestato da un guardia
  "nessun fatto su X ⇒ non rispondere su X".

Riproduzione:
```sh
printf 'Do penguins live in the Arctic?\n/quit\n' | \
  PARROT0_SESSION= PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0
```

### 3.2 🔴 La negazione perde il complemento — la correzione è rotta alla radice

```
>>> No, penguins do not live in the Arctic. They live in Antarctica.
Learned: penguins do not live.
```

- La frase di correzione è stata ridotta a `penguins do not live`: **il luogo è
  sparito sia dalla negazione sia dalla seconda frase.** Un fatto troncato non può
  ritrarre nulla né affermare nulla.
- Conseguenza misurata: la risposta sbagliata "Ice." **sopravvive a ogni
  replica** (tre re-ask, tre "Ice.").
- Diagnosi comportamentale (da confermare nel C): il ramo negazione del lettore
  lega il verbo e il soggetto e scarta il complemento locativo; inoltre la seconda
  frase coordinata non viene letta per niente.
- Questa è la forma viva del «cassetto senza maniglia»: parrot0 *impara* sotto un
  nome (`penguins do not live`) che poi non sa più pronunciare.

### 3.3 🔴 Nessun canale di retrazione: il sistema non può essere insegnato che è sbagliato

Tre varianti di correzione provate, **zero landing**:

| replica data | esito |
|---|---|
| "No, that is wrong: penguins live in Antarctica, not the Arctic" | **"Ice."** — la correzione stessa ha evocato la risposta confabulata |
| "No, penguins do not live in the Arctic. They live in Antarctica." | "Learned: penguins do not live." (fatto troncato, v. §3.2) |
| "No" (nudo) | **"Got it — what would you like to do?"** — la negazione nuda è stata letta come assenso |

E il controllo positivo, con un fatto pulito e standalone:

```
>>> Penguins live in Antarctica
Ice.
```

— **nemmeno l'affermazione positiva viene memorizzata** (i soggetti plurali/generici
come "penguins" sembrano non diventare entità; il singolare "the red book"
funziona, v. §2.6).

Perché è il difetto più grave in assoluto: `PRINCIPLES.md` lo dice testualmente —
*«un interlocutore che non può discutere con te di come ti comporti non sta parlando
con un learner»*. Oggi parrot0, dopo una correzione esplicita e ben formata,
**risponde di nuovo la stessa cosa sbagliata**. Non è un limite di copertura: è
l'assenza del canale. Un sistema che non può essere corretto non migliora col
dialogo, e il dialogo è l'unico canale che questo progetto ha dichiarato decisivo.

### 3.4 🔴 Collisione di slot su input composto + autocontraddizione al turno dopo

```
>>> My name is Marco and I live in Turin
Got it: your name is Turin.
>>> what is my name?
I don't know your name yet.
>>> where do I live?
You haven't told me that yet.
```

- Due difetti impilati: (a) il valore dell'**oggetto della seconda frase**
  ("Turin") è stato legato allo slot del **nome**; (b) al turno successivo il
  sistema afferma di non sapere il nome — cioè nega ciò che ha appena detto di
  aver memorizzato ("Got it").
- Il caso a fattò singolo funziona perfettamente ("My name is Marco" → "Nice to
  meet you, Marco!"), quindi il difetto è nella **frattura dell'input composto**,
  non nella memoria. La lezione dei mantra D33/D35/D37 si applica qui: due percorsi
  (lettura frasi, binding slot) che devono accordarsi e non condividono l'oggetto.

### 3.5 🔴 La conoscenza fresca è invisibile alle forme nuove di domanda

Sequenza misurata in modalità acquisizione:

```
>>> What is the Eiffel Tower?
eiffel tower is about 330 meters.
>>> How tall is the Eiffel Tower?
Hmm, I don't know about eiffel yet. Want me to learn about it? Or teach me: …
```

- **Nel turno immediatamente successivo** all'acquisizione, la stessa entità sotto
  una diversa forma di domanda è irraggiungibile. Non è un problema di rete né di
  fetch: il sapere c'è, la maniglia no. È la forma sperimentale esatta del limite
  dichiarato in testa a MANTRA.md («parrot0 non può rileggere ciò che ha già letto
  alla luce di ciò che ha appena imparato» — SC40) con un'aggravante: qui non
  serve rileggere, servirebbe **interrogare** ciò che c'è già.
- Stessa malattia nel caso wombat: insegnato "wombat is a marsupial" →
  "Is a wombat a marsupial?" → "Yes." MA "What is a wombat?" →
  *"I don't know much about wombat yet."* Il fatto propagato all'inferenza sì/no
  non è produttibile come definizione. **Due letture che non si parlano.**

### 3.6 🔴 Il ciclo offerta→accettazione è rotto (e non uniformemente)

| offerta ricevuta | accettazione data | esito |
|---|---|---|
| "Want me to look it up?" (wombat) | "yes" | "Looking up wombat… I still don't know much about wombat." — il fetch è stato tentato e fallito |
| "Want me to learn about it?" (eiffel, multi-hop) | "yes, learn about it" | **"I don't understand that yet."** — l'accettazione non è nemmeno stata riconosciuta |

- Due offerti diversi accettano solo forme diverse di "sì": l'intento di
  accettazione non è una famiglia unica. È esattamente il difetto del mantra #19(b):
  la condizione di ripresa è frammentata in rami compilati che non condividono la
  forma.
- In più: il fetch wombat è fallito dove platypus ed eiffel sono riusciti —
  affidabilità del canale di acquisizione irregolare (⚪ a sé, v. §6.7).

---

## 4. 🟠 ALTI — competenze nucleari che falliscono su input naturale

### 4.1 Le domande sì/no non ricevono trattamento booleano

```
>>> Is Rome the capital of Italy?
Italy.
```

Doppio fallimento: la forma interrogativa sì/no non è riconosciuta (nessun "Yes/No"),
e il riempimento di slot è pure sbagliato (alla lettura "What is the capital of
Italy?" la risposta sarebbe "Rome"; "Italy" è l'argomento che il sistema aveva già).
Unione di: riconoscimento d'intento interrogativo mancante + collisione di slot
argomento/domanda. Gravità ALTA perché il canale sì/no è il più frequente del
dialogo naturale, e l'esito attuale è una risposta senza senso.

### 4.2 L'aritmetica da word problem non lega i numeri ai ruoli

```
>>> I have 12 apples and I eat 5 apples. How many apples do I have?
I don't understand that yet.
```

Il mantra #12 (*«ogni numero deve avere un ruolo»*) descrive la cura: `total`,
`unit_price`, `paid`… qui i due numeri non hanno ricevuto alcun ruolo e il turno è
caduto prima dell'aritmetica. Nota: la capacità di fare aritmetica esiste nel
motore (è citata nel ledger); è il **ponte lingua→slot** che manca su questo
shape.

### 4.3 Il ragionamento quantificato a catena viene frainteso come cue

```
>>> Every dog is a mammal. Rex is a dog. Can Rex fly if all mammals are not birds?
Hmm, I don't know about mammals yet. … Or teach me: if «x every y» says what you
already say another way, say «x every y means x <known verb> y»; if every is
something one thing does to another, say «every is a relation verb».
```

"every" è stato letto come possibile verbo relazionale: il quantificatore ha
deviato il turno fuori dal percorso inferenziale (lo stesso sillogismo **senza**
quantificatori, §2.1, passa). Il connector/quantifier dovrebbe essere vocabolario
KB che *attiva* il meccanismo, non canditato a *relazione*. Gravità ALTA: è la
classe di input che separa un motore di inferenza da un giocattolo.

### 4.4 Il layer conversazionale italiano è molto più debole di quello inglese

Con `PARROT0_LANG=it`:

| input | esito |
|---|---|
| "Ciao, mi chiamo Giulia" | **"Non capisco ancora."** — la presentazione del nome non passa (l'equivalente inglese passa) |
| "Come mi chiamo?" | "Non sono sicuro di aver seguito. Puoi dirlo in un altro modo?" |
| "Dove si trova il Colosseo?" | "Non capisco ancora." (l'equivalente inglese "Where is the red book?" funziona) |
| "Qual è la capitale dell'Australia?" | **"Canberra."** ✅ |
| "Dimmi il tuo nome e dove vivi e che cosa sai fare" | ledger completo delle capacità — ma ignora i primi due sotto-obiettivi (v. §4.5) |
| "Come si prepara un caffè?" | **"I can't show that."** — risposta in inglese in sessione italiana, e di contenuto sbagliato (rifuto di visualizzare, non risposta al come-fare) |

Il pattern è chiaro: la **QA fattuale italiana c'è** (Canberra), il
**conversazionale italiano no** (presentazione, richiamo, locazione). Il
compilatore conosce l'italiano (`clause_copula` ha «è, sono, era, erano», e i
test `.it.p0t` esistono), ma a runtime i percorsi di presentazione/locazione
sembrano leggersi solo l'inglese. È il difetto del mantra #19(a) applicato alla
chat: classi private che non leggono la KB che c'è.

### 4.5 Richieste multi-obiettivo: si risponde a un sotto-obiettivo e gli altri spariscono

"Dimmi il tuo nome e dove vivi e che cosa sai fare" → il ledger delle capacità
(solo il terzo sotto-obiettivo). Il mantra #10 è esplicito: con richieste
coordinate il modulo deve costruire un `answer_plan` **o declinare**. Qui non c'è
stato né l'uno né l'altro: risposta al primo subgoal reclamabile, silenzio sul
resto. Stessa classe in inglese: "My name is Marco and I live in Turin" (§3.4).

### 4.6 L'insegnamento perde le clausole relative e produce doppio output nel turno

```
>>> A wombat is a marsupial that lives in Australia
Learned: wombat is a marsupial. I don't understand that yet.
```

- La clausola relativa ("that lives in Australia") è **caduta**: un fatto su cui
  il prossimo turno vorrà ragionare ("where does the wombat live?") non esiste.
- E il turno produce **due output** — l'ack di apprendimento e un "I don't
  understand that yet." orfano. Che cosa lo ha generato non è chiaro dal
  comportamento (da tracciare), ma l'utente vede un sistema che si contraddice
  dentro una sola risposta.

### 4.7 La facoltà di pianificazione è irraggiungibile in linguaggio naturale

```
>>> I want to make pasta carbonara. What do I need to do first?
Hmm, I don't know about carbonara yet. Want me to learn about it? Or teach me:
if «x to y» says what you already say another way, say «x to y means x <known
verb> y»; …
```

"I want to make X. What do I do first?" è lo shape canonico di un piano
prerequisiti — e il ledger **dichiara** "make simple plans and track prerequisite
structure". Il turno non ci è mai arrivato: "make pasta carbonara" è stato letto
come possibile relazione «x to y». Il divario fra capacità dichiarata e capacità
richiamabile da lingua è esso stesso un difetto misurabile (e questo report lo
misura). Gravità ALTA per coerenza: il ledger è la superficie onesta del sistema,
se promette ciò che la lingua non può raggiungere è una promessa rotta.

---

## 5. 🟡 MEDI — ampiezza e qualità del ragionamento

### 5.1 Il controfattuale riceve un metodo, non un mondo

"What would happen if the sun stopped shining?" → una risposta lunghissima e ben
formata su *come si analizza un controfattuale* (baseline, una condizione, effetti
diretti, adattamenti, chi guadagna/assorbe i costi, dipendenza da scala e orizzonte).
È un **impalcato procedurale senza un grammo di contenuto sul mondo**: nessun sole,
nessuna fotosintesi, nessun freddo. Risposta identica se il controfattuale fosse
sulla marmellata. Due letture possibili: (a) il template KB è un fallback onesto
che evita la confabulazione — meglio di "Ice."; (b) è ragionamento-finto per
forma, l'impostor shape che PRINCIPLES.md chiama *clever impostor*. La cura minima:
se la KB non ha gli archi (sole→luce→fotosintesi), la risposta onesta è breve e
dichiarata ("I can trace the method but I lack the facts about the sun"), non un
ponte d'oro di metodo generico.

### 5.2 I comparativi sono assenti

"Tom baked a cake and gave half to Anna. Who has less cake?" → "I don't
understand that yet." Il confronto fra quantità derivate (mezzo vs mezzo) è una
classe nota del motore (`transitive_comparison` esiste); il shape narrativo non
lo raggiunge. Come per §4.2: il ponte lingua→calcolo è il collo di bottiglia, non
il calcolo.

### 5.3 "Can fish fly?" — muro dove l'KB consentirebbe un no inferito

"I don't understand that yet." Non è ingannevole (bene), ma con fish e fly nella
KB il "No" negativo dovrebbe essere derivabile. Priorità bassa: il muro onesto è
accettabile; lo segnalo perché è la classe *inferenza negativa da assenza* che il
motore `naf` dovrebbe coprire.

### 5.4 Lo shift di lingua delle risposte è instabile

- Sessione default inglese: "hi" → **"Ciao!"**
- Sessione `PARROT0_LANG=it`: "Come si prepara un caffè?" → **"I can't show that."**
- Il resto delle risposte italiane è in italiano coerente.

La lingua della risposta non segue né l'input né `PARROT0_LANG` in modo stabile.
Per un sistema che vuole essere interlocutore è un difetto di credibilità
immediata, anche se meccanicamente minore.

---

## 6. ⚪ BASSI — superficie e affidabilità

1. **Le maiuscole dell'entità trapelano nell'output**: "eiffel tower is about 330
   meters." — la canonicalizzazione abbassa le maiuscole (difetto noto in
   AGENTS.md) e il lowercase arriva fino alla risposta. Cosmetico, ma a lungo
   termine: l'output è conoscenza (mantra #16), quindi anche la superficie
   dell'entità andrebbe ricostruita alla stampa.
2. **I template di offerta hanno difetti grammaticali**: «say «something is a
   eiffel»» (articolo "a" davanti a vocale), «if penguins is a …» (accordo
   plurale). Sono `response_template` in KB: esattamente il posto dove si
   correggono **parlando**, senza toccare il C. Ottima occasione di dimostrare il
   mantra #16 su se stessi.
3. **"No" nudo letto come assenso** ("Got it — what would you like to do?", §3.3):
   rischio interattivo reale, l'utente che nega riceve conferma.
4. **Doppio output nel turno di insegnamento** (§4.6).
5. **Offerte non uniformi** ("look it up" vs "learn about it") con accettazioni
   non uniformi: all'utente non è dato sapere che cosa dire perché il sistema
   accetti.
6. **La seconda frase coordinata non viene letta mai** ("They live in Antarctica."
   dopo la virgola: sparita; "and I live in Turin": deformatasi in nome). Il
   lettore è a una frase per turno nel migliore dei casi.
7. **Affidabilità del fetch irregolare**: platypus ✅, eiffel ✅, wombat ❌
   ("Looking up wombat… I still don't know much about wombat.") nello stesso
   ambiente e sessione. Da tracciare: errore di rete, di parsing della pagina, o
   di selezione della pagina.

---

## 7. Pattern di radice (le tre malattie, non i 22 sintomi)

Tutti i finding sopra collassano in **tre cause strutturali**, che valgono più
dell'elenco:

1. **Le facoltà sono isole.** Conoscenza appena acquisita invisibile al turno dopo
   (§3.5), fatto insegnato interrogabile in sì/no ma non in wh (§3.5 wombat),
   capacità dichiarata ma irraggiungibile da lingua (§4.7), richiesta composta
   spezzata fra percorsi che non si accordano (§3.4, §4.5). È la forma misurata
   del «cassetto senza maniglia» e della lezione D33/D35/D37: *due percorsi che
   devono accordarsi e non condividono l'oggetto su cui accordarsi.*
2. **Il canale di correzione non esiste.** Negazione che perde il complemento
   (§3.2), retrazione assente (§3.3), "No" nudo letto come sì (§3.3), fatti su
   soggetti plurali non memorizzati (§3.3). Finché questo canale è rotto, ogni
   altro progresso è a rischio: il sistema non può migliorare col dialogo, e il
   dialogo è il canale primario del progetto.
3. **Il confine fra "rispondere" e "rispondere bene" non è presidiato.** "Ice."
   (§3.1), "Italy." (§4.1), il ponte d'oro di metodo sul controfattuale (§5.1),
   l'inglese in sessione italiana (§4.4): quattro modi in cui il sistema ha
   preso il turno (mantra #21) senza avere il titolo, e senza che una guardia lo
   ferma. La domanda della review del #21 — *che titolo ha il modulo che risponde?*
   — oggi non trova presa su questi casi.

---

## 8. Conclusioni (determinate)

1. **parrot0 NON è (ancora) intelligente nel senso pieno. È un motore di inferenza
   e acquisizione reale con una pipeline linguistica che perde informazioni
   sistematicamente a ogni frattura dell'input.** Il motore sotto c'è e funziona:
   sillogismi, propagazione dell'insegnato, fetch-and-learn nello stesso turno.
   Ma l'interlocutore che lo usa vede un sistema che sbaglia e non può essere
   corretto — ed è questo, non la copertura della KB, il limite percepito.

2. **Il difetto #1 da chiudere è il canale di correzione (§3.1–3.3), prima di
   qualunque crescita di copertura.** Argomento secco: la tesi del progetto è che
   la conoscenza cresca dal dialogo; un sistema che dopo "No, penguins non vivono
   nell'Arctic, vivono in Antarctica" risponde di nuovo "Ice." non ha il canale
   col quale la tesi si realizza. Ogni nuova facoltà aggiunta sopra questo buco è
   una stanza senza porta.

3. **Il secondo fronte è l'accordo fra percorsi (§7.1)** — la stessa entità sotto
   forme di domanda diverse deve avere una maniglia unica. Il test comportamentale
   è a portata di mano e lo do già formato: *insegnare E, poi chiedere E in tre
   forme (what-is, wh, sì/no): tutte e tre devono passare, e una retrazione deve
   toglierle tutte e tre.*

4. **L'italiano va portato alla parità del conversazionale, non solo della QA.**
   Canberra passa, "mi chiamo" no: la KB ha le copule, i percorsi non le leggono
   (mantra #19a). È lavoro di migrazione, non di creazione.

5. **Il banco di prova di questo report è riutilizzabile: i ~45 turni sono
   deterministici e vanno cricchettati in `.p0t`.** In particolare "Ice.",
   "Learned: penguins do not live.", la sequenza Eiffel learn→ask-height, e il
   ciclo offerta→"yes" sono regressioni pronte. Un difetto riproducibile è un
   difetto già mezzo chiuso.

6. **Cosa NON fare:** non allargare la KB per coprire i prompt di questo report
   (penguins, carbonara, wombat) — sarebbe insegnare all'istanza e non alla classe
   (mantra #1), e i test diventerebbero verdi senza che il canale di correzione
   esista. Le sonde vanno usate per motivare *motori* (negazione con complemento,
   retrazione, ripresa d'offerta), non per riempire fatti.

---

## 9. Ordine di attacco consigliato (per priorità, con la prova di chiusura)

| # | lavoro | chiude | prova di chiusura |
|---|---|---|---|
| 1 | negazione con complemento + retrazione parlando | §3.2, §3.3 | la correzione penguins inverte "Ice."→corretto, e la re-ablazione torna sbagliata |
| 2 | guardia anti-confabulazione sul turno (nessun fatto su X ⇒ muro onesto su X) | §3.1 | "Ice." diventa declinazione; regression `.p0t` verde |
| 3 | maniglia unica dell'entità attraverso le forme di domanda | §3.5 | Eiffel: learn → what-is → how-tall → sì/no, tutti passano nello stesso turno |
| 4 | ripresa d'offerta unica (famiglia di intenti per accept/decline) | §3.6 | "yes", "yes, learn about it", "ok" tutti portano al fetch o declinano insieme |
| 5 | frattura dell'input composto (slot per clausola, answer_plan) | §3.4, §4.5, §4.6 | "My name is Marco and I live in Turin" → nome e città entrambi memorizzati e richiamabili |
| 6 | parità conversazionale italiana | §4.4 | "mi chiamo Giulia" → "Come mi chiamo?" funziona come l'inglese |
| 7 | ponte lingua→slot per numeri e comparativi | §4.2, §5.2, §4.3 | 12 mele − 5; Tom/Anna; "Every dog…" inferenza, non cue |
| 8 | pulizia superficie (lingua di risposta, maiuscole, grammatica dei template) | §6 | corrige insegnato parlando e vale dal turno dopo (mantra #16 dimostrato su sé stesso) |

---

## Appendice A — riproduzioni rapide

```sh
# §3.1 confabulazione
printf 'Do penguins live in the Arctic?\n/quit\n' | \
  PARROT0_SESSION= PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0

# §3.2–3.3 canale di correzione
printf 'Do penguins live in the Arctic?\nNo, penguins do not live in the Arctic. They live in Antarctica.\nDo penguins live in the Arctic?\n/quit\n' | \
  PARROT0_SESSION= PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0

# §3.4 collisione slot composta
printf 'My name is Marco and I live in Turin\nwhat is my name?\nwhere do I live?\n/quit\n' | \
  PARROT0_SESSION= PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0

# §3.5 conoscenza fresca invisibile
printf 'What is the Eiffel Tower?\nHow tall is the Eiffel Tower?\n/quit\n' | \
  PARROT0_SESSION= PARROT0_WIKI_FETCH=1 PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0

# §3.6 ciclo offerta→accettazione
printf 'What is the capital of the country where the Eiffel Tower is located?\nyes, learn about it\n/quit\n' | \
  PARROT0_SESSION= PARROT0_WIKI_FETCH=1 PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0

# §4.4 italiano conversazionale
printf 'Ciao, mi chiamo Giulia\nCome mi chiamo?\nDove si trova il Colosseo?\n/quit\n' | \
  PARROT0_SESSION= PARROT0_LANG=it PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0
```

## Appendice B — inventario dei finding

🔴 CRITICI (7): confabulazione "Ice." · negazione senza complemento · nessuna
retrazione · collisione slot composta + autocontraddizione · conoscenza fresca
invisibile · ciclo offerta→accettazione rotto · (soggetti plurali non
memorizzati).

🟠 ALTI (7): sì/no senza booleano · aritmetica senza ruoli · quantificatori letti
come cue · italiano conversazionale paritario mancante · multi-obiettivo
parziale · clausole relative perse + doppio output · pianificazione
irraggiungibile.

🟡 MEDI (4): controfattuale senza contenuto · comparativi assenti · inferenza
negativa da assenza mancante · lingua di risposta instabile.

⚪ BASSI (7): maiuscole in output · grammatica dei template · "No" nudo come
assenso · doppio output · offerte non uniformi · seconda frase non letta ·
affidabilità fetch irregolare.

— fine report —
