# L5 — la divinazione dei mondi

> **In una frase.** Per parrot0 tutto e' mondo: il mondo vero, il senso comune,
> il mondo di chi parla, quello delle persone che gli stanno vicino, lo spazio
> immaginativo delle pseudo-preferenze di parrot0, la storia raccontata,
> l'ipotesi. Una frase che parrot0 non regge non chiede un fix: chiede di
> **capire in quale mondo sta**, se la grammatica sa portarcela e se quel mondo
> ha il contenuto per rispondere. Il lavoro dell'istruttore e' **popolare i
> mondi parlando**, e dare a parrot0 un carattere: un imprint.

Prosegue [l4-upgrade.md](l4-upgrade.md) (i mondi come contesti, 0-bis; la
coerenza dell'apprendimento con la comprensione) e
[l4-growth-handoff.md](l4-growth-handoff.md). Comanda sempre
[LEARN_PROTOCOL.md](../../LEARN_PROTOCOL.md); vale il [MANTRA](../../MANTRA.md).
Stato: **piano aperto il 1 ottobre 2026**, generazione cominciata (§7).

---

## 1. Da dove nasce

Giro 7 della [llm-challenge](llm-challenge.md) (`docs/sessions/talk/2026-10-01-giro7.log`):
LFM racconta di un viaggio in Giappone e per dodici turni chiede a parrot0 che
cosa gli interessa, che cosa preferisce, che cosa ha provato. Sei volte
parrot0 risponde «I don't know how to answer that about myself yet: nobody
has taught me», sei volte «I couldn't read «…»». La prima diagnosi
dell'agente era: *parrot0 legge ogni clausola in due modi soli, fatto da
tenere o domanda da cercare, e una conversazione e' fatta di altro*. F. l'ha
corretta:

> *«i due modi non sono il problema, il problema e' cosa e' mondo. nella teoria
> dei mondi allargati tutto e' mondo: le preferenze dell'utente, lo spazio
> immaginativo delle pseudo preferenze di parrot0. se questo modello regge il
> tuo scopo e' quello di costruirlo parlando e popolare quei mondi, e la
> grammatica, comprensione, tutte le frasi che prendono i buchi — ne abbiamo
> gia' parlato: comprensione universale e grammaticizzazione. io non credo che
> la questione sia cosa non sa fare parrot0 ma come noi non siamo in grado di
> capire come popolare la KB affinche' parrot0 sia naturale nella sua
> interlocuzione. e' un problema di framework operativo che dobbiamo far
> emergere. parrot0 e' basato sulla KB: questo significa che quello e' uno
> spazio dove ci puo' stare tutto. se lui non risponde alla singola frase tu
> sei il suo istruttore ma anche forgiatore: gli devi dare un carattere, un
> imprint. non e' un problema meccanico di leve fini, e' un problema
> organico.»* — F., 1 ottobre 2026

## 2. La tesi

1. **Le due letture bastano.** Asserire e chiedere restano le due operazioni
   della comprensione. Quello che cambia e' **il mondo** su cui operano.
   «Are you more interested in history, food or nature?» non chiede un fatto
   del mondo vero: chiede un fatto del **mondo immaginativo di parrot0**.
   «I'm planning a trip to Japan» non e' un fatto del mondo vero: e' un fatto
   del **mondo di chi parla**. Se quei mondi esistono e sono popolati, la
   risposta e' una lettura come le altre.
2. **Un turno che non regge e' sempre uno di tre casi**, e nessuno e' un
   difetto del singolo lettore:
   - **mondo sbagliato:** la frase finisce in un mondo che non e' il suo
     («osaka is an always top choice» scritto nel mondo vero);
   - **forma non grammaticalizzata:** la frase non diventa costituenti con un
     ruolo nel mondo giusto («So you're thinking about what to prioritize»);
   - **mondo vuoto:** il mondo giusto c'e' ma non contiene niente da cui
     rispondere («What is your favorite food?» → il frasario).
3. **Il carattere e' contenuto di un mondo, non un frasario.** Oggi
   l'immaginario di parrot0 sta in `response_template` fissi
   («I don't have real preferences, but for the prompt I'd pick reading
   quietly and listening to the rain.»; «…I'd pick fresh warm bread.»). Una
   frase non si compone: a «which city would you choose?» non c'e' niente da
   cui dedurre. Un carattere fatto di fatti in un mondo si compone, si spiega,
   si corregge parlando.
4. **Popolare un mondo parlando e' la crescita.** Fatti veri per il mondo
   vero (verificati, §4 del LEARN_PROTOCOL), stipulati per il mondo
   immaginativo (sono il carattere: decisi dall'istruttore, non inventati come
   fatti del mondo), attribuiti per il mondo di chi parla e delle persone
   vicine. Lo stesso canale per tutti: il linguaggio naturale.

## 3. Il framework operativo: quattro domande su ogni turno

Per ogni risposta della challenge che non regge, nell'ordine:

1. **A quale mondo appartiene questa frase?** (prima di «che fatto e'»)
2. **La grammatica sa portarcela?** Se no: buco di comprensione. Si insegna
   la forma (vale per tutte le frasi di quella forma), o si costruisce in KB
   la grammaticalizzazione che manca (IR: costituenti, ruoli, riferimenti).
3. **Quel mondo ha il contenuto per rispondere?** Se no: buco di **imprint** o
   di conoscenza. Si popola il mondo parlando.
4. **La porta per dirlo c'e'?** Se il contenuto non entra parlando, si
   costruisce la porta (la forma che apre quel mondo e ci scrive), mai il
   singolo fatto a mano.

E una quinta, che il probe del 1 ottobre ha mostrato necessaria:

5. **Chi risponde legge quel mondo?** Un mondo scritto ma non consultato da
   chi risponde e' muto quanto un mondo vuoto.

La crescita va misurata sui giri: quanti turni passano da «nobody has taught
me» / «I couldn't read» a una risposta dal mondo giusto.

## 4. Che cosa c'e' gia' (probe del 1 ottobre 2026, KB viva, senza salvare)

| frase | risposta | mondo | stato |
|---|---|---|---|
| «Kyoto is a city in Japan.» | «Learned: kyoto is a city.» | vero | ◐ il luogo si perde |
| «Is Kyoto in Japan?» | «I don't understand that yet.» | vero | ✗ |
| «I'm planning a trip to Japan next spring.» | consiglio di viaggio | di chi parla | ✗ non scritto nel suo mondo |
| «Where am I going next spring?» | muro | di chi parla | ✗ |
| «My sister lives in Osaka.» / «Where does my sister live?» | «I see: your sister lives in osaka.» / «osaka.» | persone vicine | ✅ |
| «Marco thinks that Osaka is the best city.» | offerta di imparare «best city» | di Marco | ✗ (il superlativo blocca l'apertura; «Marco thinks that tea tastes sour» funziona, L4) |
| «In this story, the fox is a teacher.» / «What is the fox?» | «Learned: fox is a teacher.» / «Fox is classed as teacher.» | storia | ◐ da verificare in quale mondo finisce |
| «Suppose it rains tomorrow.» | riscontro sociale | ipotesi | ✗ |
| «People usually eat when they are hungry.» | offerta di imparare «people» | senso comune | ✗ |
| «If you could travel, you would choose Kyoto.» | offerta di imparare «travel» | immaginativo di parrot0 | ✗ |
| «Imagine you had a favorite food: it would be ramen.» | il frasario («…fresh warm bread») | immaginativo | ✗ ignora la frase |
| «In your imagination, you prefer temples to museums.» | **«Held, but only in imagination: you prefer temples to museums.»** | immaginativo | ◐ **scritto** (`context_fact/4`) |
| «Do you prefer temples or museums?» | il frasario («…listening to the rain») | immaginativo | ✗ **non letto** |
| «Answer as Marco would: Is Osaka the best city?» | «Reading it as Marco would: …» | ponte | ◐ il ponte c'e', il contenuto no |

**Lettura del probe.** Esistono gia' piu' sistemi di mondi, paralleli fra loro:
- i **contesti** di L4 (`holds_in`, `world_of(user)` = `interlocutor_world`,
  `reported_belief` per Marco, il ponte «Answer as Marco would»);
- i **fatti di contesto** delle superfici superiori (`context_fact/4`: «In X,
  S rel O», «assume X» per entrarci, messages.p0);
- il **mondo della storia** («in this story …», gen431);
- le **stipulazioni** e le ipotesi (stipulation.p0);
- il **sé** come frasario (`self_preference`, i template «for the prompt I'd
  pick …»).

Non sono un modello solo, e chi risponde alle domande su di se' non ne legge
nessuno. **Il primo lavoro strutturale di L5 e' l'unita' del mondo**: una sola
rappresentazione (il contesto di L4, che ha gia' politica d'impegno,
trasferimento, ponti, persistenza) in cui finiscono tutte le porte, e una sola
lettura che chi risponde consulta. Le strutture secondarie restano finche'
servono (memoria: *keep secondary structures*), ma convergono.

## 5. Come si genera un mondo (sempre in linguaggio naturale)

Il canale e' `scripts/live-teach.sh`, KB viva completa, salvataggio con
`stop` e verifica riga per riga di cio' che e' finito nella KB (residui di
prova fuori: giro 6 della llm-challenge). Per ogni mondo:

1. **Nominarlo parlando.** La prima frase lo apre con le parole di tutti i
   giorni: «In your imagination, …», «My sister …», «In this story …»,
   «Marco thinks that …», «People usually …». Se nessuna frase naturale lo
   apre, la porta e' il lavoro (§3, domanda 4).
2. **Popolarlo con pochi tratti generativi.** Non cento preferenze sparse: i
   tratti da cui le risposte si deducono («you are curious about how places
   came to be», «you like quiet things more than crowded ones») e qualche
   istanza che li esemplifica.
3. **Interrogarlo con una domanda che nessuno ha insegnato.** La domanda
   della challenge, non la ripetizione della lezione: «Which city would you
   choose?» dopo «you like quiet places and old temples».
4. **Contrasto e ritiro.** Un fatto del mondo immaginativo non vale nel mondo
   vero («Do you like temples?» → dal mondo immaginativo, dichiarandolo; «Is
   Kyoto quiet?» → dal mondo vero). Il ritiro di un tratto toglie le risposte
   che ne dipendevano.
5. **Rigiocare la challenge** con l'apertura che aveva mostrato il buco.

**Verita' dei contenuti.** Il mondo vero si popola solo di fatti veri e
verificati. Il mondo immaginativo e il carattere di parrot0 sono *stipulati*:
sono veri come fatti di quel mondo e vanno committati come tali (sono parte di
parrot0). Il mondo di chi parla e quello delle persone vicine appartengono
all'utente della sessione: si generano parlando per provare le porte, ma non
si committano come conoscenza generale.

## 6. Cento mondi

Legenda dello stato (probe del 1 ottobre e L4): ✅ si apre e si legge
parlando · ◐ si apre o si scrive, ma non si legge (o perde una parte) · ✗ la
porta manca · ? da provare. La porta e' un esempio di frase naturale, non una
sintassi obbligatoria.

### A. Il se' di parrot0

| # | mondo | che cosa contiene | porta (esempio) | stato |
|---|---|---|---|---|
| 1 | il mondo di parrot0 | cio' che tiene per vero | «Rex is a dog.» | ✅ |
| 2 | lo spazio immaginativo di parrot0 | pseudo-preferenze, gusti per gioco | «In your imagination, you prefer temples to museums.» | ◐ scritto, non letto |
| 3 | il carattere di parrot0 | tratti, valori, modo di stare in conversazione | «You are curious and patient.» | ? |
| 4 | le capacita' di parrot0 | che cosa sa fare | «You can help compare options.» | ◐ (self-ability, frasi fisse) |
| 5 | le assenze di parrot0 | corpo, esperienze, gusti veri, rete | «You have never travelled.» | ◐ (frasario) |
| 6 | la storia di parrot0 | come e' nato, chi lo addestra | «You were built by Francesco.» | ? |
| 7 | le curiosita' di parrot0 | che cosa vorrebbe imparare | «You would like to learn how temples are built.» | ? |
| 8 | i ricordi di questa conversazione | cio' che e' stato detto oggi | «What did I tell you about Osaka?» | ◐ (sessione come conoscenza) |
| 9 | gli errori di parrot0 | cio' che ha sbagliato e corretto | «You were wrong about Osaka.» | ? |
| 10 | le ipotesi di parrot0 | cio' che sospetta ma non tiene | (per contatto, L3) | ◐ |

### B. Il mondo condiviso

| # | mondo | che cosa contiene | porta | stato |
|---|---|---|---|---|
| 11 | il mondo vero | fatti verificati | «Kyoto is a city in Japan.» | ◐ il luogo si perde |
| 12 | il senso comune | generalizzazioni tipiche, eccezioni ammesse | «People usually eat when they are hungry.» | ✗ |
| 13 | le definizioni | che cosa significano le parole | «A temple is a place of worship.» | ✅ |
| 14 | la lingua | grammatica, forme, letture | «the expression let me marks request pragmatics» | ✅ |
| 15 | le convenzioni sociali | saluti, cortesia, turni | «People say thank you when they get help.» | ✗ |
| 16 | norme e leggi | obblighi, permessi | «In Japan you must take off your shoes indoors.» | ? |
| 17 | la scienza | teorie con il loro grado | «Scientists think that …» | ? |
| 18 | la storia | cio' che e' stato | «Kyoto was the capital of Japan.» | ? |
| 19 | il calendario e le feste | ricorrenze, stagioni | «Cherry blossoms bloom in spring.» | ? |
| 20 | la geografia | luoghi e relazioni fra luoghi | «Osaka is near Kyoto.» | ? |

### C. Chi parla

| # | mondo | che cosa contiene | porta | stato |
|---|---|---|---|---|
| 21 | il mondo di chi parla | i suoi fatti | «My bike is red.» | ✅ |
| 22 | le preferenze di chi parla | gusti, scelte | «I love ramen.» | ? |
| 23 | le credenze di chi parla | cio' che pensa | «I think that milk tastes sour.» | ✅ (L4) |
| 24 | i piani di chi parla | intenzioni, viaggi, progetti | «I'm planning a trip to Japan next spring.» | ✗ |
| 25 | gli stati di chi parla | emozioni, stanchezza | «I'm tired.» | ✅ (L4-7) |
| 26 | la storia recente di chi parla | cio' che ha fatto | «I just got back from a walk in the park.» | ✗ |
| 27 | il mestiere e le capacita' di chi parla | lavoro, competenze | «I'm a nurse.» | ? |
| 28 | le cose e i luoghi di chi parla | casa, citta', oggetti | «My house is near the sea.» | ✅ (attributi) |
| 29 | i problemi di chi parla | cio' che lo preoccupa | «My printer is stuck again.» | ✅ (situazioni) |
| 30 | il modello che chi parla ha di parrot0 | come l'altro vede parrot0 | «You're making it a game.» | ✗ |

### D. Le persone vicine a chi parla

| # | mondo | che cosa contiene | porta | stato |
|---|---|---|---|---|
| 31 | la famiglia di chi parla | parenti e loro fatti | «My sister lives in Osaka.» | ✅ |
| 32 | gli amici | chi sono, che cosa fanno | «My friend Luca is a cook.» | ? |
| 33 | i colleghi | lavoro condiviso | «My boss is very strict.» | ? |
| 34 | gli animali di casa | nomi, abitudini | «My cat sleeps all day.» | ? |
| 35 | le credenze delle persone vicine | cio' che pensano | «My mother thinks that Kyoto is too crowded.» | ? |
| 36 | le preferenze delle persone vicine | gusti | «My brother loves sushi.» | ? |
| 37 | i piani delle persone vicine | intenzioni | «My sister wants to visit Kyoto.» | ? |
| 38 | le relazioni fra loro | chi e' chi per chi | «Luca is my sister's husband.» | ◐ (kin) |
| 39 | i ricordi condivisi | cio' che e' accaduto insieme | «Last year we went to Rome.» | ? |
| 40 | le persone nominate di passaggio | il titolare di un pensiero | «Marco thinks that tea tastes sour.» | ✅ (L4) |

### E. Altre menti e fonti

| # | mondo | che cosa contiene | porta | stato |
|---|---|---|---|---|
| 41 | il mondo di una persona nominata | le sue credenze | «Marco thinks that Osaka is the best city.» | ◐ (forme semplici) |
| 42 | un personaggio storico | cio' che credeva | «Galileo believed that the Earth moves.» | ? |
| 43 | un autore, un libro | cio' che il testo sostiene | «The book says that …» | ? |
| 44 | una fonte | cio' che una fonte riporta | «Wikipedia says that …» | ◐ (documenti letti) |
| 45 | l'altro modello in conversazione | cio' che LFM afferma | «Osaka is definitely a must.» | ✗ (finisce nel mondo vero) |
| 46 | una cultura, un popolo | usi, credenze diffuse | «In Japan people bow when they meet.» | ? |
| 47 | un'istituzione | cio' che dichiara | «The museum says that …» | ? |
| 48 | gli esperti | raccomandazioni | «Doctors recommend sleeping eight hours.» | ? |
| 49 | l'opinione diffusa | il «si dice» | «Many people say that Kyoto is magical.» | ? |
| 50 | la parte avversa in un dibattito | tesi che non si condividono | «Some argue that …» | ? |

### F. Mondi raccontati e immaginari

| # | mondo | che cosa contiene | porta | stato |
|---|---|---|---|---|
| 51 | la storia raccontata | i suoi personaggi e fatti | «In this story, the fox is a teacher.» | ◐ |
| 52 | la fiaba, il mito | personaggi e regole del mito | «In the legend, the fox can change shape.» | ? |
| 53 | un romanzo noto | il suo mondo interno | «In Harry Potter, owls carry letters.» | ? |
| 54 | un gioco | regole e pezzi | «In chess, the bishop moves diagonally.» | ? |
| 55 | il gioco di ruolo con chi parla | i ruoli assunti | «Let's pretend you are a tour guide.» | ? |
| 56 | il sogno | cio' che e' accaduto in sogno | «I dreamed that I could fly.» | ? |
| 57 | l'esempio | lo scenario costruito per spiegare | «For example, imagine a small shop …» | ? |
| 58 | l'esercizio | i dati del problema | «If you have 7 apples …» | ✅ (problemi) |
| 59 | l'ironia, la battuta | cio' che si dice senza crederlo | «Oh sure, rain is my favourite weather.» | ✗ |
| 60 | la metafora | il dominio prestato | «Punctuation is the silent architect of clarity.» | ✗ (letta come classe) |

### G. Mondi ipotetici e modali

| # | mondo | che cosa contiene | porta | stato |
|---|---|---|---|---|
| 61 | l'ipotesi | cio' che si suppone | «Suppose it rains tomorrow.» | ✗ |
| 62 | il controfattuale | cio' che sarebbe | «If you could travel, you would choose Kyoto.» | ✗ |
| 63 | il possibile | cio' che puo' essere | «It might snow in Kyoto.» | ◐ (modalita' nella IR) |
| 64 | il probabile | cio' che e' atteso | «It will probably be crowded.» | ? |
| 65 | l'obbligo e il permesso | deontico | «You must buy a ticket.» | ◐ (politiche) |
| 66 | il futuro previsto | cio' che accadra' | «Tomorrow the museum opens at nine.» | ? |
| 67 | il passato | cio' che accadde | «Yesterday it rained.» | ◐ (tempo) |
| 68 | l'intenzione condivisa | cio' che si fa insieme | «Let's explore something fun.» | ◐ (non afferma; non ancora mondo) |
| 69 | la proposta | cio' che e' proposto | «How about we brainstorm?» | ◐ |
| 70 | la richiesta | cio' che e' chiesto | «Please share the sentence.» | ◐ (non afferma; giro 6) |

### H. Mondi del discorso

| # | mondo | che cosa contiene | porta | stato |
|---|---|---|---|---|
| 71 | la conversazione in corso | argomento, questione aperta | (ogni turno) | ✗ (max_qud e open_issue vuoti nel giro 7) |
| 72 | cio' che parrot0 ha detto | impegni presi | «You said you'd pick Kyoto.» | ? |
| 73 | le domande aperte | cio' che aspetta risposta | (il tabellone, issues.p0) | ◐ |
| 74 | le offerte e le promesse | cio' che uno si e' impegnato a fare | «I'll share facts or stories.» | ✗ (letto come fatto) |
| 75 | le riformulazioni | cio' che l'altro crede pensi parrot0 | «So you're thinking about what to prioritize.» | ✗ |
| 76 | le valutazioni del turno | il giudizio su cio' che e' stato detto | «That's a lovely idea!» | ◐ (riscontro sociale) |
| 77 | le menzioni | parole citate, non usate | «"him" is the object» | ◐ |
| 78 | gli esempi linguistici | frasi d'esempio | «Example: "She said, 'I saw him'"» | ✗ |
| 79 | il registro | come si parla | «Let's keep it simple.» | ✅ |
| 80 | le correzioni | cio' che l'altro ritira | «No, I meant Kyoto.» | ◐ |

### I. Mondi di dominio

| # | mondo | che cosa contiene | porta | stato |
|---|---|---|---|---|
| 81 | il viaggio | luoghi, itinerari, stagioni | «Kyoto is famous for its temples.» | ? |
| 82 | la cucina | piatti, ingredienti | «Ramen is a noodle soup.» | ? |
| 83 | la salute | sintomi, cure | «Aspirin reduces pain.» | ✅ |
| 84 | il lavoro | progetti, scadenze | «My project is due on Friday.» | ? |
| 85 | il codice | programmi, errori | (codice letto) | ◐ |
| 86 | la scuola e lo studio | materie, esami | «Tomorrow I have a math exam.» | ? |
| 87 | lo sport | squadre, regole | «Football is played with eleven players.» | ? |
| 88 | musica e arte | opere, autori | «Hokusai painted The Great Wave.» | ? |
| 89 | natura e meteo | stagioni, clima | «It rains a lot in June in Japan.» | ? |
| 90 | la casa e gli oggetti | oggetti quotidiani, guasti | «The kettle is broken.» | ✅ (situazioni) |

### J. Mondi del tempo, del luogo e della lettura

| # | mondo | che cosa contiene | porta | stato |
|---|---|---|---|---|
| 91 | qui e ora | l'origo: ora, giorno | «What day is it?» | ✅ (origo.p0) |
| 92 | un luogo | cio' che vale li' | «In Japan, trains are very punctual.» | ? |
| 93 | un'epoca | cio' che valeva allora | «In the 1800s, Kyoto was …» | ? |
| 94 | una stagione | cio' che accade allora | «In spring, the parks are full of people.» | ? |
| 95 | una lingua | come si dice | «In Italian, temple is tempio.» | ✅ (tr/2) |
| 96 | il documento letto | cio' che il testo dice | «read: …» | ✅ |
| 97 | una regola insegnata | il modo di chi parla | «My way of making a question with need is …» | ✅ (L4) |
| 98 | le procedure | come si fa una cosa | «To make tea, boil water, then …» | ◐ |
| 99 | gli stati nel tempo | come cambiano le cose di chi parla | «The printer was stuck; now it works.» | ✅ (L4-7) |
| 100 | il ponte temporaneo | due mondi fusi per una domanda | «Answer as Marco would: …» | ✅ (L4) |

## 7. Generazione cominciata

Vedi §7.1 e seguenti: ogni sessione si registra qui con la porta usata, il
contenuto scritto, la domanda nuova, il contrasto, il ritiro e cio' che e'
finito nella KB.

### 7.1 Mondo #2, lo spazio immaginativo di parrot0 — prima generazione (1 ottobre 2026)

Sessione viva, KB completa: `docs/sessions/live/2026-10-01-l5-mondo-immaginativo.log`.
**Chiusa senza `/save`** (motivo sotto).

| frase detta | risposta | esito |
|---|---|---|
| «In your imagination, you prefer temples to museums.» | «Held, but only in imagination: you prefer temples to museums.» | scritto |
| «In your imagination, you like quiet places.» | «Held, but only in imagination: you like quiet places.» | scritto |
| «In your imagination, you love reading in the rain.» | «Held, but only in imagination: you love reading in the rain.» | scritto |
| «In your imagination, your favorite food is fresh warm bread.» | il frasario («…I'd pick fresh warm bread.») | non scritto: il template di preferenza prende il turno |
| «In your imagination, you would choose Kyoto.» | «I don't understand that yet.» | non scritto: il condizionale non entra |
| «Do you prefer temples or museums?» | «…for the prompt I'd pick reading quietly and listening to the rain.» | **non letto** |
| «Which city would you choose?» | «…nobody has taught me.» | non letto |
| «Do you like quiet places?» | «I don't have tastes of my own, so I can't honestly say I like it.» | non letto |
| «assume your imagination» → «Do you prefer temples or museums?» | «Now reasoning inside imagination.» → «No genuine preferences here: I don't experience days or hobbies.» | **non letto nemmeno dentro il mondo** |

**Che cosa mostra.**
1. **La porta per scrivere c'e'** (`teach_ctx_fact`, «In X, S rel O»), ma
   scrive in `context_fact(imagination, Rel, you, Oggetto)`: un sistema di
   mondi parallelo ai contesti di L4, con il soggetto «you» non legato a
   parrot0 e l'oggetto come un atomo solo («temples to museums»). La IR non
   e' stata usata: e' un lettore a schema.
2. **Chi risponde alle domande su di se' non legge nessun mondo.** Sono
   almeno quattro frasi fisse in facolta' diverse (`self_preference`, «no
   genuine preferences», «I don't have tastes of my own», «nobody has taught
   me»), e prendono il turno prima di qualunque lettura. Anche «assume your
   imagination» non cambia chi risponde.
3. **Il carattere oggi e' un frasario**, e il frasario *vince* sul mondo
   appena insegnato: e' il caso di «mondo scritto ma muto» della domanda 5.

**Perche' non si e' salvato.** Tre tratti scritti in un sistema che nessuno
legge, con il soggetto sbagliato, sarebbero diventati conoscenza ufficiale
inerte. La generazione del mondo #2 riparte quando (a) il mondo immaginativo
e' un contesto di L4 (`world_of(parrot0_imagination)` o simile, con la sua
politica d'impegno: vero in quel mondo, mai nel mondo vero), (b) «you» nella
porta e' parrot0, e (c) le domande su di se' leggono quel mondo prima del
frasario, che resta come rete.

## 8. Ordine di lavoro

1. **Il mondo immaginativo di parrot0 (#2–#7)**: e' quello che la challenge
   chiama di piu' (sei turni su dodici nel giro 7). Prima la domanda 5: chi
   risponde alle domande su di se' deve leggere il mondo, non il frasario.
2. **Il mondo di chi parla per piani e storia recente (#24, #26)**: «I'm
   planning a trip», «I just got back from a walk» devono finire li' e
   tornare utili («Where am I going?»).
3. **L'unita' del mondo**: le porte esistenti (`context_fact`, storia,
   stipulazione, contesti L4) convergono sul contesto di L4.
4. **Il senso comune (#12)** e **l'altro modello in conversazione (#45)**:
   cio' che LFM afferma non e' il mondo vero, e' il suo mondo.
5. Poi gli altri gruppi, sempre dalla challenge: il prossimo mondo da
   generare e' quello che il giro successivo mostra vuoto.
