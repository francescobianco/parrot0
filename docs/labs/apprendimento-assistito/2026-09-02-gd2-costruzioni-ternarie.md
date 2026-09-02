# GD2 — costruzioni insegnabili oltre il binario

**Data:** 2026-09-02  
**Stato:** meta-capability-only  
**Dominio:** metalinguaggio delle costruzioni, partendo da F03 del corpus GD1  
**Obiettivo:** rendere insegnabile a voce una parafrasi con tre ruoli senza
assegnare i ruoli dall'ordine delle parole nel C  
**Budget:** un solo cluster causale; nessuna suite lunga  
**Fonti:** corpus locale
`2026-08-31-gd1-dialogue-corpus.tsv`, dialogo `gd1_012`; nessun fatto del mondo
candidato alla persistenza  
**Stop condition:** Replay, Transfer@3, tre domande di ruolo, target invertito,
catena, contrasto e retract; stop immediato davanti a una mappa di parole o ruoli
nel C

## Baseline

Probe completo, processo fresco per dialogo, nessun `/save`:

```text
dialoghi=60  turni=360  alignment_errors=0
muri=230    move_match=124
F03_context_coref: muri=24/30, move_match=6/30
```

La prima sonda ha separato due strati:

```text
sent is a ternary relation verb       -> Learned
the word to is a link word            -> Learned
Mira sent the draft to Luca           -> Learned: sent(mira, draft, luca)
Who sent the draft to Luca?           -> mira
```

Quindi lettore, relazione e interrogazione ternaria erano vivi. Il turno
successivo esponeva il limite:

```text
X ha messo Y su Z significa X put Y on Z
-> non riesco ad allineare esattamente due variabili condivise
```

Il lettore usava gia' `P0_MAX_SLOTS = 8`; soltanto il parser dell'atto didattico
aveva `vars[2]`, `seen[2]` e le due etichette cablate `@S`/`@O`.

## Meccanica promossa

L'atto didattico ora:

1. verifica che gli stessi nomi di variabile compaiano una volta per lato;
2. sostituisce le variabili, nella sola copia effimera, con identificatori che
   non possono essere scambiati per articoli o altre parole-funzione;
3. offre il target a `extract_frame/2` e a `p0_frame_bind`, gli stessi oggetti
   del lettore;
4. eredita dal frame i ruoli opachi e li riapplica alla sorgente;
5. rifiuta target assenti o ambigui e lezioni con variabili non allineate.

Il C non sa che `S` sia agente, `O` oggetto o `T` destinatario. Non contiene
`sent`, `to`, `manda`, `receives` o `dispatches`.

## Lezioni e prove

Le lezioni del gate sono lingua naturale:

```text
sent is a ternary relation verb
the word to is a link word
X manda Y a Z significa X sent Y to Z
X receives Y from Z means Z sent Y to X
X dispatches Y to Z means Z receives Y from X
```

La terza e la quarta provano inversioni diverse. La quinta usa come target una
costruzione appresa nel turno precedente: il target espone i ruoli in ordine
`T/O/S`, ma la nuova sorgente viene correttamente registrata come `S/O/T`.

Esiti:

| Gate | Esito |
|---|---:|
| Replay | 1/1 |
| Transfer@3 | 3/3 |
| domanda su agente/oggetto/destinatario | 3/3 |
| target non canonico | 1/1 |
| catena di due lezioni | 1/1 |
| contrasto (`da` invece di `a`) | 1/1 escluso |
| AblationFidelity | 1/1 |
| fatti storici conservati dopo retract | 2/2 |
| variabile mancante | 1/1 rifiutata, nessun frame parziale |
| FalseUnderstandingRate | 0/3 |

Ratchet puntuale:

```text
assisted_construction_ternary.p0t  33 passed
assisted_construction.p0t          65 passed, 1 failure preesistente
```

Il rosso compatibile rimasto riguarda soltanto la resa quotata del pivot
`denotes`; la logica binaria, l'inversione e le variabili insegnate a runtime
sono verdi. Per direzione del teacher non e' stata lanciata la suite lunga.

## Persistenza e conteggi

Nessuna lezione del gate e' stata salvata: i nomi servono a provare la capacita'
e non costituiscono un lotto di fatti del mondo.

```text
B0/R0 = 37778 / 2640
B1/R1 = 37778 / 2640 (processo nuovo, nessun salvataggio)
/save = non eseguito
S = 0
W = 0
L = 0
C = 0 clausole salvate; 1 meccanica generale ampliata nel motore
P = 0
O = 0
X = 0
FreshProcessRecall = n/a (nessuna conoscenza persistita)
```

Nuovi fatti veri del mondo salvati in KB: **0**  
Nuove clausole totali salvate e classificate: **0**

## Confine lasciato visibile

Il primo fatto e la domanda diretta di `gd1_012` sono ora raggiungibili dopo due
lezioni naturali. Il mini-dialogo completo non e' chiuso: `He reviewed it after
lunch`, il riferimento di `he`/`it`, il tempo ellittico e `Who originally sent
it?` richiedono ancora lessico relazionale, coreferenza di piu' ruoli ed ellissi
temporale. Il prossimo incremento deve chiudere una di queste coordinate come
classe e rimisurare F03, non aggiungere le quattro risposte del dialogo.
