# Caricamento della KB: il profilo come unico entrypoint

> **Stato:** architettura obiettivo, non ancora implementata. Questo documento
> descrive la migrazione dal boot differenziale corrente al caricamento guidato
> interamente da un profilo `.p0`.

## 1. Tesi

Un profilo non e' un supplemento applicato a parrot0 dopo il boot. E' la radice
del grafo di conoscenza che caratterizza quel parrot0.

Il boot obiettivo riceve un solo percorso curato:

```text
KB vuota
  -> kb_load(ProfileEntrypoint)
       -> include idempotenti per path canonico
       -> file eager materializzati
       -> provider lazy catalogati
  -> fatti runtime e riflessivi prodotti dal processo
```

Il C conosce il protocollo di caricamento, non i nomi dei file della KB. Non
deve sapere che esistono `lexicon.p0`, `grammar.p0`, `world-facts.p0`, gli
scacchi o la medicina. Questi archi appartengono al profilo e ai manifesti che
esso include.

## 2. Stato corrente da superare

Oggi il caricamento e' diviso in due fasi e la composizione principale vive nel
C:

1. `brain_create()` carica nominalmente `lexicon`, `social`, `roles`, `gloss`,
   `grammar`, `messages`, `intents`, `input`, `responses`, `capabilities`,
   `glue`, `morphology`, `presentation`, `procedures`, `personal`, `initiative`
   e, salvo `PARROT0_WORLD_FACTS=0`, `world-facts`;
2. `brain_boot()` aggiunge `PARROT0_BASE`, il dominio coding fisso e infine
   `PARROT0_PROFILE`;
3. alcuni moduli eseguono poi caricamenti parziali al primo uso: il motore
   lessicale apre `kb/core/lexeme.p0`, il planner apre
   `kb/experts/codebase/actions.p0`, mentre composer, rulespec e generazione di
   artefatti aprono `compose.p0` e `algo_steps.p0` tramite flag privati nel
   `Brain`.

Quindi il profilo corrente e' additivo: caratterizza soltanto l'ultima parte
della KB. L'identita' di base resta scelta da una sequenza di `kb_load()` nel C.
`PARROT0_LEXICON`, `PARROT0_BASE` e `PARROT0_WORLD_FACTS` possono inoltre
produrre combinazioni parziali che non corrispondono a nessun soggetto curato.

Questa struttura ha quattro costi:

- una nuova componente core richiede una modifica al boot C oltre al file `.p0`;
- test e host possono costruire parrot0 differenti amputando singoli strati
  dall'esterno;
- lo stesso file puo' essere raggiunto dal boot fisso e da un profilo senza che
  `include/1` possieda ancora un'identita' idempotente di file;
- la residenza contestuale e' una decisione sparsa nei consumer C: ciascun
  modulo conosce path, provenienza, flag "gia' caricato" e momento di apertura.
  Un nuovo provider richiede codice anche quando espone lo stesso protocollo
  logico di quelli esistenti.

Gli ultimi caricamenti non sono ancora `lazy_load`: sono quattro implementazioni
private della sua intenzione. La migrazione non deve aggiungervi una quinta
variante generica; deve eliminare la responsabilita' dai moduli.

## 3. Architettura obiettivo

`brain_create()` alloca strutture vuote. Tutti gli host chiamano poi lo stesso
boot con un profilo esplicito o col profilo distribuito di default:

```text
brain_create_empty()
brain_boot(ProfilePath)
    kb_load(ProfilePath, KB_BASE)
    project_runtime_state()
```

Chat, daemon, test engine e reload non ricostruiscono varianti del caricamento.
`brain_reload()` ripete lo stesso entrypoint e sostituisce atomicamente la KB
solo dopo un boot riuscito.

Non esiste quindi un entrypoint curato che non sia un profilo. Il file radice
deve dichiarare esattamente una identita' `profile(Name)`; un bundle o un file
core puo' essere incluso da un profilo, ma non puo' essere passato come radice
di boot per costruire incidentalmente un soggetto parziale. L'eventuale profilo
di default e' soltanto un percorso scelto dalla distribuzione quando il chiamante
non ne specifica uno: attraversa lo stesso identico protocollo.

Dopo il boot, nessun consumer conosce un path `.p0`. Quando una facolta' ha
bisogno di `lexeme/1`, `plan_goal/2` o `program_shape/2`, formula quel goal come
sempre. Il resolver consulta la registry costruita dal profilo: se un provider
catalogato soddisfa la propria espressione `lazy_load`, lo materializza una
volta; se il profilo non lo include, il provider non appartiene a quel soggetto.
Spariscono cosi' sia i flag `*_kb_loaded` sia la scelta del file dentro i moduli.

Restano fuori dal grafo curato soltanto fatti che nascono realmente dal processo
vivo, per esempio PID, lingua del sistema, moduli compilati e authority effettiva.
Sono un overlay runtime, non un secondo catalogo di file. Un profilo puo'
dichiarare capacita' e policy desiderate, ma non puo' concedersi permessi su
filesystem o rete che l'ambiente non gli ha dato.

## 4. Anatomia dei profili

La forma proposta usa soltanto include e fatti `.p0`:

```prolog
% kb/profiles/conversational.p0
profile(conversational).
:- include(../core/profile.p0).
:- include(../experts/programming/coding.p0).
```

```prolog
% kb/profiles/agi.p0
profile(agi).
profile_extends(agi, conversational).
:- include(conversational.p0).
:- include(../bundles/agi-experts.p0).
:- include(../bundles/agi-skills.p0).
```

`profile_extends/2` e' conoscenza interrogabile; l'ereditarieta' effettiva e'
l'`include/1`. La registry idempotente per path canonico rende sicuri profili
annidati e grafi a diamante: una dipendenza comune contribuisce fatti e regole
una volta sola, nell'ordine del primo incontro.

Quando un profilo ne include un altro, `profile_extends/2` rende esplicito il
rapporto comportamentale, ma l'identita' del soggetto avviato resta quella del
file radice. La presenza transitiva di piu' fatti `profile/1` non deve percio'
essere scambiata per piu' entrypoint simultanei: la registry conserva quale file
e' stato scelto come radice e quali profili sono sue dipendenze.

Un profilo determina due insiemi:

```text
resident_at_boot(Profile, File)  % file eager raggiunti dal grafo
available_context(Profile, File) % provider lazy raggiunti e catalogati
```

Le due viste dovranno essere riflessive o derivabili dalla registry, perche'
parrot0 possa distinguere onestamente cio' che ha gia' in memoria da cio' che il
suo profilo gli permette di aprire.

## 5. Il file come unita' di vita della KB

L'organizzazione su disco non e' cosmetica. Uno stesso confine di file governa:

- provenienza tramite `file_attribute/1`;
- identita' e dipendenze tramite `include/1`;
- residenza tramite `lazy_load/1`;
- appartenenza comportamentale tramite il profilo.

I criteri di taglio sono quindi semantici:

- il file e' abbastanza coeso da ricevere attributi comuni;
- le sue clausole tendono a essere necessarie nello stesso contesto;
- le porte necessarie a scoprirlo non sono chiuse dietro la sua barriera lazy;
- i legami stretti di una proof non vengono frammentati senza ragione;
- un aggregatore contiene direttive, non eredita o sovrascrive la provenienza
  fisica dei file figli.

Un file enorme che unisce domini indipendenti rende la lazy load inutile. Un
file per ogni fatto trasforma una proof in una sequenza di I/O. La granularita'
giusta e' il **contesto semantico che si apre e vive insieme**.

### 5.1 Audit gen392: perche' il manifesto eager non e' ancora un file

L'inventario del boot ha mostrato che una lista di `include/1` non basta a
riprodurne la semantica. L'ordine fisico corrente e' questo:

| ordine | componente | layer corrente | condizione |
|---:|---|---|---|
| 1 | `lexicon.p0` | base | percorso sostituibile da `PARROT0_LEXICON` |
| 2-9 | `social`, `roles`, `gloss`, `grammar`, `messages`, `intents`, `input`, `responses` | base | sempre |
| 10 | `capabilities.p0` | reflective | sempre; generato dal capability ledger |
| 11-16 | `glue`, `morphology`, `presentation`, `procedures`, `personal`, `initiative` | base | sempre |
| 17 | `world-facts.p0` | base | salvo `PARROT0_WORLD_FACTS=0` |
| 18 | fatti `i_am`, `module`, lingua e PID | reflective/session | proiezione dello stato vivo, non file curati |
| 19 | `base.p0`, `coding.p0`, profilo additivo | base | caricati da `brain_boot()` |
| 20 | `policy/2` | session | proiezione delle authority effettive |

`procedures.p0`, `social.p0` e `world-facts.p0` aprono inoltre sotto-grafi di
include. L'ordine non e' quindi una comodita': stabilisce precedenza, proof e
provenienza. In particolare, includere `capabilities.p0` da un manifesto caricato
come base la renderebbe persistibile. Le risposte potrebbero restare verdi mentre
il modello di se' diventerebbe conoscenza curata: una falsa equivalenza.

Il manifesto operativo richiede percio' un secondo asse di file, distinto da
`file_attribute/1`:

```prolog
:- file_layer(reflective).
```

`file_layer/1` e' una direttiva proposta, non ancora implementata. Governa
l'origine e quindi il ciclo di vita delle clausole fisicamente dichiarate nel
file. `file_attribute(machinery)`, invece, continua a produrre normali fatti
`machinery(Predicato)` e non deve acquisire implicitamente il potere di cambiare
layer. Confondere le due primitive renderebbe `file_attribute/1` non piu'
generica e legherebbe il significato di `machinery` al C.

Contratto richiesto per `file_layer/1`:

- il layer del file incluso e' locale al suo frame di caricamento; al ritorno si
  ripristina quello del chiamante;
- gli include interni ereditano il layer finche' il proprio file non ne dichiara
  uno diverso;
- la semantica riguarda fatti e regole fisicamente introdotti, come per la
  provenienza corretta di `file_attribute/1`;
- la prima versione curata ammette `base` e `reflective`; session, induced e
  hypothetical restano layer prodotti dal processo vivo, non authority che un
  profilo puo' autoassegnarsi;
- registry idempotente e path canonico identificano una sola coppia file/layer.
  Raggiungere lo stesso file chiedendo layer incompatibili e' un errore di
  manifesto, non un secondo caricamento;
- `lazy_load/1` conserva il layer catalogato e lo applica quando materializza il
  corpo, senza dipendere dal goal che ha aperto il provider.

Solo dopo questa primitiva e la registry canonica ha senso creare
`kb/manifests/core.p0`. Fino ad allora il manifesto resta una specifica
falsificabile, non un file morto che nessun entrypoint usa. La sua futura
composizione sara': core base sempre residente; capability ledger reflective;
world, coding, expert e skill scelti dal profilo; stato vivo proiettato dopo un
boot riuscito. La sessione non torna a essere un input.

## 6. Registry unica per eager, include e lazy

Ogni istanza KB possiede una hashmap indicizzata dal path canonico assoluto. Gli
stati minimi sono:

```text
loading -> loaded
catalogued_lazy -> materializing -> loaded
failed
```

Questa registry garantisce:

- include idempotente per fatti e regole;
- rilevamento dei cicli;
- un solo descrittore per provider lazy raggiunto da piu' profili/bundle;
- applicazione degli attributi al file fisico, indipendente dal primo includer;
- reset completo dello stato quando nasce una nuova KB.

Il dettaglio operativo di `include/1`, `file_attribute/1` e delle espressioni
`lazy_load(any(...))` / `lazy_load(all(...))` e' nel
[contratto del motore](prolog-like-engine.md#11-direttive-di-file).

## 7. Migrazione in sette passi

La migrazione accompagna le gen391-397 senza usare la lazy load per nascondere
il difetto di inferenza emerso sulla KB eager.

1. **gen391 — inventario e oracolo.** Censire l'ordine esatto del boot corrente
   e ogni `kb_load()` differenziale nei moduli; congelare conteggi, query, proof,
   provenance e diamanti di include.
2. **gen392 — contratto del manifesto core.** Congelare ordine, layer e
   componenti condizionali; specificare `file_layer/1` e le frontiere dei
   payload oggi aperti dai consumer. Non creare un aggregatore morto che il
   loader corrente non potrebbe interpretare fedelmente.
3. **gen393 — registry e provenienza.** Path canonici, regole non duplicate,
   cicli diagnosticati, provenienza isolata per file fisico e `file_layer/1`.
   Questo passo deve precedere l'entrypoint: il grafo AGI contiene gia' diamanti
   e un profilo senza registry non sarebbe equivalente.
4. **gen394 — entrypoint unico sperimentale.** Un profilo di test carica core e
   un dominio tramite una sola chiamata, confrontato col boot storico su layer,
   fatti, regole, proof e ordine.
5. **gen395 — profili completi.** Conversational e AGI diventano radici complete;
   `PARROT0_BASE`, `PARROT0_LEXICON` e `PARROT0_WORLD_FACTS` entrano in
   deprecazione.
6. **gen396 — lazy e introspezione.** Il profilo distingue residenti e provider
   catalogati; `lexeme`, `actions`, `compose` e `algo_steps` passano dai flag C
   ai trigger logici; cold e warm vengono misurati separatamente.
7. **gen397 — rimozione del boot nominale.** Tutti gli host e `brain_reload`
   usano il solo entrypoint; i vecchi override spariscono dopo la migrazione dei
   `.p0t`; fuori dal loader non rimangono chiamate a `kb_load()` con nomi di
   file curati.

## 8. Gate di correttezza

La migrazione e' completa soltanto se:

1. il profilo eager produce gli stessi fatti, regole, proof e ordine di soluzione
   del boot storico;
2. includere lo stesso file da due rami non modifica conteggi o risultati;
3. la provenienza di un file non cambia col percorso di inclusione;
4. il profilo lazy converge allo stesso risultato della variante eager;
5. un file guasto produce un errore di caricamento, non un gap epistemico;
6. nessun test comportamentale deve amputare lingua o macchinario per isolare il
   mondo: usa un profilo dichiarato;
7. il profilo AGI warm resta sotto il budget ordinario senza timeout;
8. nessun consumer C contiene un path di file KB o un flag di caricamento
   specifico di dominio: richiede predicati, non file.

Il risultato non e' soltanto un boot piu' pulito. E' una KB in cui struttura su
disco, provenienza, disponibilita', residenza e carattere di parrot0 sono parti
coerenti dello stesso linguaggio dichiarativo.
