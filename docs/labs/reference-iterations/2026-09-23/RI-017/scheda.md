# RI-017 — che il trattino unisca una parola si insegna

**Lotto:** `2026-09-23` · **Stato:** completa · **Classificazione:** trained

## Famiglia e contesto umano
Ortografia tecnica inglese: i modificatori composti col trattino — *step-down
transformer*, *double-pole switch*, *heat-resistant glove* — sono una parola sola.
Spezzati, il sintagma perde il modificatore e la risposta generalizza a torto
(«transformer», «resistant glove»). È il candidato nato da RI-016.

## Stato iniziale
Commit `a7ca0b1a` (dopo RI-016), profilo `agi`, KB completa. `stato.txt`.

## Stimolo e criterio
`A step-down transformer steps down the voltage.` → `What steps down the voltage?`
deve nominare il *step-down transformer*, non «transformer».

## Limite osservato — `prima-dialogo.txt`
«Buck converter and transformer.» Nel trace unico: la IR vedeva i token
`a|step|down|transformer` — «step» è un verbo, il sintagma finiva lì, e la IR
scriveva `step_down(transformer, voltage)`.

## Diagnosi e supporto generale
Tre confini di parola nel C (spazi, token del turno, token della IR), il terzo e il
secondo con regole compilate («punto fra cifre»). Nessuno sapeva che il trattino
fra due lettere unisce. Cura: la classe insegnabile `word_joiner/1`
(`class_surface(word_joiner, word joiner)`, kb/core/input.p0), letta dai due
tokenizzatori solo fra lettere; nessun membro scritto a mano. Più: la conferma
del lettore delle menzioni usa il nome detto («word joiner») e non la chiave; i
token della IR entrano nel trace unico (`ir current_turn …`), letti da
`input_structure_publish` senza ricalcolo.

## Curriculum
1. `"-" is a word joiner` (L)
2. `A step-down transformer steps down the voltage.` (W)
3. trasferimenti: `A double-pole switch disconnects both conductors.`,
   `A heat-resistant glove protects the hand.` (W)

## Certificazione (meccanismo fermo) — `certificazione-dialogo.txt`
dopo la lezione: «Buck converter and step-down transformer.» · transfer 1
(elettrotecnica): «What disconnects both conductors?» → Double-pole switch ·
transfer 2 (sicurezza): «What protects the hand?» → Heat-resistant glove; senza la
lezione «Resistant glove.» (`transfer-senza-lezione-dialogo.txt`) · contrasti:
`30-375` resta `30|375` nella IR, `3.14 / 3.41` → 3.41 · ablazione: `forget that
"-" is a word joiner` → «A step-down regulator…» torna a «regulator».

## Salvataggio e processo nuovo — `processo-nuovo-dialogo.txt`
`/save` 39 clausole: `word_joiner(-)` (L, spostato accanto alla sua classe),
`step_down(step-down_transformer, voltage)`, `disconnect(double-pole_switch,
both_conductors)`, `protects(heat-resistant_glove, hand)` (W=3; due dalla
ricaduta learned.p0 spostate in engineering.p0), provenienza e proposizioni (P).
Processo nuovo: tutto risponde; trasferimento su frase mai vista («A well-known
protocol secures the link.» → Well-known protocol). W=3, L=1, C=0, X=0.

## Replay (R7)
RI-016 (step up/down, polare), RI-012 (capacitors), decimali, «conosci xdebug»:
invariati. Banco L2 60/60.

## ⚠ soft-test
16 s su 15: **fallito**, e non rilanciato per aggirarlo. Ma nella stessa ora
l'albero di base `015ce009` (senza RI-016/017) fallisce anch'esso (un turno a
1,05 s): la macchina era più lenta che in mattinata (9–12 s). I due file misurati
senza il boot costano come la base (basics ~3,6 s contro 2,9; facts ~12 contro 12).
Da rimisurare a macchina scarica; la causa non è attribuita a RI-017 senza prova.

## Limiti e prossimo problema
- **Candidato:** «What does a band filter block?» → muro, anche senza trattino,
  mentre «What do capacitors block?» risponde: la domanda sull'oggetto con un
  soggetto di più parole preceduto dall'articolo.
- **Candidato:** la cue `pass` combacia dentro «high-pass» (mantra #8): le cue
  delle domande vanno confrontate a parola intera.
- «Is a step-down transformer a transformer?» chiede l'inferenza sulla testa del
  sintagma: non affrontata.
