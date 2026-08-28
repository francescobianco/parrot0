# parrot0 — leggi PRIMA di toccare qualsiasi cosa

## ⛔ I MANTRA — [`MANTRA.md`](MANTRA.md)

**Apri `MANTRA.md` adesso, prima di scrivere una riga di codice**, qualunque sia
il motivo per cui sei qui: un bug puntuale, una domanda, un refactoring, o niente
in particolare. Non è cerimoniale. In parrot0 la regola non è *"scrivi codice che
funziona"*, è **"non scrivere codice se la conoscenza può farlo"** — e una patch
che funziona ma mette conoscenza nel C fa REGREDIRE l'esperimento, anche quando i
test diventano verdi.

La domanda zero, prima di ogni modifica: **è generalizzabile KB-first?** Se la
risposta è "sì ma è più lavoro", si fa il lavoro.

Il test operativo: **"parrot0 può impararne un nuovo membro domani, senza
ricompilare?"** Se no, quella conoscenza è nel posto sbagliato.

## Come si verifica

- `make soft-test` — verifica di **avanzamento**, budget 15s. Da usare dentro il
  ciclo di modifica. Se sfora il budget si tolgono casi, non si alza il budget.
- `make test` — la suite intera (~1619 assert). Deve restare verde.
- `parrot0 --test FILE.p0t` — manda un singolo file al demone (`make test-engine`).

## Mappa breve

| File | Cosa |
|---|---|
| `MANTRA.md` | le 15 regole operative — **il punto di partenza** |
| `PRINCIPLES.md` | il *perché* dell'esperimento, e la regola anti-inganno |
| `AGENTS.md` | regole operative per chi modifica il codice |
| `C_TODO.md` | che cosa deve ancora uscire dal C — i residui del motore |
| `LEARN_TODO.md` | la coda dei temi da far apprendere con `LEARN_PROTOCOL.md` |
| `LEARN_PROTOCOL.md` | come si addestra parrot0 parlando, e i suoi gate |
| `docs/plans/` | i piani vivi (uno per direzione di lavoro) |
| `docs/sessions/` | resoconti di sessione: cosa si è scoperto e perché |
| `kb/core/*.p0` | la conoscenza — è qui che va la roba nuova |
| `src/brain/*.c` | il motore — deve restare adattatore, non contenere vocabolario |

## La regola che riassume tutte le altre

Una proposta è nella direzione giusta se **aumenta ciò che parrot0 vede e la sua
capacità di decidere**; è nella direzione sbagliata se **riduce ciò che un suo
pezzo vede** per avere ragione per costruzione — anche se è più semplice, anche
se i test diventano verdi.
