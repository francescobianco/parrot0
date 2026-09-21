# RI-013 — che esistano numeri con un'unità si insegna

**ID, famiglia e contesto umano.** Famiglia: *il valore misurato*. Contesto
umano: in prosa tecnica ogni numero porta la sua unità — 1500 bytes, 50 hertz,
550 nanometri, 400 MPa. Un numero senza unità, lì, non è un dato.

**Commit/stato iniziale.** `4da773f7` (RI-012 chiusa), profilo `agi`, `en`.

**Stimolo congelato.**

```text
The MTU of standard Ethernet is 1500 bytes.
What is the MTU of standard Ethernet?
```

**Criterio.** «1500 bytes», con l'unità, e ritrovabile con la domanda.

**Risposta iniziale e limite osservato** (`r1-baseline.json`):

```text
> The MTU of standard Ethernet is 1500 bytes.
Learned: the_mtu_of_standard_ethernet has 1500 bytes.
> What is the MTU of standard Ethernet?
I don't know about mtu.
```

Tutto il sintagma del soggetto — articolo e preposizione compresi — incollato in
**una** entità: un cassetto che nessuno può aprire. La misura che isola la causa:

| frase | esito |
|---|---|
| The MTU of standard Ethernet is **large**. | letta ✔ |
| The MTU of standard Ethernet is **1500**. | letta ✔ |
| The MTU of standard Ethernet is **1500 bytes**. | incollata ✘ |

Rompe **l'unità**.

## ⛔ R4 — la prima cura era sbagliata, e il piano ora lo spiega

La prima cura è stata una **forma di turno nuova più un atto nuovo in C** che
prendeva «1500 bytes» e lo conservava come testo opaco. La frase passava e
l'effetto reale era zero: aveva imparato il *motore*, ricompilando.

F., vedendolo: «la missione è stata tradita dall'insegnare nuove forme di
apprendibilità; mi sembra che gli stai verificando la grammatica». Il caso è
scritto per intero nel piano, §0.5-bis, con il test da applicare **prima** di
scrivere una cura.

**La cura giusta mette la lezione al centro.** Quali parole siano unità la KB lo
dice già con `measures/2` — una relazione **aperta parlando**, che tiene ohm,
farad e watt insegnati uno alla volta. Quindi:

- `unit_word_kb/1` — un'unità è ciò che `measures/2` dichiara (più il plurale,
  che la KB sa già fare);
- `measured_value/1` — un numero seguito da un'unità è **un valore solo**;
- `decision_numeric($V) :- measured_value($V).` — la classe che la forma
  «the <ruolo> di <cosa> è <valore>» già controllava si allarga;
- `decision_eval(...) :- role($O,$R,$V), measured_value($V).` — e un valore
  misurato è il valore di se stesso: `is/2` lo pretendeva aritmetico.

**Tre righe di KB e una forma che riusa una classe esistente. Zero C.** Da qui
un'unità nuova costa **una lezione**, e tutte le frasi che la usano si leggono.

## R5 — certificazione (`r5-certification.json`): la lezione è il driver

| turno | esito |
|---|---|
| 4 — `The frequency of European mains is 50 hertz.` | **incollata** (`the_frequency_of_european_mains has 50 hertz`) |
| 5 — **la lezione**: `The hertz measures frequency.` | «Learned» |
| 6 — **la stessa frase** | «Noted: the frequency of european mains is 50 hertz.» |
| 7 — la domanda | «For european mains: the frequency is 50 hertz.» |
| 8-11 — **altro dominio** (ottica), altra unità | identico: 8 fallisce, 9 insegna `The nanometre measures length.`, 10 legge, 11 risponde «550 nanometres» |
| 12 — **ablazione**: `forget that the hertz measures frequency.` | «Forgotten» |
| 13 — la frase gemella subito dopo | **torna a incollarsi** (`the_frequency_of_japanese_mains has 60 hertz`) |
| 14 — controllo indipendente | «What is the MTU of standard Ethernet?» → «1500 bytes» |

Non è una grammatica verificata: è **una frase detta che cambia ciò che si può
leggere**, e il suo ritiro che lo riporta indietro.

Fonti: SI Brochure (hertz, nanometro), en.wikipedia.org/wiki/Byte («a unit of
digital information»), l'MTU standard di Ethernet (1500 byte), la rete europea
a 50 Hz, il verde attorno a 550 nm.

## R6 — conteggio e processo nuovo

```text
parrot0: routed 37 clause(s) into the KB tree
```

| categoria | n | clausole |
|---|---:|---|
| `W` fatti veri | **3** | `measures(byte, information)`, `measures(hertz, frequency)`, `measures(nanometre, length)` — instradati in `kb/facts/units.p0`, accanto a ohm e farad |
| `X` | 0 | — |

**Processo nuovo** (`r6-fresh-process.json`): 5/5 — le unità insegnate
persistono, quindi le frasi con quelle unità si leggono **in un processo che non
ha mai sentito la lezione**, e il replay di RI-012 regge. `make soft-test` verde
in 13 s; `facts.p0t` e `basics.p0t` verdi.

## Limiti residui — dichiarati, non aggirati

1. **Il ruolo di due parole non passa**: «the *sample rate* of CD audio»,
   «the *mains frequency* of Europe» restano incollati, perché la forma prende
   un token come ruolo. È la specie di RI-010, e **non** le ho aggiunto una
   forma apposta: sarebbe il frasario che il §0.5-bis vieta.
2. Il **valore** misurato vive nello stato della sessione, non in KB: dopo il
   salvataggio persiste l'unità (la lezione), non il dato. È giusto per un
   circuito di decisione, ma va detto.
3. Le formule continuano a usare `is/2` e su una misura falliscono: sommare
   «1500 bytes» e «2 kilobytes» richiederebbe la conversione, che non c'è.

**Stato dell'iterazione:** completa.
**Classificazione del training:** `trained` (W = 3, X = 0), e — più di `W` — una
**classe di frasi** che prima non si leggeva e ora si apre con una lezione.
