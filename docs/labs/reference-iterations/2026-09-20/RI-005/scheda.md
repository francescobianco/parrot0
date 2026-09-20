# RI-005 — un numero misto è una quantità sola

**ID, famiglia e contesto umano.** Famiglia: *lettura di una quantità scritta in
due pezzi* — causa di fallimento diversa dalle precedenti (non una superficie
mancante, non un verso, non una forza di turno: una **frase spezzata in mano al
lettore**). Contesto umano: è la domanda più ordinaria che si faccia su
un'unità di tempo, e la si fa con la frazione perché è così che si parla.

**Commit/stato iniziale.** `749299b9` (RI-004 chiusa), profilo `agi`, `en`.

**Stimolo congelato.**

```text
How many minutes are there in two and a half hours?
```

**Criterio di riuscita.** 150. Non una cifra qualunque: il difetto iniziale era
un numero **falso detto come un fatto**.

**Risposta iniziale e limite osservato** (`r1-baseline.json`):

| turno | risposta |
|---|---|
| **How many minutes are there in two and a half hours?** | **«1.»** ✘ misclaim |
| How many minutes are there in an hour? | «A hour has 60 minutes.» ✔ |
| How many minutes are there in two hours? | «120 minutes: 2 hours with 60 minutes each.» ✔ |
| What is half of 60? | «30.» ✔ |

**La conoscenza c'era tutta**: l'unità, la moltiplicazione e la metà. Mancava
la lettura di una quantità scritta in due pezzi. `/debug` lo dice in due righe:

```text
modulo                 arith
debug_np_candidate     a half hours
debug_turn_entity      half_hours  two  many_minutes
```

Il lettore vedeva il «two», poi la congiunzione, e la frase gli si spezzava:
«a half hours» diventava un sintagma e il conto usciva da lì.

## R4 — la cura, **senza una riga di C**

Il calcolo della quantità scalata era già in KB (`count_total/3`,
`kb/core/numeric-questions.p0`): il difetto stava nel pezzo che legge il numero.
Aggiunte quattro cose, tutte in `kb/core/numeric-questions.p0`:

- `mixed_connector/1` — quali parole legano l'intero alla sua frazione (`and`, `e`);
- `fraction_article/1` — l'articolo della frazione (`a`, `an`, `un`, `una`);
- `mixed_count/4` — «NUMERO + connettore + articolo + frazione» è **una**
  quantità; le frazioni le sa già `fraction_word/2` (lexicon.p0);
- `mixed_total/4` + il testo, con `turn_plan_candidate`/`turn_response` come
  fanno le regole vicine.

Il conto si fa nell'unità piccola, come lo direbbe una persona: due ore sono
120 minuti, la mezza ne fa 30. **Misurato invece che supposto**: la divisione
del motore è in virgola mobile, quindi una prima versione con guardia `mod` e
una regola speciale per la metà erano inutili — e la regola speciale produceva
«17.5 and a half days». Tolte entrambe: resta una regola sola.

## R5 — certificazione

| prova | esito |
|---|---|
| stimolo | **«150 minutes.»** |
| TR1 — altra unità, altro intero | `one and a half minutes` → **«90 seconds.»** |
| TR2 — altra composizione | `two and a half days` → **«60 hours.»** |
| TR3 — risultato non intero | `two and a half weeks` → **«17.5 days.»** (prima: «1.») |
| contrasto — la forma vecchia non cambia | `two hours` → «120 minutes: 2 hours with 60 minutes each.» |
| lezioni nuove esercitate dalla capacità | `A week has 168 hours.` → `one and a half weeks` = **252 hours**; `A day has 1440 minutes.` → `two and a half days` = **3600 minutes** |

## R6 — salvataggio e processo nuovo

```text
parrot0: routed 17 clause(s) into the KB tree
```

| categoria | n | clausole |
|---|---:|---|
| `W` fatti veri | **2** | `quantity(week, hours, 168)`, `quantity(day, minutes, 1440)` — esatti (7×24, 24×60) |
| `P`/`O` | 15 | provenienza e battute |
| `X` | 0 | — |

**Processo nuovo** (`r6-fresh-process.json`): 7/7. Lo stimolo, i due transfer
sulle lezioni nuove, il contrasto, e i replay di RI-003 («Naples.»), RI-002
(verso rispettato) e RI-001 («north sea.»).

`make soft-test` verde in 13 s (budget 15 s).

## Limiti residui

1. «How many minutes are there in **half an hour**?» resta un muro: la frazione
   **senza** intero davanti è un'altra forma, e non è stata aperta.
2. L'ordine italiano — «due ore e mezza», con l'unità **prima** della frazione —
   non è letto: `mixed_connector(e)` c'è, la forma no.
3. La risposta del caso misto non porta la spiegazione che il caso intero porta
   («120 minutes: 2 hours with 60 minutes each»): dice il numero e basta.

**Stato dell'iterazione:** completa.
**Classificazione del training:** `trained` (W = 2, X = 0), con una cura
interamente KB: nessuna riga di C.
