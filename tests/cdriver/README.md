# tests/cdriver — i test che compilano un driver contro il C

Non sono suite conversazionali e **non diventano `.p0t`**. Ognuno compila una
sonda contro `src/*.c` e chiama una funzione del motore direttamente, per
provare cose che **nessun prompt deve poter chiedere**.

| file | che cosa prova |
|---|---|
| `integration/brain-agent-state.c` | `brain_agent_set_state`: transizioni atomiche candidate/active, rifiuto degli stati invalidi e conservazione di `rec_source/2` |
| `exec_kernel.sh` | `p0_exec`: un ciclo infinito torna come timeout tipizzato in tempo limitato; il timeout uccide il **gruppo** di processi senza orfani; un programma inesistente è `spawn_failed` e non un successo vuoto; una cwd fuori dal workspace è `unsafe_path` |
| `exec_dirfd.sh` | `p0_exec_at`: l'esecuzione su un albero allestito, ancorata a un **descrittore di directory**. `..`, percorso assoluto, directory-symlink e fd di file regolare sono tutti rifiutati; il rootfd del chiamante non viene consumato; `p0_exec` legacy condivide lo stesso nucleo |
| `patch-artifact.c` | Meccanica diretta di `patch.h` per `tests/p0t/code/patch-artifact.p0t`: immagini, digest, diff, prepare/commit, conflitto, policy, rollback e tamper |
| `integration/agentkernel.c` | Store diretto dell'agent kernel: ID, conteggi, proiezione e round-trip JSON |
| `integration/kb-evidence-scale.c` | Scala e saturazione delle evidenze KB |
| `integration/patch-check.c` | Check candidato/oracolo/policy/commit con `p0_exec_at` |

Tutti i driver C vengono eseguiti dal runner generico `run.sh`, che riceve il
driver e le sue dipendenze (`.c` o `.o`) come argomenti. Non esistono runner
shell specifici per questi contratti.

## Perché restano script

Per portarli in `.p0t` servirebbe esporre l'esecutore sul layer MCP — cioè dare
a **qualunque client** il permesso di eseguire argv arbitrari, e per
`p0_exec_at` anche di passare un descrittore di directory. È una decisione di
sicurezza, non una conversione, e va presa deliberatamente.

Il precedente opposto è `code.check_sort`: quello è un **giudice** su un
sorgente — dispone un candidato, non esegue ciò che gli si chiede — ed esporlo
aggiunge una capacità utile senza aggiungere potere. Vive infatti in
`tests/p0t/code/check_sort.p0t`.

## La regola

Un test entra qui solo se **la cosa che prova non ha, e non deve avere, una
superficie raggiungibile da un turno**. Se una superficie esiste o può esistere
senza allargare i permessi, il test va convertito.
