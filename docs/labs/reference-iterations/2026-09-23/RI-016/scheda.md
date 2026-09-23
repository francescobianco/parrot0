# RI-016 — una particella che cambia il significato del verbo si insegna

**Lotto:** `2026-09-23` · **Stato:** completa · **Classificazione:** trained

## Famiglia e contesto umano
Elettronica di potenza e biochimica: chi legge un manuale tecnico incontra verbi
con particella avverbiale — *step up*, *step down*, *break down* — dove la
particella È il significato. Un trasformatore flyback (CRT) alza la tensione, un
convertitore buck la abbassa: confonderli è un errore di sostanza.

## Stato iniziale
Commit di partenza `015ce009`, profilo `agi`, KB completa. Binario: `stato.txt`.

## Stimolo, criterio, fonti
`A flyback transformer steps up the voltage.` poi `What steps up the voltage?` —
deve rispondere il flyback, e non un dispositivo che abbassa. Fonti: il flyback
genera l'alta tensione anodica dei CRT; il buck è un DC-DC step-down; l'amilasi
idrolizza l'amido. (F., durante l'iterazione: «esistono anche trasformatori step
up»: la prima formulazione, «a transformer steps down the voltage», era una
generalizzazione falsa ed è stata sostituita.)

## Limite osservato
Prima: «I don't have the steps for voltage yet…» — il lettore delle procedure
rubava il turno sulla cue `steps`. Dopo averlo fatto cedere e insegnato il verbo,
emergeva il difetto vero: `step(transformer, voltage)` per «steps down» **e** per
«steps up», e «What steps up the voltage?» rispondeva «Transformer» — un falso.

## Curriculum
1. `up is an adverbial particle`, `down is an adverbial particle` (L: classe di parola)
2. `step up is a relation verb`, `step down is a relation verb` (L: `relation_verb(step)`, `verb_particle(step, up|down)`)
3. `A flyback transformer steps up the voltage.`, `A buck converter steps down the voltage.` (W)
4. trasferimento: `break down is a relation verb` (L: `verb_particle(break, down)`), `Amylase breaks down starch.` (W)

## Diagnosi e supporti generali (tutti trovati col trace unico del turno)
- `faculty_yield_force(process_steps, open, assertion)` — chi afferma non chiede i passi (KB).
- `p0_class_by_surface` — il nome detto di una classe è `class_surface/2`
  («verb particle» → `verb_particle_word`, non `verb_particle/2`).
- il lettore di classe e il ritiro accettano come soggetto la parola menzionata
  subito prima della copula quando il turno parla di una parola (`turn_mentions_word/1`).
- il ritiro non salta come «complementatore» la parola che precede la copula.
- `verb_particle_word(P) :- adverbial_particle(P)` — una particella si insegna prima del suo verbo.
- particella avverbiale ⇒ predicato composto in `extract_frame`, in `answer_frame`
  e nell'operatore della IR (`operator_particle_follows/2`).
- difetto preesistente curato: superficie e particella da due enumerazioni
  indipendenti (`break down` accoppiato a `into` → `break`).
- `p0_polar_reply` prova anche le entità di più parole del turno (`turn_entity_named/2`).

## Certificazione (meccanismo fermo, processo nuovo) — `dialogo.txt`
prima: muro · dopo: «Flyback transformer.» / «Buck converter.» · transfer 1
(polare): «Does a flyback transformer step up the voltage?» → Yes; sul buck →
«I don't know» (nessun No inventato) · transfer 2 (altro dominio): «What breaks
down starch?» → Amylase, `break_down(amylase, starch)` · contrasti: «flow into»
resta `flow`; «What are the steps to replace a fuse?» resta una procedura ·
ablazione: `forget that up is an adverbial particle` → «steps up» torna alla
lettura per radice e lo stimolo non risponde più il flyback.

## Salvataggio e processo nuovo — `processo-nuovo-dialogo.txt`
`/save` 56 clausole; nel diff: `adverbial_particle(up|down)` (L) + nome di classe,
`step_up(flyback_transformer, voltage)`, `step_down(buck_converter, voltage)`,
`break_down(amylase, starch)` (W = 3), provenienza e proposizioni IR (P). Tre
fatti erano finiti nella ricaduta `learned.p0`: spostati accanto ai simili
(engineering, science-nature). Processo nuovo: tutto risponde senza reinsegnare;
trasferimento su frase mai vista («A DC-DC boost converter steps up the voltage.»).
W=3, L=6 (`adverbial_particle` ×2, `relation_verb(step)`, `verb_particle` ×3, instradati in english-grammar/verbs.p0 e predication.p0), C=0, X=0.

## Replay (R7) — `replay-dialogo.txt`
RI-011…RI-014 e i fix del giorno: invariati. soft-test 11 s; L2 60/60.

## Limiti e prossimo problema
- **RI-017 candidato:** la IR perde il modificatore con trattino — «A step-down
  transformer steps down the voltage.» → «What steps down the voltage?» risponde
  «Transformer» (generalizzazione), mentre il lettore dei frame tiene
  «step-down transformer». Confine del sintagma: L2.
- costo: dopo quattro lezioni sui verbi la vista `extract_frame` si rifà per
  intero (~2 s, visibile nel trace come `view extract_frame`).
- la resa «I no longer treat «up» as a adverbial particle» sbaglia l'articolo.
