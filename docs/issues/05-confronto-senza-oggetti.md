# 05 — Confrontare due grafi, senza avere due grafi

> **Prompt (#20 dei cento).**
> `Compare two graphs structurally.`
>
> **Esito:** «Non capisco ancora.» — **muro cieco**, uno dei soli tre su cento.

## La traccia

| prova | esito |
|---|---|
| `Compare two graphs structurally.` | muro cieco ✘ |
| `compare copper and iron` | «I don't know about **compare** yet» ✘ |
| lo stesso turno dopo un `hello` | «I don't know about compare yet» — **categoria diversa** |

Due cose in una riga sola. La seconda prova mostra che **il confronto fallisce
anche con oggetti concreti**, benché `mod_compare` esista. La terza è il reperto
già registrato altrove: **lo stesso turno cambia categoria a seconda della
conversazione**, quindi la lacuna si registra o no per ragioni che non hanno a che
fare col turno.

## Perché non viene processato

1. **Il prompt è genuinamente sottospecificato.** «Due grafi» non nomina due
   grafi. Nessuna risposta è possibile, e la mossa giusta è **chiedere quali** —
   che è un atto, non una risposta.
2. **Non esiste il ripiego «chiedi».** parrot0 sa declinare e sa rispondere; non
   sa che davanti a un compito con gli slot vuoti si domanda. È la famiglia
   *investigazione* della batteria di rinforzo, e il gradino 3 dice che lì
   **la riuscita È chiedere**.
3. **`compare` non aggancia il suo modulo** nemmeno con oggetti concreti: un
   *reachability gap* pulito, indipendente dal punto 1.
4. **La registrazione della lacuna dipende dalla storia.** In sessione nuova è un
   muro cieco (e lascia traccia); dopo un turno qualunque diventa un declino
   informato. Un registro che dipende da questo non è un registro.

## Cosa manca

- **la mossa «chiedi»**: un frame con slot dichiarati (`compare(A, B, criterio)`)
  e un consumatore che, trovandoli vuoti, produce la domanda invece del muro.
  I frame con slot esistono già in KB — manca che uno slot vuoto sia una
  *richiesta* invece che un fallimento;
- l'aggancio di `compare` al suo modulo (punto 3), che è un problema di
  superficie come [03](03-traduzione.md).

## Dove sta l'autocorrezione

Lo vede — è uno dei tre muri ciechi — ma il rimedio che proverebbe è una **cue**,
e una cue non produce una domanda. Il rimedio giusto sarebbe *dichiarare uno slot
obbligatorio*, che è la stessa forma di `role_required/1` (gen416).

È il caso in cui i due lavori si toccano: **lo schema composizionale sa già dire
«manca il pezzo X»**, e la mossa mancante è trasformare quella frase da referto
diagnostico in **domanda all'interlocutore**.
