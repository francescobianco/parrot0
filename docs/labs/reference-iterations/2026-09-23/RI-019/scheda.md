# RI-019 — un nome di relazione che finisce con un verbo si insegna (PARZIALE)

**Lotto:** `2026-09-23` · **Stato:** completa (chiusa il 24 settembre) ·
**Classificazione:** trained

## Famiglia e contesto umano
Dati di schede tecniche: *flash point*, *melting point*, *working load* — nomi di
relazione la cui ultima parola è anche un verbo (*point*, *load*).

## Limite osservato — `prima-dialogo.txt` (binario e KB di `c2c35b11`)
`flash point is a relation` → «Learned: flash point relation.» — **un fatto
falso** (`point(flash, relation)`); l'affermazione sul valore → muro; la domanda
→ «I don't know».

## Diagnosi (trace unico) — quattro difetti in fila
1. Il lettore di classe tagliava il soggetto a «flash»: «point» è un verbo di
   relazione e chiude il sintagma. Il turno finiva al lettore della prosa e il
   muro annunciava `point(flash, relation)`.
2. La guardia RI-012 (`p0_bad_subject`) e il cancello dei concetti bocciavano
   «flash_point» come «nome che finisce con un verbo».
   **Cura unica, la regola di RI-012 detta per intero: una frase ha UN verbo
   finito; quando il soggetto è seguito subito dalla copula, il verbo finito è
   la copula** — la parola prima di lei non chiude il sintagma e non rende il
   soggetto un non-concetto (`p0_bad_subject_ex` / `p0_atom_is_concept_ex` con
   `before_copula`, lettore di classe e ritiro).
3. `quantity` rubava l'affermazione («has 100 degrees celsius») a una relazione
   insegnata: `faculty_yield_when(quantity, open, turn_relation_noun_reading)`
   (grammar.p0). E il dispatcher non leggeva affatto `faculty_yield_when/3` per
   le facoltà del registro (solo per chi lo chiedeva da sé): aggiunta la quarta
   famiglia all'elenco delle condotte governate.
4. `knowledge` cedeva la domanda per «quasi una lezione»: il testo fisso «what is
   the» di `decision_value_question` — una forma che LEGGE. Ora in KB: una lezione
   scrive, una domanda legge (`form_only_reads/1`, conduct-lessons.p0).

## Certificazione — `certificazione-dialogo.txt`
«38 degrees celsius.» (Jet A, ASTM D1655) · transfer 1: melting point of tin →
232 °C · transfer 2 (altro verbo, altro dominio): working load of an M10 eye
bolt → 230 kg (DIN 580) · contrasti: «The needle points north.» e «The crane
loads containers…» leggono ancora il verbo; capacitors invariato · ablazione:
`forget that flash point is a relation` → la domanda non trova più il valore
(prima della correzione del ritiro scriveva un fatto falso: corretto).

## Salvataggio e processo nuovo (24 settembre) — `salvataggio.txt`, `processo-nuovo-dialogo.txt`
Sessione pulita sullo stesso binario del commit `5f58c676` (nessuna modifica fra
certificazione e salvataggio): le tre lezioni e i tre valori, poi `/save`.
Diff classificato: `relation(flash_point)`, `relation(melting_point)`,
`relation(working_load)` in messages.p0 accanto alle sorelle (**L=3**);
`flash_point_of(jet_a_fuel, 38_degrees_celsius)`, `working_load_of(m10_eye_bolt,
230_kilograms)` spostati dalla ricaduta a engineering.p0 e
`melting_point_of(tin, 232_degrees_celsius)` a science-nature.p0, accanto a
`boils_at/freezes_at` (**W=3**); provenienza e transcript (P). **C=0, X=0.**
Processo nuovo: i tre valori rispondono; replay RI-016 (step up/down), RI-017
(heat-resistant glove), RI-018 (5 V, 13 A) e controlli (capacitors, Paris) verdi.

## Limiti e prossimo problema
- «What is the boiling point of water?» → definizione dell'acqua;
  la KB ha `boils_at(water, …)` ma nessun ponte da «boiling point».
- La regola «il verbo finito è la copula» non è ancora applicata al lettore
  della prosa/IR.
Banco L2 60/60 (128 s) sullo stesso albero.
