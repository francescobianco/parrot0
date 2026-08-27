# La sessione, il dump e la provenienza

> gen382f-g. Due meccanismi che erano diventati fragili per la stessa ragione:
> facevano fare a un file due lavori diversi.

## 1. La sessione non è un input

`kb/core/session.p0` era **insieme sorgente e destinazione**: caricato al boot
come se fosse conoscenza, e bersaglio di `/save`. Con un default fisso, uguale
per ogni parrot0 sulla stessa macchina. Da lì tre difetti in uno:

- due istanze si **sovrascrivevano a vicenda** — un utente può eseguire più
  parrot0 contemporaneamente, e la sessione è per definizione unica di ciascuno;
- ciò che si salvava **rientrava dalla porta del boot** invece di essere
  instradato nell'albero curato, quindi la conoscenza appresa restava in un file
  indistinto anziché accanto ai suoi simili;
- la sessione, che è **effimera**, si comportava come conoscenza permanente.

Ora ha una sola natura: è **runtime**.

| | prima | ora |
|---|---|---|
| caricata al boot | sì | **no**, mai |
| bersaglio di `/save` | sì | **no** — `/save` instrada (save-map) |
| default condiviso | `kb/core/session.p0` | **nessuno** |
| leggibile | solo se salvata | **sempre**, dal dump |

## 2. Il dump: una finestra, non un archivio

Il file che permette di vedere cosa parrot0 ha in memoria **adesso** è il
**dump di sessione**: una rappresentazione Prolog dello stato di runtime,
riscritta a ogni turno, da guardare con `cat`.

```
$ cat /tmp/parrot0-session-117840.p0
process_pid(117840).
policy(tools, off).
current_language(en).
dog(rex).
fact_source(dog(rex), rex, "rex is a dog").
```

Tre proprietà, e nessuna è accessoria:

- **si scrive e non si rilegge mai.** Non è conoscenza da caricare. Se lo fosse,
  saremmo tornati al file che fa due lavori;
- **è unico per processo.** Il nome porta il PID, quindi due parrot0 sulla
  stessa macchina non si toccano;
- **non è versionato.** È stato, non patrimonio.

## 3. Le variabili d'ambiente

| variabile | cosa fa | default |
|---|---|---|
| `PARROT0_SESSION_DUMP` | dove scrivere il dump | `<runtime-dir>/parrot0-session-<pid>.p0` |
| `PARROT0_RUNTIME_DIR` | cartella degli artefatti di runtime | `/tmp` |
| `PARROT0_KB_ROOT` | radice dell'albero curato per l'instradamento | `kb` |
| `PARROT0_SESSION_FALLBACK` | dove finiscono i fatti che il save-map non colloca | `kb/learning/learned.p0` |
| `PARROT0_SESSION` | **obsoleta** — non carica più nulla | — |

`PARROT0_SESSION` resta accettata perché centinaia di test la impostano, ma non
ha più effetto sul caricamento: la sessione non è un input. I test che la
mettevano a vuoto per "isolare" non ne hanno più bisogno — ed è la direzione
giusta, perché la KB è parte di parrot0 e amputarla misura una creatura che non
spediamo.

## 4. Dove finisce ciò che si impara

`/save` **instrada**: ogni fatto nuovo va accanto ai suoi simili nell'albero
curato (il *save-map*), non in un unico file. Solo ciò che il routing non sa
collocare cade nella ricaduta.

```
> luca is a barista
> /save
parrot0: routed 3 clause(s) into the KB tree
```

Stessa strada per `--dream --persist`: un sogno è un contributo **committabile**,
non un giro a vuoto.

Il nome *save-map* indica il meccanismo di collocazione, non il file
`kb/savemap.tsv`. Quel TSV e' un dump derivato: il motore non lo legge e oggi
ricostruisce comunque la tabella in memoria a ogni `/save`. La direzione e'
abolire il dump e far vivere le due chiavi di routing — coppia
`(predicato, primo_argomento)` e predicato con casa univoca — nella registry di
provenienza del loader. Se un predicato abita piu' file, la sola chiave per
predicato non puo' scegliere una casa in base all'ordine di scansione.

## 5. La provenienza: `file_attribute/1`

Lo stesso principio applicato alla classificazione della conoscenza. La regola
"il file che introduce il predicato ne dichiara lo stato" era giusta e si pagava
**una riga per predicato** — e si dimentica: il motore registra quattro fughe
(gen275, gen325, gen327, gen372) e gen382e ne ha trovate altre otto in una volta,
nascoste da un troncamento silenzioso.

Ora il file lo dice **una volta**, in testa:

```prolog
:- file_attribute(machinery).
```

e ogni predicato che quel file introduce — fatti **e** teste di regola — riceve
l'attributo al caricamento.

Non è un caso speciale del motore, ed è la differenza che conta: **il caricatore
propaga l'attributo e non sa che cosa "machinery" significhi.**

```prolog
:- file_attribute(sperimentale).
zorbness(alpha).
krantic($X) :- zorbness($X).
```
```
> is zorbness a sperimentale   → Yes.
> is krantic a sperimentale    → Yes.    (anche la testa di regola)
> is zorbness a machinery      → No.     (nessuna contaminazione)
```

Il dato resta **dichiarativo** — normalissimi fatti, interrogabili, insegnabili e
ritrattabili a runtime — quindi non è la partizione congelata al boot che gen374
ha provato e scartato: è la stessa conoscenza, **derivata** dalla provenienza
invece che ricopiata.

Il `:-` è parte del contratto: indica una direttiva del caricatore e non un fatto
del dominio. La vecchia forma nuda resta leggibile soltanto per compatibilità e
non va usata nei file nuovi. La propagazione riguarda fatti e teste di regola,
non predicati citati solo nei corpi. Oggi comprende anche ciò che il file carica
con `include/1`; per questo, finché la provenienza non sarà isolata per file
fisico, la direttiva è corretta solo per un file omogeneo che non aggreghi
contenuti di natura diversa. La registry idempotente progettata separerà i frame
di provenienza: ogni incluso conserverà allora i propri attributi,
indipendentemente dal percorso che lo ha raggiunto. Il
contratto completo, inclusi i limiti e l'esempio di espansione, è in
[prolog-like-engine.md](prolog-like-engine.md#11-direttive-di-file).

`machinery/1` scritto a mano resta valido per le **eccezioni**, e serve: un file
misto non si dichiara. `meta.p0` tiene sia la grammatica interrogativa sia
`incompatible/2` — che è conoscenza del mondo per davvero — e marca solo ciò che
serve.

## 6. Il rumore di sessione — questione aperta, da non chiudere con un filtro

> Nota di F., 2026-08-27. **Non si implementa nulla per gestire il "rumore" di
> sessione.** Quello che oggi sembra rumore potrebbe essere conoscenza di ordine
> superiore, e un filtro al momento del salvataggio la distruggerebbe prima che
> qualcuno capisca a che cosa serviva.

### Il fatto osservato

Una sessione di apprendimento reale
([`sessione-01`](labs/apprendimento-assistito/sessione-01-conoscenza-vera.md))
ha salvato 93 clausole. Sette erano fatti sul mondo, cinque erano costruzioni
imparate parlando. Il resto era di un'altra natura, e due righe sono finite
dentro file curati accanto a conoscenza del mondo:

```prolog
turn_counter(18).                                    % in discourse.p0
gap_source("what orbits saturn?", "what orbits saturn?").  % in meta.p0
```

Sono state tolte a mano. La domanda non è come toglierle da sole: è **perché
erano lì**, e se debbano sparire o avere una casa.

### Perché il routing le ha collocate lì

Il save-map instrada su due chiavi — `(predicato, primo_argomento)` e predicato
con casa univoca. Nessuna delle due sa distinguere un **seme** da uno **stato**:
`turn_counter(0)` e `turn_counter(1)` stanno in `discourse.p0` come semi del
contatore, quindi `turn_counter(18)` ha trovato lì la sua casa e ci è andato.
Ha funzionato esattamente come progettato. Il difetto non è nel meccanismo di
collocazione, è che **non esiste il posto giusto dove collocarlo**.

### L'ipotesi centrale: non è rumore, è un altro ordine

Molto di ciò che cade nella ricaduta è già conoscenza dichiarata tale altrove:

- `utterance/3` — il registro della conversazione, che dal gen240 è KB
  interrogabile e non una variabile di C;
- `fact_source/3` e `reading_fact/2` — la **provenienza**, che il piano
  dell'apprendimento assistito mette fra i gate di comprensione: una capacità
  deve sapere da quale lezione discende;
- `gap_source`, `pending_gap`, `machinery_gap` — il registro di ciò che parrot0
  **non** sa, che è il materiale di lavoro dell'autocorrezione e che il piano
  §10 vieta esplicitamente di cancellare;
- `turn_counter` — oggi un contatore; ma una serie di contatori è una misura
  della sessione, e la sessione è essa stessa oggetto di conoscenza.

Chiamare "rumore" queste righe significa presupporre che la conoscenza sia
soltanto quella di primo ordine — sul mondo. Il progetto non può permetterselo:
la riflessività è la scommessa, e un sistema che sa dire «questa risposta
l'ho data così, e questa lezione la sostiene» ha bisogno proprio di questi
fatti.

### Tre ipotesi, nessuna da implementare adesso

1. **Il problema è la casa, non il filtro.** La conoscenza di secondo ordine non
   va scartata: va messa dove sta con i suoi simili, come è appena successo per
   `construction_frame/3` con `kb/learning/constructions.p0`. `learned.p0` lo
   dice già: se la ricaduta cresce, si crea la categoria mancante. La ricaduta è
   quindi un **indicatore di categorie assenti**, e leggerla come sporcizia
   spreca il segnale.

2. **La distinzione esiste già e si chiama provenienza.** `file_attribute/1`
   permette a un file di dichiarare la natura di ciò che introduce. Il giorno in
   cui il routing vivrà nella registry di provenienza del loader — direzione già
   scritta nel §4 — la chiave non sarà più «quale file contiene un fatto con
   questo predicato» ma «quale casa dichiara di ospitare fatti di questa
   natura». Un seme in un file `machinery` non attirerebbe più uno stato di
   runtime dentro un file di conoscenza del mondo.

3. **L'ordine potrebbe essere derivabile invece che dichiarato.** Un fatto che
   parla di un turno, di una lezione o di un'altra clausola è di ordine
   superiore per la sua *forma*, non per il file in cui si trova. Se questa
   derivazione si dimostrasse solida, la collocazione diventerebbe una
   conseguenza della semantica del fatto e non della cronologia di chi l'ha
   scritto per primo.

### Il vincolo, finché la questione è aperta

Qualunque cosa si faccia, **non si scarta al salvataggio**. Un fatto tolto prima
di essere capito non lascia traccia, e il piano dell'apprendimento assistito
chiede il contrario: le strutture secondarie si marcano `superseded`, `failed` o
`partial` e restano consultabili. Nel frattempo il diff della KB si legge riga
per riga prima di committare, e ciò che sembra fuori posto si annota qui invece
di sparire.
