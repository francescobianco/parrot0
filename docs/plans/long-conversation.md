# Conversazione lunga — missione 100 turni coerenti

> **Stato:** esperimento iniziale gen335+ (2026-07-17).
> **Missione:** 100 turni consecutivi coerenti.
> **Partenza:** 3 turni.

## L'esperimento

17 turni in inglese con PARROT0_WORLD_FACTS=1. Risultato:

```
✓ ✓ ✓   greeting → nome → recall              (3 coerenti)
✗       "Remember what I told you?"            (crollo)
✓       aritmetica isolata
✗       "and plus 3?"                          (continuazione fallita)
✓ ✗     fatto OK / "and of Italy?"             
✓ ✗     fatto OK / "and the light bulb?"
✗ ✗     Einstein (routing KB) / "he" (coref)
✓       barzelletta isolata
✗       nome dimenticato dopo 11 turni
✓ ✓ ✓   meta: turn count + first utterance + bye (3 coerenti)
```

**Soffitto attuale: 3-5 turni coerenti consecutivi.** Fatti isolati sempre recuperabili.
La conversazione non muore mai (il motore non crasha), ma la coerenza collassa.

## I 4 punti di rottura (diagnosi)

### R1 — Continuazione ("and...")

| Esempio | Cosa succede | Perché |
|---------|-------------|--------|
| "2+2?" → "4." poi "and plus 3?" | Muro | `continue_resolve` funziona solo per aritmetica con `last_result` KB. Il "and" non viene riconosciuto come continuazione del turno precedente |
| "Capital of France?" → "Paris." poi "and of Italy?" | Muro | `decompose_and_dispatch` splitta su "and" solo DENTRO lo stesso turno, non attraverso turni |
| "Who invented the telephone?" → "Bell." poi "and the light bulb?" | Muro | Nessun meccanismo di "same intent, new entity" attraverso turni |

**Strategia:** `continue_resolve` va generalizzato. Non solo aritmetica: se il turno corrente è un frammento (inizia con "and", "e", "also", "what about"), il sistema deve recuperare l'INTENT del turno precedente e riapplicarlo con le nuove entità. Esempio: turno N era `capital_of_country(france, ?)`, turno N+1 è "and of Italy?" → `capital_of_country(italy, ?)`.

### R2 — Coreferenza attraverso turni ("he", "it", "they")

| Esempio | Cosa succede | Perché |
|---------|-------------|--------|
| "What year did Einstein die?" → muro → "What year was he born?" | "Who does 'he' refer to?" | `coref_resolve` cerca l'antecedente nelle entità del turno corrente, non nel turno precedente |
| "Do you still remember my name?" dopo 11 turni | Contesto perso | `memory` tiene il nome ma la domanda non matcha il pattern "what is my name" |

**Strategia:** `coref_resolve` deve accedere a un registro di entità menzionate nei turni recenti (già esiste `utterance/3` ma non viene usato per risolvere pronomi). Il turno N+1 con "he" dovrebbe matchare l'ultimo nome proprio maschile del turno N.

### R3 — Recall astratto / meta-contesto

| Esempio | Cosa succede | Perché |
|---------|-------------|--------|
| "Remember what I just told you?" | "Oh? What's it like?" | Nessun intento per richieste meta-contestuali |
| "Do you still remember my name?" (non "what is my name") | Perso | Pattern matching troppo rigido: "remember my name" ≠ "what is my name" |

**Strategia:** `intent_schema` per richieste meta-contestuali: "remember [what/who/where] [I/you] [told/mentioned/said]" → recupera entità dal turno N-1 o N-2 e rispondi. Il meccanismo `utterance/3` già esiste, va solo interrogato.

### R4 — Degrado del contesto nel tempo

Dopo 11 turni, "Do you still remember my name?" fallisce. Il nome "Francesco" è stato appreso al turno 2 e usato al turno 3, poi mai più referenziato per 8 turni.

**Strategia:** le entità apprese (`memory`, `user_name`) sono persistenti. Il problema è il ROUTING: la domanda "Do you still remember my name?" non matcha `kb_match("memory", ...)` perché la formulazione è diversa da "what is my name". Serve un `intent_schema` più flessibile: `[remember/know] [possessive] [name/age/city]` → query `memory(user, name, ?)`.

## Le 4 strategie (una per punto di rottura)

### S1 — Continuation intent (attacca R1)

```
Meccanismo: pre-dispatch transform "fragment_resolve"
Gate: turno inizia con "and", "e", "also", "what about", "e poi", "ed anche"
Azione: recupera l'intent dell'ultimo turno da utterance/3, estrai le nuove entità,
        riapplica lo stesso intent con le nuove entità
KB: intent_continuation("capital of", capital_of_country). — mappa l'intent
    a un predicato, così "and of Italy?" → capital_of_country(italy, ?)
```

### S2 — Coref cross-turn (attacca R2)

```
Meccanismo: estendere coref_resolve
Gate: pronome ("he", "she", "it", "they") non risolto nel turno corrente
Azione: cerca l'ultimo nome proprio nel turno N-1 (da utterance/3), sostituisci,
        re-dispatcha il turno con il pronome risolto
KB: entity_gender(Einstein, male). entity_gender(italy, neuter).
```

### S3 — Meta-context intent (attacca R3)

```
Meccanismo: nuovo intent_schema in mod_knowledge o mod_meta
Gate: "remember what I", "do you remember", "what did I just", "tell me what I"
Azione: recupera l'ultimo turno utente da utterance/3, estrai il fatto appreso,
        rispondi con "You told me that [fatto]."
KB: intent_phrase(meta_recall, "remember what i told you").
    intent_phrase(meta_recall, "do you still remember").
```

### S4 — Entity persistence + flexible routing (attacca R4)

```
Meccanismo: memory module già esiste; va allargato il routing
Gate: "[remember/know] [my/your] [name/age/city/fact]" — qualunque variante
Azione: query memory(user, $Slot, $Value) dove Slot è "name", "age", etc.
KB: memory_slot("name", name). memory_slot("age", age).
    intent_phrase(memory_recall, "do you still remember my name").
```

## Le 4 ipotesi (da verificare una alla volta)

| # | Ipotesi | Test | Misura |
|---|---------|------|--------|
| H1 | `fragment_resolve` porta a 5+ turni coerenti su fatti | "capital of France?" → "and of Italy?" → "and of Germany?" | 3 continuazioni consecutive |
| H2 | `coref_resolve` cross-turn risolve "he"/"she"/"it" | "When was Einstein born?" → "When did he die?" | 2 turni con pronome |
| H3 | `meta_recall` risponde a "remember what I told you" | Turno N: "my name is X" → Turno N+3: "remember what I told you?" | Recall dopo 3+ turni |
| H4 | Routing flessibile risponde a varianti di "what is my name" | "do you still remember my name?" / "what's my name again?" / "who am I?" | 3 varianti, stessa risposta |

## I 4 moti (ordine di attacco)

| Moto | Cosa | Impatto | Sforzo |
|------|------|---------|--------|
| M1 — S4 | Routing flessibile per recall memoria | Sblocca "remember my name" in tutte le varianti | Basso (fatti KB + 1 handler) |
| M2 — S2 | Coref cross-turn | Sblocca "he"/"she"/"it" attraverso turni | Medio (estendere coref_resolve) |
| M3 — S1 | Continuation intent | Sblocca "and of Italy?", "and plus 3?" | Medio (nuovo pre-dispatch) |
| M4 — S3 | Meta-context recall | Sblocca "remember what I told you" | Medio (nuovo intent_schema) |

## Traiettoria prevista

```
3-5 turni  →  oggi (fatti isolati + memoria nome)
8-10 turni  →  dopo M1+M2 (coref + routing flessibile)
15-20 turni →  dopo M3 (continuazione attraverso turni)
30+ turni  →  dopo M4 (meta-contesto)
50+ turni  →  composizione dei 4 moti + tuning
100 turni  →  missione: richiede contesto persistente, riassunto, dimenticanza selettiva
```

## Il salto da 50 a 100

Oltre i 50 turni servono capacità che oggi non esistono:
- **Summarization:** dopo N turni, il contesto va compresso. `utterance/3` ha una finestra limitata.
- **Forgetting:** le entità vecchie vanno degradate. Non tutto può restare in RAM.
- **Topic switching:** cambiare argomento e tornarci dopo 20 turni.
- **Self-correction:** "no, mi chiamo Marco non Francesco" → aggiornare `memory` e tutte le inferenze derivate.

Questi sono i gradini oltre il soffitto attuale, da affrontare dopo aver saturato M1-M4.
