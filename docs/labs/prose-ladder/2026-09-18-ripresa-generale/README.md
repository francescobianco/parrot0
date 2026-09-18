# Ripresa verso la comprensione generale — 18 settembre 2026

Sonda diagnostica su `636ce2a2`, dopo `make build` riuscito. Nessuna modifica
a C o KB. Il working tree era pulito prima dell'aggiornamento documentale.
Configurazione: `agi`, `PARROT0_SESSION=`, `PARROT0_WIKI_FETCH=0`,
`PARROT0_TOOLS=1`, `PARROT0_LANG=en`. Boot: 58.401 fatti, 4.552 regole.
La sessione vuota non riduce la KB del profilo. Nessun `/save`.

Il testo è la nona frase di
[`r320.txt`](../../../../tests/fixtures/prose/ladder/r320.txt), già presente
nel corpus del progetto; la fonte è registrata in
[`SOURCES.md`](../../../../tests/fixtures/prose/ladder/SOURCES.md).
Non è una nuova verifica della pagina web né una certificazione del piolo.

```text
The decomposition process is aided by shredding the plant matter, adding water, and ensuring proper aeration by regularly turning the mixture in a process using open piles or windrows.

What aids the decomposition process?
```

Il testo nomina triturazione della materia vegetale, aggiunta d'acqua e
aerazione tramite rivoltamento. Una risposta che riporti soltanto il primo
elemento è parziale. La sonda sulla frase isolata serve a localizzare un
difetto; il brano intero resta necessario per misurare comprensione della prosa.

## Risultato osservato

| Turno | Risposta | Tempo senza profiler |
|---|---|---|
| Frase | `Hmm, I don't know about decomposition process yet. Want me to learn about it?` | 3.181,8 ms |
| Domanda | `Hmm, I don't know about decomposition_process yet.` | 1.622,6 ms |
| `who answered?` | `No module could handle that — it fell through to the not-understood fallback.` | 739,7 ms |

Una sola misura per turno, nella stessa sessione. Tempo monotono dalla
scrittura del turno al prompt successivo, stdout non bufferizzato con
`stdbuf -o0`; boot escluso, trace/debug spenti. Non è una mediana, né una
comparazione di revisioni. L'esito coincide con quello della sessione
separata con `P0_READ_TRACE=1`.

Artefatti originali:

- [Trascrizione e tempi senza profiler](timing.json).
- [Traccia di lettura](read-trace.log).
- [Profilo separato della frase, a sessione nuova](profile.log).

## Profilo e limite della diagnosi

Con `/debug` attivato prima della frase: 3.176,4 ms di turno, 2.271,5 ms
nel solver, 904,9 ms fuori; 9.122 query, 12 ricostruzioni d'indice.

| Predicato | ms | Chiamate |
|---|---:|---:|
| `turn_teaching_offer` | 524,8 | 1 |
| `phrase_canon` | 501,1 | 3.052 |
| `turn_bookkeeping` | 452,0 | 34 |
| `np_closer` | 209,7 | 7 |
| `input_frame_observe` | 153,2 | 1 |

Questo rende concreta la prima ipotesi di lavoro: canonicalizzazione o
contabilità ripetuta potrebbero essere ridotte. **Non prova** che una cache
sia corretta né che quel costo spieghi da solo il muro. Tempo con profiler e
tempo senza profiler sono esecuzioni distinte; non usarli per stimarne
l'overhead da questo singolo campione.

Nella trace la domanda trova `cue=aids pred=aid` e finisce in fallback. Non
basta per attribuire il difetto solo alla lettura o solo alla domanda; manca
ancora il confronto dei record intermedi nei diversi percorsi. La lettura
statica trova due ingressi da raccordare: `compound_turn_lead` in
`99-registry.c` e `extract_clause` in `30-generation-reading.c`.

## Riproduzione breve

Eseguire dalla radice del repository; il testo è esplicito per non dipendere
da uno splitter che potrebbe cambiare. Il comando riproduce gli esiti, non
misura separatamente i turni. Per il profilo usare un processo nuovo con
`/debug` come prima riga e la sola frase come turno successivo.

```sh
make build
printf '%s\n' \
  'The decomposition process is aided by shredding the plant matter, adding water, and ensuring proper aeration by regularly turning the mixture in a process using open piles or windrows.' \
  'What aids the decomposition process?' \
  'who answered?' '/quit' |
  PARROT0_SESSION= PARROT0_WIKI_FETCH=0 PARROT0_TOOLS=1 \
  PARROT0_LANG=en PARROT0_PROFILE=kb/profiles/agi.p0 ./bin/parrot0
```

Nessuna suite, nessun banco completo, nessun nuovo test con timeout lungo.
Nessuna variazione di comprensione dichiarata. Il referto r320 citato come
`-2320` nel vecchio handoff non esiste: il file di misura è `-2318`; `-2320`
è un log di sessione che riporta proprio quella misura. Il valore 19/68 dopo
il ritiro resta una previsione da rimisurare, non un risultato di questa sonda.

La prossima consegna è [E0 del percorso esecutivo](../../../plans/lettura-della-prosa-esecuzione.md).
