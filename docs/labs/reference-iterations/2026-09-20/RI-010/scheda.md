# RI-010 — un nome di relazione può essere di più parole

**ID, famiglia e contesto umano.** Famiglia: *il nome della relazione*, portato
dove si rompe — due parole invece di una. Contesto umano: quasi tutte le
relazioni che una persona nomina hanno un nome composto («la città più grande»,
«la lingua ufficiale», «il luogo di nascita»).

**Commit/stato iniziale.** `0ceee46e` (RI-009 chiusa), profilo `agi`, `en`.

**Stimolo congelato.**

```text
What is the largest city of Turkey?
```

**Criterio.** Istanbul, una volta insegnato. E **mai** la capitale.

**Risposta iniziale e limite osservato** (`r1-baseline.json`):

| turno | risposta |
|---|---|
| **What is the largest city of Turkey?** | **«Ankara.»** ✘ — la capitale, detta con sicurezza |
| largest city is a relation | «Learned» ✔ |
| The largest city of Turkey is Istanbul. | *(nessuna risposta: muro)* |

Due guasti sotto una sola superficie. La regola che apre il nome costruiva il
predicato con `concat_atoms($Noun, "_of")`: per un nome di due parole ne
usciva `largest city_of`, **un atomo con uno spazio dentro** che nessuno può
interrogare. E il lettore storico di «the R of X» prende UN token come nome
(articolo, nome, «of»), quindi con due parole non combaciava affatto: il turno
finiva a chi risponde sulle capitali.

## R4 — la cura

1. **KB (`grammar.p0`)**: il predicato si costruisce unendo le parole con
   l'underscore, e la stessa riga lega il cassetto (`largest_city_of`) alla sua
   **maniglia** (la superficie «largest city») — perché asserire e interrogare
   non possano divergere. La regola a una parola resta: costa meno e copre il
   caso comune.
2. **KB (`messages.p0`) + un atto**: la forma `ask_noun_of` dice dove stanno i
   pezzi di «what is the <nome> <nome> of <entità>?», e l'atto ricompone il
   nome e chiede a `relation_noun/2`. Riconosciuta la domanda, il turno resta
   suo anche quando non sa: cederlo riportava a galla «Ankara» — il difetto di
   partenza — **proprio dopo** che il maestro aveva ritirato il fatto giusto.
3. **Un difetto di dati, trovato insegnando.** «The largest city of Turkey is
   Istanbul.» tornava «… is constantinople»: `also_known_as(constantinople,
   istanbul)` faceva del nome storico il concetto, al contrario della riga
   vicina (`also_known_as(mumbai, bombay)`), e `entity_alias/2` legge il primo
   argomento come concetto. Corretta. Il verso opposto continua a funzionare
   («What is another name for Istanbul?» → «Constantinople.»).

## R5 — certificazione (`r5-certification.json`)

| prova | esito |
|---|---|
| prima delle lezioni | «Ankara.» |
| lezione + fatto | «Learned: the largest city of turkey is istanbul.» |
| **stimolo** | **«istanbul.»** |
| **contrasto** | «What is the capital of Turkey?» → «Ankara.» — la relazione vicina non si muove |
| TR1 — altra entità | `The largest city of Germany is Berlin.` → «berlin.» |
| TR2 — altra formulazione, niente insegnato | «**Which** is the largest city of Turkey?» → «istanbul.» |
| **ablazione** (ritiro di RI-006) | «forget that the largest city of Turkey is Istanbul.» → la domanda rende **il muro onesto**, non più «Ankara» |
| controllo indipendente | «What is the largest city of Germany?» → «berlin.» |

Fonti: `en.wikipedia.org/wiki/Istanbul` («Istanbul is the largest city in
Turkey»); Berlino è la città più popolosa della Germania.

## R6 — conteggio e processo nuovo

```text
parrot0: routed 20 clause(s) into the KB tree
```

| categoria | n | clausole |
|---|---:|---|
| `W` fatti veri | **2** | `largest_city_of(turkey, istanbul)`, `largest_city_of(germany, berlin)` |
| `C` | 1 | `relation(largest_city)` — la lezione che apre il nome |
| `X` | 0 | — |

**Processo nuovo** (`r6-fresh-process.json`): 5/5 — i due fatti, il contrasto
sulla capitale, e i replay di RI-009 (il piano del viaggio) e RI-007 («north
sea»). `make soft-test` verde in 11 s.

## Limiti residui — misurati, non supposti

1. **Le risposte inscatolate sui primati rubano la superficie.** «largest lake
   is a relation» → «The Caspian Sea is the largest lake by area.»; «longest
   river is a relation» → la risposta sul Nilo; «highest mountain …» → quella
   sull'Everest. La lezione non arriva mai. È la specie di RI-009 (una risposta
   pertinente a una situazione data a tutte), con membri diversi: «largest
   city» funziona solo perché per quel nome non esiste una risposta in scatola.
2. **Un nome composto da due sostantivi legge un'altra forma**: «birth place is
   a relation» rende «Learned: birth place relation» — non `relation("birth
   place")`. La coppia aggettivo+nome funziona, nome+nome no.
3. `What is the largest city of Japan?` (niente insegnato) riceve una risposta
   sulla cultura Heian invece del muro: ancora la specie n. 1.

**Stato dell'iterazione:** completa.
**Classificazione del training:** `trained` (W = 2, C = 1, X = 0).
