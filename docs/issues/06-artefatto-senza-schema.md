# 06 — Un artefatto senza schema verificato

> **Prompt (#79 dei cento).**
> `Write a decision record with alternatives and rejected options.`
>
> **Esito:** «I understood the request — produce "decision record with
> alternatives and rejected options" — but I don't have a verified schema for
> that artifact yet; I only synthesize what an oracle can check (a sort from a
> learned shape, arithmetic composition, a count-to-threshold game).»

## La traccia

Questo caso è diverso dagli altri otto, e va detto subito: **la risposta è
corretta**. parrot0 ha capito la richiesta, l'ha ripetuta per esteso, ha nominato
il limite, e ha perfino elencato che cosa *sa* sintetizzare. È il declino
informato fatto bene.

Il documento dei cento lo classifica `FALLITO` perché il compito non è stato
svolto — ed è giusto contarlo come lavoro non fatto. Ma non è un difetto di
comprensione.

## Perché non viene processato

1. **La dottrina anti-inganno lo vieta, e ha ragione.** parrot0 sintetizza solo
   ciò che un oracolo può verificare (`verified-codegen`, gen206). Un «decision
   record» non ha un oracolo: non c'è un test che dica se è giusto. Produrne uno
   plausibile sarebbe inventare, e un artefatto inventato che *sembra* buono è
   peggio di un rifiuto.
2. **Lo schema non esiste in KB.** Un decision record ha una struttura nota —
   contesto, decisione, alternative considerate, opzioni scartate con la ragione,
   conseguenze. È **conoscenza**, e non è dichiarata da nessuna parte.
3. **Non c'è un oracolo strutturale.** Per il codice l'oracolo è l'esecuzione; per
   un documento sarebbe la **completezza degli slot**: un decision record senza
   la sezione «opzioni scartate» è incompleto, e questo si verifica.

## Cosa manca

- lo schema come fatto: `artifact_schema(decision_record, [contesto, decisione,
  alternative, scartate, conseguenze])`;
- l'oracolo strutturale: **tutti gli slot dichiarati sono presenti e non vuoti**.
  Non dice se il contenuto è buono — dice che l'artefatto ha la forma che dichiara
  di avere, ed è verificabile.

Con quei due, la dottrina anti-inganno è **soddisfatta invece che aggirata**:
l'oracolo esiste, quindi la sintesi è ammessa.

## Dove sta l'autocorrezione

Lo vede (`gap_kind = reachability`, gen414) e il registro è `artifact` (gen415b).
Il rimedio sarebbe **dichiarare uno schema**, che è la stessa forma del rimedio
di [05](05-confronto-senza-oggetti.md) e di `role_required` (gen416): una lista di
slot obbligatori.

Il che suggerisce una cosa che vale oltre questo file: **tre dei nove casi
chiedono lo stesso rimedio** — dichiarare che cosa deve esserci — e nessuno dei
tre lo può ottenere oggi, perché il ciclo sa proporre solo cue.
