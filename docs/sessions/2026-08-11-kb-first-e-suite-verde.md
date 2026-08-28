# Sessione 2026-08-11/12 — dal chatbot rotto alla suite verde, passando per un LLM

*gen368 → gen377. Dieci commit. Il filo conduttore non era previsto all'inizio:
la sessione comincia con "provalo e dimmi cosa ne pensi" e finisce con una
riscrittura del criterio con cui si decide se una modifica fa avanzare o
regredire parrot0.*

---

## 0. Come è cominciata

Richiesta iniziale: buildare e chiacchierare un po' con parrot0. Il build era
pulito; la conversazione ha mostrato subito una spaccatura netta.

**Funzionava bene:** conoscenza fattuale (Gioconda, Shakespeare, capitale della
Francia, `17×23`), il sillogismo socratico, la memoria di sessione multi-turno
inclusa la coreferenza (`what is my dog called?` → `Rex`), e soprattutto
l'**autoesplicazione**: alla domanda *"why did you answer that way?"* parrot0
elenca la catena reale dei moduli provati e nomina quello che ha rivendicato il
turno. Non è una frase preconfezionata: è introspezione sul proprio dispatch, ed
è rimasta lo strumento diagnostico più usato di tutta la sessione.

**Non funzionava:** `"I am feeling sad today"` → *"That sounds nice — tell me more
about it."* Riproducibile su tre disclosure negative. Nessun modello di valenza:
un unico secchio (`smalltalk_continue`) applicato a qualunque confidenza, mentre
la KB conteneva già `opposite(happy, sad)`.

E due errori di parsing a ogni avvio: `KB_MAX_ARGS` è 4, e due clausole di arità
5 venivano scartate in silenzio — una era la regola-testata di gen367, l'ultimo
commit di feature, **inerte dal giorno in cui è stata scritta**.

## 1. I test investigativi (gen368)

F. voleva test `.p0t` che dimostrassero non una inferenza isolata per turno, ma la
**condotta** di un'esplorazione: una situazione, domande, e ogni risposta
*seguita* — il modello del romanzo giallo.

Sondando prima di scrivere è emerso il fatto che ha reso il test una
dimostrazione e non un esercizio: la catena causale si ferma a **due salti**, ma
la radice a tre salti **è raggiungibile conducendo** — si chiede la causa
prossima, poi si chiede di *quella*. È l'avanzamento nel discorso a superare il
limite di profondità del singolo passo.

Nato così `investigation.p0t`, poi `investigation_access.p0t` (grafo relazionale
invece di catena causale, percorso nei due versi).

**Scoperta collaterale, importante:** `kb/core/procedures.p0` conteneva già la
chiusura transitiva `causes_t/2`, caricata a ogni boot, e **nessun consumer C la
interrogava** — il modulo causale rifaceva a mano una camminata a due passi. Il
limite non era profondità di ragionamento: era debito KB-first. Stavo per
consacrarlo in un test come se fosse il design.

## 2. La KB non è un volume montato (gen368-371)

**Steer di F.:** *«La KB non è qualcosa di separato da parrot0, è una sua parte.
parrot0 è software dinamico: la KB evolve evolvendo sé stesso, non è memoria "di
volume" come se fosse un volume Docker.»*

Conseguenza secca: **un parrot0 con la conoscenza staccata non è lo stesso
soggetto con meno dati, è un altro soggetto.** 251 dei 257 `.p0t` giravano
amputando la KB via variabili d'ambiente. Misurato: l'arco investigativo produce
output **identico byte per byte** con mondo pieno e con mondo vuoto — l'ermetismo
non comprava nulla.

Al posto dell'amputazione, una garanzia più forte: la **novità delle entità**. Se
soggetti e legami nascono dentro il test e non compaiono in nessun fatto della
KB, nessun corpus può fornire la risposta — e la garanzia regge *anche* con tutta
la conoscenza montata, che è la condizione reale.

Da qui le primitive di pilotaggio dall'interno del dialogo: `!forget PRED`,
`!forget PRED(a,b)`, poi `!forget @LAYER`.

**gen371** ha chiuso il sintomo peggiore: i sandbox delle premesse erano costruiti
su una KB nuda, quindi non sapevano distinguere un articolo da un sostantivo, e
ogni classe migrata dal C doveva conservare nel C una lista di riserva. Ora un
sandbox porta un collegamento alla macchineria del genitore — letto da **una sola
funzione**, quindi i fatti del mondo non hanno percorso per entrare. Le liste di
riserva sono state cancellate. Le due metà sono blindate insieme: una parola
insegnata *fuori* viene capita *dentro* l'ipotetica, e `rex is a dog. all cats are
animals. is rex an animal?` risponde ancora `No`.

## 3. L'esperimento con l'LLM, che ha cambiato il criterio (gen371 addendum)

**Obiezione di F.:** un LLM su quell'item non isola niente. Tiene entrambi i
livelli — cosa implicano le premesse e cosa è vero — e **decide** quale gli venga
chiesto. *«Non è una questione di isolamento ma di decisione adoperata dalla
conoscenza, questo vuol dire KB-first.»*

Misurato, non supposto (`tests/probes/premise_frame_probe.py`, stesso endpoint di
`llmscore`). Senza alcuna cornice il modello sceglie da sé la lettura per
entailment e nomina l'anello mancante; chiesta l'altra dà quella del mondo;
chieste entrambe le separa **dichiarandone la relazione**. E sul caso decisivo —
premessa falsa nel mondo, *"all birds can fly. penguins are birds…"* — solleva
l'ambiguità **da solo**.

| | parrot0 | LLM |
|---|---|---|
| `is a dog an animal?` | `Yes.` | Yes |
| `rex is a dog. all cats are animals…` | `No.` | No, spiegato |
| entrambe le letture, etichettate | *impossibile* | le dà e le mette in relazione |
| pinguini (premessa falsa) | **muro** | rileva la premessa falsa |

parrot0 **sa già fare i due livelli**, ma con due macchine diverse, e la scelta fra
loro non è una decisione: è la forma sintattica dell'input che instrada in
silenzio. Le righe 3 e 4 non sono funzioni mancanti — sono **irraggiungibili per
costruzione**.

Da qui il **criterio di evoluzione** (`one-kb.md` §4c), ora in `MANTRA.md`:

> Una proposta è nella direzione giusta se **aumenta ciò che parrot0 vede e la sua
> capacità di decidere** fra le viste. È nella direzione sbagliata se **riduce ciò
> che un suo pezzo vede** per avere ragione per costruzione — anche quando è più
> semplice, anche quando i test diventano verdi.

## 4. Il refactoring KB-first, e la prova che conta (gen369-370, 374)

Quattro classi lessicali chiuse portate dal C alla KB: `universal_quantifier/1`,
`definite_article/1`, `relation_preposition/1`, `asks_slot/2`.

**La prova richiesta da F. non è che il fatto stia in un file:** è potergli **dire
parlando** che una parola inventata appartiene alla classe, e vederlo usarla subito
in quel ruolo. `tests/p0t/language/taught_lexicon.p0t` lo dimostra per ognuna, e il
valore sta nel contrasto — fra il prima e il dopo non cambia il binario, non cambia
un file, non cambia la config: cambia solo ciò che parrot0 **sa**.

```
> leak is zorf trigger of outage    I don't understand that yet.
> zorf is a definite_article        Learned: definite_article(zorf).
> leak is zorf trigger of outage    Learned: trigger(leak, outage).
```

Il caso più netto è l'interrogativo: la **stessa identica frase** prima *asserisce*
e poi *interroga*, e la risposta è ciò che quella frase aveva memorizzato due turni
prima.

**gen374 — un tentativo scartato, e vale più del codice.** Dopo che `!forget @base`
si era portato via anche la grammatica, avevo iniziato uno strato `KB_MACHINERY` in
C. F. mi ha fermato: *«mi sembra una rielaborazione in parole migliori di ciò che
volevo evitare»*. Aveva ragione due volte. Era **ridondante** (`machinery/1` già lo
dice, e meglio) e soprattutto andava **nella direzione sbagliata**: rendeva il
sandbox più comodo da abitare, mentre il bersaglio è farlo sparire. Il muro era un
segnale, e quello strato lo zittiva.

Al suo posto, la stessa capacità ma viva: `machinery/1` è insegnabile a runtime.
Gli si dice che un predicato è macchineria e la sua **autodichiarazione cambia
all'istante** — 13 fatti su 2 predicati diventano 7 su 1, e `!forget` li riporta
indietro. parrot0 corregge il proprio auto-modello mentre gira.

## 5. La suite verde (gen372, 376-377)

`make test` era **rosso da prima della sessione**, e il fail-fast nascondeva quanto:
si fermava a `introspect.p0t` (riga 442) e nulla a valle girava mai.

`introspect` non era uno snapshot stantio da riallineare: era **il bug che il suo
stesso filtro esiste per prendere**. Il marcatore `machinery/1` va messo a mano a
ogni predicato di motore nuovo, e per generazioni non è stato fatto: **19 predicati**
erano finiti nei conteggi utente, e parrot0 dichiarava 310 fatti come cose sapute
del mondo. Tre dei 19 li avevo introdotti io il giorno prima.

Riparandolo sono emersi **quattro rossi nascosti**, con quattro cause diverse:

1. `counterfactual.it` — mia regressione: marcare `is_prime` come macchineria era
   sbagliato due volte (rompeva una risposta, e un numero primo è conoscenza
   matematica). La linea corre sulla **forma**: quelle procedure sono definite da
   regole, `is_prime/1` è un elenco di fatti che qualcuno può voler chiedere.
2. `strategy` — la traccia **si troncava in silenzio** a 64 moduli mentre il
   registry era cresciuto a 68. Una traccia che tronca è peggio di nessuna traccia:
   *sembra* completa. Ora la costante è condivisa e non può disallinearsi.
3. `syllogism_universal` — il ramo Barbara decideva "è una domanda polare" cercando
   `are `/`can ` **ovunque nella stringa**, trovandoli nelle premesse e in *"what
   **can** you conclude"*, e rispondeva `Yes.` a una richiesta di **enunciare** una
   conclusione. Il discriminante appartiene all'ultima frase, e quali parole siano
   interrogativi è conoscenza: `question_word/1`.
4. `kb_conjunction` — il pianificatore su `requires/2` era **irraggiungibile**. Una
   guardia KB faceva declinare `mod_plan` su ogni process request, perché il modulo
   rivendicava tutto e rispondeva "non conosco i passi", oscurando le ricette
   memorizzate. Il difetto era l'**avidità del modulo**, non l'instradamento: ora
   declina quando non ha prerequisiti, la guardia è ritratta, e i due meccanismi
   **compongono**.

**gen377 — la migrazione dichiarata completa non lo era.** Restavano due script
shell nel `make test`, e vivevano fuori per una ragione tecnica: il formato `.p0t`
sapeva *togliere* conoscenza (`!forget`) ma non *aggiungerla*, e sapeva asserire
solo l'uguaglianza esatta. Aggiunti `!assert PRED(a, b, …)`, `<~` (contiene) e `<!`
(non contiene), entrambi gli script sono diventati `.p0t`.

Migrandoli, i due fallimenti che il blob JSON rendeva illeggibili sono diventati
leggibili: uno era un'**asserzione mal posta** (inchiodava una frase alla prima voce
di un elenco, testando l'ordine degli slot fingendo di testare il formato); l'altro
è una **tensione di design reale** — il passo di famiglia ignora deliberatamente le
guardie — ora scritta nel test invece che nascosta.

**Risultato: `total: 1619 passed, 0 failed`**, e per la prima volta il target non
contiene alcuno script shell.

## 6. Cosa è nato di trasversale

- **`make soft-test`** — la verifica di *avanzamento*, non la suite. Budget di 15
  secondi che **fa fallire il target anche a test verdi**: è una guardia sul flusso,
  e sforare significa toglierne, non alzarlo. Ha guadagnato lo stipendio più volte,
  segnalando in cinque secondi il caso esatto.
- **`make test-engine` ora si verifica**: aspetta il socket, prova che il cervello
  risponda (`tests/p0t/health.p0t`) e fallisce dicendo perché. Tre guasti diversi
  si presentavano tutti come "i test non partono" — incluso il demone che ereditava
  lo stdout di make e bloccava tutto per sempre.
- **`--test`** al posto di `--test-send` (l'alias resta).

## 7. Cosa resta aperto

- **Lo scope dentro il solver SLD** — `kb_query` non ha ancora una maschera di
  provenienza. È il pezzo che permetterebbe il caso pinguini: interrogare le sole
  ipotesi, o il solo mondo, o entrambi e confrontarli. Ora la rete c'è (suite verde),
  quindi è aggredibile. `docs/plans/one-kb.md` §4.
- **`brain_scratch_init` va fatto sparire**, non perfezionato — con lui l'ultimo
  posto in cui parrot0 pensa da menomato.
- **L'iniziativa** — `docs/plans/initiative.md` è scritto (nove siti, anatomia in
  cinque stadi, lo strato post-dispatch mancante) ma la sua KB non è ancora nata.
- **La cecità alla valenza** — il difetto trovato al primo turno della sessione è
  ancora lì.
- **Un fatto insegnato non entra nei conteggi** di "quante cose sai?" — regressione
  fra gen346 e oggi, isolata e **non** consacrata nei test.
- **`KB_MAX_ARGS` è 4** e scarta clausole di arità 5 in silenzio, `invented_object/5`
  compreso — con cinque call site in C che lo interrogano.

## 8. La lezione di metodo

Tre volte in questa sessione la cosa giusta è stata **non scrivere il codice che
stavo per scrivere**: il tetto di arità che stavo per consacrare in un test, lo
strato `KB_MACHINERY`, e la lista di parole che avrei rimesso in C come rete di
sicurezza. In tutti e tre i casi il segnale è arrivato prima dal criterio che dal
compilatore — ed è per questo che i mantra sono stati promossi a prima cosa che si
incontra entrando nel progetto (`MANTRA.md`, `CLAUDE.md`, `AGENTS.md`, `README.md`,
`make mantra`).

Il corollario meno comodo: **una dichiarazione di completezza va riverificata, non
ereditata.** Il piano del test-engine diceva "MIGRAZIONE COMPLETA, 0 failed" mentre
la suite era rossa da settimane, due script stavano fuori, e quattro rossi
dormivano dietro un fail-fast.
