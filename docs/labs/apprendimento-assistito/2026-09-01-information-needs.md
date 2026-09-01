# Mossa #5 + GD13 — il pezzo mancante diventa una domanda

**Data:** 2026-09-01  
**Stato:** `meta-capability-only`  
**Priorità consumata:** Mossa #5 + GD13 da `LEARN_TODO.md`  
**Fatti veri del mondo acquisiti (`W`):** 0  
**Training naturale persistito:** nessuno  
**Scopo:** rendere distinguibili e azionabili un dato mancante e un referente
mancante, senza specializzare il motore su parole, domini o lingue.

## 1. Perché questo giro non è dichiarato training

Il giro modifica il motore dichiarativo e usa `.p0t` con fixture e nonce per
provare crescita/retrazione. Secondo `LEARN_PROTOCOL.md`, quei turni sono test
software, non apprendimento reale: non hanno fonti, non producono fatti veri da
salvare e non possono aumentare `W`.

Lo stato corretto è quindi `meta-capability-only`. Il guadagno è il canale che
permetterà a un successivo giro naturale di chiedere il fatto esatto che manca;
la prova di training dovrà ancora usare fatti reali, fontati, replay, transfer,
`/save` e processo nuovo.

## 2. Baseline causale

I due arresti avevano la stessa resa generica:

```text
> dove si trova il primo?
< Non capisco ancora.

> di che colore è il libro?
< Non sono sicuro di aver seguito. Puoi dirlo in un altro modo?
```

Nel primo turno la superficie ordinale è riconoscibile ma non esiste un
antecedente in posizione 1. Nel secondo relazione ed entità sono riconoscibili,
ma non esiste alcun valore. Dire “non ho capito” in entrambi i casi perde
informazione già derivata e non suggerisce la riparazione giusta.

La mossa misurata dall'oracolo per il riferimento vuoto era: citare
l'espressione irrisolta e chiedere contesto. Per il dato assente: dichiarare che
il valore non è posseduto e chiederlo.

## 3. Ipotesi

La stessa astrazione copre i due casi:

```text
information_need(Turno, Specie, Mancante, Goal)
```

La supercomprensione utile non è soltanto una lettura più profonda del testo.
È la capacità di proiettare la lettura nello spazio dei goal e rappresentare la
differenza minima che impedisce di soddisfarli. Se quella differenza è un
oggetto logico:

1. può essere nominata senza fingere ignoranza totale;
2. può scegliere un rimedio tipato;
3. può diventare input di un piano di compensazione;
4. può essere verificata ripetendo lo stesso turno;
5. può essere sottoposta ad ablazione per misurare la causalità.

Questo collega comprensione, metacomprensione e autocrescita nello stesso ciclo:

```text
lettura -> goal -> arresto -> information_need -> azione minima -> replay
```

## 4. Implementazione KB-first

### 4.1 Superfici nel frame universale

`kb/core/turn-frames.p0` ammette ora attraverso `turn_cue_registry/2`:

- `attribute_question_cue/2`, collegata a una relazione tramite
  `domain_relation/2`;
- una vista unaria di `ordinal_reference/3`, necessaria perché il publisher
  universale non debba conoscere arità e posizione interne.

Il kernel cerca byte e pubblica evidenza; quali superfici siano attributive o
ordinali resta conoscenza KB. Il gate aggiunge, ritrae e riaggiunge forme senza
rebuild.

### 4.2 Entità nota senza valore noto

La via storica riconosceva un'entità solo se la relazione chiesta la menzionava
già in qualche fatto. Era adeguata per trovare risposte, ma rendeva logicamente
impossibile riconoscere `missing_fact`: proprio il fatto assente impediva al
frame di ammettere l'entità.

La nuova via non scandisce il mondo e non promuove un token sconosciuto. Usa
soltanto `referent_known/1`, cioè un oggetto già introdotto nello spazio del
discorso G3. Se la relazione menziona già un'entità del turno, la via storica
resta prevalente; altrimenti i referenti compatibili sono candidati. Se ne
restano più di uno, `turn_entity_unique/3` non sceglie.

È stata provata e scartata un'ipotesi più larga — cercare qualsiasi fatto sul
token nell'intera KB — perché rendeva il costo di ogni turno lineare nella KB.
La lezione generalizzabile è: una capacità compositiva deve agganciarsi
all'indice semantico già prodotto dal dialogo, non inventare un secondo censore
globale delle entità.

### 4.3 Stati, mosse e oggetti mancanti

Gli stati sono:

```text
dialogue_state(Turno, missing_fact)
dialogue_state(Turno, unresolved_reference)
```

Le policy sono KB:

```text
move_policy(missing_fact, decline)
move_policy(unresolved_reference, clarify)
missing_fact_dialogue_strategy(name_and_ask)
reference_gap_dialogue_strategy(name_and_ask)
```

La prima mossa resta un declino: parrot0 non inventa il valore. La strategia
trasforma però il declino in una richiesta fertile del dato. La seconda chiede
contesto. Ritrarre la strategia restituisce il turno al fallback senza cambiare
il binario, il riconoscimento o i fatti.

In `kb/core/arrests.p0`:

```text
information_need(T, knowledge,
                 value(Relation, Entity),
                 answer(Language, Relation, Entity))

information_need(T, reference,
                 antecedent(Surface, Order),
                 clarify(Language, Surface))
```

L'antecedente entra anche nel DAG degli arresti attraverso
`resolve_reference/3`.

### 4.4 Realizzazione come conoscenza

Le frasi non sono letterali C. `answer_content/4` compone pezzi definiti in
`kb/core/responses.p0`; italiano e inglese sono membri della stessa classe. Una
lingua futura aggiunge fatti di realizzazione, non un ramo del motore.

Esito osservato:

```text
> dove si trova il primo?
< Non sono sicuro a cosa ti riferisci con «primo». Puoi darmi più contesto?

> where is the first?
< I am not sure what you mean by «first». Could you give me more context?

> di che colore è il libro?
< Non lo so: ho capito «di che colore», ma mi manca il valore per libro. Tu lo sai?
```

### 4.5 Perché esiste `turn_priority_response/2`

`turn_response/2` è un protocollo storico condiviso da oltre cinquanta famiglie.
Sul profilo AGI, un turno con relazione presente ma lettura assente esplorava i
rami generici fino a esaurire il budget prima di raggiungere la nuova risposta.
Ogni componente della derivazione era provabile isolatamente; la regola globale
non lo era.

Il rimedio non è stato alzare il budget e non è una lista di gap nel C. È stato
aggiunto un protocollo aperto `turn_priority_response/2`: il kernel lo interroga
prima di `turn_response/2` e poi ricade invariato nel percorso storico. Stati,
policy, lingua e testo restano nella KB. Nuove mosse metacomunicative possono
entrare come regole e devono avere prove runtime di attivazione/retrazione.

## 5. Gate causale

File principale:
`tests/p0t/conversation/named_information_need.it.p0t`.

Risultato:

```text
ok    named_information_need.it.p0t — 23 passed
```

Copertura:

1. ordinale senza referente, IT e EN;
2. superficie ordinale nuova: add → uso → retract → perdita → re-add → recupero;
3. referente esistente: risposta normale e nessuna falsa richiesta di contesto;
4. attributo assente su referente noto: richiesta del valore;
5. fatto aggiunto: risposta; fatto ritratto: bisogno riaperto;
6. fatto già noto: risposta normale;
7. entità sconosciuta: non viene falsamente promossa a “dato mancante”;
8. forma interrogativa nuova: add/retract/re-add senza rebuild;
9. entrambe le strategie: retract e riattivazione senza rebuild;
10. `information_need/4` provato come termine strutturato.

Test confinanti:

```text
turn_frame_producer.p0t   16 passed
dialogue_moves.p0t         8 passed
three_axis_gap.p0t        21 passed
discourse.it.p0t           2 passed
discourse.p0t              3 passed
discourse_recall.p0t      49 passed
savemap.p0t               10 passed
```

## 6. Debito di persistenza scoperto e chiuso

Il test di recall ha rivelato due clausole nella ricaduta che il file stesso
dichiarava vuota:

```text
exchange(paris, located_in, france).
exchange_turn(9, paris).
```

Erano vere come storia di una sessione, ma false come stato di una chat nuova:
`what are we on` rispondeva `We are on paris` subito dopo il reset.

La correzione conserva l'informazione come:

```text
archived_exchange(9, paris, located_in, france).
```

e dichiara `exchange/3` e `exchange_turn/2` come `turn_scratch`. Gli utterance
restano l'archivio delle conversazioni chiuse; i predicati consumati da
`current_topic/1` restano soltanto nello stato vivo. `savemap.p0t` prova insieme
le due proprietà: l'exchange è ancora interrogabile dopo il save nello stesso
processo, ma non viene scritto né nella ricaduta né nei transcript.

## 7. Metriche del protocollo

```text
Stato                  = meta-capability-only
WorldKnowledgeGain (W) = 0
L/C/P/O                 = 0 come training naturale
X                       = 0
/save (S)               = non eseguito: nessuna lezione naturale da persistere
LessonYield             = n/a
Transfer@3              = n/a per training; gate software multi-classe verde
FreshProcessRecall      = n/a per fatti appresi; boot/reset conversazionale verde
FalseUnderstandingRate = 0 nel gate mirato
```

Le regole e i template curati del motore non vengono riclassificati come `C`
del training: sono la meta-capacità che rende possibile un futuro training.

## 8. Prossime falsificazioni

1. **GD11:** seguire la lingua del turno nella conferma di apprendimento. Non
   tradurre in C: il contenuto e la cornice devono essere realizzati dalla KB.
2. **GD12:** il possessivo deve introdurre lo stesso referente/locativo che la
   domanda successiva interroga. Senza questo, il quarto turno reale non può
   usare GD13 perché `book` non entra nello spazio del discorso corretto.
3. **G4:** consumare `information_need(reference,...)` per ellissi,
   dimostrativo+proprietà e correzione sostitutiva.
4. **Compensazione:** collegare `information_need` ad azioni `ask_user`,
   `read_source`, `resolve_reference`; replay dello stesso turno e ablazione
   devono decidere la promozione.
5. **Training reale:** dopo GD11/GD12, usare `LEARN_PROTOCOL.md` con fatti veri e
   fontati, insegnati in lingua naturale. Nessun nonce o `!assert` deve uscire
   dai test o essere contato come conoscenza.

