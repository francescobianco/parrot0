# RI-004 — una lezione nasce nello strato che si salva

**ID, famiglia e contesto umano.** Famiglia: *persistenza di ciò che si impara
parlando*. Contesto umano: il maestro insegna, parrot0 dice «Learned», il
maestro salva — e il giorno dopo la lezione non c'è. Non c'è modo di
accorgersene dalla conversazione: **il turno dice di sì**.

Il limite viene da RI-003, che si è chiusa `partial` proprio per questo.

**Commit/stato iniziale.** `2451c96d` (RI-003 parziale), profilo `agi`, `en`.

**Stimolo e criterio.** Lo stimolo è un ciclo di due processi, non un turno:

```text
processo 1:  Verona is a city.        ->  Learned: verona is a city.
processo 1:  /save                    ->  parrot0: routed 6 clause(s)
shell:       grep -rn "^city(" kb/    ->  (niente)
processo 2:  Is Verona a city?        ->  non so
```

Riuscita: dopo `/save` il fatto insegnato **esiste in un file** e un processo
nuovo lo usa. Fallimento: `/save` dichiara di aver instradato e non scrive.

**Risposta iniziale e limite osservato.** `routed 6`, cinque clausole scritte
(provenienza ×3, battute ×2) e la sesta — **il fatto** — scomparsa.
`grep -rn "^city(" kb/` rendeva **zero** dopo quattro lezioni accettate.

## R4 — la diagnosi, fatta con gli strumenti giusti

Il grep nel sorgente non arrivava: tre siti diversi asseriscono una classe, e
la domanda non era «quale codice» ma «con quale **origine**». Due sonde
temporanee in `kb_save_routed` e in `kb_set_origin` hanno risposto in due giri:

```text
[save] fact_source(city(verona)) -> kb/core/../machinery/fact-provenance.p0:501
[save] reading_fact(city(verona)) -> kb/core/../machinery/fact-provenance.p0:885
[save] VISTO city/1 origin=8
```

`origin = 8` è **`KB_REFLECTIVE`**, e `src/kb.h` lo definisce così:
«the self-model (i_am/module) — **never persisted**». Il fatto insegnato
nasceva nello strato del modello di sé, quello che per contratto non si salva.
Il turno arriva al lettore (`mod_knowledge`) da dentro una finestra riflessiva
— la traccia di `kb_set_origin` mostra il turno che oscilla fra 2 e 8 — e il
lettore asseriva con l'origine che si trovava addosso.

**La cura è la politica che esisteva già**, applicata dove mancava: `mod_mention`,
per lo stesso atto, fissa `KB_SESSION` prima di scrivere e ripristina dopo.
Ora lo fa anche il lettore delle classi. Chi deve restare riflessivo — le sonde
di `/debug`, il pid, la lingua del giro — continua a dichiararlo dove nasce.

Aggiunta una traccia permanente e gratuita, `P0_SAVE_TRACE`: `savemap.tsv`
rispondeva già a «perché è finito lì», e mancava l'altra metà — «perché non è
finito da nessuna parte» (nessuna casa / insert fallita / scartato come
`turn_scratch`).

## R5 — certificazione sul meccanismo fermo

| stato | esito | prova |
|---|---|---|
| iniziale | `routed 6`, cinque scritte, il fatto perso; `grep "^city("` → zero | sopra |
| meccanismo riparato, senza lezioni | `Tell me a city in Italy.` → muro; `Is Milan a city?` → non so | `r5a-before-lessons.json` |
| lezioni + `/save` | il diff KB contiene `city(milan)`, `city(naples)`, `city(turin)`, `city(rome)`, `class_surface(city, city)` | `git diff -- kb/` |
| **processo nuovo** | `Tell me a city in Italy.` → «Milan.»; `…, but do not mention Milan.` → **«Naples.»**; `Is Milan a city?` → «Yes.» | `r6-fresh-process.json` |
| contrasto — la regola NON si applica al modello di sé | `grep -c "process_pid\|^i_am(\|^module("` nei file salvati → **0** | idem |
| replay RI-003 | `Tell me a country in Asia, but do not mention China.` → «India.» | idem |
| replay RI-002 / RI-001 | «Yes.» / «black sea.» (tre salti) | idem |

**RI-003 si chiude qui**: la sua seconda metà — la classe insegnata parlando —
adesso sopravvive al salvataggio, e il suo transfer funziona in un processo che
non ha mai sentito la lezione.

## R6 — conteggio

```text
parrot0: routed 42 clause(s) into the KB tree
```

| categoria | n | clausole |
|---|---:|---|
| `W` fatti veri | **4** | `city(milan)`, `city(naples)`, `city(turin)`, `city(rome)` |
| `L` linguistici | 1 | `class_surface(city, city)` |
| `X` | 0 | — |

Le altre 37 instradate erano provenienza e battute **già presenti** dal
salvataggio di RI-003 (`sm_insert` non duplica): il guadagno di questo giro è
esattamente ciò che RI-003 aveva perso.

## Limiti residui

1. **Il ritiro non si persiste.** Misurato: insegnata e salvata
   `city(bologna)`, «forget that bologna is a city» risponde «Forgotten:
   city(bologna).» e il `/save` successivo **non toglie la riga dal file**.
   `kb_save_routed` sa solo aggiungere. Una KB che non sa dimenticare su disco
   è la metà mancante di questa iterazione, ed è il candidato successivo.
2. `tests/p0t/save/persist.p0t` resta 31/6 — **identico prima e dopo** (misurato
   con e senza questa modifica). Una delle sei righe rosse è proprio questa
   specie, su un'altra via di salvataggio (`kb_save` con maschera d'origine),
   che questa cura non tocca.

**Stato dell'iterazione:** completa.
**Classificazione del training:** `trained` (W = 4, L = 1, X = 0), con la
riparazione strutturale che rende salvabile ogni lezione di classe futura.
