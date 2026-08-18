# 08 — Il sillogismo con la risposta giusta e nessuna ragione

> **Prompt (#7 dei cento).**
> `If all doctors are scientists and some scientists are artists, are all doctors artists?`
>
> **Esito:** «**No.**»
> **Risposta giusta:** No.

## Il problema

La risposta **è corretta**, e il documento dei cento la classifica `FALLITO`. La
classificazione è giusta, ed è il caso che spiega meglio perché la correttezza
non basta.

Il turno chiede una conclusione su un sillogismo con quantificatori misti (`tutti`
+ `alcuni`). La risposta giusta è «no» perché da *tutti i D sono S* e *alcuni S
sono A* non segue *tutti i D sono A* — il termine medio non è distribuito.

Non c'è nessuna prova che parrot0 abbia fatto quel ragionamento. Con un dominio
in cui la risposta fosse «sì», direbbe «No.» esattamente allo stesso modo.

## Perché non viene processato

1. **La forma del sillogismo non viene letta.** Le due premesse hanno
   quantificatori diversi; il lettore di regole gestisce l'universale
   (`every X is a Y` → regola) ma non l'esistenziale (`some X are Y`), che non ha
   nessuna rappresentazione.
2. **«No.» arriva da un'altra strada.** È la risposta di default a una domanda
   polare non dimostrabile — corretta per caso sotto mondo chiuso, non per
   deduzione. Sotto mondo chiuso *«non risulta»* e *«è falso»* si confondono, ed è
   esattamente la distinzione che un sillogismo mette alla prova.
3. **La risposta non porta la sua ragione.** Anche quando parrot0 ragiona davvero,
   qui non lo mostra — e senza mostrarlo non è distinguibile dal caso.

## Cosa manca

- **l'esistenziale**: `some X are Y` come forma, distinta dall'universale. Oggi
  esiste `every`, e la sua controparte no;
- **la distinzione fra «non dimostrabile» e «falso»**, che il motore ha per i
  fatti (`kb_is_conflicted`, i negativi espliciti) e non applica qui;
- **la traccia**: la risposta a un sillogismo dovrebbe portare il perché. Il
  motore ha già un `proof` (`proof_trace.p0t` lo verifica) — non viene usato qui.

## Dove sta l'autocorrezione

**Non lo vede.** Il turno risponde, e per giunta *bene*: è la classe
`wrong_answer` nella sua versione più insidiosa — una risposta **giusta per la
ragione sbagliata**, che nessuna misura di correttezza può separare da una giusta.

L'unico segnale possibile è **la traccia**: una risposta senza derivazione, a una
domanda che ne richiede una, è sospetta. È una forma di pertinenza (S3) applicata
al *processo* invece che al contenuto — e delle due è quella che parrot0 potrebbe
verificare da solo, perché la derivazione o c'è o non c'è.
