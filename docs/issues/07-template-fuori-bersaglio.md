# 07 — Il template che risponde a un'altra domanda

> **Prompt (#10 dei cento).**
> `Explain a counterexample to every swan is white.`
>
> **Esito:** un paragrafo di sei righe su come si costruisce un resoconto causale
> — «a causal account turns on the condition that starts it, the process that
> carries it and the effect one could actually measure…» — che non nomina mai un
> cigno.

## La traccia

| prova | esito |
|---|---|
| `Explain a counterexample to every swan is white.` | template causale, sei righe ✘ |
| `give me a counterexample to every swan is white` | «I don't understand that yet» ✘ |

La seconda prova isola la causa: **il template si aggancia a «explain», non al
contenuto**. Cambiato il verbo, il turno non trova più nessuno e mura. Non c'è
nessuna comprensione del controesempio in nessuna delle due.

## Perché non viene processato

1. **Un modulo generico vince senza verificare gli slot.** È la diagnosi che
   `parrot0-100-failures.md` dà in fondo a sé stesso: «quando il prompt contiene
   parole come `compare`, `cause`, `plan`, `explain`, un modulo generico spesso
   vince senza verificare gli slot e il formato richiesti». Qui `explain` chiama
   il resoconto causale, che è una **forma** disponibile e non c'entra.
2. **Nessuno verifica la pertinenza a valle.** Il modulo produce un paragrafo
   ben scritto; nessuno chiede se risponde alla domanda. È il buco S3.
3. **Il controesempio non è una capacità.** «Un controesempio a *ogni cigno è
   bianco*» è un cigno non bianco: serve saper negare un universale e cercarne un
   testimone. Non esiste.

## Cosa manca

- **la guardia sul generico** (MANTRA #14: «ogni collisione diventa una guardia
  teachable»): un modulo di forma non deve vincere se gli slot che dichiara sono
  vuoti. Qui il resoconto causale non ha né causa né effetto — e parte lo stesso;
- **la procedura del controesempio**: da `ogni X è Y` a «trova un X che non è Y».
  È una trasformazione su una regola, come [02](02-contrapposizione.md).

## Dove sta l'autocorrezione — e perché questo è il caso più grave

**Non lo vede, e non lo vedrà mai** con la macchineria attuale. Il turno produce
un paragrafo: nessun muro, nessun declino, nessuna lacuna. Il ciclo lo registra
come un turno **riuscito**.

Sono trentuno prompt su cento in questa condizione — **dieci volte i muri
ciechi** — e sono la classe più numerosa di fallimenti invisibili. Finché non
esiste una prova di pertinenza, ogni misura del bilancio dell'autocorrezione li
conta dalla parte sbagliata.
