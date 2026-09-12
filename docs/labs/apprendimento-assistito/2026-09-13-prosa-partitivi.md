# Prosa, 13 settembre 2026 — 16 → 17/50

Checkpoint di sviluppo sulla KB viva completa, a partire da `140ec5d8`.
Banco e attese invariati: `tests/fixtures/prose/ladder/r300.{txt,q}`.
Nessun fatto del brano persistito, nessun `/save`: **W=0, L=0,
meta-capability-only**. Le lezioni nelle sonde verificano il motore e non
costituiscono addestramento consolidato sul mondo.

| misura | baseline confermata | dopo |
|---|---:|---:|
| merito | 16/50 | 17/50 (34%) |
| parole delle domande risolte | 91 | 97 |
| meta | 2/2 | 2/2 |
| struttura | 5/5 | 5/5 |
| cancello >299 | aperto | aperto, mancano 203 parole |

Il confronto dei referti cambia solo due risposte:

- `what do shallow coral reefs form?`: `some.` diventa
  `some of earth's most diverse ecosystems.`
- `what is calcium carbonate for?`: `calcium carbonate is CaCO3.` diventa
  un declino. La relazione di scopo resta da raggiungere; non si conta un
  successo nel merito per aver tolto una risposta non pertinente.

## Le due distinzioni

**Il partitivo tiene insieme valore e dominio.** Il lettore gia' conservava
`half of the global land area`, ma tagliava `some of ... ecosystems` a `some`.
`partitive_link/2` porta in KB la congiunzione fra testa e preposizione, e
`partitive_head_class/1` collega misure e quantificatori. I due consumatori
esistenti (confine dello slot e limite di lunghezza dell'atomo) chiedono la
stessa relazione. Nessun nuovo scanner, nessun fatto sul corallo.

**La definizione non soddisfa una relazione lasciata aperta.** Le risposte
definitorie verificavano il soggetto, ma non la preposizione in coda. Il nuovo
controllo `definition_answer_ok/1` consuma i token dell'IR del turno:
`definition_unfilled_relation/1` riconosce una preposizione senza complemento.
La verifica si applica dopo la ricerca del candidato nei percorsi definitori,
compresa la tassonomia; le risposte relazionali continuano a usare le proprie
prove. `what is a compass used for?` continua a rispondere dallo scopo noto.

La sonda prova anche il cambiamento **parlato** della condotta:
`aboard is a preposition`, replay, `forget that aboard is a preposition`,
replay e reinsegnamento. E' una prova meccanica su una forma diagnostica,
non una pretesa che quella domanda sia prosa inglese naturale.

## Verifica e limiti

`tests/p0t/language/prose_relation_scope.p0t`: **31 proprieta' verdi**.
Comprende transfer a `many`, `half`, `all`, il confine del complemento
`in the animal phylum`, crescita e ablazione di una testa nuova (`handful`),
e ablazione del ponte fra quantificatori e partitivo. La testa `handful` viene
asserita con il comando del test: la lezione naturale `handful is a measure
noun` non e' capita, quindi **non** si dichiara quel canale parlato chiuso.
La crescita parlata della preposizione, invece, e' verificata nei due versi.

`make build`: riuscito, nessun warning nuovo o `PARSE ERROR` osservato.
`make soft-test`: arrestato su `basics.p0t`, timeout di 1 secondo per
`is a tiger a mammal` (1,11 s) e `what is the opposite of hot` (1,20 s).
L'handoff precedente gia' documentava timeout in questo file: non si dichiara
il gate verde, ne' si modifica il limite. Il primo tentativo non aveva potuto
aprire il socket nella sandbox; il gate e' stato eseguito fuori sandbox.

Profilo A/B sequenziale su cinque domande: somma dei tempi di turno
**4,694 → 4,664 s**. Campione piccolo, non prova un'accelerazione.
`extract_frame` resta una voce costosa: 20–30 chiamate e 150–245 ms sui turni
definitori finali. L'indicizzazione degli schemi resta aperta.

Bilancio: **C +33/-45**, principalmente per accorciamento di commenti
storici; non e' una migrazione completa del lettore. KB **+26** righe.
`split_words` resta **132/54/33 = 219**, uguale alla baseline corrente:
nessuna riscansione rimossa o aggiunta.

## Residui da non perdere

Il vecchio handoff contava due risposte false, ma il referto baseline contiene
anche `what are most coral reefs built from?` → `Colonies.`. Resta identica:
non contiene la distinzione `stony corals` richiesta dal testo. La sonda
minima e' la sola frase `Reefs are formed of colonies of coral polyps held
together by calcium carbonate.`, seguita dalla domanda: risponde `answerframe`.
Le costruzioni gia' apprese riconducono `built from` e `formed of` a `made_of`;
la frase con **Most** non viene acquisita, e il lettore delle domande usa la
relazione generica disponibile. La relazione attenuata resta un lavoro
strutturale, non un fatto da inserire a mano.

La parafrasi `what is x for means what is x used for` e' stata provata solo
in una sessione scartata: si riapplica al proprio risultato e genera
`used used used`. Non e' stata salvata; la riscrittura non idempotente resta
un reperto da affrontare separatamente.

Riproduzione: copiare `kb/ bin/ scripts/ tests/` in uno scratchpad e lanciare
`P0_PROBE_STEP2=1 ./scripts/prose-probe.sh tests/fixtures/prose/ladder/r300.txt`.
I [referti grezzi](2026-09-13-prosa-partitivi/) conservano baseline, risultato,
profili e verifiche. Il prossimo circuito e' il livello clausola: relative
`that`, coordinazione dei predicati, complemento tassonomico e quantificazione
attenuata restano fuori da questo incremento.
