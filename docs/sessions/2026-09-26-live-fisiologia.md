# Live teaching, fisiologia — 26 settembre 2026

Prima sessione vera di [live-teaching.md](../plans/live-teaching.md). Il
transcript è in [live/2026-09-26-fisiologia.log](live/2026-09-26-fisiologia.log).
La sessione di prova del 25 (cucina) era rimasta aperta: è archiviata in
[live/2026-09-25-cucina.log](live/2026-09-25-cucina.log), chiusa senza `/save`.

**Campo**: che cosa fanno gli organi (kb/facts/physiology.p0, otto fatti veri).
**Esito**: nessuna lezione ha retto. Tre strade rotte, tutte di motore, quindi
sessione chiusa per la regola 6 e senza `/save` (l'unico fatto imparato era
registrato male).

## Che cosa regge

- Le wh- con il verbo della relazione: «What does the heart pump?» → Blood;
  «What do the kidneys filter?» → Blood.
- Le polari sui fatti di base, anche con il «do» plurale (§30): «Do the kidneys
  filter blood?» → Yes.
- Un fatto nuovo si impara con una frase: «The lungs absorb oxygen from the
  air.» → «What do the lungs absorb?» Oxygen · «What absorbs oxygen?» Lungs ·
  «Where … from?» Air.

## I reperti

1. **«What does X do?» mura per ogni organo** (muro cieco, nessuna lacuna
   registrata). Il sapere c'è: `about(X, V, Y) :- relation_verb(V), holds(V, X, Y)`
   (procedures.p0) e `pumps` è un `relation_verb`. La forma
   `turn_function_question` (function-questions.p0) riconosce la domanda, ma la
   risposta la cerca solo in `topic_definition`, non nei fatti di cui X è
   soggetto. È una strada in **KB**, non in C: si apre con una clausola accanto a
   quella esistente. Non si poteva fare parlando, perché la KB caricata non
   cambia a sessione aperta.
2. **Il complemento incollato all'oggetto.** «The lungs absorb oxygen from the
   air» → `absorb(lungs, oxygen_from_the_air)`. La polare «Do the lungs absorb
   oxygen?» dà un muro falso («no fact I hold decides…»). La lezione L2 «end the
   previous noun phrase before from» **riesce nella IR** (alla seconda prova
   parrot0 dice che non c'è più un sintagma non corretto), ma la rilettura
   registra lo stesso fatto incollato: chi scrive il fatto è un lettore in C
   (percorso knowledge/learn, `10-memory-knowledge.c`) che non consulta i confini
   della IR. È la stessa specie del reperto «a cup» del 25 (`cup_count` legge
   `turn_word` e non il testo canonico): **lezione accettata, effetto nullo,
   perché il consumatore legge le parole grezze.**
3. **«What do you know about the liver?» → «I don't know anything about the
   liver.»** È falso. Senza articolo risponde («about heart» → `pumps(heart,
   blood); heart is muscular pump…`). La traccia mostra `refer mentioned
   the_liver`: la facoltà knowledge nomina l'entità con l'articolo incollato.
   Anche la risposta buona stampa il predicato grezzo invece di una frase.

Minori: «how did you read the noun phrase oxygen» senza virgolette va allo
smalltalk; «What did you learn about the lungs?» risponde con una frase
personale («I don't have any of my own…»).

## Per il motore (passi separati)

- (2) il lettore che impara «S V O PP» deve prendere l'oggetto dal sintagma
  della IR, dove le correzioni di confine già arrivano;
- (3) il lettore di «know about X» deve togliere il determinante come fa il
  resto della IR, e realizzare i fatti in frase.
- (1) è KB: `function_question` deve raggiungere anche `about/3`.

## Il dispositivo

Su richiesta di F. i dump di `/debug` non entrano più nel transcript: ne esce
una riga sola (`# /debug: N righe…`), e con il profilo acceso restano fuori
anche le righe rientrate e le `[debug]`. Ora le risposte le scrive `say` e non
più una finestra tmux che copiava tutto l'output. Il banner di avvio entra come
riga `#`.

Lezione di conduzione: una domanda di ispezione tra la frase e la sua
correzione sposta «la frase di cui stiamo parlando». La correzione va detta
subito dopo la frase.

## Seguito, stessa notte: i reperti chiusi, e un reperto corretto

Indirizzi di F.: chiuso un muro con la conoscenza, **salire** da lì
(articolazioni più ampie), non scendere a domande più piccole; quando un
reperto è C, **fermarsi e portarlo in KB**.

- (1) chiuso in KB: `function-questions.p0`, «What does the heart do?» →
  «Heart pumps blood.» La testa del sintagma sta subito prima di «do» («fire
  blanket» resta alla sua definizione).
- (3) chiuso in KB: `belief_report/2` sulle cue «what do you know about» e «tell
  me about». Descrizione (`description_relation/1`, dichiarata), classi, fatti
  con un verbo di relazione detti in lingua. Il ramo C resta come riserva.
  Aperto: le relazioni dicibili via `extract_frame` («copper is part of bronze»)
  costavano 2,4–40 s per turno; l'accordo «Kidneys is an organ»; il numero
  atomico e il simbolo di copper, che il ramo C stampava grezzi e qui mancano.
- (2) **il reperto era sbagliato.** Non è «un lettore C che ignora la IR».
  «from» è già un chiusore in KB (`np_closer($W) :- preposition($W)`), ma il
  filtro RI-012 in `p0_np_closer` toglieva dalla vista del turno OGNI chiusore
  dopo il primo, preposizioni comprese. Dopo «absorb», «from» non chiudeva più,
  e nessuna lezione di confine poteva cambiarlo. RI-012 è ora in KB
  (`turn_verb_before/1`, grammar.p0) e vale solo per i verbi; il filtro C è
  tolto. Transcript: [live/2026-09-26-fisiologia-2.log](live/2026-09-26-fisiologia-2.log).
- Linguaggio: `append/3` come regola in procedures.p0; in parrot-p0-syntax.md
  due trappole misurate (`kb_fact` con il predicato libero scandisce tutta la
  KB; le viste di `answer_frame` arrivano anche con l'argomento libero).
