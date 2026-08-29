# SC1 — unita' documentali e retorica insegnabile

Data: 2026-08-29

Protocollo: [`LEARN_PROTOCOL.md`](../../../LEARN_PROTOCOL.md)

Piani: [`apprendimento-assistito.md`](../../plans/apprendimento-assistito.md),
[`frontier-kb-natural-dialogue.md`](../../plans/frontier-kb-natural-dialogue.md)

## Parametri

| Parametro | Valore |
|---|---|
| `DOMINIO` | struttura retorica di prosa scientifica breve |
| `OBIETTIVO` | conservare due unita' ordinate con span fonte e derivare un arco di contrasto da una cue insegnata a voce |
| `BUDGET` | una lezione linguistica, replay, tre transfer, un contrasto, ablation, reteach e retention |
| `FONTI` | NASA, USGS, PubMed Central, Los Alamos National Laboratory, Smithsonian |
| `TARGET_WORLD_FACTS` | 0; il ciclo mira a una meta-capacita' |
| `TARGET_CAPABILITIES` | 1: membro nuovo di una classe retorica consumato dal Document IR |
| `STOP_CONDITION` | primo misclaim, fatto non fontato o contaminazione del save con identita' documentali process-local |

Boot della sessione verificata: `36307 facts, 2418 rules`, profilo
`kb/profiles/agi.p0`, nessun parse error.

## Fonti e proposizioni vere

| ID | Proposizione usata | Fonte | Scope |
|---|---|---|---|
| F1 | Apollo 13 non raggiunse gli obiettivi scientifici, ma la missione di soccorso fu un successo dell'agenzia | [NASA, *The Hard-won Triumph of the Apollo 13 Mission*](https://www.nasa.gov/missions/apollo/the-hard-won-triumph-of-the-apollo-13-mission-45-years-later/) | parafrasi di due claim della pagina |
| F2 | le zone di subduzione occupano una porzione ridotta della superficie ma producono i terremoti piu' grandi | [USGS, *Introduction to Subduction Zones*](https://www.usgs.gov/special-topics/subduction-zone-science/science/introduction-subduction-zones-amazing-events) | parafrasi istituzionale |
| F3 | uno studio osservazionale puo' mostrare associazioni ma non, da solo, stabilire causalita' | [PubMed Central, *Observational Studies*](https://pmc.ncbi.nlm.nih.gov/articles/PMC10589119/) | limite del disegno osservazionale |
| F4 | il mercurio e' un metallo liquido alle temperature ordinarie | [Los Alamos National Laboratory, *Mercury*](https://periodic.lanl.gov/80.shtml) | proprieta' dell'elemento |
| F5 | l'ornitorinco e' un mammifero che depone uova | [Smithsonian National Zoo](https://nationalzoo.si.edu/news/study-diversity-among-species-helps-national-zoo-scientists-breed-endangered-species) | transfer dopo reteach |

`nevertheless` e' stato insegnato come membro vero della classe linguistica
dei connettivi contrastivi. Nessuna risposta attesa o proposizione di test e'
stata aggiunta a mano alla KB.

## Baseline e diagnosi

Baseline:

```text
you> read: Apollo 13 did not achieve its scientific goals. Nevertheless,
     NASA's rescue mission was an agency triumph.
parrot0> Learned 0 fact(s), skipped 2.
```

Le due clausole venivano attraversate e subito perse. Una prima sonda ha
stabilito che la classe era gia' insegnabile:

```text
you> "albeit" is a contrastive connector.
parrot0> Learned: contrastive_connector(albeit).
```

Il replay restava pero' `0/1`: nel dump sopravvivevano la lezione e la sua
provenienza, non `current_prose`. Il gap era quindi il consumer/memoria
documentale, non il lessico.

Durante lo sviluppo sono state abortite due sessioni senza `/save`. In entrambe
la forma naturale di retract veniva riletta come asserzione:

```text
you> forget that albeit is a contrastive connector
parrot0> Learned: contrastive_connector(forget).
```

e, dopo il primo fix limitato alle virgolette:

```text
you> forget that nevertheless is a contrastive connector
parrot0> Learned: contrastive_connector(forget).
```

Entrambe sono `X=1`, diagnostiche e non promosse. La correzione finale fa
condividere al retract lo stesso parser puro dell'atto di menzione e aggiunge la
forma generale soggetto/copula/classe multi-parola. Copule, determinanti e
classi restano conoscenza KB.

## Incremento implementato

Il nuovo `kb/core/document-structure.p0` separa quattro livelli:

1. il C assegna soltanto identita' monotona, ordine e byte span;
2. `document_unit_observe/4` copia in sessione superficie e token dalla
   gerarchia transiente `current_prose` prima del clear;
3. `rhetorical_marker_class/3` lega una classe aperta a relazione e direzione;
4. `rhetorical_edge/4` resta una vista derivata. Retrarre la cue elimina
   l'arco anche dai documenti gia' osservati, senza cancellare unita' o fatti.

La regola usa `apply/2`: il consumer non nomina le superfici. La riga iniziale
e' oggi:

```prolog
rhetorical_marker_class(contrastive_connector, contrast,
                         previous_to_current).
```

Il token deve essere iniziale nell'unita'. Questa e' una guardia di precisione,
non la dichiarazione che ogni contrasto abbia quella forma: intra-frase e cue
multi-parola restano esplicitamente aperti.

## Lezione, replay e transfer

Lezione naturale:

```text
you> "nevertheless" is a contrastive connector.
parrot0> Learned: contrastive_connector(nevertheless).
you> Is nevertheless a contrastive connector?
parrot0> Yes.
```

Replay:

```text
parrot0> Mapped 2 document unit(s) and 1 rhetorical relation(s);
         learned 0 fact(s), skipped 2.
```

Transfer:

| Caso | Esito delle unita'/arco | Fatti contenuto |
|---|---|---|
| USGS, subduzione | 2 unita', 1 contrasto | 0/2 |
| studio osservazionale, senza virgola dopo la cue | 2 unita', 1 contrasto | 0/2 |
| mercurio | 2 unita', 1 contrasto | 1/2: `metal(mercury)` |

`Transfer@3 = 3/3`. La forma con e senza virgola passa dallo stesso token e
vale come prima invarianza di punteggiatura, non come una seconda cue imparata.

Il quasi-esempio con `therefore` ha prodotto due unita' e nessun arco di
contrasto. Il fatto `metal(mercury)` e l'arco hanno inoltre convissuto nello
stesso documento: la relazione retorica non sostituisce l'estrazione del
contenuto.

## Ablation, reteach e retention

```text
you> forget that nevertheless is a contrastive connector
parrot0> I no longer treat «nevertheless» as a contrastive connector.
you> Is nevertheless a contrastive connector?
parrot0> No.
```

Dopo il retract:

- `rhetorical_edge` non era piu' dimostrabile neppure sul vecchio documento;
- entrambe le `document_unit` e i loro span restavano presenti;
- `metal(mercury)` restava vero e interrogabile;
- non esisteva `contrastive_connector(forget)`;
- rileggere con la stessa superficie tornava al sommario senza `Mapped`.

La stessa lezione e' stata pronunciata di nuovo. Il transfer Smithsonian
sull'ornitorinco ha ricostruito `2 unita' / 1 relazione`, e dopo cinque turni
pertinenti `Is nevertheless a contrastive connector?` rispondeva ancora `Yes`.

## Persistenza pulita

Le sessioni documentali non sono state salvate: `document_N` e' ancora un ID
monotono locale al processo. Persistirlo prima di avere un'identita' ancorata a
fonte/versione provocherebbe collisioni fra processi e falsa genealogia.

Una sessione nuova ha quindi ripetuto **soltanto** la lezione linguistica vera e
ha eseguito `/save` prima di leggere documenti:

```text
parrot0: routed 5 clause(s) into the KB tree
```

Diff semantico:

| Categoria | Conteggio | Clausole |
|---|---:|---|
| `W` fatti veri sul mondo | 0 | — |
| `L` fatti linguistici | 1 | `contrastive_connector(nevertheless)` |
| `C` costruzioni/procedure apprese | 0 | — |
| `P` provenienza | 1 | `fact_source(...)` |
| `O` tracce di ordine superiore | 3 | `reading_fact(...)`, due `utterance(...)` |
| `X` invalide | **0** | — |

Le tracce non sono state filtrate: seguono il contratto aperto di
`docs/session-and-provenance.md`. In particolare il loro salvataggio rende
visibile che `reading_fact`, pur descritto come traccia per-read, oggi viene
promosso; e' un residuo di M14 da classificare, non qualcosa da cancellare in
silenzio.

```text
Nuovi fatti veri del mondo salvati in KB: 0
Nuove clausole totali salvate e classificate: 5
Clausole dichiarate da /save: 5
Clausole invalide: 0
```

Processo nuovo: `36312 facts, 2418 rules`. Senza ripetere la lezione:

```text
Is nevertheless a contrastive connector? -> Yes.
read: <Apollo 13> -> 2 document units, 1 rhetorical relation.
```

`FreshProcessRecall = 2/2 = 100%`; `B1-B0 = 5`, coerente con il diff e con
`S=5`.

## Metriche

| Metrica | Risultato |
|---|---:|
| LessonYield | 1/1 nel run promosso |
| Replay | pass |
| Transfer@3 | 3/3 |
| Paraphrase/punteggiatura | 2/2 |
| ContrastPrecision | 1/1 |
| Composition | 1/1 |
| AblationFidelity | 1/1 |
| Retention | 1/1 |
| FreshProcessRecall | 2/2 |
| Relation fidelity | 4/4 su replay + transfer |
| Claim coverage | 1/8 sul nucleo replay + transfer |
| FalseUnderstandingRate finale | 0 |

Il ciclo completo conserva inoltre i due misclaim diagnostici sopra: non sono
contati nel run promosso, ma non vengono nascosti dalla metrica finale.

## Verifica software

- `make build`: verde;
- `document_rhetoric.p0t`: **33 passed** in un solo processo;
- `mention.p0t`: **24 passed**;
- `taught_segment_role.p0t`: **21 passed**;
- `retract.p0t`: **17 passed** su engine ermetico senza profilo.

Il solo `make soft-test` del ciclo e' stato eseguito una volta: **55 passati,
1 fallito**. Il fallimento e' il rosso preesistente di
`frontier_chat_audit.it.p0t` riga 97:

```text
expected: I don't know about designation
got: I don't know much about your designation yet. Want me to look it up?
```

Non e' stato rilanciato.

## Stato e prossimo confine

Stato: **meta-capability-only, SC1-A chiusa**. Il nucleo causale e' provato:
unita' stabili nella sessione, relazione derivata da classe insegnabile,
transfer, contrasto, ablation, reteach e persistenza della lezione.

SC1 non autorizza ancora a dire che parrot0 comprende i claim: `Claim coverage
= 1/8`. Restano inoltre:

- ID dei documenti process-local e quindi non persistibili;
- soltanto relazioni fra unita' adiacenti con cue iniziale;
- nessuna cue multi-parola o arco intra-periodo;
- nessun `unit_act` per metodo, risultato, limite o definizione;
- nessuna identita' stabile di claim a cui SC2 possa assegnare forza epistemica.

Il prossimo incremento e' SC2, ma deve cominciare risolvendo l'identita' fonte
del documento e distinguendo **claim** da **frase** e da **commitment**. Un testo
che dice «gli autori ipotizzano X» non deve mai promuovere X a osservazione.
