# Train the Learning Process — far crescere la capacità di essere addestrato

## ⛔ PRIORITARIO — gli insegnamenti che falliscono (F., 26 settembre 2026)

*F.: «tutti gli insegnamenti che falliscono mettili come prioritari in testa a
questo file». Cinque blocchi, dal piu' recente: il tedesco insegnato dal vivo
(qui sotto, §T), la presa sull'interlocutore (§0), la grammatica inglese
insegnata come regole (§1), la meccanica
di precisione (100 lezioni, sezione successiva), il debug PHP (P1–P14, piu'
sotto). Da qui si riparte.*

### T. Una lingua sconosciuta: il tedesco insegnato dal vivo (26 settembre 2026, notte)

Richiesta di F.: insegnare a parrot0 una lingua che non conosce, con una
grammatica complessa, e giudicare il potenziale. Protocollo di
[live-teaching.md](live-teaching.md), inglese come lingua di servizio, tedesco
come lingua messa alla prova, conoscenza vera della KB. Transcript
`docs/sessions/live/2026-09-26-tedesco.log`. **Chiusa senza `/save`** (fatti
spazzatura dal V2 e dalla negazione).

**Che cosa ha imparato davvero:**
- **Tre verbi per contatto (L3), ciascuno da una sola frase naturale** e al
  primo colpo: «A dog eats meat, so it frisst meat.» → «Der Hund frisst
  Fleisch.» si legge («Reading «frisst» as «eats»»), «What frisst meat?» →
  dog; lo stesso per «produziert» (produces) e «pumpt» (pumps), con il
  trasferimento a domande mai dette.
- **Nomi per identità**: «Hund and dog name the same thing» (e Fleisch/meat,
  Heute/today) entrano nel turno canonico; «Is a Hund an animal?» → Yes,
  ereditando ciò che la KB sa del cane.
- «"kein" is a negation marker» e «another form of frisst is fressen» vengono
  accettate (con gli esiti sotto).

**Appunto di F. durante la sessione:** *«stiamo capendo che lui potrebbe
parlare il tedesco senza sapere che sta parlando il tedesco»*. Confermato: le
parole entrano come **letture di relazioni inglesi**, non come parole di una
lingua. «Do you speak German?» → «No», «What German words do you know?» →
chiacchiera, e «What does frisst mean?» → «Reading «frisst» as «eats». I don't
know yet what «frisst» means» (autocontraddizione nella stessa risposta).

**Le lezioni e i turni che falliscono, per specie (dalla più pesante):**

| # | specie | turno o lezione | risposta | che cosa manca |
|---|---|---|---|---|
| DE1 | **⛔ fatto falso dall'ordine V2** | «Heute frisst der Hund Fleisch.» (anche dopo «Heute and today name the same thing») | «Learned: heute eats der dog meat.» / «Learned: today frisst der dog meat.» | il verbo in seconda posizione con un complemento in testa: il lettore dei frame prende come soggetto ciò che precede il verbo. L'ordine è una regola di posizione e nessuna lezione la raggiunge («In German, the verb comes second in a statement.» → «don't know about verb comes second») |
| DE2 | **⛔ negazione persa, fatto positivo** | «Der Hund frisst kein Gras.» | «Learned: der dog eats kein gras.» | «kein» non è una negazione; dopo «"kein" is a negation marker» la frase si archivia come testo («Learned: der Hund frisst kein Gras.»), non come negativo («Does a dog eat grass?» → non so) |
| DE3 | **⛔ riconoscimento falso della regola (bug di L4, CORRETTO)** | «To make a question in German, put the verb before the subject.» | «It matches a rule I already use: I ask questions by putting «the» before the subject.» | l'allineamento prendeva qualunque parola prima di «before». Corretto dopo la sessione: ciò che si colloca è la fila subito prima della parola d'ordine (`lesson_placed/3`, language-lessons.p0); cricchetto in `lesson_meets_rule.p0t` |
| DE4 | **le letture «X means Y» confermate e non applicate** | «frisst means eats» · «der means the» · «Hund means dog» | «From now on I'll read «frisst» as «eats»», poi la frase mura su «frisst»; il canone è «der dog frisst meat» | le identità «name the same thing» entrano nel turno canonico, le letture «means» no; e la lettura cattura il punto finale («dog.») |
| DE5 | **la polare tedesca non si legge** | «Frisst der Hund Fleisch?» | muro | in tedesco il verbo pieno va in testa: `question_inversion` deriva solo da ausiliari e forme dichiarative; una lezione che lo dica non ha forme tedesche da cui derivare |
| DE6 | **le parole funzione tedesche non entrano** | «Was and what name the same thing.» · «Der and the name the same thing.» · «"der" is an article» | «don't know about same thing» · «I know article with 4 arguments, so I cannot use it as a class.» | il glossario è solo italiano («the italian for X is Y»; «the german for dog is Hund» → muro); «Was»/«Der» si leggono come copula e articolo inglesi; «article» non è nominabile come classe |
| DE7 | **il nome di relazione per contatto non raggiunge la domanda** | «The capital of Germany is Berlin, so Berlin is its Hauptstadt.» (due episodi, anche Vienna) | poi «What is the Hauptstadt of France?» → «Reading «hauptstadt» as «is the capital of». I don't know about hauptstadt.» | l'episodio nasce e la qualifica lo dice, ma la domanda «the N of X» non passa dal ponte; e la glossa detta è la cornice («is the capital of»), non il nome |
| DE8 | **morfologia: la forma nuova sbaglia il tempo** | «another form of frisst is fressen» · «Dogs eat meat, so they fressen meat.» · «Die Hunde fressen Fleisch.» | «Reading «fressen» as «ate»» e muro | la forma entra come forma irregolare di una radice inglese e prende il passato; accordo di numero e plurale tedesco (Hund/Hunde) assenti |
| DE9 | **l'articolo entra nel soggetto** | «Der Hund frisst Fleisch.» dopo il contatto | «Learned: der dog eats meat.» | nessuna via per dire che «der» è un articolo (DE6), quindi il sintagma lo ingloba |
| DE10 | **nessuna nozione di lingua** | «Do you speak German?» · «What German words do you know?» · «Is frisst a German word?» | «No…» · chiacchiera · «I cannot settle that…» | le parole apprese non portano la lingua a cui appartengono; `language_word(de, german)` esiste ma nessuna lezione la collega |
| DE11 | **genitivo non scomposto** | «Germany's capital is Berlin, so Berlin is its Hauptstadt.» | «Hmm, I don't know about germany's yet.» | il clitico «'s» si insegna (L3 §29) ma non è insegnato di default |

**Lettura d'insieme.** Il contatto (L3) è un canale di apprendimento lessicale
**reale e veloce**: una frase, un verbo, trasferito subito a domande nuove.
Si ferma però al lessico mappato su relazioni che la KB tiene già in inglese.
Tutto ciò che è **grammatica tedesca** (ordine V2, verbo in testa nella
domanda, negazione con «kein», articoli e casi, accordo e plurale) o non si
raggiunge, o produce fatti falsi. E manca la nozione stessa di «una lingua»
cui le parole appartengono. È il confine di L4: le regole che la comprensione
usa non si possono ancora modificare dall'esterno per una lingua nuova.

### 0. Reattività e presa sull'interlocutore — sessione live in italiano (26 settembre 2026, sera)

Richiesta di F.: *«addestrare parrot0 a essere più reattivo e utile e a non
abbandonare mai la presa con il suo interlocutore»*. Protocollo di
[live-teaching.md](live-teaching.md): un processo, KB `agi` completa, solo
lingua naturale. Transcript `docs/sessions/live/2026-09-26-presa-it.log`; un
primo giro in inglese, chiuso su richiesta di F. per rifarlo in italiano, è in
`docs/sessions/live/2026-09-26-presa-en.log`. **Chiusa senza `/save`:** ciò che
è entrato era spazzatura (frasi intere archiviate come fatti) o senza effetto.

**Che cosa ha retto** (la presa esiste, ma su poche superfici):
- «Il mandrino **del mio** tornio fa un rumore strano.» → «Vediamolo insieme. Di
  che cosa si tratta, e che cosa succede esattamente?». Il piano di
  `user_problem` era già salvato; con il possessivo la situazione scatta.
- La risposta dell'interlocutore riceve un primo seguito: «Grazie, questo aiuta.
  Da quando succede, ed è cambiato qualcosa appena prima?».
- «In cosa puoi aiutarmi?» → l'elenco delle capacità chiuso da «Di che cosa hai
  bisogno adesso?».
- «Ho bisogno di aiuto con la fresa.» → «Partiamo da un risultato osservabile…».

**Le lezioni e i turni che falliscono, per specie (dalla più pesante):**

| # | specie | turno o lezione | risposta | che cosa manca |
|---|---|---|---|---|
| PR1 | **⛔ conferma falsa, lezione senza effetto** | «"mi dai una mano" is another way to say "in cosa puoi aiutarmi"» | «Got it - «mi dai una mano» asks for the same thing as …», poi «Mi dai una mano?» → «Non capisco ancora.» | l'alias `intent_cue` punta a una frase che non è un intento (la risposta sulle capacità è la situazione `user_help_scope`): la lezione dovrebbe accorgersi che il bersaglio non porta a niente, invece di confermare |
| PR2 | **⛔ conferma falsa, fatto spazzatura** | «quando non capisci allora chiedi che cosa succede» · «quando non capisci una domanda allora chiedi di dirla in un altro modo» | «Imparato: quando non capisci allora chiedi che cosa succede.» | la situazione «non capisci» non esiste; la frase intera diventa un fatto. La condotta davanti al proprio muro, cioè il cuore del «non mollare la presa», **non è insegnabile** (catalogo P39 «if you are not sure, …» rosso: «if you are not sure, ask what they mean» → «Fair enough — tell me where I went wrong…») |
| PR3 | **la presa si perde dopo un solo seguito** | dopo «Da quando succede…?»: «È un rumore metallico, si sente quando il mandrino gira veloce.» → «Non capisco ancora.»; «Cosa potrebbe essere?» → muro; «E adesso cosa faccio?» → muro | tre muri di fila mentre l'interlocutore chiede aiuto concreto | `situation_followup/2` ha un solo anello (`user_problem` → `user_problem_detail`) e nessuna forma lo allunga parlando; «che cosa potrebbe essere?» e «che faccio?» dentro una situazione aperta non hanno lettore |
| PR4 | **la situazione nuova non si insegna** | «"mi serve una mano" significa la stessa situazione di qualcuno ha un problema» · la stessa con «means the same situation as» | «Ho trovato il separatore didattico, ma non riesco ad allineare…» / «I found the teaching pivot…» | `teach_situation_cue` non arriva mai: il lettore delle costruzioni (L2) prende prima ogni turno con «means»/«significa». Nessuna superficie italiana |
| PR5 | **lezione in italiano non letta** | «quando qualcuno chiede aiuto allora chiedi qual è il problema» | «Leggo «chiedi» come «chiede». Ehilà! Sono qui…» (chiacchiera) | la forma dei piani vuole una situazione già nota; se non la trova non lo dice, e la prende lo smalltalk. Con la virgola al posto di «then»/«allora» (come parla una persona) non entra nemmeno la forma inglese |
| PR6 | **alias italiano con effetto sbagliato** | «"mi aiuti" è un altro modo per dire "in cosa puoi aiutarmi"» (senza virgolette: «va un po' oltre le mie capacita'») | la lezione risponde «Non capisco ancora», eppure dopo «Mi aiuti?» passa da muro a «Non ho afferrato bene. Cosa vorresti sapere?» | la lezione cambia qualcosa senza dirlo, e non ciò che è stato insegnato |
| PR7 | **un guasto riferito diventa un fatto** | «Il mandrino fa un rumore strano.» (senza possessivo) · «Ciao, mi serve una mano con il mio tornio.» | «Imparato: fa(mandrino, noise_strano).» (sintassi interna esposta) · «Learned 6 prerequisites for mi.» | una segnalazione non è una lezione; il lettore dei prerequisiti impara da un saluto |
| PR8 | **risposta nella lingua sbagliata** | «Ciao, mi serve una mano con il tornio.» dopo due lezioni in inglese | «Thanks, that helps. Since when has it been happening…» | la lingua della risposta segue la lingua appiccicosa dei turni precedenti, non quella del turno |
| PR9 | **affermazione più domanda rifiutata** | «Il mio tornio fa un rumore metallico. Cosa potrebbe essere?» | «Quel turno unisce piu' affermazioni e una domanda… Dimmi le affermazioni una per turno» | un interlocutore reale parla così; il lettore composto non regge dichiarativa + domanda |
| PR10 | **domanda che non raggiunge un fatto vero** | «What is the safe internal temperature of chicken?» (KB: `safe_internal_temp(chicken, 74)`) · «Qual è la temperatura interna sicura del pollo?» | la definizione di temperatura (**risposta sbagliata**, mantra #7) | nessun lettore lega «safe internal temperature» alla relazione; nessuna lezione parlata sa farlo senza il nome interno |
| PR11 | **muri sul campo con fatti veri** | «Per quanto tempo il riso cotto può stare fuori dal frigo?» · «E il pollo?» · «A che temperatura va cotto il pollo?» · «How long can cooked rice stay out?» | «va un po' oltre…», «Non capisco ancora», «don't know about cooked» | `max_hours_out`, `safe_internal_temp` non raggiunti; le ellissi («E il pollo?») non ereditano la domanda precedente |
| PR12 | **muri su richieste d'aiuto comuni** | «Mi aiuti?» · «Mi dai una mano?» · «Non so cosa sia un cuscinetto.» · «Cosa devo fare?» | «Non capisco ancora.» · «Non so ancora tradurre «devo»…» | nessuna mossa di ripiego che tenga il filo: un muro che non chiede niente lascia cadere l'interlocutore |
| PR13 | **«qual è il tuo piano quando …?» in italiano** | «qual è il tuo piano quando qualcuno ha un problema?» | «Non è una cosa che faccio io, ma dimmi -- e tu?» | esiste solo la forma inglese «your plan when …?»: lo smalltalk prende la domanda |
| PR14 | **residuo di resa** | «ok» (senza proposta aperta) · «Perché?» | «"Non ho una proposta attiva da continuare…» | una virgoletta spuria in testa alla frase |

**La lettura d'insieme.** La presa esiste solo dove qualcuno l'ha scritta prima
(la situazione `user_problem` con le sue mosse e un seguito). La condotta che
serve a **non mollare mai**, cioè che cosa fare davanti al proprio muro, dopo
il primo seguito, davanti a una richiesta d'aiuto nuova, non si può ancora
insegnare parlando: le tre porte che dovrebbero aprirla (situazione nuova,
piano su una situazione nuova, condotta sull'incertezza) sono chiuse o rubate.
Peggio, due lezioni su tre **dicono di aver capito** e non cambiano niente
(PR1, PR2): per un apprendimento coerente (L4 §2.5, C3) è la rottura più grave,
perché chi insegna crede di aver finito.

**Da dove ripartire** (in ordine): PR2 e PR1, cioè una lezione non deve
confermare ciò che non produce effetto (è lo stesso principio del «already» del
§0 di L4); poi la porta delle situazioni nuove (PR4, PR5), senza la quale
nessuna condotta nuova entra; poi la catena dei seguiti (PR3) come conoscenza
insegnabile; poi la mossa di ripiego sul muro (PR12).

**Il divario italiano/inglese ha un piano suo** (F., stessa sera):
[piano-b-traduzione.md](piano-b-traduzione.md). Quando un turno nella lingua
della sessione non è capito, parrot0 si chiede se sa tradurre la frase intera;
se sì la rilegge in inglese e risponde in italiano dicendo come; se no chiede
ciò che manca. È un **piano di situazione insegnabile** (situazione, mosse e
ordine si insegnano, correggono e ritirano parlando; il motore offre solo
`reread(…)`), non un comportamento del motore. PR8, PR11 e PR12 sono il primo
banco dello specchio (B0).

### 1. Grammatica inglese INSEGNATA come regole — studio differenziale

Sessione live `docs/sessions/live/2026-09-26-grammatica.log` (chiusa **senza**
`/save`: i fatti entrati erano spazzatura). Ogni regola e' stata detta come la
direbbe un insegnante («To make a yes-no question with the verb be, put the verb
before the subject.»), con una sonda di comportamento prima e dopo (frasi diverse,
stessa regola) e una domanda che chiede di DIRE la regola. Accanto, che cosa la KB
viva implementa gia' (mappa verificata sul codice; `G` = kb/core/grammar.p0, `EG` =
kb/core/english-grammar/, `MK` = src/brain/10-memory-knowledge.c).

**Esito: 0 regole su 25 lette come regole.** La lezione in prosa di una regola
grammaticale non ha nessun lettore: finisce in un muro («I don't know about
singular noun»), in un'altra facolta' («I understood the request — produce …», «That
looks like a snippet of code», «We can chat in either language»), o diventa un
**fatto spazzatura** («put do does», «short adjectives make comparative», «english
sentence puts the subject first») — e uno di questi ha gia' contaminato una domanda
estranea: «What needs grease?» → **«short adjectives.»** (risposta sbagliata). Nessuna
regola e' nominabile: «how do you make a question?» non ha risposta. Tutte le
superfici insegnabili che esistono lavorano per **esempi** (un plurale, un passato,
un lemma, una contrazione), mai per **descrizione** di una regola.

| # | regola detta | la KB la ha gia'? | comportamento PRIMA | che cosa ha preso la lezione | DOPO | che cosa serve |
|---|---|---|---|---|---|---|
| G1 | domanda si/no con be: il verbo prima del soggetto | si': `polar_opener`, `polar_fronted` (G:318, G:3344) | **no** con aggettivo: «The spindle is worn. / Is the spindle worn?» → «don't know about spindle worn» | «I understood the request — produce …» (letta come richiesta di produzione) | no | la polare con predicato aggettivale; la regola non ha forma |
| G2 | domanda con can: il modale prima del soggetto | parziale: `modal_auxiliary` (G:1569), ma `polar_question_verb` esclude i modali (G:1573) | si' («Can a lathe cut threads?» → Yes) | letta come richiesta di produzione | si' | la regola era gia' operante; la lezione non lo riconosce |
| G3 | do/does/did prima del soggetto, verbo nudo | si': `bare_verb_auxiliary`, `polar_question_verb` (G:1527–1577) | si' (anche con «did» e il passato irregolare) | fatto spazzatura «put do does» | si' | sovrapposizione non riconosciuta; la lezione sporca la KB |
| G4 | negazione: not dopo l'ausiliare | si': `negation_marker`, `inability_marker` (lexicon.p0) | si' («cannot measure weight» → No) | letta come richiesta di produzione | si' | idem G2 |
| G5 | this con il singolare, these con il plurale | **no**: i dimostrativi hanno solo la distanza (`noun_feature(…, distance, …)`, EG/nominals.p0:468), non il numero | non verificabile: «Is "these lathe" correct?» → «No.» a tutto | muro «singular noun» | — | tratto di NUMERO sui dimostrativi e una regola di accordo determinante-nome |
| G6 | that/those | come G5 | come G5 | muro | — | come G5 |
| G7 | terza persona singolare: -s | si' nella GENERAZIONE delle forme (`EG/reading.p0:18–31`), non come controllo | non verificabile (il «No.» a tutto) | «Held, but only in present: simple add s …» (letta come fatto con un qualificatore di tempo!) | — | un giudizio di accordo verbo-soggetto oltre la copula (`grammar-judgement.p0` copre solo nome + is/are) |
| G8 | soggetto plurale, verbo senza -s | solo per la copula (`agreement_error`, grammar-judgement.p0:48) | non verificabile | muro «plural» | — | come G7 |
| G9 | a/an secondo il SUONO | **si', ma sbagliata**: `indefinite_article_before(en, u, "an")` (G:3887) e il C passa **una lettera** (`p0_indef_article`, MK:9635) | **sbaglia**: «puppo is an universal quantifier», «zork is an unique tool» | «I couldn't read …» | sbaglia | il C deve chiedere alla KB per parola (`article_for(Lang, Word, Art)`); poi la regola del suono si insegna |
| G10 | plurale: consonante + y → -ies | analisi si' (`plural_suffix`, G:1109), **generazione no** (`count_plural` fa solo +s, G:4126) | «What is the plural of foundry?» → «don't know about plural» | muro «noun ends» | no | la domanda sul plurale e la generazione per regola |
| G11 | plurale: -es dopo s, x, ch, sh | come G10 | come G10 | muro | no | come G10 |
| G12 | passato regolare: -ed | si': `regular_past_form` con raddoppio (EG/reading.p0:33) | la lettura si', la domanda «What is the past of hone?» no | «That looks like a snippet of code.» | no | la domanda metalinguistica sulle forme verbali |
| G13 | comparativo: -er / more | **no**: solo liste (`comparative_more`, lexicon.p0:3148) | «What is the comparative of hard?» → no | fatto spazzatura «short adjectives make comparative» | no | una regola di formazione del comparativo |
| G14 | than introduce il termine di confronto | parziale: `comparative_frame("@S is ", " than @O")` (G:2147) | «Which is harder, steel or aluminium?» → «don't know about harder» | muro «introduces» | no | la domanda di scelta sul comparativo |
| G15 | 's indica possesso | parziale: `word_clitic` vuoto di default; «'s is a clitic» si insegna (RI) | «The lathe's chuck is worn. / What is worn?» → no | muro «apostrophe» | no | l'asserzione con il genitivo (oggi «france s capital») |
| G16 | much con i non numerabili, many con i numerabili | parziale: la numerabilita' c'e' (`noun_feature(…, countability, …)`), nessun controllo much/many | non verificabile | muro «countable» | — | la regola di accordo quantificatore-numerabilita' |
| G17 | it per le cose (he, she) | parziale: `entity_pronoun`, **nessun genere/animatezza** (`resolve_entity` prende l'ultimo, MK:2663) | «The lathe is old. It needs oil. What needs oil?» → no | «I'm not sure I followed» | no | coreferenza con tratti; la lettura della seconda frase con «it» |
| G18 | they per i plurali | come G17 | «What needs grease?» → **«short adjectives.»** (sbagliata, dal fatto spazzatura di G13) | muro «plural noun» | sbagliata | come G17; e il fatto spazzatura di G13 non doveva entrare |
| G19 | who sul soggetto senza do | si': `subject_question_form` (G:1370) | no per «invented» sconosciuto, si' dopo (non per la lezione) | muro «question» | si' | la regola era gia' operante; la lezione non e' stata letta |
| G20 | presente progressivo: be + -ing | si' nell'IR (`auxiliary_chain`, EG/reading.p0:217) | «What is the machinist turning?» → «I looked up «turning»» | muro «present continuous» | no | la domanda sul progressivo |
| G21 | passivo: be + participio, agente dopo by | si' (G:2189–2260) | si' («Who turned the shaft?» → Machinist) | muro «agent comes» | si' | gia' operante; la lezione non lo riconosce |
| G22 | aggettivo prima del nome | **no** come regola (i modificatori si assorbono nel sintagma) | si' nell'uso («What causes noise?» → Worn bearing) | «We can chat in either language» (dirottata) | si' | gia' operante per l'uso; la lezione e' dirottata dal lettore della lingua |
| G23 | ordine soggetto-verbo-oggetto | implicito nei frame | si' | fatto spazzatura «english sentence puts the subject first» | no («grips» sconosciuto) | idem G3: sovrapposizione non riconosciuta |
| G24 | some nelle affermative, any nelle domande | **no** | non verificabile | muro «positive» | — | la regola some/any |
| G25 | avverbi in -ly dagli aggettivi | **no** (la classe avverbio non e' interrogabile) | «Is quickly an adverb?» → «don't know about adverb» | «I can't hold … I only keep what is true of every member of a kind» (il rifiuto piu' onesto del giro) | no | la classe `adverb` nominabile e una regola di formazione |

**Il differenziale in tre righe.** (1) Otto regole sono **gia' operanti nell'uso**
(G2, G3, G4, G19, G21, G22, G23 e G1 per il be+nome): la KB le implementa, ma la
lezione non lo riconosce — nessuna dice «I already know» — e tre la sporcano con un
fatto spazzatura. (2) Una regola e' **implementata male** (G9, l'articolo per
lettera): la lezione giusta non puo' correggerla perche' il C passa una lettera sola.
(3) Le altre sedici **mancano** come regola o come domanda: la crescita per
apprendimento richiede prima una forma che legga una regola DESCRITTA — nessuna
delle superfici attuali lo fa (lavorano per esempi) — e una faccia nominabile per
chiederla («how do you make …?», «is X correct?» oltre la copula).

**Il giudizio di correttezza** esiste solo per l'accordo nome-copula
(`kb/core/grammar-judgement.p0`: «is this correct: my name are Francesco» → spiega
l'errore); su tutto il resto «Is "…" correct?» risponde «No.» anche a una frase
giusta («do you have some bolts»): e' un «no» che nessuna regola ha guadagnato.

### Indice degli altri insegnamenti falliti, da riprendere

- **Meccanica di precisione** — 100 lezioni con problemi, una per riga, nella
  sezione subito sotto; le specie piu' pesanti: procedure non componibili (il passo
  `apply` saltato), non invertibili, la contaminazione del contatto sui nomi composti.
- **Debug PHP** — P1–P14, piu' sotto; aperti P3, P4, P6, P8, parziali P1, P2, P11, P12.


## 🟠 FORME D'INSEGNAMENTO CHE NON FUNZIONANO — sessione live «meccanica di precisione» (26 settembre 2026, pomeriggio)

Sessione di 231 lezioni vere, fermata alla **centesima lezione con problemi** come
chiesto da F. Lista completa (lezione, risposta, verifica, specie) in
[docs/sessions/2026-09-26-live-meccanica.md](../sessions/2026-09-26-live-meccanica.md);
transcript `docs/sessions/live/2026-09-26-meccanica.log`; KB curata in
`kb/facts/precision-mechanics.p0`.

**Coefficiente imparate / con problemi: 131 / 100 = 1,31** — conoscenza 131 / 62
(2,11), procedure e regole profonde 0 / 38. Ordine d'insegnamento (F.): prima L3
per contatto su famiglie di relazione, poi le altre forme a priorita' scendente,
infine le procedure con verifica **diretta, indiretta e inversa**.

### Le forme che non funzionano, per specie (dalla piu' pesante)

| specie | quante | forme ed esempi | dove sta |
|---|---:|---|---|
| **procedura non componibile** | 33 indirette ✗ | «rule for X is apply Y» non entra o il passo `apply` viene saltato in silenzio: `fahrkel` = 68 + 273,15 senza la conversione | regressione dell'interprete KB (`run_step(op(apply, …))`, procedures.p0) o del lettore della lezione — da rifare dopo la sessione |
| **procedura non invertibile** | 38 inverse ✗ | «What number gives 50.8 when inchmm is applied?» → il generatore di «resoconti causali» risponde una frase senza senso | nessuna forma per l'inversa; e una facolta' generativa prende la domanda (mantra #7, #17) |
| **conversione «to convert A to B multiply by N»** | 3 | la lezione non si legge piu' («I don't know about convert») | assisted-learning.p0: forma documentata e rotta |
| **regola profonda con classe di piu' parole** | 5 | «if x is a machine tool then x is a machine» → «could not anchor that rule»; con «something … it» regole di atomi (`holds(it_wears)`) | lettore delle regole: la clausola tipata vuole la classe in una parola; `something/it` non e' un'anafora del lettore |
| **classe composta non risalita** | 12 | «A tap is a cutting tool.» → «Is a tap a tool?» non sa; «Is a reamer a cutting tool?» → «don't know about cutting tool» | la testa del sintagma non e' un iperonimo (come P1 PHP); e la domanda polare non trova la classe composta |
| **lezione non letta** | 24 | gerundi di processo («Annealing is a heat treatment.», «Quenching …»), aggettivo+nome come oggetto («cuts internal threads», «cuts conductive materials»), verbi non noti («protects», «chamfers», «swaps», «converts»), «measures in discrete steps» | nessun lettore prende la frase: le parole sconosciute diventano muro invece di candidati |
| **domanda che non raggiunge un fatto entrato** | 31 | «What measures height?» (entrato «height gauge measures height») → «I don't understand»; «Does a spring store energy?» → idem; «What does a rotary encoder measure?» → «Angular.» (oggetto troncato) | lettori della domanda (answer_frame) su soggetti/oggetti composti |
| **⛔ contaminazione del contatto** | ≥6 | dopo «so it gauges thickness», «gauge» in **gauge blocks, bore gauge, go/no-go gauge** e' letto come *measures*; dopo «so it leads to chatter», **lead screw** e «lead does not resist scratching» diventano *causes* | l'ipotesi di contatto vale per OGNI occorrenza della parola, senza guardare il suo ruolo nel sintagma — vedi lo studio sul contesto qui sotto |
| **contatto su relazione non dichiarata / strumento** | 8 | «so it clamps / turns / conveys» su `hold`, `rotate`, `transmits` imparati da poco; «in other words, it spoils readings» | il contatto nasce solo su relazioni dichiarate (come P12 PHP); lo strumento vuole due contatti |
| **controesempio letto come negazione del fatto** | 2 | «A spindle rotates the cutting tool, but the cutting tool is not its product.» → `not(rotate(spindle, cutting_tool))`: un fatto vero negato | la frase di correzione nega la relazione sbagliata (risposta sbagliata, mantra #7) |
| **ordine transitivo muto** | 2 | «Hardening comes before tempering.» → «Held … the chain carries» ma «Does hardening come before tempering?» non decide; «What comes after tempering?» non capito | lettura della polare con «come»/«came» e della domanda inversa |
| **forme rosse del catalogo** | 2 | «rule for rpm takes 2 inputs» → **«Noted: The trip takes 2 hours.»** (dirottata); «test: apply X to N gives M» rifiutata dal cancello | #48 e #49 di §6-ter Q restano rosse; la prima produce una risposta falsa |
| **calcolo in lingua** | 2 | «One inch is 25.4 millimeters.» → «How many millimeters are in 3 inches?» non calcola | nessun ponte dalla misura detta a una procedura |
| **valore con relazione di piu' parole** | 5 | «modulus of elasticity is a relation», «coefficient of thermal expansion is a relation» non entrano; le densita' perdono l'unita' («7.85 grams») | nome di relazione con preposizione interna; valore con unita' composta troncato |
| **dopo il riavvio** | 1 | «Is a lathe a machine tool?» risponde in sessione, non dopo `/save` e riavvio | accesso alla classe composta caricata da file |

### Le 100 lezioni con problemi, una per riga

| # | forma | lezione | risposta alla lezione | verifica fallita | specie |
|---|---|---|---|---|---|
| 1 | base per il contatto | Heat causes thermal expansion. | I didn't keep that: «heat» and «thermal_expansion» don't read as things I can ho | «What causes thermal expansion?» → «Hmm, I don't know about expansion yet. Want me to learn about it?» | lezione rifiutata dal cancello dei concetti |
| 2 | L3 contatto-verbo | Tool wear causes poor surface finish, so it results in poor surface finish. | Learned: tool wear causes poor surface finish. | «What results in reading errors?» → «Hmm, I don't know about results yet. Want me to learn about it?» | lezione entrata, la domanda non la raggiunge (muro) |
| 3 | L3 contatto-nome | Backlash causes positioning errors, so positioning errors are its consequence. | Learned: backlash causes positioning errors. | «What is the consequence of tool wear?» → «I don't know about consequence.» | lezione entrata, la domanda non la raggiunge (muro) |
| 4 | L3 contatto-nome | Vibration causes chatter, so chatter is its effect. | Learned: vibration causes chatter. | «What is the effect of heat?» → «Reading «effect» as «causes». I don't know the effect of heat.» | lezione entrata, la domanda non la raggiunge (muro) |
| 5 | L3 strumento | Parallax causes reading errors; in other words, it spoils readings. | Learned: parallax causes reading errors. I couldn't read «in other words, it spo | «Does parallax spoil readings?» → «I cannot settle that: no fact I hold decides whether parallax spoilt readings. N» | lezione entrata, la domanda non la raggiunge (muro) |
| 6 | L3 strumento | Overheating causes tool failure; in other words, tool failure is its aftermath. | Learned: overheating causes tool failure. Learned: tool failure is aftermath. | «What is the aftermath of vibration?» → «I don't know any aftermath yet.» | lezione entrata, la domanda non la raggiunge (muro) |
| 7 | base per il contatto | A height gauge measures height. | Learned: height gauge measures height. | «What measures height?» → «I don't understand that yet.» | lezione entrata, la domanda non la raggiunge (muro) |
| 8 | L3 contatto-verbo | A micrometer measures thickness, so it gauges thickness. | Learned: micrometer measures thickness. | «Does a height gauge gauge height?» → «I don't understand that yet.» | lezione entrata, la domanda non la raggiunge (muro) |
| 9 | L3 contatto-verbo | A chuck holds the workpiece, so it grips the workpiece. | Learned: chuck hold workpiece. | «Does a vise grip the workpiece?» → «Reading «grip» as «held». I cannot settle that: no fact I hold decides whether v» | lezione entrata, la domanda non la raggiunge (muro) |
| 10 | L3 contatto-verbo | A collet holds the cutting tool, so it clamps the cutting tool. | Learned: collet hold cutting tool. | «What clamps the workpiece?» → «Hmm, I don't know about clamps yet. Want me to learn about it?» | lezione entrata, la domanda non la raggiunge (muro) |
| 11 | base per il contatto | Gauge blocks are made of hardened steel. | Reading «gauge» as «measures». I didn't keep that: «gauge_blocks» and «hardened_ | «What are gauge blocks made of?» → «Hmm, I don't know about blocks yet. Want me to learn about it?» | lezione rifiutata dal cancello dei concetti |
| 12 | base per il contatto | A surface plate is made of granite. | Learned: surface plate is. | «What is a surface plate made of?» → «I don't understand that yet.» | lezione entrata, la domanda non la raggiunge (muro) |
| 13 | base per il contatto | A lapping plate is made of cast iron. | Learned: lapping plate is. | «What is made of cast iron?» → «Steel.» | risposta sbagliata |
| 14 | base per il contatto | A carbide insert is made of tungsten carbide. | Learned: carbide insert is. | «What is a carbide insert made of?» → «I don't understand that yet.» | lezione entrata, la domanda non la raggiunge (muro) |
| 15 | L3 contatto-nome | A surface plate is made of granite, so granite is its material. | Learned: surface plate is. | «What is the material of a lapping plate?» → «I don't know any material yet.» | lezione entrata, la domanda non la raggiunge (muro) |
| 16 | L3 nome di piu' parole | Gauge blocks are made of hardened steel, so hardened steel is their base material. | Reading «gauge» as «measures». I didn't keep that: «gauge_blocks» and «hardened_ | «What is the base material of a carbide insert?» → «Hmm, I don't know about material yet. Want me to learn about it?» | lezione rifiutata dal cancello dei concetti |
| 17 | base per il contatto | Cast iron contains graphite. | I didn't keep that: «cast_iron» and «graphite» don't read as things I can hold a | «What contains graphite?» → «Hmm, I don't know about graphite yet. Want me to learn about it?» | lezione rifiutata dal cancello dei concetti |
| 18 | L3 contatto-verbo | Stainless steel contains chromium, so it includes chromium. | Learned: stainless steel contains chromium. | «Does cast iron include graphite?» → «Reading «include» as «contains». I cannot settle that: no fact I hold decides wh» | lezione entrata, la domanda non la raggiunge (muro) |
| 19 | L3 contatto-nome | Cast iron contains graphite, so graphite is its constituent. | I didn't keep that: «cast_iron» and «graphite» don't read as things I can hold a | «What is the constituent of tool steel?» → «I don't know about constituent.» | lezione rifiutata dal cancello dei concetti |
| 20 | base per il contatto | A ball screw converts rotary motion into linear motion. | Hmm, I don't know about converts yet. Want me to learn about it? | «What converts rotary motion into linear motion?» → «Hmm, I don't know about converts yet.» | lezione non letta (nessun lettore la prende) |
| 21 | L3 contatto-verbo | A ball screw converts rotary motion into linear motion, so it transforms rotary motion int | I looked up «converts» but found nothing. If you know it, teach me: «converts is | «Does a cam transform rotary motion into reciprocating motion» → «I cannot settle that: no fact I hold decides whether cam transform rotary motion» | lezione non letta (nessun lettore la prende) |
| 22 | L3 contatto-verbo | A spindle rotates the cutting tool, so it turns the cutting tool. | Learned: spindle rotate cutting tool. | «Does a lathe turn the workpiece?» → «Reading «turn» as «rotates». I cannot settle that: no fact I hold decides whethe» | lezione entrata, la domanda non la raggiunge (muro) |
| 23 | base per il contatto | A belt transmits power. | I don't understand that yet. | «What does a belt transmit?» → «I'm not sure I followed. Can you say it another way?» | lezione non letta (nessun lettore la prende) |
| 24 | L3 contatto-verbo | A belt transmits power, so it conveys power. | Learned: belt transmits power. | «Does a key convey torque?» → «I cannot settle that: no fact I hold decides whether key convey torque. Not prov» | lezione entrata, la domanda non la raggiunge (muro) |
| 25 | L3 controesempio | A lathe rotates the workpiece; moreover, the workpiece is its product. | Learned: lathe rotate workpiece. I couldn't read «moreover, the workpiece is its | «What is the product of a spindle?» → «I don't know any product yet.» | lezione entrata, la domanda non la raggiunge (muro) |
| 26 | L3 controesempio | A spindle rotates the cutting tool, but the cutting tool is not its product. | Learned: a spindle rotates the cutting tool, but the cutting tool is not its pro | «What is the product of a lathe?» → «I don't know any product yet.» | lezione entrata, la domanda non la raggiunge (muro) |
| 27 | relazione-L2 | modulus of elasticity is a relation | Hmm, I don't know about modulus yet. Want me to learn about it? | «» → «» | lezione non letta (nessun lettore la prende) |
| 28 | relazione-L2 | coefficient of thermal expansion is a relation | Hmm, I don't know about expansion yet. | «» → «» | lezione non letta (nessun lettore la prende) |
| 29 | valore | The hardness of a gauge block is 63 HRC. | Hmm, I don't know about hardness yet. Want me to learn about it? | «What is the hardness of a gauge block?» → «Reading «gauge» as «measures». I don't know about hardness.» | lezione non letta (nessun lettore la prende) |
| 30 | valore | The modulus of elasticity of steel is 210 gigapascals. | Hmm, I don't know about modulus yet. | «What is the modulus of elasticity of steel?» → «I don't know about modulus.» | lezione non letta (nessun lettore la prende) |
| 31 | valore | The coefficient of thermal expansion of steel is 12 micrometers per meter per kelvin. | Hmm, I don't know about expansion yet. | «What is the coefficient of thermal expansion of steel?» → «I don't know about coefficient.» | lezione non letta (nessun lettore la prende) |
| 32 | valore | The modulus of elasticity of aluminium is 69 gigapascals. | Hmm, I don't know about modulus yet. | «What is the modulus of elasticity of aluminium?» → «I don't know about modulus.» | lezione non letta (nessun lettore la prende) |
| 33 | valore | The coefficient of thermal expansion of aluminium is 23 micrometers per meter per kelvin. | Hmm, I don't know about expansion yet. | «What is the coefficient of thermal expansion of aluminium?» → «I don't know about coefficient.» | lezione non letta (nessun lettore la prende) |
| 34 | classe | A micrometer is a measuring instrument. | Learned: micrometer is a measuring instrument. | «Is a micrometer a measuring instrument?» → «Hmm, I don't know about measuring instrument yet. Want me to learn about it?» | lezione entrata, la domanda non la raggiunge (muro) |
| 35 | classe | A dial indicator is a measuring instrument. | Learned: dial indicator is a measuring instrument. | «Is a dial indicator an instrument?» → «I don't know: nothing I hold says dial indicator is instrument, and nothing says» | lezione entrata, la domanda non la raggiunge (muro) |
| 36 | classe | A reamer is a cutting tool. | Learned: reamer is a cutting tool. | «Is a reamer a cutting tool?» → «Hmm, I don't know about cutting tool yet. Want me to learn about it?» | lezione entrata, la domanda non la raggiunge (muro) |
| 37 | classe | A tap is a cutting tool. | Learned: tap is a cutting tool. | «Is a tap a tool?» → «I don't know about tool.» | lezione entrata, la domanda non la raggiunge (muro) |
| 38 | classe | A stepper motor is an electric motor. | Learned: stepper motor is an electric motor. | «Is a stepper motor a motor?» → «I don't know: nothing I hold says stepper motor is motor, and nothing says it is» | lezione entrata, la domanda non la raggiunge (muro) |
| 39 | classe | A vise is a work holding device. | Learned: vise is a work holding device. | «Is a vise a work holding device?» → «Hmm, I don't know about work holding device yet. Want me to learn about it?» | lezione entrata, la domanda non la raggiunge (muro) |
| 40 | classe | Annealing is a heat treatment. | Hmm, I don't know about annealing yet. Want me to learn about it? | «Is annealing a heat treatment?» → «I don't know about heat treatment.» | lezione non letta (nessun lettore la prende) |
| 41 | classe | Quenching is a heat treatment. | Hmm, I don't know about quenching yet. Want me to learn about it? | «What is quenching?» → «I looked up «quenching» but found nothing. If you know it, teach me: «quenching » | lezione non letta (nessun lettore la prende) |
| 42 | classe | Tempering is a heat treatment. | Hmm, I don't know about treatment yet. Want me to learn about it? | «Is tempering a treatment?» → «I don't know about treatment.» | lezione non letta (nessun lettore la prende) |
| 43 | classe | Honing is a finishing process. | Learned: honing is a finishing process. | «Is honing a process?» → «I cannot settle that: no fact I hold decides whether honing is a process, and kn» | lezione entrata, la domanda non la raggiunge (muro) |
| 44 | classe | A fly cutter is a cutting tool. | Learned: fly cutter is a cutting tool. | «Is a fly cutter a tool?» → «I don't know: nothing I hold says fly cutter is tool, and nothing says it isn't.» | lezione entrata, la domanda non la raggiunge (muro) |
| 45 | classe | A profilometer is a measuring instrument. | Learned: profilometer is a measuring instrument. | «Is a profilometer an instrument?» → «I don't know about instrument.» | lezione entrata, la domanda non la raggiunge (muro) |
| 46 | funzione | A bore gauge measures bore diameter. | Reading «gauge» as «measures». Learned: bore gauge measures bore diameter. | «What measures bore diameter?» → «Hmm, I don't know about diameter yet. Want me to learn about it?» | lezione entrata, la domanda non la raggiunge (muro) |
| 47 | funzione | A laser interferometer measures displacement. | It was a mysterious it. Then one day, it discovered what it meant to be seen. it | «Does a laser interferometer measure displacement?» → «Hmm, I don't know about laser interferometer yet. Want me to learn about it?» | lezione non letta (nessun lettore la prende) |
| 48 | funzione | A tap cuts internal threads. | Hmm, I don't know about internal yet. Want me to learn about it? | «What does a tap cut?» → «I don't understand that yet.» | lezione non letta (nessun lettore la prende) |
| 49 | funzione | A die cuts external threads. | Hmm, I don't know about external yet. Want me to learn about it? | «What cuts external threads?» → «Hmm, I don't know about external yet.» | lezione non letta (nessun lettore la prende) |
| 50 | funzione | A rotary encoder measures angular position. | Learned: rotary encoder measures angular position. | «What does a rotary encoder measure?» → «Angular.» | risposta sbagliata |
| 51 | funzione | A spring stores energy. | Learned: spring stores energy. | «Does a spring store energy?» → «I don't understand that yet.» | lezione entrata, la domanda non la raggiunge (muro) |
| 52 | funzione | Annealing softens steel. | I looked up «external» but found nothing. If you know it, teach me: «external is | «What softens steel?» → «Hmm, I don't know about softens yet. Want me to learn about it?» | lezione non letta (nessun lettore la prende) |
| 53 | funzione | A go/no-go gauge checks tolerances. | Reading «gauge» as «measures». Learned: go/no-go gauge checks tolerances. | «What checks tolerances?» → «Hmm, I don't know about checks yet. Want me to learn about it?» | lezione entrata, la domanda non la raggiunge (muro) |
| 54 | funzione | Wire EDM cuts conductive materials. | Hmm, I don't know about conductive yet. Want me to learn about it? | «What does wire EDM cut?» → «I don't understand that yet.» | lezione non letta (nessun lettore la prende) |
| 55 | funzione | A granite surface plate provides a flat reference. | Hmm, I don't know about granite surface yet. Want me to learn about it? | «What does a granite surface plate provide?» → «Hmm, I don't know about granite_surface yet.» | lezione non letta (nessun lettore la prende) |
| 56 | funzione | A stepper motor moves in discrete steps. | I understood you're asking how to make steps, but I don't have the steps for it  | «How does a stepper motor move?» → «Hmm, I don't know about stepper motor yet. Want me to learn about it?» | lezione non letta (nessun lettore la prende) |
| 57 | funzione | An end mill cuts slots. | I don't understand that yet. | «What does an end mill cut?» → «Hmm, I don't know about end mill yet. Want me to learn about it?» | lezione non letta (nessun lettore la prende) |
| 58 | funzione | A lead screw converts rotary motion into linear motion. | Reading «lead» as «causes». Learned: lead screw converts rotary motion. | «What does a lead screw convert?» → «Hmm, I don't know about convert yet. Want me to learn about it?» | lezione entrata, la domanda non la raggiunge (muro) |
| 59 | funzione | A countersink chamfers holes. | Hmm, I don't know about countersink chamfers holes yet. Want me to learn about i | «What chamfers holes?» → «Hmm, I don't know about chamfers yet. Want me to learn about it?» | lezione non letta (nessun lettore la prende) |
| 60 | funzione | A way cover protects the guideways. | Hmm, I don't know about guideways yet. Want me to learn about it? | «What does a way cover protect?» → «Hmm, I don't know about protect yet. Want me to learn about it?» | lezione non letta (nessun lettore la prende) |
| 61 | funzione | A tool changer swaps cutting tools. | Hmm, I don't know about changer yet. Want me to learn about it? | «What does a tool changer swap?» → «Hmm, I don't know about tool changer yet. Want me to learn about it?» | lezione non letta (nessun lettore la prende) |
| 62 | materiale | Bronze is made of copper and tin. | Learned: bronze made of copper. | «Is bronze made of tin?» → «I cannot settle that: no fact I hold decides whether bronze makeed of of tin. No» | lezione entrata, la domanda non la raggiunge (muro) |
| 63 | procedura | rule for inchmm is multiply by 25.4 | Held: for inchmm, 1 is «multiply by 25.4». | indiretta: «apply inchmm2 to 1» → «inchmm2(«2») — that is what the steps give.»; inversa: «What number gives 50.8 when inchmm is applied?» → «On What number gives 50.8 when inchmm is, a causal account turns on th» | verifica indiretta, inversa fallita |
| 64 | procedura | rule for mminch is divide by 25.4 | Held: for mminch, 1 is «divide by 25.4». | indiretta: «apply roundtrip to 3» → «Hmm, I don't know about roundtrip yet. Want me to learn about it?»; inversa: «What number gives 5 when mminch is applied?» → «On What number gives 5 when mminch is, a causal account turns on the c» | verifica indiretta, inversa fallita |
| 65 | procedura | rule for thimble is divide by 100 | Held: for thimble, 1 is «divide by 100». | indiretta: «apply thimbleinch to 254» → «Hmm, I don't know about thimbleinch yet. Want me to learn about it?»; inversa: «What number gives 0.37 when thimble is applied?» → «On What number gives 0.37 when thimble is, a causal account turns on t» | verifica indiretta, inversa fallita |
| 66 | procedura | rule for vernier is multiply by 0.02 | Held: for vernier, 1 is «multiply by 0.02». | indiretta: «apply vernierx to 13» → «vernierx(«23») — that is what the steps give.»; inversa: «What number gives 0.26 when vernier is applied?» → «On What number gives 0.26 when vernier is, a causal account turns on t» | verifica indiretta, inversa fallita |
| 67 | procedura | rule for rpm10 is multiply by 31.83 | Held: for rpm10, 1 is «multiply by 31.83». | indiretta: «apply rpm10half to 100» → «rpm10half(«50») — that is what the steps give.»; inversa: «What number gives 3183 when rpm10 is applied?» → «On What number gives 3183 when rpm10 is, a causal account turns on the» | verifica indiretta, inversa fallita |
| 68 | procedura | rule for growth100 is multiply by 0.0012 | Held: for growth100, 1 is «multiply by 0.0012». | indiretta: «apply growth200 to 5» → «growth200(«10») — that is what the steps give.»; inversa: «What number gives 0.006 when growth100 is applied?» → «On What number gives 0.006 when growth100 is, a causal account turns o» | verifica indiretta, inversa fallita |
| 69 | procedura | rule for stepsdeg is divide by 1.8 | Held: for stepsdeg, 1 is «divide by 1.8». | indiretta: «apply stepsturn to 360» → «Hmm, I don't know about stepsturn yet. Want me to learn about it?»; inversa: «What number gives 50 when stepsdeg is applied?» → «On What number gives 50 when stepsdeg is, a causal account turns on th» | verifica indiretta, inversa fallita |
| 70 | procedura | rule for screwtravel is multiply by 5 | Held: for screwtravel, 1 is «multiply by 5». | indiretta: «apply screwtravelinch to 127» → «Hmm, I don't know about screwtravelinch yet. Want me to learn about it»; inversa: «What number gives 60 when screwtravel is applied?» → «On What number gives 60 when screwtravel is, a causal account turns on» | verifica indiretta, inversa fallita |
| 71 | procedura | rule for diameter is double | Held: for diameter, 1 is «double». | indiretta: «apply circum to 10» → «circum(«31.4159») — that is what the steps give.»; inversa: «What number gives 25 when diameter is applied?» → «On What number gives 25 when diameter is, a causal account turns on th» | verifica indiretta, inversa fallita |
| 72 | procedura | rule for gearout is divide by 4 | Held: for gearout, 1 is «divide by 4». | indiretta: «apply gearout16 to 1440» → «Hmm, I don't know about gearout16 yet. Want me to learn about it?»; inversa: «What number gives 360 when gearout is applied?» → «On What number gives 360 when gearout is, a causal account turns on th» | verifica indiretta, inversa fallita |
| 73 | procedura a piu' passi | rule for fahrcel is subtract 32 | rule for fahrcel is multiply by 5 | rule for fahrcel is  | Held: for fahrcel, 1 is «subtract 32». / Held: for fahrcel, 2 is «multiply by 5» | indiretta: «apply fahrkel to 68» → «fahrkel(«341.15») — that is what the steps give.»; inversa: «What number gives 20 when fahrcel is applied?» → «On What number gives 20 when fahrcel is, a causal account turns on the» | verifica indiretta, inversa fallita |
| 74 | procedura a piu' passi | rule for tapm is subtract 1.5 | Held: for tapm, 1 is «subtract 1.5». | indiretta: «apply tapminch to 27.4» → «Hmm, I don't know about tapminch yet. Want me to learn about it?»; inversa: «What number gives 8.5 when tapm is applied?» → «On What number gives 8.5 when tapm is, a causal account turns on the c» | verifica indiretta, inversa fallita |
| 75 | procedura a piu' passi | rule for degrad is multiply by 3.14159 | rule for degrad is divide by 180 | Held: for degrad, 1 is «multiply by 3.14159». / Held: for degrad, 2 is «divide b | indiretta: «apply halfturn to 180» → «halfturn(«90») — that is what the steps give.»; inversa: «What number gives 3.14159 when degrad is applied?» → «On What number gives 3.14159 when degrad is, a causal account turns on» | verifica indiretta, inversa fallita |
| 76 | procedura con ramo | rule for toleven is if odd then add 1 else halve | Held: for toleven, 1 is «if odd then add 1 else halve». | inversa: «What number gives 8 when toleven is applied?» → «On What number gives 8 when toleven is, a causal account turns on the » | verifica inversa fallita |
| 77 | procedura con ciclo | rule for halvetill is repeat halve until below 1 | Held: for halvetill, 1 is «repeat halve until below 1». | indiretta: «apply halvetillx to 10» → «halvetillx(«80») — that is what the steps give.»; inversa: «What number gives 0.625 when halvetill is applied?» → «On What number gives 0.625 when halvetill is, a causal account turns o» | verifica indiretta, inversa fallita |
| 78 | conversione | to convert inches to millimeters multiply by 25.4 | Hmm, I don't know about convert yet. | diretta: «convert 3 inches to millimeters» → «I didn't quite catch that. What would you like to know?»; indiretta: «How many millimeters are 3 inches?» → «Hmm, I don't know about millimeters yet. Want me to learn about it?»; inversa: «convert 76.2 millimeters to inches» → «I looked up «millimeters» but found nothing. If you know it, teach me:» | verifica diretta, indiretta, inversa fallita |
| 79 | conversione | to convert microns to millimeters divide by 1000 | Hmm, I don't know about convert yet. | diretta: «convert 250 microns to millimeters» → «Hmm, that's a bit beyond me right now.»; indiretta: «How many millimeters are 250 microns?» → «Hmm, I don't know about millimeters yet.»; inversa: «convert 0.25 millimeters to microns» → «Hmm, I don't know about convert yet.» | verifica diretta, indiretta, inversa fallita |
| 80 | conversione | to convert newton meters to foot pounds multiply by 0.7376 | I don't understand that yet. | diretta: «convert 50 newton meters to foot pounds» → «Hmm, I don't know about convert yet.»; indiretta: «How many foot pounds are 50 newton meters?» → «Hmm, I don't know about pounds yet. Want me to learn about it?»; inversa: «convert 36.88 foot pounds to newton meters» → «I looked up «pounds» but found nothing. If you know it, teach me: «pou» | verifica diretta, indiretta, inversa fallita |
| 81 | regola profonda | if something is a machine tool then it is a machine | if something is a machine then it ne | I could not anchor that rule: the conclusion about machine talks about something | diretta: «Is a lathe a machine?» → «I don't know about machine.»; indiretta: «Does a lathe need maintenance?» → «I cannot settle that: no fact I hold decides whether lathe need mainte»; inversa: «A hammer does not need maintenance. Is a hammer a machine to» → «Learned: a hammer does not need maintenance. I couldn't read «Is a ham» | verifica diretta, indiretta, inversa fallita |
| 82 | regola profonda | if something is a cutting tool then it wears | if something wears then it needs replacemen | Learned rule: holds(it_wears) :- holds(something_is_cutting_tool). / Learned rul | diretta: «Does an end mill wear?» → «Hmm, I don't know about end_mill yet.»; indiretta: «Does an end mill need replacement?» → «I cannot settle that: no fact I hold decides whether end mill need rep»; inversa: «A granite plate does not wear. Is a granite plate a cutting » → «Learned: a granite plate does not wear. I don't know: nothing I hold s» | verifica diretta, indiretta, inversa fallita |
| 83 | regola profonda | if something is hardened steel then it is hard | if something is hard then it resists scra | I could not anchor that rule: the conclusion about hard talks about something th | diretta: «Is a gauge block hard?» → «I don't understand that yet.»; indiretta: «Does a gauge block resist scratching?» → «Hmm, I don't know about resist yet. Want me to learn about it?»; inversa: «Lead does not resist scratching. Is lead hardened steel?» → «Reading «lead» as «causes». Held: lead does not resist scratching. Rea» | verifica diretta, indiretta, inversa fallita |
| 84 | ordine transitivo | Hardening comes before tempering. | Tempering comes before grinding. | Held: hardening comes before tempering -- and the chain carries. / Held: temperi | diretta: «Does hardening come before tempering?» → «I cannot settle that: no fact I hold decides whether hardening came be»; indiretta: «Does hardening come before grinding?» → «I cannot settle that: no fact I hold decides whether hardening came be»; inversa: «What comes after tempering?» → «I don't understand that yet.» | verifica diretta, indiretta, inversa fallita |
| 85 | ordine transitivo | Centering comes before drilling. | Drilling comes before reaming. | Held: centering comes before drilling -- and the chain carries. / Held: drilling | diretta: «What comes before drilling?» → «I don't understand that yet.»; indiretta: «Does centering come before reaming?» → «I cannot settle that: no fact I hold decides whether centering came be»; inversa: «Does reaming come before centering?» → «I cannot settle that: no fact I hold decides whether reaming came befo» | verifica diretta, indiretta, inversa fallita |
| 86 | relazione inversa | drives is the inverse of is driven by | A servo motor drives the ball screw. | Held: drives and is driven by are one fact seen from the two sides. / Learned: s | diretta: «What does a servo motor drive?» → «Axis and ball.»; inversa: «Is the ball screw driven by a servo motor?» → «I don't know: nothing I hold says ball screw driven by is servo motor,» | verifica diretta, inversa fallita |
| 87 | relazione transitiva | meshes with goes both ways | The pinion meshes with the rack. | Hmm, I don't know about meshes yet. Want me to learn about it? / I looked up «me | diretta: «Does the pinion mesh with the rack?» → «Hmm, I don't know about pinion mesh yet. Want me to learn about it?»; indiretta: «What meshes with the rack?» → «Hmm, I don't know about meshes yet.»; inversa: «Does the rack mesh with the pinion?» → «Hmm, I don't know about rack mesh yet. Want me to learn about it?» | verifica diretta, indiretta, inversa fallita |
| 88 | regola profonda (x) | if x is a machine tool then x is a machine | if x is a machine then x needs maintenance |  | I could not anchor that rule: the conclusion about machine talks about something | diretta: «Is a shaper a machine?» → «I don't know about machine.»; indiretta: «Does a shaper need maintenance?» → «I cannot settle that: no fact I hold decides whether shaper need maint»; inversa: «Is a shaper not a machine?» → «I don't know: nothing I hold says shaper not is machine, and nothing s» | verifica diretta, indiretta, inversa fallita |
| 89 | regola profonda (x) | if x is a cutting tool then x is a tool | A countersink is a cutting tool. | I could not anchor that rule: the conclusion about tool talks about something th | diretta: «Is a countersink a tool?» → «I don't know about tool.»; indiretta: «Is a boring bar a tool?» → «I don't know: nothing I hold says boring bar is tool, and nothing says»; inversa: «Is a countersink not a tool?» → «I don't know: nothing I hold says countersink not is tool, and nothing» | verifica diretta, indiretta, inversa fallita |
| 90 | regola profonda (x, due premesse) | if x contains y and y is hard then x is abrasive | A grinding wheel contains corundum. | C | Learned rule: abrasive($V1) :- contains($V1, $V2), hard($V2). / Learned: grindin | diretta: «Is a grinding wheel abrasive?» → «Hmm, I don't know about grinding wheel abrasive yet. Want me to learn »; indiretta: «Why is a grinding wheel abrasive?» → «I don't know about grinding wheel abrasive.»; inversa: «Is a grinding wheel not abrasive?» → «Hmm, I don't know about grinding wheel yet. Want me to learn about it?» | verifica diretta, indiretta, inversa fallita |
| 91 | regola profonda (x, relazione) | if x drives y and y moves z then x moves z | A servo motor drives the ball screw. | The ba | Learned rule: move($V1, $V3) :- drive($V1, $V2), move($V2, $V3). / Learned: serv | diretta: «Does the ball screw move the table?» → «I cannot settle that: no fact I hold decides whether ball screw move t»; inversa: «What moves the table?» → «Ball.» | verifica diretta, inversa fallita |
| 92 | procedura: correzione di un passo | rule for mmcm is divide by 100 | step 1 of mmcm is now divide by 10 | Held: for mmcm, 1 is «divide by 100». / Held: step 1 of mmcm is now «divide by 1 | indiretta: «apply mmdm to 250» → «mmdm(«25») — that is what the steps give.»; inversa: «What number gives 25 when mmcm is applied?» → «On What number gives 25 when mmcm is, a causal account turns on the co» | verifica indiretta, inversa fallita |
| 93 | procedura: rimozione di un passo | rule for offs is add 5 | rule for offs is double | remove step 2 of offs | Held: for offs, 1 is «add 5». / Held: for offs, 2 is «double». / Removed step 2  | indiretta: «apply offs2 to 10» → «I don't understand that yet.»; inversa: «What number gives 15 when offs is applied?» → «On What number gives 15 when offs is, a causal account turns on the co» | verifica indiretta, inversa fallita |
| 94 | procedura: elenco e spiegazione | rule for feedtooth is divide by 4 | Held: for feedtooth, 1 is «divide by 4». | inversa: «why?» → «"I do not have an active proposal to continue. Give me the topic or th» | verifica inversa fallita |
| 95 | procedura: ciclo con arresto numerico | rule for doubletill is repeat double until above 100 | Held: for doubletill, 1 is «repeat double until above 100». | indiretta: «apply doubletillh to 3» → «doubletillh(«1.5») — that is what the steps give.»; inversa: «What number gives 192 when doubletill is applied?» → «On What number gives 192 when doubletill is, a causal account turns on» | verifica indiretta, inversa fallita |
| 96 | procedura: ciclo nominato (Collatz) | rule for cstep is if even then halve else triple and add 1 | rule for collatzm is repeat a | Held: for cstep, 1 is «if even then halve else triple and add 1». / I know I rep | indiretta: «apply collatzm to 6» → «Hmm, I don't know about collatzm yet. Want me to learn about it?»; inversa: «What number gives 3 when cstep is applied?» → «On What number gives 3 when cstep is, a causal account turns on the co» | verifica indiretta, inversa fallita |
| 97 | procedura: piu' ingressi | rule for rpm takes 2 inputs | Noted: The trip takes 2 hours. | diretta: «apply rpm to 100 and 10» → «I don't understand that yet.»; indiretta: «What is the spindle speed for 100 meters per minute and 10 m» → «I don't know much about spindle speed yet, and the network is off, so »; inversa: «What cutting speed gives 3183 on a 10 millimeter cutter?» → «Hmm, I don't know about 10 millimeter cutter yet. Want me to learn abo» | verifica diretta, indiretta, inversa fallita |
| 98 | procedura: oracolo di test | test: apply inchmm to 1 gives 25.4 | I didn't keep that: «test_apply_inchmm_to_1» and «25.4» don't read as things I c | inversa: «test: apply inchmm to 2 gives 51» → «I didn't keep that: «test_apply_inchmm_to_2» and «51» don't read as th» | verifica inversa fallita |
| 99 | calcolo in lingua | One inch is 25.4 millimeters. | Learned: one_inch has 25.4 millimeters. | indiretta: «How many millimeters are in 3 inches?» → «Hmm, I don't know about millimeters yet.»; inversa: «How many inches are 76.2 millimeters?» → «Hmm, I don't know about inches yet. Want me to learn about it?» | verifica indiretta, inversa fallita |
| 100 | calcolo in lingua | A micrometer thimble has 50 divisions. | One revolution of the thimble moves the spindle 0 | Learned: micrometer thimble has 50 divisions. / Learned: one revolution of the t | indiretta: «How far does the spindle move per thimble division?» → «Hmm, I don't know about thimble yet. Want me to learn about it?»; inversa: «How many divisions move the spindle 0.1 millimeters?» → «Hmm, I don't know about spindle 0.1 millimeters yet. Want me to learn » | verifica indiretta, inversa fallita |

**Il reperto piu' grave** e' la contaminazione: una lezione **riuscita** peggiora le
lezioni successive in un'altra forma. Ne segue lo studio chiesto da F. sul contesto.

## 🟠 FORME D'INSEGNAMENTO CHE NON FUNZIONANO — sessione live «debug di applicazioni PHP» (26 settembre 2026)

Annotate durante la sessione (transcript `docs/sessions/live/2026-09-26-php-debug.log`),
su richiesta di F.: *«quando una forma di insegnamento non funziona annotala qui e
vai avanti»*. Ogni riga: la frase detta, che cosa è entrato, la domanda che l'ha
mostrato, la specie del difetto. Il coefficiente della sessione è in fondo.

### Stato dopo la seconda passata (26 settembre 2026, mattina)

Verificato rigiocando tutte le lezioni in un processo nuovo
(`tests/p0t/language/php_teaching_repairs.p0t`, 31 casi verdi):

| # | stato | come |
|---|---|---|
| P1 | 🟡 | «Is Xdebug an extension?» regge per catena; «for PHP» resta perso |
| P2 | 🟡 | «What does error_log write?» risponde; l'apposizione «the NOME function» resta incollata nel fatto |
| P3 | 🔴 | oggetto frasale «whether …» non leggibile |
| P4 | 🔴 | soggetto con relativa |
| P5 | ✅ | l'altra sessione: ask_noun_of early + verso della relazione; reso «cede se vuota» perche' la capitale tornasse |
| P6 | 🔴 | l'enumerazione via contatto («What triggers …?») dà una causa sola |
| P7 | ✅ | proposizioni malformate tolte dal deposito semantico; print_r ora si impara (identificatore unito). Residuo: il deposito semantico tronca l'oggetto a «readable» |
| P8 | 🔴 | contatto con oggetto coordinato |
| P9 | ✅ | `joined_identifier/1`: un token unito è un nome (memory_limit) |
| P10 | ✅ | «fatal» chiude solo davanti a «to» (`adjective_relation_opens/1`) |
| P11 | 🟡 | «A stack trace is a list.» + la frase entrano, «What shows …?» risponde; «What does a stack trace show?» mura ancora; «calls» cade senza «function call» dichiarato |
| P12 | 🟡 | il contatto nasce («Reading handles as manages»), il trasferimento su PHP-FPM no |
| P13 | ✅ | «where» non riceve piu' un verdetto (`why_cue`); risposta-eco rifiutata (`answer_type_ok`) |
| P14 | ✅ | l'altra sessione: `named_relation_polar` |

| # | lezione detta | che cosa è entrato | la domanda che lo mostra | specie |
|---|---|---|---|---|
| P1 | «Xdebug is a debugger extension for PHP.» | `xdebug is a debugger extension`; «for PHP» perso; la risposta si porta dietro una lacuna vecchia («I looked up «var dump» but found nothing…») | «Is Xdebug an extension?» → «I don't know about extension»; «Is Xdebug a debugger?» idem | la **testa del sintagma** di una classe composta non è un iperonimo; complemento della classe perso; lacuna pendente riaffiorata nella risposta sbagliata. *Si ripara per contatto*: «A debugger extension is an extension.» → Yes |
| P2 | «The error_log function writes a message to the error log.» | soggetto `error log function` | «What does error_log write?» → «I don't know about error log» | **apposizione** «the NOME function / the NOME setting» incollata al nome. La lezione esplicita «error_log is the name of the function» diventa la classe `error log is a name`: l'apposizione non si insegna |
| P3 | «The display_errors setting controls whether errors appear in the output.» | niente: «I didn't keep that: «display_errors_setting_controls_whether_errors» and «output» don't read as things…» | «What does display_errors control?» → lacuna | **oggetto frasale** («whether …») non leggibile, più l'apposizione di P2 che incolla il verbo nel soggetto |
| P4 | «A script that exceeds memory_limit causes an allowed memory size exhausted error.» | `script exceed memory limit` | «What causes an allowed memory size exhausted error?» → «I don't know about allowed» | soggetto con **relativa** («that exceeds …») ridotto a una tripla; oggetto di cinque parole non riconosciuto come un nome (cerca «allowed») |
| P5 | L3 per contatto: «A missing semicolon causes a parse error, so a parse error is its symptom.» | «Reading «symptom» as «causes»» (il nome è preso) | «What is the symptom of a missing semicolon?» → «I can't show that»; «What is the symptom of output before the header function?» → **la definizione di «function»** | il **nome di relazione** imparato per contatto si legge, ma la domanda «the N of X» non raggiunge l'effetto; con X lungo la testa del sintagma («function») prende il posto di X: **risposta sbagliata** (mantra #7) |
| P6 | «An unclosed string causes a parse error.» dopo «A missing semicolon causes a parse error.» | due fatti veri | «What triggers a parse error?» → solo «missing semicolon» | l'**enumerazione** dà una soluzione sola (già nei cantieri, reperto 6) |
| P7 | «print_r prints a readable view of a variable.» | niente: «print_r and readable_view don't read as things I can hold a fact about» | eppure «What does print_r print?» → «Readable.» | il lettore rifiuta il fatto ma **un'altra strada risponde** dalla lettura del turno: incoerenza (dice di non averlo tenuto e poi lo usa) |
| P8 | L3 per contatto: «var_dump prints the type and value of a variable, so it displays the type and value of a variable.» | `var dump print type` e basta | «Does var_dump display the type of a variable?» → «I don't know» | il **verbo** per contatto non nasce quando l'oggetto è una **coordinazione** («type and value of a variable»); con un oggetto semplice («triggers», L11) funziona |
| P9 | «The default value of memory_limit is 128 megabytes.» | niente («I don't know about default value»), anche con «128M» e perfino con «128 seconds» | «What is the default value of memory_limit?» → «nothing I hold gives memory limit a default value» | la stessa forma legge `max_execution_time`, `default_socket_timeout`, `upload_max_filesize` (anche in megabyte, dopo «The megabyte measures information.»): il blocco è il **soggetto** `memory_limit`. Ipotesi da verificare: il fatto storto di P4 («script exceed memory limit») ha dato a «memory limit» un altro ruolo |
| P10 | «A fatal error stops the script.» (e «Fatal errors stop scripts.», «…halts…») | niente: «fatal_error and script don't read as things I can hold a fact about» | «Does a fatal error stop the script?» → non decide; «What stops the script?» → **«Fatal error and warning does not.»** | il nome «fatal error» non si può nemmeno classificare («A fatal error is an error.» → smalltalk); «script» si impara («A script is a program.») ma la lezione resta rifiutata. Ipotesi: collisione con il predicato `error/2` di `kb/experts/programming/debug.p0`. E la wh- **mescola** un fatto rifiutato e una negazione in un elenco positivo: risposta sbagliata |
| P11 | «A stack trace shows the sequence of function calls.» (e il contatto «…, so it lists …») | niente: prima «stack» e «shows_the_sequence» (**«trace» letto come verbo**), dopo «A stack trace is a report.» «stack_trace» e «sequence» — rifiutati lo stesso | «What does a stack trace show?» / «What shows a stack trace?» → la facoltà degli strumenti: «paste it here and I'll work on it» | ambiguità nome/verbo della testa («trace»); la domanda è **rubata** dal lettore di tracce (mantra #17); e il rifiuto «don't read as things I can hold a fact about» è lo stesso di P3, P7, P10: un solo cancello, `p0_fact_is_clean` / `p0_atom_is_concept` (`10-memory-knowledge.c`, template `rejected_binary_fact`), che decide in C che cosa è un «concetto» |
| P12 | L3 per contatto su un verbo appena imparato: «Composer manages dependencies, so it handles dependencies.» | solo `contact_episode(handles, manage, …)`, nessuna lettura | «Does PHP-FPM handle worker processes?» / «What handles dependencies?» → «I don't know about handle(s)» | il contatto nasce su relazioni **dichiarate** (`causes`, L11 «triggers» ✓), non su un verbo imparato un minuto prima (`manage/2`): l'episodio resta in attesa di un secondo |
| P13 | «OPcache stores compiled bytecode in shared memory.» | `stores(opcache, compiled_bytecode)`; «in shared memory» perso | «Where does OPcache store compiled bytecode?» → **«Compiled bytecode.»** | il **complemento di luogo** con «in» cade (con «from» regge, fisiologia) e la domanda «where» riceve l'oggetto: **risposta sbagliata** (mantra #7) |
| P14 | «The default port of Xdebug is 9003.» (imparata: la wh- risponde 9003) | `default_port_of(xdebug, 9003)` | «Is 9003 the default port of Xdebug?» → «I don't know about default port» | la **polare** «is V the R of X» non raggiunge una relazione nominata (limite già visto al gen509, «is-X-the-R») |

### Il coefficiente della sessione (F.: «lezioni imparate e invece lezioni con problemi, vediamo quale sono di più»)

Si conta **una lezione per frase d'insegnamento distinta** (le ripetizioni di
diagnosi della stessa frase non contano). **Imparata** = dopo la frase, una
domanda *diversa* dalla lezione (wh-, polare, o un caso tenuto fuori) risponde
giusto; **con problemi** = la frase non entra, entra storta, o la domanda che la
dovrebbe usare mura o sbaglia. Dieci delle imparate sono state riverificate in
un processo nuovo dopo `/save` e reggono tutte.

| | lezioni | quota |
|---|---:|---:|
| imparate | 19 | 53 % |
| con problemi | 17 | 47 % |
| **coefficiente imparate / con problemi** | **1,12** | |
| di cui per contatto (L3) | 1 imparata su 5 | 20 % |

Le imparate: classe per catena (Xdebug → debugger extension → extension),
cause (punto e virgola e stringa non chiusa → parse error; output prima di
`header()` → headers already sent), che cosa fanno var_dump, error_log (senza
apposizione), Composer, PHP-FPM, PHPUnit, OPcache, i valori di default di tre
impostazioni e la porta di Xdebug, la negazione «a warning does not stop the
script», il megabyte come unità, lo script come programma, il verbo «triggers»
per contatto. **Le specie dei problemi, dalla più frequente:** il cancello C
che rifiuta i nomi (P3, P7, P10, P11: `p0_fact_is_clean`), il contatto che non
nasce (P5, P8, P12), sintagmi e complementi persi (P1, P4, P13), apposizione
(P2), una domanda rubata o dirottata (P11, P5), il soggetto `memory_limit` (P9).
Quattro dei problemi producono una **risposta sbagliata**, non un muro: P5,
P7, P10, P13 — sono i primi da chiudere.


## ⏸ HANDOFF — lotto `2026-09-24`: ripartire qui

**Richiesta:** 5 iterazioni (F.: «usiamo gli strumenti di debug e trace unificati;
se non sono sufficienti miglioriamoli e ampliamoli»). **Chiuse 5**: la chiusura di
RI-019 (parziale del lotto precedente) e quattro nuove, RI-020…RI-023. Registro e
schede: `docs/labs/reference-iterations/2026-09-24/` (RI-019 in `2026-09-23/`).

| ID | capacità | classificazione | W / L / C | commit |
|---|---|---|---:|---|
| RI-019 | chiusura: save + processo nuovo di «flash point / melting point / working load is a relation» | trained | 3 / 3 / 0 | `729153c6` |
| RI-020 | un **nome di relazione** si lega a una relazione che la KB tiene già: «the boiling point of x is y means x boils at y» | trained | 1 / 0 / 2 | `f4d2a132` |
| RI-021 | una **domanda indiretta** si insegna, e l'iniziativa di dialogo ascolta la forza insegnata («I would like to know …») | meta-capability-only | 0 / 3 / 0 | `ea18201a` |
| RI-022 | una **contrazione** si insegna («"i'd like" is a contraction of "i would like"»), le riscritture vanno al punto fisso | meta-capability-only | 0 / 1 / 0 | `b73cea1c` |
| RI-023 | una **sigla** tecnica si insegna, si salva, si ritira e non si mangia le parole comuni («LED», «DC») | trained | 1 / 2 / 0 | vedi `git log` |

### Il trace unico, ampliato dove taceva (richiesta di F.)
Ogni diagnosi del lotto è partita da `/debug` e `/debug trace <parola>`; dove una
riga mancava, gliel'ho data lì e resta:
- `read.project` — la proiezione della risposta (topic, prova, fuoco, sorgente): era
  il sito muto dietro «answerframe answers» (la definizione dell'acqua per «the
  boiling point of water»).
- `read.polar` — che cosa ha interrogato il verdetto «I don't know: nothing I hold…».
- `read.form … asking the frame reader`, `lesson.anchor`, `lesson.forget`, e la prova
  a secco dice su **quale testo** gira (detto o canonizzato).
- `plan depends on …` anche per `turn_priority_response` (`turn_priority_support/2`,
  solo col trace profondo: costava un secondo di soft-test).
- `read.canon again …` — ogni passo di canonizzazione in più; `read.canon alias «x»
  needs capitals …`.
- `turn_yield_probe … because …` — il **testimone** di una vista di cessione
  (`yield_witness/2`, KB): «quasi una lezione» ora dice quale forma.
- `form op … reader-origin=…` — lo **strato** in cui scriverebbe una forma.

### Scoperte che valgono per ogni iterazione
1. **Le forme tarde scrivevano nello strato riflessivo** (dipendeva da dove girava il
   lettore): nessuna sigla insegnata arrivava su disco. Ora l'atto `assert` di una
   forma scrive in sessione, salvo `turn_form_effect_origin(F, reflective)`.
   ⚠ Riverificare le lezioni salvate prima del 24 settembre fatte con forme tarde.
2. **Il dispatcher anticipava la cessione di `knowledge`** (effetto del punto 4 di
   RI-019): «quasi una lezione» scattava prima delle sue forme. Ora
   `faculty_yields_after_forms/1`.
3. **Una guardia dentro una regola enumerata a ogni turno costa**: `naf(…)` dentro
   `phrase_canon/2` era +270 ms per turno (mantra #20). Le guardie si chiedono al
   momento dell'uso.
4. **L'ancora per nome di RI-020 si prendeva «x is a y»** (regressione trovata e
   curata in RI-023 con `surface_has_content_word/1`): prima di chiudere, lanciare
   i banchi puntuali delle forme toccate e confrontarli con la base in un worktree.
5. **soft-test al bordo sotto carico**: 14–18 s con la stessa base (Chrome, load
   2–2,7). Per attribuire un costo si usa il profilo per turno di `/debug` (tempo e
   query per turno, base contro modifica), non il cronometro del soft-test.

### Candidati per RI-024, con il sito già nominato dal trace
1. «I'd like to learn welding.» non apre l'attività: l'iniziativa legge i token grezzi
   della IR (`i|d|like`), non il canone — la IR si costruisce prima della
   canonizzazione (D33).
2. «What does the LED on the charger emit?», «Kanban limits work in progress» → la
   coda preposizionale di un sintagma.
3. Senza lezione «what is the R of X» con R ignoto riceve il riassunto di X: il fuoco
   della domanda non esiste per «of» (una regola `asked_head_misses` per «R of X»).
4. «Tell me the boiling point of ethanol» senza la lezione di RI-021; e la parafrasi
   non trasferisce la forza della sua àncora (servono tre lezioni dove ne basterebbe una).
5. `alias_needs_capitals/1` conosce solo le forme verbali (non «CAT», «SUN»).
6. La lezione di sigla con virgolette o apostrofo scrive ancora un fatto falso.

---

## ⏸ HANDOFF — lotto `2026-09-23` (notte del 23→24 settembre): ripartire qui

**Richiesta:** 5 iterazioni. **Chiuse 3** (RI-016, RI-017, RI-018), **1 parziale**
(RI-019, manca solo R6/R7), **0 diagnostiche**. Non contare RI-019 finché non ha
save e processo nuovo. Registro e schede: `docs/labs/reference-iterations/2026-09-23/`.

| ID | capacità | stato | W / L | commit |
|---|---|---|---:|---|
| RI-016 | una particella **avverbiale** fa parte del verbo: `step_up` ≠ `step_down` (flyback CRT, buck, amilasi) | completa | 3 / 6 | `a7ca0b1a` |
| RI-017 | che il trattino unisca una parola **si insegna** (`"-" is a word joiner`): «step-down transformer», «heat-resistant glove» | completa | 3 / 1 | `afcb4ed8` |
| RI-018 | un valore nominale si chiede alla **lettura**, non al frasario (USB 5 V, batteria 12 V, fusibile 13 A) | completa | 3 / 2 | `c2c35b11` |
| RI-019 | un nome di relazione che **finisce con un verbo** si insegna (flash point, melting point, working load) | **partial** | — | `5f58c676` |

### Prima cosa da fare domani
**Chiudere RI-019** (scheda: «Che cosa manca»): in sessione pulita
`flash point is a relation`, `The flash point of Jet A fuel is 38 degrees
Celsius.`, `melting point is a relation`, `The melting point of tin is 232
degrees Celsius.`, `working load is a relation`, `The working load of an M10 eye
bolt is 230 kilograms.`, poi `/save`; i valori sono predicati nuovi e finiscono
nella ricaduta `kb/learning/learned.p0` → spostarli accanto ai simili
(engineering.p0, science-nature.p0) — **per similarità, non per storia**. Poi
processo nuovo, replay RI-016..018, banco L2, commit. Poi RI-020 da un candidato
qui sotto.

### Lo strumento che ha trovato tutti i siti: il TRACE UNICO del turno
`9f2bf5c6`. Chiesto da F. sette volte in un mese. **Si parte da qui, non dal
grep e mai da una `fprintf` temporanea**:
- `/debug` (chat) e `!debug` (in un `.p0t`, nel log del demone) stampano la
  TRACCIA DEL TURNO: canone, forza, ogni facoltà del dispatch (`nome!yield` con la
  **regola** che fa cedere, declina, `=> risponde`), cancelli del lettore di
  classe (ora tutti con un nome), schemi legati (`frame bind`), ricevute
  (`refer`), token della IR (`ir current_turn a|step-down|…`), viste
  (`view … invalidata da …`, `… ms`), dipendenze della risposta del piano di turno
  (`plan depends on …`, da `kb_derivation`).
- `/debug trace <parola>` filtra; `PARROT0_TURN_LOG=file` salva ogni turno;
  `PARROT0_TRACE_ECHO=1` ripete su stderr.
- Se un sito tace nel trace, **gli si dà voce lì** (`p0_trace`, una riga) e resta.

### Scoperte che valgono per ogni iterazione
1. **Il turno annidato del «prima»**: misurare il limite in un processo SEPARATO
   dalle lezioni — la lettura sbagliata pre-lezione scrive fatti che sporcano il
   seguito (RI-017).
2. **La ricaduta si svuota a ogni /save**: predicati nuovi (valori, `step_up`…)
   finiscono in `learned.p0`; si spostano accanto ai simili prima del commit.
3. **Il rilevatore «quasi una lezione»** (conduct-lessons.p0) ora chiede testo
   fisso contiguo, riconosce il secondo canale (`class_surface`) e conta solo le
   forme che SCRIVONO. Se una domanda o una lezione completa viene «ceduta» da
   `knowledge`, guardare prima lì (`yield … lesson_almost_turn` nel trace).
4. **`faculty_yield_when/3` ora governa anche le facoltà del registro** (prima
   era letto solo da chi lo chiedeva): una condotta dichiarata per una facoltà
   qualsiasi ora vale.
5. **Una frase ha un verbo finito**: se il soggetto è seguito dalla copula, il
   verbo è la copula (RI-019) — la stessa regola vale ovunque si tagli un
   soggetto; non ancora applicata al lettore della prosa/IR.
6. **Costo**: ogni verbo insegnato rifà tutta la vista `extract_frame` (~2–2,8 s,
   visibile nel trace come `view extract_frame`). Cura giusta: manutenzione
   incrementale delle viste (non fatta). `soft-test` è al bordo del budget
   (15–16 s); la sera del 23 anche la base `015ce009` falliva nella stessa ora
   (turni oltre 1 s): rimisurare a macchina scarica prima di attribuire.

### Candidati per RI-020, con il sito già nominato (dal trace)
1. «What does a band filter block?» → muro, mentre «What do capacitors block?»
   risponde: domanda sull'oggetto con soggetto di più parole preceduto
   dall'articolo.
2. La cue `pass` combacia **dentro** «high-pass» (mantra #8): le cue delle domande
   vanno confrontate a parola intera (`read.aframe cue=pass` nel trace).
3. «What is the boiling point of water?» → definizione dell'acqua: la KB ha
   `boils_at(water, …)` ma nessun ponte da «boiling point».
4. «Kanban limits work in progress.» → «Work.»: la coda preposizionale si perde.
5. La particella preposizionale mai vista («onto is a verb particle») ancora non
   si insegna.
6. «I no longer treat «up» as a adverbial particle» (articolo); «Scartato: … non
   e' un concetto» è un messaggio italiano scritto nel C (mantra #16).
7. «Is a step-down transformer a transformer?»: inferenza sulla testa del sintagma.

### Stato del repository
`main`, working tree pulito dopo il commit dell'handoff. Il lotto è in
`docs/labs/reference-iterations/2026-09-23/`. Nessuna modifica dell'utente
toccata.

---

## ⏸ HANDOFF — due lotti chiusi (21 settembre 2026)

### ⚠ Aggiornamento 22 settembre — che cosa cambia per i lotti dopo L2

Il §0.5-bis ha un seguito operativo:
[`l2-upgrade.md`](l2-upgrade.md). Le quattro «cure» RI-011…RI-014 erano
decisioni **per occorrenza** (dove finisce un sintagma, quale parola è il
verbo) che si stavano curando come proprietà di una classe. Da qui in poi,
prima di scegliere uno stimolo, chiedersi: **l'errore è di una classe (L1) o
di questa occorrenza (L2)?** Se è di questa occorrenza, la cura non è una forma
di turno nuova: è una correzione della lettura con portata dichiarata
(occorrenza → parola → classe), ed esiste già per il confine di sintagma
(`kb/core/reading-choices.p0`, superfici in `LEARN_PROTOCOL.md` §L2).

Scoperte che valgono per **ogni** iterazione di riferimento:

1. **I ritiri insegnati non arrivavano su disco** — tranne `forget that …`
   (RI-006). Gli atti `op(retract)`/`op(retract_all)` non lasciavano la lapide
   `forgotten/1`: soglie, condotte e lezioni disdette parlando tornavano al
   boot. Curato in `978858c9`. **Conseguenza per i conteggi:** un `W` che
   contava un ritiro «verificato in processo nuovo» attraverso una di quelle
   forme prima di questa data va riverificato.
2. **Le scritture dentro una regola potevano far cadere il processo** (SIGSEGV,
   use-after-free nel censimento, `ff42ab99`). 114 righe KB scrivono dentro
   una regola: ogni crash «inspiegabile» in un banco di lezioni va letto con
   ASan prima di sospettare la KB.
3. **Il client dei test mentiva sui crash**: `ok … 0 passed` con demone morto.
   Ora è `FAIL`. Un banco verde con **0** asserzioni non è mai stato verde.
4. **La cache IR del turno finiva nei file curati** al `/save`
   (`input_entity_cached(current_turn, …)`, 16 righe in `learned.p0`). Ogni
   predicato che descrive il turno va dichiarato `turn_scratch/1`: controllarlo
   nel diff del salvataggio a ogni iterazione.
5. **«its» contamina i fatti appresi**: il possessivo viene legato all'ultima
   entità insegnata, e il falso si salva («relief valve opens above **opens**
   set pressure»). Nei lotti che salvano, evitare stimoli con possessivi finché
   il punto 6 di `l2-upgrade.md` non è chiuso, oppure contarli come reperti
   diagnostici, non come iterazioni complete.
6. **Una correzione di lettura non chiude la questione nata dall'errore**:
   l'offerta «Want me to learn about relief valve opens?» resta aperta e
   trattiene il turno. È il debito fatto→lettura (punto 5 di `l2-upgrade.md`).
7. **`!reset` costa ~5,5 s**, non 0,40 s (`views_warm` 3,5 s, `frame_cache`
   1,3 s): il budget dei banchi di lezione va pensato in reset, non in turni.


### Lotto `2026-09-21` — conoscenza TECNICA, su istruzione di F. (niente capitali, fiumi o filosofi)

**4 complete + 1 reperto diagnostico.** Riprendere da **RI-015**, che è già
diagnosticato e aspetta solo la cura.

| ID | capacità guadagnata | stato | `W` | commit |
|---|---|---|---:|---|
| RI-011 | un **termine tecnico di più parole** è un soggetto come gli altri (metallurgia, saldatura) | completa | 4 | `723072fd` |
| RI-012 | una frase ha **un** verbo finito, e il secondo non lo è — «direct current» non è un verbo (elettronica) | completa | 5 | `4da773f7` |
| RI-013 | **che esistano numeri con un'unità si insegna** (networking, ottica, elettrotecnica) | completa | 3 | `108d4e23` |
| RI-014 | una parola è **insieme nome e verbo**, e a dirlo è il maestro (idraulica, strutture) | completa | 5 | `b0554b7f` |
| RI-015 | la lezione che apre un verbo non funziona nella forma **nuda** | **diagnostic** | 0 | `7e62bc5d` |

**La correzione di rotta che ha cambiato il lotto.** A metà, F.: «la missione è
stata tradita dall'insegnare nuove forme di apprendibilità; mi sembra che gli
stai verificando la grammatica — forse dobbiamo mettere in discussione il C che
stiamo scrivendo». Aveva ragione: la prima cura di RI-013 era una forma di turno
più un atto in C che faceva passare **una** frase. Il caso è scritto per intero
nel **§0.5-bis**, con il test da applicare **prima** di scrivere una cura. Da lì
in poi le cure hanno avuto la forma giusta:

- RI-013: tre righe di KB, **zero C**, e una **lezione** al centro («The hertz
  measures frequency.») — il turno 4 si incolla, il 5 è la lezione, il 6 è la
  stessa frase che si legge, e l'ablazione riporta indietro.
- RI-014: il cancello dei concetti non decide più da solo, **chiede** — e ciò
  che chiede è quello che il maestro ha dichiarato.

**Il secondo errore mio, trovato e corretto dentro il lotto.** Il commit di
RI-011 non conteneva i fatti appresi: li aveva portati via un `git checkout --
kb/` fatto per ripulire i residui di `persist.p0t`, che scrive nella KB vera.
Il processo nuovo di RI-012 l'ha scoperto, RI-012 li ha reinsegnati e
risalvati. **Regola: dopo una suite che tocca la KB si ripulisce prima di
salvare, mai dopo.**

### Lotto `2026-09-20` — 9 complete + 1 parziale

| ID | capacità | `W` | commit |
|---|---|---:|---|
| RI-001 | verbo di relazione **con particella**, catena percorribile, classe come **filtro** | 8 | `1df94622` |
| RI-002 | una relazione ha un **verso** | 4 | `450a5ba8` |
| RI-003 | una **richiesta può escludere** (parziale) | 3 | `2451c96d` |
| RI-004 | una lezione **nasce nello strato che si salva** | 4 | `749299b9` |
| RI-005 | «two and a half hours» è **una** quantità | 2 | `2c24584c` |
| RI-006 | **si disdice con le parole con cui si è insegnato**, e il ritiro arriva su disco | 2 | `ee05216c` |
| RI-007 | al posto di un nome può stare una **descrizione** | 3 | `f326f6ac` |
| RI-008 | **l'insistenza** non è un turno più difficile | 1 | `3522504e` |
| RI-009 | una **cue dice che domanda è, non di che cosa parla** | 2 | `0ceee46e` |
| RI-010 | un **nome di relazione di più parole** | 2 | `cf81c5d7` |

**Contatori complessivi.** Richieste `N = 20`; iterazioni complete **13**; una
parziale, un reperto diagnostico, un candidato **scartato con motivazione**;
famiglie distinte **15**; `W` totale salvato e verificato in processo nuovo =
**48**; `X = 0` in tutte.

### Candidati per il lotto successivo, con il sito già nominato

1. **RI-015** (il primo): la forma nuda «X is a relation verb».
   `p0_parse_mention_membership` esce sul marcatore di menzione; la cura è
   accettare la forma nuda quando la classe è una **classe di parole**
   (derivabile da `metalinguistic_head/1`). Va certificata col contrasto: «A
   pump is a device.» non deve diventare una menzione.
2. Le **risposte inscatolate** rubano la superficie: «largest lake is a
   relation» riceve la risposta sul Caspio.
3. `scope_requirement/4` prodotto e non consumato
   (`kb/core/english-grammar/reading.p0:260`).
4. Il **ruolo di due parole** in «the <ruolo> di <cosa> è <valore>».
5. La coda preposizionale di un oggetto si perde («work in progress» → `work`).
6. Una **particella mai vista** non si insegna.
7. `answer_frame` fabbricate dal muro di una domanda.

### Come si è lavorato

- **`/debug` prima del grep**: cinque volte ha nominato in una riga il sito che
  la lettura del sorgente non trovava.
- La domanda giusta non è «quale codice» ma **«con quale origine»** (RI-004).
- **Ogni rosso misurato anche sull'albero pulito** prima di attribuirselo.
- **Le cure senza effetto misurabile si tolgono**, e le strade sbagliate si
  scrivono nel commento perché il prossimo non le riprovi (RI-014 ne ha due).
- `make soft-test` dopo ogni modifica al motore: sempre verde, 8-14 s su 15.

**Il prompt per ripartire è quello del §0**, e vale sulla KB appena cresciuta.

---

## 0. Metodo operativo vigente — l’iterazione di riferimento

**Questo piano si esegue per «iterazioni di riferimento».** Il coding agent
sceglie uno stimolo umano significativo che parrot0 non sa ancora affrontare,
misura quel fallimento, prepara un curriculum plausibile con dati veri e
lavora sui punti che impediscono alle lezioni di produrre apprendimento.
L'iterazione termina quando parrot0 affronta lo stimolo grazie alle lezioni,
trasferisce la capacità e la conserva nella KB versionata, con un commit che
rende verificabile la crescita.

> **ISTRUZIONE DI AMBITO: in questo piano il banco della prosa va ignorato.**
> Non lanciare `prose-record`, `prose-gate`, `prose-rung`, la ladder o r300;
> non usare i loro punteggi come obiettivo, prerequisito o criterio di chiusura.
> Le prescrizioni storiche di questo file che li imponevano sono superate.
> Qui la verifica è la catena **stimolo → lezioni → capacità → trasferimento →
> persistenza → commit**. Una lezione può contenere prosa naturale: a essere
> escluso è quel banco, non la possibilità di imparare leggendo.

**Per riprendere basta questo prompt:**

```text
Fai 10 iterazioni di riferimento secondo
docs/plans/train-the-learning-process.md.
```

L'invocazione autorizza l'esecuzione del ciclo e i commit delle iterazioni;
non è una richiesta di scrivere dieci proposte. Dopo ogni iterazione completa
si prosegue autonomamente alla successiva. Il ciclo vale per qualsiasi N
richiesto. Una richiesta di descrivere o modificare il metodo non avvia da sola
un lotto di training.

### 0.1 Che cosa conta come una iterazione

**Una iterazione = una lacuna di comprensione/insegnabilità superata attraverso
un curriculum reale**, con uno stimolo iniziale fissato e un incremento
persistente della KB. Le modifiche tecniche necessarie sono parte del ciclo;
non sono di per sé il suo risultato.

- Lo stimolo può essere domanda, richiesta composta, dichiarazione di intento,
  correzione, vincolo nuovo o continuazione di un'attività. Se dipende da una
  conversazione, lo stimolo comprende **tutto il preambolo necessario**, fissato
  e ripetuto nelle prove. Il successo può essere una risposta motivata oppure
  una mossa pertinente seguita da avanzamento osservabile.
- Il fallimento iniziale va **osservato**, non presunto dall'agente. Se parrot0
  riesce già, il candidato è scartato con motivazione; non si impoverisce la KB
  né si rende artificiosamente difficile la frase per fabbricare un rosso.
- La crescita deve comprendere **nuova conoscenza vera appresa e conservata**
  (clausole apprese in `W`, `L` o `C` di `LEARN_PROTOCOL.md`) e un guadagno
  di comprensione trasferibile, dimostrato su dati reali. Aprire soltanto un
  meccanismo è progresso infrastrutturale;
  aggiungere soltanto una curiosità enciclopedica è training di contenuti.
  Nessuno dei due basta, da solo, per contare una iterazione di questo piano.
- Ogni iterazione completa ha un **commit proprio**, con KB, prove e spiegazione
  della capacità guadagnata. Dieci iterazioni richiedono dieci chiusure causali
  riconoscibili, non dieci commit vuoti o dieci varianti di una stessa lezione.

Usare ID stabili, per esempio `RI-001`, all'interno di un lotto identificato.
Distinguere `tentativi`, `iterazioni_complete` e `famiglie_superate`. Un tentativo
fallito può produrre un reperto o un commit diagnostico, ma non incrementa il
numero di iterazioni complete. Uno stato `partial` non diventa riuscito perché
il budget finisce. Se resta un impedimento reale, riportare quante delle N
sono concluse, che cosa manca e come riprendere; non inventare le restanti.

### 0.2 Scegliere stimoli che facciano crescere la comprensione

Partire da un'esigenza riconoscibile: un'attività umana, una spiegazione utile,
un documento reale, una decisione con vincoli, una correzione da incorporare.
Cercare conoscenze vere già presenti nella KB alle quali connettere quelle
nuove. Scrivere **prima dello sviluppo** perché una persona porrebbe quello
stimolo e quale distinzione generale servirebbe per affrontarlo.

| scegliere | evitare |
|---|---|
| una richiesta naturale che esige collegare, distinguere, applicare o proseguire | il template esatto che il parser sa già leggere, scelto per facilitare il verde |
| una lacuna che un curriculum finito e motivato potrebbe colmare | una frase indecifrabile, una contraddizione fabbricata o una richiesta impossibile |
| dati reali con fonte, condizioni di validità e utilità oltre il turno | entità inventate, nonce words, fatti di test salvati, numeri adattati al parser |
| una nuova composizione di capacità o un limite di apprendimento | sostituire solo nomi e numeri nello stimolo dell'iterazione precedente |
| esiti verificabili nel merito e nella pertinenza | giudicare dal tono convincente, da una parola attesa o dal solo «Learned» |

**Controllo contro i prompt viziati:** «Lo avrei formulato così anche senza
conoscere i rami di parrot0? Quale altro problema diventa affrontabile se questa
lezione funziona?». Se la risposta è soltanto «questa frase passa», cambiare
il candidato prima di sviluppare. Non accorciare lo stimolo, togliere vincoli
o inserirvi la risposta dopo averlo visto fallire.

Nel lotto variare contesti, atti e soprattutto **cause del fallimento**.
Inserire anche iniziative e continuazioni, senza trasformare il lavoro in una
serie di domande scolastiche. La varietà dei sostantivi non dimostra varietà
delle capacità. La prima iterazione può aprire una forma; la successiva deve
metterla alla prova in un uso nuovo o aprire una distinzione ulteriore, invece
di ripetere la stessa dimostrazione con altri nomi.

### 0.3 Preparazione minima del lotto

Leggere `MANTRA.md`, `PRINCIPLES.md` e le sezioni pertinenti di
[`LEARN_PROTOCOL.md`](../../LEARN_PROTOCOL.md) su fonti, insegnamento, salvataggio,
conteggio e persistenza. Usare sempre il profilo completo, normalmente `agi`.
Le sessioni nuove azzerano la conversazione, non la KB condivisa.

Registrare stato Git iniziale, profilo, versione del motore e KB. Conservare
le modifiche preesistenti; scegliere un checkout separato quando serve a
rendere distinguibili gli incrementi. Non attribuire al lotto lavoro altrui.
Preparare una coda breve di candidati e un registro in
`docs/labs/reference-iterations/<lotto>/`. La baseline di ciascun candidato
va comunque misurata **dopo il commit precedente**, perché la KB è cresciuta.

Non serve completare il censimento delle 30 righe né costruire l'intero
manifest del §4.5 per iniziare. Il registro nasce dalla prima iterazione;
l'indicatore globale resta NC finché non esiste evidenza sufficiente.

### 0.4 Il ciclo, da eseguire nell'ordine

**R1 — Fissare lo stimolo e provare il limite.** Salvare testo esatto,
preambolo, esito semanticamente richiesto, fonte che lo rende verificabile e
risposta iniziale integrale. Spiegare il difetto: dato mancante, relazione non
composta, lezione non capita, scope perso, condotta inadeguata, stato dimenticato.
Registrare anche un caso affine già riuscito come controllo. Non chiamare
«mancanza di conoscenza» una risposta che viene dalla facoltà sbagliata.

**R2 — Scrivere il curriculum prima della cura.** Preparare lezioni naturali
con dipendenze esplicite: prerequisiti → concetti/relazioni → criterio o procedura
→ applicazione. Ogni lezione deve dichiarare ciò che aggiunge, la fonte, perché
serve e quale comportamento intermedio permetterà di verificare. Le proposizioni
nuove vanno verificate su fonti identificabili, preferibilmente primarie; per
informazioni variabili annotare data e condizioni. Usare fonti locali già
verificate oppure consultarle, senza trattare il ricordo dell'agente come prova.

Fissare anche: due applicazioni di trasferimento non insegnate (una con diversa
formulazione, una con diversa composizione o contesto reale), un contrasto in
cui la regola non deve applicarsi e i limiti di tempo/turni/esempi del curriculum.
Una soluzione non richiede la stessa stringa attesa: scrivere i fatti, le
relazioni e i vincoli che devono essere rispettati. Il docente può insegnare
le premesse vere necessarie; **non deve impartire una coppia domanda-risposta o
una frase finale da ripetere**. Le risposte ai transfer non entrano nelle lezioni.

**R3 — Tentare davvero l'addestramento.** Impartire le lezioni nell'ordine,
registrare le risposte e controllare gli effetti intermedi. Una conferma non
è una prova; interrogare o far usare ciò che dovrebbe essere stato acquisito.
Confrontare con una forma nativa affine: se fallisce anche quella, cercare il
confine di arbitraggio o consumo prima di inventare una nuova superficie.

**R4 — Riparare il punto che impedisce di imparare.** La ricerca nasce dalla
prima lezione che non produce l'effetto previsto. Localizzare la catena:
riconoscimento → arbitraggio → scrittura → rappresentazione → inferenza/condotta
→ risposta/azione. Formulare un'ipotesi che possa essere smentita e riprovare
quella lezione prima di proseguire con le altre.

Seguire la gerarchia KB-first: insegnamento, composizione di forme esistenti,
autocorrezione, e solo dove manca il supporto astratto una modifica generale
KB/C. Non inserire a mano i fatti del curriculum nei `.p0`; non mettere nel C
nomi, frasi, casi di dominio o condizioni linguistiche. Una modifica manuale
alla KB è giustificata solo come supporto generale che riapre subito la via
alle lezioni. Il nuovo motore deve poter ricevere il prossimo membro parlando.

Il coding agent **può modificare e ricompilare durante la ricerca**. Annotare
revisioni del curriculum, tentativi falliti e costi; non presentarli come la
prima prova riuscita. Non risolvere una difficoltà semplificando di nascosto il
bersaglio. Una revisione motivata dell'obiettivo crea un candidato distinto.
Se cambia il motore, usare `make soft-test` come controllo software secondo
`LEARN_PROTOCOL.md`; nessuna suite per il solo incremento KB e **nessun banco
della prosa in questo piano**. I replay causali delle lezioni restano obbligatori.

**R5 — Certificare il curriculum sul meccanismo ormai fermo.** Quando la cura
sembra pronta, congelare codice e supporti generali. Aprire una sessione nuova
sulla KB completa precedente alle nuove lezioni, quindi ripetere il curriculum
in lingua naturale. Tra il «prima» e il «dopo» di questa certificazione non
sono ammessi edit o ricompilazioni. Se ne servono ancora, tornare a R4.

| stato da confrontare | esito richiesto |
|---|---|
| motore e KB iniziali, prima della ricerca | stimolo fallito, con traccia |
| meccanismo riparato e supporti generali, senza le nuove lezioni | la capacità dipendente dal curriculum non è ancora acquisita |
| stesso meccanismo, dopo le lezioni | stimolo originario riuscito nel merito e nei vincoli |
| stesso stato, sulle due applicazioni non insegnate e sul contrasto | trasferimento riuscito; nessuna estensione indebita della regola |
| ritiro mirato di una lezione necessaria | si perde o si modifica l'effetto dipendente; resta il controllo indipendente |
| lezione vera reimpartita e salvata, processo nuovo | capacità acquisita e trasferimento ancora disponibili |

Se la sola patch risolve già tutto senza lezioni, registrare una riparazione
strutturale: **non attribuire al curriculum quell'effetto**. Non aggiungere
fatti irrilevanti per fingere che la KB sia cresciuta causalmente. Cercare un
vero episodio di apprendimento prima di contare una iterazione completa.

Le ablazioni sono selettive sulla KB viva. Se esiste un sostegno alternativo,
individuarlo e scegliere un effetto che dipenda davvero dalla lezione; non
cancellare la base per obbligare la risposta a cambiare. Una diagnosi tecnica
può ispezionare i predicati, ma la lezione e il suo ritiro devono avere forme
pronunciabili dal maestro. Reimpartire le conoscenze vere da conservare prima
del salvataggio. Per la condotta, verificare l'azione effettivamente scelta;
un nuovo nodo nel debug o una proposta mai emessa non chiudono il ciclo.

**R6 — Salvare e dimostrare la crescita persistente.** Applicare il pre-save
di `LEARN_PROTOCOL.md`: nessuna clausola falsa, fittizia, ambigua o inspiegata
attiva (`X = 0`); poi `/save`, ispezione del diff KB, provenienza e nuovo processo.
Se la sessione di sviluppo è contaminata da lezioni fallite, rifare il curriculum
verificato in una sessione nuova; non promuovere alla cieca tutto il dump.
Non usare file di prova per sostituire la KB del profilo.

Registrare `W/L/C/P/O/X` come definiti nel protocollo, distinguendo **clausole
apprese**, supporti generali scritti manualmente e conseguenze dedotte. Per
ogni fatto vero nuovo indicare fonte, lezione e uso; per ogni nuova forma,
esempio trasferito. Le righe Git e il numero di «Learned» non sono unità di
comprensione. Richiedere un incremento appreso `W + L + C > 0`, un guadagno
trasferibile effettivo e la rilettura riuscita nel processo nuovo. Se `W = 0`,
la classificazione del training resta `meta-capability-only`, mai `trained`: può
chiudere l’iterazione solo con vere acquisizioni linguistiche/strutturali via
lezione e applicazioni certificate su conoscenze reali. Una patch manuale o
la sola provenienza `P` non soddisfano il requisito. Non aggiungere fatti
accessori per gonfiare W: i dati reali devono esercitare la capacità acquisita.

**R7 — Conservare le capacità e committare.** Riprovare il controllo già verde
scelto in R1 e gli stimoli delle iterazioni precedenti del lotto, senza
reimpartire i loro curricula. Sono conversazioni di verifica sulla KB cresciuta,
non il banco della prosa. Una regressione introdotta impedisce la chiusura:
localizzare e correggere l'interferenza, poi ripetere la certificazione coinvolta.

Preparare il commit dell'iterazione con soli file pertinenti: incremento KB,
modifiche generali necessarie, catalogo delle nuove lezioni e prove. Il messaggio
nomina la capacità acquisita e l'ID, per esempio nella forma
`learn(reference): RI-001 — <capacità verificata>`. Il corpo spiega prima/dopo,
lezioni, trasferimento, crescita `W/L/C` e limiti. Il placeholder è da sostituire
con la capacità reale, non con «migliora comprensione».

Verificare `git diff --check`, leggere `git diff -- kb/` e mettere in staging
solo l'incremento dell'iterazione. Fare **il commit prima di iniziare la
successiva**, anche quando non c'è stata alcuna modifica C. La pubblicazione
dei checkpoint segue `LEARN_PROTOCOL.md` §13 e le istruzioni dell'operatore;
il requisito minimo qui è il commit locale verificabile, non una promessa di
committare alla fine. Non includere modifiche preesistenti dell'utente.

**R8 — Usare la scoperta per la prossima iterazione.** Aggiornare il registro,
incrementare il contatore solo dopo R7, scegliere il prossimo limite e ripartire
da R1 sulla KB appena cresciuta. Chiedersi quale lezione prima impossibile sia
ora formulabile, e quale nuovo contesto possa smentire l'astrazione appena
introdotta. Il curriculum successivo deve beneficiare della crescita precedente.

### 0.5 Artefatti minimi e conteggio del lotto

Per ogni `RI-...` conservare sotto `docs/labs/reference-iterations/<lotto>/`
una scheda e i transcript causalmente rilevanti. Non serve un nuovo framework
per iniziare; basta che il prossimo agente possa ripetere il percorso.

```text
ID, famiglia e contesto umano:
Commit/stato iniziale, profilo e hash del binario:
Stimolo e preambolo congelati:
Criterio di riuscita e fonti verificabili:
Risposta iniziale e limite osservato:
Curriculum iniziale: lezioni, dipendenze, fonti e budget:
Tentativi, revisioni e diagnosi (compresi i fallimenti):
Supporti generali modificati e lezione nuova resa possibile:
Certificazione sul meccanismo fermo: prima, dopo, due transfer, contrasto:
Ablazione, controllo indipendente e re-insegnamento:
Save, diff KB classificato W/L/C/P/O/X e prova nel processo nuovo:
Replay degli stimoli precedenti del lotto:
Capacità guadagnata, limiti residui, prossimo problema:
Stato dell’iterazione: completa / partial / diagnostic:
Classificazione del training: trained / meta-capability-only / partial / diagnostic:
ID univoco nel messaggio di commit e percorsi delle prove:
```

Il commit contiene la scheda con l'ID; il suo hash si ricava **dopo** il commit
con `git log -1` e si riporta nel resoconto del lotto. Non servono commit extra
per inserire nella scheda l'hash del commit che la contiene. Il registro
mostra richieste N, tentativi, complete/N, famiglie distinte, crescita KB per
iterazione e commit corrispondenti. Non sommare deduzioni o duplicati come
nuovi fatti insegnati e non convertire N commit in punti di learning-capability.

**Per dichiarare «10/10» devono esistere dieci commit di chiusura**, ciascuno
con crescita KB vera e con tutte le prove R1–R7. Una prova persa o non eseguita
resta tale; non si ricostruisce a memoria un transcript mancante.

### 0.5-bis ⛔ LA DERIVA DA EVITARE — «verificare la grammatica» invece di far crescere l'addestrabilità (F., 21 settembre 2026)

**Il caso, per intero, perché si riconosca quando ricapita.** Nel lotto
`2026-09-21` lo stimolo era una frase tecnica ordinaria:

```text
The MTU of standard Ethernet is 1500 bytes.
```

La misura che isolava la causa era buona: senza unità («… is 1500.») la frase
si legge, con un valore non numerico («… is large.») anche — **rompe l'unità**.
Fin qui il metodo funzionava. Poi la cura è andata nel posto sbagliato: una
**forma di turno nuova più un atto nuovo in C** che prendeva «1500 bytes» e lo
conservava come testo opaco. La frase passava, il banco diventava verde, e
l'effetto reale era **zero**: parrot0 non aveva imparato niente: aveva imparato
il *motore*, ricompilando, e a insegnarglielo ero stato io.

F., vedendolo: «la missione è stata tradita dall'insegnare nuove forme di
apprendibilità; mi sembra che gli stai verificando la grammatica — forse
dobbiamo mettere in discussione il C che stiamo scrivendo».

**La cura giusta, per contrasto.** La stessa lacuna, letta come conoscenza:
*che esistano numeri con un'unità si insegna*. Quali parole siano unità la KB
lo dice già con `measures/2` — una relazione **aperta parlando** («the byte
measures information»), che tiene ohm, farad e watt insegnati uno alla volta.
Allora la cura è una classe che si allarga (`measured_value`, `unit_word_kb`) e
una forma che riusa quella classe: da quel momento **un'unità nuova costa una
lezione**, e tutte le frasi che la usano si leggono senza toccare niente.

**Il test da applicare a ogni cura, prima di scriverla.**

| domanda | se la risposta è… |
|---|---|
| Dopo la cura, un maestro ottiene lo stesso effetto per un **membro nuovo** solo parlando? | **no** → la cura è nel posto sbagliato |
| Quante frasi nuove diventano leggibili? Una, o una classe? | **una** → è una verifica di grammatica |
| Chi ha imparato: parrot0, o il motore? | **il motore** → l'hai insegnato tu, ricompilando |
| Se domani serve il caso gemello, serve un altro atto in C? | **sì** → stai costruendo un frasario in C |

**Il sintomo da cui accorgersene**, perché arriva prima della diagnosi: le forme
e gli atti si **moltiplicano**, uno per superficie, e ciascuno con il suo
`turn_form_empty_reply`. Un atto nuovo si giustifica solo quando è **una cosa
nuova da FARE** (asserire, ritirare, interrogare) — è il confine che
`turn_form_act` dichiara da sé. Se l'atto nuovo *significa* qualcosa (questo
valore è una misura, questo termine è un soggetto), quel significato è
conoscenza e va in KB.

**E la conseguenza sul C già scritto.** Quando un pezzo di C impedisce a una
lezione di avere effetto, la mossa non è aggirarlo con una forma nuova: è
**metterlo in discussione**. Si nomina il sito, si dice quale conoscenza
dovrebbe leggere al posto della propria decisione compilata, e lo si iscrive
in `C_TODO.md`. Un cancello che rifiuta ciò che il maestro ha appena detto è
debito del C, non un limite del maestro.

### 0.6 Come evitare che il ciclo diventi meccanico

- **La lezione viene accettata ma lo stimolo fallisce?** Cercare chi consuma
  ciò che è stato appreso: la catena può fermarsi dopo la scrittura.
- **Il replay riesce, il transfer no?** È probabile che si sia insegnato il
  caso o scelto un'astrazione troppo stretta. Rivedere la regola, non aggiungere
  la risposta del transfer al curriculum.
- **Serve una parola nuova per ogni caso?** Distinguere membro, ruolo, relazione
  e condizione d'uso. Aprire la classe più generale che conserva la verità.
- **La lezione cambia il debug ma non la condotta?** Verificare arbitraggio e
  azione emessa prima di attribuire una nuova capacità.
- **Il ritiro non cambia nulla?** Cercare sostegni alternativi, cache o risposta
  memorizzata. Misurare la dipendenza, non forzare un fallimento artificiale.
- **La richiesta non è una domanda?** Misurare scopo compreso, vincolo mantenuto,
  prossimo passo pertinente e uso dell'esito al turno successivo. Per una
  correzione verificare che cambi il piano; per un arresto che si fermi.
- **Si torna sempre sullo stesso tipo di prompt?** Cercare un altro uso umano
  della capacità, una composizione nuova o una dipendenza che manca ancora.
- **Si sta progettando un grande framework prima di insegnare?** Tornare a una
  lezione concreta fallita e alla modifica minima generale che la rende efficace.

Il mantra #26 orienta la scelta: **una soluzione vale di più se amplia ciò che
si può insegnare**. Ogni iterazione deve mostrare sia quel guadagno sia la KB
vera che ne beneficia. Aggiornare `LEARN_PROTOCOL.md` nello stesso commit quando
si apre una nuova forma: sintassi naturale, effetto, ritiro e portata provata.

### 0.7 Rapporto con l'indicatore e con l'archivio

Il 60–65 storico non è una misura calibrata. Il campione riportava 18 righe su
30 non provate parlando, comprendeva un'intestazione e un istogramma dichiarato
su 12 casi che sommava a 10. L'indicatore generale resta **NC**, senza impedire
le iterazioni. I §§4–5 spiegano come non confondere prove locali e generalità;
una campagna adattiva progettata dall'agente non è una valutazione indipendente.

Le vecchie bande, i TODO e i referti della prosa qui sotto sono **archivio**,
non un ordine di lavoro. Anche gli strumenti del
[laboratorio della revisione precedente](../labs/train-learning-process-v2/README.md)
restano esterni a questo metodo. Le priorità concrete vengono dalla prima
lezione che fallisce nell'iterazione corrente. Il §6 è una mappa dei possibili
punti di intervento, non una lista da completare prima di iniziare.

---

> **La missione non è insegnare qualcosa a parrot0. È far crescere le strutture
> con cui una frase detta lo modifica** — finché la prosa di un maestro non è
> più un suggerimento, ma **un atto che davvero addestra**.
>
> Si agisce su **quattro elementi insieme**: la **KB viva**, la **IR**, la
> **comprensione universale** e il **mondo allargato**. Un piano che ne tocca
> uno solo si ferma: è il reperto comune di tutti i tentativi precedenti,
> consolidati qui.

Aperto il 21 settembre 2026 su richiesta di F. Consolida i tentativi sparsi in
`docs/plans/` (§2) e definisce l'indicatore **learning-capability** (§4).

---

## ARCHIVIO — handoff del 21 settembre 2026, notte (valutazioni superate)

**Resoconto storico:** i numeri di banda e le dichiarazioni «fatto» che seguono
descrivono la valutazione di quel giro. Per lo stato corrente usare l'handoff
operativo §0 e §§4–6; conserviamo qui esperimenti, comandi e fallimenti.
Le vecchie prescrizioni sul banco della prosa non si eseguono in questo piano.

**learning-capability ≈ 60–65.** Dentro la banda 61–75, non oltre. Il percorso
della giornata, con le misure: **30 contato** dal censimento (§4.5-bis) → 50
(pavimento risalito, condotta insegnabile) → 60–65 (la lettura estensibile
parlando).

### Lo stato del PIANO, sezione per sezione

**L'ordine di lavoro del §6** — ogni riga è la condizione della successiva, e
questa è la colonna che dice a che punto è:

| § | passo | stato |
|---|---|---|
| 6.1 | **guarire il canale prima di allargarlo** | ✅ **fatto** — chiusi i tre guasti dichiarati, più due trovati misurando: una policy incompleta accettata che rompeva il turno dopo, e la cessione decisa prima che le forme fossero tentate |
| 6.2 | **il pavimento: nessuna lezione riceve un muro cieco** | ⚠️ **a metà** — declino informato su una forma di condotta (T2); una lezione incompleta ora declina invece di asserire in silenzio; la redirezione «"X" is another way to say "Y"» funziona. Ma il pavimento misurato è **3 esiti onesti su 5**, non 5 |
| 6.3 | **L4 — la condotta diventa dicibile** | ✅ **fatto** — `conduct-lessons.p0`: la guardia di pertinenza si insegna, trasferisce alla classe, si ritira |
| 6.4 | **L5/S2 — una lezione che crea una forma di lezione** | ⚠️ **aperto a metà** — una lezione crea ora la **categoria** che un'altra lezione riempie (il genere di lettura), che è A3 su un pezzo della lettura; una lezione che crea una **forma** resta il gradino S2 |
| 6.5 | **la IR consumata invece che riscansionata** | ⚠️ **iniziato**: taglia vera **182** (non 349), attrezzo `p0_turn_ir_words/3`, **un sito migrato** col banco della prosa invariato; un approccio escluso da una misura |
| 6.6 | **il mondo allargato estensibile** | ⛔ non toccato — è **F1** di [`the-rational-philosopher.md`](the-rational-philosopher.md) |
| 6.7 | **la tripletta di accettazione** | ⛔ non iniziata |

**Le bande del §4.3:**

| banda | stato |
|---|---|
| 1–15 fatti · 16–30 superfici | ✅ con il pavimento **appena** sopra la sua soglia (maggioranza, non totalità) |
| 31–45 procedure e classi | ✅ `V is W followed by Z` regge la catena: **L3 × A2** |
| 46–60 condotta | ✅ l'àncora — *si insegnano **e** si ritirano* — è dimostrata |
| **61–75 la lettura** | ⚠️ **prima clausola sì** (un genere nuovo costa una lezione), **seconda no** (349 riscansioni, erano 217) |
| 76–90 mondo allargato + A3 | ⛔ |
| 91–100 la tripletta | ⛔ |

**I cinque TODO prioritari** (§ subito sotto) sono tutti chiusi: T1, T2, T3 e
T4 verdi; T5 chiusa come **misura** e non come cura — il costo di `soft-test`
è attribuito al boot (5,7 s), non al banco.

### Che cosa è vero adesso, e come si verifica in un minuto

```sh
# un GENERE di indizio grammaticale nuovo, insegnato parlando
printf '%s\n' 'the reading hedge is of kind modality' \
  'the expression forsooth marks hedge' 'forsooth the cat sleeps' '/debug' '/quit' | \
  PARROT0_SESSION= PARROT0_LANG=en PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0 | \
  grep debug_grammatical_cue
# → evidence(span(1, 1), modality, hedge)   accanto a quelle native
```

| capacità | dove | prova |
|---|---|---|
| un **genere di lettura** nuovo costa una lezione | `kb/core/taught-reading-kind.p0` | insegna, trasferisce a una seconda espressione, si ritira, compare nell'ispettore |
| la **condotta** si insegna e si ritira | `kb/core/conduct-lessons.p0` | la guardia di pertinenza, con trasferimento alla classe |
| «una forma ha concluso» è **interrogabile** | `turn_form_concluded/2` | distingue la lezione riuscita dal quasi |
| la cessione si decide **dopo** le forme | `mod_knowledge` | una condotta può guardare che cosa il turno *non* è riuscito a essere |
| una **regola KB può contribuire un nodo** alla IR | misurato, non ancora usato | `input_node(...) :- …` è visto dai consumatori |

### ⛔ L'esperimento del §6.5, fatto e FALLITO — leggere prima di ritentarlo

Il piano dice che le riscansioni sono il collo, e che finché durano la banda è
irraggiungibile. **La prima ipotesi era che bastasse un cambio solo**, e aveva
una base misurata: `split_words` è **una funzione sola**, quindi i suoi 349
chiamanti concordano già fra loro; il disaccordo è fra *quel* confine e quello
della IR. Cambiare il confine in un punto avrebbe fatto leggere a tutti lo
stesso testo.

**Provato.** `split_words` allineato al confine della IR (caratteri di parola,
più il decimale). **Misurato:**

| cancello | esito |
|---|---|
| `soft-test` | verde, un turno a 1,25 s |
| `facts`, `derivation`, `clause_content` | **tutti verdi** |
| `english_grammar_growth`, `taught_lexicon` | solo tempi, nessuna asserzione rotta |
| **piolo r300 della prosa** | **45/62 → 6/62** |

**Il banco dei `.p0t` non protegge il lettore di prosa.** Ogni suite era verde
e la comprensione era crollata dell'87%: le suite fanno turni corti, la prosa
no. **Revocato**, e r300 riverificato a 45/62.

**Che cosa se ne impara, e vale più del tentativo.** I consumatori delle
riscansioni **dipendono dalla semantica a spazi bianchi**: non si unificano
cambiando il confine: la lezione tecnica era migrare **un consumatore alla
volta**. Allora si prescriveva il banco della prosa; **quella prescrizione è
superata in questo piano**, che ora verifica le iterazioni di riferimento (§0).

### §6.5 — l'attrezzo, la misura giusta della taglia, e un esempio lavorato

**La taglia del §6.5 è la metà di quella scritta nel piano.** Contate le
riscansioni e classificate per quello che fanno nelle dodici righe seguenti:

| | |
|---|---|
| riscansioni totali | **340** |
| che **decidono** (confronto su token o classe lessicale) | **182** |
| che compongono o contano | 158 |

Le 158 non hanno un'opinione su *dove finisce una parola*: costruiscono una
risposta. Il lavoro che apre la banda sono le **182**.

**L'attrezzo che rende meccanico ogni passo:** `p0_turn_ir_words/3` restituisce
le parole del turno **come la IR le vede**, in ordine. Chi lo usa smette di
avere un confine proprio.

```text
"no, the cat is not grey"  →  [no] [the] [cat] [is] [not] [grey]
```

Nota la virgola che **non c'è**: è la divergenza che i consumatori aggiravano.

**L'esempio lavorato**, `correction_peel` in `src/brain/99-registry.c`. Il sito
documentava da sé il difetto:

> *«split_words keeps a trailing comma on the token ("no,"), so match on the
> punctuation-stripped marker.»*

Leggeva la stringa per conto suo e poi toglieva la virgola a mano. Ora chiede
le parole alla IR, e lo spogliatore sparisce con la virgola.

| cancello | esito |
|---|---|
| `make soft-test` | ✅ verde, 12 s |
| «no, the cat is not grey» | ✅ `Held: the cat is not grey. I no longer hold the opposite.` |
| **piolo r300 della prosa** | ✅ **45/62, invariato** |

**Questo era il modello proposto allora per le altre 181; il suo cancello
della prosa è escluso dal metodo vigente (§0).** Il fallimento storico resta
un reperto, non un’istruzione per le prossime iterazioni.

**L'indicatore non si muove per un sito su 182: resta 60–65.** Quello che è
cambiato è che §6.5 ha ora una taglia misurata, un attrezzo e un passo
verificato, invece di essere un muro di 349.

### Archivio — il precedente ordine di ripresa

1. **Il §6.5, un consumatore alla volta.** Allora si proponeva di sostituire
   un `split_words` decisionale con un consumatore della IR e misurare r300
   prima e dopo. **Non eseguire quella prescrizione nel metodo corrente:**
   riprendere da R1–R8 del §0.
2. **Il limite del nome multiparola** (`taught-reading-kind.p0`): un genere con
   un nome di due parole insegna ma non raggiunge la IR — l'atomo quotato non
   sopravvive dentro il termine della vista `expression_first_word`. Isolato,
   non curato.
3. **Il rilevatore del «quasi una lezione»** è collegato e funziona; restano i
   suoi due falsi negativi misurati (testo fisso parziale, sinonimi).
4. **18 dei 30 elementi del censimento non sono stati misurati**: la quota di
   catene che finiscono a mano resta una stima inferiore.

### Le due trappole pagate oggi, da non ripagare

- **`naf` con una variabile libera flounderà**, quindi la clausola non scatta
  mai e sembra che la regola non esista. Mi ha morso **due volte**. Le guardie
  si scrivono come facce unarie su termini legati.
- **Una forma di lezione che comincia con un jolly** non può avere una gemella
  di ritiro distinguibile: «forget that X is …» viene letta dalla forma di
  insegnamento con nome «forget that X». L'apertura dev'essere testo fisso.

---

## ARCHIVIO — TODO prioritari del giro precedente

Cinque attività **puntuali** (T1–T5): ognuna ha un esito binario, un costo in minuti e
una prova che la falsifica. Nessuna è una scansione della KB o un'indagine a
largo spettro — quelle si fanno dopo, e il §6 dice in quale ordine.

| # | attività | elemento (§3) | livello (§4.2) | passo del §6 | costo |
|---|---|---|---|---|---|
| ~~**T1**~~ | ✅ **fatta il 21 settembre** — le tre forme italiane insegnano; trovato e chiuso un guasto del canale (policy incompleta accettata che rompeva il turno dopo) | comprensione universale | L2 | 1 | ~20 min |
| ~~**T2**~~ | ✅ **fatta il 21 settembre** — declino informato su una forma: tre stati, tre messaggi, ognuno dice che cosa scrivere dopo | comprensione universale | — (il pavimento) | 2 | ~1 h |
| ~~**T3**~~ | ✅ **fatta il 21 settembre** — la guardia di pertinenza si insegna, trasferisce alla classe e si ritira. **Prima lezione L4** | KB viva | **L4** | 3 | ~1–2 h |
| ~~**T4**~~ | ✅ **fatta il 21 settembre** — la lingua di uno scambio aperto è quella dell'ancora; un seguito corto non la sposta più | KB viva | L4 | 1 | ~45 min |
| ~~**T5**~~ | ⚠ **misurata, non curata il 21 settembre** — il costo è attribuito (il boot, 5,7 s) e una cura è stata provata e ritirata perché dannosa; vedi la scheda sotto | — (banco) | — | 0 | ~1 h |

> **I cinque TODO sono chiusi**, con T5 chiusa come *misura* e non come cura:
> la sua scheda qui sotto dice perché, e sposta la domanda dal banco al boot.
> Il §5 riporta dove l'ago si è mosso e che cosa tiene il numero sotto 50: una
> coppia `situazione × comportamento` sola, la precedenza fra facoltà ancora
> scritta, e il censimento del §4.5 mai eseguito.

---

### T1 — Provare le tre forme di lezione italiane mai verificate

[`LEARN_PROTOCOL.md`](../../LEARN_PROTOCOL.md) dichiara quattro forme italiane
*«non ancora verificate end-to-end»*. **Una** è stata provata il 20 settembre
(`quando dico X intendo Y`) e funziona, con ablazione. Le altre tre no:

```text
la mossa conversazionale partenza concreta riguarda un esempio concreto
per la mossa conversazionale partenza concreta di Mostrami un esempio.
quando guidi apprendimento inizia con partenza concreta
```

**Perché per prima.** È il §6.1 — *guarire il canale prima di allargarlo* — e ha
l'esito più netto che esista: ogni forma o insegna, o riceve un muro, o la
prende il lettore sbagliato. Nessuna interpretazione. E i difetti, se ci sono,
non vanno **cercati**: li ha già nominati il catalogo.

**Prova.** Tre righe di tabella: replay, trasferimento a un altro argomento,
ritiro, replay. Verde = un sospetto in meno sulla banda. Rosso = tre difetti
localizzati senza aver scansionato niente.

### T2 — Declino informato su **una** forma di lezione

Oggi una lezione non riconosciuta riceve `Non capisco ancora.`, e il maestro non
sa se ha sbagliato la frase, il nome della mossa, o se la forma non esiste. Si
prende **una** forma e le si fa dire che cosa le mancava:

> *«sembra una lezione sulla condotta, ma non riconosco «partenza concreta»
> come nome di mossa»*

**Perché.** È il canale che **parla all'indietro** (§3): senza, l'addestramento
è un imbuto. Il §6.2 lo vuole sul catalogo intero; qui si restringe a **una**
forma per vedere se il meccanismo regge prima di generalizzarlo.

**Prova.** Si sbaglia la lezione in tre modi diversi e si ricevono tre messaggi
diversi, ognuno dei quali dice che cosa scrivere al turno dopo.

**Rischio dichiarato.** Potrebbe non esistere un punto unico a cui appendere il
messaggio. Se è così **si dice**, invece di forzarlo: sarebbe un reperto sulla
comprensione universale, non un fallimento dell'attività.

### T3 — La guardia di pertinenza sul turno che fallisce

Non costruire F1. Rendere **dicibile** questo, e basta:

> *«Quando il tema di una domanda è un turno di questa conversazione, non
> rispondere con l'elenco delle cose che sai.»*

**Perché.** Il turno di partenza — `in quale lingua ti ho chiesto quale lingua
sai parlare` → **`c, python.`** — non deve diventare *«in italiano»*: deve
diventare un **muro onesto**. Sulla scala di F. è un salto più grande di una
risposta giusta presa dal frasario, perché toglie la specie peggiore, *fluente e
infondato*. Ed è **L4** — la banda dove parrot0 è più debole — al costo di un
caso solo. La forma di seme è quella che il `/debug` di parrot0 già nomina da sé.

**Prova.** Il turno cambia; il **ritiro** lo riporta a `c, python`; e una
domanda diversa della stessa famiglia riceve lo stesso trattamento **senza una
seconda lezione**.

### T4 — La lingua che salta a metà scambio

```text
>>> sono indeciso tra leggere e passeggiare
Su leggere e passeggiare: Prima di scegliere, nomina un criterio…
>>> per me conta riposare
You added: riposare. On leggere e passeggiare: Use this criterion…
```

Rosso misurato e **pre-esistente** (identico sulla baseline), su un asse
dichiarato: *continuità dopo un seguito*. È il rosso 1 dell'handoff di
[`the-rational-philosopher.md`](the-rational-philosopher.md), e **la leva
esiste già**: la lingua dell'ancora è registrata, quindi la resa può leggere
quella invece di indovinare su quattro parole ambigue.

**Prova.** Un seguito corto non sposta più la lingua; un cambio di lingua
**esplicito** continua a funzionare.

### T5 — Il costo di `soft-test`

Il banco sta a **12–14s su 15 di budget** e ha toccato 16 con un fallimento
durante la sessione del 20 settembre. Misurare per file contro un worktree di
baseline, trovare il costo vero, curarlo. Le leve candidate sono già elencate
nell'handoff di [`the-rational-philosopher.md`](the-rational-philosopher.md).

**Perché non è manutenzione.** Finché il banco fallisce a intermittenza, **ogni
misura successiva è contestabile** — comprese quelle che direbbero se le
attività T1–T4 hanno funzionato. È la meno creativa e la più abilitante.

**Prova.** Cinque run consecutive sotto i 12s, e il delta attribuito a una
modifica precisa invece che al rumore.

---

### T5 — il costo di `soft-test`: misurato e attribuito, non curato

**Misura, 21 settembre 2026.** Cinque esecuzioni consecutive: **12, 12, 12, 11,
11 s** su un budget di 15. Stabile, nessuna intermittenza osservata — ma la
metà del budget ha un nome.

| file | costo | quota |
|---|---|---|
| `tests/p0t/knowledge/facts.p0t` | **9,3 s** | **80%** |
| `tests/p0t/conversation/basics.p0t` | 1,8 s | 15% |
| `tests/p0t/health.p0t` | 0,47 s | 4% |

**Dentro `facts.p0t`, il costo è uno solo.** Nove turni valgono ~3,5 s; il
`!reset` in testa al file ne vale **5,8**, misurato isolandolo in un file che
contiene solo le direttive e nessun turno. Il `!reset` è già «smart» — salta se
la configurazione non è cambiata e nulla è stato insegnato — ma qui la
configurazione cambia, quindi ricarica.

**E la ricarica è un boot.** Un boot a freddo, a vuoto, costa **5,7 s** su
156.451 fatti e 4.950 regole. La memoria di progetto registrava **0,40 s**:
il boot è cresciuto di circa **quattordici volte** insieme alla KB. Non è il
banco a essere lento: è l'avvio, e il banco lo paga una volta per file che
ricarica.

**Una cura provata e misurata DANNOSA, quindi ritirata.** Poiché ciò che un
test insegna vive negli strati di runtime, sembrava che un reset potesse
togliere per **origine** (`KB_SESSION|INDUCED|HYPOTHETICAL|REFLECTIVE|DERIVED`)
invece di rileggere il disco. Misurato: **15,8 s e cinque rossi**. Il livello
riflessivo non è scarto di sessione — contiene il modello di sé — e le viste
ricostruite costano più di quanto la ricarica risparmi. Revocata.

**Che cosa resta, con la sua evidenza.** La prova che T5 chiede — *cinque run
sotto i 12s con il delta attribuito a una modifica precisa* — **non è
raggiunta**: le run stanno a 11–12 s e nessuna modifica le ha spostate. Ma la
domanda è cambiata: non «perché il banco è lento» bensì **«perché il boot costa
5,7 s»**, che è una domanda sul motore e vale per ogni cosa, non per il banco.
Le leve candidate, in ordine di resa attesa:

1. **Il boot stesso** — è il 100% del costo di ogni ricarica. Vedi la memoria
   «KB growth degrades the engine»: lookup O(n) e un pass di boot quadratico.
2. **Una ricarica che conservi il livello curato** invece di rileggerlo: è ciò
   che la cura ritirata cercava di fare dalla parte sbagliata. Va fatta
   preservando il riflessivo e le viste, non buttandoli.
3. **`facts.p0t` che non cambi la configurazione**: provato, **non sposta la
   misura** (11 s con e senza le righe `!set`). Annotato perché non venga
   ritentato.

### Il resto, dopo

| | |
|---|---|
| **L'ordine strategico** | §6 — che cosa alza il numero, e perché ogni riga è la condizione della successiva |
| **La misura vera** | §4.5, il censimento delle catene. **È un'indagine a largo spettro**, ed è per questo che non sta qui: ma finché non si esegue, il `30–35` del §5 resta una stima e va detto ogni volta che lo si cita |
| **F1 — il turno come contenuto con un atto** | non è un'attività d'apertura: è il gradino su cui poggiano quattro facoltà di [`the-rational-philosopher.md`](the-rational-philosopher.md). Costruirlo prima che il canale regga significa costruirlo e non riuscire a insegnargli niente |
| **La tripletta di accettazione** | §4.4 — astrofisica, clone di un LLM, interprete PGN. Sono il bersaglio del 100, non il lavoro di domani |

---

## 1. Che cosa è «train-the-learning-process», e che cosa non è

### 1.1 La distinzione che fonda tutto

| | |
|---|---|
| **Addestrare parrot0** | insegnargli i minerali, i nodi, la grammatica inglese, le leggi fisiche. Cresce ciò che **sa**. |
| **Addestrare il processo di apprendimento** | far sì che una lezione possa cambiare cose che oggi **nessuna lezione può cambiare**. Cresce ciò che **può diventare**. |

Il primo è lavoro di dominio e si misura in copertura. Il secondo è lavoro di
struttura e si misura in **altezza**: fin dove, nello stack di parrot0, arriva
una frase detta in lingua naturale.

Un esempio dalla sessione del 20 settembre 2026 chiarisce la differenza meglio
di ogni definizione. Al turno

```
>>> in quale lingua ti ho chiesto quale lingua sai parlare
c, python.
```

si può reagire in due modi. **Insegnare la risposta** — e allora si è aggiunto
un fatto, e la domanda successiva formulata in un altro modo tornerà a
sbagliare. Oppure **accorgersi che manca un oggetto**: per parrot0 un turno
passato non è una cosa di cui si possa parlare, quindi «ti ho chiesto…» non ha
nulla a cui ancorarsi. Finché quell'oggetto non esiste, *nessuna* lezione su
quella famiglia può attecchire — si potrà solo memorizzare frasi.

**Questo piano connette i due lavori nell’iterazione di riferimento (§0):**
si apre una capacità di apprendere e la si esercita con conoscenza vera, fino
alla riuscita e alla persistenza. [`LEARN_PROTOCOL.md`](../../LEARN_PROTOCOL.md)
fornisce la disciplina delle fonti, delle lezioni e della crescita KB; qui
si ricerca e si ripara ciò che impedisce a quel curriculum di funzionare.

### 1.2 Perché è la missione giusta

Perché è l'unica che **compone**. Ogni dominio insegnato con lo stato attuale
delle strutture costa quanto il precedente. Ogni struttura nuova rende più
economici **tutti** i domini futuri, compresi quelli che nessuno ha ancora
nominato. È la stessa ragione per cui
[`procedura-crescita-kb.md`](procedura-crescita-kb.md) §1 conclude che non
stiamo accumulando, stiamo **distinguendo**: le otto abilità di una giornata
non erano otto fatti mancanti, erano otto distinzioni mancanti.

### 1.3 La regola anti-inganno, ereditata e non negoziabile

Da [`MANTRA.md`](../../MANTRA.md) — *anti-barare per l'apprendimento via prompt*:

> «Parlando» significa lingua naturale, non Prolog/P0 o una API serializzata nel
> testo. Se il teacher deve conoscere nomi di predicati interni, arità, tuple,
> `!assert`, MCP o la forma di `kb.assert`, non ha insegnato: ha **scritto nella
> KB attraverso un altro trasporto**. Quel risultato vale zero.

Il controllo operativo: *un esperto del dominio che ignora lo schema interno
saprebbe formulare la lezione?* Se no, ci si ferma e si amplia la
meta-comprensione; **non si espone la rappresentazione**.

E il secondo controllo, che vale specificamente per questo piano: **una lezione
che non si può ritirare non è una lezione.** L'ablazione fa parte della prova,
sempre. Senza, non si sa se il comportamento nuovo viene dalla lezione o dal
caso — e per questo progetto vale zero allo stesso modo.

---

## 2. I tentativi precedenti — che cosa hanno trovato, e che cosa resta valido

Questi documenti non sono superati: sono **i pezzi** di questo piano. Qui si
dice che cosa ciascuno ha stabilito e che cosa va ripreso.

### 2.1 Il metodo — come si trova il prossimo buco

| documento | che cosa ha stabilito | stato |
|---|---|---|
| [`radici-insegnabilita.md`](radici-insegnabilita.md) | **La catena di insegnabilità.** Per ogni abilità, quale superficie la insegna; e per quella superficie, quale la insegna a sua volta. Una catena finisce in una **radice** (primitiva del motore: legittima), in un **circolo** (una lezione che estende la propria forma: il caso migliore), o in una **riga a mano** (un buco: il prossimo lavoro). *«Una KB viva è una KB in cui ogni catena finisce in una radice o in un circolo.»* | **il metodo di misura di questo piano** |
| [`50-iterazioni-insegnabilita.md`](50-iterazioni-insegnabilita.md) | Il bersaglio non è far passare un prompt, è rendere insegnabile **la classe** a cui appartiene. 28 giri su 50 chiusi. | campagna aperta, 22 giri |
| [`fenomenologia-dei-difetti.md`](fenomenologia-dei-difetti.md), [`fix-patterns.md`](fix-patterns.md) | Le specie ricorrenti di guasto e le forme di cura, fra cui la **guardia di pertinenza** (dire quando una facoltà NON deve prendere il turno) | vivo |

### 2.2 Il livello KB — che cosa può contenere una lezione

| documento | che cosa ha stabilito | stato |
|---|---|---|
| [`teach-comprehension-via-prompt.md`](teach-comprehension-via-prompt.md) | La tesi: *ciò che parrot0 sa fare con una forma deve poter cambiare per effetto di una FRASE*. E la contro-tesi: se ogni frase-che-insegna richiede un pezzo di C che la riconosca, il canale-dialogo è un'illusione. | tesi vigente |
| [`teach-comprehension-via-mcp.md`](teach-comprehension-via-mcp.md) | Il gemello per l'altro canale, e i muri misurati: computazione ricorsiva, generalizzazioni defeasible | muri aperti |
| [`teachable-procedures.md`](teachable-procedures.md) | La conoscenza non è solo fatti: sono **trasformazioni**. Un interprete di riscrittura generico invece di un consumer C per costrutto. | realizzato in parte |
| [`due-strutture-kb-viva.md`](due-strutture-kb-viva.md) | **Definizione come espressione** e **fatto a ruoli aperti**: le due strutture che permettono di insegnare *relazioni fra relazioni* | prima realizzazione, non certificata |
| [`abstraction-ceiling.md`](abstraction-ceiling.md) | Che cosa è esprimibile come conoscenza **senza** nuovi primitivi C, e che cosa sta oltre il soffitto | mappa del possibile |
| [`insegnamento-super-umano.md`](insegnamento-super-umano.md) | Insegnare la **condotta** e rendere la IR programmabile da prosa. *«Da ora in poi al posto della parola NO dì la parola CAVALLO.»* | proposta, U0→U1 |

### 2.3 La IR — che cosa parrot0 vede, prima di capirlo

| documento | che cosa ha stabilito | stato |
|---|---|---|
| [`ir-e-predicato-variabile.md`](ir-e-predicato-variabile.md) | La IR combinata con `apply/2` — il predicato variabile — perché la lettura evolva senza che ogni evoluzione costi un ramo nel motore | forma d'arrivo |
| [`universal-input.md`](universal-input.md) | Lo scheletro gerarchico: token, span, ruoli, range — il materiale su cui tutto il resto si appoggia | motore |
| [`lettura-della-prosa.md`](lettura-della-prosa.md) §1 | **Il reperto più duro dell'intero repo:** la IR *esiste* ed è buona (albero con range di byte, ruoli, ~50 viste), ma i file di KB che la consumano sono **7**, contro **217** riscansioni `split_words` nei tre lettori maggiori. Ogni `split_words` riapre la frase con la propria idea di dove finiscono le cose. | **il collo di bottiglia** |

### 2.4 La comprensione universale — la legge che rende una lezione raggiungibile

Non è la IR, ed è l'errore da non fare: la IR è **l'oggetto** che il motore
pubblica, la comprensione universale è il **regime** che vincola chi lo usa.
Vive in [`universal-comprehension.md`](universal-comprehension.md), ed è
l'estensione operativa del manifesto [`kb-first.md`](kb-first.md).

| che cosa stabilisce | perché è decisiva **qui** |
|---|---|
| **Nessun muro cieco su una frase ben formata.** parrot0 sa estrarre la struttura di qualsiasi frase; strutture, ruoli e schemi d'intento vivono **nella KB**, non nel C | una lezione **è un turno**. Se una lezione detta in un modo non previsto riceve *«Non capisco ancora»*, il canale di addestramento è morto **prima** di arrivare alla KB |
| **Comprendere la forma ≠ saper rispondere.** Il muro cieco si sostituisce con il **declino informato**, che dimostra di aver letto la domanda e nomina l'anello mancante | è il **ritorno di informazione al maestro**. Un maestro che riceve *«non so nulla di Zembla»* sa che cosa dire dopo; uno che riceve un muro cieco non impara nulla dalla propria lezione fallita |
| **La forma rivela l'intento; l'intento dice che cosa servirebbe sapere** | è ciò che permette a una lezione di valere per la **classe** invece che per la frase: senza, ogni superficie nuova è una voce di frasario |
| **Le tre specie di lacuna** (§10): variante di superficie, costruzione mancante, forma telegrafica — con il test diagnostico che le distingue in tre turni. *Due su tre si chiudono senza mai vedere una chat* | è il **triage** del lavoro di insegnabilità: dice quali buchi si generano dalla struttura e quali vanno resi insegnabili parlando |
| **Il ramo sociale** (gen240): capire la forma vale anche quando non serve un fatto ma una mossa conversazionale | l'addestramento è una conversazione: il maestro corregge, insiste, cambia esempio. Anche quei turni devono essere compresi |

> **La frase che la lega a questo piano:** *«comprensione universale non vuol
> dire aver previsto tutto: vuol dire che ciò che è generabile dalla struttura
> è già chiuso, e ciò che non lo è si chiude parlando»* — §10.

### 2.5 Il mondo allargato — di che cosa parla ciò che viene detto

Cinque oggetti, costruiti il 20 settembre 2026 per la prosa e già eseguibili
(la mappa delle porte è in
[`the-rational-philosopher.md`](the-rational-philosopher.md) §4):

| oggetto | porta | che cosa permette |
|---|---|---|
| **contenuto** | `kb_clause/4` | menzionare senza credere |
| **atto** | `kb_act/3` + `act_layer/2` | *chi* ha fatto entrare un contenuto e a quale titolo |
| **contesto** | `holds_in/2`, `context-scope.p0` | posizioni che convivono senza cancellarsi |
| **giudizio** | `epistemic-status.p0` | positivo, negativo, entrambi, nessuno, ricerca incompleta |
| **derivazione** | `kb_derivation/4`, `supported_from_premises/1` | da che cosa viene, e che cosa cade se l'assunto cade |

**Quello che manca non è l'astrazione: è che quasi nulla è ancora un oggetto di
quell'astrazione** — e che nessuna lezione può aggiungerne uno.

### 2.6 I bersagli — dove si vede se il processo funziona

| documento | ruolo in questo piano |
|---|---|
| [`mimic-llm.md`](mimic-llm.md) | il bersaglio «clone di un LLM»: un profilo, pesi che fanno emergere la risposta, condotta imitata |
| [`motorize-the-class.md`](motorize-the-class.md) | *motorizza la classe, poi nutri il motore* — perché una tabella non scala e un motore sì |
| [`learning-mesh.md`](learning-mesh.md) | catene di addestramento su una KB condivisa: maestro → parrot0 A → parrot0 B |
| [`quanto-manca.md`](quanto-manca.md) | il precedente di una misura onesta: muri, risposte buone, **turni rubati** |

---

## 3. I quattro elementi, e perché vanno mossi insieme

```text
             ┌──────────────────────────────────────────────────────┐
     dice →  │  MONDO ALLARGATO   di che cosa si può parlare         │ ← qui nascono
             │  contenuto · atto · contesto · giudizio · derivazione │   i generi di cosa
             ├──────────────────────────────────────────────────────┤
             │  IR                che cosa parrot0 vede in un turno  │ ← qui si decide
             │  nodi · ruoli · range · forza · lingua                │   se c'è appiglio
             ├──────────────────────────────────────────────────────┤
             │  KB VIVA           che cosa ne fa                     │ ← qui vive
             │  fatti · forme · procedure · condotta                 │   la lezione
             └──────────────────────────────────────────────────────┘
      ╔══════════════════════════════════════════════════════════════════╗
      ║  COMPRENSIONE UNIVERSALE — la legge che attraversa tutti e tre:   ║
      ║  nessun muro cieco su una frase ben formata; la forma rivela      ║
      ║  l'intento; ciò che manca si NOMINA invece di tacere.             ║
      ╚══════════════════════════════════════════════════════════════════╝
```

I primi tre sono **strati**: ognuno è un posto dove stanno delle cose. Il
quarto non è uno strato, è un **regime** — la disciplina che attraversa gli
altri tre e dice come devono comportarsi. Disegnarlo come un quarto piano
sarebbe comodo e falso.

**La regola di dipendenza, che è il cuore di questo piano:**

> Una lezione può cambiare solo ciò che la **KB viva** sa esprimere; la KB può
> esprimere solo ciò su cui la **IR** le dà un appiglio; la IR può dare un
> appiglio solo a ciò che il **mondo allargato** ammette come *genere di cosa*;
> e **nulla di tutto questo si mette in moto se la lezione non viene capita** —
> che è il mestiere della **comprensione universale**.

La quarta clausola non è un'aggiunta ornamentale. Le prime tre descrivono che
cosa una lezione *potrebbe* cambiare una volta arrivata. La quarta dice se
**arriva**. Ed è la sola che lavora in **entrambe le direzioni**: fa entrare la
lezione, e fa uscire il **declino informato** che dice al maestro che cosa
insegnare dopo. Un canale che non parla all'indietro non è un canale di
addestramento: è un imbuto.

Da cui i **quattro** modi tipici di fallire, che sono i quattro modi in cui i
piani precedenti si sono fermati:

| sintomo | elemento mancante | esempio reale |
|---|---|---|
| la lezione detta in un modo nuovo riceve *«Non capisco ancora»*, e il maestro non sa perché | **comprensione universale**: muro cieco invece di declino informato | le quattro forme di lezione **italiane** del circuito del dialogo erano dichiarate e *«non ancora verificate end-to-end»* (`LEARN_PROTOCOL.md`): sembravano insegnabili. Una è stata provata il 20 settembre e funziona; le altre tre restano non verificate, e nulla lo direbbe |
| la lezione entra, il comportamento non cambia | **KB viva**: il predicato che la lezione scrive non lo legge nessuno | `greeting(ahoy)`: vero in KB, invisibile al comportamento |
| la lezione sarebbe esprimibile ma non c'è su che cosa dirla | **IR**: il turno non pubblica il pezzo di cui la lezione parla | insegnare a trattare l'avverbiale di tempo, quando la lettura lo scarta |
| la lezione non è nemmeno formulabile | **mondo allargato**: manca il genere di oggetto | «in quale lingua ti ho chiesto…»: un turno passato non è una cosa |

**Corollario operativo per chi legge questo piano.** Prima di aprire il codice,
dire a quale dei quattro appartiene il buco — e il primo si controlla per
primo, perché è il più economico da verificare e il più facile da scambiare per
uno degli altri tre. Una cura all'elemento sbagliato produce un verde che non
compone: è il modo più efficiente di perdere una sessione, e questo repo ne ha
esempi committati.

---

## 4. L'indicatore: **learning-capability**, da 0 a 100

### 4.1 Definizione: potenza, accessibilità e affidabilità dell'apprendimento

**learning-capability riguarda ciò che il sistema può imparare da un maestro
attraverso conversazione e curriculum, e quanto affidabilmente riesce a farlo
con risorse dichiarate.** Non misura quantità di fatti, numero di forme,
righe migrate o il migliore esempio disponibile.

**0** è assenza di cambiamenti appresi. **100 universale** resta il bersaglio
ambizioso di F.: acquisire capacità in contesti umani nuovi, anche strutture,
notazioni, procedure, condotte e modi ulteriori di apprendere. Non significa
onnipotenza, accesso a informazioni assenti o superamento di limiti di calcolo.
Soprattutto, **nessun campione finito dimostra «ogni cosa»**. Il piano deve
avvicinare quella frontiera senza dichiararla raggiunta tramite tre demo.

D'ora in poi distinguere:

- **Frontiera strutturale:** quali coppie L/A hanno almeno un episodio riuscito.
  Una scoperta L5 è importante anche se il sistema fallisce molte lezioni L1.
- **Affidabilità per ambito:** quante famiglie preregistrate superano l'intero
  contratto, con quante prove mancanti e quale costo (§4.5).
- **100 universale:** obiettivo aperto; non il risultato di una formula sui
  test disponibili. Un eventuale `LC_v=100` significherà solo tutti i requisiti
  del banco versione v, da sfidare con nuove versioni e contesti indipendenti.

La scala di comprensione della prosa resta distinta. Il pavimento condiviso
è la possibilità di capire una lezione, oppure di riconoscere il limite e
ripararlo. Un declino onesto migliora questo pavimento, ma non equivale ad
apprendere. Né 45/62 risposte nel merito né 3/5 esiti onesti si convertono in
punti di learning-capability.

### 4.2 Le due dimensioni che F. ha nominato

**Forza modificativa** — *che cosa* una lezione può cambiare:

| | livello | una lezione può cambiare… |
|---|---|---|
| **L0** | niente | nulla |
| **L1** | contenuto | un fatto del mondo |
| **L2** | superficie | una forma riconosciuta: un sinonimo, un indizio, una parafrasi |
| **L3** | procedura | una trasformazione, un piano, una mossa: il *come si fa* |
| **L4** | condotta | chi prende il turno, con quale precedenza, e **quando NON deve** |
| **L5** | struttura | un genere di cosa nuovo: un ruolo nella IR, uno strato del mondo allargato, **una forma di lezione** |

**Astrazione** — su *quale ordine* la lezione agisce:

| | ordine | la lezione vale per… |
|---|---|---|
| **A0** | istanza | questa risposta, questo turno |
| **A1** | classe | ogni membro della classe, anche futuro |
| **A2** | relazione fra relazioni | *«doubled x is x followed by x»*, poi *«grandparent is doubled parent»* |
| **A3** | la lezione stessa | una lezione che crea una **forma di lezione**: il circolo si chiude |

L5 e A3 sono il punto dove il sistema comincia a nutrirsi da sé. Sono anche i
due dove parrot0 oggi è più debole, e non è una coincidenza: sono gli unici due
che nessun lavoro di dominio produce come effetto collaterale.

### 4.3 Le vecchie bande diventano tappe, non percentuali assegnabili

Le bande precedenti confondevano difficoltà strutturale e affidabilità. Non
esisteva una regola che distinguesse 61 da 65; un solo esempio apriva una banda
mentre i requisiti inferiori restavano incompleti. Manteniamo le àncore come
**tappe tecniche**, senza convertirle automaticamente in punteggio.

| tappa | prova locale necessaria | cosa manca per chiamarla generale |
|---|---|---|
| contenuti L1 | una lezione modifica un fatto e il ritiro rimuove quell'effetto | correzioni, conflitti, provenienza, scala, interferenza e formulazioni non preparate |
| superfici L2 | una forma nuova è usata dal motore invariato | accessibilità delle diverse famiglie e riparazione delle forme sconosciute |
| procedure L3 | una regola opera su un membro non insegnato | composizione, precondizioni, stato, errori e trasferimento di dominio |
| condotta L4 | una guardia modifica la decisione effettiva e si ritira | precedenza fra facoltà, iniziativa, interruzione, contesto e correzione |
| struttura L5 / IR | un genere nuovo produce un effetto in un consumatore | uso condiviso, scope, provenienza, composizione, eliminazione motivata dei lettori divergenti |
| meta-apprendimento A3 | una nuova forma insegnata abilita una lezione successiva efficace | trasferimento a un'altra famiglia di lezioni e catene più lunghe; nessuna regola nascosta aggiunta a mano |
| mondo allargato | un nuovo oggetto/ruolo operativo si insegna e viene usato | obiettivi, vincoli, azioni ed esiti su contesti diversi, con continuità e revisione |
| curricula estesi | acquisizione di capacità complesse entro budget | affidabilità, novità indipendente, conservazione delle capacità precedenti e limiti riconosciuti |

Condividere i token è necessario per i consumatori che dipendono dai confini,
ma non prova che due facoltà concordino sul significato. La IR deve rendere
comuni anche le evidenze rilevanti, o rendere esplicite interpretazioni diverse
con la loro provenienza. Una sola interpretazione forzata non è comprensione.

### 4.4 La tripletta è uno stress test, non un certificato di 100

I tre curricula suggeriti mettono in evidenza lacune diverse. Devono essere
insegnati tramite conversazione sulla **stessa KB completa del profilo**,
senza predisporre profili ridotti per far passare ciascuna prova. Sessioni e
lezioni sono identificate e ripetibili; conoscenza preesistente si misura,
non si cancella. Eventuali profili diversi sono esperimenti distinti.

| curriculum | obblighi oltre la dimostrazione iniziale |
|---|---|
| astrofisica | concetti, relazioni e procedure insegnati; problemi nuovi che richiedono combinarli; unità e condizioni di validità; distinzione fra dato, deduzione e informazione mancante; correzione di una premessa senza riscrivere tutte le risposte |
| condotta ispirata a un LLM | iniziativa su prompt non interrogativi, domande pertinenti, continuità, revisione dello scopo, limiti dichiarati, arresto e ripresa; valutazione del comportamento ottenuto, senza chiamarla equivalenza complessiva a un LLM |
| notazione e procedure, a partire da PGN | grammatica, stato, applicazione e verifica delle mosse, errori spiegati, sequenze mai mostrate; poi trasferimento a una seconda notazione con convenzioni differenti |

Per ciascuno dichiarare: materiale già noto, lezioni nuove, budget del docente,
esempi riservati, correttezza verificabile, costo di inferenza, persistenza se
prevista e ritiro. Fare anche una **prova incrociata**: insegnare B dopo A e
verificare A, poi correggere A senza rompere B. Passarli isolatamente può
nascondere interferenza e dipendenza dall'ordine.

La difficoltà del 100 sta nel trasferire il processo: un curriculum che insegna
PGN non prova che il prossimo linguaggio sia apprendibile, né una guardia L4
prova autonomia continuativa. Aumentare domini, profondità delle composizioni,
ambiguità e durata produce una frontiera di prova, non una distanza lineare
misurabile con il numero di TODO mancanti.

### 4.5 Protocollo di misura v2 — specificato, ancora da eseguire

**Questo è il livello di valutazione aggregata, non il ciclo operativo.**
Si lavora con le iterazioni di riferimento del §0 e si raccoglie evidenza via
via. Non occorre completare questo manifest per avviare o chiudere un’iterazione.
Una iterazione completa è un successo locale verificato; non certifica da sola
un’intera famiglia sugli otto strati né soddisfa la prova indipendente qui prevista.
Il banco della prosa resta escluso anche da questa misura.

**Unità di misura: una famiglia di episodi di apprendimento, non un prompt.**
Parafrasi, replay e membri dello stesso schema sono prove correlate della
stessa famiglia. Non diventano dieci successi indipendenti. Il censimento
storico è il punto di partenza dell'inventario, non un campione rappresentativo
dell'intero comportamento umano.

**Passo A — congelare il denominatore.** Creare nel laboratorio un manifest
versionato delle famiglie estratte da: catalogo `LEARN_PROTOCOL.md`, capacità
esistenti nei test, decisioni effettive dei consumatori C/KB e usi continuativi
richiesti dal mondo allargato. Deduplicare le stesse catene presenti in più
pozzi. Ogni esclusione ha una ragione scritta. Usare almeno questi strati:

| ID | strato | cosa non può essere compensato da un altro strato |
|---|---|---|
| S1 | accesso e riparazione della lezione | una conferma sbagliata non è apprendimento |
| S2 | contenuti e superfici | non basta rispondere a una domanda già nota |
| S3 | procedure e composizione | non basta riempire gli slot di un esempio |
| S4 | condotta e arbitraggio | la regola deve cambiare chi agisce davvero |
| S5 | strutture di lettura e uso condiviso | un nodo solo diagnostico non è una facoltà |
| S6 | forme di insegnamento / A3 | un nuovo membro di un registro non è una nuova forma |
| S7 | mondo allargato e iniziativa continuativa | un'apertura plausibile non è progresso di un'attività |
| S8 | curricula, conservazione e scala | tre demo separate non dimostrano apprendimento cumulativo |

Un primo lotto piccolo è un **pilota del metodo**. Non aggiungere varianti
facili per gonfiare S2, non saltare S6–S8 perché non implementati. Uno strato
senza inventario rende l'indice complessivo **NC**, non viene omesso dal minimo.

**Passo B — fissare l'episodio prima del fix.** Ogni famiglia dichiara gli
obblighi seguenti e i casi sui quali verificarli:

1. Prova prima della lezione con KB completa; separare noto, assente e risposta
   erronea. Se già funziona, non attribuirlo alla lezione.
2. Lezione naturale con budget di turni/parole/esempi/tempo; niente schema
   interno né patch C/KB nascoste durante la certificazione R5. Il binario resta
   identico in quella fase; lo sviluppo e le ricompilazioni motivati di R4 sono
   ammessi prima, registrati e seguiti da una nuova certificazione.
3. Effetto sul comportamento finale, con traccia diagnostica della catena.
4. Trasferimento a membri non mostrati, a un contesto diverso e a una
   composizione non insegnata, con controlli negativi di pertinenza.
5. Correzione e ritiro selettivo: l'effetto dipendente scompare o si aggiorna
   secondo il contratto; capacità indipendenti restano. Nei circoli, identificare
   la radice meccanica e dimostrare il percorso: un ciclo disegnato non prova nulla.
6. Ripetizione in sessione nuova; persistenza solo se prevista e tramite il
   meccanismo reale. Le prove di ritiro non si fanno cancellando la KB di base.
7. Conservazione dopo un secondo curriculum e dopo interruzioni/rumore;
   ordine delle lezioni variato; tempo e memoria entro i budget preregistrati.
8. Prova riservata dopo congelamento della soluzione, con formulazioni di un
   maestro che non conosce gli schemi interni. Se il coding agent ha già letto e
   ottimizzato quei casi, sono sviluppo/regressione, non generalizzazione indipendente.

**Passo C — registrare senza comprimere l'evidenza.** Una riga di referto ha:

```text
family_id, stratum, inventory_version, L_claim, A_claim,
root_or_circle_or_manual_or_unknown, lesson, precondition,
budgets, engine_hash, kb_hash, transcript_paths,
before, effect, transfer, negative_controls, correction, retraction,
retention, interference, reserved_trial, costs, status, failure_boundary
```

Ogni obbligo porta `pass`, `fail` o `unmeasured` con un artefatto. Una famiglia
è `pass` solo se **tutti gli obblighi preregistrati** passano; `fail` se almeno
uno fallisce; altrimenti `unmeasured`. Eventuali obblighi non applicabili si
motivano nel manifest **prima**, mai dopo aver visto il risultato. Un crash,
un timeout o una lezione mal caricata è un fallimento operativo, non un caso
da togliere. Un guasto del banco è misura invalida da rifare.

**Passo D — calcolare copertura e intervallo di audit, senza falsi decimali.**
Per ogni strato s, con N famiglie fissate, G riuscite, F fallite e U non misurate:

```text
N = G + F + U
copertura_s = (G + F) / N
quota_dimostrata_s = G / N
intervallo_audit_s = [G / N, (G + U) / N]
LC_v = 100 × min_s(quota_dimostrata_s)
intervallo_audit_v = 100 × [min_s(G_s/N_s), min_s((G_s+U_s)/N_s)]
```

Il minimo impedisce che molte superfici compensino A3 o continuità assenti.
La distribuzione completa è **obbligatoria accanto al numero**: il minimo non
racconta la frontiera né il costo. L'estremo superiore significa soltanto
«se ogni caso non misurato passasse»; **non è una previsione**, un intervallo
di confidenza o una misura di vicinanza al 100. Se un inventario manca, NC.
Se un inventario è presente ma nessuna famiglia di uno strato passa, il minimo
è zero: significa nessuna copertura congiunta certificata, non assenza di
capacità locali. Questo indice operativo non usa più le vecchie bande.

Esempio esclusivamente aritmetico: 2 famiglie riuscite, 1 fallita e 2 non
misurate danno copertura 60%, quota dimostrata 40%, intervallo [40%,80%].
Cinque parafrasi riuscite della prima famiglia non cambiano questi numeri.
Questi rapporti descrivono **solo il manifest v**. Per stime su una popolazione
servono un campionamento dichiarato e indipendenza a livello di famiglia;
nessuna confidenza statistica è ricavabile dalle cinque lezioni scelte a mano.

**Passo E — evitare l'ottimizzazione del banco.** Separare sviluppo, regressione
congelata e prova indipendente. Dopo un fix, un caso riservato visto diventa
regressione; la prova indipendente successiva richiede nuovi casi. Aggiungere
una famiglia crea v+1 e richiede riportare entrambe le versioni sull'intersezione
per confrontare le release. Non cambiare oracolo, budget o denominatore insieme
al codice e raccontare la differenza come progresso dell'apprendimento.

**Misure ausiliarie, mai sommate a LC:** quota di muri ciechi, conferme senza
effetto, riparazioni concluse, trasferimento, costi del docente, costo runtime,
ritenzione, regressioni e consumatori IR verificati. Per ogni rapporto indicare
sempre numeratore/denominatore e sorgente. La percentuale di split migrati misura
un refactor; il numero di iterazioni misura chiusure locali. Nessuno dei due
misura tutta l’insegnabilità; il banco della prosa non entra nel conteggio.

## 4.5-bis. IL CENSIMENTO ESEGUITO — 21 settembre 2026

**Archivio delle misure e delle interpretazioni di quel giro.** I transcript
e i difetti restano evidenza; i passaggi di banda e i riferimenti alla vecchia
tripletta non sono più il criterio corrente. Applicare il protocollo v2 del
§4.5 prima di ricavarne un indicatore. In particolare, la categoria di lettura
nuova non certifica da sola A3.

**Questa sezione sostituisce la stima del §5 con una misura, e la contraddice.**
Il numero contato è **più basso** di quello stimato, e la ragione è una sola:
la stima guardava che cosa parrot0 *può* imparare, il censimento guarda che
cosa arriva quando il maestro non conosce la superficie esatta.

### Il campione, fissato prima di misurare

Regola meccanica, nessuna scelta a mano: ogni pozzo ordinato in modo
deterministico, un elemento ogni ⌊N/10⌋ a partire dal primo. **N = 30**, dieci
per pozzo (4.676 asserzioni `.p0t`; 199 righe del catalogo §6-bis; 80 facoltà
`mod_*`). Il campione è riproducibile con lo script di estrazione; una riga
estratta (`B1`) è risultata l'intestazione della tabella — **artefatto del
campionamento, lasciato dentro** proprio perché prova che la scelta è stata
meccanica.

**Misurati parlando: 12 dei 30.** Gli altri 18 restano da classificare, e
questa sezione non finge di averli visti.

### Il pavimento — la misura che decide tutto

Per ogni lezione: detta nella forma prevista, poi in una forma **non prevista**.
Il §4.5 punto 6 attendeva tre esiti; la misura ne trova **un quarto, ed è il
più numeroso e il peggiore**.

| lezione, detta in modo non previsto | esito | specie |
|---|---|---|
| «zorbo **belongs to** the birds» | `Learned: zorbo belong birds.` | **fatto storto, in silenzio** |
| «zorbi is the **plural form of** zorbo» | `Learned: zorbi is a plural. Learned: zorbi form zorbo.` | **due fatti storti** |
| «glorp **takes an event as its subject**» | `Learned: glorp take event as its subject.` | **fatto storto** |
| «**the opposite of** zabby **is** zibby» (ordine invertito) | `I don't understand that yet.` | muro cieco |
| «grandparent **means** parent **then** parent» | «I found the teaching pivot, but I cannot align the same variables…» | **declino informato** ✓ |

| esito | quota sul campione |
|---|---|
| lezione capita | — (nessuna delle cinque varianti) |
| **declino informato** | **1 su 5 (20%)** |
| muro cieco | 1 su 5 (20%) |
| **fatto storto entrato in silenzio** | **3 su 5 (60%)** |

**La quarta specie non era prevista dal piano, e va aggiunta al §4.5.** Un muro
cieco lascia il maestro all'oscuro; un fatto storto gli dice **«Learned»**
mentre scrive in KB una cosa sbagliata. È peggio del muro per la stessa ragione
per cui, sulla scala della prosa, una risposta fluente e infondata è peggio di
un «non so»: il maestro crede di aver insegnato e non ha nessun segnale.

### Le forme che funzionano, e fin dove arrivano

| # | forma | esito nella forma prevista | L | A |
|---|---|---|---|---|
| B2 | `X is a member of <classe>` | ✅ `what is zorbo?` → «zorbo is a birds»; `is zorbo a birds?` → «Yes» | L1 | A1 |
| B4 | `the plural of X is Y` | ✅ accettata e ritenuta | L2 | A1 |
| B5 | `X is the opposite of Y` | ✅ replay: `what is the opposite of zabby?` → «Zibby» | L2 | A1 |
| B9 | `V is W followed by Z` | ✅ **catena intera**: insegnata la composizione, dati due fatti, `who is the grandparent of carl?` → «ann» | **L3** | **A2** |
| — | `X is a Y` (base) | ✅ lezione, replay, **ablazione** (`Forgotten: animal(bob)`), replay | L1 | A1 |
| T3 | `when the topic … do not answer with …` | ✅ lezione, effetto, **trasferimento alla classe**, ritiro | **L4** | A1 |

**B9 e T3 sono i due punti alti misurati**, e sono veri: una relazione fra
relazioni e una condotta, entrambe insegnate parlando, entrambe con effetto
verificato.

### Le rotture misurate

| # | che cosa | esito |
|---|---|---|
| A5 | `X is the capital of Y` — **ablazione** | 🔴 **tre formulazioni, nessuna ritira**: «forget that bezra is the capital of nivora» → «I didn't know that anyway» e il fatto **resta**; «forget that the capital of nivora is bezra» → muro cieco. La forma semplice `X is a Y` invece ritira: il buco è della forma **relazionale** |
| A7 | forma di domanda con variabile (`when i say what shade is X i mean what colour is X`) | 🔴 muro cieco |
| A1 | procedura detta in prosa (`to double a number multiply it by two`) | 🔴 muro cieco, poi «I looked up «double» but found nothing» |
| B3 | `V is an event subject verb` | 🔴 **letta come due fatti mutilati**: `Learned: glorp is an event. Learned: glorp subject verb.` |

§4.7 è esplicito: **se ritirando la lezione il comportamento resta, il punto
non si conta**. A5 quindi non si conta, e con lei ogni abilità relazionale
della stessa famiglia.

### L'istogramma, e la banda

Sui **12 misurati**, livello più alto raggiunto:

| | L0 | L1 | L2 | L3 | L4 | L5 |
|---|---|---|---|---|---|---|
| abilità | 4 (rotte) | 2 | 2 | 1 | 1 | 0 |

Catene che finiscono in una **riga a mano**: le quattro rotture più le facoltà
`mod_*` del pozzo C non ancora risalite. Sulla parte misurata, **un terzo delle
abilità campionate non arriva affatto quando il maestro cambia una parola**.

> **learning-capability contato ≈ 30**, non 45–50.

**Perché non di più, ed è il punto del censimento.** La forza modificativa
*arriva* a L4 e ad A2 — B9 e T3 lo provano, e non sono stime. Ma il §4.1 dice
che la comprensione universale **non è un elemento fra gli altri: è il
pavimento**, e che *«se la banda di comprensione crolla, questa crolla con
lei»*. Il pavimento misurato è sotto la sua soglia: **l'80% delle formulazioni
non previste non riceve un declino informato**, e il 60% scrive un fatto
sbagliato dicendo «Learned». La banda 16–30 chiede esattamente il contrario.

**Perché non di meno.** Due punti alti sono reali e ripetibili, l'ablazione
funziona sulla forma base, e il canale ha imparato a parlare all'indietro su
almeno una forma (T2). Non è un sistema che «non impara nulla».

**Che cosa muoverebbe il numero più di ogni altra cosa**, e adesso è contato e
non argomentato: **le tre superfici che scrivono un fatto storto invece di
declinare**. Non servono forme nuove — serve che una forma *quasi* riconosciuta
smetta di essere accettata a metà. È il §6.2, e la misura dice che vale più di
tutto il resto messo insieme.

### Correzione del censimento, e chiusura di due superfici su tre (21 settembre 2026, sera)

**La prima lettura era troppo dura, e va corretta.** Chiamare quei tre esiti
«fatti storti» era sbagliato: misurando meglio, `zorbo belongs to the birds`
scrive `belong(zorbo, birds)`, e alla domanda speculare **«what does zorbo
belong to?» risponde «Birds.»**. Il fatto è **corretto e raggiungibile** — ma
solo dalla strada del verbo che l'ha scritto. La specie vera non è la
corruzione: è la **strada rotta** (`broken-roads-not-gaps`), cioè una lezione
che atterra su una relazione diversa da quella che il maestro intendeva, **in
silenzio**.

**Il blocco vero, e dove stava.** La lezione di redirezione esiste già —
«"X" is another way to say "Y"», la parafrasi che scrive `phrase_canon/2` — e
davanti a questi casi dava un **declino informato**, che è la specie giusta:

> *I do not understand «is a member of» well enough to copy it: teach me with a
> phrasing I already handle.*

Ma rifiutava un bersaglio che parrot0 **gestisce davvero**: `zorbo is a member
of birds` è una forma del catalogo e funziona. La causa è una soglia: il
bersaglio è accettato solo se una famiglia di cue ne copre almeno il
`lesson_anchor_min_cover` per cento — **40** — e il controllo misura la
*superficie*, non «so leggere questa frase».

**Misurato, non stimato.** La soglia più alta che ammette il caso legittimo:

| soglia | «belongs to» → «is a member of» | «qzwx» → «nothing understands this» |
|---|---|---|
| 40 (prima) | ⛔ rifiutato | ⛔ rifiutato |
| 35 / 30 / 25 | ⛔ rifiutato | ⛔ rifiutato |
| 20 | ✅ accettato | ⛔ rifiutato |
| **10 (ora)** | ✅ accettato | ⛔ **rifiutato** |

Il buco che la soglia proteggeva — il commento in `src/brain/00-lex.c` cita
*«qzwx nothing understands this»* — **non si riapre**: a tenerlo chiuso sono le
altre condizioni, non il 40%. La soglia era già conoscenza
(`lesson_anchor_min_cover/1`, `kb/core/intents.p0`) e il commento diceva che si
può stringere o allentare senza ricompilare: è **una riga di KB**, con la
misura accanto.

**Esito sulle tre superfici del censimento:**

| superficie | prima | dopo la redirezione insegnata |
|---|---|---|
| «zorbo **belongs to** the birds» | `Learned: zorbo belong birds.` | ✅ `Held: zorbo is one of the birds — it inherits what they have.` e `what is zorbo?` → «zorbo is a birds» |
| «zorbi **is the plural form of** zorbo» | `Learned: zorbi is a plural. Learned: zorbi form zorbo.` | ✅ `Learned: plural(zorbi, zorbo).` e `what is the plural of zorbo?` → «zorbi» |
| «glorp **takes an event as its subject**» | `Learned: glorp take event as its subject.` | ⛔ la redirezione è accettata, ma il **bersaglio stesso** si legge male: `is an event subject verb` produce due fatti mutilati. È il difetto **B3** già registrato dal censimento, ed è un'altra cosa |

**Che cosa è cambiato davvero, in termini della scala.** Non tre superfici in
più — quelle il §4.6 le conta zero. È un **rifiuto diventato una porta**: il
maestro che sbaglia formulazione ora può *dirlo* e la lezione arriva, per
qualunque superficie, non per queste tre. La redirezione è insegnata parlando,
non scritta in KB da noi.

**Resta aperto, e nominato:** il bersaglio `V is an event subject verb` si
legge come due fatti mutilati. Finché una forma del catalogo si legge male,
nessuna redirezione verso di lei può funzionare — e questa è la specie che
tiene ancora il pavimento sotto la sua soglia.

### B3 chiusa, e il terzo esito del censimento con lei (21 settembre 2026, sera tardi)

`LEARN_PROTOCOL.md` dichiara la forma `V is an event subject verb` e la dà per
verificata. Misurata su un verbo **mai visto**, non insegnava:

```text
> glorp is an event subject verb
Learned: glorp is an event. Learned: glorp subject verb.      (9,7 s)
```

Due fatti mutilati. La causa: «event subject verb» è un nome di classe di
**tre parole** e il lettore generico lo spezza — con `help`, già presente in
KB, non si vedeva. **Una forma che vale solo per i membri già presenti non è
una forma: è un ricordo**, e il catalogo la contava come capacità.

Chiusa dichiarandola, con la sua ritrattazione (`kb/core/conduct-lessons.p0`):

| prova | esito |
|---|---|
| lezione su un verbo nuovo | ✅ `Held: the subject of «glorp» can be an action or a means.` |
| **trasferimento** a un secondo verbo mai visto | ✅ `event_subject_verb(zorblax)` |
| **ablazione mirata** | ✅ `glorp` sparisce, `zorblax` **resta** |
| tempo del turno | 9,7 s → 4,9 s (ancora lento: è il costo del turno, non della forma) |

Con questa, il **terzo** esito del censimento è affrontato: non più «fatto
storto», ma una forma dichiarata che ora regge un membro nuovo.

### Riletura dell'indicatore dopo le due chiusure

| | prima | dopo |
|---|---|---|
| formulazione non prevista che diverge in silenzio | il maestro non aveva strumento | **può redirigerla parlando**: la porta esiste e funziona su ogni superficie |
| forme del catalogo che valgono solo per membri noti | B3, contata come capacità | chiusa, con trasferimento e ablazione |

> **learning-capability ≈ 35–40**, contro i **30** contati dal censimento.

**Perché sale.** Il §4.6 conta *«un muro cieco su una lezione diventato
declino informato»*: qui un **rifiuto** è diventato una **porta che funziona**,
che è di più. E una catena che finiva in una riga a mano (B3) ora finisce in
una forma con ablazione.

**Perché non arriva a 50, e va detto.** La divergenza silenziosa è ora
*riparabile* parlando, non *prevenuta*: il maestro deve accorgersene. Finché
una lezione quasi riconosciuta viene accettata a metà senza dirlo, il pavimento
resta sotto la soglia della banda 16–30 per quella specie. E **18 dei 30
elementi campionati non sono ancora stati misurati**: la quota di «riga a mano»
resta una stima inferiore.

**Il prossimo passo, ora nominato e non generico:** che una lettura *parziale*
di una lezione non asserisca nulla e lo dica — la specie, non le superfici.
È il §6.2, ed è l'unica cosa che porta il pavimento sopra la sua soglia.

### Il passo verso il pavimento: rilevatore costruito, **misurato, non collegato**

Il §6.2 chiede che una lettura *parziale* di una lezione non asserisca nulla e
lo dica. Il segnale strutturale c'è: il turno contiene il **testo fisso** di
una forma dichiarata — la parte che non varia, quella che identifica la
lezione — e nessuna forma conclude. Le forme si dichiarano da sé, quindi il
rilevatore varrebbe anche per quelle che verranno.

`lesson_near_miss/2` è scritto (`kb/core/conduct-lessons.p0`) e misurato:

| turno | scatta? | giudizio |
|---|---|---|
| «zorbo **is a member of**» (forma incompleta) | ✅ sì | vero positivo |
| «socrates is a man» | ❌ no | corretto |
| «what is the capital of france» | ❌ no | corretto |
| «the cat sleeps on the mat» | ❌ no | corretto |
| «glorp is an event subject» (testo fisso **parziale**) | ❌ no | falso negativo: chiede tutte le parole del testo fisso |
| «zorbo **belongs to** the birds» | ❌ no | falso negativo: è un **sinonimo**, non una forma incompleta |
| «zorbo is a member of birds» (forma **completa**) | ⚠️ **sì** | **ecco il blocco** |

**Perché non è collegato.** L'ultima riga: il rilevatore scatta anche quando la
lezione è completa e ha funzionato. Per usarlo servirebbe sapere che **nessuna
forma ha concluso**, e quel fatto oggi non è interrogabile dalla KB. Collegarlo
così spegnerebbe l'apprendimento ordinario — il canale principale — per un
segnale che non distingue il successo dal quasi.

**E non copre la specie del censimento.** I tre casi erano **sinonimi**, non
lezioni digitate a metà: «belongs to» non contiene il testo fisso di nessuna
forma. Sono due specie diverse, e averle separate misurando vale più che
averle confuse in una cura.

**Il prossimo passo, ora preciso:** rendere interrogabile *«in questo turno una
forma ha concluso»*. È un fatto che il motore già conosce — decide su di esso —
e che la KB non può leggere. Con quello, il rilevatore diventa una porta in una
riga; senza, resta una sonda.

### «Una forma ha concluso» è ora interrogabile — e il collegamento è stato provato e ritirato

Il passo nominato dalla sezione precedente è fatto: `turn_form_concluded(N, Forma)`
è scritto dal motore dove una forma dichiarata **conclude davvero**, indicizzato
col contatore del turno come ogni altro fatto di turno. Verificato:

| turno | `lesson_concluded(yes)` |
|---|---|
| «glorp is an event subject verb» (lezione riuscita) | ✅ vero |
| «zorbo is a member of» (incompleta) | ✅ falso |

Era il fatto che mancava: prima il rilevatore non distingueva il successo dal
quasi, e per questo non si poteva collegare.

**Collegato, misurato, ritirato.** Con la guardia `naf(lesson_concluded(yes))`
la porta è stata costruita davvero:

| turno | esito col collegamento |
|---|---|
| «zorbo is a member of» | ✅ **declina** invece di asserire in silenzio |
| «socrates is a man» | ✅ impara, nessun falso positivo |
| «bob is an animal» | ✅ impara |
| «zorbo is a member of birds» (lezione **completa**) | ⛔ **declina** |

**Il cancello della cessione si decide PRIMA che le forme vengano tentate**,
quindi la guardia non può ancora vedere il successo. Una regressione su una
lezione che funziona non è accettabile: ritirato, con il commento accanto in
`kb/core/conduct-lessons.p0`.

**Che cosa manca davvero, ora nominato al livello giusto.** Non un altro fatto:
**che la cessione possa essere decisa dopo il tentativo delle forme dichiarate**.
È una questione di *ordine del turno*, non di conoscenza mancante — ed è la
stessa specie del §6.1: il canale, non il suo contenuto. Finché l'ordine è
questo, il rilevatore resta una sonda e il pavimento resta sotto la soglia.

**L'indicatore non si muove per questo giro: resta 35–40.** Un fatto abilitante
in più e una porta provata e ritirata non sono una banda nuova, e contarli
sarebbe esattamente ciò che il §4.6 vieta.

### L'ordine del turno cambiato, la porta collegata, e il pavimento che si muove

Il blocco nominato dal giro precedente — *«la cessione si decide prima che le
forme vengano tentate»* — è stato tolto. In `mod_knowledge` la chiamata alla
porta condivisa è ora **dopo** `p0_turn_form_views`: le forme dichiarate hanno
già avuto il turno e, se una ha concluso, la funzione è già tornata. Solo
allora una condotta può guardare un fatto vero: **che cosa questo turno non è
riuscito a essere**.

Con quell'ordine, la porta sul «quasi una lezione» si collega senza regressione:

| turno | prima | ora |
|---|---|---|
| «zorbo is a member of birds» (lezione **completa**) | ⛔ declinava | ✅ `Held: zorbo is one of the birds…` |
| «zorbo is a member of» (incompleta) | `Learned: zorbo is a member.` | ✅ **declina** |
| «socrates is a man», «bob is an animal» | imparano | ✅ imparano |
| «glorp is an event subject verb» | ✅ | ✅ |
| guardia di pertinenza (T3) | ✅ | ✅ invariata |

### Il pavimento, rimisurato sulle stesse cinque sonde del censimento

| sonda | censimento | ora |
|---|---|---|
| «zorbo is a member of» | fatto storto | ✅ **declina** |
| «zorbi is the plural form of zorbo» | due fatti mutilati | ✅ **lacuna onesta** |
| «the plural of» | — | ✅ lacuna onesta |
| «glorp is an event subject» (testo fisso **parziale**) | fatto storto | ⛔ resta |
| «zorbo belongs to the birds» (**sinonimo**) | fatto storto | ⛔ resta, **ma riparabile parlando** |

| esito | censimento | ora |
|---|---|---|
| declino informato / lacuna onesta | **1 su 5 (20%)** | **3 su 5 (60%)** |
| fatto storto in silenzio | **3 su 5 (60%)** | **2 su 5 (40%)**, entrambi riparabili con una lezione |

> ## learning-capability ≈ **50**
>
> Contato, con le misure di questa sezione.

**Che cosa lo sostiene, riga per riga del §4.3:**

- **banda 16–30, la soglia del pavimento:** *«una lezione detta in un modo non
  previsto riceve un declino informato, non un muro cieco»* — ora vale per la
  **maggioranza** delle sonde (60%), e i due residui non lasciano il maestro
  senza strumento: la redirezione *«"X" is another way to say "Y"»* funziona e
  chiude il caso parlando. Al censimento questa riga era **sotto** soglia, ed
  era la ragione del 30.
- **banda 31–45, procedure e classi:** `V is W followed by Z` insegna una
  relazione fra relazioni e la catena regge fino in fondo (**L3 × A2**);
  la forma di `event_subject_verb` insegna, trasferisce a un membro mai visto
  e si ritira in modo mirato (**A1** con ablazione).
- **banda 46–60, l'àncora:** *«precedenza, cessione e guardie di pertinenza si
  insegnano e si ritirano»* — la guardia di pertinenza si insegna in lingua
  naturale, ha effetto, **trasferisce alla classe** senza una seconda lezione,
  e il ritiro riporta il comportamento di prima. E da questo giro la cessione
  si decide **nel punto giusto del turno**, che è ciò che la rendeva
  inutilizzabile per le condotte che dipendono dall'esito delle forme.

**Perché 50 e non di più.** Le bande sopra chiedono cose che non ci sono e che
non ho finto: **61–75** vuole la IR consumata invece che riscansionata (217
`split_words` sono ancora lì); **76–90** vuole il mondo allargato estensibile e
**A3**, una lezione che crea una forma di lezione — il gradino S2, tuttora una
riga a mano. E **18 dei 30 elementi campionati non sono stati misurati**: la
quota di catene che finiscono a mano resta una stima inferiore, quindi 50 è il
**limite superiore difendibile** di questa misura, non il suo centro.

**Costo pagato, dichiarato:** `taught_lexicon.p0t` passa da 6 a 7 rossi. Tutti
e sette sono **tempi**, nessuna asserzione rotta; il settimo è un turno a
1,16 s su un budget di 1,00. `soft-test` resta verde a 12 s, `facts.p0t` e
`derivation.p0t` verdi.

### La lettura diventa estensibile parlando (21 settembre 2026, notte)

La banda **61–75** chiede due cose. La prima è fatta e misurata; la seconda no,
e lo dico prima dei dettagli.

#### Prima clausola: *«un ruolo o un genere di nodo nuovo costa una lezione, non un ramo»* ✅

«the expression X marks Y» insegnava una lettura nuova **solo dentro un genere
che esisteva già**: lo slot accetta soltanto nomi dichiarati, e un genere nuovo
era una riga a mano in `function-words.p0`. Ora il genere si insegna:

```text
> the reading hedge is of kind modality
Held: «hedge» is now a reading of the modality kind. Teach me an expression
with «the expression … marks hedge».
> the expression forsooth marks hedge
> forsooth the cat sleeps
```

e l'ispettore, sulla IR di quel turno, mostra la lettura nuova **accanto a
quelle native**:

```text
debug_grammatical_cue — evidence(span(1, 1), modality, hedge)
                        evidence(span(2, 2), determination, definite)
```

| prova | esito |
|---|---|
| il genere arriva a `grammatical_cue/4`, cioè alla IR che i consumatori leggono | ✅ |
| una **seconda** espressione nello stesso genere insegnato | ✅ senza una seconda lezione sul genere |
| **ablazione**: ritirato il genere, l'indizio sparisce dal turno | ✅ |
| visibile nell'ispettore come una lettura qualunque | ✅ |

**Due lezioni che si compongono**: la prima crea la categoria, la seconda la
riempie. È il circolo che la scala chiama **A3** — una lezione che apre lo
spazio di un'altra lezione — su un pezzo della **lettura**, non del contenuto.

**Limiti misurati, non nascosti:**

- il nome del genere dev'essere di **una parola**. Con un nome multiparola
  («epistemic hedge») le prime quattro maglie tengono e `expression_at` non
  trova più l'espressione: l'atomo quotato non sopravvive dentro il termine
  della vista materializzata `expression_first_word`. Misurato, isolato, non
  curato;
- una forma di lezione che **comincia con un jolly** non può avere una gemella
  di ritiro distinguibile — «forget that X is …» viene letta dalla forma di
  insegnamento con nome «forget that X». Misurato due volte oggi. L'apertura è
  ora testo fisso, come tutte le coppie che funzionano.

**Un reperto abilitante, registrato per chi continua:** una **regola KB può
contribuire un nodo alla IR**. `input_node(...) :- …` scritto in KB viene visto
dai consumatori e dalle viste derivate (`input_node_parent` lo trova). Non
serve un ramo C per un genere di nodo: serve la forma di lezione che scriva
quella regola. È il gradino successivo, ed è ora a portata.

#### Seconda clausola: *«le riscansioni della stringa sono sparite»* ⛔

**Non è vera, e il numero non la aiuta: sono 349** `split_words` in 14 file
(erano 217 quando il piano fu scritto: il conteggio è cresciuto). Quello che è
misurabile è la **conseguenza** che la clausola nomina — *«due lettori dello
stesso turno non possono più essere in disaccordo su che cosa c'è scritto»*:

```text
scripts/fenomeni.sh flussi  →  nessun disaccordo sul corpus
```

dopo l'unificazione delle tre regole di confine di parola. **È meno di quanto
la clausola chiede**: il rilevatore guarda due flussi su un corpus di prosa,
non tutti i lettori su tutti gli input.

> ## learning-capability ≈ **60–65**
>
> Dentro la banda 61–75, non oltre: la prima clausola è soddisfatta e
> verificata, la seconda no.

**Che cosa serve per il 70, ora nominato al livello giusto.** Non un'altra
forma di lezione: **togliere le riscansioni**. Finché 349 chiamate leggono la
stringa per conto proprio, non c'è un *dove* stabile su cui una lezione possa
dire qualcosa, e ogni guadagno sulla lettura resta locale al consumatore che lo
ha ricevuto. È il §6.5, ed è l'unico lavoro che sposta questa banda.

**Costo dichiarato:** `english_grammar_growth.p0t` ha un rosso nuovo — un turno
a **1,01 s** su un budget di 1,00, nessuna asserzione rotta. `soft-test` verde
a 12 s, `derivation.p0t` verde, `taught_lexicon.p0t` invariato ai suoi sette
tempi.

### Che cosa questo censimento non ha fatto

- **18 elementi su 30 non sono stati misurati parlando** (gran parte del pozzo A
  e tutto il pozzo C). La risalita delle facoltà `mod_*` — *questa decisione
  potrebbe essere riappresa da una lezione?* — è la metà più costosa e manca.
- La quota di «riga a mano» è quindi **una stima inferiore**, non un conteggio.
- Il campione è di 30 su migliaia: dice la specie dei guasti, non la loro
  frequenza esatta nella KB intera.

---

### 4.6 Le regole di lettura — che cosa produce evidenza

Questi sono segnali di progresso locale. Non assegnano punti o bande: la misura complessiva segue §4.5.

| muove l'ago | **non** lo muove, anche se il numero sale |
|---|---|
| una catena che passa da **riga a mano** a radice o circolo | una superficie in più per una forma che già esisteva |
| una lezione che raggiunge un livello **L più alto** di prima | una lezione in più allo stesso livello |
| un'ablazione che **toglie davvero** il comportamento insegnato | un acknowledgement (*«ho imparato»*) senza cambiamento misurato |
| un **trasferimento** a un membro mai visto della classe | il replay dello stesso turno della lezione |
| una riscansione della stringa **sostituita** da un consumatore della IR | un consumatore della IR aggiunto accanto a una riscansione che resta |
| un **muro cieco** su una lezione diventato **declino informato**: il maestro ora sa che cosa dire dopo | un muro cieco chiuso aggiungendo la superficie che l'ha prodotto — chiude quel turno, non la specie |
| una specie di lacuna **generata dalla struttura** invece che attesa in chat (`universal-comprehension.md` §10, specie 1 e 2) | una variante in più registrata a mano dopo averla vista fallire |
| un genere di cosa nuovo, di cui ora si può parlare | un predicato nuovo senza una forma che lo insegni — *un cassetto senza maniglia*, mantra #26 |

### 4.7 Anti-inganno specifico dell'indicatore

- **Il maestro non conosce lo schema.** Se la lezione nomina predicati, arità o
  tuple, quel punto non si conta (§1.3).
- **Nessuna ricompilazione**, mai, fra la lezione e la prova.
- **L'ablazione è obbligatoria.** Se ritirando la lezione il comportamento
  resta, il punto non si conta: non veniva da lì.
- **Il campione non si sceglie dopo.** Le abilità del censimento si fissano
  prima di misurarle, e il banco non si allarga per abbassare la quota di righe
  a mano.
- **Una lezione che vale per una frase sola è A0**, anche se sembra una regola.
  La prova è il membro mai visto.

---

## 5. Stato corrente — evidenze locali, indicatore NC

Riferimento della revisione: `f50cdfe5` e i reperti nel §4.5-bis. Questa revisione
non riesegue il censimento delle lezioni; corregge ciò che gli si può attribuire.

| asse | evidenza disponibile | limite da conservare nel referto |
|---|---|---|
| inventario | 30 righe nel campione storico | 18 dichiarate non misurate; B1 è un’intestazione; istogramma da riconciliare (10, non 12); nessun manifest completo v2 |
| pavimento | 3/5 esiti onesti sul piccolo campione | due fallimenti; onestà e riparazione conclusa vanno separate |
| L3 | catena di composizione di relazioni documentata | nessuna copertura rappresentativa di procedure e curricula |
| L4 | una guardia insegnata con effetto, trasferimento e ritiro | la precedenza fra facoltà non diventa tutta insegnabile per questo |
| L5 / lettura | un genere può essere nominato e popolato parlando | rappresentazione nuova non basta: misurare decisione, composizione, ritiro |
| A3 | apertura di un registro popolabile | nuova forma di lezione end-to-end non dimostrata da quel solo esempio |
| IR | `p0_turn_ir_words`, primo sito `correction_peel` migrato | 340 chiamate classificate euristicamente in 182/158; non un inventario semantico definitivo |
| mondo allargato | infrastruttura e piano F1 | estensione operativa e iniziativa continuativa non certificate qui |
| prosa (archivio) | r300 45/62 dopo la vecchia migrazione; 6/62 nell’esperimento ritirato | banco escluso dal metodo corrente; nessun requisito sulle iterazioni |
| curricula estesi | bersagli definiti | tripletta, interferenza e prove indipendenti da eseguire |

**Nessuna nuova stima scalare.** Il §4.5 specifica come produrla e quando
lasciarla NC. Le cifre 30, 45–50, 60–65 conservate nell'archivio raccontano
valutazioni successive, non una serie storica omogenea. Non interpolarle.

## 6. Dove intervenire durante una iterazione

**L'ordine operativo è R1–R8 del §0.** Questa mappa aiuta a scegliere la cura
dopo una lezione fallita; non prescrive un refactor preventivo né il censimento
completo. Il punto di ingresso viene dalla diagnosi dell'iterazione.

| punto della catena | lavoro possibile | prova che la cura serve al curriculum |
|---|---|---|
| canale della lezione | riconoscimento, conferma veritiera, correzione, ritiro | la lezione prima inefficace produce esattamente la modifica dichiarata |
| pavimento e riparazione | nominare la parte mancante, permettere al maestro di completarla | l'interlocutore arriva a insegnare; il solo declino cortese non basta |
| condotta | insegnare pertinenza, precedenza, cessione e arresto | cambia la decisione realmente emessa, anche sul transfer |
| L5/S2 e A3 | rendere usabile una struttura nuova o insegnabile una nuova forma di lezione | la forma insegnata abilita una seconda lezione e un effetto su un caso nuovo |
| IR consumata | far condividere a un consumatore le evidenze necessarie invece di rileggere con confini privati | il curriculum funziona con scope, negazione e vincoli preservati, sullo stimolo e sui transfer |
| mondo allargato | usare scopi, vincoli, passi, esiti e revisioni come oggetti insegnabili | l'attività prosegue in modo pertinente dopo una risposta parziale o un vincolo corretto |
| curricula e scala | comporre le nuove acquisizioni con quelle delle iterazioni precedenti | nuova capacità e capacità precedenti persistono insieme, entro costi dichiarati |

Per A3 distinguere una categoria nuova riempita da una forma esistente da una
**nuova forma di lezione**. Certificare la catena forma insegnata → lezione
successiva → effetto → trasferimento → ritiro selettivo, dichiarando se ritirare
la forma incida soltanto sulle lezioni future o anche sulle derivazioni passate.

La migrazione IR si fa quando il curriculum ne rivela la necessità, un
consumatore alla volta. La vecchia classificazione 340/182/158 guarda le dodici
righe dopo ciascuna chiamata: serve a orientare un audit, non dimostra che le
158 chiamate che contano o compongono siano innocue. Anche quelle possono
influire su una decisione. Documentare il contratto del sito e verificarlo
nell'iterazione; **non riattivare il banco della prosa**.

Se si modifica il motore, il controllo software segue `LEARN_PROTOCOL.md`
(`make soft-test`); le verifiche decisive restano i replay conversazionali
R5–R7. Controllare anche gli errori di caricamento `.p0`, arità e numero di goal
in `src/kb.h`: una regola scartata non prova che il sistema non possa impararla.

## 7. Invocazione, prosecuzione e consegna

L'invocazione **«fai N iterazioni di riferimento»** con questo piano avvia il
§0. Non partire dalla tabella delle vecchie bande, da r300 o dalla scrittura
di una nuova suite. Scegliere un primo stimolo utile, dimostrare il limite e
scrivere il curriculum prima della cura.

Dopo ogni iterazione, consegnare nel registro: stimolo e fallimento iniziale,
lezioni e fonti, ostacolo riparato, risposta finale, transfer e ritiro,
crescita KB, persistenza e commit. Proseguire fino a N complete senza chiedere
conferma fra una e l'altra, salvo impedimenti che richiedano davvero l'operatore.
I fallimenti non si nascondono e non si contano come chiusure.

Il resoconto finale riporta **complete/N**, tentativi, famiglie distinte, capacità
acquisite, crescita `W/L/C` per iterazione, percorsi delle prove e hash dei
commit. Dichiarare limiti e regressioni ancora aperte. Il valore LC resta NC
quando manca il manifest necessario: questo non svaluta i risultati locali e
non autorizza a inventare un punteggio dai commit prodotti.
