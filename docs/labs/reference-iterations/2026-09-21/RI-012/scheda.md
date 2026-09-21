# RI-012 — una frase ha un verbo finito, e il secondo non lo è

**ID, famiglia e contesto umano.** Famiglia: *il modificatore omografo di un
verbo*. Contesto umano: mezza terminologia tecnica è fatta così — «direct
current», «drive shaft», «lead time», «press fit» — e chi la scrive non sta
usando un verbo.

**Commit/stato iniziale.** `723072fd` (RI-011 chiusa), profilo `agi`, `en`.

**Stimolo congelato** (preambolo + domanda):

```text
Capacitors block direct current.
What do capacitors block?
```

**Criterio.** «Direct current», intero. Fonte:
`en.wikipedia.org/wiki/Capacitor` — «capacitors are widely used in electronic
circuits for blocking direct current while allowing alternating current to
pass».

**Risposta iniziale e limite osservato** (`r1-baseline.json`, `r5a-before.json`):

```text
> Capacitors block direct current.
I didn't keep that: «capacitors_block» e «current» don't read as things I can hold a fact about.
```

**La misura che isola la causa**, e che vale più di qualunque ispezione:

| frase | esito |
|---|---|
| Capacitors block **electricity**. | Learned ✔ (oggetto di una parola) |
| Capacitors block **alternating current**. | Learned ✔ (oggetto di due parole) |
| Capacitors block **direct current**. | respinta ✘ |

Due parole in entrambi i casi: cambia solo che **«direct» è un verbo di
relazione** (dirigere). Verificato con una query alla KB:
`relation_verb(direct)` è vero, `relation_verb(check)` no. La regola
`np_closer($V) :- relation_verb($V)` (grammar.p0, gen513) lo faceva chiudere il
sintagma: uno schema «@S direct @O» combaciava con soggetto «capacitors block»
e oggetto «current», e il fatto veniva respinto dal cancello di pulizia.

## R4 — la cura, in due pezzi

1. **Dentro un turno, il primo verbo è il verbo della frase.** Quelli che
   vengono dopo, senza congiunzione, sono modificatori. La vista dei chiusori
   si ricostruisce già a ogni turno, quindi la potatura si paga **una volta**:
   si toglie dalla vista di questo turno chi compare solo **dopo** un altro
   chiusore. Nessuna parola nel C — la classe resta `np_closer`, conoscenza.
2. **Un sintagma non finisce con un verbo nudo dopo un nome.** «capacitors
   block» non è un soggetto; «the block» sì, perché il determinante davanti dice
   che lì il verbo è il nome. Quali parole siano verbi di relazione e quali
   determinanti lo dice la KB.

## R5 — certificazione (`r5-certification.json`)

| prova | esito |
|---|---|
| **stimolo** | «What do capacitors block?» → **«Direct current.»** |
| l'altra metà dello stesso fatto tecnico | «Capacitors pass alternating current.» → «Alternating current.» |
| TR1 — altro soggetto, altro verbo, stesso omografo | «Rectifiers produce direct current.» → «Direct current.» |
| TR2 — altro soggetto ancora | «Batteries supply direct current.» → «Direct current.» |
| TR3 — **altro dominio** (lean manufacturing) | «Kanban limits work in progress.» → «Work.» (parziale: la coda preposizionale si perde) |
| **ablazione** (ritiro di RI-006) | «forget that capacitors block direct current.» → la domanda perde la risposta |
| controllo indipendente | «What do rectifiers produce?» → «Direct current.» |

Prima della cura, le tre frasi con «direct current» erano **tutte e tre**
respinte con lo stesso messaggio (`r5a-before.json`, misurato ricompilando
senza la modifica).

## R6 — conteggio e processo nuovo

```text
parrot0: routed 49 clause(s) into the KB tree
```

| categoria | n | clausole |
|---|---:|---|
| `W` fatti veri | **5** | `block(capacitors, direct_current)`, `pass(capacitors, alternating_current)`, `produce(rectifiers, direct_current)`, `supply(batteries, direct_current)`, `limit(kanban, work)` |
| `P` provenienza | molte | `semantic_*`, `proposition_source_record`, `fact_source`, `reading_fact` |
| `X` | 0 | — |

**Processo nuovo** (`r6-fresh-process.json`): 5/5, con il replay di RI-011
(«Is austenitic stainless steel magnetic?» → «No.») e del lettore storico
(«Is a tiger a mammal?» → «Yes.»). `make soft-test` verde in 14 s;
`facts.p0t`, `basics.p0t` verdi; `instance_under_constraint.p0t` 29/3, la
misura di prima.

## ⚠ Un errore mio, trovato qui e corretto qui

Il processo nuovo di questo giro ha mostrato che **«Is austenitic stainless
steel magnetic?» era tornato a essere un muro**: il commit di RI-011 non
conteneva i fatti appresi. Li aveva portati via un `git checkout -- kb/`
fatto per ripulire i residui di `persist.p0t`, che scrive nella KB vera. La
prova del processo nuovo di RI-011 era reale nel momento in cui è stata fatta;
il commit no. I quattro fatti sono stati **reinsegnati e risalvati** in questo
commit, e il processo nuovo qui sopra li verifica. La regola che ne esce: dopo
una suite che tocca la KB, si ripulisce **prima** di salvare, mai dopo.

## Limiti residui

1. La coda preposizionale di un oggetto si perde: «work in progress» diventa
   `work`.
2. Il cancello di pulizia respinge fatti su entità nuove che la prosa tecnica
   introduce di continuo («Engineers direct the current.» → respinta su
   «engineers»): è un candidato a sé.
3. La potatura vale dentro un turno: una frase con due proposizioni coordinate
   davvero («X blocks A but passes B») non è stata aperta.

**Stato dell'iterazione:** completa.
**Classificazione del training:** `trained` (W = 5, X = 0).
