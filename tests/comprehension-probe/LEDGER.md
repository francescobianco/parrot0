# LEDGER — quanto parrot0 capiva, corsa per corsa

Una riga per corsa che conta: data, binario, numeri per famiglia, e le
risposte che hanno cambiato qualcosa. Le misure grezze (`results/*.tsv`)
non si committano; `probe.py summary` le riclassifica con il `wall()`
corrente. **`answered` non vuol dire giusta**: ogni corsa ha la sua lista di
fatti falsi o dubbi, che e' la parte che conta (mantra #7).

## 2026-09-08 — baseline, binario `0b860d96` (prima del gen506b), 154 item

Classificazione vecchia (muro = prima riga che apre con un marcatore).
f01–f07 sul binario di `0b860d96`; f08–f10 sul binario del gen506b (muro
«That turn joins…» contato come `answered`, il regex non lo conosceva).

| famiglia | answered | wall |
|---|---|---|
| f01 | 3 | 13 |
| f02 | 5 | 11 |
| f03 | 11 | 5 |
| f04 | 4 | 12 |
| f05 | 8 | 2 |
| f06 | 6 | 10 |
| f07 | 11 | 5 |
| f08 | 16 | 0 |
| f09 | 14 | 2 |
| f10 | 14 | 2 |
| **totale** | **92** | **62** |

Che cosa c'era dentro gli `answered`: sette turni di f01/f02 presi da
`smalltalk` («I don't have any of my own — I'm parrot0, an AI»), altri
quattro da un frame di analisi che ripeteva il prompt intero («On the state
of the door after each step, …, a workable design turns on…»), tre risposte
di `pragma` a turni che non erano obiezioni («Fair enough — tell me where I
went wrong»). Nei `wall`: quasi tutti «I don't know about <prima parola
opaca>» — «reserve», «assume», «mechanic» — cioe' un muro che nominava la
parola sbagliata.

## 2026-09-08 — corsa 1 del gen506c, binario delle 12:36 (compound_turn_lead, testimone, wrapper), 154 item

`probe.py summary`, classificazione nuova (muro solo se TUTTA la risposta e'
un muro; «I couldn't read «…»» e «I can't hold «…»» sono muri).

| famiglia | answered | wall | hung | transport | s max |
|---|---|---|---|---|---|
| f01 | 12 | 4 | 0 | 0 | 9.2 |
| f02 | 12 | 4 | 0 | 0 | 7.0 |
| f03 | 14 | 2 | 0 | 0 | 6.3 |
| f04 | 10 | 6 | 0 | 0 | 9.2 |
| f05 | 10 | 0 | 0 | 0 | 31.2 |
| f06 | 16 | 0 | 0 | 0 | 7.8 |
| f07 | 16 | 0 | 0 | 0 | 31.4 |
| f08 | 16 | 0 | 0 | 0 | 16.2 |
| f09 | 15 | 1 | 0 | 0 | 30.7 |
| f10 | 16 | 0 | 0 | 0 | 28.0 |
| **totale** | **137** | **17** | **0** | **0** | |

Che cosa e' cambiato davvero (le risposte, non i numeri):
- il turno composto si legge per clausole: «No mechanic here is a pilot» ->
  «Held: nothing is both mechanic here and pilot»; «Kibo is a giraffe that
  lives in the reserve» -> due fatti; «the table is in a museum» ->
  `located_in`; le domande finali murano e dicono QUALE clausola.
- nessun `smalltalk` su un turno composto; `pragma` rivendicava ancora tre
  turni di f01 (i07, i11, i14) attraverso la compensazione di superficie —
  corretto DOPO questa corsa (la lettura per clausole viene prima).
- «several pilots are sailors» non scrive piu' `sailor(several_pilots)`.
- nessun turno appeso, nessun errore di trasporto: la misura e' affidabile.

Fatti falsi o dubbi scritti in KB in questa corsa (tutti da lettori
PREESISTENTI, non dalla lettura per clausole — sono la coda):
- f04/i05 «Learned: tor is a vessel,» (virgola nel valore); f04/i07 «road
  connects the» (oggetto perso); f04/i10 «rin is a red tal».
- f05/i04 «Learned: now forget rule» («now forget the rule» letto come
  classe); f06/i09 «deadline is a hard requirement, located_in(deadline,
  client)»; f07/i07 «set c is an union».
- f01/i01 «kibo live in reserve» (verbo non concordato, fatto giusto).
- f01/i07 (prima della correzione) «if something is a pilot, then it is
  trusted» da «All calm pilots are trusted»: regola FALSA, il modificatore
  buttato — corretto nel binario successivo (regola congiunta).

## 2026-09-08 — corsa 2 del gen506c, binario laterale delle 12:40 (lettura per clausole PRIMA della compensazione, regola congiunta, involucri «is it true that», testimone a soggetto largo), 154 item

`PARROT0_BIN=obj/p0next probe.py all`, mentre la suite girava su `bin/parrot0`.

| famiglia | answered | wall | hung | transport | s max |
|---|---|---|---|---|---|
| f01 | 14 | 2 | 0 | 0 | 10.0 |
| f02 | 12 | 4 | 0 | 0 | 8.2 |
| f03 | 14 | 2 | 0 | 0 | 7.2 |
| f04 | 10 | 6 | 0 | 0 | 9.5 |
| f05 | 10 | 0 | 0 | 0 | 32.9 |
| f06 | 16 | 0 | 0 | 0 | 7.6 |
| f07 | 16 | 0 | 0 | 0 | 30.6 |
| f08 | 16 | 0 | 0 | 0 | 15.3 |
| f09 | 15 | 1 | 0 | 0 | 30.4 |
| f10 | 16 | 0 | 0 | 0 | 26.2 |
| **totale** | **139** | **15** | **0** | **0** | |

Che cosa e' cambiato: f01/i07 «All calm pilots are trusted» -> «Got it: if
something is a calm pilot, then it is trusted» (era «if something is a
pilot…», regola falsa); i07/i11/i14 non vanno piu' a `pragma`. Nuovi dubbi
visti: f01/i14 «all the chipped vases are in the attic» -> «Learned:
chipped_vases is located in attic» (l'universale con locativo letto come
individuo); «a vase from the attic is on the table» -> «vase is located in
table»; f01/i11 «how strong is that conclusion?» -> «Socrates is mortal.»
(una risposta dimostrativa a una domanda che non l'ha chiesta); «can you be
sure it is not from the shelf?» -> «What number should I use for «it»?».

## 2026-09-08 — corsa 3 del gen506c, binario di HEAD (13:31), 154 item

Aggiunte rispetto alla corsa 2: la lettura per clausole rivendica solo se ha
letto almeno una clausola e solo su una resa vera (lacuna dichiarata o muro
riconosciuto), e non lascia impronte quando declina.

**Numeri NON validi: 153 answered / 1 wall.** Letti uno per uno, gli
`answered` in piu' erano ECHI: f01/i05 rispondeva con il testo di f01/i04,
e cosi' altri. Causa (preesistente, scoperta solo perche' ora la lettura per
clausole DECLINA invece di rivendicare): in `not_understood` i rami
`schema_incomplete` e `register_declined` scrivevano il messaggio nel buffer
locale e non in `out` quando il template KB riusciva — il turno usciva con
la risposta del turno prima. Corretto dopo questa corsa (99-registry.c).
Lezione per il banco: `probe.py smoke` segnala «⚠ ECO del turno prima»
quando due risposte consecutive sono identiche; il conteggio non l'avrebbe
mai visto. Da rilanciare `all` solo a fine giornata, sul binario committato.

