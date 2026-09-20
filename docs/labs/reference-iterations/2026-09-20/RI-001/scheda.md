# RI-001 — la catena di un verbo di relazione insegnato (fiumi → mare)

**ID, famiglia e contesto umano.** Famiglia: *composizione transitiva di una
relazione insegnata, con selezione per classe nella domanda*. Contesto umano:
chi guarda una carta geografica chiede dove finisce l'acqua di un fiume che
conosce, e il fiume che conosce non è quello che arriva al mare.

**Commit/stato iniziale, profilo e hash del binario.**
`initial-state.txt` (commit `3cef7a5a`), profilo `kb/profiles/agi.p0`,
binario `b8995db70509ecd91ebcd2cbd9b468fa6d92b67ee41e8429c74b1f3788c89e47`.

**Stimolo e preambolo congelati.** Nessun preambolo; sessione nuova, lingua `en`.

```text
Which sea does the Aare flow into?
```

**Criterio di riuscita.** La risposta nomina il **mare del Nord** e lo fa
percorrendo Aare → Reno → mare del Nord: non basta che dica «Rhine» (è un
fiume, non un mare) né che ripeta la domanda. Il verso non deve invertirsi.

**Fonti verificabili** (consultate il 20 settembre 2026):

| ID | proposizione | fonte |
|---|---|---|
| F1 | The Aare flows into the Rhine | en.wikipedia.org/wiki/Aare — «becomes itself a tributary of the Rhine … downstream from Koblenz (Switzerland)» |
| F2 | The Rhine flows into the North Sea | en.wikipedia.org/wiki/Rhine — «eventually emptying into the North Sea» |
| F3 | The Moselle flows into the Rhine | en.wikipedia.org/wiki/Moselle_(river) — «a left bank tributary of the Rhine, which it joins at Koblenz» |
| F4 | The Salzach flows into the Inn | en.wikipedia.org/wiki/Salzach — «empties into the Inn in Haiming» |
| F5 | The Inn flows into the Danube | en.wikipedia.org/wiki/Salzach — «the Inn, which eventually joins the Danube» |
| F6 | The Danube flows into the Black Sea | en.wikipedia.org/wiki/Danube — «through the Danube Delta in Romania into the Black Sea» |

**Risposta iniziale e limite osservato** (`r1-baseline.json`):

| turno | risposta |
|---|---|
| Which sea does the Aare flow into? | «Hmm, I don't know about aare flow yet. Want me to learn about it?» |
| Which sea does the Moselle flow into? | idem su «moselle flow» |
| Which sea does the Salzach flow into? | idem su «salzach flow» |
| Does the Danube flow into the North Sea? | «I don't know: nothing I hold says …» (onesto) |
| Does the Rhine flow into the Aare? | «I don't know: nothing I hold says …» (onesto) |
| **controllo affine già verde**: What is the capital of Germany? | «Berlin.» |

Il difetto **non è** la mancanza del solo dato: manca il verbo di relazione
(`flow into` non apre nemmeno il fatto binario), e con esso ogni possibilità di
comporre due fatti. Il controllo mostra che il recupero a un salto funziona:
ciò che non esiste è la composizione su una relazione che nessuno ha ancora
nominato.

**Curriculum iniziale — scritto prima della cura.**

| # | lezione | che cosa aggiunge | perché serve | effetto intermedio da controllare |
|---|---|---|---|---|
| L1 | `flows into is a relation verb` | apre `X flows into Y` | senza il verbo non esiste nemmeno il fatto | «Does the Aare flow into the Rhine?» dopo L2 |
| L2 | `The Aare flows into the Rhine.` | F1 | primo anello | la polare su F1 |
| L3 | `The Rhine flows into the North Sea.` | F2 | secondo anello | la polare su F2 |
| L4 | `The North Sea is a sea.` | classe del valore | «which **sea**» deve poter scegliere | «Is the North Sea a sea?» |
| L5 | `flows into chains` | transitività | è l'unica cosa che compone i due anelli | lo stimolo |

Dipendenze: L1 → L2, L3; L4 indipendente; L5 dopo L2 e L3.
Budget dichiarato: 15 turni, 10 minuti, nessuna coppia domanda-risposta
insegnata, nessuna risposta dei transfer pronunciata nelle lezioni.

**Trasferimenti fissati prima (non insegnati).**

- **TR1 — altro membro, stessa forma**: insegnato il solo fatto F3, chiedere
  «Which sea does the Moselle flow into?». La regola non si ripete.
- **TR2 — altra catena, un anello in più, altro mare**: insegnati F4, F5, F6 e
  «The Black Sea is a sea», chiedere «Which sea does the Salzach flow into?»
  (tre salti: Salzach → Inn → Danubio → mar Nero).
- **Contrasto C1**: «Does the Danube flow into the North Sea?» non deve
  diventare vero (le due catene non si mescolano).
- **Contrasto C2**: «Does the Rhine flow into the Aare?» non deve diventare
  vero (la transitività non è simmetria).

---

## R3 — tentativo di addestramento: dove si è rotto

Il curriculum, impartito alla lettera, è fallito alla **prima lezione**:

| turno | risposta | lettura |
|---|---|---|
| `flow into is a relation verb` | «I already know that flow is a relation verb.» | perdita silenziosa: l'ultima istanza legge la sola prima parola |
| `The Aare flows into the Rhine.` | «I don't understand that yet.» | senza la particella la frase non si legge |
| `The North Sea is a sea.` | «Learned…» | la lezione di classe funziona |
| `Which sea does the Aare flow into?` (un solo mare in KB) | «north sea.» | **sembrava** riuscita: era l'elencatore che rendeva l'unico mare |

Confine di arbitraggio cercato con la forma nativa affine: `flow is a relation
verb` + `The Aare flows the Rhine.` **funziona**. Quindi non manca la superficie
del verbo di relazione: manca quella del verbo **con particella**. E 265 righe
`verb_particle/2` erano scritte a mano — il test del mantra era rosso.

Con due catene in KB la falsa riuscita si è scoperta da sé:

```text
Which sea does the Aare flow into?   ->  north sea, black sea.
Which sea does the Inn flow into?    ->  north sea, black sea.
```

## R4 — diagnosi e cura (tre siti, nessun vocabolario nel C)

1. **La lezione non esisteva.** `turn_form`/`turn_form_act` è già la lingua in cui
   si dichiara una forma di turno: mancava la forma. Aggiunta `teach_verb_particle`
   (`messages.p0`) che scrive `relation_verb(radice)` **e** `verb_particle(radice,
   particella)`. La particella è legata dal pezzo `named`, che tiene solo dove la
   KB la riconosce: la classe `verb_particle_word/1` è **derivata** dalle
   particelle già in uso (`grammar.p0`), quindi non è una lista nel C né in KB.
2. **La catena si percorreva solo in verifica.** `transitive_relation/1` si
   insegna, ma la percorreva `p0_relation_transitive` con i due estremi già in
   mano: chi *chiede* non ha il secondo estremo. Aggiunta la clausola (a-bis) di
   `holds/3` in `procedures.p0`, con la guardia della transitività davanti.
   Da lì ogni consumatore di `holds/3` ha la catena, non solo questa domanda.
3. **La domanda con la classe tornava all'elencatore.** Nuovo atto
   `answer_relation_in_class` (`10-memory-knowledge.c`): la classe è un **filtro
   sul valore**, i valori si leggono da `holds/3`, e — decisivo — la forma che
   ha riconosciuto la domanda **rivendica il turno anche quando non sa**, con il
   muro dichiarato in KB (`turn_form_empty_reply` → `no_value_in_class`).
   Senza quest'ultimo pezzo il turno tornava a chi elenca la classe, che
   rispondeva «north sea, black sea» a una domanda su un fiume mai sentito.
4. **Il ritiro non era pronunciabile.** `V chains` si insegnava e non si
   disdiceva: aggiunte `untransitive_taught`/`untransitive_taught2`
   («forget that flow chains»), senza le quali l'ablazione di R5 è impossibile.

Controllo software: `make soft-test` verde in 9-10 s (budget 15 s). I rossi di
`enumerate.p0t` (5/12), `faceted_enumeration.p0t` (2/8), `answerframe.p0t`
(22/3), `abduce_chain.p0t` (3/15) e `magnitude_compare.p0t` (1/5) sono stati
**misurati identici sull'albero pulito** (stash, ricompilazione, stesse righe):
sono preesistenti, non di questa iterazione.

## R5 — certificazione sul meccanismo fermo

| stato | esito | prova |
|---|---|---|
| motore e KB iniziali | stimolo fallito | `r1-baseline.json` |
| meccanismo riparato, **senza** le lezioni | «I don't know: nothing I hold gives aare a sea there.» — capacità non acquisita, e muro onesto | `r5a-mechanism-no-lessons.json` |
| dopo le lezioni | **«north sea.»** | `r5b-certification.json` t6 |
| TR1 — Mosella (membro nuovo, regola non ripetuta) | **«north sea.»** | t8 |
| TR2 — Salzach (tre salti, altra catena, altro mare) | **«black sea.»** | t13 |
| contrasto — Tevere (soggetto senza relazione) | muro onesto, **non** l'elenco dei mari | t14 |
| contrasto — `Does the Danube flow into the North Sea?` | «I don't know…», non «Yes» | t15 |
| ablazione — `forget that flow chains` | «Forgotten: flow no longer chains.» → lo stimolo **perde** la risposta | t16-17 |
| controllo indipendente sotto ablazione | `What does the Aare flow into?` → **«Rhine.»** (resta) | t18 |
| reinsegnamento | lo stimolo e il transfer tornano | t19-21 |

## R6 — salvataggio, conteggio e processo nuovo

Pre-save su sessione di sole lezioni (`/session`, dump ispezionato): nessun
candidato `X`. **Trovato e non salvato** un difetto vero: la sessione che
contiene anche le domande produce `answer_frame("which sea does", search)`,
`… research`, `… searchd_for` — clausole false fabbricate da una domanda
murata. Con quelle in sessione il cancello di `LEARN_PROTOCOL` §8 vieta il
`/save`: è il candidato di un'iterazione successiva.

```text
parrot0: routed 63 clause(s) into the KB tree
```

| categoria | n | clausole |
|---|---:|---|
| `W` fatti veri del mondo | **8** | `flow/2` ×6 (Aare, Reno, Mosella, Salzach, Inn, Danubio), `sea/1` ×2 |
| `L` linguistici/metalinguistici | 3 | `relation_verb(flow)`, `verb_particle(flow, into)`, `class_surface(sea, sea)` |
| `C` costruzioni/regole | 1 | `transitive_relation(flow)` |
| `P` provenienza | 30 | `fact_source` ×8, `reading_fact` ×8, `fact_mention` ×14 |
| `O` altre spiegate | 20 | `utterance` (il registro del dialogo) |
| `X` invalide | **0** | — |

```text
Nuovi fatti veri del mondo salvati in KB: W = 8
Nuove clausole totali salvate e classificate: W + L + C + P + O = 62
Clausole dichiarate da /save: S = 63
Clausole invalide: X = 0
```

La differenza 63 − 62 è `machinery(policy)`, instradata e già presente in
`kb/core/intents.p0:4134`: un duplicato, non una clausola aggiunta.

**Processo nuovo** (`r6-fresh-process.json`), nessuna lezione ripetuta, otto
domande con formulazioni diverse: 8/8. `FreshProcessRecall = 100%`.
Compresi i due controlli estranei (capitale della Germania, perché lievita il
pane), che rispondono come prima.

## Capacità guadagnata, limiti residui, prossimo problema

**Guadagnata.** Un verbo di relazione con particella si insegna parlando, la
sua catena si percorre anche quando non si sa dove arriva, e una domanda che
nomina una classe usa quella classe come filtro invece di rispondere con il suo
elenco. La prova che è generale e non un caso: la Salzach, mai nominata nelle
lezioni di regola, arriva al mar Nero in tre salti.

**Limiti residui, misurati:**

1. **Il «Yes» invertito.** `Does the Rhine flow into the Aare?` → «Yes.»
   `p0_polar_reply` prova **ogni coppia ordinata** dei token del turno
   («lo decide il fatto, non una posizione»): per una relazione che ha un verso
   è falso. Preesistente — si riproduce anche senza particella — e verificato sul
   albero pulito. È il candidato di RI-002.
2. **Soggetto di più parole.** `Which sea does the North Sea flow into?` →
   «north sea, black sea»: `slot(subject)` prende **un** token, la forma non
   combacia e l'elencatore riprende il turno. Specie nota (accesso multiparola).
3. **Una particella mai vista non si insegna** (`onto is a verb particle` cade
   nel muro): la superficie si risolve in `verb_particle/2`, arietà diversa.
   Il test del mantra passa per il verbo, non per la particella.
4. Le clausole `answer_frame` fabbricate dal muro di una domanda (sopra).

**Stato dell'iterazione:** completa.
**Classificazione del training:** `trained` (W = 8, L = 3, C = 1, X = 0).
