# Audit KB-first del C — che cosa nel motore e' conoscenza travestita

**gen403.** Nasce da due conversioni fatte nella stessa sessione — il nome
dell'utente (`b->name` → `user_value(name, X)`) e il registro sociale (88 cue
letterali → `kb/core/reactions.p0`) — e dalla domanda che ne e' seguita: *quante
altre ce ne sono?*

Questo file e' la risposta e la coda di lavoro. Ogni voce ha un marcatore
`TODO(kb-first):` nel punto esatto del codice, quindi `grep -rn "TODO(kb-first)"
src/` e' sempre allineato con questo elenco; qui c'e' l'ordine e il perche'.

## Perche' contarle, se funzionano

Una lista di parole nel C funziona. Il problema non e' che sbagli, e' che
**e' una conoscenza che nessuno puo' interrogare, correggere o estendere**: non
compare in `what do you know`, non si ritratta, non sopravvive a un
salvataggio, e cresce solo ricompilando. Il test operativo di `CLAUDE.md` —
*«parrot0 puo' impararne un nuovo membro domani, senza ricompilare?»* — su
ognuna di queste risponde no.

Le due conversioni del gen403 hanno mostrato che il costo di lasciarle li' non
e' teorico. In entrambe, spostare la conoscenza ha fatto **emergere guasti che
il C nascondeva**:

- le cue italiane di `mod_chitchat` erano MORTE. Venivano confrontate con il
  turno canonicalizzato, cioe' con la sua traduzione in inglese, dove quelle
  stringhe non compaiono mai. Presenti, invisibili, e ogni tanto qualcuno ne
  aggiungeva una senza accorgersi che non poteva scattare;
- «friend»/«amico» come cue di vezzeggiativo rubavano ogni frase che NOMINASSE
  un amico, e il ratchet registrava la risposta sbagliata come comportamento
  atteso;
- `self_reference` nel C non conteneva «chiamami», quindi l'imperativo italiano
  non risultava autoreferenziale — un buco che nessuno vedeva perche' la lista
  non era in nessun posto guardabile.

Il pattern si ripete: **una lista nel C e' un posto dove i buchi non si vedono.**

## Ordine di lavoro

Il criterio non e' la dimensione ma il DANNO: prima le conoscenze che la KB ha
gia' e che il C ignora (li' non c'e' nemmeno da scriverle), poi le conoscenze
del mondo, poi le superfici, infine i campi.

### 1. Duplicati — la KB ce l'ha gia', il codice la ignora

Il caso peggiore dell'audit: non una conoscenza che manca, ma una che esiste e
viene scavalcata da una copia nel C. Quando due sorgenti divergono, vince quella
che nessuno puo' correggere.

| Dove | Cosa | La sorgente vera |
|---|---|---|
| `70-social-pragma.c` `arts[]` | articoli EN+IT | `definite_article/1`, `indefinite_article/1` |
| `10-memory-knowledge.c` `bad[]` | pronomi, ausiliari, interrogativi | `question_word/1`, `auxiliary/1`, `stopword/1` |
| `60-agent-tools.c` `seq[]` ×2, `99-registry.c` `seqw[]` | sequenziatori | `sequencer/1` — **tre copie divergenti** |
| `60-agent-tools.c` `fill[]` | parole di riempimento | `stopword/1`, `function_word/1` |
| `85-translate-synth-world.c` `q[]` | ausiliari e interrogativi | come sopra — **quarta copia** |
| `70-social-pragma.c` `s[]` | parole dell'assenso | `intent_cue(agree, …)` (gen403) |
| `65-induce-verify-shell.c` `types[]` | tipi di pattern sociale | i tipi distinti in `social_pattern` |

### 2. Conoscenza del mondo dentro il motore

| Dove | Cosa | Forma proposta |
|---|---|---|
| `40-meta-reflection.c` `titles[]` | regina, faraone, imperatrice | fatti in `world-facts.p0` |
| `60-agent-tools.c` `exts[]` | estensioni di file | `file_extension(rust, ".rs")` |
| `25-wordmath-reasoning.c` `ex[]` | verbi che SOTTRAGGONO in un problema | `polarity(eat, minus)` |
| `85-translate-synth-world.c` `ops[]` | nomi delle operazioni | `operator_word/2`, come `numeric_cue/2` |
| `code.c` `kw[]` ×2, `t[]`, `pure[]` | parole chiave C/Python, metodi puri | `language_keyword(c, "if")` — un linguaggio nuovo deve essere insegnabile |

### 3. Vocabolario — classi di parole

Tutte bilingui dentro un solo `||`, che e' il sintomo: **la lingua e' diventata
una proprieta' del codice.**

| Dove | Classe |
|---|---|
| `70-social-pragma.c` `h[]` | parole dell'incertezza (maybe/forse/boh) |
| `70-social-pragma.c` `c[]` | connettivi di contrasto (but/però) — `connector/1` esiste |
| `70-social-pragma.c` `q[]` | pronomi indefiniti |
| `70-social-pragma.c` `after[]` | preposizioni che introducono un topic |
| `20-math.c` `ok[]`, `f[]` | parole ammesse in una domanda aritmetica |
| `kb.c` `stop[]` | stopword **nel kernel**, mentre `stopword/1` ne ha 297 |

`kb.c` e' l'unico con un vincolo reale: il kernel non deve dipendere dal
contenuto di una KB specifica. La strada li' non e' interrogare la KB da dentro,
ma **far passare la lista dal chiamante**, che la KB ce l'ha.

### 4. Superfici — frasi che riconoscono un intento

Sono `intent_cue`, e il gen403 ha mostrato che la precedenza fra intenti si
dichiara come dato (`chitchat_reaction(Kind, Priorita')`) invece di dipendere
dall'ordine in cui e' scritto il C.

| Dove | Intento |
|---|---|
| `99-registry.c` `v_en[]`/`v_it[]` | **le frasi del muro** — la cosa che parrot0 dice piu' spesso, e l'unica non insegnabile |
| `40-meta-reflection.c` `intros[]` | apertura di un gioco di ruolo |
| `90-repair-robust-abduce.c` `sup[]`, `markers[]`, `pre[]`, `sim_pre[]`, `opt_pre[]`, `neg_pre[]` | supposizione, sorpresa, abduzione |
| `50-self-research-loop.c` `strong_heads[]`/`weak_heads[]` | domande che chiedono di documentarsi, con forza dichiarata |
| `85-translate-synth-world.c` `triggers[]` | richiesta di un comando di shell |

### 5. Campi C che sono conoscenza sull'utente

Stessa specie di `b->name` e `b->user_mood`, convertiti nel gen403.

| Campo | Forma |
|---|---|
| `user_preference_verb/value/has_` | `user_value(like, …)` |
| `possessions[8][2][64]` | `called(cane, rex)` — oggi il nono possesso si perde in silenzio |
| `current_topic`, `user_constraint` | `user_value/2` + `session_slot/1` |
| `role_name`, `role_kind` | `session_value(role, …)` — interrogabile e ritrattabile |

Quando `current_topic` e `user_constraint` saranno fatti, il blocco «Session
context» di `mod_memory` diventa un ciclo sui `session_slot` dichiarati invece
di tre `if` che sanno i nomi.

## Il metodo, in breve

Quello che ha funzionato due volte nel gen403:

1. **Guardare se la conoscenza esiste gia'** nella KB. Meta' di questo elenco
   non e' da scrivere, e' da smettere di riscrivere.
2. **Un registro, non un elenco di rami.** `chitchat_reaction(Kind, Priorita')`
   e `slot_evidence`/`slot_eager` sono la stessa forma: una tabella che il C
   percorre, e la precedenza come dato.
3. **Interrogare anche il turno GREZZO**, non solo il normalizzato — e' li' che
   le cue italiane erano morte.
4. **Il cricchetto non verifica che la cosa funzioni** (funzionava gia'):
   verifica cio' che il C non poteva fare — aggiungerne una a runtime,
   ritirarla, cambiarne la precedenza cambiando un numero.
5. **Aspettarsi che emerga un guasto.** Ogni conversione ne ha scoperto uno. Se
   una conversione non fa cadere nessun muro e non fa emergere niente, vale la
   pena chiedersi se ha spostato davvero la conoscenza.
