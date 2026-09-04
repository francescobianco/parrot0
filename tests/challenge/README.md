# Coding-agent challenge laboratory

Questo banco mette due agenti sullo **stesso compito e sugli stessi byte**, in
sequenza. Non è un test di release e non viene eseguito da `make test`: è uno
strumento di discovery per capire quali facoltà mancano ancora a parrot0.

## Protocollo di equità

- Il controller crea una directory temporanea neutrale il cui ultimo componente
  è sempre `code/`. Nessun nome di agente compare nel prompt, nel cwd o nei nomi
  degli artefatti.
- Prima di ogni agente `code/` viene eliminata e ricostruita da `seed/`; il digest
  del seed viene registrato. I processi sono sequenziali e non vedono l'output
  dell'avversario.
- Il prompt effettivo è identico byte-per-byte. L'ordine iniziale alterna fra i
  match per rendere visibile un eventuale effetto di posizione.
- Ogni sessione ha lo stesso timeout. La velocità viene registrata ma non assegna
  punti: il punteggio deriva soltanto dal judge deterministico.
- `raw.log` conserva i byte del PTY; `transcript.txt` è la vista senza ANSI.
  Viene catturato tutto il ragionamento che la CLI sceglie di esporre, non un
  presunto pensiero interno.
- L'intero albero `code/`, non soltanto il file indicato come ancora nel manifest,
  viene archiviato prima del reset. Non esiste alcun limite single-file: ogni
  agente può leggere, modificare e creare tutti i file necessari. I judge
  compilano/eseguono una copia dello snapshot completo e producono check nominati.
- Un watcher osserva ogni chunk del PTY e registra offset, tempo e digest in
  `stream.jsonl`. Spinner, gauge e redraw sono attività reale. Se per tre secondi
  non arriva alcun byte, il controller deve riconoscere una completion o
  applicare una `logic_action` dichiarata; altrimenti annulla l'intera gara come
  invalida, senza punti e senza avviare il concorrente successivo.
- Ogni reazione interattiva è configurata, limitata nel numero di usi e salvata
  in `logic-actions.jsonl`. FreeBuff, per esempio, riceve Enter sul selettore del
  modello e il task viene inviato soltanto dopo il vero editor. Nessun prompt
  sconosciuto riceve automaticamente un consenso.
- Le azioni di sblocco del task sono scoped per agente dentro `match.json`. Un
  consenso viene inviato soltanto se il testo dello stato corrisponde a
  un'operazione prevista per quell'agente e quella codebase (lettura/edit/build/
  test nel `code/` temporaneo). Una richiesta fuori aspettativa non viene
  approvata: allo scadere del lease annulla la gara e resta nel transcript.

Il banco **non è una sandbox di sicurezza**: gli agenti possono usare strumenti
locali e i judge eseguono gli artefatti. Va usato solo con agenti fidati.

## Uso

```sh
# Solo preflight: non avvia agenti e non usa la rete.
tests/challenge/run-challenge.sh check

# Prima gara completa (sessioni potenzialmente lunghe/networked).
tests/challenge/run-challenge.sh run --match match1

# Tutti i match; un run-id esplicito facilita confronti longitudinali.
tests/challenge/run-challenge.sh run --run-id gen500-baseline

# Rigenera la classifica leggendo soltanto i result.json archiviati.
tests/challenge/run-challenge.sh scoreboard
```

L'output storico vive in
`freebuff-deepseek-flash/<match>/runs/<run-id>/`: prompt, transcript, raw PTY,
albero `code/`, stream trace, logic-action, risultato JSON e diagnosi differenziale. I file
`<match>/<agent>.log` sono copie comode dell'ultimo transcript; lo storico non
viene sovrascritto. `scoreboard.md` usa l'ultimo run completo di ogni match per
la classifica e mantiene i link a tutti i run.

FreeBuff `0.0.167` usa DeepSeek V4 Flash 07/31 come default dichiarato dalla sua
CLI; il modello effettivamente mostrato nel transcript resta l'autorità se il
servizio applica un fallback. Il primo avvio può richiedere login/onboarding: il
preflight verifica l'eseguibile, non consuma una sessione e non può certificare
l'autenticazione.

## Aggiungere un match

Creare `tasks/<id>/match.json`, `task.md`, `seed/` e un `judge.py` che riceve
come unico argomento la directory archiviata `code/`. Il judge deve stampare
come ultima riga un JSON con `score` (0–100), `status`, `checks` e
`diagnosis_tags`. Aggiungere poi l'id a `league.json`.

Un buon match parte da una codebase, non da uno snippet o da una funzione
didattica: richiede più facoltà (esplorazione, comprensione del contratto,
localizzazione, modifica multi-file, verifica e gestione di casi avversi) e
assegna punti a proprietà osservabili. Un fallimento di parrot0 che FreeBuff
supera diventa un gap discriminante; un fallimento comune mette in discussione
difficoltà, contratto o strumenti prima di diventare una lezione KB.
