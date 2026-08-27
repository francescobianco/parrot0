# Ciò che parrot0 dice è conoscenza quanto ciò che legge

**Stato:** piano aperto, 2026-08-27
**Origine:** F., davanti a una riga come questa —

```c
snprintf(msg, sizeof msg,
         "Got it - I'll treat \"%s\" as a conjunction now, like \"and\".", word);
```

> «Io devo potergli insegnare che da adesso in poi, quando impara qualcosa,
> dovrà rispondere in questo modo se ha imparato o se ha capito.»

## 1. La misura

Sonda grezza su `src/brain/*.c` e `src/main.c` — conteggio di forme, non
un'analisi semantica, quindi i numeri vanno letti come ordine di grandezza:

| forma | conta | ha una chiave KB? |
|---|---:|---|
| `kb_response_slots(b, "chiave", slots, …)` | 92 | sì |
| `kb_say(b, "chiave", "default letterale", …)` | 68 | sì, ma con un default nel C |
| `put("frase letterale", …)` | 77 | **no** |
| `snprintf(msg, …, "frase …")` verso l'uscita | ~297 | **no** |

Circa **374 testi rivolti all'interlocutore non hanno nessuna chiave**, contro
160 che ce l'hanno. La voce di parrot0 è per due terzi compilata.

I file più densi: `10-memory-knowledge.c` (79 fra `put` e `snprintf`),
`80-code.c` (61), `90-repair-robust-abduce.c` (41),
`25-wordmath-reasoning.c` (36), `40-meta-reflection.c` (39).

## 2. Perché i mantra non l'hanno fermato

Questa è la parte che conta, perché la regola c'era già e non ha morso.

**Il mantra #2 elenca solo categorie di INPUT.** «Trigger, cue, sinonimi, unità,
verbi»: tutto ciò che parrot0 *legge*. Niente nell'elenco nomina ciò che parrot0
*dice*. I mantra sono nati dai fallimenti di gen348-349, che erano fallimenti di
comprensione — quindi guardano il lato che stava fallendo, e il lato dell'uscita
non è mai stato nominato.

**Il test operativo non si applica a una frase.** «Parrot0 può impararne un nuovo
membro domani?» ha senso per una classe con membri. Davanti a
`"Got it - I'll treat X as a conjunction now"` la domanda suona come un errore di
categoria — una frase non ha membri — quindi il controllo passa in silenzio
invece di fallire. **Un test che non si applica non protegge.**

**`kb_say(b, chiave, "default", …)` sembra già conforme.** C'è una chiave, quindi
la riga si legge come KB-first. Ma il letterale accanto è ciò che viene
effettivamente detto quando la chiave manca, e nessuno se ne accorge: la forma
produce un falso negativo per costruzione. Sessantotto righe hanno questo aspetto.

**La gerarchia di crescita parla di «forma» e tutti l'hanno letta come forma
d'ingresso.** Il primo gradino dice che se una forma non è insegnabile parlando
la KB non ha raggiunto la soglia di fertilità. Vale identico per il modo in cui
parrot0 annuncia di avere imparato — e non è mai stato applicato lì.

**Nessun mantra dice che la voce è conoscenza.** Il #11 («il formato è un vincolo
semantico») è il più vicino, ma riguarda il formato che l'utente *chiede*, non la
voce di default. Fra il #2 (l'ingresso) e il #11 (il formato richiesto) c'era un
buco, ed è dove sono passati 374 testi.

## 3. Che cosa deve diventare possibile

La prova non è che i messaggi stiano in un file `.p0`: è che si possano
**cambiare parlando**, e che il cambiamento valga dal turno dopo.

```text
> learn "Segnato: ora tratto «{word}» come una congiunzione." as a reply for conjunction_taught
> use blen as a conjunction
< Segnato: ora tratto «blen» come una congiunzione.
```

La maniglia generica esiste già dal gen441 (`learnable("a reply for",
generic_family, reply_for)`): quello che manca è che **ogni messaggio abbia una
famiglia da nominare**. Oggi 374 non ce l'hanno, quindi non c'è niente da
nominare.

## 4. Direzione

1. **Ogni testo rivolto all'interlocutore ha una chiave**, cioè una famiglia
   `response_template`. Nessuna eccezione per gli errori, per i rifiuti e per i
   messaggi meccanici: anche «quella citazione è troppo lunga» è la voce di
   parrot0.
2. **Il default letterale accanto alla chiave è un debito, non una conformità.**
   `kb_say` con default resta come rete di sicurezza per il boot, ma un default
   che non ha mai una riga KB corrispondente è un messaggio non insegnabile
   travestito da messaggio insegnabile. Vanno censiti e chiusi.
3. **Si procede per famiglie, non per righe.** Un file alla volta, partendo dai
   più densi, e ogni conversione porta con sé il suo `.p0t` che insegna una
   formulazione nuova e verifica che venga usata.
4. **La lingua viene con la famiglia.** Un `response_template/3` con la lingua
   rende ogni messaggio traducibile senza toccare il C — oggi metà dei letterali
   sono inglesi in un sistema bilingue.

## 5. Il vincolo che questo piano non deve violare

Non si sostituisce un letterale con una chiave se poi la chiave non ha una riga
KB: si otterrebbe un muro al posto di una frase. Ogni conversione è
letterale → `response_template` + chiave, nello stesso commit, con il test che
prova la formulazione insegnata a runtime.

---

# Parte II — l'audit allargato: quanto siamo indietro

**Misurato il 2026-08-27**, dopo la prima e la seconda tranche. Le sonde sono
grezze (espressioni regolari su `src/brain/*.c`, 40.918 righe di C): i numeri
sono ordini di grandezza, non un censimento riga per riga.

## A. La voce — quasi metà fatta, e il resto è la parte difficile

| | conta |
|---|---:|
| con chiave KB (`kb_say` / `kb_response_slots`) | **218** |
| `put("…")` senza chiave | 7 |
| `snprintf` di frase senza chiave | **293** |

Le due tranche del gen442 hanno svuotato il secchio dei `put` — da 77 a 7, e i
sette rimasti sono in funzioni senza `Brain` in portata o contengono virgolette
interne. **Restano 293 frasi con slot**, ed è deliberatamente la parte difficile:
`snprintf(msg, …, "… %s …", x)` non diventa una chiave da sola, perché il buco
va nominato. `kb_response_slots(b, chiave, slot, n, …)` esiste già e vuole ruoli
con un nome — `{topic}`, `{word}`, `{count}` — non `{a}` e `{b}`. Convertirle a
macchina produrrebbe famiglie insegnabili e illeggibili: si fanno a mano, per
famiglia, come dice il §4.

**Indietro: ~57% della voce** (293 su 511).

## B. Il vocabolario nel C — il debito grande

| | conta |
|---|---:|
| `cue(…, "letterale")` | **1393** |
| parole confrontate con `strcmp`/`strstr` | **1629** |

**Circa 3.000 letterali di parola dentro il motore.** È il mantra #2, ed è un
ordine di grandezza più grande del debito della voce. La riga che ha aperto
questo piano ne contiene sei da sola:

```c
"conjunction", "congiunzione", "use ", "usa ", "treat ", "tratta "
```

I file più densi: `10-memory-knowledge.c` (870 confronti),
`25-wordmath-reasoning.c` (732), `20-math.c` (266), `40-meta-reflection.c` (247),
`30-generation-reading.c` (238).

Non tutti sono uguali. Un `strcmp(tok, "@S")` confronta un simbolo del
protocollo e va benissimo; `strstr(low, "three times")` è vocabolario italiano e
inglese cablato dentro un motore aritmetico. La sonda non li distingue, quindi
il numero vero è più basso di 3.000 — ma resta nell'ordine delle migliaia, e
**nessuno di questi è insegnabile a runtime**.

## C. I nomi di predicato scritti nel C

| | distinti | occorrenze |
|---|---:|---:|
| di **protocollo** (dichiarati `machinery` in KB) | 214 | 377 |
| di **dominio** o non dichiarati | **250** | **531** |

Questa distinzione è la sola che conta, e non è pignoleria. Nominare
`input_frame_commit` o `turn_teaching_offer` è legittimo: è il protocollo aperto
fra il motore e la conoscenza, ed è documentato. Nominare `borders`,
`capital_of_country`, `planet_superlative` o `magnitude` significa che il motore
sa di che cosa si parla — cioè esattamente ciò che l'esperimento nega.

**250 predicati di dominio sono cablati nel C.** Ognuno è un pezzo di mondo che
parrot0 non può reimparare diversamente.

## D. Debito già marcato

32 `TODO(kb-first)` nei moduli, indicizzati in `docs/plans/kb-first-audit.md`.
Sono i casi che qualcuno aveva già visto e lasciato: un decimo per cento del
totale misurato qui.

## Il verdetto

Siamo indietro di **tre ordini di lavoro diversi**, e vanno affrontati in
quest'ordine, perché ciascuno rende più facile il successivo:

1. **La voce (293 frasi con slot).** È il più piccolo e il più visibile:
   riguarda ciò che l'interlocutore legge, e ogni conversione lo rende capace di
   correggere parrot0 parlando. Si fa per famiglie, con il `.p0t` che prova la
   formulazione insegnata.
2. **I 250 predicati di dominio nel C.** È il più grave rispetto alla tesi del
   progetto: finché il motore nomina `borders` e `capital_of_country`, non è un
   adattatore, è un'enciclopedia con un parser davanti.
3. **I ~3.000 letterali di parola.** È il più grande e il più meccanico. Va
   affrontato per classi (`cue` chains prima, confronti aritmetici poi), mai
   riga per riga, e ogni classe chiusa deve superare il test del mantra #2:
   *«parrot0 può impararne un nuovo membro domani, senza ricompilare?»*

Una nota sull'onestà della misura: 40.918 righe di C con ~3.000 letterali di
vocabolario e 250 predicati di dominio non descrivono un motore cieco al
dominio. Descrivono un motore che **sta diventando** cieco al dominio, con la
parte già convertita che funziona (218 messaggi con chiave, 214 predicati di
protocollo dichiarati, l'intero registro sociale insegnabile) e una coda lunga
che nessuna generazione ha ancora attaccato per intero.
