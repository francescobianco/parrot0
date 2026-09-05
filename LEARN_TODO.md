# LEARN_TODO — la coda dei temi da apprendere

## HANDOFF 2026-09-05 — difetto di `extract_frame` RISOLTO; prossima porta: relazioni insegnabili

**Questa nota e l'ordine operativo seguente prevalgono sugli handoff storici
sotto.** Checkpoint di partenza `ba45d82`, gia' pubblicato e pulito. Il lavoro
richiesto era sul difetto, non sul checkpoint precedente.

La causa non era il bucket, l'arieta' due o il budget dei 360 fatti:
**`extract_frame` era una vista materializzata rimasta valida dopo una lezione
che ne cambiava le premesse.** La firma contava solo poche dipendenze dichiarate
a mano con `view_depends`, ignorando altre fonti dei corpi delle regole. Lo
stesso corpo sotto `probe_two`, senza cache, vedeva correttamente la lezione.
La traccia interna ha mostrato la firma immutata dopo `relation(zzz)`; una
modifica a `relation_verb`, gia' dichiarata, faceva comparire anche quel pattern.

**Correzione generale nel motore, non un nuovo elenco nella KB:**

- dipendenze ricavate transitivamente dai corpi delle regole, comprese fonti
  ancora vuote e goal negati; `view_depends` resta un'integrazione facoltativa;
- invalidazione su assert/retract e modifica delle regole, anche a cardinalita'
  invariata; il traffico estraneo alle dipendenze conserva la cache;
- righe scadute invisibili durante la risoluzione e rimosse fuori dal solver;
  anche una regola intermediaria vede la conoscenza aggiornata;
- insiemi di dipendenze e registro delle viste dinamici; conteggio dei soli
  inserimenti derivati reali, non dei successi idempotenti;
- origine della cache distinta da `KB_INDUCED`; enumerazione incompleta mai
  promossa a vista completa. Meta-chiamate/dipendenze dinamiche non analizzabili
  restano sull'inferenza ordinaria.

**Verifica:** build, riproduzione MCP e sonde interne con boot reale AGI;
crescita, retract, sostituzione, transito e fallback verificati. Conversazione:
nuovo verbo insegnato per menzione → lettura → retract → muro → reteach →
lettura e risposta. Conservate prova ed enumerazione dei minerali gia' appresi.
Nessuna suite del progetto, nessuna modifica ai file KB, nessun `/save`.
`meta-capability-only`, `W=0`: le sonde di motore **non sono apprendimento**.
[Diagnosi, evidenze e limiti](docs/labs/apprendimento-assistito/2026-09-05-cache-e-comprensione.md).

**Non confondere il difetto chiuso con la capacita' ancora da aprire.**
`relation_noun` asserita a runtime ora genera il frame; manca ancora il ciclo
parlato completo per insegnare un nuovo nome di relazione e interrogarlo.
La vecchia nota parlava di cinque nomi: nel file attuale quelli inglesi sono
otto. Il problema non era il loro numero, ma la porta di crescita e il consumer.

## Verso la comprensione universale e la KB viva e fertile — ordine operativo

Direzione richiesta da F.: **comprensione assoluta e universale**. Qui significa
togliere i confini di dominio, lingua e superficie imposti dal motore, facendo
crescere conoscenza e capacita' per insegnamento. Non e' una certificazione di
onniscienza ottenibile da un numero finito di risposte: ogni incremento deve
mostrare che cosa apre, che cosa non comprende e quale lezione gli manca.

Questa coda estende le attivita' di supercomprensione gia' descritte sotto:
non crea un secondo lettore, un secondo arbitro o una KB ridotta per provarle.

### P0 — una relazione nuova, dalla lezione alla domanda

- [ ] **Porta didattica del nome di relazione.** Un docente che non conosce
  predicati/arita' deve poter spiegare che cosa nomina una parola, anche composta.
  Trasformare quella lezione in `relation_noun` con meccanica e morfologia KB-first;
  distinguere nome di proprieta', verbo e semplice appartenenza a una classe.
  Chiusura: forma ignota prima, nuova lettura dopo, retract la spegne, reteach
  la riapre, stesso binario e nessuna clausola tecnica detta dal docente.
- [ ] **Stesso significato per affermare, domandare e spiegare.** Legare il lato
  domanda allo stesso `relation_noun` del lettore: non insegnare separatamente
  una stringa di risposta per ogni nome. Percorso da chiudere:
  lezione del nome → frase con soggetto/oggetto reali → domanda diretta e
  inversa → prova → correzione → ritrattazione → `/save` → processo fresco.
  Provare nomi di relazione ed entita' multi-parola, con una seconda relazione
  nuova che non richieda modifiche C.
- [ ] **Niente conferme didattiche vuote.** Il muro deve suggerire una lezione
  che il lettore sa davvero acquisire. Non suggerire «warp is a relation verb»
  quando manca un nome di proprieta'. La conferma deve corrispondere a una
  modifica verificabile della capacita'; ambiguita' → domanda mirata.

### P1 — comprendere strutture, non posizioni di parole

- [ ] **Binding condiviso degli argomenti.** Riprendere il censimento in
  `C_TODO`: eliminare i restanti `nw == N`, `w[2]`, `w[4]` semantici.
  Le stesse entita'/classi devono restare intere in lettura, domanda, prova,
  provenienza e retract. Chiusura: cambiare la lunghezza degli slot non cambia
  l'intento ne' sposta il turno a una facolta' estranea.
- [ ] **Composizione di relazioni e ambiti.** Riutilizzare frame e inferenza
  compositiva per coordinazioni, qualificazioni, quantificatori, negazioni e
  condizioni annidate; annotare chi fa cosa, a chi, in quale ambito.
  Chiusura: una struttura insegnata si trasferisce a contenuti reali diversi,
  preserva polarita'/scope e non impara come fatto una domanda o un'ipotesi.
- [ ] **Arbitrato per evidenza e lacuna.** Proseguire S4 nel piano esistente:
  confrontare copertura e argomenti delle letture concorrenti. Nessuna guardia
  ad hoc per cedere il turno. Chiusura: le alternative restano ispezionabili,
  la migliore risponde alla domanda posta, il pareggio produce chiarimento.
  Conservare la decisione di F. richiesta dal piano per la scelta architetturale.
- [ ] **Lingue come conoscenza.** Prima sanare gli atomi ibridi italiani;
  poi insegnare ruoli, connettori, flessioni e parafrasi collegati agli stessi
  significati, senza tradurre distruttivamente nomi e citazioni.
  Chiusura: lezione in una lingua, domanda/prova nell'altra, retract del
  collegamento linguistico senza perdere il fatto del mondo.

### P2 — una KB viva: sapere perche', dubitare e correggersi

- [ ] **Fonte e forza del claim fino alla risposta.** Portare provenienza,
  tempo, contesto, attribuzione e stato epistemico dal frame alla prova.
  Distinguere fatto, inferenza, proposta induttiva, ipotesi e assenza di prova.
  Chiusura: spiegare anche il no guadagnato da «why is …»; una citazione o una
  supposizione non diventa una convinzione senza un atto autorizzato.
- [ ] **Revisione che attraversa le conseguenze.** Ritirare/correggere una
  premessa deve aggiornare deduzioni, proposte induttive e domande aperte.
  L'induzione resta una domanda da verificare, non una nuova verita'.
  Chiusura: un controesempio reale cambia subito le conseguenze; re-insegnare
  ripristina solo cio' che ha di nuovo supporto, anche dopo salvataggio/reload.
- [ ] **Lacune che generano buone domande.** Una lettura parziale deve dire
  quale ruolo, relazione o premessa manca e chiedere il dato discriminante.
  Chiusura: la risposta del docente risolve il turno sospeso e migliora anche
  un altro turno reale; non basta conservare una receipt o dichiarare «Learned».

### P3 — una KB fertile: connessioni nuove e crescita sostenibile

- [ ] **Ponti veri fra conoscenze gia' presenti.** Cercare una domanda utile
  che richieda archi distribuiti in piu' parti della KB completa. Se manca un
  arco, insegnarlo parlando da una fonte verificata, non fabbricare il mondo
  della prova. Chiusura: risposta nuova dedotta, prova leggibile, un secondo
  uso utile del ponte e ablazione che renda visibile proprio la premessa persa.
- [ ] **Costo proporzionato alla conoscenza pertinente.** Estendere il
  contratto delle viste a dipendenze dinamiche/riflessione e arita' ulteriori,
  mantenendo fallback esplicito. Censire gli altri consumer di `kb_revision`
  che scambiano cardinalita' per revisione. Chiusura: assert/retract a caldo
  corretti, niente ricostruzione per ogni traccia del turno, nessun insieme
  troncato presentato come completo; misurare costi sulla KB piena.
- [ ] **Consolidamento e richiamo fresco.** A ogni lotto utile: report con
  lezione, fatto nuovo o capacita' aperta, trasferimento, ablazione e limiti;
  salvare solo conoscenza verificata e riaprire un processo AGI.
  Chiusura: la nuova conoscenza viene usata in una domanda diversa da quella
  della lezione; conteggi `W/L/C/P/O/X`, resa e richiamo sono evidenze, non
  sostituti della comprensione.

**Prossima azione concreta:** prendere P0, non ricominciare la caccia al bucket.
Prima leggere `MANTRA.md` e `PRINCIPLES.md`, poi una sessione AGI unica.
Resta valida la pausa sulle suite: build e conversazioni; sonde tecniche solo
quando servono a distinguere un difetto di motore da una lacuna della KB.

---


## HANDOFF 2026-09-05 — pausa richiesta da F., checkpoint consolidato

**Riprendere da questa nota; prevale sui vecchi handoff sotto.** Il primo
incremento e' `6276cb2`, gia' pubblicato: domanda di definizione distinta dalla
richiesta di prova; tetto di otto token rimosso dal tratto interrogativo.

Poi tre lezioni vere, dette in inglese nella stessa sessione con profilo AGI:
bornite, chalcocite e covellite sono `copper sulfide mineral`. Fonti riaperte
oggi: tre schede del *Handbook of Mineralogy*. Prova, enumerazione, contrasto,
retract/reteach verificati parlando; `/save` ha instradato **55 clausole**.
`W/L/C/P/O/X = 3/1/0/6/45/0`. Processo nuovo: **3/3 fatti, 5/5 domande**,
`38583` fatti e `2781` regole. Nessuna suite eseguita.
[Report completo](docs/labs/apprendimento-assistito/2026-09-05-solfuri-di-rame.md).

**Il prossimo lavoro resta la famiglia dei conteggi/posizioni fisse**, non
un giro di test: il soggetto della spiegazione/provenienza e' ancora `w[2]` o
`w[4]`, anche dove si cerca gia' la copula per ruolo. Provare entita' reali
multi-parola e condividere il binding con il lettore; non aggiungere un parser
per il minerale. Restano italiano ibrido, arbitrato S4 e accesso al no da
«why is …» secondo l'ordine precedente. Non e' stata insegnata una nuova
regola di tassonomia: questi tre fatti non provano da soli l'arco verso
`copper mineral`.

Nota da non perdere: le regole di `auto_induce` sono `KB_INDUCED`, e `/save`
include quell'origine. Le sette viste nella diagnosi non erano tutte verita'
verificate; la sessione diagnostica non e' stata salvata e questo lotto di
fatti non le ha generate. Non trattare quel debito come solo un annuncio.

## Ripresa 2026-09-05 — il ramo ignoto e' identificato e corretto

Primo incremento dal gen505b: il blocco delle definizioni leggeva
`question_word`, quindi rivendicava anche «why is …». Ora consulta il ruolo
KB gia' esistente `definition_interrogative`: la domanda raggiunge la prova,
sia sui minerali sia su Socrate. Tolto anche il tetto `char *w[8]` di quel
tratto di `mod_knowledge`, dimensionando i token sull'intero buffer di testo.
La coda oltre l'ottava parola restava attaccata all'ultimo token: alcune chiavi
multi-parola funzionavano solo per caso.

Build e conversazioni con profilo AGI riuscite; crescita/retract/reteach del
ruolo verificati parlando. Nessuna suite e nessuna lezione diagnostica salvata.
Stato `meta-capability-only`, `W=0`.
[Report e transcript](docs/labs/apprendimento-assistito/2026-09-05-domande-e-token.md).

La missione e l'ordine sotto restano validi, con due aggiornamenti: il ramo
della voce 2 non e' piu' ignoto, e il tetto a otto token della voce 1 e' rimosso.
Restano i conteggi dei singoli rami e i soggetti legati a una posizione sola.

# ⛔ RIPARTI DA QUI — handoff 2026-09-05 (sera), `gen505b`

> **Questo handoff prevale su tutte le intestazioni piu' vecchie del file**,
> compresa quella del mattino qui sotto, che resta valida nel merito.

## La missione, in una riga

**Allargare le abilita' di COMPRENSIONE di parrot0 facendo crescere la KB, fino
alla KB viva.** Non «far passare i test», non «coprire piu' casi»: far si' che
cio' che parrot0 gia' sa diventi raggiungibile parlando, e che ogni lezione
detta a voce lasci una capacita', non un deposito.

## ⛔ Per adesso NON servono i test — indicazione esplicita di F.

I test sono **fuori posto e lo sappiamo**: si sistemano un altro giorno. In
particolare:

- il rosso di `make soft-test` e' **preesistente e di prestazione** — il primo
  turno del demone sfora il budget di 1.00s (misurato 1.17–1.46s, A/B con e
  senza le modifiche: differenza dentro il rumore). Blocca l'intera suite,
  perche' `test-engine` esce prima;
- `inference_guard.p0t` ha un gate **stantio**: si aspetta `Learned: zorp(vex).`
  mentre il motore ora conferma in lingua naturale — la resa e' migliore
  dell'atteso;
- non spendere tempo a inseguirli. Se una modifica rompe qualcosa **di
  comportamento**, si vede subito parlando: e' quella la verifica di questo
  periodo.

Quello che invece va sempre fatto: **provare a voce** cio' che si tocca, in una
sola sessione, e commettere piccolo.

## Come si lavora, in pratica

```bash
make build
printf '%s\n' "una lezione" "una domanda" | PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0
```

- ⚠ **`PARROT0_PROFILE=kb/profiles/agi.p0` non e' facoltativo**: senza, la KB
  appresa (`kb/learning/learned.p0`) non viene caricata e sembra che parrot0
  abbia dimenticato tutto. Ci sono cascato.
- ogni invocazione e' un **processo nuovo**: lezione e verifica vanno nella
  STESSA pipe, altrimenti si misura una KB diversa da quella che si e' insegnata;
- `/save` persiste, e la persistenza va verificata in un processo fresco;
- `/debug` PRIMA del turno da' la strada e il profilo; dopo, da' le sonde.

## Che cosa e' stato chiuso oggi (tutto verificato parlando)

| | capacita' aperta |
|---|---|
| 1 | **il «no» guadagnato si insegna a voce**: «no carbonate mineral is an iron oxide mineral» → `exclusive_classes/2`, che prima aveva **zero produttori**. E' la piu' fertile delle tre vie al no: una frase risponde per tutti i membri, presenti e futuri, di entrambe le classi |
| 2 | **una classe nominata solo da un vincolo si puo' nominare** (`class_constrained/1`), quindi il no guadagnato vale anche con l'altra classe vuota |
| 3 | **il soggetto coordinato distribuisce**: «A and B are Xs» imparava solo A e diceva di aver imparato. `plural_copula/1` distingue la lista dal composto |
| 4 | **enumerazione al plurale**: «what are the copper minerals?» aveva i membri e non la superficie |
| 5 | **l'universale non impara piu' una regola falsa** su una classe multi-parola (`mineral(X) :- copper(X), mineral(X)`), e legge la testa multi-parola |
| 6 | **parrot0 spiega un «no» guadagnato**, componendo la ragione dalla KB |
| 7 | **le prove si leggono**: `class_surface/2` — come si chiama una classe nella lingua, appresa dove la classe entra — e tre percorsi di resa che la chiedono |
| 8 | **la domanda di provenienza torna al suo proprietario**: «how do you know X is a Y?» dava un saggio sui meccanismi causali |
| 9 | **cio' che si sa di una CLASSE sono i suoi membri** («what do you know about copper mineral?» rispondeva con un saluto) |

E lo strumento nuovo: [`docs/plans/inferenza-compositiva.md`](docs/plans/inferenza-compositiva.md)
— una risposta lunga e' un albero di stadi, non una stringa con dei buchi.
`kb/core/composition.p0`, tre cipolle vive. **E' la leva giusta per la
comprensione**: uno stadio nuovo e' un fatto, e vale per ogni risposta che
dimostri quella tesi. Il riferimento rapido e' in testa a
`docs/plans/apprendimento-assistito.md`.

## Da dove ripartire, in ordine

1. **La famiglia del conteggio di parole fisso** — e' il filone piu' ricco
   trovato oggi, censito in testa a [`C_TODO.md`](C_TODO.md): **62 rami
   `nw == N`**, 28 nel modulo della conoscenza, e sotto di essi il tetto
   strutturale **`char *w[8]`**. Ogni istanza nasconde una capacita' che c'e'
   gia', e il danno non e' un muro: il turno viene raccolto da una facolta'
   peggiore che risponde comunque. *Un conteggio fisso non produce silenzio,
   produce rumore convinto.* La regola per toglierli e il criterio per quali NON
   toccare sono scritti li'.
2. **Il ramo non identificato** che rivendica «why is X a *classe
   multi-parola*?» e risponde col dump della descrizione: traccia completa in
   `C_TODO`, compreso il metodo che **non** funziona (bisezione con `fprintf`:
   gli scope annidati ricalcolano `nw`/`w`).
3. **L'italiano e' fermo**, e non per una capacita' mancante: la
   canonicalizzazione produce atomi ibridi («minerale of carbonato»,
   `C_TODO` §U4). Finche' e' cosi' **non si insegna in italiano** — si
   avvelenerebbe la KB. Sbloccarlo raddoppia la superficie insegnabile.
4. **Il turno rubato** («who is a man?» → la trama di *Invisible Man*): per
   [`docs/plans/turn-arbitration.md`](docs/plans/turn-arbitration.md) **non** si
   cura con una cessione — serve l'arbitrato per copertura (S4). ⚠ `/debug`
   propone da se' una «guardia di pertinenza», che e' proprio la forma vietata:
   le due indicazioni si contraddicono e la scelta e' di F.
5. **Debiti minori**: «Because X because Y» (doppio connettivo sul percorso
   `why?`), `class_surface` persistito senza virgolette (ricarica bene, e'
   fragile), l'annuncio di regole indotte su domini estranei al primo turno di
   sessione.

## ⭐ Aggiunto in coda alla sessione — l'induzione, e il «si'» non guadagnato

Il punto 3 dell'ex-`HANDOFF.md` e' **chiuso**: insegnata a voce la relazione
tassonomica («every copper sulfide mineral is a copper mineral», e la gemella
per i carbonati), i membri diventano raggiungibili per INFERENZA e non per
ripetizione. In processo fresco:

```text
what are the copper minerals?   → chalcopyrite, chalcocite, covellite, bornite,
                                   azurite, malachite      (quattro derivati)
why is azurite a copper mineral?
  → azurite is a copper mineral because azurite is a copper carbonate mineral.
```

Facendolo e' emerso cio' che l'ex-handoff aveva gia' fiutato, e in forma
peggiore del previsto.

### Chiuso: `/save` persisteva le induzioni non verificate

`kb_save_routed` instradava `KB_INDUCED` insieme a `KB_SESSION`. Misurato:
insegnando **una** regola sui minerali ne sono state scritte **otto** nella KB
ufficiale, fra cui `chess_rank(X) :- is_prime(X)`, `expert(X) :- language(X)` e
`humanities_topic(X) :- man(X)`. Non e' rumore: e' **falsita' persistita**. Le
otto righe sono state tolte da `kb/learning/learned.p0` e il filtro ora salva
solo `KB_SESSION`. Un'induzione resta viva nel turno e nella sessione — si puo'
interrogare, criticare e **promuovere**: quella che qualcuno verifica si
riasserisce come conoscenza, e allora si salva.

### ✅ CHIUSO (gen505d): un'induzione non decide piu' una polare

⚠ **Correzione a questa stessa nota:** l'esempio che avevo usato — «is socrates
a humanities topic? → Yes.» — **era sbagliato**. `humanities_topic(socrates)` e'
un fatto CURATO (`kb/experts/philosophy/history.p0:41`), quindi quel «si'» era
sempre stato legittimo. Il fenomeno pero' era reale, e la prova stava
altrove: insegnati violino e violoncello come strumenti a corda E ad arco,
l'induzione asseriva **entrambe** le direzioni e la KB si ritrovava un CICLO —
«is guitar a bowed instrument?» rispondeva *«non posso stabilirlo: le regole
intorno a bowed_instrument si rimandano a vicenda»*. Non un «si'» inventato, ma
una KB avvelenata da regole che nessuno aveva insegnato.

**Come e' stato chiuso** (F.: *«mi piace tantissimo, vai»*): `kb_induce` non
asserisce piu' — deposita `induced_candidate(Testa, Corpo, Supporto)`, un fatto
su cui si puo' ragionare come `machinery_gap` e `saturated_read`. E i due siti
che annunciavano regole ora **chiedono**:

```text
> violin is a string instrument     > cello is a string instrument
> violin is a bowed instrument      > cello is a bowed instrument
> generalize
  every string instrument I hold is also a bowed instrument — does that always hold?
> guitar is a string instrument
> generalize
  every bowed instrument I hold is also a string instrument — does that always hold?
```

Tre proprieta', tutte misurate:

- **la pertinenza batte il supporto**: si chiede prima di un candidato che poggia
  su un fatto detto in QUESTA sessione. Senza, dopo una lezione sugli strumenti
  parrot0 chiedeva di `philosopher`/`humanities_topic` — supporto piu' alto e in
  KB da sempre, cioe' una domanda fuori dal discorso;
- **un candidato e' una lettura, non un deposito**: si ricalcola a ogni
  induzione. Conservato come fatto sopravviveva al controesempio che lo
  smentisce, e parrot0 continuava a chiedere una cosa a cui la KB aveva gia'
  risposto da sola;
- **il ciclo si chiude parlando**: il controesempio elimina la direzione falsa e
  la domanda **si sposta su quella vera**, che a quel punto si guadagna con una
  riga di lezione.

## ⭐ `kb_induce` messo alla prova su cinque campi — e la risposta alla domanda di F.

> F.: *«le due non si devono escludere a vicenda — proporre invece di
> concludere, o alzare il pavimento. Cambiamo campo, facciamo diverse
> iterazioni, e partiamo dalla domanda principale: ci serve davvero
> `kb_induce`, o e' stata una forzatura logica e i suoi risultati si potevano
> raggiungere in altro modo?»*

Esperimento, non ragionamento: quattro fatti (o sei) in un campo, poi
`generalize`, e si guarda **che cosa** viene indotto.

| campo | supporto | direzione A | direzione B |
|---|---:|---|---|
| strumenti musicali | 2 | `bowed_instrument :- string_instrument` ❌ *(la chitarra)* | `string_instrument :- bowed_instrument` ✅ |
| geometria | 2 | `parallelogram :- quadrilateral` ❌ *(il trapezio)* | `quadrilateral :- parallelogram` ✅ |
| uccelli e volo | 2 | `flier :- bird` ❌ *(il pinguino)* | `bird :- flier` ❌ *(il pipistrello)* |
| mammiferi | 3 | `animal :- mammal` ✅ | `mammal :- animal` ❌ |
| gas nobili | 3 | `inert_gas :- noble_gas` ✅ | `noble_gas :- inert_gas` ✅ |

### La legge, ed e' una sola

**L'induzione produce SEMPRE tutte e due le direzioni, con supporto identico**, e
la verita' cade in tutte e quattro le combinazioni possibili: una vera, nessuna
vera, entrambe vere. **Il supporto non porta nessuna informazione su quale.** Il
campo D lo dimostra da solo: supporto 3 invece di 2, e la direzione sbagliata
resta esattamente altrettanto sostenuta.

### Le due cure non sono alternative, e non sono pari

F. ha ragione che non si escludono, e l'esperimento dice **perche'**:

- **alzare il pavimento** cambia *quanto spesso* l'induzione sbaglia. Non tocca
  la direzione: con venti uccelli che volano, `flier :- bird` ha supporto venti
  ed e' falsa uguale;
- **proporre invece di concludere** cambia *che cosa* l'induzione afferma — da
  una conclusione a una domanda.

Quindi: **proporre e' la cura, il pavimento e' l'ordinamento.** Insieme:
si inducono i candidati, si ordinano per supporto, si CHIEDE del migliore.

### La risposta alla domanda principale

**Come produttore di conoscenza, `kb_induce` non serve** — ed e' dimostrato, non
opinato: non puo' azzeccare la direzione se non per fortuna, e i suoi risultati
si ottengono meglio in una riga di lezione («every mammal is an animal»), che e'
sempre corretta e non va ripulita dopo.

**Come generatore di DOMANDE serve, ed e' prezioso.** Trova esattamente le coppie
di classi la cui relazione la KB **non ha stabilito** — cioe' lo spazio negativo
di [`question-emergence.md`](docs/plans/question-emergence.md), calcolato a costo
quasi zero dai soli fatti. E la domanda che ne nasce si ripaga in entrambi i
versi:

```text
«ogni strumento ad arco che conosco e' anche a corda: vale sempre?»
  → «si'»  → una regola guadagnata, che l'induzione non poteva guadagnare
  → «no»   → la DIREZIONE, o un'esclusione (`exclusive_classes`, gen505)
```

Il «no» vale quanto il «si'»: e' la stessa dottrina del «no» guadagnato chiusa
oggi. Un'induzione che chiede non e' piu' una forzatura logica — e' il modo in
cui la KB si accorge di cio' che le manca.

### Che cosa resta da fare, in ordine

1. `kb_induce` **non asserisce piu'**: deposita `induced_candidate(Testa, Corpo,
   Supporto)` — un fatto su cui si puo' ragionare, come `machinery_gap` e
   `saturated_read`;
2. la domanda si compone dal candidato migliore (e' una cipolla:
   [`inferenza-compositiva.md`](docs/plans/inferenza-compositiva.md));
3. la risposta e' una lezione ordinaria e passa dalle vie gia' aperte;
4. finche' (1) non c'e', l'induzione **non deve decidere una polare**: e' il
   «si'» non guadagnato registrato piu' sopra.

## ⛔ La relazione che non si puo' insegnare — campi esotici, `gen505d`

Su indicazione di F. («le capitali sono uno stereotipo, andiamo su campi piu'
esotici») ho spostato le sonde su **nodi**, **tessitura**, **tintura** e
**fermentazione**. Le classi e le regole tassonomiche reggono in pieno:

```text
> the clove hitch is a hitch knot        Learned: clove hitch is a hitch knot.
> every twill fabric is a woven fabric   Learned rule: woven_fabric(X) :- twill_fabric(X).
> why is denim a woven fabric?           denim is a woven fabric because denim is a twill fabric.
```

Le **relazioni binarie** no, e il reperto e' preciso.

### La macchineria e' gia' giusta, e questo rende il buco piu' netto

`kb/core/grammar.p0` ha **una regola** che, per ogni relazione che dichiara il
proprio nome comune (`relation_noun/2`), costruisce da sola il pattern «the X of
@S is @O». Una relazione nuova costa **un fatto**. Misurato: scritto a mano
`relation_noun(warp_of, "warp")`, la frase «the warp of denim is cotton» viene
capita e l'eco e' corretta.

### Ma quel fatto si puo' solo SCRIVERE

- la lista e' chiusa a cinque (`capital`, `population`, `currency`, `language`,
  `author`) e chi insegna parlando non puo' aggiungerne una sesta;
- il muro **suggerisce una lezione che non funziona**: «if warp is something one
  thing does to another, say "warp is a relation verb"» → `Learned: warp is a
  relation verb.` e la capacita' **non cambia**. E' un misclaim (priorita' 2 di
  §12): parrot0 dichiara di aver imparato e non ha imparato;
- ho provato a fare il ponte in KB — «warp is a relation» asserisce
  `relation(warp)`, e una regola ne deriva `relation_noun(warp_of, warp)`.
  **La derivazione funziona** (verificato: `relation_noun(warp_of, ?)` → `warp`)
  **ma `extract_frame` non rende il pattern lo stesso**, ne' col ponte a due
  livelli ne' con una clausola diretta. Le sonde minime dicono che ogni PEZZO
  funziona in isolamento (`relation($N)` ✓, i tre `concat_atoms` ✓, il pattern
  costruito ✓): non regge solo **dentro** `extract_frame`, che ha **371
  clausole**. Sospetto forte ma non dimostrato: la saturazione della lettura
  gia' descritta in `C_TODO` §U2.

### E la domanda e' un terzo buco

Anche col fatto scritto a mano, «what is the warp of denim?» risponde «I don't
know about warp»: il lato **domanda** non passa da `relation_noun`. Assertare e
interrogare non condividono l'oggetto — e' la stessa figura del §5 (V2), qui su
una relazione invece che su un referente.

### Come riprendere, senza rifare la strada

1. capire perche' una clausola in coda a `extract_frame` non contribuisce
   (sonde gia' fatte: i pezzi funzionano da soli; guardare la saturazione prima
   di sospettare la logica);
2. dare al lato **domanda** lo stesso `relation_noun`;
3. solo allora la lezione «warp is a relation» chiude il ciclo, e il muro va
   riscritto perche' suggerisca QUELLA e non una che non registra niente.

⚠ E un difetto indipendente, visto sulle capitali e valido ovunque: «the capital
of Portugal is Lisbon» conferma «**Learned: portugal is the capital of lisbon**»
— il fatto e' giusto, l'eco dice l'inverso. Chi rileggesse la conferma
imparerebbe il contrario (e l'audit dell'eco esiste apposta, gen491). Col
`relation_noun` scritto a mano per «warp» l'eco era invece corretta: e' il
frame di resa di `capital_of` a essere invertito, non la meccanica.

## La regola che riassume il periodo

> Prima di scrivere una riga: **la capacita' esiste gia' e non si riesce a
> raggiungerla parlando?** In sette casi su nove, oggi, la risposta era si'. Il
> lavoro non e' aggiungere facolta': e' togliere i conteggi, le posizioni fisse
> e le chiavi interne che stanno fra una capacita' e la lingua.

---


# ⬇ CONFLUITO DA `HANDOFF.md` (2026-09-05) — il file e' stato eliminato

> Stava in un file a parte e si perdeva: un handoff che non e' nella coda dei
> temi non viene letto da chi riprende la coda dei temi. Il contenuto e' qui
> **integrale**, sotto l'handoff attivo che lo precede e lo aggiorna.

## Chiusura del 2026-09-05 (ex `HANDOFF.md`)

**Questa nota prevale sulle priorita' storiche riportate sotto.** La missione
attiva e' quella in testa a `LEARN_TODO.md`: allargare la comprensione facendo
crescere la KB. Prima di modificare: `MANTRA.md`, `PRINCIPLES.md` e l'handoff
di `LEARN_TODO.md`. Per adesso niente suite: build e conversazioni reali con
`PARROT0_PROFILE=kb/profiles/agi.p0`.

## Risultato consolidato

- **`6276cb2`, pubblicato:** il ramo delle definizioni accettava ogni
  `question_word` e rubava «why is …» alla prova. Ora usa la classe KB
  `definition_interrogative`, gia' esistente. Tolto il tetto `char *w[8]` di
  quel tratto di `mod_knowledge`: il vettore copre ogni token del buffer.
  Crescita/retract/reteach del ruolo verificati con menzione naturale.
- **Training successivo:** bornite, chalcocite e covellite apprese come
  solfuri di rame, dopo verifica delle tre schede del *Handbook of Mineralogy*.
  Nessun fatto scritto a mano. `/save`: 55 clausole, `W=3 L=1 C=0 P=6 O=45 X=0`.
  Processo nuovo: 38583 fatti, 2781 regole; 5/5 risposte corrette, 3/3 fatti
  richiamati senza reinsegnamento. Retract di bornite toglieva prova e membro
  dall'elenco conservando gli altri; reteach eseguito prima del salvataggio.

Report: [correzione del motore](docs/labs/apprendimento-assistito/2026-09-05-domande-e-token.md)
e [training persistito](docs/labs/apprendimento-assistito/2026-09-05-solfuri-di-rame.md).
Le sessioni sono chiuse. Nessun servizio avviato da questo lavoro resta da gestire.

## Dove riprendere

1. I singoli rami con conteggi e posizioni fisse: nelle spiegazioni e nella
   provenienza il soggetto resta un token, nonostante la ricerca della copula
   per ruolo. Prima provare con un nome reale multi-parola, poi riusare il
   binding del lettore. Il ramo gen151 prima «ignoto» e' chiuso.
2. L'italiano ibrido e l'arbitrato S4 restano aperti come in `LEARN_TODO.md`;
   non insegnare in italiano finche' le chiavi si avvelenano.
3. Non confondere fatti e tassonomia: i tre nuovi membri sono raggiungibili,
   ma non e' stata insegnata la relazione verso la classe `copper mineral`.

**Lezioni da capitalizzare:** il tokenizer al tetto non eliminava soltanto la
coda: la lasciava dentro l'ultimo token, producendo chiavi miste. E
`auto_induce` non e' soltanto una frase fuori tema: scrive regole
`KB_INDUCED`, incluse da `/save`. Le induzioni diagnostiche non verificate
non sono state salvate; il lotto reale non le ha generate. Non cancellare
tracce per mascherare il problema.

---

## Storico — `gen503`, 2026-09-05

Sessione lunga. Qui c'è **dove siamo**, **che cosa è vero e misurato**, e **da
dove si riprende**. Chi riprende legga questo file per intero prima di toccare
qualsiasi cosa: metà delle scoperte di oggi sono *correzioni di cose che i file
davano per vere*.

---

## 1. In una riga

Il ciclo di un coding agent **si chiude**: su un progetto rotto vero, in un
turno, parrot0 esegue il build, legge cosa manca, legge il contratto dalla
**prosa** di un header, sceglie una forma, la emette, **scrive il file**,
rilegge e ricompila — e il `make` passa con `-Werror` e l'eseguibile esce 0.

Cricchetto: `tests/p0t/code/build_repair_cycle.p0t` — 15 passed, col compilatore
vero dentro.

⚠ **Questo NON è la parità con freebuff**, e chiamarla così sarebbe l'inganno che
`PRINCIPLES.md` vieta. Il punteggio del banco non si è ancora mosso: su match0
nessuna forma dichiarata soddisfa il contratto di `strjoin.h` (manca
`empty_on_zero`), e dichiararne una che combaci sarebbe scrivere la risposta del
banco in KB. Vedi `CHALLENGE_TODO.md` §6.5-bis.

---

## 2. La forma comune di quasi tutti i difetti trovati oggi

**Osservazioni buttate via**, o **tetti fissi su liste che crescono**. Non erano
capacità mancanti: era roba già prodotta e poi persa.

| difetto | sintomo |
|---|---|
| uno strumento dichiarato scartato da una catena di `strcmp` | «non so scrivere un file» |
| `ident(...);` fuori da una funzione ignorato dallo scanner | un header usciva con **zero nodi** |
| il verdetto di un build stampato e dimenticato | `run make` nominava il file mancante, e un turno dopo non si sapeva più |
| i commenti cancellati prima dello scanner | il contratto non si poteva leggere |
| impronta a 128 nomi, `mod_toolplan` a 1024 byte, `learnable` a 96 righe | **troncamenti silenziosi**, e i tre si spegnevano proprio sui turni per cui esistono |
| `kb_root_prefix` che teneva in cache «la cwd va già bene» | caricamento pigro rotto **solo dentro una sandbox**, cioè solo nei test |

⭐ **Regola operativa che ne esce, ed è la cosa più utile del giorno:** *prima di
costruire una capacità, guardare se il dato che le serve viene già prodotto e
scartato.* È così che A3 si è chiusa senza scrivere un parser di Makefile.

---

## 3. Che cosa è stato costruito (tutto con cricchetto e ablazione)

```text
scrivere un file        write_tool.p0t                    4
A1 dichiarazioni        header_declarations.p0t           4
A3 obbligo dal build    tool_result_becomes_knowledge.p0t 3
A2 contratto dalla prosa header_contract.p0t              5
A6 il turno arriva      repair_broken_build.p0t          12
A5 emettitore KB        kb_code_emitter.p0t              18
M4 forma dal contratto  contract_selects_shape.p0t       13
il ciclo                build_repair_cycle.p0t           15
superfici insegnate     taught_tool_surface.p0t          12
aree logiche            crossing/ (4 file)               18
```

Piani vivi: `docs/plans/kb-code-emitter.md` (M1–M5 ✅), `CHALLENGE_TODO.md` §6.

---

## 4. ⛔ LA VOCE PIÙ IMPORTANTE APERTA — A8

**F., 2026-09-05:** *«non esiste assolutamente questa rottura del paradigma
KB-first. Gli operatori devono essere apprendibili a runtime. Anche gli
operatori devono essere KB. Presto dovrà essere possibile addestrare a runtime
un TOOL nuovo.»*

Lo **schema** di un piano è conoscenza; i **verbi** che può usare sono `else if`
in `src/brain/60-agent-tools.c`. È il mantra #19 un piano più su.

```text
rami compilati:  5 → 4      (`run_build` è diventato una frase)
passi dello schema repair_broken_build: 5 operatori → 2
```

`plan_utterance/3` è la strada e **funziona già**: un passo è una frase che
parrot0 dice a se stesso e rientra a digerire, e ciò che quel turno lascia in KB
è la connessione col passo dopo. Due passi (`list_sources`+`read_each`) sono
diventati una frase sola — *«analyze all the sources»* — che invoca uno schema
che parrot0 aveva già.

Dettaglio, misura e ordine di lavoro: `C_TODO.md` §«gli operatori di un piano
sono compilati», `CHALLENGE_TODO.md` §6.2 voce A8, e il `TODO(kb-first)` sopra
la catena stessa.

⛔ **Il confine vero, da disegnare prima di scrivere:** `local_tool/3` +
`tool_argv/2` sono già conoscenza, ma il loro argv finisce in `p0_exec` — cioè
**l'unica azione insegnabile oggi è «esegui un programma con i suoi
argomenti»**. Scrivere, ingerire, ritrattare non hanno nessuna forma
dichiarativa. Finché quel confine non è disegnato, migrare gli operatori sposta
il problema invece di chiuderlo.

---

## 5. ⬅ DA DOVE SI RIPRENDE, in ordine

### R1. Le forme di insegnamento che non funzionano — **lasciata a metà, di proposito**

**F.:** *«le forme di insegnamento che non funzionano vanno comunque sistemate,
perché massimizzare la comprensione e la metacomprensione è pure un obiettivo di
addestramento».*

Misurato: delle sei forme naturali con cui una persona insegna una parafrasi,
**una** funziona.

```text
✅ learn "X" as another way to say "Y"
❌ "X" means "Y"                              → la lezione di COSTRUZIONE la reclama
❌ "X" is another way to say "Y"
❌ saying "X" is like saying "Y"
❌ when I say "X" I mean "Y"
❌ "X" is the same as "Y"
```

**Tentativo fatto e RITIRATO oggi**, e va saputo perché: le quattro forme senza
«means» sono state aggiunte come righe di KB (cue di guardia in
`00_lex_chain332` + righe `learnable` con modo `cue_like`) e **funzionavano
tutte e quattro**. Ma `taught_cue_ladder.p0t` è passato da 23 a 15/8. Non ho
avuto il tempo di isolare quale riga rompe cosa — il sospetto è `" i mean "`,
che è una sottostringa frequentissima — e ho preferito ritirare tutto piuttosto
che lasciare un rosso.

**Come riprendere:** riaggiungere **una riga alla volta**, con
`taught_cue_ladder.p0t` come cricchetto dopo ognuna. Il diff ritirato è in
questo commit (`f8c5280`) nel messaggio, e le righe erano:

```prolog
intent_cue(00_lex_chain332, " is another way to say ").
intent_cue(00_lex_chain332, " is the same as ").
intent_cue(00_lex_chain332, " is like saying ").
intent_cue(00_lex_chain332, " i mean ").          % ← il sospetto
learnable("is another way to say", intent_cue, cue_like).
learnable("is the same as",        intent_cue, cue_like).
learnable("is like saying",        intent_cue, cue_like).
learnable("i mean",                intent_cue, cue_like).
```

Per `"X" means "Y"` la nota che serve: ha già un proprietario legittimo — la
lezione di **costruzione** (`x y means x <verbo> y`). Le due forme si
distinguono per **struttura**, non per parola: la parafrasi ha entrambi i lati
fra virgolette, la costruzione no, e `try_teach_form` esce subito senza due
stringhe citate. La convivenza è possibile, va solo verificata.

### R2. A8 — gli operatori diventano KB

L'ordine è in `C_TODO.md`. Il primo passo **non è scrivere C**: è insegnare le
superfici che mancano, ora che si può farlo parlando (§3, `cue_like`).

### R3. A6 — la fragilità dell'arbitrato, causa già nominata

`kb_cue_match` **non è un test di sottostringa**: passa da `kb_hypothesis_best`,
cioè da un punteggio di evidenza, e la cue dichiarata di un piano può **perdere**
dentro un turno che porta altra evidenza. Misurato, e non monotono:

```text
the project does not build: file            → lo schema parte
the project does not build: missing         → lo schema parte
the project does not build: file missing    → NON parte
```

⚠ È esattamente la classe di turni del banco. Che il prompt di match0 funzioni
è, con questa spiegazione, **una fortuna e non una garanzia**. Dettagli in
`CHALLENGE_TODO.md` §6.7.
⛔ Il rimedio NON è aggiungere cue finché la frase passa: sarebbe la colonna
destra di §6.5-bis. Il lavoro è sul meccanismo.

### R4. I rossi che restano, e sono di prima

```text
toolexec.p0t          19/3   (uno è marcato «ROSSO da prima» nel file stesso)
agentcommit.p0t       11/16
codeast.p0t           15/5
taught_lexicon.p0t    18/17
taught_gloss.p0t      13/2
frontier_chat_audit   53/3   (soft-test è rosso su questi tre)
```

Tutti **verificati con `git stash` come identici a prima** delle modifiche di
oggi. `check_sort.p0t` era 0/5 e ora è 5/5: non era rotto il giudice, era il
test che usava `<~` (sorgente *turn*) su un'uscita *mcp*.

---

## 6. Strumenti nuovi che chi riprende deve conoscere

- **`!cwd DIR` / `!cwd off`** nei `.p0t`: sposta la directory in cui parrot0
  lavora. Serve ai casi che **sono** una proprietà della directory (un progetto
  che non compila). ⚠ E previene un danno reale: `!exec` gira nella radice del
  repository, e un `.p0t` che scriveva e poi cancellava `Makefile` credendo di
  essere in una sandbox **ha cancellato il Makefile di questo repository**.
  Documentato in `TEST_TODO.md` §I.
- **`code.synth_shape`** su MCP, accanto a `code.check_sort`: emette una forma
  dichiarata. Comporre e giudicare restano due atti separati.
- **`make repair-check`** non esiste più: quel caso è diventato un `.p0t` vero
  grazie a `!cwd`.

---

## 7. Le regole di condotta fissate oggi, da non perdere

1. **La conoscenza operativa in KB è una conquista** (F.), e il confine è: *«serve
   anche a un compito che non è nel banco?»*. Tabella in `CHALLENGE_TODO.md`
   §6.5-bis, con il corollario sui cricchetti — un `.p0t` che asserisce le
   clausole invece di leggerle è barare travestito da verde.
2. **Un cricchetto senza ablazione non è un cricchetto.** Tutti quelli scritti
   oggi hanno la loro: prima dell'azione, la domanda non deve sapere.
3. **Codice non provato si toglie**, anche quando esprime un principio giusto.
   Oggi è successo una volta (la regola «un turno che dichiara un piano non si
   segmenta»: non serviva, il turno non era segmentato).
4. **Un test si aggiorna, non si forza.** `check_sort` era stantio, non rotto.

---

## Appendice — seconda meta' della sessione `gen503`

## A. Le forme di insegnamento: bisezione fatta, colpevole trovato

`HANDOFF §R1` diceva di riaggiungerle **una alla volta**. Fatto, ed è servito:

```text
"X" is another way to say "Y"      ✅ ladder 23, tool 16   → TENUTA
"X" is the same as "Y"             ✅ ladder 23, tool 16   → TENUTA
saying "X" is like saying "Y"      ⛔ ladder 15/8          → RIFIUTATA
```

⚠ **Il sospetto scritto ieri era sbagliato, e va detto:** non è `" i mean "`, è
**`is like saying`**. Non ho ancora capito *perché* faccia cadere il retract di
`taught_cue_ladder` (il sintomo è che `forget "…" as a casual opener` smette di
essere riconosciuto e finisce a muro). **Questa è la voce aperta**, ora col nome
giusto invece che col sospetto.

Restano da provare, sempre una alla volta:

```prolog
intent_cue(00_lex_chain332, " i mean ").      learnable("i mean", intent_cue, cue_like).
intent_cue(00_lex_chain332, " means ").       learnable("means",  intent_cue, cue_like).
```

Per `means` la nota che serve: ha già un proprietario legittimo — la lezione di
**costruzione** (`x y means x <verbo> y`). Si distinguono per **struttura**: la
parafrasi ha entrambi i lati fra virgolette, la costruzione no, e
`try_teach_form` esce subito senza due stringhe citate. La convivenza è
possibile, va verificata.

Il cricchetto `taught_tool_surface.p0t` è ora 16 e copre entrambe le forme nuove,
la seconda su un file **mai nominato nella lezione** — una parafrasi di strumento
deve generalizzare, non essere una frase sola.

## B. Il banco di sonde di F. — `var/probe/`

Dieci famiglie di prompt lunghi, un generatore di item `.p0t`, un runner che
avvia un demone fresco per famiglia e registra **latenza e risposta verbatim**.
È lo strumento giusto per misurare la comprensione dove finora si guardava a
occhio. Committato: harness, generatore e item (il pannello congelato). Le
misure no — si rigenerano, e `results/` è gitignorato.

⛔ **Non misura ancora niente.** I 16 item di `f01` danno tutti `rc=2`, e i
3,06 s per item sono esattamente il budget di retry del client
(300 × 10 ms) → *«test-send: cannot reach engine»*. **A mano funziona:** avviato
il demone con lo stesso `--sock` e mandato `i07.p0t`, la risposta arriva
(`«Fair enough — tell me where I went wrong…»`). Quindi non è il trasporto in sé:
è qualcosa nel modo in cui `probe_one.py` avvia o attende il demone. Indagine
interrotta qui su richiesta di F.

⚠ E una nota sul disegno degli item: usano `< __NEVER__` come sentinella per
farsi stampare la risposta. Funziona, ma significa che **ogni item è rosso per
costruzione** e il codice di uscita del client non distingue «il test è fallito
come previsto» da «non ho raggiunto il demone». Vale la pena separarli, o
leggere la risposta da un canale che non passi dal verdetto.

## C. Misurare il lato di parrot0 da solo — bloccato, e si sa da cosa

Il piano (§6.5) dice che ogni gradino si misura sul banco, e dopo A5 il punteggio
dovrebbe potersi muovere. Ho provato a costruire una league col solo parrot0
(`--league` esiste apposta) e mi sono fermato su un limite reale del pilota:

1. pretende **esattamente due agenti** — aggirabile con un no-op dichiarato
   (`fake_agent.py --mode silent`), e l'avevo fatto;
2. ma poi `order` sta in **`tasks/matchN/match.json`**, non nella league, e
   nomina `parrot0` e `freebuff`. Per correre da soli servirebbe **un override
   di `order` nella league**, che oggi non esiste.

È un pezzo piccolo e nominato: *permettere alla league di sovrascrivere l'ordine
di un match*. Senza, ogni misura del nostro lato richiede la presenza (e il
modello concordato) di freebuff — cioè resta bloccata dietro C0.

**Il file `league-parrot0.json` NON è stato lasciato nel repo**: era a metà, e un
artefatto a metà nel banco è peggio della sua assenza.

## D. Da dove si riprende, aggiornato

```text
R1  finire le forme di insegnamento — colpevole noto: «is like saying»
R2  A8: gli operatori diventano KB (C_TODO.md), partendo dalle superfici
R3  A6: la fragilità di kb_cue_match (CHALLENGE_TODO §6.7)
R5  far misurare la sonda di F. (§B) — a mano funziona, dal runner no
R6  override di `order` nella league, per misurare un lato solo (§C)
```

---


# ⛔ RIPARTI DA QUI — handoff 2026-09-05: addestrare la KB viva

> **Invito esplicito per chi riprende:** continua questa missione. Non tornare a
> trattare `LEARN_TODO.md` come un catalogo passivo e non aprire un fronte
> laterale soltanto perché è più facile da misurare. Il compito principale è
> far crescere parrot0, attraverso lezioni naturali vere, fino a una **KB viva**:
> conoscenza, lingua, inferenza, memoria, spiegazione, pianificazione e condotta
> devono raggiungersi a vicenda e sostenere un dialogo alla pari sul perimetro
> verificato, senza muri ciechi, risposte fuori tema o certezze inventate.
>
> **Direzione esplicita di F.:** per ora lascia perdere il lavoro
> sull'osservabilità della sonda. Conserva le evidenze verbatim quando insegni,
> ma investi il prossimo incremento nella crescita reale della KB e nei colli
> linguistico-inferenziali che quella crescita incontra.

Questo handoff prevale come punto di ripresa sulle intestazioni più vecchie del
file. La storia sotto resta utile: non cancellarla e non ripartire da zero.

## 1. La missione, senza scorciatoie

Il piano autoritativo è
[`docs/plans/apprendimento-assistito.md`](docs/plans/apprendimento-assistito.md),
§0. I due piani
[`frontier-kb-natural-dialogue.md`](docs/plans/frontier-kb-natural-dialogue.md)
e [`universal-comprehension.md`](docs/plans/universal-comprehension.md)
specificano rispettivamente il bersaglio dialogico e il contratto di
comprensione. `MANTRA.md`, `PRINCIPLES.md` e `LEARN_PROTOCOL.md` sono vincoli,
non letture facoltative.

L'equivalenza cercata con un LLM è **funzionale e osservabile**, non
architetturale. Parrot0 non deve imitare un modello neurale: deve riuscire a
seguire il turno completo, mantenere alternative e referenti, ricavare la
questione da soddisfare, usare tutte le proprie facoltà, formulare una risposta
fondata e continuare il dialogo. La sua differenza positiva deve restare la
tracciabilità: ogni conclusione ha sostegni ispezionabili e perde validità
quando quei sostegni vengono ritirati.

Due espressioni non vanno banalizzate:

- **senza muri** non significa rispondere sempre. Significa che una lacuna viene
  distinta da una negazione, nominata e trasformata nel prossimo bisogno utile;
  se manca una coordinata, parrot0 conserva il resto della lettura e chiede o
  acquisisce proprio quella coordinata;
- **senza allucinazioni o svarioni** significa che “sì”, “no”, “ho capito”, una
  spiegazione e perfino una domanda di chiarimento richiedono evidenza pertinente
  e corrente. Una frase ben formata ma irrilevante non è meglio di un muro.

Il ciclo da rendere vivo è questo:

```text
turno e storia
  → letture concorrenti con ruoli, referenti, scope e residui
  → questione / mossa / obblighi ancora aperti
  → inferenza, osservazione o procedura con sostegni correnti
  → piano di risposta che copre tutte le richieste
  → realizzazione nella lingua e nel registro del dialogo
  → seguito, correzione o nuova lezione

arresto informato
  → bisogno tipato
  → spiegazione naturale candidata
  → replay, transfer, contrasto, retract e reteach
  → revisione delle letture passate
  → persistenza e uso in un processo nuovo
```

Un fatto presente ma non interrogabile non è ancora vivo. Una regola presente
ma ignorata dai consumer non è una capacità. Una conferma di apprendimento che
non sa ridire ciò che è cambiato non è metacomprensione.

## 2. Stato esatto lasciato da questo turno

Il riordino dei tre piani è già stato pubblicato nel commit `584f090`
(`promuovi la KB viva a missione principale`). Il primo addestramento reale
successivo è sul micro-dominio **minerali di ferro**.

La sessione promossa ha acquisito parlando quattro fatti ground veri:

```text
hematite is an iron oxide mineral
magnetite is an iron oxide mineral
hematite is an iron ore mineral
magnetite is an iron ore mineral
```

Le fonti preparate prima della lezione sono istituzionali:

- [USGS, Iron Ore Statistics and Information](https://www.usgs.gov/centers/national-minerals-information-center/iron-ore-statistics-and-information):
  il minerale di ferro consiste quasi sempre di ossidi di ferro, le cui forme
  primarie sono magnetite ed ematite;
- [USGS, Hematite](https://pubs.usgs.gov/sir/2017/5118/elements/Hematite/Hmtt_txt.html):
  l'ematite è un minerale ossido di ferro e viene estratta come minerale di ferro
  insieme alla magnetite;
- [USGS, Quartz](https://pubs.usgs.gov/sir/2017/5118/elements/Quartz/Qtz_txt.html):
  il quarzo è SiO2; è servito come contrasto reale rispetto alla classe degli
  ossidi di ferro.

Stato quantitativo del checkpoint:

| misura | valore | significato |
|---|---:|---|
| `B0 / R0` | `38266 / 2732` | stato prima dell'addestramento |
| `W` | `4` | nuovi fatti veri del mondo |
| `L` | `77` | `verb_stem` collaterali materializzati dal salvataggio, non quattro nuove lezioni linguistiche |
| `C` | `0` | nessuna nuova costruzione o regola generale rivendicata |
| `P` | `8` | quattro `fact_source` e quattro `reading_fact` |
| `O` | `30` | 28 turni persistiti e due referenti discorsivi |
| `X` | `0` | nessun fatto falso o fixture nel diff promosso |
| clausole instradate | `119` | dato del salvataggio, non guadagno semantico |
| `B1 / R1` | `38308 / 2732` | processo nuovo dopo il salvataggio |
| richiamo fresco | `4/4` | i quattro fatti rispondono correttamente senza reinsegnamento |

I quattro fatti sono in `kb/learning/learned.p0`; provenienza e letture sono in
`kb/machinery/fact-provenance.p0`; i turni sono in
`kb/machinery/transcripts.p0`. La ricaduta in `learned.p0` e la
materializzazione di 77 stem estranei al micro-dominio sono un **gap di
consolidamento**, non un motivo per riscrivere a mano le lezioni in un file più
elegante. Il salvataggio ha conservato la causa reale; il router deve imparare
ad assegnare una casa semantica senza dipendere dai nomi “hematite” o
“magnetite”.

Non confondere `119` con 119 cose imparate. Il guadagno del mondo è quattro.
`B1-B0=42` è diagnostica globale e include materiale collaterale; non sostituisce
`W`.

## 2-bis. Lotto `gen505` — la siderite, e la TERZA via al «no» (2026-09-05)

Lezione reale in corsia sicura (Passo A), scelta perche' mette alla prova
proprio il §4: **la siderite e' minerale di ferro ma e' un carbonato, non un
ossido**. Basi: FeCO₃ e' un carbonato per composizione, ed e' fra i minerali da
cui si estrae ferro (stessa serie USGS gia' citata al §2). ⚠ Le URL non sono
state riaperte in questa sessione: la verifica di fonte va rifatta prima di
trattare il lotto come definitivo.

**Baseline prima di ogni lezione** (regola 2 del Passo A), tutte e quattro:
`siderite` ignota in tutte e tre le classi.

**Il §4 regge in scenario reale.** Con `iron_oxide_mineral` gia' popolata da
ematite e magnetite, la domanda su un membro nuovo **non** riceve un «No»
inventato:

```text
is siderite an iron oxide mineral?
  → I don't know: nothing I hold says siderite is an iron oxide mineral, and
    nothing says it isn't. Knowing some iron oxide minerals does not tell me
    they are all of them, so not proved is not the same as false.
```

**Il collo che la crescita ha incontrato, ed e' il risultato del turno.** Per
GUADAGNARE quel «no» bisogna poter dire che un carbonato non e' un ossido. Quella
frase non arrivava da nessuna parte: `exclusive_classes/2` — la vista costruita
dal `gen504` — aveva **zero produttori in tutta la KB**. Un cassetto senza
maniglia: la macchineria c'era e nessuna lezione poteva riempirla.

La maniglia e' la stessa forma strutturale dell'universale affermativo
(quantificatore, corpo, copula, articolo, testa) con la **polarita'** cambiata, e
quindi con un'altra conclusione: non una regola di Horn ma un vincolo fra classi.
`exclusive_quantifier/1` sta in `grammar.p0` (`no`, `nessun`, `nessuna`,
`nessuno`): il C non conosce nessuna di quelle parole, e una lingua nuova e' un
fatto. Copula e articolo insieme trovano il confine fra le due classi, quindi
entrambe possono essere **multi-parola**.

```text
no carbonate mineral is an iron oxide mineral
  → Held: nothing is both carbonate mineral and iron oxide mineral. Now I can
    answer no about either one without waiting to be told member by member.
is siderite an iron oxide mineral?
  → No.
```

E' la piu' **fertile** delle tre vie al «no»: il negativo esplicito ne guadagna
uno per volta, la classe definita da regole chiude solo se stessa, una singola
esclusione risponde per **tutti i membri presenti e futuri di entrambe** le
classi. La prova che non e' un caso particolare: in processo fresco

```text
is hematite a carbonate mineral?   → No.
```

per un membro insegnato in una **sessione precedente**, su una classe di cui a
ematite non aveva mai parlato nessuno.

| misura | valore |
|---|---|
| `W` (fatti veri del mondo) | `2` ground + `1` vincolo |
| fan-out del salvataggio | **4 righe** (contro le 119 clausole instradate del §2) |
| richiamo in processo fresco | `5/5`, incluse due negazioni guadagnate |
| `X` (falsita' promosse) | `0` |
| gate | `tests/p0t/knowledge/earned_negation.p0t`, 25 assert, nomi casuali |

### Crescita di comprensione del `gen505` (stesso turno)

Sondando le forme con cui un insegnante parla davvero, tre risultati:

1. **Il soggetto coordinato ora distribuisce.** «hematite and magnetite are iron
   oxide minerals» imparava SOLO il primo e rispondeva «Learned: hematite is an
   iron oxide mineral.» — mezza lezione persa e dichiarata riuscita, che e'
   peggio di un muro. Ogni meta' da sola era gia' leggibile (plurale compreso):
   mancava la distribuzione. Il numero della copula (`plural_copula/1`) e' il
   segnale che distingue una lista da un composto — «salt and pepper IS a
   condiment» non si spezza.
2. **L'enumerazione aveva i membri e non le superfici.** Funzionava solo per
   «what/who IS A copper mineral?», la forma che nessuno usa; «what ARE THE
   copper mineralS?» murava. Due guardie a monte (l'articolo indeterminativo, la
   copula singolare) uscivano prima ancora di guardare la frase. Ora le tre
   forme danno la stessa risposta.
3. **Un reperto non chiuso, e non causato da questo lavoro** (verificato
   ricostruendo il binario precedente): «who is a man?» risponde con la trama di
   *Invisible Man* invece di enumerare i membri della classe. Una facolta' che
   riconosce titoli rivendica il turno prima dell'enumeratore — il «turno
   rubato» di `docs/plans/generation-kb-first.md`, qui su una domanda
   elementare. Priorita' alta: e' una risposta fuori tema data con sicurezza.

### Due colli nuovi, misurati e non chiusi

1. **Una classe nominata solo in un vincolo resta «sconosciuta».** Se la seconda
   classe non ha nessun membro, la domanda finisce sul muro del predicato
   sconosciuto (`I don't know about g_mineral.`) **prima** della risposta polare,
   anche subito dopo aver insegnato l'esclusione che la nomina: `kb_knows_pred`
   (`src/kb.c`) guarda fatti, negativi e teste di regola, **non** i predicati che
   compaiono come ARGOMENTO di un fatto. La dottrina giusta esiste gia' in KB —
   `world_fact_about/1` (`gap-kinds.p0`) considera entrambe le posizioni — e va
   portata qui. Finche' resta, l'esclusione e' utile solo fra classi gia'
   popolate (che e' il caso reale, quindi non blocca la crescita).
2. **La forma italiana entra ma il nome di classe esce ibrido.** Il ramo si
   aggancia — «nessun» e' nella classe — ma la canonicalizzazione produce
   `minerale of carbonato` e `minerale of ossido of iron`. E' l'instabilita' gia'
   registrata in `C_TODO.md` §U4, qui con un'istanza precisa e riproducibile.
   **Non e' stato insegnato nulla in italiano:** avrebbe scritto atomi ibridi in
   KB. Il lotto italiano resta chiuso finche' §U4 non e' risolto.

## 3. Transcript causale del lotto promosso

Tutte le baseline sono state raccolte **prima** di aprire le due classi. Questo
ordine è necessario finché resta il difetto di negazione descritto al §4.

```text
is hematite an iron oxide mineral?
  → I don't know about iron_oxide_mineral.
is magnetite an iron oxide mineral?
  → I don't know about iron_oxide_mineral.
is hematite an iron ore mineral?
  → I don't know about iron_ore_mineral.
is magnetite an iron ore mineral?
  → I don't know about iron_ore_mineral.
```

Dopo ciascun enunciato dichiarativo, la domanda corrispondente ha risposto
`Yes.`. Alla fine, ematite e magnetite sono state ricontrollate dopo altri turni
e hanno mantenuto la risposta. Il contrasto:

```text
is quartz an iron oxide mineral?
  → No.
```

è fattualmente corretto perché il quarzo è SiO2. **Non prova però che la logica
di negazione sia corretta:** lo stesso cammino aveva appena prodotto il falso
“No” su magnetite in una sessione scartata. Un risultato corretto per caso non
certifica il metodo che lo ha generato.

Nel processo fresco i quattro interrogativi hanno risposto `Yes.`: richiamo
persistente `4/4`. Non sono state rivendicate parafrasi, composizione o una
capacità generale. Questi sono fatti isolati raggiungibili attraverso due forme
interrogative note, non ancora un'ontologia mineraria capace di inferire.

## 4. Scoperta critica: l'assenza nella classe diventa falsità

La prima sessione è stata abbandonata senza salvarla. La sequenza minima è:

```text
is hematite an iron oxide mineral?
  → I don't know about iron_oxide_mineral.
hematite is an iron oxide mineral
  → Learned: hematite is an iron oxide mineral.
is hematite an iron oxide mineral?
  → Yes.
is magnetite an iron oxide mineral?
  → No.                       ⛔ falso
```

Quando il predicato di classe non esiste, la risposta è “non so”. Appena esiste
un membro positivo, il modulo `knowledge` tratta un altro membro non registrato
come negativo. È un passaggio indebito da **mancata derivazione** a **derivazione
della negazione**.

La correzione concettuale non è aggiungere magnetite in anticipo. Occorre
distinguere almeno questi stati:

1. esiste un sostegno positivo corrente → `Yes`;
2. esiste un sostegno negativo esplicito corrente → `No`;
3. non esiste sostegno in nessuna direzione → `Unknown`;
4. esistono sostegni positivi e negativi → `Conflict`, con entrambi visibili;
5. la ricerca non è terminata o il budget è esaurito → `Incomplete`, non
   `Unknown` e certamente non `No`.

La chiusura del mondo è lecita soltanto quando esiste una dichiarazione di
completezza **pertinente alla classe, allo scope e alla revisione corrente**.
Sapere un membro di una classe non dichiara completa la sua estensione.

Gate minimo del futuro rimedio, su KB completa:

```text
nessun fatto su K:       “A è K?” → non so
si insegna “A è K”:      “A è K?” → sì
                         “B è K?” → ancora non so
si insegna “B non è K”:  “B è K?” → no
si ritrae la negazione:  “B è K?” → non so
si insegna “B è K”:      “B è K?” → sì
si ritrae il positivo:   “B è K?” → non so
```

Il nome della categoria e i membri del test devono essere insegnabili a runtime;
nessuna parola del dominio va cablata nel motore. Il C può implementare la
meccanica dei quattro/cinque stati epistemici, non decidere quali classi siano
complete.

Questo difetto ha priorità alta perché viola insieme i due lati della missione:
produce uno svarione invece di un muro informato e rende pericoloso popolare una
classe un membro alla volta, che è precisamente il modo KB-first di crescere.

## 5. Scoperta V2: un sintagma può essere appreso ma non più raggiunto

La seconda sessione, anch'essa scartata senza salvarla, aveva aggiunto un quinto
enunciato vero:

```text
iron ore is a mineral
  → Learned: iron ore is a mineral.
```

Il dump rappresentava correttamente `iron_ore` come soggetto unico. Eppure:

```text
is iron ore a mineral?
  → I don't understand that yet.
what is iron ore?
  → I don't know much about iron ore yet. Want me to look it up?
forget that iron ore is a mineral
  → proposta di imparare “forget”, nessuna retrazione
```

Quindi il difetto non è soltanto lessicale. Il lettore dichiarativo costruisce
un'entità multi-parola, ma domanda, descrizione e retrazione non condividono
sempre lo stesso oggetto. È una nuova evidenza concreta del “cassetto senza
maniglia” già documentato più avanti nel file:

> ciò che si impara da un sintagma deve essere interrogabile, correggibile e
> ridicibile attraverso lo stesso sintagma.

Il rimedio generale appartiene a V2/G1–G5:

- conservare l'intero span e la superficie originale, non soltanto una chiave
  fusa;
- far usare a dichiarazione, domanda, coreferenza, forget e realizzazione lo
  stesso referente;
- distinguere testa e proprietà mediante conoscenza della lingua;
- conservare determinante, ordine e lingua quando incidono su riferimento e
  resa;
- far sì che la retrazione ritiri il sostegno del fatto e le conseguenze che
  non hanno altri sostegni, senza cancellare la storia dell'osservazione.

Il gate non è soltanto “la domanda ora dice sì”. Deve reggere questa famiglia:

1. insegnare naturalmente un fatto con soggetto multi-parola;
2. chiederlo con la stessa superficie e con una parafrasi;
3. introdurre due referenti con la stessa testa e modificatori diversi;
4. riprenderli con una forma anaforica non ambigua;
5. chiedere una proprietà senza usare la chiave interna;
6. ritrarre il fatto con la superficie naturale;
7. osservare che la risposta torna a “non so” e non a “no”;
8. reinsegnare e verificare il richiamo in un processo nuovo.

Non aggirare questo confine insegnando all'utente la grafia `iron_ore`. La
chiave interna non è la lingua del dialogo.

## 6. Scoperte linguistiche precise

| forma | lettura osservata | conseguenza |
|---|---|---|
| “hematite is an iron oxide mineral” | unaria, soggetto `hematite`, classe composta | apprendimento e replay riescono |
| “is hematite an iron oxide mineral?” | stessa classe composta | domanda sì/no raggiungibile |
| “is hematite a type of iron oxide mineral?” | classe distinta, equivalente alla superficie `type_of_...` | la parafrasi non viene normalizzata verso la stessa relazione |
| “iron ore is a mineral” | soggetto composto correttamente nella lezione | il fatto entra, ma gli altri atti non condividono il referente |
| “is iron ore a mineral?” | analisi spezzata / reachability gap | stesso contenuto non recuperabile |
| “forget that iron ore is a mineral” | “forget” viene trattato come materiale da imparare | la ritrattazione non è simmetrica all'apprendimento |
| “what did you understand?” | “I had something on:” senza contenuto | metacomprensione vuota; non sa ridire la delta della lezione |
| “how do you know that hematite is an iron oxide mineral?” | spiegazione causale generica | confonde giustificazione epistemica e meccanismo causale |

Tre conseguenze generali:

1. **Le parafrasi devono collegare letture, non duplicare fatti.** “X è un K” e
   “X è un tipo di K” dovrebbero convergere quando la KB sostiene
   l'equivalenza della costruzione. Insegnare il fatto due volte sotto due
   predicati superficiali nasconde il gap e frammenta l'ontologia.
2. **Gli atti dialogici devono condividere il contenuto proposizionale.** La
   stessa proposizione deve poter essere asserita, chiesta, negata, corretta,
   dimenticata, spiegata e ridetta senza che ogni atto ricostruisca nomi propri.
3. **“Perché?” non è una sola relazione.** “Che meccanismo causa P?” cerca una
   catena causale; “quale evidenza sostiene P?” cerca provenienza e proof;
   “perché hai scelto questa risposta?” cerca la decisione del dialogo. La forma
   superficiale può essere ambigua, quindi letture e contesto devono restare
   concorrenti fino a evidenza sufficiente.

Le equivalenze delle forme, le cue interrogative e le realizzazioni appartengono
alla KB e devono poter essere aggiunte e ritratte parlando. Il motore può legare
slot e attraversare una lettura; non può contenere una lista privata con
“type of”, “how do you know” o le parole future.

## 7. Provenienza: presente come dato, assente come risposta

Per ognuno dei quattro fatti persistono una `reading_fact` e una
`fact_source`. Questo basta a ricordare la frase didattica, ma non basta ancora
a rispondere in modo fondato alla richiesta di giustificazione.

Nel processo fresco:

```text
how do you know that hematite is an iron oxide mineral?
  → lungo schema generico su condizione iniziale, processo ed effetto misurabile
```

La risposta è fluente ma non pertinente. Non cita la frase insegnata, non espone
un percorso di prova e non identifica USGS. Il modulo ha riconosciuto una forma
causale e ha preso il turno prima del consumer epistemico.

Occorrono tre livelli separati:

- **origine dell'osservazione:** chi ha pronunciato o quale documento contiene
  la frase, con revisione e span;
- **sostegno corrente:** quali osservazioni e regole autorizzano oggi la
  proposizione, incluse alternative indipendenti;
- **fonte di verità usata dal training:** riferimento istituzionale verificato,
  non soltanto il nome dell'entità e il testo della lezione.

La frase del teacher non diventa una fonte indipendente solo perché è stata
salvata due volte come `reading_fact` e `fact_source`. Il prossimo verticale di
provenienza deve rispondere dalla proof effettiva e deve degradare onestamente:
se conosce il fatto ma non possiede in KB la fonte istituzionale, deve dirlo
senza inventare una spiegazione causale.

## 8. Conferma di apprendimento e FalseUnderstanding

“Learned” va generato dalla modifica realmente avvenuta, non dall'intenzione del
parser. In questo lotto i quattro successi promossi sono veri perché replay e
processo fresco li confermano. Tuttavia due segnali restano rossi:

- “what did you understand?” non sa esporre il fatto appena acquisito;
- il quinto fatto su `iron ore` era internamente presente ma inutilizzabile dagli
  atti naturali successivi.

Una conferma matura dovrebbe poter dire, nella lingua dell'interlocutore:

```text
ho acquisito la proposizione P;
l'ho collegata alla lettura L della tua frase;
posso usarla nelle domande Q che conosco;
non ho ancora verificato le parafrasi o gli usi R.
```

Non serve stampare nomi di predicati. Serve conservare un oggetto “delta della
lezione” che il replay, la spiegazione e il retract possano condividere. Una
conferma che promette più di quanto il replay dimostra conta come falsa
comprensione anche se il fatto raw è entrato.

Per il lotto promosso `FalseUnderstandingRate=0/4`: ciascuno dei quattro
“Learned” ha replay e richiamo fresco. Il tentativo multi-parola non è stato
promosso e la sua sessione è stata scartata; resta una diagnosi, non un successo.

## 9. Persistenza e genealogia: due debiti da non nascondere

### 9.1 Fan-out del salvataggio

Quattro fatti hanno prodotto 119 clausole persistite. Settantasette sono stem
verbali materializzati collateralmente. Non sono falsi, ma non sono causati
semanticamente dalle quattro lezioni minerarie. Due referenti e 28 turni formano
il resto del materiale operativo.

Il prossimo lavoro sul consolidamento deve conservare tutto ciò che serve alla
genealogia senza confondere:

- delta della lezione;
- materiale già derivabile al boot;
- tracce del dialogo;
- fatti del mondo;
- indici o viste materializzate.

Una vista derivabile può essere persistita per prestazioni solo con versione,
dipendenze e invalidazione. Non deve gonfiare il conteggio delle cose imparate.

### 9.2 Identità dei turni

Gli identificatori `utterance(N,...)` ripartono in sessioni diverse. Clausole
identiche possono collidere e sparire per semantica d'insieme; infatti alcune
risposte `Yes.` non compaiono come eventi distinti nel dump promosso. Un numero
locale senza identità di sessione non è una provenienza globale sufficiente.

La correzione generale deve distinguere almeno sessione, turno, parlante e
ordine, preservando la compatibilità come vista. Non inserire un contatore
globale fragile né dedurre dalla mancanza della riga che la risposta non sia
avvenuta: il transcript verbatim del laboratorio prova che è avvenuta.

## 10. Che cosa NON è stato dimostrato

- Non è stata acquisita una regola che colleghi “ossido di ferro” e “minerale
  di ferro”. Sono due classificazioni ground parallele.
- Non è stata acquisita una capacità generale di mineralogia.
- Non è stata provata la parafrasi “a type of”.
- Non è stata provata una domanda aperta del tipo “quali sono…?”.
- Non è stata provata una composizione che ricavi una nuova conclusione.
- Non è stata provata la retrazione dei quattro fatti promossi.
- La provenienza esterna USGS è documentata nel laboratorio, ma non è ancora
  raggiungibile come sostegno istituzionale nella risposta di parrot0.
- Il `No` corretto su quarzo non certifica negazione calibrata.
- Quattro fatti e un richiamo 4/4 non dimostrano parità con un LLM.

Queste negazioni sono parte del risultato. Non trasformarle in percentuali
positive e non completarle per supposizione.

## 11. Ordine preciso per continuare

### Passo A — continuare subito la crescita fattuale, ma in corsie sicure

Proseguire con micro-lotti di fatti reali, stabili e con fonti autorevoli. Ogni
lotto deve essere piccolo abbastanza da poter essere abbandonato integralmente
al primo fatto falso o ambiguo. Finché il difetto del §4 è aperto:

1. scegliere tutti i membri positivi della stessa classe previsti nel lotto;
2. raccogliere tutte le baseline prima di insegnarne il primo;
3. insegnare soltanto in lingua naturale;
4. verificare ogni lezione subito, poi di nuovo dopo turni diversi;
5. usare contrasti la cui verità negativa è stata verificata indipendentemente;
6. non interpretare un `No` per assenza come prova di calibrazione;
7. scartare la sessione se compare una falsità o se il binding è ambiguo;
8. promuovere soltanto ciò che torna in un processo fresco.

Preferire per ora entità a parola singola e classificazioni chiare quando lo
scopo è aumentare `W`. Questo non assolve il difetto multi-parola: lo isola per
permettere alla KB di crescere mentre V2 viene chiuso. Non usare fatti già noti,
definizioni controverse, categorie vaghe o proprietà dipendenti dal campione.

Un possibile seguito dello stesso dominio è usare minerali con composizione
istituzionalmente documentata, ma ogni proposizione va ricontrollata prima della
lezione. Non insegnare formule, relazioni binarie o regole generali finché la
forma interrogativa e il retract corrispondenti non superano una prova breve:
un fatto ingestibile è più costoso di un fatto non ancora aggiunto.

### Passo B — chiudere la falsa negazione come meta-gap di crescita

> **Avanzato al `gen505` (§2-bis):** le tre vie al «no» guadagnato esistono e
> sono tutte raggiungibili **parlando** — negativo esplicito, classe definita da
> regole, ed esclusione fra classi (quest'ultima aperta al `gen505`, prima aveva
> zero produttori). Restano i due colli del §2-bis: la classe nominata solo in un
> vincolo e la forma italiana.

È il primo collo inferenziale emerso direttamente dall'addestramento. Il
rimedio deve essere generale, KB-first e indipendente dalla mineralogia. La
policy che autorizza mondo chiuso, conflitto e incompletezza appartiene alla KB;
il motore esegue la ricerca e restituisce lo stato supportato.

La prova causale deve aggiungere a runtime una categoria e membri mai compilati,
mostrare `unknown → yes → unknown/no esplicito → yes`, poi ritrarre e
reinsegnare. La KB completa resta caricata. Un test golden che contiene già i
membri non dimostra crescita.

### Passo C — chiudere il referente multi-parola attraverso tutti gli atti

Non correggere separatamente “is iron ore…”, “what is iron ore…” e “forget
that iron ore…”. Sarebbero tre toppe allo stesso difetto. Il produttore della
lettura deve pubblicare un referente con span, superficie, lingua, testa e
proprietà; domanda, memoria, coreferenza, correzione, retract e realizzazione
devono consumare quel referente o una vista derivata dallo stesso oggetto.

Il caso `iron ore is a mineral` è il ratchet reale da mantenere rosso finché
l'intera catena del §5 non passa.

### Passo D — distinguere giustificazione, causa e decisione dialogica

La richiesta “how do you know P?” deve aprire un bisogno di sostegno, non un
template causale. Il verticale deve coprire:

- un fatto appreso direttamente, per cui la risposta mostra origine e limite;
- una conclusione derivata, per cui la risposta mostra premesse congiunte e
  regola;
- due sostegni indipendenti, uno dei quali viene ritratto senza perdere P;
- l'ultimo sostegno ritratto, dopo cui P non può più parlare come corrente;
- una domanda causale vera, che continua a ottenere un meccanismo e non una
  citazione;
- una domanda sulla scelta della risposta, che espone lettura e modulo/mossa
  senza fingere una causa del mondo.

Le superfici EN e IT vengono insegnate e ritratte nella KB. Non codificare
“how do you know” o “come lo sai” nel C.

### Passo E — rendere la conferma una descrizione verificabile della delta

Collegare la conferma di apprendimento alla proposizione o capacità realmente
aggiunta. “What did you understand?” deve poter ridire contenuto, scope e limiti
senza schema interno. Retract e reteach devono puntare allo stesso oggetto della
lezione.

### Passo F — solo dopo, aprire inferenza fertile fra le classi

Quando fatti, domande, negazione e retract sono stabili, insegnare una relazione
o regola reale che colleghi classi già popolate. Il gate non è che la regola si
esegua tre volte sullo stesso pattern. Servono:

- spiegazione naturale dei ruoli;
- almeno tre held-out veri;
- due parafrasi;
- un quasi-esempio che non deve passare;
- una composizione con conoscenza già presente;
- retract, perdita delle sole conclusioni dipendenti e reteach;
- trasferimento a un secondo dominio reale.

È qui che il numero di fatti smette di crescere linearmente e la KB comincia a
“connettere i punti”. Anticiparlo su fondamenta epistemiche instabili
moltiplicherebbe gli svarioni.

## 12. Criterio di priorità per ogni turno futuro

Quando l'addestramento incontra un arresto, scegliere il lavoro successivo in
quest'ordine:

1. falsa affermazione o falsa negazione;
2. dichiarazione infondata di aver capito/appreso/eseguito;
3. fatto presente ma irraggiungibile con la sua lingua naturale;
4. provenienza o scope perso, risposta stale dopo correzione;
5. collo condiviso da più facoltà;
6. nuova conoscenza fattuale;
7. rifinitura locale di una singola superficie.

La priorità 1–5 non invita ad abbandonare l'addestramento per costruire
infrastruttura astratta. Il difetto deve emergere da una lezione reale e il
rimedio deve riaprire immediatamente quella lezione più casi indipendenti.

## 13. Regole per un agente che conosce poco questa codebase

1. Non scrivere una parola naturale nel C per riconoscerla o generarla. Se una
   parola futura deve funzionare senza ricompilare, è conoscenza KB.
2. Non insegnare al teacher nomi interni, tuple, arità o sintassi P0. La lezione
   deve essere formulabile da un esperto del dominio che ignora lo schema.
3. Non usare una KB amputata per far passare la prova. Isola la candidata, non
   il soggetto completo.
4. Non aggiungere il fatto che la domanda dovrebbe inferire. Il test di
   connessione deve contenere soltanto premesse e regole lecite.
5. Non confondere “nessuna proof trovata” con falso. Senza negazione o
   completezza, la risposta è ignota.
6. Non contare “Learned” come successo. Replay, uso nuovo, retract e processo
   fresco decidono lo stato.
7. Non riparare a mano una sessione contaminata e poi salvarla. Le sessioni
   scartate di questo turno mostrano il comportamento corretto del teacher.
8. Non trasformare ogni parafrasi in un nuovo fatto del mondo. Le forme devono
   convergere attraverso conoscenza linguistica.
9. Non lasciare una regola senza produttore, consumer e porta naturale. La sola
   presenza in KB non la rende viva.
10. Non usare un LLM esterno come oracolo di verità. Può proporre mosse o lezioni;
    i fatti hanno fonti e i risultati hanno verificatori indipendenti.
11. Non riaprire per ora il fronte dell'osservabilità della sonda. Registra
    comunque risposte esatte, revisione e stato del training: serviranno quando
    F. cambierà priorità.
12. Non cancellare strutture secondarie perché oggi sembrano ridondanti.
    Correggi precedenza e sostegni; preserva ciò che può tornare utile.

## 14. Definizione del prossimo checkpoint buono

Il prossimo checkpoint è valido se realizza **almeno uno** dei due esiti:

- un nuovo micro-lotto con `W >= 3`, fonti registrate, nessuna falsità, replay e
  richiamo fresco al 100%, senza fingere una capacità generale; oppure
- la chiusura causale di uno dei colli §4/§5/§7/§8, seguita dalla stessa lezione
  naturale che prima falliva, transfer indipendente, retract/reteach e processo
  fresco.

In entrambi i casi il resoconto deve separare fatti del mondo, lingua,
costruzioni, provenienza, tracce e viste materializzate. Ogni rosso osservato
resta nel resoconto anche se il risultato finale è verde.

Il traguardo immediatamente successivo non è “aggiungere molte righe”. È
ottenere una catena in più in cui parrot0 **impara, sa che cosa ha imparato, lo
usa, lo giustifica, lo corregge e lo ricorda**, senza che il teacher conosca i
suoi nomi interni. Riprendi da qui e continua.

# ⭐ gen502 — «vai avanti col lavoro» non esiste nella KB, ed è una lezione

Misurato sotto il profilo `agi.p0`, sette forme, nessuna funziona:

```text
continue, next            -> «Hi there! What would you like to talk about?»
go on, carry on,
go ahead, what next?      -> «I don't understand that yet.»
keep going                -> «Thanks — I'm learning as we go.»
```

Un muro, un reset di cortesia e un ringraziamento a vuoto. **Non è la parola
sbagliata: è che la nozione manca.** Trovato dal banco di gara, dove undici
turni su dodici se ne andavano a spiegare a parrot0 una parola mandata da noi
(`CHALLENGE_TODO` S4).

⚠ **Ma non si insegna come vocabolario e basta.** La superficie (`intent_phrase`
+ `learnable/3`, EN+IT) è la parte da poco; la parte vera è che ci sia *a che
cosa tornare* — `resumable/1` come vista, ipotesi D49 in
[`docs/plans/continue-as-resumption.md`](docs/plans/continue-as-resumption.md).
Insegnare le forme senza la vista produce un `continue` che rifà l'ultimo input
invece di riprendere il lavoro: sembra funzionare, e si scopre al secondo
`continue`, che è dove serve.

**Ordine:** prima R1-R3 del design (KB pura), poi le forme come lezione parlata,
e il gate è un dominio `plan_goal` **insegnato parlando**, interrotto da una
digressione e ripreso.

# ⛔ RIPARTI DA QUI — handoff 2026-09-04: comprensione universale del codice

> **Nuova direzione esplicita di F.:** portare parrot0 al livello in cui, data
> una codebase, risponde a domande strutturali, causali e qualitative — per
> esempio se una parte andrebbe migliorata o dove si concentra la latenza — non
> con un frasario o una collezione infinita di smell, ma con una IR interna,
> reasoning KB-first e conoscenza trasferibile via prompt secondo
> `LEARN_PROTOCOL.md`.
>
> **Piano autoritativo:**
> [`docs/plans/universal-code-comprehension.md`](docs/plans/universal-code-comprehension.md).
> Integra `CODE-MASTERY.md`, `universal-input.md`, T11/T12 del coding agent e W4
> della fucina; non sostituisce i piani di patch/repair, perché il primo traguardo
> è capire e giudicare in read-only.


## §0-septies. ⭐ gen492 — LA COMPRENSIONE DEL CODICE, E IL PONTE CHE LA RENDE VIVA

> Piano autoritativo:
> [`docs/plans/universal-code-comprehension.md`](docs/plans/universal-code-comprehension.md)
> (§8-bis e §8-ter portano lo stato eseguito; §7-bis le leve **misurate**).

**Che cosa è entrato**

| | |
|---|---|
| **UC1** | la IR del codice con identità, span, hash e provenienza; `code_function`/`code_calls` degradati a compatibilità |
| **UC1b** | il **ponte fra rappresentazioni** è vivo: `code_name_part/4` + `identifier_separator/1` + quattro `representation_bridge/4` |
| **UC2/UC2c/UC2d** | il giudizio qualitativo è un motore: misura come **predicato variabile**, controevidenza, severità/precedenza e Task IR come policy KB |

**La prova che vale più delle righe:**

```text
> what functions does this define: int hash_table_insert(void) { quicksort(); … }
> domain knowledge about hash_table_insert
    Key-value mapping -- O(1) average lookup/insert/delete.
> improvement note for wide
    Wide fanout — code definition fanout = 9.
```

La prima frase **non è mai stata scritta per parlare di codice**: viene
dall'esperto di algoritmi. La stessa conoscenza serve due rappresentazioni senza
essere duplicata — è il moltiplicatore che F. chiedeva.

**⛔ Il punto di ripresa, in ordine**

1. **`scripts/dead-organs.py`** — 94 predicati citati dalle regole con zero
   fatti. Il numero sovrastima (molti sono asseriti a runtime), ma il triage è
   una domanda sola: *«questo chi lo scrive, e quando?»*. Tre volte in quattro
   giorni il guadagno più grosso è venuto da qui, non da codice nuovo.
2. **Severità e ordinamento fra finding** (§5, non fatto): oggi i finding sono
   un insieme, non una lista ordinata; mancano tradeoff e azione informativa.
3. **§5.2, la latenza**: senza profilo si possono produrre solo *candidati
   costosi*. «Questa parte è lenta» senza evidenza dinamica deve restare
   impossibile — è la regola di onestà più forte del piano.
4. **La rilettura**: una lezione deve far rileggere tutto lo snapshot (§7 leva
   7). Oggi la rilettura invalida, ma nessuno la usa per capire *di più* su ciò
   che era già stato letto.
5. **Il debito del gen491**: ricostruzione **incrementale** delle viste
   materializzate — insegnare un verbo costa ancora 1406 ms nel turno dopo.

**⚠ Tre limiti del motore trovati costruendo, da conoscere prima di progettare:**
l'arità massima è **4**; `naf/1` rifiuta un goal non ground (giusto, ma la
guardia non può portarsi dietro variabili libere); e le parole di una domanda
devono essere **parole che parrot0 conosce**, o il turno viene preso dal
percorso che offre di imparare il termine ignoto.


## UC — il confine trovato dall'analisi

> **Estensione di F., stesso giorno:** la IR non deve diventare una zona isolata.
> Il fondamento è D38/GD10: `ir_denotation` dà a ogni item un'entità condivisa,
> `representation_bridge` nomina un arco fra rappresentazioni e `apply/2`
> attraversa il confine con predicato variabile. Il guadagno cercato è che una
> conoscenza appresa in un dominio soddisfi un bisogno emerso dal codice senza
> progettare quella capacità. Ogni claim conserva la basis dell'arco; ablation
> e `false_composition` impediscono che “comporre” significhi inventare.

- I 25 stimoli di `tests/code/*.code` sono tutti marcati `pass`, ma il banco è
  saturo su un insieme chiuso di domande da snippet.
- La IR code realmente condivisa oggi contiene solo `code_function/1` e
  `code_calls/2`: non porta file, span, scope, identità, source hash o revisione.
- `mod_codeast` decide ancora le specie di domanda con rami `wants_*`; explain
  compone pochi tratti riconosciuti; una domanda qualitativa non ha un oggetto
  interno su cui ragionare.
- I quattro successi SWE sono quattro scanner C specializzati. Sono sensori
  secondari utili, non il motore universale da estendere col quinto pattern.
- Gli organi generali esistono già e vanno collegati: IR dell'input,
  evidence scorer, Task IR, viste materializzate, compensazione/replay e
  provenance. `code_keyword/2` è perfino già in KB, ma i frontend usano ancora
  liste private: piano scritto, consumer mancante.

## UC — ordine di lavoro aggiornato

1. **UC1: sorgente revisionata e attraversabile nella KB.** Snapshot, source
   unit, hash, span, symbol id e provenance; C/Python pubblicano nella stessa IR
   le sole definizioni/chiamate che già vedono; `ir_denotation` + `apply/2`
   aprono gli archi verso altre zone; le viste legacy restano derivate e
   deprecabili, non vincoli architetturali.
2. **Rilettura causale.** Re-ingerire un file deve sostituire la closure vecchia,
   non accumularla. Nessuna ingestione massiva prima di questo gate.
3. **Domanda → Task IR → obbligo di evidenza.** Eliminare come via primaria i
   `wants_*` e rendere insegnabili/retrattabili via prompt le forme delle
   domande sul codice.
4. **Scope, reference, CFG, def-use ed effetti.** Sono gli archi con fan-out
   maggiore: aprono explain, impact, qualità e performance insieme.
5. **Criteri qualitativi in KB.** Finding = criterio + evidenza +
   controevidenza + policy + tradeoff; mai un verdetto hardcoded.
6. **Performance grounded.** Senza profilo solo sospetti statici e gap
   informato; con profilo ranking legato a workload, snapshot e symbol/span.
7. **Teaching completo.** Semantica di costrutto, mapping di osservazione,
   criterio e policy si insegnano in lingua naturale con replay, Transfer@3,
   contrasto, composizione, ablation, reteach e fresh process.
8. **Autocorrezione e scala.** Gap code → azione read-only → replay; poi indice
   per termine e viste materializzate guidati da `/debug`, mai timeout alzati.

**Primo incremento esatto:** §9 del piano. Non aggiungere un altro smell, non
avviare una suite lunga e non popolare una grande AST senza consumer. Il primo
verticale deve già rispondere da definizioni/chiamate revisionate, mostrare la
provenance, perdere gli archi stale dopo una rilettura e acquisire/ritrarre via
prompt almeno una nuova forma di domanda.

## UC1 — checkpoint eseguito (2026-09-04)

Il verticale è ora operativo e resta volutamente piccolo: C e Python pubblicano
snapshot, unità sorgente, nodi, nomi, span, archi e provenienza nella stessa KB;
le viste `code_function/1` e `code_calls/2` sono proiezioni di compatibilità.
`ir_denotation/4` e `representation_bridge/4` permettono a una query di passare
dal codice a una conoscenza di dominio tramite `apply/2`, conservando una
`ir_domain_claim_basis/5`; l'ablazione del ponte elimina il claim e non i fatti
di base. La rilettura sostituisce la closure della sorgente precedente.

Il cricchetto focalizzato
[`tests/p0t/code/universal_code_ir.p0t`](tests/p0t/code/universal_code_ir.p0t)
ha chiuso **61/61** verifiche: IR C/Python, keyword insegnabile e retraibile,
ponte cross-rappresentazione, provenance, rilettura e teaching/retract/reteach
della forma di domanda. Una misura iniziale mostra 20 osservazioni ground per
uno snippet C di tre funzioni; il costo è dominato dall'infrastruttura generale
(`intent_cue`/`segment_role`), non dal publisher IR.

È stata eseguita una sola `make soft-test`, come previsto dal protocollo: il
frontier chat audit resta a **53 pass / 3 fail** su risposte di fallback. La
diagnosi isolata (boot code-IR rimosso e query ripetuta) produce lo stesso esito,
quindi non è una regressione attribuibile a UC1; non viene riavviata la suite.

**Prossimo incremento:** estendere la IR con CFG e dipendenze di flusso come
relazioni generali, riusabili da domande disparate (impact, ownership,
data-flow, qualità), e solo dopo collegare provider opzionali come il profiling
dinamico.
Nessun verdetto di performance sarà emesso senza workload, snapshot e profilo.

**Regola anti-muro:** ogni domanda deve produrre una conclusione supportata o
un gap tipato con una prossima azione informativa; ogni rientro di thinking deve
cambiare rappresentazione, prospettiva, evidenza od obiettivo. Un rientro che
non cambia nessuno di questi è uno specchio e va arrestato.

Il ratchet del thinking rende ora il vincolo eseguibile: ogni rientro dichiara
`reentry_brings/3` più un `thinking_feedback/4` (`fact_delta`, `query_result` o
`gap_with_action`); un gap è valido solo con `thinking_feedback_action/3`.
`thinking_graph.p0t` verifica ablation, non-peggioramento, stop e scelta runtime:
**36/36**.

La IR ora espone anche `code_ordered_before/3`: ordine osservato tra nodi dello
stesso scope, distinto esplicitamente da una CFG. È un nuovo registro che il
thinking può interrogare per sequenza e dipendenza; branch/merge saranno
aggiunti solo quando osservati dal frontend.

---

# ⛔ RIPARTI DA QUI — handoff 2026-09-03

> **Come si riprende:** «continua da questo file». Leggi **§−1** (perché siamo
> lenti, con i numeri), **§0-bis** (il ciclo di autocorrezione che ora si chiude),
> **§0-quinquies** (il lettore strutturale acceso al gen490, e il passo che
> manca) e **§0-quater** (la catena compilata: ancora aperta),
> poi §0/§1/§2 per l'obiettivo e lo stato, poi §3 «Il prossimo lavoro».
> Il resto è storia e serve dopo.
>
> **Se hai poco tempo, leggi una cosa sola:** §−1 moltiplicatore 3 — *un piano
> scritto e non eseguito non è lavoro fatto, è lavoro che verrà rifatto in forma
> peggiore*. È la causa misurata del 42% di rilavorazione.

## §−1. ⛔ PERCHÉ SIAMO SEMPRE LENTI — misurato, e la causa vera

> F., 2026-09-03: *«fai delle analisi su come velocizzare la crescita di parrot0
> sulla base dei processi messi in campo, mi sembra che siamo sempre lenti»*.

**La cadenza non è il problema.** 477 commit in 30 giorni, ~16 al giorno.
Il problema è la **resa**:

```text
commit negli ultimi 30 giorni                              477
commit che nominano una rilavorazione, un errore o un
  difetto già noto ritrovato                               202   ≈ 42%
```

**Due commit su cinque rifanno qualcosa.** Non è disattenzione: sono tre
moltiplicatori strutturali, e ciascuno ha una cura precisa.

### Moltiplicatore 1 — il rosso annotato invece che chiuso

GD3 («`e` per `è`») è stata **trovata tre volte** prima di essere chiusa, e la
terza l'ha incontrata l'utente. Ogni ritrovamento costa una diagnosi da capo.
⇒ **Cura: chiusura al secondo avvistamento** (§10 di
`docs/plans/universal-comprehension.md`). Il registro dei rossi è una coda di
lavoro, non un archivio.

### Moltiplicatore 2 — la classe popolata dai sintomi

`imperative_opener` aveva *execute, run, migrate* e non aveva *tell, write*.
`locative_transfer_verb` aveva *put, place* e non aveva *have* — il più frequente
dei tre. Ogni membro mancante torna come un bug separato, con la sua diagnosi.
⇒ **Cura: completamento della classe quando entra**, non quando qualcuno inciampa.

### Moltiplicatore 3 — ⛔ IL PIANO SCRITTO CHE NESSUNO ESEGUE (il più caro)

Questa è la scoperta di oggi, e spiega la lentezza meglio delle altre due.

`kb/core/arrests.p0` contiene **l'intero strato dichiarativo** dell'autocorrezione
— `turn_arrest`, `compensation_obligation`, `compensation_step`,
`compensation_plan`, `compensation_stop`, `compensation_alternative` — scritto al
gen442 e completo. Sopra `turn_compensation_obligation` c'era scritto, **dal
giorno in cui è nato**:

> *«Primo consumer per gen442: **oggi non esegue una compensazione**.»*

**Il rappresentare c'era, l'eseguire no.** Conseguenza: ogni riparazione degli
ultimi cicli è nata come **ramo speciale in C** — e `autocrescita-v3.md` lo vieta
esplicitamente (*«l'autocorrezione non deve essere un sottosistema speciale»*).
Il ciclo non si chiudeva mai, quindi ogni lacuna andava chiusa a mano, una per
una, al costo pieno.

⇒ **Cura, fatta oggi: l'esecutore.** Vedi §0-bis.

> **La regola che ne esce, e che vale più delle tre cure:**
> **un piano scritto e non eseguito non è lavoro fatto: è lavoro che verrà rifatto
> in forma peggiore.** Prima di aprire un piano nuovo, si chiude il ciclo di uno
> vecchio.

## §0-bis. ✅ IL CICLO DI AUTOCORREZIONE SI CHIUDE (2026-09-03)

`autocrescita-v3.md` §0 lo descrive così:

```text
turno -> arresto TIPIZZATO -> piano di compensazione -> azione mirata
      -> REPLAY dello stesso turno -> attribuzione causale -> promozione
```

Costruito l'**esecutore** (`p0_compensate`, `src/brain/99-registry.c`), e resta
minimo di proposito: **non sceglie**. Chiede alla KB quali azioni compensano la
specie di arresto (`compensates/2`), le prova nell'ordine dichiarato, si ferma
alla prima che fa ripartire il turno. Il motore possiede solo le **primitive**,
come `chars/2` o `apply/2`; quali azioni esistano è conoscenza.

**Prima azione viva — `repair_surface`.** parrot0 esegue su di sé il test del §10
(*prova la frase minimamente diversa*), e se il turno riparte **scrive la
riparazione**:

```text
> cosa e rimasto in sospeso        (senza accento, nessuna superficie la raggiunge)
    Non c'e' niente in sospeso: ho risposto a tutto quello che hai chiesto.
!query surface_variant("cosa e rimasto in sospeso", "cosa è rimasto in sospeso")  ✅
```

Il turno dopo è un **colpo diretto**, non una seconda riparazione: *una capacità
che si riconquista ogni volta non è stata acquisita.*

⚠ **Due correzioni di rotta che F. ha imposto durante la costruzione, e vanno
ricordate:**

1. **Niente strumenti offline.** Avevo scritto un rilevatore di lacune che legge
   la KB e stampa una coda (274 varianti trovate). F.: *«non voglio uno strumento
   rilevatore di lacune offline, voglio che LUI autorilevi la lacuna e la
   corregga»*. Uno strumento offline lo esegue qualcun altro: **non è crescita di
   parrot0**. Rimosso.
2. **L'alfabeto stava nel C.** La prima versione portava à→a, è→e dentro il
   motore. F.: *«fammi capire come lo vuoi fare, deve essere KB-first»* — e la
   domanda è arrivata prima che costasse. Ora `equivalent_letter/2` è in KB, e
   spagnolo/portoghese/tedesco sono righe, non ricompilazioni.

**Il test KB-first, per intero:**

| pezzo | dove | nuovo membro domani senza ricompilare? |
|---|---|---|
| quali azioni compensano quale arresto | `compensates/2` | ✅ una riga |
| quali relazioni portano superfici | `surface_bearing_relation/1` | ✅ una riga |
| quali lettere sono equivalenti | `equivalent_letter/2` | ✅ una riga |
| che cos'è un muro | `wall_marker/1` (già esistente, **non** duplicato) | ✅ |
| la riparazione appresa | `surface_variant/2`, scritta da parrot0 | ✅ |

Gancio: la compensazione corre **dove il progetto riconosce già una resa**, non
solo su `!handled` — perché il muro quasi sempre lo produce un modulo che *ha*
rivendicato il turno. Ratchet: `tests/p0t/conversation/self_compensation.p0t`
(9 assert), che verifica anche che togliendo `compensates/2` l'azione sparisca.

**Le due azioni successive sono dichiarate e NON eseguibili** — è il prossimo
lavoro, ed è piccolo perché l'esecutore ora c'è:

```prolog
compensates(read_source, knowledge).      % ⛔ Wikipedia, l'unica sorgente ammessa
compensates(ask_user, reference).         % ⛔ manca un antecedente: si chiede
```

## §0-sexies. ⭐ IL CARICO: PROFILATO, E RIDOTTO 4× (gen491)

> F.: *«la nostra KB deve crescere e crescerà, quindi i problemi di carico li
> avremo sempre»*. Il mantra **#20** porta la strategia; qui lo stato.

`/debug` — un profilo per predicato che esisteva da generazioni e non era mai
stato puntato lì — ha detto tutto in un colpo: **`extract_frame` era l'83% del
turno** (890 ms su 1075, 442 chiamate). Congelato con `materialized_view/2`:

| | prima | dopo |
|---|---|---|
| turno a regime | 1058 ms | **259 ms** |
| solver | 903 ms | 99 ms |
| `extract_frame` | 803 ms / 266.553 passi | 1,9 ms / 442 passi |
| resa di lettura | 0,26 fatti/frase | **0,37** |

**Il pagamento:** i 50 verbi di relazione che al gen490 mandavano in timeout
metà della suite ora passano senza toccare un gate. *Quando una classe di
conoscenza non si può allargare, il difetto non è quasi mai nella classe.*

**Che cosa resta da fare sul carico**, in ordine di leva:
1. **Il prossimo predicato caro non si indovina: si profila.** Dopo il
   congelamento la classifica è cambiata — `intent_cue` (585 call/turno) e
   `segment_role` (353) sono ora in testa. Vanno guardati con `/debug` prima di
   toccarli, non prima.
2. `input_nearest_entity_before/after` usa `naf(input_entity_between(...))`,
   che è O(n²) sui nodi del turno: è il costo che frenerà il **lettore
   strutturale** quando il corpus crescerà, ed è il candidato naturale per una
   lista ordinata con early match.
3. L'indice per **argomento** (`kb_fact/2` con predicato libero) resta il §L
   originale, e ora è il residuo più grosso.

## §0-quinquies. ⭐ IL LETTORE STRUTTURALE ERA COSTRUITO E SPENTO (gen490)

> F.: *«massimizziamo il corpus di addestramento»*. Cercando dove fosse il collo
> di bottiglia è venuta fuori una cosa più grossa della domanda.

**Il corpus non era il collo di bottiglia: lo era la lettura.** Su prosa vera di
Wikipedia la resa era **0,13 fatti per frase**, e il soggetto-verbo-oggetto
semplice — `Photosynthesis releases oxygen`, la forma più comune della prosa
enciclopedica — non veniva estratto affatto. A quel rendimento nessuna quantità
di prosa serve a niente.

**La causa.** `kb/core/input-structure.p0` implementa per intero il matcher di
`universal-input.md`, ponte domanda→lettura compreso. Ma i fatti che lo
**accendono** non erano mai usciti dal laboratorio: al gen489, in tutta la KB,
`frame_pattern/4`, `frame_operator_concept/2`, `semantic_entity/1`,
`semantic_class/1` e `phrase_form/4` avevano **zero fatti**, e vivevano solo nei
profili finti di `docs/labs/autocrescita-v3/`. È il moltiplicatore 3 del §−1
nella forma più pura: qui il motore era perfino scritto, mancava la spina.

**L'accensione, e la scelta che porta il peso.** Tutto derivato, mai elencato
(`kb/core/grammar.p0`, blocco gen490): l'operatore si deriva da `relation_verb` e
`clause_copula` — classi che crescono *parlando*, quindi insegnare «governs is a
relation verb» accende la lettura di «X governs Y» dal turno dopo; e l'**entità**
si deriva da `wiki_concept/3`, cioè dal corpus. Se le entità fossero enumerate il
corpus leggibile sarebbe chiuso per sempre; così **la lettura finanzia la
lettura**.

| | prima | dopo |
|---|---|---|
| resa su 10 pagine vere, 43 frasi | 0,13 fatti/frase | **0,26** |
| che cosa resta | atomi malformati (`pigment_found`) | `semantic_proposition` + `semantic_binding`, con provenienza |
| `wiki_concept` | 232 | **279**, e 5 domini nuovi |

**Domini nuovi:** geology, politics, economics, law, music — che il §P2 qui sotto
segnava come *«muro totale nella sonda»*. Le **fonti** si conservano
(`kb/learning/sources.tsv`), le pagine no: il vincolo del gen436 resta intatto.

### ⛔ Il prossimo passo, con precisione

La **domanda non risponde ancora dalla prosa letta**, ed è a due dita.
`what does photosynthesis produce` lega il frame di domanda —
`input_semantic_frame(current_turn, question, binary(produces), …)` è
dimostrabile — ma non passa `input_frame_unique/2`: più di una lettura lega, e il
design rifiuta *apposta* di scegliere in silenzio. Il ponte esiste già
(`input_frame_reading/4`): **manca l'unicità**, cioè una guardia che dica quale
lettura vince. È il mantra #14 applicato al lettore.

Accanto c'è la morfologia (P1.4): `produce` e `produces` devono essere lo
**stesso** operatore. La riga è verificata funzionante —

```prolog
linguistic_form($S, $V, en, common) :- relation_verb($V), lemma_candidate($V, $S).
```

— ma è tenuta **fuori** finché l'unicità non è risolta: allargare le letture
senza saperle decidere peggiora l'ambiguità invece di ridurla (mantra #19b).

**E la leva che resta aperta:** `relation_verb/1` ha **35 membri**. Ogni verbo
aggiunto lì accende la lettura di una famiglia di frasi intera, e si insegna
parlando. È il posto con il rapporto più alto fra riga scritta e prosa leggibile.

## §0-quater. ⛔ LA CATENA COMPILATA — il difetto strutturale trovato il 2026-09-03

> F., guardando `10-memory-knowledge.c`: *«questa catena così come è fatta
> risulta hardcoded, non è possibile aggiungere un membro a quella catena a
> runtime: non è stato rispettato il principio KB-first»*. E poi, un piano più
> su: *«è la catena di `&&` che deve diventare essa stessa una regola nella KB»*.

**È la voce con la leva più alta di tutta questa coda**, perché non chiude una
classe di frasi: apre il canale #1 della Gerarchia di Crescita per tutte.
Il mantra **#19** porta l'analisi completa; qui sta solo lo stato.

| | al gen488 | al gen489 |
|---|---|---|
| classi `*_lex*` da UN membro con nome seriale | 1245 | **598** |
| la parola `is` in quante classi diverse | 79 | 1 (`clause_copula`) |
| catene di condizione compilate | 213 | 204, **ognuna con il suo `TODO(kb-first, gen489)`** |
| motore per valutare una forma dichiarata | **nessuno** | `turn_pattern/3` + `turn_pattern_intent/2` |

**Che cosa è cambiato davvero, in una riga:** prima si poteva insegnare *un
membro di un ruolo che esisteva già*; ora si può insegnare **una forma nuova**,
congiunzione compresa — `tests/p0t/language/taught_turn_form.p0t` la insegna in
due condizioni e poi ne toglie una per far tornare il difetto.

**Come si riprende.** I 204 `TODO(kb-first, gen489)` sono una **coda di lavoro,
non un archivio** (§−1, moltiplicatore 1): chi tocca un ramo che ne porta uno lo
chiude lì, dove costerà meno di quanto costerà mai. Le 598 classi-seriale
rimaste sono i 27 gruppi eterogenei (il sito mette insieme ruoli diversi e il
nome onesto va trovato leggendo) più i 377 termini che una casa non ce l'hanno
ancora.

**Le tre trappole già pagate, da non ripagare:**
1. Mai collassare due seriali che stanno nella **stessa** condizione: se il sito
   li distingue, distingue i *membri*, non i ruoli.
2. Allargare una classe è allargare una **rivendicazione** — `entity_pronoun`
   con dentro «you» faceva chiedere *«What number should I use for «you»?»*.
3. `a`/`an` non è `the`: fondere l'indeterminativo col determinativo faceva
   tradurre «the dog runs» in «un cane corre».

## §0-ter. ⛔ SESSIONE ITALIANA 2026-09-03 — problemi rilevati parlando

Sessione di chat reale in italiano (configurazione `make chat`: profilo `agi.p0`,
`PARROT0_WIKI_FETCH=1`, `PARROT0_TOOLS=1`), ~25 turni. Tutto cio' che segue e'
stato OSSERVATO dal vivo, non dedotto dalla suite. Per ogni problema vale il
protocollo delle TRE SPECIE del §10 di `docs/plans/universal-comprehension.md`:
un carattere → variante di superficie, una parola piena → costruzione mancante,
nessuna variante vicina → varianza vera (si rende insegnabile).

**La regola per chi riparte da qui:** ogni voce indica il canale di
riparazione che la chiude — **addestramento verbale** (insegnare la forma
parlando, zero C), **metacomprensione** (parrot0 deve sapere di non sapere, e
sapere PERCHE'), **autocorrezione** (il ciclo §0-bis che ora si chiude). Una
voce che non nomina il canale non e' pronta per la coda.

### IT-1. ✅ CHIUSA (gen489) — La traduzione IT→EN risponde male invece di murare

```text
> come si dice gatto in inglese?    Possiamo parlare in entrambe le lingue — faro' del mio meglio.  ⛔
> traduci il cane corre in inglese   Possiamo parlare in entrambe le lingue — faro' del mio meglio.  ⛔
```

La capacita' EN→IT esiste e funziona (`translate` / `into italian` in
`phrase_canon`). Il gap e' il verso inverso: **manca il vocabolario IT→EN**
(`come si dice` non ha una `phrase_canon` che porti a mod_translate, e
`traduci` non e' un `imperative_opener` coperto). La risposta data NON e' un
muro: e' un **turno rubato** — un modulo rivendica una domanda di traduzione e
emette un generico «parliamo in entrambe le lingue». Peggiore di un muro, perche'
sembra una risposta.

- **Specie:** costruzione mancante (una parola piena: manca la forma d'ingresso
  italiana, non la macchina).
- **Canale: addestramento verbale** — insegnare la forma `come si dice` e
  `traduci` come porte verso mod_translate (una riga di `phrase_canon`/classe
  KB, zero C: il test del mantra #2).
- **Canale: metacomprensione** — il modulo che ha risposto non sapeva di non
  sapere: la condotta di cessione (`faculty_yield`) per la classe «traduzione
  con richiesta in italiano» va dichiarata, esattamente come il §2(a).
- **Ratchet:** riprovare la stessa coppia di frasi dopo l'insegnamento, e
togliere la forma insegnata → deve tornare il muro (ablazione).

### IT-2. ⛔ L'insegnamento a runtime riconosce l'intento ma non lega le variabili

```text
> la parola gatto significa cat      Ho trovato il separatore didattico, ma non riesco ad allineare
                                     le stesse variabili, una volta per lato, a ruoli che conosco.  ⛔
> gatto significa cat                Non posso ancora ancorare quella lezione: non ho una lettura
                                     univoca per @S cat @O.  ⛔
> the word zorbo means a small blue bird   (stesso declino in inglese)  ⛔
```

Il sistema RICONOSCE l'intento didattico e declina onestamente (bene: niente
frasi inventate), ma non chiude il ciclo impara→usa. La lezione «X significa Y»
nomina le variabili ma non riesce ad allinearle ai ruoli — la meta-comprensione
dell'atto didattico e' ferma al riconoscimento.

- **Specie:** nessuna variante vicina — varianza vera; il sistema si ferma
  (declino onesto) come MANTRA.md prescrive, ma la soglia di fertilita' per la
  classe «definizione» non e' raggiunta.
- **Canale: metacomprensione** — il declino deve dire PERCHE' la lezione non si
  ancora (quale lato non ha lettura), cosi' il teacher puo' riformulare; e il
  sistema deve sapere quale costruzione didattica gli manca.
- **Canale: autocorrezione** — quando il binding fallisce, il §0-bis deve
  poter provare la forma minimamente diversa della lezione (come per la
  superficie) invece di lasciare il teacher senza via.
- **Ratchet:** insegnare «X significa Y» in una forma che il sistema sa
  leggere, poi interrogare il fatto; retract → il fatto sparisce.

### IT-3. ⛔ La lezione con la copula memorizza fatti ibridi non interrogabili

```text
> un gatto e' un piccolo animale domestico   Imparato: gatto e' un small animal domestico.  ⛔
> che animale e' il gatto?                    Non capisco ancora.  ⛔
```

L'atto didattico con la copula FUNZIONA (memorizza), ma produce un fatto ibrido
italiano/inglese (`small animal domestico` — vocabolario mancante) e poi non lo
sa interrogare. E' la forma del difetto ricorrente: **parrot0 impara sotto un
nome che non sa piu' pronunciare** — viola l'invariante self-echo (tutto cio'
che dice, compreso cio' che «impara», deve essergli comprensibile).

- **Specie:** costruzione mancante (glosses mancanti: `animal`, `domestico`,
  `piccolo` non raggiungono i concetti canonici) — ma il difetto vero e' a
  monte: la resa dell'apprendimento non e' verificata.
- **Canale: addestramento verbale** — insegnare i glosses mancanti e la lezione
  in forma interamente italiana.
- **Canale: metacomprensione + autocorrezione** — prima di annunciare
  «Imparato», il sistema deve verificare che il fatto sia interrogabile con la
  stessa domanda (il self-echo come gate dell'apprendimento: `self_echo.p0t`
  esteso ai fatti appena appresi). Se la verifica fallisce, non annuncia:
  declina o si corregge.

### IT-4. Il meta-ricordo non interroga il registro del discorso

```text
> il mio numero preferito e' 42        Ricevuto: il tuo numero preferito e' 42.  ✅
> che numero ti ho detto?              Non ne ho di miei — sono parrot0…  ⛔  (lettura sbagliata)
> il mio colore preferito e' il blu     Imparato: color preferito e' un blu.  ⛔
> cosa ti ho detto?                    Non capisco ancora.  ⛔
```

La memoria dichiarativa funziona (il fatto si registra), ma la porta META
dell'handoff («che cosa ti ho detto» → `qual e'`) non raggiunge il registro:
`che numero ti ho detto?` viene letto come una domanda al MIO numero
preferito, e `cosa ti ho detto?` mura pur essendo una delle forme dichiarate in
`phrase_canon`. La famiglia «conversazione» della batteria di rinforzo
dovrebbe coprire esattamente questo.

- **Specie:** costruzione mancante (le varianti con `che numero`/`dove`/`quando`
  della cornice meta) piu' sospetto di lettura sbagliata (turno rubato).
- **Canale: addestramento verbale** — estendere la cornice meta per variante
  (`che numero ti ho detto` → `qual e'`), insegnabile parlando.
- **Canale: metacomprensione** — la lettura «qual e' il mio numero» che risponde
  al posto della domanda sul discorso e' un caso da `faculty_yield`/declino.
- **Ratchet:** dichiarare il fatto, interrogarlo, ritirare la forma → muro.

### IT-5. ✅ CHIUSA (gen489) — Proprieta' e coreferenza dopo l'asserzione: muro

```text
> il mio libro e' rosso          Ricevuto: il tuo libro e' rosso.  ✅
> di che colore e' il libro?     Non capisco ancora.  ⛔
```

Il libro e' stato appena nominato con il suo colore, e la domanda sulla
proprieta' dello stesso referente mura. Sono i gradini G2-G5 di
`docs/plans/universal-comprehension.md` (testa/proprieta', referente,
coreferenza) — G1 (il sintagma multi-parola) e' chiuso: `il libro rosso e' sul
tavolo` → `dove si trova il libro rosso?` → `tavolo` funziona. Qui manca il
percorso asserzione→proprieta' interrogabile.

- **Specie:** costruzione mancante (il fatto c'e' nel discorso, manca la lettura
  di proprieta' che lo agganci).
- **Canale: addestramento verbale** — la forma `di che colore e' X` e la lettura
  di proprieta' devono raggiungere il fatto asserito.
- **Canale: autocorrezione** — il fatto e' gia' nel discorso: la riparazione
  dovrebbe poter riprovare con la lettura di proprieta' invece di dichiarare il
  muro.

### IT-6. La generazione creativa italiana e' scoperta

```text
> raccontami una poesia            Non capisco ancora.  ⛔
> scrivimi una poesia sul mare      Non sono sicuro di aver seguito…  ⛔
> raccontami una barzelletta       Non capisco ancora.  ⛔
```

`raccontami` e' un imperativo enclitico riconosciuto (il gate
`enclitic_imperative.it.p0t` e' verde), ma la PORTA creativa non risponde in
italiano. Il generatore e' il modulo che il §3 (2) vuole rifatto — qui si vede
anche il lato vocabolario.

- **Specie:** costruzione mancante (forme/cue creative italiane) + varianza vera
  (il generatore e' da rifare, non rattoppare — §3 (2)).
- **Canale: addestramento verbale** — le forme italiane devono raggiungere le
  cue creative gia' esistenti, e le classi (poesia, barzelletta) devono avere
  membri insegnabili.
- **Canale: metacomprensione** — il declino generico «Non sono sicuro di aver
  seguito» va sostituito da un gap TIPIZZATO (specie di lacuna) che il teacher
  possa riparare parlando.

### IT-7. Forma residua: «dove si trova roma» legge part-of, non locativo

```text
> dove si trova roma?    Italia e lazio.  ⚠  (vero ma lettura sbagliata: risponde a «di cosa fa parte»)
```

Non e' un muro ma una lettura scambiata: la domanda locativa viene onorata dalla
relazione di appartenenza. Da tenere nel differenziale quando si tocca la
famiglia `located_in`/`part_of` (il §(f) del handoff ha gia' mostrato la
famiglia dei vincoli di categoria).

- **Specie:** costruzione mancante (ambiguita' di lettura fra due relazioni
  compatibili).
- **Canale: addestramento verbale** — la forma locativa deve vincere sulla
  relazione di appartenenza quando la domanda e' «dove si trova».
- **Canale: metacomprensione** — in caso di pareggio fra due letture, chiedere
  (come l'ambiguita' referenziale del §3 (6)), mai scegliere in silenzio.

### In sintesi — dove sta l'addestramento

La base dichiarativa italiana e' solida (fatti, aritmetica, memoria, locativo
con self-echo, G1 multi-parola) e il sistema non ha MAI inventato una risposta.
Ma il canale di crescita #1 dei mantra — l'insegnamento via prompt — non chiude
ancora il ciclo impara→usa (IT-2, IT-3), e due porte di vocabolario mancano del
tutto (IT-1 traduzione, IT-6 creativa). Le voci IT-1…IT-7 sono la coda; ognuna
dichiara il canale che la chiude.

## §0. L'obiettivo, con le parole di F.

> *«Il nostro obiettivo è l'espansione della comprensione. L'ostacolo che abbiamo
> scoperto sono i moduli obsoleti che rubano turni, e non possiamo addestrarli a
> lavorare meglio perché non sono KB-first.»*

E, prima:

> *«Dove la capacità c'è e manca la forma, si deve garantire l'insegnabilità.»*
> *«Tutto quello che trovi cablato in C va portato KB-first.»*
> *«Le latenze vanno studiate e ottimizzate con strategie opportune: cache,
> indici, kv hashing, exit condition.»*
> *«Non ti perdere con i test»* — i gate puntuali sì, la suite lunga la indice F.

## §1. Il numero che governa tutto — misurato, non stimato

Batteria held-out `tests/llmscore-probes/`, 70 domande, 7 classi. Il registro
completo è in **`docs/plans/quanto-manca.md`**, che è il documento da leggere
subito dopo questo.

```text
muri                0 / 70    0%     ⛔ METRO SATURO — non usarlo più
risposte buone     44 / 70   63%
risposte sbagliate  7 / 70   10%     forma giusta, contenuto sbagliato
turni rubati       19 / 70   27%     risposta SICURA ed ESTRANEA
```

> **La misura che discrimina ora è a tre bucket: buona | sbagliata | ESTRANEA.**
> Una risposta estranea è **peggiore di un muro**, perché un muro si vede e una
> risposta sicura no. Contarla a parte è il primo dovere del prossimo che misura.

**Il difetto dominante non è ignoranza.** Cinque delle dieci risposte peggiori
venivano da **due moduli**, e tutte avevano la stessa forma: *una facoltà
rivendica un turno che non sa onorare e, invece di tacere, emette qualcosa.*

## §2. Che cosa è stato costruito, in ordine di leva

### (a) ⭐ Il registro legge la condotta — 44 moduli inaddestrabili → 0

**È la mossa che sblocca tutto il resto.** La condotta *era già* conoscenza
(`faculty_yield/3`, `faculty_yield_force/3`, `faculty_force/2`), ma la leggevano
**sei** facoltà che se l'erano cablata dentro, su cinquanta. Per le altre
quarantaquattro la condotta **non esisteva**: nessun fatto poteva impedire a
`spell` di scandire una parola della domanda, perché `spell` non chiedeva niente
a nessuno.

Ora la lettura è del **registro**: il nome con cui un modulo è registrato è il
suo nome pubblico. Un modulo **nuovo nasce governabile senza scrivere niente**, e
anche gli stadi che rivendicano *prima* del registro (i «lead») leggono la
condotta — uno stadio che non la legge è uno stadio inaddestrabile.

Dimostrato su un modulo con zero condotta, in `registry_is_trainable.p0t`:

```text
> What would you do if you won the lottery?   ->  «Hey! I'm here…»        ⛔
!assert intent_cue(hypothetical_self_probe, "if you won")
!assert faculty_yield(chitchat, open, hypothetical_self_probe)
> What would you do if you won the lottery?   ->  «I don't have any of my own
                                                   — I'm parrot0, an AI…»  ✅
!forget …                                     ->  torna il difetto
```

Né la classe di cue né la condotta esistevano quando il binario è stato compilato.
**Permissivo per default**, e a costo nullo per chi non è governato.

⚠ **Lezione misurata, vale più delle righe:** governare **una** facoltà *sposta*
il turno rubato invece di chiuderlo. Tolto `analysis_family` da «Explain what you
are…», il turno è finito a `role`: *«Alright — I am Without now»*. **Un turno
conteso non ha un ladro: ne ha una fila**, e la condotta va dichiarata lungo
tutta la fila.

### (b) L'arco connettivo dinamico — la conoscenza detta in una forma, letta in un'altra

Il difetto: `teflon is a molecule` memorizza il fatto **unario**, chi enumera
legge la relazione **binaria**. Tre turni a buon fine, nessuno utile.

La prima cura fu *una regola scritta a mano per quella coppia*. F. l'ha fermata —
è il mantra #2 un livello più su. Ora l'arco è un **fatto**, con i predicati come
**argomenti**: `knowledge_arc(category_member, 0, category)`,
`knowledge_alias/2`. Il motore legge una *posizione* e una *guardia*.

⚠ **Vincolo verificato del motore:** la testa di clausola **non ammette un
predicato variabile** (`strcmp(R->head.pred, g->pred)` in `kb.c`). Perciò l'arco
*dichiarato* sta in KB e il *percorso* in C. Se un giorno la testa sarà
variabile, i fatti sono già scritti nella forma giusta.

**Effetto composto, non previsto:** «tell me an animal that lives in water» ora
risponde *Amphibian*, ed è vero. Metà dei membri di `animal` era invisibile
all'enumerazione. **Un arco non aggiunge una capacità: moltiplica quelle che ci sono.**

### (c) L'insegnamento di ordine superiore — una relazione FRA relazioni

```text
> if x contains y then y is part of x
    Learned rule: part_of($V2, $V1) :- contains($V1, $V2).
> the vault contains the crown
!query part_of(crown, vault)     ->  dimostrabile
> what is the ring part of       ->  Box.
```

Prima produceva `part($V2) :- holds(x_contains_y)` **e la annunciava come
appresa**. Tre difetti: la lezione aveva un lettore di clausole *tutto suo*; la
lettura di appartenenza *ingoiava metà clausola*; la regola risultante aveva una
variabile di testa che il corpo non nomina. Aggiunto `kb_retract_clause` — una
lezione che non si può togliere non è una lezione.

### (d) La metacomprensione ha un cancello

> **Una risposta che viola un vincolo esplicito del turno non è una risposta.**

`size_constraint/2` + `size_constraint_count/2`, controllo **post-dispatch**: nel
registro la facoltà declina e il turno prosegue; in `turn_done`, dove ogni stadio
passa, il gap prende il posto del testo. **Non dentro ciascuna facoltà**, che
sarebbe di nuovo l'elenco degli incidenti.

### (e) L'insegnabilità come requisito, non come effetto

```text
> sbloccami x + 1 = 6              (muro)
> sbloccami is an equation filler  Learned.
> sbloccami x + 1 = 6              x = 5.
> !forget equation_filler(…)       (muro di nuovo)
```

La lista dei 22 riempitivi è andata **intera** in KB — un trasloco, non una
copia. Effetto collaterale che vale più della patch: **`matches_any` è stata
tolta**, la funzione la cui unica firma era *(stringa, elenco di letterali in C)*.
Chi vorrà rifare una lista cablata dovrà riscriverla.

### (f) Il residuo di un turno è un vincolo

`tell me a country in asia` → **andorra**; `in europe` → **argentina**. Tre
risposte false dette come fatti, mentre la KB sapeva verificarle. Ora
`member_satisfies/2` verifica via `kb_fact/2` — nessun nome di relazione nel C.

### (g) F03 — un riferimento si lega a un ruolo, e un pronome non è mai un'entità

`reviewed(luca, it)` era **dimostrabile**: un pronome scritto in KB come entità.
Chiuso con `reference_binding/1` (`most_recent`, `distinct_in_frame`,
`role_parallel`) e con la lettura strutturale dell'**asserzione**
(`turn_reading/3` → `turn_illocution($T, assertion)` → `faculty_yield_force/3`).

## §3. Il prossimo lavoro, in ordine

1. **⭐ Continuare a dichiarare la condotta lungo le FILE di contesa.** È il
   lavoro con il rapporto più alto e ora è **pura conoscenza**: si legge il
   registro al §2 di `quanto-manca.md`, si prende un turno rubato, si chiede
   `who answered?`, si dichiara `faculty_yield`, si rimisura. Nessuna riga di C.
2. **Il generatore di storie va RIFATTO, non rattoppato.** Giudizio di F.
   accolto: *«sceglie il protagonista per forma, senza applicare la comprensione
   universale»*. Il rifacimento ha reso il difetto **visibile** invece che
   risolverlo — ora il tema si legge davvero e lo schema, fatto per un nome di
   una parola, lo ripete in ogni casella. Riempire uno schema con token raccolti
   dal turno non è raccontare. Posto giusto: `docs/plans/generative.md` +
   `generative-prolog.md`.
3. **⛔ Le latenze — chiesto da F. e NON fatto.** Vedi §L più sotto. Sappiamo la
   causa (`kb_fact/2` con predicato libero è O(fatti) per costruzione) e la cura
   strutturale (**indice per termine / kv hashing**: il censimento in `kb.c`
   indicizza per *predicato*, non per *argomento*).
4. **Il DOMINIO del contesto** nell'ordine superiore — *«in certi contesti che
   una cosa la contiene vuol dire che ne è una parte»*. Contenere significa
   essere parte per una scatola, non per un fiume e i pesci. **Una regola che
   vale ovunque è una regola che nessuno può correggere parlando.**
5. Le due letture aritmetiche sbagliate («metà di un numero è 14» → 7;
   «4 confezioni da 6» → 10); `this shape` nel sillogismo; **l'iniziativa**
   («fammi una domanda»), unica capacità davvero *assente* del lotto.
6. L'ambiguità referenziale: due antecedenti compatibili → **deve chiedere**,
   oggi sceglie in silenzio. Dichiarata dentro `reference_binding.p0t`.

## §4. ⛔ Esperimenti fatti e RITIRATI — non rifarli

Scritti anche nel codice, sul posto:

- **Togliere il lead di `analysis_family`.** Una famiglia metodologica non è più
  specifica di nessun modulo, quindi la precedenza posizionale sembrava
  sbagliata. Misurato: **due turni peggiorati** — la famiglia stava
  **proteggendo** quei turni da rivendicazioni peggiori. *Togliere un guardiano
  non è togliere un difetto.*
- **`subject_guard/1` sulla testa del soggetto d'analisi.** Non toccava il turno
  da curare e ne rompeva un altro: «advice for someone starting a new job» →
  *«From what I know, a good plan is:»* **e nient'altro**. «someone» è cattivo
  soggetto per un *frame* e tema legittimo per un'*analisi*. *Un boilerplate si
  legge, una promessa vuota no.*
- **Un'exit condition che costa quanto ciò che evita.** Il pre-controllo «il
  valore compare da qualche parte?» era anch'esso una scansione a predicato
  libero: *aggiungeva* lavoro. La cura era **invertire il join** (7,9 s → <1 s).

## §L. ⛔ LE LATENZE — chiesto da F. e NON fatto

> F.: *«le latenze vanno studiate e ottimizzate con strategie opportune: cache,
> indici, kv hashing, exit condition»*.

Fatto in parte, e la parte mancante è quella strutturale. Che cosa si sa:

- **La causa profonda ha un nome:** `kb_fact/2` con il **predicato non legato** è
  O(fatti) *per costruzione*. Il censimento in `src/kb.c` indicizza per
  **predicato**, non per **argomento**, quindi «esiste una relazione fra M e V?»
  non può che scandire.
- **Che cosa ha funzionato:** *invertire il join*. «Questo membro è legato al
  valore?» posta N volte diventa «**chi** è legato a questo valore?» posta una
  volta (`related_to/2`) più un'intersezione: **7,9 s → sotto 1 s**. Toglie il
  fattore N, **non** l'O(n).
- **Che cosa NON ha funzionato, e perché è istruttivo:** un pre-controllo «il
  valore compare da qualche parte?» era anch'esso una scansione a predicato
  libero. *Un'exit condition che costa quanto ciò che evita non è
  un'ottimizzazione.*
- **Exit condition non lossy che ha funzionato:** provare il legatore condiviso
  solo se il segmento contiene almeno due termini di regola — è la *stessa*
  condizione che la funzione applica dopo, anticipata. Non riduce ciò che vede.
- **Cache degli schemi di lettura:** `extract_frame/2` è fatti *più* regole che
  li generano, ri-derivati da ogni consumatore. Ora si derivano una volta per
  **revisione della KB** (`kb_revision`), non su un timer: una relazione
  insegnata adesso è visibile al turno stesso.
- ⚠ **Questo ciclo ha AGGIUNTO latenza:** `turn_reading/3` esegue la fase pura
  del lettore a ogni turno. È resa **opt-in su un consumatore dichiarato** (senza
  `faculty_yield_force/3` il costo sparisce), ma il consumatore c'è.

**La cura strutturale resta da fare: un indice per TERMINE (kv hashing) in
`src/kb.c`.** È il lavoro che chiude la classe invece di spostarla.

⛔ **Regola di F. che resta:** *non alzare i budget.* Un budget alzato nasconde il
fenomeno invece di misurarlo. I blocchi nuovi dichiarano `!timeout N` **con la
misura scritta accanto**, che è una cosa diversa da un budget gonfiato in
silenzio.

## §5. Metodo, vincolante

> **⛔ Il protocollo delle TRE SPECIE DI LACUNA è in
> `docs/plans/universal-comprehension.md` §10.** Davanti a un muro si prova la
> frase **minimamente diversa**: la distanza fra ciò che funziona e ciò che non
> funziona *è* il tipo di lacuna, e il tipo decide se era prevedibile.
> **Un carattere** → variante di superficie: si genera dalla classe chiusa e si
> disambigua per contesto. **Una parola piena** → costruzione mancante: si
> completa la classe. **Nessuna variante vicina** → varianza vera: non si
> anticipa, si rende insegnabile. Con la checklist e la regola che è costata di
> più: **un rosso che compare due volte va chiuso al secondo avvistamento** — il
> registro dei rossi è una coda di lavoro, non un archivio.

- **Il differenziale prima di attribuire un rosso.** Più volte in questi cicli un
  rosso sembrava mio ed era un **timeout** del debito di latenza, o un test che
  asseriva un mondo peggiore.
- **Un gate che fallisce non è una capacità mancante.** Tre volte il codice aveva
  ragione e il test torto: l'amputazione della KB, `!query` invece di `!query!`,
  e un'asserzione che pretendeva il gap dove ora c'è una risposta giusta.
- **Un gate non si aggira: si corregge** quando descrive un mondo peggiore — e si
  scrive nel commit che è stato corretto e perché.

---

# handoff 2026-09-02 (giorno) — superato dall'handoff in testa

> **Come si riprende:** «continua da questo file». Leggi §0 e §1, poi §4 «Il
> prossimo lavoro». Sotto c'è l'handoff del 2026-09-01, che resta valido come
> dottrina (mantra #17, #18, l'errore da non rifare) ma non più come punto di
> ripresa.

> **Direzione successiva di F. (2026-09-02):** non fermare la marcia sui test
> rossi preesistenti; la priorita' e' far avanzare le abilita' di comprensione.
> I gate nuovi restano obbligatori e puntuali, la suite lunga no.


## ⛔ QUANTO MANCA — la valutazione misurata (2026-09-02)

> Domanda di F.: *«quanto manca affinché parrot0 sia addestrabile da un LLM a
> rispondere bene alla maggioranza delle domande, e quanto manca per finire la
> metacomprensione»*. Risposta misurata, non opinata: **`docs/plans/quanto-manca.md`**.

Batteria held-out `tests/llmscore-probes/` — 70 domande, 7 classi:

```text
muri                0 / 70    0%     (baseline della campagna: 31% — METRO SATURO)
risposte buone     44 / 70   63%
risposte sbagliate  7 / 70   10%     forma giusta, contenuto sbagliato
turni rubati       19 / 70   27%     risposta SICURA ed ESTRANEA alla domanda
```

**⛔ Non usare più il tasso di muro come metrica: è a zero e non discrimina.** La
misura che discrimina ora è a tre bucket, e il terzo è quello che conta:
**buona | sbagliata | estranea**. Una risposta estranea è *peggiore* di un muro,
perché un muro si vede e una risposta sicura no.

**Il difetto dominante non è ignoranza.** Cinque delle dieci risposte peggiori
vengono da **due moduli** (`analysis_family` 3, `gen` 2), e hanno tutte la stessa
forma: *una facoltà rivendica un turno che non sa onorare e, invece di tacere,
emette qualcosa.* Il registro completo con il modulo per ciascuna è al §2 del
piano.

**Risposta 1 — addestrabilità da LLM.** Il 27% mancante **non è insegnabile con
dei fatti**: nessun fatto nuovo impedisce ad `analysis_family` di rispondere a
«descrivi un'alba in una frase». Ma la condotta è *diventata conoscenza* in
questo ciclo, quindi la risposta esatta è: **manca applicare lo strato di
condotta alle cinque facoltà del registro §2** — cinque moduli, non cinquanta.
Fatto quello, un LLM corregge il resto **parlando**.

**Risposta 2 — metacomprensione.** È *sapere quando non si sa*, e il numero la
misura: 19 turni su 70 in cui parrot0 non sapeva di non sapere — non un muro, un
template. Il pezzo mancante ha un nome solo, ed è ora **costruito**:

> **Una risposta che viola un vincolo esplicito del turno non è una risposta.**

`size_constraint/2` + `size_constraint_count/2` in `turn-frames.p0`, con il
controllo **post-dispatch** (in `turn_done`, dove ogni stadio passa) e non dentro
ciascuna facoltà — altrimenti sarebbe di nuovo l'elenco degli incidenti. Il
registro declina e prosegue; i *lead*, che restituiscono e basta, ricevono il gap
al posto del testo. Ratchet: `tests/p0t/conversation/turn_size_constraint.p0t`.

⚠ **Onestà sulla resa:** lo strato cattura **1 dei 19** oggi, perché è la sola
misura dichiarata finora. Il valore non è il numero: è che una misura nuova — «in
due paragrafi», «en una frase» — costa **due righe di KB** e vale per ogni
facoltà insieme, comprese quelle che nessuno ha ancora scritto.

**L'ordine di lavoro** è al §5 di `docs/plans/quanto-manca.md`. In sintesi:
`analysis_family` dichiara la sua condotta → `gen` smette di prendere il verbo
imperativo come protagonista («Tell was a mysterious Tell») → le due letture
aritmetiche sbagliate → `this shape` → l'iniziativa.

## ⛔ AGGIORNAMENTO 2026-09-02 (sera) — F03 chiusa, e il dialogo reale

> Ordini di F. in questo ciclo, tutti vincolanti:
> 1. **niente sessioni lunghe di test** — gate puntuali sì, la suite la indice lui;
> 2. **massimizzare il corpus** e portare parrot0 alla comprensione universale;
> 3. **dove la capacità c'è e manca la forma, si deve garantire l'insegnabilità**;
> 4. **tutto quello che è cablato in C va portato KB-first**;
> 5. **le latenze vanno studiate e ottimizzate** (cache, indici, kv hashing,
>    exit condition) — vedi §L, ancora da fare.

### F03 — chiusa (commit `a4eba76`)

La matrice preregistrata del §6 è stata eseguita **una coordinata per volta prima
di scrivere una riga**, ed è servita: ha trovato tre difetti, e il peggiore non
era un muro.

| sonda | prima | ora |
|---|---|---|
| `He reviewed it` | `Learned: luca reviewed it.` ⛔ | `reviewed(orsolo, dossier)` |
| `He reviewed the draft` | `Luca.` (risponde a una domanda mai fatta) | impara |
| `Luca reviewed it` | `Learned: luca reviewed mira.` | `reviewed(orsolo, dossier)` |
| `He reviewed it after lunch` | muro | ✅ **la frase del corpus** |

Le tre cause, tutte diverse:

1. **Il binder risolveva il riferimento solo nel PRIMO slot** e negli altri lo
   lasciava passare come atomo: `reviewed(luca, it)` era *dimostrabile*. Un
   pronome scritto in KB come entità e annunciato come appreso — mantra #7.
2. **`answer_frame` rivendicava un'asserzione.** Mancava la lettura della forza:
   l'asserzione non ha *cue* (marcarle richiederebbe la lingua intera) ma ha una
   **struttura** — lega un frame dichiarativo completo. Ora
   `turn_reading/3` → `turn_illocution($T, assertion)` → `faculty_yield_force/3`.
3. **I candidati del discorso si raccoglievano per MAIUSCOLA**, cioè per
   ortografia: «Mira» e «Luca» entravano, «draft» no, e «He» ci finiva dentro.
   Metà del discorso era invisibile.

La politica di legatura è in KB (`reference_binding/1`): `most_recent`,
`distinct_in_frame`, `role_parallel`. Il **parallelismo di ruolo** è ciò che fa
leggere «He reviewed the draft» con He=persona senza inventare generi, animatezza
o un sistema di tipi: il vincolo è strutturale. Ratchet:
`tests/p0t/conversation/reference_binding.p0t`, 12 assert.

**Coordinata ancora aperta e dichiarata dentro il gate:** due antecedenti
compatibili («Vantel sent the dossier to Orsolo. He reviewed it») hanno due
letture e parrot0 ne sceglie una **in silenzio**. Deve chiedere. La cura è la
politica di ambiguità, non un golden.

### Il dialogo reale di F. — tre difetti di tre specie (commit `601be52`)

```text
>>> come stai                          Sto bene, grazie. ✅
>>> fammi una domanda semplic          Non capisco ancora.
>>> x + 1 = 6, x = ?                   Non sono sicuro di aver seguito.
>>> dimmi una molecola con l'ossigeno  Non capisco ancora.
```

**(a) `x + 1 = 6, x = ?`** — `solve x + 1 = 6` funzionava già. Mancavano: un
segmento col lato destro vuoto (o il solo `?`) **nomina l'incognita** invece di
essere una seconda equazione; e ventidue riempitivi cablati nel C in due lingue.
La lista è andata **intera** in `equation_filler/1` — un trasloco, non una copia.
Il requisito #3 di F. è ora un gate:

```text
> sbloccami x + 1 = 6              (muro)
> sbloccami is an equation filler  Learned.
> sbloccami x + 1 = 6              x = 5.
> !forget equation_filler(sbloccami)
> sbloccami x + 1 = 6              (muro)
```

**Effetto collaterale che vale più della patch:** `matches_any` — la funzione la
cui unica firma era *(stringa, elenco di letterali in C)* — è rimasta senza
chiamanti ed è stata **tolta**. Chi vorrà rifare una lista cablata dovrà
riscriverla, e a quel punto la domanda «perché non è un fatto?» si pone da sola.

**(b) `mod_namestart` — il difetto peggiore di tutto il ciclo.** Non un muro:

```text
tell me a country in asia    ->  andorra.
tell me a country in europe  ->  argentina.
tell me a country in africa  ->  australia.
```

Tre risposte **false** dette come fatti, mentre la KB sapeva verificarle. La
guardia esistente elencava tre parole — border, neighbour, neighbor — cioè i
sintomi del bug del gen240, e non poteva vedere «in asia» né «che vive
nell'acqua». La lettura giusta non nomina niente:

> **Se dopo la categoria resta del turno, quel residuo È un vincolo: o lo si
> verifica, o si tace.**

`member_satisfies/2` lo verifica via `kb_fact/2` — nessun nome di relazione nel
C, e una relazione insegnata domani vale come vincolo lo stesso giorno.
E **tacere non bastava**: cedendo in silenzio, «tell me an animal that lives in
water» finiva a `personal` che rispondeva *«Got it, I'll remember that.»* — la
domanda diventava una cosa da ricordare. Ora il gap si **dichiara**.

Su segnalazione di F. («namestart è molto opinabile»): le **sette** classi
`intent_cue` con un membro ciascuna sono diventate una,
`name_instance_request`. Erano nate una per incidente, e la prova è che
«tell me an» non c'era: entrava solo perché «tell me a» ne è un prefisso.

**(c) `dimmi una molecola con l'ossigeno`** — non mancava una macchina, mancava
il **corpus**. Aggiunta chimica vera in `world-facts.p0` (13 molecole, 10
elementi, `contains/2`, formule) + i glosses italiani. Il turno passa dalla
**stessa via generale** di «tell me a country in asia», senza nessun ramo per la
chimica. Ratchet: `tests/p0t/knowledge/instance_under_constraint.p0t`, 18 assert.

### ⛔ §L — LE LATENZE, il lavoro chiesto e non ancora fatto

F.: *«le latenze vanno studiate e ottimizzate con strategie opportune: cache,
indici, kv hashing, exit condition»*. **Non è stato fatto** e va fatto per primo
o subito dopo l'iniziativa. Quello che si sa già:

- il turno lento **non è lento in isolamento**: diventa lento **dopo N turni
  nella stessa sessione**. È funzione dei fatti accumulati, non della frase;
- perciò i rossi si concentrano **in fondo ai file** e sembrano rotture logiche
  mentre sono scadenze di budget. Nel differenziale di questo ciclo su
  `language/`+`knowledge/` **le uniche differenze erano timeout** (linee 49, 152,
  156 di `taught_lexicon.p0t`): nessuna differenza di logica;
- ⚠ questo ciclo **ha aggiunto** latenza: `turn_reading/3` esegue la fase pura del
  lettore a ogni turno. È già resa **opt-in su un consumatore dichiarato** (se
  nessuna facoltà dichiara `faculty_yield_force/3`, il costo sparisce), ma il
  consumatore c'è, quindi il costo si paga;
- **regola di F. che resta:** *non alzare i budget*. Un budget alzato nasconde il
  fenomeno invece di misurarlo.

### Il prossimo lavoro, in ordine

1. **`fammi una domanda semplice`** — l'ultimo muro del dialogo di F., e non è un
   difetto: è una **capacità mancante**. parrot0 deve *fare* una domanda. C'è già
   `docs/plans/initiative.md` e il piano dello spazio negativo della KB
   (question-emergence). È il pezzo di comprensione universale più grosso rimasto.
2. **§L, le latenze** — cache, indici, kv hashing, exit condition.
3. **L'ambiguità referenziale** (due antecedenti compatibili → chiedere).
4. **Continuare la caccia al cablato in C** (ordine #4 di F.). Il prossimo
   candidato noto: l'elenco dei file KB caricati a mano in `99-registry.c`.
5. La sessione lunga di test, **quando F. la chiede**.

## ⛔ HANDOFF OPERATIVO 2026-09-02 — oltre il binario, poi dentro F03

Questo e' il punto di ripresa per un agente nuovo. Non richiede di ricostruire
la sessione per intuito: sotto ci sono causa, modello mentale, invarianti,
prossima matrice e stop condition. Prima di modificare una riga leggere, per
intero e in quest'ordine:

1. `MANTRA.md` e `PRINCIPLES.md`;
2. `LEARN_PROTOCOL.md`;
3. `docs/plans/apprendimento-assistito.md`, in particolare M3, M4, M6, M7;
4. `docs/labs/apprendimento-assistito/2026-09-02-gd2-costruzioni-ternarie.md`;
5. il dialogo `gd1_012` nel corpus
   `docs/labs/apprendimento-assistito/2026-08-31-gd1-dialogue-corpus.tsv`.

### 1. Il risultato appena acquisito — non ridurlo a «supporta tre argomenti»

Il lettore supportava gia' frame fino a `P0_MAX_SLOTS = 8`. Il limite era in
un altro percorso: **la frase con cui il teacher insegna una costruzione**.
`p0_explicit_pattern` conteneva `vars[2]`, `seen[2]` e decideva da solo che la
prima variabile del target fosse `@S` e la seconda `@O`.

Era un difetto doppio:

- il teacher non poteva dire «X manda Y a Z significa X sent Y to Z»;
- anche togliendo il numero due, un target non canonico avrebbe ricevuto ruoli
  dall'ordine delle parole invece che dalla lettura gia' nota.

La cura non e' «2 -> 8». La cura e':

```text
lato target della lezione
  -> stessi extract_frame/2 del lettore
  -> stesso p0_frame_bind
  -> ruoli opachi osservati nel frame bersaglio
  -> quei ruoli vengono riportati sulle stesse variabili nella sorgente
```

Percio' ora funzionano entrambe:

```text
X manda Y a Z significa X sent Y to Z
X dispatches Y to Z means Z receives Y from X
```

Nel secondo caso il target `Z receives Y from X` e' una costruzione appresa un
turno prima e porta i ruoli nell'ordine `T/O/S`. La nuova sorgente viene comunque
registrata `@S dispatches @O to @T`. Questo e' il gate profondo: se passasse
soltanto il target canonico, il numero sarebbe cresciuto ma il modello sarebbe
rimasto posizionale.

### 2. Modello mentale del codice — quattro stadi, quattro responsabilita'

| stadio | dove | responsabilita' | non deve fare |
|---|---|---|---|
| geometria | `p0_lesson_variables` | stesse variabili una volta per lato | assegnare ruoli |
| menzione | copia `p0lessonvarN` | impedire che `a` variabile sia usata come articolo | cambiare la lezione salvata |
| allineamento | `p0_align_explicit_lesson` | offrire il target ai frame vivi e rifiutare l'ambiguita' | conoscere verbi, lingue o arita' |
| eredita' | `p0_lesson_source_pattern` | riapplicare alla sorgente il ruolo osservato | decidere che S/O/T significhino agente/oggetto/destinatario |

La copia `p0lessonvarN` e' meccanica interna ed effimera. E' necessaria perche'
le variabili sono **menzionate** nella lezione: dopo «a is a rule_variable», il
token `a` e' contemporaneamente una variabile didattica e un articolo inglese.
Passarlo nudo al lettore svuotava il primo slot. Non togliere questa copia e non
risolvere il caso cablando l'articolo `a`.

`p0_construction_target` resta il cancello finale: il target deve avere una sola
lettura. Una lezione non sceglie in silenzio fra due frame. `mod_forget` richiama
lo stesso parser della lezione: cambiare solo il percorso di add senza cambiare
quello di retract rompe l'ablazione parlata.

### 3. Invarianti da tenere fermi

1. **Nessuna arita' linguistica nel C.** `P0_MAX_SLOTS` e' soltanto la dimensione
   meccanica dell'appoggio, condivisa dal lettore e dall'atto didattico.
2. **Nessun ruolo per posizione.** Il target gia' compreso nomina i ruoli; il C
   trasporta lettere opache.
3. **Nessun vocabolo del gate nel C.** `sent`, `to`, `manda`, `receives` e
   `dispatches` compaiono solo nel test/report.
4. **Target noto e univoco.** Un target assente o con due letture resta un gap;
   non si prende il primo risultato di `kb_match`.
5. **Stesse variabili, esattamente una volta per lato.** Una variabile mancante,
   duplicata o nuova sul lato sorgente non lascia una costruzione parziale.
6. **Domanda e asserzione condividono il frame.** Non aggiungere un answer-frame
   per ogni slot: `p0_frame_bind` riconosce gia' la question word nello slot.
7. **Il retract toglie la capacita', non la storia.** I fatti prodotti mentre la
   costruzione era viva restano; il frame operativo sparisce.
8. **Le rese restano KB.** Il nuovo messaggio di forma non supportata e' in
   `kb/core/responses.p0`, non in uno `snprintf`.

### 4. Evidenza corrente e comandi esatti

Baseline GD1 appena misurata, nessun `/save`:

```text
60 dialoghi / 360 turni / 0 errori di allineamento
230 muri / 124 move_match
F03: 24 muri su 30, 6 move_match su 30
```

Gate nuovo:

```sh
make build
make test-engine
./bin/parrot0 --test tests/p0t/language/assisted_construction_ternary.p0t
```

Esito corrente: `33 passed`. Nell'ambiente Codex il demone aperto da
`make test-engine` puo' morire alla fine della singola shell: se il client non
trova `obj/test-engine.sock`, eseguire target e client nello **stesso comando**.
Questo e' un limite dell'ambiente di esecuzione, non di parrot0.

Compatibilita' mirata:

```sh
make test-engine
./bin/parrot0 --test tests/p0t/language/assisted_construction.p0t
```

Esito corrente: `65 passed, 1 failure`. L'unico rosso e' anteriore a questo
incremento: il test cerca virgolette ASCII nella resa di `denotes`, mentre la
voce corrente usa «caporali» e una cornice diversa. Per ordine di F. non
inseguirlo e non cambiare la risposta soltanto per rendere verde il golden.
La suite lunga non e' stata eseguita e non va eseguita finche' F. non la chiede.

### 5. Mappa dei file del checkpoint

- `src/brain/10-memory-knowledge.c`, intorno a `p0_lesson_variables`:
  meccanica generale della lezione, binder condiviso, ambiguita'.
- `kb/core/grammar.p0`, `frame_role_order/2`, `ternary_relation_verb/1`,
  `link_word/1`, `extract_frame/2`: conoscenza che genera i frame.
- `kb/core/responses.p0`, `construction_shape_unsupported`: voce del gap.
- `tests/p0t/language/assisted_construction_ternary.p0t`: ratchet causale.
- `tests/p0t/language/assisted_construction.p0t`: compatibilita' binaria,
  inversione, pivot e variabili insegnate.
- `docs/labs/apprendimento-assistito/2026-09-02-gd2-costruzioni-ternarie.md`:
  report, metriche, conteggi e confine.

Il worktree contiene anche il checkpoint precedente su forza del turno ed
enclitiche (`illocution_comprehension.p0t`, `enclitic_imperative.it.p0t` e KB
collegata). Non attribuire quelle righe al limite ternario e non cancellarle.

### 6. Prossimo esperimento esatto — scomporre `gd1_012`, non «fare coref»

Il dialogo da riprendere e':

```text
Mira sent the draft to Luca.
He reviewed it after lunch.
Who does he refer to there?
I meant Luca reviewed the draft.
And when?
Who originally sent it?
```

Dopo due lezioni naturali, il primo turno e la domanda diretta sono verdi:

```text
sent is a ternary relation verb
the word to is a link word
Mira sent the draft to Luca       -> sent(mira, draft, luca)
Who sent the draft to Luca?       -> mira
```

**Non saltare da qui a un modulo “coreference”.** Prima preregistrare questa
matrice in un processo fresco, una coordinata per volta:

| sonda | che cosa isola |
|---|---|
| `Luca reviewed the draft` | licenza lessicale/frame binario |
| `He reviewed the draft` | pronome nel primo ruolo |
| `Luca reviewed it` | pronome nel secondo ruolo |
| `He reviewed it` | composizione dei due riferimenti |
| `Luca reviewed the draft after lunch` | confine del secondo ruolo / tempo |
| `He reviewed it after lunch` | frase corpus: composizione completa |
| `Who reviewed the draft?` | domanda sul primo ruolo |
| `What did Luca review?` | dualita' interrogativa/morfologia |
| `When did Luca review it?` | ellissi temporale e terzo ruolo |
| due uomini + `he` | ambiguita': deve chiedere, non scegliere |

Per ogni riga conservare risposta, modulo vincitore, fatto candidato, referenti
prima/dopo e residuo. Variare prima il pronome, poi il complemento temporale:
se si cambiano insieme non si sapra' quale coordinata e' causale.

### 7. Ipotesi da falsificare prima di implementare

Nel binder c'e' ancora questo limite dichiarato:

```c
/* Il pronome si risolve solo nel primo slot. */
```

E' una **ipotesi**, non ancora la patch da fare. Prima verificare:

1. se il secondo slot arriva davvero al binder come `it` o viene inglobato con
   `after lunch`;
2. se il referente necessario esiste nel discourse space e con quale ruolo;
3. se `last_entity` e' troppo povero per distinguere draft, Mira e Luca;
4. se la domanda fallisce per coreferenza, morfologia (`reviewed`/`review`) o
   assenza di chiusura interrogativa;
5. se una correzione «I meant Luca…» deve rivedere il fatto o soltanto il
   referente, usando `supersedes_in/3` invece di accumulare.

Solo dopo la matrice scegliere il collo condiviso. Se e' davvero «pronome in
qualunque ruolo», la forma generalizzabile non e' un secondo `if (nslots == 1)`:
e' una relazione fra **slot del frame, superficie riferente e candidati nel
discorso**, con politica di ambiguita' in KB. Deve essere possibile insegnare un
nuovo membro della classe riferente e ritrarlo senza rebuild.

### 8. Gate minimo del prossimo incremento

Non dichiarare chiuso F03 con il solo `He reviewed it`. Servono:

1. Replay sulla frase corpus;
2. `Transfer@3=3/3`: tre relazioni e nomi held-out;
3. pronome nel primo e nel secondo ruolo separatamente;
4. composizione con un frame ternario e con un'entita' multi-parola;
5. contrasto ambiguo con almeno due antecedenti compatibili: chiarimento, non
   scelta silenziosa;
6. riferimento distante almeno tre turni;
7. correzione che supersede la lettura precedente senza cancellare il testo;
8. crescita e retract di una superficie riferente appresa a runtime;
9. nessuna stale reading che continui a rispondere dopo la correzione;
10. rimisura prima il mini-dialogo intero; il probe 360 si rilancia solo quando
    la catena chiude da capo a fondo.

### 9. Trappole gia' pagate — non ripeterle

- **Non aggiungere `sent`, `to` o `reviewed` al C.** I primi due si insegnano
  gia' parlando; il terzo deve seguire lo stesso canale.
- **Non scrivere la risposta di `gd1_012`.** Il corpus e' misura, non frasario.
- **Non usare il totale 230 come feedback stretto.** Una catena puo' guadagnare
  una coordinata e restare muro al turno successivo; si misura prima il
  verticale.
- **Non assumere che una risposta non-vuota sia comprensione.** Il probe
  `move_match` e' euristico; leggere semanticamente i transcript.
- **Non montare una KB amputata per “isolare”.** Usare nomi held-out, non
  `PARROT0_WORLD_FACTS=0`, quando la capacita' dipende dalla KB reale.
- **Non dequotare due volte lo stesso buffer.** `kb_dequote` modifica in place;
  il difetto e' gia' costato un ciclo (§1(a)).
- **Non salvare le fixture del gate.** Questo checkpoint e'
  `meta-capability-only`: `W=0`, `S=0`, `X=0`.
- **Non riparare ora la resa `sent(mira, draft, luca)`.** E' il debito M20,
  reale ma ortogonale. Prima conservare la nuova comprensione; la voce naturale
  richiedera' un renderer ternario KB-backed, non un `printf`.
- **Non creare un indice parallelo delle costruzioni.** `extract_frame/2` e il
  binder sono gia' il punto condiviso; duplicarli ricrea la divergenza appena
  tolta.

### 10. Stato protocollare del checkpoint

```text
Stato: meta-capability-only
W=0  L=0  C_salvate=0  P=0  O=0  X=0
Meccaniche generali ampliate: 1
/save: non eseguito
B0/R0: 37778/2640
B1/R1: 37778/2640
FreshProcessRecall: n/a, nessuna conoscenza persistita
```

Una nuova sessione reale potra' salvare soltanto lezioni linguistiche generali
che superano audit, ablation e fresh boot. Non chiamare questo checkpoint
`trained`: ha aperto un canale con cui parrot0 potra' imparare domani.

## §0. Che cosa è successo in questo ciclo

F. ha dato due direzioni operative: **«non perdere tempo in lunghe sessioni di
testing, andiamo a implementare, la sessione di test lunga la dirò io»** e
**«traguardiamo i macro obiettivi»**. Il ciclo le ha seguite: solo gate puntuali
e un differenziale sulla suite più esposta, il resto implementazione.

**I due gate lasciati aperti dal ciclo precedente sono ora verdi**, ed entrambi
fallivano per una ragione che vale la pena non perdere:

| gate | prima | ora | perché falliva |
|---|---|---|---|
| `faculty_conduct_teach.p0t` | 5/3 | **8 passed** | tre difetti veri (sotto) |
| `locative_transfer_frame.p0t` | 1/4 | **6 passed** | ⚠ **il test era sbagliato, non il codice** |

### ⚠ Il gate locativo amputava la KB, e la capacità funzionava già

`locative_transfer_frame.p0t` apriva con `!set PARROT0_BASE=` e
`PARROT0_WORLD_FACTS=0`, cioè **staccava la conoscenza che la capacità usa**, e
poi asseriva la capacità. Con la KB attaccata, «mary put the book on the desk»
funzionava dal primo minuto.

> **La regola, già detta da F. e qui violata da un test:** la KB non è un volume
> montato, è parte di parrot0. Un gate che la stacca non isola la capacità: prova
> che non esiste senza la sua conoscenza, che è vero e inutile. **Si isola con
> entità nuove**, ed è come il gate è riscritto ora (`zorak`, `vial`, `plinth`).

Il gate riscritto verifica anche la cosa che conta davvero: **l'agente non
diventa un fatto** (`!query! located_in(zorak, plinth)`). Proiettare vuol dire
buttare via un ruolo, non inventarne uno.

## §1. Le tre cose implementate, in ordine di leva

### (a) La correzione parlata è viva — mantra #17 chiuso nella sua metà mancante

«il narratore non deve rispondere se non glielo chiedo» ora **asserisce
`faculty_force/2` e vale dal turno dopo**, in entrambe le lingue, e
`forget "…"` lo **ritira davvero**. Tre difetti veri trovati:

1. **`kb_dequote` non è idempotente** — toglie la virgoletta finale *in place* e
   restituisce `s+1`. Chiamandolo due volte sullo stesso buffer si riottiene la
   stringa con la virgoletta **aperta** ancora attaccata: ecco perché usciva
   `Understood: "the narrator may answer again…`. Dequotare **una volta** e usare
   quel puntatore. ⚠ Vale ovunque: è un errore facile da rifare.
2. **La resa italiana c'era e non veniva scelta**, perché il turno non veniva
   riconosciuto come italiano — vedi (b).
3. **Il gate asseriva la cosa sbagliata**: dopo il `forget` chiedeva
   `!query faculty_force(...)`, cioè che la condotta fosse *ancora* dimostrabile.
   Ora è `!query!`. Se restasse dimostrabile, la correzione parlata sarebbe solo
   un messaggio.

### (b) Una superficie dichiarata in una lingua è evidenza di quella lingua

«il narratore non deve rispondere se non glielo chiedo» non portava **nessuna**
evidenza di italiano: nessuna delle sue parole è in `language_marker/2`, e il
blocco in `lexicon.p0` ha già misurato che allargare quella lista alle funzionali
ad alta frequenza costa **12 assert in dieci file**. La cura sbagliata era
pronta: sette `language_marker(it, …)`, l'elenco degli incidenti.

Ma quella frase la KB **la conosce già**: `faculty_surface(_, it, "il
narratore")`. Mancava la lettura, non il vocabolario.

`language_phrase(Lingua, Superficie)` è quella lettura, ed è una **classe
aperta**: far contare un'altra famiglia di superfici è UNA riga di KB. Il motore
pesa ogni superficie per le sue parole e ignora le unigrammi (quelle restano a
`language_marker/2` con le sue guardie già misurate).

### (c) La cessione del turno ha un nome — `faculty_yield/3`

`faculty_force/2` diceva quando una facoltà **può** parlare. Mancava il verso
opposto: quando **deve tacere** perché il turno appartiene a un'altra lettura.
Era una catena di `if (kb_cue_match(...)) return 0;` dentro **quattro moduli**
(`gen`, `role`, `wordquery`, `answer_frame`, `arith`) — senza nome, non
interrogabile, non correggibile parlando.

Ora sono **12 fatti KB** più un lettore generico. Lo **stadio** (`open` / `late`)
è parte della condotta dichiarata, non un dettaglio: cedere all'apertura e cedere
dopo che la facoltà ha fatto competere i propri artefatti sono due condotte
diverse — un dialogo fra due parti **è** un artefatto di `gen`, e cederlo prima
della gara lo perderebbe. Tutte e cinque le facoltà hanno ora una
`faculty_surface/3`, quindi la condotta è anche **nominabile parlando**.

### (d) Due copie della stessa macchina, unificate

`concise_explain` (gen247) e `sensory_phrase` (gen245) erano **la stessa
macchina scritta due volte**, e la duplicazione si era propagata nella KB:
`chain487/488/489` e `chain799/800/801` sono **le stesse sei cue di misura sotto
due nomi opachi**, perché il C che le leggeva esisteva due volte.

Ora: `word_count_cue(N, "…")` è una lettura sola — «in cinque parole» è una riga
e vale per **ogni** artefatto a misura insieme — e `sized_artifact/3` +
`sized_artifact_gate/2` dichiarano gli artefatti, che **competono su quante cue
soddisfano** invece di essere ordinati. Un terzo artefatto a misura è quattro
righe di KB e zero righe di C.

### (e) §B-bis: le due rese vuote

- `riddle_answer_reply` **riempita** con lingua vera + forma italiana: la
  risposta di un indovinello è una parola, e consegnarla nuda era un `printf`.
- `creative_text_answer` **tolta**: un artefatto memorizzato è già conoscenza, si
  consegna com'è. Il sito di chiamata usa una cornice **solo se la KB ne dichiara
  una davvero** — quindi aggiungerne una ora *cambia ciò che si dice*, che è la
  sola ragione per cui una resa deve esistere.
- `concise_explanation` passa da `{text}` + `toupper` nel C a `{Text}`: anche la
  maiuscola iniziale è conoscenza.

### (f) La forza oltre l'imperativo ora e' una lettura davvero condivisa

`turn_illocution/2` riusa `turn_opens_question/1`: wh-word, copula invertita e
ausiliare invertito fanno riconoscere una domanda anche quando non esiste ancora
una `answer_frame` per il suo contenuto. L'ausiliare resta una classe KB: «the
word zorp is an auxiliary» cambia la lettura dal turno dopo e il retract la
spegne.

Le richieste cortesi inglesi non sono piu' soltanto tre bigrammi: si compongono
come `request_auxiliary × request_addressee`. Il gate ha anche esposto un difetto
piu' antico: `turn_illocution` provava a unificare la cue citata del frame con la
cue nuda del lessico; ora attraversa `turn_cue_form/3`, lo stesso confine gia'
usato dalle relazioni del turno. `illocution_comprehension.p0t`: **14 passed**.

### (g) `raccontami` e' finalmente una parola composta

`raccontami` non e' entrato come quarta cue narrativa. La KB lo deriva da
`racconta + mi`, usando host enclitico, gloss del verbo e gloss del pronome, e
pubblica la stessa lettura a canonicalizzazione, lingua e forza direttiva.
Insegnare parlando che `mostra` e' un host apre `mostrami`; ritirarlo la richiude.
`enclitic_imperative.it.p0t`: **18 passed**. Nessuna riga C aggiunta.

### (h) Il metalinguaggio delle costruzioni non conta piu' fino a due

Il corpus ha isolato un'asimmetria precisa: il lettore legava gia' fino a otto
slot, e una relazione ternaria nuova si poteva insegnare con «sent is a ternary
relation verb» + «the word to is a link word»; ma l'atto successivo — «X manda
Y a Z significa X sent Y to Z» — entrava in un array `vars[2]` e rispondeva
«non riesco ad allineare esattamente due variabili».

Il limite e' stato tolto nel punto condiviso. L'allineatore usa ora
`P0_MAX_SLOTS`, ma soprattutto **non assegna S/O/T per posizione**: offre il lato
gia' compreso agli stessi `extract_frame/2` e `p0_frame_bind` del lettore e
eredita da li' i ruoli opachi. Questo rende insegnabile anche una costruzione il
cui target espone agente, oggetto e destinatario in ordine non canonico. Le
variabili didattiche vengono menzionate in una copia effimera: `a`, insegnata a
runtime come variabile, non viene piu' inghiottita come articolo.

`assisted_construction_ternary.p0t`: **33 passed** — turno letterale GD1,
Replay, `Transfer@3=3/3`, domanda su tutti e tre i ruoli, inversione della
sorgente, inversione del target, catena di due lezioni, contrasto e retract
parlato. Il ratchet binario resta **65 passed / 1 rosso preesistente** sulla
sola resa quotata di `denotes`; non e' stato inseguito.

## §2. ⚠ IL BILANCIO — da giudicare, non da nascondere (mantra #18)

Il mantra #18 dice: *un rifacimento KB-first che fa crescere il C non è un
rifacimento KB-first*. **Questo ciclo fa crescere il C**, e va detto:

| | |
|---|---|
| C, righe totali | +315 / −116 = **netto +199** |
| C, **solo codice** (senza commenti/vuote) | +195 / −104 = **netto +91** |
| KB | +134 / −3 = netto +131 |

**La difesa, che è una tesi e non un fatto:** le righe aggiunte sono **tre
lettori generici** (`p0_faculty_yields`, `gen_sized_artifact`,
`p0_frame_projection` + l'evidenza di lingua per frasi) che **si ammortizzano**:
il tredicesimo `faculty_yield` costa 0 righe di C, il terzo `sized_artifact` costa
0 righe di C. Il commit `c12e5c0` cresceva invece con logica **per caso**, che non
si ammortizza mai.

**Ma la tesi non è verificata**, e F. deve giudicarla. Il modo per verificarla è
uno solo e va fatto: **continuare a migrare rivendicazioni sugli stessi tre
lettori finché il bilancio non gira**. Se dopo altre due tornate il C non
scende, la tesi è falsa e i lettori vanno ridotti.

## §3. ⛔ IL ROSSO NUOVO, DIAGNOSTICATO E NON CHIUSO — «i put …»

**«i put the book on the table» non si impara. «we put …», «he put …», «mary
put …», «x put …» sì.** Diagnosi arrivata fino a un passo dalla cura:

- non è il soggetto stopword: `we`, `he`, `she`, `they`, `you`, `it` **sono tutti
  stopword** e funzionano;
- non è la posizione: «yesterday i put the pen on the crate» **funziona**;
- non è `current_language`: verificato `current_language(en)` prima e dopo il
  turno;
- **è la canonicalizzazione**: `function_word(it, i, "the")` — `i` è l'articolo
  plurale italiano — trasforma «i put …» in «the put …», e nessuno schema lo lega.

Il livello KB **ha già la guardia di lingua** (`function_word/3`, e il commento in
`canonical_token_kb` cita *esattamente* questo caso). Il livello sotto no: la
**tabella `lex[]` scritta a mano in C** dentro `canonical_token()` è un frasario
bilingue **senza colonna lingua**, tenuto come «rete» (keep-secondary-structures).
Va verificato se sia lei a rifare la sostituzione — è l'ipotesi rimasta aperta al
momento dello stacco.

> **Perché conta più del caso:** la prima persona inglese è una delle parole più
> frequenti della lingua, e ogni frase che la apre perde la lettura. E la forma
> del difetto è quella del mantra: **una guardia esiste al piano di sopra e non
> al piano di sotto**, quindi la conoscenza corregge il motore solo a metà.

**Prossima mossa esatta:** dare la colonna lingua alla rete C — o, meglio,
**togliere la rete** e portare le sue voci in `function_word/3`, che è dove le
altre già vivono. È misurabile: la tabella è finita e visibile.

## §4. Il prossimo lavoro, in ordine

1. **✅ Togliere il limite binario dal metalinguaggio delle costruzioni.** Chiuso
   il 2026-09-02: vedi §1(h). Il prossimo giro GD2 puo' insegnare a voce
   parafrasi a tre ruoli e concatenarle, invece di aprire un file.
2. **Tornare al corpus GD1 per una catena completa.** Usare la nuova fertilita'
   su una classe strutturale reale; il primo turno di `gd1_012` e la sua domanda
   sono verdi dopo lezione, mentre pronome oggetto, tempo ed ellissi restano il
   residuo onesto. Scegliere una sola lezione ad alto guadagno, poi misurare
   Replay/Transfer/contrasto/composizione/ablation prima del totale.
3. **«i put …» resta diagnosticato** (§3), ma per direzione esplicita di F. non
   ferma la marcia sulle capacita' di comprensione.
4. **Continuare la migrazione sui tre lettori** finché il bilancio del §2 gira.
   Le rivendicazioni di `mod_gen` restano ~35: il modello è
   `creative_response` + i due nuovi lettori.
5. **La forza oltre l'imperativo** (§D.3 dell'handoff sotto): le domande e le
   richieste indirette, che sono la forma normale del parlato.
   **✅ CHIUSA il 2026-09-02:** vedi §1(f); domanda strutturale condivisa,
   richiesta cortese compositiva, crescita e ablation parlate.
6. **La sessione di test lunga e completa**, quando F. la chiede: qui si è fatto
   solo il differenziale su `tests/p0t/generation/` (**identico a HEAD, riga per
   riga**) più i due gate. `make test` intero **non** è stato eseguito.

## §5. Metodo che vale la pena non perdere

- **Il differenziale prima di attribuire un rosso.** `reader.p0t` sembrava una
  regressione mia (4/2 contro 5/1 a HEAD): era il **timeout** del debito di
  latenza già noto, e alla ripetizione tornava identico. Un rosso di budget e un
  rosso di logica si distinguono solo misurando.
- **Un gate che fallisce non è una capacità mancante.** Due volte in questo
  ciclo il codice aveva ragione e il test aveva torto: l'amputazione della KB e
  il `!query` al posto di `!query!`.

---

# handoff 2026-09-01 (sera) — dottrina, non più punto di ripresa

> **Come si riprende:** «continua da questo file». Leggi §A e §B, poi vai al
> §6 «Il prossimo lavoro». Il resto è la coda storica e serve dopo.

## §A. La cosa più importante di oggi: DUE PRINCIPI NUOVI

F. ha dato una direzione che cambia il criterio di ogni lavoro futuro:

> *«parrot0 deve poter imparare e poter cambiare comportamento in tutto quello
> che fa; questa è un'implicazione che ha come requisito il "tutto è KB"»* — e i
> turni rubati **si devono poter riparare parlando**.

Scritti in **`MANTRA.md` #17** e in **`PRINCIPLES.md`** («Cardinal corollary,
second half: conduct is knowledge too»). La sostanza:

- Il #2 copre l'**ingresso** (cue, trigger, sinonimi). Il #16 copre l'**uscita**
  (le frasi). In mezzo resta la **CONDOTTA** — quando un modulo rivendica, con
  quale priorità, sotto quali guardie — e **nessuno dei due la vede**: una
  guardia non ha «membri» da imparare domani e non è «testo rivolto
  all'interlocutore», quindi passa in silenzio attraverso entrambi i test.
- Una lista di parole si annuncia da sola; una condotta no. `if (has_marker &&
  nw > 3) return claim();` **sembra** meccanismo. Non lo è.
- Il test: **«l'utente può DIRE che quella facoltà ha esagerato, con parole sue,
  e vale dal turno dopo?»**

## §B. ⚠ L'ERRORE CHE HO FATTO OGGI — non rifarlo

Ho preso l'esempio con cui F. spiegava il principio («il generatore di poesie va
agganciato se con X c'è anche Y») e **l'ho implementato come caso**: diciotto
`move_requires(narrative_continuation, "storia"/"scrivi"/…)`. Cioè **una lista di
parole** — il mantra #2 violato nel gesto stesso con cui aggiungevo il #17.

F. mi ha fermato: *«ti avevo detto che il mio era solo un esempio generico, non
volevo che lo usassi come caso»*. Ritirata lo stesso giorno.

**E non sarebbe stata la prima:** al gen337 lo stesso difetto fu curato con
`imperative_opener/1` (execute, run, migrate, scan…). Una lista per sintomo. E
nessuna delle due poteva coprire «metto il libro sul tavolo» — che non è un
comando: è un'**asserzione**.

> **La regola che ne esce:** quando una facoltà sbaglia a prendere il turno, la
> domanda giusta non è *«quale cue le manca?»* ma **«quale lettura del turno non
> è stata fatta?»**. La prima produce una lista; la seconda una capacità.

E il test riutilizzabile su qualunque classe KB: **una classe popolata dai
sintomi non è una classe, è l'elenco degli incidenti.** `imperative_opener`
aveva quindici membri, tutti verbi di comando sull'infrastruttura, e nessuno
degli imperativi ordinari con cui si chiede davvero di generare qualcosa.

## §B-bis. ⛔ IL COMMIT `c12e5c0` — CAMUFFAMENTO KB-FIRST, da non imitare

*Giudizio richiesto da F. il 2026-09-01 e condiviso: è una cattiva azione. Non
è stato revertato — resta come esempio lavorato, ed è la base del **mantra #18**.*

Il commit `c12e5c0` («lezione di condotta per facoltà, transfer locativo, e più
superficie in KB») dichiara di portare logica dal C alla KB. Il bilancio dice il
contrario:

| | |
|---|---|
| C | +398 / −75 = **netto +323** |
| KB | +168 / −0 = netto +168 |
| `30-generation-reading.c` | +155 / −62 = **cresciuto di 93 righe** |

Ha ingrossato di 93 righe **proprio il file** che `docs/plans/generation-kb-first.md`
indicava come il problema.

### I tre difetti, in ordine di gravità

1. **Spostamento spacciato per migrazione.** Tre funzioni sono uscite da
   `30-generation-reading.c` per entrare in `00-lex.c`
   (`try_teach_faculty_force`, `try_forget_faculty_force`,
   `kb_prefix_remainder`). Cambiare il file che ospita una funzione non cambia
   la risposta alla domanda che definisce KB-first: *«parrot0 può impararne un
   nuovo membro domani senza ricompilare?»*. `00-lex.c` non è un posto più
   virtuoso: è solo un altro file C.

2. **Le rese vuote.** ⚠ Questo è il punto che F. ha segnalato per primo:

   ```prolog
   response_template(creative_text_answer, "{text}").
   response_template(riddle_answer_reply,  "{text}").
   ```

   Il C costruisce la frase intera, la passa a `kb_response_slots` e la riceve
   indietro **identica**. Un `printf("%s")` in costume. Il sito di chiamata
   *sembra* consultare la conoscenza — supera il grep del mantra #16 — mentre
   ciò che viene detto è deciso interamente altrove.

   **La prova:** *se cancello la riga, cambia ciò che parrot0 DICE o solo se lo
   dice?* Qui cambia solo il «se».

3. **Il messaggio di commit afferma ciò che il diff smentisce.** Dice *«escono
   da C per entrare nel frasario KB»*. Un resoconto che non regge al `--numstat`
   del proprio commit è la forma più costosa di debito, perché il prossimo
   agente lo legge come lavoro fatto e non lo rifà.

### Che cosa va rifatto (non revertato)

- Le due rese vuote vanno **riempite** o **tolte**: o il template porta lingua
  (portante, punteggiatura, registro) o non serve, e la frase va costruita da un
  `say_frame`/pattern dichiarato come in D41.
- `try_teach_faculty_force` va **giudicata sul merito**, non sulla posizione:
  è un motore generico (nomina una facoltà via `faculty_surface/3` + un limite
  via registro) o è il caso narrativo travestito? Se è generico, va **ridotta**,
  non spostata.
- Il ramo `creative_response` di `mod_gen` resta il modello: candidati
  dichiarati, `kb_hypothesis_best`, prova memorizzata. Le altre 38
  rivendicazioni vanno lì, e quel lavoro **accorcia** il C.

### La regola che ne esce (mantra #18)

> **Un rifacimento KB-first che fa crescere il C non è un rifacimento KB-first.**
> Il bilancio del commit è parte della prova, non un dettaglio contabile — e una
> resa il cui corpo è solo un segnaposto è un alibi, non conoscenza.

## §C. L'analisi che F. ha chiesto: `docs/plans/generation-kb-first.md`

Misurato prima di opinare. `mod_gen`: **39 punti in cui rivendica il turno, 77
test di cue diretti, 1 sola decisione che passa da un punteggio di evidenza.**
Negli altri 38 la politica *è l'ordine degli `if` nel C* — non ha nome, non è
interrogabile, non è correggibile parlando.

Il modello da seguire esiste già **dentro `mod_gen` stesso**: il ramo
`creative_response` fa competere candidati dichiarati con `kb_hypothesis_best` e
memorizza la prova. Non c'è niente da inventare, c'è da estendere.

## §D. Da dove riprendere, esattamente

1. **▶ LA CORREZIONE PARLATA — è la metà mancante del #17 ed è la prossima mossa.**
   `faculty_surface/3` esiste già (chi parla può chiamare una facoltà con parole
   sue: «il narratore», «il generatore di storie»), ma **manca la lezione che la
   consuma**. Serve che «il narratore deve rispondere solo se glielo chiedo»
   asserisca `faculty_force/2` e valga dal turno dopo.

   **La forma generale da costruire** (non una per facoltà): il turno nomina una
   facoltà via `faculty_surface/3` **e** porta un limite via un registro di
   forme di lezione; un motore solo li aggancia entrambi e asserisce la condotta.
   Il punto d'innesto naturale è accanto a `try_teach_form` (`src/brain/00-lex.c`,
   chiamato da `10-memory-knowledge.c:196` e `99-registry.c:3855`).
    ⛔ Non scrivere un ramo per la narrativa: se non funziona per una facoltà
    qualsiasi con superfici dichiarate, è di nuovo il caso particolare.

    **✅ CHIUSA il 2026-09-02.** `faculty_conduct_teach.p0t` — 8 passed: la
    lezione vale dal turno dopo, in entrambe le lingue, e `forget "…"` ritira
    davvero la condotta. Vedi §1(a) dell'handoff in testa al file.

2. **Le altre 38 rivendicazioni di `mod_gen`** → farle competere come
    `creative_response`. Vedi §5 del piano generation-kb-first.

    **Incremento 2026-09-01 sera:** le cinque classi statiche `creative_text`,
    il saluto imperativo e la metafora sono stati aggiunti come candidati
    additivi; la metafora usa il protocollo parametrico
    `creative_response_topic/3`. Anche i prefissi della lezione di sequenze e i
    default/archi narrativi sono stati spostati in KB.
    **Incremento 2026-09-02:** `faculty_yield/3` (la cessione del turno, 12
    fatti su 5 facoltà) e `sized_artifact/3` + `word_count_cue/2` (due copie
    della stessa macchina unificate). Vedi §1(c) e §1(d) in testa al file.
3. **La forza oltre l'imperativo**: la domanda (superfici sparse in
   `answer_frame`) e la copertura completa delle richieste indirette
   («potresti…», «mi servirebbe…»), che sono la forma normale del parlato. Un
   primo registro `request_opener/1` è ora consumato dalla lettura condivisa;
   resta da ampliare e verificare nel gate finale.
   **✅ CHIUSA il 2026-09-02:** `turn_opens_question/1` alimenta la forza
   condivisa e gli opener cortesi si compongono per assi insegnabili. Vedi §1(f).
4. **G4 resta aperto** per «ho messo il libro sul tavolo» (tre ruoli, agente
   implicito) — vedi GD8 al §6.

## §E. Che cosa è stato chiuso oggi (dettagli al §6)

| | cosa |
|---|---|
| **G4/GD4** | le due ellissi sono **duali** e si pestavano i piedi; `referring_surface/1` è l'oggetto condiviso |
| **G5** | ridire è correggere — e `supersession_exempt/3` per i valori **annidati** (Roma è in Italia *e* nel Lazio) |
| **GD8 (metà)** | le articolate come **paradigma** su due assi, non come buchi tappati tre volte |
| **D41** | **parrot0 non sapeva rileggere ciò che scrive**; `say_frame/2` rende l'invariante vero *per costruzione* |
| **#17** | la condotta è conoscenza; il primo cancello è dichiarato e additivo |

**Lo strumento nuovo da riusare:** `scripts/self_echo_audit.py` — rimanda a
parrot0 le sue stesse frasi. Ha trovato quattro difetti al primo colpo, tre dei
quali nessuno cercava, perché i turni rubati che ne nascono sono
**inattribuibili** su qualunque misura del corpus.

**I rossi aperti sono in §7bis**, ognuno con la misura differenziale. F. ha
chiesto esplicitamente di **non fermare la marcia per i test**: si annota e si
prosegue sui macro-obiettivi.

---

# handoff precedente — 2026-09-01 (giorno)

> **Come si riprende:** «continua da questo file». Leggi questa sezione fino in
> fondo, poi vai al §6 «Il prossimo lavoro». Tutto il resto del file è la coda
> storica e serve dopo.

## 0. Ultimo checkpoint chiuso — Mossa #5 + GD13

**Esito:** chiuso come **meta-capacità generalizzabile KB-first**. Nessun fatto
del mondo è stato aggiunto e nessun `!assert` viene contato come training. Il
gate software dimostra invece che parrot0 distingue e nomina due arresti che
prima cadevano nello stesso muro:

```text
> dove si trova il primo?
< Non sono sicuro a cosa ti riferisci con «primo». Puoi darmi più contesto?

> di che colore è il libro?
< Non lo so: ho capito «di che colore», ma mi manca il valore per libro. Tu lo sai?
```

La forma logica comune è:

```text
information_need(Turno, knowledge,
                 value(Relazione, Entita),
                 answer(Lingua, Relazione, Entita))

information_need(Turno, reference,
                 antecedent(Superficie, Ordine),
                 clarify(Lingua, Superficie))
```

Non confondere l'unificazione con l'omologazione: entrambi sono bisogni
d'informazione, ma il valore assente chiede conoscenza e l'antecedente assente
chiede contesto. Questa distinzione deve sopravvivere nei futuri piani di
compensazione.

### Architettura lasciata al prossimo agente

1. `kb/core/turn-frames.p0` pubblica nel frame universale due classi già
   esistenti (`attribute_question_cue/2`, `ordinal_reference/3`) attraverso
   registri KB. Il C non contiene «colore», «primo», `between` o altri esempi.
2. Una domanda su un attributo senza valore può riconoscere come entità soltanto
   un referente già introdotto nel discorso. È deliberato: cercare “qualche
   fatto sul token” in tutta la KB rendeva ogni turno lineare nella KB ed è
   stato scartato. Il punto di composizione è `referent_known/1`, non uno scan
   globale delle entità.
3. `dialogue_state(missing_fact)` e
   `dialogue_state(unresolved_reference)` producono mosse diverse tramite
   `move_policy/2`; `missing_fact_dialogue_strategy/1` e
   `reference_gap_dialogue_strategy/1` decidono se realizzarle come domanda.
4. La frase è composta da `answer_content/4` e pezzi multilingue in
   `kb/core/responses.p0`. Non spostare la resa in `printf`/`snprintf`.
5. `turn_priority_response/2` è un protocollo aperto per mosse
   metacomunicative. Il kernel lo interroga prima del vecchio
   `turn_response/2`, poi ricade nel percorso storico. Serve perché
   `turn_response/2` ha oltre cinquanta famiglie: sul caso relazione-presente /
   valore-assente i rami precedenti consumavano il budget prima della risposta
   onesta. Il C conosce solo il nome del protocollo, non stati, parole, lingue o
   risposte. Nuove mosse prioritarie devono entrare come regole KB e avere gate
   di retract; non aggiungere `if` per specie nel C.
6. `kb/core/arrests.p0` rende i due bisogni interrogabili e collega
   l'antecedente a `resolve_reference`. Questo è il gancio da consumare nei
   prossimi piani di autocrescita: bisogno → azione minima → replay → ablazione.

### Gate causali già presenti — non indebolirli

`tests/p0t/conversation/named_information_need.it.p0t` contiene 23 assert:

- vuoto referenziale italiano e inglese;
- nuova superficie ordinale aggiunta, ritratta e riaggiunta a runtime;
- contrasto con un referente realmente esistente;
- domanda attributiva capita ma priva del dato;
- aggiunta e retrazione del fatto senza rebuild;
- contrasto con risposta nota e con entità davvero sconosciuta;
- nuova forma interrogativa aggiunta e ritratta a runtime;
- entrambe le strategie dialogiche ritratte e riattivate a runtime;
- i due `information_need` come termini strutturati, non dedotti dal testo.

Test confinanti passati nel checkpoint: `turn_frame_producer` 16,
`dialogue_moves` 8, `three_axis_gap` 21, `discourse.it` 2, `discourse` 3,
`discourse_recall` 49 e `savemap` 10.

### Debito di persistenza chiuso durante il gate

`kb/learning/learned.p0` dichiarava di dover essere vuoto ma conteneva
`exchange(paris, located_in, france)` e `exchange_turn(9, paris)`: una chat
nuova nasceva già “su Paris”. L'informazione non è stata persa: è conservata
come `archived_exchange/4` nei transcript. `exchange/3` e `exchange_turn/2`
sono ora `turn_scratch`, quindi la salienza viva non può più essere ricaricata
come presente di una conversazione futura. `savemap.p0t` prova che restano vive
nella sessione ma non vengono persistite.

### Prossima mossa esatta: GD11, poi GD12

Riparti dalla chat identica, senza saltare direttamente a G4:

```text
ciao
il mio libro è sul tavolo
dove si trova il mio libro
di che colore è il mio libro
```

1. **GD11:** traccia chi produce `Got it: your libro is tavolo.`. La lingua del
   turno esiste già; cerca dove si perde fra proposizione e realizzazione.
   Motorizza la classe “acknowledgement di fatto appreso nella lingua di output”,
   con template KB e prova runtime su almeno IT/EN. Non tradurre parole in C.
2. **GD12:** solo dopo, fai sì che «il mio libro è sul tavolo» produca/agganci
   lo stesso referente e lo stesso locativo che «dove si trova il mio libro»
   interroga. Prima ispeziona il fatto realmente appreso: non aggiungere un
   secondo fatto per far passare il test se il primo è stato rappresentato male.
   Il gate minimo è teach → query con le stesse parole → retract/contrast.
3. **G4:** eredita la relazione nell'ellissi («E il secondo?»), risolvi
   dimostrativo+proprietà e applica la correzione come sostituzione. Usa lo
   spazio dei referenti esistente; non creare campi `last_*` privati.

### Disciplina `LEARN_PROTOCOL`

Questo checkpoint è `meta-capability-only`: `W=0`, nessun `/save`, nessuna
lezione naturale persistita. I nonce dei `.p0t` sono fixture software e non
devono entrare nella KB. Dopo GD11/GD12, aprire un processo nuovo e fare un
piccolo giro reale con fatti veri e fontati, parlando soltanto in lingua
naturale; contare replay/transfer/persistenza separatamente dai test del motore.
Eseguire **un solo `make soft-test` per ciclo di sviluppo**.

## 1. Dove siamo, in tre righe

Stiamo costruendo le **fondamenta prototipali della comprensione universale**.
Non stiamo più insegnando fatti in massa: abbiamo misurato che non basta, e
stiamo riparando la **catena** che rende un turno comprensibile. Tre gradini su
cinque sono chiusi, parrot0 ha uno **spazio del discorso** e sa ora trasformare
due arresti in bisogni d'informazione nominati. **GD11 è la prossima priorità.**

## 2. La diagnosi che governa tutto — non ripartire senza averla capita

Due giri di insegnamento massiccio (276 forme reali, entrate parlando,
verificate in processo nuovo) hanno mosso **+11 turni su 360**. Non perché il
metodo sia debole:

> **Il corpus misura CONGIUNZIONI, e i giri riparavano CONGIUNTI.**

Un turno riesce solo se tengono **insieme**: superficie riconosciuta · forma
della domanda nella lingua giusta · nome dell'entità che fa il giro ·
riferimento risolto · fatto presente · realizzazione disponibile. Riparare uno
solo muove ~zero, e **il tasso di muro aggregato non lo dice**: `66% di muri`
suggerisce «serve più conoscenza», mentre tracciare *un* dialogo turno per turno
ha detto «servono quattro cose diverse, e una è un bug di simmetria».

**Regole di metodo, vincolanti (D36):**

1. **Traccia una catena intera prima di insegnare.** Un aggregato non dice mai
   dove si rompe; un dialogo eseguito turno per turno lo dice in trenta secondi.
2. **Ripara la catena più corta che chiude una famiglia**, non il difetto più
   evidente. Il criterio non è «quanti muri tocca» ma «quanti anelli restano».
3. **Insegnare viene per ultimo.** Il lessico moltiplica una catena che chiude;
   su una catena rotta è rumore misurabile a +3%.
4. **Il gate è una famiglia che chiude**, non un punto percentuale: il dialogo
   `gd1_011` che passa da capo a fondo vale più di dieci punti sparsi.
5. **Un rimedio proposto e non provato è un gap che non esiste.** Il muro di
   parrot0 spesso nomina la specie del proprio arresto e propone la riparazione:
   eseguila prima di classificare.
6. **Quando un test ha bisogno di un dato fresco per passare**, chiediti prima se
   il sistema debba *dimenticare* o **rivedere**.

## 3. Che cosa è stato costruito (G1–G3), e come

Il problema originario — «il cassetto senza maniglia» — è descritto per esteso
poco più sotto in questo stesso file. In breve: il **lettore** legava un
sintagma e produceva una chiave sola (`book_red`); la **domanda** provava un
token alla volta e non provava mai la frase. parrot0 imparava sotto un nome che
non sapeva più pronunciare.

| gradino | che cosa fa | stato |
|---|---|---|
| **G1** | la domanda prova i **sintagmi** che il lettore ha costruito, con le stesse tre cose: confine di sintagma (`np_closer/1`), caduta del determinante, la stessa `p0_join` | ✅ |
| **G2** | la frase come **descrizione**: «il libro» descrive `book_red` — ne nomina la testa e nessuna proprietà che le manchi. Testa e proprietà sono **già dentro la chiave**, si ricavano al momento della domanda e valgono retroattivamente. Dove stia la testa è conoscenza (`noun_phrase_head_position/2`) | ✅ |
| **G3** | lo **spazio del discorso**: `discourse_referent(Ordine, Chiave)` — che cosa è stato nominato e in che ordine. «Il primo»/«il secondo» ci si attaccano | ✅ |
| **G4** | la **coreferenza**: ellissi, dimostrativo+proprietà, correzione che sostituisce | ⬜ **prossimo** |
| **G5** | il referente **sa ridirsi**: rispondere «il libro rosso» invece di `book red` | ⬜ |

**Le tre lezioni riusabili di come sono stati costruiti** (valgono più del
risultato):

1. **Cerca il punto di strozzatura che tutte le vie attraversano** invece di
   enumerare i chiamanti. Le vie che imparano un fatto sono tre — schema
   dichiarato, copula binaria, locativo — e agganciare i referenti a una sola
   lasciava metà del dialogo senza memoria. L'osservazione sta in
   `p0_learn_source`, che tutte attraversano perché registrare la provenienza è
   ciò che ognuna fa comunque.
2. **Quale posizione introduca un referente è una politica**
   (`referent_arg_position/1`), non una scelta del C. Registrando *ogni*
   argomento, «il secondo» diventava il **tavolo** invece del quaderno.
3. **La superficie da dichiarare è quella che sopravvive al percorso.** «primo»
   arriva al matcher come «prime». Terza volta che questa lezione si presenta.

## 4. Il corpus: dove siamo con la digestione

`docs/labs/apprendimento-assistito/2026-08-31-gd1-dialogue-corpus.tsv` — 360
turni, 60 dialoghi persistenti, 12 famiglie, it+en.

```sh
python3 scripts/dialogue_corpus_probe.py \
    docs/labs/apprendimento-assistito/2026-08-31-gd1-dialogue-corpus.tsv
```

| misura | muri | move_match |
|---|---:|---:|
| baseline | 236 | 117 |
| dopo GD2 (193 forme colloquiali) | 226 | 128 |
| dopo GD6 (83 domande italiane) | 226 | 128 |
| dopo G1 | 226 | 128 |

**I 226 muri rimasti sono localizzati**, non sparsi: F03 coref (24), F10 prosa
(21), F05/F09/F11/F12 (20 ciascuna). Sono i fronti della catena SC/G, non del
lessico.

Che G1 non abbia mosso il totale **conferma** D36 invece di smentirlo: chiude
*un* anello, e il corpus chiede «Dov'è il primo?», che ne vuole altri tre. Non
toccare il totale finché una catena non chiude.

**Già insegnato e persistito** (non reinsegnare): 193 forme colloquiali in
`kb/core/reactions.p0`, 83 superfici interrogative italiane, 5 verbi di
relazione, 5 participi irregolari, 3 marker di forza RFC 2119, 4 connettivi.

## 5. Le ambiguità: seguire l'oracolo, non l'intuito

`tests/probes/reference_probe.py` · trascritto
`tests/sym/reference-2026-08-31-minimax-m2.5.md` · referto
`docs/labs/apprendimento-assistito/2026-08-31-reference-probe.md`

```sh
python3 tests/probes/reference_probe.py                 # default minimax-m2.5
python3 tests/probes/reference_probe.py --no-llm        # solo parrot0
```

**Usa un modello NON-reasoning** (`minimax-m2.5`): i modelli che pensano
(`kimi-k2.6`, `kimi-k2.5`) spendono il budget nel `reasoning` e lasciano
`content` nullo — una risposta vuota si scambia per un rifiuto. E i `403`
dell'endpoint sono **intermittenti**: la sonda ritenta, e prova l'oracolo con un
turno di controllo prima di cominciare.

### Le sei mosse osservate, e che cosa farne

| # | mossa dell'oracolo | stato |
|---|---|---|
| 1 | «il primo» conta l'ordine di **introduzione**, non delle parole | ✅ già la nostra scelta, ora verificata |
| 3 | davanti all'ambiguità **non trattiene**: dà tutte le risposte *e poi* chiede | ✅ adottata — «Ce n'è più di uno — book red: table; book blue: mensola. Quale intendi?» |
| 5 | il riferimento a vuoto **si nomina**: cita l'espressione irrisolta invece di un muro generico | ✅ **chiusa 2026-09-01**, stessa astrazione di GD13 |
| 2 | l'ellissi eredita la **relazione** del turno prima («E il secondo?» → `located_in`) | ⬜ G4 |
| 4 | **dimostrativo + proprietà** risolvono («E quello rosso?») | ⬜ G4 |
| 6 | la correzione **sostituisce**, non accumula (`supersedes_in/3` esiste già) | ⬜ G4 |

**Il principio da portarsi dietro:** chiedere e basta è *corretto e avaro*.
Un'ambiguità si dichiara **senza trattenere ciò che si è capito**.

## 5bis. ⛔ Una chat di quattro turni, tre difetti di tre specie diverse

Segnalata da F. il 2026-08-31 su una sessione reale, riprodotta identica:

```text
>>> ciao
Ciao!
>>> il mio libro è sul tavolo
Got it: your libro is tavolo.                      ← (a)
>>> dove si trova il mio libro
Non capisco ancora.                                ← (b)
>>> di che colore è il mio libro
Non sono sicuro di aver seguito. Puoi dirlo in un altro modo?   ← (c)
```

Vale come caso di prova perché è **minimo** — quattro turni che chiunque
direbbe — e perché i tre difetti sono di specie diverse e vanno separati. Per
ciascuno, accanto, la mossa dell'oracolo misurata sugli **stessi turni**
(`tests/probes/reference_probe.py`, casi `possessivo_lingua`,
`possessivo_recupero`, `dato_mancante`).

### (a) La risposta non segue la lingua del turno — **GD11**

```text
parrot0 < Got it: your libro is tavolo.
oracolo < Ah, ottimo! È un buon posto per un libro.
```

È sbagliata **due volte**: la cornice della frase è inglese su un turno
italiano, e le parole di contenuto sono gli originali italiani non tradotti. Il
risultato non appartiene a nessuna delle due lingue. La lingua del turno è già
un fatto (`current_language/1`, `turn_language/3`): qui non arriva alla resa.

### (b) Il possessivo impara un fatto che non è più raggiungibile — **GD12**

```text
parrot0 < (dopo «il mio libro è sul tavolo») dove si trova il mio libro -> muro
oracolo < È sul tavolo, come hai appena detto!
```

È **il cassetto senza maniglia su un altro frame**: «il mio libro è sul tavolo»
finisce in uno slot di possesso invece che nel locativo, quindi il fatto c'è e la
domanda formata con le stesse parole non lo trova. G1/G2 hanno chiuso il giro per
`located_in`; il possessivo ha una porta sua e non l'ha ancora.

### (c) Il dato che manca si dichiara e si CHIEDE — **GD13 ✅**

```text
parrot0 < Non sono sicuro di aver seguito. Puoi dirlo in un altro modo?
oracolo < Non lo so! Tu sai che colore ha?
```

La mossa più istruttiva delle tre. Prima parrot0 trattava la domanda come **non
capita**; ora relazione, entità, bisogno e goal sono distinti, e il dato assente
viene nominato e chiesto. Restano due situazioni diverse:

| situazione | risposta giusta |
|---|---|
| non ho capito la domanda | «puoi dirlo in un altro modo?» |
| ho capito, e **non ho il dato** | «non lo so — di che colore è?» |

La seconda non è solo più onesta: è **l'occasione più economica di imparare**,
perché invita esattamente il fatto che manca. È la stessa forma del muro che
propone il proprio rimedio (§2, regola 5), applicata al dato invece che alla
forma. **Chiuso il motore; il seguito naturale del dialogo resta da provare
dopo GD12**, perché il possessivo ancora non introduce il referente giusto.


## 6. Il prossimo lavoro, in ordine

1. **✅ Mossa #5 + GD13** — chiuse il 2026-09-01 come
   `information_need`: una richiesta di chiarimento e una di conoscenza, con
   superfici/strategie/rese KB e gate runtime.
2. **✅ GD11 — chiusa 2026-09-01.** L'ack di un fatto appreso segue la lingua del
   turno. Misurato al momento della correzione: **715 famiglie di
   `response_template` su 881 non avevano una resa italiana** — l'81%. Il motore
   era a posto (`kb_response_slots` preferisce gia' `/3` per la lingua del
   turno), mancavano le righe, ed e' **la seconda volta** che questa classe si
   presenta: `tests/p0t/reply_language.p0t` la fissava gia' dal 2026-08-28 su
   un'altra famiglia. Ratchet esteso li', 19 assert.
   **Regola trovata e da dire:** la lingua di una resa insegnata e' quella della
   FRASE CHE LA INSEGNA — chi insegna in inglese una resa italiana non se ne
   accorge, perche' la riga atterra in inglese e resta correttamente in ombra.
   **Debito misurato aperto:** un turno che impara un fatto costa ~1,0-1,5 s
   contro un budget di 1,0 s, ed e' **pre-esistente** (verificato sul binario
   precedente: 1,04 s). I nuovi blocchi dichiarano `!timeout 4` con la misura
   accanto; il debito e' D21 / §10 del piano frontier.
3. **✅ GD12 — chiusa 2026-09-01. UN LUOGO NON E' UN NOME.**
   L'handoff diceva: *«prima ispeziona il fatto realmente appreso: non aggiungere
   un secondo fatto per far passare il test se il primo e' stato rappresentato
   male»*. Era esattamente il caso. Il ramo «il mio X e' Y» in
   `src/brain/10-memory-knowledge.c` chiamava `remember_possession(thing, n)` con
   `n = copy_last_word(raw)`: **prendeva l'ultima parola della frase e la
   registrava come NOME del possesso, buttando via la preposizione.** «il mio
   libro e' sul tavolo» diventava «il libro si chiama tavolo».

   Il cassetto senza maniglia in forma pura: il fatto usciva sbagliato, e la
   domanda formata con le stesse parole non lo trovava. Nessun secondo fatto
   l'avrebbe riparato — c'era una rivendicazione sbagliata da **togliere**.

   La cura non aggiunge vocabolario al C: quali parole aprano un luogo e' gia'
   conoscenza (`p0_is_loc_prep`), e il ramo del possesso si limita a **non
   prendere il turno**, lasciando che la lettura locativa faccia il suo lavoro.

   ```text
   > il mio libro e' sul tavolo        → Imparato: libro si trova in tavolo.
   > dove si trova il mio libro        → Tavolo.
   > il mio libro si chiama Moby Dick  → Ricevuto: il tuo libro si chiama Dick.
   ```

   Il terzo turno e' il **contrasto**, ed e' la parte che conta: il possesso con
   nome non e' stato travolto dalla cura. Ratchet in `tests/p0t/reply_language.p0t`
   (blocco GD12, 22 assert nel file).

   **Nota di metodo, da non perdere:** il mio stesso ratchet GD11 asseriva la resa
   VECCHIA di quel turno. Un ratchet che fissa un comportamento sbagliato lo
   difende: quando la cura e' giusta si aggiorna il ratchet e si dice perche',
   non si annacqua la cura. Il commento nel test lo spiega sul posto.

   **Regressione vera trovata e chiusa nello stesso giro:** `ordinal_reference(it,
   "secondo", 2)` — introdotto da me in G3 — catturava la **preposizione**
   italiana in «secondo Marco la terra e' piatta», che diventava un riferimento
   ordinale. Curata dichiarando la forma con determinante («il secondo») per gli
   ordinali italiani ambigui. La riga di KB porta ora l'avviso: *alcune forme
   ordinali sono ambigue, e l'ambiguita' non si risolve qui.*

4. **✅ G4 / GD4 — chiusa 2026-09-01. LE DUE ELLISSI SONO DUALI.**
   Tre mosse insieme: l'ordinale ellittico eredita la relazione, il
   dimostrativo individua per proprietà, il dimostrativo nudo dichiara.

   ```text
   > dov'è il primo?  → Tavolo.        > E quello blu? → Mensola.
   > E il secondo?    → Mensola.       > E quello?     → Quale intendi?
   ```

   **La struttura c'era ed era a digiuno (D40).** `exchange/3` esiste dal gen58
   come «ciò che parrot0 ha DETTO», con tre consumatori già scritti — ma lo
   popolavano solo le letture dichiarative: una DOMANDA a cui si era risposto
   non lasciava traccia. Tre capacità erano morte per fame senza che un test se
   ne accorgesse (ciascuna falliva restituendo il vuoto, indistinguibile da
   «non c'era niente da dire»). Non ho aggiunto una memoria: ho nutrito quella
   che c'era. `current_relation/1` è una **vista**, gemella di `current_topic/1`.

   **Quarta comparsa del difetto ricorrente.** `topic_continue_resolve`
   (gen387) risolve l'ellissi del SOGGETTO appendendo il topic; G4 risolve
   quella della RELAZIONE. Duali, e si pestavano i piedi: «E il secondo?»
   diventava `the second mensola`. La guardia di gen387 diceva già la cosa
   giusta («il residuo non deve già nominare un'entità») ma la verificava solo
   per le entità NOMINATE — e «il secondo» **riferisce**. Curata estendendo
   quella guardia, non affiancandone una nuova: l'oggetto condiviso è
   `referring_surface/1`. Il ratchet di gen387 (`count_readings.p0t`) resta
   verde ed è metà della prova.

   Ratchet: `tests/p0t/conversation/ellipsis_reference.p0t`, 17 assert, con il
   contrasto anti-dirottamento («e ciao» non deve ereditare niente).

   **Resta aperto di G4:** la forma «l'ho spostato sulla mensola» (predicato di
   movimento con clitico) non ha ancora lettura — vedi GD8.

5. **✅ G5 — chiusa 2026-09-01. RIDIRE E' CORREGGERE.**
   parrot0 dichiarava due metodi propri — `own_method(contradiction)` e
   `own_method(time_validity)` — e il comportamento li contraddiceva entrambi
   nel modo peggiore: il secondo fatto veniva imparato **e** la domanda
   rispondeva col primo. Dire una cosa nuova non la cambiava: la seppelliva.

   ```text
   > il libro rosso è sul tavolo    → Imparato: located_in(book_red, tavolo).
   > il libro rosso è sulla mensola → Imparato: located_in(book_red, mensola).
   > dove si trova il libro rosso?  → mensola.        (prima: tavolo ⛔)
   ```

   **La macchina c'era già** (D40 di nuovo): `supersedes_in/3` + `holds_in/2` +
   `context_superseded/2` esistono in `context-scope.p0`, usate dal gen420 per
   «dimentica il mio nome». Mancava che i fatti del mondo le attraversassero.
   L'innesto è nel **punto di strozzatura condiviso** (`p0_learn_source`), che
   già costruisce la forma canonica `pred(a, b)` — così i due lati della
   supersessione non ricostruiscono lo stesso termine carattere per carattere,
   che è precisamente ciò che il gen420 aveva trovato fragile.

   **Il limite trovato sulla KB curata, e la cura.** `functional_relation(located_in)`
   è troppo forte da sola: «where is rome» → *«Italy and lazio»*, vere insieme,
   perché il Lazio sta **dentro** l'Italia. Il secondo valore raffina, non
   corregge, e superarlo cancellerebbe informazione vera in silenzio. Il
   criterio non è un elenco di eccezioni: `supersession_exempt/3` chiede alla
   relazione stessa se i due valori si contengono.

   Ratchet: `tests/p0t/conversation/correction_supersedes.p0t`, 12 assert, col
   raffinamento e col contrasto (correggere il libro non sposta il quaderno).
   **Debito lasciato aperto e registrato in §7bis:** la via locativa cancella il
   fatto vecchio a monte, e lo faceva già prima. G5 chiude la metà che si vede.

6. **◐ GD8 — metà chiusa 2026-09-01.** Le preposizioni articolate sì, la frase
   a tre ruoli no.

   **PARROT0 NON SAPEVA RILEGGERE CIO' CHE SCRIVE.** È il reperto più grosso del
   giro, e non l'avevo cercato:

   ```text
   > il libro è sul tavolo        → Imparato: libro SI TROVA in tavolo.
   > il libro si trova sul tavolo → Non capisco ancora.        ⛔
   ```

   parrot0 pronuncia «si trova» ogni volta che impara un luogo, e non sa
   rileggerlo: la sua **uscita** non era nella sua **lingua d'ingresso**. Chi
   ripete a parrot0 le parole che parrot0 ha appena detto sbatteva contro un
   muro — e questi turni rubati erano **inattribuibili** su qualsiasi misura del
   corpus, perché il turno sembra una frase dell'utente come tutte le altre.

   **L'invariante, che vale oltre questo caso:** *tutto ciò che parrot0 dice deve
   essergli comprensibile.* È verificabile — si prende una resa e la si rimanda
   indietro — ed è il gate `tests/p0t/conversation/self_echo.p0t`. **Ogni resa
   nuova che nomina una relazione dovrebbe passare di qui prima di dirsi finita.**
   La cura non è stata un frame nuovo: «si trova» *è* la copula locativa, e
   portarla alla copula la fa arrivare alla lettura che già esisteva.

   **Le articolate erano una classe enumerata a mano.** Aggiunte tre volte,
   ogni volta tappando il buco n-esimo (il commento del gen389 dice già «le
   articolate mancanti trovate misurando»); il 2026-09-01 è toccato a «nello».
   Un buco che si ripresenta tre volte non è un buco. Ora è un paradigma su due
   assi — preposizione × articolo — e **la glossa dipende solo dalla
   preposizione**, quindi «sullo» con glossa «in the» è impossibile per
   costruzione. Additivo: le righe enumerate restano.

    **Resta aperto:** «ho messo il libro sul tavolo» (tre ruoli, agente
    implicito) e «metto il libro sul tavolo», che oggi viene **dirottato da un
    generatore di finzione** — registrato in §7bis.

    **Implementazione avviata nel ciclo 2026-09-01 sera:** `extract_frame/2`
    genera ora frame di trasferimento locativo da verbi e preposizioni dichiarati
    in KB; `frame_projection/4` proietta i ruoli agente-oggetto-destinazione sul
    fatto pubblico `located_in/2`. La canonicalizzazione KB porta già le
    preposizioni articolate alla forma dichiarata; il gate resta da eseguire
    nella sessione finale.

7. **✅ D41 — chiusa 2026-09-01. PARROT0 NON SAPEVA RILEGGERE CIO' CHE SCRIVE.**
   Il reperto più grosso della giornata, e non lo stavo cercando:

   ```text
   > the book is on the table   →  Learned: located_in(book, table).
   > located_in(book, table)    →  That looks like a snippet of code.
   ```

   Due difetti in una riga: la risposta principale **non era lingua**, ed era
   notazione che parrot0 **non riconosceva come propria**. I turni rubati che ne
   nascono sono **inattribuibili** su qualunque misura del corpus — il turno
   sembra una frase dell'utente. Solo l'eco li separa, ed è per questo che
   serviva uno **strumento**: `scripts/self_echo_audit.py` rimanda a parrot0 le
   sue stesse frasi e ha trovato quattro difetti al primo colpo.

   **Invariante:** *tutto ciò che parrot0 dice deve essergli comprensibile.*
   Forma forte, il punto fisso: `whale is a mammal` → `Learned: whale is a
   mammal.` Gate: `tests/p0t/conversation/self_echo.p0t`.

   **La cura è che dire è l'inversa di leggere:** `say_frame(P, Pat) :-
   extract_frame(Pat, P)`. Un oggetto solo invece di un frasario destinato a
   divergere dai pattern di lettura — e l'invariante vale **per costruzione**.

   **Vocabolario trovato nel C:** `snprintf(msg, "Learned: %s(%s).", cls, subj)`
   — inglese cablato nel motore, invisibile a chi cercasse una resa in KB, nel
   punto in cui ogni fatto imparato passa.

   Ratchet aggiornati: 32 file asserivano la notazione. Ritirati, non allineati
   a un comportamento qualunque, e ognuno porta scritto in testa perché.
6. **G5** — il referente che sa ridirsi. Chiude anche la resa `book red` →
   «il libro rosso».
7. **Gate finale del giro:** `gd1_011` da capo a fondo (6 turni: due setup, due
   ordinali, una correzione, una callback).

## 7. Debiti aperti, dichiarati

### 7bis. ⛔ REGISTRO DEI ROSSI DA RIPARARE — non si ferma la marcia per questi

> F., 2026-09-01: *«quando trovi test che non passano annotali come da fixare ed
> andiamo avanti… per adesso espandiamo le sue capacità poi chiudiamo i piccoli
> sideeffect derivati, andiamo sui macro obiettivi.»*
>
> Regola d'uso: un rosso entra qui **con la misura differenziale** (mio o
> pre-esistente) e con il turno esatto. Un rosso senza differenziale non e' una
> voce di questo registro: e' un lavoro non fatto.

| test | stato | turno | mio? | nota |
|---|---|---|---|---|
| `conversation/chitchat.p0t` | 21/1 | «rough day» → risponde con la chiusura invece della resa low-energy | **no** — identico a HEAD (stash/build/unstash) | due rese smalltalk competono; classe, non istanza |
| `language/motorize_class.p0t` | 23/1 | «Who wrote the Iliad?» → «homer» col solo fatto sull'Odissea | **sì**, G1/G2 | la descrizione risolve troppo: manca il vincolo che la chiave sia dell'entita' giusta |
| suite lente in coda ai file | vari | qualunque turno dopo N nella stessa sessione | **no** | non e' logica: e' il debito di latenza, §7 sotto |
| ratchet `.p0t` non ancora passati in rassegna | — | attese `Learned: pred(arg)` residue nei file dopo `language/*` in ordine alfabetico | **sì** (D41) | la passata e' stata fermata a meta' su richiesta di F. per non perdere tempo in test lunghi; 32 file allineati, il resto quando si fa il giro di test giusto |
| «Moby Dick» → «Dick» | — | «il mio libro si chiama Moby Dick» | **no** | il nome multi-parola viene troncato all'ultima parola: stessa famiglia di D39, ramo diverso |
| articolo italiano `un stella` | — | «il sole è una stella» → «Imparato: sole è un stella» | **sì** (D41) | un/uno/una dipendono dal GENERE, che parrot0 non ha; dichiarato sul posto in grammar.p0 |
| `metto il libro sul tavolo` dirottato | — | risponde «A low hum filled the room as the dust lifted into the air.» | **no** | un generatore di finzione rivendica una frase dichiarativa ordinaria: manca la guardia, non la lettura |
| **«i put …» non si impara** | — | «i put the book on the table» → muro; «we put …» / «he put …» / «mary put …» funzionano | **no** — pre-esistente | `function_word(it, i, "the")`: l'articolo plurale italiano riscrive il pronome inglese. La guardia di lingua esiste in `function_word/3` ma **non** nella tabella `lex[]` in C. Diagnosi completa nel §3 dell'handoff 2026-09-02 |
| **la via locativa CANCELLA il fatto corretto** | — | «il libro rosso è sul tavolo» poi «…sulla mensola»: `located_in(book_red, tavolo)` non e' piu' dimostrabile | **no** — identico a HEAD | ⛔ viola `own_method(contradiction)` (*«I do not overwrite one claim with the other»*). G5 ha chiuso la meta' che si vede (la risposta ora e' quella in forza); la storia sopravvive in `supersedes_in`, il FATTO no. Finche' e' aperto, parrot0 dichiara un metodo che esegue a meta' |


- **⚠ LATENZA — il debito che ora si vede nei test, misurato il 2026-09-01.**
  F. ha detto esplicitamente: *«non ti preoccupare di questa lentezza poi la
  gestiamo»*. Non e' rimossa, e' **rimandata con i numeri in mano**, perche' la
  prossima persona non la ridiagnostichi da zero:
  - Il turno lento **non e' lento in isolamento**: «dove si trova milano» in un
    file fresco passa sotto 1,0 s. Diventa lento **dopo N turni nella stessa
    sessione**. La latenza e' **funzione dei fatti accumulati nel turno**, non
    della frase.
  - Percio' i rossi si concentrano **in fondo ai file di test**, e sembrano
    rotture logiche mentre sono scadenze di budget. `attributed_belief.p0t`
    fallisce 5 volte **sullo stesso turno**: e' un solo fenomeno, non cinque.
  - **Misura differenziale, il modo giusto di attribuirla** (stash / build /
    stesso file / unstash): `attributed_belief.p0t` → **6 passed a HEAD, 7
    passed con le modifiche di questo giro**. Le modifiche di GD12+G3
    **chiudono** una rottura e non ne aprono nessuna; il resto e' il debito.
  - E' la stessa cosa gia' nota come *lookup O(n) e boot quadratico*: la KB che
    cresce degrada il motore. Vedi D21 e §10 del piano frontier.
  - **Quando la si affrontera', la regola e': non alzare i budget.** Un budget
    alzato nasconde il fenomeno invece di misurarlo. I blocchi nuovi dichiarano
    `!timeout 4` **con la misura scritta accanto**, che e' una cosa diversa da
    un budget gonfiato in silenzio.


- **`motorize_class.p0t` 23/1** — regressione di G1/G2: «Who wrote the Iliad?»
  risponde «homer» con il solo fatto sull'Odissea. F. ha chiesto di costruire le
  fondamenta e riparare i side effect più avanti; **non è dimenticata**.
- **Rossi preesistenti**, verificati identici prima delle modifiche: `repair`
  (l'oracolo non compila in questo ambiente), `check_sort`, `forget_move`,
  `greet` 7/1, `games`, `faceted_enumeration`, `foundational_concepts`,
  `gap_dialogue`, `name_is_knowledge`, `reactions_are_knowledge`,
  `gap_is_a_fact`, `gap_anchor`, `frontier_chat_audit.it` riga 97,
  `assisted_construction` 65/1.
- **SC40 — le letture sono congelate** (rileggere accumula invece di rivedere):
  resta il TODO numero zero del piano frontier.
- **GD3 — le varianti ortografiche** azzerano una lezione. Trovata due volte,
  la seconda addosso a me: scrivendo la sonda avevo digitato `e'` invece di `è`
  e parrot0 non riconosceva la copula.

## 8. Comandi utili

```sh
make build && make test-engine
./bin/parrot0 --test tests/p0t/language/document_claims.p0t     # 182 assert
python3 scripts/dialogue_corpus_probe.py <corpus.tsv>           # la misura
python3 scripts/teach_lexicon.py <lexicon.tsv> [--save] [--lang it]
python3 scripts/teach_ladder_audit.py <ladder.tsv>              # dove arriva l'insegnamento
python3 tests/probes/reference_probe.py                         # la mossa dell'oracolo
```

Dentro la chat: `who answered?` nomina il modulo che ha risposto — **chiedilo
invece di dedurre il colpevole dall'esito**; `/debug <predicato>` ispeziona la KB.

---


Coda dei temi da far apprendere a parrot0 con il protocollo di
[`LEARN_PROTOCOL.md`](LEARN_PROTOCOL.md). Ogni voce è un'unità di lavoro: si
apre una sessione, si eseguono i gate, si committa e si pusha l'incremento.

**Non è una lista della spesa di fatti.** Un tema entra qui solo se chiude una
*classe* di fallimenti, e ogni voce dichiara la classe che chiude. Il criterio è
quello del §8 di [`docs/plans/apprendimento-assistito.md`](docs/plans/apprendimento-assistito.md):
*preferire il gap che, chiuso, libera più famiglie di frasi.*

## Da dove si pescano i temi

Oltre a questa coda, il serbatoio è [`docs/llmscores/`](docs/llmscores/): **531
prompt** estratti dalle 48 revisioni storiche di `LLMSCORE.md` e dalle sonde
tematiche, di cui **315 sono muri** — prompt su cui parrot0 ha preso 0 da un
giudice esterno — divisi in venti file per tema.

È materiale già filtrato dalla realtà: non prompt inventati da noi, ma
fallimenti veri davanti a un giudizio esterno. Quando una voce di questa coda è
chiusa e non si sa da dove ripartire, si apre il file del tema più promettente e
si prende una **famiglia** di muri, mai un prompt solo — chiudere l'istanza
senza chiudere la classe non conta come progresso.

I prompt marcati «già vinti» in quei file servono al verso opposto: se uno torna
a murare, è un tema da riapprendere e va in P4.

## Come si usa

1. Si prende la voce più alta non ancora fatta nella sezione applicabile.
2. Si esegue `LEARN_PROTOCOL.md` per intero — baseline, lezione, replay,
   transfer, contrasto, composizione, ablation, quarantena, `/save`, verifica in
   processo nuovo, report, commit, push.
3. Se la lezione **non è insegnabile parlando**, ci si ferma: è un risultato
   diagnostico, si tipizza il gap e si registra qui. Non si scrive il fatto a
   mano.
4. Se un tema è già stato fatto ma il comportamento è regredito, si **riapprende**
   — la voce torna in coda con la nota del perché.

Le tre regole che non si negoziano, dal protocollo: conoscenza **vera** e
fontata; nessuna API travestita; `X = 0` prima di `/save`.

## Missione attiva 2026-08-29 — dalla lettura alla supercomprensione

Questa e' la coda operativa delle prossime sessioni. Nasce dal confronto fra
[`apprendimento-assistito.md`](docs/plans/apprendimento-assistito.md) e il
fronte K0-K11 di
[`frontier-kb-natural-dialogue.md`](docs/plans/frontier-kb-natural-dialogue.md):
parrot0 possiede fatti, scope, proof, situazioni e piani, ma non possiede ancora
un oggetto comune per **cio' che un documento sta facendo**. Una pagina
scientifica non e' una sequenza di fatti: propone una domanda, delimita uno
scope, adotta assunzioni, descrive un metodo, riporta osservazioni, argomenta una
conclusione, qualifica un limite e lascia problemi aperti.

La missione non autorizza una sessione lunga prima del cancello M0-M14. Ogni
voce viene consumata con una sonda breve del `LEARN_PROTOCOL`; se la lezione non
e' indirizzabile parlando, l'esito e' `diagnostic`, si promuove soltanto il
motore metacognitivo generale che manca e il protocollo riparte in un processo
nuovo. Le fonti devono essere primarie o istituzionali e il testo insegnato
viene parafrasato: una fonte non diventa un corpus copiato nella KB.

### Ordine di consumo

| # | Piano complesso | Classe liberata | Interazione didattica minima | Gate duro | Stato |
|---|---|---|---|---|---|
| **SC0** | **Baseline stratificata su prosa narrativa, espositiva e scientifica** | distingue muro, fatto estratto, misclaim, perdita di subordinata e perdita di struttura documentale | presentare tre brani veri brevi, poi chiedere tesi, supporto, sequenza e limite senza anticipare le risposte | transcript classificato frase per frase; zero conoscenza consolidata; mappa M0-M20 e SC1-SC16 | **CHIUSA come diagnosi** — 2026-08-29: falso racconto e falsa autodiagnosi isolati; porta di lettura resa insegnabile; route Transfer@3 = 3/3; estrazione complessa = 0/9. Report: [`2026-08-29-supercomprensione-sc0.md`](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc0.md) |
| **SC1** | **Unita' documentali e relazioni retoriche** | una frase o sezione puo' essere definizione, sfondo, contrasto, causa, metodo, risultato, limite o transizione | «qui la seconda frase contrasta la prima», «questa frase descrive il metodo, non il risultato» | una cue retorica nuova insegnata a voce cambia la segmentazione; transfer su tre domini; retract la rimuove | **SC1-A CHIUSA; SC1-B aperta** — 2026-08-29: unita' ordinate e span stabili di sessione; cue naturale persistita; `Transfer@3=3/3`; ablation/reteach causali; restano ID fonte, atti, cue multi-parola e archi intra-periodo. [Report SC1](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc1.md) |
| **SC2** | **Claim tipati, attribuzione e forza epistemica** | separa osservazione, dato, inferenza, ipotesi, assunzione, definizione, citazione e raccomandazione | «gli autori osservano X ma concludono Y», «Z e' un'ipotesi, non un risultato» | nessun claim perde fonte, scope o status; un fatto riportato non diventa automaticamente commitment di parrot0 | **SC2-A e SC2-B CHIUSE; SC2-C aperta** — 2026-08-29. SC2-A: documenti source-addressed, claim di superficie con span, attribuzione, status/context/commitment derivati ([report](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc2.md)). SC2-B: normalizzazione come **fase pura e contestuale**, `NormalizedClaimCoverage 0/8 -> 9/9`, `QuestionAnswerCoverage 0/3 -> 3/3`, `WorldCommitLeak=0`, letture parziali rifiutate per policy ([report](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc2b.md)). Resta l'equivalenza fra frame, la modalita' e la prova pronunciata |
| **SC3** | **Grafo argomentativo e dipendenze della conclusione** | estrae premesse, conclusioni, supporti, obiezioni, qualificatori e rebuttal da prosa articolata | «questa osservazione sostiene la conclusione solo insieme a quest'altra premessa» | domande `perche'`, `da cosa dipende`, `cosa la confuterebbe`; ablation di una premessa ritira solo le conclusioni dipendenti | **SC3-A CHIUSA; SC3-B aperta** — 2026-08-29: arco di supporto fra claim (nessun terzo estrattore), supporto come vista VIVA, `unsupported_conclusion/1` dichiarata invece che taciuta, connettivo avversativo != argomento, due ablation che spengono due cose diverse, forma interrogativa che cresce parlando. Restano **supporti congiunti** (gate 3 del §18.15, vedi D23), obiezioni/qualificatori/rebuttal, «cosa la confuterebbe» e la direzione `ground`. [Report SC3](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc3.md) |
| **SC27** | **Un supporto congiunto non e' due supporti** | D23 | «A e B insieme sostengono C» non deve diventare due archi indipendenti; togliere una parte di una premessa congiunta spegne la conclusione intera | «therefore e' un marcatore di supporto congiunto» | congiunto: ritrarre una qualunque premessa porta la conclusione a `lost`; senza modo dichiarato l'ambiguita' si conserva e si dice | **CHIUSA — 2026-08-30.** Il difetto era peggiore del previsto: con tre unita' la prima premessa **spariva in silenzio** (l'arco retorico e' fra unita' adiacenti) e parrot0 rispondeva «poggia su B» con la stessa sicurezza. Ora tutte le premesse sono visibili e la risposta le nomina tutte; il modo si insegna e, se non e' dichiarato, lo stato e' `undetermined_mode` invece di una scelta. Limite dichiarato: su un documento lungo la lettura sovra-raccoglie, che e' l'errore giusto da fare qui |
| **SC28** | **La citazione e' esente dalla lingua che la contiene** | D24 | cio' che e' menzionato non partecipa alla lingua che lo menziona; una lezione quotata entra nella KB byte per byte | «"what is that based on" e' una forma con cui ti si chiede su cosa poggia» | dieci locuzioni quotate con copule, articoli, dimostrativi e ausiliari entrano esatte e si ritrattano con la stessa superficie; una perdita e' un errore dichiarato, non una riga da confrontare a occhio | **aperto.** Misurato: `"what is that based on"` entra come `claim_support_question("what based on")`. `canonicalization_exempt(mention)` esiste in `kb/core/input.p0` e non raggiunge questo percorso |
| **SC4** | **Ricostruzione del disegno scientifico** | riconosce domanda, popolazione/sistema, variabili, intervento, confronto, misura, controllo e confondenti | «il gruppo B e' il confronto; la temperatura e' mantenuta costante» | ricostruzione su esperimento, studio osservazionale e simulazione; non inventa controllo o causalita' | aperto |
| **SC5** | **Da metodo in prosa a procedura eseguibile e ispezionabile** | compila passi, input, output, precondizioni, invarianti, branch, criterio d'arresto, rischi e provenance | «"we then" e' un marcatore di passo»; «"before this" e' un marcatore di precondizione» | la lezione non viene eseguita; ogni passo ha provenienza; una procedura con un passo non legato si dichiara bloccata | **SC5-A CHIUSA; SC5-B aperta** — 2026-08-30: un passo di metodo e' una claim riportata con status `described` e contesto `reported_method`, quindi **nessun secondo estrattore** — il metodo e' una vista sulle claim, ordinata dall'ordine delle unita'. Soggetto eliso preso dall'agente attribuito (`elided_subject/2` + `agent_surface/2`). D29 applicata al saper fare: un passo non legato blocca la procedura e resta visibile. Restano input/output tipati, invarianti, branch, rischi e l'esecuzione. [Ratchet](tests/p0t/language/document_method.p0t) |
| **SC37** | **Una cue di ripetizione indipendente dal criterio d'arresto** | D29 | «questo si ripete» e «finche' X» sono due letture diverse: oggi la stessa cue porta entrambe, quindi ritrarre l'arresto toglie anche il ciclo e la porta `blocked(no_stop_criterion)` non e' misurabile | «"we repeat" e' un marcatore di ripetizione» | ritrarre il criterio d'arresto lascia vivo il ciclo e porta la procedura a `blocked(no_stop_criterion)` | **aperto.** La clausola esiste gia' in `document-method.p0` come seme dichiarato e non provato |
| **SC6** | **Modello causale dal testo** | distingue correlazione, meccanismo proposto, causa necessaria/sufficiente, mediatore e confondente | «X e Y covariano; il testo non dice ancora che X causa Y» | controfattuale e intervento coerenti; un'associazione non produce un arco causale senza evidenza dichiarata | aperto |
| **SC7** | **Modellizzazione nello spazio logico** | costruisce mondi/modelli compatibili, vincoli, invarianti, conseguenze, controesempi e residui | «cerca un caso in cui le premesse valgono e la conclusione no» | entailment tramite assenza di contromodello entro budget; se la ricerca e' incompleta lo dichiara; transfer fra logica, scienza e procedure | aperto |
| **SC8** | **Scope complesso, quantificazione e modalita'** | comprende `tutti`, `alcuni`, `nessuno`, `solo se`, `a meno che`, possibilita', necessita' e negazione annidata | «qui `solo se` introduce una condizione necessaria, non sufficiente» | coppie minime positive/negative; nessuna inversione per somiglianza superficiale; nuova forma modale assert/retract | aperto |
| **SC9** | **Coreferenza documentale e identita' attraverso paragrafi** | collega pronomi, ellissi, abbreviazioni, nomi alternativi, campioni e variabili senza fondere entita' diverse | «in questo paragrafo `esso` riprende il campione, mentre `questo risultato` riprende l'osservazione» | catene a 20+ frasi; ambiguita' conservata; correzione locale ri-deriva soltanto i legami dipendenti | aperto |
| **SC10** | **Equazioni, unita', tabelle e figure come proposizioni** | collega simboli, definizioni, dimensioni, assi, righe e didascalie al testo che li interpreta | «qui k e' una costante di velocita', non una misura osservata» | controllo dimensionale; domanda incrociata testo-tabella-equazione; simbolo nuovo insegnato senza nome interno | aperto |
| **SC11** | **Sintesi compressiva con copertura semantica** | produce un nucleo minimo da cui le proposizioni importanti restano derivabili | «riassumi conservando domanda, metodo, risultato, limite e nesso fra loro» | claim coverage, nessun claim nuovo, proof verso le unita' fonte, versione breve/lunga dalla stessa struttura | aperto |
| **SC12** | **Integrazione fra documenti e gestione del dissenso** | allinea concetti e risultati fra fonti senza cancellare scope, metodi e popolazioni diverse | «questi lavori sembrano divergere, ma usano misure e condizioni diverse» | accordo, conflitto reale e apparente distinti; ogni sintesi conserva fonte e contesto; nessuna media di confidence inventata | aperto |
| **SC13** | **Generazione di ipotesi e falsificatori** | propone spiegazioni candidate da residui, analogie strutturali e modelli concorrenti | «proponi due meccanismi compatibili e l'osservazione che li distinguerebbe» | ipotesi marcate, non promosse a fatti; previsione discriminante; controllo negativo; ablation dell'analogia toglie solo l'ipotesi | aperto |
| **SC14** | **Lettore metacognitivo attivo** | sceglie se rileggere, definire, risolvere un riferimento, cercare una premessa, chiedere o sospendere | «non ti manca il fatto: non sai a cosa si riferisce `questo`» | gap corretto su batteria eterogenea; domanda di chiarimento cambia davvero la decisione; nessun menu fisso di remediation | aperto |
| **SC15** | **Induzione metacostruttiva da esempi e controesempi** | induce una costruzione o un operatore senza che il teacher nomini lo schema interno | mostrare tre esempi veri e un contrasto, poi chiedere che cosa hanno in comune | candidato in quarantena; transfer 3/3; contrasto; descrizione naturale dei ruoli; retract; nessun predicato suggerito dal teacher | aperto |
| **SC16** | **Comprensione ricorsiva e critica di un intero articolo** | compone SC1-SC15: mappa, interroga, esegue procedure, critica limiti e aggiorna il modello | articolo open-access mai usato nel training, sezioni presentate progressivamente | almeno 70% held-out; tesi/metodo/risultati/limiti completi; domande avversariali; nessuna risposta oltre la proof; fresh-process recall 100% sui fatti promossi | aperto |

### Pacchetti di sessione

Le righe sopra non si affrontano in parallelo: ciascun pacchetto produce il
prerequisito del successivo.

1. **Pacchetto A — sensore (SC0, SC1, SC2).** Tre testi corti, una sola nuova
   relazione retorica per ciclo, classificazione completa dei misclaim. Uscita:
   Document IR minimo oppure referto del meta-gap che ne impedisce la nascita.
2. **Pacchetto B — ragione del testo (SC3, SC4, SC6).** Ricostruire un argomento
   e un disegno sperimentale; separare evidenza, inferenza e causalita'. Uscita:
   grafo interrogabile con provenance e ablation causale.
3. **Pacchetto C — fare e simulare (SC5, SC7, SC8, SC10).** Compilare una
   procedura, verificarla nello spazio dei modelli e legare simboli/unita'.
   Uscita: piano eseguibile che sa anche quando non e' giustificato.
4. **Pacchetto D — mantenere e comprimere (SC9, SC11, SC12).** Attraversare
   paragrafi e fonti, poi comprimere senza amputare tesi o limiti. Uscita:
   sintesi proposizionale reversibile verso le fonti.
5. **Pacchetto E — superare il teacher (SC13, SC14, SC15).** Usare i residui
   per scegliere la prossima domanda, indurre candidati e proporre
   falsificatori. Uscita: crescita guidata dall'informazione, non da un menu.
6. **Pacchetto F — integrazione (SC16).** Articolo held-out e replay ostile.
   Uscita: report del protocollo, non una dichiarazione impressionistica di
   comprensione.

### Metriche aggiuntive per questa missione

Le metriche del protocollo restano obbligatorie. Per la prosa complessa si
aggiungono:

- **Claim coverage:** claim corretti e collegati / claim rilevanti nella fonte;
- **Relation fidelity:** archi argomentativi corretti / archi estratti;
- **Source fidelity:** proposizioni con scope e provenance corretti /
  proposizioni affermate;
- **Procedure executability:** passi applicabili e tipati / passi estratti;
- **Countermodel yield:** conclusioni universali respinte da un controesempio
  valido / conclusioni universali false provate;
- **Compression with recovery:** claim recuperabili dalla sintesi / claim
  rilevanti prima della sintesi;
- **Diagnostic addressability:** gap per cui parrot0 formula la domanda
  didattica corretta / gap osservati;
- **Cross-genre transfer:** operatori che passano da prosa scientifica a
  normativa, tecnica e narrativa senza nuovo C.

Una risposta scorrevole non contribuisce a nessuna di queste metriche se il
grafo che la sostiene non e' interrogabile.

### Checkpoint lasciato da SC0

SC0 ha separato due problemi che prima apparivano come uno solo:

1. **accesso al lettore:** chiuso per la classe degli introduttori. Una forma
   nuova si insegna ancorandola a una forma funzionante, senza nomi interni; il
   ruolo puo' possedere l'intero payload, la KB sceglie la facolta' eager e il
   suo esito terminale non viene riclassificato come muro;
2. **comprensione del contenuto:** ancora rossa. Sui tre testi finali Apollo,
   subduzione e forza epistemica il lettore ha instradato 3/3, ma ha riportato
   `0` fatti e `9` frasi saltate. Non esistono ancora unita' documentali, claim
   tipati o archi retorici interrogabili.

Per questo la prossima voce e' **SC1**, non un altro sinonimo di `read:`. Il
primo gate dovra' insegnare a voce una relazione fra due unita' (almeno
contrasto o metodo/risultato), conservarne gli span e ritirarla senza perdere
il contenuto proposizionale.

### Handoff operativo dettagliato: riprendere da SC1

Questa sezione e' il punto di ripresa autoritativo per un agente che non ha
assistito alla sessione SC0. Non reinterpretare il risultato a partire dalla
sola risposta finale di parrot0: leggere prima il
[report SC0](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc0.md),
poi questa sezione, poi il codice indicato sotto.

#### Confine esatto gia' raggiunto

- Il commit `a722689` contiene il primo checkpoint: la lezione naturale
  `role_for`, le sue forme KB-backed, i template di risposta e il primo
  deferral verso la faculty dichiarata. E' gia' su `origin/main`.
- Il commit immediatamente successivo, quello che contiene questo handoff,
  chiude SC0 con l'**envelope epistemico insegnabile**: estensione dell'input,
  dispatch della faculty e politica del risultato sono fatti KB, mentre C fa
  soltanto binding, ordinamento e arbitraggio.
- Il test mirato persistente
  `tests/p0t/language/taught_segment_role.p0t` chiude **21/21** assert nello
  stesso processo: baseline, lezione, replay, transfer, collisione con
  `measurement error`, ablation e reteach.
- Il solo `make soft-test` permesso dal protocollo e' gia' stato consumato in
  questo ciclo. Il risultato e' **55 passati, 1 fallito**, nel test preesistente
  `frontier_chat_audit.it.p0t` alla riga 97: atteso
  `I don't know about designation`, ottenuto
  `I don't know much about your designation yet. Want me to look it up?`.
  Non attribuire questo rosso a SC0 e non rieseguire il soft-test per
  "provare ancora": nel prossimo ciclo, dopo una modifica engine, spetta una
  sola nuova esecuzione.
- Il dato che conta e' doppio: **Route Transfer@3 = 3/3**, ma **Claim coverage =
  0/9**. Apollo, subduzione e il contrasto associazione/causalita' raggiungono
  il lettore; il lettore non costruisce ancora claim complessi interrogabili.

Non investire il prossimo ciclo in altri sinonimi di `leggi:`. La porta e'
dimostrata e retraibile. La frontiera e' ora dentro il documento.

#### Mappa minima dell'implementazione da non rompere

1. `src/brain/00-lex.c`, `try_teach_form`, implementa il modo generico
   `role_for`. Il teacher insegna una forma nuova ancorandola a una forma viva,
   per esempio `impara "Leggi questo breve testo:" come un altro modo per
   introdurre "leggi:"`. Il codice non conosce quella frase: risolve il ruolo
   dell'anchor attraverso l'evidenza `input_segment` e verifica una
   `faculty_for` realmente disponibile.
2. `kb/core/intents.p0` contiene le etichette naturali learnable di `role_for`;
   `kb/core/messages.p0` contiene le conferme. Aggiungere una lingua o una
   parafrasi significa crescere queste famiglie nella KB e provarne
   assert/retract a runtime, non aggiungere confronti lessicali in C.
3. `src/code.c`, all'inizio di `input_segment`, applica la proprieta' aperta
   `segment_extent(Role, whole)`: se un ruolo insegnato possiede il payload,
   viene pubblicato un unico span prima che i registri interni competano. E'
   il fix generale che impedisce alla parola interna `error` di trasformare un
   brano scientifico in un log del compilatore. In caso di evidenza pari deve
   emergere ambiguita', non una preferenza nascosta.
4. `kb/core/input.p0` dichiara attualmente
   `segment_extent(prose_source, whole)`, `faculty_dispatch(reader, eager)` e
   `module_result_policy(reader, terminal)`. Da questi fatti deriva il
   precedence meccanico. Sono policy aperte: una nuova faculty deve poter
   entrare aggiungendo fatti e test, senza una nuova branch nominativa.
5. `src/brain/99-registry.c` fa il join generico
   `input span -> faculty_for/2 -> faculty_dispatch/2 -> registry C`, offre
   prima l'unica faculty eager e, se ve ne sono piu' di una, lascia
   l'arbitraggio al percorso ordinario. La policy `module_result_policy`
   impedisce che il risultato onesto del reader (`0 learned, N skipped`) sia
   scambiato per resa e sovrascritto da una risposta generativa.
6. Il reader vero e' in `src/brain/30-generation-reading.c`:
   `mod_reader -> read_passage -> extract_clause`. `extract_clause` pubblica
   la struttura sotto `current_prose`, prova `input_assertion_bundle` e
   `input_frame_commit`, poi i percorsi legacy. `read_passage` conserva una
   proposizione originale soltanto se qualcosa e' stato estratto.
7. `current_prose` e' oggi **transiente per clausola**: viene ripulito prima
   della clausola successiva. Questo e' probabilmente il primo ostacolo reale
   a SC1, perche' un arco retorico fra due clausole non puo' appoggiarsi a nodi
   gia' cancellati. Non risolverlo con una lista di connettivi in C. Serve una
   pubblicazione documentale stabile di sessione, con identita', ordine, span
   fonte e receipt di commit, prima del clear.
8. Esiste gia' una scomposizione concessiva in `src/brain/99-registry.c`, prima
   del registry: separa forme come `although ..., ...` consultando
   `subordinator_stance/2` in `kb/core/grammar.p0`. E' utile e non va rimossa,
   ma oggi conserva al massimo lo stance dei due lati: non crea una relazione
   retorica persistente, non prova la provenance e non sostituisce Document
   IR.
9. Esistono gia' `contrastive_connector/1` in `kb/core/lexicon.p0` e
   `subordinator_stance/2` in `kb/core/grammar.p0`. Prima di implementare un
   nuovo protocollo, provare se la lezione generica di classe accetta gia'
   `"albeit" is a contrastive connector` e se retract ne elimina davvero
   l'effetto. Non assumere che una riga statica equivalga a teachability.
10. `input_structure_publish` fornisce gia' token, span e phrase node. Riutilizzare
    quella geometria: duplicare un secondo tokenizer o ricostruire offset da
    stringhe rendera' impossibile la provenance precisa.

#### Prossimo esperimento minimo: SC1, non ancora SC2

Obiettivo stretto: due unita' di un documento sopravvivono all'analisi della
singola clausola; una relazione retorica insegnata a voce le collega; ablation
della sola cue o regola rimuove l'arco senza cancellare le proposizioni.

Usare almeno tre generi veri e separare sempre **baseline**, **lezione**,
**replay**, **transfer** e **ablation** nello stesso processo:

1. una concessiva fattuale breve, per esempio la struttura «Although X, Y»,
   con entrambi i lati veri e verificabili;
2. il caso Apollo 13 gia' documentato dalla NASA: la missione non alluno', ma
   fu considerata riuscita per il rientro salvo dell'equipaggio;
3. un passaggio scientifico in cui un'associazione osservata non autorizza una
   conclusione causale, preso da una fonte istituzionale o open-access mai
   memorizzata prima.

Prima della lezione chiedere esplicitamente: quali claim afferma ciascuna
unita', che relazione c'e' fra le due, quale span sostiene ogni risposta e se
entrambi i claim sono stati committed. La risposta corretta non basta: devono
esistere nodi interrogabili e proof verso gli span.

La prima lezione da tentare e' una lezione naturale di classe gia' disponibile,
per esempio `"albeit" is a contrastive connector`. Se la classe viene
assertita ma non cambia la relazione documentale, il gap non e' lessicale: e'
il consumer. Solo allora introdurre il piu' piccolo producer/view KB capace di
trasformare struttura e classe in una relazione. Una correzione come «qui la
seconda unita' qualifica la prima» deve restare un obiettivo successivo, non un
pretesto per esporre al teacher nomi di predicati interni.

I nomi `document_unit`, `unit_act`, `unit_source_span` e `rhetorical_edge` sono
ipotesi di lavoro, non uno schema gia' approvato. Prima di aggiungerli cercare
relazioni equivalenti con `rg`; poi scegliere la rappresentazione minima che
permetta queste query:

- quali unita' compongono il documento e in quale ordine;
- quale atto/ruolo ha ogni unita';
- quale arco collega source e target;
- quali byte o token della fonte giustificano nodo e arco;
- da quale regola/cue deriva l'arco;
- che cosa resta dopo retract della cue.

#### Sequenza di lavoro consigliata a un agente successivo

1. Rileggere `MANTRA.md`, `PRINCIPLES.md`, `LEARN_PROTOCOL.md`, il report SC0 e
   il paragrafo 18 di `docs/plans/frontier-kb-natural-dialogue.md`.
2. Eseguire una sessione diagnostica senza `/save`. Catturare transcript e
   delta; se una correzione crea un fatto falso, chiudere con `X` e annotarlo.
3. Provare la teachability gia' esistente di `contrastive_connector` e
   `subordinator_stance`; non aggiungere C finche' non e' chiaro se manca il
   sensore, il consumer, la memoria documentale o la query.
4. Progettare il piu' piccolo strato Document IR. Se serve C, limitarlo a
   meccanica fissa: numerazione delle unita', copia degli span, ordine,
   transazione e receipt. Ruoli, nomi di cue, direzione e semantica degli archi
   devono venire dalla KB.
5. Scrivere prima il test di crescita nello stesso processo:
   baseline fallisce onestamente; lezione; replay; tre transfer; contrasto
   negativo; retract; replay negativo; reteach. Verificare separatamente che
   retract dell'arco non distrugga i claim contenuto.
6. Aggiungere query di audit via MCP o linguaggio naturale per ruolo, faculty,
   unita', span, arco, sorgente della regola e confidence. Senza audit, non
   dichiarare chiuso il gate.
7. Misurare claim coverage, relation fidelity e source fidelity, non la
   scorrevolezza della risposta. Ogni claim deve essere vero rispetto alla
   fonte scelta; nessun fatto del test va promosso in core/profile per far
   diventare verde il caso.
8. Eseguire `make build`, il test mirato persistente, `git diff --check` e una
   sola volta `make soft-test` dopo la modifica engine. Aggiornare report e
   questa sezione prima di commit e push.

#### Trappole gia' osservate

- Senza envelope, un brano lungo puo' finire nella generazione narrativa:
  Apollo e' diventato un racconto inventato. Non e' "quasi comprensione".
- La parola `error` dentro `measurement error` competeva col registro dei log.
  Conservare questo caso avversariale in ogni refactor di dispatch.
- Il sommario del reader con zero fatti veniva interpretato come resa e
  sovrascritto. La policy terminale e' parte del contratto, non cosmetica.
- Una correzione naturale mal compresa ha prodotto un falso `created_by(...)`.
  Se il delta contiene falsita', usare `X`, non `/save`.
- Diversi buffer canonici sono ancora limitati (in particolare percorsi da
  circa 256 byte): un testo lungo puo' troncare senza dimostrare un limite
  concettuale. Testare dapprima brani corti, poi stressare la lunghezza come
  asse separato.
- Il conteggio del reader e' oggi per frasi/clausole e nel probe PCR ha
  riportato `2 skipped` per un passaggio percepito come tre frasi. Non usare il
  contatore come sostituto di unita' documentali esplicite.
- Output grammaticalmente fluente o nella lingua giusta non prova estrazione.
  Chiedere claim, arco, span e proof.
- L'ordine dei moduli C non e' semantica. Per nuove facolta' usare fatti come
  `faculty_dispatch`, con tie osservabile, non spostare silenziosamente righe
  del registry.
- Sono vietati `cue(...)`, `strstr(...)` o `strcmp(...)` su nuovi letterali di
  lingua naturale in `src/brain`. Ogni superficie va nella KB e deve avere un
  test assert/retract senza rebuild.
- Non trasformare esempi scientifici, nomi propri o risposte attese in world
  facts del profilo. Le fonti vere restano input/test; la KB deve contenere la
  competenza trasferibile.

#### Comandi e disciplina di verifica

Per evitare che un test crei un processo nuovo a ogni assert, mantenere un
engine unico in una PTY con socket non condiviso:

```sh
make build
./bin/parrot0 --test-engine --sock /tmp/parrot0-sc1-<id>.sock
./bin/parrot0 --test tests/p0t/language/<nuovo-test>.p0t \
  --sock /tmp/parrot0-sc1-<id>.sock
git diff --check
make soft-test
```

Terminare l'engine della propria PTY con `Ctrl-C`. Non uccidere processi di
test non creati dalla sessione: nel workspace possono operare altri agenti.
`make soft-test` compare qui come passo del **prossimo ciclo engine**: SC0 lo ha
gia' eseguito una volta e non va ripetuto durante la sua chiusura.

Il commit SC1 dovra' essere autonomo e descrivere: ipotesi provata, lezione
naturale, fatto KB cresciuto, consumer che lo usa, test di ablation, metriche,
fonti, limite residuo e prossimo gate. Se una di queste voci manca, lasciare il
gate aperto nel TODO.

### Checkpoint lasciato da SC1 — punto di ripresa autoritativo

SC1-A e' chiusa. Leggere il
[report completo](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc1.md)
prima di modificare il lettore. Il risultato non e' «comprensione dei claim»:
e' il primo strato causale di Document IR sul quale SC2 puo' finalmente
costruire status epistemici senza ricominciare dal testo grezzo.

#### Che cosa e' stato dimostrato

- `document_unit_observe/4` copia superficie, token, range e ordine dalla
  gerarchia transiente `current_prose` **prima** che venga ripulita. Due frasi
  successive restano quindi due oggetti interrogabili nella stessa sessione.
- Il C genera soltanto l'identita' monotona `document_N`, l'ordine delle unita'
  e attiva il producer KB. La semantica vive in
  `kb/core/document-structure.p0`: classe del marker, relazione e direzione
  sono aperte e `rhetorical_edge/4` e' una vista derivata.
- La lezione naturale `"nevertheless" is a contrastive connector.` crea
  `contrastive_connector(nevertheless)`. Il consumer la usa subito senza
  rebuild: Apollo replay e tre transfer indipendenti producono due unita' e un
  arco di contrasto (`Transfer@3=3/3`). `therefore` non produce quell'arco.
- Il retract naturale elimina il membro della classe e rende non dimostrabile
  l'arco anche sui documenti gia' osservati, ma conserva unita', span e il fatto
  estratto `metal(mercury)`. Reteach e retention sono verdi.
- La stessa grammatica pura di menzione e' ora condivisa da learn/query/retract.
  Questo chiude il bug diagnostico che trasformava «forget that X is a
  contrastive connector» nel fatto falso `contrastive_connector(forget)`.
- La sola lezione linguistica e' stata promossa in una sessione pulita:
  `W=0, L=1, C=0, P=1, O=3, X=0, S=5`. Un processo nuovo parte da
  `36312 facts, 2418 rules`, ricorda la classe e ricostruisce l'arco senza
  ripetere la lezione (`FreshProcessRecall=2/2`).

Il ratchet permanente e' `tests/p0t/language/document_rhetoric.p0t`: 33 assert
nello stesso processo coprono baseline, crescita, replay, tre domini,
punteggiatura, composizione, contrasto, ablation, conservazione del contenuto,
reteach e retention. Sono verdi anche `mention.p0t` (24),
`taught_segment_role.p0t` (21) e `retract.p0t` (17 su engine ermetico).
`make build` e' verde. Il solo `make soft-test` del ciclo e' gia' stato
consumato: 55 verdi e il rosso preesistente di `frontier_chat_audit.it.p0t`
riga 97 sulla formulazione di `designation`. Non rilanciarlo per SC1.

#### Confini da non mascherare

1. `document_N` e `document_N_unit_M` sono locali al processo. Le sessioni che
   contenevano documenti non sono state salvate: fra due processi gli ID
   colliderebbero e falsificherebbero la genealogia. Non promuovere queste
   clausole finche' l'identita' non e' ancorata a fonte, versione e span.
2. L'arco attuale collega soltanto unita' adiacenti quando il primo token della
   seconda e' membro della classe dichiarata. Non copre cue multi-parola,
   marker interni al periodo, strutture annidate o archi a distanza.
3. Il sistema mappa la retorica ma non identifica ancora l'atto dell'unita':
   definizione, metodo, risultato, limite e raccomandazione restano indistinti.
4. `Claim coverage=1/8` sul nucleo replay+transfer. Un sommario con «Mapped»
   misura soltanto unita' e archi: non autorizza a dire che gli otto claim sono
   stati compresi.
5. Due sessioni di sviluppo abortite produssero ciascuna
   `contrastive_connector(forget)`. Sono `X=1` diagnostici, mai salvati. Il run
   promosso ha `X=0`; conservare entrambi i dati evita di nascondere il percorso
   causale che ha imposto il parser condiviso.
6. `reading_fact(...)` e' stato classificato come `O` e promosso dal normale
   `/save`. Questo comportamento e' un residuo M14 da riesaminare, non una
   clausola da filtrare retroattivamente durante SC2.

#### Mappa dei file e invarianti

- `src/brain/30-generation-reading.c`: apre il documento, numera le unita',
  chiama `document_unit_observe/4` prima del clear e rende un sommario
  KB-templated quando esiste almeno un arco. Non aggiungere qui parole come
  `however`, `method`, `result` o `hypothesize`.
- `kb/core/document-structure.p0`: producer e viste del Document IR. La classe
  e' invocata con `apply/2`; la regola per il token iniziale passa attraverso
  `document_unit_token_precedes/2` perche' la NAF con variabile libera non e'
  affidabile nel solver corrente.
- `src/brain/10-memory-knowledge.c`: parser puro per membership esplicita e
  parser della forma unaria multi-parola usato dal retract anticipato. Copule,
  determinanti e classi vengono interrogati nella KB. Ogni futura forma di
  correzione deve restare simmetrica alla sua forma di apprendimento.
- `kb/core/responses.p0`: il sommario documentale e la conferma di retract sono
  template KB. Per la lingua di default serve la forma `response_template/2`;
  soltanto `response_template(..., en, ...)` non viene scelta dal renderer
  default.
- `src/brain.c`: `document_seq` e' meccanica di sessione, non identita'
  persistente. Sostituirla, non attribuirle significato.

#### Prossimo esperimento minimo: SC2

SC2 deve separare tre cose che oggi coincidono accidentalmente: **frase
osservata**, **claim attribuito** e **commitment di parrot0**. Prima di
implementare, cercare con `rg` strutture riusabili (`holds_in`, provenance,
claim reificati, source/version, stance) e verificare i vincoli di K4: non creare
un secondo sistema di contesti se quello esistente puo' ospitare il documento.

L'esperimento didattico minimo usa un brano vero con due proposizioni della
forma parafrasata «gli autori ipotizzano X; i dati mostrano Y». Baseline e
lezione devono rendere verificabili separatamente:

- la frase fonte e il suo span;
- il contenuto proposizionale X o Y;
- l'attribuzione agli autori/documento;
- lo status `hypothesis` contro `observation`;
- l'assenza di commitment autonomo di parrot0 a X.

Poi servono `Transfer@3` su esperimento, studio osservazionale e simulazione,
un contrasto in cui «suggerisce» non equivale a «dimostra», composizione con
una relazione gia' nota, ablation della sola cue/status e replay negativo. Il
retract deve togliere la classificazione epistemica derivata, **non** la frase
fonte e non la proposizione attribuita. Una domanda equivalente a «il documento
ha osservato X?» deve rispondere no quando X e' soltanto ipotizzata, mentre
l'ipotesi deve restare interrogabile come ipotesi.

Prima viene l'identita' persistibile: source URI canonica o altro riferimento
fontato, versione/fingerprint e span devono determinare il documento e i claim.
Il contatore puo' restare una handle effimera, ma non deve entrare nel save come
identita' globale. Se il protocollo naturale non offre ancora un envelope per
insegnare fonte/versione, registrare quel gap e chiudere il ciclo come
diagnostico invece di inventare un URI in C.

Il test KB-first e' obbligatorio: una nuova cue di status o attribuzione deve
entrare parlando, cambiare l'analisi nello stesso processo e smettere dopo
retract, senza rebuild. Il C puo' costruire identita', copiare span, fare
binding/ordine e applicare transazioni; non puo' riconoscere letteralmente
`shows`, `hypothesize`, `authors`, `data` o le loro traduzioni. Status,
direzione, ruoli e template appartengono alla KB.

#### Strategia di crescita oltre SC2

Per aiutare agenti meno esperti, mantenere questa disciplina ad ogni SC:

1. aggiungere una sola nuova distinzione semantica per ciclo e provarne la
   causalita' con ablation;
2. conservare sempre la rappresentazione piu' debole (span/frase) quando quella
   piu' forte (claim/status/arco) viene ritirata;
3. derivare viste e risposte, non duplicare verita' materializzate difficili da
   invalidare;
4. promuovere in KB soltanto competenza linguistica/meta appresa e fatti veri
   fontati, mai i documenti diagnostici con handle locali;
5. misurare coverage e precision separatamente: aumentare gli archi con claim
   coverage rosso non e' supercomprensione;
6. portare ogni nuovo predicato attraverso proof/provenance e fresh process
   prima di comporlo con SC3-SC16;
7. dopo ogni modifica engine eseguire un solo `make soft-test`, annotare il
   primo rosso e non cambiare l'attesa per farlo sparire;
8. chiudere ogni checkpoint con report, aggiornamento di questo handoff, diff
   semantico, commit e push. Se un ID, uno status o un arco non ha una
   spiegazione di retract, non e' ancora pronto per essere salvato.

### Checkpoint lasciato da SC2-A — storico, superato da SC2-B

SC2-A e' chiusa nel
[report completo](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc2.md).
Il prossimo agente deve trattare questo checkpoint come una piattaforma, non
come una promessa di comprensione semantica: parrot0 sa conservare **chi ha
riportato che cosa, con quale status, dove e sotto quale fonte**, ma il «che
cosa» e' ancora `proposition(surface("..."))`.

#### Risultato acquisito e misurato

- Il reader riconosce una coordinata meccanica `scheme://...`, la separa dalla
  prosa e costruisce `document_<hash-source>_<hash-content>`. `document_source`
  e `document_fingerprint` rendono lo stesso input riconciliabile fra processi.
  Il contatore `document_N` resta solo il fallback per testo senza fonte.
- Le frasi lunghe non attraversano piu' la lista ricorsiva SC1. Il C enumera i
  node ID gia' pubblicati; `document_unit_node_observe/3` decide in KB quali
  siano token. La baseline Salmonella conserva il token 16 (`death`) e la
  seconda unita'. Non reintrodurre una ricorsione bounded per comodita'.
- Le menzioni quotate multi-parola sono conservate letteralmente e attraversano
  learn/query/retract con la stessa analisi. Il determinante dentro le
  virgolette e' contenuto, non una stopword. Questo e' il motivo della guardia
  in `mod_forget`; rimuoverla ricrea `hypothesis_report_marker(forget)`.
- `claim_marker_class/4` fattorizza classe aperta, status, context kind,
  attribuzione ed extent. `claim_status_evidence/2` passa attraverso `apply/2`:
  membri nuovi insegnati a voce vengono usati subito senza rebuild.
- Il C usa `kb_hypothesis_best` e `kb_evidence_matches` per ottenere classe e
  span. Copia marker e remainder e chiama `document_claim_observe/4`; non
  contiene superfici come `shows`, `hypothesize`, `authors` o `data` e non
  decide `observed`, `simulated` o `hypothesized`.
- La KB materializza `document_claim`, `unit_claim`, osservazione della cue,
  attribuzione e source record. Status, context, `holds_in(context(...), ...)`
  e commitment sono **derivati dalla cue viva**. Il retract elimina queste
  viste anche da documenti vecchi e conserva superficie, fonte, span e
  attribuzione; il reteach le fa riapparire senza rilettura.
- Le claim sono `attributed_only` dentro `reported_belief`. Nessuna viene
  promossa automaticamente a `holds_in(world, ...)`.
- Il sommario del reader conta unita', claim e status vivi, ma la sua
  verbalizzazione e' in `response_template(reader_claim_summary, ...)`, non in
  una stringa naturale C.
- Le lezioni promosse sono quattro membri linguistici; il save e' esattamente
  `W=0, L=4, C=0, P=4, O=12, X=0, S=20`. `B0=36345/2433`,
  `B1=36365/2433`, dunque `B1-B0=S=20`. `kb/learning/learned.p0` e' vuoto
  rispetto a questo ciclo: le classi hanno una casa semantica.
- Processo fresco: quattro membership `Yes` e rilettura DART con lo stesso
  document ID, due claim e due status. `FreshProcessRecall=5/5`.
- Metriche: LessonYield `4/4`, Replay pass, `Transfer@3=3/3`, parafrasi `1/1`,
  contrasto `1/1`, ablation `1/1`, reteach `1/1`, status fidelity `8/8` e
  source/span fidelity `8/8`. La normalizzazione semantica resta **`0/8`** e
  le tre domande naturali restano **`0/3`**.

Il ratchet e' `tests/p0t/language/document_claims.p0t`: 50 assert con tre cue
held-out (`the investigators predict that`, `the measurements indicate that`,
`the model outputs show that`). Non sostituirle con le forme gia' persistite:
la baseline diventerebbe verde per memoria e non misurerebbe piu' crescita
runtime. Restano verdi `document_rhetoric.p0t` 33/33, `mention.p0t` 24/24,
`retract.p0t` 17/17 e `taught_segment_role.p0t` 21/21. `retract.p0t` usa `<^`
sulla testa della regola perche' l'induzione puo' aggiungere una coda lecita e
variabile quando la KB cresce.

Il solo `make soft-test` di SC2 e' gia' consumato: 55 verdi, un rosso
preesistente in `frontier_chat_audit.it.p0t` riga 97 (`designation`). Non
rilanciarlo come parte di SC2 e non cambiare quell'attesa dentro questo
checkpoint.

#### Mappa dei file per chi riprende

- `kb/core/document-structure.p0`: unita', token non ricorsivi, source e
  fingerprint. Fonte e fingerprint sono coordinate osservate; non attribuire
  loro autenticita', canonicalita' o autorevolezza.
- `kb/core/document-claims.p0`: unica casa di classi/status/attribution e viste
  epistemiche. Le quattro righe apprese sono mischiate ai seed dal router di
  persistenza: non duplicarle altrove e non spostarle in `learned.p0`.
- `src/brain/30-generation-reading.c`: meccanica URI/hash/span, invocazione
  dello scorer universale e pubblicazione. `document_claim_from_clause` crea al
  massimo una claim per unita' e assume `extent(remainder)`: sono limiti da
  generalizzare attraverso policy KB, non condizioni linguistiche da cablare.
- `src/brain/10-memory-knowledge.c`: `p0_quoted_words` e la simmetria della
  menzione. Il quoting non gestisce ancora virgolette annidate/escape complessi;
  non far dipendere SC2-B da quel caso senza un ratchet dedicato.
- `kb/core/responses.p0`: sommario claim in inglese/italiano.
- `tests/p0t/language/document_claims.p0t`: identita', fingerprint, status
  positivo/negativo, commitment, context, source span, ablation e reteach.
- `kb/machinery/fact-provenance.p0` e `kb/machinery/transcripts.p0`: diff
  naturale di `/save`. Non riordinarlo o «ripulirlo» manualmente.

#### Limiti precisi da non coprire con una risposta elegante

1. La proposition e' una stringa di superficie. Parrot0 non sa ancora che
   `kinetic impact can alter an asteroid orbit` e' una relazione modale fra
   entita' e evento, ne' che una parafrasi esprime lo stesso contenuto.
2. `What did the authors hypothesize?`, `Was X observed?` e `What did the
   simulations show?` non consumano ancora Document IR: finiscono a muro o in
   diagnostici nominali opachi.
3. Esiste una sola claim per unita'; il marker migliore/anteriore vince e il
   contenuto e' tutto il remainder. Coordinazione, negazione, citazioni
   annidate, apposizioni e piu' status nella stessa frase restano aperti.
4. L'attribuzione e' materializzata dalla classe osservata; lo status e' vivo.
   Se in futuro la stessa cue puo' avere attribuzioni diverse per sintassi, va
   resa anch'essa una vista causale, non duplicata in un secondo sistema.
5. L'estrattore URI conserva una coordinata per forma meccanica. Non fa
   canonicalizzazione, non legge DOI, data/versione, frammenti o redirect e non
   prova autenticita'. Un punto finale attaccato all'URI non e' oggi rimosso.
6. I documenti source-addressed non sono stati salvati. Prima di farlo serve
   una policy esplicita di versione, deduplicazione, volume e invalidazione.
7. FNV-1a e' un fingerprint deterministico, non una garanzia crittografica.
8. Le risposte del sommario dicono quanti status sono vivi, non dimostrano che
   il contenuto sia semanticamente compreso.

#### Prossimo esperimento minimo: SC2-B

Obiettivo: sostituire `proposition(surface(Text))` con una coppia evidenziale
che conservi la superficie e, quando giustificato, punti a un frame semantico
normalizzato prodotto dalla **pipeline di input comune**.

Ordine consigliato:

1. cercare con `rg` gli oggetti gia' esistenti (`holds_in`, `semantic_entity`,
   `semantic_class`, frame dichiarativi, provenance, belief/attribution) e
   scegliere un solo asse canonico. Non creare `document_semantic_*` se lo
   stesso frame e' gia' usato dalle asserzioni normali;
2. estrarre la trasformazione «clausola dichiarativa -> candidato semantico» in
   una funzione pura riusabile dal reader e dal normale learner. Il reader deve
   invocarla sul remainder con un origin/context che vieta il commit nel mondo;
3. conservare sempre `proposition(surface(...))`; aggiungere una relazione
   separata verso il frame normalizzato soltanto quando l'analisi e'
   unambigua. Il fallimento della normalizzazione non deve cancellare la claim;
4. costruire query naturali come intenti/frame KB-first. Insegnare una nuova
   forma interrogativa ancorandola a un modello che gia' funziona; non chiedere
   al teacher il nome di `claim_status` o `document_claim`;
5. prima chiudere tre domande: recupero per attribuzione («che cosa
   ipotizzano?»), verifica di status («X e' osservato?»), recupero per tipo di
   evidenza («che cosa mostra la simulazione?»). Ogni risposta deve avere proof
   a claim, unita', range, document source e cue viva;
6. ratchet baseline -> lezione -> replay -> tre domini -> parafrasi -> contrasto
   -> composizione con `rhetorical_edge` -> ablation -> reteach -> retention ->
   fresh process. Usare predicati e lessico held-out rispetto alle lezioni;
7. aggiungere casi negativi in cui una ipotesi non soddisfa una domanda
   osservativa, una simulazione non soddisfa `observed`, e la rimozione di una
   cue non cancella il frame proposizionale attribuito;
8. misurare separatamente `SurfaceClaimCoverage`, `NormalizedClaimCoverage`,
   `StatusPrecision`, `QuestionAnswerCoverage`, `ProofCompleteness` e
   `FalseUnderstandingRate`. Non chiudere SC2-B con il solo conteggio `Mapped`;
9. non salvare documenti durante le sonde. Se il run finale e' pulito, salvare
   soltanto le nuove competenze linguistiche/meta e poi fare diff semantico e
   fresh process secondo `LEARN_PROTOCOL.md`;
10. dopo la prima modifica engine eseguire una sola volta `make soft-test` per
    il nuovo ciclo, annotare il primo rosso e continuare con test mirati.

#### Strategia per manipolare la crescita futura della KB

- Far crescere **classi aperte** prima delle superfici: un nuovo status, extent,
  tipo di fonte o atto documentale deve avere una relazione di policy; le
  parole diventano membri insegnabili di quella relazione.
- Mantenere un lattice di evidenza monotono: byte/range -> unita' -> claim di
  superficie -> frame candidato -> status/contesto -> eventuale commitment.
  Retrarre un livello forte non deve distruggere i livelli piu' deboli.
- Rendere derivate tutte le viste che dipendono da conoscenza correggibile.
  Materializzare soltanto osservazioni irreversibili della sessione (che testo,
  quale span, quale cue fu vista), non la loro interpretazione corrente.
- Usare `apply/2` e lo scorer universale come punti di estensione. Se una nuova
  forma richiede `strcmp`, `strstr` o una cue letterale in `src/brain`, fermarsi:
  manca una relazione KB o un consumer meccanico abbastanza generale.
- Separare sempre persistenza di competenza e persistenza del corpus. Lezioni e
  policy possono essere promosse; documenti, claim e trace richiedono una
  decisione esplicita su versione, licenza, volume, invalidazione e provenienza.
- Tenere test held-out rispetto alla KB corrente. Dopo ogni `/save`, cercare le
  cue dei ratchet: se una baseline le conosce gia', sostituirle con membri nuovi
  e verificare learn/retract nello stesso processo.
- Non aumentare la metrica per inferenza nominale. Una stringa sotto
  `proposition(surface(...))` vale come copertura evidenziale, non come
  comprensione semantica; un frame senza proof vale come candidato, non come
  conoscenza.
- Ogni nuovo status deve avere almeno un near-miss: `suggests` contro
  `demonstrates`, `estimated` contro `measured`, `simulated` contro `observed`.
  La precisione negativa viene prima della copertura aggressiva.
- Quando si introducono scope e coreference, farli puntare agli stessi node/range
  di Document IR. Non ritokenizzare il testo in un modulo parallelo: due mappe
  di span divergono e rendono impossibile la genealogia.
- Conservare nei report anche gli `X` delle sessioni abortite. Il run promosso
  puo' avere `X=0`, ma cancellare la diagnosi impedisce agli agenti successivi
  di capire quali simmetrie e guardie siano causali.


### Checkpoint lasciato da SC2-B — punto di ripresa autoritativo

SC2-B e' chiusa nel
[report completo](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc2b.md)
e nell'evidenza §18.19 di
[`frontier-kb-natural-dialogue.md`](docs/plans/frontier-kb-natural-dialogue.md).
Chi riprende deve trattare questo checkpoint come una **piattaforma sul motore**,
non come una vittoria sul contenuto: parrot0 sa normalizzare il contenuto di una
claim riportata senza crederci, e sa dire quando non ci riesce; non sa ancora
riconoscere che due frasi diverse dicono la stessa cosa.

#### Risultato acquisito e misurato

- La **fase pura** esiste ed e' una sola. `P0FrameReading` / `p0_frame_bind` /
  `p0_frame_reading` in `src/brain/10-memory-knowledge.c` legano uno schema
  dichiarato al flusso di token e restituiscono predicato, slot, slot
  interrogativo e **copertura**, senza assert, senza risposta, senza traccia. Il
  consumer storico `p0_try_extract_frames_only` usa la stessa funzione: non
  esistono due legatori di schemi. Non reintrodurne uno «per comodita' del
  lettore».
- Il lettore invoca la fase pura sul remainder e consegna alla KB un candidato
  con **origine** e **copertura**. Il C non nomina verbi, status o fonti.
- `normalization_origin/2` decide il commitment: `reported -> quarantine`,
  `asserted -> world`. Il frame di una claim riportata vive in
  `holds_in(context($Claim), ...)` e **mai** in `holds_in(world, ...)`.
  Misurato: `/debug shortened` -> nessuna clausola dopo quattro letture reali.
- `normalization_extent_policy/4` rifiuta una lettura che non copre la frase.
  Coordinazione e complemento pendente diventano `gap(partial_reading)` con la
  superficie intera. **Questa non e' una limitazione da rimuovere**: e' la
  ragione per cui i frame accettati possono essere confrontati.
- La forma interrogativa e' **derivata**, non insegnata:
  `claim_question_evidence/2` toglie il complementatore dalla locuzione appresa,
  `claim_status_question_evidence/2` costruisce `«observed that»` dallo status
  dichiarato in `claim_marker_class/4`. Insegnare un marker apre la sua domanda.
- `mod_claim_question` (registrato come `claimq`, dopo gli atti didattici e
  prima di `qa`) risponde a due forme: recupero per classe e verifica di status.
  Il cancello richiede claim osservate **e** una evidenza da cue viva.
- Ogni parola pronunciata viene da `response_template`:
  `claim_content_answer`, `claim_content_answer_unsourced`,
  `claim_status_confirmed`, `claim_status_other`,
  `reader_claim_summary_normalized`.
- Save: `W=0, L=5, C=0, P=5, O=15, X=0, S=25`; `B0=36406/2454`,
  `B1=36431/2454`, `B1-B0=S`. Le cinque lezioni sono verbi di relazione veri
  (`shortens`, `shortened`, `improved`, `detected`, `eradicated`) instradati in
  `kb/learning/taught-lexicon.p0`.
- Metriche: `Transfer@3=3/3` (NASA, GMD, WHO, PRL), `ContrastPrecision=3/3`,
  `Paraphrase=1/1`, `Composition=1/1`, `AblationFidelity=1/1`, reteach `1/1`,
  `Retention=pass`, `FreshProcessRecall=7/7`, `WorldCommitLeak=0`,
  `FalseUnderstandingRate=0`.

Il ratchet e' `tests/p0t/language/document_claims.p0t`: **124 assert** (50 di
SC2-A + 74 di SC2-B), con locuzioni e verbi **held-out** rispetto alla KB
persistita (`the investigators predict that`, `the measurements indicate that`,
`the model outputs show that`; `slowed`, `deflected`, `raised`, `weakened`,
`increased`). Non sostituirli con i membri salvati: la baseline diventerebbe
verde per memoria e smetterebbe di misurare crescita a runtime.

Rossi noti e **non** introdotti da SC2-B, verificati identici su `HEAD`
precedente: `assisted_construction.p0t` 60/6 (inversione dei ruoli di una
costruzione insegnata) e il rosso di `frontier_chat_audit.it.p0t` riga 97
(`designation`) nell'unico `make soft-test` del ciclo (55 verdi, 1 rosso).

#### Mappa dei file per chi riprende

- `src/brain/10-memory-knowledge.c`: `P0FrameReading` e la fase pura. Il campo
  `consumed`/`total` e' il cuore di D14: chi lo ignora reintroduce la perdita
  silenziosa.
- `src/brain/30-generation-reading.c`: invocazione della fase pura sul
  remainder, `mod_claim_question`, sommario del lettore.
- `src/brain/99-registry.c`: posizione di `claimq`. Spostarlo prima degli atti
  didattici fa si' che una lezione o un retract che *contengono* la locuzione
  vengano interpretati come domande.
- `kb/core/document-claims.p0`: origine, policy di copertura, gap tipati, viste
  (`claim_frame`, `claim_proof`, `claim_surface`, `claim_document_source`) ed
  evidenza interrogativa derivata.
- `kb/core/responses.p0`: le quattro frasi di risposta e il sommario normalizzato.
- `tests/p0t/language/document_claims.p0t`: il ratchet.

#### Limiti precisi da non coprire con una risposta elegante

1. Il confronto fra frame e' **strutturale**. `DART shortened the period` e
   `the period was shortened by DART` restano due frame diversi.
2. Solo `binary(Relation)` a due ruoli. Misura, classe, relazione ternaria,
   negazione e modalita' (`can`, `may`) producono un gap, non un frame.
3. La domanda di contenuto e' limitata al **documento corrente**: con piu'
   documenti letti non disambigua la fonte e sceglie l'ultimo.
4. La verifica di status pretende il complementatore esplicito
   (`observed that`); `«did anyone observe X?»` non e' ancora una parafrasi.
5. `claim_proof/2` si interroga ma non si **pronuncia**: «come lo sai?» non ha
   ancora una porta naturale.
6. Restano aperti i limiti SC2-A: una claim per unita', `extent(remainder)`,
   nessun salvataggio dei documenti, nessuna canonicalizzazione degli URI.

---

## Seconda coda 2026-08-29 — dalla supercomprensione alla metacomprensione

Le voci SC0-SC16 restano valide e descrivono che cosa un documento *contiene*.
Questa seconda coda nasce da tre osservazioni misurate in SC2-B che non
riguardano i documenti ma **il motore che li legge**, ed e' l'operazionalizzazione
delle ipotesi **D13-D20** di
[`frontier-kb-natural-dialogue.md` §18.20](docs/plans/frontier-kb-natural-dialogue.md).

Le tre osservazioni:

1. l'accoppiamento fra *leggere* e *credere* non era un difetto del lettore
   documentale: era ovunque, e SC2-B lo ha sciolto in un solo punto;
2. il residuo non letto e' l'informazione piu' preziosa che il sistema buttava
   via, perche' una lettura parziale e' indistinguibile da una completa;
3. la porta interrogativa di una conoscenza e' **derivabile** dalla conoscenza
   stessa, e dove non lo e' quel «dove» e' misurabile.

Ordine obbligato dalle dipendenze, non dall'effetto: **SC2-C, SC17, SC18** sono
precondizioni di tutto il resto.

| # | Piano complesso | Ipotesi | Classe liberata | Interazione didattica minima | Gate duro | Stato |
|---|---|---|---|---|---|---|
| **SC2-C** | **Equivalenza fra frame, modalita' e prova pronunciata** | D2, D19 | due superfici diverse che dicono la stessa cosa convergono o dichiarano perche' no; `can`/`may` diventano operatori di status invece di far fallire la lettura; «come lo sai?» risponde in lingua naturale | «"X was shortened by Y" dice la stessa cosa di "Y shortened X"»; «"può" indica una possibilità, non un risultato» | parafrasi attiva e passiva sullo stesso frame; una claim modale non soddisfa una domanda osservativa; la prova nomina documento, unita', span e cue | **CHIUSA per 2 gate su 3** — 2026-08-29: attivo/passivo convergono su un frame (passivo derivato dal participio, morfologia in KB, `frame_role_order/2` chiude anche il rosso storico dell'inversione dei ruoli); la prova si pronuncia nel registro di «come lo sai?»; il **modale resta tipizzato e non compreso** — `modal_operator` e' un residuo, non uno status. [Report SC2-C](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc2c.md) |
| **SC2-D** | **La modalita' e' un operatore dello status, non un residuo** | D2, D13 | `can`, `may`, `might`, `must` modulano la forza di una claim invece di farne fallire la lettura | «"può accorciare" dice che e' possibile, non che e' successo» | una claim modale non soddisfa una domanda osservativa; ritrarre il modale cambia lo status e non la proposizione; transfer su tre modali mai usati per la lezione | **CHIUSA** — 2026-08-29: l'operatore si stacca prima della fase pura e resta detto; `claim_asserted_status/2` esclude le claim modali; classe aperta a voce («ought is a necessity marker» vale dal turno dopo); transfer 3/3 su may/must/ought; ablation 1/1. Il misclaim `shorten(kinetic_impactor_can, …)` non e' piu' producibile. [Report SC2-D](docs/labs/apprendimento-assistito/2026-08-29-supercomprensione-sc2d.md) |
| **SC25** | **Dove una conoscenza viene calcolata e' conoscenza** | D21 | una relazione dichiara se le sue conclusioni sono ricalcolate a ogni turno o materializzate all'atto dell'insegnamento, senza perdere la ritrattabilita' | nessuna: e' uno strato di motore, ma deve **restare** insegnabile cio' che oggi lo e' | spostare una relazione a `materialized_on_learn` riporta il turno nel budget, non cambia nessuna risposta e conserva l'ablation; se una delle tre non vale, e' un'ottimizzazione e va rifiutata | **aperto — prossimo.** Caso concreto gia' misurato: la radice verbale per `-ed`/`-d` porta un turno da <1s a 1,85s, quindi oggi e' una lezione invece che una derivazione (`kb/core/grammar.p0`, `verb_suffix/1`) |
| **SC26** | **Una collisione di dispatch e' un fatto osservabile** | D22, mantra #14 | non *chi* ha preso il turno — quello parrot0 lo dice gia' — ma **perche'**: quale superficie gli ha dato titolo, e chi di piu' adatto e' stato scavalcato | «quel turno l'ha preso un altro modulo per via di questa parola» | le collisioni su corpus reale si concentrano su poche coppie di moduli e poche parole comuni | **aperto, campione corretto.** `who answered?` funziona da sempre: dei cinque casi raccolti uno solo era un furto (`claim` -> `missing_referent`). Vedi §18.27 |
| **SC29** | **Uso e menzione sono un invariante, non una guardia locale** | D25 | cio' che e' menzionato non partecipa alla lingua che lo menziona, in **ogni** percorso: dispatch, parser, canonicalizzazione | «"shall" e' un marcatore di necessita'» deve valere quanto «shall e' un marcatore di necessita'» quando la parola non ha ruolo | dieci locuzioni quotate con copule, ausiliari, articoli e dimostrativi entrano byte per byte e si ritrattano uguali; ogni lezione che fallisce nella forma d'uso riesce in quella di menzione; una perdita e' un errore **dichiarato**, non muta | **aperto — prossimo.** Tre gap della serie SC2/SC3 erano questo, e il rimedio parrot0 lo proponeva gia' da se' |
| **SC34** | **`mod_lone` rivendica casi speciali, non ogni turno di una parola** | D26, D28 | la rivendicazione GENERICA su un token ignoto e' sospesa e dichiarata (`move_policy(lone_bare_token, claim)`); i casi fondati restano | nessuna: e' uno strato di motore, ma non deve togliere niente a cio' che si insegna | un token insegnato ottiene titolo; un token noto alla KB resta di `mod_lone` (gen491, «milano»); un token ignoto raggiunge un muro che PROPONE il rimedio; asserire il fatto riporta il comportamento storico nello stesso turno | **CHIUSA — 2026-08-30** (F.: «dovrebbe rivendicare alcuni casi speciali, non tutti i turni con una sola parola»). Niente e' stato cancellato: la scommessa e' dichiarata, come il gen491 aveva gia' fatto per quella fonotattica. Il rientro pieno passa da SC30/SC31 |
| **SC40** | **Rileggere alla luce di cio' che si e' appena imparato** | D33, D34, D35 | una lettura dichiara da che cosa dipende; quando quella conoscenza cambia la lettura diventa `stale` e si ri-deriva, invece di restare congelata accanto alla nuova | «adesso che sai cosa vuol dire, rileggi quel passaggio» | insegnato un verbo dopo la lettura, una domanda sul contenuto risponde **senza rileggere a mano**; la lettura vecchia e' `stale`, non coesistente; ritrattando il verbo si torna al gap; su 100 claim il receipt visita soltanto il candidato dipendente | **SC40-A + SC40-B CHIUSE.** 2026-08-30: versioni, dipendenza viva, genealogia e revisione selettiva sul delta reale della KB; gap -> frame -> gap -> frame sullo stesso span senza replay; `Transfer@3=3/3`, `StaleLeak=0`; add/retract/reteach `visited(1), changed(1)`; stress 100 claim con fan-out 1. SC41-A ha poi esteso l'identita' al supporto passivo. [SC40-A](docs/labs/apprendimento-assistito/2026-08-30-supercomprensione-sc40.md), [SC40-B](docs/labs/apprendimento-assistito/2026-08-30-supercomprensione-sc40b.md), §§18.38–18.43 |
| **SC41** | **La lettura ricorda perche' e' stata possibile** | D42–D45 | separare dipendenze di licenza, selezione e opportunita'; versionare anche quando il frame resta uguale ma cambia la prova | «bound e' un participio irregolare», dopo aver gia' letto tre passivi scientifici | una lezione apre 3 claim senza replay; retract/reteach simmetrici; schema, morfologia, ausiliare, marker, ruoli e copertura sono genealogia interrogabile | **SC41-A `passive_core` CHIUSA 7/7; SC41-B aperta.** `visited(3), changed(3)` su tre fonti NCBI, ablation/reteach 3/3, same-frame/different-proof versionato, `StaleLeak=0`, `WorldCommitLeak=0`. Restano marker epistemico, modalita', ellissi, coreferenza, precedenza e DAG downstream. [Report](docs/labs/apprendimento-assistito/2026-08-31-supercomprensione-sc41a.md), §§18.44–18.47 |
| **SC38** | **Riduzione della coda: di quale classe e' membro ogni fronte?** | D31 | molte voci della coda sono la stessa voce sotto politiche diverse; aprirle costa righe di politica e una vista, non un produttore in C | nessuna | i due fronti successivi (SC4 disegno sperimentale, SC6 causalita') si aprono senza un nuovo estrattore; se anche uno lo richiede, si scrive **quale asse della politica** non basta piu' | **aperto — prossimo.** SC5 sembrava una facolta' ed e' costata tre righe di `claim_marker_class/4` piu' una vista |
| **SC39** | **L'ellissi si recupera dalla struttura, non si inventa** | D32 | oggetto sottinteso, soggetto di relativa, unita' ereditata, agente di un passivo senza `by`: ognuno e' una riga di `elided_role/3` **se** la coordinata da cui recuperare esiste gia' | «in questo passo l'oggetto e' quello del passo prima» | ritrattare la coordinata fa sparire il recupero, non lo lascia materializzato; `unlicensed_recovery/2` resta vuoto su corpus reale; dove la coordinata non esiste, il residuo tipato resta la risposta | **aperto.** Primo caso chiuso in SC5-A: il soggetto eliso di un passo viene dall'attribuzione della classe |
| **SC35** | **Nessuna risposta piu' forte della copertura che la sostiene** | D29 | operatore, ruolo, premessa, unita' e qualificatore hanno una copertura come i token; `overclaim/2` e' interrogabile **prima** che la frase esca | nessuna: e' uno strato di motore, ma il teacher deve poter chiedere «che cosa hai lasciato fuori?» | tre difetti iniettati in un corpus reale — operatore inghiottito, premessa fuori blocco, complemento pendente — producono tre `coverage_shortfall` distinti e nessuna risposta che li ignori | **aperto — prossimo.** Tre occorrenze gia' misurate in due giorni (SC2-B, SC2-D, SC27), nessuna delle quali somigliava a un errore |
| **SC36** | **Censimento dei default impliciti del motore** | D30 | ogni punto in cui il motore sceglie senza consultare la KB diventa una politica dichiarata, a comportamento invariato | nessuna | la lista e' finita e corta; ogni voce diventa una riga di politica senza cambiare il comportamento di partenza; se e' lunga quanto il codice, D30 e' falsa | **aperto.** Cinque chiusi il 2026-08-30 con la stessa ricetta: `cue_scope`, `move_policy`, `support_mode`, `specific_participle`, `normalization_extent_policy` |
| **SC30** | **Il registro terminologico decide chi ha titolo** | D26 — asse **contesto** | un turno ha un registro terminologico, e una facolta' dichiara quali registri serve; il vocabolario arriva dai domini gia' caricati | nessuna lezione nuova: `concept_label/4`, `concept_in_domain/2` e `denotation_register/4` lo portano gia' | «a causal claim must survive confounding» smette di essere presa da un modulo di verifica **senza toccarne il codice e senza toccare l'ordine del registry**; se serve spostare il modulo, il registro non sta decidendo | **aperto.** La KB distingue gia' `fsi` da `common` sugli scacchi: la struttura c'e', governa l'uscita e non l'ingresso |
| **SC32** | **Una cue insegnata deve poter essere ritratta parlando** | D28, D25 | il contratto di conformita' di `AGENTS.md` — «retracting the cue must remove that recognition» — vale anche per le cue `substring` insegnate a voce | «forget "helyla friend" as a casual opener» deve togliere il riconoscimento | ogni frase che nomina la locuzione non la attiva; nessun tentativo di retract produce un fatto sul verbo di retract; se serve una guardia in `mod_forget` o in `mod_lone`, D28 e' falsa | **CHIUSA — 2026-08-30, e D28 CONFERMATA.** Due interventi, nessuno tocca un modulo: `cue_scope/2` + `mention_delimiter/2` (una cue non guarda dentro una menzione, additivo) e `try_forget_form` (lo stesso registro `learnable/3` letto nell'altro verso, prima di `try_teach_form` perche' «unlearn» contiene «learn»). Transfer su una seconda classe; il retract vale anche per la cue che non ottiene titolo. [Report](docs/labs/apprendimento-assistito/2026-08-30-sc32-retract-simmetrico.md) |
| **SC33** | **La scala della cue insegnabile: chiudere i gradini 8 e 9** | D28 | superficie -> passo/precondizione/criterio d'arresto (SC5), e superficie -> contratto di facolta' (`faculty_for/2` esiste, i membri non si insegnano) | «"prima di iniziare" introduce una precondizione»; «il verificatore C serve le richieste di compilazione» | il membro nuovo cambia il turno successivo, si ritratta a voce, e non richiede di toccare il registry | **aperto.** Sette gradini su nove sono gia' aperti: la ragione per cui funzionano e' che si insegna un MEMBRO di una classe con politica in KB, mai un comportamento a un modulo |
| **SC31** | **La puntualita' e' la cue dentro il ruolo** | D27 — asse **precisione** | una cue vale nel ruolo a cui appartiene, non in qualunque punto del turno; e' il mantra #8 un piano piu' su | nessuna: e' uno strato di motore | dichiarando `cue_scope(R, outside_role(mention))` nessuna cue si accende dentro una citazione, e le lezioni quotate che falliscono vanno a zero **senza riscrivere nessuna cue** | **aperto.** Ortogonale a SC30 e da comporre con essa: SC30 dice quali facolta' hanno titolo, SC31 dove possono guardare |
| **SC17** | **L'origine come parametro di ogni inferenza** | D13 | discorso citato, ipotesi controfattuale, simulazione di piano e lettura non fidata condividono la quarantena della claim riportata | «supponiamo per un momento che...», «lui sostiene che...», e poi chiedere che cosa parrot0 crede davvero | la stessa fase pura serve almeno tre origini senza un nuovo parser; ritrarre un'origine spegne **esattamente** cio' che ne dipendeva | aperto |
| **SC18** | **Il residuo non letto diventa un oggetto tipato** | D14 | ogni frase non compresa lascia span, superficie e classe del residuo, invece di un silenzio | «in questa frase la parte che non hai letto e' la coordinazione»; poi chiedere a parrot0 di elencare i propri residui | su un corpus reale i residui si raggruppano in poche classi; insegnare una classe abbassa la frequenza dell'**intera** classe | aperto |
| **SC19** | **Livelli di comprensione nominabili e monotoni** | D19 | `observed < surface < normalized < grounded < modelled`; ogni consumer dichiara il minimo che gli serve e declina nominando cio' che manca | «non chiederti se sai la frase: chiediti a che livello l'hai capita» | un consumer nuovo rifiuta input insufficienti dichiarando **solo** il proprio livello minimo; ritrarre conoscenza abbassa il livello e spegne i consumer giusti | aperto |
| **SC20** | **Chiusura interrogativa per derivazione** | D15 | la domanda si costruisce dall'asserzione con trasformazioni di lingua (caduta del complementatore, spostamento del gap, inversione dell'ausiliare) | «se sai dire "A precede B", devi saper rispondere a "che cosa precede B?"» | `InterrogativeClosure` sale aggiungendo **trasformazioni**, non consumer; una relazione insegnata oggi e' interrogabile domani senza nuovo C | aperto |
| **SC21** | **La regione, e il guadagno di una domanda** | D16, D6 | un testo denota la regione dei modelli compatibili; la precisione e' l'ampiezza della regione; la contraddizione e' la regione vuota | mostrare un paragrafo ambiguo e chiedere quale chiarimento restringerebbe di piu' | fra due chiarimenti sceglie quello a `question_gain` maggiore e sa dire perche'; ricerca incompleta -> `incomplete`, mai «nessun modello» | aperto |
| **SC22** | **Il mittente del testo e' modellabile** | D17 | obiettivo dell'autore, concessioni, funzione delle attenuazioni, obiezioni anticipate | «qui l'autore concede un punto per difendere la tesi principale» | risponde a «che cosa vuole farmi credere, e che cosa concede?» senza riassumere; ritrarre **una** locuzione concessiva cambia la risposta | aperto |
| **SC23** | **Una procedura e' un riferimento risolvibile** | D18, D5 | un metodo che delega a un'altra fonte si dichiara bloccato e nomina il passo, invece di fingersi completo | «segui il protocollo di quell'altro lavoro, con questa sola modifica» | dice **quale** passo delega e a quale fonte; fornendo la fonte diventa eseguibile senza reinsegnare i passi noti | aperto |
| **SC24** | **Il gap ricorrente diventa una richiesta di lezione** | D20, D9, D10 | parrot0 formula in lingua naturale la lezione che gli manca, con un esempio preso dal proprio residuo, ordinata per guadagno di copertura | leggere un corpus, poi chiedere «che cosa dovrei insegnarti per primo?» | la prima richiesta e' quella che, insegnata, produce il guadagno di copertura maggiore fra le candidate; il guadagno viene **misurato dopo**; `lesson_request` non auto-promuove niente | aperto |

### Pacchetti della seconda coda

1. **Pacchetto G — onesta' strutturale (SC2-C, SC17, SC18).** Sciogliere
   l'accoppiamento lettura/credenza ovunque e reificare il residuo. Uscita: un
   motore che sa dire dove finisce cio' che ha capito.
2. **Pacchetto H — tipi della comprensione (SC19, SC20).** Livelli nominabili e
   porta interrogativa derivata. Uscita: nessuna risposta costruita a un livello
   piu' alto di quello raggiunto.
3. **Pacchetto I — spazio logico (SC21).** Regione, raffinamento, guadagno di
   una domanda, regione vuota. Uscita: una definizione operativa di «buona
   domanda» che non dipende dal giudizio di chi guarda.
4. **Pacchetto J — testo come atto di un agente (SC22, SC23).** Mittente e
   procedure per riferimento. Uscita: lettura di letteratura scientifica che
   distingue cio' che l'articolo *fa* da cio' che *dice*.
5. **Pacchetto K — curriculum autogenerato (SC24).** Uscita: la coda di questo
   stesso file smette di essere scritta soltanto da noi.

### Metriche della seconda coda

Si aggiungono a quelle gia' obbligatorie:

- **WorldCommitLeak:** fatti entrati nel mondo da un'origine che non lo
  autorizzava. Deve restare `0`; qualunque valore diverso invalida il ciclo.
- **Coverage:** token consumati da una lettura / token della frase, per classe
  di residuo.
- **ResidueClassConcentration:** frazione dei residui coperta dalle prime cinque
  classi. Se resta bassa, la lettura non e' compositiva (falsifica D14).
- **InterrogativeClosure:** relazioni insegnabili interrogabili per derivazione
  / relazioni insegnabili totali.
- **LevelDiscipline:** risposte costruite a un livello non superiore a quello
  raggiunto / risposte costruite. Deve essere `1`.
- **QuestionGainAccuracy:** chiarimenti scelti che coincidono con quelli di un
  lettore esperto / chiarimenti proposti.
- **LessonRequestOrdering:** correlazione fra l'ordine delle richieste di
  lezione e il guadagno di copertura misurato dopo l'insegnamento.

### Regole di consumo di questa coda

1. Una voce per sessione, sonda breve, `LEARN_PROTOCOL.md` per intero.
2. Nessun documento viene salvato finche' la policy di versione, licenza,
   volume e invalidazione non e' esplicita. Si salvano competenze, non corpora.
3. Ogni ratchet nuovo usa membri **held-out** rispetto alla KB persistita, e
   dopo ogni `/save` si verifica che lo siano ancora.
4. Un rosso preesistente si annota e non si tocca dentro il ciclo che lo trova.
5. Se una voce richiede C, il rimedio deve trasferire ad almeno un secondo
   dominio reale non usato per progettarlo (`LEARN_PROTOCOL.md` §7.2.7).
6. Un `X > 0` ferma il salvataggio. Le sessioni abortite si riportano lo stesso:
   cancellare la diagnosi impedisce al prossimo agente di capire quali guardie
   siano causali.

---

## HANDOFF STORICO 2026-08-30 — stato precedente a SC40

Chi riprende legge **questa sezione per prima**, poi
`docs/plans/frontier-kb-natural-dialogue.md` §0 dei TODO aperti.

### Da dove ripartire, in ordine

1. **SC40 — le letture sono congelate.** È la voce prioritaria e non è una
   voce come le altre: è l'unica in coda che, chiusa, renderebbe parrot0 *più
   intelligente* invece che più capace su una classe in più. Misura riproducibile
   in §18.37; il difetto in una riga: **rileggere accumula invece di rivedere**,
   e la stessa claim resta insieme `gap(no_reading)` e `normalized(reported)`.
2. **SC35/SC36** — le due famiglie con la stessa radice (D29 l'affermazione che
   copre meno di quanto legge; D30 il default implicito).
3. **SC38** — la riduzione della coda: se D31 regge, molte voci sono la stessa
   voce sotto politiche diverse. Provarlo su SC4 e SC6 **prima** di aprirle.

### Che cosa è stato chiuso in questa sessione

| voce | esito |
|---|---|
| SC2-B | normalizzazione come fase **pura e contestuale**; `WorldCommitLeak=0` |
| SC2-C | attivo/passivo su un frame, residuo tipato, prova pronunciata |
| SC2-D | la modalità è un operatore, non una parola inghiottita |
| SC3-A | grafo argomentativo; ritrarre una premessa ritira solo i dipendenti |
| SC27 | due premesse non sono due argomenti (una **spariva in silenzio**) |
| SC32 | una cue insegnata si ritratta parlando — era una **non conformità** |
| SC30/SC34 | un token insegnato ottiene titolo; `mod_lone` cede per policy |
| SC5-A | il metodo in prosa è una **vista sulle claim**, non un secondo estrattore |

Ipotesi nuove nel piano: **D21–D33**. Ratchet nuovi:
`document_argument.p0t` (75), `document_method.p0t` (26),
`use_mention_lesson.p0t` (19), `taught_cue_ladder.p0t` (23);
`document_claims.p0t` è passato da 50 a 182 assert.

### Regole di metodo nate qui, e sono vincolanti

1. **Un rimedio proposto e non provato è un gap che non esiste.** Il muro di
   parrot0 spesso *nomina la specie del proprio arresto e propone la
   riparazione*; tre «muri» di questa serie erano riparabili al primo colpo con
   la forma che lui stesso suggeriva. Prima di classificare un gap, **eseguire
   il rimedio proposto**. (§18.27 — è il primo frutto dell'autocorrezione.)
2. **Chiedere `who answered?` invece di dedurre il colpevole.** `turn_module/2`
   c'è da sempre; dedurre il furto dall'esito ha prodotto un'ipotesi falsa.
3. **Quando un test ha bisogno di un dato fresco per passare**, chiedersi prima
   se il sistema debba *dimenticare* o **rivedere**. Aggirare un limite e
   descriverlo sono compatibili; aggirarlo e chiamarlo intenzionale no.
4. **Quando una correzione chiede di *spostare* qualcosa** (un modulo nel
   registry, una clausola prima di un'altra), quasi sempre esiste la stessa
   correzione scritta come **politica dichiarata** — e non rimanda il problema
   alla parola successiva.

### Rossi noti, tutti verificati preesistenti

`repair.p0t` (l'oracolo non compila in questo ambiente), `check_sort`,
`forget_move`, `greet` 7/1, `games`, `faceted_enumeration`,
`foundational_concepts`, `gap_dialogue`, `name_is_knowledge`,
`reactions_are_knowledge`, `gap_is_a_fact`, `gap_anchor`,
`frontier_chat_audit.it` riga 97, `assisted_construction` 65/1 (attesa stantia
sulle virgolette di `teach_form_ack` — il resto del file è stato **riportato in
verde** chiudendo il bug dell'ordine dei ruoli).

### Debiti aperti che non vanno persi

- `blocked(no_stop_criterion)` in `document-method.p0` è un **seme dichiarato e
  non provato**: serve una cue di ripetizione indipendente dall'arresto (SC37).
- Le modalità `fill` e `define` di `learnable/3` non hanno retract simmetrico.
- `mod_lone` cede per policy; il rientro pieno passa da SC30/SC31 (il **ruolo**
  del turno decide chi ha titolo, non l'ordine di arrivo).
- Il confronto fra frame è **strutturale**: due parafrasi restano due frame.

---

> ## ✅ TRAGUARDO — LO SPAZIO DEL DISCORSO (2026-08-31)
>
> **Da conservare: è la prima volta che parrot0 ricorda *che cosa* è stato
> nominato e *in che ordine*, e che un'espressione può riferirsi a quella
> memoria invece che a una parola.**
>
> ```text
> > Il libro rosso è sul tavolo.      ->  located_in(book_red, tavolo)
> > Il quaderno blu è sulla mensola.  ->  located_in(quaderno_blue, mensola)
> > Dov'è il primo?                   ->  tavolo        (prima: muro)
> > Dov'è il secondo?                 ->  mensola       (prima: muro)
> ```
>
> ### Che cos'è, esattamente
>
> Poco, di proposito: `discourse_referent(Ordine, Chiave)`. Una cosa nominata e
> la sua posizione nel discorso. Testa e proprietà si ricavano dalla chiave (G2);
> determinante, span e superficie originale non ci sono ancora (G5).
>
> Ma è la prima **memoria del dialogo che non è una lista di frasi**: è una lista
> di *cose*. Fino a ieri parrot0 conservava turni; ora conserva referenti.
>
> ### A che cosa serve, e che cosa abilita
>
> Non è una funzione in più: è il posto a cui si attaccano cose che prima non
> avevano appiglio.
>
> | abilita | perché prima non si poteva |
> |---|---|
> | **coreferenza** — «quello», «l'altro», «quello di prima» | non esisteva l'oggetto a cui riferirsi: F03 era la famiglia peggiore del corpus, 24 muri su 30 |
> | **ellissi** — «E il secondo?» senza ripetere il verbo | il turno ellittico non ha entità da nominare: deve prenderla dal discorso |
> | **correzione** — «no, quello rosso l'ho spostato» | correggere richiede di individuare *che cosa* si corregge, non solo che si corregge |
> | **ambiguità dicibile** — «Quale? …» invece di un muro | serve saper elencare i candidati, cioè averli |
> | **il soggetto eliso** (SC5) e **l'apposizione** | entrambi recuperano un ruolo da qualcosa già introdotto |
> | **la domanda di seguito** — «e dove si trova adesso?» | «adesso» presuppone una cosa di cui si stava parlando |
>
> Ed è la precondizione dichiarata di **GD4** (riferimento cross-turn) e di
> **D37/G4-G5**: il referente con proprietà, e il referente che sa ridirsi.
>
> ### Come è stato costruito — le tre cose che hanno deciso l'esito
>
> Vale più del risultato, perché sono riusabili:
>
> 1. **Il punto di strozzatura condiviso.** Le vie che imparano un fatto sono
>    più d'una — lo schema dichiarato, la copula binaria, il locativo — e la
>    prima versione agganciava i referenti a *una*. Misurato: il locativo
>    italiano non registrava niente, e metà del dialogo restava senza memoria.
>    L'osservazione sta ora in `p0_learn_source`, che **tutte** attraversano
>    perché registrare la provenienza è ciò che ogni via fa comunque. *Un
>    referente è esattamente questo: una cosa nominata, e quando.* Cercare il
>    punto che tutti attraversano invece di enumerare i chiamanti è la stessa
>    mossa della fase pura di SC2-B.
> 2. **Quale posizione introduca un referente è una politica, non una scelta del
>    C.** Registrando *ogni* argomento, «il secondo» diventava il **tavolo**
>    invece del quaderno: in «Il libro rosso è sul tavolo» sono nominati due
>    oggetti, ma quello di cui si parla è il primo. `referent_arg_position/1` lo
>    dichiara, e una relazione con un'altra geometria costa una riga.
> 3. **La superficie da dichiarare è quella che sopravvive al percorso.** «primo»
>    arriva al matcher come «prime» — la canonicalizzazione lo traduce — e la
>    forma col determinante non combacia più. È la **terza volta** che questa
>    lezione si presenta (dopo le cue di SC2-B e le locuzioni di SC2-D): finché
>    la canonicalizzazione non conserva anche l'originale (G5), una classe di
>    superfici deve tenere *entrambe* le forme.
>
> ### Il limite onesto
>
> È la **prima forma**, non la forma finale. Non c'è ancora il determinante (che
> distingue introdurre da riprendere), non c'è lo span, non c'è la superficie
> originale — quindi parrot0 sa che il libro rosso è stato nominato per primo e
> non sa ancora ridirlo «il libro rosso». E due referenti con la stessa testa si
> distinguono per proprietà (G2) ma non hanno ancora identità propria.


> ## ⛔⛔ IL CASSETTO SENZA MANIGLIA — il problema che teneva ferma la comprensione universale
>
> **Scoperto e misurato il 2026-08-31. Scritto in tutti i piani perché è la
> giunzione da cui dipendono `universal-input`, `universal-comprehension` e il
> fronte SC/GD insieme.**
>
> ### Il problema, in cinque righe di transcript
>
> ```text
> > Il libro rosso è sul tavolo.   ->  Learned: located_in(book_red, tavolo).
> > dove si trova il libro rosso   ->  muro
> > dove si trova book_red         ->  Tavolo.      ← solo col nome INTERNO
> > Il gatto è sul tetto.
> > dove si trova il gatto         ->  Tetto.       ← una parola sola: funziona
> ```
>
> Il **lettore** lega un *sintagma*: unisce i token fino al confine di sintagma e
> produce una chiave sola (`book_red`). La **domanda** provava un token alla
> volta — `located_in(il,?)`, `located_in(libro,?)`, `located_in(rosso,?)` — e
> non provava **mai** la frase intera. Il fatto c'era e non era raggiungibile.
>
> > **parrot0 imparava sotto un nome che non sapeva più pronunciare**, e ogni
> > entità di più di una parola finiva in un cassetto senza maniglia.
>
> ### Perché era il collo di tutto
>
> Due giri di insegnamento massiccio — 276 forme reali entrate parlando,
> verificate in processo nuovo — avevano mosso **+11 turni su 360**. Non perché
> il metodo fosse debole: perché un turno riesce solo se tengono **insieme**
> superficie, forma della domanda, nome dell'entità, riferimento, fatto e
> realizzazione, e il nome dell'entità cedeva sempre. I referenti multi-parola
> («il libro rosso», «il quaderno blu», «il treno notturno») sono la norma del
> parlato, non un caso limite — ed è anche il motivo per cui la coreferenza era
> la famiglia peggiore del corpus: non aveva **niente a cui attaccarsi**.
>
> `book_red` non è un nome scomodo: è un nome che ha **perso informazione**.
> Testa fusa col modificatore, determinante buttato, ordine invertito, lingua
> cambiata a metà — e ogni perdita chiude una porta diversa (chiedere «quale
> libro?», distinguere «un libro» da «il libro», risolvere «il primo»,
> ripronunciarlo come è stato detto).
>
> ### Il piano di soluzione — la giunzione in cinque gradini
>
> L'invariante che li governa tutti:
>
> > **ciò che si impara da una frase dev'essere interrogabile con la stessa
> > frase, e ridicibile come è stato detto.**
>
> | # | gradino | stato |
> |---|---|---|
> | **G1** | **La domanda prova i sintagmi che il lettore ha costruito.** Non un secondo indice: le *stesse tre cose* del lettore — confine di sintagma (`np_closer/1`, conoscenza), caduta del determinante, la stessa `p0_join`. Additivo: i passaggi per token restano. | ✅ **FATTO** 2026-08-31 |
> | **G2** | **Testa e proprietà.** «il libro rosso» → testa `libro` + proprietà `rosso`, così «il libro» combacia e «di che colore è il libro» risponde. Dov'è la testa è **conoscenza** (`noun_phrase_head_position(Language, first \| last)`), non una regola nel C. | aperto |
> | **G3** | **Il referente.** Un'entità introdotta occupa una **posizione nel discorso**: parrot0 ricorda che cosa è stato nominato e in che ordine, e «il primo»/«il secondo» ci si attaccano. Quale posizione di un fatto introduca un referente è una **politica** (`referent_arg_position/1`), non una scelta del C. | ✅ **FATTO** 2026-08-31 (prima forma: ordine + chiave; determinante e span restano G5) |
> | **G4** | **La coreferenza si attacca al referente.** «il primo», «quello», «l'altro» diventano `referent_same/3` — una **relazione**, non una fusione. | aperto |
> | **G5** | **Il referente sa ridirsi.** `referent_surface/3`: rispondere «il tavolo» come è stato detto, non `tavolo`. | aperto |
>
> ### Come si misura che funziona
>
> **Non con una percentuale sul totale: con una famiglia che chiude.** Il gate è
> che il dialogo `gd1_011` — cinque turni, due oggetti, un riferimento — passi
> **da capo a fondo**. Una catena che regge vale più di dieci punti sparsi,
> perché dieci punti sparsi non provano che nessuna catena regga.
>
> ### La forma ricorrente, che è la lezione vera
>
> È la **terza volta** che compare lo stesso difetto sotto un vestito diverso:
> D33 (un'interpretazione congelata perché la KB non può richiamare la lettura),
> D35 (una chiave costruita da un percorso e non dall'altro), D37 (una struttura
> costruita da un percorso e ignorata dall'altro).
>
> > **Due percorsi che devono accordarsi, e non condividono l'oggetto su cui
> > accordarsi.**
>
> Prima di aggiungere una capacità, la domanda da farsi è: *chi altro deve
> accordarsi con questa, e su che cosa?*
>
> E la stessa mossa, un piano più su, è **l'unificazione fra zone della KB**
> (D38, §18.43): far parlare aritmetica e sociale, prosa e geografia, non è
> ingegneria di dettaglio — è la condizione perché emergano abilità che nessuno
> ha progettato. Differenziarsi non basta: le parti differenziate devono potersi
> parlare.
>
> Dettaglio completo: `docs/plans/frontier-kb-natural-dialogue.md` §18.40 (D35),
> §18.42 (D37) · referto
> `docs/labs/apprendimento-assistito/2026-08-31-perche-non-cresceva.md` · coda
> `LEARN_TODO.md` GD9.


## Coda GD — apertura generica al dialogo (2026-08-31)

Nasce dalla richiesta di F.: «i giri fatti hanno lavorato sulla profondita' di
ragionamento; servono giri di apertura generica al dialogo — varieta', slang, in
maniera massiccia». La misura GD1 dice che aveva ragione: **236 muri su 360
turni** di conversazione ordinaria.

| # | Piano | Classe liberata | Gate duro | Stato |
|---|---|---|---|---|
| **GD1** | **Misura del dialogo generico** | 360 turni, 60 dialoghi persistenti, 12 famiglie, it+en, processi reali | il probe localizza i muri per famiglia invece di dare un numero solo | **CHIUSA** — baseline 236 muri / 117 move_match. `scripts/dialogue_corpus_probe.py` |
| **GD2** | **Lessico colloquiale massiccio** | attacchi informali, stanchezza, noia, buonumore, frustrazione, accordo, incoraggiamento, battuta | ogni forma entra parlando; il cue sopravvive dentro una frase lunga; processo nuovo | **CHIUSA** — 200 forme insegnate, 193 persistite, F01 da 14 a 18 match (+29% relativo). [Report](docs/labs/apprendimento-assistito/2026-08-31-gd1-gd2-apertura-dialogo.md) |
| **GD11** | **La risposta segue la lingua del turno** | — | una frase italiana non riceve una cornice inglese con dentro parole italiane | nessuna: la lingua del turno e' gia' un fatto (`current_language/1`) | «il mio libro e' sul tavolo» riceve una risposta interamente italiana; nessuna resa mista | **aperto — piccola e molto visibile.** Misurato: «Got it: your libro is tavolo.» Oracolo: «Ah, ottimo! E' un buon posto per un libro.» |
| **GD12** | **Il possessivo introduce un referente recuperabile** | D35, G1/G2 | «il mio libro e' sul tavolo» deve essere raggiungibile da «dove si trova il mio libro» | nessuna | la domanda formata con le STESSE parole trova il fatto appena appreso | **✅ chiusa 2026-09-01.** Non era un fatto mancante: era un fatto SBAGLIATO. Il ramo del possesso prendeva l'ultima parola come nome, cioe' registrava «il libro si chiama tavolo». Curata togliendo la rivendicazione (guardia KB `p0_is_loc_prep`), non aggiungendone una seconda. Contrasto verificato: il possesso con nome regge. Vedi §6.3 |
| **GD13** | **«Non ho capito» e «non ho il dato» sono due cose diverse** | D29, autocorrezione | una domanda compresa a cui manca il dato riceve una richiesta del dato, non un muro di incomprensione | «di che colore e' il mio libro» -> «non lo so, di che colore e'?» | le due situazioni producono due frasi diverse; la seconda invita esattamente il fatto che manca | **✅ chiuso 2026-09-01 con la mossa #5.** `information_need/4` distingue valore e antecedente mancanti; cue, strategie e resa crescono/si ritraggono a runtime. Il seguito naturale del caso possessivo attende GD12 |
| **GD10** | **Archi di ordine superiore fra zone della KB** | D38 | aritmetica x sociale, prosa x geografia: comporre due zone che oggi non si parlano | «siamo in quattro e il conto e' 86 euro, quanto ciascuno?» — nessuno ha progettato «dividere un conto» | aggiunti N archi, i compiti risolti crescono **piu' che linearmente** in N; ogni capacita' composta e' dimostrata su un compito e sparisce se si ritratta l'arco; `false_composition/2` non resta vuoto per finta | **aperto.** F.: «connettere zone attraverso archi di ordine superiore e' una sorta di unificazione dell'intelligenza». E' la stessa cura di D33/D35/D37 un piano piu' su. Vedi §18.43 |
| **GD9** | **Un'entita' e' un referente con proprieta', non un atomo fuso** | testa, proprieta', determinante, menzione con span; il fatto lega referenti | «il libro rosso» e «il libro» sono lo stesso oggetto detto con precisione diversa | dopo «Il libro rosso e' sul tavolo»: rispondono sia «dove si trova il libro rosso» sia «dove si trova il libro»; «di che colore e' il libro» risponde dalla proprieta'; «Il libro e' grande» si attacca allo stesso referente; «dov'e' il primo» si risolve o si dichiara ambiguo | **aperto — e' la forma GIUSTA di GD7.** La gerarchia esiste gia' (clausola, sintagmi, token, span): manca la giunzione, `extract_frame` la ignora e fonde. Vedi D37 §18.42 |
| **GD7** | **Round-trip del nome dell'entita'** (conseguenza di GD9, non lavoro separato) | cio' che si impara da una frase e' interrogabile **con la stessa frase**, a qualunque numero di parole e in entrambe le lingue | nessuna: e' una simmetria di motore | «Il libro rosso e' sul tavolo» + «dove si trova il libro rosso» deve rispondere; `unnameable_fact/2` tende a zero su corpus reale; se serve il nome interno la simmetria non c'e' | **aperto — IL COLLO.** Misurato: la lettura scrive `book_red`, la domanda cerca «il libro rosso», e solo il nome interno funziona. Un'entita' di una parola fa il giro, una di piu' parole no. Vedi D35 §18.40 |
| **GD8** | **La frase ordinaria a tre ruoli e le preposizioni articolate** | «ho messo il libro sul tavolo», «e' nello zaino», «invece» | «in questa frase il terzo ruolo e' il posto» | i turni dichiarativi ordinari del corpus producono un fatto invece di un muro | **aperto.** Due dei quattro guasti del dialogo tracciato |
| **GD3** | **Una forma ha una famiglia di varianti** | apostrofo, accento, abbreviazione di chat, elisione, spaziatura, maiuscole | insegnata UNA forma canonica, tutte le varianti dichiarate funzionano senza seconda lezione; ritrattarla le spegne tutte; una variante ambigua (`e`/`è`) non si risolve in silenzio | **aperto — moltiplicatore, ma dopo GD11/GD12 nella coda corrente.** Misurato: «you're a legend» funziona, «you are a legend» risponde «I am a legend now» — un misclaim, non un muro. Vedi D34 §18.38 |
| **GD4** | **Il riferimento che attraversa i turni** (richiede GD7: serve qualcosa da nominare) | «quello», «l'altro», «quello di prima»: cio' che rende un dialogo un dialogo invece che una sequenza di domande | su turni multipli il riferimento si risolve o si dichiara ambiguo, mai si sceglie in silenzio | **aperto.** F03 e' la voce singola piu' alta del corpus — **24 muri su 30** — e non aveva un fronte |
| **GD5** | **Corpus raccolto, non redatto** | il corpus GD1 e' scritto da chi lo misura, quindi misura anche le proprie assunzioni | i turni vengono da conversazioni reali; il delta si conferma su di essi | aperto |


## ⛔ HANDOFF 2026-08-30 POST-SC40-B — punto di ripresa autoritativo

Questa e' la sezione da cui deve ripartire il prossimo coding agent. Anche se
l'agente ha poco contesto, non deve ricostruire la storia dai nomi dei file:
leggere nell'ordine questo handoff, il
[report SC40-A](docs/labs/apprendimento-assistito/2026-08-30-supercomprensione-sc40.md),
[report SC40-B](docs/labs/apprendimento-assistito/2026-08-30-supercomprensione-sc40b.md),
`docs/plans/frontier-kb-natural-dialogue.md` §§18.37–18.43, quindi i file di
implementazione elencati sotto.

### Risultato acquisito — non riaprire il problema sbagliato

Il difetto D33 non e' piu' «la claim conserva insieme gap e frame». La
separazione ora e' reale:

- `document_claim`, `claim_surface`, fonte e span sono **osservazioni**;
- `claim_reading_record(Claim, Reading, Signature)` e' una versione storica;
- `claim_current_reading(Claim, Reading)` e' il puntatore sostituibile;
- `reading_depends_on(Reading, Knowledge)` nomina la licenza corrente;
- `reading_stale/2` e `revision_effect/3` conservano perdita della dipendenza e
  genealogia prima/dopo;
- `claim_proposition`, `claim_normalization` e `claim_reading_extent` sono viste
  sulla sola versione corrente non stale.

La sequenza provata sullo **stesso span**, senza un secondo `read:`, e':

```text
gap(no_reading)
  -- "warm is a relation verb" --> normalized(frame(warm, authors, tube))
  -- "forget that warm ..."   --> gap(no_reading)
  -- reteach                    --> normalized(frame(warm, authors, tube))
```

Ogni transizione conserva i record precedenti. La superficie e lo status
`described` non vengono mai cancellati; `warm(authors,tube)` e
`holds_in(world,...)` restano falsi. La procedura composta cambia invece da
`blocked(unbound_step)` a `ready`, torna bloccata all'ablation e si riapre al
re-teach.

SC40-B ha chiuso anche il costo semantico grossolano di quel pass. Prima e dopo
ogni turno con claim osservate, il motore fotografa la vista dichiarativa
`revision_dependency_member/2`. Soltanto una differenza estensionale produce
un pass; il termine cambiato entra in `revision_candidate_claim/3`. Il risultato
misurato su 100 claim, una sola contenente il predicato nuovo, e':

```prolog
last_revision_pass(relation_verb(solvates), scope(selective),
                   outcome(visited(1), changed(1))).
```

Il retract attraversa lo stesso indice. Una normale asserzione tassonomica
servita da `knowledge` non cambia il receipt; una lezione di un verbo assente
dai documenti produce `visited(0), changed(0)`. Non riaprire SC40-B sostituendo
questo diff con un nuovo elenco di moduli o con una mutation flag non tipata.

### File e contratti da non rompere

1. `kb/core/document-claims.p0`
   - SC40-A parte da `document_reading_counter/1`.
   - `document_claim_revision/3` deve restare idempotente sulla stessa firma.
   - `document_claim_revision_replace/4` esiste per stare sotto il limite di
     otto goal del dialetto; non rifondere il corpo in una regola troppo lunga.
   - `reading_basis_live(relation_verb(P))` da' l'ablation esatta ai membri
     appresi. `frame_predicate(P)` e' deliberatamente piu' grossolano: e' il
     primo bersaglio di SC41, non una precisione da fingere gia' chiusa.
   - `revision_schedule/2`, `revision_trigger_module/2` e
     `revision_dependency_member/2` sono la policy. Il modulo e' soltanto un
     gate; il delta dei membri e' la causa del pass.
   - `claim_token_observation/2` e' un indice di **osservazioni**. La regola
     candidata basata sui token serve i gap `unresolved`; quella basata su
     `reading_depends_on` serve letture riuscite e retract. Togliere una delle
     due rompe recall in una direzione diversa.
   - `last_revision_pass/3` e' il receipt auditabile. L'esito e' impacchettato
     in `outcome/2` per rispettare l'arita' P0; non trasformarlo in una stringa.
2. `src/brain/30-generation-reading.c`
   - `document_claim_interpret` e' la **sola** porta per prima lettura e
     revisione. Duplicarla ricrea due parser che divergeranno.
   - `document_claim_index_tokens` normalizza/splitta e pubblica token atomici;
     non decide classi, radici o sinonimi. Morfologia e composti sono SC41.
   - `DocumentRevisionSnapshot` enumera termini opachi dalla vista KB, confronta
     aggiunte/rimozioni e chiama `document_revision_selective_pass`. Il full
     scan sopravvive soltanto come fallback su snapshot indisponibile.
   - `document_revision_claims` e' la meccanica condivisa fra selettivo e
     fallback; misura la sostituzione del puntatore corrente, non il semplice
     successo della funzione.
   - `document_claim_status_question_turn` fa cedere `repair` usando evidenza
     derivata dalla KB; nessuna parola (`was`, `it`, `described`) e' cablata.
3. `src/brain/90-repair-robust-abduce.c`
   - la sola nuova guardia lascia passare una domanda documentale riconosciuta.
     Non allargarla a «tutti i pronomi»: romperebbe il ciclo di chiarimento.
4. `src/brain/99-registry.c`
   - la fotografia e' presa prima del dispatch e liberata dopo il confronto.
     `kb_footprint_reset` resta successivo alla fotografia, cosi' la metrica del
     turno non include l'audit preparatorio.
   - il pass avviene dopo l'atto didattico e prima del turno successivo. Il
     trigger del modulo autorizza soltanto il confronto; senza delta non parte
     nessuna revisione.
5. `kb/core/responses.p0`
   - `claim_status_unreadable` e' una risposta di **livello**, non un muro
     cosmetico: atto/status compresi, proposition non normalizzata.

### Ratchet, numeri e limiti di test

- `tests/p0t/language/document_revision.p0t`: **63/63**. E' il ratchet SC40:
  baseline naturale, revisione senza replay, record storico, world isolation,
  domanda pronunciata, ablation, reteach, retention, Transfer@3, contrasto e
  receipt selettivi `1/1` e `0/0`.
- `tests/p0t/language/document_revision_scale.p0t`: **11/11**. Stress separato:
  100 claim, una candidata, risposta con fonte, retract simmetrico. Non e' nel
  target rapido: l'ingestione sul solver corrente richiede circa 90 secondi.
- `tests/p0t/language/document_method.p0t`: **25/25**. Il blocco `warm` non
  rilegge piu' il documento.
- `tests/p0t/language/document_claims.p0t`: **182/182** dopo la migrazione a
  viste versionate.
- Questi due ultimi file e `document_revision.p0t` sono ora nel target `test`.
- L'unico `make soft-test` del ciclo SC40-B e' gia' consumato: **55 passati, 1
  fallito**, il rosso storico `frontier_chat_audit.it.p0t` riga 97 su
  `designation`. Non rieseguirlo in questo checkpoint e non cambiare l'attesa.
- Il test-engine va avviato dal binario corrente. Un demone precedente alla
  ricompilazione produce falsi rossi: controllare PID/socket e riavviare. Nel
  sandbox attuale il socket Unix puo' richiedere il permesso gia' approvato per
  `./bin/parrot0 --test-engine`.
- `repair_stress.p0t` resta rosso 7/8 su attese di virgolette/configurazione e
  induzioni preesistenti; non e' stato usato per cambiare attese.

### Persistenza e conteggio del checkpoint

`/save` ha dichiarato **S=35**. Classificazione: `W=0`, `L=3`, `C=0`, `P=6`,
`O=26`, `X=0`. Lezioni persistite: `relation_verb(modified)`,
`relation_verb(modulates)`, `relation_verb(biases)`; fonti e reading facts sono
in `fact-provenance.p0`; 24 turni in `transcripts.p0`.

`kb/learning/learned.p0` contiene ora:

```prolog
exchange(paris, located_in, france).
exchange_turn(9, paris).
```

Sono record episodici veri, non fixture; sono finiti nella ricaduta perche' il
save-map non ha una casa per predicati prodotti soltanto dentro un `assert` di
regola. **Istruzione storica, ora superata:** non andavano cancellati senza una
casa. Il checkpoint 2026-09-01 ha chiuso il debito conservandoli come
`archived_exchange/4` nei transcript e dichiarando `exchange/3` e
`exchange_turn/2` scratch di conversazione; `savemap.p0t` prova save e fresh
boot. Non reintrodurre i predicati vivi in un file caricato al boot.

B0/R0 `36715/2529`; B1/R1 `36750/2529`; differenza 35. Fresh recall lessicale
3/3; composizione GMD e proof 1/1. Una composizione NASA con la parola `impact`
e' stata rubata dal modulo di impact analysis: e' il campione concreto da
portare in SC26/SC30, non una regressione di persistenza.

SC40-B e' un secondo ciclo `meta-capability-only`: **nessun `/save`**, `S=0`.
La sessione held-out ispezionata aveva 105 clausole (`W=0, L=1, C=0, P=2,
O=102, X=0`), ma il verbo `solvates`, i documenti sintetici e i loro handle
sono rimasti in quarantena. B0/R0 SC40-B `36750/2529`; B1/R1 `36757/2536`,
incremento interamente core. Non salvare la sonda per far sembrare il ciclo un
guadagno lessicale: il risultato e' il meccanismo che usera' lezioni future.

### Coda complessa delle prossime ore — consumare in quest'ordine

| ordine | voce | ipotesi | incremento piccolo ma causale | prova che chiude il taglio |
|---:|---|---|---|---|
| 1 | **SC41 — dipendenza completa della lettura** | D34/D35/D40 | materializzare identita' di schema, morfologia, marker, modalita', ellissi e policy di copertura usati dal frame | ablation di ciascuna coordinata rende stale esattamente le letture che la usano; nessuna stale spuriosa su tre documenti controllo |
| 2 | **SC42 — propagazione transitiva** | D39/D41 | fare di argomento, metodo, sintesi e risposta nodi che dipendono da letture, non snapshot indipendenti | una lezione sul verbo aggiorna claim -> method readiness -> answer; retract percorre lo stesso DAG al contrario senza cancellare osservazioni |
| 3 | **SC43 — budget e utilita' della revisione** | D36/D37 | dichiarare costo, priorita' e `retroactive_gain`; quando il budget non basta, produrre una coda stale ispezionabile | fra due lezioni candidate si sceglie quella che riabilita piu' claim; ricerca incompleta dice `pending_revision`, mai «nessun effetto» |
| 4 | **SC40-C — receipt per evento e fault injection** | D35/D40 | sostituire l'ultimo receipt con eventi monotoni; provare snapshot indisponibile, cambi multipli e replay idempotente | nessun evento perso; fallback full produce lo stesso snapshot; due membri cambiati hanno due receipt e una sola chiusura transazionale |
| 5 | **SC35 — coverage prima della risposta** | D29 | unificare copertura di token, ruoli, premesse e operatori sotto `coverage_shortfall` | tre overclaim iniettati producono tre shortfall e nessuna risposta piu' forte della propria coverage |
| 6 | **SC38 su SC4/SC6** | D31 | aprire disegno sperimentale e causalita' come nuove policy/viste sulle claim, senza terzo estrattore | esperimento, osservazionale e simulazione; associazione non diventa causa; ritrarre una policy spegne solo la vista |
| 7 | **SC19 — reticolo dei livelli** | D34/D38 | `observed < surface < normalized < grounded < modelled`, con requisiti dei consumer | retract abbassa il livello e spegne soltanto consumer troppo esigenti; ogni declino nomina la coordinata mancante |
| 8 | **SC21 — regione nello spazio logico** | D38 | rappresentare vincoli e insieme dei modelli compatibili; una lezione restringe, un retract riallarga | entailment = assenza di contromodello entro budget; ricerca incompleta resta `incomplete`; transfer logica/scienza/metodo |
| 9 | **SC24 — curriculum dal guadagno retroattivo** | D36/D37 | aggregare residui e stimare quante letture una lezione candidata rivedrebbe | la prima richiesta didattica massimizza il guadagno misurato dopo; nessuna auto-promozione |
| 10 | **SC16 — articolo held-out ricorsivo** | tutte | comporre mappa, claim, argomento, metodo, modelli, limiti, revisioni e sintesi con proof | >=70% held-out, domande avversariali, nessun claim oltre proof, revisioni locali e recuperabili |

### Strategia concreta per SC41

Non aggiungere subito nuove famiglie al diff. Prima rendere completa e
falsificabile UNA lettura riuscita:

1. su una claim attiva/passiva/modale, registrare tutte le coordinate realmente
   consultate: identita' dello schema, regola morfologica, ordine dei ruoli,
   marker epistemico, forza modale, recupero dell'ellissi e policy di copertura;
2. distinguere dipendenze **licenza** (senza il fatto la lettura non esiste) da
   dipendenze **scelta** (il fatto ha fatto vincere una lettura fra candidate).
   Il retract della seconda deve poter riaprire ambiguita', non sempre un gap;
3. far esporre alla KB ogni nuova famiglia con
   `revision_dependency_member(document_claim, Term)`. Il C deve continuare a
   vedere soltanto termini opachi e un diff; se compare `strcmp` su `modal` o
   `passive`, fermarsi;
4. estendere l'indice candidato con relazioni KB dotate di provenance. Per
   `warm`/`warmed`, la radice deve provenire dalla morfologia dichiarata gia'
   usata dal frame binder, non da uno stemmer parallelo nel reader;
5. costruire una matrice di ablation: una coordinata ritratta per volta, tre
   documenti controllo, firma prima/dopo, receipt visited/changed e consumer
   ancora autorizzati. Ogni retract deve spegnere solo il suo taglio;
6. aggiungere il caso di sostituzione: due schemi competono, una policy cambia,
   la lettura passa da frame A a frame B conservando entrambi nella genealogia;
7. misurare `DependencyCompleteness = coordinate registrate / coordinate
   consultate`. Non dichiarare SC41 chiusa finche' il denominatore vive soltanto
   in un commento C non enumerabile;
8. conservare il fallback SC40-B durante l'estensione. Una famiglia non
   indicizzata deve causare un pass full dichiarato o restare gate aperto, mai
   perdere una revisione in silenzio.

### Stop condition per agenti futuri

Fermarsi e scrivere un referto, senza `/save`, se accade una di queste cose:

- per selezionare una revisione si aggiunge in C un verbo, marker o genere;
- una lettura vecchia viene cancellata invece di diventare stale;
- prima lettura e revisione prendono due funzioni semantiche diverse;
- il test ripresenta il documento dopo la lezione;
- una risposta attribuita entra nel mondo;
- si chiama «selettivo» un pass che non misura candidate e falsi negativi;
- una collisione viene risolta spostando un modulo senza una policy dichiarata;
- `learned.p0` viene ripulito a mano invece di dare una casa alla specie.

## ⛔ HANDOFF 2026-08-31 POST-SC41-A — punto di ripresa autoritativo

Questo handoff sostituisce quello POST-SC40-B come punto operativo. La storia
SC40 resta corretta e non va cancellata; il nuovo agente deve leggere, in
quest'ordine:

1. `MANTRA.md`, `PRINCIPLES.md`, `LEARN_PROTOCOL.md`;
2. questo handoff;
3. [report SC41-A](docs/labs/apprendimento-assistito/2026-08-31-supercomprensione-sc41a.md);
4. `docs/plans/frontier-kb-natural-dialogue.md` §§18.44–18.47;
5. i cinque file di implementazione elencati sotto.

### Risultato acquisito — non ridurre una lettura al proprio frame

Il contratto corrente e':

```text
identita' di una lettura = firma semantica + genealogia del supporto
```

Sul taglio `passive_core`, una versione normalizzata conserva:

```prolog
relation_verb(bound)                                  % licenza predicato
license(frame_pattern("@O was bound by @S", bound))  % schema scelto
license(past_participle(bound))                       % vista morfologica
license(irregular_participle(bound))                  % radice insegnabile
license(passive_auxiliary(was))
license(passive_agent_marker(by))
selection(frame_role_order(s, 1))
selection(frame_role_order(o, 2))
selection(normalization_policy(reported, normalized))
```

Una sola lezione `bound is an irregular participle`, impartita **dopo** tre
documenti NCBI, produce `visited(3), changed(3)` e tre frame corretti. Retract e
reteach percorrono lo stesso taglio 3/3 senza un secondo `read:`. Il dump
finale e' `/tmp/parrot0-session-2.p0` nella macchina del ciclo; non assumerne la
presenza in una sessione futura, il report ne conserva le righe rilevanti.

La seconda proprieta' chiusa e' same-frame/different-proof. Una costruzione
`glints -> glorphs` continua a produrre lo stesso frame quando
`relation_verb(glorphs)` viene ritratto; la nuova versione usa
`frame_predicate(glorphs)`. `document_claim_revision/3` non dichiara piu'
idempotente una firma uguale quando la versione e' stale o la base primaria e'
cambiata.

### File modificati e responsabilita'

1. `src/brain/10-memory-knowledge.c`
   - `P0FrameReading.pattern` conserva il termine esatto selezionato da
     `extract_frame/2`;
   - non interpretare il pattern in C e non aggiungere un enum `passive`.
2. `src/brain/30-generation-reading.c`
   - `claim_reading_of` restituisce pattern e copertura meccanica;
   - `document_claim_attach_frame_dependencies` enumera termini opachi da
     `frame_reading_dependency/2`;
   - `document_claim_attach_extent_dependency` chiede alla KB quale policy ha
     deciso l'esito;
   - domanda, prima lettura e revisione continuano a usare la stessa
     `claim_reading_of`.
3. `kb/core/grammar.p0`
   - `passive_frame_coordinate/4` rende interrogabili pattern, predicato,
     ausiliare e marker;
   - richiede `relation_verb(Pred)` oltre a `past_participle(Pred)`: un
     participio da solo non inventa una relazione;
   - i helper prefix/suffix tengono ogni clausola sotto il limite di otto goal.
4. `kb/core/document-claims.p0`
   - `license/1` e `selection/1` sono termini KB, non branche C;
   - la snapshot fotografa le radici insegnabili. Per i participi irregolari e'
     `license(irregular_participle(P))`, non la vista derivata
     `past_participle(P)`; questo evita due eventi sui verbi regolari;
   - `construction_frame` e policy di normalizzazione hanno per ora candidate
     conservative su tutte le letture correnti. E' recall dichiarato, non un
     indice preciso;
   - `reading_dependency_requirement/2` e
     `reading_dependency_coverage/2` danno il denominatore 7/7 locale;
   - il fallback full SC40-B e' ancora intatto.
5. `tests/p0t/language/document_revision.p0t`
   - ora **118/118** nel ratchet focalizzato;
   - contiene transfer 3/3, grow/retract/reteach, denominatore 7/7 e
     same-frame/different-proof;
   - il runner `!query` non ha reso stabili le asserzioni dirette sui termini
     composti di `reading_depends_on`; non sono state mascherate con attese
     false. La loro materializzazione e' stata ispezionata nel dump naturale,
     mentre il ratchet prova causalita' e viste piatte. Se si corregge il runner
     o la serializzazione, aggiungere il controllo strutturale diretto.

### Stato di verifica al momento dell'handoff

- `make build`: verde, nessun warning;
- `document_revision.p0t`: **118/118**;
- `document_claims.p0t`: **182/182**;
- `document_method.p0t`: **25/25**;
- sessione naturale finale: grow 3/3, retract 3/3, reteach 3/3, due risposte
  pronunciate con la fonte corretta;
- `S=0`; dump 204 clausole: `W=0, L=2, C=0, P=3, O=199, X=0`;
- B0/R0 `36757/2536`, B1/R1 `36783/2582`;
- unico `make soft-test` consumato: **55 passati, 1 fallito** nel rosso storico
  `frontier_chat_audit.it.p0t` riga 97 (`designation`: risposta corrente piu'
  ricca dell'attesa corta). Nessuna attesa e nessun codice cambiati; il daemon
  avviato dal target non e' rimasto vivo.

### SC41-B resta aperta, ma non e' il prossimo ordine del teacher

Il taglio globale non e' 7/7. Restano marker epistemico, modalita', ellissi,
coreferenza, determinanti/confini, question words, precedenza fra schemi e
propagazione downstream. Quando si tornera' a SC41-B, usare una claim che
incroci **modale + ellissi + marker nuovo** e pretendere:

1. firma epistemica versionata, non sola vista viva su `claim_modal_observation`;
2. dipendenza dalla superficie modale insegnabile e dalla forza selezionata;
3. dipendenza dalla classe/marker che attribuisce la claim;
4. recupero dell'agente come arco derivato da `elided_subject` e
   `agent_surface`, non come stringa ricostruita in C;
5. ablation una coordinata per volta e contrasto su tre claim non dipendenti;
6. `DependencyCompleteness` separata per frame, modality, epistemic marker,
   ellipsis e coreference.

### NUOVO ORDINE ESPLICITO — autocorrezione e autocrescita

Il teacher ha chiesto di affrontare, appena pubblicato SC41-A, il prompt reale:

```text
quanot fa 2 +3
```

Non correggere soltanto `quanot -> quanto`. Quello e' il campione di entrata,
non la soluzione. Il ciclo deve iniziare con uno **studio completo** dei file
dedicati ad autocorrezione, repair, gap e crescita. Usare prima:

```text
rg --files | rg -i 'auto|correc|repair|recover|growth|cresc|gap|typo|fuzzy'
rg -n -i 'autocor|self.?repair|typo|edit distance|did you mean|malformed' docs src kb tests
```

Poi leggere per intero soltanto i documenti realmente centrali trovati, senza
saltare `MANTRA.md`/`PRINCIPLES.md`. Cercare in particolare piani e report di
SC18, SC24, self-repair, gap dialogue, correction e assisted construction.

#### Gate AC0 — baseline e trace

In un processo fresco:

1. chiedere `quanot fa 2 +3` e conservare risposta, modulo vincitore, gap,
   footprint e qualunque ipotesi di repair;
2. controlli vicini: `quanto fa 2 + 3`, `qanto fa 2+3`, `quanto fà 2 +3`,
   `quanto fa due + 3`, `2 +3 quanto fa`, `quanot fa 2 + trenta`;
3. contrasti: nomi propri o parole vere vicine per distanza non devono essere
   riscritti; due correzioni equiprobabili devono produrre chiarimento, non una
   scelta invisibile;
4. individuare se il difetto e' candidate generation, ranking, intent recovery,
   tokenizer o dispatch. Non iniziare dal ramo aritmetico se il turno non lo
   raggiunge.

#### Architettura attesa, da falsificare prima di implementare

La distanza ortografica puo' essere meccanica C; **quali forme sono parole,
quali intent autorizzano la correzione e quale rischio e' accettabile sono KB**.
Il modello preferito e':

```text
superficie osservata
  -> correction_candidate(surface, candidate, evidence(edit(...)))
  -> intent-preservation / context support / ambiguity policy
  -> proposed_normalization con proof
  -> dispatch sul candidato, mantenendo la superficie originale
```

Non aggiungere `strstr("quanot")`, una mappa C di typo o un rewrite silenzioso.
Il sistema deve poter imparare a runtime un nuovo membro/una nuova classe di
confusione e perderla al retract senza rebuild. Se la correzione non e' unica,
deve chiedere. Se il candidato cambia numeri, negazione, operatore o entita',
deve essere penalizzato/bloccato da policy KB.

#### Matrice AC1 — non campioni sparsi, ma corpus stratificato

Costruire un corpus versionato e leggibile di almeno **240 prompt**, almeno 20
per ciascuno di 12 strati. Non generare 240 copie della stessa mutazione:

1. trasposizioni, omissioni, duplicazioni e sostituzioni italiane;
2. spaziatura/punteggiatura/operatori (`2+ 3`, apostrofi, accenti);
3. omissione di articoli/ausiliari e ordine colloquiale;
4. abbreviazioni chat italiane (`xke`, `cmq`, `nn`) con contesti contrastivi;
5. slang italiano contemporaneo e registri informali, senza fingere
   universalita' di forme regionali;
6. typo e contrazioni inglesi;
7. slang inglese con polisemia e contrasto;
8. code-switching italiano/inglese e termini tecnici;
9. richieste aritmetiche malformate;
10. domande fattuali/relazionali malformate;
11. continuazioni, correzioni del turno precedente e riferimenti vaghi;
12. prompt di ragionamento/procedura con rumore locale ma struttura profonda.

Dividere train/dev/held-out per **famiglia di trasformazione e lessema**: se la
stessa coppia `quanot/quanto` appare in train e test, non e' transfer. Ogni
strato deve includere positivi, quasi-positivi, negativi e ambigui.

Metriche minime:

```text
CorrectionRecall@unambiguous
CorrectionPrecision
IntentPreservation
CriticalTokenPreservation(numbers, negation, operators, entities)
ClarificationAccuracy@ambiguous
RuntimeGrowth / RuntimeRetract
TransferByTransformation
MacroCoverageByStratum
FalseRewriteRate
```

Il gate `quanot fa 2 +3 -> 5` e' necessario ma chiude soltanto AC0. AC1 richiede
macro-media, contrasto e un membro nuovo appreso senza rebuild.

### CICLO SUCCESSIVO — LEARN_PROTOCOL massivo per apertura dialogica

Il teacher non vuole un'altra manciata di esempi vari: vuole **corpus ad alta
varieta'** e piu' profondita' di ragionamento. Non usare `/save` durante la
raccolta. Il ciclo corretto e':

1. baseline su corpus, conservando tutti i gap e le collisioni;
2. clustering per causa strutturale, non per frase;
3. scegliere una lezione che copra una classe e prevederne il guadagno;
4. insegnarla parlando, provare transfer held-out, contrasto e ablation;
5. promuovere soltanto fatti linguistici veri/generali con provenance;
6. fresh boot e macro-eval sul corpus intero.

#### Corpus GD1 consigliato (minimo 360 turni)

Almeno 30 turni per 12 famiglie, bilanciati italiano/inglese dove ha senso:

- saluti, phatic, reazioni, umorismo leggero e chiusure;
- slang, abbreviazioni, registro, cortesia diretta/indiretta;
- follow-up ellittici e riferimenti a turni lontani;
- correzioni dell'utente, disaccordo, negoziazione della premessa;
- domande vaghe che richiedono un chiarimento utile;
- fact Q&A, definizioni, confronti e alternative;
- spiegazioni graduate: breve, intuitiva, tecnica, con esempio;
- ragionamento multi-step, vincoli e casi limite;
- procedure, prerequisiti, failure mode e rollback;
- prosa complessa, concessioni, scope, argomento e obiezione;
- letteratura scientifica: claim, metodo, evidenza, limite, causalita';
- code, matematica e dialogo misto con linguaggio naturale rumoroso.

Ogni famiglia deve contenere mini-dialoghi di 3–8 turni, non soltanto prompt
indipendenti: apertura generica significa mantenere scopo, atto, registro e
correzioni lungo una sequenza. Almeno un terzo dei turni di ragionamento deve
richiedere composizione di due o piu' premesse; almeno un quarto deve avere una
risposta corretta di rifiuto/chiarimento, per non addestrare l'overclaim.

Metriche GD1:

```text
DialogueMoveCoverage (macro per famiglia)
ContextRetention@3/@8
RepairSuccess e RepairOverreach
SlangParaphraseTransfer
ReasoningDepth (premesse correttamente consumate)
ProcedureCompleteness
ScientificClaimCalibration
FalseUnderstandingRate
FreshProcessRecall
```

#### Regola di promozione KB

Non salvare il corpus come trascrizioni indiscriminate e non salvare slang
ambiguo come sinonimia assoluta. Promuovere soltanto:

- cue/locuzioni con scope e registro dichiarati;
- relazioni di parafrasi con condizioni d'uso;
- pattern di repair/correzione con prova e soglia;
- template di risposta nelle due lingue;
- classi strutturali che trasferiscono ad almeno tre lessemi held-out.

Quarantena obbligatoria per source text, document handles, fixture, ipotesi non
verificate, significati regionali senza scope e qualunque risposta inventata.

### Ordine delle prossime ore, aggiornato

| ordine | checkpoint | output irreversibile soltanto dopo i gate |
|---:|---|---|
| 1 | finire SC41-A | compatibilita', unico soft-test, report finale, commit e push |
| 2 | **AC0 studio + baseline — CHIUSO** | causa isolata: `+3` e' il gap; `quanot` non e' causale |
| 3 | **AC1 meccanismo KB-first — CHIUSO sul verticale** | 27 proprieta', quattro operatori, EN/IT, runtime grow/retract e negativo telefonico |
| 4 | AC2 corpus 240 | dataset stratificato, macro-metriche, report; niente bulk-save |
| 5 | GD1 corpus 360 | baseline dialogica e clustering dei gap, mini-dialoghi multi-turno |
| 6 | GD2 LEARN_PROTOCOL | lezioni per classi, transfer/contrast/ablation/retention |
| 7 | GD3 persistenza | quarantine audit, `/save` selettivo, fresh boot, commit/push |
| 8 | ritorno SC41-B | modalita' + marker + ellissi proof-carrying |

### Stop condition aggiuntive per AC/GD

Fermarsi e refertare se:

- il gate passa con una mappa C `typo -> parola`;
- un edit-distance candidate viene trattato come verita' senza policy/contesto;
- una correzione cambia numero, negazione, operatore o entita' senza chiedere;
- train e held-out condividono la stessa coppia lessicale;
- si chiama «massivo» un elenco di parafrasi quasi duplicate;
- lo slang viene promosso senza lingua, registro, scope o provenienza;
- la crescita della KB migliora micro-accuracy e peggiora una famiglia intera;
- `/save` precede quarantine audit, ablation o fresh-process validation.

## ⛔ HANDOFF 2026-08-31 POST-AC1 — punto di ripresa autoritativo

Questo handoff sostituisce la parte AC0/AC1 dell'handoff post-SC41-A. Non
sostituisce i contratti di lettura SC41: li lascia intatti e sposta il lavoro
immediato su AC2/GD1, come richiesto dal teacher.

### Risultato che non va reinterpretato

Il prompt:

```text
quanot fa 2 +3
```

ora risponde `5.`. Non e' stato aggiunto alcun alias per `quanot`. Le baseline
controfattuali hanno mostrato:

```text
quanto fa 2 +3   -> falliva
quanot fa 2 + 3  -> riusciva
```

La causa era `parse_num("+3")`: trattandolo come numero positivo impediva
all'espansore aritmetico di pubblicare la tripletta operando/operatore/operando.
Questo reperto e' importante per agenti futuri: **non riaprire AC1 aggiungendo
un correttore ortografico al prompt motivante.** Sarebbe una riparazione non
necessaria e lascerebbe rosso il caso con `quanto` corretto.

### Contratto implementato

In `kb/core/lexicon.p0`:

```prolog
token_variation(joined_infix_rhs, "split_operator_prefix").
machinery(turn_surface_repair).
```

In `src/brain/20-math.c`:

- `token_variation_class` enumera dalla KB quale classe licenzia la meccanica;
- nessun simbolo nuovo e' elencato nel nuovo scanner: ogni carattere candidato
  passa da `infix_operator/2`/`operator_symbol/2`;
- si divide un prefisso operatore solo se esistono un operando sinistro gia'
  leggibile e un operando destro valido;
- `note_turn_surface_repair` pubblica classe, operazione, originale e forma
  consumata in origine sessione;
- il record scade all'inizio del turno successivo in `99-registry.c`.

Il C possiede la sola operazione `split_operator_prefix`. Quali famiglie la
autorizzano e quali superfici sono operatori restano KB. Il test piu' forte e':

```text
assert runtime infix_operator("x", times)
calcola 6 x7 -> 42
retract runtime infix_operator("x", times)
calcola 6 x7 -> non 42
```

Nessun rebuild fra i tre passi.

### Evidenza esatta

`tests/p0t/math/arith_surface_repair.it.p0t` passa **27 proprieta'**:

- controllo pulito e prompt motivante;
- prova che il refuso non causale non viene registrato come repair;
- forget/reteach della policy;
- `+`, `-`, `*`, `/` con operando destro unito;
- cue `fa`, `calcola`, `calculate`, `compute` e valori diversi;
- segni unari `-2 + 3`, `2 + -3`, `-2 * -3`;
- `2 -3` come vero infisso unito;
- negativo `chiama +39`;
- membro operatore `x` aggiunto e ritratto a runtime.

Focused ratchet:

```text
arith_surface_repair.it.p0t  27/27
arith_nl.it.p0t              16/16
arith_flex.it.p0t             5/5
arith.p0t                     8/8
spell_repair.p0t              8/8
```

`make soft-test` e' stato consumato una sola volta nel ciclo AC1: **55 passed,
1 failure storico** su `frontier_chat_audit.it.p0t` linea 97, la stessa
divergenza renderer `designation` documentata da SC41-A. Non cambiare
l'aspettativa per far sembrare verde AC1. Anche `arith_flex.p0t` inglese ha un
golden storico sul wording del gap `silver`; le 11 proprieta' precedenti sono
verdi e AC1 non tocca quel renderer.

### Metodo CADRE — obbligatorio per le prossime classi

I tre piani `autocrescita*.md`, `docs/autocorrezione.md` e il frontier ora
ratificano **CADRE** (*Causal Ablation, Declarative Repair, Exogenous
transfer*):

1. separare le coordinate del rumore;
2. fare replay variandone una per volta;
3. promuovere il sottoinsieme causalmente minimo;
4. licenza KB + meccanica C cieca;
5. originale e normalizzazione entrambi conservati;
6. matrice esogena, non copie della frase docente;
7. grow/retract/reteach e negativi di confine.

Metriche da portare in ogni report:

```text
CausalPrecision
FamilyTransfer
CollisionRate
CriticalTokenPreservation
RuntimeGrowth / RuntimeRetract
FalseRewriteRate
```

Non chiamare “generale” una classe con meno di `CausalPrecision=1`, transfer
completo sulla matrice preregistrata e collisioni zero. Non chiamare
“universale” una famiglia locale anche quando passa tutti questi gate.

### Prossimo lavoro: AC2 non deve diventare un generatore di copie

Costruire almeno **240 prompt**, 12 strati x 20, nella coda gia' specificata.
Per aiutare agenti meno esperti, seguire questo ordine:

1. creare un manifest tabellare con `id, lingua, strato, trasformazione,
   lessema, intent, token_critici, esito_atteso, ambiguity, split`;
2. preregistrare famiglie e split prima di eseguire parrot0;
3. separare per coppia lessicale **e** trasformazione: nessun `quanot/quanto`
   sia in train e held-out;
4. per ogni positivo creare un negativo vicino e un ambiguo dove esiste;
5. non usare una risposta stringa come unico oracle: registrare intent,
   preservazione dei token critici e necessita' di chiarimento;
6. eseguire baseline senza `/save`; conservare risposta, `turn_outcome`, modulo,
   gap e `turn_surface_repair`;
7. clusterizzare per causa (`candidate generation`, `ranking`, `tokenization`,
   `intent preservation`, `dispatch`, `context`) e non per parola;
8. scegliere una sola classe ad alto guadagno, applicare CADRE, poi rieseguire
   held-out e macro-metriche;
9. se una classe richiede slang, conservare lingua, registro, regione/epoca e
   polisemia; mai promuoverla come sinonimo assoluto;
10. nessun `/save` finche' il diff candidato non ha `X=0` e una casa di routing
    per proof e provenance.

### Subito dopo: GD1/GD2, apprendimento dialogico ad alta varieta'

GD1 e' minimo 360 turni, 12 famiglie x30, con mini-dialoghi 3–8 turni. Non
salvare trascrizioni. L'artefatto utile e' un corpus annotato e un report di
gap per classe. GD2 sceglie classi linguistiche reali e le insegna parlando
secondo `LEARN_PROTOCOL.md`; ogni lezione deve usare fonti identificate quando
porta fatti sul mondo e deve passare replay, Transfer@3, parafrasi, contrasto,
composizione, ablation e retention.

Per massimizzare la profondita' senza costruire un phrasebook, distribuire il
corpus lungo assi ortogonali:

- atto dialogico e continuita' multi-turno;
- registro, slang, cortesia e code-switching;
- ellissi, coreferenza vicina/lontana e correzione retroattiva;
- numero di premesse consumate (0, 1, 2, 3+);
- scope di negazione, concessione, condizione e citazione;
- risposta diretta, chiarimento, rifiuto calibrato o piano;
- prosa scientifica: claim, metodo, evidenza, limite e causalita';
- procedure: prerequisiti, passi, failure mode, compensazione e rollback.

Almeno un terzo dei turni di reasoning richiede due o piu' premesse; almeno un
quarto richiede un chiarimento/rifiuto corretto. Il dataset deve contenere slang
autentico ma non rapidamente variabile o non fontato se destinato alla
persistenza. Slang presente soltanto come sonda puo' restare nel lab e non deve
entrare in KB.

### File da conoscere prima di toccare AC2/GD1

1. `MANTRA.md`, `PRINCIPLES.md`, `LEARN_PROTOCOL.md` completi;
2. `docs/autocorrezione.md`, soprattutto §§0, 6, 13 e 14;
3. `docs/plans/autocrescita.md`, la nuova §12;
4. `docs/plans/autocrescita-v2.md`, §10.1;
5. `docs/plans/autocrescita-v3.md`, §8.1;
6. `docs/labs/apprendimento-assistito/2026-08-31-autocorrezione-causale-ac1.md`;
7. `tests/p0t/math/arith_surface_repair.it.p0t` come esempio di matrice causale,
   non come template da copiare per lo slang.

### Conteggi e stato del checkpoint

```text
W = 0   nessun fatto vero del mondo salvato
L = 0   nessuna forma linguistica appresa parlando
C = 1   famiglia generale di normalizzazione licenziata dalla KB
P = 1   forma di receipt runtime turn_surface_repair/4
X = 0
/save = non eseguito
stato = meta-capability-only
```

Il report permanente e'
[`2026-08-31-autocorrezione-causale-ac1.md`](docs/labs/apprendimento-assistito/2026-08-31-autocorrezione-causale-ac1.md).
Il prossimo checkpoint non deve modificare di nuovo l'aritmetica, salvo che
AC2 trovi una falsificazione della famiglia qui documentata.

---

## P0 — Le cose che parrot0 non può sbagliare

Sono i temi che rendono parrot0 *non credibile* se falliscono, a prescindere da
quanto sia bravo altrove. Hanno la precedenza su tutto.

| # | Tema | Classe che chiude | Stato |
|---|---|---|---|
| **P0.0** | **Scegliere fra due alternative date nel turno, in base a un effetto** — con le alternative che sono *codice*, non numeri | «quale di questi due X fa Y, A o B?» — la forma con cui si chiede un confronto, in qualunque dominio | **APERTA, prioritaria** (F., 2026-08-28). Vedi l'analisi qui sotto |
| P0.1 | **Una risposta è nella lingua della domanda** — sempre, muri e messaggi di errore compresi | il muro inglese in chat italiana | **parziale**: marcatori e pareggio risolti (`012e034`); resta l'output misto — `reflexive_skeleton.it` produce «aldric is coraggioso» dentro una frase inglese. **Gen490**: la *domanda* italiana è ora insegnabile con una parola (vedi P1.12), la *risposta* no — 141 famiglie `response_template` su 854 hanno una forma italiana |
| P0.2 | **Hello world e i primi snippet**, in ogni lingua naturale e nei linguaggi principali | «non so scrivere il programma più semplice del mondo» | **fatto**: python, c, shell, javascript, sql (`012e034`, `3ecfceb`) — i tre in più sono costati *zero C*, che è la prova che il meccanismo è KB-first |
| P0.3 | **Le operazioni aritmetiche e le loro parole** | `what is 2 plus 2?` era rotto da gen443 | **fatto** (`1f9f3d9`) — la classe ora si deriva da `infix_operator` |
| P0.4 | **Un muro è un muro, non una battuta sociale** | quattro input reali ricevevano riempitivo smalltalk | **aperto** — è M0, il prerequisito di tutto il resto |
| P0.5 | **Una domanda non diventa mai un fatto** | `what causes X?`, `what requires X?`, `what is the same as X?`, `what has N X?` finivano in KB come fatti falsi | **fatto**: la regola è ora UNA (`p0_turn_opens_as_question`) applicata ai quattro rami di asserzione (`6c898e5`, questo giro). Da sorvegliare a ogni ramo nuovo |
| P0.6 | **Le unità di misura e le conversioni** | le conversioni sono la domanda più banale che un assistente riceve | **fatto** per il sistema SI (`d8020b3`): 8 fatti, replay 4/4. Restano le conversioni fra sistemi — piedi, libbre, fahrenheit — che chiedono un *calcolo*, non una tabella |
| P0.7 | **Date e tempo** | idem | **parziale**: dodici mesi su dodici, decade, secolo e anno bisestile (`d8020b3` + questo giro). Resta l'**aritmetica fra date** — quanti giorni fra due date, che giorno cade il — che chiede un calcolo, non una tabella |

### P0.0 — l'analisi, misurata al `gen459`

Il turno segnalato da F.:

```
you> quale di questi due codici aumenta la variabile i++ o i--
Hmm, I don't know about questi yet. Want me to learn about it? …
```

Non è un tema di dominio: è **una forma di domanda che parrot0 non sa ricevere**,
e il dominio (il C) è solo il campione. Misurato:

| turno | esito |
|---|---|
| `which is bigger 3 or 5` | **5.** — la forma funziona sui numeri |
| `which of these increases the variable i++ or i--` | muro |
| `what does i++ do` | muro |
| `does i++ increase i` | muro, e nomina «increase» |

Quindi la classe si spacca in tre pezzi, e vanno chiusi in quest'ordine perché
ognuno è prerequisito del successivo:

1. **Il confronto fra alternative date nel turno esiste già, ma solo per i
   numeri e solo per proprietà che sono relazioni note** (`bigger`). Qui la
   proprietà è un *effetto* («aumenta») e i termini non sono valori ma
   **frammenti di codice**. È la stessa forma — «quale fra A e B ha la proprietà
   P?» — con i tre slot riempiti diversamente.
2. **La semantica degli operatori manca del tutto.** `what does i++ do` è un
   muro: non c'è conoscenza di che cosa faccia un operatore, in nessun
   linguaggio. Va insegnata come conoscenza — `i++` aumenta di uno, `i--`
   diminuisce di uno — non cablata.
3. **Il muro italiano nomina la parola sbagliata.** Indica `questi`, che è un
   dimostrativo: una parola funzione, non l'argomento del turno. È esattamente
   il fallimento che `99-registry.c` ha già annotato al gen384 — il declino che
   finiva per nominare «facciamo», «chiamo», «allora» — e che lì fu contenuto
   spegnendo la nomina dove il vocabolario non è completo. In italiano la
   guardia non basta: `questi` **è** nel lessico, quindi passa il filtro e viene
   nominato. Un declino che indica un dimostrativo non aiuta chi insegna, lo
   svia.

**Perché è P0 e non P2.** Chiedere «quale dei due fa X» è una delle forme più
comuni che un assistente riceve, in qualunque dominio: due farmaci, due
algoritmi, due città. Chiuderla su `i++`/`i--` non vale niente; chiuderla come
*forma* libera tutte le famiglie che hanno quella struttura — ed è il criterio
del §8 di `apprendimento-assistito.md`, «preferire il gap che, chiuso, libera più
famiglie di frasi».

**Nota di metodo, obbligatoria per questa voce.** Il punto 2 tenta al fatto
scritto a mano in `kb/experts/programming/`. Non si fa: il vincolo #3 del
protocollo vale anche qui, e per giunta questa voce serve proprio a misurare se
la semantica di un operatore è **insegnabile parlando**. Se non lo è, il
risultato della sessione è quel gap tipizzato, non un file toccato — e va in
`docs/plans/apprendimento-assistito.md` §6.2b accanto agli altri.

## P1 — Il metalinguaggio: ciò che sblocca l'insegnabilità

Non sono domini: sono le *forme* con cui si insegnano tutti i domini. Chiuderne
una vale più di dieci sessioni di fatti. Riferimento: M0–M14 del piano.

| # | Tema | Classe che chiude | Stato |
|---|---|---|---|
| P1.1 | **Dire di che tipo è la propria lacuna** (M13) | il registro era *scritto e muto* | **fatto** (`837a044`): referto con frase, ancore, opachi e specie; filtrato sulla conversazione corrente, citando le parole dell'utente; la cue cresce a runtime. `tests/p0t/meta/gap_report.p0t` 13/13 |
| P1.2 | **Ricongiungere le rappresentazioni** (M8) | `igneous_rock(basalt)` non soddisfa `igneous(X), rock(X)` — ogni classe composta è oggi un'isola | **aperto**: metà della causa comune con M3 è caduta, il bersaglio congiuntivo no |
| P1.3 | **Nominare i ruoli** (M4) | «qui il primo nome è chi agisce» — la frase con cui il piano si apre, e che parrot0 non sa ricevere | **aperto** |
| P1.4 | **La forma flessa e quella base sono lo stesso verbo** (M5) | `filter`/`filters`, `studies`/`study` sono due abilità scollegate **in ingresso** (in uscita il ponte c'è, `lemma_candidate/2`) | **aperto**, misurato nella sessione fisiologia |
| P1.5 | **Rifiutare un verbo che collide** | insegnare `needs` produceva un fatto falso perché `requires` è del modulo `plan`; oggi lo deve verificare il teacher con `grep` | **aperto** |
| P1.6 | **Ritrattare parlando un fatto binario** | `forget` copre la classe unaria e le costruzioni, non una relazione: un errore binario è irreversibile, e ha già costretto a buttare due sessioni | **aperto** |
| P1.7 | **Negazione, condizionale, quantificatore** (M7) | «nessun minerale è più tenero del talco» va a muro | **aperto** |
| P1.8 | **Il pronome che attraversa i turni** (M7) | «esso», «it», l'ellissi | **aperto** |
| P1.9 | **Causa, finalità, processo e condizione nella prosa** (M9) | «X perché Y» viene assorbito dallo smalltalk | **aperto** |
| P1.10 | **Procedure insegnate che non si eseguono** (M10) | «per identificare un minerale, prima…» va a muro | **aperto** |
| P1.11 | **Catena di costruzioni e induzione dagli esempi** (M3, resto) | l'arità è caduta, la catena no | **aperto** |
| P1.12 | **Il gloss di una parola** (M15/M16, il punto fisso) | una domanda italiana non è una superficie nuova, è la *stessa* superficie sotto una traduzione: `tr/2` sta a monte di ogni modulo | **fatto, parziale** (`gen490`): «usa is the italian of used» apre la famiglia intera; transfer 2/3, ablation verde. Restano la morfologia del gloss (M5) e `tr/2` binario — il punto fisso è `translation(Lingua, …)`. Vedi [il referto](docs/labs/apprendimento-assistito/2026-08-29-gloss-e-forma-della-domanda.md) |
| P1.13 | **Insegnare COME SI CHIEDE senza nominare la relazione** (M15, residuo) | il gen457 chiedeva `… as a way to ask side_color`, cioè il nome interno del predicato — il vincolo zero violato | **fatto** (`gen490`): si àncora a una domanda che già funziona, relazione e verso si deducono dal modello; guardia conclusiva contro il modello inesistente |

## P2 — I domini che allargano il mondo

Conoscenza vera, fontata, che serve a comporre. L'ordine è per quanto ciascuno
si compone con quello che c'è già.

| # | Tema | Perché | Stato |
|---|---|---|---|
| P2.1 | Geografia fisica e politica — capitali, fiumi, confini, fusi orari | è il dominio con più composizione possibile per fatto | da fare |
| P2.2 | Cronologia storica — eventi con anno, luogo, attori, cause | il mantra #13 chiede *event frame* interrogabili, non fatti sparsi | da fare |
| P2.3 | Chimica — elementi, gruppi, reazioni, stati | classi composte ovunque: ottimo banco per M8 | da fare |
| P2.4 | Biologia — tassonomia, organi, processi | la fisiologia è iniziata (`86d2126`), la tassonomia no | parziale |
| P2.5 | Matematica — definizioni, proprietà, controesempi | «un controesempio confuta un'affermazione universale» c'è; le proprietà no | parziale |
| P2.6 | Informatica — strutture dati, complessità, protocolli | serve alle abilità di coding di P3 | da fare |
| P2.7 | Diritto di base — contratto, obbligazione, nullità | muro totale nella sonda; dominio ad alta struttura | da fare |
| P2.8 | Economia — inflazione, tassi, offerta e domanda | metà già c'è, le relazioni causali no | parziale |
| P2.9 | Musica — forme, strumenti, notazione | muro totale nella sonda | da fare |
| P2.10 | Cucina e misure pratiche | è ciò per cui un assistente viene usato davvero | da fare |

## P3 — Le abilità da agente

Non conoscenza del mondo ma cose che parrot0 deve *saper fare*.

| # | Tema | Stato |
|---|---|---|
| P3.1 | Snippet per linguaggio: shell, javascript, sql, oltre python e c di P0.2 | **fatto** (`3ecfceb`) |
| P3.2 | Spiegare un frammento di codice riga per riga | parziale |
| P3.3 | Leggere un errore di compilazione e dire cosa manca | da fare |
| P3.4 | Formati richiesti — elenco, tabella, una frase, tre modi (mantra #11) | parziale |
| P3.5 | Riassumere conservando le proposizioni, non le parole | parziale |
| P3.6 | Tradurre conservando i ruoli, non parola per parola | parziale |
| P3.7 | Dire *quanto* è sicuro e perché, senza inventare una percentuale | da fare |

## P4 — Da riapprendere

Temi già passati che sono regrediti, o che erano stati chiusi su esempi troppo
vicini all'implementazione (§6.3: un gate verde sugli esempi che l'hanno guidato
non chiude lo strato).

| # | Tema | Perché torna in coda |
|---|---|---|
| P4.1 | L'annuncio di una regola appresa | il passaggio delle frasi alla KB aveva perso la testa: «every cat is a pet» rispondeva «cat(X).» (`210dd01`) |
| P4.2 | Le parole degli operatori aritmetici | perse nel passaggio dell'elenco dal C alla KB (`1f9f3d9`) |
| P4.3 | Il messaggio di `forget` | formattato e mai emesso; e comunque coperto da `answerframe` che rivendica il turno |
| P4.4 | La conversazione lunga in italiano | `frontier_chat_audit.it` misurava una KB amputata: 31 casi su 56 rossi per costruzione (`830bc59`) |
| P4.5 | Ogni tema che una migrazione KB-first tocca | le tre regressioni sopra hanno la stessa forma — la lista passa alla conoscenza e la *chiave di lettura* resta indietro. Dopo ogni migrazione, si riapprende il tema che tocca |

---

## Nota di metodo, dalle sessioni fatte

Cinque cose imparate sul campo, che valgono per ogni voce di questa coda:

1. **Il dominio si sceglie interrogando, non a tavolino.** La sessione di
   fisiologia è nata da sei muri consecutivi trovati parlando.
2. **Si verifica il verbo prima di usarlo.** `grep` su `src/` e `kb/`: un verbo
   già posseduto dal motore produce un fatto falso, non un muro (P1.5).
3. **Una sessione con anche un solo fatto falso si butta.** È già successo due
   volte, ed è meno costoso di quanto sembri: le lezioni si ridanno in minuti.
4. **La ricaduta va svuotata a ogni sessione.** Se `kb/learning/learned.p0`
   cresce, manca una casa: si crea la categoria, non si ordina il file.
5. **Un test rosso non si aggiusta cambiando l'attesa.** Delle cinquanta
   asserzioni che chiedevano «Learned rule: …» avevano ragione loro. Prima si
   capisce *chi* ha torto fra il test e il codice.
