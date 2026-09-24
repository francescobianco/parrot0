# RI-023 — una sigla tecnica si insegna, si conserva e non si mangia le parole comuni

**Lotto:** `2026-09-24` · **Stato:** completa · **Classificazione:** trained

## Famiglia e contesto umano
I testi tecnici parlano per sigle: LED, DC, AC, PWM, MOSFET. Chi legge una scheda
chiede «What does an LED emit?» o «Do capacitors block DC?» e si aspetta che la
sigla valga come il nome esteso — che la KB spesso conosce già («capacitors block
direct current», lotto del 21 settembre).

## Stato iniziale
Commit `b73cea1c` (RI-022), profilo `agi`. `stato.txt`.

## Limite osservato — `prima-dialogo.txt` (binario e KB di `b73cea1c`)
- `LED is short for light-emitting diode` → *«Hmm, I don't know about light-emitting
  yet»*: la lezione di sigla (gen507/74) non entrava;
- «What does LED stand for?» → «nothing I hold says led **stood** for» (letto come
  il passato di *lead*);
- «Do capacitors block DC?» → «I don't know» con `block(capacitors, direct_current)` in KB.

## Diagnosi con il trace unico — quattro difetti, tre dove il trace taceva
1. **`knowledge` cedeva la lezione come «quasi una lezione».** Il trace diceva solo
   `lesson_almost_turn reading=HIT`. Ora ogni vista di cessione può dichiarare il
   suo testimone (`yield_witness/2`, KB) e il trace lo stampa: `because teach_abbrev
   on is short for`. Causa: dal punto 4 di RI-019 il dispatcher valutava
   `faculty_yield_when/3` all'ingresso di **ogni** facoltà del registro — anche di
   `knowledge`, che le sue forme tarde le prova *dentro* e decide la cessione *dopo*
   (il riordino del 21 settembre). Il difetto già curato era tornato. Cura:
   `faculty_yields_after_forms(knowledge)` in KB, e il dispatcher non la anticipa
   (la riga di trace lo dice).
2. **Il contrasto rosso: la sigla si mangiava il verbo.** Dopo la lezione «The guide
   led the climbers to the summit.» diventava «the guide light-emitting diode the
   climbers». La conoscenza che mancava: *una sigla che è anche una forma di parola
   si riconosce solo scritta in maiuscolo*. `alias_needs_capitals/1` (KB, legge
   `irregular_verb_form/2` e `verb_form/3` che già dicono che «led» è un verbo);
   la canonizzazione guarda la maiuscola nel testo grezzo del turno (`canon_raw`),
   solo per la parola che ha combaciato. Trace: `read.canon alias «led» needs
   capitals: not in capitals, kept`.
   ⚠ La prima stesura metteva la guardia dentro la regola `phrase_canon/2`, che la
   canonizzazione enumera ~105 volte per turno: il profilo di `/debug` l'ha misurata
   a **+270 ms per turno** (21 → 294 ms su `phrase_canon`). Spostata al momento
   dell'uso: turni di `facts.p0t` alla pari con la base (666/580/628 ms contro
   668/612/611, stesse query).
3. **Il ritiro non esisteva** («forget that DC is short for direct current» → muro):
   forma `forget_abbrev` in KB, sulla vista `said` (nel testo canonizzato la sigla
   è già riscritta).
4. **La sigla insegnata non arrivava su disco.** `/save` taceva su `entity_alias`.
   La riga `form op …` del trace ora dice lo strato del lettore: `teach_abbrev`
   scriveva in **reflective** (mai salvato), la contrazione di RI-022 in
   **session** — dipendeva da *dove* girava il lettore delle forme, non dalla
   lezione. Cura (RI-004 detto per ogni forma): l'atto `assert` di una forma
   dichiarata scrive in sessione, salvo `turn_form_effect_origin(Form, reflective)`.

## Curriculum (fonti: IEC 60050 / uso tecnico comune)
1. `LED is short for light-emitting diode` (L)
2. `A light-emitting diode emits light.` (W)
3. `DC is short for direct current` (L)

## Certificazione sul meccanismo fermo — `certificazione-dialogo.txt`
- prima delle lezioni (stesso meccanismo): «I don't know … led stood for», «I don't understand»
- stimolo: «What does LED stand for?» → light-emitting diode; «What does an LED emit?» → light
- transfer 1 (altra relazione, conoscenza pre-esistente): «Do capacitors block DC?»
  → Yes; «What do capacitors block?» → Direct current
- transfer 2 (altra forma della domanda): «What does an LED emit?» dopo il fatto
  detto sul nome esteso
- contrasto: «The guide led the climbers to the summit.» non riscritto (declina
  senza scrivere); «PWM is short for» (lezione incompleta) → come la base
- ablazione: `forget that DC is short for direct current` → «Do capacitors block
  DC?» torna «I don't know»; il controllo indipendente (LED) resta.

## Salvataggio e processo nuovo
`/save` 22 clausole: `entity_alias(led, …)`, `entity_alias(dc, …)` in
machinery/lexical-relations.p0 accanto a `laser`, `crispr` (**L=2**);
`emits(light-emitting_diode, light)` in world-facts.p0 accanto agli altri `emits`
(**W=1**); proposizione, provenienza, transcript (P). **X=0.** Processo nuovo:
sigle, DC, contrasto «led», replay RI-016..022 verde.

## Una regressione di RI-020, trovata qui e curata
I banchi puntuali sulla KB viva hanno mostrato `taught_lesson_form.p0t` a 35/42
contro 40/42 della base di inizio sessione (`34193c7e`, misurata in un worktree
separato; i 2 rossi alle righe 114/130 preesistono). Causa: l'ancora per nome
di RI-020 (`relation_named_by/2`) si prendeva «x is a y» → `is_a`, e «x belongs
to the kind y means x is a y» diventava una costruzione invece di una forma.
Provato prima l'ordine inverso (la forma prima dell'ancora): rompeva RI-020 sul
congelamento («x freezes at y» e' leggibile da una forma, e la domanda non usa
una rilettura). Cura vera, in KB: una superficie nomina una relazione solo se ha
una **parola piena** (`surface_has_content_word/1`); «is a» è copula più
articolo. Dopo: 40/42 come la base; RI-020 riverificato
(`replay-ri020-dialogo.txt`: ritiro e reinsegnamento, acqua e azoto).

## Verifiche
Banchi puntuali: `l2_reading_choices` 60/60, `taught_illocution` 21/21,
`taught_lesson_form` 40/42 (= base), `forms_as_objects` 5/13 (= base, rossi
preesistenti). soft-test: test verdi; tempo 15–18 s sotto carico, come la base;
profilo per turno di `facts.p0t` alla pari con la base.

## Limiti e prossimo problema
- «What does the LED on the charger emit?» → muro: un sintagma con un complemento
  preposizionale non si lega (la stessa coda di «work in progress»).
- La risposta «light.» in minuscolo quando la domanda passa dalla sigla, «Light.»
  sul nome esteso: la resa non è uniforme.
- `alias_needs_capitals/1` conosce solo le forme verbali: una sigla uguale a un
  nome comune («CAT», «SUN») non è ancora protetta.
