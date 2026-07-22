# Motorize the Class — strategia per massimizzare LLMSCORE

> **Principio unico:** *motorizza la classe, poi nutri il motore su scala — non
> memorizzare istanze.* Ogni classe che parrot0 vince oggi è un MOTORE su una KB
> ampia (rime → `wordquery` + 35k `lexeme`; aritmetica → `wordproblem`). Ogni
> classe che perde è un LOOKUP su una tabella sparsa (`explanation`=4,
> `because`=25, `qa_reply`=6). Il fix è convertire ogni classe fragile da
> *tabella* a *motore + ingestione*, non aggiungere righe alla tabella.

Coerente con lo steer di F.: **meno moduli, più KB**. Un motore causale è UN
motore per l'intera classe "perché" — l'opposto di N moduli-qa. I motori sono
C (motori, non frasi); la conoscenza cresce nella KB per processo.

## Perché i fix puntuali perdono

Il giudice campiona 10 domande da una distribuzione illimitata. I run osservati
condividono ~0 domande. Una coppia `qa_cue/qa_reply` copre misura-zero →
valore atteso ~0 sul run successivo. Ottimizziamo il **punteggio atteso sulla
distribuzione**, non il prossimo campione di 10.

## Tassonomia delle classi e stato (baseline)

| Classe | Meccanismo | Copertura | Stato |
|---|---|---|---|
| Lessicale (rime/anagrammi) | motore `wordquery` + `lexeme` | qualsiasi parola | ✅ robusto |
| Aritmetica / word-problem | motore `wordproblem` | qualsiasi calcolo | ✅ robusto |
| Deduzione formale (sillogismi) | motore no-overlap (A+E) | forme A/E | 🟡 parziale |
| Generazione creativa | composer + fatti-tema | solo temi con fatti | 🟡 semi |
| Causale ("perché/come") | lookup `explanation/because` | ~30 istanze | ❌ fragile |
| Enciclopedico (fatti) | fatti a mano | capitali 175, resto sottile | ❌ fragile |
| Riddle laterali | lookup `qa_reply` | 6 istanze | ❌ fragile |
| Pragmatica / persona | insieme di fatti persona | comuni | 🟢 ok |

**Insight:** vinciamo dove abbiamo un motore su KB ampia; perdiamo dove abbiamo
un lookup su tabella sparsa.

## Leva combinatoria (da `generative-leverage.md`)

Un motore non ha bisogno di *migliaia* di fatti: ha bisogno di **fatti atomici
componibili** il cui prodotto cartesiano copre la classe.

- **La forma dell'output è già nel prompt** → il motore fa *selezione vincolata*,
  non recupero. Risolve un predicato parzialmente istanziato (es.
  `haiku(ocean, $O, $M, $C)`) e sceglie una tupla valida.
- **Prodotto, non somma:** 3 open × 3 mid × 3 close = 27 haiku; ~17 atomi
  narrativi = 486 storie. Aggiungere una variante è UN fatto; aggiungere uno
  slot è un fatto + un arco.
- **Garanzie per costruzione:** grammatica (linee pre-composte), coerenza
  (atomi etichettati per ruolo/tema), novità (l'assemblaggio esatto non è mai
  pre-scritto).
- **Corollario per la Fase 1 causale:** una spiegazione si *compone* da atomi
  relazionali (`cause` + `mechanism` + `purpose` + catena `is_a`), non da una
  stringa monolitica per topic → stessa leva prodotto, stessa economia.

Regola pratica: ogni classe generativa = UN motore + fatti atomici componibili.
L'anti-pattern è un modulo (o una `qa_reply`) per istanza.

## Le tre leve

- **(a) Motori generativi** che assemblano la risposta da relazioni strutturate
  (query sul grafo KB), così *qualsiasi* istanza della classe è tentabile.
- **(b) Ingestione per processo** (pipeline `kb/learning/` già esistente): la
  breadth viene dall'ingestione da Wikipedia statica, non dal digitare fatti.
- **(c) Uccidere il muro.** Ogni "I don't understand that yet" è uno 0 garantito
  *e* un tell squalificante. Tentativo plausibile in-dominio > muro — **senza
  inventare fatti falsi**; si declina solo in modo LLM-plausibile e onesto.

## Le fasi (ordinate per valore atteso)

### Fase 0 — Misura la CLASSE, non il campione *(prerequisito)*
Batteria di sonde per-classe (`tests/llmscore-probes/`), held-out, **mai
fixate singolarmente**. Metro oggettivo e senza-API: **wall-rate per classe**
(rileva le frasi-muro note). Un secondo metro opzionale con giudice-LLM per la
qualità. Deliverable: `make llmscore-probe` → tabella copertura per classe.

Frasi-muro rilevate: `I don't understand`, `I don't know about … yet`,
`Want me to learn`, `not sure`, `no idea`, `haven't learned`.

### Fase 1 — Motore causale "perché/come" *(EV più alto)*
Parser generico `why does X <verb>?` / `how does X work?` → query su
`cause/causes/mechanism/purpose/enables` + catene `is_a/has_part` → compone una
frase causale. Poi estrazione bulk cause→effetto dalle wiki-pages
(`docs/plans/extract-knowledge-from-prose.md`). Da ~30 a migliaia, per processo.

### Fase 2 — Ampiezza enciclopedica per ingestione
Autolearn puntato su classi di entità ad alta frequenza: superlativi
("il più alto/lungo"), autori↔opere, invenzioni↔date, capitali (già). Recall
factuale scala col processo.

### Fase 3 — Completezza deduttiva
Quadrato delle opposizioni completo (all/some/no × aff/neg) + catene 2-3 passi.
Investimento limitato, alta certezza: copre l'intera classe puzzle-logici.

### Fase 4 — Layer di framing/formato (universal-input)
Verbi di cornice (describe/explain/write/tell) e vincoli (in one sentence /
in N words / as a list) trattati come **modificatori** instradati sul contenuto,
mai come blocchi. Post-shaper generico impone il formato.

### Fase 5 — Fallback anti-muro
Ultimo stadio generativo che ingaggia la domanda in-dominio invece di murare.

## Criteri di misura (definizione di "fatto")

- **Wall-rate per classe** ↓ (metro primario Fase 0, senza API).
- **Copertura per classe %** (attempt plausibile) tracciata run-su-run.
- LLMSCORE ufficiale come controllo esterno, NON come metro di sviluppo
  (non-deterministico, campione piccolo).

## Ordine di esecuzione raccomandato

Fase 0 + Fase 1 insieme (senza la batteria non sappiamo se Fase 1 muove la
classe o l'istanza). Fasi 3-4 come quick-win in parallelo. Fase 2 e 5 dopo.

## Stato

- [ ] Fase 0: batteria sonde + `make llmscore-probe` + baseline wall-rate
- [ ] Fase 1: motore causale + ingestione cause→effetto
- [ ] Fase 2: ingestione enciclopedica mirata
- [ ] Fase 3: quadrato opposizioni completo
- [ ] Fase 4: layer framing/formato
- [ ] Fase 5: fallback anti-muro
