# Live teaching, procedure per nome — 26 settembre 2026

Transcript: [live/2026-09-26-procedure.log](live/2026-09-26-procedure.log),
chiusa senza `/save` (tutto ciò che era entrato era spazzatura). Indirizzo di
F.: «conosci Collatz? no → te lo spiego → chiedi il valore», poi fattoriale e
altre; e *«mi raccomando che le procedure siano insegnate e siano in KB»*.

**Esito**: Collatz non è insegnabile parlando, oggi. Lo studio del C e della KB
che ne è seguito sta in [parrot-p0-syntax.md §17](../parrot-p0-syntax.md) e le
forme da aprire in `LEARN_PROTOCOL.md` §6-ter Q (48a–48g).

## Che cosa regge

- `what procedures do you know?` → devowel, shout, squash, quiet.
- «What is the factorial of 5?» → 120; gcd(12, 18) → 6; «How do you compute the
  factorial of a number?» → la definizione. (Il fattoriale lo calcola il C:
  `factorial/2` in `procedures.p0` non ha consumatori.)

## I reperti

1. **Misclaim**: «How many Collatz steps does 6 take to reach 1?» → «5», da
   `wordproblem` (6 − 1), subito dopo aver detto di non conoscere Collatz.
2. «Do you know the Collatz sequence?» / «the factorial?» → smalltalk: nessuna
   forma «do you know <procedura>».
3. La spiegazione in prosa («…is computed like this: if the number is even,
   divide it by two; …») non ha lettore; «To shout a word, keep only its
   consonants…» → «Learned: shout keep only».
4. `rule for collatz is start at the number. if it is even, halve. …` → passi
   illeggibili tenuti senza avviso; `apply collatz to 6` declina.
5. Il ciclo numerico a rami (`mod_agent`) ha esattamente la forma di Collatz ma è
   anonimo, e nel profilo `agi` il turno a più frasi viene spezzato prima che lo
   veda («I couldn't read «start at 6»»).
6. `coref_resolve` ha riscritto «it» con «shout» in una frase non letta e l'ha
   fatta imparare: «Learned: repeat until shout reach 1».

## Per il motore (passo separato, KB-first)

Un solo ponte: passi **numerici** nella catena `proc_step` che riusino il
vocabolario già in KB (`agent_branch_step/3`, `agent_parity_marker/2`) ed
eseguiti via `apply_operator/4`; il nome al ciclo; il rifiuto alla lezione di un
passo ignoto; la forma «do you know X?». Poi la sessione si riapre da «Do you
know the Collatz sequence?».

## Fuori sessione

`make soft-test` sfora su «what is the capital of france» (1,14–1,20 s):
`np_closer` 502 ms su 20 chiamate, da `turn_verb_before/1` (grammar.p0, commit
RI-012 delle 00:47). Non è di questa sessione; va curato prima di ogni altra.
