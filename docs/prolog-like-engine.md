# Il motore Prolog-like di parrot0 (protocollo e contratto)

> **Stato:** scritto a gen278 (2026-07-07), da lettura del sorgente
> (`src/kb.c`) e da prove dal vivo via `parrot0 --mcp-engine`. Documenta cosa il
> motore **fa davvero**, non cosa un Prolog "dovrebbe" fare. È il riferimento
> sotto [docs/use-mcp-engine.md](use-mcp-engine.md) (il canale MCP) e
> [docs/plans/generative-prolog.md](plans/generative-prolog.md) (dove va).

parrot0 ha un motore d'inferenza **Prolog-like** al centro (`src/kb.c`, da
gen5/gen11): fatti, regole definite (Horn), unificazione di Robinson, risoluzione
**SLD** con backtracking e standardize-apart. Non è un Prolog completo (niente
cut, niente aritmetica dentro le clausole, niente liste come termini), ma il
nucleo di risoluzione è **n-ario e ricorsivo** — più potente di quanto alcune API
sappiano esprimere (vedi §5).

## 1. La sintassi delle clausole (`.p0`)

I file `.p0` sono il formato canonico. `%` inizia un commento (anche **inline**:
tutto ciò che segue un `%` fuori dalle virgolette è commento); ogni clausola
termina con `.`.

> **gen335 — il parser legge PIÙ clausole per riga.** Fino a gen334 `kb_load`
> trattava l'intera riga come **una** clausola: `a(1). b(2). c(3).` su una riga
> caricava solo la prima (o falliva) e **scartava il resto in silenzio** — una
> perdita di dati che ha morso almeno 5 volte. Ora il loader splitta la riga sui
> `.` di **livello 0** (fuori da parentesi e da virgolette: un `.` dentro `f(a.b)`
> o dentro `"…colors."` non è un separatore) e processa ogni clausola.
> **E ogni clausola non-vuota che NON si parsa emette un errore rumoroso su stderr**
> (`kb_load: PARSE ERROR in <file>: … dropped: '<clausola>'`): niente più perdite
> silenziose. Codice: `load_clause` + il loop di `kb_load` in `src/kb.c`.

> ⚠️ **Limite di lunghezza: `KB_TERM_LEN = 128`** (`src/kb.h:21`). Un argomento
> (incluse le virgolette) più lungo di 128 char è **rifiutato** da `parse_term`
> (`alen >= KB_TERM_LEN → 0`). L'errore rumoroso gen335 ha scoperto **13
> `response_template` morti** in `responses.p0`/`lexicon.p0` (spiegazioni di ~200-356
> char: cielo blu, stagioni, pioggia…) che non caricavano da sempre. Fix aperto:
> alzare `KB_TERM_LEN` (costo memoria, cambio a sé) o accorciare quei template.

```prolog
% FATTO — termine ground, arità qualsiasi
man(socrates).
parent(tom, bob).
math_op(addition, "combining two numbers").   % stringa fra virgolette = UN argomento

% REGOLA DEFINITA — head :- goal0, goal1, ...   ($ marca le variabili, gen280)
mortal($X)          :- man($X).                          % unaria, una variabile
grandparent($X, $Z) :- parent($X, $Y), parent($Y, $Z).   % n-aria, JOIN su $Y
ancestor($X, $Y)    :- parent($X, $Y).                    % ricorsione: caso base
ancestor($X, $Y)    :- parent($X, $Z), ancestor($Z, $Y).  %             caso ricorsivo

% NEGAZIONE esplicita (closed-world locale)
not(likes(alice, snakes)).
```

**Variabile vs atomo** (`is_var`, `src/kb.c:101`): un argomento è una **variabile**
SOLO se inizia con **`$`** (`$X`, `$nome` — named) o con **`_`** (anonima). Tutto
il resto è un **atomo costante**, MAIUSCOLE INCLUSE: `Madrid`, `socrates`, il
carattere `M` sono costanti. Un `_` singolo è una variabile **anonima fresca**
ogni volta (due `_` nella stessa clausola non si alias). Le stringhe fra `"..."`
sono un solo argomento: le virgole interne sono contenuto, non separatori
(`src/kb.c`).

> **gen284 — solo-`$` (fine del dual-accept).** La vecchia regola "maiuscola =
> variabile" (eredità del Prolog storico) è stata rimossa: `$` è un sigillo
> dedicato che i dati non usano mai, quindi il case è **pura presentazione**, mai
> un segnale di variabilità. Questo dissolve alla radice l'ambiguità `Madrid` e
> rende i caratteri singoli (`M`, `A`) costanti — prerequisito per le azioni-su-
> stringa come conoscenza (U4). Tutte le regole in `kb/` e i fixture di test usano
> `$`; l'hack di quotatura di U1 al bordo MCP ora quota solo `$`/`_`/spazi.

### 1.1 Direttive di file

Le direttive iniziano con `:-`: istruiscono il caricatore e **non** diventano
fatti del dominio.

```prolog
:- include(relative/path.p0).
:- file_attribute(machinery).
```

`include/1` carica il file indicato, risolvendo un percorso relativo dalla
directory del file che contiene la direttiva.

#### Idempotenza di `include/1`: contratto progettato

> **Stato: NON ANCORA IMPLEMENTATO A LIVELLO DI FILE.** Oggi i fatti identici
> vengono deduplicati da `kb_assert`, ma una regola letta due volte attraverso
> due catene di include può ancora essere aggiunta due volte. Questa non è
> idempotenza dell'include e non va scambiata per tale.

Durante la vita di una singola istanza KB, lo stesso file fisico dovrà essere
processato al massimo una volta, qualunque sia il numero di percorsi che lo
raggiungono:

```prolog
:- include(bundles/games.p0).
:- include(../kb/bundles/games.p0).  % stesso path canonico: nessun reload
```

Il loader manterrà una hashmap per istanza, indicizzata dal **percorso canonico
assoluto** dopo avere risolto directory del chiamante, `.`/`..` e link
simbolici. La stringa della direttiva non è l'identità: due percorsi che
risolvono allo stesso file condividono una voce.

La registry dovrà distinguere almeno:

- `loading`: file eager in lettura; un nuovo arco verso la stessa chiave è un
  ciclo, non un secondo caricamento;
- `catalogued_lazy`: intestazione registrata, corpo non residente;
- `materializing`: corpo lazy in caricamento, per fermare riattivazioni
  ricorsive;
- `loaded`: contenuto interamente residente; gli include successivi sono no-op;
- `failed`: apertura o parsing non conclusi; il fallimento resta rumoroso e non
  diventa un falso "file vuoto". Una nuova istanza può ritentare.

L'idempotenza vale per fatti **e regole**, preserva l'ordine del primo
caricamento e rende sicuri i grafi a diamante fra profili e bundle. Un ciclo non
ricorsiona, ma deve essere diagnosticato nominando la catena che lo ha prodotto.

`file_attribute/1` (gen382f, forma direttiva da gen383) dichiara una proprietà
della provenienza **una volta per tutto il file**. Dopo averlo caricato, per ogni
predicato introdotto da un fatto o dalla testa di una regola il loader asserisce
un normale fatto `Attributo(Predicato)`:

```prolog
:- file_attribute(sperimentale).

zorbness(alpha).
krantic($X) :- zorbness($X).
```

equivale, per la parte di metadati, ad avere anche:

```prolog
sperimentale(zorbness).
sperimentale(krantic).
```

Il motore non assegna alcun significato speciale a `sperimentale` o a
`machinery`: propaga il nome scelto come relazione unaria. I fatti prodotti sono
quindi conoscenza ordinaria, interrogabile, derivabile e ritrattabile. La
direttiva stessa non viene aggiunta alla KB.

Contratto e bordi da ricordare:

- contano i predicati dei **fatti** e delle **teste di regola** introdotti dal
  file; un predicato citato soltanto nel corpo non viene attribuito;
- la propagazione avviene a caricamento completato, quindi la posizione non
  cambia il risultato, ma la forma canonica va messa in testa per rendere
  visibile la natura del file;
- **comportamento corrente:** gli `include` ricadono nell'intervallo di clausole
  osservato dal contenitore, quindi il suo attributo si propaga anche ai
  predicati inclusi. Un aggregatore non deve per ora dichiararsi `machinery` se
  include conoscenza del mondo;
- un file omogeneo può usare la direttiva; in un file misto si usano fatti
  puntuali come `machinery(nome_predicato).` per non contaminare il resto;
- più direttive sono cumulative (al massimo `KB_MAX_ARGS`, oggi 4);
- `file_attribute(machinery).` senza `:-` è ancora accettato dal loader per
  compatibilità con gen382f, ma è una forma deprecata e non va scritta: sembra
  un fatto sul dominio pur essendo un'istruzione al file.

Questa è una primitiva di **provenienza**, non un'abbreviazione estetica. Va
preferita alle liste manuali ripetute solo quando l'attributo è vero per l'intero
file. Il caso d'uso e la relazione con il dump runtime sono approfonditi in
[session-and-provenance.md](session-and-provenance.md#5-la-provenienza-file_attribute1).

Con la registry idempotente, la semantica obiettivo sarà più rigorosa:
`file_attribute/1` riguarderà soltanto le clausole **fisicamente dichiarate in
quel file**. Un incluso aprirà un proprio frame di provenienza e applicherà i
propri attributi. Altrimenti lo status dello stesso file dipenderebbe dal primo
percorso che lo include, contraddicendo sia idempotenza sia provenienza.

### 1.2 `lazy_load/1`: specifica della feature di residenza

> **Stato: PROGETTATA, NON ANCORA IMPLEMENTATA.** Le forme di questa sezione non
> vanno ancora inserite nei `.p0` operativi: il loader corrente non le riconosce.

`lazy_load/1` sarà una direttiva del caricatore, sullo stesso piano sintattico di
`include/1` e `file_attribute/1`. Se il loader la incontra durante il bootstrap,
registra il file come provider lazy e **non materializza le clausole successive**.
Il resto del file entra in memoria soltanto quando i suoi predicati di ingresso
diventano goal della risoluzione.

Forma minima:

```prolog
:- lazy_load(chess_context).
```

Il file diventa residente la prima volta che il solver deve risolvere un goal
`chess_context(...)`, anche quando quel goal nasce nel corpo di un'altra regola.
Il trigger è il **simbolo logico del predicato**, mai una parola cercata nel
prompt. La superficie e il routing che producono quel goal restano conoscenza
eager nella KB.

#### Espressioni di attivazione: uno, OR e AND

La sintassi usa normali termini composti del dialetto `.p0`:

```prolog
:- lazy_load(piece).
:- lazy_load(any(piece, legal_move, opening)).
:- lazy_load(all(context_games, topic_chess)).
:- lazy_load(all(context_games, any(topic_chess, legal_move))).
```

Semantica:

- `lazy_load(P)` è abbreviazione di `lazy_load(any(P))`;
- `any(P1, ..., Pn)` è OR: basta un predicato presente nella frontiera dei goal;
- `all(P1, ..., Pn)` è AND: tutti devono essere presenti nella **stessa
  risoluzione**, anche come sotto-goal di una regola;
- `any` e `all` possono annidarsi, quindi esprimono formule monotone senza
  introdurre una seconda sintassi di liste;
- gli argomenti sono nomi di predicato. La prima versione non distingue le
  arità: `piece` copre ogni `piece/N`. Un eventuale indicatore `piece/2` sarà
  un'estensione separata, giustificata solo da una collisione reale;
- non esiste `not(...)` nell'espressione di attivazione: un contesto caricato non
  viene scaricato durante la vita della KB e una condizione negativa renderebbe
  l'attivazione dipendente dall'ordine di ricerca.

Per "presente" si intende un predicato nella frontiera SLD corrente: il goal
selezionato e i goal ancora da risolvere nel resolvente. Non basta che esista un
fatto con quel nome, e il loader non accumula incontri casuali fra turni
indipendenti. Questo rende `all(A,B)` una congiunzione logica osservabile, non
uno stato temporale nascosto. Se il dialogo deve mantenere un contesto fra
turni, sarà la KB eager a produrre nuovamente il goal di contesto.

#### Ciclo di caricamento

I provider lazy usano la stessa registry idempotente di `include/1`, con gli
stati `catalogued_lazy`, `materializing` e `loaded`: non devono esistere due
registri concorrenti per caricamento eager e lazy.

1. Al boot il loader legge l'intestazione, conserva nella voce canonica percorso,
   offset del corpo, espressione di attivazione e attributi di file, porta lo
   stato a `catalogued_lazy`, poi interrompe il caricamento.
2. Prima di enumerare fatti o regole per un goal, il solver valuta le espressioni
   lazy contro l'intero resolvente. Tutti i provider soddisfatti vengono aperti:
   caricarne soltanto uno renderebbe incompleta una query con più provider dello
   stesso predicato.
3. Il corpo viene materializzato una sola volta. Lo stato `materializing`
   interrompe cicli di include o attivazioni ricorsive; solo a caricamento
   completato passa a `loaded`.
4. La risoluzione riparte con gli indici aggiornati, così il goal che ha aperto
   il contesto vede immediatamente le clausole appena caricate.

La direttiva va nell'intestazione, prima della prima clausola del dominio:

```prolog
:- file_attribute(expert).
:- lazy_load(any(chess_context, legal_move)).

chess_context(chess).
legal_move(knight, l_shape).
```

Le clausole poste prima di `lazy_load/1` restano eager; farlo intenzionalmente è
ammesso, ma la forma normale lascia nell'intestazione soltanto commenti e
direttive. Quando il corpo viene materializzato, `file_attribute/1` si applica
ai predicati fisicamente dichiarati dal provider. Gli `include/1` successivi
alla barriera vengono seguiti in quel momento attraverso la stessa hashmap: un
file già residente non viene riletto e un incluso con una propria barriera
resta a sua volta `catalogued_lazy`.

#### Invarianti e bootstrap

La proprietà principale è l'**equivalenza eager/lazy**: a caricamento avvenuto,
query, binding, proof e ordine semantico delle soluzioni devono coincidere con
quelli ottenuti caricando il file interamente al boot. La lazy load cambia
residenza e costo di avvio, non la logica.

Da questa proprietà seguono vincoli precisi:

- il lessico, i frame e le regole necessari a produrre il predicato-porta non
  possono stare dietro la stessa barriera: sarebbe un bootstrap impossibile;
- l'attivazione deve avvenire prima della ricerca della prima soluzione, non
  soltanto dopo un fallimento, altrimenti un fatto eager potrebbe nascondere
  ulteriori soluzioni lazy;
- un errore di apertura o parsing deve essere rumoroso e distinguibile da
  "goal non dimostrabile"; il sistema non può scambiare un contesto guasto per
  assenza di conoscenza;
- reset e nuove istanze ripartono da `unloaded`; dump e introspezione devono
  distinguere contesti **catalogati** da contesti **residenti**;
- la direttiva è valida soltanto nei file curati: non è una clausola asseribile
  dal dialogo e non deve permettere a una sessione di scegliere percorsi;
- una policy di espulsione dalla RAM non fa parte di questa feature: il
  caricamento è monotono e avviene al massimo una volta per istanza.

`lazy_load/1` non è un cerotto per un solver che scansiona male la KB. Le query
warm sul contesto già aperto devono continuare a rispettare lo stesso budget
della modalità eager. La feature serve a organizzare la KB per contesti e a
ridurre memoria e boot; indicizzazione e complessità dell'inferenza restano un
contratto indipendente.

Il ratchet futuro dovrà essere `.p0t` e coprire almeno: trigger semplice, `any`,
`all`, formula annidata, controllo negativo, sotto-goal derivato, caricamento
singolo, due provider dello stesso predicato, attributi di file, include lazy,
errore rumoroso, reset ed equivalenza delle risposte eager/lazy. Nessun
`!timeout` è ammesso per nascondere il costo warm.

### 1.3 Il filesystem è parte della semantica della KB

Con `file_attribute/1`, include idempotente e `lazy_load/1`, la disposizione su
disco non è più un aiuto per navigare il repository. Il confine di un `.p0`
decide contemporaneamente:

- quale provenienza condividono i predicati fisicamente dichiarati;
- quale porzione di conoscenza può diventare residente come unità;
- quali dipendenze vengono aperte insieme;
- quale comportamento un profilo rende disponibile al boot.

Un file deve quindi essere **semanticamente coeso**. Un file enorme che mescola
contesti indipendenti rende la lazy load troppo grossolana; frammentare una sola
proof in molti file crea invece una catena di cold load. Porte, lessico e colla
necessari a scoprire un contesto restano eager; il payload raggiunto da quella
porta può essere lazy.

Un profilo `.p0` è un **manifesto comportamentale**, non una lista di comodità:

```prolog
:- include(../bundles/games.p0).
:- include(../skills/reasoning/analyze.p0).
```

Senza lazy load, gli include del profilo determinano ciò che diventa residente
al boot. Con lazy load determinano file eager già residenti e provider lazy
catalogati: i contesti che quel profilo autorizza a entrare in memoria quando
la proof li richiede. L'idempotenza permette a bundle e profili di condividere
dipendenze e formare grafi a diamante senza duplicare conoscenza o alterare
l'ordine delle regole.

L'architettura del singolo entrypoint, lo stato corrente dei caricamenti
nominali nel C e la migrazione sono descritti in
[kb-loading-and-profiles.md](kb-loading-and-profiles.md).

## 2. Il contratto d'inferenza (cosa garantisce il solver)

Il cuore è `solve()` (`src/kb.c:380-425`), invocato da `kb_query`. Contratto
misurato:

| Capacità | Supportata? | Dettaglio |
|---|---|---|
| Fatti ground ad arità arbitraria | ✅ | `parent(tom, bob)`, `persona(x, y, ingegnere)` |
| Regole unarie a una variabile | ✅ | `mortal(X) :- man(X)` |
| **Regole n-arie, variabili distinte** | ✅ | `grandparent(X,Z) :- parent(X,Y), parent(Y,Z)` |
| **Join su variabile condivisa** | ✅ | la `Y` sopra lega i due goal |
| **Ricorsione** | ✅ | `ancestor` transitivo; guardia `KB_MAX_DEPTH=64` (`src/kb.c:26,388`) sui cicli |
| Backtracking + standardize-apart | ✅ | `rename_term` per-clausola (`src/kb.c:346`); frame unico per invocazione |
| Proof-trace | ✅ | `kb_explain` → `"grandparent(tom,ann) because parent(tom,bob) and parent(bob,ann)"` |
| Negazione esplicita (fatti) | ✅ | `not(P)` fatto negativo ground blocca `P` (`kb_is_negated`) |
| **Negazione-per-fallimento nel corpo** | ✅ (gen282, U6) | `head :- …, naf(G)` — `naf(G)` riesce se `G` non è derivabile. Solo goal **ground** (floundering: declina); default con eccezioni (`flies($X):-bird($X),naf(abnormal($X))`). NON copre priorità fra default/probabilità |
| **Termini composti / unificazione strutturale** | ✅ (gen283, U3) | `f(a…)` è una struttura; `unify` ci ricorre; una variabile lega una sotto-struttura. `nat(s(s(z)))`, liste `cons(H,T)`/`nil`. Niente occurs-check; termini oltre `KB_TERM_LEN` troncano (cap) |
| **Computazione ricorsiva come conoscenza** | ✅ (gen283, U3) | Peano `add(s($X),$Y,s($Z)):-add($X,$Y,$Z)` → `add(s(z),s(z),$R)` dà `$R=s(s(z))`; `length`/`reverse` su liste. Insegnabile via `.p0` E via `kb.assert_clause` |
| **Azioni-su-stringa come conoscenza** (builtin `chars/2`) | ✅ (gen285, U4) | atomo↔lista-di-char bidirezionale (`chars(madrid,$L)`→`$L=cons(m,…,nil)` e viceversa). Così `capitalize_first($S,$R):-chars($S,cons($H,$T)),upper($H,$U),chars($R,cons($U,$T))` è una REGOLA (la mappa `upper/2` è una tabella di fatti), non C. Stringhe word-like; caratteri speciali un bordo |
| **Aritmetica come conoscenza** (builtin `is/2` + confronti) | ✅ (gen335) | `is($R, Expr)` valuta `Expr` (`add/sub/mul/div/mod`, annidabili) e lega `$R`; `lt/le/gt/ge/eq/ne($A,$B)` valutano **entrambi** i lati come espressioni e riescono/falliscono. Numeri **reali** (double; interi resi senza frazione), a differenza di Peano. Così *sommare, confrontare, filtrare i dispari, risolvere una relazione lineare* sono REGOLE insegnate (`sum_list`, `drop_odds`, `product/factor`, `catch_up` in `kb/core/procedures.p0`), non un consumer C. `is`,`lt`,`le`,`gt`,`ge`,`eq`,`ne` sono nomi **riservati** (come `chars`/`naf`): non ricadono mai nella KB. È l'abilitatore di [[teachable-procedures]] per il calcolo reale. Codice: `eval_num` + il dispatch in `solve` (`src/kb.c`) |
| Cut / control | ❌ | nessun `!` |
| Aritmetica *nativa* (`is/2`) nelle clausole | ❌ | i numeri restano atomi; per calcolare si usa Peano/strutture (U3) o i moduli `brain/*` (wordmath) |

**Prova viva** (via `--mcp-engine`, con le clausole del §1 in un `.p0`):

```jsonc
kb.query   {"pred":"grandparent","args":["tom","ann"]}  → {"provable":true}
kb.explain {"pred":"grandparent","args":["tom","ann"]}
   → {"explanation":"grandparent(tom, ann) because parent(tom, bob) and parent(bob, ann)"}
kb.query   {"pred":"ancestor","args":["tom","zoe"]}     → {"provable":true}   // tom→bob→ann→zoe
kb.query   {"pred":"ancestor","args":["ann","tom"]}     → {"provable":false}  // niente falsi positivi
```

## 3. Interrogazione con variabili (`kb_match`)

`kb_match` prende un pattern in cui `null` (MCP) / una variabile marca un buco e
raccoglie i valori che lo riempiono:

```jsonc
kb.match {"pred":"parent","args":["tom", null]}  → {"bindings":["bob"]}
```

**Limite attuale (Gap A):** `kb_match` colleziona il binding della **sola prima**
variabile e restituisce una lista **piatta e de-duplicata**. Con più buchi non
dà le tuple:

```jsonc
kb.match {"pred":"parent","args":[null, null]}
   → {"bindings":["tom","bob","ann"]}          // NON [["tom","bob"],["bob","ann"]]
```

Il join fra gli slot va perso. Le tuple sono il passo **P2** del piano
([docs/plans/generative-prolog.md](plans/generative-prolog.md)).

## 4. Origine e persistenza (provenienza)

Ogni clausola porta un tag di **origine** (`Rule.origin`/`Fact.origin`,
`src/kb.c:65`): `KB_BASE` (substrato curato dai file `.p0`), `KB_SESSION` (scritto
a runtime — es. tutto ciò che arriva da MCP), `KB_INDUCED` (generato da
`kb_induce`). `kb_save(kb, path, mask)` serializza selettivamente per origine: il
layer di sessione si persiste **separato** dal substrato curato, mai sopra di
esso. È la disciplina di sicurezza ripresa da MCP (`use-mcp-engine.md` §Sicurezza)
e dal manifesto [kb-first].

## 5. Il divario API ↔ motore (importante)

Il motore (§2) è più potente delle sue **porte di costruzione**. Oggi:

- Il path **`.p0`** (`kb_load` → `parse_to_term`, `src/kb.c:826`) costruisce
  termini n-ari pieni: **tutta** la potenza del §2 è raggiungibile da qui.
- Il path **programmatico** `kb_assert_rule_n` (`src/kb.c:259-289`), e quindi il
  tool MCP `kb.assert_rule`, **appiattisce** ogni goal del body a unario
  (`argc=1, args[0]="X"`). Perciò una regola con join **non è asseribile** con
  `kb.assert_rule`, anche se il solver la eseguirebbe.
  `kb.assert_rule {"head":"grandparent","body":["parent","parent"]}`
  → `{"ok":true}` ma `kb.query grandparent(tom,ann)` → `false`.

- **`kb.assert_clause` colma il divario (gen311, verificato dal vivo gen335).**
  Una regola n-aria con JOIN **è** asseribile via MCP — questa parte di §5 non è
  più "roadmap futura". Head e body come oggetti `{"pred":…,"args":[…]}`, e la
  risoluzione ricorsiva SLD la esegue. Provato dal vivo:
  ```
  kb.assert_clause {"head":{"pred":"is_a","args":["$X","$Z"]},
                    "body":[{"pred":"is_a","args":["$X","$Y"]},
                            {"pred":"is_a","args":["$Y","$Z"]}]}
  # con is_a(dog,mammal), is_a(mammal,animal) nella KB:
  kb.query   is_a(dog,animal)  → {"provable":true}
  kb.explain is_a(dog,animal)  → "…because is_a(dog,mammal) and is_a(mammal,animal)"
  ```

> ⚠️ **Footgun silenzioso (`$` obbligatorio, MCP non avvisa).** In
> `kb.assert_clause` un argomento **senza `$` è un ATOMO letterale**, non una
> variabile (è la stessa regola `is_var` del §1, ma via MCP morde forte perché
> **non c'è errore visibile**). `args:["X","Z"]` asserisce
> `is_a('X','Z') :- …` — inutile — e restituisce comunque `{"ok":true}`. La query
> poi fallisce senza spiegazione. **Regola pratica:** ogni variabile in
> `kb.assert_clause`/`kb.assert` va scritta `$X`, `$Y`, `$Z`. Le MAIUSCOLE nude
> sono costanti (gen284), non variabili come nel Prolog ISO.
>
> ⚠️ **Chiavi dei tool MCP (facili da sbagliare).** `gen.respond` vuole `{"input":…}`
> (non `text`). `kb.query`/`kb.explain`/`kb.match` vogliono `{"pred":…,"args":[…]}`
> (non `goal:"is_a(dog,animal)"`). `null` in `args` = variabile di query (slot da
> legare in `kb.match`).
>
> ⚠️ **Provabile ≠ raggiungibile in linguaggio naturale.** Un fatto può essere
> `provable:true` via `kb.query`/`kb.explain` e comunque restare **irraggiungibile**
> da `gen.respond` in prosa (la superficie "is a dog an animal?" non instrada al
> goal `is_a`). È il divario di *routing/colla linguistica*
> ([docs/plans/universal-input.md](plans/universal-input.md),
> [docs/plans/the-linguistic-glue.md](plans/the-linguistic-glue.md)), non un buco
> del motore: la conoscenza c'è, manca la mappa superficie→goal.
>
> ⚠️ **`answer_frame` matching è lessicale, non semantico.** Il cue insegnato con
> `answer_frame(Cue, Pred)` deve comparire come **token esatto** nella frase
> canonicalizzata. "how tall is X?" non matcha `answer_frame("height", ...)` perché
> il canonicalizzatore produce il token "tall", non "height". "what is the longest
> river?" non matcha `answer_frame("longest_river", ...)` perché i token separati
> "longest" e "river" non formano il token composto "longest_river". La regola
> pratica: il cue deve essere una parola che appare testualmente nella domanda dopo
> la canonicalizzazione (es. "wrote" per "who wrote X?", "largest" per "what is the
> largest X?"). Frame confermati funzionanti in gen335: definition, largest,
> founded, born, died, symbol, capital, currency, continent, population, author,
> discovered, invented, color, sound, language, sides, prevents.

Quindi, oggi: una regola n-aria si insegna da un file `.p0` **o** via
`kb.assert_clause` (con `$`-variabili); `kb.assert_rule` resta valido solo per
regole unarie. Le tuple di lettura restano la roadmap di
[docs/plans/generative-prolog.md](plans/generative-prolog.md) (§3). Il principio
non cambia: **il motore è fisso, si aprono i condotti** — nessuna logica di
risoluzione nuova.

## 5.1 Il divario `.p0` ↔ `kb.assert_clause` MCP (gen335, scoperto dal vivo)

> **Il parser `.p0` supporta nested expressions e builtin che `kb.assert_clause`
> MCP NON supporta.** La differenza è nell'adattatore JSON→termine, non nel motore.

| Costrutto | `.p0` | `kb.assert_clause` MCP | Note |
|-----------|-------|------------------------|------|
| Fatti ground | ✅ | ✅ | |
| Regole n-arie con join | ✅ | ✅ | head/body come oggetti `{"pred","args"}` |
| `naf(goal)` nel body | ✅ | ❌ | `naf` con nested `{"pred":...,"args":...}` dà `ok:false` silenzioso. **Va in `.p0`.** |
| `is($R, expr)` nel body | ✅ | ❌ | `is` con nested `{"pred":"add",...}` dà `ok:false`. **Va in `.p0`.** |
| `cons($H,$T)` / `nil` | ✅ | ❌ | Liste come argomenti nested. **Vanno in `.p0`.** |
| `lt/le/gt/ge/eq/ne` con espressioni | ✅ | ❌ | Confronti con nested expr (es. `eq(mod($N,2),0)`) richiedono `.p0`. |
| `ne/2` tra atomi | ❌ | ❌ | `ne` è solo numerico. `naf(eq(X,Y))` per atomi non è parsabile via MCP. Nessuna `dif/2` builtin. |

**Regola pratica per gen335:** se una clausola contiene `naf(…)`, `is(…)`,
`cons(…)`, o `lt/le/gt/ge/eq/ne(expr, expr)`, scrivila direttamente nel file
`.p0`. Usa `kb.assert_clause` MCP solo per clausole Horn pure (solo predicati
utente nel body, con argomenti atomici o `$`-variabili).

Esempio di clausola che **funziona** via MCP:
```jsonc
kb.assert_clause {"head":{"pred":"uncle_of","args":["$X","$Z"]},
                  "body":[{"pred":"sibling_of","args":["$X","$Y"]},
                          {"pred":"parent_of","args":["$Y","$Z"]}]}
```

Esempio di clausola che **NON funziona** via MCP (va in `.p0`):
```prolog
can_fly($X) :- is_a($X, bird), naf(flightless($X)).
factorial($N, $R) :- gt($N, 0), is($N1, sub($N, 1)), factorial($N1, $R1), is($R, mul($N, $R1)).
```

## 6. Mappa dei simboli C (per chi tocca il motore)

| Simbolo | File:riga | Ruolo |
|---|---|---|
| `unify` / `unify_term_term` / `unify_term_fact` | `src/kb.c:317-341` | unificazione di Robinson |
| `resolve` + `Subst`/`Bind` | `src/kb.c:297` | catena di binding |
| `rename_term` | `src/kb.c:346` | standardize-apart per-clausola |
| `solve` | `src/kb.c:380-425` | risoluzione SLD n-aria + backtracking |
| `kb_query` / `kb_match` / `kb_explain` | `src/kb.c` | prova booleana / binding / prova+traccia |
| `kb_assert` / `kb_assert_rule_n` | `src/kb.c` | scrittura fatti / regole (unario, §5) |
| `kb_load` / `parse_term` | `src/kb.c:735,656` | parser `.p0` (n-ario pieno) |
| `kb_induce` | `src/kb.c` | induzione di regole (unarie) dai fatti |
| `KB_MAX_DEPTH` / `KB_MAX_GOALS` / `KB_MAX_BODY` | `src/kb.c:25-26` | ceiling di ricorsione / resolvent / corpo |

## 7. Collegamenti

[docs/use-mcp-engine.md](use-mcp-engine.md) (il protocollo come tool MCP),
[docs/plans/generative-prolog.md](plans/generative-prolog.md) (dove il motore va:
Prolog generativo + sblocco dei bordi),
[docs/plans/unification.md](plans/unification.md) e
[docs/plans/unification-assessment.md](plans/unification-assessment.md)
(l'unificazione come collante, e l'assessment che ha diagnosticato i due gap).
