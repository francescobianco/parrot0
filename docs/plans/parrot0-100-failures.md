# I 100 Prompt A Cui Parrot0 Non Sa Rispondere

**Data delle prove:** 2026-08-18  
**Metodo:** tre batterie interattive eseguite con `make chat`  
**Build osservata:** `gen396-universal-answer-plan@0eb4626`  
**Profilo:** acquire, strumenti e rete attivi

## Disclaimer metodologico

Questa non e' una lista di cento prompt scelti a posteriori per far apparire
male parrot0. Sono cento casi distinti selezionati da tre batterie di prove
eseguite in sequenza. Un caso e' stato incluso quando la risposta era:

- un muro esplicito su un intento ordinario;
- una risposta fuori dominio;
- un template generico che non istanziava il problema;
- un artefatto rifiutato per schema non verificato;
- una risposta semanticamente non pertinente.

Non considero invece fallimento il rifiuto prudente di inventare un fatto quando
la richiesta non fornisce dati sufficienti. Qui sono inclusi soprattutto i casi
in cui il sistema avrebbe dovuto ragionare, chiedere chiarimenti o dichiarare
precisamente il limite, ma ha risposto con un muro generico o con un'altra
facolta'.

Le risposte sono abbreviate per leggibilita', ma la classificazione deriva dal
transcript reale. La prova va ripetuta dopo ogni release per distinguere
regressioni da miglioramenti.

## Legenda

| Codice | Significato |
|---|---|
| `MURO` | Non comprende o chiede di imparare un termine senza affrontare il compito |
| `FUORI` | Risposta estranea, spesso un template causale o progettuale |
| `SCHEMA` | Riconosce la richiesta ma non possiede uno schema verificato per produrre l'artefatto |

## ⛔ TODO — stato di chiusura (aggiornato 2026-08-18)

CHIUSE come CLASSI, non come casi (ratchet in `tests/p0t/reasoning/`):

- famiglie numeriche — confronto, ordinamento, resto, mediana (#1, #2, #3, #5):
  `numeric_questions.p0t`. Il ponte e' generico: una domanda numerica nuova e'
  una cue piu' una procedura, e il test lo prova aggiungendone una a runtime;
- contesto dichiarato («siamo in spiaggia»): `described_situation.p0t`.

APERTE, e vanno chiuse come classi allo stesso modo:

- logica formale: contrapposizione, affermazione del conseguente, sillogismi con
  quantificatori misti (#6, #7, #8, #9);
- meta-domande sul sapere: che cosa manca per rispondere, come si sa di non
  sapere (#11, #13, #14);
- salienza dentro un testo o un log (#15, #16, #17);
- trasferimento di pattern fra domini (#18, #19);
- stipulazione su un SIMBOLO — «immagina che 2 vale 3 quanto fa 2+2» — che
  chiede una sostituzione dentro l'aritmetica e non soltanto un mondo ipotetico.
  La sonda `tests/failure_modes_probe.py` ne ha registrato la mossa
  dell'oracolo: «Se 2 vale 3, allora 2 + 2 vale 3 + 3 = 6».

Il metodo resta quello: la sonda scopre la MOSSA, la KB la riproduce come regola,
il ratchet la tiene ferma. Chiudere un prompt senza chiudere la sua classe non
conta come progresso.

## Elenco dei fallimenti

| # | Prompt | Esito osservato |
|---:|---|---|
| 1 | Which is greater, 3.14 or 3.41? | `MURO`: I don't know about greater yet |
| 2 | Sort these numbers: 8, 2, 11, 4. | `MURO`: I don't know about numbers yet |
| 3 | What is the remainder of 29 divided by 5? | `MURO`: I don't know about remainder yet |
| 4 | A train leaves at 14:30 and travels for 2 hours 45 minutes. When does it arrive? | `MURO`: I don't know about travels yet |
| 5 | What is the median of 1, 4, 9, 10, 20? | `MURO`: I don't know about median yet |
| 6 | If it rains then the ground is wet. The ground is wet. Did it necessarily rain? | `MURO`: I don't know about ground yet |
| 7 | If all doctors are scientists and some scientists are artists, are all doctors artists? | `MURO`: I don't know about doctors yet |
| 8 | What is the contrapositive of if it rains then the ground is wet? | `MURO`: I don't know about ground yet |
| 9 | Can two contradictory claims both be true in the same sense? | `MURO`: I don't know about contradictory yet |
| 10 | Explain a counterexample to every swan is white. | `FUORI`: generic causal-account template |
| 11 | What information is missing before comparing the populations of two cities? | `MURO`: I don't know about missing yet |
| 12 | Break the question why cities sink into a research plan. | `FUORI`: generic workable-design template |
| 13 | What should you search for when you do not know which variables matter? | `FUORI`: generic recommendation template |
| 14 | How do you know that you do not know something? | `FUORI`: generic causal-account template |
| 15 | Which line matters most in this log: INFO ready, WARN retry, ERROR database refused? | `MURO`: I don't know about matters yet |
| 16 | Why is one line in a long report more salient than another? | `MURO`: I don't know about report yet |
| 17 | What does goal-conditioned salience mean? | `MURO`: I don't know about goal-conditioned yet |
| 18 | Can a deadlock pattern transfer from processes to people? | `FUORI`: generic causal-account template |
| 19 | What remains when domain-specific nouns are removed from an analogy? | `MURO`: I don't know about remains yet |
| 20 | Compare two graphs structurally. | `MURO`: I don't know about compare yet |
| 21 | Every Tar owns two Vels. What representation should you build? | `FUORI`: generic workable-design template |
| 22 | How do you learn a new relation from three examples? | `FUORI`: generic causal-account template |
| 23 | Forget my name. | `MURO`: I don't know about forget yet |
| 24 | I am worried. What should you say? | `FUORI`: ungrounded recommendation response |
| 25 | How do you keep a dialogue goal across ten turns? | `FUORI`: generic causal-account template |
| 26 | What does this refer to in a follow-up question? | `MURO`: I don't know about follow-up yet |
| 27 | What was the first topic we discussed? | `MURO`: I don't know about discussed yet |
| 28 | Translate the dog runs into Spanish. | `MURO`: I don't know about translate yet |
| 29 | Correct this sentence: She go to school. | `MURO`: I don't know about correct yet |
| 30 | Is Me likes apples grammatical? | `MURO`: I don't know about grammatical yet |
| 31 | Give two reasons to verify sources. | `MURO`: I don't know about reasons yet |
| 32 | Return valid JSON with keys name and value. | `MURO`: I don't know about return yet |
| 33 | Invent ten funny names for a parrot database. | `MURO`: I don't know about invent yet |
| 34 | Continue this story with the same characters and setting. | `MURO`: I don't know about continue yet |
| 35 | Change the tone of this story from playful to tragic. | `MURO`: I don't know about playful yet |
| 36 | Give three genuinely different solutions to a waiting problem. | `MURO`: I don't know about genuinely yet |
| 37 | Now add the constraint that I have no kettle. | `MURO`: I don't know about kettle yet |
| 38 | Which is safer, speed or caution in an unknown situation? | `MURO`: I don't know about caution yet |
| 39 | What should happen before a destructive file operation? | `FUORI`: generic recommendation template |
| 40 | Why did a function return early? | `MURO`: I don't know about return yet |
| 41 | What should an agent do when a tool is unavailable? | `FUORI`: generic recommendation template |
| 42 | Solve the equation 3x plus 5 equals 20. | `MURO`: I don't know about equals yet |
| 43 | What is the least common multiple of 12 and 18? | `MURO`: I don't know about common yet |
| 44 | What is the greatest common divisor of 84 and 126? | `MURO`: I don't know about greatest yet |
| 45 | Explain Bayes theorem with a numerical example. | `FUORI`: generic causal-account template |
| 46 | Explain the difference between necessary and sufficient conditions. | `MURO`: no contrast fact for Necessary/Sufficient |
| 47 | Is the following argument valid: if P then Q, Q, therefore P? | `MURO`: I don't know about following yet |
| 48 | Give a countermodel to this claim. | `MURO`: I don't know about countermodel yet |
| 49 | Explain temporal causality with an example. | `FUORI`: generic causal-account template |
| 50 | Distinguish induction from abduction. | `MURO`: I don't know about distinguish yet |
| 51 | What is the causal mechanism in this experiment? | `MURO`: I don't know about causal yet |
| 52 | How should a system rank conflicting evidence? | `FUORI`: generic recommendation template |
| 53 | What is epistemic injustice? | `MURO`: I don't know about epistemic yet |
| 54 | Compare utilitarianism and deontology. | `MURO`: I don't know about compare yet |
| 55 | Explain confounding with a concrete dataset. | `FUORI`: generic causal-account template |
| 56 | How should missing data be handled? | `MURO`: I don't know about missing yet |
| 57 | How do you separate observation from interpretation? | `FUORI`: generic causal-account template |
| 58 | Create an ontology for a fictional ecosystem. | `FUORI`: starts with an encyclopedia definition of ecosystem, not an ontology |
| 59 | Invent rules for a language with no known words. | `MURO`: I don't know about invent yet |
| 60 | Infer the grammar from these examples: ko ta, ko mi, la ta. | `MURO`: I don't know about grammar yet |
| 61 | Build a model of a world with changing states. | `FUORI`: generic workable-design template |
| 62 | How do you represent ownership over time? | `FUORI`: generic causal-account template |
| 63 | How do you handle an object with two roles? | `FUORI`: generic causal-account template |
| 64 | What is schema induction from sparse examples? | `MURO`: I don't know about schema yet |
| 65 | How do you retract one inferred consequence? | `FUORI`: generic causal-account template |
| 66 | Learn a new question form from this dialogue. | `MURO`: I don't know about question yet |
| 67 | What should happen after ablation of a cue? | `FUORI`: generic recommendation template |
| 68 | How do you prevent a learned fact from contaminating another session? | `FUORI`: generic workable-design template |
| 69 | What is provenance for a derived fact? | `MURO`: I don't know about provenance yet |
| 70 | How do you resolve a contradiction in the KB? | `FUORI`: generic causal-account template |
| 71 | How do you rank two equally supported interpretations? | `FUORI`: generic causal-account template |
| 72 | How do you represent a user preference with time validity? | `FUORI`: generic causal-account template |
| 73 | Resolve the pronoun in: Anna told Maria that she was late. | `MURO`: I don't know about resolve yet |
| 74 | Track the topic after two unrelated interruptions. | `MURO`: I don't know about unrelated yet |
| 75 | Continue a plan after the user changes the objective. | `FUORI`: generic workable-design template |
| 76 | Plan a trip with budget, dates, accessibility, and weather constraints. | `FUORI`: generic workable-design template |
| 77 | Replan after the train is cancelled. | `MURO`: I don't know about replan yet |
| 78 | Give a risk matrix with likelihood and impact. | `MURO`: I don't know about likelihood yet |
| 79 | Write a decision record with alternatives and rejected options. | `SCHEMA`: no verified schema for the artifact |
| 80 | Generate a valid YAML document with nested fields. | `SCHEMA`: no verified schema for the artifact |
| 81 | Return a CSV with headers and three rows. | `MURO`: I don't know about return yet |
| 82 | Write a regular expression for an email address. | `SCHEMA`: no verified schema for the artifact |
| 83 | Explain this stack trace and identify the root cause. | `FUORI`: explains stack generally but does not analyze a supplied trace |
| 84 | Refactor this function without changing behavior. | `MURO`: I don't know about refactor yet |
| 85 | Design a test for a race condition. | `MURO`: I don't know about condition yet |
| 86 | Show the git diff for the current worktree. | `MURO`: I don't know about worktree yet |
| 87 | Fetch the current weather in Rome. | `MURO`: I don't know about weather yet |
| 88 | What is the latest price of Bitcoin? | `MURO`: I don't know about latest yet |
| 89 | Who won yesterday's match? | `MURO`: I don't know about yesterday's yet |
| 90 | What is the population of Tokyo this year? | `MURO`: I don't know about population yet |
| 91 | What is the legal status of this contract? | `MURO`: I don't know about contract yet |
| 92 | Give medical advice for chest pain. | `FUORI`: ungrounded recommendation response |
| 93 | Diagnose this symptom from one sentence. | `MURO`: I don't know about diagnose yet |
| 94 | Invent a new scientific theory and defend it. | `MURO`: I don't know about invent yet |
| 95 | Write a coherent ten-page story outline. | `FUORI`: generic workable-design template |
| 96 | Create ten non-overlapping metaphors for memory. | `FUORI`: generic workable-design template |
| 97 | Translate this paragraph into Japanese. | `MURO`: I don't know about translate yet |
| 98 | Identify the dialect of this sentence. | `MURO`: I don't know about identify yet |
| 99 | Correct the style of this legal paragraph. | `MURO`: I don't know about correct yet |
| 100 | Explain this poem in its historical context. | `FUORI`: generic causal-account template |

## Aggregazione per problema

| Classe | Numero |
|---|---:|
| Muro esplicito o lessico non riconosciuto | 64 |
| Risposta fuori dominio o template non istanziato | 33 |
| Artefatto rifiutato per schema non verificato | 3 |
| **Totale** | **100** |

## Lettura del risultato

Il problema dominante non e' l'assenza di singoli fatti. E' la mancata
separazione tra riconoscimento dell'intento e scelta della facolta'. Quando il
prompt contiene parole come `compare`, `cause`, `plan`, `return` o `invent`, un
modulo generico spesso vince senza verificare gli slot e il formato richiesti.

I fallimenti più gravi sono:

1. muri su aritmetica, logica, traduzione e operazioni di base;
2. template causali o progettuali applicati a domande completamente diverse;
3. perdita di memoria personale e di stato del dialogo;
4. assenza di schema per JSON, YAML, CSV, regex e altri artefatti;
5. incapacità di costruire ontologie e modelli temporanei per mondi nuovi;
6. confusione fra disponibilità di tool e riconoscimento della richiesta di tool.

Questa lista deve diventare materiale di discovery, non essere copiata
automaticamente nella suite TDD. Un caso va promosso a test `.p0t` solo dopo che
la classe comportamentale e' stata generalizzata e verificata con varianti,
ablazione e prova di crescita KB-first.
