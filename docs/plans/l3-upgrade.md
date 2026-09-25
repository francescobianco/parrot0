# L3 — insegnare a parrot0 per contatto, senza schemi di lezione

**Piano di indirizzo e progettazione operativa, 24 settembre 2026.
Stato (25 settembre, notte fonda): I0–I4 chiusi; I5 primo circuito (§26); **R3 per contatto e l'acetone del §15 fatti** (§27). **§28 H1 fatto** (una condizione del contatto si impara per contatto). **Il piano NON è chiuso:** manca il criterio d'esito del §7 e la prova ricorsiva sulle politiche (§16.3). L'elenco completo di ciò che manca, e la direzione ricavata dai commit, sta nell'handoff «che cosa manca per chiudere». Soft-test verde (15 s, al limite).**
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

> **L2 E L3 CONVIVONO (F., 24 settembre 2026, §18).** Ciò che L2 dimostra di
> saper fare oggi resta: è una capacità di basso livello su cui L3 si appoggia.
> L3 nasce **accanto** a L2, non al suo posto: non si butta via niente che
> funzioni, e le cure lungo la strada sono guardie conservative, non smontaggi.

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

## HANDOFF vivo — che cosa manca per chiudere L3, 25 settembre 2026: RIPARTIRE QUI

> *F.: «non credo che si riparta da dove indicato… dai commit dovresti derivare
> una direzione differente»*, poi *«cosa manca per poter considerare questo
> piano completato»*. L'handoff R3 qui sotto proponeva il banco §16.2; F. l'ha
> respinto come punto di ripartenza.

### La direzione ricavata dai commit: percezione → lingua

I cinque commit da `c7bbee9a` a `9f3143a0` hanno fatto crescere **solo
`kb/core/contact.p0`** (~+560 righe, 774 in tutto). Ogni passo ha dato
all'**osservatore** occhi suoi: lo span del nome dopo il possessivo
(`contact_ref_span`), la relazione nominata dalle sue parole
(`contact_named_relation`), i ruoli presi dai fatti (`contact_fact_pair`), i
valori quantità con la superficie della KB (`contact_quantity_value`), la
negazione e le contrazioni (`contact_negated`, `contact_contraction`). Intanto la
lettura condivisa è rimasta indietro: il §27.5 lo ammette (il chunker legge
ancora `acetone_boils` e `its boiling`; «boils at» vale solo
nell'osservazione).

È la critica del §24 allo specchio. Il §24 ha stabilito che le **conclusioni**
del contatto devono essere lingua («nessun lettore deve sapere che esiste il
contatto»). Vale lo stesso per la sua **percezione**: il contatto non deve
vedere con occhi suoi (§14.2: niente parser privato; CLAUDE.md: una proposta è
giusta se aumenta ciò che *parrot0* vede, non ciò che vede un suo pezzo). Ciò
che l'osservatore ha dovuto scoprire da sé per allinearsi è una correzione della
lettura condivisa. Va concluso **nella stessa rappresentazione** che scriverebbe
una lezione L2 (`reading_boundary_lesson`, `reading_continuer_lesson`, forme di
frase, `answer_frame`), con lo stato d'ipotesi sopra. `contact.p0` deve
**accorciarsi**, come vuole il test del bilancio (mantra #18a).

**Misurato sulla KB viva (make chat, profilo agi, sessione vuota):**

| turno | risposta | che cosa dice |
|---|---|---|
| «Does ethanol boil at 78 degrees Celsius?» | **«No.»** — falso | `read.aframe cue="celsius" pred=celsius_to_fahrenheit`: la relazione la sceglie l'unità dentro il valore; la procedura è chiusa per definizione, quindi `closed_world_answer` autorizza il «No» |
| «Acetone boils at 56 degrees Celsius.» | «56 degrees celsius.» | nessuna lettura dell'affermazione |
| «At what temperature does ethanol boil?» | muro | il verbo non sceglie `boils_at` |
| «Mercury freezes at -39 degrees Celsius.» | «Learned: mercury freeze at 39 degrees celsius.» | il segno meno è perso |

**Tentativo interrotto (non committato, bozza fuori dall'albero).** La regola
condivisa `relation_named_here(T, Surface, R)` (in grammar.p0, accanto a RI-020:
due parole in fila del turno nominano una relazione del mondo, anche con il verbo
alla radice via `finite_present_of/2`), più
`answer_frame(Surface, R) :- relation_named_here(current_turn, Surface, R)` e
`answer_frame_turn_arg(Cue, Pred, 0)` per le altre cue.

- Sulla IR il «No.» falso **sparisce** e diventa un muro onesto, ma il turno
  passa da 0,8 a **2,5 s**.
- La versione come vista materializzata su `turn_surface_token`, durante il
  turno, è **vuota**: la stessa query dopo il turno risponde. Non è confermato
  se sia la vista a non invalidarsi a metà turno. Se lo è, il difetto tocca
  anche `scenario_claim` (situation.p0), che dichiara la stessa dipendenza. Da
  verificare prima di riprendere.
- Anche con la relazione giusta la polare non può dire «Yes»: `boils_at(ethanol,
  "78 degrees Celsius, lower than water")` ha per valore della prosa, non una
  quantità (l'ostacolo del §15.2).

### Che cosa manca per chiudere il piano

**1. Il criterio di successo del §7 — il test d'esito del piano.** Tre lezioni
rifatte **solo per contatto**, più un quarto strumento mai usato, con un
transcript che superi la prova G3 (§5):

- punto di ebollizione: fatto (§27), ma con il setup di ritiro e con una doppia
  menzione costruita dal maestro;
- «An LED, a light-emitting diode, emits light» (apposizione → sigla):
  **non fatto**; il circuito impara solo relazioni con il ruolo ripreso da un
  possessivo o da un pronome;
- «I'd like to know — I mean, I would like to know —» (riparazione,
  contrazione): **non fatto**;
- PWM tra parentesi, il quarto strumento: **non fatto** («in other words» del
  §26 vale per le relazioni, non per l'apposizione);
- la prova G3 del transcript: mai eseguita.

**2. La prova ricorsiva (§12.4, §15.3, §16.3).** Con lo stesso ciclo oggi si
corregge solo il **vocabolario** degli strumenti. Restano scritte
dall'ingegnere, e non correggibili parlando, la condizione strutturale (ruolo
ripetuto, coreferenza, verso del possessore), la regola d'induzione, la politica
di credito e il cancello di consolidamento (§26.4 residuo 1–3). Il §16.3 dice
che il progetto è smentito se «una politica appresa non può essere corretta
dallo stesso ciclo»: finché è così, si ha un primo apprendimento per contatto,
non la chiusura di L3.

**3. Le righe mancanti del banco §16.2.** Sondate il 25 settembre sulla KB viva:

- «Paris, the capital, is large.» → «I don't understand that yet»: nessun alias
  falso, ma nessuna lettura;
- «An LED, a kind of lamp, emits light.» → «light-emitting diode.»; poi «Is an
  LED a lamp?» → non lo sa: la classe non è letta (niente identità falsa);
- «John says that Einstein was born in Paris.» → **«Parisian.»**; «Where was
  Einstein born?» → ulm, quindi il fatto non è tenuto, ma la citazione non è
  attribuita a John; «Was Einstein born in Paris?» → «cannot settle … einstein
  born in in paris» (un «no» guadagnabile da `born_in` noto non è dato, e la resa
  duplica «in»).

Non scritte: correzione senza «No» e «No» senza correzione; manutenzione di
`absent` e degli aggregati quando si **aggiunge** un fatto; limite di ricerca e
overflow → `incomplete`, non falso; effetto tentato durante la prova; ablazione
del consumer (togli il contatto / togli il candidato / riattiva il consumer).

**4. Il contratto del §14 ancora aperto.**

- §14.3: nessuna prova in un contesto d'ipotesi senza effetti; il contatto
  scrive episodi globali dopo la risposta.
- §14.4: nessuna domanda discriminante **prima** dell'uso (esiste solo «competing
  readings» su domanda). Non esiste nemmeno la verifica indipendente che toglie
  la riserva: oggi la riserva non si toglie mai.
- §14.6: i cicli di sostegno H1→H2→H1 e le viste congelate che contengono un
  fatto sospeso.
- §14.7: `incomplete` riprendibile; mediana e coda dei turni rispetto al binario
  di base.

**5. Percezione → lingua (§27.5 e la direzione qui sopra).** Il chunker
(`acetone_boils`, `its boiling`); «boils at» come lettore delle affermazioni; il
«No.» falso della polare; il possessore sull'oggetto («Rome is its capital»,
§24.1); il verbo con il soggetto ripreso per nome; i valori di
`boils_at`/`freezes_at` come prosa invece che come quantità; il segno meno perso
nell'affermazione di `freezes_at`.

**6. Costo e lingua.** `make soft-test` è a 15 s su 15, senza margine; un turno
di contatto costa 7–13 s, più 5–6 s di ricostruzione di `extract_frame` al turno
dopo. Le frasi italiane del §19.3 non sono verificate. Il «perché» di un ritiro
non dice chi l'ha ritirato né quando.

Le domande aperte del §10 non sono criteri di chiusura, ma restano senza
risposta.

**Criterio di chiusura, dichiarato:** L3 è chiuso quando i punti 1 e 2 passano
sulla KB viva, senza ritiri che la amputino e senza schemi nuovi (§1-bis), con il
banco §16.2 completo (punto 3). I punti 4–6 sono condizioni del contratto: un
punto aperto si può dichiarare residuo solo con la sua misura.

**Aggiornamento, stesso giorno (F.: «parti dalle cose che abilitano
l'apprendimento di ordine superiore»).** Il lavoro è ripartito dal punto 2, non
dal 5. §28: le condizioni dell'osservatore diventano luoghi di ordine superiore.
**H1 fatto** (§28.5): il modo di riferirsi «per nome» si impara per contatto e si
corregge con i controesempi (26/26, regressioni L3 verdi, zero C). Il prossimo
passo è la condizione «ruoli = entità della lettura» (§28.3), cioè il difetto
del chunker affrontato come lezione di ordine superiore. La proposta qui sotto
resta come alternativa.

**Prossimo passo proposto (prima del §28):** il punto 5, a partire dal «No.» falso. Prima si
chiarisce se la vista su `turn_surface_token` si invalida a metà turno; poi si
porta `relation_named_here` nella grammatica condivisa a un costo ≤ +10 % sul
turno e si toglie `contact_named_relation` da `contact.p0`.

## HANDOFF precedente — R3, 25 settembre 2026, notte fonda

**Fatto:** R3 per contatto e l'esperimento dell'acetone (§27). Un nome di più
parole («home town», «raw material», «boiling point») si impara da un contatto
ordinario ed entra nella lingua come `relation_noun`; l'acetone passa con
quantità e senza cornice per «boils at», trasferisce a etanolo e azoto, legge
un'affermazione in prosa (metanolo) con la provenienza, e il ritiro sospende.
Banco `docs/labs/l3/R3/nomi-composti.p0t` **23/23**; durata
`docs/labs/l3/R3/persistenza.sh` (solo `make chat`). Regressioni L3 tutte verdi
(I2 35/40/16/13/19/7, I5 30). Zero C.

**Trovato e curato:** un turno **senza fine** dopo il ritiro di una costruzione
(anche parlato), per ritorno indietro nella via delle entità con viste sporche
(§27.3, trovato con gdb sui goal di `solve_frame`); il budget di una query sola
che faceva fallire in silenzio la forma (`contact_settle`).

**⚠ Soft-test verde a 15 s su 15:** senza margine. Se sfora, prima cosa da
guardare: i tre contabili dopo la risposta (`contact_reading`, `contact_settle`,
`contact_near`) scandiscono i fatti del turno (`kb_turn_act/4`) a ogni livello.

**Prossimo passo proposto:** il banco §16.2 mancante (Paris/the capital, LED,
citazione altrui, `absent`/aggregati, limiti di ricerca, ablazione del
consumer), poi la domanda discriminante prima dell'uso.

## HANDOFF precedente — I5, 25 settembre 2026, sera tardi

**Fatto:** I5, primo circuito (§26): uno strumento del contatto mai visto
(«in other words») si induce da due contatti quasi riusciti su relazioni
diverse, si usa su una relazione tenuta fuori, e uno strumento sbagliato
(«moreover») cade con i controesempi ordinari degli episodi nati attraverso di
lui. Banco `docs/labs/l3/I5/strumenti.p0t` **30/30**; durata provata con
`docs/labs/l3/I5/persistenza.sh` (solo `make chat`). Zero C, zero forme.

**Che cosa NON chiude L3** (§16.1, §16.3): R3 e l'esperimento dell'acetone
(§15); il banco §16.2 per Paris/the capital, LED, citazione, `absent`/aggregati,
limiti di ricerca, ablazione del consumer; la domanda discriminante prima
dell'uso; la condizione strutturale (ruoli ripetuti, riferimento) non è ancora
correggibile per contatto, solo il vocabolario degli strumenti (residuo §26.4).

**Prossimo passo proposto:** R3 (residuo di più parole come candidati
alternativi), che riapre l'acetone e mette alla prova l'induzione del §26 sui
nomi composti; poi il banco §16.2 mancante.

## HANDOFF vivo — audit di crescita, 25 settembre 2026 (chiuso)

**Richiesta:** verificare l'implementazione rispetto all'ambizione di crescita
per contatto con agenti maestri, migliorarla dove necessario e conservare qui
diagnosi e stato ad ogni passaggio significativo. Working tree iniziale pulito.

**Prima lettura, non ancora una misura:** `kb/core/contact.p0` collega davvero
le ipotesi al lessico condiviso (`relation_noun`, `construction_frame`,
`answer_frame`), ma `contact_support` conta tuple di episodi, non prove
indipendenti; l'osservazione avviene dopo la risposta e legge fatti che quella
stessa risposta può aver appena scritto. Due tuple bastano a togliere la
riserva. Il contratto del §14.4 richiede di escludere l'autoconferma, quindi
questo è il primo punto da falsificare. Il §14.6 (conseguenze e sostegni) e I4
(persistenza verificata) non risultano certificati dai banchi attuali.

**Misurato prima della cura:** `audit-crescita.p0t`, 6 verifiche passate e 3
fallite. Dopo «Einstein was born in Ulm, so his Geburtsort is Ulm.», il turno
«The Geburtsort of Kant is Konigsberg, so his Geburtsort is Konigsberg.»
produce un secondo sostegno e la domanda su Galileo risponde `pisa.` senza
riserva. Ritirato il ponte, `born_in(kant, konigsberg)` resta dimostrabile.
È autoconferma, poi una conseguenza orfana: §§14.4 e 14.6 violati in un
dialogo naturale, non soltanto incompleti sulla carta. `make soft-test`:
16/16 verifiche verdi, **16 s > 15 s**, prima di modifiche funzionali.

**Banco aggiunto prima della cura:** `docs/labs/l3/I2/ambiguita.p0t`, due
contatti che propongono la stessa parola per relazioni diverse, poi un
controesempio che distingue. Da eseguire dopo i tre banchi storici in corso.
La prima cura resta nel circuito del giudizio sulle ipotesi: non promuovere
un conteggio a certezza, conservare i concorrenti, sospenderne l'uso globale
finché non si distinguono. La manutenzione delle conseguenze richiede invece
sostegni per ogni atto di lettura, compresi due atti nello stesso strato: non
si cura cancellando il fatto comune e rischiando di perdere l'altra fonte.
**Seconda falsificazione:** `ambiguita.p0t` prima della cura: 10 verifiche
passate, 4 fallite. «Steel is made of iron, so iron is its Geburtsort.» dopo
il contatto su Einstein lascia due ponti globali attivi. Alla domanda su paper
la risposta è **«Reading «geburtsort» as «was born in». wood pulp.»**: il
significato dichiarato e quello usato non coincidono.

**Modifica in verifica:** separati candidati e ponti usabili in `contact.p0`;
un concorrente non ritirato sospende l'uso globale, senza perdere episodi o
forme. Un controesempio può distinguerli e riattivare il superstite. La riserva
resta anche con più episodi, perché non sono prove indipendenti. `/debug`
mostra anche i candidati sospesi e chiama il conteggio `observations`.
Zero modifiche C, zero forme di lezione nuove. Aggiornata l'attesa di `banco`
che prima pretendeva una promozione ingiustificata. Autoconferma nel conteggio
e conseguenze orfane restano rossi da chiudere, non dichiarati risolti.

**Primo esito dopo la cura:** `ambiguita.p0t` **25/25**, compresi ordine
opposto, parola diversa, correzione per contatto e ritiro dell'ultimo ponte.
Baseline storica prima delle modifiche: `tecnica` 13/13, `banco` 14/14,
`generalizzazione` 19/19.

**Persistenza, prova separata su copie COMPLETE in `/tmp`:**
`persistenza.py` riproduce apprendimento → salvataggio → processo nuovo →
controesempio → salvataggio → processo nuovo. Individuata una divergenza reale:
la CLI usa `kb/learning/learned.p0` come ricaduta, il default di MCP `kb.save`
usa ancora `kb/core/session.p0`, che il boot non carica. Nel secondo caso
la lezione sparisce al primo riavvio; con la ricaduta CLI sopravvive e il
controesempio la sospende. Terzo avvio ancora in verifica. Log preliminari:
`/tmp/parrot0-l3-persistenza.txt` e `/tmp/parrot0-l3-persistenza-cli.txt`.
Prossima modifica: condividere la scelta della ricaduta fra CLI e MCP, poi
ripetere il ciclo usando **MCP senza parametro path**, come farebbe un agente.
È una correzione del trasporto, nessun secondo meccanismo cognitivo.

**Ripreso il 25 settembre, giro successivo (commit `c7bbee9a` + questo).**

- ⛔ **F.: MCP è vietato per l'addestramento**; si usano i prompt in `make chat`
  (LEARN_PROTOCOL.md). `persistenza.py` (MCP) è stato **tolto** e sostituito da
  `persistenza.sh`: copia completa del repo, tre processi `make chat`, `/save`.
  Esito: lezione, controesempio e sospensione **sopravvivono** al riavvio,
  ricaduta `kb/learning/learned.p0`. La ricaduta condivisa `brain_save_fallback_path()`
  resta nel C (serve alla CLI); il ramo `kb.save` di `mcp.c` non è più usato dai banchi.
- **`ambiguita.p0t` 35/40 → 40/40**: la risposta «competing readings» non
  scattava perché mancava `turn_plan_candidate/1` per il turno ambiguo (il
  motore interroga `turn_priority_response/2` solo per i candidati).
- **Autoconferma chiusa** (`contact_independent_here/3`): un episodio conta
  solo se l'ipotesi non esisteva ancora, o se il turno dice R anche con una
  parola piena che la KB legge come R per altra via. Altrimenti è
  `contact_use/3`, visibile in `/debug` come `used_not_confirmed`, mai
  contato. `audit-crescita.p0t` 7/9 → 8/9; `ambiguita` 40/40, `banco` 14/14,
  `tecnica` 13/13, `generalizzazione` 19/19, `composizione` 7/7. Limite
  dichiarato: un fatto già noto ripetuto col solo N non conta (manca la
  provenienza per fatto nel test di indipendenza).
- **Rosso nuovo, trovato con `persistenza.sh`**: il controesempio «Marie Curie
  was born in Warsaw, but Warsaw is not her Geburtsort.» salva anche
  `not(born_in(marie_curie, warsaw))`, un **falso**: la seconda porzione è letta
  attraverso l'ipotesi che sta smentendo. Stessa radice delle conseguenze
  orfane: una lettura fatta con un ponte non porta il ponte come dipendenza.
- **Strada per entrambi** (§14.6): la provenienza per frase esiste già
  (`fact_source/3`, `reading_fact/2`, `kb/machinery/fact-provenance.p0`).
  Manca che una lettura fatta con una parola di contatto registri **quale
  ipotesi** ha usato, e che il ritiro/controesempio sospenda come strato i fatti
  che non hanno altra fonte; e che un turno che nega N non scriva R negato.
- `LEARN_PROTOCOL.md`: L3 in testa come **primo meccanismo guida**, con canale
  (`make chat`, niente MCP), ciclo, conteggi, controlli e limiti aperti.
- `make soft-test`: **verde in 15 s** (budget 15 s), al limite: nessun margine; la causa del secondo in più del §25.5 non è trovata.

**La provenienza dell'ipotesi nei fatti letti (§14.6, fatto — stesso giorno).**
Chiusi entrambi i rossi di questa radice, con due primitive generali nel motore
e le decisioni in KB:

- **motore, `src/kb.c`** — (1) ogni fatto (e ogni negazione) di sessione porta
  il turno dell'ultimo atto e quello in cui è entrato; il builtin
  `kb_turn_act(P, Args, Pol, new|again)` dice alla KB che cosa ha scritto il
  turno. (2) **Lo strato**: `kb_view_fact_visible` (e le vie delle negazioni)
  chiede `fact_withheld(fact(P, A, B, Pol))` per i soli fatti che hanno un
  `read_support/2`; se vale, il fatto è invisibile a ogni lettore, non
  cancellato. Il C non sa che cosa sia un'ipotesi o un contatto.
- **KB, `kb/core/contact.p0`** — il contabile `contact_reading` (prima di
  `contact`) lega ai fatti del turno il sostegno `hypothesis(N, R)` quando
  vengono dalla parola di contatto (nessuna lettura indipendente di R nel turno;
  o una negazione di un R(S, O) che vale), e `direct` quando un atto
  indipendente riafferma un fatto già sostenuto da un'ipotesi. Un fatto già
  noto riaffermato con la sola parola nuova (`again` senza sostegni) non riceve
  niente: `born_in(napoleon, ajaccio)` non dipende dal ponte.
  `fact_withheld(F)` vale se nessun sostegno è in forza (`direct`, o l'ipotesi
  è ancora un ponte).
- **loader** — una riga `not(F)` si carica con `kb_assert_neg_only`: caricare
  non è correggere. Prima al boot cancellava `born_in(marie_curie, warsaw)` di
  world-facts.p0 (trovato con `persistenza.sh`). Nella KB c'è un solo
  `not(…)` (`magnetic(austenitic_stainless_steel)`) e nessun positivo gemello.

**Misure.** `audit-crescita.p0t` **19/19** (i 5 contratti nuovi, aggiunti durante
la cura e dichiarati tali, controllano anche il meccanismo: sostegno `neg`
d'ipotesi, nessun sostegno su Napoleon, sostegno `direct` su Kant);
`ambiguita` 40/40, `banco` 14/14, `tecnica` 13/13, `generalizzazione` 19/19,
`composizione` 7/7; `make soft-test` verde in 15 s (invariato).
`persistenza.sh` (solo `make chat`): nel terzo processo Napoleon → «I don't know
about geburtsort», Kant → nessuna risposta (sospeso), Marie Curie → «warsaw».

**Residui dichiarati.** Le viste congelate che contengono un fatto sospeso non
si rifanno da sole (nessuna oggi misurata). Il loader dei positivi toglie
ancora una negazione della stessa origine (asimmetria, nessun caso nella KB).
Sospeso, Kant risponde «I don't understand that yet.»: la risposta non dice
*perché* non lo sa più (§19.3: «Perché non lo sai più?» dovrebbe avere una
risposta). La risposta «Learned: marie Curie was born in Warsaw, but Warsaw is
not her Geburtsort.» del turno di controesempio è una resa sbagliata del lettore
L2, non toccata.

**«Perché non lo sai più?» (§19.3, fatto — stesso giorno).** Tutto in KB
(`kb/core/contact.p0`, frasi per lingua in `contact_why_phrase/3`), nessun C:

- una **domanda** che nomina soggetto e relazione di un fatto sospeso
  (`turn_contact_withheld/3`) riceve il perché: la proposizione, la parola
  attraverso cui era stata letta, la lettura caduta, la ragione
  (`contact_why_reason/4`: controesempio con la sua coppia, ritiro, letture
  concorrenti) e la maniglia vera («tell me in other words»: un atto diretto è
  un sostegno `direct`);
- una domanda che usa la **parola** caduta (`turn_contact_dropped/3`) dice quale
  lettura aveva, perché è caduta e che la relazione resta chiedibile con le sue
  parole. Prima diceva «I don't know about geburtsort», come se la parola non
  fosse mai stata incontrata. La prima stesura prometteva «show me an example
  and I can learn it again»: **falso** (un controesempio sospende qualunque sia
  il numero dei sostegni, §16.2), provato in chat e tolto.

Misure: `audit-crescita` **35/35**, `banco` 16/16 (rivalidata p6: il muro «I
don't know about birthplace.» è diventato il perché, il valore resta assente),
`ambiguita` 40, `tecnica` 13, `generalizzazione` 19, `composizione` 7;
`make soft-test` verde in 14 s; `persistenza.sh`: la spiegazione si ricostruisce
nel processo nuovo dai sostegni e dal controesempio salvati.
**Aperto:** la frase italiana non è verificata («Dove è nato Kant?» si ferma
sulla traduzione di «nato», lacuna del lettore italiano); il «perché» di un
ritiro dice «That reading was withdrawn» senza chi o quando, perché oggi il
ritiro (`contact_bridge_withdrawn/2`) non porta una ragione.

## ⏸ HANDOFF precedente — 25 settembre 2026, notte

**⛔ Primo compito di domani: il soft-test è ROSSO.** Con l'ultimo commit (la
famiglia delle relazioni nella cipolla, §25.5) `make soft-test` misura **16, 16,
17, 19 s** contro **15 s** della base `8991707e`, in misure alternate sulla
stessa macchina. La causa **non è trovata**:
- il boot è pari (~6,0 s);
- i 9 turni di `knowledge/facts.p0t` costano uguale uno per uno;
- `facts.p0t` cronometrato da solo tre volte alternato è pari (lavoro 12,0 /
  11,5 / 11,5 s, base 11,8 / 12,0 / 11,9 s).

Quindi il secondo in più sta in qualcosa che `make soft-test` fa e il file da
solo no. Da guardare: `health.p0t` e `conversation/basics.p0t` con lo stesso
metodo (turno per turno, `!timeout 0.01` dopo ogni `[test]` fa stampare la
durata); la catena di `!reset` fra i file; la cipolla consultata a ogni turno da
`p0_composed_say` (`induction_question`, `negative_because`, 10-memory-knowledge.c
~2893), che ora attraversa tesi con clausole in più (`turn_goal_head/2`,
`turn_goal_unanchored/1` per le relazioni). Se il costo non si trova in fretta,
la regola del progetto dice di togliere, non di alzare il budget: il ripiego è
tornare a `8991707e` per il solo modulo delle relazioni.

**Dove siamo, in una riga per filo.**

| filo | stato | dove |
|---|---|---|
| L3, contatto | il meccanismo impara per contatto e **scrive lingua**: `relation_noun/2` per un nome, cornice + `answer_frame/2` per un verbo; nessun lettore sa del contatto | §21, §24 |
| L3, ritiro | per contatto (una correzione con negazione aggiunge un controesempio), ed è uno strato | §19, §22.1 |
| L3, generalizzazione | come tecnica: lessico straniero, domanda polare, prosa senza maestro; banchi `tecnica` 13/13, `banco` 14/14, `generalizzazione` 19/19 | §22, §23 |
| cortocircuiti | vista ibrida (spegne la sola clausola colpevole), ciclo logico puro al punto fisso, registro unico `paradox_event/4` | §25.1–§25.3 |
| composizione | prende la parola per le domande di classe (§25.4) e di relazione (§25.5, **commit con soft-test rosso**) | §25.4, §25.5 |

**Commit della giornata** (tutti su `origin/main`): `5791da90` I1 · `8288f6d7`
I2 · `cec3d1fa` handoff · `1dc443b7` ritiro e generalizzazione · `77a69b60`
tecnica · `81be5955` il contatto scrive lingua · `80b06ad9` vista ibrida ·
`65a9f575` ciclo puro · `4968127c` registro unico · `8991707e` composizione ·
l'ultimo, relazioni + questo handoff.

**Principi che valgono per chi riprende** (aggiunti oggi, in ordine):

1. **Ciò che il contatto conclude è lingua, non una nota sulla lingua (§24, F.).**
   Se per usare una cosa appresa bisogna toccare un consumatore, è di nuovo L2+.
2. **Dimenticare è uno strato (§19, F.).** Mai `retract` del sapere.
3. **Un cortocircuito si conosce, non si tace (§25, F.).** Consapevolezza, rimedio
   senza halt, un solo registro, una voce.
4. **Un verde su ciò che la KB sapeva già non misura niente (§21.5).** La KB viva
   collega quasi ogni nome di relazione naturale: si ritira in memoria il ponte
   specifico, dichiarandolo in testa al banco.
5. **Una guardia di assenza (`<!`) che smette di poter fallire va rivalidata**
   (§25.5): cambiare la frase di un muro rende verdi per costruzione i test che
   ne vietavano la vecchia forma.

**Prossimi passi, dopo il soft-test.**

1. **Il lettore polare delle relazioni** (§25.5): consegna il lemma della domanda
   invece del predicato («eat» per `eats`, «border» per `borders`) e l'oggetto
   con la preposizione («in pisa»). È la causa della resa brutta del nucleo e
   della **strada rotta** «does france border spain?» → «I don't know» con
   `borders(france, spain)` in KB.
2. **L3:** residuo di più parole (con R3, «boiling point»); soglia e condizione di
   osservazione come conoscenza che si può correggere parlando (§21.7, §22.3);
   una verifica attiva prima di usare un'ipotesi (§22.3).
3. **Composizione:** i membri del ciclo sono solo i predicati tagliati, non
   l'anello (§25.4); C3, togliere `no_support_either_way` (oggi ripiego).
4. **Ciclo fra viste:** oggi solo pubblicato; il rimedio (punto fisso comune)
   quando ne esisterà uno da misurare (§25.2).

**Come riprodurre in un minuto.**

```sh
make build && make test-engine
./bin/parrot0 --test docs/labs/l3/I2/tecnica.p0t          # 13/13: contatto → lingua
./bin/parrot0 --test docs/labs/l3/I2/composizione.p0t     # 7/7: la cipolla parla
make soft-test                                             # oggi ROSSO: 16-19 s
# il contatto dal vivo, con il registro dei paradossi e le ipotesi:
printf '%s\n' 'Einstein was born in Ulm, so his Geburtsort is Ulm.' \
  'What is the Geburtsort of Napoleon?' '/debug' '/quit' \
  | PARROT0_SESSION= PARROT0_LANG=en PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0
# sonde: debug_contact (43), debug_paradox (44)
```

**Rossi preesistenti, controllati contro la base** (non inseguirli come
regressioni): `engine/anon.p0t`, `reasoning/inference_guard.p0t` (solo tempi),
`crossing/mix_function_from_reading.p0t` riga «heat pump», `expert/grammar.p0t`,
`reasoning/conj.p0t`, `reasoning/taught_rules.p0t`, `reasoning/hypothesis.p0t`,
`meta/retract.p0t`.

**Trappole pagate oggi:**

- `naf` con variabili libere fallisce sempre: si passa da un ausiliario ground
  (`contact_contraction_part/2`);
- una lista di ~400 atomi nella sostituzione esaurisce i legami e la composizione
  «non trova niente» con i pezzi verdi uno a uno: si enumera, non si raccoglie;
- un argomento legato non combacia con un fatto fra virgolette
  (`function_word("isn't", …)`): si enumera e si confronta il testo;
- una regola che legge una vista mentre la definisce blocca il boot: oggi lo
  dice `paradox_event(view, …)`, prima non lo diceva niente;
- `eq/2` è numerico, non unifica atomi;
- il motore azzera `current_turn` a inizio turno solo per una lista C: un fatto
  di turno nuovo va legato a `turn_counter/1`.

## ⏸ HANDOFF precedente — 25 settembre 2026, pomeriggio

**Dove siamo.** Il primo circuito L3 funziona (§21.6, `8288f6d7`): dal contatto
«Einstein was born in Ulm, so his birthplace is Ulm.» nasce un'ipotesi inerte
(«birthplace» potrebbe nominare `born_in`), usata qualificata con un sostegno e
piena con due, ritirata come strato (§19). Banco `docs/labs/l3/I2/banco.p0t`
14/14 con setup dichiarato (§21.5). I1 non è più un cancello (§21): R2 e R5
chiusi (§20), R3 diagnosticato. **Prossimo passo, in ordine (§21.7):** ritiro
per contatto, residuo di più parole (con R3), soglia e condizione di
osservazione come conoscenza. Il residuo metalinguistico da non dimenticare è
elencato nel §21.6.

## ⏸ HANDOFF precedente — 24 settembre 2026, sera

**Dove siamo.** L3 non è implementato. Sono chiusi **I0** (la baseline, §17) e il
**gradino prima di I1** (§17.5): i contatti del §15 ora arrivano alla IR senza
essere letti in modo sbagliato e senza scrivere niente di falso. Il prossimo
incremento è **I1 — osservazione** (§16.1).

**Commit della giornata** (tutti su `origin/main`):

| commit | che cosa |
|---|---|
| `2a8ac69e` | il piano: principio adatto-linguistico, finto spostamento, G0–G3, §1-bis |
| `783f59dd` | aggiornamenti di F.: ipotesi operativa (§12), audit (§13), contratto (§14), esperimento (§15), incrementi I0–I5 (§16) |
| `c7474e77` | **I0**: baseline misurata, `docs/labs/l3/I0/` (transcript e trace), §17 |
| `08d121d7` | **gradino prima di I1**: tolti i furti T1–T3 con guardie conservative, trace ampliato, §17.5 e §18 |

**Principi che valgono per chi riprende** (in ordine di importanza):

1. **L2 e L3 convivono (§18).** Niente di ciò che L2 fa oggi si butta via; le
   cure lungo la strada sono guardie conservative, verificate contro la base.
2. **Il primo passo falso (§1-bis).** Non costruire L3 come un lettore di schemi
   più naturali: niente `turn_form` o lettore dedicato per l'apposizione, il
   «so», «I mean». Prima si esplorano le possibilità evolutive; il ripiego si
   registra come L2+.
3. **Non lasciare KB muta (§2).** Per ogni regola di macchinario nuova, la prova
   del §2.4 nella scheda: motore o decisione? Se è una decisione, la sua
   maniglia parlata.
4. **Un verde su ciò che la KB sapeva già non misura contatto (§15.1).** LED, il
   ponte «boiling point», «freezing point» e la contrazione sono già salvati: si
   usa il setup di ritiro in memoria e la relazione di controllo `born_in` ↔
   «birthplace» (§17.2).

**Prossimo passo: I1 — osservazione.** Il contatto arriva, ma la IR non ne
conserva ancora i pezzi su cui un'ipotesi può attaccarsi. In ordine:

| # | che cosa manca | che cosa si vede oggi nel trace | dove guardare |
|---|---|---|---|
| R1 | un **nodo che leghi le due proposizioni** attorno a «so»/«;» | `evidence(span(7,7), discourse, consequence)` c'è; nessun legame fra le proposizioni; il lettore composto le rilegge come turni separati | `compound_turn_lead` e `turn_publish_*` in `99-registry.c`; `kb/core/input-structure.p0` |
| R2 | «100 degrees Celsius» come **quantità** | entità `water_boils`, `degrees_celsius`; il numero sparisce. `measured_value/1` vuole numero+unità di due parole, e «degree Celsius» non è in `measures/2` | `kb/core/decisions.p0` (`measured_value`, `unit_word_kb`); `measured_value_max_words` in grammar.p0 |
| R3 | «its boiling point» come **sintagma** | candidato `its boiling`; nessun frame legge «its R is V» | lettore dei sintagmi, `np_closer/1` |
| R5 | la **coreferenza** di «its» | `refer «its» -> celsius (most_recent)` invece di `acetone` | `coref_resolve` (99-registry.c), `p0_antecedent_receipt` (10-memory-knowledge.c); l2-upgrade.md punto 6 |

Condizione di uscita di I1 (§16.1): rileggere non riscrive il passato; due
occorrenze della stessa parola e due candidati restano distinguibili; e, per il
§15, le due porzioni del contatto sono nella IR con i loro nodi, la quantità e il
referente di «its» (o la sua ambiguità dichiarata). **I1 non impara niente**: se a
questo gradino qualcosa «impara», è sospetto.

Aperto a margine, non bloccante per I1: **R6**, la proposta «Want me to learn
about X?» che prende anche il turno seguente (§17.5).

**Come riprodurre in un minuto.**

```sh
make build
S=docs/labs/l3/I0
# i contatti del §15 e del controllo, sessione pulita, KB agi completa
printf '%s\n' 'forget that the boiling point of x is y means x boils at y' \
  '/debug' 'Water boils at 100 degrees Celsius, so its boiling point is 100 degrees Celsius.' \
  '/debug' '/quit' | PARROT0_SESSION= PARROT0_LANG=en PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0
# sonde utili nel /debug: debug_grammatical_cue, debug_np_candidate,
# debug_turn_entity, debug_ir_node; filtri: /debug trace refer | read.compound | symbolic
```

I file d'ingresso delle sonde sono in `docs/labs/l3/I0/*.txt`; gli esiti di
riferimento in `*-dialogo.txt`.

**Verifiche da rifare a ogni passo:**

- banchi puntuali delle parti toccate, **confrontati con la base in un worktree
  separato** (`git worktree add <dir> <commit>`, `make build` dentro): per il
  gradino chiuso erano `conversation/compound_inquiry`, `language/coref*`,
  `language/compose_coref`, `reasoning/symbolic.it`, `mcp/compound`, e i loro
  rossi sono **preesistenti** (29/31, 6/8, 3/6, 2/3 contro 28/31, 6/8, 3/6, 2/3
  della base);
- `make soft-test`: nella sera del 24 era **fuori budget, 19–20 s con i test
  verdi**, sotto carico. Per attribuire un costo si usa il **profilo per turno**
  di `/debug` (tempo e numero di query per turno, base contro modifica); al
  gradino chiuso le query erano alla pari (42 532 contro 42 527). **Da rimisurare
  a macchina scarica** prima di concludere che il bordo sia solo il carico;
- la suite intera no: va approvata da F.

**Trappole pagate oggi:**

- il **rilancio della coreferenza** scorreva tutto il registro senza cessioni: se
  una condotta dichiarata in KB «non ha effetto», guardare se il turno è stato
  rilanciato (`faculty … (coref retry …)` nel trace);
- i **turni annidati** del lettore composto sovrascrivono la forza del turno
  esterno: ora viene restituita (`read.compound outer force restored`), ma ogni
  nuovo punto che rilancia un turno deve fare lo stesso;
- una **guardia dentro una regola enumerata a ogni turno** costa (lotto del 24:
  +270 ms per turno dentro `phrase_canon/2`): chiederla al momento dell'uso.

---

## Handoff vivo — storia dei checkpoint

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

**Checkpoint 6 — gradino prima di I1 chiuso (§17.5).** I tre furti sono curati
con guardie conservative (§18: L2 convive), e il contatto arriva alla IR. Nuovi
aperti: R5 «its» legato all'entità sbagliata, R6 proposta che prende il turno
dopo. **Prossimo passo: I1**, osservazione (R1 nodi fra le proposizioni, R2
quantità, R3 «its boiling point», R5 coreferenza).

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

### 17.5 Il gradino prima di I1, fatto: il contatto arriva al lettore

Curati i tre furti, con guardie conservative (§18) e senza toccare ciò che L2 fa:

| furto | cura | verifica |
|---|---|---|
| **T1** ramo gen241 | vale solo se la forza **pubblicata** del turno è una domanda (`p0_turn_is(b, "question", …)`, come i rami vicini); riga di trace `read.gen241` (**D1**) | il contatto sull'acqua non riceve più la risposta memorizzata; «At what temperature does water boil?» risponde come prima |
| **T2** `year_stated` | classe dello slot `year_said/1` in KB: l'oggetto dev'essere un numero | «Einstein was born in Ulm, so his birthplace is Ulm.» → vince il frame `born_in` (fatto vero); «zelnik was built in 1990» identico alla base |
| **T3** punto e virgola → codice | tre difetti, trovati in fila col trace: (a) `faculty_yield_force(symbolic, open, compound_statement)` in KB; (b) il lettore composto **restituisce al turno esterno la sua forza** dopo le clausole annidate, che la sovrascrivevano; (c) il **rilancio della coreferenza** (`coref_resolve`) scorreva tutto il registro senza nessuna cessione: ora rispetta `p0_faculty_yields` e il trace dice chi cede e chi risponde nel rilancio | il contatto sull'acetone arriva come osservazione non capita; il codice vero resta riconosciuto |

Trace nuovo: `read.gen241`, `symbolic looks_code claims …`, `read.compound outer
force restored`, `faculty … (coref retry …)`.

Banchi puntuali, lavoro contro base `c7474e77`: `compound_inquiry` 29/31 contro
28/31 (uno in più), `coref` 6/8 = , `coref_possessive` 2/2 = , `coref_resolve`
2/2 = , `compose_coref` 3/6 = , `symbolic.it` 2/3 = , `mcp/compound` 5/5 = (i
rossi sono preesistenti). Profilo per turno dei 16 turni del soft-test alla
pari (42 532 contro 42 527 query; 8,5–9,1 s contro 8,8 s). `make soft-test`:
test verdi, **19–20 s sotto carico** — sopra il budget come la base in queste ore;
da rimisurare a macchina scarica.

**Reperti nuovi, lasciati aperti:**

- **R5 — coreferenza sbagliata**: nel contatto sull'acetone «its» viene legato a
  `celsius` (`refer «its» -> celsius (most_recent)`), non ad `acetone`. È il
  punto 6 di l2-upgrade.md («its contamina»), e per L3 è centrale: il ponte si
  aggancia a *di chi* è il punto di ebollizione.
- **R6 — la proposta aperta prende il turno dopo**: dopo «I don't know about
  acetone yet. Want me to learn about it?», il turno seguente (anche codice) si
  apre con «Looking up acetone…». Meccanica preesistente delle proposte, resa
  visibile ora che il contatto produce un'offerta invece di un falso «codice».

**Stato:** i contatti del §15 ora arrivano alla IR **senza** essere letti in modo
sbagliato e senza scrivere niente di falso. È il punto di partenza richiesto da
I1 (osservazione): R1–R3 e R5 sono il suo lavoro.

## 18. L2 e L3 convivono (F., 24 settembre 2026)

> *«Vorrei che tu considerassi l'esistente, che è basato su L2, come una
> capacità di basso livello che potrà continuare a vivere. Concentriamoci sul
> far nascere L3 affianco a L2. Non voglio che dobbiamo buttare via quello che
> oggi L2 dimostra di saper fare: facciamo convivere.»*

**Che cosa vuol dire, operativamente.**

1. **Gli schemi di L1/L2 restano** come riflessi rapidi e provati: le forme di
   lezione, le ricevute di lettura, le correzioni con portata dichiarata. Nessun
   incremento di L3 li rimuove o li riscrive per farli sembrare contatto.
2. **L3 si appoggia su L2.** Le ricevute (`reading_choice/4`), la rilettura e la
   portata dichiarata sono ciò che dà a un'ipotesi di contatto un *questo* a cui
   attaccarsi (§6-bis, §14.1). Senza L2 il contatto sarebbe indovinare.
3. **L3 lavora dove L2 lascia residui**: letture non capite, conflitti fra
   lettori, decisioni che un contatto contraddice. Col tempo può *rivedere* una
   decisione di L2, sempre nella stessa mente, con provenienza e ritiro, senza
   smontare lo schema che l'aveva presa.
4. **Le cure lungo la strada sono guardie conservative.** Quando uno schema di L2
   ruba un contatto (§17, T2), gli si toglie il furto e basta: la forma
   `year_stated` continua a leggere «zelnik was built in 1990» esattamente come
   prima, verificato contro la base.
5. **Il §1-bis resta in vigore.** Convivere non significa che L3 si costruisca
   *come* altri schemi di L2: L3 è un circuito diverso che usa L2 come strato
   basso. Una forma nuova per uno strumento del contatto resta L2+, anche se
   convive benissimo con il resto.

**Il parere dell'agente.** È la scelta giusta anche tecnicamente, non solo per
prudenza: il contratto del §14 presuppone già le ricevute di L2, e il §17 mostra
che i furti che oggi impediscono il contatto stanno quasi tutti *fuori* da L2
(un ramo compilato, un rilevatore, un rilancio senza cessioni). L2 non è
l'ostacolo; è il terreno.


## 19. Dimenticare è uno strato, non una cancellazione (F., 25 settembre 2026)

> *«In L3 il forget è una stratificazione sopra la conoscenza precedente, e
> anche questo semplifica il set di primitive, di cui una era la retract. In L3
> per contatto si apprende che una cosa va dimenticata: vuol dire che sopra lo
> strato che la conosce si sovrappone quello che indica di ignorarla, o di
> considerarla conosciuta. Quando a un umano diciamo "dimenticati questa cosa
> perché…", non la rimuove dalla mente: aggiunge, assieme alla vecchia
> conoscenza ritirata, la conoscenza che la rende ritirata.»*

**Che cosa cambia.**

1. **Il ritiro è un'aggiunta.** «Forget that …», una correzione, un contatto che
   contraddice: ognuno *aggiunge* un fatto sopra la conoscenza toccata («questa
   è ritirata, da questo turno, per questa ragione»). Il contenuto vecchio resta
   leggibile con la sua storia; ciò che cambia è quale strato *vale* nell'uso.
2. **Una primitiva in meno.** Il circuito L3 non ha bisogno di `retract` per
   imparare a dimenticare: gli basta asserire, più una regola di precedenza fra
   strati (lo strato che ritira batte quello che afferma, finché un terzo non
   ritira il ritiro). `retract` resta al motore per la pulizia meccanica
   (scratch di turno, cache), non per il sapere.
3. **Dimenticare è conoscenza, quindi si può chiedere e disfare.** «Perché non
   lo sai più?» ha una risposta (il fatto che ritira, con la sua ragione); «ricordati
   di nuovo …» è un altro strato, non una ricostruzione. È anche la forma giusta
   per il §14.6: togliere una fonte non cancella l'altra, perché nessuna delle
   due è stata cancellata.
4. **«Considerarla conosciuta»** è lo stesso gesto nell'altro verso: uno strato
   che dice che una cosa è già posseduta, senza riscriverla.

**Dove c'è già, e dove no.** La lezione di costruzione ritirata del setup §15.1
già funziona così («is no longer active; I kept its lesson trace»). Il ritiro
dei fatti letti di L2 (`reading_stale_clause`) invece cancella. Da censire in I3/I4
(§16.1): ogni sito che oggi ritira *sapere* con `kb_retract` è un candidato a
diventare uno strato. Il banco del §16.2 («ritiro di H con due fonti di C»,
«salva → processo nuovo → ritiro → processo nuovo») si legge con questa regola:
il ritiro sopravvive al riavvio perché è un fatto salvato, non un'assenza.

## 20. I1 — primo passo: la quantità e il referente di «its» (25 settembre 2026)

**Stato: R2 e R5 chiusi; R3 diagnosticato e lasciato aperto; R1 da fare.**

### 20.1 R2 — «56 degrees Celsius» è una quantità

`input_quantity_node(Scope, Id, quantity(Valore, Unità), Ultimo)` in
`input-structure.p0`: un numero seguito, su token contigui, dalle parole di
un'unità di `measures/2`, ciascuna nuda o al plurale («degrees Celsius»). I
token della quantità non sono più nomi nudi (`bare_token`, il nome isolato), e
`debug_quantity` (sonda 39) la mostra. Prima della regola la IR vedeva le entità
`degrees_celsius` e `celsius` e il numero spariva; ora vede
`quantity(56, degree_celsius)`.

- **Motore o decisione (§2.4)?** Quali parole siano unità è una **decisione** e
  ha già la sua maniglia parlata: «The degree Celsius measures temperature.»
  (verificato a runtime, poi salvato con `/save`: il save-map l'ha instradato in
  `kb/facts/units.p0` con la provenienza, insieme a «degree Fahrenheit»).
  Riconoscere numero+unità è **motore**.
- **Trovato strada facendo:** `input_unit_node/4` chiedeva `measurement_unit/1`,
  che non esiste in nessun file: il frame `measure` della IR era **KB muta** (§2).
  Non l'ho toccato; è il consumer naturale della quantità in I2.

### 20.2 R5 — «its» si lega ad `acetone`

Due difetti, trovati col trace (`refer mentioned X (seq N)` è una riga nuova):

1. la **maiuscola dell'unità**: «Celsius» entrava nella storia del discorso come
   un nome. Ora il motore chiede `not_a_referent_here/1` (discourse.p0) prima di
   registrare una menzione; la clausola di oggi dice che un token di quantità
   non riferisce. Una ragione nuova è una clausola in più;
2. un **campo C che non si azzera mai**: `last_entity` veniva ri-registrato come
   menzione più recente a fine di *ogni* turno, anche annidato, anche se nessun
   modulo l'aveva risolto in quel turno. Dopo «The degree Celsius measures
   temperature.», «its» del turno seguente andava a `degree_celsius`. Ora si
   registra solo se è cambiato durante il turno (`brain_respond_dispatch`).

Esito, processo nuovo, KB viva (ritiro in memoria del ponte, §15.1):

| contatto | quantità | «its»/«his» | risposta |
|---|---|---|---|
| Acetone … ; its boiling point is 56 degrees Celsius. | `quantity(56, degree_celsius)` | `acetone` | «I don't know about acetone yet…» (come prima) |
| Water … , so its boiling point is 100 degrees Celsius. | `quantity(100, degree_celsius)` | `water` | «I don't know about degrees yet…» (**identica alla base**, R7) |
| Einstein was born in Ulm, so his birthplace is Ulm. | — | — | «Learned: einstein was born in ulm.» (come §17.5) |

La scelta di «its» è ancora per **recenza** con un solo candidato rimasto, non
un sostegno: basta al criterio di I1 (referente o ambiguità dichiarata), non a I2.

### 20.3 R3 — «its boiling point», diagnosi

Il sintagma si chiude su «point» perché `relation_verb(point)` («point at»,
verbs.p0) lo rende `np_closer`. È un'**ambiguità lessicale** (nome/verbo) che il
produttore dei sintagmi (`src/code.c`, una fermata per parola, senza contesto)
risolve sempre come verbo. Le cure brevi sono entrambe sbagliate: una guardia
sulla forza del turno non funziona (la IR si costruisce prima che la forza sia
pubblicata), e «un verbo non è seguito da una copula» è una regola di superficie
che nasconde il problema. La forma coerente con I1 è quella del criterio di
uscita: **due candidati restano distinguibili** («its boiling» chiuso dal verbo,
«its boiling point» chiuso dalla copula) e sarà il contatto — la costruzione
nominale che L2 già conosce — a sceglierli. Tocca tutti i consumatori di
`np_candidate` (vince lo span massimo): va fatto come passo a sé, misurato.

### 20.4 Reperti nuovi

- **R7** — nel contatto sull'acqua l'offerta nomina «degrees» come parola opaca:
  la prima parola sconosciuta del turno (`99-registry.c`, `sw` prima di
  `fallback_gap_offer`) non consulta la quantità. Preesistente.
- **KB muta**: `measurement_unit/1` (sopra).

### 20.5 Prossimo passo

R1 (il nodo che lega le due proposizioni attorno a «so»/«;»), poi R3 come
candidati alternativi. Con R1–R3 la IR avrà le due porzioni, la quantità e il
possessore: è il materiale su cui I2 prova ad allineare
`boils_at(acetone, 56 °C)` con «boiling point of acetone = 56 °C».

## 21. I2 parte dal controllo: `born_in` ↔ «birthplace» (F., 25 settembre 2026)

> *F.: «vorrei capire perché non stiamo iniziando con le parti salienti di L3,
> mi sembra che siamo ancora a fixare bug su L2».*

Aveva ragione. L'ordine I0 → I1 → I2 del §16.1 era diventato un **cancello**:
ogni residuo della IR sembrava un prerequisito, e ogni prerequisito ne scopriva
un altro. È la deriva del §1-bis in un'altra forma: non schemi più naturali,
ma L2 lucidato all'infinito. **Correzione d'ordine:** I1 non è più un cancello;
si fa il pezzo di osservazione che un caso di I2 chiede, quando lo chiede.

### 21.1 Perché il controllo può partire subito

«Einstein was born in Ulm, so his birthplace is Ulm.» ha già tutto ciò che
serve (misurato il 25 settembre, dopo `5791da90`):

- la prima porzione è letta dal frame tipato: `born_in(einstein, ulm)`;
- la seconda, «so his birthplace is ulm.», **resta un residuo non letto** (il
  trace la mostra passata alle forme, nessuna la prende);
- «his» ha un antecedente unico (`einstein`), «birthplace» è una parola sola
  (niente R3), il valore è un'entità (niente R2);
- la KB ha fatti veri indipendenti per il trasferimento (Curie → Warsaw,
  Napoleon → Ajaccio, Galileo → Pisa, Columbus → Genoa);
- baseline: «What is the birthplace of Marie Curie?» → «I don't know about
  birthplace.» (onesto).

### 21.2 La proposta: allineamento del residuo, nessuna forma

Il meccanismo non guarda il connettivo né la forma della seconda porzione.
Guarda **che cosa resta non spiegato** dopo la lettura:

1. la lettura del turno ha legato una relazione `R(S, O)` a certi token (il
   soggetto, l'oggetto, le parole del frame);
2. nel resto del turno ricompaiono **entrambi i ruoli** (per nome o tramite un
   riferimento risolto a quel ruolo), e tolte le parole funzionali che la KB
   già classifica resta **una sola parola piena non spiegata**, N;
3. allora nasce un'ipotesi **inerte**: *N potrebbe nominare R*, con i ruoli
   nell'ordine osservato (possessore → S, valore → O), e un sostegno: l'episodio.
   L'alternativa *coincidenza* resta implicita finché il sostegno è uno solo.

Il «so», il «;», l'ordine delle porzioni non compaiono da nessuna parte:
cambiarli non deve cambiare la proposta (§15.3). E nemmeno «his»: un riferimento
risolto a S vale quanto il nome.

### 21.3 Uso rivedibile

- **Un solo episodio → lettura qualificata.** Alla domanda «What is the
  birthplace of Napoleon?» parrot0 non dice «I don't know» e non afferma: dice
  che cosa ha supposto e da dove («If by birthplace you mean where he was born
  — as in what you told me about Einstein — Ajaccio.»). Il testo è un template.
- **Due episodi indipendenti** (soggetti e valori diversi, stessa N e stessa R)
  → la lettura si usa senza riserva, sempre con provenienza.
- **Un contrasto** (N con S e un valore diverso da R(S, ·)) → l'ipotesi cade
  per quel sostegno: il ritiro è uno strato (§19), non una cancellazione.
- **Ritiro** («forget that…» o una correzione ordinaria) → cade la via appresa,
  restano i fatti `born_in`.

### 21.4 Banco, fissato prima della cura

| passo | ingresso | atteso |
|---|---|---|
| 0 | «What is the birthplace of Marie Curie?» | «I don't know about birthplace.» (base) |
| 1 | «Einstein was born in Ulm, so his birthplace is Ulm.» | legge `born_in`; nasce l'ipotesi, visibile in `/debug` |
| 2 | «What is the birthplace of Napoleon?» | Ajaccio, **qualificata** |
| 3 | «Marie Curie was born in Warsaw; Warsaw is her birthplace.» (ordine inverso, altro connettivo) | secondo sostegno |
| 4 | «What is the birthplace of Galileo Galilei?» | Pisa, senza riserva |
| 5 | contrasto: «Columbus was born in Genoa, and he loved Ulm.» | nessun sostegno per «loved» ↔ `born_in` (O diverso) |
| 6 | ritiro dell'ipotesi, poi passo 4 | torna «I don't know about birthplace.»; `born_in(galileo_galilei, pisa)` resta |

Falsificatori del §16.2 da tenere accesi: stesso episodio ripetuto non conta come
secondo sostegno; la risposta al passo 2 non conferma l'ipotesi (auto-conferma).

### 21.5 Il controllo era contaminato (trovato il 25 settembre)

Il censimento del §17.2 diceva «il nome non è mai stato collegato». Falso: la
KB viva ha `construction_frame("@O is the birthplace of @S", "@S was born in @O",
born_in)` (constructions.p0:586) e `extract_frame("@O is the birthplace of @S",
born_in)` (grammar.p0:2782). «What is the birthplace of X?» risponde «I don't
know» per un'altra ragione: il ramo «the R of X» di `mod_knowledge`
(10-memory-knowledge.c, `idk` dopo `holds/3`) risponde prima che
`p0_try_frame_question` possa leggere la costruzione, per i nomi di una parola.
È una **strada rotta** (R8), non una lacuna: non la riparo qui, perché riparata
farebbe rispondere «birthplace» senza contatto e il banco non misurerebbe niente.
Anche `inventor` (`answer_frame(inventor, invented_by)`) e `habitat` (il
predicato stesso) sono già collegati: la KB viva collega quasi ogni nome di
relazione naturale. Il banco usa quindi il setup del §15.1: ritiro **in memoria**
dei soli due ponti, dichiarato in testa al file.

### 21.6 Fatto: il primo circuito di contatto (I2 + I3 parziale)

**Che cosa c'è** (`kb/core/contact.p0`, nuovo; due porte C generiche):

- **porta C 1, `after_reply_bookkeeper/1` + `turn_after_reply/2`**
  (`turn_done`, livello esterno): il gemello post-risposta di `bookkeeper/1`,
  che corre prima del dispatch e non vede ciò che le facoltà leggono. La porta
  non sa niente di contatti;
- **porta C 2, la struttura del turno esterno**: il lettore composto ora
  restituisce la IR del turno intero dopo le clausole, come già la forza.
  `scope_copy`/`scope_clear` sono stati estratti da `session_archive_turn` (che
  ora li usa); quali predicati siano «struttura» lo dice `outer_turn_structure/2`;
- **l'osservazione** (`contact_episode_here/4`): c'è un riferimento, una parola
  piena ripetuta, un ruolo che finisce con quella parola e l'altro fra le
  entità della IR, una relazione leggibile di cui la KB tiene il **fatto**
  R(S, O), e tolte parole funzionali, ruoli, riferimenti e le parole con cui la
  KB già legge R resta **una parola sola**. Allora si scrive
  `contact_episode(N, R, ep(S, O, Possessore))`;
- **l'uso**: `holds/3` (già interrogato dal ramo «the R of X») ha una clausola
  per le ipotesi non ritirate; con un solo sostegno la risposta porta la
  premessa di ogni lettura per ipotesi, «Reading «birthplace» as «was born
  in».» (`translation_preface`, riusata); con due, senza riserva;
- **il ritiro è uno strato** (§19): `contact_bridge_withdrawn/2` si aggiunge,
  l'episodio resta;
- **sonda** `debug_contact` (43).

**Banco** `docs/labs/l3/I2/banco.p0t`: **14/14** (esito in `esito.txt`).
Sulla KB viva senza setup il contatto non crea nessuna ipotesi: il ponte c'è già,
«birthplace» è una parola con cui la KB legge `born_in`, quindi è spiegata.

**Prova del §1-bis.** Il diff non aggiunge `turn_form` né lettori per uno
strumento del contatto; «so» e «;» danno lo stesso episodio; l'ordine nella
seconda porzione («his birthplace is Ulm» / «Warsaw is her birthplace») non
conta. È **L3**, con il residuo qui sotto.

**Residuo metalinguistico, elencato (non taciuto):**

1. la **condizione di osservazione** (un ruolo ripetuto, l'altro ripreso, un
   residuo di una parola) è scritta dall'ingegnere in `contact.p0`: nessuno può
   ancora dire a parrot0, parlando, che un'apposizione o una parafrasi valgono
   come contatto. È la politica di accettazione del §16.3, ancora G1;
2. la **soglia** (uno = qualificata, due = piena) è un letterale nelle regole;
3. il residuo deve essere **una parola**: «boiling point» (R3) non entra, quindi
   l'acetone del §15 aspetta i candidati alternativi;
4. l'ancora è un **fatto** della relazione (`kb_fact/2`), non una prova: le
   relazioni derivate non fanno da ancora (limite scelto per il costo, sotto);
5. il **ritiro** non ha ancora una maniglia parlata ordinaria: oggi è un fatto
   del banco. Il passo L3 è il ritiro per contatto (una correzione ordinaria che
   contraddice l'ipotesi), non una forma «forget that…».

**Costo, misurato contro la base `5791da90`, stesso setup:** primo turno dopo il
setup 8,2 s contro 7,3 s (la vista `contact_readable_relation` si ricostruisce
dopo il `!forget` di un frame); contatto 2,66 s contro 2,45 s; composto 3,79 s
contro 3,44 s. Le prime versioni costavano 45 s, poi 1 s, poi 5 s: il doppio
ciclo su `input_entity_node`, la lista di ~400 relazioni nella sostituzione (che
esauriva i legami e faceva «non trovare niente» alla composizione, con i pezzi
verdi uno a uno), e `apply/2` su relazioni definite da regole. **Debito
preesistente, non mio:** i turni di base su questa KB costano 2,4–7,3 s.

### 21.7 Prossimo passo

In ordine: (a) il **ritiro per contatto**: una correzione ordinaria che
contraddice un'ipotesi aggiunge lo strato che la ritira; (b) il residuo di **più
parole**, che chiede R3 (sintagmi candidati alternativi) e riapre l'acetone;
(c) la **soglia e la condizione di osservazione come conoscenza**: che parrot0
possa sentirsi dire, in lingua ordinaria, che un contatto era una coincidenza.

## 22. Ritiro per contatto e prima misura di generalizzazione (25 settembre 2026)

> *F.: «dimostrami che questo tipo di addestramento L3 generalizza o comunque
> produce lezioni utili».*

### 22.1 Ritiro per contatto (§21.7a, fatto)

Trovato provando: «Rome is in Italy, and Rome is its capital.» (vera) fa nascere
un'ipotesi **sbagliata**, «capital» ≈ `located_in`, e la correzione ordinaria
«Milan is in Italy, but Milan is not its capital.» la **confermava** (secondo
sostegno): l'osservazione non vedeva la negazione. Ora lo stesso allineamento
in un turno che nega scrive un **controesempio** (`contact_counter/3`), uno
strato che sospende l'ipotesi qualunque sia il numero dei sostegni (§19; gli
episodi restano). La negazione è `negation_marker/1`, o una contrazione che la
KB sa espandere in una negazione (`function_word("isn't", "is not")`: la IR la
spezza in `isn` + `t`). Nessuna forma per «No», «but» o «isn't».

Corretto anche un difetto del §21.6: la premessa «Reading «N» as …» restava
nei turni seguenti (il motore azzera `current_turn` solo per una lista C); l'uso
dell'ipotesi è ora legato al numero del turno (`turn_counter/1`).

### 22.2 Il banco di generalizzazione

`docs/labs/l3/I2/generalizzazione.p0t`: **19/19**; risposte verbatim in
`generalizzazione-dialogo.txt`. Un solo meccanismo, nessuna riga per relazione.

| relazione | contatto (uno) | domande su soggetti tenuti fuori | esito |
|---|---|---|---|
| `born_in` | «Einstein was born in Ulm, so his birthplace is Ulm.» | Galileo, Columbus | «Reading «birthplace» as «was born in». pisa.» / «… genoa.» |
| `made_of` | «Steel is made of iron, so iron is its material.» | glass, paper | «… sand.» / «… wood pulp.» |
| `borders` | «France borders Spain, so Spain is its neighbour.» | Germany | «Reading «neighbour» as «borders». austria, belgium, czechia, …» |

| trappola | esito |
|---|---|
| inciso, un ruolo solo («Galileo Galilei, an astronomer, was born in Pisa.») | nessun episodio |
| coincidenza vera («Rome is in Italy, and Rome is its capital.») | **ipotesi sbagliata nata** (capital ≈ located_in) |
| correzione ordinaria («Milan is in Italy, but Milan isn't its capital.») | controesempio; l'ipotesi cade; le altre tre restano |
| premessa del turno giusto | non ricompare nei turni seguenti |

**Conteggi separati (§16.3):** 3 relazioni imparate da un contatto ciascuna; 5/5
trasferimenti corretti su soggetti tenuti fuori, tutti qualificati (un solo
sostegno); 1 falsa generalizzazione nata, 1 ritirata per contatto; 0 falsi
trasferimenti osservati nelle risposte; 1 caso di non-contatto correttamente
ignorato.

### 22.3 Che cosa questa misura NON dimostra (da non tacere)

1. **Sulla KB viva le tre lezioni non erano necessarie**: «birthplace»,
   «material» e «neighbour» sono già collegati (una costruzione, un
   `relation_noun`, un cue di una catena compilata), e il banco li ritira in
   memoria. La prova è che il *meccanismo* impara un nome nuovo da un contatto
   ordinario e lo trasferisce; non che oggi riempia un buco reale. Un nome
   davvero non collegato con fatti a sufficienza non l'ho trovato nel
   censimento: la KB viva collega quasi ogni nome di relazione naturale.
2. **Generalizza fra soggetti dentro una relazione**, e il *modo di imparare*
   vale fra relazioni (zero righe per relazione). Non generalizza ancora a nomi
   di più parole (R3), a relazioni solo derivate, a lingue diverse (non provato).
3. **La falsa generalizzazione nasce facilmente**: una sola frase vera basta. La
   difesa oggi è la lettura qualificata più il ritiro per contatto; manca una
   verifica attiva (una domanda discriminante prima dell'uso, §15.2).

### 22.4 Letture sbagliate dei lettori esistenti, viste sugli stessi contatti

Non sono L3, ma i contatti le mostrano, e alcune **scrivono il falso**:

- «Galileo Galilei, an astronomer, was born in Pisa.» → «Learned: astronomer was
  born in pisa.»: il frame `born_in` lega l'apposizione come soggetto;
- «France borders Spain, so Spain is its neighbour.» → «Held: france and spain so
  spain is its neighbour share a border.»: la stessa specie di T2 (§17), l'oggetto
  attraversa il confine della proposizione;
- «Rome is in Italy, and Rome is its capital.» → «Scartato: located_in(rome,
  italy_and_rome_is_its_capital) …», in italiano in una sessione inglese.

## 23. La tecnica generalizza? Addestrabilità non prevista (25 settembre 2026)

> *F.: «hai interpretato il generalizza alla lettera… io intendevo generalizza
> come tecnica, cioè produce addestrabilità non prevista dalle ipotesi di L3».*

L'ipotesi di L3 (§12, §15, §21) era stretta: un **nome inglese di proprietà**
che nomina una relazione, detto da un **maestro**, consumato da una **domanda
«what is the N of X»**. Il meccanismo di `contact.p0` non sa niente di nomi, di
inglese, di maestri né di domande: guarda che cosa resta non spiegato attorno a
una relazione che la KB tiene. Quindi si è provato che cosa impara **fuori**
da quell'ipotesi, sulla KB viva completa e **senza ritiri** (ogni verde è un
vuoto vero). Banco `docs/labs/l3/I2/tecnica.p0t`, 8/8.

| | che cosa | esito |
|---|---|---|
| **e1** | **lessico di un'altra lingua**: «Einstein was born in Ulm, so his Geburtsort is Ulm.» | «What is the Geburtsort of Napoleon?» passa da «I don't know about geburtsort.» a «Reading «geburtsort» as «was born in». ajaccio.»; «Nachbar» → «austria, france, slovenia, switzerland» per l'Italia |
| **e2** | **un altro consumatore**, non toccato: la domanda polare | «Is Pisa the Geburtsort of Galileo Galilei?» → «… Yes.» (passa da `holds/3`) |
| **e3** | **dalla prosa, senza maestro**: una frase di un paragrafo letto | «read: Marie Curie was born in Warsaw, so her Heimatstadt is Warsaw. She won two Nobel prizes.» → «What is the Heimatstadt of Christopher Columbus?» → genoa |
| **f1** | **frontiera**: un verbo | «… so he hails from Ulm.» fa nascere `hails` ≈ `born_in`, ma nessun lettore di domande consuma un verbo appreso: «Where does Napoleon hail from?» non risponde. Il limite è la **strada**, non l'apprendimento |
| **f2** | **frontiera**: leggere un'asserzione | «The Geburtsort of Kant is Konigsberg.» non scrive `born_in(kant, konigsberg)`: l'ipotesi arriva alle domande, non al lettore delle asserzioni |

**e3 ha chiesto un solo cambio, generico:** i contabili post-risposta ora girano a
ogni livello (`turn_done`), non solo al turno esterno. Una frase di prosa e una
clausola rilette come turni annidati sono unità di osservazione quanto il turno
intero. Prima un paragrafo non insegnava niente: il residuo era calcolato sul
turno intero, e la frase successiva ci aggiungeva parole.

**Che cosa se ne ricava.** La tecnica apre tre addestrabilità che nessuna delle
ipotesi di L3 conteneva: il vocabolario di un'altra lingua, l'uso da parte di
consumatori mai toccati, l'apprendimento dalla lettura senza maestro. Le due
frontiere dicono dove va il lavoro dopo. Non si tratta di imparare di più: si
tratta di **collegare ciò che si impara a chi lo usa** (lettori di verbi,
lettore delle asserzioni). Per f2 c'è una scelta di principio da fare prima del
codice: un'ipotesi con un solo sostegno può *scrivere* un fatto, o solo
rispondere con riserva? La proposta è che scriva solo da consolidata (≥ 2
sostegni) e con la provenienza dell'ipotesi. Il costo noto: ogni
nuovo lettore di asserzioni passa dalla vista `extract_frame`, che si ricostruisce
a ogni cambiamento (~6 s, debito preesistente).

## 24. ⛔ Critica di F.: i limiti del §23 sono un tradimento (25 settembre 2026)

> *F.: «questi limiti mi sembrano un tradimento, ed è come se i verbi così
> fossero metalinguistici: non va bene. Come possiamo ricondurli?»* — riferito a:
>
> *«Dove si ferma: un verbo si impara, ma nessuno lo usa. «… so he hails from
> Ulm» crea l'ipotesi, ma «Where does Napoleon hail from?» non trova una strada
> per arrivarci. La parola appresa non legge ancora le affermazioni. «The
> Geburtsort of Kant is Konigsberg.» non scrive born_in(kant, konigsberg).
> Quindi il limite adesso non è imparare, ma collegare ciò che si impara a chi
> lo usa.»*

**Che cosa ha visto, detto nei termini del progetto.** Il contatto non ha
prodotto *conoscenza della lingua*: ha prodotto una **nota su una parola**,
`contact_episode(hails, born_in, …)`, che solo un consumatore scritto apposta
(la clausola `holds/3` in `contact.p0`) sa aprire. Il verbo appreso non è
diventato un verbo di parrot0: è rimasto una cosa detta *sulla* lingua, un fatto
metalinguistico con una maniglia sola. È il cassetto senza maniglia del gen505e
(grammar.p0) ricostruito da L3, e il §2 (KB muta) sotto un altro nome. Chiamarlo
«il limite è collegare ciò che si impara a chi lo usa» era ribaltare la colpa:
**non è il consumatore che manca, è l'apprendimento che ha scritto nel posto
sbagliato.** Una persona che capisce che «hails from» vuol dire «was born in»
lo usa subito in una domanda, in un'affermazione, in una negazione, senza che
qualcuno colleghi niente.

**La regola che se ne ricava.** Ciò che il contatto conclude deve entrare nelle
**stesse rappresentazioni che la lingua già usa**: quelle che una lezione L1/L2
avrebbe scritto (`relation_noun/2` per un nome di relazione, la cornice di
lettura per un verbo), e che *tutti* i lettori già consultano. Lo stato di
ipotesi (sostegni, controesempi, ritiro come strato) resta, ma sta **sopra**
quella conoscenza, come condizione della regola che la deriva; non in un canale
parallelo che i lettori dovrebbero imparare a interrogare. Criterio operativo:
**nessun lettore deve sapere che esiste il contatto.** Se per usare una cosa
appresa per contatto bisogna toccare un consumatore, è di nuovo L2+ al
contrario.

### 24.1 Ricondotti (25 settembre 2026): il contatto scrive lingua

Tolta la clausola privata di `holds/3`. All'osservazione si registra anche la
**forma** in cui la parola è comparsa (`contact_shape/3`), e da episodio + forma
+ stato dell'ipotesi si derivano le **stesse conoscenze che una lezione avrebbe
scritto**, che i lettori già consultano:

| forma osservata | conoscenza derivata | chi la usa, senza essere toccato |
|---|---|---|
| nome dopo un possessivo che riferisce («his Geburtsort is Ulm») | `relation_noun(born_in, geburtsort)` | domanda «what is the N of X», polare, lettore delle affermazioni «The N of X is Y», stipulazioni |
| verbo dopo un soggetto che riferisce («he hails from Ulm») | `construction_frame("@S hails from @O", "@S was born in @O", born_in)` + `answer_frame(hails/hail, born_in)` | lettore delle affermazioni, domanda «Where does X hail from?» |

Misurato (`tecnica.p0t`, 13/13, KB viva senza ritiri):

- «Where does Napoleon hail from?» → «Reading «hail» as «was born in». Ajaccio.»
- «Hegel hails from Stuttgart.» → scrive `born_in(hegel, stuttgart)`
- «The Geburtsort of Kant is Konigsberg.» → «Reading «geburtsort» as «was born
  in». Learned: kant was born in konigsberg.», e poi «Where was Kant born?» →
  konigsberg, cioè la lingua di prima ritrova il fatto letto con la parola nuova.

La riserva («Reading «N» as …») sta in ogni turno che nomina la parola finché i
sostegni sono uno, non più nella risposta di un solo consumatore.

**Una conseguenza trovata e curata:** entrata nel lessico, la parola risultava
«già spiegata» al contatto successivo e l'ipotesi non si consolidava più. Una
parola che la KB legge come R *solo* perché un contatto l'ha proposta è ora una
conferma, non una spiegazione (`contact_explained_before/2`).

**Residui dichiarati:** il possessore sull'oggetto («Rome is its capital»)
resta episodio senza lessico: `relation_noun/2` ha un verso per relazione.
Il soggetto ripreso per **nome** («…, so Einstein hails from Ulm») non dà ancora
una forma verbale, solo quello ripreso da un pronome. La polare con particella
(«Does Galileo hail from Pisa?» → «… born in from pisa …») sbaglia anche con la
cornice scritta a mano: è un difetto generale del lettore polare. Le risposte del ramo
`relation_noun` presentano gli atomi grezzi («wood_pulp.»), anche per i nomi
nativi. **Costo:** un episodio nuovo sporca la vista `extract_frame`, e il turno
dopo paga la ricostruzione (5–6 s contro 1–2 s), come per una costruzione
insegnata con una lezione. È il debito della vista, non del contatto.

## 25. I cortocircuiti si conoscono, come i paradossi (F., 25 settembre 2026)

> *F.: «non puoi fare un meccanismo che intercetta i cortocircuiti? Ne dovremmo
> avere uno per intercettare i paradossi logici, perché non lo possiamo
> riusare?»*

**Il caso.** La prima versione della cornice verbale prendeva la lettura nota di R
da `extract_frame` *dentro* la regola di `construction_frame`, da cui la vista
di `extract_frame` si costruisce: una definizione che per chiudersi consulta ciò
che sta definendo. Il motore delle viste lo ha trattato come nel vecchio mondo:
cinque viste **rifiutate in silenzio** (solo `PARROT0_BOOT_TRACE` lo mostrava),
`extract_frame` rideriva centinaia di regole a ogni lettura, il boot non finiva.
Nessun fatto, nessuna risposta, nessun colpevole nominato.

**Che cosa si riusa.** La guardia anti-isteresi nella prova (`loops_cut`, gen382)
ha già la forma giusta, per la regola di F. «consapevolezza, non halt»: conta
l'evento, lo espone, e una risposta lo dice (`undetermined_cycle`). Il rifiuto di
una vista è la stessa specie a un altro livello, e ora ha lo stesso trattamento:
`kb_view_dependencies` ricorda da quale predicato è arrivata ogni dipendenza, e
quando incontra un costrutto riflessivo pubblica
`view_short_circuit(Vista, Costrutto, Catena)`, la catena dalla vista alla
regola colpevole. `/debug` lo mostra (sonda 44). Provato su una vista finta:
`short_circuit(zz_probe_view, kb_fact, cons(zz_probe_view, cons(zz_mid,
cons(kb_fact, nil))))`.

**La cura del caso** è stata spostare la lettura nota di R al momento
dell'osservazione (`contact_shape_known/3`), fuori da ogni vista.

**Che cosa manca, in ordine:**

1. **dalla consapevolezza al rimedio senza halt**: oggi la vista resta rifiutata.
   La forma coerente con `loops_cut`, che taglia il ramo ripetuto e lascia
   completo il resto, è una vista **ibrida**: si congelano le clausole che
   chiudono, e la sola clausola colpevole resta viva sopra. Una regola nuova non
   deve poter far cadere la vista intera;
2. **il ciclo logico vero** (una vista che raggiunge se stessa passando per regole,
   senza costrutti riflessivi) oggi è coperto solo dalla guardia `building`, muta:
   va pubblicato con lo stesso fatto;
3. **la voce**: un `response_template` per spiegare perché un turno è lento o una
   lettura manca, come `undetermined_cycle` per i paradossi della prova;
4. **un solo registro**: `loops_cut` e `view_short_circuit` sono la stessa
   specie. Il passo successivo è nominarla una volta sola in KB, come
   `paradox_event(Livello, …)`, invece di tenere due contatori paralleli.

### 25.1 La vista ibrida (fatta, 25 settembre 2026)

**Prima:** un costrutto riflessivo (`kb_fact`, `findall`, `apply`…) in un punto
qualunque del grafo di una vista la faceva rifiutare intera, e con lei ogni
vista che ne dipende. **Ora** si spengono solo le clausole colpevoli, e si
ricorda la strada per arrivarci:

1. **chiusura** (`view_close`, estratta da `kb_view_dependencies`): quando
   incontra il costrutto, il motore pubblica il cortocircuito, registra la
   **catena** dalla vista al predicato colpevole (`view_hybrid_record`) e spegne
   le sole regole di quel predicato che usano il costrutto (`live_rules`). Il
   resto del grafo si chiude come prima;
2. **costruzione**: mentre la vista si congela (`building_view_plus1`), le
   clausole spente tacciono; tutto il resto si congela;
3. **lettura**: a vista viva, i fatti congelati **più la differenza**
   (`view_delta`): un sotto-risolutore sullo schema di `findall/3` percorre solo
   la catena. A ogni gradino scende per le sole clausole che portano al gradino
   dopo (`delta_rule_leads`), non risponde dai fatti (già congelati), e
   all'ultimo usa solo le clausole spente. Il gradino si chiude con un goal
   marcatore (`__end_delta_step`), come `__end_inference_scope` chiude l'ambito
   della guardia anti-isteresi. Una vista senza catene si comporta come prima.

**Misurato** su una regola vera: `construction_frame` che legge `extract_frame`
con `kb_fact/2`, cioè il caso che prima non faceva finire il boot.

| | prima | ora |
|---|---|---|
| boot | > 60 s, cinque viste rifiutate | 13,5 s con trace (≈ 11 s), quattro viste **ibride** con una clausola spente ciascuna |
| «Kant zzhails from Konigsberg.» | — | «Learned: kant was born in konigsberg.» (la regola viva legge) |
| «Where was Kant born?» | — | «konigsberg.» |
| turni | — | 0,9–1,3 s |
| KB normale (nessuna catena) | boot 6,6 s | boot 4,9 s, nessuna vista ibrida |

Vista di prova mista (una clausola pulita, una riflessiva): la pulita si
congela, la riflessiva risponde viva, e un fatto asserito a runtime si vede subito
attraverso la parte viva.

**Approssimazioni dichiarate.** (a) Il gradino si riconosce dal *nome* del
predicato: un secondo goal con lo stesso predicato nello stesso corpo verrebbe
ristretto anche lui, quindi la differenza può mancare soluzioni, mai inventarne.
(b) Le catene sono al massimo 16 gradini, e la differenza al massimo 1024
soluzioni per lettura; oltre, la ricerca si marca incompleta (`budget_hit`), non
vuota. (c) La differenza non è memorizzata: si ricalcola a ogni lettura della
vista. Costa poco perché scende solo lungo la catena, ma una catena sotto una
vista molto consultata va misurata.

**Che cosa resta del §25:** il ciclo logico puro (una vista che raggiunge se
stessa senza costrutti riflessivi) è ancora coperto solo da `building`; la voce
(`response_template` per spiegarlo); un solo registro con `loops_cut`.

**Riproduzione.** Aggiungere in fondo a un file KB caricato:

```prolog
construction_frame($O, $K, $R) :- zz_bad($O, $K, $R).
zz_bad("@S zzhails from @O", $K, born_in) :- kb_fact(extract_frame, cons($K, cons(born_in, nil))).
```

e avviare con `PARROT0_BOOT_TRACE=1`: le righe `ibrida: 1 clausole spente, 1
catene vive` e `cortocircuito: kb_fact via cons(extract_frame, …)`. Senza la
vista ibrida il boot non finiva.

### 25.2 Il ciclo logico puro (fatto, 25 settembre 2026)

**Che cosa c'era.** Una vista che raggiunge sé stessa per regole, senza
costrutti riflessivi, era coperta da due guardie mute:

- **dentro la vista**, una regola a valle che la nomina: la chiusura del grafo
  la deduplicava senza vederla;
- **fra viste**, una dipendenza che è una vista già in costruzione:
  `kb_view_ensure` la saltava.

Nessuna delle due diceva niente. Il danno misurato non è nel significato: il
solver, col suo taglio sui goal ground, risponde giusto anche derivando. È nel
**costo**. Una vista ricorsiva a sinistra su un grafo con un ciclo (`zz_loop` su
p→q→r→p) si congelava **incompleta** (144 ms buttati al boot), restava non viva,
e ogni lettura rideriva dalle regole (12 352 passi per chiamata nel caso minimo).

**Che cosa c'è ora.**

1. **Consapevolezza:** tutti e due i casi pubblicano `view_cycle(Vista, Catena)`,
   con la stessa forma di `view_short_circuit`, e il trace di boot lo dice.
2. **Rimedio senza halt, il punto fisso:** una vista che si nomina (`recursive`)
   si congela **a passate**. Dentro le sue stesse regole il richiamo ricorsivo
   risponde solo dalle righe già congelate (`in_vrule`, con il marcatore
   `__end_view_rule`, lo stesso schema di `__end_delta_step`), e si ripete finché
   una passata non aggiunge righe. È la valutazione per strati di un programma
   Datalog: stesse soluzioni, costruzione finita. Oltre 64 passate la vista si
   dichiara incompleta, non vuota.

**Misurato:** `zz_loop` si congela in 4 passate, in 0,1 ms invece di 144 ms
incompleta. La chiusura transitiva `zz_reach` su un grafo aciclico, e le
enumerazioni («tutti gli Y raggiungibili da p» = 3, tutte le coppie = 9), danno
le stesse risposte di prima. Nella KB viva **non ci sono cicli**: nessuna riga
`ciclo`, fatti e tempi identici. Il meccanismo è pronto per il primo che una
lezione o un contatto ne introdurrà.

**Il ciclo fra viste** oggi si pubblica soltanto. Il rimedio (costruire le due
viste insieme fino al punto fisso comune) non c'è, perché nella KB non ne esiste
nessuno su cui misurarlo.

**Che cosa resta del §25:** la voce (`response_template` per spiegare un
cortocircuito o un ciclo), e un solo registro in KB per `loops_cut`,
`view_short_circuit` e `view_cycle`.

### 25.3 Il registro unico (fatto, 25 settembre 2026)

Quattro eventi della stessa specie avevano tre nomi e un contatore del C:
il ciclo tagliato nella prova (`loops_cut`, gen382, letto solo dal modulo di
`undetermined_cycle`), il budget esaurito, il cortocircuito di una vista, il
ciclo di una vista. Ora sono **un fatto solo**:

```text
paradox_event(Livello, Specie,                    Dove,       Dettaglio)
              proof    loop_cut | budget           Predicato   seen(Turno)
              view     short_circuit(Costrutto)    Vista       Catena
              view     cycle                       Vista       Catena
```

- **Livello della prova:** il motore lo scrive appena una query di primo livello
  finisce (`kb_note_inference`), fuori da ogni prova in corso, una volta per
  predicato e turno. «Dove» è il predicato del goal **tagliato**
  (`S->cut_pred`, un puntatore alla testa della regola: la struttura del solver,
  che sta sulla pila, non cresce). Il turno lo passa il registro delle facoltà
  (`kb_set_paradox_turn`, lo stesso orologio di `turn_counter/1`).
- **Livello delle viste:** `view_short_circuit/3` e `view_cycle/2` restano come
  facce del registro (viste KB); il motore scrive solo `paradox_event/4`.
- **Sonda:** `/debug` 44 mostra il registro intero; la 45 è assorbita.
- **Primo consumatore: il gate C1 di [inferenza-compositiva.md](inferenza-compositiva.md).**
  `inference_incomplete(current_turn, cycle | budget)` e
  `inference_cycle(current_turn, Membri)` ora si **derivano** dal registro
  (composition.p0). Prima il piano diceva che il C non li depositava, e il test li
  metteva a mano. Misurato con il ciclo insegnato di `inference_guard.p0t`
  («every zorp is a blim» … «is vex a blim»): il registro ha l'evento, i sensori
  sono veri nello stesso turno e falsi al turno dopo. `composed_answer.p0t`
  resta 18/18.

**Che cosa non è ancora unico:** il modulo che dice `undetermined_cycle` legge
ancora il contatore della *singola* query (`kb_inference_report`), non il
registro del *turno*. Le due cose non sono equivalenti: un taglio in un'altra
query dello stesso turno non deve far dire «la ricerca si chiude su se stessa»
a una domanda che non l'ha incontrato. Il passo giusto è che la composizione
(C2, già scritta e inerte) prenda la parola al posto del template, e che il
registro porti la query (o il goal di turno) che ha tagliato.

### 25.4 La composizione prende la parola (fatto, 25 settembre 2026)

La cipolla di `composition.p0` (gen505, inerte fino a oggi) ora **risponde** al
posto dei due template monolitici della domanda di classe non guadagnata
(`undetermined_cycle`, `no_support_either_way`). Il modulo C:

1. deposita `turn_goal/3` **prima** di decidere (prima lo faceva dopo aver
   risposto, e nessuna regola poteva parlare di questa domanda);
2. se il «no» non è guadagnato (ciclo, budget, o nessuna autorizzazione a
   chiudere il mondo) chiede `composed(offer, Lingua, Testo)` e dice quello; i due
   template restano come ripiego se la composizione non produce niente.

Il registro unico ora porta la **domanda** che ha tagliato
(`seen(Turno, PredicatoDellaQuery)`), e i sensori leggono solo gli eventi di
questa domanda (`turn_goal_query/2`: la classe e la sua gemella `holds1`).

Tre correzioni di conoscenza, trovate guardando le risposte vere:

- **lo stadio `open_extension`** porta il perché che solo
  `no_support_either_way` diceva («knowing some iron oxide minerals does not tell
  me they are all of them»). È una tesi: la domanda è non ancorata, la classe ha un
  membro noto e nessuna regola la chiude;
- **il budget dentro un ciclo** non è una seconda causa: lo stadio del budget vale
  solo senza ciclo (`turn_budget_only/1`);
- **la massima dentro un ciclo**: la prova che chiuderebbe il mondo passa dallo
  stesso ciclo e `naf` declina, quindi massima e offerta sparivano. Un ciclo
  tagliato su questa domanda rende il «no» non guadagnato per definizione.

Risposte vere (`docs/labs/l3/I2/composizione.p0t`):

- ciclo: «I cannot settle that: no fact I hold decides whether vex is a blim, and
  the rules for blim lead back into each other, so the search closes on itself
  instead of reaching an answer. Not proved is not the same as false. Tell me
  either way and I will hold it.»
- estensione aperta: «I cannot settle that: no fact I hold decides whether zelnik
  is an iron oxide mineral, and knowing some iron oxide minerals does not tell me
  they are all of them. Not proved is not the same as false. Tell me either way
  and I will hold it.»
- italiano, nessuna riga di C: «Non posso stabilirlo: nessun fatto che ho decide
  se vex è un blim, e le regole di blim si rimandano a vicenda, quindi la ricerca
  si chiude su se stessa…»
- una risposta guadagnata resta «Yes.».

**Residui.** I membri del ciclo sono i predicati dei goal *tagliati*, non
l'intero anello: «is vex a zorp» dice «the rules for blim» (è vero, ma
incompleto). La relazione gemella (`multigoal.p0t`, «I don't know: nothing I
hold says tom grandparent bob») è un altro modulo con il suo template: seconda
famiglia da sfogliare. Con questo il §25 è chiuso nei quattro punti che F. aveva
aperto: consapevolezza, rimedio senza halt, registro unico, voce.

### 25.5 La famiglia delle relazioni nella stessa cipolla (25 settembre 2026)

`p0_relation_verdict` prova ogni via al «sì» e ogni «no» guadagnato
(negazione detta, valore unico occupato, esclusione, simmetria, implicazione,
`holds`, inversa, transitiva, ereditata). Quando nessuna regge, ora non dice più
il template `no_support_relation`: deposita la domanda come sensore del turno,
`turn_relation_goal(current_turn, R, S, O)`, e dice `composed(offer, …)`.

**Non una seconda cipolla, la stessa.** Verdetto, «nessun fatto decide»,
ciclo con i membri, budget, massima e offerta sono gli stadi della domanda di
classe. Cambia solo il **nucleo** (la proposizione in esame), e le tesi guadagnano
una clausola per la relazione: `turn_goal_unanchored/1`, `turn_unsettled/1`,
`unearned_negation_risk/1`, e i sensori della prova, attraverso
`turn_goal_head/2`. I due sensori di domanda si escludono: ogni modulo ritira
quello dell'altro. Il template resta come ripiego.

«is tom the grandparent of bob?» → «I cannot settle that: no fact I hold decides
whether tom grandparent bob. Not proved is not the same as false. Tell me either
way and I will hold it.» Prima: «I don't know: nothing I hold says tom
grandparent bob, and nothing says it isn't so. …»

**Il nucleo, e il difetto che mostra.** La proposizione si dice con la cornice
preferita della relazione (`say_frame_preferred`), altrimenti con le parole
lette (`relation_said/4`, in KB; prima era `p0_say_fact` nel C). Ho provato a
scegliere fra le altre cornici di `say_frame`: diceva «cat ate grass», poi «cat is
eat grass». Il motivo non è nella scelta: **il lettore polare consegna letture
sbagliate**. La relazione arriva come lemma della domanda («eat», «locate»,
«border»), non come predicato (`eats`, `located_in`, `borders`), e l'oggetto
porta la preposizione («einstein born in **in pisa**»). È la stessa causa per cui
«does france border spain?» risponde «I don't know» con `borders(france, spain)`
in KB: una **strada rotta**, non una lacuna. Il nucleo dice fedelmente che cosa è
stato letto, e così il difetto si vede. Indovinare una forma migliore qui lo
avrebbe nascosto. **Prossimo lavoro: il lettore polare delle relazioni.**

**Test rivalidati nel significato:** `multigoal.p0t` (onestà al posto di un «No.»
senza licenza: ora `<~ no fact I hold decides whether tom grandparent bob`,
`<~ Not proved…`, `<! No.`); le due guardie di assenza di `mix_causal_chain.p0t`
e `mix_function_from_reading.p0t` (una domanda di funzione non deve essere letta
come polare di relazione) vietano ora anche la frase composta, altrimenti
sarebbero diventate verdi per costruzione.

**Controllato contro la base prima della composizione (`4968127c`):**
`expert/grammar`, `conj`, `taught_rules`, `hypothesis` e `meta/retract` hanno
gli stessi rossi, sulle stesse righe. In `retract.p0t` il vecchio template diceva
«Knowing some **mans**», la composizione dice «knowing some **men**» (il plurale
che la KB conosce).


## 26. I5 — gli strumenti del contatto si imparano per contatto (25 settembre 2026)

> *F.: «procedi con I5».* Condizione d'uscita del §16.1: «nuova relazione, nuovo
> strumento di contatto e correzione della condizione appresa; nessun nuovo
> teach-handler o schema di contatto; uso della condizione corretta su un caso
> tenuto fuori dal dialogo».

### 26.1 Il rosso, misurato prima

La condizione di osservazione vuole un residuo di **una** parola. Misurato sulla
KB viva con «Einstein was born in Ulm⟨X⟩ Geburtsort is Ulm.»: passa solo
«; that is, his» (parole funzionali); **bloccano** «in other words», «namely»,
«put differently», «which means», «in short», «and hence», «incidentally»,
«moreover», «besides». Uno strumento del contatto nuovo era invisibile, e
renderlo visibile chiedeva una riga dell'ingegnere: il passo falso del §1-bis.

### 26.2 Il circuito (kb/core/contact.p0, zero C)

1. **Il contatto quasi riuscito si conserva.** L'allineamento
   (`contact_alignment_here/4`) è separato dalla scelta del residuo: una parola →
   episodio, come prima; da due a quattro parole → `contact_near(R, E, Ws)`, con
   la forma di ogni parola candidata presa subito (`contact_near_shape/3`),
   perché dopo la IR del turno non c'è più.
2. **L'induzione.** Due contatti in sospeso su relazioni **diverse** con parole in
   più **comuni** (C), e un nome solo per parte: C compare qualunque sia la
   relazione, quindi non ne nomina nessuna → `contact_instrument(C, from(E1, E2))`.
   I due contatti si **rileggono** ed entrano come episodi, con la forma e con
   `contact_episode_via(N, R, via(E, C))`.
3. **L'uso.** Le parole di uno strumento in forza non contano nel residuo
   (`contact_instrument_word/1` in `contact_unexplained`): un contatto successivo
   con lo stesso strumento dà un episodio subito, per qualunque relazione.
4. **La correzione con lo stesso circuito.** Uno strumento vale finché almeno un
   episodio nato attraverso di lui non è smentito
   (`contact_instrument_credited/1`). I controesempi sono quelli ordinari del
   §22.1: smentite tutte le ipotesi che aveva portato, lo strumento cade (uno
   strato: resta leggibile, `/debug` lo mostra `discredited`) e **non rinasce**
   da una coppia nuova di contatti in sospeso (`contact_instrument_known_bad/1`,
   trovato provando: la prima stesura lo re-induceva e gli ridava credito).

### 26.3 Misure

`docs/labs/l3/I5/strumenti.p0t` **30/30** (1 min 48 s, fuori dal soft-test):

| prova | esito |
|---|---|
| prima: «…; in other words, his Geburtsort is Ulm.» | nessun episodio, contatto in sospeso; «I don't know about geburtsort» |
| stessa relazione due volte («Heimatstadt») | nessuno strumento: una relazione sola non distingue strumento e nome |
| seconda relazione («Steel is made of iron; in other words, its Werkstoff is iron.») | strumento `[words]` («other» è funzionale); Geburtsort → ajaccio, Werkstoff → sand |
| **tenuto fuori, relazione nuova** («France borders Spain; in other words, its Nachbar is Spain.») | episodio subito; Nachbar of Italy → austria, … |
| strumento sbagliato da due coincidenze vere («Rome … moreover, Rome is its capital.», «Galileo … moreover, Pisa is his university.») | `[moreover]` in forza, due ipotesi sbagliate nate |
| controesempi ordinari («Milan … isn't its capital.», «Einstein … Ulm is not his university.») | dopo il primo lo strumento regge, dopo il secondo cade; `born_in(einstein, ulm)` resta (strato §14.6) |
| **tenuto fuori dopo la correzione** («Napoleon … moreover, his Geburtsort is Ajaccio.») | nessun episodio; nella stessa sessione «in other words» insegna ancora |

Regressioni: I2 `audit-crescita` 35, `ambiguita` 40, `banco` 16, `tecnica` 13,
`generalizzazione` 19, `composizione` 7; `make soft-test` verde in 14 s.
Durata (`docs/labs/l3/I5/persistenza.sh`, solo `make chat`, copia completa): nel
processo nuovo «in other words» insegna Nachbar, «moreover» resta screditato.

**Trovato e curato lungo la strada:** la risposta «parola caduta» del §19.3
intercettava le parole con una lettura nativa: «What is the capital of Germany?»
diceva «I no longer read «capital» as «is located in»…». Ora vale solo per una
parola che la lingua non legge per nessun'altra via (`contact_word_native/1`).

### 26.4 Prova del §1-bis e residuo, elencato

- Nessuna `turn_form`, nessun lettore per uno strumento, nessuna parola di
  strumento scritta in KB: «in other words» e «moreover» sono **risultati** di
  contatti, con provenienza. Uno strumento mai visto si apprende senza patch.
- Il **significato** dello strumento si corregge parlando, con lo stesso
  controesempio ordinario che corregge un'ipotesi: nessun «no, qui è un
  inciso» da riconoscere.

**Residuo metalinguistico (G1), da non tacere:**

1. la **regola d'induzione** (intersezione fra relazioni diverse, da due a
   quattro parole, un nome per parte) e la **politica di credito** (vale finché
   un episodio regge) sono scritte dall'ingegnere; non si correggono parlando;
2. lo strumento è un **insieme di parole**, non una posizione: le sue parole sono
   trasparenti ovunque nel residuo di un contatto;
3. si apprende il **vocabolario** degli strumenti, non la **struttura** della
   condizione: un soggetto ripreso per nome, un possessore sull'oggetto, un
   residuo di più parole che È il nome (R3) restano fuori;
4. un solo riempitivo non spiegato diventa ancora un **nome**: «…; moreover,
   Spain is its neighbour» proporrebbe «moreover» ≈ `borders` (non provato,
   dedotto dalla condizione): la cura naturale è che uno strumento noto non
   possa essere un nome, oggi vale solo per quelli già indotti;
5. le frasi italiane del §19.3 restano non verificate.

## 27. R3 — un nome di relazione di più parole si impara per contatto; l'acetone del §15 (25 settembre 2026)

> *F.: «procedi con R3».*

### 27.1 Il rosso, misurato prima

Con il ponte nativo ritirato in memoria (setup del §15.1), «Acetone boils at 56
degrees Celsius; its boiling point is 56 degrees Celsius.» non insegnava niente,
e «What is the boiling point of ethanol?» non aveva risposta. Tre lacune, trovate
interrogando i pezzi uno per uno:

1. il **valore** è una quantità (R2, `quantity(56, degree_celsius)`), ma i ruoli
   candidati erano solo le entità del chunker;
2. le **entità** del chunker erano sbagliate: `acetone_boils` («boils» non è
   riconosciuto verbo) e `boiling` («point» chiude il sintagma, §20.3);
3. **nessuna cornice legge «boils at»**: senza la costruzione del «boiling point»
   `boils_at` non ha `extract_frame`, quindi per il circuito non era leggibile.
   RI-020 dice già che una relazione del mondo è nominata dalle sue parole, ma
   solo per allineare le lezioni.

### 27.2 La cura (kb/core/contact.p0, zero C)

- **I ruoli dai fatti** (§14.2 passo 1): dal valore con la parola ripetuta si
  chiedono alla KB i soggetti che hanno quella relazione con quel valore, e si
  accettano se il turno li nomina parola per parola. Solo per le relazioni che il
  turno **nomina con le sue parole** in fila («boils at» → `boils_at`): tutte le
  ~400 relazioni leggibili esaurivano il budget, e una scansione dei fatti con la
  relazione libera costava minuti (misurato). La via delle entità resta per le
  altre, com'era.
- **I valori quantità** sono ruoli, con la superficie con cui la KB li tiene
  (`56_degrees_celsius`).
- **Il nome è lo span, non il residuo.** Dopo la parola che riferisce, le parole
  piene in fila stanno dove sta un nome di relazione: se il residuo cade tutto lì
  dentro, il nome è lo span intero — anche con parole già note. Col solo residuo
  «Steel is made of iron, so its raw material is iron.» proponeva «raw» ≈
  `made_of`. Le parole di uno strumento (I5) stanno fuori dallo span e il
  contatto resta in sospeso: la posizione decide, non un elenco.
- Il nome composto entra nella lingua come i nomi di più parole insegnati
  (RI-010): `relation_noun(boils_at, "boiling point")`. Il turno «dice» N anche
  quando N è uno span (`contact_says/2`), per la riserva, la provenienza, le
  domande sul perché e le letture concorrenti.
- **Glossa**: da una lettura nota «@S … @O» senza parole proposte dal contatto,
  o dalle parole del nome della relazione («boils at»). Prima, dopo
  l'apprendimento, la glossa veniva dalla cornice nata dal nome stesso: «Reading
  «boiling point» as «boiling point of @S is»».

### 27.3 Due difetti del circuito, trovati per strada

- **Il budget di una query sola.** `turn_after_reply(T, contact)` scriveva
  l'episodio e poi chiedeva forma e strumento nella stessa query: il budget era
  già consumato dall'allineamento, la forma falliva in silenzio (episodio sì,
  `relation_noun` no). Il registro lo diceva: `paradox(proof, budget,
  turn_after_reply)`. Ora un contabile a sé, `contact_settle`, dà forma e
  strumento agli episodi scritti nel turno (`kb_turn_act/4`).
- **Il turno senza fine dopo il ritiro di una costruzione** — riprodotto anche
  parlando («forget that the boiling point of x is y means x boils at y», poi un
  contatto): il fallimento dopo l'asserzione faceva tornare indietro a cercare
  altri allineamenti, e ogni `extract_frame` della via delle entità si
  ricalcolava dalle regole (la vista, sporcata dall'asserzione, non si ricostruisce
  a metà query). Trovato con gdb sui goal di `solve_frame`: `contact_role_pair` →
  `contact_readable_relation` → `extract_frame` → `construction_claims` →
  `construction_variant`. Tolta la causa: dopo l'asserzione `contact_store` non
  chiede più niente. Lo stesso turno col setup: da 20 s (prima di I5) e poi
  blocco, a 13 s.

### 27.4 Misure

`docs/labs/l3/R3/nomi-composti.p0t` **23/23** (1 min 15 s, fuori dal soft-test):

| prova | esito |
|---|---|
| base («What is the home town of Napoleon?») | nessuna risposta |
| «Einstein was born in Ulm, so his home town is Ulm.» | `relation_noun(born_in, "home town")`; Napoleon → «Reading «home town» as «was born in». ajaccio.», Galileo → pisa |
| «Steel is made of iron, so its raw material is iron.» | nome `raw_material`, **non** `raw`; glass → sand |
| «…; in other words, his Geburtsort is Ulm.» | nessuno span: resta contatto in sospeso (I5 intatto) |
| acetone, setup §15.1: base | «What is the boiling point of ethanol?» senza 78 |
| contatto con quantità, «boils at» senza cornice | episodio `boiling_point` ≈ `boils_at`, lingua |
| trasferimento tenuto fuori | ethanol → «Reading «boiling point» as «boils at». 78 degrees Celsius, lower than water.»; nitrogen → -196 |
| prosa: «The boiling point of methanol is 65 degrees Celsius.» | `boils_at(methanol, 65_degrees_celsius)` con sostegno d'ipotesi (§14.6) |
| ritiro (ablazione dichiarata) | ethanol senza 78; il fatto del metanolo sospeso; quello dell'acetone resta |

### 27.5 Che cosa resta (non tacere)

- Il chunker continua a leggere `acetone_boils` e `its boiling`: R3 nel senso del
  §20.3 (candidati alternativi dei sintagmi per tutti i consumatori) **non** è
  fatto; il contatto lo aggira chiedendo i ruoli ai fatti. Ogni altro lettore che
  usa `np_candidate` ha ancora il difetto.
- «boils at» resta illeggibile come affermazione («Acetone boils at 56 degrees
  Celsius.» → «I don't know about acetone yet»): la relazione nominata dalle sue
  parole vale nell'osservazione, non come lettore globale. Aprirla a tutti i
  lettori è una decisione (furti di turno, §17.5), da misurare a parte.
- L'acqua del §15.2 resta fuori: `boils_at(water, …)` è una frase, non una
  quantità, e l'ancora non combacia (ostacolo già dichiarato nel §15.2).
- Il contatto sul «boiling point» nella KB viva senza setup non insegna niente,
  giustamente: è già saputo (§21.5).
- Costo: un turno di contatto a forma nuova costa 7–13 s; la ricostruzione di
  `extract_frame` dopo un episodio pesa sul turno dopo (5–6 s), debito della vista.

## 28. L'ordine superiore prima dei difetti: le condizioni dell'apprendimento si imparano (25 settembre 2026)

> *F.: «inizia il piano ma non partire dai difetti: parti dalle cose che
> abilitano l'apprendimento di ordine superiore. Potrebbe essere che gli stessi
> difetti possano essere indirizzati da lezioni di ordine superiore».*

### 28.1 Che cosa c'è già, e dove è chiuso

Il primo ordine è «N nomina R». L'ordine superiore c'è già in un punto solo,
I5 (§26), con un motore di quattro pezzi:

1. il contatto **quasi riuscito** si conserva, invece di sparire;
2. quando due contatti quasi riusciti su relazioni **diverse** mancano per la
   stessa ragione, quella ragione non appartiene a nessuna relazione: nasce
   un'ipotesi **sulla condizione**, non su una parola;
3. i due contatti si rileggono ed entrano come episodi «nati attraverso» di lei;
4. la condizione appresa vale finché regge almeno uno di quegli episodi, e i
   controesempi ordinari la fanno cadere.

Però il motore è cablato su **una** condizione, la dimensione del residuo, e
impara soltanto un **vocabolario** (le parole di uno strumento). Le altre
condizioni dell'osservatore sono scritte dall'ingegnere e non si imparano
(§26.4, residuo 1–3): come un ruolo torna nella frase, dove sta il nome, in
quale verso sta il possessore, se ruoli e fatti devono combaciare con le entità
del chunker.

### 28.2 La mossa: ogni condizione dell'osservatore diventa un luogo di ordine superiore

Una condizione del contatto si divide in due parti:

- **che cosa si può percepire.** Primitive date dall'ingegnere, per esempio «il
  turno dice due volte la stessa parola piena», «un pronome», «un possessivo».
  Sono il residuo G1 dichiarato, come l'intersezione fra relazioni del §26;
- **quali percezioni contano** per quella condizione. Sono **dati**: alcuni
  seminati come bootstrap (il pronome e il possessivo riprendono un ruolo),
  altri **appresi** dal motore del §28.1 e correggibili nello stesso modo.

Quando un contatto sarebbe riuscito tranne che per una condizione, e una
percezione non ancora ammessa la soddisfa, il contatto si conserva **tipizzato
con la percezione che lo avrebbe fatto riuscire**. Il resto è il motore di I5,
generalizzato dal vocabolario alle condizioni.

### 28.3 Perché questo tocca anche i difetti

La domanda di F. è se i difetti dell'handoff siano indirizzabili da lezioni di
ordine superiore. Ipotesi, da verificare una per una:

| difetto (handoff, punto 5) | condizione dell'osservatore | lezione di ordine superiore possibile |
|---|---|---|
| verbo appreso solo con il soggetto ripreso da un pronome | **riferimento** | «un ruolo ripetuto per nome torna nella frase» (H1, qui sotto) |
| «Rome is its capital» resta episodio senza lingua | **verso del possessore** | un verso nuovo del nome, appreso da due contatti a verso rovesciato |
| il chunker legge `acetone_boils`; i ruoli si prendono ai fatti | **ruoli = entità della lettura** | i ruoli dai fatti che contraddicono il chunker sono un confine di sintagma: se ricorre, la lezione è `reading_boundary` (la rappresentazione di L2), non una patch |
| «boils at» letto solo dall'osservatore; il «No.» falso | **relazione nominata dalle sue parole** | dopo episodi in cui la relazione era detta con le sue parole, la lingua impara che quelle parole la leggono, per tutti i lettori |

Nessuna di queste righe è ancora provata. H1 prova la prima.

### 28.4 H1 — il ruolo ripreso per nome

**Rosso, misurato prima della cura** (`make test-engine`, KB agi completa):
«Einstein was born in Ulm, so Einstein hails from Ulm.» non lascia niente, né
episodio né contatto in sospeso. «Where does Napoleon hail from?» non risponde
ajaccio. Con «he» la stessa frase insegna (§24.1).

**Condizione di uscita, fissata prima:**

- un contatto solo, o due sulla stessa relazione, non insegna niente: il nome
  ripetuto non basta a distinguere un modo di riferirsi da una coincidenza;
- con una seconda relazione nasce `contact_reference_learned(repeated_name, …)`;
  i due contatti si rileggono ed entrano nella lingua («Where does Napoleon
  hail from?» → ajaccio);
- **caso tenuto fuori:** una terza relazione, ripresa per nome, dà subito un
  episodio;
- i banchi I2, I5 e R3 restano verdi; nessuna forma, nessuna parola nuova
  scritta in KB, zero C.

### 28.5 H1 — fatto (kb/core/contact.p0, zero C)

- **Il riferimento è dato.** `contact_refers_here/1` non elenca più il pronome e
  il possessivo: chiede i modi ammessi (`contact_reference_way/1`), cioè quelli
  seminati (`contact_reference_seed/1`: possessive, pronoun) più quelli appresi
  e in forza. Che cosa si percepisce resta in `contact_refers_by/2`: possessive,
  pronoun, repeated_name (due parole piene diverse dette due volte).
- **L'allineamento è separato dal cancello** (`contact_alignment_core/4`): un
  contatto che manca solo per il riferimento si conserva come
  `contact_near_way(Way, R, e(Ep, N))`, con la forma della parola presa subito.
- **L'induzione, la rilettura e il credito sono quelli di I5.** Due contatti in
  sospeso con la stessa percezione su relazioni diverse →
  `contact_reference_learned(Way, from(E1, E2))`; i due contatti si rileggono
  con `via(E, way(Way))`. Il modo vale finché un episodio nato attraverso di lui
  regge (`contact_instrument_credited/1`, lo stesso predicato degli strumenti),
  e un modo screditato non rinasce.
- **La forma del verbo** vale anche dopo il soggetto detto di nuovo per nome
  («…, so Einstein hails from Ulm» → «@S hails from @O»).

**Misure.** `docs/labs/l3/H1/riferimento.p0t` **26/26** (1 min 20 s):

| prova | esito |
|---|---|
| prima: «Einstein was born in Ulm, so Einstein hails from Ulm.» | nessun episodio; `near_way(repeated_name, born_in, …, hails)`; «Where does Napoleon hail from?» senza ajaccio |
| stessa relazione due volte (Galileo, Pisa) | nessun modo appreso |
| seconda relazione («Steel is made of iron, so steel derives from iron.») | nasce `repeated_name`; «Where does Napoleon hail from?» → ajaccio; «What does glass derive from?» → sand |
| **tenuto fuori** («France borders Spain, so France touches Spain.») | episodio subito, con credito `via(…, way(repeated_name))` |
| modo appreso da due coincidenze vere (capital ≈ located_in, harder ≈ made_of) | in forza, con due ipotesi sbagliate |
| controesempi ordinari (Milan non capitale; il vetro non è più duro della sabbia) | dopo il primo il modo regge, dopo il secondo cade |
| **tenuto fuori dopo la correzione** (Einstein/hails, France/touches) | nessun episodio, il modo non rinasce; il pronome («so he hails from Ajaccio») insegna ancora |

**Regressioni:** I2 `audit-crescita` 35, `ambiguita` 40, `banco` 16, `tecnica`
13, `generalizzazione` 19, `composizione` 7; I5 `strumenti` 30; R3
`nomi-composti` 23 — tutte verdi.

**Soft-test.** Al primo giro 17–18 s su 15. Misurato file per file, a
contabile acceso e spento: `facts.p0t` da solo oscilla fra 12 e 14,3 s e il
contabile non ne spiega la differenza (acceso è stato anche più veloce); anche
spento il soft-test dava 16–17 s. Su indicazione di F. `facts.p0t` esce dal
soft-test (Makefile, motivato): verde in 3 s.

**Residuo (G1), da non tacere:**

1. **Che cosa si può percepire** (`contact_refers_by/2`) è scritto
   dall'ingegnere; si impara soltanto **quale percezione conta**. Una
   percezione nuova (per esempio un epiteto, «the physicist») resta fuori.
2. La regola d'induzione (due relazioni diverse) e la politica di credito sono
   le stesse di I5, sempre scritte in KB, non apprese. È il terzo ordine: una
   politica di credito che si corregge per contatto.
3. Il cancello economico «al massimo 16 token» è una soglia dell'ingegnere.
4. Il terzo campo dell'episodio (`s`/`o`, quale ruolo porta la parola ripetuta)
   è ambiguo quando entrambi tornano per nome: l'enumerazione dà `o`.
5. La durata (salva → processo nuovo) non è provata per H1.
6. Nella seconda metà del banco il modo si impara da coincidenze: prova che è
   correggibile, non che le coincidenze siano rare.

**Prossimo passo proposto (§28.3).** Lo stesso motore sulla condizione **ruoli
= entità della lettura**: un contatto riuscito solo prendendo i ruoli dai fatti
(acetone, §27) contraddice il chunker. Se ricorre, la lezione di ordine
superiore è un confine di sintagma nella rappresentazione di L2
(`reading_boundary_lesson`). Così il difetto `acetone_boils` si cura con una
lezione e non con una patch.
