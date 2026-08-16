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
