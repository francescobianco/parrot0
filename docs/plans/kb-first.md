# Manifesto KB-first

*La bussola con cui decidiamo ogni evoluzione di parrot0: se una capacità è
struttura logica derivabile (e allora la si costruisce, senza LLM) oppure no.*

Questo documento non è un piano una-tantum: è un **metodo riusabile**. È nato da un
disaccordo concreto (la metafora del cerchio, §6) e ne ha estratto una procedura che
applichiamo a ogni nuova domanda che parrot0 oggi mura. Quando in futuro chiediamo
"questa capacità è dentro o fuori la portata onesta di parrot0?", si torna qui.

> **Esempio vivo del metodo (gen311):** [[teachable-procedures]] §6 racconta un caso
> completo — scoperta di conoscenza estesa CABLATA nel C (i costrutti grammaticali),
> astrazione prima delle FORME (tabelle→fatti), poi delle PROCEDURE (regole+interprete
> di riscrittura). È il modello di ragionamento KB-first applicato fino alle procedure:
> *la conoscenza non è solo fatti, ma anche procedure per trasformare i fatti.*

---

## 1. La tesi fondante

**Lingua e codice/logica abitano la stessa struttura elaborativa.** parrot0 ricostruisce
per via comportamentale la struttura del ragionamento di un LLM, ma *senza* un LLM a
runtime: motore simbolico fisso, conoscenza che cresce. Da ciò discende il corollario che
guida tutto:

> Una frase in linguaggio naturale non è "roba da rete neurale" solo perché è espressa
> come metafora, analogia o aforisma. Sotto la superficie c'è un'**anima logica**: simboli
> semantici sofisticati ma **derivabili**. Il nostro lavoro è trovarla e cablarla.

Scartare una frase come "fuori portata, è generativa" è lecito **solo** dopo aver provato
a derivarne lo scheletro logico e aver dimostrato che non c'è. Quasi sempre c'è.

## 2. Il principio operativo

- **Engine fixed, knowledge learns.** Il C contiene *motori* (unificazione, risoluzione
  SLD, abduzione, chooser, chiusure qualitative), non *frasi*. Ogni forma di superficie
  che parrot0 **riconosce** o **produce** vive nella KB (`intent_cue`, `response_template`,
  fatti, regole), mai come `printf` cablato. Aggiungere un fatto deve estendere una
  capacità senza toccare il C.
- **No impostor.** Un `printf("...")` che simula conoscenza è l'impostore che i PRINCIPLES
  rifiutano. La conoscenza è un fatto interrogabile e confermabile.
- **Substrato vs conoscenza dell'utente.** I fatti curati che spediamo (geografia, colori,
  monotonie, …) sono *substrato*: vanno marcati in `is_internal_pred` (conteggio/ dump) e
  in `is_struct_pred` di `kb.c` (descrizione), così non inquinano "quante cose sai?".
- **Nessuna lista di parole nel C (lo smell test decisivo).** Qualunque *elenco letterale
  di parole di superficie* nel C che faccia da trigger, cue, sinonimo, filtro o
  classificatore — i verbi che marcano un intento, i filler, i sinonimi, le parole-gate —
  **è conoscenza, non un motore**, e va nella KB come fatti (enumerati con `kb_match` /
  `kb_cue_match`). La domanda che decide, da porsi PRIMA di scrivere l'elenco:
  > *"parrot0 potrebbe imparare un nuovo membro di questa lista domani, senza ricompilare?"*
  Se sì → KB. La spia inequivocabile è una catena
  `strstr(norm,"form") || strstr(norm,"happen") || strstr(norm,"occur") || …`: quella lista
  è un fatto `causal_process_verb/1` (o `intent_cue`, `verb_syn`, …) mascherato da C. Il C
  tiene solo *come* si cerca (il ciclo di match); *cosa* si cerca vive nella KB. Regola
  gemella di "Engine fixed, knowledge learns" applicata al riconoscimento dell'input.

## 3. Il test: deduci · abduci · genera

Per **ogni** nuova domanda che parrot0 mura, si applica questa procedura.

**Passo 1 — Decomponi la superficie nello scheletro.** Quali simboli? quali relazioni?
che *tipo di conclusione* serve (sì/no, un valore, una scelta A/B, una *variazione di
grandezza*)?

**Passo 2 — Classifica la mappa fra superficie e struttura:**

| Classe | Quando | Mossa |
|---|---|---|
| **Deduci** | la struttura e la mappa sono note o derivabili | catena di regole + risoluzione → **costruisci** |
| **Abduci** | la struttura c'è, ma la mappa va *ipotizzata/scelta* (metafora nuova) | abduzione + oracolo di plausibilità → **costruisci** (più costoso/incerto) |
| **Genera** | serve produzione linguistica aperta senza struttura verificabile (poesia, prosa originale) | **soffitto onesto**: ammetti, non fingere |

Solo la terza classe è il limite no-LLM. Le prime due sono territorio di parrot0. La
distinzione vera **non** è "simbolico vs LLM": è **"mappa nota (deduci) vs mappa da
abdurre (cerca) vs generazione irriducibile (ammetti)"**.

**Passo 3 — Inventario degli organi.** Quali pezzi del motore servono e quali già esistono?
`kb_assert_rule` + SLD (catene), `mod_analogy` (mappe a:b::c:d), `mod_abduce` (scelta della
mappa), chooser binario/COPA (decisione A/B), chiusure in C dove le regole unarie non
bastano (relazioni binarie/transitivе).

**Passo 4 — Costruisci il più piccolo incremento** che generalizza (mai 1 frase = 1
`printf`), con un **bench oracolare** che misura se la struttura regge sotto pressione.

**Passo 5 — Onestà (sotto).**

## 4. Il pattern di costruzione

Per una capacità "deduci"/"abduci" il cablaggio ha quasi sempre questa forma:

1. **Parse** della forma di superficie → estrai i simboli (soggetto, relazione, feature,
   azione, …). Robusto a frasi parziali; declina se non riconosce.
2. **Fatti KB** che reificano la conoscenza necessaria (relazioni, monotonie, azioni) —
   conoscenza, non codice.
3. **Resolver**: SLD per le catene unarie; una chiusura in C sopra i fatti per le relazioni
   binarie/transitive che il motore unario non porta.
4. **Decisione**: il chooser collassa su una risposta fra le alternative ammesse.
5. **Verbalizzazione**: la risposta esce da un `response_template` (forma nella KB), con gli
   slot riempiti.
6. **Bench**: un `make …-bench` con pochi casi noti; studia il comportamento, non cuce frasi.

## 5. Le regole di onestà (non negoziabili)

- **Declina quando la catena non regge.** Se manca un fatto o un anello, parrot0 ammette
  ("non lo so / non capisco ancora"), non inventa. Esempio: la metafora del cerchio risponde
  "cresce" per *learn*, ma mura onestamente per *sleep* (nessun `increases(sleep, knowledge)`).
- **Quando la nuova conoscenza rende rispondibile un test di umiltà, sposta il test, non
  falsificare il muro.** Aggiungere `because(sky_blue, …)` ha reso rispondibile "perché il
  cielo è blu?"; il test di umiltà è stato ripuntato su "perché il cielo è verde?" (ancora
  ignoto), non finto. Stessa disciplina della collisione `capital`.
- **Non spacciare generazione per conoscenza.** Una poesia o una continuazione di prosa
  *senza* struttura verificabile resta soffitto: si ammette il limite.

## 6. Caso lavorato — la metafora del cerchio (IMPLEMENTATO, gen233)

**Domanda:** *"If knowledge is like a circle, what happens to its circumference when you
learn something new?"*

**Diagnosi iniziale (sbagliata):** "è metafora/generativo, errore di categoria, lasciala
murare". **Correzione** (dal dialogo in appendice): contraddiceva la tesi fondante. La frase
*ha* una catena logica derivabile.

**Decomposizione (Passo 1–2 → Deduci):**
- mappa (analogia): `conoscenza ↦ area`, `circonferenza ↦ frontiera col non-noto`;
- catena: `cresce(area) :- impari` → `cresce(raggio) :- cresce(area)` → `cresce(circonferenza)
  :- cresce(raggio)` (C = 2πr, monotòna);
- decisione: scelta su `{cresce, diminuisce, invariata}` → il `respond` decide A vs B.
  Esattamente il se-allora: *implica* nel `then`, *non-implica* nell'`else`.

**Cablaggio (gen233):**
- `kb/core/world-facts.p0`: fatti `grows_with(Feature, Base)` (co-monotonia: circonferenza,
  area, raggio… del cerchio) e `increases(Action, Source)` (learn/study/read → knowledge).
- `src/brain/10-memory-knowledge.c`: `qchain_reaches()` = chiusura transitiva in C su
  `grows_with/2` (la binaria che il motore unario non porta); un riconoscitore in
  `mod_knowledge` estrae *source/target/feature/action* da "X is like Y … happens to …
  when …" e conclude: se la feature è co-monotòna col target **e** l'azione incrementa il
  source → **cresce**.
- `grows_with`/`increases` marcati substrato (`is_internal_pred` + `is_struct_pred`).

**Comportamento:**
```
if knowledge is like a circle, what happens to its circumference when you learn something new?
  → It grows: more knowledge makes a bigger circle, so its circumference grows with it.
… when you sleep        → I don't understand that yet.   (nessun increases(sleep, knowledge): muro onesto)
```

**Cosa resta aperto (onestà):** la *selezione* della mappa per metafore **inedite** è
abduzione (Passo 2, classe "abduci") — ricerca + oracolo di plausibilità, ancora simbolica,
non LLM. Per le metafore convenzionali la mappa è un fatto KB e la catena parte subito.

---

## Appendice — il dialogo che ha generato il metodo

*Conservato perché il metodo è nato qui: un disaccordo che ha costretto a cercare l'anima
logica invece di murare.*

**F. (obiezione):** «proprio perché è una frase e siamo nel regime in cui linguaggio e
codice sono la stessa struttura elaborativa, questa per me non è altro che codice logico; la
metafora non è altro che uno strumento cognitivo di elaborazione […] simboli semantici
sofisticati ma derivabili. […] il se-allora potrebbe ospitare questa frase nel concetto di
*implica* e *non implica* nell'else, e poi il respond prende una decisione tra A e B.»

**Risposta (sintesi):** hai ragione, e correggo. Il mio "errore di categoria / lasciala
murare" contraddiceva il wager fondante. La frase ha una catena dimostrabile
(analogia → catena di regole monotòne → decisione A/B); i pezzi (`kb_assert_rule`+SLD,
`mod_analogy`, `mod_abduce`, chooser gen228/COPA) esistono già. La parte dura non è una
facoltà irriducibilmente-LLM: è (a) il *parse* di "X è come Y" in una mappa e (b) il *motore
qualitativo* del cambiamento — entrambi dentro il perimetro simbolico. Da **costruire**, non
da scartare. → fatto in gen233 (§6).

*Questa è la lezione del manifesto: prima di dire "serve un LLM", decomponi e prova a
derivare. Il più delle volte, la metafora è codice in attesa di essere letto.*
