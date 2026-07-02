# parrot0 → coding agent: piano di evoluzione

> **NOTA (gen256):** questo documento resta l'inventario delle parti (gap G1–G13,
> fasi A–E, metriche), fotografato a gen148. Il **piano operativo vivo** — che
> ordina i binari, fotografa le capacità misurate (4 istanze SWE-bench REALI
> risolte, learn→build, tool locali) e i vantaggi sui coding agent LLM — è
> `docs/plans/coding-agent-evolution.md`.
>
> **Subordinato a `docs/CODE-MASTERY.md`** (lo strato teorico). Questo file è un
> *inventario delle parti* (capacità → coding, schemi di fatti, metriche, rischi),
> utile e concreto, ma **l'ordine delle fasi NON è vincolante**: la teoria dice di
> costruire un pezzo solo quando una `code-bench` fallita lo richiede (pivot duty),
> non seguendo il grafo di dipendenze A→E. Leggi prima CODE-MASTERY.md per la
> direzione; usa questo per le parti.
>
> **Stato:** analisi completata il 2026-06-20 su parrot0 gen148.
> **Metodo:** esperimenti reali di interazione + studio del codice + benchmark.
> **Principio guida:** ogni fase deve produrre un comportamento testabile,
> held-out, bilingue, anti-impostor. Il piano segue la disciplina di LOOP.md:
> un obiettivo per volta, il più piccolo passo che generalizza.

---

## 0. Risultati degli esperimenti — cosa sa fare parrot0 oggi

### 0.1 Test di coding — il verdetto

| Dominio                  | Tentativi | Riusciti | Esempi riusciti                                      |
|--------------------------|-----------|----------|------------------------------------------------------|
| Generazione codice       | 5         | 0        | —                                                    |
| Debugging / analisi      | 5         | 0        | —                                                    |
| Concetti programmazione  | 5         | 0        | —                                                    |
| Identificazione linguaggi| 3         | 0        | —                                                    |
| Sintesi comandi shell    | 5         | 5        | `wc -l <file>`, `rm -r <file>`, `sort -n`            |
| Tool use (oracolo)       | 2         | 2        | word count via `echo ... \| wc -w`, agent loop        |
| Auto-consapevolezza      | 5         | 1        | solo il loop shape generico di `mod_loop`             |

**Conclusione:** parrot0 è un eccellente ragionatore simbolico ma non ha alcuna
capacità di coding agent. Riconosce che un input è codice (register detection)
ma non lo analizza, non lo genera, non lo debugga.

### 0.2 Cosa parrot0 HA GIÀ di rilevante per un coding agent

Architettura a moduli + KB + motore inferenziale. Ogni pezzo mappa direttamente
su una funzione di coding:

| Capacità esistente       | Modulo                    | Applicazione coding                                    |
|--------------------------|---------------------------|--------------------------------------------------------|
| Motore inferenziale      | `kb.c` (risoluzione SLD)  | Type checking, analisi statica, risoluzione dipendenze |
| Abduzione                | `mod_abduce`              | "Quale patch manca perché compili?"                    |
| Controfattuali           | `mod_counterfactual`      | "Cosa succede se rimuovo questa riga?"                 |
| Robustezza               | `mod_robust`              | "Su quali test si regge questa feature?"               |
| Pianificazione           | `mod_plan` / `mod_search` | Ordinare i passi di un fix                             |
| Sintesi                  | `mod_synth`               | Generare comandi shell da specifiche (già fatto!)      |
| Tool use                 | `mod_tool`                | Compilare domande in comandi + eseguire oracolo        |
| Agent loop               | `mod_agent`               | Perceive→decide→act→observe per task multi-step        |
| Induzione                | `mod_induce`/`mod_fewshot`| Imparare pattern di codice da esempi                   |
| Auto-correzione          | `mod_self` / gen103       | Correggere output quando arriva nuova informazione     |
| Memoria                  | `mod_memory`/`mod_world`  | Tenere contesto del task attraverso i turni            |
| Riparazione              | `mod_repair`              | Chiedere chiarimenti invece di fallire                 |
| Calibrazione             | `mod_calibrate`           | "Quanto sei sicuro di questa patch?"                   |
| KB persistente           | `kb/*.p0`          | Accumulare conoscenza di coding in file versionati     |

---

## 1. Gap architetturali — le fondamenta che mancano

### G1 — Filesystem reale
parrot0 non può leggere file arbitrari, scrivere file, listare directory.
Legge solo `kb/*.p0` all'avvio; scrive solo via `/save` nel file sessione.

**Impatto sul coding agent:** senza filesystem non può leggere codice sorgente,
scrivere patch, generare file di test, leggere log di compilazione.

### G2 — Esecuzione reale di processi
L'oracolo POSIX (`simulate_pipeline`) interpreta un subset di comandi in C
ma non lancia veri sottoprocessi. Non può compilare, eseguire test, lanciare
linter, misurare performance.

**Impatto sul coding agent:** senza exec non può verificare se il codice compila
o se i test passano. Il coding loop (edit→compile→test→iterate) è impossibile.

### G3 — Output multi-linea strutturato
`brain_respond()` scrive una singola riga in 8192 byte. Un coding agent deve
emettere blocchi di codice, diff, interi file.

**Impatto sul coding agent:** anche se sapesse generare codice, non potrebbe
restituirlo in un formato usabile. Ogni risposta è una riga.

### G4 — Nessun parsing di codice (AST)
Il riconoscimento di codice (`mod_symbolic`) si basa su pattern superficiali
(parentesi, keyword) per distinguere prosa da codice, ma non costruisce un AST.
Non tokenizza, non trova dichiarazioni, non fa type checking.

**Impatto sul coding agent:** senza AST non può ragionare sulla struttura del
codice. Può solo dire "this looks like code" senza capire cosa faccia.

### G5 — Loop single-turn
Il formato chat è domanda→risposta. `mod_repair` (gen141) ha già il pattern
clarify→store→resume per gap referenziali, ma non esiste un equivalente per
task di coding multi-turn.

**Impatto sul coding agent:** un task di coding richiede conversazioni
strutturate su più turni: "leggi il file X", "trova il bug", "genera la patch",
"testala", "itera". Oggi ogni turno è indipendente.

---

## 2. Gap di conoscenza — il dominio che manca

### G6 — Linguaggi di programmazione
Il KB (`kb/base.p0`) contiene solo `man(socrates)` e `mortal(X) :- man(X)`.
Non esiste conoscenza su C, Python, JavaScript, Bash, o alcun linguaggio.

**Cosa serve:** fatti e regole su:
- Sintassi: `keyword(c, if)`, `operator(c, +)`, `type(c, int)`, `delimiter(c, semicolon)`
- Semantica: `compiles_to(c, binary)`, `interpreted(python)`, `typed(c, static)`
- Librerie standard: `stdlib_function(c, printf)`, `header(c, stdio)`

### G7 — Algoritmi e strutture dati
Nessuna conoscenza di algoritmi classici (sort, search, graph), strutture dati
(array, list, tree, hash), notazione asintotica.

**Cosa serve:** `kb/algorithms.p0` con fatti come:
`algorithm(quicksort, sorting, nlogn)`, `data_structure(hash_table, lookup, o1)`.

### G8 — Tooling di sviluppo
Nessuna conoscenza di git, gcc, make, debugger, linter, test framework.

**Cosa serve:** estensione di `kb/bash.p0` ai tool di sviluppo:
`cmd(gcc, compiles_c_source)`, `flag(gcc, Wall, enables_all_warnings)`.

### G9 — Software engineering
Nessuna conoscenza di versionamento, code review, testing (unit/integration),
CI/CD, dependency management, pattern di progettazione.

**Cosa serve:** `kb/swe.p0` con concetti di processo software.

---

## 3. Gap di ragionamento — l'intelligenza applicata al codice

### G10 — Ragionamento spaziale sul codice
Il KB non ha il concetto di "posizione nel codice": numero di riga, colonna,
scope, blocco. Non può ragionare su "la riga 42 di brain.c".

**Cosa serve:** estendere il modello dei fatti con provenienza spaziale:
`at_line(declaration(main), 42)`, `in_scope(variable(x), function(main))`.

### G11 — Ragionamento causale sul codice
`mod_cause` esiste per relazioni causali generali, ma non è applicato al codice.
Non può ragionare su "questo bug è causato da quale modifica?" o "se cambio X,
quali test si rompono?".

**Cosa serve:** estendere `mod_cause` con relazioni specifiche del codice:
`calls(A, B)`, `depends_on(file, header)`, `test_covers(test, function)`.

### G12 — Modello predittivo dell'esecuzione
parrot0 non può simulare mentalmente l'esecuzione di codice. Non sa fare
"se eseguo questa funzione con input 5, restituirà 120". Questa è una capacità
fondamentale per debugging e code review.

**Cosa serve:** un interprete simbolico nel KB (o connesso all'oracolo) che
permetta di eseguire mentalmente piccoli frammenti, con fatti come:
`evaluates(expression(X), context(C), value(V))`.

### G13 — Pianificazione multi-step per task di coding
`mod_plan` sa ordinare `requires(cake, batter)` ma non sa decomporre
un obiettivo di coding in sotto-obiettivi. Manca il planning domain per:
1. Leggi bug report → 2. Trova file → 3. Identifica funzione → 4. Scrivi patch →
5. Compila → 6. Testa → 7. Se fallisce, torna a 3.

**Cosa serve:** un planning domain `code_task` che modelli il processo di
sviluppo software come grafo di dipendenze tra azioni.

---

## 4. Piano di evoluzione in fasi

### Fase A — Conoscenza di dominio (generazioni ~149–160, ~12 gen)

**Obiettivo:** popolare il KB con il dominio della programmazione così che il
motore inferenziale esistente possa iniziare a ragionare sul codice.

#### A1: `kb/c.p0` — il linguaggio C come fatti e regole
```
% Sintassi
keyword(c, if).         keyword(c, while).       keyword(c, return).
keyword(c, int).        keyword(c, void).         keyword(c, for).
operator(c, plus).      operator(c, minus).       operator(c, assign).
type(c, int).           type(c, char).            type(c, void).
delimiter(c, semicolon). delimiter(c, lbrace).    delimiter(c, rbrace).

% Libreria standard
stdlib_function(c, printf).   header(c, stdio).
stdlib_function(c, malloc).   header(c, stdlib).

% Semantica
compiles_to(c, binary).
paradigm(c, imperative).      paradigm(c, procedural).
typed(c, static).
```

**Test held-out:** "is `if` a keyword in C?" → Yes. "is `def` a keyword in C?" → No.
"what paradigm is C?" → procedural.

**Anti-impostor:** usare predicati mai visti nel training (linguaggi inventati
con proprietà note, verificare che il ragionamento sia strutturale).

#### A2: `kb/python.p0` — un secondo linguaggio per confronto
Stessa struttura di `c.p0`, così che `mod_compare` possa funzionare:
```
keyword(python, def).    keyword(python, if).      keyword(python, for).
type(python, int).       type(python, str).         type(python, list).
paradigm(python, multi_paradigm).
typed(python, dynamic).
interpreted(python).
```

**Test held-out:** "what is the difference between C and Python?" → risposta
derivata dai fatti nei due KB, non da una stringa pre-confezionata.

#### A3: `mod_code` — riconoscimento specifico del linguaggio
Evoluzione di `mod_symbolic`: non solo "this looks like code" ma "this is C code"
o "this is Python code", basato su features strutturali (keyword density,
syntax markers, comment style).

**Test held-out:** riconoscere C vs Python vs Bash vs prosa su snippet mai visti.

#### A4: `kb/algorithms.p0` — algoritmi classici
```
algorithm(bubble_sort, sorting, n2).
algorithm(quicksort, sorting, nlogn).
algorithm(merge_sort, sorting, nlogn).
algorithm(binary_search, search, logn).
algorithm(linear_search, search, n).
data_structure(array, access, o1).
data_structure(linked_list, insert, o1).
data_structure(hash_table, lookup, o1_amortized).

% Relazioni fra algoritmi
faster_than(quicksort, bubble_sort).
faster_than(binary_search, linear_search).
uses_data_structure(quicksort, array).
```

**Test held-out:** "which sorting algorithm is faster, quicksort or bubble_sort?"
→ quicksort. "what data structure does quicksort use?" → array.

#### A5: `kb/tools.p0` — tooling di sviluppo
Estensione del pattern `cmd/flag` di `bash.p0` ai tool di sviluppo:
```
cmd(gcc, compiles_c_source_code).
flag(gcc, Wall, enables_all_compiler_warnings).
flag(gcc, o, sets_output_file_name).
flag(gcc, g, includes_debug_information).
flag(gcc, O2, enables_optimization_level_two).

cmd(git, version_control_system).
flag(git, log, shows_commit_history).
flag(git, diff, shows_changes_between_versions).

cmd(make, build_automation).
cmd(gdb, debugger_for_compiled_programs).
```

**Test held-out:** "what flag enables warnings in gcc?" → `-Wall`. "compose a
command to compile with warnings and debug info" → `gcc -Wall -g`.

#### A6: `kb/swe.p0` — concetti di software engineering
```
concept(unit_test).         concept(integration_test).
concept(regression_test).   concept(code_review).
concept(continuous_integration).
concept(semantic_versioning).
concept(refactoring).

% Relazioni
catches(unit_test, regression).
improves(refactoring, maintainability).
enables(continuous_integration, early_bug_detection).
```

**Criterio di uscita Fase A:** parrot0 risponde correttamente a domande
fattuali su C, Python, algoritmi, tool e concetti SWE. Il KB è la fonte delle
risposte, non stringhe hard-coded. Almeno 3 test held-out per dominio.

---

### Fase B — Lettura e ragionamento sul codice (~161–175, ~15 gen)

**Obiettivo:** parrot0 legge codice sorgente reale, ne estrae la struttura,
e ragiona su di essa usando il motore inferenziale.

#### B1: `mod_fs` — filesystem module
Primo modulo che interagisce col filesystem reale:
- Input: `list files in src/`, `read file src/main.c`, `write file output.c <content>`
- Comandi interni: `/ls <path>`, `/cat <path>`, `/write <path> <content...>`
- Sandbox: solo path sotto la working directory
- I contenuti dei file letti entrano nel reader esistente (`mod_reader`) come fatti

**Design decision:** il filesystem è esposto come comandi speciali (`/ls`, `/cat`,
`/write`) per evitare di confondere il parser NL. I contenuti letti popolano
il KB con fatti come `file_line(path, N, content)`.

**Test held-out:** leggere un file C, rispondere a "quante righe ha?" usando
i fatti estratti.

#### B2: `mod_parse_code` — parsing strutturale di C
Costruisce un AST minimo da file C e lo carica nel KB:
```
% Da: int main(void) { return 0; }
function(main).           return_type(main, int).
function_has_body(main).  calls(main, return).
at_line(function(main), 1).

% Da: int x = 5;
variable(x).              variable_type(x, int).
variable_initialized(x).  at_line(variable(x), 2).
```

**Scope:** inizialmente solo C. Il parser è minimo ma sufficiente per funzioni,
variabili, chiamate, include, commenti. Non un compilatore completo.

**Test held-out:** dato un file C, "quante funzioni contiene?" e "quali variabili
sono dichiarate?" rispondono dai fatti nel KB.

#### B3: `mod_explain_code` — spiegare cosa fa una funzione
Usando l'AST nel KB + il motore inferenziale:
- "what does function main do?" → compone risposta da `calls(main, printf)` etc.
- "what variables does function X use?" → query sui fatti `variable` + `in_scope`
- "does function X call Y?" → query su `calls(X, Y)`

**Test held-out:** funzioni mai viste in training, spiegazione composta dai fatti
AST, non da template.

#### B4: Code reasoning — estendere `mod_cause` al codice
Fatti nel KB che modellano relazioni causali nel codice:
```
calls(main, printf).
depends_on(main, stdio_h).
test_covers(test_main, main).
```

Query supportate:
- "if I change function X, which tests should I run?" → `test_covers(T, X)`
- "which header does X depend on?" → `depends_on(X, H)`
- "what would break if I remove variable x?" → analisi di impatto via `mod_robust`

#### B5: Simulazione simbolica
Mini-interprete C nel KB per eseguire mentalmente funzioni semplici:
- Input: funzione + valori degli argomenti
- Output: valore di ritorno (o side effect) derivato dal KB
- Limite: solo operazioni aritmetiche e logiche, non puntatori/allocazione

```
% Regola: return di un valore letterale
evaluates(function(F), context([]), return_value(V)) :-
    return_statement(F, literal(V)).
```

**Test held-out:** "if I call factorial(5), what does it return?" → 120, calcolato
dall'interprete simbolico, non da esecuzione reale.

**Criterio di uscita Fase B:** parrot0 legge un file C reale (es. `src/main.c`),
ne estrae struttura (funzioni, variabili, chiamate), e risponde a domande
sul suo contenuto. Almeno 5 test held-out con file C mai visti prima.

---

### Fase C — Scrittura e azione (~176–195, ~20 gen)

**Obiettivo:** parrot0 genera codice, lo testa, e itera. Il coding loop completo.

#### C1: Output multi-linea
Estendere `brain_respond` (o aggiungere un canale parallelo) per supportare
blocchi di codice. Possibili approcci:
- (a) Formato markdown: ` ```c\n...\n``` ` nel buffer di risposta
- (b) Delimitatore speciale: `[[[CODE]]]...[[[END]]]`
- (c) Secondo canale di output per il codice

**Design decision (raccomandata):** markdown nel buffer esistente, con aumento
di `RESP_MAX_LEN`. I test case `.chat` già supportano soft-match su substring,
quindi blocchi di codice nelle risposte sono testabili.

#### C2: `mod_exec` — esecuzione reale di comandi
Sostituisce/affianca l'oracolo simulato con vera esecuzione di processi:
- Sandbox: timeout (5s), memoria limitata, no network, no fs fuori dal workspace
- Input: comando shell
- Output: exit code + stdout + stderr → fatti nel KB
- Comandi whitelist: `gcc`, `make`, `python3`, `grep`, `cat`, etc.

```
exec_result(gcc, exit_code, 0).
exec_stdout(gcc, "").
exec_stderr(gcc, "").
```

**Sicurezza:** whitelist di comandi permessi, sandboxing con `timeout` + `ulimit`,
nessuna variabile d'ambiente sensibile passata al sottoprocesso.

#### C3: Code generation — `mod_gencode`
Il duale di `mod_synth` per il codice C: da una specifica naturale a una funzione C.
Usa l'AST del KB come template e compone:
- "write a function that checks if a number is prime" → genera `int is_prime(int n) { ... }`
- "write a function that sorts an array of ints" → genera `void sort(int arr[], int n) { ... }`

**Meccanismo:** pattern matching tra specifica e algoritmi nel KB +
composizione di blocchi sintattici da `kb/c.p0`. Non un LLM: è
deterministico, grounded nel KB, limitato al dominio conosciuto.

**Anti-impostor:** la funzione generata deve compilare e passare test held-out.
Il generatore non può contenere funzioni pre-scritte; deve comporre da parti.

#### C4: `mod_debug` — analisi e fix di bug
Dato un frammento di codice e un errore (di compilazione o logico):
1. Classifica l'errore (sintassi, tipo, logico, memoria)
2. Propone una fix
3. La fix è grounded nel KB di sintassi/semantica del linguaggio

Esempi:
- `int x = "hello"` → "type mismatch: cannot assign string to int"
- `for (int i = 0; i < 10; i++) { print(i); }` → "error: 'print' is not a standard C function. Did you mean 'printf'?"

**Meccanismo:** pattern di errore nel KB + abduzione (`mod_abduce`) per inferire
la fix minima. "Cosa devo cambiare perché compili?" → abduce la modifica.

#### C5: `mod_coding_loop` — il ciclo completo
Estensione di `mod_agent` (gen116–120) al dominio del codice:
1. **Perceive:** leggi il task ("fix the bug in src/main.c line 42")
2. **Decide:** scegli l'azione (leggi file, cerca funzione, genera fix)
3. **Act:** esegui l'azione (usa `mod_fs`, `mod_exec`, `mod_gencode`)
4. **Observe:** verifica il risultato (compila, testa)
5. **Iterate:** se fallisce, torna a 2 con il nuovo stato

```
code_task_state(bug_fix, step, reading_file).
code_task_state(bug_fix, file, src/main.c).
code_task_state(bug_fix, line, 42).
code_task_state(bug_fix, error, type_mismatch).
code_task_state(bug_fix, proposed_fix, change_int_to_char).
```

**Criterio di uscita Fase C:** parrot0 prende un bug report in linguaggio
naturale, trova il file, identifica il problema, propone una fix, la verifica
compilando, e itera se necessario. Almeno 3 scenari held-out end-to-end.

---

### Fase D — Integrazione e scaling (~196–210, ~15 gen)

**Obiettivo:** comporre tutte le capacità in scenari complessi, multi-file,
multi-linguaggio. parrot0 come coding agent completo.

#### D1: Multi-file awareness
Il coding loop opera su progetti con più file:
- `#include` risolti nel filesystem
- Dipendenze tra file modellate nel KB
- Modifiche a un file triggerano ricompilazione dei dipendenti

#### D2: Test generation
Dato un file sorgente, genera test unitari:
- Estrae le funzioni pubbliche (da AST nel KB)
- Per ogni funzione, genera casi di test (da pattern nel KB + induzione)
- Esegue i test e riporta la coverage

```
generate_test(function(F)) :- public_function(F), test_template(F, T).
```

#### D3: Code review
Dato un diff o un file, produce una review strutturata:
- Style issues (da regole nel KB)
- Potenziali bug (da pattern di errore nel KB)
- Suggerimenti di refactoring (da pattern idiomatici nel KB)

#### D4: Git integration
Comandi git esposti come primitive nel KB:
- `/git log` → lista commit → fatti nel KB
- `/git diff` → cambiamenti → analizzabili dal code reasoner
- `/git blame` → autore di ogni riga → contesto per il debug

#### D5: Multi-language support
Estendere il code parser (B2) e il code generator (C3) a Python e Bash.
Il design a moduli lo permette: `mod_parse_code` dispatcia al parser corretto
in base al linguaggio rilevato da `mod_code` (A3).

**Criterio di uscita Fase D:** parrot0 opera su un progetto multi-file C,
legge il codice, capisce le dipendenze, suggerisce miglioramenti, scrive test,
e usa git per navigare la history. Almeno 2 scenari held-out end-to-end su
progetti open source semplici.

---

### Fase E — Reflexive closure (~211–218, ~8 gen)

**Obiettivo:** parrot0 capisce e modifica se stesso. Il loop di LOOP.md diventa
parzialmente interno.

#### E1: Self-hosting
parrot0 carica il proprio `src/brain.c` nel KB, ne estrae l'AST, e può:
- Rispondere a domande sulla propria implementazione ("come funziona mod_arith?")
- Identificare pattern nel proprio codice (es. "quanti moduli ho?")
- Suggerire miglioramenti basati sui pattern che riconosce

#### E2: Self-modification proposta
parrot0 non modifica i file da solo (il loop esterno mantiene il controllo),
ma può proporre modifiche grounded:
- "per aggiungere la capacità X, suggerisco di modificare Y in brain.c:Z"
- La proposta è derivata dall'analisi del proprio AST + abduzione sul task
- Il loop esterno valuta e applica (o rifiuta) la proposta

#### E3: Self-test
parrot0 esegue `make test` su se stesso dopo una modifica proposta:
- Interpreta i risultati (quali test passano/falliscono)
- Se un test fallisce, analizza il motivo (il test aspettava X, ha ottenuto Y)
- Propone aggiustamenti

**Criterio di uscita Fase E:** LOOP.md step 4 (self-challenge parity) passa da
"proposta generica del loop shape" a "proposta specifica grounded nell'AST di
brain.c, con la modifica esatta, il test case, e la previsione di impatto sugli
altri moduli".

---

## 5. Metriche di progresso per il coding agent

### M-C1 — Code knowledge coverage
Percentuale di domande fattuali su C/Python/algoritmi/tool con risposta corretta
e grounded nel KB (non in template). Target: >90%.

### M-C2 — Code understanding
Dato un file C mai visto, percentuale di funzioni/variabili/chiamate correttamente
estratte nell'AST. Target: >80% per codice C89/C99 semplice.

### M-C3 — Bug detection
Dato un set di bug iniettati in funzioni C, percentuale di bug correttamente
identificati e spiegati. Target: >70%.

### M-C4 — Fix generation
Dato un set di bug con errore di compilazione noto, percentuale di fix che
compilano al primo tentativo. Target: >60%.

### M-C5 — End-to-end coding task
Dato un task di coding in linguaggio naturale ("aggiungi una funzione che..."),
percentuale di task completati con successo (codice compila + test passano).
Target: >50% per task semplici (una funzione, <20 righe).

### M-C6 — Reflexive understanding
Percentuale di domande sulla propria architettura con risposta corretta e
grounded nell'AST di brain.c. Target: >80%.

---

## 6. Principi e disciplina per questa evoluzione

### 6.1 Ogni fase produce un test held-out
Nessuna capacità è dichiarata acquisita senza un test che la dimostri su input
mai visti in fase di sviluppo. Il test deve essere eseguibile (`make test`
deve includerlo).

### 6.2 Bilingual ratchet o equivalente strutturale
Per le capacità di codice, il ratchet bilingue (EN/IT) si applica al linguaggio
naturale di interfaccia (i comandi, le domande, le spiegazioni). Per il codice
stesso, il ratchet è cross-linguaggio: una capacità dimostrata in C deve
trasferire a Python attraverso lo stesso meccanismo strutturale, non con un
secondo implementation path.

### 6.3 Anti-impostor specifico per il coding
- Nessuna funzione generata può essere pre-scritta; deve essere composta da parti
- I file di test devono essere generati proceduralmente con nomi/predicati freschi
- Il parse di codice deve funzionare su file mai visti, non solo sugli esempi
- La conoscenza di algoritmi deve trasferire a varianti non esplicitamente
  insegnate (es. se conosce bubble_sort e quicksort, deve poter confrontare
  merge_sort vs heap_sort per analogia strutturale)

### 6.4 Mantenere la purezza C
Nessun LLM a runtime. Nessuna network call per generare codice. Tutta la
generazione deve essere deterministica, grounded nel KB, ispezionabile.

### 6.5 Ogni generazione è un passo reversibile
Il loop di LOOP.md rimane il processo. Ogni nuova capacità è una modifica
minima a `brain.c`, testata, journalizzata, committata.

---

## 7. Dipendenze tra le fasi

```
A1 (kb/c.p0)
 ├── A2 (kb/python.p0)
 ├── A3 (mod_code)
 ├── A4 (kb/algorithms.p0)
 ├── A5 (kb/tools.p0)
 └── A6 (kb/swe.p0)
      │
      ▼
B1 (mod_fs) ─────────────────────────────┐
 ├── B2 (mod_parse_code) ──────────────┐ │
 │    ├── B3 (mod_explain_code)        │ │
 │    ├── B4 (code reasoning)          │ │
 │    └── B5 (simulazione simbolica)   │ │
 │                                     │ │
 │    ┌────────────────────────────────┘ │
 │    ▼                                  ▼
 ├── C1 (output multi-linea) ◄───────────┘
 ├── C2 (mod_exec)
 ├── C3 (mod_gencode) ──┐
 ├── C4 (mod_debug) ────┤
 └── C5 (mod_coding_loop) ◄── integra tutte le C
      │
      ▼
D1 (multi-file) ──┬── D2 (test gen) ──┬── D4 (git) ──┬── D5 (multi-lang)
                  └── D3 (code review)┘               │
      │                                               │
      └───────────────────────────────────────────────┘
      │
      ▼
E1 (self-hosting) ──► E2 (self-modification) ──► E3 (self-test)
```

---

## 8. Stima effort e rischi

### Effort stimato
- **Fase A** (conoscenza): ~12 generazioni, relativamente meccanico (popolare KB,
  aggiungere `mod_code`)
- **Fase B** (comprensione): ~15 generazioni, il code parser è la parte più
  complessa (scrivere un parser C in C è non banale)
- **Fase C** (azione): ~20 generazioni, l'esecuzione reale introduce rischi di
  sicurezza, il coding loop è un'integrazione complessa
- **Fase D** (scaling): ~15 generazioni, test generation e code review sono
  i pezzi più aperti
- **Fase E** (reflexive): ~8 generazioni, il self-hosting è il momento
  asintotico del progetto
- **Totale: ~70 generazioni** (da gen149 a gen218 circa)

### Rischi principali
1. **Complessità del code parser (B2):** un parser C anche minimo è un progetto
   significativo. Rischio di blocco se si cerca di coprire troppo. Mitigazione:
   iniziare con un subset minimo (funzioni, variabili, chiamate) ed espandere
   incrementalmente.
2. **Sicurezza dell'esecuzione (C2):** eseguire codice generato è pericoloso.
   Mitigazione: sandbox rigoroso, whitelist, timeout, nessun accesso di rete,
   esecuzione in container/cgroup.
3. **Deriva da "emergence over design" (tutto il piano):** questo piano è
   intrinsecamente più design-driven dei task precedenti perché il dominio
   "coding agent" richiede molta infrastruttura prima di mostrare comportamenti
   emergenti. Mitigazione: ogni mini-step deve essere tirato da un test concreto;
   se un pezzo di infrastruttura non ha un test che lo giustifichi, non si
   costruisce (pivot duty di DESIGN.md D5.1).
4. **Phrasebook risk nella code generation (C3):** il generatore di codice
   potrebbe degenerare in template. Mitigazione: test held-out con specifiche
   mai viste; la composizionalità deve essere dimostrata (due funzioni generate
   indipendentemente devono comporre).

---

## 9. Relazione con gli obiettivi esistenti

Questo piano NON sostituisce gli obiettivi correnti in `TASKLIST.md` e `TASK.md`.
È un **ramo parallelo** che parte quando le fondamenta conversazionali (C-series,
E-series) sono sufficientemente solide.

**Prima di iniziare la Fase A, è necessario:**
- Completare i task conversazionali rimanenti (C-series) così che l'interfaccia
  utente sia sufficientemente naturale per un coding agent
- Portare il long-chat benchmark a >90% felt landing e <5% wall rate
- Avere il multi-file knowledge loading (D1 di DESIGN.md menziona file multipli)

**Il momento giusto per iniziare:** quando il long-chat benchmark mostra che
parrot0 può sostenere una conversazione tecnica multi-turn senza collassare.
Stimato: ~gen155–160, dopo aver completato C7–C14.

---

## 10. Il giudizio sintetico

parrot0 gen148 è un **ragionatore simbolico senza dominio e senza mani**.
La sua architettura (moduli + KB + motore inferenziale) è esattamente il
substrato giusto per un coding agent. Manca:

1. **Il dominio** — conoscenza di linguaggi, algoritmi, pattern, tool (~12 gen)
2. **Le mani** — filesystem reale, esecuzione reale, output multi-linea (~15 gen)
3. **Il wiring** — connettere i moduli esistenti in un coding loop integrato (~20 gen)
4. **Lo scaling** — multi-file, multi-linguaggio, test generation (~15 gen)
5. **La chiusura** — self-hosting e self-modification (~8 gen)

Il gap non è di "intelligenza" nel senso di capacità di ragionamento astratto —
quella c'è ed è sorprendentemente sofisticata (abduzione, controfattuali,
planning, induzione, auto-perturbazione). Il gap è di **conoscenza di dominio**
e **capacità di azione nel mondo** (filesystem + processi). Entrambi sono gap
colmabili con l'approccio esistente: un modulo alla volta, testato, bilingue,
anti-impostor, dentro il loop di LOOP.md.

La domanda aperta è se l'emergenza di capacità di coding da questo substrato
— senza un LLM a runtime — sia possibile al livello di competenza di un coding
agent moderno, o se invece il progetto dimostrerà asintoticamente che alcune
capacità richiedono un modello statistico. In entrambi i casi, il valore
scientifico dell'esperimento è nel *dove* e *perché* l'approccio simbolico
puro si ferma — esattamente ciò che PRINCIPLES.md si propone di scoprire.
