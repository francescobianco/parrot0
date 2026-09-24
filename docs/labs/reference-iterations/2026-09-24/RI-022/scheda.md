# RI-022 — una contrazione si insegna, e le riscritture vanno al punto fisso

**Lotto:** `2026-09-24` · **Stato:** completa · **Classificazione:** meta-capability-only
(W=0: la capacità è linguistica; le applicazioni sono certificate su conoscenza
vera già in KB.)

## Famiglia e contesto umano
Si scrive «I'd like to know …», non «I would like to know …». La KB conosceva le
contrazioni solo come righe scritte a mano (`function_word("what's", "what is")`):
una contrazione nuova non si poteva insegnare parlando.

## Stato iniziale
Commit `ea18201a` (RI-021), profilo `agi`. `stato.txt`.

## Limite osservato — `prima-dialogo.txt`
Quattro turni, **quattro fatti falsi** in sessione:
- «I'd like to know the melting point of tin.» → «Learned: i'd like to know the melting point of tin.»
- «I'd like to learn welding.» → «Learned: i'd like to learn welding.»
- le due lezioni che una persona darebbe — `"i'd like" is a contraction of "i would
  like"`, `"i'd like" is short for "i would like"` — scrivevano altri due fatti
  falsi («"i'd like" is a contraction of i would like»).

## Diagnosi con il trace unico
- La lezione non aveva una forma; la classe giusta c'era: `phrase_canon/2`, la
  riscrittura di superficie a più parole che la canonizzazione applica per tutti.
- Con la forma, il trace mostrava `read.canon «i'd like to know …» -> «i would like
  to know …»` e si fermava lì: la parafrasi di RI-021 («i would like to know» →
  «tell me») non si applicava, perché la canonizzazione faceva **un passo solo**.
  Ora ogni passo in più ha la sua riga (`read.canon again «…» -> «…»`).
- Il ritiro cadeva nella trappola già scritta nel piano (§6.4): una forma che
  comincia con uno span prende anche «forget that X is a contraction of Y», e
  scriveva la contrazione fasulla «forget that i'd like». Curato in KB con una
  classe dello slot (`contraction_said_plain/1`).

## Supporti generali
- `teach_contraction_en` / `forget_contraction_en` (lexicon.p0, accanto alle
  contrazioni): atti esistenti `op(assert|retract, phrase_canon, …)`, **zero C**.
- La canonizzazione va al punto fisso (al massimo tre passi, solo se il primo ha
  cambiato qualcosa): una riscrittura insegnata può produrre la superficie di
  un'altra. Misura deterministica: sui 16 turni del soft-test il passo in più non
  scatta mai.

**Lezione nuova resa possibile:** qualunque contrazione, detta con il contesto
che la rende non ambigua («'d» è *would* in «i'd like», *had* in «i'd seen»).

## Curriculum
1. `"i'd like" is a contraction of "i would like"` (L)

## Certificazione sul meccanismo fermo — `certificazione-dialogo.txt`
- prima della lezione (stesso meccanismo): il fatto falso
- stimolo: «I'd like to know the melting point of tin.» → 232 degrees celsius
- transfer 1 (altra relazione): flash point del Jet A → 38 °C
- transfer 2 (composizione con RI-020 e con una relazione nativa): boiling point
  dell'etanolo → 78 °C; capitale di Francia → Paris
- contrasto: «I'd seen the report before the meeting.» non viene riscritto
  (`read.canon` identico); declina senza scrivere niente
- ablazione: `forget that "i'd like" is a contraction of "i would like"` → torna il
  fatto falso; il controllo indipendente («I would like to know …» → 232 °C) resta.

## Salvataggio e processo nuovo
`/save` 8 clausole: `phrase_canon("i'd like", "i would like")` (spostata dalla
ricaduta di spelling.p0 accanto alle contrazioni in lexicon.p0) — **L=1**, W=0,
C=0, transcript; **X=0**. Processo nuovo: stimolo, fusibile via forma contratta,
replay RI-016..021 verde.

## Verifiche
`make soft-test`: test sempre verdi; tempo 15–17 s sotto carico (Chrome, load
2–2,7). La base `ea18201a` nelle stesse condizioni: 15 e 17 s. A/B in CPU utente
sulle forme nuove: dentro il rumore (15,5–17,2 contro 16,2–16,9). Banco L2 60/60.

## Limiti e prossimo problema
- «I'd like to learn welding.» dopo la lezione non apre l'attività di
  apprendimento come «I would like to learn welding.»: l'iniziativa di dialogo
  legge i token grezzi della IR (`i|d|like`), non il testo canonizzato. Due letture
  dello stesso turno che non si accordano (D33): la IR si costruisce prima della
  canonizzazione.
- La lezione di sigla («X is short for Y») scrive ancora un fatto falso quando la
  superficie è citata o ha un apostrofo.
