# Esperimento — quanto costa insegnare a parrot0 un problema di età e incontri

**Data:** 14 settembre 2026, 10:18–11:17 (59 minuti di orologio, misurati dai
timestamp della sessione). **Richiesta di F.:** misurare il tempo e l'impegno di
trasformazione e crescita necessari perché parrot0 risponda al meglio a questo
prompt, e lasciare tutto committato.

> Anna è più giovane di chiunque abbia incontrato Bruno prima di martedì, ma non di
> chi Bruno incontrò martedì. Carlo incontrò Anna mercoledì e Bruno giovedì. Nessuno
> incontrato da Bruno dopo Carlo è più anziano di Anna.
> Senza assumere informazioni non esplicitamente implicate dal testo, stabilisci chi è
> necessariamente più giovane di chi e indica anche quali confronti di età restano
> indeterminati.

## 1. La soluzione di riferimento (a mano, prima di toccare il codice)

Siano P = chi Bruno ha incontrato prima di martedì, T = chi ha incontrato martedì,
D = chi ha incontrato dopo l'incontro con Carlo (giovedì).

- Frase 1: Anna < ogni membro di P (strettamente più giovane); e Anna **non** è più
  giovane di chi sta in T, cioè ogni membro di T ≤ Anna (stessa età o più giovane).
- Frase 2: incontri datati (Carlo–Anna mercoledì, Carlo–Bruno giovedì). Non mettono
  nessun nome in P, T o D, e non escludono altri incontri.
- Frase 3: ogni membro di D ≤ Anna.

Necessario: Anna < P; T < P e D < P (per transitività attraverso Anna, stretta perché
un anello è stretto); T ≤ Anna e D ≤ Anna (l'età può coincidere); **Anna non è in P**
(sarebbe più giovane di sé); nessuno sta insieme in P e in T, né in P e in D.
Indeterminato: Anna–Bruno, Anna–Carlo, Bruno–Carlo; Bruno e Carlo rispetto a ciascun
gruppo; T rispetto a D. Nessun gruppo è detto non vuoto: ogni frase su un gruppo vale
per i suoi membri, se ne ha.

## 2. La base: che cosa faceva parrot0 alle 10:19

In `make chat`, il prompt come scritto (con l'a capo, quindi due turni):

```
>>> Leggo «prima» come «primo». Imparato: anna is more giovane than chiunque abbia
    incontrato bruno prime of tuesday, ma not of. Non capisco ancora.
>>> Non so ancora tradurre «senza». … Imparato: senza assumere informazioni not
    esplicitamente implicate from the testo, stabilisci chi confronti of age restano
    indeterminati.
```

Due fatti falsi scritti in KB e annunciati come appresi, nessuna risposta. Il punto di
partenza reale era: transitività dei comparativi in inglese («a is taller than b …»),
nessuna conversa «più anziano», nessun incontro, nessun quantificatore con relativa,
nessuna nozione di «necessario/indeterminato».

## 3. Dopo: che cosa risponde alle 11:17

Stesso prompt, stessi due turni in `make chat`:

```
>>> Ho letto il testo come premesse di un problema, non come fatti da imparare. Che cosa
    devo stabilire? Per esempio chi è necessariamente più giovane di chi, e quali
    confronti restano indeterminati.
>>> Dal testo segue necessariamente (una frase su un gruppo vale per i suoi membri, se ne
    ha: il testo non dice che esistano):
- Anna è più giovane di chiunque Bruno abbia incontrato prima di martedì.
- Chiunque Bruno abbia incontrato martedì è più giovane di chiunque Bruno abbia incontrato prima di martedì.
- Chiunque Bruno abbia incontrato dopo Carlo è più giovane di chiunque Bruno abbia incontrato prima di martedì.
- Chiunque Bruno abbia incontrato martedì non supera in età Anna (l'età può coincidere).
- Chiunque Bruno abbia incontrato dopo Carlo non supera in età Anna (l'età può coincidere).
- Anna non può essere fra chiunque Bruno abbia incontrato prima di martedì.
- Nessuno è insieme fra chiunque Bruno abbia incontrato martedì e fra chiunque Bruno abbia incontrato prima di martedì.
- Nessuno è insieme fra chiunque Bruno abbia incontrato dopo Carlo e fra chiunque Bruno abbia incontrato prima di martedì.
Restano indeterminati:
- Anna e Bruno.
- Anna e Carlo.
- Bruno e Carlo.
- Bruno e chiunque Bruno abbia incontrato martedì.   (… e gli altri due gruppi)
- Carlo e chiunque Bruno abbia incontrato martedì.   (… e gli altri due gruppi)
- Chiunque Bruno abbia incontrato martedì e chiunque Bruno abbia incontrato dopo Carlo.
Letture: i giorni sono della stessa settimana; incontrarsi è reciproco (chi ha incontrato
qualcuno è stato incontrato da lui); negare un confronto stretto lascia possibile la stessa età.
«Dopo Carlo» è dopo l'incontro di Bruno con Carlo (giovedì).
```

Ogni punto della soluzione di riferimento c'è; nulla è stato imparato; le tre letture
che il ragionamento usa sono dichiarate. Lo stesso vale col prompt in un turno solo.
Tempo del turno: ~1,3 s.

**Non è un frasario** (verificato nel cricchetto): con altri nomi, altri giorni e il
comparativo nel verso opposto («Marta è più anziana di chiunque abbia incontrato Luca
prima di venerdì …») la risposta è quella giusta per quel problema; lo specchio inglese
(«Anna is younger than anyone who met Bruno before Tuesday, but not than whoever Bruno
met on Tuesday …») risponde in inglese; un secondo problema nella stessa sessione
sostituisce il primo.

## 4. Cronologia misurata

| Ora | Durata | Che cosa |
|---|---|---|
| 10:18–10:19 | 1′ | base misurata in chat |
| 10:19–10:23 | 4′ | esplorazione: comparativi, canone italiano, cue del turno |
| 10:23–10:26 | 3′ | disegno; **ragionatore generale** `order-determinacy.p0` (10 verifiche verdi) |
| 10:26 | — | trovata e tolta una clausola illeggibile salvata in learned.p0 |
| 10:28–10:34 | 6′ | **bug chiesto da F. durante l'esperimento, contato qui**: `attenuated_reading` si salvava con la frase nuda, una virgola la spezzava al boot (vedi §6) |
| 10:34–10:44 | 10′ | il testo con compito non si spezza né si impara; protocollo delle risposte in parti; lettore delle premesse |
| 10:44–10:56 | 12′ | limiti del motore trovati uno per uno (§6): arietà 4, corpo 16, legami 384, modello di `findall`, legame da 512 byte |
| 10:56 | — | **prima risposta completa** (turno unico) |
| 10:56–11:04 | 8′ | resa neutra e note derivate; variante con altri nomi e verso; inglese; cricchetto (21) |
| 11:04–11:14 | 10′ | il prompt reale in `make chat` è **due turni**: premesse senza compito; crash con `retract` a metà risoluzione → problemi numerati; legami esauriti → contabili a stadi; cricchetto (28) |
| 11:14–11:17 | 3′ | costo del turno base (+0,2 s) curato con una guardia sulle cue |

Senza il bug delle virgolette: ~53 minuti. Circa un terzo del tempo è andato ai limiti
silenziosi del motore, non alla conoscenza.

## 5. L'impegno: che cosa è cresciuto

**KB (la parte che conta):**
- `kb/core/order-determinacy.p0` (71 righe) — facoltà **generale**: dati vincoli stretti
  (`lt`) e larghi (`le`) su individui e gruppi, che cosa è necessariamente sotto, che cosa
  non è sopra, quali coppie restano indeterminate, quali appartenenze sono escluse, quali
  gruppi sono disgiunti. Non sa niente di età né di incontri.
- `kb/core/problem-texts.p0` (556 righe) — riconoscere un testo con compito o solo le
  premesse; il lessico del problema per lingua (comparativi con verso sulla scala,
  quantificatori di persona, forme di «incontrare», marcatori di tempo, ordine della
  settimana); i gruppi descritti («chiunque abbia incontrato B prima di D», «chi B incontrò
  D», «incontrato da B dopo X», e gli equivalenti inglesi); i tre schemi di vincolo
  (affermato, contrastato «ma non di», «nessuno … è più X di»); gli incontri coordinati;
  la risposta in parti, in italiano e in inglese.
- `kb/core/turn-frames.p0` (4 righe) — un testo di problema non è `compound_statement`,
  `compound_inquiry` né `prose_carried`.

Un comparativo nuovo («più alto», «taller») è una riga di `comparative_word/3`; una forma
nuova di gruppo è una regola `group_at/4`; una lingua è lessico e le righe di risposta.

**C (adattatori, nessun vocabolario):**
- `src/brain/99-registry.c` +42 righe — `turn_response_part(Turn, Ordine, Testo)`: una
  risposta più lunga di un atomo (512 byte) si dà in parti ordinate che il motore mette in
  fila senza sapere che cosa dicano.
- `src/brain/10-memory-knowledge.c` — `p0_quote_text`: una frase come argomento di un fatto
  va tra virgolette (il bug di §6).

**Prove:** `tests/p0t/reasoning/order_determinacy_problem.p0t` (28 verifiche: il prompt in
un turno e in due, la variante, l'inglese, un secondo problema, «nessun Imparato»).
`make soft-test`: gli stessi due rossi di tempo di HEAD; `prose_triage` 75,
`facts_split_three` uguale a HEAD.

## 6. I limiti del motore trovati (vanno in C_TODO)

Ognuno ha fatto fallire **in silenzio** una regola corretta:

1. **`KB_MAX_BIND` 384** — una lettura composta di cinque pezzi, dove ogni pezzo è una
   regola sui token, esaurisce i legami del ramo e l'ultimo goal fallisce senza dirlo.
   Cura in KB: lettura a stadi, pezzi come fatti; contabili separati (ognuno ha la sua
   sostituzione).
2. **`retract` a metà risoluzione** compatta i fatti che un goal antenato sta scorrendo:
   core dump. Cura in KB: niente ritrattazioni, i problemi portano un numero e vale il più
   recente.
3. **`findall/3` raccoglie solo una variabile**, non un modello composto
   (`findall(e($S,$X,$Y), …)` torna vuoto). Cura: un aiutante che lega il termine intero.
4. **Una lista legata tiene 512 byte**: il `findall` di tutti i pezzi di un testo fallisce.
   Cura: finestre di 15 token.
5. **`KB_MAX_ARGS` 4** vale anche per `assert(pred, …)`: a cinque argomenti la clausola è
   scartata al caricamento con un PARSE ERROR che solo il boot mostra.
6. **Una frase salvata senza virgolette** si spezza alle virgole al boot seguente (bug
   chiesto da F.): `attenuated_reading` ora si asserisce tra virgolette, 12 righe migrate;
   ne è sparito anche il giro `concat_atoms($S, "", $K)` della scheda mix-12-04.
7. Per chi misura: in un `.p0t`, le regole `!assert` con testo variabile nelle parti non
   producono, e `!query` dopo un turno che ha risposto non è affidabile: le sonde vanno
   messe nel file KB o in un contabile che asserisce un segno.

## 7. Che cosa manca per «al meglio», e quanto costerebbe

- **Resa**: raggruppare («Bruno e Carlo restano indeterminati rispetto a ciascuno dei tre
  gruppi») invece di elencare; «fra chiunque» è pesante (meglio «fra le persone che»).
  ~30′, solo KB.
- **Appartenenze note**: se il testo mettesse un nome in un gruppo (Bruno incontrò Carlo
  lunedì), i vincoli del gruppo dovrebbero passare al nome. La facoltà ha già il posto
  (`order_edge` sui membri); manca la regola che deriva l'appartenenza dagli incontri datati
  e dall'ordine dei giorni. ~30′.
- **Dire perché** Carlo non è in nessun gruppo (l'incontro di giovedì non ce lo mette, ma
  altri incontri non sono esclusi). ~15′.
- **Copertura linguistica**: altri quantificatori («tutti quelli che», «qualcuno che»),
  altri tempi («fra martedì e giovedì»), altre dimensioni. Righe di lessico e regole
  `group_at/4`; ogni forma ~10′.
- **Motore**: i limiti 1–5 di §6 andrebbero resi visibili (un fallimento per esaurimento
  dovrebbe dirsi incompleto, non falso) — lavoro C, stimato mezza giornata, fuori da questo
  esperimento.

Stima per una versione «al meglio» della stessa famiglia di problemi: altre 1,5–2 ore di
lavoro KB, più il lavoro sul motore.
