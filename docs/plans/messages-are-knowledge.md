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
