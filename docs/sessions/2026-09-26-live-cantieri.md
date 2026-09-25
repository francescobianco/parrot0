# Live teaching, sicurezza sui cantieri — 26 settembre 2026

Transcript: [live/2026-09-26-cantieri.log](live/2026-09-26-cantieri.log). Un
processo, KB viva, 26 turni, tutti sotto 1,3 s. La sessione di fisiologia del
00:47 era rimasta aperta: archiviata senza `/save` in
[live/2026-09-26-fisiologia-3.log](live/2026-09-26-fisiologia-3.log).

**Campo**: dispositivi di protezione, cause degli incidenti, soglie d'obbligo.
**Domanda di partenza**: parrot0 sa dire che cosa protegge un DPI e ragionare su
una soglia (altezza, rumore) che in cantiere decide un obbligo?

## Che cosa regge (committato)

- **DPI con complemento**: «A hard hat protects the head from falling objects.»
  → «What protects the head?» Hard hat · «What does a hard hat protect the head
  from?» Falling objects. Trasferimento su piedi, orecchie, parapetti
  («What protects workers from falls?» → Guardrails); contrasto «What protects
  the knees?» → declina, non inventa. RI-012 in KB ha pagato: il complemento
  resta leggibile. Ora in engineering.p0, una sola relazione `protects/2`
  (il /save aveva scritto `protect/2` per i soggetti plurali).
- **Il valore di soglia**: «The minimum height of fall protection is 2 meters.»
  → «What is the minimum height of fall protection?» 2 meters. Con «for» invece
  di «of» la forma non legge.
- **Una causa al singolare**: «An unguarded edge causes a fall.» → polare al
  plurale Yes.

## I reperti (motore, per passi separati)

1. **Le forme leggono UNA parola dove la IR ha un sintagma.** «Can an eye bolt
   lift 300 kilograms?»: la IR ha `an eye bolt` / `eye_bolt`, ma `ability_polar`
   (messages.p0) ha `slot(subject)` di una parola → «eye» soggetto, «bolt lift…»
   oggetto. Stessa specie del reperto «from» di fisiologia: consumatore che
   ignora il confine della IR.
2. **Il comparatore non vede le quantità con unità.** «Is 3 greater than 2?» →
   Yes; «Is 3 meters greater than 2 meters?» → smalltalk. Senza questo nessuna
   soglia («Is fall protection required at 3 meters?») è interrogabile: con
   «the R of X is V» + confronto sarebbero due frasi.
3. **answerframe rivendica una dichiarativa plurale.** «Unguarded edges cause
   falls.» ha risposto «Slippery surfaces» (letto come domanda; mantra #17), e
   poi «What causes falls?» → «Unguarded edges» da una proposizione di lettura
   che la polare non vede: **misclaim** (mantra #7). Al singolare la stessa
   frase è appresa.
4. **Il lettore delle regole conosce solo triple.** «if the minimum height of x
   is h and n is greater than h then x is required at n» → `holds(atomo) :-
   holds(atomo), holds(atomo)`; tolta dalla ricaduta.
5. **because** non ha una forma: «workers wear a hard hat because…» perde la
   causale e salva `wore/2` (lemma sbagliato; tolto).
6. **Un'enumerazione dice una soluzione sola**: «What causes falls?» /
   «Name everything that causes falls» danno una causa ciascuna, diverse.
   Parte del motivo era KB: `cause/2` (practical-cases) duplicava `causes/2`
   (world-facts) e `causal_link` non lo vedeva — unificato (mantra #3; ora
   torri di raffreddamento → legionella → legionellosi è una catena). Resta la
   risposta a soluzione singola.
7. «X is a relation» con un nome di due parole («minimum height») è letto come
   proprietà unaria (`teach_property_phrase`); RI-010 «largest city is a
   relation» passava.
8. Nessuna forma per «forget the rule…»: la regola sbagliata si è tolta a freddo.
9. Domanda coordinata «What protects the head, the feet and the ears?» → solo
   la prima (mantra #10).
