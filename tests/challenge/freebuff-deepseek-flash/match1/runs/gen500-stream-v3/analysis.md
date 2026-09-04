# Gara annullata — match1 / gen500-stream-v3

Nessun vincitore e nessun punto: il controller non ha potuto provare che
entrambi gli agenti abbiano ricevuto e completato lo stesso task.

Motivo: `freebuff:timeout after 829 stream events`.

## Tracce disponibili

- `parrot0`: `completion_marker`, task_submitted=True, `./parrot0/raw.log`, `./parrot0/stream.jsonl`, `./parrot0/logic-actions.jsonl`, `./parrot0/code/`.
- `freebuff`: `timeout`, task_submitted=True, `./freebuff/raw.log`, `./freebuff/stream.jsonl`, `./freebuff/logic-actions.jsonl`, `./freebuff/code/`.
