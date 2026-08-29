# SC2-D — la modalità è un operatore, non una parola di troppo

Data: 2026-08-29 · Protocollo: [`LEARN_PROTOCOL.md`](../../../LEARN_PROTOCOL.md)
· Predecessore: [SC2-C](2026-08-29-supercomprensione-sc2c.md)

## Il misclaim che non somigliava a un errore

Baseline, processo fresco, con `shorten` insegnato come verbo di relazione:

```text
> read: … The investigators predict that a kinetic impactor can shorten an asteroid orbit.
< … 1 proposition(s) normalized in quarantine
!query claim_frame($C, shorten, kinetic_impactor_can, asteroid_orbit)     ✓ VERO
```

Il modale **non faceva fallire la lettura**. Veniva inghiottito dentro il
sintagma del soggetto, e la claim usciva come se il testo avesse detto che la
cosa succede — attribuita a un ente inesistente chiamato
`kinetic_impactor_can`. È il peggior fallimento di tutta la catena: non un muro,
non un gap, ma una proposizione che **afferma più della propria fonte** e non
somiglia a un errore. SC2-C lo aveva classificato come residuo `modal_operator`
solo perché nessun verbo di quella famiglia era insegnato; appena lo si insegna,
il residuo diventa un misclaim.

## L'incremento

Un modale porta una **forza**, e la forza è conoscenza:

```prolog
modal_force(can, possible).   modal_force(must, necessary).
modal_force(should, expected). modal_force(will, predicted).
modal_force($M, possible) :- possibility_marker($M).
modal_force($M, necessary) :- necessity_marker($M).
modal_force($M, expected)  :- expectation_marker($M).
```

L'operatore dichiarato viene **staccato prima** della fase pura di SC2-B e
conservato. Se dopo lo stacco non resta una lettura piena, il testo non ha una
lettura: ricadere sulla versione non staccata rimetterebbe dentro esattamente la
parola che cambia il significato. Il C non conosce nessun modale — chiede allo
scorer universale quale `modal_force_evidence` il testo contenga, a parola
intera, perché «can» non deve accendersi dentro «scan».

L'osservazione (`claim_modal_observation`) è irreversibile: quel testo conteneva
quel modale. La **forza** è una vista viva, quindi ritrarre la classe toglie la
modalità e lascia in piedi proposizione, superficie, fonte e span.

E la separazione che conta:

```prolog
claim_asserted_status($C, $S) :- claim_status($C, $S), naf(claim_has_modality($C)).
```

Lo status dice **chi** riporta e con quale evidenza; la forza dice **quanto**.
Le due coordinate non si sommano, e una domanda che chiede se una cosa è
accaduta non può essere soddisfatta da una claim che dice solo che potrebbe.

## Misura

```text
!query claim_frame(…, shorten, kinetic_impactor, asteroid_orbit)        ✓
!query! claim_frame(…, shorten, kinetic_impactor_can, asteroid_orbit)   ✓
!query claim_modality(…, possible)                                      ✓
!query claim_status(…, hypothesized)                                    ✓
!query! claim_asserted_status(…, hypothesized)                          ✓

> was it hypothesized that a kinetic impactor can shorten an asteroid orbit?
< Yes - that claim is reported as hypothesized, as possible (from …).
> was it observed that a kinetic impactor can shorten an asteroid orbit?
< Not as a result - that claim is reported as hypothesized, and only as possible.
```

Una domanda che nomina la **stessa forza** ha diritto a un sì; una che chiede se
è accaduto riceve la forza come risposta, non un `No` muto.

**Transfer** su modali e fonti mai usati per progettare la lezione: `may` su GMD
(`possible`), `must` su PMC5118066 (`necessary`), e — la prova che la classe è
davvero aperta — un modale **nuovo insegnato parlando**:

```text
> ought is a necessity marker
< Learned: necessity_marker(ought).
> read: … The investigators predict that a causal estimate ought survive confounding.
> was it hypothesized that a causal estimate ought survive confounding?
< Yes - that claim is reported as hypothesized, as necessary (from …).
```

Una frase, nessuna ricompilazione, e il lettore stacca un operatore che ieri non
esisteva. È il test del mantra #2 nei due sensi.

**Ablation.** Ritraendo `modal_force(can, possible)`: la modalità sparisce, la
claim torna ad asserire sotto lo status della classe, e proposizione, superficie
e fonte restano intatte. Riasserendo, torna tutto.

## Il costo della derivazione, misurato

Dopo un modale l'inglese vuole la forma nuda: «may improve» non è coperto da
`relation_verb(improved)`. La mossa KB-first è derivare la radice — e per un
verbo al presente funziona (`produces` → `produce`). Per `-ed` e `-d` **no**, e
la ragione non è semantica:

| suffissi derivati | `taught_lexicon.p0t` |
|---|---|
| `s`, `d`, `ed` | 33/2 — turno di inferenza a **1,85 s** (budget 1,00 s) |
| `s`, `ed` | 33/2 |
| `s` | **35/35** |

Non è il numero di schemi: è il costo di **ricostruirli**. La derivazione fa
ricorsione su lista di caratteri per ogni verbo e per ogni suffisso, dentro
l'enumerazione degli schemi che gira a ogni turno. Quindi oggi la forma nuda di
un verbo al passato è una **lezione**, non una derivazione, e la riga in
`grammar.p0` porta scritto il perché e la condizione per riaprirla: materializzare
le radici al momento dell'insegnamento invece di ricalcolarle a ogni lettura.

Questa è la prima volta in questa serie che un limite **di performance** decide
dove passa il confine fra derivare e insegnare. Merita di essere un'ipotesi del
piano, non una nota a piè di pagina: vedi D21.

## Una collisione osservata due volte

Due lezioni e una domanda sono state intercettate da moduli precedenti senza che
niente lo dicesse:

- `was it hypothesized that a causal **claim** must survive confounding?` →
  «I don't have the claim you mean» — un modulo diverso, agganciato alla parola
  «claim», ha preso il turno;
- `**shall** is a necessity marker` → muro, registrato come gap;
- `**known** is an irregular participle` (SC2-C) → stesso quadro.

Il ratchet usa quindi «a causal **estimate**», e la collisione resta **aperta e
scritta**, non aggirata in silenzio. Vedi D22.

## Lezioni naturali e persistenza

Il vocabolario della forza è stato preso dalla prosa **normativa**, dove è
definito invece che intuito: [RFC 2119](https://www.rfc-editor.org/rfc/rfc2119)
stabilisce che MUST/SHALL/REQUIRED sono un requisito assoluto,
SHOULD/RECOMMENDED una raccomandazione, MAY/OPTIONAL una scelta libera.

```text
required is a necessity marker      -> Learned: necessity_marker(required).
recommended is an expectation marker-> Learned: expectation_marker(recommended).
optional is a possibility marker    -> Learned: possibility_marker(optional).
detect is a relation verb           -> Learned: relation_verb(detect).
eradicate is a relation verb        -> Learned: relation_verb(eradicate).
shall is a necessity marker         -> MURO, conservato come gap
```

Cinque su sei. La sesta è un fallimento reale e conservato nel gap-registry.

```text
parrot0: routed 32 clause(s) into the KB tree
```

| Categoria | Conteggio | Contenuto |
|---|---:|---|
| `W` | 0 | — |
| `L` | 5 | 3 marker di forza (RFC 2119), 2 forme nude di verbi già persistiti |
| `C` | 0 | — |
| `P` | 5 | `fact_source(...)` |
| `O` | 22 | 5 `reading_fact`, 12 `utterance`, 5 clausole del gap-registry |
| `X` | **0** | — |

Le tre classi di forza non avevano una casa e sono state instradate a mano in
`kb/learning/taught-lexicon.p0`, dove vivono già `relation_verb` e
`irregular_participle`: è la casa della *classe grammaticale di una parola
imparata parlando*. Decisione di persistenza, non di conoscenza.

**Fresh process**, senza reinsegnare nulla: `optional` viene staccato come
possibilità su una fonte RFC, la domanda modale risponde `Yes … as possible` e
quella osservativa `Not as a result`.

## Verifica software (minima e contingente, per richiesta)

- `document_claims.p0t`: **182 passed** (SC2-A 50 + SC2-B 74 + SC2-C 32 + SC2-D 26).
- `taught_lexicon.p0t` 35/35, `retract.p0t` 17/17.
- `assisted_construction.p0t` 65/1 — il rosso è la stessa attesa stantia di SC2-C.

Il blocco SC2-C sulla modalità è stato **aggiornato, non aggirato**: quel test
asseriva un residuo `modal_operator` che SC2-D ha eliminato leggendolo. Al suo
posto resta la coordinata che non cambia — la superficie della claim è
conservata per intero comunque vada la normalizzazione.

## Metriche

| Metrica | Risultato |
|---|---:|
| LessonYield | 5/6 (la sesta è un gap classificato) |
| Modale inghiottito nel soggetto | **0** (era 1/1) |
| Transfer su modali mai usati per la lezione | 3/3 (`may`, `must`, `ought`) |
| Modale nuovo insegnato parlando | 1/1 |
| Domanda modale ↔ claim modale | 3/3 |
| Domanda osservativa su claim modale | 3/3 declinate con la forza |
| AblationFidelity | 1/1 |
| FreshProcessRecall | 2/2 |
| `WorldCommitLeak` | 0 |
| `FalseUnderstandingRate` | 0 |

## Limiti che restano

1. La forza è staccata, non **composta**: `must` dentro una subordinata o sotto
   negazione («must not») non è ancora distinto da `must`.
2. Un solo operatore per claim. «may not» e «can sometimes» restano non letti.
3. La forma nuda di un verbo al passato è una lezione (vedi il costo misurato).
4. La collisione di dispatch su parole comuni (`claim`, `shall`, `known`) è
   osservata e non risolta, e parrot0 non ha modo di dire che è successa.
5. `predicted` e `conditional` sono forze dichiarate ma senza un ratchet
   dedicato: sono seed, non capacità misurate.
